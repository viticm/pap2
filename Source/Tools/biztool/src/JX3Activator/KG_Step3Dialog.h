////////////////////////////////////////////////////////////////////////////////
//  Copyright(c) Kingsoft
//
//  FileName    : KG_Step3Dialog.h
//  Version     : 1.0
//  Creator     : Hu Hao
//  Create Date : 2009-6-1 17:31:38
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _KG_STEP_3_DIALOG_H_
#define _KG_STEP_3_DIALOG_H_

class KG_Step3Dialog : public CDialog
{
public:
	KG_Step3Dialog(CWnd* pParent = NULL);
	virtual ~KG_Step3Dialog();

    virtual BOOL ShowWindow(int nCmdShow);

    const char *GetPassport()       { return (LPCTSTR)m_strPassport; }
    const char *GetCheckPassport()  { return (LPCTSTR)m_strCheckPassport; }
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    afx_msg void OnOK();
    afx_msg void OnCancel();
    afx_msg void OnBnClickedButtonRegister();
    afx_msg void OnBnClickedButtonBack();
    afx_msg void OnBnClickedButtonNext();
    afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
    DECLARE_MESSAGE_MAP()
public:
    CString     m_strPassport;
    CString     m_strCheckPassport;
};

#endif // _KG_STEP_3_DIALOG_H_
