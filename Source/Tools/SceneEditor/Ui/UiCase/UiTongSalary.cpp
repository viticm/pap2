/*******************************************************************************
File        : UiTongSalary.cpp
Creator     : Fyt(Fan Zhanpeng)
create data : 02-24-2004(mm-dd-yyyy)
Description : 帮会发钱界面
********************************************************************************/

#include "KWin32.h"
#include "KIniFile.h"
#include "CoreShell.h"
#include "GameDataDef.h"

#include "../../../Represent/iRepresent/iRepresentShell.h"
#include "../../../Represent/iRepresent/KRepresentUnit.h"

#include "../elem/wnds.h"
#include "../Elem/WndMessage.h"

#include "../UiBase.h"
#include "../UiSoundSetting.h"

#include ".\uitongsalary.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

#define UI_TONG_SALARY_SCHEME "帮会发钱界面.ini"

KUiTongSalary* KUiTongSalary::ms_pSelf = NULL;

extern iCoreShell *g_pCoreShell;

KUiTongSalary::KUiTongSalary(void)
{
}

KUiTongSalary::~KUiTongSalary(void)
{
}


/*********************************************************************
* 功能：打开窗口
**********************************************************************/
KUiTongSalary* KUiTongSalary::OpenWindow()
{
	if (ms_pSelf == NULL)
	{
		ms_pSelf = new KUiTongSalary;
		if (ms_pSelf)
			ms_pSelf->Initialize();
	}
	if (ms_pSelf)
	{
		UiSoundPlay(UI_SI_WND_OPENCLOSE);
		ms_pSelf->BringToTop();
		ms_pSelf->Reset();
		ms_pSelf->Show();
		Wnd_SetExclusive((KWndWindow*)ms_pSelf);
	}
	return ms_pSelf;
}


/*********************************************************************
* 功能：如果窗口正被显示，则返回实例指针
**********************************************************************/
KUiTongSalary* KUiTongSalary::GetIfVisible()
{
	if (ms_pSelf && ms_pSelf->IsVisible())
	{
		return ms_pSelf;
	}
	return NULL;
}


/*********************************************************************
* 功能：关闭窗口，同时可以选则是否删除对象实例
**********************************************************************/
void KUiTongSalary::CloseWindow(bool bDestory)
{
	if (ms_pSelf)
	{
		ms_pSelf->Hide();
		Wnd_ReleaseExclusive((KWndWindow*)ms_pSelf);
		if (bDestory)
		{
			ms_pSelf->Destroy();
			ms_pSelf = NULL;
		}
	}
}


/*********************************************************************
* 功能：载入界面方案
**********************************************************************/
void KUiTongSalary::LoadScheme(const char* pScheme)
{
	if(ms_pSelf)
	{
		char		Buff[128];
    	KIniFile	Ini;
    	sprintf(Buff, "%s\\%s", pScheme, UI_TONG_SALARY_SCHEME);

    	if (Ini.Load(Buff))
    	{
			ms_pSelf->Init(&Ini, "Main");

			ms_pSelf->m_editSalary[enumIS_DIRECTOR].Init(&Ini, "EditDirector");
			ms_pSelf->m_editSalary[enumIS_MANAGER].Init(&Ini, "EditManager");
			ms_pSelf->m_editSalary[enumIS_MEMBER].Init(&Ini, "EditMember");

			ms_pSelf->m_btnIsTotal[enumIS_DIRECTOR].Init(&Ini, "BtnDirectorTotal");
			ms_pSelf->m_btnIsTotal[enumIS_MANAGER].Init(&Ini, "BtnManagerTotal");
			ms_pSelf->m_btnIsTotal[enumIS_MEMBER].Init(&Ini, "BtnMemberTotal");

			ms_pSelf->m_textDescription.Init(&Ini, "TextDescription");
			ms_pSelf->m_btnConfirm.Init(&Ini, "BtnConfirm");
			ms_pSelf->m_btnCancel.Init(&Ini, "BtnCancel");

			Ini.GetString("Settings", "Drscript", "长老:%d人 队长:%d人 帮众:%d人 发放金额总额:%d两",
						  ms_pSelf->m_szDescript, sizeof(ms_pSelf->m_szDescript));

			Ini.GetString("Settings", "NotEnoughMoney", "(帮会资金不足)",
						  ms_pSelf->m_szNotEnoughMoney, sizeof(ms_pSelf->m_szNotEnoughMoney));
		}
		//
	}
	//
}


