////////////////////////////////////////////////////////////////////////////////
//  Copyright(c) Kingsoft
//
//  FileName    : KG_MainDialog.h
//  Version     : 1.0
//  Creator     : Hu Hao
//  Create Date : 2009-5-31 14:41:54
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _KG_MAIN_DIALOG_H_
#define _KG_MAIN_DIALOG_H_


class KG_MainDialog : public CDialog
{
    // Construction
public:
    KG_MainDialog(CWnd* pParent = NULL);
protected:
    virtual BOOL OnInitDialog();
    afx_msg void OnCancel();

private:
    HICON           m_hIcon;
};

#endif // _KG_MAIN_DIALOG_H_