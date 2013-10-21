//////////////////////////////////////////////////////////////////////////////////////
//	文件名			:	UiKillerOwnTask.h
//	创建人			:	王西贝
//	创建时间		:	2003-12-26 10:48:55
//	文件说明		:	查看自己创建或者领取的通缉任务--对话框
//////////////////////////////////////////////////////////////////////////////////////

#ifndef __UIKILLEROWNTASK_H__
#define __UIKILLEROWNTASK_H__

#include "../elem/WndButton.h"
#include "../elem/WndText.h"
#include "../elem/WndObjContainer.h"
#include "../Elem/WndShowAnimate.h"
#include "../Elem/WndList.h"
#include "../Elem/WndScrollBar.h"

#include "GameDataDef.h"
#include <vector>
using namespace std;

class KUiKillerOwnTask : public KWndShowAnimate
{
public:
	static KUiKillerOwnTask* OpenWindow();				//打开窗口，返回唯一的一个类对象实例
	static KUiKillerOwnTask* GetIfVisible();			//如果窗口正被显示，则返回实例指针
	static void		CloseWindow(bool bDestroy);			//关闭窗口，同时可以选则是否删除对象实例
	static void		LoadScheme(const char* pScheme);	//载入界面方案
	void			ReceiveTask(KILLER_TASK* pTask);	//物品变化更新

private:
	KUiKillerOwnTask()
	{
		m_bRecvOver = FALSE;
		m_pTasks = NULL;
		m_nCurTaskNum = 0;
		m_nCurTaskNumMax = 0;
		m_bSortByReward = TRUE;
		m_bSortIncrease = FALSE;
	}
	virtual ~KUiKillerOwnTask()
	{ 
		ClearData();
	}
	void	Initialize();								//初始化
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);//窗口函数
	void	ClearData();
	void	ResortList();
	void	PushInSortedList(KILLER_TASK* pTask);
private:
	static	KUiKillerOwnTask*	m_pSelf;
	typedef struct {
		KILLER_TASK task;
		char szListItemText[64];
	}KILLER_TASK_ITEM;
	KILLER_TASK_ITEM*			m_pTasks;
	int							m_nCurTaskNum;
	int							m_nCurTaskNumMax;
	BOOL						m_bRecvOver;			//是否接受完了任务
	BOOL						m_bSortByReward;		//是否按赏金排序,否则按人名排序
	BOOL						m_bSortIncrease;		//是否升序排列
private:
	KWndButton			m_SortByRewardBtn;
	KWndButton			m_SortByTargetBtn;
	KWndList			m_TaskList;
	KWndScrollBar		m_TaskListCtrl;
	KWndButton			m_CancelTaskBtn;
	KWndButton			m_LeaveBtn;
};


#endif //__UIKILLEROWNTASK_H__
