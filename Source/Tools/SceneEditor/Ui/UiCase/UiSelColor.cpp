/*****************************************************************************************
//	界面--选颜色
//	Copyright : Kingsoft 2003
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2003-4-29
//  注释：此功能原来放在快速聊天输入窗口内，现独立出来
*****************************************************************************************/
#include "KWin32.h"
#include "KIniFile.h"
#include "UiSelColor.h"
#include "../Elem/WndWindow.h"
#include "../UiBase.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

#define	SCHEME_INI		"选颜色.ini"

unsigned char KUiSelColor::m_MenuData[MENU_DATA_SIZE(UI_INPUT_MSG_MAX_COLOR)] = { 0 };

//--------------------------------------------------------------------------
//	功能：如果窗口正被显示，则返回实例指针
//--------------------------------------------------------------------------
unsigned int KUiSelColor::GetColor(int nIndex)
{
	KPopupMenuData* pMenuData = (KPopupMenuData*)&m_MenuData;
	if (nIndex >= 0 && nIndex < pMenuData->nNumItem)
		return (pMenuData->Items[nIndex].uBgColor);
	return 0;
}

void KUiSelColor::LoadScheme(const char* pScheme)
{
	#define	INI_COLOR_SECTION "ColorList"
	int			i;
	KIniFile	Ini;
	char		Buffer[128], Value[4];
	sprintf(Buffer, "%s\\"SCHEME_INI, pScheme);
	if (Ini.Load(Buffer) == FALSE)
		return;

	KPopupMenuData* pMenuData = (KPopupMenuData*)&m_MenuData;
	pMenuData->nNumItem = 0;
	pMenuData->usMenuFlag =  PM_F_ZOOM_SELECTED_ITEM;
	pMenuData->byFontSize = 12;
	pMenuData->nItemTitleIndent = 0;
	pMenuData->byItemTitleUpSpace = 0;
	pMenuData->nSelectedItem = -1;
	pMenuData->uBorderLineColor = 0xff000000;
	pMenuData->uSelItemBgColor = 0;
	pMenuData->uSelTextColor = 0;
	pMenuData->uTextColor = 0;

	Ini.GetInteger(INI_COLOR_SECTION, "Width", 24, &i);
	pMenuData->nItemWidth = i;
	Ini.GetInteger(INI_COLOR_SECTION, "Height", 12, &i);
	pMenuData->nItemHeight = i;

	for (i = 0; i < UI_INPUT_MSG_MAX_COLOR; i++)
	{
		itoa(i, Value, 10);
		if (Ini.GetString("ColorList", Value, "", Buffer, sizeof(Buffer)))
		{
			pMenuData->Items[i].uBgColor = (::GetColor(Buffer) & 0xFFFFFF);
			pMenuData->nNumItem ++;
		}
		else
			break;
	}
}

void KUiSelColor::PopupMenu(KWndWindow* pCaller, unsigned int uParam, int x, int y, int nWidth)
{
	KPopupMenu::Cancel();
	KPopupMenuData* pMenuData = (KPopupMenuData*)&m_MenuData;
	pMenuData->nX = x;
	pMenuData->nY = y;
	pMenuData->nItemWidth = nWidth;
	KPopupMenu::Popup(pMenuData, pCaller, uParam);
}

void KUiSelColor::CancelMenu()
{
	KPopupMenu::Cancel();
}
