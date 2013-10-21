#include "StdAfx.h"
#include "KG3DSceneEntityMgr.h"
#include "KG3DFrustum.h"
#include "kg3dgraphicstool.h"
#include "KG3DSceneLayer.h"
#include "KG3DSceneSceneEditor.h"
#include "KG3DRepresentObject.h"
#include "KG3DModelSpeedTreeRender.h"
#include "KG3DModelSpeedTree.h"
#include "KG3DSFX.h"
#include "KG3DRepresentObjectSoundBall.h"
#include "KG3DEngineManager.h"
#include "..\..\..\Include\IKG3DSoundShell.h"
#include "KG3DModelST.h"
#include "KG3DRepresentObjectPVS.h"
#include "KG3DSFXProjectionTextureMgr.h"
#include "KG3DSceneOutDoorSpaceMgr.h"
#include "KG3DSceneBlock.h"
#include "KG3DTerrainRender.h"
#include "KG3DTerrainBlock.h"
#include "KG3DModelShadowRenderer.h"
#include "KG3DSkinModelRender.h"
#include "KG3DStaticModelRender.h"
#include "KG3DScenePointLightRender.h"
#include "KG3DModelPointLight.h"
#include "KG3DSceneBlock.h"
#include "KG3DMeshSkin.h"
#include "conio.h"
#include "KG3DProfile.h"
#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

BOOL g_bRenderRoadNode = TRUE; //Add by huangjinshou 2008-1-2
BOOL g_bRenderSoundBall = FALSE;
BOOL g_bPlaySoundBall = TRUE;

DWORD g_dwCullCounter = 0; // 为了去掉裁剪使用到的set容器而加了这个计数

extern int g_nLoadCount;
extern int g_nLoadTime ;
extern int g_nGrassVertexBufferFillPerFrame;
extern BOOL g_bAlwayRenderWater;
extern int g_nDepthTexturePackingType;
BOOL   g_bEnableOcclusionCull = FALSE;
BOOL   g_bShowClientSFX = TRUE;
int g_nMDLRenderLimit = 50;

//////////////////////////////////////////////////////////////////////////
//多线程裁剪
extern BOOL volatile g_bThreadExit;

#ifndef KG3DENGINE_DISABLE_MULTITHREAD
KThread g_CullThread;
#endif

KG3DSceneEntityMgr* g_pCurrentMultiThreadCullEntityMgr = NULL;

void g_MultiThreadCullProgress(void * pInput)        //多线程加载主函数
{
#ifndef KG3DENGINE_DISABLE_MULTITHREAD
	while (!g_bThreadExit)
	{
#endif
		if(g_pCurrentMultiThreadCullEntityMgr)
		{
			if(g_pCurrentMultiThreadCullEntityMgr->m_bKickMeOutFromMultiThreadCull)
			{
				g_pCurrentMultiThreadCullEntityMgr->m_bKickMeOutFromMultiThreadCull = FALSE;
				g_pCurrentMultiThreadCullEntityMgr = NULL;
			}
			else
			{
				g_pCurrentMultiThreadCullEntityMgr->ProcessMultiThreadCull();
			}
		}
		
#ifndef KG3DENGINE_DISABLE_MULTITHREAD
		Sleep(30);
	}
#endif
};

HRESULT InitMultiThreadCull()
{
#ifndef KG3DENGINE_DISABLE_MULTITHREAD
	g_CullThread.Create(g_MultiThreadCullProgress,NULL);
#endif

	return S_OK;
}

HRESULT UnInitMultiThreadCull()
{
	g_bThreadExit = TRUE;
#ifndef KG3DENGINE_DISABLE_MULTITHREAD
	g_CullThread.Destroy();
#endif
	return S_OK;
}

HRESULT InputEntityMgrToMultiThreadCull(KG3DSceneEntityMgr* pMgr)
{
	if(!g_pCurrentMultiThreadCullEntityMgr)
	{
		g_pCurrentMultiThreadCullEntityMgr = pMgr;
		return S_OK;
	}
	return E_FAIL;
}

//////////////////////////////////////////////////////////////////////////
bool ModelsortByViewDistance(KG3DModel* pA, KG3DModel* pB)
{
	return pA->m_fViewDistance < pB->m_fViewDistance;
}

//////////////////////////////////////////////////////////////////////////

KG3DWaterCuller::KG3DWaterCuller()
{
    m_pd3dOcclusionQuery = NULL;
    m_lpVertexBuffer = NULL;
    m_lpVertexIndexBuffer = NULL;
}

KG3DWaterCuller::~KG3DWaterCuller()
{
    ASSERT(!m_pd3dOcclusionQuery);
    ASSERT(!m_lpVertexBuffer);
    ASSERT(!m_lpVertexIndexBuffer);
}

HRESULT KG3DWaterCuller::Init()
{
    HRESULT hrResult  = E_FAIL;
    HRESULT hrRetCode = E_FAIL;
    WORD   *pIndexBuffer = NULL;

    ASSERT(!m_pd3dOcclusionQuery);
    ASSERT(!m_lpVertexBuffer);
    ASSERT(!m_lpVertexIndexBuffer);
    ASSERT(g_pd3dDevice);

    hrRetCode = g_pd3dDevice->CreateQuery(D3DQUERYTYPE_OCCLUSION, &m_pd3dOcclusionQuery);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    hrRetCode = g_pd3dDevice->CreateVertexBuffer(
        4 * sizeof(D3DXVECTOR3) * MAX_WATER_BOUNDING_PLANE_COUNT,
        D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
        D3DFVF_XYZ,
        D3DPOOL_DEFAULT,
        &m_lpVertexBuffer,
        NULL
    );
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    hrRetCode = g_pd3dDevice->CreateIndexBuffer(
        6 * sizeof(WORD) * MAX_WATER_BOUNDING_PLANE_COUNT,
        D3DUSAGE_WRITEONLY,
        D3DFMT_INDEX16,
        D3DPOOL_MANAGED,
        &m_lpVertexIndexBuffer,
        NULL
    );
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    m_lpVertexIndexBuffer->Lock(0, 0, (void **)&pIndexBuffer, 0);

    for (WORD i = 0; i < MAX_WATER_BOUNDING_PLANE_COUNT; ++i)
    {
        int nPointIndex = 4 * i;
        WORD *pTempIndex = pIndexBuffer + 6 * i;
        pTempIndex[0] = (WORD)(nPointIndex + 0);
        pTempIndex[1] = (WORD)(nPointIndex + 2);
        pTempIndex[2] = (WORD)(nPointIndex + 1);
        pTempIndex[3] = (WORD)(nPointIndex + 0);
        pTempIndex[4] = (WORD)(nPointIndex + 3);
        pTempIndex[5] = (WORD)(nPointIndex + 2);
    }

    m_lpVertexIndexBuffer->Unlock();

    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        SAFE_RELEASE(m_lpVertexIndexBuffer);
        SAFE_RELEASE(m_lpVertexBuffer);
        SAFE_RELEASE(m_pd3dOcclusionQuery);
    }

    return hrResult;
}

HRESULT KG3DWaterCuller::Uninit()
{
    SAFE_RELEASE(m_lpVertexIndexBuffer);
    SAFE_RELEASE(m_lpVertexBuffer);
    SAFE_RELEASE(m_pd3dOcclusionQuery);

    return S_OK;
}

HRESULT KG3DWaterCuller::BeginAddWaterBoundingPlanes()
{
    m_mapWaterBoundingPlanes.clear();

    return S_OK;
}

HRESULT KG3DWaterCuller::AddWaterBoundingPlanes(const D3DXVECTOR3 &crvLeftBottomPos, const D3DXVECTOR3 &crvRightUpPos)
{
    HRESULT hrResult = E_FAIL;
    int nWaterHeigth = 0;
    map<int, _RECTANGLE>::iterator itFinder;

    ASSERT(abs(crvLeftBottomPos.y - crvRightUpPos.y) < 1.0f);

    nWaterHeigth = (int)(crvLeftBottomPos.y + 0.5); // 四舍五入

    itFinder = m_mapWaterBoundingPlanes.find(nWaterHeigth);
    if (itFinder != m_mapWaterBoundingPlanes.end())
    {
        _RECTANGLE *pBoundingPlane = &(itFinder->second);
        pBoundingPlane->vLeftBottomPos.x = min(pBoundingPlane->vLeftBottomPos.x, crvLeftBottomPos.x);
        pBoundingPlane->vLeftBottomPos.z = min(pBoundingPlane->vLeftBottomPos.z, crvLeftBottomPos.z);
        pBoundingPlane->vRightUpPos.x = max(pBoundingPlane->vRightUpPos.x, crvRightUpPos.x);
        pBoundingPlane->vRightUpPos.z = max(pBoundingPlane->vRightUpPos.z, crvRightUpPos.z);
    }
    else
    {
        _RECTANGLE WaterBoudingPlane;

        WaterBoudingPlane.vLeftBottomPos = crvLeftBottomPos;
        WaterBoudingPlane.vRightUpPos = crvRightUpPos;

        m_mapWaterBoundingPlanes.insert(make_pair(nWaterHeigth, WaterBoudingPlane));
    }

    hrResult = S_OK;
//Exit0:
    return hrResult;
}

HRESULT KG3DWaterCuller::EndAddWaterBoundingPlanes()
{
    HRESULT hrResult = E_FAIL;
    HRESULT hrRetCode = E_FAIL;
    D3DXVECTOR3 *pVertexBuffer = NULL;
    map<int, _RECTANGLE>::const_iterator itIndex, itEnd;
    DWORD dwPlaneCount = (DWORD)m_mapWaterBoundingPlanes.size();
	KGLOG_PROCESS_ERROR(m_lpVertexBuffer);
    KG_PROCESS_SUCCESS((dwPlaneCount == 0) || (dwPlaneCount > MAX_WATER_BOUNDING_PLANE_COUNT));

    hrRetCode = m_lpVertexBuffer->Lock(0, 0, (void **)&pVertexBuffer, D3DLOCK_DISCARD);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    itIndex = m_mapWaterBoundingPlanes.begin();
    itEnd = m_mapWaterBoundingPlanes.end();
    for (int nPlaneIndex = 0; itIndex != itEnd; ++nPlaneIndex, ++itIndex)
    {
        int nPoindIndex = 4 * nPlaneIndex;
        pVertexBuffer[nPoindIndex + 0] = itIndex->second.vLeftBottomPos;
        pVertexBuffer[nPoindIndex + 1] = itIndex->second.vLeftBottomPos;
        pVertexBuffer[nPoindIndex + 2] = itIndex->second.vRightUpPos;
        pVertexBuffer[nPoindIndex + 3] = itIndex->second.vRightUpPos;

        pVertexBuffer[nPoindIndex + 1].x = itIndex->second.vRightUpPos.x;
        pVertexBuffer[nPoindIndex + 3].x = itIndex->second.vLeftBottomPos.x;
    }

    hrRetCode = m_lpVertexBuffer->Unlock();
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

Exit1:
    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KG3DWaterCuller::RenderWaterBoundingPlanes()
{
    HRESULT hrResult  = E_FAIL;
    HRESULT hrRetCode = E_FAIL;
    DWORD dwPlaneCount = (DWORD)m_mapWaterBoundingPlanes.size();
    KG3DRenderState RenderState;

    KG_PROCESS_SUCCESS((dwPlaneCount == 0) || (dwPlaneCount > MAX_WATER_BOUNDING_PLANE_COUNT));

	if(m_pd3dOcclusionQuery)
    {
        RenderState.SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
        RenderState.SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
        RenderState.SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
        RenderState.SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
        RenderState.SetRenderState(D3DRS_COLORWRITEENABLE, 0);
        RenderState.SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
        RenderState.SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        RenderState.SetRenderState(D3DRS_STENCILENABLE, FALSE);
        RenderState.SetRenderState(D3DRS_LIGHTING, FALSE);
	
	    hrRetCode = m_pd3dOcclusionQuery->Issue(D3DISSUE_BEGIN);
        KGLOG_PROCESS_ERROR(hrRetCode == D3D_OK);

	    g_pd3dDevice->SetStreamSource(0, m_lpVertexBuffer, 0, sizeof(D3DXVECTOR3));
	    g_pd3dDevice->SetIndices(m_lpVertexIndexBuffer);
	    g_pd3dDevice->SetFVF(D3DFVF_XYZ);
	    g_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4 * dwPlaneCount, 0, 2 * dwPlaneCount);

	    hrRetCode = m_pd3dOcclusionQuery->Issue(D3DISSUE_END);
        KGLOG_PROCESS_ERROR(hrRetCode == D3D_OK);
	}

Exit1:
    hrResult = S_OK;
Exit0:
    RenderState.Restore();
    
    return hrResult;
}

HRESULT KG3DWaterCuller::QueryResult(BOOL *pbWaterVisible)
{
    HRESULT hrResult = E_FAIL;
    HRESULT hrRetCode = E_FAIL;
    DWORD dwPixelsVisibleCount = 0;
    DWORD dwPlaneCount = (DWORD)m_mapWaterBoundingPlanes.size();

    KGLOG_PROCESS_ERROR(pbWaterVisible);

    if (dwPlaneCount == 0)
    {
        *pbWaterVisible = FALSE;
        goto Exit1;
    }

    if (dwPlaneCount > MAX_WATER_BOUNDING_PLANE_COUNT)
    {
        *pbWaterVisible = TRUE;
        goto Exit1;
    }


    hrRetCode = m_pd3dOcclusionQuery->GetData((void*)&dwPixelsVisibleCount, sizeof(DWORD), D3DGETDATA_FLUSH);
	if (hrRetCode == S_OK)
	    *pbWaterVisible = (dwPixelsVisibleCount > 0);   
    else
        *pbWaterVisible = TRUE;

Exit1:
    hrResult = S_OK;
Exit0:
    return hrResult;
}

//////////////////////////////////////////////////////////////////////////
KG3DOcclusionCuller::KG3DOcclusionCuller()
{
    for (int i = 0; i < MAX_QUERY_COUNT; ++i)
    {
        m_pd3dOcclusionQuery[i] = NULL;
        m_lpBBoxVertexBuffer[i] = NULL;
    }
    m_lpBBoxIndexBuffer = NULL;
}

KG3DOcclusionCuller::~KG3DOcclusionCuller()
{
    ASSERT(!m_lpBBoxIndexBuffer);
 
    for (int i = 0; i < MAX_QUERY_COUNT; ++i)
    {
        ASSERT(!m_pd3dOcclusionQuery[i]);
        ASSERT(!m_lpBBoxVertexBuffer[i]);
    }
}

HRESULT KG3DOcclusionCuller::Init()
{
    HRESULT hrResult = E_FAIL;
    HRESULT hrRetCode = E_FAIL;
    WORD *pIndexBuffer = NULL;

    ASSERT(g_pd3dDevice);

    for (int i = 0; i < MAX_QUERY_COUNT; ++i)
    {
        hrRetCode = g_pd3dDevice->CreateQuery(D3DQUERYTYPE_OCCLUSION, &m_pd3dOcclusionQuery[i]);
        KGLOG_COM_PROCESS_ERROR(hrRetCode);
    }

    for (int i = 0; i < MAX_QUERY_COUNT; ++i)
    {
        hrRetCode = g_pd3dDevice->CreateVertexBuffer(
            8 * sizeof(D3DXVECTOR3),
            D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
            D3DFVF_XYZ,
            D3DPOOL_DEFAULT,
            &m_lpBBoxVertexBuffer[i],
            NULL
        );
        KGLOG_COM_PROCESS_ERROR(hrRetCode);
    }

    hrRetCode = g_pd3dDevice->CreateIndexBuffer(
        36 * sizeof(WORD),
        D3DUSAGE_WRITEONLY,
        D3DFMT_INDEX16,
        D3DPOOL_MANAGED,
        &m_lpBBoxIndexBuffer,
        NULL
    );
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    m_lpBBoxIndexBuffer->Lock(0, 0, (void**)&pIndexBuffer, 0);

    // top side
    pIndexBuffer[0]  = 0; pIndexBuffer[1]  = 3; pIndexBuffer[2]  = 2;
    pIndexBuffer[3]  = 0; pIndexBuffer[4]  = 2; pIndexBuffer[5]  = 1;

    // bottom side
    pIndexBuffer[6]  = 4; pIndexBuffer[7]  = 5; pIndexBuffer[8]  = 6;
    pIndexBuffer[9]  = 4; pIndexBuffer[10] = 6; pIndexBuffer[11] = 7;

    // front side
    pIndexBuffer[12] = 4; pIndexBuffer[13] = 1; pIndexBuffer[14] = 5;
    pIndexBuffer[15] = 4; pIndexBuffer[16] = 0; pIndexBuffer[17] = 1;

    // back side
    pIndexBuffer[18] = 3; pIndexBuffer[19] = 6; pIndexBuffer[20] = 2;
    pIndexBuffer[21] = 3; pIndexBuffer[22] = 7; pIndexBuffer[23] = 6;

    // right side
    pIndexBuffer[24] = 1; pIndexBuffer[25] = 6; pIndexBuffer[26] = 5;
    pIndexBuffer[27] = 1; pIndexBuffer[28] = 2; pIndexBuffer[29] = 6;

    // left side
    pIndexBuffer[30] = 4; pIndexBuffer[31] = 3; pIndexBuffer[32] = 0;
    pIndexBuffer[33] = 4; pIndexBuffer[34] = 7; pIndexBuffer[35] = 3;

    m_lpBBoxIndexBuffer->Unlock();

    m_dwQueryIndex = 0;
    m_dwQueryCounter = 0;
    m_bDataPrepared = FALSE;

    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        SAFE_RELEASE(m_lpBBoxIndexBuffer);

        for (int i = 0; i < MAX_QUERY_COUNT; ++i)
        {
            SAFE_RELEASE(m_lpBBoxVertexBuffer[i]);
        }

        for (int i = 0; i < MAX_QUERY_COUNT; ++i)
        {     
            SAFE_RELEASE(m_pd3dOcclusionQuery[i]);
        }
    }

    return hrResult;
}

HRESULT KG3DOcclusionCuller::Unint()
{
    REPRESENT_OBJECTS_QUERY_TABLE::iterator itObjectIndex, itObjectEnd;
    KG3DRepresentObject *pObject = NULL;
    itObjectIndex = m_RepresentObjectsInQuery.begin();
    itObjectEnd = m_RepresentObjectsInQuery.end();
    for (NULL; itObjectIndex != itObjectEnd; NULL)
    {
        pObject = itObjectIndex->first;
        itObjectIndex = m_RepresentObjectsInQuery.erase(itObjectIndex);
        SAFE_RELEASE(pObject);
    }

    MODELS_QUERY_TABLE::iterator itModelIndex, itModelEnd;
    itModelEnd = m_ModelsInQuery.end();
    for (itModelIndex = m_ModelsInQuery.begin(); itModelIndex != itModelEnd; NULL)
    {
        KG3DModel* pModel = itModelIndex->first;
        itModelIndex = m_ModelsInQuery.erase(itModelIndex);
        SAFE_RELEASE(pModel);
    }

    SAFE_RELEASE(m_lpBBoxIndexBuffer);

    for (int i = 0; i < MAX_QUERY_COUNT; ++i)
    {
        SAFE_RELEASE(m_lpBBoxVertexBuffer[i]);
    }

    for (int i = 0; i < MAX_QUERY_COUNT; ++i)
    {     
        SAFE_RELEASE(m_pd3dOcclusionQuery[i]);
    }

    m_bDataPrepared = FALSE;

    return S_OK;
}


HRESULT KG3DOcclusionCuller::OcclusionCulling()
{
    HRESULT hrResult = E_FAIL;
    HRESULT hrRetCode = E_FAIL;
    REPRESENT_OBJECTS_QUERY_TABLE::iterator itOjectIndex, itObjectEnd;
    MODELS_QUERY_TABLE::iterator itModelsIndex, itModelsEnd;
    KG3DRenderState RenderState;
    DWORD dwQueryIndex = 0;
    BOOL bNeedIssueEnd = FALSE;

    KG_PROCESS_SUCCESS(!m_bDataPrepared);

    RenderState.SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
    RenderState.SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
    RenderState.SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    RenderState.SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
    RenderState.SetRenderState(D3DRS_COLORWRITEENABLE, 0);
    RenderState.SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
    RenderState.SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    RenderState.SetRenderState(D3DRS_STENCILENABLE, FALSE);
    RenderState.SetRenderState(D3DRS_LIGHTING, FALSE);

    itOjectIndex = m_RepresentObjectsInQuery.begin();
    itObjectEnd = m_RepresentObjectsInQuery.end();
    for (NULL; itOjectIndex != itObjectEnd; ++itOjectIndex)
    {
        if (itOjectIndex->second.dwQueryCounter != m_dwQueryCounter)
            continue;

        dwQueryIndex = itOjectIndex->second.dwQueryIndex;

        hrRetCode = m_pd3dOcclusionQuery[dwQueryIndex]->Issue(D3DISSUE_BEGIN);
        KGLOG_PROCESS_ERROR(hrRetCode == D3D_OK);

        bNeedIssueEnd = TRUE;

        hrRetCode = _RenderBBox(dwQueryIndex);
        KGLOG_COM_PROCESS_ERROR(hrRetCode);

        hrRetCode = m_pd3dOcclusionQuery[dwQueryIndex]->Issue(D3DISSUE_END);
        KGLOG_PROCESS_ERROR(hrRetCode == D3D_OK);

        bNeedIssueEnd = FALSE;
    }

    itModelsIndex = m_ModelsInQuery.begin();
    itModelsEnd = m_ModelsInQuery.end();

    for (NULL; itModelsIndex != itModelsEnd; ++itModelsIndex)
    {
        if (itModelsIndex->second.dwQueryCounter != m_dwQueryCounter)
            continue;

        dwQueryIndex = itModelsIndex->second.dwQueryIndex;

        hrRetCode = m_pd3dOcclusionQuery[dwQueryIndex]->Issue(D3DISSUE_BEGIN);
        KGLOG_PROCESS_ERROR(hrRetCode == D3D_OK);

        bNeedIssueEnd = TRUE;

        hrRetCode = _RenderBBox(dwQueryIndex);
        KGLOG_COM_PROCESS_ERROR(hrRetCode);

        hrRetCode = m_pd3dOcclusionQuery[dwQueryIndex]->Issue(D3DISSUE_END);
        KGLOG_PROCESS_ERROR(hrRetCode == D3D_OK);

        bNeedIssueEnd = FALSE;
    }

    m_bDataPrepared = FALSE;

Exit1:
    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        if (bNeedIssueEnd)
        {
            hrRetCode = m_pd3dOcclusionQuery[dwQueryIndex]->Issue(D3DISSUE_END);
            KGLOG_CHECK_ERROR(hrRetCode == D3D_OK);
        }
    }

    RenderState.Restore();

    return hrResult;
}

