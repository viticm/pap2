/*
 * File:     UiStallAdv.cpp
 * Desc:     摆摊广告
 * Author:   flying
 * Creation: 2003/10/23
 */
#include "KWin32.h"
#include "KIniFile.h"
#include "../elem/wnds.h"
#include "uitrade.h"
#include "UiStall.h"
#include "UiStallMarkPrice.h"
#include "UiStoreBox.h"
#include "UiTradeConfirmWnd.h"
#include "UiShop.h"
#include "../../../core/src/coreshell.h"
#include "../../../core/src/GameDataDef.h"
#include "../UiBase.h"
#include "../ShortcutKey.h"
#include "../UiSoundSetting.h"
#include "UiStallSetAdv.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

extern iCoreShell* g_pCoreShell;
KUiStallSetAdv* KUiStallSetAdv::m_pSelf = NULL;
#define		SCHEME_INI_SETADV		"摆摊设置广告.ini"

KAdvEdit::KAdvEdit()
{
	m_Text[0] = m_Text[sizeof(m_Text) - 1] = 0;
	SetTextPtr(m_Text, sizeof(m_Text));
}

KUiStallSetAdv::KUiStallSetAdv()
{
	m_bCallBack = false;
}

KUiStallSetAdv::~KUiStallSetAdv()
{
}

KUiStallSetAdv* KUiStallSetAdv::OpenWindow(bool bShow, bool bCallBack)
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiStallSetAdv;
		if (m_pSelf)
			m_pSelf->Initialize();
	}

	if (m_pSelf)
	{
		m_pSelf->m_bCallBack = bCallBack;
		if (bShow)
		{
			m_pSelf->Show();
			m_pSelf->BringToTop();
			Wnd_SetFocusWnd(&m_pSelf->AdvEditor);
		}
		else
			m_pSelf->Hide();
	}
	return m_pSelf;
}

void KUiStallSetAdv::CloseWindow(bool bDestroy)
{
	if (m_pSelf)
	{
		m_pSelf->m_bCallBack = false;
		m_pSelf->Hide();
		if (bDestroy)
		{
			m_pSelf->Destroy();
			m_pSelf = NULL;
		}
	}
}

KUiStallSetAdv* KUiStallSetAdv::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	else
		return NULL;
}

void KUiStallSetAdv::LoadScheme(const char* pScheme)
{
	if (m_pSelf)
	{
		char		Buff[128];
		KIniFile	Ini;
		sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI_SETADV);
		if (Ini.Load(Buff))
		{
			m_pSelf->Init(&Ini, "Main");
			m_pSelf->m_BtnOk.Init(&Ini, "BtnOK");
			m_pSelf->m_BtnCancel.Init(&Ini, "BtnCancel");
			m_pSelf->AdvEditor.Init(&Ini,"AdvEditor");
		}
	}
	return;
}

void KUiStallSetAdv::Initialize()
{
	AddChild(&m_BtnOk);
	AddChild(&m_BtnCancel);
	AddChild(&AdvEditor);
	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);
	Wnd_AddWindow(this, WL_TOPMOST);
	return;
}

int KUiStallSetAdv::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int nRet = 0;
	switch(uMsg)
	{
	case WND_N_BUTTON_CLICK:
		if (uParam == (unsigned int)(KWndWindow*)&m_BtnOk)
			OnOK();
		else if (uParam == (unsigned int)(KWndWindow*)&m_BtnCancel)
			OnCancel();
		break;
	case WM_KEYDOWN:
		if (uParam == VK_RETURN)
		{
			OnOK();
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
			uParam == (unsigned int)(KWndWindow*)&AdvEditor)
		{
			OnOK();
			nRet = 1;
		}
		break;
//	case WND_N_EDIT_CHANGE:
//		CheckInput();
//		break;
	default:
		nRet = KWndImage::WndProc(uMsg, uParam, nParam);
	}
	return nRet;
}

#include "..\ChatFilter.h"
#include "UiSysMsgCentre.h"

void KUiStallSetAdv::OnOK()
{
	static char szBuf[256];

	if (AdvEditor.GetText(szBuf, sizeof(szBuf) - 1, true) > 0)
	{
		extern CChatFilter g_ChatFilter;
		if (!g_ChatFilter.IsTextPass(szBuf)) //如果通不过过滤
		{
			KSystemMessage	sMsg;
			strcpy(sMsg.szMessage, "您的广告包含不合适的文字,请修改!");
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			KUiSysMsgCentre::AMessageArrival(&sMsg, NULL);
			return;
		}

		int nStallState = g_pCoreShell->GetGameData(GDI_IS_STALL, -1, -1);
		if (nStallState == 0)
		{
			g_pCoreShell->OperationRequest(GOI_SET_ADVSTRING,
				(unsigned int)szBuf, strlen(szBuf) + 1);
			if (m_bCallBack)
				g_pCoreShell->OperationRequest(GOI_SWITCH_STALL, 0, 0);
		}
		CloseWindow(false);
	}
}

void KUiStallSetAdv::OnCancel()
{
	CloseWindow(false);
}
