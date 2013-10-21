/*****************************************************************************************
//	界面--选择游戏服务器窗口
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-8-12
*****************************************************************************************/
#include "KWin32.h"
#include "KIniFile.h"
#include "../Elem/WndMessage.h"
#include "../Elem/Wnds.h"
#include "../UiShell.h"
#include "UiInit.h"
#include "UiLoginBg.h"
#include "UiConnectInfo.h"
#include "UiSelServer.h"
#include "../UiSoundSetting.h"
#include "../UiBase.h"
#include "../../Login/Login.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

#define	SCHEME_INI_SELSERV				"选服务器.ini"
#define	SERVER_LIST_FILE				"\\Settings\\ServerList.ini"

KUiSelServer* KUiSelServer::m_pSelf = NULL;

//--------------------------------------------------------------------------
//	功能：构造函数
//--------------------------------------------------------------------------
KUiSelServer::KUiSelServer()
{
	m_pServList = NULL;
	m_szLoginBg[0] = 0;
	m_bSelRegion = true;
	m_nRegionIndex = 0;
}

KUiSelServer::~KUiSelServer()
{
	m_List.SetContent(NULL, 0, 0, 0);
	if (m_pServList)
	{
		free(m_pServList);
		m_pServList = NULL;
	}
}

//--------------------------------------------------------------------------
//	功能：打开窗口，返回唯一的一个类对象实例
//--------------------------------------------------------------------------
KUiSelServer* KUiSelServer::OpenWindow()
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiSelServer;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
	{
		UiSoundPlay(UI_SI_POPUP_OUTGAME_WND);
		KUiLoginBackGround::SetConfig(m_pSelf->m_szLoginBg);
		m_pSelf->m_bSelRegion = true;
		m_pSelf->GetList();
		m_pSelf->Show();
	}
	return m_pSelf;
}

//--------------------------------------------------------------------------
//	功能：关闭窗口
//--------------------------------------------------------------------------
void KUiSelServer::CloseWindow(bool bDestroy)
{
	if (m_pSelf)
	{
		if (bDestroy)
		{
			m_pSelf->Destroy();
			m_pSelf = NULL;
		}
		else
		{
			m_pSelf->Hide();
		}
	}
}

//--------------------------------------------------------------------------
//	功能：初始化
//--------------------------------------------------------------------------
void KUiSelServer::Initialize()
{
	AddChild(&m_List);
	AddChild(&m_ScrollBar);
	AddChild(&m_Login);
	AddChild(&m_Cancel);

	m_List.SetScrollbar(&m_ScrollBar);

	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);
	Wnd_AddWindow(this, WL_TOPMOST);
}

void KUiSelServer::GetList()
{
	m_List.SetContent(NULL, 0, 0, 0);
	if (m_pServList)
	{
		free(m_pServList);
		m_pServList = NULL;
	}

	int nCount, nSel;
	if (m_bSelRegion)
	{
		g_LoginLogic.LoadLoginChoice();
		m_pServList = g_LoginLogic.GetServerRegionList(nCount, m_nRegionIndex);
		if (m_pServList)
		{
			m_List.SetContent((BYTE*)m_pServList, nCount, sizeof(KLoginServer), 0);
			m_List.SetCurSel(m_nRegionIndex);
		}
	}
	else
	{
		m_pServList = g_LoginLogic.GetServerList(m_nRegionIndex, nCount, nSel);
		if (m_pServList)
		{
			m_List.SetContent((BYTE*)m_pServList, nCount, sizeof(KLoginServer), 0);
			m_List.SetCurSel(nSel);
		}
	}
}

//--------------------------------------------------------------------------
//	功能：载入窗口的界面方案
//--------------------------------------------------------------------------
void KUiSelServer::LoadScheme(const char* pScheme)
{
	char		Buff[128];
	KIniFile	Ini;
	sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI_SELSERV);
	if (Ini.Load(Buff))
	{
		KWndShowAnimate::Init(&Ini, "Main");
		m_List     .Init(&Ini, "List");
		m_ScrollBar.Init(&Ini, "Scroll");
		m_Login    .Init(&Ini, "Login");
		m_Cancel   .Init(&Ini, "Cancel");

		Ini.GetString("Main", "LoginBg", "", m_szLoginBg, sizeof(m_szLoginBg));
	}
}

