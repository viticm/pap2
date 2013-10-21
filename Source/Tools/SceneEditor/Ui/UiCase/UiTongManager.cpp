/********************************************************************
File        : UiTongManager.cpp
Creator     : Fyt(Fan Zhanpeng)
create data : 08-29-2003(mm-dd-yyyy)
Description : 管理帮会的界面
*********************************************************************/

#include "KWin32.h"
#include "KIniFile.h"
#include "CoreShell.h"
#include "KPlayerDef.h"

#include "../elem/wnds.h"
#include "../elem/wndmessage.h"
#include "../elem/popupmenu.h"

#include "../UiBase.h"
#include "../UiSoundSetting.h"

#include "../../../Engine/src/KFilePath.h"
#include "../ChatFilter.h"

#include "UiPopupPasswordQuery.h"
#include "UiTongAssignBox.h"
#include "UiTongGetString.h"
#include "UiTongManager.h"
#include "UiInformation.h"
#include "UiTongAssignTitle.h"
#include "UiTongCreateSheet.h"
#include "UiGetString.h"
#include "uigetmoney.h"
#include "uitongsalary.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

KUiTongManager* KUiTongManager::ms_pSelf = NULL;
KTongInfo       KUiTongManager::m_TongData;
int             KUiTongManager::m_nElderDataIndex = 0;
int             KUiTongManager::m_nMemberDataIndex = 0;
int             KUiTongManager::m_nCaptainDataIndex = 0;
int				KUiTongManager::m_nTongLeagueDataIndex = 0;

#define TONG_MANAGER_INI "帮会管理界面.ini"
#define TONG_DATA_TEMP_FILE "帮会临时文件.ini"

#define TONG_REQUEST_INTERVAL 100000
#define MEMBER_LEAVE_COST 1
#define MANAGER_LEAVE_COST 10
#define DIRECTOR_LEAVE_COST 50

extern CChatFilter g_ChatFilter;
extern iCoreShell* g_pCoreShell;
extern KUiInformation g_UiInformation;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

KUiTongManager::KUiTongManager()
{
	m_uStatus = 0;
	m_szPassword[0] = 0;
	m_nSelectIndex = -1;
	m_nCurrentCheckBox = -1;
	m_TongData.szName[0] = 0;
	m_szTargetPlayerName[0] = 0;
	m_uLastRequestTongDataTime = 0;
	m_uLastRequestElderListTime = 0;
	m_uLastRequestMemberListTime = 0;
	m_uLastRequestCaptainListTime = 0;
}

KUiTongManager::~KUiTongManager()
{

}


/*********************************************************************
* 功能：打开窗口
**********************************************************************/
KUiTongManager* KUiTongManager::OpenWindow()
{
	if(g_pCoreShell)
	{
    	if (ms_pSelf == NULL)
    	{
		    ms_pSelf = new KUiTongManager;
		    if (ms_pSelf)
    			ms_pSelf->Initialize();
    	}
    	if (ms_pSelf)
    	{
		    UiSoundPlay(UI_SI_WND_OPENCLOSE);
			ms_pSelf->BringToTop();
			ms_pSelf->Show();
	    }
	}
	return ms_pSelf;
}


/*********************************************************************
* 功能：打开窗口，同时安排界面布局
**********************************************************************/
KUiTongManager* KUiTongManager::OpenWindow(char* pszPlayerName)
{
	OpenWindow();
	if(ms_pSelf)
	{
		ms_pSelf->ArrangeComposition(pszPlayerName);
	}
	return ms_pSelf;
}


/*********************************************************************
* 功能：根据传入的玩家名字，安排界面布局
**********************************************************************/
void KUiTongManager::ArrangeComposition(char* pszPlayerName)
{
	if(g_pCoreShell)
	{
    	KUiPlayerItem Player;
		int nKind;

    	ClearTongData();
    	memset(&Player, 0, sizeof(KUiPlayerItem));
		m_nIndex = 0;
    	if(pszPlayerName && pszPlayerName[0])
    	{
			if(g_pCoreShell->FindSpecialNPC(pszPlayerName, &Player, nKind))
				m_nIndex = Player.nIndex;
	    }
	    else
	    {
    		KUiPlayerBaseInfo Me;
		    g_pCoreShell->GetGameData(GDI_PLAYER_BASE_INFO, (unsigned int)&Me, 0);
			if(Me.Name[0])
			{
				strcpy(Player.Name, Me.Name);
				Player.nIndex = CLIENT_PLAYER_INDEX;
				m_nIndex = CLIENT_PLAYER_INDEX;
			}
	    }
		if(m_nIndex > 0)
		{
			strcpy(m_szPlayerName, Player.Name);
	        ms_pSelf->UpdateBtnTheme(-1, TRUE);
	        g_pCoreShell->TongOperation(GTOI_REQUEST_PLAYER_TONG, (unsigned int)&Player, (int)TRUE);
		}
		else
		{
			CloseWindow();
		}
	}
}


/*********************************************************************
* 功能：如果窗口正被显示，则返回实例指针
**********************************************************************/
KUiTongManager* KUiTongManager::GetIfVisible()
{
	if (ms_pSelf && ms_pSelf->IsVisible())
		return ms_pSelf;
	return NULL;
}


