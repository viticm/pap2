////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : UiWndNewScrollbarCommon.cpp
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2006-1-11 11:43:41
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "Stdafx.h"
#include "UiWndNewScrollbarCommon.h"
#include "UiEditor.h"
#include "resource.h"
#include "Ui.h"
#include "ClientManage.h"
#include "UiFontManage.h"

////////////////////////////////////////////////////////////////////////////////


KUiWndNewScrollBarCommon::KUiWndNewScrollBarCommon()
{
    
    m_szName[0] = 0;
}

KUiWndNewScrollBarCommon::~KUiWndNewScrollBarCommon()
{
	if (m_hWnd)
	{
		::DestroyWindow(m_hWnd);
		m_hWnd = NULL;
	}
}

KUiWndWindow* KUiWndNewScrollBarCommon::Create()
{
	KUiWndNewScrollBarCommon* pWnd = new KUiWndNewScrollBarCommon;

	return pWnd;
}

int KUiWndNewScrollBarCommon::Init(HWND hWnd, RECT* pRc, const char *pszName)
{
    int nResult = false;

    KG_PROCESS_ERROR(hWnd);
    KG_PROCESS_ERROR(pszName);
    KG_PROCESS_ERROR(pRc);

    strcpy(m_szName, pszName);
	m_hWnd = ::CreateDialog((HINSTANCE)g_hPluginInstance, MAKEINTRESOURCE(IDD_WndNewScrollbar), hWnd, (DLGPROC)WndProc);
    KG_PROCESS_ERROR(m_hWnd);
	::SetWindowPos(m_hWnd, HWND_TOP, pRc->left, pRc->top,
		pRc->right - pRc->left, pRc->bottom - pRc->top,
		SWP_SHOWWINDOW);
	::SetWindowLong(m_hWnd, GWL_USERDATA, (LONG)(KUiWndWindow*)this);

    nResult = true;
Exit0:
    return nResult;
}


void KUiWndNewScrollBarCommon::Release()
{
	delete this;
}

LRESULT CALLBACK KUiWndNewScrollBarCommon::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//KUiWndWindow::WndProc(hWnd, message, wParam, lParam);
	KUiWndNewScrollBarCommon*	pView = (KUiWndNewScrollBarCommon*)GetWindowLong(hWnd, GWL_USERDATA);
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

void KUiWndNewScrollBarCommon::OnButtonClick(int nBtnId)
{
    int nValue = 0;
    int nUpdataShower = true;
	switch(nBtnId)
	{
	case IDC_WND_NEWSCROLLBAR_COMMON_TYPE_H:
        g_Ui.GetIni()->WriteInteger(m_szName, "Type", 0);
		break;
	case IDC_WND_NEWSCROLLBAR_COMMON_TYPE_V:
        g_Ui.GetIni()->WriteInteger(m_szName, "Type", 1);
		break;
	case IDC_WND_NEWSCROLLBAR_COMMON_AUTO_HIDE_SLIDE_BUTTON:
        nValue = ::IsDlgButtonChecked(m_hWnd, IDC_WND_NEWSCROLLBAR_COMMON_AUTO_HIDE_SLIDE_BUTTON) == BST_CHECKED;
        g_Ui.GetIni()->WriteInteger(m_szName, "AutoHideSlideButton", nValue);
		break;
	case IDC_WND_NEWSCROLLBAR_COMMON_AUTO_SIZE_SLIDE_BUTTON:
        nValue = ::IsDlgButtonChecked(m_hWnd, IDC_WND_NEWSCROLLBAR_COMMON_AUTO_SIZE_SLIDE_BUTTON) == BST_CHECKED;
        g_Ui.GetIni()->WriteInteger(m_szName, "AutoResizeSlideButton", nValue);
		break;
    case IDC_WND_NEWSCROLLBAR_SLIDBUTTON_NAME_SEL:
        OnSelectWndName();
        break;
    default:
        nUpdataShower = false;
        break;
	}
	if (nUpdataShower)
	{
		g_ClientManager.UpDataWndInClient(g_Ui.GetIni(true));
        ShowErrorInformation();
	}
}

