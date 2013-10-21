// KDlgMDLCheckTool.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KDlgMDLCheckTool.h"
#include "IEKG3DModelTable.h"
#include "IEKG3DAnimationTable.h"
#include "shlwapi.h"
#include "KDlgMDLCheckToolPathSetting.h"
#include "IEEditor.h"
#include "IEKG3DClip.h"
#include "IEKG3DClipTable.h"
//#include "SceneEditorCommon.h"
#include "KSceneModelEditorFrameWnd.h"

#define CheckEditorState() { KG_PROCESS_ERROR(m_pModelEditor); }
#define CheckModelState() { CheckEditorState(); m_pModelEditor->GetCurModel(&pModel); KG_PROCESS_ERROR(pModel);}

static bool s_bMakeHeadAsFace = true;
static const TCHAR* g_pModelType[] = { "*.mesh", "*.mdl", "*.ani"};
IMPLEMENT_DYNAMIC(KDlgMDLCheckTool, CDialog)

KDlgMDLCheckTool::KDlgMDLCheckTool(CWnd* pParent /*=NULL*/)
	: CDialog(KDlgMDLCheckTool::IDD, pParent)
	, m_strAnimationFilePath(_T(""))
	, m_strFaceFilePath(_T(""))
	, m_fFrameLength(0)
	, m_fAccAniFrameLength(0)
{
	LoadConfig();
	m_bFrameLengthChanged = FALSE;
	m_bAccFrameLengthChanged = FALSE;
	m_fFrameLength = 0.0f;
	m_bFullSized = TRUE;
	m_pAccModel = NULL;
}

KDlgMDLCheckTool::~KDlgMDLCheckTool()
{
	SaveConfig();
}

void KDlgMDLCheckTool::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_MDL, m_ctlMDLList);
	DDX_Control(pDX, IDC_LIST_SOCKET, m_ctlSocketList);
	DDX_Control(pDX, IDC_LIST_ITEM, m_ctlItemList);
	DDX_Control(pDX, IDC_LIST_ANIMATION, m_ctlAnimationList);
	DDX_Control(pDX, IDC_COMBO_FACE, m_ctlFaceList);
	DDX_Text(pDX, IDC_EDIT_MDL, m_strMDLFilePath);
	DDX_Text(pDX, IDC_EDIT_ITEM, m_strItemFilePath);
	DDX_Text(pDX, IDC_EDIT_ANIMATION, m_strAnimationFilePath);
	DDX_Text(pDX, IDC_EDIT_FACE, m_strFaceFilePath);
	DDX_Text(pDX, IDC_EDIT_ACCESSORIES, m_strAccFilePath);
	DDX_Text(pDX, IDC_EDIT_FRAMELENGTH, m_fFrameLength);
	DDX_Control(pDX, IDC_LIST_ACCESSORIES, m_ctlAccessory);
	DDX_Control(pDX, IDC_LIST_ACCANIMATION, m_ctlAccAnimation);
	DDX_Control(pDX, IDC_EDIT_ACCFRAMELENGTH, m_ctlAccAniFrameLength);
	DDX_Text(pDX, IDC_EDIT_ACCFRAMELENGTH, m_fAccAniFrameLength);
}


BEGIN_MESSAGE_MAP(KDlgMDLCheckTool, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_SETDIR, &KDlgMDLCheckTool::OnBnClickedButtonSetdir)
	ON_BN_CLICKED(IDC_BUTTON_REFRESH, &KDlgMDLCheckTool::OnBnClickedButtonRefresh)
	ON_LBN_SELCHANGE(IDC_LIST_MDL, &KDlgMDLCheckTool::OnLbnSelchangeListMdl)
	ON_LBN_SELCHANGE(IDC_LIST_SOCKET, &KDlgMDLCheckTool::OnLbnSelchangeListSocket)
	ON_LBN_SELCHANGE(IDC_LIST_ITEM, &KDlgMDLCheckTool::OnLbnSelchangeListItem)
	ON_LBN_SELCHANGE(IDC_LIST_ANIMATION, &KDlgMDLCheckTool::OnLbnSelchangeListAnimation)
	ON_CBN_SELCHANGE(IDC_COMBO_FACE, &KDlgMDLCheckTool::OnCbnSelchangeComboFace)
	ON_BN_CLICKED(IDC_BUTTON_CLEARBIND, &KDlgMDLCheckTool::OnBnClickedButtonClearbind)
	ON_MESSAGE(WM_FILEPATH, &KDlgMDLCheckTool::GetPath)
	ON_BN_CLICKED(IDC_BUTTON_ACCESSORIES, &KDlgMDLCheckTool::OnBnClickedButtonAccessories)
	ON_LBN_SELCHANGE(IDC_LIST_ACCESSORIES, &KDlgMDLCheckTool::OnLbnSelchangeListAccessories)
	ON_LBN_SELCHANGE(IDC_LIST_ACCANIMATION, &KDlgMDLCheckTool::OnLbnSelchangeListAccanimation)
	ON_BN_CLICKED(IDC_BUTTON_PLAY, &KDlgMDLCheckTool::OnBnClickedButtonPlay)
	ON_BN_CLICKED(IDC_BUTTON_SAVEAS, &KDlgMDLCheckTool::OnBnClickedButtonSaveas)
