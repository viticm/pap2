// KSceneSettingDialogV2.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KSceneSettingDialogV2.h"


// KSceneSettingDialogV2 dialog

IMPLEMENT_DYNAMIC(KSceneSettingDialogV2, CDialog)

KSceneSettingDialogV2::KSceneSettingDialogV2(CWnd* pParent /*=NULL*/)
	: CDialog(KSceneSettingDialogV2::IDD, pParent)
{

}

KSceneSettingDialogV2::~KSceneSettingDialogV2()
{
}

void KSceneSettingDialogV2::DoDataExchange(CDataExchange* pDX)
{
	//DDX_Control(pDX, IDC_SCENESETTING_GRID, m_GridCtrl);
	//DDX_Control(pDX, IDC_CUSTOM1, m_Grid);
	CDialog::DoDataExchange(pDX);
}

BOOL KSceneSettingDialogV2::OnInitDialog()
{
	BOOL bRet = CDialog::OnInitDialog();
	///³õÊ¼»¯Grid
	/*{
		TypeGridCtrl::InitAppearanceStruct InitStruct;
		InitStruct.m_nRowCount = 10;
		InitStruct.m_nColumnCount = 2;
		InitStruct.m_nFixedRowCount = 1;
		InitStruct.m_nFixedColumnCount = 0;
		InitStruct.m_nDefColumnWidth = 100;
		InitStruct.m_nFixedColumnWidth = 100;
		InitStruct.m_nDefRowHeight = 15;
		InitStruct.m_bEditEnable = true;
		InitStruct.m_bInitBasicTitles = false;
		InitStruct.m_bEnableHightLighFixed = false;
		InitStruct.m_bSingleRowSelection = true;
		InitStruct.m_bSingleColomnSelection = true;
		InitStruct.m_bEnableRowResize = false;
		InitStruct.m_bEnableColResize = true;
		InitStruct.m_csCornerDescription = _T("");
		m_GridCtrl.InitAppearance(&InitStruct);
	}*/

	InitGrid();
	return bRet;
}

