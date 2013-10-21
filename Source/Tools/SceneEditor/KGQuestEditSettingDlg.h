#pragma once

#include "KGPropertyEditTabDlg.h"

class KGQuestEditSettingDlg : public CDialog
{
	DECLARE_DYNAMIC(KGQuestEditSettingDlg)

public:
	KGQuestEditSettingDlg(CWnd* pParent);   // standard constructor
	virtual ~KGQuestEditSettingDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_QUEST_EDIT_SET };
public:
	KGObjectPropertyEditTabDlg* m_pMainFrame;
	CButton		m_bnPreSerialSwitch;
	CButton	    m_bnAftRowNumSwtich;
	CButton		m_bnMarkNameSwitch;
	CButton		m_bnAppendWordSwitch;

	CComboBox	m_comboxMark1;
	CComboBox	m_comboxMark2;
	CComboBox	m_comboxMark3;
	CComboBox	m_comboxMark4;

public:
	virtual BOOL OnInitDialog();

	CString GetPlateName(CString szString);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};
