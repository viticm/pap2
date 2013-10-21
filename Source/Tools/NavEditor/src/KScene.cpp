#define _WIN32_WINNT 0x0500 
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <Windows.h>
#include <Windowsx.h>
#include <zmouse.h>
#include <WinUser.h>
#include "KApplication.h"
#include "KScene.h"
#include "XScene.h"
#include "KMainPlayer.h"
#include "MathTools.h"

using namespace std;

#define GRID_SIZE 1.0f
#define TERRAIN_REGION_SIZE 32

KScene g_Scene;

#pragma pack(1)
struct KTerrainHeader
{
    int   nVertexCountX;
    int   nVertexCountZ;
};

struct KEntityHeader
{
    int nVertexCount;
    int nTriangleCount;
};

struct KVertexData
{
    float x;
    float y;
    float z;
};

struct KTriangleData
{
    int nA;
    int nB;
    int nC;
};
#pragma pack()

KScene::KScene(void)
{
    m_pRegionTable      = NULL;
    m_bDrawUpFaceOnly   = false;
    m_bPR               = false;
    m_bDrawSolid        = true;
    m_bDrawWireframe    = false;
}

KScene::~KScene(void)
{
}

BOOL KScene::Setup()
{
    BOOL bResult    = false;
    BOOL bRetCode   = false;
    float fHeight = 0.0f;

    bRetCode = Load("Mesh.dat");
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

void KScene::Clear()
{
    for (KEntityList::iterator it = m_EntityList.begin(); it != m_EntityList.end(); ++it)
    {
        KEntity* pEntity = *it;

        KG_DELETE_ARRAY(pEntity->pVertex);
        KG_DELETE_ARRAY(pEntity->pTriangle);
        KG_DELETE(pEntity);
    }

    for (int nZ = 0; nZ < m_nRegionCountZ; nZ++)
    {
        for (int nX = 0; nX < m_nRegionCountX; nX++)
        {
            KTerrainRegion* pRegion = GetTerrainRegion(nX, nZ);
            if (pRegion)
            {
                KG_DELETE_ARRAY(pRegion->pVertex);
                KG_DELETE_ARRAY(pRegion->pTriangle);
            }
        }
    }

    KG_DELETE_ARRAY(m_pRegionTable);

    m_EntityList.clear();
}

BOOL KScene::Load(const char cszFileName[])
{
    BOOL            bResult         = false;
    int             nRetCode        = false;
    FILE*           pFile           = NULL;
    long            lFileSize       = 0;
    BYTE*           pbyBuffer       = NULL;
    size_t          uRead           = 0;
    BYTE*           pbyPos          = NULL;
    BYTE*           pbyTail         = NULL;
    int             nVertexCount    = 0;
    KTerrainHeader* pTerrainHeader  = NULL;
    size_t          uTerrainDataLen = 0;

    pFile = fopen(cszFileName, "rb");
    KG_PROCESS_ERROR(pFile);

    nRetCode = fseek(pFile, 0, SEEK_END);
    KG_PROCESS_ERROR(nRetCode == 0);

    lFileSize = ftell(pFile);
    KG_PROCESS_ERROR(lFileSize > 0);

    nRetCode = fseek(pFile, 0, SEEK_SET);
    KG_PROCESS_ERROR(nRetCode == 0);

    pbyBuffer = new BYTE[lFileSize];
    KG_PROCESS_ERROR(pbyBuffer);

    uRead = fread(pbyBuffer, lFileSize, 1, pFile);
    KG_PROCESS_ERROR(uRead == 1);

    pbyPos = pbyBuffer;
    pbyTail = pbyBuffer + lFileSize;

    KG_PROCESS_ERROR(pbyTail - pbyPos >= sizeof(KTerrainHeader));
    pTerrainHeader = (KTerrainHeader*)pbyPos;
    pbyPos += sizeof(KTerrainHeader);

    uTerrainDataLen = sizeof(float) * pTerrainHeader->nVertexCountX * pTerrainHeader->nVertexCountZ;
    KG_PROCESS_ERROR(pbyTail - pbyPos >= (int)uTerrainDataLen);

    nRetCode = LoadTerrain(pTerrainHeader->nVertexCountX, pTerrainHeader->nVertexCountZ, (float*)pbyPos);
    KG_PROCESS_ERROR(nRetCode);
    pbyPos += uTerrainDataLen;

    while (pbyPos < pbyTail)
    {
        KEntityHeader* pEntity          = NULL;
        size_t         uEntityDataLen   = 0;

        KG_PROCESS_ERROR(pbyTail - pbyPos >= sizeof(KEntityHeader));
        pEntity = (KEntityHeader*)pbyPos;
        pbyPos += sizeof(KEntityHeader);

        uEntityDataLen = sizeof(KVertexData) * pEntity->nVertexCount + sizeof(KTriangleData) * pEntity->nTriangleCount;
        KG_PROCESS_ERROR(pbyTail - pbyPos >= (int)uEntityDataLen);

        cprintf(
            "Vertex = %d, Triangle = %d --> %.1f\n",
            pEntity->nVertexCount,
            pEntity->nTriangleCount,
            (float)pEntity->nVertexCount / (float)pEntity->nTriangleCount
        );

        nRetCode = LoadEntity(pEntity->nVertexCount, pEntity->nTriangleCount, pbyPos, uEntityDataLen);
        KG_PROCESS_ERROR(nRetCode);

        pbyPos += uEntityDataLen;
    }

    for (int nZ = 0; nZ < m_nRegionCountZ; nZ++)
    {
        for (int nX = 0; nX < m_nRegionCountX; nX++)
        {
            KTerrainRegion* pRegion = GetTerrainRegion(nX, nZ);
            KG_PROCESS_ERROR(pRegion);

            D3DXVECTOR3 vMin((float)TERRAIN_REGION_SIZE * nX, 0.0f, (float)TERRAIN_REGION_SIZE * nZ);
            D3DXVECTOR3 vMax((float)TERRAIN_REGION_SIZE * (nX + 1), FLT_MAX, (float)TERRAIN_REGION_SIZE * (nZ + 1));

            for (KEntityList::iterator it = m_EntityList.begin(); it != m_EntityList.end(); ++it)
            {
                KEntity* pEntity = *it;

                nRetCode = IntersectSectionBox2Box(vMin, vMax, pEntity->vMin, pEntity->vMax);
                if (nRetCode)
                {
                    pRegion->EntityList.push_back(pEntity);
                }
            }
        }
    }

    for (KEdgeMap::iterator it = m_EdgeMap.begin(), itEnd = m_EdgeMap.end(); it != itEnd; ++it)
    {
        if (it->second != 1)
            continue;

        (it->first.first)->bFix = true;
        (it->first.second)->bFix = true;
    }

    bResult = true;
Exit0:
    KG_DELETE_ARRAY(pbyBuffer);
    if (pFile)
    {
        fclose(pFile);
        pFile = NULL;
    }
    return bResult;
}



BOOL KScene::LoadTerrain(int nVertexCountX, int nVertexCountZ, float fHeight[])
{ 
    BOOL                bResult     = false;
    KTerrainRegion*     pRegion     = NULL;
    KVertexEx*          pVertex     = NULL;
    KTriangleEx*        pTriangle   = NULL;
    KTerrainRegion*     pRegionAll  = NULL;

    m_nRegionCountX = (nVertexCountX - 1) / TERRAIN_REGION_SIZE;
    m_nRegionCountZ = (nVertexCountZ - 1) / TERRAIN_REGION_SIZE;

    m_pRegionTable = new KTerrainRegion[m_nRegionCountX * m_nRegionCountZ];
    KG_PROCESS_ERROR(m_pRegionTable);

    for (int i = 0; i < m_nRegionCountX * m_nRegionCountZ; i++)
    {
        m_pRegionTable[i].pVertex = NULL;
        m_pRegionTable[i].pTriangle = NULL;
    }

    for (int nZ = 0; nZ < m_nRegionCountZ; nZ++)
    {
        for (int nX = 0; nX < m_nRegionCountX; nX++)
        {
            int             nBaseX  = TERRAIN_REGION_SIZE * nX;
            int             nBaseZ  = TERRAIN_REGION_SIZE * nZ;
            float           fMin    = FLT_MAX;
            float           fMax    = FLT_MIN;
            KTriangleEx*    pTri    = NULL;

            pRegion = GetTerrainRegion(nX, nZ);
            KG_PROCESS_ERROR(pRegion);

            pVertex = new KVertexEx[(TERRAIN_REGION_SIZE + 1) * (TERRAIN_REGION_SIZE + 1)];
            KG_PROCESS_ERROR(pVertex);

            pTriangle = new KTriangleEx[TERRAIN_REGION_SIZE * TERRAIN_REGION_SIZE * 2];
            KG_PROCESS_ERROR(pTriangle);

            // 设置每个顶点坐标
            for (int i = 0; i < TERRAIN_REGION_SIZE + 1; i++)
            {
                for (int j = 0; j < TERRAIN_REGION_SIZE + 1; j++)
                {
                    int     nIndex  = nVertexCountX * (nBaseZ + i) + nBaseX + j;
                    float   fPointH = fHeight[nIndex] / 100.0f;

                    pVertex[(TERRAIN_REGION_SIZE + 1) * i + j].vPos.x = GRID_SIZE * (nBaseX + j);
                    pVertex[(TERRAIN_REGION_SIZE + 1) * i + j].vPos.z = GRID_SIZE * (nBaseZ + i);
                    pVertex[(TERRAIN_REGION_SIZE + 1) * i + j].vPos.y = fPointH;

                    if (fPointH > fMax)
                    {
                        fMax = fPointH;
                    }

                    if (fPointH < fMin)
                    {
                        fMin = fPointH;
                    }
                }
            }

            pTri = pTriangle;
            // 设置索引
            for (int i = 0; i < TERRAIN_REGION_SIZE; i++)
            {
                for (int j = 0; j < TERRAIN_REGION_SIZE; j++)
                {
                    int nLeftBottom     = (TERRAIN_REGION_SIZE + 1) * i + j;
                    int nRightBottom    = (TERRAIN_REGION_SIZE + 1) * i + j + 1;
                    int nLeftTop        = nLeftBottom + TERRAIN_REGION_SIZE + 1;
                    int nRightTop       = nRightBottom +TERRAIN_REGION_SIZE + 1;

                    // 下三角
                    pTri->pVertexA = pVertex + nLeftBottom;
                    pTri->pVertexB = pVertex + nRightBottom;
                    pTri->pVertexC = pVertex + nRightTop;

                    // 下三角顶点邻接关系
                    pVertex[nLeftBottom].Neighbors.insert(pVertex + nRightTop);
                    pVertex[nLeftBottom].Neighbors.insert(pVertex + nRightBottom);

                    pVertex[nRightTop].Neighbors.insert(pVertex + nLeftBottom);
                    pVertex[nRightTop].Neighbors.insert(pVertex + nRightBottom);

                    pVertex[nRightBottom].Neighbors.insert(pVertex + nLeftBottom);
                    pVertex[nRightBottom].Neighbors.insert(pVertex + nRightTop);

                    pVertex[nLeftBottom].Faces.insert(pTri);
                    pVertex[nRightTop].Faces.insert(pTri);
                    pVertex[nRightBottom].Faces.insert(pTri);

                    pTri++;

                    // 上三角
                    pTri->pVertexA = pVertex + nLeftBottom;
                    pTri->pVertexB = pVertex + nRightTop;
                    pTri->pVertexC = pVertex + nLeftTop;

                    // 上三角顶点邻接关系
                    pVertex[nLeftBottom].Neighbors.insert(pVertex + nRightTop);
                    pVertex[nLeftBottom].Neighbors.insert(pVertex + nLeftTop);

                    pVertex[nRightTop].Neighbors.insert(pVertex + nLeftBottom);
                    pVertex[nRightTop].Neighbors.insert(pVertex + nLeftTop);

                    pVertex[nLeftTop].Neighbors.insert(pVertex + nLeftBottom);
                    pVertex[nLeftTop].Neighbors.insert(pVertex + nRightTop);

                    pVertex[nLeftTop].Faces.insert(pTri);
                    pVertex[nLeftBottom].Faces.insert(pTri);
                    pVertex[nRightTop].Faces.insert(pTri);

                    pTri++;
                }
            }

            pRegion->pVertex = pVertex;
            pRegion->nVertexCount = (TERRAIN_REGION_SIZE + 1) * (TERRAIN_REGION_SIZE + 1);

            pRegion->pTriangle = pTriangle;
            pRegion->nTriangleCount = TERRAIN_REGION_SIZE * TERRAIN_REGION_SIZE * 2;

            UpdateNormal(pRegion->pVertex, pRegion->nVertexCount, pRegion->pTriangle, pRegion->nTriangleCount);

            pVertex     = NULL;
            pTriangle   = NULL;
        }
    }

    //////////////////////////////////////////////////////////////////////////

    {
        KTriangleEx*    pTri        = NULL;
        DWORD           dwOldTick   = 0;
        DWORD           dwNewTick   = 0;

        dwOldTick = GetTickCount();

        pRegionAll = new KTerrainRegion;
        KG_PROCESS_ERROR(pRegionAll);

        pRegionAll->pVertex = NULL;
        pRegionAll->pTriangle = NULL;

        pRegionAll->pVertex = new KVertexEx[(nVertexCountX + 1) * (nVertexCountZ + 1)];
        KG_PROCESS_ERROR(pRegionAll->pVertex);

        pRegionAll->pTriangle = new KTriangleEx[nVertexCountX * nVertexCountZ * 2];
        KG_PROCESS_ERROR(pRegionAll->pTriangle);

        pVertex = pRegionAll->pVertex;

        // 设置每个顶点坐标
        for (int i = 0; i < nVertexCountZ; i++)
        {
            for (int j = 0; j < nVertexCountX; j++)
            {
                int     nIndex  = nVertexCountX * i + j;
                float   fPointH = fHeight[nIndex] / 100.0f;

                pVertex[(nVertexCountX) * i + j].vPos.x = GRID_SIZE * (j);
                pVertex[(nVertexCountX) * i + j].vPos.z = GRID_SIZE * (i);
                pVertex[(nVertexCountX) * i + j].vPos.y = fPointH;
            }
        }

        pTri = pRegionAll->pTriangle;
        // 设置索引
        for (int i = 0; i < nVertexCountZ - 1; i++)
        {
            for (int j = 0; j < nVertexCountX - 1; j++)
            {
                int nLeftBottom     = (nVertexCountX) * i + j;
                int nRightBottom    = (nVertexCountX) * i + j + 1;
                int nLeftTop        = nLeftBottom + nVertexCountX;
                int nRightTop       = nRightBottom + nVertexCountX;
                std::pair<KVertexEx*, KVertexEx*> Edge;

                // 下三角
                pTri->pVertexA = pVertex + nLeftBottom;
                pTri->pVertexB = pVertex + nRightBottom;
                pTri->pVertexC = pVertex + nRightTop;

                // 下三角顶点邻接关系
                pVertex[nLeftBottom].Neighbors.insert(pVertex + nRightTop);
                pVertex[nLeftBottom].Neighbors.insert(pVertex + nRightBottom);

                pVertex[nRightTop].Neighbors.insert(pVertex + nLeftBottom);
                pVertex[nRightTop].Neighbors.insert(pVertex + nRightBottom);

                pVertex[nRightBottom].Neighbors.insert(pVertex + nLeftBottom);
                pVertex[nRightBottom].Neighbors.insert(pVertex + nRightTop);

                pVertex[nLeftBottom].Faces.insert(pTri);
                pVertex[nRightTop].Faces.insert(pTri);
                pVertex[nRightBottom].Faces.insert(pTri);

                Edge = std::make_pair(
                    nLeftBottom < nRightTop ? pVertex + nLeftBottom : pVertex + nRightTop, 
                    nLeftBottom < nRightTop ? pVertex + nRightTop : pVertex + nLeftBottom
                );
                m_EdgeMap[Edge]++;

                Edge = std::make_pair(
                    nLeftBottom < nRightBottom ? pVertex + nLeftBottom : pVertex + nRightBottom, 
                    nLeftBottom < nRightBottom ? pVertex + nRightBottom : pVertex + nLeftBottom
                );
                m_EdgeMap[Edge]++;

                Edge = std::make_pair(
                    nRightBottom < nRightTop ? pVertex + nRightBottom : pVertex + nRightTop, 
                    nRightBottom < nRightTop ? pVertex + nRightTop : pVertex + nRightBottom
                );
                m_EdgeMap[Edge]++;

                pTri++;

                // 上三角
                pTri->pVertexA = pVertex + nLeftBottom;
                pTri->pVertexB = pVertex + nRightTop;
                pTri->pVertexC = pVertex + nLeftTop;

                // 上三角顶点邻接关系
                pVertex[nLeftBottom].Neighbors.insert(pVertex + nRightTop);
                pVertex[nLeftBottom].Neighbors.insert(pVertex + nLeftTop);

                pVertex[nRightTop].Neighbors.insert(pVertex + nLeftBottom);
                pVertex[nRightTop].Neighbors.insert(pVertex + nLeftTop);

                pVertex[nLeftTop].Neighbors.insert(pVertex + nLeftBottom);
                pVertex[nLeftTop].Neighbors.insert(pVertex + nRightTop);

                pVertex[nLeftTop].Faces.insert(pTri);
                pVertex[nLeftBottom].Faces.insert(pTri);
                pVertex[nRightTop].Faces.insert(pTri);

                Edge = std::make_pair(
                    nLeftBottom < nRightTop ? pVertex + nLeftBottom : pVertex + nRightTop, 
                    nLeftBottom < nRightTop ? pVertex + nRightTop : pVertex + nLeftBottom
                );
                m_EdgeMap[Edge]++;

                Edge = std::make_pair(
                    nLeftBottom < nLeftTop ? pVertex + nLeftBottom : pVertex + nLeftTop, 
                    nLeftBottom < nLeftTop ? pVertex + nLeftTop : pVertex + nLeftBottom
                );
                m_EdgeMap[Edge]++;

                Edge = std::make_pair(
                    nLeftTop < nRightTop ? pVertex + nLeftTop : pVertex + nRightTop, 
                    nLeftTop < nRightTop ? pVertex + nRightTop : pVertex + nLeftTop
                );
                m_EdgeMap[Edge]++;

                pTri++;
            }
        }

        pRegionAll->nVertexCount = (nVertexCountX + 1) * (nVertexCountZ + 1);

        pRegionAll->nTriangleCount = nVertexCountX * nVertexCountZ * 2;

        UpdateNormal(pRegionAll->pVertex, pRegionAll->nVertexCount, pRegionAll->pTriangle, pRegionAll->nTriangleCount);

        ComputeEdgeCollapseCostsAtVertex(pRegionAll->pVertex, pRegionAll->nVertexCount, &pRegionAll->CostMap);
        m_pRegionAll    = pRegionAll;
        pRegionAll      = NULL;
        pVertex         = NULL;
        pTriangle       = NULL;

        dwNewTick = GetTickCount();
        cprintf("Load terrain use %lu ms.\n", dwNewTick - dwOldTick);
    }

    bResult = true;
Exit0:
    KG_DELETE_ARRAY(pVertex);
    KG_DELETE_ARRAY(pTriangle);
    KG_DELETE(pRegionAll);
    return bResult;
}

BOOL KScene::LoadEntity(int nVertexCount, int nTriangleCount, BYTE* pbyData, size_t uDataLen)
{
    BOOL            bResult         = false;
    KVertexData*    pVertexData     = NULL;
    KTriangleData*  pTriangleData   = NULL;
    KVertexEx*      pVertex         = NULL;
    KTriangleEx*    pTriangle       = NULL;
    KEntity*        pEntity         = NULL;
    int*            pnReindexArray  = NULL;
    D3DXVECTOR3     vMin(FLT_MAX, FLT_MAX, FLT_MAX);
    D3DXVECTOR3     vMax(FLT_MIN, FLT_MIN, FLT_MIN);

    pVertexData = (KVertexData*)pbyData;
    pTriangleData = (KTriangleData*)(pbyData + sizeof(KVertexData) * nVertexCount);

    pEntity = new KEntity;
    KG_PROCESS_ERROR(pEntity);

    pVertex = new KVertexEx[nVertexCount];
    KG_PROCESS_ERROR(pVertex);

    pnReindexArray = new int[nVertexCount];
    KG_PROCESS_ERROR(pnReindexArray);

    pTriangle = new KTriangleEx[nTriangleCount];
    KG_PROCESS_ERROR(pTriangle);

    for (int i = 0; i < nVertexCount; i++)
        pnReindexArray[i] = i;

    for (int i = 0; i < nVertexCount; i++)
    {
        pVertex[i].vPos.x   = pVertexData[i].x / 100.0f;
        pVertex[i].vPos.y   = pVertexData[i].y / 100.0f;
        pVertex[i].vPos.z   = pVertexData[i].z / 100.0f;

        vMin.x = min(vMin.x, pVertex[i].vPos.x);
        vMin.y = min(vMin.y, pVertex[i].vPos.y);
        vMin.z = min(vMin.z, pVertex[i].vPos.z);

        vMax.x = max(vMax.x, pVertex[i].vPos.x);
        vMax.y = max(vMax.y, pVertex[i].vPos.y);
        vMax.z = max(vMax.z, pVertex[i].vPos.z);

        for (int j = 0; j < i; j++)
        {
            D3DXVECTOR3 vDelta = pVertex[i].vPos - pVertex[j].vPos; 
            if (D3DXVec3Length(&vDelta) < 0.0000001f)
            {
                pnReindexArray[i] = j;
                break;
            }
        }
    }

    for (int i = 0; i < nTriangleCount; i++)
    {
        std::pair<KVertexEx*, KVertexEx*> Edge;

        pTriangle[i].pVertexA = pVertex + pnReindexArray[pTriangleData[i].nA];
        pTriangle[i].pVertexB = pVertex + pnReindexArray[pTriangleData[i].nB];
        pTriangle[i].pVertexC = pVertex + pnReindexArray[pTriangleData[i].nC];

        pTriangle[i].pVertexA->Faces.insert(pTriangle + i);
        pTriangle[i].pVertexB->Faces.insert(pTriangle + i);
        pTriangle[i].pVertexC->Faces.insert(pTriangle + i);

        pTriangle[i].pVertexA->Neighbors.insert(pTriangle[i].pVertexB);
        pTriangle[i].pVertexA->Neighbors.insert(pTriangle[i].pVertexC);

        pTriangle[i].pVertexB->Neighbors.insert(pTriangle[i].pVertexA);
        pTriangle[i].pVertexB->Neighbors.insert(pTriangle[i].pVertexC);

        pTriangle[i].pVertexC->Neighbors.insert(pTriangle[i].pVertexB);
        pTriangle[i].pVertexC->Neighbors.insert(pTriangle[i].pVertexA);

        Edge = std::make_pair(
            pTriangle[i].pVertexA < pTriangle[i].pVertexB ? pTriangle[i].pVertexA : pTriangle[i].pVertexB, 
            pTriangle[i].pVertexA < pTriangle[i].pVertexB ? pTriangle[i].pVertexB : pTriangle[i].pVertexA
        );
        m_EdgeMap[Edge]++;

        Edge = std::make_pair(
            pTriangle[i].pVertexA < pTriangle[i].pVertexC ? pTriangle[i].pVertexA : pTriangle[i].pVertexC, 
            pTriangle[i].pVertexA < pTriangle[i].pVertexC ? pTriangle[i].pVertexC : pTriangle[i].pVertexA
        );
        m_EdgeMap[Edge]++;

        Edge = std::make_pair(
            pTriangle[i].pVertexB < pTriangle[i].pVertexC ? pTriangle[i].pVertexB : pTriangle[i].pVertexC, 
            pTriangle[i].pVertexB < pTriangle[i].pVertexC ? pTriangle[i].pVertexC : pTriangle[i].pVertexB 
        );
        m_EdgeMap[Edge]++;
    }

    pEntity->pVertex = pVertex;
    pEntity->nVertexCount = nVertexCount;

    pEntity->pTriangle = pTriangle;
    pEntity->nTriangleCount = nTriangleCount;

    pEntity->vMin   = vMin;
    pEntity->vMax   = vMax;
    pEntity->bDraw  = true;

    UpdateNormal(pEntity->pVertex, pEntity->nVertexCount, pEntity->pTriangle, pEntity->nTriangleCount);

    ComputeEdgeCollapseCostsAtVertex(pEntity->pVertex, pEntity->nVertexCount, &pEntity->CostMap);

    m_EntityList.push_back(pEntity);

    bResult = true;
Exit0:
    if (!bResult)
    {
        KG_DELETE(pEntity);
        KG_DELETE_ARRAY(pVertex);
        KG_DELETE_ARRAY(pTriangle);
    }
    return bResult;
}

void KScene::UpdateViewArea(float fX, float fZ)
{
    m_nViewRegionX = (int)((fX - 1) / TERRAIN_REGION_SIZE);
    m_nViewRegionZ = (int)((fZ - 1) / TERRAIN_REGION_SIZE);

    KEEP_RANGE(m_nViewRegionX, 0, m_nRegionCountX - 1);
    KEEP_RANGE(m_nViewRegionZ, 0, m_nRegionCountZ - 1);
}

void KScene::Draw(IDirect3DDevice9* piDevice)
{
    KTerrainRegion* pRegion     = NULL;
    D3DXMATRIX      Matrix;

    D3DXMatrixTranslation(&Matrix, 0.0f, 0.0f, 0.0f);
    piDevice->SetTransform(D3DTS_WORLD, &Matrix);

    piDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(100, 50, 50));

    Draw(piDevice, m_pRegionAll->pVertex, m_pRegionAll->nVertexCount, m_pRegionAll->pTriangle, m_pRegionAll->nTriangleCount, m_bDrawSolid, m_bDrawWireframe);

    for (int nZ = m_nViewRegionZ - 3; nZ <= m_nViewRegionZ + 3; nZ++)
    {
        for (int nX = m_nViewRegionX - 3; nX <= m_nViewRegionX + 3; nX++)
        {
            pRegion = GetTerrainRegion(nX, nZ);
            if (!pRegion)
                continue;

            KTriangleEx*    pTriangle   = new KTriangleEx[pRegion->nTriangleCount];
            int             nTriangle   = 0;

            for (int i = 0; i < pRegion->nTriangleCount; i++)
            {
                KTriangleEx* pTri = pRegion->pTriangle + i;
                
                if (pTri->vNormal.y > 0.70 || !m_bDrawUpFaceOnly)
                {
                    pTriangle[nTriangle++] = pRegion->pTriangle[i];
                }
            }

            piDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(50, 50, 100));

            Draw(
                piDevice, 
                pRegion->pVertex, pRegion->nVertexCount,
                pTriangle, nTriangle, !m_bDrawSolid, false
            );

            KG_DELETE_ARRAY(pTriangle);

            for (KEntityList::iterator it = pRegion->EntityList.begin(); it != pRegion->EntityList.end(); ++it)
            {
                //(*it)->bDraw = true;
            }
        }
    }

    piDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(50, 100, 50));

    for (KEntityList::iterator it = m_EntityList.begin(); it != m_EntityList.end(); ++it)
    {
        KEntity* pEntity = *it;

        if (pEntity->bDraw)
        {
            KTriangleEx*    pTriangle   = new KTriangleEx[pEntity->nTriangleCount];
            int             nTriangle   = 0;

            for (int i = 0; i < pEntity->nTriangleCount; i++)
            {
                KTriangleEx* pTri = pEntity->pTriangle + i;

                if (pTri->vNormal.y > 0.70 || !m_bDrawUpFaceOnly)
                {
                    pTriangle[nTriangle++] = pEntity->pTriangle[i];
                }
            }

            Draw(
                piDevice, 
                pEntity->pVertex, pEntity->nVertexCount,
                pTriangle, nTriangle, true, m_bDrawWireframe
            );

            KG_DELETE_ARRAY(pTriangle);

            // pEntity->bDraw = false;
        }
    }
}

