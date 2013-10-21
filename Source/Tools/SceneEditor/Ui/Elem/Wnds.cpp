/*****************************************************************************************
//	剑侠引擎，界面窗口体系结构的最基本窗口对象
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-7-9
*****************************************************************************************/
#include "../../Public/KWin32.h"

#include "../../Public/KEngine.h"
#include "../../Public/KWin32Wnd.h"
#include "../Elem/WndMessage.h"
#include "Wnds.h"
#include "WndWindow.h"
#include "UiCursor.h"
#include "WndGameSpace.h"
#include "MouseHover.h"
#include "PopupMenu.h"
//#include "../UiBase.h"
#include "../ShortcutKey.h"

#include "CommCtrl.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

int WND_SHOW_MOUSE_OVER_WND = false;

//=================
//		窗口站
//=================
static struct KWndStation
{
	KWndWindow		LowLayerRoot;
	KWndWindow		NormalLayerRoot;
	KWndWindow		TopLayerRoot;
	KWndWindow*		pFocusWnd;
	KWndWindow*		pMouseOverWnd;
	KWndWindow*		pCaptureMouseWnd;
	KWndWindow*		pExclusiveWnd[4];

	KUiCursor		Cursor;
	SIZE			ScreenSize;

	struct KDragInfo
	{
		int					bDragging;
		KUiDraggedObject	DraggedObj;
		fnDrawDraggedObj	fnDrawObjFnc;
		int					nDropQeuryRsult;
	}		DragInfo;

	KWndWindow*		pGameSpaceWnd;
	unsigned char	bGameSpaceNotHandleInput;
	bool			bPaintGameSpace;
	bool			bDontSendToCaptureChild;

	KWndStation()
	{
		pFocusWnd        = NULL;
		pMouseOverWnd    = NULL;
		pCaptureMouseWnd = NULL;
		pExclusiveWnd[0] = NULL;
		pExclusiveWnd[1] = NULL;
		pExclusiveWnd[2] = NULL;
		pExclusiveWnd[3] = NULL;
		pGameSpaceWnd	 = NULL;
		bGameSpaceNotHandleInput = 1;
		bPaintGameSpace = false;
		bDontSendToCaptureChild = true;
		LowLayerRoot.Hide();
		NormalLayerRoot.Hide();
		TopLayerRoot.Hide();
		DragInfo.bDragging = false;
		DragInfo.DraggedObj.uGenre = 0;
		DragInfo.DraggedObj.uId = 0;
		ScreenSize.cx = 0;
		ScreenSize.cy = 0;
	}
}	s_WndStation;

static KWndWindow*	Wnd_GetActive(int x, int y, bool bBringToTop);

//--------------------------------------------------------------------------
//	功能：清除工作，销毁全部的窗口
//--------------------------------------------------------------------------
void Wnd_Cleanup()
{
	Wnd_Heartbeat();

	s_WndStation.pFocusWnd = NULL;
	s_WndStation.pMouseOverWnd = NULL;
	s_WndStation.pCaptureMouseWnd = NULL;
	s_WndStation.pExclusiveWnd[0] = NULL;
	s_WndStation.pExclusiveWnd[1] = NULL;
	s_WndStation.pExclusiveWnd[2] = NULL;
	s_WndStation.pExclusiveWnd[3] = NULL;
	s_WndStation.pGameSpaceWnd = NULL;

	s_WndStation.Cursor.Cleanup();
	s_WndStation.DragInfo.bDragging = false;
}

//--------------------------------------------------------------------------
//	功能：绘制窗口
//--------------------------------------------------------------------------
void Wnd_RenderWindows()
{
	int	bShowCursor = true;

	if (s_WndStation.pGameSpaceWnd && s_WndStation.bPaintGameSpace)
		s_WndStation.pGameSpaceWnd->Paint();
	s_WndStation.LowLayerRoot.Paint();
	s_WndStation.NormalLayerRoot.Paint();
	s_WndStation.TopLayerRoot.Paint();

	if (s_WndStation.DragInfo.bDragging &&
		s_WndStation.DragInfo.fnDrawObjFnc)
	{
		int h, v;
		s_WndStation.Cursor.GetPosition(h, v);
		if (s_WndStation.DragInfo.fnDrawObjFnc(h, v,
			s_WndStation.DragInfo.DraggedObj,
			s_WndStation.DragInfo.nDropQeuryRsult) == 0)
			bShowCursor = false;
	}
	KPopupMenu::PaintMenu();
	g_MouseOver.PaintMouseHoverInfo();

	if (WND_SHOW_MOUSE_OVER_WND && s_WndStation.pMouseOverWnd)
		s_WndStation.pMouseOverWnd->PaintDebugInfo();

	if (bShowCursor)
		s_WndStation.Cursor.Paint();
}

