#define _WIN32_WINNT 0x0500 
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <Windows.h>
#include <Windowsx.h>
#include <zmouse.h>
#include <WinUser.h>
#include "KApplication.h"
#include "KScene.h"
#include "XScene.h"
#include "MathTools.h"

XScene g_XS;

XScene::XScene()
{
    m_fRegionSize  = DEFAULT_REGION_SIZE;
    m_pViewRegion  = NULL;
    m_VertexTable.reserve(200000);
    m_TriangleTable.reserve(200000);
}

void XScene::SetRegionSize(float fSize)
{
    m_fRegionSize = fSize;
}

BOOL XScene::LoadMeshData(
    const KVertex* pVertex, int nVertexCount,
    const KTriangle* pTriangle, int nTriangleCount
)
{
    BOOL bResult            = false;
    int  nBaseVertexIndex   = (int)m_VertexTable.size();
    int  nBaseTriangleIndex = (int)m_TriangleTable.size();

    KG_PROCESS_ERROR(nVertexCount >= 0);
    KG_PROCESS_ERROR(nTriangleCount >= 0);

    for (int i = 0; i < nTriangleCount; i++)
    {
        KG_PROCESS_ERROR(pTriangle[i].nA >= 0 && pTriangle[i].nA < nVertexCount);
        KG_PROCESS_ERROR(pTriangle[i].nB >= 0 && pTriangle[i].nB < nVertexCount);
        KG_PROCESS_ERROR(pTriangle[i].nC >= 0 && pTriangle[i].nC < nVertexCount);
    }

    m_VertexTable.resize(nBaseVertexIndex + nVertexCount);
    memcpy(&m_VertexTable[nBaseVertexIndex], pVertex, sizeof(KVertex) * nVertexCount);

    m_TriangleTable.resize(nBaseTriangleIndex + nTriangleCount);

    for (int i = 0; i < nTriangleCount; i++)
    {
        m_TriangleTable[nBaseTriangleIndex + i].nA = nBaseVertexIndex + pTriangle[i].nA;   
        m_TriangleTable[nBaseTriangleIndex + i].nB = nBaseVertexIndex + pTriangle[i].nB;   
        m_TriangleTable[nBaseTriangleIndex + i].nC = nBaseVertexIndex + pTriangle[i].nC;
    }

    bResult = true;
Exit0:
    return bResult;
}

void XScene::BuildRegionTriangle()
{
    int             nVertexCount   = (int)m_VertexTable.size();
    int             nTriangleCount = (int)m_TriangleTable.size();
    KVertex*        pVertex        = NULL;
    KTriangle*      pTriangle      = NULL;
    D3DXVECTOR3     vAdjust(0.0f, 0.0f, 0.0f);
    D3DXVECTOR3     vMin(FLT_MAX, FLT_MAX, FLT_MAX);
    D3DXVECTOR3     vMax(FLT_MIN, FLT_MIN, FLT_MIN);

    // 负坐标平移修正
    for (int i = 0; i < nVertexCount; i++)
    {
        pVertex = &m_VertexTable[i];

        vMin.x = min(vMin.x, pVertex->vPos.x);
        vMin.y = min(vMin.y, pVertex->vPos.y);
        vMin.z = min(vMin.z, pVertex->vPos.z);

        vMax.x = max(vMax.x, pVertex->vPos.x);
        vMax.y = max(vMax.y, pVertex->vPos.y);
        vMax.z = max(vMax.z, pVertex->vPos.z);
    }

    if (vMin.x < 0)
    {
        vAdjust.x = vMin.x;
    }

    if (vMin.y < 0)
    {
        vAdjust.y = vMin.y;
    }

    if (vMin.z < 0)
    {
        vAdjust.z = vMin.z;
    }

    for (int i = 0; i < nVertexCount; i++)
    {
        m_VertexTable[i].vPos -= vAdjust;
    }

    vMax -= vMin;

    m_nRegionCountX = (int)(vMax.x / m_fRegionSize) + 1;
    m_nRegionCountY = (int)(vMax.y / m_fRegionSize) + 1;
    m_nRegionCountZ = (int)(vMax.z / m_fRegionSize) + 1;

    m_RegionTable.resize(m_nRegionCountX * m_nRegionCountZ * m_nRegionCountY, NULL);

    for (int i = 0; i < nTriangleCount; i++)
    {
        SetTriangleToRegions(i);    
    }

    m_vMaxCoordinate = vMax;
}

