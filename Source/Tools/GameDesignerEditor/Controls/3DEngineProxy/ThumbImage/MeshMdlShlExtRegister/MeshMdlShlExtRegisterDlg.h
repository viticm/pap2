// MeshMdlShlExtRegisterDlg.h : header file
//

#pragma once
#include "afxcmn.h"

#define WM_SHOWTASK WM_USER + 202




// CMeshMdlShlExtRegisterDlg dialog
class CMeshMdlShlExtRegisterDlg : public CDialog
{
// Construction
public:
	CMeshMdlShlExtRegisterDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_MESHMDLSHLEXTREGISTER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	NOTIFYICONDATA m_notifyIconData;

	bool m_bEngineIsRunning;

	bool   ShowSelectDirectoryDlg(const   CString   &   strDlgTitle, CString &outSelectedDirectory)  ;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	DECLARE_MESSAGE_MAP()

	void MinimizeToTray();
	void InitNID();
	afx_msg   LRESULT   onShowTask(WPARAM   wParam,LPARAM   lParam); 

	bool RegisterMeshMdlDll();
	bool UnRegisterMeshMdlDll();
	bool CallDllFunction(CString dllFilePath, LPCTSTR lpszFunctionName);
	bool TryStart3dEngine();
	bool CheckRegister();
	bool TryRun();
	void OnEngineStarted(bool bStartResult);
	void OnEngineClosed();
	void InitEngineRootTextBox();


public:
	CString DllPath;
	CString EngineRootPath;
	
	CListCtrl EngineMemoryInfoList;
	afx_msg void OnBnClickedButtonRefresh();
	
	CString strMaxWorkSetSize;
	CString strMaxPageSize;
	afx_msg void OnBnClickedButtonSetSize();
	CString EngineAppPath;
	CSliderCtrl sliderMaxMemory;
	CSliderCtrl sliderMaxPageSize;
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar);

	afx_msg void OnBnClickedButtonEnable();
	afx_msg void OnBnClickedButtonDisable();
	afx_msg void OnQuit();
	afx_msg void OnReturnBack();
};
