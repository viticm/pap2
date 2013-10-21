#if !defined(AFX_KDLG_PROITEM_SCENEOBJECTS_H__1F4AD552_323A_4854_9D11_AEA237A6D3BB__INCLUDED_)
#define AFX_KDLG_PROITEM_SCENEOBJECTS_H__1F4AD552_323A_4854_9D11_AEA237A6D3BB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KDlg_ProItem_SceneObjects.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// KDlg_ProItem_SceneObjects dialog
namespace PropertyBase
{
class KDlg_ProItem_SceneObjects : public CDialog
{
// Construction
public:
	HRESULT FillTreeWithDir(LPSTR Dir,HTREEITEM hStart);
	HRESULT FillTree();

	//KDlg_PropertyPage* m_lpPropertyPage;
	//PropertyBase::_VarProperty* m_lpProperty;

	//void SetStore(PropertyBase::_VarProperty* pProperty,KDlg_PropertyPage* lpPropertyPage)
	//{
	//	m_lpProperty = pProperty;
	//	m_lpPropertyPage = lpPropertyPage;
	//}

	KDlg_ProItem_SceneObjects(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(KDlg_ProItem_SceneObjects)
	enum { IDD = IDD_PROITEM_SCENEOBJECT };
	CTreeCtrl	m_Tree;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(KDlg_ProItem_SceneObjects)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(KDlg_ProItem_SceneObjects)
	afx_msg void OnButton();
	afx_msg void OnDblclkTree1(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

}
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KDLG_PROITEM_SCENEOBJECTS_H__1F4AD552_323A_4854_9D11_AEA237A6D3BB__INCLUDED_)
