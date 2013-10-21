/*****************************************************************************************
//	界面--login窗口
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-8-12
*****************************************************************************************/
#include "KWin32.h"
#include "KIniFile.h"
#include "../Elem/WndMessage.h"
#include "../Elem/Wnds.h"
#include "../UiBase.h"
#include "UiInformation.h"
#include "UiLogin.h"
#include "UiSelServer.h"
#include "UiLoginBg.h"
#include "UiConnectInfo.h"
#include "../../Login/Login.h"
#include "../UiSoundSetting.h"
#include "../../ENGINE/Src/KSG_MD5_String.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

#define	SCHEME_INI_LOGIN				"登陆.ini"


KUiLogin* KUiLogin::m_pSelf = NULL;

//--------------------------------------------------------------------------
//	功能：构造函数
//--------------------------------------------------------------------------
KUiLogin::KUiLogin()
{
	m_szLoginBg[0] = 0;
}

//--------------------------------------------------------------------------
//	功能：析构函数
//--------------------------------------------------------------------------
KUiLogin::~KUiLogin()
{
}

//--------------------------------------------------------------------------
//	功能：打开窗口，返回唯一的一个类对象实例
//--------------------------------------------------------------------------
KUiLogin* KUiLogin::OpenWindow()
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiLogin;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
	{
		UiSoundPlay(UI_SI_POPUP_OUTGAME_WND);
        m_pSelf->Show();
	}
    return m_pSelf;
}

//--------------------------------------------------------------------------
//	功能：关闭窗口，同时可以选则是否删除对象实例
//--------------------------------------------------------------------------
void KUiLogin::CloseWindow(bool bDestroy)
{
	if (m_pSelf)
	{
		g_LoginLogic.SetRememberAccountFlag(m_pSelf->m_RememberAccount.IsButtonChecked() != 0);
		g_LoginLogic.SetInvisibleLogin(m_pSelf->m_InvisibleLogin.IsButtonChecked() != 0);
		if (bDestroy)
		{
			m_pSelf->Destroy();
			m_pSelf = NULL;
		}
		else
		{
			m_pSelf->m_PassWord.ClearText(true);
			m_pSelf->Hide();
		}
	}
}

void KUiLogin::Show()
{
	KWndShowAnimate::Show();
	KUiLoginBackGround::SetConfig(m_szLoginBg);

	char	szAccount[32];

	m_RememberAccount.CheckButton(g_LoginLogic.GetLoginAccount(szAccount));
	g_LoginLogic.LoadAccountSetting();
	m_InvisibleLogin.CheckButton(g_LoginLogic.IsInvisibleLogin());
	m_Account.SetText(szAccount);
	if (szAccount[0])
		Wnd_SetFocusWnd(&m_PassWord);
	else
		Wnd_SetFocusWnd(&m_Account);
}

//--------------------------------------------------------------------------
//	功能：初始化
//--------------------------------------------------------------------------
void KUiLogin::Initialize()
{
	AddChild(&m_Account);
	AddChild(&m_PassWord);
	AddChild(&m_Login);
	AddChild(&m_Cancel);
	AddChild(&m_RememberAccount);
	AddChild(&m_InvisibleLogin);

	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);

	Wnd_AddWindow(this, WL_TOPMOST);
}

//--------------------------------------------------------------------------
//	功能：载入窗口的界面方案
//--------------------------------------------------------------------------
void KUiLogin::LoadScheme(const char* pScheme)
{
	char		Buff[128];
	KIniFile	Ini;
	sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI_LOGIN);
	if (Ini.Load(Buff))
	{
		KWndShowAnimate::Init(&Ini, "Main");
		m_Account .Init(&Ini, "Account");
		m_PassWord.Init(&Ini, "Password");
		m_Login   .Init(&Ini, "Login");
		m_Cancel  .Init(&Ini, "Cancel");
		m_RememberAccount.Init(&Ini, "Remember");
		m_InvisibleLogin.Init(&Ini, "Invisible");
		Ini.GetString("Main", "LoginBg", "", m_szLoginBg, sizeof(m_szLoginBg));
	}
}

