#include "StdAfx.h"
#include "KG3DAnimationMotionTag.h"
#include "KG3DSFX.h"
#include "KG3DFaceMotionSettings.h"
#include "kg3dcliptable.h"
#include "KG3DAnimationController.h"
#include "IEKG3DScene.h"
#include "KG3DGraphicsTool.h"
#include "KG3DSceneSceneEditor.h"
#include "..\..\..\Include\IKG3DSoundShell.h"
#include <dsound.h>
#include "KG3DModelTable.h"
#include "KG3DAnimationSFXTag.h"
#include "KG3DTerrainRepresentInfo.h"

#define GETFRAME(ID) ((ID & 0xffff0000) >> 16)
#define GETINDEX(ID) (ID & 0x0000ffff)

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


static DWORD s_BlockSize[] = 
{
	sizeof(MotionTagBlockDamagePercentage), //MTBT_DAMAGEPERCENTAGE,
	sizeof(MotionTagBlockMeteor), //MTBT_METEOR,
	sizeof(MotionTagBlockFaceMotion), //MTBT_FACEMOTION,
	sizeof(MotionTagBlockMissile), //MTBT_MISSILE
	sizeof(MotionTagBlockEquipSFX), //MTB_EQUIPSFX
	sizeof(MotionTagBlockGroundEffect), //MTB_GROUNDEFFECT
};

KG3DFaceMotionDefaultHandler* g_pFaceMotionDefaultHandler = NULL;
KG3DFaceMotionLifeRangeHandler* g_pLifeRangeHandler = NULL;

static const TCHAR *s_strFaceSocketName = "s_face";
static const TCHAR *s_strFaceModelKeyword = "_face";
static const TCHAR *s_strFootBone[] = 
{
	"Bip01 R Foot",
	"Bip01 L Foot",
};

KG3DAnimationMotionTag::pfOnApply KG3DAnimationMotionTag::s_fpApplyList[] = 
{
	&(KG3DAnimationMotionTag::OnApplyDamagePercentage),
	&(KG3DAnimationMotionTag::OnApplyMeteor),
	&(KG3DAnimationMotionTag::OnApplyFaceMotion),
	&(KG3DAnimationMotionTag::OnApplyMissle),
	&(KG3DAnimationMotionTag::OnApplyEquipSfx),
	&(KG3DAnimationMotionTag::OnApplyGroundEffect),
};


KG3DAnimationMotionTag::KG3DAnimationMotionTag()
{
    m_uEndFrame = INVALIDATE_FRAME;
    m_uStartFrame = INVALIDATE_FRAME;
    m_pParentModel = NULL;
    m_fParentAnimationSpeed = 1.0f;
    m_vLastPos = D3DXVECTOR3(.0f,.0f,.0f);
}

KG3DAnimationMotionTag::~KG3DAnimationMotionTag()
{
    Clear();
}

DWORD KG3DAnimationMotionTag::GetBlockType(DWORD dwFrame, DWORD dwIndex)
{
	DWORD dwReturn = static_cast<DWORD>(MTBT_INVALIDATE);
	KG_PROCESS_ERROR(m_NewData.count(dwFrame));
	KG_PROCESS_ERROR(dwIndex < m_NewData[dwFrame].Blocks.size());
	KG_PROCESS_ERROR(m_NewData[dwFrame].Blocks[dwIndex]);

	dwReturn = *(static_cast<DWORD*>(m_NewData[dwFrame].Blocks[dwIndex]));
Exit0:
	return dwReturn;
}

DWORD KG3DAnimationMotionTag::GetBlockLength(DWORD dwType)
{
	DWORD dwReturn = 0;
	KG_PROCESS_ERROR(dwType < MTBT_COUNT);
	
	dwReturn = s_BlockSize[dwType];
Exit0:
	return dwReturn;
}

void* KG3DAnimationMotionTag::GetNewBlock(DWORD dwType)
{
	void *pReturn = NULL;

	switch (dwType)
	{
	case MTBT_DAMAGEPERCENTAGE:
		pReturn = new MotionTagBlockDamagePercentage;
		break;
	case MTBT_METEOR:
		pReturn = new MotionTagBlockMeteor;
		break;
	case MTBT_FACEMOTION:
		pReturn = new MotionTagBlockFaceMotion;
		break;
	case MTBT_MISSILE:
		pReturn = new MotionTagBlockMissile;
		break;
	case MTBT_EQUIPSFX:
		pReturn = new MotionTagBlockEquipSFX;
		break;
	case MTBT_GROUNDEFFECT:
		pReturn = new MotionTagBlockGroundEffect;
		break;
	default:
		_ASSERTE(0);
	}
	KG_PROCESS_ERROR(pReturn);
Exit0:
	return pReturn;
}

void KG3DAnimationMotionTag::ClearUnused()
{
	NewDataStructure::iterator it = m_NewData.begin();
	while (it != m_NewData.end())
	{
		if (!it->second.Blocks.size())
		{
			std::vector<UINT>::iterator itFind = find(m_KeyedFrame.begin(),
				m_KeyedFrame.end(),
				it->first);
			it = m_NewData.erase(it);
			m_KeyedFrame.erase(itFind);
		}
		else
		{
			it++;
		}
	}
}

UINT KG3DAnimationMotionTag::ModifyTag(UINT uID, void *pData)
{
	UINT uFrame = GETFRAME(uID);
	UINT uIndex = GETINDEX(uID);

	if (!m_NewData.count(uFrame))
	{
		m_KeyedFrame.push_back(uFrame);
		sort(m_KeyedFrame.begin(), m_KeyedFrame.end());
		m_NewData[uFrame].RunState = MTRS_NONE;
	}

	if (uIndex != FRAMEINDEX_NEW)
	{
		DWORD dwType = *(static_cast<DWORD*>(pData));
		KG_PROCESS_ERROR(dwType != MTBT_INVALIDATE);
        if(uIndex >= m_NewData[uFrame].Blocks.size())
        {
            for (DWORD i = 0 ; i < m_NewData[uFrame].Blocks.size() ; i ++)
            {
                DWORD dwType1 = *(static_cast<DWORD*>(m_NewData[uFrame].Blocks[i]));
                KG_PROCESS_ERROR(dwType != dwType1);
            }
        }
		if (uIndex >= m_NewData[uFrame].Blocks.size())
		{
			m_NewData[uFrame].Blocks.resize(uIndex + 1);

			void *pNewBlock = GetNewBlock(dwType);
			KG_PROCESS_ERROR(pNewBlock);

			m_NewData[uFrame].Blocks[uIndex] = pNewBlock;
		}

		DWORD dwSize = GetBlockLength(dwType);
		memcpy(m_NewData[uFrame].Blocks[uIndex], pData, dwSize);
	}
	else
	{
		strcpy_s(m_NewData[uFrame].strTagName, 
			TAG_NAME_MAX_LENGTH, 
			static_cast<TCHAR*>(pData));
	}

Exit0:
	return static_cast<UINT>(IEIR_SUCCESS);
}

