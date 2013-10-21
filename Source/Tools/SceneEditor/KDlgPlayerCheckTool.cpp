// KDlgPlayerCheckTool.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KDlgPlayerCheckTool.h"
#include "KSceneModelEditorFrameWnd.h"
#include "IEKG3DModelTable.h"
#include "IEKG3DAnimationTable.h"
#include "IEKG3DClip.h"
#include "IEKG3DClipTable.h"
#include "KSceneModelEditorAnimationPanel.h"

const TCHAR* KDlgPlayerCheckTool::m_strConfigFilePath = "data\\public\\PlayerCheckTool\\PlayCheckTool.ini";
const TCHAR* KDlgPlayerCheckTool::m_strDirectorySection = "Directory";
const TCHAR* KDlgPlayerCheckTool::m_strPlayerTypeSection = "PlayerType";
const TCHAR* KDlgPlayerCheckTool::m_strPartSection = "Part";
const TCHAR* KDlgPlayerCheckTool::m_strSocketSection = "Socket";
const TCHAR* KDlgPlayerCheckTool::m_strAutoBindSection = "AutoBind";

int KDlgPlayerCheckTool::m_nHeadIndex;
int KDlgPlayerCheckTool::m_nBodyIndex;
int KDlgPlayerCheckTool::m_nFaceIndex;
int KDlgPlayerCheckTool::m_nHatIndex;
int KDlgPlayerCheckTool::m_nHandIndex;

std::vector<std::string> KDlgPlayerCheckTool::m_strSuffix;
std::vector<std::string> KDlgPlayerCheckTool::m_strSockets;
std::vector<std::string> KDlgPlayerCheckTool::m_PatternPathName;
std::vector<std::string> KDlgPlayerCheckTool::m_PatternPath;
std::vector<std::string> KDlgPlayerCheckTool::m_StandardPath;
std::vector<KDlgPlayerCheckTool::AutoBind> KDlgPlayerCheckTool::m_AutoBind;

TCHAR* KDlgPlayerCheckTool::m_ModelType[MODELTYPE_COUNT] = { ".mesh", ".mdl", "*.ani",".mtl"};
extern DWORD CheckModelTypeByKeyWord(LPCSTR strFileName);
IMPLEMENT_DYNAMIC(KDlgPlayerCheckTool, CDialog)

KDlgPlayerCheckTool::KDlgPlayerCheckTool(CWnd* pParent /*=NULL*/)
	: CDialog(KDlgPlayerCheckTool::IDD, pParent)
	, m_strItemPath(_T(""))
	, m_strSetPath(_T(""))
	, m_fFrameLength(0)
{
	m_pModel = NULL;
    m_pStillModel = NULL;
	//m_PartPath.resize(PART_COUNT);
	//m_MtlPath.resize(PART_COUNT);
	m_bFrameLengthChanged = FALSE;
    m_pScene = NULL;
	m_dwCurModelID = INVALIDATEPATTERN;
	m_bChangePos = FALSE;
    m_nHorsePlayerTypeIndex = -1;
}


KDlgPlayerCheckTool::~KDlgPlayerCheckTool()
{
    OnBnClickedButtonClear();
    for (size_t i = 0; i < m_PatternModels.size(); i++)
    {
        //SAFE_RELEASE(m_PatternModels[i]);
    }
}

void KDlgPlayerCheckTool::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_PART, m_listPart);
	DDX_Control(pDX, IDC_LIST_ITEM, m_listItem);
	DDX_Control(pDX, IDC_COMBO_MOTION, m_comboMotion);
	DDX_Control(pDX, IDC_COMBO_PART, m_comboPart);
	DDX_Control(pDX, IDC_COMBO_SOCKET, m_comboSocket);
	DDX_Control(pDX, IDC_COMBO_SET, m_comboSet);
	DDX_Text(pDX, IDC_EDIT_PLAYERPATH, m_strSetPath);
	DDX_Text(pDX, IDC_EDIT_ITEMPATH, m_strItemPath);
	DDX_Text(pDX, IDC_EDIT_FRAMELENGTH, m_fFrameLength);
	//DDX_Control(pDX, IDC_COMBO1, m_comboMtl);
	DDX_Control(pDX, IDC_LIST_MTL, m_listMtl);
	DDX_Check(pDX,IDC_CHECK1,m_bChangePos);
}


BEGIN_MESSAGE_MAP(KDlgPlayerCheckTool, CDialog)
	ON_LBN_SELCHANGE(IDC_LIST_PART, &KDlgPlayerCheckTool::OnLbnSelchangeListPart)
	ON_LBN_SELCHANGE(IDC_LIST_ITEM, &KDlgPlayerCheckTool::OnLbnSelchangeListItem)
	ON_CBN_SELCHANGE(IDC_COMBO_PART, &KDlgPlayerCheckTool::OnCbnSelchangeComboPart)
	ON_CBN_SELCHANGE(IDC_COMBO_SET, &KDlgPlayerCheckTool::OnCbnSelchangeComboSet)
	ON_BN_CLICKED(IDC_BUTTON_REFRESH, &KDlgPlayerCheckTool::OnBnClickedButtonRefresh)
	ON_BN_CLICKED(IDC_BUTTON_SAVEAS, &KDlgPlayerCheckTool::OnBnClickedButtonSaveAs)
	ON_CBN_SELCHANGE(IDC_COMBO_MOTION, &KDlgPlayerCheckTool::OnCbnSelchangeComboMotion)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, &KDlgPlayerCheckTool::OnBnClickedButtonClear)
	ON_LBN_SELCHANGE(IDC_LIST_MTL, &KDlgPlayerCheckTool::OnLbnSelchangeListMtl)
	ON_BN_CLICKED(IDC_RADIO_WIREFRAME, &KDlgPlayerCheckTool::OnBnClickedRadioWireframe)
	ON_BN_CLICKED(IDC_RADIO_SOLID, &KDlgPlayerCheckTool::OnBnClickedRadioSolid)
	ON_BN_CLICKED(IDC_VERTEX_JOINTING, &KDlgPlayerCheckTool::OnBnClickedVertexJointing)
	ON_BN_CLICKED(IDC_CHECK1, &KDlgPlayerCheckTool::OnBnClickedCheck1)
	ON_BN_CLICKED(IDC_BUTTON_H, &KDlgPlayerCheckTool::OnBnClickedButtonH)
	ON_BN_CLICKED(IDC_BUTTON_M, &KDlgPlayerCheckTool::OnBnClickedButtonM)
	ON_BN_CLICKED(IDC_BUTTON_L, &KDlgPlayerCheckTool::OnBnClickedButtonL)
	ON_BN_CLICKED(IDC_BUTTON_CLEARPART, &KDlgPlayerCheckTool::OnBnClickedButtonClearpart)
	ON_BN_CLICKED(IDC_BUTTON_SAVEAS2, &KDlgPlayerCheckTool::OnBnClickedButtonSaveas2)
END_MESSAGE_MAP()


void KDlgPlayerCheckTool::UpdatePath()
{
	KG_PROCESS_ERROR(m_strStartPath[0] != '\0');
	if (m_strStartPath[strlen(m_strStartPath) - 1] != '\\')
	{
		strcat(m_strStartPath, "\\");
	}
    int nIndex = m_comboSet.GetCurSel();
    if (nIndex == m_nHorsePlayerTypeIndex)
    {
        sprintf_s(m_strModelPath, "%shorse\\", m_strHorsePath);
        sprintf_s(m_strAnimationPath, "%s动作\\", m_strHorsePath);
    }
    else
    {
	    sprintf_s(m_strModelPath, "%s部件\\", m_strStartPath);
	    sprintf_s(m_strAnimationPath, "%s动作\\", m_strStartPath);
    }
Exit0:
	;
}

void KDlgPlayerCheckTool::RefreshPath()
{
	for (size_t i = 0; i < m_strSuffix.size(); i++)
	{
		m_PartPath[i].clear();
		FindModelInDirectory(m_strModelPath,
			TRUE,
			Model_Mesh,
			m_strSuffix[i].c_str(),
			m_PartPath[i]);	

		m_MtlPath[i].clear();
		FindModelInDirectory(m_strModelPath,
			TRUE,
			Model_Mtl,
			m_strSuffix[i].c_str(),
			m_MtlPath[i]);	
	}
}

