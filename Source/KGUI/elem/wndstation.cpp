#include "stdafx.h"
#include "./wndwindow.h"
#include "./wndframe.h"
#include "./wndmessage.h"
#include "./wndscene.h"
#include "./wndmovie.h"
#include "./wndwebpage.h"
#include "./wndedit.h"
#include "./tipcenter.h"
#include "./buildagent/iniwndbuilder.h"
#include "./Components/itemeventmgr.h"
#include "./Components/decoder.h"
#include "./Components/pickdropboxmgr.h"
#include "../config/kdebugconfig.h"
#include "../script/kscriptmgr.h"
#include "../KGUI.h"

#ifndef DISABLE_UI_EDIT
#include "./Components/itemeditmodemgr.h"    
#endif

namespace
{

	LPCSTR g_szWndLayerNames[UI::WL_END] =
	{
		"Lowest",
		"Lowest1",
		"Lowest2",
		"Normal",
		"Normal1",
		"Normal2",
		"Topmost",
		"Topmost1",
		"Topmost2",
		"Texture",
	};

	static float const MIN_CANVAS_SCALE = 0.25f;
	static float const MAX_CANVAS_SCALE = 2.0f;
}

namespace UI
{

    KWndStation::KWndStation()
		: m_pActiveFrame(NULL)
        , m_pMouseOverWnd(NULL)
        , m_pCaptureMouseWnd(NULL)
		, m_pLastFocusWnd(NULL)
		, m_bEnabled(false)
		, m_pLastMouseLButtonDownItem(NULL)
		, m_pLastMouseRButtonDownItem(NULL)
		, m_pLastMouseMButtonDownItem(NULL)
		, m_dwKeepLastButtonDownReason(0)
		, m_dwCurrentDragStatus(0)
        , m_uMsg(0)
		, m_wParam(0)
		, m_lParam(0)
        , m_nCursorX(0)
        , m_nCursorY(0)
		, m_fStandardWidth(1280.0f)
		, m_fStandardHeight(960.0f)
		, m_fUIScale(1.0f)
		, m_fClientWidth(1280.0f)
		, m_fClientHeight(960.0f)
		, m_nHideUIMode(false)
		, m_nClientX(0)
		, m_nClientY(0)
		, m_nWindowX(0)
		, m_nWindowY(0)
        , m_hImc(NULL)
		, m_dwFPS(0)
		, m_dwLastUserInputTime(0)
		, m_bSwapMouseButton(false)
		, m_bUserAction(false)
		, m_bProcessNCSizing(TRUE)
    {
		m_strTreePath.reserve(512);

        memset(&m_DeviceSettings, 0, sizeof(m_DeviceSettings));
    }


    KWndStation::~KWndStation()
    {
		m_pActiveFrame = NULL;
		m_pMouseOverWnd = NULL;
		m_pCaptureMouseWnd = NULL;
		m_pLastFocusWnd = NULL;
    }

    int KWndStation::Init()
	{
		int nRetCode = false;
        int nResult = false;

		m_bSwapMouseButton = ::GetSystemMetrics(SM_SWAPBUTTON) ? true : false;

        m_hImc = ImmGetContext(g_pUI->m_hWnd);
        ImmReleaseContext(g_pUI->m_hWnd, m_hImc);

        nRetCode = KWndMovie::Init();
		KGLOG_PROCESS_ERROR(nRetCode);

		nRetCode = KWndWebPage::Init();
		KGLOG_PROCESS_ERROR(nRetCode);

		nRetCode = KPickDropBoxMgr::Init();
		KGLOG_PROCESS_ERROR(nRetCode);

		nRetCode = KTipCenter::Init();
		KGLOG_PROCESS_ERROR(nRetCode);

#ifndef DISABLE_UI_EDIT
        nRetCode = KItemEditModeMgr::Init();
        KGLOG_PROCESS_ERROR(nRetCode);
#endif

		nRetCode = KUiComponentsDecoder::Init();
		KGLOG_PROCESS_ERROR(nRetCode);

		for (int nIndex = WL_BEGIN; nIndex < WL_END; ++nIndex) 
		{
			m_LayerRoot[nIndex].SetName(g_szWndLayerNames[nIndex]);
		}

		nResult = true;
Exit0:
        if (!nResult)
        {
		    Exit();
        }
		return nResult;
    }


    void KWndStation::Exit()
	{
        //释放所有的窗口
        for (int nIndex = WL_BEGIN; nIndex < WL_END; ++nIndex) 
        {
            if (m_LayerRoot[nIndex].GetFirstChild())
                m_LayerRoot[nIndex].GetFirstChild()->Destroy(WND_DESTROY_MY_BROTHER);
        }
		OnDestory();

		KUiComponentsDecoder::Exit();

#ifndef DISABLE_UI_EDIT
        KItemEditModeMgr::Exit();
#endif

		KTipCenter::Exit();
		KPickDropBoxMgr::Exit();
		
		KWndWebPage::Exit();
		KWndMovie::Exit();

		::SwapMouseButton(m_bSwapMouseButton);
	}

	void KWndStation::OnDestory()
	{
		if (m_aDestoryWindow.size())
		{
			for (std::vector<KWndWindow *>::iterator it = m_aDestoryWindow.begin(); it != m_aDestoryWindow.end(); ++it)
			{
				SAFE_DELETE(*it);
			}
			m_aDestoryWindow.clear();
		}
	}

	void KWndStation::DestoryWindow(KWndWindow * pWnd)
	{
		ASSERT(pWnd);
		m_aDestoryWindow.push_back(pWnd);
	}

	KWndWindow * KWndStation::GetMouseOverWindow()
	{
		int nX = 0;
		int nY = 0;
		g_pUI->m_Pointer.GetCursorPos(nX, nY);
		for (int nLayer = WL_VISIBLE - 1; nLayer >= WL_BEGIN; --nLayer) 
		{
			for (KWndWindow* pWnd = GetLastFrame(nLayer); pWnd; pWnd = pWnd->GetPrev())
			{
				if (pWnd->PtInWindow(nX, nY))
				{
					KWndWindow * pWindow = pWnd->TopChildFromPoint(nX, nY);
					if (pWindow)
						return pWindow;
					return pWnd;
				}
			}
		}

		return NULL;
	}

	bool KWndStation::ReInit()
	{
        for (int nIndex = WL_BEGIN; nIndex < WL_END; ++nIndex) 
        {
			int nRef = m_LayerRoot[nIndex].GetLuaRef();
			if (nRef != LUA_NOREF)
			{
				g_pUI->m_Script.UnRefUIObject(nRef);
				m_LayerRoot[nIndex].SetLuaRef(LUA_NOREF);
			}
            if (m_LayerRoot[nIndex].GetFirstChild())
                m_LayerRoot[nIndex].GetFirstChild()->Destroy(WND_DESTROY_MY_BROTHER);
        }
		OnDestory();

		SetUIScale(GetMaxUIScale());

		return true;
	}


	void KWndStation::Activate(bool bActivated)
	{
		if (bActivated)
		{
            ImmAssociateContext(g_pUI->m_hWnd, NULL);

			if (m_pLastFocusWnd)
			{
                SetFocusWindow(m_pLastFocusWnd);
				m_pLastFocusWnd = NULL;
			}
		}
		else
		{
			if (m_pCaptureMouseWnd)
            {
				ReleaseCapture();
            }

			m_pLastFocusWnd = GetFocusWindow();
			SetFocusWindow(NULL);

			if (m_pMouseOverWnd)
            {
				UpdateMouseOver(NULL);
            }

            if (IsClipCursor())
            {
                ClipCursor(NULL);
            }
		}
	}


