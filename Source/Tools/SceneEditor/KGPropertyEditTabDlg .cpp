////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGPropertyEditTabDlg .cpp
//  Version     : 1.0
//  Author      : ZhaoXinyu (Chiyer)
//  CopyRight   : Kingsoft Corporation All rights Reserved
//  History     : 2005-11-29 10:18:28
//  Comment     :
//                @""@         (\~~/)
//               (=^.^=)      (='.'= )
//             o~(__)_)        (") (")~~O
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SceneEditor.h"
#include "KGPropertyEditTabDlg.h"
#include "KG3dsceneObjectEditor.h"
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


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(KGObjectPropertyEditTabDlg, CDialog)

KGObjectPropertyEditTabDlg::KGObjectPropertyEditTabDlg(
	PROPERTY_EDIT_TYPE eType,
	KSceneObjectEditorPanelView* pView,
	CWnd* pParent
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
	DDX_Text(pDX, IDC_BOOK, m_szBookMark);
	DDX_Control(pDX, IDC_FRAME, m_iniWinidow);
	//DDX_Control(pDX, IDC_SAVE, m_bnSave);
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
	ON_MESSAGE(WM_USER + 2, OnListEnter)
	ON_WM_INITMENUPOPUP()
	ON_COMMAND(ID_OPEN, OnOpen)
	ON_COMMAND(ID_NEW,  OnNew)

	ON_COMMAND(ID_GLEAN, OnCreateGln)
	ON_COMMAND(ID_KILL,  OnCreateKil)
	ON_COMMAND(ID_POS,   OnCreatePos)
	ON_COMMAND(ID_TALK,  OnCreateTlk)
	ON_COMMAND(ID_OFFER, OnCreateOfr)
	ON_COMMAND(ID_OTHER, OnCreateOtr)

	ON_COMMAND(ID_SAVE,    OnSave)
	ON_COMMAND(ID_SAVE_AS, OnSaveAs)

	ON_COMMAND(ID_FIND,  OnFind)
	ON_COMMAND(ID_DEL,   OnDel)
	ON_COMMAND(ID_COPY,  OnCopy)
	ON_COMMAND(ID_CUT,   OnCut)
	ON_COMMAND(ID_PASE,  OnPase)

	ON_COMMAND(ID_MAP,   OnCreateMap)
	ON_COMMAND(ID_LINK,  OnCreateLnk)
	ON_COMMAND(ID_SUB,   OnCreateSub)

	ON_COMMAND(ID_CLOSE, OnClose)
	ON_COMMAND(ID_SET,   OnSet)
	ON_COMMAND(ID_HELP,  OnHellp)
	ON_COMMAND(ID_SH_OPEN, OnQuestSwitchOpen)
	ON_COMMAND(ID_SH_CLOSE, OnQuestSwintcClose)

	ON_COMMAND(ID_OUT,    OnQuestPop)
	ON_COMMAND(ID_IN,     OnQuestIn)
	ON_COMMAND(ID_OUT_ALL,OnQuestPopAll)
	//ON_COMMAND(ID_FIND, OnFind)
	ON_COMMAND(ID_EXPENDALL, OnExpendAll)
	ON_COMMAND(ID_SHRINKALL, OnShrinkAll)
	//ON_NOTIFY(LVN_KEYDOWN, IDC_TREE_PROPERTY, OnLvnKeydownTree)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXT, 0, 0xFFFF, OnToolTipText)
	ON_CBN_SELCHANGE(IDC_COMBO_TYPE, OnCbnSelect)
	ON_WM_SHOWWINDOW()
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
	*szNamelpatePath = '\0';

	int* nType = NULL;
	CBitmap m_bmp;
	m_szCurBookMarkName = _T("");
	m_szPreBookMarkName = _T("");

	m_hTreeRootItem     = NULL;
	m_hTreeSelItem      = NULL;
	m_nEditerState      = EDIT_STATE_CLOSE;
	m_nBeSavedFlag		= true;

	m_nLButtonFlag = false;
	m_nSizeBarFlag = false;

	m_szBookScenceName  = "";

	//
	// Create The ToolBar
	//

	m_hAccel = LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_OBJECT_PROPERTY_EDIT_DLG));
	m_menuPopup.LoadMenu(IDR_QUEST);

	nRetCode = m_wndToolBar.CreateEx(
		this, TBSTYLE_BUTTON, WS_CHILD | WS_VISIBLE | CBRS_TOP |
		CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC
	);
	KG_PROCESS_ERROR(nRetCode);
	nRetCode = m_wndToolBar.LoadToolBar(IDR_OBJECT_EDIT);

	/*

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_OBJECT_EDIT))
	{
		//TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	*/

	m_bmp.Attach(::LoadImage(AfxGetResourceHandle(), "QUEST_TOOL_BAR", IMAGE_BITMAP, 0, 0,LR_DEFAULTSIZE|LR_CREATEDIBSECTION));
	m_NormalList.Create(18, 18, ILC_COLOR24|ILC_MASK, 0, 0);
	m_NormalList.Add(&m_bmp,RGB(192,192,192)) ;
	m_bmp.Detach();
	m_wndToolBar.SendMessage(TB_SETIMAGELIST,0,(LPARAM)m_NormalList.m_hImageList);

	m_bmp.Attach(::LoadImage(AfxGetResourceHandle(), "QUEST_TOOL_BAR",IMAGE_BITMAP, 0, 0,LR_DEFAULTSIZE|LR_CREATEDIBSECTION));
	m_HotList.Create(18, 18, ILC_COLOR24|ILC_MASK, 0, 0);
	m_HotList.Add(&m_bmp,RGB(192,192,192)) ;
	m_bmp.Detach();
	m_wndToolBar.SendMessage(TB_SETHOTIMAGELIST,0,(LPARAM)m_HotList.m_hImageList);

	m_bmp.Attach(::LoadImage(AfxGetResourceHandle(), "QUEST_TOOL_BAR",IMAGE_BITMAP, 0, 0,LR_DEFAULTSIZE|LR_CREATEDIBSECTION));
	m_DisableList.Create(18, 18, ILC_COLOR24|ILC_MASK, 0, 0);
	m_DisableList.Add(&m_bmp,RGB(192,192,192)) ;
	m_bmp.Detach();
	m_wndToolBar.SendMessage(TB_SETDISABLEDIMAGELIST,0,(LPARAM)m_DisableList.m_hImageList);

	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);

	static UINT indicators[] =
	{
		ID_SEPARATOR,
		ID_SEPARATOR,
		ID_SEPARATOR,
		ID_SEPARATOR
	};

	nRetCode = m_wndStateBar.Create(this);  //, WS_CHILD | WS_VISIBLE | CBRS_BOTTOM)
	KG_PROCESS_ERROR(nRetCode);
	nRetCode = m_wndStateBar.SetIndicators(
		indicators, sizeof(indicators) / sizeof(UINT)
	);
	KG_PROCESS_ERROR(nRetCode);

	m_wndStateBar.SetPaneInfo(0, ID_SEPARATOR, SBPS_NOBORDERS | SBPS_STRETCH, 0);
	m_wndStateBar.SetPaneInfo(1, ID_SEPARATOR, SBPS_NORMAL, 200);
	m_wndStateBar.SetPaneInfo(2, ID_SEPARATOR, SBPS_NORMAL, 100);
	m_wndStateBar.SetPaneInfo(3, ID_SEPARATOR, SBPS_NORMAL, 100);

	m_wndStateBar.SetPaneText(0, _T("就绪 >_<"));
	m_wndStateBar.SetPaneText(2, _T("任务个数:"));
	m_wndStateBar.SetPaneText(3, _T("属性条数:"));

	m_proessBar.SetParent(&m_wndStateBar);

	//////////////////////////////////////////////////////////////////////////
	//{
	//CRect rectProcess;
	//CStatusBarCtrl* pStatusBarCtrl = &m_wndStateBar.GetStatusBarCtrl();
	//pStatusBarCtrl->GetRect(1, &rectProcess);
	//m_proessBar.MoveWindow(rectProcess);
	//}

	//
	// Init The Control Style
	//

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
	m_imageList.Add(AfxGetApp()->LoadIcon(IDC_POS));
	m_imageList.Add(AfxGetApp()->LoadIcon(IDC_KILL));
	m_imageList.Add(AfxGetApp()->LoadIcon(IDC_TALK));
	m_imageList.Add(AfxGetApp()->LoadIcon(IDC_OFFER));
	m_imageList.Add(AfxGetApp()->LoadIcon(IDC_OTHER));


	//m_treeObjectView.SetImageList(&m_imageList, TVSIL_NORMAL);

	dwStyle = GetWindowLong(m_hWnd, GWL_STYLE);
	dwStyle |= WS_SIZEBOX;
	SetWindowLong(m_hWnd, GWL_STYLE, dwStyle);

	m_listProperty.InsertColumn(0, "物件属性名");
	m_listProperty.InsertColumn(1, "物件属性值");
	m_listProperty.SetColumnWidth(0, 212);
	m_listProperty.SetColumnWidth(1, 662);
	m_listProperty.SetBkColor(RGB(92, 92, 92));

	m_RectSave.cx = 248;
	m_RectSave.cy = 250;
	m_rectTree.right = m_RectSave.cx;
	m_rectList.left  = m_RectSave.cy;

	switch (m_eEditType)
	{
	case PROPERTY_EDIT_TYPE_TASE :
		g_GetFullPath(szNamelpatePath, QUEST_NAMEPLATE_PATH);
		m_szNamePlateFileName = szNamelpatePath;
		m_szPropertySaveFile  = _T("__Quest__.tab");
		//////////////////////////////////////////////////////////////////////////
		nType = new int[2];
		nType[0] = 1;
		nType[1] = 2;
		m_treeObjectView.BandInfo(
			&m_pTabFile, &m_pQuestIniFile, &m_pIniFile,
			nType, 2, 4, 3, &m_imageList, GetImageIndex
		);
		//////////////////////////////////////////////////////////////////////////
		break;
	default :
		m_szNamePlateFileName = "";
		KG_PROCESS_ERROR(false);
	}

	m_listProperty.BandInfo(&m_pTabFile, &m_pIniFile);
	m_pIniFile = g_OpenIniFile(m_szNamePlateFileName.GetBuffer());
	KG_PROCESS_ERROR(m_pIniFile);

	UpdateGrid(true);
	//UpdateGrid(true);

	nResult = true;
