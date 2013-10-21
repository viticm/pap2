// KGDLG_SPEEDTREE_CONVERTER.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KGDLG_SPEEDTREE_CONVERTER.h"
//#include "KG3DEngineManager.h"
#include "IEKG3DModelSpeedTree.h"
#include "IEKG3DModelTable.h"
//#include "SceneEditorCommon.h"
#include "IEEditor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


// KGDLG_SPEEDTREE_CONVERTER dialog

IMPLEMENT_DYNAMIC(KGDLG_SPEEDTREE_CONVERTER, CDialog)

KGDLG_SPEEDTREE_CONVERTER::KGDLG_SPEEDTREE_CONVERTER(CWnd* pParent /*=NULL*/)
	: CDialog(KGDLG_SPEEDTREE_CONVERTER::IDD, pParent)
	, m_strDirectory(_T(""))
{

}

KGDLG_SPEEDTREE_CONVERTER::~KGDLG_SPEEDTREE_CONVERTER()
{
}

void KGDLG_SPEEDTREE_CONVERTER::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_DIRECTORY, m_strDirectory);
	DDX_Control(pDX, IDC_LIST2, m_listBoxResult);
}


BEGIN_MESSAGE_MAP(KGDLG_SPEEDTREE_CONVERTER, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_SELETFILE, &KGDLG_SPEEDTREE_CONVERTER::OnBnClickedButtonSeletfile)
	ON_BN_CLICKED(IDC_BUTTON_CONVERT, &KGDLG_SPEEDTREE_CONVERTER::OnBnClickedButtonConvert)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, &KGDLG_SPEEDTREE_CONVERTER::OnBnClickedButtonBrowse)
END_MESSAGE_MAP()

//IFile* g_pFile = NULL;
// KGDLG_SPEEDTREE_CONVERTER message handlers

void KGDLG_SPEEDTREE_CONVERTER::OnBnClickedButtonSeletfile()
{
	TCHAR strPathName[MAX_PATH];
	TCHAR strBuffer[2048] = "";
	sprintf_s(strPathName,
		MAX_PATH,
		"%sData\\source\\maps_source\\",
		g_szDefWorkDirectory);

	CFileDialog dlgFiles(TRUE,
		".STree",
		NULL,
		4 | 2 |OFN_ALLOWMULTISELECT | OFN_ENABLESIZING,
		"SpeedTree(*.STree)|*.STree||",
		this);

	dlgFiles.m_ofn.lpstrInitialDir = strPathName;
	dlgFiles.m_ofn.lpstrFile = strBuffer;
	dlgFiles.m_ofn.nMaxFile = 2048;
	m_vecSelectedFiles.clear();

	if (IDOK == dlgFiles.DoModal())
	{
		POSITION FilePos = dlgFiles.GetStartPosition();
		while(FilePos)
		{
			CString strPathName = dlgFiles.GetNextPathName(FilePos);
			m_vecSelectedFiles.push_back(strPathName.GetString());
		}
	}
}

void KGDLG_SPEEDTREE_CONVERTER::FindFilesInSubDir(LPCSTR strDirName)
{
	TCHAR strInput[MAX_PATH];
	CFileFind Find;
	sprintf(strInput, "%s\\*.*", strDirName);
	BOOL bFound = Find.FindFile(strInput);
	while (bFound)
	{
		bFound = Find.FindNextFile();
		if (Find.IsDirectory())
		{
			CString strDirName = Find.GetFileName();
			if (strDirName != "." && strDirName != "..")
			{
				FindFilesInSubDir(Find.GetFilePath());
			}
		}
		else
		{
			CString strFileName = Find.GetFilePath();
			strFileName.MakeLower();
			TCHAR strExt[MAX_PATH];
			_splitpath(strFileName, NULL, NULL, NULL, strExt);
			if (!strcmp(strExt, ".stree"))
			{
				DoConvert(strFileName);
			}
		}
	}
	Find.Close();
}

