#include "StdAfx.h"
#include "filetype.h"
#include "file.h"
#include "KSO3World.h"
#include "KScene.h"
#include "KPatrolPath.h"
#include "SceneEditorCommon.h"
#include "InputProxy.h"
#include "Functions.h"


CDllHolder g_ModuleRepresent;
CDllHolder g_ModuleUI;
IKSO3RepresentInterface* g_pRepresent;
IKSO3RepresentHandler* g_pRepresentHandler = NULL;
ISO3UI* g_pUI;
//BOOL g_bUseSO3Logical = TRUE;
BOOL g_bSO3LogicInited = FALSE;
BOOL g_bKG3DEngineInited = FALSE;
extern TCHAR g_EnginePath[MAX_PATH];
//extern CProgressForm g_ProgressDlg;
extern CStatic g_ProgressName;
extern CProgressCtrl g_ProgressProgress;


BOOL InitUI()
{
	BOOL bRetCode = FALSE;
	fnCreateSO3UI pfnCreateSO3UI = NULL;

	KGLOG_PROCESS_ERROR(g_GetEngineIEInterface() && "init 3d engine first!");
	KGLOG_PROCESS_ERROR(g_pRepresent && "init represent first!");

	UnInitUI();

#ifdef _DEBUG
	g_ModuleUI.Init("SO3UID.dll");
#else
	g_ModuleUI.Init("SO3UI.dll");
#endif

	pfnCreateSO3UI = (fnCreateSO3UI)GetProcAddress(g_ModuleUI.GetDllHandle(), FN_CREATE_SO3UI);
	KGLOG_PROCESS_ERROR(pfnCreateSO3UI);

	g_pUI = pfnCreateSO3UI();
	KGLOG_PROCESS_ERROR(g_pUI);

#if 0
	Param.hInst = m_hInst;
	Param.hWnd = NULL;
	Param.fnAttribute = AttributeStringToID;
	Param.fnRequire = RequireStringToID;
	Param.p3DEngineManager = g_GetEngineIEInterface();
	Param.p3DModelManager = g_GetEngineIEInterface()->Get3DModelTable();
	Param.p3DUI = g_GetEngineIEInterface()->Get3DUI();
	Param.pRepresent = g_pRepresent->GetRepresentHandelr();
	g_pUI->Init(&Param);
#endif

	return TRUE;
Exit0:
	return FALSE;
}

void UnInitUI()
{
	if (g_pUI)
	{
		g_pUI->Exit();
		g_pUI = NULL;
	}
	g_ModuleUI.UnInit();
}

BOOL InitRepresent()
{
	BOOL bRetCode = FALSE;
	fnCreateSO3Represent pfnCreateSO3Represent = NULL;

	UnInitRepresent();

#ifdef _DEBUG
	g_ModuleRepresent.Init("SO3RepresentD.dll");
#else
	g_ModuleRepresent.Init("SO3Represent.dll");
#endif

	pfnCreateSO3Represent = (fnCreateSO3Represent)GetProcAddress(g_ModuleRepresent.GetDllHandle(), FN_CREATE_SO3REPRESENT);
	KGLOG_PROCESS_ERROR(pfnCreateSO3Represent);

	g_pRepresent = pfnCreateSO3Represent();
	KGLOG_PROCESS_ERROR(g_pRepresent);

	return TRUE;
Exit0:
	return FALSE;
}

void UnInitRepresent()
{
	SAFE_RELEASE(g_pRepresent);
	g_ModuleRepresent.UnInit();
}

