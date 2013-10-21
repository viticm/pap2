#pragma once


// KDlgMDLCheckToolPathSetting dialog
class KDlgMDLCheckTool;
class KDlgMDLCheckToolPathSetting : public CDialog
{
	DECLARE_DYNAMIC(KDlgMDLCheckToolPathSetting)
	friend class KDlgMDLCheckTool;
public:
	KDlgMDLCheckToolPathSetting(CWnd* pParent = NULL);   // standard constructor
	virtual ~KDlgMDLCheckToolPathSetting();

// Dialog Data
	enum { IDD = IDD_DIALOG_MDLCHECKTOOLPATHSETTING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	CString m_strMDLFilePath;
	CString m_strItemFilePath;
	CString m_strAccFilePath;
};
