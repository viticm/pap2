#include "StdAfx.h"
#include "KG3DRepresentObjectSoundBall.h"
#include "KG3DEngineManager.h"
#include "..\..\..\Include\IKG3DSoundShell.h"
#include "KG3DGraphicsTool.h"
#include "dsound.h"
#include "KG3DSceneOutDoorSpaceMgr.h"

KG3DSoundBallDataTable g_cSoundBallTable;
extern BOOL g_bRenderSoundBall;

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//播放失败的重新检测时间
static DWORD s_dwPlayFailedRecheckTimeSpan = 5000;
//BOOL g_b3DSound = true;
SoundBallConfigInfo::SoundBallConfigInfo()
{
	m_f3DSoundMinDistance = 1.f;
	m_fMaxListenRange = 5000.0f;
	m_fSoundBallCheckRange = m_fMaxListenRange;
	m_fBoundingBoxSize = 300.0f;
	m_bRenderSoundBallRange = false;
	m_crSoundBallColor = D3DXCOLOR(0.0f, 1.0f, 0.0f, 0.5f);
	m_crSoundBallRangeColor = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f);
}

SoundBallConfigInfo g_SoundBallConfig;

static const TCHAR *s_strHeaderSectionName = "MAIN";
static const TCHAR *s_strDataSectionPrefix = "Sound";
KG3DSoundBallRender g_cSoundBallRender;


KG3DRepresentObjectSoundBall::KG3DRepresentObjectSoundBall(void)
{
	m_lpSoundBallData = NULL;
	m_uFileHash = 0;
	m_nCurrentSound = 0;
	m_dwSoundID = 0;
	m_dwLastCheckTime = 0;
	m_bHalt = FALSE;
	m_bRenderRange = FALSE;
	m_fVolume = 1.0f;
	m_bPlayFailed = TRUE;
}

HRESULT KG3DRepresentObjectSoundBall::UnInit()
{
	KG3DRepresentObject::UnInit();
	StopSound();
	m_bPlayFailed = TRUE;
	m_dwLastCheckTime = 0;
	return S_OK;
}

KG3DRepresentObjectSoundBall::~KG3DRepresentObjectSoundBall(void)
{
	g_cSoundBallTable.Release(m_uFileHash);
	m_lpSoundBallData = NULL;
	m_uFileHash = 0;
}

void KG3DSoundBallDataTable::UnInit()
{
	map<UINT,SoundBallData*>::iterator i = m_mapSoundBallData.begin();
	map<UINT,SoundBallData*>::iterator iend = m_mapSoundBallData.end();
	while (i != iend)
	{
#ifdef _DEBUG
		KGLogPrintf(KGLOG_WARNING , "Sound ball %s is not released.\n", i->second->strFileName);
#endif 
		SAFE_DELETE(i->second);
		i++;
	}
	m_mapSoundBallData.clear();
}

ULONG KG3DRepresentObjectSoundBall::AddRef()
{
	return KG3DRepresentObject::AddRef();
}

ULONG KG3DRepresentObjectSoundBall::Release()
{
	return KG3DRepresentObject::Release();
}

HRESULT KG3DSoundBallDataTable::LoadSoundBallData(SoundBallData** ppData,
											  UINT& uHashID,
											  LPCTSTR strFileName)
{
	HRESULT hRetCode = E_FAIL;
	HRESULT hResult = E_FAIL;
	UINT uFileHash = 0;
	SoundBallData *pData = NULL;
	uHashID = 0;

	KG_PROCESS_ERROR(strFileName);
	
	uFileHash = g_FileNameHash(strFileName);

	hResult = FindSoundBallData(ppData, uFileHash);
	if (FAILED(hResult))
	{
		pData = new SoundBallData;
		KG_PROCESS_ERROR(pData);

		hResult = LoadFromFile(strFileName, pData);
		KG_COM_PROCESS_ERROR(hResult);

		m_mapSoundBallData[uFileHash] = pData;
		*ppData = pData;
	}
	(*ppData)->nRef++;

	uHashID = uFileHash;
	hRetCode = S_OK;
Exit0:
	if (FAILED(hRetCode))
	{
		SAFE_DELETE(pData);
	}
	return hRetCode;
}

void KG3DSoundBallDataTable::Release(UINT uID)
{
	map<UINT,SoundBallData*>::iterator itFind = m_mapSoundBallData.find(uID);
	if (itFind != m_mapSoundBallData.end())
	{
		m_mapSoundBallData[uID]->nRef--;
		if (m_mapSoundBallData[uID]->nRef <= 0)
		{
			SAFE_DELETE(itFind->second);
			m_mapSoundBallData.erase(itFind);
		}
	}
}

