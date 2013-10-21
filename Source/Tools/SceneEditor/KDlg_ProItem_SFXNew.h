#if !defined(AFX_KDLG_PROITEM_SFXNEW_H__BDF7E17C_31CF_4A76_BCC1_3E9D05C121D4__INCLUDED_)
#define AFX_KDLG_PROITEM_SFXNEW_H__BDF7E17C_31CF_4A76_BCC1_3E9D05C121D4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KDlg_ProItem_SFXNew.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// KDlg_ProItem_SFXNew dialog
namespace PropertyBase
{
class KDlg_ProItem_SFXNew : public CDialog
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
	//		/*if(m_lpProperty->Type==PropertyBase::_VarProperty::VAR_STRING)
	//		{
	//		LPSTR pStr = (LPSTR) m_lpProperty->Address;
	//		wsprintf(pStr,"%s",m_Value);
	//		}
	//		else if(m_lpProperty->Type==PropertyBase::_VarProperty::VAR_STLSTRING)
	//		{
	//		string* pStr = (string*) m_lpProperty->Address;
	//		(*pStr) = m_Value;
	//		}*/
	//	}
	//}

	KDlg_ProItem_SFXNew(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(KDlg_ProItem_SFXNew)
	enum { IDD = IDD_PROITEM_SFXNEW };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(KDlg_ProItem_SFXNew)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(KDlg_ProItem_SFXNew)
	afx_msg void OnBbill();
	afx_msg void OnBblade();
	afx_msg void OnBpart();
	afx_msg void OnBbelt();
	afx_msg void OnBLVF();  // 体积雾
	afx_msg void OnBDL();  // 动态闪电
	afx_msg void OnBST();  // SpeedTree
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
}
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KDLG_PROITEM_SFXNEW_H__BDF7E17C_31CF_4A76_BCC1_3E9D05C121D4__INCLUDED_)
