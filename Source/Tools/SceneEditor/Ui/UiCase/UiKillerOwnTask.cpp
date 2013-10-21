//////////////////////////////////////////////////////////////////////////////////////
//	文件名			:	UiKillerOwnTask.cpp
//	创建人			:	王西贝
//	创建时间		:	2003-12-26 10:56:35
//	文件说明		:	查看自己创建或者领取的通缉任务--对话框--的实现
//////////////////////////////////////////////////////////////////////////////////////

#include "KWin32.h"
#include "UiKillerOwnTask.h"

#include "KIniFile.h"
#include "../Elem/AutoLocateWnd.h"
#include "../Elem/WndMessage.h"
#include "../Elem/Wnds.h"
#include "../UiBase.h"
#include "../UiSoundSetting.h"
#include "../../../Core/Src/CoreShell.h"
#include "../../../Core/Src/GameDataDef.h"

#include <vector>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

extern iCoreShell*		g_pCoreShell;

#define SCHEME_INI_OWNTASK		"通缉查看任务编辑框.ini"

void WPrintMoney(char* szBuf, int nMoney);

void CoreDataChangedCallback(unsigned int uDataId, unsigned int uParam, int nParam);

KUiKillerOwnTask* KUiKillerOwnTask::m_pSelf = NULL;

KUiKillerOwnTask* KUiKillerOwnTask::OpenWindow()
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiKillerOwnTask;
		if (m_pSelf)
			m_pSelf->Initialize();
	}

	if (m_pSelf)
	{
		UiSoundPlay(UI_SI_WND_OPENCLOSE);
		m_pSelf->ClearData();
		m_pSelf->Show();
		m_pSelf->BringToTop();
	}
	return m_pSelf;
}

void KUiKillerOwnTask::CloseWindow(bool bDestroy)
{
	if (m_pSelf)
	{
		m_pSelf->Hide();
		if (bDestroy)
		{
			m_pSelf->Destroy();
			m_pSelf = NULL;
		}
	}
}

KUiKillerOwnTask* KUiKillerOwnTask::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	else
		return NULL;
}

void KUiKillerOwnTask::LoadScheme(const char* pScheme)
{
	if (m_pSelf)
	{
		char		Buff[128];
		KIniFile	Ini;
		sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI_OWNTASK);
		if (Ini.Load(Buff))
		{
			m_pSelf->Init(&Ini, "Main");
			m_pSelf->m_SortByRewardBtn.Init(&Ini, "RewardBtn");
			m_pSelf->m_SortByTargetBtn.Init(&Ini, "TargetBtn");
			m_pSelf->m_TaskList.Init(&Ini, "TaskList");
			m_pSelf->m_TaskListCtrl.Init(&Ini, "TaskListScroll");
			m_pSelf->m_CancelTaskBtn.Init(&Ini, "CancelTaskBtn");
			m_pSelf->m_LeaveBtn.Init(&Ini, "LeaveBtn");
		}
	}
	return;
}

void KUiKillerOwnTask::Initialize()
{
	AddChild(&m_SortByRewardBtn);
	AddChild(&m_SortByTargetBtn);
	AddChild(&m_TaskList);
	AddChild(&m_TaskListCtrl);
	AddChild(&m_CancelTaskBtn);
	AddChild(&m_LeaveBtn);

	m_TaskList.SetScrollbar(&m_TaskListCtrl);
	m_TaskListCtrl.Enable(false);

	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);

	Wnd_AddWindow(this);
	return;
}

int KUiKillerOwnTask::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	switch(uMsg)
	{
	case WND_N_BUTTON_CLICK:
		if (!m_bRecvOver)
			break;
		if (uParam == (unsigned int)(KWndWindow*)&m_LeaveBtn)
			CloseWindow(true);
		else if (uParam == (unsigned int)(KWndWindow*)&m_CancelTaskBtn)
		{
			int nCurSel = m_TaskList.GetCurSel();
			if (nCurSel >= 0)
			{
				if (m_pTasks[nCurSel].szListItemText[0] == '$')
				{
					KSystemMessage	sMsg;
					sprintf(sMsg.szMessage, "%s", MSG_KILLER_NO_CANCEL);
					sMsg.eType = SMT_NORMAL;
					sMsg.byConfirmType = SMCT_NONE;
					sMsg.byPriority = 0;
					sMsg.byParamSize = 0;
					CoreDataChangedCallback(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				}
				else
					g_pCoreShell->OperationRequest(GOI_KILLER_CANCELTASK, m_pTasks[nCurSel].task.dwTaskID, 0);
			}
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_SortByRewardBtn)
		{
			if (m_bSortByReward)
				m_bSortIncrease = !m_bSortIncrease;
			else
				m_bSortByReward = TRUE;
			ResortList();
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_SortByTargetBtn)
		{
			if (!m_bSortByReward)
				m_bSortIncrease = !m_bSortIncrease;
			else
				m_bSortByReward = FALSE;
			ResortList();
		}
		break;
	case WND_N_SCORLLBAR_POS_CHANGED:
		if (uParam == (unsigned int)(KWndWindow*)&m_TaskListCtrl)
			m_TaskList.SetTopItemIndex(nParam);
		break;
	default:
		return KWndShowAnimate::WndProc(uMsg, uParam, nParam);
	}
	return 0;
}

