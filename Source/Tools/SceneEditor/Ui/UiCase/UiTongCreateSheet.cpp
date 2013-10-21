/********************************************************************
File        : UiTongCreateSheet.cpp
Creator     : Fyt(Fan Zhanpeng)
create data : 08-29-2003(mm-dd-yyyy)
Description : 创建帮会的表单
*********************************************************************/

#include "KWin32.h"
#include "KIniFile.h"
#include "CoreShell.h"
#include "GameDataDef.h"

#include "../elem/wnds.h"
#include "../Elem/WndMessage.h"

#include "../UiBase.h"
#include "../UiSoundSetting.h"

#include "UiTongCreateSheet.h"

#include "../ChatFilter.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

#define TONG_CREATE_SHEET_SCHEME	"帮会创建单.ini"
#define STR_NAME_NOT_CORRECT		"名字不合法。"

KUiTongCreateSheet* KUiTongCreateSheet::ms_pSelf = NULL;


extern iCoreShell *g_pCoreShell;
extern CChatFilter g_ChatFilter;



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

KUiTongCreateSheet::KUiTongCreateSheet()
{
	m_nSelectSide = -1;
	m_nUsage = enumCSU_CREATE_TONG;
}

KUiTongCreateSheet::~KUiTongCreateSheet()
{

}


/*********************************************************************
* 功能：打开窗口
**********************************************************************/
KUiTongCreateSheet* KUiTongCreateSheet::OpenWindow()
{
	if (ms_pSelf == NULL)
	{
		ms_pSelf = new KUiTongCreateSheet;
		if (ms_pSelf)
			ms_pSelf->Initialize();
	}
	if (ms_pSelf)
	{
		UiSoundPlay(UI_SI_WND_OPENCLOSE);
		ms_pSelf->BringToTop();
		ms_pSelf->Show();
		ms_pSelf->m_nUsage = enumCSU_CREATE_TONG;
	}
	return ms_pSelf;
}


/*********************************************************************
* 功能：如果窗口正被显示，则返回实例指针
**********************************************************************/
KUiTongCreateSheet* KUiTongCreateSheet::GetIfVisible()
{
	if (ms_pSelf && ms_pSelf->IsVisible())
		return ms_pSelf;
	return NULL;
}


/*********************************************************************
* 功能：关闭窗口，同时可以选则是否删除对象实例
**********************************************************************/
void KUiTongCreateSheet::CloseWindow(bool bDestory)
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
void KUiTongCreateSheet::Initialize()
{
	AddChild(&m_EditTongName);
	AddChild(&m_BtnOrder);
	AddChild(&m_BtnNatural);
	AddChild(&m_BtnChaos);
	AddChild(&m_BtnDone);
	AddChild(&m_BtnCancel);
	AddChild(&m_TextError);

	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);

	Wnd_AddWindow(this);
}


/*********************************************************************
* 功能：载入界面方案
**********************************************************************/
void KUiTongCreateSheet::LoadScheme(const char* pScheme)
{
	if(ms_pSelf)
	{
    	char		Buff[128];
    	KIniFile	Ini;
    	sprintf(Buff, "%s\\%s", pScheme, TONG_CREATE_SHEET_SCHEME);
    	if (Ini.Load(Buff))
    	{
		    ms_pSelf->Init(&Ini, "Main");

		    ms_pSelf->m_BtnOrder.Init(&Ini, "BtnOrder");
			ms_pSelf->m_BtnChaos.Init(&Ini, "BtnChaos");
			ms_pSelf->m_BtnNatural.Init(&Ini, "BtnNatural");

			ms_pSelf->m_TextError.Init(&Ini, "ErrorBox");
			ms_pSelf->m_EditTongName.Init(&Ini, "Input");

			ms_pSelf->m_BtnDone.Init(&Ini, "BtnDone");
			ms_pSelf->m_BtnCancel.Init(&Ini, "BtnCancel");

			Ini.GetString("ErrorBox", "WithoutName", "",
				ms_pSelf->m_szNameNullString, sizeof(ms_pSelf->m_szNameNullString));
			Ini.GetString("ErrorBox", "WithoutFaction", "",
				ms_pSelf->m_szAlignmentNullString, sizeof(ms_pSelf->m_szAlignmentNullString));

			Ini.GetString("ErrorBox", "ChangeFaction", "",
				ms_pSelf->m_szChangeAlignmentString, sizeof(ms_pSelf->m_szChangeAlignmentString));

			Ini.GetString("ErrorBox", "ChangeName", "",
				ms_pSelf->m_szChangeNameString, sizeof(ms_pSelf->m_szChangeNameString));
		}
		/*--------------------*/
	}
	/*--------------------*/
}