/*********************************************************************
* 功能：关闭窗口，同时可以选则是否删除对象实例
**********************************************************************/
void KUiTongManager::CloseWindow(bool bDestory)
{
	if (ms_pSelf)
	{
		ms_pSelf->Hide();
		if(KUiTongAssignBox::GetIfVisible())
		{
			KUiTongAssignBox::CloseWindow();
		}
		if(KUiTongAssignTitle::GetIfVisible())
		{
			KUiTongAssignTitle::CloseWindow();
		}
		if(KUiTongCreateSheet::GetIfVisible())
		{
			KUiTongCreateSheet::CloseWindow();
		}
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
void KUiTongManager::Initialize()
{
	AddChild(&m_MemberCount);
	AddChild(&m_MasterName);
	AddChild(&m_Alignment);
	AddChild(&m_TongName);
	AddChild(&m_Money);

	AddChild(&m_BtnDismiss);
	AddChild(&m_BtnRecruit);
	AddChild(&m_BtnAssign);
	AddChild(&m_BtnDemise);
	AddChild(&m_BtnLeave);

	AddChild(&m_BtnCaptainList);
	AddChild(&m_BtnMemberList);
	AddChild(&m_BtnElderList);
	AddChild(&m_BtnCancel);
	AddChild(&m_BtnApply);

	AddChild(&m_ListLeft);
	AddChild(&m_ListRight);
	AddChild(&m_ListScroll);
	m_ListLeft.SetScrollbar(&m_ListScroll);

	AddChild(&m_BtnRefresh);
	AddChild(&m_BtnClose);

	AddChild(&m_BtnExtendList);
	AddChild(&m_BtnTitle);
	AddChild(&m_MaleTitle);
	AddChild(&m_FemaleTitle);

	AddChild(&m_BtnAddLeague);
	AddChild(&m_BtnLeagueList);
	AddChild(&m_BtnCreateLeague);
	AddChild(&m_BtnRemoveLeague);

	AddChild(&m_BtnTakeMoney);
	AddChild(&m_BtnLeaveLeague);
	AddChild(&m_BtnDepositMoney);
	AddChild(&m_BtnDismissLeague);

	AddChild(&m_TongExp);
	AddChild(&m_TongLevel);
	AddChild(&m_btnSalary);
	AddChild(&m_BtnApplyLeague);

	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);

	Wnd_AddWindow(this);
}


/*********************************************************************
* 功能：载入界面方案
**********************************************************************/
void KUiTongManager::LoadScheme(const char* pScheme)
{
	if(ms_pSelf)
	{
		char		Buff[128];
		KIniFile	Ini;
		sprintf(Buff, "%s\\%s", pScheme, TONG_MANAGER_INI);

		if(Ini.Load(Buff))
		{
			ms_pSelf->Init(&Ini, "Main");

			ms_pSelf->m_TongName.Init(&Ini, "TextTongName");
	        ms_pSelf->m_MasterName.Init(&Ini, "TextMasterName");
	        ms_pSelf->m_Alignment.Init(&Ini, "Faction");
	        ms_pSelf->m_MemberCount.Init(&Ini, "MemberCount");
	        ms_pSelf->m_Money.Init(&Ini, "Money");

	        ms_pSelf->m_BtnDismiss.Init(&Ini, "BtnDismiss");
	        ms_pSelf->m_BtnAssign.Init(&Ini, "BtnAssign");
	        ms_pSelf->m_BtnDemise.Init(&Ini, "BtnDemise");
	        ms_pSelf->m_BtnLeave.Init(&Ini, "BtnLeave");
	        ms_pSelf->m_BtnRecruit.Init(&Ini, "BtnRecruit");

	        ms_pSelf->m_BtnApply.Init(&Ini, "BtnApply");
	        ms_pSelf->m_BtnCancel.Init(&Ini, "BtnCancel");
			ms_pSelf->m_BtnElderList.Init(&Ini, "BtnDirectorList");
	        ms_pSelf->m_BtnCaptainList.Init(&Ini, "BtnManagerList");
	        ms_pSelf->m_BtnMemberList.Init(&Ini, "BtnMemberList");

			ms_pSelf->m_ListLeft.Init(&Ini, "ListLeft");
			ms_pSelf->m_ListRight.Init(&Ini, "ListRight");
			ms_pSelf->m_ListScroll.Init(&Ini, "ListScrollbar");

			ms_pSelf->m_BtnRefresh.Init(&Ini, "BtnRefresh");
			ms_pSelf->m_BtnClose.Init(&Ini, "BtnClose");

			ms_pSelf->m_BtnExtendList.Init(&Ini, "BtnExtendList");
			ms_pSelf->m_BtnTitle.Init(&Ini, "Title");

			ms_pSelf->m_MaleTitle.Init(&Ini, "BtnMaleTitle");
			ms_pSelf->m_FemaleTitle.Init(&Ini, "BtnFemaleTitle");
			ms_pSelf->m_BtnLeagueList.Init(&Ini, "BtnLeagueList");
			ms_pSelf->m_BtnCreateLeague.Init(&Ini, "BtnCreateLeague");
			ms_pSelf->m_BtnAddLeague.Init(&Ini, "BtnAddLeagueMember");
			ms_pSelf->m_BtnRemoveLeague.Init(&Ini, "BtnRemoveLeagueMember");
			ms_pSelf->m_BtnDismissLeague.Init(&Ini, "BtnDismissLeague");
			ms_pSelf->m_BtnLeaveLeague.Init(&Ini, "BtnLeaveLeague");

			ms_pSelf->m_BtnDepositMoney.Init(&Ini, "BtnDepositMoney");
			ms_pSelf->m_BtnTakeMoney.Init(&Ini, "BtnTakeMoney");

			ms_pSelf->m_TongExp.Init(&Ini, "Exp");
			ms_pSelf->m_TongLevel.Init(&Ini, "Level");
			ms_pSelf->m_btnSalary.Init(&Ini, "BtnSalary");
			ms_pSelf->m_BtnApplyLeague.Init(&Ini, "BtnApplyLeague");

			Ini.GetString("Main", "StringDismiss", "", ms_pSelf->m_szDismiss, sizeof(ms_pSelf->m_szDismiss));
			Ini.GetString("Main", "StringAssign", "", ms_pSelf->m_szAssign, sizeof(ms_pSelf->m_szAssign));
			Ini.GetString("Main", "StringDemise", "", ms_pSelf->m_szDemise, sizeof(ms_pSelf->m_szDemise));
			Ini.GetString("Main", "StringLeave", "你真的要离开帮会？你需要付出%d两为代价。", ms_pSelf->m_szLeave, sizeof(ms_pSelf->m_szLeave));

			Ini.GetString("Main", "StringJustice", "", ms_pSelf->m_szJustice, sizeof(ms_pSelf->m_szJustice));
			Ini.GetString("Main", "StringBalance", "", ms_pSelf->m_szBalance, sizeof(ms_pSelf->m_szBalance));
			Ini.GetString("Main", "StringEvil", "", ms_pSelf->m_szEvil, sizeof(ms_pSelf->m_szEvil));

			Ini.GetString("Main", "StringConfirm", "", ms_pSelf->m_szConfirm, sizeof(ms_pSelf->m_szConfirm));
			Ini.GetString("Main", "StringCancel", "", ms_pSelf->m_szCancel, sizeof(ms_pSelf->m_szCancel));

			Ini.GetString("Main", "StringDismissLeague", "你真的要解散联盟？", ms_pSelf->m_szDismissLeague, sizeof(ms_pSelf->m_szDismissLeague));
			Ini.GetString("Main", "StringRemoveLeagueMember", "你真的要剔除这个帮会吗？", ms_pSelf->m_szRemoveLeagueMember, sizeof(ms_pSelf->m_szRemoveLeagueMember));
			Ini.GetString("Main", "StringLeaveLeague", "你的帮会真的要离开联盟？", ms_pSelf->m_szLeaveLeague, sizeof(ms_pSelf->m_szLeaveLeague));
		}
	}
}


/*********************************************************************
* 功能：窗口函数
**********************************************************************/
int KUiTongManager::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	switch(uMsg)
	{
	case WND_N_LIST_ITEM_SEL:
		if(uParam == (unsigned int)&m_ListLeft || uParam == (unsigned int)&m_ListRight)
		{
			KLinkStruct<KPilgarlicItem> *pData = GetCurrentSelectedData();
			KPilgarlicItem *pNode = NULL;
			if(uParam == (unsigned int)&m_ListLeft)
			{
				m_ListRight.SetCurSel(-1);
				m_nSelectIndex = WorkoutIndex(nParam, 1);
			}
			else
			{
				m_ListLeft.SetCurSel(-1);
				m_nSelectIndex = WorkoutIndex(nParam, 0);
			}
			if(pData)
			{
				 pNode = pData->GetItem(m_nSelectIndex);
			}
			if(nParam == -1 || (pNode && !pNode->Name[0]))
			{
				m_BtnDismiss.Enable(FALSE);
				m_BtnAssign.Enable(FALSE);
				m_BtnDemise.Enable(FALSE);
				if(nParam != -1	&& m_nCurrentCheckBox != enumTONG_FIGURE_MEMBER)
					m_BtnTitle.Enable(TRUE);
				else
					m_BtnTitle.Enable(FALSE);

				if(nParam != -1 && m_nCurrentCheckBox == TONG_LIST_TYPE_LEAGUE)
					m_BtnRemoveLeague.Enable(TRUE);
				else
					m_BtnRemoveLeague.Enable(FALSE);
			}
			else
			{
				m_BtnDismiss.Enable(TRUE);
				m_BtnAssign.Enable(TRUE);
				m_BtnDemise.Enable(TRUE);
				if(m_nCurrentCheckBox != enumTONG_FIGURE_MEMBER)
					m_BtnTitle.Enable(TRUE);
				if(m_nCurrentCheckBox == TONG_LIST_TYPE_LEAGUE)
					m_BtnRemoveLeague.Enable(TRUE);
			}
		}
		break;

	case WND_N_BUTTON_CLICK:
		if(uParam == (unsigned int)&m_BtnDismiss)
		{
			PopupConfirmWindow(m_szDismiss, RESULT_T_DISMISS);
		}
		else if(uParam == (unsigned int)&m_BtnAssign)
		{
			OnAssign();
		}
		else if(uParam == (unsigned int)&m_BtnDemise)
		{
			PopupConfirmWindow(m_szDemise, RESULT_T_DEMISE);
		}
		else if(uParam == (unsigned int)&m_BtnLeave)
		{
			char szBuff[64];
			if(m_nRelation == enumTONG_FIGURE_MEMBER)
			{
				sprintf(szBuff, m_szLeave, MEMBER_LEAVE_COST);
			}
			else if(m_nRelation == enumTONG_FIGURE_MANAGER)
			{
				sprintf(szBuff, m_szLeave, MANAGER_LEAVE_COST);
			}
			else
			{
				sprintf(szBuff, m_szLeave, DIRECTOR_LEAVE_COST);
			}
			PopupConfirmWindow(szBuff, RESULT_T_LEAVE, TRUE);
		}
		else if(uParam == (unsigned int)&m_BtnRecruit)
		{
			g_pCoreShell->TongOperation(GTOI_TONG_RECRUIT,
				                        m_BtnRecruit.IsButtonChecked(), 0);
		}
		else if(uParam == (unsigned int)&m_BtnApply)
		{
			if(g_pCoreShell)
			{
				KUiPlayerRelationWithOther Relation;
				KTongInfo TongInfo;
				g_pCoreShell->GetGameData(GDI_CLIENT_TONG_INFO, (unsigned int)&Relation, (int)&TongInfo);
				if(m_TongData.btUnionLeader &&
				   !strcmp(m_szPlayerName, m_TongData.szMasterName) &&
				   Relation.nRelation == enumTONG_FIGURE_MASTER     &&
				   TongInfo.m_btUnionFlag == 0)
				{
					OnAction(TONG_ACTION_ADD_LEAGUE_MEMBER, TRUE);
				}
				else
				{
					OnAction(TONG_ACTION_APPLY, TRUE);
				}
			}
		}
		else if(uParam == (unsigned int)&m_BtnApplyLeague)
		{
			if(g_pCoreShell)
			{
				OnAction(TONG_ACTION_ADD_LEAGUE_MEMBER, TRUE);
			}
		}
		else if(uParam == (unsigned int)&m_BtnCancel || uParam == (unsigned int)&m_BtnClose)
		{
			CloseWindow();
		}
		else if(uParam == (unsigned int)&m_BtnRefresh)
		{
			OnRefresh();
		}
		else if(uParam == (unsigned int)&m_BtnElderList)
		{
			UpdateListCheckButton(enumTONG_FIGURE_DIRECTOR);
			if(m_ElderData.GetCount() == 0)
			    LoadData(enumTONG_FIGURE_DIRECTOR);
		}
		else if(uParam == (unsigned int)&m_BtnCaptainList)
		{
			UpdateListCheckButton(enumTONG_FIGURE_MANAGER);
			if(m_CaptainData.GetCount() == 0)
			    LoadData(enumTONG_FIGURE_MANAGER);
		}
		else if(uParam == (unsigned int)&m_BtnMemberList)
		{
			UpdateListCheckButton(enumTONG_FIGURE_MEMBER);
			if(m_MemberData.GetCount() == 0)
			    LoadData(enumTONG_FIGURE_MEMBER);
		}
		else if(uParam == (unsigned int)&m_BtnLeagueList)
		{
			UpdateListCheckButton(TONG_LIST_TYPE_LEAGUE);
			if(m_LeagueData.GetCount() == 0)
				LoadData(TONG_LIST_TYPE_LEAGUE);
		}
		else if(uParam == (unsigned int)&m_BtnExtendList)
		{
			if(m_BtnExtendList.IsButtonChecked())
			{
				m_uStatus |= TONG_MANAGER_STATUS_SHOW_ALL_MEMBER;
			}
			else
			{
				m_uStatus &= ~TONG_MANAGER_STATUS_SHOW_ALL_MEMBER;
			}
			if(m_nCurrentCheckBox != enumTONG_FIGURE_MEMBER)
				UpdateTongListView(TRUE);
		}
		else if(uParam == (unsigned int)&m_BtnTitle)
		{
			KTongMemberItem *pData;
			if(m_nCurrentCheckBox == enumTONG_FIGURE_MEMBER)
			{
				int nX, nY;
				Wnd_GetCursorPos(&nX, &nY);
				OnPopupSelectSex(nX, nY);
			}
			else
			{
				int nCount = FormArrayData(&pData, m_nCurrentCheckBox);
				if(nCount)
				{
					KUiTongAssignTitle *pTitle = KUiTongAssignTitle::OpenWindow();
					if(pTitle)
					{
						pTitle->InjectData(pData, nCount, m_nCurrentCheckBox, m_nSelectIndex);
						pTitle->UpdateView();
					}
				}
			}
		}
		else if(uParam == (unsigned int)&m_MasterName)
		{
			KTongMemberItem Data;
			strcpy(Data.Name, m_TongData.szMasterName);
			strcpy(Data.szAgname, "帮主");
			KUiTongAssignTitle *pTitle = KUiTongAssignTitle::OpenWindow();
			if(pTitle)
			{
				pTitle->InjectData(&Data, 1, enumTONG_FIGURE_MASTER, 0);
				pTitle->UpdateView();
			}
		}
		else if(uParam == (unsigned int)&m_MaleTitle)
		{
			OnMemberTitle(0);
		}
		else if(uParam == (unsigned int)&m_FemaleTitle)
		{
			OnMemberTitle(1);
		}
		else if(uParam == (unsigned int)&m_Alignment)
		{
			KUiTongCreateSheet *pSheet = KUiTongCreateSheet::OpenWindow();
			if(pSheet)
			{
				pSheet->SetUsage(enumCSU_CHANGE_CAMP);
				pSheet->InjectData(m_TongData.szName, m_TongData.nFaction);
			}
		}
		else if(uParam == (unsigned int)&m_BtnCreateLeague)
		{
			KUiGetString::OpenWindow("请输入联盟名字", "", (KWndWindow *)ms_pSelf, RESULT_T_GET_LEAGUE_NAME, 2, 8);
		}
		else if(uParam == (unsigned int)&m_BtnAddLeague)
		{
		}
		else if(uParam == (unsigned int)&m_BtnRemoveLeague)
		{
			if(m_nSelectIndex > 0)
			{
				PopupConfirmWindow(m_szRemoveLeagueMember, RESULT_T_REMOVE_LEAGUE_MEMBER_CONFIRM);
			}
		}
		else if(uParam == (unsigned int)&m_BtnLeaveLeague)
		{
			PopupConfirmWindow(m_szLeaveLeague, RESULT_T_LEAVE_LEAGUE_CONFIRM, 0, 1);
		}
		else if(uParam == (unsigned int)&m_BtnDepositMoney)
		{
			if(g_pCoreShell)
			{
				int nSelfMoney = g_pCoreShell->GetGameData(GDI_PLAYER_HOLD_MONEY, 0, 0);
				if(nSelfMoney > defTONG_SAVE_MONEY_MAX)
				{
					KUiGetMoney::OpenWindow(0, defTONG_SAVE_MONEY_MAX, (KWndWindow *)ms_pSelf, RESULT_T_DEPOSIT_MONEY, NULL);
				}
				else
				{
					KUiGetMoney::OpenWindow(0, nSelfMoney, (KWndWindow *)ms_pSelf, RESULT_T_DEPOSIT_MONEY, NULL);
				}
			}
		}
		else if(uParam == (unsigned int)&m_BtnTakeMoney)
		{
			if(g_pCoreShell)
			{
				if(m_TongData.nHiMoney || m_TongData.nLoMoney > defTONG_GET_MONEY_MAX)
				{
					KUiGetMoney::OpenWindow(0, defTONG_GET_MONEY_MAX, (KWndWindow *)ms_pSelf, RESULT_T_TAKE_MONEY, NULL);
				}
				else
				{
					KUiGetMoney::OpenWindow(0, m_TongData.nLoMoney, (KWndWindow *)ms_pSelf, RESULT_T_TAKE_MONEY, NULL);
				}
			}
		}
		else if(uParam == (unsigned int)&m_BtnDismissLeague)
		{
			PopupConfirmWindow(m_szDismissLeague, RESULT_T_DISMISS_LEAGUE_CONFIRM, 0, 1);
		}
		else if(uParam == (unsigned int)&m_btnSalary)
		{
			OnSalary();
		}
		break;

	case WND_M_OTHER_WORK_RESULT:
		Show();
		if(nParam && (uParam != RESULT_T_DEPOSIT_MONEY && uParam != RESULT_T_TAKE_MONEY &&
			          uParam != RESULT_T_LEAVE && uParam != RESULT_T_REMOVE_LEAGUE_MEMBER_CONFIRM &&
					  uParam != RESULT_T_DISMISS_LEAGUE_CONFIRM) &&
					  uParam != RESULT_T_LEAVE_LEAGUE_CONFIRM)
					  //里面这些都是nParam不是字符串的
		{
			strcpy(m_szTargetPlayerName, (char *)nParam);
		}
		if(uParam == RESULT_T_DISMISS && nParam)
		{
			OnAction(TONG_ACTION_DISMISS);
		}
		else if(uParam == RESULT_T_ASSIGN && nParam)
		{
			OnAction(TONG_ACTION_ASSIGN);
		}
		else if(uParam == RESULT_T_PASSWORD_TO_DEMISE)
		{
			if(nParam)
			{
				strcpy(m_szPassword, (const char *)nParam);
				PopupConfirmWindow(m_szDemise, RESULT_T_DEMISE);
			}
		}
		else if(uParam == RESULT_T_DEMISE)
		{
			if(nParam)
			{
			    OnAction(TONG_ACTION_DEMISE);
			}
			else
			{
				memset(m_szPassword, 0, sizeof(m_szPassword));
			}
		}
		else if(uParam == RESULT_T_LEAVE && nParam)
		{
			OnAction(TONG_ACTION_LEAVE);
		}
		else if(uParam == RESULT_T_SELECT_SEX)
		{
			OnMemberTitle(nParam);
		}
		else if(uParam == RESULT_T_GET_LEAGUE_NAME)
		{
			if(m_szTargetPlayerName[0] && nParam)
			{
				if(g_ChatFilter.IsTextPass(m_szTargetPlayerName))
				{
					OnAction(TONG_ACTION_CREATE_LEAGUE);
				}
				else
				{
					UIMessageBox("阁下所起的名字，似乎不太恰当吧。");
				}
			}
		}
		else if(uParam == RESULT_T_REMOVE_LEAGUE_MEMBER_CONFIRM && nParam)
		{
			OnAction(TONG_ACTION_REMOVE_LEAGUE_MEMBER);
		}
		else if(uParam == RESULT_T_DISMISS_LEAGUE_CONFIRM && nParam)
		{
			OnAction(TONG_ACTION_DISMISS_LEAGUE);
		}
		else if(uParam == RESULT_T_DEPOSIT_MONEY)
		{
			_ASSERT(g_pCoreShell);
			OnParamAction(TONG_ACTION_DEPOSIT_MONEY, 0, nParam);
		}
		else if(uParam == RESULT_T_TAKE_MONEY)
		{
			OnParamAction(TONG_ACTION_TAKE_MONEY, 0, nParam);
		}
		else if(uParam == RESULT_T_LEAVE_LEAGUE_CONFIRM && nParam)
		{
			OnAction(TONG_ACTION_LEAVE_LEAGUE);
		}
		m_szTargetPlayerName[0] = 0;
		break;

	case WND_N_SCORLLBAR_POS_CHANGED:
		if(uParam == (unsigned int)&m_ListScroll)
		{
    		m_ListLeft.SetTopItemIndex(nParam);
			m_ListRight.SetTopItemIndex(nParam);
		}
		break;

	default:
		return KWndImage::WndProc(uMsg, uParam, nParam);
	}
	return 1;
}


