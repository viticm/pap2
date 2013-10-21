#pragma once

#include "KGPropertyEditTabDlg.h"

enum RETURN_VALUE
{
	E_OVERLAY_ALL,
	E_AUTOTUNE_ALL,
	E_OVERLAY,
	E_AUTOTUNE,
	E_CANCEL,
};

class KGInsertQuestAlertDlg : public CDialog
{
	DECLARE_DYNAMIC(KGInsertQuestAlertDlg)

public:
	KGInsertQuestAlertDlg(
		ITabFile* pExistingFile, CString szExistingFileName, int nExistingFileRow,
		ITabFile* pInsertFile, CString szInsertFileName, int nInsertFileRow,
		IIniFile* pNamePlateFile, CWnd* pParent = NULL
	);   
	virtual ~KGInsertQuestAlertDlg();
	enum { IDD = IDD_DIALOG_INSERT_TAB_FILE_ALERT };

public:
	RETURN_VALUE m_eReturnValue;
	RETURN_VALUE GetReturnValue();
	
	BOOL OnInitDialog();

public:
	KGPropertyListCtrl m_listExisting;
	KGPropertyListCtrl m_ListInsert;

	CString m_szExistingFileName;
	CString m_szInsertFileName;
	
	ITabFile* m_pExistingFile;
	ITabFile* m_pInsertFile;
	IIniFile* m_pNamePlateFile;

	int m_nExistingFileRow;
	int m_nInsertFileRow;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);   

	DECLARE_MESSAGE_MAP()
public:
public:
	afx_msg void OnBnClickedAutoture();
public:
	afx_msg void OnBnClickedOverlay();
public:
	afx_msg void OnBnClickedAutotureAll();
public:
	afx_msg void OnBnClickedOverlayAll();
public:
	afx_msg void OnBnClickedCancel();
};
