////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGPropertyEditTabDlg .cpp
//  Version     : 1.0
//  Author      : ZhaoXinyu (Chiyer)
//  CopyRight   : Kingsoft Corporation All rights Reserved
//  History     : 2005-11-29 10:18:28
//  Comment     :
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SceneEditor.h"
#include "KGPropertyEditTabDlg.h"
//#include "KG3dsceneObjectEditor.h"
#include "KSceneObjectEditorPanelView.h"
#include "KGBuildIniFileDlg.h"
#include "KGNPCAniEditSettingDlg.h"
#include "KGValueEditDlg.h"
#include "KGValueEditBoolDlg.h"
#include "KGValueEditStringDlg.h"
#include "KGValueEditMapNpcDlg.h"
#include "KGValueEditMinMaxDlg.h"
#include "KGCreateQuest.h"
#include "KGValueEditQuestTypeDlg.h"
#include "KGValueEditItemTypeIndexNumDlg.h"
#include "KGValueEditForceDlg.h"
#include "KGInsertQuestAlertDlg.h"
#include "KGValueEditGetValueDlg.h"
#include "KGQuestEditSettingDlg.h"
#include "KGInsertQuestAlertDlg.h"
#include "KGQuestEditerAboutDlg.h"
#include "KGLog.h"
#include "Shlwapi.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(KGObjectPropertyEditTabDlg, CDialog)

KGObjectPropertyEditTabDlg::KGObjectPropertyEditTabDlg(
	PROPERTY_EDIT_TYPE eType, KSceneObjectEditorPanelView* pView, CWnd* pParent
): CDialog(KGObjectPropertyEditTabDlg::IDD, pParent)
{
	m_eEditType			 = eType;
	m_pView				 = pView;
	m_pTabFile			 = NULL;
	m_pIniFile			 = NULL;
	m_pQuestIniFile		 = NULL;
	m_nRefustCountFlag   = true;
	m_nFirstResizeFlag   = true;
	m_hAccel			 = NULL;

	m_ppszClassifyFields = NULL;
	m_pszCaptionField    = NULL;
	m_pszPrimaryKesField = NULL;
}

KGObjectPropertyEditTabDlg::~KGObjectPropertyEditTabDlg()
{
	UnInit();
}

void KGObjectPropertyEditTabDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_PROPERTY, m_treeObjectView);
	DDX_Control(pDX, IDC_LIST_PROPERTY, m_listProperty);
	DDX_Control(pDX, IDC_COMBO_TYPE, m_comBoxType);
	DDX_Control(pDX, IDC_PROGRESS, m_proessBar);
	DDX_Control(pDX, IDC_BOOK, m_editBookMarker);
	DDX_Control(pDX, IDC_FRAME, m_iniWinidow);
	DDX_Text(pDX, IDC_BOOK, m_szBookMark);
}

BEGIN_MESSAGE_MAP(KGObjectPropertyEditTabDlg, CDialog)
	ON_NOTIFY(
		TVN_SELCHANGED, IDC_TREE_PROPERTY, OnTvnSelchangedObjectPropertyTree
	)
	ON_NOTIFY(
		NM_RCLICK, IDC_TREE_PROPERTY, OnNMRclickObjectPropertyTree
	)
    ON_NOTIFY(NM_DBLCLK, IDC_LIST_PROPERTY, OnNMDblclkObjectPropertyList)
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_INITMENUPOPUP()

	ON_WM_TIMER()
	ON_COMMAND(ID_OPEN,  OnOpen)
	ON_COMMAND(ID_NEW,   OnNew)

	ON_COMMAND(ID_GLEAN, OnCreateGln)
	ON_COMMAND(ID_KILL,  OnCreateKil)
	ON_COMMAND(ID_POS,   OnCreatePos)
	ON_COMMAND(ID_TALK,  OnCreateTlk)
	ON_COMMAND(ID_OFFER, OnCreateOfr)
	ON_COMMAND(ID_OTHER, OnCreateOtr)

	ON_COMMAND(ID_SAVE,    OnSave)
	ON_COMMAND(ID_SAVE_AS, OnSaveAs)

	ON_COMMAND(ID_RENAME,OnReName)
	ON_COMMAND(ID_FIND,  OnFind)
	ON_COMMAND(ID_DEL,   OnDel)
	ON_COMMAND(ID_COPY,  OnCopy)
	ON_COMMAND(ID_CUT,   OnCut)
	ON_COMMAND(ID_PASE,  OnPase)

	ON_COMMAND(ID_MAP,   OnCreateMap)
	ON_COMMAND(ID_LINK,  OnCreateLnk)
	ON_COMMAND(ID_SUB,   OnCreateSub)

	ON_COMMAND(ID_CLOSE,    OnClose)
	ON_COMMAND(ID_SET,      OnSet)
	ON_COMMAND(ID_HELP,     OnHellp)
	ON_COMMAND(ID_SH_OPEN,  OnQuestSwitchOpen)
	ON_COMMAND(ID_SH_CLOSE, OnQuestSwintcClose)

	ON_COMMAND(ID_OUT,    OnQuestExportFile)
	ON_COMMAND(ID_IN,     OnQuestInportFile)
	ON_COMMAND(ID_OUT_ALL,OnQuestExportFileAll)

	ON_COMMAND(ID_EXPENDALL, OnExpendAll)
	ON_COMMAND(ID_SHRINKALL, OnShrinkAll)

	ON_MESSAGE(WM_USER_UPDATE_TREE_ICON, OnUpdateTreeIcon)
	ON_MESSAGE(WM_USER_BACKUP_TAB_FILE, OnBackUp)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXT, 0, 0xFFFF, OnToolTipText)
	ON_CBN_SELCHANGE(IDC_COMBO_TYPE, OnCbnSelect)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_AND, OnBnClickedCheckAnd)
	ON_BN_CLICKED(IDC_OR,  OnBnClickedCheckOr)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

void KGObjectPropertyEditTabDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	int nResult  = false;
	int nRetCode = false;

	CDialog::OnShowWindow(bShow, nStatus);

	nRetCode = Init();
	KG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	return;
}

int KGObjectPropertyEditTabDlg::Init()
{
	int nResult  = false;
	int nRetCode = false;

	char szNamelpatePath[MAX_PATH];
	*szNamelpatePath	= '\0';
	int  nStringLength  = 0;

	m_szCurBookMarkName = _T("");
	m_szPreBookMarkName = _T("");

	m_hTreeRootItem     = NULL;
	m_hTreeSelItem      = NULL;
	m_nEditerState      = EDIT_STATE_CLOSE;
	m_nBeSavedFlag		= true;
	m_nLButtonFlag		= false;
	m_nSizeBarFlag		= false;

	CBitmap	bitmap;

	m_hAccel = LoadAccelerators(
		AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_OBJECT_PROPERTY_EDIT_DLG)
	);
	m_menuPopup.LoadMenu(IDR_QUEST);

	nRetCode = m_wndToolBar.CreateEx(
		this, TBSTYLE_BUTTON, WS_CHILD | WS_VISIBLE | CBRS_TOP |
		CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC
	);
	KG_PROCESS_ERROR(nRetCode);
	nRetCode = m_wndToolBar.LoadToolBar(IDR_OBJECT_EDIT);

	bitmap.Attach(::LoadImage(AfxGetResourceHandle(), "QUEST_TOOL_BAR_N", IMAGE_BITMAP, 0, 0,LR_DEFAULTSIZE | LR_CREATEDIBSECTION));
	m_NormalList.Create(18, 18, ILC_COLOR24|ILC_MASK, 0, 0);
	m_NormalList.Add(&bitmap,RGB(192,192,192)) ;
	bitmap.Detach();
	m_wndToolBar.SendMessage(TB_SETIMAGELIST,0,(LPARAM)m_NormalList.m_hImageList);

	bitmap.Attach(::LoadImage(AfxGetResourceHandle(), "QUEST_TOOL_BAR_N",IMAGE_BITMAP, 0, 0,LR_DEFAULTSIZE | LR_CREATEDIBSECTION));
	m_HotList.Create(18, 18, ILC_COLOR24|ILC_MASK, 0, 0);
	m_HotList.Add(&bitmap,RGB(192,192,192)) ;
	bitmap.Detach();
	m_wndToolBar.SendMessage(TB_SETHOTIMAGELIST,0,(LPARAM)m_HotList.m_hImageList);

	bitmap.Attach(::LoadImage(AfxGetResourceHandle(), "QUEST_TOOL_BAR_D",IMAGE_BITMAP, 0, 0,LR_DEFAULTSIZE | LR_CREATEDIBSECTION));
	m_DisableList.Create(18, 18, ILC_COLOR24|ILC_MASK, 0, 0);
	m_DisableList.Add(&bitmap,RGB(192,192,192)) ;
	bitmap.Detach();
	m_wndToolBar.SendMessage(TB_SETDISABLEDIMAGELIST,0,(LPARAM)m_DisableList.m_hImageList);

	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);

	static UINT indicators[] =
	{
		ID_SEPARATOR,
		ID_SEPARATOR,
		ID_SEPARATOR,
		ID_SEPARATOR
	};

	nRetCode = m_wndStateBar.Create(this);
	KG_PROCESS_ERROR(nRetCode);
	nRetCode = m_wndStateBar.SetIndicators(indicators, sizeof(indicators) / sizeof(UINT));
	KG_PROCESS_ERROR(nRetCode);

	m_wndStateBar.SetPaneInfo(0, ID_SEPARATOR, SBPS_NOBORDERS | SBPS_STRETCH, 0);
	m_wndStateBar.SetPaneInfo(1, ID_SEPARATOR, SBPS_NORMAL, 200);
	m_wndStateBar.SetPaneInfo(2, ID_SEPARATOR, SBPS_NORMAL, 100);
	m_wndStateBar.SetPaneInfo(3, ID_SEPARATOR, SBPS_NORMAL, 100);

	m_wndStateBar.SetPaneText(0, _T("就绪 >_<"));
	m_wndStateBar.SetPaneText(2, _T("任务个数:"));
	m_wndStateBar.SetPaneText(3, _T("属性条数:"));

	m_proessBar.SetParent(&m_wndStateBar);

	m_treeObjectView.SetBkColor(RGB( 92,  92, 92));
	m_treeObjectView.SetTextColor(RGB(255, 255, 255));
	m_treeObjectView.SetLineColor(RGB(122, 122, 122));

	DWORD dwStyle = GetWindowLong(m_treeObjectView.m_hWnd, GWL_STYLE);
	dwStyle |= TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT;
	SetWindowLong(m_treeObjectView.m_hWnd,GWL_STYLE,dwStyle);

	m_imageList.Create(16, 16, ILC_COLOR24, QUEST_COUNT, QUEST_COUNT);
	m_imageList.Add(AfxGetApp()->LoadIcon(IDC_MAP));
	m_imageList.Add(AfxGetApp()->LoadIcon(IDC_LINK));
	m_imageList.Add(AfxGetApp()->LoadIcon(IDC_MARK));
	m_imageList.Add(AfxGetApp()->LoadIcon(IDC_GLEAN));
	m_imageList.Add(AfxGetApp()->LoadIcon(IDC_KILL));
	m_imageList.Add(AfxGetApp()->LoadIcon(IDC_POS));
	m_imageList.Add(AfxGetApp()->LoadIcon(IDC_TALK));
	m_imageList.Add(AfxGetApp()->LoadIcon(IDC_OFFER));
	m_imageList.Add(AfxGetApp()->LoadIcon(IDC_OTHER));


	dwStyle = GetWindowLong(m_hWnd, GWL_STYLE);
	dwStyle |= WS_SIZEBOX;
	SetWindowLong(m_hWnd, GWL_STYLE, dwStyle);

	m_listProperty.InsertColumn(0, "物件属性名");
	m_listProperty.InsertColumn(1, "物件属性值");
	m_listProperty.SetColumnWidth(0, 212);
	m_listProperty.SetColumnWidth(1, 662);
	m_listProperty.SetBkColor(RGB(92, 92, 92));

	m_listProperty.SetupToolTip(200, RGB(68, 68, 68), RGB(255, 255, 255));
	m_listProperty.SetHoverTime(100);

	m_RectSave.cx    = 248;
	m_RectSave.cy    = 250;
	m_rectTree.right = m_RectSave.cx;
	m_rectList.left  = m_RectSave.cy;

	g_GetFullPath(szNamelpatePath, QUEST_NAMEPLATE_PATH);
	m_szNamePlateFileName = szNamelpatePath;
	g_GetFullPath(szNamelpatePath, TEMP_QUEST_PATH);
	m_szPropertySaveFile  = szNamelpatePath;

	m_pIniFile = g_OpenIniFile(m_szNamePlateFileName.GetBuffer());
	KG_PROCESS_ERROR(m_pIniFile);

	m_nClassifyFieldsSize = 2;
	m_ppszClassifyFields  = new char*[m_nClassifyFieldsSize];
	m_pIniFile->GetInteger(_T("QuestName"), _T("Length"), 0, &nStringLength);
	KG_PROCESS_ERROR(nStringLength);

	for (int i = 0; i < 2; i++)
	{
		m_ppszClassifyFields[i] = new char[nStringLength];
	}
	strncpy(m_ppszClassifyFields[0], _T("MapName"), nStringLength);
	strncpy(m_ppszClassifyFields[1], _T("TaskLinkName"), nStringLength);
	m_pszCaptionField = new char[nStringLength];
	strncpy(m_pszCaptionField, _T("QuestName"), nStringLength);
	m_pszPrimaryKesField = new char[nStringLength];
	strncpy(m_pszPrimaryKesField, _T("QuestID"), nStringLength);

	nRetCode = m_treeObjectView.BandData(
		&m_pTabFile, &m_pQuestIniFile, &m_pIniFile, &m_primaryKeyManager,
		m_ppszClassifyFields, m_nClassifyFieldsSize,
		m_pszCaptionField, m_pszPrimaryKesField, &m_imageList, GetImageIndex
	);
	KG_PROCESS_ERROR(nRetCode);
	nRetCode = m_listProperty.BandData(&m_pTabFile, &m_pIniFile);
	KG_PROCESS_ERROR(nRetCode);

	UpdateGrid(true);
	UpdateToolBarState();

	SetWindowText(_T("任务属性编辑器"));

	BackUp();

	nResult = true;
Exit0:
	if (!m_pIniFile)
	{
		::MessageBox(
			this->m_hWnd,_T("找不到相应的INI配置文件!"), _T("Init Error"), MB_OK
		);
	}
	if (!nResult)
	{
		::MessageBox(
			this->m_hWnd,_T("初此化出错!"), _T("Init Error"), MB_OK
		);
		UnInit();
	}
    return nResult;
}


