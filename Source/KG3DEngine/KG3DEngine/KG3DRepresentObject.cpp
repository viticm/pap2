////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DRepresentObject.cpp
//  Version     : 1.0
//  Author      : ZhaoXinyu (Chiyer)
//  CopyRight   : Kingsoft Corporation All rights Reserved
//  History     : 2005-9-27 14:47:07
//  Comment     : Object Class
//
////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "kg3drepresentobject.h"
#include "kg3drepresentobjecttable.h"
#include "kg3dgraphicstool.h"
#include "kg3dmodeltable.h"
#include "kg3dMesh.h"
#include "kg3dMaterial.h"
#include "KG3DRepresentSpeedTree.h"
#include "KG3DSFX.h"
#include "KG3DScene.h"
#include "kg3denginemanager.h"
#include "KG3DSceneSceneEditor.h"
#include "../physiscsystem/KG3DPhysiscScene.h"
#include "KG3DSceneBlock.h"
#include "KG3DClipTable.h"
#include "KG3DRenderSorterStrategeDefault.h"
#include "KG3DModelPointLight.h"
#include "KG3DIntersectionBase.h"
#include "KG3DBsp.h"
#include "KG3DScenePointLightRender.h"
#include "KG3DAnimationEventHandlerRepresentDummy.h"
//#include "SO3GlobalDef.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

enum
{
	FRAME_QUALITY_SPEED		=	8,
	FRAME_QUALITY_LOW		=	2,
	FRAME_QUALITY_HIGH		=	0,
	FRAME_QUALITY_NORMAL	=	1,

};




KG3DRepresentObject::KG3DRepresentObject(void)
:m_pMatParent(NULL)
,m_dwState(0)
,m_bGroupPoseValidate(FALSE)
{
	m_dwID       = 0;
	m_dwType     = REPRESENTOBJECT_DEFAULT;
	m_ulRefCount  = 0;
	//m_dwRenderCount = 0;
	m_pProperty = NULL;
	m_dwSceneLayerID = 0;
	//m_lpScene = NULL;//<SetScene Refactor>
    m_pParentObj = NULL;
	//m_bSelected = FALSE;

	m_eState = REPRESENTOBJECTSTATE_NONE;

	m_fAlpha = 1.0F;
    m_pPvsOn = NULL;

    m_nDynamicObstacleGroupIndex = -1;/*INVALID_OBSTACLE_GROUP;*///这个值让外面设进来就行了，为了这个值包含了一整个和引擎无关的头文件，而且是编辑器业务逻辑的



    D3DXMatrixIdentity(&m_matWorld);

	//m_bBBoxChanged = FALSE;
	m_HandleHeldBySpaceMgr = static_cast<UINT>(-1);
	m_fVisibleAngle = 100;
	m_dwVisibleCount = 0;
    m_bSleep = FALSE;

	m_dwLODExist = EM_LOD_EXIST_NONE;
	m_nLODLevel = EM_LOD_LEVEL_HIGH;
	m_dwLODFrameCount = emCheckLODFrameGap;
}

KG3DRepresentObject::~KG3DRepresentObject(void)
{
	UnInit();
}

ULONG STDMETHODCALLTYPE KG3DRepresentObject::AddRef()
{
    return (ULONG)KG_InterlockedIncrement((long *)&m_ulRefCount);
}

ULONG STDMETHODCALLTYPE KG3DRepresentObject::Release()
{
    ULONG uNewRefCount = (ULONG)KG_InterlockedDecrement((long *)&m_ulRefCount);
    if (uNewRefCount == 0)
        OnReferenceZero();
    return uNewRefCount;
}

HRESULT STDMETHODCALLTYPE KG3DRepresentObject::QueryInterface(
    REFIID riid, void **ppvObject
)
{
	return E_FAIL;
}

HRESULT KG3DRepresentObject::Init()
{
	/*
	TCHAR Name[256];
		m_scName = Name;*/
	m_scName.reserve(256);
	m_scName = "";
    m_dwLastCullCounter = 0;
    m_bBBoxChanged = FALSE;

	//m_bSelected = FALSE;
	//m_dwFrameTimes = 0;
	//m_dwFrameQuality = 0;

	return S_OK;
}

HRESULT KG3DRepresentObject::UnInit()
{
	//m_bSelected = FALSE;
	SetPhysicsScene(NULL);
	for (size_t i=0;i<m_vecModelL.size();i++)
	{
		KG3DModel* pModel = m_vecModelL[i];
		if(pModel)
		{
			pModel->Unadvise(this);
            pModel->SetParentObjMatrix(NULL);
		}
		SAFE_RELEASE(pModel);
	}
	m_vecModelL.clear();
	for (size_t i=0;i<m_vecModelM.size();i++)
	{
		KG3DModel* pModel = m_vecModelM[i];
		if(pModel)
		{
			pModel->Unadvise(this);
            pModel->SetParentObjMatrix(NULL);
		}
		SAFE_RELEASE(pModel);
	}
	m_vecModelM.clear();
	for (size_t i=0;i<m_vecModel.size();i++)
	{
		KG3DModel* pModel = m_vecModel[i];
		if(pModel)
		{
			pModel->Unadvise(this);
            pModel->SetParentObjMatrix(NULL);
		}
		SAFE_RELEASE(pModel);
	}
	m_vecModel.clear();
	for (size_t i=0;i<m_vecModelInformation.size();i++)
	{
		ModelInformation* pModelInfo = m_vecModelInformation[i];
		SAFE_DELETE(pModelInfo);
	}
	m_vecModelInformation.clear();
	RemoveFromContainer();
    //LeavePvs();
	//RemoveFromRegions();
	m_HandleHeldBySpaceMgr =static_cast<UINT>(-1);
    m_dwVisibleCount = 0;
    m_dwLastCullCounter = 0;
    m_bBBoxChanged = FALSE;
    m_nLODLevel = 0;
	return S_OK;
}

HRESULT KG3DRepresentObject::OnEnterPvs(KG3DRepresentObjectPVS* pvs)
{
    if (m_pPvsOn != pvs)
    {
        if (m_pPvsOn)
            m_pPvsOn->RemoveObject(this, g_cGraphicsTool.GetCurScene());
        m_pPvsOn = pvs;
    }
    return S_OK;
}

HRESULT KG3DRepresentObject::LeavePvs()
{

    if (m_pPvsOn)
        m_pPvsOn->RemoveObject(this, g_cGraphicsTool.GetCurScene());
    m_pPvsOn = NULL;

    return S_OK;
}


HRESULT KG3DRepresentObject::LoadFromFile(
    const char cszFileName[], unsigned uFileNameHash, unsigned uOption
)
{
	return S_OK;
}

HRESULT KG3DRepresentObject::SaveToFile(
    const char cszFileName[],   unsigned uOption
)
{
	return S_OK;
}

DWORD KG3DRepresentObject::GetID()
{
	return m_dwID;
}

unsigned KG3DRepresentObject::GetType()
{
	return m_dwType;
}

HRESULT KG3DRepresentObject::PrepareRender(unsigned uOption)//压缩数据
{
	return S_OK;
}
HRESULT KG3DRepresentObject::PostRender()//恢复数据，对象可以立即被使用
{
	return S_OK;
}
HRESULT KG3DRepresentObject::GetDateInformation(int* pnMem,int* pnVedioMem,int* pnConMem,int* pnConVMem)//得到数据信息，参数依次为占用内存大小，现存大小，压缩后内存大小，压缩后内存大小
{
	return S_OK;;
}
unsigned   KG3DRepresentObject::GetLastUseTime()//查询上次使用的时间
{
	return S_OK;;
}
unsigned   KG3DRepresentObject::GetUseFrequency()//查询使用频率，一段时间里的使用次数
{
	return S_OK;;
}

HRESULT KG3DRepresentObject::OnReferenceZero()
{
	return g_GetObjectTable().ReleaseResource(this, RELEASE_ATONCE);
}


HRESULT  KG3DRepresentObject::FrameMove( )
{
    BOOL bNeedComputeBBox = !m_bSleep;

	if(!m_bSleep)
	{
		if (m_pMatParent)
			m_matWorld = m_Matrix * (*m_pMatParent);
		else
			m_matWorld = m_Matrix;
	}
	//<SLEEP_REFRACTOR>
	vector<KG3DModel*>::iterator i= m_vecModel.begin();
	vector<KG3DModel*>::iterator iend = m_vecModel.end();
	switch(m_nLODLevel)
	{
	case EM_LOD_LEVEL_HIGH:
		{
			/*i= m_vecModel.begin();
			iend = m_vecModel.end();*/
		}
		break;
	case EM_LOD_LEVEL_MIDDLE:
		{
			i= m_vecModelM.begin();
			iend = m_vecModelM.end();
		}
		break;
	case EM_LOD_LEVEL_LOW:
		{
			i= m_vecModelL.begin();
			iend = m_vecModelL.end();
		}
		break;
	default:
		_ASSERT(0);
	}
	while (i != iend)
	{
		KG3DModel* pModel = *i;
		if (pModel)
		{
			if(pModel->IsNeedFrameMove() || !m_bSleep)
			{
				pModel->FrameMove();
				pModel->m_bSleep = TRUE;
			}
			if (pModel->IsBBoxChanged())
			{
				pModel->SetBBoxChanged(FALSE);
				bNeedComputeBBox = TRUE;
			}
		}
		i++;
	}

    if (bNeedComputeBBox)
    {
        ComputeBBox();
    }

    m_bSleep = TRUE;

	return S_OK;
}

//HRESULT KG3DRepresentObject::AddToPhysiscScene()
//{
//	KG3DModel* pModel = NULL;
//	pModel = m_vecModel[0];
//	if (m_lpScene && pModel && m_lpScene->GetPhysiscScene())
//		m_lpScene->GetPhysiscScene()->InputModel(pModel,TRUE);
//	return S_OK;
//}
//
//HRESULT KG3DRepresentObject::RemoveFromPhysiscScene()
//{
//	KG3DModel* pModel = NULL;
//	if(m_vecModel.size() <= 0)
//		return S_OK;
//	pModel = m_vecModel[0];
//	if (m_lpScene && pModel && m_lpScene->GetPhysiscScene())
//		m_lpScene->GetPhysiscScene()->RemoveModel(pModel,TRUE);
//	return S_OK;
//}

