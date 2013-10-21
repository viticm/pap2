////////////////////////////////////////////////////////////////////////////////
//  Copyright(c) Kingsoft
//
//  FileName    : KG_Step7Dialog.cpp
//  Version     : 1.0
//  Creator     : Hu Hao
//  Create Date : 2009-6-1 19:05:22
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Resource.h"
#include "KG_Step7Dialog.h"
#include "KG_Activator.h"

extern KG_Activator *g_pActivator;

KG_Step7Dialog::KG_Step7Dialog(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_STEP7, pParent)
{
}

KG_Step7Dialog::~KG_Step7Dialog()
{
}

void KG_Step7Dialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BOOL KG_Step7Dialog::ShowWindow(int nCmdShow)
{
    GetDlgItem(IDC_BUTTON_FINISH)->SetFocus();
    return CDialog::ShowWindow(nCmdShow);
}


BEGIN_MESSAGE_MAP(KG_Step7Dialog, CDialog)
    ON_BN_CLICKED(IDC_BUTTON_FINISH, &KG_Step7Dialog::OnBnClickedButtonFinish)
    ON_WM_HELPINFO()
END_MESSAGE_MAP()


void KG_Step7Dialog::OnOK()
{
}

void KG_Step7Dialog::OnCancel()
{
    g_pActivator->Cancel();
}

void KG_Step7Dialog::OnBnClickedButtonFinish()
{
    g_pActivator->Quit();
}


BOOL KG_Step7Dialog::OnHelpInfo(HELPINFO* pHelpInfo)
{
    ShellExecute(
        0, 
        NULL,
        _T("http://jx3.xoyo.com/zt/2009/06/02/index2.shtml"),
        NULL,
        NULL,
        SW_NORMAL
    );
    return true;
}