void KGDLG_SPEEDTREE_CONVERTER::OnBnClickedButtonConvert()
{
	TCHAR strInput[MAX_PATH];
	m_listBoxResult.ResetContent();

	if (m_vecSelectedFiles.size())
	{
		for (size_t i = 0; i < m_vecSelectedFiles.size(); i++)
		{
			DoConvert(m_vecSelectedFiles[i]);
		}
	}
	else
	{
		//g_pFile = g_CreateFile("data\\source\\maps\\speedtreepreload.dat");
		CFileFind Find;
		sprintf_s(strInput,
			MAX_PATH,
			"%s*.*",
			m_strDirectory);

		BOOL bFound = Find.FindFile(strInput);
		while (bFound)
		{
			bFound = Find.FindNextFile();
			if (Find.IsDirectory())
			{
				CString strDirName = Find.GetFileName();
				if (strDirName != "." && strDirName != "..")
				{
					FindFilesInSubDir(Find.GetFilePath());
				}
			}
			else
			{
				CString strFileName = Find.GetFilePath();
				strFileName.MakeLower();
				TCHAR strExt[MAX_PATH];
				_splitpath(strFileName, NULL, NULL, NULL, strExt);

				if (!strcmp(strExt, ".stree"))
				{
					DoConvert(strFileName);
				}
			}
		}
		Find.Close();
		//g_pFile->Close();
	}
	m_vecSelectedFiles.clear();
	m_listBoxResult.SetHorizontalExtent(MAX_PATH * 4);
}

void KGDLG_SPEEDTREE_CONVERTER::OnBnClickedButtonBrowse()
{
	HRESULT hr = E_FAIL;
	TCHAR strDirectory[MAX_PATH];
	BROWSEINFO BrowseInfo;
	LPITEMIDLIST pItemList;
	memset(&BrowseInfo, 0, sizeof(BROWSEINFO));
	sprintf(strDirectory,
		"%sData\\source\\maps_source",
		g_szDefWorkDirectory);

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
			sprintf(strDirectory, "%s%sData\\source\\maps_source\\", g_szDefWorkDirectory);
		}

		if (strDirectory[strlen(strDirectory) - 1] != '\\')
		{
			strcat(strDirectory, "\\");
		}
		m_strDirectory = strDirectory;
		UpdateData(FALSE);
	}
}

HRESULT KGDLG_SPEEDTREE_CONVERTER::DoConvert(LPCSTR strFileName)
{
	HRESULT hr = E_FAIL;
	TCHAR strOutputInfo[MAX_PATH];
    IEKG3DModel *pModel = NULL;
	IEKG3DModelSpeedTree *pModelSpeedTree = NULL;
    IEKG3DModelTable *pModelTable = NULL;
    IEKG3DEngineManager *pManager = g_GetEngineIEInterface();
	KG_PROCESS_ERROR(strFileName);

    //Get3DEngineInterface((void **)(&pManager));

    pManager->GetIEKG3DModelTable(&pModelTable);
    hr = pModelTable->IELoadResourceFromFile(strFileName, 0, 0, (IKG3DResourceBase**)&pModel);
	KG_COM_PROCESS_ERROR(hr);

    pModel->GetIEKG3DModelSpeedTree(&pModelSpeedTree);
	hr = pModelSpeedTree->SaveToFile(strFileName, 0);
	KG_COM_PROCESS_ERROR(hr);
	//TCHAR strTempDir[MAX_PATH];
	//memset(strTempDir,'\0',MAX_PATH);
	//strncpy(strTempDir,strFileName + strlen(g_szDefWorkDirectory),MAX_PATH);
	//g_pFile->Write(strTempDir,MAX_PATH);
	hr = S_OK;
Exit0:
	SAFE_RELEASE(pModel);
	if (FAILED(hr))
	{
		sprintf_s(strOutputInfo,
			MAX_PATH,
			"%s 转换失败.",
			strFileName);
	}
	else
	{
		sprintf_s(strOutputInfo,
			MAX_PATH,
			"%s 转换成功.",
			strFileName);
	}
	m_listBoxResult.AddString(strOutputInfo);
	return hr;
}


