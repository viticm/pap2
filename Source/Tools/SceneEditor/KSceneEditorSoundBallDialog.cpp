// KSceneEditorSoundBallDialog.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KSceneEditorSoundBallDialog.h"
#include "Engine.h"
#include "IKG3DSoundShell.h"
// KSceneEditorSoundBallDialog dialog

static int s_LoopControlIDs[] = 
{
	IDC_STATIC_RATION,
	IDC_EDIT_RATION,
	IDC_STATIC_SPAN,
	IDC_EDIT_SPAN,
};

static int s_TimeControlIDs[] = 
{
	IDC_STATIC_START,
	IDC_EDIT_START,
	IDC_STATIC_END,
	IDC_EDIT_END
};

static const TCHAR *s_strConfigFileName = "data\\public\\SoundBallEditor.ini";
static const TCHAR *s_strConfigSectionName = "SoundBallEditor";
static const TCHAR *s_strDefaultPath = "data\\sound\\场景";

IMPLEMENT_DYNAMIC(KSceneEditorSoundBallDialog, CDialog)

KSceneEditorSoundBallDialog::KSceneEditorSoundBallDialog(CWnd* pParent /*=NULL*/)
	: CDialog(KSceneEditorSoundBallDialog::IDD, pParent)
	, m_nSpan(0)
	, m_fRation(0)
	, m_nStart(0)
	, m_nEnd(0)
	, m_strPathName(_T(""))
	, m_dwPreviewSound(s_dwInvalidateSoundID)
	, m_fRadius(0)
{
}

BOOL KSceneEditorSoundBallDialog::OnInitDialog()
{
	IIniFile *pConfig = NULL;
	TCHAR strDefaultPath[MAX_PATH];
	CDialog::OnInitDialog();

	pConfig = g_OpenIniFile(s_strConfigFileName, true, true);
	KG_PROCESS_ERROR(pConfig);

	pConfig->GetString(s_strConfigSectionName, "DefaultDirectory", s_strDefaultPath, strDefaultPath, MAX_PATH);

	m_strDirectory = g_szDefWorkDirectory;
	m_strDirectory += strDefaultPath;

	FillTree();
	if (m_strDefaultOpen.size())//如果有默认打开的文件
	{
		HRESULT hResult = g_GetEngineManager()->GetSoundBallTable()->LoadFromFile(m_strDefaultOpen.c_str(), &m_SoundBallData);
		KG_COM_PROCESS_ERROR(hResult);

		UpdateUI();

		SetWindowText(m_strDefaultOpen.c_str());
	}
Exit0:
	SAFE_RELEASE(pConfig);
	return TRUE;
}

void KSceneEditorSoundBallDialog::UpdateUI()
{
	//更新界面
	m_strPathName = m_SoundBallData.strFileName;
	m_buttonLoop.SetCheck(m_SoundBallData.bLoop ? BST_CHECKED : BST_UNCHECKED);
	m_fRation = m_SoundBallData.fRation;
	m_fRadius = m_SoundBallData.fRadius;
	m_buttonEnableTime.SetCheck(m_SoundBallData.bTimeEnable ? BST_CHECKED : BST_UNCHECKED);
	m_nStart = m_SoundBallData.nTimeStart;
	m_nEnd = m_SoundBallData.nTimeEnd;
	m_nSpan = m_SoundBallData.nTimeCheckSpan;
	UpdateData(FALSE);
	OnBnClickedCheckTimeenable();
	OnBnClickedCheckLoop();
}

void KSceneEditorSoundBallDialog::UpdateFromUI()
{
	//更新数据
	UpdateData();
	strcpy_s(m_SoundBallData.strFileName, 
		MAX_PATH,
		m_strPathName);

	m_SoundBallData.bLoop = (m_buttonLoop.GetCheck() == BST_CHECKED);
	m_SoundBallData.fRation = m_fRation;
	m_SoundBallData.bTimeEnable = (m_buttonEnableTime.GetCheck() == BST_CHECKED);
	m_SoundBallData.nTimeStart = m_nStart;
	m_SoundBallData.nTimeEnd = m_nEnd;
	m_SoundBallData.nTimeCheckSpan = m_nSpan;
	m_SoundBallData.fRadius = m_fRadius;
}

