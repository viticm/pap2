// KG_D3DMemoryLeakAnalyzerDlg.h : header file
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "KG_DebugProcess.h"


// CKG_D3DMemoryLeakAnalyzerDlg dialog
class CKG_D3DMemoryLeakAnalyzerDlg : public CDialog
{
// Construction
public:
	CKG_D3DMemoryLeakAnalyzerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_KG_D3DMEMORYLEAKANALYZER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lStack);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedButtonBrowse();
    afx_msg void OnBnClickedOk();
    afx_msg void OnBnClickedCancel();
    afx_msg void OnBnClickedEnableMemUseAnalyze();
    afx_msg void OnBnClickedGetMemRegion();
    afx_msg void OnBnClickedGetAllocFunction();
    afx_msg void OnEnChangeEditAppPathName();
    afx_msg void OnTimer(UINT_PTR nIDEvent);

private:
    int _CopyDLlFromVCPath();
    CString _GetDebugAppWorkPath();
    CString _GetSymbolSearchPath();
private:
    static UINT __cdecl _WorkThread(LPVOID pvThis)
    {
        CKG_D3DMemoryLeakAnalyzerDlg *pThis = (CKG_D3DMemoryLeakAnalyzerDlg *)pvThis;
        ASSERT(pThis);
        return pThis->_ThreadFunction();        
    }
    int _ThreadFunction();
private:
    CString m_sDebugAppPathName;
    CString m_sCurProcPath;
    KG_DebugProcess m_Debugger;
    int m_nWorkThreadRunningFlag;
private:
    CEdit m_ctlDebugOutput;
    CEdit m_ctlReportWnd;
    CButton m_ctlEnableMemUseAnalyze;
};
