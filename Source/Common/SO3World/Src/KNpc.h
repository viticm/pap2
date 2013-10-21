/************************************************************************/
/* NPC			                                                        */
/* Copyright : Kingsoft 2005										    */
/* Author	 : Zhu Jianqiu												*/
/* History	 :															*/
/*		2004.12.30	Create												*/
/************************************************************************/
#ifndef _KNPC_H_
#define _KNPC_H_

#include "KShop.h"
#include "KCharacter.h"
#include "INpcFile.h"

enum  KNPC_SPECIES
{
    ensInvalid = 0, 
    ensHumanoid,    // 人型怪
    ensBeast,       // 野兽
    ensMechanical,  // 木甲
    ensUndead,      // 僵尸, 行尸走肉
    ensGhost,       // 妖灵
    ensPlant,       // 植物
    ensLegendary,   // 神兽，什么青龙、白虎、朱雀、玄武等等
    ensCritter,     // 小动物,家畜等
    ensOther,       // 未分类
    ensTotal
};

class KPlayer;
class KNpcTemplate;
class S2C_NPC_DISPLAY_DATA;

class KNpc : public KCharacter
{
public:
	BOOL Init();
	void UnInit();
	BOOL Activate(void);

public:
    int             m_nIntensity;       // 强度
	DWORD			m_dwScriptID;		// 绑定脚本ID		
	KShop*          m_pShop;            // 绑定的商店
	DWORD			m_dwTemplateID;		// 模板编号
    int             m_nReviveTime;      // 重生时间，单位秒
	int				m_nDisappearFrames;	// 消失帧数
	BOOL			m_bDisappearToReviveList; //消失后不删除,而是进重生队列
    KNPC_SPECIES    m_eSpecies;

    // 标记Npc身上是否有可以对话的脚本,如果这个标记为false的话,连任务Mark也不显示
    // 注意,这个标记为只是为客户端表现而设置的,并没有逻辑上的必然关系
	BOOL			m_bDialogFlag;

    // 标记Npc是否每帧都自动Active
    BOOL            m_bActiveMark;

	BOOL			m_bDisableAutoTurn;	// 被对话时自动转向
    MOVE_MODE       m_eMoveMode;

	DWORD			m_dwReliveID;					//重生群组的编号
    DWORD           m_dwRandomID;                   //重生群組的隨機編號，為0時不隨機
	int				m_nNpcTeamID;					//Npc小队ID
	int				m_nNpcTeamIndex;				//NPC小队中的编号，没有队伍的npc为-1
	int				m_nSkillCommomCD;						//Npc技能的公共CD,防止Npc一次释放多个技能
	int				m_nSkillCastFrame[MAX_NPC_AI_SKILL];	//NPC每个技能的最后释放时间

#if defined(_CLIENT)
	int				m_nUpdateMiniMapMarkFrame;		// 更新小地图标记的计时器
#endif 

#if defined(_SERVER)
	BOOL			m_bBank;						// 是否是银行
    BOOL            m_bMailbox;                     // 是否邮箱
    DWORD           m_dwSkillMasterID;              // 武功技能培训点ID
    DWORD           m_dwCraftMasterID;              // 生活技能培训点ID
    BOOL            m_bAuction;                     // 是否寄卖行
    BOOL            m_bTongRepertory;               // 是否帮会仓库

    int             m_nPathFindAstar;
    
    BOOL            m_bDropNotQuestItemFlag;        // 是否掉落非任务道具
    int             m_nAllAttackerLevel;            // 除掉落目标外，所有攻击我的角色的等级之和
#endif 
    DWORD           m_dwDropTargetPlayerID;         // 掉落目标

	KNpcTemplate*	m_pTemplate;					// 指向模板的指针
    DWORD           m_dwEmployer;                   // 这个NPC的雇主，也就是主人，一般情况下NPC是没有雇主的

#ifdef _SERVER
    // -- 新AI相关 ------------------------------------------------------->
    int             m_nSkillSelectIndex;            // 技能选择器的结果
    // <----------------------------------------------------------------------
#endif // _SERVER

#ifdef _SERVER
    int             m_nReviveGameLoop;
    int             m_nInFightGameLoop;

    int             m_nProgressID;                  // 该NPC所属的进度ID
#endif

public:
#ifdef _CLIENT
	void UpdateMiniMapMark();
#endif

	BOOL Load(const KNPC_DATA& crNpcData);

	virtual BOOL ChangeRegion(KRegion* pDestRegion);

#ifdef _SERVER
    void RegisterToViewRegion(KRegion* pFromRegion, KRegion* pToRegion);
#endif

	int GetNpcRelation(KNpc* pTarget);
	int GetPlayerRelation(KPlayer* pTarget);

