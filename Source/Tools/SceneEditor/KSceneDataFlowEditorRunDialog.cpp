// KSceneDataFlowEditorRunDialog.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KSceneDataFlowEditorRunDialog.h"
#include "IEEditor.h"

// KSceneDataFlowEditorRunDialog dialog

IMPLEMENT_DYNAMIC(KSceneDataFlowEditorRunDialog, CDialog)

KSceneDataFlowEditorRunDialog::KSceneDataFlowEditorRunDialog(CWnd* pParent /*=NULL*/)
	: CDialog(KSceneDataFlowEditorRunDialog::IDD, pParent)
{
	m_lpDataFlowEditor = NULL;
	m_lpScene = NULL;
	m_nOutputWindowID = -1;
}

KSceneDataFlowEditorRunDialog::~KSceneDataFlowEditorRunDialog()
{
}

void KSceneDataFlowEditorRunDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(KSceneDataFlowEditorRunDialog, CDialog)
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// KSceneDataFlowEditorRunDialog message handlers

void KSceneDataFlowEditorRunDialog::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	CDialog::OnOK();
}

LRESULT KSceneDataFlowEditorRunDialog::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	if(m_lpDataFlowEditor)
	{
		m_lpDataFlowEditor->ProcessRunMessage(message,wParam,lParam);
	}
	return CDialog::WindowProc(message, wParam, lParam);
}

BOOL KSceneDataFlowEditorRunDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void KSceneDataFlowEditorRunDialog::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	m_lpScene->OnResizeWindow(NULL);
	// TODO: Add your message handler code here
}

void KSceneDataFlowEditorRunDialog::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	if(m_nOutputWindowID!=-1)
	{
		m_lpScene->RemoveOutputWindow(m_nOutputWindowID);
		m_nOutputWindowID = -1;
		m_lpDataFlowEditor->RunDataFlow(FALSE);
	}

	CDialog::OnClose();
}

void KSceneDataFlowEditorRunDialog::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	if (bShow)
	{
		m_lpScene->AddOutputWindow("DataFlow Run", GetSafeHwnd(), OUTPUTWND_DATAFLOWEDITOR_RUN, 0,
			&m_nOutputWindowID);
	}
	else
	{
		m_lpScene->RemoveOutputWindow(m_nOutputWindowID);
		m_nOutputWindowID = -1;
		m_lpDataFlowEditor->RunDataFlow(FALSE);

	}
	// TODO: Add your message handler code here
}
