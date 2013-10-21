////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KSceneSettingDialogDWGCKeyTrack.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2007-2-9 10:22:47
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
// KSceneSettingDialogDWGCKeyTrack.cpp : implementation file
//

#include "stdafx.h"
#include "KSceneSettingDialogDWGCKeyTrack.h"
#include "SceneEditor.h"
#include "IEKG3DEnvEffDynamicWeather.h"
#include "IEKG3DEnvironment.h"
#include "KSceneSceneGridCtrl.h"
#include "KGStringFunctionsEx.h"
#include "KSceneCommonParamDialog.h"
#include "KSceneSettingPropertySheet.h"
#include "IEKG3DRegionInfoManager.h"
#include "KSceneSceneHelpers.h"
#include "KSceneCustomGridCtrl.h"
#include "KSceneCustomTreeCtrl.h"
#include "KSceneSettingPageDW.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
// KSceneSettingDialogDWGCKeyTrack dialog

IMPLEMENT_DYNAMIC(KDummyPropertyPage, CDialog)

BEGIN_MESSAGE_MAP(KDummyPropertyPage, CDialog)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

typedef KSceneCustomGridCtrl<KG3DENVEFF_DWGCKEY_HANDLE> TypeGridCtrl;
typedef KSceneCustomTreeCtrl<KG3DENVEFF_HANDLE> TypeTreeCtrl;

void KDummyPropertyPage::OnShowWindow( BOOL bShow, UINT nStatus )
{
	CDialog::OnShowWindow(bShow, nStatus);
	if (bShow)
	{
		this->OnSetActive();
	}
	else
	{
		this->OnKillActive();
	}
}

IMPLEMENT_DYNAMIC(KSceneSettingDialogDWGCKeyTrack, KSceneSettingDialogDWGCKeyTrack::TypeBase)


#define  CM_REFLESH_GRID (WM_USER + 301)
#define  CM_CHECK_WEATHER_TREE_GRID_MATCH (WM_USER + 302)

enum
{
	DWGC_KT_EditMode_Normal = 0,
	DWGC_KT_EditMode_SetKey = 1,
	DWGC_KT_EditMode_EditKey = 2,
	DWGC_KT_EditMode_EditKeyDirect = 3,
};

enum
{
	DWGC_KT_CopyMode_ByValue = 0,
	DWGC_KT_CopyMode_ByRef = 1,
};

KSceneSettingDialogDWGCKeyTrack::KSceneSettingDialogDWGCKeyTrack()
: TypeBase(KSceneSettingDialogDWGCKeyTrack::IDD)
, m_bIsDWPlaying(false)
, m_nCopyMode(DWGC_KT_CopyMode_ByValue)
, m_pGridCtrl(NULL)
, m_pTree(NULL)
, m_hWndToPropertySheet(0)
{
	m_nFlag = 0;

	try
	{
		if(NULL == m_pGridCtrl)
		{
			m_pGridCtrl = new TypeGridCtrl();
			_ASSERTE(NULL != m_pGridCtrl);
		}
		if (NULL == m_pTree)
		{
			m_pTree = new TypeTreeCtrl();
			_ASSERTE(NULL != m_pTree);
		}
	}
	catch (...)
	{
		
	}
}

KSceneSettingDialogDWGCKeyTrack::~KSceneSettingDialogDWGCKeyTrack()
{
	if (m_bIsDWPlaying)
	{
		IEKG3DEnvEffDynamicWeather* pDW = this->GetDynamicWeather();
		if (pDW)
		{
			pDW->StopQuickPlay(NULL);
		}
	}
	SAFE_DELETE(m_pGridCtrl);	
	SAFE_DELETE(m_pTree);
}

void KSceneSettingDialogDWGCKeyTrack::DoDataExchange(CDataExchange* pDX)
{
	TypeBase::DoDataExchange(pDX);
	if (NULL != m_pTree)
	{
		DDX_Control(pDX, IDC_DWGC_TREE, *m_pTree);
	}
	if(NULL != m_pGridCtrl)
	{
		DDX_Control(pDX, IDC_GRID, *m_pGridCtrl);
	}
	DDX_Control(pDX, IDC_ED_DWGC_REGION, m_edRegion);
	DDX_Control(pDX, IDC_DWGC_HOUR, m_edHour);
	DDX_Control(pDX, IDC_BN_DWGC_PLAY, m_cbPlay);
	DDX_Radio(pDX, IDC_RADIO1, m_nCopyMode);
	DDX_Control(pDX, IDC_SLIDER1, m_Slider);
}

int KSceneSettingDialogDWGCKeyTrack::Update()
{
	return 1;
}
BEGIN_MESSAGE_MAP(KSceneSettingDialogDWGCKeyTrack, KSceneSettingDialogDWGCKeyTrack::TypeBase)
	ON_BN_CLICKED(IDC_BN_DWGC_ROW_ADD, &KSceneSettingDialogDWGCKeyTrack::OnBnClickedBnDwgcRowAdd)
	ON_BN_CLICKED(IDC_BN_DWGC_ROW_DEL, &KSceneSettingDialogDWGCKeyTrack::OnBnClickedBnDwgcRowDel)
	ON_BN_CLICKED(IDC_BN_DWGC_ROW_MOD, &KSceneSettingDialogDWGCKeyTrack::OnBnClickedBnDwgcRowMod)
	ON_BN_CLICKED(IDC_BN_DWGC_ROW_DOWN, &KSceneSettingDialogDWGCKeyTrack::OnBnClickedBnDwgcRowDown)
	ON_BN_CLICKED(IDC_SS_BN_SETKEY, &KSceneSettingDialogDWGCKeyTrack::OnBnClickedSsBnSetKey)
	ON_BN_CLICKED(IDC_BN_DWGC_PLAY, &KSceneSettingDialogDWGCKeyTrack::OnBnClickedBnDwgcPlay)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_SS_BN_HELP, &KSceneSettingDialogDWGCKeyTrack::OnBnClickedSsBnHelp)
	ON_BN_CLICKED(IDC_SS_BN_SETALLKEY, &KSceneSettingDialogDWGCKeyTrack::OnBnClickedSsBnSetAllKey)
	ON_BN_CLICKED(IDC_SS_BN_COPY_PRE_KEY, &KSceneSettingDialogDWGCKeyTrack::OnBnClickedSsBnCopyPrevKey)
	ON_BN_CLICKED(IDC_SS_BN_DEL_ALL_KEY, &KSceneSettingDialogDWGCKeyTrack::OnBnClickedSsBnDelAllKey)
	ON_WM_HSCROLL()
	
END_MESSAGE_MAP()


