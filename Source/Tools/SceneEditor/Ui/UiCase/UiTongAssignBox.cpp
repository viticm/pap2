/*******************************************************************************
File        : UiTongAssignBox.h
Creator     : Fyt(Fan Zhanpeng)
create data : 08-29-2003(mm-dd-yyyy)
Description : 任命帮会职位的界面
********************************************************************************/

#include "KWin32.h"
#include "KIniFile.h"
#include "CoreShell.h"

#include "../elem/wnds.h"
#include "../elem/wndmessage.h"

#include "../UiBase.h"
#include "../UiSoundSetting.h"

#include "../../../Engine/src/KFilePath.h"
#include "../../Core/Src/GameDataDef.h"

#include "UiTongAssignBox.h"
#include "UiInformation.h"
#include "UiTongGetString.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

#define TONG_ASSIGN_BOX_INI "帮会职位任命.ini"


extern iCoreShell *g_pCoreShell;

KUiTongAssignBox* KUiTongAssignBox::ms_pSelf = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

KUiTongAssignBox::KUiTongAssignBox()
{
	m_pMain = NULL;
	m_pManagerData  = NULL;
	m_nSelectFigure = -1;
	m_pDirectorData = NULL;
	m_nManagerDataCount  = 0;
	m_nDirectorDataCount = 0;
	m_szTargetPlayerName[0] = 0;
	m_szTargetPlayerAgName[0] = 0;
}

KUiTongAssignBox::~KUiTongAssignBox()
{

}


/*********************************************************************
* 功能：打开窗口
**********************************************************************/
KUiTongAssignBox* KUiTongAssignBox::OpenWindow()
{
	if(g_pCoreShell)
	{
    	if (ms_pSelf == NULL)
    	{
		    ms_pSelf = new KUiTongAssignBox;
		    if (ms_pSelf)
    			ms_pSelf->Initialize();
    	}
    	if (ms_pSelf)
    	{
		    UiSoundPlay(UI_SI_WND_OPENCLOSE);
			ms_pSelf->BringToTop();
			ms_pSelf->Show();
	    }
	}
	return ms_pSelf;
}


/*********************************************************************
* 功能：如果窗口正被显示，则返回实例指针
**********************************************************************/
KUiTongAssignBox* KUiTongAssignBox::GetIfVisible()
{
	if (ms_pSelf && ms_pSelf->IsVisible())
		return ms_pSelf;
	return NULL;
}


/*********************************************************************
* 功能：关闭窗口，同时可以选则是否删除对象实例
**********************************************************************/
void KUiTongAssignBox::CloseWindow(bool bDestory)
{
	if (ms_pSelf)
	{
		ms_pSelf->Hide();
		if (bDestory)
		{
			ms_pSelf->Destroy();
			ms_pSelf = NULL;
		}
	}
}


/*********************************************************************
* 功能：初始化
**********************************************************************/
void KUiTongAssignBox::Initialize()
{
	AddChild(&m_BtnPilgarlic);
	AddChild(&m_TargetName);
	AddChild(&m_BtnCaptain);
	AddChild(&m_BtnConfirm);
	AddChild(&m_BtnCancel);
	AddChild(&m_BtnElder);
	AddChild(&m_TextError);
	AddChild(&m_AssignPlace);
	AddChild(&m_ScrollBar);
	m_AssignPlace.SetScrollbar(&m_ScrollBar);
	m_AssignPlace.ResetContent();

//	m_BtnPilgarlic.Hide();
//	m_TargetName.Hide();
//	m_BtnCaptain.Hide();
//	m_BtnConfirm.Hide();
//	m_BtnCancel.Hide();
//	m_BtnElder.Hide();
//	m_BtnConfirm.Enable(FALSE);

	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);

	Wnd_AddWindow(this);
}


