#pragma once

//#include "KG3DToolsImpl.h"
// KSceneSceneEditorDialogOption dialog
interface IEKG3DSceneSceneEditor;
class KSceneEditorDocLogical;
class KScene;
class KSceneEditorDoc;



class KSceneSceneEditorDialogOption : public CDialog
{
	DECLARE_DYNAMIC(KSceneSceneEditorDialogOption)

public:
	KSceneSceneEditorDialogOption(CWnd* pParent = NULL);   // standard constructor
	virtual ~KSceneSceneEditorDialogOption();
	void SetSceneEditor(IEKG3DSceneSceneEditor* pSceneEdit)
	{
		m_lpSceneSceneEditor = pSceneEdit;
	}
	void SetDocLogical(KSceneEditorDocLogical* pLogical)
	{
		m_lpDocLogical = pLogical;
	}
	void SetDoc(KSceneEditorDoc* pDoc)
	{
		m_lpDoc = pDoc;
	}// Dialog Data
	enum { IDD = IDD_SCENEEDITOR_DOCOPTION };

protected:

	IEKG3DSceneSceneEditor*  m_lpSceneSceneEditor;
	KSceneEditorDocLogical* m_lpDocLogical;
	KSceneEditorDoc* m_lpDoc;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	BOOL m_bShowNpc;
	BOOL m_bShowDoodad;
	BOOL m_bShowWayPoint;
	BOOL m_bShowRefreshPoint;
	BOOL m_bFreezeNpc;
	BOOL m_bFreezeDoodad;
	BOOL m_bFreezeWayPoint;
	BOOL m_bFreezeRefreshPoint;
	int m_nSceneID;
	float m_fLODDistance;
	BOOL m_bShort;
public:
	virtual BOOL OnInitDialog();

	//<显示选项>
	std::vector<CButton*>	m_vecButtons;
	VOID	ShowRenderOptionButtons();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//</显示选项>
private:
	VOID	SyncInterfaceToInner();
	VOID	SyncInnerToInterface();
public:
	afx_msg void OnBnClickedCheckShownpc();
	afx_msg void OnBnClickedCheckShowdoodad();
	afx_msg void OnBnClickedCheckShowwaypoint();
	afx_msg void OnBnClickedCheckShowrefreshpoint();
	afx_msg void OnBnClickedCheckFreezenpc();
	afx_msg void OnBnClickedCheckFreezedoodad();
	afx_msg void OnBnClickedCheckFreezewaypoint();
	afx_msg void OnBnClickedCheckFreezerefreshpoint();
	afx_msg void OnBnClickedCheckShowvisiablerange();
	afx_msg void OnBnClickedCheckShowattackrange();
	afx_msg void OnEnChangeEditLodDistance();
	afx_msg void OnBnClickedCheckZfar();

	afx_msg void OnBnClickedButtonSetRenderAll();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedApply();
	afx_msg void OnBnEnableNormalCheck();
    afx_msg void OnBnClickedCheckPointlight();
};


