// EffectCompilerDlg.h : header file
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"

#include "../GridCtrl_src/GridCtrl.h"

// CEffectCompilerDlg dialog
class CEffectCompilerDlg : public CDialog
{
// Construction
public:
	CEffectCompilerDlg(CWnd* pParent = NULL);	// standard constructor
	~CEffectCompilerDlg();

// Dialog Data
	enum { IDD = IDD_EFFECTCOMPILER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
protected:
	HICON m_hIcon;
	

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	
	CGridCtrl	m_GridCtrl;
	CEdit m_EditContent;
	CEdit m_EditStatistic;
	afx_msg void OnBnClickedCheckForceCompile();
	afx_msg void OnBnClickedCheckShowOnlyError();
	afx_msg void OnGridSelectChange(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonBrowse();
	CEdit m_EditDirectory;
};