	bool KWndStation::Breathe()
	{
		for (int nIndex = WL_BEGIN; nIndex != WL_END; ++nIndex) 
			m_LayerRoot[nIndex].Breathe();
		OnDestory();

		if (m_pMouseOverWnd)
			m_pMouseOverWnd->WndProc(WND_M_REFRESH_TIP, 0, (LPARAM)m_pMouseOverWnd);

		return true;
	}


	int KWndStation::GetLayerIndexByName(LPCSTR pszLayerName)
	{
        ASSERT(pszLayerName);

		for (int nIndex = WL_BEGIN; nIndex < WL_END; ++nIndex) 
		{
			if (stricmp(pszLayerName, g_szWndLayerNames[nIndex]) == 0) 
			{
				return nIndex;
			}
		}

		return WL_UNKNOWN;
	}

	bool KWndStation::AddFrame(KWndFrame *pWnd, int nLayer)
	{
		KGLOG_PROCESS_ERROR(nLayer >= WL_BEGIN);
		KGLOG_PROCESS_ERROR(nLayer < WL_END);
		KGLOG_PROCESS_ERROR(pWnd);

		m_LayerRoot[nLayer].AddChild(pWnd);

		return true;
Exit0:
		return false;
	}

	void KWndStation::SetBlurWindow(KWndWindow* pWnd)
	{
		ASSERT(pWnd);

		if (m_pMouseOverWnd == pWnd)
			UpdateMouseOver(NULL);

		if (m_pCaptureMouseWnd == pWnd)
			ReleaseCapture();

		if (m_pActiveFrame == pWnd)
			SetActiveFrame(NULL, false);

		if (GetFocusWindow() == pWnd)
			SetFocusWindow(NULL);

		if (m_pLastFocusWnd == pWnd)
			m_pLastFocusWnd = NULL;

		RemoveMovingWindow(pWnd);
	}

	void KWndStation::SetBlurItem(KItemNull* pItem)
	{
		ASSERT(pItem);

		if (m_pLastMouseLButtonDownItem == pItem)
			m_pLastMouseLButtonDownItem = NULL; 
		if (m_pLastMouseRButtonDownItem == pItem)
			m_pLastMouseRButtonDownItem = NULL;
		if (m_pLastMouseMButtonDownItem == pItem)
			m_pLastMouseMButtonDownItem = NULL;
	}


	void KWndStation::SetFocusWindow(KWndWindow* pWnd)
	{
		if (pWnd && pWnd->IsStyle(WND_S_TOBEDESTROY))
			return;

		KWndFrame* pRootFrame = GetRootFrame(pWnd);
		if (pRootFrame && pRootFrame->IsVisible())
		{
			SetActiveFrame(pRootFrame, true);
			pRootFrame->SetFocusWnd(pWnd);              
		}
		else
		{
			SetActiveFrame(NULL, false);
		}
	}

	KWndWindow*	KWndStation::GetFocusWindow()
	{
		if (!m_pActiveFrame) 
			return NULL;
		return m_pActiveFrame->GetFocusWnd();
	}

	void KWndStation::SetActiveFrame(KWndFrame *pFrame, bool bBringToTop)
	{
		if (pFrame && pFrame->IsStyle(WND_S_TOBEDESTROY))
			return;

        if (m_pActiveFrame == pFrame)
            return;

        if (m_pActiveFrame)
            m_pActiveFrame->SetFocusWnd(NULL);

		m_pActiveFrame = pFrame;

		if (pFrame && bBringToTop)
		{
			pFrame->BringToTop();
		}
	}

	KWndFrame* KWndStation::GetActiveFrame()
	{
		return m_pActiveFrame;
	}

	KWndFrame* KWndStation::GetLastFrame(int nLayer)
	{
		if (nLayer < WL_BEGIN || nLayer >= WL_END)
			return NULL;

		KWndWindow* pWnd = NULL;
		for (pWnd = m_LayerRoot[nLayer].GetFirstChild(); pWnd; pWnd = pWnd->GetNext())
		{
			 if (pWnd->GetNext() == NULL)
			 {
				 break;
			 }
		}
		return (KWndFrame*)CastWndWindow(pWnd, "WndFrame");
	}

	KWndFrame* KWndStation::GetTopFrame(int nLayer)
	{
		KWndWindow* pWnd = NULL;
		
		if (nLayer < WL_BEGIN || nLayer >= WL_END)
			return NULL;

		if (m_nHideUIMode)
		{
			for (pWnd = GetLastFrame(nLayer); pWnd; pWnd = pWnd->GetPrev())
			{
				KWndFrame* pFrame = (KWndFrame*)CastWndWindow(pWnd, "WndFrame");
				if (pFrame && pFrame->IsVisible() && pFrame->IsVisibleWhenUIHide())
				{
					return pFrame;
				}
			}
			return NULL;
		}

		for (pWnd = GetLastFrame(nLayer); pWnd; pWnd = pWnd->GetPrev())
		{
			if (pWnd->IsVisible())
			{
				return (KWndFrame*)CastWndWindow(pWnd, "WndFrame");
			}
		}
		return NULL;
	}

	KWndFrame* KWndStation::GetTopFrame(int nX, int nY)
	{
		for (int nLayer = WL_VISIBLE - 1; nLayer >= WL_BEGIN; --nLayer) 
		{
			for (KWndWindow* pWnd = GetLastFrame(nLayer); pWnd; pWnd = pWnd->GetPrev())
			{
				if (pWnd->PtInWindow(nX, nY))
				{
					return (KWndFrame*)CastWndWindow(pWnd, "WndFrame");
				}
			}
		}

		return NULL;
	}

	KWndFrame* KWndStation::GetRootFrame(KWndWindow* pWnd)
	{
		for (; pWnd; pWnd = pWnd->GetParent())
		{
			if (IsRootWindow(pWnd))
			{
				return (KWndFrame*)CastWndWindow(pWnd, "WndFrame");
			}
		}

		return NULL;
	}

	bool KWndStation::IsRootWindow(KWndWindow const *pWnd) const
	{
		if (pWnd == NULL) 
			return false;

		if (pWnd->GetParent() == NULL) 
			return true;

		for (int nIndex = WL_BEGIN; nIndex != WL_END; ++nIndex) 
		{
			if (pWnd->GetParent() == &m_LayerRoot[nIndex]) 
			{
				return true;
			}
		}

		return false;
	}

	void KWndStation::SetCapture(KWndWindow* pWnd, bool bDontSendToChild)
	{
		if (pWnd && pWnd->IsStyle(WND_S_TOBEDESTROY))
			return;

		if (m_pCaptureMouseWnd == pWnd)
			return;

		KWndWindow *pTemp = m_pCaptureMouseWnd;

		m_pCaptureMouseWnd = pWnd;
        SetFocusWindow(pWnd);

		if (pTemp)
		{
			pTemp->WndProc(WND_M_CAPTURECHANGED, (WPARAM)pWnd, 0);
			KWndWindow *pFather = pTemp->GetParent();
			if (pFather)
				pFather->WndProc(WND_M_CHILD_CAPTURECHANGED, (WPARAM)pTemp, 0);
		}

		if (m_pCaptureMouseWnd)
			::SetCapture(g_pUI->m_hWnd);
		else
			::ReleaseCapture();
	}

	KWndWindow* KWndStation::GetCapture()
	{
	    return m_pCaptureMouseWnd;
	}

