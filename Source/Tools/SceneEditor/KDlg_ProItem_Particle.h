#if !defined(AFX_KDLG_PROITEM_PARTICLE_H__40468A02_C5CF_4933_9D92_C057FD3D8BCA__INCLUDED_)
#define AFX_KDLG_PROITEM_PARTICLE_H__40468A02_C5CF_4933_9D92_C057FD3D8BCA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KDlg_ProItem_Particle.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// KDlg_ProItem_Particle dialog
#include "KDlg_PropertyPage.h"
namespace PropertyBase
{
class KDlg_ProItem_Particle : public CDialog
{
// Construction
public:

	KDlg_PropertyPage* m_lpPropertyPage;
	KModel* m_lpModel;
	PropertyBase::_VarProperty* m_lpProperty;

	void SetStore(PropertyBase::_VarProperty* pProperty,KDlg_PropertyPage* lpPropertyPage)
	{
		m_lpProperty = pProperty;
		m_lpPropertyPage = lpPropertyPage;
	}


	KDlg_ProItem_Particle(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(KDlg_ProItem_Particle)
	enum { IDD = IDD_PROITEM_SFXPART };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(KDlg_ProItem_Particle)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(KDlg_ProItem_Particle)
	afx_msg void OnButton1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
}
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KDLG_PROITEM_PARTICLE_H__40468A02_C5CF_4933_9D92_C057FD3D8BCA__INCLUDED_)
