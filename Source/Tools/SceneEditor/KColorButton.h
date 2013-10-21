#if !defined(AFX_KCOLORBUTTON_H__DC2CF8FE_596E_4A47_9E88_3B48341BF172__INCLUDED_)
#define AFX_KCOLORBUTTON_H__DC2CF8FE_596E_4A47_9E88_3B48341BF172__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KColorButton.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// KColorButton window

class KColorButton : public CButton
{
// Construction
public:
	DWORD m_dwColor;
	
	KColorButton();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(KColorButton)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~KColorButton();

	// Generated message map functions
protected:
	//{{AFX_MSG(KColorButton)
	afx_msg void OnPaint();
	//}}AFX_MSG
	//afx_msg void OnClicked();

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KCOLORBUTTON_H__DC2CF8FE_596E_4A47_9E88_3B48341BF172__INCLUDED_)
