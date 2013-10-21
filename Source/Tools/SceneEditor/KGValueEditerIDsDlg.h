#pragma once


#include "KGPropertyListCtrl.h"

class KGValueEditerIDsDlg : public CDialog
{
	DECLARE_DYNAMIC(KGValueEditerIDsDlg)

public:
	KGValueEditerIDsDlg(
		ITabFile* pTabFile, LPKGLISTITEM pItem, CString strWndTitle, CWnd* pParent = NULL
	);   // standard constructor
	virtual ~KGValueEditerIDsDlg();

	BOOL OnInitDialog();
	int GetID();

// Dialog Data
	enum { IDD = IDD_DIALOG_ID };

public :
	CWnd*		m_pParant;
	ITabFile*	 m_pTabFile;
	LPKGLISTITEM m_pItem;
	CString	    m_strIDField;
	CString		m_strWndTitle;

	int			m_nSelID;

	CString     m_strID;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};
