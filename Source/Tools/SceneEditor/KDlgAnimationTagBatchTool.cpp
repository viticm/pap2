// KDlgAnimationTagBatchTool.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KDlgAnimationTagBatchTool.h"
#include "KGFileFind.h"
#include "KDlgResourceList.h"
#include "KSceneModelEditorAnimationPanel.h"
// KDlgAnimationTagBatchTool dialog

IMPLEMENT_DYNAMIC(KDlgAnimationTagBatchTool, CDialog)

static RECT rectListFile = {17, 44, 116, 216};

//LBS_NOINTEGRALHEIGHT | WS_VSCROLL | WS_TABSTOP

const TCHAR *KDlgAnimationTagBatchTool::s_ProcessOption[KDlgAnimationTagBatchTool::PROCESS_COUNT] = 
{
	"生成列表",//PROCESS_GENERATELIST,
	"替换资源",//PROCESS_CHANGESOURCE,
	"删除资源",//PROCESS_DELETE,
	"增加资源",//PROCESS_ADD,
	"播放方式",//PROCESS_CHANGEPLAYTYPE
	"删除所有音效", //PROCESS_DELETEALLSOUND
	"插入刀光",//PROCESS_INSERTMETOR 
	"音效标签版本3",//PROCESS_SOUNDVERSION3
};

KDlgAnimationTagBatchTool::ProcessTypeInfo KDlgAnimationTagBatchTool::s_ProcessTypeInfo[KDlgAnimationTagBatchTool::PROCESS_COUNT] = 
{
	{"", "", SW_HIDE, SW_HIDE, SW_HIDE, SW_HIDE},//PROCESS_GENERATELIST,
	{"旧", "新", SW_SHOW, SW_SHOW, SW_SHOW, SW_HIDE},//PROCESS_CHANGESOURCE,
	{"资源", "", SW_SHOW, SW_HIDE, SW_SHOW, SW_HIDE},//PROCESS_DELETE,
	{"帧", "资源", SW_SHOW, SW_SHOW, SW_SHOW, SW_SHOW},//PROCESS_ADD,
	{"", "资源",SW_HIDE ,SW_SHOW,SW_SHOW, SW_SHOW},//PROCESS_CHANGEPLAYTYPE
	{"", "", SW_HIDE, SW_HIDE, SW_SHOW, SW_HIDE},//PROCESS_DELETEALLSOUND
	{"桢", "", SW_SHOW, SW_HIDE, SW_SHOW, SW_HIDE},//PROCESS_INSERTMETOR
	{"帧","",SW_SHOW, SW_HIDE, SW_SHOW, SW_HIDE},//PROCESS_SOUNDVERSION3
};

static const TCHAR *s_strSfxPlayType[] = 
{
	"循环",
	"单次保持",
	"单次",
};
static const TCHAR *s_strSoundType[] =
{
	"smile",
};
static const TCHAR *s_strNewResoureType[] = 
{
	".sfx",
	".wav",
};

static DWORD s_NewResourceType[] = 
{
	KGAnimationTagBatchTool::TYPE_SFX,
	KGAnimationTagBatchTool::TYPE_SOUND,
};
static int s_IDC_METOR[] = // 有关于刀光的控件 add by chenganlai
{
	IDC_STATIC_METORGROUP,
	IDC_STATIC_METOR,
	IDC_COMBO_SOCKET,
	IDC_LIST_SOCKET,
	IDC_RADIO_OPEN_METOR,
	IDC_RADIO_CLOSE_METOR,
};
static int s_IDC_SoundVersion3[] = 
{
	IDC_STATIC_SOUNDTYPE,
	IDC_COMBO_SOUNDTYPE,
	IDC_STATIC_SOUND,
	IDC_CHECK_SOUNDLOOP,
	IDC_CHECK_SOUNDINTERUPT, 
	IDC_CHECK_SOUNDKEEP, 
	IDC_STATIC_SOUNDVOLUME, 
	IDC_EDIT_SOUNDVOLUME, 
	IDC_CHECK_DELETE,
};
static int s_IDC_SoundVersion3Only[] = 
{
	IDC_COMBO_SOUNDTYPE,
	IDC_STATIC_SOUNDTYPE,
	IDC_CHECK_DELETE,
};
static int s_ExtraControl[] = 
{
	IDC_STATIC_SOUND,
	IDC_CHECK_SOUNDLOOP,
	IDC_CHECK_SOUNDINTERUPT, 
	IDC_CHECK_SOUNDKEEP, 
	IDC_STATIC_SOUNDVOLUME, 
	IDC_EDIT_SOUNDVOLUME, 
	IDC_STATIC_SOUNDRATE,
	IDC_EDIT_SOUDNRATE,

	IDC_STATIC_SFX,
	IDC_COMBO_SFXPLAYTYPE, 
	IDC_STATIC_SFXPLAYTYPE,
	IDC_STATIC_SFXPSPEED, 
	IDC_EDIT_SFXSPEED, 
	IDC_CHECK_SFXSPEEDAUTO, 
	IDC_CHECK_SFXOVERWRITE, 
	IDC_CHECK_SFXKEEP, 
	IDC_CHECK_SFXNOTIFY, 
	IDC_CHECK_SFXWORLD, 
};

