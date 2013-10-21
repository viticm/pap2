//{{AFX_INCLUDES()
#include "_frmframe.h"
//}}AFX_INCLUDES
#if !defined(AFX_DLG_MAINTOOL_H__476D6FAB_4F4F_4D6E_91AD_CA342B0AB5DC__INCLUDED_)
#define AFX_DLG_MAINTOOL_H__476D6FAB_4F4F_4D6E_91AD_CA342B0AB5DC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DLG_MainTool.h : header file
//

//#include "SceneSkinEdit.h"

/////////////////////////////////////////////////////////////////////////////
// CDLG_MainTool dialog
//#include "MeshTerran.h"
namespace MFCFramework
{
class CDLG_MainTool : public CDialog
{
// Construction
public:
	CDLG_MainTool(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDLG_MainTool)
	enum { IDD = IDD_MAINTOOL };
	MFCFramework::C_frmFrame	m_MainTool;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDLG_MainTool)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDLG_MainTool)
	afx_msg void OnEGroundUpFrmframe1();
	afx_msg void OnEGroundDownFrmframe1();
	afx_msg void OnEGroundFlatFrmframe1();
	afx_msg void OnEBrush0Frmframe1();
	afx_msg void OnEBrush1Frmframe1();
	afx_msg void OnEBrush2Frmframe1();
	afx_msg void OnEBrush3Frmframe1();
	afx_msg void OnEBrush4Frmframe1();
	afx_msg void OnEHeightMapFrmframe1();
	afx_msg void OnETitleSetChangeFrmframe1();
	afx_msg void OnEViewFrameFrmframe1();
	afx_msg void OnEAdvancedSetFrmframe1();
	afx_msg void OnEAlphaDownFrmframe1();
	afx_msg void OnEAlphaFlatFrmframe1();
	afx_msg void OnEAlphaNoiseFrmframe1();
	afx_msg void OnEAlphaUpFrmframe1();
	afx_msg void OnECircleSelectFrmframe1();
	afx_msg void OnEGroundCaveFrmframe1();
	afx_msg void OnEGroundCliffFrmframe1();
	afx_msg void OnEGroundDeepWaterFrmframe1();
	afx_msg void OnEGroundThinWaterFrmframe1();
	afx_msg void OnELastMaterial1Frmframe1();
	afx_msg void OnELastMaterial2Frmframe1();
	afx_msg void OnELastMaterial3Frmframe1();
	afx_msg void OnELastMaterial4Frmframe1();
	afx_msg void OnEMaterialDownFrmframe1();
	afx_msg void OnEMaterialUpFrmframe1();
	afx_msg void OnESuckFrmframe1();
	afx_msg void OnEViewTextureFrmframe1();
	virtual BOOL OnInitDialog();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	//KModelTerrain* m_lpMap;
};
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_MAINTOOL_H__476D6FAB_4F4F_4D6E_91AD_CA342B0AB5DC__INCLUDED_)
