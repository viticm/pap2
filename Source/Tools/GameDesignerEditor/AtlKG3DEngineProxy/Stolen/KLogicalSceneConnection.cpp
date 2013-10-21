#include "stdafx.h"
#include "../AtlKG3DEngineProxy.h"
#include "IEEditor.h"
#include "KLogicalSceneConnection.h"
#include "KGLog.h"
#include "KSO3World.h"
#include "KPatrolPath.h"
#include "AtlBase.h"
#include "ieother.h"

#include "KGSTLWrappers.h"
#include "KG3DRepresentNPC.h" // fuck


BOOL KLogicalSceneConnection::CheckObstacle(int nXCell, int nYCell, int nLayer)
{
	if(m_lpLogicalScene)
		return m_lpLogicalScene->CheckObstacle(nXCell,nYCell,nLayer);
	else
		return FALSE;
}


KRegion* KLogicalSceneConnection::GetRegion(int nRegionX, int nRegionY)
{
	if(m_lpLogicalScene)
		return m_lpLogicalScene->GetRegion(nRegionX,nRegionY);
	else
		return NULL;
}

HRESULT KLogicalSceneConnection::CreateLogicalRegionInfo()
{
	SAFE_DELETE_ARRAY(m_lpLogicalRegionInfo);

	int nX = m_lpLogicalScene->GetRegionWidth();
	int nY = m_lpLogicalScene->GetRegionHeight();

	m_lpLogicalRegionInfo = new _KG3DLogicalRegionInfo[nX * nY];
	KGLOG_PROCESS_ERROR(m_lpLogicalRegionInfo);

	ZeroMemory(m_lpLogicalRegionInfo,sizeof(_KG3DLogicalRegionInfo) * nX * nY);

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KLogicalSceneConnection::FillLogicalCellsInfo( D3DXVECTOR3 A,int X,int Z,int& nCellCount,KCell* pCell,IELogicalVertex* pLLine, IELogicalVertex* pLFace, BOOL bTerrian,BOOL bItem, BOOL bSelected, int nMaxCount )
{
	if(nCellCount >= nMaxCount)
		return E_FAIL;

	float xx = A.x + X * LOGICAL_CELL_CM_LENGTH;
	float zz = A.z + Z * LOGICAL_CELL_CM_LENGTH;
	float y0 = A.y + pCell->m_wLowLayer * (float)ALTITUDE_UNIT + TERRAIN_MIN_HEIGHT;
	y0 = max(-200,y0);
	float y1 = A.y + pCell->m_wHighLayer * (float)ALTITUDE_UNIT+ TERRAIN_MIN_HEIGHT + 1;
	DWORD c3 = 0xFFFFFFFF;
	DWORD c1 = 0xA0FF00FF;
	DWORD c2 = 0xA0FF0000;

	AABBOX Box;
	Box.AddPosition(D3DXVECTOR3(xx,y0,zz));
	Box.AddPosition(D3DXVECTOR3(xx + LOGICAL_CELL_CM_LENGTH, y1, zz + LOGICAL_CELL_CM_LENGTH));
	D3DXVECTOR3 BoxPos[8];
	Box.Extract(BoxPos);

	if (bTerrian)
	{
		// Line
		int nCellCountBase = nCellCount*8;
		pLLine[nCellCountBase+0].posi = BoxPos[BOX_A2];pLLine[nCellCountBase+1].posi = BoxPos[BOX_A1];pLLine[nCellCountBase+0].color=c1;pLLine[nCellCountBase+1].color=c3;
		pLLine[nCellCountBase+2].posi = BoxPos[BOX_B2];pLLine[nCellCountBase+3].posi = BoxPos[BOX_B1];pLLine[nCellCountBase+2].color=c1;pLLine[nCellCountBase+3].color=c3;
		pLLine[nCellCountBase+4].posi = BoxPos[BOX_C2];pLLine[nCellCountBase+5].posi = BoxPos[BOX_C1];pLLine[nCellCountBase+4].color=c1;pLLine[nCellCountBase+5].color=c3;
		pLLine[nCellCountBase+6].posi = BoxPos[BOX_D2];pLLine[nCellCountBase+7].posi = BoxPos[BOX_D1];pLLine[nCellCountBase+6].color=c1;pLLine[nCellCountBase+7].color=c3;

		int nCellCount12 = nCellCount * 12; 
		// Face
		pLFace[nCellCount12 + 0].posi = BoxPos[BOX_A1];
		pLFace[nCellCount12 + 1].posi = BoxPos[BOX_C1];
		pLFace[nCellCount12 + 2].posi = BoxPos[BOX_B1];

		pLFace[nCellCount12 + 3].posi = BoxPos[BOX_A1];
		pLFace[nCellCount12 + 4].posi = BoxPos[BOX_D1];
		pLFace[nCellCount12 + 5].posi = BoxPos[BOX_C1];

		pLFace[nCellCount12 + 6].posi = BoxPos[BOX_A2];
		pLFace[nCellCount12 + 7].posi = BoxPos[BOX_C2];
		pLFace[nCellCount12 + 8].posi = BoxPos[BOX_B2];

		pLFace[nCellCount12 + 9].posi = BoxPos[BOX_A2];
		pLFace[nCellCount12 + 10].posi = BoxPos[BOX_D2];
		pLFace[nCellCount12 + 11].posi = BoxPos[BOX_C2];

		pLFace[nCellCount12 + 0].color = c1;
		pLFace[nCellCount12 + 1].color = c1;
		pLFace[nCellCount12 + 2].color = c1;
		pLFace[nCellCount12 + 3].color = c1;
		pLFace[nCellCount12 + 4].color = c1;
		pLFace[nCellCount12 + 5].color = c1;
		pLFace[nCellCount12 + 6].color = c2;
		pLFace[nCellCount12 + 7].color = c2;
		pLFace[nCellCount12 + 8].color = c2;
		pLFace[nCellCount12 + 9].color = c2;
		pLFace[nCellCount12 + 10].color = c2;
		pLFace[nCellCount12 + 11].color = c2;

		nCellCount++;
	}
	if (bItem && pCell->m_pNext)
	{
		FillLogicalCellsInfo(
			A, X, Z, nCellCount, pCell->m_pNext, pLLine, pLFace, TRUE, TRUE, FALSE, nMaxCount);
	}
	return S_OK;
}

FLOAT KLogicalSceneConnection::GetLogicalHeight( const D3DXVECTOR3& vLogicalCoord, size_t* pOrderOfCell )
{
	POINT regionIndex = LogicalToLogicalRegionIndex(vLogicalCoord.x, vLogicalCoord.y);
	KRegion* pRegion = this->GetRegion(regionIndex.x, regionIndex.y);
	KG_PROCESS_ERROR(NULL != pRegion);
	{
		INT nCellIndexLocalX = (INT)(vLogicalCoord.x / CELL_LENGTH) % REGION_GRID_WIDTH;
		INT nCellIndexLocalY = (INT)(vLogicalCoord.y / CELL_LENGTH) % REGION_GRID_HEIGHT;
		KCell* pCell = GetCurOrLowerObstacle(*pRegion, nCellIndexLocalX, nCellIndexLocalY, (INT)vLogicalCoord.z, pOrderOfCell);
		KG_PROCESS_ERROR(NULL != pCell);

		return (FLOAT)(pCell->m_wHighLayer * POINT_PER_ALTITUDE);
	}
Exit0:
	return -FLT_MAX;
}
KCell* KLogicalSceneConnection::GetNextCell(const KCell* pCell, BOOL bIgnoreDynamicSwitch)
{
	KCell*  pCellNode     = NULL;

	pCellNode = pCell->m_pNext;

	while (pCellNode && pCellNode->m_BaseInfo.dwDynamic)
	{
		//这里一定忽略动态障碍
		//// 忽略动态障碍时,动态障碍一律视为disable
		//if (!bIgnoreDynamicSwitch)
		//{
		//	KDynamicCell* pDynamicCell   = (KDynamicCell*)pCellNode;
		//	BOOL bDynamicSwitch = m_pScene->GetDynamicObstacleState(pDynamicCell->m_wGroupIndex);
		//	if (bDynamicSwitch)
		//	{
		//		return pCellNode;
		//	}
		//}

		pCellNode = pCellNode->m_pNext;
	}

	return pCellNode;
}

KCell* KLogicalSceneConnection::GetLowestObstacle(const KRegion& regionCur, int nXCell, int nYCell)
{
	assert(nXCell >= 0);
	assert(nXCell < REGION_GRID_WIDTH);
	assert(nYCell >= 0);
	assert(nYCell < REGION_GRID_HEIGHT);

	return regionCur.m_pCells + REGION_GRID_WIDTH * nYCell + nXCell;
}
POINT KLogicalSceneConnection::LogicalToLogicalRegionIndex( FLOAT fLogicalX, FLOAT fLogicalY )
{
	POINT pTemp = 
	{
		(LONG)((fLogicalX / static_cast<FLOAT>(CELL_LENGTH)) / static_cast<FLOAT>(REGION_GRID_WIDTH)),
		(LONG)((fLogicalY / static_cast<FLOAT>(CELL_LENGTH)) / static_cast<FLOAT>(REGION_GRID_HEIGHT)),
	};
	return pTemp;
}
KCell* KLogicalSceneConnection::GetCurOrLowerObstacle( const KRegion& regionCur, int nXCell, int nYCell, int nZ, size_t* pOrderOfCell )
{
	KCell*  pResult     = NULL;
	KCell*  pCellNode   = NULL;
	int     nAltitude   = nZ / POINT_PER_ALTITUDE;
	size_t uOrder = 0;

	// 从最下面的一个Cell开始找,找到最上一个 "上表面<=nZ" 的Cell,也就是链表中最后一个 "上表面<=nZ" 的Cell
	// 如果所有Cell上表面都高于nZ,则返回NULL

	assert(nXCell >= 0);
	assert(nXCell < REGION_GRID_WIDTH);
	assert(nYCell >= 0);
	assert(nYCell < REGION_GRID_HEIGHT);

	pCellNode = GetLowestObstacle(regionCur, nXCell, nYCell);
	pResult		= pCellNode;

	while(NULL != pCellNode)
	{
		if (pCellNode->m_wLowLayer > nAltitude)	//得到下一个底部比当前高的，用上一个就行了
		{
			break;
		}

		pResult = pCellNode;

		if (pCellNode->m_wLowLayer <= nAltitude && nAltitude <= pCellNode->m_wHighLayer)	//如果正好包住，那用当前的
		{
			break;
		}

		pCellNode = GetNextCell(pCellNode, false);
		++uOrder;
	}

	if (NULL != pOrderOfCell)
	{
		*pOrderOfCell = uOrder;
	}

	return pResult;
}

KCell* KLogicalSceneConnection::GetCellOfRegion( KRegion* pRegion, INT nIndex )
{
	_ASSERTE(NULL != pRegion && nIndex < REGION_GRID_WIDTH * REGION_GRID_HEIGHT);
	return &pRegion->m_pCells[nIndex];
}