static DWORD s_dwListPendingType[KDlgAnimationTagBatchTool::FILETYPE_COUNT] = 
{
	KDlgAnimationTagBatchTool::FILETYPE_SOURCE,
	KDlgAnimationTagBatchTool::FILETYPE_TANI,
};

KDlgAnimationTagBatchTool::KDlgAnimationTagBatchTool(CWnd* pParent,
													 IEKG3DAnimationTagContainer *pTagContainer)
	: CDialog(KDlgAnimationTagBatchTool::IDD, pParent)
	, m_strKeyword(_T(""))
	, m_strFileFullPath(_T(""))
	, m_strOldSource(_T(""))
	, m_strNewSource(_T(""))
	, m_strStaticOld(_T(""))
	, m_strStaticNew(_T(""))
	, m_strSuffix(_T("_new"))
	, m_fSoundPlayRate(0)
	, m_fSoundVolume(0)
	, m_fSFXSpeed(0)
	, m_bDefaultValueSet(false)
{
	m_pFileLists[FILETYPE_TANI] = &m_ListFiles;
	m_pFileLists[FILETYPE_SOURCE] = &m_ListFileSource;
	m_pTagContainer = pTagContainer;
	Create(IDD, pParent);
	ShowWindow(SW_SHOW);
}

KDlgAnimationTagBatchTool::~KDlgAnimationTagBatchTool()
{
}

void KDlgAnimationTagBatchTool::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_FILES, m_ListFiles);
	DDX_Control(pDX, IDC_LIST_PENDING, m_ListFilePending);
	DDX_Text(pDX, IDC_EDIT_KEYWORD, m_strKeyword);
	DDX_Text(pDX, IDC_EDIT_FULLPATH, m_strFileFullPath);
	DDX_Text(pDX, IDC_EDIT_OLD, m_strOldSource);
	DDX_Text(pDX, IDC_EDIT_NEW, m_strNewSource);
	DDX_Control(pDX, IDC_EDIT_OLD, m_editOld);
	DDX_Control(pDX, IDC_EDIT_NEW, m_editNew);
	DDX_Control(pDX, IDC_COMBO_PROCESSOPTION, m_comboProcessOption);
	DDX_Text(pDX, IDC_STATIC_OLD, m_strStaticOld);
	DDX_Text(pDX, IDC_STATIC_NEW, m_strStaticNew);
	DDX_Text(pDX, IDC_EDIT_SUFFIX, m_strSuffix);
	DDX_Control(pDX, IDC_LIST_FILES_SOURCE, m_ListFileSource);
	DDX_Text(pDX, IDC_EDIT_SOUDNRATE, m_fSoundPlayRate);
	DDX_Text(pDX, IDC_EDIT_SOUNDVOLUME, m_fSoundVolume);
	DDX_Text(pDX, IDC_EDIT_SFXSPEED, m_fSFXSpeed);
	DDX_Control(pDX, IDC_COMBO_SFXPLAYTYPE, m_comboSFXPlayType);
	DDX_Control(pDX, IDC_LIST_SOCKET, m_ListBoxSocket);
	DDX_Control(pDX, IDC_COMBO_SOCKET, m_CombSocket);
	DDX_Control(pDX, IDC_COMBO_SOUNDTYPE, mComSoundType);
}


