#if !defined(AFX_KDLG_PROITEM_ENUM_H__F2E923A4_D9D8_4852_B2AD_B89B1109A001__INCLUDED_)
#define AFX_KDLG_PROITEM_ENUM_H__F2E923A4_D9D8_4852_B2AD_B89B1109A001__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KDlg_ProItem_Enum.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// KDlg_ProItem_Enum dialog
namespace PropertyBase
{
class KDlg_ProItem_Enum : public CDialog
{ 
// Construction
public:
	//PropertyBase::_VarProperty* m_lpProperty;
	//PropertyBase::_VarProperty::_StoreEnum* m_lpStore;


	/*void SetStore(PropertyBase::_VarProperty* pProperty,
		PropertyBase::_VarProperty::_StoreEnum* pStore)
	{
		m_lpProperty = pProperty;
		m_lpStore = pStore;
	}

	void RefreshData()
	{
		if(m_lpProperty)
		{
			int Value = m_Combo.GetItemData(m_Value);
			int* pInt = (int*) m_lpProperty->Address;
			(*pInt) = Value;
		}
	}*/

	KDlg_ProItem_Enum(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(KDlg_ProItem_Enum)
	enum { IDD = IDD_PROITEM_ENUM };
	CComboBox	m_Combo;
	int		m_Value;
	CString	m_Name;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(KDlg_ProItem_Enum)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(KDlg_ProItem_Enum)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeCombo1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
}
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KDLG_PROITEM_ENUM_H__F2E923A4_D9D8_4852_B2AD_B89B1109A001__INCLUDED_)