void KDlgPlayerCheckTool::OnLbnSelchangeListPart()
{
	HRESULT hr = E_FAIL;
	int nCurPartModel = m_listPart.GetCurSel();
	IKG3DModel* pModel = NULL;
	IEKG3DSceneModelEditor *pModelEditor = NULL;
	int nCurPart =0;
	KG_PROCESS_ERROR(nCurPartModel != LB_ERR);
//	KG_PROCESS_ERROR(nCurPartModel < PART_COUNT);
	nCurPart = m_comboPart.GetCurSel();
	KG_PROCESS_ERROR(nCurPart != CB_ERR);
	
	KG_PROCESS_ERROR(m_pScene);

	if (nCurPartModel != 0)
	{
		m_pScene->GetSceneModelEditor(&pModelEditor);
		KG_PROCESS_ERROR(pModelEditor);

		hr = pModelEditor->LoadModel(m_PartPath[nCurPart][nCurPartModel - 1].c_str(), 
			&pModel);
		KG_COM_PROCESS_ERROR(hr);
		pModel->SetDetail(0,0);
	}

	m_CurrentSelection[nCurPart] = nCurPartModel;

	if (!ApplyAutoBind(nCurPart, pModel))
	{
		KG_PROCESS_ERROR(m_pModel);
		hr = m_pModel->ChangeModel(nCurPart, pModel);
		KG_COM_PROCESS_ERROR(hr);
		//OnLbnSelchangeListMtl();
	}
	else
	{
		if (nCurPart == m_nFaceIndex && pModel)
		{
			pModel->SetFaceMotionMode();
		}
		hr = S_OK;
	}
	
	{
		KSceneModelEditorFrameWnd* pFrame = static_cast<KSceneModelEditorFrameWnd*>(GetModelFrameWnd());
		KG_PROCESS_ERROR(pFrame);

        char szPartName[MAX_PATH] = {0};
        m_listPart.GetText(nCurPartModel, szPartName);
		pFrame->SendMessage(WM_USER_MODEL_CHANGE, (WPARAM)szPartName);
	}
Exit0:
	if (FAILED(hr) && (nCurPartModel != LB_ERR))
	{
		if (nCurPartModel - 1 < (int)m_PartPath[nCurPart].size())
		{
			TCHAR strErrInfo[MAX_PATH << 1];
			sprintf_s(strErrInfo,
			MAX_PATH << 1,
				"替换模型%s失败， 检查模型是否正确.",
				m_PartPath[nCurPart][nCurPartModel].c_str());
			MessageBox(strErrInfo, "错误", MB_OK);
		}
	};
}

void KDlgPlayerCheckTool::OnLbnSelchangeListItem()
{
	HRESULT hr = E_FAIL;
	int nCurItem = m_listItem.GetCurSel();
	KG_PROCESS_ERROR(nCurItem != LB_ERR);
	int nItemIndex = (int)m_listItem.GetItemData(nCurItem);
	IKG3DModel* pItemModel = NULL;
	
	IEKG3DSceneModelEditor *pModelEditor = NULL;
	KG_PROCESS_ERROR(m_pScene);

	m_pScene->GetSceneModelEditor(&pModelEditor);
	KG_PROCESS_ERROR(pModelEditor);

	hr = pModelEditor->LoadModel(m_ItemPath[nItemIndex].c_str(), 
		&pItemModel);
	KG_COM_PROCESS_ERROR(hr);

	KG_PROCESS_ERROR(m_pModel);
	int nCurSocket = m_comboSocket.GetCurSel();
	KG_PROCESS_ERROR(nCurSocket != CB_ERR);
	pItemModel->BindToSocket(m_pModel, m_strSockets[nCurSocket].c_str());
	m_ItemModels.push_back(pItemModel);

    {
        KSceneModelEditorFrameWnd* pFrame = static_cast<KSceneModelEditorFrameWnd*>(GetModelFrameWnd());
        KG_PROCESS_ERROR(pFrame);

        char szPartName[MAX_PATH] = {0};
        m_listItem.GetText(nCurItem, szPartName);
        pFrame->SendMessage(WM_USER_MODEL_CHANGE, (WPARAM)szPartName);
    }

Exit0:
	;
}


void KDlgPlayerCheckTool::OnCbnSelchangeComboPart()
{
	int nIndex = m_comboPart.GetCurSel();
	KG_PROCESS_ERROR(nIndex != CB_ERR);
	m_listPart.ResetContent();
	KG_PROCESS_ERROR(nIndex < static_cast<int>(m_strSuffix.size()));
	m_listPart.AddString("Null");
	for (size_t i = 0; i < m_PartPath[nIndex].size(); i++)
	{
		TCHAR strFileName[MAX_PATH];
		_splitpath_s(m_PartPath[nIndex][i].c_str(), 
			NULL, 
			0, 
			NULL,
			0,
			strFileName,
			MAX_PATH,
			NULL,
			0);
		m_listPart.AddString(strFileName);
	}

	m_listMtl.ResetContent();
	for (size_t i = 0; i < m_MtlPath[nIndex].size(); i++)
	{
		TCHAR strFileName[MAX_PATH];
		_splitpath_s(m_MtlPath[nIndex][i].c_str(), 
			NULL, 
			0, 
			NULL,
			0,
			strFileName,
			MAX_PATH,
			NULL,
			0);
		m_listMtl.AddString(strFileName);
	}

	m_listPart.SetCurSel(m_CurrentSelection[nIndex]);

	KSceneModelEditorFrameWnd* pFrame = static_cast<KSceneModelEditorFrameWnd*>(GetModelFrameWnd());
	KG_PROCESS_ERROR(pFrame);
	pFrame->SendMessage(WM_USER_MODEL_CHANGE);
Exit0:
	;
}

void KDlgPlayerCheckTool::OnCbnSelchangeComboSet()
{
	TCHAR strSet[MAX_PATH];
	TCHAR strlower[MAX_PATH];
    DWORD i = 0;
	KSceneModelEditorFrameWnd *pWnd = NULL;
    IEKG3DSceneModelEditor* pModelEditor = NULL;
	KSceneModelEditorAnimationPanel *pAnimationSeekBar = NULL;
    int nCurSel = m_comboSet.GetCurSel();
	HRESULT hr = E_FAIL;
    KG_PROCESS_ERROR(nCurSel != CB_ERR);
	m_comboSet.GetLBText(nCurSel, strSet);

	if (nCurSel == m_nHorsePlayerTypeIndex)
    {
	    sprintf_s(m_strStartPath, 
		    MAX_PATH,
		    "%s%s",
		    m_strHorsePath, 
		    strSet); 
    }
    else
    {
        sprintf_s(m_strStartPath, 
            MAX_PATH,
            "%s%s",
            m_strSetPath, 
            strSet); 
    }
	strcpy_s(strlower, MAX_PATH, strSet);
    _strlwr_s(strlower);
    
	pWnd = static_cast<KSceneModelEditorFrameWnd*>(GetModelFrameWnd());
	KG_PROCESS_ERROR(pWnd);

	pAnimationSeekBar = pWnd->GetAnimationSeekBar();
	KG_PROCESS_ERROR(pAnimationSeekBar);

	for (i = 0; i < m_PatternPathName.size(); i++)
    {
        if (!strcmp(strlower, m_PatternPathName[i].c_str()))
        {
            SetModel(i);
			pAnimationSeekBar->m_PatternID = static_cast<DWORD>(i);
			m_dwCurModelID = static_cast<DWORD>(i);
            break;
        }
    }

	KG_PROCESS_ERROR(i < m_PatternPathName.size());

	UpdateStandardData();
    UpdatePath();
	RefreshPath();
	OnRefreshMotion();
	m_comboMotion.SetCurSel(0);
	m_comboPart.SetCurSel(0);
	for (int i = 0; i < m_comboPart.GetCount(); i++)
	{
		m_comboPart.SetCurSel(i);
		OnCbnSelchangeComboPart();
		if (m_listPart.GetCount() > 1)
		{
			m_listPart.SetCurSel(1);
		}
		else
		{
			m_listPart.SetCurSel(0);
		}
		OnLbnSelchangeListPart();
	}
	
	OnCbnSelchangeComboMotion();
	hr = m_pScene->GetSceneModelEditor(&pModelEditor);
    KG_COM_PROCESS_ERROR(hr);

	hr = pModelEditor->OpenModel((IEKG3DModel*)m_pModel, FALSE);
    KG_COM_PROCESS_ERROR(hr);

    KSceneModelEditorFrameWnd* pFrame = static_cast<KSceneModelEditorFrameWnd*>(GetModelFrameWnd());
	KG_PROCESS_ERROR(pFrame);
    pFrame->SendMessage(WM_USER_MODEL_CHANGE);
Exit0:
	;
}

