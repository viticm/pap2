/*******************************************************************************
File        : UiTongAssignTitle.cpp
Creator     : Fyt(Fan Zhanpeng)
create data : 10-31-2003(mm-dd-yyyy)
Description : 管理帮会的界面
********************************************************************************/

#include "KWin32.h"
#include "KIniFile.h"
#include "CoreShell.h"
#include "KPlayerDef.h"

#include "../elem/wnds.h"
#include "../elem/popupmenu.h"
#include "../elem/wndmessage.h"

#include "../UiBase.h"
#include "../UiSoundSetting.h"

#include "../../../Engine/src/KFilePath.h"

#include "UiTongAssignTitle.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

KUiTongAssignTitle*  KUiTongAssignTitle::ms_pSelf = NULL;
extern iCoreShell* g_pCoreShell;


#define TONG_ASSIGN_TITLE_INI "帮会指派称号.ini"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

KUiTongAssignTitle::KUiTongAssignTitle()
{
	m_pData = NULL;
	m_nDataCount = 0;
}

KUiTongAssignTitle::~KUiTongAssignTitle()
{

}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction End
//////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------
  /*__(@_                功能：打开窗口
 /     ) \______________________________________________________
(_)@8@8{}<______________________________________________________>
       )_/
      (*/
KUiTongAssignTitle* KUiTongAssignTitle::OpenWindow()
{
	if(g_pCoreShell)
	{
    	if (ms_pSelf == NULL)
    	{
		    ms_pSelf = new KUiTongAssignTitle;
		    if (ms_pSelf)
    			ms_pSelf->Initialize();
    	}
    	if (ms_pSelf)
    	{
		    UiSoundPlay(UI_SI_WND_OPENCLOSE);
			ms_pSelf->BringToTop();
			ms_pSelf->Show();
			Wnd_SetExclusive(ms_pSelf);
	    }
	}
	return ms_pSelf;
}


//-------------------------------------------------------------------------------------
  /*__(@_        功能：如果窗口正被显示，则返回实例指针
 /     ) \______________________________________________________
(_)@8@8{}<______________________________________________________>
       )_/
      (*/
KUiTongAssignTitle* KUiTongAssignTitle::GetIfVisible()
{
	if (ms_pSelf && ms_pSelf->IsVisible())
		return ms_pSelf;
	return NULL;
}


//-------------------------------------------------------------------------------------
  /*__(@_    功能：关闭窗口，同时可以选则是否删除对象实例
 /     ) \______________________________________________________
(_)@8@8{}<______________________________________________________>
       )_/
      (*/
void KUiTongAssignTitle::CloseWindow(bool bDestory)
{
	if (ms_pSelf)
	{
		Wnd_ReleaseExclusive(ms_pSelf);
		ms_pSelf->Hide();
		if (bDestory)
		{
			ms_pSelf->Destroy();
			ms_pSelf = NULL;
		}
	}
}


//-------------------------------------------------------------------------------------
  /*__(@_               功能：载入界面方案
 /     ) \______________________________________________________
(_)@8@8{}<______________________________________________________>
       )_/
      (*/
void KUiTongAssignTitle::LoadScheme(const char* pScheme)
{
	if(ms_pSelf)
	{
		char		Buff[128];
		KIniFile	Ini;
		sprintf(Buff, "%s\\%s", pScheme, TONG_ASSIGN_TITLE_INI);

		if(Ini.Load(Buff))
		{
			ms_pSelf->Init(&Ini, "Main");

			ms_pSelf->m_BtnTarget.Init(&Ini, "Target");
			ms_pSelf->m_EditTitle.Init(&Ini, "EditTitle");
			ms_pSelf->m_BtnConfirm.Init(&Ini, "Confirm");
			ms_pSelf->m_BtnCancel.Init(&Ini, "Cancel");
		}
	}
}


//-------------------------------------------------------------------------------------
  /*__(@_                功能：初始化
 /     ) \______________________________________________________
(_)@8@8{}<______________________________________________________>
       )_/
      (*/
void KUiTongAssignTitle::Initialize()
{
	//AddChild(&m_BtnTarget);
	AddChild(&m_EditTitle);
	AddChild(&m_BtnConfirm);
	AddChild(&m_BtnCancel);

	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);

	Wnd_AddWindow(this);
}


//-------------------------------------------------------------------------------------
  /*__(@_        功能：窗口函数，处理这个界面的消息
 /     ) \______________________________________________________
(_)@8@8{}<______________________________________________________>
       )_/
      (*/