/*********************************************************************
* 功能：根据打开人的身份更新按钮配置方案
**********************************************************************/
void KUiTongManager::UpdateBtnTheme(int nType, BOOL IsDissable)
{
	m_BtnDismiss.Hide();
	m_BtnRecruit.Hide();
	m_BtnAssign.Hide();
	m_BtnDemise.Hide();
	m_BtnLeave.Hide();

	m_BtnCaptainList.Hide();
	m_BtnMemberList.Hide();
	m_BtnElderList.Hide();
	m_BtnCancel.Hide();
	m_BtnApply.Hide();

	m_BtnTitle.Hide();
	m_MaleTitle.Hide();
	m_BtnRefresh.Hide();
	m_FemaleTitle.Hide();
	m_BtnExtendList.Hide();

	m_MasterName.Enable(FALSE);
	m_Alignment.Enable(FALSE);
	m_ListRight.Enable(FALSE);
	m_ListLeft.Enable(FALSE);
	m_BtnLeagueList.Hide();

	m_BtnAddLeague.Hide();
	m_BtnTakeMoney.Hide();
	m_BtnLeaveLeague.Hide();
	m_BtnRemoveLeague.Hide();
	m_BtnCreateLeague.Hide();
	m_BtnDepositMoney.Hide();
	m_BtnDismissLeague.Hide();

	m_BtnApplyLeague.Hide();
	m_btnSalary.Hide();

	if(IsDissable)
	{
		return;
	}

	switch(nType)
	{
	case enumTONG_FIGURE_MASTER:
		m_BtnDismiss.Show();
		m_BtnRecruit.Show();
		m_BtnAssign.Show();
		m_BtnDemise.Show();

		m_BtnCaptainList.Show();
	    m_BtnMemberList.Show();
		m_BtnExtendList.Show();
	    m_BtnElderList.Show();

		m_BtnTitle.Show();
		m_MaleTitle.Show();
		m_FemaleTitle.Show();
		m_MasterName.Enable(TRUE);

		m_Alignment.Enable(TRUE);
		m_ListRight.Enable(TRUE);
		m_ListLeft.Enable(TRUE);
		m_BtnLeagueList.Show();

		m_BtnDepositMoney.Show();
		m_BtnTakeMoney.Show();
		m_BtnRefresh.Show();
		m_btnSalary.Show();
		break;

	case enumTONG_FIGURE_DIRECTOR:
		m_BtnDismiss.Show();
		m_BtnRecruit.Show();
		m_BtnAssign.Show();
		m_BtnLeave.Show();

		m_BtnCaptainList.Show();
		m_ListLeft.Enable(TRUE);
	    m_BtnMemberList.Show();
	    m_BtnElderList.Show();

		m_ListRight.Enable(TRUE);
		m_BtnDepositMoney.Show();
		m_BtnLeagueList.Show();
		m_BtnTakeMoney.Show();

		m_BtnRefresh.Show();
		break;

	case enumTONG_FIGURE_MANAGER:
		m_BtnRecruit.Show();
		m_BtnLeave.Show();

		m_BtnElderList.Show();
		m_ListLeft.Enable(TRUE);
		m_ListRight.Enable(TRUE);
		m_BtnDepositMoney.Show();

		m_BtnLeagueList.Show();

		m_BtnRefresh.Show();
		break;

	case enumTONG_FIGURE_MEMBER:
		m_BtnLeave.Show();

		m_BtnElderList.Show();
		m_ListLeft.Enable(TRUE);
		m_ListRight.Enable(TRUE);
		m_BtnDepositMoney.Show();

		m_BtnLeagueList.Show();

		m_BtnRefresh.Show();
		break;

	default:
		m_BtnElderList.Show();
		m_BtnCancel.Show();
		if(g_pCoreShell)
		{
			KTongInfo TongInfo;
			KUiPlayerRelationWithOther Relation;
			g_pCoreShell->GetGameData(GDI_CLIENT_TONG_INFO, (unsigned int)&Relation, (int)&TongInfo);

			if(Relation.nRelation == enumTONG_FIGURE_MASTER && TongInfo.m_btUnionFlag == 0 &&
			   m_TongData.btUnionLeader)
			{
				m_BtnApplyLeague.Show();
			}
			else if(Relation.nRelation < 0)
			{
				m_BtnApply.Show();
			}
		}
		break;
	}

	if(g_pCoreShell && nType >= enumTONG_FIGURE_MANAGER && nType <= enumTONG_FIGURE_MASTER)
	{
		KUiPlayerItem Player;
		memset(&Player, 0, sizeof(KUiPlayerItem));
		strcpy(Player.Name, m_szPlayerName);
		m_BtnRecruit.CheckButton(g_pCoreShell->TongOperation(GTOI_TONG_IS_RECRUIT, (unsigned int)&Player, 0));
	}
}