// KSceneSettingDialogDWGCKeyTrack message handlers
BOOL KSceneSettingDialogDWGCKeyTrack::OnInitDialog()
{
	TypeBase::OnInitDialog();

	// TODO:  Add extra initialization here
#if defined(_DEBUG) | defined(DEBUG)
#if defined(KG_ENABLE_TDD)
	KSceneCustomGridCtrlTest	TestGridCtrl;
	KG_USE_ARGUMENT(TestGridCtrl);
	KSceneCustomTreeCtrlTester  TestTreeCtrl;
	KG_USE_ARGUMENT(TestTreeCtrl);
#endif
#endif

	_ASSERTE(NULL != m_pTree);
	m_pTree->SetItemHeight(16);
	int nTreeItemHeight = (int)m_pTree->GetItemHeight();
	{///顶部向下移动一点点，对其Grid第一行
		CRect rect;
		m_pTree->GetWindowRect(rect);
		this->ScreenToClient(rect);
		if(rect.Width() > nTreeItemHeight)
			rect.top += nTreeItemHeight;
		m_pTree->MoveWindow(rect, TRUE);
	}
 
	///初始化Grid
	_ASSERTE(NULL != m_pGridCtrl);
	{
		KSceneSceneGridCtrl::InitAppearanceStruct InitStruct;
		InitStruct.m_bEditEnable = true;
		InitStruct.m_bInitBasicTitles = true;
		InitStruct.m_bEnableHightLighFixed = false;
		InitStruct.m_csCornerDescription = _T("R/T");
		InitStruct.m_nColumnCount = KG3DENVEFFTI_HOUR_IN_A_DAY + 1;
		InitStruct.m_nDefRowHeight = nTreeItemHeight;
		InitStruct.m_nDefColumnWidth = 25;
		InitStruct.m_nFixedColumnCount = 1;
		InitStruct.m_nFixedColumnWidth = 100;
		InitStruct.m_nFixedRowCount = 1;
		InitStruct.m_nRowCount = 1;
		InitStruct.m_bSingleRowSelection = 1;
		InitStruct.m_bSingleColomnSelection = 1;
		InitStruct.m_bEnableColResize = false;
		InitStruct.m_bEnableRowResize = false;
		m_pGridCtrl->InitAppearance(&InitStruct);

		HWND hWndGrid = NULL;
		this->GetDlgItem(IDC_GRID, &hWndGrid);
		if (NULL != hWndGrid)
		{
			CRect rect;
			::GetWindowRect(hWndGrid, &rect);
			this->ScreenToClient(&rect);

			m_pGridCtrl->MoveWindow(&rect, TRUE);
		}
	}

	//Slider控件
	m_Slider.SetRange(KG3DENVEFFTI_HOUR_MIN, KG3DENVEFFTI_HOUR_MAX);
	m_Slider.SetPos(KG3DENVEFFTI_HOUR_MIN);
	
	for (int i = KG3DENVEFFTI_HOUR_MIN + 1; i < KG3DENVEFFTI_HOUR_MAX; ++i)
	{
		m_Slider.SetTic(i);
	}
	m_Slider.SetTicFreq(1);

	//Tree加入选择图标
	m_ImageList.Create(IDB_BITMAP_ARROW, 16, 1, RGB(255,255,255));
	m_pTree->SetImageList(&m_ImageList, TVSIL_NORMAL);


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL KSceneSettingDialogDWGCKeyTrack::OnSetActive()
{
	// TODO: Add your specialized code here and/or call the base class
	
	HRESULT hr = E_FAIL;

	//为了下面展开左边Tree和右边Grid，要先复制一次状态，不然一加载，状态就乱了
	KeyTrackStateStruct TempStateStruct = m_KeyTrackStateStruct;

	IEKG3DEnvEffDynamicWeather* pDW = this->GetDynamicWeather();
	KG_PROCESS_ERROR(pDW);

	//读入左边的Tree和右边的Grid
	{
		//卸载回调函数
		BindCallBacks(FALSE);
		ClearContents();///注意这两个函数的顺序

		ReloadAllEnvEff();//在Reload的时候会触发OnSeletect，就会更新右边的Grid
	}
	
	//一些不允许更改的按钮
	static INT UnModifiableIDs[] = {IDC_SS_BN_SETKEY, IDC_ED_DWGC_REGION, IDC_DWGC_HOUR};
	{
		KSCENEEDITOR_HELPERCLASSES::LockChildWindows(this->GetSafeHwnd(), FALSE, UnModifiableIDs, _countof(UnModifiableIDs));
		UpdateData(FALSE);
	}

	{
		m_cbPlay.SetWindowText(_T("播放"));
		m_cbPlay.SetCheck(BST_UNCHECKED);
		_ASSERTE(false == m_bIsDWPlaying);
	}

	//在编辑关键帧状态下面，禁止关键帧的播放
	pDW->EnablePlay(FALSE);

	BindCallBacks(TRUE);

	//根据保留的State，展开左边的Tree和右边的Grid
	if(TempStateStruct.m_bIsValid)
	{
		_ASSERTE(NULL != m_pTree);
		HTREEITEM hItem = static_cast<TypeTreeCtrl*>(m_pTree)->GetHandleOfBigData(TempStateStruct.m_Handle);
		if(hItem)
		{
			m_pTree->EnsureVisible(hItem);
			m_pTree->SelectItem(hItem);	//会触发SelChange从而刷新帧表
		}
		INT nRowToSelectInGrid = -1;
		for (int i = 1; i < m_pGridCtrl->GetRowCount(); ++i)
		{
			KG3DENVEFF_DWGCKEY_HANDLE* p = static_cast<TypeGridCtrl*>(m_pGridCtrl)->GetItemBigData(i, 0);
			if (p)
			{
				if(p->m_Region == TempStateStruct.m_KeyPos.m_Region)
				{
					nRowToSelectInGrid = i;
					break;
				}
			}
		}
		INT nHourToSelect = TempStateStruct.m_KeyPos.m_Hour;
		if (nRowToSelectInGrid > 0)
		{
			m_pGridCtrl->SetSelectedRange(nRowToSelectInGrid, nHourToSelect
				, nRowToSelectInGrid + 1, nHourToSelect + 1, TRUE);
		}

		{
			CString TempStr;
			KG_CUSTOM_HELPERS::TValueToString(nRowToSelectInGrid, TempStr);
			m_edRegion.SetWindowText(TempStr);
			KG_CUSTOM_HELPERS::TValueToString(nHourToSelect, TempStr);
			m_edHour.SetWindowText(TempStr);
		}
	}
	
	m_pGridCtrl->ShowWindow(SW_SHOW);
	
	this->SetTimer(0, 500, NULL);	//在Timmer中同步Slider和DW
	this->SetTimer(1, 500, NULL);

	return TypeBase::OnSetActive();
Exit0:
	BindCallBacks(TRUE);
	BOOL bResult = TypeBase::OnSetActive();
	KSCENEEDITOR_HELPERCLASSES::LockChildWindows(this->GetSafeHwnd(), TRUE, UnModifiableIDs, _countof(UnModifiableIDs));///锁定所有子窗口
	MessageBox("Dynamic Weather不存在");
	return bResult;
}

BOOL KSceneSettingDialogDWGCKeyTrack::OnKillActive()
{
	// TODO: Add your specialized code here and/or call the base class
	BindCallBacks(FALSE);
	ClearContents();

	IEKG3DEnvEffDynamicWeather* pDW = this->GetDynamicWeather();
	if (pDW)
	{
		pDW->StopQuickPlay(NULL);
		pDW->EnablePlay(TRUE);
	}
	m_bIsDWPlaying = false;

	this->KillTimer(0);
	this->KillTimer(1);

	m_KeyTrackStateStruct.m_bIsValid = TRUE;

	return TypeBase::OnKillActive();
}

VOID KSceneSettingDialogDWGCKeyTrack::BindCallBacks( BOOL bBind )
{
	BindGridCallbacks(bBind);
	BindTreeCallbacks(bBind);
}
VOID KSceneSettingDialogDWGCKeyTrack::BindTreeCallbacks( BOOL bBind )
{
	if (bBind)
	{
		_ASSERTE(NULL != m_pTree);
		m_pTree->SetEventHandler(this);
	}
	else
	{
		_ASSERTE(NULL != m_pTree);
		m_pTree->SetEventHandler(NULL);
	}
}
VOID KSceneSettingDialogDWGCKeyTrack::BindGridCallbacks( BOOL bBind )
{
	if (bBind)
	{
		m_pGridCtrl->SetEventHandler(this);
	}
	else
	{
		m_pGridCtrl->SetEventHandler(NULL);
	}
}
void KSceneSettingDialogDWGCKeyTrack::GridReloadKey( INT nRow, INT nColumn)
{
	_ASSERTE(nRow >= 1 && nRow < m_pGridCtrl->GetRowCount());

	KG3DENVEFF_DWGCKEY_HANDLE* pKeyHandle = static_cast<TypeGridCtrl*>(m_pGridCtrl)->GetItemBigData(nRow, 0);
	KG_PROCESS_ERROR(pKeyHandle);
	{
		_ASSERTE(NULL != m_pTree);
		HTREEITEM hItemSelected = static_cast<TypeTreeCtrl*>(m_pTree)->GetSelectedItem();
		KG3DENVEFF_HANDLE* pEnvEffHandle = static_cast<TypeTreeCtrl*>(m_pTree)->GetItemBigData(hItemSelected);
		KG_PROCESS_ERROR(pEnvEffHandle);

		m_pGridCtrl->ClearRow(nRow, FALSE);

		IEKG3DEnvEffDynamicWeather* pDW = GetDynamicWeather();
		KG_PROCESS_ERROR(pDW);

		INT Region = pKeyHandle->m_Region;
		
		KG3DENVEFF_DWGCKEY_HANDLE KeyHandle;
		size_t KeyInfo = 0;
		HRESULT hr = pDW->TL_GetFirstKeyHandleOfRegion(*pEnvEffHandle, Region, &KeyHandle, reinterpret_cast<LPVOID>(&KeyInfo));
		while (SUCCEEDED(hr))
		{
			_ASSERTE(KeyHandle.m_Region == Region);
			this->SetGridKey(nRow, KeyHandle.m_Hour, KeyInfo);

			hr = pDW->TL_GetNextKeyHandleOfRegion(*pEnvEffHandle, Region, KeyHandle, &KeyHandle, reinterpret_cast<LPVOID>(&KeyInfo));
		}
	}

Exit0:
	m_pGridCtrl->RedrawRow(nRow);
	return;	
}

BOOL KSceneSettingDialogDWGCKeyTrack::IsEditStringValid( KSceneSceneGridCtrl& ctrl, CPoint point, LPCTSTR pStrEdit )
{
	INT nRow = point.x;
	INT nColumn = point.y;

	size_t uStringLen = _tcslen(pStrEdit);
	HRESULT hr = E_FAIL;
	KG_PROCESS_ERROR(uStringLen <= 20);
	{
		KG_PROCESS_ERROR(0 == _tcscmp(pStrEdit, _T("")));//Edit的情况只允许Del
		this->GridDelKey(nRow, nColumn);	
		return TRUE;
	}
Exit0:
	return FALSE;
}

BOOL KSceneSettingDialogDWGCKeyTrack::IsEditCharValid( KSceneSceneGridCtrl& ctrl, CPoint point, UINT uChar )
{
	return FALSE;
}

BOOL KSceneSettingDialogDWGCKeyTrack::GridDelKey( INT Row, INT Column )
{
	HRESULT hr = E_FAIL;
	{
		IEKG3DEnvEffDynamicWeather* pDW = this->GetDynamicWeather();
		KG_PROCESS_ERROR(pDW);

		KG3DENVEFF_DWGCKEY_HANDLE* pKeyHandle = static_cast<TypeGridCtrl*>(m_pGridCtrl)->GetItemBigData(Row, 0);
		KG_PROCESS_ERROR(pKeyHandle);

		_ASSERTE(NULL != m_pTree);
		HTREEITEM hItemSelected = static_cast<TypeTreeCtrl*>(m_pTree)->GetSelectedItem();
		KG3DENVEFF_HANDLE* pEnvEffHandle = static_cast<TypeTreeCtrl*>(m_pTree)->GetItemBigData(hItemSelected);
		KG_PROCESS_ERROR(pEnvEffHandle);

		hr = pDW->TL_DelKey(*pEnvEffHandle, KG3DENVEFF_DWGCKEY_HANDLE(pKeyHandle->m_Region, Column));
		KG_COM_PROCESS_ERROR(hr);

		GridReloadKey(Row, Column);

		return TRUE;
	}
Exit0:
	return FALSE;
}

BOOL KSceneSettingDialogDWGCKeyTrack::IsLButtomDoubleClickValid( KSceneSceneGridCtrl& ctrl, CPoint CellPos )
{
	INT nRow = CellPos.x;
	INT nColumn = CellPos.y;
	_KG3D_DEBUG_SELF_OUTPUT2("点击了以下Cell: %d, %d", CellPos.x, CellPos.y);
	
	{
		IEKG3DEnvEffDynamicWeather* pDW = this->GetDynamicWeather();
		KG_PROCESS_ERROR(pDW);

		KG3DENVEFF_DWGCKEY_HANDLE* pKeyHandle = static_cast<TypeGridCtrl&>(ctrl).GetItemBigData(nRow, 0);
		KG_PROCESS_ERROR(pKeyHandle);
		INT nHour = nColumn;

		_ASSERTE(NULL != m_pTree);
		HTREEITEM hItemSelected = static_cast<TypeTreeCtrl*>(m_pTree)->GetSelectedItem();
		KG3DENVEFF_HANDLE* pEnvEffHandle = static_cast<TypeTreeCtrl*>(m_pTree)->GetItemBigData(hItemSelected);
		KG_PROCESS_ERROR(pEnvEffHandle);

		HRESULT hr = pDW->KeyToState(KG3DENVEFF_DWGCKEY_HANDLE(pKeyHandle->m_Region, nHour), *pEnvEffHandle);
		KG_COM_PROCESS_ERROR(hr);

		_ASSERTE(this->m_pGridCtrl->GetItemText(nRow, nColumn) != _T(""));
	}
Exit0:
	return FALSE;
}

void KSceneSettingDialogDWGCKeyTrack::OnGridEndEdit( NMHDR *pNotifyStruct,  LRESULT* pResult )
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;

	CString tempString = m_pGridCtrl->GetItemText(pItem->iRow, pItem->iColumn);
	static int nInValidStringLength = std::numeric_limits<INT>::digits10;
	int nStringLength = tempString.GetLength();
	if (nStringLength > nInValidStringLength)
	{
		*pResult = -1;
		return;
	}
	int nRet = _ttoi((LPCTSTR)tempString);

	*pResult = 0;
}

