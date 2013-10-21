/*****************************************************************************************
//	遊戲窗口的接口
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-9-18
------------------------------------------------------------------------------------------
	向无数抗日先烈。致敬！倡议9.18全国鸣警报。
*****************************************************************************************/
#include "KWin32.h"

#include "Windows.h"
#include "../Elem/WndMessage.h"
#include "../../../core/Src/CoreShell.h"
#include "../../../core/Src/GameDataDef.h"
#include "UiGame.h"
#include "memory.h"

#include "UiPlayerBar.h"
#include "UiChatCentre.h"
#include "UiFaceSelector.h"
#include "UiTongManager.h"
#include "UiGetMoney.h"
#include "UiMiniMap.h"
#include "../UiChatPhrase.h"
#include "../Elem/PopupMenu.h"
#include "../../../Engine/src/Text.h"
#include "UiKillerAllTask.h"
#include "UiKillerOwnTask.h"
#include "UiKillerCreateTask.h"
#include "UiGetString.h"

#include "../../../core/Src/KPlayerMenuState.h"

#include "../Elem/Wnds.h"

#include "../ShortcutKey.h"
#include "CommCtrl.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

extern iCoreShell*		g_pCoreShell;
extern KUiChatPhrase    g_UiChatPhrase;

KUiGameSpace	g_WndGameSpace;

#define SEL_ACTION_MENU 1
#define SEL_EMOTE_MENU 2

extern int			g_bDisconnect;

KUiGameSpace::KUiGameSpace()
{
	memset(m_nGameSetting, 0, sizeof(m_nGameSetting));
}