HRESULT KG3DRepresentObject::SetModel(DWORD dwIndex,const char pcszMeshFileName[],const char pcszMtlFileName[],const char pcszAniFileName[],BOOL bForceLoad )
{
	//return E_FAIL;
	HRESULT hr = S_OK;
	DWORD dwNowTime = timeGetTime();
	DWORD cost1 = 0;
	KG3DPhysiscScene *pPhysicsSceneSave = NULL;
	KG3DModel* pModel = NULL;
	BOOL bAnimation = FALSE;
	DWORD dwLoadOption = 0;

	BOOL bAllowMult = (m_dwType == REPRESENTOBJECT_DEFAULT)
		||(m_dwType == REPRESENTOBJECT_SET)||
		(m_dwType == REPRESENTOBJECT_NPC)||
		(m_dwType == REPRESENTOBJECT_DOODAD)||
		(m_dwType == REPRESENTOBJECT_PVS);

    KG_PROCESS_ERROR(pcszMeshFileName);

	if(!bForceLoad)
		dwLoadOption = (g_eEngineOption.bUseMultiThreadLoad&&bAllowMult) ? (MLO_TEXTURE_MULTI | MLO_MESH_MULTI) : 0;

	if (pcszAniFileName &&
		pcszAniFileName[0] != '\0' )
	{
		//dwLoadOption = MLO_TEXTURE_MULTI;
		bAnimation = TRUE;
		//return S_OK;
	}

	if(dwIndex>=m_vecModel.size())
	{
		m_vecModel.resize(dwIndex+1,NULL);
	}
	pModel = m_vecModel[dwIndex];
	if(pModel)
	{
		pPhysicsSceneSave = pModel->m_lpPhysicsScene;
		pModel->Unadvise(this);
		pModel->SetPhysicsScene(NULL);
	}
	SAFE_RELEASE(pModel);
	m_vecModel[dwIndex] = NULL;

	hr = g_cModelTable.LoadResourceFromFile(pcszMeshFileName,
		0,
		dwLoadOption,
		(IKG3DResourceBase **)&pModel);
	KG_COM_PROCESS_ERROR(hr);

	pModel->Advise(this);
	m_vecModel[dwIndex] = pModel;

	cost1 = timeGetTime() - dwNowTime;

    pModel->SetParentObjMatrix(&m_matWorld);

    pModel->SetPhysicsScene(pPhysicsSceneSave);

	if(bAnimation)
	{
		if(pModel)
		{
			float K = rand()%10*0.01F;
			hr = pModel->PlayAnimation(ENU_ANIMATIONPLAY_CIRCLE, pcszAniFileName, 1.0f+K, 0, NULL, NULL, ANICTL_PRIMARY);
			IKG3DAnimationController *pController = pModel->GetAnimationController(ANICTL_PRIMARY);
			pController->RegistereEventHandler(g_pRepresentEventHandlerDummy);
		}
	}
    else if (pModel->m_dwType == MESHTYPE_SFX)
    {
        KG3DSFX *pSFX = (KG3DSFX*)pModel;
        pSFX->PlayAnimation(SFX_AP_LOOP);
    }

	///////////加载LOD模型/////////////////////////////////////////////////////
	if(g_cEngineOption.bEnableModelLod)
	{
		TCHAR strModelNameL[MAX_PATH];
		TCHAR strModelNameM[MAX_PATH];
		//分析出低模名称
		{
			TCHAR strFile[MAX_PATH];
			TCHAR strDrive[MAX_PATH];
			TCHAR strPath[MAX_PATH];
			TCHAR strExt[MAX_PATH];
			_splitpath_s(pcszMeshFileName,
				strDrive,MAX_PATH,
				strPath, MAX_PATH,
				strFile, MAX_PATH,
				strExt,  MAX_PATH);
			sprintf_s(strModelNameM,MAX_PATH,"%s%s%s_M%s",strDrive,strPath,strFile,strExt);
			sprintf_s(strModelNameL,MAX_PATH,"%s%s%s_L%s",strDrive,strPath,strFile,strExt);
			//sprintf_s(strModelNameL,MAX_PATH,"\\data\\source\\maps_source\\aaa.Mesh");
			//sprintf_s(strModelNameM,MAX_PATH,"\\data\\source\\maps_source\\石头\\WJ_石头008_004.Mesh");
		}
		SetMiddleModel(dwIndex,strModelNameM,"",dwLoadOption);
		SetLowModel(dwIndex,strModelNameL,"",dwLoadOption);
	}
	///////////////////////////////////////////////////////////////////////
	UpdateTransformation();

	DWORD dwCost = timeGetTime() - dwNowTime;
	if(dwCost >= 30)
	{
		//KGLogPrintf(KGLOG_ERR, "%d 1:%d KG3DRepresentObject::SetModel %s.",dwCost,cost1,pcszMeshFileName);
	}

	return S_OK;
Exit0:
	return E_FAIL;
}

void KG3DRepresentObject::UpdateTransformation()
{
	KG3DTransformation::UpdateTransformation();

	for (size_t i = 0; i < m_vecModel.size(); i++)
	{
		if (m_vecModel[i])
		{
			m_vecModel[i]->SetParentObjMatrix(&m_matWorld);
			m_vecModel[i]->Helper_UpdateTransformation(TRUE);
			m_vecModel[i]->ComputeBBox();
		}
	}

	for (size_t i = 0; i < m_vecModelM.size(); i++)
	{
		if (m_vecModelM[i])
		{
			m_vecModelM[i]->SetParentObjMatrix(&m_matWorld);
			m_vecModelM[i]->Helper_UpdateTransformation(TRUE);
			m_vecModelM[i]->ComputeBBox();
		}
	}
	for (size_t i = 0; i < m_vecModelL.size(); i++)
	{
		if (m_vecModelL[i])
		{
			m_vecModelL[i]->SetParentObjMatrix(&m_matWorld);
			m_vecModelL[i]->Helper_UpdateTransformation(TRUE);
			m_vecModelL[i]->ComputeBBox();
		}
	}
	ComputeBBox();
}

int KG3DRepresentObject::IsSockWave()
{
    if (
        m_vecModel.size() &&
        m_vecModel[0]     &&
        m_vecModel[0]->m_nIsSockWave
     )
        return TRUE;
    else
        return FALSE;
}

HRESULT KG3DRepresentObject::RenderReflect(float fWaterHeight,D3DXPLANE& WaterPlane,D3DXPLANE& WaterPlaneFX)
{

	for(size_t i=0;i<m_vecModel.size();i++)
	{
		KG3DModel* pModel = m_vecModel[i];

		if (pModel)
		{
			pModel->RenderReflect(fWaterHeight,WaterPlane,WaterPlaneFX);
		}
	}

	return S_OK;
}

BOOL KG3DRepresentObject::HasSfx()
{
    for(size_t i=0;i<m_vecModel.size();i++)
    {
        KG3DModel* pModel = m_vecModel[i];

        if (pModel)
        {
			DWORD dwType = pModel->GetType();
			if(dwType == MESHTYPE_SFX || dwType == MESHTYPE_POINTLIGHT)
				return TRUE;
        }
    }

    return FALSE;

}

HRESULT KG3DRepresentObject::RenderHierarchy(BOOL bCull /* = TRUE */, KG3DScenePointLightRender* pRender)
{
    if (pRender)
    {
        for (size_t i = 0; i < m_vecModel.size(); ++i)
        {
            KG3DModel* pModel = m_vecModel[i];

            if (!pModel)
                continue;

            if (pModel->GetType() == MESHTYPE_POINTLIGHT)
                pRender->AddPointLight((KG3DModelPointLight*)pModel);
            else
                pRender->FindRefrenceLightForModel(pModel);
        }
    }

    return Render(0);
}

HRESULT  KG3DRepresentObject::Render(DWORD dwOption)
{

	for(size_t i=0;i<m_vecModel.size();i++)
	{
		KG3DModel* pModel = m_vecModel[i];

		if (pModel)
		{
			//pModel->GetRenderEnviromentParamenter(m_lpScene);
			pModel->Render(dwOption, NULL);
		}
	}


	return S_OK;
}

HRESULT KG3DRepresentObject::RenderSelectMark(DWORD dwColor)
{
	//if(m_vecModel.size()>1)

    bool bDrawObjBox = true;
    for (size_t i = 0; i < m_vecModel.size(); ++i)
    {
        KG3DModel* pModel = m_vecModel[i];
        if (pModel)
            if (pModel->GetType() == MESHTYPE_POINTLIGHT)
            {
                bDrawObjBox = false;
                break;
            }
    }

    if (bDrawObjBox)
	    g_cGraphicsTool.DrawAABBox(&m_BBox, 0xff808080);

	for(size_t i=0;i<m_vecModel.size();i++)
	{
		KG3DModel* pModel = m_vecModel[i];
		if (pModel)
		{
			if (pModel->GetType() == MESHTYPE_SFX || pModel->GetType() == MESHTYPE_POINTLIGHT)
			{
				pModel->RenderSelectMark(dwColor);
			}
			else
			{
				static D3DXVECTOR3 BoxPos[8];
				pModel->m_BBoxLocal.TransformCoord(BoxPos,&pModel->m_matWorld);
				g_cGraphicsTool.DrawOBBox(BoxPos,dwColor);
			}
		}
	}
	return S_OK;
}

BOOL KG3DRepresentObject::IsVertexInside(D3DXVECTOR3& vVertex, D3DXVECTOR3& vDir)
{
    BOOL bhit = FALSE;

    if(m_vecModel.size()==0)
        return FALSE;

    for(size_t i=0;i<m_vecModel.size();i++)
    {
        KG3DModel* pModel = m_vecModel[i];
        if(pModel && pModel->GetMesh())
        {
            bhit = pModel->IsVertexInside(vVertex, vDir);
            if (bhit)
                return true;
        }
    }
    return false;
}


BOOL KG3DRepresentObject::GetCameraNearRayIntersect(IN CONST D3DXVECTOR3 vSrc[],
                                              IN CONST D3DXVECTOR3 vDst[],
                                              IN CONST INT         nNum,
                                              OUT      FLOAT*      pDis
                                              )
{
    BOOL bhit = FALSE;

    if (m_vecModel.size() == 0)
        return FALSE;

    D3DXMATRIX matSave;
    g_pd3dDevice->GetTransform(D3DTS_WORLD, &matSave);
    D3DXMATRIX Mat = m_matWorld*matSave;
    g_pd3dDevice->SetTransform(D3DTS_WORLD,&Mat);

    for(size_t i=0;i<m_vecModel.size();i++)
    {
        KG3DModel* pModel = m_vecModel[i];
        if(NULL != pModel)
        {
            /*
                KG3DOBJProperty *pObjProperty = NULL;
                			pModel->m_lpMesh->GetObjProperty((BYTE**)&pObjProperty);
                			if (!pObjProperty->bObscatleCamera)
                                continue;*/
			KG3DMesh* pMesh = pModel->GetMesh();
			if(NULL == pMesh)
				continue;

            if(! pMesh->GetObjProperty().bObscatleCamera)
				continue;

            float fDis = 0.f;

            if (pModel->GetCameraNearRayIntersect(vSrc, vDst, nNum, &fDis))
            {
                if (!i || (*pDis) < fDis)
                    (*pDis) = fDis;

                bhit = TRUE;
            }
        }
    }

    g_pd3dDevice->SetTransform(D3DTS_WORLD, &matSave);

    return bhit;
}
BOOL KG3DRepresentObject::RayIntersection( const D3DXVECTOR3& vSrc, const D3DXVECTOR3& vNormalizedDir
										  , FLOAT* pfRet, BOOL bIntersectBoxOnly, KG3DIntersectionFilter* pFilter /*= NULL*/ )
{
	BOOL bHit = FALSE;
	FLOAT fDistance = FLT_MAX;

	if (NULL != pFilter)
	{
		HRESULT hr = pFilter->Accept(*this);
		if (FAILED(hr))
			return FALSE;
	}

	if (bIntersectBoxOnly)
	{
		bHit = m_BBox.RayIntersection(vSrc, vNormalizedDir, &fDistance, NULL);
		if (bHit && NULL != pfRet)
		{
			*pfRet = fDistance;
		}
		return bHit;
	}

	//用Box加速跳出，避免复杂的Mesh碰撞
	if(! m_BBox.RayIntersection(vSrc, vNormalizedDir, NULL, NULL))
		return FALSE;

	for(size_t i=0;i<m_vecModel.size();i++)
	{
		KG3DModel* pModel = m_vecModel[i];
		if(pModel/* && pModel->m_lpMesh*/)
		{
			FLOAT fTemp = NULL;
			BOOL bHitTemp = pModel->RayIntersection(vSrc, vNormalizedDir, &fTemp,NULL, bIntersectBoxOnly, pFilter);

			if(bHitTemp)
			{
				if (fTemp < fDistance && fTemp > 0)
				{
					bHit |= bHitTemp;
					fDistance = fTemp;
				}
			}
		}
	}
	if (bHit)
	{
		_ASSERTE(fDistance < FLT_MAX);
		if (NULL != pfRet)
		{
			*pfRet = fDistance;
		}
	}
	return bHit;
}

