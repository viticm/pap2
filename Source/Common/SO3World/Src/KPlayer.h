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
#include "KSkillList.h"
#include "KProfessionList.h"
#include "KRecipeList.h"
#include "KCDTimerList.h"
#include "KQuestInfoList.h"
#include "KQuestList.h"
#include "KGPQ.h"
#include "KItemList.h"
#include "KRoleDBDataDef.h"
#include "KPatrolPath.h"
#include "KReputation.h"
#include "KPKController.h"
#include "KTradingBox.h"
#include "KUserPreferences.h"
#include "KBookList.h"
#include "KSkillRecipeList.h"
#include "KCustomData.h"
#include "KAchievement.h"
#include "KDesignation.h"
#include "KProbability.h"

#ifdef _SERVER
#include "KScriptTimerList.h"
#include "KAntiFarmer.h"
#endif //_SERVER

#define MAX_POSITION_RECORD	            32
#define PLAYER_SCRIPT                   SCRIPT_DIR"/player/PlayerScript.lua"
#define PLAYER_EXT_POINT_SCRIPT         SCRIPT_DIR"/player/PlayerExtPoint.lua"
#define PLAYER_PRESENT_CODE_SCRIPT      SCRIPT_DIR"/player/PlayerPresentCode.lua"
#define CLIENT_PLAYER_ACTIVATE_SCRIPT   SCRIPT_DIR"/player/ClientPlayerActivate.lua"

class IKG_Buffer;
class S2C_PLAYER_DISPLAY_DATA;

enum GAME_STATUS
{
	gsInvalid = 0,			    // 无效状态

	gsWaitForConnect,			// 正在等待客户端的连接
	gsWaitForPermit,		    // 登陆时正在等待GC的确认
    gsWaitForRoleData,		    // 登录时正在等待Goddess的回包
    gsWaitForLoginLoading,      // 登录(或跨服)流程中等待客户端载入
	gsWaitForNewMapLoading,     // 切换地图过程中等待客户端载入

	gsPlaying,				    // 正在游戏中

	gsSearchMap,			    // 正在搜索地图,不一定需要跨服
    gsWaitForTransmissionSave,  // 保存数据准备跨服
	gsWaitForTransmissionGuid,	// 跨服时正在等待目标服务器返回GUID

	gsDeleting,				    // 客户端断线引起的延时登出

	gsTotal
};

enum PREEMPTIVE_ATTACK
{
    paInvalid = 0,

    paSelf,
    paTarget,

    paTotal,
};

struct KWindowSelectList
{
	KTarget			Target;
	DWORD			dwScriptID;		//脚本ID
	DWORD			dwIndex;		//流水号
	int				nFuncCount;		//函数个数
	DWORD			dwMapID;
	int			    nMapCopyIndex;
	int				nX;
	int				nY;
	int				nZ;
	char			szFuncName[MAX_WINDOW_SELECT_COUNT][MAX_SELECT_FUNCNAME_LEN];
};

#define KPLAYER_REVIVE_CTRL_UI_TEXT_LEN 32

struct KPLAYER_REVIVE_CTRL 
{
    int     nNextCheckReviveFrame;  // 服务器下次检查复活条件的时间
    int     nReviveFrame;           // 复活倒计时，会被同步给界面

    BOOL    bReviveInSitu;
    BOOL    bReviveInAltar;
    BOOL    bReviveByPlayer;

    time_t  nLastSituReviveTime;    // 上次原地复活时间
    int     nSituReviveCount;       // 原地复活计数器, 用于计算玩家死亡惩罚时间
    int     nMessageID;

    DWORD   dwReviver;                 // 被玩家A复活的时候,这个表示A的ID,下面XYZ是A的坐标
    int     nX;                        
    int     nY;
    int     nZ;
    int     nReviveLife;
    int     nReviveMana;
};

// 玩家接到死亡通知后的应答码
enum KDEATH_RESPOND_CODE
{
    drcInvalid,
    drcReviveInSitu,        // 原地复活
    drcReviveByPlayer,      // 被玩家复活
    drcReviveInAltar,       // 复活点复活
    drcCancelReviveByPlayer,// 取消被玩家复活
    drcTotal
};

// 聊天同步物品信息
struct TALK_SYNC_ITEM
{
    unsigned uDataSize;
    BYTE byData[0];
};

struct TALK_SYNC_ITEM_INFO
{
    int     nVersion;
    DWORD   dwTabType;
    int     dwIndex;
};

struct TALK_SYNC_QUEST 
{
    DWORD   dwQuestID;
};

struct TALK_SYNC_RECIPE 
{
    DWORD   dwCraftID;
    DWORD   dwRecipeID;
};

struct TALK_SYNC_ENCHANT 
{
    DWORD   dwProID;
    DWORD   dwCraftID;
    DWORD   dwRecipeID;
};

struct TALK_SYNC_SKILL_RECIPE 
{
    DWORD   dwID;
    DWORD   dwLevel;
};

struct TALK_SYNC_BOOK
{
    TALK_SYNC_ITEM_INFO ItemInfo;
    int   nBookRecipeID;
};

struct TALK_SYNC_ACHIEVEMENT
{
	DWORD dwAchievementID;
};

struct TALK_SYNC_DESIGNATION
{
	DWORD dwDesignationID;
	DWORD dwPrefix;
};

struct KROLE_POSITION
{
    DWORD	dwMapID;
    int	    nMapCopyIndex;
    int		nX;
    int		nY;
    int		nZ;
    int     nFaceDirection;
};

struct KSHARE_QUEST_RESULT
{
    DWORD dwQuestID;
    DWORD dwTargetPlayerID;
};

struct KMOVE_CTRL 
{
    BOOL bDisable;      // 禁止玩家作主动控制
    BOOL bMove;         // 是否在移动
    BOOL bWalk;         // 是否在Walk(否则是Run,或者Swim)
    int  nDirection8;   // 移动的方向,相对Face direction,只有8个方向数值[0,7]
    int  nTurn;         // 0, 不转,1,顺转,-1反转
    int  nMoveCount;    // 客户端移动操作计数器
    BOOL bUpdate;
};

// 观察点
struct KVIEW_POINT : KNode
{
    KPlayer*    pPlayer;    		// 观察点拥有者
    KRegion*    pRegion;    		// 观察点所在Region
    int         nDisappearFrame;	// 消失帧数
};

void DumpData(char* szFileName, BYTE* pbyData, size_t uDataLen);

class KPlayer : public KCharacter
{
public:
	KPlayer();
	~KPlayer();

	BOOL Init(void);
	void UnInit(void);

	BOOL Activate(void);

public:
	BOOL OpenDoodad(KDoodad* pDoodad);

    void MoveCtrl();

public:
	GUID				m_Guid;

#if defined(_SERVER)
	int					m_nConnIndex;
    char                m_szClientIP[_NAME_LEN];
	char				m_szAccount[_NAME_LEN];
	tagExtPointInfo		m_ExtPointInfo;         // 扩展点
    BOOL                m_bExtPointLock;        // 扩展点操作锁
    int                 m_nLastExtPointIndex;   // 上一次操作扩展点索引
    short               m_nLastExtPointValue;   // 上一次操作扩展点值
    time_t              m_nEndTimeOfFee;        // 收费总截止时间
    time_t              m_nNoFeeTime;           // 发现玩家余额不足的时间
    int                 m_nLastClientFrame;
    int                 m_nNextSaveFrame;
	DWORD				m_dwRoundRollCount;  // 玩家轮流拾取的计数器

