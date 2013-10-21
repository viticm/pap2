////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DObjectEditor.cpp
//  Version     : 1.0
//  Author      : ZhaoXinyu (Chiyer)
//  CopyRight   : Kingsoft Corporation All rights Reserved
//  History     : 2006-3-20 13:35:31
//  Comment     :
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KSceneObjectEditorPanelView.h"
#include "KG3DObjectEditor.h"
#include "KGLog.h"
#include "resource.h"
#include "KG3DFileViewDialog.h"
//#include "KG3DSceneObjectEditor.h"
//#include "KG3DRepresentObject.h"
#include "KGValueEditGetValueDlg.h"
#include "KGNewObjectOption.h"
//#include "kg3denginemanager.h" //by dengzhihui Refactor 2006年11月6日
//#include "SceneEditorCommon.h"	 //by dengzhihui Refactor 2006年11月6日


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

extern IKG3DScene* GetBaseScene(IEKG3DSceneObjectEditor* p);


TCHAR g_strWeaponDirectory[MAX_PATH];
TCHAR g_strItemDirectory[MAX_PATH];

IMPLEMENT_DYNAMIC(KG3DObjectEditor, CDialog)

KG3DObjectEditor::KG3DObjectEditor(CWnd* pParent /*=NULL*/)
	: CDialog(KG3DObjectEditor::IDD, pParent)
{
	m_pParentWindow  = (KSceneObjectEditorPanelView*)pParent;
	m_pControlPanel  = NULL;
	m_pPropertyList  = NULL;
	m_pObjectScene   = NULL;
	m_nHaveShowFlag  = false;
	m_nTabFilePos    = 0;
	sprintf_s(g_strWeaponDirectory, MAX_PATH, "%sData\\source\\item\\weapon\\", g_szDefWorkDirectory);
	sprintf_s(g_strItemDirectory, MAX_PATH, "%sData\\source\\item\\prop\\", g_szDefWorkDirectory);
}

KG3DObjectEditor::~KG3DObjectEditor()
{
	UnInitSubEditer();
	UnInitScene();
	UnInitInterface();
}

void KG3DObjectEditor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE, m_treeCtrlObjectList);
	DDX_Control(pDX, IDC_TAB,  m_tabCtrlClassify);
	DDX_Control(pDX, IDC_VIEW, m_wndObjectView);
}

void KGInitDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS, m_progressCtrl);
	DDX_Control(pDX, IDC_TEXT,     m_staticCtrl);
}

BEGIN_MESSAGE_MAP(KG3DObjectEditor, CDialog)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE,		 OnTvnSelchangedObjectPropertyTree)
	ON_NOTIFY(NM_RCLICK, IDC_TREE,			 OnNMRclickObjectPropertyTree)
	ON_COMMAND(ID_PROPERTY_DLG,				 OnCommandShowListWindow)
	ON_COMMAND(ID_CONTROL_DLG,				 OnCommandShowControlColumn)
	ON_COMMAND(ID_NEW,						 OnNewObject)
	ON_COMMAND(ID_NEW_CD,					 OnNewDirChld)
	ON_COMMAND(ID_NEW_SD,					 OnNewDirSlib)
	ON_COMMAND(ID_COPY,						 OnCopy)
	ON_COMMAND(ID_PASE,						 OnPase)
	ON_COMMAND(ID_SAVE,						 OnSave)
	ON_COMMAND(ID_DEL,						 OnDel)
	ON_COMMAND(ID_CUT,						 OnCut)
	ON_MESSAGE(WM_NOTIFY_ITEM_VALUE_UPDATE,	 OnListItemUpdate)
	ON_MESSAGE(WM_LIST_UPDATE_SCENE,		 OnUpdateScene)
	ON_COMMAND(ID_RENAME,					 OnReName)
	ON_WM_INITMENUPOPUP()
	ON_WM_SHOWWINDOW()
	ON_WM_SIZE()
	ON_WM_MOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	ON_WM_SETFOCUS()
	ON_WM_CTLCOLOR()
	ON_WM_ACTIVATE()
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXT,  0, 0xFFFF, OnToolTipText)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB, &KG3DObjectEditor::OnTcnSelchangeTab)
END_MESSAGE_MAP()


BOOL KG3DObjectEditor::OnInitDialog()
{
	return CDialog::OnInitDialog();
}


void KG3DObjectEditor::OnShowWindow(BOOL bShow, UINT nStatus)
{
	int nResult  = false;
	int nRetCode = false;

	KG_PROCESS_SUCCESS(m_nHaveShowFlag);

	nRetCode = InitInterface();
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = UpdateInterface();
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = InitSubEditer();
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = InitScene();
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = CheckSubEditerConsistency();
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = UpdateSubEditer();
	KGLOG_PROCESS_ERROR(nRetCode);

	m_pPropertyList->SendMessage(WM_PAINT);

	SetTimer(0, RENDER_DELAY_TIME, NULL);

Exit1:
	m_nHaveShowFlag = true;
	nResult = true;
Exit0:
	if (!nResult)
	{
		::MessageBox(
			this->m_hWnd, _T("Object Editer Initalize Error !!"), _T("Error"),
			MB_OK | MB_ICONERROR
		);
		OnCancel();
	}
	return;
}


int KG3DObjectEditor::UpdateInterface()
{
	SendMessage(WM_SIZE);
	return true;
}


int KG3DObjectEditor::InitInterface()
{
	int nResult  = false;
	int nRetCode = false;

	HIMAGELIST hImageList = NULL;
//	SHFILEINFO shFileInfo;

	static UINT indicators[] =
	{
		ID_SEPARATOR,
		ID_SEPARATOR,
		ID_SEPARATOR,
		ID_SEPARATOR
	};

	DWORD dwStyle = 0;

	nRetCode = m_wndToolBar.Create(
		this, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER |
		CBRS_TOOLTIPS  | CBRS_FLYBY | CBRS_SIZE_DYNAMIC
	);
	KGLOG_PROCESS_ERROR(nRetCode);
	nRetCode = m_wndToolBar.LoadToolBar(IDR_OBJECT_EDITER);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = m_wndStateBar.Create(this);
	KGLOG_PROCESS_ERROR(nRetCode);
	nRetCode = m_wndStateBar.SetIndicators(indicators, sizeof(indicators) / sizeof(UINT));
	KGLOG_PROCESS_ERROR(nRetCode);

	m_wndStateBar.SetPaneInfo(0, ID_SEPARATOR, SBPS_NOBORDERS | SBPS_STRETCH, 0);
	m_wndStateBar.SetPaneInfo(1, ID_SEPARATOR, SBPS_NORMAL, 200);
	m_wndStateBar.SetPaneInfo(2, ID_SEPARATOR, SBPS_NORMAL, 100);
	m_wndStateBar.SetPaneInfo(3, ID_SEPARATOR, SBPS_NORMAL, 100);
	m_wndStateBar.SetPaneText(0, _T("就绪 >_<"));

	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);

	m_wndStateBar.GetWindowRect(&m_rectStateBar);
	m_wndToolBar.GetWindowRect(&m_rectToolBar);

	m_rectTabCtrl.right = INIT_RECT_TABCTRL_RIGHT;

	GetClientRect(&m_rectListCtrl);
	m_rectListCtrl.left   = m_rectListCtrl.right - INIT_LIST_CTRL_WIDTH;

	/*
	hImageList = (HIMAGELIST)SHGetFileInfo(
		_T("C:\\"), 0, &shFileInfo, sizeof(shFileInfo),
		SHGFI_SYSICONINDEX | SHGFI_SMALLICON
	);

	m_imageListTree.m_hImageList = hImageList;
	*/

	m_imageListTree.Create(16, 16, ILC_COLOR24, 4, 4);
	m_imageListTree.Add(AfxGetApp()->LoadIcon(IDC_FC));
	m_imageListTree.Add(AfxGetApp()->LoadIcon(IDC_FO));
	m_imageListTree.Add(AfxGetApp()->LoadIcon(IDC_TALK));

	m_treeCtrlObjectList.SetImageList(&m_imageListTree, TVSIL_NORMAL);
	m_treeCtrlObjectList.SetParent(&m_tabCtrlClassify);
	m_treeCtrlObjectList.SetTextColor(RGB(255, 255, 255));
	m_treeCtrlObjectList.SetLineColor(RGB(122, 122, 122));
	m_treeCtrlObjectList.SetBkColor(RGB(92,  92, 92));

	m_pPropertyList = new KGObjectEditerProjectListDlg(this);
	m_pPropertyList->Create(IDD_DIALOG_OBJECT_EDITER_LIST, this);
	m_pPropertyList->ShowWindow(SW_SHOW);

	m_pControlPanel = new KGControlColumnDlg(this);
	m_pControlPanel->Create(IDD_DIALOG_CONTROL_PANEL, this);;
	m_pControlPanel->ShowWindow(SW_SHOW);

	dwStyle = GetWindowLong(m_treeCtrlObjectList.m_hWnd, GWL_STYLE);
	dwStyle |= TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT;
	SetWindowLong(m_treeCtrlObjectList.m_hWnd,GWL_STYLE,dwStyle);

	dwStyle = GetWindowLong(m_hWnd, GWL_STYLE);
	dwStyle |= WS_SIZEBOX;
	SetWindowLong(m_hWnd, GWL_STYLE, dwStyle);

	m_menuPopup.LoadMenu(IDR_OBJECT);

	SetWindowText(_T("我是物件编辑器v1.0"));

	m_nLButtonFlag  = false;
	m_nLSizeBarFlag = false;
	m_nRSizeBarFlag = false;

	nResult = true;
