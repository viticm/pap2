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

#pragma pack(1)
struct COOLDOWN_LIST_DB_DATA
{
    WORD    wCount;
    struct COOLDOWN_DB_DATA
    {
        WORD    wTimerID;
        int     nTime;
        int     nInterval;
    } CoolDown[0];
};
#pragma pack()

class KCDTimerList
{
public:
	KCDTimerList(void);
	~KCDTimerList(void);

    BOOL Init(KPlayer* pOwner);
    void UnInit();

#ifdef _SERVER
    BOOL  Load(BYTE* pbyData, size_t uDataLen);
    BOOL  Save(size_t* puUsedSize, BYTE* pbyBuffer, size_t uBufferSize);
#endif

#ifdef _CLIENT
	void SetTimer(DWORD dwTimerID, int nInterval, int nEndFrame);
#endif

#ifdef _SERVER
	void ResetTimer(DWORD dwTimerID, int nInterval);
#endif

	// 检查计时器，返回true为计时器已经计时结束，false为未结束
	BOOL CheckTimer(DWORD dwTimerID);
	
    void ClearTimer(DWORD dwTimerID);
	BOOL GetTimerData(DWORD dwTimerID, int& rnInterval, int& rnEndFrame);

private:
	struct KTIMER_DATA 
	{
		int		nEndFrame; // 单位：帧
		int		nInterval; // 单位：帧
	};
	typedef std::map<DWORD, KTIMER_DATA> TIMER_LIST;
	TIMER_LIST	m_TimerList;

    KPlayer*    m_pOwner;
};

#endif	//_KCDTIMER_LIST_H_
