#include "stdafx.h"
#include "KCDTimerList.h"
#include "KSO3World.h"
#include "Global.h"
#include "KPlayer.h"
#include "KPlayerServer.h"

KCDTimerList::KCDTimerList(void)
{
    m_pOwner = NULL;
}

KCDTimerList::~KCDTimerList(void)
{
    assert(m_pOwner == NULL);
}

BOOL KCDTimerList::Init(KPlayer* pOwner)
{
    BOOL bResult = false;

    KGLOG_PROCESS_ERROR(pOwner);

    m_pOwner = pOwner;
    m_TimerList.clear();

    bResult = true;
Exit0:
    return bResult;
}

void KCDTimerList::UnInit()
{
    m_TimerList.clear();
    m_pOwner = NULL;
}

#ifdef _SERVER
BOOL KCDTimerList::Load(BYTE* pbyData, size_t uDataLen)
{
    BOOL                                    bResult             = false;
    COOLDOWN_LIST_DB_DATA*                  pCoolDownListDBData = NULL;
    size_t                                  uLeftSize           = uDataLen;
    BYTE*                                   pbyOffset           = pbyData;
	std::pair<TIMER_LIST::iterator, bool>   InsRet;

	// 清除原有数据
    m_TimerList.clear();

    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(COOLDOWN_LIST_DB_DATA));
    pCoolDownListDBData = (COOLDOWN_LIST_DB_DATA*)pbyOffset;
    uLeftSize -= sizeof(COOLDOWN_LIST_DB_DATA);
    pbyOffset += sizeof(COOLDOWN_LIST_DB_DATA);

    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(COOLDOWN_LIST_DB_DATA::COOLDOWN_DB_DATA) * pCoolDownListDBData->wCount);
    uLeftSize -= sizeof(COOLDOWN_LIST_DB_DATA::COOLDOWN_DB_DATA) * pCoolDownListDBData->wCount;
    pbyOffset += sizeof(COOLDOWN_LIST_DB_DATA::COOLDOWN_DB_DATA) * pCoolDownListDBData->wCount;

	// 读取计时器表
    for (int nIndex = 0; nIndex < pCoolDownListDBData->wCount; nIndex++)
    {
        KTIMER_DATA TimerData;

        if (pCoolDownListDBData->CoolDown[nIndex].nTime <= g_pSO3World->m_nCurrentTime)
        {
            continue;
        }

        TimerData.nEndFrame = g_pSO3World->m_nGameLoop +
            (int)(pCoolDownListDBData->CoolDown[nIndex].nTime - g_pSO3World->m_nCurrentTime) * GAME_FPS;

        TimerData.nInterval = pCoolDownListDBData->CoolDown[nIndex].nInterval;

        InsRet = m_TimerList.insert(
            std::make_pair(pCoolDownListDBData->CoolDown[nIndex].wTimerID, TimerData)
        );
        KGLOG_PROCESS_ERROR(InsRet.second);

        g_PlayerServer.DoResetCooldown(
            m_pOwner->m_nConnIndex, pCoolDownListDBData->CoolDown[nIndex].wTimerID, 
            TimerData.nEndFrame,    TimerData.nInterval
        );
    }

    KGLOG_PROCESS_ERROR(uLeftSize == 0);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KCDTimerList::Save(size_t* puUsedSize, BYTE* pbyBuffer, size_t uBufferSize)
{
    BOOL                    bResult             = false;
    COOLDOWN_LIST_DB_DATA*  pCoolDownListDBData = NULL;
    size_t                  uLeftSize           = uBufferSize;
    BYTE*                   pbyOffset           = pbyBuffer;
    int                     nCount              = 0;

    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(COOLDOWN_LIST_DB_DATA));
    pCoolDownListDBData = (COOLDOWN_LIST_DB_DATA*)pbyOffset;
    uLeftSize -= sizeof(COOLDOWN_LIST_DB_DATA);
    pbyOffset += sizeof(COOLDOWN_LIST_DB_DATA);

    for (TIMER_LIST::iterator it = m_TimerList.begin(); it != m_TimerList.end(); ++it)
    {
        if (it->second.nEndFrame <= g_pSO3World->m_nGameLoop + LOGOUT_WAITING_TIME)
        {
            continue;
        }

        KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(COOLDOWN_LIST_DB_DATA::COOLDOWN_DB_DATA));
        uLeftSize -= sizeof(COOLDOWN_LIST_DB_DATA::COOLDOWN_DB_DATA);
        pbyOffset += sizeof(COOLDOWN_LIST_DB_DATA::COOLDOWN_DB_DATA);

        pCoolDownListDBData->CoolDown[nCount].wTimerID = (WORD)it->first;
        pCoolDownListDBData->CoolDown[nCount].nTime = (int)(
            (it->second.nEndFrame - g_pSO3World->m_nGameLoop) / GAME_FPS + g_pSO3World->m_nCurrentTime
        );
        pCoolDownListDBData->CoolDown[nCount].nInterval = (it->second).nInterval;

        nCount++;
    }
    
    KGLOG_CHECK_ERROR(nCount < MAX_COOL_DOWN_COUNT);

    pCoolDownListDBData->wCount = (WORD)nCount;

    *puUsedSize = uBufferSize - uLeftSize;

    bResult = true;