//--------------------------------------------------------------------------
//	功能：让窗口活动
//--------------------------------------------------------------------------
void Wnd_Heartbeat()
{
	s_WndStation.LowLayerRoot.LetMeBreathe();
	s_WndStation.NormalLayerRoot.LetMeBreathe();
	s_WndStation.TopLayerRoot.LetMeBreathe();
	if (s_WndStation.pExclusiveWnd[0])
		s_WndStation.pExclusiveWnd[0]->BringToTop();
}

//--------------------------------------------------------------------------
//	功能：添加窗口
//--------------------------------------------------------------------------
void Wnd_AddWindow(KWndWindow* pWnd, WND_LAYER_LIST eLayer/* = WL_NORMAL*/)
{
	if (eLayer == WL_LOWEST)
		s_WndStation.LowLayerRoot.AddBrother(pWnd);
	else if (eLayer == WL_TOPMOST)
		s_WndStation.TopLayerRoot.AddBrother(pWnd);
	else
		s_WndStation.NormalLayerRoot.AddBrother(pWnd);
}

//--------------------------------------------------------------------------
//	功能：设置运行游戏窗口
//--------------------------------------------------------------------------
void Wnd_SetGameSpaceWnd(KWndWindow* pWnd)
{
	s_WndStation.pGameSpaceWnd = pWnd;
}

void Wnd_GameSpaceHandleInput(bool bHandle)
{
/*	if (bHandle)
	{
		if (s_WndStation.bGameSpaceNotHandleInput)
			s_WndStation.bGameSpaceNotHandleInput --;
	}
	else
		s_WndStation.bGameSpaceNotHandleInput ++;
*/
	s_WndStation.bGameSpaceNotHandleInput = !bHandle;
}


BOOL Wnd_IsReceiveInput()
{
	return !s_WndStation.bGameSpaceNotHandleInput;
}


void Wnd_ShowHideGameSpace(bool bShow)
{
	s_WndStation.bPaintGameSpace = bShow;
}

//--------------------------------------------------------------------------
//	功能：响应窗口析构
//--------------------------------------------------------------------------
void Wnd_OnWindowDelete(KWndWindow* pWnd)
{
	if (pWnd)
	{
		if (s_WndStation.pMouseOverWnd == pWnd)
			s_WndStation.pMouseOverWnd = NULL;
		if (s_WndStation.pCaptureMouseWnd == pWnd)
			s_WndStation.pCaptureMouseWnd = NULL;
		if (s_WndStation.pFocusWnd == pWnd)
			s_WndStation.pFocusWnd = NULL;
		if (s_WndStation.pExclusiveWnd[0] == pWnd)
		{
			s_WndStation.pExclusiveWnd[0] = s_WndStation.pExclusiveWnd[1];
			s_WndStation.pExclusiveWnd[1] = s_WndStation.pExclusiveWnd[2];
			s_WndStation.pExclusiveWnd[2] = s_WndStation.pExclusiveWnd[3];
			s_WndStation.pExclusiveWnd[3] = NULL;
		}
		else if (s_WndStation.pExclusiveWnd[1] == pWnd)
		{
			s_WndStation.pExclusiveWnd[1] = s_WndStation.pExclusiveWnd[2];
			s_WndStation.pExclusiveWnd[2] = s_WndStation.pExclusiveWnd[3];
			s_WndStation.pExclusiveWnd[3] = NULL;
		}
		else if (s_WndStation.pExclusiveWnd[2] == pWnd)
		{
			s_WndStation.pExclusiveWnd[2] = s_WndStation.pExclusiveWnd[3];
			s_WndStation.pExclusiveWnd[3] = NULL;
		}
		else if (s_WndStation.pExclusiveWnd[3] == pWnd)
			s_WndStation.pExclusiveWnd[3] = NULL;
		else
			KPopupMenu::OnWndDelete(pWnd);
	}
}