	void KWndStation::ReleaseCapture()
	{
		if (m_pCaptureMouseWnd == NULL)
			return;

		KWndWindow *pTemp = m_pCaptureMouseWnd;
		::ReleaseCapture();

		m_pCaptureMouseWnd = NULL;

		pTemp->WndProc(WND_M_CAPTURECHANGED, (WPARAM)NULL, 0);
		KWndWindow *pFather = pTemp->GetParent();
		if (pFather)
			pFather->WndProc(WND_M_CHILD_CAPTURECHANGED, (WPARAM)pTemp, 0);
	}

    HIMC KWndStation::GetHIMC() const
    {
        return m_hImc;
    }

	void KWndStation::UpdateMouseOver(KWndWindow* pMouseOverWnd)
	{
		if (m_pMouseOverWnd != pMouseOverWnd)
		{
			if (m_pMouseOverWnd) 
				m_pMouseOverWnd->WndProc(WND_M_MOUSE_LEAVE, 0, (LPARAM)pMouseOverWnd);

			if (pMouseOverWnd)
				pMouseOverWnd->WndProc(WND_M_MOUSE_ENTER, 0, (LPARAM)m_pMouseOverWnd);

			m_pMouseOverWnd = pMouseOverWnd;
		}
		if (m_pMouseOverWnd && m_pMouseOverWnd->IsStyle(WND_S_TOBEDESTROY))
			m_pMouseOverWnd = NULL;
	}

	LRESULT	KWndStation::PreProcessInput(UINT& uMsg, WPARAM &wParam, LPARAM &lParam)
	{
		int nRet;

		if (::InSendMessage())
			goto Exit1;

		m_uMsg = uMsg;
		m_wParam = wParam;
		m_lParam = lParam;

		switch (uMsg)
		{
		case WM_SETCURSOR:
			g_pUI->m_Pointer.RestoreCursor();
			goto Exit1;
		case WM_MOUSEWHEEL:
			m_bUserAction = true;
			g_pUI->m_Script.ClearShieldTable();
			m_dwLastUserInputTime = KG_GetTickCount();
		case WM_MOUSEHOVER:
			g_pUI->m_Pointer.GetCursorPos(m_nCursorX, m_nCursorY);
			m_lParam = lParam = MAKELPARAM(m_nCursorX, m_nCursorY);
			goto Exit0;
		case WM_MOUSEMOVE:
			m_nCursorX = LOWORD(lParam);
            m_nCursorY = HIWORD(lParam);
			KTipCenter::GetSelf().OnMouseMove();

			if (m_nCursorX >= 0 && m_nCursorX <= m_fClientWidth && m_nCursorY >= 0 && m_nCursorY <= m_fClientHeight)
			{
				for (std::vector<MovingWindow>::iterator it = m_MovingWindow.begin(); it != m_MovingWindow.end(); ++it)
				{
					MovingWindow &rWinodw = *it;
					rWinodw.pWindow->SetAbsPos(rWinodw.fX + m_nCursorX - rWinodw.nMouseX, rWinodw.fY + m_nCursorY - rWinodw.nMouseY);
				}
			}
            goto Exit0;
		case WM_RBUTTONDOWN:
			m_bUserAction = true;
			g_pUI->m_Script.ClearShieldTable();
			m_dwLastUserInputTime = KG_GetTickCount();
			g_pUI->m_Script.FireGlobalEvent("OnGlobalRButtonDown", &nRet);
			if (nRet)
				goto Exit1;
			goto Exit0;
		case WM_RBUTTONUP:
			m_bUserAction = true;
			g_pUI->m_Script.ClearShieldTable();
			m_dwLastUserInputTime = KG_GetTickCount();
			g_pUI->m_Script.FireGlobalEvent("OnGlobalRButtonUp", &nRet);
			if (nRet)
				goto Exit1;
			goto Exit0;
		case WM_RBUTTONDBLCLK:
			m_bUserAction = true;
			g_pUI->m_Script.ClearShieldTable();
			m_dwLastUserInputTime = KG_GetTickCount();
			g_pUI->m_Script.FireGlobalEvent("OnGlobalRButtonDBClick", &nRet);
			if (nRet)
				goto Exit1;
			goto Exit0;
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_LBUTTONDBLCLK:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_MBUTTONDBLCLK:
		case WM_XBUTTONDOWN:
		case WM_XBUTTONUP:
		case WM_XBUTTONDBLCLK:
			m_nCursorX = LOWORD(lParam);
            m_nCursorY = HIWORD(lParam);
		case WM_KEYDOWN:
		case WM_KEYUP:
			m_bUserAction = true;
			g_pUI->m_Script.ClearShieldTable();
			m_dwLastUserInputTime = KG_GetTickCount();
		default:
			goto Exit0;
		}
Exit1:
		return TRUE;
Exit0:
		return FALSE;
	}

	LRESULT	KWndStation::EndProcessInput(UINT& uMsg, WPARAM &wParam, LPARAM &lParam)
	{
		switch (uMsg)
		{
		case WM_MOUSEWHEEL:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_RBUTTONDBLCLK:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_LBUTTONDBLCLK:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_MBUTTONDBLCLK:
		case WM_XBUTTONDOWN:
		case WM_XBUTTONUP:
		case WM_XBUTTONDBLCLK:
		case WM_KEYDOWN:
		case WM_KEYUP:
			m_bUserAction = false;
			g_pUI->m_Script.ClearShieldTable();
			break;
		default:
			break;
		}
		return TRUE;
	}

	LRESULT	KWndStation::ProcessInputModeless(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if ((uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST) || uMsg == WM_MOUSEHOVER)
		{
			//滚轮消息以外的鼠标消息
			int x = GET_X_LPARAM(lParam);
			int y = GET_Y_LPARAM(lParam);

			if (m_pCaptureMouseWnd)
			{
				KWndWindow *pWndMouseOver = m_pCaptureMouseWnd->TopChildFromPoint(x, y);
				UpdateMouseOver(pWndMouseOver);
				return m_pCaptureMouseWnd->WndProc(uMsg, wParam, lParam);
			}
			else
			{
				KWndFrame *pTopFrame = GetTopFrame(x, y);
				KWndWindow *pWndMouseOver = NULL;
				switch (uMsg)
				{
				case WM_LBUTTONDOWN:
				case WM_RBUTTONDOWN:
				case WM_MBUTTONDOWN:
				case WM_LBUTTONDBLCLK:
				case WM_RBUTTONDBLCLK:
				case WM_MBUTTONDBLCLK:
					SetActiveFrame(pTopFrame, true);
					break;
				}
				if (pTopFrame)
					pWndMouseOver = pTopFrame->TopChildFromPoint(x, y);
				UpdateMouseOver(pWndMouseOver);

				if (pTopFrame)
					return pTopFrame->WndProc(uMsg, wParam, lParam);
				return FALSE;
			}
		} 
		else
		{
			if (m_pActiveFrame && m_pActiveFrame->IsVisible())
				return m_pActiveFrame->WndProc(uMsg, wParam, lParam);
			return FALSE;
		}
	}


	LRESULT	KWndStation::HandleInput(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		LRESULT lRet = FALSE;

		if (!IsEnabled())
			return lRet;

		if (PreProcessInput(uMsg, wParam, lParam)) 
			goto Exit1;
		if (ProcessInputModeless(uMsg, wParam, lParam))
			goto Exit1;

		goto Exit0;
		
Exit1:
		lRet = TRUE;
Exit0:
		lRet = ProcessGlobalHotkey(uMsg, wParam, lParam, lRet);
		ProcessItemInvalidMouseKeyUp(uMsg, wParam, lParam);
		EndProcessInput(uMsg, wParam, lParam);
		return lRet;
	}

