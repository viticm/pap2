#include "StdAfx.h"
#include "kg3dmodeltable.h"
#include "KG3DGraphicsTool.h"
#include "KG3DTexture.h"
#include "KG3DTextureTable.h"
#include "kg3dcamerapreset.h"
#include "KG3DModelST.h"
#include "kg3danimationcontroller.h"
#include "kg3dbip.h"
#include "KG3DSceneSceneEditor.h"
#include "KG3DEngineManager.h"
#include "KG3DAnimationEventHandlerRepresentDummy.h"
#include "PhysiscSystem\KG3DPhysiscScene.h"
#include "PhysiscSystem\KG3DPhysiscManager.h"
#include "KG3DBipTable.h"
#include "KG3DClipTools.h"
#include "KG3DRecorder.h"
#include "KG3DMultiThreadLoader.h"
#include "KG3DIntersectionBase.h"
#include "KG3DMeshSkin.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

KG3DAnimationEventHandlerRepresentDummy* s_pDummy = NULL;

KG3DModelST::KG3DModelST(void)
{
	m_bDrawBigBox = FALSE;
	s_pDummy->m_pModel = this;
	m_pBip = NULL;
	m_nNumModelNeedToAttach = 0;
	m_bFrameMoved = FALSE;
	m_lpSkinMeshUion = NULL;
}

KG3DModelST::~KG3DModelST(void)
{
	UnInit();
}

void KG3DModelST::RelaseBip()
{
	if (m_pBip)
	{
		g_cBipTable.ReleaseBip(m_pBip->m_dwID);
		m_pBip->SetModelST(NULL);
		KG_COM_RELEASE(m_pBip);
	}
}

void KG3DModelST::Detach()
{
	RelaseBip();	
}

HRESULT KG3DModelST::FrameMoveAnimations()
{
    if (m_Splitter.GetEnable())
    {
        D3DXMATRIX *pResult = m_pBip->GetObjMatricesArray();
        m_Splitter.FrameMove(pResult);
    }
    else
	{
        m_pBip->FrameMove(m_Controllers);
	}

	for (size_t i = 0; i < m_Models.size(); i++)
	{
		KG3DModelLod& Lod = m_Models[i];
		KG3DModel *pModel = Lod.pModelCurrent;
		if (pModel)
		{
			pModel->SetClientAdd(m_bClientAdd);
			pModel->FrameMove(RENDER_BIP, NULL);
		}	

		KG3DModel *pModelHigh = Lod.pModelHigh;
		if(pModelHigh && pModelHigh!=pModel)
		{
			pModelHigh->m_matWorld = pModel->m_matWorld;
			pModelHigh->UpdateChildModel();
		}
	}

	m_dwFrameCount++;

	return S_OK;
}

HRESULT KG3DModelST::FrameMove()
{
	HRESULT hrResult = E_FAIL;
	HRESULT hrRetCode = E_FAIL;

	KG_PROCESS_SUCCESS(m_bInMultiThreadLoadList);

	KG_PROCESS_ERROR(m_pBip);

	if(IsLoaded())
	{
        ProcessCommands();

		if(m_nNumModelNeedToAttach == 0)
		{
			if (!m_bMeshPostLoadProcessed && IsResourceReady(FALSE) && g_nNumTextureDynamicLoadedFrame<=2)
			{
				ComputeBBoxLocal();
				ComputeBBox();

				//CreateSkinMeshUion();

				m_bMeshPostLoadProcessed = TRUE;
				g_nNumTextureDynamicLoadedFrame++;
			}

			m_Controllers[ANICTL_PRIMARY].FrameMove();
			m_Controllers[ANICTL_SECONDARY].FrameMove();

            hrRetCode = Helper_UpdateTransformation(TRUE);
            KG_COM_PROCESS_ERROR(hrRetCode);

			FrameMoveAnimations();

			UpdateChildModel();

			Helper_CaptionFrameMove();

			if(m_lpSkinMeshUion)
				m_lpSkinMeshUion->FrameMove();
		}
	}
	else
	{
		for (size_t i = 0; i < m_Models.size(); i++)
		{
			KG3DModel *pModel = m_Models[i].pModelCurrent;
			KG3DModel *pModelHigh = m_Models[i].pModelHigh;
			if (pModel)
			{
				pModel->FrameMove(RENDER_BIP, NULL);
			}	

			if (pModel != pModelHigh)
			{
				pModelHigh->UpdateChildModel();
			}
		}

		UpdateChildModel();
	}
   
Exit1:
    if (g_bClient)
        m_bVisible = FALSE;
	m_bFrameMoved = TRUE;
    hrResult = S_OK;
Exit0:
	return hrResult;
}

HRESULT KG3DModelST::_LoadFromFile(const char cszFileName[], 
								   unsigned uFileNameHash, 
								   unsigned uOption)
{
	TCHAR strExt[MAX_PATH];
	_splitpath_s(cszFileName, 
		NULL,
		0, 
		NULL,
		0, 
		NULL,
		0, 
		strExt, 
		MAX_PATH);
	_strlwr_s(strExt, MAX_PATH * sizeof(TCHAR));

	if(_strcmpi(strExt,".ModelST")==0)
	{
		return LoadCompactFromModelSTFile(cszFileName,"Default",uOption);
	}
	else
	{
		HRESULT hResult = E_FAIL;
		HRESULT hRetCode = E_FAIL;
		const MDLFileContent* pContent = g_cModelSTCache.GetMDLContent(cszFileName);
		KG3DBip *pBipSrc = NULL;
		unsigned uOptionSave = uOption | MLO_MESH_BIP;
		KG_PROCESS_ERROR(pContent);
		KG_PROCESS_ERROR(pContent->strBipFile.size());

        pBipSrc = g_cBipTable.GetBip(pContent->strBipFile.c_str());
        KGLOG_PROCESS_ERROR(pBipSrc);

		ASSERT(!m_pBip);
		m_pBip = new KG3DBip;
		KGLOG_PROCESS_ERROR(m_pBip);

        m_pBip->Clone(pBipSrc);
		m_pBip->OnModelSTAttach(this,m_Controllers);
		m_pBip->SetModelST(this);

		for (DWORD i = 0; i < pContent->dwNumModels; i++)
		{
			KG3DModel *pModel = NULL;
			const std::string &strMeshFile = pContent->strMeshFile[i];
			const std::string &strMtlFile = pContent->strMaterialFile[i];
			uOption = uOptionSave;
			if (!strMtlFile.empty())
			{
				uOption |= MLO_MATERIAL_NOMAT;
			}

			hRetCode = g_cModelTable.LoadResourceFromFile(strMeshFile.c_str(), 
				0, 
				uOption, 
				(IKG3DResourceBase **)&pModel);
			KG_COM_PROCESS_ERROR(hRetCode);

			_ASSERTE(pModel);

			if (!strMtlFile.empty())
			{
				hRetCode = pModel->LoadMaterialFromFile(strMtlFile.c_str(), uOption);
				if (FAILED(hRetCode))
				{
					g_OutputDebugInfoFmt("Failed LoadMaterialFromFile: %s", strMtlFile.c_str());
					SAFE_RELEASE(pModel);
				}
				KGLOG_COM_PROCESS_ERROR_CONSOLE(hRetCode);
			}

			int n = AttachModel(pModel);

			if(n!=-1)
			{
				m_BBoxLocal.AddPosition(pModel->m_BBox.A);//最后m_BBoxLocal记录的是联合ModelST里面的模型的BBox
				m_BBoxLocal.AddPosition(pModel->m_BBox.B);
			}

			pModel->Release();
		}
		ComputeBBox();
		m_fShadowScaleFactor = ComputeShadowScaleFactor(m_BBoxLocal);

		hResult = S_OK;
Exit0:
		return hResult;
	}
}

