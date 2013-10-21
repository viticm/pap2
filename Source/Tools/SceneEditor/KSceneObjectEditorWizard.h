#if !defined(AFX_KSCENEOBJECTEDITORWIZARD_H__DA85DA08_67CE_4157_8C4E_3AD7E94A3F1C__INCLUDED_)
#define AFX_KSCENEOBJECTEDITORWIZARD_H__DA85DA08_67CE_4157_8C4E_3AD7E94A3F1C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KSceneObjectEditorWizard.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// KSceneObjectEditorWizard dialog

class KSceneObjectEditorWizard : public CDialog
{
// Construction
public:
	KSceneObjectEditorWizard(CWnd* pParent = NULL);   // standard constructor

	void UnCheckAllTypeCheck();
// Dialog Data
	//{{AFX_DATA(KSceneObjectEditorWizard)
	enum { IDD = IDD_OBJECTEDITOR_WIZARD };
	BOOL	m_bBrush;
	BOOL	m_bBuilding;
	BOOL	m_bHorRotate;
	BOOL	m_bPitchOnly;
	BOOL	m_ToCrid;
	BOOL	m_bGrass;
	BOOL	m_bItem;
	BOOL	m_bNPC;
	BOOL	m_bOranment;
	BOOL	m_bOranmentBrakable;
	BOOL	m_bPlayer;
	BOOL	m_bTree;
	BOOL	m_bWall;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(KSceneObjectEditorWizard)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(KSceneObjectEditorWizard)
	afx_msg void OnBuliding();
	afx_msg void OnOranment();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KSCENEOBJECTEDITORWIZARD_H__DA85DA08_67CE_4157_8C4E_3AD7E94A3F1C__INCLUDED_)
