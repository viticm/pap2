/*****************************************************************************************
//	窗口消息
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-7-9
------------------------------------------------------------------------------------------
*****************************************************************************************/
#pragma once

#define	WND_GAMESPACE			0xFFFFFFFF

#ifndef WM_MOUSEHOVER
	#define	WM_MOUSEHOVER	0x02A1
#endif

#define	WND_M_BASE				0x500	//WM_USER + 0x100

//------------------------------------------------
//	窗口一般消息
#define WND_M_OTHER_WORK_RESULT	WND_M_BASE + 1				//界面操作结果返回
//uParam = 操作需求界面执行定义的操作参数，比如需求界面用此参数来区分自己可以发出的多个需求
//nParam = 取值为WND_OPER_RESULT_0或者WND_OPER_RESULT(n), n指代任意一个整数值，其含义依据具体的界面应用自己确认
#define	WND_M_MENUITEM_SELECTED	WND_M_BASE + 2
#define	WND_M_MENUITEMRIGHT_SELECTED	WND_M_BASE + 3
//(KWndWindow*)uParam = 调用菜单的的窗口
//	(short)(nParam & 0xffff) = 所选的菜单项的索引，如果选择取消则为-1
//	(short)((nParam >> 16) & 0xffff) = 操作需求界面执行定义的操作参数，比如需求界面用此参数来区分自己可以发出的多个需求
/*
#define	WND_M_CREATE			WND_M_BASE + 1
#define	WND_M_DESTROY			WND_M_BASE + 2
#define	WND_M_CLOSE				WND_M_BASE + 3*/
#define WND_M_MOUSE_ENTER		WND_M_BASE + 6				//鼠标指针进入窗口范围
#define	WND_M_MOUSE_LEAVE		WND_M_BASE + 7				//鼠标指针离开窗口范围

#define	WND_N_MOUSE_ENTER_CHILD	WND_M_BASE + 8				//鼠标指针进入某子窗口范围
//(KWndWindow*)uParam = 鼠标指针进入的子窗口的指针

#define	WND_N_MOUSE_LEAVE_CHILD	WND_M_BASE + 9				//鼠标指针离开某子窗口范围
//(KWndWindow*)uParam = 鼠标指针离开的子窗口的指针

#define	WND_M_SET_FOCUS			WND_M_BASE + 10				//设置输入焦点
//(KWndWindow*)uParam = 原来拥有输入焦点的窗口的指针

#define	WND_M_KILL_FOCUS		WND_M_BASE + 11				//失去输入焦点
//(KWndWindow*)uParam = 将要拥有输入焦点的窗口的指针

#define	WND_M_CAPTURECHANGED	WND_M_BASE + 12				//失去对鼠标指针消息跟踪

#define	WND_M_HOTKEY			WND_M_BASE + 13				//热键
//uParam = 热键ID

#define WND_M_POPUPMENU			WND_M_BASE + 14				//右键菜单弹出
//(KWndWindow*)uParam = 被右键点击的子窗口的指针
//	(short)(nParam & 0xffff) = x屏幕坐标
//	(short)((nParam >> 16) & 0xffff) = y屏幕坐标


#define WND_N_ITEM_PICKDROP		WND_M_BASE + 17				//捡起/放下物品
//(ITEM_PICKDROP_PLACE*)uParam = 欲捡起物品的所在位置，如无物品要被捡起，则uParam = 0
//(ITEM_PICKDROP_PLACE*)nParam = 欲放下物品的放下位置，如无东西要被放下，则nParam = 0

#define WND_N_LEFT_CLICK_ITEM	WND_M_BASE + 18				//左键点按物品对象
//(const KUiDraggedObject*)uParam = 被点按的物品的信息
//(KWndWindow*)nParam = 物品所在的窗口的指针
#define WND_N_RIGHT_CLICK_ITEM	WND_M_BASE + 19				//右键点按物品对象
//参数含义同WND_N_LEFT_CLICK_ITEM

#define WND_N_CHILD_MOVE		WND_M_BASE + 20				//子窗口被拖动了
//(KWndWindow*)uParam = 被拖动了的子窗口的指针
//当nParam != 0 时
//	(short)(nParam & 0xffff) = 横向移动的距离
//	(short)((nParam >> 16) & 0xffff) = 纵向移动的距离
//当nParam == 0 时，表示窗口的拖动操作结束了

//------------------------------------------------
//	按键消息
#define WND_M_BUTTON_BASE			WND_M_BASE + 100
#define WND_N_BUTTON_CLICK			WND_M_BUTTON_BASE + 1	//点击按钮操作
//(KWndWindow*)uParam = 被点击的按钮窗口的指针
//nParam = 对于CheckBox类型的按钮，此参数表示按钮被后是否处于按下状态

