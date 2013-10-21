// KDlgMDLCheckToolPathSetting.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KDlgMDLCheckToolPathSetting.h"
#include "KDlgMDLCheckTool.h"

// KDlgMDLCheckToolPathSetting dialog

IMPLEMENT_DYNAMIC(KDlgMDLCheckToolPathSetting, CDialog)

KDlgMDLCheckToolPathSetting::KDlgMDLCheckToolPathSetting(CWnd* pParent /*=NULL*/)
	: CDialog(KDlgMDLCheckToolPathSetting::IDD, pParent)
	, m_strMDLFilePath(_T(""))
	, m_strItemFilePath(_T(""))
{

}

KDlgMDLCheckToolPathSetting::~KDlgMDLCheckToolPathSetting()
{
}

void KDlgMDLCheckToolPathSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_MDL, m_strMDLFilePath);
	DDX_Text(pDX, IDC_EDIT_ITEM, m_strItemFilePath);
	DDX_Text(pDX, IDC_EDIT_ACCESSORY, m_strAccFilePath);
}


BEGIN_MESSAGE_MAP(KDlgMDLCheckToolPathSetting, CDialog)
	ON_BN_CLICKED(IDOK, &KDlgMDLCheckToolPathSetting::OnBnClickedOk)
END_MESSAGE_MAP()


// KDlgMDLCheckToolPathSetting message handlers

void KDlgMDLCheckToolPathSetting::OnBnClickedOk()
{
	CWnd *pParent = GetParent();
	if (pParent)
	{
		UpdateData();
		KDlgMDLCheckTool::FilePath Path = {&m_strMDLFilePath, &m_strItemFilePath, &m_strAccFilePath};
		pParent->SendMessage(KDlgMDLCheckTool::WM_FILEPATH, (WPARAM)(&Path));
	}
	OnOK();
}

BOOL KDlgMDLCheckToolPathSetting::OnInitDialog()
{
	CDialog::OnInitDialog();
	UpdateData(FALSE);
	return TRUE;
}
