// KSceneEditorVisibleSetDialog.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KSceneEditorVisibleSetDialog.h"
//#include "KG3DRepresentObjectVisibleEntitySet.h"
#include "KG3DFileViewDialog.h"
//#include "KG3DSceneSceneEditor.h"
#include "IEKG3DSceneLayer.h"
#include "IEEditor.h"//by dengzhihui Refactor 2006年11月29日


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

// KSceneEditorVisibleSetDialog dialog

IMPLEMENT_DYNAMIC(KSceneEditorVisibleSetDialog, CDialog)

KSceneEditorVisibleSetDialog::KSceneEditorVisibleSetDialog(CWnd* pParent /*=NULL*/)
	: CDialog(KSceneEditorVisibleSetDialog::IDD, pParent)
{
	m_lpSceneEditor = NULL;
	m_lpVisibleSet = NULL;
}

KSceneEditorVisibleSetDialog::~KSceneEditorVisibleSetDialog()
{
}

void KSceneEditorVisibleSetDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_MODELNAME, m_ButtonName);
	DDX_Control(pDX, IDC_COMBO_INLAYER, m_Combo_InLayer);
	DDX_Control(pDX, IDC_COMBO_OUTLAYER, m_Combo_OutLayer);
	DDX_Control(pDX, IDC_COMBO2, m_Combo_Depth);
}


BEGIN_MESSAGE_MAP(KSceneEditorVisibleSetDialog, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_MODELNAME, &KSceneEditorVisibleSetDialog::OnBnClickedButtonModelname)
	ON_WM_SHOWWINDOW()
	ON_CBN_SELCHANGE(IDC_COMBO_INLAYER, &KSceneEditorVisibleSetDialog::OnCbnSelchangeComboInlayer)
	ON_CBN_SELCHANGE(IDC_COMBO_OUTLAYER, &KSceneEditorVisibleSetDialog::OnCbnSelchangeComboOutlayer)
	ON_CBN_SELCHANGE(IDC_COMBO2, &KSceneEditorVisibleSetDialog::OnCbnSelchangeCombo2)
END_MESSAGE_MAP()


// KSceneEditorVisibleSetDialog message handlers

void KSceneEditorVisibleSetDialog::OnBnClickedButtonModelname()
{
	if(!m_lpVisibleSet)
		return;

	CFileDialog dlg(true, "Mesh", 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "剑网3D模型文件(*.mesh)|*.mesh||");
	if(dlg.DoModal() == IDOK)
	{
		TCHAR Name[256];
		wsprintf(Name,"%s",dlg.GetPathName());

		m_lpVisibleSet->LoadEdgeModel(Name);
	}
}

void KSceneEditorVisibleSetDialog::GetVisibleEntitySetInfo()
{
	if(!m_lpVisibleSet)
		return;

	TCHAR MeshName[256];
	TCHAR MtlName[256];
	TCHAR AniName[256];

	IEKG3DRepresentObject* pObject = (IEKG3DRepresentObject*)(m_lpVisibleSet);

	HRESULT hr = pObject->GetModelFileName(0,MeshName,MtlName,AniName);

	m_ButtonName.SetWindowText(MeshName);
}

void KSceneEditorVisibleSetDialog::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	if(bShow)
	{
		FillDepthCombo();

		GetVisibleEntitySetInfo();
		if(m_lpVisibleSet)
		{
			FillLayerCombo(&m_Combo_InLayer,m_lpVisibleSet->GetInsideLayerID());
			FillLayerCombo(&m_Combo_OutLayer,m_lpVisibleSet->GetOutsideLayerID());

			m_Combo_Depth.SetCurSel(m_lpVisibleSet->GetSetDepth());
		}
	}
}

void KSceneEditorVisibleSetDialog::OnCbnSelchangeComboInlayer()
{
	if(m_lpVisibleSet)
	{
		int nIndex = m_Combo_InLayer.GetCurSel();
		if(nIndex>=0)
		{
			DWORD LayerID = (DWORD)m_Combo_InLayer.GetItemData(nIndex);
			m_lpVisibleSet->SetInsideLayerID( LayerID );
		}
	}
}

void KSceneEditorVisibleSetDialog::OnCbnSelchangeComboOutlayer()
{
	if(m_lpVisibleSet)
	{
		int nIndex = m_Combo_OutLayer.GetCurSel();
		if(nIndex>=0)
		{
			DWORD LayerID = (DWORD)m_Combo_OutLayer.GetItemData(nIndex);
			m_lpVisibleSet->SetOutsideLayerID( LayerID );
		}
	}
}

void KSceneEditorVisibleSetDialog::FillLayerCombo(CComboBox* pCombo,DWORD dwCurID)
{
    HRESULT hRetCode = E_FAIL;
    LPCTSTR pcszLayerName = NULL;

	pCombo->SetCurSel(-1);
	int nCount = pCombo->GetCount();
	for(int i=0;i<nCount+1;i++)
	{
		pCombo->DeleteString(0);
	}

    KG_PROCESS_ERROR(m_lpSceneEditor);
	IEKG3DSceneEditorBase* pBase = NULL;
	m_lpSceneEditor->GetSceneEditorBase(&pBase);
	KG_PROCESS_ERROR(pBase);

	//for(int i=0;i<pBase->GetSceneLayerSize();i++)
	//{
	//	IEKG3DSceneLayer *piLayer = NULL;
	//	pBase->GetSceneLayer(i, &piLayer);//m_lpSceneEditor->m_vecSceneLayer[i];
	//	TCHAR Name[256];
	//	if(piLayer)
	//	{
 //           hRetCode = piLayer->GetName(&pcszLayerName);
 //           KGLOG_COM_PROCESS_ERROR(hRetCode);

	//		wsprintf(Name, "%d : %s", i, pcszLayerName);
	//		int nIndex = pCombo->AddString(Name);
	//		pCombo->SetItemData(nIndex,i);
	//		if(dwCurID == (DWORD)nIndex)
	//		{
	//			pCombo->SetCurSel(nIndex);
	//		}
	//	}
	//	else
	//	{
	//		wsprintf(Name,"%d : 未定义",i);
	//		int nIndex = pCombo->AddString(Name);
	//		pCombo->SetItemData(nIndex,i);
	//	}
	//}
Exit0:
    return;
}

void KSceneEditorVisibleSetDialog::OnCbnSelchangeCombo2()
{
	// TODO: Add your control notification handler code here
	if(m_lpVisibleSet)
	{
		m_lpVisibleSet->SetSetDepth( m_Combo_Depth.GetCurSel() );
	}
}

void KSceneEditorVisibleSetDialog::FillDepthCombo()
{
	m_Combo_Depth.SetCurSel(-1);
	int nCount = m_Combo_Depth.GetCount();
	for(int i=0;i<nCount+1;i++)
	{
		m_Combo_Depth.DeleteString(0);
	}

	for(int i=0;i<16;i++)
	{
		TCHAR Name[256];
		wsprintf(Name,"深度%d",i);
		m_Combo_Depth.AddString(Name);
	}
}
