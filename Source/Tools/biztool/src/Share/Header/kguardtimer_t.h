/* -------------------------------------------------------------------------
//	文件名		：	kguardtimer_t.h
//	创建者		：	wangbin
//	创建时间	：	2008-1-9 10:38:00
//	功能描述	：	进程间通信定时器模版
// -----------------------------------------------------------------------*/
#ifndef __KGUARDTIMER_T_H__
#define __KGUARDTIMER_T_H__

#include "ksdef.h"
#include <list>

template <class KType>
class KGuardTimer_T
{
public:
	typedef VOID (KType::*fnTimer)();
	struct KTIMER
	{
		fnTimer		pfnTimer;
		KType*		pTimer;
		time_t		tLastTime;
		INT			nInterval;
	};
public:
	VOID Activate(time_t tNow)
	{
		ASSERT(tNow > 0);
		typename std::list<KTIMER>::iterator it = m_listTimer.begin();
		for (; it != m_listTimer.end(); ++it)
		{
			KTIMER& sTimer = *it;
			if (tNow - sTimer.tLastTime >= sTimer.nInterval)
			{
				(sTimer.pTimer->*sTimer.pfnTimer)();
				sTimer.tLastTime = tNow;
			}
		}
	}
	// 注册定时器，时间间隔为秒
	VOID Register(KType* pTimer, fnTimer pfnTimer, INT nInterval)
	{
		ASSERT(pTimer && pfnTimer && nInterval > 0);
		KTIMER sTimer;
		sTimer.pTimer	 = pTimer;
		sTimer.pfnTimer	 = pfnTimer;
		sTimer.tLastTime = 0;
		sTimer.nInterval = nInterval;
		m_listTimer.push_back(sTimer);
	}
	VOID Clear()
	{
		m_listTimer.clear();
	};
private:
	std::list<KTIMER> m_listTimer;
};

#endif //__KGUARDTIMER_T_H__