BYTE KScene::Draw(
    IDirect3DDevice9* piDevice,
    KVertexEx* pVertex, int nVertexCount,
    KTriangleEx* pTriangle, int nTriangleCount,
    BOOL bSolid, 
    BOOL bWireframe
)
{
    BOOL                    bResult         = false;
    HRESULT                 hRetCode        = E_FAIL;
    IDirect3DVertexBuffer9* piVertexBuffer  = NULL;
    IDirect3DIndexBuffer9*  piIndexBuffer   = NULL;
    void*                   pvBuffer        = NULL;

    hRetCode = piDevice->CreateVertexBuffer(
        sizeof(KVertex) * nVertexCount, 0, 
        VERTEX_FVF, D3DPOOL_MANAGED, &piVertexBuffer, 0
    );
    KG_PROCESS_ERROR(!FAILED(hRetCode));

    hRetCode = piVertexBuffer->Lock(0, 0, &pvBuffer, D3DLOCK_DISCARD);
    KG_PROCESS_ERROR(!FAILED(hRetCode));

    for (int i = 0; i < nVertexCount; i++)
    {
        KVertex* pV = (KVertex*)pvBuffer + i;
        pV->vPos    = pVertex[i].vPos;
        pV->vNormal = pVertex[i].vNormal;
    }
    piVertexBuffer->Unlock();

    hRetCode = piDevice->CreateIndexBuffer(
        sizeof(KTriangle) * nTriangleCount, 0, 
        D3DFMT_INDEX32, D3DPOOL_MANAGED, &piIndexBuffer, 0
    );
    KG_PROCESS_ERROR(!FAILED(hRetCode));

    hRetCode = piIndexBuffer->Lock(0, 0, &pvBuffer, D3DLOCK_DISCARD);
    KG_PROCESS_ERROR(!FAILED(hRetCode));

    for (int i = 0; i < nTriangleCount; i++)
    {
        KTriangle* pT = (KTriangle*)pvBuffer + i;

        if (!pTriangle[i].pVertexA)
            pT->nA = 0;
        else
            pT->nA = (int)(pTriangle[i].pVertexA - pVertex);

        if (!pTriangle[i].pVertexB)
            pT->nB = 0;
        else
            pT->nB = (int)(pTriangle[i].pVertexB - pVertex);

        if (!pTriangle[i].pVertexC)
            pT->nC = 0;
        else
            pT->nC = (int)(pTriangle[i].pVertexC - pVertex);
    }
    piIndexBuffer->Unlock();

    piDevice->SetStreamSource(0, piVertexBuffer, 0, sizeof(KVertex));
    piDevice->SetIndices(piIndexBuffer);
    piDevice->SetFVF(VERTEX_FVF);

    if (bSolid)
    {
        piDevice->SetRenderState(D3DRS_LIGHTING, true);
        piDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
        piDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, nVertexCount, 0, nTriangleCount);
    }

    if (bWireframe)
    {
        piDevice->SetRenderState(D3DRS_LIGHTING, false);
        piDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
        piDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, nVertexCount, 0, nTriangleCount);
    }

    bResult = true;