#define	WND_N_BUTTON_DOWN			WND_M_BUTTON_BASE + 2	//按钮被按下操作
//(KWndWindow*)uParam = 被点击的按钮窗口的指针

#define	WND_N_BUTTON_MR_DOWN		WND_M_BUTTON_BASE + 3	//鼠标右按键按下
//(KWndWindow*)uParam = 被点击的按钮窗口的指针

#define	WND_N_BUTTON_HOLD			WND_M_BUTTON_BASE + 4	//按钮被持续按住不放
//(KWndWindow*)uParam = 被按住的按钮窗口的指针

#define WND_N_BUTTON_OVER			WND_M_BUTTON_BASE + 5	//鼠标移到按钮上
//(KWndWindow*)uParam = 按钮窗口的指针

#define WND_N_BUTTON_DB_CLICK		WND_M_BUTTON_BASE + 6	//双击按钮操作
//(KWndWindow*)uParam = 被点击的按钮窗口的指针
//对于CheckBox类型的按钮不会生成此消息

//------------------------------------------------
// Scroll Bar消息
#define WND_M_SCORLLBAR_BASE		WND_M_BASE + 200
#define WND_N_SCORLLBAR_POS_CHANGED	WND_M_SCORLLBAR_BASE + 1//滚动条滑块位置发生变化
//(KWndWindow*)uParam = 被点击的滚动条窗口的指针
//nParam = 滚动条滑块位置关联的数值

//------------------------------------------------
// 文本编辑框消息
#define	WND_M_EDIT_BASE				WND_M_BASE + 300
#define	WND_N_EDIT_CHANGE			WND_M_EDIT_BASE + 1		//文本编辑窗口内的内容发生了变化
//(KWndWindow*)uParam = 文本编辑框窗口的指针

#define	WND_N_EDIT_SPECIAL_KEY_DOWN	WND_M_EDIT_BASE + 2		//文本编辑窗口获得特定的按键操作
//(KWndWindow*)uParam = 文本编辑框窗口的指针
//nParam = 按键的visual key code
//特定按键包括 回车键、Esc键、制表符tab键、向上键头键、向下键头键、前翻页键，后翻页键

//------------------------------------------------
// 列表框窗口消息
#define	WND_M_LIST_BASE				WND_M_BASE + 400
#define	WND_N_LIST_ITEM_SEL			WND_M_LIST_BASE + 1		//列表框选中项发生变换
//(KWndWindow*)uParam = 列表框窗口的指针
//nParam = 新选择的项的索引
#define	WND_N_LIST_ITEM_ACTIVE		WND_M_LIST_BASE + 2		//列表框选中项被激活（点左键或按enter）
//参数含义同WND_N_LIST_ITEM_SEL
#define	WND_N_LIST_ITEM_D_CLICK		WND_M_LIST_BASE + 3
//(KWndWindow*)uParam = 列表框窗口的指针
//nParam = 双击的项的索引
#define WND_N_LIST_ITEM_R_CLICK		WND_M_LIST_BASE + 4
//(KWndWindow*)uParam = 列表框窗口的指针
//nParam = 右键点击的项的索引
#define	WND_N_LIST_ITEM_HIGHLIGHT	WND_M_LIST_BASE + 5		//列表框选鼠标指针指向的项发生变换
//(KWndWindow*)uParam = 列表框窗口的指针
//nParam = 列表框选鼠标指针指向的项的索引

//====界面操作结果返回值定义====
#define WND_OPER_RESULT_0		0		//界面操作返回的结果0
#define WND_OPER_RESULT(n)		(n)		//界面操作返回的结果

// flying add the macro so that my compile work can continue.
// in m$ header file, there's this condition: 
// #if (_WIN32_WINNT >= 0x0400) || (_WIN32_WINDOWS > 0x0400)
#if !defined WM_MOUSEWHEEL
#define WM_MOUSEWHEEL                   0x020A
#endif
#if !defined WHEEL_DELTA
#define WHEEL_DELTA                     120
#endif

//=========================
//	描述被拖动对象的结构
//=========================
struct KUiDraggedObject
{
	unsigned int	uGenre;
	unsigned int	uId;
	int				DataX;
	int				DataY;
	int				DataW;
	int				DataH;
};

//=========================
//	描述对象位置的结构
//=========================
struct ITEM_PICKDROP_PLACE
{
	class KWndWindow*	pWnd;
	int					h;
	int					v;
};