KRegion* XScene::GetRegion(int nX, int nY, int nZ, BOOL bAutoCreate)
{
    KRegion*    pResult = NULL;
    KRegion*    pRegion = NULL;
    int         nIndex  = 0;

    KG_PROCESS_ERROR(nX >= 0 && nX < m_nRegionCountX);
    KG_PROCESS_ERROR(nY >= 0 && nY < m_nRegionCountY);
    KG_PROCESS_ERROR(nZ >= 0 && nZ < m_nRegionCountZ);

    nIndex = m_nRegionCountX * m_nRegionCountZ * nY + m_nRegionCountX * nZ + nX;

    pRegion = m_RegionTable[nIndex];
    if (pRegion == NULL && bAutoCreate)
    {
        pRegion = new KRegion;
        KG_PROCESS_ERROR(pRegion);
        
        pRegion->FaceTable.reserve(256);
        m_RegionTable[nIndex] = pRegion;
    }

    pResult = pRegion;
Exit0:
    return pResult;
}

KRegion* XScene::GetRegion(float fX, float fY, float fZ, BOOL bAutoCreate)
{
    int nX = (int)(fX / m_fRegionSize);
    int nY = (int)(fY / m_fRegionSize);
    int nZ = (int)(fZ / m_fRegionSize);

    return GetRegion(nX, nY, nZ, bAutoCreate);
}

int XScene::UpdateViewRegions(float fX, float fY, float fZ)
{
    int             nViewTriangleCount  = 0;
    int             nViewX              = (int)(fX / m_fRegionSize);
    int             nViewY              = (int)(fY / m_fRegionSize);
    int             nViewZ              = (int)(fZ / m_fRegionSize);
    KRegion*        pRegion             = NULL;
    int             nTriangleCount      = 0;

    KG_PROCESS_ERROR(nViewX >= 0 && nViewX < m_nRegionCountX);
    KG_PROCESS_ERROR(nViewY >= 0 && nViewY < m_nRegionCountY);
    KG_PROCESS_ERROR(nViewZ >= 0 && nViewZ < m_nRegionCountZ);

    pRegion = GetRegion(nViewX, nViewY, nViewZ);
    KG_PROCESS_ERROR(pRegion != m_pViewRegion);

    m_pViewRegion = pRegion;
    m_ViewVertexTable.clear();

    KG_PROCESS_ERROR(pRegion);

    nTriangleCount = (int)pRegion->FaceTable.size();
    m_ViewVertexTable.resize(3 * nTriangleCount);

    for (int i = 0; i < nTriangleCount; i++)
    {
        int        nTriangleIndex = pRegion->FaceTable[i];
        KTriangle* pTriangle      = &m_TriangleTable[nTriangleIndex];

        m_ViewVertexTable[3 * i + 0] = m_VertexTable[pTriangle->nA];
        m_ViewVertexTable[3 * i + 1] = m_VertexTable[pTriangle->nB];
        m_ViewVertexTable[3 * i + 2] = m_VertexTable[pTriangle->nC];
    }

Exit0:
    if (m_pViewRegion)
    {
        nViewTriangleCount = (int)m_pViewRegion->FaceTable.size();
    }
    return nViewTriangleCount;
}

BOOL XScene::GetViewVertexData(void* pvBuffer, size_t uBufferSize)
{
    BOOL bResult        = false;
    int  nVertexCount   = (int)m_ViewVertexTable.size();

    KG_PROCESS_ERROR(uBufferSize >= sizeof(KVertex) * nVertexCount);

    memcpy(pvBuffer, &m_ViewVertexTable[0], sizeof(KVertex) * nVertexCount);

    bResult = true;
Exit0:
    return bResult;
}