Exit0:
    KG_COM_RELEASE(piIndexBuffer);
    KG_COM_RELEASE(piVertexBuffer);
    return bResult;
}

KTerrainRegion* KScene::GetTerrainRegion(int nX, int nZ)
{
    KTerrainRegion* pRegion = NULL;
    int             nIndex  = m_nRegionCountX * nZ + nX;

    KG_PROCESS_ERROR(nX >= 0 && nX < m_nRegionCountX);
    KG_PROCESS_ERROR(nZ >= 0 && nZ < m_nRegionCountZ);

    pRegion = &m_pRegionTable[nIndex];    
Exit0:
    return pRegion;
}

KTerrainRegion* KScene::GetTerrainRegion(float fX, float fZ)
{
    int             nX      = (int)(fX / TERRAIN_REGION_SIZE);
    int             nZ      = (int)(fZ / TERRAIN_REGION_SIZE);
    
    return GetTerrainRegion(nX, nZ);
}

void KScene::GetMaxCoordinate(D3DXVECTOR3* pMax)
{
    pMax->x = (float)TERRAIN_REGION_SIZE * m_nRegionCountX - 1.0f;
    pMax->y = FLT_MAX;
    pMax->z = (float)TERRAIN_REGION_SIZE * m_nRegionCountZ - 1.0f;
}