BOOL KSceneSettingDialogDWGCKeyTrack::OnItemExpanded( KSceneSceneTreeCtrl& tree, HTREEITEM hItemExpander )
{
	return TRUE;
}

BOOL KSceneSettingDialogDWGCKeyTrack::OnItemDelete( KSceneSceneTreeCtrl& tree, HTREEITEM hItemDelete )
{
	return TRUE;
}

BOOL KSceneSettingDialogDWGCKeyTrack::OnItemInserted( KSceneSceneTreeCtrl& tree, HTREEITEM hItemInserted )
{
	HTREEITEM hParentItem = tree.GetParentItem(hItemInserted);
	if (!hParentItem)
		hParentItem = TVI_ROOT;
	KG_PROCESS_SUCCESS(hParentItem != TVI_ROOT && !tree.IsItemExpanded(hParentItem));
	{
		INT nPos = tree.GetItemExpandedPos(hItemInserted);
		KG_PROCESS_SUCCESS(nPos < 0);

		KSceneSettingDialogDWGCKeyTrack* pDialog = this;
		INT nRowCount = pDialog->m_pGridCtrl->GetRowCount();
		_ASSERTE(nPos <= nRowCount && pDialog);
		if (nPos == nRowCount)///注意Grid的第一行是标题
		{	
			pDialog->m_pGridCtrl->AddRow(-1);		
		}
		else
		{
			pDialog->m_pGridCtrl->AddRow(nPos);
		}
		_ASSERTE(tree.GetExpandedTreeItemCount()+1 == pDialog->m_pGridCtrl->GetRowCount());

		struct TempVisitor : TreeItemVisitor 
		{
			KSceneSettingDialogDWGCKeyTrack* m_pTrack;
			virtual HRESULT Accept(HTREEITEM hItem)
			{
				m_pTrack->OnEnumTree(hItem);
				return S_OK;
			}
		};
		TempVisitor visitor;
		visitor.m_pTrack = this;

		tree.TraverseItemsExpanded(hParentItem, visitor);
	}

Exit1:
	return TRUE;
}

VOID KSceneSettingDialogDWGCKeyTrack::OnEnumTree( HTREEITEM hItem )
{
	return;
}

BOOL KSceneSettingDialogDWGCKeyTrack::OnItemSelectChange( KSceneSceneTreeCtrl& tree, HTREEITEM hItemSeleted, HTREEITEM hItemLastSelect )
{
	_ASSERTE(NULL != m_pGridCtrl);
	//得到需要的控制器
	TypeTreeCtrl* pTree = static_cast<TypeTreeCtrl*>(&tree);
	_ASSERTE(pTree);
	KG3DENVEFF_HANDLE* pCurTreeItemEnvHandle = pTree->GetItemBigData(hItemSeleted);
	KG_PROCESS_ERROR(pCurTreeItemEnvHandle);

	{

		//把选择记录下来
		{
			m_KeyTrackStateStruct.m_Handle = *pCurTreeItemEnvHandle;
		}


		IEKG3DEnvEffDynamicWeather* pDW = GetDynamicWeather();
		KG_PROCESS_ERROR(pDW);

		m_pGridCtrl->SetRowCount(1);//这样就清空了所有内容了

		KG_PROCESS_ERROR(pDW->IsEffectDWGCAvailable(*pCurTreeItemEnvHandle));

		std::map<INT, INT> MapRegionRow;	//用于下面填Key的时候容易找到对应的行
		//在Grid的左边第一列，加入Region的序列号，并在Data域加入标志	
		{
			ULONG uRegionsCount = pDW->TL_GetRegionIndicesCount();
			_ASSERTE(uRegionsCount < 256);
			KG_PROCESS_ERROR(m_pGridCtrl->SetRowCount((int)uRegionsCount + 1));

			INT nRegionIndex = 0;
			ULONG AddCount = 0;
			TCHAR	tczItemText[MAX_PATH];
			ZeroMemory(tczItemText, sizeof(tczItemText));
			HRESULT hr = pDW->TL_GetFirstRegionIndex(&nRegionIndex);
			while(SUCCEEDED(hr))
			{
				++AddCount;
				_ASSERTE(AddCount <= uRegionsCount);

				_stprintf_s(tczItemText, "R_%u", nRegionIndex);

				m_pGridCtrl->SetItemText(AddCount, 0, tczItemText);
				static_cast<TypeGridCtrl*>(m_pGridCtrl)->SetItemBigData(AddCount, 0, KG3DENVEFF_DWGCKEY_HANDLE(nRegionIndex, KG3DENVEFFTI_HOUR_INVALID));

				MapRegionRow[nRegionIndex] = AddCount;

				hr = pDW->TL_GetNextRegionIndex(nRegionIndex, &nRegionIndex);
			}
		}

		//填入Key
		{
			KG3DENVEFF_DWGCKEY_HANDLE	KeyHandle;
			size_t KeyInfo = 0;//实际是KeyTable内部分配的序列号

			HRESULT hr = pDW->TL_GetFirstKeyHandle(*pCurTreeItemEnvHandle, &KeyHandle, (LPVOID)(&KeyInfo));
			while (SUCCEEDED(hr))
			{
				//找到要插入的Row
				INT nRowToInsert = MapRegionRow[KeyHandle.m_Region];
				_ASSERTE(nRowToInsert > 0);

#if defined(DEBUG) | defined(_DEBUG)
				{
					KG3DENVEFF_DWGCKEY_HANDLE* pKeyRowHead = static_cast<TypeGridCtrl*>(m_pGridCtrl)->GetItemBigData(nRowToInsert, 0);
					_ASSERTE(pKeyRowHead);
					_ASSERTE(pKeyRowHead->m_Region == KeyHandle.m_Region);
				}
#endif
				SetGridKey(nRowToInsert,  KeyHandle.m_Hour, KeyInfo);

				hr = pDW->TL_GetNextKeyHandle(*pCurTreeItemEnvHandle, KeyHandle, &KeyHandle, (LPVOID)(&KeyInfo));
			}
			m_pGridCtrl->RedrawWindow();
		}
		//一些额外的界面操作
		_ASSERTE(NULL != m_pTree);
		KG3DENVEFF_HANDLE* p = static_cast<TypeTreeCtrl*>(m_pTree)->GetItemBigData(hItemSeleted);
		if (p)
		{			
			CString Cs = _T("R/T ");
			Cs += KSH::GetRTTITypeName(*p);

			m_pGridCtrl->SetItemText(0,0,Cs);
			m_pGridCtrl->RedrawCell(0,0);
		}

	}

Exit0:
	return TRUE;
}