BOOL KG3DOcclusionCuller::_IsPointNearBBox(BBox& rBbox, D3DXVECTOR3& rPoint3D)
{
    float fDelta = 100.0f;
    if (
        rBbox.A.x - rPoint3D.x - fDelta <= 0.0f && rBbox.A.y - rPoint3D.y - fDelta <= 0.0f && 
        rBbox.A.z - rPoint3D.z - fDelta <= 0.0f && rBbox.B.x - rPoint3D.x + fDelta >= 0.0f && 
        rBbox.B.y - rPoint3D.y + fDelta >= 0.0f && rBbox.B.z - rPoint3D.z + fDelta >= 0.0f
    )
    {
        return TRUE;
    }

    return FALSE;
}

HRESULT KG3DOcclusionCuller::QueryResult()
{
    HRESULT hrResult = E_FAIL;
    HRESULT hrRetCode = E_FAIL;
    DWORD dwQueryIndex = 0;
    DWORD dwPixelsVisibleCount = 0;
    REPRESENT_OBJECTS_QUERY_TABLE::iterator itObject, itObjectEnd;
    MODELS_QUERY_TABLE::iterator itModel, itModelEnd;

    itObject = m_RepresentObjectsInQuery.begin();
    itObjectEnd = m_RepresentObjectsInQuery.end();
    for (NULL; itObject != itObjectEnd; NULL)
    {
        if (m_dwQueryCounter - itObject->second.dwQueryCounter > 10)
        {
            KG3DRepresentObject* pObject = itObject->first; 
            itObject = m_RepresentObjectsInQuery.erase(itObject);
            SAFE_RELEASE(pObject);
            continue;
        }

        if (itObject->second.dwQueryCounter != m_dwQueryCounter)
        {
            itObject->second.bOccluded = FALSE;
            ++itObject;
            continue;
        }

        dwQueryIndex = itObject->second.dwQueryIndex;
        

        if (g_bClient)
        {
            hrRetCode = m_pd3dOcclusionQuery[dwQueryIndex]->GetData((void*)&dwPixelsVisibleCount, sizeof(DWORD), D3DGETDATA_FLUSH);
            if (hrRetCode == S_OK)
            {
                itObject->second.bOccluded = (dwPixelsVisibleCount == 0);
            }
            else
            {
                itObject->second.bOccluded = FALSE;
            }
        }
        else
        {
            while (true)
            {
                hrRetCode = m_pd3dOcclusionQuery[dwQueryIndex]->GetData((void*)&dwPixelsVisibleCount, sizeof(DWORD), D3DGETDATA_FLUSH);
                if (hrRetCode == S_OK)
                {
                    itObject->second.bOccluded = (dwPixelsVisibleCount == 0);
                    break;
                }
                else if (hrRetCode == S_FALSE)
                {
					break;
                    //continue;
                }
                else
                {
                    itObject->second.bOccluded = FALSE;
                    break;
                }
            }
        }

        ++itObject;
    }

    itModel = m_ModelsInQuery.begin();
    itModelEnd = m_ModelsInQuery.end();
    for (NULL; itModel != itModelEnd; NULL)
    {
        if (m_dwQueryCounter - itModel->second.dwQueryCounter > 10)
        {
            KG3DModel* pModel = itModel->first;
            itModel = m_ModelsInQuery.erase(itModel);
            SAFE_RELEASE(pModel);
            continue;
        }

        if (itModel->second.dwQueryCounter != m_dwQueryCounter)
        {
            itModel->second.bOccluded = FALSE;   
            ++itModel;
            continue;
        }
        
        dwQueryIndex = itModel->second.dwQueryIndex;

        hrRetCode = m_pd3dOcclusionQuery[dwQueryIndex]->GetData((void*)&dwPixelsVisibleCount, sizeof(DWORD), D3DGETDATA_FLUSH);
        if (hrRetCode == S_OK)
        {
            itModel->second.bOccluded = (dwPixelsVisibleCount == 0);
        }
        else
        {
            itModel->second.bOccluded = FALSE;
        }

        ++itModel;
    }

    m_dwQueryIndex = 0;
    
    ++m_dwQueryCounter;

    hrResult = S_OK;
//Exit0:
    return hrResult;
}

