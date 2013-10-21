#if !defined(AFX_KDLG_PROITEM_MTLLIST_H__CE958C02_6F98_4D15_9906_0FF54A4437AF__INCLUDED_)
#define AFX_KDLG_PROITEM_MTLLIST_H__CE958C02_6F98_4D15_9906_0FF54A4437AF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KDlg_ProItem_MtlList.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// KDlg_ProItem_MtlList dialog
namespace PropertyBase
{
class KDlg_ProItem_MtlList : public CDialog
{ 
// Construction
public:
	HRESULT ReCreateEffect();
	HRESULT FillList();

	//KDlg_PropertyPage* m_lpPropertyPage;
	//KModel* m_lpModel;
	//PropertyBase::_VarProperty* m_lpProperty;

	//void SetStore(PropertyBase::_VarProperty* pProperty,KDlg_PropertyPage* lpPropertyPage)
	//{
	//	m_lpProperty = pProperty;
	//	m_lpPropertyPage = lpPropertyPage;
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

	KDlg_ProItem_MtlList(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(KDlg_ProItem_MtlList)
	enum { IDD = IDD_PROITEM_MTLLIST };
	CListCtrl	m_List;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(KDlg_ProItem_MtlList)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	int         m_nTest;
	CImageList  m_imgList;

	// Generated message map functions
	//{{AFX_MSG(KDlg_ProItem_MtlList)
	afx_msg void OnBselectall();
	afx_msg void OnBselectinvert();
	afx_msg void OnBselectnone();
	afx_msg void OnBunit();
	afx_msg void OnBmovebutton();
	afx_msg void OnBmovetop();
	virtual BOOL OnInitDialog();
	afx_msg void OnClickList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBPRT();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
}
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KDLG_PROITEM_MTLLIST_H__CE958C02_6F98_4D15_9906_0FF54A4437AF__INCLUDED_)
