// KModelEditorBspGenerateDialog.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KModelEditorBspGenerateDialog.h"


// KModelEditorBspGenerateDialog dialog

IMPLEMENT_DYNAMIC(KModelEditorBspGenerateDialog, CDialog)

KModelEditorBspGenerateDialog::KModelEditorBspGenerateDialog(CWnd* pParent /*=NULL*/)
	: CDialog(KModelEditorBspGenerateDialog::IDD, pParent)
	, m_fLowRation(0)
	, m_fMediumRation(0)
{
	m_pScene = NULL;
	m_pModelEditor = NULL;
	m_fLowRation = 0.2F;
	m_fMediumRation = 0.6F;
}

KModelEditorBspGenerateDialog::~KModelEditorBspGenerateDialog()
{
}

void KModelEditorBspGenerateDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_List);
	DDX_Control(pDX, IDC_LIST2, m_ListReport);
	DDX_Text(pDX, IDC_EDIT1, m_fLowRation);
	DDX_Text(pDX, IDC_EDIT6, m_fMediumRation);
}


BEGIN_MESSAGE_MAP(KModelEditorBspGenerateDialog, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_SETDIR, &KModelEditorBspGenerateDialog::OnBnClickedButtonSetdir)
	ON_LBN_SELCHANGE(IDC_LIST1, &KModelEditorBspGenerateDialog::OnLbnSelchangeList1)
	ON_BN_CLICKED(IDC_BUTTON_GENERATE, &KModelEditorBspGenerateDialog::OnBnClickedButtonGenerate)
	ON_BN_CLICKED(IDC_BUTTON_SETDIR3, &KModelEditorBspGenerateDialog::OnBnClickedButtonSetdir3)
	ON_BN_CLICKED(IDC_BUTTON_GENERATE2, &KModelEditorBspGenerateDialog::OnBnClickedButtonGenerate2)
	ON_EN_CHANGE(IDC_EDIT1, &KModelEditorBspGenerateDialog::OnEnChangeEdit1)
	ON_EN_CHANGE(IDC_EDIT6, &KModelEditorBspGenerateDialog::OnEnChangeEdit6)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// KModelEditorBspGenerateDialog message handlers

void KModelEditorBspGenerateDialog::OnBnClickedButtonSetdir()
{
	HRESULT hr = S_OK;
	TCHAR strDirectory[MAX_PATH];
	TCHAR szWorkDir[MAX_PATH];

	BROWSEINFO BrowseInfo;
	LPITEMIDLIST pItemList;
	memset(&BrowseInfo, 0, sizeof(BROWSEINFO));
	sprintf(strDirectory,
		"%sData",
		g_szDefWorkDirectory);

	::GetCurrentDirectory(MAX_PATH,szWorkDir);

	USES_CONVERSION;
	hr = SHParseDisplayName(A2CW(strDirectory),
		NULL,
		&pItemList,
		0,
		NULL);

	BrowseInfo.hwndOwner = GetSafeHwnd();
	BrowseInfo.pidlRoot = pItemList;

	LPCITEMIDLIST pItem = SHBrowseForFolder(&BrowseInfo);
	UpdateData(TRUE);
	if (pItem)
	{
		if (!SHGetPathFromIDList(pItem, strDirectory))
		{
			sprintf(strDirectory, "%sData\\", g_szDefWorkDirectory);
		}

		if (strDirectory[strlen(strDirectory) - 1] != '\\')
		{
			strcat(strDirectory, "\\");
		}
		
		m_strDirectory = strDirectory;
		UpdateData(FALSE);

	}

	::SetCurrentDirectory(szWorkDir);

	FindFiles(FALSE);
	FillList();
}


void KModelEditorBspGenerateDialog::FindFilesInDir(LPSTR pDir,BOOL bNoLodMesh)
{
	CFileFind finder;
	TCHAR szAppDirectory[MAX_PATH];

	TCHAR szPath[MAX_PATH];
	sprintf(szPath,"%s%s",pDir,"*.*");

	BOOL bWorking = finder.FindFile(szPath);
	while (bWorking)
	{
		bWorking = finder.FindNextFile();

		TCHAR FileName[MAX_PATH];
		wsprintf(FileName,"%s",(LPCTSTR)finder.GetFilePath());

		TCHAR strDrive[MAX_PATH];
		TCHAR strPath[MAX_PATH];
		TCHAR strFile[MAX_PATH];
		TCHAR strExt[MAX_PATH];

		_splitpath(FileName, strDrive, strPath, strFile, strExt);

		if(finder.IsDirectory())
		{
			if(_strcmpi(strExt,".")!=0 && (_strcmpi(strExt,"..")!=0) && (_strcmpi(strExt,".svn")!=0))
			{
				TCHAR szDirName[MAX_PATH];
				wsprintf(szDirName,"%s\\",FileName);

				FindFilesInDir(szDirName,bNoLodMesh);
			}

		}
		else
		{


			if(_strcmpi(strExt,".Mesh") == 0)
			{
				meshinfo info;

				TCHAR RetFileName[MAX_PATH];
				g_pEngineManager->GetDefWorkDirectory(szAppDirectory, MAX_PATH);
				g_pEngineManager->GetRelativePath(szAppDirectory,FileName,RetFileName);

				if(bNoLodMesh)
				{
					string fileName = RetFileName;
					int nFind0 = fileName.find("_l.");
					int nFind1 = fileName.find("_m.");
					if(nFind0==-1 && nFind1==-1)
					{
						info.bBsp   = FALSE;
						info.szName = RetFileName;
						m_vecMeshInfo.push_back(info);
					}
				}
				else
				{
					info.bBsp   = FALSE;
					info.szName = RetFileName;
					m_vecMeshInfo.push_back(info);
				}
			}
		}
	}
}

