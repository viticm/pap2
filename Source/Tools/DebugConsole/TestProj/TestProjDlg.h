// TestProjDlg.h : 头文件
//

#pragma once
#include "afxwin.h"

#define WM_EDIT_RECEIVE_STRING WM_USER + 100

// CTestProjDlg 对话框
class CTestProjDlg : public CDialog
{
// 构造
public:
	CTestProjDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_TESTPROJ_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedButtonSend();
public:
    CEdit m_edit;
};
