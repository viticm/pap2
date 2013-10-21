#pragma once
interface IEKG3DSceneSceneEditor;
#include "KG3DMessenger.h"

// KGDlgSnapTool dialog
class KG3DSnapTool;

class KGDlgSnapTool : public CDialog, public KG3DMessageSingleSenderReceiver
{
	DECLARE_DYNAMIC(KGDlgSnapTool)

public:
	KGDlgSnapTool(CWnd* pParent = NULL);   // standard constructor
	virtual ~KGDlgSnapTool();
	void SetEditor(IEKG3DSceneSceneEditor* pEditor);
	void EnableExportTexture(BOOL bEnable);
// Dialog Data
	enum { IDD = IDD_DIALOG_SNAPTOOL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
private:
	afx_msg void OnBnClickedOk();
	afx_msg void OnEnableTree();
	afx_msg void OnBnClickedCancel();
	void OnOK();
	void SaveSketch();
	void ExportTerrainTexture();
	BOOL  m_bExportTexture;
	float m_fSclae;
	float m_fLeafScale;
	IEKG3DSceneSceneEditor* m_pEditor;
	CEdit m_editLeafScale;
	CStatic m_staticLeafScale;
	CButton m_checkEnableTree;

    virtual void __stdcall OnReceiveMessage( const KG3DMessage& Message , IKG3DMessageSenderBase* pSender );
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

public:
	afx_msg void OnEnChangeEdit1();
	void UpdateDataButton();
};