	time_t				m_nLastSaveTime; 
    time_t              m_nLastLoginTime;
    time_t              m_nCurrentLoginTime;
    int64_t             m_nTotalGameFrame; // 总在线帧数
    time_t              m_nCreateTime;

	KCustomData<128>    m_CustomData;

	KScriptTimerList<KPlayer, 16>		m_ScriptTimerList;
    
    // 表示玩家将被传送到上一个入口点的时间，为0表示不需传送
    time_t              m_nBanishTime;
    KROLE_POSITION      m_LastEntry;
#endif

    BOOL                m_bChargeFlag;          // 玩家收费状态 false:免费 true:收费
    int                 m_nCoin;                // 玩家的金币(元宝)
    BOOL                m_bFreeLimitFlag;       // 玩家是否因为免费而收到限制
    BOOL                m_bFarmerLimit;         // 是否受到反打钱掉落限制

#if defined(_CLIENT)
    char                m_szTongName[_NAME_LEN];
#endif

    KMOVE_CTRL          m_MoveCtrl;

	ROLE_TYPE			m_eRoleType;

	GAME_STATUS			m_eGameStatus;
	int					m_nTimer;

	KQuestList			m_QuestList;
    KGPQList            m_PQList;
	KSkillList			m_SkillList;

	KProfessionList		m_ProfessionList;
	KRecipeList			m_RecipeList;

	// 玩家的Cooldown计时器表
	KCDTimerList		m_TimerList;
	// 道具相关
	KItemList			m_ItemList;			//玩家的道具集合，装备的、道具栏的、箱子里的

	// 声望相关
	KReputation			m_ReputeList;		//势力声望

	KPKController		m_PK;				//PK相关的开关

	KWindowSelectList	m_SelectList;		//选择回调函数列表

	KUserPreferences	m_UserPreferences;	//  玩家界面数据存盘

	KBookList			m_BookList;			//  玩家的读书进度

    KDesignation        m_Designation;      // 玩家的称号

    // 玩家所属队伍的ID
    DWORD               m_dwSystemTeamID;
    DWORD               m_dwTeamID;

    BOOL SetCurrentTeamID(BOOL bSystem, DWORD dwTeamID);
    DWORD GetCurrentTeamID();
    int GetCurrentTeamGroupIndex();

#ifdef _SERVER
    int                 m_nExpPercent;          // 杀怪后获得经验值的系数
    int                 m_nReputationPercent;   // 杀怪后获得声望值的系数
#endif
	
	int	                m_nExperience;			// 角色经验
	
	DWORD				m_dwBankNpcID;          // 打开银行的Npc
    DWORD               m_dwTongRepertoryNpcID; // 打开帮会仓库的Npc

    KTradingBox*        m_pTradingBox;           
    DWORD               m_dwTradingInviteDst;   // 交易邀请的目标玩家
    DWORD               m_dwTradingInviteSrc;   // 发起交易的玩家
    
    BOOL                m_bOnHorse;             // 是否处于骑马状态
	// 人物装备表现
    WORD				m_wRepresentId[perRepresentCount];
    DWORD               m_dwRepresentIdLock;    // 低perRepresentCount位用于锁定人物表现

    DWORD               m_dwSchoolID;           // 用于存储装备内功的门派，来做一系列的表现判断
    
    // ----------------- 技能相关 ---------------------------->
    BOOL				m_bOnPracticeRoom;          // 是否在练功房

    KSkillRecipeList    m_SkillRecipeList;

    // ----------------- 修为相关 -------------------------------->
    int                 m_nCurrentTrainValue;       // 修为
    int                 m_nMaxTrainValue;           // 最大修为值
    int                 m_nUsedTrainValue;          // 已经冲入经脉的修为值
#ifdef _SERVER
    int                 m_nAddTrainTimeInToday;     // 今天已经加的修为的时间
#endif

    // ----------------- 成就相关 -------------------------------->
    KAchievement        m_Achievement;

    // ----------------- 打通筋脉的成功率 ------------------------>
    int                 m_nBaseVenationCof;         // 基础系
    int                 m_nPhysicsVenationCof;      // 外功系
    int                 m_nMagicVenationCof;        // 内功系
    int                 m_nAssistVenationCof;       // 辅助系

#ifdef _SERVER
    // ----------------- BUFF衰减相关 ------------------------>
    int                 m_nBuffDecayCount[MAX_BUFF_DECAY_TYPE];
    int                 m_nBuffDecayNextFrame[MAX_BUFF_DECAY_TYPE];
#endif //_SERVER

#ifdef _CLIENT
	// ----------------- 小地图雷达 -------------------------->
	MINIMAP_RADAR_TYPE	m_eRadarType;				//雷达类型
	int					m_nRadarParam;				//雷达参数
#endif

    typedef std::vector<int /* nRouteNode */> KOPEN_ROUTE_NODE_VECTOER;

    KOPEN_ROUTE_NODE_VECTOER m_OpenRouteNodeList;
    
    // 玩家访问场景的记录
    KCustomData<MAX_MAP_ID_DATA_SIZE> m_SceneVisitRecord;
    
    //  ------------ 杀气值相关 ----------------------------------->
    int     m_nCurrentKillPoint;        // 杀气值, 最大值不能超过65536
#ifdef _SERVER
    time_t  m_nNextKillPointReduceTime; // 下一次杀气值衰减的时间
    BOOL    m_bAbradeEquipmentDouble;   // 背包中和身上的装备扣双倍耐久
    BOOL    m_bAbradeEquipmentAll;      // 背包中和身上的装备扣光耐久
    BOOL    m_bAttackByGuard;           // 被卫兵NPC攻击
#endif
    BOOL    m_bOnlyReviveInSitu;        // 对于能原地复活的场景，只能原地复活    
    BOOL    m_bCannotDialogWithNPC;     // 不能与NPC对话
    BOOL    m_bRedName;                 // 红名
    //  ------------------------------------------------------------>
    DWORD   m_dwTongID;
    int     m_nContribution;

    int     m_nMaxLevel;
    BOOL    m_bHideHat;

#ifdef _SERVER
    int     m_nTalkWorldDailyCount;
    int     m_nTalkForceDailyCount;
    int     m_nTalkCampDailyCount;
#endif

