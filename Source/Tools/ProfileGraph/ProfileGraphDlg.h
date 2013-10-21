// ProfileGraphDlg.h : 头文件
//

#pragma once


#include  "afxcmn.h"
#include  "NSChartCtrl.h"
#include  "ProfileGraphDocument.h"


// CProfileGraphDlg 对话框
class CProfileGraphDlg : public CDialog
{
// 构造
public:
	CProfileGraphDlg(CWnd* pParent = NULL);	// 标准构造函数
	~CProfileGraphDlg();

// 对话框数据
	enum { IDD = IDD_PROFILEGRAPH_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBnClickedBtnOpen();
	afx_msg void OnBnClickedBtnTable();

	afx_msg void OnTvnSelchangedGraphcall(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedGraphcounter(NMHDR *pNMHDR, LRESULT *pResult);

	DECLARE_MESSAGE_MAP()



protected:
	////////////////////////////////////////////////
	// fill system info
	void  fillGraphSysInfo();


	////////////////////////////////////////////////
	// fill tree / list ctrl
	void  fillGraphCallTree();
	void  fillGraphCallTree( HTREEITEM hParent, CProfileGraphNode* pNode );

	void  fillGraphChart( HTREEITEM hTreeSel );
	void  fillGraphLine( HTREEITEM hTreeSel );

	void  fillGraphList( HTREEITEM hTreeSel );
	void  fillGraphList( char const* name, CProfileGraphNode* pChild, CProfileGraphNode* pNode );



protected:
	////////////////////////////////////////////////
	// member
	CProfileGraphDocument	m_GraphDoc;
	CNSChartCtrl			m_GraphChart;
	CStatic					m_GraphLine;
	CEdit					m_GraphSysInfo;

	
	CTreeCtrl m_treeGraphCall;
	CListCtrl m_lstGraphCounter;


	////////////////////////////////////////////////
	// bitmap member
	CDC		  m_CompatibleDC;
	CBitmap	  m_CompatibleBitmap;

};
