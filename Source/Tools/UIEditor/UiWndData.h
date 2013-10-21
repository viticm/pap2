#pragma once

enum UI_WND_TYPE
{
	UI_WND_WINDOW = 0,
	UI_WND_IMAGE,
	UI_WND_BUTTON,
	UI_WND_SCROLLBAR,
    UI_WND_EDIT,
    UI_WND_SHOWANIMATE,
    UI_WND_PAGE,
    UI_WND_PAGESET,
    UI_WND_FRAME,
    UI_WND_CHECKBOX,
	UI_WND_MODEL,
	UI_WND_TEXTURE,
	UI_WND_MINIMAP,
	UI_WND_SCENE,
	UI_WND_WEBPAGE,

    // 下面是Item的
    UI_ITEM_NULL,
    UI_ITEM_TEXT,
    UI_ITEM_IMAGE,
    UI_ITEM_SHADOW,
    UI_ITEM_ANIAMTE,
	UI_ITEM_BOX,
    UI_ITEM_SCENE,
    UI_ITEM_HANDLE,
    UI_ITEM_TREE_LEAFE,

    UI_WND_TYPE_COUNT,
    UI_WND_TYPE_UNKNOWN = UI_WND_TYPE_COUNT,
};

struct	KUiWndWindowData
{
	UI_WND_TYPE	Type;
	int			Left;
	int			Top;
	int			Width;
	int			Height;
	char		Comment[128];
	char		Parent[64];
	KUiWndWindowData()
	{
		Type = UI_WND_WINDOW;
		Left = Top = Width = Height = 0;
		Comment[0] = 0;
		Parent[0] = 0;
	}
};

struct KUiWndImageData : KUiWndWindowData
{
	char	Image[128];
	int		Frame;
};

struct KUIWNDPAGEINFOMATION 
{
    int m_PageCount;
    char m_szPage[8][32];
};

int         g_GetWndType(const char *pszType);
int			g_GetWndType(const char* pszType, UI_WND_TYPE& Type);
const char *g_GetWndTypeKey(UI_WND_TYPE Type);
const char *g_GetWndTypeString(UI_WND_TYPE Type);
const char *g_GetWndTypePrefix(UI_WND_TYPE Type);
