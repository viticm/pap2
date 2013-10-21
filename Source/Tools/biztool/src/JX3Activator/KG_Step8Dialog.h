////////////////////////////////////////////////////////////////////////////////
//  Copyright(c) Kingsoft
//
//  FileName    : KG_Step8Dialog.h
//  Version     : 1.0
//  Creator     : Hu Hao
//  Create Date : 2009-6-1 19:03:17
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _KG_STEP_8_DIALOG_H_
#define _KG_STEP_8_DIALOG_H_

class KG_Step8Dialog : public CDialog
{
public:
	KG_Step8Dialog(CWnd* pParent = NULL);
	virtual ~KG_Step8Dialog();

    virtual BOOL ShowWindow(int nCmdShow);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    afx_msg void OnOK();
    afx_msg void OnCancel();
    afx_msg void OnBnClickedButtonFinish();
    afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
    DECLARE_MESSAGE_MAP()
};

#endif  // _KG_STEP_8_DIALOG_H_