BOOL KSceneSettingDialogDWGCKeyTrack::IsLButtonDownValid( KSceneSceneGridCtrl& ctrl, CPoint CellPos )
{
	ctrl.HighLightRow(static_cast<INT>(CellPos.x));

	//记录选择的位置
	KG3DENVEFF_DWGCKEY_HANDLE* p = static_cast<TypeGridCtrl&>(ctrl).GetItemBigData(CellPos.x, 0);

	if (NULL != p)
	{
		this->m_KeyTrackStateStruct.m_KeyPos = KG3DENVEFF_DWGCKEY_HANDLE(p->m_Region, CellPos.y);
	}


	if (CellPos.y == 0)//选最前面的话，SetRegion
	{
		if (NULL != p)
		{
			IEKG3DEnvEffDynamicWeather* pDW = this->GetDynamicWeather();
			pDW->SetCurRegion(p->m_Region, ! this->IsBlendingNeededToBeShown());
		}
	}

	return TRUE;
}

BOOL KSceneSettingDialogDWGCKeyTrack::OnItemExpanding( KSceneSceneTreeCtrl& tree, HTREEITEM hItemExpanding )
{
	tree.SelectItem(hItemExpanding);
	return TRUE;
}

BOOL KSceneSettingDialogDWGCKeyTrack::IsCopyValid( KSceneSceneGridCtrl& ctrl, CPoint CellPos )
{
	KG_PROCESS_ERROR(ctrl.GetItemText(CellPos.x, CellPos.y) != _T(""));
	{
		KG3DENVEFF_DWGCKEY_HANDLE* pKeyHandle = static_cast<TypeGridCtrl&>(ctrl).GetItemBigData(CellPos.x, 0);
		KG_PROCESS_ERROR(pKeyHandle);

		HTREEITEM hItemSelected = static_cast<TypeTreeCtrl*>(m_pTree)->GetSelectedItem();
		KG3DENVEFF_HANDLE* pEnvEffHandle = static_cast<TypeTreeCtrl*>(m_pTree)->GetItemBigData(hItemSelected);
		KG_PROCESS_ERROR(pEnvEffHandle);

		KeyTrackCopyStruct& CopyStruct = this->GetCopyStruct();
		CopyStruct.m_HandleSrc = *pEnvEffHandle;
		CopyStruct.m_KeySrc = KG3DENVEFF_DWGCKEY_HANDLE(pKeyHandle->m_Region, CellPos.y);
		CopyStruct.m_bIsValid = true;
		CopyStruct.m_bCopyOrCut = true;
		return FALSE;
	}
Exit0:
	this->GetCopyStruct().m_bIsValid = false;
	return FALSE;//不让默认的OnEditCopy运行，下面相同
}

BOOL KSceneSettingDialogDWGCKeyTrack::IsCutValid( KSceneSceneGridCtrl& ctrl, CPoint CellPos )
{
	KG_PROCESS_ERROR(ctrl.GetItemText(CellPos.x, CellPos.y) != _T(""));
	{
		KG3DENVEFF_DWGCKEY_HANDLE* pKeyHandle = static_cast<TypeGridCtrl&>(ctrl).GetItemBigData(CellPos.x, 0);
		KG_PROCESS_ERROR(pKeyHandle);

		_ASSERTE(NULL != m_pTree);
		HTREEITEM hItemSelected = static_cast<TypeTreeCtrl*>(m_pTree)->GetSelectedItem();
		KG3DENVEFF_HANDLE* pEnvEffHandle = static_cast<TypeTreeCtrl*>(m_pTree)->GetItemBigData(hItemSelected);
		KG_PROCESS_ERROR(pEnvEffHandle);

		KeyTrackCopyStruct& CopyStruct = this->GetCopyStruct();
		CopyStruct.m_HandleSrc = *pEnvEffHandle;
		CopyStruct.m_KeySrc = KG3DENVEFF_DWGCKEY_HANDLE(pKeyHandle->m_Region, CellPos.y);
		CopyStruct.m_bIsValid = true;
		CopyStruct.m_bCopyOrCut = false;

		return FALSE;
	}
Exit0:
	this->GetCopyStruct().m_bIsValid = false;
	return FALSE;//不让默认的OnEditCopy运行
}

BOOL KSceneSettingDialogDWGCKeyTrack::IsPasteValid( KSceneSceneGridCtrl& ctrl, CPoint CellPos )
{
	IEKG3DEnvEffDynamicWeather* pDW = this->GetDynamicWeather();
	KG_PROCESS_ERROR(pDW);
	{
		KeyTrackCopyStruct& CopyStruct = this->GetCopyStruct();
		KG_PROCESS_ERROR(CopyStruct.m_bIsValid);

		_ASSERTE(NULL != m_pTree);

		_ASSERTE(NULL != m_pTree);
		HTREEITEM hItemSelected = static_cast<TypeTreeCtrl*>(m_pTree)->GetSelectedItem();
		KG3DENVEFF_HANDLE* pHandleTo = static_cast<TypeTreeCtrl*>(m_pTree)->GetItemBigData(hItemSelected);
		KG_PROCESS_ERROR(pHandleTo);

		KG3DENVEFF_DWGCKEY_HANDLE* pRegionHandle = static_cast<TypeGridCtrl&>(ctrl).GetItemBigData(CellPos.x, 0);
		KG_PROCESS_ERROR(pRegionHandle);

		this->UpdateData();
		INT CopyByValueOrByRef = this->m_nCopyMode == DWGC_KT_CopyMode_ByValue ? TRUE : FALSE;
		INT Hour = CellPos.y;
		HRESULT hr = pDW->TL_CopyKeyTo(CopyStruct.m_HandleSrc
			, CopyStruct.m_KeySrc
			, *pHandleTo
			, KG3DENVEFF_DWGCKEY_HANDLE(pRegionHandle->m_Region, Hour)
			, CopyByValueOrByRef);
		if(FAILED(hr))
		{
			this->MessageBox(_T("关键帧粘贴失败：可能如下\n\
								   1.位置上已经存在关键帧，要先删除，因为有引用拷贝的存在，程序不会自动覆盖原有关键帧\n\
								   2.不能拷贝类型的关键帧，但同类型的可以在不同的关键帧表中相互拷贝\n\
								   3.位置不合法，检查Region，Hour"));
		}
		else
		{
			if (! CopyStruct.m_bCopyOrCut)//如果是Cut的话，删除原来的
			{
				pDW->TL_DelKey(CopyStruct.m_HandleSrc, CopyStruct.m_KeySrc);

			}
		}


		this->GridReloadKey(CellPos.x, CellPos.y);
		return FALSE;
	}
Exit0:
	this->GetCopyStruct().m_bIsValid = false;
	this->GridReloadKey(CellPos.x, CellPos.y);
	return FALSE;
}

