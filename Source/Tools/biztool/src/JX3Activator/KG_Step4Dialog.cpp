////////////////////////////////////////////////////////////////////////////////
//  Copyright(c) Kingsoft
//
//  FileName    : KG_Step4Dialog.cpp
//  Version     : 1.0
//  Creator     : Hu Hao
//  Create Date : 2009-6-2 21:37:31
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Resource.h"
#include "KG_Step4Dialog.h"
#include "KG_Activator.h"

extern KG_Activator *g_pActivator;

KG_Step4Dialog::KG_Step4Dialog(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_STEP4, pParent)
{
}

KG_Step4Dialog::~KG_Step4Dialog()
{
}

void KG_Step4Dialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BOOL KG_Step4Dialog::ShowWindow(int nCmdShow)
{
    GetDlgItem(IDC_EDIT_PHONE)->SetFocus();
    return CDialog::ShowWindow(nCmdShow);
}

BEGIN_MESSAGE_MAP(KG_Step4Dialog, CDialog)
    ON_BN_CLICKED(IDC_BUTTON_BACK, &KG_Step4Dialog::OnBnClickedButtonBack)
    ON_BN_CLICKED(IDC_BUTTON_NEXT, &KG_Step4Dialog::OnBnClickedButtonNext)
    ON_WM_HELPINFO()
END_MESSAGE_MAP()


void KG_Step4Dialog::OnOK()
{
}

void KG_Step4Dialog::OnCancel()
{
    g_pActivator->Cancel();
}

void KG_Step4Dialog::OnBnClickedButtonBack()
{
    g_pActivator->Back();    
}

void KG_Step4Dialog::OnBnClickedButtonNext()
{
    GetDlgItem(IDC_EDIT_PHONE)->GetWindowText(m_strPhone);
    GetDlgItem(IDC_EDIT_EMAIL)->GetWindowText(m_strEmail);
    GetDlgItem(IDC_EDIT_QQ_OR_MSN)->GetWindowText(m_strQQOrMSN);

    g_pActivator->Next();
}

BOOL KG_Step4Dialog::OnHelpInfo(HELPINFO* pHelpInfo)
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