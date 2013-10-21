#if !defined(AFX_KDLG_PROPERTYDIALOG_H__B25E97BC_B984_4609_A435_886794522AFF__INCLUDED_)
#define AFX_KDLG_PROPERTYDIALOG_H__B25E97BC_B984_4609_A435_886794522AFF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KDlg_PropertyDialog.h : header file
//
#include "./KDlg_PropertyPage.h"

/////////////////////////////////////////////////////////////////////////////
// KDlg_PropertyDialog dialog
namespace PropertyBase
{

class KDlg_PropertyDialog : public CDialog
{ 
// Construction
public:
	BOOL m_bShow;
	BOOL ShowWindow (int nCmdShow);

	HRESULT CreateUI();
	HRESULT Clear();
	PropertyBase::KPropertyBase*  m_lpPropertyBase;

	vector<KDlg_PropertyPage*>m_vecPage;

	void SetPropertyBase(PropertyBase::KPropertyBase* pProperty);

	KDlg_PropertyDialog(CWnd* pParent = NULL);   // standard constructor

	virtual ~KDlg_PropertyDialog();
// Dialog Data
	//{{AFX_DATA(KDlg_PropertyDialog)
	enum { IDD = IDD_PRODLG };
	CTabCtrl	m_Tab;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(KDlg_PropertyDialog)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void RecalcLayout();
	int AddPage(KDlg_PropertyPage *pPage, CRect *pRect);

	// Generated message map functions
	//{{AFX_MSG(KDlg_PropertyDialog)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnPaint();
	afx_msg void OnClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KDLG_PROPERTYDIALOG_H__B25E97BC_B984_4609_A435_886794522AFF__INCLUDED_)