UINT KG3DAnimationMotionTag::RemoveTag(UINT uID)
{
    UINT uReturn = uID;
	switch(uID)
	{
	case MTBT_START:
		m_uStartFrame = INVALIDATE_FRAME;
		break;
	case MTBT_END:
		m_uEndFrame = INVALIDATE_FRAME;
		break;
	default:
		{
			UINT uFrame = (uID & 0xffff0000) >> 16;
			UINT uIndex = (uID & 0x0000ffff);

			NewDataStructure::iterator itFind = m_NewData.find(uFrame);
			std::vector<void*>::iterator itFindSub;
			std::vector<UINT>::iterator itFind1 = find(m_KeyedFrame.begin(), m_KeyedFrame.end(), uFrame);
			KG_PROCESS_ERROR(itFind1 != m_KeyedFrame.end());
			KG_PROCESS_ERROR(itFind != m_NewData.end());
			KG_PROCESS_ERROR(uIndex < itFind->second.Blocks.size());

			itFindSub = itFind->second.Blocks.begin();
			advance(itFindSub, uIndex);
			itFind->second.Blocks.erase(itFindSub);
			if (!itFind->second.Blocks.size())
			{
				m_NewData.erase(itFind);
				m_KeyedFrame.erase(itFind1);
			}
		}
	}
	uReturn = static_cast<UINT>(IEIR_SUCCESS);
Exit0:
    return uReturn;
}

UINT KG3DAnimationMotionTag::MoveTag(UINT uID, UINT uFrameTo)
{
    UINT uReturn = IEIR_FAILED;
    uReturn = CopyTag(uID, uFrameTo);
    KG_PROCESS_ERROR(uReturn != IEIR_FAILED);
    KG_PROCESS_ERROR(uReturn != uFrameTo);
    if (m_uStartFrame == uID)
    {
        m_uStartFrame = uFrameTo;
    }
    if (m_uEndFrame == uID)
    {
        m_uEndFrame = uFrameTo;
    }
    uReturn = RemoveTag(uID);
Exit0:
    return uReturn;
}

UINT KG3DAnimationMotionTag::CopyTag(UINT uID, UINT uFrameDest)
{
    UINT uReturn = IEIR_FAILED;
	UINT uFrame = 0;
	UINT uIndex = 0;
	UINT uDestID = 0;
	UINT uIndexDest = 0;
	NewDataStructure::iterator itFindSrc;
	std::vector<UINT>::iterator itFindDest;

	KG_PROCESS_ERROR(uID != MTBT_START);
	KG_PROCESS_ERROR(uID != MTBT_END);
	
	uFrame = GETFRAME(uID);
	uIndex = GETINDEX(uID);
	
	itFindSrc = m_NewData.find(uFrame);
	KG_PROCESS_ERROR(itFindSrc != m_NewData.end());
	
	CreateNewTag(uFrameDest, itFindSrc->second.strTagName);
	
	uIndexDest = static_cast<UINT>(m_NewData[uFrameDest].Blocks.size());
	uDestID = GETID(uFrameDest, uIndexDest);
	
	ModifyTag(uDestID, m_NewData[uFrame].Blocks[uIndex]);

    uReturn = static_cast<UINT>(IEIR_SUCCESS);
Exit0:
    return uReturn;
}

UINT KG3DAnimationMotionTag::GetNumKeyFrame(UINT uFrame)
{
    UINT uReturn = 0;
    if (m_NewData.count(uFrame))
    {
        uReturn = static_cast<UINT>(m_NewData[uFrame].Blocks.size());
		//新建的情况, size() == 0, 但是需要返回1才能在界面正常显示按钮
		uReturn = uReturn ? uReturn : 1;
    }
	else if (uFrame == m_uEndFrame || uFrame == m_uStartFrame)
	{
		uReturn = 1;
	}
    return uReturn;
}

HRESULT KG3DAnimationMotionTag::Clear()
{
    for (size_t i = 0; i < m_KeyedFrame.size(); i++)
	{
		for (size_t j = 0; j < m_NewData[m_KeyedFrame[i]].Blocks.size(); j++)
		{
			SAFE_DELETE(m_NewData[m_KeyedFrame[i]].Blocks[j]);
		}
	}
	m_NewData.clear();
    m_KeyedFrame.clear();
	m_setSockets.clear();
    m_uEndFrame = INVALIDATE_FRAME;
    m_uStartFrame = INVALIDATE_FRAME;
    return S_OK;
}

void KG3DAnimationMotionTag::OnApplyFaceMotion(void *pData,
											   KG3DAnimationController *pController, 
											   D3DXVECTOR3 *pvPos)
{
	HRESULT hResult = E_FAIL;
	TCHAR strMotionPathName[MAX_PATH];
	std::vector<IKG3DModel*> vecFaceModels;
	KG3DFaceMotionSettings *pSettings = NULL;
	MotionTagBlockFaceMotion *pFaceMotion = static_cast<MotionTagBlockFaceMotion*>(pData);
	KG_PROCESS_ERROR(pFaceMotion);
	KG_PROCESS_ERROR(m_pParentModel);

	strMotionPathName[0] = '\0';

	pSettings = static_cast<KG3DFaceMotionSettings*>(g_GetEngineManager()->GetFaceMotionSettings());
	KG_PROCESS_ERROR(pSettings);
	
	m_pParentModel->GetBindToSocketModel(s_strFaceSocketName, vecFaceModels);
	KG_PROCESS_SUCCESS(!vecFaceModels.size());

	for (size_t i = 0; i < vecFaceModels.size(); i++)
	{
		KG3DModel *pModelFace = static_cast<KG3DModel*>(vecFaceModels[i]);
		IKG3DAnimationController *pFaceAniController = NULL;
		if (!pModelFace)
			continue;

		if (!strstr(pModelFace->m_scName.c_str(), s_strFaceModelKeyword))
			continue;

		if (pFaceMotion->fPlayRation < rand() % 100)
		{
			//ATLTRACE("Face motion triggered, missed.\n");
			continue;
		}

		if (pFaceMotion->dwMotionType != FACEMOTION_SPECIFIED)
		{
			pSettings->GetFaceMotion(pFaceMotion->dwMotionType, 
				pModelFace->m_scName.c_str(),
				strMotionPathName);		
		}
		else
		{
			TCHAR strDriver[MAX_PATH];
			TCHAR strPath[MAX_PATH];
			TCHAR strFile[MAX_PATH];

			_splitpath_s(pModelFace->m_scName.c_str(),
				strDriver,
				MAX_PATH,
				strPath,
				MAX_PATH,
				strFile,
				MAX_PATH,
				NULL,
				0);

			pSettings->GetFaceMotionPathFromFacePath(strPath);

			sprintf_s(strMotionPathName, 
				MAX_PATH,
				"%s%s%s%s%s.ani",
				strDriver,
				strPath,
				g_FaceMotionSettings.GetFaceMotionModifyPath(),
				strFile,
				pFaceMotion->strPath);
		}

		hResult = pModelFace->PlayAnimation(ENU_ANIMATIONPLAY_ONCE, 
			strMotionPathName,	
			pFaceMotion->fPlaySpeed,
			0,
			NULL,
			NULL, 
			ANICTL_PRIMARY);
		if (FAILED(hResult))
			continue;

		pFaceAniController = pModelFace->GetAnimationController(ANICTL_PRIMARY);
		KG_PROCESS_ERROR(pFaceAniController);

		pFaceAniController->SetMotionExtraInfo(_kg_to_smaller_size_cast<DWORD>(pData));
		pFaceAniController->RegistereEventHandler(g_pLifeRangeHandler);
	}
Exit1:
Exit0:
	;
}