Exit0:
	if (!m_pIniFile)
	{
		::MessageBox(
			this->m_hWnd,_T("找不到相应的INI配置文件!"), _T("Init Error"), MB_OK
		);
	}
    return nResult;
}


BOOL KGObjectPropertyEditTabDlg::PreTranslateMessage(MSG* pMsg)
{
	if(m_hAccel)
	{
		if(::TranslateAccelerator(GetSafeHwnd(), m_hAccel, pMsg))
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
		nRow, _T("QuestType"), "", szQuestType, sizeof(szQuestType)
	);

	if (CString(szQuestType) == _T("收集"))
	{
		nIndex = 3;
	}
	else if (CString(szQuestType) == _T("送信"))
	{
		nIndex = 4;
	}
	else if (CString(szQuestType) == _T("杀怪"))
	{
		nIndex = 5;
	}
	else if (CString(szQuestType) == _T("对话"))
	{
		nIndex = 6;
	}
	else if (CString(szQuestType) == _T("贡献"))
	{
		nIndex = 7;
	}
	else
	{
		nIndex = 8;
	}

	return nIndex;

}

// Update In 2006.2.27.5.34 By Chiyer
int KGObjectPropertyEditTabDlg::CopySubsetIniFile(
	HTREEITEM hTreeItem, IIniFile* pPopIniFile
)
{
	int nResult   = false;
	int nRetCode  = false;
	int nItemDate = 0;

	char szKeyName[256];
	char szKeyValue[1024];
	char szBookMark[4000];

	*szKeyValue = '\0';
	*szKeyName  = '\0';
	*szBookMark = '\0';

	CString szSectionName = "";
	CString szItemDate    = "";
	CString szName		  = "";

	HTREEITEM hTempItem = hTreeItem;
	KG_PROCESS_ERROR(hTreeItem);

	nItemDate  = m_treeObjectView.GetItemData(hTreeItem);

	szItemDate.Format("%d", nItemDate);

	if (nItemDate < 1 || nItemDate == SUBSET_TREE_DATA)
	{
		while (hTempItem)
		{
			szName = m_treeObjectView.GetItemText(hTempItem) + szName;
			hTempItem  = m_treeObjectView.GetParentItem(hTempItem);
			if (hTempItem)
				szName = _T("-") + szName;
		}
		m_pQuestIniFile->GetString(
			szName.GetBuffer(), _T("BookMark"), "",
			szBookMark, sizeof(szBookMark)
		);
		pPopIniFile->WriteString(szName.GetBuffer(), _T("BookMark"), szBookMark);
	}

	m_treeObjectView.IsInSubset(m_pQuestIniFile, szItemDate, szSectionName);

	if (szSectionName != "")
	{
		while (m_pQuestIniFile->GetNextKey(szSectionName, szKeyName, szKeyName))
		{
			if (CString(szKeyName) != _T("TreeHandle"))
			{
				m_pQuestIniFile->GetString(
					szSectionName, szKeyName, "", szKeyValue, sizeof(szKeyValue)
				);
				pPopIniFile->WriteString(szSectionName, szKeyName, szKeyValue);
			}
		}
	}
	else
	{
		CopySubsetIniFile(m_treeObjectView.GetChildItem(hTreeItem), pPopIniFile);
	}

	CopySubsetIniFile(m_treeObjectView.GetNextSiblingItem(hTreeItem), pPopIniFile);

	nResult = true;
Exit0:
	return nResult;
}

