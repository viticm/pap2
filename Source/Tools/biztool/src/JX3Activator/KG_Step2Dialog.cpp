////////////////////////////////////////////////////////////////////////////////
//  Copyright(c) Kingsoft
//
//  FileName    : KG_Step2Dialog.cpp
//  Version     : 1.0
//  Creator     : Hu Hao
//  Create Date : 2009-6-1 17:49:26
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Resource.h"
#include "KG_Step2Dialog.h"
#include "KG_Activator.h"

extern KG_Activator *g_pActivator;

KG_Step2Dialog::KG_Step2Dialog(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_STEP2, pParent)
{
}

KG_Step2Dialog::~KG_Step2Dialog()
{
}

BOOL KG_Step2Dialog::ShowWindow(int nCmdShow)
{
    GetDlgItem(IDC_EDIT_PASSPORT)->SetFocus();
    return CDialog::ShowWindow(nCmdShow);
}

BOOL KG_Step2Dialog::DestroyWindow()
{
    if (!m_VerifyImage.IsNull())
    {
        m_VerifyImage.Destroy();
    }

    CDialog::DestroyWindow();

    return TRUE;
}


int KG_Step2Dialog::UpdateVerifyImage(const TCHAR cszImageFileName[])
{
    GetDlgItem(IDC_EDIT_CHECK_CODE)->SetWindowText("");

    if (!m_VerifyImage.IsNull())
        m_VerifyImage.Destroy();

    if (cszImageFileName[0])
        m_VerifyImage.Load(cszImageFileName);
    else
        m_VerifyImage.LoadFromResource(AfxGetApp()->m_hInstance, (UINT)IDB_BITMAP7);

    OnPaint();

    return true;
}

void KG_Step2Dialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(KG_Step2Dialog, CDialog)
    ON_BN_CLICKED(IDC_BUTTON_REGISTER,     &KG_Step2Dialog::OnBnClickedButtonRegister)
    ON_BN_CLICKED(IDC_BUTTON_PASSWORD,     &KG_Step2Dialog::OnBnClickedButtonPassword)
    ON_BN_CLICKED(IDC_BUTTON_BACK,         &KG_Step2Dialog::OnBnClickedButtonBack)
    ON_BN_CLICKED(IDC_BUTTON_NEXT,         &KG_Step2Dialog::OnBnClickedButtonNext)
    ON_BN_CLICKED(IDC_BUTTON_UPDATE_IMAGE, &KG_Step2Dialog::OnBnClickedButtonUpdateImage)
    ON_WM_PAINT()
    ON_WM_DESTROY()
    ON_WM_HELPINFO()
END_MESSAGE_MAP()


// KG_Step2Dialog message handlers

void KG_Step2Dialog::OnPaint()
{
    CRect rect;

    CDialog::OnPaint();

    GetDlgItem(IDC_VERIFY_IMAGE)->GetWindowRect(&rect);
    ScreenToClient(&rect);

    if (!m_VerifyImage.IsNull())
    {
        m_VerifyImage.Draw(GetDC()->m_hDC, rect);
    }
}

void KG_Step2Dialog::OnOK()
{
}

void KG_Step2Dialog::OnCancel()
{
    g_pActivator->Cancel();
}

void KG_Step2Dialog::OnBnClickedButtonRegister()
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

void KG_Step2Dialog::OnBnClickedButtonPassword()
{
    ShellExecute(
        0, 
        NULL,
        _T("https://pass.kingsoft.com/password-restore-index"),
        NULL,
        NULL,
        SW_NORMAL
    );
}

void KG_Step2Dialog::OnBnClickedButtonBack()
{
    g_pActivator->Back();
}

void KG_Step2Dialog::OnBnClickedButtonNext()
{
    GetDlgItem(IDC_EDIT_PASSPORT)->GetWindowText(m_strPassport);
    GetDlgItem(IDC_EDIT_PASSWORD)->GetWindowText(m_strPassword);
    GetDlgItem(IDC_EDIT_CHECK_CODE)->GetWindowText(m_strCheckCode);

    g_pActivator->Next();
}

void KG_Step2Dialog::OnBnClickedButtonUpdateImage()
{
    g_pActivator->UpdateVerifyImage();
}


BOOL KG_Step2Dialog::OnHelpInfo(HELPINFO* pHelpInfo)
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