/*********************************************************************
* 功能：根据选择的列表安排按钮方案
**********************************************************************/
void KUiTongManager::UpdateThemeForList(int nType)
{
	if(nType == TONG_LIST_TYPE_LEAGUE)
	{
		m_BtnExtendList.Hide();
		m_FemaleTitle.Hide();
		m_BtnRefresh.Hide();
		m_BtnDismiss.Hide();
		m_BtnRecruit.Hide();
		m_BtnAssign.Hide();
		m_BtnDemise.Hide();
		m_MaleTitle.Hide();
		m_BtnLeave.Hide();
		m_BtnTitle.Hide();

		m_BtnDismissLeague.Hide();
		m_BtnCreateLeague.Hide();
		m_BtnRemoveLeague.Hide();
		m_BtnLeaveLeague.Hide();

		if(m_Relation == enumTONG_FIGURE_MASTER)
		{
			if(!m_TongData.m_btUnionFlag)
			{
				m_BtnCreateLeague.Show();
			}
			else if(m_TongData.btUnionLeader)
			{
				m_BtnDismissLeague.Show();
				m_BtnRemoveLeague.Show();
			}
			else
			{
				m_BtnLeaveLeague.Show();
			}
		}
	}
	else
	{
		m_BtnAddLeague.Hide();
		m_BtnLeaveLeague.Hide();
		m_BtnCreateLeague.Hide();
		m_BtnRemoveLeague.Hide();
		m_BtnDismissLeague.Hide();

		UpdateBtnTheme(m_Relation);
		if(nType == enumTONG_FIGURE_DIRECTOR)
		{
			if(m_nRelation == enumTONG_FIGURE_MASTER)
			{
				m_BtnAssign.Show();
				m_BtnDismiss.Show();
			}
			else
			{
				m_BtnAssign.Hide();
				m_BtnDismiss.Hide();
			}
		}
		if(nType == enumTONG_FIGURE_MANAGER)
		{
			if(m_nRelation == enumTONG_FIGURE_MASTER || m_nRelation == enumTONG_FIGURE_DIRECTOR)
			{
				m_BtnAssign.Show();
			}
			else
			{
				m_BtnAssign.Hide();
			}
		}
		if(nType == enumTONG_FIGURE_MEMBER)
		{
			if(m_nRelation == enumTONG_FIGURE_MASTER || m_nRelation == enumTONG_FIGURE_DIRECTOR)
			{
				m_BtnAssign.Show();
			}
			else
			{
				m_BtnAssign.Hide();
			}
		}
	}
}


/*********************************************************************
* 功能：更新帮会讯息的显示
**********************************************************************/
void KUiTongManager::UpdateTongInfoView()
{
	char Buff[16];

	m_TongName.SetText(m_TongData.szName);
	m_MasterName.SetText(m_TongData.szMasterName);
	switch(m_TongData.nFaction)
	{
	case camp_justice:
		m_Alignment.SetText(m_szJustice);
		break;

	case camp_balance:
		m_Alignment.SetText(m_szBalance);
		break;

	case camp_evil:
		m_Alignment.SetText(m_szEvil);
		break;
	}
	m_MemberCount.SetText(itoa(m_TongData.nMemberCount + m_TongData.nManagerCount + m_TongData.nDirectorCount, Buff, 10));
	int* pNumber;
	__int64 lMoney;
	char szBuff[32];

	pNumber = (int *)&lMoney;
	*pNumber++ = m_TongData.nLoMoney;
	*pNumber   = m_TongData.nHiMoney;
	sprintf(szBuff, "%I64d", lMoney);
	m_Money.SetText(szBuff);

	m_TongLevel.SetIntText(m_TongData.nLevel);
	if(m_TongData.m_btUnionFlag)
	{
		if(m_TongData.btUnionLeader)
		{
			sprintf(szBuff, "%s盟主", m_TongData.szUnionName);
		}
		else
		{
			sprintf(szBuff, "%s成员", m_TongData.szUnionName);
		}
		m_TongExp.SetText(szBuff);
	}
	else
	{
		m_TongExp.SetText("");
	}

}


/*********************************************************************
* 功能：更新列表栏的显示
**********************************************************************/
void KUiTongManager::UpdateTongListView(BOOL bAll)
{
	KLinkStruct<KPilgarlicItem> *pData = GetCurrentSelectedData();
	bAll = TRUE;

	if(pData)
	{
		int nViewCount, nIndex = 0;
		if(bAll)
		{
			m_ListLeft.ResetContent();
			m_ListRight.ResetContent();
			nViewCount = 0;
		}
		else
		{
			nViewCount = m_ListLeft.GetCount();
			nViewCount += m_ListRight.GetCount();
		}
    	int nDataCount = pData->GetCount();
		if(nDataCount > nViewCount)
		{
			KPilgarlicItem *pNode = pData->GetItem(nViewCount);
			if(pNode)
			{
				char Buff[128], BuffColor[64], szNotOnlineColor[64];
				strcpy(BuffColor, "<color=Yellow>");
				strcpy(szNotOnlineColor, "<color=Water>");

				for(int i = nViewCount;i < nDataCount;i++)
				{
					if(!pNode || (!pNode->Name[0] && !(m_uStatus & TONG_MANAGER_STATUS_SHOW_ALL_MEMBER)))
					{
						pNode = pData->NextItem();
						continue;
					}
					if(m_uStatus & TONG_MANAGER_STATUS_SHOW_ALL_MEMBER)
					{
						if(pNode->Name[0])
						{
							if(pNode->nFlag & 0x02)
							{
								sprintf(Buff, "%-18s %s%s", pNode->Name, BuffColor, pNode->szAgname);
							}
							else
							{
								sprintf(Buff, "%s%-18s %s%s", szNotOnlineColor, pNode->Name, BuffColor, pNode->szAgname);
							}
						}
						else
							sprintf(Buff, "%s%s%s", "  《空缺》         ", BuffColor, pNode->szAgname);
					}
					else
					{
						if(pNode->nFlag & 0x02)
						{
							sprintf(Buff, "%-18s %s%s", pNode->Name, BuffColor, pNode->szAgname);
						}
						else
						{
							sprintf(Buff, "%s%-18s %s%s", szNotOnlineColor, pNode->Name, BuffColor, pNode->szAgname);
						}
					}
					if(nIndex % 2)
					{
						m_ListRight.AddString(nIndex, Buff);
					}
					else
					{
						m_ListLeft.AddString(nIndex, Buff);
					}
					pNode = pData->NextItem();
					nIndex ++;
				}
			}
		}
	}
}


/*********************************************************************
* 功能：返回当前所选择的列表类型所对应的数据集
**********************************************************************/
KLinkStruct<KPilgarlicItem>* KUiTongManager::GetCurrentSelectedData()
{
	switch(m_nCurrentCheckBox)
	{
	case enumTONG_FIGURE_DIRECTOR:
		return &m_ElderData;
		break;

	case enumTONG_FIGURE_MANAGER:
		return &m_CaptainData;
		break;

	case enumTONG_FIGURE_MEMBER:
		return &m_MemberData;
		break;

	case TONG_LIST_TYPE_LEAGUE:
		return &m_LeagueData;
		break;

	default:
		return NULL;
		break;
	}
}


