/*****************************************************************************************
//	窗口消息
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-7-9
------------------------------------------------------------------------------------------
*****************************************************************************************/
#ifndef UI_ELEM_K_WND_MESSAGE_H
#define UI_ELEM_K_WND_MESSAGE_H

#define	WND_M_BASE				0x500	//WM_USER + 0x100

//------------------------------------------------
//	窗口一般消息
#define WND_M_MOUSE_ENTER		WND_M_BASE + 6				//鼠标指针进入窗口范围
#define	WND_M_MOUSE_LEAVE		WND_M_BASE + 7				//鼠标指针离开窗口范围
#define	WND_M_REFRESH_TIP		WND_M_BASE + 8				//鼠标指针下的窗口跟新

#define	WND_M_SET_FOCUS			WND_M_BASE + 10				//设置输入焦点
//(KWndWindow*)wParam = 原来拥有输入焦点的窗口的指针

#define	WND_M_KILL_FOCUS		WND_M_BASE + 11				//失去输入焦点
//(KWndWindow*)wParam = 将要拥有输入焦点的窗口的指针

#define	WND_M_CAPTURECHANGED	WND_M_BASE + 12				//失去对鼠标指针消息跟踪

#define	WND_M_CHILD_CAPTURECHANGED	WND_M_BASE + 13			//制窗口失去对鼠标指针消息跟踪

//------------------------------------------------
//	按键消息
#define WND_M_BUTTON_BASE			WND_M_BASE + 100
#define WND_N_BUTTON_CLICK			WND_M_BUTTON_BASE + 1	//点击按钮操作
//(KWndWindow*)wParam = 被点击的按钮窗口的指针
//nParam = 对于CheckBox类型的按钮，此参数表示按钮被后是否处于按下状态

#define	WND_N_BUTTON_DOWN			WND_M_BUTTON_BASE + 2	//按钮被按下操作
//(KWndWindow*)wParam = 被点击的按钮窗口的指针

#define	WND_N_BUTTON_HOLD			WND_M_BUTTON_BASE + 4	//按钮被持续按住不放
//(KWndWindow*)wParam = 被按住的按钮窗口的指针

//------------------------------------------------
// Scroll Bar消息
#define WND_M_SCORLLBAR_BASE		WND_M_BASE + 200
#define WND_N_SCORLLBAR_POS_CHANGED	WND_M_SCORLLBAR_BASE + 1//滚动条滑块位置发生变化
//(KWndWindow*)wParam = 被点击的滚动条窗口的指针
//nParam = 滚动条滑块位置关联的数值

//------------------------------------------------
// 文本编辑框消息
#define	WND_M_EDIT_BASE				WND_M_BASE + 300

#define	WND_N_EDIT_SPECIAL_KEY_DOWN	WND_M_EDIT_BASE + 2		//文本编辑窗口获得特定的按键操作
//(KWndWindow*)wParam = 文本编辑框窗口的指针
//nParam = 按键的visual key code
//特定按键包括 回车键、Esc键、制表符tab键、向上键头键、向下键头键、前翻页键，后翻页键

#endif // UI_ELEM_K_WND_MESSAGE_H