KSceneEditorSoundBallDialog::~KSceneEditorSoundBallDialog()
{
	IIniFile *pConfig = NULL;
	TCHAR strDirectory[MAX_PATH];
	TCHAR strLower[MAX_PATH];
	TCHAR *pStart = strDirectory;
	pConfig = g_OpenIniFile(s_strConfigFileName, true, true);
	KG_PROCESS_ERROR(pConfig);

	strcpy_s(strDirectory, MAX_PATH, m_strDirectory.c_str());
	_strlwr_s(strDirectory, MAX_PATH);

	strcpy_s(strLower, MAX_PATH, g_szDefWorkDirectory);
	_strlwr_s(strLower);
	
	TCHAR *pFind = strstr(strDirectory, strLower);
	if (pFind)
	{
		pStart = pFind + strlen(strLower);
	}
	pConfig->WriteString(s_strConfigSectionName, "DefaultDirectory", pStart);
Exit0:
	pConfig->Save(s_strConfigFileName);
	SAFE_RELEASE(pConfig);
}

void KSceneEditorSoundBallDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_Tree);
	DDX_Check(pDX, IDC_CHECK_TIMEENABLE, m_bTimeEnable);
	DDX_Control(pDX, IDC_CHECK_LOOP, m_buttonLoop);
	DDX_Control(pDX, IDC_CHECK_TIMEENABLE, m_buttonEnableTime);
	DDX_Text(pDX, IDC_EDIT_RATION, m_fRation);
	DDX_Text(pDX, IDC_EDIT_SPAN, m_nSpan);
	DDX_Text(pDX, IDC_EDIT_START, m_nStart);
	DDX_Text(pDX, IDC_EDIT_END, m_nEnd);
	DDX_Text(pDX, IDC_EDIT_FileNAME, m_strPathName);
	DDX_Text(pDX, IDC_EDIT_RADIUS, m_fRadius);
}


BEGIN_MESSAGE_MAP(KSceneEditorSoundBallDialog, CDialog)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, &KSceneEditorSoundBallDialog::OnTvnSelchangedTree1)
	ON_BN_CLICKED(IDC_BUTTON_SETDIR, &KSceneEditorSoundBallDialog::OnBnClickedButtonSetdir)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE1, &KSceneEditorSoundBallDialog::OnNMDblclkTree1)
	ON_BN_CLICKED(IDC_CHECK_TIMEENABLE, &KSceneEditorSoundBallDialog::OnBnClickedCheckTimeenable)
	ON_BN_CLICKED(IDC_BUTTON_LOAD, &KSceneEditorSoundBallDialog::OnBnClickedButtonLoad)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &KSceneEditorSoundBallDialog::OnBnClickedButtonSave)
	ON_BN_CLICKED(IDC_CHECK_LOOP, &KSceneEditorSoundBallDialog::OnBnClickedCheckLoop)
	ON_BN_CLICKED(IDC_BUTTON_PLAY, &KSceneEditorSoundBallDialog::OnBnClickedButtonPlay)
	ON_BN_CLICKED(IDC_BUTTON_SAVEAS, &KSceneEditorSoundBallDialog::OnBnClickedButtonSaveas)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// KSceneEditorSoundBallDialog message handlers

void KSceneEditorSoundBallDialog::OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	HRESULT hResult = E_FAIL;
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	
	TCHAR strPathName[MAX_PATH];
	KG_PROCESS_ERROR(g_pSoundShell);

	hResult = GetSelectedItemFullName(strPathName);
	KG_COM_PROCESS_ERROR(hResult);

	PlaySound(strPathName);