void KDlgPlayerCheckTool::OnRefreshMotion()
{
	m_comboMotion.ResetContent();
	m_MotionPath.clear();
	FindModelInDirectory(m_strAnimationPath,
		FALSE,
		Model_Ani,
		"",
		m_MotionPath);
	for (size_t i = 0; i < m_MotionPath.size(); i++)
	{
		TCHAR strFileName[MAX_PATH];
		_splitpath_s(m_MotionPath[i].c_str(),
			NULL,
			0,
			NULL,
			0,
			strFileName,
			MAX_PATH,
			NULL,
			0);
		m_comboMotion.AddString(strFileName);
	}
}

void KDlgPlayerCheckTool::OnOK()
{
	IEKG3DClip* pClip = NULL;
	IEKG3DClipTable *piClipTable = NULL;
	float fOldLenght = m_fFrameLength;
	UpdateData();
	if (m_strItemPath.GetLength() && m_strItemPath.GetAt(m_strItemPath.GetLength() - 1) != '\\')
	{
		m_strItemPath.AppendChar('\\');
	}
	if (m_strSetPath.GetLength() && m_strSetPath.GetAt(m_strSetPath.GetLength() - 1) != '\\')
	{
		m_strSetPath.AppendChar('\\');
	}
	
	if (abs(fOldLenght - m_fFrameLength) > 0.1f)
	{
		int nCurMotion = m_comboMotion.GetCurSel();
		KG_PROCESS_ERROR(nCurMotion != CB_ERR);

		g_pEngineManager->GetIEKG3DClipTable(&piClipTable);
		piClipTable->LoadResourceFromFileInEditor(m_MotionPath[nCurMotion].c_str(),
			0,
			0,
			(IEKG3DClip **)&pClip);
		pClip->SetFrameLength(m_fFrameLength);
		m_bFrameLengthChanged = TRUE;
	}
	UpdateData(FALSE);
Exit0:
	SAFE_RELEASE(pClip);
}

void KDlgPlayerCheckTool::OnBnClickedButtonRefresh()
{
	OnOK();
	m_comboSet.ResetContent();
	if (m_strSetPath.GetLength())
	{
		CFileFind Find;
		TCHAR strFind[MAX_PATH];
		sprintf_s(strFind,
			"%s*.*",
			m_strSetPath);
		BOOL bReturn = Find.FindFile(strFind);
		while (bReturn)
		{
			bReturn = Find.FindNextFile();
			if (Find.IsDirectory())
			{
				if (Find.GetFileName() != "." && Find.GetFileName() != ".." &&Find.GetFileName().MakeLower() != ".svn")
				{
					m_comboSet.AddString(Find.GetFileName());
				}
			}
		}
	}
    m_comboSet.AddString("horse");
	OnRefreshItem();
	m_comboSet.SetCurSel(0);
	OnCbnSelchangeComboSet();
}

void KDlgPlayerCheckTool::FindModelInDirectory(LPCSTR strStartDir, 
											BOOL bIncludeSubDir,
											enuModelType Type,
											const TCHAR* strKeyWord,
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
				strKeyWord,
				vecStore);
		}
	};
	sprintf_s(strStartFiles,
		MAX_PATH,
		"%s%s%s",
		strStartDir,
		strKeyWord,
		m_ModelType[Type]);
	KG_PROCESS_ERROR(FindMDL.FindFile(strStartFiles));
	bWorking = TRUE; 
	while (bWorking)
	{
		bWorking = FindMDL.FindNextFile();
		TCHAR strFilePath[MAX_PATH];
		strcpy_s(strFilePath,
			MAX_PATH,
			FindMDL.GetFilePath());
		string strtmp = strFilePath;
		if(strtmp.find("_l.")==-1 && strtmp.find("_m.")==-1)
		{
			vecStore.push_back(strFilePath);
		}
	};
	FindMDL.Close();
	Find.Close();
Exit0:
	;
}

void KDlgPlayerCheckTool::OnCbnSelchangeComboMotion()
{
	HRESULT hr = E_FAIL;
	static int nLastMotion = 0;
	static float fLastLength = 0.0f;
	IEKG3DClip* pClip = NULL;
	IEKG3DClip* pMotion = NULL;
	DWORD dwSplitType = 0;
	IEKG3DClipTable *piClipTable = NULL;
	KSceneModelEditorFrameWnd *pWnd = NULL;
	KSceneModelEditorAnimationPanel *pAnimationSeekBar = NULL;
	IEKG3DModel *pModel = NULL;

	UpdateData();
	if (m_bFrameLengthChanged)
	{
		g_pEngineManager->GetIEKG3DClipTable(&piClipTable);
		hr = piClipTable->LoadResourceFromFileInEditor(m_MotionPath[nLastMotion].c_str(),
			0,
			0,
			(IEKG3DClip **)&pClip);
		if (MessageBox("动画的帧间隔已经改变，是否要保存修改.", "提示", MB_YESNO) == IDYES)
		{
			hr = pClip->SaveToFile(m_MotionPath[nLastMotion].c_str(), 0);
			if (FAILED(hr))
			{
				TCHAR strOutputInfo[MAX_PATH << 1];
				TCHAR strTemp[] = "未知Animation";
				LPCTSTR pName = NULL;
				if (FAILED(pClip->GetName(&pName)))
				{
					pName = strTemp;
				}
				sprintf_s(strOutputInfo, MAX_PATH << 1, "保存%s失败。", pName);
				MessageBox(strOutputInfo);
			}
		}
		else
		{
			//pAnimation->SetBaseClipInfo(0, SETBASECLIPFLAG_FRAMELENGTH, fLastLength, NULL);
			pClip->SetFrameLength(fLastLength);
		}
		m_bFrameLengthChanged = FALSE;
	}
	KG_PROCESS_ERROR(m_pModel);
	int nCurMotion = m_comboMotion.GetCurSel();
	KG_PROCESS_ERROR(nCurMotion != CB_ERR);
    nLastMotion = nCurMotion;

    int nSetIndex = m_comboSet.GetCurSel();
    if (nSetIndex == m_nHorsePlayerTypeIndex)
    {
        m_pModel->PlayAnimation(ENU_ANIMATIONPLAY_CIRCLE,
            m_MotionPath[nCurMotion].c_str(),
            1.0, 0, NULL, NULL, ANICTL_PRIMARY);
        KG_PROCESS_ERROR(false);
    }

	dwSplitType = CheckModelTypeByKeyWord(m_MotionPath[nCurMotion].c_str());
    KG_PROCESS_ERROR(dwSplitType != 0xffffff);
	hr = m_pModel->PlaySplitAnimation(
		m_MotionPath[nCurMotion].c_str(),
		dwSplitType,
		ANI_JOINT_TOP,
		ENU_ANIMATIONPLAY_CIRCLE,
		1.0f,
		0,
		NULL,
		NULL,
		ANICTL_PRIMARY);
	
	hr = m_pModel->PlaySplitAnimation(
		m_MotionPath[nCurMotion].c_str(),
		dwSplitType,
		ANI_JOINT_BOTTOM,
		ENU_ANIMATIONPLAY_CIRCLE,
		1.0f,
		0,
		NULL,
		NULL,
		ANICTL_PRIMARY);

	//m_pModel->PlayAnimation(ENU_ANIMATIONPLAY_CIRCLE, m_MotionPath[nCurMotion].c_str(), 1.0f, 0, NULL, NULL);
	m_fFrameLength = 0.0f;
	g_pEngineManager->GetIEKG3DClipTable(&piClipTable);
	KG_PROCESS_ERROR(piClipTable);
	
	hr = piClipTable->LoadResourceFromFileInEditor(m_MotionPath[nCurMotion].c_str(),
		0,
		0,
		(IEKG3DClip**)&pMotion);
	KG_COM_PROCESS_ERROR(hr);
	//pMotion->GetBaseClipFrameLength(0, &m_fFrameLength);
	pMotion->GetFrameLength(&m_fFrameLength);
	fLastLength = m_fFrameLength;
	UpdateData(FALSE);


	pWnd = static_cast<KSceneModelEditorFrameWnd*>(GetModelFrameWnd());
	KG_PROCESS_ERROR(pWnd);

	pAnimationSeekBar = pWnd->GetAnimationSeekBar();
	KG_PROCESS_ERROR(pAnimationSeekBar);

	pModel = dynamic_cast<IEKG3DModel*>(m_pModel);

	pModel->GetAnimationTagContainer(&pAnimationSeekBar->m_pContainer);

	pAnimationSeekBar->SetAnimation(m_MotionPath[nCurMotion].c_str());
Exit0:
	SAFE_RELEASE(pClip);
	SAFE_RELEASE(pMotion);
}