BEGIN_MESSAGE_MAP(KDlgAnimationTagBatchTool, CDialog)
	ON_BN_CLICKED(IDOK, &KDlgAnimationTagBatchTool::OnBnClickedOk)
	ON_BN_CLICKED(IDC_RADIO_TANI, &KDlgAnimationTagBatchTool::OnBnClickedRadioTani)
	ON_BN_CLICKED(IDC_RADIO_SOURCE, &KDlgAnimationTagBatchTool::OnBnClickedRadioSource)
	ON_LBN_SELCHANGE(IDC_LIST_FILES, &KDlgAnimationTagBatchTool::OnLbnSelchangeListFiles)
	ON_LBN_DBLCLK(IDC_LIST_FILES, &KDlgAnimationTagBatchTool::OnLbnDblclkListFiles)
	ON_BN_CLICKED(IDC_BUTTON_PROCESS, &KDlgAnimationTagBatchTool::OnBnClickedButtonProcess)
	ON_CBN_SELCHANGE(IDC_COMBO_PROCESSOPTION, &KDlgAnimationTagBatchTool::OnCbnSelchangeComboProcessoption)
	ON_LBN_DBLCLK(IDC_LIST_PENDING, &KDlgAnimationTagBatchTool::OnLbnDblclkListPending)
	ON_LBN_SELCHANGE(IDC_LIST_FILES_SOURCE, &KDlgAnimationTagBatchTool::OnLbnSelchangeListFilesSource)
	ON_LBN_DBLCLK(IDC_LIST_FILES_SOURCE, &KDlgAnimationTagBatchTool::OnLbnDblclkListFilesSource)
	ON_LBN_SELCHANGE(IDC_LIST_PENDING, &KDlgAnimationTagBatchTool::OnLbnSelchangeListPending)
	ON_BN_CLICKED(IDC_BUTTON_CLEARPENDING, &KDlgAnimationTagBatchTool::OnBnClickedButtonClearpending)
	ON_BN_CLICKED(IDC_BUTTON_EXPORTLIST, &KDlgAnimationTagBatchTool::OnBnClickedButtonExportlist)
	/*ON_BN_CLICKED(IDC_BUTTON_LOADMETOR, &KDlgAnimationTagBatchTool::OnBnClickedButtonLoadmetor)*/
	ON_CBN_SELCHANGE(IDC_COMBO_SOCKET, &KDlgAnimationTagBatchTool::OnCbnSelchangeComboSocket)
	/*ON_BN_CLICKED(IDC_BUTTON_DELETE_SOCKET, &KDlgAnimationTagBatchTool::OnBnClickedButtonDeleteSocket)*/
	/*ON_BN_CLICKED(IDC_BUTTON_SOCKETBIND_DELETE, &KDlgAnimationTagBatchTool::OnBnClickedButtonSocketbindDelete)*/
	ON_LBN_SELCHANGE(IDC_LIST_SOCKET, &KDlgAnimationTagBatchTool::OnLbnSelchangeListSocket)
	ON_BN_CLICKED(IDC_RADIO_CLOSE_METOR, &KDlgAnimationTagBatchTool::OnBnClickedRadioCloseMetor)
	ON_BN_CLICKED(IDC_RADIO_OPEN_METOR, &KDlgAnimationTagBatchTool::OnBnClickedRadioOpenMetor)
	ON_LBN_DBLCLK(IDC_LIST_SOCKET, &KDlgAnimationTagBatchTool::OnLbnDblclkListSocket)
END_MESSAGE_MAP()


// KDlgAnimationTagBatchTool message handlers

void KDlgAnimationTagBatchTool::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}


BOOL KDlgAnimationTagBatchTool::OnInitDialog()
{
	CDialog::OnInitDialog();
	for (size_t i = 0; i < sizeof(s_strSfxPlayType) / sizeof(const TCHAR*); i++)
	{
		m_comboSFXPlayType.AddString(s_strSfxPlayType[i]);
	}
	FillSoundType();
	MoveMetroControl();
	MoveSoundV3Control();
	m_comboSFXPlayType.SetCurSel(0);
	OnBnClickedRadioTani();
	return TRUE;
}

void KDlgAnimationTagBatchTool::FillSoundType()
{
	mComSoundType.ResetContent();
	for (size_t i = 0 ; i < sizeof(s_strSoundType) / sizeof(TCHAR *) ; i ++)
	{
		mComSoundType.AddString(s_strSoundType[i]);
	}
}
void KDlgAnimationTagBatchTool::MoveSoundV3Control()
{
	RECT retDest;
	RECT retSrc;
	GetDlgItem(IDC_STATIC_SOUNDRATE)->GetWindowRect(&retDest);
	GetDlgItem(IDC_STATIC_SOUNDTYPE)->GetWindowRect(&retSrc);
	int nMoveX = retSrc.left - retDest.left;
	int nMoveY = retSrc.top - retDest.top;
	for (size_t i = 0 ; i < sizeof(s_IDC_SoundVersion3Only) / sizeof(int) ; i ++)
	{
		CWnd *pCtrl = GetDlgItem(s_IDC_SoundVersion3Only[i]);
		pCtrl->GetWindowRect(&retSrc);
		retSrc.left -= nMoveX;
		retSrc.right -= nMoveX;
		retSrc.top -= nMoveY;
		retSrc.bottom -= nMoveY;
		ScreenToClient(&retSrc);
		pCtrl->MoveWindow(&retSrc);
	}
}
void KDlgAnimationTagBatchTool::MoveMetroControl()
{
	RECT rectWindow;
	RECT rectWindow1;
	GetDlgItem(IDC_STATIC_SOUND)->GetWindowRect(&rectWindow);
	GetDlgItem(s_IDC_METOR[0])->GetWindowRect(&rectWindow1);
	int nMoveX = rectWindow1.left - rectWindow.left;
	int nMoveY = rectWindow1.top - rectWindow.top;
	for (size_t i = 0; i < sizeof(s_IDC_METOR) / sizeof(const TCHAR*); i++)
	{
		CWnd *pCtrl = GetDlgItem(s_IDC_METOR[i]);
		pCtrl->GetWindowRect(&rectWindow1);
		rectWindow1.left -= nMoveX;
		rectWindow1.right -= nMoveX;
		rectWindow1.top -= nMoveY;
		rectWindow1.bottom -= nMoveY;
		ScreenToClient(&rectWindow1);
		pCtrl->MoveWindow(&rectWindow1);
		pCtrl->ShowWindow(SW_HIDE);
	}
}
HRESULT KDlgAnimationTagBatchTool::Init_Motion_Metor()
{
	HRESULT hResult = E_FAIL;
	HRESULT hRet = E_FAIL;
	hRet = FillCombSocket();
	KG_COM_PROCESS_ERROR(hRet);
	m_ListBoxSocket.ResetContent();
	((CButton *)GetDlgItem(IDC_RADIO_OPEN_METOR))->SetCheck(FALSE);
	((CButton *)GetDlgItem(IDC_RADIO_CLOSE_METOR))->SetCheck(FALSE);
	MetroSelect.clear();
	hResult = S_OK;
Exit0:
	return hResult;
}
HRESULT KDlgAnimationTagBatchTool::FillCombSocket()
{
	HRESULT hResult = E_FAIL;
	HRESULT hRet = E_FAIL;
	IEKG3DModel* pCurModel = NULL;
	LPCTSTR pszSocketName = NULL;

	int nSocketNum = 0 ;
	IEKG3DSceneModelEditor* pEditor = NULL;
	m_CombSocket.ResetContent();
	hRet = g_AnimationTagBatchTool.Get_Model_Editor(&pEditor);
	KG_COM_PROCESS_ERROR(hRet);
	hRet = pEditor->GetCurModel(&pCurModel);
	KG_COM_PROCESS_ERROR(hRet);
	hRet = pCurModel->GetNumSocket(&nSocketNum);
	KG_COM_PROCESS_ERROR(hRet);
	for (int i = 0 ; i < nSocketNum ; i ++)
	{
		hRet = pCurModel->GetSocketName(i,&pszSocketName);
		KG_COM_PROCESS_ERROR(hRet);
		m_CombSocket.AddString(pszSocketName);
	}
	hResult = S_OK;
Exit0:
	return hResult;
}
CListBox& KDlgAnimationTagBatchTool::GetCurrentFileListBox()
{
	return *m_pFileLists[m_dwType];
}

