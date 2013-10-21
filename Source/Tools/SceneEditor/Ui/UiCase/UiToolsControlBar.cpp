/*****************************************************************************************
//	界面--屏幕顶控制操作条
//	Copyright : Kingsoft 2003
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2003-4-22
*****************************************************************************************/
#include "KWin32.h"
#include "KIniFile.h"
#include "../elem/wnds.h"
#include "../Elem/WndMessage.h"
#include "../UiBase.h"
#include "UiToolsControlBar.h"
#include "UiChatCentre.h"

#include "../ShortcutKey.h"
#include "../../../core/src/coreshell.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

extern iCoreShell*		g_pCoreShell;

#define	SCHEME_INI		"工具控制条.ini"

KUiToolsControlBar* KUiToolsControlBar::m_pSelf = NULL;

//--------------------------------------------------------------------------
//	功能：打开窗口，返回唯一的一个类对象实例
//--------------------------------------------------------------------------
KUiToolsControlBar* KUiToolsControlBar::OpenWindow()
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiToolsControlBar;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
		m_pSelf->Show();
	return m_pSelf;
}

//--------------------------------------------------------------------------
//	功能：关闭窗口
//--------------------------------------------------------------------------
void KUiToolsControlBar::CloseWindow()
{
	if (m_pSelf)
	{
		m_pSelf->Destroy();
		m_pSelf = NULL;
	}
}

//初始化
void KUiToolsControlBar::Initialize()
{
	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);

	m_Style &= ~WND_S_VISIBLE;
	Wnd_AddWindow(this, WL_TOPMOST);
}

//载入界面方案
void KUiToolsControlBar::LoadScheme(const char* pScheme)
{
	char		Buff[128];
	KIniFile	Ini;
	if (m_pSelf)
	{
		sprintf(Buff, "%s\\"SCHEME_INI, pScheme);
		if (Ini.Load(Buff))
		{
			m_pSelf->Init(&Ini, "Main");
		}
	}
}

//重新初始化界面
void KUiToolsControlBar::DefaultScheme(const char* pScheme)
{
	char		Buff[128];
	KIniFile	Ini;
	if (m_pSelf)
	{
		sprintf(Buff, "%s\\"SCHEME_INI, pScheme);
		if (Ini.Load(Buff))
		{
			int	nValue1, nValue2;
			Ini.GetInteger("Main", "Left",  0, &nValue1);
			Ini.GetInteger("Main", "Top",   0, &nValue2);
			m_pSelf->SetPosition(nValue1, nValue2);
		}
	}
}

void KUiToolsControlBar::Breathe()
{
	UpdateData();
}

