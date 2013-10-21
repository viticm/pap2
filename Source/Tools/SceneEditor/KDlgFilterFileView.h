#pragma once
#include "afxwin.h"


// KDlgFilterFileView dialog
typedef void (*pfCallBack)(LPCSTR, void*);


class KDlgFilterFileView : public CDialog
{
	DECLARE_DYNAMIC(KDlgFilterFileView)

public:
	KDlgFilterFileView(CWnd* pParent = NULL);   // standard constructor
	virtual ~KDlgFilterFileView();

// Dialog Data
	enum { IDD = IDD_DIALOG_FILTERFILEVIEW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	void FillListByFilter();
	void OnRefreshInitDir();
	CListBox m_lstFiles;
	std::vector<CString> m_strFilesInDir;
	std::vector<CString> m_strFileNames;
public:
	CString m_strInitDir;
	CString m_strFilter;
	pfCallBack m_pfCallBack;
	afx_msg void OnBnClickedButtonRefresh();
	virtual void OnOK();
	afx_msg void OnLbnDblclkListFiles();
private:
	void FilterStringToKeys();
	CString m_strFullPath;
	std::vector<string> m_Keywords;
public:
	afx_msg void OnLbnSelchangeListFiles();
};
