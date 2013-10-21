#if !defined(AFX_KDLG_PROITEM_BOOL_H__A4F7A92B_4334_4D51_8F25_6A51A3B3D180__INCLUDED_)
#define AFX_KDLG_PROITEM_BOOL_H__A4F7A92B_4334_4D51_8F25_6A51A3B3D180__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KDlg_ProItem_Bool.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// KDlg_ProItem_Bool dialog
namespace PropertyBase
{
class KDlg_ProItem_Bool : public CDialog
{ 
// Construction
public:
	//PropertyBase::_VarProperty* m_lpProperty;
	//PropertyBase::_VarProperty::_StoreBool* m_lpStoreBool;


	//void SetStore(PropertyBase::_VarProperty* pProperty,
	//	PropertyBase::_VarProperty::_StoreBool* pStoreBool)
	//{
	//	m_lpProperty = pProperty;
	//	m_lpStoreBool = pStoreBool;
	//}
	//void RefreshData()
	//{
	//	if(m_lpProperty)
	//	{
	//		m_lpStoreBool->bValue = m_Bool;
	//		BOOL* pBool = (BOOL*) m_lpProperty->Address;
	//		(*pBool) = m_Bool;
	//	}
	//}

	KDlg_ProItem_Bool(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(KDlg_ProItem_Bool)
	enum { IDD = IDD_PROITEM_BOOL };
	CButton	m_Check;
	BOOL	m_Bool;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(KDlg_ProItem_Bool)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(KDlg_ProItem_Bool)
	afx_msg void OnCheck1();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KDLG_PROITEM_BOOL_H__A4F7A92B_4334_4D51_8F25_6A51A3B3D180__INCLUDED_)
