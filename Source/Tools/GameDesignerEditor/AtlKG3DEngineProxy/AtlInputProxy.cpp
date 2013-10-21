// AtlInputProxy.cpp : Implementation of CAtlInputProxy

#include "stdafx.h"
#include "AtlInputProxy.h"
#include "IEOther.h"
#include "IEEditor.h"
#include "InputProxy_Impl.h"
#include "Stolen/KG3DSceneLogicalEditorBase.h"


extern BYTE* g_SceneEditorKeyState;
extern IEKG3DEngineManager* g_pEngineManager;
extern IEKG3DSceneSceneEditor* g_pSceneEditor;
extern IEKG3DSceneEditorBase* g_pEditorBase;
extern IEKG3DSceneLogicalEditorBase* g_pLogicEditorBase;


// CAtlInputProxy
STDMETHODIMP CAtlInputProxy::Init(LONG hWnd)
{
	m_inputproxy = new CInputProxy();
	static PROXYKEYMAP KEYMAP[] =
	{	//           C  S  A  L  M  R  ?    Trigger       ID  Prev Ig Exclusive
		{'X'	   , 0, 0, 0, 1, 0, 0, 0, WM_LBUTTONDOWN, EXEACTION_PRESURE,  0, 0, 1},	//压感调整
		{'Z'	   , 0, 1, 0, 1, 0, 0, 0, WM_LBUTTONDOWN, EXEACTION_INNER_CIRCLE,  0, 0, 1},	//内外圈同时调整
		{'Z'	   , 0, 0, 0, 1, 0, 0, 0, WM_LBUTTONDOWN, EXEACTION_OUTTER_CIRCLE,  0, 0, 1},	//外圈调整

		{VK_RBUTTON, 0, 0, 0, 0, 0, 1, 0, WM_MOUSEMOVE,   EXEACTION_ROTATE_CAMERA,  0, 0, 0},	//alt+MBT, 旋转摄像机镜头
		{VK_CONTROL, 1, 0, 0, 0, 1, 0, 0, WM_MOUSEMOVE,   EXEACTION_ZOOM_VIEW,  0, 0, 0},	//ctrl+MBT, 快速缩放视角
		{VK_MBUTTON, 0, 0, 0, 0, 1, 0, 0, WM_MOUSEMOVE,   EXEACTION_PAN_VIEW,  0, 0, 0},	//MBT, PAN视图
		{VK_MBUTTON, 0, 1, 0, 0, 1, 0, 0, WM_MOUSEMOVE,   EXEACTION_ROTATE_VIEW,  0, 0, 0},	//SHIFT+MBT, 以选定点为中心旋转视图
		{VK_LBUTTON, 1, 0, 0, 1, 0, 0, 0, WM_LBUTTONDOWN, EXEACTION_INVERT_ACTION,  0, 0, 0},	//CTRL＋LBT，反向当前操作
		{VK_LBUTTON, 0, 1, 0, 1, 0, 0, 0, WM_MOUSEMOVE,   EXEACTION_DE_MOVE_ANEAR_GROUND,  0, 0, 0}, // shift + 左键移动，物件贴地
		
		
		// ahpho suntao
 		{'W'	   , 0, 0, 0, 0, 0, 0, 0, WM_KEYDOWN    , EXEACTION_GO_FORWARD,  0, 2, 0},	//前进
 		{'S'	   , 0, 0, 0, 0, 0, 0, 0, WM_KEYDOWN    , EXEACTION_GO_BACKWARD, 0, 2, 0},	//后退
 		//{'A'	   , 0, 0, 0, 0, 0, 0, 0, WM_KEYDOWN    , EXEACTION_TURN_LEFT,   0, 2, 0},	//左
 		//{'D'	   , 0, 0, 0, 0, 0, 0, 0, WM_KEYDOWN    , EXEACTION_TURN_RIGHT,  0, 2, 0},	//右
		{'A'	   , 0, 0, 0, 0, 0, 0, 0, WM_KEYDOWN    , EXEACTION_MOV_LEFT,    0, 2, 0},	//左移 忽略键盘
		{'D'	   , 0, 0, 0, 0, 0, 0, 0, WM_KEYDOWN    , EXEACTION_MOV_RIGHT,   0, 2, 0},	//右移 忽略键盘
		{'Q'	   , 0, 0, 0, 0, 0, 0, 0, WM_KEYDOWN    , EXEACTION_MOV_UP,      0, 3, 0},	//升高，忽略所有
		{'E'	   , 0, 0, 0, 0, 0, 0, 0, WM_KEYDOWN    , EXEACTION_MOV_DOWN,    0, 3, 0},	//下降，忽略所有

		{VK_RBUTTON, 0, 0, 0, 0, 0, 1, 0, WM_MOUSEMOVE  , EXEACTION_CAM_CIRCLE,  0, 0, 0},	//按住右键不放，就转动摄像机
		{' '	   , 0, 0, 0, 0, 0, 0, 0, WM_KEYDOWN    , EXEACTION_JUMP,  0, 3, 0},	//跳，忽略所有
		{'A'	   , 0, 0, 0, 0, 0, 0, 0, WM_KEYDOWN    , EXEACTION_TURN_LEFT_VIEW_FOLLOWED,  0, 1, 0},	//左转，同时转动摄像机
		{'D'	   , 0, 0, 0, 0, 0, 0, 0, WM_KEYDOWN    , EXEACTION_TURN_RIGHT_VIEW_FOLLOWED,  0, 1, 0},	//右转

		{VK_LBUTTON, 0, 0, 0, 1, 0, 0, 0, WM_LBUTTONDOWN, EXEACTION_LEFE_KEY_DOWN,  0, 0, 0},	//左键按下，可以作选择、改变地形等很多动作
		{VK_SHIFT,   0, 1, 0, 1, 0, 0, 0, WM_LBUTTONDOWN, EXEACTION_SMOOTH_TERRAIN,  0, 0, 0},	//平滑

		{VK_ADD,	 0, 0, 0, 0, 0, 0, 0, WM_KEYUP,		EXEACTION_SPEED_UP,  0, 3, 0},	//加速
		{VK_SUBTRACT,0, 0, 0, 0, 0, 0, 0, WM_KEYUP,     EXEACTION_SPEED_DOWN,  0, 3, 0},	//减速

		{VK_LBUTTON, 1, 0, 0, 1, 0, 0, 0, WM_LBUTTONDOWN, EXEACTION_SELECTION_ADD,  0, 0, 0},	//在当前选择范围中，再增加选择内容
		{VK_LBUTTON, 0, 0, 1, 1, 0, 0, 0, WM_LBUTTONDOWN, EXEACTION_SELECTION_SUTRACT,  0, 0, 0},	//在当前选择范围中，再减少选择内容

		{'1'	   , 0, 0, 0, 0, 0, 0, 0, WM_KEYDOWN   , EXEACTION_SELECT,  0, 3, 0},	//选
		{'2'	   , 0, 0, 0, 0, 0, 0, 0, WM_KEYDOWN   , EXEACTION_MOVE,    0, 3, 0},	//移
		{'3'	   , 0, 0, 0, 0, 0, 0, 0, WM_KEYDOWN   , EXEACTION_ROTATE,  0, 3, 0},	//转
		{'4'	   , 0, 0, 0, 0, 0, 0, 0, WM_KEYDOWN   , EXEACTION_SCALE,   0, 3, 0},	//放

		{0		   , 0, 0, 0, 0, 0, 0, 0, 0            , 0,  0, 0, 0},	//结束
		//           C  S  A  L  M  R  ?    Trigger     ID  Prev Ig Exclusive
	};


	m_inputproxy->Init(g_pEditorBase, CWnd::FromHandle((HWND)hWnd), KEYMAP);

	KG3DSceneLogicalEditorBase* pLogicEditorBase = (KG3DSceneLogicalEditorBase*)g_pLogicEditorBase;
	if (pLogicEditorBase)
		pLogicEditorBase->SetInputProxy(m_inputproxy);
		
	return S_OK;
}

STDMETHODIMP CAtlInputProxy::CheckInput(PMSG pMsg, LONG* pnRet)
{
	if (pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST)
	{
		g_SceneEditorKeyState[pMsg->wParam] = !!(GetAsyncKeyState(INT(pMsg->wParam)) & 0x8000);
	}
	if (pMsg->message >= WM_MOUSEFIRST && pMsg->message <= WM_MOUSELAST && pMsg->message != WM_MOUSEMOVE || pMsg->message == WM_COMMAND)
	{	
		g_SceneEditorKeyState[VK_LBUTTON] = !!(GetAsyncKeyState(VK_LBUTTON) & 0x8000);
		g_SceneEditorKeyState[VK_MBUTTON] = !!(GetAsyncKeyState(VK_MBUTTON) & 0x8000);
		g_SceneEditorKeyState[VK_RBUTTON] = !!(GetAsyncKeyState(VK_RBUTTON) & 0x8000);
	}
	*pnRet = m_inputproxy->CheckInputEvent(pMsg);
	return S_OK;
}
