#pragma once
#include "afxwin.h"


// KSceneEditorAntiPortalSetupDialog dialog
class IEKG3DSceneSceneEditor;
class IEKG3DRepresentAntiPortal;


class KSceneEditorAntiPortalSetupDialog : public CDialog
{
	DECLARE_DYNAMIC(KSceneEditorAntiPortalSetupDialog)

public:
	KSceneEditorAntiPortalSetupDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~KSceneEditorAntiPortalSetupDialog();

	IEKG3DSceneSceneEditor* m_lpSceneEditor;
	IEKG3DRepresentAntiPortal* m_lpAntiPortal;

	void SetEditor(IEKG3DSceneSceneEditor* pEditor)
	{
		m_lpSceneEditor = pEditor;
	}
// Dialog Data
	enum { IDD = IDD_SCENEEDITOR_ANTIPORTAL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	void SetData();
	void GetData();

	virtual BOOL OnInitDialog();
	CComboBox m_ComboxBox_Type;
	float m_fWidthBottom;
	float m_fWidthTop;
	float m_fHeight;
	afx_msg void OnBnClickedButton1();
	int m_nType;
	afx_msg void OnCbnSelchangeCombo1();
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnEnChangeEdit7();
	afx_msg void OnEnChangeEdit8();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};