Exit0:
	;
}

void KSceneEditorSoundBallDialog::OnBnClickedButtonSetdir()
{
	HRESULT hr = S_OK;
	TCHAR strDirectory[MAX_PATH];
	BROWSEINFO BrowseInfo;
	LPITEMIDLIST pItemList;
	memset(&BrowseInfo, 0, sizeof(BROWSEINFO));
	sprintf(strDirectory,
		"%sData",
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
			sprintf(strDirectory, "%sData\\", g_szDefWorkDirectory);
		}

		if (strDirectory[strlen(strDirectory) - 1] != '\\')
		{
			strcat(strDirectory, "\\");
		}
		m_strDirectory = strDirectory;
		UpdateData(FALSE);
	}

	FillTree();
}



HRESULT KSceneEditorSoundBallDialog::FillTree()
{
	m_Tree.DeleteAllItems();

	FillTreeWithDir(m_strDirectory.c_str(),TVI_ROOT);

	return S_OK;
}

HRESULT KSceneEditorSoundBallDialog::FillTreeWithDir(LPCSTR Dir,HTREEITEM hStart)
{
	TCHAR path_buffer[256];
	TCHAR drive[256];
	TCHAR dir[256];
	TCHAR fname[256];
	TCHAR ext[256];

	WIN32_FIND_DATA Find;
	TCHAR str[256];
	wsprintf(str,"%s\\*.*",Dir);

	HANDLE hFind = FindFirstFile(str,&Find);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		wsprintf(path_buffer,"%s",Find.cFileName);
		_splitpath( path_buffer, drive, dir, fname, ext );
		wsprintf(str,"%s%s",fname,ext);

		if((_strcmpi(path_buffer,".")!=0)&&(_strcmpi(path_buffer,"..")!=0))
		{
			wsprintf(str,"%s%s",Dir,path_buffer);
			DWORD Attrib = GetFileAttributes(str);

			if((Attrib!=0xFFFFFFFF)&&(Attrib&FILE_ATTRIBUTE_DIRECTORY)&&(!(Attrib&FILE_ATTRIBUTE_HIDDEN)))
			{
				HTREEITEM hItem = m_Tree.InsertItem(fname,1,1,hStart);
				wsprintf(path_buffer,"%s\\",str);
				FillTreeWithDir(path_buffer,hItem);
			}
			else if(_strcmpi(ext,".Wav")==0)
			{
				HTREEITEM hItem = m_Tree.InsertItem(fname,0,0,hStart);
			}
		}

		while(FindNextFile(hFind,&Find))
		{
			wsprintf(path_buffer,"%s",Find.cFileName);
			_splitpath( path_buffer, drive, dir, fname, ext );
			wsprintf(str,"%s%s",fname,ext);
			if((_strcmpi(path_buffer,".")!=0)&&(_strcmpi(path_buffer,"..")!=0))
			{
				wsprintf(str,"%s%s",Dir,path_buffer);
				DWORD Attrib = GetFileAttributes(str);

				if((Attrib!=0xFFFFFFFF)&&(Attrib&FILE_ATTRIBUTE_DIRECTORY)&&(!(Attrib&FILE_ATTRIBUTE_HIDDEN)))
				{
					HTREEITEM hItem = m_Tree.InsertItem(fname,1,1,hStart);
					wsprintf(path_buffer,"%s\\",str);
					FillTreeWithDir(path_buffer,hItem);
				}
				else if(_strcmpi(ext,".Wav")==0)
				{
					HTREEITEM hItem = m_Tree.InsertItem(fname,0,0,hStart);
				}
			}
		}
		FindClose(hFind);
	}
	return S_OK;
}