void KDlgAnimationTagBatchTool::OnTypeChange(BOOL bClearPendingList /*=TRUE*/)
{
	m_comboProcessOption.ResetContent();
	g_AnimationTagBatchTool.FillListBox(&GetCurrentFileListBox(), m_dwType);
	
	for (size_t i = 0; i < FILETYPE_COUNT; i++)
	{
		int nShow = SW_HIDE;
		if (i == m_dwType)
		{
			nShow = SW_SHOW;
		}
		m_pFileLists[i]->ShowWindow(nShow);
	}

	//Update file name hash list
	m_vecFileNameHashList.clear();
	int nCount = GetCurrentFileListBox().GetCount();
	for (int i = 0; i < nCount; i++)
	{
		m_vecFileNameHashList.push_back(static_cast<DWORD>(GetCurrentFileListBox().GetItemData(i)));
	}

	//修改处理选项列表
	for (size_t i = 0; i < sizeof(s_ProcessOption) / sizeof(const TCHAR*); i++)
	{
		m_comboProcessOption.AddString(s_ProcessOption[i]);
	}

	m_comboProcessOption.SetCurSel(0);
	OnCbnSelchangeComboProcessoption();
	Init_Motion_Metor();// 初始化刀光控件
	if (bClearPendingList)
	{
		m_setFilePendingHash.clear();
		m_ListFilePending.ResetContent();
	}
}

void KDlgAnimationTagBatchTool::OnBnClickedRadioTani()
{
	m_dwType = FILETYPE_TANI;
	OnTypeChange();
}

void KDlgAnimationTagBatchTool::OnBnClickedRadioSource()
{
	m_dwType = FILETYPE_SOURCE;
	OnTypeChange();
}

void KDlgAnimationTagBatchTool::OnLbnSelchangeListFiles()
{
	int nSel = 0;
	int nSelCount = m_ListFiles.GetSelCount();
	KG_PROCESS_ERROR(nSelCount);

	m_ListFiles.GetSelItems(1, &nSel);

	DWORD dwHash = static_cast<DWORD>(m_ListFiles.GetItemData(nSel));
	m_strFileFullPath = g_AnimationTagBatchTool.GetFullPath(m_dwType, dwHash);
	UpdateData(FALSE);
Exit0:
	;
}

void KDlgAnimationTagBatchTool::OnLbnDblclkListFiles()
{
	const static DWORD s_dwMaxSel = 1000;
	int nSel[s_dwMaxSel];
	int nResult = 0;

	nResult = m_ListFiles.GetSelItems(s_dwMaxSel, nSel);
	KG_PROCESS_ERROR(nResult != LB_ERR);

	for (int i = 0; i < nResult; i++)
	{
		DWORD dwHash = static_cast<DWORD>(m_ListFiles.GetItemData(nSel[i]));
		if (!m_setFilePendingHash.count(dwHash))
		{
			CString strFileName;
			m_ListFiles.GetText(nSel[i], strFileName);
			int nNewItem = m_ListFilePending.AddString(strFileName);
			m_ListFilePending.SetItemData(nNewItem, dwHash);
			m_setFilePendingHash.insert(dwHash);
		}
		m_ListFiles.SetSel(nSel[i], FALSE);
	}
Exit0:
	;
}

