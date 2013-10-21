#include "StdAfx.h"
#include "Functions.h"
#include "IEEditor.h"
#include "./3DEngine/InputProxy.h"

void InitKeyState(IEKG3DSceneEditorBase* pBase, HWND hWnd, CInputProxy* pInputProxy)
{
	_ASSERTE(pInputProxy);

	static PROXYKEYMAP KEYMAP[] =
	{	//           C  S  A  L  M  R  ?    Trigger     ID  Prev Ig Exclusive
		{'X'	   , 0, 0, 0, 1, 0, 0, 0, WM_LBUTTONDOWN , 16,  0, 0, 1},	//压感调整
		{'Z'	   , 0, 1, 0, 1, 0, 0, 0, WM_LBUTTONDOWN , 17,  0, 0, 1},	//内外圈同时调整
		{'Z'	   , 0, 0, 0, 1, 0, 0, 0, WM_LBUTTONDOWN , 18,  0, 0, 1},	//外圈调整

		{VK_MENU   , 0, 0, 1, 0, 1, 0, 0, WM_MOUSEMOVE,   1,  0, 0, 0},	//alt+MBT, 旋转摄像机镜头
		{VK_CONTROL, 1, 0, 0, 0, 1, 0, 0, WM_MOUSEMOVE,   2,  0, 0, 0},	//ctrl+MBT, 快速缩放视角
		{VK_MBUTTON, 0, 0, 0, 0, 1, 0, 0, WM_MOUSEMOVE,   3,  0, 0, 0},	//MBT, PAN视图
		{VK_MBUTTON, 0, 1, 0, 0, 1, 0, 0, WM_MOUSEMOVE,   4,  0, 0, 0},	//SHIFT+MBT, 以选定点为中心旋转视图
		{VK_LBUTTON, 1, 0, 0, 1, 0, 0, 0, WM_LBUTTONDOWN, 5,  0, 0, 0},	//CTRL＋LBT，反向当前操作

		{'W'	   , 0, 0, 0, 0, 0, 0, 0, WM_KEYDOWN    , 6,  0, 2, 0},	//前进 忽略鼠标键
		{VK_LBUTTON, 0, 0, 0, 1, 0, 1, 0, WM_MOUSEMOVE  , 6,  0, 0, 0},	//按住左右键不放，就直走
		{'S'	   , 0, 0, 0, 0, 0, 0, 0, WM_KEYDOWN    , 7,  0, 2, 0},	//后退 忽略鼠标键
		{'A'	   , 0, 0, 0, 1, 0, 0, 0, WM_KEYDOWN    , 8,  0, 1, 0},	//左转 
		{'D'	   , 0, 0, 0, 1, 0, 0, 0, WM_KEYDOWN    , 9,  0, 1, 0},	//右转 
		{'A'	   , 0, 0, 0, 0, 1, 0, 0, WM_KEYDOWN    , 8,  0, 1, 0},	//左转 
		{'D'	   , 0, 0, 0, 0, 1, 0, 0, WM_KEYDOWN    , 9,  0, 1, 0},	//右转 
		{'A'	   , 0, 0, 0, 0, 0, 1, 0, WM_KEYDOWN    ,10,  0, 1, 0},	//左移 忽略键盘
		{'D'	   , 0, 0, 0, 0, 0, 1, 0, WM_KEYDOWN    ,11,  0, 1, 0},	//右移 忽略键盘

		{VK_RBUTTON, 0, 0, 0, 0, 0, 1, 0, WM_MOUSEMOVE , 12,  0, 0, 0},	//按住右键不放，就转动摄像机
		{' '	   , 0, 0, 0, 0, 0, 0, 0, WM_KEYDOWN   , 13,  0, 3, 0},	//跳，忽略所有
		{'A'	   , 0, 0, 0, 0, 0, 0, 0, WM_KEYDOWN   , 14,  0, 1, 0},	//左转，同时转动摄像机
		{'D'	   , 0, 0, 0, 0, 0, 0, 0, WM_KEYDOWN   , 15,  0, 1, 0},	//右转

		{VK_LBUTTON, 0, 0, 0, 1, 0, 0, 0, WM_LBUTTONDOWN, 19,  0, 0, 0},	//左键按下，可以作选择、改变地形等很多动作
		{VK_SHIFT,   0, 1, 0, 1, 0, 0, 0, WM_LBUTTONDOWN, 20,  0, 0, 0},	//平滑

		{VK_ADD,	 0, 0, 0, 0, 0, 0, 0, WM_LBUTTONDOWN, 21,  0, 3, 0},	//加速
		{VK_SUBTRACT,0, 0, 0, 0, 0, 0, 0, WM_KEYDOWN,     22,  0, 3, 0},	//减速

		{VK_LBUTTON, 1, 0, 0, 1, 0, 0, 0, WM_LBUTTONDOWN, 23,  0, 0, 0},	//在当前选择范围中，再增加选择内容
		{VK_LBUTTON, 0, 0, 1, 1, 0, 0, 0, WM_LBUTTONDOWN, 24,  0, 0, 0},	//在当前选择范围中，再减少选择内容

		{0		   , 0, 0, 0, 0, 0, 0, 0, 0             , 0,  0, 0, 0},	//结束
		//           C  S  A  L  M  R  ?    Trigger     ID  Prev Ig Exclusive
	};

	pInputProxy->Init(pBase, CWnd::FromHandle(hWnd), KEYMAP);
}