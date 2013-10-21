#ifndef _KAI_PATROL_GROUP_MANAGER_H_
#define _KAI_PATROL_GROUP_MANAGER_H_

#include <vector>
#include <map>


class KScene;
class KCharacter;

struct KPatrolGroup 
{
    KPatrolGroup();

    std::vector<DWORD> MemberIDs;
    int nDestX;
    int nDestY;
    int nCurrentX;
    int nCurrentY;
    int nCurrentZ;
    int nDirection;
    int nPatrolPathIndex;
    int nRestUntilFrame;
};

class KAIPatrolGroupManager
{
public:
    KAIPatrolGroupManager() : m_pScene(NULL) {};

    BOOL    Init(KScene* pScene);
    BOOL    UnInit();

    BOOL    JoinGroup(DWORD dwPatrolPathID, DWORD dwCharacterID);
    BOOL    LeaveGroup(DWORD dwPatrolPathID, DWORD dwCharacterID);
    DWORD   GetLeader(DWORD dwPatrolPathID);
    BOOL    SetLeader(DWORD dwPatrolPathID, DWORD dwCharacterID);

    BOOL    UpdateGroupPosition(DWORD dwPatrolPathID, int nOrderIndex, int nDestX, int nDestY, int nCurrentX, int nCurrentY, int nCurrentZ, int nDirection);
    BOOL    GetMemberPosition(DWORD dwPatrolPathID, int nOrderIndex, int* pnX, int* pnY, int* pnDirection);

    int     SetGroupPatrolPathIndex(DWORD dwPatrolPathID, int nPatrolPathIndex);
    int     GetGroupPatrolPathIndex(DWORD dwPatrolPathID);
    int     GetNpcIndex(DWORD dwPatrolPathID, DWORD dwCharacterID);

    BOOL    SetRestFrame(DWORD dwPatrolPathID, int nRestFrame);
    int     GetRestFrame(DWORD dwPatrolPathID);

    KPatrolGroup* GetPatrolGroup(DWORD dwPatrolPathID);

private:
    KScene*                         m_pScene;
    std::map<DWORD, KPatrolGroup>   m_PatrolGroupMap;
};

#endif
