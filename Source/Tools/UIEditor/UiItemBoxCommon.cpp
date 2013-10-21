////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : UiItemBoxCommon.cpp
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2006-3-1 15:35:38
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "Stdafx.h"
#include <Windows.h>
#include <tchar.h>
#include "UiItemBoxCommon.h"
#include "UiEditor.h"
#include "resource.h"
#include "UiItemCommon.h"
#include "ClientManage.h"
////////////////////////////////////////////////////////////////////////////////

KUiItemBoxCommon::KUiItemBoxCommon()
{
	m_szName[0] = '\0';
}

KUiItemBoxCommon::~KUiItemBoxCommon()
{
	if (m_hWnd)
	{
		::DestroyWindow(m_hWnd);
		m_hWnd = NULL;
	}
}

KUiWndWindow *KUiItemBoxCommon::Create()
{
	KUiItemBoxCommon* pWnd = new KUiItemBoxCommon;
	return pWnd;
}

int KUiItemBoxCommon::Init(HWND hWnd, RECT* pRc, const char *pszName)
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
		MAKEINTRESOURCE(IDD_ItemBoxCommon), 
		hWnd, (DLGPROC)WndProc
		);
	KG_PROCESS_ERROR(m_hWnd);

	::SetWindowPos(m_hWnd, HWND_TOP, pRc->left, pRc->top,
		pRc->right - pRc->left, pRc->bottom - pRc->top,
		SWP_SHOWWINDOW);
	::SetWindowLong(m_hWnd, GWL_USERDATA, (LONG)this);

	hSpin = ::GetDlgItem(m_hWnd, IDC_ITEM_BOX_INDEX_SPIN);
	if (hSpin) 
		::SendMessage(hSpin, UDM_SETRANGE, 0, MAKELPARAM(10000, 0));

	nResult = true;
Exit0:
	if (!nResult)
		Release();
	return nResult;
}

void KUiItemBoxCommon::Release()
{
	delete this;
}

LRESULT CALLBACK KUiItemBoxCommon::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	KUiItemBoxCommon*	pView = (KUiItemBoxCommon*)GetWindowLong(hWnd, GWL_USERDATA);
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

void KUiItemBoxCommon::OnButtonClick(int nBtnId)
{
}

void KUiItemBoxCommon::OnUpdateEditText(int nId)
{
	int nValue = 0;
	int nUpdateShower = true;
	switch(nId)
	{
	case IDC_ITEM_BOX_INDEX:
		nValue = ::GetDlgItemInt(m_hWnd, IDC_ITEM_BOX_INDEX, NULL, true);
		g_Ui.GetIni()->WriteInteger(m_szName, "Index", nValue);
		break;
	default:
		nUpdateShower = false;
		break;
	}

	if (nUpdateShower)
	{
		g_ClientManager.UpDataWndInClient(g_Ui.GetIni(true));
		ShowErrorInformation();
	}
}

void KUiItemBoxCommon::UpdateData(const char* pszName, KUiWndWindowData& data, IIniFile* pIni)
{
	int nRetCode = false;
	int nValue = 0;
	KG_PROCESS_ERROR(pszName);
	KG_PROCESS_ERROR(pIni);

	strncpy(m_szName, pszName, sizeof(m_szName));

	pIni->GetInteger(m_szName, "Index", 0, &nValue);
	::SetDlgItemInt(m_hWnd, IDC_ITEM_BOX_INDEX, nValue, TRUE);

Exit0:
	return;
}

void KUiItemBoxCommon::Show(int bShow, int nSubWnd /* = -1 */)
{
	if (m_hWnd)
		::ShowWindow(m_hWnd, bShow ? SW_SHOW : SW_HIDE);
}


