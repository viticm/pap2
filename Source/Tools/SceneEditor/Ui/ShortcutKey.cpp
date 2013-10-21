 /*****************************************************************************************
//	快捷键系统
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2003-1-13
*****************************************************************************************/
#include "KWin32.h"
#include "KIniFile.h"
#include "ShortcutKey.h"
#include "UiBase.h"
#include "UiCase/UiTeamManage.h"
#include "UiCase/UiOptions.h"
#include "UiCase/UiStatus.h"
#include "UiCase/UiItem.h"
#include "UiCase/UiSkills.h"
#include "UiCase/UiChannelSubscibe.h"
#include "UiCase/UiPlayerBar.h"
#include "UiCase/UiESCDlg.h"
#include "UiCase/UiSkillTree.h"
#include "UiCase/UiMiniMap.h"
#include "UiCase/UiMsgCentrePad.h"
#include "UiCase/UiChatCentre.h"
#include "UICase/UiFaceSelector.h"
#include "UICase/UiHelper.h"
#include "UICase/UiHelper2.h"
#include "UICase/UiTaskNote.h"
#include "UICase/UiStallSetAdv.h"
#include "UiCase/UiTrade.h"
#include "UiCase/UiShop.h"
#include "UiCase/UiStoreBox.h"
#include "UiCase/UiHeaderControlBar.h"
#include "UiCase/UiToolsControlBar.h"
#include "UiCase/UiGame.h"
#include "UiCase/UiNewsMessage.h"
#include "UiCase/uisysmsgcentre.h"
#include "UiCase/UiTongManager.h"
#include "UiCase/UiTongCreateSheet.h"
#include "UiCase/UiTrembleItem.h"
#include "UiCase/UiPortrait.h"
#include "UiCase/UiDiceWeaveBingo.h"
#include "UiCase/UiNewPlayer.h"
#include "Elem/SpecialFuncs.h"
#include "Elem/Wnds.h"

#include "UiShell.h"


#include "KProtocol.h"
#include "KRelayProtocol.h"
#include "KTongProtocol.h"


#include "../../core/Src/KPlayerMenuState.h"

#include "../../core/src/coreshell.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

extern iCoreShell*		g_pCoreShell;

KLuaScript KShortcutKeyCentre::ms_Script;
COMMAND_SETTING* KShortcutKeyCentre::ms_pCommands = NULL;
int KShortcutKeyCentre::ms_nCommands = 0;
bool KShortcutKeyCentre::ms_Enable = false;
SHORTFUNCMAP KShortcutKeyCentre::ms_FunsMap;

bool KShortcutKeyCentre::ms_bMouse = false;
int KShortcutKeyCentre::ms_MouseX = 0;
int KShortcutKeyCentre::ms_MouseY = 0;

#define	UI_AUTOEXEC_SETTING_FILE	"\\Ui\\autoexec.lua"
#define GAME_CONFIG_FILE            "\\config.ini"

enum SCREEN_MODE
{
	SCREEN_MODE_1D = 1,
	SCREEN_MODE_2D = 2,
	SCREEN_MODE_3D = 3,
};

//////////////////////////////////////
//x assist funcs

static inline bool __x_isgraph(char c)
{
	return c < 0 || isgraph(c);
}

inline bool __x_memcpy_n(void* d, size_t l, const void* s, size_t n)
{
        if (l < n)
                return false;
        memcpy(d, s, n);
        return true;
}

//////////////////////////////////////


int	KShortcutKeyCentre::HandleKeyInput(unsigned int uKey, int nModifier)
{
	//暂时用这个方式判定ALT Up And Down咯
	if(uKey == VK_MENU && g_pCoreShell)
	{
		if((nModifier & HOTKEYF_ALT) == HOTKEYF_ALT)
			g_pCoreShell->OperationRequest(GOI_SHOW_GAMESPACE_ITEM_NAME, 0, 1);
		else
			g_pCoreShell->OperationRequest(GOI_SHOW_GAMESPACE_ITEM_NAME, 0, 0);
	}
	//end
	int nIndex = FindCommand(MAKELONG(uKey, nModifier));
	if (nIndex >= 0)
	{
		return ExcuteScript(ms_pCommands[nIndex].szDo);
	}

	return false;
}

int	KShortcutKeyCentre::HandleMouseInput(unsigned int uKey, int nModifier, int x, int y)
{
	if (ms_bMouse)
		return false;

	ms_bMouse = true;
	ms_MouseX = x;
	ms_MouseY = y;
	int nIndex = FindCommand(MAKELONG(uKey, nModifier));
	int nRet = false;
	if (nIndex >= 0)
	{
		nRet = ExcuteScript(ms_pCommands[nIndex].szDo);
	}

	ms_bMouse = false;

	return nRet;
}

void KShortcutKeyCentre::Enable(bool b)
{
	ms_Enable = b;
}

/////////////////////////////////////////////////////////////////////////

//窗口列表
char* l_WindowList[] =
{
	"team",		//0 队伍
	"map",		//1 地图
	"status",	//2 状态
	"Items",	//3 物品
	"skills",	//4 技能
	"system",	//5 系统
	"friend",	//6 好友
	"help",		//7 详细帮助
	"tasknote",	//8 任务记事
	"leftskill",	//9 左手技能
	"rightskill",	//10 右手技能
	"commandline",	//11 命令行
	"options",		//12 选项
	"statustool",	//13 状态工具条
	"normaltool",	//14 常用工具条
	"chatroom",		//15 聊天窗口
	"newsmessage",  //16 新闻窗口
	"tongcreate",   //17 帮会创建窗口
	"tongmanager",  //18 帮会管理
	"beset",		//19 打造界面
	"portrait",		//20 选择头像界面
	"DiceWeave",	//21 动感色子老虎机
};

int FindWindow(const char* szname)
{
	for (int i = 0; i < sizeof(l_WindowList) / sizeof(char*); i++)
	{
		if (strcmpi(l_WindowList[i], szname) == 0)
			return i;
	}
	return -1;
}

bool UiCloseWndsInGame(bool bAll);


int LuaChangeMiniMap(Lua_State * L)
{
	if (Lua_GetTopIndex(L) != 1)
		return 0;

	int nIndex = (int)Lua_ValueToNumber(L, 1);
	if(g_pCoreShell)
	{
		g_pCoreShell->SceneMapOperation(GSMOI_SCENE_CHANGE_MAP, 0, nIndex);
	}

	return 0;
}

int LuaOpenWindow(Lua_State * L)
{
	if (Lua_GetTopIndex(L) != 1)
		return 0;

	char * strWindow = (char *)Lua_ValueToString(L, 1);

	if (strWindow)
	{
		switch(FindWindow(strWindow))
		{
		case 0:	//队伍
			if (KUiTeamManage::GetIfVisible())
				KUiTeamManage::CloseWindow();
			else
			{
				KUiTeamManage::OpenWindow();
			}
			break;
		case 1:	//地图
			MapToggleStatus();
			break;
		case 2:	//状态
			if (KUiStatus::GetIfVisible())
				KUiStatus::CloseWindow(false);
			else
				KUiStatus::OpenWindow();
			break;
		case 3:	//物品
			if (KUiItem::GetIfVisible())
				KUiItem::CloseWindow(false);
			else
				KUiItem::OpenWindow();
			break;
		case 4:	//技能
			if (KUiSkills::GetIfVisible())
				KUiSkills::CloseWindow(false);
			else
				KUiSkills::OpenWindow();
			break;
		case 5:	//系统
			if (UiCloseWndsInGame(false) == false)
				KUiESCDlg::OpenWindow();
			break;
			//选项
		case 6: //好友
			if (KUiChatCentre::GetIfVisible())
				KUiChatCentre::CloseWindow(false);
			else
				KUiChatCentre::OpenWindow(true);
			break;
		case 7: //详细帮助
			if (KUiHelper2::GetIfVisible())
				KUiHelper2::CloseWindow(false);
			else
				KUiHelper2::OpenWindow(true);
			break;
		case 8: //任务记事
			if (KUiTaskNote::GetIfVisible())
				KUiTaskNote::CloseWindow(false);
			else
				KUiTaskNote::OpenWindow();
			break;
		case 9: //左手技能
			if (KUiSkillTree::GetIfVisible())
				KUiSkillTree::CloseWindow(false);
			else
				KUiSkillTree::OpenWindow(true);
			break;
		case 10: //右手技能
			if (KUiSkillTree::GetIfVisible())
				KUiSkillTree::CloseWindow(false);
			else
				KUiSkillTree::OpenWindow(false);
			break;
		case 12: //选项
			if (KUiOptions::GetIfVisible())
				KUiOptions::CloseWindow();
			else
				KUiOptions::OpenWindow();
			break;
		case 16: //新闻
			if (KUiNewsMessage::GetIfVisible())
				KUiNewsMessage::CloseWindow();
			else
				KUiNewsMessage::OpenWindow();
			break;
		case 17: //帮会创建
			if (KUiTongCreateSheet::GetIfVisible())
				KUiTongCreateSheet::CloseWindow();
			else
				KUiTongCreateSheet::OpenWindow();
			break;
		case 18: //帮会管理
			if (KUiTongManager::GetIfVisible())
				KUiTongManager::CloseWindow();
			else
				KUiTongManager::OpenWindow();
			break;
		case 19: //打造界面
			if (KUiTrembleItem::GetIfVisible())
				KUiTrembleItem::CloseWindow();
			else
				KUiTrembleItem::OpenWindow();
			break;
		case 20: //选择头像界面
			if (KUiPortrait::GetIfVisible())
				KUiPortrait::CloseWindow();
			else
				KUiPortrait::OpenWindow();
			break;
		case 21:
			if (KUiDiceWeaveBingo::GetIfVisible())
				KUiDiceWeaveBingo::CloseWindow();
			else
				KUiDiceWeaveBingo::OpenWindow();
			break;
		}
	}

	return 0;
}

KWndWindow* FindWndWindow(const char* szname)
{
	KWndWindow* pWin = NULL;
	switch(FindWindow(szname))
	{
	case 1:		//1 地图
		pWin = KUiMiniMap::GetSelf();
		break;
	case 13:	//13 状态工具条
		pWin = KUiHeaderControlBar::GetSelf();
		break;
	case 14:	//14 常用工具条
		pWin = KUiToolsControlBar::GetSelf();
		break;
	case 15:	//15 聊天窗口
		pWin = KUiMsgCentrePad::GetSelf();
		break;
	}

	return pWin;
}


int LuaMoveWindow(Lua_State * L)
{
	if (Lua_GetTopIndex(L) != 3)
		return 0;

	char * strWindow = (char *)Lua_ValueToString(L, 1);
	int nX = (int)(Lua_ValueToNumber(L, 2));
	int nY = (int)(Lua_ValueToNumber(L, 3));

	KWndWindow* pWin = FindWndWindow(strWindow);
	if (pWin)
	{
		pWin->SetPosition(nX, nY);
	}
	return 0;
}

int LuaSizeWindow(Lua_State * L)
{
	if (Lua_GetTopIndex(L) != 3)
		return 0;

	char * strWindow = (char *)Lua_ValueToString(L, 1);
	int nW = (int)(Lua_ValueToNumber(L, 2));
	int nH = (int)(Lua_ValueToNumber(L, 3));

	KWndWindow* pWin = FindWndWindow(strWindow);
	if (pWin)
	{
		pWin->SetSize(nW, nH);
	}
	return 0;
}

int LuaShowWindow(Lua_State * L)
{
	if (Lua_GetTopIndex(L) != 2)
		return 0;

	char * strWindow = (char *)Lua_ValueToString(L, 1);
	int nShow = (int)(Lua_ValueToNumber(L, 2));

	if (strWindow)
	{
		KWndWindow* pWin = FindWndWindow(strWindow);

		if (pWin)
		{
			if (nShow)
				pWin->Show();
			else
				pWin->Hide();
		}
	}
	return 0;
}

int LuaFocusWindow(Lua_State * L)
{
	if (Lua_GetTopIndex(L) != 1)
		return 0;

	char * strWindow = (char *)Lua_ValueToString(L, 1);

	if (strWindow)
	{
		switch(FindWindow(strWindow))
		{
		case 11:	// 命令行
			KUiPlayerBar::InputNameMsg(false, "", true);
			break;
		}
	}

	return 0;
}

int LuaShortcutSkill(Lua_State * L)
{
	if (Lua_GetTopIndex(L) != 1)
		return 0;

	int nIndex = (int)Lua_ValueToNumber(L, 1);

	KUiSkillTree::HandleShortcutKey(nIndex);

	return 0;
}

int LuaDirectShortcutSkill(Lua_State * L)
{
	if (Lua_GetTopIndex(L) != 1)
		return 0;

	int nIndex = (int)Lua_ValueToNumber(L, 1);

	KUiSkillTree::DirectHandleShortcutKey(nIndex);

	return 0;
}

int LuaShortcutUseItem(Lua_State * L)
{
	if (Lua_GetTopIndex(L) != 1)
		return 0;

	int nIndex = (int)Lua_ValueToNumber(L, 1);

	//快捷物品的使用
	KUiPlayerBar::OnUseItem(nIndex);

	return 0;
}

//状态列表
char* l_StatusList[] =
{
	"run",		//0 跑步
	"sit",		//1 打坐
	"trade",	//2 交易
	"pk",		//3 PK
	"horse",	//4 马
	"showplayername",	//5 显示玩家名字
	"showplayerlife",	//6 显示玩家生命
	"showplayermana",	//7 显示玩家内力
	"showplayernumber",	//8 界面显示玩家数字
};

