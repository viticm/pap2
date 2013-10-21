/*******************************************************************************
File        : UiDiceWeaveBingo.h
Creator     : Fyt(Fan Zhanpeng)
create data : 12-12-2003(mm-dd-yyyy)
Description : 色子老虎机，中国人在宋朝的伟大发明！！！(至少剑网里是这样哈哈哈)
********************************************************************************/

#if !defined(AFX_UIDICEWEAVEBINGO_H__9204EC3A_3A24_4298_A6B4_46A8FED47C86__INCLUDED_)
#define AFX_UIDICEWEAVEBINGO_H__9204EC3A_3A24_4298_A6B4_46A8FED47C86__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../elem/wndpuretextbtn.h"
#include "../elem/wndbutton.h"
#include "../elem/wndimage.h"
#include "../elem/wndtext.h"


class KMyWndImage : public KWndImage
{
public:
	virtual int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
};


class KUiDiceWeaveBingo : public KWndImage
{
public:
	KUiDiceWeaveBingo();
	virtual ~KUiDiceWeaveBingo();

	static        KUiDiceWeaveBingo* OpenWindow();      //打开窗口
	static        KUiDiceWeaveBingo* GetIfVisible();    //如果窗口正被显示，则返回实例指针
	static void   CloseWindow(bool bDestory = TRUE);	//关闭窗口，同时可以选则是否删除对象实例
	static void   LoadScheme(const char* pScheme);		//载入界面方案

	void			UpdateUI(int nBingoMoney);	//更新界面的显示

	//一个色子结果进来了
	void			DiceNumberIncome(int nDice, int nNumber);

	//获奖结果进来了
	void			ResultIncome(int nResultStatus, char *szString = NULL);

public:
	enum enumDICE_WEAVER_BINGO_STATUS					//界面所处的状态
	{
		enumDWBS_INITIALIZE,		//初始化
		enumDWBS_WAITING_BET,		//等待下注
		enumDWBS_BETTING,			//正在赌博
		enumDWBS_FINISH_ONCE,		//完成咯~播放特效和设置数据
		enumDWBS_WIN,				//赢钱了
		enumDWBS_LOSE,				//输钱了
		enumDWBS_TRY_AGAIN/*?*/,	//询问是否再来一次
	};

private:
	static        KUiDiceWeaveBingo *ms_pSelf;

private:
	void			Initialize();
	virtual int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	virtual void	Breathe();

	void			RequestBingoMoney();		//向服务器要求大奖累积奖金

	//响应色子停止动作
	void			OnStopDice(KMyWndImage* Dice);

	int				PlayEffect();				//播放中奖效果
	int				PlayDiceRolling();			//播放色子转动
	
	int				InitializeGame();			//初始化游戏

	int				OnBet();					//响应下注

private: /*Controls*/
	KWndText32		m_TotleBingoMoney;
	KWndText32		m_PlayerMoney;

	KWndText256		m_TextTip;
	KMyWndImage		m_Dice[3];
	KWndButton		m_AgainAndBet;
	KWndButton		m_Leave;

private:
	unsigned int	m_uColor;
	unsigned int	m_uWinkColor;

	int				m_aryStopTime[3];
	int				m_nStatus;

	int				m_nbIsBingoMoneyIncome;	//是否已经获取大奖奖金的数目
	int				m_nbIsResultIncome;		//是否结果已经进来了

	int				m_aryDiceResult[3];		//三个色子的结果
	int				m_nResultStatus;		//获奖情况

	int				m_nTextFontSize;		//提示文字的大小

	char			m_szPay[256];			//提示下注
	char			m_szNotEnoughMoney[256];//提示金钱不够
	char			m_szBet[256];			//提示按停止
	char			m_szLose[256];			//提示输了
	char			m_szEarnSome[256];		//提示赢了一点儿
	char			m_szEarnMore[256];		//提示赢多了一点
	char			m_szBingo[256];			//提示赢大奖
	char			m_szAgain[256];			//询问是否再来一次

	//外部进来的结果字串
	char			m_szComingResultString[256];
	int				m_nPlayEffectTime;

	//记录状态句柄
	int				m_nStatusHandle;
};

#endif // !defined(AFX_UIDICEWEAVEBINGO_H__9204EC3A_3A24_4298_A6B4_46A8FED47C86__INCLUDED_)
