// -------------------------------------------------------------------------
//	文件名		：	UiGetString.cpp
//	创建者		：	Wooy(Wu yue)
//	创建时间	：	2003-3-18
//	功能描述	：	获得字符串界面
// -------------------------------------------------------------------------
#include "KWin32.h"
#include "KIniFile.h"
#include "../Elem/AutoLocateWnd.h"
#include "../Elem/WndMessage.h"
#include "../Elem/Wnds.h"
#include "../UiBase.h"
#include "UiTongGetString.h"
#include "UiInformation.h"
#include "../UiSoundSetting.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

#define	SCHEME_INI 	"输入字串界面2.ini"

KUiTongGetString* KUiTongGetString::m_pSelf = NULL;

KUiTongGetString::KUiTongGetString()
{
	m_pRequester = NULL;
}

//--------------------------------------------------------------------------
//	功能：打开窗口，返回唯一的一个类对象实例
//--------------------------------------------------------------------------
KUiTongGetString* KUiTongGetString::OpenWindow(const char* pszTitle,
						const char* pszInitString,
						KWndWindow* pRequester, unsigned int uParam,
 						int nMinLen, int nMaxLen)
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiTongGetString;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
	{
		UiSoundPlay(UI_SI_WND_OPENCLOSE);
		m_pSelf->m_nMinLen = nMinLen;
		m_pSelf->m_nMaxLen = nMaxLen;
		m_pSelf->m_pRequester = pRequester;
		m_pSelf->m_uRequesterParam = uParam;
		if (pszTitle)
			m_pSelf->m_Title.SetText(pszTitle);
		else
			m_pSelf->m_Title.SetText("");
		if (pszInitString)
			m_pSelf->m_StringEdit.SetText(pszInitString);
		else
			m_pSelf->m_StringEdit.SetText("");

		m_pSelf->BringToTop();
		m_pSelf->Show();
	}
	return m_pSelf;
}

//--------------------------------------------------------------------------
//	功能：关闭销毁窗口
//--------------------------------------------------------------------------
void KUiTongGetString::CloseWindow(bool bDestroy)
{
	if (m_pSelf)
	{
		m_pSelf->m_pRequester = NULL;
		if (bDestroy)
		{
			m_pSelf->Destroy();
			m_pSelf = NULL;
		}
		else
			m_pSelf->Hide();
	}
}

KUiTongGetString*	KUiTongGetString::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	return NULL;
}

//显示窗口
void KUiTongGetString::Show()
{
	int Left, Top;
	ALW_GetWndPosition(Left, Top, m_Width, m_Height);
	SetPosition(Left, Top);
	KWndImage::Show();
	Wnd_SetFocusWnd(&m_StringEdit);
	Wnd_SetExclusive((KWndWindow*)this);
}

//隐藏窗口
void KUiTongGetString::Hide()
{
	Wnd_SetFocusWnd(NULL);
	Wnd_ReleaseExclusive((KWndWindow*)this);
	KWndImage::Hide();
}

//--------------------------------------------------------------------------
//	功能：初始化
//--------------------------------------------------------------------------
void KUiTongGetString::Initialize()
{
	AddChild(&m_Title);
	AddChild(&m_StringEdit);
	AddChild(&m_OkBtn);
	AddChild(&m_CancelBtn);
	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);

	Wnd_AddWindow(this);
}

//--------------------------------------------------------------------------
//	功能：载入窗口的界面方案
//--------------------------------------------------------------------------
void KUiTongGetString::LoadScheme(const char* pScheme)
{
	if (m_pSelf)
	{
		char		Buff[128];
		KIniFile	Ini;
		sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI);
		if (Ini.Load(Buff))
		{
			m_pSelf->Init(&Ini, "Main");
			m_pSelf->m_Title.Init(&Ini, "Title");
			m_pSelf->m_StringEdit.Init(&Ini, "StringInput");
			m_pSelf->m_OkBtn.Init(&Ini, "OkBtn");
			m_pSelf->m_CancelBtn.Init(&Ini, "CancelBtn");
		}
	}
}

//--------------------------------------------------------------------------
//	功能：窗口消息函数
//--------------------------------------------------------------------------
int KUiTongGetString::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int nRet = 0;
	switch(uMsg)
	{
	case WND_N_BUTTON_CLICK:
		if (uParam == (unsigned int)(KWndWindow*)&m_OkBtn)
			OnOk();
		else if (uParam == (unsigned int)(KWndWindow*)&m_CancelBtn)
			OnCancel();
		break;
	case WM_KEYDOWN:
		if (uParam == VK_RETURN)
		{
			OnOk();
			nRet = 1;
		}
		else if (uParam == VK_ESCAPE)
		{
			OnCancel();
			nRet = 1;
		}
		break;
/*	case WND_N_EDIT_SPECIAL_KEY_DOWN:
		if (nParam == VK_RETURN &&
			uParam == (unsigned int)(KWndWindow*)&m_StringEdit)
		{
			OnOk();
			return 1;
		}
		break;*/
	default:
		return KWndImage::WndProc(uMsg, uParam, nParam);
	}
	return 0;
}

//--------------------------------------------------------------------------
//	功能：响应点击确认按钮
//--------------------------------------------------------------------------
void KUiTongGetString::OnOk()
{
	char	szString[32];

	int nLen = m_StringEdit.GetText(szString, sizeof(szString));
	if (nLen >= m_nMinLen && nLen <= m_nMaxLen)
	{
		if (m_pRequester)
			m_pRequester->WndProc(WND_M_OTHER_WORK_RESULT, m_uRequesterParam, (int)&szString);
		CloseWindow(true);
	}
	else
	{
		UIMessageBox(((nLen < m_nMinLen) ?
			"输入的字串太少！" : "输入的字数超过允许的最大长度！"));
	}
}

void KUiTongGetString::OnCancel()
{
	if (m_pRequester)
		m_pRequester->WndProc(WND_M_OTHER_WORK_RESULT, m_uRequesterParam, 0);
	CloseWindow(true);
}
