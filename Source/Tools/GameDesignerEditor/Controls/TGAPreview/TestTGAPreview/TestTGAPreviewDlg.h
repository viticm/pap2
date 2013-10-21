// TestTGAPreviewDlg.h : header file
//

#pragma once


// CTestTGAPreviewDlg dialog
class CTestTGAPreviewDlg : public CDialog
{
// Construction
public:
	CTestTGAPreviewDlg(CWnd* pParent = NULL);	// standard constructor
	~CTestTGAPreviewDlg();

// Dialog Data
	enum { IDD = IDD_TESTTGAPREVIEW_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnChange();
	afx_msg void OnRemove();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	DECLARE_MESSAGE_MAP()

	virtual void OnOK();
	virtual void OnCancel();

	UINT_PTR m_nTimer, m_nTimer1;
};
