/*****************************************************************************************
//	弹出选择菜单
//	Copyright : Kingsoft 2003
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2003-2-21
------------------------------------------------------------------------------------------
*****************************************************************************************/
#pragma once

struct KPopupMenuItem
{
	char			szData[64];
	unsigned int	uDataLen;
	unsigned int	uBgColor;
	unsigned short	uID;
};

#define MENU_DEFAULT_POS				-607
#define	MENU_ITEM_DEFAULT_WIDTH			0
#define	MENU_ITEM_DEFAULT_HEIGHT		0
#define	MENU_ITEM_DEFAULT_RIGHT_WIDTH	-1
#define MENU_ITEM_DEFAULT_INDENT		255
#define MENU_ITEM_DEFAULT_TITLEUPSPACE	1

enum POPUPMENU_FLAG
{
	PM_F_HAVE_HEAD_TAIL_IMG		= 0x0001,		//在菜单顶上底下添加边界图形
	PM_F_ZOOM_SELECTED_ITEM		= 0x0002,		//放大当前选中的菜单项
	PM_F_TAB_SPLIT_ITEM_TEXT	= 0x0004,		//tab字符分割的菜单项
	PM_F_HAVE_ITEM_SEPARATOR	= 0x0008,		//菜单项之间有分隔线
	PM_F_AUTO_DEL_WHEN_HIDE		= 0x0010,		//菜单消失的时候自动删除菜单数据
	PM_F_CANCEL_BY_CALLER		= 0x0020,		//由调用者来负责否菜单的消失/关闭
};

struct KPopupMenuData
{
	short			nX;					//菜单弹出位置的x坐标
	short			nY;					//菜单弹出位置的y坐标
	short			nXX;				//原始的菜单弹出位置的x坐标
	short			nYY;				//原始的菜单弹出位置的y坐标

	short			nItemWidth;			//菜单项的宽度（包括下面的宽度）
	short			nItemRightWidth;	//菜单项右部特殊点击区域的宽度
	short			nItemHeight;		//菜单项的高度
	short			nNumItem;			//菜单项的数目
	short			nSelectedItem;		//当前选中的菜单项
	unsigned short	usMenuFlag;			//菜单特性的一些标记，为枚举类型POPUPMENU_FLAG取值的组合

	unsigned char	byFontSize;			//菜单项字体的大小
	unsigned char	byItemTitleUpSpace;	//菜单项文字离项顶端的间隔（单位：像素点）
	short			nItemTitleIndent;	//菜单项文字缩进

//	bool			bZoomSelectedItem;	//是否放大当前选中的菜单项
//	bool			bHaveHeadTailImg;	//是否在菜单顶上底下添加边界图形
//	bool			bHaveSeparator;		//菜单项是否有分隔线
//	bool			bAutoDelete;		//是否菜单消失的时候自动删除菜单数据
//	bool			bManualCancel;		//是否由调用者来决定是否菜单消失

	unsigned int	uBorderLineColor;	//菜单的边框颜色，为0则无
	unsigned int	uSelItemBgColor;	//被选中菜单项的背景色
	unsigned int	uTextColor;			//菜单项的文字色
	unsigned int	uTextBorderColor;	//菜单项的文字边缘色
	unsigned int	uSelTextColor;		//被选中菜单项的文字色
	unsigned int	uSelTextBorderColor;//被选中菜单项的文字边缘色

	KPopupMenuItem	Items[1];			//菜单项数据
};

#define	MENU_DATA_SIZE(nNumItem)	(sizeof(KPopupMenuData) + (nNumItem - 1) * sizeof(KPopupMenuItem))

class KWndWindow;
class KPopupMenu
{
public:
	static void	Popup(KPopupMenuData* pMenu, KWndWindow* pCaller, unsigned int uParam);
	static void Cancel();
	static void	PaintMenu();
	static void	LoadTheme(const char* pScheme);		//载入界面方案
	static int	HandleInput(unsigned int uMsg, unsigned int uParam, int nParam);//窗口函数
	static void OnWndDelete(KWndWindow* pWnd);
	static void	InitMenuData(KPopupMenuData* pMenu, int nNumItem, int nFontSize = 12);
	static KPopupMenuData*	GetMenuData() {	return m_pMenu;}
	static int  GetHeight() {return m_nMenuHeight;}
private:
	static bool SelectItem(int x, int y, bool bApply);
	static bool SelectItem(int nIndex, bool bApply);
private:
	static KPopupMenuData*  m_pMenu;
	static KWndWindow*		m_pCaller;
	static unsigned int		m_uCallerParam;
	
	static short			m_nImgWidth;	//图形水平长度
	static short			m_nImgHeight;	//图形垂直高度
	static short			m_nIndent;
    static short			m_nFrame;
	static int				m_nMenuHeight;
	static char				m_szImage[128];	//图形文件名

	static unsigned int	m_uDefBorderLineColor;	//菜单的变框颜色，为0则无
	static unsigned int	m_uDefItemBgColor;		//菜单项的背景色
	static unsigned int	m_uDefSelItemBgColor;	//被选中菜单项的背景色
	static unsigned int	m_uDefTextColor;		//菜单项的文字色
	static unsigned int	m_uDefTextBorderColor;	//菜单项的文字边缘色
	static unsigned int	m_uDefSelTextColor;		//被选中菜单项的文字色
	static unsigned int	m_uDefSelTextBorderColor;//被选中菜单项的文字边缘色
};
