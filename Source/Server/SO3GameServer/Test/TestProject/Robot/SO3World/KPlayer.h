/************************************************************************/
/* 玩家			                                                        */
/* Copyright : Kingsoft 2005										    */
/* Author	 : Zhu Jianqiu												*/
/* History	 :															*/
/*		2004.12.30	Create												*/
/************************************************************************/
#ifndef _KPLAYER_H_
#define	_KPLAYER_H_

#include <list>
#include "SO3ProtocolBasic.h"
#include "KCharacter.h"
#include "KCircleList.h"
//#include "KSkillList.h"
#include "KCDTimerList.h"
#include "KItemList.h"
#include "KRoleDBDataDef.h"
#include "KReputation.h"

class IKG_Buffer;

//玩家游戏状态：
enum GAME_STATUS
{
	gsInvalid = 0,			//无效状态

	gsWaitForLogin,			//正在等待客户端的连接
	gsWaitForPermit,		//登陆时正在等待GC的确认
    gsWaitForRoleData,		//登录时正在等待Goddess的回包
	gsSyncData,				//正在向客户端同步角色数据

	gsPlaying,				//正在游戏中

	gsSearchMap,			//正在搜索地图,不一定需要跨服
	gsTransferData,			//跨服时正在传输角色数据

	gsLogouting,			//客户端主动请求的延时登出,连接仍然在
	gsDeleting,				//客户端断线引起的延时登出
	gsWaitForDisconnect,	//跨服时源服务器等待客户端主动断线

	gsTotal
};

struct KWindowSelectList
{
	KTarget			Target;
	DWORD			dwScriptID;		//脚本ID
	DWORD			dwIndex;		//流水号
	int				nFuncCount;		//函数个数
	DWORD			dwMapID;
	DWORD			dwMapCopyIndex;
	int				nX;
	int				nY;
	int				nZ;
	char			szFuncName[MAX_WINDOW_SELECT_COUNT][MAX_SELECT_FUNCNAME_LEN];
};

enum KCHARACTER_REVIVE_STATE
{
    crsInvalid = 0,
    crsWaitingForRevive,     // 死亡状态中,等待复活时间到或者是被复活
    crsConfirmRevive,        // 被复活中,A要复活B,但B还未回复
    crsTotal
};

struct KPLAYER_REVIVE_PARAM
{
    time_t                  nLastDieTime;              // 上次死亡时间
    time_t                  nLastSituReviveTime;       // 上次原地复活时间
    DWORD                   dwSituReviveCounter;       // 原地复活计数器, 用于计算玩家死亡惩罚时间
    KCHARACTER_REVIVE_STATE eState;

    int                     nLife;
    int                     nMana;
    DWORD                   dwMapID;
    int                     nX;
    int                     nY;
    int                     nZ;
};

// 玩家接到死亡通知后的应答码
enum KDEATH_RESPOND_CODE
{
    drcInvalid,
    drcReviveInSitu,        // 原地复活
    drcRevivedByPlayer,     // 被玩家复活
    drcRevivedInAltar,      // 复活点复活      
    drcTotal
};

// 聊天同步物品信息
struct TALK_SYNC_ITEM
{
    int nType;
    unsigned uDataSize;
    BYTE byData[0];
};

class KPlayer : public KCharacter
{
public:
	// GUID的作用是校验连接到GS上的用户的合法性
	GUID				m_Guid;

	ROLE_TYPE			m_eRoleType;

	//玩家游戏状态
	GAME_STATUS			m_eGameStatus;
	//计时器,一共4个地方使用:断线/主动登出/跨服时等待客户端主动断线/登陆时
	int					m_nTimer;
	
	//玩家有效技能列表
//	KSkillList			m_SkillList;
	
	//玩家的Cooldown计时器表
	KCDTimerList		m_TimerList;
	//道具相关
	KItemList			m_ItemList;			//玩家的道具集合，装备的、道具栏的、箱子里的
	//声望相关
	KReputation			m_ReputeList;		//势力声望

//	KPlayerPK			m_PK;				//PK相关的开关

	KWindowSelectList	m_SelectList;		//选择回调函数列表

	//玩家所属阵营
	KCAMP				m_eCamp;

