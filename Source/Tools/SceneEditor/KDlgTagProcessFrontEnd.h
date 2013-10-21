#pragma once
#include "afxwin.h"


// KDlgTagProcessFrontEnd dialog

class KDlgTagProcessFrontEnd : public CDialog
{
	DECLARE_DYNAMIC(KDlgTagProcessFrontEnd)

public:
	KDlgTagProcessFrontEnd(CWnd* pParent = NULL);   // standard constructor
	virtual ~KDlgTagProcessFrontEnd();

// Dialog Data
	enum { IDD = IDD_DIALOG_TAGPROCESSFRONTEND };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLbnDblclkListSeldir();
	CString m_strNewDire;
	virtual void OnOK();
	std::set<DWORD> m_setDirs;
	std::vector<CString> m_Paths;
	CListBox m_listDir;
	CEdit m_editDir;
	afx_msg void OnBnClickedButtonOk();
};