VOID KSceneSettingDialogV2::InitGrid()
{
	// Add "About..." menu item to system menu.
#if 1
	// get the size of the place holder, this will be used when creating the grid.

	CRect rc(0,0,100,100);
	
	{
		HWND hWnd = NULL;
		this->GetDlgItem(IDC_CUSTOM1, &hWnd);
		if (NULL != hWnd)
		{
			::GetWindowRect(hWnd, &rc);
			ScreenToClient(&rc);
			::ShowWindow(hWnd, SW_HIDE);
		}
	}

	// create the property grid.
	if ( m_Grid.Create( rc, this, IDC_SCENESETTING_GRID) )
	{
		LOGFONT lf;
		GetFont()->GetLogFont( &lf );

		// create document settings category.
		CXTPPropertyGridItem* pSettings        = m_Grid.AddCategory(_T("Document Settings"));

		// add child items to category.
		CXTPPropertyGridItem* pItemSaveOnClose = pSettings->AddChildItem(new CXTPPropertyGridItemBool(_T("SaveOnClose"), TRUE));
		pSettings->AddChildItem(new CXTPPropertyGridItemFont(_T("WindowFont"), lf));
		pSettings->AddChildItem(new CXTPPropertyGridItemSize(_T("WindowSize"), CSize(100, 100)));

		pSettings->Expand();
		pItemSaveOnClose->Select();

		// create global settings category.
		CXTPPropertyGridItem* pGlobals      = m_Grid.AddCategory(_T("Global Settings"));

		// add child items to category.
		CXTPPropertyGridItem* pItemGreeting = pGlobals->AddChildItem(new CXTPPropertyGridItem(_T("Greeting Text"), _T("Welcome to your application!")));
		pGlobals->AddChildItem(new CXTPPropertyGridItemNumber(_T("ItemsInMRUList"), 4));
		CXTPPropertyGridItem* pItemRate     = pGlobals->AddChildItem(new CXTPPropertyGridItemNumber(_T("MaxRepeatRate"), 10));
		pGlobals->AddChildItem(new CXTPPropertyGridItemColor(_T("ToolbarColor"), RGB(255, 192,128)));

		pItemGreeting->SetReadOnly(TRUE);
		pItemRate->SetDescription(_T("The rate in milliseconds that the text will repeat."));

		// create version category.
		CXTPPropertyGridItem* pVersion      = m_Grid.AddCategory(_T("Version"));

		// add child items to category.
		CXTPPropertyGridItem* pItemVersion  = pVersion->AddChildItem(new CXTPPropertyGridItem(_T("AppVerion"), _T("1.0")));
		CXTPPropertyGridItem* pItemLanguage = pVersion->AddChildItem(new CXTPPropertyGridItem(IDC_COMBO1, _T("English (United States)")));

		pItemVersion->SetReadOnly(TRUE);
		pVersion->Expand();

		CXTPPropertyGridItemConstraints* pList = pItemLanguage->GetConstraints();

		pList->AddConstraint(_T("Neutral"));
		pList->AddConstraint(_T("Arabic"));
		pList->AddConstraint(_T("German"));
		pList->AddConstraint(_T("Chinese(Taiwan)"));
		pList->AddConstraint(_T("English (United Kingdom)"));
		pList->AddConstraint(_T("English (United States)"));
		pList->AddConstraint(_T("France"));
		pList->AddConstraint(_T("Russian"));

		pItemLanguage->SetFlags(xtpGridItemHasComboButton | xtpGridItemHasEdit);


		// create standard items category.
		CXTPPropertyGridItem* pStandard   = m_Grid.AddCategory(_T("Standard Items"));
		pStandard->AddChildItem(new CXTPPropertyGridItem(_T("String item")));
		pStandard->AddChildItem(new CXTPPropertyGridItemNumber(_T("Integer item")));
		pStandard->AddChildItem(new CXTPPropertyGridItemDouble(_T("Double item")));
		pStandard->AddChildItem(new CXTPPropertyGridItemColor(_T("Color item")));
		pStandard->AddChildItem(new CXTPPropertyGridItemBool(_T("Bool item")));
		pStandard->AddChildItem(new CXTPPropertyGridItemFont(_T("Font item"), lf));
		COleDateTime dates(1981, 1, 26, 0, 0, 0 );
		pStandard->AddChildItem(new CXTPPropertyGridItemDate(_T("Date item"), dates));
		pStandard->AddChildItem(new CXTPPropertyGridItemSize(_T("Size item")));

		CXTPPropertyGridItem* pItem = pStandard->AddChildItem(new CXTPPropertyGridItemEnum(_T("Enum item"), 2));
		pItem->GetConstraints()->AddConstraint(_T("Windows 98"), 1);
		pItem->GetConstraints()->AddConstraint(_T("Windows 2000"), 2);
		pItem->GetConstraints()->AddConstraint(_T("Windows XP"), 3);

		pItem = pStandard->AddChildItem(new CXTPPropertyGridItemFlags(_T("Flag item"), 1 + 2));
		pItem->GetConstraints()->AddConstraint(_T("All Windows"), 1 + 2 + 4);
		pItem->GetConstraints()->AddConstraint(_T("Windows 98"), 1);
		pItem->GetConstraints()->AddConstraint(_T("Windows 2000"), 2);
		pItem->GetConstraints()->AddConstraint(_T("Windows XP"), 4);



		// create custom items category.
		//CXTPPropertyGridItem* pCustom   = m_Grid.AddCategory(_T("Custom Items"));

		//// add child items to category.
		//CXTPPropertyGridItem* pItemIcon = pCustom->AddChildItem(new CCustomItemIcon(_T("Icon"), m_hIcon));
		//CXTPPropertyGridItem* pItemDock = pCustom->AddChildItem(new CCustomItemChilds(_T("DockPadding"), CRect(100, 20, 400, 50)));
		//pCustom->AddChildItem(new CCustomItemColor(_T("CustomCombolist"), RGB(0xFF, 0x80, 0x40)));

		//pItemIcon->SetDescription(_T("This sample shows how to override draw function"));
		//pItemDock->SetDescription(_T("This sample shows how to add item with childs"));

		//CXTPPropertyGridItem* pItemSpin = pCustom->AddChildItem(new CCustomItemSpin(_T("SpinButton")));
		//pItemSpin->SetDescription(_T("This sample shows how to add new button type"));
	}

//#ifdef _XTP_INCLUDE_CONTROLS
//	// Set control resizing.
//	SetResize(IDC_PROPERTY_GRID, SZ_TOP_LEFT, SZ_BOTTOM_RIGHT);
//
//	SetResize(IDC_GBOX_APPEAR,       SZ_TOP_RIGHT, SZ_TOP_RIGHT);
//	SetResize(IDC_CHK_TOOLBAR,       SZ_TOP_RIGHT, SZ_TOP_RIGHT);
//	SetResize(IDC_CHK_HELP,          SZ_TOP_RIGHT, SZ_TOP_RIGHT);
//	SetResize(IDC_CHK_DOUBLE,        SZ_TOP_RIGHT, SZ_TOP_RIGHT);
//	SetResize(IDC_CHK_TABITEMS,      SZ_TOP_RIGHT, SZ_TOP_RIGHT);
//	SetResize(IDC_COMBO_THEME,      SZ_TOP_RIGHT, SZ_TOP_RIGHT);
//	SetResize(IDC_GBOX_SORT,         SZ_TOP_RIGHT, SZ_TOP_RIGHT);
//	SetResize(IDC_SORT_CATEGORIES,   SZ_TOP_RIGHT, SZ_TOP_RIGHT);
//	SetResize(IDC_SORT_ALPHABETICAL, SZ_TOP_RIGHT, SZ_TOP_RIGHT);
//	SetResize(IDC_SORT_NOSORT,       SZ_TOP_RIGHT, SZ_TOP_RIGHT);
//	SetResize(IDC_GBOX_COLOR,        SZ_TOP_RIGHT, SZ_TOP_RIGHT);
//	SetResize(IDC_CHK_CUSTOMCOLORS,  SZ_TOP_RIGHT, SZ_TOP_RIGHT);
//	SetResize(IDC_BUTTON_SWITCHSTATE,  SZ_TOP_RIGHT, SZ_TOP_RIGHT);
//
//	// Load window placement
//	AutoLoadPlacement(_T("CPropertyGridDlg"));
//#endif
//
//	m_cmbTheme.AddString(_T("Default"));
//	m_cmbTheme.AddString(_T("NativeWinXP"));
//	m_cmbTheme.AddString(_T("Office2003"));
//	m_cmbTheme.AddString(_T("Cool"));
//	m_cmbTheme.AddString(_T("Simple"));
//	m_cmbTheme.AddString(_T("Delphi"));
//	m_cmbTheme.AddString(_T("Whidbey"));
//	m_cmbTheme.SetCurSel(0);
#endif

}

BEGIN_MESSAGE_MAP(KSceneSettingDialogV2, CDialog)
END_MESSAGE_MAP()


// KSceneSettingDialogV2 message handlers
