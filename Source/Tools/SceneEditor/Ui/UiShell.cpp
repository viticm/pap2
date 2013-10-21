/*****************************************************************************************
//	剑侠应用界面窗口的公共接口
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-7-17
*****************************************************************************************/
#include "KWin32.h"
#include "KWin32Wnd.h"
#include "KIme.h"
#include "../resource.h"
#include "KIniFile.h"
#include "Elem/Wnds.h"
#include "Elem/UiImage.h"
#include "UiShell.h"
#include "UiBase.h"
//#include "../../../core/src/gamedatadef.h"
#include "../../core/src/coreshell.h"

#include "UiCase/UiInit.h"
#include "UiCase/UiConnectInfo.h"
#include "UiCase/UiInformation.h"
#include "UiCase/UiInformation2.h"
#include "UiCase/UiFollowInfo.h"
#include "UiCase/UiPlayerBar.h"
#include "UiCase/UiStatus.h"
#include "UiCase/UiParadeItem.h"
#include "UiCase/UiItem.h"
#include "UiCase/UiSkills.h"
#include "UiCase/UiSkillTree.h"
#include "UiCase/UiEscDlg.h"
#include "UiCase/UiGame.h"
#include "UiCase/UiOptions.h"
#include "UiCase/UiMsgCentrePad.h"
#include "UiCase/UiLoginBg.h"
#include "UiCase/UiLogin.h"
#include "UiCase/UiSelPlayer.h"
#include "UiCase/UiSelServer.h"
#include "UiCase/UiNewPlayer.h"
#include "UiCase/UiSelNativePlace.h"
#include "UiCase/UiChatCentre.h"
#include "UiCase/UiChatStatus.h"
#include "UiCase/UiSysMsgCentre.h"
#include "UiCase/UiHeaderControlBar.h"
#include "UiCase/UiToolsControlBar.h"
#include "UiCase/UiStoreBox.h"
#include "UiCase/UiChannelSubscibe.h"
#include "UiCase/UiSelPlayerNearby.h"
#include "UiCase/UiMsgSel.h"
#include "UiCase/UiTeamManage.h"
#include "UiCase/UiShop.h"
#include "UiCase/UiMiniMap.h"
#include "UiCase/UiTrade.h"
#include "UiCase/UiTradeConfirmWnd.h"
#include "UiCase/UiNewsMessage.h"
#include "ShortcutKey.h"
#include "UiSoundSetting.h"
#include "../../Represent/iRepresent/iRepresentShell.h"
#include "../NetConnect/NetConnectAgent.h"
#include "../Login/Login.h"
#include "UiCase/UiNewPlayerStartMsg.h"
#include "UiCase/UiHelper.h"
#include "UiCase/UiHelper2.h"
#include "UiCase/UiTaskNote.h"
#include "UiCase/UiReconnect.h"
#include "UiCase/UiFaceSelector.h"
#include "UiCase/UiStrengthRank.h"
#include "UiCase/UiTongManager.h"
#include "UiCase/UiTongCreateSheet.h"
#include "UiCase/UiTongAssignBox.h"
#include "UiCase/UiEquipmentRepairAlert.h"
#include "UiCase/UiTrembleItem.h"
#include "UiCase/UiPortrait.h"
#include "UiCase/UiStall.h"
#include "UiCase/UiStallSetAdv.h"
#include "UiCase/UiDiceWeaveBingo.h"
#include "UiCase/UiKillerAllTask.h"
#include "UiCase/UiKillerOwnTask.h"
#include "UiCase/UiKillerCreateTask.h"
#include "UiCase/UiCityManager.h"
#include "UiCase/UiTongSalary.h"

#include "UiChatPhrase.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

extern iCoreShell*		g_pCoreShell;
extern iRepresentShell*	g_pRepresentShell;
extern KUiChatPhrase    g_UiChatPhrase;
extern int				g_bScreen;
HIMC   g_OldIMC       = NULL;

enum UI_LIVING_STATUS
{
	UI_LIVING_S_DEAD,			//该退出程序了
	UI_LIVING_S_OUTGAME,		//不是游戏运行中
	UI_LIVING_S_INGAME,			//游戏运行中
};

static UI_LIVING_STATUS	s_UiLiveSeed = UI_LIVING_S_DEAD;

int			g_bDisconnect = false;

#ifdef _DEBUG
	#include "KTimer.h"
	static KTimer	s_Timer;
	static int		s_nFrameRate = 30;
#endif
#ifdef SWORDONLINE_SHOW_DBUG_INFO
	#include <shlwapi.h>
	int				g_nShowDebugInfo = true;
#endif