int FindStatus(const char* szname)
{
	for (int i = 0; i < sizeof(l_StatusList) / sizeof(char*); i++)
	{
		if (strcmpi(l_StatusList[i], szname) == 0)
			return i;
	}
	return -1;
}


int LuaSwitchStatus(Lua_State * L)
{
	int nParamCount = Lua_GetTopIndex(L);
	if (nParamCount != 1 && nParamCount != 2)
		return 0;

	char * strStatus = (char *)Lua_ValueToString(L, 1);
	int nStatus = -1;

	if(nParamCount == 2)
	{
		nStatus = (int)Lua_ValueToNumber(L, 2);
	}

	if (strStatus)
	{
		switch(FindStatus(strStatus))
		{
		case 0:		//跑步
			if (g_pCoreShell)
				g_pCoreShell->OperationRequest(GOI_PLAYER_ACTION, PA_RUN, 0);
			break;
		case 1:		//打坐
			if (g_pCoreShell)
				g_pCoreShell->OperationRequest(GOI_PLAYER_ACTION, PA_SIT, 0);
			break;
		case 2:	//交易
			if (g_pCoreShell)
				g_pCoreShell->OperationRequest(GOI_TRADE_WILLING, 0, true);
			break;
		case 3: //PK
			if (g_pCoreShell)
			{
				if(nStatus == -1)
				{
					g_pCoreShell->OperationRequest(GOI_PK_SETTING, 0,
						!g_pCoreShell->GetGameData(GDI_PK_SETTING, 0, 0));
				}
				else
				{
					g_pCoreShell->OperationRequest(GOI_PK_SETTING, 0, nStatus);
				}
			}
			break;
		case 4: //马
			if (g_pCoreShell)
			{
				KUiPlayerRuntimeInfo RTInfo;
				g_pCoreShell->GetGameData(GDI_PLAYER_RT_INFO, (unsigned int)&RTInfo, 0);
				if ((RTInfo.byActionDisable & PA_RIDE) == 0)
				{
					g_pCoreShell->OperationRequest(GOI_SWITCH_HORSE,
						(RTInfo.byAction & PA_RIDE) ? 1 : 0, 0);
				}
			}
			break;
		case 5:	//显示玩家名字
			if (g_pCoreShell)
			{
				g_pCoreShell->OperationRequest(GOI_SHOW_PLAYERS_NAME, 0,
					!g_pCoreShell->GetGameData(GDI_SHOW_PLAYERS_NAME, 0, 0));
			}
			break;
		case 6:	//显示玩家生命
			if (g_pCoreShell)
			{
				g_pCoreShell->OperationRequest(GOI_SHOW_PLAYERS_LIFE, 0,
					!g_pCoreShell->GetGameData(GDI_SHOW_PLAYERS_LIFE, 0, 0));
			}
			break;
		case 7:	//显示玩家内力
			if (g_pCoreShell)
			{
				g_pCoreShell->OperationRequest(GOI_SHOW_PLAYERS_MANA, 0,
					!g_pCoreShell->GetGameData(GDI_SHOW_PLAYERS_MANA, 0, 0));
			}
			break;
		case 8: //界面显示玩家数字
			Player_Life::m_bText = !Player_Life::m_bText;
			Player_Mana::m_bText = !Player_Mana::m_bText;
			Player_Stamina::m_bText = !Player_Stamina::m_bText;
			Player_Exp::m_bText = !Player_Exp::m_bText;
			break;
		case 9:
			KUiPlayerBar::SwitchChannel();
			break;
		}
	}

	return 0;
}

int LuaSwitchChannel(Lua_State * L)
{
	int nbUp = 0;

	if (Lua_GetTopIndex(L) == 1)
	{
		nbUp = (int)Lua_ValueToNumber(L, 1);
	}
	KUiPlayerBar::SwitchChannel(nbUp);

	return 0;
}

int LuaRegisterEvent(Lua_State * L)
{
	if (Lua_GetTopIndex(L) == 2)
	{
		char * szName = (char *)Lua_ValueToString(L, 1);
		int hWnd = (int)Lua_ValueToNumber(L, 2);

		g_UiBase.RegisterEvent(szName, (HANDLE)hWnd);
	}

	return 0;
}

int LuaUnregisterEvent(Lua_State * L)
{
	if (Lua_GetTopIndex(L) == 2)
	{
		char * szName = (char *)Lua_ValueToString(L, 1);
		int hWnd = (int)Lua_ValueToNumber(L, 2);

		g_UiBase.UnregisterEvent(szName, (HANDLE)hWnd);
	}

	return 0;
}

int LuaSendEvent(Lua_State * L)
{
	if (Lua_GetTopIndex(L) == 2)
	{
		char * szName = (char *)Lua_ValueToString(L, 1);

		int nType = Lua_GetValueType(L, 2);
		char * szEvent = NULL;

		if (nType == LUA_TSTRING)
			szEvent = (char *)Lua_ValueToString(L, 2);
		else if (nType == LUA_TNUMBER)
		{
			int nNum = (int)Lua_ValueToNumber(L, 2);
			static char szNum[64];
			sprintf(szNum, "%d", nNum);
			szEvent = szNum;
		}

		if (szEvent)
			g_UiBase.NotifyOneEvent(szName, szEvent);
	}

	return 0;
}

extern 	int		g_bRepresent3;
extern int	g_bScreen;
int LuaGetAppStatus(Lua_State * L)
{
	int nRet = -1;
	if (Lua_GetTopIndex(L) == 1)
	{
		char * szName = (char *)Lua_ValueToString(L, 1);
		if (szName)
		{
			if (strcmpi(szName, "FullWindow") == 0)
			{
				nRet = g_bScreen ? 1 : 0;
			}
			else if (strcmpi(szName, "3D") == 0)
			{
				nRet = g_bRepresent3 ? 1 : 0;
			}
			else if (strcmpi(szName, "2D") == 0)
			{
				nRet = !g_bRepresent3 ? 1 : 0;
			}
		}
	}
	Lua_PushNumber(L, nRet);
	return 1;
}

namespace hotkey_str
{
	std::string DescHotKey(DWORD hk)
	{
		static const char* modidesc_table[] = {
			//	0		1		2		3		4		5		6		7
			"Shift",	"Ctrl",	"Alt",	"Ext",	"",		"",		"",		""
		};
		static const char* vkeydesc_table[] = {
		//	0			1			2			3			4			5			6			7
		//	8			9			A			B			C			D			E			F
			"",			"LButton",	"RButton",	"Cancel",	"MButton",	"",			"",			"",				//0
			"BackSpace","Tab",		"",			"",			"Clear",	"Enter",	"",			"",
			"",			"",			"",			"Pause",	"CapLock",	"",			"",			"",				//1
			"",			"",			"",			"ESC",		"Convert",	"NonConvert","Accept",	"ModeChange",
			"Space",	"PageUp",	"PageDown",	"End",		"Home",		"Left",		"Up",		"Right",		//2
			"Down",		"Select",	"Print",	"Execute",	"PrintScreen",	"Insert",	"Delete",	"Help",
			"0",		"1",		"2",		"3",		"4",		"5",		"6",		"7",			//3
			"8",		"9",		"",			"",			"",			"",			"",			"",
			"",			"A",		"B",		"C",		"D",		"E",		"F",		"G",			//4
			"H",		"I",		"J",		"K",		"L",		"M",		"N",		"O",
			"P",		"Q",		"R",		"S",		"T",		"U",		"V",		"W",			//5
			"X",		"Y",		"Z",		"Windows",	"",			"Menu",		"",			"",
			"Num0",		"Num1",		"Num2",		"Num3",		"Num4",		"Num5",		"Num6",		"Num7",			//6
			"Num8",		"Num9",		"Num*",		"Num+",		"Separator","Num-",		"Num.",		"Num/",
			"F1",		"F2",		"F3",		"F4",		"F5",		"F6",		"F7",		"F8",			//7
			"F9",		"F10",		"F11",		"F12",		"F13",		"F14",		"F15",		"F16",
			"F17",		"F18",		"F19",		"F20",		"F21",		"F22",		"F23",		"F24",			//8
			"",			"",			"",			"",			"",			"",			"",			"",
			"NumLock",	"ScrollLock","",		"",			"",			"",			"",			"",				//9
			"",			"",			"",			"",			"",			"",			"",			"",
			"",			"",			"",			"",			"",			"",			"",			"",				//A
			"",			"",			"",			"",			"",			"",			"",			"",
			"",			"",			"",			"",			"",			"",			"",			"",				//B
			"",			"",			";",		"=",		",",		"-",		".",		"/",
			"`",		"",			"",			"",			"",			"",			"",			"",				//C
			"",			"",			"",			"",			"",			"",			"",			"",
			"",			"",			"",			"",			"",			"",			"",			"",				//D
			"",			"",			"",			"[",		"\\",		"]",		"'",		"",
			"",			"",			"",			"",			"",			"",			"",			"",				//E
			"",			"",			"",			"",			"",			"",			"",			"",
			"",			"",			"",			"",			"",			"",			"",			"",				//F
			"",			"",			"",			"",			"",			"",			"",			"",
			"LDButton", "RDButton", "MDButton", "",			"",			"",			"",			"",				//10
			"",			"",			"",			"",			"",			"",			"",			"",
		};
		static const size_t count_moditbl = sizeof(modidesc_table) / sizeof(modidesc_table[0]);
		static const size_t count_vkeytbl = sizeof(vkeydesc_table) / sizeof(vkeydesc_table[0]);


		static const char STR_DELIMITER[] = " + ";
		static const size_t LEN_DELIMITER = sizeof(STR_DELIMITER) - sizeof(STR_DELIMITER[0]);


		const WORD modi = HIWORD(hk);
		const WORD vkey = LOWORD(hk);
		if ((modi & 0xFF00) || (vkey >= count_vkeytbl))
			return "";

		const char* szVkDesc = vkeydesc_table[vkey];
		if (!szVkDesc[0])
			return "";


		std::string desc;

		{{
		for (size_t pos = 0; pos <= count_moditbl; pos++)
		{
			const char* szDesc = NULL;

			if (pos < count_moditbl)
			{
				if (!(modi & (0x01 << pos)))
					continue;
				szDesc = modidesc_table[pos];
				if (!szDesc[0])
					return "";
			}
			else
			{
				szDesc = szVkDesc;
			}

			if (!desc.empty())
				desc += STR_DELIMITER;
			desc += szDesc;
		}
		}}

		return desc;
	}

