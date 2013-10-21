#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// KDlgResourceList dialog

class KDlgResourceList : public CDialog
{
	DECLARE_DYNAMIC(KDlgResourceList)

public:
	KDlgResourceList(CWnd* pParent = NULL);   // standard constructor
	virtual ~KDlgResourceList();

// Dialog Data
	enum { IDD = IDD_DIALOG_TAGRESOUCELIST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	void Reset();
	HTREEITEM AddBrach(LPCSTR strBrach);
	void AddLeaf(HTREEITEM hRoot, 
		LPCSTR strName, 
		DWORD dwFrame, 
		DWORD dwIndex, 
		DWORD dwType);

	CTreeCtrl m_treeResource;
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnNMDblclkTreeTaglist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonLog();
	CButton m_buttonGenerateLog;
	std::vector<std::string> m_vecFileName;
};
