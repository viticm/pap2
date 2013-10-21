/*******************************************************************************
File        : UiEquipmentRepairAlert.cpp
Creator     : Fyt(Fan Zhanpeng)
create data : 10-15-2003(mm-dd-yyyy)
Description : 物品修理警报
********************************************************************************/

#include "KWin32.h"
#include "KIniFile.h"
#include "CoreShell.h"
#include "GameDataDef.h"

#include "../elem/wnds.h"
#include "../Elem/WndMessage.h"
#include "../elem/mousehover.h"

#include "../UiBase.h"
#include "../UiSoundSetting.h"

#include "UiEquipmentRepairAlert.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

#define REPAIR_ITEM_ALERT_INI "修理警报.ini"

KUiEquipmentRepairAlert* KUiEquipmentRepairAlert::ms_pSelf = NULL;

extern iCoreShell*		g_pCoreShell;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

KUiEquipmentRepairAlert::KUiEquipmentRepairAlert()
{

}

KUiEquipmentRepairAlert::~KUiEquipmentRepairAlert()
{

}


/*********************************************************************
* 功能：打开窗口
**********************************************************************/
KUiEquipmentRepairAlert* KUiEquipmentRepairAlert::OpenWindow()
{
	if (ms_pSelf == NULL)
	{
		ms_pSelf = new KUiEquipmentRepairAlert;
		if (ms_pSelf)
			ms_pSelf->Initialize();
	}
	if (ms_pSelf)
	{
		UiSoundPlay(UI_SI_WND_OPENCLOSE);
		ms_pSelf->Hide();
		ms_pSelf->BringToTop();
	}
	return ms_pSelf;
}


/*********************************************************************
* 功能：如果窗口正被显示，则返回实例指针
**********************************************************************/
KUiEquipmentRepairAlert* KUiEquipmentRepairAlert::GetIfVisible()
{
	if (ms_pSelf && ms_pSelf->IsVisible())
		return ms_pSelf;
	return NULL;
}


/*********************************************************************
* 功能：关闭窗口，同时可以选则是否删除对象实例
**********************************************************************/
void KUiEquipmentRepairAlert::CloseWindow(bool bDestory)
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
void KUiEquipmentRepairAlert::Initialize()
{
	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);

	Wnd_AddWindow(this, WL_TOPMOST);
}


/*********************************************************************
* 功能：窗口函数
**********************************************************************/
int KUiEquipmentRepairAlert::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int nRet = 0;
	switch(uMsg)
	{
	case WM_MOUSEMOVE:
	case WM_MOUSEHOVER:
		g_MouseOver.SetMouseHoverInfo((void*)(KWndWindow*)this, 0,
				GET_X_LPARAM(nParam), GET_Y_LPARAM(nParam), false, FALSE);
		g_MouseOver.SetMouseHoverTitle(m_szTip, strlen(m_szTip), 0xffffffff);
		nRet = 1;
		break;

	default:
		Wnd_TransmitInputToGameSpace(uMsg, uParam, nParam);
	}
	return nRet;
}


/*********************************************************************
* 功能：载入界面方案
**********************************************************************/
void KUiEquipmentRepairAlert::LoadScheme(const char* pScheme)
{
	if(ms_pSelf)
	{
		char		Buff[128];
    	KIniFile	Ini;
    	sprintf(Buff, "%s\\%s", pScheme, REPAIR_ITEM_ALERT_INI);
    	if (Ini.Load(Buff))
    	{
			ms_pSelf->Init(&Ini, "Main");
			Ini.GetInteger("Main", "Yellow", 3, &ms_pSelf->m_nYellowAlertSignal);
			Ini.GetInteger("Main", "Red", 1, &ms_pSelf->m_nRedAlertSignal);
		}
	}
}


/*********************************************************************
* 功能：显示窗口
**********************************************************************/
void KUiEquipmentRepairAlert::Show(int nDur, char *szName)
{
    if(nDur > m_nYellowAlertSignal || nDur == -1)
	    Hide();
	else
	{
		KWndImage::Show();
		if(nDur > m_nRedAlertSignal)
		{
			SetTargetItemTip(szName);
    	    SetFrame(0);
		}
		else
		{
			SetTargetItemTip(szName, TRUE);
    		SetFrame(1);
		}
	}
}


/*********************************************************************
* 功能：如果窗体已经被创建，则返回对象实例，否则返回NULL
**********************************************************************/
KUiEquipmentRepairAlert* KUiEquipmentRepairAlert::GetIfCreate()
{
	if(ms_pSelf)
		return ms_pSelf;
	else
		return NULL;
}


/*********************************************************************
* 功能：设置要修理的那个装备的index
**********************************************************************/
int	KUiEquipmentRepairAlert::SetTargetItemTip(char *szName, int nbIsAlmostCrash)
{
	if(g_pCoreShell)
	{
		if(nbIsAlmostCrash)
		{
			sprintf(m_szTip, "你的 %s 看样子马上就要坏了。", szName);
		}
		else
		{
			sprintf(m_szTip, "你感到你的 %s 有了一些裂痕。", szName);
		}
		return 1;
	}
	return 0;
}
