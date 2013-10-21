#if !defined(AFX_KDLG_PROITEM_FLOAT_H__E5E973A6_722F_4AF9_ACDE_2A7BE5EB6ACE__INCLUDED_)
#define AFX_KDLG_PROITEM_FLOAT_H__E5E973A6_722F_4AF9_ACDE_2A7BE5EB6ACE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KDlg_ProItem_Float.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// KDlg_ProItem_Float dialog
namespace PropertyBase
{
class KDlg_ProItem_Float : public CDialog
{  
// Construction
public:
	//PropertyBase::_VarProperty* m_lpProperty;
	//PropertyBase::_VarProperty::_StoreFloat* m_lpStore;


	//void SetStore(PropertyBase::_VarProperty* pProperty,
	//	PropertyBase::_VarProperty::_StoreFloat* pStore)
	//{
	//	m_lpProperty = pProperty;
	//	m_lpStore = pStore;
	//}

	//void RefreshData()
	//{
	//	if(m_lpProperty)
	//	{
	//		m_lpStore->Value = m_Value;
	//		float* pFloat = (float*) m_lpProperty->Address;
	//		(*pFloat) = m_Value;
	//	}
	//}

	KDlg_ProItem_Float(CWnd* pParent = NULL);   // standard constructor

	int TotalStep;
// Dialog Data
	//{{AFX_DATA(KDlg_ProItem_Float)
	enum { IDD = IDD_PROITEM_FLOAT };
	CEdit	m_Edit;
	CSpinButtonCtrl	m_Spin;
	float	m_Value;
	CString	m_Name;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(KDlg_ProItem_Float)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(KDlg_ProItem_Float)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeEdit1();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
}
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KDLG_PROITEM_FLOAT_H__E5E973A6_722F_4AF9_ACDE_2A7BE5EB6ACE__INCLUDED_)
