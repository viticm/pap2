#pragma once


// KModelEditorBillBoardCloudCreater dialog
class IEKG3DBillBoardCloudCreater;

class KModelEditorBillBoardCloudCreater : public CDialog
{
	DECLARE_DYNAMIC(KModelEditorBillBoardCloudCreater)

public:
	KModelEditorBillBoardCloudCreater(CWnd* pParent = NULL);   // standard constructor
	virtual ~KModelEditorBillBoardCloudCreater();

// Dialog Data
	enum { IDD = IDD_MODELEDITOR_BILLCLOUD };


	IEKG3DBillBoardCloudCreater* m_lpCreater;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	float m_fPrecision;
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};
