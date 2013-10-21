/*****************************************************************************************
//	界面--聊天主界面
//	Copyright : Kingsoft 2003
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2003-3-10
*****************************************************************************************/
#include "KWin32.h"
#include "KIniFile.h"
#include "../Elem/WndMessage.h"
#include "UiChatCentre.h"
#include "../Elem/Wnds.h"
#include "UiChatStatus.h"
#include "UiGame.h"
#include "../UiBase.h"
#include "../../login/login.h"
#include "UiGetString.h"
#include "UiSelPlayerNearby.h"
#include "../UiSoundSetting.h"
#include "../../../core/src/coreshell.h"
#include "../../../Represent/iRepresent/iRepresentShell.h"
#include "../../../Represent/iRepresent/KRepresentUnit.h"
#include "../../../Engine/Src/Text.h"
#include "../../../core/src/GameDataDef.h"
#include "KProtocol.h"
#include "KRelayProtocol.h"
#include "KTongProtocol.h"
#include "malloc.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

extern iRepresentShell*	g_pRepresentShell;
extern iCoreShell*		g_pCoreShell;
extern KUiGameSpace		g_WndGameSpace;

#define	SCHEME_INI					"聊天主窗口.ini"

#define	GROUP_MENU_SELECT_BASE		0x100
#define	FRIEND_MENU_SELECT_BASE		0x200
#define	SEL_GROUP_MENU_SELECT_BASE	0x300
#define	SEL_UNIT_MENU_SELECT_BASE	0x400

enum GROUP_MENU_ITEM
{
	GROUP_MENU_ITEM_NEW = 0,
	GROUP_MENU_ITEM_RENAME,
	GROUP_MENU_ITEM_DELETE,
};

enum FRIEND_MENU_ITEM
{
	FRIEND_MENU_ITEM_SEND_MSG = 0,
	FRIEND_MENU_ITEM_DELETE,
	FRIEND_MENU_ITEM_MOVE,
};

enum INPUT_STRING_PARAM
{
	ISP_NEW_GROUP		= 0x100,
	ISP_RENAME_GROUP	= 0x200,
};

#define DEF_GROUP_MENU_ITEM_COUNT	1

const char* l_pGrupMenuItemTitle[GROUP_MENU_ITEM_COUNT] =
{
	 "新建组", "重命名组",	"删除组"
};
const char* l_pFriendMenuItemTitle[FRIEND_MENU_ITEM_COUNT] =
{
	"发送消息",	"删除", "移动到其它组"
};

#define FRIEND_INDEX	0
#define FRIEND_UNITNAME "好友"

#define BROTHER_INDEX	1
#define BROTHER_UNITNAME "亲人"

#define ENEMY_INDEX		2
#define ENEMY_UNITNAME	 "仇人"

#define MATE_INDEX		3
#define MATE_UNITNAME	"配偶"

#define DEFAULT_GROUPNAME	"我的"

#define ONLINE_NAME	"online"
#define OFFLINE_NAME	"offline"

KUiChatCentre* KUiChatCentre::m_pSelf = NULL;
ServerNotify KUiChatCentre::ms_SNotify;


int	ServerNotify::CreateUnit(const char* Unit)
{
	AddinNotify::CreateUnit(Unit);

	SendSyncData(Unit);

	return 0;
}

int	ServerNotify::RenameUnitGroup(const char* Unit, const char* Name, const char* NewName, const STRINGLIST& friends)
{
	AddinNotify::RenameUnitGroup(Unit, Name, NewName, friends);

	if (NewName && NewName[0])
		SendNotifyGroupFriend(Unit, std::string(NewName), friends);

	return 0;
}

int	ServerNotify::DeleteUnitGroup(const char* Unit, const char* Name, const STRINGLIST& friends)
{
	AddinNotify::DeleteUnitGroup(Unit, Name, friends);

	if (Name && Name[0])
		SendNotifyGroupFriend(Unit, "", friends);

	return 0;
}

int	ServerNotify::MoveUnitGroup(const char* Unit, const char* Name, const char* Name2, const STRINGLIST& friends)
{
	AddinNotify::MoveUnitGroup(Unit, Name, Name2, friends);

	if (Name2 && Name2[0])
		SendNotifyGroupFriend(Unit, std::string(Name2), friends);

	return 0;
}

int	ServerNotify::DeleteUnitMember(const char* Unit, const char* Name)
{
	AddinNotify::DeleteUnitMember(Unit, Name);

	SendNotifyDeleteFriend(Unit, Name);

	return 0;
}

int	ServerNotify::MoveUnitMember(const char* Unit, const char* Name, const char* Group)
{
	AddinNotify::MoveUnitMember(Unit, Name, Group);

	if (Name && Name[0])
	{
		STRINGLIST friends;
		friends.push_back(Name);
		SendNotifyGroupFriend(Unit, Group, friends);
	}

	return 0;
}

void ServerNotify::SendSyncData(const char* Unit)
{
	if (m_nRef <= 0)
		return;
	static int nSend = 0;	//公用的初始化只能做1次

	if (g_pCoreShell && nSend == 0)
	{
		size_t asksize = sizeof(ASK_SYNCFRIENDLIST);
		size_t pckgsize = sizeof(tagExtendProtoHeader) + asksize;

		tagExtendProtoHeader* pExHeader = (tagExtendProtoHeader*)_alloca(pckgsize);
		pExHeader->ProtocolType = c2s_extendfriend;
		pExHeader->wLength = pckgsize - 1;

		ASK_SYNCFRIENDLIST* pCccCmd = (ASK_SYNCFRIENDLIST*)(pExHeader + 1);
		pCccCmd->ProtocolFamily = pf_friend;
		pCccCmd->ProtocolID = friend_c2s_asksyncfriendlist;
		pCccCmd->pckgid = -1;
		pCccCmd->full = true;

		g_pCoreShell->SendNewDataToServer(pExHeader, pckgsize);
	}
	nSend = (nSend + 1) % m_nRef;
}

void ServerNotify::SendNotifyGroupFriend(const char* Unit, const std::string& group, const STRINGLIST& friends)
{
	if (m_nRef <= 0)
		return;
	if (Unit == NULL)
		return;

	if (friends.empty())
		return;

	std::string sendgname = Unit;
	sendgname += "\n";
	sendgname += group;

	static const size_t max_packagesize = 1024 - 32 - sizeof(DWORD) * 2;
	char buffer[max_packagesize];	//max package size
	size_t basesize = sizeof(tagExtendProtoHeader) + sizeof(GROUP_FRIEND);

	tagExtendProtoHeader* pExHeader = (tagExtendProtoHeader*)buffer;
	pExHeader->ProtocolType = c2s_extendfriend;

	GROUP_FRIEND* pGf = (GROUP_FRIEND*)(pExHeader + 1);
	pGf->ProtocolFamily = pf_friend;
	pGf->ProtocolID = friend_c2s_groupfriend;

	buffer[basesize++] = specGroup;
	strcpy(buffer + basesize, sendgname.c_str());
	basesize += sendgname.size() + 1;


	size_t cursor = basesize;

	for (STRINGLIST::const_iterator itFriend = friends.begin(); itFriend != friends.end(); itFriend++)
	{
		std::string dst = *itFriend;
		size_t appendsize = 1 + dst.size() + 1;

		if (cursor + appendsize + 1 + 1 > max_packagesize)
		{
			buffer[cursor++] = specOver;
			pExHeader->wLength = cursor - 1;

			g_pCoreShell->SendNewDataToServer(buffer, cursor);

			cursor = basesize;
		}

		buffer[cursor] = specRole;
		strcpy(buffer + 1 + cursor, dst.c_str());
		cursor += appendsize;
	}

	if (cursor > basesize)
	{
		buffer[cursor++] = specOver;
		pExHeader->wLength = cursor - 1;

		g_pCoreShell->SendNewDataToServer(buffer, cursor);
	}
}

void ServerNotify::SendNotifyDeleteFriend(const char* Unit, const char* Name)
{
	if (m_nRef <= 0)
		return;

	if (Name == NULL || Name[0] == 0)
		return;

	if (g_pCoreShell)
	{
		size_t asksize = sizeof(ERASE_FRIEND);
		size_t pckgsize = sizeof(tagExtendProtoHeader) + asksize;

		tagExtendProtoHeader* pExHeader = (tagExtendProtoHeader*)_alloca(pckgsize);
		pExHeader->ProtocolType = c2s_extendfriend;
		pExHeader->wLength = pckgsize - 1;

		ERASE_FRIEND* pCccCmd = (ERASE_FRIEND*)(pExHeader + 1);
		pCccCmd->ProtocolFamily = pf_friend;
		pCccCmd->ProtocolID = friend_c2s_erasefriend;
		strncpy(pCccCmd->friendrole, Name, _NAME_LEN);

		g_pCoreShell->SendNewDataToServer(pExHeader, pckgsize);
	}
}

///////////////////////////////////////////////////////////////////////////////

