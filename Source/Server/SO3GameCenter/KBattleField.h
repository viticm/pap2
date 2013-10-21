////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KBattleField.h
//  Version     : 1.0
//  Creator     : Zhao Chunfeng
//  Create Date : 2007-04-10 11:21:31
//  Comment     : 战场控制器相关代码实现
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _KBATTLE_FIELD_H_
#define _KBATTLE_FIELD_H_
#include <list>
#include <map>

#define MAX_PLAYER_IN_BATTLE_FIELD 40

class KBattleFieldController
{
public:
    KBattleFieldController();

    BOOL Init();
    void UnInit();

    void Activate();

    BOOL OnCreateBattleField(DWORD dwMapID, int64_t nCopyIndex);
    BOOL OnDestroyBattleField(DWORD dwMapID, int64_t nCopyIndex);

    BOOL CheckPermission(DWORD dwPlayerID, DWORD dwMapID, int64_t nCopyIndex);

    BOOL OnPlayerLeave(DWORD dwPlayerID, DWORD dwMapID, int64_t nCopyIndex);
    BOOL OnPlayerEnter(DWORD dwPlayerID, DWORD dwMapID, int64_t nCopyIndex);
    BOOL OnPlayerAccpetJoin(DWORD dwPlayerID, DWORD dwMapID, int64_t nCopyIndex);

private:
    struct KSolder
    {
        DWORD   dwRoleID;
        time_t  nJoinTime;
        time_t  nLastOnlineTime;
        int     dwGroupID;
    };

    typedef std::list<KSolder> KPLAYER_LIST;
    struct KBattleField
    {
        DWORD           dwMapID;
        int64_t         nCopyIndex;
        BOOL            bActive;
        int             nMaxPlayerPerSide;
        KPLAYER_LIST    PlayingList;
        KPLAYER_LIST    WaitingList;
    };

    typedef std::map<int64_t, KBattleField> KBATTLE_FIELD_COPY_TABLE;
    typedef std::map<DWORD, KBATTLE_FIELD_COPY_TABLE> KBATTLE_FIELD_TABLE;
    KBATTLE_FIELD_TABLE m_BattleFieldTable;

    struct KBattleFieldStateInfo
    {
        DWORD dwMapID;
        int   nMaxPlayerPerSide;
        int   nMaxBattleFieldCopyCount;
    };
    typedef std::map<DWORD, KBattleFieldStateInfo> KSTATE_INFO_TABLE;
    KSTATE_INFO_TABLE m_BattleFieldStateInfo;

private:
    void ProcessBattleFieldGroup(KBATTLE_FIELD_COPY_TABLE* pCopyList);
    void ProcessBattleField(KBattleField* pBattleField);

    BOOL JoinInBattleField(DWORD dwPlayerID, KBattleField* pBattleField, DWORD dwGroupID);
};

#endif
