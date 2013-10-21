#include "stdafx.h"
#include "InputProxy.h"
//#include "..\..\KG3DEngine\KG3DEngine\KG3DSceneSceneEditor.h"
#include "IEEditor.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

//static const int KEYMAPCOUNT = sizeof(KEYMAP) / sizeof(KEYMAP[0]);
BOOL CInputProxy::m_bIgnoreInit = 0;
WORD CInputProxy::m_wIgnoreKeyBoardMask = 0;
WORD CInputProxy::m_wIgnoreMouseMask = 0;
WORD CInputProxy::m_wIgnoreAllMask = 0;

CInputProxy::CInputProxy() : m_pSceneEditor(NULL), m_pWnd(NULL), m_pKeyMapArray(NULL), m_bInCapture(FALSE)
{
	if (!m_bIgnoreInit)
	{
		KeyStateSnap stKeyState;
		WORD* pdwKeyState = (WORD*)&(stKeyState);

		*pdwKeyState = 0xffff;
		stKeyState.AltState = 0;
		stKeyState.CtrlState = 0;
		stKeyState.ShiftState = 0;
		m_wIgnoreKeyBoardMask = *(WORD*)&(stKeyState);
		*pdwKeyState = 0xffff;
		stKeyState.LBTState = 0;
		stKeyState.MBTState = 0;
		stKeyState.RBTState = 0;
		m_wIgnoreMouseMask = *(WORD*)&(stKeyState);
		//
		stKeyState.AltState = 0;
		stKeyState.CtrlState = 0;
		stKeyState.ShiftState = 0;
		m_wIgnoreAllMask = *(WORD*)&(stKeyState);
		m_bIgnoreInit = TRUE;
	}
}

CInputProxy::~CInputProxy(void)
{
	m_pSceneEditor = NULL;
	m_pWnd = NULL;
	m_pKeyMapArray = NULL;
}

//BYTE     g_SceneEditorKeyState[256];
extern BYTE* g_SceneEditorKeyState;
//extern int g_nMouseMoveX,
void CInputProxy::Init(IEKG3DSceneEditorBase* pSceneEditor, CWnd* pWnd, PROXYKEYMAP* pKeyMapArray)
{
	//KeyStateSnap stKeyState;
	_ASSERTE(pSceneEditor);
	_ASSERTE(pWnd);
	_ASSERTE(pKeyMapArray);

	m_pSceneEditor = pSceneEditor;
	m_pWnd = pWnd;
	m_pKeyMapArray = pKeyMapArray;

	g_SceneEditorKeyState[0] = 1;
}

