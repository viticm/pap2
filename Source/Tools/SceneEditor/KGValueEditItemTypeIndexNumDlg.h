#pragma once


// KGValueEditItemTypeIndexNumDlg 对话框

class KGValueEditItemTypeIndexNumDlg : public CDialog
{
	DECLARE_DYNAMIC(KGValueEditItemTypeIndexNumDlg)

public:
	KGValueEditItemTypeIndexNumDlg(CWnd* pParent, int nItem);   // 标准构造函数
	virtual ~KGValueEditItemTypeIndexNumDlg();

// 对话框数据
	enum { IDD = IDD_ITEM_TYPE_INDEX_NUM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