/*********************************************************************
* 功能：窗口函数
**********************************************************************/
int KUiTongCreateSheet::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	switch(uMsg)
	{
	case WND_N_BUTTON_CLICK:
		if(uParam == (unsigned int)&m_BtnOrder )
		{
			AlignmentButtonCheck(camp_justice);
		}
		else if(uParam == (unsigned int)&m_BtnChaos)
		{
			AlignmentButtonCheck( camp_evil  );
		}
		else if(uParam == (unsigned int)&m_BtnNatural  )
		{
			AlignmentButtonCheck(camp_balance);
		}
		/*----------------------------------------------------------*/
		else if(uParam == (unsigned int)&m_BtnCancel)
		{
			CloseWindow();
		}
		else if(uParam == (unsigned int)&m_BtnDone)
		{
			OnDone();
		}
		break;

	default:
		return KWndImage::WndProc(uMsg, uParam, nParam);
		break;
	}
    return 1;
}


/*********************************************************************
* 功能：正中邪CheckBox的管理函数
**********************************************************************/
void KUiTongCreateSheet::AlignmentButtonCheck(NPCCAMP eSide)
{
	m_BtnOrder.CheckButton(FALSE);
	m_BtnChaos.CheckButton(FALSE);
	m_BtnNatural.CheckButton(FALSE);

	switch(eSide)
	{
	case camp_justice:
		m_BtnOrder.CheckButton(TRUE);
		break;

	case camp_evil:
		m_BtnChaos.CheckButton(TRUE);
		break;

	case camp_balance:
		m_BtnNatural.CheckButton(TRUE);
		break;
	}
	m_nSelectSide = eSide;
}


/*********************************************************************
* 功能：响应确认按钮被按下
**********************************************************************/
void KUiTongCreateSheet::OnDone()
{
	char Buff[16];

	m_EditTongName.GetText(Buff, sizeof(Buff), TRUE);

	if(Buff[0] && g_pCoreShell)
	{
		if(m_nSelectSide != -1)
		{
			if(m_nUsage == enumCSU_CREATE_TONG)
			{
				if(g_ChatFilter.IsTextPass(Buff))
				{
					g_pCoreShell->TongOperation(GTOI_TONG_CREATE, (unsigned int)Buff, (int)m_nSelectSide);
					CloseWindow();
				}
				else
				{
					m_TextError.SetText(STR_NAME_NOT_CORRECT, strlen(STR_NAME_NOT_CORRECT));
				}
			}
			else if(m_nUsage == enumCSU_CHANGE_CAMP && m_nSelectSide != m_nOldAlignment)
			{
				//waiting fyt  改阵营
				KTongOperationParam Param;
				Param.eOper = TONG_ACTION_CHANGE_CAMP;
				Param.nData[0] = m_nSelectSide;
				Param.nData[1] = Param.nData[2] = Param.nData[3] = 0;
				m_EditTongName.GetText(Param.szPassword, sizeof(Param.szPassword), FALSE);
				g_pCoreShell->TongOperation(GTOI_TONG_ACTION, (unsigned int)&Param, 0);
				CloseWindow();
			}
			else if(m_nUsage == enumCSU_CHANGE_NAME)
			{
				//waiting fyt  改名
				CloseWindow();
			}
		}
		else
		{
			m_TextError.SetText(m_szAlignmentNullString, strlen(m_szAlignmentNullString));
		}
	}
	else
	{
		m_TextError.SetText(m_szNameNullString, strlen(m_szNameNullString));
	}
}


  /*__(@_               功能：设置界面用途
 /     ) \______________________________________________________
(_)@8@8{}<______________________________________________________>
       )_/
      (*/
void KUiTongCreateSheet::SetUsage(int nUsage)
{
	m_EditTongName.Enable(FALSE);
	m_BtnOrder.Enable(FALSE);
	m_BtnNatural.Enable(FALSE);
	m_BtnChaos.Enable(FALSE);

	switch(nUsage)
	{
	case enumCSU_CREATE_TONG:
		m_EditTongName.Enable(TRUE);
		m_BtnOrder.Enable(TRUE);
		m_BtnNatural.Enable(TRUE);
		m_BtnChaos.Enable(TRUE);
		break;

	case enumCSU_CHANGE_CAMP:
		m_BtnOrder.Enable(TRUE);
		m_BtnNatural.Enable(TRUE);
		m_BtnChaos.Enable(TRUE);
		m_TextError.SetText(m_szChangeAlignmentString);
		break;

	case enumCSU_CHANGE_NAME:
		m_EditTongName.Enable(TRUE);
		m_TextError.SetText(m_szChangeNameString);
		break;

	default:
		return;
		break;
	}
	m_nUsage = nUsage;
}


  /*__(@_                功能：导入预设值
 /     ) \______________________________________________________
(_)@8@8{}<______________________________________________________>
       )_/
      (*/
void KUiTongCreateSheet::InjectData(const char *szName, int nCamp)
{
	if(szName && szName[0])
	{
		m_EditTongName.SetText(szName);
	}
	AlignmentButtonCheck((NPCCAMP)nCamp);
	m_nOldAlignment = nCamp;
}