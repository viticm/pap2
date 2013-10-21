#if !defined(AFX_KDLG_PROITEM_MTLSTATE_H__98F0443F_5F28_421D_9405_DF7AAB35F0D5__INCLUDED_)
#define AFX_KDLG_PROITEM_MTLSTATE_H__98F0443F_5F28_421D_9405_DF7AAB35F0D5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KDlg_ProItem_MtlState.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// KDlg_ProItem_MtlState dialog
namespace PropertyBase
{
class KDlg_ProItem_MtlState : public CDialog
{ 
// Construction
public:
	//KModel* m_lpModel;
	//PropertyBase::_VarProperty* m_lpProperty;
	void Update(BOOL Dir);

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

	KDlg_ProItem_MtlState(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(KDlg_ProItem_MtlState)
	enum { IDD = IDD_PROITEM_MTLSTATE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(KDlg_ProItem_MtlState)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(KDlg_ProItem_MtlState)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
}
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KDLG_PROITEM_MTLSTATE_H__98F0443F_5F28_421D_9405_DF7AAB35F0D5__INCLUDED_)
