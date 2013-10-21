/*****************************************************************************************
//	界面--选择附近的某个玩家
//	Copyright : Kingsoft 2003
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2003-2-26
*****************************************************************************************/
#include "KWin32.h"
#include "KIniFile.h"
#include "../elem/wnds.h"
#include "../Elem/WndMessage.h"
#include "../UiBase.h"
#include "UiSelPlayerNearby.h"
#include "../../../core/src/coreshell.h"
#include "../../../core/src/gamedatadef.h"
#include "../UiSoundSetting.h"

#include "UiGame.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

extern iCoreShell*		g_pCoreShell;

#define	SCHEME_INI	"选择附近玩家.ini"

KUiSelPlayerNearby* KUiSelPlayerNearby::m_pSelf = NULL;

//--------------------------------------------------------------------------
//	功能：打开窗口，返回唯一的一个类对象实例
//--------------------------------------------------------------------------
KUiSelPlayerNearby* KUiSelPlayerNearby::OpenWindow()
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiSelPlayerNearby;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
	{
		UiSoundPlay(UI_SI_WND_OPENCLOSE);
		m_pSelf->BringToTop();
		m_pSelf->UpdateData();
		m_pSelf->Show();
	}
	return m_pSelf;
}

KUiSelPlayerNearby::KUiSelPlayerNearby()
{
	m_pDataList = NULL;
	m_nPlayerCount = 0;
	m_nActionCount = 0;
}

int	KUiSelPlayerNearby::DoesHaveVisibleWnd()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return true;
	return false;
}

//初始化
void KUiSelPlayerNearby::Initialize()
{
	AddChild(&m_PlayerList);
	AddChild(&m_ListScroll);
	m_PlayerList.SetScrollbar(&m_ListScroll);
	AddChild(&m_InputEdit);
	AddChild(&m_RefreshBtn);
	AddChild(&m_CancelBtn);

	Wnd_AddWindow(this);

	char Buffer[256];
	g_UiBase.GetCurSchemePath(Buffer, sizeof(Buffer));
	strcat(Buffer, "\\"SCHEME_INI);
	KIniFile	Ini;
	if (Ini.Load(Buffer))
	{
		LoadScheme(&Ini);
	}
}

void KUiSelPlayerNearby::LoadActionList(KIniFile* pSetting)
{
	m_nActionCount = 0;

	int	i;
	if (pSetting)
	{
		pSetting->GetInteger("InterPlayerAction", "Count", 0, &m_nActionCount);
		if (m_nActionCount > 0)
		{
			if (m_nActionCount > MAX_NUM_BUTTON)
				m_nActionCount = MAX_NUM_BUTTON;
			char	szKey[16];
			for (i = 0; i < m_nActionCount; i++)
			{
				sprintf(szKey, "%d_Name", i);
				pSetting->GetString("InterPlayerAction", szKey, "",
					m_ActionList[i].szActionName,
					sizeof(m_ActionList[i].szActionName));
				sprintf(szKey, "%d_Id", i);
				pSetting->GetInteger("InterPlayerAction", szKey, -1,
						&m_ActionList[i].nId);
				m_ActionBtns[i].SetText(m_ActionList[i].szActionName);
			}
		}
	}
	for (i = 0;i < m_nActionCount; i++)
		AddChild(&m_ActionBtns[i]);
	for (i = m_nActionCount; i < MAX_NUM_BUTTON; i++)
		m_ActionBtns[i].LeaveAlone();
}

//--------------------------------------------------------------------------
//	功能：关闭窗口，同时可以选则是否删除对象实例
//--------------------------------------------------------------------------
void KUiSelPlayerNearby::CloseWindow(bool bDestroy)
{
	if (m_pSelf)
	{
		m_pSelf->Clear();
		if (bDestroy == false)
		{
			m_pSelf->Hide();
		}
		else
		{
			m_pSelf->Destroy();
			m_pSelf = NULL;
		}
	}
}

void KUiSelPlayerNearby::LoadScheme(const char* pScheme)
{
	if (m_pSelf)
	{
		char		Buff[128];
		KIniFile	Ini;
		sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI);
		if (Ini.Load(Buff))
		{
			m_pSelf->LoadScheme(&Ini);
		}
	}
}