/*********************************************************************
* 功能：返回所选择的会员
**********************************************************************/
KPilgarlicItem* KUiTongManager::GetSelectedPlayer()
{
	KLinkStruct<KPilgarlicItem>* pData = NULL;
	KPilgarlicItem* pNode = NULL;

	pData = GetCurrentSelectedData();
	if(pData)
	{
    	pNode = pData->GetItem(m_nSelectIndex);
	}
	return pNode;
}


/*********************************************************************
* 功能：三个列表CheckBox的管理
**********************************************************************/
void KUiTongManager::UpdateListCheckButton(int nType)
{
   	m_BtnElderList.CheckButton(FALSE);
   	m_BtnMemberList.CheckButton(FALSE);
	m_BtnLeagueList.CheckButton(FALSE);
   	m_BtnCaptainList.CheckButton(FALSE);

   	switch(nType)
   	{
   	case enumTONG_FIGURE_DIRECTOR:
	    m_BtnElderList.CheckButton(TRUE);
	    break;

    case enumTONG_FIGURE_MANAGER:
   		m_BtnCaptainList.CheckButton(TRUE);
	    break;

    case enumTONG_FIGURE_MEMBER:
   		m_BtnMemberList.CheckButton(TRUE);
	    break;

	case TONG_LIST_TYPE_LEAGUE:
		m_BtnLeagueList.CheckButton(TRUE);
		break;

	default:
		return;
    }
	if(nType != m_nCurrentCheckBox)
	{
		m_nCurrentCheckBox = nType;
		m_nSelectIndex = -1;
		UpdateTongListView(TRUE);
		UpdateThemeForList(nType);
	}
}


/*********************************************************************
* 功能：把帮会信息栏清空
**********************************************************************/
void KUiTongManager::ClearTongData()
{
	m_MemberCount.SetText("");
	m_MasterName.SetText("");
	m_Alignment.SetText("");
	m_TongName.SetText("");
	m_Money.SetText("");

	m_ListLeft.ResetContent();
	m_ListRight.ResetContent();

	m_CaptainData.Clear();
	m_MemberData.Clear();
	m_ElderData.Clear();

	m_nElderDataIndex = 0;
	m_nMemberDataIndex = 0;
	m_nCaptainDataIndex = 0;
	m_nTongLeagueDataIndex = 0;
}


/*********************************************************************
* 功能：弹出确认窗口
**********************************************************************/
void KUiTongManager::PopupConfirmWindow(const char* pszInfo, unsigned int uHandleID, int nbSelf, int nbLeague)
{
	//UIMessageBox(pszInfo, this, m_szConfirm, m_szCancel, uHandleID);
	KPilgarlicItem *pPlayer = GetSelectedPlayer();
	if(pPlayer && !nbSelf && !nbLeague)
	{
	    KUiTongGetString::OpenWindow(pszInfo, pPlayer->Name, this, uHandleID, 0, 31);
	}
	else if(m_TongData.szUnionName[0] && nbLeague)
	{
		KUiTongGetString::OpenWindow(pszInfo, m_TongData.szUnionName, this, uHandleID, 0, 31);
	}
	else
	{
		KUiTongGetString::OpenWindow(pszInfo, m_szPlayerName, this, uHandleID, 0, 31);
	}
	Hide();
}


/*********************************************************************
* 功能：接受新的数据的数据接口
**********************************************************************/
void KUiTongManager::NewDataArrive(KUiGameObjectWithName *ParamInfo, KTongMemberItem *pIncome)
{
	KLinkStruct<KPilgarlicItem> *pData;
	int *pNum, nCount;

	if(ParamInfo->nData == enumTONG_FIGURE_DIRECTOR)
	{
		if(ms_pSelf)
		{
		    pData = &ms_pSelf->m_ElderData;
		}
		pNum = &m_nElderDataIndex;
		nCount = defTONG_MAX_DIRECTOR;
	}
	else if(ParamInfo->nData == enumTONG_FIGURE_MANAGER)
	{
		if(ms_pSelf)
		{
		    pData = &ms_pSelf->m_CaptainData;
		}
		pNum = &m_nCaptainDataIndex;
		nCount = defTONG_MAX_MANAGER;
	}
	else if(ParamInfo->nData == enumTONG_FIGURE_MEMBER)
	{
		if(ms_pSelf)
		{
		    pData = &ms_pSelf->m_MemberData;
		}
		pNum = &m_nMemberDataIndex;
		nCount = m_TongData.nMemberCount;
	}
	else if(ParamInfo->nData == TONG_LIST_TYPE_LEAGUE)
	{
		if(ms_pSelf)
		{
		    pData = &ms_pSelf->m_LeagueData;
			pData->Clear();
			ms_pSelf->UpdateTongListView();
		}
		pNum = &m_nTongLeagueDataIndex;
		*pNum = 0;
		nCount = defTONG_MAX_LEAGUE;
	}
	else
	{
		return;
	}
	if(*pNum != ParamInfo->nParam)
	{
		return;
	}

	if(ParamInfo->uParam)
	{
		*pNum += ParamInfo->uParam;
		if(!strcmp(m_TongData.szName, ParamInfo->szName) && ms_pSelf)
		{
			KPilgarlicItem Add;

			for(int i = 0;(unsigned int)i < ParamInfo->uParam;i++)
			{
				memcpy(&Add, pIncome + i, sizeof(KTongMemberItem));
				Add.nHashID = StringToHash(Add.Name);
				pData->AddItem(Add);
			}
			ms_pSelf->UpdateTongListView();
		}
		SaveNewData(ParamInfo, pIncome);
		if(ms_pSelf)
		{
			ms_pSelf->GiveDataToOthers(pIncome, ParamInfo->uParam, ParamInfo->nData);
		}
		if(*pNum < nCount)
		{
			RequestData(ParamInfo->nData, *pNum);
		}
	}
}


/*********************************************************************
* 功能：把数据主动传递给其他需要这些数据的界面
**********************************************************************/
void KUiTongManager::GiveDataToOthers(KTongMemberItem *pItem, int nCount, int nType)
{
	KUiTongSalary    *pSalary    = KUiTongSalary::GetIfVisible();

    if(pSalary)
	{
		pSalary->InjectData(pItem, nCount, nType);
	}
}


/*********************************************************************
* 功能：查询特定位置的前面，某个名字是否已经存在
**********************************************************************/
int KUiTongManager::IsPlayerExist(KPilgarlicItem *MeToCmp, KLinkStruct<KPilgarlicItem> *pData)
{
	unsigned int nID;
	KPilgarlicItem *pNode;

	nID = MeToCmp->nHashID;
	pNode = pData->Begin();
	while(pNode || pNode != MeToCmp)
	{
		if(pNode->nHashID == nID)
		{
			if(!strcmp(MeToCmp->Name, pNode->Name))
			{
				return 1;
			}
		}
		pNode = pData->NextItem();
	}
	return 0;
}


/*********************************************************************
* 功能：接受帮会信息
**********************************************************************/
void KUiTongManager::TongInfoArrive(KUiPlayerRelationWithOther* Relation, KTongInfo *pTongInfo)
{
	if(ms_pSelf && !strcmp(Relation->Name, ms_pSelf->m_szPlayerName))
	{
		memcpy(&ms_pSelf->m_TongData, pTongInfo, sizeof(KTongInfo));
		ms_pSelf->m_nRelation = Relation->nRelation;

		ms_pSelf->UpdateTongInfoView();
		ms_pSelf->UpdateBtnTheme(Relation->nRelation);
		ms_pSelf->m_Relation = (TONG_MEMBER_FIGURE)Relation->nRelation;
   		ms_pSelf->UpdateListCheckButton(enumTONG_FIGURE_DIRECTOR);

		ms_pSelf->PrepareData();
	}
}


/*********************************************************************
* 功能：根据m_nRelation的值取得信息
**********************************************************************/
void KUiTongManager::PrepareData(BOOL bIsRefresh)
{
	if(m_Relation == enumTONG_FIGURE_MASTER || m_Relation == enumTONG_FIGURE_DIRECTOR)
	{
		if(bIsRefresh)
		{
			Refresh(enumTONG_FIGURE_MANAGER);
		}
		else
		{
			m_CaptainData.Clear();
			LoadData(enumTONG_FIGURE_MANAGER);
		}
	}
}


/*********************************************************************
* 功能：把新到的列表信息存到临时文件
**********************************************************************/
void KUiTongManager::SaveNewData(KUiGameObjectWithName *ParamInfo, KTongMemberItem *pIncome)
{
	char szSection[32];
	KIniFile Ini;

	if(ParamInfo->nData == enumTONG_FIGURE_DIRECTOR)
	{
		sprintf(szSection, "%s_Director", ParamInfo->szName);
	}
	else if(ParamInfo->nData == enumTONG_FIGURE_MANAGER)
	{
		sprintf(szSection, "%s_Manager", ParamInfo->szName);
	}
	else if(ParamInfo->nData == enumTONG_FIGURE_MEMBER)
	{
		sprintf(szSection, "%s_Member", ParamInfo->szName);
	}
	if(Ini.Load(TONG_DATA_TEMP_FILE))
	{
		int nCount, nDataCount;
		char szKey[32];

		Ini.GetInteger(szSection, "Count", 0, &nCount);
		nDataCount = nCount + ParamInfo->nParam;
		for(int i = nCount;i < nDataCount;i++)
		{
			sprintf(szKey, "%d_Name", i);
			Ini.WriteString(szSection, szKey, pIncome[i - nCount].Name);

			sprintf(szKey, "%d_AgName", i);
			Ini.WriteString(szSection, szKey, pIncome[i - nCount].szAgname);

			sprintf(szKey, "%d_Hash", i);
			Ini.WriteInteger(szSection, szKey, StringToHash(pIncome[i - nCount].Name));
		}
		Ini.Save(TONG_DATA_TEMP_FILE);
	}
}