	DWORD ParseHotKey(const std::string& desc)
	{
		static const struct PATTERNMAP
		{
			enum {MASK_VKEY = 0x0000FFFF};

			typedef std::pair<DWORD, DWORD>	HOTKEYPART;
			typedef std::map<std::string, HOTKEYPART, string_iless>	DESC2HKPMAP;
			DESC2HKPMAP theMap;

			PATTERNMAP()
			{
				const struct _PATTERN
				{
					char* desc;
					DWORD mask;
					DWORD value;
				} pattern_table[] = {
					//modifier
					{"Shift", HOTKEYF_SHIFT<<16, HOTKEYF_SHIFT<<16}, {"Control", HOTKEYF_CONTROL<<16, HOTKEYF_CONTROL<<16},
					{"Alt", HOTKEYF_ALT<<16, HOTKEYF_ALT<<16}, {"Ext", HOTKEYF_EXT<<16, HOTKEYF_EXT<<16},

					//vk
					{"LButton", MASK_VKEY, VK_LBUTTON}, {"RButton", MASK_VKEY, VK_RBUTTON},
					{"Cancel", MASK_VKEY, VK_CANCEL}, {"MButton", MASK_VKEY, VK_MBUTTON},
					{"BackSpace", MASK_VKEY, VK_BACK}, {"Tab", MASK_VKEY, VK_TAB}, {"Clear", MASK_VKEY, VK_CLEAR},
					{"Return", MASK_VKEY, VK_RETURN}, {"Pause", MASK_VKEY, VK_PAUSE},
					{"Convert", MASK_VKEY, VK_CONVERT}, {"NonConvert", MASK_VKEY, VK_NONCONVERT},
					{"Accept", MASK_VKEY, VK_ACCEPT}, {"ModeChange", MASK_VKEY, VK_MODECHANGE},
					{"Escape", MASK_VKEY, VK_ESCAPE}, {"Space", MASK_VKEY, VK_SPACE},
					{"Prior", MASK_VKEY, VK_PRIOR}, {"Next", MASK_VKEY, VK_NEXT}, {"End", MASK_VKEY, VK_END}, {"Home", MASK_VKEY, VK_HOME},
					{"Left", MASK_VKEY, VK_LEFT}, {"Up", MASK_VKEY, VK_UP}, {"Right", MASK_VKEY, VK_RIGHT}, {"Down", MASK_VKEY, VK_DOWN},
					{"Insert", MASK_VKEY, VK_INSERT}, {"Delete", MASK_VKEY, VK_DELETE},
					{"Select", MASK_VKEY, VK_SELECT}, {"Print", MASK_VKEY, VK_PRINT}, {"Execute", MASK_VKEY, VK_EXECUTE},
					{"SnapShot", MASK_VKEY, VK_SNAPSHOT}, {"Help", MASK_VKEY, VK_HELP},
					{"0", MASK_VKEY, '0'}, {"1", MASK_VKEY, '1'}, {"2", MASK_VKEY, '2'}, {"3", MASK_VKEY, '3'},
					{"4", MASK_VKEY, '4'}, {"5", MASK_VKEY, '5'}, {"6", MASK_VKEY, '6'}, {"7", MASK_VKEY, '7'},
					{"8", MASK_VKEY, '8'}, {"9", MASK_VKEY, '9'},
					{"A", MASK_VKEY, 'A'}, {"B", MASK_VKEY, 'B'}, {"C", MASK_VKEY, 'C'}, {"D", MASK_VKEY, 'D'},
					{"E", MASK_VKEY, 'E'}, {"F", MASK_VKEY, 'F'}, {"G", MASK_VKEY, 'G'}, {"H", MASK_VKEY, 'H'},
					{"I", MASK_VKEY, 'I'}, {"J", MASK_VKEY, 'J'}, {"K", MASK_VKEY, 'K'}, {"L", MASK_VKEY, 'L'},
					{"M", MASK_VKEY, 'M'}, {"N", MASK_VKEY, 'N'}, {"O", MASK_VKEY, 'O'}, {"P", MASK_VKEY, 'P'},
					{"Q", MASK_VKEY, 'Q'}, {"R", MASK_VKEY, 'R'}, {"S", MASK_VKEY, 'S'}, {"T", MASK_VKEY, 'T'},
					{"U", MASK_VKEY, 'U'}, {"V", MASK_VKEY, 'V'}, {"W", MASK_VKEY, 'W'}, {"X", MASK_VKEY, 'X'},
					{"Y", MASK_VKEY, 'Y'}, {"Z", MASK_VKEY, 'Z'},
					{"Num0", MASK_VKEY, VK_NUMPAD0}, {"Num1", MASK_VKEY, VK_NUMPAD1}, {"Num2", MASK_VKEY, VK_NUMPAD2}, {"Num3", MASK_VKEY, VK_NUMPAD3},
					{"Num4", MASK_VKEY, VK_NUMPAD4}, {"Num5", MASK_VKEY, VK_NUMPAD5}, {"Num6", MASK_VKEY, VK_NUMPAD6}, {"Num7", MASK_VKEY, VK_NUMPAD7},
					{"Num8", MASK_VKEY, VK_NUMPAD8}, {"Num9", MASK_VKEY, VK_NUMPAD9},
					{"Num+", MASK_VKEY, VK_ADD}, {"Num-", MASK_VKEY, VK_SUBTRACT}, {"Num*", MASK_VKEY, VK_MULTIPLY}, {"Num/", MASK_VKEY, VK_DIVIDE},
					{"Separator", MASK_VKEY, VK_SEPARATOR}, {"Num.", MASK_VKEY, VK_DECIMAL},
					{"F1", MASK_VKEY, VK_F1}, {"F2", MASK_VKEY, VK_F2}, {"F3", MASK_VKEY, VK_F3}, {"F4", MASK_VKEY, VK_F4},
					{"F5", MASK_VKEY, VK_F5}, {"F6", MASK_VKEY, VK_F6}, {"F7", MASK_VKEY, VK_F7}, {"F8", MASK_VKEY, VK_F8},
					{"F9", MASK_VKEY, VK_F9}, {"F10", MASK_VKEY, VK_F10}, {"F11", MASK_VKEY, VK_F11}, {"F12", MASK_VKEY, VK_F12},
					{"F13", MASK_VKEY, VK_F13}, {"F14", MASK_VKEY, VK_F14}, {"F15", MASK_VKEY, VK_F15}, {"F16", MASK_VKEY, VK_F16},
					{"F17", MASK_VKEY, VK_F17}, {"F18", MASK_VKEY, VK_F18}, {"F19", MASK_VKEY, VK_F19}, {"F20", MASK_VKEY, VK_F20},
					{"F21", MASK_VKEY, VK_F21}, {"F22", MASK_VKEY, VK_F22}, {"F23", MASK_VKEY, VK_F23}, {"F24", MASK_VKEY, VK_F24},
					{"CapLock", MASK_VKEY, VK_CAPITAL}, {"NumLock", MASK_VKEY, VK_NUMLOCK}, {"ScrollLock", MASK_VKEY, VK_SCROLL},
					{";", MASK_VKEY, 0x00BA},	{"=", MASK_VKEY, 0x00BB},	{",", MASK_VKEY, 0x00BC},	{"-", MASK_VKEY, 0x00BD},
					{".", MASK_VKEY, 0x00BE},	{"/", MASK_VKEY, 0x00BF},	{"`", MASK_VKEY, 0x00C0},
					{"[", MASK_VKEY, 0x00DB},
					{"\\", MASK_VKEY, 0x00DC}, {"]", MASK_VKEY, 0x00DD}, {"'", MASK_VKEY, 0x00DE},


					//modifier alias
					{"Ctrl", HOTKEYF_CONTROL<<16, HOTKEYF_CONTROL<<16}, {"Menu", HOTKEYF_ALT<<16, HOTKEYF_ALT<<16},
					{"Break", MASK_VKEY, VK_PAUSE},

					//vk alias
					{"ESC", MASK_VKEY, VK_ESCAPE}, {"Enter", MASK_VKEY, VK_RETURN},
					{"BACK", MASK_VKEY, VK_BACK},
					{"INS", MASK_VKEY, VK_INSERT}, {"DEL", MASK_VKEY, VK_DELETE},
					{"PageUp", MASK_VKEY, VK_PRIOR}, {"PageDown", MASK_VKEY, VK_NEXT},
					{"ScrlLock", MASK_VKEY, VK_SCROLL},
					{"NumAdd", MASK_VKEY, VK_ADD}, {"NumSub", MASK_VKEY, VK_SUBTRACT}, {"NumMul", MASK_VKEY, VK_MULTIPLY}, {"NumDiv", MASK_VKEY, VK_DIVIDE},
					{"NumDecimal", MASK_VKEY, VK_DECIMAL},
					{"PrintScreen", MASK_VKEY, VK_SNAPSHOT},

					{"LDButton", MASK_VKEY, VK_LDBUTTON}, {"RDButton", MASK_VKEY, VK_RDBUTTON}, {"MDButton", MASK_VKEY, VK_MDBUTTON},
				};
				for (size_t i = 0; i < sizeof(pattern_table)/sizeof(pattern_table[0]); i++)
				{
					const _PATTERN& pat = pattern_table[i];
					theMap[std::string(pat.desc)] = std::make_pair(pat.mask, pat.value);
				}
			}
		} s_mapPattern;

		static const char CH_DELIMITER = '+';


		if (desc.empty())
			return 0;

		DWORD hkcode = 0;

		const char* szToken = desc.c_str(), * szLimit = NULL, * szNext = NULL;
		for ( ; *szToken; szToken = szNext)
		{
			for (szNext = NULL, szLimit = szToken; *szLimit; szLimit++)
			{
				if (*szLimit == CH_DELIMITER)
				{
					for (szNext = szLimit + 1; ; szNext++)
					{
						if (!*szNext)
						{
							szLimit ++;
							break;
						}
						if (*szNext == CH_DELIMITER)
							szLimit ++;
						else if (__x_isgraph(*szNext))
							break;
					}
					if (szLimit <= szToken)
						return 0;
					break;
				}
			}


			while (!__x_isgraph(*szToken))
			{
				szToken ++;
				if (szToken >= szLimit)
					return 0;
			}
			const char* pe = szLimit - 1;
			while (!__x_isgraph(*pe))
				pe --;
			size_t toklen = pe - szToken + 1;



			PATTERNMAP::DESC2HKPMAP::const_iterator it = s_mapPattern.theMap.find(std::string(szToken, toklen));
			if (it == s_mapPattern.theMap.end())
				return 0;
			const PATTERNMAP::HOTKEYPART& hkp = (*it).second;

			if (hkcode & hkp.first)
				return 0;
			hkcode |= hkp.second;

			if (szNext == NULL)
				break;
		}

		if (!(hkcode & PATTERNMAP::MASK_VKEY))
			return 0;

		return hkcode;
	}

} //namespace hotkey_str


int LuaAddCommand(Lua_State * L)
{
	if (Lua_GetTopIndex(L) != 3)
		return 0;

	char * strUKey = (char *)Lua_ValueToString(L, 1);
	char * strName = (char *)Lua_ValueToString(L, 2);
	char * strDo = (char *)Lua_ValueToString(L, 3);

	COMMAND_SETTING cs;
	cs.uKey = hotkey_str::ParseHotKey(strUKey);
	strncpy(cs.szCommand, strName, 31);
	cs.szCommand[31] = 0;
	strncpy(cs.szDo, strDo, 127);
	cs.szDo[127] = 0;
	KShortcutKeyCentre::AddCommand(&cs);

	return 0;
}

int LuaRemoveCommand(Lua_State * L)
{
	if (Lua_GetTopIndex(L) != 2)
		return 0;

	char * strUKey = (char *)Lua_ValueToString(L, 1);
	char * strName = (char *)Lua_ValueToString(L, 2);

	COMMAND_SETTING cs;
	cs.uKey = hotkey_str::ParseHotKey(strUKey);
	if (cs.uKey != 0)
		KShortcutKeyCentre::RemoveCommand(KShortcutKeyCentre::FindCommand(cs.uKey));
	else if (strName && strName[0] != 0)
	{
		strncpy(cs.szCommand, strName, 31);
		cs.szCommand[31] = 0;
		KShortcutKeyCentre::RemoveCommand(KShortcutKeyCentre::FindCommand(cs.szCommand));
	}
	else	//清除所有命令
	{
		KShortcutKeyCentre::RemoveCommandAll();
	}

	return 0;
}

int LuaMakeFriend(Lua_State * L)
{
	if (Lua_GetTopIndex(L) != 1)
		return 0;

	char * strName = (char *)Lua_ValueToString(L, 1);

	if (strName && strName[0] != 0)
	{
		KUiPlayerItem SelectPlayer;
		strncpy(SelectPlayer.Name, strName, 32);
		int nKind = -1;
		if (g_pCoreShell)
		{
			ProcessPeople(&SelectPlayer, ACTION_MAKEFRIEND);
		}
	}

	return 0;
}

int LuaCreateTeam(Lua_State * L)
{
	if (g_pCoreShell)
	{
		g_pCoreShell->TeamOperation(TEAM_OI_CREATE, 0, 0);
	}

	return 0;
}

int LuaJoinTeam(Lua_State * L)
{
	if (Lua_GetTopIndex(L) != 1)
		return 0;

	char * strName = (char *)Lua_ValueToString(L, 1);

	if (strName && strName[0] != 0)
	{
		KUiPlayerItem SelectPlayer;
		int nKind = -1;
		if (g_pCoreShell &&
			g_pCoreShell->FindSpecialNPC(strName, &SelectPlayer, nKind) && nKind == kind_player)
		{
			ProcessPeople(&SelectPlayer, ACTION_JOINTEAM);
		}
	}

	return 0;
}

int LuaInviteTeam(Lua_State * L)
{
	if (Lua_GetTopIndex(L) != 1)
		return 0;

	char * strName = (char *)Lua_ValueToString(L, 1);

	if (strName && strName[0] != 0)
	{
		KUiPlayerItem SelectPlayer;
		int nKind = -1;
		if (g_pCoreShell &&
			g_pCoreShell->FindSpecialNPC(strName, &SelectPlayer, nKind) && nKind == kind_player)
		{
			ProcessPeople(&SelectPlayer, ACTION_INVITETEAM);
		}
	}

	return 0;
}

int LuaTrade(Lua_State * L)
{
	if (Lua_GetTopIndex(L) != 1)
		return 0;

	char * strName = (char *)Lua_ValueToString(L, 1);

	if (strName && strName[0] != 0)
	{
		KUiPlayerItem SelectPlayer;
		int nKind = -1;
		if (g_pCoreShell &&
			g_pCoreShell->FindSpecialNPC(strName, &SelectPlayer, nKind) && nKind == kind_player)
		{
			ProcessPeople(&SelectPlayer, ACTION_TRADE);
		}
	}

	return 0;
}

int LuaRevenge(Lua_State * L)
{
	if (Lua_GetTopIndex(L) != 1)
		return 0;

	char * strName = (char *)Lua_ValueToString(L, 1);

	if (strName && strName[0] != 0)
	{
		KUiPlayerItem SelectPlayer;
		int nKind = -1;
		if (g_pCoreShell &&
			g_pCoreShell->FindSpecialNPC(strName, &SelectPlayer, nKind) && nKind == kind_player)
		{
			ProcessPeople(&SelectPlayer, ACTION_REVENGE);
		}
	}

	return 0;
}

int LuaFollow(Lua_State * L)
{
	if (Lua_GetTopIndex(L) != 1)
		return 0;

	char * strName = (char *)Lua_ValueToString(L, 1);

	if (strName && strName[0] != 0)
	{
		KUiPlayerItem SelectPlayer;
		int nKind = -1;
		if (g_pCoreShell &&
			g_pCoreShell->FindSpecialNPC(strName, &SelectPlayer, nKind) && nKind == kind_player)
		{
			ProcessPeople(&SelectPlayer, ACTION_FOLLOW);
		}
	}

	return 0;
}

int LuaViewItem(Lua_State * L)
{
	if (Lua_GetTopIndex(L) != 1)
		return 0;

	char * strName = (char *)Lua_ValueToString(L, 1);

	if (strName && strName[0] != 0)
	{
		KUiPlayerItem SelectPlayer;
		int nKind = -1;
		if (g_pCoreShell &&
			g_pCoreShell->FindSpecialNPC(strName, &SelectPlayer, nKind) && nKind == kind_player)
		{
			ProcessPeople(&SelectPlayer, ACTION_VIEWITEM);
		}
	}

	return 0;
}

int LuaPrintScreen(Lua_State * L)
{
	SaveScreenToFile();
	return 0;
}