BOOL KG3DRepresentObject::GetRayIntersect(DWORD& dwCount,D3DXVECTOR3* pInterPoint,DWORD dwMaxNum,D3DXVECTOR3 Origin,D3DXVECTOR3 Direction, BOOL bFiltrate)
{
	BOOL bhit = FALSE;

	if(m_vecModel.size()==0)
		return FALSE;

	D3DXVECTOR3* pVec = pInterPoint;
	D3DXMATRIX matSave;
	g_pd3dDevice->GetTransform(D3DTS_WORLD, &matSave);
	D3DXMATRIX Mat = m_matWorld*matSave;
	g_pd3dDevice->SetTransform(D3DTS_WORLD,&Mat);

	for(size_t i=0;i<m_vecModel.size();i++)
	{
		KG3DModel* pModel = m_vecModel[i];
		if(NULL != pModel)
        {
			KG3DOBJProperty *pObjProperty = NULL;

			KG3DMesh* pMesh = pModel->GetMesh();
			if (NULL == pMesh)
				continue;

			pMesh->GetObjProperty((BYTE**)&pObjProperty);
			if (bFiltrate && !pObjProperty->bObscatleCamera)
                continue;

			if(dwCount < dwMaxNum)
			{
				DWORD dwNum = 0;
				DWORD dwSize = dwMaxNum - dwCount;
				BOOL bHitC = pModel->GetRayIntersect(dwNum,pVec,dwSize,Origin,Direction);
				bhit |= bHitC;
				if(bHitC)
				{
					dwCount += dwNum;
					pVec += dwNum;
				}
			}

		}
	}
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &matSave);

	return bhit;
}

//BOOL KG3DRepresentObject::GetRayIntersectPointAndDir(DWORD& dwCount,
//												 D3DXVECTOR3* pInterPoint,
//												 float* pAngle,
//												 const D3DXVECTOR3& vec3Org,
//												 const D3DXVECTOR3& vec3Dir)
//{
//	BOOL bReturn = FALSE;
//
//	D3DXMATRIX matSave;
//	D3DXMATRIX matCurrent;
//	KG_PROCESS_ERROR(m_vecModel.size() != 0);
//
//	g_pd3dDevice->GetTransform(D3DTS_WORLD, &matSave);
//	matCurrent = m_matWorld * matSave;
//	g_pd3dDevice->SetTransform(D3DTS_WORLD, &matCurrent);
//
//	for(size_t i = 0; i < m_vecModel.size(); i++)
//	{
//		KG3DModel* pModel = m_vecModel[i];
//		if(!pModel)
//			continue;
//		bReturn |= pModel->GetRayIntersectPointAndDir(dwCount,
//			pInterPoint,
//			pAngle,
//			vec3Org,
//			vec3Dir);
//	}
//	g_pd3dDevice->SetTransform(D3DTS_WORLD, &matSave);
//
//Exit0:
//	return bReturn;
//}

BOOL KG3DRepresentObject::IsRayIntersect(D3DXVECTOR3&Inter,D3DXVECTOR3 Origin,D3DXVECTOR3 Direction)
{
	BOOL bhit = FALSE;

	if(m_vecModel.size()==0)
		return FALSE;

	D3DXMATRIX matSave;
	g_pd3dDevice->GetTransform(D3DTS_WORLD, &matSave);
	D3DXMATRIX Mat = m_matWorld*matSave;
	g_pd3dDevice->SetTransform(D3DTS_WORLD,&Mat);

	for(size_t i=0;i<m_vecModel.size();i++)
	{
		KG3DModel* pModel = m_vecModel[i];

        if (!pModel)
            continue;

        if (pModel->GetType() == MESHTYPE_POINTLIGHT)
        {
            Inter = D3DXVECTOR3(pModel->m_matWorld._41, pModel->m_matWorld._42, pModel->m_matWorld._43);
            bhit = TRUE;
            break;
        }

        if(pModel)
		{
			bhit = pModel->IsRayIntersect(Inter,Origin,Direction);
			if(bhit)
				break;
		}
	}

	g_pd3dDevice->SetTransform(D3DTS_WORLD, &matSave);

	return bhit;
}

BOOL KG3DRepresentObject::IsVisible(const vector<KG3DFrustum>& vFrustum)
{
    for (size_t v = 0; v < vFrustum.size(); ++v)
        if (vFrustum[v].IsAABBoxVisible(m_BBox))
            return TRUE;
    return FALSE;
}

const D3DXVECTOR3& KG3DRepresentObject::GetWorldPos()
{
    return D3DXMatrixGetTranslationPart(m_matWorld);
}

HRESULT KG3DRepresentObject::ComputeBBox()
{
	m_BBox.Clear();
    if(!m_bSleep)
    {
        if (m_pMatParent)
            m_matWorld = m_Matrix * (*m_pMatParent);
        else
            m_matWorld = m_Matrix;
    }
	
    D3DXVECTOR3 vPos(m_matWorld._41, m_matWorld._42, m_matWorld._43);
	D3DXVECTOR3 V(50,50,50);
	m_BBox.AddPosition(vPos + V);
	m_BBox.AddPosition(vPos - V);

	vector<KG3DModel*>::iterator i= m_vecModel.begin();
	vector<KG3DModel*>::iterator iend = m_vecModel.end();
	switch(m_nLODLevel)
	{
	case EM_LOD_LEVEL_HIGH:
		{
			/*i = m_vecModel.begin();
			iend = m_vecModel.end();*/
		}
		break;
	case EM_LOD_LEVEL_MIDDLE:
		{
			i = m_vecModelM.begin();
			iend = m_vecModelM.end();
		}
		break;
	case EM_LOD_LEVEL_LOW:
		{
			i = m_vecModelL.begin();
			iend = m_vecModelL.end();
		}
		break;
	default:
		_ASSERT(0);
	}
	while(i != iend)
	{
		KG3DModel* pModel = *i;
		if (pModel)
		{
			pModel->ComputeBBox();
			D3DXVECTOR3 BoxPos[8];
			pModel->m_BBox.Extract(BoxPos);
			m_BBox.AddPosition(BoxPos, 8);
		}
		i++;
	}

    m_bBBoxChanged = TRUE;

	return S_OK;
}
HRESULT KG3DRepresentObject::OnInformationChange(KG3DScene* pScene)
{
	if (!pScene || !pScene->m_lpSceneOutDoorSpaceMgr)
		return E_FAIL;

	return pScene->m_lpSceneOutDoorSpaceMgr->RequestObjectInformationChange(this);
}
HRESULT KG3DRepresentObject::OnTransformationChange(KG3DScene* pScene)
{
    if (!pScene || !pScene->m_lpSceneOutDoorSpaceMgr)
        return E_FAIL;

    return pScene->m_lpSceneOutDoorSpaceMgr->RequestObjectTransformationChange(this);
}

HRESULT KG3DRepresentObject::Copy(KG3DRepresentObject* pSrcObject)
{
	HRESULT hr = E_FAIL;
	const char* strAniFileName = NULL;
	TCHAR strFileName[MAX_PATH];
	strFileName[0] = '\0';

	for(size_t i=0;i<pSrcObject->m_vecModel.size();i++)
	{
		KG3DModel* pModelDest = NULL;
		KG3DModel* pModelSrc = pSrcObject->m_vecModel[i];
		if (!pModelSrc)
			continue;

		ModelInformation* pModelInfoSrc = pSrcObject->GetModelInformation(static_cast<int>(i));

		if (pModelSrc->GetType() == MESHTYPE_MODELST)
		{
            strAniFileName = pModelSrc->GetAnimationController(ANICTL_PRIMARY)->GetAnimationFileName();
			hr = SetModel((DWORD)i, pModelSrc->m_scName.c_str(), "", strAniFileName,TRUE);

			hr = GetModel(pModelDest,(DWORD)i,FALSE);
		}
		else
		{
			hr = pModelSrc->GetMeshFileName(strFileName);
			if (strFileName[0] != '\0')
			{
				if (pModelSrc->GetType() == MESHTYPE_SFX || pModelSrc->GetType() == MESHTYPE_POINTLIGHT)
				{
					hr = SetModel((DWORD)i, strFileName, "", "",TRUE);
				}
				else
				{
					strAniFileName = pModelSrc->GetAnimationController(ANICTL_PRIMARY)->GetAnimationFileName();
					hr = SetModel((DWORD)i,strFileName,"", strAniFileName,TRUE);
				}
				hr = GetModel(pModelDest,(DWORD)i,FALSE);
			}
		}

		//////////////////////////////////////////////////////////////////////////
		if((pModelSrc)&&(pModelDest))
		{
			D3DXVECTOR3    vModelTrans;
			D3DXVECTOR3    vModelScale;
			D3DXQUATERNION  vModelRot;
			pModelSrc->GetTranslation(&vModelTrans);
			pModelSrc->GetScaling(&vModelScale);
			pModelSrc->GetRotation(&vModelRot);

			pModelDest->SetTranslation(&vModelTrans);
			pModelDest->SetScaling(&vModelScale);
			pModelDest->SetRotation(&vModelRot);

			pModelDest->UpdateTransformation();
			pModelDest->SetPhysicsScene(pModelSrc->m_lpPhysicsScene);

			if(pModelInfoSrc)
			{
				SetModelInformation(static_cast<int>(i), pModelInfoSrc);
			}

            if (pModelDest->GetType() == MESHTYPE_POINTLIGHT)
            {
                KG3DModelPointLight* pSrcLight = static_cast<KG3DModelPointLight*>(pModelSrc);
                KG3DModelPointLight* pDstLight = static_cast<KG3DModelPointLight*>(pModelDest);

                pDstLight->m_vColor = pSrcLight->m_vColor;
                pDstLight->m_bEnable = pSrcLight->m_bEnable;


            }
            else if (pModelDest->GetType() == MESHTYPE_SPEEDTREE)
            {
                KG3DModelSpeedTree *pSpdTree = static_cast<KG3DModelSpeedTree*>(pModelDest);
                pSpdTree->SetLeafScaling(&pSrcObject->m_Scaling);
            }

		}

	}

	m_Rotation  = pSrcObject->m_Rotation;
    ASSERT_FLOAT_IS_VALIED(m_Rotation.x);

	m_RotationCenter = pSrcObject->m_RotationCenter;
	m_Scaling = pSrcObject->m_Scaling;
	m_ScalingRotation = pSrcObject->m_ScalingRotation;
	m_ScalingCenter = pSrcObject->m_ScalingCenter;
	m_Translation = pSrcObject->m_Translation;
	m_Matrix = pSrcObject->m_Matrix;
    ASSERT_FLOAT_IS_VALIED(m_Matrix._11);

    m_bGroupPoseValidate = pSrcObject->m_bGroupPoseValidate;
    m_vGroupTrans = pSrcObject->m_vGroupTrans;
    m_vGroupScal = pSrcObject->m_vGroupScal;
    m_qGroupRota = pSrcObject->m_qGroupRota;

    m_pMatParent = pSrcObject->m_pMatParent;
    m_pParentObj = pSrcObject->m_pParentObj;

	m_eState = pSrcObject->m_eState;
	m_fAlpha = pSrcObject->GetAlpha();

    m_scName = pSrcObject->m_scName;

	UpdateTransformation();
	return S_OK;
}

