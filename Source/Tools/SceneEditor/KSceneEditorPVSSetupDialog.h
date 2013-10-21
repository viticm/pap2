#pragma once
#include "afxwin.h"


// KSceneEditorPVSSetupDialog dialog
class IEKG3DSceneSceneEditor;
class IEKG3DRepresentPVS;

class KSceneEditorPVSSetupDialog : public CDialog
{
	DECLARE_DYNAMIC(KSceneEditorPVSSetupDialog)

public:
	KSceneEditorPVSSetupDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~KSceneEditorPVSSetupDialog();

	IEKG3DRepresentPVS* m_lpPVS;
	IEKG3DSceneSceneEditor* m_lpSceneEditor;
	void SetEditor(IEKG3DSceneSceneEditor* pEditor)
	{
		m_lpSceneEditor = pEditor;
	}
// Dialog Data
	enum { IDD = IDD_SCENEEDITOR_PVS };

protected:
	void SetBorderBox();

	void SetPortal();
	void FillPortalListBox();
	void FillObjectInsideListBox();

	void FillData();

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedButtonConverppvs();
	CListBox m_ListBoxPortal;
	CListBox m_ListBoxObjectInside;
	afx_msg void OnLbnSelchangeListPortal();
	float m_fPositionX;
	float m_fPositionY;
	float m_fPositionZ;
	float m_fDirectionX;
	float m_fDirectionY;
	float m_fDirectionZ;
	float m_fWidth;
	float m_fHeight;
	afx_msg void OnEnChangeEditHeight();
	afx_msg void OnBnClickedButtonAddportal();
	afx_msg void OnBnClickedButtonDeleteportal();
	afx_msg void OnBnClickedButtonAddobject();
	afx_msg void OnEnChangeEditPosx();
	afx_msg void OnEnChangeEditPosy();
	afx_msg void OnEnChangeEditPosz();
	afx_msg void OnEnChangeEditDirx();
	afx_msg void OnEnChangeEditDiry();
	afx_msg void OnEnChangeEditDirz();
	afx_msg void OnEnChangeEditWidth();
	afx_msg void OnBnClickedButton2();
	float m_fBoxLength;
	float m_fBoxWidth;
	afx_msg void OnEnChangeEditBoxw();
	float m_fBoxHeight;
	afx_msg void OnEnChangeEditBoxh();
	afx_msg void OnEnChangeEditBoxl();
	BOOL m_bShowBorder;
	afx_msg void OnBnClickedCheckUsebox();
};


