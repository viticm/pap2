#if !defined(AFX_KDLG_PROITEMBASE_H__79B2ECED_3616_4710_9125_FDC0FF153C9A__INCLUDED_)
#define AFX_KDLG_PROITEMBASE_H__79B2ECED_3616_4710_9125_FDC0FF153C9A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KDlg_ProItemBase.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// KDlg_ProItemBase dialog
namespace PropertyBase
{
class KDlg_ProItemBase : public CDialog
{ 
// Construction
public:
	KDlg_ProItemBase(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(KDlg_ProItemBase)
	enum { IDD = IDD_PROITEM };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(KDlg_ProItemBase)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(KDlg_ProItemBase)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
}
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KDLG_PROITEMBASE_H__79B2ECED_3616_4710_9125_FDC0FF153C9A__INCLUDED_)
