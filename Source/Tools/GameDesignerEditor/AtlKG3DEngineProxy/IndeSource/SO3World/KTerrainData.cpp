#include "stdafx.h"
#include "KScene.h"
#include "KRegion.h"
#include "KTerrainData.h"
#include "KSO3World.h"
#include "Engine/KMemory.h"

KTerrainScene::KTerrainScene()
{
    m_nRefCount     = 1;
    m_nRegionCountX = 0;
    m_nRegionCountY = 0;
    m_bLoadFlag     = false;
    memset(m_RegionList, 0, sizeof(m_RegionList));
}

int KTerrainScene::AddRef()
{
    return ++m_nRefCount;
}

int KTerrainScene::Release()
{
    assert(m_nRefCount > 0);

    m_nRefCount--;

    if (m_nRefCount == 0)
    {
        Clean();

        KMemory::Delete(this);
        return 0;
    }

    return m_nRefCount;
}

KCell* KTerrainScene::GetRegionTerrainData(int nRegionX, int nRegionY)
{
    KCell*          pResult  = NULL;
    KTerrainRegion* pRegion  = NULL;

    assert(nRegionX >= 0 && nRegionX < m_nRegionCountX);
    assert(nRegionY >= 0 && nRegionY < m_nRegionCountY);
    
    pRegion = m_RegionList[nRegionX][nRegionY];
    KGLOG_PROCESS_ERROR(pRegion);

    pResult = pRegion->Cells;
Exit0:
    return pResult;
}

DWORD* KTerrainScene::GetRegionScriptData(int nRegionX, int nRegionY)
{
    DWORD*          pResult = NULL;
    KTerrainRegion* pRegion = NULL;

    assert(nRegionX >= 0 && nRegionX < m_nRegionCountX);
    assert(nRegionY >= 0 && nRegionY < m_nRegionCountY);
    
    pRegion = m_RegionList[nRegionX][nRegionY];
    KGLOG_PROCESS_ERROR(pRegion);

    pResult = pRegion->dwScriptList;
Exit0:
    return pResult;
}

BOOL KTerrainScene::Create(int nRegionCountX, int nRegionCountY)
{
    BOOL bResult    = false;
    BOOL bRetCode   = false;

    Clean();

    m_nRegionCountX     = nRegionCountX;
    m_nRegionCountY     = nRegionCountY;

    memset(m_RegionList, 0, sizeof(m_RegionList));

    memset(m_PartitionTable, 0, sizeof(m_PartitionTable));

    for (int nY = 0; nY < nRegionCountY; nY++)
    {
        for (int nX = 0; nX < nRegionCountX; nX++)
        {
            KTerrainRegion* pRegion = KMemory::New<KTerrainRegion>();
            KGLOG_PROCESS_ERROR(pRegion);

            memset(pRegion->Cells, 0, sizeof(pRegion->Cells));
            memset(pRegion->dwScriptList, 0, sizeof(pRegion->dwScriptList));

            m_RegionList[nX][nY] = pRegion;
        }
    }

    m_bLoadFlag = false;

    bResult = true;
Exit0:
    if (!bResult)
    {
        Clean();
    }
    return bResult;
}

void KTerrainScene::Clean()
{
    for (int nY = 0; nY < m_nRegionCountY; nY++)
    {
        for (int nX = 0; nX < m_nRegionCountX; nX++)
        {
            KTerrainRegion* pRegion = m_RegionList[nX][nY];
            if (!pRegion)
                continue;

            for (int i = 0; i < REGION_GRID_WIDTH * REGION_GRID_HEIGHT; i++)
            {
                KCell* pCell = pRegion->Cells[i].m_pNext;

                while (pCell)
                {
                    KCell* pNext = pCell->m_pNext;

                    if (pCell->m_BaseInfo.dwDynamic)
                    {
                        KMemory::RawDelete((KDynamicCell*)pCell);
                    }
                    else
                    {
                        KMemory::RawDelete(pCell);
                    }

                    pCell = pNext;
                }
            }

            KMemory::Delete(pRegion);
            pRegion = NULL;
        }
    }

    memset(m_RegionList, 0, sizeof(m_RegionList));
}
