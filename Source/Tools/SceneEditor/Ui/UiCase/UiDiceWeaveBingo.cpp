/*******************************************************************************
File        : UiDiceWeaveBingo.cpp
Creator     : Fyt(Fan Zhanpeng)
create data : 12-12-2003(mm-dd-yyyy)
Description : 色子老虎机，中国人在宋朝的伟大发明！！！(至少剑网里是这样哈哈哈)
********************************************************************************/

#include "KWin32.h"
#include "KIniFile.h"
#include "CoreShell.h"
#include "GameDataDef.h"

#include "../elem/wnds.h"
#include "../Elem/WndMessage.h"

#include "../UiBase.h"
#include "../UiSoundSetting.h"

#include "UiItem.h"
#include "UiPlayerBar.h"
#include "UiSysMsgCentre.h"
#include "UiDiceWeaveBingo.h"

#include "time.h"




#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

#define UI_INI_SERIES_DICE_BINGO "动感色子老虎机.ini"
#define MONEY_PER_BET 1000
#define EFFECT_LAST_TIME 5000


extern iCoreShell*		g_pCoreShell;

KUiDiceWeaveBingo* KUiDiceWeaveBingo::ms_pSelf = NULL;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

KUiDiceWeaveBingo::KUiDiceWeaveBingo()
{
	m_nStatusHandle = 0;
	srand(time(NULL));
}

KUiDiceWeaveBingo::~KUiDiceWeaveBingo()
{

}


/*********************************************************************
* 功能：打开窗口
**********************************************************************/
KUiDiceWeaveBingo* KUiDiceWeaveBingo::OpenWindow()
{
	if (ms_pSelf == NULL)
	{
		ms_pSelf = new KUiDiceWeaveBingo;
		if (ms_pSelf)
			ms_pSelf->Initialize();
	}
	if (ms_pSelf)
	{
		ms_pSelf->BringToTop();
		ms_pSelf->m_nStatus = enumDWBS_INITIALIZE;
		ms_pSelf->Show();
		g_UiBase.SetStatus(UIS_S_BESET, ms_pSelf->m_nStatusHandle);
		ms_pSelf->m_TextTip.SetText(ms_pSelf->m_szPay);
	}
	return ms_pSelf;
}


/*********************************************************************
* 功能：如果窗口正被显示，则返回实例指针
**********************************************************************/
KUiDiceWeaveBingo* KUiDiceWeaveBingo::GetIfVisible()
{
	if (ms_pSelf && ms_pSelf->IsVisible())
		return ms_pSelf;
	return NULL;
}


/*********************************************************************
* 功能：关闭窗口，同时可以选则是否删除对象实例
**********************************************************************/
void KUiDiceWeaveBingo::CloseWindow(bool bDestory)
{
	if (ms_pSelf)
	{
		ms_pSelf->Hide();
		g_UiBase.ReleaseStatus(ms_pSelf->m_nStatusHandle);
		if (bDestory)
		{
			ms_pSelf->Destroy();
			ms_pSelf = NULL;
		}
	}
}


/*********************************************************************
* 功能：初始化
**********************************************************************/
void KUiDiceWeaveBingo::Initialize()
{
	AddChild(&m_TotleBingoMoney);
	AddChild(&m_PlayerMoney);
	AddChild(&m_TextTip);
	AddChild(&m_Dice[0]);
	AddChild(&m_Dice[1]);
	AddChild(&m_Dice[2]);
	AddChild(&m_AgainAndBet);
	AddChild(&m_Leave);

	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);

	Wnd_AddWindow(this);
}


