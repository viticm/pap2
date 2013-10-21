#pragma once
#include "afxcmn.h"


// KGShortcutKey dialog

class KGShortcutKeyDlg : public CDialog
{
	DECLARE_DYNAMIC(KGShortcutKeyDlg)

public:
	KGShortcutKeyDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~KGShortcutKeyDlg();
	
// Dialog Data
	enum { IDD = IDD_SHORTCUT_KEY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	BOOL OnInitDialog();
	BOOL FillKeyList();
public:
	CListCtrl m_KeyList;
};
