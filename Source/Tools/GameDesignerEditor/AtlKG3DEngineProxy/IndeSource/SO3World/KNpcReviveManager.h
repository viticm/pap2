////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KNpcReviveManager.h
//  Version     : 1.0
//  Creator     : Chen Jie
//  Comment     : Npc重生管理器
//
////////////////////////////////////////////////////////////////////////////////
#ifndef _KNPCREVIVEMANAGER_H_
#define _KNPCREVIVEMANAGER_H_

#include <deque>
#include "INpcFile.h"

#define MAX_REVIVE_NPC_PER_FRAME 16

class KScene;
class KNpc;
struct KNPC_DATA;

struct KNPC_REVIVE_NODE 
{
    KNpc*   pNpc;
    int     nFrame;
};

struct KNPC_RANDOM_DATA
{
    DWORD dwNpcTemplateID;
};

struct KNPC_REVIVE_QUEUE 
{
    KNPC_REVIVE_QUEUE()
    {
        nMinQueueSize   = 0;
        nMaxQueueSize   = INT_MAX;
        bRandom         = false;
    };

    std::deque<KNPC_REVIVE_NODE>    ReviveQueue;
    int                             nMinQueueSize;
    int                             nMaxQueueSize;
    BOOL                            bRandom;
};

struct KNPC_TABLE_DATA
{
    KNPC_TABLE_DATA()
    {
        nMinSize = 0;
        nMaxSize = INT_MAX;
        bRandom  = false;
    };

    int     nMinSize;
    int     nMaxSize;
    BOOL    bRandom;
};

struct KNPC_RANDOM_NODE
{
    KNpc*       pNpc;
    KNPC_DATA   NpcData;
};

class KNpcReviveManager 
{
public:
    KNpcReviveManager();
    ~KNpcReviveManager();

    BOOL Init(KScene* pScene);
    BOOL UnInit();

    BOOL Load(const char cszSceneName[]);

    void Activate();

    BOOL AddNpc(KNpc* pNpc, int nReviveTime);
    BOOL LoadNpc(KNpc* pNpc, KNPC_DATA& rNpcData);

    BOOL ForceGroupNpcRevive(int dwReviveID);

    BOOL CompleteLoading();

    // DEBUG
    BOOL GetReviveGroupState(int nGroupID, int* pnRetSize, int* pnRetMinSize, int* pnRetMaxSize, int* pnRetFristReviveTime);

    BOOL DestroyNpc(KNpc* pNpc);
    BOOL ForceReviveNpc(KNpc* pNpc);

private:
    BOOL ReviveNpc(DWORD dwReviveID);

    BOOL LoadNpcReviveTable(const char cszFileName[]);
    BOOL LoadNpcRandomTable(const char cszFileName[], const char cszSceneName[]);
    BOOL LoadNpcRandomTableSub(DWORD dwRandomID, const char cszFileName[]);

    BOOL CalculateReviveLimit();    
    BOOL DisorganizeReviveList();
    BOOL CreateRandomNpc();

private:
    typedef std::map<DWORD /* ReviveID */, KNPC_REVIVE_QUEUE>   KNpcReviveMap;
    typedef std::pair<DWORD /* ReviveID */, DWORD /* RandomID */> KNpcRandomKey;
    typedef std::vector<KNPC_RANDOM_NODE /* NPC */>  KNpcVector;
    typedef std::map<KNpcRandomKey, KNpcVector> KNpcRandomMap;
    typedef std::vector<KNPC_RANDOM_DATA> KNpcRandomDataVector;
    typedef std::map<DWORD, KNpcRandomDataVector> KNpcRandomDataMap;
    typedef std::map<DWORD /*ReviveID*/, KNPC_TABLE_DATA> KNpcTableMap;

    KScene*             m_pScene;
    KNpcReviveMap       m_NpcReviveMap;
    KNpcRandomMap       m_NpcRandomMap;
    KNpcRandomDataMap   m_NpcRandomDataMap;
    KNpcTableMap        m_NpcTableMap;
};

#endif
