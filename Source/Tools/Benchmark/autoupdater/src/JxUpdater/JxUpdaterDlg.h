// JxUpdaterDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"

#define WM_UPDATE_MSG (WM_USER + 10)
#define WM_STARTUP_MSG (WM_USER + 20)

// KJxUpdaterDlg dialog
class KJxUpdaterDlg : public CDialog
{
// Construction
public:
	KJxUpdaterDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_JXUPDATER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

// Implementation
protected:
	HICON m_hIcon;
	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnBnClickedCheckUpdate();
	afx_msg void OnBnClickedQuit();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnDownloaderMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnStartUpMsg(WPARAM wParam, LPARAM lParam);
	//afx_msg LRESULT Update(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

private:
	CStatic       m_CtrlLogo;
	CStatic       m_CtrlXsjLogo;
	CBitmap       m_BmpLogo;
	CBitmap       m_BmpXsjLogo;
	CProgressCtrl m_CtrlProgress;
	CButton       m_CtrlCheckUpdate;
	CButton       m_CtrlQuit;
	CStatic       m_CtrlStatus;
	
private:
	int  m_nVersion;
	int  m_nNewVersion;
	bool m_bNeedUpdate;
	char m_szServerUrl[MAX_PATH];
	char m_szAppPath[MAX_PATH];
	char m_szUpdatePath[MAX_PATH];
	char m_szUpdateFileName[MAX_PATH];
private:
	int  DownloadFile(const char* szFilename, bool bNotify = true, bool bCovner = false);
	int  Update();
	int  LoadConfig();
	int  CheckVersion();
	void CheckUpdate();
	int  UpdateExtract(const CString &strPack);
	int  ClearFile(const CString &strPath);
	int  CreateLogPath(const char* szPath);
	static DWORD WINAPI ShowWebThreadFunc(LPVOID lpParam);
	INT _ShowWebThreadFunc();
	void Startup();
	void InitParam();
	
};