HRESULT KG3DRepresentObject::GetModelFileName(
    int nIndex,LPSTR pMeshFile,LPSTR pMtlFile,LPSTR pAniName)
{
	if(nIndex>=(int)m_vecModel.size())
		return E_FAIL;
	KG3DModel* pModel = m_vecModel[nIndex];
	if(!pModel)
		return E_FAIL;

	pModel->GetMeshFileName(pMeshFile);
	pModel->GetMaterialFileName(pMtlFile);
	pModel->GetAnimationFileName(pAniName, MAX_PATH);
	return S_OK;
}

HRESULT KG3DRepresentObject::GetName(LPTSTR pName, size_t BufferSize)
{
	if(m_vecModel.size()==0)
	{
		_stprintf_s(pName, BufferSize, "物件%d",m_dwID);
	}
	else
	{
		KG3DModel* pModel = m_vecModel[0];
		if(!pModel)
		{
			_stprintf_s(pName, BufferSize, "物件%d",m_dwID);
		}
		else if (pModel->GetType() == MESHTYPE_MODELST)
		{
			TCHAR strFile[MAX_PATH];
			TCHAR strExt[MAX_PATH];
			_tsplitpath(pModel->m_scName.c_str(), NULL, NULL, strFile, strExt);
			_stprintf_s(pName, BufferSize, "%s%s", strFile, strExt);
		}
		else
		{
            TCHAR path_name[MAX_PATH];
            TCHAR drive[MAX_PATH];
            TCHAR dir[MAX_PATH];
            TCHAR fname[MAX_PATH];
            TCHAR ext[MAX_PATH];
            pModel->GetMeshFileName(path_name);
            _tsplitpath(path_name, drive, dir, fname, ext);
			_stprintf_s(pName, BufferSize, "%s%s", fname, ext);
		}
	}
	return S_OK;
}

BOOL KG3DRepresentObject::IsSecondUVExist()
{
    for(size_t i=0;i<m_vecModel.size();i++)
    {
        KG3DModel* pModel = m_vecModel[i];

        if (pModel)
        {
            return pModel->IsSecondUVExist();
        }
    }
    return false;
}

HRESULT KG3DRepresentObject::RenderSelfShadow(LPD3DXEFFECT &lpEffect,const D3DXMATRIX &matVP,
                                              const D3DXMATRIX &matWVP,LPDIRECT3DTEXTURE9 lpDepthMap)
{
    for(size_t i=0;i<m_vecModel.size();i++)
    {
        KG3DModel* pModel = m_vecModel[i];

        if (pModel)
        {
            pModel->RenderSelfShadow(lpEffect,matVP,matWVP,lpDepthMap);
        }
    }

    return S_OK;
}

HRESULT KG3DRepresentObject::RenderShadowMap(LPD3DXEFFECT &lpEffect,const D3DXMATRIX &matVP,const D3DXMATRIX &matWVP)
{
	for(size_t i=0;i<m_vecModel.size();i++)
	{
		KG3DModel* pModel = m_vecModel[i];

		if (pModel)
		{
			static D3DXMATRIX mat;
			mat = pModel->m_matWorld * matVP;
			pModel->RenderShadowMap(lpEffect,matVP,mat);
		}
	}

    return S_OK;
}


HRESULT KG3DRepresentObject::RenderPlaneShadow(D3DXPLANE& plane,const D3DXVECTOR4& LightDir)
{
	D3DXMATRIX matSave,matShadow;
	D3DXMatrixShadow(&matShadow,&LightDir,&plane);

	g_pd3dDevice->GetTransform(D3DTS_WORLD, &matSave);
	D3DXMATRIX Mat = m_matWorld*matSave*matShadow;
	g_pd3dDevice->SetTransform(D3DTS_WORLD,&Mat);

	GraphicsStruct::RenderStateCullMode CullMode(D3DCULL_NONE);

	GraphicsStruct::RenderState RsFog(D3DRS_FOGENABLE, FALSE);

	DWORD dwLight = 0;
	g_pd3dDevice->GetRenderState(D3DRS_LIGHTING, &dwLight);
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	D3DMATERIAL9 mtlSave,mtl;
	g_pd3dDevice->GetMaterial(&mtlSave);
	mtl.Ambient.r=mtl.Ambient.g=mtl.Ambient.b=1.0F;mtl.Ambient.a=1.0F;
	mtl.Emissive.r=mtl.Emissive.g=mtl.Emissive.b=mtl.Emissive.a=0.0F;
	mtl.Diffuse = mtl.Ambient;
	mtl.Specular = mtl.Emissive;
	g_pd3dDevice->SetMaterial(&mtl);
	BOOL LightEnable[8];
	for(int i=0;i<8;i++)
	{
		g_pd3dDevice->GetLightEnable(i,&LightEnable[i]);
		g_pd3dDevice->LightEnable(i,FALSE);
	}
	g_pd3dDevice->SetTexture(0,NULL);
	g_pd3dDevice->SetTexture(1,NULL);

	for(size_t i=0;i<m_vecModel.size();i++)
	{
		KG3DModel* pModel = m_vecModel[i];

		if (pModel)
		{
			pModel->Render(RENDER_SHADOW, NULL);
		}
	}

	g_pd3dDevice->SetTransform(D3DTS_WORLD, &matSave);
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, dwLight);
	g_pd3dDevice->SetMaterial(&mtlSave);
	for(int i=0;i<8;i++)
	{
		g_pd3dDevice->LightEnable(i,LightEnable[i]);
	}
	return S_OK;
}

DWORD KG3DRepresentObject::GetModelCount()
{
	return (DWORD)m_vecModel.size();
}

HRESULT KG3DRepresentObject::GetModel(IKG3DModel*& pModel,DWORD dwIndex)
{
	if(dwIndex<m_vecModel.size())
	{
		pModel = m_vecModel[dwIndex];
		if(pModel)
			return S_OK;
		else
			return E_FAIL;
	}
	else
		return E_FAIL;
}

HRESULT KG3DRepresentObject::GetModel(KG3DModel*& pModel,DWORD dwIndex,BOOL bLod)
{
	if(bLod)
	{
		switch(m_nLODLevel)
		{
		case EM_LOD_LEVEL_HIGH:
			{
				if(dwIndex<m_vecModel.size())
				{
					pModel = m_vecModel[dwIndex];
					if(pModel)
						return S_OK;
					else
						return E_FAIL;
				}
				else
					return E_FAIL;
			}
			break;
		case EM_LOD_LEVEL_MIDDLE:
			{
				if(dwIndex<m_vecModelM.size())
				{
					pModel = m_vecModelM[dwIndex];
					if(pModel)
						return S_OK;
					else
						return E_FAIL;
				}
				else
					return E_FAIL;
			}
			break;
		case EM_LOD_LEVEL_LOW:
			{
				if(dwIndex<m_vecModelL.size())
				{
					pModel = m_vecModelL[dwIndex];
					if(pModel)
						return S_OK;
					else
						return E_FAIL;
				}
				else
					return E_FAIL;
			}
			break;
		default:
			_ASSERT(0);
		}
	}
	else
	{
		if(dwIndex<m_vecModel.size())
		{
			pModel = m_vecModel[dwIndex];
			if(pModel)
				return S_OK;
			else
				return E_FAIL;
		}
		else
			return E_FAIL;
	}

   return E_FAIL;
}

KG3DModel* KG3DRepresentObject::GetModel( DWORD dwIndex )
{
	switch(m_nLODLevel)
	{
	case EM_LOD_LEVEL_HIGH:
		{
			if (dwIndex < m_vecModel.size())
			{
				return m_vecModel[dwIndex];
			}
		}
		break;
	case EM_LOD_LEVEL_MIDDLE:
		{
			if (dwIndex < m_vecModelM.size())
			{
				return m_vecModelM[dwIndex];
			}
		}
		break;
	case EM_LOD_LEVEL_LOW:
		{
			if (dwIndex < m_vecModelL.size())
			{
				return m_vecModelL[dwIndex];
			}
		}
		break;
	default:
		_ASSERT(0);
	}

	return NULL;
}

KG3DModel* KG3DRepresentObject::GetModel( DWORD dwIndex,INT nLODLevel)
{
	switch(nLODLevel)
	{
	case EM_LOD_LEVEL_HIGH:
		{
			if (dwIndex < m_vecModel.size())
			{
				return m_vecModel[dwIndex];
			}
		}
		break;
	case EM_LOD_LEVEL_MIDDLE:
		{
			if (dwIndex < m_vecModelM.size())
			{
				return m_vecModelM[dwIndex];
			}
		}
		break;
	case EM_LOD_LEVEL_LOW:
		{
			if (dwIndex < m_vecModelL.size())
			{
				return m_vecModelL[dwIndex];
			}
		}
		break;
	default:
		_ASSERT(0);
	}

	return NULL;
}
HRESULT KG3DRepresentObject::SetLOD(float fLod)
{
	for(size_t i=0;i<m_vecModel.size();i++)
	{
		KG3DModel* pModel = m_vecModel[i];
		if(!pModel)
			continue;
		pModel->SetLOD(fLod);
	}
	return S_OK;
}

HRESULT KG3DRepresentObject::ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam)	//<PROCESS_MESSAGE>
{
	return S_OK;
}

HRESULT KG3DRepresentObject::BindSFXSuedeFromFile(
    LPCSTR pstrBindFileName, IKG3DScene* pActiveScene)
{
    for (size_t i = 0; i < m_vecModel.size(); ++i)
    {
        if (m_vecModel[i])
            m_vecModel[i]->BindSFXSuedeFromFile(pstrBindFileName, pActiveScene);
    }
    return S_OK;
}

HRESULT KG3DRepresentObject::Reload()
{
    for (size_t i = 0; i < m_vecModel.size(); ++i)
    {
        if (FAILED(m_vecModel[i]->Reload()))
            return E_FAIL;
    }
    return S_OK;
}

