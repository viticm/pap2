// kg3ddebugerDlg.h : header file
//

#pragma once
#include "afxcmn.h"


// Ckg3ddebugerDlg dialog
class Ckg3ddebugerDlg : public CDialog
{
// Construction
public:
	Ckg3ddebugerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_KG3DDEBUGER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	
    DECLARE_MESSAGE_MAP()
    afx_msg void OnCheckNotity(UINT id);
public:
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    CSliderCtrl m_SliderZFar;
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    CSpinButtonCtrl m_SpinCpuNum;
    int m_nCpuNum;
    CSliderCtrl m_SlidAngel;
	afx_msg void OnBnClickedCheckTerrain();
};