END_MESSAGE_MAP()


void KDlgMDLCheckTool::OnBnClickedButtonSetdir()
{
	KDlgMDLCheckToolPathSetting dlg(this);
	dlg.m_strItemFilePath = m_strItemPath;
	dlg.m_strMDLFilePath = m_strMDLPath;
	dlg.m_strAccFilePath = m_strAccPath;
	KG_PROCESS_SUCCESS(dlg.DoModal() == IDCANCEL);
	OnBnClickedButtonRefresh();
Exit1:
	;
}

LRESULT KDlgMDLCheckTool::GetPath(WPARAM wParam, LPARAM lParam)
{
	FilePath* pFilePath = (FilePath*)wParam;
	strcpy_s(m_strMDLPath, MAX_PATH, *pFilePath->strMDLPath);
	strcpy_s(m_strItemPath, MAX_PATH, *pFilePath->strItemPath);
	strcpy_s(m_strAccPath, MAX_PATH, *pFilePath->strAccPath);
	if (m_strMDLPath[strlen(m_strMDLPath) - 1] != '\\')
	{
		strcat(m_strMDLPath, "\\");
	}
	if (m_strItemPath[strlen(m_strItemPath) - 1] != '\\')
	{
		strcat(m_strItemPath, "\\");
	}
	if (m_strAccPath[strlen(m_strAccPath) - 1] != '\\')
	{
		strcat(m_strAccPath, "\\");
	}
	return S_OK;
}

void KDlgMDLCheckTool::OnBnClickedButtonRefresh()
{
	m_vecMdlFilePath.clear();
	m_vecItemFilePath.clear();
	m_ctlMDLList.ResetContent();
	m_ctlItemList.ResetContent();
	m_ctlSocketList.ResetContent();

	FindModelInDirectory(m_strMDLPath,
		TRUE,
		Model_MDL,
		m_vecMdlFilePath);

	FindModelInDirectory(m_strItemPath,
		TRUE,
		Model_Mesh,
		m_vecItemFilePath);
	for (size_t i = 0; i < m_vecMdlFilePath.size(); i++)
	{
		TCHAR strFileName[MAX_PATH];
		_splitpath_s(m_vecMdlFilePath[i].c_str(),
			NULL,
			0,
			NULL,
			0,
			strFileName,
			MAX_PATH,
			NULL,
			0);
		m_ctlMDLList.AddString(strFileName);
	}

	for (size_t i = 0; i < m_vecItemFilePath.size(); i++)
	{
		TCHAR strFileName[MAX_PATH];
		_splitpath_s(m_vecItemFilePath[i].c_str(),
			NULL,
			0,
			NULL,
			0,
			strFileName,
			MAX_PATH,
			NULL,
			0);
		int nIndex = m_ctlItemList.AddString(strFileName);
		m_ctlItemList.SetItemData(nIndex, (DWORD_PTR)i);
	}

	if (m_bFullSized)
	{
		m_vecAccFilePath.clear();
		m_vecAccAniFilePath.clear();
		m_ctlAccessory.ResetContent();
		m_ctlAccAnimation.ResetContent();

		FindModelInDirectory(m_strAccPath,
			TRUE,
			Model_Mesh,
			m_vecAccFilePath);

		for (size_t i = 0; i < m_vecAccFilePath.size(); i++)
		{
			TCHAR strFileName[MAX_PATH];
			_splitpath_s(m_vecAccFilePath[i].c_str(),
				NULL,
				0,
				NULL,
				0,
				strFileName,
				MAX_PATH,
				NULL,
				0);
			int nIndex = m_ctlAccessory.AddString(strFileName);
			m_ctlAccessory.SetItemData(nIndex, (DWORD_PTR)i);
		}
	}
}

void KDlgMDLCheckTool::FindModelInDirectory(LPCSTR strStartDir, 
										  BOOL bIncludeSubDir,
										  enuModelType Type,
										  std::vector<std::string>& vecStore)
{
	CFileFind Find;
	CFileFind FindMDL;
	TCHAR strStartFiles[MAX_PATH];
	KG_PROCESS_ERROR(strStartDir);
	sprintf_s(strStartFiles,
		MAX_PATH,
		"%s*.*",
		strStartDir);
	KG_PROCESS_ERROR(Find.FindFile(strStartFiles));
	BOOL bWorking = TRUE;
	while (bWorking)
	{
		bWorking = Find.FindNextFile();
		if (Find.IsDirectory() && 
			bIncludeSubDir && 
			!Find.IsDots())
		{
			TCHAR strSubPath[MAX_PATH];
			sprintf_s(strSubPath,
				MAX_PATH,
				"%s\\",
				Find.GetFilePath());
			FindModelInDirectory(strSubPath,
				bIncludeSubDir,
				Type,
				vecStore);
		}
	};
	sprintf_s(strStartFiles,
		MAX_PATH,
		"%s%s",
		strStartDir,
		g_pModelType[Type]);
	KG_PROCESS_ERROR(FindMDL.FindFile(strStartFiles));
	bWorking = TRUE; 
	while (bWorking)
	{
		bWorking = FindMDL.FindNextFile();
		TCHAR strFilePath[MAX_PATH];
		strcpy_s(strFilePath,
			MAX_PATH,
			FindMDL.GetFilePath());
		//////////////////////////////////////////////////////////////////////////
		TCHAR szAppName[MAX_PATH];
		TCHAR RetFileName[MAX_PATH];
		g_pEngineManager->GetDefWorkDirectory(szAppName, MAX_PATH);
		g_pEngineManager->GetRelativePath(szAppName,strFilePath,RetFileName);

		vecStore.push_back(RetFileName);
	};
	FindMDL.Close();
	Find.Close();
Exit0:
	;
}

