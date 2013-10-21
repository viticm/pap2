// KSceneEditorLayerDialog.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KSceneEditorLayerDialog.h"
//#include "Scene/KG3DSceneEditorBase.h"
#include "IEKG3DSceneLayer.h"
#include "IEEditor.h"
// KSceneEditorLayerDialog dialog

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


IMPLEMENT_DYNAMIC(KSceneEditorLayerDialog, CDialog)

KSceneEditorLayerDialog::KSceneEditorLayerDialog(CWnd* pParent /*=NULL*/)
	: CDialog(KSceneEditorLayerDialog::IDD, pParent)
{
	m_lpEditor = NULL;
}

KSceneEditorLayerDialog::~KSceneEditorLayerDialog()
{
}

void KSceneEditorLayerDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_TreeLayer);
	DDX_Control(pDX, IDC_EDIT_NAME, m_LayerName);
}


BEGIN_MESSAGE_MAP(KSceneEditorLayerDialog, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_NEWLAYER, &KSceneEditorLayerDialog::OnBnClickedButtonNewlayer)
	ON_BN_CLICKED(IDC_BUTTON_DELETELAYER, &KSceneEditorLayerDialog::OnBnClickedButtonDeletelayer)
	ON_EN_CHANGE(IDC_EDIT_NAME, &KSceneEditorLayerDialog::OnEnChangeEditName)
	ON_BN_CLICKED(IDC_BUTTON_RENAME, &KSceneEditorLayerDialog::OnBnClickedButtonRename)
	ON_BN_CLICKED(IDC_CHECK_HIDE, &KSceneEditorLayerDialog::OnBnClickedCheckHide)
	ON_BN_CLICKED(IDC_CHECK_FREEZE, &KSceneEditorLayerDialog::OnBnClickedCheckFreeze)
	ON_BN_CLICKED(IDC_BUTTON_SELECT, &KSceneEditorLayerDialog::OnBnClickedButtonSelect)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BUTTON_INPUTSELECTED, &KSceneEditorLayerDialog::OnBnClickedButtonInputselected)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, &KSceneEditorLayerDialog::OnTvnSelchangedTree1)
END_MESSAGE_MAP()


// KSceneEditorLayerDialog message handlers

void KSceneEditorLayerDialog::OnBnClickedButtonNewlayer()
{
    HRESULT hResult  = E_FAIL;
    HRESULT hRetCode = E_FAIL;
    HTREEITEM hItem = NULL;
    IEKG3DSceneLayer *piLayer = NULL;
    IEKG3DSceneLayer *piNewLayer = NULL;

    KG_PROCESS_ERROR(m_lpEditor);

    hItem = m_TreeLayer.GetSelectedItem();
    KG_PROCESS_ERROR(hItem);

    piLayer = (IEKG3DSceneLayer *)(m_TreeLayer.GetItemData(hItem));
    KGLOG_PROCESS_ERROR(piLayer);

    //hRetCode  = m_lpEditor->AddSceneLayer(piLayer, "新图层", &piNewLayer);
    //KGLOG_COM_PROCESS_ERROR(hRetCode);
    //ASSERT(piNewLayer);

    //FillOneLayer(piNewLayer,hItem);

//Exit1:
    hResult = S_OK;
Exit0:
    return;
}

void KSceneEditorLayerDialog::OnBnClickedButtonDeletelayer()
{
	if(!m_lpEditor)
		return;

	HTREEITEM hItem = m_TreeLayer.GetSelectedItem();
	if(!hItem)
		return;
	IEKG3DSceneLayer *piLayer = (IEKG3DSceneLayer *)(m_TreeLayer.GetItemData(hItem));
	if(!piLayer)
		return;
	//m_lpEditor->DeleteSceneLayer(piLayer);
}

void KSceneEditorLayerDialog::OnEnChangeEditName()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}

void KSceneEditorLayerDialog::OnBnClickedButtonRename()
{
    HRESULT hResult  = E_FAIL;
    HRESULT hRetCode = E_FAIL;
    HTREEITEM hItem  = NULL;
    IEKG3DSceneLayer *piLayer = NULL;
    CString name;

    KG_PROCESS_ERROR(m_lpEditor);

	hItem = m_TreeLayer.GetSelectedItem();
    KG_PROCESS_ERROR(hItem);

    piLayer = (IEKG3DSceneLayer *)(m_TreeLayer.GetItemData(hItem));
    KG_PROCESS_ERROR(piLayer);

	m_LayerName.GetWindowText(name);

	hRetCode = piLayer->SetName(name);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hResult = S_OK;
Exit0:
    return;
}

