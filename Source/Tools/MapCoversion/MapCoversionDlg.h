// MapCoversionDlg.h : header file
//

#pragma once

#include "DriveCombo.h"	// Added by ClassView



// CMapCoversionDlg dialog
class CMapCoversionDlg : public CDialog
{
// Construction
public:
	CMapCoversionDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_MAPCOVERSION_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedSearch();
	afx_msg void OnBnClickedDelete();
	afx_msg void OnBnClickedCoversion();
	afx_msg void OnBnClickedComparison();
	DECLARE_MESSAGE_MAP()


protected:
	void  saveMapBox();
	void  cleanMapBox( );

	void  fillMapBox( CString& fileFolder );
	void  fillMapBox( CFileStatus& fileStatus );


protected:
	CDriveCombo	m_cboDrives;
	CString m_strDirectory;


};