void KDlgAnimationTagBatchTool::OnBnClickedButtonProcess()
{
	SFXKeyFrameInfoVersion1 DefaultSFX;
	SoundDataInfoVersion2 DefaultSound;
	SoundDataInfoVersion3 DefaultSoundVersion3;
	void *pDefaultKey = NULL;
	UpdateData();

	int nProcess = m_comboProcessOption.GetCurSel();
	KG_PROCESS_ERROR(nProcess != LB_ERR);

	if (nProcess == PROCESS_GENERATELIST)
	{
		int nCurSource = GetCurrentFileListBox().GetCurSel();
		KG_PROCESS_ERROR(nCurSource != LB_ERR);

		static KDlgResourceList *pdlgResourceList = NULL;
		if (!pdlgResourceList)
		{
			pdlgResourceList = new KDlgResourceList;
		}
		KG_PROCESS_ERROR(pdlgResourceList);
		pdlgResourceList->Reset();
		pdlgResourceList->ShowWindow(SW_SHOW);

		switch(m_dwType)
		{
		case FILETYPE_TANI:
			{
				int nCount = m_ListFilePending.GetCount();
				for (int i = 0; i < nCount; i++)
				{
					DWORD dwHash = static_cast<DWORD>(m_ListFilePending.GetItemData(i));
					DWORD dwType = g_AnimationTagBatchTool.GetType(m_dwType, dwHash);
					g_AnimationTagBatchTool.ApplyProcess(nProcess, 
						m_pTagContainer, 
						dwType, 
						dwHash, 
						m_strOldSource, 
						m_strNewSource, 
						m_strSuffix,
						NULL,
						pdlgResourceList);
				}
			}
			break;
		case FILETYPE_SOURCE:
			{
				int nCurSel = GetCurrentFileListBox().GetCurSel();
				KG_PROCESS_ERROR(nCurSel != LB_ERR);

				DWORD dwHash = static_cast<DWORD>(GetCurrentFileListBox().GetItemData(nCurSel));
				HTREEITEM hRoot = pdlgResourceList->AddBrach(g_AnimationTagBatchTool.GetFullPath(m_dwType, dwHash));
				KGAnimationTagBatchTool::SrcToDest S2D;
				HRESULT hRetCode = g_AnimationTagBatchTool.GetUsedTagFile(dwHash, S2D);
				KG_COM_PROCESS_ERROR(hRetCode);

				for (size_t i = 0; i < S2D.vecDest.size(); i++)
				{
					TagSourceInfo& Info = S2D.vecDest[i];
					pdlgResourceList->AddLeaf(hRoot, 
						Info.strFilePath.c_str(),
						Info.dwFrame,
						Info.dwIndex,
						Info.dwType);
				}
			}
			break;
		}
	}
	else
	{
		if (s_ProcessTypeInfo[nProcess].nExtraControl == SW_SHOW)
		{
			//需要获取默认的帧信息
			CString strLower = m_strNewSource;
			strLower.MakeLower();
			DWORD dwType = KGAnimationTagBatchTool::TYPE_COUNT;
			for (size_t i = 0; i < sizeof(s_strNewResoureType) / sizeof(const TCHAR*); i++)
			{
				if (strstr(strLower, s_strNewResoureType[i]))
				{
					dwType = s_NewResourceType[i];
					break;
				}
			}
			KG_PROCESS_ERROR(dwType != KGAnimationTagBatchTool::TYPE_COUNT);

			switch(dwType)
			{
			case KGAnimationTagBatchTool::TYPE_SFX:
				{
					strcpy_s(DefaultSFX.strTagName, TAG_NAME_MAX_LENGTH, "Batch tool added");
					DefaultSFX.bAdaptSpeedToMotion = (static_cast<CButton*>(GetDlgItem(IDC_CHECK_SFXSPEEDAUTO))->GetCheck() == BST_CHECKED);
					DefaultSFX.bInterruptable = (static_cast<CButton*>(GetDlgItem(IDC_CHECK_SFXOVERWRITE))->GetCheck() == BST_CHECKED);
					DefaultSFX.bNotify = (static_cast<CButton*>(GetDlgItem(IDC_CHECK_SFXNOTIFY))->GetCheck() == BST_CHECKED);
					DefaultSFX.bSetWorldPosition = (static_cast<CButton*>(GetDlgItem(IDC_CHECK_SFXWORLD))->GetCheck() == BST_CHECKED);
					DefaultSFX.FinishState = static_cast<SFXFinishState>(m_comboSFXPlayType.GetCurSel());
					DefaultSFX.fSpeed = m_fSFXSpeed;
					DefaultSFX.MotionFinishEvent = (static_cast<CButton*>(GetDlgItem(IDC_CHECK_SFXKEEP))->GetCheck() == BST_CHECKED) ? SFX_MFS_KEEP : SFX_MFS_DELETE;
					pDefaultKey = static_cast<void*>(&DefaultSFX);
				}
				break;
			case KGAnimationTagBatchTool::TYPE_SOUND:
				{
					strcpy_s(DefaultSound.strTagName, TAG_NAME_MAX_LENGTH, "Batch tool added");
					DefaultSound.bLoop = (static_cast<CButton*>(GetDlgItem(IDC_CHECK_SOUNDLOOP))->GetCheck() == BST_CHECKED);
					DefaultSound.InterruptMode = (static_cast<CButton*>(GetDlgItem(IDC_CHECK_SOUNDINTERUPT))->GetCheck() == BST_CHECKED) ? SPI_ENABLE : SPI_DISABLE;
					DefaultSound.MotionChangeState = (static_cast<CButton*>(GetDlgItem(IDC_CHECK_SOUNDKEEP))->GetCheck() == BST_CHECKED) ? SMCS_KEEP : SMCS_DELETE;
					DefaultSound.SoundCandidates[0].fHitRate = m_fSoundPlayRate;
					DefaultSound.SoundCandidates[0].fVolume = m_fSoundVolume;
					pDefaultKey = static_cast<void*>(&DefaultSound);
				}
				break;
			default:
				_ASSERTE(0);
			}
		}

		int nCount = m_ListFilePending.GetCount();
		if (nProcess == KDlgAnimationTagBatchTool::PROCESS_INSERTMETOR)
		{
			pDefaultKey = static_cast<void *>(&MetroSelect);
		}
		if(nProcess == PROCESS_SOUNDVERSION3)
		{
			BOOL bDelete = FALSE;
			strcpy_s(DefaultSoundVersion3.strTagName, TAG_NAME_MAX_LENGTH, "Batch tool added");
			DefaultSoundVersion3.bLoop = (static_cast<CButton*>(GetDlgItem(IDC_CHECK_SOUNDLOOP))->GetCheck() == BST_CHECKED);
			DefaultSoundVersion3.InterruptMode = (static_cast<CButton*>(GetDlgItem(IDC_CHECK_SOUNDINTERUPT))->GetCheck() == BST_CHECKED) ? SPI_ENABLE : SPI_DISABLE;
			DefaultSoundVersion3.MotionChangeState = (static_cast<CButton*>(GetDlgItem(IDC_CHECK_SOUNDKEEP))->GetCheck() == BST_CHECKED) ? SMCS_KEEP : SMCS_DELETE;
			DefaultSoundVersion3.fVolume = m_fSoundVolume;
			DefaultSoundVersion3.soundType = static_cast<SoundType>(mComSoundType.GetCurSel());
			bDelete = ((CButton*)GetDlgItem(IDC_CHECK_DELETE))->GetCheck();
			if(bDelete)
				DefaultSoundVersion3.soundType = ST_NONE;
			pDefaultKey = static_cast<void*>(&DefaultSoundVersion3);
		}
		for (int i = 0; i < nCount; i++)
		{
			DWORD dwHash = static_cast<DWORD>(m_ListFilePending.GetItemData(i));
			DWORD dwType = g_AnimationTagBatchTool.GetType(m_dwType, dwHash);
			g_AnimationTagBatchTool.ApplyProcess(nProcess, 
				m_pTagContainer, 
				dwType, 
				dwHash, 
				m_strOldSource, 
				m_strNewSource, 
				m_strSuffix,
				pDefaultKey, 
				NULL);
		}
	}

	OnTypeChange(FALSE);
Exit0:
	;
}

