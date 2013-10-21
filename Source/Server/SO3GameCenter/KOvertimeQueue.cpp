#include "stdafx.h"
#include "KOvertimeQueue.h"
#include <time.h>

KOvertimeQueue::KOvertimeQueue(void)
{
	m_OnEvent = NULL;
}

KOvertimeQueue::~KOvertimeQueue(void)
{
	m_EventList.clear();
	m_EventIDMap.clear();
}

BOOL KOvertimeQueue::Init(EVENT_FUNC pEventFunc)
{
	KGLOG_PROCESS_ERROR(pEventFunc);
		
	m_OnEvent = pEventFunc;

	return TRUE;

Exit0:

	return FALSE;
}

void KOvertimeQueue::Breathe(void)
{
    time_t  timeCurrent  = 0;
    EVENT*  pEvent       = NULL;

    KG_PROCESS_ERROR(!m_EventList.empty());

    time(&timeCurrent);
    pEvent = &(m_EventList.front());
    assert(pEvent);

	if (pEvent->m_nOvertime < timeCurrent)
	{
		KGLOG_PROCESS_ERROR(m_OnEvent);
		m_OnEvent(pEvent->m_nEventID, pEvent->m_dwData);

		EVENT_ID_MAP::iterator it = m_EventIDMap.find(pEvent->m_nEventID);
		m_EventIDMap.erase(it);
		m_EventList.pop_front();
        pEvent = NULL;
	}

Exit0:
	return;
}

BOOL KOvertimeQueue::AddEvent(int nOvertime, int64_t nEventID, DWORD dwData)
{
	time_t	timeCurrent;
	EVENT_LIST::iterator it;
	std::pair<EVENT_ID_MAP::iterator, BOOL> InsRet;
	EVENT NewEvent;

	InsRet.second = TRUE;
	time(&timeCurrent);
	
	NewEvent.m_nEventID = nEventID;
	NewEvent.m_nOvertime = timeCurrent + nOvertime;
	NewEvent.m_dwData = dwData;

	it = m_EventList.begin();
	while (it != m_EventList.end() && NewEvent.m_nOvertime > it->m_nOvertime)
		++it;

	it = m_EventList.insert(it, NewEvent);
	KGLOG_PROCESS_ERROR(it != m_EventList.end());

	InsRet = m_EventIDMap.insert(std::make_pair(NewEvent.m_nEventID, it));
	KGLOG_PROCESS_ERROR(InsRet.second);

	return TRUE;

Exit0:

	if (!InsRet.second)
	{
		m_EventList.erase(it);
	}

	return FALSE;
}

BOOL KOvertimeQueue::DelEvent(int64_t nEventID)
{
	EVENT_ID_MAP::iterator it = m_EventIDMap.find(nEventID);	
	KG_PROCESS_ERROR(it != m_EventIDMap.end());

	m_EventList.erase(it->second);
	m_EventIDMap.erase(it);

	return TRUE;

Exit0:

	return FALSE;
}

//检测事件是否存在
BOOL KOvertimeQueue::CheckEvent(int64_t nEventID)
{
	EVENT_ID_MAP::iterator it = m_EventIDMap.find(nEventID);	
	KG_PROCESS_ERROR(it != m_EventIDMap.end());

	return TRUE;

Exit0:

	return FALSE;

}