/*********************************************************************
* 功能：初始化
**********************************************************************/
void KUiTongSalary::Initialize()
{
	int i;
	for(i = 0;i < enumIS_NUM;i++)
	{
		AddChild(m_editSalary + i);
	}
	for(i = 0;i < enumIS_NUM;i++)
	{
		AddChild(m_btnIsTotal + i);
	}
	AddChild(&m_textDescription);
	AddChild(&m_btnConfirm);
	AddChild(&m_btnCancel);

	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);

	Wnd_AddWindow(this);
}


/*********************************************************************
* 功能：窗口消息处理函数
**********************************************************************/
int KUiTongSalary::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int nRet = 0;
	switch(uMsg)
	{
	case WND_N_BUTTON_CLICK:
		{
			KWndWindow *pWnd = (KWndWindow *)uParam;
			if(pWnd == &m_btnConfirm)
			{
				OnConfirm();
			}
			else if(pWnd == &m_btnCancel)
			{
				CloseWindow();
			}
			else if(pWnd >= m_btnIsTotal && pWnd <= m_btnIsTotal + enumIS_NUM - 1)
			{
				OnIsTotalButtonCheck();
			}
		}
		break;

	case WND_N_EDIT_CHANGE:
		{
			KWndWindow *pWnd = (KWndWindow *)uParam;
			OnEditChange(pWnd);
		}
		break;

	default:
		nRet = KWndImage::WndProc(uMsg, uParam, nParam);
		break;
	}

	return nRet;
}


/*********************************************************************
* 功能：重置计数数值
**********************************************************************/
void KUiTongSalary::Reset()
{
	memset(m_aryCount, 0, sizeof(m_aryCount[0]) * enumIS_NUM);
	memset(m_arySalary, 0, sizeof(m_arySalary[0]) * enumST_NUM);
	memset(m_arybIsTotal, 1, sizeof(m_arybIsTotal) * enumIS_NUM);
	memset(m_aryInputSalary, 0, sizeof(m_aryInputSalary[0]) * enumIS_NUM);
}


/*********************************************************************
* 功能：清理人数
**********************************************************************/
void KUiTongSalary::ClearCount()
{
	memset(m_aryCount, 0, sizeof(m_aryCount));
}


/*********************************************************************
* 功能：外部数据进来的处理，简单的统计在线人数就ok
**********************************************************************/
void KUiTongSalary::InjectData(KTongMemberItem *pItem, int nCount, int nType)
{
	enumINPUT_SALARY eType;
	if (nType == enumTONG_FIGURE_DIRECTOR)
	{
		eType = enumIS_DIRECTOR;
	}
	else if (nType == enumTONG_FIGURE_MANAGER)
	{
		eType = enumIS_MANAGER;
	}
	else if (nType == enumTONG_FIGURE_MEMBER)
	{
		eType = enumIS_MEMBER;
	}
	else
	{
		return;
	}
	for (int i = 0;i < nCount;i++)
	{
		if (pItem[i].nFlag & 0x02)
		{
			m_aryCount[(int)eType]++;
		}
	}
	InjectNotify();
}


/*********************************************************************
* 功能：导入部分数据完成的提醒
**********************************************************************/
void KUiTongSalary::InjectNotify()
{
	WorkoutSalary();
	UpdateView();
}