//--------------------------------------------------------------------------
//	功能：输入处理消息以及一些特定窗口消息的响应
//--------------------------------------------------------------------------
int KUiGameSpace::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	bool bDefault = true;
	int nKind = -1;
	int nStallStat = 0;

	BOOL bForbit = KUiKillerAllTask::GetIfVisible() || KUiKillerOwnTask::GetIfVisible() || KUiKillerCreateTask::GetIfVisible();
	bForbit = bForbit || (KUiGetString::GetIfVisible() && KUiGetString::GetIfVisible()->IsGameSpaceForbitMove());
	nStallStat = g_pCoreShell->GetGameData(GDI_IS_STALL, -1, -1);
	// if the player is in the "stall" status, all mouse operation should
	// be denied.
	//		flying commented.
	if (!g_bDisconnect &&
		((nStallStat || bForbit) && uMsg != WND_M_MENUITEM_SELECTED && uMsg != WND_M_OTHER_WORK_RESULT &&
		!(uMsg == WM_RBUTTONDOWN && (uParam & MK_CONTROL) && !(uParam & MK_SHIFT) && !((GetKeyState(VK_MENU) & 0x8000)))))
		return 0;

	switch(uMsg)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		{
			int nModifier = 0;
			if (uParam & MK_CONTROL)
				nModifier |= HOTKEYF_CONTROL;

			if (uParam & MK_SHIFT)
				nModifier |= HOTKEYF_SHIFT;

			if (GetKeyState(VK_MENU) & 0x8000)
				nModifier |= HOTKEYF_ALT;

			if (KShortcutKeyCentre::HandleMouseInput(uMsg == WM_LBUTTONDOWN ? VK_LBUTTON : VK_RBUTTON, nModifier, GET_X_LPARAM(nParam), GET_Y_LPARAM(nParam)))
				bDefault = false;

			KUiMiniMap::RestoreMapPos();
		}
		break;
	case WM_LBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:
		{
			int nModifier = 0;
			if (uParam & MK_CONTROL)
				nModifier |= HOTKEYF_CONTROL;

			if (uParam & MK_SHIFT)
				nModifier |= HOTKEYF_SHIFT;

			if (GetKeyState(VK_MENU) & 0x8000)
				nModifier |= HOTKEYF_ALT;

			if (KShortcutKeyCentre::HandleMouseInput(uMsg == WM_LBUTTONDBLCLK ? VK_LDBUTTON : VK_RDBUTTON, nModifier, GET_X_LPARAM(nParam), GET_Y_LPARAM(nParam)))
				bDefault = false;
		}
		break;
	case WND_M_MENUITEM_SELECTED:
		if (uParam == (unsigned int)(KWndWindow*)this)
		{
			if (GET_Y_LPARAM(nParam) == SEL_ACTION_MENU && GET_X_LPARAM(nParam) >= 0)
			{
				ProcessPeople(&m_SelectPlayer, GET_X_LPARAM(nParam));
				bDefault = false;
			}
			if (GET_Y_LPARAM(nParam) == SEL_EMOTE_MENU && GET_X_LPARAM(nParam) >= 0)
			{
				ProcessEmote(m_SelectPlayer.Name, "", GET_X_LPARAM(nParam));
				bDefault = false;
			}
		}
		break;
	case WM_MOUSEMOVE:
	case WM_MOUSEHOVER:
		if (uParam & MK_LBUTTON)
		{
			if (g_pCoreShell)
				g_pCoreShell->GotoWhere(GET_X_LPARAM(nParam), GET_Y_LPARAM(nParam), 0);
		}
		else if (uParam & MK_RBUTTON)
		{
			KShortcutKeyCentre::HandleMouseInput(0, VK_RBUTTON, GET_X_LPARAM(nParam), GET_Y_LPARAM(nParam));
		}

		OnMouseMoveCursor(GET_X_LPARAM(nParam), GET_Y_LPARAM(nParam), uParam & MK_CONTROL);
		bDefault = false;
		break;
	case WM_MOUSEWHEEL:
		if (g_pCoreShell)
		{
			int zDelta = GET_Y_LPARAM(uParam);
			int nPos = (-zDelta / WHEEL_DELTA);
			if (nPos > 0)
				g_pCoreShell->Turn(0);
			else if (nPos < 0)
				g_pCoreShell->Turn(1);
		}
		break;

	case WND_M_OTHER_WORK_RESULT:
		if(uParam == enumGSR_GET_MONEY_FOR_GIVE && g_pCoreShell && nParam > 0)
		{
			g_pCoreShell->OperationRequest(GOI_GIVE_SOMEONE_MONEY, m_uGiveMoneyTargetID, nParam);
		}
		else if (uParam == enumGSR_GET_ROLENAME_FOR_WISEMAN && g_pCoreShell && nParam)
		{
			g_pCoreShell->OperationRequest(GOI_KILLER_QUERY_WISEMAN, nParam, 0);
		}
		else if (uParam == enumGSR_GET_STRING_COMMON && g_pCoreShell && nParam)
		{
			g_pCoreShell->OperationRequest(GOI_GET_STRING_COMMON, nParam, 0);
		}
		else if (uParam == enumGSR_GET_NUMBER_COMMON && g_pCoreShell)
		{
			g_pCoreShell->OperationRequest(GOI_GET_NUMBER_COMMON, nParam, 0);
		}
		break;

	default:
		break;
	}

	if (bDefault && g_pCoreShell)
		g_pCoreShell->ProcessInput(uMsg, uParam, nParam);

	return 0;
}

//--------------------------------------------------------------------------
//	功能：绘制游戏世界
//--------------------------------------------------------------------------
void KUiGameSpace::PaintWindow()
{
	g_pCoreShell->DrawGameSpace();
}

