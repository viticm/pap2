#pragma once

#include "resource.h"
// KLogicInfoRenameDlg dialog

class KLogicInfoRenameDlg : public CDialog
{
	DECLARE_DYNAMIC(KLogicInfoRenameDlg)

public:
	KLogicInfoRenameDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~KLogicInfoRenameDlg();
	CString m_strEditText;
// Dialog Data
	enum { IDD = IDD_LOGICINFORENAME };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CEdit	m_Edit;
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog(void);
	afx_msg void OnBnClickedButtonCancel();
	afx_msg void OnBnClickedButtonOK();
};