void KG3DRepresentObject::GetProfile(DWORD& dwNumFacesTotal,DWORD& dwNumVertices,DWORD& dwNumTextures,DWORD& dwNumID)
{
	for (size_t i = 0; i < m_vecModel.size(); ++i)
	{
		if(m_vecModel[i])
		{
			m_vecModel[i]->GetProfile(dwNumFacesTotal,dwNumVertices,dwNumTextures,dwNumID);
		}
	}
}

void KG3DRepresentObject::SetAlpha(float fAlpha)
{
	m_fAlpha = fAlpha;

	for (size_t i = 0; i < m_vecModel.size(); ++i)
	{
		if(m_vecModel[i])
		{
			m_vecModel[i]->SetAlpha(fAlpha, TRUE);
		}
	}
}

DWORD KG3DRepresentObject::GetSceneLayerID()
{
	return m_dwSceneLayerID;
}

void  KG3DRepresentObject::SetSceneLayerID(DWORD dwID)
{
	m_dwSceneLayerID = dwID;
}

HRESULT KG3DRepresentObject::BindModel(LPCSTR strBindTo,
		KG3DModel* pModel)
{
	HRESULT hr = E_FAIL;
	const TCHAR* pResult = NULL;
	BindExtraInfo BindExtInfo;

	KG_PROCESS_ERROR(strBindTo);

	pResult = strstr(strBindTo, "S_" );
	if (pResult)
	{
		for (size_t i = 0; i < m_vecModel.size(); i++)
		{
			KG3DModel* pParentModel = m_vecModel[i];

			if (pParentModel)
			{
				int nReturn = pParentModel->FindSocket(strBindTo, &BindExtInfo);
				if (nReturn != -1)
				{
					hr = pModel->BindToSocket(BindExtInfo.pModel, strBindTo);
					if (FAILED(hr))
					{
						KGLogPrintf(KGLOG_ERR, "KG3DRepresentObject::BindModel(%s) failed.\n", strBindTo);
					}
				}
			}
		}
	}
	else
	{
		for (size_t i = 0; i < m_vecModel.size(); i++)
		{
			KG3DModel* pParentModel = m_vecModel[i];

			if (pParentModel)
			{
				int nReturn = pParentModel->FindBone(strBindTo, &BindExtInfo);
				if (nReturn != -1)
				{
					hr = pModel->BindToBone(BindExtInfo.pModel, strBindTo);
					if (FAILED(hr))
					{
						KGLogPrintf(KGLOG_ERR, "KG3DRepresentObject::BindModel(%s) failed.\n", strBindTo);
					}
				}
			}
		}
	}
	hr = S_OK;
Exit0:
	return hr;
}

HRESULT KG3DRepresentObject::ResetGroupPose()
{
    if (!m_bGroupPoseValidate)
        return E_FAIL;

    SetScaling(&m_vGroupScal);
    SetRotation(&m_qGroupRota);
    SetTranslation(&m_vGroupTrans);

    OnTransformationChange(g_cGraphicsTool.GetCurScene());
    FrameMove();
    ComputeBBox();

    return S_OK;
}

void KG3DRepresentObject::GetBspModel(vector<KG3DModel*>* pvModel)
{
    if (!pvModel)
        return;

    for (size_t i = 0; i < m_vecModel.size(); ++i)
    {
        KG3DModel* pModel = m_vecModel[i];

        if (!pModel)
            continue;

        KG3DMesh* pMesh = pModel->GetMesh();

        if (!pMesh)
            continue;

        KG3DBsp* pBsp = pMesh->GetBsp();

        if (!pBsp)
            continue;

        pvModel->push_back(pModel);
    }
}