BOOL KScene::GetHeight(D3DXVECTOR3& vPos, float* pfHeight)
{
    BOOL            bResult     = false;
    BOOL            bRetCode    = false;
    BOOL            bIntersect  = false;
    KTerrainRegion* pRegion     = NULL;
    float           fHeight     = 0.0f;
    D3DXVECTOR3     vTop(vPos.x, vPos.y + 30.0f, vPos.z);
    D3DXVECTOR3     vDown(vPos.x, vPos.y - 30.0f, vPos.z);
    D3DXVECTOR3     Out;

    pRegion = GetTerrainRegion(vPos.x, vPos.z);
    KG_PROCESS_ERROR(pRegion);

    for (int i = 0; i < pRegion->nTriangleCount; i++)
    {
        KTriangleEx* pTriangle = &pRegion->pTriangle[i];

        if (!pTriangle->pVertexA || !pTriangle->pVertexB || !pTriangle->pVertexC)
            continue;

        bRetCode = IntersectSectionLine2Triangle(
            pTriangle->pVertexA->vPos,
            pTriangle->pVertexB->vPos,
            pTriangle->pVertexC->vPos,
            vTop, vDown,
            &Out
        );
        if (bRetCode)
        {
            // 地表三角形只可能有一个与垂直线相交
            fHeight = Out.y;
            bIntersect = true;
            break;
        }
    }

    for (KEntityList::iterator it = pRegion->EntityList.begin(); it != pRegion->EntityList.end(); ++it)
    {
        KEntity* pEntity = *it;

        for (int i = 0; i < pEntity->nTriangleCount; i++)
        {
            KTriangleEx* pTriangle = &pEntity->pTriangle[i];

            if (!pTriangle->pVertexA || !pTriangle->pVertexB || !pTriangle->pVertexC)
                continue;

            bRetCode = IntersectSectionLine2Triangle(
                pTriangle->pVertexA->vPos, 
                pTriangle->pVertexB->vPos, 
                pTriangle->pVertexC->vPos, 
                vTop, vDown,
                &Out
            );
            if (bRetCode && Out.y > fHeight)
            {
                fHeight = Out.y;
                bIntersect = true;
            }
        }
    }

    KG_PROCESS_ERROR(bIntersect);

    *pfHeight = fHeight;

    bResult = true;
Exit0:
    return bResult;
}

