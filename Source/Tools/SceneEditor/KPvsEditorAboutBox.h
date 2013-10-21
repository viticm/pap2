#pragma once


// KPvsEditorAboutBox dialog

class KPvsEditorAboutBox : public CDialog
{
	DECLARE_DYNAMIC(KPvsEditorAboutBox)

public:
	KPvsEditorAboutBox(CWnd* pParent = NULL);   // standard constructor
	virtual ~KPvsEditorAboutBox();

// Dialog Data
	enum { IDD = IDD_DIALOG46 };

    BOOL OnInitDialog();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
    CString m_strText;
    afx_msg void OnBnClickedOk();
};
