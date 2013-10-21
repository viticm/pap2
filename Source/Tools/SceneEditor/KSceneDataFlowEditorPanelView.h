#pragma once


class KSceneDataFlowEditorDefineDialog;
class KSceneDataFlowEditorRunDialog;


// KSceneDataFlowEditorPanelView form view

class KSceneDataFlowEditorPanelView : public CFormView
{
	DECLARE_DYNCREATE(KSceneDataFlowEditorPanelView)

protected:
	KSceneDataFlowEditorPanelView();           // protected constructor used by dynamic creation
	virtual ~KSceneDataFlowEditorPanelView();

	int m_nOutputWindowID;

public:
	enum { IDD = IDD_KSCENEDATAFLOWEDITORPANELVIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

	KSceneDataFlowEditorDefineDialog* m_lpDailogDefine;
	KSceneDataFlowEditorRunDialog*    m_lpDialogRun;
	 
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate();

	CStatic m_Static_PreView;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonDatadefine();
	afx_msg void OnBnClickedButtonRun();
	afx_msg void OnBnClickedButtonProcessor();
};