/*********************************************************************
* 功能：载入界面方案
**********************************************************************/
void KUiDiceWeaveBingo::LoadScheme(const char* pScheme)
{
	if(ms_pSelf)
	{
		char		Buff[128];
    	KIniFile	Ini;
    	sprintf(Buff, "%s\\%s", pScheme, UI_INI_SERIES_DICE_BINGO);
    	if (Ini.Load(Buff))
    	{
			ms_pSelf->Init(&Ini, "Main");

			ms_pSelf->m_AgainAndBet.Init(&Ini, "Bet");
			ms_pSelf->m_Leave.Init(&Ini, "Leave");
			ms_pSelf->m_TotleBingoMoney.Init(&Ini, "BingoMoney");
			ms_pSelf->m_PlayerMoney.Init(&Ini, "PlayerMoney");
			ms_pSelf->m_TextTip.Init(&Ini, "Tip");
			ms_pSelf->m_Dice[0].Init(&Ini, "Dice1");
			ms_pSelf->m_Dice[1].Init(&Ini, "Dice2");
			ms_pSelf->m_Dice[2].Init(&Ini, "Dice3");

			Ini.GetInteger("Tip", "Font", 12, &ms_pSelf->m_nTextFontSize);

			char szBuff[256];
			Ini.GetString("Tip", "Color", "0,0,0", szBuff, 16);
			ms_pSelf->m_uColor = GetColor(szBuff);
			Ini.GetString("Tip", "WinkColor", "0,0,0", szBuff, 16);
			ms_pSelf->m_uWinkColor = GetColor(szBuff);

			Ini.GetString("Tip", "Pay", "嘿嘿，这位客官可要下注吗？这可是滚滚的财源喔", ms_pSelf->m_szPay, sizeof(ms_pSelf->m_szPay));
			Ini.GetString("Tip", "NotEnoughMoney", "没钱可别进来！", ms_pSelf->m_szNotEnoughMoney, sizeof(ms_pSelf->m_szNotEnoughMoney));
			Ini.GetString("Tip", "Bet", "哟，这位客官，你可以投色子了", ms_pSelf->m_szBet, sizeof(ms_pSelf->m_szBet));
			Ini.GetString("Tip", "Lose", "嘿嘿，可惜，客官你输了！", ms_pSelf->m_szLose, sizeof(ms_pSelf->m_szLose));
			Ini.GetString("Tip", "EarnSome", "你赢了一些钱", ms_pSelf->m_szEarnSome, sizeof(ms_pSelf->m_szEarnSome));
			Ini.GetString("Tip", "EarnMore", "你赢了一笔钱", ms_pSelf->m_szEarnMore, sizeof(ms_pSelf->m_szEarnMore));
			Ini.GetString("Tip", "Bingo", "你中了累积大奖！", ms_pSelf->m_szBingo, sizeof(ms_pSelf->m_szBingo));
			Ini.GetString("Tip", "Again", "这位客官，可想再来一次？", ms_pSelf->m_szAgain, sizeof(ms_pSelf->m_szAgain));
		}
	}
}


/*********************************************************************
* 功能：载入界面方案
**********************************************************************/
int KUiDiceWeaveBingo::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int nRet = 0;
	switch(uMsg)
	{
	case WND_N_BUTTON_CLICK:
		if(uParam == (unsigned int)&m_AgainAndBet)
		{
			if(m_nStatus == enumDWBS_WAITING_BET && g_pCoreShell && m_nbIsBingoMoneyIncome)
			{
				OnBet();
				m_nStatus = enumDWBS_BETTING;
			}
			else if(m_nStatus == enumDWBS_WIN)
			{
				InitializeGame();
				OnBet();
				m_nStatus = enumDWBS_BETTING;
			}
		}
		else if(uParam >= (unsigned int)m_Dice && uParam <= (unsigned int)(m_Dice + 2))
		{
			if(m_nStatus == enumDWBS_BETTING)
			{
				OnStopDice((KMyWndImage *)uParam);
			}
		}
		else if(uParam == (unsigned int)&m_Leave)
		{
			if(g_pCoreShell)
			{
				g_pCoreShell->OperationRequest(GOI_DICE_WEAVER_CLOSE, 0, 0);
			}
			CloseWindow();
		}
		break;

	case WM_KEYDOWN:
		if(uParam == VK_SPACE)
		{
			if(!KUiPlayerBar::IsChatEditFocus())
			{
				if(m_nStatus == enumDWBS_WIN)
				{
					InitializeGame();
					OnBet();
					m_nStatus = enumDWBS_BETTING;
				}
				else if(m_nStatus == enumDWBS_WAITING_BET)
				{
					OnBet();
					m_nStatus = enumDWBS_BETTING;
				}
				else if(m_nStatus == enumDWBS_BETTING)
				{
					if(m_aryStopTime[0] < 0)
					{
						OnStopDice(m_Dice);
					}
					else if(m_aryStopTime[1] < 0)
					{
						OnStopDice(m_Dice + 1);
					}
					else if(m_aryStopTime[2] < 0)
					{
						OnStopDice(m_Dice + 2);
					}
				}
			}
		}
		break;

	default:
		return KWndWindow::WndProc(uMsg, uParam, nParam);
	}
	return nRet;
}


