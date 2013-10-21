#ifndef  _KTERRAINDATA_H_
#define _KTERRAINDATA_H_

#include "KCell.h"

#define MAX_PARTITION_COUNT  (1 << MAX_PARTITION_INDEX_BIT)

struct KTerrainRegion
{
    KCell Cells[REGION_GRID_WIDTH * REGION_GRID_HEIGHT];
    DWORD dwScriptList[SCRIPT_COUNT_PER_REGION];
};

#pragma pack(1)
struct KPartition
{
    DWORD		dwScriptID;
    DWORD		dwForbiddenRide     : 1;          // ½ûÖ¹ÆïÂí
    DWORD       dwForbiddenPass     : 1;          // ½ûÖ¹Í¨ÐÐ
    DWORD       dwForbiddenPK       : 1;          // ½ûÖ¹PK
    DWORD       dwRevivePosition    : 3;          // ¸´»îµãË÷ÒýºÅ
};
#pragma pack()


class KTerrainScene
{
public:
    KTerrainScene();

    int AddRef();
    int Release();

    KCell* GetRegionTerrainData(int nRegionX, int nRegionY);

    DWORD* GetRegionScriptData(int nRegionX, int nRegionY);

    KPartition* GetPartitionTable() { return m_PartitionTable; };

public:
    BOOL Create(int nRegionCountX, int nRegionCountY);

    BOOL m_bLoadFlag;

private:
    void Clean();

private:

    int                          m_nRefCount;
    KTerrainRegion*	             m_RegionList[MAX_REGION_WIDTH][MAX_REGION_HEIGHT];
    int                          m_nRegionCountX;
    int                          m_nRegionCountY;
    KPartition                   m_PartitionTable[MAX_PARTITION_COUNT];
};

#endif

