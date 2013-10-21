///////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
//
// FileName : KNotificationDlg.h
// Creator  : chengzhaohui
// Date     : 
// Comment  :
//
///////////////////////////////////////////////////////////////

#ifndef _KNOTIFICATIONDLG_H_
#define _KNOTIFICATIONDLG_H_

#include "resource.h"
#include <windows.h>

#define STR_ERROR           _T("错误")
#define STR_CHECK_RPT_ERROR _T("查看错误报告出错!")

extern HINSTANCE g_hInst;

enum OPERATION_TYPE
{
    otSend,
    otQuit,
    otDebug
};

class KNotificationDlg
{
public:
    KNotificationDlg(void);
    virtual ~KNotificationDlg(void);

public:
    int Notify(const TCHAR szReportFilePath[], OPERATION_TYPE *pOprType);
    TCHAR *GetFeedbackInfo(){return m_szFeedbackInfo;}
    int GetRestartFlag(){return m_nRestartFlag;}

private:
    static INT_PTR CALLBACK DlgWndProcShell(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);
	INT_PTR DlgWndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);

	static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);    
    
private:
    int OnInitDialog(HWND hWnd);
    int UnInitDialog();
    int OnSend(HWND hWnd);
    int GetRestartFlag(HWND hWnd);

private:
    static KNotificationDlg *ms_pMyself;

    HWND m_hwndCheckStatic;
	WNDPROC m_wndOldStaticProc;	

    HBRUSH m_hBrushStatic;
    HFONT m_hFontStatic;

    TCHAR m_szReportFilePath[MAX_PATH];

    TCHAR m_szFeedbackInfo[512];

    OPERATION_TYPE m_nOprType;

    int m_nRestartFlag;
};

#endif //_KNOTIFICATIONDLG_H_