BOOL KScene::ComputeEdgeCollapseCostsAtVertex(KVertexEx* pVertex, int nVertexCount, KCostMap* pCostMap)
{
    BOOL bResult    = false;
    BOOL bRetCode   = false;

    KG_PROCESS_ERROR(pVertex);
    KG_PROCESS_ERROR(nVertexCount > 0);
    KG_PROCESS_ERROR(pCostMap);

    for (int i = 0; i < nVertexCount; i++)
    {
        KVertexEx* pVertexU = pVertex + i;

        if (pVertexU->Neighbors.size() == 0)
        {
            pVertexU->fCandiCost = - 0.01f;
            pVertexU->pCandiCollapse = NULL;
            continue;
        }

        std::pair<KCostMap::iterator, KCostMap::iterator> itPair;

        itPair = pCostMap->equal_range(pVertexU->fCandiCost);
        for (KCostMap::iterator it = itPair.first; it != itPair.second; ++it)
        {
            if (it->second != pVertexU)
                continue;

            pCostMap->erase(it);
            break;
        }

        pVertexU->fCandiCost = FLT_MAX;
        pVertexU->pCandiCollapse = NULL;

        for (std::set<KVertexEx*>::iterator it = pVertexU->Neighbors.begin(), itEnd = pVertexU->Neighbors.end(); it != itEnd; ++it)
        {
            KVertexEx* pVertexV = *it;

            bRetCode = ComputeEdgeCollapseCosts(pVertexU, pVertexV);
            KG_PROCESS_ERROR(bRetCode);
        }

        pCostMap->insert(std::make_pair(pVertexU->fCandiCost, pVertexU));
    }

    bResult = true;
Exit0:
    return bResult;
}