//是否动态连接Represent连接库
#define DYNAMIC_LINK_REPRESENT_LIBRARY

#ifdef DYNAMIC_LINK_REPRESENT_LIBRARY
	extern 	int		g_bRepresent3;
#endif

void		UiCloseWndsOutGame(bool bAll);
bool		UiCloseWndsInGame(bool bAll);

#ifdef SWORDONLINE_SHOW_DBUG_INFO
bool GetFileTimeVersionString(const char* pszFile, char* pszVersionString, int nSize)
{
	if (pszVersionString)
		pszVersionString[0] = 0;
	//---打开文件----
	HANDLE	hFile = CreateFile(pszFile, GENERIC_READ, 0, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return false;

	FILETIME	time, FileTime;
	SYSTEMTIME	SysTime;
	if (::GetFileTime(hFile, NULL, NULL, &time) == FALSE)
	{
		CloseHandle(hFile);
		return false;
	}
	CloseHandle(hFile);
	hFile = NULL;
	FileTimeToLocalFileTime(&time, &FileTime);
	FileTimeToSystemTime(&FileTime, &SysTime);
	int nLen = GetDateFormat(LOCALE_USER_DEFAULT, 0, &SysTime, "M-d ", pszVersionString, nSize);
	if (nLen)
		nLen --;
	GetTimeFormat(LOCALE_USER_DEFAULT, 0, &SysTime, "H:mm", &pszVersionString[nLen], nSize - nLen);
	return true;
}
#endif

//--------------------------------------------------------------------------
//	功能：发送推出消息
//--------------------------------------------------------------------------
void UiPostQuitMsg()
{
	s_UiLiveSeed = UI_LIVING_S_DEAD;
}

//是否已经选择退出程序
bool UiIsAlreadyQuit()
{
	return (s_UiLiveSeed == UI_LIVING_S_DEAD);
}

//是否游戏运行中
bool UiIsInGame()
{
	return (s_UiLiveSeed == UI_LIVING_S_INGAME);
}

//--------------------------------------------------------------------------
//	功能：界面系统初始化
//--------------------------------------------------------------------------
int	UiInit()
{
	Player_Life::RegisterSelfClass();
	Player_Mana::RegisterSelfClass();
	Player_Stamina::RegisterSelfClass();
	Player_Exp::RegisterSelfClass();
	Player_Level::RegisterSelfClass();
	Player_WorldSort::RegisterSelfClass();
	Player_Status::RegisterSelfClass();
	Player_Items::RegisterSelfClass();
	Player_Skills::RegisterSelfClass();
	Player_Team::RegisterSelfClass();
	Player_Sit::RegisterSelfClass();
	Player_Run::RegisterSelfClass();
	Player_Horse::RegisterSelfClass();
	Player_Exchange::RegisterSelfClass();
	Player_PK::RegisterSelfClass();
	Player_Faction::RegisterSelfClass();

//	KWndEdit::Initialize(hInstance);

	Wnd_ShowCursor(false);
	if (g_UiBase.Init() == false)
		return false;
	IR_UpdateTime();
	g_UiInformation.Initialize();
	g_UiInformation2.Initialize();
	g_UiFollowInfo.Initialize();
	KUiPortrait::InitializeProtrait();
	KUiLoginBackGround::OpenWindow("Init");
	Wnd_SetGameSpaceWnd(&g_WndGameSpace);

	UiSoundLoadSetting();

	KUiFaceSelector::LoadFaceList();
	KShortcutKeyCentre::InitScript();	//要放到KUiFaceSelector之后

	g_UiChatPhrase.LoadEntireEmote();
	KUiChatPhrase::Initialize();

	g_UiBase.NotifyEvent(APP_START);

	return true;
}

//--------------------------------------------------------------------------
//	功能：界面系统退出
//--------------------------------------------------------------------------
void	UiExit()
{
	if (s_UiLiveSeed == UI_LIVING_S_INGAME)
	{
		g_UiBase.SavePrivateConfig();
	}
	KReconnectWnd::Exit(false);

	KUiInit::StopTitleMusic();

	//各种界面删除自己的临时文件
	KUiStrengthRank::RemoveTempFile();
	KUiTongManager::RemoveTempFile();
	KUiPortrait::ReleasePortrait();
	//删除完毕

	UiCloseWndsInGame(true);
	UiCloseWndsOutGame(true);

	Wnd_SetGameSpaceWnd(NULL);
	g_LoginLogic.ReturnToIdle();
	g_LoginLogic.SaveLoginChoice();
	g_UiBase.Exit();
	KShortcutKeyCentre::UninitScript();//要在g_UiBase.Exit();后，因为要存盘
	s_UiLiveSeed = UI_LIVING_S_DEAD;
	KUiFaceSelector::Clear();
	Wnd_Cleanup();

	g_UiBase.NotifyEvent(APP_EXIT);
	g_UiBase.CleanTempDataFolder();
}

//--------------------------------------------------------------------------
//	功能：開始界面控制流程
//--------------------------------------------------------------------------
int UiStart()
{
	s_UiLiveSeed = UI_LIVING_S_OUTGAME;
	Wnd_ShowCursor(true);
	Wnd_SwitchCursor(CURSOR_NORMAL);
	return (KUiInit::OpenWindow(true, true) != NULL);
}

//--------------------------------------------------------------------------
//	功能：绘制界面
//--------------------------------------------------------------------------
void UiPaint(int nGameLoop)
{
	if (g_pRepresentShell == NULL ||
		g_pRepresentShell->RepresentBegin(false, 0) == false)
	{
		if (g_pCoreShell)
			g_pCoreShell->SetRepresentShell(g_pRepresentShell);
		return;
	}

	Wnd_RenderWindows();

#ifdef _DEBUG
	DWORD	dwPing = 0;
	char	Info[128];
	s_Timer.GetFPS(&s_nFrameRate);
	if (g_pCoreShell)
		dwPing = g_pCoreShell->GetPing();
#endif
#ifdef SWORDONLINE_SHOW_DBUG_INFO
	if (g_nShowDebugInfo)
	{
		sprintf(Info,"FPS=%d LOOP=%d PING=%d", s_nFrameRate, nGameLoop, dwPing);
		g_pRepresentShell->OutputText(12, Info, -1, 10, 20, 0xffffffff, 0);

		#ifdef DYNAMIC_LINK_REPRESENT_LIBRARY
		{
			int nWidth, nHeight;
			Wnd_GetScreenSize(nWidth, nHeight);
			g_pRepresentShell->OutputText(12, g_bRepresent3 ? "Represent3" : "Represent2",
				-1, nWidth - 100, 10, 0xffffffff, 0);
		}
		#endif
	}
#endif

	g_pRepresentShell->RepresentEnd();
}

//--------------------------------------------------------------------------
//	功能：界面活动
//--------------------------------------------------------------------------
int UiHeartBeat()
{
	if (s_UiLiveSeed != UI_LIVING_S_DEAD)
	{
		if (g_bDisconnect == false)
		{
			IR_UpdateTime();
			Wnd_Heartbeat();
		}
		else
		{
			if (s_UiLiveSeed == UI_LIVING_S_INGAME &&
				KReconnectWnd::IsReconnecttingGoingOn() == false)
			{
				UiOnGameServerDisconnect();
				g_pCoreShell->OperationRequest(GOI_GAMESPACE_DISCONNECTED, 0, 0);
				UiCloseWndsInGame(false);
				KReconnectWnd::LaunchReconnect();
			}
			else
			{
				g_LoginLogic.NotifyDisconnect();
			}
			g_bDisconnect = false;
		}
		return true;
	}
	return false;
}

//--------------------------------------------------------------------------
//	功能：处理输入
//--------------------------------------------------------------------------
void	UiProcessInput(unsigned int uMsg, unsigned int uParam, int nParam)
{
    switch (uMsg)
    {
    case WM_COPYDATA:
		if (nParam)
		{
			COPYDATASTRUCT* pData = (COPYDATASTRUCT*)nParam;
			if (pData && pData->cbData > 0 && pData->lpData && ((char*)pData->lpData)[pData->cbData - 1] == 0)
			{	//有数据,而且数据长度不等于0, 而且数据是以0结尾的字符串
				KShortcutKeyCentre::ExcuteHWNDScript((const char *)pData->lpData);
			}
		}
        break;

    case WM_ACTIVATEAPP:
		if (g_pCoreShell)
			g_pCoreShell->OperationRequest(GOI_GAME_APP_ACTIVE, uParam, 0);
		if (uParam)
		Wnd_RestoreCursor();

    default:
		Wnd_ProcessInput(uMsg, uParam, nParam);
    }
}

void UiCloseWndsOutGame(bool bAll)
{
	KUiLogin::CloseWindow(true);
	KUiSelServer::CloseWindow(true);
	KUiOptions::CloseWindow();
	KUiInit::CloseWindow();
	KUiSelPlayer::CloseWindow();
	KUiNewPlayer::CloseWindow(true);
	KUiSelNativePlace::CloseWindow(true);
	KUiConnectInfo::CloseWindow(true);
	g_UiInformation.Close();
	g_UiInformation2.Close();
	g_UiFollowInfo.Close();
	if (bAll)
		KUiLoginBackGround::CloseWindow(true);
}

void UiOnJustCreatedRole()
{
	KUiNewPlayerStartMsg::OpenWindow();
//	KShortcutKeyCentre::ExcuteScript(SCK_SHORTCUT_PK);
	if (!g_pCoreShell->GetGameData(GDI_SHOW_PLAYERS_NAME, 0, 0))
		KShortcutKeyCentre::ExcuteScript(SCK_SHORTCUT_SHOWPLAYERNAME);
	if (!g_pCoreShell->GetGameData(GDI_SHOW_PLAYERS_LIFE, 0, 0))
		KShortcutKeyCentre::ExcuteScript("Switch([[showplayerlife]])");
}



//--------------------------------------------------------------------------
//	功能：进入游戏运行时
//--------------------------------------------------------------------------
void UiStartGame()
{
	g_UiBase.CleanTempDataFolder();

	KUiInit::StopTitleMusic();

	UiCloseWndsOutGame(true);
	KUiLoginBackGround::CloseWindow(false);
	KUiConnectInfo::CloseWindow(true);

	KUiMsgCentrePad::OpenWindow();
	KUiPlayerBar::OpenWindow();
	KUiSysMsgCentre::OpenWindow();
	KUiHeaderControlBar::OpenWindow();
	KUiToolsControlBar::OpenWindow();
	KUiNewsMessage::OpenWindow();
	KUiEquipmentRepairAlert::OpenWindow();
	Wnd_ShowHideGameSpace(true);
	g_UiBase.ClearStatus();
	Wnd_GameSpaceHandleInput(true);

	KShortcutKeyCentre::Enable(true);

	KUiMsgCentrePad::ReleaseActivateChannelAll();
	KUiMsgCentrePad::QueryAllChannel();

	KUiChatPhrase::SetAutoReply(-1);

	MapSetMode(MINIMAP_M_BRIEF_PIC);

	if (g_LoginLogic.IsRoleNewCreated())
    {
		UiOnJustCreatedRole();
    }

	if (g_bScreen == false)
	{
		KShortcutKeyCentre::ExcuteScript("SetChatFontSize(12)");
	}

	g_UiBase.LoadPrivateConfig();
	s_UiLiveSeed = UI_LIVING_S_INGAME;

	char szRole[64];
	szRole[0] = 0;
	g_LoginLogic.GetLoginAccount(szRole);
	if (szRole[0] != 0)
	{
		char szEvent[256];
		sprintf(szEvent, APP_STARTGAME, szRole);
		g_UiBase.NotifyEvent(szEvent);
	}

    #ifdef KSG_USE_WIN32_EDIT
    g_OldIMC = ::ImmAssociateContext(g_GetMainHWnd(), NULL);
    ::SetFocus(g_GetMainHWnd());
    #endif
}

//游戏（断线后）继续
void UiResumeGame()
{
	s_UiLiveSeed = UI_LIVING_S_INGAME;
}

bool UiCloseWndsInGame(bool bAll)
{
	if (bAll == false)
	{
		if (KUiStatus::GetIfVisible() == NULL &&
			KUiItem::GetIfVisible() == NULL &&
			KUiSkills::GetIfVisible() == NULL &&
			KUiSkillTree::GetIfVisible() == NULL &&
			KUiTrade::GetIfVisible() == NULL &&
			KUiOptions::GetIfVisible() == NULL &&
			KUiChatStatus::GetIfVisible() == NULL &&
			KUiStoreBox::GetIfVisible() == NULL &&
			KUiTeamManage::GetIfVisible() == NULL &&
			KUiTradeConfirm::GetIfVisible() == NULL &&
			KUiShop::GetIfVisible() == NULL &&
			KUiHelper::GetIfVisible() == NULL &&
			KUiHelper2::GetIfVisible() == NULL &&
			KUiTaskNote::GetIfVisible() == NULL &&
			KUiSelPlayerNearby::DoesHaveVisibleWnd() == false &&
			KUiParadeItem::GetIfVisible() == NULL &&
			KUiFaceSelector::GetIfVisible() == NULL &&
			KUiStrengthRank::GetIfVisible() == NULL &&
			KUiTongManager::GetIfVisible() == NULL &&
			KUiTongCreateSheet::GetIfVisible() == NULL &&
			KUiTongAssignBox::GetIfVisible() == NULL &&
			KUiTrembleItem::GetIfVisible() == NULL &&
			KUiPortrait::GetIfVisible() == NULL &&
			KUiStall::GetIfVisible() == NULL &&
			KUiStallSetAdv::GetIfVisible() == NULL &&
			KUiDiceWeaveBingo::GetIfVisible() == NULL &&
			KUiKillerAllTask::GetIfVisible() == NULL &&
			KUiKillerOwnTask::GetIfVisible() == NULL &&
			KUiKillerCreateTask::GetIfVisible() == NULL &&
			KUiCityManager::GetIfVisible() == NULL &&
			KUiTongSalary::GetIfVisible() == NULL
			//KUiManage::GetIfVisible() ||
			)
		{
			return false;
		}
	}
	else
	{
		KUiMsgSel::CloseWindow(true);
		KUiPlayerBar::CloseWindow(true);
		KUiMsgCentrePad::CloseWindow(true);
		KUiHeaderControlBar::CloseWindow();
		KUiToolsControlBar::CloseWindow();
		KUiESCDlg::CloseWindow(true);
		KUiSysMsgCentre::CloseWindow();
		KUiChatCentre::CloseWindow(true);
		//KUiManage::CloseWindow();
		MapSetMode(MINIMAP_M_NONE);
		KUiNewsMessage::CloseWindow(TRUE);
		g_UiInformation.Close();
		g_UiInformation2.Close();
		g_UiFollowInfo.Close();
	}

	KUiFaceSelector::CloseWindow(bAll);
	KUiStatus::CloseWindow(bAll);
	KUiItem::CloseWindow(bAll);
	KUiSkills::CloseWindow(bAll);
	KUiSkillTree::CloseWindow(bAll);
	KUiTrade::CloseWindow();
	KUiOptions::CloseWindow();
	KUiChatStatus::CloseWindow();
	KUiStoreBox::CloseWindow();
	KUiTeamManage::CloseWindow();
	KUiTradeConfirm::CloseWindow(bAll);
	KUiShop::CloseWindow();
	KUiSelPlayerNearby::CloseWindow(bAll);
	// Of cause i should add this line   -- flying
	KUiHelper::CloseWindow(bAll);
	KUiHelper2::CloseWindow(bAll);
	KUiTaskNote::CloseWindow(bAll);
	KUiParadeItem::CloseWindow(bAll);
	KUiStrengthRank::CloseWindow();
	KUiTongManager::CloseWindow();
	KUiTongCreateSheet::CloseWindow();
	KUiTongAssignBox::CloseWindow();
	KUiTrembleItem::CloseWindow();
	KUiPortrait::CloseWindow();
	KUiStall::CloseWindow(bAll);
	KUiStallSetAdv::CloseWindow(bAll);
	KUiDiceWeaveBingo::CloseWindow();
	KUiKillerAllTask::CloseWindow(bAll);
	KUiKillerOwnTask::CloseWindow(bAll);
	KUiKillerCreateTask::CloseWindow(bAll);
	KUiCityManager::CloseWindow(bAll);
	KUiTongSalary::CloseWindow(bAll);
	return true;
}

//--------------------------------------------------------------------------
//	功能：离开游戏运行时
//--------------------------------------------------------------------------
void UiEndGame()
{
    if (g_OldIMC)
    {
        ::ImmAssociateContext(g_GetMainHWnd(), g_OldIMC);
        g_OldIMC = NULL;
        g_pIme->CloseIME();
    }

	KReconnectWnd::Exit(false);
	g_UiBase.SavePrivateConfig();

	Wnd_DragFinished();
	Wnd_SwitchCursor(CURSOR_NORMAL);
	g_NetConnectAgent.DisconnectClient();
	s_UiLiveSeed = UI_LIVING_S_OUTGAME;

	KShortcutKeyCentre::Enable(false);

	UiCloseWndsInGame(true);
	Wnd_ShowHideGameSpace(false);
	Wnd_GameSpaceHandleInput(false);
	KUiEquipmentRepairAlert::CloseWindow();
	KUiLoginBackGround::OpenWindow("Init");
	g_UiBase.ClearStatus();

	g_UiBase.NotifyEvent(APP_EXITGAME);
}

//设置屏幕范围大小
void UiSetScreenSize(int nWidth, int nHeight)
{
	Wnd_SetScreenSize(nWidth, nHeight);
}

//与一个旧的GameServer断开连接
void UiOnGameServerDisconnect()
{
	g_UiBase.SavePrivateConfig();
}

//与一个新的GameServer建立连接，启动同步完毕
void UiOnGameServerStartSyncEnd()
{
	KUiChatCentre::OpenWindow(false);
	KUiChatCentre::CreateSeverUnit();
	g_UiBase.LoadPrivateConfig();	//不太好:-(
}

/////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_COMCLASS(Player_Life)
bool Player_Life::m_bText = true;
void Player_Life::UpdateData()
{
	if (g_pCoreShell)
	{
		KUiPlayerRuntimeInfo	Info;
		memset(&Info, 0, sizeof(KUiPlayerRuntimeInfo));
		g_pCoreShell->GetGameData(GDI_PLAYER_RT_INFO, (int)&Info, 0);

		Info.nLife = max(Info.nLife, 0);
		Info.nLifeFull = max(Info.nLifeFull, 0);

		Set2IntValue(Info.nLife, Info.nLifeFull);
		if (m_bText)
			Set2IntText(Info.nLife, Info.nLifeFull, '/');
		else
			SetText(NULL, 0);
	}
}

void Player_Life::OnButtonClick()
{
	KShortcutKeyCentre::ExcuteScript(SCK_SHORTCUT_SHOWPLAYERNUMBER);
	UpdateData();
}


IMPLEMENT_COMCLASS(Player_Mana)
bool Player_Mana::m_bText = true;
void Player_Mana::UpdateData()
{
	if (g_pCoreShell)
	{
		KUiPlayerRuntimeInfo	Info;
		memset(&Info, 0, sizeof(KUiPlayerRuntimeInfo));
		g_pCoreShell->GetGameData(GDI_PLAYER_RT_INFO, (int)&Info, 0);

		Info.nMana = max(Info.nMana, 0);
		Info.nManaFull = max(Info.nManaFull, 0);

		Set2IntValue(Info.nMana, Info.nManaFull);

		if (m_bText)
			Set2IntText(Info.nMana, Info.nManaFull, '/');
		else
			SetText(NULL, 0);
	}
}

void Player_Mana::OnButtonClick()
{
	KShortcutKeyCentre::ExcuteScript(SCK_SHORTCUT_SHOWPLAYERNUMBER);
	UpdateData();
}


IMPLEMENT_COMCLASS(Player_Stamina)
bool Player_Stamina::m_bText = true;
void Player_Stamina::UpdateData()
{
	if (g_pCoreShell)
	{
		KUiPlayerRuntimeInfo	Info;
		memset(&Info, 0, sizeof(KUiPlayerRuntimeInfo));
		g_pCoreShell->GetGameData(GDI_PLAYER_RT_INFO, (int)&Info, 0);

		Info.nStamina = max(Info.nStamina, 0);
		Info.nStaminaFull = max(Info.nStaminaFull, 0);

		Set2IntValue(Info.nStamina, Info.nStaminaFull);

		if (m_bText)
			Set2IntText(Info.nStamina, Info.nStaminaFull, '/');
		else
			SetText(NULL, 0);
	}
}

void Player_Stamina::OnButtonClick()
{
	KShortcutKeyCentre::ExcuteScript(SCK_SHORTCUT_SHOWPLAYERNUMBER);
	UpdateData();
}


IMPLEMENT_COMCLASS(Player_Exp)
bool Player_Exp::m_bText = true;

void Player_Exp::UpdateData()
{
	int nFull, nCurrLevelExp, nCurrent;

	if (KUiPlayerBar::GetExp(nFull, nCurrLevelExp, nCurrent))
	{
		//float精度不够
		double np = 100;
		int    nNp/*小数点左边*/, nPointP/*小数点右边*/;

		if (nCurrLevelExp != 0)
			np = ((nCurrent - nFull + nCurrLevelExp) * 100.0) / nCurrLevelExp;
		np = max(np, 0);
		np = min(np, 100);
		Set2IntValue((int)np, 100);

		/*这两个计算分别运算出小数点左边和右边分别要显示的数值*/
		nNp = (int)np;
		nPointP = (int)(np * 100) - nNp * 100;
		/******************************************************/
		/*如果直接用sprintf %.2f的方式生成字串，那么99.9999999*/
		/*这样的数字会变成100.00，不能得到想要的结果          */

		if (m_bText)
		{
			int nLen;
			char szExp[32];
			KUiPlayerAttribute	Info;
			memset(&Info, 0, sizeof(KUiPlayerAttribute));
			Info.nLevel = 1;
			if(g_pCoreShell)
			{
				g_pCoreShell->GetGameData(GDI_PLAYER_RT_ATTRIBUTE, (unsigned int)&Info, 0);
			}
			if(Info.nLevel > 79)
			{
				nLen = sprintf(szExp, "%d.%2d%%", nNp, nPointP);
			}
			else
			{
				nLen = sprintf(szExp, "%d%%", nNp);
			}
			SetText(szExp, nLen);
		}
		else
			SetText(NULL, 0);
	}
}

void Player_Exp::OnButtonClick()
{
	KShortcutKeyCentre::ExcuteScript(SCK_SHORTCUT_SHOWPLAYERNUMBER);
	UpdateData();
}

int Player_Exp::GetToolTipInfo(char* szTip, int nMax)
{
	int		nFull, nCurrLevelExp, nCurrent;
	int		nLen = 0;
	if (KUiPlayerBar::GetExp(nFull, nCurrLevelExp, nCurrent) && szTip && nMax > 0)
	{
		char	szBuffer[64];
		nLen = sprintf(szBuffer, "经验 %d/%d", nCurrent, nFull);
		if (nLen <= nMax)
			memcpy(szTip, szBuffer, nLen);
		else
			nLen = 0;
	}
	return nLen;
}

IMPLEMENT_COMCLASS(Player_Level)
void Player_Level::UpdateData()
{
	if (g_pCoreShell)
	{
		KUiPlayerAttribute	Info;
		memset(&Info, 0, sizeof(KUiPlayerAttribute));
		g_pCoreShell->GetGameData(GDI_PLAYER_RT_ATTRIBUTE, (unsigned int)&Info, 0);
		SetIntText(Info.nLevel, 0);
	}
}

IMPLEMENT_COMCLASS(Player_WorldSort)
void Player_WorldSort::UpdateData()
{
	if (g_pCoreShell)
	{
		KUiPlayerBaseInfo	Info;
		memset(&Info, 0, sizeof(KUiPlayerBaseInfo));
		g_pCoreShell->GetGameData(GDI_PLAYER_BASE_INFO, (unsigned int)&Info, 0);
		if(Info.nRankInWorld > 0)
		{
			SetIntText(Info.nRankInWorld, 0);
		}
		else
		{
			SetText("?");
		}
	}
}

void Player_WorldSort::OnButtonClick()
{
	KUiStrengthRank::OpenDefaultWindow();
}

IMPLEMENT_COMCLASS(Player_Status)
void Player_Status::OnButtonClick()
{
	KShortcutKeyCentre::ExcuteScript(SCK_SHORTCUT_STATUS);
}

const char*	Player_Status::GetShortKey()
{
	return KShortcutKeyCentre::GetKeyName(
		KShortcutKeyCentre::GetCommandKey(
		KShortcutKeyCentre::FindCommandByScript(SCK_SHORTCUT_STATUS)));
}


IMPLEMENT_COMCLASS(Player_Items)
void Player_Items::OnButtonClick()
{
	KShortcutKeyCentre::ExcuteScript(SCK_SHORTCUT_ITEMS);
}

const char*	Player_Items::GetShortKey()
{
	return KShortcutKeyCentre::GetKeyName(
		KShortcutKeyCentre::GetCommandKey(
		KShortcutKeyCentre::FindCommandByScript(SCK_SHORTCUT_ITEMS)));
}


IMPLEMENT_COMCLASS(Player_Skills)
void Player_Skills::OnButtonClick()
{
	KShortcutKeyCentre::ExcuteScript(SCK_SHORTCUT_SKILLS);
}

const char*	Player_Skills::GetShortKey()
{
	return KShortcutKeyCentre::GetKeyName(
		KShortcutKeyCentre::GetCommandKey(
		KShortcutKeyCentre::FindCommandByScript(SCK_SHORTCUT_SKILLS)));
}


IMPLEMENT_COMCLASS(Player_Team)
void Player_Team::OnButtonClick()
{
	KShortcutKeyCentre::ExcuteScript(SCK_SHORTCUT_TEAM);
}

const char*	Player_Team::GetShortKey()
{
	return KShortcutKeyCentre::GetKeyName(
		KShortcutKeyCentre::GetCommandKey(
		KShortcutKeyCentre::FindCommandByScript(SCK_SHORTCUT_TEAM)));
}


IMPLEMENT_COMCLASS(Player_Faction)
void Player_Faction::OnButtonClick()
{
	if(KUiTongManager::GetIfVisible())
		KUiTongManager::CloseWindow();
	else
	    KUiTongManager::OpenWindow(NULL);
}
const char*	Player_Faction::GetShortKey()
{
	return NULL;
}


IMPLEMENT_COMCLASS(Player_Sit)
void Player_Sit::OnButtonClick()
{
	KShortcutKeyCentre::ExcuteScript(SCK_SHORTCUT_SIT);
}

void Player_Sit::UpdateData()
{
	if (g_pCoreShell)
	{
		KUiPlayerRuntimeInfo	Info;
		memset(&Info, 0, sizeof(KUiPlayerRuntimeInfo));
		g_pCoreShell->GetGameData(GDI_PLAYER_RT_INFO, (int)&Info, 0);
		CheckButton(Info.byAction & PA_SIT);
	}
}

const char*	Player_Sit::GetShortKey()
{
	return KShortcutKeyCentre::GetKeyName(
		KShortcutKeyCentre::GetCommandKey(
		KShortcutKeyCentre::FindCommandByScript(SCK_SHORTCUT_SIT)));
}


IMPLEMENT_COMCLASS(Player_Run)
void Player_Run::OnButtonClick()
{
	KShortcutKeyCentre::ExcuteScript(SCK_SHORTCUT_RUN);
}

void Player_Run::UpdateData()
{
	if (g_pCoreShell)
	{
		KUiPlayerRuntimeInfo	Info;
		memset(&Info, 0, sizeof(KUiPlayerRuntimeInfo));
		g_pCoreShell->GetGameData(GDI_PLAYER_RT_INFO, (int)&Info, 0);
		CheckButton(Info.byAction & PA_RUN);
	}
}

const char*	Player_Run::GetShortKey()
{
	return KShortcutKeyCentre::GetKeyName(
		KShortcutKeyCentre::GetCommandKey(
		KShortcutKeyCentre::FindCommandByScript(SCK_SHORTCUT_RUN)));
}

IMPLEMENT_COMCLASS(Player_Horse)
void Player_Horse::OnButtonClick()
{
	KShortcutKeyCentre::ExcuteScript(SCK_SHORTCUT_HORSE);
}

void Player_Horse::UpdateData()
{
	if (g_pCoreShell)
	{
		KUiPlayerRuntimeInfo	Info;
		memset(&Info, 0, sizeof(KUiPlayerRuntimeInfo));
		g_pCoreShell->GetGameData(GDI_PLAYER_RT_INFO, (int)&Info, 0);
		CheckButton(Info.byAction & PA_RIDE);
	}
}

const char*	Player_Horse::GetShortKey()
{
	return KShortcutKeyCentre::GetKeyName(
		KShortcutKeyCentre::GetCommandKey(
		KShortcutKeyCentre::FindCommandByScript(SCK_SHORTCUT_HORSE)));
}

IMPLEMENT_COMCLASS(Player_Exchange)
void Player_Exchange::OnButtonClick()
{
	KShortcutKeyCentre::ExcuteScript(SCK_SHORTCUT_TRADE);
}

void Player_Exchange::UpdateData()
{
	if (g_pCoreShell)
	{
		CheckButton(g_pCoreShell->GetGameData(GDI_TRADE_OPER_DATA, UTOD_IS_WILLING, 0));
	}
}

const char*	Player_Exchange::GetShortKey()
{
	return KShortcutKeyCentre::GetKeyName(
		KShortcutKeyCentre::GetCommandKey(
		KShortcutKeyCentre::FindCommandByScript(SCK_SHORTCUT_TRADE)));
}

IMPLEMENT_COMCLASS(Player_PK)
void Player_PK::OnButtonClick()
{
	PopupSelectMenu();
}

void Player_PK::UpdateData()
{
	if (g_pCoreShell)
	{
		SetFrame(g_pCoreShell->GetGameData(GDI_PK_SETTING, 0, 0));
	}
}

void Player_PK::PopupSelectMenu()
{
	KPopupMenuData* pMenuData = (KPopupMenuData*)malloc(MENU_DATA_SIZE(3));
	if (pMenuData)
	{
		KPopupMenu::InitMenuData(pMenuData, 3);

		strcpy(pMenuData->Items[0].szData, "练功");
		pMenuData->Items[0].uDataLen = strlen(pMenuData->Items[0].szData);
		strcpy(pMenuData->Items[1].szData, "战斗");
		pMenuData->Items[1].uDataLen = strlen(pMenuData->Items[1].szData);
		strcpy(pMenuData->Items[2].szData, "屠杀");
		pMenuData->Items[2].uDataLen = strlen(pMenuData->Items[2].szData);

		int nX, nY;
		Wnd_GetCursorPos(&nX, &nY);

		pMenuData->nX = nX;
		pMenuData->nY = nY;
		KPopupMenu::Popup(pMenuData, (KWndWindow*)this, 0);
	}
}

int Player_PK::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	if(uMsg == WND_M_MENUITEM_SELECTED)
	{
		char szBuff[128];
		sprintf(szBuff, SCK_SHORTCUT_PK, nParam);
		KShortcutKeyCentre::ExcuteScript(szBuff);
	}
	else if(uMsg == WM_LBUTTONUP)
	{
		OnButtonClick();
	}
	return KMyWndImage::WndProc(uMsg, uParam, nParam);
}

const char*	Player_PK::GetShortKey()
{
	return KShortcutKeyCentre::GetKeyName(
		KShortcutKeyCentre::GetCommandKey(
		KShortcutKeyCentre::FindCommandByScript(SCK_SHORTCUT_PK)));
}
