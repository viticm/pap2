#include "StdAfx.h"
#include "kg3dmodel.h"
#include "KG3DModelTable.h"
#include "kg3dmaterialtable.h"
#include "kg3dcliptable.h"
#include "KG3DGraphicsTool.h"
#include "KG3DRenderState.h"
#include "KG3DModelST.h"
#include "kg3dcamerapreset.h"
#include "KG3DTextureTable.h"
#include "kg3dbip.h"
#include "KG3DMaterialEx.h"
#include "KG3DMeshTable.h"
#include "KG3DSceneSceneEditor.h"
#include "PhysiscSystem/KG3DPhysiscScene.h"
#include "KG3DAnimationTagContainer.h"
#include "KG3DModelSpeedTree.h"
#include "PhysiscSystem/KG3DPhysiscManager.h"
#include "KG3DSFXProjectionTextureMgr.h"
#include "KG3DClipTools.h"
#include "KG3DEffectFileTable.h"
#include "KG3DBillboardCloud.h"
#include "kg3dfacemotionsettings.h"
#include "KG3DFlexibleBody.h"
#include "..\KG3DMultiThreadLoader.h"
#include "KG3DIntersectionBase.h"
#include "KG3DTempleteMultiThead.h"
#include "KG3DModelLightMap.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#define RAYINTERSECT_USEBONEBOX_DISTANCE  500.0f

extern float GetRandom(float fMin, float fMax);

extern BOOL	g_bOnlyShowPitchOn;
extern BOOL volatile g_bThreadExit;
extern DWORD g_dwMainThreadID;
extern TKG3DResourcePool<KG3D_PointLight_Info>g_cPointLightInfoPool;
//float g_fModelLODLevelHighToMiddle = 200.0f;
//float g_fModelLODLevelMiddleToLow = 400.0f;
//float g_fModelLODLevelHighToMiddleSq = g_fModelLODLevelHighToMiddle * g_fModelLODLevelHighToMiddle;
//float g_fModelLODLevelMiddleToLowSq = g_fModelLODLevelMiddleToLow * g_fModelLODLevelMiddleToLow;

//float g_fLODLevel12 = 2000.0f;
//float g_fsLODLevel23 = 8000.0f;

BOOL KG3DModel::m_Static_bUseBoneBox = TRUE;

extern BOOL IsAniLoaded(enuModelPlayAnimationType  AniType,
				 KG3DAnimationTagContainer* pTagInput,
				 KG3DClip*& pClipInput);

extern HRESULT GetAniResource(LPCSTR strAnimationName,
					   enuModelPlayAnimationType&  AniType,
					   KG3DAnimationTagContainer*& pTagInput,
                       KG3DClip*& pClipInput,
                       BOOL IsFirstAni);

////////////////////////////////////////////////////////////////////////////////
static void _LogMatrix(const char cszName[], D3DXMATRIX *pMat)
{
	ASSERT(cszName);
	ASSERT(pMat);
    KGLogPrintf(
        KGLOG_ERR, 
        "%s:[%f %f %f %f] [%f %f %f %f] [%f %f %f %f] [%f %f %f %f]",
        cszName,
		pMat->_11, pMat->_12, pMat->_13, pMat->_14,
        pMat->_21, pMat->_22, pMat->_23, pMat->_24,
        pMat->_31, pMat->_32, pMat->_33, pMat->_34,
        pMat->_41, pMat->_42, pMat->_43, pMat->_44
    );

    return;
}

static int _LogErrorModel(KG3DModel *pModel, unsigned uIndex)
{
    int nResult  = false;
    KG3DModel *pAniModel = NULL;
    IKG3DAnimationController *piAniController = NULL;
    KG3DClip *pClip = NULL;
    LPCTSTR cpszClipST1 = "";
    LPCTSTR cpszClipST2 = "";
    const char *pcszBipName = "";
    const char *pcszModelSTName = "";

    ASSERT(pModel);

    if (pModel->m_pBip)
    {
        pcszBipName = pModel->m_pBip->GetBipFileName();
    }

    pAniModel = pModel;
    if (pModel->m_lpParentModelST)
    {
        pAniModel = pModel->m_lpParentModelST;
        pcszModelSTName = pAniModel->m_scName.c_str();
    }

    piAniController = pAniModel->GetAnimationController(ANICTL_PRIMARY);
    if (piAniController)
    {
        pClip = ((KG3DAnimationController *)piAniController)->GetCurAnimation();
        if (pClip && pClip->IsLoad())
        {
            pClip->GetPathName(&cpszClipST1);
        }
    }

    piAniController = pAniModel->GetAnimationController(ANICTL_SECONDARY);
    if (piAniController)
    {
        pClip = ((KG3DAnimationController *)piAniController)->GetCurAnimation();
        if (pClip && pClip->IsLoad())
        {
            pClip->GetPathName(&cpszClipST2);
        }
    }

    nResult = true;
//Exit0:
    KGLogPrintf(
        KGLOG_ERR, 
        "[BoneMatrixError]ModelST:%s,Model:%s,BoneIndex=%u,Bip=%s,Mesh2BipSize=%u,Ani1=%s,Ani2=%s",
        pcszModelSTName, pModel->m_scName.c_str(), 
        uIndex, pcszBipName, pModel->m_Mesh2Bip.size(), 
        cpszClipST1, cpszClipST2
    );
    return nResult;
}
////////////////////////////////////////////////////////////////////////////////

KG3DModel::KG3DModel()
	:m_fShadowScaleFactor(0)
	//,m_pBillboardTexture(NULL)
	,m_pBillboardCloud(NULL)
	,m_bBillboardCloudCanBeLoaded(TRUE)
	,m_fHeadLineHeight(0.0f)
    ,m_piFlexibleBody(NULL)
{
	m_dwID = 0;
	m_dwType = 0;
	m_ulRefCount = 0;
	m_lpMesh = NULL;
	m_lpMaterial = NULL;

	m_lpClip = NULL;
	m_bIsAnimationed = FALSE;
	m_pParentModel = NULL;
	m_bSelfDestructFramemoveFlag = FALSE;
	m_lpSceneRegionOn = NULL;

	D3DXMatrixIdentity(&m_Matrix);
	m_matWorld = m_Matrix;

	//m_pEventHandler = NULL;
	m_dwPlayAnimationCommandCnt = 0;
	m_fAlpha = 1.0f;
	m_fIlluminationLevel = 1.0f;
	m_fLOD = 1.0F;

	m_pBoneMatricesForRender = NULL;
	m_pBoneLocalMatrixBuffer = NULL;

	m_bShowBones = FALSE;

	m_BindType = ENUM_BIND_NONE;
	m_nBindIndex = -1;
	m_dwVisibleCount = 0;

	m_dwNumSockets = 0;
	m_pSocketMatrix = NULL;

    m_pSFXSuede = NULL;
	
	m_pLogicalBox = NULL;
	m_nCellInX = 0;
    m_nCellInY = 0;
    m_nCellInZ = 0;

	m_pParentMatrix = NULL;

	m_lpParentModelST = NULL;

	//m_nLightMapTextureIndex = -1;
	m_dwDefaultColorCastIndex = 0;

	m_pPropertys = NULL;
	m_dwSceneLayerID = 0;

	m_pBip = NULL;
    m_ChildModels.clear();

    m_nIsSockWave = FALSE;
	m_nCloseFloorFlag = FALSE;
    m_dwPlayType = 0;
    m_pParentSFXSuede       = NULL;
    // afterimage 
    m_nEnagleAfterimage     = FALSE;
    m_fAfterimageLayDelay   = 0.0f;
    m_fCurAftiimageLayDelay = 0.0f;
    m_nAftImageLife         = 500;
    m_nAftImageCallFlag     = FALSE;
    m_dwPrevTime            = g_cGraphicsTool.GetNowTime();
	m_bBoneTransformEnable = FALSE;
	m_bRenderSpecialAlpha = FALSE;
	m_bBindChanged = FALSE;
	m_bClientAdd = FALSE;
	memset(m_pTaggedAnimationContainer, 
		0, 
		sizeof(KG3DAnimationTagContainer*) * ANICTL_COUNT);

	m_bMeshPostLoadProcessed = FALSE;
    m_bBBoxChanged = FALSE;
	m_dwPicthOn = (DWORD)-1;
	m_bDrawBigBox = FALSE;

	//m_bSelectable = FALSE;
	m_bVisible = TRUE;

	m_dwFrameCount = 0;

    //m_bHasBillboardCounld = FALSE;
	m_dwRecordID = 0;
    //m_pBillBoardCloundVertexBuffer = NULL;
    //m_dwBillBoardCount = 0;
	m_lpPhysicsScene = NULL;
	m_bSelectable = FALSE;
	m_bCreateDynamicLight = FALSE;
    m_pPvsOn = NULL;

    m_dwShockMode   = SHOCK_WAVE_ONLY;
   // Data Member Fot Attach To Ground.
	m_AttachType = MODELCLOSE_NONE;
	m_pSceneToAttach = NULL;

	m_vec3LastPosInModel = m_Translation;
	m_LastDirInModel = D3DXQUATERNION(0.0f,1.0f,0.0f,0.0f);
	m_fLastAngleInModel = .0f;
    m_fShockWavePower = 1.f;
	for (DWORD i = 0; i < KG3DCaption2D::MAX_NUM_CAPTION; i++)
	{
		m_dwHeadLineID[i] = CAPTION_INVALIDATE_INDEX;
	}

	m_bHideFromRenderTools = FALSE;

	m_nDetailTableIndex  = 0;
	m_nDetailDefineIndex = 0;
	m_lpDetail           = NULL;

	m_lpPointLightInfo = NULL;
	m_bInMultiThreadLoadList = FALSE;
	m_uOption = 0;
	m_nMtlLoadCmd = 0;
	m_bNeedCheckResourceReady = FALSE;

	m_Translation_Last = D3DXVECTOR3(0,0,0);

	m_lpMeshCurrent = NULL;
	m_lpMeshMedium  = NULL;
	m_lpMeshlLow    = NULL;
	m_lpModelMedium  = NULL;
	m_lpModelLow    = NULL;

    m_bHasAnimation = FALSE;

	m_lpLightMap = NULL;

	m_eRuntimeShaderType = DEFST_COUNT;
}

KG3DModel::~KG3DModel(void)
{
	BOOL bIsPassRecord = FALSE;
	if (g_cRecorder.IsRecord() && m_dwRecordID != 0  && !g_cRecorder.IsLock())
	{
		KG3DRecorder::Parameters MyParameters;
		g_cRecorder.RecordEvent(m_dwRecordID,FT_KG3DMODEL_DESTRUCT,GetCurrentTime(),MyParameters);
		bIsPassRecord = TRUE;
		g_cRecorder.Lock();
	}

    ASSERT(!m_piFlexibleBody);
    	
    //TODO:remove it
    UnInit();

	if (bIsPassRecord)
		g_cRecorder.UnLock();
}

bool KG3DModel::CheckNeedRender()
{
    if (!this)
        return false;

    if (m_nIsSockWave && m_dwShockMode == SHOCK_WAVE_ONLY)
        return false;

    return true;
}

HRESULT KG3DModel::AjustWorldDirection(const D3DXVECTOR3* pVecDir, const D3DXVECTOR3* pSelfDir)
{
    if (!pVecDir || !pSelfDir)
        return E_FAIL;

    D3DXMATRIX  matSelfInv;
    D3DXMATRIX  matParent;
    D3DXMATRIX  matParentInv;

    D3DXVECTOR3 vWorld2Parent;
    D3DXVECTOR3 vSelfDir;

    D3DXMatrixInverse(&matSelfInv, NULL, &m_Matrix);
    matParent = matSelfInv * m_matWorld;

    D3DXMatrixInverse(&matParentInv, NULL, &matParent);
    D3DXVec3TransformNormal(&vWorld2Parent, pVecDir, &matParentInv);
    D3DXVec3TransformNormal(&vSelfDir, pSelfDir, &m_Matrix);

    D3DXVec3Normalize(&vSelfDir, &vSelfDir);
    D3DXVec3Normalize(&vWorld2Parent, &vWorld2Parent);

    float fDot   = D3DXVec3Dot(&vSelfDir, &vWorld2Parent);
    fDot = max(-1.f, min(1.f, fDot));
    float fAngel = acosf(fDot);
    D3DXVECTOR3 vRotAxis;
    D3DXVec3Cross(&vRotAxis, &vSelfDir, &vWorld2Parent);

    D3DXMATRIX matRotation;
    D3DXMATRIX matRotSave;
    D3DXQUATERNION qRotSave;
    D3DXQUATERNION qRot;
    D3DXMatrixRotationAxis(&matRotation, &vRotAxis, fAngel);

    D3DXQuaternionRotationMatrix(&qRot, &matRotation);
    GetRotation(&qRotSave);
    D3DXMatrixRotationQuaternion(&matRotSave, &qRotSave);

    matRotation = matRotSave * matRotation;

    D3DXQuaternionRotationMatrix(&qRot, &matRotation);
    SetRotation(&qRot);

    return S_OK;
}

DWORD KG3DModel::GetSocketCount()
{
    return m_dwNumSockets;
}