void KDlgPlayerCheckTool::OnRefreshItem()
{
	m_listItem.ResetContent();
	m_ItemPath.clear();
	if (m_strItemPath.GetLength())
	{
		FindModelInDirectory(m_strItemPath, 
			TRUE,
			Model_Mesh,
			"*",
			m_ItemPath);
		for (size_t i = 0; i < m_ItemPath.size(); i++)
		{
			TCHAR strFileName[MAX_PATH];
			_splitpath_s(m_ItemPath[i].c_str(),
				NULL,
				0,
				NULL,
				0,
				strFileName,
				MAX_PATH,
				NULL,
				0);
			int nIndex = m_listItem.AddString(strFileName);
			m_listItem.SetItemData(nIndex, (DWORD_PTR)i);
		}
	}
}

void KDlgPlayerCheckTool::SaveConfig()
{
	TCHAR strConfigFilePath[MAX_PATH];
	sprintf_s(strConfigFilePath,
		MAX_PATH,
		"%sConfig.ini",
		g_szDefWorkDirectory);
	IIniFile* fpConfig = g_OpenIniFile(strConfigFilePath);
	KG_PROCESS_ERROR(fpConfig);
	//fpConfig->WriteString(m_strConfigSection, "SetPart", m_strSetPath);
	//fpConfig->WriteString(m_strConfigSection, "ItemPath", m_strItemPath);
	fpConfig->Save(strConfigFilePath);
	fpConfig->Release();
	return;
Exit0:
	KGLogPrintf(KGLOG_ERR, "KDlgMDLCheckTool save configuration failed.");
	;
}

//void KDlgPlayerCheckTool::LoadConfig()
//{
//	TCHAR strDefaultPath[MAX_PATH];
//	TCHAR strData[MAX_PATH];
//	TCHAR strConfigFilePath[MAX_PATH];
//	sprintf_s(strConfigFilePath,
//		MAX_PATH,
//		"%sConfig.ini",
//		g_szDefWorkDirectory);
//	IIniFile* fpConfig = g_OpenIniFile(strConfigFilePath);
//	KG_PROCESS_ERROR(fpConfig);
//	sprintf_s(strDefaultPath, MAX_PATH, "%sData\\source\\player\\", g_szDefWorkDirectory);
//	fpConfig->GetString(m_strConfigSection, "SetPart", strDefaultPath, strData, MAX_PATH);
//	m_strSetPath = strData;
//	sprintf_s(strDefaultPath, MAX_PATH, "%sData\\source\\item\\", g_szDefWorkDirectory);
//	fpConfig->GetString(m_strConfigSection, "ItemPath", strDefaultPath, strData, MAX_PATH);
//	m_strItemPath = strData;
//	fpConfig->Release();
//	return;
//Exit0:
//	KGLogPrintf(KGLOG_ERR, "KDlgPlayerCheckTool load configuration failed.");
//	;
//}

void KDlgPlayerCheckTool::LoadConfig()
{
	int nNumPart          = 0;
	int nNumSocket        = 0;
	int nNumPlayerType    = 0; 
	int nNumAutoBind      = 0;
	IIniFile *pConfigFile = NULL;
	TCHAR strData[MAX_PATH];
	TCHAR strDefaultPath[MAX_PATH];
	
	pConfigFile = g_OpenIniFile(m_strConfigFilePath);
	KG_PROCESS_ERROR(pConfigFile);

	//读取配置路径
	sprintf_s(strDefaultPath, 
		MAX_PATH, 
		"%sdata\\source\\player\\", 
		g_szDefWorkDirectory);
	pConfigFile->GetString(m_strDirectorySection, 
		"SetPath", 
		strDefaultPath, 
		strData, 
		MAX_PATH);
	m_strSetPath = strData;
	sprintf_s(strDefaultPath, 
		MAX_PATH, 
		"%sdata\\source\\item\\", 
		g_szDefWorkDirectory);
	pConfigFile->GetString(m_strDirectorySection, 
		"ItemPath", 
		strDefaultPath, 
		strData, 
		MAX_PATH);
	m_strItemPath = strData;
    sprintf_s(strDefaultPath,
        MAX_PATH,
        "%sdata\\source\\NPC_source\\Horse\\",
        g_szDefWorkDirectory);
    pConfigFile->GetString(m_strDirectorySection,
        "HorsePath",
        strDefaultPath,
        strData,
        MAX_PATH);
    m_strHorsePath = strData;

	//读取主角模型类型信息
	pConfigFile->GetInteger(m_strPlayerTypeSection, "NumType", 0, &nNumPlayerType);
	m_PatternPath.resize(nNumPlayerType);
	m_PatternPathName.resize(nNumPlayerType);
	m_StandardPath.resize(nNumPlayerType);
	
	for (int i = 0; i < nNumPlayerType; i++)
	{
		TCHAR strKey[MAX_PATH];
		TCHAR strPlayerType[MAX_PATH];
		TCHAR strStandardFile[MAX_PATH];
		TCHAR strPlayerTypeName[MAX_PATH];
		sprintf_s(strKey,
			MAX_PATH,
			"PlayerType%d",
			i);
		pConfigFile->GetString(m_strPlayerTypeSection, 
			strKey, 
			"", 
			strPlayerType, 
			MAX_PATH);
		m_PatternPath[i] = strPlayerType;

		sprintf_s(strKey,
			MAX_PATH,
			"PlayerTypeName%d",
			i);
		pConfigFile->GetString(m_strPlayerTypeSection,
			strKey,
			"",
			strPlayerTypeName,
			MAX_PATH);
		m_PatternPathName[i] = strPlayerTypeName;

		//读取标准化模型信息
		sprintf_s(strKey,
			MAX_PATH,
			"PlayerStandard%d",
			i);
		pConfigFile->GetString(m_strPlayerTypeSection,
			strKey,
			"",
			strStandardFile,
			MAX_PATH);
		m_StandardPath[i] = strStandardFile;

        if (!strcmp(strPlayerTypeName, "horse"))
        {
            m_nHorsePlayerTypeIndex = i;
        }
	}

	//读取主角部件信息
	pConfigFile->GetInteger(m_strPartSection, "NumPart", 0, &nNumPart);
	m_strSuffix.resize(nNumPart);
	m_PartPath.resize(nNumPart);
	m_MtlPath.resize(nNumPart);
	m_CurrentSelection.resize(nNumPart);

	for (int i = 0; i < nNumPart; i++)
	{
		TCHAR strKey[MAX_PATH];
		TCHAR strPartKey[MAX_PATH];
		TCHAR strPartName[MAX_PATH];

		sprintf_s(strKey,
			MAX_PATH,
			"PartKey%d",
			i);
		pConfigFile->GetString(m_strPartSection, 
			strKey, 
			"", 
			strPartKey, 
			MAX_PATH);
		m_strSuffix[i] = strPartKey;

		sprintf_s(strKey,
			MAX_PATH,
			"PartName%d",
			i);
		pConfigFile->GetString(m_strPartSection,
			strKey,
			"",
			strPartName,
			MAX_PATH);
		m_comboPart.AddString(strPartName);
	}

	//读取特殊位置部件的Index
	pConfigFile->GetInteger(m_strPartSection, "HeadIndex", 0, &m_nHeadIndex);
	pConfigFile->GetInteger(m_strPartSection, "BodyIndex", 0, &m_nBodyIndex);
	pConfigFile->GetInteger(m_strPartSection, "FaceIndex", 0, &m_nFaceIndex);
	pConfigFile->GetInteger(m_strPartSection, "HatIndex",  0, &m_nHatIndex);
	pConfigFile->GetInteger(m_strPartSection, "HandIndex", 0, &m_nHandIndex);
	
	//读取插槽配置信息
	pConfigFile->GetInteger(m_strSocketSection, "NumSocket", 0, &nNumSocket);
	m_strSockets.resize(nNumSocket);
	for (int i = 0; i < nNumSocket; i++)
	{
		TCHAR strKey[MAX_PATH];
		TCHAR strSocket[MAX_PATH];
		TCHAR strSocketName[MAX_PATH];

		sprintf_s(strKey,
			MAX_PATH,
			"Socket%d",
			i);
		pConfigFile->GetString(m_strSocketSection,
			strKey,
			"",
			strSocket,
			MAX_PATH);
		m_strSockets[i] = strSocket;
		
		sprintf_s(strKey,
			MAX_PATH,
			"SocketName%d",
			i);
		pConfigFile->GetString(m_strSocketSection,
			strKey,
			"",
			strSocketName,
			MAX_PATH);
		m_comboSocket.AddString(strSocketName);
	}

	//读取自动绑定配置信息
	pConfigFile->GetInteger(m_strAutoBindSection, "NumAutoBind", 0, &nNumAutoBind);
	for (int i = 0; i < nNumAutoBind; i++)
	{
		DWORD dwInvalidatePartIndex = 0xffffffff;
		DWORD dwPartIndex = dwInvalidatePartIndex;
		TCHAR strKey[MAX_PATH];
		TCHAR strBindDest[MAX_PATH];
		TCHAR strPartSrc[MAX_PATH];
		sprintf_s(strKey,
			MAX_PATH,
			"Parent%d",
			i);
		pConfigFile->GetString(m_strAutoBindSection, 
			strKey, 
			"", 
			strBindDest, 
			MAX_PATH);

		sprintf_s(strKey,
			MAX_PATH,
			"Child%d",
			i);
		pConfigFile->GetString(m_strAutoBindSection,
			strKey,
			"",
			strPartSrc,
			MAX_PATH);
		for (int i = 0; i < m_comboPart.GetCount(); i++)
		{
			CString strPartName;
			m_comboPart.GetLBText(i, strPartName);
			if (strPartName == strPartSrc)
			{
				dwPartIndex = static_cast<DWORD>(i);
				break;
			}
		}
		if (dwPartIndex != dwInvalidatePartIndex)
		{
			AutoBind NewAutoBind = {strBindDest, dwPartIndex, NULL};
			m_AutoBind.push_back(NewAutoBind);
		}
	}
//Exit1:
Exit0:
	SAFE_RELEASE(pConfigFile);
}

