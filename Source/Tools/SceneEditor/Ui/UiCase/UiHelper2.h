/* 
 * File:     KUiHelper2.h
 * Desc:     详细帮助窗口
 * Author:   flying
 * Creation: 2003/7/16
 */
//////////////////////////////////////////////////////////////////////////
//                     帮助界面文件名字的历史来历
// 为什么详细帮助界面叫做Helper2而由详细帮助界面激活的图片帮助界面叫做
// Helper呢？这是有原因的。什么原因呢？最初的时候传说需要一个类似Diablo
// 按H弹出的关于按钮的图片帮助。这也是最初所理解的帮助系统，而且在那时
// 实现，名字自然叫做Helper了。之后需要一个索引类型的帮助，而且把原先所
// 要求的帮助系统转移到那里去激活，于是在开发时间上在图片帮助之后的索引
// 帮助就叫做Helper2了。口合  口合
//////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
#pragma once

#if !defined _UIHELPER2
#define _UIHELPER2

#include "../Elem/WndShowAnimate.h"
#include "../Elem/WndButton.h"
#include "../Elem/WndScrollBar.h"
#include "../Elem/WndList.h"
#include "../Elem/WndMessageListBox.h"

#define		MAX_INDEX_NUM		512

class KUiHelper2 : protected KWndShowAnimate
{
public:
	//打开窗口，返回唯一的一个类对象实例
	static KUiHelper2*	OpenWindow(bool bShow);
	//如果窗口正被显示，则返回实例指针
	static KUiHelper2*  GetIfVisible();
	//关闭窗口，同时可以选则是否删除对象实例
	static void			 CloseWindow(bool bDestory);
	//载入界面方案
	static void			 LoadScheme(const char* pScheme);
private:
	KUiHelper2();
	~KUiHelper2();
	void	Initialize();
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void	LoadIndexList(const char* pScheme);
	void	Clear();
	void	UpdateData(int nIndex);
private:
	static KUiHelper2*	m_pSelf;
	KWndButton				m_BtnClose;
	KWndButton				m_BtnPic;
	KWndButton              m_BtnKeyboard;
	KWndButton              m_BtnWuxing;
	KWndList				m_IndexList;
	KWndScrollBar			m_IndexScroll;
	KWndMessageListBox		m_MessageList;
	KWndScrollBar			m_MessageScroll;
	int						m_nIndexCount;
	
	typedef struct _IndexData
	{
		char szName[128];
		char szIni[128];
		char szIniSection[128];
	} IndexData;
	IndexData m_IndexData[MAX_INDEX_NUM];
};

#endif