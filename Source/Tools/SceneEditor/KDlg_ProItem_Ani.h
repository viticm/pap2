#if !defined(AFX_KDLG_PROITEM_ANI_H__1E105A29_2070_44AE_B439_177AA24282D8__INCLUDED_)
#define AFX_KDLG_PROITEM_ANI_H__1E105A29_2070_44AE_B439_177AA24282D8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KDlg_ProItem_Ani.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// KDlg_ProItem_Ani dialog
namespace PropertyBase
{
class KDlg_ProItem_Ani : public CDialog
{ 
// Construction
public:
	HRESULT CreateListCum();
	HRESULT FillList();

	//KModel* m_lpModel;
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

	KDlg_ProItem_Ani(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(KDlg_ProItem_Ani)
	enum { IDD = IDD_PROITEM_ANI };
	CSliderCtrl	m_Slider;
	CListCtrl	m_List;
	float	m_FrameLength;
	float	m_TotalTime;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(KDlg_ProItem_Ani)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(KDlg_ProItem_Ani)
	virtual BOOL OnInitDialog();
	afx_msg void OnBopen();
	afx_msg void OnAppAbout();
	afx_msg void OnBplay();
	afx_msg void OnBdelete();
	afx_msg void OnBstop();
	afx_msg void OnRplaycircle();
	afx_msg void OnRplaynormal();
	afx_msg void OnRplayppang();
	afx_msg void OnRplayreverse();
	afx_msg void OnReleasedcaptureSlider1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeEframe();
	afx_msg void OnChangeEtotaltime();
	afx_msg void OnClickList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBsave();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KDLG_PROITEM_ANI_H__1E105A29_2070_44AE_B439_177AA24282D8__INCLUDED_)