HRESULT KG3DAnimationMotionTag::ProcessUnderWater(D3DXVECTOR3 *pvPos)
{
    HRESULT hResult = E_FAIL;
    HRESULT hRetCode = E_FAIL;
	KG3DSceneSceneEditor *pEditor = NULL;
	D3DXVECTOR3 pIn;
	D3DXVECTOR3 pOut;
	BOOL bUnderWater = FALSE;
    KG3DScene *pScene = NULL; 
    FLOAT fComBy = 50.0;
    pScene = g_cGraphicsTool.GetCurScene();
    KG_PROCESS_ERROR(pScene);
	pEditor = static_cast<KG3DSceneSceneEditor*>(pScene);
	KG_PROCESS_ERROR(pEditor);
	pIn = *pvPos * 100.0f;
	hRetCode = pEditor->GetWaterHeight(&pOut,&pIn);
    KG_COM_PROCESS_ERROR(hRetCode);
    hRetCode = g_GetTerrainRepresentInfo().GetWaterDis(&fComBy);
    KG_COM_PROCESS_ERROR(hRetCode);
	if ( pIn.y - pOut.y < fComBy)// 模型基准点与水面高度的差值小于标准值 则模型接触水面
	{
		bUnderWater = TRUE;
	}
    KG_PROCESS_ERROR(bUnderWater);
    hResult = S_OK;
Exit0:
    return hResult; 
}
void KG3DAnimationMotionTag::OnApplyGroundEffect(void *pData,
												 KG3DAnimationController *pController,
												 D3DXVECTOR3 *pvPos)
{
    HRESULT hResult = E_FAIL;
    HRESULT hRetCode = E_FAIL; 
    KG3DScene *pScene = NULL;
    KG3DSceneSceneEditor *pEditor = NULL;
    const TerrainRepresentInfo *pInfo = NULL;
    MotionTagBlockGroundEffect *pGroundEffect = NULL ;
    KG3DTerrainRepresentInfo::GroundDefaultSfx *pDefaultSfx = NULL;
   
    FLOAT fComBy = 50.0;
    FLOAT fGroundHeight = 0.0;
    FLOAT fFloorHeight = 0.0;
    FLOAT fRandom = 0.0;

    BOOL bOnTerrain = FALSE;
    D3DXVECTOR3 PIn = *pvPos * 100.0;

    TCHAR strSfx[MAX_PATH];
    strSfx[0] = '\0';
    TCHAR strSfxTerrain[MAX_PATH];
    strSfxTerrain[0] = '\0';

    if( fabs(m_vLastPos.x - pvPos->x) < 1e-5 && fabs(m_vLastPos.y - pvPos->y) < 1e-5 
        && fabs(m_vLastPos.z - pvPos->z) < 1e-5)// 判断是否移动
        KG_PROCESS_ERROR(FALSE);
    
    m_vLastPos = *pvPos;
    hRetCode = ProcessUnderWater(pvPos); //  成功说明在水下 水下无效果
    KG_COM_PROCESS_SUCCESS(hRetCode);

    pScene = g_cGraphicsTool.GetCurScene();
    KG_PROCESS_ERROR(pScene);
    pEditor = static_cast<KG3DSceneSceneEditor*>(pScene);
    KG_PROCESS_ERROR(pEditor);
    pGroundEffect = static_cast<MotionTagBlockGroundEffect*>(pData);
    KG_PROCESS_ERROR(pGroundEffect);
    KG_PROCESS_ERROR(pGroundEffect->dwSFXIndex < TERRAINREPRESENTSFXNUM);
    pInfo = pEditor->GetGroundRepresentInfo(PIn);
    KG_PROCESS_ERROR(pInfo);
    hRetCode = g_GetTerrainRepresentInfo().GetDis(&fComBy);
    KG_COM_PROCESS_ERROR(hRetCode);
    hRetCode = pEditor->GetGroundHeight(fGroundHeight,PIn.x ,PIn.z);
    KG_COM_PROCESS_ERROR(hRetCode);
    
    hRetCode = pEditor->GetFloor(&PIn,&fFloorHeight);
    KG_COM_PROCESS_ERROR(hRetCode);
    bOnTerrain = TRUE;
    if(fFloorHeight > fGroundHeight)
        bOnTerrain = FALSE;// 说明地表上有物体存在
    
    if(!bOnTerrain)
    {  
        if(PIn.y - fFloorHeight < fComBy)
        {// 站在模型表面 采用默认特效
            hRetCode = g_GetTerrainRepresentInfo().GetDefaultSfx(&pDefaultSfx);
            KG_COM_PROCESS_ERROR(hRetCode);
            strcpy_s(strSfx,MAX_PATH,pDefaultSfx->str_DefaultSfx[pGroundEffect->dwSFXIndex]);
            strcpy_s(strSfxTerrain,MAX_PATH,pDefaultSfx->str_DefaultSfxTerrain);
        }
    }
    else
    {
        if(PIn.y - fGroundHeight < fComBy)
        {// 站在地表表面 采用表中的特效
            strcpy_s(strSfx,MAX_PATH,pInfo->strSfx[pGroundEffect->dwSFXIndex]);
            strcpy_s(strSfxTerrain,MAX_PATH,pInfo->strSfxTerrain);
        }
    }
    PlayGoundEffectSfx(strSfx,&PIn);
    fRandom = (FLOAT)(rand() % 100 /100.0);
    if(fRandom <= pInfo->fSfxPlayRate)
        PlayGoundEffectSfx(strSfxTerrain,&PIn);
Exit1:
    hResult = S_OK;
Exit0:
	;
}
HRESULT KG3DAnimationMotionTag::PlayGoundEffectSfx(LPCSTR pSfxName,D3DXVECTOR3 *pvPos)
{
    HRESULT hResult = E_FAIL;
    HRESULT hRetCode = E_FAIL;
    KG3DModel *pSfx = NULL;
    KG3DAnimationController* pControllerSfx = NULL;
    KG3DScene *pScene = NULL;
    BOOL bSave = FALSE;
    KG_PROCESS_ERROR(pSfxName&&pSfxName[0]);
    hRetCode = g_cModelTable.LoadResourceFromFile(pSfxName,
        0,
        0,
        (IKG3DResourceBase**)&pSfx);
    KG_COM_PROCESS_ERROR(hRetCode);
    pScene = g_cGraphicsTool.GetCurScene();
    KG_PROCESS_ERROR(pScene);

    pSfx->SetTranslation(pvPos);
    pControllerSfx = static_cast<KG3DAnimationController*>(pSfx->GetAnimationController(ANICTL_PRIMARY));
    KG_PROCESS_ERROR(pControllerSfx);
    pControllerSfx->RegistereEventHandler(GetSFXTagSceneWorldPosEventHandeler());
    bSave = g_bEnableAddEntity;
    g_bEnableAddEntity = TRUE;
    pScene->AddRenderEntity(pSfx, 0, FALSE, FALSE);
    g_bEnableAddEntity = bSave;
    pSfx->PlayAnimation(ENU_ANIMATIONPLAY_ONCE,NULL, 1.0f, 0, NULL, NULL, ANICTL_PRIMARY);
    hResult = S_OK;
Exit0:
    if(FAILED(hResult))
    {
        SAFE_RELEASE(pSfx);
    }
    return hResult;
}
void KG3DAnimationMotionTag::OnApplyMeteor(void *pData, 
										   KG3DAnimationController *pController, 
										   D3DXVECTOR3 *pvPos)
{
	DWORD i = 0;
	MotionTagBlockMeteor *pMeteor = static_cast<MotionTagBlockMeteor*>(pData);
	KG_PROCESS_ERROR(pMeteor);

	for (i = 0; i < pMeteor->dwNumFlag; i++)
	{
		std::vector<IKG3DModel*> vecChildModels;
		m_pParentModel->GetBindToSocketModel(pMeteor->strIndicator[i], vecChildModels);
		BOOL bSamplingEnabled = FALSE;
		for (size_t j = 0; j < vecChildModels.size(); j++)
		{
			IKG3DModel *pModel = vecChildModels[j];
			if (!pModel)
				continue;
			
			if(pModel->GetType() == MESHTYPE_SFX)
			{
				KG3DSFX* pSFX = dynamic_cast<KG3DSFX*>(pModel);
				pSFX->SetMeteorSampling(pMeteor->dwFlags[i]);
			}
			else
			{
				std::set<IKG3DModel*> setChildModel;
				pModel->GetBindToOrgModel(setChildModel);
				set<IKG3DModel*>::iterator it = setChildModel.begin();
				while (it != setChildModel.end())
				{
					if((*it)->GetType() == MESHTYPE_SFX)
					{
						KG3DSFX* pSFX = dynamic_cast<KG3DSFX*>(*it);
						pSFX->SetMeteorRunState(pMeteor->dwFlags[i], pController);

						if (pMeteor->dwFlags[i])
						{
							bSamplingEnabled = TRUE;
						}
					}
					it++;
				}
			}
		}
		if (bSamplingEnabled)
		{
			m_setSockets.insert(pMeteor->strIndicator[i]);
		}
		else
		{
			m_setSockets.erase(pMeteor->strIndicator[i]);
		}
	}
Exit0:
	;
}

