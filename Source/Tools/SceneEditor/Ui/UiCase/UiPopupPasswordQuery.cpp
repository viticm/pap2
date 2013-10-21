/********************************************************************
File        : UiPopupPasswordQuery.cpp
Creator     : Fyt(Fan Zhanpeng)
create data : 08-30-2003(mm-dd-yyyy)
Description : 询问密码窗口
*********************************************************************/

#include "KWin32.h"
#include "KIniFile.h"
#include "KSG_MD5_String.h"

#include "../elem/wnds.h"
#include "../Elem/WndMessage.h"

#include "../UiBase.h"
#include "../UiSoundSetting.h"

#include "UiPopupPasswordQuery.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

#define POPUP_PASSWORD_QUERY_INI "密码询问窗口.ini"

KUiPopupPasswordQuery* KUiPopupPasswordQuery::ms_pSelf = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

KUiPopupPasswordQuery::KUiPopupPasswordQuery()
{

}

KUiPopupPasswordQuery::~KUiPopupPasswordQuery()
{

}


/*********************************************************************
* 功能：打开窗口
**********************************************************************/
KUiPopupPasswordQuery* KUiPopupPasswordQuery::OpenWindow(KWndWindow *pCaller, unsigned int uParam)
{
	if(pCaller)
	{
    	if (ms_pSelf == NULL)
    	{
		    ms_pSelf = new KUiPopupPasswordQuery;
		    if (ms_pSelf)
    			ms_pSelf->Initialize();
    	}
    	if (ms_pSelf)
    	{
			ms_pSelf->m_pCaller = pCaller;
			ms_pSelf->m_uParam  = uParam;
		    UiSoundPlay(UI_SI_WND_OPENCLOSE);
		    ms_pSelf->BringToTop();
		    ms_pSelf->Show();
			Wnd_SetExclusive((KWndWindow *)ms_pSelf);
	    }
	}
	return ms_pSelf;
}


/*********************************************************************
* 功能：如果窗口正被显示，则返回实例指针
**********************************************************************/
KUiPopupPasswordQuery* KUiPopupPasswordQuery::GetIfVisible()
{
	if (ms_pSelf && ms_pSelf->IsVisible())
		return ms_pSelf;
	return NULL;
}


/*********************************************************************
* 功能：关闭窗口，同时可以选则是否删除对象实例
**********************************************************************/
void KUiPopupPasswordQuery::CloseWindow(bool bDestory)
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
void KUiPopupPasswordQuery::Initialize()
{
	AddChild(&m_Password);
	AddChild(&m_Confirm);

	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);

	Wnd_AddWindow(this);
}


/*********************************************************************
* 功能：载入界面方案
**********************************************************************/
void KUiPopupPasswordQuery::LoadScheme(const char* pScheme)
{
	if(ms_pSelf)
	{
    	char		Buff[128];
    	KIniFile	Ini;
    	sprintf(Buff, "%s\\%s", pScheme, POPUP_PASSWORD_QUERY_INI);
    	if (Ini.Load(Buff))
    	{
		    ms_pSelf->Init(&Ini, "Main");

		    ms_pSelf->m_Password.Init(&Ini, "Password");
			ms_pSelf->m_Confirm.Init(&Ini, "BtnConfirm");
		}
		/*--------------------*/
	}
	/*--------------------*/
}


/*********************************************************************
* 功能：窗口函数
**********************************************************************/
int KUiPopupPasswordQuery::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	switch(uMsg)
	{
	case WND_N_BUTTON_CLICK:
		if(uParam == (unsigned int)&m_Confirm)
		{
			OnConfirm();
		}
		break;

	case WND_N_EDIT_SPECIAL_KEY_DOWN:
		if(uParam == (unsigned int)&m_Password && nParam == VK_RETURN)
		{
			OnConfirm();
		}
		break;

	default:
		return KWndImage::WndProc(uMsg, uParam, nParam);
	}
	return 1;
}


/*********************************************************************
* 功能：窗口函数
**********************************************************************/
void KUiPopupPasswordQuery::OnConfirm()
{
	if(m_pCaller)
	{
		char Buff[16], szMD5[64];

		m_Password.GetText(Buff, sizeof(Buff), TRUE);
		if(Buff[0])
		{
    		KSG_StringToMD5String(szMD5, Buff);
			Wnd_ReleaseExclusive((KWndWindow*)this);
	    	m_pCaller->WndProc(WND_M_OTHER_WORK_RESULT, m_uParam, (int)szMD5);
		}
		else
		{
			return;
		}
	}
	m_Password.ClearText();
	CloseWindow();
}
