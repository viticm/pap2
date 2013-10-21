#ifndef _KTEAM_SERVER_H_
#define _KTEAM_SERVER_H_ 1

#include <list>
#include <map>
#include "Engine/KMemory.h"
#include "SO3GlobalDef.h"

#ifdef _SERVER
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
    int             nMaxRage;
    int             nLifePercent;
    int             nManaPercent;
    int             nRagePercent;
    int             nPosX;
    int             nPosY;
	int		        nRoleType;
    BOOL            bDeathFlag;
};

typedef KMemory::KAllocator<KTEAM_MEMBER_INFO> KALLOC_TEAM_MEMBER;
typedef std::list<KTEAM_MEMBER_INFO, KALLOC_TEAM_MEMBER> KTEAM_MEMBER_LIST;

struct KTeamGroup
{
    DWORD               dwFormationLeader;
    KTEAM_MEMBER_LIST   MemberList;
};

struct KTeam
{
    BOOL        bSystem;
    DWORD       dwAuthority[tatTotal];
    KCAMP       eCamp;
    int         nLootMode;
    int         nRollQuality;
    int         nGroupNum;
    DWORD       dwMark[PARTY_MARK_COUNT];
    KTeamGroup  MemberGroup[MAX_TEAM_GROUP_NUM];
};

typedef KMemory::KAllocator<std::pair<DWORD, KTeam> > KALLOC_TEAM;
typedef std::map<DWORD, KTeam, less<DWORD>, KALLOC_TEAM> KTEAM_TABLE;

class KTeamServer
{
public:
    KTeamServer();

    BOOL Init();
    void UnInit();

    KTeam* GetTeam(DWORD dwTeamID);

    BOOL Create(DWORD dwTeamID);
    BOOL Destroy(DWORD dwTeamID);

    BOOL AddMember(DWORD dwTeamID, int nGroupIndex, DWORD dwPlayerID);
    BOOL DelMember(DWORD dwTeamID, int nGroupIndex, DWORD dwPlayerID);
    BOOL ChangeMember(
        DWORD dwTeamID, int nSrcGroupIndex, int nDstGroupIndex, DWORD dwSrcPlayerID, DWORD dwDstPlayerID
    );

    BOOL IsPlayerInTeam(DWORD dwTeamID, DWORD dwPlayerID);
    KTEAM_MEMBER_INFO* GetMember(DWORD dwTeamID, DWORD dwPlayerID);

    BOOL SetAuthority(DWORD dwTeamID, KTEAM_AUTHORITY_TYPE eType, DWORD dwTargetID);
    BOOL SetFormationLeader(DWORD dwTeamID, int nGroupIndex, DWORD dwTargetID);

    BOOL SetMark(DWORD dwTeamID, int nMarkType, DWORD dwTargetID);
    int GetMemberGroupIndex(DWORD dwTeamID, DWORD dwMemberID);

    template <class TFunc> BOOL Traverse(TFunc& Func);

private:
    KTEAM_TABLE m_TeamTable;
};

struct KGetAllTeamMember
{
    KGetAllTeamMember()
    {
        m_dwTeamID      = ERROR_ID;
        m_dwExceptID    = ERROR_ID;
        m_vecResult.clear();
    }

    DWORD m_dwTeamID;
    DWORD m_dwExceptID;
    std::vector<DWORD> m_vecResult;

    BOOL operator () (KTEAM_MEMBER_INFO* pMemberInfo);
};

template <class TFunc>
BOOL KTeamServer::Traverse(TFunc& Func)
{
    BOOL    bResult     = false;
    BOOL    bRetCode    = false;
    KTeam*  pTeam       = NULL;

    pTeam = GetTeam(Func.m_dwTeamID);
    KGLOG_PROCESS_ERROR(pTeam);

    for (int i = 0; i < pTeam->nGroupNum; i++)
    {
        for (
            KTEAM_MEMBER_LIST::iterator it = pTeam->MemberGroup[i].MemberList.begin();
            it != pTeam->MemberGroup[i].MemberList.end(); ++it
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
#endif // _SERVER
#endif // _KTEAM_SERVER_H_
