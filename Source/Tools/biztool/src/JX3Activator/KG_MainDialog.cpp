////////////////////////////////////////////////////////////////////////////////
//  Copyright(c) Kingsoft
//
//  FileName    : KG_MainDialog.cpp
//  Version     : 1.0
//  Creator     : Hu Hao
//  Create Date : 2009-5-31 15:19:39
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Resource.h"
#include "KG_MainDialog.h"
#include "KG_Activator.h"

extern KG_Activator *g_pActivator;

KG_MainDialog::KG_MainDialog(CWnd* pParent /* = NULL */)
    : CDialog(IDD_MAIN, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

BOOL KG_MainDialog::OnInitDialog()
{
    int nResult  = false;
    int nRetCode = false;

    nRetCode = CDialog::OnInitDialog();
    KGLOG_PROCESS_ERROR(nRetCode && "CDialog::OnInitDialog()");

    SetIcon(m_hIcon, TRUE);			// Set big icon
    SetIcon(m_hIcon, FALSE);		// Set small icon

    nRetCode = g_pActivator->InitChildDialog();
    KGLOG_PROCESS_ERROR(nRetCode && "g_pActivator->InitChildDialog()");
    
    nResult = true;
Exit0:
    return nResult;
}

void KG_MainDialog::OnCancel()
{
    g_pActivator->Cancel();
}