HRESULT KG3DModelST::LoadFromFile(const char cszFileName[], 
								  unsigned uFileNameHash, 
								  unsigned uOption)
{
	HRESULT hr = E_FAIL;

	BOOL bIsPassRecord = FALSE;
	if (g_cRecorder.IsRecord() && !g_cRecorder.IsLock())
	{
		if (uOption & MLO_MODEL_RECORD)
		{
			m_dwRecordID =  KG3DRecorder::GetMasterID();
			KG3DRecorder::Parameters MyParameters;
			MyParameters.nNum = 4;
			MyParameters.size_tDataSize = sizeof(TrueType) + 
										  MAX_PATH * sizeof(char) + sizeof(uFileNameHash) + sizeof(uOption);
			char* pData = new char[MyParameters.size_tDataSize];
			MyParameters.pData = pData;
			MyParameters.nFrom[0] = 0;		
			*(TrueType*)(&pData[MyParameters.nFrom[0]]) = TT_MODELST;
			MyParameters.nFrom[1] = sizeof(TrueType);
			strcpy(&pData[MyParameters.nFrom[1]],cszFileName);
			MyParameters.nFrom[2] = sizeof(TrueType) + MAX_PATH * sizeof(char);
			*(unsigned*)(&pData[MyParameters.nFrom[2]]) = uFileNameHash;
			MyParameters.nFrom[3] = sizeof(TrueType) + MAX_PATH * sizeof(char) + sizeof(uFileNameHash);
			*(unsigned*)(&pData[MyParameters.nFrom[3]]) = uOption;
			g_cRecorder.RecordEvent(m_dwRecordID,FT_KG3DMODEL_LOADFROMFILE,GetCurrentTime(),MyParameters);

			bIsPassRecord = TRUE;
			g_cRecorder.Lock();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	m_uOption = uOption;
	m_scName  = cszFileName;

	/*if((uOption & MLO_MESH_MULTI) && (g_eEngineOption.bUseMultiThreadLoad))
	{
		g_cMultiThreadLoader.TryMultiThreadLoad_Model(this);
		hr = S_OK;
	}
	else*/
	{
		hr = _LoadFromFile(cszFileName,uFileNameHash,uOption);
	}

	if (bIsPassRecord)
		g_cRecorder.UnLock();

	return hr;
}

HRESULT KG3DModelST::Helper_PlayAnimationNormal(DWORD dwPlayType, 
												LPCSTR strAnimationName, 
												FLOAT fSpeed, 
                                                int   nOffsetTime, 
												PVOID pReserved,
												enuAnimationControllerPriority Priority,
												enuModelPlayAnimationType AnimationType,
												KG3DAnimationTagContainer* pTagContainerNew,
												KG3DClip* pClip)
{
	m_Splitter.EnableWarper(FALSE);
	HRESULT hr = E_FAIL;
	KGLOG_PROCESS_ERROR(m_pBip);
	hr = m_pBip->PlayAnimation(strAnimationName, dwPlayType, nOffsetTime, fSpeed, pReserved, Priority,m_Controllers,pClip);
	KGLOG_COM_PROCESS_ERROR(hr);
	hr = S_OK;
Exit0:
	return hr;
}

HRESULT KG3DModelST::Helper_PlayAnimationTagedAnimation(DWORD dwPlayType,
													  LPCSTR strFileName,
													  FLOAT fSpeed,
													  int   nOffsetTime,
													  PVOID pReserved,
													  enuAnimationControllerPriority Priority,
													  enuModelPlayAnimationType AnimationType,
													  KG3DAnimationTagContainer* pTagContainerNew,
													  KG3DClip* pClip)
{
	HRESULT hRetCode = E_FAIL;
	
	KG_PROCESS_ERROR(strFileName);
	KG_PROCESS_ERROR(m_pBip);

	if (pReserved)
	{
		m_pBip->m_TweenSpan = *(TweenTimeSpan*)pReserved;
	}
	else
	{
		m_pBip->m_TweenSpan.fTweenWeigth = 1.0f;
		m_pBip->m_TweenSpan.dwTweenIn = 0;
		m_pBip->m_TweenSpan.dwTweenOut = 0;
	}

	hRetCode = KG3DModel::Helper_PlayAnimationTagedAnimation(dwPlayType,
		strFileName,
		fSpeed,
		nOffsetTime,
		pReserved,
		Priority,
		AnimationType,
		pTagContainerNew,
		pClip);
Exit0:
	return hRetCode;
}

HRESULT KG3DModelST::LoadAnimationFromFile(LPCSTR strFileName, DWORD dwOption)
{
	HRESULT hr = E_FAIL;
	hr = PlayAnimation(ENU_ANIMATIONPLAY_CIRCLE, strFileName, 1.0f, 0, NULL, NULL, ANICTL_PRIMARY);
	return hr;
}


void KG3DModelST::OnBind(IKG3DModel* pModel,KG3DModel* pModelMedium,KG3DModel* pModelLow)
{
	KG3DModel *p3DModel = static_cast<KG3DModel*>(pModel);
	KG3DModelLod Lod;

	KG_PROCESS_ERROR(pModel);

	Lod.pModelHigh = p3DModel;
	Lod.pModelMedium = pModelMedium;
	Lod.pModelLow = pModelLow;
	Lod.pModelCurrent = Lod.pModelHigh;
	if(Lod.pModelMedium)
	{
		Lod.pModelMedium->AddRef();
	}
	if(Lod.pModelLow)
	{
		Lod.pModelLow->AddRef();
	}

	m_ChildModels.insert(Lod);
	m_bBindChanged = TRUE;
Exit0:
	return;
}

void KG3DModelST::_SetPhysicsScene(KG3DPhysiscScene *pScene)
{
	KG3DModel::_SetPhysicsScene(pScene);
	for (size_t i = 0; i < m_Models.size(); i++)
	{
		KG3DModelLod& lod = m_Models[i];
		if (lod.pModelHigh)
		{
			lod.pModelHigh->_SetPhysicsScene(pScene);
		}
		if (lod.pModelMedium)
		{
			lod.pModelMedium->_SetPhysicsScene(pScene);
		}
		if (lod.pModelLow)
		{
			lod.pModelLow->_SetPhysicsScene(pScene);
		}
	}
}
void KG3DModelST::SetPhysicsScene(KG3DPhysiscScene *pScene)
{
    if (m_lpPhysicsScene == pScene)
        return;	

    if(IsLoaded() && m_nNumModelNeedToAttach==0)
    {
        _SetPhysicsScene(pScene);
    }
    else
    {
        KG3DModelCommand_SetPhysicsScene* pCmd = new KG3DModelCommand_SetPhysicsScene(pScene);
        m_Commands.AddCommand(pCmd,FALSE);
    }
}

int KG3DModelST::FindSocket(const TCHAR* strSocketName, BindExtraInfo* pExtInfo)
{
	int nReturn = -1;
	HRESULT hr = E_FAIL;
	TCHAR strLower[KG3DMesh::s_dwMaxSocketName];

	KG_PROCESS_ERROR(pExtInfo);
	KG_PROCESS_ERROR(strSocketName);

	pExtInfo->pModel = NULL;
	pExtInfo->nBindIndex = -1;
	pExtInfo->nMatrixIndex = -1;

	strcpy_s(strLower, KG3DMesh::s_dwMaxSocketName, strSocketName);
	_strlwr_s(strLower);

	for (size_t i = 0; i < m_Models.size(); i++)
	{
		if (m_Models[i].pModelHigh)
		{
			int n = m_Models[i].pModelHigh->FindSocket(strLower, NULL);
			if (n != -1)
			{
				pExtInfo->nBindIndex = n;
				pExtInfo->pModel = m_Models[i].pModelHigh;
				hr = S_OK;
				break;
			}
		}
	}
	KG_COM_PROCESS_ERROR(hr);

	nReturn = pExtInfo->nBindIndex;
Exit0:
	return nReturn;
}

int KG3DModelST::FindBone(const TCHAR* strBoneName, BindExtraInfo* pExtInfo)
{
	int nReturn = -1;
	HRESULT hr = E_FAIL;
	TCHAR strLower[KG3DMesh::s_dwMaxBoneName];
	strcpy_s(strLower, KG3DMesh::s_dwMaxBoneName, strBoneName);
	_strlwr_s(strLower);
	if (pExtInfo)
	{
		pExtInfo->pModel = NULL;
		pExtInfo->nBindIndex = -1;
		pExtInfo->nMatrixIndex = -1;
		KG_PROCESS_ERROR(strBoneName);
		for (size_t i = 0; i < m_Models.size(); i++)
		{
			if (m_Models[i].pModelHigh)
			{
				int n = m_Models[i].pModelHigh->FindBone(strLower, NULL);
				if (n != -1)
				{
					pExtInfo->nBindIndex = n;
					pExtInfo->pModel = m_Models[i].pModelHigh;
					hr = S_OK;
					break;
				}
			}
		}
		KG_COM_PROCESS_ERROR(hr);
		nReturn = pExtInfo->nBindIndex;
	}
	else
	{
		KG_PROCESS_ERROR(m_pBip);
		nReturn = m_pBip->FindBone(strLower);
	}
Exit0:
	return nReturn;
}

HRESULT KG3DModelST::ComputeBBox()
{
	D3DXVECTOR3 vec3BBox[2];

	m_BBox.Clear();

	if(!m_pParentMatrix)
	{
		m_BBox.AddPosition(m_Translation+D3DXVECTOR3(100,100,100));
		m_BBox.AddPosition(m_Translation-D3DXVECTOR3(100,100,100));
	}

	for (size_t i = 0; i < m_Models.size(); i++)
	{
		if (m_Models[i].pModelCurrent)
		{
			m_Models[i].pModelCurrent->GetBBox(vec3BBox[0], vec3BBox[1]);
			m_BBox.AddPosition(vec3BBox, 2);
		}
	}

	return S_OK;
}

IKG3DAnimationController* KG3DModelST::GetAnimationController(enuAnimationControllerPriority nPriority)
{
	IKG3DAnimationController *pReturn = NULL;
	KG_PROCESS_ERROR(nPriority < ANICTL_COUNT);

	pReturn = &m_Controllers[nPriority];
Exit0:
	return pReturn;
}

void KG3DModelST::_SeekSplitAnimation(DWORD dwSplitType, 
									 enuAnimationControllerPriority nPrority,
									 DWORD dwSeekType, 
									 float fOffset)
{
	m_Splitter.SeekAnimation(dwSplitType, 
		nPrority,
		dwSeekType,
		static_cast<int>(fOffset));
}

HRESULT KG3DModelST::SetPointLightInfo(KG3D_PointLight_Info& vInfo)
{
    for (size_t i = 0; i < m_Models.size(); ++i)
    {
        if (m_Models[i].pModelCurrent)
            m_Models[i].pModelCurrent->SetPointLightInfo(vInfo);
    }

    return S_OK;
}

BOOL KG3DModelST::IsRayIntersect(D3DXVECTOR3& Inter,D3DXVECTOR3 Origin,D3DXVECTOR3 Direction)
{
	BOOL bReturn = FALSE;
	// --------- whene show bone --------------
	if (m_bShowBones)
	{
		TCHAR szSelBone[128];
		DWORD dwType;
		m_boneMash.GetSelBoneName(szSelBone, sizeof(szSelBone) / sizeof(TCHAR), &dwType);
		if (stricmp(szSelBone, TEXT("")))
		{
			Inter.x = m_matWorld._41;
			Inter.y = m_matWorld._42;
			Inter.z = m_matWorld._43;

			return TRUE;
		}
	}
	else if (m_BBox.IsRayIntersect3D(Inter, Origin, Direction, NULL, NULL))
	{
		for (size_t i = 0; i < m_Models.size(); i++)
		{
			if (m_Models[i].pModelCurrent)
			{
				if (m_Models[i].pModelCurrent->IsRayIntersect(Inter, Origin, Direction))
				{
					bReturn = TRUE;
					break;
				}  
			}
		}
		////if (nResult && m_pContainer && !g_bClient)
		//重新计算Local的原因是动的时候需要重新计算Local的Box，但ShadowScaleFactor不需要重新更新

		ComputeBBoxLocal();
	}

	return bReturn;
}
BOOL KG3DModelST::RayIntersection( const D3DXVECTOR3& vSrc , const D3DXVECTOR3& vNormalizedDir, FLOAT* pfRet 
								 ,D3DXVECTOR3* pFaceNormal , BOOL bIntersectBoxOnly, KG3DIntersectionFilter* pFilter /*= NULL*/)
{
	if (NULL != pFilter)
	{
		HRESULT hr = pFilter->Accept(*this);
		if(FAILED(hr))
			return FALSE;
		hr = pFilter->Accept(static_cast<KG3DModel&>(*this));
		if(FAILED(hr))
			return FALSE;
	}

	BOOL bRet = FALSE;
	FLOAT fRet = FLT_MAX;
	
	if (bIntersectBoxOnly)
	{
		bRet = m_BBox.RayIntersection(vSrc, vNormalizedDir, &fRet, NULL);
		if(bRet && NULL != pfRet)
		{
			*pfRet = fRet;
		}
		return bRet;
	}

	bRet = m_BBox.RayIntersection(vSrc, vNormalizedDir, NULL, NULL);
	if (! bRet)
		return FALSE;	
	
	for (size_t i = 0; i < m_Models.size(); i++)
	{
		if (m_Models[i].pModelCurrent)
		{
			FLOAT fTemp = FLT_MAX;
			if (m_Models[i].pModelCurrent->RayIntersection(vSrc, vNormalizedDir, &fTemp, NULL,bIntersectBoxOnly, pFilter))
			{
				if (fTemp < fRet && fTemp > 0)
				{
					fRet = fTemp;
				}
			}  
		}
	}

	bRet = fRet < FLT_MAX;
	if (pfRet && bRet)
	{
		*pfRet = fRet;
	}
	return bRet;
}

static BOOL bRenderAlphaSpecial = FALSE;

HRESULT KG3DModelST::Render(unsigned int uOption, void* pExtInfo)
{
	HRESULT hr = E_FAIL;
	KG3DRenderState RenderState;

	std::set<KG3DModelLod>::iterator it = m_ChildModels.begin();
	std::set<KG3DModelLod>::iterator itEnd = m_ChildModels.end();

	KG_PROCESS_ERROR(IsResourceReady(TRUE));

	if (m_bShowBones)
		m_boneMash.Render();
	
	//here is setting mipmap info
	RenderState.SetSamplerState(0, D3DSAMP_MIPMAPLODBIAS, FtoDW(-1.0f));
	RenderState.SetSamplerState(0, D3DSAMP_MINFILTER, (D3DTEXTUREFILTERTYPE)g_cEngineOption.nSampMinFilter);
	RenderState.SetSamplerState(0, D3DSAMP_MAGFILTER, (D3DTEXTUREFILTERTYPE)g_cEngineOption.nSampMagFilter);
	RenderState.SetSamplerState(0, D3DSAMP_MIPFILTER, (D3DTEXTUREFILTERTYPE)g_cEngineOption.nSampMipFilter);
	RenderState.SetSamplerState(0, D3DSAMP_MAXANISOTROPY ,g_cEngineOption.dwMaxAnisotropy);

	//g_pd3dDevice->GetLight(0, &LightSave);
	//g_pd3dDevice->GetRenderState(D3DRS_AMBIENT, &AmbSave);

	{
		//KG3DLight*   pLight    = NULL;  
		//IEKG3DScene* pCurScene = g_cGraphicsTool.GetScene();
		//KG3DScene* pSolidScene = dynamic_cast<KG3DScene*>(pCurScene);
		//if (pSolidScene)
		//{
		//	pLight = &g_cGraphicsTool.GetPlayerLight(*pSolidScene);
		//}
		//KG_PROCESS_ERROR(pLight);

		////g_pd3dDevice->SetLight(0, &pLight->GetD3DLight());
		//pLight->Apply(0);
		//g_pd3dDevice->SetRenderState(D3DRS_AMBIENT, pLight->GetSceneAmbient());
	}
	

	if (m_bRenderSpecialAlpha && !bRenderAlphaSpecial)
	{
		hr = RenderSpecialAlpha(uOption, pExtInfo);
		bRenderAlphaSpecial = FALSE;
	}
	else
	{
		hr = RenderCommon(uOption, pExtInfo);
	}
	KG_COM_PROCESS_ERROR(hr);

	while (it != itEnd) 
	{
		//if (*it && !(*it)->m_nIsSockWave)
		KG3DModelLod& Lod = *it;
        if (Lod.pModelCurrent->CheckNeedRender())
		{
			Lod.pModelCurrent->Render(0, NULL);
		}
		it++;
	}

Exit0:
	//g_pd3dDevice->SetLight(0, &LightSave);
	//g_pd3dDevice->SetRenderState(D3DRS_AMBIENT, AmbSave);
	return hr;
}

D3DXMATRIX* KG3DModelST::GetRenderMatrix()
{
	D3DXMATRIX* pResult = NULL;
	KG_PROCESS_ERROR(m_pBip);
	pResult = m_pBip->GetObjMatricesArray();
Exit0:
	return pResult;
}

HRESULT KG3DModelST::RenderCommon(unsigned int uOption, void* pExtInfo)
{
	HRESULT hRetCode = E_FAIL;
	DWORD dwRenderOption = uOption;
	D3DXMATRIX* pRenderMatrix = GetRenderMatrix();
	MeshRenderExtraInfo Info;

	if(!IsLoaded())
		return S_OK;

	pExtInfo = NULL;
	Info.Data = pRenderMatrix;
	Info.dwType = ENUM_MESHRENDER_EXTR_INFO_MATRIX;

	if (pRenderMatrix)
	{
		dwRenderOption |= RENDER_BIP;
		pExtInfo = &Info;
	}
	for(size_t i = 0; i < m_Models.size(); i++)
	{
        KG3DModel* p3DModel = m_Models[i].pModelCurrent;
		if (p3DModel)
		{
			p3DModel->Render(dwRenderOption, pExtInfo);
		}
		if(m_Models[i].pModelHigh && p3DModel != m_Models[i].pModelHigh)
		{
			m_Models[i].pModelHigh->RenderChildModels(uOption,NULL);
		}
	}
	hRetCode = S_OK;
	return hRetCode;
}

HRESULT KG3DModelST::RenderReflect(float fWaterHeight,D3DXPLANE& WaterPlane,D3DXPLANE& WaterPlaneFX)
{
	HRESULT hr = E_FAIL;
	UINT uOption = 0;
	IKG3DAnimationController* pController = NULL;
	KG_PROCESS_ERROR(m_pBip);

	pController = GetAnimationController(ANICTL_PRIMARY);
	if ( IsAnimationed() )
	{
		for(size_t i = 0; i < m_Models.size(); i++)
		{
			if (m_Models[i].pModelCurrent)
			{	
				MeshRenderExtraInfo Info;
				Info.Data = m_pBip->GetObjMatricesArray();
				Info.dwType = ENUM_MESHRENDER_EXTR_INFO_MATRIX;
				m_Models[i].pModelCurrent->RenderReflect(uOption | RENDER_BIP, &Info,fWaterHeight,WaterPlaneFX,WaterPlaneFX);
			}
		}
	}
	else
	{
		for(size_t i = 0; i < m_Models.size(); i++)
		{
			if (m_Models[i].pModelCurrent)
			{
				m_Models[i].pModelCurrent->RenderReflect(uOption, NULL,fWaterHeight,WaterPlaneFX,WaterPlaneFX);
			}
		}
	}

	if (! m_ChildModels.empty())
	{
		for (std::set<KG3DModelLod>::iterator i = m_ChildModels.begin(); i != m_ChildModels.end(); i++)
		{
			KG3DModelLod& Lod = *i;

			KG3DModel* pModel = Lod.pModelCurrent;
			//if (pModel && ! pModel->m_nIsSockWave)
            if (pModel->CheckNeedRender())
			{
				pModel->RenderReflect(fWaterHeight,WaterPlane,WaterPlaneFX);
			}
		}
	}


	hr = S_OK;
Exit0:
	return hr;
}

HRESULT KG3DModelST::RenderSpecialAlpha(unsigned int uOption, void* pExtInfo)
{
	HRESULT hResult = E_FAIL;
	HRESULT hRetCode = E_FAIL;

	if(!IsResourceReady(TRUE))
		return S_OK;

	BOOL bSaveAlphaBlend = g_cEngineOption.bEnableSpecialAlphaBlend;
	g_cEngineOption.bEnableSpecialAlphaBlend = false;
	bRenderAlphaSpecial = TRUE;
	m_RuntimeMaterial.SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	m_RuntimeMaterial.SetRenderState(D3DRS_COLORWRITEENABLE, 0);
	for (size_t i = 0; i < m_Models.size(); i++)
	{
		if (m_Models[i].pModelCurrent)
		{
			KG3DRuntimeMaterial* pRuntimeMaterial = NULL;
			pRuntimeMaterial = m_Models[i].pModelCurrent->GetRuntimeMaterial();
			ASSERT(pRuntimeMaterial);

			pRuntimeMaterial->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
			pRuntimeMaterial->SetRenderState(D3DRS_COLORWRITEENABLE, 0);
			pRuntimeMaterial->Enable(TRUE);
			m_Models[i].pModelCurrent->SetRenderSpecialAlpha(FALSE);
			m_Models[i].pModelCurrent->UpdateChildRuntimMaterial();
		}
	}
	hResult = Render(uOption, pExtInfo);
	KG_COM_PROCESS_ERROR(hResult);

	m_RuntimeMaterial.SetRenderState(D3DRS_COLORWRITEENABLE, 0xffffffff);
	m_RuntimeMaterial.SetRenderState(D3DRS_ZFUNC, D3DCMP_EQUAL);
	for (size_t i = 0; i < m_Models.size(); i++)
	{
		if (m_Models[i].pModelCurrent)
		{
			KG3DRuntimeMaterial* pRuntimeMaterial = NULL;
			pRuntimeMaterial = m_Models[i].pModelCurrent->GetRuntimeMaterial();
			ASSERT(pRuntimeMaterial);

			pRuntimeMaterial->SetRenderState(D3DRS_COLORWRITEENABLE, 0xffffffff);
			pRuntimeMaterial->SetRenderState(D3DRS_ZFUNC, D3DCMP_EQUAL);
			pRuntimeMaterial->Enable(TRUE);
			m_Models[i].pModelCurrent->SetRenderSpecialAlpha(FALSE);
			m_Models[i].pModelCurrent->UpdateChildRuntimMaterial();
		}
	}
	hResult = Render(uOption, pExtInfo);
	KG_COM_PROCESS_ERROR(hResult);

	m_RuntimeMaterial.Clear();
	m_RuntimeMaterial.Enable(FALSE);
	for (size_t i = 0; i < m_Models.size(); i++)
	{
		if (m_Models[i].pModelCurrent)
		{
			KG3DRuntimeMaterial* pRuntimeMaterial = NULL;
			pRuntimeMaterial = m_Models[i].pModelCurrent->GetRuntimeMaterial();
			ASSERT(pRuntimeMaterial);

			pRuntimeMaterial->Clear();
			pRuntimeMaterial->Enable(FALSE);
		}
	}

	

	hRetCode = S_OK;
Exit0:
	g_cEngineOption.bEnableSpecialAlphaBlend = bSaveAlphaBlend;
	return hRetCode;
}

HRESULT KG3DModelST::GetSocketMatrix(const BindExtraInfo* pInfo, 
									 D3DXMATRIX* pmatResult,
									 D3DXVECTOR3* pvec3Offset)
{
	HRESULT hr = E_FAIL;
	KG_PROCESS_ERROR(pInfo);
	KG_PROCESS_ERROR(pmatResult);
	KG_PROCESS_ERROR(pInfo->pModel);
	hr = pInfo->pModel->GetSocketMatrix(pInfo->nBindIndex, *pmatResult, pvec3Offset);
	hr = S_OK;
Exit0:
	return hr;
}

HRESULT KG3DModelST::GetBoneMatrix(const BindExtraInfo* pInfo, 
								   D3DXMATRIX* pmatResult, 
								   D3DXVECTOR3* pvec3Offset)
{
	HRESULT hr = E_FAIL;
	int nBipIndex = 0;
	D3DXMATRIX mat;
	KG_PROCESS_ERROR(pInfo);
	KG_PROCESS_ERROR(pmatResult);

	nBipIndex = pInfo->pModel->m_Mesh2Bip[pInfo->nBindIndex];
	if (pvec3Offset)
	{
		mat = m_pBip->GetBoneObjMatrix(nBipIndex);

		mat._41 += pvec3Offset->x;
		mat._42 += pvec3Offset->y;
		mat._43 += pvec3Offset->z;

		*pmatResult = mat * m_matWorld;
	}
	else
	{
		*pmatResult = m_pBip->GetBoneObjMatrix(nBipIndex) * m_matWorld;
	}

	hr = S_OK;
Exit0:
	return hr;
}

HRESULT KG3DModelST::UpdateBoneMesh(KG3DScene* pScene)
{
	return m_boneMash.Attach(this, pScene);
}

void KG3DModelST::SetBoneMeshAttachScene(KG3DScene* pScene)
{
	m_boneMash.SetAttachScene(pScene);
}

HRESULT KG3DModelST::UpdateBoneBBox(KG3DScene* pScene)
{
	HRESULT hr = E_FAIL;
	for (size_t i = 0; i < m_Models.size(); i++)
	{
		if (m_Models[i].pModelCurrent)
		{
			hr = m_Models[i].pModelCurrent->UpdateBoneBBox(pScene);
			/*	if (SUCCEEDED(hr))
			{
			return hr;
			}*/
		}
	}
	return S_OK;
}
HRESULT KG3DModelST::ShowBones(BOOL bShow)
{
	m_bShowBones = bShow;
	for (size_t i = 0; i <m_Models.size(); i++)
	{
		m_Models[i].pModelCurrent->ShowBones(m_bShowBones);
	}
	return S_OK;
}

HRESULT KG3DModelST::RenderToScreen(const D3DXVECTOR2* pA, 
									const D3DXVECTOR2* pC, 
									const D3DXVECTOR2* pUVA, 
									const D3DXVECTOR2* pUVC, 
									float fZ, 
									DWORD dwFlags,
									DWORD dwBackColor,
									void* pExtInfo)
{	
	HRESULT hr = E_FAIL;
	D3DXMATRIX matViewSave;
	D3DXMATRIX matProjSave;
	KG3DTexture* pTexture = NULL;
	IKG3DAnimationController* pController = NULL;
	DWORD dwSampleStateMinFilterPrev = 0;
	DWORD dwSampleStateMagFilterPrev = 0;
	DWORD dwSampleStateMaxAnisotoryPrev = 0;
	DWORD dwRenderOption = 0;
	LPDIRECT3DBASETEXTURE9 pTexturePrev = NULL;
	MeshRenderExtraInfo Info;
	float fWidth = abs(pC->x - pA->x);
	float fHeight = abs(pC->y - pA->y);

	g_pd3dDevice->GetTransform(D3DTS_VIEW, &matViewSave);
	g_pd3dDevice->GetTransform(D3DTS_PROJECTION, &matProjSave);
	pExtInfo = NULL;
	KG3DCameraPreset TestCamera;
	static KG3DCameraPreset::enuCameraPresetType Type = KG3DCameraPreset::CPT_FRONT;
	TestCamera.GetPresetCamera(this, Type, fWidth / fHeight);
	TestCamera.SetCamera(Type); 

	g_pd3dDevice->GetTexture(0, &pTexturePrev);
	if (pTexturePrev)
	{
		pTexturePrev->Release();
	}
	g_pd3dDevice->GetSamplerState(0, D3DSAMP_MAGFILTER, &dwSampleStateMagFilterPrev);
	g_pd3dDevice->GetSamplerState(0, D3DSAMP_MINFILTER, &dwSampleStateMinFilterPrev);
	g_pd3dDevice->GetSamplerState(0, D3DSAMP_MAXANISOTROPY,&dwSampleStateMaxAnisotoryPrev);

	pTexture = g_cTextureTable.GetRenderTargetTexture(
		(unsigned)fWidth,
		(unsigned)fHeight,
		D3DFMT_A8R8G8B8);
	KG_PROCESS_ERROR(pTexture);

	KG_PROCESS_ERROR(m_pBip);

	pController = GetAnimationController(ANICTL_PRIMARY);
	Info.Data = m_pBip->GetObjMatricesArray();
	Info.dwType = ENUM_MESHRENDER_EXTR_INFO_MATRIX;
	
	if (pController->HasAnimation())
	{
		pExtInfo = &Info;
		dwRenderOption = RENDER_BIP;
	}

	size_t uSize = m_Models.size();
	for (size_t i = 0; i < uSize; i++)
	{
		if (!m_Models[i].pModelCurrent)
			continue;

		m_Models[i].pModelCurrent->RenderToTexture(static_cast<DWORD>(KG3DTextureTable::STID_RENDERTARGET), 
			pA,
			pC,
			pUVA,
			pUVC,
			fZ,
			dwFlags,
			dwBackColor,
			dwRenderOption,
			pExtInfo);
		dwFlags = 0;
	}
	
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, (D3DTEXTUREFILTERTYPE)g_cEngineOption.nSampMinFilter);
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, (D3DTEXTUREFILTERTYPE)g_cEngineOption.nSampMagFilter);
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_MAXANISOTROPY ,g_cEngineOption.dwMaxAnisotropy);

	g_cGraphicsTool.DrawScreenRect(pA, pC, fZ, 1.0f, 1.0f, 0xffffffff, static_cast<DWORD>(KG3DTextureTable::STID_RENDERTARGET));



	hr = S_OK;