    //玩家所属队伍的ID
    DWORD               m_dwPartyID;
    //队长标志，表示我是不是队长
    BOOL                m_bPartyLeaderFlag;

	//剩余的双倍经验值
	int					m_nDoubleExp;

	WORD				m_wRepresentId[REPRESENT_COUNT];

    // ----------------- 技能相关 ---------------------------->
    BOOL				m_bOnPracticeRoom;          // 是否在练功房
    int					m_nQiToSkillExpPercentAdd;  // 消耗的气对所加技能熟练度的百分比加成

//玩家相关操作接口
public:
	KPlayer();
	~KPlayer();

	BOOL Init(void);
	BOOL UnInit(void);

	BOOL Activate(void);

    void SetDisplayData(
        char	szRoleName[_NAME_LEN],
        int     nRoleType,
        WORD	wRepresentId[REPRESENT_COUNT],
		int		nLevel
    );

	void AddExp(int nExpIncrement, BOOL bUseDoubleExp = FALSE);
	//以set和get为前缀的函数，表明是Lua宏生成的属性访问函数，可以在派生类中重载
	void setLevel(int nLevel);	

	virtual BOOL ChangeRegion(KRegion* pDestRegion);
	virtual BOOL ChangeCell(KCell* pDestCell);

	int GetPlayerRelation(KPlayer* pTarget);
	int GetNpcRelation(KNpc* pTarget);
    
    // 该函数作用: 当客户端玩家当前没有目标, 并且一个Npc或Player试图对其释放一个有害技能时,
    // 客户端玩家就会把当前目标切换到试图攻击他的Npc或Player
    BOOL SwitchSelectTarget(DWORD dwSkillSrcID);

	//出错时,输出Player的信息
	void Dump(void);
	//保存现在的玩家坐标
	BOOL SaveCurrentPosition(void);
	//恢复玩家的原始位置
	BOOL RestorePosition(void);

    // ----------------------- 气 ------------------------------->
    int m_nCurrentQi;

    BOOL AddLogOutQi(time_t TotalLogOutTime);
    BOOL SetQi(int nQi);

    //<----------------------- 生活技能 --------------------------------

    //------------------------ 生活技能 ------------------------------->
    int	m_nCurrentStamina;
    int	m_nStaminaReplenish;
    int m_nStaminaReplenishNextFrame;
    
	//------------------------ 聊天接口 ------------------------------->
#ifdef _CLIENT
    BOOL CopyTalkData(unsigned uSize, BYTE* pbyData);
#endif
#ifdef _SERVER
    BOOL SendSystemMessage(const char cszText[]);
#endif
    BOOL TalkToOne( const char cszTalkTo[],       unsigned uTalkDataSize, BYTE* pbyTalkData);    //密聊
    BOOL TalkToMany(PLAYER_TALK_RANGE eTalkRange, unsigned uTalkDataSize, BYTE* pbyTalkData);    //群聊（eg. 小队、帮会、自定义频道）
	//<----------------------- 聊天接口 --------------------------------

private:
	//跨服时保存旧坐标,供恢复用
	struct KOldPosition
	{
		int				m_nX;
		int				m_nY;
		int				m_nZ;
		DWORD			m_dwMapID;
		DWORD			m_dwMapCopyIndex;
	}m_OldPosition;

    static BYTE     s_byTalkData[MAX_CHAT_TEXT_LEN];
    static unsigned s_uTalkDataSize;
	//检测角色当前所在格子是否有Trap脚本
	BOOL CheckTrap(KCell* pOldCell);
	DWORD m_dwLastScriptID;
	//处理本循环的指令，对于玩家来说是指令列表
	virtual BOOL GetCommand(void);


	//使用物品之前，对物品挂有技能情况的判断
	int ItemTryCastSkillXYZ(KItem* pItem, KTarget &Target);
	int ItemTryCastSkill(KItem* pItem, KTarget &Target);

public:
	BOOL SetDoubleExp(DWORD dwDoubleExp); 
    int GetCDValue(DWORD dwCooldownID);