LRESULT KSceneSettingDialogDWGCKeyTrack::WindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{	
	return TypeBase::WindowProc(message, wParam, lParam);
}

VOID KSceneSettingDialogDWGCKeyTrack::ClearContents()
{
	_ASSERTE(NULL != m_pGridCtrl);
	m_pGridCtrl->SetRowCount(1);
	_ASSERTE(NULL != m_pTree);
	m_pTree->DeleteAllItems();
	m_vecEnvEffHandle.clear();
	{
		KG3DENVEFF_HANDLE BlankHandle;
		BlankHandle.m_dwType = 0;
		BlankHandle.m_nIndex = 0;
		m_vecEnvEffHandle.push_back(BlankHandle);///作为无效索引
	}
}
IEKG3DEnvEffDynamicWeather* KSceneSettingDialogDWGCKeyTrack::GetDynamicWeather()
{
	return KSH::GetEngineInterface<KG3DTYPE_ENVEFF_DYNAMIC_WEATHER>();
}

//////////////////////////////////////////////////////////////////////////


VOID KSceneSettingDialogDWGCKeyTrack::SetGridKey( int nRow, int nColumn, INT nValue, LPCTSTR pText/* = NULL*/ )
{
	_ASSERTE(NULL != m_pGridCtrl);
	_ASSERTE(nRow >= 0 && nRow < m_pGridCtrl->GetRowCount() && nColumn >= 0 && nColumn < m_pGridCtrl->GetColumnCount());
	if (pText)
	{
		m_pGridCtrl->SetItemText(nRow, nColumn, pText);
	}
	else
	{
		TCHAR StringBuffer[64];
		if (KG_CUSTOM_HELPERS::TValueToString(nValue, StringBuffer, _countof(StringBuffer)))
		{
			m_pGridCtrl->SetItemText(nRow, nColumn, StringBuffer);
		}
		
	}	
	m_pGridCtrl->SetItemData(nRow, nColumn, (LPARAM)nValue);
	m_pGridCtrl->MarkCell(nRow, nColumn);
}

VOID KSceneSettingDialogDWGCKeyTrack::SetGridKey( INT Row, INT Column, size_t Info )
{
	_ASSERTE(NULL != m_pGridCtrl);
	TCHAR tczItemText[MAX_PATH];
	KG_CUSTOM_HELPERS::TValueToString((UINT)Info, tczItemText);
	m_pGridCtrl->SetItemText(Row, Column, tczItemText);
	m_pGridCtrl->MarkCell(Row, Column);
}

void KSceneSettingDialogDWGCKeyTrack::OnBnClickedBnDwgcRowAdd()
{
	// TODO: Add your control notification handler code here
	KSceneCommonParamDialog CTDialog;
	INT nRegionIndex = 0;
	INT nParamIndex = CTDialog.AddAnParam(_T("添加(1-255)："), nRegionIndex,TRUE);
	if (CTDialog.DoModal() != IDOK)
		return;  
	if (! CTDialog.GetData(nParamIndex, nRegionIndex))
		return;
	IEKG3DEnvEffDynamicWeather* pDW = GetDynamicWeather();
	if(! pDW)
		return;
	pDW->TL_AddRegionIndex(nRegionIndex);
	//FuncTreeItemSelectedChangeCallback(&m_Tree, m_Tree.GetSelectedItem(), NULL, reinterpret_cast<DWORD_PTR>(this));//强制要求重读
	_ASSERTE(NULL != m_pTree);
	this->OnItemSelectChange(*m_pTree, m_pTree->GetSelectedItem(), NULL);
}

void KSceneSettingDialogDWGCKeyTrack::OnBnClickedBnDwgcRowDel()
{
	_ASSERTE(NULL != m_pGridCtrl);
	INT Row = m_pGridCtrl->GetSelectedMinRow();
	KG_PROCESS_ERROR(Row > 0 && Row < m_pGridCtrl->GetRowCount());//第一行是不能删除的
	{
		KG3DENVEFF_DWGCKEY_HANDLE* pKeyHandle = static_cast<TypeGridCtrl*>(m_pGridCtrl)->GetItemBigData(Row, 0);
		KG_PROCESS_ERROR(pKeyHandle);

		IEKG3DEnvEffDynamicWeather* pDW = this->GetDynamicWeather();
		KG_PROCESS_ERROR(pDW);

		HRESULT hr = pDW->TL_DelRegionIndex(pKeyHandle->m_Region);
		KG_COM_PROCESS_ERROR(hr);

		_ASSERTE(NULL != m_pTree);
		this->OnItemSelectChange(*m_pTree, m_pTree->GetSelectedItem(), NULL);
	}
Exit0:
	return;
}

void KSceneSettingDialogDWGCKeyTrack::OnBnClickedBnDwgcRowMod()
{
	// TODO: Add your control notification handler code here
	_ASSERTE(NULL != m_pGridCtrl);
	INT Row = m_pGridCtrl->GetSelectedMinRow();
	KG_PROCESS_ERROR(Row > 0 && Row < m_pGridCtrl->GetRowCount());//第一行是不能删除的
	{
		KG3DENVEFF_DWGCKEY_HANDLE* pKeyHandle = static_cast<TypeGridCtrl*>(m_pGridCtrl)->GetItemBigData(Row, 0);
		KG_PROCESS_ERROR(pKeyHandle);

		IEKG3DEnvEffDynamicWeather* pDW = this->GetDynamicWeather();
		KG_PROCESS_ERROR(pDW);

		INT nRegionIndexNew = 0;
		{
			KSceneCommonParamDialog CTDialog;
			nRegionIndexNew = pKeyHandle->m_Region;
			INT nParamIndex = CTDialog.AddAnParam(_T("改Region的Index："), nRegionIndexNew,TRUE);
			KG_PROCESS_SUCCESS(CTDialog.DoModal() != IDOK);
			KG_PROCESS_ERROR(CTDialog.GetData(nParamIndex, nRegionIndexNew));
			KG_PROCESS_ERROR(nRegionIndexNew != 0);
			KG_PROCESS_ERROR(nRegionIndexNew != pKeyHandle->m_Region);
		}

		HRESULT hr = pDW->TL_ModifyRegionIndex(pKeyHandle->m_Region, nRegionIndexNew);
		KG_COM_PROCESS_ERROR(hr);

		//FuncTreeItemSelectedChangeCallback(&m_Tree, m_Tree.GetSelectedItem(), NULL, reinterpret_cast<DWORD_PTR>(this));
		_ASSERTE(NULL != m_pTree);
		this->OnItemSelectChange(*m_pTree, m_pTree->GetSelectedItem(), NULL);
	}
Exit1:
Exit0:
	return;
}

void KSceneSettingDialogDWGCKeyTrack::OnBnClickedBnDwgcRowDown()//从分块表读取
{
	// TODO: Add your control notification handler code here
	IEKG3DRegionInfoManager* pRIM = KSH::GetEngineInterface<KG3DTYPE_REGION_INFO_MANAGER>();
	IEKG3DEnvEffDynamicWeather* pDW =  this->GetDynamicWeather();
	if (!pRIM || !pDW)
		return;
	INT nDiffArrayCount = 0;
	DWORD* pDiffBuffer = NULL;
	HRESULT hr = pRIM->GetDiffArrayCount(&nDiffArrayCount);
	KG_COM_PROCESS_ERROR(hr);
	{
		_ASSERTE(nDiffArrayCount >= 0 && nDiffArrayCount < 1024);
		pDiffBuffer = new DWORD[nDiffArrayCount];
		HRESULT hr = pRIM->GetDiffArray(pDiffBuffer, nDiffArrayCount);
		KG_COM_PROCESS_ERROR(hr);
		
		
		INT nRegionCount = pDW->TL_GetRegionIndicesCount();
		if (nDiffArrayCount+nRegionCount > KG3DENVEFFRI_REGION_MAX)
		{
			MessageBox("分块图中的分块数全部添加之后比最大允许的区域分块数还要多，可能只能部分添加，\
请考虑修改分块图减少分块数，或者删除当前的分块");
		}
		for (int i = 0; i < nDiffArrayCount; i++)
		{
			pDW->TL_AddRegionIndex(pDiffBuffer[i]);
		}
		_ASSERTE(NULL != m_pTree);
		this->OnItemSelectChange(*m_pTree, m_pTree->GetSelectedItem(), NULL);//强制要求重读
		SAFE_DELETE_ARRAY(pDiffBuffer);
		return;
	}
Exit0:
	SAFE_DELETE_ARRAY(pDiffBuffer);
	return;
}