Exit0:
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, dwSampleStateMagFilterPrev);
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, dwSampleStateMinFilterPrev);
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_MAXANISOTROPY,dwSampleStateMaxAnisotoryPrev);

	g_pd3dDevice->SetTransform(D3DTS_VIEW, &matViewSave);
	g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &matProjSave);
	return hr;
}


ULONG STDMETHODCALLTYPE KG3DModelST::AddRef()
{
    return (ULONG)KG_InterlockedIncrement((long *)&m_ulRefCount);
}

void KG3DModelST::SetHighLightLevel(float fIllumination, BOOL bInherit)
{
	m_fIlluminationLevel = fIllumination;

	for (size_t i = 0; i < m_Models.size(); i++)
	{
		if (m_Models[i].pModelCurrent)
		{
			m_Models[i].pModelCurrent->SetHighLightLevel(fIllumination, bInherit);
		}
	}
}

HRESULT KG3DModelST::SetAlpha(float fAlpha, BOOL bUseSpecail)
{
	BOOL bIsPassRecord = FALSE;
	if (g_cRecorder.IsRecord() && m_dwRecordID != 0 && !g_cRecorder.IsLock())
	{
		KG3DRecorder::Parameters MyParameters;
		MyParameters.nNum = 2;
		MyParameters.size_tDataSize = sizeof(fAlpha) + sizeof(bUseSpecail);
		char* pData = new char[MyParameters.size_tDataSize];
		MyParameters.pData = pData;
		MyParameters.nFrom[0] = 0;
		*(float*)(&pData[MyParameters.nFrom[0]]) = fAlpha;
		MyParameters.nFrom[1] = sizeof(fAlpha);
		*(BOOL*)(&pData[MyParameters.nFrom[1]]) = bUseSpecail;
		g_cRecorder.RecordEvent(m_dwRecordID,FT_KG3DMODEL_SETALPHA,
			GetCurrentTime(),MyParameters);
		bIsPassRecord = TRUE;
		g_cRecorder.Lock();
	}
	
	KG3DModel::SetAlpha(fAlpha, bUseSpecail);

	for (size_t i = 0; i < m_Models.size(); i++)
	{
		if (m_Models[i].pModelHigh)
		{
			m_Models[i].pModelHigh->SetAlpha(fAlpha, bUseSpecail);
		}
		if (m_Models[i].pModelMedium)
		{
			m_Models[i].pModelMedium->SetAlpha(fAlpha, bUseSpecail);
		}
		if (m_Models[i].pModelLow)
		{
			m_Models[i].pModelLow->SetAlpha(fAlpha, bUseSpecail);
		}
	}

	if (bIsPassRecord)
		g_cRecorder.UnLock();
	return S_OK;
}

