/*****************************************************************************************
//	界面--聊天选项/状态设置
//	Copyright : Kingsoft 2003
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2003-3-5
*****************************************************************************************/
#include "KWin32.h"
#include "KIniFile.h"
#include "../elem/wnds.h"
#include "../Elem/WndMessage.h"
#include "UiChatStatus.h"
#include "../UiBase.h"
#include "../../../core/src/coreshell.h"
#include "../../../Engine/src/Text.h"
#include "../UiSoundSetting.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

extern iCoreShell*		g_pCoreShell;

#define	SCHEME_INI		"聊天状态设置窗口.ini"

KUiChatStatus* KUiChatStatus::m_pSelf = NULL;

//--------------------------------------------------------------------------
//	功能：打开窗口，返回唯一的一个类对象实例
//--------------------------------------------------------------------------
KUiChatStatus* KUiChatStatus::OpenWindow()
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiChatStatus;
		if (m_pSelf)
		{
			UiSoundPlay(UI_SI_WND_OPENCLOSE);
			m_pSelf->Initialize();
			m_pSelf->Show();
		}
	}
	return m_pSelf;
}

//--------------------------------------------------------------------------
//	功能：如果窗口正被显示，则返回实例指针
//--------------------------------------------------------------------------
KUiChatStatus*	KUiChatStatus::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	return NULL;
}

//--------------------------------------------------------------------------
//	功能：关闭窗口，同时可以选则是否删除对象实例
//--------------------------------------------------------------------------
void KUiChatStatus::CloseWindow()
{
	if (m_pSelf)
	{
		m_pSelf->Destroy();
		m_pSelf = NULL;
	}
}

//初始化
void KUiChatStatus::Initialize()
{
	AddChild(&m_OnLineBtn);
	AddChild(&m_LeaveBtn);
	AddChild(&m_BusyBtn);
	AddChild(&m_AutoReplyBtn);
	AddChild(&m_HideBtn);
	AddChild(&m_CloseBtn);
	AddChild(&m_AddMsg);
	AddChild(&m_RemoveMsg);
	AddChild(&m_SetDefaultMsg);
	AddChild(&m_ReplyMsgEdit);
	AddChild(&m_MsgList);
	AddChild(&m_MsgScroll);
	m_MsgList.SetScrollbar(&m_MsgScroll);

	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);


	m_Style &= ~WND_S_VISIBLE;

	Wnd_AddWindow(this);
}

//载入界面方案
void KUiChatStatus::LoadScheme(const char* pScheme)
{
	char		Buff[128];
	KIniFile	Ini;
	if (m_pSelf)
	{
		sprintf(Buff, "%s\\"SCHEME_INI, pScheme);
		if (Ini.Load(Buff))
		{
			m_pSelf->Init(&Ini, "Main");
			m_pSelf->m_OnLineBtn.Init(&Ini, "OnLineBtn");
			m_pSelf->m_LeaveBtn.Init(&Ini, "LeaveBtn");
			m_pSelf->m_BusyBtn.Init(&Ini, "BusyBtn");
			m_pSelf->m_AutoReplyBtn.Init(&Ini, "AutoReplyBtn");
			m_pSelf->m_HideBtn.Init(&Ini, "HideBtn");
			m_pSelf->m_CloseBtn.Init(&Ini, "CloseBtn");
			m_pSelf->m_AddMsg.Init(&Ini, "AddMsg");
			m_pSelf->m_RemoveMsg.Init(&Ini, "RemoveMsg");
			m_pSelf->m_SetDefaultMsg.Init(&Ini, "SetDefaultMsg");
			m_pSelf->m_ReplyMsgEdit.Init(&Ini, "ReplyMsgEdit");
			m_pSelf->m_MsgList.Init(&Ini, "MsgList");
			m_pSelf->m_MsgScroll.Init(&Ini, "Scroll");
		}
	}
}


//窗口函数
int KUiChatStatus::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	switch(uMsg)
	{
	case WND_N_BUTTON_CLICK:
		if (uParam == (unsigned int)(KWndWindow*)&m_OnLineBtn)
		{
			m_OnLineBtn.CheckButton(true);
			m_LeaveBtn.CheckButton(false);
			m_BusyBtn.CheckButton(false);
			m_AutoReplyBtn.CheckButton(false);
			m_HideBtn.CheckButton(false);
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_LeaveBtn)
		{
			m_OnLineBtn.CheckButton(false);
			m_LeaveBtn.CheckButton(true);
			m_BusyBtn.CheckButton(false);
			m_AutoReplyBtn.CheckButton(false);
			m_HideBtn.CheckButton(false);
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_BusyBtn)
		{
			m_OnLineBtn.CheckButton(false);
			m_LeaveBtn.CheckButton(false);
			m_BusyBtn.CheckButton(true);
			m_AutoReplyBtn.CheckButton(false);
			m_HideBtn.CheckButton(false);
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_AutoReplyBtn)
		{
			m_OnLineBtn.CheckButton(false);
			m_LeaveBtn.CheckButton(false);
			m_BusyBtn.CheckButton(false);
			m_AutoReplyBtn.CheckButton(true);
			m_HideBtn.CheckButton(false);
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_HideBtn)
		{
			m_OnLineBtn.CheckButton(false);
			m_LeaveBtn.CheckButton(false);
			m_BusyBtn.CheckButton(false);
			m_AutoReplyBtn.CheckButton(false);
			m_HideBtn.CheckButton(true);
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_AddMsg)
			OnAddMsg();
		else if (uParam == (unsigned int)(KWndWindow*)&m_RemoveMsg)
			m_MsgList.RemoveAMessage(m_MsgList.GetCurSel());
		else if (uParam == (unsigned int)(KWndWindow*)&m_SetDefaultMsg)
			OnSetDefMsg();
		else if (uParam == (unsigned int)(KWndWindow*)&m_CloseBtn)
			CloseWindow();
		break;
	case WND_N_EDIT_SPECIAL_KEY_DOWN:
		if (uParam == VK_RETURN)
			OnAddMsg();
		break;
	case WND_N_LIST_ITEM_SEL:
		OnSelMsg(nParam);
		break;
	case WND_N_SCORLLBAR_POS_CHANGED:
		if (uParam = (unsigned int)(KWndWindow*)(&m_MsgScroll))
			m_MsgList.SetFirstShowLine(nParam);
		break;
	default:
		return KWndImage::WndProc(uMsg, uParam, nParam);
	}
	return 0;
}

// 加入一条消息
void KUiChatStatus::OnAddMsg()
{
	char	Buffer[512];
	int nLen = m_ReplyMsgEdit.GetText(Buffer, 512, true);
	if (nLen)
	{
		nLen = TEncodeText(Buffer, nLen);
		if (nLen)
			m_MsgList.AddOneMessage(Buffer, nLen);
		m_ReplyMsgEdit.SetText("");
	}
}

void KUiChatStatus::OnSelMsg(int nIndex)
{
	char	Buffer[512];
	int nLen = m_MsgList.GetOneMessage(nIndex, Buffer, 512, true);
	if (nLen > 0)
	{
		m_ReplyMsgEdit.SetText(Buffer);
	}
}

void KUiChatStatus::OnSetDefMsg()
{
}