void KG3DAnimationMotionTag::OnApplyDamagePercentage(void *pData, 
													 KG3DAnimationController *pController, 
													 D3DXVECTOR3 *pvPos)
{
	IKG3DAnimationEventHandler *pEventHandler = NULL;
	MotionTagBlockDamagePercentage *pDamage = static_cast<MotionTagBlockDamagePercentage*>(pData);
	TagMotionCallBackExtraInfo Info;
	KG_PROCESS_ERROR(pDamage);
	KG_PROCESS_ERROR(pController);

	pEventHandler = pController->GetEventHandler();
	KG_PROCESS_SUCCESS(!pEventHandler);

	Info.Type = pDamage->Type;
	Info.fData[0] = pDamage->fDamagePercentage;
	pEventHandler->OnMotionNotify(pController, pController->GetMotionExtraInfo(), &Info);
Exit1:
Exit0:
	;
}

void KG3DAnimationMotionTag::OnApplyMissle(void *pData, 
										   KG3DAnimationController *pController, 
										   D3DXVECTOR3 *pvPos)
{
	IKG3DAnimationEventHandler *pEventHandler = NULL;
	MotionTagBlockMissile *pMissle = static_cast<MotionTagBlockMissile*>(pData);
	TagMotionCallBackExtraInfo Info;
	KG_PROCESS_ERROR(pMissle);
	KG_PROCESS_ERROR(pController);

	pEventHandler = pController->GetEventHandler();
	KG_PROCESS_SUCCESS(!pEventHandler);

	Info.Type = pMissle->Type;
	Info.fData[0] = pMissle->fDamagePercentage;
	Info.dwData[0] = pMissle->dwMissileID;
	pEventHandler->OnMotionNotify(pController, pController->GetMotionExtraInfo(), &Info);
Exit1:
Exit0:
	;
}

