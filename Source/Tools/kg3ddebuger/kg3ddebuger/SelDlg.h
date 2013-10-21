#pragma once
#include "afxwin.h"


// CSelDlg dialog

class CSelDlg : public CDialog
{
	DECLARE_DYNAMIC(CSelDlg)

public:
	CSelDlg(vector<tstring> procName, CWnd* pParent = NULL);   // standard constructor
	virtual ~CSelDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG1 };

    vector<tstring> m_ProcName;

    BOOL OnInitDialog();

    int m_SelProc;

    void OnOK();
    void OnCancel();

    int GetSelProc() { return m_SelProc; }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
    CListBox m_listProcess;
    afx_msg void OnBnClickedOk();
    afx_msg void OnLbnSelchangeList();
    afx_msg void OnLbnDblclkList();
};