	BOOL Say(char* pszText, int nRange = 0);		    // 说话(屏幕近聊)
	BOOL Yell(char* pszText);						    // 说话(场景喊话)
    BOOL Whisper(KPlayer* pTarget, char* pszText);	    // 说话(密语)
    BOOL SayTo(KPlayer* pTarget, char* pszText);	    // 和指定玩家说话
    BOOL YellTo(KPlayer* pTarget, char* pszText);	    // 向指定玩家大喊
    BOOL SayToCamp(KCAMP eCamp, char* pszText);        // NPC阵营频道

    BOOL SayToTargetUseSentenceID(KPlayer* pTarget, DWORD dwSentenceID);	// 和指定玩家说话

    BOOL SetRevivePoint(int nX, int nY, int nZ, int nDirection);            // 设置NPC的重生位置
    BOOL GetRevivePoint(KScene* pScene, int* pnRetX, int* pnRetY, int* pnRetZ, int* pnRetDirection);    // 获取NPC的重生位置

#ifdef _SERVER
    BOOL SetScript(const char* pszFileName);
    void LootExp(KPlayer* pDropTarget);
    void LootPrestige(KPlayer* pDropTarget);
    void LootContribution(KPlayer* pDropTarget);
    void LootAchievement(KPlayer* pDropTarget);
    void LootCampScore();
	BOOL AdjustLevel();
	BOOL SetDisappearFrames(int nFrames, BOOL bDisappearToReviveList = false);
	// 自己回到重生列表,相当于被打死
	BOOL GotoReviveList(); 
	// Npc生成自动对话的文本
	BOOL GetAutoDialogString(KPlayer* pPlayer, char* szBuffer, size_t uBufferSize);
    
    KPlayer*    GetDropTarget();
    KDoodad*    GenerateCorpse(void);
    void        ClearDropTarget();
    void        InitDialogFlag();
#endif //_SERVER

protected:

#if defined(_CLIENT)
	int m_nNextActionFrame;
#endif	//_CLIENT

public:

#ifdef _SERVER
    BOOL Revive();
    BOOL CallDeathScript(DWORD dwKiller);
#endif

public:
	DECLARE_LUA_CLASS(KNpc);

    DECLARE_LUA_INTEGER(Intensity);
	DECLARE_LUA_DWORD(ScriptID);
	DECLARE_LUA_DWORD(TemplateID);
    DECLARE_LUA_INTEGER(ReviveTime);
	DECLARE_LUA_BOOL(DialogFlag);
	DECLARE_LUA_BOOL(DisableAutoTurn);
    DECLARE_LUA_ENUM(Species);
    DECLARE_LUA_DWORD(DropTargetPlayerID);
    DECLARE_LUA_DWORD(Employer);

#ifdef _SERVER
	DECLARE_LUA_DWORD(ReliveID);
    DECLARE_LUA_DWORD(RandomID);
	DECLARE_LUA_INTEGER(NpcTeamID);
	DECLARE_LUA_INTEGER(NpcTeamIndex);
#endif //_SERVER

#ifdef _SERVER
	int LuaGetQuestString(Lua_State* L);	
	int LuaGetAutoDialogString(Lua_State* L);
    int LuaBindNpcShop(Lua_State* L);
    int LuaUnbindNpcShop(Lua_State* L);
    int LuaGetShop(Lua_State* L);
	int	LuaSetBank(Lua_State* L);

	int LuaFollowTarget(Lua_State* L);
	int LuaModifyThreat(Lua_State* L);		// 脚本修改仇恨
    int LuaCopyThreatList(Lua_State* L);
    int LuaSetActiveMark(Lua_State* L);
	int LuaSetPatrolPath(Lua_State* L);
    int LuaSetNpcTeamID(Lua_State* L);
	int LuaSetScript(Lua_State *L);

	int LuaSetDisappearFrames(Lua_State* L);
    int LuaGotoReviveList(Lua_State* L);
    int LuaGetTemplateReviveTime(Lua_State* L);

	int LuaSetDialogFlag(Lua_State* L);

	int LuaSay(Lua_State* L);
	int LuaYell(Lua_State* L);
    int LuaWhisper(Lua_State* L);
    int LuaSayTo(Lua_State* L);
    int LuaYellTo(Lua_State* L);
    int LuaSayToCamp(Lua_State* L);

    int LuaGetAISelectSkill(Lua_State* L);
    int LuaCopyAITarget(Lua_State* L);
#endif
    int LuaIsSelectable(Lua_State* L);
    int LuaCanSeeLifeBar(Lua_State* L);
    int LuaCanSeeName(Lua_State* L);
    int LuaGetNpcQuest(Lua_State* L);

#ifdef _SERVER
    int LuaSetBattleZone(Lua_State* L);
#endif
};
#endif	//_KNPC_H_