void KModelEditorBspGenerateDialog::FindFiles(BOOL bNoLodMesh)
{

	TCHAR szRootPath[MAX_PATH];
	g_GetRootPath(szRootPath);

	m_vecMeshInfo.clear();

	FindFilesInDir((LPSTR)m_strDirectory.c_str(),bNoLodMesh);

}

void KModelEditorBspGenerateDialog::FillList()
{
	int nComboCount = m_List.GetCount();
	for ( int i = 0;i<nComboCount;i++ )
	{
		m_List.DeleteString( 0 );
	}

	for (size_t i=0;i<m_vecMeshInfo.size();i++)
	{
		TCHAR szName[MAX_PATH];
		wsprintf(szName,"%s",m_vecMeshInfo[i].szName.c_str());

		int nIndex = m_List.AddString(szName);
	}
}

void KModelEditorBspGenerateDialog::OnLbnSelchangeList1()
{
	// TODO: Add your control notification handler code here
}

void KModelEditorBspGenerateDialog::OnBnClickedButtonGenerate()
{
	HRESULT hr = E_FAIL;

	if(!m_pModelEditor)
		return;

	for (size_t i=0;i<m_vecMeshInfo.size();i++)
	{
		meshinfo& info = m_vecMeshInfo[i];

		m_List.SetCurSel((int)i);
		
		hr = m_pModelEditor->GenerateBspForMesh((LPSTR)info.szName.c_str());

		//Sleep(30);
	}

	MessageBox("生成结束","提升");
}

void KModelEditorBspGenerateDialog::OnBnClickedButtonSetdir3()
{
	HRESULT hr = S_OK;
	TCHAR strDirectory[MAX_PATH];
	TCHAR szWorkDir[MAX_PATH];

	BROWSEINFO BrowseInfo;
	LPITEMIDLIST pItemList;
	memset(&BrowseInfo, 0, sizeof(BROWSEINFO));
	sprintf(strDirectory,
		"%sData",
		g_szDefWorkDirectory);

	::GetCurrentDirectory(MAX_PATH,szWorkDir);

	USES_CONVERSION;
	hr = SHParseDisplayName(A2CW(strDirectory),
		NULL,
		&pItemList,
		0,
		NULL);

	BrowseInfo.hwndOwner = GetSafeHwnd();
	BrowseInfo.pidlRoot = pItemList;

	LPCITEMIDLIST pItem = SHBrowseForFolder(&BrowseInfo);
	UpdateData(TRUE);
	if (pItem)
	{
		if (!SHGetPathFromIDList(pItem, strDirectory))
		{
			sprintf(strDirectory, "%sData\\", g_szDefWorkDirectory);
		}

		if (strDirectory[strlen(strDirectory) - 1] != '\\')
		{
			strcat(strDirectory, "\\");
		}

		m_strDirectory = strDirectory;
		UpdateData(FALSE);

	}

	::SetCurrentDirectory(szWorkDir);

	FindFiles(TRUE);
	FillList();
}

void KModelEditorBspGenerateDialog::OnBnClickedButtonGenerate2()
{
	HRESULT hr = E_FAIL;

	if(!m_pModelEditor)
		return;

	int nComboCount = m_ListReport.GetCount();
	for ( int i = 0;i<nComboCount;i++ )
	{
		m_ListReport.DeleteString( 0 );
	}

	for (size_t i=0;i<m_vecMeshInfo.size();i++)
	{
		meshinfo& info = m_vecMeshInfo[i];

		m_List.SetCurSel((int)i);

		int nMedium = 0;
		int nLow = 0;
		hr = m_pModelEditor->GenerateLodForMesh((LPSTR)info.szName.c_str(),nMedium,nLow,m_fLowRation,m_fMediumRation);

		TCHAR szName[MAX_PATH];
		wsprintf(szName,"%d %d %s",nLow,nMedium,info.szName.c_str());
		m_ListReport.AddString(szName);
		
		//Sleep(30);
	}

	MessageBox("生成结束","低模生成");
}

void KModelEditorBspGenerateDialog::OnEnChangeEdit1()
{
	UpdateData(TRUE);
}

void KModelEditorBspGenerateDialog::OnEnChangeEdit6()
{
	UpdateData(TRUE);
}

void KModelEditorBspGenerateDialog::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);


	UpdateData(FALSE);
}
