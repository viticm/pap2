#pragma once


// KSceneModelEditorMeshSocketSetDialog dialog

class KSceneModelEditorMeshSocketSetDialog : public CDialog
{
	DECLARE_DYNAMIC(KSceneModelEditorMeshSocketSetDialog)

public:
	KSceneModelEditorMeshSocketSetDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~KSceneModelEditorMeshSocketSetDialog();

	// Dialog Data
	//{{AFX_DATA(KSceneModelEditorPropertyPageAnimation)
	enum { IDD = IDD_MODELEDITOR_SOCKETSET };
	CSliderCtrl	m_SliderCtrl_Y;
	CSliderCtrl	m_SliderCtrl_P;
	CSliderCtrl	m_SliderCtrl_R;
	//}}AFX_DATA


	// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(KSceneModelEditorPropertyPageAnimation)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL



protected:
	BOOL               m_bMoving;
	BOOL               m_bRotation;
	/*K3DScene::ENTITYLIST m_listEntityRender;
	K3DScene::ENTITYLIST m_listEntitySeletable;
	K3DScene::ENTITYLIST m_listEntitySeleted;*/

	/*KModel::LPSOCKET   m_lpSocket;
	KSceneModelEditor* m_lpModelEditor;*/

	// Generated message map functions
	//{{AFX_MSG(KSceneModelEditorPropertyPageAnimation)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	/*void SetSocket(KSceneModelEditor* pEditor,KModel::LPSOCKET pSocket)
	{
		m_lpModelEditor = pEditor;
		m_lpSocket = pSocket;
	}*/
	afx_msg void OnBnClickedButtonEdit();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedButtonEditrotation();
	afx_msg void OnNMCustomdrawSliderY(NMHDR *pNMHDR, LRESULT *pResult);
};
