////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KDoodadReviveManager.h
//  Version     : 1.0
//  Creator     : Wang tao
//  Comment     : Doodad重生管理器
//
////////////////////////////////////////////////////////////////////////////////
#ifndef _KDOODADREVIVEMANAGER_H_
#define _KDOODADREVIVEMANAGER_H_ 1

#include <deque>
#include "IDoodadFile.h"

#define MAX_REVIVE_DOODAD_PER_FRAME 16

class KScene;
class KDoodad;

struct KDOODAD_REVIVE_NODE
{
    KDoodad* pDoodad;
    int      nFrame;
};

struct KDOODAD_REVIVE_QUEUE
{
    KDOODAD_REVIVE_QUEUE()
    {
        bRandom = false;
        nMinQueueSize = 0;
        nMaxQueueSize = INT_MAX;
    }
    std::deque<KDOODAD_REVIVE_NODE> ReviveQueue;
    int                             nMinQueueSize;
    int                             nMaxQueueSize;
    BOOL                            bRandom;
};

struct KDOODAD_TABLE_DATA 
{
    KDOODAD_TABLE_DATA()
    {
        nMinSize = 0;
        nMaxSize = INT_MAX;
        bRandom  = 0;
    }

    int  nMinSize;
    int  nMaxSize;
    BOOL bRandom;
};

class KDoodadReviveManager
{
public:
    KDoodadReviveManager();
    ~KDoodadReviveManager();

    BOOL Init(KScene* pScene);
    BOOL UnInit();

    BOOL Load(const char cszFileName[]);

    void Activate();

    BOOL AddDoodad(KDoodad* pDoodad, int nReviveTime);

    BOOL CompleteLoading();     // Call this function once, when all Doodad have been loaded into ReviveQueue;

    BOOL DestroyDoodad(KDoodad* pDoodad);
    BOOL ForceReviveDoodad(KDoodad* pDoodad);

private:
    BOOL ReviveDoodad(DWORD dwReviveID);

    BOOL LoadDoodadReviveTable(const char cszFileName[]);

    BOOL CalculateReviveLimit();
    BOOL DisorganizeReviveList();

private:
    typedef std::map<DWORD /* ReviveID */, KDOODAD_REVIVE_QUEUE>    KDoodadReviveMap;
    typedef std::map<DWORD /* ReviveID */, KDOODAD_TABLE_DATA>      KDoodadTableMap;

    KScene*             m_pScene;
    KDoodadReviveMap    m_DoodadReviveMap;
    KDoodadTableMap     m_DoodadTableMap;
};

#endif // _KDOODADREVIVEMANAGER_H_
