// TopViewForm.cpp : Implementation of CTopViewForm

#include "stdafx.h"
#include "TopViewForm.h"
#include "IEKG3DScene.h"

// CTopViewForm
void CTopViewForm::InitTopView(IEKG3DSceneSceneEditor* pEditor)
{
	m_bInited = true;
	m_nOutputWndID = -1;
	m_bSelecting = FALSE;
	m_bAddPolyVertex = FALSE;
	m_bLBPushed = FALSE;
	m_bMBPushed = FALSE;
	m_bRBPushed = FALSE;

	m_lpSceneEditor = pEditor;
	m_lpSceneEditor->AddOutputWindow("TopView", m_hWnd, OUTPUTWND_SCENEEDITOR_TOPVIEW, 0, &m_nOutputWndID);
	IEKG3DScene* pScene = NULL;
	m_lpSceneEditor->GetScene(&pScene);
	pScene->OnResizeWindow(NULL);

	D3DXVECTOR3 CamMainPos = m_lpSceneEditor->GetCameraMainPos();
	m_lpSceneEditor->SetCameraTopPos(CamMainPos.x, CamMainPos.z);
}

void CTopViewForm::UnInitTopView()
{
	m_lpSceneEditor->RemoveOutputWindow(m_nOutputWndID);
	m_nOutputWndID = -1;
}

LRESULT CTopViewForm::OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	UnInitTopView();	
	DestroyWindow();
	return 0;
}

LRESULT CTopViewForm::OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	m_bLBPushed = TRUE;
	GetCursorPos(&m_MouseDown);
	m_vCameraPos = m_lpSceneEditor->GetCameraTopPos();
	return 0;
}

LRESULT CTopViewForm::OnMButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	m_bMBPushed = TRUE;
	GetCursorPos(&m_MouseDown);
	m_vCameraPos = m_lpSceneEditor->GetCameraTopPos();
	return 0;
}

LRESULT CTopViewForm::OnRButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	m_bRBPushed = TRUE;
	GetCursorPos(&m_MouseDown);
	m_vCameraPos = m_lpSceneEditor->GetCameraTopPos();
	return 0;
}

LRESULT CTopViewForm::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (m_bMBPushed)
	{
		POINT ptCurrent;
		GetCursorPos(&ptCurrent);
		m_lpSceneEditor->SetCameraTopPos(m_vCameraPos.x - (ptCurrent.x - m_MouseDown.x) * m_lpSceneEditor->GetCameraTopScale(),
			m_vCameraPos.z + (ptCurrent.y - m_MouseDown.y) * m_lpSceneEditor->GetCameraTopScale()
			);
	}
	return 0;
}

LRESULT CTopViewForm::OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	m_bLBPushed = FALSE;
	return 0;
}

LRESULT CTopViewForm::OnMButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	m_bMBPushed = FALSE;
	return 0;
}

LRESULT CTopViewForm::OnRButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	m_bRBPushed = FALSE;
	return 0;
}

LRESULT CTopViewForm::OnLButtonDblClk(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	IEKG3DSceneOutputWnd *piSceneCurOutputWnd = NULL;
	m_lpSceneEditor->GetCurOutputWnd(&piSceneCurOutputWnd);
	if(!piSceneCurOutputWnd)
		return 0;
	//m_lpSceneEditor->SetNPCPosition(TRUE);
	return 0;
}