HRESULT KG3DSoundBallDataTable::FindSoundBallData(SoundBallData** ppData,
											 UINT uHashID)
{
	map<UINT,SoundBallData*>::iterator i = m_mapSoundBallData.find(uHashID);
	if(i != m_mapSoundBallData.end())
	{
		(*ppData) = i->second;
		return S_OK;
	}
	return E_FAIL;
}

HRESULT KG3DRepresentObjectSoundBall::LoadFromSoundBallDataFile(LPCTSTR strFileName)
{
	HRESULT hResult = E_FAIL;
	HRESULT hRetCode = E_FAIL;
	
	g_cSoundBallTable.Release(m_uFileHash);

	hResult = g_cSoundBallTable.LoadSoundBallData(&m_lpSoundBallData, 
		m_uFileHash,
		strFileName);
	KG_COM_PROCESS_ERROR(hResult);

	m_szSoundBallDataFileName = strFileName;
	hRetCode = S_OK;
Exit0:
	return hRetCode;
}

static float s_fVolumeRange = 3000.0f;

HRESULT KG3DRepresentObjectSoundBall::FrameMove(const D3DXMATRIX& matView)
{
	HRESULT hResult = E_FAIL;
	HRESULT hRetCode = E_FAIL;

	D3DXVECTOR3 vTransPosition;
	D3DXVECTOR3 vSpeed(.0f, .0f, .0f);
	IKG3DSoundShell *pSound = g_cEngineManager.GetSound();

	KG_PROCESS_SUCCESS(m_bHalt);
	KG_PROCESS_ERROR(m_lpSoundBallData);
	KG_PROCESS_SUCCESS(!pSound);

	D3DXVec3TransformCoord(&vTransPosition, &m_Translation, &matView);
	float fDis = D3DXVec3Length(&vTransPosition);
	float fRadius = m_Scaling.y * m_lpSoundBallData->fRadius;
	float fCheckRange = fRadius + g_SoundBallConfig.m_fMaxListenRange;
	if(fDis <= fCheckRange)
	{
		//在CheckRange到fRadius之间直接从0.0 - 1.0f衰减音量， 以达到声音的平滑出现和消失
		//在fRadius范围内的时候， 音量恒定为1.0f， 声音大小的衰减由D3DSound控制
		float fVolume = min(1.0f, max(0.0f, g_SoundBallConfig.m_fMaxListenRange - (fDis - fRadius)) / g_SoundBallConfig.m_fMaxListenRange );
		//在范围内，可以听到
		if (m_lpSoundBallData->bLoop)
		{
			//如果是连续循环播放的
			DWORD dwCurrentTime = timeGetTime();
			//循环播放的加上了播放失败的处理， 设置失败之后再次尝试播放的间隔
			if (m_bPlayFailed && (dwCurrentTime - m_dwLastCheckTime) > s_dwPlayFailedRecheckTimeSpan)
			{
				//如果还没有声音， 需要播放；如果有了不用做任何处理
				if (!m_dwSoundID || !pSound->IsPlaying3DSound(m_dwSoundID))
				{
					m_dwLastCheckTime = dwCurrentTime;
					hResult = pSound->Play3DSound(SCENE_SOUND,
						m_lpSoundBallData->strFileName,
						fVolume,
						TRUE,
						&(0.01f * m_Translation),
						&vSpeed,
						fRadius * g_SoundBallConfig.m_f3DSoundMinDistance * 0.01f,
						DS3D_DEFAULTMAXDISTANCE,
						&m_dwSoundID);
					KG_COM_PROCESS_ERROR(hResult);
					m_bPlayFailed = FALSE;
					m_fVolume = fVolume;
				}
			}
		}
		else
		{
			//有几率检测
			DWORD dwCurTime = timeGetTime();
			DWORD dwTimeSpan = dwCurTime - m_dwLastCheckTime;
			if (dwTimeSpan > static_cast<DWORD>(m_lpSoundBallData->nTimeCheckSpan))
			{
				//通过间隔时间检测测试
				m_dwLastCheckTime = dwCurTime;
				if (m_lpSoundBallData->fRation > (rand() % 100 / 100.0f))
				{
					//通过几率测试
					hResult = pSound->Play3DSound(SCENE_SOUND,
						m_lpSoundBallData->strFileName,
						fVolume,
						FALSE,
						&(0.01f * m_Translation),
						&vSpeed,
						fRadius * g_SoundBallConfig.m_f3DSoundMinDistance * 0.01f,
						DS3D_DEFAULTMAXDISTANCE,
						&m_dwSoundID);
					KG_COM_PROCESS_ERROR(hResult);
					m_fVolume = fVolume;
				}
			}
		}

		if (abs(m_fVolume - fVolume) > FLT_EPSILON)
		{
			pSound->Set3DSoundVolume(m_dwSoundID, fVolume);
			m_fVolume = fVolume;
		}
	}
	else 
	{
		//超出了范围
		if (m_dwSoundID && m_lpSoundBallData->bLoop)
		{
			pSound->Stop3DSoundAfterThisTime(m_dwSoundID);
			m_dwSoundID = 0;
			m_bPlayFailed = TRUE;
		}
	}
Exit1:
	hRetCode = S_OK;
Exit0:
	return hRetCode;
}