BOOL KGObjectPropertyEditTabDlg::PreTranslateMessage(MSG* pMsg)
{
	if(m_hAccel)
	{
		if (::TranslateAccelerator(GetSafeHwnd(), m_hAccel, pMsg))
			return TRUE;
	}
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
	{
		return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

int KGObjectPropertyEditTabDlg::GetImageIndex(ITabFile* pTabFile, int nRow)
{
	int  nIndex = 0;
	char szQuestType[256];
	*szQuestType = '\0';

	ASSERT(pTabFile);
	ASSERT(nRow > 2);

	pTabFile->GetString(
		nRow, _T("Class"), "", szQuestType, sizeof(szQuestType)
	);

	if (CString(szQuestType) == _T("收集"))
	{
		nIndex = QUEST_GLN;
	}
	else if (CString(szQuestType) == _T("送信"))
	{
		nIndex = QUEST_POS;
	}
	else if (CString(szQuestType) == _T("杀怪"))
	{
		nIndex = QUEST_KIL;
	}
	else if (CString(szQuestType) == _T("对话"))
	{
		nIndex = QUEST_TAK;
	}
	else if (CString(szQuestType) == _T("贡献"))
	{
		nIndex = QUEST_OFR;
	}
	else
	{
		nIndex = QUEST_OTR;
	}

	return nIndex;

}

int KGObjectPropertyEditTabDlg::CopyIniFileSubset(
	HTREEITEM hTreeItem, IIniFile* pIniFile
)
{
	int nResult   = false;
	int nRetCode  = false;

	char szBookMark[BOOKMARK_SIZE];
	char szKeyName[SECTION_NAME_SIZE];
	char szKeyValue[1024];

	*szKeyValue = '\0';
	*szKeyName  = '\0';
	*szBookMark = '\0';

	CString szSectionName = _T("");
	CString szItemPath    = _T("");

	CString szItemDate    = _T("");
	int		nItemDate     = 0;

	HTREEITEM hTempItem = hTreeItem;
	KG_PROCESS_ERROR(hTempItem);

	KG_PROCESS_ERROR(m_pQuestIniFile);
	KG_PROCESS_ERROR(m_pTabFile);

	nItemDate  = (int)m_treeObjectView.GetItemData(hTreeItem);

	szItemDate.Format(_T("%d"), nItemDate);

	if (
		(nItemDate == SUBSET_TREE_DATA) ||
		(nItemDate < 1 )
	)
	{
		while (hTempItem)
		{
			szItemPath = m_treeObjectView.GetItemText(hTempItem) + szItemPath;
			hTempItem  = m_treeObjectView.GetParentItem(hTempItem);
			if (hTempItem)
			{
				szItemPath = _T("-") + szItemPath;
			}
		}
		m_pQuestIniFile->GetString(
			szItemPath.GetBuffer(), _T("BookMark"), _T(""),
			szBookMark, sizeof(szBookMark)
		);
		nRetCode = pIniFile->WriteString(
			szItemPath.GetBuffer(), _T("BookMark"), szBookMark
		);
		KG_PROCESS_ERROR(nRetCode);
	}

	FindInSubset(m_pQuestIniFile, szItemDate, szSectionName);

	if (szSectionName != _T(""))
	{
		while (
			m_pQuestIniFile->GetNextKey(
				szSectionName.GetBuffer(), szKeyName, szKeyName
			)
		)
		{
			if (CString(szKeyName) != _T("TreeHandle"))
			{
				m_pQuestIniFile->GetString(
					szSectionName.GetBuffer(), szKeyName, _T(""),
					szKeyValue, sizeof(szKeyValue)
				);
				pIniFile->WriteString(
					szSectionName.GetBuffer(), szKeyName, szKeyValue
				);
			}
		}
	}
	else
	{
		CopyIniFileSubset(m_treeObjectView.GetChildItem(hTreeItem), pIniFile);
	}

	CopyIniFileSubset(m_treeObjectView.GetNextSiblingItem(hTreeItem), pIniFile);

	nResult = true;
Exit0:
	szSectionName.ReleaseBuffer();
	szItemDate.ReleaseBuffer();
	szItemPath.ReleaseBuffer();

	return nResult;
}

int KGObjectPropertyEditTabDlg::UpdateGrid(int nEnbale)
{
	OnSize(SIZE_MAXIMIZED, 2, 2);

	return true;
}

void KGObjectPropertyEditTabDlg::OnNew()
{
	int nResult  = false;
	int nRetCode = false;

	int nMainClassNumber = m_comBoxType.GetCount();
	int i = 0;
	int nPropertyNumber = 0;
	int nInsertNumber = 0;
	int nMinId = 0;
	int nMaxId = 0;

	char szKeyName[256];
	char szKeyValue[256];
	*szKeyValue = '\0';
	*szKeyName  = '\0';

	if (!m_nBeSavedFlag)
	{
		nRetCode = ::MessageBox(
			this->m_hWnd, _T("是否保存当前的编辑?"), _T("警告!"),
			MB_YESNOCANCEL | MB_ICONWARNING
		);

		if (nRetCode == IDYES)
		{
			OnSave();
		}
		else if (nRetCode == IDCANCEL)
		{
			KG_PROCESS_SUCCESS(true);
		}
	}

	//ShowCursor(FALSE);
	BusyCursor(this);

	m_szPropertyFileName = "";

	KG_PROCESS_ERROR(m_pIniFile);

	for (i = 0; i < nMainClassNumber; i++)
	{
		m_comBoxType.DeleteString(0);
	}
	while (m_pIniFile->GetNextKey(_T("_MainClass_"), szKeyName, szKeyName))
	{
		m_pIniFile->GetString(
			_T("_MainClass_"), szKeyName, "", szKeyValue, sizeof(szKeyValue)
		);
		m_comBoxType.AddString(szKeyValue);
	}

	m_pIniFile->GetInteger(_T("Main"), _T("Count"), 0, &nPropertyNumber);
	m_proessBar.SetRange32(1, nPropertyNumber);
	m_proessBar.SetPos(1);


	SAFE_RELEASE(m_pTabFile);
	m_pTabFile = g_CreateTabFile();
	KG_PROCESS_ERROR(m_pTabFile);
	SAFE_RELEASE(m_pQuestIniFile);
	m_pQuestIniFile = g_CreateIniFile();
	KG_PROCESS_ERROR(m_pQuestIniFile);

	nRetCode = BuildPrepertyFile(
		m_pTabFile, m_pIniFile, _T("_MainClass_"), &nInsertNumber, &m_proessBar
	);
	KG_PROCESS_ERROR(nRetCode);
	m_proessBar.SetPos(1);

	/*m_wndStateBar.SetPaneText(0, _T("正在构造任务ID管理器..."));
	m_primaryKeyManager.Refrush(1, 40000, &m_proessBar);*/
	m_pIniFile->GetInteger(_T("QuestID"), _T("Min"), 0, &nMinId);
	m_pIniFile->GetInteger(_T("QuestID"), _T("Max"), 0, &nMaxId);

	if (nMinId < 0 || nMaxId < 0 || nMinId >= nMaxId || nMaxId > INT_MAX)
	{
		MessageBox(
			_T("ID管理器初始化出错,请检查培植文件!!"), _T("导入出错!!"),
			MB_OK | MB_ICONERROR
		);
		KGLOG_PROCESS_ERROR(false);
	}

	m_wndStateBar.SetPaneText(0, _T("正在构造任务ID管理器....."));
	m_primaryKeyManager.Refrush(nMinId, nMaxId, &m_proessBar, &m_wndStateBar);

	m_wndStateBar.SetPaneText(0, _T("正在初此化任务ID管理器..."));
	g_IniPrimaryKesManager(
		&m_primaryKeyManager, &m_pTabFile, 1, _T("QuestID"), &m_proessBar
	);
	m_wndStateBar.SetPaneText(0, _T("就绪"));

	m_szCurBookMarkName = _T("");
	m_szPreBookMarkName = _T("");
	m_szBookMark        = _T("");

	UpdateData(false);

	m_treeObjectView.Update();
	m_wndStateBar.SetPaneText(0, _T("正在编辑 ^_^"));
	m_wndStateBar.SetPaneText(3, _T("属性条数 : "));
	m_wndStateBar.SetPaneText(2, _T("任务个数 : "));

	SetWindowText("任务编辑器->正在编辑任务文件 : 新建文件");
	m_nEditerState = EDIT_STATE_EDIT_BOOKMARK;

	UpdateToolBarState();
	UpdateGrid(true);

	m_nBeSavedFlag = false;

	KillTimer(1);
	SetTimer(1, 5000, NULL);

Exit1:
	nResult = true;
Exit0:
	if (!nResult)
	{
		m_nEditerState = EDIT_STATE_CLOSE;
		UpdateGrid(true);
	}
	FreeCursor(this);

	//ShowCursor(TRUE);
	return;
}

void KGObjectPropertyEditTabDlg::OnOpen()
{
	int  nResult  = false;
	int  nRetCode = false;

	int  nMainClassNumber = 0;
	int  i = 0;
	int  nLoadNumber = 0;
	int  nMinId = 0;
	int  nMaxId = 0;

	char szSectionName[SECTION_NAME_SIZE];
	char szKeyName[KEY_NAME_SIZE];
	char szKeyValue[256];

	*szSectionName = '\0';
	*szKeyValue = '\0';
	*szKeyName  = '\0';

	CString szQuestIniFile;
	CString szLoadNumber = "";

	CFileDialog dlg(
		true, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"Quest File|*.tab|All File|*.*|", this
	);

	if (!m_nBeSavedFlag)
	{
		nRetCode = ::MessageBox(
			this->m_hWnd, _T("是否保存当前的编辑?"), _T("警告!"),
			MB_YESNOCANCEL | MB_ICONWARNING
			);

		if (nRetCode == IDYES)
		{
			OnSave();
			KG_PROCESS_SUCCESS(true);
		}
		else if (nRetCode == IDCANCEL)
		{
			KG_PROCESS_SUCCESS(true);
		}
	}

	if (dlg.DoModal() == IDOK)
	{
		//ShowCursor(FALSE);
		BusyCursor(this);

		m_szPropertyFileName = dlg.GetPathName();
		szQuestIniFile = m_szPropertyFileName;

		SAFE_RELEASE(m_pTabFile);
		SAFE_RELEASE(m_pQuestIniFile);

		m_pTabFile = g_OpenTabFile(
			m_szPropertyFileName.GetBuffer(), false, true
		);
		KG_PROCESS_ERROR(m_pTabFile);

		//m_wndStateBar.SetPaneText(0, _T("正在构造任务ID管理器..."));
		//m_primaryKeyManager.Refrush(1, 40000, &m_proessBar);
		m_pIniFile->GetInteger(_T("QuestID"), _T("Min"), 0, &nMinId);
		m_pIniFile->GetInteger(_T("QuestID"), _T("Max"), 0, &nMaxId);

		if (nMinId < 0 || nMaxId < 0 || nMinId >= nMaxId || nMaxId > INT_MAX)
		{
			MessageBox(
				_T("ID管理器初始化出错,请检查培植文件!!"), _T("导入出错!!"),
				MB_OK | MB_ICONERROR
			);
			KGLOG_PROCESS_ERROR(false);
		}

		m_wndStateBar.SetPaneText(0, _T("正在构造任务ID管理器....."));
		m_primaryKeyManager.Refrush(nMinId, nMaxId, &m_proessBar, &m_wndStateBar);


		m_wndStateBar.SetPaneText(0, _T("正在初此化任务ID管理器..."));
		g_IniPrimaryKesManager(
			&m_primaryKeyManager, &m_pTabFile, 1, _T("QuestID"), &m_proessBar
		);
		m_wndStateBar.SetPaneText(0, _T("就绪"));
		m_wndStateBar.SetPaneText(3, _T("属性条数 : "));
		m_wndStateBar.SetPaneText(2, _T("任务个数 : "));

		SetWindowText(
			_T("任务编辑器->正在编辑任务文件:") + m_szPropertyFileName
		);

		szQuestIniFile.Replace(".tab", ".ini");
		m_pQuestIniFile = g_OpenIniFile(szQuestIniFile.GetBuffer(), false, true);

		//////////
		//
		while (m_pQuestIniFile->GetNextSection(szSectionName, szSectionName))
		{
			*szKeyName = '\0';
			while (m_pQuestIniFile->GetNextKey(szSectionName, szKeyName, szKeyName))
			{
				nRetCode = strcmp(szKeyName, _T("TreeHandle"));
				if (!nRetCode)
					m_pQuestIniFile->EraseKey(szSectionName, szKeyName);
			}
		}
		//
		////////////

		nMainClassNumber = m_comBoxType.GetCount();
		for (i = 0; i < nMainClassNumber; i++)
		{
			m_comBoxType.DeleteString(0);
		}

		*szKeyName = '\0';
		while (m_pIniFile->GetNextKey(_T("_MainClass_"), szKeyName, szKeyName))
		{
			m_pIniFile->GetString(
				_T("_MainClass_"), szKeyName, szKeyValue,
				szKeyValue, sizeof(szKeyValue)
			);
			m_comBoxType.AddString(szKeyValue);
		}

		nRetCode = m_treeObjectView.Update(&nLoadNumber);
		KG_PROCESS_ERROR(nRetCode);


		m_szCurBookMarkName = _T("");
		m_szPreBookMarkName = _T("");
		m_szBookMark        = _T("");
		UpdateData(false);

		m_nEditerState = EDIT_STATE_EDIT_BOOKMARK;

		m_nBeSavedFlag = true;

		szLoadNumber.Format("任务个数 : %d", nLoadNumber);
		m_wndStateBar.SetPaneText(0, _T("正在编辑 ^_^"));
		m_wndStateBar.SetPaneText(2, szLoadNumber.GetBuffer());

		UpdateToolBarState();
		UpdateGrid(true);

		BackUp();

		KillTimer(1);
		SetTimer(1, 5000, NULL);
	}

Exit1:
	nResult = true;
Exit0:
	//ShowCursor(TRUE);
	FreeCursor(this);

	return;
}

void KGObjectPropertyEditTabDlg::OnClose()
{
	int nResult  = false;
	int nRetCode = false;

	if (!m_nBeSavedFlag)
	{
		nRetCode = ::MessageBox(
			this->m_hWnd, _T("是否保存当前的编辑?"), _T("警告!"),
			MB_YESNOCANCEL | MB_ICONWARNING
		);
		if (nRetCode == IDYES)
		{
			OnSave();
		}
		else if (nRetCode == IDCANCEL)
		{
			KG_PROCESS_SUCCESS(true);
		}
		else
		{
			m_nBeSavedFlag = true;
		}
	}

	SAFE_RELEASE(m_pTabFile);
	SAFE_RELEASE(m_pQuestIniFile);

	m_treeObjectView.Update();
	UpdateProperty();

	m_szPropertyFileName = "";
	SetWindowText("任务编辑_" + m_szPropertyFileName);

	m_wndStateBar.SetPaneText(0, _T("就绪 >_<"));
	m_nEditerState = EDIT_STATE_CLOSE;

	UpdateGrid(true);

	KillTimer(1);

Exit1:
	nResult = true;
//Exit0:
	return;
}

void KGObjectPropertyEditTabDlg::OnQuestSwitchOpen()
{
	int nResult  = false;
	int nRetCode = false;

	int nQuestSwitch = 0;

	m_pTabFile->GetInteger(
		m_nCurRow, _T("IsAvailable"), 0, &nQuestSwitch
	);

	if (nQuestSwitch)
	{
		KG_PROCESS_SUCCESS(true);
	}
	else
	{
		m_pTabFile->WriteInteger(m_nCurRow , _T("IsAvailable"), 1);
		m_menuPopup.CheckMenuItem(ID_SH_CLOSE, MF_UNCHECKED);
		m_menuPopup.CheckMenuItem(ID_SH_OPEN,  MF_CHECKED);
	}

	m_listProperty.UpdateProperty(m_nCurRow);

Exit1:
	nResult = true;
//Exit0:
	return;
}

void KGObjectPropertyEditTabDlg::OnQuestSwintcClose()
{
	int nResult  = false;
	int nRetCode = false;

	int nQuestSwitch = 0;

	m_pTabFile->GetInteger(
		m_nCurRow, _T("IsAvailable"), 0, &nQuestSwitch
	);

	if (!nQuestSwitch)
	{
		KG_PROCESS_SUCCESS(true);
	}
	else
	{
		m_pTabFile->WriteInteger(m_nCurRow , _T("IsAvailable"), 0);
		m_menuPopup.CheckMenuItem(ID_SH_CLOSE, MF_CHECKED);
		m_menuPopup.CheckMenuItem(ID_SH_OPEN,  MF_UNCHECKED);
	}

	m_listProperty.UpdateProperty(m_nCurRow);

Exit1:
	nResult = true;
//Exit0:
	return;
}

void KGObjectPropertyEditTabDlg::OnHellp()
{
	KGQuestEditerAboutDlg aboutDlg(this);
	aboutDlg.DoModal();
}

void KGObjectPropertyEditTabDlg::OnSet()
{
	int nResult  = false;
	int nRetCode = false;

	KGQuestEditSettingDlg dlgSetting(this);

	KG_PROCESS_ERROR(m_pTabFile);
	KG_PROCESS_ERROR(m_pIniFile);

	KG_PROCESS_ERROR(dlgSetting.DoModal() == IDOK);

	nRetCode = m_pIniFile->Save(m_szNamePlateFileName.GetBuffer());
	KG_PROCESS_ERROR(nRetCode);
	m_pIniFile->Release();
	m_pIniFile = g_OpenIniFile(m_szNamePlateFileName.GetBuffer());
	KG_PROCESS_ERROR(m_pIniFile);

	nResult = true;
Exit0:
	return m_treeObjectView.Invalidate();
}

void KGObjectPropertyEditTabDlg::OnCbnSelect()
{
	int nResult  = false;
	int nRetCode = false;

	CString szJumpTitle = _T("");
	CSize   sizeScroll;
	CRect	rectItem;

	int nJumpIndex  = 0;

	nRetCode = ::IsWindow(m_listProperty.m_hWnd);
	KGLOG_PROCESS_ERROR(nRetCode);

	nJumpIndex = m_comBoxType.GetCurSel();
	KGLOG_PROCESS_ERROR(nJumpIndex != -1);

	m_listProperty.GetItemRect(nJumpIndex, &rectItem, true);
	m_comBoxType.GetLBText(nJumpIndex, szJumpTitle);

	nRetCode = m_listProperty.FindItemByText(szJumpTitle);
	KGLOG_PROCESS_ERROR(nRetCode);

	nJumpIndex  = m_listProperty.FindItemPos(m_listProperty.FindNextItem());
	KGLOG_PROCESS_ERROR(nJumpIndex != -1);

	ZeroMemory(&sizeScroll, sizeof(sizeScroll));
	sizeScroll.cy =
		(nJumpIndex - m_listProperty.GetTopIndex()) * rectItem.Height();

	m_listProperty.Scroll(sizeScroll);

	nResult = true;
Exit0:
	return;
}

BOOL KGObjectPropertyEditTabDlg::OnToolTipText(
	UINT, NMHDR* pNMHDR, LRESULT* pResult
)
{
	int nResult  = false;
	int nRetCode = false;

	ASSERT(pNMHDR->code == TTN_NEEDTEXT);

	TOOLTIPTEXT* pTTTA = (TOOLTIPTEXT*)pNMHDR;

	TCHAR   szFullText[512];
	CString strTipText;
	UINT_PTR nID = (UINT_PTR)pNMHDR->idFrom;

	if (pNMHDR->code == TTN_NEEDTEXT && (pTTTA->uFlags & TTF_IDISHWND))
	{
		nID = ((UINT_PTR)(WORD)::GetDlgCtrlID((HWND)nID));
	}

	::LoadString(AfxGetInstanceHandle(), (UINT)nID, szFullText, sizeof(szFullText));
	strTipText = szFullText;

	strncpy(pTTTA->szText, strTipText, sizeof(pTTTA->szText));

	*pResult = 0;

	::SetWindowPos(
		pNMHDR->hwndFrom, HWND_TOP, 0, 0, 0, 0,
		SWP_NOACTIVATE | SWP_NOSIZE  | SWP_NOMOVE
	);

	nResult = true;
//Exit0:
	return nResult;
}

void KGObjectPropertyEditTabDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_nLButtonFlag = true;
	if (point.x >= m_rectTree.right && point.x <= m_rectList.left)
	{
		m_nSizeBarFlag = true;
	}
	SetCapture();
}

void KGObjectPropertyEditTabDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_nLButtonFlag = false;
	m_nSizeBarFlag = false;
	ReleaseCapture();
}

void KGObjectPropertyEditTabDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	if (point.x >= m_rectTree.right && point.x <= m_rectList.left)
	{
		HCURSOR hCr = LoadCursor(NULL, IDC_SIZEWE);
		SetClassLongPtr(this->m_hWnd, GCL_HCURSOR, (LONG)(LONG_PTR)hCr);
	}
	else
	{
		HCURSOR hCr = LoadCursor(NULL, IDC_ARROW);
		SetClassLongPtr(this->m_hWnd, GCL_HCURSOR, (LONG)(LONG_PTR)hCr);
	}

	if (m_nLButtonFlag && m_nSizeBarFlag)
	{
		m_rectTree.right = point.x;
		m_rectList.left  = point.x + 2;
		m_treeObjectView.MoveWindow(&m_rectTree);
		m_listProperty.MoveWindow(&m_rectList);
		m_editBookMarker.MoveWindow(&m_rectList);
		m_RectSave.cx    = m_rectTree.right;
		m_RectSave.cy    = m_rectList.left;
	}
}

void KGObjectPropertyEditTabDlg::OnCreateMap()
{
	int nResult  = false;
	int nRetCote = false;

	CString szMapName     = _T("");
	CString szTempName    = _T("");
	int nTreeItemData     = 0;
	int nDlgReturn		  = false;

	HTREEITEM hTempItem   = NULL;
	HTREEITEM hInsertItem = NULL;

	KGValueEditGetValueDlg dlg(this, _T("请输入要创建的[地图]名 :"), 64);

	// Update 2006-4-22
	while (dlg.DoModal() == IDOK)
	{
		szMapName = dlg.GetValue();
		szMapName.TrimRight();
		szMapName.TrimLeft();
		if (szMapName == _T(""))
		{
			MessageBox(_T("地图名不能为空!!"), _T("Error!!"), MB_OK | MB_ICONWARNING);
			continue;
		}

		hTempItem = m_treeObjectView.GetRootItem();
		while (hTempItem)
		{
			szTempName = m_treeObjectView.GetItemText(hTempItem);
			if (szTempName == szMapName)
			{
				MessageBox(_T("此地图已经存在!!"), _T("Error!!"), MB_OK | MB_ICONWARNING);
				break;
			}
			hTempItem = m_treeObjectView.GetNextSiblingItem(hTempItem);
		}

		if (hTempItem)
		{
			continue;
		}

		nDlgReturn = true;
		break;
	}
	KG_PROCESS_ERROR(nDlgReturn);
	//

	hInsertItem = m_treeObjectView.InsertItem(
		szMapName.GetBuffer(), QUEST_MAP, QUEST_MAP,
		TVI_ROOT, m_hTreeSelItem
	);
	KG_PROCESS_ERROR(hInsertItem);
	m_treeObjectView.SetItemData(hInsertItem, 0);
	m_treeObjectView.SelectItem(hInsertItem);

	nResult = true;
Exit0:
	return;
}

void KGObjectPropertyEditTabDlg::OnCreateLnk()
{
	int nResult  = false;
	int nRetCote = false;

	CString szMapName     = _T("");
	CString szTempName    = _T("");
	int nTreeItemData     = 0;
	int nDlgReturn		  = false;

	HTREEITEM hTempItem   = NULL;
	HTREEITEM hInsertItem = NULL;

	KGValueEditGetValueDlg dlg(this, _T("请输入要创建的[任务链]名 :"), 64);

	KG_PROCESS_ERROR(m_hTreeSelItem);
	nTreeItemData = (int)m_treeObjectView.GetItemData(m_hTreeSelItem);

	// Update 2006-4-22
	if (nTreeItemData == 0)
	{
		hTempItem = m_treeObjectView.GetChildItem(m_hTreeSelItem);
	}
	else if (nTreeItemData == -1)
	{
		hTempItem = m_treeObjectView.GetParentItem(m_hTreeSelItem);
		hTempItem = m_treeObjectView.GetChildItem(hTempItem);
	}

	while (dlg.DoModal() == IDOK)
	{
		szMapName = dlg.GetValue();
		szMapName.TrimRight();
		szMapName.TrimLeft();
		if (szMapName == _T(""))
		{
			MessageBox(_T("任务链名不能为空!!"), _T("Error!!"), MB_OK | MB_ICONWARNING);
			continue;
		}

		while (hTempItem)
		{
			szTempName = m_treeObjectView.GetItemText(hTempItem);
			if (szTempName == szMapName)
			{
				MessageBox(_T("此任务链已经存在!!"), _T("Error!!"), MB_OK | MB_ICONWARNING);
				break;
			}
			hTempItem = m_treeObjectView.GetNextSiblingItem(hTempItem);
		}

		if (hTempItem)
		{
			continue;
		}
		nDlgReturn = true;
		break;
	}
	KG_PROCESS_ERROR(nDlgReturn);
	//

	if (nTreeItemData == 0)
	{
		hInsertItem = m_treeObjectView.InsertItem(
			szMapName.GetBuffer(), QUEST_LNK, QUEST_LNK, m_hTreeSelItem
		);
		KG_PROCESS_ERROR(hInsertItem);
	}
	else if (nTreeItemData == -1)
	{
		hInsertItem = m_treeObjectView.InsertItem(
			szMapName.GetBuffer(),QUEST_LNK, QUEST_LNK,
			m_treeObjectView.GetParentItem(m_hTreeSelItem),
			m_hTreeSelItem
		);
		KG_PROCESS_ERROR(hInsertItem);
	}
	else
	{

	}

	KG_PROCESS_ERROR(hInsertItem);
	m_treeObjectView.SetItemData(hInsertItem, (DWORD_PTR)-1);
	m_treeObjectView.SelectItem(hInsertItem);

	nResult = true;
Exit0:
	return;
}

void KGObjectPropertyEditTabDlg::OnCreateSub()
{
	int nResult  = false;
	int nRetCote = false;

	CString szSubsetName = "";
	int nTreeItemData = 0;
	HTREEITEM hInsertItem = NULL;

	KGValueEditGetValueDlg dlg(this, _T("请输入要创建的[任务集]名 :"), 64);
	if (dlg.DoModal() == IDOK)
	{
		szSubsetName = dlg.GetValue();
	}
	KG_PROCESS_ERROR(szSubsetName != "");

	nTreeItemData = (int)m_treeObjectView.GetItemData(m_hTreeSelItem);

	if (nTreeItemData == - 1)
	{
		hInsertItem = m_treeObjectView.InsertItem(
			szSubsetName.GetBuffer(), QUEST_MAK, QUEST_MAK, m_hTreeSelItem
		);
	}
	else if (nTreeItemData == SUBSET_TREE_DATA)
	{
		hInsertItem = m_treeObjectView.InsertItem(
			szSubsetName.GetBuffer(), QUEST_MAK, QUEST_MAK,
			m_treeObjectView.GetParentItem(m_hTreeSelItem),
			m_hTreeSelItem
		);
	}
	else if (nTreeItemData >= 1)
	{
		CString szTreeItemData;
		CString szTemp;
		szTreeItemData.Format("%d", nTreeItemData);
		nRetCote = FindInSubset(m_pQuestIniFile, szTreeItemData, szTemp);
		if (nRetCote)
		{
		}
		else
		{
			hInsertItem = m_treeObjectView.InsertItem(
				szSubsetName.GetBuffer(), QUEST_MAK, QUEST_MAK,
				m_treeObjectView.GetParentItem(m_hTreeSelItem),
				m_hTreeSelItem
			);
		}
	}
	else
	{
	}
	KG_PROCESS_ERROR(hInsertItem);
	m_treeObjectView.SetItemData(hInsertItem, SUBSET_TREE_DATA);
	m_treeObjectView.SelectItem(hInsertItem);

	nResult = true;
Exit0:
	return;
}

int KGObjectPropertyEditTabDlg::CreateOneQuest(QUEST_TYPE eQuestType)
{
	int nResult  = false;
	int nRetCode = false;

	int nLaodNumber = 0;
	CString szLoadNumber = "";

	CString szQuestName = _T("");
	CString szQuestType = _T("");

	HTREEITEM hPrevDataItem = NULL;
	HTREEITEM hNextDataItem = NULL;
	HTREEITEM hInsertItem = NULL;
	HTREEITEM hParentItem = NULL;
	HTREEITEM hAfterItem  = NULL;
	HTREEITEM hPreQstItem = NULL;
	HTREEITEM hAftQstItem = NULL;

	int nSourItemPos  = 0;
	int nDestItemPos  = 0;
	int nTreeItemData = 0;
	int nRowNumber    = 0;
	int nItemData     = 0;

	switch (eQuestType)
	{
	case QUEST_GLN :
		szQuestType = _T("收集");
		break;
	case QUEST_POS :
		szQuestType = _T("送信");
		break;
	case QUEST_KIL :
		szQuestType = _T("杀怪");
		break;
	case QUEST_TAK :
		szQuestType = _T("对话");
		break;
	case QUEST_OFR :
		szQuestType = _T("贡献");
		break;
	default :
		szQuestType = _T("其他");
		break;
	}

	KGValueEditGetValueDlg dlg(
		this, _T("请输入要创建的[") + szQuestType +_T("]名 :"), 32
	);
	if (dlg.DoModal() == IDOK)
	{
		szQuestName = dlg.GetValue();
	}
	KG_PROCESS_ERROR(szQuestName != _T(""));

	nRowNumber = m_pTabFile->GetHeight();
	KG_PROCESS_ERROR(nRowNumber >= 2);

	nTreeItemData = (int)m_treeObjectView.GetItemData(m_hTreeSelItem);

	if (nTreeItemData == -1)
	{
		hParentItem = m_hTreeSelItem;
		hAfterItem  = NULL;
		hPreQstItem = NULL;
		hAftQstItem = NULL;
	}
	else if (nTreeItemData == SUBSET_TREE_DATA)
	{
		hParentItem = m_hTreeSelItem;//m_treeObjectView.GetParentItem(m_hTreeSelItem);
		hAfterItem  = NULL;
		hPreQstItem = m_treeObjectView.GetPrevDataItem(m_hTreeSelItem);
		hAftQstItem = m_treeObjectView.GetNextDataItem(m_hTreeSelItem);
										//GetNextSiblingItem(m_hTreeSelItem);
	}
	else if (nTreeItemData >= 1)
	{
		hParentItem = m_treeObjectView.GetParentItem(m_hTreeSelItem);
		hAfterItem  = m_hTreeSelItem;
		hPreQstItem = hAfterItem;
		nItemData   = (int)m_treeObjectView.GetItemData(hParentItem);
		if (nItemData == SUBSET_TREE_DATA)
			hAftQstItem = m_treeObjectView.GetNextDataItem(hParentItem);//GetNextSiblingItem(hParentItem);
		else
			hAftQstItem = m_treeObjectView.GetNextDataItem(hPreQstItem);//GetNextSiblingItem(hPreQstItem);
	}
	else
	{
		hParentItem = NULL;
		hAfterItem  = NULL;
		hPreQstItem = NULL;
		hAftQstItem = NULL;
	}

	hInsertItem = m_treeObjectView.InsertItem(
		szQuestName, eQuestType, eQuestType, hParentItem, hAfterItem
	);

	nSourItemPos = 2;	// 2 Is The Define Value Row In Quest Tab File

	hPrevDataItem = m_treeObjectView.GetPrevDataItem(hInsertItem);
	//hNextDataItem = m_treeObjectView.GetNextDataItem(hInsertItem);

	nRetCode = m_treeObjectView.ItemIsInSubset(hInsertItem);
	if (nRetCode)
	{
		hNextDataItem = m_treeObjectView.GetNextDataItem(
			m_treeObjectView.GetLastSibingItem(hInsertItem)
		);
	}
	else
	{
		hNextDataItem = m_treeObjectView.GetNextDataItem(hInsertItem);
	}

	if (hPrevDataItem)
	{
		nDestItemPos = m_treeObjectView.GetItemPos(hPrevDataItem);
	}
	else
	{
		nDestItemPos = 2;
	}

	KG_PROCESS_ERROR(nDestItemPos >= 2);

	m_pTabFile->InsertAfter(nDestItemPos++);
	nRetCode = m_treeObjectView.CopyOneRow(nSourItemPos, nDestItemPos);
	KG_PROCESS_ERROR(nRetCode);

	nRetCode  = m_primaryKeyManager.GetKey(&nItemData);
	KG_PROCESS_ERROR(nRetCode);
	nRetCode  = m_treeObjectView.SetItemData(hInsertItem, nItemData);
	KG_PROCESS_ERROR(nRetCode);
	nRetCode  = m_pTabFile->WriteInteger(
		nDestItemPos, _T("QuestID"), nItemData
	);
	KG_PROCESS_ERROR(nRetCode);
	nRetCode = m_pTabFile->WriteString(
		nDestItemPos, _T("Class"), szQuestType.GetBuffer()
	);
	KG_PROCESS_ERROR(nRetCode);
	nRetCode = m_treeObjectView.RefrushTypeInfo(hInsertItem);
	KG_PROCESS_ERROR(nRetCode);
	nRetCode = m_treeObjectView.UpdateInfoRelating(hInsertItem);
	KG_PROCESS_ERROR(nRetCode);

	nRetCode = m_treeObjectView.ItemIsInSubset(hNextDataItem);

	if (nRetCode)
	{
		while (hNextDataItem)
		{
			nRetCode = m_treeObjectView.UpdateInfoRelating(hNextDataItem);
			KG_PROCESS_ERROR(nRetCode);
			hNextDataItem = m_treeObjectView.GetNextSiblingItem(hNextDataItem);
		}
	}
	else
	{
		nRetCode = m_treeObjectView.UpdateInfoRelating(hNextDataItem);
		//KG_PROCESS_ERROR(nRetCode);
	}
	m_treeObjectView.SelectItem(hInsertItem);

	/*nLaodNumber = m_pTabFile->GetHeight() - 2;
	szLoadNumber.Format(_T("任务个数 : %d"), nLaodNumber);
	m_wndStateBar.SetPaneText(2, szLoadNumber.GetBuffer());*/
	UpdateStateBar();

	m_nBeSavedFlag = false;
	SendMessage(WM_USER_BACKUP_TAB_FILE);

	nResult = true;
Exit0:
	return nResult;
}

