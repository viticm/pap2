////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KTeamClient.h
//  Version     : 1.0
//  Creator     : Wang Tao
//  Create Date : 2009-02-23 18:59:00
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#ifndef _KTEAM_CLIENT_H_
#define _KTEAM_CLIENT_H_ 1

#include <list>
#include <vector>

#ifdef _CLIENT
struct KTEAM_MEMBER_INFO 
{
	char            szRoleName[_NAME_LEN];
	DWORD           dwMemberID;
	KPORTRAIT_INFO  PortraitInfo;
	BOOL            bOnlineFlag;
	DWORD           dwMapID;
	int             nMapCopyIndex;
	DWORD           dwForceID;
	KCAMP           eCamp;
	int             nLevel;
	int             nMaxLife; 
	int             nMaxMana; 
	int             nLifePercent;
	int             nManaPercent;
	int             nPosX;
	int             nPosY;
	int		        nRoleType;
	BOOL            bDeathFlag;
	int             nFormationCoefficient;
};

typedef std::list<KTEAM_MEMBER_INFO> KTEAM_MEMBER_LIST;

struct KTEAM_GROUP
{
    DWORD               dwFormationLeader;
    KTEAM_MEMBER_LIST   MemberList;
};

class KTeamClient
{
public:
    KTeamClient();
    ~KTeamClient();

    BOOL Init();
    void UnInit();

public:
    BOOL Create();
    BOOL Destroy();

    BOOL AddMember(DWORD dwMemberID, int nGroupIndex, char szMemberName[]);
    BOOL DelMember(DWORD dwMemberID, int nGroupIndex);
    BOOL ChangeMember(DWORD dwMemberID, int nSrcGroupIndex, int nDstGroupIndex);

    BOOL IsPlayerInParty(DWORD dwPlayerID);
    KTEAM_MEMBER_INFO* GetMemberInfo(DWORD dwPlayerID);
    KTEAM_MEMBER_INFO* GetMemberInfoByName(const char cszMemberName[]);

    void Reset();

    BOOL SetFormationLeader(int nGroupIndex, DWORD dwFormationLeader);
    BOOL SetTeamMark(int nMarkType, DWORD dwTargetID);

    template <class TFunc> BOOL Traverse(TFunc& Func);

public:
    DWORD       m_dwTeamID;
    int         m_nGroupNum;
    int         m_nLootMode;
    int         m_nRollQuality;
    KCAMP       m_eCamp;
    DWORD       m_dwAuthority[tatTotal];
    DWORD       m_dwTeamMark[PARTY_MARK_COUNT];
    KTEAM_GROUP m_TeamGroup[MAX_TEAM_GROUP_NUM];

public:
    int LuaInviteJoinTeam(Lua_State* L);
    int LuaRespondTeamInvite(Lua_State* L);
    int LuaApplyJoinTeam(Lua_State* L);
    int LuaRespondTeamApply(Lua_State* L);
    int LuaRequestLeaveTeam(Lua_State* L);
    int LuaTeamKickoutMember(Lua_State* L);

    int LuaChangeTeamLeader(Lua_State* L);
    int LuaSetTeamLootMode(Lua_State* L);
    int LuaSetTeamRollQuality(Lua_State* L);
    int LuaTeamNotifySignpost(Lua_State* L);

    int LuaGetTeamSize(Lua_State* L);
    int LuaGetTeamMemberList(Lua_State* L);
    int LuaGetTeamOnlineMemberCount(Lua_State* L);

    int LuaCanSetTeamDistributeMan(Lua_State* L);
    int LuaSetTeamDistributeMan(Lua_State* L);

    int LuaCanSetTeamFormationLeader(Lua_State* L);
    int LuaSetTeamFormationLeader(Lua_State* L);

    int LuaIsPlayerInTeam(Lua_State* L);

    // -- MemberInfo --->> 考虑合成一个函数
    int LuaGetClientTeamMemberName(Lua_State* L);
    int LuaGetClientTeamMemberPortraitID(Lua_State* L);
    int LuaGetClientTeamMemberRoleType(Lua_State* L);
    int LuaGetClientTeamMemberOnlineFlag(Lua_State* L);
    int LuaGetClientTeamMemberMapID(Lua_State* L);
    int LuaGetClientTeamMemberLevel(Lua_State* L);
    int LuaGetClientTeamMemberMaxLife(Lua_State* L);
    int LuaGetClientTeamMemberMaxMana(Lua_State* L);
    int LuaGetClientTeamMemberCurrentLife(Lua_State* L);
    int LuaGetClientTeamMemberCurrentMana(Lua_State* L);
    int LuaGetClientTeamMemberCurrentPosition(Lua_State* L);
    int LuaGetClientTeamMemberDeathFlag(Lua_State* L);
    int LuaGetClientTeamMemberFormationCoefficient(Lua_State* L);
    int LuaGetClientTeamMemberForceID(Lua_State* L);
    // <<-----------------

    int LuaGetTeamMark(Lua_State* L);
    int LuaSetTeamMark(Lua_State* L);

    int LuaGetGroupInfo(Lua_State* L);
    int LuaGetMemberInfo(Lua_State* L);

    int LuaGetMemberGroupIndex(Lua_State* L);

    int LuaLevelUpRaid(Lua_State* L);
    int LuaChangeMemberGroup(Lua_State* L);

public:
    DECLARE_LUA_CLASS(KTeamClient);

    DECLARE_LUA_DWORD(TeamID);
    DECLARE_LUA_STRUCT_DWORD(TeamLeader, m_dwAuthority[tatLeader]);
    DECLARE_LUA_INTEGER(LootMode);
    DECLARE_LUA_INTEGER(RollQuality);
    DECLARE_LUA_ENUM(Camp);
    DECLARE_LUA_STRUCT_DWORD(DistributeMan, m_dwAuthority[tatDistribute]);
    DECLARE_LUA_STRUCT_DWORD(MarkMan, m_dwAuthority[tatMark]);
    DECLARE_LUA_STRUCT_DWORD(FormationLeader, m_TeamGroup[0].dwFormationLeader);
    DECLARE_LUA_STRUCT_INTEGER(GroupNum, m_nGroupNum);
};

struct KGetAllTeamMember
{
    DWORD m_dwExceptID;
    std::vector<KTEAM_MEMBER_INFO> m_vecResult;

    BOOL operator () (KTEAM_MEMBER_INFO* pMemberInfo);
};

template <class TFunc>
BOOL KTeamClient::Traverse(TFunc& Func)
{
    BOOL    bResult     = false;
    BOOL    bRetCode    = false;

    for (int i = 0; i < m_nGroupNum; i++)
    {
        for (
            KTEAM_MEMBER_LIST::iterator it = m_TeamGroup[i].MemberList.begin();
            it != m_TeamGroup[i].MemberList.end(); ++it
        )
        {
            if (it->dwMemberID == Func.m_dwExceptID)
                continue;

            bRetCode = Func(&(*it));
            KG_PROCESS_ERROR(bRetCode);
        }
    }

    bResult = true;
Exit0:
    return bResult;
}

#endif

#endif // _KTEAM_CLIENT_H_