    // -------------- 人物属性加成 --------------------------------->
    int     m_nVitalityToParryValueCof;
    int     m_nVitalityToMaxLifeCof;
    int     m_nVitalityToPhysicsAttackPowerCof;
    int     m_nVitalityToSolarAttackPowerCof;
    int     m_nVitalityToLunarAttackPowerCof;
    int     m_nVitalityToNeutralAttackPowerCof;
    int     m_nVitalityToPoisonAttackPowerCof;
    int     m_nVitalityToTherapyPowerCof;
    int     m_nVitalityToPhysicsShieldCof;
    int     m_nSpiritToMaxManaCof;
    int     m_nSpiritToSolarCriticalStrikeCof;
    int     m_nSpiritToLunarCriticalStrikeCof;
    int     m_nSpiritToNeutralCriticalStrikeCof;
    int     m_nSpiritToPoisonCriticalStrikeCof;
    int     m_nSpiritToPhysicsAttackPowerCof;
    int     m_nSpiritToPhysicsCriticalStrikeCof;
    int     m_nSpiritToManaReplenishCof;
    int     m_nSpiritToSolarMagicShieldCof;
    int     m_nSpiritToLunarMagicShieldCof;
    int     m_nSpiritToNeutralMagicShieldCof;
    int     m_nSpiritToPoisonMagicShieldCof;
    int     m_nSpunkToManaReplenishCof;
    int     m_nSpunkToSolarAttackPowerCof;
    int     m_nSpunkToLunarAttackPowerCof;
    int     m_nSpunkToNeutralAttackPowerCof;
    int     m_nSpunkToPoisonAttackPowerCof;
    int     m_nStrengthToPhysicsCriticalStrikeCof;
    int     m_nStrengthToPhysicsAttackPowerCof;
    int     m_nAgilityToPhysicsCriticalStrikeCof;

#ifdef _SERVER
public:
    // 视野同步相关
    BOOL IsInViewRangeByPlayer(KRegion* pRegion);
    BOOL IsInViewRangeByViewPoint(KRegion* pRegion);
    void SyncObjectView();
    void RegisterViewObjectByPlayer(KRegion* pFromRegion, KRegion* pToRegion);
    void RegisterViewObjectByViewPoint(KRegion* pFromRegion, KRegion* pToRegion);

    struct KViewObject 
    {
        KViewObject() {};
        KViewObject(DWORD dwID, int nFrame) : dwObjectID(dwID), nViewFrame(nFrame) {};
        DWORD dwObjectID;
        int   nViewFrame;
    };

    typedef std::set<DWORD, std::less<DWORD>, KMemory::KAllocator<DWORD> > KVIEW_SYNC_TABLE;
    typedef std::list<KViewObject, KMemory::KAllocator<KViewObject> > KVIEW_SYNC_QUEUE;

    KVIEW_SYNC_TABLE    m_PlayerSyncTable;
    KVIEW_SYNC_QUEUE    m_PlayerSyncQueue;

    KVIEW_SYNC_TABLE    m_NpcSyncTable;
    KVIEW_SYNC_QUEUE    m_NpcSyncQueue;

    KVIEW_SYNC_TABLE    m_DoodadSyncTable;
    KVIEW_SYNC_QUEUE    m_DoodadSyncQueue;
#endif

public:
    // 观察点
#ifdef _SERVER
    KVIEW_POINT         m_ViewPoint;
#else
    KRegion*            m_pViewPointRegion;
#endif


public:
#ifdef _CLIENT
	void SetDisplayData(S2C_PLAYER_DISPLAY_DATA& rPlayerDisplayData);
#endif

    void SetGenderByRoleType(int nRoleType);

#ifdef _SERVER
    BOOL    Load(BYTE* pbyData, size_t uDataLen);
    BOOL    LoadExtRoleData(BYTE* pbyData, size_t uDataLen);
    BOOL    LoadSkillRecipeList(BYTE* pbyData, size_t uDataLen);
    BOOL    LoadQuestData(BYTE* pbyData, size_t uDataLen);
    BOOL    LoadStateInfo(BYTE* pbyData, size_t uDataLen);
    BOOL    LoadRoadOpenNode(BYTE* pbyData, size_t uDataLen);
    BOOL    LoadHeroData(BYTE* pbyData, size_t uDataLen);

    BOOL    CallLoginScript();
    BOOL    RefreshDailyVariable(int nDays);

    BOOL    Save(size_t* puUsedSize, BYTE* pbyBuffer, size_t uBufferSize);
    BOOL    SaveSkillRecipeList(size_t* puUsedSize, BYTE* pbyBuffer, size_t uBufferSize);
    BOOL    SaveQuestStateAndList(size_t* puUsedSize, BYTE* pbyBuffer, size_t uBufferSize);
    BOOL    SaveStateInfo(size_t* puUsedSize, BYTE* pbyBuffer, size_t uBufferSize);
    BOOL    SaveRoadOpenNode(size_t* puUsedSize, BYTE* pbyBuffer, size_t uBufferSize);
    BOOL    SaveHeroData(size_t* puUsedSize, BYTE* pbyBuffer, size_t uBufferSize);
    BOOL    SavePosition();
    // 调用这个函数之前应该确保m_SavePosition被正确的设置(比如通过SavePosition)
    BOOL    SaveBaseInfo(KRoleBaseInfo* pBaseInfo);
    BOOL    LoadBaseInfo(KRoleBaseInfo* pBaseInfo);

    // 保存角色数据,或者在Load时,先要把玩家的位置信息保存到这个结构里面,因为过程中
    // 需要访问的Scene,Cell等指针可能为空
	KROLE_POSITION m_SavePosition;

    // 为延迟调用 SwitchMap 保存参数
    KROLE_POSITION m_DelayedSwitchMapParam;

    void    SwitchMap(DWORD dwMapID, int nCopyIndex, int nX, int nY, int nZ);
    BOOL    RealSwitchMap(DWORD dwMapID, int nCopyIndex, int nX, int nY, int nZ);
#endif

public:
#ifdef _SERVER
    BOOL CheckRevive(BOOL bFirstCheck);
    BOOL Revive(int nMode);
#endif

#ifdef _SERVER
	BOOL	CallDeathScript(DWORD dwKiller);
	void	ReplenishAttribute();		// 恢复属性,主要是玩家专有的精气神等
#endif	//_SERVER

	void AddExp(int nExpIncrement);
#ifdef _SERVER
    BOOL AddContribution(int nContribution);

    BOOL CallLevelupScript();
#endif
	void SetLevel(int nLevel);	

	virtual BOOL ChangeRegion(KRegion* pDestRegion);
#ifdef _SERVER
    void SetCell(KCell* pDstCell);

    BOOL CallTrapScript(DWORD dwScript, const char* szFunction);

	DWORD m_dwLastScriptID;
#endif

	int GetPlayerRelation(KPlayer* pTarget);
	int GetNpcRelation(KNpc* pTarget);

    BOOL IsInRaid();
    
#ifdef _CLIENT
    // 该函数作用: 当客户端玩家当前没有目标, 并且一个Npc或Player试图对其释放一个有害技能时,
    // 客户端玩家就会把当前目标切换到试图攻击他的Npc或Player
    BOOL SwitchSelectTarget(DWORD dwSkillSrcID);
#endif

	//------------------------ 生活技能 ------------------------------->
	int	m_nCurrentStamina;
    int m_nCurrentThew;
    
    int	m_nMaxStamina;
    int m_nMaxThew;

	BOOL CostStamina(int nStamina);
    BOOL CostThew(int nThew);

	BOOL LearnProfession(DWORD dwProfessionID);
	BOOL LearnBranch(DWORD dwProfessionID, DWORD dwBranchID);
	BOOL ForgetProfession(DWORD dwProfessionID);
	BOOL AddProfessionProficiency(DWORD dwProfessionID, DWORD dwExp);
	BOOL SetMaxProfessionLevel(DWORD dwProfessionID, DWORD dwLevel);

