/*****************************************************************************************
//	界面--组队管理界面
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-9-2
*****************************************************************************************/
#include "KWin32.h"
#include "KIniFile.h"
#include "../Elem/WndMessage.h"
#include "../elem/wnds.h"
#include "UiTeamManage.h"
#include "UiInformation.h"
#include "UiPlayerBar.h"
#include "../UiSoundSetting.h"
#include "../../../core/src/CoreShell.h"
#include "../UiBase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

extern iCoreShell*		g_pCoreShell;

#define MANAGE_INI_TEAM			 "队伍管理.ini"

KUiTeamManage* KUiTeamManage::m_pSelf = NULL;

//--------------------------------------------------------------------------
//	功能：构造函数
//--------------------------------------------------------------------------
KUiTeamManage::KUiTeamManage()
{
	memset(&m_Info, 0, sizeof(KUiPlayerTeam));
	m_pPlayersList = NULL;
	m_pNearbyPlayersList = NULL;
//	m_pNearbyTeamsList = NULL;
}

//--------------------------------------------------------------------------
//	功能：如果窗口正被显示，则返回实例指针
//--------------------------------------------------------------------------
KUiTeamManage* KUiTeamManage::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	return NULL;
}

//--------------------------------------------------------------------------
//	功能：打开窗口，返回唯一的一个类对象实例
//--------------------------------------------------------------------------
KUiTeamManage* KUiTeamManage::OpenWindow()
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiTeamManage;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
	{
		UiSoundPlay(UI_SI_WND_OPENCLOSE);
		m_pSelf->UpdateData(NULL);
		m_pSelf->BringToTop();
		m_pSelf->Show();
	}
	return m_pSelf;
}

//--------------------------------------------------------------------------
//	功能：关闭窗口，同时可以选则是否删除对象实例
//--------------------------------------------------------------------------
void KUiTeamManage::CloseWindow()
{
	if (m_pSelf)
	{
		m_pSelf->Clear();
		m_pSelf->Destroy();
		m_pSelf = NULL;
	}
}

//--------------------------------------------------------------------------
//	功能：初始化
//--------------------------------------------------------------------------
void KUiTeamManage::Initialize()
{
	AddChild(&m_LeaderAbility);
//	AddChild(&m_NearbyPlayerTab);
//	AddChild(&m_NearbyTeamTab);
	AddChild(&m_TeamMember);
	AddChild(&m_NearbyList);
	AddChild(&m_MemberScroll);
	AddChild(&m_NearbyScroll);

	AddChild(&m_btnInvite);
	AddChild(&m_btnKick);
//	AddChild(&m_btnApplyJoin);

	AddChild(&m_btnAppoint);
	AddChild(&m_btnLeave);
	AddChild(&m_btnDismiss);
	AddChild(&m_btnRefresh);
	AddChild(&m_btnCloseTeam);
	AddChild(&m_btnCancel);

	m_TeamMember.SetScrollbar(&m_MemberScroll);
	m_NearbyList.SetScrollbar(&m_NearbyScroll);
	m_MemberScroll.Enable(false);
	m_NearbyScroll.Enable(false);
//	m_btnApplyJoin.Hide();

	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);

//	m_NearbyPlayerTab.CheckButton(true);
//	m_NearbyTeamTab.CheckButton(false);

	Wnd_AddWindow(this);
}

//--------------------------------------------------------------------------
//	功能：载入窗口的界面方案
//--------------------------------------------------------------------------
void KUiTeamManage::LoadScheme(class KIniFile* pSetting)
{
	_ASSERT(pSetting);
	Init(pSetting, "Main");
	m_LeaderAbility .Init(pSetting, "LeaderAbility");
//	m_NearbyPlayerTab.Init(pSetting, "NearbyPlayerTab");
//	m_NearbyTeamTab	.Init(pSetting, "NearbyTeamTab");
	m_TeamMember	.Init(pSetting, "MemberList");
	m_NearbyList	.Init(pSetting, "NearbyList");
	m_MemberScroll	.Init(pSetting, "MemberScroll");
	m_NearbyScroll	.Init(pSetting, "NearbyScroll");

	m_btnInvite		.Init(pSetting, "Invite");
	m_btnKick		.Init(pSetting, "Kick");
//	m_btnApplyJoin	.Init(pSetting, "ApplyJoin");
	m_btnAppoint	.Init(pSetting, "Appoint");
	m_btnLeave		.Init(pSetting, "Leave");
	m_btnDismiss	.Init(pSetting, "Dismiss");
	m_btnRefresh	.Init(pSetting, "Refresh");
	m_btnCloseTeam	.Init(pSetting, "CloseTeam");
	m_btnCancel		.Init(pSetting, "Cancel");
}