HRESULT KG3DModel::GetIEKG3DAnimation(IEKG3DClip **ppiClip)
{
    HRESULT hResult = E_FAIL;
    KG_PROCESS_ERROR(ppiClip);
    
    *ppiClip = m_lpClip;

    hResult = S_OK;
Exit0:
    return hResult;

}
HRESULT KG3DModel::GetIEKG3DAnimationController(IEKG3DAnimationController **ppiAniController)
{
    HRESULT hResult = E_FAIL;

    IKG3DAnimationController *piAniController = NULL;
    KG3DAnimationController *pAniController = NULL;

    piAniController = GetAnimationController(ANICTL_PRIMARY);

    pAniController = dynamic_cast<KG3DAnimationController *>(piAniController);
    KG_PROCESS_ERROR(pAniController);

    *ppiAniController = pAniController;
    
    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT KG3DModel::GetMeshIKG3DResourceBase(IKG3DResourceBase **ppiMesh)
{
    HRESULT hResult = E_FAIL;
    KG_PROCESS_ERROR(ppiMesh);
    *ppiMesh = m_lpMesh;
    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT KG3DModel::GetIEKG3DMesh(IEKG3DMesh **ppiMesh)
{
    HRESULT hResult = E_FAIL;
    KG_PROCESS_ERROR(ppiMesh);
	KG_PROCESS_ERROR(m_lpMesh);
    *ppiMesh = m_lpMesh;
    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT KG3DModel::GetIEKG3DMaterial(IEKG3DMaterial **ppiMat)
{
    HRESULT hResult = E_FAIL;
    KG_PROCESS_ERROR(ppiMat);
    *ppiMat = m_lpMaterial;
    hResult = S_OK;
Exit0:
    return hResult;

}

KG3DMaterial *KG3DModel::GetMaterial()
{
    return m_lpMaterial;//BUG 统一引用计数
}

HRESULT KG3DModel::GetIEKG3DModelST(IEKG3DModelST **ppiModelST)
{
    HRESULT hResult = E_FAIL;
    KGLOG_PROCESS_ERROR(ppiModelST);
    KG3DModelST *pModelST = NULL;
    
    pModelST = dynamic_cast<KG3DModelST *>(this);
    KGLOG_PROCESS_ERROR(pModelST);

    *ppiModelST = pModelST;
    
    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT KG3DModel::GetIEKG3DModelSpeedTree(IEKG3DModelSpeedTree **ppiSpeedTree)
{
    HRESULT hResult = E_FAIL;
    KGLOG_PROCESS_ERROR(ppiSpeedTree);
    KG3DModelSpeedTree *pModelSpeedTree = NULL;
    pModelSpeedTree = dynamic_cast<KG3DModelSpeedTree *>(this);
    KGLOG_PROCESS_ERROR(pModelSpeedTree);
    *ppiSpeedTree = pModelSpeedTree;

    hResult = S_OK;
Exit0:
    return hResult;

}


ULONG STDMETHODCALLTYPE KG3DModel::AddRef()
{
    return (ULONG)KG_InterlockedIncrement((long *)&m_ulRefCount);
}

ULONG STDMETHODCALLTYPE KG3DModel::Release()
{
    ULONG uNewRefCount = (ULONG)KG_InterlockedDecrement((long *)&m_ulRefCount);
    if (uNewRefCount == 0)
        OnReferenceZero(); 
    return uNewRefCount;
}

HRESULT STDMETHODCALLTYPE KG3DModel::QueryInterface(
	/* [in] */ REFIID riid,
	/* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject)
{
	return S_OK;
}
//////////////////////////////////////////////////////////////////////////
//初始化,清除数据
HRESULT KG3DModel::Init()
{
	/*KG3DRecorder::Parameters MyParameters;
	g_cRecorder.RecordEvent(m_dwRecordID,"Init()",GetCurrentTime(),MyParameters);*/
	
	HRESULT hr = E_FAIL;
	m_scName.clear();
    //m_scFullName.clear();

	//m_bChainEnable = true;
    m_bMeshPostLoadProcessed = FALSE;
    m_bBBoxChanged = FALSE;
	m_lpMesh = NULL;
	m_lpMaterial = NULL;
	m_lpClip = NULL;

	for (size_t i = 0; i < ANICTL_COUNT; i++)
	{
		SAFE_RELEASE(m_pTaggedAnimationContainer[i]);
	}

	m_AnimationController.Init();
	m_lpPhysicsScene = NULL;
    hr = S_OK;
//Exit0:
	return hr;
}

HRESULT KG3DModel::UnInit()
{
	m_dwPlayAnimationCommandCnt = 0;
	RemoveCaption();
    if (m_lpPhysicsScene)
    {
        SetPhysicsScene(NULL);
        m_lpPhysicsScene = NULL;
    }
    KG_COM_RELEASE(m_piFlexibleBody);
	
	for (size_t i = 0; i < ANICTL_COUNT; i++)
	{
		SAFE_RELEASE(m_pTaggedAnimationContainer[i]);
	}
    m_bBoneTransformEnable = FALSE;

	size_t i = 0;
	for(i = 0; i < m_vecAnimationLoaded.size(); i++)
	{
		m_vecAnimationLoaded[i]->Release();
	}
	m_vecAnimationLoaded.clear();

    m_nCellInX = 0;
    m_nCellInY = 0;
    m_nCellInZ = 0;
	SAFE_DELETE_ARRAY(m_pLogicalBox);
	if(m_lpMesh)
	{
		m_lpMesh->Unadvise(this);
	}

	//if(m_lpMaterial)
	//	KGLogPrintf(KGLOG_INFO,"ReleaseDebug 0x%x KG3DModel::UnInit %s ",m_lpMaterial, m_scName.c_str());

	SAFE_RELEASE(m_lpMaterial);
	SAFE_RELEASE(m_lpMesh);
	SAFE_RELEASE(m_lpClip);
	
	SAFE_DELETE_ARRAY(m_pSocketMatrix);
	m_dwNumSockets = 0;

	UnBindFromOther();

	while(m_ChildModels.size())
	{
		set<KG3DModelLod>::iterator it = m_ChildModels.begin();
		KG3DModelLod& pLod = (*it);

		if(pLod.pModelHigh)
			pLod.pModelHigh->UnBindFromOther();

	/*	if (pModel->GetType() == MESHTYPE_SFX)
		{
			IKG3DAnimationController* pController = pModel->GetAnimationController(ANICTL_PRIMARY);
			if (pController)
			{
				IKG3DAnimationEventHandler* pEventHandler = pController->GetEventHandler();
				if (pEventHandler)
					pEventHandler->OnPlayAnimationFinish(pController);
			}
		}*/
	}

	SAFE_DELETE_ARRAY(m_pBoneMatricesForRender);
	SAFE_DELETE_ARRAY(m_pBoneLocalMatrixBuffer);

	SAFE_RELEASE(m_lpMeshMedium);
	SAFE_RELEASE(m_lpMeshlLow);
	m_lpMeshCurrent = NULL;
	SAFE_RELEASE(m_lpModelMedium);
	SAFE_RELEASE(m_lpModelLow);
   
	KG_COM_RELEASE(m_pBip);

    SAFE_DELETE(m_pSFXSuede);
    
    m_pParentMatrix = NULL;
    m_lpParentModelST = NULL;
	m_Splitter.UnInit();

	m_Commands.UnInit();

	LeaveSceneRegion(TRUE);
    //LeavePvs();

#if 0
	for (size_t i = 0; i < m_vecLODMesh.size(); ++i)
	{
		LPD3DXMESH& RefPMesh = m_vecLODMesh[i].m_pMesh;
		SAFE_RELEASE(RefPMesh);
	}
	m_vecLODMesh.clear();
#endif

	SAFE_RELEASE(m_pBillboardCloud);

    SAFE_DELETE(m_lpDetail);

	if(m_lpPointLightInfo)
	{
		g_cPointLightInfoPool.FreeResource(m_lpPointLightInfo);
		m_lpPointLightInfo = NULL;
	}
	m_bMeshPostLoadProcessed = FALSE;

    m_bHasAnimation = FALSE;

	SAFE_DELETE(m_lpLightMap);

	return S_OK;
}

HRESULT KG3DModel::_LoadFromFile(const char cszFileName[], 
								unsigned uFileNameHash, 
								unsigned uOption)
{
	HRESULT hRetCode = E_FAIL;
	HRESULT hResult = E_FAIL;

	TCHAR strMtlName[MAX_PATH];

	m_scName = cszFileName;
	//分析出材质名
	{
		TCHAR strFile[MAX_PATH]; 
		TCHAR strDrive[MAX_PATH];
		TCHAR strPath[MAX_PATH];
		TCHAR strExt[MAX_PATH];
		_splitpath_s(cszFileName, 
			strDrive, 
			MAX_PATH,
			strPath, 
			MAX_PATH, 
			strFile, 
			MAX_PATH, 
			strExt,
			MAX_PATH);
		TCHAR strModelName[MAX_PATH];
		sprintf_s(strModelName, 
			MAX_PATH,
			"%s%s%s", 
			strDrive,
			strPath, 
			strFile);
		//m_scFullName = strModelName;
		sprintf_s(strMtlName, 
			MAX_PATH, 
			"%s.mtl", 
			strModelName);
	}



	hResult = LoadMeshFromFile(cszFileName, uOption & MLO_MESH_MASK);
	KG_COM_PROCESS_ERROR(hResult);

	KG_PROCESS_SUCCESS(m_lpMesh->GetType() == MESHTYPE_SFX);
	KG_PROCESS_SUCCESS(uOption & MLO_MATERIAL_NOMAT);

	if(uOption & MLO_MODEL_LOD)
	{
		CheckModelLodFiles(this,uOption);
	}

	hResult = LoadMaterialFromFile(strMtlName, uOption & MLO_MATERIAL_MASK);
	KG_COM_PROCESS_ERROR(hResult);

	m_bSleep = FALSE;

Exit1:
    if (IsLoaded())
	    ComputeBBox();

	m_fShadowScaleFactor = ComputeShadowScaleFactor(m_BBoxLocal);

	//<BillboardCloudV1>
	m_bBillboardCloudCanBeLoaded = KG3DBillboardCloud::CheckIsBillboardExist(m_scName.c_str());
	if(g_eEngineOption.bUseProgressMeshLOD && m_bBillboardCloudCanBeLoaded)
	{
		SAFE_RELEASE(m_pBillboardCloud);
		m_pBillboardCloud = KG3DBillboardCloud::Build(cszFileName,*m_lpMaterial,0);
	}
	//<BillboardCloudV1>

	hRetCode = S_OK;
Exit0:
	return hRetCode;
}
//////////////////////////////////////////////////////////////////////////
//加载,保存
HRESULT KG3DModel::LoadFromFile(const char cszFileName[], 
								unsigned uFileNameHash, 
								unsigned uOption)
{
	HRESULT hr = E_FAIL;
	BOOL bIsPassRecord = FALSE;
	DWORD dwNowTime = timeGetTime();

	if (g_cRecorder.IsRecord() && !g_cRecorder.IsLock())
	{
		if (uOption & MLO_MODEL_RECORD)
		{
			m_dwRecordID =  KG3DRecorder::GetMasterID();
			KG3DRecorder::Parameters MyParameters;
			MyParameters.nNum = 4;
			MyParameters.size_tDataSize = sizeof(TrueType) + 
				(MAX_PATH) * sizeof(char) + sizeof(uFileNameHash) + sizeof(uOption);
			char* pData = new char[MyParameters.size_tDataSize];
			MyParameters.pData = pData;
			MyParameters.nFrom[0] = 0;		
			*(TrueType*)(&pData[MyParameters.nFrom[0]]) = TT_MODEL;
			MyParameters.nFrom[1] = sizeof(TrueType);
			strcpy(&(pData[MyParameters.nFrom[1]]),cszFileName);
			MyParameters.nFrom[2] = sizeof(TrueType) + MAX_PATH * sizeof(char);
			*(unsigned*)(&pData[MyParameters.nFrom[2]]) = uFileNameHash;
			MyParameters.nFrom[3] = sizeof(TrueType) + MAX_PATH * sizeof(char) + sizeof(uFileNameHash);
			*(unsigned*)(&pData[MyParameters.nFrom[3]]) = uOption;
			g_cRecorder.RecordEvent(m_dwRecordID,FT_KG3DMODEL_LOADFROMFILE,GetCurrentTime(),MyParameters);
			bIsPassRecord = TRUE;
			g_cRecorder.Lock();
		}
	}

	m_uOption = uOption;
	m_scName  = cszFileName;

/*	if((uOption & MLO_MESH_MULTI) && (g_eEngineOption.bUseMultiThreadLoad))
	{
		if (FAILED(hr = g_cMultiThreadLoader.TryMultiThreadLoad_Model(this)))
        {
            switch (hr)
            {
            case E_MUTL_LOAD_ARRAY_FULL :
                hr = _LoadFromFile(cszFileName,uFileNameHash,uOption);
                break;
            case E_MUTL_LOAD_ALREADY_IN_ARRAY :
                hr = S_OK;
                break;
            default :
                hr = E_FAIL;
                break;
            }
        }
        else
            hr = S_OK;
	}
	else
	{
		hr =_LoadFromFile(cszFileName,uFileNameHash,uOption);
	}*/

	hr = _LoadFromFile(cszFileName, uFileNameHash, uOption);

//Exit0:
	if (bIsPassRecord)
		g_cRecorder.UnLock();

	DWORD dwCost = timeGetTime() - dwNowTime;
	if(dwCost >= 30)
	{
		//KGLogPrintf(KGLOG_ERR, "%d 1:%d 2:%d 3:%d KG3DModel::LoadFromFile %s.",dwCost,dwCost1,dwCost2,dwCost3,cszFileName);
	}

	return hr;
}


HRESULT KG3DModel::SaveToFile(const char cszFileName[],   unsigned uOption)
{
	HRESULT hr = S_OK;
	TCHAR strMeshFileName[MAX_PATH];
	TCHAR strMtlFileName[MAX_PATH];
	TCHAR strDriver[MAX_PATH];
	TCHAR strPath[MAX_PATH];
	TCHAR strFile[MAX_PATH];
	TCHAR strExt[MAX_PATH];

	if(uOption == 0)
	{
		_splitpath(cszFileName, strDriver, strPath, strFile, strExt);

		if (m_lpMesh)
		{
			sprintf(strMeshFileName, "%s%s%s.Mesh", strDriver, strPath, strFile);
			hr = m_lpMesh->SaveToFile(strMeshFileName, uOption);
		}

		if (m_lpMaterial)
		{
			sprintf(strMtlFileName, "%s%s%s.Mtl", strDriver, strPath, strFile);
			hr = m_lpMaterial->SaveToFile(strMtlFileName, uOption);
		}
	}
	else
	{
		_ASSERTE(0);
		if (m_lpMesh)
		{
			hr = m_lpMesh->SaveToFile(m_lpMesh->m_scName.c_str(), 0);
		}

		if (m_lpMaterial)
		{
			hr = m_lpMaterial->SaveToFile(m_lpMaterial->m_scName.c_str(), 0);
		}
	}


	return hr;
}

//////////////////////////////////////////////////////////////////////////
//ID,type
DWORD KG3DModel::GetID()
{
	return m_dwID;
}

unsigned KG3DModel::GetType()
{
	return m_dwType;
}

//////////////////////////////////////////////////////////////////////////
//内存管理


HRESULT KG3DModel::PrepareRender(unsigned uOption)//压缩数据
{
	//KG3DRecorder::Parameters MyParameters;
	//MyParameters.nNum = 1;
	//MyParameters.size_tDataSize = sizeof(uOption);
	//char* pData = new char[MyParameters.size_tDataSize];
	//MyParameters.pData = pData;
	//MyParameters.nFrom[0] = 0;
	//*(unsigned*)(&pData[MyParameters.nFrom[0]]) = uOption;
	//g_cRecorder.RecordEvent(m_dwRecordID,"PrepareRender(unsigned uOption)",
	//	GetCurrentTime(),MyParameters);
	
	if(m_lpMesh)
	{
		if (m_lpMesh->GetType() == MESHTYPE_DEFAULT)
		{
			m_lpMesh->SetModel(this);
		}
	}
	return S_OK;
}

HRESULT KG3DModel::PostRender()//恢复数据，对象可以立即被使用
{


	if(m_lpMesh)
	{
		if (m_lpMesh->GetType() == MESHTYPE_DEFAULT)
		{
			m_lpMesh->SetModel(NULL);
		}
	}


	return S_OK;
}

HRESULT KG3DModel::GetDateInformation(int* pnMem,int* pnVedioMem,int* pnConMem,int* pnConVMem)//得到数据信息，参数依次为占用内存大小，现存大小，压缩后内存大小，压缩后内存大小
{
	return S_OK;;
}

unsigned   KG3DModel::GetLastUseTime()//查询上次使用的时间
{
	return S_OK;;
}

HRESULT KG3DModel::Reload()
{
    return S_OK;
}

unsigned   KG3DModel::GetUseFrequency()//查询使用频率，一段时间里的使用次数
{
	return S_OK;;
}

//HRESULT KG3DModel::SkinMeshRenderPerpare()
//{
//	HRESULT hRetCode = E_FAIL;
//	
//
//	hRetCode = S_OK;
//Exit0:
//	return hRetCode;
//}
HRESULT KG3DModel::MeshPostLoadProcess()
{
	HRESULT hrResult  = E_FAIL;
    HRESULT hrRetCode = E_FAIL;
	DWORD dwStartTime = timeGetTime();

	PostMeshLoadData Data = { &m_pBoneLocalMatrixBuffer, 
		&m_pBoneMatricesForRender, 
		&m_dwNumSockets, 
		&m_pSocketMatrix };
	D3DXMATRIX mat;

	//Create render matrices for skinned mesh;
	hrRetCode = m_lpMesh->PostLoadProcess((void*)&Data);
	KGLOG_COM_PROCESS_ERROR(hrRetCode);

    m_bMeshPostLoadProcessed = TRUE;

    m_BBoxLocal.Clear();
    ComputeBBox();
    m_bBBoxChanged = TRUE;

	hrRetCode = KG3DCreateFlexibleBodyFromModel(this, &m_piFlexibleBody);
	if (FAILED(hrRetCode))
    {
        // if is in editor, we should enable the model continue load, then it can fix error
        if ((g_bClient) && (hrRetCode != KG3D_E_NO_FLEXIBLE_BODY_DATA))
		    KGLOG_COM_PROCESS_ERROR(hrRetCode);
    }
	m_fShadowScaleFactor = ComputeShadowScaleFactor(m_BBoxLocal);
	
	hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        KG_COM_RELEASE(m_piFlexibleBody);
    }

	DWORD dwCost = timeGetTime() - dwStartTime;
	if(dwCost > 30)
	{
		KGLogPrintf(KGLOG_ERR, "%d KG3DModel::MeshPostLoadProcess  %s.",dwCost , m_scName.c_str());
	}

	return hrResult;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//网格
HRESULT KG3DModel::LoadMeshFromFile(LPCSTR strFileName, DWORD dwOption)
{
	HRESULT hRetCode = E_FAIL;
    HRESULT hResult  = E_FAIL;
	KG3DMesh *pMesh  = NULL;
	DWORD dwNowTime = timeGetTime();
	DWORD dwCost1 = 0;
	
	KG_PROCESS_ERROR(strFileName);

	hResult = g_cMeshTable.LoadResourceFromFile(strFileName, 
		0, 
		dwOption, 
		(IKG3DResourceBase**)&pMesh);
	KG_COM_PROCESS_ERROR(hResult);
	dwCost1 = timeGetTime() - dwNowTime;

	//如果是同一个Mesh，就不用做后续的处理了
	KG_PROCESS_SUCCESS(pMesh == m_lpMesh);
	
	if(m_lpMesh)
	{
		m_lpMesh->Unadvise(this);
	}
	SAFE_RELEASE(m_lpMesh);

	m_lpMesh = pMesh;
	m_lpMesh->AddRef();
	m_lpMesh->Advise(this);

	DWORD dwThreadID = ::GetCurrentThreadId();
	BOOL bInMainThread = (dwThreadID == g_dwMainThreadID);

	if(bInMainThread)
	{
		if ((!g_eEngineOption.bUseMultiThreadLoad) || 
			!(dwOption & MLO_MESH_MULTI))
		{
			m_lpMesh->CheckLoadingState();	
		}

		if (m_lpMesh->m_eDynamicLoadState == KG3DMesh::STATE_VIDEOMEM)
		{
			hResult = MeshPostLoadProcess();
			KG_COM_PROCESS_ERROR(hResult);
		}
	}

Exit1:
	hRetCode = S_OK;
Exit0:
	SAFE_RELEASE(pMesh);

	DWORD dwCost = timeGetTime() - dwNowTime;
	if(dwCost >= 50)
	{
		//KGLogPrintf(KGLOG_ERR, "%d 1:%d KG3DModel::LoadMeshFromFile %s.",dwCost,dwCost1,strFileName);
	}
	return hRetCode;
}


KG3DMesh* KG3DModel::GetMesh()
{
	/*//原来是带引用计数的，只检查到内部暂存式的使用，所以不需要引用计数了，外部的引用我都改好 Last modify by Chen TianHong:2008-6-18 9:50:29
	if (m_lpMesh)
		{
			m_lpMesh->AddRef();
		}*/
	
	return m_lpMesh;
}

HRESULT KG3DModel::SetMesh(KG3DMesh* pMesh)
{
	if(m_lpMesh)
	{
		m_lpMesh->Unadvise(this);
	}
	SAFE_RELEASE(m_lpMesh);

	m_lpMesh = pMesh;
	if (m_lpMesh)
	{
		m_lpMesh->AddRef();
		m_lpMesh->Advise(this);
	}
	return S_OK;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//材质
HRESULT KG3DModel::LoadMaterialFromFile(LPCSTR strFileName, DWORD Option)//从文件中载入材质，根据文件中材质的数目和模型材质的数目进行自动匹配
{
	HRESULT hr = E_FAIL;

	if (g_cRecorder.IsRecord() && !g_cRecorder.IsLock()  && m_dwRecordID != 0 )
	{
		KG3DRecorder::Parameters MyParameters;
		MyParameters.nNum = 2;
		MyParameters.size_tDataSize = MAX_PATH * sizeof(char) + sizeof(Option);
		char* pData = new char[MyParameters.size_tDataSize];
		MyParameters.pData = pData;
		MyParameters.nFrom[0] = 0;		
		strcpy(pData,strFileName);
		MyParameters.nFrom[1] = MAX_PATH * sizeof(char);
		*(DWORD*)(&pData[MyParameters.nFrom[1]]) = Option;
		g_cRecorder.RecordEvent(m_dwRecordID,FT_KG3DMODEL_LOADMATERIALFROMFILE,GetCurrentTime(),MyParameters);
	}

	BOOL bLoadNow = TRUE;
	bLoadNow &= IsLoaded();
	if(m_lpMaterial)
	{
		if(!m_lpMaterial->IsLoaded())
			bLoadNow = FALSE;
	}

	if(bLoadNow && m_nMtlLoadCmd==0 )//取消材质的多线程加载先
	{
		hr = _LoadMaterialFromFile(strFileName,Option);
		KGLOG_COM_PROCESS_ERROR(hr);
	}
	else
	{
		KG3DModelCommand_KG3DModel_LoadMaterialFromFile* pCmd = new KG3DModelCommand_KG3DModel_LoadMaterialFromFile(strFileName,Option);
		m_Commands.AddCommand(pCmd,FALSE);
		m_nMtlLoadCmd++;
		hr = S_OK;
	}
Exit0:
	return hr;
}

HRESULT KG3DModel::_LoadMaterialFromFile(LPCSTR strFileName, DWORD Option)//从文件中载入材质，根据文件中材质的数目和模型材质的数目进行自动匹配
{

	HRESULT hr = E_FAIL;
	KG3DMaterial* pNewMtl = NULL;

	hr = g_cMaterialTable.LoadResourceFromFile(strFileName, 
		0, 
		Option, 
		(IKG3DResourceBase **)&pNewMtl);
	KGLOG_COM_PROCESS_ERROR(hr);

	hr = SetMaterial(pNewMtl);
Exit0:
	if(FAILED(hr))
	{
		KGLogPrintf(KGLOG_ERR,"KG3DModel::_LoadMaterialFromFile %s Failed",strFileName);
	}
	return hr;
}

HRESULT KG3DModel::LoadMaterialFromFileEx(LPSTR strFileName,DWORD Option,DWORD SrcID,DWORD DestID)//手动配置材质，将模型中第DestID的材质替换为文件中第SrcID号材质，材质记数从0开始
{
	return S_OK;
}

HRESULT KG3DModel::SetMaterial(IEKG3DMaterial* pMtl)
{
	HRESULT hr = E_FAIL;
	DWORD dwThreadID = ::GetCurrentThreadId();
	BOOL bInMainThread = (dwThreadID == g_dwMainThreadID);
	KG3DMaterial* lpMaterial = static_cast<KG3DMaterial *>(pMtl);

	if(bInMainThread)
	{
		hr = _SetMaterial(pMtl);//确保更换材质的动作在主线程中完成，否则绘制会出现问题
		SAFE_RELEASE(pMtl);
	}
	else
	{
		KG3DModelCommand_SetMaterial *pCmd = new KG3DModelCommand_SetMaterial(lpMaterial);
		m_Commands.AddCommand(pCmd,FALSE);
		SAFE_RELEASE(pMtl);
		hr = S_OK;
	}
	return hr;
}

HRESULT KG3DModel::_SetMaterial(IEKG3DMaterial* pMtl)
{
	SAFE_RELEASE(m_lpMaterial);
	m_lpMaterial = static_cast<KG3DMaterial *>(pMtl);
	if (m_lpMaterial)
	{
		m_lpMaterial->AddRef();

		if(m_lpMaterial->GetMaterialDetailDefineCount() && m_nDetailDefineIndex)
		{
			SetDetail(m_nDetailTableIndex,m_nDetailDefineIndex);
		}

		if(m_lpModelMedium)
		{
			m_lpModelMedium->_SetMaterial(pMtl);
		}
		if(m_lpModelLow)
		{
			m_lpModelLow->_SetMaterial(pMtl);
		}
	}
	return S_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//动画
HRESULT KG3DModel::LoadAnimationFromFile(LPCSTR strFileName,DWORD dwOption)//从文件中载入动画，可以根据Option选择覆盖还是添加，或者去处文件中指定类型的动作
{
	HRESULT hr = E_FAIL;
	KG3DClip* pClip = NULL;
	DWORD dwStartTime = timeGetTime();

	hr = g_cClipTable.LoadResourceFromFile(strFileName, 0, 0/*dwOption*/, &pClip);
	KG_COM_PROCESS_ERROR(hr);
	m_vecAnimationLoaded.push_back(pClip);

Exit0:
	{
		DWORD dwCost = timeGetTime() - dwStartTime;
		if(g_cEngineOption.bEnableTimeOptimizeLog && dwCost > 10)
		{
			KGLogPrintf(KGLOG_INFO,"TimeOptimize %d KG3DModel::LoadAnimationFromFile %s ",dwCost , strFileName);
		}
	}
	return hr;
}

HRESULT KG3DModel::LoadAnimationFromFileEx(LPCSTR strFileName,DWORD Option,DWORD SrcID,DWORD DestID) //从文件中载入指定动画，并覆盖指定DestID的动画
{
	return S_OK;
}

HRESULT KG3DModel::PlayAnimation(DWORD dwPlayType, 
								 FLOAT fSpeed, 
								 int   nOffsetTime, 
								 PVOID pReserved, 
								 PVOID pUserdata,
								 enuAnimationControllerPriority Priority)//用指定播放方式播放指定动画，Time 是相对于动画起点的相对时间
{
	HRESULT hr = E_FAIL;
	size_t uSize = m_vecAnimationLoaded.size();
	m_Splitter.EnableWarper(FALSE);
	SAFE_RELEASE(m_pTaggedAnimationContainer[Priority]);
	if (uSize)
	{
		KG3DClip* pClip = m_vecAnimationLoaded[uSize - 1];
		hr = PlayAnimation(dwPlayType, 
			pClip->m_scName.c_str(), 
			fSpeed, 
			nOffsetTime, 
			pReserved, 
			pUserdata,
			Priority);
	}
	return hr;
}


HRESULT KG3DModel::UpdateBoneMesh(KG3DScene* pScene)
{
	return m_boneMash.Attach(this, pScene);
}

void KG3DModel::SetBoneMeshAttachScene(KG3DScene* pScene)
{
	m_boneMash.SetAttachScene(pScene);
}

HRESULT KG3DModel::UpdateBoneBBox(KG3DScene* pScene)
{
	HRESULT hr = E_FAIL;
	
	hr = UpdateSelBoneBBox(pScene,m_pBoneMatricesForRender);

	return S_OK;
}

HRESULT KG3DModel::UpdateBoneMeshSel()
{
    return m_boneMash.UpdateSelBone(TRUE);
}

HRESULT KG3DModel::Helper_PlayAnimationTagedAnimation(
    DWORD dwPlayType,
    LPCSTR strFileName,
    FLOAT fSpeed,
    int nOffsetTime,
    PVOID pReserved,
    enuAnimationControllerPriority Priority,
    enuModelPlayAnimationType AnimationType,
    KG3DAnimationTagContainer* pTagContainerNew,
    KG3DClip* pClip
)
{
    HRESULT hrResult = E_FAIL;
    HRESULT hrRetCode = E_FAIL;

    KGLOG_PROCESS_ERROR(strFileName);
    KGLOG_PROCESS_ERROR(pTagContainerNew);

    SAFE_RELEASE(m_pTaggedAnimationContainer[Priority]);
    m_pTaggedAnimationContainer[Priority] = pTagContainerNew;
    m_pTaggedAnimationContainer[Priority]->AddRef();

    hrRetCode = m_pTaggedAnimationContainer[Priority]->AttachToModel(this);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    hrRetCode = m_pTaggedAnimationContainer[Priority]->Start(
        fSpeed, nOffsetTime, dwPlayType, Priority
    );
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KG3DModel::_PlayAnimation(
    DWORD dwPlayType, 
    LPCTSTR strAnimationName, 
    FLOAT fSpeed, 
    int   nOffsetTime, 
    PVOID pReserved,
    PVOID pUserdata,
    enuAnimationControllerPriority Priority,
    enuModelPlayAnimationType AnimationType,
    KG3DAnimationTagContainer* pTagContainerNew,
    KG3DClip* pClip
)
{
    HRESULT hrResult = E_FAIL;
    HRESULT hrRetCode = E_FAIL;
    IKG3DAnimationController* pController = NULL;

	static KG3DModel::pHelper_PlayAnimation s_HelperPlayAnimationFunc[] = 
	{
		&KG3DModel::Helper_PlayAnimationNormal, 
		&KG3DModel::Helper_PlayAnimationTagedAnimation 
	};

    KGLOG_PROCESS_ERROR(strAnimationName);

	pController = GetAnimationController(Priority);
	if (pController)
	{
		pController->SetUserdata(pUserdata);
		pController->RemoveAnimationControllerUpdateNotifier(m_pTaggedAnimationContainer[Priority]);
	}

	SAFE_RELEASE(m_pTaggedAnimationContainer[Priority]);

	KGLOG_PROCESS_ERROR(AnimationType < MPAT_COUNT);
    
	hrRetCode = (this->*s_HelperPlayAnimationFunc[AnimationType])(dwPlayType, 
		strAnimationName, 
		fSpeed, 
		nOffsetTime, 
		pReserved,
		Priority,
		AnimationType,
		pTagContainerNew,
		pClip);
    KG_COM_PROCESS_ERROR(hrRetCode);

	m_dwFrameCount = 0;

    m_bHasAnimation = TRUE;

    hrResult = S_OK;
Exit0:
	return hrResult;
}


HRESULT KG3DModel::PlayAnimation(DWORD dwPlayType, 
								 LPCTSTR strAnimationName, 
								 FLOAT fSpeed, 
								 int   nOffsetTime, 
								 PVOID pReserved,
								 PVOID pUserdata,
								 enuAnimationControllerPriority Priority)
{
	HRESULT hrRetCode = E_FAIL;

	BOOL bIsPassRecord = FALSE;
	if (g_cRecorder.IsRecord() && !g_cRecorder.IsLock()  && m_dwRecordID != 0 )
	{
			KG3DRecorder::Parameters MyParameters;
			MyParameters.nNum = 4;
			MyParameters.size_tDataSize = MAX_PATH * sizeof(char) +
				                                sizeof(dwPlayType) + sizeof(fSpeed) + sizeof(nOffsetTime);
			char* pData = new char[MyParameters.size_tDataSize];
			MyParameters.pData = pData;
			MyParameters.nFrom[0] = 0;		
			*(DWORD*)(&pData[MyParameters.nFrom[0]]) = dwPlayType;
			MyParameters.nFrom[1] = sizeof(dwPlayType);
            if (strAnimationName == NULL)
                pData[MyParameters.nFrom[1]] = NULL;
            else
                strcpy(&(pData[MyParameters.nFrom[1]]),strAnimationName);
			MyParameters.nFrom[2] = MAX_PATH * sizeof(char) + sizeof(dwPlayType);
			*(FLOAT*)(&pData[MyParameters.nFrom[2]]) = fSpeed;
			MyParameters.nFrom[3] = MAX_PATH * sizeof(char) + sizeof(dwPlayType) + sizeof(fSpeed);
			*(int*)(&pData[MyParameters.nFrom[3]]) = nOffsetTime;
			g_cRecorder.RecordEvent(m_dwRecordID,FT_KG3DMODEL_PLAYANIMATION,GetCurrentTime(),MyParameters);
			bIsPassRecord = TRUE;
			g_cRecorder.Lock();
	}

	//////////////////////////////////////////////////////////////////////////
	enuModelPlayAnimationType  AniType = MPAT_INVALIDATE;
	KG3DAnimationTagContainer* pTagInput = NULL;
	KG3DClip* pClipInput = NULL;
    BOOL bIsFirstAni = (!m_bHasAnimation) && (m_dwType == MESHTYPE_MODELST) && g_bClient;

	hrRetCode = GetAniResource(strAnimationName,AniType,pTagInput,pClipInput, bIsFirstAni);
	KGLOG_COM_PROCESS_ERROR(hrRetCode);

	BOOL bAniLoaded = IsAniLoaded(AniType,pTagInput,pClipInput);

	if(IsLoaded() && bAniLoaded && !m_dwPlayAnimationCommandCnt)
	{
		hrRetCode = _PlayAnimation(dwPlayType,strAnimationName,fSpeed,nOffsetTime,pReserved,pUserdata,Priority,AniType,pTagInput,pClipInput);
	}
	else
	{
		KG3DModelCommand_PlayAnimation* pCmd = new KG3DModelCommand_PlayAnimation(
			dwPlayType,
			strAnimationName,
			fSpeed,
			nOffsetTime,
			pReserved,
			pUserdata,
			Priority,
			AniType,pTagInput,pClipInput);
		m_dwPlayAnimationCommandCnt++;
		m_Commands.AddCommand(pCmd,TRUE);
		hrRetCode = S_OK;
	}

	if (bIsPassRecord)
		g_cRecorder.UnLock();

Exit0:
	SAFE_RELEASE(pTagInput);
	SAFE_RELEASE(pClipInput);
	return hrRetCode;
}

HRESULT KG3DModel::_Helper_PlayAnimationNormal(
	DWORD dwPlayType, 
	KG3DClip* pClip, 
	FLOAT fSpeed, 
	int   nOffsetTime, 
	DWORD dwTweenTime,
	enuAnimationControllerPriority Priority)
{
    HRESULT hrResult = E_FAIL;
    HRESULT hrRetCode = E_FAIL;

    hrRetCode = GetClipTool().CheckClipByMesh(pClip, m_lpMesh);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    switch (dwPlayType)
    {
        case ENU_ANIMATIONPLAY_ADDCURRENT_ONCE:
            dwPlayType = ENU_ANIMATIONPLAY_ONCE;
            break;
        case ENU_ANIMATIONPLAY_ADDCURRENT:
            dwPlayType = ENU_ANIMATIONPLAY_CIRCLE;
            break;
    }

    m_AnimationController.Detach();
    m_AnimationController.Attach(pClip);
    m_AnimationController.StartAnimation(dwPlayType, nOffsetTime, fSpeed);

	m_dwPlayType = dwPlayType;

	hrResult = S_OK;
Exit0:
	return hrResult;
}

HRESULT KG3DModel::Helper_PlayAnimationNormal(
    DWORD dwPlayType, 
    LPCTSTR strAnimationName, 
    FLOAT fSpeed, 
    int   nOffsetTime, 
    PVOID pReserved,
	enuAnimationControllerPriority Priority,
	enuModelPlayAnimationType AnimationType,
	KG3DAnimationTagContainer* pTagContainerNew,
	KG3DClip* pClip)
{
    HRESULT hrResult = E_FAIL;
    HRESULT hrRetCode = E_FAIL;
    DWORD dwTweenTime = 0;
    if (pReserved) 
    {
        dwTweenTime = *(DWORD*)pReserved;
    }

    hrRetCode = _Helper_PlayAnimationNormal(dwPlayType,pClip,fSpeed,nOffsetTime,dwTweenTime,Priority);
    KG_COM_PROCESS_ERROR(hrRetCode);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KG3DModel::SetAnimation(KG3DClip* pClip)
{
    HRESULT hrResult = E_FAIL;

    KG_PROCESS_ERROR(pClip);

    SAFE_RELEASE(m_lpClip);
    m_lpClip = pClip;
    m_lpClip->AddRef();

    hrResult = S_OK;
Exit0:
    return hrResult;
}


/////////////////////////////////////////////////////////
// chain member
/////////////////////////////////////////////////////////
HRESULT KG3DModel::ChainAdd(IKG3DModel *pOtherModel)
{
	/*ChainIter_t iter = std::find(m_vecChainModel.begin(), m_vecChainModel.end(), pOtherModel);
	if (iter == m_vecChainModel.end())
	{
		m_vecChainModel.push_back(pOtherModel);
	}*/
	return S_OK;
}

HRESULT KG3DModel::ChainRemove(IKG3DModel *pOtherModel)
{
	/*ChainIter_t iter = std::find(m_vecChainModel.begin(), m_vecChainModel.end(), pOtherModel);
	if (iter != m_vecChainModel.end())
	{
		m_vecChainModel.erase(iter);
	}*/
	return S_OK;
}


HRESULT KG3DModel::ChainEnable() 
{
	//m_bChainEnable = true;
	return S_OK;
}

HRESULT KG3DModel::ChainDisable()
{
	//m_bChainEnable = false;
	return S_OK;
}


void KG3DModel::UpdateChildModel()
{
	HRESULT hr = E_FAIL;
	D3DXMATRIX matParent;

	std::set<KG3DModelLod> UpdateList = m_ChildModels;

	for (std::set<KG3DModelLod>::iterator i = UpdateList.begin(); 
		i != UpdateList.end(); 
		i++)
	{
		KG3DModelLod& pChild = *i; 
		KG3DModel* pLast = pChild.pModelCurrent;

		if(pChild.pModelCurrent)
		{
			pChild.pModelCurrent->SetClientAdd(m_bClientAdd);
			hr = pChild.pModelCurrent->FrameMove();
		}

		if(pChild.pModelCurrent != pChild.pModelHigh)
		{
			if(pLast != pChild.pModelCurrent)
			{
				*(KG3DTransformation*)pChild.pModelCurrent = *(KG3DTransformation*)pChild.pModelHigh;
			}
			pChild.pModelHigh->SetClientAdd(m_bClientAdd);
			//hr = pChild.pModelHigh->FrameMove();
		}
	}
}

HRESULT KG3DModel::_BindToSocketProcess(KG3DModel* pParent,int nSocket,KG3DModel *pTemp, const char cszSocketName[])
{
	m_pParentModel = pParent;
	m_BindType = ENUM_BIND_SOCKET;
	m_nBindIndex = nSocket;
	m_scBindToObjName = cszSocketName;

	SetAlpha(pTemp->GetAlpha(), TRUE);
	SetHighLightLevel(pTemp->GetHightLightLevel(), TRUE);

	if(m_pParentModel)
	{
		SetPhysicsScene(m_pParentModel->m_lpPhysicsScene);
		SetClientAdd(m_pParentModel->IsClientAdd());
	}

	EnableHideFromRenderTools(FALSE);

	return S_OK;
}

HRESULT KG3DModel::_BindToSocket(KG3DModel *pTemp, const char cszSocketName[])
{
	HRESULT hr = E_FAIL;
	int nSocket = -1;
	m_nBindIndex = -1;
	BindExtraInfo Info;

	KG3DModel* pModelMedium = this->m_lpModelMedium;
	KG3DModel* pModelLow = this->m_lpModelLow;

	UnBindFromOther();
	nSocket = pTemp->FindSocket(cszSocketName, &Info);
	KGLOG_PROCESS_ERROR(nSocket != -1);

	m_pParentModel = Info.pModel;
	m_pParentModel->OnBind(this,pModelMedium,pModelLow);

	this->        _BindToSocketProcess(Info.pModel,nSocket,pTemp,cszSocketName);

	if(pModelMedium)
	{
		*(KG3DTransformation*)pModelMedium   = *(KG3DTransformation*)this;
		pModelMedium->_BindToSocketProcess(Info.pModel,nSocket,pTemp,cszSocketName);
	}

	if(pModelLow)
	{
		*(KG3DTransformation*)pModelLow     = *(KG3DTransformation*)this;
		pModelLow->   _BindToSocketProcess(Info.pModel,nSocket,pTemp,cszSocketName);
	}

	if(_strcmpi(cszSocketName,"S_Face")==0)
		m_bNeedCheckResourceReady = TRUE;

	hr = S_OK;
Exit0:
	return hr;
}

HRESULT KG3DModel::BindToSocket(IKG3DModel *pModel, const char cszSocketName[])//绑定
{
	HRESULT hr = E_FAIL;
	KG3DModel *pTemp = static_cast<KG3DModel*>(pModel);

	BOOL bIsPassRecord = FALSE;

	KGLOG_PROCESS_ERROR(pTemp);
	KGLOG_PROCESS_ERROR(cszSocketName);

	if (g_cRecorder.IsRecord() && m_dwRecordID != 0 && pTemp->m_dwRecordID != 0 && !g_cRecorder.IsLock())
	{
		KG3DRecorder::Parameters MyParameters;
		MyParameters.nNum = 2;
		MyParameters.size_tDataSize = sizeof(DWORD) + MAX_PATH * sizeof(char);
		char* pData = new char[MyParameters.size_tDataSize];
		MyParameters.pData = pData;
		MyParameters.nFrom[0] = 0;
		*(DWORD*)(&pData[MyParameters.nFrom[0]]) = pTemp->m_dwRecordID;
		MyParameters.nFrom[1] = sizeof(DWORD);
		strcpy(&pData[MyParameters.nFrom[1]],cszSocketName);
		g_cRecorder.RecordEvent(m_dwRecordID,FT_KG3DMODEL_BINDTOSOCKET,GetCurrentTime(),MyParameters);
		bIsPassRecord = TRUE;
		g_cRecorder.Lock();
	}



	if(pTemp->IsLoaded())
	{
		BindExtraInfo Info;
		int nSocket = pTemp->FindSocket(cszSocketName, &Info);
		if(nSocket != -1)
		{

			hr = _BindToSocket(pTemp,cszSocketName);
		}
		else
		{
			AddRef();
			KG3DModelCommand_BindToSocket* pCmd = new KG3DModelCommand_BindToSocket(this,cszSocketName);
			pTemp->m_Commands.AddCommand(pCmd,FALSE);
			hr = S_OK;
		}
	}
	else
	{
		AddRef();
		KG3DModelCommand_BindToSocket* pCmd = new KG3DModelCommand_BindToSocket(this,cszSocketName);
		pTemp->m_Commands.AddCommand(pCmd,FALSE);
		hr = S_OK;
	}

Exit0:
	if (bIsPassRecord)
		g_cRecorder.UnLock();
	return hr;
}

HRESULT KG3DModel::_BindToBone(KG3DModel* pModel, const char* strBoneName)
{
	HRESULT hr = E_FAIL;
	int nBoneIndex = -1;
	m_nBindIndex = -1;
	BindExtraInfo Info;
	KG3DModel *pTemp = static_cast<KG3DModel*>(pModel);

	KGLOG_PROCESS_ERROR(pTemp);
	KG_PROCESS_ERROR(strBoneName);
	KG_PROCESS_ERROR(pModel != this);
	
	UnBindFromOther();
	nBoneIndex = pTemp->FindBone(strBoneName, &Info);
	if (nBoneIndex != -1)
    {
	    m_pParentModel = Info.pModel;
	    m_BindType = ENUM_BIND_BONE;
	    m_nBindIndex = nBoneIndex;
	    m_scBindToObjName = strBoneName;
		
        //if (m_dwType != MESHTYPE_SFX)
        {
            D3DXVECTOR3 vScaling;
            GetScaling(&vScaling);
            vScaling /= 2.54f;
            SetScaling(&vScaling);
        }
    }
    else
    {
		m_pParentModel = pTemp;
		m_pParentModel->OnBindOrgModel(this);
		SetParentObjMatrix(&m_pParentModel->m_matWorld);
        m_BindType = ENUM_BIND_NONE;
    }
////////////////////////////////////////////////////////////////////////////////////////////
/////Modify by huangjinshou 2007_11_22;
///// 如果是m_nIsSockWave为true的Model应该放到m_listPostRenderEntity里面绘制
  /*  if (g_cEngineManager.GetFocusScene() && m_nIsSockWave)
        g_cEngineManager.GetFocusScene()->AddRenderEntity(this, SCENEENTITY_MODEL, false);*/
	
	if (g_cEngineManager.GetFocusScene() && m_nIsSockWave)
		g_cEngineManager.GetFocusScene()->AddPostRenderEntity(this, SCENEENTITY_MODEL, false);
//////////////////////////////////////////////////////////////////////////////////////////////
	if(m_pParentModel)
		m_pParentModel->OnBind(this,NULL,NULL);
	//m_pParentModel->UpdateChildModel();
	FrameMove();
	SetAlpha(pModel->GetAlpha(), TRUE);
	SetHighLightLevel(pModel->GetHightLightLevel(), TRUE);

	if(m_pParentModel)
	{
		SetPhysicsScene(m_pParentModel->m_lpPhysicsScene);
		SetClientAdd(m_pParentModel->IsClientAdd());
	}

	EnableHideFromRenderTools(FALSE);

	hr = S_OK;
Exit0:
	return hr;
}

HRESULT KG3DModel::BindToBone(IKG3DModel* pModel, const char* strBoneName)
{
	BOOL bIsPassRecord = FALSE;
	HRESULT hr = E_FAIL;

	KG3DModel *pTemp = static_cast<KG3DModel*>(pModel);
	KGLOG_PROCESS_ERROR(pTemp);
	KG_PROCESS_ERROR(strBoneName);
	KG_PROCESS_ERROR(pModel != this);

	if (g_cRecorder.IsRecord() && m_dwRecordID != 0 && pTemp->m_dwRecordID != 0 && !g_cRecorder.IsLock())
	{
		KG3DRecorder::Parameters MyParameters;
		MyParameters.nNum = 2;
		MyParameters.size_tDataSize = sizeof(DWORD) + MAX_PATH * sizeof(char);
		char* pData = new char[MyParameters.size_tDataSize];
		MyParameters.pData = pData;
		MyParameters.nFrom[0] = 0;
		*(DWORD*)(&pData[MyParameters.nFrom[0]]) = pTemp->m_dwRecordID;
		MyParameters.nFrom[1] = sizeof(DWORD);
		strcpy(&pData[MyParameters.nFrom[1]],strBoneName);
		g_cRecorder.RecordEvent(m_dwRecordID,FT_KG3DMODEL_BINDTOBONE,GetCurrentTime(),MyParameters);
		bIsPassRecord = TRUE;
		g_cRecorder.Lock();
	}

	if(pTemp->IsLoaded())
	{
		hr = _BindToBone(pTemp,strBoneName);
	}
	else
	{
		AddRef();
		KG3DModelCommand_BindToBone* pCmd = new KG3DModelCommand_BindToBone(this,strBoneName);
		pTemp->m_Commands.AddCommand(pCmd,FALSE);
		hr = S_OK;
	}

Exit0:
	if (bIsPassRecord)
		g_cRecorder.UnLock();
	return hr;
}

HRESULT KG3DModel::SetModelColor(DWORD dwSubset, D3DCOLORVALUE color)
{
    HRESULT hrResult = E_FAIL;

    KG_ASSERT_EXIT(dwSubset < m_lpMaterial->m_dwNumMaterials);

	KG3DMaterial::KG3DMaterialSubset* pSubMtl = m_lpMaterial->GetSubMaterial(dwSubset);
	KGLOG_PROCESS_ERROR(pSubMtl);
    
	pSubMtl->m_sMaterial9.Diffuse = color;
	SetAlpha(color.a, m_bRenderSpecialAlpha);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KG3DModel::GetModelColor(DWORD dwSubset, D3DCOLORVALUE* color)
{
    HRESULT hrResult = E_FAIL;

    KG_ASSERT_EXIT(dwSubset < m_lpMaterial->m_dwNumMaterials);

	KG3DMaterial::KG3DMaterialSubset* pSubMtl = m_lpMaterial->GetSubMaterial(dwSubset);
	KGLOG_PROCESS_ERROR(pSubMtl);

	*color = pSubMtl->m_sMaterial9.Diffuse;
    color->a = m_fAlpha;

    hrResult = S_OK;
Exit0:
    return hrResult;
}


HRESULT KG3DModel::UnBindFromOther()
{
    if (m_BindType == ENUM_BIND_BONE)
    {
        {
            D3DXVECTOR3 vScaling;
            GetScaling(&vScaling);
            vScaling *= 2.54f;
            SetScaling(&vScaling);
        }
    }

	m_BindType = ENUM_BIND_NONE;
	if(m_pParentModel)
	{
		KG3DModelLod lod;
		lod.pModelHigh = this;

		set<KG3DModelLod>::iterator s = m_pParentModel->m_ChildModels.find(lod);
		if(s!=m_pParentModel->m_ChildModels.end())
		{
			KG3DModelLod& vLod = *s;
			if(vLod.pModelLow)
			{
				vLod.pModelLow->m_pParentModel = NULL;
				vLod.pModelLow->m_nBindIndex = -1;
				vLod.pModelLow->m_BindType = ENUM_BIND_NONE;
			}
			if(vLod.pModelMedium)
			{
				vLod.pModelMedium->m_pParentModel = NULL;
				vLod.pModelMedium->m_nBindIndex = -1;
				vLod.pModelMedium->m_BindType = ENUM_BIND_NONE;
			}
			SAFE_RELEASE(vLod.pModelLow);
			SAFE_RELEASE(vLod.pModelMedium);
			m_pParentModel->m_ChildModels.erase(s);
		}


	    m_pParentModel->m_BindedOrgModel.erase(this);

        if (g_cEngineManager.GetFocusScene() && m_nIsSockWave)
            g_cEngineManager.GetFocusScene()->RemoveRenderEntity(this, SCENEENTITY_MODEL, false);
	}
	
	EnableHideFromRenderTools(TRUE);

	m_pParentModel = NULL;
	m_scBindToObjName.clear();
	m_nBindIndex = -1;

	SetParentObjMatrix(NULL);
    if (m_lpPhysicsScene)
    {
        SetPhysicsScene(NULL);
        m_lpPhysicsScene = NULL;
    }
	return S_OK;
}

BOOL KG3DModel::IsBBoxChanged() 
{
    return m_bBBoxChanged;
}

void KG3DModel::SetBBoxChanged(BOOL bBBoxChanged)
{
    m_bBBoxChanged = bBBoxChanged;
}

HRESULT KG3DModel::ComputeBBox()
{

	//这里如果是绑定的模型，他的BBox依赖于它的父物体
	//不能直接计算， 所以是要在FrameMove里面通过它的父物体来计算
	//这样才能保证父物体的位置已经被更新了

	Helper_UpdateTransformation(TRUE);

	if (m_BindType != ENUM_BIND_NONE)
		return S_OK;

	if(IsLoaded())
	{
		D3DXVECTOR3 BoxPos[8];	
		if (m_BBoxLocal.bInit)
		{
			m_BBoxLocal.TransformCoord(BoxPos, &m_matWorld);
			m_BBox.Clear();
			m_BBox.AddPosition(BoxPos, 8);
		}
		else if (m_lpMesh)
		{
			m_BBoxLocal = m_lpMesh->m_BBox;
			m_BBox.Clear();

			m_BBoxLocal.TransformCoord(BoxPos, &m_matWorld);
			m_BBox.AddPosition(BoxPos,8);//bigBox
			vector<BBox>boneBoxInfo;
			m_lpMesh->GetAllBoneBox(boneBoxInfo);
			m_BoneBoxInfo = boneBoxInfo;
		}
	}
	else
	{
		m_BBox.Clear();
		if(!m_pParentMatrix)
		{
			m_BBox.AddPosition(m_Translation+D3DXVECTOR3(100,100,100));
			m_BBox.AddPosition(m_Translation-D3DXVECTOR3(100,100,100));
		}
	}

	
	return S_OK;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DModel::RenderSubset(DWORD SubsetID,DWORD Option)	//渲染其中一部分
{
	//RenderCommon(0,NULL);
	return S_OK;
}

//HRESULT KG3DModel::FillLightMap(LPDIRECT3DSURFACE9 pSurface)
//{
//	HRESULT hr = S_OK;
//	D3DSURFACE_DESC descSrc,descDest;
//	D3DLOCKED_RECT  rectSrc,rectDest;
//    D3DCOLORVALUE* pcolors = NULL;
//    LPDIRECT3DTEXTURE9 pTexture = NULL;
//
//	KGLOG_PROCESS_ERROR_CONSOLE(pSurface);
//
//	pTexture = m_lpLightMapTexture->GetTexture();
//	KGLOG_PROCESS_ERROR_CONSOLE(pTexture);
//
//	hr = pSurface->GetDesc(&descSrc);
//	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);
//	hr = pSurface->LockRect(&rectSrc,NULL,0);
//	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);
//
//	hr = pTexture->GetLevelDesc(0,&descDest);
//	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);
//	hr = pTexture->LockRect(0,&rectDest,NULL,0);
//	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);
//
//	DWORD* pDest = (DWORD*)rectDest.pBits;
//	DWORD* pSrc = (DWORD*)rectSrc.pBits;
//
//	ASSERT(descDest.Width);
//    ASSERT(descDest.Height);
//
//    int Wrate = descSrc.Width/descDest.Width;
//	int Hrate = descSrc.Height/descDest.Height;
//	//////////////////////////////////////////////////////////////////////////
//	int size = descDest.Width * descDest.Height * sizeof(D3DCOLORVALUE);
//	pcolors = (D3DCOLORVALUE*)malloc(size);
//	ZeroMemory(pcolors,size);
//
//	float K = 1.0F /(Wrate * Hrate);
//	//////////////////////////////////////////////////////////////////////////
//	if(K<=1)
//	{
//		for(int Z=0;Z<(int)descSrc.Height;Z++)
//		{
//			for (int X=0;X<(int)descSrc.Width;X++)
//			{
//				int IndexScr = Z * descSrc.Width + X;
//				int IndexDest = (Z / Hrate)*descDest.Width + (X/Wrate);
//
//				D3DCOLORVALUE value = Color2Value(pSrc[IndexScr]);
//				pcolors[IndexDest].a += value.a * K;
//				pcolors[IndexDest].r += value.r * K;
//				pcolors[IndexDest].g += value.g * K;
//				pcolors[IndexDest].b += value.b * K;
//
//				pDest[IndexDest] = ColorValue(pcolors[IndexDest]);
//			}
//		}
//	}
//	else
//	{
//		for(int Z=0;Z<(int)descDest.Height;Z++)
//		{
//			for (int X=0;X<(int)descDest.Width;X++)
//			{
//				int IndexDest = Z * descDest.Width + X;
//				int IndexScr = (Z * Hrate)*descSrc.Width + (X*Wrate);
//				pDest[IndexDest] = pSrc[IndexScr];
//			}
//		}
//	}
//
//	hr = pSurface->UnlockRect();
//	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);
//	hr = pTexture->UnlockRect(0);
//	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);
//
//	D3DXFilterTexture(pTexture,NULL,0,D3DX_FILTER_BOX);
//
//	free(pcolors);
//	return S_OK;
//Exit0:
//    if (pSurface)
//    {
//        pSurface->UnlockRect();
//    }
//    if (pTexture)
//    {
//        pTexture->UnlockRect(0);
//    }
//    if (pcolors)
//    {
//        free(pcolors);
//    }
//	return E_FAIL;
//}

HRESULT KG3DModel::RenderSelfShadow(LPD3DXEFFECT &lpEffect,const D3DXMATRIX &matVP,
									const D3DXMATRIX &matLVP,LPDIRECT3DTEXTURE9 lpDepthMap)
{
	if(!m_lpMesh)
		return S_FALSE;

	HRESULT hr;

	D3DXMATRIX matprojectwvpmat = m_matWorld * matVP;
	D3DXMATRIX matmodelLVP = m_matWorld * matLVP;

    hr = lpEffect->SetTechnique("effectEntityLight");
    KGLOG_COM_CHECK_ERROR(hr);

    hr = lpEffect->SetMatrix("projectwvpmat",&matprojectwvpmat);
	hr = lpEffect->SetMatrix("lightVP",&matmodelLVP);
	hr = lpEffect->SetTexture("DepthMap",lpDepthMap);

	UINT pass;
	hr = lpEffect->Begin(&pass,0);
	hr = lpEffect->BeginPass(2);

	m_lpMesh->RenderSelfShadow(lpEffect, matVP,matLVP,lpDepthMap);

	hr = lpEffect->EndPass();
	hr = lpEffect->End();	

	return S_OK;
}

HRESULT KG3DModel::RenderShadowMap(LPD3DXEFFECT &lpEffect,const D3DXMATRIX &matVP,const D3DXMATRIX &matWVP)
{
	HRESULT hr = E_FAIL;

	/*if (!IsSecondUVExist())
		return E_INVALIDARG;*/

	if(!m_lpMesh)
        return S_FALSE;

	D3DXMATRIX mat;
	//mat = m_matWorld * matVP;
    hr = lpEffect->SetMatrix("projectwvpmat",&matWVP);
    hr = lpEffect->CommitChanges();
 
    hr = m_lpMesh->RenderShadowMap(lpEffect,m_lpMaterial,matVP,matWVP);
	
	return hr;
}

bool bRenderLogicalBox = false;

HRESULT KG3DModel::RenderReflect(unsigned uOption, void* pExtInfo,float fWaterHeight,D3DXPLANE& WaterPlane,D3DXPLANE& WaterPlaneFX)
{
	BOOL bFX = FALSE;

	if(m_lpMesh)
	{
		if(m_lpMesh->m_dwOption & MESH_SKINED)
			bFX = TRUE;
	}

	if(bFX)
	{
		g_pd3dDevice->SetRenderState(D3DRS_CLIPPLANEENABLE,D3DCLIPPLANE0);
		g_pd3dDevice->SetClipPlane(0,(float *)&WaterPlaneFX);
	}
	else
	{
		g_pd3dDevice->SetRenderState(D3DRS_CLIPPLANEENABLE,D3DCLIPPLANE0);
		g_pd3dDevice->SetClipPlane(0,(float *)&WaterPlane);
	}

	Render(uOption,pExtInfo);

	g_pd3dDevice->SetRenderState(D3DRS_CLIPPLANEENABLE,FALSE);

	return S_OK;
}

HRESULT KG3DModel::RenderReflect(float fWaterHeight,D3DXPLANE& WaterPlane,D3DXPLANE& WaterPlaneFX)
{
	BOOL bFX = FALSE;

	if(m_lpMesh)
	{
		if(m_lpMesh->m_dwOption & MESH_SKINED)
			bFX = TRUE;
	}

	if(bFX)
	{
		g_pd3dDevice->SetRenderState(D3DRS_CLIPPLANEENABLE,D3DCLIPPLANE0);
		g_pd3dDevice->SetClipPlane(0,(float *)&WaterPlaneFX);
	}
	else
	{
		g_pd3dDevice->SetRenderState(D3DRS_CLIPPLANEENABLE,D3DCLIPPLANE0);
		g_pd3dDevice->SetClipPlane(0,(float *)&WaterPlane);
	}

	Render(0,NULL);

	g_pd3dDevice->SetRenderState(D3DRS_CLIPPLANEENABLE,FALSE);

	return S_OK;
}

HRESULT KG3DModel::RenderSpecialAlpha(unsigned int uOption, void* pExtInfo)
{
	HRESULT hr = E_FAIL;
	BOOL bSaveAlphaBlend = g_cEngineOption.bEnableSpecialAlphaBlend;
	g_cEngineOption.bEnableSpecialAlphaBlend = false;
	m_RuntimeMaterial.SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	m_RuntimeMaterial.SetRenderState(D3DRS_COLORWRITEENABLE, 0);
	m_RuntimeMaterial.Enable(TRUE);
	hr = RenderCommon(uOption, pExtInfo);
	m_RuntimeMaterial.SetRenderState(D3DRS_COLORWRITEENABLE, 0xffffffff);
	m_RuntimeMaterial.SetRenderState(D3DRS_ZFUNC, D3DCMP_EQUAL);
	m_RuntimeMaterial.Enable(TRUE);
	hr = RenderCommon(uOption, pExtInfo);
	m_RuntimeMaterial.Clear();
	m_RuntimeMaterial.Enable(FALSE);
	g_cEngineOption.bEnableSpecialAlphaBlend = bSaveAlphaBlend;
	return hr;
}

HRESULT KG3DModel::RenderCommon(unsigned uOption, void *pExtInfo)
{
	HRESULT hr = E_FAIL;
	MeshRenderExtraInfo Info;
	KG3DMesh *pMeshForRender = m_lpMesh;
	if (bRenderLogicalBox)
	{
		RenderLogicCellBox();
	}

	KG_PROCESS_ERROR(m_lpMesh);
	KG_PROCESS_ERROR(m_lpMaterial);
	SetMatrix();

	if (m_fIlluminationLevel > 1.0f)
		uOption |= RENDER_HIGHTLIGHT;

    if (m_Splitter.GetEnable())
        uOption |= RENDER_WITH_WARPER;
	
	
    if (m_bShowBones)
    {
        m_boneMash.Render();
    }
    else
    {
	    //如果有动画
	    if (m_AnimationController.HasAnimation())
	    {
		    if (m_AnimationController.GetUpateAnimationInfo().pMesh)
		    {
			    //顶点/UV动画
			    Info.Data = m_AnimationController.GetUpateAnimationInfo().pMesh;
			    Info.dwType = ENUM_MESHRENDER_EXTR_INFO_MESH;
				pExtInfo = &Info;
		    }
		    else
		    {
			    //骨骼动画
			    if (!pExtInfo)
			    {
					Info.dwType = ENUM_MESHRENDER_EXTR_INFO_MATRIX;			
					Info.Data = m_pBoneMatricesForRender;
				    pExtInfo = &Info;
			    }
		    }
	    }
	    else
	    {
		    //没有动画
		    //如果有pExtInfo, 说明是modelset的一个部件
		    //否则就是单个模型, 使用模型自己的Render矩阵
		    if (!pExtInfo)
		    {
			    Info.dwType = ENUM_MESHRENDER_EXTR_INFO_MATRIX;			
			    if (!m_lpParentModelST)
				{
					Info.Data = m_pBoneMatricesForRender;
				}
				else
				{
					Info.Data = m_lpParentModelST->GetRenderMatrix();
				}
			    pExtInfo = &Info;
		    }
	    }
		_ASSERTE(pMeshForRender);
		pMeshForRender->SetModel(this);
		hr = pMeshForRender->Render(m_lpMaterial, m_fAlpha, uOption, pExtInfo,IsClientAdd());
		pMeshForRender->SetModel(NULL);
    }
	
	RestoreMatrix();
	RenderChildModels(0, NULL);//pExtInfo);
	hr = S_OK;
Exit0:
	return hr;
}

void KG3DModel::UpdateChildRuntimMaterial()
{
	std::set<KG3DModelLod>::iterator it = m_ChildModels.begin();
	std::set<KG3DModelLod>::iterator itend = m_ChildModels.end();
	while (it != itend)
	{
		KG3DModelLod& Lod = *it;

		if (Lod.pModelCurrent)
		{
			*Lod.pModelCurrent->GetRuntimeMaterial() = m_RuntimeMaterial;
			Lod.pModelCurrent->UpdateChildRuntimMaterial();
		}

		++it;
	}
}

DWORD KG3DModel::GetBillBoardCloudPlaneNum()//<BillboardCloudV1>
{
    /*KG3DMesh* pMesh = dynamic_cast<KG3DMesh*>(m_lpMesh);
    if (!pMesh)
        return 0;
    return pMesh->m_dwBillBoardCount;*/
	//return m_dwBillBoardCount;

	if(NULL == m_pBillboardCloud)
		return 0;
	return m_pBillboardCloud->GetPlaneCount();
}

void KG3DModel::IEReleaseBillboardCloud()//<BillboardCloudV1>
{
	/*if(NULL == m_lpMesh)
		return;

	SAFE_RELEASE(pMesh->m_pBillBoardCloundVertexBuffer);
	SAFE_RELEASE(m_pBillboardTexture);

	SAFE_RELEASE(m_pBillboardCloud);

	m_bBillboardCloudCanBeLoaded = bHas;*/

	SAFE_RELEASE(m_pBillboardCloud);
}

HRESULT KG3DModel::RenderBillboardCloud_Begin()
{
	if (m_pBillboardCloud != NULL)
	{
		return m_pBillboardCloud->RenderBegin();
	}
	return E_FAIL;;
}

HRESULT KG3DModel::RenderBillboardCloud(const D3DXMATRIX* pmatOffset)
{
	if (NULL != m_pBillboardCloud)
	{
		m_pBillboardCloud->SetTransform(NULL != pmatOffset ? m_matWorld * (*pmatOffset): m_matWorld);
		return m_pBillboardCloud->Render();
	}
	return E_FAIL;
}

HRESULT KG3DModel::RenderBillboardCloud_End()
{
	if(NULL != m_pBillboardCloud)
	{
		return m_pBillboardCloud->RenderEnd();
	}
	return E_FAIL;
}

HRESULT KG3DModel::Render(unsigned uOption, void* pExtInfo)
{
	HRESULT hr = E_FAIL;

	if(!IsResourceReady(TRUE))
		return S_OK;

    //ApplayLighting();
	PrepareRender(uOption);
	if (m_bRenderSpecialAlpha)
	{
		hr = RenderSpecialAlpha(uOption, pExtInfo);
	}
	else
	{
		hr = RenderCommon(uOption, pExtInfo);
	}

	PostRender();

	return hr;
}

HRESULT KG3DModel::RefurbishTexts()
{
    HRESULT hResult  = E_FAIL;
//    HRESULT hRetCode = E_FAIL;

	KG_PROCESS_ERROR(m_lpMaterial);

	/*m_lpMaterial->ReloadTexture()
    int nCount = 0;
    hRetCode = GetNumMaterial(&nCount);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    for (int i = 0; i < nCount; ++i)
    {
        
		for (int s = 0; s < 5; s++)
        {
            DWORD dwTexId = 0;
            hRetCode = GetMaterialSubSetTextureID(i, s, &dwTexId);
            
            if (FAILED(hRetCode))
                break;

            IEKG3DTexture* pTexture = NULL;
            hRetCode = g_cTextureTable.GetIEKG3DTexture(&pTexture, dwTexId);

            if (FAILED(hRetCode))
                continue;

            pTexture->ForceReload();
        }

    }*/

    hResult = S_OK;
Exit0 :
    return hResult;
}

HRESULT KG3DModel::RenderWithoutStateChange()
{
	DWORD dwRenderOption = 0;
	//FrameMove();

	SetMatrix();

	if (!m_lpClip)
		dwRenderOption |= RENDER_NO_UPDATE;
	if (m_lpMesh)
	{
		m_lpMesh->Render(NULL, m_fAlpha, dwRenderOption,NULL,IsClientAdd());
	}

	RestoreMatrix();
	//// chain render
	//if (m_bChainEnable) 
	//{
	//	for (ChainIter_t iter = m_vecChainModel.begin();
	//		iter != m_vecChainModel.end();
	//		++ iter)
	//	{
	//		(*iter)->RenderWithoutStateChange();
	//	}

	//}
	return S_OK;
}

HRESULT KG3DModel::Helper_UpdateTransformation(BOOL bUpdateBBox)
{
	HRESULT hResult = E_FAIL;
	HRESULT hRetCode = E_FAIL;
	D3DXMATRIX matParent;
	KG_PROCESS_SUCCESS(m_bInMultiThreadLoadList);

	switch(m_BindType)
	{
	case ENUM_BIND_NONE:
		{
			if (m_pParentMatrix)
				m_matWorld = m_Matrix * (*m_pParentMatrix);
			else
				m_matWorld = m_Matrix;
            ASSERT_FLOAT_IS_VALIED(m_matWorld._11);

			break;
		}
	case ENUM_BIND_BONE:
		{
			KG_PROCESS_ERROR(m_pParentModel);
			KG_PROCESS_ERROR(m_pParentModel->IsLoaded());

			hResult = m_pParentModel->GetBoneMatrix(m_nBindIndex, &matParent, NULL);
			KG_COM_PROCESS_ERROR(hResult);
			
			m_matWorld = m_Matrix * matParent;
            ASSERT_FLOAT_IS_VALIED(m_matWorld._11);

			break; 
		}
	case ENUM_BIND_SOCKET:
		{
			KG_PROCESS_ERROR(m_pParentModel);
			KG_PROCESS_ERROR(m_pParentModel->IsLoaded());

			m_pParentModel->UpdateSocketsEx(m_nBindIndex);
			hResult = m_pParentModel->GetSocketMatrix(m_nBindIndex, matParent);
			KG_COM_PROCESS_ERROR(hResult);
			
			m_matWorld = m_Matrix * matParent;
            ASSERT_FLOAT_IS_VALIED(m_matWorld._11);

			break;
		}
	}

	if(bUpdateBBox)
	{
		D3DXVECTOR3 vec3Pos[8];
		m_BBoxLocal.TransformCoord(vec3Pos, &m_matWorld);
		m_BBox.Clear();
		m_BBox.AddPosition(vec3Pos, 8);
	}

Exit1:
	hRetCode = S_OK;
Exit0:
	return hRetCode;
}

BOOL KG3DModel::IsNeedFrameMove()
{
	if(!m_bSleep || IsAnimationed() || !m_bMeshPostLoadProcessed || m_Commands.HasCommand() )
		return TRUE;

	return FALSE;
}

HRESULT KG3DModel::UpdateSocketsEx(int nIndex)
{
	if(IsLoaded())
	{
		if(m_lpParentModelST && m_lpParentModelST->IsLoaded())
		{
			KG3DBip* pBip =m_lpParentModelST->GetBip();
			if(pBip)
			{
				D3DXMATRIX* pObjMatrices = pBip->GetObjMatricesArray();
				if(pObjMatrices)
					UpdateSockets(pObjMatrices,pBip->GetNumBones(),nIndex);
			}
		}
		else
		{
			int nBones = 0;
			GetNumBones(&nBones);

			if(m_pBoneMatricesForRender)
				UpdateSockets(m_pBoneMatricesForRender,nBones,nIndex);
		}
	}

	return S_OK;
}

HRESULT KG3DModel::FrameMove(DWORD dwOption, void *pExtInfo)
{
	HRESULT hResult = E_FAIL;
    HRESULT hRetCode = E_FAIL;
	DWORD dwStartTime = timeGetTime();
	//std::set<KG3DModel*> Updated;
	std::set<KG3DModel*>::iterator i;
	
	DWORD dwThreadID = ::GetCurrentThreadId();
	KG_PROCESS_ERROR(dwThreadID==g_dwMainThreadID);

	m_bSelfDestructFramemoveFlag = FALSE;
	BOOL bUpdateBBox = (m_dwFrameCount<100 || g_dwRenderCount % 10 == 5);
	hRetCode = Helper_UpdateTransformation(bUpdateBBox);
	KG_COM_PROCESS_ERROR(hRetCode);

    KG_PROCESS_ERROR(m_lpMesh);

    hRetCode = m_lpMesh->CheckLoadingState();
    KG_COM_PROCESS_ERROR(hRetCode);

	if(!m_bMeshPostLoadProcessed && m_lpMesh->m_eDynamicLoadState == KG3DMesh::STATE_VIDEOMEM)
	{
		MeshPostLoadProcess();
	}

	KG_PROCESS_SUCCESS(!IsLoaded());

	m_AnimationController.FrameMove();
	ProcessCommands();

	KG_PROCESS_SUCCESS(dwOption & RENDER_BIP);
	KG_PROCESS_SUCCESS(!m_AnimationController.HasAnimation());

	{
		m_AnimationController.UpdateAnimation(NULL, TRUE, TRUE);
		if (m_AnimationController.GetUpateAnimationInfo().bNeedUpdateToWorld)
		{
			//Need to update matrices to world matrices
			KG_PROCESS_ERROR(m_AnimationController.GetUpateAnimationInfo().pMatrix);
			//Here we can be sure mesh is a KG3DMesh
			m_lpMesh->UpdateToObj(m_pBoneMatricesForRender, m_pBoneLocalMatrixBuffer);
		}
	}

	int nBones = 0;
	GetNumBones(&nBones);
	UpdateSockets(m_pBoneMatricesForRender,nBones);

Exit1:
	if (m_bMeshPostLoadProcessed && m_piFlexibleBody)
	{
		hRetCode = m_piFlexibleBody->Update();
		//KGLOG_COM_PROCESS_ERROR(hRetCode);
	}

	UpdateChildModel();	
	Helper_CaptionFrameMove();

	m_dwFrameCount++;
	
	DWORD dwCost = timeGetTime() - dwStartTime;
	if (g_cEngineOption.bEnableTimeOptimizeLog && dwCost > 10)
	{
		KGLogPrintf(KGLOG_ERR, "TimeOptimize %d KG3DModel::FrameMove  %s.",dwCost , m_scName.c_str());
	}

    hResult = S_OK;
Exit0:
	return hResult;
}

HRESULT KG3DModel::FrameMove()
{
	return FrameMove(0, NULL);
}
 
HRESULT KG3DModel::SetAlpha(float fAlpha, BOOL bUseSpecail)
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

	m_fAlpha = fAlpha;
	if (m_fAlpha < 1.0f)
	{
		m_bRenderSpecialAlpha = bUseSpecail;
		m_RuntimeMaterial.SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	}
	else
	{
		m_bRenderSpecialAlpha = FALSE;
		m_RuntimeMaterial.Clear();
		m_RuntimeMaterial.SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	}
	m_RuntimeMaterial.SetAlpha(m_fAlpha);

	std::set<KG3DModelLod>::iterator i = m_ChildModels.begin();
	std::set<KG3DModelLod>::iterator iend = m_ChildModels.end();
	while (i != iend)
	{
		KG3DModelLod& Lod = *i;

		if(Lod.pModelCurrent)
			Lod.pModelCurrent->SetAlpha(fAlpha, bUseSpecail);

		++i;
	}

	if (bIsPassRecord)
		g_cRecorder.UnLock();
	return S_OK;
}

float KG3DModel::GetAlpha()
{
	return m_fAlpha;
};


HRESULT KG3DModel::OnReferenceZero()
{
	return g_cModelTable.ReleaseResource(this, RELEASE_ATONCE);
}

HRESULT KG3DModel::RenderSelectMark(DWORD dwColor)
{
    g_cGraphicsTool.DrawAABBox(&m_BBox, dwColor);
	return S_OK;
}

void KG3DModel::SetTranslation(D3DXVECTOR3 *pValue)
{
	
	BOOL bIsPassRecord = FALSE;
	if (g_cRecorder.IsRecord() && m_dwRecordID != 0 && !g_cRecorder.IsLock())
	{
		KG3DRecorder::Parameters MyParameters;
		MyParameters.nNum = 1;
		MyParameters.size_tDataSize = sizeof(D3DXVECTOR3);
		char* pData = new char[MyParameters.size_tDataSize];
		MyParameters.pData = pData;
		MyParameters.nFrom[0] = 0;
		*(D3DXVECTOR3*)(&pData[MyParameters.nFrom[0]]) = *pValue;
		g_cRecorder.RecordEvent(m_dwRecordID,FT_KG3DMODEL_SETTRANSLATION,
			GetCurrentTime(),MyParameters);
		bIsPassRecord = TRUE;
		g_cRecorder.Lock();
	}
	KG3DTransformation::SetTranslation(pValue);

	// chain transformation
	/*if (m_bChainEnable) 
	{
		for (ChainIter_t iter = m_vecChainModel.begin();
			iter != m_vecChainModel.end();
			++ iter)
		{
			(*iter)->SetTranslation(pValue);
		}

	}*/
	//ComputeBBox();
	if (bIsPassRecord)
		g_cRecorder.UnLock();
}

void KG3DModel::SetRotation(D3DXQUATERNION *pValue)
{
	BOOL bIsPassRecord = FALSE;
	if (g_cRecorder.IsRecord() && m_dwRecordID != 0 && !g_cRecorder.IsLock())
	{
		KG3DRecorder::Parameters MyParameters;
		MyParameters.nNum = 1;
		MyParameters.size_tDataSize = sizeof(D3DXQUATERNION);
		char* pData = new char[MyParameters.size_tDataSize];
		MyParameters.pData = pData;
		MyParameters.nFrom[0] = 0;
		*(D3DXQUATERNION*)(&pData[MyParameters.nFrom[0]]) = *pValue;
		g_cRecorder.RecordEvent(m_dwRecordID,FT_KG3DMODEL_SETROTATION,
			GetCurrentTime(),MyParameters);
		bIsPassRecord = TRUE;
		g_cRecorder.Lock();
	}
	KG3DTransformation::SetRotation(pValue);

	// chain transformation
	/*if (m_bChainEnable) 
	{
		for (ChainIter_t iter = m_vecChainModel.begin();
			iter != m_vecChainModel.end();
			++ iter)
		{
			(*iter)->SetRotation(pValue);
		}
	}*/
	//ComputeBBox();
	if (bIsPassRecord)
		g_cRecorder.UnLock();
}

void KG3DModel::SetRotationCenter(D3DXVECTOR3* pValue)
{
	BOOL bIsPassRecord = FALSE;
	if (g_cRecorder.IsRecord() &&m_dwRecordID != 0 && !g_cRecorder.IsLock())
	{
		KG3DRecorder::Parameters MyParameters;
		MyParameters.nNum = 1;
		MyParameters.size_tDataSize = sizeof(D3DXVECTOR3);
		char* pData = new char[MyParameters.size_tDataSize];
		MyParameters.pData = pData;
		MyParameters.nFrom[0] = 0;
		*(D3DXVECTOR3*)(&pData[MyParameters.nFrom[0]]) = *pValue;
		g_cRecorder.RecordEvent(m_dwRecordID,FT_KG3DMODEL_SETROTATIONCENTER,
			GetCurrentTime(),MyParameters);
		bIsPassRecord = TRUE;
		g_cRecorder.Lock();
	}
	KG3DTransformation::SetRotationCenter(pValue);



	// chain transformation
	/*if (m_bChainEnable) 
	{
		for (ChainIter_t iter = m_vecChainModel.begin();
			iter != m_vecChainModel.end();
			++ iter)
		{
			(*iter)->SetRotationCenter(pValue);
		}

	}*/
	//ComputeBBox();
	if (bIsPassRecord)
		g_cRecorder.UnLock();
}
void KG3DModel::SetScaling(D3DXVECTOR3 *pValue)
{
	BOOL bIsPassRecord = FALSE;
	if (g_cRecorder.IsRecord() && m_dwRecordID != 0 && !g_cRecorder.IsLock())
	{
		KG3DRecorder::Parameters MyParameters;
		MyParameters.nNum = 1;
		MyParameters.size_tDataSize = sizeof(D3DXVECTOR3);
		char* pData = new char[MyParameters.size_tDataSize];
		MyParameters.pData = pData;
		MyParameters.nFrom[0] = 0;
		*(D3DXVECTOR3*)(&pData[MyParameters.nFrom[0]]) = *pValue;
		g_cRecorder.RecordEvent(m_dwRecordID,FT_KG3DMODEL_SETSCALING,
			GetCurrentTime(),MyParameters);
		bIsPassRecord = TRUE;
		g_cRecorder.Lock();
	}
	KG3DTransformation::SetScaling(pValue);
	
	
	// chain transformation
	/*if (m_bChainEnable) 
	{
		for (ChainIter_t iter = m_vecChainModel.begin();
			iter != m_vecChainModel.end();
			++ iter)
		{
			(*iter)->SetScaling(pValue);
		}

	}*/
//	ComputeBBox();
	if (bIsPassRecord)
		g_cRecorder.UnLock();    
}
void KG3DModel::SetScalingRotation(D3DXQUATERNION* pValue)
{
	BOOL bIsPassRecord = FALSE;
	if (g_cRecorder.IsRecord() && m_dwRecordID != 0 && !g_cRecorder.IsLock())
	{
		KG3DRecorder::Parameters MyParameters;
		MyParameters.nNum = 1;
		MyParameters.size_tDataSize = sizeof(D3DXQUATERNION);
		char* pData = new char[MyParameters.size_tDataSize];
		MyParameters.pData = pData;
		MyParameters.nFrom[0] = 0;
		*(D3DXQUATERNION*)(&pData[MyParameters.nFrom[0]]) = *pValue;
		g_cRecorder.RecordEvent(m_dwRecordID,FT_KG3DMODEL_SETSCALINGROTATION,
			GetCurrentTime(),MyParameters);
		bIsPassRecord = TRUE;
		g_cRecorder.Lock();
	}
	KG3DTransformation::SetScalingRotation(pValue);



	// chain transformation
	/*if (m_bChainEnable) 
	{
		for (ChainIter_t iter = m_vecChainModel.begin();
			iter != m_vecChainModel.end();
			++ iter)
		{
			(*iter)->SetScalingRotation(pValue);
		}

	}*/

//	ComputeBBox();
	if (bIsPassRecord)
		g_cRecorder.UnLock();  
}
void KG3DModel::SetScalingCenter(D3DXVECTOR3* pValue)
{
	BOOL bIsPassRecord = FALSE;
	if (g_cRecorder.IsRecord() && m_dwRecordID != 0 && !g_cRecorder.IsLock())
	{
		KG3DRecorder::Parameters MyParameters;
		MyParameters.nNum = 1;
		MyParameters.size_tDataSize = sizeof(D3DXVECTOR3);
		char* pData = new char[MyParameters.size_tDataSize];
		MyParameters.pData = pData;
		MyParameters.nFrom[0] = 0;
		*(D3DXVECTOR3*)(&pData[MyParameters.nFrom[0]]) = *pValue;
		g_cRecorder.RecordEvent(m_dwRecordID,FT_KG3DMODEL_SETSCALINGCENTER,
			GetCurrentTime(),MyParameters);
		bIsPassRecord = TRUE;
		g_cRecorder.Lock();
	}
	KG3DTransformation::SetScalingCenter(pValue);



	// chain transformation
	/*if (m_bChainEnable) 
	{
		for (ChainIter_t iter = m_vecChainModel.begin();
			iter != m_vecChainModel.end();
			++ iter)
		{
			(*iter)->SetScalingCenter(pValue);
		}

	}*/

	//ComputeBBox();
	if (bIsPassRecord)
		g_cRecorder.UnLock();  
}

void KG3DModel::GetTranslation(D3DXVECTOR3* pValue)
{
	KG3DTransformation::GetTranslation(pValue);
   
}
void KG3DModel::GetRotation(D3DXQUATERNION* pValue)
{
	KG3DTransformation::GetRotation(pValue);
}
void KG3DModel::GetRotationCenter(D3DXVECTOR3* pValue)
{
	KG3DTransformation::GetRotationCenter(pValue);
}
void KG3DModel::GetScaling(D3DXVECTOR3* pValue)
{
	KG3DTransformation::GetScaling(pValue);
}
void KG3DModel::GetScalingRotation(D3DXQUATERNION* pValue)
{
	KG3DTransformation::GetScalingRotation(pValue);
}

void KG3DModel::GetScalingCenter(D3DXVECTOR3* pValue)
{
	KG3DTransformation::GetScalingCenter(pValue);
}

HRESULT KG3DModel::RenderToScreen(const D3DXVECTOR2* pA, 
								  const D3DXVECTOR2* pC, 
								  const D3DXVECTOR2* pUVA, 
								  const D3DXVECTOR2* pUVC, 
								  float fZ, 
								  DWORD dwFlags,
								  DWORD dwBackColor,
								  void* pExtInfo)
{
	HRESULT hr = E_FAIL;
	KG3DTexture* pTexture = NULL;
	DWORD dwSampleStateMinFilterPrev = 0;
	DWORD dwSampleStateMagFilterPrev = 0;
	DWORD dwSampleStateMaxAnisotoryPrev = 0;
	LPDIRECT3DBASETEXTURE9 pTexturePrev = NULL;
	
	g_pd3dDevice->GetTexture(0, &pTexturePrev);
    if (pTexturePrev)
	{
		pTexturePrev->Release();
	}
	g_pd3dDevice->GetSamplerState(0, D3DSAMP_MAGFILTER, &dwSampleStateMagFilterPrev);
	g_pd3dDevice->GetSamplerState(0, D3DSAMP_MINFILTER, &dwSampleStateMinFilterPrev);
	g_pd3dDevice->GetSamplerState(0, D3DSAMP_MAXANISOTROPY,&dwSampleStateMaxAnisotoryPrev);

	KG3DCameraPreset TestCamera;

	float fWidth = abs(pC->x - pA->x);
	float fHeight = abs(pC->y - pA->y);

	D3DXMATRIX matViewSave;
	D3DXMATRIX matProjSave;
	g_pd3dDevice->GetTransform(D3DTS_VIEW, &matViewSave);
	g_pd3dDevice->GetTransform(D3DTS_PROJECTION, &matProjSave);

	TestCamera.GetPresetCamera(this, 
		KG3DCameraPreset::CPT_FRONT, 
		fWidth / fHeight);
	TestCamera.SetCamera(KG3DCameraPreset::CPT_FRONT);

	pTexture = g_cTextureTable.GetRenderTargetTexture((UINT)fWidth,
		(UINT)fHeight,
		D3DFMT_A8R8G8B8);
	KG_PROCESS_ERROR(pTexture);
    
	hr = RenderToTexture(static_cast<DWORD>(KG3DTextureTable::STID_RENDERTARGET), 
		pA, 
		pC, 
		pUVA,
		pUVC,
		0.0f, 
		dwFlags,
		dwBackColor,
		0,
		pExtInfo);
	
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, (D3DTEXTUREFILTERTYPE)g_cEngineOption.nSampMinFilter);
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, (D3DTEXTUREFILTERTYPE)g_cEngineOption.nSampMagFilter);
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_MAXANISOTROPY ,g_cEngineOption.dwMaxAnisotropy);

	g_cGraphicsTool.DrawScreenRect(pA, 
		pC, 
		fZ, 
		1.0f, 
		1.0f, 
		0xffffffff, 
		static_cast<DWORD>(KG3DTextureTable::STID_RENDERTARGET)); 
Exit0:
	g_pd3dDevice->SetTransform(D3DTS_VIEW, &matViewSave);
	g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &matProjSave);
	g_pd3dDevice->SetTexture(0, pTexturePrev);
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, dwSampleStateMinFilterPrev);
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, dwSampleStateMagFilterPrev);
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_MAXANISOTROPY,dwSampleStateMaxAnisotoryPrev);

	return hr;
}

HRESULT KG3DModel::RenderPlaneShadow(D3DXPLANE& plane,const D3DXVECTOR4& LightDir)
{
	return S_OK;
}


HRESULT KG3DModel::GetMeshFileName(LPTSTR strFileName)
{
	if (m_lpMesh)
		wsprintf(strFileName,"%s",m_lpMesh->m_scName.c_str());
	else
		wsprintf(strFileName,"");
	return S_OK;
}

HRESULT KG3DModel::GetMaterialFileName(LPTSTR strFileName)
{
	if (m_lpMaterial)
		wsprintf(strFileName,"%s",m_lpMaterial->m_scName.c_str());
	else
		wsprintf(strFileName,"");
	return S_OK;
}

HRESULT KG3DModel::GetAnimationFileName(LPTSTR strFileName, size_t tBufferSize)
{
	HRESULT hResult = E_FAIL;
	KG3DClip *pClip = NULL; 

	KG_PROCESS_ERROR(strFileName);
	memset(strFileName, 0, tBufferSize);

	hResult = m_Commands.GetAnimationFileName(strFileName,tBufferSize);
	KG_COM_PROCESS_SUCCESS(hResult);

	KG3DAnimationController *pController = static_cast<KG3DAnimationController*>(GetAnimationController(ANICTL_PRIMARY));
	KG_PROCESS_ERROR(pController);

	pClip = static_cast<KG3DClip*>(pController->GetCurAnimation());
	KG_PROCESS_ERROR(pClip);

	strcpy_s(strFileName, tBufferSize, pClip->m_scName.c_str());
Exit1:
	hResult = S_OK;
Exit0:
	return hResult;
}

void KG3DModel::_SetPhysicsScene(KG3DPhysiscScene *pScene)
{
    HRESULT hrRetCode = E_FAIL;

	std::set<KG3DModelLod>::iterator it;
	for (it = m_ChildModels.begin(); it != m_ChildModels.end(); ++it)
	{
		KG3DModelLod& ChildModel = *it;
		if(ChildModel.pModelCurrent)
			ChildModel.pModelCurrent->SetPhysicsScene(pScene);
	}

    // if already in physics scene, remove from it first
    if (m_lpPhysicsScene && m_piFlexibleBody)
    {
        hrRetCode = m_piFlexibleBody->RemoveFromPhysicsScene();
        KGLOG_COM_PROCESS_ERROR(hrRetCode);
        m_lpPhysicsScene = NULL;
    }

	if (pScene && m_piFlexibleBody)
	{
        hrRetCode = m_piFlexibleBody->AddToPhysicsScene(pScene);
        KGLOG_COM_PROCESS_ERROR(hrRetCode);
	}

    m_lpPhysicsScene = pScene;	
Exit0:
	return ;
}

void KG3DModel::SetPhysicsScene(KG3DPhysiscScene *pScene)
{
	if (m_lpPhysicsScene == pScene)
		return;	

	if(IsLoaded())
	{
		_SetPhysicsScene(pScene);
	}
	else
	{
		KG3DModelCommand_SetPhysicsScene* pCmd = new KG3DModelCommand_SetPhysicsScene(pScene);
		m_Commands.AddCommand(pCmd,FALSE);
	}

}

HRESULT KG3DModel::SetMatrix()
{
	HRESULT hr = S_OK;
	hr = g_pd3dDevice->GetTransform(D3DTS_WORLD, &m_matWorldSave);
	hr = g_pd3dDevice->SetTransform(D3DTS_WORLD, &m_matWorld);
	return S_OK;
}

HRESULT KG3DModel::RestoreMatrix()
{
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &m_matWorldSave);
	return S_OK;
}


BOOL KG3DModel::GetCameraNearRayIntersect(IN CONST D3DXVECTOR3 vSrc[],
                                    IN CONST D3DXVECTOR3 vDst[], 
                                    IN CONST INT         nNum,
                                    OUT      FLOAT*      pDis 
                                    )
{
    KG3DMesh* pMesh = dynamic_cast<KG3DMesh*>(m_lpMesh);
	INT nAttributeBuffer[255];
	INT nNumAttribuate = 0;
    if (!pMesh)
        return FALSE;

    D3DXMATRIX matWorldInv;

    D3DXMatrixInverse(&matWorldInv,NULL,&m_matWorld);

    D3DXVECTOR3 src[8];
    D3DXVECTOR3 dst[8];

    for (int i = 0; i < nNum; i++)
    {
        D3DXVec3TransformCoord(&src[i], &vSrc[i], &matWorldInv);
        D3DXVec3TransformCoord(&dst[i], &vDst[i], &matWorldInv);
    }

    D3DXVECTOR3 vNear;
    INT         Index;

	KGLOG_PROCESS_ERROR(m_lpMaterial && m_lpMaterial->IsLoaded());

	for (int i = 0; i < (int)m_lpMaterial->m_dwNumMaterials; i++)
	{
		KG3DMaterial::KG3DMaterialSubset* pSubMtl = m_lpMaterial->GetSubMaterial(i);
		if(pSubMtl)
		{
			if ((pSubMtl->m_dwOption  & MATERIAL_OPTION_NOLOGICALCELL))
			{
				nAttributeBuffer[nNumAttribuate] = i;
				nNumAttribuate++;
			}
		}
	}
	
    if (pMesh->GetCameraNearRayIntersect(src, dst, nNum, nAttributeBuffer, nNumAttribuate, &vNear, &Index))
    {
        D3DXVec3TransformCoord(&vNear, &vNear, &m_matWorld);
        *pDis = D3DXVec3Length(&(vNear - vSrc[Index]));

        return true;
    }
Exit0:
    return false;
}

BOOL KG3DModel::GetRayIntersect(DWORD& dwCount,D3DXVECTOR3* pInterPoint,DWORD dwMaxNum,D3DXVECTOR3 Origin,D3DXVECTOR3 Direction)
{
	if (!m_lpMesh)
		return FALSE;

	D3DXMATRIX matWorldInv;

	/*SetMatrix();

	RestoreMatrix();*/

	D3DXVECTOR3 A = Origin;
	//D3DXVECTOR3 B = A + Direction;

	D3DXMatrixInverse(&matWorldInv,NULL,&m_matWorld);
	D3DXVec3TransformCoord(&A,&A,&matWorldInv);
	//D3DXVec3TransformCoord(&B,&B,&matWorldInv);

	D3DXVECTOR3 NewDir;// = B - A;
	//D3DXVec3Normalize(&NewDir,&NewDir);
    D3DXVec3TransformNormal(&NewDir, &Direction, &matWorldInv);

	//DWORD dwCountStart = dwCount;

	D3DXVECTOR3 NewInterLow,NewInterHigh;
	if (m_lpMesh->GetRayIntersect(dwCount,pInterPoint,dwMaxNum,A,NewDir))
	{
		for(DWORD i=0;i<dwCount;i++)
		{
			D3DXVec3TransformCoord(&pInterPoint[i],&pInterPoint[i],&m_matWorld);
		}
		return TRUE;
	}
	return FALSE;
}

BOOL KG3DModel::RayIntersection(const D3DXVECTOR3& vSrc , const D3DXVECTOR3& vNormalizedDir
								, FLOAT* pfRet,D3DXVECTOR3* pFaceNormal
								, BOOL bIntersectBoxOnly
								, KG3DIntersectionFilter* pFilter /*= NULL*/)
{
	if (!m_lpMesh)
		return FALSE;

	if (NULL != pFilter)
	{
		HRESULT	hr = pFilter->Accept(*this);
		if (FAILED(hr))
			return FALSE;
	}

	FLOAT fRet = FLT_MAX;
 	BOOL bRet = FALSE;
	if (bIntersectBoxOnly)
	{
		bRet = m_BBox.RayIntersection(vSrc, vNormalizedDir, &fRet, NULL);
		if (bRet && NULL != pfRet)
		{
			*pfRet = fRet;
		}
		return bRet;
	}
	
	//如果和盒子不碰撞，就不再检查内部物件
	bRet = m_BBox.RayIntersection(vSrc, vNormalizedDir, &fRet, NULL);
	if(! bRet)
		return FALSE;

	D3DXMATRIX matWorldInv;

	D3DXVECTOR3 vNewSrc;
	D3DXMatrixInverse(&matWorldInv,NULL,&m_matWorld);
	D3DXVec3TransformCoord(&vNewSrc,&vSrc,&matWorldInv);

	D3DXVECTOR3 vNewDir;
	D3DXVec3TransformNormal(&vNewDir, &vNormalizedDir, &matWorldInv);
	D3DXVec3Normalize(&vNewDir, &vNewDir);
	D3DXVECTOR3 vFaceNormal;

	bRet = m_lpMesh->RayIntersection(vNewSrc, vNewDir, &fRet,&vFaceNormal,pFilter);
	if (bRet && fRet > 0)
	{
		_ASSERTE(fRet < FLT_MAX);
		if (NULL != pfRet)
		{
			D3DXVECTOR3 vHitPos = vNewSrc + fRet * vNewDir;
			D3DXVec3TransformCoord(&vHitPos, &vHitPos, &m_matWorld);
			*pfRet = D3DXVec3Length(&(vHitPos - vSrc));
			if(pFaceNormal)
			{
				D3DXVec3TransformNormal(pFaceNormal,&vFaceNormal,&m_matWorld);
			}
		}	
	}
	return bRet;
}

BOOL KG3DModel::GetRayIntersectPointAndDir(DWORD& dwCount, 
											   D3DXVECTOR3* pIntersectPoint,
											   float* pAngle,
											   const D3DXVECTOR3& vec3Org,
											   const D3DXVECTOR3& vec3Dir)
{
	BOOL bReturn = FALSE;
	D3DXMATRIX matWorldInv;
	D3DXVECTOR3 vec3A = vec3Org;
	D3DXVECTOR3 vec3B = vec3A + vec3Dir;
	D3DXVECTOR3 vec3NewDir;
	
	KG_PROCESS_ERROR(m_lpMesh);
	KG_PROCESS_ERROR(m_lpMesh->GetType() == MESHTYPE_DEFAULT);

	/*SetMatrix();

	RestoreMatrix();*/

	D3DXMatrixInverse(&matWorldInv,NULL,&m_matWorld);
    D3DXVec3TransformCoord(&vec3A, &vec3A, &matWorldInv);
    D3DXVec3TransformCoord(&vec3B, &vec3B,&matWorldInv);
    vec3NewDir = vec3B - vec3A;
    D3DXVec3Normalize(&vec3NewDir, &vec3NewDir);

    DWORD dwCountStart = dwCount;
    bReturn = m_lpMesh->GetRayIntersectPointAndDir(vec3A, 
        vec3NewDir, 
        pIntersectPoint,
        pAngle, 
        dwCount, 
        100);

    KG_PROCESS_ERROR(bReturn);

    for(DWORD i = dwCountStart; i < dwCount; i++)
    {
        D3DXVec3TransformCoord(&pIntersectPoint[i], &pIntersectPoint[i], &m_matWorld);
    }

Exit0:
    return bReturn;
}

BOOL KG3DModel::IsVertexInside(D3DXVECTOR3& vVertex, D3DXVECTOR3& vVexDir)
{
    if (!m_lpMesh)
        return FALSE;

    D3DXMATRIX matWorldInv;
    D3DXVECTOR3 vPos = vVertex;
    D3DXVECTOR3 vDir = vVertex + vVexDir * 100.f;

    D3DXMatrixInverse(&matWorldInv, NULL, &m_matWorld);
    D3DXVec3TransformCoord(&vPos,&vPos,&matWorldInv);
    D3DXVec3TransformCoord(&vDir,&vDir,&matWorldInv);

    D3DXVec3Normalize(&vDir, &(vDir - vPos));

    return m_lpMesh->IsVertexInside(vPos, vDir, this);
}


BOOL KG3DModel::IsRayIntersect(D3DXVECTOR3& Inter,D3DXVECTOR3 Origin,D3DXVECTOR3 Direction)
{
    if (!m_lpMesh)
        return FALSE;
    D3DXMATRIX matWorldInv;
    float fHitDis = 0.0f;

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

    D3DXVECTOR3 A = Origin;
    D3DXVECTOR3 B = A + Direction;

    D3DXMatrixInverse(&matWorldInv,NULL,&m_matWorld);
    D3DXVec3TransformCoord(&A,&A,&matWorldInv);
    D3DXVec3TransformCoord(&B,&B,&matWorldInv);

    D3DXVECTOR3 NewDir = B - A;
    D3DXVec3Normalize(&NewDir,&NewDir);

    D3DXVECTOR3 NewInter;
    KG_PROCESS_SUCCESS(!(m_lpMesh->IsSkin() && m_Static_bUseBoneBox));
    KG_PROCESS_SUCCESS(!m_pBoneMatricesForRender);
    //ComputeBBox();
    if (m_BBox.IsRayIntersect3D(Inter,Origin,Direction,NULL,&fHitDis))
    {
        KG_PROCESS_SUCCESS(fHitDis < RAYINTERSECT_USEBONEBOX_DISTANCE);
		if(IsRayIntersectBoneBox(NewInter,A,NewDir, m_pBoneMatricesForRender))
        {
            D3DXVec3TransformCoord(&Inter,&NewInter,&m_matWorld);
            return TRUE;
        }
    }

    return FALSE;
Exit1:
    if (m_lpMesh->IsRayIntersect(NewInter,A,NewDir, m_pBoneMatricesForRender, &m_BBoxLocal))
    {
        D3DXVec3TransformCoord(&Inter,&NewInter,&m_matWorld);
        return TRUE;
    }
    return FALSE;

}

BOOL KG3DModel::IsRayIntersectBoneBox(D3DXVECTOR3& Inter,D3DXVECTOR3 Origin,D3DXVECTOR3 Direction,D3DXMATRIX* matWorld)
{
	BOOL bHit = FALSE;
	KG_PROCESS_ERROR(m_lpMesh);
	KGLOG_PROCESS_ERROR(IsLoaded());

	if (m_lpParentModelST && m_Mesh2Bip.size())
	{
		D3DXMATRIX *pMatices = m_lpParentModelST->GetRenderMatrix();
		for (size_t i=0; i< m_BoneBoxInfo.size(); i++)
		{		
			D3DXMATRIX matTemp;
			DWORD dwBoneIndex = m_Mesh2Bip[i];
			if (dwBoneIndex != -1)
			{
				matTemp = m_lpMesh->m_pmatReverseMatrix[i] * pMatices[dwBoneIndex];
			}
			else
			{
				matTemp = m_lpMesh->m_pmatReverseMatrix[i] * matWorld[i];
			}
			if(m_BoneBoxInfo[i].IsRayIntersect3D(Inter,Origin,Direction, &matTemp))
			{
				bHit = TRUE;
				break;
			}
		}
	}
	else
	{
		for (size_t i=0; i< m_BoneBoxInfo.size(); i++)
		{		
			D3DXMATRIX matTemp;
			matTemp = m_lpMesh->m_pmatReverseMatrix[i] * matWorld[i];
			if(m_BoneBoxInfo[i].IsRayIntersect3D(Inter,Origin,Direction, &matTemp))
			{
				bHit = TRUE;
				break;
			}
		}
	}
Exit0:
	return bHit;
}


HRESULT KG3DModel::RegistereEventHandler(IKG3DAnimationEventHandler* pHandler)
{
	return g_cClipTable.GetEventCenter()->RegistereEventHandler(this,pHandler);
//	HRESULT hrResult = E_FAIL;
//
//	KGLOG_PROCESS_ERROR(pHandler);
//
//    m_pEventHandler = pHandler;
//
//	hrResult = S_OK;
//Exit0:
//	return hrResult;
}

HRESULT KG3DModel::UnregisterEventHandler(IKG3DAnimationEventHandler* pHandler)
{
	return g_cClipTable.GetEventCenter()->UnregisterEventHandler(this,pHandler);
//    HRESULT hrResult = E_FAIL;
//
//    KGLOG_PROCESS_ERROR(pHandler);
//    KGLOG_PROCESS_ERROR(m_pEventHandler == pHandler);
//
//	m_pEventHandler = NULL;
//
//	hrResult = S_OK;
//Exit0:
//	return hrResult;
}

HRESULT KG3DModel::GetParentModel(IKG3DModel** ppModel)
{
    HRESULT hr = E_FAIL;
	KG_PROCESS_ERROR(ppModel);

	if (m_pParentModel)
    {
        *ppModel = m_pParentModel;
        
    }
	hr = S_OK;
Exit0:
    return hr;
}


HRESULT KG3DModel::GetBindBoneName(std::string &strName)
{
    if (m_pParentModel)
    {
        strName = m_scBindToObjName;
        return S_OK;
    }
    return E_FAIL;
}
HRESULT KG3DModel::IEGetBindBoneName(LPCTSTR *ppszName)
{
    HRESULT hResult = E_FAIL;

    KGLOG_PROCESS_ERROR(ppszName);
    KGLOG_PROCESS_ERROR(m_pParentModel);
    
    *ppszName = m_scBindToObjName.c_str();        

    hResult = S_OK;
Exit0:
    return hResult;

}


void KG3DModel::SetHighLightLevel(float fIllumination, BOOL bInherit)
{
	m_fIlluminationLevel = fIllumination;
	m_RuntimeMaterial.SetHighLight(fIllumination);
	if (bInherit)
	{
		for (std::set<KG3DModelLod>::iterator i = m_ChildModels.begin(); 
			i != m_ChildModels.end(); i++)
		{
			KG3DModelLod& Lod = *i;
			if (Lod.pModelCurrent)
			{
				Lod.pModelCurrent->SetHighLightLevel(fIllumination, bInherit);
			}
		}
	}
}

float KG3DModel::GetHightLightLevel()
{
	return m_fIlluminationLevel;
}

HRESULT KG3DModel::SetLOD(float fLod)
{
	m_fLOD = fLod;

	std::set<KG3DModelLod>::iterator i = m_ChildModels.begin();
	std::set<KG3DModelLod>::iterator itEnd = m_ChildModels.end();

	while (i != itEnd)
	{
		KG3DModelLod& Lod = *i;

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

		}

		if(!Lod.pModelCurrent->m_lpMaterial)
		{
			Lod.pModelCurrent->m_lpMaterial = Lod.pModelHigh->m_lpMaterial;
			if(Lod.pModelCurrent->m_lpMaterial)
				Lod.pModelCurrent->m_lpMaterial->AddRef();
		}
		
		i++;
	}
	return S_OK;
}