HRESULT KG3DModelST::EnableWarper(BOOL bEnable) 
{
	HRESULT hrResult = E_FAIL;
    HRESULT hrRetCode = E_FAIL;

    m_Splitter.SetModel(this);
    KG3DAnimationWarper *pWarper = m_Splitter.GetAnimationWarper();
    hrRetCode = pWarper->Enable(TRUE);
    KG_COM_PROCESS_ERROR(hrRetCode);

	hrResult = S_OK;
Exit0:
	return hrResult;
}

HRESULT KG3DModelST::UnInit() 
{
	SAFE_DELETE(m_lpSkinMeshUion);
	for(size_t i = 0; i < m_Models.size(); i++)
	{
		if(m_Models[i].pModelHigh)
		{
			m_Models[i].pModelHigh->m_lpParentModelST = NULL;
			m_Models[i].pModelHigh->SetParentObjMatrix(NULL);
		}
		if(m_Models[i].pModelMedium)
		{
			m_Models[i].pModelMedium->m_lpParentModelST = NULL;
		}
		if(m_Models[i].pModelLow)
		{
			m_Models[i].pModelLow->m_lpParentModelST = NULL;
		}
		SAFE_RELEASE(m_Models[i].pModelHigh);
		SAFE_RELEASE(m_Models[i].pModelMedium);
		SAFE_RELEASE(m_Models[i].pModelLow);
	}
	m_Models.clear();
    RelaseBip();

	for (DWORD i = 0; i < ANICTL_COUNT; i++)
	{
		m_Controllers[i].Detach();
	}

	m_nNumModelNeedToAttach = 0;
	KG3DModel::UnInit();
	return S_OK;
}


KG3DBip* KG3DModelST::GetBip()
{
	return m_pBip;
}

HRESULT KG3DModelST::GetModel(unsigned int uIndex, IKG3DModel** ppModel)
{
	HRESULT hr = E_FAIL;
	KG_PROCESS_ERROR(uIndex < m_Models.size());
	KG_PROCESS_ERROR(ppModel);

	*ppModel = m_Models[uIndex].pModelCurrent;
	hr = S_OK;
Exit0:
	return hr;
}

HRESULT KG3DModelST::GetModelEx(unsigned int uIndex, KG3DModel** ppModelCurrent,KG3DModel** ppModelHigh)
{
	HRESULT hr = E_FAIL;
	KG_PROCESS_ERROR(uIndex < m_Models.size());
	KG_PROCESS_ERROR(ppModelCurrent);
	KG_PROCESS_ERROR(ppModelHigh);

	*ppModelCurrent = m_Models[uIndex].pModelCurrent;
	*ppModelHigh    = m_Models[uIndex].pModelHigh;

	hr = S_OK;
Exit0:
	return hr;
}


HRESULT KG3DModelST::ReflushPhysicsSys()
{
	if (m_lpPhysicsScene && g_cEngineOption.bEnablePhysics)
	{
        KG3DPhysiscScene* lpPhysiscScene = m_lpPhysicsScene;
        SetPhysicsScene(NULL);
        SetPhysicsScene(lpPhysiscScene);
	}

	return S_OK;
}

HRESULT KG3DModelST::ChangeModelProcess(KG3DModel* pModel)
{
	if (pModel)
	{
		pModel->AddRef();
		pModel->SetPhysicsScene(NULL);
		pModel->SetParentObjMatrix(&m_matWorld);
		pModel->AttachBip(m_pBip);
		pModel->FrameMove();
		pModel->ComputeBBox();
		pModel->m_lpParentModelST = this;
	}
	return S_OK;
}

HRESULT KG3DModelST::ChangeModel(unsigned int uIndex, IKG3DModel* pModel)
{
	HRESULT hr = E_FAIL;
	KG3DModel* pTemp = static_cast<KG3DModel*>(pModel);
	KG_PROCESS_ERROR(uIndex < m_Models.size());
	
	KG3DModel* pTempMedium = NULL;
	KG3DModel* pTempLow    = NULL;
	if (pTemp)
	{
		pTempMedium = pTemp->m_lpModelMedium;
		pTempLow = pTemp->m_lpModelLow;
	}

	if (m_Models[uIndex].pModelHigh)
	{
		if (pTemp)
		{
			pTemp->CopyBindInfo(m_Models[uIndex].pModelHigh);
		}
		m_Models[uIndex].pModelHigh->m_lpParentModelST = NULL;
		//不能使用Detach, 因为detach会破坏模型的顺序
        m_Models[uIndex].pModelHigh->SetPhysicsScene(NULL);
		m_Models[uIndex].pModelHigh->SetParentObjMatrix(NULL);
		SAFE_RELEASE(m_Models[uIndex].pModelHigh);
	}
	if (m_Models[uIndex].pModelMedium && pTempMedium)
	{
		pTempMedium->CopyBindInfo(m_Models[uIndex].pModelMedium);
		m_Models[uIndex].pModelMedium->m_lpParentModelST = NULL;
		//不能使用Detach, 因为detach会破坏模型的顺序
		m_Models[uIndex].pModelMedium->SetPhysicsScene(NULL);
		m_Models[uIndex].pModelMedium->SetParentObjMatrix(NULL);
		SAFE_RELEASE(m_Models[uIndex].pModelMedium);
	}
	if (m_Models[uIndex].pModelLow && pTempLow)
	{
		pTempLow->CopyBindInfo(m_Models[uIndex].pModelLow);
		m_Models[uIndex].pModelLow->m_lpParentModelST = NULL;
		//不能使用Detach, 因为detach会破坏模型的顺序
		m_Models[uIndex].pModelLow->SetPhysicsScene(NULL);
		m_Models[uIndex].pModelLow->SetParentObjMatrix(NULL);
		SAFE_RELEASE(m_Models[uIndex].pModelLow);
	}

	ChangeModelProcess(pTemp);
	if(pTempMedium)
		ChangeModelProcess(pTempMedium);
	if(pTempLow)
		ChangeModelProcess(pTempLow);
	m_Models[uIndex].pModelHigh = pTemp;
	m_Models[uIndex].pModelMedium = pTempMedium;
	m_Models[uIndex].pModelLow = pTempLow;
	m_Models[uIndex].pModelCurrent = pTemp;
	if (pTemp)
	{
		pTemp->SetPhysicsScene(NULL);
		pTemp->SetParentObjMatrix(&m_matWorld);
	
		m_Models[uIndex].pModelHigh->AttachBip(m_pBip);
		pTemp->m_lpParentModelST = this;
		pTemp->FrameMove();
		pTemp->ComputeBBox();
		pTemp->SetClientAdd(m_bClientAdd);
	}
	ReflushPhysicsSys();

	hr = S_OK;
Exit0:
	return hr;
}

vector<KG3DModel*> KG3DModelST::GetAllChilds()
{
	vector<KG3DModel*> vecChilds = KG3DModel::GetAllChilds();
	vecChilds.reserve(vecChilds.size() + m_Models.size());	//预分配内存能加速
	for (size_t i = 0; i < m_Models.size(); i++)
	{
		if (m_Models[i].pModelCurrent)
		{
			vector<KG3DModel*> vecTemp = m_Models[i].pModelCurrent->GetAllChilds();
			for (size_t j = 0; j < vecTemp.size(); j++)
			{
				vecChilds.push_back(vecTemp[j]);
			}
		}
	}

	return vecChilds;
}

HRESULT KG3DModelST::GetAllChilds(vector<IEKG3DModel*> &vecpChildsModel)
{
	HRESULT hr = E_FAIL;
	vecpChildsModel.reserve(vecpChildsModel.size() + m_Models.size());//预分配内存能加速
	for (size_t i = 0; i < m_Models.size(); i++)
	{
		if (m_Models[i].pModelCurrent)
		{
			hr = m_Models[i].pModelCurrent->GetAllChilds(vecpChildsModel);
			KG_COM_PROCESS_ERROR(hr);
		}
	}
Exit0:
	return hr;
}

HRESULT KG3DModelST::GetFirstChildModel(IEKG3DModel **ppModel)
{
	HRESULT hResult = E_FAIL;

	m_vecChilds.clear();
	vector<KG3DModel*>vecModels = GetAllChilds();

	for (size_t i=0;i<vecModels.size();i++)
	{
		KG3DModelLod lod;
		lod.pModelHigh = vecModels[i];
		m_vecChilds.push_back(lod);
	}

	m_nCurrChild = 0;

	KG_PROCESS_ERROR(m_vecChilds.size());

	*ppModel = m_vecChilds[m_nCurrChild++].pModelHigh;

	hResult = S_OK;
Exit0:
	if (FAILED(hResult))
		*ppModel = NULL;
	return hResult;
}

HRESULT KG3DModelST::GetNextChildModel(IEKG3DModel **ppModel)
{
	HRESULT hResult = E_FAIL;

	KG_PROCESS_ERROR(m_nCurrChild < static_cast<int>(m_vecChilds.size()));

	*ppModel = m_vecChilds[m_nCurrChild++].pModelHigh;

	hResult = S_OK;
Exit0:
	if (FAILED(hResult))
		*ppModel = NULL;
	return hResult;
}


D3DXMATRIX* KG3DModelST::GetBoneAnimationResultPool()
{
	D3DXMATRIX *pReturn = NULL;
	KG_PROCESS_ERROR(m_pBip);

	pReturn = m_pBip->GetObjMatricesArray();
Exit0:
	return pReturn;
}

HRESULT KG3DModelST::GetNumBones(INT* pRetNumBones)
{
	HRESULT hr = E_FAIL;
	KG_PROCESS_ERROR(pRetNumBones);
	*pRetNumBones = m_pBip->GetNumBones();
	hr = S_OK;
Exit0:
	return hr;
}

