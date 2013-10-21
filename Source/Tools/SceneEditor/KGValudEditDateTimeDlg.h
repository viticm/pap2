#pragma once
#include "afxdtctl.h"
#include "KGPropertyEditTabDlg.h"

class KGValudEditDateTimeDlg : public CDialog
{
	DECLARE_DYNAMIC(KGValudEditDateTimeDlg)

public:
	KGValudEditDateTimeDlg(
		ITabFile* pTabFile, IIniFile* pIniFile, LPKGLISTITEM pListItem, 
		CWnd* pParent = NULL
	);
	virtual ~KGValudEditDateTimeDlg();
	enum { IDD = KG_VALUE_EDIT_DATETIME };
	
	virtual BOOL OnInitDialog();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);   
	DECLARE_MESSAGE_MAP()

public:
	ITabFile*		m_pTabFile;
	IIniFile*		m_pIniFile;
	LPKGLISTITEM	m_pListItem;
	CWnd*			m_pParant;

	CDateTimeCtrl   m_dateCtrlStartDate;
	CDateTimeCtrl   m_dateCtrlEndDate;

	CSpinButtonCtrl m_spinButtonStartGameTime;
	CSpinButtonCtrl m_spinButtonEndGameTime;

	CSpinButtonCtrl m_spinButtonStartTime;
	CSpinButtonCtrl m_spinButtonEndTime;

	int             m_nStartGameTime;
	int				m_nEndGameTime;
	int             m_nStartTime;
	int				m_nEndTime;

	CButton			m_checkBoxTime;
	CButton			m_checkBoxGameTime;

public:
	afx_msg void OnBnClickedOk();
public:
	afx_msg void OnBnClickedCheckG();
public:
	afx_msg void OnBnClickedCheck();
};