/*********************************************************************
* 功能：把指定类型的列表数据整个存储起来
**********************************************************************/
void KUiTongManager::SaveWholeData(int nType)
{
	if(ms_pSelf && ms_pSelf->m_TongData.szName[0])
	{
    	KLinkStruct<KPilgarlicItem> *pData = NULL;
		char szSection[32];
		switch(nType)
		{
		case enumTONG_FIGURE_DIRECTOR:
			pData = &ms_pSelf->m_ElderData;
			sprintf(szSection, "%s_Director", ms_pSelf->m_TongData.szName);
			break;

		case enumTONG_FIGURE_MANAGER:
			pData = &ms_pSelf->m_CaptainData;
			sprintf(szSection, "%s_Manager", ms_pSelf->m_TongData.szName);
			break;

		case enumTONG_FIGURE_MEMBER:
			pData = &ms_pSelf->m_MemberData;
			sprintf(szSection, "%s_Member", ms_pSelf->m_TongData.szName);
			break;
		}
		if(pData)
		{
			KPilgarlicItem *pNode = pData->Reset();
			int nCount = pData->GetCount();
			char szKey[32];
			KIniFile Ini;
		/*----------------------------------*/
			Ini.Load(TONG_DATA_TEMP_FILE);
			Ini.EraseSection(szSection);
			for(int i = 0;i < nCount;i++)
			{
				sprintf(szKey, "%d_Name", i);
			    Ini.WriteString(szSection, szKey, pNode->Name);

			    sprintf(szKey, "%d_AgName", i);
			    Ini.WriteString(szSection, szKey, pNode->szAgname);

			    sprintf(szKey, "%d_Hash", i);
			    Ini.WriteInteger(szSection, szKey, pNode->nHashID);

				pNode = pData->NextItem();
			}
		}
	}
}


/*********************************************************************
* 功能：载入资料
**********************************************************************/
void KUiTongManager::LoadData(int nType)
{
	if(m_TongData.szName[0])
	{
    	char szSection[32];

	    switch(nType)
	    {
	    case enumTONG_FIGURE_DIRECTOR:
    		sprintf(szSection, "%s_Director", m_TongData.szName);
		    if(!(m_nElderDataIndex = LoadDataHandler(&m_ElderData, szSection)) &&
				m_nElderDataIndex < defTONG_MAX_DIRECTOR)
			{
				m_nElderDataIndex = 0;
    			RequestData(enumTONG_FIGURE_DIRECTOR, m_nElderDataIndex);
			}
		    break;

	    case enumTONG_FIGURE_MANAGER:
    		sprintf(szSection, "%s_Manager", m_TongData.szName);
		    if(!(m_nCaptainDataIndex = LoadDataHandler(&m_CaptainData, szSection)))
			{
				m_nCaptainDataIndex = 0;
    			RequestData(enumTONG_FIGURE_MANAGER, m_nCaptainDataIndex);
			}
		    break;

	    case enumTONG_FIGURE_MEMBER:
    		sprintf(szSection, "%s_Member", m_TongData.szName);
		    if(!(m_nMemberDataIndex = LoadDataHandler(&m_MemberData, szSection)))
			{
				m_nMemberDataIndex = 0;
    			RequestData(enumTONG_FIGURE_MEMBER, m_nMemberDataIndex);
			}
		    break;

		case TONG_LIST_TYPE_LEAGUE:
			sprintf(szSection, "%s_League", m_TongData.szName);
		    if(!(m_nTongLeagueDataIndex = LoadDataHandler(&m_CaptainData, szSection)))
			{
				m_nTongLeagueDataIndex = 0;
    			RequestData(TONG_LIST_TYPE_LEAGUE, m_nTongLeagueDataIndex);
			}
			break;

		default:
			break;
	    }
	}
}


/*********************************************************************
* 功能：载入资料的处理函数
**********************************************************************/
int KUiTongManager::LoadDataHandler(KLinkStruct<KPilgarlicItem> *pData,
									const char* szSection)
{
	char szFileName[256];
	KIniFile Ini;

	sprintf(szFileName, "%s\\%s", g_UiBase.GetUserTempDataFolder(), TONG_DATA_TEMP_FILE);
	if(Ini.Load(szFileName) && m_TongData.szName[0] && Ini.IsSectionExist(szSection))
	{
		int nCount, i;
		char szKey[32];
		KPilgarlicItem AddItem;

		pData->Clear();
		Ini.GetInteger(szSection, "Count", 0, &nCount);

		for(i = 0;i < nCount;i++)
		{
			sprintf(szKey, "%d_Name", i);
			Ini.GetString(szSection, szKey, "", AddItem.Name, sizeof(AddItem.Name));

			sprintf(szKey, "%d_AgName", i);
			Ini.GetString(szSection, szKey, "", AddItem.szAgname, sizeof(AddItem.szAgname));

			sprintf(szKey, "%d_Hash", i);
			Ini.GetInteger(szSection, szKey, 0, &AddItem.nHashID);

			pData->AddItem(AddItem);
		}
		UpdateTongListView();
		return nCount;
	}
	return 0;
}


/*********************************************************************
* 功能：向Core发出数据请求
**********************************************************************/
void KUiTongManager::RequestData(int nType, int nIndex)
{
	if(g_pCoreShell)
	{
    	KUiGameObjectWithName ParamInfo;
    	strcpy(ParamInfo.szName, m_TongData.szName);
    	ParamInfo.nData = nType;
    	ParamInfo.nParam = nIndex;
    	g_pCoreShell->TongOperation(GTOI_REQUEST_TONG_DATA, (unsigned int)&ParamInfo, 0);
	}
}


/*********************************************************************
* 功能：删除临时文件
**********************************************************************/
int KUiTongManager::RemoveTempFile()
{
	char szPath[256], szFile[128];

	sprintf(szFile, "%s\\%s", g_UiBase.GetUserTempDataFolder(), TONG_DATA_TEMP_FILE);
	g_GetFullPath(szPath, szFile);
	return remove(szPath);
}


/*********************************************************************
* 功能：各种帮会操作的处理函数
**********************************************************************/
void KUiTongManager::OnAction(int nType, int nbForceBondingName)
{
	if(g_pCoreShell)
	{
		KPilgarlicItem *pNode = GetSelectedPlayer();
		KTongOperationParam Param;
		KTongMemberItem Player;

		memset(&Player, 0, sizeof(KTongMemberItem));
		Param.eOper = (TONG_ACTION_TYPE)nType;
		if(nType == TONG_ACTION_LEAVE)
		{
			Param.nData[0] = m_nRelation;
			Param.nData[1] = -1;
		}
		else
		{
			Param.nData[0] = m_nCurrentCheckBox;
			Param.nData[1] = m_nIndex;
		}
		strcpy(Param.szPassword, m_szPassword);

		if(nbForceBondingName)
		{
			strcpy(Player.Name, m_szPlayerName);
		}
		else if(m_szTargetPlayerName[0])
		{
			strcpy(Player.Name, m_szTargetPlayerName);
		}
	    else if(pNode && pNode->Name[0])
	    {
		    strcpy(Player.Name, pNode->Name);
	    }
		else
		{
			strcpy(Player.Name, m_szPlayerName);
		}
		g_pCoreShell->TongOperation(GTOI_TONG_ACTION, (unsigned int)&Param, (int)&Player);
		memset(m_szPassword, 0, sizeof(m_szPassword));
	}
}


//带有参数的帮会操作函数
void KUiTongManager::OnParamAction(int nType, unsigned int uParam, int nParam)
{
	_ASSERT(g_pCoreShell);
	KTongOperationParam Param;
	KTongMemberItem Player;

	memset(&Param, 0, sizeof(KTongOperationParam));
	memset(&Player, 0, sizeof(KTongMemberItem));

	Param.eOper = (TONG_ACTION_TYPE)nType;
	Param.nData[0] = uParam;
	Param.nData[1] = nParam;

	g_pCoreShell->TongOperation(GTOI_TONG_ACTION, (unsigned int)&Param, (int)&Player);
}


/*********************************************************************
* 功能：响应刷新按钮的操作函数
**********************************************************************/
void KUiTongManager::OnRefresh()
{
	if(m_TongData.szName[0])
	{
		Refresh(m_nCurrentCheckBox);
	}
}


void KUiTongManager::Refresh(int nType)
{
	KIniFile Ini;
	char szSection[32];

	if(nType == enumTONG_FIGURE_DIRECTOR)
	{
		sprintf(szSection, "%s_Director", m_TongData.szName);
		m_ElderData.Clear();
		m_nElderDataIndex = 0;
	}
	else if(nType == enumTONG_FIGURE_MANAGER)
	{
		sprintf(szSection, "%s_Manager", m_TongData.szName);
		m_CaptainData.Clear();
		m_nCaptainDataIndex = 0;
	}
	else if(nType == enumTONG_FIGURE_MEMBER)
	{
		sprintf(szSection, "%s_Member", m_TongData.szName);
		m_MemberData.Clear();
		m_nMemberDataIndex = 0;
	}
	else if(nType == TONG_LIST_TYPE_LEAGUE)
	{
		sprintf(szSection, "%s_League", m_TongData.szName);
		m_LeagueData.Clear();
		m_nTongLeagueDataIndex = 0;
	}
	else
	{
		return;
	}

	if(Ini.Load(TONG_DATA_TEMP_FILE))
	{
		Ini.EraseSection(szSection);
		Ini.Save(TONG_DATA_TEMP_FILE);
	}

	m_ListLeft.ResetContent();
	m_ListRight.ResetContent();
	LoadData(nType);
}