int KUiTongAssignTitle::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int nRet = 0;
	switch(uMsg)
	{
	case WND_N_BUTTON_CLICK:
		if(uParam == (unsigned int)&m_BtnTarget)
		{
			int nX, nY, nMenuHeight, nWidth, nHeight;
			m_BtnTarget.GetAbsolutePos(&nX, &nY);
			m_BtnTarget.GetSize(&nWidth, &nHeight);
			nMenuHeight = KPopupMenu::GetHeight();
			nX += nWidth;
			nY -= m_nDataCount * nMenuHeight / 2;
			if(nY < 20)
			{
				nY = 20;
			}
			PopupSelectMenu(nX, nY);
		}
		else if(uParam == (unsigned int)&m_BtnConfirm && g_pCoreShell)
		{
			char szBuf[64];
			KTongOperationParam TongParam;
			TongParam.eOper = TONG_ACTION_TITLE;
			TongParam.nData[0] = m_nFigure;
			TongParam.nData[1] = m_nSelected;
			m_EditTitle.GetText(szBuf, sizeof(szBuf), TRUE);
			if(szBuf[0])
			{
				strcpy(TongParam.szPassword, szBuf);
				g_pCoreShell->TongOperation(GTOI_TONG_ACTION, (unsigned int)&TongParam, 0);
				CloseWindow();
			}
		}
		else if(uParam == (unsigned int)&m_BtnCancel)
		{
			CloseWindow();
		}
		break;

	case WND_M_MENUITEM_SELECTED:
		if((nParam >> 16) == RESULT_SELECT_TARGET_MENU && m_pData)
		{
			char szName[64];
			int i = nParam & 0xffff;
			if(i >= 0 && i < m_nDataCount)
			{
				m_SelectData = m_pData[i];
				m_nSelected  = i;
				m_EditTitle.SetText(m_SelectData.szAgname);
				m_BtnTarget.SetText(itoa(i + 1, szName, 10));
			}
		}
		break;

	default:
		nRet = KWndWindow::WndProc(uMsg, uParam, nParam);
		break;
	}
	return nRet;
}


//-------------------------------------------------------------------------------------
  /*__(@_           功能：把数据注入到这个界面中
 /     ) \______________________________________________________
(_)@8@8{}<______________________________________________________>
       )_/
      (*/
void KUiTongAssignTitle::InjectData(KTongMemberItem *pData, int nDataCount, int nFigure, int nSelect)
{
	if(m_pData)
	{
		delete(m_pData);
		m_pData = NULL;
	}
	if(nDataCount > 0 && pData)
	{
		m_pData = new KTongMemberItem[nDataCount];
		if(m_pData)
		{
			memcpy(m_pData, pData, sizeof(KTongMemberItem) * nDataCount);
			m_nDataCount = nDataCount;
			if(nSelect < 0)
			{
				nSelect = 0;
			}
			else if(nSelect >= m_nDataCount)
			{
				nSelect = m_nDataCount - 1;
			}
			m_SelectData = m_pData[nSelect];
			m_nSelected  = nSelect;
			m_nFigure    = nFigure;
		}
	}
}


//-------------------------------------------------------------------------------------
  /*__(@_           功能：弹出目标对象选择框
 /     ) \______________________________________________________
(_)@8@8{}<______________________________________________________>
       )_/
      (*/
void KUiTongAssignTitle::PopupSelectMenu(int nX, int nY)
{
	if (m_nDataCount > 0)
	{
		KPopupMenuData* pMenuData = (KPopupMenuData*)malloc(MENU_DATA_SIZE(m_nDataCount));
		if (pMenuData)
		{
			char szItem[128];
			KPopupMenu::InitMenuData(pMenuData, m_nDataCount);
			for (int i = 0;i < m_nDataCount;i++)
			{
				sprintf(szItem, "%-2d.%12s %s", i + 1, m_pData[i].Name, m_pData[i].szAgname);
				strcpy(pMenuData->Items[i].szData, szItem);
				pMenuData->Items[i].uDataLen = strlen(pMenuData->Items[i].szData);
			}
			pMenuData->nX = nX;
			pMenuData->nY = nY;
			KPopupMenu::Popup(pMenuData, (KWndWindow*)this, RESULT_SELECT_TARGET_MENU);
		}
	}
}


  /*__(@_             功能：更新界面显示数据
 /     ) \______________________________________________________
(_)@8@8{}<______________________________________________________>
       )_/
      (*/
void KUiTongAssignTitle::UpdateView()
{
	if(m_pData && m_nSelected >= 0)
	{
		char szBuf[64];

		m_BtnTarget.SetText(itoa(m_nSelected + 1, szBuf, 10));
		m_EditTitle.SetText(m_pData[m_nSelected].szAgname);
		m_SelectData = m_pData[m_nSelected];
	}
}