//载入界面方案
void KUiTeamManage::LoadScheme(const char* pScheme)
{
	if (m_pSelf && pScheme)
	{
		char		Buff[128];
		KIniFile	Ini;
		sprintf(Buff, "%s\\%s", pScheme, MANAGE_INI_TEAM);
		if (Ini.Load(Buff))
			m_pSelf->LoadScheme(&Ini);
	}
}

//--------------------------------------------------------------------------
//	功能：窗口函数
//--------------------------------------------------------------------------
int KUiTeamManage::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	switch(uMsg)
	{
	case WND_N_BUTTON_CLICK:
		if (uParam == (unsigned int)(KWndWindow*)&m_btnInvite)
			OnInvite();
		else if (uParam == (unsigned int)(KWndWindow*)&m_btnKick)
			OnKick();
//		else if (uParam == (unsigned int)(KWndWindow*)&m_btnApplyJoin)
//			OnApplyJoin();
		if (uParam == (unsigned int)(KWndWindow*)&m_btnAppoint)
			OnAppoint();
		else if (uParam == (unsigned int)(KWndWindow*)&m_btnLeave ||
			uParam == (unsigned int)(KWndWindow*)&m_btnDismiss)
			OnLeave();
		else if (uParam == (unsigned int)(KWndWindow*)&m_btnRefresh)
			OnRefresh();
//		else if (uParam == (unsigned int)(KWndWindow*)&m_NearbyPlayerTab)
//			SwitchNearbyMode(true);
//		else if (uParam == (unsigned int)(KWndWindow*)&m_NearbyTeamTab)
//			SwitchNearbyMode(false);
		else if (uParam == (unsigned int)(KWndWindow*)&m_btnCloseTeam)
			g_pCoreShell->TeamOperation(TEAM_OI_CLOSE, 0, nParam);
		else if (uParam == (unsigned int)(KWndWindow*)&m_btnCancel)
			CloseWindow();
		break;
	case WND_N_LIST_ITEM_SEL:
		if (uParam == (unsigned int)(KWndWindow*)&m_TeamMember)
			OnSelTeamMember(nParam);
		else if (uParam == (unsigned int)(KWndWindow*)&m_NearbyList)
			OnSelNearby(nParam);
		break;
	case WND_N_SCORLLBAR_POS_CHANGED:
		if (uParam == (unsigned int)(KWndWindow*)&m_MemberScroll)
			m_TeamMember.SetTopItemIndex(nParam);
		else if (uParam == (unsigned int)(KWndWindow*)&m_NearbyScroll)
			m_NearbyList.SetTopItemIndex(nParam);
		break;
	default:
		return KWndShowAnimate::WndProc(uMsg, uParam, nParam);
	}
	return 0;
}

void KUiTeamManage::OnSelTeamMember(int nItem)
{
	bool bEnable = false;
	const char* pcszSelfName = KUiPlayerBar::SelfName();
	if (m_Info.bTeamLeader && pcszSelfName && nItem >= 0)
	{
		bEnable = (strcmp(pcszSelfName, m_pPlayersList[nItem].Name) != 0);
	}
	m_btnKick.Enable(bEnable);
	m_btnAppoint.Enable(bEnable);
}

void KUiTeamManage::OnSelNearby(int nItem)
{
	bool bEnable = false;
//	if (m_NearbyPlayerTab.IsButtonChecked())
	{
		if (nItem >= 0 &&
			((m_Info.cNumMember && m_Info.bTeamLeader) || m_Info.cNumMember == 0))
		{
			bEnable = true;
		}
		m_btnInvite.Enable(bEnable);
	}
//	else if (m_NearbyTeamTab.IsButtonChecked())
//	{
//		if (nItem >= 0 && m_Info.cNumMember == 0)
//			bEnable = true;
//		m_btnApplyJoin.Enable(bEnable);
//	}
}