void KDlgPlayerCheckTool::OnCancel()
{
	SaveConfig();
	CDialog::OnCancel();
	//PostNcDestroy();
}

void KDlgPlayerCheckTool::PostNcDestroy()
{
	delete this;
}

BOOL KDlgPlayerCheckTool::OnInitDialog()
{
	CDialog::OnInitDialog();
	LoadConfig();
	LoadPreset();
	UpdateData(FALSE);
	OnBnClickedButtonRefresh();
	return TRUE;
}

HRESULT KDlgPlayerCheckTool::LoadPreset()
{
    HRESULT hr = E_FAIL;
    BOOL bLoadSucced = FALSE;
    TCHAR strPresetPath[MAX_PATH];
    IKG3DResourceManager* pModelMgr = g_GetModelManager();
    ClearPatternModel();
    KG_PROCESS_ERROR(pModelMgr);
    for (size_t i = 0; i < m_PatternPath.size(); i++)
    {
        IKG3DModel* pModel = NULL;
		sprintf_s(strPresetPath,
            MAX_PATH,
            "%s%s",
            g_szDefWorkDirectory,
            m_PatternPath[i].c_str());
		
		IEKG3DSceneModelEditor *pModelEditor = NULL;
		KG_PROCESS_ERROR(m_pScene);

		m_pScene->GetSceneModelEditor(&pModelEditor);
		KG_PROCESS_ERROR(pModelEditor);

		hr = pModelEditor->LoadModel(strPresetPath, 
			&pModel);
		//Ignore failure.
        if (SUCCEEDED(hr) && !bLoadSucced)
        {
            IEKG3DSceneModelEditor *pModelEditor = NULL;
			m_pScene->GetSceneModelEditor(&pModelEditor);
			ASSERT(pModelEditor);
			pModelEditor->OpenModel(static_cast<IEKG3DModel*>(pModel), FALSE);
			//m_pScene->AddRenderEntity(SCENEENTITY_MODEL, (void*)pModel);
            bLoadSucced = TRUE;
            m_pModel = pModel;
        }
        m_PatternModels.push_back(pModel);

        pModel = NULL;
        hr = pModelEditor->LoadModel(strPresetPath, &pModel);
        m_vecStillModel.push_back(pModel);
    }

	hr = S_OK;
Exit0:
    return hr;
}

void KDlgPlayerCheckTool::OnBnClickedButtonClear()
{
	for (size_t i = 0; i < m_ItemModels.size(); i++)
	{
		if (m_ItemModels[i])
		{
			m_ItemModels[i]->UnBindFromOther();
		}
	}
	m_ItemModels.clear();
    {
        KSceneModelEditorFrameWnd* pFrame = static_cast<KSceneModelEditorFrameWnd*>(GetModelFrameWnd());
        KG_PROCESS_ERROR(pFrame);

        int nCurIndex = m_listPart.GetCurSel();
        char szPartName[MAX_PATH] = {0};
        if (nCurIndex != -1)
        {
            m_listPart.GetText(nCurIndex, szPartName);
        }
        pFrame->SendMessage(WM_USER_MODEL_CHANGE, (WPARAM)szPartName);
    }
Exit0:
    return;
}