void KDlgMDLCheckTool::LoadConfig()
{
	TCHAR strConfigFilePath[MAX_PATH];
	TCHAR strInputPath[MAX_PATH];
	TCHAR strDefaultInputPath[MAX_PATH];
	const TCHAR* strDefaultMDLPath = "Data\\source\\npc\\";
	const TCHAR* strDefaultItemPath = "Data\\source\\item\\";
	const TCHAR* strDefaultAccPath = "Data\\source\\other";
	sprintf_s(strConfigFilePath,
		MAX_PATH,
		"%sConfig.ini",
		g_szDefWorkDirectory);
	IIniFile* fpConfig = g_OpenIniFile(strConfigFilePath);
	KG_PROCESS_ERROR(fpConfig);
	
	//MDL
	sprintf_s(strDefaultInputPath, 
		MAX_PATH, 
		"%s%s", 
		g_szDefWorkDirectory,
		strDefaultMDLPath);
	fpConfig->GetString("MDLCheckTool", 
		"MDLPath", 
		strDefaultInputPath, 
		strInputPath, 
		MAX_PATH);
	if (!PathFileExists(strInputPath))
	{
		strcpy_s(m_strMDLPath, MAX_PATH, strDefaultInputPath);
	}
	else
	{
		strcpy_s(m_strMDLPath, MAX_PATH, strInputPath);
	}
	
	//Item
	sprintf_s(strDefaultInputPath, 
		MAX_PATH, 
		"%s%s", 
		g_szDefWorkDirectory,
		strDefaultItemPath);
	fpConfig->GetString("MDLCheckTool", "ItemPath", strDefaultInputPath, strInputPath, MAX_PATH);
	if (!PathFileExists(strInputPath))
	{
		strcpy_s(m_strItemPath, MAX_PATH, strDefaultInputPath);
	}
	else
	{
		strcpy_s(m_strItemPath, MAX_PATH, strInputPath);
	}
	//Accessory
	sprintf_s(strDefaultInputPath, 
		MAX_PATH, 
		"%s%s", 
		g_szDefWorkDirectory,
		strDefaultAccPath);
	fpConfig->GetString("MDLCheckTool", "AccessoryPath", strDefaultInputPath, strInputPath, MAX_PATH);
	if (!PathFileExists(strInputPath))
	{
		strcpy_s(m_strAccPath, MAX_PATH, strDefaultInputPath);
	}
	else
	{
		strcpy_s(m_strAccPath, MAX_PATH, strInputPath);
	}
	fpConfig->Release();
	return;
Exit0:
	KGLogPrintf(KGLOG_ERR, "KDlgMDLCheckTool save configuration failed.");
	;
}

void KDlgMDLCheckTool::SaveConfig()
{
	TCHAR strConfigFilePath[MAX_PATH];
	sprintf_s(strConfigFilePath,
		MAX_PATH,
		"%sConfig.ini",
		g_szDefWorkDirectory);
	IIniFile* fpConfig = g_OpenIniFile(strConfigFilePath);
	KG_PROCESS_ERROR(fpConfig);
	fpConfig->WriteString("MDLCheckTool", "MDLPath", m_strMDLPath);
	fpConfig->WriteString("MDLCheckTool", "ItemPath", m_strItemPath);
	fpConfig->WriteString("MDLCheckTool", "AccessoryPath", m_strAccPath);
	fpConfig->Save(strConfigFilePath);
	fpConfig->Release();
	return;
Exit0:
	KGLogPrintf(KGLOG_ERR, "KDlgMDLCheckTool save configuration failed.");
	;
}