int KGObjectPropertyEditTabDlg::UpdateGrid(int nEnbale)
{
	OnSize(SIZE_MAXIMIZED, 2, 2);

	return true;
}


// Update In 2006.2.27.5.34 By Chiyer
void KGObjectPropertyEditTabDlg::OnNew()
{
	int nResult  = false;
	int nRetCode = false;

	int nMainClassNumber = m_comBoxType.GetCount();
	int i = 0;
	int nPropertyNumber = 0;
	int nInsertNumber = 0;

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
	m_proessBar.SetRange(1, nPropertyNumber);
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

	m_szCurBookMarkName = _T("");
	m_szPreBookMarkName = _T("");
	m_szBookMark        = _T("");
	UpdateData(false);

	m_treeObjectView.Update();
	m_wndStateBar.SetPaneText(0, _T("正在编辑 ^_^"));
	m_nEditerState = EDIT_STATE_EDIT_BOOKMARK;
	UpdateGrid(true);

	m_nBeSavedFlag = true;

Exit1:
	nResult = true;
Exit0:
	if (!nResult)
	{
		m_nEditerState = EDIT_STATE_CLOSE;
		UpdateGrid(true);
	}
	return;
}

// Update In 2006.2.27.5.34 By Chiyer
int KGObjectPropertyEditTabDlg::BuildPrepertyFile(
	ITabFile* pTabFile, IIniFile* pIniFile,
	const char pcszSectionName[], int* pnInsertNumber,
	CProgressCtrl* pProCtrl
)
{
	int nResult  = false;
	int nRetCode = false;

	char szPropertyValue[128];
	char szKeyValue[128];
	char szKeyName[128];
	char szDependValue[128];
	char szDependName[128];

	*szKeyValue     = '\0';
	*szKeyName   	= '\0';
	*szDependValue  = '\0';
	*szDependName   = '\0';

	KG_PROCESS_ERROR(pcszSectionName);
	KG_PROCESS_ERROR(pTabFile);
	KG_PROCESS_ERROR(pIniFile);

	if (KGPropertyListCtrl::IsClassifySection(pIniFile, pcszSectionName))
	{

		while (
			pIniFile->GetNextKey(pcszSectionName, szKeyName, szKeyName)
		)
		{
			pIniFile->GetString(
				pcszSectionName, szKeyName, _T(""), szKeyValue, sizeof(szKeyValue)
			);
			if (
				(CString(szKeyName) != _T("Depend_Name")) &&
				(CString(szKeyName) != _T("Depend_Value"))
				)
			{
				BuildPrepertyFile(
					pTabFile, pIniFile, szKeyValue, pnInsertNumber
				);
			}
		}
	}
	else
	{
		char szPropertyValueType[128];
		char szTempValue[128];
		ITEM_VILUE_TYPE eItemValueType;

		*szPropertyValueType ='\0';
		*szTempValue = '\0';

		nRetCode = pIniFile->GetString(
			pcszSectionName, _T("Type"), _T(""),
			szPropertyValueType, sizeof(szPropertyValueType)
		);

		eItemValueType = KGPropertyListCtrl::GetKeyType(szPropertyValueType);

		switch (eItemValueType)
		{
		case VALUE_STRING :
		case VALUE_FLOAT  :
		case VALUE_COLOR  :
		case VALUE_INT    :
		case VALUE_BOOL   :
		case VALUE_TYPE   :
		case VALUE_FORCE_ID :
		case VALUE_COOLDOWN_ID :
		case VALUE_STATE_ID :
		case VALUE_NPC_ID :
		case VALUE_NPC_TEMPLATE :
			nRetCode = pTabFile->InsertNewCol(pcszSectionName);
			KG_PROCESS_ERROR(nRetCode);
			nRetCode = pIniFile->GetString(
				pcszSectionName,_T("Def"), _T(""), szTempValue, sizeof(szTempValue)
			);
			if (nRetCode)
			{
				nRetCode = pTabFile->WriteString(2, pcszSectionName, szTempValue);
				KG_PROCESS_ERROR(nRetCode);
			}
			*pnInsertNumber++;
			break;
		case VALUE_QUEST_MAPNPC :
		case VALUE_MIN_MAX      :
		case VALUE_DITEM      :
		case VALUE_ITEM       :
		case VALUE_FORCE_IV   :
		case VALUE_STATE_IV   :
		case VALUE_LIVE_IV    :
		case VALUE_SKILL_IV   :
		case VALUE_NPC_ID_NUM :
		case VALUE_ITEM_TID_IID :
		case VALUE_ITEM_TID_IID_NUM :
		case VALUE_GOSSIP :
		case VALUE_PREQUEDT_ID :
			nRetCode = pIniFile->GetString(
				pcszSectionName,_T("PARAM_1"), _T(""), szTempValue, sizeof(szTempValue)
			);
			nRetCode = pTabFile->InsertNewCol(szTempValue);
			KG_PROCESS_ERROR(nRetCode);

			nRetCode = pIniFile->GetString(
				pcszSectionName,_T("PARAM_2"), _T(""), szTempValue, sizeof(szTempValue)
			);
			nRetCode = pTabFile->InsertNewCol(szTempValue);
			KG_PROCESS_ERROR(nRetCode);

			*pnInsertNumber += 2;

			if (eItemValueType == VALUE_ITEM_TID_IID_NUM)
			{
				nRetCode = pIniFile->GetString(
					pcszSectionName,_T("PARAM_3"), _T(""),
					szTempValue, sizeof(szTempValue)
				);
				nRetCode = pTabFile->InsertNewCol(szTempValue);
				KG_PROCESS_ERROR(nRetCode);
				*pnInsertNumber++;
			}
			if (eItemValueType == VALUE_GOSSIP || eItemValueType == VALUE_PREQUEDT_ID)
			{
				nRetCode = pIniFile->GetString(
					pcszSectionName,_T("PARAM_3"), _T(""),
					szTempValue, sizeof(szTempValue)
				);
				nRetCode = pTabFile->InsertNewCol(szTempValue);
				KG_PROCESS_ERROR(nRetCode);
				nRetCode = pIniFile->GetString(
					pcszSectionName,_T("PARAM_4"), _T(""),
					szTempValue, sizeof(szTempValue)
				);
				nRetCode = pTabFile->InsertNewCol(szTempValue);
				KG_PROCESS_ERROR(nRetCode);
				nRetCode = pIniFile->GetString(
					pcszSectionName,_T("PARAM_5"), _T(""),
					szTempValue, sizeof(szTempValue)
				);
				nRetCode = pTabFile->InsertNewCol(szTempValue);
				KG_PROCESS_ERROR(nRetCode);
				*pnInsertNumber += 3;
			}
			break;
		default :
			nRetCode = pTabFile->InsertNewCol(pcszSectionName);
			KG_PROCESS_ERROR(nRetCode);
			nRetCode = pIniFile->GetString(
				pcszSectionName,_T("Def"), _T(""), szTempValue, sizeof(szTempValue)
			);
			if (nRetCode)
			{
				nRetCode = pTabFile->WriteString(2, pcszSectionName, szTempValue);
				KG_PROCESS_ERROR(nRetCode);
			}
			*pnInsertNumber++;
			break;
		}
		if (pProCtrl)
		{
			pProCtrl->SetPos(*pnInsertNumber);
		}
	}

	nResult = true;
Exit0:
	return nResult;
}