void KDlgAnimationTagBatchTool::OnOK()
{
	SearchKeyword();
}

void KDlgAnimationTagBatchTool::SearchKeyword()
{
	UpdateData();
	if (m_strKeyword == "")
	{
		OnTypeChange();
	}
	else
	{
		CString strKeyLower;
		std::vector<CString> FindName;
		m_CurrentList.clear();
		strKeyLower = m_strKeyword;
		strKeyLower.MakeLower();
		GetCurrentFileListBox().ResetContent();
		for (size_t i = 0; i < m_vecFileNameHashList.size(); i++)
		{
			CString strFileName = g_AnimationTagBatchTool.GetFileName(m_dwType, m_vecFileNameHashList[i]);
			strFileName.MakeLower();
			if (strstr(strFileName, strKeyLower))
			{
				m_CurrentList.push_back(m_vecFileNameHashList[i]);
				FindName.push_back(strFileName);
			}
		}

		for (size_t i = 0; i < m_CurrentList.size(); i++)
		{
			int nIndex = GetCurrentFileListBox().AddString(FindName[i]);
			GetCurrentFileListBox().SetItemData(nIndex, m_CurrentList[i]);
		}
	}
}

void KDlgAnimationTagBatchTool::OnCbnSelchangeComboProcessoption()
{
	UpdateData();
	int nShowMetor = SW_HIDE;
	int nShowSoundVersion3 = SW_HIDE;
	int nCurSel = m_comboProcessOption.GetCurSel();
	KG_PROCESS_ERROR(nCurSel != CB_ERR);

	m_strStaticNew = s_ProcessTypeInfo[nCurSel].strNew;
	m_strStaticOld = s_ProcessTypeInfo[nCurSel].strOld;
	m_editNew.ShowWindow(s_ProcessTypeInfo[nCurSel].nNewShow);
	m_editOld.ShowWindow(s_ProcessTypeInfo[nCurSel].nOldShow);
	GetDlgItem(IDC_STATIC_SUFFIX)->ShowWindow(s_ProcessTypeInfo[nCurSel].nSuffix);
	GetDlgItem(IDC_EDIT_SUFFIX)->ShowWindow(s_ProcessTypeInfo[nCurSel].nSuffix);

	for (size_t i = 0; i < sizeof(s_ExtraControl) / sizeof(int); i++)
	{
		GetDlgItem(s_ExtraControl[i])->ShowWindow(s_ProcessTypeInfo[nCurSel].nExtraControl);
	}
	if(nCurSel == PROCESS_INSERTMETOR)
	{
		nShowMetor = SW_SHOW;
	}
	// add by chenganlai
	for (size_t i = 0 ; i < sizeof(s_IDC_METOR)/sizeof(int) ; i ++)
	{ // 显示刀光的控件
		GetDlgItem(s_IDC_METOR[i])->ShowWindow(nShowMetor);
	}
	if(nCurSel == PROCESS_SOUNDVERSION3)
	{
		nShowSoundVersion3 = SW_SHOW;
		for (size_t i = 0 ; i < sizeof(s_IDC_SoundVersion3)/ sizeof(int) ; i ++)
		{// 显示声音标签版本3
			GetDlgItem(s_IDC_SoundVersion3[i])->ShowWindow(nShowSoundVersion3);
		}
	}
	else
	{
		for (size_t i = 0 ; i < sizeof(s_IDC_SoundVersion3Only) / sizeof(int) ; i ++)
		{
			GetDlgItem(s_IDC_SoundVersion3Only[i])->ShowWindow(SW_HIDE);
		}
	}
	UpdateData(FALSE);
Exit0:
	;
}

