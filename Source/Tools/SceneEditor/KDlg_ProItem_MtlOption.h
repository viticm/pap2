#if !defined(AFX_KDLG_PROITEM_MTLOPTION_H__1330D9E5_C8DA_4637_B4FE_4953F4F60E0E__INCLUDED_)
#define AFX_KDLG_PROITEM_MTLOPTION_H__1330D9E5_C8DA_4637_B4FE_4953F4F60E0E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KDlg_ProItem_MtlOption.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// KDlg_ProItem_MtlOption dialog
namespace PropertyBase
{
class KDlg_ProItem_MtlOption : public CDialog
{ 
// Construction
public:
	//KModel::LPMATERIAL m_lpCurMaterial;

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

	KDlg_ProItem_MtlOption(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(KDlg_ProItem_MtlOption)
	enum { IDD = IDD_PROITEM_MTLOPTION };
	BOOL	m_2Face;
	BOOL	m_Wire;
	BOOL	m_Z;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(KDlg_ProItem_MtlOption)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(KDlg_ProItem_MtlOption)
	virtual BOOL OnInitDialog();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnZbuffer();
	afx_msg void OnCwire();
	afx_msg void On2face();
	afx_msg void OnBapply();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KDLG_PROITEM_MTLOPTION_H__1330D9E5_C8DA_4637_B4FE_4953F4F60E0E__INCLUDED_)