void KUiWndNewScrollBarCommon::OnSelectWndName()
{
    POINT MousePosition;
    int   nId;
    int   i;
    HMENU hMenu;
    char szWndName[128];
    szWndName[0] = 0;
    IIniFile *pKIniFile = g_Ui.GetIni(true);
    KG_PROCESS_ERROR(pKIniFile);
    
    nId =81000;
	hMenu = CreatePopupMenu();
    KG_PROCESS_ERROR(hMenu);
	while(g_Ui.GetNextUnit(szWndName))
	{
        char szWndType[128];
        pKIniFile->GetString(szWndName, "._WndType", "", szWndType, sizeof(szWndType));
        if (!strcmp(szWndType, "WndButton")) 
        {
            AppendMenu(hMenu, MF_ENABLED | MF_STRING, nId, szWndName);
        }
        else
        {
            AppendMenu(hMenu, MF_DISABLED | MF_GRAYED | MF_STRING, nId, szWndName);
        }
        nId++;
	}
    GetCursorPos(&MousePosition);
    int nSel = TrackPopupMenu(hMenu, TPM_RETURNCMD | TPM_LEFTBUTTON | TPM_NONOTIFY, 
        MousePosition.x, MousePosition.y, 0, m_hWnd, NULL);
	DestroyMenu(hMenu);
    szWndName[0] = 0;
    for (i = 81000; i <= nSel; i++) 
    {
        g_Ui.GetNextUnit(szWndName);
    }

    ::SetDlgItemText(m_hWnd, IDC_WND_NEWSCROLLBAR_SLIDBUTTON_NAME, szWndName);
    pKIniFile->WriteString(m_szName, "SlideBtn", szWndName);

	g_ClientManager.UpDataWndInClient(g_Ui.GetIni(true));
	ShowErrorInformation();
Exit0:
    return;
}

void KUiWndNewScrollBarCommon::OnUpdateEditText(int nId)
{
    int nValue = 0;
    int nUpdataShower = true;
    switch(nId)
    {
    case IDC_WND_NEWSCROLLBAR_COMMON_TOTAL_COUNT:
        nValue = ::GetDlgItemInt(m_hWnd, IDC_WND_NEWSCROLLBAR_COMMON_TOTAL_COUNT, NULL, FALSE);
        g_Ui.GetIni()->WriteInteger(m_szName, "StepCount", nValue);
        break;
    case IDC_WND_NEWSCROLLBAR_COMMON_PAGE_COUNT:
        nValue = ::GetDlgItemInt(m_hWnd, IDC_WND_NEWSCROLLBAR_COMMON_PAGE_COUNT, NULL, FALSE);
        g_Ui.GetIni()->WriteInteger(m_szName, "PageStepCount", nValue);
        break;
    default:
        nUpdataShower = false;
        break;
    }
	if (nUpdataShower)
	{
		g_ClientManager.UpDataWndInClient(g_Ui.GetIni(true));
        ShowErrorInformation();
	}
}

void KUiWndNewScrollBarCommon::UpdateData(const char* pszName, KUiWndWindowData& data, IIniFile* pIni)
{
	if (!m_hWnd || !pIni || !pszName)
		return;

    strcpy(m_szName, pszName);
	int		nValue;
    char  szValue[128];
    szValue[0] = '\0';

    pIni->GetInteger(pszName, "StepCount", 0, &nValue);
	::SetDlgItemInt(m_hWnd, IDC_WND_NEWSCROLLBAR_COMMON_TOTAL_COUNT, nValue, FALSE);
    pIni->GetInteger(pszName, "PageStepCount", 0, &nValue);
	::SetDlgItemInt(m_hWnd, IDC_WND_NEWSCROLLBAR_COMMON_PAGE_COUNT, nValue, FALSE);

	pIni->GetInteger(pszName, "Type", 0, &nValue);
	if (nValue == 1)
		::CheckDlgButton(m_hWnd, IDC_WND_NEWSCROLLBAR_COMMON_TYPE_V, BST_CHECKED);
	else
		::CheckDlgButton(m_hWnd, IDC_WND_NEWSCROLLBAR_COMMON_TYPE_H, BST_CHECKED);

	pIni->GetInteger(pszName, "AutoHideSlideButton", 0, &nValue);
	::CheckDlgButton(m_hWnd, IDC_WND_NEWSCROLLBAR_COMMON_AUTO_HIDE_SLIDE_BUTTON, nValue ? BST_CHECKED : 0);
	pIni->GetInteger(pszName, "AutoResizeSlideButton", 0, &nValue);
	::CheckDlgButton(m_hWnd, IDC_WND_NEWSCROLLBAR_COMMON_AUTO_SIZE_SLIDE_BUTTON, nValue ? BST_CHECKED : 0);

    pIni->GetString(pszName, "SlideBtn", "", szValue, sizeof(szValue));
    ::SetDlgItemText(m_hWnd, IDC_WND_NEWSCROLLBAR_SLIDBUTTON_NAME, szValue);

}
void KUiWndNewScrollBarCommon::Show(int bShow, int nSubWnd)
{
	if (m_hWnd)
	{
		::ShowWindow(m_hWnd, bShow ? SW_SHOW : SW_HIDE);
	} 
}