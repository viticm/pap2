#if !defined(AFX_DLG_GROUNDTOOL_H__7324B2E7_24D6_45A7_867D_46D5A58FF221__INCLUDED_)
#define AFX_DLG_GROUNDTOOL_H__7324B2E7_24D6_45A7_867D_46D5A58FF221__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DLG_GroundTool.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDLG_GroundTool dialog
namespace MFCFramework
{

class CDLG_GroundTool : public CDialog
{
// Construction
public:
	CDLG_GroundTool(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDLG_GroundTool)
	enum { IDD = IDD_GROUNDTOOL };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDLG_GroundTool)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDLG_GroundTool)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_GROUNDTOOL_H__7324B2E7_24D6_45A7_867D_46D5A58FF221__INCLUDED_)
