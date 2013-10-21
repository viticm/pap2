// -------------------------------------------------------------------------
//	文件名		：	UiReconnect.h
//	创建者		：	Freeway Chen
//	创建时间	：	2003-7-18
//	功能描述	：	自动重连的流程控制	
//
// -------------------------------------------------------------------------
#ifndef __UIRECONNECT_H__
#define __UIRECONNECT_H__

#include "..\Elem\WndWindow.h"

class KReconnectWnd : private KWndWindow
{
public:
	static bool LaunchReconnect(int nMaxTimes = 12);
	static bool	IsReconnecttingGoingOn();
	static void Exit(bool bQuitGame);

private:
	KReconnectWnd()  {}
	~KReconnectWnd() {}

	virtual int	    WndProc(unsigned int uMsg, unsigned int uParam, int nParam);//窗口函数
	virtual void	Breathe();													//窗口的持续行为
	void			FirstReconnect();
	bool			StartReconnect();

private:
	static KReconnectWnd*	m_pSelf;
	unsigned int			m_uWaitStartTime;
	unsigned int			m_uToWaitTime;
	short					m_nReconnectTimes;
	short					m_nMaxReconnectTimes;
	int						m_nHideTimes;
	bool					m_bWaitToReconnect;
	bool					m_bStop;
};

#endif // __UIRECONNECT_H__