HRESULT KG3DRepresentObjectSoundBall::ComputeBBox()
{
	if(!m_lpSoundBallData)
		return S_OK;

	//float fSize = m_Scaling.y * m_lpSoundBallData->fRadius;

	D3DXVECTOR3 V(g_SoundBallConfig.m_fBoundingBoxSize, 
		g_SoundBallConfig.m_fBoundingBoxSize,
		g_SoundBallConfig.m_fBoundingBoxSize);
	m_BBox.A = m_Translation - V;
	m_BBox.B = m_Translation + V;
	return S_OK;
}


HRESULT KG3DRepresentObjectSoundBall::Render(DWORD dwOption)
{
	if (g_bRenderSoundBall)
	{
		g_cSoundBallRender.PushBack(this);
	}
	return S_OK;
}

void KG3DRepresentObjectSoundBall::Render()
{
	D3DXMATRIX matTrans;
	KG3DRenderState RenderState;
	GraphicsStruct::RenderStateAlpha Alpha(0x20,FALSE,TRUE,D3DBLEND_INVSRCALPHA ,D3DBLEND_SRCALPHA);
	GraphicsStruct::RenderState Fog(D3DRS_FOGENABLE, FALSE);
	GraphicsStruct::RenderStateCullMode CullMode(D3DCULL_NONE);

	KG_PROCESS_SUCCESS(!g_bRenderSoundBall);
	KG_PROCESS_ERROR(m_lpSoundBallData);

	g_pd3dDevice->SetTexture(0, NULL);
	g_pd3dDevice->SetTexture(1, NULL);

	RenderState.SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	RenderState.SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	RenderState.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	RenderState.SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	RenderState.SetTextureStageState(0, D3DTSS_ALPHAOP,  D3DTOP_SELECTARG1);
	RenderState.SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
	D3DXMatrixTranslation(&matTrans, m_Translation.x, m_Translation.y, m_Translation.z);
	if (g_SoundBallConfig.m_bRenderSoundBallRange || m_bRenderRange)
	{
		g_cGraphicsTool.DrawSphereEx(&matTrans, g_SoundBallConfig.m_crSoundBallRangeColor, m_lpSoundBallData->fRadius * m_Scaling.y, TRUE);
	}
	g_cGraphicsTool.DrawSphereEx(&matTrans, g_SoundBallConfig.m_crSoundBallColor, g_SoundBallConfig.m_fBoundingBoxSize, TRUE);
	
Exit1:
Exit0:
	;
}

HRESULT KG3DRepresentObjectSoundBall::Copy(KG3DRepresentObject* pSrcObject)
{
	KG3DRepresentObjectSoundBall* pSrcBall = static_cast<KG3DRepresentObjectSoundBall*>(pSrcObject);

	g_cSoundBallTable.Release(m_uFileHash);
	
	m_lpSoundBallData = pSrcBall->m_lpSoundBallData;
	m_szSoundBallDataFileName = pSrcBall->m_szSoundBallDataFileName;
	m_uFileHash = pSrcBall->m_uFileHash;

	pSrcBall->m_lpSoundBallData->nRef++;

	KG3DRepresentObject::Copy(pSrcObject);

	ComputeBBox();
	return S_OK;
}

HRESULT KG3DRepresentObjectSoundBall::RenderSelectMark(DWORD dwColor /* = 0xFFFF0000 */)
{
	g_cGraphicsTool.DrawAABBox(&m_BBox,dwColor);
	return S_OK;
}

BOOL KG3DRepresentObjectSoundBall::IsRayIntersect(D3DXVECTOR3&Inter,D3DXVECTOR3 Origin,D3DXVECTOR3 Direction)
{
	if(!m_lpSoundBallData)
		return FALSE;

	float K = PointDistanceLine(&m_Translation,&Origin,&(Origin+Direction));

	if(K<m_lpSoundBallData->fRadius * m_Scaling.y)
	{
		Inter = m_Translation;
		return TRUE;
	}
	return FALSE;
}

