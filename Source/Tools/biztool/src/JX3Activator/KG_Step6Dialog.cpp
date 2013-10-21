////////////////////////////////////////////////////////////////////////////////
//  Copyright(c) Kingsoft
//
//  FileName    : KG_Step6Dialog.cpp
//  Version     : 1.0
//  Creator     : Hu Hao
//  Create Date : 2009-6-1 19:05:22
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Resource.h"
#include "KG_Step6Dialog.h"
#include "KG_Activator.h"

extern KG_Activator *g_pActivator;

KG_Step6Dialog::KG_Step6Dialog(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_STEP6, pParent)
{
}

KG_Step6Dialog::~KG_Step6Dialog()
{
}

void KG_Step6Dialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BOOL KG_Step6Dialog::ShowWindow(int nCmdShow)
{
    GetDlgItem(IDC_BUTTON_FINISH)->SetFocus();
    return CDialog::ShowWindow(nCmdShow);
}


BEGIN_MESSAGE_MAP(KG_Step6Dialog, CDialog)
    ON_BN_CLICKED(IDC_BUTTON_FINISH, &KG_Step6Dialog::OnBnClickedButtonFinish)
    ON_WM_HELPINFO()
END_MESSAGE_MAP()


void KG_Step6Dialog::OnOK()
{
}

void KG_Step6Dialog::OnCancel()
{
    g_pActivator->Cancel();
}

void KG_Step6Dialog::OnBnClickedButtonFinish()
{
    g_pActivator->Quit();
}

BOOL KG_Step6Dialog::OnHelpInfo(HELPINFO* pHelpInfo)
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