// Update In 2006.2.27.5.34 By Chiyer
void KGObjectPropertyEditTabDlg::OnOpen()
{
	int  nResult  = false;
	int  nRetCode = false;

	int  nMainClassNumber = 0;
	int  i = 0;
	int  nLoadNumber = 0;

	char szKeyName[256];
	char szKeyValue[256];
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
		m_szPropertyFileName = dlg.GetPathName();
		szQuestIniFile = m_szPropertyFileName;

		SAFE_RELEASE(m_pTabFile);
		SAFE_RELEASE(m_pQuestIniFile);

		m_pTabFile = g_OpenTabFile(
			m_szPropertyFileName.GetBuffer(), false, true
		);
		KG_PROCESS_ERROR(m_pTabFile);
		SetWindowText(
			_T("任务编辑器->正在编辑任务文件:") + m_szPropertyFileName
		);

		szQuestIniFile.Replace(".tab", ".ini");
		m_pQuestIniFile = g_OpenIniFile(szQuestIniFile.GetBuffer());
		//m_szBookScenceName = "";

		//if (!m_pBookFile)
		//{
		//	m_pBookFile = g_CreateIniFile();
		//}

		nMainClassNumber = m_comBoxType.GetCount();
		for (i = 0; i < nMainClassNumber; i++)
		{
			m_comBoxType.DeleteString(0);
		}
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

		UpdateGrid(true);
	}

Exit1:
	nResult = true;
Exit0:
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

Exit1:
	nResult = true;
Exit0:
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
Exit0:
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
Exit0:
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
	CString    szString;
	int        nIndex;
	CSize      size;
	CRect	   rect;

	UpdateData();

	nIndex = m_comBoxType.GetCurSel();
	m_comBoxType.GetLBText(nIndex, szString);
	m_listProperty.GetItemRect(nIndex, &rect, true);

	m_listProperty.FindItemByText(szString);
	nIndex  = m_listProperty.FindItemPos(m_listProperty.FindNextItem());

	size.cy = (nIndex - m_listProperty.GetTopIndex()) * rect.Height();

	m_listProperty.Scroll(size);
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

	::LoadString(AfxGetInstanceHandle(), nID, szFullText, sizeof(szFullText));
	strTipText = szFullText;

	strncpy(pTTTA->szText, strTipText, sizeof(pTTTA->szText));

	*pResult = 0;

	::SetWindowPos(
		pNMHDR->hwndFrom, HWND_TOP, 0, 0, 0, 0,
		SWP_NOACTIVATE | SWP_NOSIZE  | SWP_NOMOVE
	);

	nResult = true;
