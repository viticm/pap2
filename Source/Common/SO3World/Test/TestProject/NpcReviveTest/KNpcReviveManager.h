////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KNpcReviveManager.h
//  Version     : 1.0
//  Creator     : Chen Jie
//  Comment     : Npc重生管理器
//
////////////////////////////////////////////////////////////////////////////////
#ifndef _KNPCREVIVEMANAGER_H_1
#define _KNPCREVIVEMANAGER_H_1

#include <deque>

class KScene;
class KNpc;
struct KNPC_DATA;

class KNpcReviveManager1 
{
public:
    KNpcReviveManager1();
    ~KNpcReviveManager1();

    BOOL Init(KScene* pScene);
    BOOL UnInit();

    BOOL AddNpc(KNpc* pNpc, int nReviveTime);
    BOOL LoadNpc(KNpc* pNpc);

    BOOL LoadNpcComplete();     // Call this function once, when all NPC have been loaded into ReviveQueue;

    BOOL CheckNpcRevive();

private:
    BOOL ReviveNpc(DWORD dwReviveID);

    BOOL LoadNpcReviveTable(const char* cpszFilePath);
    BOOL LoadNpcRandomTable(const char* cpszFilePath);
    BOOL LoadNpcRandomTableSub(DWORD dwRandomID, const char* cpszFilePath);

    BOOL CalculateReviveLimit();    
    BOOL DisorganizeReviveList();
    BOOL CreateRandomNpc();

    BOOL CheckChange(char szInfo[], int nCloseFlag = false);


private:
    typedef std::map<DWORD /* ReviveID */, KNPC_REVIVE_QUEUE>   KNpcReviveMap;
    typedef std::pair<DWORD /* ReviveID */, DWORD /* RandomID */> KNpcRandomKey;
    typedef std::vector<KNpc* /* NPC */>  KNpcVector;
    typedef std::map<KNpcRandomKey, KNpcVector> KNpcRandomMap;
    typedef std::vector<KNPC_RANDOM_DATA> KNpcRandomDataVector;
    typedef std::map<DWORD, KNpcRandomDataVector> KNpcRandomDataMap;

    KScene*             m_pScene;
    KNpcReviveMap       m_NpcReviveMap;
    KNpcRandomMap       m_NpcRandomMap;
    KNpcRandomDataMap   m_NpcRandomDataMap;
};

#endif
