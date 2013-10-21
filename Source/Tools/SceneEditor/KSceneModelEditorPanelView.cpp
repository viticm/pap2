// KSceneModelEditorPanelView.cpp : implementation file
//

#include "stdafx.h"
#include "sceneeditor.h"
#include "KSceneModelEditorPanelView.h"
#include "mesheditordoc.h"
#include "KSceneModelEditorSFXNewDialog.h"
#include "KSceneModelEditorModelSaveDialog.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////



IMPLEMENT_DYNCREATE(KSceneModelEditorPanelView, CFormView)

KSceneModelEditorPanelView::KSceneModelEditorPanelView()
	: CFormView(KSceneModelEditorPanelView::IDD)
{
	//{{AFX_DATA_INIT(KSceneModelEditorPanelView)
	m_scModelName = _T("当前模型:");
	//}}AFX_DATA_INIT
	m_pPropertySheet = NULL;
}

KSceneModelEditorPanelView::~KSceneModelEditorPanelView()
{
	SAFE_DELETE(m_pPropertySheet);
}

void KSceneModelEditorPanelView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(KSceneModelEditorPanelView)
	DDX_Text(pDX, IDC_STATIC_CURMODEL, m_scModelName);
	DDX_Control(pDX, IDC_COMBO_MODELPART, m_comboPartList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(KSceneModelEditorPanelView, CFormView)
	//{{AFX_MSG_MAP(KSceneModelEditorPanelView)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnBnClickedButtonSave)
	ON_CBN_SELCHANGE(IDC_COMBO_MODELPART, &KSceneModelEditorPanelView::OnCbnSelchangeComboModelpart)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// KSceneModelEditorPanelView diagnostics

#ifdef _DEBUG
void KSceneModelEditorPanelView::AssertValid() const
{
	CFormView::AssertValid();
}

void KSceneModelEditorPanelView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// KSceneModelEditorPanelView message handlers

void KSceneModelEditorPanelView::OnInitialUpdate()
{
    IEKG3DModel *pModel = NULL;
	CFormView::OnInitialUpdate();
	CWnd* pwndPropSheetHolder = GetDlgItem(IDC_MODELEDITOR_TAB);
	KSceneModelEditorDoc* pDoc = (KSceneModelEditorDoc*) GetDocument();
	if (!m_pPropertySheet)
	{
		m_pPropertySheet = new KSceneModelEditorPropertySheet(pwndPropSheetHolder);
		if (!m_pPropertySheet->Create(pwndPropSheetHolder,
		WS_CHILD | WS_VISIBLE, 0))
		{
			delete m_pPropertySheet;
			m_pPropertySheet = NULL;
			return;
		}
	}
	// fit the property sheet into the place holder window, and show it
	CRect rectPropSheet;
	pwndPropSheetHolder->GetWindowRect(rectPropSheet);
	m_pPropertySheet->SetWindowPos(NULL,0, 0,
		rectPropSheet.Width(), rectPropSheet.Height(),
		SWP_NOZORDER | SWP_NOACTIVATE);
	KG_PROCESS_ERROR(pDoc);
	pDoc->GetCurrentModel(&pModel);
	KG_PROCESS_ERROR(pModel);

	DWORD dwType = 0;
	pModel->GetType(&dwType);
	if (dwType == MESHTYPE_MODELST)
	{
		UpdateModelPartList();
	}
	else
	{
		if (dwType == MESHTYPE_SPEEDTREE)
		{
			IEKG3DModelSpeedTree *pTree = dynamic_cast<IEKG3DModelSpeedTree*>(pModel);
			IEKG3DModel *pTrunk = NULL;
			pTree->GetModelTree(&pTrunk);
			pModel = pTrunk;
		}
		m_pPropertySheet->m_PropertyPageMesh.SetCurModel(pModel,pModel);
		m_pPropertySheet->m_PropertyPageMaterial.SetCurModel(pModel);
		m_pPropertySheet->m_PropertyPageMisc.SetCurModel(pModel);
	}

	m_pPropertySheet->m_PropertyPageAnimation.SetModel(pModel);
Exit0:
	;
}

void KSceneModelEditorPanelView::OnBnClickedButtonSave()
{
	KSceneModelEditorDoc* pDoc = (KSceneModelEditorDoc*) GetDocument();
    
    IEKG3DModel* pModel = NULL;//pDoc->m_pCurrentModel[0];

	if(!pDoc)
		return;
    
    pDoc->GetCurrentModel(&pModel);

	KSceneModelEditorModelSaveDialog Dlg;
	Dlg.SetModel(pModel);

	Dlg.DoModal();
}

void KSceneModelEditorPanelView::OnCbnSelchangeComboModelpart()
{
	KSceneModelEditorDoc* pDoc = (KSceneModelEditorDoc*) GetDocument();
	IEKG3DModel *pModel = NULL;
	pDoc->GetCurrentModel(&pModel);

	CComboBox* pPartList = static_cast<CComboBox*>(GetDlgItem(IDC_COMBO_MODELPART));
	int nCurSel = 0;
	IEKG3DModel* pPartModel = 0;
	nCurSel = pPartList->GetCurSel();

	KG_PROCESS_ERROR(nCurSel != LB_ERR);

	pPartModel = static_cast<IEKG3DModel*>(pPartList->GetItemDataPtr(nCurSel));

	m_pPropertySheet->m_PropertyPageMesh.SetCurModel(pPartModel,pModel);
	m_pPropertySheet->m_PropertyPageMaterial.SetCurModel(pPartModel);
Exit0:
	return;
}

void KSceneModelEditorPanelView::UpdateModelPartList()
{
	KSceneModelEditorDoc* pDoc = (KSceneModelEditorDoc*) GetDocument();
	IEKG3DModel *pModel = NULL;
	DWORD dwModelType = 0;

    KG_PROCESS_ERROR(pDoc);
    pDoc->GetCurrentModel(&pModel);
    KG_PROCESS_ERROR(pModel);

	pModel->GetType(&dwModelType);

    if (dwModelType == MESHTYPE_MODELST)
	{
		CComboBox* pListBox = static_cast<CComboBox*>(GetDlgItem(IDC_COMBO_MODELPART));

		KG_PROCESS_ERROR(pListBox);
		pListBox->EnableWindow(TRUE);

		if(1)
			m_ModelSetEdit.FilltModelSetList(*pListBox, pModel);
		else
		{
			vector<IEKG3DModel*>vecModels;
			pDoc->GetAllModels(&vecModels);
			m_ModelSetEdit.FilltModelSetList(*pListBox,&vecModels);
		}
	}
	else
	{
		GetDlgItem(IDC_COMBO_MODELPART)->EnableWindow(FALSE);
	}

Exit0:
	return;
}


KSceneModelEditorPropertyPageMesh* KSceneModelEditorPanelView::GetMeshPanel()
{
	return &(m_pPropertySheet->m_PropertyPageMesh);
}

KSceneModelEditorPropertyPageMaterial* KSceneModelEditorPanelView::GetMaterialPanel()
{
	return &(m_pPropertySheet->m_PropertyPageMaterial);
}

KSceneModelEditorPropertyPageAnimation* KSceneModelEditorPanelView::GetAnimationPanel()
{
    return &(m_pPropertySheet->m_PropertyPageAnimation);
}


int KSceneModelEditorPanelView::GetCurrentPartIndex()
{
	return m_comboPartList.GetCurSel();
}

int KSceneModelEditorPanelView::SetCurrentPartIndex(LPTSTR pszPartName, int nIndex)
{
    int    nResult = false;
    char   szName[MAX_PATH];
    CComboBox* pListBox = static_cast<CComboBox*>(GetDlgItem(IDC_COMBO_MODELPART));
    int nFindIndex = -1;
    int nCount = pListBox->GetCount();

    KG_PROCESS_ERROR(pszPartName);

    for (int i = 0; i < nCount; i++)
    {
        pListBox->GetLBText(i, szName);
        if (strcmp(szName, pszPartName) == 0)
        {
            nFindIndex = i;
            break;
        }
    }
    pListBox->SetCurSel(nFindIndex);
    nResult = true;
Exit0:
    if (!nResult || nFindIndex == -1)
    {
        pListBox->SetCurSel(nIndex);
    }
    return nResult;
}