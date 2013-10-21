/************************************************************************/
/* NPC			                                                        */
/* Copyright : Kingsoft 2005										    */
/* Author	 : Zhu Jianqiu												*/
/* History	 :															*/
/*		2004.12.30	Create												*/
/************************************************************************/
#ifndef _KNPC_H_
#define _KNPC_H_

//#include "KShop.h"
#include "KCharacter.h"

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


class KNpcTemplate;

class KNpc : public KCharacter
{
public:
    int             m_nIntensity;       // 强度
	DWORD			m_dwScriptID;		// 绑定脚本ID		
//	KShop*          m_pShop;            // 绑定的商店
	DWORD			m_dwTemplateID;		// 模板编号
    int             m_nAlarmRange;      // 攻击范围
    int             m_nReviveTime;      // 重生时间，单位秒
	int				m_nDisappearFrames;	//消失帧数
    KNPC_SPECIES    m_eSpecies;
	DWORD			m_dwRepresentID;

public:
	KNpc(void);
	~KNpc(void);

	BOOL Init();
	BOOL UnInit();
	BOOL Activate(void);

#ifdef _CLIENT
    void SetDisplayData(DWORD dwTemplateID, int nLevel, DWORD dwRepresentID);
#endif

	BOOL Save(KNPC_DATA* pNpcData);
	BOOL Load(const KNPC_DATA& crNpcData);

	virtual BOOL ChangeRegion(KRegion* pRegion);
	virtual BOOL ChangeCell(KCell* pCell);

	int GetNpcRelation(KNpc* pTarget);
	int GetPlayerRelation(KPlayer* pTarget);


protected:
	//处理本循环的指令，对于玩家来说是指令列表，对于NPC来说是调用NPCAI
	virtual BOOL GetCommand(void);

#if defined(_CLIENT)
	int m_nNextActionFrame;
#endif	//_CLIENT

public:
    BOOL CallDeathScript(KCharacter* pKiller);

public:
	DECLARE_LUA_CLASS(KNpc);

    DECLARE_LUA_INTEGER(Intensity);
	DECLARE_LUA_DWORD(ScriptID);
	DECLARE_LUA_DWORD(TemplateID);
    DECLARE_LUA_INTEGER(AlarmRange);
    DECLARE_LUA_INTEGER(ReviveTime);

	int LuaGetQuestString(Lua_State* L);
	int LuaShareQuest(Lua_State* L);

};
#endif	//_KNPC_H_