void KUiGameSpace::OnMouseMoveCursor(int x, int y, BOOL bCtrl)
{
	if (g_pCoreShell == NULL)
		return;

	KUiPlayerItem SelectPlayer;
	int nKind = -1;
	int nObjectIdx = -1;
	if (g_pCoreShell->FindSelectNPC(x, y, relation_all, true, &SelectPlayer, nKind))
	{
		if (nKind == kind_dialoger)
			Wnd_SwitchCursor(MOUSE_CURSOR_DIALOG);
		else if (g_pCoreShell->GetNPCRelation(SelectPlayer.nIndex) == relation_enemy)
			Wnd_SwitchCursor(MOUSE_CURSOR_FIGHT);
		else if (bCtrl && g_pCoreShell->GetGameData(GDI_IS_STALL, SelectPlayer.nIndex, -1) == 1)
			Wnd_SwitchCursor(CURSOR_QUERY_STALL);
		else
			Wnd_SwitchCursor(MOUSE_CURSOR_NORMAL);
	}
	else if (g_pCoreShell->FindSelectObject(x, y, true, nObjectIdx, nKind))
	{
		if (nKind == Obj_Kind_MapObj)
			Wnd_SwitchCursor(MOUSE_CURSOR_DIALOG);
		else if (nKind == Obj_Kind_Item || nKind == Obj_Kind_Money)
			Wnd_SwitchCursor(MOUSE_CURSOR_PICK);
		else if (nKind == Obj_Kind_Prop)
			Wnd_SwitchCursor(MOUSE_CURSOR_USE);
		else if (nKind == Obj_Kind_Box)
			Wnd_SwitchCursor(CURSOR_POINT_TO_OBJ_NPC);
	}
	else
		Wnd_SwitchCursor(MOUSE_CURSOR_NORMAL);
}

void AddBlackList(const char* strName, const char* strGroup, bool bNotify);
bool IsInBlackName(char* strName);

void ProcessPeople(KUiPlayerItem* pDest, int nAction)
{
	if (pDest == NULL || pDest->Name[0] == 0)
		return;
	switch(nAction)
	{
	case ACTION_JOINTEAM:	//要用nIndex
		if (g_pCoreShell && pDest->nData == PLAYER_MENU_STATE_TEAMOPEN && pDest->nIndex != -1)
			g_pCoreShell->ApplyAddTeam(pDest);
		break;
	case ACTION_TRADE:	//要用nIndex
		if (g_pCoreShell && pDest->nData == PLAYER_MENU_STATE_TRADEOPEN && pDest->nIndex != -1)
			g_pCoreShell->TradeApplyStart(pDest);
		break;
	case ACTION_CHAT:		//要用Name
		KUiPlayerBar::InputNameMsg(false, pDest->Name, true);
		break;
	case ACTION_REVENGE:			//要用uId
		if (g_pCoreShell && pDest->uId != 0)
			g_pCoreShell->OperationRequest(GOI_REVENGE_SOMEONE, (unsigned int)pDest, 0);
		break;
	case ACTION_MAKEFRIEND:	//要用Name
		if (g_pCoreShell && !KUiChatCentre::IsMyFriend(pDest->Name))
			g_pCoreShell->OperationRequest(GOI_CHAT_FRIEND_ADD,	(unsigned int)pDest, 0);
		break;
	case ACTION_INVITETEAM:			//要用uId
		if (g_pCoreShell && pDest->uId != 0)
		{
			KUiPlayerTeam	TeamInfo;
			TeamInfo.cNumMember = 0;
			g_pCoreShell->TeamOperation(TEAM_OI_GD_INFO, (unsigned int)&TeamInfo, 0);
			if (TeamInfo.cNumMember == 0)
				g_pCoreShell->TeamOperation(TEAM_OI_CREATE, 0, 0);
			g_pCoreShell->TeamOperation(TEAM_OI_INVITE, (unsigned int)pDest, 0);
		}
		break;
	case ACTION_FOLLOW:	//要用nIndex
		if (g_pCoreShell && pDest->nIndex != -1)
			g_pCoreShell->OperationRequest(GOI_FOLLOW_SOMEONE, (unsigned int)pDest, 0);
		break;
	case ACTION_VIEWITEM:			//要用uId
		if (g_pCoreShell && pDest->uId != 0)
			g_pCoreShell->OperationRequest(GOI_VIEW_PLAYERITEM, (unsigned int)pDest->uId, 0);
		break;
	case ACTION_BLACKLIST:	//要用Name
		if (KUiChatCentre::IsMyFriend(pDest->Name) == false && IsInBlackName(pDest->Name) == false)
			AddBlackList(pDest->Name, "", true);
		break;
	case ACTION_TONG:
		KUiTongManager::OpenWindow(pDest->Name);
		break;
	case ACTION_GIVE_MONEY: //要用uId
		if(g_pCoreShell)
		{
			g_WndGameSpace.SetGiveMoneyTargetID(pDest->uId);
			KUiGetMoney::OpenWindow(0, g_pCoreShell->GetGameData(GDI_PLAYER_HOLD_MONEY, 0, 0), &g_WndGameSpace, enumGSR_GET_MONEY_FOR_GIVE, NULL);
		}
		break;
	case ACTION_FILTER_WORLD_CHANNEL:
		KUiMsgCentrePad *pMsgCenter = KUiMsgCentrePad::GetSelf();
		if(pMsgCenter)
		{
			KUiGetString::OpenWindow("关键字(空格分隔)", "", pMsgCenter, KUiMsgCentrePad::enumRT_WORLD_CHANNEL_KEYWORD, 0, 32);
		}
		break;
	}
}


  /*__(@_           功能：设置赠送金钱的对象的ID
 /     ) \______________________________________________________
(_)@8@8{}<______________________________________________________>
       )_/
      (*/