	LRESULT	KWndStation::ProcessGlobalHotkey(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT lProcessed)
	{
		switch (uMsg)
		{
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_LBUTTONDBLCLK:
		case WM_RBUTTONDBLCLK:
		case WM_MBUTTONDBLCLK:
			if (lProcessed)
				return TRUE;
            else
			{
				KWndWindow *pFocus = GetFocusWindow();
				if (pFocus && !((KWndScene*)CastWndWindow(pFocus, "WndScene")))
					return TRUE;
			}
			break;
		case WM_KEYUP:
		case WM_SYSKEYUP:
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MBUTTONUP:
			return g_pUI->m_HotkeyMgr.HandleHotkey(uMsg, wParam, lParam);
			break;
		}
		if (!lProcessed)
			return g_pUI->m_HotkeyMgr.HandleHotkey(uMsg, wParam, lParam);
		return TRUE;
	}

	LRESULT	KWndStation::ProcessItemInvalidMouseKeyUp(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		DWORD dwEvent = 0;
		KItemNull *pItem = NULL;
		
		static LPARAM sdwLastLBtnDownParam = 0;
		static LPARAM sdwLastRBtnDownParam = 0;
		static LPARAM sdwLastMBtnDownParam = 0;
		switch (uMsg)
		{
		case WM_LBUTTONDOWN:
			sdwLastLBtnDownParam = lParam;
			return false;
		case WM_RBUTTONDOWN:
			sdwLastRBtnDownParam = lParam;
			return false;
		case WM_MBUTTONDOWN:
			sdwLastMBtnDownParam = lParam;
			return false;
		case WM_LBUTTONUP:
			if (!m_pLastMouseLButtonDownItem)
				return false;
			pItem = m_pLastMouseLButtonDownItem;
			m_dwCurrentDragStatus &= ~ITEM_EVENT_LBUTTONDRAG;
			m_pLastMouseLButtonDownItem = NULL;
			if (!(m_dwKeepLastButtonDownReason & ITEM_EVENT_LBUTTONUP))
				return false;
			m_dwKeepLastButtonDownReason &= ~ITEM_EVENT_LBUTTONUP;
			m_dwKeepLastButtonDownReason &= ~ITEM_EVENT_LBUTTONDRAG;
			dwEvent = ITEM_EVENT_LBUTTONUP;
			g_pUI->m_Script.FireItemEvent(pItem, "OnItemLButtonUp");
			break;
		case WM_RBUTTONUP:
			if (!m_pLastMouseRButtonDownItem)
				return false;
			pItem = m_pLastMouseRButtonDownItem;
			m_dwCurrentDragStatus &= ~ITEM_EVENT_RBUTTONDRAG;
			m_pLastMouseRButtonDownItem = NULL;
			if (!(m_dwKeepLastButtonDownReason & ITEM_EVENT_RBUTTONUP))
				return false;
			m_dwKeepLastButtonDownReason &= ~ITEM_EVENT_RBUTTONUP;
			m_dwKeepLastButtonDownReason &= ~ITEM_EVENT_RBUTTONDRAG;
			dwEvent = ITEM_EVENT_RBUTTONUP;
			g_pUI->m_Script.FireItemEvent(pItem, "OnItemRButtonUp");
			break;
		case WM_MBUTTONUP:
			if (!m_pLastMouseMButtonDownItem)
				return false;
			pItem = m_pLastMouseMButtonDownItem;
			m_dwCurrentDragStatus &= ~ITEM_EVENT_MBUTTONDRAG;
			m_pLastMouseMButtonDownItem = NULL;
			if (!(m_dwKeepLastButtonDownReason & ITEM_EVENT_MBUTTONUP))
				return false;
			m_dwKeepLastButtonDownReason &= ~ITEM_EVENT_MBUTTONUP;
			m_dwKeepLastButtonDownReason &= ~ITEM_EVENT_MBUTTONDRAG;
			dwEvent = ITEM_EVENT_MBUTTONUP;
			g_pUI->m_Script.FireItemEvent(pItem, "OnItemMButtonUp");
			break;
		case WM_MOUSEMOVE:
            //ATLTRACE("WM_MOUSEMOVE:%d-%d (%d)\n", GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), wParam & MK_LBUTTON ? 1: 0);
			if (wParam & MK_LBUTTON && 
				m_dwKeepLastButtonDownReason & ITEM_EVENT_LBUTTONDRAG && 
				m_pLastMouseLButtonDownItem
			)
			{
				int nDeltaX = GET_X_LPARAM(lParam) - GET_X_LPARAM(sdwLastLBtnDownParam);
				int nDeltaY = GET_Y_LPARAM(lParam) - GET_Y_LPARAM(sdwLastLBtnDownParam);
				if (nDeltaX * nDeltaX + nDeltaY * nDeltaY < 10)
					return false;

				m_dwCurrentDragStatus |= ITEM_EVENT_LBUTTONDRAG;
				m_dwKeepLastButtonDownReason &= ~ITEM_EVENT_LBUTTONDRAG;
				g_pUI->m_Script.FireItemEvent(m_pLastMouseLButtonDownItem, "OnItemLButtonDrag");
			}
			if (wParam & MK_RBUTTON && 
				m_dwKeepLastButtonDownReason & ITEM_EVENT_RBUTTONDRAG && 
				m_pLastMouseRButtonDownItem
				)
			{
				int nDeltaX = GET_X_LPARAM(lParam) - GET_X_LPARAM(sdwLastRBtnDownParam);
				int nDeltaY = GET_Y_LPARAM(lParam) - GET_Y_LPARAM(sdwLastRBtnDownParam);
				if (nDeltaX * nDeltaX + nDeltaY * nDeltaY < 10)
					return false;

				m_dwCurrentDragStatus |= ITEM_EVENT_RBUTTONDRAG;
				m_dwKeepLastButtonDownReason &= ~ITEM_EVENT_RBUTTONDRAG;
				g_pUI->m_Script.FireItemEvent(m_pLastMouseRButtonDownItem, "OnItemRButtonDrag");
			}
			if (wParam & MK_MBUTTON && 
				m_dwKeepLastButtonDownReason & ITEM_EVENT_MBUTTONDRAG && 
				m_pLastMouseMButtonDownItem
				)
			{
				int nDeltaX = GET_X_LPARAM(lParam) - GET_X_LPARAM(sdwLastMBtnDownParam);
				int nDeltaY = GET_Y_LPARAM(lParam) - GET_Y_LPARAM(sdwLastMBtnDownParam);
				if (nDeltaX * nDeltaX + nDeltaY * nDeltaY < 10)
					return false;

				m_dwCurrentDragStatus |= ITEM_EVENT_MBUTTONDRAG;
				m_dwKeepLastButtonDownReason &= ~ITEM_EVENT_MBUTTONDRAG;
				g_pUI->m_Script.FireItemEvent(m_pLastMouseRButtonDownItem, "OnItemMButtonDrag");
			}
			return false;
		default:
			return false;
		}

		return false;
	}


	void KWndStation::Paint()
	{
		UpdateFPS();

		for (int nIndex = WL_BEGIN; nIndex < WL_VISIBLE; ++nIndex)
		{
			m_LayerRoot[nIndex].Paint();
		}
	}


	void KWndStation::UpdateFPS()
	{
		static DWORD s_dwFPS = 0;
		static DWORD s_dwLastTime = 0;

		DWORD dwCurrentTime = KG_GetTickCount();

		++s_dwFPS;

		if (dwCurrentTime - s_dwLastTime > 1000)
		{
			s_dwLastTime = dwCurrentTime;
			m_dwFPS = s_dwFPS;
			s_dwFPS = 0;
		}
	}


	DWORD KWndStation::GetFPS() const
	{
		return m_dwFPS;
	}


