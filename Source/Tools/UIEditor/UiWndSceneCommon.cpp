#include "stdafx.h"
#include "UiWndSceneCommon.h"
#include "UiEditor.h"
#include "resource.h"
#include "Ui.h"
#include "ClientManage.h"
#include "UiFontManage.h"


KUiWndSceneCommon::KUiWndSceneCommon()
{
    m_szName[0] = '\0';
}

KUiWndSceneCommon::~KUiWndSceneCommon()
{
	if (m_hWnd)
	{
		::DestroyWindow(m_hWnd);
		m_hWnd = NULL;
	}
}

KUiWndWindow* KUiWndSceneCommon::Create()
{
	return new KUiWndSceneCommon;
}

int KUiWndSceneCommon::Init(HWND hWnd, RECT* pRc, const char *pszName)
{
    int nResult = false;

    KG_PROCESS_ERROR(hWnd);
    KG_PROCESS_ERROR(pszName);
    KG_PROCESS_ERROR(pRc);

    strcpy(m_szName, pszName);

	m_hWnd = ::CreateDialog((HINSTANCE)g_hPluginInstance, MAKEINTRESOURCE(IDD_WndSceneCommon), hWnd, (DLGPROC)WndProc);
    KG_PROCESS_ERROR(m_hWnd);

	::SetWindowPos(m_hWnd, HWND_TOP, pRc->left, pRc->top, pRc->right - pRc->left, pRc->bottom - pRc->top, SWP_SHOWWINDOW);

	::SetWindowLong(m_hWnd, GWL_USERDATA, (LONG)(KUiWndWindow*)this);

    nResult = true;
Exit0:
    return nResult;
}

void KUiWndSceneCommon::Release()
{
	delete this;
}

LRESULT CALLBACK KUiWndSceneCommon::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	KUiWndSceneCommon* pView = (KUiWndSceneCommon*)GetWindowLong(hWnd, GWL_USERDATA);
	if (pView == NULL)
		return 0;

	if (message == WM_COMMAND)
	{
		int nNotify = HIWORD(wParam);
		switch (nNotify)
		{
		case BN_CLICKED:
			pView->OnButtonClick(LOWORD(wParam));
			break;
		}
	}
	return 0;
}

void KUiWndSceneCommon::OnButtonClick(int nId)
{
    BOOL bModified = FALSE;
    int nValue = 0;

	switch (nId)
	{
	case IDC_WND_SCENE_DISABLE_TERRAIN:
        nValue = ::IsDlgButtonChecked(m_hWnd, nId) == BST_CHECKED;
        g_Ui.GetIni()->WriteInteger(m_szName, "DisableRenderTerrain", nValue);
		bModified = TRUE;
		break;
	case IDC_WND_SCENE_DISABLE_SKY_BOX:
        nValue = ::IsDlgButtonChecked(m_hWnd, nId) == BST_CHECKED;
        g_Ui.GetIni()->WriteInteger(m_szName, "DisableRenderSkyBox", nValue);
		bModified = TRUE;
		break;
	case IDC_WND_SCENE_ALPHA:
        nValue = ::IsDlgButtonChecked(m_hWnd, nId) == BST_CHECKED;
        g_Ui.GetIni()->WriteInteger(m_szName, "EnableAlpha", nValue);
		bModified = TRUE;
		break;
    default:
        break;
	}

	if (bModified)
	{
		g_ClientManager.UpDataWndInClient(g_Ui.GetIni(true));
        ShowErrorInformation();
	}
}

void KUiWndSceneCommon::UpdateData(const char* pszName, KUiWndWindowData& data, IIniFile* pIni)
{
	int nValue;

	if (!m_hWnd || !pIni || !pszName)
		return;

    strcpy(m_szName, pszName);

    pIni->GetInteger(pszName, "DisableRenderTerrain", 0, &nValue);
	::CheckDlgButton(m_hWnd, IDC_WND_SCENE_DISABLE_TERRAIN, nValue ? BST_CHECKED : BST_UNCHECKED);

    pIni->GetInteger(pszName, "DisableRenderSkyBox", 0, &nValue);
	::CheckDlgButton(m_hWnd, IDC_WND_SCENE_DISABLE_SKY_BOX, nValue ? BST_CHECKED : BST_UNCHECKED);

    pIni->GetInteger(pszName, "EnableAlpha", 0, &nValue);
	::CheckDlgButton(m_hWnd, IDC_WND_SCENE_ALPHA, nValue ? BST_CHECKED : BST_UNCHECKED);
}

void KUiWndSceneCommon::Show(int bShow, int nSubWnd)
{
	if (m_hWnd)
	{
		::ShowWindow(m_hWnd, bShow ? SW_SHOW : SW_HIDE);
	} 
}
