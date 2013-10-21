#if !defined(AFX_KSCENEMODELEDITORSFXNEWDIALOG_H__4330FA72_6A77_41E6_A470_3352F06686F4__INCLUDED_)
#define AFX_KSCENEMODELEDITORSFXNEWDIALOG_H__4330FA72_6A77_41E6_A470_3352F06686F4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KSceneModelEditorSFXNewDialog.h : header file
//
//class KG3DSceneModelEditor;
class KSceneModelEditorDoc;
interface IEKG3DSceneModelEditor;//by dengzhihui Refactor 2006Äê11ÔÂ24ÈÕ

/////////////////////////////////////////////////////////////////////////////
// KSceneModelEditorSFXNewDialog dialog

class KSceneModelEditorSFXNewDialog : public CDialog
{
// Construction
public:
	KSceneModelEditorSFXNewDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(KSceneModelEditorSFXNewDialog)
	enum { IDD = IDD_MODELEDITOR_SFXNEW };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(KSceneModelEditorSFXNewDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation

protected:

	IEKG3DSceneModelEditor* m_lpSceneModelEditor;
	KSceneModelEditorDoc*  m_lpSceneModelDoc;

	// Generated message map functions
	//{{AFX_MSG(KSceneModelEditorSFXNewDialog)
	afx_msg void OnButtonBillboard();
	afx_msg void OnButtonBelt();
	afx_msg void OnButtonBlade();
	afx_msg void OnButtonFogv();
	afx_msg void OnButtonLight();
	afx_msg void OnButtonParticle();
	afx_msg void OnButtonSpeedtree();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void SetModelEditor(IEKG3DSceneModelEditor* pEditor,KSceneModelEditorDoc* pDoc)
	{
		m_lpSceneModelEditor = pEditor;
		m_lpSceneModelDoc = pDoc;
	}
	afx_msg void OnBnClickedButton8();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KSCENEMODELEDITORSFXNEWDIALOG_H__4330FA72_6A77_41E6_A470_3352F06686F4__INCLUDED_)