void KG3DAnimationMotionTag::OnApplyEquipSfx(void *pData,
											 KG3DAnimationController *pController,
											 D3DXVECTOR3 *pvPos)
{
	IKG3DAnimationEventHandler *pEventHandler = NULL;
	MotionTagBlockEquipSFX *pEquipSfx = static_cast<MotionTagBlockEquipSFX*>(pData);
	TagMotionCallBackExtraInfo Info;
	KG_PROCESS_ERROR(pEquipSfx);
	KG_PROCESS_ERROR(pController);

	pEventHandler = pController->GetEventHandler();
	KG_PROCESS_SUCCESS(!pEventHandler);

	
	Info.Type = pEquipSfx->Type;
	Info.dwData[0] = pEquipSfx->dwEquipType;
	Info.dwData[1] = pEquipSfx->dwEquipRepesentID;
	Info.dwData[2] = pEquipSfx->dwSfxID;
	pEventHandler->OnMotionNotify(pController, pController->GetMotionExtraInfo(), &Info);
Exit1:
Exit0:
	;
}

HRESULT KG3DAnimationMotionTag::Helper_Apply(KG3DAnimationController *pController, 
											 UINT uStartFrame, 
											 UINT uEndFrame, 
											 D3DXVECTOR3 *pvPos)
{
	HRESULT hRetCode = E_FAIL;
	HRESULT hr = E_FAIL;
	KG_PROCESS_SUCCESS(!m_KeyedFrame.size());
	KG_PROCESS_ERROR(m_pParentModel);
	if (uStartFrame > uEndFrame)
	{
		//处理循环的情况
		hr = Helper_Apply(pController, 
			uStartFrame, 
			max(m_KeyedFrame[m_KeyedFrame.size() - 1], uStartFrame), 
			pvPos);
		KG_COM_PROCESS_ERROR(hr);

		Reset();
		hr = Helper_Apply(pController, 
			min(uEndFrame, m_KeyedFrame[0]), 
			uEndFrame, 
			pvPos);
		KG_COM_PROCESS_ERROR(hr);
	}
	else
	{
		UINT uStartIndex = 0;
		UINT uEndIndex = 0;
		hr = FindKeyFrame(uStartFrame, FFT_GREATEREQUAL, m_KeyedFrame, uStartIndex);
		KG_PROCESS_SUCCESS(hr);//if failed(hr), no key frame found, exit
		hr = FindKeyFrame(uEndFrame, FFT_LESSEQUAL, m_KeyedFrame, uEndIndex);
		KG_PROCESS_SUCCESS(hr);

		for (UINT uIndex = uStartIndex; uIndex <= uEndIndex; uIndex++)
		{
			UINT uFrame = m_KeyedFrame[uIndex];
			//如果已经激活就不再处理了
			if (m_NewData[uFrame].RunState == MTRS_ACTIVE)
				continue;

			DWORD dwSize = static_cast<DWORD>(m_NewData[uFrame].Blocks.size());
			for (DWORD i = 0; i < dwSize; i++)
			{
				DWORD dwType = GetBlockType(uFrame, i);
				(this->*s_fpApplyList[dwType])(m_NewData[uFrame].Blocks[i],
					pController, 
					pvPos);
			}
			m_NewData[uFrame].RunState = MTRS_ACTIVE;
		}
	}
Exit1:
	hRetCode = S_OK;
Exit0:
	return hRetCode;
}

BOOL KG3DAnimationMotionTag::CheckValidation()
{
	return TRUE;
}

void KG3DAnimationMotionTag::SetAnimationSpeed(float fSpeed)
{
    m_fParentAnimationSpeed = fSpeed;
}

void KG3DAnimationMotionTag::OnMotionChange()
{
    //关闭所有的刀光
	std::set<string>::iterator itSockets;
	std::vector<IKG3DModel*> vecFaceModels;

	KG_PROCESS_ERROR(m_pParentModel);

	itSockets = m_setSockets.begin();
	while (itSockets != m_setSockets.end())
	{
		std::vector<IKG3DModel*> vecChildModels;
		m_pParentModel->GetBindToSocketModel((*itSockets).c_str(), vecChildModels);
		vector<IKG3DModel*>::iterator i = vecChildModels.begin();
		while (i != vecChildModels.end())
		{
			std::set<IKG3DModel*> setChildModel;
			(*i)->GetBindToOrgModel(setChildModel);
			set<IKG3DModel*>::iterator it = setChildModel.begin();
			while (it != setChildModel.end())
			{
				if((*it)->GetType() == MESHTYPE_SFX)
				{
					KG3DSFX* pSFX = dynamic_cast<KG3DSFX*>(*it);
					pSFX->SetMeteorRunState(FALSE, NULL);
				}
				it++;
			}
			i++;
		}
		itSockets++;
	}

	//处理表情动画
	KG_PROCESS_ERROR(m_pParentModel);
	m_pParentModel->GetBindToSocketModel(s_strFaceSocketName, vecFaceModels);
	for (size_t i = 0; i < vecFaceModels.size(); i++)
	{
		KG3DModel *pModel = static_cast<KG3DModel*>(vecFaceModels[i]);
		if (!pModel)
			continue;

		KG3DAnimationController *pController = static_cast<KG3DAnimationController*>(pModel->GetAnimationController(ANICTL_PRIMARY));
		_ASSERTE(pController);

		if (pController->m_bOnFinish)
		{
			//如果已经播放完成了需要强制调用一次Finish保证Keep属性的表情也能正确切换
			pModel->RestoreFace();
		}
		IKG3DAnimationEventHandler *pEventHandler = pController->GetEventHandler();

		if (!pEventHandler)
			continue;
			//设置成默认表情的回调，当前表情播放完成之后就会开始默认表情的播放
		pController->RegistereEventHandler(g_pFaceMotionDefaultHandler);
	}
Exit0:
	m_setSockets.clear();
}

void KG3DAnimationMotionTag::SetParentModel(IKG3DModel* pModel)
{
    m_pParentModel = pModel;
}

DWORD KG3DAnimationMotionTag::GetMotionTagType(UINT uID)
{
	return static_cast<enuAnimationMotionTagType>(GetBlockType(GETFRAME(uID), GETINDEX(uID)));
}

HRESULT KG3DAnimationMotionTag::DeleteAllSound()
{
	return S_OK;
}

