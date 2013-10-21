// InputBox.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "InputBox.h"


int InputBox(const char szTitle[], CWnd* pParent, char szText[])
{
    CInputBox box(szText, pParent);

    if (box.DoModal() == IDOK)
    {
        box.GetText(szText);
        return IDOK;
    }

    return IDCANCEL;
}

IMPLEMENT_DYNAMIC(CInputBox, CDialog)

CInputBox::CInputBox(CString szInitText, CWnd* pParent /* = NULL */)
	: CDialog(CInputBox::IDD, pParent)
    , m_szText(szInitText)
{

}

CInputBox::~CInputBox()
{
}

void CInputBox::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT, m_szText);
}


BEGIN_MESSAGE_MAP(CInputBox, CDialog)
    ON_BN_CLICKED(IDOK, &CInputBox::OnBnClickedOk)
    ON_BN_CLICKED(IDCANCEL, &CInputBox::OnBnClickedCancel)
END_MESSAGE_MAP()

CString CInputBox::GetText()
{
    return m_szText;
}

void CInputBox::GetText(char szText[])
{
    strcpy(szText, m_szText.GetBuffer());
    m_szText.ReleaseBuffer();
}

// CInputBox message handlers

void CInputBox::OnBnClickedOk()
{
    UpdateData();
    OnOK();
}

void CInputBox::OnBnClickedCancel()
{
    // TODO: Add your control notification handler code here
    OnCancel();
}
