////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : UiItemShadowCommon.cpp
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2006-1-5 14:17:37
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "Stdafx.h"
#include <Windows.h>
#include <tchar.h>
#include "UiItemShadowCommon.h"
#include "UiEditor.h"
#include "resource.h"
#include "UiItemCommon.h"
#include "ClientManage.h"
#include "UiColorManage.h"

////////////////////////////////////////////////////////////////////////////////

KUiItemShadowCommon::KUiItemShadowCommon()
{
    m_szName[0] = '\0';
}

KUiItemShadowCommon::~KUiItemShadowCommon()
{
	if (m_hWnd)
	{
		::DestroyWindow(m_hWnd);
		m_hWnd = NULL;
	}
}

KUiWndWindow *KUiItemShadowCommon::Create()
{
	KUiItemShadowCommon* pWnd = new KUiItemShadowCommon;
	return pWnd;
}

int KUiItemShadowCommon::Init(HWND hWnd, RECT* pRc, const char *pszName)
{
    int nResult = false;
    HWND hwndCombo = NULL;
    HWND hSpin = NULL;
    KG_PROCESS_ERROR(hWnd);
    KG_PROCESS_ERROR(pRc);
    KG_PROCESS_ERROR(pszName);

    strncpy(m_szName, pszName, sizeof(m_szName));
    m_hWnd = ::CreateDialog(
        (HINSTANCE)g_hPluginInstance, 
        MAKEINTRESOURCE(IDD_ItemShadowCommon), 
        hWnd, (DLGPROC)WndProc
    );
    KG_PROCESS_ERROR(m_hWnd);

	::SetWindowPos(m_hWnd, HWND_TOP, pRc->left, pRc->top,
		pRc->right - pRc->left, pRc->bottom - pRc->top,
		SWP_SHOWWINDOW);
	::SetWindowLong(m_hWnd, GWL_USERDATA, (LONG)this);

    hSpin = ::GetDlgItem(m_hWnd, IDC_ITEM_SHADOW_COLOR_ALPHA_CHANGE);
    if (hSpin) 
        ::SendMessage(hSpin, UDM_SETRANGE, 0, MAKELPARAM(255, 0));

    nResult = true;
Exit0:
    if (!nResult)
        Release();
    return nResult;
}

void KUiItemShadowCommon::Release()
{
    delete this;
}

LRESULT CALLBACK KUiItemShadowCommon::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	KUiItemShadowCommon*	pView = (KUiItemShadowCommon*)GetWindowLong(hWnd, GWL_USERDATA);
	if (!pView)
		return 0;
	if (message == WM_COMMAND)
	{
		int nNotify = HIWORD(wParam);
		switch(nNotify)
		{
		case EN_CHANGE:
			pView->OnUpdateEditText(LOWORD(wParam));
			break;
		case 0:
			pView->OnButtonClick(LOWORD(wParam));
			break;
		}
	}
	return 0;
}

void KUiItemShadowCommon::OnButtonClick(int nBtnId)
{
    int nUpdateShower = true;
    int nRetCode = false;
    char szValue[32];
    szValue[0] = '\0';
    switch(nBtnId)
    {
    case IDC_ITEM_SHADOW_COLOR_SEL:
        if (UICOLORMANAGE::SelColor(m_hWnd, szValue)) 
        {
            ::SetDlgItemText(m_hWnd, IDC_ITEM_SHADOW_COLOR, szValue);
            g_Ui.GetIni(true)->WriteString(m_szName, "ShadowColor", szValue);
        }
        break;
    default:
        nUpdateShower = false;
        break;
    }
    if (nUpdateShower)
    {
        g_ClientManager.UpDataWndInClient(g_Ui.GetIni(true));
        ShowErrorInformation();
    }
}

void KUiItemShadowCommon::OnUpdateEditText(int nId)
{
    int nValue = 0;
    int nUpdateShower = true;
    char szValue[32];
    szValue[0] = '\0';    
    switch(nId)
    {
    case IDC_ITEM_SHADOW_COLOR:
        ::GetDlgItemText(m_hWnd, IDC_ITEM_SHADOW_COLOR, szValue, sizeof(szValue));
        g_Ui.GetIni()->WriteString(m_szName, "ShadowColor", szValue);
        break;
    case IDC_ITEM_SHADOW_COLOR_ALPHA:
        nValue = ::GetDlgItemInt(m_hWnd, IDC_ITEM_SHADOW_COLOR_ALPHA, NULL, true);
        g_Ui.GetIni()->WriteInteger(m_szName, "Alpha", nValue);
        break;
    default:
        nUpdateShower = false;
        break;
    }
    if (nUpdateShower)
    {
        g_ClientManager.UpDataWndInClient(g_Ui.GetIni(true));
        ShowErrorInformation();
    }
}

void KUiItemShadowCommon::UpdateData(const char* pszName, KUiWndWindowData& data, IIniFile* pIni)
{
    int nRetCode = false;
    int nValue = 0;
    char szValue[32];
    KG_PROCESS_ERROR(pszName);
    KG_PROCESS_ERROR(pIni);

    strncpy(m_szName, pszName, sizeof(m_szName));

    pIni->GetString(m_szName, "ShadowColor", "", szValue, sizeof(szValue));
    ::SetDlgItemText(m_hWnd, IDC_ITEM_SHADOW_COLOR, szValue);
    pIni->GetInteger(m_szName, "Alpha", 0, &nValue);
    ::SetDlgItemInt(m_hWnd, IDC_ITEM_SHADOW_COLOR_ALPHA, nValue, TRUE);

Exit0:
    return;
}

void KUiItemShadowCommon::Show(int bShow, int nSubWnd /* = -1 */)
{
	if (m_hWnd)
		::ShowWindow(m_hWnd, bShow ? SW_SHOW : SW_HIDE);
}