void InitKeyState(IEKG3DSceneEditorBase* pBase, HWND hWnd, CInputProxy* pInputProxy)
{
	_ASSERTE(pInputProxy);

// 	static PROXYKEYMAP KEYMAP[] =
// 	{	//           C  S  A  L  M  R  ?    Trigger     ID  Prev Ig Exclusive
// 		{'X'	   , 0, 0, 0, 1, 0, 0, 0, WM_LBUTTONDOWN , 16,  0, 0, 1},	//压感调整
// 		{'Z'	   , 0, 1, 0, 1, 0, 0, 0, WM_LBUTTONDOWN , 17,  0, 0, 1},	//内外圈同时调整
// 		{'Z'	   , 0, 0, 0, 1, 0, 0, 0, WM_LBUTTONDOWN , 18,  0, 0, 1},	//外圈调整
// 
// 		{VK_MENU   , 0, 0, 1, 0, 1, 0, 0, WM_MOUSEMOVE,   1,  0, 0, 0},	//alt+MBT, 旋转摄像机镜头
// 		{VK_CONTROL, 1, 0, 0, 0, 1, 0, 0, WM_MOUSEMOVE,   2,  0, 0, 0},	//ctrl+MBT, 快速缩放视角
// 		{VK_MBUTTON, 0, 0, 0, 0, 1, 0, 0, WM_MOUSEMOVE,   3,  0, 0, 0},	//MBT, PAN视图
// 		{VK_MBUTTON, 0, 1, 0, 0, 1, 0, 0, WM_MOUSEMOVE,   4,  0, 0, 0},	//SHIFT+MBT, 以选定点为中心旋转视图
// 		{VK_LBUTTON, 1, 0, 0, 1, 0, 0, 0, WM_LBUTTONDOWN, 5,  0, 0, 0},	//CTRL＋LBT，反向当前操作
// 
// 		{'W'	   , 0, 0, 0, 0, 0, 0, 0, WM_KEYDOWN    , 6,  0, 2, 0},	//前进 忽略鼠标键
// 		{VK_LBUTTON, 0, 0, 0, 1, 0, 1, 0, WM_MOUSEMOVE  , 6,  0, 0, 0},	//按住左右键不放，就直走
// 		{'S'	   , 0, 0, 0, 0, 0, 0, 0, WM_KEYDOWN    , 7,  0, 2, 0},	//后退 忽略鼠标键
// 		{'A'	   , 0, 0, 0, 1, 0, 0, 0, WM_KEYDOWN    , 8,  0, 1, 0},	//左转 
// 		{'D'	   , 0, 0, 0, 1, 0, 0, 0, WM_KEYDOWN    , 9,  0, 1, 0},	//右转 
// 		{'A'	   , 0, 0, 0, 0, 1, 0, 0, WM_KEYDOWN    , 8,  0, 1, 0},	//左转 
// 		{'D'	   , 0, 0, 0, 0, 1, 0, 0, WM_KEYDOWN    , 9,  0, 1, 0},	//右转 
// 		{'A'	   , 0, 0, 0, 0, 0, 1, 0, WM_KEYDOWN    ,10,  0, 1, 0},	//左移 忽略键盘
// 		{'D'	   , 0, 0, 0, 0, 0, 1, 0, WM_KEYDOWN    ,11,  0, 1, 0},	//右移 忽略键盘
// 
// 		{VK_RBUTTON, 0, 0, 0, 0, 0, 1, 0, WM_MOUSEMOVE , 12,  0, 0, 0},	//按住右键不放，就转动摄像机
// 		{' '	   , 0, 0, 0, 0, 0, 0, 0, WM_KEYDOWN   , 13,  0, 3, 0},	//跳，忽略所有
// 		{'A'	   , 0, 0, 0, 0, 0, 0, 0, WM_KEYDOWN   , 14,  0, 1, 0},	//左转，同时转动摄像机
// 		{'D'	   , 0, 0, 0, 0, 0, 0, 0, WM_KEYDOWN   , 15,  0, 1, 0},	//右转
// 
// 		{VK_LBUTTON, 0, 0, 0, 1, 0, 0, 0, WM_LBUTTONDOWN, 19,  0, 0, 0},	//左键按下，可以作选择、改变地形等很多动作
// 		{VK_SHIFT,   0, 1, 0, 1, 0, 0, 0, WM_LBUTTONDOWN, 20,  0, 0, 0},	//平滑
// 
// 		{VK_ADD,	 0, 0, 0, 0, 0, 0, 0, WM_LBUTTONDOWN, 21,  0, 3, 0},	//加速
// 		{VK_SUBTRACT,0, 0, 0, 0, 0, 0, 0, WM_KEYDOWN,     22,  0, 3, 0},	//减速
// 
// 		{VK_LBUTTON, 1, 0, 0, 1, 0, 0, 0, WM_LBUTTONDOWN, 23,  0, 0, 0},	//在当前选择范围中，再增加选择内容
// 		{VK_LBUTTON, 0, 0, 1, 1, 0, 0, 0, WM_LBUTTONDOWN, 24,  0, 0, 0},	//在当前选择范围中，再减少选择内容
// 
// 		{0		   , 0, 0, 0, 0, 0, 0, 0, 0             , 0,  0, 0, 0},	//结束
// 		//           C  S  A  L  M  R  ?    Trigger     ID  Prev Ig Exclusive
// 	};

	static PROXYKEYMAP KEYMAP[] =
	{	//           C  S  A  L  M  R  ?  Trigger		  ID     Prev Ig   Exclusive
		{VK_RBUTTON, 0, 0, 0, 0, 0, 1, 0, WM_MOUSEMOVE,   1,     0,   0,   0},	// 鼠标右键, 旋转摄像机镜头
		//{VK_LBUTTON, 0, 0, 0, 1, 0, 0, 0, WM_MOUSEMOVE,   1001,  0,   0,   0},	// 鼠标左键, 快速缩放视角
		{VK_MBUTTON, 0, 0, 0, 0, 1, 0, 0, WM_MOUSEMOVE,   3,     0,   0,   0},	// 鼠标中键, PAN 视图
		{0		   , 0, 0, 0, 0, 0, 0, 0, 0             , 0,     0,   0},	    // 结束
	};

	pInputProxy->Init(pBase, CWnd::FromHandle(hWnd), KEYMAP);
}


