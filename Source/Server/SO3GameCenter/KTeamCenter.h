#ifndef _KTEAM_CENTER_H_
#define _KTEAM_CENTER_H_ 1

#include <list>
#include <map>
#include "KRole.h"
#include "Engine/KMemory.h"
#include "SO3GlobalDef.h"

#define EMPTY_TEAM_TIME_OUT         (60 * 60) // 一个小时
#define INVITE_AND_APPLT_TIME_OUT   (60 * 10) // 十分钟

struct KTEAM_MEMBER_INFO
{
    DWORD dwMemberID;
    BOOL  bIsOnline;
};

typedef KMemory::KAllocator<KTEAM_MEMBER_INFO> KALLOC_TEAM_MEMBER;
typedef std::list<KTEAM_MEMBER_INFO, KALLOC_TEAM_MEMBER> KTEAM_MEMBER_LIST;

struct KGroup 
{
    DWORD             dwFormationLeader;
    KTEAM_MEMBER_LIST MemberList;
};

struct KTeam
{
    BOOL    bDelete;
    BOOL    bSystem;
    DWORD   dwAuthority[tatTotal];
    KCAMP   eCamp;
    int     nLootMode;
    int     nRollQuality;
    int     nGroupNum;
    time_t  nLastActiveTime;
    DWORD   dwTeamMark[PARTY_MARK_COUNT];
    KGroup  MemberGroup[MAX_TEAM_GROUP_NUM];
};

typedef KMemory::KAllocator<std::pair<DWORD, KTeam> > KALLOC_TEAM;
typedef std::map<DWORD, KTeam, less<DWORD>, KALLOC_TEAM> KTEAM_TABLE;

class KTeamCenter
{
public:
    KTeamCenter();

    BOOL Init();
    void UnInit();

    void Activate();

    DWORD CreateTeam(BOOL bSystem);
    BOOL DestroyTeam(DWORD dwTeamID);

    KTeam* GetTeam(DWORD dwTeamID);

    BOOL AddMember(DWORD dwTeamID, DWORD dwRoleID);
    BOOL DelMember(DWORD dwTeamID, DWORD dwRoleID);
    BOOL ChangeGroup(DWORD dwTeamID, int nDstGroupIndex, DWORD dwDstRoleID, DWORD dwRoleID);

    BOOL SetAuthority(DWORD dwTeamID, KTEAM_AUTHORITY_TYPE eType, DWORD dwTargetID);
    BOOL SetFormationLeader(DWORD dwTeamID, int nGroupIndex, DWORD dwRequestSrcID, DWORD dwTargetID);

    BOOL SetMark(DWORD dwTeamID, int nMarkType, DWORD dwTargetID);

    void SyncNewServer(int nConnIndex);

    int GetTeamMemberCount(DWORD dwTeamID);
    BOOL IsRoleInTeam(DWORD dwTeamID, DWORD dwRoleID);

    BOOL RegisterInvite(DWORD dwInviteSrc, DWORD dwInviteDst);
    BOOL UnregisterInvite(DWORD dwInviteSrc, DWORD dwInviteDst);
    BOOL ClearInvite(DWORD dwInviteSrc);
    BOOL IsInviteExist(DWORD dwInviteSrc, DWORD dwInviteDst);

    BOOL RegisterApply(DWORD dwApplySrc, DWORD dwApplyDst);
    BOOL UnregisterApply(DWORD dwApplySrc, DWORD dwApplyDst);
    BOOL ClearApply(DWORD dwApplySrc);
    BOOL IsApplyExist(DWORD dwApplySrc, DWORD dwApplyDst);

    int GetTeamCount();

private:
    KTEAM_TABLE m_TeamTable;

    DWORD       m_dwTeamSNGenerator;

    typedef KMemory::KAllocator<std::pair<int64_t, time_t> > KALLOC_INVITE;
    typedef std::map<int64_t, time_t, less<int64_t>, KALLOC_INVITE> KINVITE_TABLE;
    KINVITE_TABLE m_InviteTable;

    typedef KMemory::KAllocator<std::pair<int64_t, time_t> >KALLOC_APPLY;
    typedef std::map<int64_t, time_t, less<int64_t>, KALLOC_APPLY> KAPPLY_TABLE;
    KAPPLY_TABLE m_ApplyTable;

private:
    void ProcessTeam(DWORD dwTeamID, KTeam* pTeam);
    void ProcessAuthority(DWORD dwTeamID, KTeam* pTeam);
    void ProcessFormation(DWORD dwTeamID, KTeam* pTeam);
    void ProcessMember(DWORD dwTeamID, KTeam* pTeam);
    void ProcessInviteAndApplyTimeOut();

    void OnChangeLeader(DWORD dwLeaderID);

    void SetRoleTeamID(KRole* pRole, BOOL bSystem, DWORD dwTeamID);
};

#endif