Exit0:
	return nResult;
}


int KG3DObjectEditor::UnInitInterface()
{
	int nResult  = false;
	int nRetCode = false;

	if (m_pPropertyList)
		m_pPropertyList->DestroyWindow();
	if (m_pControlPanel)
		m_pControlPanel->DestroyWindow();

	SAFE_DELETE(m_pPropertyList);
	SAFE_DELETE(m_pControlPanel);

	if (m_pParentWindow)
	{
		m_pParentWindow->m_pObjectDlg = NULL;
	}

	nResult = true;
//Exit0:
	return nResult;
}


int KG3DObjectEditor::InitSubEditer()
{
	int nResult  = false;
	int nRetCode = false;

	int nLoopStep = 0;

	ITabFile* pEachTabFile[2];
	IIniFile* pSettingIniFile = NULL;

	char szSettingFileName[MAX_PATH];

	char szSectionName[SECTION_NAME_SIZE];
//	char szKeyName[KEY_NAME_SIZE];

	char szClassifyFileName[MAX_PATH];
	char szIniAssitFileName[MAX_PATH];
	char szTabFileName[MAX_PATH];
//	char szTabFileType[64];
	char szPrimaryKeyField[SECTION_NAME_SIZE];

	int  nMinPrimaryKey = 0;
	int  nMaxPrimaryKey = 0;
	int  nExportIndex   = 0;
	int  nSubEditerID   = 0;

	*szPrimaryKeyField  = '\0';
	*szSettingFileName  = '\0';
	*szClassifyFileName = '\0';
	*szSectionName	    = '\0';
	*szTabFileName      = '\0';
	*szIniAssitFileName	= '\0';

	for (nLoopStep = 0; nLoopStep < 2; nLoopStep++)
		pEachTabFile[nLoopStep] = NULL;

	g_GetFullPath(szSettingFileName, SETTING_INI_FILE_NAME);

	pSettingIniFile = g_OpenIniFile(szSettingFileName);
	KGLOG_PROCESS_ERROR(pSettingIniFile);

	while (pSettingIniFile->GetNextSection(szSectionName, szSectionName))
	{
		KGSubEditer* psubEditer = new KGSubEditer();

		nRetCode = pSettingIniFile->GetString(
			szSectionName, _T("Classify"), _T(""), szClassifyFileName,
			sizeof(szClassifyFileName)
		);
		KGLOG_PROCESS_ERROR(nRetCode);

		g_GetFullPath(psubEditer->m_szClassifyFilePath, szClassifyFileName);

		psubEditer->m_pClassifyFile = g_OpenIniFile(
			psubEditer->m_szClassifyFilePath, false, true
		);
		KGLOG_PROCESS_ERROR(psubEditer->m_pClassifyFile);

		nRetCode = pSettingIniFile->GetString(
			szSectionName, _T("Name"), _T(""), psubEditer->m_szSubEditerName,
			sizeof(psubEditer->m_szSubEditerName)
		);
		KGLOG_PROCESS_ERROR(nRetCode);

		// GameLogic TabFile
		nRetCode = pSettingIniFile->GetString(
			szSectionName, _T("IniFile_L"), _T(""), szIniAssitFileName,
			sizeof(szIniAssitFileName)
		);
		KGLOG_PROCESS_ERROR(nRetCode);

		g_GetFullPath(psubEditer->m_szIniFilePath_GameLogic, szIniAssitFileName);

		psubEditer->m_pGameLogicIniFile = g_OpenIniFile(
			psubEditer->m_szIniFilePath_GameLogic, false, true
		);
		KGLOG_PROCESS_ERROR(psubEditer->m_pGameLogicIniFile);

		nRetCode = pSettingIniFile->GetString(
			szSectionName, _T("TabFile_L"), _T(""), szTabFileName, sizeof(szTabFileName)
		);

		g_GetFullPath(psubEditer->m_szTabFilePath_GameLogic, szTabFileName);

		psubEditer->m_pGameLogicTabFile = g_OpenTabFile(
			psubEditer->m_szTabFilePath_GameLogic, false, true
		);
		KGLOG_PROCESS_ERROR(psubEditer->m_pGameLogicTabFile);

		// Represent TabFile
		nRetCode = pSettingIniFile->GetString(
			szSectionName, _T("IniFile_R"), _T(""), szIniAssitFileName,
			sizeof(szIniAssitFileName)
		);
		KGLOG_PROCESS_ERROR(nRetCode);

		g_GetFullPath(psubEditer->m_szIniFilePath_Represent, szIniAssitFileName);

		psubEditer->m_pRepresentIniFile = g_OpenIniFile(
			psubEditer->m_szIniFilePath_Represent, false, true
		);
		KGLOG_PROCESS_ERROR(psubEditer->m_pRepresentIniFile);

		nRetCode = pSettingIniFile->GetString(
			szSectionName, _T("TabFile_R"), _T(""), szTabFileName, sizeof(szTabFileName)
		);

		g_GetFullPath(psubEditer->m_szTabFilePath_Represent, szTabFileName);

		psubEditer->m_pRepresentTabFile = g_OpenTabFile(
			psubEditer->m_szTabFilePath_Represent, false, true
		);
		KGLOG_PROCESS_ERROR(psubEditer->m_pRepresentTabFile);

		// PrimaryKey Manager
		nRetCode = psubEditer->m_pGameLogicIniFile->GetString(
			_T("Main"), _T("PrimaryKey_Field"), _T(""), szPrimaryKeyField,
			sizeof(szPrimaryKeyField)
		);
		KGLOG_PROCESS_ERROR(nRetCode);

		psubEditer->m_pGameLogicIniFile->GetInteger(
			szPrimaryKeyField, _T("Min"), 0, &nMinPrimaryKey
		);
		psubEditer->m_pGameLogicIniFile->GetInteger(
			szPrimaryKeyField, _T("Max"), 0, &nMaxPrimaryKey
		);

		KGLOG_PROCESS_ERROR(nMaxPrimaryKey > nMinPrimaryKey);

		nRetCode = psubEditer->m_primaryKeyManager_L.Refrush(
			nMinPrimaryKey, nMaxPrimaryKey
		);
		KGLOG_PROCESS_ERROR(nRetCode);

		g_IniPrimaryKesManager(
			&psubEditer->m_primaryKeyManager_L, &(psubEditer->m_pGameLogicTabFile), 1,
			szPrimaryKeyField
		);

		nRetCode = psubEditer->m_pRepresentIniFile->GetString(
			_T("Main"), _T("PrimaryKey_Field"), _T(""), szPrimaryKeyField,
			sizeof(szPrimaryKeyField)
		);
		KGLOG_PROCESS_ERROR(nRetCode);

		psubEditer->m_pRepresentIniFile->GetInteger(
			szPrimaryKeyField, _T("Min"), 0, &nMinPrimaryKey
		);
		psubEditer->m_pRepresentIniFile->GetInteger(
			szPrimaryKeyField, _T("Max"), 0, &nMaxPrimaryKey
		);

		KGLOG_PROCESS_ERROR(nMaxPrimaryKey > nMinPrimaryKey);

		nRetCode = psubEditer->m_primaryKeyManager_R.Refrush(
			nMinPrimaryKey, nMaxPrimaryKey
		);

		KGLOG_PROCESS_ERROR(nRetCode);
		g_IniPrimaryKesManager(
			&psubEditer->m_primaryKeyManager_R, &(psubEditer->m_pRepresentTabFile), 1,
			szPrimaryKeyField
		);

		m_arraySubEditer.Add(psubEditer);
		m_tabCtrlClassify.InsertItem(nSubEditerID++, szSectionName);

	}

	m_nCurSubEditerIndex = 0;
	m_pPropertyList->BandData(
		&m_arraySubEditer[m_nCurSubEditerIndex]->m_pGameLogicTabFile,
		&m_arraySubEditer[m_nCurSubEditerIndex]->m_pGameLogicIniFile,
		&m_arraySubEditer[m_nCurSubEditerIndex]->m_pRepresentTabFile,
		&m_arraySubEditer[m_nCurSubEditerIndex]->m_pRepresentIniFile
	);
	m_pControlPanel->BandData(
		&m_arraySubEditer[m_nCurSubEditerIndex]->m_pRepresentTabFile,
		&m_arraySubEditer[m_nCurSubEditerIndex]->m_pRepresentIniFile
	);

	/*
	m_pObjectScene->BandObjectTemplate(
		m_arraySubEditer[m_nCurSubEditerIndex]->m_pUniteTabFile
	);
	*/

	nResult = true;
Exit0:
	SAFE_RELEASE(pSettingIniFile);
	return nResult;
}


