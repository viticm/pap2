/*****************************************************************************************
//	界面--聊天主界面
//	Copyright : Kingsoft 2003
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2003-3-10
------------------------------------------------------------------------------------------
*****************************************************************************************/
#pragma once
#include "../Elem/WndShowAnimate.h"
#include "../Elem/WndLabeledButton.h"
#include "../Elem/WndMessageListBox.h"
#include "../Elem/WndScrollBar.h"
#include "../Elem/WndList.h"
#include "../Elem/PopupMenu.h"
#include "../Elem/WndPureTextBtn.h"

#pragma warning( disable : 4786 4800 4355 4146 4273 4503 )

#include <map>
#include <string>
#include <list>

using namespace std;

#define	MAX_UNIT_NUM	10
#define	MAX_GROUP_NUM	10		//最多允许的好友分组的数目

enum UICHATCENTRE_UPDATE
{
	UICC_U_ALL,
	UICC_U_GROUP,
	UICC_U_FRIEND,
};

typedef std::list<std::string>	STRINGLIST;

struct FriendNotify
{
	virtual int	CreateUnit(const char* Unit) = 0;
	virtual int	DeleteUnit(const char* Unit) = 0;
	virtual int	CreateUnitGroup(const char* Unit, const char* Name) = 0;
	virtual int	RenameUnitGroup(const char* Unit, const char* Name, const char* NewName, const STRINGLIST& friends) = 0;
	virtual int	DeleteUnitGroup(const char* Unit, const char* Name, const STRINGLIST& friends) = 0;
	virtual int	MoveUnitGroup(const char* Unit, const char* Name, const char* Name2, const STRINGLIST& friends) = 0;
	virtual int	CreateUnitMember(const char* Unit, const char* Name, const char* Group) = 0;
	virtual int	DeleteUnitMember(const char* Unit, const char* Name) = 0;
	virtual int	MoveUnitMember(const char* Unit, const char* Name, const char* Group) = 0;
	virtual int	SetUnitMemberStatus(const char* Unit, const char* Name, const char* status) = 0;
};

struct AddinNotify : public FriendNotify
{
	virtual int	CreateUnit(const char* Unit);
	virtual int	DeleteUnit(const char* Unit);
	virtual int	CreateUnitGroup(const char* Unit, const char* Name);
	virtual int	RenameUnitGroup(const char* Unit, const char* Name, const char* NewName, const STRINGLIST& friends);
	virtual int	MoveUnitGroup(const char* Unit, const char* Name, const char* Name2, const STRINGLIST& friends);
	virtual int	DeleteUnitGroup(const char* Unit, const char* Name, const STRINGLIST& friends);
	virtual int	CreateUnitMember(const char* Unit, const char* Name, const char* Group);
	virtual int	DeleteUnitMember(const char* Unit, const char* Name);
	virtual int	MoveUnitMember(const char* Unit, const char* Name, const char* Group);
	virtual int	SetUnitMemberStatus(const char* Unit, const char* Name, const char* status);
};

struct ServerNotify : public AddinNotify
{
	virtual int	CreateUnit(const char* Unit);
	virtual int	RenameUnitGroup(const char* Unit, const char* Name, const char* NewName, const STRINGLIST& friends);
	virtual int	MoveUnitGroup(const char* Unit, const char* Name, const char* Name2, const STRINGLIST& friends);
	virtual int	DeleteUnitGroup(const char* Unit, const char* Name, const STRINGLIST& friends);
	virtual int	DeleteUnitMember(const char* Unit, const char* Name);
	virtual int	MoveUnitMember(const char* Unit, const char* Name, const char* Group);

	void	SendSyncData(const char* Unit);
	void	SendNotifyGroupFriend(const char* Unit, const std::string& group, const STRINGLIST& friends);
	void	SendNotifyDeleteFriend(const char* Unit, const char* Name);
	int m_nRef;
};

struct KUiPlayerItem;

class KUiChatCentre : public KWndShowAnimate
{
public:
	//----界面面板统一的接口函数----
	static KUiChatCentre*	OpenWindow(bool bShow);		//打开窗口，返回唯一的一个类对象实例
	static KUiChatCentre*	GetIfVisible();		//如果窗口正被显示，则返回实例指针
	static void				CloseWindow(bool bDestroy);	//关闭窗口，同时可以选则是否删除对象实例
	static void				LoadScheme(const char* pScheme);//载入界面方案	

	static void				CreateSeverUnit();
	static BOOL				LoadPrivateSetting(KIniFile* pFile);
	static int				SavePrivateSetting(KIniFile* pFile);

	static void				UpdateData(UICHATCENTRE_UPDATE eFlag,
								unsigned int uParam, int nGroupIndex);
	static bool				IsMyFriend(char* Name);

	static bool				AddFriendInfo(int nUnitIndex, char* Name, char* group);
	static bool				FriendStatus(int nUnitIndex, char* Name, BYTE state);
	static void				DeleteFriend(int nUnitIndex, char* Name, bool bNotify);
	static void				NewGroup(int nUnitIndex, const char* szGroup);
	static void				ReplaceGroupName(int nUnitIndex, const char* sz, const char* szGroup);
	static void				MoveFriendToGroup(int nUnitIndex, const char* Name, const char* group);
	static void				MoveGroupToGroup(int nUnitIndex, const char* group, const char* group2, bool bDelgroup);