void KGObjectPropertyEditTabDlg::OnCreateGln()
{
	int nResult  = false;
	int nRetCode = false;

	nRetCode = CreateOneQuest(QUEST_GLN);
	KG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	return;
}

void KGObjectPropertyEditTabDlg::OnCreateKil()
{
	int nResult  = false;
	int nRetCode = false;

	nRetCode = CreateOneQuest(QUEST_KIL);
	KG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	return;
}

void KGObjectPropertyEditTabDlg::OnCreatePos()
{
	int nResult  = false;
	int nRetCode = false;

	nRetCode = CreateOneQuest(QUEST_POS);
	KG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	return;
}

void KGObjectPropertyEditTabDlg::OnCreateTlk()
{
	int nResult  = false;
	int nRetCode = false;

	nRetCode = CreateOneQuest(QUEST_TAK);
	KG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	return;
}

void KGObjectPropertyEditTabDlg::OnCreateOfr()
{
	int nResult  = false;
	int nRetCode = false;

	nRetCode = CreateOneQuest(QUEST_OFR);
	KG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	return;
}

void KGObjectPropertyEditTabDlg::OnCreateOtr()
{
	int nResult  = false;
	int nRetCode = false;

	nRetCode = CreateOneQuest(QUEST_OTR);
	KG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	return;
}


void KGObjectPropertyEditTabDlg::OnCopy()
{
	if (GetFocus() == &m_treeObjectView)
	{
		HTREEITEM hItem = m_treeObjectView.GetSelectedItem();
		if (hItem)
			m_treeObjectView.SendMessage(WM_TR_COPY, (WPARAM)hItem, (LPARAM)MOVE_TYPE_COPY);
	}
	else if (GetFocus() == &m_listProperty)
	{

	}
}

void KGObjectPropertyEditTabDlg::OnCut()
{
	if (GetFocus() == &m_treeObjectView)
	{
		HTREEITEM hItem = m_treeObjectView.GetSelectedItem();
		if (hItem)
			m_treeObjectView.SendMessage(WM_TR_COPY, (WPARAM)hItem, (LPARAM)MOVE_TYPE_CUT);
	}
	else if (GetFocus() == &m_listProperty)
	{
	}
}


void KGObjectPropertyEditTabDlg::OnPase()
{
	int nResult  = false;
	int nRetCode = false;

	if (GetFocus() == &m_treeObjectView)
	{
		HTREEITEM hItem = m_treeObjectView.GetSelectedItem();
		if (hItem)
			m_treeObjectView.SendMessage(WM_TR_STICK, (WPARAM)hItem);
	}
	else if (GetFocus() == &m_listProperty)
	{

	}

	nResult = true;
//Exit0:
	return;
}

void KGObjectPropertyEditTabDlg::OnFind()
{
	if (m_nEditerState != EDIT_STATE_CLOSE)
	{
		KGProperytEditFinderDlg* pDlg =
			new KGProperytEditFinderDlg(&m_listProperty, NULL);
		pDlg->Create(IDD_DIALOG26, this);
		pDlg->ShowWindow(SW_SHOW);
	}
}

void KGObjectPropertyEditTabDlg::OnReName()
{
	m_treeObjectView.EditLabel(m_treeObjectView.GetSelectedItem());
}

void KGObjectPropertyEditTabDlg::OnDel()
{
	if (::GetFocus() == m_treeObjectView.m_hWnd)
	{
		if (
			::MessageBox(
				this->m_hWnd, "是否要删除任务", "Message", MB_YESNO
			) == IDYES
		)
		{
			HTREEITEM hItem = m_treeObjectView.GetSelectedItem();
			if (hItem)
				m_treeObjectView.SendMessage(WM_TR_DEL, (WPARAM)hItem);
		}
	}
	if (::GetFocus() == m_listProperty.m_hWnd)
	{

	}
}

void KGObjectPropertyEditTabDlg::OnExpendAll()
{
	if (::GetFocus() == m_listProperty.m_hWnd)
	{
		m_listProperty.ExpandAll();
	}
	else if (::GetFocus() == m_treeObjectView.m_hWnd)
	{
		m_treeObjectView.ExpendAll();
	}
}

void KGObjectPropertyEditTabDlg::OnShrinkAll()
{
	if (::GetFocus() == m_listProperty.m_hWnd)
	{
		m_listProperty.ShrinkAll();
	}
	else
	{
		m_treeObjectView.ShrinkAll();
	}
}


