/*
 * File:     UiStallMarkPrice.cpp
 * Desc:     摆摊标价
 * Author:   flying
 * Creation: 2003/10/23
 */
#include "KWin32.h"
#include "KIniFile.h"
#include "../Elem/AutoLocateWnd.h"
#include "../Elem/WndMessage.h"
#include "../Elem/Wnds.h"
#include "../UiBase.h"
#include "UiStallMarkPrice.h"
#include "../UiSoundSetting.h"
#include "../../Core/Src/CoreShell.h"

//#include "../../linux/core/KPlayerStall.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

#define	SCHEME_INI 	"摆摊标价.ini"

extern iCoreShell*		g_pCoreShell;

KUiStallMarkPrice* KUiStallMarkPrice::m_pSelf = NULL;
KWndText256   KUiStallMarkPrice::m_ObjectName;
KUiStallMarkPrice::KUiStallMarkPrice()
{
	m_nMaxMoney = 0;
}

//--------------------------------------------------------------------------
//	功能：打开窗口，返回唯一的一个类对象实例
//--------------------------------------------------------------------------
KUiStallMarkPrice* KUiStallMarkPrice::OpenWindow(int nMoney, int nMaxMoney, int nObjIdx,
							unsigned int uParam, KWndWindow* pMoneyWnd)
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiStallMarkPrice;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
	{
		UiSoundPlay(UI_SI_WND_OPENCLOSE);
		if (nMaxMoney > 0)
			m_pSelf->m_nMaxMoney = nMaxMoney;
		else
			m_pSelf->m_nMaxMoney = 0;
		m_pSelf->m_nObjIndex = nObjIdx;
		m_pSelf->m_cPrice.SetIntText(nMoney);
		m_pSelf->Show(pMoneyWnd);
		m_pSelf->BringToTop();
	}
	return m_pSelf;
}

//--------------------------------------------------------------------------
//	功能：关闭销毁窗口
//--------------------------------------------------------------------------
void KUiStallMarkPrice::CloseWindow(bool bDestroy)
{
	if (m_pSelf)
	{
		if (bDestroy)
		{
			m_pSelf->Destroy();
			m_pSelf = NULL;
		}
		else
			m_pSelf->Hide();
	}
}

KUiStallMarkPrice*	KUiStallMarkPrice::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	return NULL;
}

//显示窗口
void KUiStallMarkPrice::Show(KWndWindow* pMoneyWnd)
{
	int Left, Top;
	if (pMoneyWnd)
	{
		pMoneyWnd->GetAbsolutePos(&Left, &Top);
		int	nWidth;
		pMoneyWnd->GetSize(&nWidth, NULL);
		Left += nWidth / 2;
		ALW_GetWndPosition(Left, Top, m_Width, m_Height, true);
	}
	else
		ALW_GetWndPosition(Left, Top, m_Width, m_Height);
	SetPosition(Left, Top);
	KWndImage::Show();
	Wnd_SetFocusWnd(&m_cPrice);
	Wnd_SetExclusive((KWndWindow*)this);
}

//隐藏窗口
void KUiStallMarkPrice::Hide()
{
	Wnd_ReleaseExclusive((KWndWindow*)this);
	KWndImage::Hide();
}

//--------------------------------------------------------------------------
//	功能：初始化
//--------------------------------------------------------------------------
int KUiStallMarkPrice::Initialize()
{
	AddChild(&m_cPrice);
	AddChild(&m_OkBtn);
	AddChild(&m_CancelBtn);
	AddChild(&m_ObjectName);
	Wnd_AddWindow(this);

	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);

	return true;
}

//--------------------------------------------------------------------------
//	功能：载入窗口的界面方案
//--------------------------------------------------------------------------
void KUiStallMarkPrice::LoadScheme(const char* pScheme)
{
	if (m_pSelf)
	{
		char		Buff[128];
		KIniFile	Ini;
		sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI);
		if (Ini.Load(Buff))
		{
			m_pSelf->Init(&Ini, "Main");
			m_pSelf->m_cPrice.Init(&Ini, "PriceInput");
			m_pSelf->m_OkBtn.Init(&Ini, "OkBtn");
			m_pSelf->m_CancelBtn.Init(&Ini, "CancelBtn");
			m_pSelf->m_ObjectName.Init(&Ini, "ObjectName");
		}
	}
}

//--------------------------------------------------------------------------
//	功能：窗口消息函数
//--------------------------------------------------------------------------
int KUiStallMarkPrice::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
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
	case WND_N_EDIT_SPECIAL_KEY_DOWN:
		if (nParam == VK_RETURN &&
			uParam == (unsigned int)(KWndWindow*)&m_cPrice)
		{
			OnOk();
			nRet = 1;
		}
		break;
	case WND_N_EDIT_CHANGE:
		CheckInput();
		break;
	default:
		nRet = KWndImage::WndProc(uMsg, uParam, nParam);
	}
	return nRet;
}

//--------------------------------------------------------------------------
//	功能：响应点击确认按钮，实际标价
//--------------------------------------------------------------------------
void KUiStallMarkPrice::OnOk()
{
	CheckInput();
	int nMoney = m_cPrice.GetIntNumber();
	if (g_pCoreShell)
		g_pCoreShell->OperationRequest(GOI_MARK_STALL_PRICE, m_nObjIndex, nMoney);
	CloseWindow(true);
}

void KUiStallMarkPrice::CheckInput()
{
	int nMoney = m_cPrice.GetIntNumber();
	if (nMoney < 0)
		nMoney = 0;
	else if (nMoney > m_nMaxMoney)
		nMoney = m_nMaxMoney;
	char	szBuff1[16], szBuff2[16];
	itoa(nMoney, szBuff1, 10);
	m_cPrice.GetText(szBuff2, sizeof(szBuff2), true);
	if (strcmp(szBuff1, szBuff2))
		m_cPrice.SetIntText(nMoney);
}

void KUiStallMarkPrice::OnCancel()
{
	if (g_pCoreShell)
		g_pCoreShell->OperationRequest(GOI_MARK_STALL_PRICE, m_nObjIndex, 0);
	CloseWindow(true);
}
void   KUiStallMarkPrice::SetObjectName(char *pObjectName)
{
	m_ObjectName.SetText(pObjectName);
}