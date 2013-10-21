// GameConfigDlg.h : 头文件
//

#pragma once

#include "afxcmn.h"

#include "MainTabCtrl.h"
#include "KClientSetting.h"
#include "KClientEnvironment.h"


// KGameConfigDlg 对话框
class KGameConfigDlg : public CDialog
{
// 构造
public:
	KGameConfigDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_GAMECONFIG_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;
	KMainTabCtrl m_MainTab;
	//KClientEnvironment m_InfoPage;
	//KClientSetting  m_ConfigPage;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedBtnOk();
	afx_msg void OnBnClickedBtnQuit();
	afx_msg void OnBnClickedBtnSysInfo();
	afx_msg void OnBnClickedBtnApply();
};