//--------------------------------------------------------------------------
//	功能：窗口函数
//--------------------------------------------------------------------------
int KUiLogin::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int	nRet = 0;
	switch(uMsg)
	{
	case WND_N_BUTTON_CLICK:
		if (uParam == (unsigned int)(KWndWindow*)&m_Login)
			OnLogin();
		else if (uParam == (unsigned int)(KWndWindow*)&m_Cancel)
			OnCancel();
		break;
	case WND_N_EDIT_SPECIAL_KEY_DOWN:
		if (nParam == VK_TAB)
		{
            if ((GetKeyState(VK_SHIFT) & 0x8000) == 0)
            {
                // 如果没有按住Shift
			    if (uParam == (unsigned int)(KWndWindow*)&m_Account)
			    {
				    int x, y, cx;
				    m_PassWord.GetAbsolutePos(&x, &y);
				    m_PassWord.GetSize(&cx, NULL);
				    Wnd_SetCursorPos(x + cx, y);
				    Wnd_SetFocusWnd(&m_PassWord);
				    nRet = 1;
			    }
			    else if (uParam == (unsigned int)(KWndWindow*)&m_PassWord)
			    {
				    Wnd_SetFocusWnd(NULL);
					m_InvisibleLogin.SetCursorAbove();
				    nRet = 1;
			    }
            }
            else
            {
                // 如果按住Shift
			    if (uParam == (unsigned int)(KWndWindow*)&m_Account)
			    {
 				    Wnd_SetFocusWnd(NULL);
      			    m_Cancel.SetCursorAbove();
				    nRet = 1;
			    }
			    else if (uParam == (unsigned int)(KWndWindow*)&m_PassWord)
			    {
			        int x, y, cx;
			        m_Account.GetAbsolutePos(&x, &y);
			        m_Account.GetSize(&cx, NULL);
			        Wnd_SetCursorPos(x + cx, y);
			        Wnd_SetFocusWnd(&m_Account);

				    nRet = 1;
			    }
            }
		}
		else if (nParam == VK_RETURN)
		{
			if (uParam == (unsigned int)(KWndWindow*)&m_Account)
			{
				Wnd_SetFocusWnd(&m_PassWord);
				nRet = 1;
			}
			else if (uParam == (unsigned int)(KWndWindow*)&m_PassWord)
			{
				OnLogin();
				nRet = 1;
			}
		}
		else if (nParam == VK_ESCAPE)
		{
			OnCancel();
			nRet = 1;
		}
		break;
	case WM_KEYDOWN:
		OnKeyDown(uParam);
		nRet = 1;
		break;
	case WM_SYSKEYDOWN:
		if (uParam == 'A')
			OnEnableAutoLogin();
		break;
	case WND_N_EDIT_CHANGE:
		if(uParam == (unsigned int)(&m_Account))
		{
			char szBuffer[32];
			m_Account.GetText(szBuffer, sizeof(szBuffer), true);
			g_LoginLogic.SetLoginAccount(szBuffer);
			g_LoginLogic.LoadAccountSetting();
			m_InvisibleLogin.CheckButton(g_LoginLogic.IsInvisibleLogin());
		}
		break;
	default:
		nRet = KWndShowAnimate::WndProc(uMsg, uParam, nParam);
		break;
	}
	return nRet;
}

void KUiLogin::OnEnableAutoLogin()
{
	KIniFile*	pSetting = g_UiBase.GetCommConfigFile();
	int nAutoLogin = false;
	if (pSetting)
	{
		pSetting->GetInteger("Main", "AutoLogin", 0, &nAutoLogin);
		g_UiBase.CloseCommConfigFile();
	}
	if (nAutoLogin == 6323)
	{
		g_LoginLogic.SetRememberAllFlag(true);
		m_RememberAccount.CheckButton(true);
	}
}

void KUiLogin::OnKeyDown(unsigned int uKey)
{
	if (uKey == VK_RETURN)
	{
		if (m_Cancel.IsButtonActive())
			OnCancel();
		else
			OnLogin();
	}
	else if (uKey == VK_TAB)
	{
        if ((GetKeyState(VK_SHIFT) & 0x8000) == 0)
        {
            // 如果没有按住Shift
			if (m_InvisibleLogin.IsButtonActive())
				m_Login.SetCursorAbove();
    		else if (m_Login.IsButtonActive())
			    m_RememberAccount.SetCursorAbove();
		    else if (m_RememberAccount.IsButtonActive())
			    m_Cancel.SetCursorAbove();
		    else
		    {
			    int x, y, cx;
			    m_Account.GetAbsolutePos(&x, &y);
			    m_Account.GetSize(&cx, NULL);
			    Wnd_SetCursorPos(x + cx, y);
			    Wnd_SetFocusWnd(&m_Account);
		    }
        }
        else
        {
            // 如果按住Shift
    		if (m_InvisibleLogin.IsButtonActive())
            {
				int x, y, cx;
				m_PassWord.GetAbsolutePos(&x, &y);
				m_PassWord.GetSize(&cx, NULL);
				Wnd_SetCursorPos(x + cx, y);
				Wnd_SetFocusWnd(&m_PassWord);
            }
			else if (m_Login.IsButtonActive())
			{
				m_InvisibleLogin.SetCursorAbove();
			}
		    else if (m_RememberAccount.IsButtonActive())
            {
	    	    m_Login.SetCursorAbove();
		    }
            else   // if m_Cancel.IsButtonActive()
		    {
                m_RememberAccount.SetCursorAbove();
		    }

        }
	}
	else if (uKey == VK_SPACE)
	{
		if (m_Login.IsButtonActive())
			OnLogin();
		else if (m_Cancel.IsButtonActive())
			OnCancel();
		else if (m_RememberAccount.IsButtonActive())
			m_RememberAccount.CheckButton(!m_RememberAccount.IsButtonChecked());
		else if (m_InvisibleLogin.IsButtonActive())
			m_InvisibleLogin.CheckButton(!m_InvisibleLogin.IsButtonChecked());
	}
	else if (uKey == VK_LEFT)
		m_Login.SetCursorAbove();
	else if (uKey == VK_RIGHT)
		m_Cancel.SetCursorAbove();
	else if (uKey == VK_ESCAPE)
		OnCancel();
}