int KG3DObjectEditor::UnInitSubEditer()
{
	int nResult  = false;
	int nRetCode = false;

	INT_PTR nArraySize  = 0;
	INT_PTR nArrayIndex = 0;

	nArraySize = m_arraySubEditer.GetSize();

	for (nArrayIndex = 0; nArrayIndex < nArraySize; nArrayIndex++)
	{
		m_arraySubEditer[nArrayIndex]->Save();
		SAFE_DELETE(m_arraySubEditer[nArrayIndex]);
	}
	m_arraySubEditer.RemoveAll();

	nResult = true;
//Exit0:
	return nResult;
}


int KG3DObjectEditor::InitScene()
{
	int nResult    = false;
	int nRetCode   = false;

	int nOutputWindowID = 0;
//	char szFullPath[MAX_PATH];

	//by dengzhihui Refactor 2006年11月6日
	g_pEngineManager->NewOneScene(
		SCENETYPE_OBJECTEDITOR,(IKG3DScene**)&m_pObjectScene
	);
	KGLOG_PROCESS_ERROR(m_pObjectScene);

	/*g_GetFullPath(szFullPath, "\\Data\\source\\maps\\物件编辑器测试场景\\ObjectTest.map");
	m_pObjectScene->LoadFromFile(szFullPath,0,0);
	g_pEngineManager->SetFocusScene(m_pObjectScene);*/

	IKG3DScene* pScene = GetBaseScene(m_pObjectScene);
	g_pEngineManager->SetFocusScene(pScene);//by dengzhihui Refactor 2006年11月6日
	pScene->AddOutputWindow(
		_T("OBJECT EDITER RENDER WINDOW"), m_wndObjectView.m_hWnd,
		OUTPUTWND_OBJECTEDITOR_MAIN, 0, &nOutputWindowID
	);

	m_pObjectScene->BandObjectTemplate(
		m_arraySubEditer[m_nCurSubEditerIndex]->m_pRepresentTabFile,
		m_arraySubEditer[m_nCurSubEditerIndex]->m_pGameLogicTabFile
	);

	//TODO:by dengzhihui Refactor 2006年11月15日, 待IKG3DScene接口完成
	//m_pPropertyList->m_listPropertyView_L.SetScene(pScene);
	//m_pPropertyList->m_listPropertyView_R.SetScene(pScene);
	//)

	nResult = true;
Exit0:
	return nResult;
}


int KG3DObjectEditor::CheckSubEditerConsistency()
{
	int nResult  = false;
	int nRetCode = false;

	int nIndex   = 0;

	for (nIndex = 0; nIndex < m_arraySubEditer.GetCount(); nIndex++)
	{
		nRetCode = m_arraySubEditer[nIndex]->CheckConsistency();
		KGLOG_PROCESS_ERROR(nRetCode);
	}

	nResult = true;
Exit0:
	return nResult;
}


int KG3DObjectEditor::UnInitScene()
{
	int nResult  = false;
	int nRetCode = false;

	IKG3DScene* pScene = GetBaseScene(m_pObjectScene);
	g_pEngineManager->RemoveScene(pScene);//by dengzhihui Refactor 2006年11月6日

	nResult = true;
//Exit0:
	return nResult;
}


void KG3DObjectEditor::OnNewDirSlib()
{
	int nResult  = false;
	int nRetCode = false;

	HTREEITEM hSelTreeItem = NULL;
	HTREEITEM hNewTreeItem = NULL;
	CString szTitleText = _T("");

	hSelTreeItem = m_treeCtrlObjectList.GetSelectedItem();
	szTitleText.Format(
		_T("请输入要创建的[%s目录]名 :"),
		m_arraySubEditer[m_nCurSubEditerIndex]->m_szSubEditerName
	);
	KGValueEditGetValueDlg dlg(this, szTitleText, 64);

	if (dlg.DoModal() == IDOK)
	{
		szTitleText = dlg.GetValue();
		if (szTitleText == _T(""))
			szTitleText = _T("Unknow Name");
		hNewTreeItem = m_treeCtrlObjectList.InsertItem(
			szTitleText, 0, 0, m_treeCtrlObjectList.GetParentItem(hSelTreeItem),
			hSelTreeItem
		);
		if (hSelTreeItem)
			m_treeCtrlObjectList.SetItemData(
				hNewTreeItem, m_treeCtrlObjectList.GetItemData(hSelTreeItem)
			);
		else
			m_treeCtrlObjectList.SetItemData(hNewTreeItem, 0);
		m_treeCtrlObjectList.SelectItem(hNewTreeItem);
	}

	nResult = true;
//Exit0:
	return;
}


void KG3DObjectEditor::OnNewDirChld()
{
	int nResult  = false;
	int nRetCode = false;

	HTREEITEM hSelTreeItem = NULL;
	HTREEITEM hNewTreeItem = NULL;
	CString szTitleText = _T("");
	int nItemData = 0;

	hSelTreeItem = m_treeCtrlObjectList.GetSelectedItem();
	szTitleText.Format(
		_T("请输入要创建的[%s目录]名 :"),
		m_arraySubEditer[m_nCurSubEditerIndex]->m_szSubEditerName
	);
	KGValueEditGetValueDlg dlg(this, szTitleText, 64);
	if (dlg.DoModal() == IDOK)
	{
		szTitleText = dlg.GetValue();
		if (szTitleText == _T(""))
			szTitleText = _T("Unknow Name");
		hNewTreeItem = m_treeCtrlObjectList.InsertItem(
			szTitleText, 0, 0, hSelTreeItem, NULL
		);
		if (hSelTreeItem)
			m_treeCtrlObjectList.SetItemData(
				hNewTreeItem, m_treeCtrlObjectList.GetItemData(hSelTreeItem) - 1
			);
		else
			m_treeCtrlObjectList.SetItemData(hNewTreeItem, 0);

		m_treeCtrlObjectList.SelectItem(hNewTreeItem);
	}

	nResult = true;
//Exit0:
	return;
}


void KG3DObjectEditor::OnNewObject()
{
	int nResult  = false;
	int nRetCode = false;

	nRetCode = BuildOneObject(NULL);
	KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	return;
}


