////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : UIEditorMgr.cpp
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2007-12-5 14:57:49
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "UIEditorMgr.h"
#include "../config/kdebugconfig.h"
#include "../elem/wndframe.h"
#include "../elem/wndbutton.h"
#include "../elem/wndwindow.h"
#include "../elem/Components/itemhandle.h"
#include "../elem/buildagent/iniwndbuilder.h"
#include "../script/kbasescripttable.h"
#include "../script/kbasescripttable.h"
#include "../script/kscriptmgr.h"
#include "../scheme/kfontschememgr.h"
#include "../driver/kimageinfo.h"
#include "../KGUI.h"


////////////////////////////////////////////////////////////////////////////////

namespace UI
{

KUIEditorMgr::KUIEditorMgr()
{
	m_pUiEditor = NULL;
	m_hUIEditor = NULL;
}

KUIEditorMgr::~KUIEditorMgr()
{
	CLoseUIEditor();
}


int KUIEditorMgr::OpenUIEditor(int nForce)
{
	int nResult = false;
	IIniFile *pIni = NULL;
	char szUIEditerFilePath[MAX_PATH];
	szUIEditerFilePath[0] = '\0';
	FnCreatePlugin pFnCreatePlugin = NULL;

	KG_PROCESS_SUCCESS(m_hUIEditor);

	pIni = g_OpenIniFile("\\config.ini");
	KG_PROCESS_ERROR(pIni);

	if (!nForce)
	{
		int nEnable = false;
		pIni->GetInteger("UIEditor", "Enable", 0, &nEnable);
		KG_PROCESS_ERROR(nEnable);
	}

	pIni->GetString("UIEditor", "Path", "", szUIEditerFilePath, _countof(szUIEditerFilePath));
	KG_PROCESS_ERROR(szUIEditerFilePath[0]);
	KG_PROCESS_ERROR(strlen(szUIEditerFilePath) < _countof(szUIEditerFilePath) - 20);

#ifdef _DEBUG
	strcat(szUIEditerFilePath, "D.dll");
#else
	strcat(szUIEditerFilePath, ".dll");
#endif

	m_hUIEditor = ::LoadLibraryA(szUIEditerFilePath);
	KG_PROCESS_ERROR(m_hUIEditor);

	pFnCreatePlugin = (FnCreatePlugin)GetProcAddress(m_hUIEditor, FN_CREATE_PLUGIN);
	KG_PROCESS_ERROR(pFnCreatePlugin);

	m_pUiEditor = pFnCreatePlugin();
	KG_PROCESS_ERROR(m_pUiEditor);

	m_pUiEditor->SetCallback(PCF_UI_BEHAVIOR, UIEditorCallback);

	m_pUiEditor->Create(g_pUI->m_hWnd, NULL);	
	m_pUiEditor->Init();

Exit1:
	nResult = true;
Exit0:
	SAFE_RELEASE(pIni);
	if (!nResult)
	{
		if (m_hUIEditor)
		{
			FreeLibrary(m_hUIEditor);
			m_hUIEditor = NULL;
		}
	}
	return nResult;
}

int KUIEditorMgr::CLoseUIEditor()
{
	SAFE_RELEASE(m_pUiEditor);
	if (m_hUIEditor)
	{
		::FreeLibrary(m_hUIEditor);
		m_hUIEditor = NULL;
	}
	return true;
}

int KUIEditorMgr::IsUIEditorOpend()
{
	if (m_hUIEditor)
		return true;
	return false;
}

LRESULT KUIEditorMgr::UIEditorCallback(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRet = 0;

	switch (uMsg)
	{
	case PCB_UI_GET_IMAGE_SIZE:
		if (wParam)
		{
			SIZE sizeImage = { 0, 0 };
			KImageInfo* pImageInfo = g_pUI->m_ImageInfoMgr.GetImageInfo((LPCSTR)wParam);
			if (pImageInfo)
			{
				sizeImage = pImageInfo->GetImageSize();
				lRet = MAKELRESULT(sizeImage.cx, sizeImage.cy);
			}
		}
		break;
	case PCB_UI_GET_IMAGE_FRAME_COUNT:
		{
			KImageInfo* pImageInfo = g_pUI->m_ImageInfoMgr.GetImageInfo((LPCSTR)wParam);
			if (pImageInfo)
			{
				*(int*)lParam = pImageInfo->GetFrameCount();
				lRet = 1;
			}
		}
		break;
	case PCB_UI_GET_IMAGE_FRAME_SIZE:
		{
			KPluginUIImage* pParam = (KPluginUIImage*)wParam;
			if(pParam)
			{
				KImageInfo* pImageInfo = g_pUI->m_ImageInfoMgr.GetImageInfo(pParam->szFilePath);
				if (pImageInfo)
				{
					*(LPSIZE)lParam = pImageInfo->GetFrameSize(pParam->nFrame);
					lRet = 1;
				}
			}
		}
		break;
	case PCB_UI_RELOAD_COLOR_SCHEME:
		{
			g_pUI->m_ColorSchemeMgr.Exit();
			lRet = g_pUI->m_ColorSchemeMgr.Init();
		}
		break;
	case PCB_UI_RELOAD_FONT_SCHEME:
		{
			lRet = g_pUI->m_FontSchemeMgr.LoadScheme();
		}
		break;
	case PCB_UI_RELOAD_STRING_SCHEME:
		{
			// lRet = g_pUI->m_GlobalStrValuableMgr.LoadScheme();
		}
		break;
	case PCB_UI_WINDOW_GET_POS:
		lRet = 0xFFFFFFFF;
		if (wParam)
		{
			LPCSTR pszTreePath = LPCSTR(wParam);
			LPCSTR pszItemTreePath = LPCSTR(lParam);
			KWndWindow * pWnd = g_pUI->m_WndStation.SearchWindow(pszTreePath);
			if (!pWnd)
				break;

			float fX = 0.0f;
			float fY = 0.0f;
			float fParentX = 0.0f;
			float fParentY = 0.0f;

			if (!pszItemTreePath)
			{
				if (!g_pUI->m_WndStation.IsRootWindow(pWnd))
				{
					KWndWindow *pParent = pWnd->GetParent();
					fParentX = pParent->GetAbsX();
					fParentY = pParent->GetAbsY();
				}

				fX = pWnd->GetAbsX() / g_pUI->m_WndStation.GetUIScale();
				fY = pWnd->GetAbsY() / g_pUI->m_WndStation.GetUIScale();

				lRet = MAKELPARAM((int)(fX - fParentX), (int)(fY - fParentY)); 
				break;
			}

			KItemHandle *pHandle = pWnd->GetItemHandle();
			if (!pHandle)
				break;
			if (pszItemTreePath[0] == '\0')
			{
				fParentX = pWnd->GetAbsX();
				fParentY = pWnd->GetAbsY();
				pHandle->GetAbsPos(fX, fY);
				fX /= g_pUI->m_WndStation.GetUIScale();
				fY /= g_pUI->m_WndStation.GetUIScale();
				lRet = MAKELPARAM((int)(fX - fParentX), (int)(fY - fParentY)); 
			}
			else
			{
				KItemHandle *pParent = NULL;
				KItemNull* pItem = pHandle->GetItemByTreePath(pszItemTreePath, &pParent);
				if (pParent && pItem)
				{
					pParent->GetAbsPos(fParentX, fParentY);
					pItem->GetAbsPos(fX, fY);
					fX /= g_pUI->m_WndStation.GetUIScale();
					fY /= g_pUI->m_WndStation.GetUIScale();
					lRet = MAKELPARAM((int)(fX - fParentX), (int)(fY - fParentY)); 
				}
			}
		}
		break;
	case PCB_UI_WINDOW_GET_SIZE:
		lRet = 0xFFFFFFFF;
		if (wParam)
		{
			LPCSTR pszTreePath = LPCSTR(wParam);
			LPCSTR pszItemTreePath = LPCSTR(lParam);
			KWndWindow * pWnd = g_pUI->m_WndStation.SearchWindow(pszTreePath);
			if (!pWnd)
				break;
			float w, h;
			if (!pszItemTreePath)
			{
				pWnd->GetSize(&w, &h);
				w /= g_pUI->m_WndStation.GetUIScale();
				h /= g_pUI->m_WndStation.GetUIScale();
				lRet = MAKELPARAM((int)w, (int)h); 
				break;
			}

			KItemHandle *pHandle = pWnd->GetItemHandle();
			if (!pHandle)
				break;
			if (pszItemTreePath[0] == '\0')
			{
				pHandle->GetSize(w, h);
				w /= g_pUI->m_WndStation.GetUIScale();
				h /= g_pUI->m_WndStation.GetUIScale();
				lRet = MAKELPARAM((int)w, (int)h); 
			}
			else
			{
				KItemNull *pItem = pHandle->GetItemByTreePath(pszItemTreePath);
				if (pItem)
				{
					pItem->GetSize(w, h);
					w /= g_pUI->m_WndStation.GetUIScale();
					h /= g_pUI->m_WndStation.GetUIScale();
					lRet = MAKELPARAM((int)w, (int)h); 
				}
			}
		}
		break;
	case PCB_UI_SHOW_MOUSE_WND:
		{
			DWORD dwState = 0;
			KBaseScriptTable::HostGetDebugModeState("ShowMouseWnd", dwState);
			if (dwState == 1 && lParam == 0 || dwState == 0 && lParam == 1)
				KBaseScriptTable::HostDebug("ShowMouseWnd");
		}
		break;
	case PCB_UI_ENABLE_RENDER_UI:
		{
			DWORD dwState = 0;
			KBaseScriptTable::HostGetDebugModeState("EnableRenderUI", dwState);
			if (dwState == 1 && lParam == 0 || dwState == 0 && lParam == 1)
			{
				KBaseScriptTable::HostDebug("EnableRenderUI");
			}
		}
		break;
	case PCB_UI_SHOW_FRAME:
		{
			DWORD dwState = 0;
			KBaseScriptTable::HostGetDebugModeState("ShowFrame", dwState);
			if (dwState == 1 && lParam == 0 || dwState == 0 && lParam == 1)
				KBaseScriptTable::HostDebug("ShowFrame");
		}
		break;
	case PCB_UI_SHOW_NAME:
		{
			DWORD dwState = 0;
			KBaseScriptTable::HostGetDebugModeState("ShowName", dwState);
			if (dwState == 1 && lParam == 0 || dwState == 0 && lParam == 1)
				KBaseScriptTable::HostDebug("ShowName");
		}
		break;
	case PCB_UI_SHOW_FRAME_DRAG_AREA_BODER:
		{
			DWORD dwState = 0;
			KBaseScriptTable::HostGetDebugModeState("ShowFrameDragAreaBoder", dwState);
			if (dwState == 1 && lParam == 0 || dwState == 0 && lParam == 1)
				KBaseScriptTable::HostDebug("ShowFrameDragAreaBoder");
		}
		break;
	case PCB_UI_SHOW_TEXT_ATOM_BOUNDARY:
		{
			DWORD dwState = 0;
			KBaseScriptTable::HostGetDebugModeState("ShowTextAtomBoundary", dwState);
			if (dwState == 1 && lParam == 0 || dwState == 0 && lParam == 1)
				KBaseScriptTable::HostDebug("ShowTextAtomBoundary");
		}
		break;
	case PCB_UI_ENABLE_RENDER_SAMPLER:
		break;
	case PCB_UI_ENABLE_GLOBAL_BACK_BUFFER:
		break;
	case PCB_UI_DRAG_WND_AND_ITEM_MODE:
		{
			DWORD dwState = 0;
			KBaseScriptTable::HostGetDebugModeState("DragWndAndItemMode", dwState);
			if (dwState == 1 && lParam == 0 || dwState == 0 && lParam == 1)
				KBaseScriptTable::HostDebug("DragWndAndItemMode");
			g_pUI->m_WndStation.ReleaseCapture();
		}
		break;
	case PCB_UI_EDIT_MODE:
		{
			DWORD dwState = 0;
			KBaseScriptTable::HostGetDebugModeState("EditMode", dwState);
			if (dwState == 1 && lParam == 0 || dwState == 0 && lParam == 1)
				KBaseScriptTable::HostDebug("EditMode");
			g_pUI->m_WndStation.ReleaseCapture();
		}
		break;
	case PCB_UI_RESIZE_WND_AND_ITEM_MODE:
		{
			DWORD dwState = 0;
			KBaseScriptTable::HostGetDebugModeState("ResizeWndAndItemMode", dwState);
			if (dwState == 1 && lParam == 0 || dwState == 0 && lParam == 1)
				KBaseScriptTable::HostDebug("ResizeWndAndItemMode");
			g_pUI->m_WndStation.ReleaseCapture();
		}
		break;
	case PCB_UI_ENABLE_SEL_WND_IN_CLIENT:
		{
			DWORD dwState = 0;
			KBaseScriptTable::HostGetDebugModeState("SelWndInClient", dwState);
			if (dwState == 1 && lParam == 0 || dwState == 0 && lParam == 1)
				KBaseScriptTable::HostDebug("SelWndInClient");
			g_pUI->m_WndStation.ReleaseCapture();
		}
		break;
	case PCB_UI_LOAD_MAIN_UI:
		if(g_pUI)
		{
			if (wParam)
				g_pUI->ReInit(START_GAME_LOGIN);
			else
				g_pUI->ReInit(RETURN_CLEAR_UI);
		}
		break;
	case PCB_UI_CREATE_FRAME:
		{
			IIniFile* pIni = (IIniFile*)wParam;
			if (pIni)
			{
				BOOL bRetCode = FALSE;
				float fUIScale = 1.0f;
				LPCSTR pcszScript = NULL;
				KWndFrame* pWnd = NULL;

				pcszScript = (LPCSTR)lParam;
				if (pcszScript && pcszScript[0])
					g_pUI->m_Script.LoadScriptFile(pcszScript, STR_ADD_ON_ENV);

				pWnd = CreateWndFrame(pIni);
				KGLOG_PROCESS_ERROR(pWnd);

				if (pcszScript && pcszScript[0])
				{
					BindWndScript(pWnd);
				}

				lRet = 1;
			}
		}
		break;
	case PCB_UI_DELETE_FRAME:
		if (wParam)
		{
			LPCSTR pszRootSectionName = LPCSTR(wParam);
			KWndFrame *pFrame = g_pUI->m_WndStation.SearchFrame(pszRootSectionName);               
			if (pFrame)
				pFrame->Destroy(WND_DO_NOT_DESTROY_MY_BROTHER);
			//ÈÃ´°¿Ú×Ô¼ºÉ¾³ý.
			g_pUI->m_WndStation.Breathe();
			lRet = 1;
		}
		break;
	case PCB_UI_GET_FOCUS_WND_NAME:
		if (wParam && ((int)lParam > 0))
		{
			KWndWindow *pFocusWnd = g_pUI->m_WndStation.GetFocusWindow();
			if (pFocusWnd)
				strncpy((LPSTR)wParam, pFocusWnd->GetName(), int(lParam));
			else
				((LPSTR)wParam)[0] = '\0';
		}
		lRet = false;
		break;
	case PCB_UI_RELOAD_ALL_IMAGE:
		g_pUI->m_p3DUI->UnloadAllImage();
		break;
	case PCB_UI_GET_ROOT_PATH:
		if (wParam)
		{
			LPSTR pszBuffer = (LPSTR)wParam;
            g_GetRootPath(pszBuffer);
			lRet = TRUE;
		}
		break;
	default:
		break;
	}

	return lRet;
Exit0:
	return FALSE;
}

LRESULT KUIEditorMgr::UIEditorOperation(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (m_pUiEditor)
		return m_pUiEditor->Operation(uMsg, wParam, lParam);
	return FALSE;
}

}