HRESULT KG3DModel::GetLOD(float& fLod)
{
	fLod = m_fLOD;
	return S_OK;
}

HRESULT KG3DModel::ShowBones(BOOL bShow)
{
	m_bShowBones = bShow;
    return S_OK;
}


HRESULT KG3DModel::EnableWarper(BOOL bEnable) 
{
    ASSERT(FALSE);
    return E_FAIL;
}

HRESULT KG3DModel::AttachBip(KG3DBip* pBip)
{
    HRESULT hrResult = E_FAIL;
    HRESULT hrRetCode = E_FAIL;

    KGLOG_PROCESS_ERROR(pBip);

    m_pBip = pBip;
    m_pBip->AddRef();

    m_Mesh2Bip.clear();

    KG_ASSERT_EXIT(m_lpMesh);

    hrRetCode = m_lpMesh->AttachBip(m_pBip, m_Mesh2Bip);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    D3DXMATRIX* pObjMatrices = m_pBip->GetObjMatricesArray();
    KG_ASSERT_EXIT(pObjMatrices);
    for (DWORD i = 0; i < m_lpMesh->m_dwNumBone; i++)
    {
        if (m_Mesh2Bip[i] != -1)
        {
            pObjMatrices[m_Mesh2Bip[i]] = m_lpMesh->m_pBoneInfo[i].CurMatrix;
        }
    }

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KG3DModel::GetMeshBoneToBipInfo(int& nNumBoneBip,int& nNumBoneMesh)
{
	nNumBoneBip  = 0;
	nNumBoneMesh = 0;

	for (DWORD i = 0; i < m_lpMesh->m_dwNumBone; i++)
	{
		if (m_Mesh2Bip[i] == -1)
		{
			nNumBoneMesh++;
		}
		else
		{
			nNumBoneBip++;
		}
	}

	return S_OK;
}

KG3DAnimationWarper* KG3DModel::GetAnimationWarper()
{
	return m_Splitter.GetAnimationWarper();
}
int KG3DModel::FindSocket(const TCHAR* strSocketName, BindExtraInfo* pExtInfo)
{
	int nReturn = -1;
	KG_PROCESS_ERROR(m_lpMesh);
	((IEKG3DMesh *)(KG3DMesh *)m_lpMesh)->FindSocket(strSocketName, &nReturn);
	if (pExtInfo)
	{
		pExtInfo->pModel = this;
		pExtInfo->nBindIndex = nReturn;
	}
Exit0:
	return nReturn;
}

int KG3DModel::FindBone(const TCHAR* strBoneName, BindExtraInfo* pExtInfo)
{
	int nReturn = -1;
	KG_PROCESS_ERROR(m_lpMesh);
	nReturn = m_lpMesh->FindBone(strBoneName);
	if (pExtInfo)
	{
		pExtInfo->pModel = this;
	}
Exit0:
	return nReturn;	
}
HRESULT KG3DModel::FindBone(const TCHAR* strBoneName, BindExtraInfo* pExtInfo, INT *pnRet)
{
    HRESULT hResult = E_FAIL;

    KGLOG_PROCESS_ERROR(strBoneName);
    //KGLOG_PROCESS_ERROR(pExtInfo); pExtInfo could be null
    KGLOG_PROCESS_ERROR(pnRet);

    *pnRet = FindBone(strBoneName, pExtInfo);

    hResult = S_OK;
Exit0:
    return hResult;
}

enuModelBindType KG3DModel::GetBindType()
{
	return m_BindType;
}

HRESULT KG3DModel::IEGetBindType(enuModelBindType *pEnu)
{   
    HRESULT hResult = E_FAIL;

    KGLOG_PROCESS_ERROR(pEnu);
   
    *pEnu = m_BindType;

    hResult = S_OK;
Exit0:
    return hResult;
}
HRESULT KG3DModel::GetSocketMatrix(unsigned int uIndex, 
								   D3DXMATRIX& mat,
								   D3DXVECTOR3* pvec3LocalOffset /* = NULL*/)
{
	HRESULT hr = E_FAIL;
	D3DXMATRIX matTemp;

	KGLOG_PROCESS_ERROR(uIndex < m_dwNumSockets);
    KGLOG_PROCESS_ERROR(m_pSocketMatrix);
	if (pvec3LocalOffset)
	{
		matTemp = m_pSocketMatrix[uIndex];
		matTemp._41 += pvec3LocalOffset->x;
		matTemp._42 += pvec3LocalOffset->y;
		matTemp._43 += pvec3LocalOffset->z;

		mat = matTemp * m_matWorld;
	}
	else
	{
		mat = m_pSocketMatrix[uIndex] * m_matWorld;
	}
	hr = S_OK;
Exit0:
	return hr;
}

HRESULT KG3DModel::GetBoneMatrixOrg(UINT nBoneIndex, D3DXMATRIX* pMatOrg)
{
    HRESULT hResult  = E_FAIL;

    KG_PROCESS_ERROR(m_lpMesh);
    KG_PROCESS_ERROR(pMatOrg);
    KG_PROCESS_ERROR((int)nBoneIndex < m_lpMesh->GetNumBones());

    *pMatOrg = m_lpMesh->m_pBoneInfo[nBoneIndex].CurMatrix;

    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT KG3DModel::GetBoneFamily(
        DWORD dwBoneIndex,
        DWORD *pdwRetParentIndex, 
        DWORD *pdwRetChildBoneNum,
        const DWORD **ppdwRetChildBoneIndexs
)
{
    HRESULT hResult  = E_FAIL;
    KG3DMesh::BoneInfo1 *pBoneInfo = NULL;

    KGLOG_PROCESS_ERROR(pdwRetParentIndex);
    KGLOG_PROCESS_ERROR(pdwRetChildBoneNum);
    KGLOG_PROCESS_ERROR(ppdwRetChildBoneIndexs);

    KGLOG_PROCESS_ERROR(m_lpMesh);
    KGLOG_PROCESS_ERROR(dwBoneIndex < m_lpMesh->m_dwNumBone);

    pBoneInfo = &(m_lpMesh->m_pBoneInfo[dwBoneIndex]);

    *pdwRetParentIndex  = pBoneInfo->dwParentIndex;
    *pdwRetChildBoneNum = pBoneInfo->dwNumChild;
    *ppdwRetChildBoneIndexs = &(pBoneInfo->dwChildIndex[0]);

    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT KG3DModel::GetBoneMatrixLocal(
								 UINT uIndex, 
								 D3DXMATRIX* pMat, 
								 D3DXVECTOR3* pvec3LocalOffset
								 )
{
	HRESULT hrResult = E_FAIL;
	HRESULT hrRetCode = E_FAIL;
	static int s_nOutputErrorFlag = true;
	int nNumBones = 0;
	D3DXMATRIX matTemp;


	KG_ASSERT_EXIT(pMat);
	KG_PROCESS_ERROR(m_lpMesh);
	hrRetCode = m_lpMesh->GetNumBones(&nNumBones);
	KGLOG_COM_PROCESS_ERROR(hrRetCode);
	KG_PROCESS_ERROR((int)uIndex < nNumBones);

	if (m_pBip)
	{
		D3DXMATRIX* pObjMatrices = m_pBip->GetObjMatricesArray();
		ASSERT(pObjMatrices);

		if (pvec3LocalOffset)
		{
			if(uIndex < m_Mesh2Bip.size())
			{
				if (m_Mesh2Bip[uIndex] == -1)
				{
					matTemp = m_pBoneMatricesForRender[uIndex];
					matTemp._41 += pvec3LocalOffset->x;
					matTemp._42 += pvec3LocalOffset->y;
					matTemp._43 += pvec3LocalOffset->z;
				}
				else
				{
					matTemp = pObjMatrices[m_Mesh2Bip[uIndex]];
					matTemp._41 += pvec3LocalOffset->x;
					matTemp._42 += pvec3LocalOffset->y;
					matTemp._43 += pvec3LocalOffset->z;
				}
				*pMat = matTemp;
			}

		}
		else
		{
			if(uIndex < m_Mesh2Bip.size())
			{
				if (m_Mesh2Bip[uIndex] == -1)
				{
					*pMat = m_pBoneMatricesForRender[uIndex];
				}	
				else
				{
					*pMat = pObjMatrices[m_Mesh2Bip[uIndex]];
				}
			}
		}
	}
	else
	{
		if (pvec3LocalOffset)
		{
			matTemp = m_pBoneMatricesForRender[uIndex];
			matTemp._41 += pvec3LocalOffset->x;
			matTemp._42 += pvec3LocalOffset->y;
			matTemp._43 += pvec3LocalOffset->z;

			*pMat =  matTemp;
		}
		else
		{
			*pMat =  m_pBoneMatricesForRender[uIndex];
		}
	}

	hrResult = S_OK;
Exit0:
	return hrResult;
}
HRESULT KG3DModel::GetBoneMatrix(
    UINT uIndex, 
    D3DXMATRIX* pMat, 
    D3DXVECTOR3* pvec3LocalOffset
)
{
    HRESULT hrResult = E_FAIL;

	D3DXMATRIX matTemp;
	hrResult = GetBoneMatrixLocal(uIndex,&matTemp,pvec3LocalOffset);
	KGLOG_COM_PROCESS_ERROR(hrResult);

	(*pMat) = matTemp * m_matWorld;

    hrResult = S_OK;
Exit0:
	return hrResult;
}
HRESULT KG3DModel::SetBoneMatrixLocal(UINT uIndex, D3DXMATRIX* pMat)
{
	HRESULT hrResult = E_FAIL;
	HRESULT hRetCode = E_FAIL;
	int nNumBones = 0;
	static int s_nOutputErrorFlag = true;

	KG_ASSERT_EXIT(pMat);
	KG_PROCESS_ERROR(m_lpMesh);

	hrResult = m_lpMesh->GetNumBones(&nNumBones);
	KGLOG_COM_PROCESS_ERROR(hrResult);

	KGLOG_PROCESS_ERROR(uIndex < (unsigned)nNumBones);

	if (m_pBip)
	{
		D3DXMATRIX* pObjMatrices = m_pBip->GetObjMatricesArray();

		ASSERT(pObjMatrices);
		if (m_Mesh2Bip[uIndex] == -1)
		{
			m_pBoneMatricesForRender[uIndex] = (*pMat) ;
		}	
		else
		{
			pObjMatrices[m_Mesh2Bip[uIndex]] = (*pMat) ;
		}
	}
	else
	{
		m_pBoneMatricesForRender[uIndex] = (*pMat) ;
	}

	hRetCode = S_OK;
Exit0:
	return hRetCode;
}

void KG3DModel::GetMatrixWorldInv(D3DXMATRIX& matWorldInv)
{
	D3DXMatrixInverse(&matWorldInv,NULL,&m_matWorld);
}

HRESULT KG3DModel::SetBoneMatrix(UINT uIndex, D3DXMATRIX* pMat,D3DXMATRIX& matWorldInv)
{
	HRESULT hrResult = E_FAIL;
	HRESULT hrRetCode = E_FAIL;

    D3DXMATRIX matLocal;
    static int s_nOutputErrorFlag = true;

    // 检查爆模
    if(
        s_nOutputErrorFlag &&
        (
            (fabs(pMat->_41 - m_matWorld._41) > 1000.0f) || 
            (fabs(pMat->_42 - m_matWorld._42) > 1000.0f) || 
            (fabs(pMat->_43 - m_matWorld._43) > 1000.0f)
        )
    )
    {
        _LogMatrix("KG3DModel::SetBoneMatrix::pMat", pMat);
        _LogMatrix("KG3DModel::SetBoneMatrix::m_matWorld", &m_matWorld);
        _LogErrorModel(this, uIndex);
        s_nOutputErrorFlag = false;
        ASSERT(false && "Out of range");
    }
    
	matLocal = (*pMat) * matWorldInv;

	hrRetCode = SetBoneMatrixLocal(uIndex,&matLocal);
	KGLOG_COM_PROCESS_ERROR(hrRetCode);

	hrResult = S_OK;
Exit0:
	return hrResult;
}
HRESULT KG3DModel::ResizeSocketMatrix()
{
    HRESULT hResult  = E_FAIL;

    KG3DMesh* pMesh = dynamic_cast<KG3DMesh*>(m_lpMesh);

    KG_PROCESS_ERROR(pMesh);
    KG_PROCESS_SUCCESS(pMesh->m_dwNumSocket <= 0);

    SAFE_DELETE_ARRAY(m_pSocketMatrix);
    m_pSocketMatrix = new D3DXMATRIX[pMesh->m_dwNumSocket];
    KG_PROCESS_ERROR(m_pSocketMatrix);

    for (DWORD i = 0; i < pMesh->m_dwNumSocket; i++)
	{
		m_pSocketMatrix[i] = pMesh->m_pSockets[i].matMatrixOffset;
	}


Exit1:
    hResult = S_OK;
Exit0:
    return hResult;
}

void KG3DModel::SetVisible()
{
	std::set<KG3DModelLod>::iterator i = m_ChildModels.begin();
	std::set<KG3DModelLod>::iterator itEnd = m_ChildModels.end();
	m_bVisible = TRUE;
	while (i != itEnd)
	{
		KG3DModelLod& Lod = *i;
		if (Lod.pModelCurrent)
		{
			Lod.pModelCurrent->SetVisible();
		}
		i++;
	}
}

void KG3DModel::UpdateSockets(D3DXMATRIX* matSrc,int nCount)
{
	KG_PROCESS_ERROR(matSrc);
	KG_PROCESS_ERROR(m_lpMesh);
	m_dwNumSockets = m_lpMesh->GetNumSockets();
	KG_PROCESS_ERROR(m_dwNumSockets);
	KG_PROCESS_ERROR(m_pSocketMatrix);

	if (!m_Mesh2Bip.size())
	{
		for (DWORD i = 0; i < m_dwNumSockets; i++)
		{
			DWORD dwParentBone = m_lpMesh->m_pSockets[i].dwParentBoneIndex;
			
			if((int)dwParentBone < nCount)
				m_pSocketMatrix[i] = m_lpMesh->m_pSockets[i].matMatrixOffset * matSrc[dwParentBone];
		}
	}
	else
	{
		for (DWORD i = 0; i < m_dwNumSockets; i++)
		{
			DWORD dwParentBone = m_lpMesh->m_pSockets[i].dwParentBoneIndex;
			if(dwParentBone < (DWORD)m_Mesh2Bip.size())
			{
				int nBipBoneIndex = m_Mesh2Bip[dwParentBone];

				if(nBipBoneIndex>=0 && nBipBoneIndex<nCount)
				{
					m_pSocketMatrix[i] = m_lpMesh->m_pSockets[i].matMatrixOffset * matSrc[nBipBoneIndex];
				}
			}
		}
	}

Exit0:
	return;
}

void KG3DModel::UpdateSockets(D3DXMATRIX* matSrc,int nCount,int nIndex)
{
	KG_PROCESS_ERROR(matSrc);
	KG_PROCESS_ERROR(m_lpMesh);
	m_dwNumSockets = m_lpMesh->GetNumSockets();
	KG_PROCESS_ERROR(m_dwNumSockets);
	KG_PROCESS_ERROR(m_pSocketMatrix);
	if (!m_Mesh2Bip.size())
	{
		DWORD dwParentBone      = m_lpMesh->m_pSockets[nIndex].dwParentBoneIndex;
		m_pSocketMatrix[nIndex] = m_lpMesh->m_pSockets[nIndex].matMatrixOffset * matSrc[dwParentBone];
	}
	else
	{
		DWORD dwParentBone = m_lpMesh->m_pSockets[nIndex].dwParentBoneIndex;
		if(dwParentBone < (DWORD)m_Mesh2Bip.size())
		{
			int nBipBoneIndex = m_Mesh2Bip[dwParentBone];
			if(nBipBoneIndex>=0 && nBipBoneIndex<nCount)
			{
				m_pSocketMatrix[nIndex] = m_lpMesh->m_pSockets[nIndex].matMatrixOffset * matSrc[nBipBoneIndex];
			}
			else
			{
				//ASSERT(0);
			}
		}
	}

Exit0:
	return;
}

void KG3DModel::OnBind(IKG3DModel* pModel,KG3DModel* pModelMedium,KG3DModel* pModelLow)
{
	m_bBindChanged = TRUE;
	KG3DModelLod Lod;
	Lod.pModelHigh = static_cast<KG3DModel*>(pModel);
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
} 

IKG3DAnimationController* KG3DModel::GetAnimationController(enuAnimationControllerPriority nPriority)
{
	IKG3DAnimationController* pReturn = NULL;
    KG_PROCESS_ERROR(nPriority == ANICTL_PRIMARY);
	
	m_AnimationController.SetModel(this);
	pReturn = &m_AnimationController;
	
	m_AnimationController.GetUpateAnimationInfo().pMatrix = m_pBoneLocalMatrixBuffer;
	if (m_lpMesh && m_lpMesh->GetType() == MESHTYPE_DEFAULT)
	{
		m_AnimationController.GetUpateAnimationInfo().pMeshSrc = m_lpMesh->m_ppMeshes[KG3DMesh::SMBT_NORMAL];
	}
	else
	{
		m_AnimationController.GetUpateAnimationInfo().pMeshSrc = NULL;
	}
Exit0:
	return pReturn;
}

HRESULT KG3DModel::GetSFXSuede(IEKG3DSFXSuede **ppSFX)
{
    HRESULT hResult = E_FAIL;

    KGLOG_PROCESS_ERROR(ppSFX);

    if (!m_pSFXSuede)
        m_pSFXSuede = new KG3DSFXSuede;
    
    *ppSFX = m_pSFXSuede;
    
    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT KG3DModel::UnbindChild(const TCHAR* strBindObjName)
{
    HRESULT hResult = E_FAIL;
    
	set<KG3DModelLod>::iterator i;
    KG_PROCESS_ERROR(strBindObjName);
    i = m_ChildModels.begin();
	while (i != m_ChildModels.end())
	{
		KG3DModelLod& Lod = *i;

		if (!strcmp(Lod.pModelCurrent->m_scBindToObjName.c_str(), strBindObjName))
		{
			Lod.pModelCurrent->UnBindFromOther();
			i = m_ChildModels.begin();
		}
		else
		{
			i++;
		}
	}
    
    hResult = S_OK;
Exit0:
	return hResult;
}

HRESULT KG3DModel::GetBoneMatrix(
    const BindExtraInfo* pInfo, 
    D3DXMATRIX* pmatResult, 
    D3DXVECTOR3* pvec3Offset
)
{
	HRESULT hr = E_FAIL;
	KG_PROCESS_ERROR(pInfo);
	KG_PROCESS_ERROR(pmatResult);

	hr = pInfo->pModel->GetBoneMatrix(pInfo->nBindIndex, pmatResult, pvec3Offset);
	KG_COM_PROCESS_ERROR(hr);

Exit0:
	return hr;
}

HRESULT KG3DModel::GetSocketMatrix(const BindExtraInfo* pInfo, 
								   D3DXMATRIX* pmatResult,
								   D3DXVECTOR3* pvec3Offset)
{
	HRESULT hr = E_FAIL;
	KG_PROCESS_ERROR(pInfo);
	KG_PROCESS_ERROR(pmatResult);

	hr = pInfo->pModel->GetSocketMatrix(pInfo->nBindIndex, *pmatResult, pvec3Offset);
	KG_COM_PROCESS_ERROR(hr);
Exit0:
	return hr;
}

void AlignVal(int &nVal, float fVal, int Align)
{
    if (fVal < 0)
        nVal = (int)(fVal) - 1;
    else
        nVal = (int)(fVal) + 1;

    int t = nVal - nVal / Align * Align;
    if (t > 0)
        nVal = nVal / Align * Align + Align;
    else if (t == 0)
        nVal = nVal / Align * Align;
    else
        nVal = nVal / Align * Align - Align;
}

void KG3DModel::RenderLogicCellBox()
{
	KG_PROCESS_ERROR(m_lpMesh);
	KG_PROCESS_ERROR(m_lpMesh->GetType() == MESHTYPE_DEFAULT);

	KG_PROCESS_ERROR(m_lpMesh->m_ppMeshes[KG3DMesh::SMBT_NORMAL]);

//	if (!m_pLogicalBox)
//	{
//        BBox box = m_BBox;
//        g_cGraphicsTool.AlignBBox(box, 50, 12.5, 50);
//
////		g_cGraphicsTool.CreateLogicalInfoByMesh_Verion_Spe(&m_pLogicalBox, 
//        int nAlloc = 0;
//		g_cGraphicsTool.CreateLogicalInfoByMesh(m_pLogicalBox, 
//			box, 
//			pMesh->m_ppMeshes[KG3DMesh::SMBT_NORMAL],
//			50,
//			12.5,
//			m_Matrix,
//			m_nCellInX,
//            m_nCellInY,
//            m_nCellInZ,
//            nAlloc);
//	}
//
//	g_cGraphicsTool.RenderMeshLogicalBBox(m_pLogicalBox, m_nCellInX, m_nCellInY, m_nCellInZ);
Exit0:
	return;
}

HRESULT KG3DModel::RenderToTexture(DWORD dwTextureID, 
								   D3DXVECTOR2 const* pA, 
								   D3DXVECTOR2 const* pC, 
								   D3DXVECTOR2 const* pUVA, 
								   D3DXVECTOR2 const* pUVC,
								   float fZ, 
								   DWORD dwFlags,
								   DWORD dwBackColor,
								   DWORD dwRenderOption,
								   void* pExtInfo)
{
	HRESULT hr = E_FAIL;
	HRESULT hRetCode = E_FAIL;
    KG3DTexture* piRenderTexture = NULL;
	IDirect3DTexture9* piTexture = NULL;
	IDirect3DSurface9* piSurface = NULL;
	IDirect3DSurface9* piSurfaceSave = NULL;
	D3DVIEWPORT9 viewportSave;
	static D3DXMATRIX matViewSave;
	static D3DXMATRIX matProjSave;

	hr = g_pd3dDevice->GetViewport(&viewportSave);
	
	hr = g_cTextureTable.GetResource(&piRenderTexture, dwTextureID);
	KG_COM_PROCESS_ERROR(hr);

	piTexture = piRenderTexture->GetTexture();
	KG_PROCESS_ERROR(piTexture);

	hr = piTexture->GetSurfaceLevel(0, &piSurface);
	KG_COM_PROCESS_ERROR(hr);

	hr = g_pd3dDevice->GetRenderTarget(0, &piSurfaceSave);
    KG_COM_PROCESS_ERROR(hr);

	hr = g_pd3dDevice->SetRenderTarget(0, piSurface);
    KG_COM_PROCESS_ERROR(hr);

	if (dwFlags)
	{
		hr = g_pd3dDevice->Clear(0, 0, dwFlags, dwBackColor, 1.0f, 0);
	}

	Render(dwRenderOption, pExtInfo);
	g_pd3dDevice->SetRenderTarget(0, piSurfaceSave);
	g_pd3dDevice->SetViewport(&viewportSave);
    hRetCode = S_OK;
Exit0:
    KG_COM_RELEASE(piSurfaceSave);
    KG_COM_RELEASE(piSurface);
    //KG_COM_RELEASE(piTexture); // KG3DTexture.GetTexture not add reference
    KG_COM_RELEASE(piRenderTexture);
	return hRetCode;
}

void KG3DModel::GetBBox(D3DXVECTOR3& vec3A, 
					    D3DXVECTOR3& vec3B)
{
	vec3A = m_BBox.A;
	vec3B = m_BBox.B;
}


void KG3DModel::SetParentObjMatrix(D3DXMATRIX* pMatrix)
{
 	m_pParentMatrix = pMatrix;
}

D3DXMATRIX* KG3DModel::GetParentObjMarrix(D3DXMATRIX* pMatrix)
{
	return m_pParentMatrix;
}

HRESULT KG3DModel::RenderChildModels(unsigned int uOption, void* pExtInfo)
{
	std::set<KG3DModelLod>::iterator iend = m_ChildModels.end();
	for (std::set<KG3DModelLod>::iterator i = m_ChildModels.begin(); i != iend; i++)
	{
		KG3DModelLod& Lod = *i;
		//if (*i && ((!(*i)->m_nIsSockWave) || ((*i)->m_dwShockMode == SHOCK_WAVE_ADD)))

		if (Lod.pModelCurrent->CheckNeedRender())
        {
			Lod.pModelCurrent->Render(uOption | RENDER_PLAYER, NULL);
		}
	}

	return S_OK;
}

HRESULT KG3DModel::RenderChain(unsigned int uOption, void* pExtInfo)
{
	HRESULT hr = E_FAIL;
	hr = Render(uOption, pExtInfo);
	
	/*std::set<KG3DModel*>::iterator iend = m_ChildModels.end();
	for (std::set<KG3DModel*>::iterator i = m_ChildModels.begin(); i != iend; i++)
	{
		if (*i && !(*i)->m_nIsSockWave)
		{
			(*i)->Render(uOption, NULL);
		}
	}*/

	/*for(std::set<KG3DModel*>::iterator i = m_ChildModels.begin(); 
		i != m_ChildModels.end();
		i++)
	{
		hr = (*i)->Render(uOption, NULL);
		
	}*/

	return hr;
}

BOOL KG3DModel::IsSecondUVExist()
{
	BOOL bReturn = FALSE;
	KG_PROCESS_ERROR(m_lpMesh);

	bReturn = m_lpMesh->IsSecondUV();
Exit0:
	return bReturn;
}

void KG3DModel::GetProfile(DWORD& dwNumFacesTotal,DWORD& dwNumVertices,DWORD& dwNumTextures,DWORD& dwNumID)
{
	if(m_lpMesh)
	{
		m_lpMesh->GetProfile(dwNumFacesTotal,dwNumVertices);
	}
	if(m_lpMaterial && m_lpMaterial->IsLoaded())
	{
		dwNumID +=m_lpMaterial->GetNumMaterial();
		dwNumTextures += m_lpMaterial->GetNumMaterial();
	}
}


DWORD KG3DModel::GetDefaultColorCastIndex()
{
	return m_dwDefaultColorCastIndex;
}

void KG3DModel::SetDefaultColorCastIndex(DWORD dwIndex)
{
	if (dwIndex >= KG3DMaterial::KG3DMaterialSubset::cNumColorCast)
		m_dwDefaultColorCastIndex = 0;
	else
		m_dwDefaultColorCastIndex = dwIndex;
}

DWORD KG3DModel::GetSceneLayerID()
{
	return m_dwSceneLayerID;
}

void  KG3DModel::SetSceneLayerID(DWORD dwID)
{
	m_dwSceneLayerID = dwID;
}

HRESULT KG3DModel::GetRenderTargetTextureCount(OUT DWORD* pTexCount)
{
    KG_PROCESS_ERROR(pTexCount);
    *pTexCount = 0;
Exit0:
    return E_NOTIMPL;
}

HRESULT KG3DModel::GetRenderTargetTexture(OUT DWORD* pResourceId, IN DWORD dwTexIndex /* = 0 */)
{
    KG_PROCESS_ERROR(pResourceId);
    *pResourceId = 0;
Exit0:
    return E_NOTIMPL;
}

HRESULT KG3DModel::SetCallbackRender(CallbackRender fpCallbackRender, void* pUserData)
{
    return E_FAIL;
}

//HRESULT KG3DModel::UndateSuedeBindInfo()
//{
//    HRESULT hResult  = E_FAIL;
//    HRESULT hRetCode = E_FAIL;
//    
//    DWORD dwSFXCount = 0;
//    set<KG3DModel*>::iterator i = m_ChildModels.begin();
//    KG_PROCESS_ERROR(m_pSFXSuede);
//    while (i != m_ChildModels.end())
//    {
//        (*i)->UnBindFromOther();
//        i = m_ChildModels.begin();
//    }
//    dwSFXCount = static_cast<DWORD>(m_pSFXSuede->GetSFXCount());
//    for (DWORD dw = 0; dw < dwSFXCount; dw++)
//    {
//        KG3DSFX* pSFX = (KG3DSFX*)m_pSFXSuede->GetSFX(dw);
//        hRetCode = pSFX->BindToBone(this, block.cszBoneName);
//    }
//
//    hResult = S_OK;
//Exit0:
//    return hResult;
//}


HRESULT KG3DModel::BindSFXSuedeFromFile(LPCSTR pstrBindFileName, IKG3DScene* pScene)
{
    HRESULT hResult  = E_FAIL;
    HRESULT hRetCode = E_FAIL;

    KG3DScene* pActiveScene = NULL;
    DWORD      dwSFXCount   = 0;

    KGLOG_PROCESS_ERROR(pstrBindFileName);

    SAFE_DELETE(m_pSFXSuede);
    m_pSFXSuede = new KG3DSFXSuede();
    ASSERT(m_pSFXSuede);

   /* hRetCode = m_pSFXSuede->LoadFromBindFile(this, pstrBindFileName);*/
    hRetCode = m_pSFXSuede->LoadFromFile(pstrBindFileName, 0, 0);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = m_pSFXSuede->Bind(this);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    if (pScene)
        pActiveScene = dynamic_cast<KG3DScene*>(pScene);
    else
        pActiveScene = dynamic_cast<KG3DScene*>(g_cGraphicsTool.GetScene());
    KGLOG_PROCESS_ERROR(pActiveScene);

    dwSFXCount = static_cast<DWORD>(m_pSFXSuede->GetModelCount());
    for (DWORD dw = 0; dw < dwSFXCount; dw++)
    {
        KG3DModel* pSFX = (KG3DModel*)m_pSFXSuede->GetModel(dw);
        ASSERT(pSFX->GetType() == MESHTYPE_SFX);
        pActiveScene->RemoveRenderEntity(pSFX, SCENEENTITY_MODEL, TRUE);
        pActiveScene->AddRenderEntity(pSFX, SCENEENTITY_MODEL, TRUE,FALSE);
        pSFX->PlayAnimation(ENU_ANIMATIONPLAY_CIRCLE, NULL, 1.0f, 0, NULL, NULL, ANICTL_PRIMARY);
    }
    hResult = S_OK;
Exit0:
    return hResult;
}


HRESULT KG3DModel::GetModel(unsigned int uIndex, IKG3DModel** ppModel)
{
	return E_NOTIMPL;
}

HRESULT KG3DModel::ChangeModel(unsigned int uIndex, IKG3DModel* pModel)
{
	return E_NOTIMPL;
}

void KG3DModel::CopyBindInfo(KG3DModel* pModel)
{
	std::set<KG3DModelLod>Children;
    KG_PROCESS_ERROR(pModel);
    Children = pModel->m_ChildModels;

	for (std::set<KG3DModelLod>::iterator i = m_ChildModels.begin();
		i != m_ChildModels.end(); i++)
	{
		KG3DModelLod& Lod = *i;
		if (Lod.pModelHigh)
		{
			Lod.pModelHigh->UnBindFromOther();
		}
	}

	for (std::set<KG3DModelLod>::iterator i = Children.begin();
		i != Children.end(); i++)
	{
		KG3DModelLod& Lod = *i;

		if (!Lod.pModelHigh)
			continue;

		/*sprintf_s(strCopyModelPathName, 
			MAX_PATH, 
			"%s",
			(*i)->m_scName.c_str());

		hr = g_cModelTable.LoadResourceFromFile(strCopyModelPathName, 0, MLO_TEXTURE_MULTI, (IKG3DResourceBase**)&pChildModel);
		if (FAILED(hr))
		{
			KGLogPrintf(KGLOG_ERR, 
				"KG3DModel::CopyBindInfo() from model %s load ChildModel %s failed.", 
				m_scName.c_str(), 
				(*i)->m_scName.c_str());
			continue;
		}*/

		TCHAR strBindObjName[MAX_PATH];
		strcpy_s(strBindObjName, MAX_PATH, Lod.pModelHigh->m_scBindToObjName.c_str());
		switch(Lod.pModelHigh->m_BindType)
		{
		case ENUM_BIND_BONE:
			{
				Lod.pModelHigh->BindToBone(this, strBindObjName);
			}
			break;
		case ENUM_BIND_SOCKET:
			{
				Lod.pModelHigh->BindToSocket(this, strBindObjName);
			}
			break;
		}
	}
Exit0:
	return;
}

HRESULT KG3DModel::SetEditMode(BOOL bEditMode, 
				 IEKG3DSceneModelEditor* pScene)
{	
    return E_FAIL;
}


vector<KG3DModel*> KG3DModel::GetAllChilds()
{
    vector<KG3DModel*> vecChilds;
	vecChilds.reserve(m_ChildModels.size());//预分配内存能大大加速
    for (std::set<KG3DModelLod>::iterator i = m_ChildModels.begin(); i != m_ChildModels.end(); i++)
	{
		KG3DModelLod& Lod = *i;
		if(Lod.pModelCurrent)
			vecChilds.push_back(Lod.pModelCurrent);
	}
    return vecChilds;
}

HRESULT KG3DModel::GetAllChilds(vector<IEKG3DModel*> &vecpChildsModel)
{
	vecpChildsModel.reserve(vecpChildsModel.size() + m_ChildModels.size());
	for (std::set<KG3DModelLod>::iterator i = m_ChildModels.begin(); i != m_ChildModels.end(); i++)
	{		
		KG3DModelLod& Lod = *i;
		if(Lod.pModelCurrent)
			vecpChildsModel.push_back(static_cast<IEKG3DModel*>(Lod.pModelCurrent));
	}
	return S_OK;
}

HRESULT KG3DModel::GetAllChilds(vector<IKG3DModel*> &vecpChildsModel)
{
	for (std::set<KG3DModelLod>::iterator i = m_ChildModels.begin(); i != m_ChildModels.end(); i++)
	{
		KG3DModelLod& Lod = *i;
		if(Lod.pModelCurrent)
			vecpChildsModel.push_back(static_cast<IKG3DModel*>(Lod.pModelCurrent));
	}
	return S_OK;
}

D3DXMATRIX* KG3DModel::GetBoneAnimationResultPool()
{
	return NULL;
}

DWORD KG3DModel::GetVertexNum()
{
    KG3DMesh* pMesh = dynamic_cast<KG3DMesh*>(m_lpMesh);

    if (!pMesh || !pMesh->m_ppMeshes[KG3DMesh::SMBT_NORMAL])
        return 0;

    return pMesh->m_ppMeshes[KG3DMesh::SMBT_NORMAL]->GetNumVertices();

}

DWORD KG3DModel::GetFaceNum()
{
    KG3DMesh* pMesh = dynamic_cast<KG3DMesh*>(m_lpMesh);

    if (!pMesh || !pMesh->m_ppMeshes[KG3DMesh::SMBT_NORMAL])
        return 0;

    return pMesh->m_ppMeshes[KG3DMesh::SMBT_NORMAL]->GetNumFaces();
}

HRESULT KG3DModel::GetNumBones(INT *pRetNumBones)
{
	HRESULT hr = E_FAIL;
	KG_PROCESS_ERROR(pRetNumBones);
	KG_PROCESS_ERROR(m_lpMesh);
	*pRetNumBones = m_lpMesh->GetNumBones();
    hr = S_OK;
Exit0:
	return hr;
}

HRESULT KG3DModel::GetNumSocket(INT *pRetNumSocket)
{
	HRESULT hrResult = E_FAIL;
    
    KGLOG_PROCESS_ERROR(pRetNumSocket);
	KG_PROCESS_ERROR(m_lpMesh);
	
    *pRetNumSocket = m_lpMesh->GetNumSockets();
    
    hrResult = S_OK;
Exit0:
	return hrResult;
}

LPCSTR KG3DModel::GetBoneName(int nIndex)
{
	KG_PROCESS_ERROR(m_lpMesh);
	return m_lpMesh->GetBoneName(nIndex);
Exit0:
	return NULL;
}

HRESULT KG3DModel::GetSocketName(int nIndex, LPCTSTR *ppRetName)
{
    HRESULT hrResult = E_FAIL;
    KGLOG_PROCESS_ERROR(ppRetName);
    KG_PROCESS_ERROR(m_lpMesh);

	*ppRetName = m_lpMesh->GetSocketName(nIndex);    
    KGLOG_PROCESS_ERROR(*ppRetName);

    hrResult = S_OK;
Exit0:
	return hrResult;
}

HRESULT KG3DModel::_PlaySplitAnimation(LPCSTR strAnimationName,
									   DWORD dwSplitType,
									   DWORD dwExtraInfo,
									   DWORD dwPlayType, 
									   FLOAT fSpeed,
									   int   nOffsetTime,
									   PVOID pReserved,
									   PVOID pUserdata,
									   enuAnimationControllerPriority Priority,
									   enuModelPlayAnimationType AnimationType,
									   KG3DAnimationTagContainer* pTagContainerNew,
									   KG3DClip* pClip)
{
    HRESULT hrRetsult = E_FAIL;
	HRESULT hrRetCode = E_FAIL;

	switch (dwPlayType)
	{
	case ENU_ANIMATIONPLAY_ADDCURRENT_ONCE:
		dwPlayType = ENU_ANIMATIONPLAY_ONCE;
		break;
	case ENU_ANIMATIONPLAY_ADDCURRENT:
		dwPlayType = ENU_ANIMATIONPLAY_CIRCLE;
		break;
	}

    m_Splitter.SetModel(this);

	hrRetCode = m_Splitter.LoadConfig();
	KG_COM_PROCESS_ERROR(hrRetCode);

	hrRetCode = m_Splitter.PlayAnimation(
        strAnimationName, 
		dwSplitType, 
		dwExtraInfo, 
		dwPlayType,
		fSpeed, 
		nOffsetTime, 
		pReserved,
		pUserdata,
		Priority,
		AnimationType,
		pTagContainerNew,
		pClip);
    KG_COM_PROCESS_ERROR(hrRetCode);

    m_bHasAnimation = TRUE;

    hrRetsult = S_OK;
Exit0:
	return hrRetsult;
}

HRESULT KG3DModel::PlaySplitAnimation(LPCSTR strAnimationName,
									  DWORD dwSplitType,
									  DWORD dwExtraInfo,
									  DWORD dwPlayType, 
									  FLOAT fSpeed,
									  int   nOffsetTime,
									  PVOID pReserved,
									  PVOID pUserdata,
									  enuAnimationControllerPriority Priority)
{
	HRESULT hr = E_FAIL;
	enuModelPlayAnimationType  AniType = MPAT_INVALIDATE;
	KG3DAnimationTagContainer* pTagInput = NULL;
	KG3DClip* pClipInput = NULL;

	//////////////////////////////////////////////////////////////////////////
	BOOL bIsPassRecord = FALSE;
	if (g_cRecorder.IsRecord() && !g_cRecorder.IsLock()  && m_dwRecordID != 0 )
	{
		KG3DRecorder::Parameters MyParameters;
		size_t uOffSet = 0;
		MyParameters.nNum = 6;
		MyParameters.size_tDataSize =MAX_PATH * sizeof(char) +
			sizeof(dwSplitType) + sizeof(dwExtraInfo) + sizeof(dwPlayType) + sizeof(fSpeed) + sizeof(nOffsetTime);
		char* pData = new char[MyParameters.size_tDataSize];
		KG_PROCESS_ERROR(pData);
		MyParameters.pData = pData;
		MyParameters.nFrom[0] = 0;		
		strcpy(pData,strAnimationName);
		uOffSet = MAX_PATH * sizeof(char);
		MyParameters.nFrom[1] = static_cast<int>(uOffSet);
		*(DWORD*)(&pData[MyParameters.nFrom[1]]) = dwSplitType;
		uOffSet += sizeof(dwSplitType);
		MyParameters.nFrom[2] = static_cast<int>(uOffSet);
		*(DWORD*)(&pData[MyParameters.nFrom[2]]) = dwExtraInfo;
		uOffSet += sizeof(dwExtraInfo);
		MyParameters.nFrom[3] = static_cast<int>(uOffSet);
		*(DWORD*)(&pData[MyParameters.nFrom[3]]) = dwPlayType;
		uOffSet += sizeof(dwPlayType);
		MyParameters.nFrom[4] = static_cast<int>(uOffSet);
		*(float*)(&pData[MyParameters.nFrom[4]]) = fSpeed;
		uOffSet += sizeof(fSpeed);
		MyParameters.nFrom[5] = static_cast<int>(uOffSet);
		*(int*)(&pData[MyParameters.nFrom[5]]) = nOffsetTime;

		g_cRecorder.RecordEvent(m_dwRecordID,FT_KG3DMODEL_PALYSPLITANIMATION,GetCurrentTime(),MyParameters);
		g_cRecorder.Lock();
		bIsPassRecord = TRUE;
	}

    BOOL bIsFirstAni = (!m_bHasAnimation && g_bClient);

	hr = GetAniResource(strAnimationName,AniType,pTagInput,pClipInput, bIsFirstAni);
	KGLOG_COM_PROCESS_ERROR(hr);

	BOOL bAniLoaded = IsAniLoaded(AniType,pTagInput,pClipInput);

	if (IsLoaded() && bAniLoaded && !m_dwPlayAnimationCommandCnt)
	{
		hr = _PlaySplitAnimation(strAnimationName,
			dwSplitType,
			dwExtraInfo,
			dwPlayType, 
			fSpeed,
			nOffsetTime,
			pReserved,
			pUserdata,
			Priority,
			AniType,pTagInput,pClipInput);
	}
	else
    {
		KG3DModelCommand_PlaySplitAnimation* pCmd = new KG3DModelCommand_PlaySplitAnimation(strAnimationName,
			dwSplitType,
			dwExtraInfo,
			dwPlayType, 
			fSpeed,
			nOffsetTime,
			pReserved,
			pUserdata,
			Priority,
			AniType,pTagInput,pClipInput);
		m_dwPlayAnimationCommandCnt++;
		m_Commands.AddCommand(pCmd, TRUE);
		hr = S_OK;
	}
Exit0:
	SAFE_RELEASE(pTagInput);
	SAFE_RELEASE(pClipInput);

	if (bIsPassRecord)
    {
		g_cRecorder.UnLock();
        bIsPassRecord = false;
    }
	return hr;
}

HRESULT KG3DModel::SetBoneTransform(LPCSTR strBoneName,
								 const BoneTransformationInputData& Data)
{
	ASSERT(0);
	return S_OK;
}

void KG3DModel::EnableBoneTransform(BOOL bEnable)
{
	m_bBoneTransformEnable = bEnable;
}

BOOL KG3DModel::IsBoneTransformEnable()
{
	return m_bBoneTransformEnable;
}
//(by dengzhihui Refactor 2006年11月9日
KG3DMaterial* GetMaterialFromModel(KG3DModel* pModel)
{
	KG3DMaterial* pReturn = NULL;
	KG3DModelSpeedTree* pSpdTree = NULL;
	KG_PROCESS_ERROR(pModel);

	if (pModel->GetType() == MESHTYPE_SPEEDTREE)
	{
		pSpdTree = static_cast<KG3DModelSpeedTree*>(pModel);
        KG_PROCESS_ERROR(pSpdTree);
		KG_PROCESS_ERROR(pSpdTree->m_lpModelTree);
		pReturn = pSpdTree->m_lpModelTree->m_lpMaterial;
	}
	else
	{
		pReturn = pModel->m_lpMaterial;
	}
Exit0:
	return pReturn;
}

HRESULT KG3DModel::GetMaterialSubsetOption(INT nSubIndex, DWORD* dwOption)	//pSubMat->m_dwOption
{
	_ASSERTE(dwOption);
	KG_PROCESS_ERROR(dwOption);

	KG3DMaterial* pMaterial = GetMaterialFromModel(static_cast<KG3DModel *>(this));
	
    KG_PROCESS_ERROR(pMaterial);
	_ASSERTE(pMaterial->IsLoaded() && _T("KG3DModel::GetMaterialSubsetOption"));

	KG3DMaterial::KG3DMaterialSubset* pSubMtl = pMaterial->GetSubMaterial(nSubIndex);
	KGLOG_PROCESS_ERROR(pSubMtl);

	*dwOption = pSubMtl->m_dwOption;

	return S_OK;
Exit0:
	return E_FAIL;
}

D3DCOLORVALUE* KG3DModel::GetMaterialSubsetColorCast(int nSubIndex,int nColorCastIndex)
{
    KG3DMaterial* pMaterial = GetMaterialFromModel(static_cast<KG3DModel *>(this));

	KGLOG_PROCESS_ERROR(pMaterial && pMaterial->IsLoaded());

	KG3DMaterial::KG3DMaterialSubset* pSubMtl = pMaterial->GetSubMaterial(nSubIndex);
	KGLOG_PROCESS_ERROR(pSubMtl);
    
	if (nColorCastIndex >=0 && nColorCastIndex < (KG3DMaterial::KG3DMaterialSubset::cNumColorCast))
		return &(pSubMtl->m_ColorCast[nColorCastIndex]);
    
Exit0:
    return NULL;
}

float*  KG3DModel::GetMaterialSubsetSpecPower(int nSubIndex)
{
    KG3DMaterial* pMaterial = GetMaterialFromModel(static_cast<KG3DModel *>(this));

	KGLOG_PROCESS_ERROR(pMaterial && pMaterial->IsLoaded());

	KG3DMaterial::KG3DMaterialSubset* pSubMtl = pMaterial->GetSubMaterial(nSubIndex);
	KGLOG_PROCESS_ERROR(pSubMtl);

    return &(pSubMtl->m_fSpecPower);

Exit0:
    return NULL;
}

float*  KG3DModel::GetMaterialSubsetEmssPower(int nSubIndex)
{
    KG3DMaterial* pMaterial = GetMaterialFromModel(static_cast<KG3DModel *>(this));

	KGLOG_PROCESS_ERROR(pMaterial && pMaterial->IsLoaded());

	KG3DMaterial::KG3DMaterialSubset* pSubMtl = pMaterial->GetSubMaterial(nSubIndex);
	KGLOG_PROCESS_ERROR(pSubMtl);

    return &(pSubMtl->m_fEmssPower);
Exit0:
    return NULL;
}

HRESULT KG3DModel::SetMaterialSubsetOption(INT nSubIndex, DWORD dwOption)		//pSubMat->m_dwOption
{
	KG3DMaterial* pMaterial = GetMaterialFromModel(static_cast<KG3DModel *>(this));

	KGLOG_PROCESS_ERROR(pMaterial && pMaterial->IsLoaded());

	KG3DMaterial::KG3DMaterialSubset* pSubMtl = pMaterial->GetSubMaterial(nSubIndex);
	KGLOG_PROCESS_ERROR(pSubMtl);

	pSubMtl->m_dwOption = dwOption;

    return S_OK;
Exit0:
    return E_FAIL;
}

HRESULT KG3DModel::GetNumMaterial(INT* pnCount)
{
	_ASSERTE(pnCount);
	KG_PROCESS_ERROR(pnCount);

	KG3DMaterial* pMaterial = GetMaterialFromModel(static_cast<KG3DModel *>(this));
	KG_PROCESS_ERROR(pMaterial && pMaterial->IsLoaded());

	*pnCount = pMaterial->GetNumMaterial();

	return S_OK;
Exit0:
	return E_FAIL;
}

KG3DMaterial::KG3DMaterialSubset* GetSubMat(KG3DModel* pModel, INT nSubIndex)
{
    KG3DMaterial::KG3DMaterialSubset *pSubMat = NULL;
    KG3DMaterial *pMaterial = NULL;

    KGLOG_PROCESS_ERROR(pModel);

	pMaterial = GetMaterialFromModel(pModel);
	KGLOG_PROCESS_ERROR(pMaterial && pMaterial->IsLoaded());

	pSubMat = pMaterial->GetSubMaterial(nSubIndex);
Exit0:
    return pSubMat;
}

HRESULT KG3DModel::GetMaterialSubsetMtlOptionData(INT nSubIndex, DWORD dwType, DWORD dwExtInfo, INT* pnData)
{
	_ASSERTE(pnData);
	KG_PROCESS_ERROR(pnData);

	KG3DMaterial::KG3DMaterialSubset* pSubMat = GetSubMat(this, nSubIndex);
	KGLOG_PROCESS_ERROR(pSubMat);
	KG3DMaterial::_MtlOption* pMtlOption = static_cast<KG3DMaterial::_MtlOption*>(pSubMat->GetMtlOption(dwType, dwExtInfo));
    KG_PROCESS_ERROR(pMtlOption);

    *pnData = static_cast<int>(*(DWORD*)(pMtlOption->pData) - 1);

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DModel::GetMaterialSubSetMaterial9(INT nSubIndex, D3DMATERIAL9** ppMaterial9)
{
	_ASSERTE(ppMaterial9);
	KG_PROCESS_ERROR(ppMaterial9);

	KG3DMaterial::KG3DMaterialSubset* pSubMat = GetSubMat(this, nSubIndex);
	KG_PROCESS_ERROR(pSubMat);
	*ppMaterial9 = &pSubMat->m_sMaterial9;

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DModel::RelaseMaterial()
{
	KG3DMaterial* pMaterial = GetMaterialFromModel(this);
	if (pMaterial)
	{
		SAFE_RELEASE(pMaterial);
		return S_OK;
	}
	else
		return E_FAIL;
}

HRESULT KG3DModel::SetMaterialSubSetAlphaOption(INT nSubIndex, DWORD dwSrcBlend, DWORD dwDestBlend,BOOL bEnableAlphaBlend)
{
	HRESULT hr = E_FAIL;
    _MtlAlphaOperation* pAlpha = NULL;
    KG3DMaterial::KG3DMaterialSubset* pSubMat = GetSubMat(this, nSubIndex);
    KG_PROCESS_ERROR(pSubMat);

    pAlpha = pSubMat->GetAlphaOperation();

	KG_PROCESS_ERROR(pAlpha);

	pAlpha->SrcBlend = dwSrcBlend;
	pAlpha->DestBlend = dwDestBlend;
	pAlpha->EnableAlphaBlend = bEnableAlphaBlend;

	hr = S_OK;
Exit0:
	return hr;
}

HRESULT KG3DModel::GetMaterialSubsetRenderTwiceAlphaOption(INT nSubsetIndex,
														BOOL& bEnable,
														DWORD& dwTopRef,
														DWORD& dwBottomRef)
{
	HRESULT hRetCode = E_FAIL;
	_MtlRenderTwiceAlphaOption* pAlpha = NULL;
	KG3DMaterial::KG3DMaterialSubset* pSubMat = GetSubMat(this, nSubsetIndex);
	KG_PROCESS_ERROR(pSubMat);

	pAlpha = pSubMat->GetAlphaRenderTwice();
	KG_PROCESS_ERROR(pAlpha);

	bEnable = pAlpha->bEnable;
	dwTopRef = pAlpha->dwAlphaRefPass0;
	dwBottomRef = pAlpha->dwAlphaRefPass1;
	hRetCode = S_OK;
Exit0:
	return hRetCode;
}

HRESULT KG3DModel::SetMaterialSubsetRenderTwiceAlphaOption(INT nSubsetIndex,
														   BOOL bEnable,
														   DWORD dwTopRef,
														   DWORD dwBottomRef)
{
	HRESULT hRetCode = E_FAIL;
	_MtlRenderTwiceAlphaOption* pAlpha = NULL;
	KG3DMaterial::KG3DMaterialSubset* pSubMat = GetSubMat(this, nSubsetIndex);
	KG_PROCESS_ERROR(pSubMat);

	pAlpha = pSubMat->GetAlphaRenderTwice();
	KG_PROCESS_ERROR(pAlpha);

	pAlpha->bEnable = bEnable;
	pAlpha->dwAlphaRefPass0 = dwTopRef;
	pAlpha->dwAlphaRefPass1 = dwBottomRef;
	hRetCode = S_OK;
Exit0:
	return hRetCode;
}

HRESULT KG3DModel::CopyMaterialOption(DWORD dwType, DWORD dwExtInfo, unsigned int nSrc, unsigned int nDest)
{
	KG3DMaterial* pMaterial = GetMaterialFromModel(this);
	
	if(pMaterial)
		pMaterial->CopyOption(dwType, dwExtInfo, nSrc, nDest);

	return S_OK;
}

HRESULT KG3DModel::GetMaterialName(LPTSTR lpName, INT nBufLen)
{
	_ASSERTE(lpName && nBufLen <= MAX_PATH);
	KG_PROCESS_ERROR(lpName && nBufLen <= MAX_PATH);
	
	KG3DMaterial* pMaterial = GetMaterialFromModel(this);
    KG_PROCESS_ERROR(pMaterial);
	_ASSERTE(nBufLen > (INT)pMaterial->m_scName.length());
	KG_PROCESS_ERROR(nBufLen > (INT)pMaterial->m_scName.length());
	
	if (pMaterial)
	{
		lstrcpy(lpName, pMaterial->m_scName.c_str());
		return S_OK;
	}
	else
		return E_FAIL;
Exit0:
	return E_FAIL;
}

HRESULT KG3DModel::SaveMaterialToFile(LPCTSTR lpFileName, unsigned uOption)
{
	_ASSERTE(m_lpMaterial);
	HRESULT hr = E_FAIL;
	if(m_lpMaterial && m_lpMaterial->IsLoaded())
		hr = m_lpMaterial->SaveToFile(lpFileName, uOption);
	return hr;
}

////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DModel::GetMatWorld(D3DXMATRIX **ppMatWorld)
{
    HRESULT hResult = E_FAIL;

    KGLOG_PROCESS_ERROR(ppMatWorld);
    
    *ppMatWorld = &m_matWorld;

    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT KG3DModel::GetName(LPCTSTR *ppszName)
{
    HRESULT hResult = E_FAIL;

    KGLOG_PROCESS_ERROR(ppszName);

    *ppszName = m_scName.c_str();

    hResult = S_OK;
Exit0:
    return hResult;

}
/*
HRESULT KG3DModel::GetFullName(LPCTSTR *ppszFullName)
{
    HRESULT hResult = E_FAIL;

    KGLOG_PROCESS_ERROR(ppszFullName);

    *ppszFullName = m_scFullName.c_str();

    hResult = S_OK;
Exit0:
    return hResult;
}*/


HRESULT KG3DModel::GetType(DWORD *pdwType) 
{   
    HRESULT hResult = E_FAIL;

    KGLOG_PROCESS_ERROR(pdwType);

    *pdwType = m_dwType;
    
    hResult = S_OK;
Exit0:
    return hResult;
}
HRESULT KG3DModel::GetIEKG3DAnimationWarper(IEKG3DAnimationWarper **ppAniWarper)
{
    HRESULT hResult = E_FAIL;

    KGLOG_PROCESS_ERROR(ppAniWarper);

    *ppAniWarper = m_Splitter.GetAnimationWarper();

    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT KG3DModel::GetAnimationTagContainer(IEKG3DAnimationTagContainer** pContainer)
{
    HRESULT hr = E_FAIL;
    KG_PROCESS_ERROR(pContainer);
	if (!m_pTaggedAnimationContainer[ANICTL_PRIMARY])
	{
		m_pTaggedAnimationContainer[ANICTL_PRIMARY] = new KG3DAnimationTagContainer;
		KG_PROCESS_ERROR(m_pTaggedAnimationContainer[ANICTL_PRIMARY]);

		m_pTaggedAnimationContainer[ANICTL_PRIMARY]->AttachToModel(this);
	}

	*pContainer = dynamic_cast<IEKG3DAnimationTagContainer*>(m_pTaggedAnimationContainer[ANICTL_PRIMARY]);
    KG_PROCESS_ERROR(*pContainer);
    hr = S_OK;
Exit0:
    return hr;
}

HRESULT KG3DModel::GetIEKG3DMeshBone(IEKG3DMeshBone **ppMeshBone)
{
    HRESULT hResult = E_FAIL;

    KGLOG_PROCESS_ERROR(ppMeshBone);

    *ppMeshBone = &m_boneMash;

    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT KG3DModel::IEGetAnimationController(IKG3DAnimationController **ppiAniController)
{
    HRESULT hResult = E_FAIL;

    KGLOG_PROCESS_ERROR(ppiAniController);

    *ppiAniController = GetAnimationController(ANICTL_PRIMARY);

    hResult = S_OK;
Exit0:
    return hResult;  
}
HRESULT KG3DModel::GetFirstChildModel(IEKG3DModel **ppModel)
{
    HRESULT hResult = E_FAIL;
    IEKG3DModel *piModel = NULL;
    
    KGLOG_PROCESS_ERROR(ppModel);

    m_itChildModel = m_ChildModels.begin();
    if (m_itChildModel != m_ChildModels.end())
    {
		KG3DModelLod& Lod = *m_itChildModel;
        piModel = Lod.pModelCurrent;
        ++m_itChildModel;
    }

    *ppModel = piModel;
    hResult = S_OK;
Exit0:
    return hResult;
}
HRESULT KG3DModel::GetNextChildModel(IEKG3DModel **ppModel)
{
    HRESULT hResult = E_FAIL;
    IEKG3DModel *piModel = NULL;
	KGLOG_PROCESS_ERROR(ppModel);
    *ppModel = NULL;

    if (m_itChildModel != m_ChildModels.end())
    {
		KG3DModelLod& Lod = *m_itChildModel;

        piModel = Lod.pModelCurrent;
        ++m_itChildModel;
    }

    *ppModel = piModel;
    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT KG3DModel::GetTransformationMatrix(D3DXMATRIX *pMatrix)
{
    HRESULT hResult = E_FAIL;

    KGLOG_PROCESS_ERROR(pMatrix);

    *pMatrix = m_Matrix;

    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT KG3DModel::SetTransformationMatrix(D3DXMATRIX *pMatrix)
{
    m_Matrix = *pMatrix;
    ASSERT_FLOAT_IS_VALIED(m_Matrix._11);

    return S_OK;
}

HRESULT KG3DModel::IEReSet()
{
    KG3DTransformation::ReSet();
    return S_OK;
}

void KG3DModel::GetRenderEnviromentParamenter(IKG3DScene* pScene)
{
	KG_PROCESS_ERROR(pScene);
	switch(pScene->GetType())
	{
	case SCENETYPE_SCENEEDITOR:
		{
			KG3DSceneSceneEditor* pEditor = static_cast<KG3DSceneSceneEditor*>(pScene);
			KG3DSceneSceneEditor::EnviromentParameter EnvPara;
			D3DXVECTOR3 vec3Pos(m_matWorld._41, m_matWorld._42, m_matWorld._43);
			std::vector<KG3DRuntimeMaterial*> RunTimeMaterial;
			KG_PROCESS_ERROR(pEditor);
			pEditor->GetEnviromentParamenter(vec3Pos, EnvPara);
			m_RuntimeMaterial.SetDiffuse(EnvPara.Diffuse);
			m_RuntimeMaterial.SetLightDiffuse(0, EnvPara.LightDiffuse);
			m_RuntimeMaterial.SetLightAmbient(EnvPara.LightAmbient);
			UpdateChildRuntimMaterial();
		}
		break;
	default:
		;
	}
Exit0:
	;
}

KG3DRuntimeMaterial* KG3DModel::GetRuntimeMaterial()
{
	return &m_RuntimeMaterial;
}

int KG3DModel::GetNumModel()
{
	return 0;
}

BOOL KG3DModel::IsNeedCheckResourceReady()
{
	return m_bNeedCheckResourceReady;
}

BOOL KG3DModel::IsResourceReady(BOOL bTestTexture)
{
	BOOL bReturn = IsLoaded();
	
	if (m_lpMaterial)
	{
		bReturn &= m_lpMaterial->IsResourceReady(bTestTexture);

		if(m_bClientAdd && m_nDetailDefineIndex>0)
		{
			if(m_lpDetail)
				bReturn &= m_lpDetail->IsResourceReady();
			/*else
				bReturn = FALSE;*/
		}
	}
	else
		bReturn = FALSE;

	for (std::set<KG3DModelLod>::iterator i = m_ChildModels.begin(); 
		i != m_ChildModels.end(); 
		i++)
	{
		KG3DModelLod& Lod = *i;

		KG3DModel* pChild = Lod.pModelCurrent;

		if(pChild->IsNeedCheckResourceReady())
		{
			bReturn &= pChild->IsResourceReady(bTestTexture);
		}
	}
	return bReturn;
}

HRESULT KG3DModel::PlayTaggedAnimation(LPCSTR strFileName, 
									   float fSpeed, 
									   DWORD dwType, 
									   enuAnimationControllerPriority Priority)
{
    HRESULT hr = E_FAIL;
	SAFE_RELEASE(m_pTaggedAnimationContainer[Priority]);
	
    m_pTaggedAnimationContainer[Priority] = g_tagConTable.LoadTag(strFileName);// g_cClipTable.GetTagTable()->LoadTag(strFileName);
    ASSERT(m_pTaggedAnimationContainer[Priority]);

    m_pTaggedAnimationContainer[Priority]->AttachToModel(this);
    
    hr = m_pTaggedAnimationContainer[Priority]->Start(fSpeed, 0, dwType, Priority);
    KG_COM_PROCESS_ERROR(hr);

    hr = S_OK;
Exit0:
    return hr;
}

IKG3DAnimationController* KG3DModel::GetSplitAnimationContorller(DWORD dwPartIndex, DWORD dwControllerIndex)
{
    HRESULT hr = E_FAIL;
    IKG3DAnimationController *pReturn = NULL;
    KG3DAnimationComposer* pComposer = NULL;
    KG3DAnimationWarper *pWarper = m_Splitter.GetAnimationWarper();
    KGLOG_PROCESS_ERROR(pWarper);
    KG_PROCESS_ERROR(pWarper->GetEnable());
    hr = pWarper->GetComposer(dwPartIndex, (IEKG3DAnimationComposer**)&pComposer);
    KGLOG_COM_PROCESS_ERROR(hr);
    pReturn = pComposer->GetAnimationController(dwControllerIndex);
Exit0:
    return pReturn;
}

DWORD KG3DModel::GetShaderType()
{
	if(m_lpMesh)
	{
		if ( m_lpMesh->IsSkin())
		{
			return KG3DMaterial::MAT_SHADER_TYPE_SKIN;
		}
	}
	if(m_lpMaterial && m_lpMaterial->IsLoaded())
	{
		return m_lpMaterial->GetShaderType();
	}
	return KG3DMaterial::MAT_SHADER_TYPE_NONE;
}
/////////////////////////////////////////////////////////////////////////////////////////

HRESULT KG3DModel::Shader_RenderSkinMeshDepth_Begin()
{
	HRESULT hRetCode = E_FAIL;

	KG_PROCESS_ERROR(m_lpMesh);	
	m_lpMesh->Shader_RenderSkinMeshDepth_Begin();

	hRetCode = S_OK;
Exit0:
	return hRetCode;
}
HRESULT KG3DModel::Shader_RenderSkinMeshDepth_NoChange()
{
	HRESULT hResult  = E_FAIL;
	if(!IsLoaded())
		return S_OK;

	PrepareRender(0);

	SetMatrix();

	if(m_lpMesh)
	{
		//KG_COM_PROCESS_ERROR(m_lpMesh->CheckLoadingState());

		m_lpMesh->SetLOD(m_fLOD);

		//if (!m_bMeshPostLoadProcessed && pMesh->m_eDynamicLoadState == KG3DMesh::STATE_VIDEOMEM)
		//	MeshPostLoadProcess();

		LPD3DXMESH pMeshToRender = NULL;
		D3DXMATRIX* pRenderMatrices = NULL;
		DWORD dwOption = 0;

		if(m_lpParentModelST)
		{
			pRenderMatrices = m_lpParentModelST->GetRenderMatrix();
			dwOption |= RENDER_BIP;
		}

		if (!pRenderMatrices)
		{
			MeshRenderExtraInfo Info;
			//
			if (m_AnimationController.HasAnimation())
			{
				if (!m_AnimationController.GetUpateAnimationInfo().pMesh)
				{
					Info.dwType = ENUM_MESHRENDER_EXTR_INFO_MATRIX;			
					Info.Data = m_pBoneMatricesForRender;
				}
			}
			else
			{
				Info.dwType = ENUM_MESHRENDER_EXTR_INFO_MATRIX;			
				Info.Data = m_pBoneMatricesForRender;
			}
			m_lpMesh->GetRenderMatrix(pMeshToRender, pRenderMatrices, &Info);
		}

		if (m_fIlluminationLevel > 1.0f)
		{
			dwOption |= RENDER_HIGHTLIGHT;
		}
		m_lpMesh->Shader_RenderSkinMeshDepth_NoChange(pRenderMatrices,m_lpMaterial,dwOption);
		m_lpMesh->SetModel(NULL);
	}
	hResult = S_OK;
	//Exit0:
	RestoreMatrix();

	PostRender();

	return hResult;
}
HRESULT KG3DModel::Shader_RenderSkinMeshDepth_End()
{
	if(m_lpMesh)
	{
		m_lpMesh->Shader_RenderSkinMeshDepth_End();
	}
	return S_OK;
}

//////////////////////////////////////////////////////////////////////////////////////////
//HRESULT	KG3DModel::Shader_DrawMeshAmbientMap_End()
//{
//	if(m_lpMesh)
//	{
//		m_lpMesh->Shader_DrawMeshAmbientMap_End(m_lpMesh->m_ppMeshes[KG3DMesh::SMBT_OPTIMIZED],m_fAlpha,0);
//	}
//	return S_OK;
//}

HRESULT KG3DModel::Shader_RenderSkinMesh_Begin(BOOL bUseConverTexture)
{
    HRESULT hRetCode = E_FAIL;
	
	KG_PROCESS_ERROR(m_lpMesh);	

	m_lpMesh->Shader_RenderSkinMesh_Begin(bUseConverTexture,IsClientAdd());
	
	hRetCode = S_OK;
Exit0:
	return hRetCode;
}

HRESULT KG3DModel::Shader_RenderSkinMesh_NoChange()
{
	HRESULT hResult  = E_FAIL;
	if(!IsLoaded())
		return S_OK;

	PrepareRender(0);

	SetMatrix();

	if(m_lpMesh)
	{
		//KG_COM_PROCESS_ERROR(m_lpMesh->CheckLoadingState());

		m_lpMesh->SetLOD(m_fLOD);

		//if (!m_bMeshPostLoadProcessed && pMesh->m_eDynamicLoadState == KG3DMesh::STATE_VIDEOMEM)
		//	MeshPostLoadProcess();

		LPD3DXMESH pMeshToRender = NULL;
		D3DXMATRIX* pRenderMatrices = NULL;
		DWORD dwOption = 0;

		if(m_lpParentModelST)
		{
			pRenderMatrices = m_lpParentModelST->GetRenderMatrix();
			dwOption |= RENDER_BIP;
		}

		if (!pRenderMatrices)
		{
			MeshRenderExtraInfo Info;
			//
			if (m_AnimationController.HasAnimation())
			{
				if (!m_AnimationController.GetUpateAnimationInfo().pMesh)
				{
					Info.dwType = ENUM_MESHRENDER_EXTR_INFO_MATRIX;			
					Info.Data = m_pBoneMatricesForRender;
				}
			}
			else
			{
				Info.dwType = ENUM_MESHRENDER_EXTR_INFO_MATRIX;			
				Info.Data = m_pBoneMatricesForRender;
			}
			m_lpMesh->GetRenderMatrix(pMeshToRender, pRenderMatrices, &Info);
		}

		if (m_fIlluminationLevel > 1.0f)
		{
			dwOption |= RENDER_HIGHTLIGHT;
		}
		m_lpMesh->Shader_RenderSkinMesh_NoChange(pRenderMatrices,m_lpMaterial,m_fAlpha,dwOption,IsClientAdd());
		m_lpMesh->SetModel(NULL);
	}
    hResult = S_OK;
//Exit0:
	RestoreMatrix();

    PostRender();

	return hResult;
}

HRESULT KG3DModel::Shader_RenderSkinMesh_End()
{
	if(m_lpMesh)
	{
		m_lpMesh->Shader_RenderSkinMesh_End();
	}

	return S_OK;
}

HRESULT KG3DModel::Shader_RenderSkinMesh_SingleSubset()
{
    HRESULT hResult  = E_FAIL;
    if(!IsLoaded())
        return S_OK;

    PrepareRender(0);

    SetMatrix();

    if(m_lpMesh)
    {
        m_lpMesh->SetLOD(m_fLOD);

        LPD3DXMESH pMeshToRender = NULL;
        D3DXMATRIX* pRenderMatrices = NULL;
        DWORD dwOption = 0;

        if(m_lpParentModelST)
        {
            pRenderMatrices = m_lpParentModelST->GetRenderMatrix();
            dwOption |= RENDER_BIP;
        }

        if (!pRenderMatrices)
        {
            MeshRenderExtraInfo Info;
			
            if (m_AnimationController.HasAnimation())
            {
                if (!m_AnimationController.GetUpateAnimationInfo().pMesh)
                {
                    Info.dwType = ENUM_MESHRENDER_EXTR_INFO_MATRIX;			
                    Info.Data = m_pBoneMatricesForRender;
                }
            }
            else
            {
                Info.dwType = ENUM_MESHRENDER_EXTR_INFO_MATRIX;			
                Info.Data = m_pBoneMatricesForRender;
            }
            m_lpMesh->GetRenderMatrix(pMeshToRender, pRenderMatrices, &Info);
        }

        m_lpMesh->Shader_RenderSkinMesh_SingleSubset(pRenderMatrices, m_lpMaterial, dwOption, IsClientAdd());
        m_lpMesh->SetModel(NULL);
    }
    hResult = S_OK;
//Exit0:
    RestoreMatrix();

    PostRender();

    return hResult;
}

HRESULT KG3DModel::Shader_DrawMeshNormalNoSkin_Begin()
{
	return KG3DMesh::Shader_DrawMeshNormalNoSkin_Begin(/*m_lpMesh->m_ppMeshes[KG3DMesh::SMBT_OPTIMIZED],m_lpMaterial,m_fAlpha,0*/);
}

HRESULT KG3DModel::Shader_DrawMeshNormalNoSkin_End()
{
	return KG3DMesh::Shader_DrawMeshNormalNoSkin_End(/*m_lpMesh->m_ppMeshes[KG3DMesh::SMBT_OPTIMIZED],m_lpMaterial,m_fAlpha,0*/);
}

HRESULT KG3DModel::Shader_DrawMeshNormalNoSkin_NoChange()
{
	HRESULT hr = E_FAIL;
	KG_PROCESS_ERROR(m_lpMesh);

	PrepareRender(0);
	SetMatrix();

	hr = m_lpMesh->Shader_DrawMeshNormalNoSkin_NoChange(m_lpMesh->m_ppMeshes[KG3DMesh::SMBT_OPTIMIZED],m_lpMaterial,m_fAlpha,0);
	KG_COM_PROCESS_ERROR(hr);
	
	RestoreMatrix();
    PostRender();

	hr = S_OK;
Exit0:
	return hr;
}

HRESULT KG3DModel::OnLostDevice()
{
	return S_OK;
}

HRESULT KG3DModel::OnResetDevice()
{
	return S_OK;
}

void KG3DModel::SinkFunc(INT nSinkID)
{
	if(nSinkID == KG3DEVENT_MESH_LOADED)
	{
        m_BBoxLocal.Clear();
        ComputeBBox();
        m_bBBoxChanged = TRUE;

		FireEvent(KG3DEVENT_MESH_LOADED);
	}
}

void KG3DModel::SinkFunc2(INT nSinkID,INT nsubSinkID,DWORD dwValue,PVOID pPoint)
{
	;
}
int KG3DModel::GetBindToSocketModel(const char cszSocketName[],vector<IEKG3DModel*> &vecpModel)
{
	int nCount = 0;

	std::set<KG3DModelLod>::iterator i = m_ChildModels.begin();
	while (i != m_ChildModels.end())
	{
		KG3DModelLod& Lod = *i;

		std::string SocketName;
		Lod.pModelCurrent->GetBindBoneName(SocketName);
		if(stricmp(SocketName.c_str(),cszSocketName) == 0)
		{
			vecpModel.push_back(Lod.pModelCurrent);
			nCount++;
		}
		i++;
	}	

	return nCount;
	
}

int KG3DModel::GetBindToSocketModel(const char cszSocketName[],vector<IKG3DModel*> &vecpModel)
{
	int nCount = 0;

	std::set<KG3DModelLod>::iterator i = m_ChildModels.begin();
	while (i != m_ChildModels.end())
	{
		KG3DModelLod& Lod = *i;
		std::string SocketName;
		Lod.pModelCurrent->GetBindBoneName(SocketName);
		if(stricmp(SocketName.c_str(),cszSocketName) == 0)
		{
			vecpModel.push_back(Lod.pModelCurrent);
			nCount++;
		}
		i++;
	}	

	return nCount;
}

BOOL KG3DModel::IsLoaded()
{
	BOOL bMesh = m_bMeshPostLoadProcessed && !m_bInMultiThreadLoadList;
	/*BOOL bMtl = m_lpMaterial && m_lpMaterial->IsLoaded();
	if(!m_lpMaterial)
		bMtl = TRUE;*/

	return  bMesh;
}

BOOL KG3DModel::IsVisible() const
{
    return m_bVisible;
}

void KG3DModel::GetBindToOrgModel(set<IKG3DModel*> &set_OrgModel)
{
	for (std::set<KG3DModel*>::iterator i = m_BindedOrgModel.begin(); i != m_BindedOrgModel.end(); i++)
		set_OrgModel.insert(static_cast<IKG3DModel*>(*i));
}

void KG3DModel::OnBindOrgModel(KG3DModel* pModel)
{
	m_BindedOrgModel.insert(pModel);
}

int KG3DModel::AttachModel(IKG3DModel *pModel)
{
	return 0;
}

void KG3DModel::DetachModel(IKG3DModel* pModel)
{
	return;
}

/*
HRESULT KG3DModel::EnterSceneRegion(void* pSceneRegion)
{
	if (m_lpSceneRegionOn != pSceneRegion)
	{
		KG3DSceneRegion* pRegion_1 = static_cast<KG3DSceneRegion*>(m_lpSceneRegionOn);
		KG3DSceneRegion* pRegion_2 = static_cast<KG3DSceneRegion*>(pSceneRegion);

		if(pRegion_1)
			pRegion_1->setModel.erase(this);
		if(pRegion_2)
			pRegion_2->setModel.insert(this);
		m_lpSceneRegionOn = pSceneRegion;
	}
	return S_OK;
}*/

HRESULT KG3DModel::OnEnterPvs(KG3DRepresentObjectPVS* pvs)
{
    if (m_pPvsOn != pvs)
    {
        if (m_pPvsOn)
            m_pPvsOn->RemoveModel(this);
        m_pPvsOn = pvs;
    }
    return S_OK;
}

HRESULT KG3DModel::LeavePvs(BOOL bRemoveFromPVS)
{
	if(bRemoveFromPVS)
	{
		if (m_pPvsOn)
			m_pPvsOn->RemoveModel(this);
	}
	m_pPvsOn = NULL;

    return S_OK;
}

KG3DBsp* KG3DModel::GetBsp()
{
    if (m_lpMesh)
        return m_lpMesh->GetBsp();
    return NULL;
}

HRESULT KG3DModel::OnEnterSceneRegion(KG3DSceneRegion& Region)
{
	if (m_lpSceneRegionOn != &Region)
	{
		if(NULL != m_lpSceneRegionOn)
			static_cast<KG3DSceneRegion*>(m_lpSceneRegionOn)->m_Data.EraseModel(this);
		m_lpSceneRegionOn = &Region;

		//Region执行完Add之后会调用这个函数。
	}
	return S_OK;
}

HRESULT KG3DModel::LeaveSceneRegion(BOOL bErase)
{
	if (m_lpSceneRegionOn)
	{
		KG3DSceneRegion* pRegion_1 = static_cast<KG3DSceneRegion*>(m_lpSceneRegionOn);
		
		if(bErase)
			pRegion_1->m_Data.EraseModel(this);

		m_lpSceneRegionOn = NULL;
	}
	return S_OK;
}

HRESULT KG3DModel::ComputeMeshBigBox()
{
	vector<BBox>boneBoxInfo;
	KG_PROCESS_ERROR(m_lpMesh);
	m_lpMesh->CreateBBoxOfBone();
	m_lpMesh->GetAllBoneBox(boneBoxInfo);
	m_BoneBoxInfo = boneBoxInfo;
Exit0:
	return S_OK;;
}

void KG3DModel::SetbDrawBigBox(BOOL val)
{
	m_bDrawBigBox = val;
	/*KG_PROCESS_ERROR(m_lpMesh);
	static_cast<KG3DMesh*>(m_lpMesh)->SetbDrawBigBox(val);*/
//	return ;
}

BOOL KG3DModel::GetbDrawBigBox()
{
	return m_bDrawBigBox;
}

HRESULT KG3DModel::DrawBoneBBox()
{
	HRESULT hr= E_FAIL;
	D3DXMATRIX* pMatices = m_pBoneMatricesForRender;
	KG_PROCESS_ERROR(m_lpMesh);

	if (m_lpParentModelST)
	{
		pMatices = m_lpParentModelST->GetRenderMatrix();
	}
	
	if (m_lpMesh->IsSkin() && m_bDrawBigBox && m_lpMesh->GetType() == MESHTYPE_DEFAULT)
	{
		D3DXMATRIX matSave;
		g_pd3dDevice->GetTransform(D3DTS_WORLD, &matSave);
		D3DXMATRIX Mat = m_Matrix*matSave;
		g_pd3dDevice->SetTransform(D3DTS_WORLD,&m_matWorld);
		if (g_bOnlyShowPitchOn)
		{
			if(m_lpMesh->m_dwCurSel < m_BoneBoxInfo.size())
				g_cGraphicsTool.DrawAABBox(&m_BoneBoxInfo[m_lpMesh->m_dwCurSel],
				0xff00DDff,
				&(m_lpMesh->m_pmatReverseMatrix[m_lpMesh->m_dwCurSel] * m_pBoneMatricesForRender[m_lpMesh->m_dwCurSel]),
				TRUE);
		}
		else
		{
			D3DCOLOR color;
			if (!m_Mesh2Bip.size())
			{
				for (DWORD i = 0 ;i < m_BoneBoxInfo.size() ; i++ )
				{
					color = 0xffcc1212;
					if (m_lpMesh->m_dwCurSel == i)
					{
						color = 0xff00DDff;
					}
					if (m_dwPicthOn == i)
					{
						color = 0xffedff00;
					}
					g_cGraphicsTool.DrawAABBox(&m_BoneBoxInfo[i],color, &(m_lpMesh->m_pmatReverseMatrix[i] * m_pBoneMatricesForRender[i]),TRUE);
				}
			}
			else
			{
				for (DWORD i = 0 ;i < m_BoneBoxInfo.size() ; i++ )
				{
					color = 0xffcc1212;
					if (m_lpMesh->m_dwCurSel == i)
					{
						color = 0xff00DDff;
					}
					if (m_dwPicthOn == i)
					{
						color = 0xffedff00;
					}
					DWORD dwBoneIndex = m_Mesh2Bip[i];
					if (dwBoneIndex == -1)
					{
						g_cGraphicsTool.DrawAABBox(&m_BoneBoxInfo[i],color, &(m_lpMesh->m_pmatReverseMatrix[i] * m_pBoneMatricesForRender[i]),TRUE);
					}
					else
					{
						g_cGraphicsTool.DrawAABBox(&m_BoneBoxInfo[i],color, &(m_lpMesh->m_pmatReverseMatrix[i] * pMatices[dwBoneIndex]),TRUE);
					}
				}
			}
		}
		
		g_pd3dDevice->SetTransform(D3DTS_WORLD,&matSave);
	}

	hr = S_OK;
Exit0:
	return hr;
}

HRESULT KG3DModel::UpdateSelBoneBBox(KG3DScene* pScene ,D3DXMATRIX* matWorld)
{
	HRESULT hResult  = E_FAIL;
	HRESULT hRetCode = E_FAIL;
	IEKG3DSceneOutputWnd *piSceneCurOutputWnd = NULL;

	D3DXMATRIX  mat,matWorldInv;
	D3DXVECTOR3 vOrg,A;
	D3DXVECTOR3 vDir,B,NewDir;
	D3DXVECTOR3 Inter;

	KG_PROCESS_ERROR(pScene);

	hRetCode = pScene->GetCurOutputWnd(&piSceneCurOutputWnd);
	KGLOG_COM_PROCESS_ERROR(hRetCode);
	KG_PROCESS_ERROR(piSceneCurOutputWnd);

	piSceneCurOutputWnd->GetPickRay(&vOrg, &vDir, NULL);
	BOOL bHasPichtOn = FALSE;
	A = vOrg;
	B = A + vDir;
	D3DXMatrixInverse(&matWorldInv,NULL,&m_matWorld);
	D3DXVec3TransformCoord(&A,&A,&matWorldInv);
	D3DXVec3TransformCoord(&B,&B,&matWorldInv);

	NewDir = B - A;
	D3DXVec3Normalize(&NewDir,&NewDir);
	for (DWORD i=0; i< m_BoneBoxInfo.size(); i++)
	{		
	
		
		if( m_BoneBoxInfo[i].IsRayIntersect3D(Inter,A,NewDir,&matWorld[i]))
		{
			m_dwPicthOn = i;
			bHasPichtOn = TRUE;

		}
	}
	if (!bHasPichtOn)
	{
		m_dwPicthOn = (DWORD)-1;
	}
	hResult = S_OK;
Exit0:
	return hResult;
}

HRESULT KG3DModel::SaveMeshToFile(LPCTSTR lpFileName, unsigned uOption)
{
	_ASSERTE(m_lpMesh);
	HRESULT hr = E_FAIL;
	if(m_lpMesh)
		hr =  m_lpMesh->SaveToFile(lpFileName, uOption);
	return hr;
}

BOOL KG3DModel::IsAnimationed()
{
	int nResult = false;
    int nRetCode = false;
    KG3DAnimationWarper *pAnimationWarper = NULL;
    IKG3DAnimationController *piController = NULL;

	KG_PROCESS_SUCCESS(m_piFlexibleBody && m_lpPhysicsScene);
    
    pAnimationWarper = m_Splitter.GetAnimationWarper();
    if (pAnimationWarper)
	{
		nRetCode = pAnimationWarper->GetEnable();
        KG_PROCESS_SUCCESS(nRetCode);
	}

	piController = GetAnimationController(ANICTL_PRIMARY);
    if (piController)
    {
        nRetCode = piController->HasAnimation();
        KG_PROCESS_SUCCESS(nRetCode);
    }

	if (m_lpParentModelST)
    {
       nRetCode = m_lpParentModelST->IsAnimationed();
       KG_PROCESS_SUCCESS(nRetCode);
    }
    

    KG_PROCESS_ERROR(false);

Exit1:
    nResult = true;
Exit0:
    return nResult;
}

HRESULT KG3DModel::ComputeDynamicBBox()
{
	HRESULT hrResult  = E_FAIL;
    D3DXVECTOR3 BoxPos[8];
    vector<BBox>boneBoxInfo;
    BBox bBox;
    D3DXVECTOR3 vPos;

    KG_PROCESS_SUCCESS(!m_lpMesh);
    KG_PROCESS_SUCCESS(!IsLoaded());
    KG_PROCESS_SUCCESS(!m_lpMesh->IsSkin());
    KG_PROCESS_SUCCESS(m_lpMesh->GetType() != MESHTYPE_DEFAULT);
    KG_PROCESS_SUCCESS(!m_lpParentModelST);

    m_BBoxLocal.Clear();
    m_BBox.Clear();

    m_BoneBoxInfo.clear();
	m_lpMesh->GetAllBoneBox(m_BoneBoxInfo);

    if (m_lpParentModelST)
    {
        KG3DBip *pBip = m_lpParentModelST->GetBip();
        KGLOG_PROCESS_ERROR(pBip);

        int nBoneNum = (int)m_Mesh2Bip.size();

        for (int i = 0; i < nBoneNum; ++i)
        {
            if ((m_Mesh2Bip[i] == -1) || (m_BoneBoxInfo[i] == bBox))
                continue;

            const D3DXMATRIX& matMatrix = pBip->GetBoneObjMatrix(m_Mesh2Bip[i]);         

            m_BBoxLocal.AddPosition(
                m_BoneBoxInfo[i].A, 
                m_BoneBoxInfo[i].B, 
                &(m_lpMesh->m_pmatReverseMatrix[i] * matMatrix)
            );
        }

    }
    else
    {
	    for(DWORD Index = 0; Index < m_BoneBoxInfo.size();Index++)
	    {
		    if (bBox == m_BoneBoxInfo[Index])
		    {
			    continue;
		    }
		    m_BBoxLocal.AddPosition(m_BoneBoxInfo[Index].A,m_BoneBoxInfo[Index].B, &m_pBoneMatricesForRender[Index]);
	    }
    }

    m_BBoxLocal.TransformCoord(BoxPos, &m_matWorld);
    m_BBox.AddPosition(BoxPos,8);//bigBox	

Exit1:
	hrResult = S_OK;
Exit0:
	return hrResult;
}

void KG3DModel::SetRenderSpecialAlpha(BOOL bEnable)
{
	m_bRenderSpecialAlpha = bEnable; 
	std::set<KG3DModelLod>::iterator it = m_ChildModels.begin();
	while (it != m_ChildModels.end())
	{
		KG3DModelLod& Lod = *it;
		Lod.pModelCurrent->SetRenderSpecialAlpha(bEnable);
		it++;
	}
}

HRESULT KG3DModel::UpdateBoneBBoxInfo()
{
	if (m_lpMesh)
	{
		vector<BBox>boneBoxInfo;
		m_lpMesh->GetAllBoneBox(boneBoxInfo);
		m_BoneBoxInfo = boneBoxInfo;
	}
	return S_OK;
}

FLOAT KG3DModel::ComputeShadowScaleFactor( BBox& BBox )
{
	if (this->GetType() != MESHTYPE_MODELST)
	{
		return 0;	//非ModelST是不画影子的
	}

	FLOAT fLength = BBox.GetLength();
	FLOAT fWidth = BBox.GetWidth();
	FLOAT fScale = min(fLength, fWidth);
	fScale /= 2;		//这个Factor是当半径用的，所以要除以2
	fScale /= 0.618f;	//这个大小是经验值，用上面那个0.618（黄金分割）效果已经可以了，再大一点会清晰一些

	MT_LIMIT_TO_RANGE(fScale, 90, 0);

	

	return fScale;
}

HRESULT KG3DModel::CreateProgressMesh()
{
	HRESULT hr = E_FAIL;
	if(m_lpMesh)
	{
		if(m_lpMesh->GetType() == MESHTYPE_DEFAULT)
		{
			hr = m_lpMesh->CreateProgressMesh();
		}
	}

	return hr;
}

HRESULT KG3DModel::Shader_DrawMeshSubset_Begin(int nSubsetIndex,KG3DMaterial::KG3DMaterialSubset* pSubMtl,LPD3DXEFFECT pShader)
{
	if(m_lpMesh)
	{
		m_lpMesh->Shader_DrawMeshSubset_Begin(nSubsetIndex,pSubMtl,m_fAlpha,0,pShader,m_bClientAdd);
	}

	return S_OK;
}

HRESULT KG3DModel::RenderSubsetDepthOnly(int nSubsetIndex,KG3DMaterial::KG3DMaterialSubset* pSubMtl)
{
	if(!IsLoaded())
		return S_OK;

	PrepareRender(0);

	SetMatrix();

	g_pd3dDevice->SetTexture(0,NULL);

	if(m_lpMesh)
	{
		LPD3DXMESH pRenderMesh = m_lpMesh->m_ppMeshes[KG3DMesh::SMBT_OPTIMIZED];

		if( pRenderMesh)
		{
			m_lpMesh->Shader_DrawMeshSubset_DepthOnly(pRenderMesh,nSubsetIndex,pSubMtl);
		}
	}

	RestoreMatrix();
	PostRender();
	return S_OK;
}

LPD3DXMESH KG3DModel::GetRenderXMesh()
{
	if(!IsLoaded())
		return NULL;

	LPD3DXMESH pRenderMesh = NULL;

	if(m_lpMesh)
	{
		pRenderMesh = m_lpMesh->m_ppMeshes[KG3DMesh::SMBT_OPTIMIZED];
		if (m_AnimationController.HasAnimation())
		{
			if (m_AnimationController.GetUpateAnimationInfo().pMesh)
			{
				//顶点/UV动画
				/*Info.Data = m_AnimationController.GetUpateAnimationInfo().pMesh;
				Info.dwType = ENUM_MESHRENDER_EXTR_INFO_MESH;
				hr = m_lpMesh->Render(m_lpMaterial, m_fAlpha, uOption, &Info);*/
				pRenderMesh = m_AnimationController.GetUpateAnimationInfo().pMesh;
			}
		}
	}

	return pRenderMesh;
}

HRESULT KG3DModel::Shader_DrawMeshSubset_NoChange(int nSubsetIndex,KG3DMaterial::KG3DMaterialSubset* pSubMtl,LPD3DXEFFECT pShader)
{
	if(!IsLoaded())
		return S_OK;

	PrepareRender(0);

	SetMatrix();

	if(m_lpMesh)
	{
		LPD3DXMESH pRenderMesh = m_lpMesh->m_ppMeshes[KG3DMesh::SMBT_OPTIMIZED];
		if (m_AnimationController.HasAnimation())
		{
			if (m_AnimationController.GetUpateAnimationInfo().pMesh)
			{
				//顶点/UV动画
				/*Info.Data = m_AnimationController.GetUpateAnimationInfo().pMesh;
				Info.dwType = ENUM_MESHRENDER_EXTR_INFO_MESH;
				hr = m_lpMesh->Render(m_lpMaterial, m_fAlpha, uOption, &Info);*/
				pRenderMesh = m_AnimationController.GetUpateAnimationInfo().pMesh;
			}
		}

		if( pRenderMesh)
		{
			m_lpMesh->Shader_DrawMeshSubset_NoChange(pRenderMesh,nSubsetIndex,pSubMtl,m_fAlpha,0,pShader,m_bClientAdd);
		}
	}

	RestoreMatrix();

	PostRender();

	return S_OK;

}

HRESULT	KG3DModel::Shader_DrawMeshSubset_End(int nSubsetIndex,KG3DMaterial::KG3DMaterialSubset* pSubMtl,LPD3DXEFFECT pShader)
{
	if(m_lpMesh)
	{
		m_lpMesh->Shader_DrawMeshSubset_End(pSubMtl,pShader);
	}
	return S_OK;
}

HRESULT KG3DModel::GetBoneScale(DWORD dwIndex, 
								D3DXVECTOR3 *pScale)
{
	if(m_pBip)
	{
		if(dwIndex < m_Mesh2Bip.size())
		{
			int nIndex = m_Mesh2Bip[dwIndex];

			const TCHAR* pName = m_pBip->GetBoneName(nIndex);
			return m_pBip->GetBoneScale(pName,pScale);
		}
	}

	(*pScale) = D3DXVECTOR3(1,1,1);
	return S_OK;
}

HRESULT KG3DModel::SetBoneScale(DWORD dwIndex, 
								D3DXVECTOR3 *pScale)
{
	if(m_pBip)
	{
		if(dwIndex < m_Mesh2Bip.size())
		{
			int nIndex = m_Mesh2Bip[dwIndex];

			const TCHAR* pName = m_pBip->GetBoneName(nIndex);
			m_pBip->SetBoneScale(pName,*pScale);
			m_pBip->EnableBoneScale(TRUE);
		}
	}

	return S_OK;
}

HRESULT KG3DModel::RenderToPic(LPCSTR strOutputFile,
							   DWORD dwWidth,
							   DWORD dwHeight,
							   DWORD dwBackColor,
							   BOOL bUsedForMeshMdlThumbnail /* = FALSE */)
{
	HRESULT hRetCode = E_FAIL;
	HRESULT hResult  = E_FAIL;
	HRESULT hBegin   = E_FAIL;

	//float fLength = 0.0f;
	KG3DCameraPreset TestCamera;
	KG3DModel *pModelForceReload = NULL;

	hResult = g_cModelTable.LoadResourceFromFile(m_scName.c_str(),
		0,
		0,
		(IKG3DResourceBase**)(&pModelForceReload));
	KG_COM_PROCESS_ERROR(hResult);

	KG3DTexture* pTexture = g_cTextureTable.GetRenderTargetTexture(dwWidth, 
		dwHeight, 
		D3DFMT_A8R8G8B8);
	KG_PROCESS_ERROR(pTexture);

	TestCamera.GetPresetCamera(pModelForceReload, 
		KG3DCameraPreset::CPT_FRONT, 
		static_cast<float>(dwWidth) / dwHeight,
		static_cast<bool>(bUsedForMeshMdlThumbnail));
	TestCamera.SetCamera(KG3DCameraPreset::CPT_FRONT);

	hBegin = g_pd3dDevice->BeginScene();

	hResult = RenderToTexture(pTexture->GetID(),
		&D3DXVECTOR2(0, 0),
		&D3DXVECTOR2((float)dwWidth, (float)dwHeight),
		&D3DXVECTOR2(0.0f, 0.0f),
		&D3DXVECTOR2(0.0f, 0.0f),
		0.0f,
		D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
		dwBackColor,
		0,
		NULL);
	KG_COM_PROCESS_ERROR(hResult);

	hResult = pTexture->SaveToFile(strOutputFile, 0);
	KG_COM_PROCESS_ERROR(hResult);
	hRetCode = S_OK;

Exit0:
	if (SUCCEEDED(hBegin))
	{
		g_pd3dDevice->EndScene();
	}
	SAFE_RELEASE(pModelForceReload);
	return hRetCode;
}


HRESULT KG3DModel::RenderAnimationToPicSerial(LPCSTR strAnimationName, 
											  LPCSTR strSerialName,
											  DWORD dwWidth,
											  DWORD dwHeight,
											  DWORD dwBackColor)
{
	HRESULT hRetCode = E_FAIL;
	HRESULT hResult  = E_FAIL;
	HRESULT hBegin = E_FAIL;
	KG3DClip *pClip = NULL;
	float fLength = 0.0f;
	KG3DCameraPreset TestCamera;
	KG3DModel *pModelForceReload = NULL;
	BOOL bSave = g_eEngineOption.bUseMultiThreadLoad;
	g_eEngineOption.bUseMultiThreadLoad = FALSE;
	
	if (!IsLoaded())
	{
		hResult = g_cModelTable.LoadResourceFromFile(m_scName.c_str(),
		0,
		0,
		(IKG3DResourceBase**)(&pModelForceReload));
		KG_COM_PROCESS_ERROR(hResult);
		SAFE_RELEASE(pModelForceReload);
	}

	_ASSERTE(IsLoaded());

	KG3DTexture* pTexture = g_cTextureTable.GetRenderTargetTexture(dwWidth, 
		dwHeight, 
		D3DFMT_A8R8G8B8);
	KG_PROCESS_ERROR(pTexture);

	DWORD dwId = pTexture->GetID();
	DWORD dwNumFrame = 0;
	
	hResult = g_cClipTable.LoadResourceFromFile(strAnimationName, 0, 0, &pClip);
	KG_COM_PROCESS_ERROR(hResult);

	hResult = PlayAnimation(ENU_ANIMATIONPLAY_ONCE, 
		strAnimationName,
		1.0f,
		0,
		NULL,
		NULL,
		ANICTL_PRIMARY);
	KG_COM_PROCESS_ERROR(hResult);
	
	hResult = pClip->GetNumFrames(&dwNumFrame);
	KG_COM_PROCESS_ERROR(hResult);

	hResult = pClip->GetFrameLength(&fLength);
	KG_COM_PROCESS_ERROR(hResult);
	
	TestCamera.GetPresetCamera(this, 
		KG3DCameraPreset::CPT_FRONT, 
		static_cast<float>(dwWidth) / dwHeight);
	TestCamera.SetCamera(KG3DCameraPreset::CPT_FRONT);

	hBegin = g_pd3dDevice->BeginScene();

	const int nPicCount = 20;
	IKG3DAnimationController* pController = GetAnimationController(ANICTL_PRIMARY);
	int nCycleTime = pController->GetCycleTime();
	for (int i = 1; i <= nPicCount; i++)
	{
		TCHAR strFileName[MAX_PATH];
		pController->SeekAnimation(ANIMATION_SEEK_SET,  nCycleTime / nPicCount * i);
		FrameMove();
		RenderToTexture(dwId, 
			&D3DXVECTOR2(0, 0),
			&D3DXVECTOR2(200.0f, 200.0f),
			&D3DXVECTOR2(0.0f, 0.0f),
			&D3DXVECTOR2(0.0f, 0.0f),
			0.0f,
			D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
			dwBackColor,
			0,
			NULL);
		sprintf_s(strFileName, MAX_PATH, "%s%03d.bmp", strSerialName, i);
		pTexture->SaveToFile(strFileName, 0);
	}
	hRetCode = S_OK;
Exit0:
	if (SUCCEEDED(hBegin))
	{
		g_pd3dDevice->EndScene();
	}
	SAFE_RELEASE(pClip);
	g_eEngineOption.bUseMultiThreadLoad = bSave;
	return hRetCode;
}

HRESULT KG3DModel::RefreshOption(unsigned uOption)
{
	return S_OK;
}

void KG3DModel::ResetTransform()
{
	BOOL bIsPassRecord = FALSE;
	if (g_cRecorder.IsRecord() && m_dwRecordID != 0  && !g_cRecorder.IsLock())
	{
		KG3DRecorder::Parameters MyParameters;
		g_cRecorder.RecordEvent(m_dwRecordID,FT_KG3DMODEL_RESETTRANSFORM,GetCurrentTime(),MyParameters);
		bIsPassRecord = TRUE;
		g_cRecorder.Lock();
	}
	KG3DTransformation::ReSet();
	KG3DTransformation::UpdateTransformation();
	if (bIsPassRecord)
		g_cRecorder.UnLock();
}

//BOOL KG3DModel::GetNearRayIntersect( const D3DXVECTOR3& vSrc, const D3DXVECTOR3& vNormalizedDir , FLOAT* pfRet ) /*只需要和线碰撞的最近点，其它忽略，由vSrc + vNormalizedDir * fRet就可以得到交点坐标 */
//{
//	return this->RayIntersection(vSrc, vNormalizedDir, pfRet);
//}

VOID KG3DModel::SetBillboardCloud( KG3DBillboardCloud* pCloud )
{
	SAFE_RELEASE(m_pBillboardCloud);
	m_pBillboardCloud = pCloud;
}

BOOL KG3DModel::IsSortAsSFX()
{
	if(m_lpMaterial && m_lpMaterial->IsLoaded())
		return m_lpMaterial->IsSortAsSFX();
	return FALSE;
}

void KG3DModel::OnSecondaryAnimationFinish(KG3DAnimationController *pController)
{
	if (m_Splitter.GetAnimationWarper()->GetEnable())
	{
		m_Splitter.OnSecondaryAnimationFinish(pController);
	}
	else
	{
		KG3DModelST *pModel = NULL;
		KG3DBip *pBip = NULL;
		//处理2号控制器播放完成之后的切回1号控制器
		KG_PROCESS_ERROR(GetType() == MESHTYPE_MODELST);

		pModel = dynamic_cast<KG3DModelST*>(this);

		pBip = pModel->GetBip();
		KG_PROCESS_ERROR(pBip);

		pBip->OnSecondaryAnimationFinish();
	}
Exit0:
	;
}

VOID KG3DModel::SetAttachType(MODELCLOSEGROUND_TYPE nType,IKG3DScene* pScene)
{
	m_AttachType = nType;
	m_pSceneToAttach = pScene;
}

HRESULT KG3DModel::AdjustToAttachGround()
{
	return S_OK;
}

void KG3DModel::SetClientAdd(BOOL bClient)
{
	m_bClientAdd = bClient;
}

BOOL KG3DModel::IsClientAdd()
{
	return m_bClientAdd;
}

HRESULT KG3DModel::RemoveTexture(DWORD dwSubsetIndex, 
								 DWORD dwIndex)
{
	HRESULT hRetCode = E_FAIL;
	HRESULT hResult  = E_FAIL;
	KG_PROCESS_ERROR(m_lpMaterial);

	hResult = m_lpMaterial->RemoveTexture(dwSubsetIndex, 
		dwIndex);
	KG_COM_PROCESS_ERROR(hResult);

	hRetCode = S_OK;
Exit0:
	return hRetCode;
}

HRESULT KG3DModel::ChangeTexture(DWORD dwSubsetIndex, 
								 DWORD dwIndex, 
								 LPCSTR strNewTextureFileName)
{
	HRESULT hRetCode = E_FAIL;
	HRESULT hResult  = E_FAIL;
	KG_PROCESS_ERROR(m_lpMaterial);

	hResult = m_lpMaterial->ChangeTexture(dwSubsetIndex,
		dwIndex,
		strNewTextureFileName);
	KG_COM_PROCESS_ERROR(hResult);

	hRetCode = S_OK;
Exit0:
	return hRetCode;
}

void KG3DModel::SetCaptionFloorHeight(float fHeight)
{
	m_fHeadLineHeight = fHeight;
}


HRESULT KG3DModel::SetCaptionVisible(DWORD dwIndex, BOOL bVisible)
{
	HRESULT hRetCode = E_FAIL;
	KG_PROCESS_ERROR(dwIndex < KG3DCaption2D::MAX_NUM_CAPTION + 1);
	KG_PROCESS_ERROR(m_dwHeadLineID[0] != CAPTION_INVALIDATE_INDEX);
	
	g_cCaption2D.SetVisible(m_dwHeadLineID[0], dwIndex, bVisible);
	hRetCode = S_OK;
Exit0:
	return hRetCode;

}

HRESULT KG3DModel::SetCaption(DWORD dwIndex, LPCSTR strContent)
{
	HRESULT hRetCode = E_FAIL;
	KG_PROCESS_ERROR(dwIndex < KG3DCaption2D::MAX_NUM_CAPTION);

	m_dwHeadLineID[0] = g_cCaption2D.Add(m_dwHeadLineID[0], dwIndex, strContent);
	KG_PROCESS_ERROR(m_dwHeadLineID[0] != CAPTION_INVALIDATE_INDEX);

	hRetCode = S_OK;
Exit0:
	return hRetCode;
}

HRESULT KG3DModel::SetCaptionColor(DWORD dwIndex, const D3DCOLORVALUE& Color)
{
	HRESULT hRetCode = E_FAIL;
	KG_PROCESS_ERROR(dwIndex < KG3DCaption2D::MAX_NUM_CAPTION);
	KG_PROCESS_ERROR(m_dwHeadLineID[0] != CAPTION_INVALIDATE_INDEX);

	g_cCaption2D.SetRenderData(m_dwHeadLineID[0], NULL, &Color, NULL, NULL);

	hRetCode = S_OK;
Exit0:
	return hRetCode;
}

HRESULT KG3DModel::SetPercentage(float fPercentage)
{
	HRESULT hRetCode = E_FAIL;
	KG_PROCESS_ERROR(m_dwHeadLineID[0] != CAPTION_INVALIDATE_INDEX);

	g_cCaption2D.SetRenderData(m_dwHeadLineID[0], NULL, NULL, &fPercentage, NULL);
	hRetCode = S_OK;
Exit0:
	return hRetCode;
}

HRESULT KG3DModel::SetPercentageVisible(BOOL bVisible)
{
	return SetCaptionVisible(KG3DCaption2D::MAX_NUM_CAPTION, bVisible);
}



HRESULT KG3DModel::SetPercentageStyle(const D3DCOLORVALUE &Color)
{
	HRESULT hRetCode = E_FAIL;
	KG_PROCESS_ERROR(m_dwHeadLineID[0] != CAPTION_INVALIDATE_INDEX);
	
	g_cCaption2D.SetRenderData(m_dwHeadLineID[0], NULL, NULL, NULL, &Color);
	hRetCode = S_OK;
Exit0:
	return hRetCode;
}


void KG3DModel::Helper_CaptionFrameMove()
{
	KG_PROCESS_ERROR(m_dwHeadLineID[0] != CAPTION_INVALIDATE_INDEX);
	{
		D3DXVECTOR3 vec3Pos;
		D3DXVECTOR3 vec3Local(0.0f, 0.0f, 0.0f);
		vec3Pos = D3DXVECTOR3(m_matWorld._41, m_matWorld._42 + m_fHeadLineHeight, 
			m_matWorld._43);
		g_cCaption2D.SetRenderData(m_dwHeadLineID[0], &vec3Pos, NULL, NULL, NULL);
	}
Exit0:
	;
}

HRESULT KG3DModel::EnableHideFromRenderTools(BOOL bEnable)
{
	m_bHideFromRenderTools = bEnable;

	for (std::set<KG3DModelLod>::iterator i = m_ChildModels.begin(); i != m_ChildModels.end(); i++)
	{
		KG3DModelLod& Lod = *i;

		KG3DModel* pSubModel = Lod.pModelCurrent;
		pSubModel->EnableHideFromRenderTools(bEnable);
	}
	return S_OK;
}


void KG3DModel::RenderForSelector(DWORD dwOption, void *pExtInfo)
{
	HRESULT hr = E_FAIL;
	MeshRenderExtraInfo Info;
	KG3DMesh *pMeshForRender = m_lpMesh;
	KG_PROCESS_ERROR(m_lpMesh);
	KG_PROCESS_ERROR(m_lpMaterial);
	SetMatrix();

	if (m_AnimationController.HasAnimation())
	{
		if (m_AnimationController.GetUpateAnimationInfo().pMesh)
		{
			//顶点/UV动画
			Info.Data = m_AnimationController.GetUpateAnimationInfo().pMesh;
			Info.dwType = ENUM_MESHRENDER_EXTR_INFO_MESH;
			pExtInfo = &Info;
		}
		else
		{
			//骨骼动画
			if (!pExtInfo)
			{
				Info.dwType = ENUM_MESHRENDER_EXTR_INFO_MATRIX;			
				Info.Data = m_pBoneMatricesForRender;
				pExtInfo = &Info;
			}
			else
			{
				dwOption |= RENDER_BIP;
			}
		}
	}
	else
	{
		//没有动画
		//如果有pExtInfo, 说明是modelset的一个部件
		//否则就是单个模型, 使用模型自己的Render矩阵
		if (!pExtInfo)
		{
			Info.dwType = ENUM_MESHRENDER_EXTR_INFO_MATRIX;			
			if (!m_lpParentModelST)
			{
				Info.Data = m_pBoneMatricesForRender;
			}
			else
			{
				Info.Data = m_lpParentModelST->GetRenderMatrix();
			}
			pExtInfo = &Info;
		}


	}

	_ASSERTE(pMeshForRender);
	pMeshForRender->SetModel(this);
	hr = pMeshForRender->RenderForSelector(m_lpMaterial, pExtInfo, dwOption);
	pMeshForRender->SetModel(NULL);

	RestoreMatrix();
	//RenderChildModels(0, NULL);
Exit0:
	;
}

void KG3DModel::SetSelectIndex(DWORD dwIndex)
{
	m_dwSelectIndex = dwIndex;
	std::set<KG3DModelLod>::iterator it = m_ChildModels.begin();
	std::set<KG3DModelLod>::iterator itEnd = m_ChildModels.end();
	while(it != itEnd)
	{
		KG3DModelLod& Lod = *it;

		if (Lod.pModelCurrent)
		{
			Lod.pModelCurrent->SetSelectIndex(dwIndex);
		}
		it++;
	}
}

DWORD KG3DModel::GetSelectIndex()
{
	return m_dwSelectIndex;
}

HRESULT KG3DModel::RestoreFace()
{
	HRESULT hResult = E_FAIL;
	KG3DAnimationController *pController = static_cast<KG3DAnimationController*>(GetAnimationController(ANICTL_PRIMARY));
	
	KG_PROCESS_ERROR(pController);
	
	pController->Attach(NULL);
	SetFaceMotionMode();

	hResult = S_OK;
Exit0:
	return hResult;
}

void KG3DModel::SetFaceMotionMode()
{
	g_DefaultFaceMotionMgr.SetDefaultFaceMotion(this);
}



HRESULT KG3DModel::_SetDetail(int nDefineTable,int nIndex)
{
	HRESULT hr = E_FAIL;

	if(m_lpModelMedium)
	{
		m_lpModelMedium->SetDetail(nDefineTable,nIndex);
	}
	if(m_lpModelLow)
	{
		m_lpModelLow->SetDetail(nDefineTable,nIndex);
	}

	m_nDetailTableIndex  = nDefineTable;
	m_nDetailDefineIndex = nIndex;

	/*if(nIndex == 0)
	{
		hr = EnableDetail(FALSE);
		KGLOG_COM_PROCESS_ERROR(hr);
	}
	else*/
	{
		hr = EnableDetail(TRUE);
		KGLOG_COM_PROCESS_ERROR(hr);

		KGLOG_PROCESS_ERROR(m_lpMaterial && m_lpMaterial->IsLoaded());
		KGLOG_PROCESS_ERROR(m_lpDetail);

		int nMtlIndex = nIndex;
		hr = m_lpMaterial->GetMaterilDetail(nMtlIndex,m_lpDetail);

		if(FAILED(hr))
		{
			TCHAR szName[MAX_PATH];
			TCHAR szDetail0[MAX_PATH];
			TCHAR szDetail1[MAX_PATH];
			TCHAR szFlowTexture[MAX_PATH];
			ZeroMemory(szDetail0,sizeof(szDetail0));
			ZeroMemory(szDetail1,sizeof(szDetail1));
			ZeroMemory(szName,sizeof(szName));
			ZeroMemory(szFlowTexture,sizeof(szFlowTexture));
				
			g_cTextureTable.Detail_GetBodyColor(nDefineTable,nMtlIndex,
					m_lpDetail->m_cMainColor,
					m_lpDetail->m_cMainColor1,
					m_lpDetail->m_fMainColorMax,
					m_lpDetail->m_fMainColorMin,
					m_lpDetail->m_fMainColorScale,
					m_lpDetail->m_fEnv,
					m_lpDetail->m_fSpecular,
					szName,
					m_lpDetail->m_cDetial0Color,
					m_lpDetail->m_fDetail0ColorScale,
					m_lpDetail->m_fDetail0UVScale,
					m_lpDetail->m_fDetail0Env,
					m_lpDetail->m_fDetail0Specular,
					szDetail0,
					m_lpDetail->m_cDetial1Color,
					m_lpDetail->m_fDetail1ColorScale,
					m_lpDetail->m_fDetail1UVScale,
					m_lpDetail->m_fDetail1Env,
					m_lpDetail->m_fDetail1Specular,
					szDetail1,
					&m_lpDetail->m_Flow_Color,
					&m_lpDetail->m_fFlow_ColorScale,
					&m_lpDetail->m_fFlow_UVScale,
					&m_lpDetail->m_vFlow_Speed,
					szFlowTexture
					);
			m_lpDetail->m_szDetail0FileName = szDetail0;
			m_lpDetail->m_szDetail1FileName = szDetail1;
			m_lpDetail->m_szFlow_Texture    = szFlowTexture;
		}
		m_lpDetail->LoadTextures();
	}

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DModel::SetDetail(int nDefineTable,int nIndex)
{

	if(IsLoaded() && m_lpMaterial && m_lpMaterial->IsLoaded())
	{
		return _SetDetail(nDefineTable,nIndex);
	}
	else
	{
		KG3DModelCommand_SetDetail* pCmd = new KG3DModelCommand_SetDetail(nDefineTable,nIndex);
		m_Commands.AddCommand(pCmd,FALSE);
	}

	return S_OK;
}

HRESULT KG3DModel::GetDetail(int* pnDefineTable,int* pnIndex)
{
	(*pnDefineTable) = m_nDetailTableIndex ;
	(*pnIndex)       = m_nDetailDefineIndex;

	return S_OK;
}

HRESULT KG3DModel::EnableDetail(BOOL bEnable)
{
	HRESULT hr = E_FAIL;
    if(bEnable)
	{
        TEXTURE_LOAD_STATE eLoadState = TEXTURE_LOAD_UNKNOWN_ERROR;
        KGLOG_PROCESS_ERROR(m_lpMaterial && m_lpMaterial->IsLoaded());

        TCHAR szTextureName[MAX_PATH];
        TCHAR szChannelName[MAX_PATH];

        hr = m_lpMaterial->GetTexturePathName(0, 0, szTextureName, MAX_PATH, &eLoadState);
        KG_COM_PROCESS_ERROR(hr);

        TCHAR strFile[MAX_PATH]; 
        TCHAR strDrive[MAX_PATH];
        TCHAR strPath[MAX_PATH];
        TCHAR strExt[MAX_PATH];
        _splitpath_s(szTextureName, 
            strDrive, 
            MAX_PATH,
            strPath, 
            MAX_PATH, 
            strFile, 
            MAX_PATH, 
            strExt,
            MAX_PATH);

        sprintf_s(szChannelName,
			MAX_PATH, 
			"%s%s%s_l%s",
			strDrive,
			strPath,
			strFile,
			strExt);
		
        if(!m_lpDetail)
        {
            m_lpDetail = new KG3DMaterialDetail;
            KGLOG_PROCESS_ERROR(m_lpDetail);
        }

        m_lpDetail->m_szChannelFileName = szChannelName;
		KG_PROCESS_SUCCESS(!g_IsFileExist(szChannelName));
        
		m_lpDetail->LoadTextures();
	}
	else
	{
        SAFE_DELETE(m_lpDetail);
	}
Exit1:
	hr = S_OK;
Exit0:
	return hr;
}



HRESULT KG3DModel::GetMaterilDetail(int nDetailType, int nSubSetID, LPSTR pszName, LPSTR pszTextureFileName, float* pfEnv,float* pfSpecular,
									   float* pBodyMax,float* pBodyMin,float* pBodyScale,D3DCOLORVALUE* pBodyColor,D3DCOLORVALUE* pBodyColor1,LPSTR pChannelFile,
									   float* pDetail0_ColorScale,float* pDetail0_UVScale,float* pfDetail0_Env,float* pfDetail0_Specular,D3DCOLORVALUE* pDetail0_Color,LPSTR pDetail0_File,
									   float* pDetail1_ColorScale,float* pDetail1_UVScale,float* pfDetail1_Env,float* pfDetail1_Specular,D3DCOLORVALUE* pDetail1_Color,LPSTR pDetail1_File,
									   D3DCOLORVALUE* pFlow_Color,float* pfFlow_ColorScale,float* pfFlow_UVScale,D3DXVECTOR2* pvSpeed,LPSTR pFlowTexture)
{
	HRESULT hr       = E_FAIL;
    HRESULT hRetCode = E_FAIL;
	INT     nCount   = 0;
	hr = EnableDetail(TRUE);
	KGLOG_COM_PROCESS_ERROR(hr);

	KGLOG_PROCESS_ERROR(m_lpDetail);
    KGLOG_PROCESS_ERROR(m_lpMaterial);
    
    hRetCode = GetNumMaterial(&nCount);
    KG_COM_PROCESS_ERROR(hRetCode);
    KG_PROCESS_ERROR(nSubSetID < nCount);

	if(nDetailType == -1)
	{
		if(m_lpDetail)
		{
            BOOL bWholeColorCast = m_lpMaterial->GetDetailWholeColorCast();
            KG3DMaterialDetail* pDetail = m_lpDetail->GetSubSetDetail(bWholeColorCast, nSubSetID);
            KG_PROCESS_ERROR(pDetail);

			(*pBodyMax)   = pDetail->m_fMainColorMax;
			(*pBodyMin)   = pDetail->m_fMainColorMin;
			(*pBodyScale) = pDetail->m_fMainColorScale;
			(*pBodyColor) = pDetail->m_cMainColor;
			(*pBodyColor1)= pDetail->m_cMainColor1;
			(*pfEnv)      = pDetail->m_fEnv;
			(*pfSpecular) = pDetail->m_fSpecular;
			wsprintf(pChannelFile,"%s",pDetail->m_szChannelFileName.c_str());

			(*pDetail0_ColorScale) = pDetail->m_fDetail0ColorScale;
			(*pDetail0_UVScale)    = pDetail->m_fDetail0UVScale;
			(*pDetail0_Color)      = pDetail->m_cDetial0Color;
			wsprintf(pDetail0_File,"%s",pDetail->m_szDetail0FileName.c_str());

			(*pDetail1_ColorScale) = pDetail->m_fDetail1ColorScale;
			(*pDetail1_UVScale)    = pDetail->m_fDetail1UVScale;
			(*pDetail1_Color)      = pDetail->m_cDetial1Color;
			wsprintf(pDetail1_File,"%s",pDetail->m_szDetail1FileName.c_str());

			(*pfDetail0_Env) = pDetail->m_fDetail0Env;
			(*pfDetail0_Specular) = pDetail->m_fDetail0Specular;
			(*pfDetail1_Env) = pDetail->m_fDetail1Env;
			(*pfDetail1_Specular) = pDetail->m_fDetail1Specular;

            wsprintf(pszTextureFileName, "%s", m_lpDetail->m_szTextrueFileName.c_str());

			(*pFlow_Color) = pDetail->m_Flow_Color;
			(*pfFlow_ColorScale) = pDetail->m_fFlow_ColorScale;
			(*pfFlow_UVScale) = pDetail->m_fFlow_UVScale;
			(*pvSpeed) = pDetail->m_vFlow_Speed;
			wsprintf(pFlowTexture, "%s", m_lpDetail->m_szFlow_Texture.c_str());


			return S_OK;
		}
	}
	else if(m_lpMaterial && m_lpMaterial->IsLoaded())
	{
		return m_lpMaterial->GetMaterilDetail(nDetailType, nSubSetID, pszName, pszTextureFileName, pfEnv,pfSpecular,
			pBodyMax,pBodyMin,pBodyScale,pBodyColor,pBodyColor1,pChannelFile,
			pDetail0_ColorScale,pDetail0_UVScale,pfDetail0_Env,pfDetail0_Specular,pDetail0_Color,pDetail0_File,
			pDetail1_ColorScale,pDetail1_UVScale,pfDetail1_Env,pfDetail1_Specular,pDetail1_Color,pDetail1_File,
			pFlow_Color,pfFlow_ColorScale,pfFlow_UVScale,pvSpeed,pFlowTexture);
	}

Exit0:
	return E_FAIL;
}

HRESULT KG3DModel::SetMaterilDetail(int nDetailType, int nSubSetID, LPSTR pszName, LPSTR pszTextureFileName, float fEnv,float fSpecular,
									   float fBodyMax,float fBodyMin,float fBodyScale,D3DCOLORVALUE vBodyColor,D3DCOLORVALUE vBodyColor1,LPSTR pChannelFile,
									   float fDetail0_ColorScale,float fDetail0_UVScale,float fDetail0_Env,float fDetail0_Specular,D3DCOLORVALUE vDetail0_Color,LPSTR pDetail0_File,
									   float fDetail1_ColorScale,float fDetail1_UVScale,float fDetail1_Env,float fDetail1_Specular,D3DCOLORVALUE vDetail1_Color,LPSTR pDetail1_File,
									   D3DCOLORVALUE Flow_Color,float fFlow_ColorScale,float fFlow_UVScale,D3DXVECTOR2 vSpeed,LPSTR pFlowTexture)
{
	HRESULT hr  = E_FAIL;

	hr = EnableDetail(TRUE);
	KGLOG_COM_PROCESS_ERROR(hr);

	KGLOG_PROCESS_ERROR(m_lpDetail);
    KGLOG_PROCESS_ERROR(m_lpMaterial);

	if(nDetailType == -1)
	{
		if(m_lpDetail)
		{
            BOOL bWholoColorCast = m_lpMaterial->GetDetailWholeColorCast();
            KG3DMaterialDetail* pDetail = m_lpDetail->GetSubSetDetail(bWholoColorCast, nSubSetID);
            KG_PROCESS_ERROR(pDetail);

			pDetail->m_fMainColorMax   =  fBodyMax  ;
			pDetail->m_fMainColorMin   =  fBodyMin  ;
			pDetail->m_fMainColorScale =  fBodyScale;
			pDetail->m_cMainColor      =  vBodyColor;
			pDetail->m_cMainColor1     =  vBodyColor1;
			pDetail->m_fEnv            =  fEnv      ;
			pDetail->m_fSpecular       =  fSpecular ;

			pDetail->m_szChannelFileName  = pChannelFile;

			pDetail->m_fDetail0ColorScale = fDetail0_ColorScale;
			pDetail->m_fDetail0UVScale    = fDetail0_UVScale   ;
			pDetail->m_fDetail0Env        = fDetail0_Env       ;
			pDetail->m_fDetail0Specular   = fDetail0_Specular  ;
			pDetail->m_cDetial0Color      = vDetail0_Color     ;
			pDetail->m_szDetail0FileName  = pDetail0_File;

			pDetail->m_fDetail1ColorScale = fDetail1_ColorScale;
			pDetail->m_fDetail1UVScale    = fDetail1_UVScale   ;
			pDetail->m_fDetail1Env        = fDetail1_Env       ;
			pDetail->m_fDetail1Specular   = fDetail1_Specular  ;
			pDetail->m_cDetial1Color      = vDetail1_Color     ;
			pDetail->m_szDetail1FileName  = pDetail1_File;

            m_lpDetail->m_szTextrueFileName = pszTextureFileName;

			pDetail->m_Flow_Color = Flow_Color;
			pDetail->m_fFlow_ColorScale = fFlow_ColorScale;
			pDetail->m_fFlow_UVScale = fFlow_UVScale;
			pDetail->m_vFlow_Speed = vSpeed;
			pDetail->m_szFlow_Texture = pFlowTexture;


			pDetail->LoadTextures();
                
			return S_OK;
		}
	}
	else if(m_lpMaterial && m_lpMaterial->IsLoaded())
	{
		return m_lpMaterial->SetMaterilDetail(nDetailType,nSubSetID,pszName, pszTextureFileName, fEnv,fSpecular,
			fBodyMax,fBodyMin,fBodyScale,vBodyColor,vBodyColor1,pChannelFile,
			fDetail0_ColorScale,fDetail0_UVScale,fDetail0_Env,fDetail0_Specular,vDetail0_Color,pDetail0_File,
			fDetail1_ColorScale,fDetail1_UVScale,fDetail1_Env,fDetail1_Specular,vDetail1_Color,pDetail1_File,
			Flow_Color,fFlow_ColorScale,fFlow_UVScale,vSpeed,pFlowTexture);
	}

Exit0:
	return E_FAIL;
}

HRESULT KG3DModel::SetPointLightInfo(KG3D_PointLight_Info& vInfo)
{
	if (g_bEnablePointLight)
	{
		if(vInfo.m_nNumPointLights)
		{
			if(!m_lpPointLightInfo)
			{
				m_lpPointLightInfo = g_cPointLightInfoPool.RequestResource();
			}
			KGLOG_PROCESS_ERROR(m_lpPointLightInfo);

			(*m_lpPointLightInfo) = vInfo;
		}
		else
		{
			if(m_lpPointLightInfo)
			{
				g_cPointLightInfoPool.FreeResource(m_lpPointLightInfo);
				m_lpPointLightInfo = NULL;
			}
		}
		for (std::set<KG3DModelLod>::iterator i = m_ChildModels.begin(); i != m_ChildModels.end(); i++)
		{
			KG3DModelLod& Lod = *i;

			KG3DModel* pSubModel = Lod.pModelCurrent;
			pSubModel->SetPointLightInfo(vInfo);
		}
	}
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DModel::ReloadMesh()
{
	if(m_lpMesh)
	{
		return m_lpMesh->LoadFromFile(m_lpMesh->m_scName.c_str(),0,0);
	}
	return S_OK;
}

HRESULT KG3DModel::LoadPostProcess()
{
    if (m_lpMesh)
    {
        m_lpMesh->CheckLoadingState();

        if(!m_bMeshPostLoadProcessed && m_lpMesh->m_eDynamicLoadState == KG3DMesh::STATE_VIDEOMEM)
        {
            MeshPostLoadProcess();
        }
    }

    return S_OK;
}

HRESULT KG3DModel::LoadFromFileMultiThread()
{
	_LoadFromFile(m_scName.c_str(),0,m_uOption);
	return S_OK;
}


HRESULT KG3DModel::ProcessCommands()
{
	return m_Commands.ProcessCommandsForUser(this);
}

void KG3DModel::SeekSplitAnimation(DWORD dwSplitType, 
									 enuAnimationControllerPriority nPrority,
									 DWORD dwSeekType, 
									 float fOffset)
{
	if (IsLoaded() && !m_dwPlayAnimationCommandCnt)
	{
		_SeekSplitAnimation(dwSplitType,
			nPrority,
			dwSeekType,
			fOffset);
	}
	else
	{
		KG3DModelCommand_SeekAnimation *pSeek = new KG3DModelCommand_SeekAnimation(dwSplitType,
			nPrority,
			dwSeekType,
			fOffset);
		m_dwPlayAnimationCommandCnt++;
		m_Commands.AddCommand(pSeek,TRUE);
	}
}

void KG3DModel::_SeekSplitAnimation(DWORD dwSplitType, 
								   enuAnimationControllerPriority nPrority,
								   DWORD dwSeekType, 
								   float fOffset)
{
	_ASSERTE(0);
}


BOOL KG3DModel::OnProcessCommands(KG3DModelCommandBase* pCmd)
{
	BOOL bSucceed = FALSE;


	switch(pCmd->m_eType)
	{
	case CMD_PlayAnimation:
		{
			KG3DModelCommand_PlayAnimation* pAni = static_cast<KG3DModelCommand_PlayAnimation*>(pCmd);				
			if(IsAniLoaded(pAni->m_ePlayAnimationType,pAni->m_pTag,pAni->m_pClip))
			{
				PVOID pReserved = NULL;
				if(pAni->m_pReserved)
				{
					pReserved = (void*)&pAni->Span;
				}
				_PlayAnimation(pAni->m_dwPlayType,
					pAni->m_strAnimationName.c_str(),
					pAni->m_fSpeed,
					pAni->m_nOffsetTime,
					pReserved,
					pAni->m_pUserdata,
					pAni->m_Priority,
					pAni->m_ePlayAnimationType,
					pAni->m_pTag,
					pAni->m_pClip);
				m_dwPlayAnimationCommandCnt--;
				bSucceed = TRUE;
			}
			break;
		}
	case CMD_PlaySplitAnimation:
		{
			KG3DModelCommand_PlaySplitAnimation* pAni = static_cast<KG3DModelCommand_PlaySplitAnimation*>(pCmd);
			if(IsAniLoaded(pAni->m_ePlayAnimationType,pAni->m_pTag,pAni->m_pClip))
			{
				PVOID pReserved = NULL;
				if(pAni->m_pReserved)
				{
					pReserved = (void*)&pAni->Span;
				}
				_PlaySplitAnimation(pAni->m_strAnimationName.c_str(),
					pAni->m_dwSplitType,pAni->m_dwExtraInfo,pAni->m_dwPlayType,
					pAni->m_fSpeed,pAni->m_nOffsetTime,
					pReserved,
					pAni->m_pUserdata,
					pAni->m_Priority,
					pAni->m_ePlayAnimationType,
					pAni->m_pTag,
					pAni->m_pClip);
				m_dwPlayAnimationCommandCnt--;
				bSucceed = TRUE;
			}
			break;
		}
	case CMD_BindToSocket:
		{
			KG3DModelCommand_BindToSocket* pBin = static_cast<KG3DModelCommand_BindToSocket*>(pCmd);
			pBin->m_lpModel->FrameMove();
			pBin->m_nTryCount++;

			if(pBin->m_nTryCount<10000)
			{
				BindExtraInfo Info;
				int nSocket = this->FindSocket(pBin->m_cszSocketName.c_str(), &Info);
				if(nSocket != -1)
				{
					pBin->m_lpModel->_BindToSocket(this,pBin->m_cszSocketName.c_str());	
					bSucceed = TRUE;			
				}
			}
			else
			{
				bSucceed = TRUE;
			}

			break;
		}
	case CMD_BindToBone:
		{
			KG3DModelCommand_BindToBone* pBin = static_cast<KG3DModelCommand_BindToBone*>(pCmd);
			
			pBin->m_lpModel->_BindToBone(this,pBin->m_cszBoneName.c_str());
			bSucceed = TRUE;
			
			break;
		}	
	case CMD_SetPhysicsScene:
		{
			KG3DModelCommand_SetPhysicsScene* pPhysics = static_cast<KG3DModelCommand_SetPhysicsScene*>(pCmd);
			_SetPhysicsScene(pPhysics->m_pScene);
			bSucceed = TRUE;
			break;
		}
	case CMD_SeekAnimation:
		{
			KG3DModelCommand_SeekAnimation *pSeek = static_cast<KG3DModelCommand_SeekAnimation*>(pCmd);
			_SeekSplitAnimation(pSeek->m_dwSplitType,
				pSeek->m_Prority,
				pSeek->m_dwSeekType,
				pSeek->m_fOffset);
			m_dwPlayAnimationCommandCnt--;
			bSucceed = TRUE;
			break;
		}	
	case CMD_SetDetail:
		{
			KG3DModelCommand_SetDetail *pDetail = static_cast<KG3DModelCommand_SetDetail*>(pCmd);
			if(m_lpMaterial && m_lpMaterial->IsLoaded())
			{
				_SetDetail(pDetail->m_nTable,pDetail->m_nDefine);
				bSucceed = TRUE;
			}
			break;
		}	
	case CMD_SetMaterial:
		{
			KG3DModelCommand_SetMaterial *pMtl = static_cast<KG3DModelCommand_SetMaterial*>(pCmd);
			_SetMaterial(pMtl->m_lpMlt);
			bSucceed = TRUE;
			break;
		}
	case CMD_KG3DModel_LoadMaterialFromFile:
		{
			if(!m_lpMaterial)
			{
				KG3DModelCommand_KG3DModel_LoadMaterialFromFile* pLoad = static_cast<KG3DModelCommand_KG3DModel_LoadMaterialFromFile*>(pCmd);
				_LoadMaterialFromFile(pLoad->m_szFileName.c_str(),pLoad->m_dwOption);
				bSucceed = TRUE;
				m_nMtlLoadCmd--;
			}
			else if(m_lpMaterial->IsLoaded())
			{
				KG3DModelCommand_KG3DModel_LoadMaterialFromFile* pLoad = static_cast<KG3DModelCommand_KG3DModel_LoadMaterialFromFile*>(pCmd);
				_LoadMaterialFromFile(pLoad->m_szFileName.c_str(),pLoad->m_dwOption);
				bSucceed = TRUE;
				m_nMtlLoadCmd--;				
			}
			break;
		}
	default:
		;
	}
	//ASSERT(bSucceed);
	return bSucceed;
}

void KG3DModel::RemoveCaption()
{
	g_cCaption2D.Remove(m_dwHeadLineID[0]);
	m_dwHeadLineID[0] = CAPTION_INVALIDATE_INDEX;	
}

KG3DMaterialDetail* KG3DModel::GetDetail(int nSubSetID)
{
    KG3DMaterialDetail* pDetail = NULL;
    BOOL bWholeColorCast = TRUE;
    KG_PROCESS_ERROR(m_lpMaterial);
    
	if(!m_lpDetail)
	{
		EnableDetail(TRUE);
	}

    KG_PROCESS_ERROR(m_lpDetail);
    bWholeColorCast = m_lpMaterial->GetDetailWholeColorCast();
	pDetail = m_lpDetail->GetSubSetDetail(bWholeColorCast, nSubSetID);
    KG_PROCESS_ERROR(pDetail);

Exit0:
    return pDetail;
}

BOOL KG3DModel::HasDetail()
{
	return m_lpDetail!=NULL;
}


void KG3DModel::PauseAnimation(BOOL bPause)
{
	if (m_Splitter.GetEnable())
	{
		m_Splitter.GetAnimationWarper()->Pause(bPause);
	}
	else
	{
		m_AnimationController.PauseAnimation(bPause);
	}
}

void KG3DModel::FrameMoveOnce()
{
}

KG3DMaterial::KG3DMaterialSubset* KG3DModel::GetMaterialSubset(DWORD dwIndex)
{
	if(m_lpMaterial && m_lpMaterial->IsLoaded())
		return m_lpMaterial->GetSubMaterial(dwIndex);
	return NULL;
}

HRESULT KG3DModel::IESetLod(float fLod)
{
	return SetLOD(fLod);
}

HRESULT KG3DModel::CheckMeshLodFiles(KG3DMesh* pDefaultMesh,UINT uOption)
{
	HRESULT hr = E_FAIL;
	
	TCHAR szMeshName[MAX_PATH];
	TCHAR szMeshNameL[MAX_PATH];
	TCHAR szMeshNameM[MAX_PATH];

	wsprintf(szMeshName,"%s",pDefaultMesh->m_scName.c_str());

	TCHAR drive[256];
	TCHAR dir[256];
	TCHAR fname[256];
	TCHAR ext[256];

	_splitpath( szMeshName, drive, dir, fname, ext );

	wsprintf(szMeshNameL,"%s%s%s_l%s",drive,dir,fname,ext);
	wsprintf(szMeshNameM,"%s%s%s_m%s",drive,dir,fname,ext);

	KG3DMesh* pModelM = NULL;
	KG3DMesh* pModelL = NULL;

	if(g_IsFileExist(szMeshNameM))
	{
		hr = g_cMeshTable.LoadResourceFromFile(szMeshNameM,0,uOption,(IKG3DResourceBase**)&pModelM);
		if(SUCCEEDED(hr))
		{
			SAFE_RELEASE(m_lpMeshMedium);
			m_lpMeshMedium = pModelM;
		}
	}

	if(g_IsFileExist(szMeshNameL))
	{
		hr = g_cMeshTable.LoadResourceFromFile(szMeshNameL,0,uOption,(IKG3DResourceBase**)&pModelL);
		if(SUCCEEDED(hr))
		{
			SAFE_RELEASE(m_lpMeshlLow);
			m_lpMeshlLow = pModelL;
		}
	}

	return S_OK;
}

HRESULT KG3DModel::CheckModelLodFiles(KG3DModel* pDefaultModel,UINT uOption)
{
	if(!g_cEngineOption.bEnableModelLod)
		return S_OK;

	HRESULT hr = E_FAIL;
	TCHAR szMtlName[MAX_PATH];
	TCHAR szMeshName[MAX_PATH];
	TCHAR szMeshNameL[MAX_PATH];
	TCHAR szMeshNameM[MAX_PATH];
	pDefaultModel->GetMaterialFileName(szMtlName);
	pDefaultModel->GetMeshFileName(szMeshName);

	TCHAR drive[256];
	TCHAR dir[256];
	TCHAR fname[256];
	TCHAR ext[256];

	_splitpath( szMeshName, drive, dir, fname, ext );

	wsprintf(szMeshNameL,"%s%s%s_l%s",drive,dir,fname,ext);
	wsprintf(szMeshNameM,"%s%s%s_m%s",drive,dir,fname,ext);

	KG3DModel* pModelM = NULL;
	KG3DModel* pModelL = NULL;

	if(g_IsFileExist(szMeshNameM))
	{
		hr = g_cModelTable.LoadResourceFromFile(szMeshNameM,0,uOption|MLO_MATERIAL_NOMAT,(IKG3DResourceBase**)&pModelM);
		if(SUCCEEDED(hr))
		{
			SAFE_RELEASE(m_lpModelMedium);
			m_lpModelMedium = pModelM;
		}
	}

	if(g_IsFileExist(szMeshNameL))
	{
		hr = g_cModelTable.LoadResourceFromFile(szMeshNameL,0,uOption|MLO_MATERIAL_NOMAT,(IKG3DResourceBase**)&pModelL);
		if(SUCCEEDED(hr))
		{
			SAFE_RELEASE(m_lpModelLow);
			m_lpModelLow = pModelL;
		}
	}

	return S_OK;
}

BOOL KG3DModel::IsReadyToAttchModelST()
{
	BOOL bReady = TRUE;
	
	bReady &= m_bMeshPostLoadProcessed;
	if(m_lpModelMedium)
	{
		bReady &= m_lpModelMedium->m_bMeshPostLoadProcessed;
	}
	if(m_lpModelLow)
	{
		bReady &= m_lpModelLow->m_bMeshPostLoadProcessed;
	}

	return bReady;
}

HRESULT KG3DModel::MultiThreadProcess()
{
	return S_OK;
}

HRESULT KG3DModel::UpdataBoneFamily(DWORD dwBoneIndex)
{
	HRESULT hResult  = E_FAIL;
	KG3DMesh::BoneInfo1 *pBoneInfo = NULL;
	D3DXMATRIX matBone;

	KGLOG_PROCESS_ERROR(m_lpMesh);
	KGLOG_PROCESS_ERROR(dwBoneIndex < m_lpMesh->m_dwNumBone);

	pBoneInfo = &(m_lpMesh->m_pBoneInfo[dwBoneIndex]);


	GetBoneMatrixLocal(dwBoneIndex,&matBone,NULL);

	_UpdataBoneFamily(pBoneInfo,matBone);

	hResult = S_OK;
Exit0:
	return hResult;
}

HRESULT KG3DModel::_UpdataBoneFamily(KG3DMesh::BoneInfo1 *pBoneInfo,D3DXMATRIX& matBone)
{
	HRESULT hResult  = E_FAIL;
	D3DXMATRIX matChild;

	for (DWORD j = 0; j < pBoneInfo->dwNumChild; j++)
	{
		DWORD dwChildIndex = pBoneInfo->dwChildIndex[j];

		KG3DMesh::BoneInfo1* pChild = &m_lpMesh->m_pBoneInfo[dwChildIndex];

		matChild = pChild->ParentReletiveMatrix * matBone;

		SetBoneMatrixLocal(dwChildIndex,&matChild);

		_UpdataBoneFamily(pChild,matChild);
	}

	hResult = S_OK;
//Exit0:
	return hResult;
}

HRESULT KG3DModel::EnableUseLightmap2(BOOL bEnable,float ratioA,float ratioB)
{
	if(m_lpLightMap)
	{
		m_lpLightMap->EnableUseLightmap2(bEnable,ratioA,ratioB);
	}
	return S_OK;
}

HRESULT KG3DModel::EnableLightmap(BOOL bEnable,int nWidth,BOOL bComputing,LPSTR pFileName)
{
	if(bEnable)
	{
		if(!m_lpLightMap)
		{
			m_lpLightMap = new KG3DModelLightMap;
			KGLOG_PROCESS_ERROR(m_lpLightMap);
		}
		m_lpLightMap->Init(nWidth,bComputing,pFileName);
	}
	else
	{
		SAFE_DELETE(m_lpLightMap);
	}
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DModel::CheckRuntimeShaderType()
{
	return S_OK;
}