void KGObjectPropertyEditTabDlg::OnInitMenuPopup(
	CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu
)
{
	int nReuslt  = false;
	int nRetCode = false;

	int nTreeItemData = 0;
	int nQuestSwitch  = 0;

	CString szTemp;

	CMenu* pMenuMain = GetMenu();
	KG_PROCESS_ERROR(pMenuMain);

	CDialog::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);
	m_hTreeSelItem = m_treeObjectView.GetSelectedItem();

	if (pPopupMenu == pMenuMain->GetSubMenu(0))
	{
		if (m_nEditerState == EDIT_STATE_CLOSE)
		{
			pPopupMenu->EnableMenuItem(ID_IN,      MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			pPopupMenu->EnableMenuItem(ID_OUT,     MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			pPopupMenu->EnableMenuItem(ID_OUT_ALL, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			pPopupMenu->EnableMenuItem(ID_SET,     MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		}
		else
		{
			pPopupMenu->EnableMenuItem(ID_IN,      MF_ENABLED);
			pPopupMenu->EnableMenuItem(ID_OUT,     MF_ENABLED);
			pPopupMenu->EnableMenuItem(ID_OUT_ALL, MF_ENABLED);
			pPopupMenu->EnableMenuItem(ID_SET,     MF_ENABLED);
		}

	}

	if (pPopupMenu == pMenuMain->GetSubMenu(1) || pPopupMenu == m_menuPopup.GetSubMenu(0))
	{
		pPopupMenu->EnableMenuItem(
			ID_GLEAN, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED
		);
		pPopupMenu->EnableMenuItem(
			ID_KILL,  MF_BYCOMMAND | MF_DISABLED | MF_GRAYED
		);
		pPopupMenu->EnableMenuItem(
			ID_POS,   MF_BYCOMMAND | MF_DISABLED | MF_GRAYED
		);
		pPopupMenu->EnableMenuItem(
			ID_TALK,  MF_BYCOMMAND | MF_DISABLED | MF_GRAYED
		);
		pPopupMenu->EnableMenuItem(
			ID_LINK,  MF_BYCOMMAND | MF_DISABLED | MF_GRAYED
		);
		pPopupMenu->EnableMenuItem(
			ID_MAP,   MF_BYCOMMAND | MF_DISABLED | MF_GRAYED
		);
		pPopupMenu->EnableMenuItem(
			ID_SUB,   MF_BYCOMMAND | MF_DISABLED | MF_GRAYED
		);
		pPopupMenu->EnableMenuItem(
			ID_OFFER, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED
		);
		pPopupMenu->EnableMenuItem(
			ID_OTHER, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED
		);
		pPopupMenu->EnableMenuItem(
			ID_SH_OPEN,  MF_BYCOMMAND | MF_DISABLED | MF_GRAYED
		);
		pPopupMenu->EnableMenuItem(
			ID_SH_CLOSE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED
		);

		if (!m_pTabFile)
		{
			KG_PROCESS_SUCCESS(true);
		}

		if (!m_hTreeSelItem)
		{
			pPopupMenu->EnableMenuItem(ID_MAP ,  MF_ENABLED);
			KG_PROCESS_SUCCESS(true);
		}

		nTreeItemData = (int)m_treeObjectView.GetItemData(m_hTreeSelItem);

		if (nTreeItemData == 0)
		{
			pPopupMenu->EnableMenuItem(ID_LINK,  MF_ENABLED);
			pPopupMenu->EnableMenuItem(ID_MAP ,  MF_ENABLED);
		}
		else if (nTreeItemData == -1)
		{
			pPopupMenu->EnableMenuItem(ID_LINK,  MF_ENABLED);
			pPopupMenu->EnableMenuItem(ID_GLEAN, MF_ENABLED);
			pPopupMenu->EnableMenuItem(ID_KILL,  MF_ENABLED);
			pPopupMenu->EnableMenuItem(ID_POS,   MF_ENABLED);
			pPopupMenu->EnableMenuItem(ID_TALK,  MF_ENABLED);
			pPopupMenu->EnableMenuItem(ID_OFFER, MF_ENABLED);
			pPopupMenu->EnableMenuItem(ID_OTHER, MF_ENABLED);
			pPopupMenu->EnableMenuItem(ID_SUB,   MF_ENABLED);
		}
		else if (nTreeItemData == SUBSET_TREE_DATA)
		{
			pPopupMenu->EnableMenuItem(ID_SUB, MF_ENABLED);
			HTREEITEM hTempItem = m_treeObjectView.GetChildItem(m_hTreeSelItem);
			if (hTempItem)
			{
				int nSibCount = m_treeObjectView.GetSibingCount(hTempItem);
				if (nSibCount < RELATING_DATA_COUNT)
				{
					pPopupMenu->EnableMenuItem(ID_GLEAN, MF_ENABLED);
					pPopupMenu->EnableMenuItem(ID_KILL,  MF_ENABLED);
					pPopupMenu->EnableMenuItem(ID_POS,   MF_ENABLED);
					pPopupMenu->EnableMenuItem(ID_TALK,  MF_ENABLED);
					pPopupMenu->EnableMenuItem(ID_OFFER, MF_ENABLED);
					pPopupMenu->EnableMenuItem(ID_OTHER, MF_ENABLED);
				}
				else
				{
				}
			}
			else
			{
				pPopupMenu->EnableMenuItem(ID_GLEAN, MF_ENABLED);
				pPopupMenu->EnableMenuItem(ID_KILL,  MF_ENABLED);
				pPopupMenu->EnableMenuItem(ID_POS,   MF_ENABLED);
				pPopupMenu->EnableMenuItem(ID_TALK,  MF_ENABLED);
				pPopupMenu->EnableMenuItem(ID_OFFER, MF_ENABLED);
				pPopupMenu->EnableMenuItem(ID_OTHER, MF_ENABLED);
			}

		}
		else if (nTreeItemData >= 1)
		{
			HTREEITEM hParentItem = m_treeObjectView.GetParentItem(
				m_hTreeSelItem
			);
			int nItemData = (int)m_treeObjectView.GetItemData(hParentItem);
			if (nItemData == SUBSET_TREE_DATA)
			{
				int nSibCount = m_treeObjectView.GetSibingCount(m_hTreeSelItem);
				if (nSibCount < 4)
				{
					pPopupMenu->EnableMenuItem(ID_GLEAN, MF_ENABLED);
					pPopupMenu->EnableMenuItem(ID_KILL,  MF_ENABLED);
					pPopupMenu->EnableMenuItem(ID_POS,   MF_ENABLED);
					pPopupMenu->EnableMenuItem(ID_TALK,  MF_ENABLED);
					pPopupMenu->EnableMenuItem(ID_OFFER, MF_ENABLED);
					pPopupMenu->EnableMenuItem(ID_OTHER, MF_ENABLED);
				}
				else
				{
				}
			}
			else
			{
				pPopupMenu->EnableMenuItem(ID_SUB, MF_ENABLED);
				pPopupMenu->EnableMenuItem(ID_GLEAN, MF_ENABLED);
				pPopupMenu->EnableMenuItem(ID_KILL,  MF_ENABLED);
				pPopupMenu->EnableMenuItem(ID_POS,   MF_ENABLED);
				pPopupMenu->EnableMenuItem(ID_TALK,  MF_ENABLED);
				pPopupMenu->EnableMenuItem(ID_OFFER, MF_ENABLED);
				pPopupMenu->EnableMenuItem(ID_OTHER, MF_ENABLED);
			}

			pPopupMenu->EnableMenuItem(ID_SH_CLOSE, MF_ENABLED);
			pPopupMenu->EnableMenuItem(ID_SH_OPEN,  MF_ENABLED);
		}
		else
		{
		}

		nRetCode = m_pTabFile->GetInteger(
			m_nCurRow, _T("IsAvailable"), 0, &nQuestSwitch
		);

		if (nQuestSwitch)
		{
			pPopupMenu->CheckMenuItem(ID_SH_CLOSE, MF_UNCHECKED);
			pPopupMenu->CheckMenuItem(ID_SH_OPEN,  MF_CHECKED);
		}
		else
		{
			pPopupMenu->CheckMenuItem(ID_SH_CLOSE, MF_CHECKED);
			pPopupMenu->CheckMenuItem(ID_SH_OPEN,  MF_UNCHECKED);
		}

	}

Exit1:
	nReuslt = true;
Exit0:
	return;
}

int KGObjectPropertyEditTabDlg::BackUp()
{
	int nResult  = false;
	int nRetCode = false;

	static int nYear = 0;
	static int nMon  = 0;
	static int nDay  = 0;
	static int nHour = 0;
	static int nMin  = 0;

	char szBackPath[MAX_PATH];
	char szTempPath[MAX_PATH];
	char szBuffer[MAX_PATH];

	*szBackPath   = '\0';
	*szTempPath	  = '\0';
	*szBuffer	  = '\0';

	time_t tmtNow = 0;
	tm     tmNow;
	int	   nPos   = 0;

	CString szTemp1 = _T("");
	CString szTemp2 = _T("");

	KG_PROCESS_ERROR(m_pTabFile);
	g_GetFullPath(szTempPath, BACK_UP_PATH);
	tmtNow = time(NULL);

	localtime_r(&tmtNow, &tmNow);

	if (
		(nYear == tmNow.tm_year) &&
		(nMon  == tmNow.tm_mon)  &&
		(nDay  == tmNow.tm_mday) &&
		(nHour == tmNow.tm_hour) &&
		(nMin  == tmNow.tm_min)
	)
	{
		KG_PROCESS_SUCCESS(true);
	}

	snprintf(
		szBuffer, sizeof(szBuffer) - 1,
		"%d-%2.2d-%2.2d-%2.2d-%2.2d-%2.2d",
		tmNow.tm_year + 1900,
		tmNow.tm_mon + 1,
		tmNow.tm_mday,
		tmNow.tm_hour,
		tmNow.tm_min,
		tmNow.tm_sec
	);
	szBuffer[sizeof(szBuffer) - 1] = '\0';

	nYear = tmNow.tm_year;
	nMon  = tmNow.tm_mon;
	nDay  = tmNow.tm_mday;
	nHour = tmNow.tm_hour;
	nMin  = tmNow.tm_min;

	while (nPos >= 0)
	{
		szTemp2 = szTemp1;
		szTemp1 = m_szPropertyFileName.Tokenize(_T("\\"), nPos);
	}
	if (szTemp2.Right(4) == _T(".tab"))
	{
		szTemp2.Delete(szTemp2.GetLength() - 4, 4);
	}

	sprintf_s(szBackPath, "%s\\%s(%s).tab", szTempPath, szTemp2, szBuffer);

	if (!PathFileExists(szTempPath))
	{
		::CreateDirectory(szTempPath, NULL);
	}

	m_pTabFile->Save(szBackPath);

Exit1:
Exit0:
	return true;
}

LRESULT KGObjectPropertyEditTabDlg::OnBackUp(WPARAM wParam, LPARAM lParam)
{
	return BackUp();
}
//
//	Check Current  Quest File Validate
//	Whent Save The Quest File
//

int KGObjectPropertyEditTabDlg::CheckValidate()
{
	int nResult  = false;
	int nRetCode = false;

	int nQuestFileColsCount = 0;
	int nQuestFileRowsCount = 0;
	int nCurCols = 0;
	int nCurRow  = 0;

	KG_PROCESS_ERROR(m_pQuestIniFile);
	KG_PROCESS_ERROR(m_pTabFile);
	KG_PROCESS_ERROR(m_pIniFile);

	nQuestFileColsCount = m_pTabFile->GetWidth();
	nQuestFileRowsCount = m_pTabFile->GetHeight();

	m_proessBar.SetRange32(1, nQuestFileColsCount);

	for (nCurCols = 1; nCurCols <= nQuestFileColsCount; nCurCols++)
	{
		char szPropertyValue[256];
		char szPropertyPlate[256];
		char szPropertyName[256];

		*szPropertyValue = '\0';
		*szPropertyPlate = '\0';
		*szPropertyName  = '\0';

		int  nNeedFlag   = false;

		m_proessBar.SetPos(nCurCols);

		nRetCode = m_pTabFile->GetString(
			1, nCurCols, _T(""), szPropertyName, sizeof(szPropertyName)
		);
		KG_PROCESS_ERROR(nRetCode);
		m_pIniFile->GetInteger(szPropertyName, _T("Need"), 0, &nNeedFlag);
		m_pIniFile->GetString(
			szPropertyName, _T("Name"), _T(""),
			szPropertyPlate, sizeof(szPropertyPlate)
		);

		if (!nNeedFlag)
			continue;

		for (nCurRow = 3; nCurRow <= nQuestFileRowsCount; nCurRow++)
		{
			HTREEITEM hTreeItem = NULL;
			//LVFINDINFO info;
			CString szQuestPath = "";
			CRect rect;
			SIZE  size;
			int nListItemPos = 0;
			int nQuestId = 0;

			m_pTabFile->GetString(
				nCurRow, nCurCols,_T(""), szPropertyValue, sizeof(szPropertyValue)
			);
			nRetCode = m_pTabFile->GetInteger(nCurRow, _T("QuestID"), 0, &nQuestId);
			KG_PROCESS_ERROR(nRetCode);

			if (*szPropertyValue != '\0')
				continue;

			hTreeItem = m_treeObjectView.GetItemByPrimaryKey(
				m_treeObjectView.GetRootItem(), nQuestId
			);
			m_treeObjectView.SelectItem(hTreeItem);

			m_listProperty.GetItemRect(nListItemPos, &rect, true);

			m_listProperty.FindItemByText(szPropertyPlate);
			nListItemPos  = m_listProperty.FindItemPos(
				m_listProperty.FindNextItem()
			);
			size.cy =
				(nListItemPos - m_listProperty.GetTopIndex()) * rect.Height();

			m_listProperty.Scroll(size);

			if (m_listProperty.GetTopIndex() == nListItemPos)
			{
				size.cy = -3 * rect.Height();
				m_listProperty.Scroll(size);
			}

			/*
			info.flags   = LVFI_PARTIAL | LVFI_STRING;
			info.psz     = szPropertyPlate;
			nListItemPos = m_listProperty.FindItem(&info);

			KG_PROCESS_ERROR(nListItemPos > -1);
			*/

			m_listProperty.SetItemState(
				nListItemPos,
				LVIS_SELECTED | LVIS_FOCUSED,
				LVIS_SELECTED | LVIS_FOCUSED
			);

			szQuestPath = m_szCurBookMarkName + _T("-") + szPropertyPlate + _T(" 为必填项!!");
			::MessageBox(
				m_hWnd, szQuestPath.GetBuffer(),
				_T("Error!!"), MB_OK | MB_ICONERROR
			);

			KG_PROCESS_ERROR(false);
		}
	}


	nResult = true;
Exit0:
	return nResult;
}

void KGObjectPropertyEditTabDlg::OnSaveAs()
{
	CString szFileSave = m_szPropertyFileName;
	m_szPropertyFileName = "";
	OnSave();
	if (m_szPropertyFileName == "")
	{
		m_szPropertyFileName = szFileSave;
	}
	else
	{
		m_nBeSavedFlag = true;
	}
}

void KGObjectPropertyEditTabDlg::OnSave()
{
	int nResult  = false;
	int nRetCode = false;

	KG_PROCESS_ERROR(m_pQuestIniFile);
	KG_PROCESS_ERROR(m_pTabFile);
	KG_PROCESS_ERROR(m_pIniFile);

	nRetCode = CheckValidate();
	KG_PROCESS_ERROR(nRetCode);
	m_proessBar.SetPos(1);

	if (m_szPropertyFileName == "")
	{
		CFileDialog dlg(
			false, "tab", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
			"Quest File|*.tab|All File|*.*|", this
		);
		if (dlg.DoModal() == IDOK)
		{
			m_szPropertyFileName = dlg.GetPathName();
			if (m_szPropertyFileName.Find(".tab") == -1)
			{
				m_szPropertyFileName += ".tab";
			}
		}
	}
	if (m_szPropertyFileName != "")
	{
		char szSectionName[256];
		char szKeyName[256];
		*szSectionName = '\0';
		*szKeyName     = '\0';

		UpdateProperty();

		m_pTabFile->Save(m_szPropertyFileName);
		m_pTabFile->Release();
		m_pTabFile = g_OpenTabFile(
			m_szPropertyFileName.GetBuffer(), false, true
		);

		CString szQuestIniFileName = m_szPropertyFileName;
		szQuestIniFileName.Replace(".tab", ".ini");

		while (m_pQuestIniFile->GetNextSection(szSectionName, szSectionName))
		{
			*szKeyName = '\0';
			while (
				m_pQuestIniFile->GetNextKey(szSectionName, szKeyName, szKeyName)
			)
			{
				if (CString(szKeyName) == _T("TreeHandle"))
				{
					m_pQuestIniFile->EraseKey(szSectionName, szKeyName);
					*szSectionName = '\0';
				}

			}
		}

		m_pQuestIniFile->Save(szQuestIniFileName);
		m_pQuestIniFile->Release();
		m_pQuestIniFile = g_OpenIniFile(szQuestIniFileName);

		SetWindowText("任务编辑_" + m_szPropertyFileName);
	}

	m_nBeSavedFlag = true;
Exit0:
	return;
}

void KGObjectPropertyEditTabDlg::OnQuestExportFileAll()
{
	int nResult  = false;
	int nRetCode = false;

	IIniFile* pPopIniFile = NULL;

	CString szExistingFileName = _T("");
	CString szPopQuestFile     = _T("");

	char szSectionName[SECTION_NAME_SIZE];
	char szKeyName[KEY_NAME_SIZE];

	*szSectionName = '\0';
	*szKeyName     = '\0';

	char szFullPath[MAX_PATH];
	*szFullPath = '\0';

	CFileDialog dlg(
		false, "tab", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"Quest File|*.tab|All File|*.*|", this
	);
	KG_PROCESS_SUCCESS(dlg.DoModal() != IDOK);

	g_GetFullPath(szFullPath, TEMP_QUEST_PATH);

	szExistingFileName = szFullPath;
	KG_PROCESS_ERROR(szExistingFileName != _T(""));

	m_proessBar.SetRange(1, 4);
	m_proessBar.SetPos(1);

	szPopQuestFile = dlg.GetPathName();
	KG_PROCESS_ERROR(szPopQuestFile != _T(""));

	nRetCode = szPopQuestFile.Find(_T(".tab"));
	if (nRetCode == -1)
	{
		szPopQuestFile += _T(".tab");
	}

	m_pTabFile->Save(szExistingFileName.GetBuffer());

	nRetCode = ::CopyFile(
		szExistingFileName.GetBuffer(), szPopQuestFile.GetBuffer(),	FALSE
	);
	KG_PROCESS_ERROR(nRetCode);
	m_proessBar.SetPos(2);

	::DeleteFile(szExistingFileName.GetBuffer());

	szExistingFileName.Replace(_T(".tab"), _T(".ini"));
	szPopQuestFile.Replace(_T(".tab"), _T(".ini"));
	m_proessBar.SetPos(3);

	m_pQuestIniFile->Save(szExistingFileName.GetBuffer());

	nRetCode = ::CopyFile(
		szExistingFileName.GetBuffer(), szPopQuestFile.GetBuffer(), FALSE
	);
	KG_PROCESS_ERROR(nRetCode);

	pPopIniFile = g_OpenIniFile(szPopQuestFile.GetBuffer());
	KG_PROCESS_ERROR(pPopIniFile);
	while (pPopIniFile->GetNextSection(szSectionName, szSectionName))
	{
		*szKeyName = '\0';
		while (pPopIniFile->GetNextKey(szSectionName, szKeyName, szKeyName))
		{
			nRetCode = strcmp(szKeyName, _T("TreeHandle"));
			if (!nRetCode)
				pPopIniFile->EraseKey(szSectionName, szKeyName);
		}
	}
	pPopIniFile->Save(szPopQuestFile.GetBuffer());

	m_proessBar.SetPos(4);

	::DeleteFile(szExistingFileName.GetBuffer());

Exit1:
	m_proessBar.SetPos(1);
	nResult = true;
Exit0:
	SAFE_RELEASE(pPopIniFile);
	return;
}

void KGObjectPropertyEditTabDlg::OnQuestExportFile()
{
	int nResult  = false;
	int nRetCode = false;

	CString szExportQuestFileName = _T("");
	CString szItemPath	     = _T("");

	HTREEITEM hSelTreeItem   = NULL;
	HTREEITEM hTempTreeItem  = NULL;

	ITabFile* pExportTabFile = NULL;
	IIniFile* pExportIniFile = NULL;

	int	nTreeItemData   = 0;
	int	nFirstRow       = 0;
	int nLastRow		= 0;
	int nCurRow			= 0;
	int nCurCol		    = 0;
	int	nCurExportRow   = 0;
	int	nColsNumber	    = 0;

	char szTempValue[1024];
	*szTempValue = '\0';

	CFileDialog dlg(
		false, "tab", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"Quest File|*.tab|All File|*.*|", this
	);

	pExportTabFile = g_CreateTabFile();
	pExportIniFile = g_CreateIniFile();

	KG_PROCESS_ERROR(pExportTabFile);
	KG_PROCESS_ERROR(pExportIniFile);

	KG_PROCESS_ERROR(m_pTabFile);

	nColsNumber = m_pTabFile->GetWidth();

	KG_PROCESS_SUCCESS(dlg.DoModal() != IDOK);

	//ShowCursor(FALSE);
	BusyCursor(this);

	szExportQuestFileName = dlg.GetPathName();

	nRetCode = szExportQuestFileName.Find(_T(".tab"));

	if (nRetCode == -1)
	{
		szExportQuestFileName += _T(".tab");
	}

	hSelTreeItem = m_treeObjectView.GetSelectedItem();
	KG_PROCESS_ERROR(hSelTreeItem);

	for (nCurRow = 1; nCurRow <= 2; nCurRow++)
	{
		for (nCurCol = 1; nCurCol <= nColsNumber; nCurCol++)
		{
			nRetCode = m_pTabFile->GetString(
				nCurRow, nCurCol, _T(""), szTempValue, sizeof(szTempValue)
			);
			nRetCode = pExportTabFile->WriteString(
				nCurRow, nCurCol, szTempValue
			);
			KG_PROCESS_ERROR(nRetCode);
		}
	}

	nTreeItemData = (int)m_treeObjectView.GetItemData(hSelTreeItem);
	if (nTreeItemData < 1)
	{
		nFirstRow = m_treeObjectView.GetCollectivityFirstPos(hSelTreeItem);
		nLastRow  = m_treeObjectView.GetCollectivityLastPos(hSelTreeItem);
	}
	else if (nTreeItemData == SUBSET_TREE_DATA)
	{
		HTREEITEM hChildItem = m_treeObjectView.GetChildItem(hSelTreeItem);
		if (hChildItem)
		{
			nFirstRow = m_treeObjectView.GetItemPos(hChildItem);
		}
		else
		{
			nFirstRow = -1;
		}

		while (m_treeObjectView.GetNextSiblingItem(hChildItem))
		{
			hChildItem = m_treeObjectView.GetNextSiblingItem(hChildItem);
		}

		if (hChildItem)
		{
			nLastRow = m_treeObjectView.GetItemPos(hChildItem);
		}
		else
		{
			nLastRow = -1;
		}
	}
	else if (nTreeItemData >= 1)
	{
		nFirstRow = m_treeObjectView.GetItemPos(hSelTreeItem);
		nLastRow  = nFirstRow;
	}
	else
	{
		KG_PROCESS_ERROR(false);
	}

	m_proessBar.SetRange32(1, (nLastRow - nFirstRow) * nColsNumber + 1);
	m_proessBar.SetPos(1);

	nCurExportRow = 3;
	for (nCurRow = nFirstRow; nCurRow <= nLastRow; nCurRow++)
	{
		for (nCurCol = 1; nCurCol <= nColsNumber; nCurCol++)
		{
			nRetCode = m_pTabFile->GetString(
				nCurRow, nCurCol, _T(""), szTempValue, sizeof(szTempValue)
			);
			nRetCode = pExportTabFile->WriteString(nCurExportRow, nCurCol, szTempValue);
			m_proessBar.SetPos((nCurRow - nFirstRow) * nColsNumber + nCurCol);
		}
		nCurExportRow++;
	}

	hTempTreeItem = hSelTreeItem;
	while (hTempTreeItem)
	{
		szItemPath     = m_treeObjectView.GetItemText(hTempTreeItem) + szItemPath;
		hTempTreeItem  = m_treeObjectView.GetParentItem(hTempTreeItem);
		if (hTempTreeItem)
		{
			szItemPath = _T("-") + szItemPath;
		}
	}
	m_pQuestIniFile->GetString(
		szItemPath.GetBuffer(), _T("BookMark"), _T(""),
		szTempValue, sizeof(szTempValue)
	);
	nRetCode = pExportIniFile->WriteString(
		szItemPath.GetBuffer(), _T("BookMark"), szTempValue
	);
	KG_PROCESS_ERROR(nRetCode);

	CopyIniFileSubset(
		m_treeObjectView.GetChildItem(hSelTreeItem), pExportIniFile
	);

	pExportTabFile->Save(szExportQuestFileName);
	szExportQuestFileName.Replace(_T(".tab"), _T(".ini"));
	pExportIniFile->Save(szExportQuestFileName);

	m_proessBar.SetPos(1);

Exit1:
	nResult = true;
Exit0:
	//ShowCursor(TRUE);
	FreeCursor(this);

	SAFE_RELEASE(pExportTabFile);
	SAFE_RELEASE(pExportIniFile);
	return;
}

int FindUsableIdInTwoFile(ITabFile* pFirshFile, ITabFile* pSecondlyFile)
{
	int nId = 0;
	for (nId = 1; nId <= 40000; nId++)
	{
		int i = 0;
		for (i = 3; i <= pFirshFile->GetHeight(); i++)
		{
			int nFirstId = 0;
			pFirshFile->GetInteger(i, _T("QuestID"), 0, &nFirstId);
			if (nFirstId == nId)
				break;
		}

		if (i <= pFirshFile->GetHeight())
			continue;

		int j = 0;
		for (j = 3; j <= pSecondlyFile->GetHeight(); j++)
		{
			int nSecondlyId = 0;
			pSecondlyFile->GetInteger(j, _T("QuestID"), 0, &nSecondlyId);
			if (nSecondlyId == nId)
				break;
		}

		if (j > pSecondlyFile->GetHeight())
			break;
	}

	if (nId > 40000)
		nId = -1;

	return nId;
}

LRESULT KGObjectPropertyEditTabDlg::OnUpdateTreeIcon(WPARAM wParam, LPARAM lParam)
{
	m_treeObjectView.SetItemImage(
		m_treeObjectView.GetSelectedItem(),GetImageIndex(m_pTabFile, m_nCurRow),
		GetImageIndex(m_pTabFile, m_nCurRow)
	);
	return 1;
}

// :)
void KGObjectPropertyEditTabDlg::OnQuestInportFileEx()
{
	int nResult  = false;
	int nRetCode = false;

	KGPrimaryKeyManager primaryKeyManager;

	ITabFile* pInsertTabFile  = NULL;
	IIniFile* pInsertIniFile  = NULL;

	ITabFile* ppTabFile[2];

	int nInsertRowsCount	  = 0;
	int nInsertColsCount	  = 0;
	int nCurInsertRow		  = 0;
	int nCurInsertCol		  = 0;

	int nExistRowCount        = 0;
	int nExistColCount	      = 0;

	int nMinId				  = 0;
	int nMaxId				  = 0;

	CString szInsertTabFile   = _T("");
	CString szInsertIniFile   = _T("");

	CFileDialog dlg(
		true, "tab", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"Quest File|*.tab|All File|*.*|", this
	);

	KGLOG_PROCESS_ERROR(m_pQuestIniFile);
	KGLOG_PROCESS_ERROR(m_pTabFile);
	KGLOG_PROCESS_ERROR(m_pIniFile);

	nRetCode  = (int)dlg.DoModal();
	KG_PROCESS_SUCCESS(nRetCode != IDOK);

	szInsertTabFile = dlg.GetPathName();
	szInsertIniFile = dlg.GetPathName();

	nRetCode = szInsertTabFile.Find(_T(".tab"));
	if (nRetCode == -1)
	{
		MessageBox(
			_T("不能导入非任务(.Tab)文件"), _T("导入出错!!"), MB_OK | MB_ICONERROR
		);
		KGLOG_PROCESS_ERROR(false);
	}
	szInsertIniFile.Replace(_T(".tab"), _T(".ini"));

	if (szInsertTabFile == m_szPropertyFileName)
	{
		MessageBox(
			_T("不能导入正在编辑的文件!!"), _T("导入出错!!"), MB_OK | MB_ICONERROR
		);
		KGLOG_PROCESS_ERROR(false);
	}

	pInsertTabFile = g_OpenTabFile(szInsertTabFile, false, true);
	KGLOG_PROCESS_ERROR(pInsertTabFile);

	pInsertIniFile = g_OpenIniFile(szInsertIniFile);
	KGLOG_PROCESS_ERROR(pInsertIniFile);

	nInsertRowsCount = pInsertTabFile->GetHeight();
	nInsertColsCount = pInsertTabFile->GetWidth();

	nExistRowCount   = m_pTabFile->GetHeight();
	nExistColCount   = m_pTabFile->GetWidth();

	if (nExistColCount != nInsertColsCount)
	{
		nRetCode = MessageBox(
			_T("导入文件与现有文件版本不一致, 是否继续!!"), _T("导入出错!!"),
			MB_YESNO | MB_ICONERROR
		);
		KGLOG_PROCESS_ERROR(nRetCode == IDYES);
	}

	m_pIniFile->GetInteger(_T("QuestID"), _T("Min"), 0, &nMinId);
	m_pIniFile->GetInteger(_T("QuestID"), _T("Max"), 0, &nMaxId);

	if (nMinId < 0 || nMaxId < 0 || nMinId >= nMaxId || nMaxId > INT_MAX)
	{
		MessageBox(
			_T("ID管理器初始化出错,请检查培植文件!!"), _T("导入出错!!"),
			MB_OK | MB_ICONERROR
		);
		KGLOG_PROCESS_ERROR(false);
	}

	m_wndStateBar.SetPaneText(0, _T("正在构造任务ID管理器....."));
	primaryKeyManager.Refrush(nMinId, nMaxId, &m_proessBar, &m_wndStateBar);

	ppTabFile[0] = m_pTabFile;
	ppTabFile[1] = pInsertTabFile;

	m_wndStateBar.SetPaneText(0, _T("正在初此化任务ID管理器..."));
	g_IniPrimaryKesManager(
		&primaryKeyManager, ppTabFile, 2, _T("QuestID"), &m_proessBar
	);



	nResult = true;
Exit1:
Exit0:
	return;
}


// This Function Need Optimize!!
void KGObjectPropertyEditTabDlg::OnQuestInportFile()
{
	int nResult  = false;
	int nRetCode = false;

	KGPrimaryKeyManager primaryKeyManager;

	ITabFile** ppTabFile = new ITabFile*[2];

	CString szInsertQuestFile = _T("");
	ITabFile* pInsertTabFile  = NULL;
	IIniFile* pInsertIniFile  = NULL;

	RETURN_VALUE eReturnValue    = E_CANCEL;
	RETURN_VALUE eReturnValueOld = E_CANCEL;

	int nRowCount        = 0;	// Rows Count  Of The Existing Quest File
	int nCurRow			 = 0;	// The Current Of The Existing Quest File
	int nInsertRowsCount = 0;	// Rows Count  Of The Insert Quest File
	int nCurInsertRow	 = 0;	// The Current Of The Insert Quest File
	int nInsertColsCount = 0;	// Cols Count  Of The Insert Quest File
	int nColsCount       = 0;	// Cols Count  Of The Existing Quest File
	int nIndex           = 0;
	int nMinId           = 0;
	int nMaxId			 = 0;

	char szSectionName[SECTION_NAME_SIZE];
	char szKeyName[KEY_NAME_SIZE];
	char szKeyValue[1024];
	char szQuestName[64];

	*szSectionName = '\0';
	*szKeyValue    = '\0';
	*szKeyName     = '\0';
	*szQuestName   = '\0';

	CString szStateBarText = _T("");

	for (nIndex = 0; nIndex < 2; nIndex++)
	{
		ppTabFile[nIndex] = NULL;
	}

	CFileDialog dlg(
		true, "tab", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"Quest File|*.tab|All File|*.*|", this
	);

	KGLOG_PROCESS_ERROR(m_pQuestIniFile);
	KGLOG_PROCESS_ERROR(m_pTabFile);
	KGLOG_PROCESS_ERROR(m_pIniFile);

	nRowCount = m_pTabFile->GetHeight();

	nRetCode  = (int)dlg.DoModal();
	KG_PROCESS_SUCCESS(nRetCode != IDOK);

	//ShowCursor(FALSE);
	/*BusyCursor(this);*/

	szInsertQuestFile = dlg.GetPathName();

	nRetCode = szInsertQuestFile.Find(_T(".tab"));
	if (nRetCode == -1)
	{
		::MessageBox(
			this->m_hWnd, _T("不能导入非任务(.Tab)文件"), _T("导入出错!!"),
			MB_OK | MB_ICONERROR
		);
		KGLOG_PROCESS_ERROR(false);
	}

	if (szInsertQuestFile == m_szPropertyFileName)
	{
		::MessageBox(
			this->m_hWnd, _T("不能导入正在编辑的文件!!"), _T("导入出错!!"),
			MB_OK | MB_ICONERROR
		);
		KGLOG_PROCESS_ERROR(false);
	}

	pInsertTabFile = g_OpenTabFile(szInsertQuestFile, false, true);
	KGLOG_PROCESS_ERROR(pInsertTabFile);

	nInsertRowsCount = pInsertTabFile->GetHeight();
	KG_PROCESS_SUCCESS(nInsertRowsCount <= 2);

	nInsertColsCount = pInsertTabFile->GetWidth();
	nColsCount = m_pTabFile->GetWidth();

	if (nInsertColsCount != nColsCount)
	{
		nRetCode = ::MessageBox(
			this->m_hWnd, _T("导入文件与现有文件版本不一致, 是否继续!!"), _T("导入出错!!"),
			MB_YESNO| MB_ICONERROR
		);
		KGLOG_PROCESS_ERROR(nRetCode == IDYES);
	}

	szInsertQuestFile.Replace(_T(".tab"), _T(".ini"));
	pInsertIniFile = g_OpenIniFile(szInsertQuestFile);
	szInsertQuestFile.Replace(_T(".ini"), _T(".tab"));
	KGLOG_PROCESS_ERROR(pInsertIniFile);

	m_pIniFile->GetInteger(_T("QuestID"), _T("Min"), 0, &nMinId);
	m_pIniFile->GetInteger(_T("QuestID"), _T("Max"), 0, &nMaxId);

	if (nMinId < 0 || nMaxId < 0 || nMinId >= nMaxId || nMaxId > INT_MAX)
	{
		MessageBox(
			_T("ID管理器初始化出错,请检查培植文件!!"), _T("导入出错!!"),
			MB_OK | MB_ICONERROR
			);
		KGLOG_PROCESS_ERROR(false);
	}

	m_wndStateBar.SetPaneText(0, _T("正在构造任务ID管理器....."));
	primaryKeyManager.Refrush(nMinId, nMaxId, &m_proessBar, &m_wndStateBar);


	ppTabFile[0] = m_pTabFile;
	ppTabFile[1] = pInsertTabFile;

	m_wndStateBar.SetPaneText(0, _T("正在初此化任务ID管理器..."));
	g_IniPrimaryKesManager(
		&primaryKeyManager, ppTabFile, 2, _T("QuestID"), &m_proessBar
	);

	m_proessBar.SetRange32(1, nInsertRowsCount);
	m_proessBar.SetPos(1);

	for (nCurInsertRow = 3; nCurInsertRow <= nInsertRowsCount; nCurInsertRow++ )
	{
		CString pszNameplate[2];

		CString szPrimaryKey        = _T("");
		CString szSubsetSectionName = _T("");

		char szNameplate[256];
		*szNameplate = '\0';

		int nInSubsetFlag    = 0;
		int nInsertNameplate = 0;
		int nInsertRow       = 0;
		int nCurCols         = 0;

		pInsertTabFile->GetString(
			nCurInsertRow, _T("MapName"), _T(""), szNameplate, sizeof(szNameplate)
		);
		pszNameplate[0] = szNameplate;

		pInsertTabFile->GetString(
			nCurInsertRow, _T("TaskLinkName"), _T(""),
			szNameplate, sizeof(szNameplate)
		);
		pszNameplate[1] = szNameplate;

		pInsertTabFile->GetInteger(
			nCurInsertRow, _T("QuestID"), 0, &nInsertNameplate
		);



		/*nRowCount = m_pTabFile->GetHeight();*/
		/*for (nCurRow = 3; nCurRow <= nRowCount; nCurRow++)
		{
			int nNameplate = 0;
			m_pTabFile->GetInteger(nCurRow, _T("QuestID"), 0, &nNameplate);
			if (nNameplate == nInsertNameplate)
			{
				break;
			}
		}*/

		//
		char szQuestId[8];
		int	 nFinded = 0;
		itoa(nInsertNameplate, szQuestId, 10);
		nFinded = m_pTabFile->FindRow(szQuestId);

		//


		/*if (nCurRow > nRowCount || nRowCount == 2)*/
		if (nFinded == -1 /*|| nRowCount == 2*/)
		{
			nInsertRow = m_treeObjectView.FindInsertPosInFile(pszNameplate, 2);
			m_pTabFile->InsertAfter(nInsertRow++);
			goto INSERT;
		}

		eReturnValueOld = eReturnValue;

		szPrimaryKey.Format(_T("%d"), nInsertNameplate);
		FindInSubset(pInsertIniFile,szPrimaryKey, szSubsetSectionName);
		if (szSubsetSectionName != _T(""))
		{
			eReturnValue = E_AUTOTUNE;
		}

		if (
			(
				(eReturnValue == E_AUTOTUNE)  ||
				(eReturnValue == E_OVERLAY)   ||
				(eReturnValue == E_CANCEL)
			)
			&& szSubsetSectionName == _T("")
		)
		{
			KGInsertQuestAlertDlg insertAlertDlg(
				m_pTabFile, m_szPropertyFileName, nCurRow,pInsertTabFile,
				szInsertQuestFile, nCurInsertRow, m_pIniFile, this
			);
			insertAlertDlg.DoModal();
			eReturnValue = insertAlertDlg.GetReturnValue();
		}

		if (
			(eReturnValue == E_AUTOTUNE_ALL) ||
			(eReturnValue == E_AUTOTUNE)
		)
		{
			CString szNewSubSectionName = szSubsetSectionName;
			CString szNewPrimaryKey;

			int nOldNameplate = nInsertNameplate;

			//
			//nInsertNameplate  = m_treeObjectView.FindId(_T("QuestID"));
			//
			nRetCode = primaryKeyManager.GetKey(&nInsertNameplate);
			KGLOG_PROCESS_ERROR(nRetCode);

			//nInsertNameplate = FindUsableIdInTwoFile(m_pTabFile, pInsertTabFile);
			//KG_PROCESS_ERROR(nInsertNameplate != -1);

			nRetCode = pInsertTabFile->WriteInteger(
				nCurInsertRow, _T("QuestID"), nInsertNameplate
			);
			KGLOG_PROCESS_ERROR(nRetCode);

			szNewPrimaryKey.Format(_T("%d"), nInsertNameplate);

			nInsertRow = m_treeObjectView.FindInsertPosInFile(pszNameplate, 2);
			m_pTabFile->InsertAfter(nInsertRow++);


			////////////////////////////////////////////////////////////////////
			//
			if (szNewSubSectionName != _T(""))
			{
				char szTempValue[256];
				*szTempValue = '\0';

				int nPos = szNewSubSectionName.Find(
					_T("-") + szPrimaryKey  + _T("-")
				);

				if (nPos == -1)
				{
					nPos = szNewSubSectionName.Find(szPrimaryKey + _T("-"));
					if (nPos != 0)
						nPos = -1;
					if (nPos == -1)
					{
						nPos = szNewSubSectionName.Find(_T("-") + szPrimaryKey);
						if (nPos == -1)
						{
							nPos = szNewSubSectionName.Find(szPrimaryKey);
							KGLOG_PROCESS_ERROR(nPos == -1);
							szNewSubSectionName.Replace(
								szPrimaryKey,szNewPrimaryKey
							);
						}
						else
						{
							szNewSubSectionName.Replace(
								_T("-") + szPrimaryKey,
								_T("-") + szNewPrimaryKey
							);
						}
					}
					else
					{
						szNewSubSectionName.Replace(
							szPrimaryKey     + _T("-"),
							szNewPrimaryKey  + _T("-")
						);
					}
				}
				else
				{
					szNewSubSectionName.Replace(
						_T("-") + szPrimaryKey     + _T("-"),
						_T("-") + szNewPrimaryKey  + _T("-")
					);
				}

				pInsertIniFile->GetString(
					szSubsetSectionName, _T("Name"), _T(""),
					szTempValue, sizeof(szTempValue)
				);
				pInsertIniFile->WriteString(
					szNewSubSectionName, _T("Name"), szTempValue
				);
				pInsertIniFile->EraseSection(szSubsetSectionName);
			}

		}
		else if (
			(eReturnValue == E_OVERLAY_ALL) ||
			(eReturnValue == E_OVERLAY)
		)
		{
			nInsertRow = nCurRow;
		}
		else if (eReturnValue == E_CANCEL)
		{
			continue;
		}

INSERT :

		for (nCurCols = 1; nCurCols <= nColsCount; nCurCols++)
		{
			char szTempValue[1024];
			*szTempValue = '\0';
			pInsertTabFile->GetString(
				nCurInsertRow, nCurCols, _T(""), szTempValue, sizeof(szTempValue)
			);
			m_pTabFile->WriteString(nInsertRow, nCurCols, szTempValue);
		}

		m_proessBar.SetPos(nCurInsertRow);

		if (szSubsetSectionName != _T(""))
		{
			eReturnValue = eReturnValueOld;
		}

		m_primaryKeyManager.DelKey(nInsertNameplate);
		m_pTabFile->GetString(nInsertRow, _T("QuestName"), _T(""), szQuestName, sizeof(szQuestName));
		szStateBarText =
			_T("导入 : ") + pszNameplate[0] + _T("-") + pszNameplate[1] + _T("-") + CString(szQuestName);
		m_wndStateBar.SetPaneText(0, szStateBarText.GetBuffer());

	}

	*szSectionName = '\0';
	while (pInsertIniFile->GetNextSection(szSectionName, szSectionName))
	{
		*szKeyName = '\0';
		while (pInsertIniFile->GetNextKey(szSectionName, szKeyName, szKeyName))
		{
			pInsertIniFile->GetString(
				szSectionName, szKeyName, _T(""),
				szKeyValue, sizeof(szKeyValue)
			);
			m_pQuestIniFile->WriteString(
				szSectionName, szKeyName, szKeyValue
			);
		}
	}

	*szSectionName = '\0';
	while (m_pQuestIniFile->GetNextSection(szSectionName, szSectionName))
	{
		*szKeyName = '\0';
		while (
			m_pQuestIniFile->GetNextKey(szSectionName, szKeyName, szKeyName)
		)
		{
			if (CString(szKeyName) == _T("TreeHandle"))
			{
				m_pQuestIniFile->EraseKey(szSectionName, szKeyName);
				*szSectionName = '\0';
			}

		}
	}

	m_proessBar.SetPos(1);
	m_wndStateBar.SetPaneText(0, _T("就绪"));

	m_treeObjectView.Update();
	//m_treeObjectView.ExpendAll();
	m_nBeSavedFlag = false;

	// Temp.......
	//{
	//	HTREEITEM hSelTreeItem = m_treeObjectView.GetRootItem();

	//	while (hSelTreeItem)
	//	{
	//		operation_tree_item(
	//			&m_treeObjectView, hSelTreeItem, &KGObjectPropertyEditTabDlg::UpdateDependData
	//		);
	//		hSelTreeItem = m_treeObjectView.GetNextSiblingItem(hSelTreeItem);
	//	}
	//}

	SendMessage(WM_USER_BACKUP_TAB_FILE);

Exit1:
	nResult = true;
Exit0:
	if (!nResult)
	{
		::MessageBox(
			this->m_hWnd, _T("导入出错，请查看日志!!"), _T("导入出错"), MB_OK | MB_ICONERROR
		);
	}
	szInsertQuestFile.ReleaseBuffer();
	SAFE_DELETE_ARRAY(ppTabFile);
	SAFE_RELEASE(pInsertTabFile);
	SAFE_RELEASE(pInsertIniFile);

	//ShowCursor(TRUE);
	/*FreeCursor(this);*/

	return;
}

int KGObjectPropertyEditTabDlg::UpdateToolBarState()
{
	int nResult  = false;
	int nRetCode = false;

	CString szQuestNumber = _T("");
	int nItemData    = 0;
	int nQuestNumber = 0;

	HTREEITEM hSelTreeItem     = m_treeObjectView.GetSelectedItem();
	CToolBarCtrl* pToolBarCtrl = &m_wndToolBar.GetToolBarCtrl();

	KG_PROCESS_ERROR(pToolBarCtrl);

	pToolBarCtrl->EnableButton(ID_MAP,   FALSE);
	pToolBarCtrl->EnableButton(ID_LINK,  FALSE);
	pToolBarCtrl->EnableButton(ID_SUB,   FALSE);
	pToolBarCtrl->EnableButton(ID_GLEAN, FALSE);
	pToolBarCtrl->EnableButton(ID_POS,   FALSE);
	pToolBarCtrl->EnableButton(ID_KILL,  FALSE);
	pToolBarCtrl->EnableButton(ID_TALK,  FALSE);
	pToolBarCtrl->EnableButton(ID_OFFER, FALSE);
	pToolBarCtrl->EnableButton(ID_OTHER, FALSE);

	KG_PROCESS_SUCCESS(m_nEditerState == EDIT_STATE_CLOSE);

	if (!hSelTreeItem)
	{
		pToolBarCtrl->EnableButton(ID_MAP);
		KG_PROCESS_SUCCESS(true);
	}

	nItemData = (int)m_treeObjectView.GetItemData(hSelTreeItem);

	if (nItemData == 0)
	{
		pToolBarCtrl->EnableButton(ID_LINK);
		pToolBarCtrl->EnableButton(ID_MAP);
	}
	else if (nItemData == -1)
	{
		pToolBarCtrl->EnableButton(ID_LINK);
		pToolBarCtrl->EnableButton(ID_GLEAN);
		pToolBarCtrl->EnableButton(ID_KILL);
		pToolBarCtrl->EnableButton(ID_POS);
		pToolBarCtrl->EnableButton(ID_TALK);
		pToolBarCtrl->EnableButton(ID_OFFER);
		pToolBarCtrl->EnableButton(ID_OTHER);
		pToolBarCtrl->EnableButton(ID_SUB);
	}
	else if (nItemData == SUBSET_TREE_DATA)
	{
		pToolBarCtrl->EnableButton(ID_SUB);
		HTREEITEM hTempItem = m_treeObjectView.GetChildItem(hSelTreeItem);
		if (hTempItem)
		{
			int nSibCount = m_treeObjectView.GetSibingCount(hTempItem);
			if (nSibCount < RELATING_DATA_COUNT)
			{
				pToolBarCtrl->EnableButton(ID_GLEAN);
				pToolBarCtrl->EnableButton(ID_KILL);
				pToolBarCtrl->EnableButton(ID_POS);
				pToolBarCtrl->EnableButton(ID_TALK);
				pToolBarCtrl->EnableButton(ID_OFFER);
				pToolBarCtrl->EnableButton(ID_OTHER);
			}
			else
			{
			}
		}
		else
		{
			pToolBarCtrl->EnableButton(ID_GLEAN);
			pToolBarCtrl->EnableButton(ID_KILL);
			pToolBarCtrl->EnableButton(ID_POS);
			pToolBarCtrl->EnableButton(ID_TALK);
			pToolBarCtrl->EnableButton(ID_OFFER);
			pToolBarCtrl->EnableButton(ID_OTHER);
		}

	}
	else if (nItemData >= 1)
	{
		HTREEITEM hParentItem = m_treeObjectView.GetParentItem(hSelTreeItem);
		int nItemData = (int)m_treeObjectView.GetItemData(hParentItem);
		if (nItemData == SUBSET_TREE_DATA)
		{
			int nSibCount = m_treeObjectView.GetSibingCount(hSelTreeItem);
			if (nSibCount < RELATING_DATA_COUNT)
			{
				pToolBarCtrl->EnableButton(ID_GLEAN);
				pToolBarCtrl->EnableButton(ID_KILL);
				pToolBarCtrl->EnableButton(ID_POS);
				pToolBarCtrl->EnableButton(ID_TALK);
				pToolBarCtrl->EnableButton(ID_OFFER);
				pToolBarCtrl->EnableButton(ID_OTHER);
			}
			else
			{
			}
		}
		else
		{
			pToolBarCtrl->EnableButton(ID_SUB);
			pToolBarCtrl->EnableButton(ID_GLEAN);
			pToolBarCtrl->EnableButton(ID_KILL);
			pToolBarCtrl->EnableButton(ID_POS);
			pToolBarCtrl->EnableButton(ID_TALK);
			pToolBarCtrl->EnableButton(ID_OFFER);
			pToolBarCtrl->EnableButton(ID_OTHER);
		}
	}
	else
	{
	}

	if (m_pTabFile)
	{
		nQuestNumber = m_pTabFile->GetHeight() - 2;
		szQuestNumber.Format(_T("任务个数 : %d"), nQuestNumber);
		m_wndStateBar.SetPaneText(2, szQuestNumber.GetBuffer());
	}

Exit1:
	nResult = true;
Exit0:
	return nResult;

}

void KGObjectPropertyEditTabDlg::OnTimer(UINT_PTR nIDEvent)
{
	//if (nIDEvent == 1)
	//	UpdateStateBar();
	CDialog::OnTimer(nIDEvent);
}

int KGObjectPropertyEditTabDlg::UpdateDependData(CTreeCtrl* pTree, HTREEITEM hTreeItem)
{
	int nResult  = false;
	int nRetCode = false;

	KG_PROCESS_ERROR(pTree);
	KG_PROCESS_ERROR(hTreeItem);

	((KGPropertyTreeCtrl*)pTree)->UpdateInfoRelating(hTreeItem);

	nResult  =true;
Exit0:
	return nResult;
}

int KGObjectPropertyEditTabDlg::UpdateStateBar()
{
	int nResult  = false;
	int nRetCode = true;

	static int sc_nQuestCount = 0;
	CString szQuestCount  = _T("");

	HTREEITEM hSelTreeItem = NULL;
	int nQuestCount = 0;
	int nProppertyCount = 0;

	nRetCode = ::IsWindow(m_wndStateBar.m_hWnd);
	KG_PROCESS_ERROR(nRetCode);

	if (m_pTabFile)
		nQuestCount = m_pTabFile->GetHeight() - 2;

	KG_PROCESS_SUCCESS(sc_nQuestCount == nQuestCount);
	sc_nQuestCount = nQuestCount;

	szQuestCount.Format(_T("任务个数 : %d"), nQuestCount);

	m_wndStateBar.SetPaneText(2, szQuestCount.GetBuffer());

	// Update 2006-4-22
	/*hSelTreeItem = m_treeObjectView.GetSelectedItem();
	if (!hSelTreeItem)
	{
		m_wndStateBar.SetPaneText(3, _T("属性条数 : "));
	}
	else
	{
		int nTreeItemData = m_treeObjectView.GetItemData(hSelTreeItem);
		if (nTreeItemData < 1)
		{
			m_wndStateBar.SetPaneText(3, _T("属性条数 : "));
		}
	}*/
	//

Exit1:
	nResult = true;
Exit0:
	return nResult;
}

// Updated 2006.3.2
void KGObjectPropertyEditTabDlg::OnNMRclickObjectPropertyTree(
	NMHDR *pNMHDR, LRESULT *pResult
)
{
	int nResult  = false;
	int nRetCode = false;

	POINT   pt;
	CMenu*  pMeu;
	GetCursorPos(&pt);
	CPoint point(pt.x, pt.y);

	KG_PROCESS_ERROR(m_nEditerState != EDIT_STATE_CLOSE);

	m_treeObjectView.ScreenToClient(&point);

	m_hTreeSelItem = m_treeObjectView.HitTest(point);

	m_treeObjectView.SelectItem(m_hTreeSelItem);

	OnClickTaskTree();

	pMeu = m_menuPopup.GetSubMenu(0);
	//pMeu = GetMenu()->GetSubMenu(1);
	pMeu->TrackPopupMenu(TPM_LEFTALIGN, pt.x, pt.y, this);

	nResult = true;
Exit0:
	return;
}

void KGObjectPropertyEditTabDlg::OnTvnSelchangedObjectPropertyTree(
	NMHDR *pNMHDR, LRESULT *pResult
)
{
	int  nResult  = false;
	int  nRetCode = false;

	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	UpdateToolBarState();

	switch(m_eEditType)
	{
	case PROPERTY_EDIT_TYPE_TASE :
		nRetCode = OnClickTaskTree();
		KG_PROCESS_ERROR(nRetCode);
		break;
	default :
		KG_PROCESS_ERROR(false);
	}

	nRetCode = UpdateProperty();
	KG_PROCESS_ERROR(nRetCode);

	//m_nBeSavedFlag = false;

	nResult = true;
Exit0:
	*pResult = 0;
}

void KGObjectPropertyEditTabDlg::OnNMDblclkObjectPropertyList(
	NMHDR *pNMHDR, LRESULT *pResult
)
{
	int nResult    = false;
	int nRetCode   = false;

	HTREEITEM      hCurSelTreeItem   = NULL;
	LPKGLISTITEM   pCurClickListItem = NULL;

	USER_ITEM_DATA itemData;

	POSITION     pos   = m_listProperty.GetFirstSelectedItemPosition();
	int          nItem = m_listProperty.GetNextSelectedItem(pos);

	KG_PROCESS_ERROR(nItem != -1);

	pCurClickListItem = (LPKGLISTITEM)m_listProperty.GetItemData(nItem);
	KG_PROCESS_ERROR(pCurClickListItem);

	m_nBeSavedFlag = false;

	hCurSelTreeItem = m_treeObjectView.GetSelectedItem();
	KG_PROCESS_ERROR(hCurSelTreeItem);

	pCurClickListItem->GetStructData(&itemData, sizeof(itemData));

	if (
		CString(itemData.szPropertyName) == _T("QuestName")
	)
	{
		m_treeObjectView.SetItemText(
			hCurSelTreeItem, itemData.szPropertyValue
		);
	}

Exit0:
	*pResult = 0;
}

int KGObjectPropertyEditTabDlg::UpdateProperty()
{
	int  nResult  = false;
	int  nRetCode = false;

	int  nLoadNumber = 0;

	CString szLoadNumber = "";

	EDIT_STATE eCurEditerState = EDIT_STATE_CLOSE;

	KG_PROCESS_ERROR(m_pQuestIniFile);
	KG_PROCESS_ERROR(m_pTabFile);
	KG_PROCESS_ERROR(m_pTabFile);

	UpdateData();

	if (m_szPreBookMarkName != "")
	{
		m_szBookMark.Replace(_T("\r\n"), _T("\\n"));
		m_pQuestIniFile->WriteString(
			m_szPreBookMarkName.GetBuffer(), _T("BookMark"),
			m_szBookMark.GetBuffer()
		);
	}

	if (m_nCurRow >= 2 && m_nCurRow != SUBSET_TREE_DATA)
	{
		m_listProperty.UpdateProperty(m_nCurRow, &nLoadNumber);
		szLoadNumber.Format(_T("属性条数 : %d"), nLoadNumber);
		m_wndStateBar.SetPaneText(3, szLoadNumber.GetBuffer());

		eCurEditerState = EDIT_STATE_EDIT_PROPERTY;
	}
	else
	{
		char szBookMark[4000];

		m_pQuestIniFile->GetString(
			m_szCurBookMarkName.GetBuffer(), _T("BookMark"), "",
			szBookMark, sizeof(szBookMark)
		);
		m_szBookMark = szBookMark;
		m_szBookMark.Replace(_T("\\n"), _T("\r\n"));
		UpdateData(false);
		m_szPreBookMarkName = m_szCurBookMarkName;

		if (m_nCurRow == SUBSET_TREE_DATA)
			eCurEditerState = EDIT_STATE_EDIT_SUBSETBOOKMARK;
		else
			eCurEditerState = EDIT_STATE_EDIT_BOOKMARK;

		szLoadNumber = _T("属性条数 : ");
		m_wndStateBar.SetPaneText(3, szLoadNumber.GetBuffer());
	}

	if (eCurEditerState == EDIT_STATE_EDIT_SUBSETBOOKMARK)
	{
		HTREEITEM hSelTreeItem   = m_treeObjectView.GetSelectedItem();
		HTREEITEM hTreeChildItem = m_treeObjectView.GetChildItem(hSelTreeItem);
		HTREEITEM hTreeNextItem  = NULL;

		if (hTreeChildItem)
		{
			hTreeChildItem = m_treeObjectView.GetLastSibingItem(hTreeChildItem);
		}

		if (hTreeChildItem)
		{
			hTreeNextItem = m_treeObjectView.GetNextDataItem(hTreeChildItem);
		}

		if (hTreeNextItem)
		{
			char szTempValue[2];
			*szTempValue = '\0';
			int nTreeItemPos = m_treeObjectView.GetItemPos(hTreeNextItem);
			nRetCode = m_pTabFile->GetString(
				nTreeItemPos, _T("PrequestLogic"), _T(""),
				szTempValue, sizeof(szTempValue)
			);
			if (CString(szTempValue) == _T("1") )
			{
				((CButton*)GetDlgItem(IDC_AND))->SetCheck(true);
				((CButton*)GetDlgItem(IDC_OR))->SetCheck(false);
			}
			else if (CString(szTempValue) == _T("0"))
			{
				((CButton*)GetDlgItem(IDC_AND))->SetCheck(false);
				((CButton*)GetDlgItem(IDC_OR))->SetCheck(true);
			}
			else
			{
				((CButton*)GetDlgItem(IDC_AND))->SetCheck(false);
				((CButton*)GetDlgItem(IDC_OR))->SetCheck(false);
			}

			GetDlgItem(IDC_AND)->EnableWindow(true);
			GetDlgItem(IDC_OR)->EnableWindow(true);
		}
		else
		{
			GetDlgItem(IDC_AND)->EnableWindow(false);
			GetDlgItem(IDC_OR)->EnableWindow(false);
		}
	}

	if (eCurEditerState != m_nEditerState)
	{
		m_nEditerState = eCurEditerState;
		UpdateGrid(true);
	}

	m_wndStateBar.SetPaneText(0, m_szCurBookMarkName.GetBuffer());

	nResult = true;
Exit0:
	return nResult;
}

int KGObjectPropertyEditTabDlg::OnClickTaskTree()
{
	int nResult  = false;
	int nRetCode = false;

	int nItemData = 0;
	int nQuestId  = 0;
	int nRow      = 3;
	int nCol      = 1;
	int nWidth    = 0;
	int nHeight   = 0;

	CString szText = "";
	CString szName = "";

	KG_PROCESS_ERROR(m_pTabFile);

	HTREEITEM hItem = m_treeObjectView.GetSelectedItem();
	KG_PROCESS_ERROR(hItem);

	m_hTreeSelItem = hItem;
	szText = m_treeObjectView.GetItemText(hItem);
	nItemData = (int)m_treeObjectView.GetItemData(hItem);

	/*
	do
	{
		szName =
			m_treeObjectView.GetItemText(hItem) + "-" + szName;
		hItem  = m_treeObjectView.GetParentItem(hItem);
	}
	while (hItem);
	*/

	while (hItem)
	{
		szName = m_treeObjectView.GetItemText(hItem) + szName;
		hItem  = m_treeObjectView.GetParentItem(hItem);
		if (hItem)
			szName = _T("-") + szName;
	}

	m_szCurBookMarkName = szName;

	nWidth  = m_pTabFile->GetWidth();
	nHeight = m_pTabFile->GetHeight();

	KG_PROCESS_ERROR(nWidth);
	KG_PROCESS_SUCCESS(nHeight < 2);


	if (nItemData < 1 || nItemData == SUBSET_TREE_DATA)
	{
		m_nCurRow = nItemData;
		KG_PROCESS_SUCCESS(true);
	}

	for (nRow = 3; nRow <= nHeight; nRow++)
	{
		m_pTabFile->GetInteger(nRow, "QuestID", -1000, &nQuestId);
		KG_PROCESS_ERROR(nQuestId != -1);
		if (nQuestId == nItemData)
		{
			break;
		}
	}

	KG_PROCESS_ERROR(nRow <= nHeight);
	m_nCurRow = nRow;

Exit1:
	nResult = true;
Exit0:
	if (nQuestId == -1000)
	{
		AfxMessageBox("文件格式有错!");
		UnInit();
	}

	return nResult;
}

void KGObjectPropertyEditTabDlg::OnSize(UINT nType, int cx, int cy)
{
	int nResult  = false;
	int nRetCode = false;

	CRect rectMainFrame;
	CRect rectTypeCombo;
	CRect rectIniWindow;
	CRect rectProcess;
	CRect rectLogAnd;
	CRect rectLogOr;

	CStatusBarCtrl* pStatusBarCtrl = &m_wndStateBar.GetStatusBarCtrl();
	GetClientRect(rectMainFrame);

	if (m_nFirstResizeFlag)
	{
		m_nFirstResizeFlag = false;
		KG_PROCESS_SUCCESS(true);
	}

	m_rectTree.left   = 0;
	m_rectTree.top    = 30;
	m_rectTree.bottom = rectMainFrame.bottom - 18;

	m_rectList.top    = 30;
	m_rectList.right  = rectMainFrame.right;
	m_rectList.bottom = rectMainFrame.bottom - 18;

	rectTypeCombo.left   = m_rectList.right - 160;
	rectTypeCombo.right  = m_rectList.right;
	rectTypeCombo.bottom = m_rectList.bottom;
	rectTypeCombo.top    = 4;

	rectIniWindow.left   = m_rectTree.left;
	rectIniWindow.top    = m_rectTree.top;
	rectIniWindow.right  = m_rectList.right;
	rectIniWindow.bottom = m_rectList.bottom;

	GetDlgItem(IDC_AND)->GetWindowRect(rectLogAnd);
	GetDlgItem(IDC_OR)->GetWindowRect(rectLogOr);

	rectLogAnd.left      = m_rectList.left + 2;
	rectLogAnd.top       = m_rectList.top;
	rectLogAnd.right     = rectLogAnd.left + 100;
	rectLogAnd.bottom    = rectLogAnd.top  + 20;

	rectLogOr.left		 = rectLogAnd.left + rectLogAnd.Width();
	rectLogOr.top        = m_rectList.top;
	rectLogOr.right      = m_rectList.right;
	rectLogOr.bottom     = rectLogOr.top  + 20;

	m_comBoxType.MoveWindow(rectTypeCombo);
	pStatusBarCtrl->GetRect(1, &rectProcess);
	m_proessBar.MoveWindow(rectProcess);

	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);

	if (m_nEditerState == EDIT_STATE_EDIT_PROPERTY)
	{
		m_treeObjectView.MoveWindow(m_rectTree);
		m_listProperty.MoveWindow(m_rectList);

		m_treeObjectView.ShowWindow(SW_SHOW);
		m_listProperty.ShowWindow(SW_SHOW);
		m_editBookMarker.ShowWindow(SW_HIDE);
		m_iniWinidow.ShowWindow(SW_HIDE);

		GetDlgItem(IDC_AND)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_OR)->ShowWindow(SW_HIDE);
	}
	else if (m_nEditerState == EDIT_STATE_EDIT_BOOKMARK)
	{
		m_editBookMarker.MoveWindow(m_rectList);
		m_treeObjectView.MoveWindow(m_rectTree);

		m_editBookMarker.ShowWindow(SW_SHOW);
		m_listProperty.ShowWindow(SW_HIDE);
		m_treeObjectView.ShowWindow(SW_SHOW);
		m_iniWinidow.ShowWindow(SW_HIDE);

		GetDlgItem(IDC_AND)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_OR)->ShowWindow(SW_HIDE);
	}
	else if (m_nEditerState == EDIT_STATE_EDIT_SUBSETBOOKMARK)
	{
		GetDlgItem(IDC_AND)->MoveWindow(rectLogAnd);
		GetDlgItem(IDC_OR)->MoveWindow(rectLogOr);

		m_rectList.top = rectLogAnd.bottom + 2;

		m_editBookMarker.MoveWindow(m_rectList);
		m_treeObjectView.MoveWindow(m_rectTree);

		m_editBookMarker.ShowWindow(SW_SHOW);
		m_listProperty.ShowWindow(SW_HIDE);
		m_treeObjectView.ShowWindow(SW_SHOW);
		m_iniWinidow.ShowWindow(SW_HIDE);

		GetDlgItem(IDC_AND)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_OR)->ShowWindow(SW_SHOW);
	}
	else
	{
		m_iniWinidow.MoveWindow(rectIniWindow);

		m_editBookMarker.ShowWindow(SW_HIDE);
		m_listProperty.ShowWindow(SW_HIDE);
		m_treeObjectView.ShowWindow(SW_HIDE);
		m_iniWinidow.ShowWindow(SW_SHOW);

		GetDlgItem(IDC_AND)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_OR)->ShowWindow(SW_HIDE);
	}

	if (m_nRefustCountFlag)
	{
		m_nRefustCountFlag = false;
		PostMessage(WM_SIZE);
	}
	else
	{
		m_nRefustCountFlag = true;
	}

