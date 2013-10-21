#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "KGNormalMapCtreatorSetting.h"

#define ParamFile   "data\\public\\normal_map_creator_params.ini"

class KGNormalMapCreatorDlg : public CDialog
{
	DECLARE_DYNAMIC(KGNormalMapCreatorDlg)

public:
	KGNormalMapCreatorDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~KGNormalMapCreatorDlg();

    BOOL ExecuteCmd(const char* cmd, DWORD dwWait = INFINITE);
    void CheckBuildPath(const char file_name[]);
    BOOL OnInitDialog();
    void ClearSelected();
    void FillList(CString strPath);

	enum { IDD = IDD_DIALOG_NORMAL_CREATER };

public :
    CProgressCtrl m_Progress;
    CListCtrl m_InfoList;
    BOOL m_bProcess;
    NormalMapParam m_Params;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedButtonAddDir();
    afx_msg void OnLbnSelchangeList2();
    afx_msg void OnBnClickedButtonClear();
    afx_msg void OnBnClickedOk();
    afx_msg void OnBnClickedButton2();
    afx_msg void OnBnClickedButton3();
    afx_msg void OnLvnItemchangedListInfo(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnHdnItemdblclickListInfo(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMDblclkListInfo(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnBnClickedButtonGenSel();
    afx_msg void OnBnClickedButtonDelete();
    afx_msg void OnBnClickedButtonSetting();
};
