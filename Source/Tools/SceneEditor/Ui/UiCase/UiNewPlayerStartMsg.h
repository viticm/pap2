/*****************************************************************************************
//	界面--新手进入的提示界面
//	Copyright : Kingsoft 2003
//	Author	:   Freeway Chen
//	CreateTime:	2003-7-8
------------------------------------------------------------------------------------------
*****************************************************************************************/
#pragma once
#include "../Elem/WndList.h"
#include "../Elem/WndScrollBar.h"
#include "../Elem/WndMessageListBox.h"
#include "../Elem/WndShowAnimate.h"

class KUiNewPlayerStartMsg : protected KWndShowAnimate
{
public:
	static KUiNewPlayerStartMsg* OpenWindow();		//打开窗口，返回唯一的一个类对象实例
	static void	CloseWindow(bool bDestroy);         //关闭窗口

private:
	KUiNewPlayerStartMsg();
	~KUiNewPlayerStartMsg();

	void	Initialize();						//初始化
	void	LoadScheme(const char* pScheme);	//载入界面方案
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);	//窗口函数
	void	OnOk();
	int		OnKeyDown(unsigned int uKey);
	void	OnClickButton(KWndButton* pBtn);
private:
	void	LoadList();
	static  KUiNewPlayerStartMsg* m_pSelf;
private:
	KScrollMessageListBox	m_List;
	KScrollMessageListBox	m_AdvanceList;
	KWndButton				m_NewerBtn;
	KWndButton				m_AdvanceBtn;
	KWndButton		        m_Ok;

};