HRESULT KG3DAnimationMotionTag::SaveToMemory(BYTE** ppData, DWORD* pdwSize,DWORD dwMaxFrame)
{
	HRESULT hRetCode = E_FAIL;
	AnimationTagBlockHeader *pHeader = NULL;
	MotionTagDataInfoNew *pFrameHeader = NULL;
	BYTE* pCurrent = NULL;
	NewDataStructure::iterator it = m_NewData.begin();
	NewDataStructure::iterator itEnd = m_NewData.end();
	DWORD dwFrameLenght = 0 ;
	DWORD dwNumFrameUsed = 0;
	DWORD uStartFrame =  m_uStartFrame;
	DWORD uEndFrame =  m_uEndFrame;
	ClearUnused();
	KG_PROCESS_ERROR(ppData);
	KG_PROCESS_ERROR(pdwSize);

	//计算保存的数据大小
	while (it != itEnd)
	{
		UINT uFrame = it->first;
		if(uFrame > dwMaxFrame)
			break;

		dwNumFrameUsed++;
		const MotionTagDataRuntime& DataRuntime = it->second;
		size_t uNumBlock = DataRuntime.Blocks.size();
		for (size_t i = 0; i < uNumBlock; i++)
		{
			DWORD dwType = GetBlockType(uFrame, static_cast<DWORD>(i));
			DWORD dwLenght = GetBlockLength(dwType);
			dwFrameLenght += dwLenght;//*pdwSize += dwLenght;//每个子关键帧的数据大小
		}
		
		it++;
	}
	*pdwSize = static_cast<DWORD>(sizeof(AnimationTagBlockHeader) + //Tag头信息
		sizeof(DWORD) * 2 + //动作融合起始和结束帧信息
		sizeof(MotionTagDataInfoNew) * dwNumFrameUsed + // 每个关键帧的帧头信息
		dwFrameLenght);
	*ppData = new BYTE[*pdwSize];
	KG_PROCESS_ERROR(ppData);

	pCurrent = *ppData;
	pHeader = reinterpret_cast<AnimationTagBlockHeader*>(*ppData);
	ASSERT(pHeader);

	pHeader->dwNumKeyFrames = dwNumFrameUsed;//static_cast<DWORD>(m_NewData.size());
	pHeader->dwType = TAGTYPE_MOTION;
	pHeader->dwVersion = s_dwCurrentVersion;

	pCurrent += sizeof(AnimationTagBlockHeader);
	DWORD *pMotionFrame = reinterpret_cast<DWORD*>(pCurrent);

	if(m_uStartFrame > dwMaxFrame)
		uStartFrame = INVALIDATE_FRAME;
	if(m_uEndFrame > dwMaxFrame)
		uEndFrame = INVALIDATE_FRAME;
	*pMotionFrame = uStartFrame;
	*(pMotionFrame + 1) = uEndFrame;
	pCurrent += sizeof(DWORD) * 2;

	it = m_NewData.begin();
	while (it != itEnd)
	{
		UINT uFrame = it->first;
		if(uFrame <= dwMaxFrame)
		{
			pFrameHeader = reinterpret_cast<MotionTagDataInfoNew*>(pCurrent);
			const MotionTagDataRuntime& DataRuntime = it->second;
			strcpy_s(pFrameHeader->strTagName, TAG_NAME_MAX_LENGTH, DataRuntime.strTagName);
			pFrameHeader->dwFrame = it->first;
			pFrameHeader->dwNumMotionExtension = static_cast<DWORD>(DataRuntime.Blocks.size());
			pCurrent += sizeof(MotionTagDataInfoNew);

			for (DWORD i = 0; i < pFrameHeader->dwNumMotionExtension; i++)
			{
				DWORD dwType = GetBlockType(uFrame, i);
				pFrameHeader->dwBlockLength[i] = GetBlockLength(dwType);//子块的大小
				memcpy(pCurrent, DataRuntime.Blocks[i], pFrameHeader->dwBlockLength[i]);
				pCurrent += pFrameHeader->dwBlockLength[i];
			}
		}

		it++;
	}
	hRetCode = S_OK;
Exit0:
	return hRetCode;
}

HRESULT KG3DAnimationMotionTag::LoadFromFile(IFile* pFile, DWORD dwVersion, DWORD dwNumFrames)
{
    HRESULT hr = E_FAIL;
    unsigned long uReturn = 0;
	BYTE *pBuffer = NULL;
	DWORD dwBufferSize = 0;
    DWORD dwStartTime = timeGetTime();

    KG_PROCESS_ERROR(pFile);

    switch (dwVersion)
    {
    case 0:
        {
            for (DWORD i = 0; i < dwNumFrames; i++)
            {
                MotionTagKeyFrameSave KeyFrame;

                uReturn = pFile->Read(&KeyFrame, sizeof(MotionTagKeyFrameSave));
                KG_PROCESS_ERROR(uReturn == sizeof(MotionTagKeyFrameSave));

                ConvertToVersion1(KeyFrame);
            }
        }
        break;
	case 1:
		{
			uReturn = pFile->Read(&m_uStartFrame, sizeof(DWORD));
			KG_PROCESS_ERROR(uReturn == sizeof(DWORD));

			uReturn = pFile->Read(&m_uEndFrame, sizeof(DWORD));
			KG_PROCESS_ERROR(uReturn == sizeof(DWORD));

			for (DWORD i = 0; i < dwNumFrames; i++)
			{
				MotionTagDataInfoNew KeyFrame;
				uReturn = pFile->Read(&KeyFrame, sizeof(MotionTagDataInfoNew));
				KG_PROCESS_ERROR(uReturn == sizeof(MotionTagDataInfoNew));

				DWORD dwID = GETID(KeyFrame.dwFrame, FRAMEINDEX_NEW);
				ModifyTag(dwID, KeyFrame.strTagName);

				for (DWORD j = 0; j < KeyFrame.dwNumMotionExtension; j++)
				{
					DWORD dwID = GETID(KeyFrame.dwFrame, j);
					if (dwBufferSize < KeyFrame.dwBlockLength[j])
					{
						SAFE_DELETE_ARRAY(pBuffer);
						pBuffer = new BYTE[KeyFrame.dwBlockLength[j]];
						dwBufferSize = KeyFrame.dwBlockLength[j];
					}
					uReturn = pFile->Read(pBuffer, KeyFrame.dwBlockLength[j]);
					KG_PROCESS_ERROR(uReturn == KeyFrame.dwBlockLength[j]);

					ModifyTag(dwID, pBuffer);
				}
			}
		}
		break;
    default:
        assert(0);
    }

    hr = S_OK;
Exit0:
    SAFE_DELETE_ARRAY(pBuffer);
    DWORD dwCost = timeGetTime() - dwStartTime;
    if (g_cEngineOption.bEnableTimeOptimizeLog && dwCost > 10)
    {
        KGLogPrintf(KGLOG_INFO,"TimeOptimize %d %s", dwCost, KG_FUNCTION);
    }
    return hr;
}

void KG3DAnimationMotionTag::Reset()
{
    NewDataStructure::iterator it = m_NewData.begin();
    while (it != m_NewData.end())
    {
        it->second.RunState = MTRS_NONE;
        it++;
    }
	m_setSockets.clear();
}


