/*****************************************************************************************
//	界面--聊天选项/状态设置
//	Copyright : Kingsoft 2003
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2003-3-5
------------------------------------------------------------------------------------------
*****************************************************************************************/
#pragma once
#include "../Elem/WndImage.h"
#include "../Elem/WndButton.h"
#include "../Elem/WndEdit.h"
#include "../Elem/WndMessageListBox.h"
#include "../Elem/WndScrollBar.h"

class KUiChatStatus : protected KWndImage
{
public:
	//----界面面板统一的接口函数----
	static KUiChatStatus*	OpenWindow();		//打开窗口，返回唯一的一个类对象实例
	static KUiChatStatus*	GetIfVisible();		//如果窗口正被显示，则返回实例指针
	static void				CloseWindow();		//关闭窗口，同时可以选则是否删除对象实例
	static void				LoadScheme(const char* pScheme);//载入界面方案	
private:
	KUiChatStatus() {}
	~KUiChatStatus() {}
	void	Initialize();							//初始化
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);//窗口函数
	void	OnAddMsg();
	void	OnSetDefMsg();
	void	OnSelMsg(int nIndex);
private:
	static KUiChatStatus*	m_pSelf;
	KWndButton			m_OnLineBtn, m_LeaveBtn, m_BusyBtn;
	KWndButton			m_AutoReplyBtn,	m_HideBtn;
	KWndButton			m_AddMsg, m_RemoveMsg, m_SetDefaultMsg;
	KWndEdit512			m_ReplyMsgEdit;
	KWndMessageListBox	m_MsgList;
	KWndScrollBar		m_MsgScroll;
	KWndButton			m_CloseBtn;
};