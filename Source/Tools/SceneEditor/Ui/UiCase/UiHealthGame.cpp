/*******************************************************************************
File        : UiHealthGame.cpp
Creator     : Fyt(Fan Zhanpeng)
create data : 11-21-2003(mm-dd-yyyy)
Description : 健康游戏忠告
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

#include "UiSelServer.h"
#include "UiHealthGame.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

KUiHealthGame*			KUiHealthGame::ms_pSelf = NULL;

#define UI_HEALTH_GAME "健康游戏公告.ini"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

KUiHealthGame::KUiHealthGame()
{

}

KUiHealthGame::~KUiHealthGame()
{

}


/*********************************************************************
* 功能：打开窗口
**********************************************************************/
KUiHealthGame* KUiHealthGame::OpenWindow()
{
	if (ms_pSelf == NULL)
	{
		ms_pSelf = new KUiHealthGame;
		if (ms_pSelf)
			ms_pSelf->Initialize();
	}
	if (ms_pSelf)
	{
		ms_pSelf->BringToTop();
		ms_pSelf->Show();
	}
	return ms_pSelf;
}


/*********************************************************************
* 功能：如果窗口正被显示，则返回实例指针
**********************************************************************/
KUiHealthGame* KUiHealthGame::GetIfVisible()
{
	if (ms_pSelf && ms_pSelf->IsVisible())
		return ms_pSelf;
	return NULL;
}


/*********************************************************************
* 功能：关闭窗口，同时可以选则是否删除对象实例
**********************************************************************/
void KUiHealthGame::CloseWindow(bool bDestory)
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
void KUiHealthGame::Initialize()
{
	AddChild(&m_BtnClose);
	AddChild(&m_Text);

	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);

	Wnd_AddWindow(this);
}


/*********************************************************************
* 功能：载入界面方案
**********************************************************************/
void KUiHealthGame::LoadScheme(const char* pScheme)
{
	if(ms_pSelf)
	{
		char		Buff[128];
    	KIniFile	Ini;
    	sprintf(Buff, "%s\\%s", pScheme, UI_HEALTH_GAME);
    	if (Ini.Load(Buff))
    	{
			ms_pSelf->Init(&Ini, "Main");
			ms_pSelf->m_BtnClose.Init(&Ini, "Close");
			ms_pSelf->m_Text.Init(&Ini, "List");
		}
	}
}


/*********************************************************************
* 功能：处理界面消息的处理方法
**********************************************************************/
int KUiHealthGame::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int nRet = 0;

	switch(uMsg)
	{
		case WND_N_BUTTON_CLICK:
			if(uParam == (unsigned int)&m_BtnClose)
			{
				if(KUiSelServer::OpenWindow())
				{
    				CloseWindow();
				}
			}
			break;
		case WM_KEYDOWN:
			if (uParam  == VK_RETURN || uParam == VK_SPACE || uParam == VK_ESCAPE)
			{
				if(KUiSelServer::OpenWindow())
				{
    				CloseWindow();
				}
			}
			break;

		default:
			nRet = KWndWindow::WndProc(uMsg, uParam, nParam);
	}
	return nRet;
}
