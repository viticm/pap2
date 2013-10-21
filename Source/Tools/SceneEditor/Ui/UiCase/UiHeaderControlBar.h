/*****************************************************************************************
//	界面--屏幕顶控制操作条
//	Copyright : Kingsoft 2003
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2003-4-22
------------------------------------------------------------------------------------------
*****************************************************************************************/
#pragma once
#include "../Elem/WndToolBar.h"
#include "../Elem/WndButton.h"

class KUiHeaderControlBar : public KWndToolBar
{
public:
	//----界面面板统一的接口函数----
	static KUiHeaderControlBar* OpenWindow();	//打开窗口，返回唯一的一个类对象实例
	static void				CloseWindow();		//关闭窗口
	static void				LoadScheme(const char* pScheme);//载入界面方案
	static void				DefaultScheme(const char* pScheme);//重新初始化界面
	static KUiHeaderControlBar* GetSelf()	{return m_pSelf;}
private:
	~KUiHeaderControlBar() {}
	void	Initialize();							//初始化
	void	Breathe();
private:
	static KUiHeaderControlBar*	m_pSelf;
};