void KUiTeamManage::Clear()
{
	m_TeamMember.SetContent(0, 0, 0, 0);
	m_NearbyList.SetContent(0, 0, 0, 0);

	m_btnInvite.Enable(false);
	m_btnKick.Enable(false);
//	m_btnApplyJoin.Enable(false);

	m_btnAppoint.Enable(false);
	m_btnLeave.Enable(false);
	m_btnDismiss.Enable(false);
	m_btnCloseTeam.CheckButton(false);

	memset(&m_Info, 0, sizeof(KUiPlayerTeam));
	if (m_pPlayersList)
	{
		free(m_pPlayersList);
		m_pPlayersList = NULL;
	}
	if (m_pNearbyPlayersList)
	{
		free (m_pNearbyPlayersList);
		m_pNearbyPlayersList = NULL;
	}
//	if (m_pNearbyTeamsList)
//	{
//		free (m_pNearbyTeamsList);
//		m_pNearbyTeamsList = NULL;
//	}
}

//--------------------------------------------------------------------------
//	功能：更新队伍信息
//--------------------------------------------------------------------------
void KUiTeamManage::UpdateData(KUiPlayerTeam* pInfo)
{
	Clear();

	KUiPlayerLeaderShip	LeaderShip;
	memset(&LeaderShip, 0, sizeof(KUiPlayerLeaderShip));
	g_pCoreShell->GetGameData(GDI_PLAYER_LEADERSHIP,
		(unsigned int)&LeaderShip, 0);
	m_LeaderAbility.SetIntText(LeaderShip.nLeaderShipLevel);

	if (pInfo)
		m_Info = *pInfo;
	else if (!g_pCoreShell->TeamOperation(TEAM_OI_GD_INFO, (unsigned int)&m_Info, 0))
	{
		memset(&m_Info, 0, sizeof(m_Info));
	}

	if (m_Info.bTeamLeader)
	{
		m_btnCloseTeam.CheckButton(m_Info.bOpened);

		m_btnDismiss.Show();
		m_btnDismiss.Enable(true);
		m_btnLeave.Hide();
	}
	else
	{
		m_btnLeave.Show();
		m_btnLeave.Enable(m_Info.cNumMember > 0);
		m_btnDismiss.Hide();
	}

	if (m_Info.cNumMember > 0)
	{
		m_pPlayersList = (KUiPlayerItem*)malloc(sizeof(KUiPlayerItem) * (m_Info.cNumMember));
		if (m_pPlayersList)
		{
			int nCount = g_pCoreShell->TeamOperation(TEAM_OI_GD_MEMBER_LIST,
				(unsigned int)m_pPlayersList, m_Info.cNumMember);
			_ASSERT(nCount == m_Info.cNumMember);
			m_TeamMember.SetContent((unsigned char*)m_pPlayersList, m_Info.cNumMember, sizeof(KUiPlayerItem),
				((char*)&m_pPlayersList->Name - (char*)m_pPlayersList));
		}
		else
		{
			m_Info.cNumMember = 0;
		}
	}

	OnRefresh();
}

/*void KUiTeamManage::SwitchNearbyMode(bool bNearbyPlayer)
{
	m_NearbyPlayerTab.CheckButton(bNearbyPlayer);
	m_NearbyTeamTab.CheckButton(!bNearbyPlayer);

	if (bNearbyPlayer)
	{
		m_btnKick.Show();
		m_btnApplyJoin.Hide();
	}
	else
	{
		m_btnInvite.Enable(false);
		m_btnKick.Hide();
		m_btnApplyJoin.Show();
	}

	OnRefresh();
}*/

