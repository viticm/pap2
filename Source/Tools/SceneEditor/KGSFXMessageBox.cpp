////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGSFXMessageBox.cpp
//  Version     : 1.0
//  Creator     : Chiyer (Zhao XinYu)
//  Create Date : 2006-9-2 10:42:04
//  Comment     :
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SceneEditor.h"
#include "KGSFXMessageBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


DWORD SFXMessageBox(LPSTR szMessage, LPSTR szTitle, DWORD dwType)
{
    KGSFXMessageBox messageBox(szMessage, szTitle, dwType);
    messageBox.DoModal();
    return messageBox.GetResult();
}

IMPLEMENT_DYNAMIC(KGSFXMessageBox, CDialog)
KGSFXMessageBox::KGSFXMessageBox(
    LPSTR szMessage,
    LPSTR szTitle,
    DWORD dwType,
    CWnd* pParent
) : CDialog(KGSFXMessageBox::IDD, pParent)
{
    m_dwType = dwType;
    m_strTitle = szTitle;
    m_strInformation = szMessage;
    m_strInformation.Replace(TEXT("\n"), TEXT("\r\n"));
    m_dwReturn = SFXMESSAGE_YES;
}

KGSFXMessageBox::~KGSFXMessageBox()
{
}

void KGSFXMessageBox::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_INFO, m_strInformation);
}


BEGIN_MESSAGE_MAP(KGSFXMessageBox, CDialog)
    ON_BN_CLICKED(IDC_BUTTON_A, OnClickBottonA)
    ON_BN_CLICKED(IDC_BUTTON_B, OnClickBottonB)
    ON_BN_CLICKED(IDC_BUTTON_C, OnClickBottonC)
END_MESSAGE_MAP()

BOOL KGSFXMessageBox::OnInitDialog()
{
    int nResult  = CDialog::OnInitDialog();
    SetWindowText(TEXT("特效编辑器信息提示框>_<"));
    GetDlgItem(IDC_TITLE)->SetWindowText(m_strTitle);
    UpdateData(FALSE);
    return nResult;
}

void  KGSFXMessageBox::OnClickBottonA()
{
    switch (m_dwType)
    {
    case SFX_MESSAGE_STUEDE_LOAD :
        m_dwReturn = SFXMESSAGE_ADD;
        break;
    default :
        break;
    }
    OnOK();
}

void  KGSFXMessageBox::OnClickBottonB()
{
    switch (m_dwType)
    {
    case SFX_MESSAGE_STUEDE_LOAD :
        m_dwReturn = SFXMESSAGE_COVER;
        break;
    default :
        break;
    }
    OnOK();
}

void  KGSFXMessageBox::OnClickBottonC()
{
    switch (m_dwType)
    {
    case SFX_MESSAGE_STUEDE_LOAD :
        m_dwReturn = SFXMESSAGE_CANCEL;
        break;
    default :
        break;
    }
    OnOK();
}

DWORD KGSFXMessageBox::GetResult()
{
    return m_dwReturn;
}
