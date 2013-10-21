#if !defined(AFX_KDLG_PROITEM_STRING_H__08756332_AC8E_4647_A3D5_6FAF7F61FF46__INCLUDED_)
#define AFX_KDLG_PROITEM_STRING_H__08756332_AC8E_4647_A3D5_6FAF7F61FF46__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KDlg_ProItem_String.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// KDlg_ProItem_String dialog
namespace PropertyBase
{
class KDlg_ProItem_String : public CDialog
{ 
// Construction
public:
	//PropertyBase::_VarProperty* m_lpProperty;

	//void SetStore(PropertyBase::_VarProperty* pProperty)
	//{
	//	m_lpProperty = pProperty;
	//}

	//void RefreshData()
	//{
	//	if(m_lpProperty)
	//	{
	//		//m_lpStore->pStr = m_Value;
	//		if(m_lpProperty->Type==PropertyBase::_VarProperty::VAR_STRING)
	//		{
	//			LPSTR pStr = (LPSTR) m_lpProperty->Address;
	//			wsprintf(pStr,"%s",m_Value);
	//		}
	//		else if(m_lpProperty->Type==PropertyBase::_VarProperty::VAR_STLSTRING)
	//		{
	//			string* pStr = (string*) m_lpProperty->Address;
	//			(*pStr) = m_Value;
	//		}
	//	}
	//}

	KDlg_ProItem_String(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(KDlg_ProItem_String)
	enum { IDD = IDD_PROITEM_STRING };
	CString	m_Value;
	CString	m_Name;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(KDlg_ProItem_String)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(KDlg_ProItem_String)
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	afx_msg void OnChangeEdit1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
}
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KDLG_PROITEM_STRING_H__08756332_AC8E_4647_A3D5_6FAF7F61FF46__INCLUDED_)
