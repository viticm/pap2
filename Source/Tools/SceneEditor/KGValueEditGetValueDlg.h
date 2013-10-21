#pragma once


// KGValueEditGetValueDlg 对话框

class KGValueEditGetValueDlg : public CDialog
{
	DECLARE_DYNAMIC(KGValueEditGetValueDlg)

public:
	KGValueEditGetValueDlg(CWnd* pParent, CString szTitle, int nValueSize);   // 标准构造函数
	virtual ~KGValueEditGetValueDlg();
	enum { IDD = IDD_VALUE_EDIT_GET_VALUE };
	CString GetValue();
	virtual BOOL OnInitDialog();
public:
	int m_nValueSize;
	CString m_szValue;
	CString m_szTitle;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};
