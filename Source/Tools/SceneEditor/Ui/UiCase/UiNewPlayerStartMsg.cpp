/*****************************************************************************************
//	界面--新手进入的提示界面
//	Copyright : Kingsoft 2003
//	Author	:   Freeway Chen
//	CreateTime:	2003-7-8
------------------------------------------------------------------------------------------
*****************************************************************************************/
#include "KWin32.h"
#include "KIniFile.h"
#include "../Elem/WndMessage.h"
#include "../Elem/Wnds.h"
#include "../UiBase.h"
#include "../UiShell.h"
#include "UiInit.h"
#include "UiNewPlayerStartMsg.h"
#include "UiHelper.h"

#include "CoreShell.h"
//#include "../UiSoundSetting.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

#define	SCHEME_INI_NEWENTERINFO			"新手进入的初始界面.ini"
#define	NEWENTERINFO_INI				"\\Ui\\NewEnterInfo.ini"

KUiNewPlayerStartMsg* KUiNewPlayerStartMsg::m_pSelf = NULL;

extern iCoreShell *g_pCoreShell;

//--------------------------------------------------------------------------
//	功能：构造函数
//--------------------------------------------------------------------------
KUiNewPlayerStartMsg::KUiNewPlayerStartMsg()
{
}

KUiNewPlayerStartMsg::~KUiNewPlayerStartMsg()
{
}


//--------------------------------------------------------------------------
//	功能：打开窗口，返回唯一的一个类对象实例
//--------------------------------------------------------------------------
KUiNewPlayerStartMsg* KUiNewPlayerStartMsg::OpenWindow()
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiNewPlayerStartMsg;
		m_pSelf->Initialize();
	}
	if (m_pSelf)
	{
		//UiSoundPlay(UI_SI_POPUP_OUTGAME_WND);
		m_pSelf->Show();
        Wnd_SetExclusive(m_pSelf);
	}

	return m_pSelf;
}

//--------------------------------------------------------------------------
//	功能：关闭窗口
//--------------------------------------------------------------------------
void KUiNewPlayerStartMsg::CloseWindow(bool bDestroy)
{
	if (m_pSelf)
	{
        Wnd_ReleaseExclusive(m_pSelf);
		if (bDestroy)
		{
			m_pSelf->Destroy();
			m_pSelf = NULL;
		}
		else
		{
			m_pSelf->Hide();
		}
	}
}

//--------------------------------------------------------------------------
//	功能：初始化
//--------------------------------------------------------------------------
void KUiNewPlayerStartMsg::Initialize()
{
	AddChild(&m_List);
	AddChild(&m_AdvanceList);
	AddChild(&m_NewerBtn);
	AddChild(&m_AdvanceBtn);
	AddChild(&m_Ok);

	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);

	LoadList();

	m_NewerBtn.CheckButton(true);
	m_AdvanceBtn.CheckButton(false);
	m_AdvanceList.Hide();

	Wnd_AddWindow(this, WL_TOPMOST);
}

void KUiNewPlayerStartMsg::LoadList()
{
	KIniFile	File;
	if (!File.Load(NEWENTERINFO_INI))
		return;

    MsgListBox_LoadContent(m_List.GetMessageListBox(), &File, "List");
    MsgListBox_LoadContent(m_AdvanceList.GetMessageListBox(), &File, "AdvanceList");
}

//--------------------------------------------------------------------------
//	功能：载入窗口的界面方案
//--------------------------------------------------------------------------
void KUiNewPlayerStartMsg::LoadScheme(const char* pScheme)
{
	char		Buff[128];
	KIniFile	Ini;
	sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI_NEWENTERINFO);
	if (Ini.Load(Buff))
	{
		KWndShowAnimate::Init(&Ini, "Main");
		m_List              .Init(&Ini, "List");
		m_AdvanceList       .Init(&Ini, "List");
        m_NewerBtn          .Init(&Ini, "NewerBtn");
        m_AdvanceBtn        .Init(&Ini, "AdvanceNewerBtn");
		m_Ok                .Init(&Ini, "Ok");
	}
}

//--------------------------------------------------------------------------
//	功能：窗口函数
//--------------------------------------------------------------------------
int KUiNewPlayerStartMsg::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int	nRet = 0;
	switch(uMsg)
	{
	case WND_N_BUTTON_CLICK:
		OnClickButton((KWndButton*)(KWndWindow*)uParam);
		break;
	case WM_KEYDOWN:
		nRet = OnKeyDown(uParam);
		break;
	default:
		nRet = KWndShowAnimate::WndProc(uMsg, uParam, nParam);
		break;
	}
	return nRet;
}

int KUiNewPlayerStartMsg::OnKeyDown(unsigned int uKey)
{
	int	nRet = 1;
	switch (uKey)
	{
	case VK_RETURN:
	case VK_ESCAPE:
		OnOk();
		break;
//	case VK_UP:
//		nRet = m_List.GetCurSel();
//		if (nRet  > 0)
//			m_List.SetCurSel(nRet - 1);
//		else if (nRet < 0)
//			m_List.SetCurSel(0);
//		m_ScrollBar.SetScrollPos(m_List.GetTopItemIndex());
//		nRet = 1;
//		break;
//	case VK_DOWN:
//		nRet = m_List.GetCurSel();
//		if (nRet < m_List.GetCount() - 1)
//			m_List.SetCurSel(nRet + 1);
//		else if (nRet < 0)
//			m_List.SetCurSel(0);
//		m_ScrollBar.SetScrollPos(m_List.GetTopItemIndex());
//		nRet = 1;
//		break;
	default:
		nRet = 0;
		break;
	}
	return nRet;
}

void KUiNewPlayerStartMsg::OnClickButton(KWndButton* pBtn)
{
	if(pBtn == &m_Ok)
		OnOk();
	else if (pBtn == &m_NewerBtn)
	{
		m_List.Show();
		m_AdvanceList.Hide();
        m_NewerBtn.CheckButton(true);
        m_AdvanceBtn.CheckButton(false);
	}
	else if (pBtn == &m_AdvanceBtn)
	{
		m_List.Hide();
		m_AdvanceList.Show();
        m_NewerBtn.CheckButton(false);
        m_AdvanceBtn.CheckButton(true);
	}
}

//--------------------------------------------------------------------------
//	功能：取消
//--------------------------------------------------------------------------
void KUiNewPlayerStartMsg::OnOk()
{
	CloseWindow(true);
    KUiHelper::OpenWindow(true);
	if(g_pCoreShell)
	{
		if(g_pCoreShell->GetGameData(GDI_PLAYER_IS_MALE, 0, 0))
		{
			g_pCoreShell->OperationRequest(GOI_PLAYER_SET_PORTRAIT, 0, 2);
		}
		else
		{
			g_pCoreShell->OperationRequest(GOI_PLAYER_SET_PORTRAIT, 0, 3);
		}
	}
}