HRESULT KG3DOcclusionCuller::_FillBBox(DWORD dwIndex, BBox ObjectBBox)
{
    HRESULT hrResult = E_FAIL;
    HRESULT hrRetCode = E_FAIL;
    D3DXVECTOR3 *pVertexBuffer = NULL;
    D3DXVECTOR3  BoxPos[8];

    KGLOG_PROCESS_ERROR(dwIndex < MAX_QUERY_COUNT);

    // must add a delta value, or occlusion query will fail when bbox is equal to object size
    ObjectBBox.A = ObjectBBox.A + D3DXVECTOR3(-5.0, -5.0, -5.0);
    ObjectBBox.B = ObjectBBox.B + D3DXVECTOR3(5.0, 5.0, 5.0);

    ObjectBBox.Extract(BoxPos);

    hrRetCode = m_lpBBoxVertexBuffer[dwIndex]->Lock(0, 0, (void**)&pVertexBuffer, D3DLOCK_DISCARD);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

	KGLOG_PROCESS_ERROR(pVertexBuffer);

    pVertexBuffer[0] = BoxPos[0];
    pVertexBuffer[1] = BoxPos[1];
    pVertexBuffer[2] = BoxPos[2];
    pVertexBuffer[3] = BoxPos[3];
    pVertexBuffer[4] = BoxPos[4];
    pVertexBuffer[5] = BoxPos[5];
    pVertexBuffer[6] = BoxPos[6];
    pVertexBuffer[7] = BoxPos[7];

    hrRetCode = m_lpBBoxVertexBuffer[dwIndex]->Unlock();
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KG3DOcclusionCuller::_FillObjectBBox(KG3DRepresentObject* pObject)
{
    HRESULT hrResult = E_FAIL;
    HRESULT hrRetCode = E_FAIL;
    BBox ObjectBBox = pObject->GetBBox();
    KG3DQUERY_INFO QueryInfo;

    KG_PROCESS_SUCCESS(m_dwQueryIndex >= MAX_QUERY_COUNT);
    
    hrRetCode = _FillBBox(m_dwQueryIndex, ObjectBBox);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    QueryInfo.bOccluded = FALSE;
    QueryInfo.dwQueryCounter = m_dwQueryCounter;
    QueryInfo.dwQueryIndex = m_dwQueryIndex;
    if (m_RepresentObjectsInQuery.find(pObject) == m_RepresentObjectsInQuery.end())
    {
        pObject->AddRef();
    }
    m_RepresentObjectsInQuery[pObject] = QueryInfo;

    ++m_dwQueryIndex;

Exit1:
    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KG3DOcclusionCuller::_FillModelBBox(KG3DModel* pModel)
{
    HRESULT hrResult = E_FAIL;
    HRESULT hrRetCode = E_FAIL;
    BBox ObjectBBox = pModel->GetBBox();
    KG3DQUERY_INFO QueryInfo;

    KG_PROCESS_SUCCESS(m_dwQueryIndex >= MAX_QUERY_COUNT);

    hrRetCode = _FillBBox(m_dwQueryIndex, ObjectBBox);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);
	
    QueryInfo.bOccluded = FALSE;
    QueryInfo.dwQueryCounter = m_dwQueryCounter;
    QueryInfo.dwQueryIndex = m_dwQueryIndex;
    if (m_ModelsInQuery.find(pModel) == m_ModelsInQuery.end())
    {
        pModel->AddRef();
    }
    m_ModelsInQuery[pModel] = QueryInfo;

    ++m_dwQueryIndex;

Exit1:
    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KG3DOcclusionCuller::_RenderBBox(DWORD dwIndex)
{
    HRESULT hrResult = E_FAIL;

    g_pd3dDevice->SetStreamSource(0, m_lpBBoxVertexBuffer[dwIndex], 0, sizeof(D3DXVECTOR3));
    g_pd3dDevice->SetIndices(m_lpBBoxIndexBuffer);
    g_pd3dDevice->SetFVF(D3DFVF_XYZ);
    g_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 8, 0, 12);

    hrResult = S_OK;
//Exit0:
    return hrResult;
}

// hardware occlusion queries are expensive, there is a way to reduce the number of objects in query queque
// if objects are not occluded last frame, divide them into several groups, then get one group of them and put into query queque
HRESULT KG3DOcclusionCuller::CheckOccluded(
    std::vector<KG3DRepresentObject*>& rvecRepresentObjects,
    std::set<KG3DModel*>& rsetModels
)
{
    HRESULT hrResult    = E_FAIL;
    HRESULT hrRetCode   = E_FAIL;
    std::set<KG3DModel*>::iterator itModelsIndex, itModelsEnd;
    KG3DRepresentObject* pObject = NULL;
    KG3DModel* pModel = NULL;
    BBox ObjectBBox;
    KG3DCamera* pCamera = NULL;
    D3DXVECTOR3 CameraPos(0, 0, 0);
    REPRESENT_OBJECTS_QUERY_TABLE::iterator itObject;
    MODELS_QUERY_TABLE::iterator itModel;
    const DWORD MAX_VISIBLE_OJBECT_QUERY_GROUP_COUNT = 5;
    static DWORD s_dwVisibleObjectQueryGroupIndex = 0;
    DWORD dwVisibleObjectCounter = 0;

    pCamera = g_cGraphicsTool.GetCamera();
    KGLOG_PROCESS_ERROR(pCamera);

    hrRetCode = pCamera->GetPosition(&CameraPos);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    // because will remove element in the loop, so not ++i at for
	for (size_t i = 0; i < rvecRepresentObjects.size(); NULL)
    {
        pObject = rvecRepresentObjects[i];
        if (!pObject->IsLoaded())
        {
            ++i;
            continue;
        }

        itObject = m_RepresentObjectsInQuery.find(pObject);
        
        ObjectBBox = pObject->GetBBox();
        if (itObject != m_RepresentObjectsInQuery.end() && itObject->second.bOccluded)
        {
            if (_IsPointNearBBox(ObjectBBox, CameraPos))
            {
                itObject->second.bOccluded = FALSE;
                ++i;
            }
            else
            {
                hrRetCode = _FillObjectBBox(pObject);
                KGLOG_COM_PROCESS_ERROR(hrRetCode);
         
                size_t uSize = rvecRepresentObjects.size();
                ASSERT(uSize > 0);

                rvecRepresentObjects[i] = rvecRepresentObjects[uSize - 1];
                rvecRepresentObjects.resize(uSize - 1);
                
                SAFE_RELEASE(pObject);
            }

            continue;
        }

        if ((dwVisibleObjectCounter % MAX_VISIBLE_OJBECT_QUERY_GROUP_COUNT) == s_dwVisibleObjectQueryGroupIndex)
        {
            hrRetCode = _FillObjectBBox(pObject);
            KGLOG_COM_PROCESS_ERROR(hrRetCode);
        }
        
        ++dwVisibleObjectCounter;

        ++i;
    }

    itModelsIndex = rsetModels.begin();
    itModelsEnd = rsetModels.end();

    for (NULL; itModelsIndex != itModelsEnd; NULL)
    {
        pModel = (*itModelsIndex);
        if (!pModel->IsLoaded())
        {
            ++itModelsIndex;
            continue;
        }

        itModel = m_ModelsInQuery.find(pModel);

        if (itModel != m_ModelsInQuery.end() && itModel->second.bOccluded)
        {
            if (_IsPointNearBBox(pModel->m_BBox, CameraPos))
            {
                itModel->second.bOccluded = FALSE;
                ++itModelsIndex;
            }
            else
            {
                hrRetCode = _FillModelBBox(pModel);
                KGLOG_COM_PROCESS_ERROR(hrRetCode);

                itModelsIndex = rsetModels.erase(itModelsIndex);
            }

            continue;
        }

        if (
            (pModel->GetType() == MESHTYPE_MODELST) &&
            (dwVisibleObjectCounter % MAX_VISIBLE_OJBECT_QUERY_GROUP_COUNT == s_dwVisibleObjectQueryGroupIndex)
        )
        {
            hrRetCode = _FillModelBBox(pModel);
            KGLOG_COM_PROCESS_ERROR(hrRetCode);
        }

        if ((pModel->GetType() == MESHTYPE_MODELST))
            ++dwVisibleObjectCounter;

        ++itModelsIndex;
    }

    s_dwVisibleObjectQueryGroupIndex = 
        (s_dwVisibleObjectQueryGroupIndex + 1) % MAX_VISIBLE_OJBECT_QUERY_GROUP_COUNT;

    m_bDataPrepared = TRUE;

    hrResult = S_OK;
Exit0:
    return hrResult;
}

////////////////////////////////////////////////////////////////////////////////
KG3DCullData::KG3DCullData()
    :m_bCheckVisibleCount(TRUE)
{

	m_dwRenderCount = 0;

	m_pFrustum = NULL;

	m_dwLastUseTime = 0;
	
	m_fVisibleAngleGate = g_cEngineOption.fLODSwitchAngleGates[EM_LOD_HIGH_TO_MIDDLE];
	m_fLODMiddleToLowSwitchAngleGate = g_cEngineOption.fLODSwitchAngleGates[EM_LOD_MIDDLE_TO_LOW];
    m_fNearbyObjectVisibleAngleGate = 0;

	m_pFrustum = new KG3DFrustum;
	KGLOG_PROCESS_ERROR(m_pFrustum);

    m_pOcclusionCuller = NULL;

Exit0:
	;
}

KG3DCullData::~KG3DCullData()
{
	SAFE_DELETE(m_pFrustum);

	Clear();
}

HRESULT KG3DCullData::Init(KG3DOcclusionCuller *pOcclusioCuller)
{
    ASSERT(!m_pOcclusionCuller);
    
    m_pOcclusionCuller = pOcclusioCuller;

    return S_OK;
}

HRESULT KG3DCullData::Uninit()
{
    m_pOcclusionCuller = NULL;
    return S_OK;
}

KG3DSceneEntityMgr::KG3DSceneEntityMgr(void)
:m_bHasInited(false)
,m_lpTerrainRender(NULL)
{
	m_bUseMultiThread = FALSE;


	m_fCameraFar = 100000.f;//40000;测试提高可视距离到2KM

	m_bKickMeOutFromMultiThreadCull = FALSE;

	m_lpTerrainRender = new KG3DTerrainRender;

	m_bGetEntityFromContainerDirect = FALSE;

	m_bNeedReGetVisibleEntity = TRUE;

	m_nCullLevelDepth = 3;//

	m_lpSKinModelRender = NULL;
	m_lpSKinModelRender = new KG3DSkinModelRender;
	m_lpSKinModelRender->AddRef();
	m_lpSKinModelRender->Init();

	m_lpStaticModelRender = NULL;
	m_lpStaticModelRender = new KG3DStaticModelRender;
	m_lpStaticModelRender->Init();

	m_lpPointLightRender = NULL;
	m_lpPointLightRender = new KG3DScenePointLightRender;
	m_lpPointLightRender->Init();

	m_lpSpeedTreeRender = NULL;
	m_lpSpeedTreeRender = new KG3DModelSpeedTreeRender;
	m_lpSpeedTreeRender->Init();


    m_pOcclusionCuller = NULL;
    m_pWaterCuller = NULL;
    m_bUseOcclusionCull = FALSE;

	m_lpCurrentRenderData = &m_RenderData[0];
}

KG3DSceneEntityMgr::~KG3DSceneEntityMgr(void)
{
    UnInit();
    SAFE_RELEASE(m_lpSKinModelRender);

    SAFE_DELETE(m_lpTerrainRender);

    SAFE_DELETE(m_lpStaticModelRender);
	
	SAFE_DELETE(m_lpPointLightRender);

	SAFE_DELETE(m_lpSpeedTreeRender);

}

HRESULT KG3DSceneEntityMgr::OnLostDevice()
{
    HRESULT hrResult = E_FAIL;
    HRESULT hrRetCode = E_FAIL;

    if (m_lpTerrainRender)
    {
        hrRetCode = m_lpTerrainRender->OnLostDevice();
        KGLOG_COM_PROCESS_ERROR(hrRetCode);
    }


	if (m_lpSpeedTreeRender)
    {
		hrRetCode = m_lpSpeedTreeRender->OnLostDevice();
        KGLOG_COM_PROCESS_ERROR(hrRetCode);
    }

    hrRetCode = m_CullData.Uninit();
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    if (m_bUseOcclusionCull)
    {
        hrRetCode = _UnInitOcclusionCull();
        KGLOG_COM_PROCESS_ERROR(hrRetCode);
    }

	if(m_lpSKinModelRender)
	{
		m_lpSKinModelRender->OnLostDevice();
	}

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KG3DSceneEntityMgr::OnResetDevice()
{
    HRESULT hrResult = E_FAIL;
    HRESULT hrRetCode = E_FAIL;
    BOOL bOcclusionCullInit = FALSE;
    BOOL bCullDataInit = FALSE;

	if (m_lpSpeedTreeRender)
    {
		hrRetCode = m_lpSpeedTreeRender->OnResetDevice();
        KGLOG_COM_PROCESS_ERROR(hrRetCode);
    }

    if (m_lpTerrainRender)
    {
        hrRetCode = m_lpTerrainRender->OnResetDevice();
        KGLOG_COM_PROCESS_ERROR(hrRetCode);
    }
    
    if (m_bUseOcclusionCull)
    {
        hrRetCode = _InitOcclusionCull();
        KGLOG_COM_CHECK_ERROR(hrRetCode);

        bOcclusionCullInit = (SUCCEEDED(hrRetCode));
    }

	if(g_bEnableOcclusionCull)
	{
		hrRetCode = m_CullData.Init(m_pOcclusionCuller);
		KGLOG_COM_PROCESS_ERROR(hrRetCode);
		bCullDataInit = TRUE;
	}

	if(m_lpSKinModelRender)
	{
		m_lpSKinModelRender->OnResetDevice();
	}

    hrResult = S_OK;
Exit0:
    if (FAILED(hrRetCode))
    {
        if (bCullDataInit)
        {
            m_CullData.Uninit();
            bCullDataInit = FALSE;
        }

        if (bOcclusionCullInit)
        {
            _UnInitOcclusionCull();
            bOcclusionCullInit = FALSE;
        }
    }
    return hrResult;
}

HRESULT KG3DSceneEntityMgr::Init(BOOL bOcclusionCull)
{
    HRESULT hrResult = E_FAIL;
    HRESULT hrRetCode = E_FAIL;
    BOOL bTerrainRenderInit = FALSE;
    BOOL bOcclusionCullInit = FALSE;
    BOOL bCullDataInit = FALSE;

    KGLOG_PROCESS_ERROR(!m_bHasInited);
	hrRetCode = m_lpTerrainRender->Init();
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    bTerrainRenderInit = TRUE;
	
    if (bOcclusionCull)
    {
        hrRetCode = _InitOcclusionCull();
        KGLOG_COM_CHECK_ERROR(hrRetCode);
        bOcclusionCullInit = (SUCCEEDED(hrRetCode));
    }

	if(g_bEnableOcclusionCull)
	{
		hrRetCode = m_CullData.Init(m_pOcclusionCuller);
		KGLOG_COM_PROCESS_ERROR(hrRetCode);

		bCullDataInit = TRUE;
	}
	
	m_bUseOcclusionCull = bOcclusionCull;
    m_bWaterVisible = FALSE;

    m_vCameraDirLast = D3DXVECTOR3(0, 0, 0);
    m_vCameraPosLast = D3DXVECTOR3(0, 0, 0);

	m_bHasInited = true;

    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {

        if (bCullDataInit)
        {
            m_CullData.Uninit();
            bCullDataInit = FALSE;
        }

        if (bOcclusionCullInit)
        {
            _UnInitOcclusionCull();
            bOcclusionCullInit = FALSE;
        }
		
        if (bTerrainRenderInit)
        {
            m_lpTerrainRender->UnInit();
            bTerrainRenderInit = FALSE;
        }
    }

	return hrResult;
}

HRESULT KG3DSceneEntityMgr::UnInit()
{
    HRESULT hrResult = E_FAIL;
    HRESULT hrRetCode = E_FAIL;

	if(g_eEngineOption.bUseMultiThreadCull && m_bUseMultiThread && g_pCurrentMultiThreadCullEntityMgr == this)
	{
		m_bKickMeOutFromMultiThreadCull = TRUE;
		g_pCurrentMultiThreadCullEntityMgr = NULL;
		/*while (g_pCurrentMultiThreadCullEntityMgr)
		{
			int s = 0;
		}*/
		m_bUseMultiThread = FALSE;
	}

	hrRetCode = m_lpTerrainRender->UnInit();
    KGLOG_COM_CHECK_ERROR(hrRetCode);

    hrRetCode = m_CullData.Uninit();
    KGLOG_COM_CHECK_ERROR(hrRetCode);

    if (m_bUseOcclusionCull)
    {
        hrRetCode = _UnInitOcclusionCull();
        KGLOG_COM_CHECK_ERROR(hrRetCode);
    }

	if (m_lpPointLightRender)
		m_lpPointLightRender->UnInit();

    if (m_lpSpeedTreeRender)
        m_lpSpeedTreeRender->UnInit();

    m_bHasInited = FALSE;

    hrResult = S_OK;
    return hrResult;
}

HRESULT KG3DSceneEntityMgr::_InitOcclusionCull()
{
    HRESULT hrResult = E_FAIL;
    HRESULT hrRetCode = E_FAIL;
    BOOL bOcclusionCullerInit = FALSE;
    BOOL bWaterCullerInit = FALSE;

    ASSERT(!m_pOcclusionCuller);
    ASSERT(!m_pWaterCuller);

    m_pOcclusionCuller = new KG3DOcclusionCuller;
    KGLOG_PROCESS_ERROR(m_pOcclusionCuller);

    // some graphics cards do not support hardware occlusion query, so init() may fail
    hrRetCode = m_pOcclusionCuller->Init();
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    bOcclusionCullerInit = TRUE;

    m_pWaterCuller = new KG3DWaterCuller;
    KGLOG_PROCESS_ERROR(m_pWaterCuller);

    hrRetCode = m_pWaterCuller->Init();
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    bWaterCullerInit = TRUE;

    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        if (bWaterCullerInit)
        {
            m_pWaterCuller->Uninit();
            bWaterCullerInit = FALSE;
        }
        KG_DELETE(m_pWaterCuller);

        if (bOcclusionCullerInit)
        {
            m_pOcclusionCuller->Unint();
            bOcclusionCullerInit = FALSE;
        }
        KG_DELETE(m_pOcclusionCuller);
    }
    return hrResult;
}

HRESULT KG3DSceneEntityMgr::_UnInitOcclusionCull()
{
    HRESULT hrResult = E_FAIL;
    HRESULT hrRetCode = E_FAIL;

    if (m_pWaterCuller)
    {
        hrRetCode = m_pWaterCuller->Uninit();
        KGLOG_COM_CHECK_ERROR(hrRetCode);

        KG_DELETE(m_pWaterCuller);
    }

    if (m_pOcclusionCuller)
    {
        hrRetCode = m_pOcclusionCuller->Unint();
        KGLOG_COM_CHECK_ERROR(hrRetCode);

        KG_DELETE(m_pOcclusionCuller);
    }

    hrResult = S_OK;
    return hrResult;
}

HRESULT KG3DCullData::FrameMoveVisibleEntity()
{

	{   
		std::vector<KG3DRepresentObject*>::iterator t = m_vecVisiableObjectsRef.begin();
		std::vector<KG3DRepresentObject*>::iterator iend = m_vecVisiableObjectsRef.end();
		while (t!=iend)
		{
			KG3DRepresentObject* pObject = *t;
            
            if (pObject->GetType() == REPRESENTOBJECT_DEFAULT)
            {
                KG3DModel* pModel = pObject->GetModel(0);

                if (pModel)
                {
                    if (pModel->GetType() == MESHTYPE_SFX )
                    {
                        if (!g_cEngineManager.GetFrameMoveOption(FRAMEMOVE_SFX))
                        {
                            ++t;
                            continue;
                        }
                    }
                    else if (pModel->GetType() == MESHTYPE_SPEEDTREE)
                    {
                        if (!g_cEngineManager.GetFrameMoveOption(FRAMEMOVE_SPEEDTREE))
                        {
                            ++t;
                            continue;
                        }
                    }
                    else 
                    {
                        if (!g_cEngineManager.GetFrameMoveOption(FRAMEMOVE_NORL_MESH))
                        {
                            ++t;
                            continue;
                        }
                    }
                }
            }


            if (m_lpEntityMgrSave && m_lpEntityMgrSave->m_lpPointLightRender)
            {
                for (DWORD i = 0; i < pObject->GetModelCount(); ++i)
                {
                    KG3DModel* pModel = pObject->GetModel(i);

                    if (!pModel)
                        continue;

                    m_lpEntityMgrSave->m_lpPointLightRender->FindRefrenceLightForModel(pModel);
                }
            }

			pObject->FrameMove();

			if (pObject->GetType() == REPRESENTOBJECT_ANTIPORTAL)
			{
				KG3DRepresentObjectAntiPortal* pAntiPortal = static_cast<KG3DRepresentObjectAntiPortal*>(pObject);
				KG3DCamera* pCamera = g_cGraphicsTool.GetCamera();
				if(pCamera)
				{
					pAntiPortal->UpdateToWorld(pCamera);
					pAntiPortal->UpdateFrustum(pCamera);
				}

			}

			++t;
		}
	}
	{
		std::set<KG3DModel*>::iterator t = m_setVisiableModels.begin();
		std::set<KG3DModel*>::iterator iend = m_setVisiableModels.end();
		while (t!=iend)
		{
			KG3DModel* pModel = *t;
			if (m_lpEntityMgrSave && m_lpEntityMgrSave->m_lpPointLightRender)
			{
				if (!pModel)
					continue;

				m_lpEntityMgrSave->m_lpPointLightRender->FindRefrenceLightForModel(pModel);
			}
			++t;
		}
	}

	return S_OK;
}

HRESULT KG3DSceneEntityMgr::FrameMoveVisibleEntity()
{
	/*if (m_lpTerrainRender)
		m_lpTerrainRender->PrepareRender(TRUE);*/

	m_CullData.FrameMoveVisibleEntity();

	return S_OK;
}

HRESULT KG3DRenderData::RemoveEntity(DWORD dwClass,KG3DSceneEntity Entity)
{
	if(Entity.GetType()!=SCENEENTITY_OBJECT)
		return S_OK;


	m_vecVisibleModelST.clear();
	m_listModelForLimitSort.clear();

	m_mapBillboardCloudGroup.clear();//<BillboardCloudV1>

	for (DWORD i = 0; i < DEFST_COUNT; ++i)
	{
		m_Renders[i].Clear(&m_cSubMaterialGroupSubInfoPool);
	}

	ClearSFX();
	ClearModelRef();
    m_vecSpecialAlphaModel.clear();
	m_mapMaterialGroupSkinShaderNoCharactor.clear();
	m_mapMaterialGroupNormalNoSkin.clear();
    m_SkinShaderRender.Clear();
	m_mapMaterialModelST.clear();
	//m_setObjectSoundBall.clear();
	m_vecVisiableObjectsNotSort.clear();
	m_vecVisableObjectSortByDistance.clear();

	return S_OK;
}

HRESULT KG3DCullData::RemoveEntity(DWORD dwClass,KG3DSceneEntity Entity)
{
	if(Entity.GetType()!=SCENEENTITY_OBJECT)
		return S_OK;
	KG3DRepresentObject* pObject = static_cast<KG3DRepresentObject*>(Entity.m_pPointer);

    vector<KG3DRepresentObject *>::iterator itFinder = 
        find(m_vecVisiableObjectsRef.begin(), m_vecVisiableObjectsRef.end(), pObject);

	if(itFinder != m_vecVisiableObjectsRef.end())
	{
        m_vecVisiableObjectsRef.erase(itFinder);
        pObject->OnRemoveFromEntityMgr(this);
		pObject->Release();
	}

	return S_OK;
}

HRESULT KG3DSceneEntityMgr::RemoveEntity(DWORD dwClass,KG3DSceneEntity Entity)
{
	m_CullData.RemoveEntity(dwClass,Entity);

	m_lpSpeedTreeRender->BegainPushSpeedTree();
	m_lpSpeedTreeRender->EndPushSpeedTree();

	m_RenderData[0].RemoveEntity(dwClass,Entity);
	m_RenderData[1].RemoveEntity(dwClass,Entity);

	return S_OK;
}

bool objectsort(KG3DRepresentObject* pObjectA,KG3DRepresentObject* pObjectB)
{
	return pObjectA->GetDistance() > pObjectB->GetDistance();
}


HRESULT KG3DCullData::_GetBlockIndexFromContainers(
    vector<POINT> *pRetZoneIndex, 
    vector<POINT> *pRetSectionIndex, 
    vector<POINT> *pRetRegionIndex, 
    vector<KG3DSceneEntityContainer *> *pvecContainer
)
{
    HRESULT hrResult  = E_FAIL;
    KG3DSceneEntityContainer *pContainer = NULL;
    POINT vPoint;
    vector<KG3DSceneEntityContainer *>::const_iterator itIndex;
    vector<KG3DSceneEntityContainer *>::const_iterator itEnd;

    ASSERT(pRetZoneIndex);
    ASSERT(pRetSectionIndex);
    ASSERT(pRetRegionIndex);

    itEnd = (*pvecContainer).end();
    for (itIndex = (*pvecContainer).begin(); itIndex != itEnd; ++itIndex)
    {
        pContainer = (*itIndex);
        if (!pContainer)
            continue;
        int nType = pContainer->GetType();
        switch (nType)
        {
        case KG3DTYPE_SCENE_ZONE:
            {
                KG3DSceneZone *pZone = static_cast<KG3DSceneZone *>(pContainer);
                vPoint.x = pZone->GetIndexX();
                vPoint.y = pZone->GetIndexZ();
                pRetZoneIndex->push_back(vPoint);
            }
            break;
        case KG3DTYPE_SCENE_SECTION:
            {
                KG3DSceneSection *pSection = static_cast<KG3DSceneSection *>(pContainer);
                vPoint.x = pSection->m_nIndexX;
                vPoint.y = pSection->m_nIndexZ;
                pRetSectionIndex->push_back(vPoint);

            }
            break;
        case KG3DTYPE_SCENE_REGION:
            {
                KG3DSceneRegion *pRegion = static_cast<KG3DSceneRegion *>(pContainer);
                vPoint.x = pRegion->m_nIndexX;
                vPoint.y = pRegion->m_nIndexZ;
                pRetRegionIndex->push_back(vPoint);
            }
            break;
        default:
            KG_ASSERT_EXIT(FALSE && "unexpected block type");
        }
    }

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KG3DCullData::GetVisibleSceneRegions(KG3DSceneEntityMgr*        pEntityMgr, 
                                                             const vector<KG3DFrustum>& vFrustum, 
                                                             KG3DSceneSceneEditor*      pEditor,
                                                             const D3DXVECTOR3& vFocusPos)
{
	DWORD dwStartTime = timeGetTime();
    D3DXVECTOR3 vCameraPos = vFocusPos;
   // KG3DCamera *pCamera = g_cGraphicsTool.GetCamera();
   // ASSERT(pCamera);

   // pCamera->GetPosition(&vCameraPos);

    m_dwRenderCount++;

    pEntityMgr->m_lpTerrainRender->SetWaterVisible(FALSE);

    pEntityMgr->m_bNeedReGetVisibleEntity = FALSE;

    m_vecSceneZonesIndex.clear();
    m_vecSceneRegionsIndex.clear();
    m_vecSceneSectionsIndex.clear();

    pEntityMgr->m_lpTerrainRender->SetEyePos(vCameraPos);
    pEntityMgr->m_lpTerrainRender->CleanRoad();
    pEntityMgr->m_lpTerrainRender->CleanRiver();

    m_vecEntityContainer.clear();
    m_vecEntityContainer.push_back(&pEditor->m_lpSceneOutDoorSpaceMgr->GetErrorRegion());

    for (size_t v = 0;  v < vFrustum.size(); ++v)
    {
        pEditor->m_lpSceneOutDoorSpaceMgr->GetVisibleSceneZone(
            &m_vecEntityContainer,
            pEntityMgr->m_lpTerrainRender,
            pEntityMgr->m_lpTerrainRender->GetRoadMap(),
            pEntityMgr->m_lpTerrainRender->GetRiverSet(),
            &vFrustum[v],
            vCameraPos,
            pEntityMgr->GetCullLevelDepth()
       );
    }

    if (!g_bClient || g_cEngineManager.GetRenderOption(RENDER_INFO))
    {
        _GetBlockIndexFromContainers(
            &m_vecSceneZonesIndex,
            &m_vecSceneSectionsIndex,
            &m_vecSceneRegionsIndex,
            &m_vecEntityContainer
        );
    }

    m_dwLastUseTime = timeGetTime();

	DWORD dwCost = timeGetTime() - dwStartTime;
	if(g_cEngineOption.bEnableTimeOptimizeLog && dwCost > 5)
	{
		KGLogPrintf(KGLOG_INFO,"TimeOptimize %d KG3DCullData::GetVisibleSceneRegions ",dwCost);
	}

    return S_OK;
}

HRESULT KG3DCullData::GetVisibleSceneRegionsPosition(KG3DSceneEntityMgr* pEntityMgr, 
															 KG3DCamera *pCamera,
															 KG3DSceneSceneEditor *pEditor,D3DXVECTOR3& vFocusPos)
{

	if(!m_pFrustum)
		return E_FAIL;

	if(!pCamera)
		return E_FAIL;

	m_dwRenderCount++;

    m_pFrustum->CreateByCamera(*pCamera);

	m_vecSceneZonesIndex.clear();
	m_vecSceneRegionsIndex.clear();
	m_vecSceneSectionsIndex.clear();
	//////////////////////////////////////////////////////////////////////////
	pEntityMgr->m_lpTerrainRender->CleanRoad();
	pEntityMgr->m_lpTerrainRender->CleanRiver();
	
	m_vecEntityContainer.clear();

	pEditor->m_lpSceneOutDoorSpaceMgr->GetVisibleSceneZone(
		&m_vecEntityContainer,
		pEntityMgr->m_lpTerrainRender,
		pEntityMgr->m_lpTerrainRender->GetRoadMap(),
		pEntityMgr->m_lpTerrainRender->GetRiverSet(),
		m_pFrustum,vFocusPos,pEntityMgr->GetCullLevelDepth());

    if (!g_bClient || g_cEngineManager.GetRenderOption(RENDER_INFO))
    {
        _GetBlockIndexFromContainers(
            &m_vecSceneZonesIndex,
            &m_vecSceneSectionsIndex,
            &m_vecSceneRegionsIndex,
            &m_vecEntityContainer
        );
    }

    m_dwLastUseTime = timeGetTime();

	return S_OK;
}

HRESULT KG3DCullData::ClearVisibleObjectSet()
{
	vector<KG3DRepresentObject*>::iterator i = m_vecVisiableObjectsRef.begin();
	vector<KG3DRepresentObject*>::iterator iend = m_vecVisiableObjectsRef.end();
	while (i!=iend)
	{
		KG3DRepresentObject* pObject = *i;
		SAFE_RELEASE(pObject);
		++i;
	}
	m_vecVisiableObjectsRef.clear();
	return S_OK;
}
HRESULT KG3DCullData::GetEntityFromContainersDirect()
{
	DWORD dwStartTime = timeGetTime();

	ClearVisibleObjectSet();
	m_setVisiableModels.clear();

    std::vector<KG3DSceneEntityContainer *>::const_iterator itIndex = m_vecEntityContainer.begin();
    std::vector<KG3DSceneEntityContainer *>::const_iterator itEnd = m_vecEntityContainer.end();
    for (NULL; itIndex != itEnd; ++itIndex)
	{
		KG3DSceneEntityContainer* pCon = *itIndex;

		//////////////////////////////////////////////////////////////////////////
        if (!g_bClient)
		{

			KG3DSceneEntityContainer::TypeAccessProxy proxy = pCon->GetSetObjects();
			for(proxy->Begin(); ! proxy->IsEnd(); proxy->StepForward())
			{
				KG3DRepresentObject* pObject = proxy->GetCur();

				OnObjectVisible(pObject);
			}
		}
		//////////////////////////////////////////////////////////////////////////
		{
			DWORD dwCount = 0;
			static KG3DRepresentObject* szObject[1024];

			pCon->m_Data.GetAllObject(dwCount,szObject,1024);

			for (DWORD t=0;t<dwCount;t++)
			{
				KG3DRepresentObject* pObject = szObject[t];
				OnObjectVisible(pObject);
			}
		}
		//////////////////////////////////////////////////////////////////////////
		{
			pCon->m_Data.OnModelsVisible(&m_setVisiableModels,0);
		}
	}

	//////////////////////////////////////////////////////////////////////////
	DWORD dwCost = timeGetTime() - dwStartTime;
	if(g_cEngineOption.bEnableTimeOptimizeLog && dwCost > 5)
	{
		KGLogPrintf(KGLOG_INFO,"TimeOptimize %d KG3DCullData::GetEntityFromContainersDirect ",dwCost);
	}
	return S_OK;
}

HRESULT KG3DCullData::OnObjectVisible(KG3DRepresentObject* pObject)
{
    if (!pObject)
        return E_FAIL;

    /* set the visable count */
    pObject->SetVisibleCount(g_dwRenderCount);
    if (pObject->m_dwLastCullCounter != g_dwCullCounter)
	{
		pObject->AddRef();
        m_vecVisiableObjectsRef.push_back(pObject);
		pObject->ChangeState(KG3DRepresentObject::REPRESENTOBJECTSTATE_ZOOMIN);
	
        pObject->m_dwLastCullCounter = g_dwCullCounter;
    }
    /* update the sub object, as the child object of the pvs */
    pObject->UpdateVisiableSubset(this);

	return S_OK;
}

HRESULT KG3DCullData::OnObjectUnVisible(KG3DRepresentObject* pObject)
{
	return S_OK;
}

HRESULT KG3DCullData::CheckUnVisibleEntity()
{
	DWORD dwUnVisible = 20;
	vector<KG3DRepresentObject*>::iterator i = m_vecVisiableObjectsRef.begin();
	while (i != m_vecVisiableObjectsRef.end())
	{
		KG3DRepresentObject* pObject = *i;
		if(g_dwRenderCount - pObject->GetVisibleCount()>dwUnVisible)
		{
			OnObjectUnVisible(pObject);

			i = m_vecVisiableObjectsRef.erase(i);
			pObject->Release();
		}
		else
		{
			++i;
		}
	}

	return S_OK;
}

HRESULT KG3DRenderData::_ProcessMaterialShaderTypeNone(KG3DModel* pModel,KG3DSceneEntityMgr* pMgr)
{
    HRESULT hrResult = E_FAIL;

    //如果Alpha < 1.0f的， 表示是半透明的， 这种不放到排序里面， 放到m_vecSpecialAlphaModel列表里面
    if (pModel->GetAlpha() < 1.0f)
    {
        IKG3DModel *pParent = NULL;
        pModel->GetParentModel(&pParent);
        if (!pParent)
        {
            //如果没有父物体， 把自己加入到列表中， 如果有父物体直接返回
            m_vecSpecialAlphaModel.push_back(pModel);
        }

        goto Exit1;
    }

    DWORD a = 0, b = 0, c = 0, d = 0;
    pModel->GetProfile(a,b,c,d);
    m_dwFaceCount += a;

    if(g_cEngineOption.bMultiTextureRender/* && !pModel->IsAnimationed()*/)
    {
        if(pModel->IsLoaded())
            pMgr->m_lpStaticModelRender->AddModel(pModel);
    }
    else
    {
        int nCount = 0;
		pModel->GetNumMaterial(&nCount);
        for (int i=0; i< nCount; ++i)
        {
            KG3DMaterial::KG3DMaterialSubset* pSubMtl = pModel->GetMaterialSubset(i);
            if(pSubMtl)
            {
                DWORD dwSubsetKey = pSubMtl->m_dwMaterialID;

                KG3DModelSubsetInfo newsubset;
                newsubset.dwSubsetID = i;
                newsubset.pModel = pModel;

				if(pModel->IsClientAdd())
				{
					m_Renders[DEFST_NEWLIGHT_DETAIL].InputMeshSubset(dwSubsetKey,newsubset,&m_cSubMaterialGroupPool,&m_cSubMaterialGroupSubInfoPool);
				}
				else
				{
					m_Renders[DEFST_NEWLIGHT].InputMeshSubset(dwSubsetKey,newsubset,&m_cSubMaterialGroupPool,&m_cSubMaterialGroupSubInfoPool);
				}
            }
        }
    }

Exit1:
    hrResult = S_OK;
//Exit0:
    return hrResult;
}

HRESULT KG3DRenderData::_ProcessMaterialShaderTypeSkin(KG3DModel* pModel,KG3DSceneEntityMgr* pMgr)
{
    HRESULT hrResult = E_FAIL;

	float Lod = 1;
	pModel->GetLOD(Lod);

    if (pModel->GetAlpha() < 1.0f && !pModel->m_lpParentModelST && Lod >= 0.7F)
    {
        m_vecSpecialAlphaModel.push_back(pModel);
		goto Exit1;
    }

	KG3DMaterial* pMtl = pModel->GetMaterial();
	KGLOG_PROCESS_ERROR(pMtl);

    DWORD dwMtlID = pMtl->GetID();
    if (pModel->IsClientAdd())
    {
        m_SkinShaderRender.InputModel(pModel);

        if(g_cEngineOption.bCpuSkin && pModel->IsLoaded()/* && pModel->m_lpParentModelST == NULL*/)
            pMgr->m_lpSKinModelRender->AddModel(pModel);
    }
    else
    {
        //场景的物件
        std::map<DWORD,ModelGroup>::iterator s = m_mapMaterialGroupSkinShaderNoCharactor.find(dwMtlID);

        if(s != m_mapMaterialGroupSkinShaderNoCharactor.end())
        {
            ModelGroup& modelGroup = s->second;
            modelGroup.push_back(pModel);
        }
        else
        {
            ModelGroup newgroup;
            newgroup.push_back(pModel);
            m_mapMaterialGroupSkinShaderNoCharactor[dwMtlID] = newgroup;
        }

        if(g_cEngineOption.bCpuSkin && pModel->IsLoaded()/* && pModel->m_lpParentModelST == NULL*/)
            pMgr->m_lpSKinModelRender->AddModel(pModel);
    }

Exit1:
    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KG3DRenderData::_ProcessMaterialShaderTypeNormalNoSkin(KG3DModel* pModel,KG3DSceneEntityMgr* pMgr)
{
    HRESULT hrResult = E_FAIL;
    std::map<DWORD,ModelGroup>::iterator s;
    KG3DMaterial* pMtl = NULL;
    DWORD dwMtlID = 0;

    pMtl = pModel->GetMaterial();
    KG_PROCESS_ERROR(pMtl);

    dwMtlID = pMtl->GetID();

    s = m_mapMaterialGroupNormalNoSkin.find(dwMtlID);

    if(s!=m_mapMaterialGroupNormalNoSkin.end())
    {
        ModelGroup& modelGroup = s->second;
        modelGroup.push_back(pModel);
    }
    else
    {
        ModelGroup newgroup;
        newgroup.push_back(pModel);
        m_mapMaterialGroupNormalNoSkin[dwMtlID] = newgroup;
    }

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KG3DRenderData::_ProcessMeshTypeDefault(KG3DModel* pModel,KG3DSceneEntityMgr* pMgr)
{
    HRESULT hrResult = E_FAIL;
    HRESULT hrRetCode = E_FAIL;


    if(!pModel->IsResourceReady(TRUE))
		goto Exit1;


	/*if(pModel->m_lpMaterial && !pModel->m_lpMaterial->m_bTextureLoaded)
	{
		pModel->m_lpMaterial->_LoadTexture(0);
	}*/

	if (pModel->HasBillBoardCloud())
	{
		//这个判断是干嘛的？
		//if(pModel->m_fViewDistance < g_fLODMiddleToLowSwitchAngleGate)
		{
			map<DWORD, ModelGroup>::iterator iFind = m_mapBillboardCloudGroup.find(pModel->GetID());

			if (iFind == m_mapBillboardCloudGroup.end())
			{
				ModelGroup modelGroup;
				modelGroup.push_back(pModel);
				m_mapBillboardCloudGroup[pModel->GetID()] = modelGroup;
			}
			else
			{
				ModelGroup& modelGroup = iFind->second;
				modelGroup.push_back(pModel);
			}

			goto Exit1;
		}
	}

    if(pModel->IsSortAsSFX())
    {
        m_vecVisiableSFX.push_back(pModel);
		pModel->AddRef();
        goto Exit1;
    }

	if (pModel->GetAlpha() < 1.0f)
	{
		IKG3DModel *pParent = NULL;
		pModel->GetParentModel(&pParent);
		if (!pParent)
        {
            m_vecSpecialAlphaModel.push_back(pModel);
        }
		goto Exit1;
	}

	if(pModel->m_eRuntimeShaderType == DEFST_LIGHT_MAP )
	{
		pModel->AddRef();
		m_vecVisiableSFX.push_back(pModel);
		/*int nCount = 0;
		pModel->GetNumMaterial(&nCount);
		for (int i=0; i< nCount; ++i)
		{
			KG3DMaterial::KG3DMaterialSubset* pSubMtl = pModel->GetMaterialSubset(i);
			if(pSubMtl)
			{
				DWORD dwSubsetKey = pSubMtl->m_dwMaterialID;

				KG3DModelSubsetInfo newsubset;
				newsubset.dwSubsetID = i;
				newsubset.pModel = pModel;
				m_Renders[DEFST_LIGHT_MAP].InputMeshSubset(dwSubsetKey,newsubset,&m_cSubMaterialGroupPool,&m_cSubMaterialGroupSubInfoPool);
			}
		}*/
	}
	else
	{
		DWORD dwShaderType = pModel->GetShaderType();
		switch (dwShaderType)
		{
		case KG3DMaterial::MAT_SHADER_TYPE_NONE:
			hrRetCode = _ProcessMaterialShaderTypeNone(pModel,pMgr);
			KG_COM_PROCESS_ERROR(hrRetCode);
			break;
		case KG3DMaterial::MAT_SHADER_TYPE_SKIN:
			hrRetCode = _ProcessMaterialShaderTypeSkin(pModel,pMgr);
			KG_COM_PROCESS_ERROR(hrRetCode);
			break;
		case KG3DMaterial::MAT_SHADER_TYPE_NORMAL_NOSKIN:
			hrRetCode = _ProcessMaterialShaderTypeNormalNoSkin(pModel,pMgr);
			KG_COM_PROCESS_ERROR(hrRetCode);
			break;
		default:
			KG_ASSERT_EXIT(false);
		}
	}

Exit1:
    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KG3DRenderData::_ProcessMeshTypeSpeedTree(KG3DModel* pModel,KG3DSceneEntityMgr* pMgr)
{
    HRESULT hrResult = E_FAIL;

    KG3DCamera* pCamera = g_cGraphicsTool.GetCamera();
    KGLOG_CHECK_ERROR(pCamera);

    if (pModel->HasBillBoardCloud() && pCamera)
    {
        //这个判断是干嘛的？
        //if(fVisibleAngle < m_fLODMiddleToLowSwitchAngleGate )
        {
            map<DWORD, ModelGroup>::iterator iFind = m_mapBillboardCloudGroup.find(pModel->GetID());

            if (iFind == m_mapBillboardCloudGroup.end())
            {
                ModelGroup modelGroup;
                modelGroup.push_back(pModel);
                m_mapBillboardCloudGroup[pModel->GetID()] = modelGroup;
            }
            else
            {
                ModelGroup& modelGroup = iFind->second;
                modelGroup.push_back(pModel);
            }
 
            KG3DModelSpeedTree* pTree = static_cast<KG3DModelSpeedTree*>(pModel);

            if(pTree->m_lpModelTree)
                SortMaterialForOneModel(pTree->m_lpModelTree,pMgr,pTree->m_lpModelTree,FALSE);

            goto Exit1;
        }
    }

	/*if(g_dwRenderCount % 10 < 1)
	{
		D3DXVECTOR3 vEye = pCamera->GetPosition();
		float L = D3DXVec3Length(&(pModel->m_BBox.GetCenter() - vEye));
		float fLOD = 1-min(1,L*0.00002F);
		pModel->SetLOD(fLOD);
	}*/

    KG3DModelSpeedTree* pTree = static_cast<KG3DModelSpeedTree*>(pModel);
    pMgr->m_lpSpeedTreeRender->PushSpeedTree(pTree);

Exit1:
    hrResult = S_OK;
//Exit0:
    return hrResult;
}

HRESULT KG3DRenderData::_ProcessMeshTypeModelst(KG3DModel* pModel,KG3DSceneEntityMgr* pMgr)
{
    HRESULT hrResult = E_FAIL;
    KG3DModelST* pModelST = static_cast<KG3DModelST*>(pModel);
    int nModelCount = 0;

    KG_PROCESS_SUCCESS(!pModelST->IsResourceReady(TRUE));

    if (pModelST->GetAlpha() < 1.0f)
    {
        m_vecSpecialAlphaModel.push_back(pModelST);
        goto Exit1;
    }

	if(pModelST->m_lpSkinMeshUion)
	{
		nModelCount = pModelST->GetNumModel();
		for (int i = 0; i < nModelCount; ++i)
		{
			KG3DModel* pSubModelCur  = NULL;
			KG3DModel* pSubModelHigh = NULL;

			if(SUCCEEDED(pModelST->GetModelEx(i,&pSubModelCur,&pSubModelHigh)))
			{					
				pSubModelCur->m_bHideFromRenderTools = pModel->m_bHideFromRenderTools;

				std::set<KG3DModelLod>::iterator i = pSubModelCur->m_ChildModels.begin();
				std::set<KG3DModelLod>::iterator iend = pSubModelCur->m_ChildModels.end();
				while(i!=iend)
				{
					KG3DModelLod& lod = *i;

					if (lod.pModelCurrent->CheckNeedRender())
					{
						KG3DModel* pSubModel = lod.pModelCurrent;
						pSubModel->SetVisibleCount(g_dwRenderCount);
						SortMaterialForOneModel(pSubModel,pMgr,pSubModel,FALSE);
					}
					++i;
				};
			}
		}
		//////////////////////////////////////////////////////////////////////////
	}
	else
	{
		if(g_cEngineOption.bCpuSkin)
		{
			pMgr->m_lpSKinModelRender->AddModel(pModel);
		}

		nModelCount = pModelST->GetNumModel();
		for (int i = 0; i < nModelCount; ++i)
		{
			KG3DModel* pSubModelCur  = NULL;
			KG3DModel* pSubModelHigh = NULL;

			if(SUCCEEDED(pModelST->GetModelEx(i,&pSubModelCur,&pSubModelHigh)))
			{					
				pSubModelCur->m_bHideFromRenderTools = pModel->m_bHideFromRenderTools;
				pSubModelCur->SetVisibleCount(g_dwRenderCount);
				SortMaterialForOneModel(pSubModelCur,pMgr,pSubModelHigh,FALSE);
			}
		}
	}

    m_vecVisibleModelST.push_back(pModelST);//现在暂时只用于脚底影子

Exit1:
    hrResult = S_OK;
//Exit0:
    return hrResult;
}
HRESULT KG3DCullData::_GetEntityShellFromContainers(KG3DSceneEntityContainer* pCon,const D3DXVECTOR3& vCameraPos, KG3DScene* pScene, const vector<KG3DFrustum>& vFrustum)
{
	DWORD dwCount = 0;
	static KG3DRepresentObject* szObject[1024];

	pCon->m_Data._GetEntityShellFromContainers(m_bCheckVisibleCount,
		m_fVisibleAngleGate,m_fNearbyObjectVisibleAngleGate,
		dwCount,szObject,1024,
		vCameraPos,pScene,vFrustum);

	for (DWORD i=0;i<dwCount;i++)
	{
		KG3DRepresentObject* pObject = szObject[i];
		OnObjectVisible(pObject);
	}

	return S_OK;
}


HRESULT KG3DCullData::_GetModelFromContainers(KG3DSceneEntityContainer* pCon, const vector<KG3DFrustum>& vFrustum)
{
	pCon->m_Data._GetModelFromContainers(&m_setVisiableModels,vFrustum);
    return S_OK;
}

HRESULT KG3DCullData::_GetEntityFromContainers(const D3DXVECTOR3& vCameraPos, KG3DScene* pScene, const vector<KG3DFrustum>& vFrustum)
{
    HRESULT hrResult = E_FAIL;
    HRESULT hrRetCode = E_FAIL;

    if (!pScene)
        return E_FAIL;

	DWORD dwStartTime = timeGetTime();
    DWORD dwVisible = 10;

    m_fNearbyObjectVisibleAngleGate = m_fVisibleAngleGate + D3DX_PI * (1 - (g_cEngineOption.fCameraDistance / 20000)) / 180;
    m_fNearbyObjectVisibleAngleGate = max(m_fNearbyObjectVisibleAngleGate, 0);

    ClearVisibleObjectSet();
    m_setVisiableModels.clear();

    std::vector<KG3DSceneEntityContainer *>::const_iterator itIndex = m_vecEntityContainer.begin();
    std::vector<KG3DSceneEntityContainer *>::const_iterator itEnd = m_vecEntityContainer.end();
    for (NULL; itIndex != itEnd; ++itIndex)
	{
		KG3DSceneEntityContainer* pCon = *itIndex;
		pCon->SetVisibleCount(g_dwRenderCount);
		
        if (!g_bClient)
        {
			KG3DSceneEntityContainer::TypeAccessProxy proxy = pCon->GetSetObjects();
			for(proxy->Begin(); ! proxy->IsEnd(); proxy->StepForward())
			{
				KG3DRepresentObject* pObject = proxy->GetCur();
            
				if(m_bCheckVisibleCount)
				{
                    BBox  ObjectBBox;
                    KG3DFrustum::INTERSECT_TYPE eFrustumIntersectType = KG3DFrustum::OUTSIDE;
                    DWORD dwVisibleFrameCount = pObject->GetVisibleCount();

                    if (pCon->m_Data.m_eFrustumIntersectType == KG3DFrustum::INSIDE)
                    {
                        ++dwVisibleFrameCount;
                        pObject->SetVisibleCount(dwVisibleFrameCount);
                        if (dwVisibleFrameCount > dwVisible) // 判断是否连续看的到
						    OnObjectVisible(pObject);

                        continue;
                    }

                    ObjectBBox = pObject->GetBBox();
                    
                    for (size_t i = 0; i < vFrustum.size(); ++i)
                    {
                        eFrustumIntersectType = (KG3DFrustum::INTERSECT_TYPE)vFrustum[i].Intersect(ObjectBBox);
                        if (eFrustumIntersectType != KG3DFrustum::OUTSIDE)
                            break;
                    }

                    if (eFrustumIntersectType != KG3DFrustum::OUTSIDE)
                    {
                        ++dwVisibleFrameCount;
                        pObject->SetVisibleCount(dwVisibleFrameCount);
                        if (dwVisibleFrameCount > dwVisible) // 判断是否连续看的到
						    OnObjectVisible(pObject);
                    }
                    else
                    {
                        pObject->SetVisibleCount(0);
                    }
				}
				else
					OnObjectVisible(pObject);
			}
		}


		hrRetCode = _GetEntityShellFromContainers(pCon,vCameraPos,pScene,vFrustum);
        KGLOG_COM_PROCESS_ERROR(hrRetCode);
	}

	/* if the camera is in indoor object now, add the indoor object to visable list */

	if (KG3DSceneSpaceNode::GetTopVisableNode() != pScene->m_lpSceneOutDoorSpaceMgr)
	{
		KG3DRepresentObject* pObject = dynamic_cast<KG3DRepresentObject*>(KG3DSceneSpaceNode::GetTopVisableNode());
		ASSERT(pObject);

		if(pObject)
		{
			OnObjectVisible(pObject);

            KG3DEntityObjectShell* pObjShell = pScene->m_lpSceneOutDoorSpaceMgr->GetObjShellByHandle(pObject->GetHandleHeldBySpaceMgr());

            if (pObjShell)
                pObjShell->SetVisibleCount(pObject->GetVisibleCount());
				
		}
	}

    {
        KG3DSceneSceneEditor *pEditor = static_cast<KG3DSceneSceneEditor *>(pScene);

        pEditor->GetVisibleModelFromOutDoor(m_setVisiableModels, vFrustum);
    }

    DWORD dwFillMode = D3DFILL_SOLID;
    g_pd3dDevice->GetRenderState(D3DRS_FILLMODE, &dwFillMode);
    if (dwFillMode == D3DFILL_SOLID)
    {
        if (m_pOcclusionCuller)
        {   
            hrRetCode = m_pOcclusionCuller->CheckOccluded(m_vecVisiableObjectsRef, m_setVisiableModels);     
            KGLOG_COM_PROCESS_ERROR(hrRetCode);
        }
    }


    hrResult = S_OK;
Exit0:

	DWORD dwCost = timeGetTime() - dwStartTime;
	if(g_cEngineOption.bEnableTimeOptimizeLog && dwCost > 5)
	{
		KGLogPrintf(KGLOG_INFO,"TimeOptimize %d KG3DCullData::_GetEntityFromContainers ",dwCost);
	}
	return hrResult;
}

HRESULT KG3DRenderData::BeforeRenderProcess()
{
	for (DWORD i = 0; i < DEFST_COUNT; ++i)
	{
		m_Renders[i].Optimize();
	}

	for (size_t i=0;i<m_vecVisibleModelST.size();i++)
	{
		KG3DModel* pModel = m_vecVisibleModelST[i];
		if(pModel)
		{
			pModel->SetVisible();
		}
	}

    for (size_t i = 0; i<m_vecVisiableModelsRef.size(); i++)
    {
        KG3DModel* pModel = m_vecVisiableModelsRef[i];
        if(pModel)
        {
            pModel->SetVisible();
        }
    }

	return S_OK;
}

HRESULT KG3DRenderData::SortVisibleEntity(KG3DCullData* pCullData,KG3DSceneEntityMgr* pMgr)
{
	DWORD dwStartTime = timeGetTime();
	KG3DCamera *pCamera = NULL;

    pCamera = g_cGraphicsTool.GetCamera();
    ASSERT(pCamera);
    D3DXVECTOR3 vCameraPos;

    pCamera->GetPosition(&vCameraPos);

	//////////////////////////////////////////////////////////////////////////
	//对物件进行排序
	SortMaterialForVisiableObjects(pMgr);

	//////////////////////////////////////////////////////////////////////////
    for (size_t i = 0; i < m_vecVisableObjectSortByDistance.size(); ++i)
	{
		KG3DRepresentObject* pObject = m_vecVisableObjectSortByDistance[i];
		float X = vCameraPos.x - pObject->m_Translation.x;
		float Z = vCameraPos.z - pObject->m_Translation.z;
		pObject->SetDistance(X * X + Z * Z);
	}

    sort(m_vecVisableObjectSortByDistance.begin(), m_vecVisableObjectSortByDistance.end(), objectsort);

	SortModelsByDistance(vCameraPos.x, vCameraPos.z, pMgr);

    m_SkinShaderRender.Sort();

	DWORD dwCost = timeGetTime() - dwStartTime;
	if(g_cEngineOption.bEnableTimeOptimizeLog && dwCost > 5)
	{
		KGLogPrintf(KGLOG_INFO,"TimeOptimize %d KG3DCullData::SortVisibleEntity ",dwCost);
	}

	return S_OK;
}


// new verson

HRESULT KG3DCullData::GetEntityFromVisibleContainers(KG3DSceneEntityMgr* pEntityMgr, 
													 const vector<KG3DFrustum>& vFrustum, 
													 KG3DSceneSceneEditor *pEditor,const D3DXVECTOR3& vFocusPos)
{
	if(pEntityMgr->m_bGetEntityFromContainerDirect)
		GetEntityFromContainersDirect();
	else
		_GetEntityFromContainers(vFocusPos, pEditor, vFrustum);

	return S_OK;
}

HRESULT KG3DCullData::GetVisibleContainersByFrustum(KG3DSceneEntityMgr* pEntityMgr, 
                                                       const vector<KG3DFrustum>& vFrustum, 
                                                       KG3DSceneSceneEditor *pEditor,const D3DXVECTOR3& vFocusPos
                                                       )
{
    DWORD dwStartTime = timeGetTime();

	GetVisibleSceneRegions(pEntityMgr, vFrustum, pEditor,vFocusPos);

    
	//////////////////////////////////////////////////////////////////////////
	DWORD dwCost = timeGetTime() - dwStartTime;
	if(g_cEngineOption.bEnableTimeOptimizeLog && dwCost > 10)
	{
		KGLogPrintf(KGLOG_INFO,"TimeOptimize %d KG3DCullData::GetVisibleEntityByFrustum ",dwCost);
	}

    return S_OK;
}


// old verson

HRESULT KG3DCullData::GetVisibleEntityByCamera(KG3DSceneEntityMgr* pEntityMgr,
											 KG3DCamera *pCamera,
											 KG3DSceneSceneEditor *pEditor,D3DXVECTOR3& vFocusPos)
{
	if(!m_pFrustum)
		return E_FAIL;

	if(!pCamera)
		return E_FAIL;

	D3DXVECTOR3 vPos;
	D3DXVECTOR3 vFront;
	pCamera->GetPosition(&vPos);
	vFront = pCamera->GetCameraFront();

	m_dwRenderCount++;
	//////////////////////////////////////////////////////////////////////////
	//获得可见的场景REGION
	GetVisibleSceneRegionsPosition(pEntityMgr,pCamera,pEditor,vFocusPos);

	//////////////////////////////////////////////////////////////////////////
	//获取可见REGION上的物件对象
	if(pEntityMgr->m_bGetEntityFromContainerDirect)
	{
		GetEntityFromContainersDirect();
	}
	else
	{
        vector<KG3DFrustum> vFrustum;
        vFrustum.push_back(*m_pFrustum);
		_GetEntityFromContainers(vFocusPos, pEditor, vFrustum);
	}

	return S_OK;
}

HRESULT KG3DSceneEntityMgr::CheckCurrentRenderData()
{
	if(m_bUseMultiThread)
	{
		if(&m_RenderData[0] != m_lpCurrentRenderData && m_RenderData[0].m_eState == STATE_DATAOK)//优先选择另一个绘制数据
		{
			if(m_lpCurrentRenderData)
			{
				m_lpCurrentRenderData->m_eState = STATE_EMPTY;
			}
			m_lpCurrentRenderData = &m_RenderData[0];
		}
		else if(&m_RenderData[1] != m_lpCurrentRenderData && m_RenderData[1].m_eState == STATE_DATAOK)
		{
			if(m_lpCurrentRenderData)
			{
				m_lpCurrentRenderData->m_eState = STATE_EMPTY;
			}
			m_lpCurrentRenderData = &m_RenderData[1];
		}
		else
		{
			if(m_lpCurrentRenderData && m_lpCurrentRenderData->m_eState == STATE_RENDERED)//另一个没有准备好，就用自己
				;
			else
				m_lpCurrentRenderData = NULL;
		}
	}
	else
	{
		m_lpCurrentRenderData = &m_RenderData[0];
	}

	return S_OK;
}

HRESULT KG3DSceneEntityMgr::GetVisibleEntity(
    const vector<KG3DFrustum>& vFrustum, 
    KG3DSceneSceneEditor*      pEditor, 
    const D3DXVECTOR3& vFocusPos
)
{
    DWORD dwNowTime = timeGetTime();

    ++g_dwCullCounter;

    g_ProjTextureRender.RemoveAllProjSource();
    m_CullData.m_lpEntityMgrSave = this;

    m_CullData.GetVisibleContainersByFrustum(this, vFrustum, pEditor,vFocusPos);

    m_CullData.GetEntityFromVisibleContainers(this, vFrustum, pEditor, vFocusPos);        

    m_CullData.ExportToRenderData(m_lpCurrentRenderData,this);

    if (!pEditor->m_bUnderWater && m_pWaterCuller)
    {
        m_lpTerrainRender->GetWaterBoundingPlanes(m_pWaterCuller);
    }

    m_lpTerrainRender->PrepareRenderData(pEditor->m_lpSceneOutDoorSpaceMgr);
    m_lpTerrainRender->SetTerrainMaterialMgr(pEditor->m_lpSceneOutDoorSpaceMgr->m_lpDetailMtls);
    m_lpTerrainRender->SetTerrainConverMgr(pEditor->m_lpSceneOutDoorSpaceMgr->m_lpConverMap);

    m_bNeedReGetVisibleEntity = FALSE;

    m_bWaterVisible = m_lpTerrainRender->IsWaterVisible();

    if (!pEditor->m_bUnderWater && m_pWaterCuller && m_bWaterVisible)
    {
        m_pWaterCuller->QueryResult(&m_bWaterVisible);
    }

    if(m_lpCurrentRenderData)
    {
        m_lpCurrentRenderData->BeforeRenderProcess();
    }

	//////////////////////////////////////////////////////////////////////////
	DWORD dwCost = timeGetTime() - dwNowTime;
	if(g_cEngineOption.bEnableTimeOptimizeLog && dwCost > 10)
	{
		KGLogPrintf(KGLOG_INFO,"TimeOptimize %d KG3DSceneEntityMgr::GetVisibleEntity",dwCost);
	}

    return S_OK;
}

HRESULT KG3DSceneEntityMgr::GetVisibleEntityEx(
    KG3DCamera *pCamera, KG3DSceneSceneEditor *pEditor,D3DXVECTOR3& vFocusPos
)
{
    ++g_dwCullCounter;

	//不支持多线程
	m_CullData.m_lpEntityMgrSave = this;
	m_CullData.GetVisibleEntityByCamera(this,pCamera,pEditor,vFocusPos);

	m_lpTerrainRender->SetTerrainMaterialMgr(pEditor->m_lpSceneOutDoorSpaceMgr->m_lpDetailMtls);
	m_lpTerrainRender->SetTerrainConverMgr(pEditor->m_lpSceneOutDoorSpaceMgr->m_lpConverMap);

	//////////////////////////////////////////////////////////////////////////
	m_CullData.ExportToRenderData(m_lpCurrentRenderData,this);
	m_lpTerrainRender->PrepareRenderData(pEditor->m_lpSceneOutDoorSpaceMgr);

	if(m_lpCurrentRenderData)
	{
		m_lpCurrentRenderData->BeforeRenderProcess();
	}

	return S_OK;
}

HRESULT KG3DCullData::ClearFrustum()
{
    for (vector<KG3DRepresentObject*>::iterator itor = m_vecVisiableObjectsRef.begin();
         itor != m_vecVisiableObjectsRef.end(); ++itor)
    {
        if((*itor)->GetType() == REPRESENTOBJECT_PVS)
        {
            KG3DRepresentObjectPVS* pPVS = static_cast<KG3DRepresentObjectPVS*>(*itor);
            pPVS->ClearFrustums();
        }
    }
    return S_OK;
}

HRESULT KG3DCullData::Clear()
{
	ClearVisibleObjectSet();

	m_dwRenderCount = 0;
	
	return S_OK;
}

HRESULT KG3DSceneEntityMgr::ClearFrustum()
{
    m_CullData.ClearFrustum();
    return S_OK;
}

HRESULT KG3DSceneEntityMgr::Clear()
{
	m_CullData.Clear();
	return S_OK;
}

static D3DXVECTOR3 s_vec3CameraPos;
static bool SortModelByDistance(KG3DModel *pModel1, KG3DModel *pModel2)
{
	float fX = pModel1->m_matWorld._41 - s_vec3CameraPos.x;
	float fY = pModel1->m_matWorld._42 - s_vec3CameraPos.y;
	float fZ = pModel1->m_matWorld._43 - s_vec3CameraPos.z;

	float fDis1 = fX * fX + fY * fY + fZ * fZ;
	fX = pModel2->m_matWorld._41 - s_vec3CameraPos.x;
	fY = pModel2->m_matWorld._42 - s_vec3CameraPos.y;
	fZ = pModel2->m_matWorld._43 - s_vec3CameraPos.z;

	float fDis2 = fX * fX + fY * fY + fZ * fZ;

	return fDis1 > fDis2;
}

HRESULT KG3DRenderData::ShockWaveRender()
{
	for (size_t i=0;i<m_vecPostRenderSFX.size();i++)
	{
		m_vecPostRenderSFX[i]->Render(0,NULL);
	}

	return S_OK;
}

HRESULT KG3DRenderData::RenderVisibleEntitySFX(KG3DSceneSceneEditor* pEditor,KG3DCullData* pCullData)
{
	HRESULT hrResult = E_FAIL;
	
	std::vector<KG3DModel*> vecModelNeedSort;
	if (!g_cEngineManager.GetRenderOption(RENDER_SFX))
        return S_OK;

    if (!pEditor)
        return E_FAIL;

	m_vecPostRenderSFX.clear();

	GraphicsStruct::RenderStateAlpha Alpha(0x10,TRUE,TRUE,D3DBLEND_SRCALPHA,D3DBLEND_INVSRCALPHA);

    for (size_t i = 0; i < m_vecVisiableSFX.size(); ++i)
    {
		KG3DModel* pSFX = m_vecVisiableSFX[i];
		if(pSFX && pSFX->IsLoaded())
		{
			DWORD dwCbNum = 0;
			pSFX->GetRenderTargetTextureCount(&dwCbNum);

			if (dwCbNum > 0 && !g_cEngineManager.GetRenderOption(RENDER_DRAWABLE_BILLBOARD))
			{
				continue;
			}

			if (pSFX->CheckNeedRender())
			{
				if (pSFX->IsSortAsSFX())
				{
					vecModelNeedSort.push_back(pSFX);
				}
				else
				{
					pSFX->Render(0,NULL);
				}
			}

			if (pSFX->m_nIsSockWave)
				m_vecPostRenderSFX.push_back( pSFX );
		}
		else
		{
			ASSERT(0);
		}
	}

	{
		GraphicsStruct::RenderState Zenable(D3DRS_ZENABLE,D3DZB_TRUE);
		GraphicsStruct::RenderState ZWrite(D3DRS_ZWRITEENABLE,TRUE);
		GraphicsStruct::RenderState ZFun(D3DRS_ZFUNC,D3DCMP_LESS);
		GraphicsStruct::RenderState colorWrite(D3DRS_COLORWRITEENABLE ,D3DCOLORWRITEENABLE_ALPHA|D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_GREEN);
        KG3DCamera *pCamera = g_cGraphicsTool.GetCamera();
        ASSERT(pCamera);
        KGLOG_PROCESS_ERROR(pCamera);

        pCamera->GetPosition(&s_vec3CameraPos);

		sort(vecModelNeedSort.begin(), vecModelNeedSort.end(), SortModelByDistance);
		for (size_t i = 0; i < vecModelNeedSort.size(); i++)
		{
			vecModelNeedSort[i]->Render(0, NULL);
		}
	}

    hrResult = S_OK;
Exit0:
	return hrResult;
}

HRESULT KG3DRenderData::RenderProfile()
{
	TCHAR str [256];

	sprintf(str,"All:%d Fs%d NoSort:%d SGc:%d  Skin Model:%d SGN:%d SFX:%d Billboard Cloud %d Lcal:%d LCount:%d LTime:%d",
		m_vecVisiableObjectsRef.size(),
		m_dwFaceCount,
		m_vecVisiableObjectsNotSort.size(),
		m_Renders[DEFST_NEWLIGHT].m_mapMaterialGroup.size(),
		m_SkinShaderRender.GetModelCount(),
		m_mapMaterialGroupNormalNoSkin.size(),
		m_vecVisiableSFX.size(),
        m_mapBillboardCloudGroup.size(),
		m_dwListObjectRendered,
		g_nLoadCount,
		g_nLoadTime
    );

	g_cGraphicsTool.DrawDebugText(str);//现在全局统一排序绘制


	return S_OK;
}

HRESULT KG3DSceneEntityMgr::RenderProfile()
{
	if(m_lpCurrentRenderData)
		return m_lpCurrentRenderData->RenderProfile();
	else
		return S_OK;
}

HRESULT KG3DRenderData::SortMaterialForOneModel(KG3DModel* pModel,KG3DSceneEntityMgr* pMgr,KG3DModel* pModelWithChild,BOOL bUseNumberLimit)//原来的bSorted参数去掉，用返回值SUCCEEDED(hr)来判断是否被Sort
{
    HRESULT hrResult = E_FAIL;
    HRESULT hrRetCode = E_FAIL;

	if(!pModel->IsLoaded())
	{
		hrResult = S_OK;
		goto Exit0; 
	}
	DWORD dwType = pModel->GetType();

	pModel->AddRef();
	m_vecModelRef.push_back(pModel);

    switch(dwType)
	{
	case MESHTYPE_DEFAULT:
        {
            hrRetCode = _ProcessMeshTypeDefault(pModel,pMgr);
            KG_COM_PROCESS_ERROR(hrRetCode);
        }
        break;
	case MESHTYPE_SFX:
		{
			KG3DSFX* pSFX = static_cast<KG3DSFX*>(pModel);
			if(pSFX->IsResourceReady(TRUE))
			{
				if(!g_bShowClientSFX && pSFX->IsClientAdd() )
				{
					;
				}
				else
				{
					m_vecVisiableSFX.push_back(pSFX);
					pSFX->AddRef();
					g_ProjTextureRender.CheckInputProjectSource(pModel);
				}
			}
		}
		break;
	case MESHTYPE_SPEEDTREE:
        {
            hrRetCode = _ProcessMeshTypeSpeedTree(pModel,pMgr);
            KG_COM_PROCESS_ERROR(hrRetCode);
        }
		break;
	case MESHTYPE_MODELST:
        {
			if(pModel->IsResourceReady(TRUE))
			{
				if(bUseNumberLimit)
				{
					m_listModelForLimitSort.push_back(pModel);
				}
				else
				{
					hrRetCode =_ProcessMeshTypeModelst(pModel,pMgr);
					KG_COM_PROCESS_ERROR(hrRetCode);
				}	
			}
        }
        break;
	case MESHTYPE_POINTLIGHT:
		{
			KG3DModelPointLight* pLight = static_cast<KG3DModelPointLight*>(pModel);
			pMgr->m_lpPointLightRender->AddPointLight(pLight);

            if (g_cEngineManager.GetRenderOption(RENDER_POINT_SIGN) && !g_bClient)
			{
                m_vecVisiableSFX.push_back(pLight);
				pModel->AddRef();
			}
		}
		break;
    default:
        goto Exit0;
	}

	//if(0)
	{
		std::set<KG3DModelLod>::iterator i = pModelWithChild->m_ChildModels.begin();
		std::set<KG3DModelLod>::iterator iend = pModelWithChild->m_ChildModels.end();
		while(i!=iend)
		{
			KG3DModelLod& lod = *i;

			if (lod.pModelCurrent->CheckNeedRender())
			{
				KG3DModel* pSubModel = lod.pModelCurrent;
				pSubModel->SetVisibleCount(g_dwRenderCount);
				SortMaterialForOneModel(pSubModel,pMgr,pSubModel,FALSE);
			}
			++i;
		}
	}

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KG3DRenderData::SortMaterialForOneObject(KG3DRepresentObject* pObject,float fVisibleAngle,KG3DSceneEntityMgr* pMgr)//用返回值表示时候被Sort
{
	BOOL bSorted = FALSE;

	/*if(pObject->GetAlpha()<1.0F)
	{
		m_listVisableObjectSortByDistance.push_back(pObject);
	}
	else*/
	{
		DWORD dwObjType = pObject->GetType();
		switch(dwObjType)
		{
		case REPRESENTOBJECT_NODE:
			{
				bSorted = TRUE;
				if(!g_bClient && g_bRenderRoadNode)
				{
					m_vecVisableObjectSortByDistance.push_back(pObject);
				}
				break;
			}
			
		case REPRESENTOBJECT_SOUNDBALL:
			{
				bSorted = TRUE;
				//KG3DRepresentObjectSoundBall* pSound = static_cast<KG3DRepresentObjectSoundBall*>(pObject);
				//if(g_bPlaySoundBall)
				//	m_setObjectSoundBall.insert(pSound);
				if ((g_bRenderSoundBall)&&(!g_bClient))
					m_vecVisableObjectSortByDistance.push_back(pObject);
				break;
			}
		case REPRESENTOBJECT_NPC:
		case REPRESENTOBJECT_DOODAD:
			{
               /* DWORD dwCount = pObject->GetModelCount();
                DWORD i = 0;

                while (i < dwCount)
                {
                    KG3DModel* pModel = NULL;

                    if(SUCCEEDED(pObject->GetModel(pModel,i,TRUE)))
                    {
                        if (pModel->GetType() == MESHTYPE_SFX)
                            break;
                    }

                    ++i;
                }

                if (i < dwCount)
                {
                    BOOL AllPutIn = TRUE;
                    for (DWORD i=0;i<dwCount;++i)
                    {
                        KG3DModel* pModel = NULL;
                        if(SUCCEEDED(pObject->GetModel(pModel,i,TRUE)))
                        {
                            HRESULT hr = SortMaterialForOneModel(pModel,pMgr,pModel);
                            AllPutIn &= SUCCEEDED(hr);
                        }
                    }
                    bSorted = AllPutIn;

                    break;
                }*/

				bSorted = TRUE;
				m_vecVisableObjectSortByDistance.push_back(pObject);
			}
			break;
		case REPRESENTOBJECT_WALL:
			bSorted = FALSE;
			break;
		case REPRESENTOBJECT_POLY:
			bSorted = TRUE;
			//KG3DRepresentPoly* pPoly = (KG3DRepresentPoly*)pObject;
			m_vecPolygon.push_back(pObject);
			break;
		case REPRESENTOBJECT_TERRAINBLOCK:
			bSorted = FALSE;
			break;
		//case REPRESENTOBJECT_TRAFFICPOINT:
		//	bSorted = FALSE;
		//	break;
		//case REPRESENTOBJECT_PVS:
		case REPRESENTOBJECT_ANTIPORTAL:
			{
				if (!g_bClient)
				{
					bSorted = FALSE;
					break;
				}
				else
				{
					bSorted = TRUE;
					break;
				}
			}
		case REPRESENTOBJECT_TRAFFICPOINT:
		case REPRESENTOBJECT_WAYPOINT:
			{
			//	int k = 0;
			}
		default:
			{
				DWORD dwCount = pObject->GetModelCount();
				if(dwCount > 0)//这句话很重要，没有Model的类型就会返回E_FAIL
				{
					BOOL AllPutIn = TRUE;
					for (DWORD i=0;i<dwCount;++i)
					{
						KG3DModel* pModel = NULL;

						if(SUCCEEDED(pObject->GetModel(pModel,i,TRUE)))
						{
							HRESULT hr = SortMaterialForOneModel(pModel,pMgr,pModel,FALSE);

							AllPutIn &= SUCCEEDED(hr);
						}
                    }

                    if (dwObjType == REPRESENTOBJECT_PVS && !g_bClient)
                    {
                        KG3DRepresentObjectPVS* pvs = (KG3DRepresentObjectPVS*)pObject;
                        for (list<KG3DPvsPortal*>::iterator i = pvs->m_listPortal.begin();
                             i != pvs->m_listPortal.end(); ++i)
                             m_vecPvsPortal.push_back(*i);
                    }
                        //AllPutIn = FALSE;

                    bSorted = AllPutIn;
				}
			}		
			//新添加的类型，默认不会分类，外部就会放到没有分类的list里面，还是能绘制出来。
			break;
		}
	}
	return bSorted ? S_OK : E_FAIL;
}

HRESULT KG3DRenderData::SortMaterialForVisiableObjects(KG3DSceneEntityMgr* pMgr)
{
    HRESULT hrRetCode = E_FAIL;
	DWORD dwStartTime = timeGetTime();

    m_mapBillboardCloudGroup.clear();
	
	for (DWORD i = 0; i < DEFST_COUNT; ++i)
	{
		m_Renders[i].Clear(&m_cSubMaterialGroupSubInfoPool);
	}

	ClearSFX();
	ClearModelRef();
	g_ClearRenderTwiceInfo();

	m_dwGroupedObjectCount = 0;
    m_SkinShaderRender.Clear();
	m_vecVisibleModelST.clear();
	m_listModelForLimitSort.clear();

	m_mapMaterialGroupNormalNoSkin.clear();
	m_vecVisiableObjectsNotSort.clear();
	m_mapMaterialModelST.clear();
	m_vecVisableObjectSortByDistance.clear();
    m_vecPvsPortal.clear();
	m_vecPolygon.clear();
    m_vecSpecialAlphaModel.clear();
	m_mapMaterialGroupSkinShaderNoCharactor.clear();
	pMgr->m_lpSpeedTreeRender->BegainPushSpeedTree();

	std::vector<KG3DRepresentObject*>::iterator t = m_vecVisiableObjectsRef.begin();
	std::vector<KG3DRepresentObject*>::iterator iend = m_vecVisiableObjectsRef.end();
	while (t!=iend)
	{
		KG3DRepresentObject* pObject = *t;
		
		pObject->CheckLODLevel();

		hrRetCode = SortMaterialForOneObject(pObject,pObject->m_fVisibleAngle,pMgr);
		
		if(FAILED(hrRetCode))
			m_vecVisiableObjectsNotSort.push_back(pObject);
		else
			m_dwGroupedObjectCount++;

		++t;
	}


	pMgr->m_lpSpeedTreeRender->EndPushSpeedTree();
	pMgr->m_lpSpeedTreeRender->PrepareSpeedTreeRenderData();

	//////////////////////////////////////////////////////////////////////////
	//sort speedtree tree model
	if (g_bRenderTree)
	{
        KG3DModelSpeedTree *pSpeedTree = NULL;
        size_t uSize = pMgr->m_lpSpeedTreeRender->m_vecSpeedTreeRenderData.size();
        for (size_t i = 0; i < uSize; ++i)
        {
             pSpeedTree = pMgr->m_lpSpeedTreeRender->m_vecSpeedTreeRenderData[i];
             if(pSpeedTree->m_lpModelTree)
             {
                 SortMaterialForOneModel(pSpeedTree->m_lpModelTree, pMgr,pSpeedTree->m_lpModelTree,FALSE);
             }
        }
	
	}

	DWORD dwCost = timeGetTime() - dwStartTime;
	if(g_cEngineOption.bEnableTimeOptimizeLog && dwCost > 5)
	{
		KGLogPrintf(KGLOG_INFO,"TimeOptimize %d KG3DCullData::SortMaterialForVisiableObjects ",dwCost);
	}
	return S_OK;
}

HRESULT KG3DRenderData::RenderVisibleObjectUnSort()
{
	DWORD dwNowTime = timeGetTime();
	//////////////////////////////////////////////////////////////////////////
	///没有排序的
	GraphicsStruct::RenderStateAlpha Alpha(0x20,TRUE,TRUE,D3DBLEND_SRCALPHA,D3DBLEND_INVSRCALPHA);

    for (size_t i = 0; i < m_vecVisiableObjectsNotSort.size(); ++i)
	{
		//if(m_vecVisiableObjectsNotSort[i])
			m_vecVisiableObjectsNotSort[i]->Render(0);
	}

	DWORD dwCost = timeGetTime() - dwNowTime;
	if(dwCost >= 30)
	{
		KGLogPrintf(KGLOG_ERR, "%d KG3DSceneEntityMgr::RenderVisibleObjectUnSort.",dwCost);
	}

	return S_OK;
}


HRESULT KG3DRenderData::RenderVisibleModelNormalNoSkin()
{
	//////////////////////////////////////////////////////////////////////////
	///渲染带NormalMap的部分

	HRESULT hr = E_FAIL;
	if(0 == m_mapMaterialGroupNormalNoSkin.size())
		return S_OK;

	{
		hr = KG3DModel::Shader_DrawMeshNormalNoSkin_Begin();
		if (SUCCEEDED(hr))
		{
			std::map<DWORD,ModelGroup>::iterator t = m_mapMaterialGroupNormalNoSkin.begin();
			std::map<DWORD,ModelGroup>::iterator iend = m_mapMaterialGroupNormalNoSkin.end();

			while (t!=iend)
			{
				ModelGroup& Group = t->second;
				//////////////////////////////////////////////////////////////////////////
				for (size_t s=0;s<Group.size();s++)
				{
					KG3DModel* pModel = Group[s];

					pModel->Shader_DrawMeshNormalNoSkin_NoChange();
				}
				++t;
			}

			hr = KG3DModel::Shader_DrawMeshNormalNoSkin_End();
		}

		
	}
	return hr;
}

HRESULT KG3DRenderData::RenderVisibleModelSkinForDepth()
{
	//////////////////////////////////////////////////////////////////////////
	///带Skin的Shader的部分
	if(!g_cEngineOption.bCpuSkin)
	{    
        m_SkinShaderRender.RenderForDepth();
	}

	if(!g_cEngineOption.bCpuSkin)
	{
		std::map<DWORD,ModelGroup>::iterator t = m_mapMaterialGroupSkinShaderNoCharactor.begin();
		std::map<DWORD,ModelGroup>::iterator iend = m_mapMaterialGroupSkinShaderNoCharactor.end();
		while (t!=iend)
		{
			ModelGroup& Group = t->second;
			//////////////////////////////////////////////////////////////////////////
			for (size_t s=0;s<Group.size();s++)
			{
				KG3DModel* pModel = Group[s];

				if( s==0 )
				{
					pModel->Shader_RenderSkinMeshDepth_Begin();
				}

				pModel->Shader_RenderSkinMeshDepth_NoChange();

				if (s == Group.size() - 1)
				{
					pModel->Shader_RenderSkinMeshDepth_End();
				}
			}
			++t;
		}
	}
	return S_OK;
}

HRESULT KG3DRenderData::RenderVisibleModelSkinShaderUnion(KG3DTerrainConverMap* pConverMap)
{
	HRESULT hr = E_FAIL;
	LPD3DXEFFECT pShader = NULL;
	UINT uDummy = 0;

	pShader = g_cGraphicsTool.GetDefaultShader(DEFST_NEWLIGHT_DETAIL);
	KG_PROCESS_ERROR(pShader);

	pShader->SetTechnique("ConverMapTechnique");
	pShader->Begin(&uDummy,0);
	pShader->BeginPass(0);

	for (size_t i=0;i<m_vecVisibleModelST.size();++i)
	{
		KG3DModel* pModel = m_vecVisibleModelST[i];
		KG3DModelST* pModelST = static_cast<KG3DModelST*>(pModel);
		if(pModelST && !pModelST->m_bHideFromRenderTools && pModelST->m_lpSkinMeshUion)
		{
			pModelST->m_lpSkinMeshUion->RenderNoChange(pModelST,pShader);
		}
	}

	pShader->EndPass();
	//////////////////////////////////////////////////////////////////////////
	pShader->End();

	hr = S_OK;
Exit0:
	return hr;
}
HRESULT KG3DRenderData::RenderVisibleModelSkinShader(KG3DTerrainConverMap* pConverMap,BOOL bJustNear)
{
	GraphicsStruct::RenderStateAlpha Alpha(0x80,TRUE,TRUE,D3DBLEND_SRCALPHA,D3DBLEND_INVSRCALPHA);
	GraphicsStruct::SamplerState sa( 0, D3DSAMP_MIPMAPLODBIAS, FtoDW(-1.0f));

	RenderVisibleModelSkinShaderUnion(pConverMap);

	//////////////////////////////////////////////////////////////////////////
	///带Skin的Shader的部分
	if(!g_cEngineOption.bCpuSkin)
	{
        m_SkinShaderRender.Render(bJustNear);
	}

	if(!g_cEngineOption.bCpuSkin)
	{
		std::map<DWORD,ModelGroup>::iterator t = m_mapMaterialGroupSkinShaderNoCharactor.begin();
		std::map<DWORD,ModelGroup>::iterator iend = m_mapMaterialGroupSkinShaderNoCharactor.end();
		while (t!=iend)
		{
			ModelGroup& Group = t->second;
			//////////////////////////////////////////////////////////////////////////
			for (size_t s=0;s<Group.size();s++)
			{
				KG3DModel* pModel = Group[s];

				if( s==0 )
				{
					pModel->Shader_RenderSkinMesh_Begin(TRUE);
				}

				pModel->Shader_RenderSkinMesh_NoChange();

				if (s == Group.size() - 1)
				{
					pModel->Shader_RenderSkinMesh_End();
				}
			}
			++t;
		}
	}


	return S_OK;
}

HRESULT KG3DRenderData::RenderVisibleBillboardCloud()
{
	/*
	render billboard clouds//<BillboardCloudV1>
	*/
	{
		std::map<DWORD, ModelGroup>::iterator iter = m_mapBillboardCloudGroup.begin();
		std::map<DWORD, ModelGroup>::iterator iend = m_mapBillboardCloudGroup.end();

		while (iter != iend)
		{
			ModelGroup& Group = iter->second;

			HRESULT hr = Group.front()->RenderBillboardCloud_Begin();
			if(FAILED(hr))//如果Begin失败，下面的也不用做了
				break;

			for (size_t i = 0; i < Group.size(); ++i)
				Group[i]->RenderBillboardCloud();

			Group.back()->RenderBillboardCloud_End();

			++iter;
		}
	}
	return S_OK;
}

HRESULT KG3DRenderData::RenderVisibleObjectSortByDistance()
{
	//////////////////////////////////////////////////////////////////////////
	///按距离排序的
	GraphicsStruct::RenderStateAlpha Alpha(0x80,TRUE,TRUE,D3DBLEND_SRCALPHA,D3DBLEND_INVSRCALPHA);
    for (size_t i = 0; i < m_vecVisableObjectSortByDistance.size(); ++i)
    {
        KG3DRepresentObject *pObject = m_vecVisableObjectSortByDistance[i];
        pObject->Render(0);

        if (g_bClient)
            continue;

        //在编辑器中，把NPC的脚底影子也渲染出来
		if (pObject->GetType() != REPRESENTOBJECT_NPC )
			continue;
		
		if(0 == pObject->GetModelCount())
			continue;

		if(1 == pObject->GetModelCount())
		{
			KG3DModel* p = pObject->GetModel(0);
			_ASSERTE(NULL != p);
			if (! KG3DModelShadowRenderer::IsModelShadowNeedToBeRender(*p))
			{
				continue;
			}
		}

		g_GetModelShadowRenderer().InputShadowDummy(pObject->GetTranslation(), 50.0f);
    }

	return S_OK;
}

HRESULT KG3DRenderData::SortModelsByDistance(float X,float Z,KG3DSceneEntityMgr* pMgr)
{
	DWORD dwStartTime = timeGetTime();
    float fDistance = 0.0f;

	BOOL bCheckLod = /*g_cEngineOption.bEnableModelLod && */(g_dwRenderCount % 5 == 0);

	std::vector<KG3DModel*>::iterator t = m_vecVisiableModelsRef.begin();
	std::vector<KG3DModel*>::iterator iend = m_vecVisiableModelsRef.end();

	while (t!=iend)
	{
		KG3DModel* pModel = *t;

		D3DXVECTOR3 vTrans;
		pModel->GetTranslation(&vTrans);


		if(bCheckLod)
		{
			fDistance = (vTrans.x - X)*(vTrans.x - X) + (vTrans.z - Z)*(vTrans.z - Z);
			float fLod = 1.0F - min(1, fDistance / 1440000);
			pModel->SetLOD(fLod);
			pModel->m_fViewDistance = fDistance;
		}

        SortMaterialForOneModel(pModel, pMgr, pModel,TRUE);

		++t;
	}
	//////////////////////////////////////////////////////////////////////////
	{
		m_listModelForLimitSort.sort(ModelsortByViewDistance);

		int nCount = 0;
		list<KG3DModel*>::iterator i = m_listModelForLimitSort.begin();
		list<KG3DModel*>::iterator iend = m_listModelForLimitSort.end();
		while (i!=iend)
		{
			KG3DModel* pModel = *i;

			SortMaterialForOneModel(pModel, pMgr, pModel,FALSE);

			if(nCount >= g_nMDLRenderLimit)
				break;
			nCount++;
			i++;
		}

		m_listModelForLimitSort.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	DWORD dwCost = timeGetTime() - dwStartTime;
	if(g_cEngineOption.bEnableTimeOptimizeLog && dwCost > 5)
	{
		KGLogPrintf(KGLOG_INFO,"TimeOptimize %d KG3DCullData::SortModelsByDistance ",dwCost);
	}

	return S_OK;
}

HRESULT KG3DRenderData::RenderPvsPortal()
{
    if (g_bClient)
        return S_OK;

    for (size_t i = 0; i < m_vecPvsPortal.size(); ++i)
    {
        m_vecPvsPortal[i]->Render();
    }

    return S_OK;
}

HRESULT KG3DRenderData::RenderPoly()
{
	if (g_bClient)
		return S_OK;
	GraphicsStruct::RenderState Zenable(D3DRS_ZENABLE,D3DZB_TRUE);
	GraphicsStruct::RenderState ZWrite(D3DRS_ZWRITEENABLE,TRUE);
	GraphicsStruct::RenderState ZFun(D3DRS_ZFUNC,D3DCMP_LESS);
	for (size_t i = 0; i < m_vecPolygon.size(); ++i)
	{
		m_vecPolygon[i]->Render(0);
	}

	return S_OK;
}

HRESULT KG3DSceneEntityMgrShadow::Init(KG3DSceneEntityMgr* pBaseEntityMgr)
{
	KG3DSceneEntityMgr::Init(FALSE);

	if(pBaseEntityMgr)
	{
		SAFE_RELEASE(m_lpSKinModelRender);
		m_lpSKinModelRender = pBaseEntityMgr->m_lpSKinModelRender;
		m_lpSKinModelRender->AddRef();
	}

	return S_OK;
}

HRESULT KG3DSceneEntityMgr::PrintfProfile()
{
	int K = 0;
	KG_PROCESS_ERROR(m_lpCurrentRenderData);

	KGLogPrintf(KGLOG_ERR,"按SUBSET排序的组为 %d",m_lpCurrentRenderData->m_Renders[DEFST_NEWLIGHT].m_mapMaterialGroup.size());
	{
		std::map<DWORD,KG3DSubMaterialGroup*>::iterator t = m_lpCurrentRenderData->m_Renders[DEFST_NEWLIGHT].m_mapMaterialGroup.begin();
		std::map<DWORD,KG3DSubMaterialGroup*>::iterator iend = m_lpCurrentRenderData->m_Renders[DEFST_NEWLIGHT].m_mapMaterialGroup.end();
		while (t!=iend)
		{
			KG3DSubMaterialGroup*& Group = t->second;
			//////////////////////////////////////////////////////////////////////////
			KGLogPrintf(KGLOG_ERR," SUBSET排序组序号 %x",t->first);
			K += (int)Group->m_vecSubsetMaterialSet.size();
			Group->PrintfProfile();
			++t;
		}
	}

	KGLogPrintf(KGLOG_ERR,"总ID数目为 %d",K);

Exit0:
	return S_OK;
}

vector<POINT>* KG3DSceneEntityMgr::GetSceneRegionsIndexVector()
{
	return &m_CullData.m_vecSceneRegionsIndex;
}

vector<POINT>* KG3DSceneEntityMgr::GetSceneSectionsIndexVector()
{
	return &m_CullData.m_vecSceneSectionsIndex;
}

HRESULT KG3DSceneEntityMgr::RemoveRenderModel(KG3DModel* pModel)
{
	m_CullData.m_setVisiableModels.erase(pModel);

	return S_OK;
}

KG3DCullData* KG3DSceneEntityMgr::GetUnReadyCullData()
{
	/*if(!m_CullData[0].m_bReady)
		return &m_CullData[0];
	if(!m_CullData[1].m_bReady)
		return &m_CullData[1];*/
	return NULL;
}

HRESULT KG3DCullData::ProcessMultiThreadCull()
{
	return S_OK;
}

HRESULT KG3DSceneEntityMgr::ProcessMultiThreadCull()
{
	m_CullData.ProcessMultiThreadCull();

	return S_OK;
}

HRESULT KG3DSceneEntityMgr::OnRenderEnd()
{
	DWORD dwNowTime = timeGetTime();

	g_nGrassVertexBufferFillPerFrame = 0;

    if (m_pOcclusionCuller)
    {
        m_pOcclusionCuller->QueryResult();
    }

	if(g_eEngineOption.bUseMultiThreadCull && m_bUseMultiThread)
	{
		CheckCurrentRenderData();
	}

	DWORD dwCost = timeGetTime() - dwNowTime;
	if(dwCost >= 50)
	{
		KGLogPrintf(KGLOG_ERR, "%d KG3DSceneEntityMgr::OnRenderEnd.",dwCost);
	}
	return S_OK;
}

HRESULT KG3DSceneEntityMgr::RenderVisibleTerrainBlocks(KG3DSceneSceneEditor* pEditor,BOOL bGrass,BOOL bStone,BOOL bFrutex)
{
    HRESULT hr = S_OK;

    if (g_cEngineManager.GetRenderOption(RENDER_TERRAIN))
    {
	    KG3DLightParam param = g_cGraphicsTool.GetSunLightParam(*pEditor);
	    m_lpTerrainRender->RenderTerrainBlocks_Detail(param,bGrass,bStone,bFrutex);

        hr = m_lpTerrainRender->PostRender();
        KGLOG_COM_PROCESS_ERROR(hr);
    }

Exit0:
    return hr;
}

HRESULT KG3DSceneEntityMgr::RenderVisibleZoneBox(KG3DSceneSceneEditor* pEditor,BOOL bDetail)
{
	for (size_t i=0;i<m_CullData.m_vecSceneZonesIndex.size();++i)
	{
		POINT tempPT = m_CullData.m_vecSceneZonesIndex[i];

		KG3DSceneZone* pZone = pEditor->m_lpSceneOutDoorSpaceMgr->GetZone(tempPT.x,tempPT.y);
		if (pZone)
		{
			g_cGraphicsTool.DrawAABBox(&pZone->GetAABBox(),0xFFFF0000);
		}
	}

	RenderVisibleSectionBox(pEditor);
	RenderVisibleRegionBox(pEditor);

	return S_OK;
}

HRESULT KG3DSceneEntityMgr::RenderVisibleSectionBox(KG3DSceneSceneEditor* pEditor)
{
	for (size_t i=0;i<m_CullData.m_vecSceneSectionsIndex.size();++i)
	{
		POINT tempPT = m_CullData.m_vecSceneSectionsIndex[i];

		KG3DSceneSection* pSection = pEditor->m_lpSceneOutDoorSpaceMgr->GetSection(tempPT.x,tempPT.y);
		if (pSection)
		{
			g_cGraphicsTool.DrawAABBox(&pSection->GetAABBox(),0xFF00FF00);
		}
	}

	return S_OK;
}

HRESULT KG3DSceneEntityMgr::RenderVisibleRegionBox(KG3DSceneSceneEditor* pEditor)
{
	for (size_t i=0;i<m_CullData.m_vecSceneRegionsIndex.size();++i)
	{
		POINT tempPT = m_CullData.m_vecSceneRegionsIndex[i];

		KG3DSceneRegion* pRegion = pEditor->m_lpSceneOutDoorSpaceMgr->GetRegion(tempPT.x,tempPT.y);
		if (pRegion)
		{
			g_cGraphicsTool.DrawAABBox(&pRegion->GetAABBox(),0xFFFFFF00);
		}
	}

	return S_OK;
}


HRESULT KG3DRenderData::RenderBilloarCloudDepth()
{
    GraphicsStruct::RenderState colorWrite(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_ALPHA);
	DWORD dwNowTime = timeGetTime();

    std::map<DWORD, ModelGroup>::iterator iter = m_mapBillboardCloudGroup.begin();
    std::map<DWORD, ModelGroup>::iterator iend = m_mapBillboardCloudGroup.end();

    while (iter != iend)
    {
        ModelGroup& Group = iter->second;

        Group.front()->RenderBillboardCloud_Begin();

        for (size_t i = 0; i < Group.size(); ++i)
            Group[i]->RenderBillboardCloud();

        Group.back()->RenderBillboardCloud_End();

        ++iter;
    }

	DWORD dwCost = timeGetTime() - dwNowTime;
	if(g_cEngineOption.bEnableTimeOptimizeLog && dwCost >= 30)
	{
		KGLogPrintf(KGLOG_ERR, "TimeOptimize %d KG3DCullData::RenderBilloarCloudDepth.",dwCost);
	}
    return S_OK;
}

VOID KG3DRenderData::RenderModelFeetShadow()
{
	//渲染人物之前，渲染出脚底影子
	KGCH::TFrontAccessProxySolid<TypeVisibleModelList> proxy = KGCH::CreateFrontAccessProxy(m_vecVisibleModelST);
	g_GetModelShadowRenderer().Render(proxy);
}

HRESULT KG3DSceneEntityMgr::RenderForDepthOnly(KG3DSceneSceneEditor* pEditor,BOOL bTerrain,BOOL bTree,BOOL bObject)
{
	//////////////////////////////////////////////////////////////////////////
    HRESULT hrResult = E_FAIL;
    HRESULT hrRetCode = E_FAIL;
	DWORD dwNowTime = timeGetTime();
	DWORD dwCost1=0,dwCost2 =0,dwCost4=0,dwCost5=0,dwCost6=0;

	KG3DLight& rLight = g_cGraphicsTool.GetSunLight(*pEditor);

	DWORD dwColorWrite = 0;
	if(g_cEngineOption.bUseRenderTarget_DepthTexture)
	{
		if (g_nDepthTexturePackingType == 0)
			dwColorWrite = D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_ALPHA;
		else
			dwColorWrite = D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA;
	}

	if(bTree)
		m_lpSpeedTreeRender->RenderSpeedTreeLeafsSortWithZpass(TRUE,FALSE,rLight,NULL,0);
	
    /* render billoar cloud depth */
	dwCost1 = timeGetTime() - dwNowTime;

	//KG_PROCESS_ERROR(m_lpCurrentRenderData);
	if(m_lpCurrentRenderData)
		m_lpCurrentRenderData->RenderBilloarCloudDepth();

	if(bObject)
	{
		//////////////////////////////////////////////////////////////////////////

        if (g_cEngineManager.GetRenderOption(RENDER_NORL_MESH))
        {
		    m_lpCurrentRenderData->m_Renders[DEFST_NEWLIGHT].RenderDepthOnly(1);
		    dwCost2 = timeGetTime() - dwNowTime;
			m_lpCurrentRenderData->m_Renders[DEFST_NEWLIGHT_DETAIL].RenderDepthOnly(1);
			
			m_lpCurrentRenderData->m_Renders[DEFST_LIGHT_MAP].RenderDepthOnly(1);
        }

        if (g_cEngineManager.GetRenderOption(RENDER_CLIENT_OBJ))
        {
		    GraphicsStruct::RenderState Zenable(D3DRS_ZENABLE,D3DZB_TRUE);
		    GraphicsStruct::RenderState ZWrite(D3DRS_ZWRITEENABLE,TRUE);
		    GraphicsStruct::RenderState ZFun(D3DRS_ZFUNC,D3DCMP_LESS);

		    m_lpCurrentRenderData->RenderVisibleObjectUnSort();
		    dwCost4 = timeGetTime() - dwNowTime;
        }

		//m_lpCurrentCullData->RenderVisibleObjectsZoomOut();

		//m_lpCurrentCullData->RenderVisibleObjectSortByDistance();

		//this->RenderVisibleObjectBBox();

		/*if(g_cEngineOption.bCpuSkin)
		{
			m_lpSKinModelRender->Render();	
			m_lpStaticModelRender->Render();
		}*/

		if(g_cEngineOption.bMultiTextureRender)
		{
			GraphicsStruct::RenderState ZFun(D3DRS_ZFUNC,D3DCMP_LESS);
			GraphicsStruct::RenderState colorWrite(D3DRS_COLORWRITEENABLE ,dwColorWrite);
			GraphicsStruct::RenderStateAlpha Alpha(0x80,TRUE,FALSE,D3DBLEND_SRCALPHA,D3DBLEND_INVSRCALPHA);
			GraphicsStruct::RenderState alphafun(D3DRS_ALPHAFUNC,D3DCMP_GREATEREQUAL);
			m_lpStaticModelRender->RenderForDepth();
			dwCost5 = timeGetTime() - dwNowTime;
		}

		if(g_cEngineOption.bCpuSkin)
		{
			GraphicsStruct::RenderStateAlpha Alpha(0x80,TRUE,FALSE,D3DBLEND_SRCALPHA,D3DBLEND_INVSRCALPHA);
			GraphicsStruct::RenderState alphafun(D3DRS_ALPHAFUNC,D3DCMP_GREATEREQUAL);
			m_lpSKinModelRender->RenderForDepth();
			dwCost6 = timeGetTime() - dwNowTime;
		}
	}

    if(bTerrain)
        m_lpTerrainRender->RenderTerrainBlockDepthOnly(TRUE,TRUE,TRUE,TRUE,TRUE);
	if(g_cEngineManager.NeedDrawTwice())
	{
		if (g_cEngineManager.GetRenderOption(RENDER_ENTITY))
		{
			GraphicsStruct::RenderState Zenable(D3DRS_ZENABLE,D3DZB_TRUE);
			GraphicsStruct::RenderState ZWrite(D3DRS_ZWRITEENABLE,TRUE);
			GraphicsStruct::RenderState ZFun(D3DRS_ZFUNC,D3DCMP_LESS);
			//GraphicsStruct::RenderState colorWrite2(D3DRS_COLORWRITEENABLE ,D3DCOLORWRITEENABLE_ALPHA);
			GraphicsStruct::RenderState colorWrite(D3DRS_COLORWRITEENABLE ,dwColorWrite);
			GraphicsStruct::RenderStateAlpha Alpha(0x80,TRUE,FALSE,D3DBLEND_SRCALPHA,D3DBLEND_INVSRCALPHA);
			GraphicsStruct::RenderState alphafun(D3DRS_ALPHAFUNC,D3DCMP_GREATEREQUAL);
			

			if(!g_cEngineOption.bCpuSkin)
			{
				BOOL bSaveAlphaBlend = g_cEngineOption.bEnableSpecialAlphaBlend;
				g_cEngineOption.bEnableSpecialAlphaBlend = false;
				if (g_cEngineManager.GetRenderOption(RENDER_SKIN_MESH))
					m_lpCurrentRenderData->RenderVisibleModelSkinForDepth();
				g_cEngineOption.bEnableSpecialAlphaBlend = bSaveAlphaBlend;
			}
		}
	}

    DWORD dwFillMode = D3DFILL_SOLID;
    g_pd3dDevice->GetRenderState(D3DRS_FILLMODE, &dwFillMode);

    if (dwFillMode == D3DFILL_SOLID)
    {
        if (!pEditor->m_bUnderWater && m_pWaterCuller)
        {
           hrRetCode = m_pWaterCuller->RenderWaterBoundingPlanes();
           KGLOG_COM_PROCESS_ERROR(hrRetCode);
        }

        if (m_pOcclusionCuller)
        {
            hrRetCode = m_pOcclusionCuller->OcclusionCulling();
            KGLOG_COM_PROCESS_ERROR(hrRetCode);
        }
    }

    DWORD dwCost = timeGetTime() - dwNowTime;
	if(g_cEngineOption.bEnableTimeOptimizeLog && dwCost >= 20)
	{
		KGLogPrintf(KGLOG_ERR, "TimeOptimize %d 1:%d 2:%d 4:%d 5:%d 6:%d KG3DSceneEntityMgr::RenderForDepthOnly.",
			dwCost,dwCost1,dwCost2,dwCost4,dwCost5,dwCost6);
	}

    hrResult = S_OK;
Exit0:
	return hrResult;
}

HRESULT KG3DSceneEntityMgr::RenderForColorOnly(KG3DSceneSceneEditor* pEditor, KG3DSceneOutputWnd* pWnd)
{
	GraphicsStruct::RenderState colorWrite(D3DRS_COLORWRITEENABLE ,D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_GREEN);

	KG3DLight& rLight = g_cGraphicsTool.GetSunLight(*pEditor);
	KG3DTerrainConverMap* pConverMap = NULL;
	pConverMap = pEditor->m_lpSceneOutDoorSpaceMgr->m_lpConverMap;
	
	KG_PROCESS_ERROR(m_lpCurrentRenderData);	
	
	//////////////////////////////////////////////////////////////////////////

    if (g_cEngineManager.GetRenderOption(RENDER_NORL_MESH) && g_cEngineManager.GetRenderOption(RENDER_ENTITY))
    {
	    m_lpCurrentRenderData->m_Renders[DEFST_NEWLIGHT].RenderForColorOnly(1,FALSE,pConverMap);
		m_lpCurrentRenderData->m_Renders[DEFST_LIGHT_MAP].RenderForColorOnly(1,FALSE,pConverMap);

		{
			GraphicsStruct::SamplerState sa( 0, D3DSAMP_MIPMAPLODBIAS, FtoDW(-1.0f));
			m_lpCurrentRenderData->m_Renders[DEFST_NEWLIGHT_DETAIL].RenderForColorOnly(1,FALSE,pConverMap);
		}

		if(g_cEngineOption.bMultiTextureRender)
		{
			GraphicsStruct::RenderState fog(D3DRS_FOGENABLE,FALSE);
			GraphicsStruct::RenderState cull(D3DRS_CULLMODE,D3DCULL_CCW);
			m_lpStaticModelRender->Render();
		}

		if(g_cEngineOption.bCpuSkin)
		{
			GraphicsStruct::RenderState fog(D3DRS_FOGENABLE,FALSE);
			GraphicsStruct::RenderState cull(D3DRS_CULLMODE,D3DCULL_CCW);
			m_lpSKinModelRender->Render(FALSE);
		}

    }
   
    ////////////////////////////////////////////////////////////////////////

    if (g_cEngineManager.GetRenderOption(RENDER_TREE))
	    m_lpSpeedTreeRender->RenderSpeedTreeLeafsSortWithZpass(FALSE,FALSE,rLight,pConverMap,0);

	/////////////////////s/////////////////////////////////////////////////////

	m_lpCurrentRenderData->RenderVisibleBillboardCloud();

	if(g_cEngineManager.GetRenderOption(RENDER_CLIENT_OBJ))
	{
		GraphicsStruct::RenderState Zenable(D3DRS_ZENABLE,D3DZB_TRUE);
		GraphicsStruct::RenderState ZWrite(D3DRS_ZWRITEENABLE,TRUE);
		GraphicsStruct::RenderState ZFun(D3DRS_ZFUNC,D3DCMP_LESS);

		m_lpCurrentRenderData->RenderVisibleObjectUnSort();
		m_lpCurrentRenderData->RenderVisibleObjectSortByDistance();
	}
	
	//////////////////////////////////////////////////////////////////////////


    RenderVisibleTerrainBlocks(pEditor,TRUE,TRUE,TRUE);

	m_lpCurrentRenderData->RenderModelFeetShadow();//确保这个在地形后面画
	
    if (pWnd)
        pWnd->GetPostRenderManager()->RenderAoe();


    if (g_cEngineManager.GetRenderOption(RENDER_ENTITY))
    {
        GraphicsStruct::RenderState Zenable(D3DRS_ZENABLE,D3DZB_TRUE);
		D3DCMPFUNC enuZFunc = D3DCMP_LESSEQUAL;
		BOOL bZWrite = TRUE;
		if(g_cEngineManager.NeedDrawTwice())
		{
			enuZFunc = 	D3DCMP_EQUAL;
			bZWrite = FALSE;
		}
		GraphicsStruct::RenderState ZWrite(D3DRS_ZWRITEENABLE,bZWrite);
		GraphicsStruct::RenderState ZFun(D3DRS_ZFUNC,enuZFunc);
        GraphicsStruct::RenderState colorWrite(D3DRS_COLORWRITEENABLE ,D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_GREEN);

		if(!g_cEngineOption.bCpuSkin)
		{
            if (g_cEngineManager.GetRenderOption(RENDER_SKIN_MESH))
                m_lpCurrentRenderData->RenderVisibleModelSkinShader(pConverMap,FALSE);
		}
    }
	
	{
		GraphicsStruct::RenderStateAlpha Alpha1(0x60,FALSE,TRUE,D3DBLEND_SRCALPHA,D3DBLEND_INVSRCALPHA);
        m_lpCurrentRenderData->RenderPvsPortal();
		m_lpCurrentRenderData->RenderPoly();
		if(g_cEngineOption.bEnableDebugBBox)
			RenderVisibleObjectBBox();
	}

Exit0:
    return S_OK;
}

static D3DXVECTOR3 s_vec3ViewPoint;

bool Modelsort(KG3DModel* pA, KG3DModel* pB)
{
    D3DXVECTOR3 vec3A(pA->m_matWorld._41, pA->m_matWorld._42, pA->m_matWorld._43);
    D3DXVECTOR3 vec3B(pB->m_matWorld._41, pB->m_matWorld._42, pB->m_matWorld._43);
    return D3DXVec3Length(&(vec3A - s_vec3ViewPoint)) > D3DXVec3Length(&(vec3B - s_vec3ViewPoint));
}

HRESULT KG3DRenderData::RenderSpecialAlphaModel()
{
    HRESULT hrResult = E_FAIL;

    return S_OK; //先关闭SpecialAlpha, 解决外网蓝屏

    D3DXMATRIX matView;
    g_pd3dDevice->GetTransform(D3DTS_VIEW, &matView);
    D3DXMatrixInverse(&matView, NULL, &matView);
    s_vec3ViewPoint = D3DXVECTOR3(matView._41, matView._42, matView._43);

    sort(m_vecSpecialAlphaModel.begin(), m_vecSpecialAlphaModel.end(), Modelsort);

    for (size_t i = 0; i < m_vecSpecialAlphaModel.size(); i++)
    {
        m_vecSpecialAlphaModel[i]->RenderSpecialAlpha(0, NULL);
    }

    hrResult = S_OK;
//Exit0:
    return hrResult;
}

HRESULT KG3DSceneEntityMgr::RenderSpecialAlphaModel()
{
    if (m_lpCurrentRenderData)
    {
        return m_lpCurrentRenderData->RenderSpecialAlphaModel();
    }

    return E_FAIL;
}

HRESULT KG3DSceneEntityMgr::RenderTerrain_Texture()
{		
	return m_lpTerrainRender->RenderTerrainBlocks_OnlyTexture();
}
//HRESULT KG3DSceneEntityMgr::RenderWaterRefractWave(KG3DSceneSceneEditor* pEditor)
//{
//    ASSERT(pEditor);
//
//    GraphicsStruct::RenderState Zenable(D3DRS_ZENABLE,D3DZB_TRUE);
//    GraphicsStruct::RenderState ZWrite(D3DRS_ZWRITEENABLE,FALSE);
//    GraphicsStruct::RenderState ZFun(D3DRS_ZFUNC,D3DCMP_LESS);
//    GraphicsStruct::RenderState colorWrite(D3DRS_COLORWRITEENABLE ,D3DCOLORWRITEENABLE_ALPHA|D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_GREEN);
//
//    BOOL bWaterVisible = m_lpTerrainRender->GetWaterVisible();
//
//    if ((g_cEngineManager.GetRenderOption(RENDER_WATER) && bWaterVisible)|| g_bAlwayRenderWater )
//    {
//        m_lpTerrainRender->RenderWaterRefractWave(pEditor);
//    }
//
//    return S_OK;
//
//}

HRESULT KG3DSceneEntityMgr::RenderWaterRefract(KG3DSceneSceneEditor* pEditor)
{
    ASSERT(pEditor);

    BOOL bWaterVisible = m_lpTerrainRender->IsWaterVisible();

    if ((g_cEngineManager.GetRenderOption(RENDER_WATER) && bWaterVisible)|| g_bAlwayRenderWater )
    {
        GraphicsStruct::RenderState colWrite(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_ALPHA);
        GraphicsStruct::RenderState alpEable(D3DRS_ALPHABLENDENABLE, FALSE);
        GraphicsStruct::RenderState atxEable(D3DRS_ALPHATESTENABLE, FALSE);
        GraphicsStruct::RenderState aaa(D3DRS_COLORVERTEX, FALSE);
        GraphicsStruct::RenderState bbb(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);

        GraphicsStruct::RenderState Zenable(D3DRS_ZENABLE, D3DZB_TRUE);
        GraphicsStruct::RenderState ZWrite(D3DRS_ZWRITEENABLE, FALSE);
        GraphicsStruct::RenderStateCullMode CullMode(D3DCULL_NONE);
        GraphicsStruct::RenderState ZCmp(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);

        D3DMATERIAL9 mtlSave;
        D3DMATERIAL9 mtlWhite;
        
        mtlWhite.Diffuse.r = 1.f;
        mtlWhite.Diffuse.g = 1.f;
        mtlWhite.Diffuse.b = 1.f;
        mtlWhite.Diffuse.a = 1.f;
        
        mtlWhite.Ambient = mtlWhite.Diffuse;
        mtlWhite.Emissive = mtlWhite.Diffuse;
        mtlWhite.Specular = mtlWhite.Diffuse;

        g_pd3dDevice->GetMaterial(&mtlSave);
        g_pd3dDevice->SetMaterial(&mtlWhite);
        g_pd3dDevice->SetTexture(0, NULL);
        m_lpTerrainRender->RenderWaterPrimitive();
        g_pd3dDevice->SetMaterial(&mtlSave);
    }

    return S_OK;

}

HRESULT KG3DSceneEntityMgr::RenderWaterAndSFX(KG3DSceneSceneEditor* pEditor)
{
    HRESULT hrResult = E_FAIL;

	GraphicsStruct::RenderState Zenable(D3DRS_ZENABLE,D3DZB_TRUE);
	GraphicsStruct::RenderState ZWrite(D3DRS_ZWRITEENABLE,FALSE);
	GraphicsStruct::RenderState ZFun(D3DRS_ZFUNC,D3DCMP_LESS);
	GraphicsStruct::RenderState colorWrite(D3DRS_COLORWRITEENABLE ,D3DCOLORWRITEENABLE_ALPHA|D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_GREEN);
    
	KG_PROCESS_ERROR(m_lpCurrentRenderData);

    if ((g_cEngineManager.GetRenderOption(RENDER_WATER) && m_bWaterVisible)|| g_bAlwayRenderWater )
    {
	    m_lpTerrainRender->RenderWater(pEditor,pEditor->m_bUseFFTW);
    }

	if (g_cEngineManager.GetRenderOption(RENDER_WATER))
		m_lpTerrainRender->RenderRiver(pEditor);

	if (g_pd3dDevice->TestCooperativeLevel() == D3D_OK)
	{
		m_lpCurrentRenderData->RenderVisibleEntitySFX(pEditor,&m_CullData);
	}
	
    hrResult = S_OK;
Exit0:
	return hrResult;
}


//////////////////////////////////////////////////////////////////////////

HRESULT KG3DSceneEntityMgrShadow::RenderForShadowMap(KG3DSceneSceneEditor* pEditor,BOOL bTerrain,BOOL bTree,BOOL bObject,BOOL bSkinModel)
{
	GraphicsStruct::RenderState FogEnabled(D3DRS_FOGENABLE,FALSE);
	//////////////////////////////////////////////////////////////////////////
	KG3DLight& rLight = g_cGraphicsTool.GetSunLight(*pEditor);

	KG_PROCESS_ERROR(m_lpCurrentRenderData);

	if(bTree)
		m_lpSpeedTreeRender->RenderSpeedTreeLeafsSortWithZpass(TRUE,FALSE,rLight,NULL,1);

	//if(bObject)
	//{
		//////////////////////////////////////////////////////////////////////////

    if (g_cEngineManager.GetRenderOption(RENDER_NORL_MESH) && bObject)
    {
		m_lpCurrentRenderData->m_Renders[DEFST_NEWLIGHT].RenderDepthOnly(1);
		m_lpCurrentRenderData->m_Renders[DEFST_LIGHT_MAP].RenderDepthOnly(1);
		m_lpCurrentRenderData->m_Renders[DEFST_NEWLIGHT_DETAIL].RenderDepthOnly(1);

		if(g_cEngineOption.bCpuSkin)
		{
			GraphicsStruct::RenderStateAlpha Alpha(0x80,TRUE,FALSE,D3DBLEND_SRCALPHA,D3DBLEND_INVSRCALPHA);
			GraphicsStruct::RenderState alphafun(D3DRS_ALPHAFUNC,D3DCMP_GREATEREQUAL);

			if (g_cEngineManager.GetRenderOption(RENDER_SKIN_MESH))
				m_lpSKinModelRender->Render(FALSE);
		}

		if(g_cEngineOption.bMultiTextureRender)
		{
			GraphicsStruct::RenderStateAlpha Alpha(0x80,TRUE,FALSE,D3DBLEND_SRCALPHA,D3DBLEND_INVSRCALPHA);
			GraphicsStruct::RenderState alphafun(D3DRS_ALPHAFUNC,D3DCMP_GREATEREQUAL);

			m_lpStaticModelRender->Render();
		}
    }

	if(bSkinModel)
	{
		if(g_cEngineOption.bCpuSkin)
		{
			;
		}
		else
		{
            if (g_cEngineManager.GetRenderOption(RENDER_SKIN_MESH))
			    m_lpCurrentRenderData->RenderVisibleModelSkinShader(NULL,FALSE);
		}
	}
	//}

	if(bTerrain)
		m_lpTerrainRender->RenderTerrainBlockDepthOnly(bTerrain,TRUE,TRUE,TRUE,FALSE);

Exit0:
	return S_OK;
}

HRESULT KG3DSceneEntityMgr::SetVisibleGate(float fVisible,float fBillBoardChange)
{
	m_CullData.m_fVisibleAngleGate = fVisible;
	m_CullData.m_fLODMiddleToLowSwitchAngleGate = fBillBoardChange;
    
	return S_OK;
}

HRESULT KG3DSceneEntityMgr::GetSelectableEntityList(/*list<KG3DSceneEntity>*plistEntity, */KG3DScene& scene, BOOL bObject,BOOL bModel)
{
	//KGLOG_PROCESS_ERROR(plistEntity);
    //KGLOG_PROCESS_ERROR(pScene);

	DWORD dwNowTime = timeGetTime();

	//plistEntity->clear();

	KG3DSceneSelectionTool& selTool = scene.GetSelectionTool();
	selTool.ClearSelectable();

	IEKG3DSceneEditorBase* pEditBase = NULL;
	scene.GetSceneEditorBase(&pEditBase);
	_ASSERTE(NULL != pEditBase);

    if (!scene.GetEditSpaceNode() || scene.GetEditSpaceNode() == scene.m_lpSceneOutDoorSpaceMgr)
    {
	    {
		    if(bObject)
		    {
			    vector<KG3DRepresentObject*>::iterator i = m_CullData.m_vecVisiableObjectsRef.begin();
			    vector<KG3DRepresentObject*>::iterator iend = m_CullData.m_vecVisiableObjectsRef.end();
			    while (i!=iend)
			    {
				    KG3DRepresentObject* pObject = *i;

				    if(pObject)
				    {
                        if (!pObject->GetParentObject() || pObject->GetParentObject()->Conver2SpaceNode() == scene.m_lpSceneOutDoorSpaceMgr)
                        {
					       /* KG3DSceneEntity Entity(SCENEENTITY_OBJECT,pObject);
							selTool.SetSelectable(Entity, TRUE);*///<Selectable Refactor>
							pEditBase->IEInsertEntitySelectable(SCENEENTITY_OBJECT, pObject, TRUE);
                        }
				    }
				    ++i;
			    }
		    }

		    if(bModel)
		    {
			    set<KG3DModel*>::iterator s = m_CullData.m_setVisiableModels.begin();
			    set<KG3DModel*>::iterator send = m_CullData.m_setVisiableModels.end();
			    while (s!=send)
			    {
				    KG3DModel* pModel = *s;
				    if (pModel && pModel->m_bSelectable)
				    {
					   /* KG3DSceneEntity Entity(SCENEENTITY_MODEL,pModel);
					   selTool.SetSelectable(Entity, TRUE);*///<Selectable Refactor>
						pEditBase->IEInsertEntitySelectable(SCENEENTITY_MODEL, pModel, TRUE);
				    }
				    ++s;
			    }

		    }
	    }
    }
    else
    {
        KG3DRepresentObjectPVS* pvs = (KG3DRepresentObjectPVS*)scene.GetEditSpaceNode();

        for (set<KG3DRepresentObject*>::iterator i = pvs->m_setObjectVisiable.begin();
            i != pvs->m_setObjectVisiable.end(); ++i)
        {
            /*KG3DSceneEntity e(SCENEENTITY_OBJECT, *i);//<Selectable Refactor>
			selTool.SetSelectable(e, TRUE);*///<Selectable Refactor>
			pEditBase->IEInsertEntitySelectable(SCENEENTITY_OBJECT, *i, TRUE);
        }

        for (list<KG3DPvsPortal*>::iterator i = pvs->m_listPortal.begin(); 
             i != pvs->m_listPortal.end(); ++i)
        {
           /* KG3DSceneEntity e(SCENEENTITY_PVS_PORTAL, *i);
		   selTool.SetSelectable(e, TRUE);*///<Selectable Refactor>
			pEditBase->IEInsertEntitySelectable(SCENEENTITY_PVS_PORTAL, *i, TRUE);
        }
    }


	DWORD dwCost = timeGetTime() - dwNowTime;
	if(dwCost >= 50)
	{
		KGLogPrintf(KGLOG_ERR, "%d KG3DSceneEntityMgr::GetSelectableEntityList.",dwCost);
	}
	return S_OK;
}

HRESULT KG3DSceneEntityMgr::RenderLogicalCells(BOOL bTerrian, BOOL bItem, BOOL bSelected,IELogicalSceneConnection* pLogicalScene,KG3DScene* pScene)
{
	{
		for(size_t i = 0;i < m_CullData.m_vecSceneRegionsIndex.size();++i)
		{
			POINT p = m_CullData.m_vecSceneRegionsIndex[i];
			
			int nXStart = 0;
			int nZStart = 0;

			g_cGraphicsTool.DrawLogicalCells(pLogicalScene,
				p.x,p.y,
				nXStart,nZStart,
				pScene->m_nLogicalSceneX_Start,
				pScene->m_nLogicalSceneZ_Start,
				bTerrian, bItem, bSelected);
		}
	}
	
	return S_OK;
}

HRESULT KG3DSceneEntityMgr::RenderVisibleObjectBBox()
{
	GraphicsStruct::RenderState ZFun(D3DRS_ZFUNC,D3DCMP_LESSEQUAL);

	//{
	//	set<KG3DSceneEntityContainer*>::iterator i = m_CullData.m_setEntityContainer.begin();
	//	set<KG3DSceneEntityContainer*>::iterator iend = m_CullData.m_setEntityContainer.end();
	//	while (i!=iend)
	//	{
	//		KG3DSceneEntityContainer* pCon = *i;

	//		set<KG3DEntityObjectShell*>::iterator s = pCon->setObjectShell.begin();
	//		set<KG3DEntityObjectShell*>::iterator send = pCon->setObjectShell.end();
	//		while (s!=send)
	//		{
	//			KG3DEntityObjectShell* pShell = *s;
	//			//if(!pShell->m_lpObject)
	//			{
	//				static AABBOX bbox;
	//				bbox.A = pShell->m_vBBox_A;
	//				bbox.B = pShell->m_vBBox_B;
	//				g_cGraphicsTool.DrawAABBox(&bbox,0xFFFFFF00);

	//			}

	//			if(!pShell->m_lpObject)
	//			{
	//				int k = 0;
	//			}
	//			++s;
	//		}
	//		i++;
	//	}
	//}
	{
		vector<KG3DRepresentObject*>::iterator i = m_CullData.m_vecVisiableObjectsRef.begin();
		vector<KG3DRepresentObject*>::iterator iend = m_CullData.m_vecVisiableObjectsRef.end();
		while (i!=iend)
		{
			KG3DRepresentObject* pObject = *i;

			g_cGraphicsTool.DrawAABBox(&pObject->m_BBox,0xFF0000FF);
			++i;
		}

	}
	{
		set<KG3DModel*>::iterator i = m_CullData.m_setVisiableModels.begin();
		set<KG3DModel*>::iterator iend = m_CullData.m_setVisiableModels.end();
		while (i!=iend)
		{
			KG3DModel* pModel = *i;

			g_cGraphicsTool.DrawAABBox(&pModel->m_BBox,0xFFFF00FF);
			++i;
		}
	}

	return S_OK;
}

HRESULT KG3DSceneEntityMgr::RenderReflection(KG3DSceneSceneEditor* pEditor,float fWaterHeight,D3DXPLANE& WaterPlaneFFP,D3DXPLANE& WaterPlanePP)
{
	g_pd3dDevice->SetRenderState(D3DRS_CLIPPLANEENABLE,D3DCLIPPLANE0);
	g_pd3dDevice->SetClipPlane(0,(float *)&WaterPlanePP);
	g_pd3dDevice->SetRenderState(D3DRS_CLIPPLANEENABLE,D3DCLIPPLANE0);

	KG3DTerrainConverMap* pConverMap = NULL;
    pConverMap = pEditor->m_lpSceneOutDoorSpaceMgr->m_lpConverMap;
	
    KG3DLight& rLight = g_cGraphicsTool.GetSunLight(*pEditor);
	KG_PROCESS_ERROR(m_lpCurrentRenderData);

	if(1)
	{
		GraphicsStruct::RenderStateAlpha Alpha(0x20,FALSE,FALSE,D3DBLEND_SRCALPHA,D3DBLEND_INVSRCALPHA);

		GraphicsStruct::RenderState Zenable(D3DRS_ZENABLE,D3DZB_TRUE);
		GraphicsStruct::RenderState ZWrite(D3DRS_ZWRITEENABLE,TRUE);
		GraphicsStruct::RenderState ZFun(D3DRS_ZFUNC,D3DCMP_LESS);

		GraphicsStruct::RenderState lightEnable(D3DRS_LIGHTING,FALSE);

		{
			//////////////////////////////////////////////////////////////////////////
			GraphicsStruct::RenderStateAlpha Alpha(0x80,TRUE,FALSE,D3DBLEND_SRCALPHA,D3DBLEND_INVSRCALPHA);
			GraphicsStruct::RenderState colorwrite(D3DRS_COLORWRITEENABLE,D3DCOLORWRITEENABLE_ALPHA);

            if (g_cEngineManager.GetRenderOption(RENDER_NORL_MESH))
            {
			    m_lpCurrentRenderData->m_Renders[DEFST_LIGHT_MAP].RenderDepthOnly(1);
				m_lpCurrentRenderData->m_Renders[DEFST_NEWLIGHT].RenderDepthOnly(1);
                m_lpCurrentRenderData->m_Renders[DEFST_NEWLIGHT_DETAIL].RenderDepthOnly(1);
            }

            if (g_cEngineManager.GetRenderOption(RENDER_TREE))
			    m_lpSpeedTreeRender->RenderSpeedTreeLeafsSortWithZpass(TRUE,FALSE,rLight,pConverMap,0);
		}

        if (g_cEngineManager.GetRenderOption(RENDER_TERRAIN))
		    m_lpTerrainRender->RenderTerrainBlockDepthOnly(TRUE,FALSE,FALSE,FALSE,FALSE);
	}
	//////////////////////////////////////////////////////////////////////////
	if(1)
	{
		GraphicsStruct::RenderState Zenable(D3DRS_ZENABLE,D3DZB_TRUE);
		GraphicsStruct::RenderState ZWrite(D3DRS_ZWRITEENABLE,FALSE);
		GraphicsStruct::RenderState ZFun(D3DRS_ZFUNC,D3DCMP_EQUAL);

		GraphicsStruct::RenderState colorWrite(D3DRS_COLORWRITEENABLE ,0xF);

        RenderVisibleTerrainBlocks(pEditor,FALSE,FALSE,FALSE);

        if (g_cEngineManager.GetRenderOption(RENDER_NORL_MESH))
        {
		    m_lpCurrentRenderData->m_Renders[DEFST_LIGHT_MAP].RenderForColorOnly(1,FALSE,pConverMap);
			m_lpCurrentRenderData->m_Renders[DEFST_NEWLIGHT].RenderForColorOnly(1,FALSE,pConverMap);
            m_lpCurrentRenderData->m_Renders[DEFST_NEWLIGHT_DETAIL].RenderForColorOnly(1,FALSE,pConverMap);
        }

        if (g_cEngineManager.GetRenderOption(RENDER_TREE))
		    m_lpSpeedTreeRender->RenderSpeedTreeLeafsSortWithZpass(FALSE,FALSE,rLight,pConverMap,0);

		if (1)
		{
			GraphicsStruct::RenderState Zenable(D3DRS_ZENABLE,D3DZB_TRUE);
			GraphicsStruct::RenderState ZWrite(D3DRS_ZWRITEENABLE,TRUE);
			GraphicsStruct::RenderState ZFun(D3DRS_ZFUNC,D3DCMP_LESS);
			GraphicsStruct::RenderState colorWrite(D3DRS_COLORWRITEENABLE ,D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_GREEN);

            if (g_cEngineManager.GetRenderOption(RENDER_ENTITY))
            {
				if(g_cEngineOption.bMultiTextureRender)
				{
					GraphicsStruct::RenderState alphafun(D3DRS_ALPHAFUNC,D3DCMP_GREATEREQUAL);
					GraphicsStruct::RenderStateAlpha Alpha(0x80,TRUE,TRUE,D3DBLEND_SRCALPHA,D3DBLEND_INVSRCALPHA);
					GraphicsStruct::RenderState colorWrite(D3DRS_COLORWRITEENABLE ,D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_GREEN);
					GraphicsStruct::RenderState fog(D3DRS_FOGENABLE,FALSE);

					if ( g_cEngineManager.GetRenderOption(RENDER_NORL_MESH))
						m_lpStaticModelRender->Render();
				}

			    if(g_cEngineOption.bCpuSkin)
			    {
				    GraphicsStruct::RenderState alphafun(D3DRS_ALPHAFUNC,D3DCMP_GREATEREQUAL);
				    GraphicsStruct::RenderStateAlpha Alpha(0x80,TRUE,TRUE,D3DBLEND_SRCALPHA,D3DBLEND_INVSRCALPHA);
				    GraphicsStruct::RenderState colorWrite(D3DRS_COLORWRITEENABLE ,D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_GREEN);
					GraphicsStruct::RenderState fog(D3DRS_FOGENABLE,FALSE);

                    if (g_cEngineManager.GetRenderOption(RENDER_SKIN_MESH))
				        m_lpSKinModelRender->Render(FALSE);

			    }
			    else
			    {
                    if (g_cEngineManager.GetRenderOption(RENDER_SKIN_MESH))
				        m_lpCurrentRenderData->RenderVisibleModelSkinShader(pConverMap,TRUE);
			    }
            }

            g_pd3dDevice->SetClipPlane(0,(float *)&WaterPlaneFFP);
            m_lpCurrentRenderData->RenderVisibleEntitySFX(pEditor,&m_CullData);

		}
	}

	g_pd3dDevice->SetRenderState(D3DRS_CLIPPLANEENABLE,FALSE);

Exit0:
	return S_OK;
}

HRESULT KG3DSceneEntityMgr::FrameMove(KG3DSceneSceneEditor* pEditor,BOOL bUpdateSkin)
{
	DWORD dwStartTime = timeGetTime();

	if(m_lpPointLightRender)
	{
		m_lpPointLightRender->FrameMove();
	}	
	

	if(m_lpTerrainRender && (g_dwRenderCount % 100 <= 0))
	{
		m_lpTerrainRender->ProcessPointLighting(m_lpPointLightRender);
	}


    if (g_cEngineOption.bCpuSkin && bUpdateSkin && g_cEngineManager.GetFrameMoveOption(FRAMEMOVE_SKIN_MESH))
	    m_lpSKinModelRender->FrameMove(m_lpPointLightRender);


    if (g_cEngineOption.bMultiTextureRender && g_cEngineManager.GetRenderOption(RENDER_NORL_MESH))
        m_lpStaticModelRender->FrameMove(m_lpPointLightRender);

	DWORD dwCost = timeGetTime() - dwStartTime;
	if(g_cEngineOption.bEnableTimeOptimizeLog && dwCost > 10)
	{
		KGLogPrintf(KGLOG_INFO,"TimeOptimize %d KG3DSceneEntityMgr::FrameMove",dwCost);
	}

	return S_OK;
}

VOID KG3DSceneEntityMgr::EnableCheckVisibleCount( BOOL bEnable )
{
	m_CullData.EnableCheckVisibleCount(bEnable);
	if(NULL != m_lpTerrainRender)
		m_lpTerrainRender->EnableCheckVisibleCount(bEnable);
}

namespace {

    __forceinline bool IsBoxOverlap(D3DXPLANE planes[], const AABBOX& box)
    {
        D3DXVECTOR3 vCenter = box.GetCenter();
        D3DXVECTOR3 vHalf   = vCenter - box.A;

        for (int i = 0; i < 6; ++i)
        {
            FLOAT fDist = 
                planes[i].a * vCenter.x +
                planes[i].b * vCenter.y +
                planes[i].c * vCenter.z +
                planes[i].d * 1.f;

            FLOAT fMaxAbsDist =
                fabs(planes[i].a * vHalf.x) + 
                fabs(planes[i].b * vHalf.y) +
                fabs(planes[i].c * vHalf.z);

            if (fDist < -fMaxAbsDist)
                return false;
        }

        return true;
    
    }

}

int KG3DSceneEntityMgr::GetModelOverlapBox(KG3DModel* pModels[], int maxNum, D3DXVECTOR3 box[8])
{
    int nNum = 0;
    vector<KG3DModel*> vModels;

    if (!pModels || maxNum <= 0)
        return 0;

    for (vector<KG3DRepresentObject*>::iterator i = m_CullData.m_vecVisiableObjectsRef.begin();
         i != m_CullData.m_vecVisiableObjectsRef.end(); ++i)
    {
        KG3DRepresentObject* obj = (*i);

        if (!obj)
            continue;

        obj->GetSubModels(vModels);
    }

    if (vModels.empty())
        return 0;

    D3DXPLANE planes[6];

    D3DXPlaneFromPoints(&planes[0], &box[BOX_C1], &box[BOX_B1], &box[BOX_A1]);
    D3DXPlaneFromPoints(&planes[1], &box[BOX_C2], &box[BOX_D2], &box[BOX_A2]);
    D3DXPlaneFromPoints(&planes[2], &box[BOX_C2], &box[BOX_C1], &box[BOX_D1]);
    D3DXPlaneFromPoints(&planes[3], &box[BOX_B2], &box[BOX_A2], &box[BOX_A1]);
    D3DXPlaneFromPoints(&planes[4], &box[BOX_B1], &box[BOX_B1], &box[BOX_C1]);
    D3DXPlaneFromPoints(&planes[5], &box[BOX_D2], &box[BOX_D1], &box[BOX_A1]);

    for (size_t i = 0; i < vModels.size(); ++i)
    {
        KG3DModel* mdl = vModels[i];

        if (!mdl || !mdl->GetBsp() || !IsBoxOverlap(planes, mdl->GetBBox()))
            continue;
            
        pModels[nNum++] = mdl;
       
        if (nNum >= maxNum)
            break;
    }

    return nNum;
}

void KG3DSceneEntityMgr::GetVisable(DWORD& dwZoom, DWORD& dwSection, DWORD& dwRegion,DWORD& dwGrass)
{
    dwZoom = (DWORD)(m_CullData.m_vecSceneZonesIndex.size());
    dwSection = (DWORD)(m_CullData.m_vecSceneSectionsIndex.size());
    dwRegion = (DWORD)(m_CullData.m_vecSceneRegionsIndex.size());
    dwGrass = m_lpTerrainRender->GetVisibleGrassBlockCount();
}

DWORD KG3DSceneEntityMgr::GetVisableObjectNum()
{
    return (DWORD)(m_CullData.m_vecVisiableObjectsRef.size());
}

DWORD KG3DSceneEntityMgr::GetVisibleClientModels()
{
	if(m_lpStaticModelRender)
	{
		return m_lpStaticModelRender->GetNumModel();
	}
	return 0;
}

HRESULT KG3DSceneEntityMgr::SetVisibleEntity(vector<KG3DModel*>* pvecModel)
{
	{
		m_CullData.m_lpEntityMgrSave = this;
		m_CullData.m_setVisiableModels.clear();

		for (size_t i=0;i<pvecModel->size();i++)
		{
			KG3DModel* pModel = (*pvecModel)[i];
			pModel->SetVisibleCount(g_dwRenderCount);
			m_CullData.m_setVisiableModels.insert(pModel);
		}
		
		m_CullData.CheckUnVisibleEntity();
	
		//////////////////////////////////////////////////////////////////////////
		m_CullData.ExportToRenderData(&m_RenderData[0],this);

	}
	return S_OK;
}

HRESULT KG3DSceneEntityMgr::GetAllModels(vector<IEKG3DModel*>* pvecModels)
{
	if(m_lpSKinModelRender)
	{
		m_lpSKinModelRender->GetAllModels(pvecModels);
	}

	if(m_lpStaticModelRender)
	{
		m_lpStaticModelRender->GetAllModels(pvecModels);
	}
	return S_OK;
}

HRESULT KG3DSceneEntityMgr::AutoReloadModifiedResource()
{
	return S_OK;
}

HRESULT KG3DSceneEntityMgr::ProcessPointLightsForTerrain()
{
	return m_lpTerrainRender->ProcessPointLighting(m_lpPointLightRender);
}

HRESULT KG3DSceneEntityMgr::RenderForTranslucence()
{
	GraphicsStruct::RenderState cull(D3DRS_CULLMODE,D3DCULL_NONE);
	GraphicsStruct::RenderState light(D3DRS_LIGHTING,TRUE);
	GraphicsStruct::RenderState ZWrite(D3DRS_ZWRITEENABLE,FALSE);
	GraphicsStruct::RenderState ZCmp(D3DRS_ZFUNC,D3DCMP_LESS);
	//GraphicsStruct::RenderState alphafun(D3DRS_ALPHAFUNC,D3DCMP_LESS);
	GraphicsStruct::RenderStateAlpha Alpha(0xFE,FALSE,TRUE,D3DBLEND_SRCALPHA,D3DBLEND_INVSRCALPHA);
	GraphicsStruct::RenderState colorWrite(D3DRS_COLORWRITEENABLE ,D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_GREEN);

	m_lpStaticModelRender->Render();
	m_lpSKinModelRender->Render(FALSE);

	return S_OK;
}

KG3DRenderData::KG3DRenderData()
{
	m_vecVisiableSFX.reserve(128);
	m_vecVisiableObjectsNotSort.reserve(256);
	m_vecVisableObjectSortByDistance.reserve(256);

	for (DWORD i = 0; i < DEFST_COUNT; ++i)
	{
		m_Renders[i].m_ShaderType = static_cast<KG3DDefaultShaderType>(i);
	}

	m_dwFaceCount = 0;
	m_dwListObjectRendered = 0;
	m_dwGroupedObjectCount = 0;

    m_SkinShaderRender.Init();
    m_cSubMaterialGroupPool.Init(512, 256);
    m_cSubMaterialGroupSubInfoPool.Init(512, 256);

	m_eState = STATE_EMPTY;
}

KG3DRenderData::~KG3DRenderData()
{
	UnInit();

	m_cSubMaterialGroupPool.UnInit();
	m_cSubMaterialGroupSubInfoPool.UnInit();
}

HRESULT KG3DRenderData::UnInit()
{
	for (DWORD i = 0; i < DEFST_COUNT; ++i)
	{
		m_Renders[i].UnInit(&m_cSubMaterialGroupPool,&m_cSubMaterialGroupSubInfoPool);
	}

	ClearSFX();
	ClearModelRef();

	m_mapBillboardCloudGroup.clear();//<BillboardCloudV1>
	Clear();

    m_SkinShaderRender.UnInit();

    m_vecSpecialAlphaModel.clear();

    return S_OK;
}

HRESULT KG3DCullData::ExportToRenderData(KG3DRenderData* pRenderData,KG3DSceneEntityMgr* pMgr)
{
	KG_PROCESS_ERROR(pRenderData);

	pRenderData->Clear();

	pRenderData->m_dwFaceCount = 0;
	
	{
		pRenderData->m_vecVisiableObjectsRef.reserve(m_vecVisiableObjectsRef.size());
		vector<KG3DRepresentObject*>::iterator i = m_vecVisiableObjectsRef.begin();
		vector<KG3DRepresentObject*>::iterator iend = m_vecVisiableObjectsRef.end();
		while (i!=iend)
		{
			KG3DRepresentObject* pObject = *i;
			if(pObject)
			{
				pObject->AddRef();
                pRenderData->m_vecVisiableObjectsRef.push_back(pObject);
			}
			++i;
		}
	}

	{
	    pRenderData->m_vecVisiableModelsRef.reserve(m_setVisiableModels.size());
		set<KG3DModel*>::iterator i = m_setVisiableModels.begin();
		set<KG3DModel*>::iterator iend = m_setVisiableModels.end();
		while (i!=iend)
		{
			KG3DModel* pModel = *i;
			if(pModel)
			{
				pModel->AddRef();
                pRenderData->m_vecVisiableModelsRef.push_back(pModel);
			}
			++i;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	pRenderData->SortVisibleEntity(this,pMgr);

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DRenderData::Clear()
{
	{
		vector<KG3DRepresentObject*>::iterator i = m_vecVisiableObjectsRef.begin();
		vector<KG3DRepresentObject*>::iterator iend = m_vecVisiableObjectsRef.end();
		while (i!=iend)
		{
			KG3DRepresentObject* pObject = *i;
			SAFE_RELEASE(pObject);
			++i;
		}
		m_vecVisiableObjectsRef.clear();
	}

	{
		vector<KG3DModel*>::iterator i = m_vecVisiableModelsRef.begin();
		vector<KG3DModel*>::iterator iend = m_vecVisiableModelsRef.end();
		while (i!=iend)
		{
			KG3DModel* pModel = *i;
			SAFE_RELEASE(pModel);
			++i;
		}
		m_vecVisiableModelsRef.clear();
	}

	return S_OK;
}

HRESULT KG3DRenderData::ClearSFX()
{
	for (size_t i=0;i<m_vecVisiableSFX.size();i++)
	{
		SAFE_RELEASE(m_vecVisiableSFX[i]);
	}
	m_vecVisiableSFX.clear();
	m_vecPostRenderSFX.clear();
	return S_OK;
}

HRESULT KG3DRenderData::ClearModelRef()
{
	for (size_t i = 0; i < m_vecModelRef.size(); ++i)
	{
		KG3DModel* pModel = m_vecModelRef[i];
		SAFE_RELEASE(pModel);
	}
	m_vecModelRef.clear();
	return S_OK;
}

HRESULT KG3DSceneEntityMgr::ShockWaveRender()
{	
	if(m_lpCurrentRenderData)
	{
		return m_lpCurrentRenderData->ShockWaveRender();
	}

	return S_OK;
}