//--------------------------------------------------------------------------
//	功能：开始拖动物体
//--------------------------------------------------------------------------
int Wnd_DragBegin(const KUiDraggedObject* pObj, fnDrawDraggedObj fnDrawObjFunc)
{
	//原来有个对象正在被拖动
	if (s_WndStation.DragInfo.bDragging == false && pObj && pObj->uGenre)
	{
		s_WndStation.DragInfo.DraggedObj = *pObj;
		s_WndStation.DragInfo.fnDrawObjFnc = fnDrawObjFunc;
		s_WndStation.DragInfo.bDragging = true;
		return true;
	}
	return false;
}

int	Wnd_GetDragObj(KUiDraggedObject* pObj)
{
	if (pObj)
	{
		if (s_WndStation.DragInfo.bDragging)
			memcpy(pObj, &s_WndStation.DragInfo.DraggedObj, sizeof(KUiDraggedObject));
		else
			memset(pObj, 0, sizeof(KUiDraggedObject));
	}
	return s_WndStation.DragInfo.bDragging;
}
//--------------------------------------------------------------------------
//	功能：结束拖动状态
//--------------------------------------------------------------------------
void Wnd_DragFinished()
{
	s_WndStation.DragInfo.bDragging = false;
}

//--------------------------------------------------------------------------
//	功能：处理输入
//--------------------------------------------------------------------------
void Wnd_ProcessInput(unsigned int uMsg, unsigned int uParam, int nParam)
{
	KWndWindow* pActiveWnd = NULL;
	static bool bLastCursorEventProcessedByGameSpace = false;

	int	bHandle = false;
	for (int i = 0; i < 3 && bHandle == false; i++)
	{
		if (i == 0)
			pActiveWnd = s_WndStation.TopLayerRoot.GetNextWnd();
		else if (i == 1)
			pActiveWnd = s_WndStation.NormalLayerRoot.GetNextWnd();
		else
			pActiveWnd = s_WndStation.LowLayerRoot.GetNextWnd();

		while(pActiveWnd)
		{
			KWndWindow* pNextWnd = pActiveWnd->GetNextWnd();
			if (pActiveWnd->IsVisible() &&
				pActiveWnd->WndProc(uMsg, uParam, nParam))
			{
				bHandle = true;
				break;
			}
			pActiveWnd = pNextWnd;
		}
	}

	if (bHandle == false && s_WndStation.pGameSpaceWnd &&
		s_WndStation.bGameSpaceNotHandleInput == false)
	{
		s_WndStation.pGameSpaceWnd->WndProc(uMsg, uParam, nParam);
	}


	return ;

	if (InSendMessage())
		return;

    if (uMsg == WM_SETCURSOR)
    {
        Wnd_RestoreCursor();
        return;
    }

	//====鼠标指针消息====
	if ((uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST) || uMsg == WM_MOUSEHOVER)
	{
		int x = 0;
		int y = 0;

		if (uMsg != WM_MOUSEWHEEL)
		{
			x = GET_X_LPARAM(nParam);
			y = GET_Y_LPARAM(nParam);
		}
		else
		{
			s_WndStation.Cursor.GetPosition(x, y);
		}
		//====设置鼠标指针图形位置====
		s_WndStation.Cursor.SetPosition(x, y);

		if (KPopupMenu::HandleInput(uMsg, uParam, nParam))
		{
			if (bLastCursorEventProcessedByGameSpace)
			{
				bLastCursorEventProcessedByGameSpace = false;
				//g_UiBase.SetCurrentCursor();
			}
			return;
		}
		g_MouseOver.UpdateCursorPos(x, y);

		if (s_WndStation.pCaptureMouseWnd)
			pActiveWnd = s_WndStation.pCaptureMouseWnd;
		else if (s_WndStation.pExclusiveWnd[0] == NULL)
		{
			//====取得鼠标所指的最上层窗口面板====
			//====让被鼠标点击的窗口成为最上层窗口面板====
			if ((uMsg == WM_LBUTTONDOWN || uMsg == WM_RBUTTONDOWN || uMsg == WM_MBUTTONDOWN) &&
				s_WndStation.DragInfo.bDragging == 0)
			{
				DWORD	dwPos = GetMessagePos();
				POINT	pos;
				pos.x = GET_X_LPARAM(dwPos);
				pos.y = GET_Y_LPARAM(dwPos);
				//g_ScreenToClient(&pos);
				ScreenToClient(g_GetDrawHWnd(), &pos);
				if (pos.x != x || pos.y != y)
					return;
				pActiveWnd = Wnd_GetActive(x, y, true);
			}
			else
			{
				pActiveWnd = Wnd_GetActive(x, y, false);
			}
		}
		else	//有独占窗口时，Active窗口就是它了
			pActiveWnd = s_WndStation.pExclusiveWnd[0];

		{
			KWndWindow* pTopWnd = NULL;
			if (pActiveWnd)
				pTopWnd = pActiveWnd->TopChildFromPoint(x, y);	//只传给位于鼠标指针下方的最上层的窗口
			if (pTopWnd != s_WndStation.pMouseOverWnd)
			{
				g_MouseOver.CancelMouseHoverInfo();
				KWndWindow* pOverParent = NULL;
				if (s_WndStation.pMouseOverWnd)
				{
					s_WndStation.pMouseOverWnd->WndProc(WND_M_MOUSE_LEAVE, 0, 0);
					pOverParent = s_WndStation.pMouseOverWnd->GetParent();
					if (pOverParent)
					{
						pOverParent->WndProc(WND_N_MOUSE_LEAVE_CHILD,
							(unsigned int)s_WndStation.pMouseOverWnd, 0);
					}
				}
				if (pTopWnd)
				{
					pTopWnd->WndProc(WND_M_MOUSE_ENTER, 0, 0);
					pOverParent = pTopWnd->GetParent();
					if (pOverParent)
					{
						pOverParent->WndProc(WND_N_MOUSE_ENTER_CHILD,
							(unsigned int)pTopWnd, 0);
					}
				}
				s_WndStation.pMouseOverWnd = pTopWnd;
			}

			if (s_WndStation.pFocusWnd && pTopWnd != s_WndStation.pFocusWnd &&
				(uMsg == WM_LBUTTONDOWN || uMsg == WM_RBUTTONDOWN || uMsg == WM_MBUTTONDOWN))
			{
				Wnd_SetFocusWnd(NULL);
			}
		}

		bool bPrecessedByGameSpace = false;
		KWndWindow* pDestWnd = s_WndStation.pMouseOverWnd;
		if (s_WndStation.pCaptureMouseWnd)
		{
			if (s_WndStation.bDontSendToCaptureChild || pDestWnd == NULL)
			pDestWnd = s_WndStation.pCaptureMouseWnd;
		}
		else if (pDestWnd == NULL && s_WndStation.pExclusiveWnd[0] == NULL &&
			s_WndStation.bGameSpaceNotHandleInput == false && s_WndStation.pGameSpaceWnd)
		{
			pDestWnd = s_WndStation.pGameSpaceWnd;
			bLastCursorEventProcessedByGameSpace = true;
			bPrecessedByGameSpace = true;
		}
		if (pDestWnd)
			pDestWnd->WndProc(uMsg, uParam, nParam);

		if (bPrecessedByGameSpace == false && bLastCursorEventProcessedByGameSpace)
		{
			bLastCursorEventProcessedByGameSpace = false;
			//g_UiBase.SetCurrentCursor();
		}
	}
	else
	{//====处理其他消息（其实都是键盘输入消息）====
		if (KPopupMenu::HandleInput(uMsg, uParam, nParam))
			return;

		if (s_WndStation.pFocusWnd && s_WndStation.pFocusWnd->WndProc(uMsg, uParam, nParam))
			return;
		if (s_WndStation.pExclusiveWnd[0])
		{
			s_WndStation.pExclusiveWnd[0]->WndProc(uMsg, uParam, nParam);
			return;
		}
		if (uMsg == WM_KEYDOWN || (uMsg == WM_KEYUP && uParam == VK_SNAPSHOT))
		{
			if (LOWORD(nParam) == 0)
				return;
			int nModifier = 0;
			if (GetKeyState(VK_CONTROL) & 0x8000)
				nModifier |= HOTKEYF_CONTROL;

			if (GetKeyState(VK_SHIFT) & 0x8000)
				nModifier |= HOTKEYF_SHIFT;

			if (GetKeyState(VK_MENU) & 0x8000)
			{
				nModifier |= HOTKEYF_ALT;
			}

			//if (KShortcutKeyCentre::HandleKeyInput(uParam, nModifier))
			//	return;
		}
		else if (uMsg == WM_SYSKEYDOWN || (uMsg == WM_SYSKEYUP && uParam == VK_MENU))
		{
			int nModifier = 0;
			if (GetKeyState(VK_CONTROL) & 0x8000)
				nModifier |= HOTKEYF_CONTROL;

			if (GetKeyState(VK_SHIFT) & 0x8000)
				nModifier |= HOTKEYF_SHIFT;

			if (GetKeyState(VK_MENU) & 0x8000)
				nModifier |= HOTKEYF_ALT;

			//if (KShortcutKeyCentre::HandleKeyInput(uParam, nModifier))
			//	return;
		}

		int	bHandle = false;
		for (int i = 0; i < 3 && bHandle == false; i++)
		{
			if (i == 0)
				pActiveWnd = s_WndStation.TopLayerRoot.GetNextWnd();
			else if (i == 1)
				pActiveWnd = s_WndStation.NormalLayerRoot.GetNextWnd();
			else
				pActiveWnd = s_WndStation.LowLayerRoot.GetNextWnd();

			while(pActiveWnd)
			{
				KWndWindow* pNextWnd = pActiveWnd->GetNextWnd();
				if (pActiveWnd->IsVisible() &&
					pActiveWnd->WndProc(uMsg, uParam, nParam))
				{
					bHandle = true;
					break;
				}
				pActiveWnd = pNextWnd;
			}
		}

		if (bHandle == false && s_WndStation.pGameSpaceWnd &&
			s_WndStation.bGameSpaceNotHandleInput == false)
		{
			s_WndStation.pGameSpaceWnd->WndProc(uMsg, uParam, nParam);
		}
	}
}