void KDlgAnimationTagBatchTool::OnLbnDblclkListPending()
{
	std::set<DWORD>::iterator itErase;
	int nCurSel = m_ListFilePending.GetCurSel();
	KG_PROCESS_ERROR(nCurSel != LB_ERR);

	DWORD dwHash = static_cast<DWORD>(m_ListFilePending.GetItemData(nCurSel));
	itErase = m_setFilePendingHash.find(dwHash);
	KG_PROCESS_ERROR(itErase != m_setFilePendingHash.end());

	m_setFilePendingHash.erase(itErase);
	m_ListFilePending.DeleteString(nCurSel);
Exit0:
	;
}

void KDlgAnimationTagBatchTool::OnLbnSelchangeListFilesSource()
{
	int nSel = m_ListFileSource.GetCurSel();
	KG_PROCESS_ERROR(nSel != LB_ERR);

	DWORD dwHash = static_cast<DWORD>(m_ListFileSource.GetItemData(nSel));
	m_strFileFullPath = g_AnimationTagBatchTool.GetFullPath(m_dwType, dwHash);
	UpdateData(FALSE);
Exit0:
	;
}

void KDlgAnimationTagBatchTool::OnLbnDblclkListFilesSource()
{
	HRESULT hRetCode = E_FAIL;
	KGAnimationTagBatchTool::SrcToDest Tags;
	m_setFilePendingHash.clear();
	m_ListFilePending.ResetContent();

	int nSel = m_ListFileSource.GetCurSel();
	KG_PROCESS_ERROR(nSel != LB_ERR);

	DWORD dwHash = static_cast<DWORD>(m_ListFileSource.GetItemData(nSel));
	hRetCode = g_AnimationTagBatchTool.GetUsedTagFile(dwHash, Tags);

	for (size_t i = 0; i < Tags.vecDest.size(); i++)
	{
		if (!m_setFilePendingHash.count(Tags.vecDest[i].dwHash))
		{
			int nIndex = m_ListFilePending.AddString(Tags.vecDest[i].strFileName);
			m_ListFilePending.SetItemData(nIndex, Tags.vecDest[i].dwHash);
			m_setFilePendingHash.insert(Tags.vecDest[i].dwHash);
		}
	}
Exit0:
	;
}

void KDlgAnimationTagBatchTool::OnLbnSelchangeListPending()
{
	int nCurSel = m_ListFilePending.GetCurSel();
	KG_PROCESS_ERROR(nCurSel != LB_ERR);

	DWORD dwHash = static_cast<DWORD>(m_ListFilePending.GetItemData(nCurSel));
	m_strFileFullPath = g_AnimationTagBatchTool.GetFullPath(s_dwListPendingType[m_dwType], dwHash);
	UpdateData(FALSE);
Exit0:
	;
}

void KDlgAnimationTagBatchTool::OnBnClickedButtonClearpending()
{
	m_ListFilePending.ResetContent();
	m_setFilePendingHash.clear();
}