void KSceneEditorLayerDialog::OnBnClickedCheckHide()
{
    HRESULT hResult  = E_FAIL;
    HRESULT hRetCode = E_FAIL;
    BOOL    bLayerVisible = FALSE;
    HTREEITEM hItem = NULL;
    IEKG3DSceneLayer *piLayer = NULL;

    KG_PROCESS_ERROR(m_lpEditor);

	hItem = m_TreeLayer.GetSelectedItem();
    KG_PROCESS_ERROR(hItem);

    piLayer = (IEKG3DSceneLayer *)(m_TreeLayer.GetItemData(hItem));
    KG_PROCESS_ERROR(piLayer);

    hRetCode = piLayer->GetVisible(&bLayerVisible);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = piLayer->SetVisible(!bLayerVisible);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hResult = S_OK;
Exit0:
    return;
}

void KSceneEditorLayerDialog::OnBnClickedCheckFreeze()
{
    HRESULT hResult  = E_FAIL;
    HRESULT hRetCode = E_FAIL;
    BOOL    bLayerSelectable = FALSE;
    HTREEITEM hItem = NULL;
    IEKG3DSceneLayer *piLayer = NULL;

    KG_PROCESS_ERROR(m_lpEditor);

    hItem = m_TreeLayer.GetSelectedItem();
    KG_PROCESS_ERROR(hItem);

    piLayer = (IEKG3DSceneLayer *)(m_TreeLayer.GetItemData(hItem));
    KG_PROCESS_ERROR(piLayer);

    hRetCode = piLayer->GetSelectable(&bLayerSelectable);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = piLayer->SetSelectable(!bLayerSelectable);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hResult = S_OK;
Exit0:
    return;
}

void KSceneEditorLayerDialog::OnBnClickedButtonSelect()
{
	// TODO: Add your control notification handler code here
}

void KSceneEditorLayerDialog::FillLayerTree()
{
	m_TreeLayer.DeleteAllItems();
	if(!m_lpEditor)
		return;
	//IEKG3DSceneLayer* pRootLayer = NULL;
	//m_lpEditor->GetRootLayer(&pRootLayer);
	//if(!pRootLayer)
	//	return;
	//FillOneLayer(pRootLayer,TVI_ROOT);
}

void KSceneEditorLayerDialog::FillOneLayer(IEKG3DSceneLayer *piLayer,HTREEITEM hRoot)
{
    HRESULT hResult  = E_FAIL;
    HRESULT hRetCode = E_FAIL;
    LPCTSTR pcszLayerName    = NULL;
    DWORD   dwLayerID        = 0;
    BOOL    hLayerVisible    = FALSE;
    BOOL    bLayerSelectable = FALSE;
    IEKG3DSceneLayer *piChildLayer = NULL;
    DWORD dwChildLayerCount = 0;
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

    hRetCode = piLayer->GetChildLayerCount(&dwChildLayerCount);

    for(i = 0; i < dwChildLayerCount; ++i)
    {
        hRetCode = piLayer->GetChildLayer(i, &piChildLayer);
        KGLOG_COM_PROCESS_ERROR(hRetCode);

        FillOneLayer(piChildLayer, hItem);
    }

    hResult = S_OK;
Exit0:
    return;
}

void KSceneEditorLayerDialog::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	// TODO: Add your message handler code here
	if(bShow)
	{
		FillLayerTree();
	}
}

void KSceneEditorLayerDialog::OnBnClickedButtonInputselected()
{
	if(!m_lpEditor)
		return;

	HTREEITEM hItem = m_TreeLayer.GetSelectedItem();
	if(!hItem)
		return;
	IEKG3DSceneLayer *piLayer = (IEKG3DSceneLayer *)(m_TreeLayer.GetItemData(hItem));
	if(!piLayer)
		return;

	//m_lpEditor->PutSelectedEntityToLayer(piLayer);
}

void KSceneEditorLayerDialog::OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
    HRESULT hRetCode = E_FAIL;
    LPCTSTR pcszLayerName    = NULL;
    BOOL    bLayerSelectable = FALSE;
    BOOL    bLayerVisible    = FALSE;
    HTREEITEM hItem = NULL;
    IEKG3DSceneLayer *piLayer = NULL;



    LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

    KG_PROCESS_ERROR(m_lpEditor);

	hItem = m_TreeLayer.GetSelectedItem();
	if(!hItem)
	{
		GetDlgItem(IDC_CHECK_HIDE)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_FREEZE)->EnableWindow(FALSE);
		KG_PROCESS_ERROR(false);
	}
	piLayer = (IEKG3DSceneLayer *)(m_TreeLayer.GetItemData(hItem));
    KG_PROCESS_ERROR(piLayer);

    hRetCode = piLayer->GetName(&pcszLayerName);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = piLayer->GetVisible(&bLayerVisible);
    KGLOG_COM_PROCESS_ERROR(hRetCode);
    CheckDlgButton(IDC_CHECK_HIDE,!bLayerVisible);

    hRetCode = piLayer->GetSelectable(&bLayerSelectable);
    KGLOG_COM_PROCESS_ERROR(hRetCode);
	CheckDlgButton(IDC_CHECK_FREEZE,!bLayerSelectable);

	m_LayerName.SetWindowText(pcszLayerName);
Exit0:
    return;
}
