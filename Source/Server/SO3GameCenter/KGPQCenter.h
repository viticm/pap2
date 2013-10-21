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
#include <deque>
#include "Luna.h"

#define PQ_PARAM_COUNT 8

class KGPQTemplateInfo
{
public:
    DWORD   dwTemplateID;

    time_t  nBeginTime;
    time_t  nEndTime;
    time_t  nExpiration;

    BOOL    bGlobalAutoCreate;
    BOOL    bLoginAutoJoin;

    DWORD   dwMapAutoCreate;
    BOOL    bMapAutoJoin;

    DWORD   dwKillNpcTemplateID[PQ_PARAM_COUNT];
    int     nPQValue[PQ_PARAM_COUNT];

	char	szScriptFile[MAX_PATH];
};

enum KGPQ_OWNER_TYPE
{
    pqotInvalid = 0, 
    
    pqotScene, 
    pqotBattleField, 

    pqotTotal
};

class KGPQ
{
public:
    KGPQ();
    KGPQ(DWORD dwPQID, KGPQTemplateInfo* pPQTemplate);

    BOOL AddValue(int nIndex, int nAddValue);
    BOOL CanFinish();
    BOOL Finish();

    BOOL Save();

private:
    BOOL CallScriptFunc(const char cszFunction[]);

public:
    DWORD               m_dwPQID;
    KGPQTemplateInfo*   m_pPQTemplate;
    time_t              m_nBeginTime;
    time_t              m_nExpiration;
    time_t              m_nEndTime;
    int                 m_nPQValue[PQ_PARAM_COUNT];
    BOOL                m_bFinished;
    BOOL                m_bUpdate;
    BOOL                m_bDelete;

    KGPQ_OWNER_TYPE     m_eOwnerType;
    DWORD               m_dwOwnerID1;
    DWORD               m_dwOwnerID2;
    DWORD               m_dwOwnerID3;

public:
    DECLARE_LUA_CLASS(KGPQ);

    DECLARE_LUA_DWORD(PQID);
    DECLARE_LUA_BOOL(Finished);
    DWORD   getTemplateID(void) {return m_pPQTemplate->dwTemplateID;};
    int     getBeginTime(void)  {return (int)m_nBeginTime;};
    int     getExpiration(void) {return (int)m_nExpiration;};
    int     getEndTime(void)    {return (int)m_nEndTime;};

    DECLARE_LUA_ENUM(OwnerType);
    DECLARE_LUA_DWORD(OwnerID1);
    DECLARE_LUA_DWORD(OwnerID2);
    DECLARE_LUA_DWORD(OwnerID3);
    
    int LuaGetPQValue(Lua_State* L);
    int LuaAddPQValue(Lua_State* L);
    int LuaFinish(Lua_State* L);
    int LuaSave(Lua_State* L);
    int LuaAddEndTime(Lua_State* L);
};

struct KGPQ_SCHEDULE_NODE 
{
    time_t  nBeginTime;
    DWORD   dwPQTemplateID;
};

class KGPQCenter
{
public:
    BOOL    Init(); 
    BOOL    UnInit();

    BOOL    Activate();

    BOOL    OnGameServerCreate(int nConnIndex);
    BOOL    OnSceneCreate(DWORD dwMapID, int nMapCopyID);

    DWORD   CreatePQ(DWORD dwPQTemplateID, KGPQ_OWNER_TYPE eOwnerType = pqotInvalid, DWORD dwOwnerID1 = 0, DWORD dwOwnerID2 = 0, DWORD dwOwnerID3 = 0);
    BOOL    LoadPQ(const KGPQ& crPQ);
    BOOL    DeletePQ(DWORD dwPQID);
    KGPQ*   GetPQ(DWORD dwPQID);

    void    PendSave(DWORD dwPQID);
    void    SaveAll();

    KGPQTemplateInfo*   GetPQTemplate(DWORD dwPQTemplateID);

    template <class TFunc>
    BOOL    Traverse(TFunc& Func);

public:
    BOOL    m_bLoadCompleted;

private:
    BOOL    LoadPQTemplate(ITabFile* piTabFile, int nIndex, KGPQTemplateInfo* pPQTemplateInfo, const KGPQTemplateInfo& rDefaultPQTemplate);
    
private:
    typedef std::map<DWORD, KGPQ>                   PQ_MAP;
    typedef std::map<DWORD, KGPQTemplateInfo>       PQ_TEMPLATE_MAP;
    typedef std::vector<KGPQ_SCHEDULE_NODE>         PQ_SCHEDULE_VECTOR;
    typedef std::vector<std::pair<DWORD, DWORD> >   PQ_SCENE_AUTO_CREATE_VECTOR;
    typedef std::deque<DWORD>                       PQ_SAVE_QUEUE;
    typedef std::set<DWORD>                         PQ_SAVE_SET;

    PQ_MAP                                          m_PQMap;
    PQ_TEMPLATE_MAP                                 m_PQTemplateMap;
    PQ_SCHEDULE_VECTOR                              m_PQScheduleVector;
    PQ_SCENE_AUTO_CREATE_VECTOR                     m_PQSceneAutoCreateVector;
    PQ_SAVE_QUEUE                                   m_PQPendSaveQueue;
    PQ_SAVE_SET                                     m_PQPendSaveSet;
};

template <class TFunc>
BOOL KGPQCenter::Traverse(TFunc& Func)
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

#endif //_KGPQ_H_

