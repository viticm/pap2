#if !defined(AFX_KDLG_PROITEM_TERRAIN_GROUND_TEXTURE_H__B799D487_FF86_4FEB_A6BC_A651A12B8372__INCLUDED_)
#define AFX_KDLG_PROITEM_TERRAIN_GROUND_TEXTURE_H__B799D487_FF86_4FEB_A6BC_A651A12B8372__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KDlg_ProItem_Terrain_Ground_Texture.h : header file
//
#include "./KColorButton.h"

/////////////////////////////////////////////////////////////////////////////
// KDlg_ProItem_Terrain_Ground_Texture dialog
namespace PropertyBase
{
class KDlg_ProItem_Terrain_Ground_Texture : public CDialog
{
// Construction
public:
	//KDlg_PropertyPage* m_lpPropertyPage;
	//PropertyBase::_VarProperty* m_lpProperty;

	//void SetStore(PropertyBase::_VarProperty* pProperty,KDlg_PropertyPage* lpPropertyPage)
	//{
	//	m_lpProperty = pProperty;
	//	m_lpPropertyPage = lpPropertyPage;
	//}


	KDlg_ProItem_Terrain_Ground_Texture(CWnd* pParent = NULL);   // standard constructor

	void FillTree();
// Dialog Data
	//{{AFX_DATA(KDlg_ProItem_Terrain_Ground_Texture)
	enum { IDD = IDD_PROITEM_TERRAIN_GROUNDTEXTURE };
	KColorButton m_ColorButton;
	CSliderCtrl	m_SliderAlpha;
	CSliderCtrl	m_SliderSize;
	CTreeCtrl	m_Tree;
	CString	m_CurTextureName;
	int		m_Size;
	int		m_Alpha;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(KDlg_ProItem_Terrain_Ground_Texture)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(KDlg_ProItem_Terrain_Ground_Texture)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangedTree1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRshape1();
	afx_msg void OnRshape2();
	afx_msg void OnRshape3();
	afx_msg void OnRshape4();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBcolor();
	afx_msg void OnRegionAdd();
	afx_msg void OnRegionDelete();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

}
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KDLG_PROITEM_TERRAIN_GROUND_TEXTURE_H__B799D487_FF86_4FEB_A6BC_A651A12B8372__INCLUDED_)
