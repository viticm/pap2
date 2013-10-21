////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGNPCAniEditSettingDlg.cpp
//  Version     : 1.0
//  Author      : ZhaoXinyu (Chiyer)
//  CopyRight   : Kingsoft Corporation All rights Reserved
//  History     : 2005-11-14 13:19:11
//  Comment     :
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SceneEditor.h"
#include "KGNPCAniEditSettingDlg.h"
//#include "KG3DEngineManager.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(KGProjectEditConfigSettingDlg, CDialog)
KGProjectEditConfigSettingDlg::KGProjectEditConfigSettingDlg(
	PROPERTY_EDIT_TYPE eType, CWnd* pParent
): CDialog(KGProjectEditConfigSettingDlg::IDD, pParent)
{
	m_eEditType = eType;
}

KGProjectEditConfigSettingDlg::~KGProjectEditConfigSettingDlg()
{
}

void KGProjectEditConfigSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_DIRECTORY, m_szDirectory);
	DDX_Text(pDX, IDC_NAMEPLATE, m_szNamePlate);
	DDX_Text(pDX, IDC_TEMPLATE, m_szTemplate);
}


BEGIN_MESSAGE_MAP(KGProjectEditConfigSettingDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, OnBnClickedButton3)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

BOOL KGProjectEditConfigSettingDlg::OnInitDialog()
{
//	char szAbsName[MAX_PATH];
//	char szRetName[MAX_PATH];
	char szValue[MAX_PATH];

	CString szSection;
	switch (m_eEditType)
	{
	case PROPERTY_EDIT_TYPE_EQUIP :
		szSection = "EQUIP";
		break;
	case PROPERTY_EDIT_TYPE_NPC_ANI :
		szSection = "NPC_ANI";
		break;
	case PROPERTY_EDIT_TYPE_PLAYER_ANI :
		szSection = "PLAYER_ANI";
		break;
	default:;
	}

	CString szPath =
		CString(g_szDefWorkDirectory)  +
		CString("ObjectPropertyEditConfig.ini");
	IFile* pFile = g_CreateFile(szPath.GetBuffer());
	m_szFilePath = szPath;
	pFile->Release();


	IIniFile* pIniFile = g_OpenIniFile(m_szFilePath);

	if (!pIniFile) return FALSE;

	pIniFile->GetString(
		szSection.GetString(), "Directory", "",  szValue, sizeof(szValue)
	);
	m_szDirectory = CString(szValue);
	pIniFile->GetString(
		szSection.GetString(), "Nameplate", "",  szValue, sizeof(szValue)
	);
	m_szNamePlate = CString(szValue);
	pIniFile->GetString(
		szSection.GetString(), "Template", "",  szValue, sizeof(szValue)
	);
	m_szTemplate = CString(szValue);

	pIniFile->Release();

	UpdateData(FALSE);

	return TRUE;
}

CString KGProjectEditConfigSettingDlg::GetFilePath()
{
	return m_szFilePath;
}

int CALLBACK BrowseCallbackProc(
	HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM lpData
)
{
	if (uMsg == BFFM_INITIALIZED)
	{
		::SendMessage(
			hWnd, BFFM_SETSELECTION, (LPARAM)TRUE, (LPARAM)g_szDefWorkDirectory
		);
	}
	return TRUE;
}

void KGProjectEditConfigSettingDlg::OnBnClickedButton1()
{
	BROWSEINFO   binfo;
	TCHAR        szPath[MAX_PATH];
	ZeroMemory(&binfo, sizeof(BROWSEINFO));
	binfo.hwndOwner  = this->m_hWnd;
	binfo.lpfn       = (BFFCALLBACK)BrowseCallbackProc;
	switch (m_eEditType)
	{
	case PROPERTY_EDIT_TYPE_EQUIP :
		binfo.lpszTitle  = "请选择装备配置文件所在的文件夹";
		break;
	case PROPERTY_EDIT_TYPE_NPC_ANI :
		binfo.lpszTitle  = "请选择NPC动画配置文件所在的文件夹";
		break;
	case PROPERTY_EDIT_TYPE_PLAYER_ANI :
		binfo.lpszTitle  = "请选择主角动画配置文件所在的文件夹";
		break;
	default:;
	}
	binfo.ulFlags    = BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS;
	ITEMIDLIST* pidl = ::SHBrowseForFolder(&binfo);
	if(::SHGetPathFromIDList(pidl, szPath))
	{
		m_szDirectory = CString(szPath);
		if (m_szDirectory.Right(1) != "\\")
		{
			m_szDirectory += "\\";
		}
	}

	UpdateData(FALSE);
}

void KGProjectEditConfigSettingDlg::OnBnClickedButton2()
{
	CFileDialog fileDlg(
		TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		NULL, this
	);

	fileDlg.m_ofn.lpstrInitialDir = m_szDirectory.GetBuffer();
	if (fileDlg.DoModal() == IDOK)
	{
		m_szNamePlate = fileDlg.GetPathName();
		UpdateData(FALSE);
	}
}

void KGProjectEditConfigSettingDlg::OnBnClickedButton3()
{
	CFileDialog fileDlg(
		TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		NULL, this
	);
	fileDlg.m_ofn.lpstrInitialDir = m_szDirectory.GetBuffer();
	if (fileDlg.DoModal() == IDOK)
	{
		m_szTemplate = fileDlg.GetPathName();
		UpdateData(FALSE);
	}
}

void KGProjectEditConfigSettingDlg::OnBnClickedOk()
{
	UpdateData();

	char szValue[MAX_PATH];

	CString szSection;
	switch (m_eEditType)
	{
	case PROPERTY_EDIT_TYPE_EQUIP :
		szSection = "EQUIP";
		break;
	case PROPERTY_EDIT_TYPE_NPC_ANI :
		szSection = "NPC_ANI";
		break;
	case PROPERTY_EDIT_TYPE_PLAYER_ANI :
		szSection = "PLAYER_ANI";
		break;
	default:;
	}

	wsprintf(szValue,"%sObjectPropertyEditConfig.ini",g_szDefWorkDirectory);
	IIniFile* pIniFile = g_OpenIniFile(szValue);

	if (!pIniFile) return;

	pIniFile->WriteString(
		szSection.GetBuffer(), "Directory", m_szDirectory.GetBuffer()
	);
	pIniFile->WriteString(
		szSection.GetBuffer(), "Nameplate", m_szNamePlate.GetBuffer()
	);
	pIniFile->WriteString(
		szSection.GetBuffer(), "Template",   m_szTemplate.GetBuffer()
	);

	pIniFile->Save(szValue);

	pIniFile->Release();

	OnOK();
}
