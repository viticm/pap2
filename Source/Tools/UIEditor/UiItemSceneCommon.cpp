////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : UiItemSceneCommon.cpp
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2007-4-4 11:59:16
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "Stdafx.h"
#include <Windows.h>
#include "UiItemBox.h"
#include "UiEditor.h"
#include "resource.h"
#include "UiItemCommon.h"
#include "UiItemSceneCommon.h"
#include "ClientManage.h"
////////////////////////////////////////////////////////////////////////////////

KUiItemSceneCommon::KUiItemSceneCommon()
{
	m_szName[0] = '\0';
}

KUiItemSceneCommon::~KUiItemSceneCommon()
{
	if (m_hWnd)
	{
		::DestroyWindow(m_hWnd);
		m_hWnd = NULL;
	}
}

KUiWndWindow *KUiItemSceneCommon::Create()
{
	KUiItemSceneCommon* pWnd = new KUiItemSceneCommon;
	return pWnd;
}

int KUiItemSceneCommon::Init(HWND hWnd, RECT* pRc, const char *pszName)
{
	int nResult = false;
	HWND hwndCombo = NULL;
	HWND hSpin = NULL;
	KG_PROCESS_ERROR(hWnd);
	KG_PROCESS_ERROR(pRc);
	KG_PROCESS_ERROR(pszName);

	strncpy(m_szName, pszName, sizeof(m_szName));
	m_hWnd = ::CreateDialog(
		(HINSTANCE)g_hPluginInstance, 
		MAKEINTRESOURCE(IDD_ItemSceneCommon), 
		hWnd, (DLGPROC)WndProc
		);
	KG_PROCESS_ERROR(m_hWnd);

	::SetWindowPos(m_hWnd, HWND_TOP, pRc->left, pRc->top,
		pRc->right - pRc->left, pRc->bottom - pRc->top,
		SWP_SHOWWINDOW);
	::SetWindowLong(m_hWnd, GWL_USERDATA, (LONG)this);

	nResult = true;
Exit0:
	if (!nResult)
		Release();
	return nResult;
}

void KUiItemSceneCommon::Release()
{
	delete this;
}

LRESULT CALLBACK KUiItemSceneCommon::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	KUiItemSceneCommon*	pView = (KUiItemSceneCommon*)GetWindowLong(hWnd, GWL_USERDATA);
	if (!pView)
		return 0;
	if (message == WM_COMMAND)
	{
		int nNotify = HIWORD(wParam);
		switch(nNotify)
		{
		case EN_CHANGE:
			pView->OnUpdateEditText(LOWORD(wParam));
			break;
		case 0:
			pView->OnButtonClick(LOWORD(wParam));
			break;
		}
	}
	return 0;
}

void KUiItemSceneCommon::OnButtonClick(int nBtnId)
{
	int nRetCode = false;
	int nValue = 0;
	char *pszkey = NULL;
	switch(nBtnId)
	{
	case IDC_ITEM_SCENE_RENDER_TERRAIN:
		nValue = ::IsDlgButtonChecked(m_hWnd, IDC_ITEM_SCENE_RENDER_TERRAIN);
		pszkey = "RenderTerrain";
		break;
	case IDC_ITEM_SCENE_RENDER_SKYBOX:
		nValue = ::IsDlgButtonChecked(m_hWnd, IDC_ITEM_SCENE_RENDER_SKYBOX);
		pszkey = "RenderSky";
		break;
	case IDC_ITEM_SCENE_ENABLE_ALPHA:
		nValue = ::IsDlgButtonChecked(m_hWnd, IDC_ITEM_SCENE_ENABLE_ALPHA);
		pszkey = "EnableAlpha";
		break;
	}

	if (pszkey)
	{
		g_Ui.GetIni()->WriteInteger(m_szName, pszkey, nValue);
		g_ClientManager.UpDataWndInClient(g_Ui.GetIni(true));
		ShowErrorInformation();
	}

}

void KUiItemSceneCommon::OnUpdateEditText(int nId)
{
}

void KUiItemSceneCommon::UpdateData(const char* pszName, KUiWndWindowData& data, IIniFile* pIni)
{
	int nRetCode = false;
	int nValue = 0;
	KG_PROCESS_ERROR(pszName);
	KG_PROCESS_ERROR(pIni);

	strncpy(m_szName, pszName, sizeof(m_szName));

	pIni->GetInteger(m_szName, "RenderTerrain", 1, &nValue);
	if (nValue)
		CheckDlgButton(m_hWnd, IDC_ITEM_SCENE_RENDER_TERRAIN, BST_CHECKED);
	else
		CheckDlgButton(m_hWnd, IDC_ITEM_SCENE_RENDER_TERRAIN, BST_UNCHECKED);

	pIni->GetInteger(m_szName, "RenderSky", 1, &nValue);
	if (nValue)
		CheckDlgButton(m_hWnd, IDC_ITEM_SCENE_RENDER_SKYBOX, BST_CHECKED);
	else
		CheckDlgButton(m_hWnd, IDC_ITEM_SCENE_RENDER_SKYBOX, BST_UNCHECKED);

	pIni->GetInteger(m_szName, "EnableAlpha", 1, &nValue);
	if (nValue)
		CheckDlgButton(m_hWnd, IDC_ITEM_SCENE_ENABLE_ALPHA, BST_CHECKED);
	else
		CheckDlgButton(m_hWnd, IDC_ITEM_SCENE_ENABLE_ALPHA, BST_UNCHECKED);

Exit0:
	return;
}

void KUiItemSceneCommon::Show(int bShow, int nSubWnd /* = -1 */)
{
	if (m_hWnd)
		::ShowWindow(m_hWnd, bShow ? SW_SHOW : SW_HIDE);
}
