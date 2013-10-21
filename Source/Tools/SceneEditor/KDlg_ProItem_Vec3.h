#if !defined(AFX_KDLG_PROITEM_VEC3_H__2E659838_7BC6_4645_B6DC_AB1FA2C48384__INCLUDED_)
#define AFX_KDLG_PROITEM_VEC3_H__2E659838_7BC6_4645_B6DC_AB1FA2C48384__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KDlg_ProItem_Vec3.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// KDlg_ProItem_Vec3 dialog
namespace PropertyBase
{

class KDlg_ProItem_Vec3 : public CDialog
{
// Construction
public:
	//PropertyBase::_VarProperty* m_lpProperty;

	//void SetStore(PropertyBase::_VarProperty* pProperty)
	//{
	//	m_lpProperty = pProperty;
	//}

	KDlg_ProItem_Vec3(CWnd* pParent = NULL);   // standard constructor
	//
	//void RefreshData()
	//{
	//	if(m_lpProperty)
	//	{
	//		*(D3DXVECTOR3*) m_lpProperty->Address= m_vec3Value;
	//	}
	//}

// Dialog Data
	//{{AFX_DATA(KDlg_ProItem_Vec3)
	enum { IDD = IDD_PROITEM_VEC3 };
		// NOTE: the ClassWizard will add data members here
	CString m_strName;
	D3DXVECTOR3 m_vec3Value;
	CEdit m_ctlEditVec3X;
	CEdit m_ctlEditVec3Y;
	CEdit m_ctlEditVec3Z;
	CSpinButtonCtrl m_ctlSpinVec3X;
	CSpinButtonCtrl m_ctlSpinVec3Y;
	CSpinButtonCtrl m_ctlSpinVec3Z;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(KDlg_ProItem_Vec3)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(KDlg_ProItem_Vec3)
	virtual BOOL OnInitDialog(void);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnChangeEditVec3X();
	afx_msg void OnChangeEditVec3Y();
	afx_msg void OnChangeEditVec3Z();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
	
};

}


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KDLG_PROITEM_VEC3_H__2E659838_7BC6_4645_B6DC_AB1FA2C48384__INCLUDED_)