//--------------------------------------------------------------------------
//	功能：更新周围队伍的列表
//--------------------------------------------------------------------------
/*void KUiTeamManage::UpdateNearbyTeams(KUiTeamItem* pList, int nCount)
{
	if (m_pSelf == NULL)
		return;
	if (m_pSelf->m_NearbyTeamTab.IsButtonChecked())
	{
		m_pSelf->m_NearbyList.SetContent(NULL, 0, 0, 0);
		if (m_pSelf->m_pNearbyTeamsList)
		{
			free (m_pSelf->m_pNearbyTeamsList);
			m_pSelf->m_pNearbyTeamsList = NULL;
		}
		if (pList && nCount)
		{
			m_pSelf->m_pNearbyTeamsList = (KUiTeamItem*)malloc(sizeof(KUiTeamItem) * nCount);
			if (m_pSelf->m_pNearbyTeamsList)
			{
				memcpy(m_pSelf->m_pNearbyTeamsList, pList, sizeof(KUiTeamItem) * nCount);
				m_pSelf->m_NearbyList.SetContent((unsigned char*)m_pSelf->m_pNearbyTeamsList,
					nCount, sizeof(KUiTeamItem), (int)&(((KUiTeamItem*)0)->Leader.Name));
			}
		}
	}
}
*/
//--------------------------------------------------------------------------
//	功能：任命队长
//--------------------------------------------------------------------------
void KUiTeamManage::OnAppoint()
{
	int nCurSel = m_TeamMember.GetCurSel();
	if (nCurSel >= 0)
		g_pCoreShell->TeamOperation(TEAM_OI_APPOINT, (unsigned int)&m_pPlayersList[nCurSel], 0);
}

//--------------------------------------------------------------------------
//	功能：踢除
//--------------------------------------------------------------------------
void KUiTeamManage::OnKick()
{
	int nCurSel = m_TeamMember.GetCurSel();
	if (nCurSel >= 0)
		g_pCoreShell->TeamOperation(TEAM_OI_KICK, (unsigned int)&m_pPlayersList[nCurSel], 0);
}

//--------------------------------------------------------------------------
//	功能：申请加入
//--------------------------------------------------------------------------
/*void KUiTeamManage::OnApplyJoin()
{
	int nCurSel = m_NearbyList.GetCurSel();
	if (nCurSel >= 0)
	{
		g_pCoreShell->OperationRequest(GOI_TEAM_APPLY,
			(unsigned int)&m_pNearbyTeamsList[nCurSel], 0);
	}
}*/

//--------------------------------------------------------------------------
//	功能：邀请
//--------------------------------------------------------------------------
void KUiTeamManage::OnInvite()
{
	int nCurSel = m_NearbyList.GetCurSel();
	if (nCurSel >= 0)
	{
		if (m_Info.cNumMember == 0)
			g_pCoreShell->TeamOperation(TEAM_OI_CREATE, 0, 0);
		g_pCoreShell->TeamOperation(TEAM_OI_INVITE,	(unsigned int)(&m_pNearbyPlayersList[nCurSel]), 0);
	}
}

void KUiTeamManage::OnRefresh()
{
	m_NearbyList.SetContent(NULL, 0, 0, 0);
	if (m_pNearbyPlayersList)
	{
		free (m_pNearbyPlayersList);
		m_pNearbyPlayersList = NULL;
	}
//	if (m_pNearbyTeamsList)
//	{
//		free (m_pNearbyTeamsList);
//		m_pNearbyTeamsList = NULL;
//	}

//	if (m_NearbyPlayerTab.IsButtonChecked())
	{
		int nCount = g_pCoreShell->GetGameData(GDI_NEARBY_IDLE_PLAYER_LIST, 0, 0);
		if (nCount)
		{
			m_pNearbyPlayersList = (KUiPlayerItem*)malloc(sizeof(KUiPlayerItem) * nCount);
			if (m_pNearbyPlayersList)
			{
				g_pCoreShell->GetGameData(GDI_NEARBY_IDLE_PLAYER_LIST, (unsigned int)m_pNearbyPlayersList, nCount);
				m_NearbyList.SetContent((unsigned char*)m_pNearbyPlayersList, nCount,
					sizeof(KUiPlayerItem), ((char*)&m_pNearbyPlayersList->Name - (char*)m_pNearbyPlayersList));
			}
		}
	}
//	else
//	{
//		g_pCoreShell->OperationRequest(GOI_TEAM_COLLECT_NEARBY_LIST, 0, 0);
//	}
}

//--------------------------------------------------------------------------
//	功能：离开
//--------------------------------------------------------------------------
void KUiTeamManage::OnLeave()
{
	g_pCoreShell->TeamOperation(TEAM_OI_LEAVE, 0, 0);
}