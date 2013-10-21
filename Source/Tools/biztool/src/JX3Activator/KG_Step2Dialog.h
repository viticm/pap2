////////////////////////////////////////////////////////////////////////////////
//  Copyright(c) Kingsoft
//
//  FileName    : KG_Step2Dialog.h
//  Version     : 1.0
//  Creator     : Hu Hao
//  Create Date : 2009-5-31 17:01:26
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _KG_STEP_2_DIALOG_H_
#define _KG_STEP_2_DIALOG_H_

#include <atlimage.h>

class KG_Step2Dialog : public CDialog
{
public:
	KG_Step2Dialog(CWnd* pParent = NULL);
	virtual ~KG_Step2Dialog();

    virtual BOOL ShowWindow(int nCmdShow);
    virtual BOOL DestroyWindow();

    const char *GetPassport()   { return (LPCTSTR)m_strPassport;  }
    const char *GetPassword()   { return (LPCTSTR)m_strPassword;  }
    const char *GetVerifyCode() { return (LPCTSTR)m_strCheckCode; }

    int UpdateVerifyImage(const TCHAR cszImageFileName[]);
    
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    afx_msg void OnPaint();
    afx_msg void OnOK();
    afx_msg void OnCancel();
    afx_msg void OnBnClickedButtonRegister();
    afx_msg void OnBnClickedButtonPassword();
    afx_msg void OnBnClickedButtonUpdateImage();
    afx_msg void OnBnClickedButtonBack();
    afx_msg void OnBnClickedButtonNext();
    afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
    DECLARE_MESSAGE_MAP()
private:
    CString     m_strPassport;
    CString     m_strPassword;
    CString     m_strCheckCode;
    CImage      m_VerifyImage;
};

#endif // _KG_STEP_2_DIALOG_H_