void KDlgMDLCheckTool::OnLbnSelchangeListMdl()
{
	//change model to render and refill the socket list
	HRESULT hr = E_FAIL;
	IEKG3DModel* pModel = NULL;
    int nNumSocket = 0;
	CString strCurAni;
	int nCurSel = m_ctlMDLList.GetCurSel();
	KG_PROCESS_ERROR(nCurSel != LB_ERR);
	
	CheckEditorState();
	/*pModel = m_pModelEditor->GetCurModel();
	SAFE_RELEASE(pModel);*/

	hr = m_pModelEditor->OpenModel(m_vecMdlFilePath[nCurSel].c_str(), 
		NULL,
		NULL,
		FALSE,
		&pModel);

	KG_COM_PROCESS_ERROR(hr);

    hr = pModel->GetNumSocket(&nNumSocket);
    KGLOG_COM_PROCESS_ERROR(hr);
	m_ctlSocketList.ResetContent();
	for (int i = 0; i < nNumSocket; i++)
	{
		LPCSTR strSocketName = NULL;
        pModel->GetSocketName(i, &strSocketName);
		if (strSocketName)
		{
			m_ctlSocketList.AddString(strSocketName);
		}
	}

	int nCurSelAni = m_ctlAnimationList.GetCurSel();
	if (nCurSelAni != LB_ERR)
	{
		m_ctlAnimationList.GetText(nCurSelAni, strCurAni);
		strCurAni.MakeLower();
	}

	NotifyAnimationChange();
	FindAnimationFile(m_vecMdlFilePath[nCurSel].c_str(), strCurAni);
	FindMDLFacePart();
	m_strMDLFilePath = m_vecMdlFilePath[nCurSel].c_str();
	UpdateData(FALSE);

	KSceneModelEditorFrameWnd* pFrame = static_cast<KSceneModelEditorFrameWnd*>(GetModelFrameWnd());
	KG_PROCESS_ERROR(pFrame);
	pFrame->SendMessage(WM_USER_MODEL_CHANGE);
Exit0:
	;
}
void KDlgMDLCheckTool::NotifyAnimationChange()
{
	HRESULT hr = S_OK;
    IEKG3DClipTable *piClipTable = NULL;

    hr = g_pEngineManager->GetIEKG3DClipTable(&piClipTable);
    KGLOG_COM_PROCESS_ERROR(hr);

	if (m_bFrameLengthChanged)
	{
		int nReturn = MessageBox("动画关键帧已经改变，是否要保存？", "提示", MB_YESNO);
		if (nReturn == IDYES)
		{
			for (int i = 0; i < m_ctlAnimationList.GetCount(); i++)
			{
				if (m_vecAnimationChanged[i])
				{
					IEKG3DClip* pClip = NULL;
					piClipTable->LoadResourceFromFileInEditor(m_vecAnimationFilePath[i].c_str(), 
						0,
						0,
						(IEKG3DClip **)&pClip);
					if (!pClip)
						continue;
					hr = pClip->SaveToFile(m_vecAnimationFilePath[i].c_str(), 0);
					if (FAILED(hr))
					{
						TCHAR strOutputInfo[MAX_PATH << 1];
						sprintf_s(strOutputInfo, 
							MAX_PATH << 1,
							"保存文件%s失败.", m_vecAnimationFilePath[i].c_str());
						MessageBox(strOutputInfo, "错误");
					}
					else
					{
						TCHAR strOutputInfo[MAX_PATH << 1];
						sprintf_s(strOutputInfo, 
							MAX_PATH << 1,
							"保存文件%s成功.", m_vecAnimationFilePath[i].c_str());
							MessageBox(strOutputInfo, "成功");
					}
					SAFE_RELEASE(pClip);
				}
			}
		}
	}
	if (m_bAccFrameLengthChanged)
	{
		int nReturn = MessageBox("附件动画关键帧已经改变，是否要保存？", "提示", MB_YESNO);
		if (nReturn == IDYES)
		{
			for (int i = 0; i < m_ctlAccAnimation.GetCount(); i++)
			{
				if (m_vecAccAnimationChanged[i])
				{
					IEKG3DClip* pClip = NULL;
					piClipTable->LoadResourceFromFileInEditor(m_vecAccAniFilePath[i].c_str(), 
						0,
						0,
						(IEKG3DClip**)&pClip);
					if (!pClip)
						continue;
					hr = pClip->SaveToFile(m_vecAccAniFilePath[i].c_str(), 0);
					if (FAILED(hr))
					{
						TCHAR strOutputInfo[MAX_PATH << 1];
						sprintf_s(strOutputInfo, 
							MAX_PATH << 1,
							"保存文件%s失败.", m_vecAccAniFilePath[i].c_str());
						MessageBox(strOutputInfo, "错误");
					}
					else
					{
						TCHAR strOutputInfo[MAX_PATH << 1];
						sprintf_s(strOutputInfo, 
							MAX_PATH << 1,
							"保存文件%s成功.", m_vecAccAniFilePath[i].c_str());
						MessageBox(strOutputInfo, "成功");
					}
					SAFE_RELEASE(pClip);
				}
			}
		}
	}
	m_bAccFrameLengthChanged = FALSE;
	m_bFrameLengthChanged = FALSE;
Exit0:
    return;
}