void Wnd_TransmitInputToGameSpace(unsigned int uMsg, unsigned int uParam, int nParam)
{
	if (s_WndStation.bGameSpaceNotHandleInput == false && s_WndStation.pGameSpaceWnd)
		s_WndStation.pGameSpaceWnd->WndProc(uMsg, uParam, nParam);
}

//--------------------------------------------------------------------------
//	功能：把处于某个位置的窗口带到最上层
//	返回：最上层窗口的指针
//--------------------------------------------------------------------------
KWndWindow* Wnd_GetActive(int x, int y, bool bBringToTop)
{
	KWndWindow* pActive = NULL;
	for (int i = 0; i < 3; i++)
	{
		KWndWindow*	pWnd = NULL;
		if (i == 0)
			pWnd = &s_WndStation.TopLayerRoot;
		else if (i == 1)
			pWnd = &s_WndStation.NormalLayerRoot;
		else
			pWnd = &s_WndStation.LowLayerRoot;

		while(pWnd = pWnd->GetNextWnd())
		{
			if (pWnd->IsVisible())
				pActive = pWnd;
		}
		while(pActive)
		{
			if (pActive->PtInWindow(x, y))
			{
				if (i == 1 && bBringToTop)
					pActive->BringToTop();
				break;
			}
			pActive = pActive->GetPreWnd();
		}
		if (pActive)
			break;
	}
	return pActive;
}

