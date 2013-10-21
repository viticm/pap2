#include "stdafx.h"
#include "./kmouse.h"
#include "../elem/wnds.h"
//#include "../uibase.h"

namespace UI
{
	KMouse::~KMouse()
	{
	}

	int KMouse::SetCurrentCursorIndex(int nIndex)
	{
		return Wnd_SwitchCursor(nIndex);
	}

	int KMouse::GetCurrentCursorByStatus() const
	{
#if 0
		int	nCursorIndex = CURSOR_NORMAL;
		switch (g_UiBase.GetAppearStatus())
		{
		case UIS_S_TRADE_PLAYER:	//（与Player）交易
		case UIS_S_TRADE_NPC:		//（与npc）交易
			nCursorIndex = CURSOR_NORMAL;
			break;
		case UIS_S_TRADE_SALE:		//（与npc）交易 卖
			nCursorIndex = CURSOR_SELL;
			break;
		case UIS_S_TRADE_BUY:		//（与npc）交易 买
			nCursorIndex = CURSOR_BUY;
			break;
		case UIS_S_TRADE_REPAIR:	//（与npc）交易 修理
			nCursorIndex = CURSOR_REPAIR;
			break;
		case UIS_S_STALL_SETPRICE:
			nCursorIndex = CURSOR_STALL_MARK_PIRCE;	//暂时用这个鼠标指针来标记[wxb 2003-11-18]
			break;
		case UIS_S_STALL_BUY:
			nCursorIndex = CURSOR_BUY;	//摆摊:(买方)处于买东西状态
			break;
		case UIS_S_IDENTIFY:
			nCursorIndex = CURSOR_IDENTIFY; // 鉴定物品
			break;
		case UIS_S_INTERACTIVE:
			nCursorIndex = CURSOR_INTERACTIVE;	//与玩家交互
			break;
		case UIS_S_SEPARATE_ITEM:
			nCursorIndex = CURSOR_SEPARATE_ITEM;	// 拆分物品
			break;
		}
		return nCursorIndex;
#endif // 0
		return 0;
	}

	int KMouse::HandleMouseInput(unsigned int uKey, int nModifier, int x, int y)
	{
		// TODO : 跟KShortcutkeyCenter连接
		return 0;
	}

}