HRESULT KG3DRepresentObjectSoundBall::ComputeDynamicBBox()
{
	return ComputeBBox();
}

HRESULT KG3DRepresentObjectSoundBall::SaveToInformationBuffer(/*DWORD& dwType,DWORD& dwLength,*/KG3DMemoryFile* pFile)
{
	//pFile->Reset();
	//dwType = m_dwType;

	CHAR  SoundName[MAX_PATH];
	ZeroMemory(SoundName,sizeof(SoundName));

	DWORD dwVersion = 0;

	pFile->write(&dwVersion,sizeof(DWORD));

	wsprintf(SoundName,"%s",m_szSoundBallDataFileName.c_str());
	pFile->write(SoundName, sizeof(CHAR) * MAX_PATH);

	//dwLength = pFile->GetLength();
	return S_OK;

} 

HRESULT KG3DRepresentObjectSoundBall::LoadFromInformationBuffer(/*DWORD dwType,DWORD dwLength,*/const KG3DMemoryFile* pFile,DWORD dwOption)
{
	//pFile->Reset();
	//m_dwType = dwType;

	CHAR  SoundName[MAX_PATH];
	DWORD dwVersion = 0;

	pFile->read(&dwVersion,sizeof(DWORD));

	pFile->read(SoundName,sizeof(CHAR) * MAX_PATH);

	LoadFromSoundBallDataFile(SoundName);

	return S_OK;
}

HRESULT KG3DRepresentObjectSoundBall::Load( LPCTSTR lpFileName, DWORD_PTR wParam, DWORD_PTR lParam )
{
	return this->LoadFromSoundBallDataFile(lpFileName);
}

HRESULT KG3DRepresentObjectSoundBall::GetAnotherIEInterface( DWORD dwType, VOID** ppAnotherInterface )
{
	if (dwType == REPRESENTOBJECT_SOUNDBALL && NULL != ppAnotherInterface)
	{
		*ppAnotherInterface = reinterpret_cast<VOID*>(static_cast<IEKG3DRepresentObjectSoundBall*>(this));
		return S_OK;
	}
	return E_FAIL;
}


HRESULT KG3DSoundBallDataTable::LoadFromFile(LPCTSTR strFileName,
										 SoundBallData *pData)
{
	HRESULT hRetCode = E_FAIL;
	IIniFile* pFile = NULL;
	TCHAR strDataSectionName[MAX_PATH];
	int nVersion = 0;
	KG_PROCESS_ERROR(strFileName);

	pFile = g_OpenIniFile(strFileName);
	KG_PROCESS_ERROR(pFile);

	pFile->GetInteger(s_strHeaderSectionName, "Version", 0, &nVersion);
	pFile->GetFloat(s_strHeaderSectionName, "Radius", 1000.0f, &pData->fRadius);
	pFile->GetInteger(s_strHeaderSectionName, "TimCheckSpan", 10000, &pData->nTimeCheckSpan);
	//这里soundcount没有实际的用途， 只处理第一个sound
	pFile->GetInteger(s_strHeaderSectionName,"SoundCount", 0, &pData->nSoundCount);

	sprintf_s(strDataSectionName, 
		MAX_PATH,
		"%s0", 
		s_strDataSectionPrefix);

	pFile->GetString(strDataSectionName, "FileName", "", pData->strFileName, MAX_PATH);
	pFile->GetInteger(strDataSectionName, "Loop", 0, &pData->bLoop);
	pFile->GetFloat(strDataSectionName, "Ration", 1.0f, &pData->fRation);
	pFile->GetInteger(strDataSectionName, "UseTime", FALSE, &pData->bTimeEnable);
	pFile->GetInteger(strDataSectionName, "TimeStart", 0, &pData->nTimeStart);
	pFile->GetInteger(strDataSectionName, "TimeEnd", 0, &pData->nTimeEnd);

	if ((nVersion == 0) &&
		(pData->fRation == 1.0f))
	{
		pData->bLoop = 1;
	}
	hRetCode = S_OK;
Exit0:
	if (FAILED(hRetCode) && strFileName)
	{
		KGLogPrintf(KGLOG_ERR, "Load sound ball %s failed.\n", strFileName);
	}
	SAFE_RELEASE(pFile);
	return hRetCode;
}