//改变鼠标指针显示状态
void Wnd_ShowCursor(int bShow)
{
	s_WndStation.Cursor.Show(bShow);
}

//--------------------------------------------------------------------------
//	功能：设置鼠标指针位置
//--------------------------------------------------------------------------
void Wnd_SetCursorPos(int x, int y)
{
	s_WndStation.Cursor.SetPosition(x, y);

	HWND	hWnd = g_GetMainHWnd();
	POINT	Pos;
	Pos.x = x;
	Pos.y = y;
	::ClientToScreen(hWnd, &Pos);
	::SetCursorPos(Pos.x, Pos.y);
}

//--------------------------------------------------------------------------
//	功能：获取鼠标指针位置
//--------------------------------------------------------------------------
void Wnd_GetCursorPos(int* px, int* py)
{
	int h, v;
	s_WndStation.Cursor.GetPosition(h, v);
	if (px)
		*px = h;
	if (py)
		*py = v;
}

//--------------------------------------------------------------------------
//	功能：获取当前挂接的输入焦点窗口
//--------------------------------------------------------------------------
KWndWindow* Wnd_GetFocusWnd()
{
	return s_WndStation.pFocusWnd;
}

//--------------------------------------------------------------------------
//	功能：挂接输入焦点窗口
//--------------------------------------------------------------------------
void Wnd_SetFocusWnd(KWndWindow* pWnd)
{
	if (s_WndStation.pFocusWnd != pWnd)
	{
		if (s_WndStation.pFocusWnd)
			s_WndStation.pFocusWnd->WndProc(WND_M_KILL_FOCUS, (unsigned int)pWnd, 0);
		if (pWnd)
			pWnd->WndProc(WND_M_SET_FOCUS, (unsigned int)s_WndStation.pFocusWnd, 0);
		s_WndStation.pFocusWnd = pWnd;
	}
}