KVertexEx* KScene::GetMinCostEdge(KCostMap* pCostMap)
{
    KVertexEx*  pResult     = NULL;
    float       fMinCost    = FLT_MAX;

    KG_PROCESS_ERROR(pCostMap);

    for (KCostMap::iterator it = pCostMap->begin(), itEnd = pCostMap->end(); it != itEnd; NULL)
    {
        KVertexEx* pV = it->second;

        if (!pV->pCandiCollapse)
        {
            pCostMap->erase(it++);
            continue;
        }

        if (pV->bFix && !pV->pCandiCollapse->bFix)
        {
            pCostMap->erase(it++);
            continue;
        }

        if (pV->bFix && pV->pCandiCollapse->bFix)
        {
            BOOL        bCollapse = false;
            D3DXVECTOR3 UV = pV->pCandiCollapse->vPos - pV->vPos;

            for (std::set<KVertexEx*>::iterator itX = pV->Neighbors.begin(), itXEnd = pV->Neighbors.end(); itX != itXEnd; ++itX)
            {
                float       fLen    = 0.0f;
                KVertexEx*  pX      = *itX;
                D3DXVECTOR3 UX      = pX->vPos - pV->vPos;
                D3DXVECTOR3 Cross;

                if (pX == pV->pCandiCollapse)
                    continue;

                D3DXVec3Cross(&Cross, &UX, &UV);
                fLen = D3DXVec3Length(&Cross);
                if (fLen < 0.0000001f)
                {
                    bCollapse = true;
                    break;
                }
            }

            if (!bCollapse)
            {
                pCostMap->erase(it++);
                continue;
            }
        }

        pResult = it->second;
        pCostMap->erase(it++);
        break;
    }

Exit0:
    return pResult;
}