BOOL XScene::Draw(IDirect3DDevice9* piDevice)
{
    BOOL                    bResult         = false;
    HRESULT                 hRetCode        = E_FAIL;
    IDirect3DVertexBuffer9* piVertexBuffer  = NULL;
    void*                   pvBuffer        = NULL;
    D3DXMATRIX              Matrix;

    hRetCode = piDevice->CreateVertexBuffer(
        (UINT)(sizeof(KVertex) * m_ViewVertexTable.size()), D3DUSAGE_DYNAMIC, 
        VERTEX_FVF, D3DPOOL_DEFAULT, &piVertexBuffer, 0
    );
    KG_PROCESS_ERROR(!FAILED(hRetCode));

    hRetCode = piVertexBuffer->Lock(0, 0, &pvBuffer, D3DLOCK_DISCARD);
    KG_PROCESS_ERROR(!FAILED(hRetCode));

    memcpy(pvBuffer, &m_ViewVertexTable[0], sizeof(KVertex) * m_ViewVertexTable.size());
    piVertexBuffer->Unlock();

    piDevice->SetStreamSource(0, piVertexBuffer, 0, sizeof(KVertex));
    piDevice->SetFVF(VERTEX_FVF);
    piDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, (UINT)m_ViewVertexTable.size() / 3);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piVertexBuffer);
    return bResult;
}

BOOL XScene::GetHeight(D3DXVECTOR3& vPos, float* pfOut)
{
    BOOL        bResult     = false;
    BOOL        bRetCode    = false;
    KRegion*    pRegion     = NULL;
    float       fHeight     = FLT_MAX;
    int         nFaceCount  = 0;
    D3DXVECTOR3 vOut;
    D3DXVECTOR3 vFloor(vPos.x, 0.0f, vPos.z);

    pRegion = GetRegion((int)(vPos.x / m_fRegionSize), (int)(vPos.y / m_fRegionSize), (int)(vPos.z / m_fRegionSize));
    KG_PROCESS_ERROR(pRegion);

    nFaceCount = (int)pRegion->FaceTable.size();
    for (int i = 0; i < nFaceCount; i++)
    {
        int        nTriangle = pRegion->FaceTable[i];
        KTriangle* pTriangle = &m_TriangleTable[nTriangle];

        bRetCode = IntersectSectionLine2Triangle(
            m_VertexTable[pTriangle->nA].vPos, 
            m_VertexTable[pTriangle->nB].vPos, 
            m_VertexTable[pTriangle->nC].vPos,
            vPos, vFloor,
            &vOut
        );
        if (bRetCode && vOut.y < fHeight)
        {
            fHeight = vOut.y;
        }
    }

    KG_PROCESS_ERROR(fHeight < FLT_MAX);

    *pfOut = fHeight;

    bResult = true;
Exit0:
    return bResult;
}

BOOL XScene::GetMaxHeight(float fX, float fZ, float* pfHeight)
{
    BOOL        bResult     = false;
    BOOL        bRetCode    = false;
    KRegion*    pRegion     = NULL;
    float       fHeight     = FLT_MAX;
    int         nFaceCount  = 0;
    D3DXVECTOR3 vOut;
    D3DXVECTOR3 vPos(fX, m_vMaxCoordinate.y, fZ);
    D3DXVECTOR3 vFloor(fX, 0.0f, fZ);

    for (float fY = m_vMaxCoordinate.y; fY > 0.0f ; fY -= m_fRegionSize)
    {
        pRegion = GetRegion(fX, fY, fZ);
        if (pRegion)
        {
            break;
        }
    }
    KG_PROCESS_ERROR(pRegion);

    nFaceCount = (int)pRegion->FaceTable.size();
    for (int i = 0; i < nFaceCount; i++)
    {
        int        nTriangle = pRegion->FaceTable[i];
        KTriangle* pTriangle = &m_TriangleTable[nTriangle];

        bRetCode = IntersectSectionLine2Triangle(
            m_VertexTable[pTriangle->nA].vPos, 
            m_VertexTable[pTriangle->nB].vPos, 
            m_VertexTable[pTriangle->nC].vPos,
            vPos, vFloor,
            &vOut
        );
        if (bRetCode && vOut.y < fHeight)
        {
            fHeight = vOut.y;
        }
    }

    KG_PROCESS_ERROR(fHeight < FLT_MAX);

    *pfHeight = fHeight;

    bResult = true;
Exit0:
    return bResult;
}

