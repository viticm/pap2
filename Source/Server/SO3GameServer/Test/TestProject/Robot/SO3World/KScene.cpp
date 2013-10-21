#include "stdafx.h"
#include "KObjectManager.h"
#include "KDoodad.h"
#include "KScene.h"
#include "KRegion.h"
#include "KSO3World.h"
#include "KWorldSettings.h"
#include "KMapFile.h"
#include "KScriptCenter.h"
#include "KMath.h"

KScene::KScene()
{
}

KScene::~KScene()
{
}

BOOL KScene::Init()
{
	m_dwMapID			= 0;
	m_dwMapCopyIndex	= 0;
	m_eSceneState		= ssInvalid;
	m_dwCreatorID		= 0;
	m_szName[0]			= '\0';
	m_szFilePath[0]		= '\0';
	memset(m_RegionList, 0, sizeof(m_RegionList));
	m_nRegionWidth		= 0;
	m_nRegionHeight	    = 0;
	m_dwScriptID		= 0;

	return TRUE;
}

void KScene::UnInit()
{
	BOOL bRetCode = FALSE;
	bRetCode = TRUE;
}

BOOL KScene::DelRegion(int nRegionX, int nRegionY)
{
	BOOL bRetCode = FALSE;

	KGLOG_PROCESS_ERROR(nRegionX < MAX_REGION_WIDTH);
	KGLOG_PROCESS_ERROR(nRegionY < MAX_REGION_HEIGHT);

	KRegion* pRegion = m_RegionList[nRegionX][nRegionY];

	if (pRegion)
	{
		bRetCode = pRegion->DeleteAllCell();
		KGLOG_PROCESS_ERROR(bRetCode);

		m_RegionList[nRegionX][nRegionY] = NULL;

		g_ObjectManager.m_RegionSet.Delete(pRegion);
	}

	return TRUE;

Exit0:

	return FALSE;
}

BOOL KScene::DeleteAllObstacle(void)
{
	BOOL bRetCode = FALSE;

	for (int nWidth = 0; nWidth < m_nRegionWidth; nWidth++)
	{
		for (int nHeight = 0; nHeight < m_nRegionHeight; nHeight++)
		{
			KRegion* pRegion = GetRegion(nWidth, nHeight);
			if (pRegion)
			{
				bRetCode = pRegion->DeleteAllCell();
				KGLOG_CHECK_ERROR(bRetCode);
			}
		}
	}

	return TRUE;
}


BOOL KScene::DeleteAllRegion(void)
{
	BOOL bRetCode = FALSE;

	for (int nWidth = 0; nWidth < m_nRegionWidth; nWidth++)
	{
		for (int nHeight = 0; nHeight < m_nRegionHeight; nHeight++)
		{
			DelRegion(nWidth, nHeight);
		}
	}

	m_nRegionWidth = 0;
	m_nRegionHeight = 0;

	return TRUE;
}

struct cell_ltcmp
{
	bool operator()(KCell* X, KCell* Y) const
	{
		return memcmp(X, Y, sizeof(KCell)) < 0;
	}
};
std::map<KCell*, int, cell_ltcmp>	mapCellList;
std::map<KCell*, int, cell_ltcmp>::iterator it;

BOOL KScene::PreLoadScene()
{
    BOOL bRetCode = FALSE;
    KRegion *pRegion = NULL;

	KGLOG_PROCESS_ERROR(m_nRegionWidth < MAX_REGION_WIDTH);
	KGLOG_PROCESS_ERROR(m_nRegionHeight < MAX_REGION_HEIGHT);

	for (int nY = 0; nY < m_nRegionHeight; nY++)
    {
		for (int nX = 0; nX < m_nRegionWidth; nX++)
        {
            pRegion = g_ObjectManager.m_RegionSet.New();
            KGLOG_PROCESS_ERROR(pRegion);

            bRetCode = pRegion->Init(this, nX, nY);
            KGLOG_PROCESS_ERROR(bRetCode);

            bRetCode = SetRegion(nX, nY, pRegion);
            KGLOG_PROCESS_ERROR(bRetCode);

            bRetCode = pRegion->ClientLoad();
            KGLOG_PROCESS_ERROR(bRetCode);

			// 建立邻接Region的链接关系
			if (nX > 0 && nY > 0)
			{
				m_RegionList[nX - 1][nY - 1]->m_pConnectRegion[UP_RIGHT_DIRECTION] = pRegion;
				pRegion->m_pConnectRegion[DOWN_LEFT_DIRECTION] = m_RegionList[nX - 1][nY - 1];
			}

			if (nY > 0)
			{
				m_RegionList[nX][nY - 1]->m_pConnectRegion[UP_DIRECTION] = pRegion;
				pRegion->m_pConnectRegion[DOWN_DIRECTION] = m_RegionList[nX][nY - 1];
			}

			if (nX < (m_nRegionWidth - 1) && nY > 0)
			{
				m_RegionList[nX + 1][nY - 1]->m_pConnectRegion[UP_LEFT_DIRECTION] = pRegion;
				pRegion->m_pConnectRegion[DOWN_RIGHT_DIRECTION] = m_RegionList[nX + 1][nY - 1];
			}

			if (nX > 0)
			{
				m_RegionList[nX - 1][nY]->m_pConnectRegion[RIGHT_DIRECTION] = pRegion;
				pRegion->m_pConnectRegion[LEFT_DIRECTION] = m_RegionList[nX - 1][nY];
			}

            printf("\r\tMap Loading(%s %d,%d).............%d%%", 
                m_szName, m_dwMapID, m_dwMapCopyIndex,
                ((nY * m_nRegionWidth + (nX + 1)) * 100) / (m_nRegionHeight * m_nRegionWidth)
                );
        }
    }	

    return TRUE;

Exit0:

    return FALSE;
}

BOOL KScene::ClientLoadRegion(int nRegionX, int nRegionY)
{
    int  nResult  = FALSE;
    BOOL bRetCode = FALSE;

    //校验场景的长宽是否超出
    KGLOG_PROCESS_ERROR(m_nRegionWidth < MAX_REGION_WIDTH);
    KGLOG_PROCESS_ERROR(m_nRegionHeight < MAX_REGION_HEIGHT);

    if (g_SO3World.m_nPreLoadScene)
        return TRUE;

    m_Lock.Lock();
    //初始化Region

    for (int nX = (int)nRegionX - 1; nX <= (int)nRegionX + 1; nX++)
    {
        for (int nY = (int)nRegionY - 1; nY <= (int)nRegionY + 1; nY++)
        {
            if (nX >= 0 && nY >= 0 && 
                nX < (int)GetRegionWidth() &&
                nY < (int)GetRegionHeight()
            )
            {                
                KRegion* pRegion = GetRegion(nX, nY);
                if (pRegion)
                    continue;

                pRegion = g_ObjectManager.m_RegionSet.New();
                KGLOG_PROCESS_ERROR(pRegion);

                bRetCode = pRegion->Init(this, nX, nY);
                KGLOG_PROCESS_ERROR(bRetCode);

                bRetCode = SetRegion(nX, nY, pRegion);
                KGLOG_PROCESS_ERROR(bRetCode);

                bRetCode = pRegion->ClientLoad();
                KGLOG_PROCESS_ERROR(bRetCode);
            }
        }
    }

    //设定邻接Region
    for (int nX = (int)nRegionX - 1; nX <= (int)nRegionX + 1; nX++)
    {
        for (int nY = (int)nRegionY - 1; nY <= (int)nRegionY + 1; nY++)
        {
            if (nX >= 0 && nY >= 0 && nX < (int)GetRegionWidth() &&
                nY < (int)GetRegionHeight())
            {
                KRegion* pRegion = GetRegion(nX, nY);
                KGLOG_PROCESS_ERROR(pRegion);

                pRegion->m_pConnectRegion[0] = GetRegion(nX + 1, nY);
                pRegion->m_pConnectRegion[1] = GetRegion(nX + 1, nY + 1);
                pRegion->m_pConnectRegion[2] = GetRegion(nX, nY + 1);
                pRegion->m_pConnectRegion[3] = GetRegion(nX - 1, nY + 1);
                pRegion->m_pConnectRegion[4] = GetRegion(nX - 1, nY);
                pRegion->m_pConnectRegion[5] = GetRegion(nX - 1, nY - 1);
                pRegion->m_pConnectRegion[6] = GetRegion(nX, nY - 1);
                pRegion->m_pConnectRegion[7] = GetRegion(nX + 1, nY - 1);
            }
        }
    }

    nResult = TRUE;
Exit0:
    m_Lock.Unlock();
    return nResult;
}