void KDlgMDLCheckTool::FindAnimationFile(LPCSTR strMdlFilePath, 
										 LPCSTR strSelect /*= NULL*/)
{
	TCHAR strMdlFilter[MAX_PATH];
	TCHAR strDriver[MAX_PATH];
	TCHAR strPath[MAX_PATH];
	CFileFind Find;
	int nSel = -1;
	KG_PROCESS_ERROR(strMdlFilePath);
	m_ctlAnimationList.ResetContent();
	_splitpath_s(strMdlFilePath,
		strDriver,
		MAX_PATH,
		strPath,
		MAX_PATH,
		0,
		0,
		0,
		0);
	strPath[strlen(strPath) - 5] = '\0';
	sprintf_s(strMdlFilter,
		MAX_PATH,
		"%s%s动作\\*.ani",
		strDriver,
		strPath);
	m_vecAnimationFilePath.clear();
	KG_PROCESS_ERROR(Find.FindFile(strMdlFilter));
	BOOL bWorking = TRUE;
	while (bWorking)
	{
		bWorking = Find.FindNextFile();
		TCHAR strFilePath[MAX_PATH];
		strcpy_s(strFilePath, 
			MAX_PATH,
			Find.GetFilePath());
		m_vecAnimationFilePath.push_back(strFilePath);
	};

	m_ctlAnimationList.ResetContent();
	for (size_t i = 0; i < m_vecAnimationFilePath.size(); i++)
	{
		TCHAR strAnimationFileName[MAX_PATH];
		_splitpath_s(m_vecAnimationFilePath[i].c_str(),
			NULL,
			0,
			NULL,
			0,
			strAnimationFileName,
			MAX_PATH,
			NULL,
			0);
		m_ctlAnimationList.AddString(strAnimationFileName);
		if (nSel == -1 && strSelect)
		{
			TCHAR strLower[MAX_PATH];
			strcpy_s(strLower, MAX_PATH, strAnimationFileName);
			_strlwr_s(strLower);
			if (!strcmp(strLower, strSelect))
			{
				nSel = i;
			}
		}
	}

	if (m_vecAnimationFilePath.size())
	{
		if (nSel == -1)
		{
			nSel = 0;
		}
		m_ctlAnimationList.SetCurSel(nSel);
		OnLbnSelchangeListAnimation();
	}
	else
	{
		m_strAnimationFilePath = "";
	}

	m_vecAnimationChanged.clear();
	m_vecAnimationChanged.resize(m_vecAnimationFilePath.size(), FALSE);
	
Exit0:
	Find.Close();
	;
}

void KDlgMDLCheckTool::OnLbnSelchangeListSocket()
{
	
}

void KDlgMDLCheckTool::OnLbnSelchangeListItem()
{
	HRESULT hrRetCode = E_FAIL;
	IEKG3DModel *pModelItem = NULL;
	IEKG3DModel *pModel = NULL;
    LPCSTR strSocketName = NULL;
    IEKG3DModelTable *piModelTable = NULL;

	int nCurSelSocket = m_ctlSocketList.GetCurSel();
	KG_PROCESS_ERROR(nCurSelSocket != LB_ERR);
	int nCurSelItem = m_ctlItemList.GetCurSel();
	KG_PROCESS_ERROR(nCurSelItem != LB_ERR);

	int nItemIndex = (int)m_ctlItemList.GetItemData(nCurSelItem);

//	OnLbnSelchangeListMdl();

	CheckModelState();
	pModel->GetSocketName(nCurSelSocket, &strSocketName);
	KG_PROCESS_ERROR(strSocketName);

    hrRetCode = g_pEngineManager->GetIEKG3DModelTable(&piModelTable);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

	hrRetCode = piModelTable->IELoadResourceFromFile(m_vecItemFilePath[nItemIndex].c_str(), 
		0,
		0,
		(IKG3DResourceBase**)&pModelItem);
	KG_COM_PROCESS_ERROR(hrRetCode);
	pModel->UnbindChild(strSocketName);
	pModelItem->BindToSocket(pModel, strSocketName);
	m_strItemFilePath = m_vecItemFilePath[nItemIndex].c_str();
	UpdateData(FALSE);
Exit0:
	;
}

void KDlgMDLCheckTool::OnLbnSelchangeListAnimation()
{
	IEKG3DModel *pModel = NULL;
	IEKG3DClip *pClip = NULL;
    IEKG3DClipTable *piClipTable = NULL;
	CheckModelState();
	int nCurSelAnimation = m_ctlAnimationList.GetCurSel();
	KG_PROCESS_ERROR(nCurSelAnimation != LB_ERR);
	pModel->PlayAnimation(ENU_ANIMATIONPLAY_CIRCLE, 
		m_vecAnimationFilePath[nCurSelAnimation].c_str(),
		1.0f,
		0,
		NULL,
		NULL,
		ANICTL_PRIMARY);
	m_strAnimationFilePath = m_vecAnimationFilePath[nCurSelAnimation].c_str();
	UpdateData(FALSE);
    
    g_pEngineManager->GetIEKG3DClipTable(&piClipTable);
    piClipTable->LoadResourceFromFileInEditor(m_vecAnimationFilePath[nCurSelAnimation].c_str(),
		0,
		0,
		(IEKG3DClip**)&pClip);
	pClip->GetFrameLength(&m_fFrameLength);
	//>GetBaseClipFrameLength(0, &m_fFrameLength);
	UpdateData(FALSE);
Exit0:
	SAFE_RELEASE(pClip);
	;
}