	BOOL CanLearnRecipe(DWORD dwCraftID, DWORD dwRecipeID, DWORD dwMasterID);
	BOOL LearnRecipe(DWORD dwCraftID, DWORD dwRecipeID);
	BOOL ForgetRecipe(DWORD dwProfessionID);

	CRAFT_RESULT_CODE CanCastProfessionSkill(DWORD dwCraftID, DWORD dwRecipeID, DWORD dwRBookItemID, KTarget& Target);
	CRAFT_RESULT_CODE CastProfessionSkill(DWORD dwCraftID, DWORD dwRecipeID, DWORD dwRBookItemID, KTarget& Target);
	//<----------------------- 生活技能 --------------------------------

#ifdef _CLIENT
    BOOL CanFinishQuestOnNpc(KNpc* pNpc);
    BOOL CanFinishTheQuestOnNpc(DWORD dwQuestID, KNpc* pNpc);
    QUEST_DIFFICULTY_LEVEL GetQuestDiffcultyLevel(DWORD dwQuestID); // 取得任务的难易度
#endif

#ifdef _CLIENT
    BOOL CopyTalkData(DWORD dwTalkerID, size_t uSize, BYTE* pbyData);
#endif

private:

#ifdef _CLIENT
    static BYTE     s_byTalkData[MAX_CHAT_TEXT_LEN];
    static unsigned s_uTalkDataSize;
    static BOOL     s_bFilterTalkText;
#endif

public:
    int GetCDValue(DWORD dwCooldownID);

    // 武功装备调用流程: Client->LuaMountKungfu->server->MountKungfu->Client->MountKungfu .
    BOOL    MountKungfu(DWORD dwID, DWORD dwLevel);
    BOOL    UmountKungfu();
#if defined _CLIENT
	int IsEquiped(KCustomEquipInfo *pInfo);
#endif

    // ----------- 位置移动的历史记录 ------------------
#if defined(_SERVER)

    // 注意: 什么参数不能放在这个"历史记录"里面?
    // 放在历史记录里面的参数就意味着会发生会滚,那么显然:
    // ----对于某个参数X,如果从1000帧回滚到990帧后,它在1000帧时的数值以后会从此丢失或者发生错误的,就不能回滚
    // ----比如: 凡是会被技能Buff修改(包括间接修改)的东西,都是不能放在这里面回滚的,包括比如m_nMaxLife, m_nRunSpeed等等...

    KPOSITION_RECORD    m_PositionRecord[MAX_POSITION_RECORD];
	int                 m_nVirtualFrame;   
	int                 m_nRecordCount;

    BOOL ReverseFrame(int nFrame);
    BOOL CheckMoveAdjust(int nClientFrame, const KMOVE_CRITICAL_PARAM& crParam);
#endif

#ifdef _SERVER
    // 自身数值上次同步值
	int		m_nLastSyncSelfMaxLife;
    int		m_nLastSyncSelfMaxMana;  
    int     m_nLastSyncSelfMaxRage;
    int     m_nLastSyncSelfMaxStamina;
	int		m_nLastSyncSelfCurrentLife;
	int		m_nLastSyncSelfCurrentMana; 
    int     m_nLastSyncSelfCurrentRage;
    int     m_nLastSyncSelfCurrentStamina;
    int     m_nLastSyncSelfCurrentThew;
    int     m_nLastSyncSelfUpperWeak;
    int     m_nLastSyncSelfMiddleWeak;
    int     m_nLastSyncSelfLowerWeak;
	// 目标数值的上一次同步值
	int		m_nLastSyncTargetMaxLife;
	int		m_nLastSyncTargetMaxMana; 
    int     m_nLastSyncTargetMaxRage;
	int		m_nLastSyncTargetLifePercent;
    int		m_nLastSyncTargetManaPercent; 
    int		m_nLastSyncTargetRagePercent; 
    int     m_nLastSyncTargetUpperWeak;
    int     m_nLastSyncTargetMiddleWeak;
    int     m_nLastSyncTargetLowerWeak;
    DWORD   m_dwLastSyncTargetTargetID;
    int		m_nLastSyncTargetTargetMaxLife;
    int		m_nLastSyncTargetTargetMaxMana; 
    int		m_nLastSyncTargetTargetMaxRage; 
    int		m_nLastSyncTargetTargetLifePercent;
    int		m_nLastSyncTargetTargetManaPercent; 
    int		m_nLastSyncTargetTargetRagePercent; 
    // 同场景队友上一次同步数值
    int     m_nLastSyncLocalTeamLifePercent;
    int     m_nLastSyncLocalTeamManaPercent;
    int     m_nLastSyncLocalTeamRagePercent;
    DWORD   m_dwLastSyncLocalTeamMemberMapID;
    int     m_nLastSyncLocalTeamMemberMapCopyIndex;
    int     m_nLastSyncLocalTeamMemberPosX;
    int     m_nLastSyncLocalTeamMemberPosY;
    // 跨服同步数据的上一次同步数值
    int     m_nLastSyncGlobalTeamLifePercent;
    int     m_nLastSyncGlobalTeamManaPercent;
    int     m_nLastSyncGlobalTeamRagePercent;
    DWORD   m_dwLastSyncGlobalTeamMemberMapID;
    int     m_nLastSyncGlobalTeamMemberMapCopyIndex;
    int     m_nLastSyncGlobalTeamMemberPosX;
    int     m_nLastSyncGlobalTeamMemberPosY;
    // 目标上次Buff同步
    DWORD	m_dwTargetBuffCRC;
    DWORD	m_dwTargetTargetBuffCRC;
    // 目标上次掉落所属者
    DWORD   m_dwTargetDropID;

    void DoCycleSynchronous();
    void ResetTeamLastSyncParamRecord();
#endif

    BOOL RideHorse();
    BOOL DownHorse();
    
#ifdef _SERVER
    BOOL AddTrain(int nTrain);
    BOOL AddTrainNoLimit(int nTrain);
    BOOL ShareQuest(DWORD dwQuestID);
#endif
    
#ifdef _CLIENT
    DWORD m_dwFormationEffectID;
    int   m_nFormationEffectLevel;

    DWORD m_dwMentorFormationEffectID;
    int   m_nMentorFormationEffectLevel;
#endif

    BOOL IsFormationLeader(); // 阵眼
#ifdef _SERVER
    BOOL CanSetFormationLeader();

    BOOL SyncFormationCoefficient();
    BOOL ProcessTeamMemberAttraction();
    BOOL ProcessTeamMemberOnlyGroupAttraction();
#endif
    KPLAYER_REVIVE_CTRL     m_ReviveCtrl;

#ifdef _SERVER
    time_t  m_nBanTime;
#endif

    BOOL CanAddFoe();
    BOOL IsFoe(DWORD dwDestPlayerID); // 是否在我的仇人列表里

// 阵营----------------------------------->
public:
    KCAMP   m_eCamp;
    BOOL    m_bCampFlag;        //  阵营开关
    time_t  m_nCloseCampFlagTime; // 阵营开关关闭的时间

    int getCloseCampFlagTime(){return (int)m_nCloseCampFlagTime;}
    void setCloseCampFlagTime(int nCloseCampFlagTime){ m_nCloseCampFlagTime = (time_t)nCloseCampFlagTime;}
    