/*********************************************************************
* 功能：响应下注
**********************************************************************/
int	KUiDiceWeaveBingo::OnBet()
{
	g_pCoreShell->OperationRequest(GOI_BET_ON_DICE_WEAVE, 0, 0);
	m_AgainAndBet.Hide();
	m_Leave.Hide();
	m_TextTip.SetText(m_szBet);
	return 1;
}


/*********************************************************************
* 功能：初始化游戏
**********************************************************************/
int	KUiDiceWeaveBingo::InitializeGame()
{
	memset(m_aryStopTime, -1, sizeof(m_aryStopTime));
	m_TextTip.SetFontSize(m_nTextFontSize);
	m_TextTip.SetTextColor(m_uColor);
	m_AgainAndBet.Enable(TRUE);
	m_nPlayEffectTime = 0;
	m_Leave.Enable(TRUE);
	m_AgainAndBet.Show();
	m_Leave.Show();
	return 1;
}


/*********************************************************************
* 功能：活动函数
**********************************************************************/
void KUiDiceWeaveBingo::Breathe()
{
	switch(m_nStatus)
	{
	case enumDWBS_INITIALIZE:
		InitializeGame();
		m_nStatus = enumDWBS_WAITING_BET;
		break;

	case enumDWBS_WAITING_BET:
		break;

	case enumDWBS_BETTING:
		if(PlayDiceRolling() == 3 && g_pCoreShell)
		{
			g_pCoreShell->OperationRequest(GOI_CAN_HANDLE_RESULT_ON_DICE_WEAVE, 0, 0);
		}
		break;

	case enumDWBS_FINISH_ONCE:
		if(m_nResultStatus == 0/*输了*/)
		{
			m_nStatus = enumDWBS_LOSE;
		}
		else
		{
			m_nStatus = enumDWBS_WIN;
			m_AgainAndBet.Show();
			m_Leave.Show();
		}
		break;

	case enumDWBS_WIN:
		if(m_szComingResultString[0])
		{
			m_TextTip.SetText(m_szComingResultString);
		}
		else if(m_nResultStatus == 1)
		{
			m_TextTip.SetText(m_szEarnSome);
		}
		else if(m_nResultStatus == 2)
		{
			m_TextTip.SetText(m_szEarnMore);
		}
		else
		{
			m_TextTip.SetText(m_szBingo);
		}
		if(!PlayEffect())
		{
			m_TextTip.SetText(m_szAgain);
			m_nStatus = enumDWBS_TRY_AGAIN;
		}
		break;

	case enumDWBS_LOSE:
		m_TextTip.SetText(m_szLose);
		m_nStatus = enumDWBS_TRY_AGAIN;
		break;

	case enumDWBS_TRY_AGAIN:
		m_nStatus = enumDWBS_INITIALIZE;
		break;
	}
	return;
}


/*********************************************************************
* 功能：更新界面的显示
**********************************************************************/
void KUiDiceWeaveBingo::UpdateUI(int nBingoMoney)
{
	if(g_pCoreShell)
	{
		m_PlayerMoney.SetIntText(g_pCoreShell->GetGameData(GDI_PLAYER_HOLD_MONEY, 0, 0));
		m_TotleBingoMoney.SetIntText(nBingoMoney);
		m_nbIsBingoMoneyIncome = 1;
	}
}


/*********************************************************************
* 功能：向服务器要求大奖累积奖金
**********************************************************************/
void KUiDiceWeaveBingo::RequestBingoMoney()
{
	if(g_pCoreShell)
	{
		g_pCoreShell->OperationRequest(GOI_QUERY_DICE_WEAVE_BINGO_MONEY, 0, 0);
	}
}