/*********************************************************************
* 功能：响应任命的操作
**********************************************************************/
void KUiTongManager::OnAssign()
{
	KPilgarlicItem *pNode = GetSelectedPlayer();
	KTongMemberItem *pData = NULL, *pManData = NULL;

	int nCount = FormArrayData(&pData, enumTONG_FIGURE_DIRECTOR);
	int nManCount = FormArrayData(&pManData, enumTONG_FIGURE_MANAGER);

	KUiTongAssignBox *pAssignBox = KUiTongAssignBox::OpenWindow();
	KUiTongAssignBox::LinkToMainWindow(this);
	KUiTongAssignBox::ArrangeData(m_nRelation, pNode->Name, pNode->szAgname, m_nCurrentCheckBox, m_nIndex);
	if(pAssignBox && pData)
	{
		pAssignBox->InjectData(pData, nCount, enumTONG_FIGURE_DIRECTOR);
		pAssignBox->InjectData(pManData, nManCount, enumTONG_FIGURE_MANAGER);
	}
	if(pData)
	{
		delete pData;
	}
	if(pManData)
	{
		delete pManData;
	}
}


/*********************************************************************
* 功能：响应发钱操作
**********************************************************************/
void KUiTongManager::OnSalary()
{
	KUiTongSalary *pSalary = KUiTongSalary::OpenWindow();

	if(pSalary)
	{
		pSalary->ClearCount();

		KTongMemberItem *pData = NULL;
		int nCount;

		pSalary->ArrangeDecree(m_TongData);

		nCount = FormArrayData(&pData, enumTONG_FIGURE_DIRECTOR);
		if(pData)
		{
			pSalary->InjectData(pData, nCount, enumTONG_FIGURE_DIRECTOR);
			delete pData;
			pData = NULL;
		}

		nCount = FormArrayData(&pData, enumTONG_FIGURE_MANAGER);
		if(pData)
		{
			pSalary->InjectData(pData, nCount, enumTONG_FIGURE_MANAGER);
			delete pData;
			pData = NULL;
		}

		nCount = FormArrayData(&pData, enumTONG_FIGURE_MANAGER);
		if(pData)
		{
			pSalary->InjectData(pData, nCount, enumTONG_FIGURE_MEMBER);
			delete pData;
			pData = NULL;
		}

		if(!m_MemberData.GetCount())
		{
			LoadData(enumTONG_FIGURE_MEMBER);
		}
	}
}


/*********************************************************************
* 功能：响应帮众称号修改的操作
**********************************************************************/
void KUiTongManager::OnMemberTitle(int nParam)
{
	KTongMemberItem Item[2];

	char szAgName[32] = "";

	if(nParam)//女
	{
		strcpy(szAgName, m_TongData.szFemaleTitle);
	}
	else//男
	{
		strcpy(szAgName, m_TongData.szMaleTitle);
	}
	strcpy(Item[0].szAgname, szAgName);
	strcpy(Item[1].szAgname, szAgName);
	Item[0].Name[0] = Item[1].Name[0] = 0;
	KUiTongAssignTitle *pTitle = KUiTongAssignTitle::OpenWindow();
	if(pTitle)
	{
		pTitle->InjectData(Item, 2, enumTONG_FIGURE_MEMBER, nParam > 0);
		pTitle->UpdateView();
	}
}


/*********************************************************************
* 功能：对结果的响应入口
**********************************************************************/
void KUiTongManager::ResponseResult(KUiGameObjectWithName *pResult, int nbIsSucceed)
{
	if(ms_pSelf)
	{
    	switch(pResult->nData)
    	{
    	case TONG_ACTION_DISMISS:
			ms_pSelf->ResponseDismiss(pResult, nbIsSucceed);
		    break;

	    case TONG_ACTION_DEMISE:
			ms_pSelf->ResponseDemise(pResult, nbIsSucceed);
    		break;

    	case TONG_ACTION_LEAVE:
			ms_pSelf->ResponseLeave(pResult, nbIsSucceed);
		    break;

		case TONG_ACTION_ASSIGN:
			ms_pSelf->ResponseAssign(pResult, nbIsSucceed);
		    break;

		case TONG_ACTION_APPLY:
			ms_pSelf->ResponseApply(pResult, nbIsSucceed);
			break;

		case TONG_ACTION_TITLE:
			ms_pSelf->ResponseTitle(pResult, nbIsSucceed);
			break;

		case TONG_ACTION_CHANGE_CAMP:
			ms_pSelf->ResponseCamp(pResult, nbIsSucceed);
			break;

		case TONG_ACTION_CREATE_LEAGUE:
			ms_pSelf->ResponseCreateLeague(pResult, nbIsSucceed);
			break;

		case TONG_ACTION_DISMISS_LEAGUE:
			ms_pSelf->ResponseDismisLeague(pResult, nbIsSucceed);
			break;

		case TONG_ACTION_DEPOSIT_MONEY:
			ms_pSelf->ResponseDepositMoney(pResult, nbIsSucceed);
			break;

		case TONG_ACTION_TAKE_MONEY:
			ms_pSelf->ResponseTakeMoney(pResult, nbIsSucceed);
			break;

		case TONG_ACTION_LEAGUE_ACTION:
			ms_pSelf->ResponseLeagueAction(pResult, nbIsSucceed);
			break;

		case TONG_ACTION_SALARY:
			ms_pSelf->ResponseSalary(pResult, nbIsSucceed);
			break;
	    }
	}
	else
	{
		RemoveTempFile();
	}
}


/*********************************************************************
* 功能：响应离帮后的操作
**********************************************************************/
void KUiTongManager::ResponseLeave(KUiGameObjectWithName *pResult, int nbIsSucceed)
{
	if(nbIsSucceed)
	{
    	RemoveTempFile();
    	CloseWindow();
	}
}


/*********************************************************************
* 功能：响应踢人后的操作
**********************************************************************/
void KUiTongManager::ResponseDismiss(KUiGameObjectWithName *pResult, int nbIsSucceed)
{
	if(nbIsSucceed)
	{
		if(strcmp(pResult->szName, m_szPlayerName) == 0)
		{
			RemoveTempFile();
			CloseWindow();
		}
		else
		{
			KLinkStruct<KPilgarlicItem> *pForDelete;
			pForDelete = FindPlayer(pResult->szName);
			if(pForDelete)
			{
				if(pResult->nParam == enumTONG_FIGURE_MEMBER)
				{
					pForDelete->RemoveItem();
					m_TongData.nMemberCount --;
				}
				else
				{
					if(pResult->nParam == enumTONG_FIGURE_DIRECTOR)
					{
						m_TongData.nDirectorCount --;
					}
					else
					{
						m_TongData.nManagerCount --;
					}
					KPilgarlicItem *pNode = pForDelete->Current();
					pNode->Name[0] = 0;
					pNode->nData   = 0;
					pNode->nHashID = 0;
					pNode->nIndex  = 0;
					pNode->uId     = 0;
				}
				SaveWholeData(pResult->nParam);
				UpdateTongListView(TRUE);
				UpdateTongInfoView();
			}
		}
	}
}


/*********************************************************************
* 功能：响应传位后的操作
**********************************************************************/
void KUiTongManager::ResponseDemise(KUiGameObjectWithName *pResult, int nbIsSucceed)
{
	if(nbIsSucceed)
	{
		RemoveTempFile();
		ArrangeComposition(NULL);
	}
}


/*********************************************************************
* 功能：响应传位后的操作
**********************************************************************/
void KUiTongManager::ResponseAssign(KUiGameObjectWithName *pResult, int nbIsSucceed)
{
	if(nbIsSucceed)
	{
		int nNewFigure, nNewPos, nOldFigure, nOldPos;
		nNewFigure = HIWORD(pResult->nParam);
		nNewPos    = LOWORD(pResult->nParam);
		nOldFigure = HIWORD(pResult->uParam);
		nOldPos    = LOWORD(pResult->uParam);
		if(!strcmp(pResult->szName, m_szPlayerName))
		{
			RemoveTempFile();
		    ArrangeComposition(NULL);
		}
		else
		{
			KLinkStruct<KPilgarlicItem> *pPlayer = FindPlayer(pResult->szName);
			if(pPlayer)
			{
				char szBuf[32];
				KPilgarlicItem Add, *pNode = pPlayer->Current();
				memcpy(&Add, pNode, sizeof(KPilgarlicItem));
				if(nOldFigure == enumTONG_FIGURE_MEMBER)
					pPlayer->RemoveItem();
				else
				{
					strcpy(szBuf, pNode->szAgname);
					memset(pNode, 0, sizeof(KTongMemberItem));
					strcpy(pNode->szAgname, szBuf);
				}
				switch(nNewFigure)
				{
				case enumTONG_FIGURE_DIRECTOR:
					pNode = m_ElderData.GetItem(nNewPos);
					if(pNode)
					{
						strcpy(szBuf, pNode->szAgname);
						memcpy(pNode, &Add, sizeof(KTongMemberItem));
						strcpy(pNode->szAgname, szBuf);
					}
					break;

				case enumTONG_FIGURE_MANAGER:
					pNode = m_CaptainData.GetItem(nNewPos);
					if(pNode)
					{
						strcpy(szBuf, pNode->szAgname);
						memcpy(pNode, &Add, sizeof(KTongMemberItem));
						strcpy(pNode->szAgname, szBuf);
					}
					break;

				case enumTONG_FIGURE_MEMBER:
					strcpy(Add.szAgname, pResult->szString);
					m_MemberData.AddItem(Add);
					break;

				default:
					return;
				}
				SaveWholeData(nOldFigure);
				SaveWholeData(nNewFigure);
				UpdateTongListView(TRUE);
			}
			//////////
		}
		/////////
	}
	////////
}


/*********************************************************************
* 功能：响应申请后的操作
**********************************************************************/
void KUiTongManager::ResponseApply(KUiGameObjectWithName *pResult, int nbIsSucceed)
{
	if(nbIsSucceed)
	{
		if(!strcmp(m_szPlayerName, pResult->szName))
		{
			RemoveTempFile();
			ArrangeComposition(NULL);
		}
		else
		{
			KPilgarlicItem Add;
			strcpy(Add.Name, pResult->szName);
			strcpy(Add.szAgname, pResult->szString);
			Add.nHashID = StringToHash(Add.Name);
			m_MemberData.AddItem(Add);
			SaveWholeData(enumTONG_FIGURE_MEMBER);
			UpdateTongListView(TRUE);
		}
	}
}


  /*__(@_            功能：响应改变称号后的操作
 /     ) \______________________________________________________
(_)@8@8{}<______________________________________________________>
       )_/
      (*/