BOOL KScene::SetCellData(int nXCell, int nYCell, KCell::KBaseInfo* pBaseInfo, 
				 WORD wLowLayer, WORD wHighLayer)
{
	DWORD dwRegionX = nXCell / REGION_GRID_WIDTH;
	DWORD dwRegionY = nYCell / REGION_GRID_HEIGHT;

	KRegion* pRegion = GetRegion(dwRegionX, dwRegionY);
	KGLOG_PROCESS_ERROR(pRegion);

	nXCell = nXCell % REGION_GRID_WIDTH;
	nYCell = nYCell % REGION_GRID_HEIGHT;

	KCell* pCell = &(pRegion->m_Cells[nXCell][nYCell]);

	memcpy(&(pCell->m_BaseInfo), pBaseInfo, sizeof(KCell::KBaseInfo));
	pCell->m_wLowLayer = wLowLayer;
	pCell->m_wHighLayer = wHighLayer;

	return TRUE;

Exit0:

	return FALSE;
}

BOOL KScene::GetCellData(int nXCell, int nYCell, KCell::KBaseInfo& rBaseInfo, 
				 WORD& rwLowLayer, WORD& rwHighLayer)
{
	DWORD dwRegionX = nXCell / REGION_GRID_WIDTH;
	DWORD dwRegionY = nYCell / REGION_GRID_HEIGHT;

	KRegion* pRegion = GetRegion(dwRegionX, dwRegionY);
	KGLOG_PROCESS_ERROR(pRegion);

	nXCell = nXCell % REGION_GRID_WIDTH;
	nYCell = nYCell % REGION_GRID_HEIGHT;

	KCell* pCell = &(pRegion->m_Cells[nXCell][nYCell]);

	memcpy(&rBaseInfo, &(pCell->m_BaseInfo), sizeof(KCell::KBaseInfo));
	rwLowLayer = pCell->m_wLowLayer;
	rwHighLayer = pCell->m_wHighLayer;

	return TRUE;

Exit0:

	return FALSE;
}

BOOL KScene::NewObstacle(
    int nXCell, int nYCell, KCell::KBaseInfo* pBaseInfo, WORD wLowLayer, WORD wHighLayer
)
{
    BOOL        bResult     = false;
	BOOL        bRetCode    = false;
	DWORD       dwRegionX   = nXCell / REGION_GRID_WIDTH;
	DWORD       dwRegionY   = nYCell / REGION_GRID_HEIGHT;
    KRegion*    pRegion     = NULL;
    int         nMaxCellX   = m_nRegionWidth * REGION_GRID_WIDTH;
    int         nMaxCellY   = m_nRegionHeight * REGION_GRID_HEIGHT;

    KG_PROCESS_ERROR(nXCell >= 0 && nXCell < nMaxCellX);
    KG_PROCESS_ERROR(nYCell >= 0 && nYCell < nMaxCellY);

	pRegion = GetRegion(dwRegionX, dwRegionY);
	KGLOG_PROCESS_ERROR(pRegion);

	nXCell = nXCell % REGION_GRID_WIDTH;
	nYCell = nYCell % REGION_GRID_HEIGHT;

	bRetCode = pRegion->AddObstacle(nXCell, nYCell, pBaseInfo, wLowLayer, wHighLayer);
	KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
	return bResult;
}

BOOL KScene::DelObstacle(int nXCell, int nYCell, int nZ)
{
	BOOL bRetCode = FALSE;

	DWORD dwRegionX = nXCell / REGION_GRID_WIDTH;
	DWORD dwRegionY = nYCell / REGION_GRID_HEIGHT;

	KRegion* pRegion = GetRegion(dwRegionX, dwRegionY);
	KGLOG_PROCESS_ERROR(pRegion);

	nXCell = nXCell % REGION_GRID_WIDTH;
	nYCell = nYCell % REGION_GRID_HEIGHT;

	KCell* pCell = pRegion->RemoveObstacle(nXCell, nYCell, nZ);
	KGLOG_PROCESS_ERROR(pCell);

	bRetCode = g_ObjectManager.m_CellSet.Delete(pCell);
	KGLOG_PROCESS_ERROR(bRetCode);

	return TRUE;

Exit0:

	return FALSE;
}

BOOL KScene::GetObstacle(int nXCell, int nYCell, int nZ, KCell::KBaseInfo& rBaseInfo, 
						 WORD& rwLowLayer, WORD& rwHighLayer)
{
	BOOL bRetCode = FALSE;

	DWORD dwRegionX = nXCell / REGION_GRID_WIDTH;
	DWORD dwRegionY = nYCell / REGION_GRID_HEIGHT;

	KRegion* pRegion = GetRegion(dwRegionX, dwRegionY);
	KGLOG_PROCESS_ERROR(pRegion);

	nXCell = nXCell % REGION_GRID_WIDTH;
	nYCell = nYCell % REGION_GRID_HEIGHT;

	KCell* pCell = pRegion->GetObstacle(nXCell, nYCell, nZ);
	KGLOG_PROCESS_ERROR(pCell);

	memcpy(&rBaseInfo, &(pCell->m_BaseInfo), sizeof(KCell::KBaseInfo));
	rwLowLayer = pCell->m_wLowLayer;
	rwHighLayer = pCell->m_wHighLayer;

	return TRUE;

Exit0:

	return FALSE;
}

BOOL KScene::CheckObstacle(int nXCell, int nYCell, int nLayer)
{
	BOOL bRetCode = FALSE;

	DWORD dwRegionX = nXCell / REGION_GRID_WIDTH;
	DWORD dwRegionY = nYCell / REGION_GRID_HEIGHT;

	KRegion* pRegion = GetRegion(dwRegionX, dwRegionY);
	KG_PROCESS_SUCCESS(!pRegion);

	nXCell = nXCell % REGION_GRID_WIDTH;
	nYCell = nYCell % REGION_GRID_HEIGHT;

	return pRegion->CheckObstacle(nXCell, nYCell, nLayer);

Exit1: 
	return TRUE;
}

DWORD KScene::GetScriptID(int nXCell, int nYCell)
{
	DWORD dwRegionX = nXCell / REGION_GRID_WIDTH;
	DWORD dwRegionY = nYCell / REGION_GRID_HEIGHT;

	KRegion* pRegion = GetRegion(dwRegionX, dwRegionY);
	KG_PROCESS_ERROR(pRegion);

	nXCell = nXCell % REGION_GRID_WIDTH;
	nYCell = nYCell % REGION_GRID_HEIGHT;

	return pRegion->GetScriptID(nXCell, nYCell);
Exit0:
	return 0;
}


BOOL KScene::SetRegionWidth(int nWidth)
{
	KGLOG_PROCESS_ERROR(nWidth <= MAX_REGION_WIDTH);

	m_nRegionWidth = nWidth;

	return TRUE;

Exit0:

	return FALSE;
}

BOOL KScene::SetRegionHeight(int nHeight)
{
	KGLOG_PROCESS_ERROR(nHeight <= MAX_REGION_HEIGHT);

	m_nRegionHeight = nHeight;

	return TRUE;

Exit0:

	return FALSE;
}

int KScene::GetRegionWidth(void)
{
	return m_nRegionWidth;
}

int KScene::GetRegionHeight(void)
{
	return m_nRegionHeight;
}

BOOL KScene::SetRegion(int nRegionX, int nRegionY, KRegion* pRegion)
{
	ASSERT(pRegion);
	ASSERT(nRegionX < MAX_REGION_WIDTH);
	ASSERT(nRegionY < MAX_REGION_HEIGHT);
    ASSERT(m_RegionList[nRegionX][nRegionY] == NULL);

	m_RegionList[nRegionX][nRegionY] = pRegion;

	return true;
}

KRegion* KScene::GetRegion(int nRegionX, int nRegionY)
{
	if (nRegionX >= 0 && nRegionX < m_nRegionWidth && nRegionY >= 0 && nRegionY < m_nRegionHeight)
    {
		return m_RegionList[nRegionX][nRegionY];
    }
	return NULL;
}

