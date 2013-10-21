////////////////////////////////////////////////////////////////////////////////
//  Copyright(c) Kingsoft
//
//  FileName    : KG_Step4Dialog.h
//  Version     : 1.0
//  Creator     : Hu Hao
//  Create Date : 2009-6-1 16:52:35
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _KG_STEP_4_DIALOG_H_
#define _KG_STEP_4_DIALOG_H_

class KG_Step4Dialog : public CDialog
{
public:
	KG_Step4Dialog(CWnd* pParent = NULL);
	virtual ~KG_Step4Dialog();

    virtual BOOL ShowWindow(int nCmdShow);

    const char *GetPhone()  { return (LPCTSTR)m_strPhone; }
    const char *GetEmail()  { return (LPCTSTR)m_strEmail; }
    const char *GetQQ() { return (LPCTSTR)m_strQQOrMSN; }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    afx_msg void OnOK();
    afx_msg void OnCancel();
    afx_msg void OnBnClickedButtonBack();
    afx_msg void OnBnClickedButtonNext();
    afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
    DECLARE_MESSAGE_MAP()
public:
    CString     m_strPhone;
    CString     m_strEmail;
    CString     m_strQQOrMSN;
};

#endif // _KG_STEP_4_DIALOG_H_