Exit1:
	nResult = true;
//Exit0:
	return;
}

int KGObjectPropertyEditTabDlg::UnInit()
{
	int nResult  = false;
	int nRetCode = false;

	int nIndex   = 0;

	SAFE_RELEASE(m_pQuestIniFile);
	SAFE_RELEASE(m_pTabFile);
	SAFE_RELEASE(m_pIniFile);

	for (nIndex = 0; nIndex < m_nClassifyFieldsSize; nIndex++)
	{
		SAFE_DELETE_ARRAY(m_ppszClassifyFields[nIndex])
	}
	SAFE_DELETE_ARRAY(m_ppszClassifyFields);
	SAFE_DELETE_ARRAY(m_pszCaptionField);
	SAFE_DELETE_ARRAY(m_pszPrimaryKesField);

	KG_PROCESS_ERROR(m_pView);
	m_pView->m_pTaseDlg = NULL;

//Exit1:
	nResult = true;
Exit0:
	return nResult;
}

int  KGObjectPropertyEditTabDlg::UpdateTaskTree()
{
	int  nResult  = false;
	int  nRetCode = false;

	nRetCode = m_treeObjectView.Update();
	KG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	return nResult;
}

void KGObjectPropertyEditTabDlg::OnCancel()
{
	int nResult  = false;
	int nRetCode = false;

	KG_PROCESS_ERROR(m_pQuestIniFile);
	KG_PROCESS_ERROR(m_pTabFile);
	KG_PROCESS_ERROR(m_pIniFile);

	if (!m_nBeSavedFlag)
	{
		nRetCode = ::MessageBox(
			this->m_hWnd, _T("你有更改没保存, 是否保存当前的编辑?"), _T("警告!"),
			MB_YESNOCANCEL | MB_ICONWARNING
		);

		if (nRetCode == IDYES)
		{
			OnSave();
		}
		else if (nRetCode == IDCANCEL)
		{
			return;
		}
	}

Exit0:
	DestroyWindow();
}