void KUiGameSpace::SetGiveMoneyTargetID(unsigned int uID)
{
	m_uGiveMoneyTargetID = uID;
}


  /*__(@_           功能：事件触发器
 /     ) \______________________________________________________
(_)@8@8{}<______________________________________________________>
       )_/
      (*/
void KUiGameSpace::Trigger(enumTRIGGER_TYPE eType, unsigned int uParam, int nParam)
{
	switch(eType)
	{
	case enumTT_SWITCH_SCENE_FINISH:
		KUiMiniMap::HandleMapInfo(uParam);
		break;

	case enumTT_GAME_START:
		///设置好友列表隐身状态
		{
			if(g_pCoreShell)
			{
				g_pCoreShell->OperationRequest(GOI_INVISIBLE_IN_FRIEND_LIST, 0, m_nGameSetting[enumGRS_INVISIBLE_FRIEND_LIST]);
			}
		}
		break;
	}
}


  /*__(@_           功能：设置游戏参数
 /     ) \______________________________________________________
(_)@8@8{}<______________________________________________________>
       )_/
      (*/
void KUiGameSpace::Setting(enumGAME_RUNNING_SETTING eType, int nValue)
{
	if(eType >= 0 && eType < enumGRS_NUM)
	{
		m_nGameSetting[eType] = nValue;
	}
}


  /*__(@_           功能：设置游戏参数
 /     ) \______________________________________________________
(_)@8@8{}<______________________________________________________>
       )_/
      (*/
int KUiGameSpace::GetSetting(enumGAME_RUNNING_SETTING eType)
{
	if(eType >= 0 && eType < enumGRS_NUM)
	{
		return m_nGameSetting[eType];
	}
	else
	{
		return 0;
	}
}