void KSceneSettingDialogDWGCKeyTrack::OnBnClickedSsBnSetKey()
{
	// TODO: Add your control notification handler code here	
	IEKG3DEnvEffDynamicWeather* pDW = GetDynamicWeather();
	KG_PROCESS_ERROR(pDW);

	{
		_ASSERTE(NULL != m_pGridCtrl);
		CCellID cellID = m_pGridCtrl->GetFocusCell();

		INT nRow = cellID.row;
		INT nColumn = cellID.col;
		KG3DENVEFF_DWGCKEY_HANDLE* pKeyHandle = static_cast<TypeGridCtrl*>(m_pGridCtrl)->GetItemBigData(nRow, 0);
		KG_PROCESS_ERROR(pKeyHandle);
		INT nHour = nColumn;
		INT nRegion = pKeyHandle->m_Region;

		_ASSERTE(NULL != m_pTree);
		HTREEITEM hItemSelected = static_cast<TypeTreeCtrl*>(m_pTree)->GetSelectedItem();
		KG3DENVEFF_HANDLE* pEnvEffHandle = static_cast<TypeTreeCtrl*>(m_pTree)->GetItemBigData(hItemSelected);
		KG_PROCESS_ERROR(pEnvEffHandle);

		HRESULT hr = pDW->StateToKey(*pEnvEffHandle, KG3DENVEFF_DWGCKEY_HANDLE(nRegion, nHour));
		this->GridReloadKey(nRow,  nColumn);//不管怎样都应该Reload

		KG_COM_PROCESS_ERROR(hr);
	}
Exit0:
	return;

}

void KSceneSettingDialogDWGCKeyTrack::OnBnClickedSsBnSetAllKey()
{
	IEKG3DEnvEffDynamicWeather* pDW = GetDynamicWeather();
	{
		_ASSERTE(NULL != m_pGridCtrl);
		CCellID cellID = m_pGridCtrl->GetFocusCell();

		INT nRow = cellID.row;
		INT nColumn = cellID.col;
		KG3DENVEFF_DWGCKEY_HANDLE* pKeyHandle = static_cast<TypeGridCtrl*>(m_pGridCtrl)->GetItemBigData(nRow, 0);
		KG_PROCESS_ERROR(pKeyHandle);
		INT nHour = nColumn;
		INT nRegion = pKeyHandle->m_Region;

		struct Visitor : public KSH::CTreeCtrlVisitor 
		{
			IEKG3DEnvEffDynamicWeather* m_pDW;
			KG3DENVEFF_DWGCKEY_HANDLE KeyHandle;
			std::vector<KG3DENVEFF_HANDLE> m_vecFailedEffect;
#if defined(DEBUG) | defined(_DEBUG)
			UINT m_VisitedItemCount;
#endif
			
			void Accept(CTreeCtrl& Tree, HTREEITEM hItem)
			{
				_ASSERTE(m_pDW && hItem != TVI_ROOT);
#if defined(DEBUG) | defined(_DEBUG)
				++m_VisitedItemCount;
#endif
				KG3DENVEFF_HANDLE* pEnvEffHandle = (dynamic_cast<TypeTreeCtrl&>(Tree)).GetItemBigData(hItem);
				HRESULT hr = E_FAIL;
				KG_PROCESS_ERROR(pEnvEffHandle);
				KG_PROCESS_SUCCESS(! m_pDW->IsEffectDWGCAvailable(*pEnvEffHandle));
				hr = m_pDW->StateToKey(*pEnvEffHandle, KeyHandle);
				KG_COM_PROCESS_ERROR(hr);
Exit1:
				return;
Exit0:
				m_vecFailedEffect.push_back(*pEnvEffHandle);
			}
		};

		Visitor visitorTemp;
		visitorTemp.KeyHandle = KG3DENVEFF_DWGCKEY_HANDLE(nRegion, nHour);
		visitorTemp.m_pDW = this->GetDynamicWeather();
#if defined(DEBUG) | defined(_DEBUG)
		visitorTemp.m_VisitedItemCount = 0;
#endif
		KG_PROCESS_ERROR(visitorTemp.m_pDW);

		_ASSERTE(NULL != m_pTree);
		KSH::CTreeCtrlTraverse(*m_pTree, visitorTemp, KSH::emDepthFirst);
		_ASSERTE(visitorTemp.m_VisitedItemCount == m_pTree->GetCount());

		this->GridReloadKey(nRow, nColumn);

		if (! visitorTemp.m_vecFailedEffect.empty())
		{
			CString TempStr = _T("以下Effect，在SetAllKey过程中失败，原因未知：\n");
			for (size_t i = 0; i < visitorTemp.m_vecFailedEffect.size(); ++i)
			{
				TempStr += KSH::GetRTTITypeName(visitorTemp.m_vecFailedEffect[i]);
				TempStr += _T(";\n");
			}
			
			MessageBox(TempStr);
		}
	}
Exit0:
	return;
}

void KSceneSettingDialogDWGCKeyTrack::OnBnClickedSsBnCopyPrevKey()
{
	IEKG3DEnvEffDynamicWeather* pDW = GetDynamicWeather();
	KG_PROCESS_ERROR(pDW);
	{
		_ASSERTE(NULL != m_pGridCtrl);
		CCellID cellID = m_pGridCtrl->GetFocusCell();

		INT nRow = cellID.row;
		INT nColumn = cellID.col;
		KG3DENVEFF_DWGCKEY_HANDLE* pKeyHandle = static_cast<TypeGridCtrl*>(m_pGridCtrl)->GetItemBigData(nRow, 0);
		KG_PROCESS_ERROR(pKeyHandle);
		INT nHour = nColumn;
		INT nRegion = pKeyHandle->m_Region;
		KG3DENVEFF_DWGCKEY_HANDLE SrcHandle;
		{
			bool bIsAnyPrevKeyExist = false;
			for (int i = nColumn; i > 0; --i)
			{
				if (m_pGridCtrl->GetItemText(nRow, i) != _T(""))
				{
					SrcHandle = KG3DENVEFF_DWGCKEY_HANDLE(nRegion, i);
					bIsAnyPrevKeyExist = true;
					break;
				}
			}
			if (! bIsAnyPrevKeyExist)
			{
				MessageBox(_T("当前选中的Key前面没有Key，这个操作不成功"));
				goto Exit0;
			}
		}
		KG3DENVEFF_DWGCKEY_HANDLE DescHandle(nRegion, nHour);

		//首先检查前一个Key的位置是否都是一样的
		{
			struct Visitor : public  KSH::CTreeCtrlVisitor
			{
				KG3DENVEFF_DWGCKEY_HANDLE	m_KeyHandleSrc;
				KG3DENVEFF_DWGCKEY_HANDLE	m_KeyHandleDesc;
				IEKG3DEnvEffDynamicWeather* m_pDW;
				std::vector<KG3DENVEFF_HANDLE>	m_vecPrevKeyUnmatchEffects;
				std::vector<KG3DENVEFF_HANDLE>	m_vecPrevKeyDonotExistEffect;
				void Accept(CTreeCtrl& Tree, HTREEITEM hItem)
				{
					_ASSERTE(m_pDW);
					KG3DENVEFF_HANDLE* pEnvEffHandle = (dynamic_cast<TypeTreeCtrl&>(Tree)).GetItemBigData(hItem);
					_ASSERTE(pEnvEffHandle);
					
					if (! m_pDW->IsEffectDWGCAvailable(*pEnvEffHandle))
					{
						return;
					}

					KG3DENVEFF_DWGCKEY_HANDLE HandleRet;
					
					HRESULT hr = m_pDW->TL_GetPrevKeyHandle(*pEnvEffHandle, m_KeyHandleDesc, &HandleRet);
					if (SUCCEEDED(hr))
					{
						if (HandleRet != m_KeyHandleSrc)
						{
							m_vecPrevKeyUnmatchEffects.push_back(*pEnvEffHandle);
						}												
					}
					else
					{
						m_vecPrevKeyDonotExistEffect.push_back(*pEnvEffHandle);
					}
				}
			};

			Visitor visitorTemp;
			visitorTemp.m_pDW = pDW;
			visitorTemp.m_KeyHandleSrc = SrcHandle;
			visitorTemp.m_KeyHandleDesc = DescHandle;
			
			_ASSERTE(NULL != m_pTree);
			KSH::CTreeCtrlTraverse(*m_pTree, visitorTemp, KSH::emDepthFirst);

			if (! visitorTemp.m_vecPrevKeyDonotExistEffect.empty())
			{
				CString TempStr = _T("在以下的Effect的Key表中，当前选中的Key前面没有Key，操作不成功:\n");
				for (size_t i = 0; i < visitorTemp.m_vecPrevKeyDonotExistEffect.size(); ++i)
				{
					TempStr += KSH::GetRTTITypeName(visitorTemp.m_vecPrevKeyDonotExistEffect[i]);
					TempStr += _T(";\n");
				}
				MessageBox(TempStr);
				goto Exit0;
			}

			if (! visitorTemp.m_vecPrevKeyUnmatchEffects.empty())
			{
				CString TempStr = _T("在以下的Effect的Key表中，前面的Key的位置不一致，操作不成功:\n");
				for (size_t i = 0; i < visitorTemp.m_vecPrevKeyUnmatchEffects.size(); ++i)
				{
					TempStr += KSH::GetRTTITypeName(visitorTemp.m_vecPrevKeyUnmatchEffects[i]);
					TempStr += _T(";\n");
				}
				MessageBox(TempStr);
				goto Exit0;
			}
		}

		//开始正式拷贝
		{
			struct Visiter: public KSH::CTreeCtrlVisitor
			{
				KG3DENVEFF_DWGCKEY_HANDLE	m_KeyHandleSrc;
				KG3DENVEFF_DWGCKEY_HANDLE	m_KeyHandleDesc;
				IEKG3DEnvEffDynamicWeather* m_pDW;
				void Accept(CTreeCtrl& Tree, HTREEITEM hItem)
				{
					_ASSERTE(m_pDW);
					KG3DENVEFF_HANDLE* pEnvEffHandle = (dynamic_cast<TypeTreeCtrl&>(Tree)).GetItemBigData(hItem);
					_ASSERTE(pEnvEffHandle);
					if (! m_pDW->IsEffectDWGCAvailable(*pEnvEffHandle))
					{
						return;
					}
					m_pDW->TL_CopyKeyTo(*pEnvEffHandle, m_KeyHandleSrc
						, *pEnvEffHandle, m_KeyHandleDesc, TRUE);//这种拷贝只会ByValue

				}
			};

			Visiter visiterTemp;
			visiterTemp.m_pDW = pDW;
			visiterTemp.m_KeyHandleDesc = DescHandle;
			visiterTemp.m_KeyHandleSrc = SrcHandle;
			
			_ASSERTE(NULL != m_pTree);
			KSH::CTreeCtrlTraverse(*m_pTree, visiterTemp, KSH::emDepthFirst);

			//强制重新加载当前的Key表
			
			_ASSERTE(NULL != m_pTree);
			this->OnItemSelectChange(*m_pTree, m_pTree->GetSelectedItem(), NULL);
		}

	}
Exit0:
	return;
}