BOOL KScene::AddNpc(KNpc* pNpc)
{
	BOOL bRetCode = FALSE;
	KRegion* pRegion = NULL;


	//得到Region的指针
	pRegion = GetRegionByPoint(pNpc->m_nX, pNpc->m_nY);
	KGLOG_PROCESS_ERROR(pRegion);

	//向Region中添加Npc
	bRetCode = pRegion->AddNpc(pNpc);
	KGLOG_PROCESS_ERROR(bRetCode);

	pNpc->m_pScene = this;

	return TRUE;
Exit0:
	return FALSE;
}

BOOL KScene::RemoveNpc(KNpc* pNpc)
{
	BOOL        bRetCode = false;
    KRegion*    pRegion  = NULL;

	pRegion = pNpc->m_pRegion;
    ASSERT(pRegion);

	bRetCode = pRegion->RemoveNpc(pNpc);
	ASSERT(bRetCode);

	pNpc->m_pScene = NULL;

	return true;
}

BOOL KScene::MoveNpc(KNpc* pNpc, int nX, int nY, int nZ)
{
    ASSERT(pNpc);
    return pNpc->MoveTo(nX, nY, nZ);
}

BOOL KScene::AddDoodad(KDoodad* pDoodad)
{
	BOOL bRetCode = FALSE;
	KRegion* pRegion = NULL;

	//检测传入指针
	KGLOG_PROCESS_ERROR(pDoodad);

	//得到Region的指针
	pRegion = GetRegionByPoint(pDoodad->m_nX, pDoodad->m_nY);
	KGLOG_PROCESS_ERROR(pRegion);

	//向Region中添加Doodad
	bRetCode = pRegion->AddDoodad(pDoodad);
	KGLOG_PROCESS_ERROR(bRetCode);

	pDoodad->m_pScene = this;

	bRetCode = pDoodad->ApplyObstacle();
	KGLOG_CHECK_ERROR(bRetCode);

	return TRUE;

Exit0:

	return FALSE;
}

BOOL KScene::RemoveDoodad(KDoodad* pDoodad)
{
	BOOL bRetCode = FALSE;

	//检测传入的Doodad指针
	KGLOG_PROCESS_ERROR(pDoodad);

	//移处障碍信息
	bRetCode = pDoodad->RemoveObstacle();
	KGLOG_CHECK_ERROR(bRetCode);

	//得到并检测Doodad所在Region的指针
	KRegion* pRegion = pDoodad->m_pRegion;
	KGLOG_PROCESS_ERROR(pRegion);

	//删除Region中的Doodad
	bRetCode = pRegion->RemoveDoodad(pDoodad);
	KGLOG_PROCESS_ERROR(bRetCode);

	pDoodad->m_pScene = NULL;

	return TRUE;

Exit0:

	return FALSE;
}

BOOL KScene::MoveDoodad(KDoodad* pDoodad, int nX, int nY, int nZ /* = 0 */)
{
	return TRUE;
}

BOOL KScene::AddPlayer(KPlayer* pPlayer)
{
	BOOL        bRetCode    = false;
	KRegion*    pRegion     = NULL;


	//得到Region指针
	pRegion = GetRegionByPoint(pPlayer->m_nX, pPlayer->m_nY);
	KGLOG_PROCESS_ERROR(pRegion);

	//向Region中添加Player
	bRetCode = pRegion->AddPlayer(pPlayer);
	KGLOG_PROCESS_ERROR(bRetCode);

	pPlayer->m_pScene = this;

	return TRUE;

Exit0:
	
	return FALSE;
}

BOOL KScene::RemovePlayer(KPlayer* pPlayer)
{
	BOOL bRetCode = FALSE;

	//检测传入的Player指针
	KGLOG_PROCESS_ERROR(pPlayer);

	//得到并检测Player所在Region的指针
	KRegion* pRegion = pPlayer->m_pRegion;
	KGLOG_PROCESS_ERROR(pRegion);

	//删除Region中的Player
	bRetCode = pRegion->RemovePlayer(pPlayer);
	KGLOG_PROCESS_ERROR(bRetCode);

	pPlayer->m_pScene = NULL;

	//KGLogPrintf(KGLOG_DEBUG, "Remove player %s %d from scene %d-%d",
	//	pPlayer->m_szName, pPlayer->m_dwID, m_dwMapID, m_dwMapCopyIndex);

	return TRUE;

Exit0:

	return FALSE;
}


//BOOL KScene::SetObstacle(int nXCell, int nYCell, KObstacle* pObstacle)
//{
//	BOOL bRetCode = FALSE;
//
//	//计算Region的坐标
//	DWORD dwRegionX = nXCell / REGION_GRID_WIDTH;
//	DWORD dwRegionY = nYCell / REGION_GRID_HEIGHT;
//
//	//检测坐标的合法性
//	KGLOG_PROCESS_ERROR(dwRegionX < m_nRegionWidth);
//	KGLOG_PROCESS_ERROR(dwRegionY < m_nRegionHeight);
//	
//	KRegion* pRegion = m_RegionList[dwRegionX][dwRegionY];
//
//	KGLOG_PROCESS_ERROR(pRegion);
//
//	int nXCellinRegion = nXCell % REGION_GRID_WIDTH;
//	int nYCellinRegion = nYCell % REGION_GRID_HEIGHT;
//
//	KGLOG_PROCESS_ERROR(nXCellinRegion >= 0 && nXCellinRegion < REGION_GRID_WIDTH);
//	KGLOG_PROCESS_ERROR(nYCellinRegion >= 0 && nYCellinRegion < REGION_GRID_HEIGHT);
//
//	KCell* pCell = &pRegion->m_Cells[nXCellinRegion][nYCellinRegion];
//	KGLOG_PROCESS_ERROR(pCell);
//
//	pCell->m_pObstacle = pObstacle;
//
//	return TRUE;
//
//Exit0:
//
//	return FALSE;
//}
//
//BOOL KScene::GetObstacle(int nXCell, int nYCell, KObstacle* pObstacle)
//{
//	BOOL bRetCode = FALSE;
//
//	//计算Region的坐标
//	DWORD dwRegionX = nXCell / REGION_GRID_WIDTH;
//	DWORD dwRegionY = nYCell / REGION_GRID_HEIGHT;
//
//	//检测坐标的合法性
//	KGLOG_PROCESS_ERROR(dwRegionX < m_nRegionWidth);
//	KGLOG_PROCESS_ERROR(dwRegionY < m_nRegionHeight);
//
//	KRegion* pRegion = m_RegionList[dwRegionX][dwRegionY];
//
//	KGLOG_PROCESS_ERROR(pRegion);
//
//	int nXCellinRegion = nXCell % REGION_GRID_WIDTH;
//	int nYCellinRegion = nYCell % REGION_GRID_HEIGHT;
//
//	KGLOG_PROCESS_ERROR(nXCellinRegion >= 0 && nXCellinRegion < REGION_GRID_WIDTH);
//	KGLOG_PROCESS_ERROR(nYCellinRegion >= 0 && nYCellinRegion < REGION_GRID_HEIGHT);
//
//	KCell* pCell = &pRegion->m_Cells[nXCellinRegion][nYCellinRegion];
//	KGLOG_PROCESS_ERROR(pCell);
//
//	pObstacle = pCell->m_pObstacle;
//
//	return TRUE;
//
//Exit0:
//
//	return FALSE;
//}

#ifdef _SERVER

BOOL KScene::AddReviveNpc(KNpc *pNpc, BOOL bIgnoreReliveTime/* = FALSE*/)
{
	BOOL bResult = FALSE;
    KNPC_REBIRTH_INFO ReviveNpc;
    CREBIRTH_LIST::iterator it;

	// 重生时间为0的,默认不加入重生队列
	if (pNpc->m_nReviveTime == 0)
	{
		KGLogPrintf(KGLOG_WARNING, "Try to add NPC [%u] to revive list with delay time 0 !\n", pNpc->m_dwTemplateID);
		return true;
	}


	if (pNpc->m_dwReliveID)
	{
		KNpcRelive* pNpcRelive = g_WorldSettings.m_NpcReliveList.GetNpcRelive(m_dwMapID, pNpc->m_dwReliveID);
		KGLOG_PROCESS_ERROR(pNpcRelive);

		bResult = pNpcRelive->ReleaseRelivePoint(pNpc->m_nCurrentReliveIndex);
		KGLOG_PROCESS_ERROR(bResult);

		pNpc->m_nCurrentReliveIndex = -1;

		bResult = TRUE;
	}
Exit0:
	if (bIgnoreReliveTime)
		ReviveNpc.ReviveTime = g_SO3World.m_CurrentTime;
	else
		ReviveNpc.ReviveTime  = g_SO3World.m_CurrentTime + pNpc->m_nReviveTime;
    ReviveNpc.pNpc        = pNpc;

    for (it = m_ReviveList.begin(); it != m_ReviveList.end(); ++it)
    {
        if (it->ReviveTime > ReviveNpc.ReviveTime)
            break;
    }

    m_ReviveList.insert(it, ReviveNpc);

	return TRUE;
}

