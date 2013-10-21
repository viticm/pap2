// KSceneModelEditorSFXNewDialog.cpp : implementation file
//

#include "stdafx.h"
#include "sceneeditor.h"
#include "KSceneModelEditorSFXNewDialog.h"
#include "IEKG3DModelSpeedTree.h"
//#include "KG3DSceneModelEditor.h"
#include "MeshEditorDoc.h"
#include "MeshEditorview.h"
#include "IEKG3DModelTable.h"
//#include "SceneEditorCommon.h"
#include "IEEditor.h"
#include "KSceneModelEditorFrameWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// KSceneModelEditorSFXNewDialog dialog


KSceneModelEditorSFXNewDialog::KSceneModelEditorSFXNewDialog(CWnd* pParent /*=NULL*/)
	: CDialog(KSceneModelEditorSFXNewDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(KSceneModelEditorSFXNewDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_lpSceneModelEditor = NULL;
	m_lpSceneModelDoc = NULL;
}


void KSceneModelEditorSFXNewDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(KSceneModelEditorSFXNewDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(KSceneModelEditorSFXNewDialog, CDialog)
	//{{AFX_MSG_MAP(KSceneModelEditorSFXNewDialog)
	ON_BN_CLICKED(IDC_BUTTON_BILLBOARD, OnButtonBillboard)
	ON_BN_CLICKED(IDC_BUTTON_BELT, OnButtonBelt)
	ON_BN_CLICKED(IDC_BUTTON_BLADE, OnButtonBlade)
	ON_BN_CLICKED(IDC_BUTTON_FOGV, OnButtonFogv)
	ON_BN_CLICKED(IDC_BUTTON_LIGHT, OnButtonLight)
	ON_BN_CLICKED(IDC_BUTTON_PARTICLE, OnButtonParticle)
	ON_BN_CLICKED(IDC_BUTTON_SPEEDTREE, OnButtonSpeedtree)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON8, OnBnClickedButton8)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// KSceneModelEditorSFXNewDialog message handlers

void KSceneModelEditorSFXNewDialog::OnButtonBillboard() 
{
	/*if(m_lpSceneModelEditor)
	{
		ModelSFX::KModelSFXBillBoard* pBillboard = NULL;

		g_cMeshTable.Get1NewXMesh((LPMODEL*)(&pBillboard),MODELTYPE_BILLBOARD);
		pBillboard->LoadMesh("");

		m_lpSceneModelEditor->AddRenderMesh(pBillboard);
		m_lpSceneModelEditor->AddSelectableMesh(pBillboard);

	}*/
	
	OnOK();
}

void KSceneModelEditorSFXNewDialog::OnButtonBelt() 
{
	/*if(m_lpSceneModelEditor)
	{
		ModelSFX::KModelBelt* pBillboard = NULL;

		g_cMeshTable.Get1NewXMesh((LPMODEL*)(&pBillboard),MODELTYPE_BELT);
		pBillboard->LoadMesh("");

		m_lpSceneModelEditor->AddRenderMesh(pBillboard);
		m_lpSceneModelEditor->AddSelectableMesh(pBillboard);
		m_lpSceneModelEditor->AddSelectedMesh(pBillboard);
	}*/
	OnOK();

}

void KSceneModelEditorSFXNewDialog::OnButtonBlade() 
{
	/*if(m_lpSceneModelEditor)
	{
		ModelSFX::KModelSFXBlade* pModel = NULL;

		g_cMeshTable.Get1NewXMesh((LPMODEL*)(&pModel),MODELTYPE_BLADE);
		pModel->LoadMesh("");

		m_lpSceneModelEditor->AddRenderMesh(pModel);
		m_lpSceneModelEditor->AddSelectableMesh(pModel);
		m_lpSceneModelEditor->m_SelectedList.clear();
		m_lpSceneModelEditor->AddSelectedMesh(pModel);
	}*/
	OnOK();

}

void KSceneModelEditorSFXNewDialog::OnButtonFogv() 
{
	//if(m_lpSceneModelEditor)
	//{
	//	KDlg_Create_LVF dlg(this);
	//	if(dlg.DoModal() != IDD_Create_LVF_Create)
	//	{
	//		return;
	//	}

	//	KVolumeFog* pModel = dlg.GetVolumeFog(m_lpSceneModelEditor->m_lpMeshTable);

	//	//;
	//	//pModel->LoadMesh("模型文件\\Test.lvf");

	//	m_lpSceneModelEditor->AddRenderMesh(pModel);
	//	m_lpSceneModelEditor->AddSelectableMesh(pModel);
	//}
	OnOK();

}

void KSceneModelEditorSFXNewDialog::OnButtonLight() 
{
	/*if(m_lpSceneModelEditor)
	{
		KDlg_Create_Lightning dlg(this);
		if(dlg.DoModal() != IDOK)
		{
			return;
		}
		KLightning* pModel = dlg.GetLightning(m_lpSceneModelEditor->m_lpMeshTable);

		m_lpSceneModelEditor->AddRenderMesh(pModel);
		m_lpSceneModelEditor->AddSelectableMesh(pModel);
	}*/
	OnOK();

}

void KSceneModelEditorSFXNewDialog::OnButtonParticle() 
{
	/*if(m_lpSceneModelEditor)
	{
		KModelParticleSystem* pModel = NULL;

		g_cMeshTable.Get1NewXMesh((LPMODEL*)(&pModel),MODELTYPE_PARTICLESYSTEM);
		pModel->LoadMesh("");

		m_lpSceneModelEditor->AddRenderMesh(pModel);
		m_lpSceneModelEditor->AddSelectableMesh(pModel);

	}*/
	OnOK();

}

void KSceneModelEditorSFXNewDialog::OnButtonSpeedtree() 
{
    HRESULT hrResult  = E_FAIL;
    HRESULT hrRetCode = E_FAIL;
    IEKG3DModel *pModel = NULL;
    IEKG3DModelTable *pModelTable = NULL;
    IEKG3DModelSpeedTree* pModelTree = NULL;
    IEKG3DModel *pModelTreeModel = NULL;
    INT nNumBones = 0;
    //TCHAR strPartFileName[MAX_PATH];
    POSITION ViewPos;
    DWORD dwType;
    CString strSpdTreeName;
    LPCSTR strBuffer;

	KSceneModelEditorFrameWnd *pFrameWnd = NULL;
    int nReturn = MessageBox("要把现有模型转换成speedtree吗?", "提示", MB_YESNO);
    KG_PROCESS_SUCCESS(nReturn != IDYES);
    KG_PROCESS_ERROR(m_lpSceneModelEditor);

    hrRetCode = m_lpSceneModelDoc->GetCurrentModel(&pModel);
    KG_COM_PROCESS_ERROR (hrRetCode);

    pModel->GetType(&dwType);
    KG_PROCESS_ERROR(dwType == MESHTYPE_DEFAULT);

    hrRetCode = pModel->GetNumBones(&nNumBones);
    KG_COM_PROCESS_ERROR(hrRetCode);
    if (!nNumBones)
    {
        MessageBox("转化为SpeedTree失败，确认是带骨胳的模型.");
        KG_PROCESS_ERROR(nNumBones);
    }

    hrRetCode = g_pEngineManager->GetIEKG3DModelTable(&pModelTable);
    KG_COM_PROCESS_ERROR (hrRetCode);

    hrRetCode = pModelTable->IEGet1NewResourse(&pModelTreeModel, MESHTYPE_SPEEDTREE);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    hrRetCode = pModelTreeModel->GetIEKG3DModelSpeedTree(&pModelTree);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);				

    hrRetCode = pModelTree->Init(pModel);
    if (FAILED(hrRetCode))
    {
        MessageBox("转化为SpeedTree失败");
    }
    KG_COM_PROCESS_ERROR(hrRetCode);

    pModel->GetName(&strBuffer);
	strSpdTreeName = strBuffer;
	strSpdTreeName.MakeLower();

	m_lpSceneModelEditor->OpenModel(pModelTreeModel, FALSE);
    ViewPos = m_lpSceneModelDoc->GetFirstViewPosition();
    if (ViewPos)
    {
        KSceneModelEditorView* pView = static_cast<KSceneModelEditorView*>(m_lpSceneModelDoc->GetNextView(ViewPos));
        if (pView)
        {
            pView->Zoom();
        }
    }

	    
    int nStartPos = strSpdTreeName.Find(".mesh", 0);
    strSpdTreeName.Insert(nStartPos, "_ST");
  
    m_lpSceneModelDoc->SetPathName(strSpdTreeName + ".stree");

	CMDIFrameWnd *pMainFrame = (CMDIFrameWnd*)AfxGetMainWnd();
	KG_PROCESS_ERROR(pMainFrame);

	pFrameWnd = (KSceneModelEditorFrameWnd*)pMainFrame->GetActiveFrame();
	KG_PROCESS_ERROR(pFrameWnd);

	pFrameWnd->OnModelChange(0, 0);

Exit1:
    hrResult = S_OK;
Exit0:
	/*if (FAILED(hrResult))
	{
		SAFE_RELEASE(pModel);
	}*/
	OnOK();
}

void KSceneModelEditorSFXNewDialog::OnBnClickedButton8()
{
	/*if(m_lpSceneModelEditor)
	{
		KModelSpeedTree* pModel = NULL;

		g_cMeshTable.Get1NewXMesh((LPMODEL*)(&pModel),MODELTYPE_SCENESFX);
		pModel->LoadMesh("");

		m_lpSceneModelEditor->AddRenderMesh(pModel);
		m_lpSceneModelEditor->AddSelectableMesh(pModel);
		m_lpSceneModelEditor->m_SelectedList.clear();
		m_lpSceneModelEditor->AddSelectedMesh(pModel);
	}*/
	OnOK();
}