void KSceneSettingDialogDWGCKeyTrack::OnBnClickedSsBnDelAllKey()
{
	int nRet = MessageBox(_T("将会删除所有关键帧表上的当前选中的Region/Hour对应的关键帧，是否继续?"), NULL, MB_YESNO);
	KG_PROCESS_SUCCESS(nRet != IDYES);
	{
		_ASSERTE(NULL != m_pGridCtrl);
		IEKG3DEnvEffDynamicWeather* pDW = GetDynamicWeather();
		KG_PROCESS_ERROR(pDW);
		
		CCellID cellID = m_pGridCtrl->GetFocusCell();

		INT nRow = cellID.row;
		INT nColumn = cellID.col;
		KG3DENVEFF_DWGCKEY_HANDLE* pKeyHandle = static_cast<TypeGridCtrl*>(m_pGridCtrl)->GetItemBigData(nRow, 0);
		KG_PROCESS_ERROR(pKeyHandle);
		INT nHour = nColumn;
		INT nRegion = pKeyHandle->m_Region;
		KG3DENVEFF_DWGCKEY_HANDLE SrcHandle(nRegion, nHour);

		struct Visiter : public KSH::CTreeCtrlVisitor 
		{
			IEKG3DEnvEffDynamicWeather* m_pDW;
			KG3DENVEFF_DWGCKEY_HANDLE	m_KeyHandle;
			void Accept(CTreeCtrl& TreeBase, HTREEITEM hItem)
			{
				TypeTreeCtrl& Tree = dynamic_cast<TypeTreeCtrl&>(TreeBase);
				KG3DENVEFF_HANDLE* pEffHandle = Tree.GetItemBigData(hItem);
				KG_PROCESS_ERROR(pEffHandle);
				_ASSERTE(m_pDW);
				if (m_pDW->IsEffectDWGCAvailable(*pEffHandle))
				{
					m_pDW->TL_DelKey(*pEffHandle, m_KeyHandle);

				}
			Exit0:
				return;

			}
		};

		Visiter visitorTemp;
		visitorTemp.m_pDW = pDW;
		visitorTemp.m_KeyHandle = SrcHandle;

		_ASSERTE(NULL != m_pTree);
		KSH::CTreeCtrlTraverse(*m_pTree, visitorTemp, KSH::emDepthFirst);

		this->GridReloadKey(nRow, nColumn);
	}

Exit1:
Exit0:
	return;
}

void KSceneSettingDialogDWGCKeyTrack::OnBnClickedBnDwgcPlay()
{
	// TODO: Add your control notification handler code here

	if (m_bIsDWPlaying)///播放中
	{
		IEKG3DEnvEffDynamicWeather* pDW = this->GetDynamicWeather();
		if (pDW)
		{
			pDW->StopQuickPlay(NULL);
			HRESULT hr = pDW->EnablePlay(FALSE);
			_ASSERTE(SUCCEEDED(hr));
		}
		m_cbPlay.SetWindowText(_T("播放"));
		m_bIsDWPlaying = false;
		goto Exit1;
	}
	{
		CString csRegion, csHour;//, csBoostFactor;
		m_edRegion.GetWindowText(csRegion);
		m_edHour.GetWindowText(csHour);
		BOOL bRet = FALSE;
		INT nRegion  = 0;
		INT nHour = 0;
		
		HRESULT hr = E_FAIL;
		IEKG3DEnvEffDynamicWeather* pDW = this->GetDynamicWeather();
		if(!pDW)
		{
			MessageBox(_T("动态天气不存在?"));
			goto Exit0;
		}
#if defined(_DEBUG) | defined(DEBUG)
		{
			CString csZero = _T("000.0000");
			FLOAT fValue = 1;
			BOOL dbBRet = KG_CUSTOM_HELPERS::TStringToValue(csZero, fValue);
			_ASSERTE(dbBRet && fValue == 0.0f);
		}
#endif
		bRet = KG_CUSTOM_HELPERS::TStringToValue(csRegion, nRegion);
		if (!bRet)
		{
			_KG3D_DEBUG_SELF_REPORT(_T("Region转换失败，为什么?\n"));
			goto Exit0;
		}
		bRet = KG_CUSTOM_HELPERS::TStringToValue(csHour, nHour);
		if (!bRet || nHour <1 || nHour > 24)
		{
			MessageBox(_T("Hour不合法"));
			goto Exit0;
		}		
		hr = pDW->EnablePlay(TRUE);
		KG_COM_PROCESS_ERROR(hr);
		hr = pDW->StartQuickPlay(nRegion, nHour, NULL);
		KG_COM_PROCESS_ERROR(hr);
		
		//_ASSERTE(SUCCEEDED(hr)); 

#if defined(_DEBUG) | defined(DEBUG)
		CString csTemp2;
		m_cbPlay.GetWindowText(csTemp2);
		_ASSERTE(csTemp2 == _T("播放"));
#endif
		
		m_cbPlay.SetWindowText(_T("停止"));
		m_bIsDWPlaying = true;
	}
	
Exit1:
	return;
Exit0:
	return;
}

void KSceneSettingDialogDWGCKeyTrack::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if (0 == nIDEvent)
	{
		IEKG3DEnvEffDynamicWeather* pDW = this->GetDynamicWeather();
		if (pDW)
		{
			CString TempStr = _T("");
			int nHour = static_cast<int>(pDW->GetCurHour());
			
			if (nHour != m_Slider.GetPos())
			{
				m_Slider.SetPos(nHour);
			
				KG_CUSTOM_HELPERS::TValueToString<INT>(nHour, TempStr);
				m_edHour.SetWindowText(TempStr);
			}

			int nRegion = static_cast<int>(pDW->GetCurRegion());
			KG_CUSTOM_HELPERS::TValueToString(nRegion, TempStr);
			m_edRegion.SetWindowText(TempStr);
		}
	}
	else if (1 == nIDEvent)
	{
		if (::GetActiveWindow() == this->GetSafeHwnd())
		{
			HTREEITEM hItemSelected = static_cast<TypeTreeCtrl*>(m_pTree)->GetSelectedItem();
			KG3DENVEFF_HANDLE* pEnvEffHandle = static_cast<TypeTreeCtrl*>(m_pTree)->GetItemBigData(hItemSelected);
			if(NULL != pEnvEffHandle)
			{
				this->UpdatePage(pEnvEffHandle->m_dwType);
			}
		}
	}
	TypeBase::OnTimer(nIDEvent);
}

