#if !defined(AFX_KSCENEMODELEDITORMODELSAVEDIALOG_H__0CBB6F85_6B2E_4FFB_9692_0CE523F9CA46__INCLUDED_)
#define AFX_KSCENEMODELEDITORMODELSAVEDIALOG_H__0CBB6F85_6B2E_4FFB_9692_0CE523F9CA46__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KSceneModelEditorModelSaveDialog.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// KSceneModelEditorModelSaveDialog dialog
interface IEKG3DModel;
class KSceneModelEditorModelSaveDialog : public CDialog
{
// Construction
public:
	KSceneModelEditorModelSaveDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(KSceneModelEditorModelSaveDialog)
	enum { IDD = IDD_MODELEDITOR_SAVEMODEL };
	BOOL	m_bSaveAni;
	BOOL	m_bSaveMtl;
	BOOL	m_bSaveMesh;
	CString	m_CString_Ani;
	CString	m_CString_Mtl;
	CString	m_CString_Mesh;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(KSceneModelEditorModelSaveDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetModel(IEKG3DModel* pModle)
	{
		m_lpModel = pModle;
	}
protected:
	IEKG3DModel* m_lpModel;


	// Generated message map functions
	//{{AFX_MSG(KSceneModelEditorModelSaveDialog)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCheckMesh();
	afx_msg void OnBnClickedCheckMatrial();
	afx_msg void OnBnClickedCheckAnimation();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KSCENEMODELEDITORMODELSAVEDIALOG_H__0CBB6F85_6B2E_4FFB_9692_0CE523F9CA46__INCLUDED_)