Exit0:
    return bResult;
}
#endif

#ifdef _CLIENT
void KCDTimerList::SetTimer(DWORD dwTimerID, int nInterval, int nEndFrame)
{
	KTIMER_DATA TimerData;

	TimerData.nEndFrame = nEndFrame;
	TimerData.nInterval = nInterval;

	m_TimerList[dwTimerID] = TimerData;

	return;
}
#endif

#ifdef _SERVER
void KCDTimerList::ResetTimer(DWORD dwTimerID, int nInterval)
{
	KTIMER_DATA TimerData;

	TimerData.nEndFrame = g_pSO3World->m_nGameLoop + nInterval;
	TimerData.nInterval = nInterval;
    
    g_PlayerServer.DoResetCooldown(m_pOwner->m_nConnIndex, dwTimerID, TimerData.nEndFrame, TimerData.nInterval);

	m_TimerList[dwTimerID] = TimerData;

Exit0:
	return;
}
#endif

BOOL KCDTimerList::CheckTimer(DWORD dwTimerID)
{
    BOOL                    bResult = false;
	TIMER_LIST::iterator    it      = m_TimerList.find(dwTimerID);
	
	KG_PROCESS_SUCCESS(it == m_TimerList.end());
    
    KG_PROCESS_ERROR(g_pSO3World->m_nGameLoop >= it->second.nEndFrame);

    m_TimerList.erase(it);

Exit1:
    bResult = true;
Exit0:
    return bResult;
}

void KCDTimerList::ClearTimer(DWORD dwTimerID)
{
	TIMER_LIST::iterator it = m_TimerList.find(dwTimerID);
	
    KG_PROCESS_ERROR(it != m_TimerList.end());

	m_TimerList.erase(it);

#ifdef _SERVER
    g_PlayerServer.DoResetCooldown(m_pOwner->m_nConnIndex, dwTimerID, -1, 0); // 给客户端同步一个nEndFrame为-1的值，用来Clear客户端的CD
#endif

Exit0:
	return;
}

BOOL KCDTimerList::GetTimerData(DWORD dwTimerID, int& rnInterval, int& rnEndFrame)
{
    BOOL                    bResult = false;
	TIMER_LIST::iterator    it      = m_TimerList.find(dwTimerID);

	KG_PROCESS_ERROR(it != m_TimerList.end());

	rnInterval = it->second.nInterval;
	rnEndFrame = it->second.nEndFrame;

    bResult = true;
Exit0:
	return bResult;
}