HRESULT KSceneEditorSoundBallDialog::GetSelectedItemFullName(TCHAR FullName[])
{
	TCHAR szName[MAX_PATH];
	int nImage = 0;
	HTREEITEM hItem = m_Tree.GetSelectedItem();
	KG_PROCESS_ERROR(hItem);

	m_Tree.GetItemImage(hItem,nImage,nImage);
	KG_PROCESS_ERROR(nImage == 0);

	wsprintf(szName,"%s", m_Tree.GetItemText(hItem));
	HTREEITEM hParent = m_Tree.GetParentItem(hItem);
	while (hParent)
	{
		TCHAR AAA[MAX_PATH];
		wsprintf(AAA,"%s\\%s",m_Tree.GetItemText(hParent),szName);
		wsprintf(szName,"%s",AAA);

		hParent = m_Tree.GetParentItem(hParent);
	}

	wsprintf(FullName,"%s%s.wav",m_strDirectory.c_str(),szName);
	return S_OK;
Exit0:
	return E_FAIL;
}

void KSceneEditorSoundBallDialog::OnNMDblclkTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	HRESULT hResult = E_FAIL;
	TCHAR strPathName[MAX_PATH];
	TCHAR strAppDirectory[MAX_PATH];
	IEKG3DEngineManager *pManager = NULL;

	hResult = GetSelectedItemFullName(strPathName);
	KG_COM_PROCESS_ERROR(hResult);

	pManager = g_GetEngineIEInterface();
	KG_PROCESS_ERROR(pManager);
	
	pManager->GetDefWorkDirectory(strAppDirectory, MAX_PATH);
	pManager->GetRelativePath(strAppDirectory, strPathName, m_SoundBallData.strFileName);
	
	UpdateUI();
Exit0:
	*pResult = 0;
}


void KSceneEditorSoundBallDialog::OnBnClickedCheckTimeenable()
{
	BOOL bEnable = (m_buttonEnableTime.GetCheck() == BST_CHECKED);
	for (int i = 0; i < sizeof(s_TimeControlIDs) / sizeof(int); i++)
	{
		GetDlgItem(s_TimeControlIDs[i])->EnableWindow(bEnable);
	}
}

void KSceneEditorSoundBallDialog::OnBnClickedButtonLoad()
{
	HRESULT hResult = E_FAIL;
	HRESULT hRetCode = E_FAIL;
	CFileDialog dlg(true, 
		"SoundBall", 
		0, 
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
		"剑网三 音效球文件(*.SoundBall)|*.SoundBall||");
	
	KG_PROCESS_SUCCESS(dlg.DoModal() != IDOK);
	
	hResult = g_GetEngineManager()->GetSoundBallTable()->LoadFromFile(dlg.GetPathName(), &m_SoundBallData);
	KG_COM_PROCESS_ERROR(hResult);

	m_strDefaultOpen = dlg.GetPathName();
	SetWindowText(m_strDefaultOpen.c_str());

	UpdateUI();
Exit1:
	hRetCode = S_OK;
Exit0:
	if (FAILED(hRetCode))
	{
		TCHAR strErrInfo[MAX_PATH << 1];
		sprintf_s(strErrInfo,
			"加载%s失败.",
			dlg.GetPathName());
		MessageBox("错误", strErrInfo, MB_OK);
	}
}

