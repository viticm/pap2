/************************************************************************/
/* 场景装饰物	                                                        */
/* Copyright : Kingsoft 2005										    */
/* Author	 : Zhu Jianqiu												*/
/* History	 :															*/
/*		2004.12.31	Create												*/
/* Comment	 :															*/
/*		轻量级的场景物体对象											*/
/************************************************************************/
#ifndef _KDOODAD_H_
#define	_KDOODAD_H_

#include "Luna.h"
#include "KSceneObject.h"
#include "IDoodadFile.h"

#define MAX_ROLL_FRAME 90 * GAME_FPS

enum DOODAD_STATE
{
	dsInvalid = 0,				

	// 受控状态
	dsIdle,						// 正常状态
	dsBeHit,					// 被操作或被击
	dsAfterHit,					// 打开后或被击死亡

	dsTotal
};

class KCharacter;
class KLootList;
class KDoodadTemplate;

class KDoodad : public KSceneObject
{
public:
	BOOL        Init();
	void        UnInit();
	BOOL        Activate(void);
	BOOL        Load(const KDOODAD_DATA& crDoodadData);
    void        ChangeState(DOODAD_STATE eState);
    BOOL        CheckOpen(KPlayer* pOpener);
    BOOL        HaveQuest(KPlayer* pPlayer);
    int	        GetOpenFrame(KPlayer* pOpener);
    DWORD       GetRecipeID();
    void        OnBreakPickPrepare();
    void        CallBreakScriptFunction(KPlayer* pOpener);

private:
    void        DoIdle();
    void        DoBeHit();
    void        DoAfterHit();

#ifdef _CLIENT
public:
    void        OpenLootList(KCharacter* pCharacter);
    void        UpdateMiniMapMark();
#endif

#ifdef _SERVER
public:
    void        LootOver();
    BOOL        SetDelayRemove();
    void        SetDisappearFrames(int nDisappearFrames, BOOL bDisappearToReviveList = true);
    int         GetDisappearFrames();
    void        Revive();
    BOOL        PerOpen(KPlayer* pPlayer);
    BOOL        SetScript(const char* pcszFileName);		    // 修改挂的脚本的路径
    KLootList*  GenerateLootList(KPlayer* pCharacter, BOOL bIsAbsoluteFree);          // 生成掉落表
    KLootList*  GenerateLootListFromNpc(KPlayer* pDropTarget, KNpc* pNpc);
    KLootList*  GenerateEmptyLootList();                        // 生成空的掉落表
    void        Close();
    
private:
    void        Remove(BOOL bGotoReviveList = true);
#endif

public:
	DWORD					m_dwTemplateID;					// 模板编号
	char					m_szName[_NAME_LEN];			// 名称
	DOODAD_KIND				m_eKind;						// 类型
	int						m_nOpenFrames;					// 打开帧数
	int						m_nDisappearFrames;				// 消失帧数
	BOOL					m_bDisappearToReviveList;       // 消失后不删除,而是进重生队列
	DOODAD_STATE			m_eState;						// 状态
	DWORD					m_dwNpcTemplateID;
    DWORD                   m_dwNpcID;                      // 尸体对应的NPC
	DWORD					m_dwRepresentID;				// Doodad的表现ID
	BOOL					m_bLooted;						// 尸体是否被拾取完的标记
    int                     m_nObstacleGroup;               // 控制动态障碍的分组

	DWORD					m_dwReviveID;					// 重生群组的编号
	int						m_nCurrentReliveIndex;			// 当前复活点的编号
    KDoodadTemplate*		m_pTemplate;					// 指向模板的指针
    DWORD					m_dwScriptID;					// 绑定脚本ID
    KLootList*				m_pLootList;					// 拾取列表

#ifdef _SERVER
	int						m_nLastLootCount;				// 剩余的拾取次数
	DWORD					m_dwOpenPlayerID;               // 当前操作doodad的玩家
	DWORD					m_dwOwnerID;					// 脚本创建这个Doodad的PlayerID

    int                     m_nProgressID;                  // 该Doodad的进度ID
#endif

#ifdef _CLIENT
	int						m_nUpdateMiniMapMarkFrame;		// 更新小地图标记的计时器
#endif 
   
public:
    DECLARE_LUA_CLASS(KDoodad);
    DECLARE_LUA_DWORD(TemplateID);
    DECLARE_LUA_STRING(Name, _NAME_LEN);
    DECLARE_LUA_ENUM(Kind);
    DECLARE_LUA_INTEGER(OpenFrames);
    DECLARE_LUA_INTEGER(DisappearFrames);
    DECLARE_LUA_DWORD(RepresentID);
    DECLARE_LUA_DWORD(NpcTemplateID);
    DECLARE_LUA_DWORD(ScriptID);

#ifdef _SERVER
    DECLARE_LUA_DWORD(OwnerID);
#endif

public:
	int LuaCanDialog(Lua_State* L);
	int LuaIsSelectable(Lua_State* L);
    int LuaCanSearch(Lua_State* L);
	int LuaHaveQuest(Lua_State* L);
    int LuaGetRecipeID(Lua_State* L);

#ifdef _CLIENT
    int LuaGetLootItem(Lua_State* L);
	int LuaGetLootMoney(Lua_State* L);
    int LuaCanLoot(Lua_State* L);
    int LuaDistributeItem(Lua_State* L);
	int LuaGetLooterList(Lua_State* L);
    int LuaCanSit(Lua_State* L);
    int LuaGetRollFrame(Lua_State* L);
#endif

#ifdef _SERVER
	int LuaSetScript(Lua_State* L);
	int LuaGenerateEmptyLootList(Lua_State* L);
	int LuaAddLootItem(Lua_State* L);
	int LuaSetDisappearFrames(Lua_State* L);
    int LuaIsDoorClose(Lua_State* L);
    int LuaOpenDoor(Lua_State* L);
    int LuaCloseDoor(Lua_State* L);

    int LuaSetCustomInteger4(Lua_State* L);
    int LuaGetCustomInteger4(Lua_State* L);
    int LuaSetCustomInteger2(Lua_State* L);
    int LuaGetCustomInteger2(Lua_State* L);
    int LuaSetCustomInteger1(Lua_State* L);
    int LuaGetCustomInteger1(Lua_State* L);

    int LuaSetCustomUnsigned4(Lua_State* L);
    int LuaGetCustomUnsigned4(Lua_State* L);
    int LuaSetCustomUnsigned2(Lua_State* L);
    int LuaGetCustomUnsigned2(Lua_State* L);
    int LuaSetCustomUnsigned1(Lua_State* L);
    int LuaGetCustomUnsigned1(Lua_State* L);

    int LuaSetCustomBoolean(Lua_State* L);
    int LuaGetCustomBoolean(Lua_State* L);

    // 偏移量, 第几个Bit(从左到右, 从0开始计数), 数值为true/false
    int LuaSetCustomBitValue(Lua_State* L);
    int LuaGetCustomBitValue(Lua_State* L);

    int LuaSetCustomString(Lua_State* L);
    int LuaGetCustomString(Lua_State* L);

    int LuaGenerateLootListByNpcMode(Lua_State* L);
#endif	//_SERVER
};
#endif
