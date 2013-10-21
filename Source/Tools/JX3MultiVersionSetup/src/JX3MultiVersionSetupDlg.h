// JX3NewVersionSetupDlg.h : header file
//

#pragma once


// CJX3NewVersionSetupDlg dialog
class CJX3NewVersionSetupDlg : public CDialog
{
// Construction
public:
	CJX3NewVersionSetupDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_JX3NEWVERSIONSETUP_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
};
