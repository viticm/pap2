//////////////////////////////////////////////////////////////////////////////////////
//	文件名			:	UiKillerCreateTask.cpp
//	创建人			:	王西贝
//	创建时间		:	2003-12-26 10:55:32
//	文件说明		:	创建通缉任务对话框的实现
//////////////////////////////////////////////////////////////////////////////////////

#include "KWin32.h"
#include "UiKillerCreateTask.h"
#include "KIniFile.h"
#include "../Elem/AutoLocateWnd.h"
#include "../Elem/WndMessage.h"
#include "../Elem/Wnds.h"
#include "../UiBase.h"
#include "../UiSoundSetting.h"
#include "UiInformation.h"
#include "UiInformation2.h"
#include "../../Core/Src/CoreShell.h"
#include "../../Core/Src/KKillerTask.h"
#include "../../../core/src/gamedatadef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

#define	SCHEME_INI 	"通缉创建任务编辑框.ini"

extern iCoreShell*		g_pCoreShell;

KUiKillerCreateTask* KUiKillerCreateTask::m_pSelf = NULL;
//KWndText256   KUiKillerCreateTask::m_ObjectName;
KUiKillerCreateTask::KUiKillerCreateTask()
{
	m_nActiveTimeNum = 1;
}

KRoleNameEdit::KRoleNameEdit()
{
	m_Text[0] = m_Text[sizeof(m_Text) - 1] = 0;
	SetTextPtr(m_Text, sizeof(m_Text));
}


void WPrintMoney(char* szBuf, int nMoney);
#define	ACTIVE_TIME_FORMAT	"%d 小时"
//--------------------------------------------------------------------------
//	功能：打开窗口，返回唯一的一个类对象实例
//--------------------------------------------------------------------------
KUiKillerCreateTask* KUiKillerCreateTask::OpenWindow(unsigned int uParam)
{
	static char szMsg[128];
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiKillerCreateTask;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
	{
		m_pSelf->m_nActiveTimeNum = 1;
		if (g_pCoreShell->GetGameData(GDI_PLAYER_HOLD_MONEY, 0, 0) < m_pSelf->m_nMoneyPerHour + m_pSelf->m_nMinReward)
		{
			strcpy(szMsg, "对不起，您没有足够的现金，建立一个通缉任务至少需要纹银");
			WPrintMoney(szMsg + strlen(szMsg), m_pSelf->m_nActiveTimeNum * m_pSelf->m_nMoneyPerHour + m_pSelf->m_nMinReward);
			strcat(szMsg, "！！");
			UIMessageBox(szMsg, NULL, "哦，知道了.....");
			return NULL;
		}
		UiSoundPlay(UI_SI_WND_OPENCLOSE);
		sprintf(szMsg, ACTIVE_TIME_FORMAT, m_pSelf->m_nActiveTimeNum * 4);
		m_pSelf->m_cActiveTime.SetText(szMsg);
		WPrintMoney(szMsg, m_pSelf->m_nActiveTimeNum * m_pSelf->m_nMoneyPerHour);
		m_pSelf->m_cCommision.SetText(szMsg);
		m_pSelf->m_cReward.SetIntText(m_pSelf->m_nMinReward);

		m_pSelf->Show();
		m_pSelf->BringToTop();
	}
	return m_pSelf;
}

//--------------------------------------------------------------------------
//	功能：关闭销毁窗口
//--------------------------------------------------------------------------
void KUiKillerCreateTask::CloseWindow(bool bDestroy)
{
	if (m_pSelf)
	{
		if (bDestroy)
		{
			m_pSelf->Destroy();
			m_pSelf = NULL;
		}
		else
			m_pSelf->Hide();
	}
}

KUiKillerCreateTask*	KUiKillerCreateTask::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	return NULL;
}