int LuaClearMessage(Lua_State * L)
{
	KUiMsgCentrePad::Clear();

	return 0;
}

int LuaSwitchPrivateMsgMode(Lua_State * L)
{
	KUiMsgCentrePad::SetPrivateMsgMode(!KUiMsgCentrePad::IsPrivateMsgMode());
	return 0;
}

#include "../../Engine/src/Text.h"

int LuaSay(Lua_State * L)
{
	if (Lua_GetTopIndex(L) != 2)
		return 0;

	char * strName = (char *)Lua_ValueToString(L, 1);
	char * strMessage = (char *)Lua_ValueToString(L, 2);
	int nLen = strlen(strMessage);

	if (strName && strName[0] != 0 && !KUiPlayerBar::IsSelfName(strName))
	{
		if (KUiPlayerBar::IsCanSendMessage(strMessage, nLen, strName, -1))
		{
			char Buffer[1536];
			nLen = KUiFaceSelector::ConvertFaceText(Buffer, strMessage, nLen);
			nLen = TEncodeText(Buffer, nLen);
			KUiPlayerBar::OnSendSomeoneMessage(strName, Buffer, nLen);
		}
	}

	return 0;
}

int LuaChat(Lua_State * L)
{
	if (Lua_GetTopIndex(L) != 2)
		return 0;

	char * strChannelName = (char *)Lua_ValueToString(L, 1);
	char * strMessage = (char *)Lua_ValueToString(L, 2);
	int nLen = strlen(strMessage);

	if (strChannelName && strChannelName[0] != 0)
	{
		int nIndex = KUiMsgCentrePad::GetChannelIndex(strChannelName);
		DWORD nChannelID = KUiMsgCentrePad::GetChannelID(nIndex);
		if (nChannelID != -1)
		{
			if (KUiPlayerBar::IsCanSendMessage(strMessage, nLen, strChannelName, nChannelID))
			{
				char Buffer[1536];
				nLen = KUiFaceSelector::ConvertFaceText(Buffer, strMessage, nLen);
				nLen = TEncodeText(Buffer, nLen);
				KUiMsgCentrePad::CheckChannel(nIndex, true);
				KUiPlayerBar::OnSendChannelMessage(nChannelID, Buffer, nLen);
			}
		}
	}

	return 0;
}

int LuaRegisterFunctionAlias(Lua_State * L)
{
	int nCount = Lua_GetTopIndex(L);
	if (nCount < 2)
		return 0;

	char * strFunAlias = (char *)Lua_ValueToString(L, 1);
	char * strFun = (char *)Lua_ValueToString(L, 2);
	int nParam = 0;
	if (nCount >= 3)
	{
		nParam = (int)Lua_ValueToNumber(L, 3);
	}

	PARAMLIST List;
	for(int i = 4; i <= nCount; i++)
	{
		char* sDefault = (char *)Lua_ValueToString(L, i);
		if (sDefault == NULL || sDefault[0] == 0)
			List.push_back("\"\"");
		else
			List.push_back(sDefault);
	}

	KShortcutKeyCentre::RegisterFunctionAlias(strFunAlias, strFun, nParam, List);

	return 0;
}

#include "UiChatPhrase.h"
extern KUiChatPhrase g_UiChatPhrase;

int LuaSetOfflineTransmit(Lua_State * L)
{
	char	buffer[sizeof(tagExtendProtoHeader) + sizeof(CHAT_SET_ROUTE) + 32];
	tagExtendProtoHeader* pPak = (tagExtendProtoHeader*)buffer;
	CHAT_SET_ROUTE*	pSet = (CHAT_SET_ROUTE*)(pPak + 1);
	CHAT_GET_ROUTE* pGet = (CHAT_GET_ROUTE*)(pPak + 1);
	char	szInfo[128];

	pSet->bRouteAll = false;
	pPak->wLength = 0;
	if (Lua_GetTopIndex(L) == 0)
	{
		pSet->ProtocolType = chat_set_route;
		pSet->byNameLen = 1;
		pSet->szName[0] = 0;
		pPak->wLength = sizeof(tagExtendProtoHeader) + sizeof(CHAT_SET_ROUTE) - sizeof(pPak->ProtocolType);
		strcpy(szInfo, "取消离线时消息转发。");
		KUiMsgCentrePad::SystemMessageArrival(szInfo, strlen(szInfo));	// 加入一条消息
	}
	else
	{
		char* pszDest = (char *)Lua_ValueToString(L, 1);
		if (pszDest[0] == '?' && pszDest[1] == '\0')
		{
			pGet->ProtocolType = chat_get_route;
			pPak->wLength = sizeof(tagExtendProtoHeader) + sizeof(CHAT_GET_ROUTE) - sizeof(pPak->ProtocolType);
		}
		else
		{
			int nLen = strlen(pszDest);
			if (KUiNewPlayer::IsValidRoleName(pszDest, nLen))
			{
				pSet->ProtocolType = chat_set_route;
				pSet->byNameLen = nLen + 1;
				memcpy(pSet->szName, pszDest, pSet->byNameLen);
				char* pszCondit = (char *)Lua_ValueToString(L, 2);
				if (pszCondit)
				{
					if (strcmp(pszCondit, "1") == 0 || stricmp(pszCondit, "online") == 0)
						pSet->bRouteAll = true;
				}
				pPak->wLength = sizeof(tagExtendProtoHeader) + sizeof(CHAT_SET_ROUTE) + nLen - sizeof(pPak->ProtocolType);
				sprintf(szInfo, (pSet->bRouteAll == false) ? "设置离线时转发消息给%s！" :
					"设置把消息转发给%s！\n输入 /OfflineTrans 可以取消设置。", pszDest);
				KUiMsgCentrePad::SystemMessageArrival(szInfo, strlen(szInfo));	// 加入一条消息
			}
		}
	}

	if (pPak->wLength)
	{
		pPak->ProtocolType = c2s_extendchat;
		g_pCoreShell->SendNewDataToServer(pPak, pPak->wLength + sizeof(pPak->ProtocolType));
	}
	return 0;
}


int LuaSetAutoReply(Lua_State * L)
{
	int nNumParam = Lua_GetTopIndex(L);
	if (nNumParam <= 0)
	{
		KUiChatPhrase::SetAutoReply(-1);
		return 0;
	}

	if (nNumParam == 1)
	{
		char* pszReply = (char *)Lua_ValueToString(L, 1);
		if (pszReply[0] == '0')
		{
			if (pszReply[1] == 0)
			{
				KUiChatPhrase::SetAutoReply(0);
				return 0;
			}
		}
		int nIndex = atoi(pszReply);
		if (nIndex >= 1 && KUiChatPhrase::SetAutoReply(nIndex))
		{
			return 0;
		}
	}

	char	szAutoReply[256] = "";
	int		nLen = 0;
	for (int i = 1; i <= nNumParam; i++)
	{
		char* pszParam = (char *)Lua_ValueToString(L, i);
		if (pszParam == NULL)
			break;
		int nLen2 = strlen(pszParam);
		if (nLen2 <= 0)
			continue;
		if (i != 1)
		{
			strcat(szAutoReply, " ");
			nLen++;
		}
		if (nLen + nLen2 < sizeof(szAutoReply))
		{
			memcpy(szAutoReply + nLen, pszParam, nLen2);
			nLen += nLen2;
			szAutoReply[nLen] = 0;
		}
	}

	if (nLen > 0)
		KUiChatPhrase::SetAutoReply(szAutoReply, true);
	else
		KUiChatPhrase::SetAutoReply(-1);
	return 0;
}

int LuaSayPhrase(Lua_State * L)
{
	if (Lua_GetTopIndex(L) != 1)
		return 0;

	int nIndex = (int)Lua_ValueToNumber(L, 1);

	char szPhrase[64];
	int nLen = g_UiChatPhrase.GetPhrase(nIndex, szPhrase);
	if (nLen > 0)
	{
		DWORD nChannelID = -1;
		//附近玩家
		int nChannelDataCount = KUiMsgCentrePad::GetChannelCount();
		for (int n = 0; n < nChannelDataCount; n++)
		{
			if (KUiMsgCentrePad::IsChannelType(n, KUiMsgCentrePad::ch_Screen))
			{
				nChannelID = KUiMsgCentrePad::GetChannelID(n);
				break;
			}
		}

		if (nChannelID != -1)
		{
			if (KUiPlayerBar::IsCanSendMessage(szPhrase, nLen, KUiMsgCentrePad::GetChannelTitle(KUiMsgCentrePad::GetChannelIndex(nChannelID)), nChannelID))
			{
				char Buffer[1536];
				nLen = KUiFaceSelector::ConvertFaceText(Buffer, szPhrase, nLen);
				nLen = TEncodeText(Buffer, nLen);
				KUiMsgCentrePad::CheckChannel(n, true);
				KUiPlayerBar::OnSendChannelMessage(nChannelID, Buffer, nLen);
			}
		}
	}

	return 0;
}

int LuaSayEmote(Lua_State * L)
{
	char *pszDestName, *pszDestChannel;
	int  nEmoteIndex;

	if (Lua_GetTopIndex(L) != 3)
		return 0;

	nEmoteIndex = (int)Lua_ValueToNumber(L, 3);
	pszDestName   = (char *)Lua_ValueToString(L, 1);
	pszDestChannel  = (char *)Lua_ValueToString(L, 2);

    ProcessEmote(pszDestName, pszDestChannel, nEmoteIndex);

	return 0;
}

int LuaWorldFilter(Lua_State * L)
{
	KUiMsgCentrePad *pMsgCenter = KUiMsgCentrePad::GetSelf();
	int nValueCount;

	if(pMsgCenter)
	{
		nValueCount = (int)Lua_GetTopIndex(L);
		char szCmd[256], *pKeyWord;

		szCmd[0] = 0;
		for(int i = 0;i < nValueCount;i++)
		{
			pKeyWord = (char *)Lua_ValueToString(L, i + 1);
			strcat(szCmd, pKeyWord);
			strcat(szCmd, " ");
		}
		pMsgCenter->SetFilter(KUiMsgCentrePad::enumFL_INCOMING, 1, szCmd, NULL);
	}
	return 1;
}

int LuaConstructFilter(Lua_State * L)
{
	char* pKeyWord;
	int   nValueCount;

	KUiMsgCentrePad *pMsgCenter = KUiMsgCentrePad::GetSelf();
	if(pMsgCenter)
	{
		nValueCount = (int)Lua_GetTopIndex(L);

		pMsgCenter->CleanFilterModule();
		for(int i = 0;i < nValueCount;i++)
		{
			pKeyWord = (char *)Lua_ValueToString(L, i + 1);
			pMsgCenter->AddFilterExpression(pKeyWord);
		}
	}
	return 0;
}

int LuaGetCurrentChannelName(Lua_State * L)
{
	int nCurChannel = KUiPlayerBar::GetCurChannel();
	if(nCurChannel >= 0 && nCurChannel < (int)KUiMsgCentrePad::GetChannelCount())
	{
		Lua_PushString(L, KUiMsgCentrePad::GetChannelTitle(nCurChannel));
	}
	else
	{
		Lua_PushString(L, "");
	}
	return 1;
}

int LuaCheckChannel(Lua_State * L)
{
	int nIndex = Lua_GetTopIndex(L);
	if (nIndex != 2)
	{
		return 0;
	}
	char* pszChannel = (char *)Lua_ValueToString(L, 1);
	int bCheck = (int)Lua_ValueToNumber(L, 2);
	KUiMsgCentrePad::CheckChannel(pszChannel, bCheck);

	return 0;
}

int LuaTransCmdToCore(Lua_State * L)
{
	int nIndex = Lua_GetTopIndex(L);
	if (nIndex != 1)
		return 0;
#ifdef _DEBUG
	char* pszCmd = (char *)Lua_ValueToString(L, 1);
	if (pszCmd)
		g_pCoreShell->Debug(DEBUG_GM_CMD, 0, (int)pszCmd);
#endif
	return 0;
}

int LuaGetRecentPlayerName(Lua_State * L)
{
	int nCurChannel = KUiPlayerBar::GetCurChannel();
	if(nCurChannel >= (int)KUiMsgCentrePad::GetChannelCount())
	{
		Lua_PushString(L, KUiPlayerBar::GetRecentPlayerName(nCurChannel));
	}
	else
	{
		Lua_PushString(L, "");
	}
	return 1;
}

int LuaConvertEmotes(Lua_State * L)
{
	g_UiChatPhrase.ConstructFunctionAlias();
	return 0;
}

int LuaSetEmote(Lua_State *L)
{
    int nIndex = -1, nParamCount = Lua_GetTopIndex(L);
	char *pszCmd, *pszName, *pszStringTarget, *pszStringSelf;

	switch(nParamCount)
	{
	case 5:
		nIndex = (int)Lua_ValueToNumber(L, 5);

	case 4:
		pszCmd = (char *)Lua_ValueToString(L, 1);
		pszName = (char *)Lua_ValueToString(L, 2);
		pszStringSelf = (char *)Lua_ValueToString(L, 4);
		pszStringTarget = (char *)Lua_ValueToString(L, 3);
		break;

	default:
		return 0;
	}

	g_UiChatPhrase.SetEmote(pszCmd, pszName, pszStringTarget, pszStringSelf, nIndex);
	return 0;
}