	KWndFrame* KWndStation::SearchFrame(LPCSTR szFrameName)
	{
		ASSERT(szFrameName);

		for (int nIndex = WL_BEGIN; nIndex < WL_END; ++nIndex) 
		{
			for (KWndWindow* pWnd = m_LayerRoot[nIndex].GetFirstChild(); pWnd; pWnd = pWnd->GetNext())
			{
				if (stricmp(pWnd->GetName(), szFrameName) == 0 && !pWnd->IsStyle(WND_S_TOBEDESTROY)) 
				{
					return (KWndFrame*)CastWndWindow(pWnd, "WndFrame");
				}
			}
		}

		return NULL;
	}


	KWndWindow* KWndStation::SearchWindow(KWndWindow *pRoot, LPCSTR szSectionName)
	{
		if (pRoot == NULL)
			return NULL;
		
		if (!szSectionName || szSectionName[0] == '\0')
			return NULL;

		if (stricmp(szSectionName, pRoot->GetName()) == 0 && !pRoot->IsStyle(WND_S_TOBEDESTROY))
			return pRoot;

		for (KWndWindow* pChild = pRoot->GetFirstChild(); pChild; pChild = pChild->GetNext())
		{
			KWndWindow* pFind = SearchWindow(pChild, szSectionName);
			if (pFind)
			{
				return pFind;
			}
		}

		return NULL;
	}


	KWndWindow* KWndStation::SearchWindow(LPCSTR szTreePath, LPCSTR szDelimit)
	{
		if (!szTreePath || szTreePath[0] == '\0')
			return NULL;

		if (!szDelimit)
			szDelimit = "/\\";

		static char szSrcToken[WND_TREE_PATH_LEN] = { 0 };

		strncpy(szSrcToken, szTreePath, _countof(szSrcToken) - 1);

		LPCSTR szToken = strtok(szSrcToken, szDelimit);

		KWndWindow* pWnd = NULL;
		while (szToken != NULL)
		{
			if (pWnd == NULL)
			{
				int const nLayer = GetLayerIndexByName(szToken);
				if (nLayer != WL_UNKNOWN) 
				{
					pWnd = &m_LayerRoot[nLayer];
					if (pWnd->IsStyle(WND_S_TOBEDESTROY))
					{
						pWnd = NULL;
						break;
					}
				}
			}
			else
			{
				pWnd = SearchChildWindow(pWnd, szToken);
			}

			szToken = strtok(NULL, szDelimit);
		}

		return pWnd;
	}

	KWndWindow* KWndStation::SearchChildWindowByPath(KWndWindow *pFather, LPCSTR szTreePath, LPCSTR szDelimit)
	{
		if (szTreePath == NULL || szTreePath[0] == '\0')
			return NULL;

		if (szDelimit == NULL)
			szDelimit = "/\\";

		static char s_szSrcToken[WND_TREE_PATH_LEN];

		if (pFather == NULL)
			return SearchWindow(szTreePath, szDelimit);

		strncpy(s_szSrcToken, szTreePath, _countof(s_szSrcToken) - 1);

		LPSTR szToken = strtok(s_szSrcToken, szDelimit);

		KWndWindow* pWnd = pFather;
		while (pWnd && szToken)
		{
			pWnd = SearchChildWindow(pWnd, szToken);
			szToken = strtok(NULL, szDelimit);
		}

		return pWnd;
	}


	KWndWindow* KWndStation::SearchChildWindow(KWndWindow *pParent, LPCSTR szSectionName)
	{
		if (pParent == NULL)
			return NULL;

		if (szSectionName == NULL || szSectionName[0] == '\0')
			return NULL;

		for (KWndWindow* pWnd = pParent->GetFirstChild(); pWnd; pWnd = pWnd->GetNext())
		{
			if (stricmp(szSectionName, pWnd->GetName()) == 0 && !pWnd->IsStyle(WND_S_TOBEDESTROY))
			{
				return pWnd;
			}
		}

		return NULL;
	}


	LPCSTR	KWndStation::GetTreePath(KWndWindow const* pWnd)
	{
		typedef std::vector<KWndWindow const*>	KWndWindowPtrList;

		if (pWnd == NULL)
			return "";

		KWndWindowPtrList aWndPtr;

		for (; pWnd; pWnd = pWnd->GetParent())
			aWndPtr.push_back(pWnd);

		m_strTreePath.clear();
		for (KWndWindowPtrList::reverse_iterator it = aWndPtr.rbegin(); it != aWndPtr.rend(); ++it)
		{	
			m_strTreePath += (*it)->GetName();
			m_strTreePath += "/";
		}

		return m_strTreePath.c_str(); 
	}


	bool KWndStation::IsEnabled() const
	{
		return m_bEnabled;
	}


	void KWndStation::Enable(bool bEnable)
	{
		m_bEnabled = bEnable;
	}


	BOOL KWndStation::ToggleFrame(LPCSTR szFileName, LPCSTR szFrameName)
	{
		BOOL bRetCode = FALSE;

		KG_PROCESS_ERROR(szFileName);
		KG_PROCESS_ERROR(szFrameName);

		KWndWindow* pWnd = SearchFrame(szFrameName);
		if (pWnd)
			bRetCode = CloseFrame(szFrameName);
		else
			bRetCode = OpenFrame(szFileName, szFrameName) != NULL;

Exit0:
		return bRetCode;
	}


	KWndFrame* KWndStation::OpenFrame(LPCSTR szFileName, LPCSTR szFrameName)
	{
		KWndFrame* pWnd = NULL;

		ASSERT(szFileName);
		ASSERT(szFrameName);

		pWnd = SearchFrame(szFrameName);
		if (pWnd)
			return pWnd;

		// 通过配置文件打开窗口
#if 0
		DWORD dwTickCount = KG_GetTickCount();
#endif

		pWnd = CreateWndFrame(szFileName, szFrameName);
		if (pWnd == NULL)
		{
			KGLogPrintf(KGLOG_ERR, "KGUI KWndStation::OpenFrame(%s, %s) failed\n", szFileName, szFrameName);
			return NULL;
		}
#if 0
		KGLogPrintf(KGLOG_INFO, "KGUI KWndStation::OpenFrame(%s, %s) %d\n", szFileName, szFrameName, KG_GetTickCount() - dwTickCount);
#endif

		pWnd->Show();
		pWnd->BringToTop();
		return pWnd;
	}


	BOOL KWndStation::CloseFrame(LPCSTR szFrameName)
	{
		if (szFrameName == NULL)
			return FALSE;

		KWndWindow* pWnd = SearchFrame(szFrameName);
		if (pWnd == NULL)
			return FALSE;

		pWnd->Destroy(WND_DO_NOT_DESTROY_MY_BROTHER);
		return TRUE;
	}