int  KG3DObjectEditor::BuildOneObject(const char cszObjectName[])
{
	int nResult  = false;
	int nRetCode = false;

	HTREEITEM hSelTreeItem = NULL;
	HTREEITEM hParentItem  = NULL;
	HTREEITEM hAfterItem   = NULL;
	HTREEITEM hNewTreeItem = NULL;

	CString   stringObjectName = _T("");

	int nNewRepresentIdFlag = 1;
	int nNewRepresentId     = 0;
	int	nItemData		    = 0;
	int	nInsertPos_L		= 0;
	int nInsertPos_R		= 0;

	char szPrimaryKeyField[64];
	char szTitleField[64];

	*szPrimaryKeyField = '\0';
	*szTitleField	   = '\0';

	KGNewObjectOption optionDlg(
		m_arraySubEditer[m_nCurSubEditerIndex]->m_pRepresentTabFile, this
	);

	hSelTreeItem = m_treeCtrlObjectList.GetSelectedItem();
	KGLOG_PROCESS_ERROR(hSelTreeItem);

	nItemData = (int)m_treeCtrlObjectList.GetItemData(hSelTreeItem);
	if (nItemData >= 1)
	{
		hParentItem = m_treeCtrlObjectList.GetParentItem(hSelTreeItem);
		hAfterItem  = hSelTreeItem;
	}
	else
	{
		hParentItem = hSelTreeItem;
		hAfterItem  = NULL;
	}

	KG_PROCESS_SUCCESS(optionDlg.DoModal() != IDOK);

	stringObjectName = optionDlg.GetObjectName();

	hNewTreeItem = m_treeCtrlObjectList.InsertItem(
		stringObjectName, 2, 2, hParentItem, hAfterItem
	);
	KGLOG_PROCESS_ERROR(hNewTreeItem);

	nRetCode =
		m_arraySubEditer[m_nCurSubEditerIndex]->m_primaryKeyManager_L.GetKey(&nItemData);
	KGLOG_PROCESS_ERROR(nRetCode);

	m_treeCtrlObjectList.SetItemData(hNewTreeItem, nItemData);

	nRetCode = m_arraySubEditer[m_nCurSubEditerIndex]->m_pGameLogicIniFile->GetString(
		_T("Main"), _T("PrimaryKey_Field"), _T(""),
		szPrimaryKeyField, sizeof(szPrimaryKeyField)
	);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = m_arraySubEditer[m_nCurSubEditerIndex]->m_pGameLogicIniFile->GetString(
		_T("Main"), _T("Title_Field"), _T(""),  szTitleField, sizeof(szTitleField)
	);
	KGLOG_PROCESS_ERROR(nRetCode);

	nInsertPos_L = g_GetPosByPrimaryKey(
		m_arraySubEditer[m_nCurSubEditerIndex]->m_pGameLogicTabFile,
		szPrimaryKeyField, nItemData, false
	);

	m_arraySubEditer[m_nCurSubEditerIndex]->m_pGameLogicTabFile->InsertAfter(nInsertPos_L - 1);
	g_CopyOneRow(
		m_arraySubEditer[m_nCurSubEditerIndex]->m_pGameLogicTabFile, 2, nInsertPos_L
	);
	m_arraySubEditer[m_nCurSubEditerIndex]->m_pGameLogicTabFile->WriteInteger(
		nInsertPos_L, szPrimaryKeyField, nItemData
	);
	m_arraySubEditer[m_nCurSubEditerIndex]->m_pGameLogicTabFile->WriteString(
		nInsertPos_L, szTitleField, stringObjectName
	);
	nRetCode = m_treeCtrlObjectList.RefrushClassInfo(hNewTreeItem);
	KGLOG_PROCESS_ERROR(nRetCode);

	nNewRepresentIdFlag = optionDlg.IsNewRepresentId();

	if (nNewRepresentIdFlag)
	{
		nRetCode =
			m_arraySubEditer[m_nCurSubEditerIndex]->m_primaryKeyManager_R.GetKey(&nNewRepresentId);
		KGLOG_PROCESS_ERROR(nRetCode);

		nInsertPos_R = g_GetPosByPrimaryKey(
			m_arraySubEditer[m_nCurSubEditerIndex]->m_pRepresentTabFile,
			_T("RepresentID"), nNewRepresentId, false
		);

		m_arraySubEditer[m_nCurSubEditerIndex]->m_pRepresentTabFile->InsertAfter(nInsertPos_R - 1);
		g_CopyOneRow(
			m_arraySubEditer[m_nCurSubEditerIndex]->m_pRepresentTabFile, 2, nInsertPos_R
		);
		m_arraySubEditer[m_nCurSubEditerIndex]->m_pRepresentTabFile->WriteInteger(
			nInsertPos_R, _T("RepresentID"), nNewRepresentId
		);
		m_arraySubEditer[m_nCurSubEditerIndex]->m_pGameLogicTabFile->WriteInteger(
			nInsertPos_L, _T("RepresentID"), nNewRepresentId
		);

	}
	else
	{
		nNewRepresentId = optionDlg.GetSelRepresentId();
	}

	m_arraySubEditer[m_nCurSubEditerIndex]->m_pGameLogicTabFile->WriteInteger(
		nInsertPos_L, _T("RepresentID"), nNewRepresentId
	);

	m_treeCtrlObjectList.SelectItem(hNewTreeItem);


Exit1:
	nResult = true;
Exit0:
	return nResult;

}


void KG3DObjectEditor::OnCopy()
{
	if (GetFocus() == &m_treeCtrlObjectList)
	{
		HTREEITEM hTreeItem = m_treeCtrlObjectList.GetSelectedItem();
		if (hTreeItem)
			m_treeCtrlObjectList.SendMessage(
				WM_TR_COPY, (WPARAM)hTreeItem, (LPARAM)MOVE_TYPE_COPY
			);
	}
	else if (GetFocus() == m_pPropertyList)
	{

	}
}


void KG3DObjectEditor::OnPase()
{
	int nResult  = false;
	int nRetCode = false;

	if (GetFocus() == &m_treeCtrlObjectList)
	{
		HTREEITEM hTreeItem = m_treeCtrlObjectList.GetSelectedItem();
		if (hTreeItem)
			m_treeCtrlObjectList.SendMessage(WM_TR_STICK, (WPARAM)hTreeItem);
	}
	else if (GetFocus() == m_pPropertyList)
	{

	}

	nResult = true;
//Exit0:
	return;
}


void KG3DObjectEditor::OnCut()
{
	if (GetFocus() == &m_treeCtrlObjectList)
	{
		HTREEITEM hTreeItem = m_treeCtrlObjectList.GetSelectedItem();
		if (hTreeItem)
			m_treeCtrlObjectList.SendMessage(WM_TR_COPY, (WPARAM)hTreeItem, (LPARAM)MOVE_TYPE_CUT);
	}
	else if (GetFocus() == m_pPropertyList)
	{
	}
}


void KG3DObjectEditor::OnDel()
{
	if (GetFocus() == &m_treeCtrlObjectList)
	{
		if (
			::MessageBox(
			this->m_hWnd, "是否要删除", "Message", MB_YESNO
			) == IDYES
		)
		{
			HTREEITEM hTreeItem = m_treeCtrlObjectList.GetSelectedItem();
			if (hTreeItem)
				m_treeCtrlObjectList.SendMessage(WM_TR_DEL, (WPARAM)hTreeItem);
		}
	}
	if (GetFocus() == m_pPropertyList)
	{

	}
}


void KG3DObjectEditor::OnSave()
{
	int nResult  = false;
	int nRetCode = false;

	nRetCode = m_arraySubEditer[m_nCurSubEditerIndex]->Save();
	KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	if (!nResult)
		MessageBox(
			_T("Save Faild!!\t\t   "),  m_arraySubEditer[m_nCurSubEditerIndex]->m_szSubEditerName,
			MB_OK | MB_ICONERROR
		);
	else
		MessageBox(
			_T("Save Successed!!\t\t"), m_arraySubEditer[m_nCurSubEditerIndex]->m_szSubEditerName,
			MB_OK
		);
	return;
}