/*********************************************************************
* 功能：载入界面方案
**********************************************************************/
void KUiTongAssignBox::LoadScheme(const char* pScheme)
{
	if(ms_pSelf)
	{
		char		Buff[128];
		KIniFile	Ini;
		sprintf(Buff, "%s\\%s", pScheme, TONG_ASSIGN_BOX_INI);

		if(Ini.Load(Buff))
		{
			ms_pSelf->Init(&Ini, "Main");

			ms_pSelf->m_BtnPilgarlic.Init(&Ini, "BtnMember");
			ms_pSelf->m_BtnCaptain.Init(&Ini, "BtnManager");
			ms_pSelf->m_BtnConfirm.Init(&Ini, "BtnConfirm");
			ms_pSelf->m_BtnElder.Init(&Ini, "BtnDirector");
			ms_pSelf->m_BtnCancel.Init(&Ini, "BtnCancel");
			ms_pSelf->m_TargetName.Init(&Ini, "PlayerName");
			ms_pSelf->m_TextError.Init(&Ini, "ErrorBox");
			ms_pSelf->m_AssignPlace.Init(&Ini, "List");
			ms_pSelf->m_ScrollBar.Init(&Ini, "ListScroll");

			ms_pSelf->m_BtnElder.SetText("长老");
			ms_pSelf->m_BtnCaptain.SetText("队长");
			ms_pSelf->m_BtnPilgarlic.SetText("帮众");

			Ini.GetString("Main", "AssignString", "", ms_pSelf->m_szAssign, sizeof(ms_pSelf->m_szAssign));
			Ini.GetString("Main", "CancelString", "", ms_pSelf->m_szCancel, sizeof(ms_pSelf->m_szCancel));
			Ini.GetString("Main", "ConfirmString", "", ms_pSelf->m_szConfirm, sizeof(ms_pSelf->m_szConfirm));
			Ini.GetString("Main", "ErrorFigure", "请选择职位", ms_pSelf->m_szErrorNotSelectFigure, sizeof(ms_pSelf->m_szErrorNotSelectFigure));
			Ini.GetString("Main", "ErrorPlace", "请选择在职位中的地位", ms_pSelf->m_szErrorNotSelectPlace, sizeof(ms_pSelf->m_szErrorNotSelectPlace));
			Ini.GetString("Main", "ErrorNotEmpty", "那个位置已经有人了", ms_pSelf->m_szIsNotEmpty, sizeof(ms_pSelf->m_szIsNotEmpty));
		}
	}
}


/*********************************************************************
* 功能：窗口函数
**********************************************************************/
int KUiTongAssignBox::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	switch(uMsg)
	{
	case WND_N_BUTTON_CLICK:
		if(uParam == (unsigned int)&m_BtnConfirm)
		{
			OnConfirm();
		}
		else if(uParam == (unsigned int)&m_BtnCancel)
		{
			if(m_pMain)
			{
    			m_pMain->Show();
	    		m_pMain = NULL;
			}
			CloseWindow();
		}
		else if(uParam == (unsigned int)&m_BtnElder)
		{
			UpdateCheckButton(enumTONG_FIGURE_DIRECTOR);
		}
		else if(uParam == (unsigned int)&m_BtnCaptain)
		{
			UpdateCheckButton(enumTONG_FIGURE_MANAGER);
		}
		else if(uParam == (unsigned int)&m_BtnPilgarlic)
		{
			UpdateCheckButton(enumTONG_FIGURE_MEMBER);
		}
		break;

	case WND_M_OTHER_WORK_RESULT:
		if(uParam == RESULT_T_ASSIGN)
		{
			if(nParam && g_pCoreShell)
			{
				KTongOperationParam Param;
    			KTongMemberItem Member;

			    Param.eOper = TONG_ACTION_ASSIGN;
			    Param.nData[0] = m_nCurrentFigure;
			    Param.nData[2] = m_nSelectFigure;
			    Param.nData[1] = m_nCurrentPostion;
			    Param.nData[3] = m_AssignPlace.GetCurSel();

			    memset(&Member, 0, sizeof(KTongMemberItem));
			    strcpy(Member.Name, (char *)nParam);

 			    g_pCoreShell->TongOperation(GTOI_TONG_ACTION, (unsigned int)&Param, (int)&Member);
			}
			if(m_pMain)
			{
    			m_pMain->Show();
	    		m_pMain = NULL;
			}
			CloseWindow();
		}
		break;

	case WND_N_SCORLLBAR_POS_CHANGED:
		if(uParam == (unsigned int)&m_ScrollBar)
		{
    		m_AssignPlace.SetTopItemIndex(nParam);
		}

	default:
		return KWndImage::WndProc(uMsg, uParam, nParam);
	}

	return 1;
}


