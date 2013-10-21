////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGBuildIniFileDlg.cpp
//  Version     : 1.0
//  Author      : ZhaoXinyu (Chiyer)
//  CopyRight   : Kingsoft Corporation All rights Reserved
//  History     : 2005-11-10 16:41:25
//  Comment     :
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SceneEditor.h"
#include "KGBuildIniFileDlg.h"
#include "KGObjectPropertyEditDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(KGBuildIniFileDlg, CDialog)
KGBuildIniFileDlg::KGBuildIniFileDlg(
	CString* pszFileName, CWnd* pParent /*=NULL*/)
	: CDialog(KGBuildIniFileDlg::IDD, pParent)
{
	m_pszFileName = pszFileName;
}

KGBuildIniFileDlg::~KGBuildIniFileDlg()
{
}

void KGBuildIniFileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_NAME, m_szName);
	DDX_Control(pDX, IDC_TYPE, m_comboxType);
}


BEGIN_MESSAGE_MAP(KGBuildIniFileDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

CString KGBuildIniFileDlg::GetName()
{
	return m_szName;
}

BOOL KGBuildIniFileDlg::IsBuildedAFile()
{
	if (m_nSelect)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}

}

void KGBuildIniFileDlg::OnBnClickedOk()
{
	UpdateData();
	m_nSelect = m_comboxType.GetCurSel();
	CString szTemplate = g_szDefWorkDirectory + CString(NPC_ANI_INI_TEMPLATE_FILE);
	if (m_nSelect)
	{
		*m_pszFileName = *m_pszFileName + "\\" + m_szName + ".ini";
		if (m_szName == "")
		{
			::MessageBox(this->m_hWnd, "请写如文件名", "Error", MB_OK);
			return;
		}
		if (!::CopyFile(szTemplate.GetBuffer(), m_pszFileName->GetBuffer(), TRUE))
		{
			::MessageBox(this->m_hWnd, "创建文件有错", "Error", MB_OK);
			OnCancel();
			return;
		}
	}
	else
	{
		*m_pszFileName = *m_pszFileName + "\\" + m_szName;
		if (m_szName == "")
		{
			::MessageBox(this->m_hWnd, "请写如目录名", "Error", MB_OK);
			return;
		}
		if (::CreateDirectory(m_pszFileName->GetBuffer(), NULL))
		{
			::SetFileAttributes(
				m_pszFileName->GetBuffer(), FILE_ATTRIBUTE_NORMAL
			);
		}
		else
		{
			::MessageBox(this->m_hWnd, "创建目录有错", "Error", MB_OK);
			OnCancel();
			return;
		}
	}

	OnOK();
}

BOOL KGBuildIniFileDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_comboxType.AddString("创建目录");
	m_comboxType.AddString("创建文件");
	m_comboxType.SetCurSel(1);
	return TRUE;
}