    int     m_nCurrentPrestige; //  威望值
    int     m_nKilledCount;
    time_t  m_nNextResetKilledCountTime;

    BOOL SetCamp(KCAMP eNewCamp);

    BOOL CanOpenCampFlag();
    BOOL CanCloseCampFlag();

#ifdef _SERVER
    BOOL SetCampFlag(BOOL bCampFlag);

    int  m_nClientCampInfoVersion;

    BOOL AddPrestige(int nAddPrestige);
    void ProcessCampPK(DWORD dwKillerID);
    void CallChangeCampScript();

private:
    int GetDiffLevelPercent(int nKillerLevel);
    void CheckReduceKillPoint();
#endif
// <----------------------------------阵营

// 英雄副本----------------------------------->
public:
    BOOL m_bHeroFlag;

    int getHeroFlag(){return m_bHeroFlag;}
    void setHeroFlag(int nValue);

#ifdef _SERVER
    KCustomData<MAX_MAP_ID_DATA_SIZE> m_HeroMapCopyOpenFlag;
#endif
// <-----------------------------------英雄副本
   
// 扩展点操作
public:
#ifdef _SERVER
    BOOL SetExtPoint(int nIndex, short nChangeValue);
#endif

// 修正概率----------------------------------->
#ifdef _SERVER
public:
    BOOL SaveRandData(size_t* puUsedSize, BYTE* pbyBuffer, size_t uBufferSize);
    BOOL LoadRandData(BYTE* pbyData, size_t uDataLen);

    BOOL SaveMentorData(size_t* puUsedSize, BYTE* pbyBuffer, size_t uBufferSize);
    BOOL LoadMentorData(BYTE* pbyData, size_t uDataLen);

    BOOL IsQuestDrop(DWORD dwID, int nIndex, unsigned uProbability);
    BOOL IsVenationSuccess(DWORD dwSkillID, DWORD dwSkillLevel, unsigned uProbability);

private:
    KProbability<uint64_t, MILLION_NUM, 128>    m_QuestRand;
    KProbability<uint64_t, 1024, 32>            m_VenationRand;

public:
    BOOL AddCoin(int nDeltaCoin);   // 修改金币
#endif
// <-----------------------------------修正概率

private:
    void UpdateFreeLimitFlag();

#ifdef _SERVER
public:

    void ProcessAntiFarmer();

    KAntiFarmer m_AntiFarmer;

    void OpenBox(TItemPos& Pos);

    void AddStaminaAndThew();   // 在线回复精力体力

    BOOL CanOpenTongRepertory();
#endif

// 师徒相关
public:
    int     m_nMaxApprenticeNum;
    int     m_nAcquiredMentorValue;
    time_t  m_nLastEvokeMentorTime;
    int     m_nEvokeMentorCount;

public:
	DECLARE_LUA_CLASS(KPlayer);

#ifdef _SERVER
    DECLARE_LUA_STRING(ClientIP, sizeof(m_szClientIP));
    DECLARE_LUA_STRING(Account, sizeof(m_szAccount));
    DECLARE_LUA_DWORD(KillerID);
    DECLARE_LUA_DWORD(LastScriptID);
#endif

    DECLARE_LUA_BOOL(FreeLimitFlag);
    DECLARE_LUA_ENUM(Camp);
    DECLARE_LUA_BOOL(CampFlag);
    DECLARE_LUA_INTEGER(CurrentPrestige);
	DECLARE_LUA_DWORD(TeamID);
	DECLARE_LUA_ENUM(RoleType);
	DECLARE_LUA_INTEGER(Experience);
    DECLARE_LUA_BOOL(OnPracticeRoom);
	DECLARE_LUA_INTEGER(CurrentStamina);
    DECLARE_LUA_INTEGER(CurrentThew);
    DECLARE_LUA_INTEGER(MaxStamina);
    DECLARE_LUA_INTEGER(MaxThew);
	DECLARE_LUA_DWORD(BankNpcID);
    DECLARE_LUA_BOOL(OnHorse);
    
    DECLARE_LUA_DWORD(SchoolID);

    DECLARE_LUA_INTEGER(CurrentTrainValue);
    DECLARE_LUA_INTEGER(MaxTrainValue);
    DECLARE_LUA_INTEGER(UsedTrainValue);
#ifdef _SERVER
    DECLARE_LUA_INTEGER(AddTrainTimeInToday);
#endif

    DECLARE_LUA_INTEGER(BaseVenationCof);
    DECLARE_LUA_INTEGER(PhysicsVenationCof);
    DECLARE_LUA_INTEGER(MagicVenationCof);
    DECLARE_LUA_INTEGER(AssistVenationCof);

#ifdef _SERVER
    DECLARE_LUA_STRUCT_INTEGER(Reviver,     m_ReviveCtrl.dwReviver);
    DECLARE_LUA_STRUCT_INTEGER(ReviveX,     m_ReviveCtrl.nX);
    DECLARE_LUA_STRUCT_INTEGER(ReviveY,     m_ReviveCtrl.nY);
    DECLARE_LUA_STRUCT_INTEGER(ReviveZ,     m_ReviveCtrl.nZ);
    DECLARE_LUA_STRUCT_INTEGER(ReviveLife,  m_ReviveCtrl.nReviveLife);
    DECLARE_LUA_STRUCT_INTEGER(ReviveMana,  m_ReviveCtrl.nReviveMana);
#endif

#ifdef _CLIENT
    DECLARE_LUA_DWORD(FormationEffectID);
    DECLARE_LUA_INTEGER(FormationEffectLevel);

    DECLARE_LUA_DWORD(MentorFormationEffectID);
    DECLARE_LUA_INTEGER(MentorFormationEffectLevel);

	DECLARE_LUA_BOOL(HideHat);
#endif
    
    DECLARE_LUA_INTEGER(CurrentKillPoint);
#ifdef _SERVER
    DECLARE_LUA_BOOL(AbradeEquipmentDouble);
    DECLARE_LUA_BOOL(AbradeEquipmentAll);
    DECLARE_LUA_BOOL(AttackByGuard);
#endif
    DECLARE_LUA_BOOL(OnlyReviveInSitu);
    DECLARE_LUA_BOOL(CannotDialogWithNPC);
    DECLARE_LUA_BOOL(RedName);
    DECLARE_LUA_DWORD(TongID);
    DECLARE_LUA_INTEGER(MaxLevel);
    DECLARE_LUA_INTEGER(Contribution);
    DECLARE_LUA_INTEGER(Coin);

#ifdef _SERVER
    DECLARE_LUA_BOOL(FarmerLimit);
#endif

    DECLARE_LUA_INTEGER(MaxApprenticeNum);
    DECLARE_LUA_INTEGER(AcquiredMentorValue);
    DECLARE_LUA_TIME(LastEvokeMentorTime);
    DECLARE_LUA_INTEGER(EvokeMentorCount);
    
	int LuaGetItem(Lua_State* L);
	int LuaDelItem(Lua_State* L);

	int LuaGetItemByIndex(Lua_State* L);
	int LuaGetEquipItem(Lua_State* L);