void KScene::CheckNpcRevive()
{
    BOOL	bRetCode = false;
	KNpc*	pNpc = NULL;
	KCell*	pCell = NULL;
    CREBIRTH_LIST::iterator it;

    for (it = m_ReviveList.begin(); it != m_ReviveList.end(); NULL)
    {
        if (it->ReviveTime > g_SO3World.m_CurrentTime)
            break;
        
        pNpc = it->pNpc;

		if (pNpc->m_dwReliveID)
		{
			KNpcRelive* pNpcRelive = g_WorldSettings.m_NpcReliveList.GetNpcRelive(m_dwMapID, pNpc->m_dwReliveID);
			KGLOG_PROCESS_ERROR(pNpcRelive);
			
			KNpcRelivePoint rNpcRelivePoint;
			bRetCode = pNpcRelive->GetFreeRelivePoint(rNpcRelivePoint);
			if (bRetCode == FALSE)
			{
				it = m_ReviveList.erase(it);
				AddReviveNpc(pNpc);
				return;
			}

			pNpc->m_nCurrentReliveIndex = rNpcRelivePoint.dwIndex;
			pNpc->m_nFaceDirection = rNpcRelivePoint.nDir;

			KRegion* pRegion = GetRegionByPoint(rNpcRelivePoint.nX, rNpcRelivePoint.nY);
			KGLOG_PROCESS_ERROR(pRegion);

			pCell = &pRegion->m_Cells[rNpcRelivePoint.nX / CELL_LENGTH % REGION_GRID_WIDTH][rNpcRelivePoint.nY / CELL_LENGTH % REGION_GRID_HEIGHT];
			KGLOG_PROCESS_ERROR(pCell);

			bRetCode = pNpc->AdjustLevel();
			KGLOG_PROCESS_ERROR(bRetCode);
	
			bRetCode = g_SO3World.AddNpc(pNpc, this, rNpcRelivePoint.nX, 
													rNpcRelivePoint.nY, 
													rNpcRelivePoint.nZ);
			KGLOG_PROCESS_ERROR(bRetCode);	

			pNpc->Revive();

			bRetCode = pNpc->Turn(rNpcRelivePoint.nDir);
			KGLOG_PROCESS_ERROR(bRetCode);
		}
		else
		{
			int nX;
			int nY;
			int nZ;
			int nDir;

			bRetCode = pNpc->m_AIController.GetOriginPoint(nX, nY, nZ, nDir);
			KGLOG_PROCESS_ERROR(bRetCode);
			
			pNpc->m_nFaceDirection = nDir;

			KRegion* pRegion = GetRegionByPoint(nX, nY);
			KGLOG_PROCESS_ERROR(pRegion);

			pCell = &pRegion->m_Cells[nX / CELL_LENGTH % REGION_GRID_WIDTH][nY / CELL_LENGTH % REGION_GRID_HEIGHT];
			KGLOG_PROCESS_ERROR(pCell);

			bRetCode = pNpc->AdjustLevel();
			KGLOG_PROCESS_ERROR(bRetCode);

			bRetCode = g_SO3World.AddNpc(pNpc, this, nX, nY, nZ);
			KGLOG_PROCESS_ERROR(bRetCode);

			pNpc->Revive();
		}

		pNpc->m_AIController.SetOriginPoint(pNpc->m_nX, pNpc->m_nY, 
			pNpc->m_nZ + pCell->m_wHighLayer * POINT_PER_ALTITUDE, pNpc->m_nFaceDirection);
		pNpc->m_AIController.Restart();
		pNpc->m_AIController.DoIdle(0);
		
        it = m_ReviveList.erase(it);
    }

	return ;
Exit0:
	if (pNpc)
	{
		g_SO3World.RemoveNpc(pNpc);

		pNpc->UnInit();
		g_ObjectManager.m_NpcSet.Delete(pNpc);
	}
	it = m_ReviveList.erase(it);

	return;
}

BOOL KScene::AddReviveDoodad(KDoodad *pDoodad, BOOL bIgnoreReliveTime/* = FALSE*/)
{
	BOOL bRetCode = FALSE;
	KDoodadTemplate *pDoodadTemplate = NULL;
	KDOODAD_REVIVE_INFO ReviveDoodad;
	DOODAD_REVIVE_LIST::iterator it;

	ASSERT(pDoodad);

	pDoodadTemplate = g_WorldSettings.m_DoodadTemplateList.GetTemplate(pDoodad->m_dwTemplateID);
	KGLOG_PROCESS_ERROR(pDoodadTemplate);

	// 重生时间为0的,默认不加入重生队列
	if (pDoodadTemplate->m_nReviveDelay == 0)
	{
		KGLogPrintf(KGLOG_WARNING, "Try to add doodad [%u] to revive list with delay time 0 !\n", pDoodad->m_dwTemplateID);
		goto Exit1;
	}

	if (pDoodad->m_dwReliveID)
	{
		KDoodadRelive* pDoodadRelive = g_WorldSettings.m_DoodadReliveList.GetDoodadRelive(m_dwMapID, pDoodad->m_dwReliveID);
		if (pDoodadRelive)
		{
			bRetCode = pDoodadRelive->ReleaseRelivePoint(pDoodad->m_nCurrentReliveIndex);
			KGLOG_CHECK_ERROR(bRetCode);

			pDoodad->m_nCurrentReliveIndex = -1;
		}
	}

	if (bIgnoreReliveTime)
		ReviveDoodad.ReviveTime = g_SO3World.m_CurrentTime;
	else
		ReviveDoodad.ReviveTime = g_SO3World.m_CurrentTime + pDoodadTemplate->m_nReviveDelay;
	ReviveDoodad.pDoodad = pDoodad;
	ReviveDoodad.nX = pDoodad->m_nX;
	ReviveDoodad.nY = pDoodad->m_nY;
	ReviveDoodad.nZ = pDoodad->m_nZ;
	ReviveDoodad.nDir = pDoodad->m_nFaceDirection;

	for (it = m_DoodadReviveList.begin(); it != m_DoodadReviveList.end(); ++it)
	{
		if (it->ReviveTime > ReviveDoodad.ReviveTime)
			break;
	}
	m_DoodadReviveList.insert(it, ReviveDoodad);

Exit1:
	return TRUE;

Exit0:
	return FALSE;
}