void KDlgPlayerCheckTool::OnBnClickedButtonSaveAs()
{
	TCHAR curDir[MAX_PATH * 2] = {_T('\0')};
	::GetCurrentDirectory(_countof(curDir), curDir);

	TCHAR strOutputInfo[MAX_PATH << 1];
	CFileDialog dlgSave(FALSE, "mdl", 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "MDL文件(*.mdl)|*.mdl||");
	CString strFileName;
	KG_PROCESS_SUCCESS(dlgSave.DoModal() != IDOK);
	strFileName = dlgSave.GetPathName();
	HRESULT hr = m_pModel->SaveToFile(strFileName, 0);	
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


void KDlgPlayerCheckTool::SetModel(DWORD ID)
{ 
    ASSERT(ID < m_PatternModels.size());
    KG_PROCESS_ERROR(m_pScene);
    m_pScene->RemoveRenderEntity(SCENEENTITY_MODEL, (void*)m_pModel);
    m_pModel = m_PatternModels[ID];
    m_pStillModel = m_vecStillModel[ID];
    KG_PROCESS_ERROR(m_pModel);
    m_pScene->AddRenderEntity(SCENEENTITY_MODEL, (void*)m_pModel);
  
Exit0:
    ;
}

void KDlgPlayerCheckTool::OnLbnSelchangeListMtl()
{
	HRESULT hr = E_FAIL;
	IKG3DModel* pModel = NULL;
	IEKG3DSceneModelEditor *pModelEditor = NULL;
	int nCurPartModel = m_listPart.GetCurSel();
	KG_PROCESS_ERROR(nCurPartModel != LB_ERR);

	int nCurPart = m_comboPart.GetCurSel();
	KG_PROCESS_ERROR(nCurPart != CB_ERR);

	int nCurMtlModel = m_listMtl.GetCurSel();
	KG_PROCESS_ERROR(nCurMtlModel != LB_ERR);
//	KG_PROCESS_ERROR(nCurMtlModel < PART_COUNT);

	/*int nCurMtl = m_comboMtl.GetCurSel();
	KG_PROCESS_ERROR(nCurMtl != CB_ERR);*/

	if (nCurPartModel != 0)
	{
		KG_PROCESS_ERROR(m_pScene);

		m_pScene->GetSceneModelEditor(&pModelEditor);
		KG_PROCESS_ERROR(pModelEditor);

		hr = pModelEditor->LoadModel(m_PartPath[nCurPart][nCurPartModel-1].c_str(), 
			&pModel);
		KG_COM_PROCESS_ERROR(hr);

		hr = pModel->LoadMaterialFromFile(m_MtlPath[nCurPart][nCurMtlModel].c_str(),0);
		KG_COM_PROCESS_ERROR(hr);
		KG_PROCESS_ERROR(m_pModel);

		if (!ApplyAutoBind(nCurPart, pModel))
		{
			hr = m_pModel->ChangeModel(nCurPart, pModel);
		}

        {
		    KSceneModelEditorFrameWnd* pFrame = static_cast<KSceneModelEditorFrameWnd*>(GetModelFrameWnd());
		    KG_PROCESS_ERROR(pFrame);
            pFrame->SendMessage(WM_USER_MODEL_CHANGE);
        }
	}

Exit0:
	;
}

HRESULT KDlgPlayerCheckTool::EnableWireframeMode(BOOL bEnable)
{
	HRESULT hr = E_FAIL;
	int nMatCout = 0;
	DWORD dwOption = 0;
	IEKG3DModel* pIEModel = NULL;
	KG_PROCESS_ERROR(m_pModel);

	if(m_pModel->GetType() == MESHTYPE_MODELST)
	{
		int nNum = static_cast<IEKG3DModel*>(m_pModel)->GetNumModel();
		for (int n=0;n<nNum; n++)
		{
			IKG3DModel* pSubModel = NULL;
			m_pModel->GetModel(n,&pSubModel);
			if (pSubModel)
			{
				pSubModel->GetNumMaterial(&nMatCout);
				for (int i=0;i<nMatCout;i++)
				{
					hr = pSubModel->GetMaterialSubsetOption(i, &dwOption);
					KG_COM_PROCESS_ERROR(hr);
					if(bEnable)
					{
						dwOption |= MATERIAL_OPTION_FILL_WIREFRAME;
						dwOption &= ~MATERIAL_OPTION_FILL_SOLID;
					}
					else
					{
						dwOption &= ~MATERIAL_OPTION_FILL_WIREFRAME;
						dwOption |= MATERIAL_OPTION_FILL_SOLID;
					}
					hr = pSubModel->SetMaterialSubsetOption(i, dwOption);
					KG_COM_PROCESS_ERROR(hr);
				}
			}	
		}
		
	}
Exit0:
	return hr;
}
void KDlgPlayerCheckTool::OnBnClickedRadioWireframe()
{
	EnableWireframeMode(TRUE);
}

void KDlgPlayerCheckTool::OnBnClickedRadioSolid()
{
	EnableWireframeMode(FALSE);
}

void KDlgPlayerCheckTool::OnBnClickedVertexJointing()
{
	if(!((m_dwCurModelID == 0/*PM_F_NORMAL*/)||(m_dwCurModelID == 3/*PM_M_NORMAL*/)))
	{
		MessageBox("现在只能处理M2 和F2！");
		return ;
	}
	int nRet = MessageBox("此操作会改变模型的Normal值，确定要执行吗？","提示",MB_OKCANCEL);
	if(nRet == IDCANCEL)
		return ;
	OnChangeModelJointNormal();
	return ;
}

HRESULT KDlgPlayerCheckTool::LoadStandardDataFromIni(const TCHAR* strFileName)
{
	HRESULT hr = E_FAIL;
	KSmartIniFile pFile ;
	int nNum = 0;
	D3DXVECTOR3 vPos,vNor;
	const float fScale = 2.54f;
	m_vecSphereFace.clear();
	m_vecSphereHead.clear();
	m_vecSphereNeck.clear();
	m_vecSphereHand.clear();

	pFile = g_OpenIniFile(strFileName);
	KG_PROCESS_ERROR(pFile);
	TCHAR strPos[MAX_PATH];
	TCHAR strNormal[MAX_PATH];
	pFile->GetInteger("Face", "VertCount", 0,&nNum);
	for (int i=1;i<=nNum;i++)
	{
		KSphere Sphe;
		sprintf_s(strPos, MAX_PATH, "Vertex%i_Pos", i);
		sprintf_s(strNormal, MAX_PATH, "Vertex%i_Normal", i);
		pFile->GetMultiFloat("Face", strPos,(float*)(&vPos),3);
		pFile->GetMultiFloat("Face", strNormal,(float*)(&vNor),3);
		Sphe.vPos.x = vPos.x * fScale;
		Sphe.vPos.y = vPos.z * fScale;
		Sphe.vPos.z = vPos.y * fScale;
		Sphe.vNormal.x = vNor.x* fScale;
		Sphe.vNormal.y = vNor.z* fScale;
		Sphe.vNormal.z = vNor.y* fScale;
		m_vecSphereFace.push_back(Sphe);
	}
	pFile->GetInteger("Head", "VertCount", 0,&nNum);
	for (int i=1;i<=nNum;i++)
	{
		KSphere Sphe;
		sprintf_s(strPos, MAX_PATH, "Vertex%i_Pos", i);
		sprintf_s(strNormal, MAX_PATH, "Vertex%i_Normal", i);
		pFile->GetMultiFloat("Head", strPos,(float*)(&vPos),3);
		pFile->GetMultiFloat("Head", strNormal,(float*)(&vNor),3);
		Sphe.vPos.x = vPos.x * fScale;
		Sphe.vPos.y = vPos.z * fScale;
		Sphe.vPos.z = vPos.y * fScale;
		Sphe.vNormal.x = vNor.x* fScale;
		Sphe.vNormal.y = vNor.z* fScale;
		Sphe.vNormal.z = vNor.y* fScale;
		m_vecSphereHead.push_back(Sphe);
	}
	pFile->GetInteger("Neck", "VertCount", 0,&nNum);
	for (int i=1;i<=nNum;i++)
	{
		KSphere Sphe;
		sprintf_s(strPos, MAX_PATH, "Vertex%i_Pos", i);
		sprintf_s(strNormal, MAX_PATH, "Vertex%i_Normal", i);
		pFile->GetMultiFloat("Neck", strPos,(float*)(&vPos),3);
		pFile->GetMultiFloat("Neck", strNormal,(float*)(&vNor),3);
		Sphe.vPos.x = vPos.x * fScale;
		Sphe.vPos.y = vPos.z * fScale;
		Sphe.vPos.z = vPos.y * fScale;
		Sphe.vNormal.x = vNor.x* fScale;
		Sphe.vNormal.y = vNor.z* fScale;
		Sphe.vNormal.z = vNor.y* fScale;
		m_vecSphereNeck.push_back(Sphe);
	}

	pFile->GetInteger("LeftArm", "VertCount", 0,&nNum);
	for (int i=1;i<=nNum;i++)
	{
		KSphere Sphe;
		sprintf_s(strPos, MAX_PATH, "Vertex%i_Pos", i);
		sprintf_s(strNormal, MAX_PATH, "Vertex%i_Normal", i);
		pFile->GetMultiFloat("LeftArm", strPos,(float*)(&vPos),3);
		pFile->GetMultiFloat("LeftArm", strNormal,(float*)(&vNor),3);
		Sphe.vPos.x = vPos.x * fScale;
		Sphe.vPos.y = vPos.z * fScale;
		Sphe.vPos.z = vPos.y * fScale;
		Sphe.vNormal.x = vNor.x* fScale;
		Sphe.vNormal.y = vNor.z* fScale;
		Sphe.vNormal.z = vNor.y* fScale;
		m_vecSphereHand.push_back(Sphe);
	}
	pFile->GetInteger("RightArm", "VertCount", 0,&nNum);
	for (int i=1;i<=nNum;i++)
	{
		KSphere Sphe;
		sprintf_s(strPos, MAX_PATH, "Vertex%i_Pos", i);
		sprintf_s(strNormal, MAX_PATH, "Vertex%i_Normal", i);
		pFile->GetMultiFloat("RightArm", strPos,(float*)(&vPos),3);
		pFile->GetMultiFloat("RightArm", strNormal,(float*)(&vNor),3);
		Sphe.vPos.x = vPos.x * fScale;
		Sphe.vPos.y = vPos.z * fScale;
		Sphe.vPos.z = vPos.y * fScale;
		Sphe.vNormal.x = vNor.x* fScale;
		Sphe.vNormal.y = vNor.z* fScale;
		Sphe.vNormal.z = vNor.y* fScale;
		m_vecSphereHand.push_back(Sphe);
	}
	hr = S_OK;
Exit0:
	return hr;
}

HRESULT KDlgPlayerCheckTool::ChangeJointNormal(IEKG3DModel* pModel,vector<KSphere> &vecSphere,BOOL bChangePos, int nIndex)
{
	HRESULT hr = S_OK;
	VFormat::FACES_NORMAL_TEXTURE1 * pVers = NULL;
	DWORD dwFVF = 0;
	DWORD dwNumVertex = 0;
	D3DXMATRIX matInverse;
	D3DXVECTOR3 Pos;
	IEKG3DMesh* pMesh = NULL;
	BOOL bMeshSys = FALSE;
	DWORD dwSize = (DWORD)vecSphere.size();

	DWORD dwCountWed = 0;
    D3DXMATRIX ResultMatrix;
    D3DXMATRIX InverseMatrix;

	KG_PROCESS_ERROR(pModel);

	pModel->GetIEKG3DMesh(&pMesh);
	KGLOG_PROCESS_ERROR(pMesh);

	pMesh->IsMeshSysNotNull(&bMeshSys);
	KGLOG_PROCESS_ERROR(bMeshSys);

	pMesh->GetMeshSysFVF(&dwFVF);
	KGLOG_PROCESS_ERROR(dwFVF==(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1));

	pMesh->GetMeshSysNumVertices(&dwNumVertex);

	hr = pMesh->LockVertexBufferOfMesfSys(0, (void**)&pVers);
	KGLOG_COM_PROCESS_ERROR(hr);

    if (nIndex == m_nHatIndex)
    {
        BindExtraInfo ExtraInfo;
        int     nRetCode = false;
        HRESULT hRetCode = E_FAIL;
        D3DXMATRIX* pMatrixRetCode = NULL;

        nRetCode = m_pStillModel->FindSocket("s_hat", &ExtraInfo);
        KG_PROCESS_ERROR(nRetCode >= 0);

        hRetCode = m_pStillModel->GetSocketMatrix(&ExtraInfo, &ResultMatrix, NULL);
        KG_COM_CHECK_ERROR(hRetCode);

        pMatrixRetCode = D3DXMatrixInverse(&InverseMatrix, NULL, &ResultMatrix);
        KG_PROCESS_ERROR(pMatrixRetCode);

        for (DWORD i = 0; i < dwNumVertex; i++)
        {
            D3DXVECTOR3 Vector;
            D3DXVec3TransformCoord(&Vector, &pVers[i].p, &ResultMatrix);
            pVers[i].p = Vector;

            D3DXVec3TransformNormal(&Vector, &pVers[i].Normal, &ResultMatrix);
            pVers[i].Normal = Vector;
        }
    }
    
	//////////////////////////////////////////////////////////////////////////
	for(DWORD i = 0; i < dwNumVertex; i++)
	{
		int nCountInOnePoint=0;
		Pos = pVers[i].p;
		for(DWORD k = 0; k < dwSize; k++)
		{
			KSphere& sphe = vecSphere[k];
			if(sphe.IsPositionIn(Pos))
			{
				nCountInOnePoint++;
				//ATLTRACE("pVers[%i]=(%f,%f,%f)\n",i,Pos.x,Pos.y,Pos.z);
				if(bChangePos)
					pVers[i].p = sphe.vPos;
				D3DXVec3Normalize(&(pVers[i].Normal),	&(sphe.vNormal));
			}
		}
		if(nCountInOnePoint>0)
			dwCountWed++;
		if(nCountInOnePoint > 1)
			ATLTRACE("第%i个顶点，受到了%i个点的影响\n",i,nCountInOnePoint);
	}
	//////////////////////////////////////////////////////////////////////////
    if (nIndex == m_nHatIndex)
    {
        for (DWORD i = 0; i < dwNumVertex; i++)
        {
            D3DXVECTOR3 Vector;
            D3DXVec3TransformCoord(&Vector, &pVers[i].p, &InverseMatrix);
            pVers[i].p = Vector;

            D3DXVec3TransformNormal(&Vector, &pVers[i].Normal, &InverseMatrix);
            pVers[i].Normal = Vector;
        }
    }

	hr = pMesh->UnLockVertexBufferOfMesfSys();
	KG_COM_PROCESS_ERROR(hr);
	if(dwCountWed < dwSize)
	{
		LPCTSTR pszName = NULL;
		TCHAR strOutputInfo[MAX_PATH];
		DWORD dwCount = dwSize -dwCountWed;
		pMesh->GetName(&pszName);
		sprintf_s(strOutputInfo, MAX_PATH, "模型%s\n有%d个顶点改变了Normal值，少了%d个顶点，标准模型接缝处有%d个顶点。\n模型接缝处的顶点与标准模型的数据不匹配！请查检模型", pszName,dwCountWed,dwCount,dwSize);
		MessageBox(strOutputInfo,"提示",0);
	}

	pMesh->RefreshByBase();

	return S_OK;
Exit0:
	return E_FAIL;
}

void KDlgPlayerCheckTool::UpdateStandardData()
{
	LoadStandardDataFromIni(m_StandardPath[m_dwCurModelID].c_str());
}

void KDlgPlayerCheckTool::OnChangeModelJointNormal()
{
	HRESULT hr=E_FAIL;
	DWORD dwSum = 0;
	vector<string> vecError;
	IEKG3DSceneModelEditor *pModelEditor = NULL;
	IEKG3DEngineManager *piManager = g_GetEngineIEInterface();
	BOOL bSave = FALSE;
	ASSERT(piManager);

	KG3DEngineEditorOption vOption,vSaveOption;

	int nCurPart = m_comboPart.GetCurSel();
	KG_PROCESS_ERROR(nCurPart != CB_ERR);
	KG_PROCESS_ERROR(m_pScene);
	m_pScene->GetSceneModelEditor(&pModelEditor);
	KG_PROCESS_ERROR(pModelEditor);

	piManager->GetEngineEditorOption(&vSaveOption);
	bSave = TRUE;
	piManager->GetEngineEditorOption(&vOption);
	vOption.bUseMultiThreadLoad = FALSE;
	piManager->SetEngineEditorOption(&vOption);
	if (nCurPart == m_nHeadIndex)
	{
	//	_ASSERTE(0);
		//处理脸模型接缝处的顶点
		{
			int nFaceCount = (int)m_PartPath[m_nFaceIndex].size(); 
			dwSum += nFaceCount;
			for(int nFace = 0; nFace < nFaceCount; nFace++)
			{
				IEKG3DModel* pFaceModel = NULL;
				pModelEditor->LoadModel(m_PartPath[m_nFaceIndex][nFace].c_str(),(IKG3DModel**)&pFaceModel);
				//pModelEditor->LoadModel("data\\source\\player\\M2\\部件\\M2_1006_face.mesh",(IKG3DModel**)&pFaceModel);
				hr = ChangeJointNormal(pFaceModel,m_vecSphereFace,m_bChangePos, m_nFaceIndex);
				if(SUCCEEDED(hr))
				{
					hr = pFaceModel->SaveToFile(m_PartPath[m_nFaceIndex][nFace].c_str(),0);
					if(FAILED(hr))
						vecError.push_back(m_PartPath[m_nFaceIndex][nFace]);
				}
				else
				{
					vecError.push_back(m_PartPath[m_nFaceIndex][nFace]);
				}
			}
		}
		//处理头模型接缝处的顶点
		{
			int nHeadCount = (int)m_PartPath[m_nHeadIndex].size();;//m_listPart.GetCount();
			dwSum += nHeadCount;
			for(int nHead = 0; nHead < nHeadCount;nHead++)
			{
				IEKG3DModel* pHeadModel = NULL;
				hr = pModelEditor->LoadModel(m_PartPath[m_nHeadIndex][nHead].c_str(),(IKG3DModel**)&pHeadModel);
				hr = ChangeJointNormal(pHeadModel,m_vecSphereHead,m_bChangePos, m_nHeadIndex);
				if(FAILED(hr))
				{
					vecError.push_back(m_PartPath[m_nHeadIndex][nHead]);
					continue;
				}
				hr = ChangeJointNormal(pHeadModel,m_vecSphereNeck,m_bChangePos, m_nHeadIndex);
				if(FAILED(hr))
				{
					vecError.push_back(m_PartPath[m_nHeadIndex][nHead]);
					continue;
				}
				hr = pHeadModel->SaveToFile(m_PartPath[m_nHeadIndex][nHead].c_str(),0);
				if(FAILED(hr))
					vecError.push_back(m_PartPath[m_nHeadIndex][nHead]);
			}
		}
	}
	else if (nCurPart == m_nBodyIndex)
	{
		//处理身体模型neck接缝处的顶点
		{
			int nBodyCount = (int)m_PartPath[m_nBodyIndex].size();//m_listPart.GetCount();
			dwSum += nBodyCount;
			for(int nBody = 0; nBody < nBodyCount;nBody++)
			{
				IEKG3DModel* pBodyModel = NULL;
				pModelEditor->LoadModel(m_PartPath[m_nBodyIndex][nBody].c_str(),(IKG3DModel**)&pBodyModel);
				hr = ChangeJointNormal(pBodyModel,m_vecSphereNeck,m_bChangePos, m_nBodyIndex);
				if(FAILED(hr))
				{
					vecError.push_back(m_PartPath[m_nBodyIndex][nBody]);
					continue;
				}
				hr = ChangeJointNormal(pBodyModel,m_vecSphereHand,m_bChangePos, m_nBodyIndex);
				if(FAILED(hr))
				{
					vecError.push_back(m_PartPath[m_nBodyIndex][nBody]);
					continue;
				}
			
				hr = pBodyModel->SaveToFile(m_PartPath[m_nBodyIndex][nBody].c_str(),0);
				if(FAILED(hr))
					vecError.push_back(m_PartPath[m_nBodyIndex][nBody]);
			}	
		}
		//处理手模型接缝处的顶点
		{
			int nHandCount = (int)m_PartPath[m_nHandIndex].size();;//m_listPart.GetCount();
			dwSum += nHandCount;
			for(int nHand = 0; nHand < nHandCount;nHand++)
			{
				IEKG3DModel* pHandModel = NULL;
				hr = pModelEditor->LoadModel(m_PartPath[m_nHandIndex][nHand].c_str(),(IKG3DModel**)&pHandModel);
				hr = ChangeJointNormal(pHandModel,m_vecSphereHand,m_bChangePos, m_nHandIndex);
				if(FAILED(hr))
				{
					vecError.push_back(m_PartPath[m_nHeadIndex][nHand]);
				}
				hr = pHandModel->SaveToFile(m_PartPath[m_nHandIndex][nHand].c_str(),0);
				if(FAILED(hr))
					vecError.push_back(m_PartPath[m_nHandIndex][nHand]);				
			 }
		}
	}
    else if (nCurPart == m_nHatIndex)
    {
        //处理脸模型接缝处的顶点
        {
            int nFaceCount = (int)m_PartPath[m_nFaceIndex].size(); 
            dwSum += nFaceCount;
            for(int nFace = 0; nFace < nFaceCount; nFace++)
            {
                IEKG3DModel* pFaceModel = NULL;
                pModelEditor->LoadModel(m_PartPath[m_nFaceIndex][nFace].c_str(),(IKG3DModel**)&pFaceModel);
                hr = ChangeJointNormal(pFaceModel,m_vecSphereFace,m_bChangePos, m_nFaceIndex);
                if(SUCCEEDED(hr))
                {
                    hr = pFaceModel->SaveToFile(m_PartPath[m_nFaceIndex][nFace].c_str(),0);
                    if(FAILED(hr))
                        vecError.push_back(m_PartPath[m_nFaceIndex][nFace]);
                }
                else
                {
                    vecError.push_back(m_PartPath[m_nFaceIndex][nFace]);
                }
            }
        }
        //处理头模型接缝处的顶点
        {
            int nHatIndex = (int)m_PartPath[m_nHatIndex].size();
            dwSum += nHatIndex;
            for(int nHat = 0; nHat < nHatIndex; nHat++)
            {
                IEKG3DModel* pHatModel = NULL;
                hr = pModelEditor->LoadModel(m_PartPath[m_nHatIndex][nHat].c_str(),(IKG3DModel**)&pHatModel);
                hr = ChangeJointNormal(pHatModel,m_vecSphereHead,m_bChangePos, m_nHatIndex);
                if(FAILED(hr))
                {
                    vecError.push_back(m_PartPath[m_nHatIndex][nHat]);
                    continue;
                }
                hr = ChangeJointNormal(pHatModel,m_vecSphereNeck,m_bChangePos, m_nHatIndex);
                if(FAILED(hr))
                {
                    vecError.push_back(m_PartPath[m_nHatIndex][nHat]);
                    continue;
                }
                hr = pHatModel->SaveToFile(m_PartPath[m_nHatIndex][nHat].c_str(),0);
                if(FAILED(hr))
                    vecError.push_back(m_PartPath[m_nHatIndex][nHat]);
            }
        }
    }

	else
	{
		MessageBox("请选择需要处里的部件（只能选头部或身体）！");
	}
	if (vecError.size()> 0)
	{
		TCHAR strOutputInfo[MAX_PATH];
		DWORD dwSucceededCount = dwSum - (DWORD)vecError.size();
		sprintf_s(strOutputInfo, MAX_PATH, "有%d个模型需要处理，成功的有%d个。\n以下文件处理失败\n", dwSum,dwSucceededCount);

		std::string strText = strOutputInfo;
		for (DWORD i = 0; i<vecError.size();i++)
		{
			strText += vecError[i];
			strText += "\n";
		}

		MessageBox(strText.c_str());
	}
	else
	{
		TCHAR strOutputInfo[MAX_PATH];
		sprintf_s(strOutputInfo, MAX_PATH, "恭喜，%d个模型处理完成。要重新启动编辑器后，才能看到改变后的效果！！！", dwSum);
		MessageBox(strOutputInfo);
	}
	
Exit0:
	if(bSave)
		piManager->SetEngineEditorOption(&vSaveOption);;
}

void KDlgPlayerCheckTool::OnBnClickedCheck1()
{
	// TODO: Add your control notification handler code here
	UpdateData();
}

void KDlgPlayerCheckTool::SetCurModelLod(float fLod)
{
	IEKG3DSceneModelEditor *pModelEditor = NULL;
	KG_PROCESS_ERROR(m_pScene);
	m_pScene->GetSceneModelEditor(&pModelEditor);
	KG_PROCESS_ERROR(pModelEditor);

	IEKG3DModel* pModel = NULL;
	pModelEditor->GetCurModel(&pModel);
	KG_PROCESS_ERROR(pModel);

	pModel->IESetLod(fLod);
Exit0:
	;
}

void KDlgPlayerCheckTool::OnBnClickedButtonH()
{
	SetCurModelLod(1);
}

void KDlgPlayerCheckTool::OnBnClickedButtonM()
{
	SetCurModelLod(0.5F);
}

void KDlgPlayerCheckTool::OnBnClickedButtonL()
{
	SetCurModelLod(0);
}

BOOL KDlgPlayerCheckTool::ApplyAutoBind(int nPartIndex, 
										IKG3DModel *pNewModel)
{
	BOOL bRetCode = FALSE;
	HRESULT hResult = E_FAIL;
	
	KG_PROCESS_ERROR(nPartIndex >= 0);
	
	for (size_t i = 0; i < m_AutoBind.size(); i++)
	{
		if (m_AutoBind[i].dwPartIndex == static_cast<DWORD>(nPartIndex))
		{
			if (m_AutoBind[i].pCurrent)
			{
				m_AutoBind[i].pCurrent->UnBindFromOther();
				m_AutoBind[i].pCurrent->SetAlpha(0, FALSE);
			}

			SAFE_RELEASE(m_AutoBind[i].pCurrent);

			KG_PROCESS_ERROR(m_pModel);
			if (pNewModel)
			{
				pNewModel->BindToSocket(m_pModel, m_AutoBind[i].strDest.c_str());
				pNewModel->AddRef();
			}
			m_AutoBind[i].pCurrent = pNewModel;
			bRetCode = TRUE;
			break;
		}
	}
Exit0:
	return bRetCode;
}

static float g_fJiontFixEpsilon = 0.01f;

BOOL KDlgPlayerCheckTool::KSphere::IsPositionIn(D3DXVECTOR3& vP)
{
	D3DXVECTOR3 C = vP - vPos;
	float fS = D3DXVec3Length(&C);
	if(fS < g_fJiontFixEpsilon)
		return TRUE;
	return FALSE;
}

void KDlgPlayerCheckTool::OnBnClickedButtonClearpart()
{
	for (int i = 0; i < m_comboPart.GetCount(); i++)
	{
		m_comboPart.SetCurSel(i);
		OnCbnSelchangeComboPart();
		m_listPart.SetCurSel(0);
		OnLbnSelchangeListPart();
	}
}

void Test()
{
	//"E:\\Sword3\\sword3-products\\trunk\\client\\data\\source\\player\\F2\\部件\\"

}

void KDlgPlayerCheckTool::OnBnClickedButtonSaveas2()
{
	TCHAR curDir[MAX_PATH * 2] = {_T('\0')};
	::GetCurrentDirectory(_countof(curDir), curDir);

	TCHAR strOutputInfo[MAX_PATH << 1];
	CFileDialog dlgSave(FALSE, "ModelST", 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "ModelST文件(*.ModelST)|*.ModelST||");
	CString strFileName;
	KG_PROCESS_SUCCESS(dlgSave.DoModal() != IDOK);
	strFileName = dlgSave.GetPathName();
	HRESULT hr = m_pModel->SaveToFile(strFileName, 0);	
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
