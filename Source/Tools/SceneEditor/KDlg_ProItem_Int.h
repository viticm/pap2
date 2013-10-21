#if !defined(AFX_KDLG_PROITEM_INT_H__59F7705C_78A8_4139_A8E1_F1E25739BF9E__INCLUDED_)
#define AFX_KDLG_PROITEM_INT_H__59F7705C_78A8_4139_A8E1_F1E25739BF9E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KDlg_ProItem_Int.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// KDlg_ProItem_Int dialog
namespace PropertyBase
{
class KDlg_ProItem_Int : public CDialog
{
// Construction
public:
	//PropertyBase::_VarProperty* m_lpProperty;
	//PropertyBase::_VarProperty::_StoreInt* m_lpStoreInt;


	//void SetStore(PropertyBase::_VarProperty* pProperty,
	//	PropertyBase::_VarProperty::_StoreInt* pStoreInt)
	//{
	//	m_lpProperty = pProperty;
	//	m_lpStoreInt = pStoreInt;
	//}
	//void RefreshData()
	//{
	//	if(m_lpProperty)
	//	{
	//		m_lpStoreInt->Value = m_Int;
	//		int* pInt = (int*) m_lpProperty->Address;
	//		(*pInt) = m_Int;
	//	}
	//}
	KDlg_ProItem_Int(CWnd* pParent = NULL);   // standard constructor
// Dialog Data
	//{{AFX_DATA(KDlg_ProItem_Int)
	enum { IDD = IDD_PROITEM_INT };
	CEdit	m_Edit;
	CSpinButtonCtrl	m_Spin;
	int		m_Int;
	CString	m_Name;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(KDlg_ProItem_Int)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(KDlg_ProItem_Int)
	virtual BOOL OnInitDialog();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnChangeEdit1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
}
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KDLG_PROITEM_INT_H__59F7705C_78A8_4139_A8E1_F1E25739BF9E__INCLUDED_)