	int LuaGetEquipSetItemCount(Lua_State* L);

#ifdef _SERVER
    int LuaHaveItem(Lua_State* L);
    int LuaCostSingleItemByID(Lua_State* L);
    int LuaCanAddItem(Lua_State* L);
    int LuaAddItem(Lua_State* L);
	int LuaCostItem(Lua_State* L);
    int LuaCostItemInAllPackage(Lua_State* L);
	int LuaAddMoney(Lua_State* L);
    int LuaAddContribution(Lua_State* L);
	int LuaOpenWindow(Lua_State* L);
    int LuaGetLastSaveTime(Lua_State* L);
    int LuaGetLastLoginTime(Lua_State* L);
    int LuaGetCurrentLoginTime(Lua_State* L);
    int LuaGetTotalGameTime(Lua_State* L);
    int LuaGetCreateTime(Lua_State* L);
#endif
	
	int LuaGetMoney(Lua_State* L);
	int LuaExchangeItem(Lua_State* L);

	int LuaEnableBankPackage(Lua_State* L);
	int	LuaGetBankPackageCount(Lua_State* L);
	int LuaIsBankPackageEnabled(Lua_State* L);

	int LuaDestroyItem(Lua_State* L);

	int LuaGetRepresentID(Lua_State* L);

#ifdef _SERVER
    int LuaSetRepresentID(Lua_State* L);
#endif
	
	int LuaGetBoxType(Lua_State* L);
	int LuaGetBoxSize(Lua_State* L);
	int LuaGetContainType(Lua_State* L);
	int LuaCanExchange(Lua_State* L);

	int LuaOpenBank(Lua_State* L);
	int LuaCloseBank(Lua_State* L);

	int LuaGetEquipPos(Lua_State* L);	//获取装备可以装备的地方，不能装备返回nil
	int LuaGetFreeRoom(Lua_State* L);
	int LuaGetFreeRoomSize(Lua_State* L);
    int LuaGetBoxFreeRoomSize(Lua_State* L);
	int LuaGetFreeRoomInPackage(Lua_State* L);
	int LuaGetStackRoomInPackage(Lua_State* L);
	int LuaGetStackRoomInBank(Lua_State* L);
	int LuaSatisfyRequire(Lua_State* L);	//是否满足装备需求
    
    int LuaGetItemAmountInAllPackages(Lua_State* L);
	int LuaGetItemAmount(Lua_State* L);
	
	int LuaGetSchoolList(Lua_State* L);
	int LuaGetKungfuList(Lua_State* L);
	int LuaGetSkillList(Lua_State* L);
	int LuaGetAllSkillList(Lua_State* L);
#ifdef _CLIENT
    int LuaGetSkillRecipeKey(Lua_State* L);
	int LuaGetSkillRecipeKeyCount(Lua_State* L);
#endif
	
	int LuaCanAcceptQuest(Lua_State* L);
	int LuaCanFinishQuest(Lua_State* L);
	int LuaGetQuestState(Lua_State* L);
	int LuaGetQuestPhase(Lua_State* L);
	int LuaGetQuestList(Lua_State* L);
#ifdef _CLIENT
	int LuaGetQuestTree(Lua_State* L);
    int LuaGetQuestReputationInfo(Lua_State* L);
#endif

	int LuaGetQuestIndex(Lua_State* L);
	int LuaGetQuestID(Lua_State* L);
	int LuaGetQuestExpAttenuation(Lua_State* L);

#ifdef _CLIENT
	int _LuaGetUnAcceptQuestTraceInfo(Lua_State* L);
	int LuaGetQuestTraceInfo(Lua_State* L);
    int LuaGetQuestAssistDailyCount(Lua_State* L);
#endif

	int LuaSetQuestValue(Lua_State* L);
	int LuaGetQuestValue(Lua_State* L);

	int LuaSetQuestBitValue(Lua_State* L);
	int LuaGetQuestBitValue(Lua_State* L);

	// 设置杀Npc数量
	int LuaSetKillNpcCount(Lua_State* L);
	// 读取杀Npc数量
	int LuaGetKillNpcCount(Lua_State* L);
	// 读取任务剩余时间
	int LuaGetQuestLastTime(Lua_State* L);
	// 设置任务失败标记
	int LuaSetQuestFailedFlag(Lua_State* L);
	// 读取任务失败标记
	int LuaGetQuestFailedFlag(Lua_State* L);

	int LuaAcceptQuest(Lua_State* L);
	int LuaFinishQuest(Lua_State* L);
	int LuaCancelQuest(Lua_State* L);
    int LuaClearQuest(Lua_State* L);
	int LuaShareQuest(Lua_State* L);
    int LuaGetFinishedDailyQuestCount(Lua_State* L);

#ifdef _SERVER
    int LuaForceFinishQuest(Lua_State* L);
#endif
#ifdef _CLIENT
	int LuaAcceptEscortQuest(Lua_State* L);
#endif
    int LuaRegisterQuestAssistance(Lua_State* L);
	
	int LuaGetProfession(Lua_State* L);
	int LuaGetProfessionLevel(Lua_State* L);
    int LuaGetProfessionAdjustLevel(Lua_State* L);
    int LuaGetProfessionMaxLevel(Lua_State* L);
    int LuaGetProfessionProficiency(Lua_State* L);
	int LuaGetProfessionBranch(Lua_State* L);
	int LuaForgetProfession(Lua_State* L);
	int LuaGetRecipe(Lua_State* L);
	int LuaCastProfessionSkill(Lua_State* L);
	int LuaGetBookList(Lua_State* L);
	int LuaGetBookSegmentList(Lua_State* L);
    int LuaIsBookMemorized(Lua_State* L);

	int LuaCanLearnSkill(Lua_State* L);
	int LuaLearnSkill(Lua_State* L);

#if defined(_CLIENT)
	int LuaLearnRecipe(Lua_State* L);
#endif

#if defined(_SERVER)
    int LuaSetLastEntry(Lua_State* L);
	int LuaSwitchMap(Lua_State* L);
    int LuaAutoFly(Lua_State* L);
	int LuaAddExp(Lua_State* L);
    int LuaAddSkillExp(Lua_State* L);
    int LuaForgetSkill(Lua_State* L);

	int LuaLearnProfession(Lua_State* L);
	int LuaLearnBranch(Lua_State* L);
	int LuaAddProfessionProficiency(Lua_State* L);
	int LuaSetMaxProfessionLevel(Lua_State* L);

	int LuaLearnRecipe(Lua_State* L);

	int LuaAddStamina(Lua_State* L);
    int LuaAddThew(Lua_State* L);
	int LuaSetPartyLootMode(Lua_State* L);
	int LuaCanDoCustomOTAction(Lua_State* L);
    int LuaDoCustomOTAction(Lua_State* L);
#endif	//_SERVER

#if defined(_CLIENT)
    int LuaOpenBook(Lua_State* L);
#endif

