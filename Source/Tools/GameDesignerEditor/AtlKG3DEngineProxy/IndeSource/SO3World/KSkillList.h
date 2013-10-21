/************************************************************************/
/* Player skill list                                                    */
/* Copyright : Kingsoft 2005										    */
/* Author	 : Zhu Jianqiu												*/
/* History	 :															*/
/*		2005.10.18	Create												*/
/*      2007.09.03  Modified for skill learning and skill realization.  */ 
/* Comment	 :															*/
/*      1   This list represents which skills a player have learned or  */
/*          realized.                                                   */
/*		2   This list contains skill information which are highly       */
/*          related to individual player (e.g. Skill experience).       */
/************************************************************************/
#ifndef _KSKILL_LIST_H_
#define _KSKILL_LIST_H_

#include <map>
class KSkill;
class KPlayer;

// warning C4200: nonstandard extension used : zero-sized array in struct/union
#pragma warning(disable: 4200)

#pragma pack(1)
struct KDB_SKILL_DATA
{
    DWORD       dwMountKungfuID;
    DWORD       dwMountKungfuLevel;
    int         nSkillCount;
    struct KDB_SKILL_ITEM
    {
        WORD	wSkillID;
        BYTE	byMaxLevel;
        DWORD	dwExp;
    } SkillItems[0];
};
#pragma pack()

#define MAX_PLAYER_SKILL_COUNT  1024

class KSkillList
{
public:
	KSkillList(void);
	~KSkillList(void);

    void    Init(KPlayer *pPlayer);
    void    UnInit();

#ifdef _SERVER
    BOOL    Load(BYTE* pbyData, size_t uDataLen);
    BOOL    Save(size_t* puUsedSize, BYTE* pbyBuffer, size_t uBufferSize);
#endif

    // 技能领悟和技能学习
    BOOL    CanRealizeSkill(DWORD dwSkillID);
    BOOL    CanLearnSkill(DWORD dwSkillID, DWORD dwMasterID);
#ifdef _SERVER
	BOOL	LearnSkill(DWORD dwSkillID, BOOL bNotifyPlayer, DWORD dwMasterID = 0);
	BOOL	ForgetSkill(DWORD dwSkillID);

	BOOL	AddSkillExp(DWORD dwSkillID, DWORD dwExp);

    void    OnPlayerLevelUp();    // 人物升级时，会回调这个函数
#endif

	// 更新技能，用于载入技能数据和客户端响应技能数据的同步
    BOOL    UpdateSkill(DWORD dwSkillID, DWORD dwSkillLevel, DWORD dwExp, BOOL bNotifyPlayer = true);

    BOOL    IsSkillExist(DWORD dwID, DWORD dwLevel);
	DWORD	GetSkillExp(DWORD dwSkillID);
	DWORD	GetSkillLevel(DWORD dwSkillID);

	int		_LuaGetSkillSchoolList(Lua_State* L);
	int		_LuaGetSkillKungfuList(Lua_State* L, DWORD dwSchollID);
	int		_LuaGetSkillList(Lua_State* L, DWORD dwKungfuID);
	int		_LuaGetAllSkillList(Lua_State* L);

    template <class TFunc>
    BOOL    Traverse(TFunc& Func);

private:
	struct KPlayerSkill	
	{
		DWORD	dwLevel;
		DWORD	dwExp;
	};

	typedef std::map<DWORD, KPlayerSkill>	PLAYER_SKILL_LIST;
	PLAYER_SKILL_LIST	m_PlayerSkillList;

    KPlayer* m_pPlayer;

public:
    DWORD m_dwMountKungfuID;
    DWORD m_dwMountKungfuLevel;
};

template <class TFunc>
BOOL KSkillList::Traverse(TFunc& Func)
{
    BOOL                        bResult     = false;
    BOOL                        bRetCode    = false;
    PLAYER_SKILL_LIST::iterator it          = m_PlayerSkillList.begin();
    PLAYER_SKILL_LIST::iterator itEnd       = m_PlayerSkillList.end();

    for (NULL; it != itEnd; ++it)
    {
        DWORD   dwSkillID       = it->first;
        DWORD   dwSkillLevel    = (it->second).dwLevel;
        bRetCode = Func(dwSkillID, dwSkillLevel);
        if (!bRetCode)
            break;
    }

Exit1:
    bResult = true;
Exit0:
    return bResult;
}

#endif	//_KSKILL_LIST_H_