//--------------------------------------------------------------------------
//	功能：设置跟踪鼠标动作窗口
//	参数：bDontSendToChild -> 是否可以把消息发给子窗口，true不发
//--------------------------------------------------------------------------
void Wnd_SetCapture(KWndWindow* pWnd, bool bDontSendToChild/* = true*/)
{
	if (s_WndStation.pCaptureMouseWnd != pWnd)
	{
		Wnd_ReleaseCapture();
		if (s_WndStation.pCaptureMouseWnd = pWnd)
		{
			SetCapture(g_GetMainHWnd());
			s_WndStation.bDontSendToCaptureChild = bDontSendToChild;
		}
	}
}

//--------------------------------------------------------------------------
//	功能：释放窗口对鼠标动作的跟踪
//--------------------------------------------------------------------------
void Wnd_ReleaseCapture()
{
	if (s_WndStation.pCaptureMouseWnd)
	{
		ReleaseCapture();
		s_WndStation.pCaptureMouseWnd->WndProc(WND_M_CAPTURECHANGED, 0, 0);
		s_WndStation.pCaptureMouseWnd = NULL;
	}
}

//--------------------------------------------------------------------------
//	功能：设置独占窗口
//--------------------------------------------------------------------------
void Wnd_SetExclusive(KWndWindow* pWnd)
{
	int i;
	if (pWnd)
	{
		if (s_WndStation.pFocusWnd && s_WndStation.pFocusWnd->GetOwner() != pWnd->GetOwner())
			Wnd_SetFocusWnd(NULL);

		for (i = 0; i < 4; i++)
		{
			if (s_WndStation.pExclusiveWnd[i] == pWnd)
			{
				for (int j = i; j > 0; j--)
					s_WndStation.pExclusiveWnd[j] = s_WndStation.pExclusiveWnd[j - 1];
				s_WndStation.pExclusiveWnd[0] = pWnd;
				return;
			}
		}
		s_WndStation.pExclusiveWnd[3] = s_WndStation.pExclusiveWnd[2];
		s_WndStation.pExclusiveWnd[2] = s_WndStation.pExclusiveWnd[1];
		s_WndStation.pExclusiveWnd[1] = s_WndStation.pExclusiveWnd[0];
		s_WndStation.pExclusiveWnd[0] = pWnd;
	}
}

//--------------------------------------------------------------------------
//	功能：取消独占窗口
//--------------------------------------------------------------------------
void Wnd_ReleaseExclusive(KWndWindow* pWnd)
{
	if (pWnd)
	{
		for (int i = 0; i < 4; i++)
		{
			if (s_WndStation.pExclusiveWnd[i] == pWnd)
			{
				for (int j = i; j < 3; j++)
					s_WndStation.pExclusiveWnd[j] = s_WndStation.pExclusiveWnd[j + 1];
				s_WndStation.pExclusiveWnd[3] = NULL;
			}
		}
	}
}

//--------------------------------------------------------------------------
//	功能：载入鼠标指针资源
//--------------------------------------------------------------------------
void Wnd_LoadCursor(int nIndex, const char* pImgFile)
{
	s_WndStation.Cursor.SetImage(nIndex, pImgFile);
}

//--------------------------------------------------------------------------
//	功能：切换当前鼠标指针图形
//--------------------------------------------------------------------------
int Wnd_SwitchCursor(int nIndex)
{
	return s_WndStation.Cursor.SwitchImage(nIndex);
}

//--------------------------------------------------------------------------
//	功能：获得当前鼠标指针图形
//--------------------------------------------------------------------------
int	Wnd_GetCurrentCursor()
{
	return s_WndStation.Cursor.GetCursorImageIndex();
}

//设置屏幕范围大小
void Wnd_SetScreenSize(int nWidth, int nHeight)
{
	s_WndStation.ScreenSize.cx = nWidth;
	s_WndStation.ScreenSize.cy = nHeight;
}

//获取屏幕范围大小
void Wnd_GetScreenSize(int& nWidth, int& nHeight)
{
	nWidth = s_WndStation.ScreenSize.cx;
	nHeight = s_WndStation.ScreenSize.cy;
}

//还原鼠标图形
void	Wnd_RestoreCursor()
{
	s_WndStation.Cursor.RestoreCursor();
}