int  KG3DObjectEditor::Save()
{
	int nResult  = false;
	int nRetCode = false;

	INT_PTR nSubEditerCount = 0;
	INT_PTR nCurSubIndex    = 0;

	nSubEditerCount = m_arraySubEditer.GetCount();
	for (nCurSubIndex = 0; nCurSubIndex < nSubEditerCount; nCurSubIndex++)
	{
		nRetCode = m_arraySubEditer[nCurSubIndex]->Save();
		KGLOG_PROCESS_ERROR(nRetCode);
	}

	nResult = true;
Exit0:
	return nResult;
}


void KG3DObjectEditor::OnTimer(UINT nIDEvent)
{
	int nRetCode  = !IsTopParentActive();
	static int n = true;

	KG_PROCESS_SUCCESS(nRetCode);

	if (n)
	{
		UpdateScene();
		n = false;
	}

	g_pEngineManager->FrameMove();//by dengzhihui Refactor 2006年11月6日
	g_pEngineManager->Render();   //by dengzhihui Refactor 2006年11月6日

Exit1:
	return;
}


void KG3DObjectEditor::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	IKG3DScene* pScene = GetBaseScene(m_pObjectScene);
	g_pEngineManager->SetFocusScene(pScene); //by dengzhihui Refactor 2006年11月6日
	CDialog::OnActivate(nState, pWndOther, bMinimized);
}


int KG3DObjectEditor::OpenDocument(const char pcszPropertyFileName[])
{
	int nResult  = false;
	int nRetCode = false;

	KGLOG_PROCESS_ERROR(pcszPropertyFileName);

	nResult = true;
Exit0:
	return nResult;

}


void KG3DObjectEditor::OnNMRclickObjectPropertyTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	int nResult  = false;
	int nRetCode = false;

	HTREEITEM hTreeSelItem = NULL;
	POINT   pt;
	CMenu*  pMeu;
	GetCursorPos(&pt);
	CPoint point(pt.x, pt.y);

	m_treeCtrlObjectList.ScreenToClient(&point);
	hTreeSelItem = m_treeCtrlObjectList.HitTest(point);
	if (hTreeSelItem)
		m_treeCtrlObjectList.SelectItem(hTreeSelItem);

	pMeu = m_menuPopup.GetSubMenu(0);
	pMeu->TrackPopupMenu(TPM_LEFTALIGN, pt.x, pt.y, this);

	nResult = true;
//Exit0:
	return;
}


void KG3DObjectEditor::OnTvnSelchangedObjectPropertyTree(
	NMHDR *pNMHDR, LRESULT *pResult
)
{
	int  nResult  = false;
	int  nRetCode = false;

	HTREEITEM hSelTreeItem = NULL;
	char szPrimaryKeyField[64];
	*szPrimaryKeyField = '\0';

	int nTreeItemData = 0;
	int	nTreeItemPos  = 0;
	int nRepresentID  = 0;
	int nRepresentPos = 0;

	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	hSelTreeItem  = m_treeCtrlObjectList.GetSelectedItem();
	KG_PROCESS_ERROR(hSelTreeItem);

	nTreeItemData = (int)m_treeCtrlObjectList.GetItemData(hSelTreeItem);

	nRetCode = m_arraySubEditer[m_nCurSubEditerIndex]->m_pGameLogicIniFile->GetString(
		_T("Main"), _T("PrimaryKey_Field"), _T(""),
		szPrimaryKeyField, sizeof(szPrimaryKeyField)
	);
	KGLOG_PROCESS_ERROR(nRetCode);

	nTreeItemPos = g_GetPosByPrimaryKey(
		m_arraySubEditer[m_nCurSubEditerIndex]->m_pGameLogicTabFile,
		szPrimaryKeyField, (int)nTreeItemData
	);
	m_nTabFilePos = nTreeItemPos;

	m_pPropertyList->UpdateProperty(nTreeItemPos);


	m_arraySubEditer[m_nCurSubEditerIndex]->m_pGameLogicTabFile->GetInteger(
		nTreeItemPos, "RepresentID", 0, &nRepresentID
	);
	nRepresentPos = g_GetPosByPrimaryKey(
		m_arraySubEditer[m_nCurSubEditerIndex]->m_pRepresentTabFile,
		"RepresentID", (int)nRepresentID
	);
	m_pControlPanel->Update(nRepresentPos);

	UpdateScene();

	m_treeCtrlObjectList.Invalidate(TRUE);

	nResult = true;
Exit0:
	*pResult = 0;
	return;

}


void KG3DObjectEditor::OnCommandShowControlColumn()
{
	int nRetCode = false;

	CMenu* pMenu = NULL;

	KGLOG_PROCESS_ERROR(m_pControlPanel);

	pMenu = GetMenu()->GetSubMenu(4);

	nRetCode = pMenu->GetMenuState(ID_CONTROL_DLG, MF_BYCOMMAND);

	if (nRetCode == MF_CHECKED)
	{
		pMenu->CheckMenuItem(ID_CONTROL_DLG, MF_UNCHECKED);
		m_pControlPanel->ShowWindow(SW_HIDE);
		m_pControlPanel->SetShowMode(SHOW_MODEL_HIDE);
	}
	else
	{
		pMenu->CheckMenuItem(ID_CONTROL_DLG, MF_CHECKED);
		m_pControlPanel->ShowWindow(SW_SHOW);
		m_pControlPanel->SetShowMode(SHOW_MODEL_BIND);
	}

	UpdateInterface();

Exit0:
	return;
}


void KG3DObjectEditor::OnCommandShowListWindow()
{
	int nRetCode = false;

	CMenu* pMenu = NULL;

	KGLOG_PROCESS_ERROR(m_pPropertyList);

	pMenu = GetMenu()->GetSubMenu(4);

	nRetCode = pMenu->GetMenuState(ID_PROPERTY_DLG, MF_BYCOMMAND);

	if (nRetCode == MF_CHECKED)
	{
		pMenu->CheckMenuItem(ID_PROPERTY_DLG, MF_UNCHECKED);
		m_pPropertyList->ShowWindow(SW_HIDE);
		m_pPropertyList->SetShowMode(SHOW_MODEL_HIDE);
	}
	else
	{
		pMenu->CheckMenuItem(ID_PROPERTY_DLG, MF_CHECKED);
		m_pPropertyList->ShowWindow(SW_SHOW);
		m_pPropertyList->SetShowMode(SHOW_MODEL_BIND);
	}

	UpdateInterface();

Exit0:
	return;
}


void KG3DObjectEditor::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
	int nRetCode = false;

	CDialog::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);

	KGLOG_PROCESS_ERROR(m_pPropertyList);
	KGLOG_PROCESS_ERROR(m_pControlPanel);

	nRetCode = m_pPropertyList->GetShowMode();
	if (nRetCode == SHOW_MODEL_HIDE)
	{
		pPopupMenu->CheckMenuItem(ID_PROPERTY_DLG, MF_UNCHECKED);
	}
	else
	{
		pPopupMenu->CheckMenuItem(ID_PROPERTY_DLG, MF_CHECKED);
	}

	nRetCode = m_pControlPanel->GetShowMode();

	if (nRetCode == SHOW_MODEL_HIDE)
	{
		pPopupMenu->CheckMenuItem(ID_CONTROL_DLG, MF_UNCHECKED);
	}
	else
	{
		pPopupMenu->CheckMenuItem(ID_CONTROL_DLG, MF_CHECKED);
	}

Exit0:
	return;
}


void KG3DObjectEditor::OnReName()
{
	m_treeCtrlObjectList.EditLabel(m_treeCtrlObjectList.GetSelectedItem());
}


BOOL KG3DObjectEditor::OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult)
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


