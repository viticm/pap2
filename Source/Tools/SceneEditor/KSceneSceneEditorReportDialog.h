#pragma once
#include "afxwin.h"


// KSceneSceneEditorReportDialog dialog

class KSceneSceneEditorReportDialog : public CDialog
{
	DECLARE_DYNAMIC(KSceneSceneEditorReportDialog)

public:
	KSceneSceneEditorReportDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~KSceneSceneEditorReportDialog();

	void AddLine(int nLine,LPCTSTR pName);

// Dialog Data
	enum { IDD = IDD_SCENEEDITOR_REPORT };

protected:

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
public:
	afx_msg void OnEnChangeEdit2();
public:
	CEdit m_Edit;
public:
	CString m_Cstring;
};