HRESULT KG3DModelST::GetNumSocket(INT *pRetNumSocket)
{
	HRESULT hr = E_FAIL;
	INT nNumSocket = 0;

	KG_PROCESS_ERROR(pRetNumSocket);

	*pRetNumSocket = 0;
	for (size_t i = 0; i < m_Models.size(); i++)
	{
		if (m_Models[i].pModelCurrent)
		{
			m_Models[i].pModelCurrent->GetNumSocket(&nNumSocket);
			*pRetNumSocket+= nNumSocket;
		}
	}

	hr = S_OK;
Exit0:
	return hr;
}

LPCSTR KG3DModelST::GetBoneName(int nIndex)
{
	LPCSTR strReturn = NULL;
	KG_PROCESS_ERROR(m_pBip);

	strReturn = m_pBip->GetBoneName(nIndex);
Exit0:
	return strReturn;
}

HRESULT KG3DModelST::GetSocketName(int nIndex, LPCTSTR *ppRetName)
{
	HRESULT hr = E_FAIL;
	int nSocket = 0;
	INT nNumSocket = 0;

	KG_PROCESS_ERROR(ppRetName);
	for (size_t i = 0; i < m_Models.size(); i++)
	{
		if (m_Models[i].pModelCurrent)
		{
			int nLastNumSocket = nSocket;
			m_Models[i].pModelCurrent->GetNumSocket(&nNumSocket);
			nSocket += nNumSocket;
			if (nSocket > nIndex)
			{
				m_Models[i].pModelCurrent->GetSocketName(nIndex - nLastNumSocket, ppRetName);
				break;
			}
		}
	}
	hr = S_OK;
Exit0:
	return hr;
}


HRESULT KG3DModelST::UnbindChild(const TCHAR* strBindObjName)
{
	KG_PROCESS_ERROR(strBindObjName);
	for (size_t i = 0; i < m_Models.size(); i++)
	{
		if (m_Models[i].pModelCurrent)
		{
			m_Models[i].pModelCurrent->UnbindChild(strBindObjName);
		}
	}
	if (strBindObjName[0] == '\0')
	{
		std::set<KG3DModelLod>::iterator it = m_ChildModels.begin();
		while (it != m_ChildModels.end())
		{
			KG3DModelLod& Lod = *it;

			KG3DModel *pChild = Lod.pModelHigh;
			if (pChild)
			{
				pChild->UnBindFromOther();
			}
		}
	}
Exit0:
	;
	return S_OK;
}

HRESULT KG3DModelST::_SetBoneTransform(LPCSTR strBoneName,
									  const BoneTransformationInputData& Data)
{
	HRESULT hrResult = E_FAIL;
    HRESULT hrRetCode = E_FAIL;

	KGLOG_PROCESS_ERROR(!m_bInMultiThreadLoadList);
	KGLOG_PROCESS_ERROR(m_pBip);

	EnableBoneTransform(TRUE);

	KG3DAnimationWarper* pWarper = m_Splitter.GetAnimationWarper();
	ASSERT(pWarper);
    
    if (pWarper->GetEnable())
    {
        hrRetCode = pWarper->SetBoneTransformation(strBoneName, Data);
        KG_COM_PROCESS_ERROR(hrRetCode);
    }
    else
    {
        hrRetCode = m_pBip->SetBoneTransform(strBoneName, Data);
        KG_COM_PROCESS_ERROR(hrRetCode);
    }

	hrResult = S_OK;
Exit0:
	return hrResult;
}

HRESULT KG3DModelST::SetBoneTransform(LPCSTR strBoneName,
									  const BoneTransformationInputData& Data)
{
	HRESULT hr = E_FAIL;

	if(IsLoaded())
	{
		hr = _SetBoneTransform(strBoneName,Data);
	}
	else
	{
		float K = *static_cast<const float*>(Data.Data);
		KG3DModelCommand_KG3DModelST_SetBoneTransform* pCmd = new KG3DModelCommand_KG3DModelST_SetBoneTransform(strBoneName,K);
		m_Commands.AddCommand(pCmd,FALSE);
		hr = S_OK;
	}
	
	return hr;
}

void KG3DModelST::EnableBoneTransform(BOOL bEnable)
{
	KG_PROCESS_ERROR(m_pBip);

	KG3DAnimationWarper* pWarper = m_Splitter.GetAnimationWarper();
	ASSERT(pWarper);

    if (pWarper->GetEnable())
	    pWarper->EnableBoneTransformEnable(bEnable);
    else
        m_pBip->EnableBoneTransform(bEnable);

Exit0:
	return;
}

BOOL KG3DModelST::IsBoneTransformEnable()
{
	KG3DAnimationWarper* pWarper = m_Splitter.GetAnimationWarper();
	ASSERT(pWarper);
	KG_PROCESS_ERROR(m_pBip);
	if (pWarper->GetEnable())
	{
		return pWarper->IsBoneTransformEnable();
	}
	else
	{
		return m_pBip->IsBoneTransformEnable();
	}

Exit0:
	return FALSE;
}

HRESULT KG3DModelST::IEChangeModel(DWORD dwIndex, IKG3DModel* pModel)
{
	HRESULT hResult = E_FAIL;
	KGLOG_PROCESS_ERROR(pModel);

	unsigned uIndex = (unsigned)(dwIndex);

	ChangeModel(uIndex, pModel);

	hResult = S_OK;
Exit0:
	return hResult;

}
HRESULT KG3DModelST::GetIKG3DModel(IKG3DModel **ppiModel)
{
	HRESULT hResult = E_FAIL;
	KGLOG_PROCESS_ERROR(ppiModel);

	*ppiModel = this;

	hResult = S_OK;
Exit0:
	return hResult;

}
void KG3DModelST::GetRenderEnviromentParamenter(IKG3DScene* pScene)
{
	KG_PROCESS_ERROR(pScene);
	switch(pScene->GetType())
	{
	case SCENETYPE_SCENEEDITOR:
		{
			KG3DSceneSceneEditor* pEditor = static_cast<KG3DSceneSceneEditor*>(pScene);
			KG3DSceneSceneEditor::EnviromentParameter EnvPara;
			KG3DRuntimeMaterial* pRuntimeMaterial = NULL;
			D3DXVECTOR3 vec3Pos(m_matWorld._41, m_matWorld._42, m_matWorld._43);
			KG_PROCESS_ERROR(pEditor);
			pEditor->GetEnviromentParamenter(vec3Pos, EnvPara);

			m_RuntimeMaterial.SetDiffuse(EnvPara.Diffuse);
			m_RuntimeMaterial.SetLightDiffuse(0, EnvPara.LightDiffuse);
			m_RuntimeMaterial.SetLightAmbient(EnvPara.LightAmbient);

			for (size_t i = 0; i < m_Models.size(); i++)
			{
				if (!m_Models[i].pModelCurrent)
					continue;
				
				pRuntimeMaterial = m_Models[i].pModelCurrent->GetRuntimeMaterial();
				ASSERT(pRuntimeMaterial);

				pRuntimeMaterial->SetDiffuse(EnvPara.Diffuse);
				pRuntimeMaterial->SetLightDiffuse(0, EnvPara.LightDiffuse);
				pRuntimeMaterial->SetLightAmbient(EnvPara.LightAmbient);
				m_Models[i].pModelCurrent->UpdateChildRuntimMaterial();
			}

			UpdateChildRuntimMaterial();
		}
		break;
	default:
		;
	}
Exit0:
	;
}

HRESULT KG3DModelST::SaveToMDLFile(const char cszFileName[], unsigned uOption)
{
	HRESULT hr = E_FAIL;
	FILE* fpFile = NULL;
	TCHAR strRelativePath[MAX_PATH];
	TCHAR strMtlRelativePath[MAX_PATH];
	KG_PROCESS_ERROR(cszFileName);

	fpFile = fopen(cszFileName, "w");
	KG_PROCESS_ERROR(fpFile);

	const KG3DBip* pBip = GetBip();
	KG_PROCESS_ERROR(pBip);

	const TCHAR* strBipFileName = pBip->GetBipFileName();
	KG_PROCESS_ERROR(strBipFileName);

	g_pFileFinder->GetRelativePath(g_Def_WorkDirectory, strBipFileName, strRelativePath);
	if (strRelativePath[0] != '\\')
	{
		fprintf_s(fpFile, 
			"\\%s\n",
			strRelativePath);
	}
	else
	{
		fprintf_s(fpFile,
			"%s\n",
			strRelativePath);
	}

	for (size_t i = 0; i < m_Models.size(); i++)
	{
		if (m_Models[i].pModelHigh)
		{
			g_pFileFinder->GetRelativePath(g_Def_WorkDirectory, 
				m_Models[i].pModelHigh->m_scName.c_str(), 
				strRelativePath);
			g_pFileFinder->GetRelativePath(g_Def_WorkDirectory, 
				m_Models[i].pModelHigh->m_lpMaterial->m_scName.c_str(), 
				strMtlRelativePath);

			if (strRelativePath[0] != '\\')
			{
				fprintf_s(fpFile, 
					"\\%s",
					strRelativePath);

			}
			else
			{
				fprintf_s(fpFile,
					"%s",
					strRelativePath);
			}

			if (ComparePath(strRelativePath,strMtlRelativePath,'.'))
			{
				fprintf_s(fpFile,"\n");
			}
			else
			{
				if (strMtlRelativePath[0] != '\\')
				{
					fprintf_s(fpFile, 
						"  \\%s\n",
						strMtlRelativePath);
				}
				else
				{
					fprintf_s(fpFile,
						"  %s\n",
						strMtlRelativePath);
				}
			}
			m_Models[i].pModelHigh->SaveToFile(m_Models[i].pModelHigh->m_scName.c_str(), 0);
		}
	}
	hr = S_OK;
Exit0:
	if (fpFile)
	{
		fclose(fpFile);
		fpFile = NULL;
	}
	return hr;
}

HRESULT KG3DModelST::SaveToFile(const char cszFileName[], unsigned uOption)
{
	TCHAR strExt[MAX_PATH];
	_splitpath_s(cszFileName, 
		NULL,
		0, 
		NULL,
		0, 
		NULL,
		0, 
		strExt, 
		MAX_PATH);
	_strlwr_s(strExt, MAX_PATH * sizeof(TCHAR));

	if(_strcmpi(strExt,".ModelST")==0)
	{
		return SaveCompactToModelSTFile(cszFileName,"Default",uOption);
	}
	else
	{
		return SaveToMDLFile(cszFileName,uOption);
	}
}

int KG3DModelST::GetNumModel()
{
	return static_cast<int>(m_Models.size());
}

bool KG3DModelST::ComparePath(const char cszFileName1[],const char cszFileName2[],const char sign)
{
	bool bResult = false;
	KG_PROCESS_ERROR(cszFileName1);
	KG_PROCESS_ERROR(cszFileName2);
	TCHAR strPath1[MAX_PATH];
	TCHAR strPath2[MAX_PATH];
	TCHAR* strFind;
	strcpy(strPath1,cszFileName1);
	strcpy(strPath2,cszFileName2);
	strFind = strrchr(strPath1,sign);
	if (strFind)
	{
		strFind[0] = '\0';
	}
	strFind = strrchr(strPath2,sign);
	if (strFind)
	{
		strFind[0] = '\0';
	}
	strlwr(strPath1);
	strlwr(strPath2);
	if (strcmp(strPath1,strPath2) == 0)
	{
		bResult = true;
	}
Exit0:
	return bResult;
}

int KG3DModelST::GetBindToSocketModel(const char cszSocketName[],vector<IEKG3DModel*> &vecpModel)
{
	int nCount = 0;
	KG_PROCESS_ERROR(cszSocketName);
	for (size_t i = 0; i < m_Models.size(); i++)
	{
		if (m_Models[i].pModelCurrent)
		{
			int n = m_Models[i].pModelCurrent->GetBindToSocketModel(cszSocketName , vecpModel);
			nCount += n;
		}
	}
Exit0:
	return nCount;
}

int KG3DModelST::GetBindToSocketModel(const char cszSocketName[],vector<IKG3DModel*> &vecpModel)
{
	int nCount = 0;
	KG_PROCESS_ERROR(cszSocketName);
	for (size_t i = 0; i < m_Models.size(); i++)
	{
		if (m_Models[i].pModelHigh)
		{
			int n = m_Models[i].pModelHigh->GetBindToSocketModel(cszSocketName , vecpModel);
			nCount += n;
		}
	}
Exit0:
	return nCount;
}

