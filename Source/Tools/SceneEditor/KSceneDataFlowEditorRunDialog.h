#pragma once


// KSceneDataFlowEditorRunDialog dialog
class IEKG3DSceneDataFlowEditor;
class IEKG3DScene;

class KSceneDataFlowEditorRunDialog : public CDialog
{
	DECLARE_DYNAMIC(KSceneDataFlowEditorRunDialog)

public:
	KSceneDataFlowEditorRunDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~KSceneDataFlowEditorRunDialog();

// Dialog Data
	enum { IDD = IDD_KSCENEDATAFLOWEDITORRUN };

	HRESULT SetDataFlowEditor(IEKG3DScene* pScene,IEKG3DSceneDataFlowEditor* pEditor)
	{
		m_lpScene = pScene;
		m_lpDataFlowEditor = pEditor;
		return S_OK;
	}
protected:
	IEKG3DSceneDataFlowEditor* m_lpDataFlowEditor;
	IEKG3DScene* m_lpScene; 
	int m_nOutputWindowID;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual void OnOK();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClose();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};