//--------------------------------------------------------------------------
//	功能：初始化
//--------------------------------------------------------------------------
int KUiKillerCreateTask::Initialize()
{
	AddChild(&m_cKilleeName);
	AddChild(&m_cReward);
	AddChild(&m_cActiveTime);
	AddChild(&m_BtnIncrease);
	AddChild(&m_BtnDecrease);
	AddChild(&m_cCommision);
	m_cCommision.Enable(false);						//使得不能输入
	AddChild(&m_OkBtn);
	AddChild(&m_CancelBtn);
	Wnd_AddWindow(this);
	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);
	m_nMoneyPerHour	= g_KillerTask.GetMoneyPerHour();
	m_nMaxActiveTime = g_KillerTask.GetMaxActiveTaskTime();
	m_nMinReward = g_KillerTask.GetMinReward();
	return true;
}

//--------------------------------------------------------------------------
//	功能：载入窗口的界面方案
//--------------------------------------------------------------------------
void KUiKillerCreateTask::LoadScheme(const char* pScheme)
{
	if (m_pSelf)
	{
		char		Buff[128];
		KIniFile	Ini;
		sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI);
		if (Ini.Load(Buff))
		{
			m_pSelf->Init(&Ini, "Main");
			m_pSelf->m_cKilleeName.Init(&Ini, "InputTarget");
			m_pSelf->m_cReward.Init(&Ini, "InputReward");
			m_pSelf->m_cActiveTime.Init(&Ini, "InputTaskTime");
			m_pSelf->m_BtnIncrease.Init(&Ini, "AddTaskTimeBtn");
			m_pSelf->m_BtnDecrease.Init(&Ini, "SubTaskTimeBtn");
			m_pSelf->m_cCommision.Init(&Ini, "TaskTimeCost");
			m_pSelf->m_OkBtn.Init(&Ini, "OkBtn");
			m_pSelf->m_CancelBtn.Init(&Ini, "CancelBtn");
		}
	}
}

//--------------------------------------------------------------------------
//	功能：窗口消息函数
//--------------------------------------------------------------------------
int KUiKillerCreateTask::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int nRet = 0;
	switch(uMsg)
	{
	case WND_N_BUTTON_CLICK:
		if (uParam == (unsigned int)(KWndWindow*)&m_OkBtn)
			OnOk();
		else if (uParam == (unsigned int)(KWndWindow*)&m_CancelBtn)
			OnCancel();
		else if(uParam == (unsigned int)&m_BtnIncrease)
		{
			OnModifyCount(TRUE);
		}
		else if(uParam == (unsigned int)&m_BtnDecrease)
		{
			OnModifyCount(FALSE);
		}

		break;
	case WM_KEYDOWN:
		if (uParam == VK_RETURN)
		{
			OnOk();
			nRet = 1;
		}
		else if (uParam == VK_ESCAPE)
		{
			OnCancel();
			nRet = 1;
		}
		break;
	case WND_N_EDIT_SPECIAL_KEY_DOWN:
		if (nParam == VK_RETURN &&
			uParam == (unsigned int)(KWndWindow*)&m_OkBtn)
		{
			OnOk();
			nRet = 1;
		}
		break;
	case WND_N_EDIT_CHANGE:
		if (uParam == (unsigned int)(KWndWindow*)&m_cReward)
			OnRewardInputChanged();
		break;
	case WND_N_BUTTON_HOLD:
		if(uParam == (unsigned int)&m_BtnIncrease)
			OnModifyCount(true);
		else if(uParam == (unsigned int)&m_BtnDecrease)
			OnModifyCount(false);

	default:
		nRet = KWndShowAnimate::WndProc(uMsg, uParam, nParam);
	}
	return nRet;
}