void KUiKillerOwnTask::ClearData()
{
	m_bRecvOver = FALSE;
	m_TaskList.SetContent(NULL, 0, 0, 0);
	m_nCurTaskNum = m_nCurTaskNumMax = 0;
	if (m_pTasks)
	{
		delete m_pTasks;
		m_pTasks = NULL;
	}
}

void KUiKillerOwnTask::ResortList()
{
	if (m_nCurTaskNum == 0)
		return;

	std::vector<KILLER_TASK> backup;
	int i;
	for (i = 0; i < m_nCurTaskNum; i++)
	{
		if (i == 0)
			m_pTasks[i].task.byPart = 0;
		else
			m_pTasks[i].task.byPart = 1;
		backup.push_back(m_pTasks[i].task);
	}
	m_pTasks[0].task.byPart = 2;
	backup.push_back(m_pTasks[0].task);

	//recreate list
	ClearData();
	for (i = 0; i < (int)backup.size(); i++)
		ReceiveTask(&backup[i]);
}

void KUiKillerOwnTask::PushInSortedList(KILLER_TASK* pTask)
{
	m_TaskList.SetContent(NULL, 0, 0, 0);
	if (m_nCurTaskNum == m_nCurTaskNumMax)
	{
		m_nCurTaskNumMax += 32;
		if (m_pTasks)
		{
			KILLER_TASK_ITEM* pBack = m_pTasks;
			m_pTasks = new KILLER_TASK_ITEM[m_nCurTaskNumMax];
			memcpy(m_pTasks, pBack, sizeof(KILLER_TASK_ITEM) * m_nCurTaskNum);
			delete pBack;
		}
		else
		{
			m_pTasks = new KILLER_TASK_ITEM[m_nCurTaskNumMax];
		}
	}

	int i;
	for (i = 0; i < m_nCurTaskNum; i++)
	{
		if (m_pTasks[i].task.byType != pTask->byType)
			continue;
		if (m_bSortByReward)
		{
			if (m_pTasks[i].task.nTaskReward < pTask->nTaskReward)
			{
				if (m_bSortIncrease)
					continue;
				else
					break;
			}
			else
			{
				if (!m_bSortIncrease)
					continue;
				else
					break;
			}
		}
		else
		{
			if (strcmp(m_pTasks[i].task.szTargetName, pTask->szTargetName) < 0)
			{
				if (m_bSortIncrease)
					continue;
				else
					break;
			}
			else
			{
				if (!m_bSortIncrease)
					continue;
				else
					break;
			}
		}
	}

	if (i < m_nCurTaskNum)	//insert
		memmove(m_pTasks + i + 1, m_pTasks + i, (m_nCurTaskNum - i) * sizeof(KILLER_TASK_ITEM));
	//else					//append
	//	;

	m_pTasks[i].task = *pTask;
	m_pTasks[i].szListItemText[0] = 0;
	if (pTask->byType == 1)
		sprintf(m_pTasks[i].szListItemText, "$ ");
	::WPrintMoney(m_pTasks[i].szListItemText + strlen(m_pTasks[i].szListItemText), pTask->nTaskReward);

	//add space to align column text
#define FIRST_COL_WIDTH	27
	for (int j = strlen(m_pTasks[i].szListItemText); j < FIRST_COL_WIDTH; j++)
		m_pTasks[i].szListItemText[j] = ' ';
	sprintf(m_pTasks[i].szListItemText + FIRST_COL_WIDTH, "%s", pTask->szTargetName);
	m_nCurTaskNum++;

	m_TaskList.SetContent((BYTE*)m_pTasks, m_nCurTaskNum, sizeof(m_pTasks[0]), (int)(&(((KILLER_TASK_ITEM*)NULL)->szListItemText)));
}

void KUiKillerOwnTask::ReceiveTask(KILLER_TASK* pTask)	//物品变化更新
{
	if (pTask->byPart == 0)			//Head
	{
		if (pTask->byType == 0)
		{
			_ASSERT(m_bRecvOver == 0 && m_nCurTaskNum == 0);
			ClearData();
		}
		PushInSortedList(pTask);
	}
	else if (pTask->byPart == 1)	//Mid
	{
		_ASSERT(m_bRecvOver == 0 && m_nCurTaskNum != 0);
		PushInSortedList(pTask);
	}
	else						//Tail
	{
		_ASSERT(pTask->byPart == 2 && m_bRecvOver == 0);
		if (pTask->byType == 1)
			m_bRecvOver = TRUE;
	}
}