// KGAlertMessageBox.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KGAlertMessageBox.h"


// KGAlertMessageBox dialog

KGAlertMessageBox KGAlertMessageBox::m_sAlertMessageBox;

KGAlertMessageBox& g_AlertMessageBox = KGAlertMessageBox::m_sAlertMessageBox;

IMPLEMENT_DYNAMIC(KGAlertMessageBox, CDialog)

KGAlertMessageBox::KGAlertMessageBox(CWnd* pParent /*=NULL*/)
	: CDialog(KGAlertMessageBox::IDD, pParent)
{
}

KGAlertMessageBox::~KGAlertMessageBox()
{
}

void KGAlertMessageBox::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST, m_ListMessage);
}


BEGIN_MESSAGE_MAP(KGAlertMessageBox, CDialog)
    ON_BN_CLICKED(IDOK, &KGAlertMessageBox::OnBnClickedOk)
END_MESSAGE_MAP()


void KGAlertMessageBox::Clear()
{
    m_ListMessage.ResetContent();
}

void KGAlertMessageBox::AddMessage(const char strMessage[])
{
    m_ListMessage.AddString(strMessage);
}

int KGAlertMessageBox::Size()
{
    return m_ListMessage.GetCount();
}

// KGAlertMessageBox message handlers

void KGAlertMessageBox::OnBnClickedOk()
{
    // TODO: Add your control notification handler code here
    OnOK();
}