	bool KWndStation::AdjustFrameListPosition(KSmartIniFile& Ini, bool bRemainVisibleState)
	{
		char szSection[WND_SECTION_NAME_LEN];
		char szValue[WND_SECTION_NAME_LEN];
		int nValue = 0;
		bool bRetCode = true;

		ASSERT(Ini);

		szSection[0] = '\0';
		while (Ini->GetNextSection(szSection, szSection) && szSection[0] != '\0')
		{
			int nNeedAdjust = 0;
			int nSide = 0;
			int nRelSide = 0;
			int nX = 0;
			int nY = 0;
			int nRelX = 0;
			int nRelY = 0;
			float fX = 0.0f;
			float fY = 0.0f;
			float fRelX = 0.0f;
			float fRelY = 0.0f;
			KWndFrame* pWnd = NULL;
			KWndFrame* pRelWnd = NULL;

			pWnd = (KWndFrame*)CastWndWindow(SearchFrame(szSection), "WndFrame");
			if (pWnd == NULL)
			{
				bRetCode = false;
				continue;
			}

			if (Ini->GetString(szSection, "Side", "", szValue, _countof(szValue)))
			{
				nSide = KWndWindow::GetSide(szValue);
				if (nSide >= WND_SIDE_BEGIN && nSide < WND_SIDE_END)
				{
					nNeedAdjust |= TRUE;
				}
			}

			if (Ini->GetString(szSection, "RelSide", "", szValue, _countof(szValue)))
			{
				nRelSide = KWndWindow::GetSide(szValue);
				if (nRelSide >= WND_SIDE_BEGIN && nRelSide < WND_SIDE_END)
				{
					nNeedAdjust |= TRUE;
				}
			}

			nNeedAdjust |= Ini->GetInteger(szSection, "X", 0, &nX);
			nNeedAdjust |= Ini->GetInteger(szSection, "Y", 0, &nY);
			nNeedAdjust |= Ini->GetInteger(szSection, "RelX", 0, &nRelX);
			nNeedAdjust |= Ini->GetInteger(szSection, "RelY", 0, &nRelY);
			nNeedAdjust |= Ini->GetString(szSection, "RelFrameName", "", szValue, _countof(szValue));

			pRelWnd = (KWndFrame*)CastWndWindow(SearchFrame(szValue), "WndFrame");

			fX = (float)nX * m_fUIScale;
			fY = (float)nY * m_fUIScale;
			fRelX = (float)nRelX * m_fUIScale;
			fRelY = (float)nRelY * m_fUIScale;

			if (nNeedAdjust != 0 && !pWnd->SetPoint(nSide, fX, fY, pRelWnd, nRelSide, fRelX, fRelY))
				bRetCode = false;

			if (!bRemainVisibleState)
			{
				if (!(Ini->GetInteger(szSection, "Hide", 0, &nValue) && nValue != 0))
				{
					pWnd->Show();
				}
			}
		}

		return bRetCode;
	}


	bool KWndStation::AdjustFrameListPosition(LPCSTR szFilePath)
	{
		bool bRetCode = true;
		KSmartIniFile Ini;

		ASSERT(szFilePath);

		Ini = g_OpenIniFile(szFilePath);
		KGLOG_PROCESS_ERROR(Ini);

		bRetCode = AdjustFrameListPosition(Ini, true);
		KG_PROCESS_ERROR(bRetCode);

        return bRetCode;
Exit0:
		return false;
	}


	bool KWndStation::LoadFrameList(LPCSTR szFilePath)
	{
#if 0
		static DWORD s_dwTickCount = 0;
		s_dwTickCount = KG_GetTickCount();
		ATLTRACE("KWndStation::LoadFrameList(%s)\t%u\n", szFilePath, s_dwTickCount);
#endif

		bool bRetCode = true;
		char szSection[WND_SECTION_NAME_LEN];
		char szValue[WND_SECTION_NAME_LEN];
		int nValue = 0;
		KSmartIniFile Ini;

		ASSERT(szFilePath);

		Ini = g_OpenIniFile(szFilePath);
		KGLOG_PROCESS_ERROR(Ini);

		// Open all frames
		szSection[0] = '\0';
		while (Ini->GetNextSection(szSection, szSection) && szSection[0] != '\0')
		{
			LPCSTR szFileName = szSection;
			KWndFrame* pWnd = NULL;

			Ini->GetString(szSection, "Ini", "", szValue, _countof(szValue));
			if (szValue[0] != '\0')
				szFileName = szValue;
			pWnd = OpenFrame(szFileName, szSection);
			if (pWnd == NULL)
				bRetCode = false;
			else
				pWnd->Hide();
		}

		// Set position
		bRetCode = AdjustFrameListPosition(Ini, false);
		KGLOG_PROCESS_ERROR(bRetCode);

#if 0
		ATLTRACE("KWndStation::LoadFrameList(%s) end\t%u\n", szFilePath, KG_GetTickCount() - s_dwTickCount);
#endif 
		return bRetCode;
Exit0:
#if 0
		KGLogPrintf(KGLOG_ERR, "KGUI KWndStation::LoadFrameList(%s)\n", szFilePath);
#endif
		return false;
	}


	bool KWndStation::UnloadFrameList(LPCSTR szFilePath)
	{
#if 0
		ATLTRACE("KWndStation::UnloadFrameList(%s)\n", szFilePath);
#endif

		char szSection[WND_SECTION_NAME_LEN];
		int nValue = 0;
		bool bRetCode = true;
		KSmartIniFile Ini;

		ASSERT(szFilePath);

		Ini = g_OpenIniFile(szFilePath);
		KGLOG_PROCESS_ERROR(Ini);

		// Close all frames
		szSection[0] = '\0';
		while (Ini->GetNextSection(szSection, szSection) && szSection[0] != '\0')
		{
			if (!CloseFrame(szSection))
			{
#if 0
				KGLogPrintf(KGLOG_ERR, "KGUI KWndStation::LoadFrameList(%s) failed\n", szSection);
#endif
				bRetCode = false;
			}
		}

		return bRetCode;
Exit0:
		KGLogPrintf(KGLOG_ERR, "KGUI KWndStation::UnloadFrameList(%s) close frame [%s] failed\n", szFilePath, szSection);
		return false;
	}


	int KWndStation::GetCursorPosInClient(int &nX, int &nY)
	{
		POINT pt;
		::GetCursorPos(&pt);
		::ScreenToClient(g_pUI->m_hWnd, &pt);
		nX = pt.x;
		nY = pt.y;
		return true;
	}

	UINT KWndStation::GetMessage()
	{
		return m_uMsg;
	}

	int	KWndStation::GetMessagePos(int &nX, int &nY)
	{
		nX = m_nCursorX;
		nY = m_nCursorY;

		return true;
	}

	float KWndStation::GetMessageWheelDelta()
	{
		return -((float)(GET_Y_LPARAM(m_wParam))) / WHEEL_DELTA;
	}

	int KWndStation::GetMessageKey()
	{
		return (int)(LOBYTE(LOWORD(m_wParam)));
	}

	//-----------------item事件相关--------------------
	int	KWndStation::SetLastMouseLButtonDownInfo(KItemNull *pItem, DWORD dwReason)
	{
		m_pLastMouseLButtonDownItem = pItem;
		m_dwKeepLastButtonDownReason &= ~ITEM_EVENT_LBUTTONUP;
		m_dwKeepLastButtonDownReason &= ~ITEM_EVENT_LBUTTONCLICK;
		m_dwKeepLastButtonDownReason &= ~ITEM_EVENT_LBUTTONDRAG;
		m_dwKeepLastButtonDownReason |= dwReason;

		m_dwCurrentDragStatus &= ~ITEM_EVENT_LBUTTONDRAG;

		return true;
	}
	KItemNull* KWndStation::GetLastMouseLButtonDownItem()
	{
		return m_pLastMouseLButtonDownItem;
	}

	int	KWndStation::SetLastMouseRButtonDownInfo(KItemNull *pItem, DWORD dwReason)
	{
		m_pLastMouseRButtonDownItem = pItem;
		m_dwKeepLastButtonDownReason &= ~ITEM_EVENT_RBUTTONUP;
		m_dwKeepLastButtonDownReason &= ~ITEM_EVENT_RBUTTONCLICK;
		m_dwKeepLastButtonDownReason &= ~ITEM_EVENT_RBUTTONDRAG;
		m_dwKeepLastButtonDownReason |= dwReason;

		m_dwCurrentDragStatus &= ~ITEM_EVENT_RBUTTONDRAG;

		return true;
	}
	KItemNull* KWndStation::GetLastMouseRButtonDownItem()
	{
		return m_pLastMouseRButtonDownItem;
	}