HRESULT KG3DModelST::GetIEBip(IEKG3DBip **ppiBip)
{
	HRESULT hResult = E_FAIL;      
	KGLOG_PROCESS_ERROR(ppiBip);
	*ppiBip = m_pBip;
	hResult = S_OK;
Exit0:
	return hResult;
}

HRESULT KG3DModelST::_AttachModelProcess(KG3DModel* pDefaultModel)
{
	pDefaultModel->AddRef();
	pDefaultModel->SetParentObjMatrix(&m_matWorld);

	pDefaultModel->FrameMove();
	pDefaultModel->ComputeBBox();

	m_BBoxLocal.AddPosition(pDefaultModel->m_BBoxLocal.A);
	m_BBoxLocal.AddPosition(pDefaultModel->m_BBoxLocal.B);	
	
	pDefaultModel->m_lpParentModelST = this;
	pDefaultModel->SetPhysicsScene(m_lpPhysicsScene);
	
	pDefaultModel->SetClientAdd(m_bClientAdd);

	return S_OK;
}

int KG3DModelST::_AttachModel(KG3DModel *pModelHigh,KG3DModel *pModelMedium,KG3DModel *pModelLow)
{
	int nReturn = -1;

	KG3DModelLod lod;
	lod.pModelHigh = pModelHigh;
	lod.pModelMedium = pModelMedium;
	lod.pModelLow = pModelLow;

	if(pModelLow)
	{
		*(KG3DTransformation*)pModelLow   = *(KG3DTransformation*)pModelHigh;
		_AttachModelProcess(pModelLow);
		lod.pModelCurrent = pModelLow;
	}
	if(pModelMedium)
	{
		*(KG3DTransformation*)pModelMedium = *(KG3DTransformation*)pModelHigh;
		_AttachModelProcess(pModelMedium);
		lod.pModelCurrent = pModelMedium;
	}
	if(pModelHigh)
	{
		_AttachModelProcess(pModelHigh);
		lod.pModelCurrent = pModelHigh;
	}

	m_Models.push_back(lod);
	nReturn = static_cast<int>(m_Models.size() - 1);

	m_fShadowScaleFactor = ComputeShadowScaleFactor(m_BBoxLocal);
	//////////////////////////////////////////////////////////////////////////

	if (m_pBip)
	{
		if(pModelHigh)
			nReturn = pModelHigh->AttachBip(m_pBip);
		if(pModelMedium)
			nReturn = pModelMedium->AttachBip(m_pBip);
		if(pModelLow)
			nReturn = pModelLow->AttachBip(m_pBip);
	}
	else
	{
		ASSERT(0);
	}
	ComputeBBox();

	return nReturn;
}

int KG3DModelST::AttachModel(IKG3DModel *pModel)
{
	int nReturn = -1;
	BOOL bIsPassRecord = FALSE;

	unsigned int uType = pModel->GetType();
	KGLOG_PROCESS_ERROR(uType == MESHTYPE_DEFAULT);

	KG3DModel* pDefaultModel = static_cast<KG3DModel*>(pModel);
	if (g_cRecorder.IsRecord() && m_dwRecordID != 0 && pDefaultModel->m_dwRecordID != 0 && !g_cRecorder.IsLock())
	{
		KG3DRecorder::Parameters MyParameters;
		MyParameters.nNum = 1;
		MyParameters.size_tDataSize = sizeof(DWORD);
		char* pData = new char[MyParameters.size_tDataSize];
		MyParameters.pData = pData;
		MyParameters.nFrom[0] = 0;
		*(DWORD*)(&pData[MyParameters.nFrom[0]]) = pDefaultModel->m_dwRecordID;
		g_cRecorder.RecordEvent(m_dwRecordID,FT_KG3DMODEL_ATTACHMODEL,GetCurrentTime(),MyParameters);
		bIsPassRecord = TRUE;
		g_cRecorder.Lock();
	}
	//////////////////////////////////////////////////////////////////////////

	if( IsLoaded() && pDefaultModel->IsReadyToAttchModelST() )
	{
		KG3DModel* pModelM = pDefaultModel->m_lpModelMedium;
		KG3DModel* pModelL = pDefaultModel->m_lpModelLow;

		nReturn = _AttachModel((KG3DModel*)pModel,pModelM,pModelL);
	}
	else
	{
		KG3DModelCommand_Attach* pCMD = new KG3DModelCommand_Attach(pDefaultModel);
		m_Commands.AddCommand(pCMD,TRUE);
		m_nNumModelNeedToAttach++;
		nReturn = 0;
	}

Exit0:
	ASSERT(nReturn != -1);
	if (bIsPassRecord)
		g_cRecorder.UnLock();
	return nReturn;
}


HRESULT KG3DModelST::ComputeBBoxLocal()
{
	m_BBoxLocal.Clear();
	for (size_t i = 0; i < m_Models.size(); ++i)
	{
		if (m_Models[i].pModelCurrent)
		{
			m_BBoxLocal.AddPosition(m_Models[i].pModelCurrent->m_BBoxLocal.A);
			m_BBoxLocal.AddPosition(m_Models[i].pModelCurrent->m_BBoxLocal.B);
		}
	}

	return S_OK;
}

void KG3DModelST::DetachModel(IKG3DModel* pModel)
{
	KG3DModel* pRecordModel = static_cast<KG3DModel*>(pModel);
	BOOL bIsPassRecord = FALSE;
	if (pRecordModel && g_cRecorder.IsRecord() && m_dwRecordID != 0 && pRecordModel->m_dwRecordID != 0 && !g_cRecorder.IsLock())
	{
		KG3DRecorder::Parameters MyParameters;
		MyParameters.nNum = 1;
		MyParameters.size_tDataSize = sizeof(DWORD);
		char* pData = new char[MyParameters.size_tDataSize];
		MyParameters.pData = pData;
		MyParameters.nFrom[0] = 0;
		*(DWORD*)(&pData[MyParameters.nFrom[0]]) = pRecordModel->m_dwRecordID;
		g_cRecorder.RecordEvent(m_dwRecordID,FT_KG3DMODEL_DETCHMODEL,GetCurrentTime(),MyParameters);
		bIsPassRecord = TRUE;
		g_cRecorder.Lock();
	}

	if(IsLoaded() && pRecordModel->IsLoaded())
	{
		_DetachModel(pModel);
	}
	else
	{
		KG3DModelCommand_DetachModel* pCMD = new KG3DModelCommand_DetachModel(pRecordModel);
		m_Commands.AddCommand(pCMD,TRUE);
	}

	if (bIsPassRecord)
		g_cRecorder.UnLock();
}

void KG3DModelST::_DetachModelProcess(KG3DModel* pModel)
{
	if(pModel)
	{
		pModel->m_lpParentModelST = NULL;
		pModel->SetParentObjMatrix(NULL);
		pModel->SetPhysicsScene(NULL);
		pModel->EnableHideFromRenderTools(TRUE);
	}
	SAFE_RELEASE(pModel);
}

void KG3DModelST::_DetachModel(IKG3DModel* pModel)
{
	D3DXVECTOR3 vec3PosBuffer[8];
	for(size_t i = 0; i < m_Models.size(); i++)
	{
		if (static_cast<KG3DModel*>(pModel) == m_Models[i].pModelHigh)
		{
			_DetachModelProcess(m_Models[i].pModelHigh);
			_DetachModelProcess(m_Models[i].pModelMedium);
			_DetachModelProcess(m_Models[i].pModelLow);
			
			m_Models[i] = m_Models[m_Models.size() - 1];
			m_Models.pop_back();
			break;
		}
	}

	ComputeBBoxLocal();
	m_BBox.Clear();

	m_BBoxLocal.TransformCoord(vec3PosBuffer, &(m_matWorld));
	m_BBox.AddPosition(vec3PosBuffer, 8);

	m_fShadowScaleFactor = this->ComputeShadowScaleFactor(m_BBoxLocal);

}

void KG3DModelST::SetbDrawBigBox(BOOL val)
{
	m_bDrawBigBox = val;
	for (size_t i = 0; i < m_Models.size(); i++)
	{
		if (m_Models[i].pModelCurrent)
		{
			m_Models[i].pModelCurrent->SetbDrawBigBox(val);
		}
	}
}

BOOL KG3DModelST::GetbDrawBigBox()
{
	return m_bDrawBigBox;
}

HRESULT KG3DModelST::ComputeMeshBigBox()
{
	for (size_t i = 0; i < m_Models.size(); i++)
	{
		if (m_Models[i].pModelCurrent)
		{
			m_Models[i].pModelCurrent->ComputeMeshBigBox();
		}
	}
	return S_OK;
}

HRESULT KG3DModelST::ComputeDynamicBBox()
{
    HRESULT hrResult = E_FAIL;

	for (size_t i = 0; i< m_Models.size();i++)
	{
		if (m_Models[i].pModelCurrent)
		{
			m_Models[i].pModelCurrent->ComputeDynamicBBox();
		}
	}
 
    ComputeBBoxLocal();

    Helper_UpdateTransformation(TRUE);
   
    hrResult = S_OK;
//Exit0:
	return hrResult;
}

HRESULT KG3DModelST::OnReferenceZero()
{
	return KG3DModel::OnReferenceZero();
}

HRESULT KG3DModelST::GetMeshFileName(LPTSTR strFileName)
{
	wsprintf(strFileName,"%s",m_scName.c_str());
	return S_OK;
}

HRESULT KG3DModelST::CreateProgressMesh()
{
	for (size_t i = 0; i < m_Models.size(); i++)
	{
		if (m_Models[i].pModelHigh)
		{
			m_Models[i].pModelHigh->CreateProgressMesh();
		}
	}
	return S_OK;
}

HRESULT KG3DModelST::_SetLOD(float fLod)
{
	for (size_t i = 0; i < m_Models.size(); i++)
	{
		KG3DModelLod& Lod = m_Models[i];
		KG3DModel* pLast = Lod.pModelCurrent;

		if(fLod<=0.33333F && Lod.pModelLow)
		{
			Lod.pModelCurrent = Lod.pModelLow;
		}
		else if(fLod<=0.66666F && Lod.pModelMedium)
		{
			Lod.pModelCurrent = Lod.pModelMedium;
		}
		else
		{
			Lod.pModelCurrent = Lod.pModelHigh;
		}

		if (Lod.pModelCurrent)
		{
			Lod.pModelCurrent->SetLOD(fLod);
		}

		if(Lod.pModelCurrent != Lod.pModelHigh)
		{
			Lod.pModelHigh->SetLOD(fLod);

			if(pLast != Lod.pModelCurrent)
				*(KG3DTransformation*)Lod.pModelCurrent = *(KG3DTransformation*)Lod.pModelHigh;
		}
	}

	if(m_lpSkinMeshUion)
	{
		if(fLod<0.2)
			m_lpSkinMeshUion->m_nUpdateTime = 0;
		else if(fLod<0.5)
			m_lpSkinMeshUion->m_nUpdateTime = 30;
		else 
			m_lpSkinMeshUion->m_nUpdateTime = 100;
	}
	return S_OK;;
}

HRESULT KG3DModelST::SetLOD(float fLod)
{
	KG3DModelCommand_SetLod* pCmd = new KG3DModelCommand_SetLod(fLod);
	KGLOG_PROCESS_ERROR(pCmd);
	m_Commands.AddCommand(pCmd,FALSE);
Exit0:
	return S_OK;
}

void KG3DModelST::SetVisible()
{
	for (size_t i = 0; i < m_Models.size(); i++)
	{
		if (m_Models[i].pModelCurrent)
		{
			m_Models[i].pModelCurrent->SetVisible();
		}
	}
	KG3DModel::SetVisible();
}

//HRESULT KG3DModelST::SetBoneScale(DWORD dwIndex, 
//								  D3DXVECTOR3 *pScale)
//{
//	return S_OK;
//	HRESULT hrResult = E_FAIL;
//	INT nNumBones = 0;
//	KG_PROCESS_ERROR(pScale);
//
//	GetNumBones(&nNumBones);
//	KG_PROCESS_ERROR(dwIndex < static_cast<DWORD>(nNumBones));
//	KG_PROCESS_ERROR(m_pBip);
//
//	D3DXMatrixScaling(&m_pBip->m_BoneLocalScale[dwIndex].matScale,
//		pScale->x,
//		pScale->y,
//		pScale->z);
//	hrResult = S_OK;
//Exit0:
//	return hrResult;
//}
//
//HRESULT KG3DModelST::GetBoneScale(DWORD dwIndex, 
//								  D3DXVECTOR3 *pScale)
//{
//	return S_OK;
//	HRESULT hrResult = E_FAIL;
//	INT nNumBones = 0;
//	KG_PROCESS_ERROR(pScale);
//
//	GetNumBones(&nNumBones);
//	KG_PROCESS_ERROR(dwIndex < static_cast<DWORD>(nNumBones));
//	KG_PROCESS_ERROR(m_pBip);
//
//	pScale->x = m_pBip->m_BoneLocalScale[dwIndex].matScale._11;
//	pScale->y = m_pBip->m_BoneLocalScale[dwIndex].matScale._22;
//	pScale->z = m_pBip->m_BoneLocalScale[dwIndex].matScale._33;
//	hrResult = S_OK;
//Exit0:
//	return hrResult;
//}