HRESULT KG3DSoundBallDataTable::SaveToFile(LPCTSTR strFileName,
									   SoundBallData *pData)
{
	HRESULT hRetCode = E_FAIL;
	IIniFile* pFile = NULL;
	TCHAR strDataSectionName[MAX_PATH];

	KG_PROCESS_ERROR(strFileName);

	pFile = g_OpenIniFile(strFileName, true, true);
	KG_PROCESS_ERROR(pFile);

	pFile->WriteInteger(s_strHeaderSectionName, "TimCheckSpan", pData->nTimeCheckSpan);
	pFile->WriteFloat(s_strHeaderSectionName, "Radius", pData->fRadius);
	pFile->WriteInteger(s_strHeaderSectionName, "SoundCount", pData->nSoundCount);
	pFile->WriteInteger(s_strHeaderSectionName, "Version", s_nVersion);
	sprintf_s(strDataSectionName, 
		MAX_PATH,
		"%s0", 
		s_strDataSectionPrefix);

	pFile->WriteString(strDataSectionName, "FileName", pData->strFileName);
	pFile->WriteInteger(strDataSectionName, "Loop", pData->bLoop);
	pFile->WriteFloat(strDataSectionName, "Ration", pData->fRation);
	pFile->WriteInteger(strDataSectionName, "UseTime", pData->bTimeEnable);
	pFile->WriteInteger(strDataSectionName, "TimeStart", pData->nTimeStart);
	pFile->WriteInteger(strDataSectionName, "TimeEnd", pData->nTimeEnd);
	
	pFile->Save(strFileName);
	hRetCode = S_OK;
Exit0:
	SAFE_RELEASE(pFile);
	return hRetCode;
}

KG3DSoundBallRender::KG3DSoundBallRender()
{

}

KG3DSoundBallRender::~KG3DSoundBallRender()
{

}

void KG3DSoundBallRender::Render()
{
	KG_PROCESS_SUCCESS(!g_bRenderSoundBall);

	for (size_t i = 0; i < m_SoundBalls.size(); i++)
	{
		if (m_SoundBalls[i])
		{
			m_SoundBalls[i]->Render();
			m_SoundBalls[i]->Release();
		}
	}
	m_SoundBalls.clear();
Exit1:
	;
}

void KG3DSoundBallRender::PushBack(KG3DRepresentObjectSoundBall *pSoundBall)
{
	KG_PROCESS_ERROR(pSoundBall);
	pSoundBall->AddRef();
	m_SoundBalls.push_back(pSoundBall);
Exit0:
	;
}

HRESULT KG3DRepresentObjectSoundBall::GetName(LPTSTR pName, size_t BufferSize)
{
	strcpy_s(pName, 
		BufferSize,
		m_szSoundBallDataFileName.c_str());
	return S_OK;
}

void KG3DSoundBallDataTable::Refresh(LPCSTR strFileName)
{
	std::map<UINT, SoundBallData*>::iterator it;
	KG_PROCESS_ERROR(strFileName);
	UINT uHash = g_FileNameHash(strFileName);
	it = m_mapSoundBallData.find(uHash);
	KG_PROCESS_ERROR(it != m_mapSoundBallData.end());

	LoadFromFile(strFileName, it->second);
Exit0:
	;
}

HRESULT KG3DRepresentObjectSoundBall::SetScaling(const D3DXVECTOR3* pScale)
{
	KG3DRepresentObject::SetScaling(pScale);
	ComputeBBox();
	return StopSound();
}

HRESULT KG3DRepresentObjectSoundBall::SetTranslation(const D3DXVECTOR3* pTrans)
{
	KG3DRepresentObject::SetTranslation(pTrans);
	ComputeBBox();
	return StopSound();
}

HRESULT KG3DRepresentObjectSoundBall::StopSound()
{
	IKG3DSoundShell *pSound = g_cEngineManager.GetSound();
	KG_PROCESS_SUCCESS(!pSound);
	if (m_dwSoundID)
	{
		pSound->Stop3DSound(m_dwSoundID);
		m_dwSoundID = 0;
	}
Exit1:
	return S_OK;
}

void KG3DRepresentObjectSoundBall::Stop()
{
	m_bHalt = TRUE;
	StopSound();
}

void KG3DRepresentObjectSoundBall::Start()
{
	m_bHalt = FALSE;
}

HRESULT KG3DRepresentObjectSoundBall::OnSelect()
{
	m_bRenderRange = TRUE;
	return S_OK;
}

HRESULT KG3DRepresentObjectSoundBall::OnUnSelect()
{
	m_bRenderRange = FALSE;
	return S_OK;
}
