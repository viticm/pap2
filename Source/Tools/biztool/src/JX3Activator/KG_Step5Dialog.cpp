////////////////////////////////////////////////////////////////////////////////
//  Copyright(c) Kingsoft
//
//  FileName    : KG_Step5Dialog.cpp
//  Version     : 1.0
//  Creator     : Hu Hao
//  Create Date : 2009-6-1 19:00:05
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Resource.h"
#include "KG_Step5Dialog.h"
#include "KG_Activator.h"

extern KG_Activator *g_pActivator;

KG_Step5Dialog::KG_Step5Dialog(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_STEP4, pParent)
{
}

KG_Step5Dialog::~KG_Step5Dialog()
{
}

BOOL KG_Step5Dialog::ShowWindow(int nCmdShow)
{
    GetDlgItem(IDC_BUTTON_FINISH)->SetFocus();
    return CDialog::ShowWindow(nCmdShow);
}

void KG_Step5Dialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(KG_Step5Dialog, CDialog)
    ON_BN_CLICKED(IDC_BUTTON_FINISH,   &KG_Step5Dialog::OnBnClickedButtonFinish)
    ON_BN_CLICKED(IDC_BUTTON_DOWNLOAD, &KG_Step5Dialog::OnBnClickedButtonDownload)
    ON_WM_HELPINFO()
END_MESSAGE_MAP()


void KG_Step5Dialog::OnOK()
{
}

void KG_Step5Dialog::OnCancel()
{
    g_pActivator->Cancel();
}

void KG_Step5Dialog::OnBnClickedButtonFinish()
{
    g_pActivator->Quit();
}

void KG_Step5Dialog::OnBnClickedButtonDownload()
{
    ShellExecute(
        0, 
        NULL,
        _T("http://jx3.xoyo.com/xgxz/2009/06/02/135813.shtml"),
        NULL,
        NULL,
        SW_NORMAL
    );
}


BOOL KG_Step5Dialog::OnHelpInfo(HELPINFO* pHelpInfo)
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