void KGObjectPropertyEditTabDlg::PostNcDestroy()
{
	CDialog::PostNcDestroy();
	delete this;
}

void KGObjectPropertyEditTabDlg::OnBnClickedCheckAnd()
{
	int nRetCode = false;

	HTREEITEM hSelTreeItem   = NULL;
	HTREEITEM hTreeChildItem = NULL;
	HTREEITEM hTreeNextItem  = NULL;

	hSelTreeItem   = m_treeObjectView.GetSelectedItem();
	hTreeChildItem = m_treeObjectView.GetChildItem(hSelTreeItem);

	if (hTreeChildItem)
	{
		hTreeChildItem = m_treeObjectView.GetLastSibingItem(hTreeChildItem);
	}
	else
	{
		KG_PROCESS_SUCCESS(true);
	}

	hTreeNextItem = m_treeObjectView.GetNextDataItem(hTreeChildItem);

	KG_PROCESS_SUCCESS(!hTreeNextItem);

	nRetCode = ((CButton*)GetDlgItem(IDC_AND))->GetCheck();

	if (nRetCode)
	{
		if (m_treeObjectView.ItemIsInSubset(hTreeNextItem))
		{
			while (hTreeNextItem)
			{
				int nTreeItemPos = m_treeObjectView.GetItemPos(hTreeNextItem);
				m_pTabFile->WriteInteger(nTreeItemPos, _T("PrequestLogic"), 1);
				hTreeNextItem = m_treeObjectView.GetNextSiblingItem(hTreeNextItem);
			}
		}
		else
		{
			int nTreeItemPos = m_treeObjectView.GetItemPos(hTreeNextItem);
			m_pTabFile->WriteInteger(nTreeItemPos, _T("PrequestLogic"), 1);
		}

		((CButton*)GetDlgItem(IDC_OR))->SetCheck(false);
	}
	else
	{
		if (m_treeObjectView.ItemIsInSubset(hTreeNextItem))
		{
			while (hTreeNextItem)
			{
				int nTreeItemPos = m_treeObjectView.GetItemPos(hTreeNextItem);
				m_pTabFile->WriteInteger(nTreeItemPos, _T("PrequestLogic"), 0);
				hTreeNextItem = m_treeObjectView.GetNextSiblingItem(hTreeNextItem);
			}
		}
		else
		{
			int nTreeItemPos = m_treeObjectView.GetItemPos(hTreeNextItem);
			m_pTabFile->WriteInteger(nTreeItemPos, _T("PrequestLogic"), 0);
		}
		((CButton*)GetDlgItem(IDC_OR))->SetCheck(true);
	}

	m_treeObjectView.Invalidate();
Exit1:
	return;
}

