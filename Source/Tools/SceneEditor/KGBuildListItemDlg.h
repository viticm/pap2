#pragma once


// KGBuildListItemDlg 对话框

class KGBuildListItemDlg : public CDialog
{
	DECLARE_DYNAMIC(KGBuildListItemDlg)

public:
	KGBuildListItemDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~KGBuildListItemDlg();

// 对话框数据
	enum { IDD = IDD_BUILDITEM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