BOOL KG3DModelST::IsResourceReady(BOOL bTestText)
{
	BOOL bReturn = IsLoaded();

	for (size_t i = 0; i < m_Models.size(); i++)
	{
		if (m_Models[i].pModelCurrent)
		{
			bReturn &= m_Models[i].pModelCurrent->IsResourceReady(bTestText);
		}
	}

    if (g_bClient)
    {
        bReturn &= m_bHasAnimation;
    }

	return bReturn;
}

BOOL KG3DModelST::IsLoaded()
{
	BOOL bReturn = !m_bInMultiThreadLoadList;

	if(!bReturn)
		return FALSE;

	for (size_t i = 0; i < m_Models.size(); i++)
	{
		if (m_Models[i].pModelCurrent)
		{
			if(!m_Models[i].pModelCurrent->IsLoaded())
				return FALSE;
		}
	}

	return bReturn;
}

void KG3DModelST::SetClientAdd(BOOL bClient)
{
	m_bClientAdd = bClient;
	for (size_t i = 0; i < m_Models.size(); i++)
	{
		if (m_Models[i].pModelCurrent)
		{
			m_Models[i].pModelCurrent->SetClientAdd(bClient);
		}
	}
}

HRESULT KG3DModelST::EnableHideFromRenderTools(BOOL bEnable)
{
	for (size_t i = 0; i < m_Models.size(); i++)
	{
		if (m_Models[i].pModelCurrent)
		{
			m_Models[i].pModelCurrent->EnableHideFromRenderTools(bEnable);
		}
	}

	return KG3DModel::EnableHideFromRenderTools(bEnable);
}

void KG3DModelST::RenderForSelector(DWORD dwOption, void *pExtInfo)
{
	D3DXMATRIX* pRenderMatrix = GetRenderMatrix();
	MeshRenderExtraInfo Info;

	pExtInfo = NULL;
	Info.Data = pRenderMatrix;
	Info.dwType = ENUM_MESHRENDER_EXTR_INFO_MATRIX;

	if (pRenderMatrix)
	{
		dwOption |= RENDER_BIP;
		pExtInfo = &Info;
	}
	for (size_t i = 0; i < m_Models.size(); i++)
	{
		if (m_Models[i].pModelCurrent)
		{
			m_Models[i].pModelCurrent->RenderForSelector(dwOption, pExtInfo);
		}
	}
}

void KG3DModelST::SetSelectIndex(DWORD dwIndex)
{
	KG3DModel::SetSelectIndex(dwIndex);
	for (size_t i = 0; i < m_Models.size(); i++)
	{
		if (m_Models[i].pModelCurrent)
		{
			m_Models[i].pModelCurrent->SetSelectIndex(dwIndex);
		}
	}
}

HRESULT KG3DModelST::LoadPostProcess()
{
    for (size_t i = 0; i < m_Models.size(); i++)
    {
        if (m_Models[i].pModelHigh)
        {
            m_Models[i].pModelHigh->LoadPostProcess();
        }
    }

    return S_OK;
}

HRESULT KG3DModelST::LoadFromFileMultiThread()
{
	_LoadFromFile(m_scName.c_str(),0,m_uOption);

	return S_OK;
}

BOOL KG3DModelST::OnProcessCommands(KG3DModelCommandBase* pCmd)
{
	BOOL bSucceed = FALSE;
	BOOL bTestModel = TRUE;

	switch(pCmd->m_eType)
	{
	case CMD_ATTACH:
		{
			KG3DModelCommand_Attach* pAttach = static_cast<KG3DModelCommand_Attach*>(pCmd);
			KG3DModel* pModelM = pAttach->m_lpModel->m_lpModelMedium;
			KG3DModel* pModelL = pAttach->m_lpModel->m_lpModelLow;

			pAttach->m_lpModel->FrameMove();
			if(pModelL)
				pModelL->FrameMove();
			if(pModelM)
				pModelM->FrameMove();

			if(pAttach->m_lpModel->IsReadyToAttchModelST())
			{

				_AttachModel(pAttach->m_lpModel,pModelM,pModelL);
				m_nNumModelNeedToAttach--;
				bSucceed = TRUE;
			}
			break;
		}
	case CMD_DetachModel:
		{
			KG3DModelCommand_DetachModel* pAttach = static_cast<KG3DModelCommand_DetachModel*>(pCmd);
			if(pAttach->m_lpModel->m_bMeshPostLoadProcessed)
			{
				_DetachModel(pAttach->m_lpModel);
				bSucceed = TRUE;
			}
			break;
		}
    case CMD_SetPhysicsScene:
        {
			if(m_nNumModelNeedToAttach==0)
			{
				KG3DModelCommand_SetPhysicsScene* pPhysics = static_cast<KG3DModelCommand_SetPhysicsScene*>(pCmd);
				_SetPhysicsScene(pPhysics->m_pScene);
				bSucceed = TRUE;		
			}
			bTestModel = FALSE;
            break;
        }
	case CMD_SetDetail:
		{
			if(IsResourceReady(TRUE) && m_nNumModelNeedToAttach==0)
			{
				KG3DModelCommand_SetDetail *pDetail = static_cast<KG3DModelCommand_SetDetail*>(pCmd);
				_SetDetail(pDetail->m_nTable,pDetail->m_nDefine);
				bSucceed = TRUE;
			}
			bTestModel = FALSE;
			break;
		}
	case CMD_KG3DModelST_SetBoneTransform:
		{
			KG3DModelCommand_KG3DModelST_SetBoneTransform* pSet = static_cast<KG3DModelCommand_KG3DModelST_SetBoneTransform*>(pCmd);

			BoneTransformationInputData Data;
			Data.Data = &pSet->m_fBoneTrans;
			_SetBoneTransform(pSet->m_BoneName.c_str(),Data);
            bSucceed = TRUE;
			break;
		}
	case CMD_SetLod:
		{
			KG3DModelCommand_SetLod* pLod = static_cast<KG3DModelCommand_SetLod*>(pCmd);

			_SetLOD(pLod->m_fLod);

			bSucceed = TRUE;
			break;
		}
	}

	if(!bSucceed && bTestModel)
	{
		bSucceed = KG3DModel::OnProcessCommands(pCmd);
	}
	return bSucceed;
}

HRESULT KG3DModelST::LoadMDLContent(LPCSTR strFileName, MDLFileContent& Content)
{
	HRESULT hResult = E_FAIL;
	char szFilePath[MAX_PATH];
	IFile* pFile = NULL;
	unsigned long uSize = 0;
	unsigned long uFileSize = 0;
	char* pBuffer = NULL;
	std::stringstream ss;
	std::string strBuffer;
	std::string strFilePath;
	
	Content.dwNumModels = 0;
	pFile = g_OpenFile(strFileName);
	KG_PROCESS_ERROR(pFile);

	uFileSize = pFile->Size();

	pBuffer = (char*)malloc(uFileSize + 1);
	KG_PROCESS_ERROR(pBuffer);

	uSize = pFile->Read(pBuffer, uFileSize);
	KG_PROCESS_ERROR(uSize == uFileSize);

	pBuffer[uSize] = '\0'; // TODO : text 文件没有使用'\0'作结束，不能作为字符串处理，特别麻烦，建议使用binary

	ss << pBuffer;

	std::getline(ss, strBuffer);
	strBuffer.erase(strBuffer.find_last_not_of("\n\r") + 1);
	KG_PROCESS_ERROR(!strBuffer.empty());

	g_ExtractFilePath(szFilePath, strFileName);

	strFilePath = szFilePath;
	strFilePath += "\\";

	if (strBuffer[0] == '\\')
	{
		Content.strBipFile = strBuffer;
	}
	else
	{
		Content.strBipFile = std::string(strFilePath + strBuffer);
	}

	while (std::getline(ss, strBuffer))
	{
		strBuffer.erase(strBuffer.find_last_not_of("\n\r") + 1);
		if (strBuffer.empty())
			break;

		std::stringstream ssLine(strBuffer);
		std::string strMesh, strMtl;
		ssLine >> strMesh >> strMtl;

		if (strMtl.size())
		{
			Content.strMaterialFile[Content.dwNumModels] = strMtl;
		}

		if (strMesh.size())
		{
			if (strMesh[0] == '\\')
			{
				Content.strMeshFile[Content.dwNumModels] = strMesh;
			}
			else
			{
				Content.strMeshFile[Content.dwNumModels] = strFilePath + strMesh;
			}
			Content.dwNumModels++;
		}
	}
	hResult = S_OK;
Exit0:
	SAFE_FREE(pBuffer);
	KG_COM_RELEASE(pFile);

	if (FAILED(hResult) && strFileName)
	{
		KGLogPrintf(KGLOG_ERR, "Load mdl %s failed.\n", strFileName);
	}
	return hResult;
}

void KG3DModelST::FrameMoveOnce()
{
	if (!m_bFrameMoved)
	{
		FrameMove();
	}
}

void KG3DModelST::PauseAnimation(BOOL bPause)
{
    if (m_Splitter.GetEnable())
    {
        m_Splitter.GetAnimationWarper()->Pause(bPause);
    }
    else
    {
        for (DWORD i = 0; i < ANICTL_COUNT; i++)
        {
            m_Controllers[i].PauseAnimation(bPause);
        }
    }
}

HRESULT KG3DModelST::_SetDetail(int nDefineTable, int nIndex)
{
    HRESULT hResult = E_FAIL;

	m_nDetailTableIndex  = nDefineTable;
	m_nDetailDefineIndex = nIndex;

    for (int i = 0; i < (int)m_Models.size(); i++)
    {
        if (m_Models[i].pModelHigh)
        {
			if(m_Models[i].pModelHigh->m_nDetailDefineIndex==0)
				m_Models[i].pModelHigh->SetDetail(nDefineTable, nIndex);
        }

        if (m_Models[i].pModelMedium)
        {
			if(m_Models[i].pModelMedium->m_nDetailDefineIndex==0)
				m_Models[i].pModelMedium->SetDetail(nDefineTable, nIndex);
        }

        if (m_Models[i].pModelLow)
        {
			if(m_Models[i].pModelLow->m_nDetailDefineIndex==0)
				m_Models[i].pModelLow->SetDetail(nDefineTable, nIndex);
        }
    }

    hResult = S_OK;
//Exit0:
    return hResult;
}