void KScene::CheckDoodadRevive()
{
	DOODAD_REVIVE_LIST::iterator it;
	KDoodad *pDoodad = NULL;
	BOOL bRetCode = FALSE;

	for (it = m_DoodadReviveList.begin(); it != m_DoodadReviveList.end(); )
	{
		if (it->ReviveTime > g_SO3World.m_CurrentTime)
			break;
		
		pDoodad = it->pDoodad;
		KGLOG_PROCESS_ERROR(pDoodad);

		if (pDoodad->m_dwReliveID)
		{
			KDoodadRelive* pDoodadRelive = g_WorldSettings.m_DoodadReliveList.\
											GetDoodadRelive(m_dwMapID, pDoodad->m_dwReliveID);
			KGLOG_PROCESS_ERROR(pDoodadRelive);

			KDoodadRelivePoint rDoodadRelivePoint;
			bRetCode = pDoodadRelive->GetFreeRelivePoint(rDoodadRelivePoint);
			if (bRetCode == FALSE)
			{
				//没有空闲的位置,重新加入死亡队列
				it = m_DoodadReviveList.erase(it);
				AddReviveDoodad(pDoodad);
				return;
			}

			pDoodad->m_nFaceDirection = rDoodadRelivePoint.nDir;
			pDoodad->m_nCurrentReliveIndex = (int)rDoodadRelivePoint.dwIndex;

			KRegion* pRegion = GetRegionByPoint(rDoodadRelivePoint.nX, rDoodadRelivePoint.nY);
			KGLOG_PROCESS_ERROR(pRegion);

			KCell* pCell = &pRegion->m_Cells[rDoodadRelivePoint.nX / CELL_LENGTH % REGION_GRID_WIDTH][rDoodadRelivePoint.nY / CELL_LENGTH % REGION_GRID_HEIGHT];
			KGLOG_PROCESS_ERROR(pCell);

			bRetCode = g_SO3World.AddDoodad(pDoodad, this, rDoodadRelivePoint.nX, 
				rDoodadRelivePoint.nY, 
				rDoodadRelivePoint.nZ + pCell->m_wHighLayer * POINT_PER_ALTITUDE);
			KGLOG_PROCESS_ERROR(bRetCode);

			pDoodad->Relive();
		}
		else
		{
			pDoodad->m_nFaceDirection = it->nDir;

			KRegion* pRegion = GetRegionByPoint(it->nX, it->nY);
			KGLOG_PROCESS_ERROR(pRegion);

			KCell* pCell = &pRegion->m_Cells[it->nX / CELL_LENGTH % REGION_GRID_WIDTH][it->nY / CELL_LENGTH % REGION_GRID_HEIGHT];
			KGLOG_PROCESS_ERROR(pCell);

			bRetCode = g_SO3World.AddDoodad(pDoodad, this, it->nX, it->nY, pCell->m_wHighLayer * POINT_PER_ALTITUDE + it->nZ);
			//bRetCode = g_SO3World.AddDoodad(pDoodad, this, it->nX, it->nY, it->nZ + pCell->m_wHighLayer * POINT_PER_ALTITUDE);
			KGLOG_PROCESS_ERROR(bRetCode);

			pDoodad->Relive();
		}

		it = m_DoodadReviveList.erase(it);
	}
	
	return ;
Exit0:
	if (pDoodad)
	{
		g_SO3World.RemoveDoodad(pDoodad);
		
		pDoodad->UnInit();
		g_ObjectManager.m_DoodadSet.Delete(pDoodad);
	}
	it = m_DoodadReviveList.erase(it);
	return ;
}

BOOL KScene::AddNickNameNpc(KNpc* pNpc)
{
	BOOL bRetCode = FALSE;
	
	KGLOG_PROCESS_ERROR(pNpc);
	if (pNpc->m_szName && pNpc->m_szName[0])
	{
		string Name = pNpc->m_szName;
		m_mapNickNameList[Name] = pNpc->m_dwID;
	}

	bRetCode = TRUE;
Exit0:
	return bRetCode;
}

KNpc* KScene::GetNickNameNpc(const char* szNpcName)
{
	KNpc* pNpc = NULL;
	KNICK_NAME_LIST::iterator it;

	if (szNpcName && szNpcName[0])
	{
		string Name = szNpcName;
		
		it = m_mapNickNameList.find(Name);
		if (it != m_mapNickNameList.end())
		{
			pNpc = g_ObjectManager.m_NpcSet.GetObj(it->second);
		}
	}

	return pNpc;
}

#endif

