#include "stdafx.h"
#include "UiWndWebPageCommon.h"
#include "UiEditor.h"
#include "resource.h"
#include "Ui.h"
#include "ClientManage.h"
#include "UiFontManage.h"


KUiWndWebPageCommon::KUiWndWebPageCommon()
{
    m_szName[0] = '\0';
}

KUiWndWebPageCommon::~KUiWndWebPageCommon()
{
	if (m_hWnd)
	{
		::DestroyWindow(m_hWnd);
		m_hWnd = NULL;
	}
}

KUiWndWindow* KUiWndWebPageCommon::Create()
{
	return new KUiWndWebPageCommon;
}

int KUiWndWebPageCommon::Init(HWND hWnd, RECT* pRc, const char *pszName)
{
    int nResult = false;

    KG_PROCESS_ERROR(hWnd);
    KG_PROCESS_ERROR(pszName);
    KG_PROCESS_ERROR(pRc);

    strcpy(m_szName, pszName);

	m_hWnd = ::CreateDialog((HINSTANCE)g_hPluginInstance, MAKEINTRESOURCE(IDD_WndWebPageCommon), hWnd, (DLGPROC)WndProc);
    KG_PROCESS_ERROR(m_hWnd);

	::SetWindowPos(m_hWnd, HWND_TOP, pRc->left, pRc->top, pRc->right - pRc->left, pRc->bottom - pRc->top, SWP_SHOWWINDOW);

	::SetWindowLong(m_hWnd, GWL_USERDATA, (LONG)(KUiWndWindow*)this);

    nResult = true;
Exit0:
    return nResult;
}


void KUiWndWebPageCommon::Release()
{
	delete this;
}

LRESULT CALLBACK KUiWndWebPageCommon::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	KUiWndWebPageCommon* pView = (KUiWndWebPageCommon*)GetWindowLong(hWnd, GWL_USERDATA);
	if (pView == NULL)
		return 0;

	if (message == WM_COMMAND)
	{
		int nNotify = HIWORD(wParam);
		switch(nNotify)
		{
		case EN_CHANGE:
			pView->OnUpdateEditText(LOWORD(wParam));
			break;
		case BN_CLICKED:
			pView->OnButtonClick(LOWORD(wParam));
			break;
		}
	}

	return 0;
}

void KUiWndWebPageCommon::OnUpdateEditText(int nId)
{
	char szValue[1024];
	
	szValue[0] = '\0';

    switch (nId)
    {
    case IDC_WND_WEB_PAGE_URL:
		EnableWindow(GetDlgItem(m_hWnd, IDC_WND_WEB_PAGE_SAVE), TRUE);
        break;
    default:
        break;
    }
}

void KUiWndWebPageCommon::OnButtonClick(int nId)
{
    BOOL bModified = FALSE;
	char szValue[1024];
	
	szValue[0] = '\0';

	switch (nId)
	{
	case IDC_WND_WEB_PAGE_SAVE:
        ::GetDlgItemText(m_hWnd, IDC_WND_WEB_PAGE_URL, szValue, _countof(szValue));
        g_Ui.GetIni()->WriteString(m_szName, "$URL", szValue);
		EnableWindow(GetDlgItem(m_hWnd, IDC_WND_WEB_PAGE_SAVE), FALSE);
		bModified = TRUE;
		break;
    default:
        break;
	}

	if (bModified)
	{
		g_ClientManager.UpDataWndInClient(g_Ui.GetIni(true));
        ShowErrorInformation();
	}
}

void KUiWndWebPageCommon::UpdateData(const char* pszName, KUiWndWindowData& data, IIniFile* pIni)
{
	char szValue[1024];
	
	szValue[0] = '\0';

	if (!m_hWnd || !pIni || !pszName)
		return;

    strcpy(m_szName, pszName);

	pIni->GetString(pszName, "$URL", "about:blank", szValue, sizeof(szValue));
    ::SetDlgItemText(m_hWnd, IDC_WND_WEB_PAGE_URL, szValue);
}

void KUiWndWebPageCommon::Show(int bShow, int nSubWnd)
{
	if (m_hWnd)
	{
		::ShowWindow(m_hWnd, bShow ? SW_SHOW : SW_HIDE);
	} 
}