UINT KG3DAnimationMotionTag::GetKeyFrame(UINT uFrame, MotionTagDataRuntime* &pData)
{
	UINT uRetCode = IEIR_FAILED;
	NewDataStructure::iterator itFind = m_NewData.find(uFrame);
	KG_PROCESS_ERROR(itFind != m_NewData.end());
	
	pData = &(itFind->second);
	uRetCode = static_cast<UINT>(IEIR_SUCCESS);
Exit0:
	return uRetCode;

}


enuTagRunState KG3DAnimationMotionTag::GetState()
{
	//暂时不存在有资源需要释放的情况, 所以可以直接返回FINISH
	//如果出现需要释放的资源，这里需要做相应的修改
	return TAG_STATE_FINISHED;
}

UINT KG3DAnimationMotionTag::GetBindedSocketName(vector<string> &vecSocketName)
{
	for (int i = 0;i < (int)m_KeyedFrame.size();i++)
	{
		int nFrame = m_KeyedFrame[i];
		
		
		for (size_t j = 0; j < m_NewData[nFrame].Blocks.size(); j++)
		{
			if (GetBlockType(nFrame, static_cast<DWORD>(j)) == MTBT_METEOR)
			{
				MotionTagBlockMeteor *pMeteor = static_cast<MotionTagBlockMeteor*>(m_NewData[nFrame].Blocks[j]);
				for (DWORD k = 0; k < pMeteor->dwNumFlag; k++)
				{
					int nReturn = -1;
					for (int nFind =0;nFind < (int)vecSocketName.size(); nFind++)
					{
						nReturn = strcmp(pMeteor->strIndicator[k], vecSocketName[nFind].c_str());
						if(nReturn == 0)
							break;
					}
					if(nReturn != 0)
						vecSocketName.push_back(pMeteor->strIndicator[k]);
				}
			}
		}
	}
	return (UINT)vecSocketName.size();
}


UINT KG3DAnimationMotionTag::CreateNewTag(UINT uFrame, 
										  TCHAR* strTagName)
{
	UINT uReturn = IEIR_FAILED;
	TCHAR strAutoTagName[TAG_NAME_MAX_LENGTH];
	KG_PROCESS_ERROR(GetNumKeyFrame(uFrame) == 0);
	if (!strTagName)
	{
		sprintf_s(strAutoTagName, "Key Frame @ %d", uFrame);
		strTagName = strAutoTagName;
	}
	
	{
		UINT uID = GETID(uFrame, FRAMEINDEX_NEW);
		ModifyTag(uID, static_cast<void*>(strTagName));
	}
	uReturn = IEIR_FAILED;
Exit0:
	return uReturn;
}

HRESULT KG3DAnimationMotionTag::ConvertToVersion1(MotionTagKeyFrameSave& Data)
{
	HRESULT hRetCode = E_FAIL;
	UINT uResult = IEIR_FAILED;

	MotionTagDataInfo &KeyFrame = Data.FrameData.MotionKeyFrameData;
	switch(KeyFrame.Type)
	{
	case MTT_START:
		{
			m_uStartFrame = Data.uFrame;
		}
		break;
	case MTT_END:
		{
			m_uEndFrame = Data.uFrame;
		}
		break;
	case MTT_USERDEFINE:
		{
			CreateNewTag(Data.uFrame, KeyFrame.strTagName);
			MotionTagDataRuntime *pNewKeyFrame = NULL;
			uResult = GetKeyFrame(Data.uFrame, pNewKeyFrame);
			KG_PROCESS_ERROR(uResult == IEIR_SUCCESS);

			UINT uIndex = static_cast<DWORD>(pNewKeyFrame->Blocks.size());
			UINT uID = GETID(Data.uFrame, uIndex);
			switch (KeyFrame.ExtraInfo.Type)
			{
			case MTBT_DAMAGEPERCENTAGE:
				{
					MotionTagBlockDamagePercentage DP;
					DP.fDamagePercentage = KeyFrame.ExtraInfo.fData[0];
					DP.Type = MTBT_DAMAGEPERCENTAGE;
					ModifyTag(uID, static_cast<void*>(&DP));
				}
				break;
			case MTBT_METEOR:
				{
					MotionTagBlockMeteor Meteor;
					Meteor.Type = MTBT_METEOR;
					Meteor.dwNumFlag = static_cast<DWORD>(KeyFrame.ExtraInfo.fData[7]);
					memcpy(Meteor.dwFlags, KeyFrame.ExtraInfo.dwData, sizeof(DWORD) * 8);
					for (DWORD i = 0; i < Meteor.dwNumFlag; i++)
					{
						strcpy_s(Meteor.strIndicator[i], MAX_BONE_NAME_LENGTH, KeyFrame.ExtraInfo.strData[i]);
					}
					ModifyTag(uID, static_cast<void*>(&Meteor));
				}
				break;
			case MTBT_MISSILE:
			case MTBT_FACEMOTION:
			case MTBT_EQUIPSFX:
				_ASSERTE(0);//这个是Version1之后才有的所以不可能出现需要转换的
				break;
			}
		}
		break;
	}

	hRetCode = S_OK;
Exit0:
	return hRetCode;
}

KG3DFaceMotionLifeRangeHandler::~KG3DFaceMotionLifeRangeHandler()
{
	g_cClipTable.GetEventCenter()->RemoveEventHandler(this);
}

KG3DFaceMotionLifeRangeHandler::KG3DFaceMotionLifeRangeHandler()
{

}

HRESULT KG3DFaceMotionLifeRangeHandler::OnPlayAnimationFinish(IKG3DAnimationController* pController)
{
	//在正常的播放情况下动画播放结束
	HRESULT hRetCode = E_FAIL;
	MotionTagBlockFaceMotion *pFaceMotion = NULL;
	KG3DFaceMotionSettings *pSettings = NULL;
	KG_PROCESS_ERROR(pController);
	
	pSettings = static_cast<KG3DFaceMotionSettings*>(g_GetEngineManager()->GetFaceMotionSettings());
	KG_PROCESS_ERROR(pSettings);

	pFaceMotion = _kg_to_bigger_size_cast<MotionTagBlockFaceMotion*>(pController->GetMotionExtraInfo());
	KG_PROCESS_ERROR(pFaceMotion);

	if (pFaceMotion->dwPlayType == ENU_ANIMATIONPLAY_ONCE)
	{
		//如果是单次播放的, 并且没有选择在播放完成之后保持，需要恢复到默认表情
		if (!pFaceMotion->bKeepOnMotionFinish)
		{
			KG3DModel *pFaceModel = static_cast<KG3DModel*>(pController->GetModel());
			_ASSERTE(pFaceModel);
			g_DefaultFaceMotionMgr.SetDefaultFaceMotion(pFaceModel);
		}
	}
	else if (pFaceMotion->fPlayRation > rand() % 100)
	{
		//循环的并且通过了几率测试
		pController->StartAnimation(ENU_ANIMATIONPLAY_ONCE, 0, pFaceMotion->fPlaySpeed);
	}
	hRetCode = S_OK;
Exit0:
	return hRetCode;
}