HRESULT KG3DRepresentObject::SaveToIniFile(LPCTSTR pFileName,DWORD dwOption)
{
	HRESULT hr = E_FAIL;
	IIniFile* pIniFile = NULL;

	KGLOG_PROCESS_ERROR(pFileName);

	pIniFile = g_OpenIniFile(pFileName);
	if (!pIniFile)
	{
		IFile* pFile = g_CreateFile(pFileName);
		KG_COM_RELEASE(pFile);
		pIniFile = g_OpenIniFile(pFileName,false,true);
	}
	KGLOG_PROCESS_ERROR(pIniFile);

	pIniFile->Clear();
	//////////////////////////////////////////////////////////////////////////
	for(DWORD i=0;i<m_vecModel.size();i++)
	{
		KG3DModel* pModel = NULL;
		hr = GetModel(pModel,i,FALSE);
		KGLOG_COM_PROCESS_ERROR(hr);

		if(pModel)
		{
			TCHAR SectionName[MAX_PATH];
			TCHAR MeshName[MAX_PATH];
			TCHAR MtlName[MAX_PATH];
			TCHAR AniName[MAX_PATH];
			TCHAR RetMeshName[MAX_PATH];
			TCHAR RetMtlName[MAX_PATH];
			TCHAR RetAniName[MAX_PATH];

			D3DXVECTOR3    vModelTrans;
			D3DXVECTOR3    vModelScale;
			D3DXQUATERNION  vModelRot;
			//////////////////////////////////////////////////////////////////////////
			hr = GetModelFileName(i,MeshName,MtlName,AniName);
			KGLOG_COM_PROCESS_ERROR(hr);

			pModel->GetTranslation(&vModelTrans);
			pModel->GetScaling(&vModelScale);
			pModel->GetRotation(&vModelRot);

			wsprintf(SectionName,"Model%d",i);
			g_pFileFinder->GetRelativePath(g_Def_WorkDirectory,MeshName,RetMeshName);
			g_pFileFinder->GetRelativePath(g_Def_WorkDirectory,MtlName,RetMtlName);
			g_pFileFinder->GetRelativePath(g_Def_WorkDirectory,AniName,RetAniName);

			pIniFile->WriteString(SectionName,"MeshName",RetMeshName);
			pIniFile->WriteString(SectionName,"MtlName",RetMtlName);
			pIniFile->WriteString(SectionName,"AniName",RetAniName);

			pIniFile->WriteMultiFloat(SectionName,"Translation",(float*)&vModelTrans,3);
			pIniFile->WriteMultiFloat(SectionName,"Scaling",(float*)&vModelScale,3);
			pIniFile->WriteMultiFloat(SectionName,"Rotation",(float*)&vModelRot,4);
			//////////////////////////////////////////////////////////////////////////
			//Extend information
			ModelInformation* pInfo = GetModelInformation(i);
			if(pInfo)
			{
				TCHAR RetSource[256];
				g_pFileFinder->GetRelativePath(g_Def_WorkDirectory,pInfo->szSource.c_str(),RetSource);

				pIniFile->WriteString(SectionName,"SourceName",RetSource);
				pIniFile->WriteInteger(SectionName,"SourceType",pInfo->nTypeSource);
				pIniFile->WriteInteger(SectionName,"SourceIndex",pInfo->nIndex);
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////

    // group pose
    pIniFile->WriteMultiFloat("GroupPose","Translation",(float*)&m_Translation,3);
    pIniFile->WriteMultiFloat("GroupPose","Scaling",(float*)&m_Scaling,3);
    pIniFile->WriteMultiFloat("GroupPose","Rotation",(float*)&m_Rotation,4);



	pIniFile->Save(pFileName);
	KG_COM_RELEASE(pIniFile);
	return S_OK;
Exit0:
	KG_COM_RELEASE(pIniFile);
	return E_FAIL;
}

HRESULT KG3DRepresentObject::LoadFromIniFile(LPCTSTR pFileName,DWORD dwOption)
{
	HRESULT hr = E_FAIL;
	IIniFile* pIniFile = NULL;
	int i = 0;

	KGLOG_PROCESS_ERROR(pFileName);

	pIniFile = g_OpenIniFile(pFileName);
	KGLOG_PROCESS_ERROR(pIniFile);
	//////////////////////////////////////////////////////////////////////////
	while(1)
	{
		TCHAR SectionName[256];
		wsprintf(SectionName,"Model%d",i);
		if(pIniFile->IsSectionExist(SectionName))
		{
			TCHAR MeshName[256];
			TCHAR MtlName[256];
			TCHAR AniName[256];

			pIniFile->GetString(SectionName, "MeshName", "", MeshName, 256);
			pIniFile->GetString(SectionName, "MtlName", "", MtlName, 256);
			pIniFile->GetString(SectionName, "AniName", "", AniName, 256);

			D3DXVECTOR3 vModelTrans;
			D3DXVECTOR3 vModelScale;
			D3DXQUATERNION vModelRot;

			pIniFile->GetMultiFloat(SectionName, "Translation",(float*)&vModelTrans,3);
			pIniFile->GetMultiFloat(SectionName, "Scaling",(float*)&vModelScale,3);
			pIniFile->GetMultiFloat(SectionName, "Rotation",(float*)&vModelRot,4);

			hr = SetModel(i, MeshName, MtlName, AniName,TRUE);
			KGLOG_COM_PROCESS_ERROR(hr);

			KG3DModel* pModel = NULL;
			hr = GetModel(pModel,i,FALSE);
			KGLOG_COM_PROCESS_ERROR(hr);

			KGLOG_PROCESS_ERROR(pModel);

		/*	pModel->SetTranslation(&vModelTrans);
			pModel->SetScaling(&vModelScale);
			pModel->SetRotation(&vModelRot);*/
			//把低模的位置信息也更新
			SetModelTranslation(i,vModelTrans);
			SetModelScaling(i,vModelScale);
			SetModelRotation(i,vModelRot);
			//////////////////////////////////////////////////////////////////////////
			{
				TCHAR AbsSource[256];
				pIniFile->GetString(SectionName,"SourceName","",AbsSource,256);
				if (AbsSource[0] != '\0')
				{
					ModelInformation Info;
					Info.szSource = AbsSource;
					pIniFile->GetInteger(SectionName,"SourceType",0,&Info.nTypeSource);
					pIniFile->GetInteger(SectionName,"SourceIndex",0,&Info.nIndex);

					SetModelInformation(i,&Info);
				}
			}
		}
		else
			break;

		i++;
	}

    if (pIniFile->IsSectionExist("GroupPose"))
    {
        // group pose
        pIniFile->GetMultiFloat("GroupPose","Translation",(float*)&m_vGroupTrans,3);
        pIniFile->GetMultiFloat("GroupPose","Scaling",(float*)&m_vGroupScal,3);
        pIniFile->GetMultiFloat("GroupPose","Rotation",(float*)&m_qGroupRota,4);
        m_bGroupPoseValidate = TRUE;
    }
    else
    {
        m_bGroupPoseValidate = FALSE;
    }


	FrameMove();
	ComputeBBox();
	//////////////////////////////////////////////////////////////////////////
	KG_COM_RELEASE(pIniFile);
	return S_OK;
Exit0:
	KG_COM_RELEASE(pIniFile);
	return E_FAIL;
}

KG3DRepresentObject::ModelInformation* KG3DRepresentObject::GetModelInformation(int nIndex)
{
	if ((nIndex < 0) ||
		(nIndex >= static_cast<int>(m_vecModelInformation.size())))
		return NULL;
	return m_vecModelInformation[nIndex];
}

HRESULT KG3DRepresentObject::SetModelInformation(int nIndex,KG3DRepresentObject::ModelInformation* pInfo)
{
	if (nIndex<0)
		return E_FAIL;
	if(nIndex >= static_cast<int>(m_vecModelInformation.size()))
	{
		m_vecModelInformation.resize(nIndex+1,NULL);
	}

	if(!m_vecModelInformation[nIndex])
	{
		m_vecModelInformation[nIndex] = new ModelInformation;
	}

	(*m_vecModelInformation[nIndex]) = *pInfo;

	return S_OK;
}

HRESULT KG3DRepresentObject::GetModelCount(INT* pValue)
{
	(*pValue) = (int)GetModelCount();
	return S_OK;
}

HRESULT KG3DRepresentObject::SetScaling(const D3DXVECTOR3 *pValue)
{
	KG3DTransformation::SetScaling(pValue);
	for (size_t i = 0; i < m_vecModel.size(); i++)
	{
		if (m_vecModel[i])
		{
			if (m_vecModel[i]->GetType() == MESHTYPE_SPEEDTREE)
			{
				KG3DModelSpeedTree *pSpdTree = static_cast<KG3DModelSpeedTree*>(m_vecModel[i]);
				D3DXVECTOR3 vec3Scale;
				pSpdTree->GetScaling(&vec3Scale);
				vec3Scale = vec3Scale * pValue->x;
				static_cast<KG3DModelSpeedTree*>(m_vecModel[i])->SetLeafScaling(&vec3Scale);
			}
		}
	}
	return S_OK;
}


HRESULT KG3DRepresentObject::ChangeState(OBJECT_STATE eState)
{
	m_fAlphaWhenStateChanged = m_fAlpha;
	m_eState = eState;
	return S_OK;
}

BOOL KG3DRepresentObject::IsResourceReady()
{
	BOOL bReady = TRUE;

	for (size_t i = 0; i < m_vecModel.size(); i++)
	{
		if (m_vecModel[i])
		{
			if (! m_vecModel[i]->IsResourceReady(TRUE) )
			{
				bReady = FALSE;
			}
		}
	}
	return bReady;
}

void KG3DRepresentObject::SinkFunc(INT nSinkID)
{
   /* if (!m_lpScene)		//<SetScene Refactor>
        return;*/	//Object链接的是Mesh，和Scene没关系吧

	if(nSinkID == KG3DEVENT_MESH_LOADED)
	{
		ComputeBBox();

		FireEvent2(KG3DEVENT_MESH_LOADED,0,0,this);
	}
}

void KG3DRepresentObject::SinkFunc2(INT nSinkID,INT nsubSinkID,DWORD dwValue,PVOID pPoint)
{
	;
}

BOOL KG3DRepresentObject::IsLoaded()
{
	for (size_t i = 0; i < m_vecModel.size(); i++)
	{
		if (m_vecModel[i])
		{
			if (! m_vecModel[i]->IsLoaded() )
			{
				return FALSE;
			}
		}
	}
	return TRUE;
}

//HRESULT KG3DRepresentObject::RemoveFromRegions()
//{
//	list<KG3DSceneRegion*>::iterator i = m_listSceneRegion.begin();
//	list<KG3DSceneRegion*>::iterator iend = m_listSceneRegion.end();
//	while (i!=iend)
//	{
//		KG3DSceneRegion* pRegion = *i;
//		pRegion->setObject.erase(this);
//		++i;
//	}
//
//	m_listSceneRegion.clear();
//
//	return S_OK;
//}


HRESULT KG3DRepresentObject::ComputeDynamicBBox()
{
	//KG_PROCESS_SUCCESS(!IsResourceReady());
	if(m_vecModel.empty())//这句话必须有，不然没有Model的话，Box的信息就被搞没有了
		return S_OK;

	m_BBox.Clear();

	D3DXVECTOR3 vPos(m_matWorld._41, m_matWorld._42, m_matWorld._43);
	D3DXVECTOR3 V(50,50,50);
	m_BBox.AddPosition(vPos + V);
	m_BBox.AddPosition(vPos - V);

	for(size_t i=0;i<m_vecModel.size();i++)
	{
		KG3DModel* pModel = m_vecModel[i];
		if (pModel)
		{
			if (pModel->IsAnimationed())
			{
				pModel->ComputeDynamicBBox();
			}
			else
			{
				pModel->ComputeBBox();
			}
			D3DXVECTOR3 BoxPos[8];
			pModel->m_BBox.Extract(BoxPos);
			m_BBox.AddPosition(BoxPos, 8);
		}
	}

	return S_OK;
}

void KG3DRepresentObject::SetParentMatrix(const D3DXMATRIX* pMat)
{
    m_pMatParent = pMat;
}

/*
HRESULT KG3DRepresentObject::OnDeleteFromScene(KG3DSceneSceneEditor* pEditor)
{
	return E_NOTIMPL;
}*/


/*
HRESULT KG3DRepresentObject::OnAddToScene(KG3DSceneSceneEditor* pEditor)
{
	return E_NOTIMPL;
}*/





HRESULT KG3DRepresentObject::OneInputContainer(KG3DSceneEntityContainer* pContainer)
{
	m_setRefrenceContainer.insert(pContainer);
	return S_OK;
}

HRESULT KG3DRepresentObject::RemoveFromContainer()
{
	set<KG3DSceneEntityContainer*>::iterator i = m_setRefrenceContainer.begin();
	set<KG3DSceneEntityContainer*>::iterator iend = m_setRefrenceContainer.end();
	while (i!=iend)
	{
		KG3DSceneEntityContainer* pContainer = *i;
		if(pContainer)
		{
			pContainer->RemoveRepresentObject(this);
			//Release();
		}
		++i;
	}
	m_setRefrenceContainer.clear();
	return S_OK;
}

HRESULT KG3DRepresentObject::OnRemoveFromContainer( KG3DSceneEntityContainer* pContainer )
{
	m_setRefrenceContainer.erase(pContainer);
	return S_OK;
}
HRESULT KG3DRepresentObject::SaveToInformationBuffer(/*DWORD& dwType,DWORD& dwLength,*/KG3DMemoryFile* pFile)
{
	//HRESULT hr = S_OK;

	//这个函数关系到存盘和读取，不能随便改，必须判断Version，根据Version来读写

	CHAR MeshName[MAX_PATH];
	CHAR MtlName [MAX_PATH];
	CHAR AniName [MAX_PATH];
	CHAR SourceName[MAX_PATH];
	D3DXVECTOR3 vScaling;
	D3DXVECTOR3 vScalingCenter;
	D3DXQUATERNION vScalingRotation;
	D3DXQUATERNION vRotation;
	D3DXVECTOR3 vRotaionCenter;
	D3DXVECTOR3 vTranslation;
    D3DCOLORVALUE colorPointLight = {1.f, 1.f, 1.f, 1.f};

	DWORD dwModelCount = (DWORD)m_vecModel.size();
	DWORD dwVersion = 2;

	//pFile->write(&m_dwState, sizeof(m_dwState));
	pFile->write(&dwModelCount,sizeof(DWORD));

	pFile->write(&dwVersion,sizeof(DWORD));

	for (DWORD i=0;i<dwModelCount;i++)
	{
		ZeroMemory(MeshName,sizeof(MeshName));
		ZeroMemory(MtlName,sizeof(MtlName));
		ZeroMemory(AniName,sizeof(AniName));

		vScaling = D3DXVECTOR3(1,1,1);
		vScalingCenter = D3DXVECTOR3(0,0,0);
		vScalingRotation = D3DXQUATERNION(0,0,0,0);
		vRotation = D3DXQUATERNION(0,0,0,0);
		vRotaionCenter = D3DXVECTOR3(0,0,0);
		vTranslation = D3DXVECTOR3(0,0,0);

		GetModelFileName(i,MeshName,MtlName,AniName);

        TypeInfo* pTypeInfo;
        g_cModelTable.GetTypeInfoByFileName(&pTypeInfo, MeshName);


		KG3DModel* pModel = GetModel(i);
		if (pModel)
		{
			pModel->GetScaling(&vScaling);
			pModel->GetScalingCenter(&vScalingCenter);
			pModel->GetScalingRotation(&vScalingRotation);
			pModel->GetRotation(&vRotation);
			pModel->GetRotationCenter(&vRotaionCenter);
			pModel->GetTranslation(&vTranslation);

            if (pTypeInfo->dwType == MESHTYPE_POINTLIGHT)
            {
                KG3DModelPointLight* pLight = static_cast<KG3DModelPointLight*>(pModel);
                pLight->GetModelColor(0, &colorPointLight);
            }

		}

		pFile->write(MeshName,sizeof(CHAR) * MAX_PATH);
		pFile->write(MtlName,sizeof(CHAR) * MAX_PATH);
		pFile->write(AniName,sizeof(CHAR) * MAX_PATH);
		pFile->write(&vScaling,sizeof(D3DXVECTOR3));
		pFile->write(&vScalingCenter,sizeof(D3DXVECTOR3));
		pFile->write(&vScalingRotation,sizeof(D3DXQUATERNION));
		pFile->write(&vRotation,sizeof(D3DXQUATERNION));
		pFile->write(&vRotaionCenter,sizeof(D3DXVECTOR3));
		pFile->write(&vTranslation,sizeof(D3DXVECTOR3));

        // point light info

        if (pTypeInfo->dwType == MESHTYPE_POINTLIGHT)
        {
            pFile->write(&colorPointLight, sizeof(colorPointLight));
        }

	}
	DWORD dwInfoNum = (DWORD)m_vecModelInformation.size();
	pFile->write(&dwInfoNum,sizeof(DWORD));
	for (DWORD i =0;i< dwInfoNum;i++)
	{
		int nTypeSource = 0;
		int nIndex = 0;
		ModelInformation* pInfo = m_vecModelInformation[i];
		if(pInfo)
		{
			nTypeSource = pInfo->nTypeSource;
			nIndex = pInfo->nIndex;
			wsprintf(SourceName,"%s", GetRetPath(pInfo->szSource.c_str()));
		}
		else
		{
			wsprintf(SourceName,"%s","");
		}
		pFile->write(&nTypeSource,sizeof(int));
		pFile->write(&nIndex,sizeof(int));
		pFile->write(SourceName,sizeof(CHAR) * MAX_PATH);
	}

    // version 2
    // group pose
    pFile->write(&m_bGroupPoseValidate, sizeof(m_bGroupPoseValidate));
    pFile->write(&m_vGroupTrans, sizeof(m_vGroupTrans));
    pFile->write(&m_vGroupScal, sizeof(m_vGroupScal));
    pFile->write(&m_qGroupRota, sizeof(m_qGroupRota));



	//dwLength = pFile->GetLength();//用两次GetLength()得到就好了
	return S_OK;
/*
Exit0:
	return E_FAIL;*/

}

HRESULT KG3DRepresentObject::LoadFromInformationBuffer(/*DWORD dwType,DWORD dwLength,*/const KG3DMemoryFile* pFile,DWORD dwOption)
{
	//这个函数关系到存盘和读取，不能随便改，必须判断Version，根据Version来读写

	DWORD dwNowTime = timeGetTime();

	CHAR MeshName[MAX_PATH];
	CHAR MtlName [MAX_PATH];
	CHAR AniName [MAX_PATH];
	D3DXVECTOR3 vScaling;
	D3DXVECTOR3 vScalingCenter;
	D3DXQUATERNION vScalingRotation;
	D3DXQUATERNION vRotation;
	D3DXVECTOR3 vRotaionCenter;
	D3DXVECTOR3 vTranslation;
    D3DCOLORVALUE colorPointLight = {1.f, 1.f, 1.f, 1.f};

	BOOL bForceLoad = TRUE;
	if(dwOption & OBJLOAD_MULTITHEAD)
		bForceLoad = FALSE;
	//////////////////////////////////////////////////////////////////////////
	DWORD dwModelCount = 0;
	DWORD dwVersion = 0;

//	ASSERT(m_vecModel.size()!=0);

	//pFile->read(&m_dwState, sizeof(m_dwState));
	pFile->read(&dwModelCount,sizeof(DWORD));
	pFile->read(&dwVersion,sizeof(DWORD));

	for (DWORD i=0;i<dwModelCount;i++)
	{
		pFile->read(MeshName,sizeof(CHAR) * MAX_PATH);
		pFile->read(MtlName,sizeof(CHAR) * MAX_PATH);
		pFile->read(AniName,sizeof(CHAR) * MAX_PATH);
		pFile->read(&vScaling,sizeof(D3DXVECTOR3));
		pFile->read(&vScalingCenter,sizeof(D3DXVECTOR3));
		pFile->read(&vScalingRotation,sizeof(D3DXQUATERNION));
		pFile->read(&vRotation,sizeof(D3DXQUATERNION));
		pFile->read(&vRotaionCenter,sizeof(D3DXVECTOR3));
		pFile->read(&vTranslation,sizeof(D3DXVECTOR3));

        // get type info
        TypeInfo* pTypeInfo = NULL;
        HRESULT hr = g_cModelTable.GetTypeInfoByFileName(&pTypeInfo, MeshName);
		if(FAILED(hr))
		{
			KGLogPrintf(KGLOG_ERR, "KG3DRepresentObject::LoadFromInformationBuffer  %s 取TypeInfo出错",MeshName);
		}
		KGLOG_COM_PROCESS_ERROR(hr);

        if (pTypeInfo && pTypeInfo->dwType == MESHTYPE_POINTLIGHT)
        {
            pFile->read(&colorPointLight, sizeof(colorPointLight));
        }



		SetModel(i,MeshName,MtlName,AniName,bForceLoad);
		KG3DModel* pModel = GetModel(i,EM_LOD_LEVEL_HIGH);
		if(pModel)
		{
			pModel->SetScaling(&vScaling);
			pModel->SetScalingCenter(&vScalingCenter);
			pModel->SetScalingRotation(&vScalingRotation);
			pModel->SetRotation(&vRotation);
			pModel->SetRotationCenter(&vRotaionCenter);
			pModel->SetTranslation(&vTranslation);

			pModel->ComputeBBox();

            if (pTypeInfo->dwType == MESHTYPE_POINTLIGHT)
            {
                KG3DModelPointLight* pLight = static_cast<KG3DModelPointLight*>(pModel);
                pLight->SetModelColor(0, colorPointLight);
            }

		}
		//////////////////////////////////////////////////////
		{
		KG3DModel* pModelM = GetModel(i,EM_LOD_LEVEL_MIDDLE);
		if(pModelM)
		{
			pModelM->SetScaling(&vScaling);
			pModelM->SetScalingCenter(&vScalingCenter);
			pModelM->SetScalingRotation(&vScalingRotation);
			pModelM->SetRotation(&vRotation);
			pModelM->SetRotationCenter(&vRotaionCenter);
			pModelM->SetTranslation(&vTranslation);

			pModelM->ComputeBBox();

			if (pTypeInfo->dwType == MESHTYPE_POINTLIGHT)
			{
				KG3DModelPointLight* pLight = static_cast<KG3DModelPointLight*>(pModelM);
				pLight->SetModelColor(0, colorPointLight);
			}

		}
		}
		/////////////////////////////////////////////////////////////
		{
		KG3DModel* pModelL = GetModel(i,EM_LOD_LEVEL_LOW);
		if(pModelL)
		{
			pModelL->SetScaling(&vScaling);
			pModelL->SetScalingCenter(&vScalingCenter);
			pModelL->SetScalingRotation(&vScalingRotation);
			pModelL->SetRotation(&vRotation);
			pModelL->SetRotationCenter(&vRotaionCenter);
			pModelL->SetTranslation(&vTranslation);

			pModelL->ComputeBBox();

			if (pTypeInfo->dwType == MESHTYPE_POINTLIGHT)
			{
				KG3DModelPointLight* pLight = static_cast<KG3DModelPointLight*>(pModelL);
				pLight->SetModelColor(0, colorPointLight);
			}

		}
		}
	}
	if(dwVersion >= 1)
	{
		DWORD dwNum =0;
		TCHAR SourceName[MAX_PATH];
		pFile->read(&dwNum,sizeof(DWORD));

		for(DWORD i=0;i<dwNum;i++)
		{
			ModelInformation ModelInfo;
			pFile->read(&(ModelInfo.nTypeSource),sizeof(int));
			pFile->read(&(ModelInfo.nIndex),sizeof(int));
			pFile->read(&SourceName,sizeof(CHAR) * MAX_PATH);
			ModelInfo.szSource = SourceName;
			SetModelInformation(i,&ModelInfo);
		}
	}

    if (dwVersion >= 2)
    {
        pFile->read(&m_bGroupPoseValidate, sizeof(m_bGroupPoseValidate));
        pFile->read(&m_vGroupTrans, sizeof(m_vGroupTrans));
        pFile->read(&m_vGroupScal, sizeof(m_vGroupScal));
        pFile->read(&m_qGroupRota, sizeof(m_qGroupRota));
    }




	ComputeBBox();

	DWORD dwCost = timeGetTime() - dwNowTime;
	if(dwCost >= 50)
	{
		//KGLogPrintf(KGLOG_ERR, "%d KG3DRepresentObject::LoadFromInformationBuffer %s.",dwCost,MeshName);
	}

#if defined(DEBUG) | defined(_DEBUG)
	float LA = D3DXVec3Length(&m_BBox.A);
	float LB = D3DXVec3Length(&m_BBox.B);

	ASSERT(LA>1 || LB >1);
#endif
	return S_OK;
Exit0:
	return E_FAIL;
}
HRESULT KG3DRepresentObject::RefreshOption(unsigned uOption)
{
	return S_OK;
}

HRESULT KG3DRepresentObject::Load( LPCTSTR lpFileName, DWORD_PTR wParam, DWORD_PTR lParam )
{
	TCHAR path_buffer[256];
	TCHAR drive[256];
	TCHAR dir[256];
	TCHAR fname[256];
	TCHAR ext[256];
	TCHAR strAnimationFile[MAX_PATH];
	KG3DClip* pClip = NULL;
	strAnimationFile[0] = '\0';
	DWORD dwOption = _kg_to_smaller_size_cast<DWORD>(wParam);
	BOOL  bForceLoad = TRUE;
	if(dwOption & OBJLOAD_MULTITHEAD)
		bForceLoad = FALSE;

	KG_PROCESS_ERROR(NULL != lpFileName);
	{
		_stprintf_s(path_buffer,_T("%s"),lpFileName);
		_tsplitpath_s(path_buffer, drive, dir, fname, ext );

		//Attempt to load a default animation
		_stprintf_s(strAnimationFile, "%s%s%s.ani", drive, dir, fname);
		HRESULT hr = g_cClipTable.LoadResourceFromFile(strAnimationFile, 0, 0, &pClip);
		if (FAILED(hr))
		{
			strAnimationFile[0] = '\0';
		}
		SAFE_RELEASE(pClip);

		if(_tcsicmp(fname,_T("")) != 0)
		{
			if (_tcsicmp(ext, _T(".group")) == 0 || _tcsicmp(ext, _T(".ini")) == 0)
			{
				hr = this->LoadFromIniFile(lpFileName, dwOption);//Group型的Obj会用这种形式
			}
			else
			{
				hr = this->SetModel(0,lpFileName,"",strAnimationFile,bForceLoad);
				KG_COM_PROCESS_ERROR(hr);
			}
		}

        m_scName = lpFileName;

		return S_OK;
	}



Exit0:
	SAFE_RELEASE(pClip);
	return E_FAIL;
}

//HRESULT KG3DRepresentObject::SetScene( KG3DScene* pScene )//<SetScene Refactor>
//{
//	m_lpScene = pScene;
//	return S_OK;
//}

HRESULT KG3DRepresentObject::SetTranslation(const D3DXVECTOR3* pValue )
{
	KG3DTransformation::SetTranslation(pValue);
	return S_OK;
}

HRESULT KG3DRepresentObject::SetRotation( const D3DXQUATERNION* pRotaion )
{
	KG3DTransformation::SetRotation(pRotaion);
	return S_OK;
}

//HRESULT KG3DRepresentObject::SetScaling( const D3DXVECTOR3* pScale )
//{
//	KG3DTransformation::SetScaling(pScale);
//	return S_OK;
//}

void KG3DRepresentObject::SetPhysicsScene(KG3DPhysiscScene *pScene)
{
	size_t uSize = m_vecModel.size();
	for (size_t i = 0; i < uSize; i++)
	{
		KG3DModel *pModel = m_vecModel[i];
		if (pModel)
		{
			pModel->SetPhysicsScene(pScene);
		}
	}
}

//HRESULT KG3DRepresentObject::AddSceneRegion( KG3DSceneRegion* pRegion )
//{
//	m_listSceneRegion.push_back(pRegion);
//	return S_OK;
//}

//HRESULT KG3DRepresentObject::ClearSceneRegionList()
//{
//	m_listSceneRegion.clear();
//	return S_OK;
//}

int KG3DRepresentObject::GetDynamicObstacleGroupIndex()
{
	return m_nDynamicObstacleGroupIndex;//INVALID_OBSTACLE_GROUP;//原来这里返回INVALID_OBSTACLE_GROUP，这样引擎里面就需要包含这个值的定义文件了，但这样引入和超多耦合和环形依赖，所以还是把值设进来比较好。
}
HRESULT KG3DRepresentObject::SetDynamicObstacleGroupIndex(int nindex)
{
	m_nDynamicObstacleGroupIndex = nindex;
	return S_OK;
}

//BOOL KG3DRepresentObject::GetNearRayIntersect( const D3DXVECTOR3& vSrc, const D3DXVECTOR3& vNormalizedDir , FLOAT* pfRet )
//{
//	return this->RayIntersection(vSrc, vNormalizedDir, pfRet);
//}

const AABBOX& KG3DRepresentObject::GetBBox()
{
	return m_BBox;
}
#if 0
KG3DRenderSorterStratege& KG3DRepresentObject::GetRenderSorterStratege()
{
	struct KG3DRenderSorterStrategeMultiModelObj : public KG3DRenderSorterStratege
	{
		virtual HRESULT PutIntoRenderData(KG3DRepresentObject& Obj, KG3DRenderData& Data)
		{
			return E_FAIL;
		}
	};
	static KG3DRenderSorterStrategeDefault st1;
	static KG3DRenderSorterStrategeMultiModelObj st2;
	if(m_vecModel.size() == 0)
		return st1;
	return st2;
}
#endif

size_t KG3DRepresentObject::GetSubModels(vector<KG3DModel*>& vModels)
{
    size_t s = vModels.size();

    for (size_t i = 0; i < m_vecModel.size(); ++i)
        if (m_vecModel[i])
            vModels.push_back(m_vecModel[i]);

    return vModels.size() - s;
}

HRESULT KG3DRepresentObject::SetLowModel(DWORD dwIndex,const char scFileName[],const char scMtlName[],DWORD dwOption )
{
	HRESULT hr = S_OK;
	KG3DModel* pModel = NULL;
//	TCHAR strModleName[MAX_PATH];
	KG_PROCESS_ERROR(scFileName);
	KG_PROCESS_ERROR(g_IsFileExist(scFileName));

	if(dwIndex>=m_vecModelL.size())
	{
		m_vecModelL.resize(dwIndex+1,NULL);
	}
	pModel = m_vecModelL[dwIndex];
	if(pModel)
	{
		pModel->Unadvise(this);
		pModel->SetPhysicsScene(NULL);
	}
	SAFE_RELEASE(pModel);
	m_vecModelL[dwIndex] = NULL;

	hr = g_cModelTable.LoadResourceFromFile(scFileName,
		0,
		dwOption,
		(IKG3DResourceBase **)&pModel);
	KG_COM_PROCESS_ERROR(hr);

	//pModel->LoadMaterialFromFile(scMtlName,dwOption);

	pModel->Advise(this);
	m_vecModelL[dwIndex] = pModel;

	pModel->SetParentObjMatrix(&m_matWorld);
	m_dwLODExist |= EM_LOD_EXIST_LOW;
	return S_OK;
Exit0:
	m_dwLODExist &= ~EM_LOD_EXIST_LOW;
	//m_nLODLevel = EM_LOD_LEVEL_HIGH;
	return E_FAIL;
}


HRESULT KG3DRepresentObject::SetMiddleModel(DWORD dwIndex,const char scFileName[],const char scMtlName[],DWORD dwOption )
{
	HRESULT hr = S_OK;
	KG3DModel* pModel = NULL;
	//TCHAR strModleName[MAX_PATH];
	KG_PROCESS_ERROR(scFileName);
	KG_PROCESS_ERROR(g_IsFileExist(scFileName));

	if(dwIndex>=m_vecModelM.size())
	{
		m_vecModelM.resize(dwIndex+1,NULL);
	}
	pModel = m_vecModelM[dwIndex];
	if(pModel)
	{
		pModel->Unadvise(this);
		pModel->SetPhysicsScene(NULL);
	}
	SAFE_RELEASE(pModel);
	m_vecModelM[dwIndex] = NULL;

	hr = g_cModelTable.LoadResourceFromFile(scFileName,
		0,
		dwOption,
		(IKG3DResourceBase **)&pModel);
	KG_COM_PROCESS_ERROR(hr);

	//pModel->LoadMaterialFromFile(scMtlName,dwOption);

	pModel->Advise(this);
	m_vecModelM[dwIndex] = pModel;

	pModel->SetParentObjMatrix(&m_matWorld);
	m_dwLODExist |= EM_LOD_EXIST_MIDDLE;
	return S_OK;
Exit0:
	m_dwLODExist &= ~EM_LOD_EXIST_MIDDLE;
	//m_nLODLevel = EM_LOD_LEVEL_HIGH;
	return E_FAIL;
}

HRESULT KG3DRepresentObject::CheckLODLevel()
{

	if(m_dwLODExist == EM_LOD_EXIST_NONE)
		return S_OK;
	m_dwLODFrameCount++;
	if(m_dwLODFrameCount < emCheckLODFrameGap )
		return S_OK;
	{
		m_dwLODFrameCount = 0;
		D3DXVECTOR3 CameraPos,vDis;
		D3DXVECTOR3 vPos(m_matWorld._41,m_matWorld._42,m_matWorld._43);
		float fDistance = 0.0f;
		KG3DCamera* pCamera = NULL;
        int nLODLevel = EM_LOD_LEVEL_HIGH;
        KG3DModel *pModel = NULL;
        vector<KG3DModel*>::iterator it, itEnd;
		pCamera = g_cGraphicsTool.GetSceneMainCamera();
		KG_PROCESS_ERROR(pCamera);
		pCamera->GetPosition(&CameraPos);
		vDis =CameraPos - vPos;
		fDistance = D3DXVec3LengthSq(&vDis);
		
		if((fDistance > g_cEngineOption.fModelLODSwitchGates[EM_LOD_MIDDLE_TO_LOW]) && (m_dwLODExist & EM_LOD_EXIST_LOW))
		{
			nLODLevel = EM_LOD_LEVEL_LOW;
		}
		else if (fDistance > (g_cEngineOption.fModelLODSwitchGates[EM_LOD_HIGH_TO_MIDDLE])&& (m_dwLODExist & EM_LOD_EXIST_MIDDLE))
		{
			nLODLevel = EM_LOD_LEVEL_MIDDLE;
		}

        // LOD切换时, 需检查目标等级模型资源是否加载完,
        // 否则会导致场景物件闪烁
        if (m_nLODLevel == nLODLevel)
            goto Exit0;

        switch(nLODLevel)
        {
        case EM_LOD_LEVEL_HIGH:
            it = m_vecModel.begin();
            itEnd = m_vecModel.end();
            break;
        case EM_LOD_LEVEL_MIDDLE:
            it = m_vecModelM.begin();
            itEnd = m_vecModelM.end();
            break;
        case EM_LOD_LEVEL_LOW:
            it = m_vecModelL.begin();
            itEnd = m_vecModelL.end();
            break;
        default:
            KG_ASSERT_EXIT(false);
        }
        BOOL bResourceReady = TRUE;
        for (NULL; it != itEnd; ++it)
        {
            pModel = *it;
            if (pModel)
            {
                if (!pModel->IsResourceReady(TRUE))
                {
                    bResourceReady &= FALSE;
                    pModel->FrameMove();
                }
            }
        }

        if (bResourceReady)
        {
            m_nLODLevel = nLODLevel;
        }
	}
Exit0:
	return S_OK;
}

HRESULT KG3DRepresentObject::SetSelectable( BOOL bSelectable )
{
	if(bSelectable)
		m_dwState |= em_state_selectable;
	else
		m_dwState &= (~em_state_selectable);
	return S_OK;
}

ULONG KG3DRepresentObject::IsSelectable()
{
	return m_dwState &em_state_selectable;
}

HRESULT KG3DRepresentObject::LoadPostProcess()
{
    for (size_t i = 0; i < m_vecModel.size(); ++i)
    {
        if (m_vecModel[i])
            m_vecModel[i]->LoadPostProcess();
    }

    return S_OK;
}

HRESULT KG3DRepresentObject::SetModelTranslation(DWORD dwIndex,D3DXVECTOR3 &vTrans)
{
	KG3DModel* pModel = NULL;
	//设置高模位置
	if(dwIndex<m_vecModel.size())
	{
		pModel = m_vecModel[dwIndex];
		if(pModel)
			pModel->SetTranslation(&vTrans);
	}
	//设置中模位置
	if(dwIndex<m_vecModelM.size())
	{
		pModel = m_vecModelM[dwIndex];
		if(pModel)
			pModel->SetTranslation(&vTrans);
	}
    //设置低模位置
	if(dwIndex<m_vecModelL.size())
	{
		pModel = m_vecModelL[dwIndex];
		if(pModel)
			pModel->SetTranslation(&vTrans);
	}
	return S_OK;
}

HRESULT KG3DRepresentObject::SetModelScaling(DWORD dwIndex,D3DXVECTOR3 &vScale)
{
	KG3DModel* pModel = NULL;
	//设置高模位置
	if(dwIndex<m_vecModel.size())
	{
		pModel = m_vecModel[dwIndex];
		if(pModel)
			pModel->SetScaling(&vScale);
	}
	//设置中模位置
	if(dwIndex<m_vecModelM.size())
	{
		pModel = m_vecModelM[dwIndex];
		if(pModel)
			pModel->SetScaling(&vScale);
	}
	//设置低模位置
	if(dwIndex<m_vecModelL.size())
	{
		pModel = m_vecModelL[dwIndex];
		if(pModel)
			pModel->SetScaling(&vScale);
	}
	return S_OK;
}
HRESULT KG3DRepresentObject::SetModelRotation(DWORD dwIndex,D3DXQUATERNION &qRot)
{
	KG3DModel* pModel = NULL;
	//设置高模位置
	if(dwIndex<m_vecModel.size())
	{
		pModel = m_vecModel[dwIndex];
		if(pModel)
			pModel->SetRotation(&qRot);
	}
	//设置中模位置
	if(dwIndex<m_vecModelM.size())
	{
		pModel = m_vecModelM[dwIndex];
		if(pModel)
			pModel->SetRotation(&qRot);
	}
	//设置低模位置
	if(dwIndex<m_vecModelL.size())
	{
		pModel = m_vecModelL[dwIndex];
		if(pModel)
			pModel->SetRotation(&qRot);
	}
	return S_OK;
}
HRESULT KG3DRepresentObject::UpdateModelTransformation(DWORD dwIndex)
{
	KG3DModel* pModel = NULL;
	//设置高模位置
	if(dwIndex<m_vecModel.size())
	{
		pModel = m_vecModel[dwIndex];
		if(pModel)
			pModel->UpdateTransformation();
	}
	//设置中模位置
	if(dwIndex<m_vecModelM.size())
	{
		pModel = m_vecModelM[dwIndex];
		if(pModel)
			pModel->UpdateTransformation();
	}
	//设置低模位置
	if(dwIndex<m_vecModelL.size())
	{
		pModel = m_vecModelL[dwIndex];
		if(pModel)
			pModel->UpdateTransformation();
	}
	return S_OK;
}
HRESULT KG3DRepresentObject::ComputeModelBBox(DWORD dwIndex)
{
	KG3DModel* pModel = NULL;
	//设置高模位置
	if(dwIndex<m_vecModel.size())
	{
		pModel = m_vecModel[dwIndex];
		if(pModel)
			pModel->ComputeBBox();
	}
	//设置中模位置
	if(dwIndex<m_vecModelM.size())
	{
		pModel = m_vecModelM[dwIndex];
		if(pModel)
			pModel->ComputeBBox();
	}
	//设置低模位置
	if(dwIndex<m_vecModelL.size())
	{
		pModel = m_vecModelL[dwIndex];
		if(pModel)
			pModel->ComputeBBox();
	}
	return S_OK;
}

#if defined(KG_ENABLE_TDD)
KG_TEST_BEGIN(KG3DRepresentObject)
{
	{
		KG3DRepresentObject objectTemp;
		_ASSERTE(! objectTemp.IsSelectable());
		objectTemp.SetSelectable(TRUE);
		_ASSERTE(objectTemp.IsSelectable());
		objectTemp.SetSelectable(FALSE);
		_ASSERTE(! objectTemp.IsSelectable());
	}
}
KG_TEST_END(KG3DRepresentObject)
#endif