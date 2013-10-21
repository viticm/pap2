/*****************************************************************************************
//	界面--选择附近的某个玩家
//	Copyright : Kingsoft 2003
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2003-2-26
------------------------------------------------------------------------------------------
*****************************************************************************************/
#pragma once

#include "../Elem/WndShowAnimate.h"
#include "../Elem/WndButton.h"
#include "../Elem/WndPureTextBtn.h"
#include "../Elem/WndList.h"
#include "../Elem/WndScrollBar.h"
#include "../Elem/WndEdit.h"

struct KPlayerInterAction
{
	char	szActionName[32];	//行为名称
	int		nId;				//行为id
};

struct KUiPlayerItem;

class KUiSelPlayerNearby : protected KWndShowAnimate
{
public:
	static KUiSelPlayerNearby*	OpenWindow();				//打开窗口，返回唯一的一个类对象实例
	static int					DoesHaveVisibleWnd();
	static void					CloseWindow(bool bDestroy);				//关闭窗口，同时可以选则是否删除对象实例
	static void					LoadScheme(const char* pScheme);		//载入界面方案
private:
	KUiSelPlayerNearby();
	~KUiSelPlayerNearby() {}
	void	Initialize();					//初始化
	void	LoadScheme(class KIniFile* pIni);//载入界面方案
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);//窗口函数
	void	OnClickXButton(int nAction);
	void	UpdateData();
	void	Clear();
	void	LoadActionList(KIniFile* pSetting);
private:
	static KUiSelPlayerNearby*	m_pSelf;
	KUiPlayerItem*			m_pDataList;
	int						m_nPlayerCount;
	KWndList				m_PlayerList;
	KWndScrollBar			m_ListScroll;

#define	MAX_NUM_BUTTON		9
	KWndPureTextBtn			m_ActionBtns[MAX_NUM_BUTTON];
	KPlayerInterAction		m_ActionList[MAX_NUM_BUTTON];
	int						m_nActionCount;
		
	KWndButton				m_CancelBtn, m_RefreshBtn;
	KWndEdit32	m_InputEdit;
};