BOOL KDlgMDLCheckTool::OnInitDialog()
{
	CDialog::OnInitDialog();
	RECT rect;
	GetWindowRect(&rect);
	m_nFullLength = rect.right - rect.left;
	OnBnClickedButtonAccessories();
	return TRUE;
}

void KDlgMDLCheckTool::FindMDLFacePart()
{
	CFileFind Find;
	int nCurSelMDL = m_ctlMDLList.GetCurSel();
	TCHAR strFaceFilter[MAX_PATH];
	TCHAR strDriver[MAX_PATH];
	TCHAR strPath[MAX_PATH];
	m_vecFaceFilePath.clear();
	m_ctlFaceList.ResetContent();
	KG_PROCESS_ERROR(nCurSelMDL != LB_ERR);
	
	_splitpath_s(m_vecMdlFilePath[nCurSelMDL].c_str(),
		strDriver,
		MAX_PATH,
		strPath,
		MAX_PATH,
		NULL,
		0,
		NULL,
		0);
	
	sprintf_s(strFaceFilter,
		MAX_PATH,
		"%s%s*face*.mesh",
		strDriver,
		strPath);

	BOOL bWorking = Find.FindFile(strFaceFilter);
	while (bWorking)
	{
		bWorking = Find.FindNextFile();
		TCHAR strFilePath[MAX_PATH];
		strcpy_s(strFilePath, 
			MAX_PATH,
			Find.GetFilePath());
		m_vecFaceFilePath.push_back(strFilePath);
	}

	for (size_t i = 0; i < m_vecFaceFilePath.size(); i++)
	{
		TCHAR strFaceName[MAX_PATH];
		_splitpath_s(m_vecFaceFilePath[i].c_str(),
			NULL,
			0,
			NULL,
			0,
			strFaceName,
			MAX_PATH,
			NULL,
			0);
		m_ctlFaceList.AddString(strFaceName);
	}

	if (s_bMakeHeadAsFace)
	{
		size_t uStart = m_vecFaceFilePath.size();
		//把头也加入到face列表中
		sprintf_s(strFaceFilter,
			MAX_PATH,
			"%s%s*head*.mesh",
			strDriver,
			strPath);

		BOOL bWorking = Find.FindFile(strFaceFilter);
		while (bWorking)
		{
			bWorking = Find.FindNextFile();
			TCHAR strFilePath[MAX_PATH];
			strcpy_s(strFilePath, 
				MAX_PATH,
				Find.GetFilePath());
			m_vecFaceFilePath.push_back(strFilePath);
		}

		for (size_t i = uStart; i < m_vecFaceFilePath.size(); i++)
		{
			TCHAR strFaceName[MAX_PATH];
			_splitpath_s(m_vecFaceFilePath[i].c_str(),
				NULL,
				0,
				NULL,
				0,
				strFaceName,
				MAX_PATH,
				NULL,
				0);
			m_ctlFaceList.AddString(strFaceName);
		}

	}
	/*if (m_vecFaceFilePath.size())
	{
		m_ctlFaceList.SetCurSel(0);
		OnCbnSelchangeComboFace();
	}
	else*/
	{
		m_strFaceFilePath = "";
	}

Exit0:
	Find.Close();
	;
}

void KDlgMDLCheckTool::OnCbnSelchangeComboFace()
{
    HRESULT hrRetCode = E_FAIL;
	IEKG3DModel *pModel = NULL;
    IEKG3DModel *pFaceModel = NULL;
    IEKG3DModelTable *piModelTable = NULL;
	int nCurSel = m_ctlFaceList.GetCurSel();
	KG_PROCESS_ERROR(nCurSel != CB_ERR);
	CheckModelState();
	
    hrRetCode = g_pEngineManager->GetIEKG3DModelTable(&piModelTable);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);
	hrRetCode = piModelTable->IELoadResourceFromFile(m_vecFaceFilePath[nCurSel].c_str(),
		0,
		0,
		(IKG3DResourceBase**)&pFaceModel);
	KG_COM_PROCESS_ERROR(hrRetCode);

	pModel->UnbindChild("S_Face");
	pFaceModel->BindToSocket(pModel, "S_Face");
	m_strFaceFilePath = m_vecFaceFilePath[nCurSel].c_str();
	UpdateData(FALSE);
Exit0:
	;
}

void KDlgMDLCheckTool::OnBnClickedButtonClearbind()
{
	IEKG3DModel *pModel = NULL;
    int nNumSocket = 0;
    LPCTSTR pcszSocketName = NULL;

	CheckModelState();

    pModel->GetNumSocket(&nNumSocket);
	for (int i = 0; i < nNumSocket; i++)
	{
		pModel->GetSocketName(i, &pcszSocketName);
        pModel->UnbindChild(pcszSocketName);
	}
Exit0:
	;
}

