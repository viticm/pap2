#if !defined(AFX_DRIVECOMBO_H__62FBEE4A_DB5C_4267_8A5B_94642534BF87__INCLUDED_)
#define AFX_DRIVECOMBO_H__62FBEE4A_DB5C_4267_8A5B_94642534BF87__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DriveCombo.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDriveCombo window

class CDriveCombo : public CComboBox
{
// Construction
public:
	CDriveCombo();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDriveCombo)
	public:
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	CString GetSelectedDirectory();
	void LoadDrives();
	virtual ~CDriveCombo();

	// Generated message map functions
protected:
	//{{AFX_MSG(CDriveCombo)
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void OnSelendok();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DRIVECOMBO_H__62FBEE4A_DB5C_4267_8A5B_94642534BF87__INCLUDED_)