	int LuaCanLearnProfession(Lua_State* L);
	int LuaCanLearnBranch(Lua_State* L);
	int LuaCanLearnRecipe(Lua_State* L);
    int LuaIsProfessionLearnedByCraftID(Lua_State* L);
	int	LuaIsRecipeLearned(Lua_State* L);

#ifdef _SERVER
    int	LuaForgetRecipe(Lua_State* L);
#endif
	
#if defined(_CLIENT)
	int LuaWindowSelect(Lua_State* L);
    int LuaDoDeathRespond(Lua_State* L);
#endif	//_CLIENT


#if defined(_CLIENT)
	int LuaOnCloseLootWindow(Lua_State* L);
#endif
    int LuaGetCommonSkill(Lua_State* L);
#if defined(_CLIENT)
    int LuaCastCommonSkill(Lua_State* L);
    int LuaStopCurrentAction(Lua_State* L);
#endif //_CLIENT

	int LuaGetItemCDProgress(Lua_State* L);

    int LuaGetSkillCDProgress(Lua_State* L);
    int LuaGetCDInterval(Lua_State* L);
    int LuaGetCDLeft(Lua_State* L);
    int LuaClearCDTime(Lua_State* L);
    int LuaGetSkillLevel(Lua_State* L);

    int LuaGetSkillLifeCost(Lua_State* L);
    int LuaGetSkillManaCost(Lua_State* L);
    int LuaGetSkillStaminaCost(Lua_State* L);

    int LuaGetSkillPrepare(Lua_State* L);
    int LuaGetSkillChannel(Lua_State* L);

    int LuaGetSkillMinRadius(Lua_State* L);
    int LuaGetSkillMaxRadius(Lua_State* L);

    int LuaGetSkillExp(Lua_State* L);

    int LuaMountKungfu(Lua_State* L);
    int LuaUmountKungfu(Lua_State* L);
    int LuaGetKungfuMount(Lua_State* L);
	int	LuaGetOTActionState(Lua_State* L);

#ifdef _SERVER
    int LuaAddSkillRecipe(Lua_State* L);
    int LuaDelSkillRecipe(Lua_State* L);
#endif
    int LuaGetSkillRecipeList(Lua_State* L);
    int LuaIsSkillRecipeExist(Lua_State* L);
    int LuaCanLearnSkillRecipe(Lua_State* L);
    int LuaActiveSkillRecipe(Lua_State* L);
    int LuaDeactiveSKillRecipe(Lua_State* L);

	//--- AI ----------------------------------
#if defined(_CLIENT)
	int LuaFollowSelectTarget(Lua_State* L);

	void HostStopFollow();
	int LuaStopFollow(Lua_State* L);
#endif //_CLIENT

	int LuaSearchForAllies(Lua_State* L);
	int LuaSearchForEnemy(Lua_State* L);
	//-----------------------------------------

#ifdef _CLIENT
	int LuaUseItem(Lua_State* L);
#endif

	int LuaGetItemPos(Lua_State* L);

#ifdef _SERVER
    int LuaUseItemOnNoTarget(Lua_State* L);
    int LuaUseItemOnNpc(Lua_State* L);
    int LuaUseItemOnPlayer(Lua_State* L);
    int LuaUseItemOnItem(Lua_State* L);
    int LuaUseItemOnDoodad(Lua_State* L);
    int LuaUseItemOnCoordinates(Lua_State* L);
#endif

    int LuaSearchForPartyMember(Lua_State* L);

#if defined(_SERVER)
	int LuaAbradeEquipmentOnDeath(Lua_State* L);    

	int LuaSetReputation(Lua_State* L);
	int LuaAddReputation(Lua_State* L);
	int LuaOpenReputation(Lua_State* L);

	// 计时器
	int LuaSetTimer(Lua_State* L);
	int LuaStopTimer(Lua_State* L);
	int LuaRestartTimer(Lua_State* L);
	int LuaRemoveTimer(Lua_State* L);
#endif

	int LuaGetReputation(Lua_State* L);
    int LuaGetReputeLevel(Lua_State* L);
	int LuaIsReputationHide(Lua_State* L);

#ifdef _CLIENT
	int LuaSetFightFlag(Lua_State* L);
	int LuaGetFightFlag(Lua_State* L);
	int LuaIsFightFlagLocked(Lua_State* L);

    int LuaTradingInviteRequest(Lua_State* L);
    int LuaTradingInviteRespond(Lua_State* L);
    int LuaTradingAddItem(Lua_State* L);
    int LuaTradingDeleteItem(Lua_State* L);
    int LuaTradingSetMoney(Lua_State* L);
    int LuaTradingConfirm(Lua_State* L);

    int LuaGetTradingItem(Lua_State* L);
    int LuaGetTradingMoney(Lua_State* L);
#endif

    int LuaTalk(Lua_State* L);
#ifdef _CLIENT
    int LuaGetTalkData(Lua_State* L);
    int LuaGetTalkLinkItem(Lua_State* L);
    int LuaDoAnimation(Lua_State* L);

	int LuaIsPlayerInMyParty(Lua_State* L);
	int LuaIsPartyLeader(Lua_State* L);
	int LuaIsPartyFull(Lua_State* L);

	int LuaSetMinimapRadar(Lua_State* L);
	int LuaGetMiniMapRadar(Lua_State* L);
#endif
    int LuaIsInParty(Lua_State* L);

#ifdef _SERVER
    int LuaSendSystemMessage(Lua_State* L);
    int LuaGmSendMessage(Lua_State* L);
#endif
    //-----------------------------------------


    //--- 好友 --------------------------------
    int LuaAddFellowship(Lua_State* L);
    int LuaDelFellowship(Lua_State* L);
    int LuaAddFellowshipGroup(Lua_State* L);
    int LuaDelFellowshipGroup(Lua_State* L);
    int LuaRenameFellowshipGroup(Lua_State* L);
    int LuaSetFellowshipRemark(Lua_State* L);
    int LuaCanAddFoe(Lua_State* L);
    int LuaAddFoe(Lua_State* L);
    int LuaDelFoe(Lua_State* L);
    int LuaAddBlackList(Lua_State* L);
    int LuaDelBlackList(Lua_State* L);
#ifdef _SERVER
    int LuaAddFellowshipAttraction(Lua_State* L);
    int LuaGetFellowshipAttraction(Lua_State* L);
    int LuaGetFellowshipAttractionLevel(Lua_State* L);
    int LuaIsFoe(Lua_State* L);
#endif

#ifdef _CLIENT
    int LuaSetFellowshipGroup(Lua_State* L);

    int LuaUpdateFellowshipInfo(Lua_State* L);
    int LuaUpdateFoeInfo(Lua_State* L);
    int LuaUpdateBlackListInfo(Lua_State* L);

    int LuaGetFellowshipGroupName(Lua_State* L);
    int LuaGetFellowshipData(Lua_State* L);
    int LuaGetFellowshipInfo(Lua_State* L);
    int LuaGetFoeInfo(Lua_State* L);
    int LuaGetBlackListInfo(Lua_State* L);
    int LuaGetFellowshipGroupInfo(Lua_State* L);
    int LuaGetFellowshipNameList(Lua_State* L);
    int LuaIsExistFellowshipList(Lua_State* L);
#endif
    //-----------------------------------------


#ifdef _SERVER
    int LuaGetPartyMemberList(Lua_State* L);

