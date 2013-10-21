/*****************************************************************************************
//	界面--选项界面
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-9-2
------------------------------------------------------------------------------------------
*****************************************************************************************/
#pragma once

#include "../Elem/WndLabeledButton.h"
#include "../Elem/WndScrollBar.h"
#include "../Elem/WndList.h"
//#include "../../Engine/Src/LinkStruct.h"

struct KPopupMenuData;

enum	SWORD_ONLINE_OPTION_INDEX
{
	OPTION_I_START = 0,
	OPTION_I_DYNALIGHT = OPTION_I_START,	//动态光影
	OPTION_I_WEATHER,						//天气开关
	OPTION_I_PERSPECTIVE,					//透视模式

	OPTION_INDEX_COUNT,
};

struct KToggleOptionItem
{
	char	szName[32];	//名称
	bool	bInvalid;	//不可用（有效）
	short	bEnable;	//此选项是否选中
};

class KUiOptions : protected KWndImage
{
public:
	//----界面面板统一的接口函数----
	static KUiOptions*	OpenWindow(KWndWindow* pReturn = NULL);//打开窗口，返回唯一的一个类对象实例
	static KUiOptions*	GetIfVisible();					//如果窗口正被显示，则返回实例指针
	static void			CloseWindow();					//关闭窗口
	static void			LoadScheme(const char* pScheme);//载入界面方案
	
	static void			LoadSetting(bool bReload, bool bUpdate);

//	void				SetPerspective(int);
//	void				SetDynaLight(int);
	void				SetMusicValue(int);
	void				SetSoundValue(int);
	void				SetBrightness(int);
//	void                SwitchWeather();
	void				ToggleOption(int nIndex);	//切换开关型选项

private:
	KUiOptions();
	~KUiOptions() {}
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);	//窗口函数
	void	OnScrollBarPosChanged(KWndWindow* pWnd, int nPos);	//响应滚动条被拖动
//	void	PopupSkinMenu();
	void	CancelMenu();
	void	StoreSetting();
	void	UpdateSettingSet(int eSet, bool bOnlyUpdateUi = false);
	void	Initialize();					// 初始化
	void    PopupSeleteSetMenu(int nX, int nY);	//弹出选择配置方案的菜单
	void	LoadScheme(KIniFile* pIni);	//载入界面方案
	void	UpdateAllToggleBtn();
	void	UpdateAllStatusImg();

private:
	static KUiOptions* m_pSelf;
private:
	KWndWindow* m_pReturn;

	// 按钮
	KWndButton		m_ShortcutKeyBtn;	//打开快捷键设定界面
	KWndButton		m_CloseBtn;			//关闭按钮
//	KWndLabeledButton	m_SkinBtn;		//界面方案按钮

	KWndScrollBar	m_BrightnessScroll;	//亮度调节滑块
	KWndScrollBar	m_BGMValue;			//音乐音量滑块
	KWndScrollBar	m_SoundValue;		//音效音量滑块
	KPopupMenuData*	m_pSkinMenu;

	KWndLabeledButton m_ShortcutSetView;//e...这个表示当前所选择的快捷键方案

	int	m_nBrightness, m_nSoundValue, m_nMusicValue;
	int m_nShortcutSet;

#define	MAX_TOGGLE_BTN_COUNT	4
	KWndScrollBar		m_Scroll;
	KWndLabeledButton	m_ToggleBtn[MAX_TOGGLE_BTN_COUNT];
	KWndImage		m_StatusImage[MAX_TOGGLE_BTN_COUNT];
	unsigned int	m_uEnableTextColor;		//m_ToggleBtn标题文字的颜色
	unsigned int	m_uDisableTextColor;	//m_ToggleBtn标题文字的颜色
	unsigned int	m_uInvalidTextColor;	//m_ToggleBtn标题文字的颜色
	int				m_nStatusEnableFrame;
	int				m_nStatusDisableFrame;
	int				m_nStatusInvalidFrame;

	KToggleOptionItem	m_ToggleItemList[OPTION_INDEX_COUNT];
	int					m_nFirstControlableIndex;
	int					m_nToggleBtnValidCount;
	int					m_nToggleItemCount;
};