void KDlgAnimationTagBatchTool::OnBnClickedButtonExportlist()
{
	FILE *fpFile = fopen("exportFileList.txt", "w+");
	KG_PROCESS_ERROR(fpFile);

	CListBox &ListBox = GetCurrentFileListBox();
	int nCount = ListBox.GetCount();
	for (int i = 0; i < nCount; i++)
	{
		DWORD dwHash = static_cast<DWORD>(ListBox.GetItemData(i));
		CString strFullPath = g_AnimationTagBatchTool.GetFullPath(m_dwType, dwHash);
		fprintf(fpFile, "%s\n", strFullPath);
	}
Exit0:
	if (fpFile)
	{
		fclose(fpFile);
		fpFile = NULL;
	}
}
DWORD KDlgAnimationTagBatchTool::GetNameHash(LPCSTR strName)
{
	TCHAR strLower[MAX_PATH];
	strcpy_s(strLower,MAX_PATH,strName);
	_strlwr_s(strLower,MAX_PATH);
	DWORD dwNameHash = g_FileNameHash(strLower);
	return dwNameHash;
}
void KDlgAnimationTagBatchTool::OnCbnSelchangeComboSocket()
{
	// TODO: Add your control notification handler code here
	int nSel = 0;
	nSel = m_CombSocket.GetCurSel();
	KG_PROCESS_ERROR(nSel != CB_ERR);
	{
		TCHAR strSocket[MAX_PATH];
		MetorSelInfo metroSelInfo;
		DWORD dwNameHash;
		m_CombSocket.GetLBText(nSel,strSocket);
		dwNameHash = GetNameHash(strSocket);
		if(MetroSelect.find(dwNameHash) != MetroSelect.end())
		{
			MessageBox("该socket已被添加","ERROR");
			KG_PROCESS_ERROR(FALSE);
		}
		metroSelInfo.bIsMetorOn = FALSE;
		strcpy_s(metroSelInfo.pSocketName,MAX_PATH,strSocket);
		metroSelInfo.dwNameHash = dwNameHash;
		MetroSelect[dwNameHash] = metroSelInfo;
	}
	FillMetorListBox();
Exit0:
	;
}
void KDlgAnimationTagBatchTool::FillMetorListBox()
{
	UpdateData();
	map<DWORD,MetorSelInfo>::iterator iter;
	m_ListBoxSocket.ResetContent();
	for (iter = MetroSelect.begin(); iter != MetroSelect.end() ; iter ++)
	{
		MetorSelInfo metorSelInfo = iter->second;
		m_ListBoxSocket.AddString(metorSelInfo.pSocketName);
	}
	UpdateData(FALSE);
}
void KDlgAnimationTagBatchTool::OnLbnSelchangeListSocket()
{
	// TODO: Add your control notification handler code here
	UpdateData();
	MetorSelInfo metorSelInfo;
	int nSel = m_ListBoxSocket.GetCurSel();
	DWORD dwNameHash = 0;
	TCHAR strSocketName[MAX_PATH];
	KG_PROCESS_ERROR(nSel >= 0);
	m_ListBoxSocket.GetText(nSel,strSocketName);
	dwNameHash = GetNameHash(strSocketName);
	metorSelInfo = MetroSelect[dwNameHash];
	((CButton *)GetDlgItem(IDC_RADIO_CLOSE_METOR))->SetCheck(!metorSelInfo.bIsMetorOn);
	((CButton *)GetDlgItem(IDC_RADIO_OPEN_METOR))->SetCheck(metorSelInfo.bIsMetorOn);
	UpdateData(FALSE);
Exit0:
	;
}
void KDlgAnimationTagBatchTool::SetMeteorSwith(BOOL bMetor)
{
	UpdateData();
	int nSel = m_ListBoxSocket.GetCurSel();
	DWORD dwNameHash = 0;
	TCHAR strSocketName[MAX_PATH];
	MetorSelInfo metorSelInfo; 
	KG_PROCESS_ERROR(nSel >= 0);
	m_ListBoxSocket.GetText(nSel,strSocketName);
	dwNameHash = GetNameHash(strSocketName);
	metorSelInfo = MetroSelect[dwNameHash];
	metorSelInfo.bIsMetorOn = bMetor;
	MetroSelect[dwNameHash] = metorSelInfo;
	OnLbnSelchangeListSocket();
Exit0:
	;
}
void KDlgAnimationTagBatchTool::OnBnClickedRadioCloseMetor()
{
	// TODO: Add your control notification handler code here
	SetMeteorSwith(FALSE);
}

void KDlgAnimationTagBatchTool::OnBnClickedRadioOpenMetor()
{
	// TODO: Add your control notification handler code here
	SetMeteorSwith(TRUE);
}

void KDlgAnimationTagBatchTool::OnLbnDblclkListSocket()
{
	// TODO: Add your control notification handler code here
	TCHAR strSocketName[MAX_PATH];
	DWORD dwNameHash = 0;
	DWORD dwSfxNameHash = 0;
	map<DWORD,MetorSelInfo>::iterator iter;
	int nSel = m_ListBoxSocket.GetCurSel();
	if(nSel < 0)
	{
		MessageBox("没有选中任何Socket","ERROR");
		KG_PROCESS_ERROR(FALSE);
	}
	m_ListBoxSocket.GetText(nSel,strSocketName);
	dwNameHash = GetNameHash(strSocketName);
	iter = MetroSelect.find(dwNameHash);

	if(iter != MetroSelect.end())
	{
		MetroSelect.erase(iter);
	}
	FillMetorListBox();
Exit0:
	;
}