	int LuaMapTest(Lua_State* L);
	int LuaGetSelectCharacter(Lua_State* L);
    int LuaGetSelectDoodad(Lua_State* L);
	int LuaAddMapMark(Lua_State* L);	//小地图标记,问路功能用
	int LuaRevive(Lua_State* L);
	int LuaPlaySound(Lua_State* L);
	int LuaPlayMiniGame(Lua_State* L);
	int LuaSendMessageNotify(Lua_State* L);
#endif
	int LuaSyncMidMapMark(Lua_State* L);	//中地图标记,同步玩家自己在地图上做的标记

	//------------- PK ----------------------->
	int LuaCanApplyDuel(Lua_State* L);
	int LuaCanApplySlay(Lua_State* L);

	int LuaApplyDuel(Lua_State* L);
	int LuaRefuseDuel(Lua_State* L);
	int LuaAcceptDuel(Lua_State* L);
	int LuaLossDuel(Lua_State* L);
	
	int LuaApplySlay(Lua_State* L);
    int LuaCanCloseSlay(Lua_State* L);
	int LuaCloseSlay(Lua_State* L);

	int LuaGetPKState(Lua_State* L);
	//<----------------------------------------

#ifdef _SERVER
    int LuaRideHorse(Lua_State* L);
    int LuaDownHorse(Lua_State* L);

    int LuaAddEnchant(Lua_State* L);
    int LuaRemoveEnchant(Lua_State* L);
	int LuaGetEnchantDestItemSubType(Lua_State* L);

    int LuaSetEquipColor(Lua_State* L);

    int LuaMountItem(Lua_State* L);
    int LuaUnMountItem(Lua_State* L);
#endif
    int LuaCanMountItem(Lua_State* L);
    int LuaCanUnMountItem(Lua_State* L);
    
    //------------- 交通 ----------------------->
#ifdef _SERVER
    int LuaOpenRouteNode(Lua_State* L);
#endif
    int LuaIsOpenRouteNode(Lua_State* L);
	//<------------------------------------------

    // ------------- 修为相关 -------------------->
    int LuaAddTrain(Lua_State* L);
    int LuaAddTrainNoLimit(Lua_State* L);
    int LuaSetMaxTrain(Lua_State* L);
    // <-------------------------------------------

    // ----------------- 成就相关 ---------------->
    int LuaIsAchievementAcquired(Lua_State* L);
#ifdef _SERVER
    int LuaAcquireAchievement(Lua_State* L);
    int LuaAddAchievementCount(Lua_State* L);
#endif
    int LuaGetAchievementPoint(Lua_State* L);
    int LuaGetAchievementRecord(Lua_State* L);
    int LuaGetAchievementCount(Lua_State* L);
    // <-------------------------------------------

    int LuaSetCurrentDesignation(Lua_State* L);
    int LuaGetAcquiredDesignationPrefix(Lua_State* L);
    int LuaGetAcquiredDesignationPostfix(Lua_State* L);
    int LuaIsDesignationPrefixAcquired(Lua_State* L);
    int LuaIsDesignationPostfixAcquired(Lua_State* L);
#ifdef _SERVER
    int LuaAcquireDesignationPrefix(Lua_State* L);
    int LuaAcquireDesignationPostfix(Lua_State* L);
    int LuaRemoveDesignationPrefix(Lua_State* L);
    int LuaRemoveDesignationPostfix(Lua_State* L);
    int LuaSetGeneration(Lua_State* L);
	
#endif

	int LuaGetAcquiredDesignationCount(Lua_State* L);

    int LuaGetCurrentDesignationPrefix(Lua_State* L);
    int LuaGetCurrentDesignationPostfix(Lua_State* L);
    int LuaGetDesignationGeneration(Lua_State* L);
    int LuaGetDesignationByname(Lua_State* L);
    int LuaGetDesignationDisplayFlag(Lua_State* L);

#ifdef _CLIENT
    int LuaGetCurrentDesignation(Lua_State* L);
	//设置当前称号的字符串，使用character的sztitle，方便表现逻辑统一调用
	int LuaSetDesignationContent(Lua_State* L);
#endif

#ifdef _CLIENT
    // ------------- 经脉 ----------------------->
    int LuaOpenVenation(Lua_State* L);
    // <------------------------------------------
#endif

    int LuaSetTargetAStar(Lua_State* L);
    int LuaGetAroundPlayerID(Lua_State* L);

    int LuaIsOnSlay(Lua_State* L);

    int LuaSetMapVisitFlag(Lua_State* L);
    int LuaGetMapVisitFlag(Lua_State* L);

#ifdef _CLIENT
    int LuaIsPartyMemberInSameScene(Lua_State* L);
	int LuaOpen(Lua_State* L);
    int LuaGetQuestDiffcultyLevel(Lua_State* L);
#endif

#ifdef _SERVER
    int LuaKickout(Lua_State* L);
    int LuaBan(Lua_State* L);
    int LuaSetCurrentKillPoint(Lua_State* L);
    int LuaSetCannotDialogWithNPC(Lua_State* L);
    int LuaSetRedName(Lua_State* L);
    int LuaSetOnlyReviveInSitu(Lua_State* L);
    
    int LuaAddPrestige(Lua_State* L);

    int LuaAddPQ(Lua_State* L);
    int LuaDelPQ(Lua_State* L);
    int LuaGetPQIDTable(Lua_State* L);
    int LuaSetHeroMapCopyOpenFlag(Lua_State* L);
    int LuaGetHeroMapCopyOpenFlag(Lua_State* L);
#endif

    int LuaSetCamp(Lua_State* L);
    int LuaCanOpenCampFlag(Lua_State* L);
    int LuaCanCloseCampFlag(Lua_State* L);

#ifdef _SERVER
    int LuaOpenCampFlag(Lua_State* L);
    int LuaCloseCampFlag(Lua_State* L);
    int LuaGetReviveCtrl(Lua_State* L);
    int LuaSetReviveCtrl(Lua_State* L);
    int LuaSetMaxLevel(Lua_State* L);
#endif

#ifdef _CLIENT
    int LuaHideHat(Lua_State* L);
#endif

#ifdef _SERVER
    int LuaCreateViewPoint(Lua_State* L);
    int LuaRemoveViewPoint(Lua_State* L);
    int LuaIsViewPointExist(Lua_State* L);
    int LuaPreemptiveAttack(Lua_State* L);
	int LuaIsVenationSuccess(Lua_State* L);
#endif

// 扩展点操作
#ifdef _SERVER
    int LuaGetExtPoint(Lua_State* L);
    int LuaSetExtPoint(Lua_State* L);

// 激活礼品卡
    int LuaActivePresentCode(Lua_State* L);

    int LuaGetChargeFlag(Lua_State* L);

    int LuaIsGM(Lua_State* L);

// 师徒相关
    int LuaAddMaxApprenticeNum(Lua_State* L);
#endif

    int LuaIsMyMentor(Lua_State* L);
    int LuaIsMyApprentice(Lua_State* L);
    int LuaIsMentor(Lua_State* L);
    int LuaIsApprentice(Lua_State* L);
    
    int LuaCanLootBoxItem(Lua_State* L);
    int LuaOpenBox(Lua_State* L);
#ifdef _SERVER
    int LuaLootBoxItem(Lua_State* L);
    int LuaSetItemGenTime(Lua_State* L);
#endif
#ifdef _CLIENT
    int LuaGetBoxItem(Lua_State* L);
#endif
};
#endif	//_KPLAYER_H_