HRESULT KG3DFaceMotionLifeRangeHandler::OnTagPlayingSFX(IKG3DAnimationController* pController, 
								IKG3DModel* pModelSFX, 
								TagSFXCallBackExtraInfo* pExtraInfo)
{
	return E_NOTIMPL;
}

HRESULT KG3DFaceMotionLifeRangeHandler::OnMotionNotify(IKG3DAnimationController* pController, 
							   DWORD dwMotionInfo, 
							   TagMotionCallBackExtraInfo *pMotionCallBackExtraInfo)
{
	return E_NOTIMPL;
}

KG3DFaceMotionDefaultHandler::KG3DFaceMotionDefaultHandler()
{

}

KG3DFaceMotionDefaultHandler::~KG3DFaceMotionDefaultHandler()
{
	g_cClipTable.GetEventCenter()->RemoveEventHandler(this);
}

HRESULT KG3DFaceMotionDefaultHandler::OnPlayAnimationFinish(IKG3DAnimationController* pController)
{
	//播放默认表情
	HRESULT hRetCode = E_FAIL;
	KG3DModel *pFaceModel = NULL;

	pFaceModel = static_cast<KG3DModel*>(pController->GetModel());
	KG_PROCESS_ERROR(pFaceModel);

	g_DefaultFaceMotionMgr.SetDefaultFaceMotion(pFaceModel);

	hRetCode = S_OK;
Exit0:
	return hRetCode;
}

HRESULT KG3DFaceMotionDefaultHandler::OnTagPlayingSFX(IKG3DAnimationController* pController, 
													 IKG3DModel* pModelSFX, 
													 TagSFXCallBackExtraInfo* pExtraInfo)
{
	return E_NOTIMPL;
}

HRESULT KG3DFaceMotionDefaultHandler::OnMotionNotify(IKG3DAnimationController* pController, 
													DWORD dwMotionInfo, 
													TagMotionCallBackExtraInfo *pMotionCallBackExtraInfo)
{
	return E_NOTIMPL;
}

UINT KG3DAnimationMotionTag::GetMotionStart()
{
	return m_uStartFrame;
}

UINT KG3DAnimationMotionTag::GetMotionEnd()
{
	return m_uEndFrame;
}

void KG3DAnimationMotionTag::SetMotionStart(UINT uFrame)
{
	m_uStartFrame = uFrame;
}

void KG3DAnimationMotionTag::SetMotionEnd(UINT uFrame)
{
	m_uEndFrame= uFrame;
}

HRESULT KG3DAnimationMotionTag::Clone(IKG3DAnimationTag* pSrc)
{
	HRESULT hr = E_FAIL;
	KG3DAnimationMotionTag* pSrcTag = dynamic_cast<KG3DAnimationMotionTag*>(pSrc);
	KGLOG_PROCESS_ERROR(pSrcTag);

	m_KeyedFrame  = pSrcTag->m_KeyedFrame;
	m_uStartFrame = pSrcTag->m_uStartFrame;
	m_uEndFrame   = pSrcTag->m_uEndFrame;
	m_pParentModel= pSrcTag->m_pParentModel;
	m_fParentAnimationSpeed = pSrcTag->m_fParentAnimationSpeed;
	m_setSockets = pSrcTag->m_setSockets;

	ASSERT(m_NewData.size() == 0);

	{
		NewDataStructure::iterator i = pSrcTag->m_NewData.begin();
		NewDataStructure::iterator iend = pSrcTag->m_NewData.end();
		while (i!=iend)
		{
			UINT uKey = i->first;
			MotionTagDataRuntime& dataSrc = i->second;
			MotionTagDataRuntime newData;

			strcpy_s(newData.strTagName, 
				TAG_NAME_MAX_LENGTH, 
				dataSrc.strTagName);

			newData.RunState = MTRS_NONE;

			for (size_t t=0;t<dataSrc.Blocks.size();t++)
			{
				void* pSrcData = dataSrc.Blocks[t];

				DWORD dwDataType = *(static_cast<DWORD*>(pSrcData));//type 总在最前面
				DWORD dwSize = GetBlockLength(dwDataType);

				void* pNewData = GetNewBlock(dwDataType);
				KGLOG_PROCESS_ERROR(pNewData);
				memcpy(pNewData, pSrcData, dwSize);

				newData.Blocks.push_back(pNewData);
			}

			m_NewData[uKey] = newData;

			i++;
		}

	}

	hr = S_OK;
Exit0:
	return hr;
}

DWORD KG3DAnimationMotionTag::GetSourceReference(TagSourceInfo *pBuffer, DWORD dwSize)
{
	DWORD dwReturn = INVALIDATESOURCEREFCOUNT;
	KG_PROCESS_ERROR(pBuffer);
	
	//TAGTYPE_MOTION,
	dwReturn = 0;
Exit0:
	return dwReturn;
}

HRESULT KG3DAnimationMotionTag::ChangeResource(UINT uFrame, UINT uIndex, LPCSTR strNew)
{
	return S_OK;
}

HRESULT KG3DAnimationMotionTag::DeleteResource(UINT uFrame, UINT uIndex)
{
	return S_OK;
}

HRESULT KG3DAnimationMotionTag::AddResource(UINT uFrame, LPCSTR strAdd, void *pDefault)
{
	return S_OK;
}

HRESULT KG3DAnimationMotionTag::ChangePlayType(UINT uFrame, UINT uIndex, void *pDefault)
{
	return S_OK;
}

//HRESULT KG3DAnimationMotionTag::AutoFootIntersection(KG3DClip *pClip)
//{
//	HRESULT hResult = E_FAIL;
//	std::vector<int> vecBones;
//	DWORD 
//	KG_PROCESS_ERROR(pClip);
//
//	for (size_t i = 0; i < sizeof(s_strFootBone) / sizeof(const TCHAR*); i++)
//	{
//		int nFoot = pClip->FindBone(s_strFootBone[i]);
//		if (nFoot != -1)
//		{
//			vecBones.push_back(nFoot);
//		}
//	}
//
//	pClip->GetNumFrames()
//	hResult = S_OK;
//Exit0:
//	return hResult;
//}