int LuaSetScreenShotFolder(Lua_State * L)
{
	if(Lua_GetTopIndex(L) == 1)
	{
		char *pszFolder = (char *)Lua_ValueToString(L, 1);
		char szPath[256], Buff[128];

		if(pszFolder[0] == '$' && pszFolder[1] == '\\')
		{
			g_GetRootPath(Buff);
			sprintf(szPath, "%s\\%s", Buff, pszFolder + 2);
		}
		else
		{
			memcpy(szPath, pszFolder, strlen(pszFolder) + 1);
		}
		SetScrPicPath(szPath);
	}
	return 0;
}

int LuaSetPhrase(Lua_State * L)
{
	if (Lua_GetTopIndex(L) != 2)
		return 0;

	int nIndex = (int)Lua_ValueToNumber(L, 1);
	char * strPhrase = (char *)Lua_ValueToString(L, 2);
	int nLen = strlen(strPhrase);

	if (nIndex >= 0 && nIndex < g_UiChatPhrase.GetPhraseCount())
	{
		g_UiChatPhrase.SetPhrase(nIndex, strPhrase, nLen);
	}

	return 0;
}

#include "../Login/Login.h"
#include "UiCase/UiInit.h"

int LuaExitGame(Lua_State * L)
{
	if (KUiESCDlg::GetIfVisible())
		KUiESCDlg::CloseWindow(false);

	if (g_pCoreShell)
		g_pCoreShell->OperationRequest(GOI_EXIT_GAME, 0, 0);

	g_LoginLogic.ReturnToIdle();
	UiEndGame();
	KUiInit::OpenWindow(true, false);

	return 0;
}

int LuaExit(Lua_State * L)
{
	UiPostQuitMsg();

	return 0;
}

#define GAME_CONFIG_STRING_INI "\\Ui\\杂烩.ini"
int LuaSet3D(Lua_State * L)
{
	if (Lua_GetTopIndex(L) != 1)
		return 0;

	KIniFile Ini, IniString;
	KSystemMessage Msg;

	int nBool = (int)Lua_ValueToNumber(L, 1);

	Msg.eType = SMT_NORMAL;
	Msg.byConfirmType = SMCT_NONE;
	Msg.byPriority = 0;
	Msg.byParamSize = 0;

	Ini.Load(GAME_CONFIG_FILE);
	IniString.Load(GAME_CONFIG_STRING_INI);
	if (nBool)
	{
		IniString.GetString("Config", "3D", "游戏设为3D模式，重新启动游戏后生效。", Msg.szMessage, sizeof(Msg.szMessage));
		Ini.WriteInteger("Client", "Represent", SCREEN_MODE_3D);
	}
	else
	{
		IniString.GetString("Config", "2D", "游戏设为2D模式，重新启动游戏后生效。", Msg.szMessage, sizeof(Msg.szMessage));
		Ini.WriteInteger("Client", "Represent", SCREEN_MODE_2D);
	}
	KUiSysMsgCentre::AMessageArrival(&Msg, NULL);
	Ini.Save(GAME_CONFIG_FILE);

	return 0;
}

int LuaSetFullWindow(Lua_State * L)
{
	if (Lua_GetTopIndex(L) != 1)
		return 0;

	KIniFile Ini, IniString;
	KSystemMessage Msg;

	int nBool = (int)Lua_ValueToNumber(L, 1);

	Msg.eType = SMT_NORMAL;
	Msg.byConfirmType = SMCT_NONE;
	Msg.byPriority = 0;
	Msg.byParamSize = 0;

	Ini.Load(GAME_CONFIG_FILE);
	IniString.Load(GAME_CONFIG_STRING_INI);
	if (nBool)
	{
		IniString.GetString("Config", "FullWindow", "游戏设定改变为全屏运行，将在下次启动游戏的时候生效。", Msg.szMessage, sizeof(Msg.szMessage));
		Ini.WriteInteger("Client", "FullScreen", 1);
	}
	else
	{
		IniString.GetString("Config", "Window", "游戏设定改变为窗口运行，将在下次启动游戏的时候生效。", Msg.szMessage, sizeof(Msg.szMessage));
		Ini.WriteInteger("Client", "FullScreen", 0);
	}
	KUiSysMsgCentre::AMessageArrival(&Msg, NULL);
	Ini.Save(GAME_CONFIG_FILE);

	return 0;
}

int LuaMoveTo(Lua_State * L)
{
	if (Lua_GetTopIndex(L) != 2)
		return 0;

	if (!Wnd_IsReceiveInput())
	{
		return 0;
	}

	int nDir = (int)Lua_ValueToNumber(L, 1);
	int nM = (int)Lua_ValueToNumber(L, 2);

	if (nDir < 0)
		nDir = 0;

	if (nDir > 63)
		nDir = 63;

	if (nM < 0)
		nM = 0;

	if (nM > 2)
		nM = 2;

	if (g_pCoreShell)
		g_pCoreShell->Goto(nDir, nM);

	return 0;
}

int LuaTurnTo(Lua_State * L)
{
	if (Lua_GetTopIndex(L) != 1)
		return 0;

	int nDir = (int)Lua_ValueToNumber(L, 1);

	if (nDir < 0)
		nDir = 0;

	if (nDir > 2)
		nDir = 2;

	if (g_pCoreShell)
		g_pCoreShell->Turn(nDir);

	return 0;
}

int Mouse_Action(Lua_State * L)
{
	if (!KShortcutKeyCentre::ms_bMouse)
		return 0;

	if (g_pCoreShell == NULL)
		return 0;

	if (g_pCoreShell->ThrowAwayItem())	//先扔掉手里的物品
		return 0;

	KUiPlayerItem SelectPlayer;
	int nNPCKind = -1;
	if (g_pCoreShell->FindSelectNPC(KShortcutKeyCentre::ms_MouseX, KShortcutKeyCentre::ms_MouseY, relation_all, false, &SelectPlayer, nNPCKind))
	{
		int nRelation = g_pCoreShell->GetNPCRelation(SelectPlayer.nIndex);
		if (nRelation == relation_enemy)
		{
			KUiPlayerImmedItemSkill immedItemSkillInfo;
			memset(&immedItemSkillInfo, 0, sizeof(KUiPlayerImmedItemSkill));
			g_pCoreShell->GetGameData(GDI_PLAYER_IMMED_ITEMSKILL, (int)&immedItemSkillInfo, 0);

			if (!g_pCoreShell->LockSomeoneUseSkill(SelectPlayer.nIndex, immedItemSkillInfo.IMmediaSkill[0].uId))	//锁定失败
				g_pCoreShell->LockSomeoneAction(0);	//解锁
		}
		else if (nRelation == relation_dialog)
		{
			g_pCoreShell->LockSomeoneAction(SelectPlayer.nIndex);
		}
	}
	else
		g_pCoreShell->LockSomeoneAction(0);

	int nObjKind = -1;
	int nObjectIdx = 0;
	if (g_pCoreShell->FindSelectObject(KShortcutKeyCentre::ms_MouseX, KShortcutKeyCentre::ms_MouseY, false, nObjectIdx, nObjKind))
	{
		g_pCoreShell->LockObjectAction(nObjectIdx);
	}
	else
		g_pCoreShell->LockObjectAction(0);

	g_pCoreShell->GotoWhere(KShortcutKeyCentre::ms_MouseX, KShortcutKeyCentre::ms_MouseY, 0);

	return 0;
}

int Mouse_Force0(Lua_State * L)
{
	if (!KShortcutKeyCentre::ms_bMouse)
		return 0;

	if (g_pCoreShell == NULL)
		return 0;

	if (g_pCoreShell->ThrowAwayItem())	//先扔掉手里的物品
		return 0;

	KUiPlayerImmedItemSkill immedItemSkillInfo;
	memset(&immedItemSkillInfo, 0, sizeof(KUiPlayerImmedItemSkill));
	g_pCoreShell->GetGameData(GDI_PLAYER_IMMED_ITEMSKILL, (int)&immedItemSkillInfo, 0);

	g_pCoreShell->UseSkill(KShortcutKeyCentre::ms_MouseX, KShortcutKeyCentre::ms_MouseY, immedItemSkillInfo.IMmediaSkill[0].uId);

	return 0;
}

int Mouse_Force1(Lua_State * L)
{
	if (!KShortcutKeyCentre::ms_bMouse)
		return 0;

	if (g_pCoreShell == NULL)
		return 0;

	if (g_pCoreShell->ThrowAwayItem())	//先扔掉手里的物品
		return 0;

	KUiPlayerImmedItemSkill immedItemSkillInfo;
	memset(&immedItemSkillInfo, 0, sizeof(KUiPlayerImmedItemSkill));
	g_pCoreShell->GetGameData(GDI_PLAYER_IMMED_ITEMSKILL, (int)&immedItemSkillInfo, 0);

	g_pCoreShell->UseSkill(KShortcutKeyCentre::ms_MouseX, KShortcutKeyCentre::ms_MouseY, immedItemSkillInfo.IMmediaSkill[1].uId);

	return 0;
}

int Mouse_MoveTo(Lua_State * L)
{
	if (!KShortcutKeyCentre::ms_bMouse)
		return 0;

	if (g_pCoreShell == NULL)
		return 0;

	if (Lua_GetTopIndex(L) != 1)
		return 0;

	int nM = (int)Lua_ValueToNumber(L, 1);

	if (nM < 0)
		nM = 0;

	if (nM > 2)
		nM = 2;

	g_pCoreShell->GotoWhere(KShortcutKeyCentre::ms_MouseX, KShortcutKeyCentre::ms_MouseY, nM);

	return 0;
}

int Mouse_Menu(Lua_State * L)
{
	if (!KShortcutKeyCentre::ms_bMouse)
		return 0;

	if (g_pCoreShell == NULL)
		return 0;

	KUiPlayerItem SelectPlayer;
	int nKind = -1;
	if (!(g_pCoreShell->FindSelectNPC(KShortcutKeyCentre::ms_MouseX, KShortcutKeyCentre::ms_MouseY, relation_none | relation_ally | relation_enemy, false, &SelectPlayer, nKind)
		 && kind_player == nKind))
		return 0;

	PopUpContextPeopleMenu(SelectPlayer, KShortcutKeyCentre::ms_MouseX, KShortcutKeyCentre::ms_MouseY);

	return 0;
}

int Mouse_Emote_Menu(Lua_State * L)
{
	if (!KShortcutKeyCentre::ms_bMouse)
		return 0;

	if (g_pCoreShell == NULL)
		return 0;

	KUiPlayerItem SelectPlayer;
	int nKind = -1;
	if (!(g_pCoreShell->FindSelectNPC(KShortcutKeyCentre::ms_MouseX, KShortcutKeyCentre::ms_MouseY, relation_none | relation_ally | relation_enemy, false, &SelectPlayer, nKind)
		 && kind_player == nKind))
		return 0;

	PopUpContextEmoteMenu(SelectPlayer.Name, KShortcutKeyCentre::ms_MouseX, KShortcutKeyCentre::ms_MouseY);

	return 0;
}

int Mouse_Say(Lua_State * L)
{
	if (!KShortcutKeyCentre::ms_bMouse)
		return 0;

	if (g_pCoreShell == NULL)
		return 0;

	KUiPlayerItem SelectPlayer;
	int nKind = -1;
	if (!(g_pCoreShell->FindSelectNPC(KShortcutKeyCentre::ms_MouseX, KShortcutKeyCentre::ms_MouseY, relation_none | relation_ally | relation_enemy, false, &SelectPlayer, nKind)
		 && kind_player == nKind))
		return 0;

	int nStallStat = g_pCoreShell->GetGameData(GDI_IS_STALL, -1, -1);
	if (!nStallStat && g_pCoreShell->GetGameData(GDI_IS_STALL, SelectPlayer.nIndex, -1) == 1)
		g_pCoreShell->LockSomeoneAction(SelectPlayer.nIndex);
	else
		ProcessPeople(&SelectPlayer, ACTION_CHAT);

	return 0;
}

int Mouse_MakeFriend(Lua_State * L)
{
	if (!KShortcutKeyCentre::ms_bMouse)
		return 0;

	if (g_pCoreShell == NULL)
		return 0;

	KUiPlayerItem SelectPlayer;
	int nKind = -1;
	if (!(g_pCoreShell->FindSelectNPC(KShortcutKeyCentre::ms_MouseX, KShortcutKeyCentre::ms_MouseY, relation_none | relation_ally | relation_enemy, false, &SelectPlayer, nKind)
		 && kind_player == nKind))
		return 0;

	ProcessPeople(&SelectPlayer, ACTION_MAKEFRIEND);

	return 0;
}

int Mouse_JoinTeam(Lua_State * L)
{
	if (!KShortcutKeyCentre::ms_bMouse)
		return 0;

	if (g_pCoreShell == NULL)
		return 0;

	KUiPlayerItem SelectPlayer;
	int nKind = -1;
	if (!(g_pCoreShell->FindSelectNPC(KShortcutKeyCentre::ms_MouseX, KShortcutKeyCentre::ms_MouseY, relation_none | relation_ally | relation_enemy, false, &SelectPlayer, nKind)
		 && kind_player == nKind))
		return 0;

	ProcessPeople(&SelectPlayer, ACTION_JOINTEAM);

	return 0;
}

int Mouse_InviteTeam(Lua_State * L)
{
	if (!KShortcutKeyCentre::ms_bMouse)
		return 0;

	if (g_pCoreShell == NULL)
		return 0;

	KUiPlayerItem SelectPlayer;
	int nKind = -1;
	if (!(g_pCoreShell->FindSelectNPC(KShortcutKeyCentre::ms_MouseX, KShortcutKeyCentre::ms_MouseY, relation_none | relation_ally | relation_enemy, false, &SelectPlayer, nKind)
		 && kind_player == nKind))
		return 0;

	ProcessPeople(&SelectPlayer, ACTION_INVITETEAM);

	return 0;
}

