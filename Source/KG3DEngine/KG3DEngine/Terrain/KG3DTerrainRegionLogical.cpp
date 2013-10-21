#include "StdAfx.h"
#include "KG3DTerrainRegionLogical.h"
//#include "KScene.h"
#include "KG3DTerrainRegion.h"
#include "../KG3DGraphicsTool.h"
#include "KG3DTerrainEx.h"


#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

namespace Terrain
{

KG3DTerrainRegionLogical::KG3DTerrainRegionLogical(void)
{
	//ZeroMemory(m_Cells,sizeof(KCell)*32*32);

}

KG3DTerrainRegionLogical::~KG3DTerrainRegionLogical(void)
{
	ClearCells();
}

HRESULT KG3DTerrainRegionLogical::ComputeLogicalCellsOverGround(D3DXVECTOR2 xz,KCell* pCell,KG3DTerrainRegion* pRegion,KG3DTerrainEx* pTerrain)
{
	DWORD dwCount;
	D3DXVECTOR3 vInter[128];
	D3DXVECTOR3 vIn(xz.x,0,xz.y);

	pTerrain->GetStandHeight(dwCount,vInter,vIn);

	float fGroundHeihgt = vInter[0].y;

	list<float>listHeight;
	for(DWORD i=0;i<dwCount;i++)
	{
		if(vInter[i].y>fGroundHeihgt)
			listHeight.push_back(vInter[i].y);
	}
	listHeight.sort();
	listHeight.reverse();


	list<KCell*>listCellOverGround;

	list<float>::iterator i = listHeight.begin();
	while(i!=listHeight.end())
	{
		float H = *i;
		float Low = fGroundHeihgt;

		KCell* pNewCell = new KCell;
		ZeroMemory(pNewCell,sizeof(KCell));
		//m_listCellOverGround.push_back(pNewCell);
		//listCellOverGround.push_front(pNewCell);

		pNewCell->m_wHighLayer = (WORD)((H-TERRAIN_MIN_HEIGHT) / 25);
		Low = H - 25.5F;
		i++;
		if(i!=listHeight.end())
		{
			Low = *i;
			i++;
		}

		pNewCell->m_wHighLayer = (WORD)((H-TERRAIN_MIN_HEIGHT) / 25);
		pNewCell->m_wLowLayer = (WORD)((Low-TERRAIN_MIN_HEIGHT) / 25);
	}

	KCell* pThisCell = pCell;
	list<KCell*>::iterator s = listCellOverGround.begin();
	while (s!=listCellOverGround.end())
	{
		KCell* pOverCell = *s;
		pThisCell->m_pNext = pOverCell;
		pThisCell = pThisCell->m_pNext;
		s++;
	}
	return S_OK;
}

HRESULT KG3DTerrainRegionLogical::ComputeLogicalCells(KG3DTerrainRegion* pRegion,KG3DTerrainEx* pTerrain)
{
	//int XX = pRegion->m_nIndex_X - pTerrain->m_RegionManager.m_nRegionStart_X;
	//int ZZ = pRegion->m_nIndex_X - pTerrain->m_RegionManager.m_nRegionStart_Z;

	//for (int z=0;z<32;z++)
	//{
	//	for(int x=0;x<32;x++)
	//	{
	//		int nLogicalX = XX * 32 + x;
	//		int nLogicalZ = ZZ * 32 + z;

	//		KCell cell;
	//		pTerrain->m_lpLogicalScene->GetCellData(nLogicalX,nLogicalZ,cell.m_BaseInfo,cell.m_wLowLayer,cell.m_wHighLayer,cell.m_dwScriptID);
	//		cell.m_wLowLayer = 0;
	//		cell.m_wLowLayer = (WORD)(100-TERRAIN_MIN_HEIGHT)/ALTITUDE_UNIT;
	//		pTerrain->m_lpLogicalScene->SetCellData(nLogicalX,nLogicalZ,cell.m_BaseInfo,cell.m_wLowLayer,cell.m_wHighLayer,cell.m_dwScriptID);
	//		//int nIndex = z * 32 + x;
	//		//m_Cells[nIndex].m_wLowLayer = 0;
	//		//m_Cells[nIndex].m_wHighLayer = 
	//		//if(pRegion->m_lpHeightGround)
	//		//{
	//		//	D3DXVECTOR2 XZ =
	//		//		D3DXVECTOR2(pRegion->m_BBox.A.x,pRegion->m_BBox.A.z) +
	//		//		D3DXVECTOR2(x*50.0F+25,z*50.0F+25);
	//		//	D3DXVECTOR3 vPosition;
	//		//	pRegion->m_lpHeightGround->GetPosition(&vPosition,XZ);
	//		//	m_Cells[nIndex].m_wHighLayer = (WORD)((vPosition.y-TERRAIN_MIN_HEIGHT)/25);

	//		//	//ComputeLogicalCellsOverGround(XZ,&m_Cells[nIndex],pRegion,pTerrain);

	//		//	m_lp
	//		//}
	//	}
	//}
	return S_OK;
}

HRESULT KG3DTerrainRegionLogical::ClearCells()
{
	/*ZeroMemory(m_Cells,sizeof(KCell)*32*32);
	list<KCell*>::iterator i = m_listCellOverGround.begin();
	while(i!=m_listCellOverGround.end())
	{
		KCell* pCell = *i;
		SAFE_DELETE(pCell);
		i++;
	}
	m_listCellOverGround.clear();*/
	return S_OK;
}

HRESULT KG3DTerrainRegionLogical::Render(KG3DTerrainRegion* pRegion)
{
	return S_OK;//g_cGraphicsTool.DrawLogicalCells(m_Cells,pRegion);
}

HRESULT KG3DTerrainRegionLogical::TranslateLogicalDataCell(KScene* pLogicalScene,KCell* pCell,int nX,int nZ)
{
	pLogicalScene->SetCellData(nX,nZ,&pCell->m_BaseInfo,
		pCell->m_wLowLayer,
		pCell->m_wHighLayer,
		0);
	if(pCell->m_pNext)
	{
		return TranslateLogicalDataCell(pLogicalScene,pCell->m_pNext,nX,nZ);
	}
	return S_OK;
}
HRESULT KG3DTerrainRegionLogical::TranslateLogicalData(KScene* pLogicalScene,KG3DTerrainRegion* pRegion,KG3DTerrainEx* pTerrain,
													   int nXStart,int nZStart)
{
	/*for (int z=0;z<32;z++)
	{
		for(int x=0;x<32;x++)
		{
			int nIndex = z * 32 + x;
			KCell* pCell = &m_Cells[nIndex];

			pLogicalScene->SetCellData(nXStart+x,nZStart+z,&pCell->m_BaseInfo,
				pCell->m_wLowLayer,
				pCell->m_wHighLayer,
				0);
			while (pCell->m_pNext)
			{
				pLogicalScene->NewObstacle(nXStart+x,nZStart+z,&pCell->m_BaseInfo,
					pCell->m_wLowLayer,
					pCell->m_wHighLayer,
					0);

				pCell = pCell->m_pNext;
			}
		}
	}*/
	return S_OK;
}

};