//--------------------------------------------------------------------------
//	功能：登录
//--------------------------------------------------------------------------
void KUiLogin::OnLogin()
{
	char szAccount[32];
    char	     szPassword[KSG_PASSWORD_MAX_SIZE];
	KSG_PASSWORD Password;
	if (GetInputInfo(szAccount, szPassword))
	{
        #ifdef SWORDONLINE_USE_MD5_PASSWORD

        KSG_StringToMD5String(Password.szPassword, szPassword);

        #else

        #pragma message (KSG_ATTENTION("Add Password to MD5 string"))
        strncpy(Password.szPassword, szPassword, sizeof(Password.szPassword));
        Password.szPassword[sizeof(Password.szPassword) - 1] = '\0';

        #endif

		g_LoginLogic.SetInvisibleLogin(m_InvisibleLogin.IsButtonChecked() != FALSE);
		g_LoginLogic.AccountLogin(szAccount, Password);
		KUiConnectInfo::OpenWindow(CI_MI_CONNECTING, LL_S_ROLE_LIST_READY);
		memset(&szPassword, 0, sizeof(szPassword));
		memset(&Password, 0, sizeof(Password));
		CloseWindow(false);
	}
}

//--------------------------------------------------------------------------
//	功能：取消
//--------------------------------------------------------------------------
void KUiLogin::OnCancel()
{
	CloseWindow(false);
	g_LoginLogic.ReturnToIdle();
	KUiSelServer::OpenWindow();	//点击『取消』按钮返回『初始界面』；
}

//--------------------------------------------------------------------------
//	功能：获取输入的账号密码信息
//--------------------------------------------------------------------------
int KUiLogin::GetInputInfo(char* pszAccount, char* pszPassword)
{
	_ASSERT(pszAccount && pszPassword);
	if (m_Account.GetText(pszAccount, 32, false) &&
		m_PassWord.GetText(pszPassword, 32, false))
	{
		return true;
	}
	CloseWindow(false);
	KUiConnectInfo::OpenWindow(CI_MI_ERROR_LOGIN_INPUT, CI_NS_LOGIN_WND);
	return false;
/*	int nLen = m_Account.GetText(m_szAccount, 32, false);
	unsigned char	cCode;
	if (nLen >= LOGIN_ACCOUNT_MIN_LEN && nLen <= LOGIN_ACCOUNT_MAX_LEN)
	{
		for (int i = 0; i < nLen;)
		{
			cCode = (unsigned char)m_szAccount[i];
			if (i + 2 <= nLen && cCode >= 0x81 && cCode <= 0xfe &&
				(cCode <= 0xa0 || cCode >= 0xaa))
			{
				i += 2;
			}
			else if ((cCode >= 0x30 && cCode <= 0x39) ||
				(cCode >= 0x41 && cCode <= 0x5a) ||
				(cCode >= 0x61 && cCode <= 0x7a))
			{
				i++;
			}
			else
				break;
		}

		if (i == nLen)
		{
			nLen = m_PassWord.GetText(m_szPassword, 32, false);
			if (IsValidPwd(m_szPassword, nLen))
				return true;
		}
	}

	KUiConnectInfo::OpenWindow(CI_MI_ACCOUNT_PWD_ERROR, CI_AE_BACK_TO_LOGIN);
	CloseWindow(false);
	return false;*/
}

bool KUiLogin::IsValidPassword(const char* pszPassword, int nLen)
{
	return true;	//to be del this line;

	if (pszPassword)
	{
		if (nLen < 0)
			nLen = strlen(pszPassword);

		if (nLen >= LOGIN_PASSWORD_MIN_LEN && nLen <= LOGIN_PASSWORD_MAX_LEN)
		{
			for (int i = 0; i < nLen; i++)
			{
				unsigned char cCode = (unsigned char)pszPassword[i];
				if ((cCode < 0x30 || cCode > 0x39) &&
					(cCode < 0x41 || cCode > 0x5a) &&
					(cCode < 0x61 || cCode > 0x7a))
				{
					break;
				}
				if (i == nLen)
					return true;
			}
		}
	}
	return false;
}