void XScene::SetTriangleToRegions(int nTriangle)
{
    BOOL            bRetCode    = false;
    KTriangle*      pTriangle   = &m_TriangleTable[nTriangle];
    float           fTriangle[3][3];
    D3DXVECTOR3     vMin;
    D3DXVECTOR3     vMax;

    fTriangle[0][0] = m_VertexTable[pTriangle->nA].vPos.x;
    fTriangle[0][1] = m_VertexTable[pTriangle->nA].vPos.y;
    fTriangle[0][2] = m_VertexTable[pTriangle->nA].vPos.z;

    fTriangle[1][0] = m_VertexTable[pTriangle->nB].vPos.x;
    fTriangle[1][1] = m_VertexTable[pTriangle->nB].vPos.y;
    fTriangle[1][2] = m_VertexTable[pTriangle->nB].vPos.z;

    fTriangle[2][0] = m_VertexTable[pTriangle->nC].vPos.x;
    fTriangle[2][1] = m_VertexTable[pTriangle->nC].vPos.y;
    fTriangle[2][2] = m_VertexTable[pTriangle->nC].vPos.z;

    vMin.x = min(min(m_VertexTable[pTriangle->nA].vPos.x, m_VertexTable[pTriangle->nB].vPos.x), m_VertexTable[pTriangle->nC].vPos.x);
    vMin.y = min(min(m_VertexTable[pTriangle->nA].vPos.y, m_VertexTable[pTriangle->nB].vPos.y), m_VertexTable[pTriangle->nC].vPos.y);
    vMin.z = min(min(m_VertexTable[pTriangle->nA].vPos.z, m_VertexTable[pTriangle->nB].vPos.z), m_VertexTable[pTriangle->nC].vPos.z);

    vMax.x = max(max(m_VertexTable[pTriangle->nA].vPos.x, m_VertexTable[pTriangle->nB].vPos.x), m_VertexTable[pTriangle->nC].vPos.x);
    vMax.y = max(max(m_VertexTable[pTriangle->nA].vPos.y, m_VertexTable[pTriangle->nB].vPos.y), m_VertexTable[pTriangle->nC].vPos.y);
    vMax.z = max(max(m_VertexTable[pTriangle->nA].vPos.z, m_VertexTable[pTriangle->nB].vPos.z), m_VertexTable[pTriangle->nC].vPos.z);

    for (int nY = (int)(vMin.y / m_fRegionSize); nY <= (int)(vMax.y / m_fRegionSize); nY++)
    {
        for (int nZ = (int)(vMin.z / m_fRegionSize); nZ <= (int)(vMax.z / m_fRegionSize); nZ++)
        {
            for (int nX = (int)(vMin.x / m_fRegionSize); nX <= (int)(vMax.x / m_fRegionSize); nX++)
            {
                // 遍历每个可能相交的Region
                float fBoxCenter[3] = { m_fRegionSize * nX + m_fRegionSize / 2, m_fRegionSize * nY + m_fRegionSize / 2, m_fRegionSize * nZ + m_fRegionSize / 2 };
                float fBoxHalf[3] = { m_fRegionSize / 2, m_fRegionSize / 2, m_fRegionSize / 2};

                bRetCode = triBoxOverlap(fBoxCenter, fBoxHalf, fTriangle);
                if (!bRetCode)
                    continue;

                SetTriangleTo27Regions(nTriangle, nX, nY, nZ);
            }
        }
    }
}

void XScene::SetTriangleTo27Regions(int nTriangle, int nCenterX, int nCenterY, int nCenterZ)
{
    KRegion*        pRegion     = NULL;

    for (int nY = nCenterY - 1; nY <= nCenterY + 1; nY++)
    {
        for (int nZ = nCenterZ - 1; nZ <= nCenterZ + 1; nZ++)
        {
            for (int nX = nCenterX - 1; nX <= nCenterX + 1; nX++)
            {
                pRegion = GetRegion(nX, nY, nZ, true);
                if (pRegion == NULL)
                    continue;

                int nTableSize = (int)pRegion->FaceTable.size();
                int nIndex     = 0;

                for (nIndex = 0; nIndex < nTableSize; nIndex++)
                {
                    if (pRegion->FaceTable[nIndex] == nTriangle)
                        break;
                }

                // 这个面已经加过了,不用重复加入
                if (nIndex < nTableSize)
                    continue;

                pRegion->FaceTable.push_back(nTriangle);
            }
        }
    }
}

