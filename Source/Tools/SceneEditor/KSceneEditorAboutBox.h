#pragma once


// KSceneEditorAboutBox dialog

class KSceneEditorAboutBox : public CDialog
{
	DECLARE_DYNAMIC(KSceneEditorAboutBox)

public:
	KSceneEditorAboutBox(CWnd* pParent = NULL);   // standard constructor
	virtual ~KSceneEditorAboutBox();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	CString m_strVersion;
};