/*********************************************************************
* 功能：建立这个界面和主界面的关联
**********************************************************************/
void KUiTongAssignBox::LinkToMainWindow(KWndWindow *pMain)
{
	if(pMain && ms_pSelf)
	{
		ms_pSelf->m_pMain = pMain;
		ms_pSelf->m_pMain->Hide();
	}
}


/*********************************************************************
* 功能：为任命案的书写安排数据
**********************************************************************/
void KUiTongAssignBox::ArrangeData(int nType, char* szTarName, char* szTarAgName, int nCurrentFigure, int nCurrentPosition)
{
	if((nType == enumTONG_FIGURE_DIRECTOR || nType == enumTONG_FIGURE_MASTER) &&
		szTarName && szTarName[0] && nCurrentFigure >= enumTONG_FIGURE_MEMBER &&
		nCurrentFigure <= enumTONG_FIGURE_DIRECTOR && ms_pSelf)
	{
		ms_pSelf->m_nType = nType;
		strcpy(ms_pSelf->m_szTargetPlayerName, szTarName);
		ms_pSelf->m_TargetName.SetText(szTarName);

		if(szTarAgName && szTarAgName[0])
		{
			strcpy(ms_pSelf->m_szTargetPlayerAgName, szTarAgName);
		}
		ms_pSelf->m_nCurrentFigure = nCurrentFigure;
		ms_pSelf->m_nCurrentPostion = nCurrentPosition;

		if(nType == enumTONG_FIGURE_DIRECTOR)
		{
			ms_pSelf->m_BtnElder.Hide();
			ms_pSelf->m_BtnCaptain.Show();
			ms_pSelf->m_BtnPilgarlic.Show();
		}
		else if(nType == enumTONG_FIGURE_MASTER)
		{
			ms_pSelf->m_BtnElder.Show();
			ms_pSelf->m_BtnCaptain.Show();
			ms_pSelf->m_BtnPilgarlic.Show();
		}
		if(nCurrentFigure == enumTONG_FIGURE_DIRECTOR)
		{
			//ms_pSelf->m_BtnElder.Hide();
		}
		else if(nCurrentFigure == enumTONG_FIGURE_MANAGER)
		{
			//ms_pSelf->m_BtnCaptain.Hide();
		}
		else if(nCurrentFigure == enumTONG_FIGURE_MEMBER)
		{
			ms_pSelf->m_BtnPilgarlic.Hide();
		}
	}
}


/*********************************************************************
* 功能：Check管理函数
**********************************************************************/
void KUiTongAssignBox::UpdateCheckButton(int nFigure)
{
	if(nFigure >= enumTONG_FIGURE_MEMBER && nFigure <= enumTONG_FIGURE_DIRECTOR)
	{
		m_BtnElder.CheckButton(FALSE);
		m_BtnCaptain.CheckButton(FALSE);
		m_BtnPilgarlic.CheckButton(FALSE);

		m_nSelectFigure = nFigure;
		switch(nFigure)
		{
		case enumTONG_FIGURE_MEMBER:
			m_BtnPilgarlic.CheckButton(TRUE);
			break;

		case enumTONG_FIGURE_MANAGER:
			m_BtnCaptain.CheckButton(TRUE);
			break;

		case enumTONG_FIGURE_DIRECTOR:
			m_BtnElder.CheckButton(TRUE);
			break;
		}
		UpdateAssignList(nFigure);
		m_BtnConfirm.Enable(TRUE);
	}
	else
	{
		m_BtnConfirm.Enable(FALSE);
	}
}


/*********************************************************************
* 功能：弹出确认窗口
**********************************************************************/
void KUiTongAssignBox::PopupConfirmWindow(const char* pszInfo, unsigned int uHandleID)
{
    KUiTongGetString::OpenWindow(pszInfo, m_szTargetPlayerName, this, uHandleID, 2, 32);
	Hide();
}