BOOL KScene::PreprocessCalculateData()
{
    BOOL bResult    = false;
    BOOL bRetCode   = false;

    bRetCode = ReviseCalculateGradient();
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = CalculateObstacleRange();
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KScene::ReviseCalculateGradient()
{
    BOOL        bResult             = false;    
    KCell*      pLowestCell         = NULL;
	KRegion*    pProcessRegion      = NULL;
    KCell*      pProcessCell        = NULL;
    int         nProcessPointX      = 0;
    int         nProcessPointY      = 0;
    int         nLowerPointX        = 0;
    int         nLowerPointY        = 0;
    int         nLowerCellX         = 0;
    int         nLowerCellY         = 0;
	KRegion*    pLowerRegion        = NULL;
    KCell*      pLowerCell          = NULL;
    int         nGradientDirection  = 0;
    int         nSceneMaxPointX     = 0;
    int         nSceneMaxPointY     = 0;
    int         nRelativeHeight     = 0;
    int         nDistanceXY         = 0;
    int         nGradientDegree     = 0;
    int         nOriginalGradient   = 0;

    nSceneMaxPointX = m_nRegionWidth * REGION_GRID_WIDTH * CELL_LENGTH;
    nSceneMaxPointY = m_nRegionHeight * REGION_GRID_HEIGHT * CELL_LENGTH;

	for (int nYCell = 0; nYCell < m_nRegionHeight * REGION_GRID_HEIGHT; nYCell++)
	{
		for (int nXCell = 0; nXCell < m_nRegionWidth * REGION_GRID_WIDTH; nXCell++)
		{
            pProcessRegion = GetRegion(nXCell / REGION_GRID_WIDTH, nYCell / REGION_GRID_HEIGHT);
			KGLOG_PROCESS_ERROR(pProcessRegion);

			pLowestCell = pProcessRegion->GetLowestObstacle(
                nXCell % REGION_GRID_WIDTH, nYCell % REGION_GRID_HEIGHT
            );
            KGLOG_PROCESS_ERROR(pLowestCell);

            for (pProcessCell = pLowestCell; pProcessCell != NULL; pProcessCell = pProcessCell->m_pNext)
            {
                if (pProcessCell->m_BaseInfo.dwCellType == ctWater)
                    continue;

                nOriginalGradient = pProcessCell->GetGradientDegree();
                if (!nOriginalGradient)
                    continue;

                nGradientDirection = pProcessCell->GetGradientDirection();
                
                nLowerPointX = CELL_LENGTH * nXCell + CELL_LENGTH / 2 + 
                    CELL_LENGTH * g_Cos(nGradientDirection) / SIN_COS_NUMBER;

                nLowerPointY = CELL_LENGTH * nYCell + CELL_LENGTH / 2 + 
                    CELL_LENGTH * g_Sin(nGradientDirection) / SIN_COS_NUMBER;

                if (nLowerPointX < 0 || nLowerPointX >= nSceneMaxPointX)
                    continue;

                if (nLowerPointY < 0 || nLowerPointY >= nSceneMaxPointY)
                    continue;

                nLowerCellX = nLowerPointX / CELL_LENGTH;
                nLowerCellY = nLowerPointY / CELL_LENGTH;

                nLowerPointX = nLowerCellX * CELL_LENGTH + CELL_LENGTH / 2;
                nLowerPointY = nLowerCellY * CELL_LENGTH + CELL_LENGTH / 2;

                pLowerRegion = GetRegionByPoint(nLowerPointX, nLowerPointY);
                KGLOG_PROCESS_ERROR(pLowerRegion);

                pLowerCell = pLowerRegion->GetAdaptedCellForObj(
                    nLowerCellX % REGION_GRID_WIDTH, nLowerCellY % REGION_GRID_HEIGHT,
                    pProcessCell->m_wHighLayer * POINT_PER_ALTITUDE,
                    0, 0, &nRelativeHeight, NULL
                );
                if (!pLowerCell)
                {
                    pProcessCell->SetGradientDegree(0);
                    continue;
                }

                nProcessPointX = nXCell * CELL_LENGTH + CELL_LENGTH / 2;
                nProcessPointY = nYCell * CELL_LENGTH + CELL_LENGTH / 2;

                nDistanceXY = (int)sqrt(
                    (float)(nLowerPointX - nProcessPointX) * (nLowerPointX - nProcessPointX) +
                    (float)(nLowerPointY - nProcessPointY) * (nLowerPointY - nProcessPointY)
                );

                nGradientDegree = g_GetDirection(
                    0, 0, XYPOINT_TO_ZPOINT(nDistanceXY), nRelativeHeight
                );

                ASSERT(nGradientDegree >= 0 && nGradientDegree < DIRECTION_COUNT / 4);
                
                pProcessCell->SetGradientDegree(nGradientDegree);
            }
        }
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KScene::CalculateObstacleRange()
{
	int nLastDistance = 0;
	for (int nYCell = 0; nYCell < m_nRegionHeight * REGION_GRID_HEIGHT; nYCell++)
	{
		for (int nXCell = 0; nXCell < m_nRegionWidth * REGION_GRID_WIDTH; nXCell++)
		{
			KRegion* pRegion = GetRegion(nXCell / REGION_GRID_WIDTH, nYCell / REGION_GRID_HEIGHT);
			KGLOG_PROCESS_ERROR(pRegion);

			KCell* pCell = &(pRegion->m_Cells[nXCell % REGION_GRID_WIDTH][nYCell % REGION_GRID_HEIGHT]);
			
			int nDistance = (nLastDistance - 1) >= 0 ? nLastDistance : 0;
			while (nDistance < 64)
			{
				BOOL bIsObstacle = FALSE;

				for (int nDestX = nXCell - nDistance; nDestX < nXCell + nDistance; nDestX++)
				{
					int nDestY = nYCell - nDistance;
					KRegion* pDestRegion = GetRegion(nDestX / REGION_GRID_WIDTH, nDestY / REGION_GRID_HEIGHT);
					if (pDestRegion && nDestX >= 0 && nDestY >= 0)
					{
						KCell* pDestCell = &(pDestRegion->m_Cells[nDestX % REGION_GRID_WIDTH][nDestY % REGION_GRID_HEIGHT]);

						if (pDestCell->m_wHighLayer > pCell->m_wHighLayer + 8 || 
							pDestCell->m_pNext != NULL)
						{
							bIsObstacle = TRUE;
							break;
						}
					}

					nDestY = nYCell + nDistance;
					pDestRegion = GetRegion(nDestX / REGION_GRID_WIDTH, nDestY / REGION_GRID_HEIGHT);
					if (pDestRegion && nDestX >= 0 && nDestY >= 0)
					{
						KCell* pDestCell = &(pDestRegion->m_Cells[nDestX % REGION_GRID_WIDTH][nDestY % REGION_GRID_HEIGHT]);

						if (pDestCell->m_wHighLayer > pCell->m_wHighLayer + 8 || 
							pDestCell->m_pNext != NULL)
						{
							bIsObstacle = TRUE;
							break;
						}
					}
				}
				if (bIsObstacle)
					break;

				for (int nDestY = nYCell - nDistance; nDestY < nYCell + nDistance; nDestY++)
				{
					int nDestX = nXCell - nDistance;
					KRegion* pDestRegion = GetRegion(nDestX / REGION_GRID_WIDTH, nDestY / REGION_GRID_HEIGHT);
					if (pDestRegion && nDestX >= 0 && nDestY >= 0)
					{
						KCell* pDestCell = &(pDestRegion->m_Cells[nDestX % REGION_GRID_WIDTH][nDestY % REGION_GRID_HEIGHT]);

						if (pDestCell->m_wHighLayer > pCell->m_wHighLayer + 8 || 
							pDestCell->m_pNext != NULL)
						{
							bIsObstacle = TRUE;
							break;
						}
					}

					nDestX = nXCell + nDistance;
					pDestRegion = GetRegion(nDestX / REGION_GRID_WIDTH, nDestY / REGION_GRID_HEIGHT);
					if (pDestRegion && nDestX >= 0 && nDestY >= 0)
					{
						KCell* pDestCell = &(pDestRegion->m_Cells[nDestX % REGION_GRID_WIDTH][nDestY % REGION_GRID_HEIGHT]);

						if (pDestCell->m_wHighLayer > pCell->m_wHighLayer + 8 || 
							pDestCell->m_pNext != NULL)
						{
							bIsObstacle = TRUE;
							break;
						}
					}
				}
				if (bIsObstacle)
					break;

				nDistance++;
			}

			pCell->m_BaseInfo.dwNoObstacleRange = nDistance < 64 ? nDistance : 63;
			nLastDistance = nDistance;
		}
	}

	return TRUE;

Exit0:

	return FALSE;
}

BOOL KScene::IsVisible(int nSrcX, int nSrcY, int nSrcZ, int nDstX, int nDstY, int nDstZ)
{
    BOOL        bResult             = false;    // 默认有障碍
    KRegion*    pSrcRegion          = NULL;
    KCell*      pSrcCell            = NULL;
    int         nSrcCellX           = 0;
    int         nSrcCellY           = 0;
    DWORD       dwSrcRegionX        = 0;
    DWORD       dwSrcRegionY        = 0;
    int         nDstCellX           = 0;   
    int         nDstCellY           = 0;   
    DWORD       dwDstRegionX        = 0;
    DWORD       dwDstRegionY        = 0;
    KRegion*    pDstRegion          = NULL;
    KCell*      pDstCell            = NULL;
    int         nNoObstacleRange    = 0;
    int         nDeltaX             = 0;
    int         nDeltaY             = 0;
    int         nLastAltitude       = 0;
    int         nProportionX        = 0;
    int         nProportionY        = 0;
    int         nProportionZ        = 0;
    KRegion*    pRegion             = NULL;
    KCell*      pCell               = NULL; 
    int         nAltitude           = 0;
    int         nXCell              = 0;
    int         nYCell              = 0;
    int         nZCell              = 0;
    DWORD       dwRegionX           = 0;
    DWORD       dwRegionY           = 0;
    DWORD       dwRegionZ           = 0;

    nSrcCellX       = nSrcX / CELL_LENGTH;
    nSrcCellY       = nSrcY / CELL_LENGTH;
	dwSrcRegionX    = nSrcCellX / REGION_GRID_WIDTH;
	dwSrcRegionY    = nSrcCellY / REGION_GRID_HEIGHT;

    pSrcRegion     = GetRegion(dwSrcRegionX, dwSrcRegionY);
    KG_PROCESS_ERROR(pSrcRegion);

	pSrcCell = &(pSrcRegion->m_Cells[nSrcCellX % REGION_GRID_WIDTH][nSrcCellY % REGION_GRID_HEIGHT]);
	KG_PROCESS_ERROR(pSrcCell);

    nDstCellX       = nDstX / CELL_LENGTH;
    nDstCellY       = nDstY / CELL_LENGTH;
	dwDstRegionX    = nDstCellX / REGION_GRID_WIDTH;
	dwDstRegionY    = nDstCellY / REGION_GRID_HEIGHT;

    pDstRegion = GetRegion(dwDstRegionX, dwDstRegionY);
    KG_PROCESS_ERROR(pDstRegion);

	pDstCell = &(pDstRegion->m_Cells[nDstCellX % REGION_GRID_WIDTH][nDstCellY % REGION_GRID_HEIGHT]);
	KG_PROCESS_ERROR(pDstCell);

	if (nSrcCellX == nDstCellX && nSrcCellY == nDstCellY)
	{
        // 同一个Cell
        goto Exit1;
	}
  
	// 检测是否在无障碍范围内
	nNoObstacleRange = 
		pSrcCell->m_BaseInfo.dwNoObstacleRange > pDstCell->m_BaseInfo.dwNoObstacleRange ?
		pSrcCell->m_BaseInfo.dwNoObstacleRange : pDstCell->m_BaseInfo.dwNoObstacleRange;

   
    // 采用这个判断时会跳过地形因素造成的不可见,所以暂时去掉
    /*   
	if (abs(nSrcCellX - nDstCellX) < nNoObstacleRange && abs(nSrcCellY - nDstCellY) < nNoObstacleRange)
    {
        // 没有障碍
        goto Exit0;
    }
    */

	//int nDirection = g_GetDirection(nSrcCellX, nSrcCellY, nDstCellX, nDstCellY);

	//if ((nDirection < (DIRECTION_COUNT * 1 / 8) || nDirection > (DIRECTION_COUNT * 7 / 8)) ||
	//	(nDirection > (DIRECTION_COUNT * 3 / 8) && nDirection < (DIRECTION_COUNT * 5 / 8)))

    nDeltaX         = abs(nDstCellX - nSrcCellX);
    nDeltaY         = abs(nDstCellY - nSrcCellY);
	nLastAltitude   = nSrcZ;

    if (nDeltaX > nDeltaY)
	{
		//计算Y轴和Z轴上的比例

        KGLOG_PROCESS_ERROR(nDeltaX);

		nProportionY = (nDstCellY - nSrcCellY) * SIN_COS_NUMBER / (nDstCellX - nSrcCellX);
		nProportionZ = (nDstZ - nSrcZ) * SIN_COS_NUMBER / (nDstCellX - nSrcCellX);

		if ((nSrcCellX < nDstCellX) && (nSrcZ <= nDstZ))
		{
			for (nXCell = (nSrcCellX + 1); nXCell <= nDstCellX; nXCell++)
			{
				nYCell = nSrcCellY + (nXCell - nSrcCellX) * nProportionY / SIN_COS_NUMBER;

				dwRegionX = nXCell / REGION_GRID_WIDTH;
				dwRegionY = nYCell / REGION_GRID_HEIGHT;

				pRegion = GetRegion(dwRegionX, dwRegionY);
				KG_PROCESS_ERROR(pRegion);
					
                pCell = &(pRegion->m_Cells[nXCell % REGION_GRID_WIDTH][nYCell % REGION_GRID_HEIGHT]);

				nAltitude = nSrcZ + (nXCell - nSrcCellX) * nProportionZ / SIN_COS_NUMBER;

				while (pCell)
				{
					KG_PROCESS_ERROR(
                        pCell->m_wLowLayer >= nAltitude || 
                        pCell->m_wHighLayer <= nLastAltitude || 
                        pCell->m_BaseInfo.dwCellType == ctWater
                    );
				    pCell = pCell->m_pNext;
				}
				nLastAltitude = nAltitude;
			}
		}
        else if ((nSrcCellX < nDstCellX) && (nSrcZ > nDstZ))
        {
			for (nXCell = (nSrcCellX + 1); nXCell <= nDstCellX; nXCell++)
			{
				nYCell = nSrcCellY + (nXCell - nSrcCellX) * nProportionY / SIN_COS_NUMBER;

				dwRegionX = nXCell / REGION_GRID_WIDTH;
				dwRegionY = nYCell / REGION_GRID_HEIGHT;

				pRegion = GetRegion(dwRegionX, dwRegionY);
				KG_PROCESS_ERROR(pRegion);
					
                pCell = &(pRegion->m_Cells[nXCell % REGION_GRID_WIDTH][nYCell % REGION_GRID_HEIGHT]);

				nAltitude = nSrcZ + (nXCell - nSrcCellX) * nProportionZ / SIN_COS_NUMBER;

				while (pCell)
				{
					KG_PROCESS_ERROR(
                        pCell->m_wLowLayer >= nLastAltitude || 
                        pCell->m_wHighLayer <= nAltitude || 
                        pCell->m_BaseInfo.dwCellType == ctWater
                    );
				    pCell = pCell->m_pNext;
				}
				nLastAltitude = nAltitude;
			}
        }
		else if ((nSrcCellX > nDstCellX) && (nSrcZ <= nDstZ))
		{
			for (nXCell = (nSrcCellX - 1); nXCell >= nDstCellX; nXCell--)
			{
				nYCell = nSrcCellY + (nXCell - nSrcCellX) * nProportionY / SIN_COS_NUMBER;

				dwRegionX = nXCell / REGION_GRID_WIDTH;
				dwRegionY = nYCell / REGION_GRID_HEIGHT;

				pRegion = GetRegion(dwRegionX, dwRegionY);
                KG_PROCESS_ERROR(pRegion);
					
                pCell = &(pRegion->m_Cells[nXCell % REGION_GRID_WIDTH][nYCell % REGION_GRID_HEIGHT]);

				nAltitude = nSrcZ + (nXCell - nSrcCellX) * nProportionZ / SIN_COS_NUMBER;

				while (pCell)
				{
					KG_PROCESS_ERROR(
                        pCell->m_wLowLayer >= nAltitude || 
                        pCell->m_wHighLayer <= nLastAltitude || 
                        pCell->m_BaseInfo.dwCellType == ctWater
                    );
				    pCell = pCell->m_pNext;
				}
				nLastAltitude = nAltitude;				
			}
		}
        else if ((nSrcCellX > nDstCellX) && (nSrcZ > nDstZ))
        {
			for (nXCell = (nSrcCellX - 1); nXCell >= nDstCellX; nXCell--)
			{
				nYCell = nSrcCellY + (nXCell - nSrcCellX) * nProportionY / SIN_COS_NUMBER;

				dwRegionX = nXCell / REGION_GRID_WIDTH;
				dwRegionY = nYCell / REGION_GRID_HEIGHT;

				pRegion = GetRegion(dwRegionX, dwRegionY);
                KG_PROCESS_ERROR(pRegion);
					
                pCell = &(pRegion->m_Cells[nXCell % REGION_GRID_WIDTH][nYCell % REGION_GRID_HEIGHT]);

				nAltitude = nSrcZ + (nXCell - nSrcCellX) * nProportionZ / SIN_COS_NUMBER;

				while (pCell)
				{
					KG_PROCESS_ERROR(
                        pCell->m_wLowLayer >= nLastAltitude || 
                        pCell->m_wHighLayer <= nAltitude || 
                        pCell->m_BaseInfo.dwCellType == ctWater
                    );
				    pCell = pCell->m_pNext;
				}
				nLastAltitude = nAltitude;				
			}
        }
	}
	else
	{
		//计算X轴和Z轴的比例

        KGLOG_PROCESS_ERROR(nDeltaY);

		nProportionX = (nDstCellX - nSrcCellX) * SIN_COS_NUMBER / (nDstCellY - nSrcCellY);
		nProportionZ = (nDstZ - nSrcZ) * SIN_COS_NUMBER / (nDstCellY - nSrcCellY);

		if ((nSrcCellY < nDstCellY) && (nSrcZ <= nDstZ))
		{
			for (nYCell = (nSrcCellY + 1); nYCell <= nDstCellY; nYCell++)
			{
				nXCell = nSrcCellX + (nYCell - nSrcCellY) * nProportionX / SIN_COS_NUMBER;

				dwRegionX = nXCell / REGION_GRID_WIDTH;
				dwRegionY = nYCell / REGION_GRID_HEIGHT;

				pRegion = GetRegion(dwRegionX, dwRegionY);
                KG_PROCESS_ERROR(pRegion);

				pCell = &(pRegion->m_Cells[nXCell % REGION_GRID_WIDTH][nYCell % REGION_GRID_HEIGHT]);

				nAltitude = nSrcZ + (nYCell - nSrcCellY) * nProportionZ / SIN_COS_NUMBER;

				while (pCell)
				{
					KG_PROCESS_ERROR(
                        pCell->m_wLowLayer >= nAltitude || 
                        pCell->m_wHighLayer <= nLastAltitude || 
                        pCell->m_BaseInfo.dwCellType == ctWater
                    );
					pCell = pCell->m_pNext;
				}
				nLastAltitude = nAltitude;
			}
		}
        else if ((nSrcCellY < nDstCellY) && (nSrcZ > nDstZ))
        {
			for (nYCell = (nSrcCellY + 1); nYCell <= nDstCellY; nYCell++)
			{
				nXCell = nSrcCellX + (nYCell - nSrcCellY) * nProportionX / SIN_COS_NUMBER;

				dwRegionX = nXCell / REGION_GRID_WIDTH;
				dwRegionY = nYCell / REGION_GRID_HEIGHT;

				pRegion = GetRegion(dwRegionX, dwRegionY);
                KG_PROCESS_ERROR(pRegion);

				pCell = &(pRegion->m_Cells[nXCell % REGION_GRID_WIDTH][nYCell % REGION_GRID_HEIGHT]);

				nAltitude = nSrcZ + (nYCell - nSrcCellY) * nProportionZ / SIN_COS_NUMBER;

				while (pCell)
				{
					KG_PROCESS_ERROR(
                        pCell->m_wLowLayer >= nLastAltitude || 
                        pCell->m_wHighLayer <= nAltitude || 
                        pCell->m_BaseInfo.dwCellType == ctWater
                    );
					pCell = pCell->m_pNext;
				}
				nLastAltitude = nAltitude;
			}
        }
		else  if ((nSrcCellY > nDstCellY) && (nSrcZ <= nDstZ))
		{
			for (nYCell = (nSrcCellY - 1); nYCell >= nDstCellY; nYCell--)
			{
				nXCell = nSrcCellX + (nYCell - nSrcCellY) * nProportionX / SIN_COS_NUMBER;

				dwRegionX = nXCell / REGION_GRID_WIDTH;
				dwRegionY = nYCell / REGION_GRID_HEIGHT;

				pRegion = GetRegion(dwRegionX, dwRegionY);
                KG_PROCESS_ERROR(pRegion);

				pCell = &(pRegion->m_Cells[nXCell % REGION_GRID_WIDTH][nYCell % REGION_GRID_HEIGHT]);

				nAltitude = nSrcZ + (nYCell - nSrcCellY) * nProportionZ / SIN_COS_NUMBER;

				while (pCell)
				{
					KG_PROCESS_ERROR(
                        pCell->m_wLowLayer >= nAltitude || 
                        pCell->m_wHighLayer <= nLastAltitude ||
                        pCell->m_BaseInfo.dwCellType == ctWater
                    );
					pCell = pCell->m_pNext;
				}
				nLastAltitude = nAltitude;
			}
		}
        else if ((nSrcCellY > nDstCellY) && (nSrcZ > nDstZ))
        {
			for (nYCell = (nSrcCellY - 1); nYCell >= nDstCellY; nYCell--)
			{
				nXCell = nSrcCellX + (nYCell - nSrcCellY) * nProportionX / SIN_COS_NUMBER;

				dwRegionX = nXCell / REGION_GRID_WIDTH;
				dwRegionY = nYCell / REGION_GRID_HEIGHT;

				pRegion = GetRegion(dwRegionX, dwRegionY);
                KG_PROCESS_ERROR(pRegion);

				pCell = &(pRegion->m_Cells[nXCell % REGION_GRID_WIDTH][nYCell % REGION_GRID_HEIGHT]);

				nAltitude = nSrcZ + (nYCell - nSrcCellY) * nProportionZ / SIN_COS_NUMBER;

				while (pCell)
				{
					KG_PROCESS_ERROR(
                        pCell->m_wLowLayer >= nLastAltitude || 
                        pCell->m_wHighLayer <= nAltitude ||
                        pCell->m_BaseInfo.dwCellType == ctWater
                    );
					pCell = pCell->m_pNext;
				}
				nLastAltitude = nAltitude;
			}
        }
	}    

Exit1:
    bResult = true;
Exit0:
    return bResult;
}

BOOL KScene::IsReachable(int nSrcX, int nSrcY, int nSrcZ, int nDstX, int nDstY, int nDstZ, int nHeight)
{
    BOOL    bResult         = false;
    KCell*  pPreCell        = NULL;

    int nSrcCellX = nSrcX / CELL_LENGTH;
    int nSrcCellY = nSrcY / CELL_LENGTH;
    
    KRegion* pSrcRegion = GetRegion(nSrcCellX / REGION_GRID_WIDTH, nSrcCellY / REGION_GRID_HEIGHT);
    KG_PROCESS_ERROR(pSrcRegion);
    KCell* pSrcCell = &(pSrcRegion->m_Cells[nSrcCellX % REGION_GRID_WIDTH][nSrcCellY % REGION_GRID_HEIGHT]);
    
    DWORD dwSrcHeigt = nSrcZ / POINT_PER_ALTITUDE;
    while (pSrcCell)
    {
        if ((pSrcCell->m_pNext == NULL) || (pSrcCell->m_pNext->m_wHighLayer > dwSrcHeigt))
            break;
        pSrcCell = pSrcCell->m_pNext;
    }
    KG_PROCESS_ERROR(pSrcCell);

    int nDstCellX = nDstX / CELL_LENGTH;
    int nDstCellY = nDstY / CELL_LENGTH;
    
    KRegion* pDstRegion = GetRegion(nDstCellX / REGION_GRID_WIDTH, nDstCellY / REGION_GRID_HEIGHT);
    KG_PROCESS_ERROR(pDstRegion);
    KCell* pDstCell = &(pDstRegion->m_Cells[nDstCellX % REGION_GRID_WIDTH][nDstCellY % REGION_GRID_HEIGHT]);
    
    DWORD dwDstHeight = nDstZ / POINT_PER_ALTITUDE;
    while (pDstCell)
    {
        if ((pDstCell->m_pNext == NULL) || (pDstCell->m_pNext->m_wHighLayer > dwDstHeight))
            break;
        pDstCell = pDstCell->m_pNext;
    }
    KG_PROCESS_ERROR(pDstCell);    

    if (pSrcCell == pDstCell)
        goto Exit1;

    pPreCell = pSrcCell;

    int nDeltaX = abs(nSrcCellX - nDstCellX);
    int nDeltaY = abs(nSrcCellY - nDstCellY);

    if (nDeltaX >= nDeltaY)
    {
        int nProportion = (nSrcY - nDstY) * SIN_COS_NUMBER / (nSrcX - nDstX);
        if (nSrcCellX >= nDstCellX)
        {
            for (int nXCell = nSrcCellX - 1; nXCell >= nDstCellX; --nXCell)
            {
                int nYCell = nSrcCellY + (nXCell - nSrcCellX) * (nSrcCellY - nDstCellY) / (nSrcCellX - nDstCellX);

                KRegion* pRegion = GetRegion(nXCell / REGION_GRID_WIDTH, nYCell / REGION_GRID_HEIGHT);
                KG_PROCESS_ERROR(pRegion);

                pPreCell = pRegion->GetLowerObstacle(
                    nXCell % REGION_GRID_WIDTH, nYCell % REGION_GRID_HEIGHT, pPreCell->m_wHighLayer * POINT_PER_ALTITUDE + CLIMB_ABILITY
                );
                KG_PROCESS_ERROR(pPreCell);

                if (pPreCell->m_pNext && (pPreCell->m_pNext->m_wLowLayer - pPreCell->m_wHighLayer) * POINT_PER_ALTITUDE < nHeight)
                   goto Exit0;
                
                KG_PROCESS_ERROR(!pPreCell->m_BaseInfo.dwBlockCharacter);
            }        
        }
        else
        {
            for (int nXCell = nSrcCellX + 1; nXCell <= nDstCellX; ++nXCell)
            {
                int nYCell = nSrcCellY +  (nXCell - nSrcCellX) * (nSrcCellY - nDstCellY) / (nSrcCellX - nDstCellX);

                KRegion* pRegion = GetRegion(nXCell / REGION_GRID_WIDTH, nYCell / REGION_GRID_HEIGHT);
                KG_PROCESS_ERROR(pRegion);

                pPreCell = pRegion->GetLowerObstacle(
                    nXCell % REGION_GRID_WIDTH, nYCell % REGION_GRID_HEIGHT, pPreCell->m_wHighLayer * POINT_PER_ALTITUDE + CLIMB_ABILITY
                );
                KG_PROCESS_ERROR(pPreCell);

                if (pPreCell->m_pNext && (pPreCell->m_pNext->m_wLowLayer - pPreCell->m_wHighLayer) * POINT_PER_ALTITUDE < nHeight)
                    goto Exit0;
                
                KG_PROCESS_ERROR(!pPreCell->m_BaseInfo.dwBlockCharacter);
            }   
        }

    }
    else
    {
        if (nSrcCellY >= nDstCellY)
        {
            for (int nYCell = nSrcCellY - 1; nYCell >= nDstCellY; --nYCell)
            {
                int nXCell = nSrcCellX + (nYCell - nSrcCellY) * (nSrcCellX - nDstCellX) / (nSrcCellY - nDstCellY);

                KRegion* pRegion = GetRegion(nXCell / REGION_GRID_WIDTH, nYCell / REGION_GRID_HEIGHT);
                KG_PROCESS_ERROR(pRegion);

                pPreCell = pRegion->GetLowerObstacle(
                    nXCell % REGION_GRID_WIDTH, nYCell % REGION_GRID_HEIGHT, pPreCell->m_wHighLayer * POINT_PER_ALTITUDE + CLIMB_ABILITY
                );
                KG_PROCESS_ERROR(pPreCell);

                if (pPreCell->m_pNext && (pPreCell->m_pNext->m_wLowLayer - pPreCell->m_wHighLayer) * POINT_PER_ALTITUDE < nHeight)
                    goto Exit0;

                KG_PROCESS_ERROR(!pPreCell->m_BaseInfo.dwBlockCharacter);
            }        
        }
        else
        {
            for (int nYCell = nSrcCellY + 1; nYCell <= nDstCellY; ++nYCell)
            {
                int nXCell = nSrcCellX + (nYCell - nSrcCellY) * (nSrcCellX - nDstCellX) / (nSrcCellY - nDstCellY);

                KRegion* pRegion = GetRegion(nXCell / REGION_GRID_WIDTH, nYCell / REGION_GRID_HEIGHT);
                KG_PROCESS_ERROR(pRegion);

                pPreCell = pRegion->GetLowerObstacle(
                    nXCell % REGION_GRID_WIDTH, nYCell % REGION_GRID_HEIGHT, pPreCell->m_wHighLayer * POINT_PER_ALTITUDE + CLIMB_ABILITY
                );
                KG_PROCESS_ERROR(pPreCell);

                if (pPreCell->m_pNext && (pPreCell->m_pNext->m_wLowLayer - pPreCell->m_wHighLayer) * POINT_PER_ALTITUDE < nHeight)
                    goto Exit0;

                KG_PROCESS_ERROR(!pPreCell->m_BaseInfo.dwBlockCharacter);
            }  
        }
    }
    
    KG_PROCESS_ERROR(pDstCell == pPreCell);

Exit1:
    bResult = true;
Exit0:
    return bResult;
}