	int	KWndStation::SetLastMouseMButtonDownInfo(KItemNull *pItem, DWORD dwReason)
	{
		m_pLastMouseMButtonDownItem = pItem;
		m_dwKeepLastButtonDownReason &= ~ITEM_EVENT_MBUTTONUP;
		m_dwKeepLastButtonDownReason &= ~ITEM_EVENT_MBUTTONCLICK;
		m_dwKeepLastButtonDownReason &= ~ITEM_EVENT_MBUTTONDRAG;
		m_dwKeepLastButtonDownReason |= dwReason;

		m_dwCurrentDragStatus &= ~ITEM_EVENT_MBUTTONDRAG;
		return true;
	}
	KItemNull* KWndStation::GetLastMouseMButtonDownItem()
	{
		return m_pLastMouseMButtonDownItem;
	}

	DWORD KWndStation::GetCurrentDragStatus()
	{
		return m_dwCurrentDragStatus;
	}

	float KWndStation::GetUIScale() const
	{
		ASSERT(m_fUIScale > FLT_EPSILON);
		return m_fUIScale;
	}

	int KWndStation::SetUIScale(float fScale, BOOL bForceUpdate /*= false*/)
	{
		int nResult = false;
		float fSacleWnd = 1.0f;

		KGLOG_PROCESS_ERROR(fScale > FLT_EPSILON);

		if (!bForceUpdate)
		{
			KG_PROCESS_SUCCESS(abs(m_fUIScale - fScale) < FLT_EPSILON);
		}

		if (m_fUIScale > FLT_EPSILON )
			fSacleWnd = fScale / m_fUIScale;

		g_pUI->m_FontSchemeMgr.SetFontScale(fScale);

        m_fUIScale = fScale;

		for (int nIndex = WL_BEGIN; nIndex < WL_VISIBLE; ++nIndex) 
			m_LayerRoot[nIndex].Scale(fSacleWnd, fSacleWnd);

		// TODO:

		g_pUI->m_Script.FireGlobalEvent("ResizeUI"); 
		g_pUI->m_Script.FireEvent("UI_SCALED", 0);

Exit1:
		nResult = true;
Exit0:
        if (!nResult)
        {
            KGLogPrintf(KGLOG_ERR, "KWndStation::SetUIScale(%f, %d)\n", fScale, bForceUpdate);
        }
		return nResult;
	}

	float KWndStation::GetMaxUIScale() const
	{
		float fScale = 1.0f;

		if (m_fClientHeight / m_fClientWidth <= m_fStandardHeight / m_fStandardWidth)
		{
			if (m_fClientHeight < m_fStandardHeight)
				fScale = m_fClientHeight / m_fStandardHeight;
		}
		else
		{
			if (m_fClientWidth < m_fStandardWidth)
				fScale = m_fClientWidth / m_fStandardWidth;
		}
		return fScale;
	}

	int KWndStation::SetStandardSize(float fW, float fH)
	{
		int nResult = false;

		KG_PROCESS_ERROR(fW > FLT_EPSILON);
		KG_PROCESS_ERROR(fH > FLT_EPSILON);

		m_fStandardWidth = fW;
		m_fStandardHeight = fH;

		nResult = true;
Exit0:
		return nResult;
	}

	float KWndStation::GetStandardWidth() const
	{
		return m_fStandardWidth;
	}

	float KWndStation::GetStandardHeight() const
	{
		return m_fStandardHeight;
	}

	int	KWndStation::SetClientSize(UINT uWidth, UINT uHeight)
	{
        HRESULT hr = E_FAIL;
        int nRetCode = false;
		int nResult = false;
		float fScale = 1.0f;
		float fSLast = 1.0f;
		float fSet = 1.0f;
        float fWidth = 0.0f;
        float fHeight = 0.0f;
		BOOL bForce = FALSE;

		KG_PROCESS_ERROR(uWidth > 0);
		KG_PROCESS_ERROR(uHeight > 0);

        KG_PROCESS_ERROR(m_fStandardWidth > FLT_EPSILON);
        KG_PROCESS_ERROR(m_fStandardHeight > FLT_EPSILON);

        hr = g_pUI->m_p3DUI->SetViewportSize(uWidth, uHeight);
        KGLOG_COM_PROCESS_ERROR(hr);

		m_DeviceSettings.uWidth = uWidth;
		m_DeviceSettings.uHeight = uHeight;

        fWidth = (float)uWidth;
        fHeight = (float)uHeight;

		if (abs(m_fClientWidth - fWidth) > FLT_EPSILON || abs(m_fClientHeight - fHeight) > FLT_EPSILON)
			bForce = TRUE;

        if (fHeight / fWidth <= m_fStandardHeight / m_fStandardWidth)
		{
			if (fHeight < m_fStandardHeight)
            {
				fScale = fHeight / m_fStandardHeight;
            }
		}
		else
		{
			if (fWidth < m_fStandardWidth)
            {
				fScale = fWidth / m_fStandardWidth;
            }
		}

		if (m_fClientHeight / m_fClientWidth <= m_fStandardHeight / m_fStandardWidth)
		{
			if (m_fClientHeight < m_fStandardHeight)
            {
				fSLast = m_fClientHeight / m_fStandardHeight;
            }
		}
		else
		{
			if (m_fClientWidth < m_fStandardWidth)
            {
				fSLast = m_fClientWidth / m_fStandardWidth;
            }
		}

		fSet = m_fUIScale *  fScale / fSLast;
		
		m_fClientWidth = fWidth;
		m_fClientHeight = fHeight;

		if (fSet > fScale)
			fSet = fScale;

		SetUIScale(fSet, bForce);

		nResult = true;
Exit0:
		return nResult;
	}

	float KWndStation::GetClientWidth() const
	{
		return m_fClientWidth;
	}

	float KWndStation::GetClientHeight() const
	{
		return m_fClientHeight;
	}

	int KWndStation::IsFullScreen() const
	{
		return m_DeviceSettings.bFullScreen;
	}

	int KWndStation::IsPanauision() const
	{
		return m_DeviceSettings.bPanauision;
	}

	int KWndStation::IsExclusiveMode() const
	{
		return m_DeviceSettings.bExclusiveMode;
	}

    int KWndStation::IsClipCursor() const
    {
        return m_DeviceSettings.bExclusiveMode;
    }