HRESULT KG3DModelST::SetDetail(int nDefineTable, int nIndex)
{
    HRESULT hResult  = E_FAIL;
    HRESULT hRetCode = E_FAIL;

    if (IsResourceReady(TRUE) && m_nNumModelNeedToAttach == 0)
    {
        hRetCode = _SetDetail(nDefineTable, nIndex);
        KG_COM_PROCESS_ERROR(hRetCode);
    }
    else
    {
        KG3DModelCommand_SetDetail* pCmd = new KG3DModelCommand_SetDetail(nDefineTable,nIndex);
        m_Commands.AddCommand(pCmd,FALSE);
    }

    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT KG3DModelST::CreateSkinMeshUion()
{
	HRESULT hr  = E_FAIL;
	SAFE_DELETE(m_lpSkinMeshUion);
	m_lpSkinMeshUion = new KG3DMeshSkin;
	KGLOG_PROCESS_ERROR(m_lpSkinMeshUion);

	hr = m_lpSkinMeshUion->CreateFromModelST(this);
	KGLOG_COM_PROCESS_ERROR(hr);

Exit0:
	return hr;
}

HRESULT KG3DModelST::LoadAndBindModel(LPCSTR pszFileName, LPCSTR pszSocketName)
{
	HRESULT hResult  = E_FAIL;
	HRESULT hRetCode = E_FAIL;
	KG3DModel* pBindModel = NULL;

	KG_PROCESS_ERROR(pszFileName && pszFileName[0] != 0);
	KG_PROCESS_ERROR(pszSocketName && pszSocketName[0] != 0);

	hRetCode = g_cModelTable.LoadResourceFromFile(pszFileName, 
		0, 
		MLO_MODEL_LOD | MLO_TEXTURE_MULTI,
		(IKG3DResourceBase**)&pBindModel);
	KG_COM_PROCESS_ERROR(hRetCode);

	pBindModel->BindToSocket(this, pszSocketName);

	if (!strcmp(pszSocketName, "s_face"))
		pBindModel->SetFaceMotionMode();

	hResult = S_OK;
Exit0:
	return hResult;
}

HRESULT KG3DModelST::LoadCompactFromFileIni(IIniFile* pConfigFile, LPSTR pSectionName,unsigned uOption)
{
	HRESULT hr = E_FAIL;
	vector<IEKG3DModel*>vecBindedModel;

	TCHAR szBipName[MAX_PATH];
	ZeroMemory(szBipName,sizeof(szBipName));

	KGLOG_PROCESS_ERROR(pConfigFile);
	KGLOG_PROCESS_ERROR(pSectionName);


	//////////////////////////////////////////////////////////////////////////
	pConfigFile->GetString(pSectionName,"BipFileName","",szBipName,MAX_PATH);

	KG3DBip *pBipSrc = NULL;
	unsigned uOptionSave = uOption | MLO_MESH_BIP;

	pBipSrc = g_cBipTable.GetBip(szBipName);
	KGLOG_PROCESS_ERROR(pBipSrc);

	ASSERT(!m_pBip);
	m_pBip = new KG3DBip;
	KGLOG_PROCESS_ERROR(m_pBip);

	m_pBip->Clone(pBipSrc);
	m_pBip->OnModelSTAttach(this,m_Controllers);
	m_pBip->SetModelST(this);
	//////////////////////////////////////////////////////////////////////////
	int nPart = 0;
	pConfigFile->GetInteger(pSectionName,"NumPart",0,&nPart);

	for (int i=0;i<nPart;i++)
	{
		TCHAR szMeshName[MAX_PATH];
		TCHAR szMtlName [MAX_PATH];
		TCHAR szAniName [MAX_PATH];
		ZeroMemory(szMeshName,sizeof(szMeshName));
		ZeroMemory(szMtlName ,sizeof(szMtlName ));
		ZeroMemory(szAniName ,sizeof(szAniName ));

		TCHAR KeyName[MAX_PATH];



		wsprintf(KeyName,"Part%d_Mesh",i);
		pConfigFile->GetString(pSectionName,KeyName,"",szMeshName,MAX_PATH);
		wsprintf(KeyName,"Part%d_Mtl",i);
		pConfigFile->GetString(pSectionName,KeyName,"",szMtlName,MAX_PATH);
		wsprintf(KeyName,"Part%d_Ani",i);
		pConfigFile->GetString(pSectionName,KeyName,"",szAniName,MAX_PATH);

		int nDetailTableIndex = 0;
		int nDetailDefineIndex = 0;
		wsprintf(KeyName,"Part%d_Detail",i);
		pConfigFile->GetInteger2(pSectionName,KeyName,
			&nDetailTableIndex,&nDetailDefineIndex);
		//////////////////////////////////////////////////////////////////////////
		if(strcmp(szMeshName,"")!=0)
		{
			KG3DModel *pModel = NULL;
			uOption = uOptionSave;

			hr = g_cModelTable.LoadResourceFromFile(szMeshName, 
				0, 
				uOption, 
				(IKG3DResourceBase **)&pModel);
			KG_COM_PROCESS_ERROR(hr);

			_ASSERTE(pModel);

			if(strcmp(szMtlName,"")!=0)
			{
				hr = pModel->LoadMaterialFromFile(szMtlName, uOption);
				if (FAILED(hr))
				{
					g_OutputDebugInfoFmt( "Failed LoadMaterialFromFile: %s", szMtlName );
					SAFE_RELEASE(pModel);
				}
				KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);
			}

			int n = AttachModel(pModel);

			if(n!=-1)
			{
				m_BBoxLocal.AddPosition(pModel->m_BBox.A);//最后m_BBoxLocal记录的是联合ModelST里面的模型的BBox
				m_BBoxLocal.AddPosition(pModel->m_BBox.B);
			}

			pModel->SetDetail(nDetailTableIndex,nDetailDefineIndex);

			pModel->Release();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	int nBind = 0;
	pConfigFile->GetInteger(pSectionName,"NumBind",0,&nBind);

	for (int i=0;i<nBind;i++)
	{
		TCHAR szMeshName[MAX_PATH];
		TCHAR szMtlName [MAX_PATH];
		TCHAR szAniName [MAX_PATH];
		ZeroMemory(szMeshName,sizeof(szMeshName));
		ZeroMemory(szMtlName ,sizeof(szMtlName ));
		ZeroMemory(szAniName ,sizeof(szAniName ));

		TCHAR KeyName[MAX_PATH];

		wsprintf(KeyName,"Bind%d_Mesh",i);
		pConfigFile->GetString(pSectionName,KeyName,"",szMeshName,MAX_PATH);
		wsprintf(KeyName,"Bind%d_Mtl",i);
		pConfigFile->GetString(pSectionName,KeyName,"",szMtlName,MAX_PATH);
		wsprintf(KeyName,"Bind%d_Ani",i);
		pConfigFile->GetString(pSectionName,KeyName,"",szAniName,MAX_PATH);
		wsprintf(KeyName,"Bind%d_Detail",i);

		int nDetailTableIndex = 0;
		int nDetailDefineIndex = 0;
		pConfigFile->GetInteger2(pSectionName,KeyName,
			&nDetailTableIndex,&nDetailDefineIndex);

		enuModelBindType eBindType = ENUM_BIND_NONE;
		wsprintf(KeyName,"Bind%d_Type",i);
		pConfigFile->GetInteger(pSectionName,KeyName,ENUM_BIND_NONE,(int*)&eBindType);
		wsprintf(KeyName,"Bind%d_Name",i);

		TCHAR szBindObjName[MAX_PATH];
		pConfigFile->GetString(pSectionName,KeyName,"",szBindObjName,MAX_PATH);
		//////////////////////////////////////////////////////////////////////////
		if(strcmp(szMeshName,"")!=0)
		{
			KG3DModel *pModel = NULL;
			uOption = uOptionSave;

			hr = g_cModelTable.LoadResourceFromFile(szMeshName, 
				0, 
				uOption, 
				(IKG3DResourceBase **)&pModel);
			KG_COM_PROCESS_ERROR(hr);

			_ASSERTE(pModel);

			if(strcmp(szMtlName,"")!=0)
			{
				hr = pModel->LoadMaterialFromFile(szMtlName, uOption);
				if (FAILED(hr))
				{
					g_OutputDebugInfoFmt( "Failed LoadMaterialFromFile: %s", szMtlName );
					SAFE_RELEASE(pModel);
				}
				KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);
			}

			if(eBindType == ENUM_BIND_SOCKET)
				pModel->BindToSocket(this,szBindObjName);
			else if(eBindType == ENUM_BIND_BONE)
				pModel->BindToBone(this,szBindObjName);

			pModel->SetDetail(nDetailTableIndex,nDetailDefineIndex);
		}
	}

	ComputeBBox();
	m_fShadowScaleFactor = ComputeShadowScaleFactor(m_BBoxLocal);

	hr = S_OK;
Exit0:
	return hr;
}

HRESULT KG3DModelST::LoadCompactFromModelSTFile(const char cszFileName[], LPSTR pSectionName,unsigned uOption)
{
	HRESULT hr = E_FAIL;
	IIniFile* pFile = g_OpenIniFile(cszFileName);
	KGLOG_PROCESS_ERROR(pFile);

	hr = LoadCompactFromFileIni(pFile,pSectionName,uOption);
	KGLOG_COM_PROCESS_ERROR(hr);

Exit0:
	SAFE_RELEASE(pFile);

	return hr;
}

HRESULT KG3DModelST::SaveCompactToModelSTFile(const char cszFileName[], LPSTR pSectionName,unsigned uOption)
{
	HRESULT hr = E_FAIL;
	IIniFile* pFile = g_OpenIniFile(cszFileName,true,true);
	KGLOG_PROCESS_ERROR(pFile);

	hr = SaveCompactFromFileIni(pFile,pSectionName,uOption);
	KGLOG_COM_PROCESS_ERROR(hr);

	pFile->Save(cszFileName);

Exit0:
	SAFE_RELEASE(pFile);

	return hr;
}

HRESULT KG3DModelST::SaveCompactFromFileIni(IIniFile* pConfigFile, LPSTR pSectionName,unsigned uOption)
{
	HRESULT hr = E_FAIL;
	vector<IEKG3DModel*>vecBindedModel;

	KGLOG_PROCESS_ERROR(pConfigFile);
	KGLOG_PROCESS_ERROR(pSectionName);

	KG3DBip* pBip = GetBip();
	KGLOG_PROCESS_ERROR(pBip);

	//////////////////////////////////////////////////////////////////////////
	pConfigFile->WriteString(pSectionName,"BipFileName",pBip->GetBipFileName());

	int nPart = (int)m_Models.size();
	pConfigFile->WriteInteger(pSectionName,"NumPart",nPart);


	for (size_t i=0;i<m_Models.size();i++)
	{
		TCHAR szMeshName[MAX_PATH];
		TCHAR szMtlName [MAX_PATH];
		TCHAR szAniName [MAX_PATH];
		ZeroMemory(szMeshName,sizeof(szMeshName));
		ZeroMemory(szMtlName ,sizeof(szMtlName ));
		ZeroMemory(szAniName ,sizeof(szAniName ));

		TCHAR KeyName[MAX_PATH];

		KG3DModel* pModelPart = m_Models[i].pModelHigh;
		if(pModelPart)
		{
			pModelPart->GetMeshFileName     (szMeshName);
			pModelPart->GetMaterialFileName (szMtlName );
			pModelPart->GetAnimationFileName(szAniName,MAX_PATH);

			wsprintf(KeyName,"Part%d_Mesh",i);
			pConfigFile->WriteString(pSectionName,KeyName,szMeshName);
			wsprintf(KeyName,"Part%d_Mtl",i);
			pConfigFile->WriteString(pSectionName,KeyName,szMtlName);
			wsprintf(KeyName,"Part%d_Ani",i);
			pConfigFile->WriteString(pSectionName,KeyName,szAniName);
			wsprintf(KeyName,"Part%d_Detail",i);
			pConfigFile->WriteInteger2(pSectionName,KeyName,
				pModelPart->m_nDetailTableIndex,pModelPart->m_nDetailDefineIndex);

			pModelPart->GetAllChilds(vecBindedModel);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	int nBind = (int)vecBindedModel.size();
	pConfigFile->WriteInteger(pSectionName,"NumBind",nBind);

	for (size_t i=0;i<vecBindedModel.size();i++)
	{
		TCHAR szMeshName[MAX_PATH];
		TCHAR szMtlName [MAX_PATH];
		TCHAR szAniName [MAX_PATH];
		ZeroMemory(szMeshName,sizeof(szMeshName));
		ZeroMemory(szMtlName ,sizeof(szMtlName ));
		ZeroMemory(szAniName ,sizeof(szAniName ));

		TCHAR KeyName[MAX_PATH];

		KG3DModel* pModelBind = (KG3DModel*)vecBindedModel[i];
		if(pModelBind)
		{
			pModelBind->GetMeshFileName     (szMeshName);
			pModelBind->GetMaterialFileName (szMtlName );
			pModelBind->GetAnimationFileName(szAniName,MAX_PATH);

			wsprintf(KeyName,"Bind%d_Mesh",i);
			pConfigFile->WriteString(pSectionName,KeyName,szMeshName);
			wsprintf(KeyName,"Bind%d_Mtl",i);
			pConfigFile->WriteString(pSectionName,KeyName,szMtlName);
			wsprintf(KeyName,"Bind%d_Ani",i);
			pConfigFile->WriteString(pSectionName,KeyName,szAniName);
			wsprintf(KeyName,"Bind%d_Detail",i);
			pConfigFile->WriteInteger2(pSectionName,KeyName,
				pModelBind->m_nDetailTableIndex,pModelBind->m_nDetailDefineIndex);

			wsprintf(KeyName,"Bind%d_Type",i);
			pConfigFile->WriteInteger(pSectionName,KeyName,pModelBind->m_BindType);
			wsprintf(KeyName,"Bind%d_Name",i);
			pConfigFile->WriteString(pSectionName,KeyName,pModelBind->m_scBindToObjName.c_str());

		}
	}
	hr = S_OK;
Exit0:
	return hr;
}