int Mouse_Trade(Lua_State * L)
{
	if (!KShortcutKeyCentre::ms_bMouse)
		return 0;

	if (g_pCoreShell == NULL)
		return 0;

	KUiPlayerItem SelectPlayer;
	int nKind = -1;
	if (!(g_pCoreShell->FindSelectNPC(KShortcutKeyCentre::ms_MouseX, KShortcutKeyCentre::ms_MouseY, relation_none | relation_ally | relation_enemy, false, &SelectPlayer, nKind)
		 && kind_player == nKind))
		return 0;

	ProcessPeople(&SelectPlayer, ACTION_TRADE);

	return 0;
}

int Mouse_Revenge(Lua_State * L)
{
	if (!KShortcutKeyCentre::ms_bMouse)
		return 0;

	if (g_pCoreShell == NULL)
		return 0;

	KUiPlayerItem SelectPlayer;
	int nKind = -1;
	if (!(g_pCoreShell->FindSelectNPC(KShortcutKeyCentre::ms_MouseX, KShortcutKeyCentre::ms_MouseY, relation_none | relation_ally | relation_enemy, false, &SelectPlayer, nKind)
		 && kind_player == nKind))
		return 0;

	ProcessPeople(&SelectPlayer, ACTION_REVENGE);

	return 0;
}

int Mouse_Follow(Lua_State * L)
{
	if (!KShortcutKeyCentre::ms_bMouse)
		return 0;

	if (g_pCoreShell == NULL)
		return 0;

	KUiPlayerItem SelectPlayer;
	int nKind = -1;
	if (!(g_pCoreShell->FindSelectNPC(KShortcutKeyCentre::ms_MouseX, KShortcutKeyCentre::ms_MouseY, relation_none | relation_ally | relation_enemy, false, &SelectPlayer, nKind)
		 && kind_player == nKind))
		return 0;

	ProcessPeople(&SelectPlayer, ACTION_FOLLOW);

	return 0;
}

int Mouse_ViewItem(Lua_State * L)
{
	if (!KShortcutKeyCentre::ms_bMouse)
		return 0;

	if (g_pCoreShell == NULL)
		return 0;

	KUiPlayerItem SelectPlayer;
	int nKind = -1;
	if (!(g_pCoreShell->FindSelectNPC(KShortcutKeyCentre::ms_MouseX, KShortcutKeyCentre::ms_MouseY, relation_none | relation_ally | relation_enemy, false, &SelectPlayer, nKind)
		 && kind_player == nKind))
		return 0;

	ProcessPeople(&SelectPlayer, ACTION_VIEWITEM);

	return 0;
}

int Mouse_BlackList(Lua_State * L)
{
	if (!KShortcutKeyCentre::ms_bMouse)
		return 0;

	if (g_pCoreShell == NULL)
		return 0;

	KUiPlayerItem SelectPlayer;
	int nKind = -1;
	if (!(g_pCoreShell->FindSelectNPC(KShortcutKeyCentre::ms_MouseX, KShortcutKeyCentre::ms_MouseY, relation_none | relation_ally | relation_enemy, false, &SelectPlayer, nKind)
		 && kind_player == nKind))
		return 0;

	ProcessPeople(&SelectPlayer, ACTION_BLACKLIST);

	return 0;
}

int LuaThrowAwayItem(Lua_State * L)
{
	if (g_pCoreShell == NULL)
		return 0;

	g_pCoreShell->ThrowAwayItem();

	return 0;
}

int LuaSetChatFontSzie(Lua_State * L)
{
	if (Lua_GetTopIndex(L) != 1)
		return 0;

	int nF = (int)Lua_ValueToNumber(L, 1);

	KUiMsgCentrePad::SetFontSize(nF);

	return 0;
}

int LuaSetChannelTextColor(Lua_State * L)
{
	if (Lua_GetTopIndex(L) != 3)
		return 0;

	char * strName = (char *)Lua_ValueToString(L, 1);
	char * strColor = (char *)Lua_ValueToString(L, 2);
	char * strBColor = (char *)Lua_ValueToString(L, 3);

	if (strName && strColor)
	{
		KUiMsgCentrePad::SetChannelTextColor(strName, GetColor(strColor), GetColor(strBColor));
	}

	return 0;
}

int LuaSetMeTextColor(Lua_State * L)
{
	if (Lua_GetTopIndex(L) != 2)
		return 0;

	char * strColor = (char *)Lua_ValueToString(L, 1);
	char * strBColor = (char *)Lua_ValueToString(L, 2);

	if (strColor)
	{
		KUiMsgCentrePad::SetMSNTextColor(0, GetColor(strColor), GetColor(strBColor));
	}

	return 0;
}

int LuaSetFriendTextColor(Lua_State * L)
{
	if (Lua_GetTopIndex(L) != 2)
		return 0;

	char * strColor = (char *)Lua_ValueToString(L, 1);
	char * strBColor = (char *)Lua_ValueToString(L, 2);

	if (strColor)
	{
		KUiMsgCentrePad::SetMSNTextColor(1, GetColor(strColor), GetColor(strBColor));
	}

	return 0;
}

int LuaSetStrangerTextColor(Lua_State * L)
{
	if (Lua_GetTopIndex(L) != 2)
		return 0;

	char * strColor = (char *)Lua_ValueToString(L, 1);
	char * strBColor = (char *)Lua_ValueToString(L, 2);

	if (strColor)
	{
		KUiMsgCentrePad::SetMSNTextColor(2, GetColor(strColor), GetColor(strBColor));
	}

	return 0;
}


extern int WND_SHOW_MOUSE_OVER_WND;

int LuaDebug(Lua_State * L)
{
	if (Lua_GetTopIndex(L) != 1)
		return 0;

	char * strMode = (char *)Lua_ValueToString(L, 1);

	if (strcmpi(strMode, "ShowMouseWnd") == 0)
	{
		WND_SHOW_MOUSE_OVER_WND = !WND_SHOW_MOUSE_OVER_WND;
	}

	return 0;
}

void AddBlackList(const char* strName, const char* strGroup, bool bNotify);

int LuaAddBlackList(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 1 || Lua_GetTopIndex(L) > 3)
		return 0;

	bool   bNotify = true;
	char * strName = (char *)Lua_ValueToString(L, 1);
	char * strGroup = (char *)Lua_ValueToString(L, 2);
	if (Lua_GetTopIndex(L) == 3)
		bNotify = (Lua_ValueToNumber(L, 3) != 0);

	std::string group;
	if (strGroup)
		group = strGroup;

	AddBlackList(strName, group.c_str(), bNotify);

	return 0;
}

void RemoveBlackList(char* strName);
void ClearBlackList();

int LuaRemoveBlackList(Lua_State * L)
{
	if (Lua_GetTopIndex(L) != 1)
		return 0;

	char * strName = (char *)Lua_ValueToString(L, 1);

	if (strName == NULL || strName[0] == 0)
		ClearBlackList();
	else
		RemoveBlackList(strName);

	return 0;
}

void CreateAddinUnit(const char* UnitName);

int LuaCreateAddinUnit(Lua_State * L)
{
	if (Lua_GetTopIndex(L) != 1)
		return 0;

	char * strName = (char *)Lua_ValueToString(L, 1);

	CreateAddinUnit(strName);

	return 0;
}

void DeleteAddinUnit(const char* UnitName);
int LuaDeleteAddinUnit(Lua_State * L)
{
	if (Lua_GetTopIndex(L) != 1)
		return 0;

	char * strName = (char *)Lua_ValueToString(L, 1);

	DeleteAddinUnit(strName);

	return 0;
}

int LuaCreateUnitGroup(Lua_State * L)
{
	if (Lua_GetTopIndex(L) != 2)
		return 0;

	char * strName = (char *)Lua_ValueToString(L, 1);
	char * strGroup = (char *)Lua_ValueToString(L, 2);

	int nUint = KUiChatCentre::FindUnitIndex(strName);
	if (nUint >= 0)
	{
		KUiChatCentre::NewGroup(nUint, strGroup);
	}

	return 0;
}

int LuaRenameUnitGroup(Lua_State * L)
{
	if (Lua_GetTopIndex(L) != 3)
		return 0;

	char * strName = (char *)Lua_ValueToString(L, 1);
	char * strGroup = (char *)Lua_ValueToString(L, 2);
	char * strNewGroup = (char *)Lua_ValueToString(L, 3);

	int nUint = KUiChatCentre::FindUnitIndex(strName);
	if (nUint >= 0)
	{
		KUiChatCentre::ReplaceGroupName(nUint, strGroup, strNewGroup);
	}

	return 0;
}

int LuaMoveUnitGroup(Lua_State * L)
{
	if (Lua_GetTopIndex(L) != 3)
		return 0;

	char * strName = (char *)Lua_ValueToString(L, 1);
	char * strGroup = (char *)Lua_ValueToString(L, 2);
	char * strNewGroup = (char *)Lua_ValueToString(L, 3);

	int nUint = KUiChatCentre::FindUnitIndex(strName);
	if (nUint >= 0)
	{
		KUiChatCentre::MoveGroupToGroup(nUint, strGroup, strNewGroup, false);
	}

	return 0;
}

int LuaDeleteUnitGroup(Lua_State * L)
{
	if (Lua_GetTopIndex(L) != 2)
		return 0;

	char * strName = (char *)Lua_ValueToString(L, 1);
	char * strGroup = (char *)Lua_ValueToString(L, 2);

	int nUint = KUiChatCentre::FindUnitIndex(strName);
	if (nUint >= 0)
	{
		KUiChatCentre::MoveGroupToGroup(nUint, strGroup, "", true);
	}

	return 0;
}

int LuaMoveUnitMember(Lua_State * L)
{
	if (Lua_GetTopIndex(L) != 3)
		return 0;

	char * strName = (char *)Lua_ValueToString(L, 1);
	char * strMember = (char *)Lua_ValueToString(L, 2);
	char * strGroup = (char *)Lua_ValueToString(L, 3);

	int nUint = KUiChatCentre::FindUnitIndex(strName);
	if (nUint >= 0)
	{
		KUiChatCentre::MoveFriendToGroup(nUint, strMember, strGroup);
	}

	return 0;
}

int LuaCreateAddinUnitMember(Lua_State * L)
{
	if (Lua_GetTopIndex(L) != 3)
		return 0;

	char * strName = (char *)Lua_ValueToString(L, 1);
	char * strMember = (char *)Lua_ValueToString(L, 2);
	char * strGroup = (char *)Lua_ValueToString(L, 3);

	int nUint = KUiChatCentre::FindAddinUnitIndex(strName);
	if (nUint >= 0)
	{
		KUiChatCentre::AddFriendInfo(nUint, strMember, strGroup);
	}

	return 0;
}

int LuaDeleteAddinUnitMember(Lua_State * L)
{
	if (Lua_GetTopIndex(L) != 2)
		return 0;

	char * strName = (char *)Lua_ValueToString(L, 1);
	char * strMember = (char *)Lua_ValueToString(L, 2);

	int nUint = KUiChatCentre::FindAddinUnitIndex(strName);
	if (nUint >= 0)
	{
		KUiChatCentre::DeleteFriend(nUint, strMember, true);
	}

	return 0;
}

int LuaSetAddinUnitMemberStatus(Lua_State * L)
{
	if (Lua_GetTopIndex(L) != 3)
		return 0;

	char * strName = (char *)Lua_ValueToString(L, 1);
	char * strMember = (char *)Lua_ValueToString(L, 2);
	char * strStatus = (char *)Lua_ValueToString(L, 3);

	int nUint = KUiChatCentre::FindAddinUnitIndex(strName);
	if (nUint >= 0)
	{
		int nStatus = stateOnline;
		if (strStatus == NULL || strStatus[0] == 0)
			nStatus = stateOffline;
		KUiChatCentre::FriendStatus(nUint, strMember, nStatus);
	}

	return 0;
}

int LuaShowSomeoneMessage(Lua_State * L)
{
	if (Lua_GetTopIndex(L) != 2)
		return 0;

	char * strName = (char *)Lua_ValueToString(L, 1);
	char * strMessage = (char *)Lua_ValueToString(L, 2);

	if (strName == NULL || strName[0] == 0)
		return 0;

	int nLen = strlen(strMessage);
	char Buffer[1536];
	nLen = KUiFaceSelector::ConvertFaceText(Buffer, strMessage, nLen);
	nLen = TEncodeText(Buffer, nLen);
	KUiMsgCentrePad::ShowSomeoneMessage(strName, Buffer, nLen);

	return 0;
}

