#pragma once


#include "KG3DObjectEditor.h"

class KGValueEdit4FilePathDlg : public CDialog
{
	DECLARE_DYNAMIC(KGValueEdit4FilePathDlg)

public:
	KGValueEdit4FilePathDlg(
		ITabFile* pTabFile, IIniFile* pIniFile, LPKGLISTITEM pListItem, 
		CWnd* pParent = NULL
	);
	virtual ~KGValueEdit4FilePathDlg();
	enum { IDD = IDD_DIALOG_4FILE };

	virtual BOOL OnInitDialog();
	int UpdateParam(CString* pStringParam);

public :
	CString				m_stringParam_1;
	CString				m_stringParam_2;
	CString				m_stringParam_3;
	CString				m_stringParam_4;

	CButton				m_bnParam_1;
	CButton				m_bnParam_2;
	CButton				m_bnParam_3;
	CButton				m_bnParam_4;

	ITabFile*			m_pTabFile;
	IIniFile*			m_pIniFile;
	LPKGLISTITEM		m_pListItem;
	CWnd*				m_pParant;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedOk();

	DECLARE_MESSAGE_MAP()
};