INT CInputProxy::CheckInputEvent(MSG* pMsg)
{
	_ASSERTE(pMsg);
	if (!pMsg)
		return 0;
	_ASSERTE(m_pSceneEditor);
	_ASSERTE(m_pWnd);
	_ASSERTE(m_pKeyMapArray);
	if (!m_pSceneEditor || !m_pWnd || !m_pKeyMapArray)
		return 0;

	static WORD IgnoreKeyBoardMask = 0;
	static WORD IgnoreMouseMask = 0;
	static WORD IgnoreAllMask = 0;

	INT nRet = 0;
	if ( (pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST) ||
		(pMsg->message >= WM_MOUSEFIRST && pMsg->message <= WM_MOUSELAST)
		)
	{
		KeyStateSnap stKeyState;
		*(WORD*)&(stKeyState) = 0;

		switch(pMsg->message)
		{
		case WM_MBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_LBUTTONDOWN:
			m_pWnd->SetCapture();
			m_bInCapture = TRUE; //只有在当前窗口中点击，才有效
			break;
		case WM_MBUTTONUP:
		case WM_RBUTTONUP:
		case WM_LBUTTONUP:
			ReleaseCapture();
			m_bInCapture = FALSE;
			break;
		}

		stKeyState.AltState = g_SceneEditorKeyState[VK_MENU];
		stKeyState.CtrlState = g_SceneEditorKeyState[VK_CONTROL];
		stKeyState.ShiftState = g_SceneEditorKeyState[VK_SHIFT];
		stKeyState.LBTState = g_SceneEditorKeyState[VK_LBUTTON] && m_bInCapture;
		stKeyState.MBTState = g_SceneEditorKeyState[VK_MBUTTON] && m_bInCapture;
		stKeyState.RBTState = g_SceneEditorKeyState[VK_RBUTTON] && m_bInCapture;
ReLoop:
		LPARAM lParam = pMsg->lParam;
		INT nMapIndex = 0;
		WORD wTemp = 0;
		WORD* pdwKeyState = &wTemp;
		while (m_pKeyMapArray[nMapIndex].wActionID)
		{
			PROXYKEYMAP& KeyMap = m_pKeyMapArray[nMapIndex];
			stKeyState.MainKey = g_SceneEditorKeyState[KeyMap.keystate.MainKey] == 1 ? KeyMap.keystate.MainKey : 0;
			wTemp = *(WORD*)&(stKeyState);
			//忽略鼠标或者键盘，或者两者都忽略
			switch(KeyMap.wIgnoreState)
			{
			case 1://忽略C S A
				*pdwKeyState &= m_wIgnoreKeyBoardMask;
				break;
			case 2://忽略鼠标
				*pdwKeyState &= m_wIgnoreMouseMask;
				break;
			case 3://全部忽略
				*pdwKeyState &= m_wIgnoreAllMask;
				break;
			}

			//if (pMsg->message == KeyMap[nMapIndex].TriggerMsgID || 0 == KeyMap[nMapIndex].TriggerMsgID)
			{
				if (*pdwKeyState == *(WORD*)&(KeyMap.keystate)) //找到符合的组合键
				{
					KeyMap.bPrevState = TRUE;

					if (pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST || pMsg->message != WM_MOUSEMOVE)
					{
						m_pSceneEditor->GetMouseXY(&m_nMouseX, &m_nMouseY);
						lParam = MAKELPARAM(m_nMouseX, m_nMouseY);
					}

					m_pSceneEditor->PreExecAction(KeyMap.wActionID, 1, pMsg->wParam, lParam);
					//如果是不能使用状态的按键，如W，在编辑模式下就没用，要交给WINDOWS继续处理
					if (!m_pSceneEditor->ExecAction(KeyMap.wActionID, 1, pMsg->wParam, lParam))
					{
						//return 0;
						break; //需要完成SetCapture动作，不然拉选框这样的动作鼠标出VIEW范围就不会动了。
					}
					//发送已经完成某动作执行的消息
					m_pWnd->PostMessage(WM_ACTIONEXECUTED, KeyMap.wActionID, 1);
					nRet = KeyMap.wActionID;
					if (KeyMap.bExclusive) //独占时，不需要再判断其他的组合情况, 防止在对话框上改变刷子大小时，地形刷同时在刷地形
						break;
					nMapIndex++;
					continue;
				}
			}
			if (KeyMap.bPrevState &&
				*pdwKeyState != *(WORD*)&(KeyMap.keystate)) //按键状态发生了变化，需要发出通知
			{
				KeyMap.bPrevState = FALSE;
				//notify 3dengine
				if (pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST || pMsg->message != WM_MOUSEMOVE)
				{
					m_pSceneEditor->GetMouseXY(&m_nMouseX, &m_nMouseY);
					lParam = MAKELPARAM(m_nMouseX, m_nMouseY);
				}

				m_pSceneEditor->PreExecAction(KeyMap.wActionID, 0, pMsg->wParam, lParam);
				m_pSceneEditor->ExecAction(KeyMap.wActionID, 0, pMsg->wParam, lParam);
				m_pWnd->PostMessage(WM_ACTIONEXECUTED, KeyMap.wActionID, 0);
				//return KEYMAP[i].wActionID;
				//继续循环，把所有的键查一次
				//有键放开后，需要重新检查所有的键，看是否有t符合的组合键
				goto ReLoop;
			}
			nMapIndex++;
		}
		/*POINT pt;
		GetCursorPos(&pt);
		g_nMouseMoveX = pt.x;
		g_nMouseMoveY = pt.y;*/
		if (pMsg->message >= WM_MOUSEFIRST && pMsg->message <= WM_MOUSELAST)
		{
			int xPos = LOWORD(pMsg->lParam);
			int yPos = HIWORD(pMsg->lParam);
			m_nMouseX = xPos > 0x7fff ? (xPos - 0xffff) : xPos;
			m_nMouseY = yPos > 0x7fff ? (yPos - 0xffff) : yPos;
			m_pSceneEditor->SetMouseXY(m_nMouseX, m_nMouseY);

			m_pSceneEditor->SyncPickRay();

			if(pMsg->message == WM_MOUSEMOVE)
			{
				m_pSceneEditor->OnMouseMove(pMsg->wParam, lParam);
			}
		}
	}
	return nRet;
}
/* 让所有非法使用的人产生链接错误
CInputProxy& CInputProxy::operator=(const CInputProxy& src)
{
	_ASSERTE("非法调用！");
	return *this;
}

CInputProxy::CInputProxy(const CInputProxy& src)
{
	_ASSERTE("非法调用！");
}
*/