Exit0:
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
		SetClassLong(this->m_hWnd, GCL_HCURSOR, (LONG)hCr);
	}
	else
	{
		HCURSOR hCr = LoadCursor(NULL, IDC_ARROW);
		SetClassLong(this->m_hWnd, GCL_HCURSOR, (LONG)hCr);
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

	CString szMapName = "";
	int nTreeItemData = 0;
	HTREEITEM hInsertItem = NULL;

	KGValueEditGetValueDlg dlg(this, _T("请输入要创建的[地图]名 :"));
	if (dlg.DoModal() == IDOK)
	{
		szMapName = dlg.GetValue();
	}
	KG_PROCESS_ERROR(szMapName != "");

	/*
	nTreeItemData = m_treeObjectView.GetItemData(m_hTreeSelItem);
	KG_PROCESS_ERROR(nTreeItemData == 0);
	*/

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

	CString szMapName = "";
	int nTreeItemData = 0;
	HTREEITEM hInsertItem = NULL;

	KGValueEditGetValueDlg dlg(this, _T("请输入要创建的[任务链]名 :"));
	if (dlg.DoModal() == IDOK)
	{
		szMapName = dlg.GetValue();
	}
	KG_PROCESS_ERROR(szMapName != "");

	nTreeItemData = m_treeObjectView.GetItemData(m_hTreeSelItem);

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
	m_treeObjectView.SetItemData(hInsertItem, -1);
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

	KGValueEditGetValueDlg dlg(this, _T("请输入要创建的[任务集]名 :"));
	if (dlg.DoModal() == IDOK)
	{
		szSubsetName = dlg.GetValue();
	}
	KG_PROCESS_ERROR(szSubsetName != "");

	nTreeItemData = m_treeObjectView.GetItemData(m_hTreeSelItem);

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
		nRetCote = m_treeObjectView.IsInSubset(
			m_pQuestIniFile, szTreeItemData, szTemp
		);
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

	KGValueEditGetValueDlg dlg(this, _T("请输入要创建的[") + szQuestType +_T("]名 :"));
	if (dlg.DoModal() == IDOK)
	{
		szQuestName = dlg.GetValue();
	}
	KG_PROCESS_ERROR(szQuestName != _T(""));

	nRowNumber = m_pTabFile->GetHeight();
	KG_PROCESS_ERROR(nRowNumber >= 2);

	nTreeItemData = m_treeObjectView.GetItemData(m_hTreeSelItem);

	if (nTreeItemData == -1)
	{
		hParentItem = m_hTreeSelItem;
		hAfterItem  = NULL;
		hPreQstItem = NULL;
		hAftQstItem = NULL;
	}
	else if (nTreeItemData == SUBSET_TREE_DATA)
	{
		hParentItem = m_hTreeSelItem;
		hAfterItem  = NULL;
		hPreQstItem = NULL;//m_treeObjectView.GetPrevSiblingItem(m_hTreeSelItem);
		hAftQstItem = m_treeObjectView.GetNextValidateItem(m_hTreeSelItem);
										//GetNextSiblingItem(m_hTreeSelItem);
	}
	else if (nTreeItemData >= 1)
	{
		hParentItem = m_treeObjectView.GetParentItem(m_hTreeSelItem);
		hAfterItem  = m_hTreeSelItem;
		hPreQstItem = hAfterItem;
		nItemData   = m_treeObjectView.GetItemData(hParentItem);
		if (nItemData = SUBSET_TREE_DATA)
			hAftQstItem = m_treeObjectView.GetNextValidateItem(hParentItem);//GetNextSiblingItem(hParentItem);
		else
			hAftQstItem = m_treeObjectView.GetNextValidateItem(hPreQstItem);//GetNextSiblingItem(hPreQstItem);
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

	// Find The Dest Row In Quest Tab File To Insert

//

	hPreQstItem = m_treeObjectView.GetPrevValidateItem(hInsertItem);
	if (!hAfterItem)
	{
		nItemData = m_treeObjectView.GetItemData(hParentItem);
		if (nItemData == SUBSET_TREE_DATA)
		{
			hPreQstItem = m_treeObjectView.GetPrevValidateItem(hParentItem);
		}
	}

//
	if (hPreQstItem)
	{
		nDestItemPos = m_treeObjectView.GetItemPos(hPreQstItem);
	}
	else
	{
		nDestItemPos = m_treeObjectView.GetCollectivityLastPos(hParentItem);
		if (nDestItemPos == -1)
		{
			if (hParentItem)
			{
				HTREEITEM hTempItem =
					m_treeObjectView.GetParentItem(hParentItem);
				while (nDestItemPos == -1 && hTempItem)
				{
					nDestItemPos =
						m_treeObjectView.GetCollectivityLastPos(hTempItem);
					hTempItem = m_treeObjectView.GetParentItem(hTempItem);
				}
				if (nDestItemPos == -1)
				{
					nDestItemPos = nRowNumber;
				}
			}
			else
			{
				nDestItemPos = nRowNumber;
			}
		}
	}

	m_pTabFile->InsertAfter(nDestItemPos++);
	nRetCode = m_treeObjectView.CopyOneRow(nSourItemPos, nDestItemPos);
	KG_PROCESS_ERROR(nRetCode);

	nItemData = m_treeObjectView.FindId(_T("QuestID"));
	nRetCode  = m_treeObjectView.SetItemData(hInsertItem, nItemData);
	KG_PROCESS_ERROR(nRetCode);
	nRetCode  = m_pTabFile->WriteInteger(
		nDestItemPos, _T("QuestID"), nItemData
	);
	KG_PROCESS_ERROR(nRetCode);
	nRetCode = m_pTabFile->WriteString(
		nDestItemPos, _T("QuestType"), szQuestType.GetBuffer()
	);
	KG_PROCESS_ERROR(nRetCode);
	nRetCode = m_treeObjectView.RefrushTypeInfo(hInsertItem);
	KG_PROCESS_ERROR(nRetCode);
	nRetCode = m_treeObjectView.SetDataRelating(hInsertItem);
	KG_PROCESS_ERROR(nRetCode);
	if (hAftQstItem)
	{
		nRetCode = m_treeObjectView.SetDataRelating(hAftQstItem);
		KG_PROCESS_ERROR(nRetCode);
	}
	m_treeObjectView.SelectItem(hInsertItem);

	nLaodNumber = m_pTabFile->GetHeight() - 2;
	szLoadNumber.Format(_T("任务个数 : %d"), nLaodNumber);
	m_wndStateBar.SetPaneText(2, szLoadNumber.GetBuffer());

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
Exit0:
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

		nTreeItemData = m_treeObjectView.GetItemData(m_hTreeSelItem);

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
			int nItemData = m_treeObjectView.GetItemData(hParentItem);
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

	m_proessBar.SetRange(1, nQuestFileColsCount);

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
			LVFINDINFO info;
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

			hTreeItem = m_treeObjectView.GetItemByNameplate(
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
			false, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
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

void KGObjectPropertyEditTabDlg::OnQuestPopAll()
{
	int nResult  = false;
	int nRetCode = false;

	CString   szExistingFileName = m_szPropertyFileName;
	CString   szPopQuestFile = "";

	CFileDialog dlg(
		false, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"Quest File|*.tab|All File|*.*|", this
	);
	KG_PROCESS_SUCCESS(dlg.DoModal() != IDOK);

	szPopQuestFile = dlg.GetPathName();
	KG_PROCESS_ERROR(szPopQuestFile != _T(""));

	if (szPopQuestFile.Find(".tab") == -1)
	{
		szPopQuestFile += ".tab";
	}

	nRetCode = ::CopyFile(
		szExistingFileName.GetBuffer(), szPopQuestFile.GetBuffer(), TRUE
	);
	KG_PROCESS_ERROR(nRetCode);

	szExistingFileName.Replace(_T(".tab"), _T(".ini"));
	szPopQuestFile.Replace(_T(".tab"), _T(".ini"));

	nRetCode = ::CopyFile(
		szExistingFileName.GetBuffer(), szPopQuestFile.GetBuffer(), TRUE
	);
	KG_PROCESS_ERROR(nRetCode);

Exit1:
	nResult = true;
Exit0:
	return;
}

// Update 2006.3.7 10:43
void KGObjectPropertyEditTabDlg::OnQuestPop()
{
	int nResult  = false;
	int nRetCode = false;

	CString   szPopQuestFile = "";

	ITabFile* pPopTabFile    = g_CreateTabFile();
	IIniFile* pPopIniFile	 = g_CreateIniFile();

	HTREEITEM hSelTreeItem   = NULL;

	int       nTreeItemData  = 0;
	int		  nFirstRow      = 0;
	int		  nLastRow		 = 0;
	int		  nRow			 = 0;
	int		  nCol			 = 0;
	int		  nPopRow		 = 0;
	int		  nColsNumber	 = 0;

	char	  szTempValue[1024];
	*szTempValue = '\0';

	CFileDialog dlg(
		false, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"Quest File|*.tab|All File|*.*|", this
	);

	KG_PROCESS_ERROR(pPopTabFile);
	KG_PROCESS_ERROR(pPopIniFile);
	KG_PROCESS_ERROR(m_pTabFile);

	nColsNumber = m_pTabFile->GetWidth();

	KG_PROCESS_ERROR(dlg.DoModal() == IDOK);
	szPopQuestFile = dlg.GetPathName();
	if (szPopQuestFile.Find(".tab") == -1)
	{
		szPopQuestFile += ".tab";
	}

	hSelTreeItem = m_treeObjectView.GetSelectedItem();
	KG_PROCESS_ERROR(hSelTreeItem);

	for (int nRow = 1; nRow <= 2; nRow++)
	{
		for (nCol = 1; nCol <= nColsNumber; nCol++)
		{
			nRetCode = m_pTabFile->GetString(
				nRow, nCol, "", szTempValue, sizeof(szTempValue)
			);
			nRetCode = pPopTabFile->WriteString(nRow, nCol, szTempValue);
		}
	}

	nTreeItemData = m_treeObjectView.GetItemData(hSelTreeItem);
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

	nPopRow = 3;
	for (int nRow = nFirstRow; nRow <= nLastRow; nRow++)
	{
		for (nCol = 1; nCol <= nColsNumber; nCol++)
		{
			nRetCode = m_pTabFile->GetString(
				nRow, nCol, "", szTempValue, sizeof(szTempValue)
			);
			nRetCode = pPopTabFile->WriteString(nPopRow, nCol, szTempValue);
			m_proessBar.SetPos((nRow - nFirstRow) * nColsNumber + nCol);
		}
		nPopRow++;
	}

	CopySubsetIniFile(m_treeObjectView.GetChildItem(hSelTreeItem), pPopIniFile);

	pPopTabFile->Save(szPopQuestFile);
	szPopQuestFile.Replace(_T(".tab"), _T(".ini"));
	pPopIniFile->Save(szPopQuestFile);

	m_proessBar.SetPos(1);

	nResult = true;
Exit0:
	SAFE_RELEASE(pPopTabFile);
	SAFE_RELEASE(pPopIniFile);
	return;
}

void KGObjectPropertyEditTabDlg::OnQuestIn()
{
	int nResult  = false;
	int nRetCode = false;


	CString szInsertQuestFile = _T("");
	CString szErrorMessage    = _T("");

	ITabFile* pInsertTabFile  = NULL;
	IIniFile* pInsertIniFile  = NULL;

	RETURN_VALUE eReturnValue    = E_CANCEL;
	RETURN_VALUE eReturnValueOld = E_CANCEL;

	int nRowCount        = 0;
	int nCurRow			 = 0;
	int nInsertRowsCount = 0;
	int nCurInsertRow	 = 0;
	int nInsertColsCount = 0;
	int nColsCount       = 0;

	char szSectionName[256];
	char szKeyName[256];
	char szKeyValue[1024];

	*szSectionName = '\0';
	*szKeyValue    = '\0';
	*szKeyName     = '\0';

	CFileDialog dlg(
		true, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"Quest File|*.tab|All File|*.*|", this
	);

	KG_PROCESS_ERROR(m_pQuestIniFile);
	KG_PROCESS_ERROR(m_pTabFile);
	nRowCount = m_pTabFile->GetHeight();

	KG_PROCESS_SUCCESS(dlg.DoModal() != IDOK);

	szInsertQuestFile = dlg.GetPathName();
	KG_PROCESS_ERROR(szInsertQuestFile.Find(".tab") != -1);

	pInsertTabFile = g_OpenTabFile(szInsertQuestFile, false, true);
	KG_PROCESS_ERROR(pInsertTabFile);

	nInsertRowsCount = pInsertTabFile->GetHeight();
	KG_PROCESS_SUCCESS(nInsertRowsCount <= 2);

	szInsertQuestFile.Replace(_T(".tab"), _T(".ini"));
	pInsertIniFile = g_OpenIniFile(szInsertQuestFile);
	szInsertQuestFile.Replace(_T(".ini"), _T(".tab"));
	KG_PROCESS_ERROR(pInsertIniFile);

	nInsertColsCount = pInsertTabFile->GetWidth();
	nColsCount = m_pTabFile->GetWidth();

	KG_PROCESS_ERROR(nInsertColsCount == nColsCount);

	m_proessBar.SetRange32(1, nInsertRowsCount * nColsCount);
	m_proessBar.SetPos(1);

	for (nCurInsertRow = 3; nCurInsertRow <= nInsertRowsCount; nCurInsertRow++ )
	{
		CString pszNameplate[2];

		CString szPrimaryKey        = "";
		CString szSubsetSectionName = "";

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

		for (nCurRow = 3; nCurRow <= nRowCount; nCurRow++)
		{
			int nNameplate = 0;
			m_pTabFile->GetInteger(nCurRow, _T("QuestID"), 0, &nNameplate);
			if (nNameplate == nInsertNameplate)
			{
				break;
			}
		}

		if (nCurRow > nRowCount || nRowCount == 2)
		{
			nInsertRow = m_treeObjectView.FindInsertPosInFile(pszNameplate, 2);
			m_pTabFile->InsertAfter(nInsertRow++);
			goto INSERT;
		}

		eReturnValueOld = eReturnValue;

		szPrimaryKey.Format(_T("%d"), nInsertNameplate);
		nInSubsetFlag = m_treeObjectView.IsInSubset(
			pInsertIniFile, szPrimaryKey, szSubsetSectionName
		);
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

			nInsertNameplate  = m_treeObjectView.FindId(_T("QuestID"));
			nRetCode = pInsertTabFile->WriteInteger(
				nCurInsertRow, _T("QuestID"), nInsertNameplate
			);
			KG_PROCESS_ERROR(nRetCode);

			szNewPrimaryKey.Format(_T("%d"), nInsertNameplate);

			nInsertRow = m_treeObjectView.FindInsertPosInFile(pszNameplate, 2);
			m_pTabFile->InsertAfter(nInsertRow++);

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
					if (nPos == -1)
					{
						nPos = szNewSubSectionName.Find(_T("-") + szPrimaryKey);
						if (nPos == -1)
						{
							nPos = szNewSubSectionName.Find(szPrimaryKey);
							KG_PROCESS_ERROR(nPos == -1);
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
			m_proessBar.SetPos(nCurInsertRow * nColsCount + nCurCols);
		}

		if (szSubsetSectionName != _T(""))
		{
			eReturnValue = eReturnValueOld;
		}

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

	m_treeObjectView.Update();
	m_treeObjectView.ExpendAll();
	m_nBeSavedFlag = false;

Exit1:
	nResult = true;
Exit0:
	if (nInsertColsCount != nColsCount)
	{
		szErrorMessage = _T("导入文件与现有文件版本不一致!!");
		::MessageBox(
			this->m_hWnd, szErrorMessage.GetBuffer(), _T("Insert File Error"),
			MB_OK | MB_ICONERROR
		);
	}
	if (!pInsertIniFile || !pInsertTabFile)
	{
		szErrorMessage = _T("Can Not Open File : ") + szInsertQuestFile;
		::MessageBox(
			this->m_hWnd, szErrorMessage.GetBuffer(), _T("Insert File Error"),
			MB_OK | MB_ICONERROR
		);
	}
	szInsertQuestFile.ReleaseBuffer();
	szErrorMessage.ReleaseBuffer();
	SAFE_RELEASE(pInsertTabFile);
	SAFE_RELEASE(pInsertIniFile);

	return;
}

ITEM_VILUE_TYPE KGObjectPropertyEditTabDlg::GetKeyType(CString szKeyType)
{
	ITEM_VILUE_TYPE eKetType;
/*
	if (szKeyType == "STRING")
	{
		eKetType = VALUE_STRING;
	}
	else if (szKeyType == "INT")
	{
		eKetType = VALUE_INT;
	}
	else if (szKeyType == "FLOAT")
	{
		eKetType = VALUE_FLOAT;
	}
	else if (szKeyType == "BOOL")
	{
		eKetType = VALUE_BOOL;
	}
	else if (szKeyType == "STATE3")
	{
		eKetType = VALUE_STATE_TREE;
	}
	else if (szKeyType == "COLOR")
	{
		eKetType = VALUE_COLOR;
	}*/
	/*else if (szKeyType == "TITLE")
	{
		eKetType = VALUE_TITLE;
	}
	else if (szKeyType == "SUBTITLE")
	{
		eKetType = VALUE_SUBTITLE;
	}*//*
	else if (szKeyType == "QUEST_TYPE")
	{
		eKetType = VALUE_TYPE;
	}
	else if (szKeyType == "FILE")
	{
		eKetType = VALUE_FILE_PATH;
	}
	else if (szKeyType == "MAP_NPC")
	{
		eKetType = VALUE_QUEST_MAPNPC;
	}
	else if (szKeyType == "DITEM")
	{
		eKetType = VALUE_DITEM;
	}
	else if (szKeyType == "ITEM")
	{
		eKetType = VALUE_ITEM;
	}
	else if (szKeyType == "FORCE_ID")
	{
		eKetType = VALUE_FORCE_ID;
	}
	else if (szKeyType == "FORCE_IV")
	{
		eKetType = VALUE_FORCE_IV;
	}
	else if (szKeyType == "COOLDOWN_ID")
	{
		eKetType = VALUE_COOLDOWN_ID;
	}
	else if (szKeyType == "STATE_IV")
	{
		eKetType = VALUE_STATE_IV;
	}
	else if (szKeyType == "LIVE_IV")
	{
		eKetType = VALUE_LIVE_IV;
	}
	else if (szKeyType == "SKILL_IV")
	{
		eKetType = VALUE_SKILL_IV;
	}
	else if (szKeyType == "NPC_ID")
	{
		eKetType = VALUE_NPC_ID;
	}
	else if (szKeyType == "NPC_ID_NUM")
	{
		eKetType = VALUE_NPC_ID_NUM;
	}
	else if (szKeyType == "MIN_MAX")
	{
		eKetType = VALUE_MIN_MAX;
	}
	else
	{
		eKetType = VALUE_UNKNOW;
	}
*/
	return eKetType;
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

LRESULT KGObjectPropertyEditTabDlg::OnListEnter(WPARAM wParam, LPARAM lParam)
{
	int nParam = 2;
	OnNMDblclkObjectPropertyList((NMHDR*)&nParam, (LRESULT*)&nParam);
	return true;
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

		eCurEditerState = EDIT_STATE_EDIT_BOOKMARK;
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

	int nIndex   = 0;
	int nQuestId = 0;
	int nRow     = 3;
	int nCol     = 1;
	int nWidth   = 0;
	int nHeight  = 0;

	CString szText = "";
	CString szName = "";

	KG_PROCESS_ERROR(m_pTabFile);

	HTREEITEM hItem = m_treeObjectView.GetSelectedItem();
	KG_PROCESS_ERROR(hItem);

	m_hTreeSelItem = hItem;
	szText = m_treeObjectView.GetItemText(hItem);
	nIndex = m_treeObjectView.GetItemData(hItem);

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
	KG_PROCESS_SUCCESS(nHeight <= 2);


	if (nIndex < 1 || nIndex == SUBSET_TREE_DATA)
	{
		m_nCurRow = nIndex;
		KG_PROCESS_SUCCESS(true);
	}

	for (nRow = 3; nRow <= nHeight; nRow++)
	{
		m_pTabFile->GetInteger(nRow, "QuestID", -1000, &nQuestId);
		KG_PROCESS_ERROR(nQuestId != -1);
		if (nQuestId == nIndex)
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
	}
	else if (m_nEditerState == EDIT_STATE_EDIT_BOOKMARK)
	{
		m_editBookMarker.MoveWindow(m_rectList);
		m_treeObjectView.MoveWindow(m_rectTree);

		m_editBookMarker.ShowWindow(SW_SHOW);
		m_listProperty.ShowWindow(SW_HIDE);
		m_treeObjectView.ShowWindow(SW_SHOW);
		m_iniWinidow.ShowWindow(SW_HIDE);
	}
	else
	{
		m_iniWinidow.MoveWindow(rectIniWindow);

		m_editBookMarker.ShowWindow(SW_HIDE);
		m_listProperty.ShowWindow(SW_HIDE);
		m_treeObjectView.ShowWindow(SW_HIDE);
		m_iniWinidow.ShowWindow(SW_SHOW);
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
Exit0:
	return;
}

int KGObjectPropertyEditTabDlg::UnInit()
{
	int nResult  = false;
	int nRetCode = false;

	SAFE_RELEASE(m_pQuestIniFile);
	SAFE_RELEASE(m_pTabFile);
	SAFE_RELEASE(m_pIniFile);

	KG_PROCESS_ERROR(m_pView);
	m_pView->m_pTaseDlg = NULL;

Exit1:
	nResult = true;
Exit0:
	return nResult;
}

int  KGObjectPropertyEditTabDlg::UpdateTaskTree()
{
	int  nResult  = false;
	int  nRetCode = false;
	/*
	int  nRow     = 3;
    int  nCol     = 1;
	int  nWidth   = 0;
	int  nHeight  = 0;

	int  nQusetId	= 0;
	int  nQuestType = 0;

	const int DEFAULT_VALUE_NUM = 2;

	char szMapName[MAX_LENGTH];
	char szTaskLinkName[MAX_LENGTH];
	char szQuestName[MAX_LENGTH];
	char szQuestType[MAX_LENGTH];

	CString szErrorInfo;

	HTREEITEM pTreeItem;

	//ASSERT(m_pTabFile);

	//nWidth  = m_pTabFile->GetWidth();
	//nHeight = m_pTabFile->GetHeight();

	m_treeObjectView.DeleteAllItems();
	KG_PROCESS_ERROR(m_pTabFile);

	nWidth  = m_pTabFile->GetWidth();
	nHeight = m_pTabFile->GetHeight();

	KG_PROCESS_ERROR(nWidth);
	KG_PROCESS_SUCCESS(nHeight == 2);
	KG_PROCESS_ERROR(nHeight >= 2);

	m_proessBar.ShowWindow(SW_SHOW);
	m_proessBar.SetRange(1, nHeight);

	//
	// Insert The First Tree Node
	//

	m_pTabFile->GetString(
		nRow, _T("MapName"), "", szMapName, sizeof(szMapName)
	);
	KG_PROCESS_ERROR(CString(szMapName) != "");
	m_hTreeRootItem = m_treeObjectView.InsertItem(
		szMapName, QUEST_MAP, QUEST_MAP
	);
	KG_PROCESS_ERROR(m_hTreeRootItem);
	m_treeObjectView.SetItemData(m_hTreeRootItem, -1);

	m_pTabFile->GetString(
		nRow, _T("TaskLinkName"), "",
		szTaskLinkName, sizeof(szTaskLinkName)
	);
	KG_PROCESS_ERROR(CString(szTaskLinkName) != "");
	pTreeItem = m_treeObjectView.InsertItem(
		szTaskLinkName, QUEST_LINK, QUEST_LINK, m_hTreeRootItem
	);
	KG_PROCESS_ERROR(pTreeItem);
	m_treeObjectView.SetItemData(pTreeItem, -2);

	m_pTabFile->GetString(
		nRow, _T("QuestName"), "", szQuestName, sizeof(szQuestName)
	);
	KG_PROCESS_ERROR(szQuestName);

	m_pTabFile->GetString(
		nRow, _T("QuestType"), "其他", szQuestType, sizeof(szQuestType)
	);

	if (CString(szQuestType) == "收集")
	{
		nQuestType = QUEST_GLEAN;
	}
	else if (CString(szQuestType) == "杀怪")
	{
		nQuestType = QUEST_KILL;
	}
	else if (CString(szQuestType) == "送信")
	{
		nQuestType = QUEST_POS;
	}
	else if (CString(szQuestType) == "对话")
	{
		nQuestType = QUEST_TALK;
	}
	else if (CString(szQuestType) == "贡献")
	{
		nQuestType = QUEST_OFFER;
	}
	else
	{
		nQuestType = QUEST_OTHER;
	}

	pTreeItem = m_treeObjectView.InsertItem(
		szQuestName, nQuestType, nQuestType, pTreeItem
	);
	KG_PROCESS_ERROR(pTreeItem);

	m_pTabFile->GetInteger(
		nRow, _T("QuestID"), -1, &nQusetId
	);
	KG_PROCESS_ERROR(nQusetId != -1);
	m_treeObjectView.SetItemData(pTreeItem, nQusetId);

	//
	// Insert All Node Of The Tree
	//

	// Need Optimize :)
	for (nRow = 4; nRow <= nHeight; nRow++)
	{
		m_proessBar.SetPos(nRow);
		m_pTabFile->GetString(
			nRow, "MapName", "", szMapName, sizeof(szMapName)
		);
		KG_PROCESS_ERROR(CString(szMapName) != "");

		m_pTabFile->GetString(
			nRow, _T("TaskLinkName"), "",
			szTaskLinkName, sizeof(szTaskLinkName)
		);
		KG_PROCESS_ERROR(CString(szTaskLinkName) != "");

		m_pTabFile->GetString(
			nRow, _T("QuestName"), "",
			szQuestName, sizeof(szQuestName)
		);
		KG_PROCESS_ERROR(CString(szQuestName) != "");

		m_pTabFile->GetInteger(
			nRow, _T("QuestID"), -1, &nQusetId
		);
		KG_PROCESS_ERROR(nQusetId != -1);

		m_pTabFile->GetString(
			nRow, _T("QuestType"), "其他", szQuestType, sizeof(szQuestType)
		);

		if (CString(szQuestType) == "收集")
		{
			nQuestType = QUEST_GLEAN;
		}
		else if (CString(szQuestType) == "杀怪")
		{
			nQuestType = QUEST_KILL;
		}
		else if (CString(szQuestType) == "送信")
		{
			nQuestType = QUEST_POS;
		}
		else if (CString(szQuestType) == "对话")
		{
			nQuestType = QUEST_TALK;
		}
		else if (CString(szQuestType) == "贡献")
		{
			nQuestType = QUEST_OFFER;
		}
		else
		{
			nQuestType = QUEST_OTHER;
		}

		HTREEITEM pItem = m_hTreeRootItem;
		while (pItem)
		{
			CString szName = m_treeObjectView.GetItemText(pItem);
			if (szName == CString(szMapName))
			{
				HTREEITEM pSubItem = m_treeObjectView.GetChildItem(pItem);
				while (pSubItem)
				{
					CString szText = m_treeObjectView.GetItemText(pSubItem);
					if (szText == CString(szTaskLinkName))
					{
						HTREEITEM hItem = m_treeObjectView.InsertItem(
							szQuestName, nQuestType, nQuestType, pSubItem
						);
						KG_PROCESS_ERROR(hItem);
						m_treeObjectView.SetItemData(hItem, nQusetId);
						break;
					}
					pSubItem = m_treeObjectView.GetNextItem(pSubItem, TVGN_NEXT);
				}
				if (!pSubItem)
				{
					pSubItem = m_treeObjectView.InsertItem(
						szTaskLinkName, QUEST_LINK, QUEST_LINK, pItem
					);
					KG_PROCESS_ERROR(pSubItem);
					m_treeObjectView.SetItemData(pSubItem, -2);
					pSubItem = m_treeObjectView.InsertItem(
						szQuestName, nQuestType, nQuestType, pSubItem
					);
					KG_PROCESS_ERROR(pSubItem);
					m_treeObjectView.SetItemData(pSubItem, nQusetId);
				}
				break;
			}
			pItem = m_treeObjectView.GetNextItem(pItem, TVGN_NEXT);
		}
		if (!pItem)
		{
			pItem = m_treeObjectView.InsertItem(szMapName, QUEST_MAP, QUEST_MAP);
			KG_PROCESS_ERROR(pItem);
			m_treeObjectView.SetItemData(pItem, -1);
		    pItem = m_treeObjectView.InsertItem(
				szTaskLinkName, QUEST_LINK, QUEST_LINK, pItem
			);
			KG_PROCESS_ERROR(pItem);
			m_treeObjectView.SetItemData(pItem, -2);
			pItem = m_treeObjectView.InsertItem(
				szQuestName, nQuestType, nQuestType, pItem
			);
			KG_PROCESS_ERROR(pItem);
			m_treeObjectView.SetItemData(pItem, nQusetId);
		}
	}

	m_proessBar.SetPos(1);
	m_proessBar.ShowWindow(SW_HIDE);
	*/
Exit1:
	nResult = true;
Exit0:
	/*
	if (CString(szMapName) == "")
	{
		szErrorInfo.Format(
			"%s%d%s", "任务配置文件有错,第 ", nRow, " 行缺地图名!"
		);
		AfxMessageBox(szErrorInfo);
	}
	if (CString(szTaskLinkName) == "")
	{
		szErrorInfo.Format(
			"%s%d%s", "任务配置文件有错,第 ", nRow, " 行缺任务链名!"
		);
		AfxMessageBox(szErrorInfo);
	}
	if (CString(szQuestName) == "")
	{
		szErrorInfo.Format(
			"%s%d%s", "任务配置文件有错,第 ", nRow, " 行缺任务名!"
		);
		AfxMessageBox(szErrorInfo);
	}
*/
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