KUiChatCentre::KUiChatCentre()
{
	m_nNumValidGroup = 0;
	m_nNumFriendInList = 0;
	m_pFriendList = NULL;
	memset(&m_bValidGroup, 0, sizeof(m_bValidGroup));
	m_uFriendOffLineColor = 0;
	m_bPopupMenu = false;
	m_pSelGrupMenu = NULL;

	int	i;
	KPopupMenuData* pMenuData = (KPopupMenuData*)(&m_GroupMenuData);
	KPopupMenu::InitMenuData(pMenuData, GROUP_MENU_ITEM_COUNT);
	for (i = 0; i < GROUP_MENU_ITEM_COUNT; i++)
	{
		strcpy(pMenuData->Items[i].szData, l_pGrupMenuItemTitle[i]);
		pMenuData->Items[i].uDataLen = strlen(pMenuData->Items[i].szData);
	}
	pMenuData = (KPopupMenuData*)(&m_FriendMenuData);
	KPopupMenu::InitMenuData(pMenuData, FRIEND_MENU_ITEM_COUNT);
	for (i = 0; i < FRIEND_MENU_ITEM_COUNT; i++)
	{
		strcpy(pMenuData->Items[i].szData, l_pFriendMenuItemTitle[i]);
		pMenuData->Items[i].uDataLen = strlen(pMenuData->Items[i].szData);
	}

	m_nMaxVisibleLine = 0;
	m_nMinVisibleLine = 0;
	m_nNumVisibleLine = 0;

	m_nLineHeight = 8;
	m_nFirstVisibleLine = 0;
	memset(&m_ListBgImage, 0, sizeof(m_ListBgImage));

	m_ListBgImgLeft = 0;
	m_ListBgImgTop = 0;
	m_nTailImgDragOffsetY = 0;

	m_nCurentFriendUnit = FRIEND_INDEX;
	ms_SNotify.m_nRef = 0;

	//初始化好友
	m_AllFriendUnits[FRIEND_INDEX].m_UnitName = FRIEND_UNITNAME;
	ClearUnitData(FRIEND_INDEX);
	m_AllFriendUnits[FRIEND_INDEX].m_pNotify = &ms_SNotify;
	ms_SNotify.m_nRef++;

	//初始化亲友
	m_AllFriendUnits[BROTHER_INDEX].m_UnitName = BROTHER_UNITNAME;
	ClearUnitData(BROTHER_INDEX);
	m_AllFriendUnits[BROTHER_INDEX].m_pNotify = &ms_SNotify;
	ms_SNotify.m_nRef++;

	//初始化仇人
	m_AllFriendUnits[ENEMY_INDEX].m_UnitName = ENEMY_UNITNAME;
	ClearUnitData(ENEMY_INDEX);
	m_AllFriendUnits[ENEMY_INDEX].m_pNotify = &ms_SNotify;
	ms_SNotify.m_nRef++;

	//初始化配偶
	m_AllFriendUnits[MATE_INDEX].m_UnitName = MATE_UNITNAME;
	ClearUnitData(MATE_INDEX);
	m_AllFriendUnits[MATE_INDEX].m_pNotify = &ms_SNotify;
	ms_SNotify.m_nRef++;

	m_nFriendUnitCount = 4;
}

//--------------------------------------------------------------------------
//	功能：如果窗口正被显示，则返回实例指针
//--------------------------------------------------------------------------
KUiChatCentre* KUiChatCentre::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	return NULL;
}

//--------------------------------------------------------------------------
//	功能：打开窗口，返回唯一的一个类对象实例
//--------------------------------------------------------------------------
KUiChatCentre* KUiChatCentre::OpenWindow(bool bShow)
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiChatCentre;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf && bShow)
	{
		UiSoundPlay(UI_SI_WND_OPENCLOSE);
		m_pSelf->UpdateAll(m_pSelf->m_AllFriendUnits[m_pSelf->m_nCurentFriendUnit].m_nCurExpandGroup);
		m_pSelf->UpdateInterface();
		m_pSelf->BringToTop();
		m_pSelf->Show();

		m_pSelf->m_BtnInvisible.CheckButton(g_WndGameSpace.GetSetting(KUiGameSpace::enumGRS_INVISIBLE_FRIEND_LIST));
	}
	return m_pSelf;
}

//--------------------------------------------------------------------------
//	功能：初始化
//--------------------------------------------------------------------------
void KUiChatCentre::Initialize()
{
	AddChild(&m_ScrollUpBtn);
	AddChild(&m_TailImg);
	AddChild(&m_FindBtn);
	AddChild(&m_CloseBtn);
	AddChild(&m_ScrollDownBtn);
	AddChild(&m_UnitBtnFriend);
	AddChild(&m_UnitBtnBrother);
	AddChild(&m_UnitBtnEnemy);
	AddChild(&m_UnitBtnOther);
	AddChild(&m_BtnInvisible);

	for (int i = 0; i < MAX_GROUP_NUM; i++)
		AddChild(&m_GroupBtn[i]);

	AddChild(&m_FriendList);

	char Scheme[128];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);

	m_Style &= ~WND_S_VISIBLE;
	Wnd_AddWindow(this);
}