void KDlgMDLCheckTool::OnOK()
{
	HRESULT hr = E_FAIL;
	IEKG3DClip *pClip = NULL;
    IEKG3DClipTable *piClipTable = NULL;
    FLOAT fBaseClipFrameLength = 0.0;

    hr = g_pEngineManager->GetIEKG3DClipTable(&piClipTable);
    KGLOG_COM_PROCESS_ERROR(hr);

	UpdateData();
	int nCurSel = m_ctlAnimationList.GetCurSel();
	if (nCurSel != LB_ERR)
    {
        hr = piClipTable->LoadResourceFromFileInEditor(m_vecAnimationFilePath[nCurSel].c_str(),
			0,
			0,
			(IEKG3DClip **)&pClip);
		KG_COM_PROCESS_ERROR(hr);

        //hr = pAnimation->GetBaseClipFrameLength(0, &fBaseClipFrameLength);
		hr = pClip->GetFrameLength(&fBaseClipFrameLength); 
        KGLOG_COM_PROCESS_ERROR(hr);

        if (abs(m_fFrameLength - fBaseClipFrameLength) > 0.001f)
		{
			//pAnimation->SetBaseClipInfo(0, SETBASECLIPFLAG_FRAMELENGTH, m_fFrameLength, NULL);
			pClip->SetFrameLength(m_fFrameLength);
			m_vecAnimationChanged[nCurSel] = TRUE;
			m_bFrameLengthChanged = TRUE;
		}
	}

	IEKG3DClip *pAccClip = NULL;
	int nCurAccSel = m_ctlAccAnimation.GetCurSel();
	if (nCurAccSel != LB_ERR)
	{
		hr = piClipTable->LoadResourceFromFileInEditor(m_vecAccAniFilePath[nCurAccSel].c_str(),
			0,
			0,
			(IEKG3DClip **)&pAccClip);
		KG_COM_PROCESS_ERROR(hr);

        //hr = pAccAnimation->GetBaseClipFrameLength(0, &fBaseClipFrameLength);
		hr = pAccClip->GetFrameLength(&fBaseClipFrameLength);
        KGLOG_COM_PROCESS_ERROR(hr);

        if (abs(m_fAccAniFrameLength - fBaseClipFrameLength) > 0.001f)
		{
			//pAccAnimation->SetBaseClipInfo(0, SETBASECLIPFLAG_FRAMELENGTH, m_fAccAniFrameLength, NULL);
			pAccClip->SetFrameLength(m_fAccAniFrameLength);
			m_vecAccAnimationChanged[nCurSel] = TRUE;
			m_bFrameLengthChanged = TRUE;
		}
	}
Exit0:
	SAFE_RELEASE(pClip);
	;
}

void KDlgMDLCheckTool::OnCancel()
{
	NotifyAnimationChange();
	CDialog::OnCancel();
}

void KDlgMDLCheckTool::OnBnClickedButtonAccessories()
{
	RECT rect, rectBorder;
	m_bFullSized = !m_bFullSized;
	GetWindowRect(&rect);
	m_ctlAccessory.GetWindowRect(&rectBorder);
	if (m_bFullSized)
	{
		rect.right = rect.left + m_nFullLength;
		m_vecAccFilePath.clear();
		m_vecAccAniFilePath.clear();
		m_ctlAccessory.ResetContent();
		m_ctlAccAnimation.ResetContent();

		FindModelInDirectory(m_strAccPath,
			TRUE,
			Model_Mesh,
			m_vecAccFilePath);

		for (size_t i = 0; i < m_vecAccFilePath.size(); i++)
		{
			TCHAR strFileName[MAX_PATH];
			_splitpath_s(m_vecAccFilePath[i].c_str(),
				NULL,
				0,
				NULL,
				0,
				strFileName,
				MAX_PATH,
				NULL,
				0);
			int nIndex = m_ctlAccessory.AddString(strFileName);
			m_ctlAccessory.SetItemData(nIndex, (DWORD_PTR)i);
		}
	}
	else
	{
		rect.right = rectBorder.left;
	}
	m_ctlAccessory.EnableWindow(m_bFullSized);
	m_ctlAccAnimation.EnableWindow(m_bFullSized);
	m_ctlAccAniFrameLength.EnableWindow(m_bFullSized);
	m_ctlAccessory.ShowWindow(m_bFullSized);
	m_ctlAccAnimation.ShowWindow(m_bFullSized);
	m_ctlAccAniFrameLength.ShowWindow(m_bFullSized);
	MoveWindow(&rect);
}

