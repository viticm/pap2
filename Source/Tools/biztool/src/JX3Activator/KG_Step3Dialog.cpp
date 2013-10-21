////////////////////////////////////////////////////////////////////////////////
//  Copyright(c) Kingsoft
//
//  FileName    : KG_Step3Dialog.cpp
//  Version     : 1.0
//  Creator     : Hu Hao
//  Create Date : 2009-6-1 17:32:13
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Resource.h"
#include "KG_Step3Dialog.h"
#include "KG_Activator.h"

extern KG_Activator *g_pActivator;

KG_Step3Dialog::KG_Step3Dialog(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_STEP3, pParent)
{
}

KG_Step3Dialog::~KG_Step3Dialog()
{
}

BOOL KG_Step3Dialog::ShowWindow(int nCmdShow)
{
    GetDlgItem(IDC_EDIT_PASSPORT)->SetFocus();
    return CDialog::ShowWindow(nCmdShow);
}

void KG_Step3Dialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(KG_Step3Dialog, CDialog)
    ON_BN_CLICKED(IDC_BUTTON_REGISTER, &KG_Step3Dialog::OnBnClickedButtonRegister)
    ON_BN_CLICKED(IDC_BUTTON_BACK,     &KG_Step3Dialog::OnBnClickedButtonBack)
    ON_BN_CLICKED(IDC_BUTTON_NEXT,     &KG_Step3Dialog::OnBnClickedButtonNext)
    ON_WM_HELPINFO()
END_MESSAGE_MAP()

void KG_Step3Dialog::OnOK()
{
}

void KG_Step3Dialog::OnCancel()
{
    g_pActivator->Cancel();
}

void KG_Step3Dialog::OnBnClickedButtonRegister()
{
    ShellExecute(
        0, 
        NULL,
        _T("https://pass.kingsoft.com/register-reg"),
        NULL,
        NULL,
        SW_NORMAL
    );
}

void KG_Step3Dialog::OnBnClickedButtonBack()
{
    g_pActivator->Back();
}

void KG_Step3Dialog::OnBnClickedButtonNext()
{
    GetDlgItem(IDC_EDIT_PASSPORT)->GetWindowText(m_strPassport);
    GetDlgItem(IDC_EDIT_PASSPORT_CHECK)->GetWindowText(m_strCheckPassport);

    g_pActivator->Next();
}

BOOL KG_Step3Dialog::OnHelpInfo(HELPINFO* pHelpInfo)
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