/*********************************************************************
* 功能：响应确认操作
**********************************************************************/
void KUiTongAssignBox::OnConfirm()
{
	if(m_nSelectFigure != -1)
	{
		int i = m_AssignPlace.GetCurSel();
		if(m_nSelectFigure != enumTONG_FIGURE_MEMBER && i == -1)
		{
			m_TextError.SetText(m_szErrorNotSelectPlace);
		}
		else
		{
			if(!IsPlaceEmpty(m_nSelectFigure, m_AssignPlace.GetCurSel()))
			{
				m_TextError.SetText(m_szIsNotEmpty);
			}
			else
			{
				PopupConfirmWindow(m_szAssign, RESULT_T_ASSIGN);
			}
		}
	}
	else
	{
		m_TextError.SetText(m_szErrorNotSelectFigure);
	}
}


  /*__(@_             功能：根据nType插入数据
 /     ) \______________________________________________________
(_)@8@8{}<______________________________________________________>
       )_/
      (*/
void KUiTongAssignBox::InjectData(KTongMemberItem *pData, int nDataCount, int nType)
{
	KTongMemberItem **ppShzi;
	int              *pCount;
	switch(nType)
	{
	case enumTONG_FIGURE_DIRECTOR:
		ppShzi = &m_pDirectorData;
		pCount = &m_nDirectorDataCount;
		break;

	case enumTONG_FIGURE_MANAGER:
		ppShzi = &m_pManagerData;
		pCount = &m_nManagerDataCount;
		break;
	}
	if(*ppShzi)
	{
		delete(*ppShzi);
		*ppShzi = NULL;
	}
	*ppShzi = new KTongMemberItem[nDataCount];
	if(*ppShzi)
	{
		memcpy(*ppShzi, pData, sizeof(KTongMemberItem) * nDataCount);
		*pCount = nDataCount;
	}
}


  /*__(@_              功能：更新职位列表
 /     ) \______________________________________________________
(_)@8@8{}<______________________________________________________>
       )_/
      (*/
void KUiTongAssignBox::UpdateAssignList(int nType)
{
	KTongMemberItem **pShzi;
	int              *pCount;
	switch(nType)
	{
	case enumTONG_FIGURE_DIRECTOR:
		pShzi  = &m_pDirectorData;
		pCount = &m_nDirectorDataCount;
		break;

	case enumTONG_FIGURE_MANAGER:
		pShzi  = &m_pManagerData;
		pCount = &m_nManagerDataCount;
		break;

	case enumTONG_FIGURE_MEMBER:
		pShzi  = NULL;
		pCount = NULL;
		break;

	default:
		return;
		break;
	}
	m_AssignPlace.ResetContent();
	if(pShzi != NULL && pCount != NULL && *pShzi != NULL && *pCount > 0)
	{
		char szBuf[64];
		for(int i = 0;i < *pCount;i++)
		{
			sprintf(szBuf, "%-2d.%15s %s", i+1, pShzi[0][i].Name, pShzi[0][i].szAgname);
			m_AssignPlace.AddString(i, szBuf);
		}
	}
}


  /*__(@_           功能：判断一个位置是否已经有人
 /     ) \______________________________________________________
(_)@8@8{}<______________________________________________________>
       )_/
      (*/
int	KUiTongAssignBox::IsPlaceEmpty(int nFigure, int nPlace)
{
	KTongMemberItem *pData;
	int nCount;
	switch(nFigure)
	{
	case enumTONG_FIGURE_DIRECTOR:
		pData  = m_pDirectorData;
		nCount = m_nDirectorDataCount;
		break;

	case enumTONG_FIGURE_MANAGER:
		pData  = m_pManagerData;
		nCount = m_nManagerDataCount;
		break;

	case enumTONG_FIGURE_MEMBER:
		return 1;
		break;

	default:
		return 0;
		break;
	}

	if(nPlace < nCount)
	{
		if(pData[nPlace].Name[0])
			return 0;
		else
			return 1;
	}
	return 0;
}
