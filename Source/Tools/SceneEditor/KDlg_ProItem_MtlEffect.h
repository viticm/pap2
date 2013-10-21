#if !defined(AFX_KDLG_PROITEM_MTLEFFECT_H__8924EA4C_73C8_44F2_B641_544B9957372A__INCLUDED_)
#define AFX_KDLG_PROITEM_MTLEFFECT_H__8924EA4C_73C8_44F2_B641_544B9957372A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KDlg_ProItem_MtlEffect.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// KDlg_ProItem_MtlEffect dialog
namespace PropertyBase
{
class KDlg_ProItem_MtlEffect : public CDialog
{ 
// Construction
public:
	//KModel* m_lpModel;
	//PropertyBase::_VarProperty* m_lpProperty;
	//void Update(BOOL Dir);

	//void SetStore(PropertyBase::_VarProperty* pProperty)
	//{
	//	m_lpProperty = pProperty;
	//	m_lpModel = (LPMODEL)(m_lpProperty->Address);
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
	KDlg_ProItem_MtlEffect(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(KDlg_ProItem_MtlEffect)
	enum { IDD = IDD_PROITEM_MTLEFFECT };
	CComboBox	m_Tec;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(KDlg_ProItem_MtlEffect)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(KDlg_ProItem_MtlEffect)
	afx_msg void OnBload();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeCombo1();
	afx_msg void OnBapply();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KDLG_PROITEM_MTLEFFECT_H__8924EA4C_73C8_44F2_B641_544B9957372A__INCLUDED_)
