#ifndef _KSCRIPT_TIMER_LIST_
#define _KSCRIPT_TIMER_LIST_
#include "KSO3World.h"
#include "Global.h"
#include "Engine/KMemory.h"

#ifdef _SERVER

struct KScriptTimer
{
	int     nEndFrame;
	int     nStopFrame;
	int     nParam1;
	int     nParam2;
	DWORD   dwScriptID;
    BOOL    bRemoved;
};

template<class T, int C>
class KScriptTimerList
{
public:
	BOOL Init(T* pOwner);
	void UnInit();

	void Activate();

	DWORD SetTimer(int nTime, const char* szScriptName, int nParam1, int nParam2);
	BOOL StopTimer(DWORD dwTimerID);
	BOOL RestartTimer(DWORD dwTimerID);
	BOOL RemoveTimer(DWORD dwTimerID);

private:
    typedef KMemory::KAllocator<std::pair<DWORD, KScriptTimer> > KTIMER_MAP_ALLOCATOR;
    typedef std::map<DWORD, KScriptTimer, less<DWORD>, KTIMER_MAP_ALLOCATOR> KSCRIPT_TIMMER_MAP;
    KSCRIPT_TIMMER_MAP m_ScriptTimerMap;

	DWORD   m_dwScriptTimmerID;
	T*      m_pOwner;
};

template<class T, int C>
BOOL KScriptTimerList<T, C>::Init(T* pOwner)
{
	assert(pOwner);

	m_pOwner            = pOwner;
    m_dwScriptTimmerID  = 0;

	return true;
}

template<class T, int C>
void KScriptTimerList<T, C>::UnInit()
{	
	m_pOwner = NULL;
    m_ScriptTimerMap.clear();

	return;
}

template<class T, int C>
void KScriptTimerList<T, C>::Activate()
{
	BOOL                         bRetCode	= false;
	int                          nTopIndex	= 0;
    KSCRIPT_TIMMER_MAP::iterator it;

	assert(m_pOwner);

    it = m_ScriptTimerMap.begin();

	while (it != m_ScriptTimerMap.end())
	{
		KScriptTimer* pScriptTimer = &it->second;

        if (pScriptTimer->bRemoved)
        {
            m_ScriptTimerMap.erase(it++);
            continue;
        }

		if (pScriptTimer->nStopFrame == 0 && g_pSO3World->m_nGameLoop > pScriptTimer->nEndFrame)
		{
			bRetCode = g_pSO3World->m_ScriptCenter.IsScriptExist(pScriptTimer->dwScriptID);
			if (bRetCode && g_pSO3World->m_ScriptCenter.IsFuncExist(pScriptTimer->dwScriptID, SCRIPT_ON_TIMER))
			{
				g_pSO3World->m_ScriptCenter.SafeCallBegin(&nTopIndex);

				g_pSO3World->m_ScriptCenter.PushValueToStack(m_pOwner);
				g_pSO3World->m_ScriptCenter.PushValueToStack(pScriptTimer->nParam1);
				g_pSO3World->m_ScriptCenter.PushValueToStack(pScriptTimer->nParam2);

				g_pSO3World->m_ScriptCenter.CallFunction(pScriptTimer->dwScriptID, SCRIPT_ON_TIMER, 0);
				g_pSO3World->m_ScriptCenter.SafeCallEnd(nTopIndex);
			}
            
            m_ScriptTimerMap.erase(it++);
            continue;
		}
        
        ++it;
	}

Exit0:
	return;
}

template<class T, int C>
DWORD KScriptTimerList<T, C>::SetTimer(int nTime, const char* szScriptName, int nParam1, int nParam2)
{
	DWORD           dwResult    = 0;
	int             dwTimmerID  = 0;
	KScriptTimer*   pNewTimer   = NULL;
    std::pair<KSCRIPT_TIMMER_MAP::iterator, bool> InsRet;

	assert(m_pOwner);
    assert(szScriptName);
    KGLOG_PROCESS_ERROR(m_ScriptTimerMap.size() < C);
    
    dwTimmerID = ++m_dwScriptTimmerID;

    InsRet = m_ScriptTimerMap.insert(make_pair(dwTimmerID, KScriptTimer()));
    KGLOG_PROCESS_ERROR(InsRet.second);

    pNewTimer = &InsRet.first->second;
	
    pNewTimer->nEndFrame  = g_pSO3World->m_nGameLoop + nTime;
	pNewTimer->nStopFrame = 0;
	pNewTimer->nParam1    = nParam1;
	pNewTimer->nParam2    = nParam2;
	pNewTimer->dwScriptID = g_FileNameHash(szScriptName);
    pNewTimer->bRemoved   = false;
    
	dwResult = dwTimmerID;
Exit0:
	return dwResult;
}

template<class T, int C>
BOOL KScriptTimerList<T, C>::StopTimer(DWORD dwTimerID)
{
	BOOL            bResult         = false;
    KScriptTimer*   pScriptTimer    = NULL;
    KSCRIPT_TIMMER_MAP::iterator it;

	assert(m_pOwner);
    
    it = m_ScriptTimerMap.find(dwTimerID);
    KGLOG_PROCESS_ERROR(it != m_ScriptTimerMap.end());

    pScriptTimer = &it->second;

	if (pScriptTimer->nStopFrame == 0)
	{
		pScriptTimer->nStopFrame = g_pSO3World->m_nGameLoop;
	}

	bResult = true;
Exit0:
	return bResult;
}

template<class T, int C>
BOOL KScriptTimerList<T, C>::RestartTimer(DWORD dwTimerID)
{	
	BOOL            bResult         = false;
    KScriptTimer*   pScriptTimer    = NULL;
    KSCRIPT_TIMMER_MAP::iterator it;
    
    it = m_ScriptTimerMap.find(dwTimerID);
    KGLOG_PROCESS_ERROR(it != m_ScriptTimerMap.end());
    
    pScriptTimer = &it->second;

	if (pScriptTimer->nStopFrame != 0)
	{
		pScriptTimer->nEndFrame    += g_pSO3World->m_nGameLoop - pScriptTimer->nStopFrame;
		pScriptTimer->nStopFrame   = 0;
	}

	bResult = true;
Exit0:
	return bResult;
}

template<class T, int C>
BOOL KScriptTimerList<T, C>::RemoveTimer(DWORD dwTimerID)
{
    BOOL            bResult         = false;
    KSCRIPT_TIMMER_MAP::iterator it;
    
    it = m_ScriptTimerMap.find(dwTimerID);
    KG_PROCESS_ERROR(it != m_ScriptTimerMap.end());
    
    it->second.bRemoved = true;

    bResult = true;
Exit0:
    return bResult;
}
#endif
#endif //_KSCRIPT_TIMER_LIST_
