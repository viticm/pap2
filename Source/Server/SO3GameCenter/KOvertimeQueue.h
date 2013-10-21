/************************************************************************/
/* 超时队列									                            */
/* Copyright : Kingsoft 2004										    */
/* Author	 : Zhu Jianqiu												*/
/* History	 :															*/
/*		2004.11.19	Create												*/
/************************************************************************/
#ifndef _KOVERTIME_QUEUE_
#define _KOVERTIME_QUEUE_

#include <list>
#include <map>

typedef void (*EVENT_FUNC)(int64_t nEventID, DWORD dwData);

class KOvertimeQueue
{
public:
	KOvertimeQueue(void);
	virtual ~KOvertimeQueue(void);

	BOOL Init(EVENT_FUNC pEventFunc);
	void Breathe(void);

	BOOL AddEvent(int nOvertime, int64_t nEventID, DWORD dwData);
	BOOL DelEvent(int64_t nEventID);
	//检测事件是否存在
	BOOL CheckEvent(int64_t nEventID);

private:
	typedef struct _EVENT
	{
		time_t	m_nOvertime;
		int64_t m_nEventID;
		DWORD	m_dwData;
	}EVENT;

	typedef std::list<EVENT> EVENT_LIST;
	EVENT_LIST	m_EventList;
	typedef std::map<int64_t, EVENT_LIST::iterator> EVENT_ID_MAP;
	EVENT_ID_MAP	m_EventIDMap;
	EVENT_FUNC m_OnEvent;
};
#endif	//_KOVERTIME_QUEUE_