TLua_Funcs GameScriptFuns[] =
{
	{"Mouse_Action", Mouse_Action},
	{"Mouse_Force0", Mouse_Force0},
	{"Mouse_Force1", Mouse_Force1},
	{"Mouse_MoveTo", Mouse_MoveTo},	//mode
	{"Mouse_Menu", Mouse_Menu},
	{"Mouse_Emote_Menu", Mouse_Emote_Menu},
	{"Mouse_Say", Mouse_Say},
	{"Mouse_MakeFriend", Mouse_MakeFriend},
	{"Mouse_JoinTeam", Mouse_JoinTeam},
	{"Mouse_InviteTeam", Mouse_InviteTeam},
	{"Mouse_Trade", Mouse_Trade},
	{"Mouse_Revenge", Mouse_Revenge},
	{"Mouse_Follow", Mouse_Follow},
	{"Mouse_ViewItem", Mouse_ViewItem},
	{"Mouse_BlackList", Mouse_BlackList},
	{"AddCommand", LuaAddCommand},			//char* szKey, char* szName, char* szScript
	{"RemoveCommand", LuaRemoveCommand},	//char* szKey, char* szName
	{"Open", LuaOpenWindow},		//char* szWindow
	{"Focus", LuaFocusWindow},		//char* szWindow
	{"Switch", LuaSwitchStatus},	//char* szStatus
	{"ShortcutSkill", LuaShortcutSkill},	//int nIdex [0, SKILLTREE_SHORTCUT_SKILL_COUNT)
	{"DirectShortcutSkill", LuaDirectShortcutSkill},	//int nIdex [0, SKILLTREE_SHORTCUT_SKILL_COUNT)
	{"ShortcutUseItem", LuaShortcutUseItem},	//int nIdex [0, UPB_IMMEDIA_ITEM_COUNT)
	{"MakeFriend", LuaMakeFriend},		//char* szPlayerName
	{"CreateTeam", LuaCreateTeam},
	{"JoinTeam", LuaJoinTeam},		//char* szTeamLeaderName
	{"InviteTeam", LuaInviteTeam},		//char* szPlayerName
	{"Trade", LuaTrade},		//char* szPlayerName
	{"Revenge", LuaRevenge},		//char* szPlayerName
	{"Follow", LuaFollow},		//char* szPlayerName
	{"ViewItem", LuaViewItem},		//char* szPlayerName
	{"PrintScreen", LuaPrintScreen},
	{"ClearMessage", LuaClearMessage},
	{"SwitchPrivateMsgMode", LuaSwitchPrivateMsgMode},
	{"Say", LuaSay},		//	char * strPlayerName,	char * strMessage
	{"Chat", LuaChat},		//	char * strChannelName,	char * strMessage
	{"RegisterFunctionAlias", LuaRegisterFunctionAlias},	//char * strFunAlias, char * strFun, [int nParam], [Paramlist...]
	{"SayPhrase", LuaSayPhrase},	//int nIndex
	{"SetPhrase", LuaSetPhrase},	//int nIndex, char * strPhrase
	{"SayEmote", LuaSayEmote},      //char* pszDestName, char* pszDestChannel, int nEmoteIndex
	{"ExitGame", LuaExitGame},
	{"Exit", LuaExit},
	{"Set3D", LuaSet3D},					//int nBool
	{"SetFullWindow", LuaSetFullWindow},	//int nBool
	{"MoveWindow", LuaMoveWindow},		//char* szWindow, x, y
	{"SizeWindow", LuaSizeWindow},		//char* szWindow, w, h
	{"ShowWindow", LuaShowWindow},		//char* szWindow, bool
	{"MoveTo", LuaMoveTo},		//dir, mode
	{"TurnTo", LuaTurnTo},		//dir
	{"ThrowAwayItem", LuaThrowAwayItem},
	{"SetChatFontSize", LuaSetChatFontSzie},	//nFontSize
	{"SetChannelTextColor", LuaSetChannelTextColor},	//char* strname, char* strColor, char* strBorderColor
	{"SetMeTextColor", LuaSetMeTextColor},	//char* strColor, char* strBorderColor
	{"SetFriendTextColor", LuaSetFriendTextColor},	//char* strColor, char* strBorderColor
	{"SetStrangerTextColor", LuaSetStrangerTextColor},	//char* strColor, char* strBorderColor
	{"AddBlackList", LuaAddBlackList},	//char* strName, [char* strGroup]
	{"RemoveBlackList", LuaRemoveBlackList},	//char* strName
	{"Debug", LuaDebug},	//char* strMode
	{"SwitchChannel", LuaSwitchChannel},
	{"RegisterEvent", LuaRegisterEvent},	//char* strName, HWND hWnd
	{"UnregisterEvent", LuaUnregisterEvent},	//char* strName, HWND hWnd
	{"SendEvent", LuaSendEvent},	//char* strName, char* szEvent
	{"GetAppStatus", LuaGetAppStatus},
	{"ShowSomeoneMessage", LuaShowSomeoneMessage},	//char* strName, char* szMessage
	{"GetCurrentChannelName", LuaGetCurrentChannelName},//return the Current Channel Name if isn't private channel, or null in private channel
	{"GetRecentPlayerName", LuaGetRecentPlayerName},    //return the telling player name if is private channel, or null in public channel
	{"CreateAddinUnit", LuaCreateAddinUnit},	//char* strName
	{"DeleteAddinUnit", LuaDeleteAddinUnit},	//char* strName
	{"CreateUnitGroup", LuaCreateUnitGroup},	//char* strName, char* strGroup
	{"RenameUnitGroup", LuaRenameUnitGroup},	//char* strName, char* strGroup, char* strNewGroup
	{"MoveUnitGroup", LuaMoveUnitGroup},		//char* strName, char* strGroup, char* strNewGroup
	{"DeleteUnitGroup", LuaDeleteUnitGroup},	//char* strName, char* strGroup
	{"MoveUnitMember", LuaMoveUnitMember},		//char* strName, char* strMember, char* strGroup
	{"CreateAddinUnitMember", LuaCreateAddinUnitMember}, //char* strName, char* strMember, char* strGroup
	{"DeleteAddinUnitMember", LuaDeleteAddinUnitMember}, //char* strName, char* strMember
	{"SetAddinUnitMemberStatus", LuaSetAddinUnitMemberStatus}, //char* strName, char* strMember, char* strStatus
	{"ConvertEmotes", LuaConvertEmotes},        //Convert the WHOLE Emote File into Lua file
	{"SetEmote", LuaSetEmote},                  //Set a emote expression, give 5 param,1 = command,2 = emote name,3 = emote expression with target,4 = emote expression without target,5 = emote index,if 5 set give,willl change the command,name,strings of emote with this index,if not give,this function will search the emote index by the command
	{"SetScreenShotFolder", LuaSetScreenShotFolder},//Set the ScreenShot save folder
	{"AutoReply", LuaSetAutoReply},			//设置自动回复 char* strReply
	{"OfflineTrans", LuaSetOfflineTransmit},//离线转发
	{"CheckChannel", LuaCheckChannel},			//char* strChannelName, int bCheck
	{"TransferCmd", LuaTransCmdToCore},		//中转操作命令到ｃｏｒｅ
	{"ChangeMiniMap", LuaChangeMiniMap},	//更换小地图
	{"WorldFilter", LuaWorldFilter},		//世界频道关键字过滤
	{"ConstructFilter", LuaConstructFilter},//构造每次过滤的关键字
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

int g_GetGameScriptFunNum()
{
	return sizeof(GameScriptFuns)  / sizeof(TLua_Funcs);
}

BOOL KShortcutKeyCentre::InitScript()
{
	if (ms_Script.Init() && ms_Script.RegisterFunctions(GameScriptFuns, g_GetGameScriptFunNum()))
	{
		return LoadScript(UI_AUTOEXEC_SETTING_FILE);
	}

	return FALSE;
}

BOOL KShortcutKeyCentre::LoadScript(char* pFileName)
{
	ClearScript();

	return ms_Script.Load(pFileName);
}

BOOL KShortcutKeyCentre::UninitScript()
{
	return ClearScript();
}

BOOL KShortcutKeyCentre::ClearScript()
{
	if (ms_pCommands)
		free(ms_pCommands);
	ms_pCommands = NULL;
	ms_nCommands = 0;

	ms_FunsMap.clear();

	g_UiChatPhrase.ClearAllPhrase();

	return TRUE;
}

#define SCRIPT_SECTION "ScriptAuto"

BOOL KShortcutKeyCentre::LoadPrivateSetting(KIniFile* pFile)
{
	if (pFile == NULL)
		return FALSE;

	int nVersion = 0;
	pFile->GetInteger(SCRIPT_SECTION, "Version", 0, &nVersion);

	int nVersionDefault = 1;

	KIniFile* pFileDefault = g_UiBase.GetCommConfigFile();
	if (pFileDefault)
		pFileDefault->GetInteger("ShortcutSet", "Version", 1, &nVersionDefault);

	if (nVersion < nVersionDefault)	//没有系统缺省版本高的私人版本不加载
		return FALSE;

	ClearScript();

	char szKey[10];
	char szLine[256];
	int i = 0;
	while (1)
	{
		sprintf(szKey, "%d", i);
		pFile->GetString(SCRIPT_SECTION, szKey, "", szLine, 255);
		if (szLine[0] == 0)
			break;
		else
		{
			if (ms_Script.LoadBuffer((PBYTE)szLine, strlen(szLine)))
			{
				ms_Script.ExecuteCode();
			}
		}
		i++;
	}
	return TRUE;
}

int	SaveBlackListPrivateSetting(KIniFile* pFile, LPCSTR lpSection, int nStart);

BOOL KShortcutKeyCentre::SavePrivateSetting(KIniFile* pFile)
{
	if (pFile == NULL)
		return FALSE;

	int nVersionDefault = 1;

	KIniFile* pFileDefault = g_UiBase.GetCommConfigFile();
	if (pFileDefault)
		pFileDefault->GetInteger("ShortcutSet", "Version", 1, &nVersionDefault);

	pFile->WriteInteger(SCRIPT_SECTION, "Version", nVersionDefault);

	char szKey[10];
	char szLine[512];
	int i = 0;
	for (; i < ms_nCommands; i++)
	{
		sprintf(szKey, "%d", i);
		sprintf(szLine, "AddCommand(\"%s\", \"%s\", \"%s\")", GetKeyName(ms_pCommands[i].uKey), ms_pCommands[i].szCommand, ms_pCommands[i].szDo);
		pFile->WriteString(SCRIPT_SECTION, szKey, szLine);
	}
	SHORTFUNCMAP::iterator iFun = ms_FunsMap.begin();
	while  (iFun != ms_FunsMap.end())
	{
		sprintf(szKey, "%d", i);
		int nLen = sprintf(szLine, "RegisterFunctionAlias(\"%s\", \"%s\", %d", iFun->first.c_str(), iFun->second.strName.c_str(),
				iFun->second.nParamNum);
		PARAMLIST::iterator iP = iFun->second.strDefaultParam.begin();
		while (iP != iFun->second.strDefaultParam.end())
		{
			std::string strP = (*iP);
			if (strP.empty() || strP == "\"\"")
				nLen += sprintf(szLine + nLen, ", \"\"");
			else
				nLen += sprintf(szLine + nLen, ", \"%s\"", strP.c_str());
			iP++;
		}
		strcat(szLine + nLen, ")");
		pFile->WriteString(SCRIPT_SECTION, szKey, szLine);
		iFun++;
		i++;
	}

	i = g_UiChatPhrase.SavePrivateSetting(pFile, SCRIPT_SECTION, i);
	i = g_UiChatPhrase.SavePrivateEmote(pFile, SCRIPT_SECTION, i);

	{
		int nChCount = KUiMsgCentrePad::GetChannelCount();
		for (int nC = 0; nC < nChCount; nC++)
		{
			if (!KUiMsgCentrePad::GetChannelSubscribe(nC))	//没有订阅的才存盘
			{
				sprintf(szKey, "%d", i);
				sprintf(szLine, "CheckChannel(\"%s\", %d)", KUiMsgCentrePad::GetChannelTitle(nC), 0);
				pFile->WriteString(SCRIPT_SECTION, szKey, szLine);
				i++;
			}
		}
	}

	KWndWindow* pWin = NULL;
	int nX, nY;
	int nW, nH;

	pWin = KUiMiniMap::GetSelf();
	if (pWin)
	{
		sprintf(szKey, "%d", i);
		pWin->GetPosition(&nX, &nY);
		sprintf(szLine, "MoveWindow(\"%s\", %d, %d)", "map", nX, nY);
		pFile->WriteString(SCRIPT_SECTION, szKey, szLine);
		i++;

		sprintf(szKey, "%d", i);
		sprintf(szLine, "ShowWindow(\"%s\", %d)", "map", pWin->IsVisible() ? 1 : 0);
		pFile->WriteString(SCRIPT_SECTION, szKey, szLine);
		i++;
	}
	pWin = KUiHeaderControlBar::GetSelf();
	if (pWin)
	{
		sprintf(szKey, "%d", i);
		pWin->GetPosition(&nX, &nY);
		sprintf(szLine, "MoveWindow(\"%s\", %d, %d)", "statustool", nX, nY);
		pFile->WriteString(SCRIPT_SECTION, szKey, szLine);
		i++;

		sprintf(szKey, "%d", i);
		sprintf(szLine, "ShowWindow(\"%s\", %d)", "statustool", pWin->IsVisible() ? 1 : 0);
		pFile->WriteString(SCRIPT_SECTION, szKey, szLine);
		i++;
	}
	pWin = KUiToolsControlBar::GetSelf();
	if (pWin)
	{
		sprintf(szKey, "%d", i);
		pWin->GetPosition(&nX, &nY);
		sprintf(szLine, "MoveWindow(\"%s\", %d, %d)", "normaltool", nX, nY);
		pFile->WriteString(SCRIPT_SECTION, szKey, szLine);
		i++;

		sprintf(szKey, "%d", i);
		sprintf(szLine, "ShowWindow(\"%s\", %d)", "normaltool", pWin->IsVisible() ? 1 : 0);
		pFile->WriteString(SCRIPT_SECTION, szKey, szLine);
		i++;
	}
	pWin = KUiMsgCentrePad::GetSelf();
	if (pWin)
	{
		sprintf(szKey, "%d", i);
		pWin->GetPosition(&nX, &nY);
		sprintf(szLine, "MoveWindow(\"%s\", %d, %d)", "chatroom", nX, nY);
		pFile->WriteString(SCRIPT_SECTION, szKey, szLine);
		i++;

		sprintf(szKey, "%d", i);
		pWin->GetSize(&nW, &nH);
		sprintf(szLine, "SizeWindow(\"%s\", %d, %d)", "chatroom", nW, nH);
		pFile->WriteString(SCRIPT_SECTION, szKey, szLine);
		i++;

		sprintf(szKey, "%d", i);
		sprintf(szLine, "ShowWindow(\"%s\", %d)", "chatroom", pWin->IsVisible() ? 1 : 0);
		pFile->WriteString(SCRIPT_SECTION, szKey, szLine);
		i++;

		sprintf(szKey, "%d", i);
		sprintf(szLine, "SetChatFontSize(%d)", KUiMsgCentrePad::GetFontSize());
		pFile->WriteString(SCRIPT_SECTION, szKey, szLine);
		i++;

		int nCIndex = 0;
		DWORD uColor = 0;
		DWORD uBColor = 0;
		char* pTitle = NULL;
		while (1)
		{
			uColor = 0;
			uBColor = 0;
			pTitle = KUiMsgCentrePad::GetChannelTextColor(nCIndex++, uColor, uBColor);
			if (pTitle == NULL)
				break;
			sprintf(szKey, "%d", i);
			sprintf(szLine, "SetChannelTextColor(\"%s\", \"%s\", \"%s\")", pTitle, std::string(GetColorString(uColor)).c_str(), std::string(GetColorString(uBColor)).c_str());
			pFile->WriteString(SCRIPT_SECTION, szKey, szLine);
			i++;
		}

		uColor = 0;
		uBColor = 0;
		KUiMsgCentrePad::GetMSNTextColor(0, uColor, uBColor);
		sprintf(szKey, "%d", i);
		sprintf(szLine, "SetMeTextColor(\"%s\", \"%s\")", std::string(GetColorString(uColor)).c_str(), std::string(GetColorString(uBColor)).c_str());
		pFile->WriteString(SCRIPT_SECTION, szKey, szLine);
		i++;

		uColor = 0;
		uBColor = 0;
		KUiMsgCentrePad::GetMSNTextColor(1, uColor, uBColor);
		sprintf(szKey, "%d", i);
		sprintf(szLine, "SetFriendTextColor(\"%s\", \"%s\")", std::string(GetColorString(uColor)).c_str(), std::string(GetColorString(uBColor)).c_str());
		pFile->WriteString(SCRIPT_SECTION, szKey, szLine);
		i++;

		uColor = 0;
		uBColor = 0;
		KUiMsgCentrePad::GetMSNTextColor(2, uColor, uBColor);
		sprintf(szKey, "%d", i);
		sprintf(szLine, "SetStrangerTextColor(\"%s\", \"%s\")", std::string(GetColorString(uColor)).c_str(), std::string(GetColorString(uBColor)).c_str());
		pFile->WriteString(SCRIPT_SECTION, szKey, szLine);
		i++;
	}

	i = SaveBlackListPrivateSetting(pFile, SCRIPT_SECTION, i);

	return i;
}


size_t Compile(const char* src, char* dst, size_t dstlen)
{
        static const char str_param_begin[] = " ( ";
        static const char str_param_end[] = " ) ";
        static const char str_param_split[] = ", ";
 		static const char str_quote_begin[] = "\"";
		static const char str_quote_end[] = "\"";
		static const size_t len_param_begin = sizeof(str_param_begin) - sizeof(str_param_begin[0]);
        static const size_t len_param_end = sizeof(str_param_end) - sizeof(str_param_end[0]);
        static const size_t len_param_split = sizeof(str_param_split) - sizeof(str_param_split[0]);
		static const char len_quote_begin = sizeof(str_quote_begin) - sizeof(str_quote_begin[0]);
		static const char len_quote_end = sizeof(str_quote_end) - sizeof(str_quote_end[0]);



        if (src == NULL || dst == NULL || dstlen <= 0)
                return 0;

		SHORTFUNCMAP::iterator iFun = KShortcutKeyCentre::ms_FunsMap.end();
        size_t uselen = 0;

        size_t cntToken = 0;
        const char* szToken = NULL;
        const char* szNext = src - 1;
        for ( ; ; )
        {
                for (szToken = szNext + 1; *szToken; szToken++)
                {
                        if (__x_isgraph(*szToken))
                                break;
                }
                if (!*szToken)
                        break;

                for (szNext = szToken + 1; *szNext; szNext++)
                {
                        if (!__x_isgraph(*szNext))
                                break;
                }

                size_t toklen = szNext - szToken;

                if (cntToken > 0)
                {//param
                        if (cntToken > 1)
                        {//non first param
                                if (!__x_memcpy_n(dst + uselen, dstlen - uselen, str_param_split, len_param_split))
									return 0;
                                uselen += len_param_split;
                        }

						if (!__x_memcpy_n(dst + uselen, dstlen - uselen, str_quote_begin, len_quote_begin))
							return 0;
						uselen += len_quote_begin;

						if (!__x_memcpy_n(dst + uselen, dstlen - uselen, szToken, toklen))
							return 0;
						uselen += toklen;

						if (!__x_memcpy_n(dst + uselen, dstlen - uselen, str_quote_end, len_quote_end))
							return 0;
						uselen += len_quote_end;
                }
                else
                {//func name
						iFun = KShortcutKeyCentre::ms_FunsMap.find(std::string(szToken, toklen));
						if (iFun == KShortcutKeyCentre::ms_FunsMap.end())
						{
							if (!__x_memcpy_n(dst + uselen, dstlen - uselen, szToken, toklen))
								return 0;
							uselen += toklen;
						}
						else
						{
							const std::string& name = iFun->second.strName;
							if (!__x_memcpy_n(dst + uselen, dstlen - uselen, name.c_str(), name.size()))
								return 0;
							uselen += name.size();
						}


                        if (!__x_memcpy_n(dst + uselen, dstlen - uselen, str_param_begin, len_param_begin))
                                return 0;
                        uselen += len_param_begin;
                }

                cntToken ++;


				if (!*szNext)
					break;
        }

        if (cntToken > 0)
        {//is func, fill ')'
			if (iFun != KShortcutKeyCentre::ms_FunsMap.end())
			{
				int nParamCount = cntToken - 1;
				if (iFun->second.nParamNum - nParamCount > 0)
				{	//补默认参数
					_ASSERT(iFun->second.strDefaultParam.size() == iFun->second.nParamNum);
					PARAMLIST::iterator iP = iFun->second.strDefaultParam.begin();
					for (int nSkip = nParamCount; nSkip > 0; nSkip--)
					{
						iP++;
					}

					for (int nDefault = iFun->second.nParamNum - nParamCount; nDefault > 0; nDefault--)
					{
						if (nParamCount > 0)
						{
							if (!__x_memcpy_n(dst + uselen, dstlen - uselen, str_param_split, len_param_split))
								return 0;
							uselen += len_param_split;
						}

						const std::string& name = (*iP);
						iP++;
						if (!__x_memcpy_n(dst + uselen, dstlen - uselen, name.c_str(), name.size()))
							return 0;
						uselen += name.size();

						nParamCount++;
					}
				}
			}

            if (!__x_memcpy_n(dst + uselen, dstlen - uselen, str_param_end, len_param_end))
				return 0;
            uselen += len_param_end;
        }

        return uselen;
}


BOOL KShortcutKeyCentre::TranslateExcuteScript(const char * ScriptCommand)
{
	if (ScriptCommand && ScriptCommand[0] != 0)
	{
		int nIndex = FindCommand(ScriptCommand);	//首先寻找快捷键带的名称
		if (nIndex >= 0)
		{
			if (ms_pCommands[nIndex].szDo[0] != 0)
				return ExcuteScript(ms_pCommands[nIndex].szDo);
		}
		else//翻译通常语法为严格语法
		{
			char szTrueCommand[512];
			szTrueCommand[0] = 0;
			//语法转换和函数名转换
			int nLen = Compile(ScriptCommand, szTrueCommand, 511);
			szTrueCommand[nLen] = 0;
			return ExcuteScript(szTrueCommand);
		}
	}

	return FALSE;
}

BOOL KShortcutKeyCentre::ExcuteScript(const char * ScriptCommand)
{
	DWORD dwStatus = g_UiBase.GetStatus();
	if ((!dwStatus && (dwStatus & ~UIS_S_STORE_BOX)) || !ms_Enable)
		return FALSE;

	if (ScriptCommand && ScriptCommand[0] != 0)
	{
		if ((ScriptCommand[0] != '-' || ScriptCommand[1] != '-') &&
			ScriptCommand[0] != '#')
		{
            if (ms_Script.LoadBuffer((PBYTE)ScriptCommand, strlen(ScriptCommand)))
				return ms_Script.ExecuteCode();
		}
	}

	return FALSE;
}

BOOL KShortcutKeyCentre::ExcuteHWNDScript(const char * ScriptCommand)
{
	if (ScriptCommand && ScriptCommand[0] != 0)
	{
		if (ms_Script.LoadBuffer((PBYTE)ScriptCommand, strlen(ScriptCommand)))
		{
			return ms_Script.ExecuteCode();
		}
	}

	return FALSE;
}

int KShortcutKeyCentre::AddCommand(COMMAND_SETTING* pAdd)	//复制Add数据并增加到Commands中，如果uKey!=0则覆盖原uKey，否则如szCommand[0]!=0则覆盖szCommand相同者
{
	int nIndex = -1;
	if (pAdd == NULL)
		return nIndex;
	if (pAdd->uKey != 0)
	{
		nIndex = FindCommand(pAdd->uKey);
	}
	else if (pAdd->szCommand[0] != 0)
	{
		nIndex = FindCommand(pAdd->szCommand);
	}
	else
		return nIndex;

	if (nIndex >= 0)
	{
		ms_pCommands[nIndex] = *pAdd;
	}
	else
	{
		if (ms_pCommands == NULL)
		{
			ms_pCommands = (COMMAND_SETTING*)malloc(sizeof(COMMAND_SETTING));
			ms_pCommands[0] = *pAdd;
			nIndex = 0;
			ms_nCommands = 1;
		}
		else
		{
			ms_pCommands = (COMMAND_SETTING*)realloc(ms_pCommands, sizeof(COMMAND_SETTING) * (ms_nCommands + 1));
			ms_pCommands[ms_nCommands] = *pAdd;
			nIndex = ms_nCommands;
			ms_nCommands++;
		}
	}
	return nIndex;
}

int	KShortcutKeyCentre::RemoveCommand(int nIndex)	//返回剩余Command的总数
{
	if (ms_pCommands && nIndex >= 0 && nIndex < ms_nCommands)
	{
		if (nIndex != ms_nCommands - 1)
		{
			memmove(ms_pCommands + nIndex, ms_pCommands + nIndex + 1, sizeof(COMMAND_SETTING));
		}

		ms_nCommands--;
	}

	return ms_nCommands;
}

void KShortcutKeyCentre::RemoveCommandAll()
{
	ms_nCommands = 0;
}

int	KShortcutKeyCentre::FindCommand(DWORD uKey)
{
	if (uKey == 0)
		return -1;
	for (int i = 0; i < ms_nCommands; i++)
	{
		if (ms_pCommands[i].uKey != 0 && ms_pCommands[i].uKey == uKey)
			return i;
	}
	return -1;
}

int	KShortcutKeyCentre::FindCommand(const char* szCommand)
{
	if (szCommand == NULL || szCommand[0] == 0)
		return -1;
	for (int i = 0; i < ms_nCommands; i++)
	{
		if (ms_pCommands[i].szCommand[0] != 0 &&
			strcmpi(ms_pCommands[i].szCommand, szCommand) == 0)
			return i;
	}
	return -1;
}

int	KShortcutKeyCentre::FindCommandByScript(const char* szScript)
{
	if (szScript == NULL || szScript[0] == 0)
		return -1;
	for (int i = 0; i < ms_nCommands; i++)
	{
		if (ms_pCommands[i].szDo[0] != 0 &&
			strcmpi(ms_pCommands[i].szDo, szScript) == 0)
			return i;
	}
	return -1;
}

DWORD KShortcutKeyCentre::GetCommandKey(int nIndex)
{
	if (nIndex >= 0 && nIndex < ms_nCommands)
	{
		return ms_pCommands[nIndex].uKey;
	}
	return 0;
}

const char*	KShortcutKeyCentre::GetKeyName(DWORD Key)
{
	static std::string s_descHK;

	s_descHK = hotkey_str::DescHotKey(Key);
	return s_descHK.c_str();
}

BOOL KShortcutKeyCentre::RegisterFunctionAlias(const char * strFunAlias, const char * strFun, int nParam, const PARAMLIST& List)
{
	if (strFunAlias && strFunAlias[0] != 0 &&
		strFun && strFun[0] != 0)
	{
		ShortFuncInfo info;
		info.strName = strFun;
		info.nParamNum = nParam;
		info.strDefaultParam = List;
		if (info.nParamNum >= (int)info.strDefaultParam.size())
		{
			for (int i = info.nParamNum - info.strDefaultParam.size(); i > 0; i--)
			{
				info.strDefaultParam.push_back("\"\"");
			}
		}
		else
		{
			for (int i = info.strDefaultParam.size() - info.nParamNum; i > 0; i--)
			{
				info.strDefaultParam.pop_back();
			}
		}
		_ASSERT(info.nParamNum == info.strDefaultParam.size());

		ms_FunsMap[strFunAlias] = info;
		return TRUE;
	}

	return FALSE;
}
