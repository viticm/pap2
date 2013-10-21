/*
 * File:     KUiHelper.h
 * Desc:     简单帮助界面
 * Author:   flying
 * Creation: 2003/7/16
 */
//-----------------------------------------------------------------------------
#pragma warning(disable:4243)

#include "KWin32.h"
#include "KIniFile.h"
#include "../UiBase.h"
#include "../Elem/Wnds.h"
#include "UiHelper.h"
#include "../ShortcutKey.h"
#include "UiHeaderControlBar.h"
#include "UiToolsControlBar.h"
#include "UiMsgCentrePad.h"
#include "UiMiniMap.h"
#include "UiChatCentre.h"
#include "UiOptions.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

KUiHelper* KUiHelper::m_pSelf = NULL;
int        KUiHelper::ms_nImgFrame = 0;

#define		SCHEME_INI_HELP		"简单帮助界面.ini"
#define     INTERFACE_FRAME       0
#define     KEYBOARD_FRAME        1
#define     WUXING_INTERFIX_FRAME 2

KUiHelper::KUiHelper()
{
}

KUiHelper::~KUiHelper()
{
}

KUiHelper* KUiHelper::OpenWindow(bool bShow)
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiHelper;
		if (m_pSelf)
			m_pSelf->Initialize();
	}

	if (m_pSelf)
	{
		if (bShow)
		{
			m_pSelf->Show();
			m_pSelf->BringToTop();
		}
		else
			m_pSelf->Hide();
	}
	return m_pSelf;
}

void KUiHelper::CloseWindow(bool bDestroy)
{
	if (m_pSelf)
	{
		m_pSelf->Hide();
		if (bDestroy)
		{
			m_pSelf->Destroy();
			m_pSelf = NULL;
		}
	}
}

KUiHelper* KUiHelper::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	else
		return NULL;
}

//初始化
void KUiHelper::Initialize()
{
	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);
	Wnd_AddWindow(this, WL_TOPMOST);
	return;
}

int KUiHelper::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int nResult = false;

	switch(uMsg)
	{
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_KEYDOWN:
		KShortcutKeyCentre::Enable(true);
		KUiMsgCentrePad::ShowAllMessage();
		CloseWindow(false);
		nResult = true;
		break;
	default:
		nResult = KWndWindow::WndProc(uMsg, uParam, nParam);
		break;
	}
	return nResult;
}

void KUiHelper::LoadScheme(const char* pScheme)
{
	char		Buff[128];
	KIniFile	InitFile;
	sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI_HELP);

	if (InitFile.Load(Buff))
	{
		switch(ms_nImgFrame)
		{
		case KEYBOARD_FRAME:
			InitFile.GetString("Main", "KeyboardImage", "", Buff, sizeof(Buff));
			if(Buff[0])
				InitFile.WriteString("Main", "Image", Buff);
			break;
		case WUXING_INTERFIX_FRAME:
			InitFile.GetString("Main", "WuxingImage", "", Buff, sizeof(Buff));
			if(Buff[0])
				InitFile.WriteString("Main", "Image", Buff);
			break;
		case INTERFACE_FRAME:
			break;
		default:
			break;
		}
		Init(&InitFile, "Main");
	}
	return;
}

bool UiCloseWndsInGame(bool bAll);

void KUiHelper::Show()
{
	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	KUiHeaderControlBar::DefaultScheme(Scheme);
	KUiToolsControlBar::DefaultScheme(Scheme);
	KUiMsgCentrePad::DefaultScheme(Scheme);
	KUiMsgCentrePad::HideAllMessage();
	MapSetMode(MINIMAP_M_BRIEF_PIC);
	KUiMiniMap::LoadScheme(Scheme);
	KUiChatCentre::CloseWindow(false);
	UiCloseWndsInGame(false);
	KWndImage::Show();
	KShortcutKeyCentre::Enable(false);
}

void KUiHelper::SelectInterfacePic()
{
	char Scheme[256];

	ms_nImgFrame = INTERFACE_FRAME;
	if(m_pSelf)
	{
    	g_UiBase.GetCurSchemePath(Scheme, 256);
    	m_pSelf->LoadScheme(Scheme);
	}
}

void KUiHelper::SelectKeyboardPic()
{
	char Scheme[256];

	ms_nImgFrame = KEYBOARD_FRAME;
	if(m_pSelf)
	{
    	g_UiBase.GetCurSchemePath(Scheme, 256);
    	m_pSelf->LoadScheme(Scheme);
	}
}

void KUiHelper::SelectWuxingPic()
{
	char Scheme[256];

	ms_nImgFrame = WUXING_INTERFIX_FRAME;
	if(m_pSelf)
	{
    	g_UiBase.GetCurSchemePath(Scheme, 256);
    	m_pSelf->LoadScheme(Scheme);
	}
}
