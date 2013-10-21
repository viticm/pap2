/************************************************************************/
/* 玩家Cooldown计时器表						                            */
/* Copyright : Kingsoft 2005										    */
/* Author	 : Zhu Jianqiu												*/
/* History	 :															*/
/*		2005.11.03	Create												*/
/*		2006.05.15	Modified interval precision from second to frame	*/
/************************************************************************/
#ifndef _KCDTIMER_LIST_H_
#define _KCDTIMER_LIST_H_

class KPlayer;

class KCDTimerList
{
	EXPORT_TEST(KCDTimerList);
public:
	KCDTimerList(void);
	~KCDTimerList(void);

	char* Load(char* pData);
	char* Save(char* pData);

	//重置计时器，延时为nInterval帧
	void ResetTimer(DWORD dwTimerID, int nInterval);
	//检查计时器，返回True为计时器已经计时结束，FALSE为未结束
	BOOL CheckTimer(DWORD dwTimerID);
	//清除计时器的计时
	void ClearTimer(DWORD dwTimerID);
	//获取计时时长，单位：帧
	BOOL GetTimerData(DWORD dwTimerID, int& rnInterval, int& rnEndFrame);

    KPlayer *m_pPlayer;

private:
	struct _TIMER_DATA 
	{
		int		nEndFrame;
		int		nInterval;
	};
	typedef std::map<DWORD, _TIMER_DATA> TIMER_LIST;
	TIMER_LIST	m_TimerList;
};

#endif	//_KCDTIMER_LIST_H_