void KSceneSettingDialogDWGCKeyTrack::OnBnClickedSsBnHelp()
{
	// TODO: Add your control notification handler code here
	CString csTemp = _T("1.首先在Environment面板里面加上各个天气效果，其中天空盒和远山要在各自的面板中添加\
，以后会考虑全部在Environment面板中添加。如果不添加动态天气，本面板无效。\n\
2.添加好Region，对应的天气会自动在Region下面列出。\n\
3.使用四种模式之一在右边的关键帧DopeSheet中编辑关键帧：普通，双击预览；\
SetKey，双击以天气的当前状态添加（如果不存在）关键帧或者修改关键帧；\
编辑，双击打开对应的面板，单独编辑，界面会被锁定直到编辑完成；\
直接编辑，双击直接编辑Key的Index。所有同种Key的Index可以直接写到格子里面，\
拷贝关键帧也可以这样完成。某些不能编辑的关键帧，如SkySystem的关键帧是组合关键帧，关键帧的内容\
就是下属的SkyBox，远山等的Key的Index值，直接编辑就可以了。删除也是用直接编辑，把关键帧填成空就\
会删除掉\n\
其他使用方式参考Max的关键帧编辑器TrackGraph和DopeSheet\n\
4.播放需要提供哪个Region，从哪个时间开始，以及加速多少倍，一般建议1000倍为上限，开到10000的话\
因为有自动平衡机制，来不及切换的关键帧会直接扔掉，所以反而看不出效果。其实在普通模式下双击\
来预览也很清楚的。\
5.编辑完的东西存放在场景文件同目录下面的SeneSetting.ini中，格式基本是稳定的，所以界面错误并\
不会影响编辑好的东西。\n\
6.很多小问题没有来的及完成，以后会一直跟进，有什么Bug之类或建议的请联系RTX：chentianhong。");
	MessageBox(csTemp, _T("提示"), MB_OK|MB_ICONWARNING);
}

VOID KSceneSettingDialogDWGCKeyTrack::ReloadAllEnvEff()
{
	BindTreeCallbacks(FALSE);
	{
		_ASSERTE(NULL != m_pTree);
		m_pTree->DeleteAllItems();
		IEKG3DEnvironment* pEnv = KSH::GetEngineInterfaceEx<IEKG3DEnvironment>();
		KG_PROCESS_ERROR(pEnv);

		KG3DENVEFF_HANDLE Handle;
		IEKG3DEnvEffect*	pTopEff = NULL;
		HRESULT hr = E_FAIL;
		
		KGCH::IteratorHolder itHolder;
		for(hr = pEnv->GetFirstTopEnvEffect(itHolder, &pTopEff)
			; SUCCEEDED(hr)
			; hr = pEnv->GetNextTopEnvEffect(itHolder, &pTopEff))
		{
			Handle = KG3DENVEFF_HANDLE(pTopEff->GetType(), 0);
			_ASSERTE(pTopEff);
			HTREEITEM hTopItem = static_cast<TypeTreeCtrl*>(m_pTree)->InsertItemEx2(KSH::GetRTTITypeName(Handle), TVI_ROOT
				, TVI_LAST, &Handle, 0,1);
			
			if(hTopItem)
			{
				INT nSubCount = 0;
				KG3DENVEFF_HANDLE SubHandle;
				HRESULT hrSub = pTopEff->GetSubEffectCount(&nSubCount);
				if (SUCCEEDED(hrSub))
				{
					for (INT i = 0; i < nSubCount; ++i)
					{
						hrSub = pTopEff->GetSubEffect(i, NULL, &SubHandle);
						if (SUCCEEDED(hrSub))
						{
							static_cast<TypeTreeCtrl*>(m_pTree)->InsertItemEx2(KSH::GetRTTITypeName(SubHandle), hTopItem, TVI_LAST, &SubHandle,0,1);
						}
					}
				}
			}
		}
	}	
Exit0:
	BindTreeCallbacks(TRUE);
	HTREEITEM h = static_cast<TypeTreeCtrl*>(m_pTree)->GetChildItem(TVI_ROOT);
	if (h)
	{
		static_cast<TypeTreeCtrl*>(m_pTree)->SelectItem(h);
	}
	
}

void KSceneSettingDialogDWGCKeyTrack::OnHScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar )
{
	_ASSERTE(NULL != m_pGridCtrl);
	//横向时间滑动条
	if ((LPVOID)pScrollBar == (LPVOID)&m_Slider)
	{
		int nHour = m_Slider.GetPos();
		IEKG3DEnvEffDynamicWeather* pDW = GetDynamicWeather();
		if (pDW)
		{
			INT nRegion = static_cast<INT>(pDW->GetCurRegion());
			if(nRegion < KG3DENVEFFRI_REGION_MIN || nRegion > KG3DENVEFFRI_REGION_MAX)
			{
				INT nRet = MessageBox(_T("动态天气的当前区域不合法，可能是Region列表刚建立引起的\
，是否自动把Region列表中的第一项设为动态天气的当前区域?"), NULL, MB_YESNO);
				if(nRet == IDYES)
				{
					KG3DENVEFF_DWGCKEY_HANDLE* pKeyHandle = static_cast<TypeGridCtrl*>(m_pGridCtrl)->GetItemBigData(1,0);
					if(pKeyHandle)
					{
						pDW->SetCurRegion(pKeyHandle->m_Region, ! this->IsBlendingNeededToBeShown());
					}
				}
			}
			HRESULT hr = pDW->SetCurHour(nHour, ! this->IsBlendingNeededToBeShown());
			if (SUCCEEDED(hr))
			{
				INT InnerHour = static_cast<INT>(pDW->GetCurHour());
				_ASSERTE(InnerHour == nHour);
				CString TempStr;
				KG_CUSTOM_HELPERS::TValueToString<INT>(InnerHour, TempStr);
				m_edHour.SetWindowText(TempStr);
			}
		}
	}
	TypeBase::OnHScroll(nSBCode, nPos, pScrollBar);
}

VOID KSceneSettingDialogDWGCKeyTrack::UpdatePage( DWORD dwType )
{
	if(0 == m_hWndToPropertySheet)
		return;
	
	CWnd* p = CWnd::FromHandle(m_hWndToPropertySheet);

	try
	{
		KSceneSettingPropertySheet* pSheet = dynamic_cast<KSceneSettingPropertySheet*>(p);
		if(NULL == pSheet)
			return;

		CPropertyPage* pPropertyPage = pSheet->GetActivePage();
		KSceneSettingPageBase* pSettingPage = dynamic_cast<KSceneSettingPageBase*>(pPropertyPage);
		if(NULL == pSettingPage)
			return;

		pSettingPage->SynFromInnerToInterface(0);
	}
	catch (...)
	{

	}
}

VOID KSceneSettingDialogDWGCKeyTrack::SetAssociateHWnd( HWND hWnd )
{
	m_hWndToPropertySheet = hWnd;
}

#if 0
LRESULT KSceneSettingDialogDWGCKeyTrack::TypeTreeCtrl::WindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	if(message == WM_LBUTTONDOWN || message == WM_LBUTTONDBLCLK)
	{
		if (wParam == MK_LBUTTON)
		{
			INT x = LOWORD(lParam);
			INT y = HIWORD(lParam);
			UINT uFlags; 
			HTREEITEM hItem = this->HitTest(CPoint(x, y), &uFlags);
			if (hItem != NULL && TVHT_ONITEM & uFlags || TVHT_ONITEMBUTTON & uFlags)
			{
				//_KG3D_DEBUG_OUTPUT0("Selected\n");
				KG3DENVEFF_HANDLE* p = this->GetItemBigData(hItem);
				if (p && m_pDW)
				{
					if (! m_pDW->IsEffectDWGCAvailable(*p))
					{
						return FALSE;//截获不能使用动态天气的类，不让改选择
					}
				}	
			}
		}	
	}
	else if (message == TVM_SELECTITEM)
	{
		HTREEITEM hItem = reinterpret_cast<HTREEITEM>(lParam);
		KG3DENVEFF_HANDLE* p = this->GetItemBigData(hItem);
		if (p && m_pDW)
		{
			if (! m_pDW->IsEffectDWGCAvailable(*p))
			{
				return FALSE;
			}
		}
	}
	return TypeTreeCtrlBase::WindowProc(message, wParam, lParam);
}
#endif