BOOL KG3DObjectEditor::PreTranslateMessage(MSG* pMsg)
{
	POINT point;

	GetCursorPos(&point);
	ScreenToClient(&point);

	switch (pMsg->message)
	{
	case WM_LBUTTONDOWN :
	case WM_LBUTTONUP   :
	case WM_MBUTTONDOWN :
	case WM_MBUTTONUP   :
	case WM_MOUSEWHEEL  :
	case WM_MOUSEMOVE   :
	case WM_KEYDOWN     :
	case WM_KEYUP       :
	case WM_CHAR        :
		if (m_rectObjView.PtInRect(point))
		{
			m_wndObjectView.SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
			if (
				pMsg->message == WM_CHAR    ||
				pMsg->message == WM_KEYDOWN ||
				pMsg->message == WM_KEYDOWN
			)
			{
				return TRUE;
			}
		}
		break;
	default :
		break;
	}

	if ((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_ESCAPE))
	{
		return TRUE;
	}

	/*
	if(m_hAccel)
	{
		if (::TranslateAccelerator(GetSafeHwnd(), m_hAccel, pMsg))
			return TRUE;
	}*/

	return CDialog::PreTranslateMessage(pMsg);
}


void KG3DObjectEditor::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_nLButtonFlag = true;

	if ((point.x >= m_rectTabCtrl.right) && (point.x <= m_rectObjView.left))
	{
		m_nLSizeBarFlag = true;
	}

	if (
		(m_pPropertyList->GetShowMode() == SHOW_MODEL_BIND) &&
		(point.x >= m_rectObjView.right) &&
		(point.x <= m_rectListCtrl.left)
	)
	{
		m_nRSizeBarFlag = true;
	}

	SetCapture();
}


void KG3DObjectEditor::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_nLButtonFlag  = false;
	m_nLSizeBarFlag = false;
	m_nRSizeBarFlag = false;

	HCURSOR hCr = LoadCursor(NULL, IDC_ARROW);
	SetClassLongPtr(this->m_hWnd, GCL_HCURSOR, (LONG)(LONG_PTR)(hCr));

	ReleaseCapture();

}


void KG3DObjectEditor::OnMouseMove(UINT nFlags, CPoint point)
{
	CRect rectTreeCtrl;
	CRect rectTabItem;
	HCURSOR hCr = NULL;

	if (
		((point.x >= m_rectTabCtrl.right) && (point.x <= m_rectObjView.left)) ||
		((point.x >= m_rectObjView.right) && (point.x <= m_rectListCtrl.left) && m_pPropertyList->GetShowMode() == SHOW_MODEL_BIND)
	)
	{
		hCr = LoadCursor(NULL, IDC_SIZEWE);
		SetClassLongPtr(this->m_hWnd, GCL_HCURSOR, (LONG)(LONG_PTR)hCr);
	}
	else
	{
		hCr = LoadCursor(NULL, IDC_ARROW);
		SetClassLongPtr(this->m_hWnd, GCL_HCURSOR, (LONG)(LONG_PTR)hCr);
	}

	if (m_nLButtonFlag && m_nLSizeBarFlag)
	{
		m_rectTabCtrl.right = point.x;
		m_rectObjView.left  = point.x + 2;

		UpdateInterface();
	}

	if (m_nLButtonFlag && m_nRSizeBarFlag)
	{
		m_rectObjView.right = point.x;
		m_rectListCtrl.left = point.x + 2;

		UpdateInterface();
	}
}


void KG3DObjectEditor::OnMove(int x, int y)
{
	if (m_pControlPanel && m_pControlPanel->GetShowMode() == SHOW_MODEL_BIND)
	{
		UpdateInterface();
	}
	else if (m_pPropertyList && m_pPropertyList->GetShowMode() == SHOW_MODEL_BIND)
	{
		UpdateInterface();
	}
	else
	{
		CDialog::OnMove(x, y);
	}
}


LRESULT KG3DObjectEditor::OnListItemUpdate(WPARAM wParam, LPARAM lParam)
{
	int nResult  = false;
	int nRetCode = false;

	LPKGLISTITEM pListItem  = (LPKGLISTITEM)wParam;
	USER_ITEM_DATA itemData;
	HTREEITEM hSelTreeItem  = m_treeCtrlObjectList.GetSelectedItem();

	KGLOG_PROCESS_ERROR(hSelTreeItem);
	KGLOG_PROCESS_ERROR(pListItem);

	pListItem->GetStructData(&itemData, sizeof(itemData));

	if (!strcmp(itemData.szPropertyName, _T("Name")))
	{
		m_treeCtrlObjectList.SetItemText(hSelTreeItem, itemData.szPropertyValue);
	}

	nResult = true;
Exit0:
	return nResult;
}


LRESULT KG3DObjectEditor::OnUpdateScene(WPARAM wParam, LPARAM lParam)
{
	UpdateScene();
	return true;
}


int  KG3DObjectEditor::UpdateScene()
{
	HRESULT hr = E_FAIL;
	int nResult  = false;
	int nRetCode = false;

	HTREEITEM hSelTreeItem = NULL;

	char szPrimaryKeyField[64];
	TCHAR szModelFullPath[MAX_PATH];
	TCHAR szModelFile[MAX_PATH];
	TCHAR strDriver[MAX_PATH];
	TCHAR strPath[MAX_PATH];
	TCHAR strFileName[MAX_PATH];
	TCHAR strAniBaseName[MAX_PATH];
//	TCHAR strAniFilePath[MAX_PATH];

	IEKG3DModel* pModel = NULL;
	*szPrimaryKeyField = '\0';
	*szModelFullPath = '\0';
	*szModelFile = '\0';

	int nTreeItemData = 0;
	int	nTreeItemPos   = 0;
	int	nLogicID   = 0;

	hSelTreeItem  = m_treeCtrlObjectList.GetSelectedItem();
	KGLOG_PROCESS_ERROR(hSelTreeItem);

	nTreeItemData = (int)m_treeCtrlObjectList.GetItemData(hSelTreeItem);
	nRetCode = m_arraySubEditer[m_nCurSubEditerIndex]->m_pGameLogicTabFile->GetInteger(
		m_nTabFilePos, _T("ID"), 0, &nLogicID
	);

	/*
	nRetCode = m_arraySubEditer[m_nCurSubEditerIndex]->m_pIniAssitFile->GetString(
		_T("Main"), _T("PrimaryKey_Field"), _T(""),
		szPrimaryKeyField, sizeof(szPrimaryKeyField)
	);
	KGLOG_PROCESS_ERROR(nRetCode);

	nTreeItemPos = g_GetPosByPrimaryKey(
		m_arraySubEditer[m_nCurSubEditerIndex]->m_pUniteTabFile, szPrimaryKeyField,
		nTreeItemData
	);
*/
	//nRetCode = m_arraySubEditer[m_nCurSubEditerIndex]->m_pUniteTabFile->GetString(
	//	nTreeItemPos, _T("ModelFile"), _T(""), szModelFile, sizeof(szModelFile)
	//);
	//KGLOG_PROCESS_ERROR(nRetCode);


	m_pObjectScene->OpenObject(nLogicID);
	m_pControlPanel->m_comboxModList.SetCurSel(0);
	m_pControlPanel->OnCbnModSelect();

	m_pControlPanel->m_comboxAniList.Clear();

	szModelFullPath[0] = '\0';

	IEKG3DRepresentObject* pObj = NULL;
	m_pObjectScene->GetRepresentObject(&pObj);
	KGLOG_PROCESS_ERROR(pObj);
	if (pObj)
	{
		//hr = pObj->GetModel(pModel, 0);
		//if (SUCCEEDED(hr) && pModel)
		TCHAR szName[MAX_PATH];
		hr = pObj->GetName(szName, _countof(szName));//by dengzhihui Refactor 2006年11月15日
		if (SUCCEEDED(hr))
		{
			strcpy_s(szModelFullPath, MAX_PATH, szName);//pModel->m_scName.c_str());//by dengzhihui Refactor 2006年11月15日

			_splitpath_s(szModelFullPath,
				strDriver,
				MAX_PATH,
				strPath,
				MAX_PATH,
				strFileName,
				MAX_PATH,
				NULL,
				0);

			sprintf_s(strAniBaseName,
				MAX_PATH,
				"%s%s%s",
				strDriver,
				strPath,
				strFileName);

			KG_PROCESS_ERROR(m_arraySubEditer[m_nCurSubEditerIndex]);
			//nRetCode = m_arraySubEditer[m_nCurSubEditerIndex]->m_pRepresentIniFile->GetString
		}
	}


	//g_GetFullPath(szModelFullPath, szModelFile);
	//m_pObjectScene->OpenObject(szModelFullPath);

	/*
	IEKG3DModel* pModel = NULL;
	m_pObjectScene->m_lpRepresentObject->GetModel(pModel, 0);
	pModel->PlayAnimation(
		PLAY_CIRCLE, "\\模型文件\\剑三M2 动作\\主角\\男主角\\M3man.三段击.Ani", 0.4f, 0, NULL
	);
	*/
	nResult = true;
Exit0:
	return nResult;
}


