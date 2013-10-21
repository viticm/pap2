////////////////////////////////////////////////////////////////////////////////
//  Copyright(c) Kingsoft
//
//  FileName    : KG_Step1Dialog.h
//  Version     : 1.0
//  Creator     : Hu Hao
//  Create Date : 2009-5-31 15:14:46
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _KG_STEP_1_DIALOG_H_
#define _KG_STEP_1_DIALOG_H_

class KG_Step1Dialog : public CDialog
{
public:
	KG_Step1Dialog(CWnd* pParent = NULL);
	virtual ~KG_Step1Dialog();
    
    virtual BOOL ShowWindow(int nCmdShow);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();
    afx_msg void OnOK();
    afx_msg void OnCancel();
    afx_msg void OnBnClickedCheckAccept();
    afx_msg void OnBnClickedCheckRefuse();
    afx_msg void OnBnClickedButtonNext();
    afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	DECLARE_MESSAGE_MAP()
private:
    int _Check();
};

#endif  // _KG_STEP_1_DIALOG_H_
