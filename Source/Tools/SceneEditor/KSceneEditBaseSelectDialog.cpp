// KSceneEditBaseSelectDialog.cpp : implementation file
//

#include "stdafx.h"
#include "sceneeditor.h"
#include "KSceneEditBaseSelectDialog.h"
#include ".\ksceneeditbaseselectdialog.h"
#include "IEKG3DSceneLayer.h"
#include "IEEditor.h"
#include "KSceneHelperFuncs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// KSceneEditBaseSelectDialog dialog
#define GETSELECT(pSceneEditorBase) \
	IEKG3DSceneSelectBase* pSelector = NULL;\
	pSceneEditorBase->GetSceneSelector(&pSelector);\
	_ASSERTE(pSelector);\
	if (!pSelector) return

KSceneEditBaseSelectDialog::KSceneEditBaseSelectDialog(CWnd* pParent /*=NULL*/)
	: CDialog(KSceneEditBaseSelectDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(KSceneEditBaseSelectDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	//m_lpSceneEditor = NULL;
}


void KSceneEditBaseSelectDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(KSceneEditBaseSelectDialog)
	DDX_Control(pDX, IDC_LIST1, m_ListBoxEntity);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_TREE_LAYER, m_TreeLayer);
	DDX_Control(pDX, IDC_LIST_LAYERENTITY, m_ListBoxLayer);
}


BEGIN_MESSAGE_MAP(KSceneEditBaseSelectDialog, CDialog)
	//{{AFX_MSG_MAP(KSceneEditBaseSelectDialog)
	ON_LBN_SELCHANGE(IDC_LIST1, OnSelchangeList1)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON_SELECTNONE, OnBnClickedButtonSelectNone)
	ON_BN_CLICKED(IDC_BUTTON_SELECTINVERT, OnBnClickedButtonSelectInvert)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_LAYER, &KSceneEditBaseSelectDialog::OnTvnSelchangedTreeLayer)
	ON_BN_CLICKED(IDC_BUTTON_ADDLAYER, &KSceneEditBaseSelectDialog::OnBnClickedButtonAddlayer)
	ON_BN_CLICKED(IDC_BUTTON_DELETELAYER, &KSceneEditBaseSelectDialog::OnBnClickedButtonDeletelayer)
	ON_BN_CLICKED(IDC_BUTTON_ADDENTITY, &KSceneEditBaseSelectDialog::OnBnClickedButtonAddentity)
	ON_BN_CLICKED(IDC_BUTTON_FREEON, &KSceneEditBaseSelectDialog::OnBnClickedButtonFreeon)
	ON_BN_CLICKED(IDC_TOGGLEHIDE, &KSceneEditBaseSelectDialog::OnBnClickedButtonHide)
	ON_LBN_SELCHANGE(IDC_LIST_LAYERENTITY, &KSceneEditBaseSelectDialog::OnLbnSelchangeListLayerentity)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// KSceneEditBaseSelectDialog message handlers

void KSceneEditBaseSelectDialog::OnSelchangeList1() 
{
	TCHAR strOutputInfo[MAX_PATH];
	// TODO: Add your control notification handler code here
	if(!m_lpSceneEditor)
		return;

	IEKG3DSceneSelectionTool* pTool = NULL;
	HRESULT hr = m_lpSceneEditor->GetSelectionToolInterface(&pTool);
	if (FAILED(hr))
	{
		return;
	}

	//m_lpSceneEditor->m_listSelectedEntity.clear();
	IEKG3DSceneSelectBase* pSelector = NULL;
	m_lpSceneEditor->GetSceneSelector(&pSelector);
	_ASSERTE(pSelector);
	
	//pSelector->ClearSelectedEntity();
	pTool->ClearSelection();

	
	int nSelected = m_ListBoxEntity.GetSelCount();
	if(nSelected>0)
	{
		int* pInt = new int[nSelected];
		m_ListBoxEntity.GetSelItems(nSelected,pInt);
		for(int i=0;i<nSelected;i++)
		{
            KG3DSceneEntityInfo *pEntityInfo = &m_vecEntityInListBox[pInt[i]];

			//pSelector->AddSelectedEntity(pEntityInfo->Type, pEntityInfo->pvPointer);
			pTool->SetSelected(pEntityInfo->Type, pEntityInfo->pvPointer, TRUE);
		}
		SAFE_DELETE_ARRAY(pInt);
	}
	sprintf(strOutputInfo, "已选中的模型个数: %d", nSelected);
	GetDlgItem(IDC_STATIC_COUNT)->SetWindowText(strOutputInfo);
	
}

BOOL KSceneEditBaseSelectDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	FillEntityListBox();

	FillLayerTree();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void KSceneEditBaseSelectDialog::FillLayerEntity(IEKG3DSceneLayer *piLayer)
{
	{
		m_ListBoxLayer.SetCurSel(-1);
		DWORD Count = m_ListBoxLayer.GetCount();
		for(DWORD i=0;i<Count*2+3;i++)
		{
			m_ListBoxLayer.DeleteString(0);
		}
	}

	//KG3DSceneEntityList::iterator i = piLayer->m_listEntity.begin();
	//while (i!=piLayer->m_listEntity.end())
	//{
	//	KG3DSceneEntity& Entity = *i;

	//	TCHAR Name[256];
	//	Entity.GetName(Name);
	//	m_ListBoxLayer.AddString(Name);
	//	i++;
	//}
}

void KSceneEditBaseSelectDialog::FillEntityListBox()
{
	m_vecEntityInListBox.clear();
	{
		m_ListBoxEntity.SetCurSel(-1);
		DWORD Count = m_ListBoxEntity.GetCount();
		for(DWORD i=0;i<Count*2+3;i++)
		{
			m_ListBoxEntity.DeleteString(0);
		}
	}
	
	if(!m_lpSceneEditor)
		return;

    BOOL bEntityEqual = FALSE;
	IEKG3DSceneSelectBase* pSelector = NULL;
	m_lpSceneEditor->GetSceneSelector(&pSelector);
	_ASSERTE(&pSelector);
	INT nCount = pSelector->GetSelectableEntitySize();
	
	IEKG3DSceneSelectionToolAccessProxy* pProxy = KSF::GetSelectionProxy(m_lpSceneEditor);
	if(NULL == pProxy)
		return;

	INT i = 0;
	while(i < nCount)
	{
        KG3DSceneEntityInfo EntityInfo;
		IEKG3DSceneEntity* pEntity = NULL;
        pSelector->GetSelectableEntity(i, &pEntity);
		_ASSERTE(&pEntity);
		pEntity->GetType(&EntityInfo.Type);
        pEntity->GetPointer(&EntityInfo.pvPointer);

		TCHAR Name[256];
		pEntity->GetName(Name, _countof(Name));
		
		int Index = m_ListBoxEntity.AddString(Name);
		m_vecEntityInListBox.push_back(EntityInfo);

		/*INT nSelectedSize = pSelector->GetSelectedEntitySize();
		INT k = 0;
		while(k < nSelectedSize)
		{
			IEKG3DSceneEntity* pSelectedEntity = NULL;
			pSelector->GetSelectedEntity(k, &pSelectedEntity);
			_ASSERTE(pSelectedEntity);*/

		
		for(IEKG3DSceneEntity* pSelectedEntity = pProxy->Reset(); NULL != pSelectedEntity; pSelectedEntity = pProxy->Next())
		{
			pEntity->Compare(pSelectedEntity, &bEntityEqual);
			if(bEntityEqual)
			{
				m_ListBoxEntity.SetSel(Index);
			}
			//k++;
		}
		i++;
	}
}

void KSceneEditBaseSelectDialog::OnOK() 
{
	// TODO: Add extra validation here
	if(!m_lpSceneEditor)
		return;

	//m_lpSceneEditor->m_listSelectedEntity.clear();
	IEKG3DSceneSelectBase* pSelector = NULL;
	m_lpSceneEditor->GetSceneSelector(&pSelector);
	_ASSERTE(pSelector);
	//pSelector->ClearSelectedEntity();

	IEKG3DSceneSelectionTool* pTool = NULL;
	HRESULT hr = m_lpSceneEditor->GetSelectionToolInterface(&pTool);
	if (FAILED(hr))
	{
		return;
	}
	pTool->ClearSelection();

	int nSelected = m_ListBoxEntity.GetSelCount();
	if(nSelected>0)
	{
		int* pInt = new int[nSelected];

		m_ListBoxEntity.GetSelItems(nSelected,pInt);
		for(int i=0;i<nSelected;i++)
		{
            KG3DSceneEntityInfo *pEntityInfo = &m_vecEntityInListBox[pInt[i]];
			//pSelector->AddSelectedEntity(pEntityInfo->Type, pEntityInfo->pvPointer);
			pTool->SetSelected(pEntityInfo->Type, pEntityInfo->pvPointer, TRUE);
		}

		SAFE_DELETE_ARRAY(pInt);
	}
	CDialog::OnOK();
}