//--------------------------------------------------------------------------
//	功能：关闭窗口，同时可以选则是否删除对象实例
//--------------------------------------------------------------------------
void KUiChatCentre::CloseWindow(bool bDestroy)
{
	if (m_pSelf)
	{
		m_pSelf->CancelMenu();
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

//--------------------------------------------------------------------------
//	功能：载入窗口的界面方案
//--------------------------------------------------------------------------
void KUiChatCentre::LoadScheme(const char* pScheme)
{
	if (m_pSelf)
	{
		char		Buff[128];
		KIniFile	Ini;
		sprintf(Buff, "%s\\"SCHEME_INI, pScheme);
		if (Ini.Load(Buff))
			m_pSelf->LoadScheme(&Ini);
	}
}

void KUiChatCentre::LoadScheme(class KIniFile* pIni)
{
	int			i;
	char		Buff[32];
	_ASSERT(pIni);

	int		nVisibleLine = m_nNumVisibleLine;
	m_nMinVisibleLine = 0;
	AdjustSize(0);

	KWndShowAnimate::Init(pIni, "Main");
	m_TailImg.Init(pIni, "TailImg");
	m_FindBtn.Init(pIni, "FindBtn");
	m_CloseBtn.Init(pIni, "CloseBtn");
	m_ScrollUpBtn.Init(pIni, "ScrollUp");
	m_ScrollDownBtn.Init(pIni, "ScrollDown");
	m_UnitBtnFriend.Init(pIni, "UnitBtnFriend");
	m_BtnInvisible.Init(pIni, "Invisible");
	m_UnitBtnFriend.SetText(FRIEND_UNITNAME);
	m_UnitBtnBrother.Init(pIni, "UnitBtnBrother");
	m_UnitBtnBrother.SetText(BROTHER_UNITNAME);
	m_UnitBtnEnemy.Init(pIni, "UnitBtnEnemy");
	m_UnitBtnEnemy.SetText(ENEMY_UNITNAME);
	m_UnitBtnOther.Init(pIni, "UnitBtnOther");
	m_UnitBtnOther.SetText("其它");

	m_FriendList.Init(pIni, "MemberList");
	if (pIni->GetString("MemberList", "OfflineColor", "0,0,0", Buff, sizeof(Buff)))
		m_uFriendOffLineColor = GetColor(Buff);
	else
		m_uFriendOffLineColor = 0;

	pIni->GetInteger2("MemberList", "NumLine",
			&m_nMinVisibleLine, &m_nMaxVisibleLine);

	if (nVisibleLine < m_nMinVisibleLine)
		pIni->GetInteger("MemberList", "InitLine", m_nMinVisibleLine, &nVisibleLine);

	if (m_nMinVisibleLine < 2)
		m_nMinVisibleLine = 2;
	if (m_nMaxVisibleLine < m_nMinVisibleLine)
		m_nMaxVisibleLine = m_nMinVisibleLine;

	m_GroupBtn[0].Init(pIni, "GroupBtn");
	for (i = 1; i < MAX_GROUP_NUM; i++)
		m_GroupBtn[0].Clone(&m_GroupBtn[i]);

	m_GroupBtn[0].GetPosition(&m_nFirstGroupBtnLeft, &m_nFirstGroupBtnTop);

	m_FriendList.SetCustomColorOffset((int)&(((KUiPlayerItem*)0)->nData));

	//好友列表背景图
	const char* SectionTitle = "ListBgImg";
	int nValue;

	m_ListBgImage.nType = ISI_T_SPR;
	m_ListBgImage.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
	m_ListBgImage.Color.Color_dw = 0;
	m_ListBgImage.Color.Color_b.a = 255;
	pIni->GetInteger(SectionTitle, "Left", 0, &m_ListBgImgLeft);
	pIni->GetInteger(SectionTitle, "Top", 0, &m_ListBgImgTop);
	pIni->GetInteger(SectionTitle, "Width", 0, &m_ListBgImgWidth);
	pIni->GetInteger(SectionTitle, "Height", 0, &m_ListBgImgHeight);

	m_nLineHeight = m_ListBgImgHeight;
	if (m_nLineHeight < 8)
		m_nLineHeight = 8;

	m_ListBgImage.uImage = 0;
	m_ListBgImage.nISPosition = IMAGE_IS_POSITION_INIT;
	pIni->GetString(SectionTitle, "Image", "" , m_ListBgImage.szImage,
		sizeof(m_ListBgImage.szImage));
	pIni->GetInteger(SectionTitle, "Frame", 0, &nValue);
	m_ListBgImage.nFrame = nValue;

	AdjustSize(nVisibleLine);
}

void KUiChatCentre::CreateSeverUnit()
{
	if (m_pSelf)
	{
		m_pSelf->ClearInterface();
		for (int i = 0; i < m_pSelf->m_nFriendUnitCount; i++)
		{
			m_pSelf->ClearUnitData(i);

			if (m_pSelf->m_AllFriendUnits[i].m_pNotify &&
				m_pSelf->m_AllFriendUnits[i].m_pNotify == &ms_SNotify)
			{
				m_pSelf->m_AllFriendUnits[i].m_pNotify->CreateUnit(m_pSelf->m_AllFriendUnits[i].m_UnitName.c_str());
			}
		}
	}
}

BOOL KUiChatCentre::LoadPrivateSetting(KIniFile* pFile)
{
	if (m_pSelf)
	{
		char key[10];
		char GroupName[128];
		for (int j = 0; j < m_pSelf->m_nFriendUnitCount; j++)
		{
			for (int i = 1; i < MAX_GROUP_NUM; i++)
			{
				sprintf(key, "group%d", i);
				pFile->GetString(m_pSelf->m_AllFriendUnits[j].m_UnitName.c_str(), key, "", GroupName, 128);
				m_pSelf->m_AllFriendUnits[j].m_GroupInfo[i].strGroupName = GroupName;
			}
		}
	}
	return 1;
}

int	KUiChatCentre::SavePrivateSetting(KIniFile* pFile)
{
	if (m_pSelf)
	{
		char key[10];
		for (int j = 0; j < m_pSelf->m_nFriendUnitCount; j++)
		{
			for (int i = 1; i < MAX_GROUP_NUM; i++)
			{
				if (m_pSelf->m_AllFriendUnits[j].m_GroupInfo[i].strGroupName.empty())
					break;
				sprintf(key, "group%d", i);
				pFile->WriteString(m_pSelf->m_AllFriendUnits[j].m_UnitName.c_str(),
					key, m_pSelf->m_AllFriendUnits[j].m_GroupInfo[i].strGroupName.c_str());
			}
		}
	}
	return 1;
}


//--------------------------------------------------------------------------
//	功能：窗口消息函数
//--------------------------------------------------------------------------
int KUiChatCentre::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int nRet = 0;
	switch(uMsg)
	{
	case WND_N_BUTTON_CLICK:
		OnClickButton((KWndWindow*)uParam);
		break;
	case WND_N_BUTTON_HOLD:
		if (uParam == (unsigned int)(KWndWindow*)&m_ScrollUpBtn)
			OnScrollList(true);
		else if (uParam == (unsigned int)(KWndWindow*)&m_ScrollDownBtn)
			OnScrollList(false);
		break;
	case WND_M_POPUPMENU:
		OnClickGroupBtn((KWndWindow*)uParam, false);
		break;
	case WND_N_LIST_ITEM_D_CLICK:
		SendFriendMsg(nParam, true);
		break;
	//case WND_N_LIST_ITEM_SEL:
	//	SendFriendMsg(nParam, false);
	//	break;
	case WND_N_LIST_ITEM_R_CLICK:
		PopupFriendMenu(nParam);
		break;
	case WND_M_MENUITEM_SELECTED:
		if (uParam == (unsigned int)(KWndWindow*)this)
		{
			m_bPopupMenu = false;
			if (GET_Y_LPARAM(nParam) >= SEL_UNIT_MENU_SELECT_BASE && GET_X_LPARAM(nParam) >= 0)
			{
				OnSelUnit(GET_X_LPARAM(nParam));
			}
			else if (GET_Y_LPARAM(nParam) >= SEL_GROUP_MENU_SELECT_BASE && GET_X_LPARAM(nParam) >= 0)
			{
				OnMoveFriendToGroup(GET_Y_LPARAM(nParam) - SEL_GROUP_MENU_SELECT_BASE, GET_X_LPARAM(nParam));
				CancelMenu();
			}
			else if (GET_Y_LPARAM(nParam) >= FRIEND_MENU_SELECT_BASE)
				OnSelFriendMenu(GET_Y_LPARAM(nParam) - FRIEND_MENU_SELECT_BASE, GET_X_LPARAM(nParam));
			else
				OnSelGroupMenu(GET_Y_LPARAM(nParam) - GROUP_MENU_SELECT_BASE, GET_X_LPARAM(nParam));
		}
		break;
	case WND_M_OTHER_WORK_RESULT:
		if (nParam)
		{
			if (uParam == ISP_NEW_GROUP)
			{
				NewGroup(m_nCurentFriendUnit, (const char*)nParam);
			}
			else
			{
				int nGroupIndex = uParam - ISP_RENAME_GROUP;
				if (nGroupIndex >= 0 && nGroupIndex < MAX_GROUP_NUM)
				{
					ReplaceGroupName(m_nCurentFriendUnit, m_AllFriendUnits[m_nCurentFriendUnit].m_GroupInfo[nGroupIndex].strGroupName.c_str(), (const char*)nParam);
				}
			}
		}
		break;
	case WND_N_CHILD_MOVE:
		if (uParam == (unsigned int)(KWndWindow*)&m_TailImg)
			DragWndSize(nParam);
		break;
	default:
		nRet = KWndShowAnimate::WndProc(uMsg, uParam, nParam);
		break;
	}
	return nRet;
}

void KUiChatCentre::OnMoveFriendToGroup(int nFriendIndex, int nIndexInMenu)
{
	if (nFriendIndex >= 0 && nFriendIndex < m_nNumFriendInList)
	{
		for (int nNewGroupIndex = 0;
			nIndexInMenu >= 0 && nNewGroupIndex < MAX_GROUP_NUM;
			nNewGroupIndex++)
		{
			if (m_bValidGroup[nNewGroupIndex] && nNewGroupIndex != m_AllFriendUnits[m_nCurentFriendUnit].m_nCurExpandGroup)
			{
				if ((nIndexInMenu--) == 0)
				{
					MoveFriendToGroup(m_nCurentFriendUnit, m_pFriendList[nFriendIndex].Name, m_AllFriendUnits[m_nCurentFriendUnit].m_GroupInfo[nNewGroupIndex].strGroupName.c_str());
				}
			}
		}
	}
}

void KUiChatCentre::OnSelGroupMenu(int nGroupIndex, int nCmd)
{
	if (nGroupIndex < 0 || nGroupIndex >= MAX_GROUP_NUM ||
		m_bValidGroup[nGroupIndex] == false)
		return;
	switch(nCmd)
	{
	case GROUP_MENU_ITEM_RENAME:
		{
			char	Name[32] = "";
			m_GroupBtn[nGroupIndex].GetLabel(Name, sizeof(Name));
			KUiGetString::OpenWindow("输入新的组名称", Name,
				(KWndWindow*)this, ISP_RENAME_GROUP + nGroupIndex);
		}
		break;
	case GROUP_MENU_ITEM_DELETE:
		{
			MoveGroupToGroup(m_nCurentFriendUnit, m_AllFriendUnits[m_nCurentFriendUnit].m_GroupInfo[nGroupIndex].strGroupName.c_str(), "", true);
		}
		break;
	case GROUP_MENU_ITEM_NEW:
		KUiGetString::OpenWindow("输入新组的名称", "",
			(KWndWindow*)this, ISP_NEW_GROUP);
		break;
	}
}

void KUiChatCentre::DragWndSize(int nMoveOffset)
{
	if (nMoveOffset == 0)
	{
		m_nTailImgDragOffsetY = 0;
	}
	else
	{
		int nMoveOffsetX = (short)(nMoveOffset & 0xffff);
		int nMoveOffsetY = (short)((nMoveOffset >> 16) & 0xffff);
		int x, y;
		m_TailImg.GetPosition(&x, &y);
		x -= nMoveOffsetX;
		y -= nMoveOffsetY;
		m_TailImg.SetPosition(x, y);

		m_nTailImgDragOffsetY += nMoveOffsetY;

		int nLine;
		if (m_nTailImgDragOffsetY > 0)
			nLine = (m_nTailImgDragOffsetY + m_nLineHeight / 2) / m_nLineHeight;
		else
			nLine = (m_nTailImgDragOffsetY - m_nLineHeight / 2) / m_nLineHeight;
		nLine += m_nNumVisibleLine;
		if (nLine < m_nMinVisibleLine)
			nLine = m_nMinVisibleLine;
		else if (nLine > m_nMaxVisibleLine)
			nLine = m_nMaxVisibleLine;

		if (nLine != m_nNumVisibleLine)
		{
			m_nTailImgDragOffsetY -= (nLine - m_nNumVisibleLine) * m_nLineHeight;
			AdjustSize(nLine);
		}
	}
}

void KUiChatCentre::OnSelFriendMenu(int nFriendIndex, int nCmd)
{
	if (nFriendIndex < 0 || nFriendIndex >= m_nNumFriendInList)
		return;
	KUiPlayerItem	Friend;
	Friend = m_pFriendList[nFriendIndex];
	Friend.nData = 0;
	int				nParam = 0;
	switch(nCmd)
	{
	case FRIEND_MENU_ITEM_SEND_MSG:
		SendFriendMsg(nFriendIndex, true);
		break;
	case FRIEND_MENU_ITEM_DELETE:
		DeleteFriend(m_nCurentFriendUnit, Friend.Name, true);
		break;
	case FRIEND_MENU_ITEM_MOVE:
		PopupSelGroupMenu(nFriendIndex);
		break;
	}
}

bool KUiChatCentre::OnClickGroupBtn(KWndWindow* pBtn, bool bLeftClick)
{
	int nIndex = -1;
	for (int i = 0; i < MAX_GROUP_NUM; i++)
	{
		if (pBtn == (KWndWindow*)(&m_GroupBtn[i]))
		{
			nIndex = i;
			break;
		}
	}
	if (nIndex < 0)
		return false;
	if (bLeftClick)
	{
		if (nIndex != m_AllFriendUnits[m_nCurentFriendUnit].m_nCurExpandGroup)
		{
			UpdateGroup(nIndex);
			UpdateInterface();
		}
	}
	else
		PopupupGroupMenu(nIndex);
	return true;
}

#include "UiPlayerBar.h"
#include "UiMsgCentrePad.h"

void KUiChatCentre::SendFriendMsg(int nFriendIndex, bool bDBClick)
{
	if (m_pFriendList && nFriendIndex >= 0 && nFriendIndex < m_nNumFriendInList)
	{
		KUiPlayerBar::InputNameMsg(false, m_pFriendList[nFriendIndex].Name, bDBClick);
	}
}

void KUiChatCentre::UpdateData(UICHATCENTRE_UPDATE eFlag, unsigned int uParam, int nGroupIndex)
{
	if (m_pSelf)
	{
		switch(eFlag)
		{
		case UICC_U_ALL:
			m_pSelf->UpdateAll(m_pSelf->m_AllFriendUnits[m_pSelf->m_nCurentFriendUnit].m_nCurExpandGroup);
			break;
		case UICC_U_GROUP:
			if (nGroupIndex == m_pSelf->m_AllFriendUnits[m_pSelf->m_nCurentFriendUnit].m_nCurExpandGroup)
				m_pSelf->UpdateGroup(nGroupIndex);
			break;
		case UICC_U_FRIEND:
				m_pSelf->UpdateFriend((KUiPlayerItem*)uParam, nGroupIndex);
			break;
		}
		m_pSelf->UpdateInterface();
	}
}

bool IsInBlackName(char* strName);

bool KUiChatCentre::IsMyFriend(char* Name)
{
	if (Name == NULL || Name[0] == 0)
		return false;

	if (m_pSelf)
	{
		for (int i = 0; i < m_pSelf->m_nFriendUnitCount; i++)
		{
			FRIENDINFOLIST::iterator iF = m_pSelf->m_AllFriendUnits[i].m_FriendMap.find(Name);
			if (iF != m_pSelf->m_AllFriendUnits[i].m_FriendMap.end())
			{
				if (IsInBlackName(Name) == false)
					return true;
				break;
			}
		}
	}
	return false;
}

bool KUiChatCentre::AddFriendInfo(int nUnitIndex, char* Name, char* group)
{
	if (Name == NULL || Name[0] == 0 || group == NULL)
		return false;

	if (m_pSelf)
	{
		if (nUnitIndex < 0 || nUnitIndex >= m_pSelf->m_nFriendUnitCount)
			return false;

		if (m_pSelf->m_AllFriendUnits[nUnitIndex].m_FriendMap.find(Name) != m_pSelf->m_AllFriendUnits[nUnitIndex].m_FriendMap.end())
		{	//已存在于列表中
			NewGroup(nUnitIndex, group);
			MoveFriendToGroup(nUnitIndex, Name, group);
			return true;
		}
		std::string strGroup;
		strGroup = group;

		int nCount = 0;
		int i = 0;
		if (m_pSelf->m_AllFriendUnits[nUnitIndex].m_GroupInfo[i].strGroupName == strGroup)
		{
			nCount = m_pSelf->m_AllFriendUnits[nUnitIndex].m_GroupInfo[i].nGroupMemberNum + 1;
		}
		else
		{
			for (i = 1; i < MAX_GROUP_NUM; i++)
			{
				if (m_pSelf->m_AllFriendUnits[nUnitIndex].m_GroupInfo[i].strGroupName.empty())
					break;
				if (m_pSelf->m_AllFriendUnits[nUnitIndex].m_GroupInfo[i].strGroupName == strGroup)
				{
					nCount = m_pSelf->m_AllFriendUnits[nUnitIndex].m_GroupInfo[i].nGroupMemberNum + 1;
					break;
				}
			}
		}

		if (i < MAX_GROUP_NUM)
		{
			if (nCount > 0)	//已有的组
				m_pSelf->m_AllFriendUnits[nUnitIndex].m_GroupInfo[i].nGroupMemberNum = nCount;
			else	//没有的组,而且有空组的位置
			{
				m_pSelf->m_AllFriendUnits[nUnitIndex].m_GroupInfo[i].strGroupName = strGroup;
				m_pSelf->m_AllFriendUnits[nUnitIndex].m_GroupInfo[i].nGroupMemberNum = 1;
			}
		}
		else	//新组没有空位,归到默认组
		{
			strGroup = m_pSelf->m_AllFriendUnits[nUnitIndex].m_GroupInfo[0].strGroupName;
			m_pSelf->m_AllFriendUnits[nUnitIndex].m_GroupInfo[0].nGroupMemberNum++;
		}

		m_pSelf->m_AllFriendUnits[nUnitIndex].m_FriendMap[Name] = strGroup;

		if (m_pSelf->m_AllFriendUnits[nUnitIndex].m_pNotify)
			m_pSelf->m_AllFriendUnits[nUnitIndex].m_pNotify->CreateUnitMember(
					m_pSelf->m_AllFriendUnits[nUnitIndex].m_UnitName.c_str(), Name, group);

		if (m_pSelf->m_AllFriendUnits[nUnitIndex].m_UnitName == m_pSelf->m_AllFriendUnits[m_pSelf->m_nCurentFriendUnit].m_UnitName)
		{
			m_pSelf->UpdateAll(m_pSelf->m_AllFriendUnits[m_pSelf->m_nCurentFriendUnit].m_nCurExpandGroup);
			m_pSelf->UpdateInterface();
		}
		return true;
	}
	return false;
}

const char* KUiChatCentre::QueryUnitNameByUnitIndex(int nUnit)
{
	if(m_pSelf && nUnit >= 0 && nUnit < m_pSelf->m_nFriendUnitCount)
	{
		return m_pSelf->m_AllFriendUnits[nUnit].m_UnitName.c_str();
	}
	return NULL;
}

//返回值表示是否成功改变了状态,操作失败与状态不变返回false
bool KUiChatCentre::FriendStatus(int nUnitIndex, char* Name, BYTE state)
{
	if (m_pSelf)
	{
		if (nUnitIndex < 0 || nUnitIndex >= m_pSelf->m_nFriendUnitCount)
			return false;

		if (Name && Name[0] != 0)
		{
			if (m_pSelf->m_AllFriendUnits[nUnitIndex].m_FriendOnlineMap[Name] == state)
				return false;

			m_pSelf->m_AllFriendUnits[nUnitIndex].m_FriendOnlineMap[Name] = state;

			if (m_pSelf->m_AllFriendUnits[nUnitIndex].m_pNotify)
				m_pSelf->m_AllFriendUnits[nUnitIndex].m_pNotify->SetUnitMemberStatus(
					m_pSelf->m_AllFriendUnits[nUnitIndex].m_UnitName.c_str(), Name,
					state == stateOnline ? ONLINE_NAME : OFFLINE_NAME);

			if (m_pSelf->m_nCurentFriendUnit == nUnitIndex)
			{
				FRIENDINFOLIST::iterator i = m_pSelf->m_AllFriendUnits[nUnitIndex].m_FriendMap.find(Name);
				if (i != m_pSelf->m_AllFriendUnits[nUnitIndex].m_FriendMap.end())
				{
					int nNewGroupIndex = m_pSelf->m_AllFriendUnits[nUnitIndex].FindGroupIndex(i->second);
					if (nNewGroupIndex == m_pSelf->m_AllFriendUnits[m_pSelf->m_nCurentFriendUnit].m_nCurExpandGroup)
					{
						m_pSelf->UpdateGroup(nNewGroupIndex);
						m_pSelf->UpdateInterface();
					}
				}
			}

			if (state == stateOnline)
				UiSoundPlay(UI_SI_FRIEND_ONLINE);
			return true;
		}
	}

	return false;
}

void KUiChatCentre::NewGroup(int nUnitIndex, const char* szGroup)
{
	if (m_pSelf)
	{
		if (nUnitIndex < 0 || nUnitIndex >= m_pSelf->m_nFriendUnitCount)
			return;

		std::string strGroup;
		strGroup = szGroup;

		if (m_pSelf->m_AllFriendUnits[nUnitIndex].m_GroupInfo[0].strGroupName == strGroup)
		{
			return;
		}

		for (int i = 1; i < MAX_GROUP_NUM; i++)
		{
			if (m_pSelf->m_AllFriendUnits[nUnitIndex].m_GroupInfo[i].strGroupName == strGroup)
			{
				break;
			}
			else if (m_pSelf->m_AllFriendUnits[nUnitIndex].m_GroupInfo[i].strGroupName.empty())
			{
				m_pSelf->m_AllFriendUnits[nUnitIndex].m_GroupInfo[i].strGroupName = strGroup;

				if (m_pSelf->m_AllFriendUnits[nUnitIndex].m_pNotify)
					m_pSelf->m_AllFriendUnits[nUnitIndex].m_pNotify->CreateUnitGroup(m_pSelf->m_AllFriendUnits[nUnitIndex].m_UnitName.c_str(), strGroup.c_str());

				if (nUnitIndex == m_pSelf->m_nCurentFriendUnit)
				{
					m_pSelf->UpdateAll(m_pSelf->m_AllFriendUnits[nUnitIndex].m_nCurExpandGroup);
					m_pSelf->UpdateInterface();
				}
				break;
			}
		}
	}
}

void KUiChatCentre::ReplaceGroupName(int nUnitIndex, const char* sz, const char* szGroup)
{
	if (m_pSelf)
	{
		if (nUnitIndex < 0 || nUnitIndex >= m_pSelf->m_nFriendUnitCount)
			return;

		std::string str;
		if (sz)
			str = sz;

		std::string strGroup;
		if (szGroup)
			strGroup = szGroup;

		if (str == strGroup)
			return;

		if (m_pSelf->m_AllFriendUnits[nUnitIndex].FindGroupIndex(strGroup) >= 0)
			return;

		int i = m_pSelf->m_AllFriendUnits[nUnitIndex].FindGroupIndex(str);
		if (i <= 0)	//0是不能改名的
			return;

		STRINGLIST list;
		FRIENDINFOLIST::iterator iF = m_pSelf->m_AllFriendUnits[nUnitIndex].m_FriendMap.begin();
		for (; iF != m_pSelf->m_AllFriendUnits[nUnitIndex].m_FriendMap.end(); iF++)
		{
			if (iF->second == str)
			{
				iF->second = strGroup;
				list.push_back(iF->first);
			}
		}
		std::string strOld = m_pSelf->m_AllFriendUnits[nUnitIndex].m_GroupInfo[i].strGroupName;
		m_pSelf->m_AllFriendUnits[nUnitIndex].m_GroupInfo[i].strGroupName = strGroup;
		if (m_pSelf->m_AllFriendUnits[nUnitIndex].m_pNotify)
			m_pSelf->m_AllFriendUnits[nUnitIndex].m_pNotify->RenameUnitGroup(
				m_pSelf->m_AllFriendUnits[nUnitIndex].m_UnitName.c_str(), strOld.c_str(), strGroup.c_str(), list);

		if (nUnitIndex == m_pSelf->m_nCurentFriendUnit)
		{
			m_pSelf->UpdateAll(m_pSelf->m_AllFriendUnits[nUnitIndex].m_nCurExpandGroup);
			m_pSelf->UpdateInterface();
		}
	}
}

void KUiChatCentre::MoveFriendToGroup(int nUnitIndex, const char* Name, const char* group)
{
	if (m_pSelf)
	{
		if (nUnitIndex < 0 || nUnitIndex >= m_pSelf->m_nFriendUnitCount)
			return;

		int nNewGroupIndex = m_pSelf->m_AllFriendUnits[nUnitIndex].FindGroupIndex(group);
		if (nNewGroupIndex < 0)
			return;

		FRIENDINFOLIST::iterator iF = m_pSelf->m_AllFriendUnits[nUnitIndex].m_FriendMap.find(Name);
		if (iF != m_pSelf->m_AllFriendUnits[nUnitIndex].m_FriendMap.end())
		{
			int nSrcIndex = m_pSelf->m_AllFriendUnits[nUnitIndex].FindGroupIndex(iF->second);
			if (nSrcIndex >= 0)
				m_pSelf->m_AllFriendUnits[nUnitIndex].m_GroupInfo[nSrcIndex].nGroupMemberNum--;
			iF->second = group;
			m_pSelf->m_AllFriendUnits[nUnitIndex].m_GroupInfo[nNewGroupIndex].nGroupMemberNum++;

			if (m_pSelf->m_AllFriendUnits[nUnitIndex].m_pNotify)
				m_pSelf->m_AllFriendUnits[nUnitIndex].m_pNotify->MoveUnitMember(
					m_pSelf->m_AllFriendUnits[nUnitIndex].m_UnitName.c_str(), Name, group);

			if (nUnitIndex == m_pSelf->m_nCurentFriendUnit &&
				(nSrcIndex == m_pSelf->m_AllFriendUnits[nUnitIndex].m_nCurExpandGroup ||
				 nNewGroupIndex == m_pSelf->m_AllFriendUnits[nUnitIndex].m_nCurExpandGroup)
				)
			{
				m_pSelf->UpdateGroup(m_pSelf->m_AllFriendUnits[nUnitIndex].m_nCurExpandGroup);
				m_pSelf->UpdateInterface();
			}
		}
	}
}

void KUiChatCentre::MoveGroupToGroup(int nUnitIndex, const char* group, const char* group2, bool bDelgroup)
{
	if (m_pSelf)
	{
		if (nUnitIndex < 0 || nUnitIndex >= m_pSelf->m_nFriendUnitCount)
			return;

		int nSrcIndex = m_pSelf->m_AllFriendUnits[nUnitIndex].FindGroupIndex(group);
		if (nSrcIndex < 0)
			return;
		int nNewGroupIndex = m_pSelf->m_AllFriendUnits[nUnitIndex].FindGroupIndex(group2);
		if (nNewGroupIndex < 0)
			return;

		if (nSrcIndex == nNewGroupIndex)
			return;

		STRINGLIST list;
		FRIENDINFOLIST::iterator iF = m_pSelf->m_AllFriendUnits[nUnitIndex].m_FriendMap.begin();
		while (iF != m_pSelf->m_AllFriendUnits[nUnitIndex].m_FriendMap.end())
		{
			if (iF->second == group)
			{
				m_pSelf->m_AllFriendUnits[nUnitIndex].m_GroupInfo[nSrcIndex].nGroupMemberNum--;
				iF->second = group2;
				m_pSelf->m_AllFriendUnits[nUnitIndex].m_GroupInfo[nNewGroupIndex].nGroupMemberNum++;
				list.push_back(iF->first);
			}
			iF++;
		}

		if (m_pSelf->m_AllFriendUnits[nUnitIndex].m_pNotify)
		{
			if (bDelgroup)
			{
				m_pSelf->m_AllFriendUnits[nUnitIndex].m_pNotify->DeleteUnitGroup(
					m_pSelf->m_AllFriendUnits[nUnitIndex].m_UnitName.c_str(), group, list);
			}
			else
			{
				m_pSelf->m_AllFriendUnits[nUnitIndex].m_pNotify->MoveUnitGroup(
					m_pSelf->m_AllFriendUnits[nUnitIndex].m_UnitName.c_str(), group, group2, list);
			}
		}
		if (bDelgroup)
		{
			if (nSrcIndex != 0)	//0是每个Unit的默认组,不能删除
			{
				for (;nSrcIndex < MAX_GROUP_NUM - 1; nSrcIndex++)
				{
					m_pSelf->m_AllFriendUnits[nUnitIndex].m_GroupInfo[nSrcIndex] = m_pSelf->m_AllFriendUnits[nUnitIndex].m_GroupInfo[nSrcIndex + 1];
				}
				m_pSelf->m_AllFriendUnits[nUnitIndex].m_GroupInfo[nSrcIndex].strGroupName = "";
				m_pSelf->m_AllFriendUnits[nUnitIndex].m_GroupInfo[nSrcIndex].nGroupMemberNum = 0;
			}
		}

		if (nUnitIndex == m_pSelf->m_nCurentFriendUnit)
		{
			m_pSelf->UpdateAll(nNewGroupIndex);
			m_pSelf->UpdateInterface();
		}
	}
}

int	KUiChatCentre::FindUnitIndex(const char* unit)
{
	if (unit == NULL || unit[0] == 0)
		return -1;

	if (m_pSelf)
	{
		for (int i = 0; i < m_pSelf->m_nFriendUnitCount; i++)
		{
			if (m_pSelf->m_AllFriendUnits[i].m_UnitName == unit)
				return i;
		}
	}

	return -1;
}

int	KUiChatCentre::FindUnitIndexByRoleNameAtServerUnit(char* roleName)
{
	if (roleName == NULL || roleName[0] == 0)
		return -1;

	if (m_pSelf)
	{
		for (int i = 0; i < m_pSelf->m_nFriendUnitCount; i++)
		{
			if (m_pSelf->m_AllFriendUnits[i].m_pNotify == &ms_SNotify &&
				m_pSelf->m_AllFriendUnits[i].m_FriendMap.find(roleName) != m_pSelf->m_AllFriendUnits[i].m_FriendMap.end())
			{
				return i;
			}
		}
	}

	return -1;
}

int	KUiChatCentre::FindAddinUnitIndex(const char* unit)
{
	if (unit == NULL || unit[0] == 0)
		return -1;

	if (m_pSelf)
	{
		for (int i = 0; i < m_pSelf->m_nFriendUnitCount; i++)
		{
			if (m_pSelf->m_AllFriendUnits[i].m_UnitName == unit &&
				m_pSelf->m_AllFriendUnits[i].m_pNotify != &ms_SNotify)
				return i;
		}
	}

	return -1;
}

int	KUiChatCentre::AddAddinUnit(const char* unit, FriendNotify* pNotify)
{
	if (unit == NULL || unit[0] == 0)
		return -1;

	int nUnit = FindUnitIndex(unit);
	if (nUnit < 0)
	{
		if (m_pSelf && m_pSelf->m_nFriendUnitCount < MAX_UNIT_NUM)
		{
			m_pSelf->m_AllFriendUnits[m_pSelf->m_nFriendUnitCount].m_UnitName = unit;
			m_pSelf->ClearUnitData(m_pSelf->m_nFriendUnitCount);
			m_pSelf->m_AllFriendUnits[m_pSelf->m_nFriendUnitCount].m_pNotify = pNotify;
			m_pSelf->m_nFriendUnitCount++;

			if (pNotify)
				pNotify->CreateUnit(unit);

			return m_pSelf->m_nFriendUnitCount - 1;
		}
	}

	return nUnit;
}

int KUiChatCentre::RemoveAddinUnit(const char* unit)
{
	if (unit == NULL || unit[0] == 0)
		return -1;

	int nUnit = FindUnitIndex(unit);
	if (nUnit >= 0)
	{
		if (m_pSelf && m_pSelf->m_AllFriendUnits[nUnit].m_pNotify != &ms_SNotify)
		{
			bool bU = (nUnit == m_pSelf->m_nCurentFriendUnit);
			m_pSelf->m_nFriendUnitCount--;
			for (; nUnit < m_pSelf->m_nFriendUnitCount; nUnit++)
			{
				m_pSelf->m_AllFriendUnits[nUnit] = m_pSelf->m_AllFriendUnits[nUnit + 1];
			}

			if (m_pSelf->m_AllFriendUnits[nUnit].m_pNotify)
				m_pSelf->m_AllFriendUnits[nUnit].m_pNotify->DeleteUnit(unit);

			if (bU)
			{
				m_pSelf->m_nCurentFriendUnit = FRIEND_INDEX;
				m_pSelf->UpdateAll(m_pSelf->m_AllFriendUnits[m_pSelf->m_nCurentFriendUnit].m_nCurExpandGroup);
				m_pSelf->UpdateInterface();
			}
			return m_pSelf->m_nFriendUnitCount;
		}
	}

	return -1;
}


void KUiChatCentre::DeleteFriend(int nUnitIndex, char* Name, bool bNotify)
{
	if (Name == NULL || Name[0] == 0)
		return;

	if (m_pSelf)
	{
		FRIENDINFOLIST::iterator iF = m_pSelf->m_AllFriendUnits[nUnitIndex].m_FriendMap.find(Name);
		if (iF != m_pSelf->m_AllFriendUnits[nUnitIndex].m_FriendMap.end())
		{
			int nI = m_pSelf->m_AllFriendUnits[nUnitIndex].FindGroupIndex(iF->second);
			if (nI >= 0)
			{
				m_pSelf->m_AllFriendUnits[nUnitIndex].m_GroupInfo[nI].nGroupMemberNum--;
			}
			m_pSelf->m_AllFriendUnits[nUnitIndex].m_FriendMap.erase(iF);

			if (m_pSelf->m_AllFriendUnits[nUnitIndex].m_pNotify && bNotify)
				m_pSelf->m_AllFriendUnits[nUnitIndex].m_pNotify->DeleteUnitMember(m_pSelf->m_AllFriendUnits[nUnitIndex].m_UnitName.c_str(), Name);

			if (nUnitIndex == m_pSelf->m_nCurentFriendUnit &&
				nI == m_pSelf->m_AllFriendUnits[nUnitIndex].m_nCurExpandGroup)
			{
				m_pSelf->UpdateGroup(m_pSelf->m_AllFriendUnits[nUnitIndex].m_nCurExpandGroup);
				m_pSelf->UpdateInterface();
			}
		}
	}
}

// -------------------------------------------------------------------------
// 功能		: 点击按钮后的处理
// -------------------------------------------------------------------------
void KUiChatCentre::OnClickButton(KWndWindow* pWnd)
{
	if (pWnd == (KWndWindow*)&m_ScrollUpBtn)
		OnScrollList(true);
	else if (pWnd == (KWndWindow*)&m_ScrollDownBtn)
		OnScrollList(false);
	else if (pWnd == (KWndWindow*)&m_FindBtn)
		KUiSelPlayerNearby::OpenWindow();
	else if (pWnd == (KWndWindow*)&m_CloseBtn)
		CloseWindow(false);
	else if (pWnd == (KWndWindow*)&m_UnitBtnFriend)
	{
		OnSelUnit(FRIEND_INDEX);
	}
	else if (pWnd == (KWndWindow*)&m_UnitBtnBrother)
	{
		OnSelUnit(BROTHER_INDEX);
	}
	else if (pWnd == (KWndWindow*)&m_UnitBtnEnemy)
	{
		OnSelUnit(ENEMY_INDEX);
	}
	else if (pWnd == (KWndWindow*)&m_UnitBtnOther)
	{
		PopupSelUnitMenu();
	}
	else if (pWnd == (KWndWindow*)&m_BtnInvisible)
	{
		if(g_pCoreShell)
		{
			g_pCoreShell->OperationRequest(GOI_INVISIBLE_IN_FRIEND_LIST, 0, m_BtnInvisible.IsButtonChecked());
			g_LoginLogic.SetInvisibleLogin(m_BtnInvisible.IsButtonChecked());
			g_LoginLogic.SaveAccountSetting();
		}
	}
	else
		OnClickGroupBtn(pWnd, true);
}

void KUiChatCentre::OnSelUnit(int nUnitIndex)
{
	if (nUnitIndex >= 0 && nUnitIndex < m_nFriendUnitCount)
	{
		m_nCurentFriendUnit = nUnitIndex;
		UpdateAll(m_AllFriendUnits[m_nCurentFriendUnit].m_nCurExpandGroup);
		UpdateInterface();
	}
}

void KUiChatCentre::OnScrollList(bool bUp)
{
	if (bUp == false)
	{
		m_nFirstVisibleLine ++;
		UpdateInterface();
	}
	else if(m_nFirstVisibleLine > 0)
	{
		m_nFirstVisibleLine--;
		UpdateInterface();
	}
}

void KUiChatCentre::CancelMenu()
{
	if (m_bPopupMenu)
	{
		KPopupMenu::Cancel();
		m_bPopupMenu = false;
	}
	if (m_pSelGrupMenu)
	{
		free(m_pSelGrupMenu);
		m_pSelGrupMenu = NULL;
	}
}

void KUiChatCentre::PopupupGroupMenu(int nGroupIndex)
{
	int x, y;
	Wnd_GetCursorPos(&x, &y);
	KPopupMenuData* pMenuData = (KPopupMenuData*)&m_GroupMenuData;
	pMenuData->nX = x;
	pMenuData->nY = y;
	pMenuData->nItemWidth = MENU_ITEM_DEFAULT_WIDTH;
	pMenuData->nNumItem = nGroupIndex ? GROUP_MENU_ITEM_COUNT : DEF_GROUP_MENU_ITEM_COUNT;
	KPopupMenu::Popup(pMenuData, (KWndWindow*)this,
		GROUP_MENU_SELECT_BASE + nGroupIndex);
	m_bPopupMenu = true;
}

void KUiChatCentre::PopupFriendMenu(int nFriendIndex)
{
	int x, y;
	Wnd_GetCursorPos(&x, &y);
	KPopupMenuData* pMenuData = (KPopupMenuData*)&m_FriendMenuData;
	pMenuData->nX = x;
	pMenuData->nY = y;
	pMenuData->nItemWidth = MENU_ITEM_DEFAULT_WIDTH;
	pMenuData->nNumItem = FRIEND_MENU_ITEM_COUNT;

	if (m_nCurentFriendUnit == MATE_INDEX)
		pMenuData->nNumItem -= 2;	//移动好友到其它组与删除无效
	else if (m_nNumValidGroup <= 1)
		pMenuData->nNumItem --;		//移动好友到其它组无效
	KPopupMenu::Popup(pMenuData, (KWndWindow*)this,
		FRIEND_MENU_SELECT_BASE + nFriendIndex);
	m_bPopupMenu = true;
}

void KUiChatCentre::PopupSelGroupMenu(int nFriendIndex)
{
	if (m_nNumValidGroup <= 1)
		return;
	m_pSelGrupMenu = (KPopupMenuData*)malloc(MENU_DATA_SIZE(m_nNumValidGroup - 1));
	if (m_pSelGrupMenu == NULL)
		return;
	KPopupMenu::InitMenuData(m_pSelGrupMenu, m_nNumValidGroup - 1);
	m_pSelGrupMenu->nNumItem = 0;
	for (int i = 0; i < MAX_GROUP_NUM && m_pSelGrupMenu->nNumItem < m_nNumValidGroup - 1; i++)
	{
		if (m_bValidGroup[i] && i != m_AllFriendUnits[m_nCurentFriendUnit].m_nCurExpandGroup)
		{
			m_GroupBtn[i].GetLabel(m_pSelGrupMenu->Items[m_pSelGrupMenu->nNumItem].szData,
				sizeof(m_pSelGrupMenu->Items[0].szData));
			m_pSelGrupMenu->Items[m_pSelGrupMenu->nNumItem].uDataLen = strlen(m_pSelGrupMenu->Items[m_pSelGrupMenu->nNumItem].szData);
			m_pSelGrupMenu->nNumItem ++;
		}
	}
	KPopupMenu::Popup(m_pSelGrupMenu, this, SEL_GROUP_MENU_SELECT_BASE + nFriendIndex);
}

void KUiChatCentre::PopupSelUnitMenu()
{
	struct KPopupMenuData* pSelUnitMenu = (KPopupMenuData*)malloc(MENU_DATA_SIZE(m_nFriendUnitCount));
	if (pSelUnitMenu == NULL)
		return;
	KPopupMenu::InitMenuData(pSelUnitMenu, m_nFriendUnitCount);
	pSelUnitMenu->nNumItem = 0;
	pSelUnitMenu->usMenuFlag |= PM_F_AUTO_DEL_WHEN_HIDE;
	for (int i = 0; i < m_nFriendUnitCount; i++)
	{
		strncpy(pSelUnitMenu->Items[i].szData, m_AllFriendUnits[i].m_UnitName.c_str(), 63);
		pSelUnitMenu->Items[i].szData[sizeof(pSelUnitMenu->Items[i].szData) - 1] = 0;
		pSelUnitMenu->Items[i].uDataLen = strlen(pSelUnitMenu->Items[i].szData);
		pSelUnitMenu->nNumItem++;
	}
	KPopupMenu::Popup(pSelUnitMenu, this, SEL_UNIT_MENU_SELECT_BASE);
}

void KUiChatCentre::ClearInterface()
{
	CancelMenu();
	ClearFriendList();
	m_AllFriendUnits[m_nCurentFriendUnit].m_nCurExpandGroup = 0;
	m_nNumValidGroup = 0;
	for (int nIndex = 0; nIndex < MAX_GROUP_NUM; nIndex++)
		m_bValidGroup[nIndex] = false;
}

void KUiChatCentre::ClearFriendList()
{
	m_FriendList.SetContent(NULL, 0, 0, 0);
	m_nNumFriendInList = 0;
	if (m_pFriendList)
	{
		free(m_pFriendList);
		m_pFriendList = NULL;
	}
}

void KUiChatCentre::ClearUnitData(int nUnit)
{
	if (nUnit >= 0 && nUnit < MAX_UNIT_NUM)
	{
		//不清理名字和m_pNotify
		m_AllFriendUnits[nUnit].m_FriendMap.clear();
		m_AllFriendUnits[nUnit].m_FriendOnlineMap.clear();
		for (int i = 0; i < MAX_GROUP_NUM; i++)
		{
			m_AllFriendUnits[nUnit].m_GroupInfo[i].strGroupName = "";
			m_AllFriendUnits[nUnit].m_GroupInfo[i].nGroupMemberNum	= 0;
		}
		m_AllFriendUnits[nUnit].m_nCurExpandGroup = 0;
	}
}

void KUiChatCentre::UpdateAll(int nCurGroup)
{
	ClearInterface();
	int	nIndex = 0;
	m_bValidGroup[nIndex] = true;
	std::string strName = DEFAULT_GROUPNAME;
	strName += m_AllFriendUnits[m_nCurentFriendUnit].m_UnitName;
	m_GroupBtn[nIndex].SetLabel(strName.c_str());
	m_nNumValidGroup++;
	for (nIndex = 1; nIndex < MAX_GROUP_NUM; nIndex++)
	{
		if (m_AllFriendUnits[m_nCurentFriendUnit].m_GroupInfo[nIndex].strGroupName.empty())
		{
			m_bValidGroup[nIndex] = false;
		}
		else
		{
			m_bValidGroup[nIndex] = true;
			m_GroupBtn[nIndex].SetLabel(m_AllFriendUnits[m_nCurentFriendUnit].m_GroupInfo[nIndex].strGroupName.c_str());
			m_nNumValidGroup++;
		}
	}

	m_AllFriendUnits[m_nCurentFriendUnit].m_nCurExpandGroup = nCurGroup;

	if (m_bValidGroup[m_AllFriendUnits[m_nCurentFriendUnit].m_nCurExpandGroup] == false)
	{
		m_AllFriendUnits[m_nCurentFriendUnit].m_nCurExpandGroup = 0;
		for (nIndex = 0; nIndex < MAX_GROUP_NUM; nIndex++)
		{
			if (m_bValidGroup[nIndex])
			{
				m_AllFriendUnits[m_nCurentFriendUnit].m_nCurExpandGroup = nIndex;
				break;
			}
		}
	}

	UpdateGroup(m_AllFriendUnits[m_nCurentFriendUnit].m_nCurExpandGroup);
}

void KUiChatCentre::UpdateGroup(int nGroupIndex)
{
	ClearFriendList();

	if (nGroupIndex < 0 || nGroupIndex >= m_nNumValidGroup)
		return;

	char groupname[64];
	m_GroupBtn[nGroupIndex].GetLabel(groupname, 64);

	if (nGroupIndex == 0)
	{
		groupname[0] = 0;
	}
	else if (nGroupIndex >= 1 && nGroupIndex < MAX_GROUP_NUM)
	{
		if (m_AllFriendUnits[m_nCurentFriendUnit].m_GroupInfo[nGroupIndex].strGroupName.empty())
			return;
	}

	m_AllFriendUnits[m_nCurentFriendUnit].m_nCurExpandGroup = nGroupIndex;

	KUiChatGroupInfo	Group;
	strncpy(Group.szTitle, groupname, 32);
	Group.nNumFriend = m_AllFriendUnits[m_nCurentFriendUnit].m_GroupInfo[nGroupIndex].nGroupMemberNum;
	if (Group.nNumFriend)
	{
		m_pFriendList = (KUiPlayerItem*)malloc(sizeof(KUiPlayerItem) * Group.nNumFriend);
		if (m_pFriendList)
		{
			FRIENDINFOLIST onlineList;
			FRIENDINFOLIST offlineList;
			FRIENDINFOLIST::iterator i = m_AllFriendUnits[m_nCurentFriendUnit].m_FriendMap.begin();
			for (; i != m_AllFriendUnits[m_nCurentFriendUnit].m_FriendMap.end(); i++)
			{
				if (strcmp(i->second.c_str(), groupname) == 0)
				{
					if (m_AllFriendUnits[m_nCurentFriendUnit].m_FriendOnlineMap.find(i->first) != m_AllFriendUnits[m_nCurentFriendUnit].m_FriendOnlineMap.end())
					{
						if (m_AllFriendUnits[m_nCurentFriendUnit].m_FriendOnlineMap[i->first] == stateOnline)
							onlineList[i->first] = stateOnline;
						else
							offlineList[i->first] = stateOffline;
					}
					else
						offlineList[i->first] = stateOffline;
				}
			}

			m_nNumFriendInList = 0;
			i = onlineList.begin();
			while (i != onlineList.end() && m_nNumFriendInList < Group.nNumFriend)
			{
				strncpy(m_pFriendList[m_nNumFriendInList].Name, i->first.c_str(), 32);
				m_pFriendList[m_nNumFriendInList].nData = 0;
				m_nNumFriendInList++;
				i++;
			}
			i = offlineList.begin();
			while (i != offlineList.end() && m_nNumFriendInList < Group.nNumFriend)
			{
				strncpy(m_pFriendList[m_nNumFriendInList].Name, i->first.c_str(), 32);
				m_pFriendList[m_nNumFriendInList].nData = m_uFriendOffLineColor;
				m_nNumFriendInList++;
				i++;
			}

			m_FriendList.SetContent((unsigned char*)m_pFriendList, m_nNumFriendInList, sizeof(KUiPlayerItem),
				(int)&(((KUiPlayerItem*)0)->Name));
		}
	}
}

void KUiChatCentre::UpdateFriend(KUiPlayerItem* pFriend, int nGroupIndex)
{
	if (pFriend == NULL)
		return;
	if (nGroupIndex == m_AllFriendUnits[m_nCurentFriendUnit].m_nCurExpandGroup && m_pFriendList)
	{
		UpdateGroup(nGroupIndex);
	}
}

int	KUiChatCentre::FriendUnit::FindGroupIndex(const std::string& str)
{
	if (str.empty())
		return 0;

	for (int nIndex = 1; nIndex < MAX_GROUP_NUM; nIndex++)
	{
		if (m_GroupInfo[nIndex].strGroupName.empty())
		{
			return -1;
		}
		else if (m_GroupInfo[nIndex].strGroupName == str)
			return nIndex;
	}

	return -1;
}

void KUiChatCentre::UpdateInterface()
{
	int	i;

	m_UnitBtnFriend.CheckButton(0);
	m_UnitBtnBrother.CheckButton(0);
	m_UnitBtnEnemy.CheckButton(0);
	m_UnitBtnOther.CheckButton(0);
	if (m_nCurentFriendUnit == FRIEND_INDEX)
		m_UnitBtnFriend.CheckButton(1);
	else if (m_nCurentFriendUnit == BROTHER_INDEX)
		m_UnitBtnBrother.CheckButton(1);
	else if (m_nCurentFriendUnit == ENEMY_INDEX)
		m_UnitBtnEnemy.CheckButton(1);
	else
		m_UnitBtnOther.CheckButton(1);

	//先隐藏全部的组按钮
	for (i = 0; i < MAX_GROUP_NUM; i++)
		m_GroupBtn[i].Hide();

	//计算好友列表之前组按钮的数目
	int nNumGroupAhead = 0;
	for (i = 0; i <= m_AllFriendUnits[m_nCurentFriendUnit].m_nCurExpandGroup; i++)
	{
		if (m_bValidGroup[i])
		{
			nNumGroupAhead++;
		}
	}

	//计算好友列表显示的行数，至少要显示一行
	int nFriendListLines = (m_nNumFriendInList > 1) ? m_nNumFriendInList : 1;

	//组按钮的高度
	int nBtnHeight;
	m_GroupBtn[0].GetSize(NULL, &nBtnHeight);

	//调整确认被显示的第一行内容的位置
	int nLimitLine = m_nNumValidGroup + nFriendListLines;
	int nContentWndHeight = m_nNumVisibleLine * m_nLineHeight;
	int nHeight = nContentWndHeight;	//从此处开始，nHeight表示窗口剩余的高度
	while(true)
	{
		//好友列表后的组按钮
		if (m_nNumValidGroup - nNumGroupAhead)
		{
			i = nHeight / nBtnHeight;
			if (m_nNumValidGroup - nNumGroupAhead > i)
			{
				nLimitLine -= i;
				break;
			}
			nHeight -= (m_nNumValidGroup - nNumGroupAhead) * nBtnHeight;
			nLimitLine -= m_nNumValidGroup - nNumGroupAhead;
		}
		//好友列表
		i = nHeight / m_nLineHeight;
		if (nFriendListLines > i)
		{
			nLimitLine -= i;
			break;
		}
		nHeight -= nFriendListLines * m_nLineHeight;
		nLimitLine -= nFriendListLines;
		//好友列表前的组按钮
		i = nHeight / nBtnHeight;
		if (nNumGroupAhead > i)
			nLimitLine = nNumGroupAhead - i;
		else
			nLimitLine = 0;
		break;
	}
	if (m_nFirstVisibleLine > nLimitLine)
		m_nFirstVisibleLine = nLimitLine;

	int	nGroupIndex = 0, nLineIndex = 0;
	int nTop = 0;	//nTop用于表示已显示内容的高度

	//前面跳过不显示的组按钮
	for (; nLineIndex < m_nFirstVisibleLine &&
			nGroupIndex <= m_AllFriendUnits[m_nCurentFriendUnit].m_nCurExpandGroup && nGroupIndex < MAX_GROUP_NUM;
			nGroupIndex++)
	{
		if (m_bValidGroup[nGroupIndex])
		{
			nLineIndex++;
		}
	}

	//显示当前展开组前(包括)的组按钮
	for (; nGroupIndex <= m_AllFriendUnits[m_nCurentFriendUnit].m_nCurExpandGroup && nGroupIndex < MAX_GROUP_NUM; nGroupIndex++)
	{
		if (m_bValidGroup[nGroupIndex])
		{
			m_GroupBtn[nGroupIndex].Show();
			m_GroupBtn[nGroupIndex].SetPosition(m_nFirstGroupBtnLeft, nTop + m_nFirstGroupBtnTop);
			nTop += nBtnHeight;
			if (nTop >= nContentWndHeight)
				break;
		}
	}

	m_FriendList.Hide();
	if (nTop >= nContentWndHeight)
		return;

	//===处理好友列表的显示===
	int nNumFriendSkip = 0;	//头前要跳过显示的好友的数目
	nHeight = 0;	//从此处开始，nHeight表示好友列表窗口的显示高度
	if (nLimitLine == 0)
	{
		nHeight = nContentWndHeight - m_nNumValidGroup * nBtnHeight;
	}
	else
	{
		if (nLineIndex < m_nFirstVisibleLine)
		{
			nNumFriendSkip = m_nFirstVisibleLine - nLineIndex;
			if (nFriendListLines > nNumFriendSkip)
			{
				nFriendListLines -= nNumFriendSkip;
				nLineIndex += nNumFriendSkip;
			}
			else
			{
				nFriendListLines = 0;
				nLineIndex += nFriendListLines;
			}
		}

		nHeight = nContentWndHeight - nTop;
		if (nHeight / m_nLineHeight < nFriendListLines)
			nFriendListLines = nHeight / m_nLineHeight;
		else
			nHeight = nFriendListLines * m_nLineHeight;
	}

	if (nHeight)
	{
		//设置好友列表窗口的位置
		int x;
		m_FriendList.GetPosition(&x, NULL);
		m_FriendList.SetPosition(x, nTop + m_nFirstGroupBtnTop);
		m_FriendList.GetSize(&x, NULL);
		m_FriendList.SetSize(x, nHeight);
		m_FriendList.SetTopItemIndex(nNumFriendSkip);
		m_FriendList.Show();
		nLineIndex += nFriendListLines;
		nTop += nHeight;
	}

	if (nTop >= nContentWndHeight)
		return;

	//当前展开组后的组按钮
	for (; nGroupIndex < MAX_GROUP_NUM; nGroupIndex++)
	{
		if (m_bValidGroup[nGroupIndex])
		{
			if (nLineIndex < m_nFirstVisibleLine)
				nLineIndex ++;
			else
			{
				m_GroupBtn[nGroupIndex].Show();
				m_GroupBtn[nGroupIndex].SetPosition(m_nFirstGroupBtnLeft, nTop + m_nFirstGroupBtnTop);
				nTop += nBtnHeight;
				if (nTop >= nContentWndHeight)
					break;
			}
		}
	}
}

//绘制窗口
void KUiChatCentre::PaintWindow()
{
	m_ListBgImage.oPosition.nX = m_nAbsoluteLeft + m_ListBgImgLeft;
	m_ListBgImage.oPosition.nY = m_nAbsoluteTop + m_ListBgImgTop;
	for (int i = 0; i < m_nNumVisibleLine; i++)
	{
		g_pRepresentShell->DrawPrimitives(1, &m_ListBgImage, RU_T_IMAGE, true);
		m_ListBgImage.oPosition.nY += m_nLineHeight;
	}
	KWndShowAnimate::PaintWindow();
}

void KUiChatCentre::AdjustSize(int nNumLine)
{
	if (nNumLine < m_nMinVisibleLine)
		nNumLine = m_nMinVisibleLine;
	else if (nNumLine > m_nMaxVisibleLine)
		nNumLine = m_nMaxVisibleLine;

	int nOffsetY = (nNumLine - m_nNumVisibleLine) * m_nLineHeight;
	m_nNumVisibleLine = nNumLine;

	int x, y;
	m_FindBtn.GetPosition(&x, &y);
	y += nOffsetY;
	m_FindBtn.SetPosition(x, y);

	m_ScrollDownBtn.GetPosition(&x, &y);
	y += nOffsetY;
	m_ScrollDownBtn.SetPosition(x, y);

	m_CloseBtn.GetPosition(&x, &y);
	y += nOffsetY;
	m_CloseBtn.SetPosition(x, y);

	m_TailImg.GetPosition(&x, &y);
	y += nOffsetY;
	m_TailImg.SetPosition(x, y);

	m_BtnInvisible.GetPosition(&x, &y);
	y += nOffsetY;
	m_BtnInvisible.SetPosition(x, y);

	m_TailImg.GetSize(&x, &nOffsetY);
	y += nOffsetY;

	GetSize(&x, NULL);
	SetSize(x, y);

	UpdateInterface();
}

//判断一个点是否在窗口范围内,传入的是绝对坐标
int KUiChatCentre::PtInWindow(int x, int y)
{
	int nRet = 0;
	if (KWndWindow::PtInWindow(x, y))
	{
		if (x > m_nAbsoluteLeft + m_ListBgImgLeft &&
			y > m_nAbsoluteTop + m_nFirstGroupBtnTop &&
			x < m_nAbsoluteLeft + m_nFirstGroupBtnLeft + m_ListBgImgWidth &&
			y < m_nAbsoluteTop + m_nFirstGroupBtnTop + m_nNumVisibleLine * m_nLineHeight)
		{
			nRet = 1;
		}
		if (nRet == 0)
		{
			nRet = KWndShowAnimate::PtInWindow(x, y);
			if (nRet == 0)
			{
				nRet = m_TailImg.PtInWindow(x, y);
			}
		}
	}
	return nRet;
}