void ProcessEmote(char* szDest, char *szDestChannel, int nEmoteIndex)
{
	if (g_pCoreShell && szDest)
	{
		KUiPlayerBaseInfo MyInfo;
		char szBuff[256];
		int nStringLen, nChannelIndex, nDestChannel;

		nDestChannel = KUiMsgCentrePad::GetChannelIndex(szDestChannel);
		if(nDestChannel == -1)
		{
			nChannelIndex = KUiPlayerBar::GetCurChannel();
		}
		else
		{
			nChannelIndex = nDestChannel;
		}
		if(nChannelIndex != -1)
		{
			int nChannelCount = KUiMsgCentrePad::GetChannelCount();
			int nChannelID    = KUiMsgCentrePad::GetChannelID(nChannelIndex);

			//开始获取和转换字串
			g_pCoreShell->GetGameData(GDI_PLAYER_BASE_INFO, (unsigned int)&MyInfo, 0);
       		if(szDest[0])
	            nStringLen = g_UiChatPhrase.GetEmote(nEmoteIndex, szBuff, sizeof(szBuff));
		    else
			    nStringLen = g_UiChatPhrase.GetEmote(nEmoteIndex, szBuff, sizeof(szBuff), 1);
			//获取和转换字串玩了

			//发出去
			if(szBuff[0] && KUiPlayerBar::IsCanSendMessage(szBuff, nStringLen, szDest, nChannelID))
			{
				char szBuffConverted[256];
				nStringLen = KUiFaceSelector::ConvertFaceText(szBuffConverted, szBuff, nStringLen);
				nStringLen = g_UiChatPhrase.ConvertEmoteString(szBuffConverted, nStringLen, MyInfo.Name, szDest);
    			if(nChannelIndex >= nChannelCount)
			    {
					if (szDest[0])
					{
    				    KUiPlayerBar::OnSendSomeoneMessage(szDest, szBuffConverted, nStringLen);
						int nIndex = KUiPlayerBar::AddRecentPlayer(szDest);
						KUiPlayerBar::SetCurrentChannel(nIndex);
					}
			    }
				else
				{
					KUiPlayerBar::OnSendChannelMessage(nChannelID, szBuffConverted, nStringLen);
				}
				///////////////////////////////////////////
			}
			/////////////////////////////
		}
		////////////////////
	}
	/////////////////
}

char g_ActionName[][32] =
{
	"聊天",
	"好友",
	"交易",
	"入队",
	"组队",
	"跟随",
	"信息",
	"帮会",
	"黑名单",
	"仇杀",
	"送钱",
	"公频关键字",
};

void PopUpContextPeopleMenu(const KUiPlayerItem& SelectPlayer, int x, int y)
{
	if (SelectPlayer.Name[0] == 0)
		return;

	if (KUiPlayerBar::IsSelfName((char*)SelectPlayer.Name))
		return;

	int nbIsRecruit = g_pCoreShell->TongOperation(GTOI_TONG_IS_RECRUIT, (unsigned int)&SelectPlayer, 0);
	int nActionDataCount = sizeof(g_ActionName) / 32;

	KPopupMenuData* pMenuData = (KPopupMenuData*)malloc(MENU_DATA_SIZE(nActionDataCount));
	if (pMenuData == NULL)
		return;

	KPopupMenu::InitMenuData(pMenuData, nActionDataCount);
	pMenuData->usMenuFlag |= PM_F_AUTO_DEL_WHEN_HIDE;
	pMenuData->usMenuFlag &= ~PM_F_HAVE_HEAD_TAIL_IMG;

	int nStallState = g_pCoreShell->GetGameData(GDI_IS_STALL, -1, -1);
	int nStallStateOther = g_pCoreShell->GetGameData(GDI_IS_STALL, SelectPlayer.nIndex, -1);
	KUiPlayerTeam TeamInfo;
	TeamInfo.nCaptainPower = 0;
	int nbInTeam = g_pCoreShell->TeamOperation(TEAM_OI_GD_INFO, (unsigned int)&TeamInfo, 0);
	int nMenuCount = 0;
	int i;
	pMenuData->nItemHeight = 0;
	for (i = 0; i < nActionDataCount; i++)
	{
		if ((i == ACTION_JOINTEAM && !nStallState && !nStallStateOther && SelectPlayer.nIndex != -1 && SelectPlayer.nData == PLAYER_MENU_STATE_TEAMOPEN && !nbInTeam) || //"申请入队", 对方未打开队伍时不能加入
			(i == ACTION_TRADE && !nStallState && !nStallStateOther && SelectPlayer.nIndex != -1 && SelectPlayer.nData == PLAYER_MENU_STATE_TRADEOPEN) ||	//"交易物品", 对方未打开交易时不能加入
			(i == ACTION_MAKEFRIEND && !KUiChatCentre::IsMyFriend((char*)SelectPlayer.Name)) || //"加为好友", 对方已是我的好友时不能再邀请
			(i == ACTION_INVITETEAM && !nStallState && !nStallStateOther && SelectPlayer.uId != 0 && TeamInfo.nCaptainPower > 0 && (!nbInTeam || TeamInfo.bTeamLeader))	||	//"邀请加入", 队伍不可以加人了就不能邀请加入
			(i == ACTION_FOLLOW && !nStallState && !nStallStateOther && SelectPlayer.nIndex != -1) ||
			(i == ACTION_REVENGE && !nStallState && SelectPlayer.uId != 0) ||
			(i == ACTION_VIEWITEM && SelectPlayer.uId != 0) ||
			(i == ACTION_TONG && !nStallState && !nStallStateOther && SelectPlayer.nIndex != -1 && nbIsRecruit) ||
			(i == ACTION_BLACKLIST && !(KUiChatCentre::IsMyFriend((char*)SelectPlayer.Name) || IsInBlackName((char*)SelectPlayer.Name))) ||
			(i == ACTION_CHAT) ||
			(i == ACTION_GIVE_MONEY && !nStallState && !nStallStateOther && SelectPlayer.uId != 0 && 0) ||
			(i == ACTION_FILTER_WORLD_CHANNEL && KUiMsgCentrePad::IsPopupFilterItem())
			)
		{
			if (i == ACTION_INVITETEAM)
			{
				KUiPlayerRuntimeInfo	Self, Other;
				g_pCoreShell->GetGameData(GDI_PLAYER_RT_INFO, (unsigned int)&Self, 0);
				g_pCoreShell->GetGameData(GDI_NPC_OVERVIEW, SelectPlayer.nIndex, (int)&Other);
				if (Self.nCurrentCamp == camp_begin && Other.nCurrentCamp != camp_begin)
                    continue;
			}
			strcpy(pMenuData->Items[nMenuCount].szData, g_ActionName[i]);
			pMenuData->Items[nMenuCount].uDataLen = strlen(g_ActionName[i]);
			pMenuData->Items[nMenuCount].uID = i;
			nMenuCount++;
		}
	}
	pMenuData->nNumItem = nMenuCount;

	pMenuData->nX = x;
	pMenuData->nY = y;
	g_WndGameSpace.m_SelectPlayer = SelectPlayer;
	KPopupMenu::Popup(pMenuData, &g_WndGameSpace, SEL_ACTION_MENU);
}

