// KSceneDataFlowEditorPanelView.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KSceneDataFlowEditorPanelView.h"
#include "KSceneDataFlowEditorDoc.h"
#include "IEEditor.h"
#include "KSceneDataFlowEditorDefineDialog.h"
#include "KSceneDataFlowEditorRunDialog.h"

#define GET_SCENE() \
	KSceneDataFlowEditorDoc* pDoc = (KSceneDataFlowEditorDoc*) GetDocument();\
	IEKG3DScene* pScene = pDoc->GetScene();\
	if(!pScene) return;

#define GET_EDITOR() \
	IEKG3DSceneDataFlowEditor* pEditor = NULL;\
	pScene->GetSceneDataFlowEditor(&pEditor); \
	if(!pEditor)	\
	return;

#define GET_BASE()\
	IEKG3DSceneEditorBase* pBase = NULL;\
	pEditor->GetSceneEditorBase(&pBase);\
	_ASSERTE(pBase);\
	if (!pBase) return;

// KSceneDataFlowEditorPanelView

IMPLEMENT_DYNCREATE(KSceneDataFlowEditorPanelView, CFormView)

KSceneDataFlowEditorPanelView::KSceneDataFlowEditorPanelView()
	: CFormView(KSceneDataFlowEditorPanelView::IDD)
{
	m_nOutputWindowID = -1;
	m_lpDailogDefine = NULL;
	m_lpDialogRun = NULL;

}

KSceneDataFlowEditorPanelView::~KSceneDataFlowEditorPanelView()
{
	SAFE_DELETE(m_lpDailogDefine);
	SAFE_DELETE(m_lpDialogRun);

	GET_SCENE();

	KG_PROCESS_ERROR(pScene);

	if(m_nOutputWindowID!=-1)
	{
		pScene->RemoveOutputWindow(m_nOutputWindowID);
		m_nOutputWindowID = -1;
	}
Exit0:
	;
}

void KSceneDataFlowEditorPanelView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PREVIEW, m_Static_PreView);

}

BEGIN_MESSAGE_MAP(KSceneDataFlowEditorPanelView, CFormView)
	ON_BN_CLICKED(IDC_BUTTON_DATADEFINE, &KSceneDataFlowEditorPanelView::OnBnClickedButtonDatadefine)
	ON_BN_CLICKED(IDC_BUTTON_RUN, &KSceneDataFlowEditorPanelView::OnBnClickedButtonRun)
	ON_BN_CLICKED(IDC_BUTTON_PROCESSOR, &KSceneDataFlowEditorPanelView::OnBnClickedButtonProcessor)
END_MESSAGE_MAP()


// KSceneDataFlowEditorPanelView diagnostics

#ifdef _DEBUG
void KSceneDataFlowEditorPanelView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void KSceneDataFlowEditorPanelView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// KSceneDataFlowEditorPanelView message handlers


void KSceneDataFlowEditorPanelView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	GET_SCENE();
	KGLOG_PROCESS_ERROR(pScene);

	GET_EDITOR();
	GET_BASE();


	pScene->AddOutputWindow("DataFlow Main", m_Static_PreView.GetSafeHwnd(), OUTPUTWND_DATAFLOWEDITOR_PREVIEW, 0,
		&m_nOutputWindowID);

Exit0:
	;
}

void KSceneDataFlowEditorPanelView::OnBnClickedButtonDatadefine()
{
	if(!m_lpDailogDefine)
	{
		GET_SCENE();
		GET_EDITOR();

		m_lpDailogDefine = new KSceneDataFlowEditorDefineDialog;
		m_lpDailogDefine->SetEditor(pEditor);
		m_lpDailogDefine->Create(KSceneDataFlowEditorDefineDialog::IDD,this);
	}
	m_lpDailogDefine->ShowWindow(SW_SHOWNORMAL);

}

void KSceneDataFlowEditorPanelView::OnBnClickedButtonRun()
{
	GET_SCENE();
	GET_EDITOR();
	if(!m_lpDialogRun)
	{

		m_lpDialogRun = new KSceneDataFlowEditorRunDialog;
		m_lpDialogRun->SetDataFlowEditor(pScene,pEditor);
		m_lpDialogRun->Create(KSceneDataFlowEditorRunDialog::IDD,this);
	}
	m_lpDialogRun->ShowWindow(SW_SHOWNORMAL);
	pEditor->RunDataFlow(TRUE);
}

void KSceneDataFlowEditorPanelView::OnBnClickedButtonProcessor()
{
	GET_SCENE();
	GET_EDITOR();
	pEditor->ToggleEditCurrentSelectedProcessor(TRUE);
}
