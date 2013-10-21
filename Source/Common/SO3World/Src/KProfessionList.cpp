#include "stdafx.h"
#include "KProfession.h"
#include "KProfessionList.h"
#include "KPlayer.h"
#include "KPlayerServer.h"
#include "KSO3World.h"

#define INIT_PROFESSION_MAX_LEVEL 30
#define INIT_PROFESSION_LEVEL 10

BOOL KProfessionList::Init(KPlayer *pPlayer)
{
	m_pPlayer = pPlayer;
	ZeroMemory(m_PlayerProfessionList, sizeof(m_PlayerProfessionList));

	return true;
}

void KProfessionList::UnInit()
{
    m_pPlayer = NULL;
	return;
}

#ifdef _SERVER
BOOL KProfessionList::Load(BYTE* pbyData, size_t uDataLen)
{
    BOOL                bResult             = false;
    KProfessionDBData*  pProfessionDBData   = (KProfessionDBData*)pbyData;;
    KProfessionItem*    pProfessionItem     = NULL;
    
    assert(pbyData);

    KGLOG_PROCESS_ERROR(
        uDataLen == sizeof(KProfessionDBData) + pProfessionDBData->byCount * sizeof(KProfessionItem)
    );

    for (int i = 0; i < pProfessionDBData->byCount; ++i)
    {
        pProfessionItem = &(pProfessionDBData->Profession[i]);

		KGLOG_PROCESS_ERROR(pProfessionItem->m_byProfessionID > 0);

        m_PlayerProfessionList[pProfessionItem->m_byProfessionID - 1].m_dwCurrentLevel          = pProfessionItem->m_byCurrentLevel;
        m_PlayerProfessionList[pProfessionItem->m_byProfessionID - 1].m_dwCurrentProficiency    = pProfessionItem->m_wProficiency;
        m_PlayerProfessionList[pProfessionItem->m_byProfessionID - 1].m_dwMaxLevel              = pProfessionItem->m_byMaxLevel;
        m_PlayerProfessionList[pProfessionItem->m_byProfessionID - 1].m_bIsLearnd               = true;
        m_PlayerProfessionList[pProfessionItem->m_byProfessionID - 1].m_dwBranchID              = pProfessionItem->m_byBranchID;

        g_PlayerServer.DoUpdatePlayerProfession(
            m_pPlayer->m_nConnIndex,
            pProfessionItem->m_byProfessionID,
            m_PlayerProfessionList[pProfessionItem->m_byProfessionID - 1].m_dwMaxLevel,
            m_PlayerProfessionList[pProfessionItem->m_byProfessionID - 1].m_dwCurrentLevel,
            m_PlayerProfessionList[pProfessionItem->m_byProfessionID - 1].m_dwCurrentProficiency,
            m_PlayerProfessionList[pProfessionItem->m_byProfessionID - 1].m_dwBranchID
        );
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KProfessionList::Save(size_t* puUseSize, BYTE* pbyBuffer, size_t uBufferSize)
{
    BOOL                        bResult	                = false;
    size_t                      uLeftSize               = uBufferSize;
    KPlayerProfession*          pProfession             = NULL;
    KProfessionDBData*          pProfessionDBData       = NULL;
    KProfessionItem*            pProfessionItem         = NULL;
    int                         nSaveProfessionIndex    = 0;

    assert(puUseSize);
    assert(pbyBuffer);
    
    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(KProfessionDBData));
    
    pProfessionDBData = (KProfessionDBData*)pbyBuffer;
    uLeftSize -= sizeof(KProfessionDBData);
    
    pProfessionDBData->byCount = (BYTE)GetProfessionCount();
    
    if (pProfessionDBData->byCount == 0)
    {
        goto Exit1;
    }
    
    KGLOG_PROCESS_ERROR(uLeftSize >= pProfessionDBData->byCount * sizeof(KProfessionItem));

    for (int i = 0; i < MAX_PROFESSION_COUNT; ++i)
    {
        pProfession = &m_PlayerProfessionList[i];

        if (!pProfession->m_bIsLearnd)
        {
            continue;
        }
        
        pProfessionItem = &(pProfessionDBData->Profession[nSaveProfessionIndex++]);

        pProfessionItem->m_byProfessionID   = (BYTE)(i + 1);
		pProfessionItem->m_byCurrentLevel   = (BYTE)pProfession->m_dwCurrentLevel;
		pProfessionItem->m_wProficiency     = (WORD)pProfession->m_dwCurrentProficiency;
		pProfessionItem->m_byMaxLevel       = (BYTE)pProfession->m_dwMaxLevel;
		pProfessionItem->m_byBranchID       = (BYTE)pProfession->m_dwBranchID;
    }

    uLeftSize -= pProfessionDBData->byCount * sizeof(KProfessionItem);

Exit1:
    *puUseSize = uBufferSize - uLeftSize;

    bResult = true;
Exit0:
    return bResult;
}
#endif

BOOL KProfessionList::CanLearnProfession(DWORD dwProfessionID)
{
	BOOL bResult    = false;
    BOOL bRetCode   = false;

	KG_PROCESS_ERROR(dwProfessionID > 0);
	KGLOG_PROCESS_ERROR(dwProfessionID <= MAX_PROFESSION_COUNT);
	KGLOG_PROCESS_ERROR(m_pPlayer);

	bRetCode = g_pSO3World->m_ProfessionManager.IsProfessionExist(dwProfessionID);
	KGLOG_PROCESS_ERROR(bRetCode);

	KG_PROCESS_ERROR(!(m_PlayerProfessionList[dwProfessionID - 1].m_bIsLearnd));

	bResult = true;
Exit0:
    return bResult;
}

BOOL KProfessionList::LearnProfession(DWORD dwProfessionID)
{
    BOOL bResult    = false;
	BOOL bRetCode   = false;

	KG_PROCESS_ERROR(dwProfessionID > 0);

	bRetCode = CanLearnProfession(dwProfessionID);
	KG_PROCESS_ERROR(bRetCode);

	m_PlayerProfessionList[dwProfessionID - 1].m_bIsLearnd            = true;
	m_PlayerProfessionList[dwProfessionID - 1].m_dwCurrentLevel       = INIT_PROFESSION_LEVEL;
	m_PlayerProfessionList[dwProfessionID - 1].m_dwCurrentProficiency = 0;
	m_PlayerProfessionList[dwProfessionID - 1].m_dwMaxLevel           = INIT_PROFESSION_MAX_LEVEL;
	m_PlayerProfessionList[dwProfessionID - 1].m_dwBranchID           = 0;

	bResult = true;
Exit0:
    return bResult;
}

BOOL KProfessionList::ForgetProfession(DWORD dwProfessionID)
{
    BOOL bResult    = false;
	BOOL bRetCode   = false;

	KG_PROCESS_ERROR(dwProfessionID > 0);
	KGLOG_PROCESS_ERROR(dwProfessionID <= MAX_PROFESSION_COUNT);
	KGLOG_PROCESS_ERROR(m_pPlayer);

	KG_PROCESS_ERROR(m_PlayerProfessionList[dwProfessionID - 1].m_bIsLearnd);

	m_PlayerProfessionList[dwProfessionID - 1].m_bIsLearnd              = false;
	m_PlayerProfessionList[dwProfessionID - 1].m_dwCurrentLevel         = 0;
	m_PlayerProfessionList[dwProfessionID - 1].m_dwCurrentProficiency   = 0;
	m_PlayerProfessionList[dwProfessionID - 1].m_dwMaxLevel             = 0;

	//同时遗忘相关的配方
	bRetCode = m_pPlayer->ForgetRecipe(dwProfessionID);
	KGLOG_PROCESS_ERROR(bRetCode);
	
	bResult = true;
Exit0:
    return bResult;
}

DWORD KProfessionList::GetProfessionLevel(DWORD dwProfessionID)
{
	KG_PROCESS_ERROR(dwProfessionID > 0);
	KGLOG_PROCESS_ERROR(dwProfessionID <= MAX_PROFESSION_COUNT);

	KG_PROCESS_ERROR(m_PlayerProfessionList[dwProfessionID - 1].m_bIsLearnd);

	return m_PlayerProfessionList[dwProfessionID - 1].m_dwCurrentLevel;
Exit0:
	return 0;
}

int KProfessionList::GetProfessionAdjustLevel(DWORD dwProfessionID)
{
    int nResult = 0;

    KG_PROCESS_ERROR(dwProfessionID > 0);
    KG_PROCESS_ERROR(dwProfessionID <= MAX_PROFESSION_COUNT);

    KG_PROCESS_ERROR(m_PlayerProfessionList[dwProfessionID - 1].m_bIsLearnd);

    nResult = m_PlayerProfessionList[dwProfessionID - 1].m_nAdjustLevel;
Exit0:
    return nResult;
}

DWORD KProfessionList::GetProfessionMaxLevel(DWORD dwProfessionID)
{
	KG_PROCESS_ERROR(dwProfessionID > 0);
	KGLOG_PROCESS_ERROR(dwProfessionID <= MAX_PROFESSION_COUNT);

	KG_PROCESS_ERROR(m_PlayerProfessionList[dwProfessionID - 1].m_bIsLearnd);

	return m_PlayerProfessionList[dwProfessionID - 1].m_dwMaxLevel;
Exit0:
	return 0;
}

int	KProfessionList::GetProficiency(DWORD dwProfessionID)
{
	KG_PROCESS_ERROR(dwProfessionID > 0);
	KGLOG_PROCESS_ERROR(dwProfessionID <= MAX_PROFESSION_COUNT);
	KG_PROCESS_ERROR(m_PlayerProfessionList[dwProfessionID - 1].m_bIsLearnd);

	return m_PlayerProfessionList[dwProfessionID - 1].m_dwCurrentProficiency;
Exit0:
	return -1;
}

DWORD KProfessionList::GetBranchID(DWORD dwProfessionID)
{
	KG_PROCESS_ERROR(dwProfessionID > 0);
	KGLOG_PROCESS_ERROR(dwProfessionID <= MAX_PROFESSION_COUNT);
	KG_PROCESS_ERROR(m_PlayerProfessionList[dwProfessionID - 1].m_bIsLearnd);

	return m_PlayerProfessionList[dwProfessionID - 1].m_dwBranchID;
Exit0:
	return 0;
}

BOOL KProfessionList::IsProfessionLearned(DWORD dwProfessionID)
{
    BOOL  bResult = false;

    KGLOG_PROCESS_ERROR(dwProfessionID > 0 && dwProfessionID <= MAX_PROFESSION_COUNT);
    KG_PROCESS_ERROR(m_PlayerProfessionList[dwProfessionID - 1].m_bIsLearnd);
        
    bResult = true;
Exit0:
    return bResult;
}

DWORD KProfessionList::GetProfessionCount(void)
{
	DWORD dwCount = 0;
	int nIndex = 0;

	for (nIndex = 0; nIndex < MAX_PROFESSION_COUNT; nIndex++)
	{
		if (m_PlayerProfessionList[nIndex].m_bIsLearnd)
			dwCount++;
	}

	return dwCount;
}

BOOL KProfessionList::AddProfessionProficiency(DWORD dwProfessionID, DWORD dwExp)
{
    BOOL                nResult                  = false;
	DWORD               dwProficiencyAtThisLevel = 0;
	DWORD               dwLevel                  = 0;
    KPlayerProfession*  pPlayerProfession        = NULL;
    
	KGLOG_PROCESS_ERROR(dwProfessionID > 0 && dwProfessionID <= MAX_PROFESSION_COUNT);
    
    KG_PROCESS_ERROR(m_PlayerProfessionList[dwProfessionID - 1].m_bIsLearnd);

	pPlayerProfession = &m_PlayerProfessionList[dwProfessionID - 1];

    // 已经达到最大等级
    KG_PROCESS_ERROR(pPlayerProfession->m_dwCurrentLevel < pPlayerProfession->m_dwMaxLevel);

	dwLevel = pPlayerProfession->m_dwCurrentLevel;
	KGLOG_PROCESS_ERROR(dwLevel > 0);

	dwProficiencyAtThisLevel = g_pSO3World->m_ProfessionManager.GetLevelProficiency(dwProfessionID, dwLevel);
	KG_PROCESS_ERROR(dwProficiencyAtThisLevel > 0);
	
	pPlayerProfession->m_dwCurrentProficiency += dwExp;
	while (
        pPlayerProfession->m_dwCurrentLevel < pPlayerProfession->m_dwMaxLevel && 
		pPlayerProfession->m_dwCurrentProficiency >= dwProficiencyAtThisLevel
    )
	{
		pPlayerProfession->m_dwCurrentProficiency -= dwProficiencyAtThisLevel;
		dwLevel = ++(pPlayerProfession->m_dwCurrentLevel);
		dwProficiencyAtThisLevel = g_pSO3World->m_ProfessionManager.GetLevelProficiency(dwProfessionID, dwLevel);
#ifdef _SERVER
        g_pSO3World->m_StatDataServer.UpdateLearnProfessionLevelState(dwProfessionID, dwLevel);
#endif
	}
	
    assert(pPlayerProfession->m_dwCurrentLevel <= pPlayerProfession->m_dwMaxLevel);

    if (pPlayerProfession->m_dwCurrentLevel == pPlayerProfession->m_dwMaxLevel)
	{
		pPlayerProfession->m_dwCurrentProficiency = 0;
	}
    
	nResult = true;
Exit0:
	return nResult;
}

BOOL KProfessionList::SetAdjustLevel(DWORD dwProfessionID, int nAdjustLevel)
{
    BOOL bResult    = false;

    KG_PROCESS_ERROR(dwProfessionID > 0);
    KG_PROCESS_ERROR(dwProfessionID <= MAX_PROFESSION_COUNT);
    KG_PROCESS_ERROR(m_pPlayer);

    KG_PROCESS_ERROR(m_PlayerProfessionList[dwProfessionID - 1].m_bIsLearnd);

    m_PlayerProfessionList[dwProfessionID - 1].m_nAdjustLevel = nAdjustLevel;

    bResult = true;
Exit0:
    return bResult;
}

BOOL KProfessionList::SetMaxLevel(DWORD dwProfessionID, DWORD dwMaxLevel)
{
    BOOL bResult    = false;

	KG_PROCESS_ERROR(dwProfessionID > 0);
	KGLOG_PROCESS_ERROR(dwProfessionID <= MAX_PROFESSION_COUNT);
	KGLOG_PROCESS_ERROR(dwMaxLevel > 0 && dwMaxLevel <= PROFESSION_MAX_LEVEL);
	KGLOG_PROCESS_ERROR(m_pPlayer);

	KG_PROCESS_ERROR(m_PlayerProfessionList[dwProfessionID - 1].m_bIsLearnd);

	m_PlayerProfessionList[dwProfessionID - 1].m_dwMaxLevel = dwMaxLevel;

	bResult = true;
Exit0:
    return bResult;
}

BOOL KProfessionList::LearnBranch(DWORD dwProfessionID, DWORD dwBranchID)
{
    BOOL            bResult     = false;
	KProfession*    pProfession = NULL;

	KG_PROCESS_ERROR(dwProfessionID > 0);
	KGLOG_PROCESS_ERROR(dwProfessionID <= MAX_PROFESSION_COUNT);
	KGLOG_PROCESS_ERROR(dwBranchID > 0 && dwBranchID <= PROFESSION_BRANCH_COUNT);
	KGLOG_PROCESS_ERROR(m_pPlayer);

	pProfession = g_pSO3World->m_ProfessionManager.GetProfession(dwProfessionID);
	KGLOG_PROCESS_ERROR(pProfession);									// 生活技能存在
	KGLOG_PROCESS_ERROR(pProfession->m_bBranchUsed[dwBranchID - 1]);		// 生活技能分支存在

	KG_PROCESS_ERROR(m_PlayerProfessionList[dwProfessionID - 1].m_bIsLearnd);

	m_PlayerProfessionList[dwProfessionID - 1].m_dwBranchID = dwBranchID;
	
	bResult = true;
Exit0:
    return bResult;
}

BOOL KProfessionList::UpdatePlayerProfession(
    DWORD dwProfessionID, DWORD dwMaxLevel, DWORD dwCurrentLevel, 
    DWORD dwCurrentProficiency, DWORD dwBranchID
)
{
    BOOL              bResult  = false;

	KG_PROCESS_ERROR(dwProfessionID > 0);
	KGLOG_PROCESS_ERROR(dwProfessionID <= MAX_PROFESSION_COUNT);
    
    m_PlayerProfessionList[dwProfessionID - 1].m_bIsLearnd              = true;
	m_PlayerProfessionList[dwProfessionID - 1].m_dwMaxLevel			    = dwMaxLevel;
	m_PlayerProfessionList[dwProfessionID - 1].m_dwCurrentLevel		    = dwCurrentLevel;
	m_PlayerProfessionList[dwProfessionID - 1].m_dwCurrentProficiency   = dwCurrentProficiency;
	m_PlayerProfessionList[dwProfessionID - 1].m_dwBranchID			    = dwBranchID;

	bResult = true;
Exit0:
	return bResult;
}

BOOL KProfessionList::GetProfessionList(std::vector<DWORD>& vecProfessionIDList)
{
	vecProfessionIDList.clear();

	for (int nIndex = 0; nIndex < MAX_PROFESSION_COUNT; nIndex++)
	{
		if (m_PlayerProfessionList[nIndex].m_bIsLearnd)
		{
			vecProfessionIDList.push_back(nIndex + 1);
		}
	}

	return true;
}

