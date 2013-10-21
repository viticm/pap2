/*****************************************************************************************
//	界面--选颜色
//	Copyright : Kingsoft 2003
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2003-4-29
//  注释：此功能原来放在快速聊天输入窗口内，现独立出来
------------------------------------------------------------------------------------------
*****************************************************************************************/
#pragma once

#include "../Elem/PopupMenu.h"
#include "../../../Represent/iRepresent/KRepresentUnit.h"
#include "../../../Core/src/GameDataDef.h"

class KUiSelColor
{
public:
	static void			LoadScheme(const char* pScheme);//载入界面方案
	static void			PopupMenu(KWndWindow* pCaller, unsigned int uParam, int x, int y, int nWidth);
	static void			CancelMenu();
	static unsigned int GetColor(int nIndex);
private:
	enum { UI_INPUT_MSG_MAX_COLOR = 12 };
	static unsigned char	m_MenuData[MENU_DATA_SIZE(UI_INPUT_MSG_MAX_COLOR)];
};