void KGObjectPropertyEditTabDlg::OnBnClickedCheckOr()
{
	int nRetCode = false;

	HTREEITEM hSelTreeItem   = NULL;
	HTREEITEM hTreeChildItem = NULL;
	HTREEITEM hTreeNextItem  = NULL;

	hSelTreeItem   = m_treeObjectView.GetSelectedItem();
	hTreeChildItem = m_treeObjectView.GetChildItem(hSelTreeItem);

	if (hTreeChildItem)
	{
		hTreeChildItem = m_treeObjectView.GetLastSibingItem(hTreeChildItem);
	}
	else
	{
		KG_PROCESS_SUCCESS(true);
	}

	hTreeNextItem = m_treeObjectView.GetNextDataItem(hTreeChildItem);

	KG_PROCESS_SUCCESS(!hTreeNextItem);

	nRetCode = ((CButton*)GetDlgItem(IDC_OR))->GetCheck();

	if (nRetCode)
	{
		if (m_treeObjectView.ItemIsInSubset(hTreeNextItem))
		{
			while (hTreeNextItem)
			{
				int nTreeItemPos = m_treeObjectView.GetItemPos(hTreeNextItem);
				m_pTabFile->WriteInteger(nTreeItemPos, _T("PrequestLogic"), 0);
				hTreeNextItem = m_treeObjectView.GetNextSiblingItem(hTreeNextItem);
			}
		}
		else
		{
			int nTreeItemPos = m_treeObjectView.GetItemPos(hTreeNextItem);
			m_pTabFile->WriteInteger(nTreeItemPos, _T("PrequestLogic"), 0);
		}

		((CButton*)GetDlgItem(IDC_AND))->SetCheck(false);
	}
	else
	{
		if (m_treeObjectView.ItemIsInSubset(hTreeNextItem))
		{
			while (hTreeNextItem)
			{
				int nTreeItemPos = m_treeObjectView.GetItemPos(hTreeNextItem);
				m_pTabFile->WriteInteger(nTreeItemPos, _T("PrequestLogic"), 1);
				hTreeNextItem = m_treeObjectView.GetNextSiblingItem(hTreeNextItem);
			}
		}
		else
		{
			int nTreeItemPos = m_treeObjectView.GetItemPos(hTreeNextItem);
			m_pTabFile->WriteInteger(nTreeItemPos, _T("PrequestLogic"), 1);
		}
		((CButton*)GetDlgItem(IDC_AND))->SetCheck(true);
	}

	m_treeObjectView.Invalidate();
Exit1:
	return;
}


HBRUSH KGObjectPropertyEditTabDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hResult;

	if (pWnd == &m_editBookMarker)
	{
		hResult = ::CreateSolidBrush(RGB(255, 255, 255));
		KG_PROCESS_SUCCESS(true);
	}

	hResult = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

Exit1:
	return hResult;
}

void KGObjectPropertyEditTabDlg::OnOK()
{
	return;
}