void KG3DObjectEditor::OnSize(UINT nType, int cx, int cy)
{
	int nResult  = false;
	int nRetCode = false;

	int nTitleBarHeight = 0;

	DWORD dwStyle = 0;
	CRect rectMainFrame;
	CRect rectTreeCtrl;
	CRect rectTabItem;

	nTitleBarHeight = GetSystemMetrics(SM_CYCAPTION);
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);

	GetClientRect(&rectMainFrame);
	m_tabCtrlClassify.GetItemRect(0, &rectTabItem);

	m_rectTabCtrl.left	  = rectMainFrame.left;
	m_rectTabCtrl.top     = m_rectToolBar.Height() + 2;
	m_rectTabCtrl.bottom  = rectMainFrame.bottom - (m_rectStateBar.Height() + 2);

	if (m_pPropertyList->GetShowMode() == SHOW_MODEL_BIND)
	{
		m_rectListCtrl.left   = rectMainFrame.right - m_rectListCtrl.Width();
		m_rectListCtrl.right  = rectMainFrame.right;
		m_rectListCtrl.top    = m_rectToolBar.Height() + 2;
		m_rectListCtrl.bottom = rectMainFrame.bottom - (m_rectStateBar.Height() + 2);
	}

	if (m_pControlPanel->GetShowMode() == SHOW_MODEL_BIND)
	{
		m_rectControl.left    = m_rectTabCtrl.right + 2;
		m_rectControl.bottom  = rectMainFrame.bottom - (m_rectStateBar.Height() + 2);
		m_rectControl.top     = m_rectControl.bottom - 120;

		if (m_pPropertyList->GetShowMode() == SHOW_MODEL_BIND)
		{

			m_rectControl.right   = m_rectListCtrl.left - 2;
		}
		else
		{
			m_rectControl.right  = rectMainFrame.right;
		}
	}

	m_rectObjView.left    = m_rectTabCtrl.right + 2;
	m_rectObjView.top     = m_rectToolBar.Height() + 2;

	if (m_pPropertyList->GetShowMode() == SHOW_MODEL_BIND)
	{
		m_rectObjView.right = m_rectListCtrl.left - 2;
	}
	else
	{
		m_rectObjView.right = rectMainFrame.right;
	}

	if (m_pControlPanel->GetShowMode() == SHOW_MODEL_BIND)
	{
		m_rectObjView.bottom = m_rectControl.top - 2;
	}
	else
	{
		m_rectObjView.bottom = rectMainFrame.bottom - m_rectStateBar.Height();
	}

	m_tabCtrlClassify.GetClientRect(&rectTreeCtrl);
	rectTreeCtrl.top = rectTabItem.bottom + 2;

	m_tabCtrlClassify.MoveWindow(&m_rectTabCtrl);
	m_wndObjectView.MoveWindow(&m_rectObjView);
	m_treeCtrlObjectList.MoveWindow(&rectTreeCtrl);

	if (m_pPropertyList->GetShowMode() == SHOW_MODEL_BIND)
	{
		ClientToScreen(&m_rectListCtrl);
		m_pPropertyList->ModifyStyle(WS_CAPTION | WS_SIZEBOX, 0);
		SetWindowLong(m_pPropertyList->m_hWnd, GWL_EXSTYLE, 0);
		m_pPropertyList->MoveWindow(&m_rectListCtrl);
		ScreenToClient(&m_rectListCtrl);
	}
	else
	{
		dwStyle = GetWindowLong(m_pPropertyList->m_hWnd, GWL_STYLE);
		if (!(dwStyle & WS_CAPTION))
		{
			m_rectListCtrl.top = m_rectListCtrl.top - nTitleBarHeight;
			m_pPropertyList->ModifyStyle(0, WS_CAPTION | WS_SIZEBOX);
			ClientToScreen(&m_rectListCtrl);
			m_pPropertyList->MoveWindow(&m_rectListCtrl);
			ScreenToClient(&m_rectListCtrl);
		}
	}

	if (m_pControlPanel->GetShowMode() == SHOW_MODEL_BIND)
	{
		ClientToScreen(&m_rectControl);
		m_pControlPanel->ModifyStyle(WS_CAPTION | WS_SIZEBOX, 0);
		SetWindowLong(m_pControlPanel->m_hWnd, GWL_EXSTYLE, 0);
		m_pControlPanel->MoveWindow(&m_rectControl);
		ScreenToClient(&m_rectControl);
	}
	else
	{
		dwStyle = GetWindowLong(m_pControlPanel->m_hWnd, GWL_STYLE);
		if (!(dwStyle & WS_CAPTION))
		{
			m_rectControl.top = m_rectControl.top - nTitleBarHeight;
			m_rectControl.bottom = m_rectControl.bottom + 5;
			m_pControlPanel->ModifyStyle(0, WS_CAPTION | WS_SIZEBOX);
			ClientToScreen(&m_rectControl);
			m_pControlPanel->MoveWindow(&m_rectControl);
			ScreenToClient(&m_rectControl);
		}
	}

	if (m_pObjectScene)
	{
		IKG3DScene* pScene = GetBaseScene(m_pObjectScene);//by dengzhihui Refactor 2006年11月15日
		pScene->OnResizeWindow(NULL);
	}

	nResult = true;
//Exit0:
	return;
}


int KG3DObjectEditor::UpdateSubEditer()
{
	int nResult  = false;
	int nRetCode = false;

	m_nCurSubEditerIndex = m_tabCtrlClassify.GetCurSel();
	m_treeCtrlObjectList.Update(m_arraySubEditer[m_nCurSubEditerIndex]);

	nResult = true;
//Exit0:
	return nResult;
}


void KG3DObjectEditor::PostNcDestroy()
{
	CDialog::PostNcDestroy();
	delete this;
}


void KG3DObjectEditor::OnCancel()
{
	DestroyWindow();
	return;
}

void KG3DObjectEditor::OnOK()
{
	return;
}


HBRUSH KG3DObjectEditor::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	if (pWnd == this)
	{
		return CreateSolidBrush(RGB(68, 68, 68));
	}
	if (pWnd == &m_wndObjectView)
	{
		return CreateSolidBrush(RGB(2, 2, 2));
	}
	return hbr;
}


KGSubEditer::KGSubEditer()
{
	*m_szTabFilePath_Represent = '\0';
	*m_szTabFilePath_GameLogic = '\0';
	*m_szIniFilePath_Represent = '\0';
	*m_szIniFilePath_GameLogic = '\0';
	*m_szClassifyFilePath      = '\0';
	*m_szSubEditerName		   = '\0';

	m_pRepresentTabFile		   = NULL;
	m_pGameLogicTabFile		   = NULL;
	m_pRepresentIniFile		   = NULL;
	m_pGameLogicIniFile        = NULL;
	m_pClassifyFile			   = NULL;

}


KGSubEditer::~KGSubEditer()
{
	SAFE_RELEASE(m_pClassifyFile)
	SAFE_RELEASE(m_pRepresentTabFile);
	SAFE_RELEASE(m_pGameLogicTabFile);;
	SAFE_RELEASE(m_pRepresentIniFile);
	SAFE_RELEASE(m_pGameLogicIniFile);

}


int KGSubEditer::CheckConsistency()
{
	int nResult  = false;
	int nRetCode = false;

	nResult = true;
//Exit0:
	return nResult;
}


