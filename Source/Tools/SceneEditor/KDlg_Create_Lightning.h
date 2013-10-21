#pragma once


// KDlg_Create_Lightning dialog

class KDlg_Create_Lightning : public CDialog
{
	DECLARE_DYNAMIC(KDlg_Create_Lightning)

public:
	KDlg_Create_Lightning(CWnd* pParent = NULL);   // standard constructor
	virtual ~KDlg_Create_Lightning();

// Dialog Data
	enum { IDD = IDD_CREATE_LIGHTNING }; 

	void GetName(string& sName) const { sName = m_Name; }
	void GetPath(string& sPath) const { sPath = m_Path; }
	//KLightning* GetLightning(KModelTable* pKMT);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	string m_Name, m_Path;
	unsigned int m_MaxNumOfNodes;


	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBrowseClicked();
	afx_msg void OnOkClicked();
	afx_msg void OnCancelClicked();
	afx_msg void OnClose();
};