void KSceneEditBaseSelectDialog::OnBnClickedButton1()//select all
{
	/*GETSELECT(m_lpSceneEditor);

	pSelector->SelectAll(0);*/

	IEKG3DSceneSelectionTool* pTool = NULL;
	m_lpSceneEditor->GetSelectionToolInterface(&pTool);
	if (NULL != pTool)
	{
		pTool->SelectAll();
	}

	FillEntityListBox();
}


void KSceneEditBaseSelectDialog::OnBnClickedButtonSelectNone()
{
	/*GETSELECT(m_lpSceneEditor);

	pSelector->SelectNone(0);*/

	IEKG3DSceneSelectionTool* pSelTool = NULL;
	HRESULT hr = m_lpSceneEditor->GetSelectionToolInterface(&pSelTool);
	_ASSERTE(SUCCEEDED(hr));
	if(NULL != pSelTool)
		pSelTool->ClearSelection();

	FillEntityListBox();
}

void KSceneEditBaseSelectDialog::OnBnClickedButtonSelectInvert()
{
	GETSELECT(m_lpSceneEditor);

	//pSelector->SelectInvert(0);
	IEKG3DSceneSelectionTool* pSelTool = NULL;
	HRESULT hr = m_lpSceneEditor->GetSelectionToolInterface(&pSelTool);
	_ASSERTE(SUCCEEDED(hr));
	if(NULL != pSelTool)
		pSelTool->SelectInvert();

	FillEntityListBox();
}

void KSceneEditBaseSelectDialog::OnTvnSelchangedTreeLayer(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	HTREEITEM hItem = m_TreeLayer.GetSelectedItem();
	if(!hItem)
		return;
	IEKG3DSceneLayer *piLayer = (IEKG3DSceneLayer *)(m_TreeLayer.GetItemData(hItem));
	if(!piLayer)
		return;
	FillLayerEntity(piLayer);
}

void KSceneEditBaseSelectDialog::FillLayerTree()
{
	m_TreeLayer.DeleteAllItems();
	if(!m_lpSceneEditor)
		return;
    /*IEKG3DSceneLayer *piRootLayer = NULL;
    m_lpSceneEditor->GetRootLayer(&piRootLayer);
	if(!piRootLayer)
		return;
	FillOneLayer(piRootLayer,TVI_ROOT);*/
}

void KSceneEditBaseSelectDialog::FillOneLayer(IEKG3DSceneLayer *piLayer, HTREEITEM hRoot)
{
    HRESULT hRetCode = E_FAIL;
    LPCTSTR pcszLayerName    = NULL;
    DWORD   dwLayerID        = 0;
    BOOL    hLayerVisible    = FALSE;
    BOOL    bLayerSelectable = FALSE;
    DWORD   dwChildLayerCount = 0;
    IEKG3DSceneLayer *piChildLayer = NULL;
    DWORD   i = 0;
	TCHAR name[256];

    ASSERT(piLayer);
    
    hRetCode = piLayer->GetName(&pcszLayerName);
    KGLOG_COM_PROCESS_ERROR(hRetCode);
    ASSERT(pcszLayerName);
 
    hRetCode = piLayer->GetID(&dwLayerID);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = piLayer->GetVisible(&hLayerVisible);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = piLayer->GetSelectable(&bLayerSelectable);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = piLayer->GetChildLayerCount(&dwChildLayerCount);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    wsprintf(name,"%s %d 显示 %d 选择 %d 子节点 %d",
		pcszLayerName,
        dwLayerID,
		hLayerVisible,
		bLayerSelectable,
		dwChildLayerCount
    );

	HTREEITEM hItem = m_TreeLayer.InsertItem(name,hRoot,TVI_LAST);
	m_TreeLayer.SetItemData(hItem,(DWORD_PTR)(piLayer));

	for(i = 0; i < dwChildLayerCount; ++i)
	{
		hRetCode = piLayer->GetChildLayer(i, &piChildLayer);
        KGLOG_COM_PROCESS_ERROR(hRetCode);
        
    	FillOneLayer(piChildLayer, hItem);
	}
Exit0:
    return;
}