    // 武功装备调用流程: Client->LuaMountKungfu->server->MountKungfu->Client->MountKungfu .
    BOOL    MountKungfu(DWORD dwID, DWORD dwLevel);
    BOOL    UmountKungfu();

public:
    int CanRevive();
    KPLAYER_REVIVE_PARAM m_ReviveParam;
    
public:
	//脚本接口定义
	DECLARE_LUA_CLASS(KPlayer);

    DECLARE_LUA_ENUM(Camp);
	DECLARE_LUA_BOOL(PartyLeaderFlag);
	DECLARE_LUA_DWORD(PartyID);
	DECLARE_LUA_ENUM(RoleType);
	DECLARE_LUA_INTEGER(DoubleExp);

	int LuaGetItem(Lua_State* L);
	int LuaDelItem(Lua_State* L);

	int LuaGetItemByIndex(Lua_State* L);
	
	int LuaGetMoney(Lua_State *L);

	int LuaExchangeItem(Lua_State* L);

	int LuaDestroyItem(Lua_State* L);
	int LuaGetRepresentId(Lua_State* L);

	int LuaGetBoxSize(Lua_State* L);
	int LuaCanExchange(Lua_State* L);

	int LuaGetEquipPos(Lua_State* L);	//获取装备可以装备的地方，不能装备返回nil
	int LuaGetFreeRoom(Lua_State* L);
	int LuaSatisfyRequire(Lua_State* L);	//是否满足装备需求

	int LuaGetItemAmount(Lua_State* L);

    int LuaGetSkillList(Lua_State* L);
	
	int LuaCastProfessionSkill(Lua_State* L);

    int LuaWindowSelect(Lua_State* L);
    int LuaDoDeathRespond(Lua_State* L);

	int LuaOnCloseLootWindow(Lua_State* L);
    int LuaCastCommonSkill(Lua_State* L);
    int LuaStopCurrentAction(Lua_State* L);

    int LuaGetSkillCDProgress(Lua_State* L);
    int LuaGetCDInterval(Lua_State* L);
    int LuaGetCDLeft(Lua_State* L);
    int LuaGetSkillLevel(Lua_State* L);

    int LuaGetSkillLifeCost(Lua_State* L);
    int LuaGetSkillManaCost(Lua_State* L);
    int LuaGetSkillRageCost(Lua_State* L);
    int LuaGetSkillStaminaCost(Lua_State* L);

    int LuaGetSkillPrepare(Lua_State* L);
    int LuaGetSkillChannel(Lua_State* L);

    int LuaGetSkillMinRadius(Lua_State* L);
    int LuaGetSkillMaxRadius(Lua_State* L);

    int LuaGetSkillExp(Lua_State* L);

    int LuaMountKungfu(Lua_State* L);
    int LuaUmountKungfu(Lua_State* L);
    int LuaGetKungfuMount(Lua_State* L);

	int LuaUseItemOnSelf(Lua_State* L);
	int LuaUseItemOnTarget(Lua_State* L);
	int LuaUseItemOnCoordination(Lua_State* L);

	int	LuaGetOTActionState(Lua_State* L);

	//--- AI ----------------------------------
	int LuaFollowSelectTarget(Lua_State* L);
	int LuaStopFollow(Lua_State* L);

	int LuaSearchForAllies(Lua_State* L);
	int LuaSearchForEnemy(Lua_State* L);
	//-----------------------------------------

	int LuaUseItem(Lua_State* L);
	int LuaGetItemPos(Lua_State* L);

    int LuaSearchForPartyMember(Lua_State* L);

    int LuaTradingInviteRequest(Lua_State* L);
    int LuaTradingInviteRespond(Lua_State* L);
    int LuaTradingAddItem(Lua_State* L);
    int LuaTradingDeleteItem(Lua_State* L);
    int LuaTradingSetMoney(Lua_State* L);
    int LuaTradingConfirm(Lua_State* L);
    //--- Talk --------------------------------
    int LuaGetTalkData(Lua_State* L);
    int LuaGetTalkLinkItem(Lua_State* L);
    int LuaTalkToOne(Lua_State* L);
    int LuaTalkToMany(Lua_State* L);
    int LuaDoAnimation(Lua_State* L);
    //-----------------------------------------
    int LuaAddRelation(Lua_State* L);
    int LuaDelRelation(Lua_State* L);
};
#endif	//_KPLAYER_H_