/*********************************************************************
* 功能：响应色子停止动作
**********************************************************************/
void KUiDiceWeaveBingo::OnStopDice(KMyWndImage* Dice)
{
	if(Dice == m_Dice && m_aryStopTime[0] < 0)
	{
		m_aryStopTime[0] = 0;
		g_pCoreShell->OperationRequest(GOI_DICE_NUMBER_ON_DICE_WEAVE, 0, 0);
	}
	else if(Dice == (m_Dice + 1) && m_aryStopTime[1] < 0)
	{
		m_aryStopTime[1] = 0;
		g_pCoreShell->OperationRequest(GOI_DICE_NUMBER_ON_DICE_WEAVE, 1, 0);
	}
	else if(Dice == (m_Dice + 2) && m_aryStopTime[2] < 0)
	{
		m_aryStopTime[2] = 0;
		g_pCoreShell->OperationRequest(GOI_DICE_NUMBER_ON_DICE_WEAVE, 2, 0);
	}
}


/*********************************************************************
* 功能：播放中奖效果
**********************************************************************/
int KUiDiceWeaveBingo::PlayEffect()
{
	if(m_nPlayEffectTime == 0)
	{
		m_nPlayEffectTime = IR_GetCurrentTime();
	}
	if(IR_GetCurrentTime() - m_nPlayEffectTime > EFFECT_LAST_TIME)
	{
		m_TextTip.SetFontSize(m_nTextFontSize);
		m_TextTip.SetTextColor(m_uColor);
		m_nPlayEffectTime = 0;
		return 0;
	}
	else
	{
		if(m_TextTip.GetFontSize() != 16)
		{
			m_TextTip.SetFontSize(16);
		}
		if(IR_GetCurrentTime() & 1)
		{
			m_TextTip.SetTextColor(m_uColor);
		}
		else
		{
			m_TextTip.SetTextColor(m_uWinkColor);
		}
		return 1;
	}
}


/*********************************************************************
* 功能：播放色子转动
**********************************************************************/
int	KUiDiceWeaveBingo::PlayDiceRolling()
{
	int nRet = 0;
	int nTime = IR_GetCurrentTime();

	if(m_aryStopTime[0] > nTime || m_aryStopTime[0] < 1)
	{
		m_Dice[0].SetFrame(rand() % 6);
	}
	else
	{
		m_Dice[0].SetFrame(m_aryDiceResult[0] - 1);
		nRet  ++;
	}

	if(m_aryStopTime[1] > nTime || m_aryStopTime[1] < 1)
	{
		m_Dice[1].SetFrame(rand() % 6);
	}
	else
	{
		m_Dice[1].SetFrame(m_aryDiceResult[1] - 1);
		nRet  ++;
	}

	if(m_aryStopTime[2] > nTime || m_aryStopTime[2] < 1)
	{
		m_Dice[2].SetFrame(rand() % 6);
	}
	else
	{
		m_Dice[2].SetFrame(m_aryDiceResult[2] - 1);
		nRet  ++;
	}

	return nRet;
}


/*********************************************************************
* 功能：一个色子结果进来了
**********************************************************************/
void KUiDiceWeaveBingo::DiceNumberIncome(int nDice, int nNumber)
{
	if(nDice >= 0 && nDice < (sizeof(m_aryDiceResult) / sizeof(int)))
	{
		m_aryDiceResult[nDice] = nNumber;
		m_aryStopTime[nDice] = IR_GetCurrentTime();
		PlayDiceRolling();
	}
}


//获奖结果进来了
void KUiDiceWeaveBingo::ResultIncome(int nResultStatus, char *szString)
{
	m_nResultStatus = nResultStatus;
	m_nStatus = enumDWBS_FINISH_ONCE;
	if(szString && szString[0])
	{
		strcpy(m_szComingResultString, szString);
	}
	else
	{
		m_szComingResultString[0] = 0;
	}
}



int	KMyWndImage::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	if(uMsg == WM_LBUTTONDOWN && m_pParentWnd)
	{
		m_pParentWnd->WndProc(WND_N_BUTTON_CLICK, (unsigned int)(KWndWindow*)this, 0);
	}
	return 0;
}
