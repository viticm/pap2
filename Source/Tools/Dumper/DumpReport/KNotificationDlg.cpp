#include "StdAfx.h"
#include <shellapi.h>
#include "KNotificationDlg.h"

KNotificationDlg *KNotificationDlg::ms_pMyself = NULL;

KNotificationDlg::KNotificationDlg(void)
{   
    m_hwndCheckStatic = NULL;
    m_wndOldStaticProc = NULL;	
    m_hBrushStatic = NULL;
    m_hFontStatic = NULL;    
    m_szReportFilePath[0] = _T('\0');
    m_szFeedbackInfo[0]   = _T('\0');
    m_nOprType = otSend;

    m_nRestartFlag = FALSE;
}

KNotificationDlg::~KNotificationDlg(void)
{
}

int KNotificationDlg::Notify(const TCHAR szReportFilePath[], OPERATION_TYPE *pOprType)
{
    ASSERT(szReportFilePath);
    ASSERT(pOprType);
    KNotificationDlg::ms_pMyself = this;

    strcpy(m_szReportFilePath, szReportFilePath);
    DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgWndProcShell);

    *pOprType = m_nOprType;

    return TRUE;
}

INT_PTR CALLBACK KNotificationDlg::DlgWndProcShell(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
    return ms_pMyself->DlgWndProc(hWnd, uMessage, wParam, lParam);
}

INT_PTR KNotificationDlg::DlgWndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
{	
	switch (uMessage)
    {
    case WM_INITDIALOG:
        OnInitDialog(hWnd);
        return (INT_PTR)TRUE;
    case WM_COMMAND:		
        switch (LOWORD(wParam))
        {
        case IDOK:
            m_nOprType = otSend;
			m_nRestartFlag = GetRestartFlag(hWnd);
            OnSend(hWnd);
            UnInitDialog();
            EndDialog(hWnd, LOWORD(wParam));
            return (INT_PTR)TRUE;
        case IDCANCEL:
            m_nOprType = otQuit;
			m_nRestartFlag = GetRestartFlag(hWnd);
            UnInitDialog();
            EndDialog(hWnd, LOWORD(wParam));
            return (INT_PTR)TRUE;
        case IDDEBUG:
            m_nOprType = otDebug;
            UnInitDialog();
            EndDialog(hWnd, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;        
    case WM_CTLCOLORSTATIC:  
        if (m_hwndCheckStatic == (HWND)lParam)
        {
            SetTextColor((HDC)wParam, RGB(0, 0, 255));
            SetBkColor((HDC)wParam, GetSysColor(COLOR_3DFACE));
            return (INT_PTR)m_hBrushStatic;
        }
        break;
    }	
    return (INT_PTR)FALSE;
}

LRESULT CALLBACK KNotificationDlg::StaticWndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
{   
    HINSTANCE hRetCode;
	switch (uMessage)
    {
    case WM_LBUTTONDOWN:
        hRetCode = ShellExecute(NULL, _T("open"), ms_pMyself->m_szReportFilePath, NULL, NULL, SW_SHOWNORMAL);
        if (hRetCode <= (HINSTANCE)32)
            MessageBox(NULL, STR_CHECK_RPT_ERROR, STR_ERROR, MB_ICONEXCLAMATION | MB_OK);
        break;
	case WM_SETCURSOR:
		SetCursor(LoadCursor(NULL, IDC_HAND));
		ShowCursor(TRUE);
		return 1;
    }

	return CallWindowProc(ms_pMyself->m_wndOldStaticProc, hWnd, uMessage, wParam, lParam);
}

int KNotificationDlg::OnInitDialog(HWND hWnd)
{
    int nRetCode = FALSE;
    int nResult  = FALSE;
    LOGFONT LogFont;

    HWND hwndRestartCheck = NULL;
    m_hwndCheckStatic = GetDlgItem(hWnd, IDC_CHECK_STATIC);
    KG_PROCESS_ERROR(m_hwndCheckStatic);    
	m_wndOldStaticProc = (WNDPROC)SetWindowLong(m_hwndCheckStatic, GWL_WNDPROC, (long)StaticWndProc);
    KG_PROCESS_ERROR(m_wndOldStaticProc);

    m_hBrushStatic = CreateSolidBrush(GetSysColor(COLOR_3DFACE));
    KG_PROCESS_ERROR(m_hBrushStatic);

    m_hFontStatic = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
    KG_PROCESS_ERROR(m_hFontStatic);
    GetObject(m_hFontStatic, sizeof(LOGFONT), &LogFont);
    LogFont.lfUnderline = 1;
    m_hFontStatic = CreateFontIndirect(&LogFont);
    KG_PROCESS_ERROR(m_hFontStatic);
    SendMessage(m_hwndCheckStatic, WM_SETFONT, (WPARAM)m_hFontStatic, 0);

    //hwndRestartCheck = GetDlgItem(hWnd, IDC_CHECK1);
    //KG_PROCESS_ERROR(hwndRestartCheck);
    //SendMessage(hwndRestartCheck, BM_SETCHECK, (WPARAM)BST_UNCHECKED, 0);

    nResult = TRUE;
Exit0:
    return nResult;
}

int KNotificationDlg::UnInitDialog()
{
    DeleteObject(m_hBrushStatic);
    DeleteObject(m_hFontStatic);
    return TRUE;
}

int KNotificationDlg::OnSend(HWND hWnd)
{
	HWND hEditWnd = GetDlgItem(hWnd, IDC_EDIT1);
	KG_PROCESS_ERROR(hEditWnd);
	
	GetWindowText(hEditWnd, m_szFeedbackInfo, sizeof(m_szFeedbackInfo) / sizeof(TCHAR));
    m_szFeedbackInfo[sizeof(m_szFeedbackInfo) / sizeof(TCHAR) - 1] = _T('\0');

Exit0:
    return TRUE;
}

int KNotificationDlg::GetRestartFlag(HWND hWnd)
{    
    HWND hWndCheck = GetDlgItem(hWnd, IDC_CHECK1);
    if (hWndCheck)
		return SendMessage(hWndCheck, BM_GETCHECK, 0, 0);
	return FALSE;
}
