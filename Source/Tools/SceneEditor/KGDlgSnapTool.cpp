// KGDlgSnapTool.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KGDlgSnapTool.h"
#include "IEEditor.h"

// KGDlgSnapTool dialog
IMPLEMENT_DYNAMIC(KGDlgSnapTool, CDialog)

KGDlgSnapTool::KGDlgSnapTool(CWnd* pParent /*=NULL*/)
	: CDialog(KGDlgSnapTool::IDD, pParent)
{
	m_pEditor = NULL;
	m_fSclae = 2.0f;
	m_bExportTexture = FALSE;
}

KGDlgSnapTool::~KGDlgSnapTool()
{
}

void KGDlgSnapTool::SetEditor(IEKG3DSceneSceneEditor* pEditor)
{
	m_pEditor = pEditor;
}
void KGDlgSnapTool::EnableExportTexture(BOOL bEnable)
{
	m_bExportTexture = bEnable;
}
void KGDlgSnapTool::DoDataExchange(CDataExchange* pDX)
{
	DDX_Text(pDX, IDC_EDIT1, m_fSclae);
	DDX_Text(pDX, IDC_EDIT_TREELEAFSCALE, m_fLeafScale);
	DDX_Control(pDX, IDC_EDIT_TREELEAFSCALE, m_editLeafScale);
	DDX_Control(pDX, IDC_STATIC_LEAFSCALE, m_staticLeafScale);
	DDX_Control(pDX, IDC_CHECK_RENDERTREE, m_checkEnableTree);
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(KGDlgSnapTool, CDialog)
	ON_BN_CLICKED(ID_BUTTON_START, &KGDlgSnapTool::OnBnClickedOk)
	ON_BN_CLICKED(IDC_CHECK_RENDERTREE, &KGDlgSnapTool::OnEnableTree)
	ON_BN_CLICKED(IDCANCEL,&KGDlgSnapTool::OnBnClickedCancel)
	ON_EN_CHANGE(IDC_EDIT1, &KGDlgSnapTool::OnEnChangeEdit1)
END_MESSAGE_MAP()


// KGDlgSnapTool message handlers
void KGDlgSnapTool::ExportTerrainTexture()
{
	if(!m_pEditor)
		return ;

	TCHAR strFilePath[MAX_PATH];
	CFileDialog dlgSave(FALSE, 
		"tga", 
		NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"(*.tga)|*.tga||",
		this);

	if( dlgSave.DoModal() == IDOK)
	{
		OnOK();
		IEKG3DScene* pScene = NULL;
		m_pEditor->GetScene(&pScene);
		if (NULL != pScene)
		{
			pScene->RegisterListener(KM_LOADING_PERCENT, this);
		}	

		sprintf_s(strFilePath,
			MAX_PATH,
			"%s",
			dlgSave.GetPathName());

		m_pEditor->ExprotTerrainTexture(strFilePath);
	}

	
}
void KGDlgSnapTool::SaveSketch()
{
	BOOL bSavePloy = FALSE;
	TCHAR strFilePath[MAX_PATH];
	CFileDialog dlgSave(FALSE, 
		"bmp", 
		NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"位图文件 (*.bmp)|*.bmp||",
		this);

	OnOK();
	KG_PROCESS_SUCCESS(dlgSave.DoModal() != IDOK);

	sprintf_s(strFilePath,
		MAX_PATH,
		"%s",
		dlgSave.GetPathName());

	IEKG3DScene* pScene = NULL;
	m_pEditor->GetScene(&pScene);
	if (NULL != pScene)
	{
		pScene->RegisterListener(KM_LOADING_PERCENT, this);
	}	

	bSavePloy = m_pEditor->IsRenderPoly();
	m_pEditor->EnableRenderPoly(FALSE);
	m_pEditor->SetSnapToolScale(m_fSclae);
	g_pEngineManager->SetSpeedTreeLeafScale(m_fLeafScale);
	m_pEditor->RenderSceneSnap(strFilePath);
	m_pEditor->EnableRenderPoly(bSavePloy);
Exit1:
	;
}
void KGDlgSnapTool::OnBnClickedOk()
{
	TCHAR szDir[MAX_PATH];
	::GetCurrentDirectory(MAX_PATH,szDir);

	if(m_bExportTexture)
		ExportTerrainTexture();
	else
		SaveSketch();
	::SetCurrentDirectory(szDir);

	CDialog::OnOK();
}

void KGDlgSnapTool::OnOK()
{
	UpdateData();
	KG_PROCESS_ERROR(m_pEditor);

	m_pEditor->SetSnapToolScale(m_fSclae);
	g_pEngineManager->SetSpeedTreeLeafScale(m_fLeafScale);
Exit0:
	return;

}

void KGDlgSnapTool::OnEnableTree()
{
	if (m_checkEnableTree.GetCheck() == BST_CHECKED)
	{
        g_pEngineManager->SetRenderTreeFlag(TRUE);
		m_staticLeafScale.EnableWindow(TRUE);
		m_editLeafScale.EnableWindow(TRUE);
	}
	else
	{
        g_pEngineManager->SetRenderTreeFlag(FALSE);
		m_staticLeafScale.EnableWindow(FALSE);
		m_editLeafScale.EnableWindow(FALSE);
	}
}

BOOL KGDlgSnapTool::OnInitDialog()
{
	BOOL bRenderTree = 0;

    CDialog::OnInitDialog();

    g_pEngineManager->GetRenderTreeFlag(&bRenderTree);
	if (bRenderTree)
	{
		m_checkEnableTree.SetCheck(BST_CHECKED);
	}
	else
	{
		m_checkEnableTree.SetCheck(BST_UNCHECKED);
	}
    g_pEngineManager->GetSpeedTreeLeafScale(&m_fLeafScale);
	UpdateData(FALSE);
	if (m_bExportTexture)
	{
		this->SetWindowText("导出地表纹理贴图");
		m_checkEnableTree.ShowWindow(SW_HIDE);
		m_editLeafScale.ShowWindow(SW_HIDE);
		m_staticLeafScale.ShowWindow(SW_HIDE);
	}
	return TRUE;
}
void KGDlgSnapTool::UpdateDataButton()
{
	BOOL bRenderTree = 0;
	g_pEngineManager->GetRenderTreeFlag(&bRenderTree);
	if (bRenderTree)
	{
		m_checkEnableTree.SetCheck(BST_CHECKED);
	}
	else
	{
		m_checkEnableTree.SetCheck(BST_UNCHECKED);
	}
	g_pEngineManager->GetSpeedTreeLeafScale(&m_fLeafScale);
	UpdateData(FALSE);
}
void KGDlgSnapTool::OnBnClickedCancel()
{
	 g_pEngineManager->SetRenderTreeFlag(TRUE);
	 CDialog::OnCancel();
}

#define  KGWM_RELEASE_LISTENER (WM_USER+400)
void __stdcall KGDlgSnapTool::OnReceiveMessage( const KG3DMessage& Message , IKG3DMessageSenderBase* pSender )
{
	if (Message.m_uMessage == KM_SCENE_LOADING_PERCENT)
	{
		KG3DMessageSceneLoadingPercent* pPercent = (KG3DMessageSceneLoadingPercent*)Message.m_dwWParam;
		TCHAR	Buffer[MAX_PATH];
		Buffer[MAX_PATH-1] = '\0';
		_stprintf_s(Buffer, _T("加载地形(千份之)：%d；物件：%d，模型：%d")
			, (int)(pPercent->TerrainLoadingPercent * 1000)
			, (int)(pPercent->ObjectLoadingPercent * 1000)
			, (int)(pPercent->RestLoadingPercent * 1000)
			);

		this->SetWindowText(Buffer);
		this->RedrawWindow();
		if (pPercent->RestLoadingPercent > 1 - FLT_EPSILON)
		{
			this->PostMessage(KGWM_RELEASE_LISTENER, 0,0);
		}
	}
}
LRESULT KGDlgSnapTool::WindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	if (message == KGWM_RELEASE_LISTENER)
	{
		if(NULL != m_pEditor)
		{
			IEKG3DScene* pScene = NULL;
			m_pEditor->GetScene(&pScene);
			pScene->UnRegisterListener(KM_LOADING_PERCENT, this);
		}
	}
	return CDialog::WindowProc(message, wParam, lParam);
}
void KGDlgSnapTool::OnEnChangeEdit1()
{
	UpdateData(TRUE);
	if (m_bExportTexture)
		return ;
	if(m_fSclae>2 || m_fSclae <=0)
	{
		m_fSclae = 1;
		UpdateData(FALSE);
	}

}
