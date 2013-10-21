#pragma once

#include "KCell.h"
#include <list>

using namespace std;

class KScene;

namespace Terrain
{
class KG3DTerrainRegion;
class KG3DTerrainEx;

class KG3DTerrainRegionLogical
{
private:
	//KCell m_Cells[32*32];//地面的格子
	//list<KCell*>m_listCellOverGround;//空中的格子
public:
	KG3DTerrainRegionLogical(void);
public:
	virtual ~KG3DTerrainRegionLogical(void);

public:
	HRESULT TranslateLogicalDataCell(KScene* pLogicalScene,KCell* pCell,int nX,int nZ);

	HRESULT TranslateLogicalData(KScene* pLogicalScene,KG3DTerrainRegion* pRegion,KG3DTerrainEx* pTerrain,int nXStart,int nZStart);
	HRESULT ComputeLogicalCellsOverGround(D3DXVECTOR2 xz,KCell* pCell,KG3DTerrainRegion* pRegion,KG3DTerrainEx* pTerrain);
	HRESULT Render(KG3DTerrainRegion* pRegion);
	HRESULT ComputeLogicalCells(KG3DTerrainRegion* pRegion,KG3DTerrainEx* pTerrain);
	HRESULT ClearCells();
};

};