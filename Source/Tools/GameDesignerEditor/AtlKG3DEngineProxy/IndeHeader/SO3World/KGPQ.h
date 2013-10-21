////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGPQ.h
//  Version     : 1.0
//  Creator     : Chen Jie
//  Comment     : 公共任务模块
//
////////////////////////////////////////////////////////////////////////////////
#ifndef _KGPQ_H_
#define _KGPQ_H_

#include <map>
#include "Luna.h"
#include "SO3GlobalDef.h"

#define PQ_PARAM_COUNT 8

class KGPQTemplateInfo
{
public:
    DWORD   dwTemplateID;

    time_t  nBeginTime;
    time_t  nEndTime;

    BOOL    bGlobalAutoCreate;
    BOOL    bLoginAutoJoin;

    DWORD   dwMapAutoCreate;
    BOOL    bMapAutoJoin;

    DWORD   dwKillNpcTemplateID[PQ_PARAM_COUNT];
    int     nPQValue[PQ_PARAM_COUNT];

    BOOL    bStatistics;

    char	szScriptFile[MAX_PATH];
};

enum KGPQ_OWNER_TYPE
{
    pqotInvalid = 0, 

    pqotScene, 
    pqotBattleField, 

    pqotTotal
};

struct  KPQ_STATISTICS_DATA
{
    KPQ_STATISTICS_DATA() { memset(this, 0, sizeof(KPQ_STATISTICS_DATA)); };
    int nValueArray[psiTotal];

#ifdef _CLIENT
    DWORD   dwPlayerForceID;
    int     nBattleFieldSide;
    char    szPlayerName[_NAME_LEN];
#endif
};

class KGPQ
{
public:
    KGPQ();
    KGPQ(DWORD dwPQID, KGPQTemplateInfo* pPQTemplate);

    BOOL SetValue(int nIndex, int nValue);
    BOOL Finish();

private:
    BOOL CallScriptFunc(const char cszFunction[]);

public:
    DWORD               m_dwPQID;
    KGPQTemplateInfo*   m_pPQTemplate;
    time_t              m_nBeginTime;
    int                 m_nPQValue[PQ_PARAM_COUNT];
    BOOL                m_bFinished;

    KGPQ_OWNER_TYPE     m_eOwnerType;
    DWORD               m_dwOwnerID1;
    DWORD               m_dwOwnerID2;
    DWORD               m_dwOwnerID3;

    typedef std::map<DWORD, KPQ_STATISTICS_DATA> KPQ_STATISTICS_MAP;
    KPQ_STATISTICS_MAP  m_StatisticsMap;

public:
    DECLARE_LUA_CLASS(KGPQ);

    DECLARE_LUA_DWORD(PQID);
    DWORD getTemplateID(void) {return m_pPQTemplate->dwTemplateID;};
    DECLARE_LUA_BOOL(Finished);

    DECLARE_LUA_ENUM(OwnerType);
    DECLARE_LUA_DWORD(OwnerID1);
    DECLARE_LUA_DWORD(OwnerID2);
    DECLARE_LUA_DWORD(OwnerID3);

    int LuaGetPQValue(Lua_State* L);

    int LuaSetStatistics(Lua_State* L);
    int LuaGetStatistics(Lua_State* L);
    int LuaGetStatisticsTable(Lua_State* L);

    int LuaGetScene(Lua_State* L);
};

#pragma pack(1)
struct KGPQ_DB_LIST 
{
    WORD    wCount;
    DWORD   dwPQID[0];
};
#pragma pack()

class KGPQList
{
public:
    BOOL AddPQ(DWORD dwPQID);
    BOOL DelPQ(DWORD dwPQID);

#ifdef _SERVER
    BOOL Load(BYTE* pbyData, size_t uDataLen);
    BOOL Save(size_t* puUsedSize, BYTE* pbyBuffer, size_t uBufferSize);

    BOOL OnKillNpc(DWORD dwNpcTemplateID);
#endif

    template <class TFunc>
    BOOL Traverse(TFunc& Func);

public:
    typedef std::vector<DWORD>                              PQ_VECTOR;
    typedef std::multimap<DWORD, std::pair<DWORD, int> >    NPC_2_PQ_MAP;

    PQ_VECTOR       m_PQVector;
    NPC_2_PQ_MAP    m_Npc2PQMap;
};

template <class TFunc>
BOOL KGPQList::Traverse(TFunc& Func)
{
    BOOL bResult  = false;
    BOOL bRetCode = false;

    for (PQ_VECTOR::iterator it = m_PQVector.begin(), itEnd = m_PQVector.end(); it != itEnd; ++it)
    {
        bRetCode = Func(*it);
        if (!bRetCode)
            break;
    }

    bResult = true;
//Exit0:
    return bResult;
}

class KGPQManager
{
public:
    BOOL    Init();
    BOOL    UnInit();
    
    BOOL    LoadPQ(const KGPQ& crPQ);
    BOOL    DeletePQ(DWORD dwPQID);
    KGPQ*   GetPQ(DWORD dwPQID);

    KGPQTemplateInfo*   GetPQTemplate(DWORD dwPQTemplateID);

    template <class TFunc>
    BOOL    Traverse(TFunc& Func);

private:
    BOOL    LoadPQTemplate(ITabFile* piTabFile, int nIndex, KGPQTemplateInfo* pPQTemplateInfo, const KGPQTemplateInfo& rDefaultPQTemplate);
    
private:
    typedef std::map<DWORD, KGPQTemplateInfo>   PQ_TEMPLATE_MAP;
    typedef std::map<DWORD, KGPQ>               PQ_MAP;
    PQ_TEMPLATE_MAP                             m_PQTemplateMap;
    PQ_MAP                                      m_PQMap;
};

template <class TFunc>
BOOL KGPQManager::Traverse(TFunc& Func)
{
    BOOL bResult  = false;
    BOOL bRetCode = false;

    for (PQ_MAP::iterator it = m_PQMap.begin(), itEnd = m_PQMap.end(); it != itEnd; ++it)
    {
        bRetCode = Func(it->second);
        if (!bRetCode)
            break;
    }

    bResult = true;
    //Exit0:
    return bResult;
}


struct KGPQ_STATISTICS_INCREMENT 
{
    KGPQ_STATISTICS_INCREMENT(KGPQManager* pPQManager, DWORD dwPlayerID, KPQ_STATISTICS_INDEX eIndex, int nAdd = 1)
    {
        assert(pPQManager);
        assert(dwPlayerID);
        assert(eIndex < psiTotal);

        m_pPQManager    = pPQManager;
        m_dwPlayerID    = dwPlayerID;
        m_eIndex        = eIndex;
        m_nAdd          = nAdd;
    }

    BOOL operator () (DWORD dwPQID)
    {
        KGPQ* pPQ = NULL;

        if (!m_pPQManager)
            return false;

        if (!m_dwPlayerID)
            return false;

        pPQ = m_pPQManager->GetPQ(dwPQID);
        if (!pPQ)
            return true;

        if (!pPQ->m_pPQTemplate->bStatistics)
            return true;

        if (pPQ->m_bFinished)
            return true;

        pPQ->m_StatisticsMap[m_dwPlayerID].nValueArray[m_eIndex] += m_nAdd;

        return true;
    };

    KGPQManager*            m_pPQManager;
    DWORD                   m_dwPlayerID;
    KPQ_STATISTICS_INDEX    m_eIndex;
    int                     m_nAdd;
};
#endif //_KGPQ_H_