//--------------------------------------------------------------------------
//	功能：窗口函数
//--------------------------------------------------------------------------
int KUiSelServer::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int	nRet = 0;
	switch(uMsg)
	{
	case WND_N_BUTTON_CLICK:
		OnClickButton((KWndButton*)(KWndWindow*)uParam);
		break;
	case WND_N_SCORLLBAR_POS_CHANGED:
		m_List.SetTopItemIndex(nParam);
		break;
	case WM_KEYDOWN:
		nRet = OnKeyDown(uParam);
		break;
	case WND_N_LIST_ITEM_D_CLICK:
		if (nParam >= 0 && uParam == (unsigned int)(KWndWindow*)&m_List)
			OnLogin();
		break;
	default:
		nRet = KWndShowAnimate::WndProc(uMsg, uParam, nParam);
	}
	return nRet;
}

int KUiSelServer::OnKeyDown(unsigned int uKey)
{
	int	nRet = 1;
	KWndWindow* pToActive = NULL;
	switch (uKey)
	{
	case VK_RETURN:
		if (m_Cancel.IsButtonActive())
			OnCancel();
		else
			OnLogin();
		break;
	case VK_ESCAPE:
		OnCancel();
		break;
	case VK_LEFT:
		if (m_Login.IsButtonActive() == 0)
			pToActive = &m_Login;
		break;
	case VK_RIGHT:
		if (m_Cancel.IsButtonActive() == 0)
			pToActive = &m_Cancel;
		break;
	case VK_UP:
		nRet = m_List.GetCurSel();
		if (nRet  > 0)
			m_List.SetCurSel(nRet - 1);
		else if (nRet < 0)
			m_List.SetCurSel(0);
		m_ScrollBar.SetScrollPos(m_List.GetTopItemIndex());
		nRet = 1;
		break;
	case VK_DOWN:
		nRet = m_List.GetCurSel();
		if (nRet < m_List.GetCount() - 1)
			m_List.SetCurSel(nRet + 1);
		else if (nRet < 0)
			m_List.SetCurSel(0);
		m_ScrollBar.SetScrollPos(m_List.GetTopItemIndex());
		nRet = 1;
		break;
	default:
		nRet = 0;
		break;
	}
	if (pToActive)
		pToActive->SetCursorAbove();
	return nRet;
}

void KUiSelServer::OnClickButton(KWndButton* pBtn)
{
	if (pBtn == &m_Login)
		OnLogin();
	else if(pBtn == &m_Cancel)
		OnCancel();
}

//--------------------------------------------------------------------------
//	功能：登陆游戏世界服务器
//--------------------------------------------------------------------------
void KUiSelServer::OnLogin()
{
	int nSelServer = m_List.GetCurSel();
	if (m_pServList && nSelServer >= 0)
	{
		if (m_bSelRegion)
		{
			m_nRegionIndex = nSelServer;
			m_bSelRegion = false;
			GetList();
		}
		else
		{
	        g_LoginLogic.SetAccountServer(m_pServList[nSelServer]);
			g_LoginLogic.CreateConnection(m_pServList[nSelServer].Address);
			KUiConnectInfo::OpenWindow(CI_MI_CONNECTING, LL_S_WAIT_INPUT_ACCOUNT);
			CloseWindow(false);
		}
	}
}

//--------------------------------------------------------------------------
//	功能：取消
//--------------------------------------------------------------------------
void KUiSelServer::OnCancel()
{
	if (m_bSelRegion)
	{
		CloseWindow(true);
		KUiInit::OpenWindow(false, false);
	}
	else
	{
		m_bSelRegion = true;
		GetList();
	}
}