int KGSubEditer::IsValidate()
{
	int nResult  = false;
	int nRetCode = false;

	KGLOG_PROCESS_ERROR(m_pClassifyFile);
	KGLOG_PROCESS_ERROR(m_pGameLogicIniFile);
	KGLOG_PROCESS_ERROR(m_pRepresentIniFile);
	KGLOG_PROCESS_ERROR(m_pGameLogicTabFile);
	KGLOG_PROCESS_ERROR(m_pRepresentTabFile);;

	nResult = true;
Exit0:
	return nResult;
}


int KGSubEditer::UniteTabFile()
{
	int nResult  = false;
	int nRetCode = false;
/*
	char szFieldName[64];
	char szFieldValue[1024];

	*szFieldName  = '\0';
	*szFieldValue = '\0';

	int nColsCount = 0;
	int nRowsCount = 0;
	int nCurCols   = 0;
	int nCurRows   = 0;

	KGLOG_PROCESS_ERROR(m_pRepresentTabFile);
	KGLOG_PROCESS_ERROR(m_pGameLogicTabFile);

	SAFE_RELEASE(m_pUniteTabFile);
	m_pUniteTabFile = g_CreateTabFile();
	KGLOG_PROCESS_ERROR(m_pUniteTabFile);

	nColsCount = m_pRepresentTabFile->GetWidth();
	for (nCurCols = 1; nCurCols <= nColsCount; nCurCols++)
	{
		strncpy(
			szFieldName, m_pGameLogicTabFile->GetColName(nCurCols),
			sizeof(szFieldName)
		);
		KGLOG_PROCESS_ERROR(*szFieldName != '\0');
		nRetCode = m_pUniteTabFile->FindColumn(szFieldName);
		if (nRetCode == -1)
			m_pUniteTabFile->InsertNewCol(szFieldName);
	}

	nColsCount = m_pGameLogicTabFile->GetWidth();
	for (nCurCols = 1; nCurCols <= nColsCount; nCurCols++)
	{
		strncpy(
			szFieldName, m_pRepresentTabFile->GetColName(nCurCols),
			sizeof(szFieldName)
		);
		KGLOG_PROCESS_ERROR(*szFieldName != '\0');
		nRetCode = m_pUniteTabFile->FindColumn(szFieldName);
		if (nRetCode == -1)
			m_pUniteTabFile->InsertNewCol(szFieldName);
	}

	nRowsCount = m_pGameLogicTabFile->GetHeight();
	KGLOG_PROCESS_ERROR(nRowsCount > 1);

	nColsCount = m_pGameLogicTabFile->GetWidth();
	for (nCurRows = 2; nCurRows <= nRowsCount; nCurRows++)
	{
		for (nCurCols = 1; nCurCols <= nColsCount; nColsCount)
		{
			strncpy(
				szFieldName, m_pGameLogicTabFile->GetColName(nCurCols),
				sizeof(szFieldName)
			);
			m_pGameLogicTabFile->GetString(
				nCurRows, szFieldName, _T(""), szFieldValue, sizeof(szFieldValue)
			);
			m_pUniteTabFile->WriteString(nCurRows, szFieldName, szFieldValue);
		}
	}
*/
	nResult = true;
//Exit0:
	return nResult;
}


int KGSubEditer::Save(const char pcszFileName[] /* = NULL */)
{
	int nResult  = false;
	int nRetCode = false;
	int nIndex   = 0;

	CString szTempFileName = _T("");

	KGLOG_PROCESS_ERROR(m_pClassifyFile);
	KGLOG_PROCESS_ERROR(m_pGameLogicIniFile);
	KGLOG_PROCESS_ERROR(m_pRepresentIniFile);
	KGLOG_PROCESS_ERROR(m_pGameLogicTabFile);
	KGLOG_PROCESS_ERROR(m_pRepresentTabFile);

	nRetCode = CheckConsistency();
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = m_pClassifyFile->Save(m_szClassifyFilePath);
	KGLOG_PROCESS_ERROR(nRetCode);
	SAFE_RELEASE(m_pClassifyFile);
	m_pClassifyFile = g_OpenIniFile(m_szClassifyFilePath, false, true);
	KGLOG_PROCESS_ERROR(m_pClassifyFile);

	nRetCode = m_pRepresentIniFile->Save(m_szIniFilePath_Represent);
	KGLOG_PROCESS_ERROR(nRetCode);
	SAFE_RELEASE(m_pRepresentIniFile);
	m_pRepresentIniFile = g_OpenIniFile(m_szIniFilePath_Represent, false, true);
	KGLOG_PROCESS_ERROR(m_pRepresentIniFile);

	nRetCode = m_pGameLogicIniFile->Save(m_szIniFilePath_GameLogic);
	KGLOG_PROCESS_ERROR(nRetCode);
	SAFE_RELEASE(m_pGameLogicIniFile);
	m_pGameLogicIniFile = g_OpenIniFile(m_szIniFilePath_GameLogic, false, true);
	KGLOG_PROCESS_ERROR(m_pGameLogicIniFile);

	nRetCode = m_pRepresentTabFile->Save(m_szTabFilePath_Represent);
	KGLOG_PROCESS_ERROR(nRetCode);
	SAFE_RELEASE(m_pRepresentTabFile);
	m_pRepresentTabFile = g_OpenTabFile(m_szTabFilePath_Represent, false, true);
	KGLOG_PROCESS_ERROR(m_pRepresentTabFile);

	nRetCode = m_pGameLogicTabFile->Save(m_szTabFilePath_GameLogic);
	KGLOG_PROCESS_ERROR(nRetCode);
	SAFE_RELEASE(m_pGameLogicTabFile);
	m_pGameLogicTabFile = g_OpenTabFile(m_szTabFilePath_GameLogic, false, true);
	KGLOG_PROCESS_ERROR(m_pGameLogicTabFile);


	nResult = true;
Exit0:
	return nResult;
}


void KG3DObjectEditor::OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	m_nCurSubEditerIndex = m_tabCtrlClassify.GetCurSel();
	m_pPropertyList->BandData(
		&m_arraySubEditer[m_nCurSubEditerIndex]->m_pGameLogicTabFile,
		&m_arraySubEditer[m_nCurSubEditerIndex]->m_pGameLogicIniFile,
		&m_arraySubEditer[m_nCurSubEditerIndex]->m_pRepresentTabFile,
		&m_arraySubEditer[m_nCurSubEditerIndex]->m_pRepresentIniFile
	);

	m_pControlPanel->BandData(
		&m_arraySubEditer[m_nCurSubEditerIndex]->m_pRepresentTabFile,
		&m_arraySubEditer[m_nCurSubEditerIndex]->m_pRepresentIniFile
	);

	m_pObjectScene->BandObjectTemplate(
		m_arraySubEditer[m_nCurSubEditerIndex]->m_pRepresentTabFile,
		m_arraySubEditer[m_nCurSubEditerIndex]->m_pGameLogicTabFile
	);

	UpdateSubEditer();

//Exit0:
	*pResult = 0;
}


IMPLEMENT_DYNAMIC(KG3DViewWindow, CWnd)
BEGIN_MESSAGE_MAP(KG3DViewWindow, CWnd)
END_MESSAGE_MAP()


BOOL KG3DViewWindow::PreTranslateMessage(MSG* pMsg)
{

	switch (pMsg->message)
	{
	case WM_LBUTTONDOWN :
	case WM_LBUTTONUP   :
	case WM_MBUTTONDOWN :
	case WM_MBUTTONUP   :
	case WM_MOUSEWHEEL  :
	case WM_MOUSEMOVE   :
	case WM_KEYDOWN     :
	case WM_KEYUP       :
	case WM_CHAR        :
		SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
		break;
	default :
		break;
	}

	return CWnd::PreTranslateMessage(pMsg);
}


LRESULT KG3DViewWindow::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_LBUTTONDOWN :
	case WM_LBUTTONUP   :
		SetFocus();
	case WM_MBUTTONDOWN :
	case WM_MBUTTONUP   :
	case WM_MOUSEWHEEL  :
	case WM_MOUSEMOVE   :
	case WM_KEYDOWN     :
	case WM_KEYUP       :
	case WM_CHAR        :
			g_pEngineManager->ProcessMessage(message, wParam, lParam);//<PROCESS_MESSAGE>
		break;
	default :
		break;
	}
	return CWnd::WindowProc(message, wParam, lParam);
}