void KUiSelPlayerNearby::LoadScheme(KIniFile* pIni)
{
	if (pIni)
	{
		Init(pIni, "Main");
		m_PlayerList.Init(pIni, "PlayerList");
		m_ListScroll.Init(pIni, "Scroll");
		m_CancelBtn	.Init(pIni, "Close");
		m_RefreshBtn.Init(pIni, "Refresh");
		m_InputEdit.Init(pIni, "InputEdit");

		char	szSection[16];
		for (int i = 0; i < MAX_NUM_BUTTON; i++)
		{
			sprintf(szSection, "ActionBtn_%d", i);
			m_ActionBtns[i].Init(pIni, szSection);
		}
		LoadActionList(pIni);
	}
}

//窗口函数
int KUiSelPlayerNearby::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int nRet = 0;
	switch(uMsg)
	{
	case WND_N_BUTTON_CLICK:
		if (uParam == (unsigned int)(KWndWindow*)&m_CancelBtn)
			CloseWindow(false);
		else if (uParam == (unsigned int)(KWndWindow*)&m_RefreshBtn)
			UpdateData();
		else
		{
			for (int i = 0; i < m_nActionCount; i++)
			{
				if (uParam == (unsigned int)(KWndWindow*)&m_ActionBtns[i])
				{
					OnClickXButton(i);
					break;
				}
			}
		}
		break;
	case WND_N_SCORLLBAR_POS_CHANGED:
		if (uParam == (unsigned int)(KWndWindow*)&m_ListScroll)
			m_PlayerList.SetTopItemIndex(nParam);
		break;
	case WND_N_LIST_ITEM_SEL:
		if (uParam == (unsigned int)(KWndWindow*)&m_PlayerList)
		{
			char szCurText[256];
			szCurText[0] = 0;
			m_PlayerList.GetString(nParam, szCurText);
			m_InputEdit.SetText(szCurText);
		}
		break;
	default:
		nRet = KWndShowAnimate::WndProc(uMsg, uParam, nParam);
	}
	return nRet;
}

void KUiSelPlayerNearby::OnClickXButton(int nAction)
{
	char	Buffer[64];
	KUiMsgParam	Param;
	Param.nMsgLength = m_InputEdit.GetText(Buffer, sizeof(Buffer), true);
	if (Param.nMsgLength && nAction >= 0 && nAction < m_nActionCount)
	{
		if (m_ActionList[nAction].nId >= 0)
		{
			KUiPlayerItem player;
			player.Name[0] = 0;
			for (int i = 0; i < m_nPlayerCount; i++)
			{
				if (strcmp(Buffer, m_pDataList[i].Name) == 0)
				{
					player = m_pDataList[i];
					break;
				}
			}
			if (player.Name[0] == 0)
			{
				strcpy(player.Name, Buffer);
				player.nIndex = -1;
				player.uId = 0;
				player.nData = 0;
			}
			ProcessPeople(&player, m_ActionList[nAction].nId);
			UpdateData();
		}
	}
}

void KUiSelPlayerNearby::UpdateData()
{
	Clear();
	int nCount = g_pCoreShell->GetGameData(GDI_NEARBY_PLAYER_LIST, 0, 0);
	if (nCount)
	{
		m_pDataList = (KUiPlayerItem*)malloc(sizeof(KUiPlayerItem) * nCount);
		if (m_pDataList)
		{
			KUiPlayerItem Item;
			m_nPlayerCount = g_pCoreShell->GetGameData(GDI_NEARBY_PLAYER_LIST, (unsigned int)m_pDataList, nCount);
			_ASSERT(m_nPlayerCount == nCount);
			m_PlayerList.SetContent((unsigned char*)m_pDataList, m_nPlayerCount,
				sizeof(KUiPlayerItem), (char*)(&Item.Name) - (char*)&Item);
		}
	}
}

void KUiSelPlayerNearby::Clear()
{
	if (m_pDataList)
	{
		m_PlayerList.SetContent(0, 0, 0, 0);
		free(m_pDataList);
		m_pDataList = NULL;
		m_nPlayerCount = 0;
	}
}