    int KWndStation::SetVideoSettings(KG3DDeviceSettings* pSettings)
	{
        HRESULT hr = E_FAIL;
        int nResult = false;
        int nRetCode = false;
		int nX = 0;
		int nY = 0;
        UINT uWidth = 0;
        UINT uHeight = 0;
        RECT rc;
		BOOL bExclusiveStateChanged = FALSE;
		DWORD dwVisible = 0;

        KGLOG_PROCESS_ERROR(pSettings);
        KGLOG_PROCESS_ERROR(pSettings->uWidth >= 100);
        KGLOG_PROCESS_ERROR(pSettings->uHeight >= 30);

        KG_PROCESS_SUCCESS(memcmp(&m_DeviceSettings, pSettings, sizeof(m_DeviceSettings)) == 0);

		bExclusiveStateChanged = m_DeviceSettings.bExclusiveMode != pSettings->bExclusiveMode;
		memcpy(&m_DeviceSettings, pSettings, sizeof(m_DeviceSettings));

		m_bProcessNCSizing = FALSE;

		dwVisible = ::GetWindowLong(g_pUI->m_hWnd, GWL_STYLE) & WS_VISIBLE;

        nRetCode = ::SetWindowPos(g_pUI->m_hWnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
        KGLOG_CHECK_ERROR(nRetCode);

        if (bExclusiveStateChanged)
        {
            hr = g_pUI->m_p3DUI->SetDeviceSettings(&m_DeviceSettings);
            KGLOG_COM_PROCESS_ERROR(hr);
        }

		if (m_DeviceSettings.bExclusiveMode)
		{
			nRetCode = ::SetWindowLong(g_pUI->m_hWnd, GWL_STYLE, WS_OVERLAPPED | dwVisible);
			KGLOG_CHECK_ERROR(nRetCode);

			nRetCode = ::SetWindowPos(g_pUI->m_hWnd, NULL, 0, 0, m_DeviceSettings.uWidth, m_DeviceSettings.uHeight, SWP_NOZORDER | SWP_FRAMECHANGED); 
			KGLOG_CHECK_ERROR(nRetCode);

			goto Exit2;
		}

		if (m_DeviceSettings.bFullScreen)
		{
			MONITORINFO Info;
			Info.cbSize = sizeof(MONITORINFO);
			HMONITOR hM = ::MonitorFromWindow(g_pUI->m_hWnd, MONITOR_DEFAULTTONEAREST);
			if (hM && ::GetMonitorInfo(hM, &Info))
			{
				nX = Info.rcMonitor.left;
				nY = Info.rcMonitor.top;
				uWidth = Info.rcMonitor.right - Info.rcMonitor.left;
				uHeight = Info.rcMonitor.bottom - Info.rcMonitor.top;
			}
			else
			{
				nX = 0;
				nY = 0;
				uWidth = ::GetSystemMetrics(SM_CXSCREEN);
				uHeight = ::GetSystemMetrics(SM_CYSCREEN);
			}

			nRetCode = ::SetWindowLong(g_pUI->m_hWnd, GWL_STYLE, WS_OVERLAPPED | dwVisible);
			KGLOG_CHECK_ERROR(nRetCode);

			m_bProcessNCSizing = TRUE;
			nRetCode = ::SetWindowPos(g_pUI->m_hWnd, NULL, nX, nY, uWidth, uHeight, SWP_NOZORDER | SWP_FRAMECHANGED | SWP_DEFERERASE); 
			KGLOG_CHECK_ERROR(nRetCode);
			::SendMessageA(g_pUI->m_hWnd, WM_NCPAINT, 0, 0);
			m_bProcessNCSizing = FALSE;

			goto Exit2;
		}

        if (m_DeviceSettings.bMaximize)
		{
			nRetCode = ::SetWindowLong(g_pUI->m_hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW | dwVisible | WS_MAXIMIZE);
			KGLOG_CHECK_ERROR(nRetCode);

			goto Exit2;
		}

		uWidth = pSettings->uWidth;
		uHeight = pSettings->uHeight;

		if (m_DeviceSettings.bPanauision)
			uHeight = min(uHeight, uWidth * 9 / 16);

		nRetCode = ::SetWindowLong(g_pUI->m_hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW | dwVisible);
		KGLOG_CHECK_ERROR(nRetCode);
		
		rc.left = 0;
		rc.top = 0;
		rc.right = uWidth;
		rc.bottom = uHeight;
		nRetCode = ::GetClientRect(g_pUI->m_hWnd, &rc);
		KGLOG_CHECK_ERROR(nRetCode);
		rc.right = rc.left + uWidth;
		rc.bottom = rc.top + uHeight;
		nRetCode = ::AdjustWindowRectEx(&rc, ::GetWindowLong(g_pUI->m_hWnd, GWL_STYLE), FALSE, ::GetWindowLong(g_pUI->m_hWnd, GWL_EXSTYLE)); 
		KGLOG_CHECK_ERROR(nRetCode);
		nRetCode = ::SetWindowPos(g_pUI->m_hWnd, NULL, 0, 0, rc.right - rc.left, rc.bottom - rc.top, SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED); 
		KGLOG_CHECK_ERROR(nRetCode);

Exit2:
		nRetCode = ::GetClientRect(g_pUI->m_hWnd, &rc);
		KGLOG_CHECK_ERROR(nRetCode);

		m_DeviceSettings.uWidth = rc.right - rc.left;
		m_DeviceSettings.uHeight = rc.bottom - rc.top;

		hr = g_pUI->m_p3DUI->SetDeviceSettings(&m_DeviceSettings);
		KGLOG_COM_PROCESS_ERROR(hr);

		nRetCode = ::GetClientRect(g_pUI->m_hWnd, &rc);
		KGLOG_CHECK_ERROR(nRetCode);

		m_DeviceSettings.uWidth = rc.right - rc.left;
		m_DeviceSettings.uHeight = rc.bottom - rc.top;

		nRetCode = SetClientSize(m_DeviceSettings.uWidth, m_DeviceSettings.uHeight);
		KGLOG_CHECK_ERROR(nRetCode);

		nRetCode = ::SetWindowPos(g_pUI->m_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		KGLOG_CHECK_ERROR(nRetCode);

Exit1:
        nResult = true;
Exit0:
		if(pSettings)
			memcpy(pSettings, &m_DeviceSettings, sizeof(m_DeviceSettings));
		m_bProcessNCSizing = TRUE;
        return nResult;
	}

	void KWndStation::SetProcessNCSizing(BOOL bProcess)
	{
		m_bProcessNCSizing = bProcess;
	}

	BOOL KWndStation::IsProcessNCSizing()
	{
		return m_bProcessNCSizing;
	}

	void KWndStation::Hide()
	{
		m_nHideUIMode = true;
	}

	void KWndStation::Show()
	{
		m_nHideUIMode = false;
	}

	int KWndStation::IsVisible()
	{
		return !m_nHideUIMode;
	}

	int KWndStation::GetClientX() const
	{
		return m_nClientX;
	}

	int KWndStation::GetClientY() const
	{
		return m_nClientY;
	}

	void KWndStation::SetClientPosition(int nX, int nY)
	{
		m_nClientX = nX;
		m_nClientY = nY;
	}


	int KWndStation::GetWindowX() const
	{
		return m_nWindowX;
	}

	int KWndStation::GetWindowY() const
	{
		return m_nWindowY;
	}

	void KWndStation::SetWindowPosition(int nX, int nY)
	{
		m_nWindowX = nX;
		m_nWindowY = nY;
	}

	DWORD KWndStation::GetIdleTime()
	{
		return (KG_GetTickCount() - m_dwLastUserInputTime);
	}

	void KWndStation::ClearIdleTime()
	{
		m_dwLastUserInputTime = KG_GetTickCount();
	}

	void KWndStation::AddMovingWindow(KWndWindow * pWindow)
	{
		if (!pWindow)
			return;
		for (std::vector<MovingWindow>::iterator it = m_MovingWindow.begin(); it != m_MovingWindow.end(); ++it)
		{
			if (pWindow == it->pWindow)
				return;
		}
		MovingWindow Window;
		Window.pWindow = pWindow;
		Window.fX = pWindow->GetAbsX();
		Window.fY = pWindow->GetAbsY();
		g_pUI->m_Pointer.GetCursorPos(Window.nMouseX, Window.nMouseY);
		m_MovingWindow.push_back(Window);
	}

	void KWndStation::RemoveMovingWindow(KWndWindow * pWindow)
	{
		for (std::vector<MovingWindow>::iterator it = m_MovingWindow.begin(); it != m_MovingWindow.end(); ++it)
		{
			if (pWindow == it->pWindow)
			{
				m_MovingWindow.erase(it);
				break;
			}
		}
	}

}
