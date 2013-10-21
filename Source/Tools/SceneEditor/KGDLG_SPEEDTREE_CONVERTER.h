#pragma once


// KGDLG_SPEEDTREE_CONVERTER dialog
//#include <string>
#include <vector>
#include "afxcmn.h"
#include "afxwin.h"

class KGDLG_SPEEDTREE_CONVERTER : public CDialog
{
	DECLARE_DYNAMIC(KGDLG_SPEEDTREE_CONVERTER)

public:
	KGDLG_SPEEDTREE_CONVERTER(CWnd* pParent = NULL);   // standard constructor
	virtual ~KGDLG_SPEEDTREE_CONVERTER();

// Dialog Data
	enum { IDD = IDD_DIALOG_SPDVERSIONCONVERT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	CString m_strDirectory;
	afx_msg void OnBnClickedButtonSeletfile();
	afx_msg void OnBnClickedButtonConvert();
	afx_msg void OnBnClickedButtonBrowse();
	HRESULT DoConvert(LPCSTR strFileName);
	void FindFilesInSubDir(LPCSTR strDirName);
	std::vector<CString> m_vecSelectedFiles;
	CListBox m_listBoxResult;
};
