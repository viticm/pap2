#include "stdafx.h"
#include "KCDTimerList.h"
#include "KSO3World.h"
#include "Global.h"
#include "KPlayer.h"
#include "KMonster.h"

KCDTimerList::KCDTimerList(void)
{
    m_pPlayer = NULL;
}

KCDTimerList::~KCDTimerList(void)
{
}

char* KCDTimerList::Load(char* pData)
{
	std::pair<TIMER_LIST::iterator, bool> InsRet;

	KGLOG_PROCESS_ERROR(pData);

	//清除原有数据
	m_TimerList.clear();

	//读取计时器个数
	WORD* pwCount = (WORD*)pData;
	pData += sizeof(WORD);

	//读取计时器表
	for (int nIndex = 0; nIndex < *pwCount; nIndex++)
	{
		DWORD* pdwTimerID = (DWORD*)pData;
		pData += sizeof(DWORD);

		int* pnTime = (int*)pData;
		pData += sizeof(int);

		int* pnInterval = (int*)pData;
		pData += sizeof(int);

		if (*pnTime > g_SO3World.m_CurrentTime)
		{
			_TIMER_DATA TimerData;
			TimerData.nEndFrame = m_pPlayer->m_pOwner->m_nGameLoop + (int)(*pnTime - g_SO3World.m_CurrentTime) * GAME_FPS;
			TimerData.nInterval = *pnInterval;

			InsRet = m_TimerList.insert(std::make_pair(*pdwTimerID, TimerData));
			KGLOG_PROCESS_ERROR(InsRet.second);
		}
	}

	return pData;

Exit0:

	return NULL;
}

char* KCDTimerList::Save(char* pData)
{
	KGLOG_PROCESS_ERROR(pData);

	//写入计时器个数
	WORD* pwCount = (WORD*)pData;
	pData += sizeof(WORD);

	*pwCount = 0;
	//写入计时器表
	for (TIMER_LIST::iterator it = m_TimerList.begin();
		it != m_TimerList.end(); ++it)
	{
		if (it->second.nEndFrame > m_pPlayer->m_pOwner->m_nGameLoop + LOGOUT_WAITING_TIME)
		{
			DWORD* pdwTimerID = (DWORD*)pData;
			pData += sizeof(DWORD);

			int* pnTime = (int*)pData;
			pData += sizeof(int);

			int* pnInterval = (int*)pData;
			pData += sizeof(int);

			*pdwTimerID = it->first;
			*pnTime = (int)((it->second.nEndFrame - m_pPlayer->m_pOwner->m_nGameLoop) / GAME_FPS + 
				g_SO3World.m_CurrentTime);
			*pnInterval = it->second.nInterval;

			(*pwCount)++;
		}
	}

	return pData;

Exit0:

	return NULL;
}

void KCDTimerList::ResetTimer(DWORD dwTimerID, int nInterval)
{
	_TIMER_DATA TimerData;
	TimerData.nEndFrame = m_pPlayer->m_pOwner->m_nGameLoop + nInterval;
	TimerData.nInterval = nInterval;

	m_TimerList[dwTimerID] = TimerData;

	return;
}

BOOL KCDTimerList::CheckTimer(DWORD dwTimerID)
{
	TIMER_LIST::iterator it = m_TimerList.find(dwTimerID);
	
	if (it == m_TimerList.end())
		return TRUE;

	if (it->second.nEndFrame <= m_pPlayer->m_pOwner->m_nGameLoop)
	{
		m_TimerList.erase(it);

		return TRUE;
	}
	else
		return FALSE;
}

void KCDTimerList::ClearTimer(DWORD dwTimerID)
{
	TIMER_LIST::iterator it = m_TimerList.find(dwTimerID);
	KG_PROCESS_SUCCESS(it == m_TimerList.end());

	m_TimerList.erase(it);

Exit1:

	return;
}

//获取计时时长
BOOL KCDTimerList::GetTimerData(DWORD dwTimerID, int& rnInterval, int& rnEndFrame)
{
	TIMER_LIST::iterator it = m_TimerList.find(dwTimerID);

	if (it == m_TimerList.end())
		return FALSE;

	rnInterval = it->second.nInterval;
	rnEndFrame = it->second.nEndFrame;

	return TRUE;
}