	static int				FindUnitIndex(const char* unit);
	static int				FindUnitIndexByRoleNameAtServerUnit(char* roleName);
	//根据给出的Unit索引获取Unit名字
	static const char*      QueryUnitNameByUnitIndex(int nUnit);
	static int				AddAddinUnit(const char* unit, FriendNotify* pNotify);
	static int				RemoveAddinUnit(const char* unit);
	static int				FindAddinUnitIndex(const char* unit);

private:

	KUiChatCentre();
	~KUiChatCentre() {}
	void	Initialize();							//初始化
	void	LoadScheme(class KIniFile* pIni);
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);//窗口函数
	
	void	CancelMenu();
	void	ClearInterface();
	void	ClearFriendList();
	void	ClearUnitData(int nUnit);

	void	OnClickButton(KWndWindow* pBtn);
	void	OnSelGroupMenu(int nGroupIndex, int nCmd);
	void	OnSelFriendMenu(int nFriendIndex, int nCmd);
	void	OnMoveFriendToGroup(int nFriendIndex, int nNewGroupIndex);
	bool	OnClickGroupBtn(KWndWindow* pBtn, bool bLeftClick);
	void	OnScrollList(bool bUp);
	void	OnSelUnit(int nUnitIndex);
	
	void	PopupupGroupMenu(int nGroupIndex);
	void	PopupFriendMenu(int nFriendIndex);
	void	PopupSelGroupMenu(int nIndexInMenu);
	void	PopupSelUnitMenu();

	void	SendFriendMsg(int nFriendIndex, bool bDBClick);

	void	UpdateAll(int nGroupIndex);
	void	UpdateGroup(int nGroupIndex);
	void	UpdateFriend(KUiPlayerItem* pFriend, int nGroupIndex);
	void	UpdateInterface();
	
	void	PaintWindow();							//绘制窗口

	void	DragWndSize(int nMoveOffset);
	void	AdjustSize(int nNumLine);
	int		PtInWindow(int x, int y);				//判断一个点是否在窗口范围内,传入的是绝对坐标
	
private:
	typedef std::map<std::string, std::string> FRIENDINFOLIST;
	typedef std::map<std::string, BYTE> FRIENDONLINELIST;
	struct GROUPINFO
	{
		std::string strGroupName;
		BYTE nGroupMemberNum;
	};

	struct FriendUnit
	{
		std::string m_UnitName;
		FRIENDINFOLIST m_FriendMap;
		FRIENDONLINELIST m_FriendOnlineMap;
		GROUPINFO	m_GroupInfo[MAX_GROUP_NUM];
		int m_nCurExpandGroup;
		int	FindGroupIndex(const std::string& str);
		FriendNotify* m_pNotify;
	};

	int m_nCurentFriendUnit;
	FriendUnit m_AllFriendUnits[MAX_UNIT_NUM];
	int m_nFriendUnitCount;

	static ServerNotify ms_SNotify;

	static KUiChatCentre* m_pSelf;
private:
	KWndImage			m_TailImg;
	KWndButton			m_FindBtn;
	KWndButton			m_CloseBtn;
	KWndButton			m_ScrollUpBtn, m_ScrollDownBtn;
	KWndPureTextBtn			m_UnitBtnFriend;
	KWndPureTextBtn			m_UnitBtnBrother;
	KWndPureTextBtn			m_UnitBtnEnemy;
	KWndPureTextBtn			m_UnitBtnOther;
	KWndButton				m_BtnInvisible;

	KWndLabeledButton	m_GroupBtn[MAX_GROUP_NUM];

	KWndList			m_FriendList;
	
	bool				m_bValidGroup[MAX_GROUP_NUM];
	int					m_nNumValidGroup;
	bool				m_bPopupMenu;
	int					m_nNumFriendInList;
	KUiPlayerItem*		m_pFriendList;

	unsigned int		m_uFriendOffLineColor;
	int					m_nMaxVisibleLine, m_nMinVisibleLine;
	int					m_nLineHeight;

	int					m_nFirstGroupBtnLeft, m_nFirstGroupBtnTop;
	int					m_nFirstVisibleLine;
	int					m_nNumVisibleLine;
	int					m_nTailImgDragOffsetY;

	//好友列表背景图
	KRUImage			m_ListBgImage;
	int					m_ListBgImgLeft;
	int					m_ListBgImgTop;
	int					m_ListBgImgWidth;
	int					m_ListBgImgHeight;

#define	GROUP_MENU_ITEM_COUNT	3
#define	FRIEND_MENU_ITEM_COUNT	3
	//弹出菜单的内容
	char	m_GroupMenuData[MENU_DATA_SIZE(GROUP_MENU_ITEM_COUNT)];
	char	m_FriendMenuData[MENU_DATA_SIZE(FRIEND_MENU_ITEM_COUNT)];
	struct KPopupMenuData*	m_pSelGrupMenu;
};