void KSceneEditorSoundBallDialog::OnBnClickedButtonSave()
{
	HRESULT hResult = E_FAIL;
	HRESULT hRetCode = E_FAIL;
	TCHAR strFileName[MAX_PATH];

	UpdateFromUI();
	if (m_strDefaultOpen.size())
	{
		//如果是有默认打开的直接保存
		strcpy_s(strFileName, 
			MAX_PATH,
			m_strDefaultOpen.c_str());
	}
	else
	{
		CFileDialog dlg(FALSE, 
		"SoundBall", 
		0, 
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
		"剑网三 音效球文件(*.SoundBall)|*.SoundBall||");
	
		KG_PROCESS_SUCCESS(dlg.DoModal() != IDOK);

		strcpy_s(strFileName,
			MAX_PATH,
			dlg.GetPathName());
	}

	LPCSTR strStart = strstr(strFileName, g_szDefExeDirectory);
	if (strStart)
	{
		strStart += strlen(g_szDefExeDirectory);
	}
	else
	{
		strStart = strFileName;
	}

	hResult = g_GetEngineManager()->GetSoundBallTable()->SaveToFile(strFileName, &m_SoundBallData);
	KG_COM_PROCESS_ERROR(hResult);

	g_GetEngineManager()->GetSoundBallTable()->Refresh(strStart);

	m_strDefaultOpen = strStart;
	SetWindowText(m_strDefaultOpen.c_str());
Exit1:
	hRetCode = S_OK;
Exit0:
	if (FAILED(hRetCode))
	{
		TCHAR strErrInfo[MAX_PATH << 1];
		sprintf_s(strErrInfo,
			"保存%s失败.",
			strFileName);
		MessageBox("错误", strErrInfo, MB_OK);
	}
}

void KSceneEditorSoundBallDialog::OnBnClickedButtonSaveas()
{
	HRESULT hResult = E_FAIL;
	HRESULT hRetCode = E_FAIL;
	TCHAR strFileName[MAX_PATH];

	UpdateFromUI();
	CFileDialog dlg(FALSE, 
		"SoundBall", 
		0, 
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
		"剑网三 音效球文件(*.SoundBall)|*.SoundBall||");

	KG_PROCESS_SUCCESS(dlg.DoModal() != IDOK);

	strcpy_s(strFileName,
		MAX_PATH,
		dlg.GetPathName());

	LPCSTR strStart = strstr(strFileName, g_szDefExeDirectory);
	if (strStart)
	{
		strStart += strlen(g_szDefExeDirectory);
	}
	else
	{
		strStart = strFileName;
	}

	hResult = g_GetEngineManager()->GetSoundBallTable()->SaveToFile(strFileName, &m_SoundBallData);
	KG_COM_PROCESS_ERROR(hResult);

	g_GetEngineManager()->GetSoundBallTable()->Refresh(strStart);

	m_strDefaultOpen = strStart;
	SetWindowText(m_strDefaultOpen.c_str());
Exit1:
	hRetCode = S_OK;
Exit0:
	
	if (FAILED(hRetCode))
	{
		TCHAR strErrInfo[MAX_PATH << 1];
		sprintf_s(strErrInfo,
			"保存%s失败.",
			strFileName);
		MessageBox("错误", strErrInfo, MB_OK);
	}
}


void KSceneEditorSoundBallDialog::OnBnClickedCheckLoop()
{
	BOOL bEnable = (m_buttonLoop.GetCheck() == BST_UNCHECKED);
	for (int i = 0; i < sizeof(s_LoopControlIDs) / sizeof(int); i++)
	{
		GetDlgItem(s_LoopControlIDs[i])->EnableWindow(bEnable);
	}
}

void KSceneEditorSoundBallDialog::OnOK()
{
	UpdateFromUI();	
}

void KSceneEditorSoundBallDialog::PlaySound(LPCSTR strFileName)
{
	KG_PROCESS_ERROR(g_pSoundShell);
	if (m_dwPreviewSound != s_dwInvalidateSoundID && g_pSoundShell->IsPlayingSound(m_dwPreviewSound))
	{
		g_pSoundShell->StopSound(m_dwPreviewSound);
		m_dwPreviewSound = s_dwInvalidateSoundID;
	}
	g_pSoundShell->PlaySound(1, strFileName, 0, 1.0f, FALSE, &m_dwPreviewSound);
Exit0:
	;
}

void KSceneEditorSoundBallDialog::OnBnClickedButtonPlay()
{
	PlaySound(m_strPathName);
}

void KSceneEditorSoundBallDialog::OnClose()
{
	if (MessageBox("是否保存", "提示", MB_YESNO) == IDYES)
	{
		OnBnClickedButtonSave();
	}
	CDialog::OnClose();
}