void KSceneEditBaseSelectDialog::OnBnClickedButtonAddlayer()
{
    HRESULT hResult  = E_FAIL;
    HRESULT hRetCode = E_FAIL;
    HTREEITEM hItem = NULL;
    IEKG3DSceneLayer *piLayer = NULL;
    IEKG3DSceneLayer *piNewLayer = NULL;

    hItem = m_TreeLayer.GetSelectedItem();
    KG_PROCESS_ERROR(hItem);

    piLayer = (IEKG3DSceneLayer *)(m_TreeLayer.GetItemData(hItem));
    KGLOG_PROCESS_ERROR(piLayer);

    /*hRetCode  = m_lpSceneEditor->AddSceneLayer(piLayer, "新图层", &piNewLayer);
    KGLOG_COM_PROCESS_ERROR(hRetCode);
    ASSERT(piNewLayer);

    FillOneLayer(piNewLayer, hItem);*/

//Exit1:
    hResult = S_OK;
Exit0:
    return ;
}

void KSceneEditBaseSelectDialog::OnBnClickedButtonDeletelayer()
{
	if(!m_lpSceneEditor)
		return;

	HTREEITEM hItem = m_TreeLayer.GetSelectedItem();
	if(!hItem)
		return;
	IEKG3DSceneLayer *piLayer = (IEKG3DSceneLayer *)(m_TreeLayer.GetItemData(hItem));
	if(!piLayer)
		return;
	//m_lpSceneEditor->DeleteSceneLayer(piLayer);
}

void KSceneEditBaseSelectDialog::OnBnClickedButtonAddentity()
{
	// TODO: Add your control notification handler code here
	if(!m_lpSceneEditor)
		return;

	HTREEITEM hItem = m_TreeLayer.GetSelectedItem();
	if(!hItem)
		return;
	IEKG3DSceneLayer *piLayer = (IEKG3DSceneLayer *)(m_TreeLayer.GetItemData(hItem));
	if(!piLayer)
		return;

	//m_lpSceneEditor->PutSelectedEntityToLayer(piLayer);

}

void KSceneEditBaseSelectDialog::OnBnClickedButtonHide()
{
    HRESULT hRetCode = E_FAIL;
    BOOL    bLayerVisible = FALSE;

    KG_PROCESS_ERROR(m_lpSceneEditor);

	HTREEITEM hItem = m_TreeLayer.GetSelectedItem();
    KG_PROCESS_ERROR(hItem);

    IEKG3DSceneLayer *piLayer = (IEKG3DSceneLayer *)(m_TreeLayer.GetItemData(hItem));
    KG_PROCESS_ERROR(piLayer);

    hRetCode = piLayer->GetVisible(&bLayerVisible);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = piLayer->SetVisible(!bLayerVisible);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

Exit0:
    return;
}

void KSceneEditBaseSelectDialog::OnBnClickedButtonFreeon()
{
    HRESULT hRetCode = E_FAIL;
    BOOL    bLayerSelectable = FALSE;
    HTREEITEM hItem = NULL;
    IEKG3DSceneLayer *piLayer = NULL;

    KG_PROCESS_ERROR(m_lpSceneEditor);

	hItem = m_TreeLayer.GetSelectedItem();
    KG_PROCESS_ERROR(hItem);

    piLayer = (IEKG3DSceneLayer *)(m_TreeLayer.GetItemData(hItem));
    KG_PROCESS_ERROR(piLayer);
	
    hRetCode = piLayer->GetSelectable(&bLayerSelectable);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = piLayer->SetSelectable(!bLayerSelectable);
    KGLOG_COM_PROCESS_ERROR(hRetCode);
Exit0:
    return;
}

void KSceneEditBaseSelectDialog::OnLbnSelchangeListLayerentity()
{
	// TODO: Add your control notification handler code here
}