void PopUpContextEmoteMenu(char* szDest, int x, int y)
{
	if (szDest == NULL || szDest[0] == 0)
		return;

	if (KUiPlayerBar::IsSelfName(szDest))
		return;

	int nEmoteCount = g_UiChatPhrase.GetEmoteCount((KUiChatPhrase::EMOTE_TYPE)(1));

	KPopupMenuData* pMenuData = (KPopupMenuData*)malloc(MENU_DATA_SIZE(nEmoteCount));
	if (pMenuData == NULL)
	{
		return;
	}

	KPopupMenu::InitMenuData(pMenuData, nEmoteCount);
	pMenuData->usMenuFlag |= PM_F_AUTO_DEL_WHEN_HIDE;
	pMenuData->usMenuFlag &= ~PM_F_HAVE_HEAD_TAIL_IMG;

	int i, nBuffLen;
	char szBuff[128];
	pMenuData->nItemHeight = 0;
	for (i = 0; i < nEmoteCount; i++)
	{
		nBuffLen = g_UiChatPhrase.GetMenuEmoteName(i, szBuff, sizeof(szBuff));
		memcpy(pMenuData->Items[i].szData, szBuff, nBuffLen);
		pMenuData->Items[i].uDataLen = nBuffLen;
		pMenuData->Items[i].uID = i;
	}
	pMenuData->nNumItem = nEmoteCount;

	pMenuData->nX = x;
	pMenuData->nY = y;
	strncpy(g_WndGameSpace.m_SelectPlayer.Name, szDest, 32);
	g_WndGameSpace.m_SelectPlayer.nIndex = -1;
	g_WndGameSpace.m_SelectPlayer.uId = 0;
	g_WndGameSpace.m_SelectPlayer.Name[31] = 0;
	KPopupMenu::Popup(pMenuData, &g_WndGameSpace, SEL_EMOTE_MENU);
}