void KUiTongManager::ResponseTitle(KUiGameObjectWithName *pResult, int nbIsSucceed)
{
	KLinkStruct<KPilgarlicItem> *pData;
	KPilgarlicItem              *pNode;
	switch(pResult->nParam)
	{
	case enumTONG_FIGURE_DIRECTOR:
		pData = &m_ElderData;
		break;

	case enumTONG_FIGURE_MANAGER:
		pData = &m_CaptainData;
		break;

	case enumTONG_FIGURE_MEMBER:
		if(pResult->uParam)
		{
			strcpy(m_TongData.szFemaleTitle, pResult->szString);
		}
		else
		{
			strcpy(m_TongData.szMaleTitle, pResult->szString);
		}
		UpdateTongInfoView();
		Refresh(enumTONG_FIGURE_MEMBER);
		return;
		break;

	default:
		return;
		break;
	}
	pNode = pData->GetItem(pResult->uParam);
	if(pNode)
	{
		strcpy(pNode->szAgname, pResult->szString);
	}
	UpdateTongListView(TRUE);
}


/*********************************************************************
* 功能：响应改变阵营后的操作
**********************************************************************/
void KUiTongManager::ResponseCamp(KUiGameObjectWithName *pResult, int nbIsSucceed)
{
	if(nbIsSucceed && !strcmp(pResult->szName, m_TongData.szName))
	{
		m_TongData.nFaction = pResult->nParam;
		UpdateTongInfoView();
	}
}


/*********************************************************************
* 功能：响应创建联盟后的操作
**********************************************************************/
void KUiTongManager::ResponseCreateLeague(KUiGameObjectWithName *pResult, int nbIsSucceed)
{
	if(nbIsSucceed)
	{
		m_TongData.btUnionLeader = 1;
		m_TongData.m_btUnionFlag = 1;
		m_TongData.m_btUnionNum  = 1;
		strcpy(m_TongData.szUnionName, pResult->szString);
		if(m_nCurrentCheckBox == TONG_LIST_TYPE_LEAGUE)
		{
			UpdateThemeForList(TONG_LIST_TYPE_LEAGUE);
		}
		UpdateTongInfoView();
	}
}


/*********************************************************************
* 功能：响应解散联盟后的操作
**********************************************************************/
void KUiTongManager::ResponseDismisLeague(KUiGameObjectWithName *pResult, int nbIsSucceed)
{
	if(nbIsSucceed)
	{
		m_TongData.btUnionLeader = 0;
		m_TongData.m_btUnionFlag = 0;
		m_TongData.m_btUnionNum  = 0;
		m_TongData.szUnionName[0] = 0;
		if(m_nCurrentCheckBox == TONG_LIST_TYPE_LEAGUE)
		{
			UpdateThemeForList(TONG_LIST_TYPE_LEAGUE);
		}
		UpdateTongInfoView();
	}
}


/*********************************************************************
* 功能：响应存钱后的操作
**********************************************************************/
void KUiTongManager::ResponseDepositMoney(KUiGameObjectWithName *pResult, int nbIsSucceed)
{
	ArrangeComposition(m_szPlayerName);
}


/*********************************************************************
* 功能：响应取钱后的操作
**********************************************************************/
void KUiTongManager::ResponseTakeMoney(KUiGameObjectWithName *pResult, int nbIsSucceed)
{
	ArrangeComposition(m_szPlayerName);
}


/*********************************************************************
* 功能：联盟的一般操作的响应入口
**********************************************************************/
void KUiTongManager::ResponseLeagueAction(KUiGameObjectWithName *pResult, int nbIsSucceed)
{
	ArrangeComposition(m_szPlayerName);
}


/*********************************************************************
* 功能：响应发奖金后的操作
**********************************************************************/
void KUiTongManager::ResponseSalary(KUiGameObjectWithName *pResult, int nbIsSucceed)
{
	__int64 *pn64Money = (__int64 *)&m_TongData.nLoMoney;
	pn64Money -= pResult->nParam;
	UpdateTongInfoView();
}


/*********************************************************************
* 功能：查询特定名字的玩家的数据位置
**********************************************************************/
KLinkStruct<KPilgarlicItem>* KUiTongManager::FindPlayer(char *pszName)
{
	KLinkStruct<KPilgarlicItem> *pData = NULL;
	if(pszName && pszName[0])
	{
    	pData = HandleFind(&m_MemberData, pszName);
    	if(!pData)
    	{
		    pData = HandleFind(&m_CaptainData, pszName);
		    if(!pData)
		    {
    			pData = HandleFind(&m_ElderData, pszName);
		    }
	    }
	}
	return pData;
}


/*********************************************************************
* 功能：查找玩家处理函数
**********************************************************************/
KLinkStruct<KPilgarlicItem>* KUiTongManager::HandleFind(KLinkStruct<KPilgarlicItem> *pData, char *pszName)
{
	KLinkStruct<KPilgarlicItem> *pReturn = NULL;
	KPilgarlicItem *pNode = pData->Reset();
	int nID = StringToHash(pszName);
	int nCount = pData->GetCount();

	for(int i = 0;i < nCount;i++)
	{
		pNode->nHashID = StringToHash(pNode->Name);
		if(nID == pNode->nHashID)
		{
			if(strcmp(pszName, pNode->Name) == 0)
			{
				pReturn = pData;
				break;
			}
		}
		pNode = pData->NextItem();
	}
	return pReturn;
}


  /*__(@_           功能：把链表数据组织成数组数据
 /     ) \______________________________________________________
(_)@8@8{}<______________________________________________________>
       )_/
      (*/
int KUiTongManager::FormArrayData(KTongMemberItem **pData, int nType)
{
	KLinkStruct<KPilgarlicItem> *pShzi;

	switch(nType)
	{
	case enumTONG_FIGURE_DIRECTOR:
		pShzi = &m_ElderData;
		break;

	case enumTONG_FIGURE_MANAGER:
		pShzi = &m_CaptainData;
		break;

	case enumTONG_FIGURE_MEMBER:
		return 0;
		break;
	default:
		return 0;
		break;
	}

	int nCount = pShzi->GetCount();
	if(nCount > 0)
	{
		*pData = new KTongMemberItem[nCount];
		if(*pData)
		{
			KPilgarlicItem *pNode = pShzi->Begin();
			KTongMemberItem *pTemp = *pData;
			for(int i = 0;i < nCount;i++)
			{
				memcpy(pTemp + i, pNode, sizeof(KTongMemberItem));
				pNode = pShzi->NextItem();
			}
			return nCount;
		}
	}
	return 0;
}


  /*__(@_        功能：弹出帮众称号更改的性别选择菜单
 /     ) \______________________________________________________
(_)@8@8{}<______________________________________________________>
       )_/
      (*/
void KUiTongManager::OnPopupSelectSex(int nX, int nY)
{
	KPopupMenuData* pMenuData = (KPopupMenuData*)malloc(MENU_DATA_SIZE(2));
	if (pMenuData)
	{
		KPopupMenu::InitMenuData(pMenuData, 2);

		strcpy(pMenuData->Items[0].szData, "男性");
		pMenuData->Items[0].uDataLen = strlen(pMenuData->Items[0].szData);
		strcpy(pMenuData->Items[1].szData, "女性");
		pMenuData->Items[1].uDataLen = strlen(pMenuData->Items[1].szData);

		pMenuData->nX = nX;
		pMenuData->nY = nY;
		KPopupMenu::Popup(pMenuData, (KWndWindow*)this, RESULT_T_SELECT_SEX);
	}
}


unsigned long StringToHash(const char *pString, BOOL bIsCaseSensitive)
{
	if(pString && pString[0])
	{
		unsigned long id = 0;
		const char *ptr;
		int index = 0;

		if(bIsCaseSensitive)
		{
			ptr = pString;

			while(*ptr)
			{
    			id = (id + (++index) * (*ptr)) % 0x8000000b * 0xffffffef;
		        ptr++;
	        }
		}
		else
		{
			char Buff[256];
			strcpy(Buff, pString);
			strlwr(Buff);
			ptr = Buff;

        	while(*ptr)
		    {
    			id = (id + (++index) * (*ptr)) % 0x8000000b * 0xffffffef;
		        ptr++;
	        }
		}
		return (id ^ 0x12345678);
	}

	return 0;
}


  /*__(@_       功能：根据列表的选择和当前界面状态运算出
 /     ) \______________________________________________________
(_)@8@8{}<______________________________________________________>
       )_/                  对于数据的index
      (*/
int	KUiTongManager::WorkoutIndex(int nSelectIndex, int nbLeft)
{
	KLinkStruct<KPilgarlicItem> *pData = GetCurrentSelectedData();
	int nRet = -1;
	int nRealSelectIndex;

	if(nbLeft)
	{
		nRealSelectIndex = nSelectIndex * 2;
	}
	else
	{
		nRealSelectIndex = nSelectIndex * 2 + 1;
	}
	if(m_uStatus & TONG_MANAGER_STATUS_SHOW_ALL_MEMBER)
	{
		nRet = nRealSelectIndex;
	}
	else
	{
		int nIndex = -1;
		int i = -1;
		if(pData)
		{
			int nCount = pData->GetCount();
			KPilgarlicItem *pNode = pData->Reset();
			for(i = 0;i < nCount;i++)
			{
				if(pNode->Name[0])
					nIndex ++;
				if(nIndex >= nRealSelectIndex)
				{
					nRet = i;
					break;
				}
				pNode = pData->NextItem();
			}
		}
	}
	return nRet;
}