//--------------------------------------------------------------------------
//	功能：响应点击确认按钮，实际标价
//--------------------------------------------------------------------------
void KUiKillerCreateTask::OnOk()
{
//	OnRewardInputChanged();
//	OnTimeInputChanged();
	int nHoldMoney = g_pCoreShell->GetGameData(GDI_PLAYER_HOLD_MONEY, 0, 0);
	int nReward = m_cReward.GetIntNumber() > 0 ? m_cReward.GetIntNumber() : 0;
	int nActicveHour = m_nActiveTimeNum;
	if (nActicveHour * m_nMoneyPerHour + nReward > nHoldMoney ||
		nActicveHour <= 0 || nReward < m_nMinReward)
	{
		return;
	}
	tagKillerSubmitTask  summitTask;
	summitTask.m_nRentFee = nActicveHour * m_nMoneyPerHour;
	summitTask.m_nReward = nReward;
	if (m_cKilleeName.GetText(summitTask.m_chKilleeName, sizeof(summitTask.m_chKilleeName) - 1, true) > 0)
	{

		if (g_pCoreShell)
			g_pCoreShell->OperationRequest(GOI_KILLER_SUBMITTASK, (unsigned int)&summitTask, 0);
		CloseWindow(true);
	}
}


void KUiKillerCreateTask::OnCancel()
{
//	if (g_pCoreShell)
//		g_pCoreShell->OperationRequest(GOI_MARK_STALL_PRICE, m_nObjIndex, 0);
	CloseWindow(true);
}
void KUiKillerCreateTask::OnTimeInputChanged()
{
	int nNewCount;
	int nReward = m_cReward.GetIntNumber() > 0 ? m_cReward.GetIntNumber() : 0;

	int nCount = m_nActiveTimeNum;
	int nHoldMoney = g_pCoreShell->GetGameData(GDI_PLAYER_HOLD_MONEY, 0, 0);
	if (m_nActiveTimeNum <= 0)
	{
		nNewCount = 0;
	}
	else
	{
		if (nCount * m_nMoneyPerHour + nReward > nHoldMoney)
			nNewCount = nHoldMoney / m_nMoneyPerHour;
		else
			nNewCount = nCount;
		if (nNewCount < 0)
			nNewCount = 0;
	}
	static char szMsg[128];
	if (nNewCount != nCount)
	{
		sprintf(szMsg, ACTIVE_TIME_FORMAT, m_nActiveTimeNum * 4);
		m_cActiveTime.SetText(szMsg);
	}
	WPrintMoney(szMsg, m_nActiveTimeNum * m_nMoneyPerHour);
	m_cCommision.SetText(szMsg);
}

void KUiKillerCreateTask::OnRewardInputChanged()
{
	int nReward = m_cReward.GetIntNumber();
	int nCommision = m_nActiveTimeNum * m_nMoneyPerHour;
	int nHoldMoney = g_pCoreShell->GetGameData(GDI_PLAYER_HOLD_MONEY, 0, 0);
	if (nReward > nHoldMoney - nCommision)
	{
		nReward = nHoldMoney - nCommision;
		m_cReward.SetIntText(nReward);
	}
}
void KUiKillerCreateTask::OnModifyCount(bool bAdd)
{
	int nCount = m_nActiveTimeNum > 1 ? m_nActiveTimeNum : 1;
	int nReward = m_cReward.GetIntNumber() > 0 ? m_cReward.GetIntNumber() : 0;
	int nHoldMoney = g_pCoreShell->GetGameData(GDI_PLAYER_HOLD_MONEY, 0, 0);
	static char szMsg[128];
	if (bAdd)
	{
		if (nCount + 1 <= m_nMaxActiveTime &&
			(nCount + 1) * m_nMoneyPerHour + nReward <= nHoldMoney)
		{
			m_nActiveTimeNum = nCount + 1;
			sprintf(szMsg, ACTIVE_TIME_FORMAT, m_nActiveTimeNum * 4);
			m_cActiveTime.SetText(szMsg);
			WPrintMoney(szMsg, m_nActiveTimeNum * m_nMoneyPerHour);
			m_cCommision.SetText(szMsg);
		}
	}
	else if (nCount > 1 && !bAdd)
	{
		m_nActiveTimeNum = nCount - 1;
		sprintf(szMsg, ACTIVE_TIME_FORMAT, m_nActiveTimeNum * 4);
		m_cActiveTime.SetText(szMsg);
		WPrintMoney(szMsg, m_nActiveTimeNum * m_nMoneyPerHour);
		m_cCommision.SetText(szMsg);
	}
}