void KDlgMDLCheckTool::OnLbnSelchangeListAccessories()
{
	HRESULT hrRetCode = E_FAIL;
	TCHAR strDriver[MAX_PATH];
	TCHAR strPath[MAX_PATH];
	TCHAR strAniPath[MAX_PATH];
	m_vecAccAniFilePath.clear();
	int nCurSel = m_ctlAccessory.GetCurSel();
	KG_PROCESS_ERROR(nCurSel != LB_ERR);
	int nAccIndex = (int)m_ctlAccessory.GetItemData(nCurSel);
	
	hrRetCode = m_pModelEditor->RemoveModel(m_pAccModel);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

	hrRetCode = m_pModelEditor->OpenModel(m_vecAccFilePath[nAccIndex].c_str(), 
		NULL,
		NULL,
		TRUE,
		(IEKG3DModel**)&m_pAccModel);
	KGLOG_COM_PROCESS_ERROR(hrRetCode);
	NotifyAnimationChange();
	m_ctlAccAnimation.ResetContent();
	m_vecAccAnimationChanged.clear();
	m_strAccFilePath = m_vecAccFilePath[nAccIndex].c_str();
	_splitpath_s(m_vecAccFilePath[nAccIndex].c_str(),
		strDriver,
		MAX_PATH,
		strPath,
		MAX_PATH,
		NULL,
		0,
		NULL,
		0);
	sprintf_s(strAniPath,
		MAX_PATH,
		"%s%s",
		strDriver,
		strPath);
	FindModelInDirectory(strAniPath, FALSE, Model_Ani, m_vecAccAniFilePath);
	for (size_t i = 0; i < m_vecAccAniFilePath.size(); i++)
	{
		TCHAR strFileName[MAX_PATH];
		_splitpath_s(m_vecAccAniFilePath[i].c_str(), 
			NULL,
			0,
			NULL,
			0,
			strFileName,
			MAX_PATH,
			NULL,
			0);
		m_ctlAccAnimation.AddString(strFileName);
	}
	m_vecAccAnimationChanged.resize(m_vecAccAniFilePath.size(), FALSE);
	UpdateData(FALSE);
Exit0:
	return;
}

void KDlgMDLCheckTool::OnLbnSelchangeListAccanimation()
{
	IEKG3DClip *pClip = NULL;
    IEKG3DClipTable *piClipTable = NULL;
    g_pEngineManager->GetIEKG3DClipTable(&piClipTable);
    
	int nCurSel = m_ctlAccAnimation.GetCurSel();
	KG_PROCESS_ERROR(nCurSel != LB_ERR);
	m_pAccModel->PlayAnimation(ENU_ANIMATIONPLAY_CIRCLE, m_vecAccAniFilePath[nCurSel].c_str(), 1.0f, 0, NULL, NULL, ANICTL_PRIMARY);
	piClipTable->LoadResourceFromFileInEditor(m_vecAccAniFilePath[nCurSel].c_str(),
		0,
		0,
		(IEKG3DClip**)&pClip);
	pClip->GetFrameLength(&m_fAccAniFrameLength);
	UpdateData(FALSE);
Exit0:
	SAFE_RELEASE(pClip);
	return;
}

void KDlgMDLCheckTool::OnBnClickedButtonPlay()
{
	HRESULT hrRetCode =E_FAIL;
    IEKG3DModel *pModel = NULL;

    KG_PROCESS_ERROR(m_pModelEditor);
    hrRetCode = m_pModelEditor->GetCurModel(&pModel);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);
	KG_PROCESS_ERROR(pModel);
	int nCurSel = m_ctlAnimationList.GetCurSel();
	if (nCurSel != LB_ERR)
	{
		pModel->PlayAnimation(ENU_ANIMATIONPLAY_CIRCLE, 
			m_vecAnimationFilePath[nCurSel].c_str(), 
			1.0f, 
			0,
			NULL,
			NULL,
			ANICTL_PRIMARY);
	}
	KG_PROCESS_ERROR(m_pAccModel);
	nCurSel = m_ctlAccAnimation.GetCurSel();
	if (nCurSel != LB_ERR)
	{
		m_pAccModel->PlayAnimation(ENU_ANIMATIONPLAY_CIRCLE,
			m_vecAccAniFilePath[nCurSel].c_str(),
			1.0f,
			0,
			NULL,
			NULL,
			ANICTL_PRIMARY);
	}
Exit0:
	;
}

void KDlgMDLCheckTool::OnBnClickedButtonSaveas()
{
	TCHAR curDir[MAX_PATH * 2] = {_T('\0')};
	::GetCurrentDirectory(_countof(curDir), curDir);
	IEKG3DModel* pModel = NULL;

	m_pModelEditor->GetCurModel(&pModel);
	if(!pModel)
		return;

	TCHAR strOutputInfo[MAX_PATH << 1];
	CFileDialog dlgSave(FALSE, "ModelST", 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "ModelST文件(*.ModelST)|*.ModelST||");
	CString strFileName;
	KG_PROCESS_SUCCESS(dlgSave.DoModal() != IDOK);
	strFileName = dlgSave.GetPathName();
	HRESULT hr = pModel->SaveToFile(strFileName, 0);	
	if (SUCCEEDED(hr))
	{
		sprintf_s(strOutputInfo, MAX_PATH << 1, "保存%s成功.请不要移动MDL文件的位置.", strFileName);
		MessageBox(strOutputInfo, "提示");
	}
	else
	{
		sprintf_s(strOutputInfo, MAX_PATH << 1, "保存%s失败.", strFileName);
		MessageBox(strOutputInfo, "提示");
	}

	::SetCurrentDirectory(curDir);
Exit1:
	;
}