/*********************************************************************
* 功能：根据人数计算各个薪水数字
**********************************************************************/
void KUiTongSalary::WorkoutSalary()
{
	if(m_arybIsTotal[enumIS_DIRECTOR])
	{
		if(m_aryCount[enumIS_DIRECTOR])
		{
			m_arySalary[enumST_WHOLE_DIRECTOR] = m_aryInputSalary[enumIS_DIRECTOR];
			m_arySalary[enumST_PER_DIRECTOR] = m_aryInputSalary[enumIS_DIRECTOR] / m_aryCount[enumIS_DIRECTOR];
		}
		else
		{
			m_arySalary[enumST_WHOLE_DIRECTOR] = 0;
			m_arySalary[enumST_PER_DIRECTOR] = 0;
		}
	}
	else
	{
		m_arySalary[enumST_PER_DIRECTOR] = m_aryInputSalary[enumIS_DIRECTOR];
		m_arySalary[enumST_WHOLE_DIRECTOR] = m_aryInputSalary[enumIS_DIRECTOR] * m_aryCount[enumIS_DIRECTOR];
	}

	if(m_arybIsTotal[enumIS_MANAGER])
	{
		if(m_aryCount[enumIS_MANAGER])
		{
			m_arySalary[enumST_WHOLE_MANAGER] = m_aryInputSalary[enumIS_MANAGER];
			m_arySalary[enumST_PER_MANAGER] = m_aryInputSalary[enumIS_MANAGER] / m_aryCount[enumIS_MANAGER];
		}
		else
		{
			m_arySalary[enumST_WHOLE_MANAGER] = 0;
			m_arySalary[enumST_PER_MANAGER] = 0;
		}
	}
	else
	{
		m_arySalary[enumST_PER_MANAGER] = m_aryInputSalary[enumIS_MANAGER];
		m_arySalary[enumST_WHOLE_MANAGER] = m_aryInputSalary[enumIS_MANAGER] * m_aryCount[enumIS_MANAGER];
	}

	if(m_arybIsTotal[enumIS_MEMBER])
	{
		if(m_aryCount[enumIS_MEMBER])
		{
			m_arySalary[enumST_WHOLE_MEMBER] = m_aryInputSalary[enumIS_MEMBER];
			m_arySalary[enumST_PER_MEMBER] = m_aryInputSalary[enumIS_MEMBER] / m_aryCount[enumIS_MEMBER];
		}
		else
		{
			m_arySalary[enumST_WHOLE_MEMBER] = 0;
			m_arySalary[enumST_PER_MEMBER] = 0;
		}
	}
	else
	{
		m_arySalary[enumST_PER_MEMBER] = m_aryInputSalary[enumIS_MEMBER];
		m_arySalary[enumST_WHOLE_MEMBER] = m_aryInputSalary[enumIS_MEMBER] * m_aryCount[enumIS_MEMBER];
	}

	m_arySalary[enumST_WHOLE] = m_arySalary[enumST_WHOLE_DIRECTOR]  + m_arySalary[enumST_WHOLE_MANAGER]
																	+ m_arySalary[enumST_WHOLE_MEMBER];
}


/*********************************************************************
* 功能：响应字符编辑框的改动
**********************************************************************/
void KUiTongSalary::OnEditChange(KWndWindow *pWnd)
{
	for(int i = 0;i < enumIS_NUM;i++)
	{
		if(pWnd == &m_editSalary[i])
		{
			int nSalary = m_editSalary[i].GetIntNumber();
			m_aryInputSalary[i] = nSalary;
		}
	}
	//
	WorkoutSalary();
	UpdateView();
}


/*********************************************************************
* 功能：响应三个总数/单人切换按钮的点击
**********************************************************************/
void KUiTongSalary::OnIsTotalButtonCheck()
{
	for(int i = 0;i < enumIS_NUM;i++)
	{
		m_arybIsTotal[i] = m_btnIsTotal[i].IsButtonChecked();
	}
}


/*********************************************************************
* 功能：为薪水发放命令的书写安排文案数据(就是告诉我有多少资金啦)
**********************************************************************/
void KUiTongSalary::ArrangeDecree(KTongInfo sInfo)
{
	memcpy(&m_TongInfo, &sInfo, sizeof(m_TongInfo));
}


/*********************************************************************
* 功能：更新界面显示
**********************************************************************/
void KUiTongSalary::UpdateView()
{
	char szDescription[256];

	sprintf(szDescription, m_szDescript, m_aryCount[enumIS_DIRECTOR], m_aryCount[enumIS_MANAGER], m_aryCount[enumIS_MEMBER], m_arySalary[enumST_WHOLE]);
	if ((DWORD)m_arySalary[enumST_WHOLE] > m_TongInfo.nLoMoney && !m_TongInfo.nHiMoney)
	{
		strcat(szDescription, m_szNotEnoughMoney);
	}
	m_textDescription.SetText(szDescription);
}


/*********************************************************************
* 功能：响应确定按钮按下
**********************************************************************/
void KUiTongSalary::OnConfirm()
{
	if((DWORD)m_arySalary[enumST_WHOLE] > m_TongInfo.nLoMoney || m_TongInfo.nHiMoney)
	{
		if(g_pCoreShell)
		{
			KTongOperationParam sParam;
			sParam.eOper = TONG_ACTION_SALARY;
			sParam.nData[0] = m_arySalary[enumST_PER_DIRECTOR];
			sParam.nData[1] = m_arySalary[enumST_PER_MANAGER];
			sParam.nData[1] = m_arySalary[enumST_PER_MEMBER];
			g_pCoreShell->TongOperation(GTOI_TONG_ACTION, (unsigned int)&sParam, 0);
		}
		CloseWindow();
	}
}