BOOL KScene::CheckCanCollapse(KVertexEx* pVertexU)
{
    BOOL        bResult     = false;
    KVertexEx*  pVertexV    = NULL;
    std::set<KTriangleEx*>      VFaces;
    std::vector<KTriangleEx>    TempFace;

    KG_PROCESS_ERROR(pVertexU);

    pVertexV = pVertexU->pCandiCollapse;
    KG_PROCESS_ERROR(pVertexV);

    VFaces = pVertexV->Faces;

    for (std::set<KTriangleEx*>::iterator it = pVertexU->Faces.begin(), itEnd = pVertexU->Faces.end(); it != itEnd; ++it)
    {
        KTriangleEx*    pTri = *it;
        D3DXVECTOR3     vB2A;
        D3DXVECTOR3     vC2B;
        D3DXVECTOR3     vNormal;

        if (pTri->pVertexA == pVertexV || pTri->pVertexB == pVertexV || pTri->pVertexC == pVertexV)
        {
            VFaces.erase(pTri);
            continue;
        }

        if (pTri->pVertexA == pVertexU)
        {
            KTriangleEx* pNewTri = NULL;

            TempFace.push_back(*pTri);
            pNewTri = &TempFace.back();
            pNewTri->pVertexA = pVertexV;
            pNewTri->UpdateNormal();
        }

        if (pTri->pVertexB == pVertexU)
        {
            KTriangleEx* pNewTri = NULL;

            TempFace.push_back(*pTri);
            pNewTri = &TempFace.back();
            pNewTri->pVertexB = pVertexV;
            pNewTri->UpdateNormal();
        }

        if (pTri->pVertexC == pVertexU)
        {
            KTriangleEx* pNewTri = NULL;

            TempFace.push_back(*pTri);
            pNewTri = &TempFace.back();
            pNewTri->pVertexC = pVertexV;
            pNewTri->UpdateNormal();
        }
    }

    for (std::set<KTriangleEx*>::iterator it = VFaces.begin(), itEnd = VFaces.end(); it != itEnd; ++it)
    {
        TempFace.push_back(**it);
    }

    for (std::vector<KTriangleEx>::iterator itA = TempFace.begin(), itAEnd =TempFace.end(); itA != itAEnd; ++itA)
    {
        for (std::vector<KTriangleEx>::iterator itB = TempFace.begin(), itBEnd =TempFace.end(); itB != itBEnd; ++itB)
        {
            float           fCur    = 0.0f;
            KTriangleEx&    TriA    = *itA;
            KTriangleEx&    TriB    = *itB;

            fCur = D3DXVec3Dot(&(TriA.vNormal), &(TriB.vNormal));
            fCur = 1.0f - fCur;

            KG_PROCESS_ERROR(fCur < 1.1f);
        }
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KScene::Collapse(KVertexEx* pVertexU, KCostMap* pCostMap)
{
    BOOL        bResult     = false;
    KVertexEx*  pVertexV    = NULL;
    std::vector<KVertexEx*> Temp;

    KG_PROCESS_ERROR(pVertexU);
    KG_PROCESS_ERROR(pCostMap);

    pVertexV = pVertexU->pCandiCollapse;
    KG_PROCESS_ERROR(pVertexV);

    Temp.reserve(8);

    for (std::set<KTriangleEx*>::iterator it = pVertexU->Faces.begin(), itEnd = pVertexU->Faces.end(); it != itEnd; ++it)
    {
        KTriangleEx*    pTri = *it;
        D3DXVECTOR3     vB2A;
        D3DXVECTOR3     vC2B;
        D3DXVECTOR3     vNormal;

        if (pTri->pVertexA == pVertexV || pTri->pVertexB == pVertexV || pTri->pVertexC == pVertexV)
        {
            if (pTri->pVertexA != pVertexU)
                pTri->pVertexA->Faces.erase(pTri);

            if (pTri->pVertexB != pVertexU)
                pTri->pVertexB->Faces.erase(pTri);

            if (pTri->pVertexC != pVertexU)
                pTri->pVertexC->Faces.erase(pTri);

            pTri->pVertexA = NULL;
            pTri->pVertexB = NULL;
            pTri->pVertexC = NULL;

            pTri->vNormal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

            continue;
        }
        
        if (pTri->pVertexA == pVertexU)
        {
            pVertexV->Faces.insert(pTri);
            pVertexV->Neighbors.insert(pTri->pVertexB);
            pVertexV->Neighbors.insert(pTri->pVertexC);

            pTri->pVertexB->Neighbors.insert(pVertexV);
            pTri->pVertexC->Neighbors.insert(pVertexV);

            pTri->pVertexA = pVertexV;
        }

        if (pTri->pVertexB == pVertexU)
        {
            pVertexV->Faces.insert(pTri);
            pVertexV->Neighbors.insert(pTri->pVertexA);
            pVertexV->Neighbors.insert(pTri->pVertexC);

            pTri->pVertexA->Neighbors.insert(pVertexV);
            pTri->pVertexC->Neighbors.insert(pVertexV);

            pTri->pVertexB = pVertexV;
        }

        if (pTri->pVertexC == pVertexU)
        {
            pVertexV->Faces.insert(pTri);
            pVertexV->Neighbors.insert(pTri->pVertexA);
            pVertexV->Neighbors.insert(pTri->pVertexB);

            pTri->pVertexA->Neighbors.insert(pVertexV);
            pTri->pVertexB->Neighbors.insert(pVertexV);

            pTri->pVertexC = pVertexV;
        }

        pTri->UpdateNormal();
    }

    for (std::set<KVertexEx*>::iterator it = pVertexU->Neighbors.begin(), itEnd = pVertexU->Neighbors.end(); it != itEnd; ++it)
    {
        KVertexEx* pV = *it;

        assert(pV != pVertexU);

        pV->Neighbors.erase(pVertexU);

        Temp.push_back(pV);
    }

    pVertexU->vPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
    pVertexU->fCandiCost = FLT_MAX;
    pVertexU->pCandiCollapse = NULL;
    pVertexU->Neighbors.clear();
    pVertexU->Faces.clear();

    pVertexV->Neighbors.erase(pVertexU);

    for (int i = 0; i < (int)Temp.size(); i++)
    {
        ComputeEdgeCollapseCostsAtVertex(Temp[i], 1, pCostMap);
    }

    bResult = true;
Exit0:
    return bResult;
}

void KScene::PR()
{
    BOOL    bRetCode    = false;
    DWORD   dwOldTick   = 0;
    DWORD   dwNewTick   = 0;
    DWORD   dwLast      = 0;
    int     nCount      = 0;

    //cprintf("PR begin %lu vertex.\n", m_pRegionAll->CostMap.size());

    for (KEntityList::iterator it = m_EntityList.begin(), itEnd = m_EntityList.end(); it != itEnd; ++it)
    {
        KEntity* pEntity = *it;

        //if (!pEntity->bDraw)
            //continue;

        while (m_bPR)
        {
            DWORD       dwNow       = GetTickCount();
            KVertexEx*  pVertexEx   = GetMinCostEdge(&pEntity->CostMap);

            if (!pVertexEx)
                break;

            if (pVertexEx->fCandiCost > 0.1f)
                break;

            bRetCode = CheckCanCollapse(pVertexEx);
            if (!bRetCode)
                continue;

            Collapse(pVertexEx, &pEntity->CostMap);

            if (dwNow - dwLast > 40)
            {
                g_App.ProcessMessage();
                g_App.FrameUpdate();
                dwLast = dwNow;
            }
        }
    }

    nCount = 0;
    for (int i = 0; i < m_pRegionAll->nTriangleCount; i++)
    {
        if (!m_pRegionAll->pTriangle[i].pVertexA)
            continue;

        if (!m_pRegionAll->pTriangle[i].pVertexB)
            continue;

        if (!m_pRegionAll->pTriangle[i].pVertexC)
            continue;

        nCount++;
    }
    cprintf("PR begin %d tri.\n", nCount);

    dwOldTick = GetTickCount();

    while (m_bPR)
    {
        DWORD       dwNow       = GetTickCount();
        KVertexEx*  pVertexEx   = GetMinCostEdge(&m_pRegionAll->CostMap);

        if (!pVertexEx)
            break;

        if (pVertexEx->fCandiCost > 0.1f)
            break;

        bRetCode = CheckCanCollapse(pVertexEx);
        if (!bRetCode)
            continue;

        Collapse(pVertexEx, &m_pRegionAll->CostMap);

        if (dwNow - dwLast > 40)
        {
            g_App.ProcessMessage();
            g_App.FrameUpdate();
            dwLast = dwNow;
        }
    }

    for (int i = 0; i < m_pRegionAll->nTriangleCount; i++)
    {
        KTriangleEx* pTri = m_pRegionAll->pTriangle + i;

        if (!pTri->pVertexA)
            continue;

        if (!pTri->pVertexB)
            continue;

        if (!pTri->pVertexC)
            continue;

        if (pTri->vNormal.y < 0.70)
        {
            pTri->pVertexA = NULL;
            pTri->pVertexB = NULL;
            pTri->pVertexC = NULL;
        }
    }

//     KTerrainRegion* pTerr = m_pRegionTable;
// 
//     for (int i = 0; i < m_nRegionCountX; i++)
//     {
//         for(int j = 0; j < m_nRegionCountZ; j++)
//         {
//             while (true)
//             {
//                 KVertexEx*  pVertexEx   = GetMinCostEdge(pTerr->pVertex, pTerr->nVertexCount);
// 
//                 if (!pVertexEx)
//                     break;
// 
//                 if (pVertexEx->fCandiCost > 0.1f)
//                     break;
// 
//                 Collapse(pVertexEx);
//             }
// 
//             UpdateNormal(pTerr->pVertex, pTerr->nVertexCount, pTerr->pTriangle, pTerr->nTriangleCount);
//             pTerr++;
//         }
//     }

    dwNewTick = GetTickCount();

    cprintf("PR end %lu vertex.\n", m_pRegionAll->CostMap.size());
    cprintf("PR use %lu ms.\n", dwNewTick - dwOldTick);

    nCount = 0;
    for (int i = 0; i < m_pRegionAll->nTriangleCount; i++)
    {
        if (!m_pRegionAll->pTriangle[i].pVertexA)
            continue;

        if (!m_pRegionAll->pTriangle[i].pVertexB)
            continue;

        if (!m_pRegionAll->pTriangle[i].pVertexC)
            continue;

        nCount++;
    }
    cprintf("PR end %d tri.\n", nCount);

    m_bPR = false;
}

BOOL KScene::ComputeEdgeCollapseCosts(KVertexEx *pVertexU, KVertexEx *pVertexV)
{
    BOOL        bResult     = false;
    float       fEdgeLength = 0.0f;
    float       fCurvature  = 0.0f;
    float       fCost       = 0.0f;
    D3DXVECTOR3 vLen;
    std::vector<KTriangleEx*>       UVEdgeTriangles;
    std::set<KVertexEx*>::iterator  itVertex;

    KG_PROCESS_ERROR(pVertexU);
    KG_PROCESS_ERROR(pVertexV);

    itVertex = pVertexU->Neighbors.find(pVertexV);
    assert(itVertex != pVertexU->Neighbors.end());

    itVertex = pVertexV->Neighbors.find(pVertexU);
    assert(itVertex != pVertexV->Neighbors.end());

    vLen = pVertexV->vPos - pVertexU->vPos;
    fEdgeLength = D3DXVec3Length(&vLen);

    for (std::set<KTriangleEx*>::iterator it = pVertexU->Faces.begin(), itEnd = pVertexU->Faces.end(); it != itEnd; ++it)
    {
        KTriangleEx* pTri = *it;
        std::set<KTriangleEx*>::iterator itFind = pVertexV->Faces.find(pTri);
        if (itFind == pVertexV->Faces.end())
            continue;

        UVEdgeTriangles.push_back(pTri);
    }

    for (std::set<KTriangleEx*>::iterator itU = pVertexU->Faces.begin(), itUEnd = pVertexU->Faces.end(); itU != itUEnd; ++itU)
    {
        float fMinCurv = 1.0f;

        for (std::vector<KTriangleEx*>::iterator itUV = UVEdgeTriangles.begin(), itUVEnd = UVEdgeTriangles.end(); itUV != itUVEnd; ++itUV)
        {
            KTriangleEx*    pTriU   = *itU;
            KTriangleEx*    pTriUV  = *itUV;
            float           fCurv   = D3DXVec3Dot(&pTriU->vNormal, &pTriUV->vNormal);

            if (D3DXVec3Length(&pTriU->vNormal) < 0.0000001f || D3DXVec3Length(&pTriUV->vNormal) < 0.000001f)
                fCurv = 1.0f;

            fCurv = (1.0f - fCurv) / 3.0f;

            fMinCurv = min(fMinCurv, fCurv);
        }
        fCurvature = max(fCurvature, fMinCurv);
    }

    fCost = fEdgeLength * fCurvature;
    if (fCost < pVertexU->fCandiCost)
    {
        pVertexU->fCandiCost = fCost;
        pVertexU->pCandiCollapse = pVertexV;
    }

    bResult = true;
Exit0:
    return bResult;
}
