#include "stdafx.h"
#include "KGPQCenter.h"
#include "KSO3GameCenter.h"

#define PQ_FILE_NAME            "PQ.tab"
#define PQ_FINISH_FUNC          "OnPQFinish"
#define PQ_VALUE_CHANGE_FUNC    "OnPQValueChange"

KGPQ::KGPQ()
{
    new (this) KGPQ(0, NULL);
}

KGPQ::KGPQ(DWORD dwPQID, KGPQTemplateInfo* pPQTemplate)
{
    m_dwPQID            = dwPQID;
    m_pPQTemplate       = pPQTemplate;

    memset(m_nPQValue, 0, sizeof(m_nPQValue));

    m_nBeginTime        = g_pSO3GameCenter->m_nTimeNow;
    m_nEndTime          = 0;
    m_nExpiration       = 0;
    m_bFinished         = false;
    m_bUpdate           = false;
    m_bDelete           = false;
    m_eOwnerType        = pqotInvalid;
    m_dwOwnerID1        = 0;
    m_dwOwnerID2        = 0;
    m_dwOwnerID3        = 0;

    if (m_pPQTemplate)
    {
        m_nExpiration   = m_nBeginTime + m_pPQTemplate->nExpiration;

        if (m_pPQTemplate->nEndTime)
            m_nEndTime      = m_nBeginTime + m_pPQTemplate->nEndTime;
    }
}

BOOL KGPQ::AddValue(int nIndex, int nAddValue)
{
    BOOL bResult    = false;
    BOOL bRetCode   = false;

    KGLOG_PROCESS_ERROR(nIndex >= 0);
    KGLOG_PROCESS_ERROR(nIndex < PQ_PARAM_COUNT);

    KG_PROCESS_SUCCESS(m_bFinished);
    KG_PROCESS_SUCCESS(m_pPQTemplate->nPQValue[nIndex] && m_nPQValue[nIndex] >= m_pPQTemplate->nPQValue[nIndex]);

    m_nPQValue[nIndex] += nAddValue;

    if (m_pPQTemplate->nPQValue[nIndex])
    {
        m_nPQValue[nIndex] = min(m_nPQValue[nIndex], m_pPQTemplate->nPQValue[nIndex]);
        m_nPQValue[nIndex] = max(m_nPQValue[nIndex], 0);
    }

    g_pSO3GameCenter->m_GameServer.DoBroadcastPQValueChange(m_dwPQID, nIndex, m_nPQValue[nIndex]);

    CallScriptFunc(PQ_VALUE_CHANGE_FUNC);

    if (m_nPQValue[nIndex] >= m_pPQTemplate->nPQValue[nIndex])
    {
        bRetCode = CanFinish();
        if (bRetCode)
            Finish();
    }

    m_bUpdate = true;

Exit1:
    bResult = true;
Exit0:
    return bResult;
}

BOOL KGPQ::CanFinish()
{
    BOOL    bResult         = false;
    int     nCount          = sizeof(m_nPQValue) / sizeof(m_nPQValue[0]);
    int     nFinishCount    = 0;

    for (int i = 0; i < nCount; i++)
    {
        if (!m_pPQTemplate->nPQValue[i])
            continue;

        KG_PROCESS_ERROR(m_nPQValue[i] >= m_pPQTemplate->nPQValue[i]);
        nFinishCount++;
    }

    KG_PROCESS_ERROR(nFinishCount);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KGPQ::Finish()
{
    BOOL bResult    = false;
    BOOL bRetCode   = false;

    KG_PROCESS_ERROR(!m_bFinished);

    m_bFinished = true;

    g_pSO3GameCenter->m_GameServer.DoBroadcastPQFinish(m_dwPQID);

    CallScriptFunc(PQ_FINISH_FUNC);

    m_bUpdate = true;

    bResult = true;
Exit0:
    return bResult;
}

BOOL KGPQ::Save()
{
    BOOL bResult    = false;
    BOOL bRetCode   = false;
    KGPQ_DB_DATA    DBData;

    KG_PROCESS_SUCCESS(!m_bUpdate);

    if (m_bDelete)
    {
        g_pSO3GameCenter->m_MiscDB.DoDeletePQ(m_dwPQID);
        goto Exit1;
    }

    DBData.byVersion        = 1;
    DBData.dwPQID           = m_dwPQID;
    DBData.dwPQTemplateID   = m_pPQTemplate->dwTemplateID;
    DBData.nBeginTime       = m_nBeginTime;
    DBData.bFinished        = m_bFinished ? 1 : 0;
    DBData.byOnwerType      = (BYTE)m_eOwnerType;
    DBData.dwOnwerID1       = m_dwOwnerID1;
    DBData.dwOnwerID2       = m_dwOwnerID2;
    DBData.dwOnwerID3       = m_dwOwnerID3;

    assert(sizeof(DBData.nPQValue) == sizeof(m_nPQValue));
    memcpy(&DBData.nPQValue, &m_nPQValue, sizeof(DBData.nPQValue));

    g_pSO3GameCenter->m_MiscDB.DoSavePQ(DBData);

Exit1:
    m_bUpdate = false;
    bResult = true;
//Exit0:
    return bResult;
}

BOOL KGPQ::CallScriptFunc(const char cszFunction[])
{
    BOOL    bResult         = false;
    BOOL    bRetCode        = false;
    int     nLuaTopIndex    = 0;

    KG_PROCESS_ERROR(m_pPQTemplate->szScriptFile[0] != '\0');

    bRetCode = g_pSO3GameCenter->m_ScriptManager.IsFuncExist(m_pPQTemplate->szScriptFile, cszFunction);
    KG_PROCESS_ERROR(bRetCode);

    g_pSO3GameCenter->m_ScriptManager.SafeCallBegin(&nLuaTopIndex);

    g_pSO3GameCenter->m_ScriptManager.Push(m_dwPQID);
    g_pSO3GameCenter->m_ScriptManager.Call(m_pPQTemplate->szScriptFile, cszFunction, 1);
    g_pSO3GameCenter->m_ScriptManager.GetValuesFromStack("b", &bRetCode);

    g_pSO3GameCenter->m_ScriptManager.SafeCallEnd(nLuaTopIndex);
    KGLOG_CHECK_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

int KGPQ::LuaGetPQValue(Lua_State* L)
{
    int     nIndex      = 0;
    int     nTopIndex   = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    nIndex = (int)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(nIndex >= 0);
    KGLOG_PROCESS_ERROR(nIndex < PQ_PARAM_COUNT);

    Lua_PushNumber(L, m_nPQValue[nIndex]);
    return 1;
Exit0:
    return 0;
}

int KGPQ::LuaAddPQValue(Lua_State* L)
{
    BOOL    bResult     = false;
    int     nIndex      = 0;
    int     nAddValue   = 0;
    int     nTopIndex   = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 2);

    nIndex = (int)Lua_ValueToNumber(L, 1);
    nAddValue = (int)Lua_ValueToNumber(L, 2);

    KGLOG_PROCESS_ERROR(nIndex >= 0);
    KGLOG_PROCESS_ERROR(nIndex < PQ_PARAM_COUNT);

    AddValue(nIndex, nAddValue);

    bResult = true;
Exit0:
    Lua_PushBoolean(L, bResult);
    return 1;
}

int KGPQ::LuaFinish(Lua_State* L)
{
    BOOL    bResult     = false;

    Finish();

    bResult = true;
//Exit0:
    Lua_PushBoolean(L, bResult);
    return 1;    
}

int KGPQ::LuaSave(Lua_State* L)
{
    BOOL    bResult     = false;

    Save();

    bResult = true;
//Exit0:
    Lua_PushBoolean(L, bResult);
    return 1;    
}

int KGPQ::LuaAddEndTime(Lua_State* L)
{
    BOOL    bResult     = false;
    int     nTopIndex   = 0;
    int     nAddEndTime = 0;
    
    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    nAddEndTime = (int)Lua_ValueToNumber(L, 1);

    KGLOG_PROCESS_ERROR(m_nEndTime + nAddEndTime > g_pSO3GameCenter->m_nTimeNow);
    KGLOG_PROCESS_ERROR(m_nEndTime + nAddEndTime < m_nExpiration);

    m_nEndTime += nAddEndTime;

    bResult = true;
Exit0:
    Lua_PushBoolean(L, bResult);
    return 1;    
}

DEFINE_LUA_CLASS_BEGIN(KGPQ)
    REGISTER_LUA_DWORD_READONLY(KGPQ, PQID)
    REGISTER_LUA_BOOL_READONLY(KGPQ, Finished)
    REGISTER_LUA_DWORD_READONLY(KGPQ, TemplateID)
    REGISTER_LUA_INTEGER_READONLY(KGPQ, BeginTime)
    REGISTER_LUA_INTEGER_READONLY(KGPQ, Expiration)
    REGISTER_LUA_INTEGER_READONLY(KGPQ, EndTime)

    REGISTER_LUA_ENUM_READONLY(KGPQ, OwnerType)
    REGISTER_LUA_DWORD_READONLY(KGPQ, OwnerID1)
    REGISTER_LUA_DWORD_READONLY(KGPQ, OwnerID2)
    REGISTER_LUA_DWORD_READONLY(KGPQ, OwnerID3)

    REGISTER_LUA_FUNC(KGPQ, GetPQValue)
    REGISTER_LUA_FUNC(KGPQ, AddPQValue)
    REGISTER_LUA_FUNC(KGPQ, Finish)
    REGISTER_LUA_FUNC(KGPQ, Save)
    REGISTER_LUA_FUNC(KGPQ, AddEndTime)
DEFINE_LUA_CLASS_END(KGPQ)

BOOL _KGPGScheduleNodeSortComp(const KGPQ_SCHEDULE_NODE& crLeft, const KGPQ_SCHEDULE_NODE& crRight)
{
    if (crLeft.nBeginTime == crRight.nBeginTime)
        return crLeft.dwPQTemplateID > crRight.dwPQTemplateID;

    return crLeft.nBeginTime > crRight.nBeginTime;
}

BOOL KGPQCenter::Init()
{
    BOOL                bResult     = false;
    int                 nRetCode    = false;
    int                 nHeight     = 0;
    ITabFile*           piTabFile   = NULL;
    char                szFilePath[MAX_PATH];
    KGPQTemplateInfo    DefaultPQInfo;
    std::pair<PQ_TEMPLATE_MAP::iterator, bool> InsRet;

    m_PQTemplateMap.clear();
    m_PQMap.clear();
    m_PQScheduleVector.clear();
    m_PQSceneAutoCreateVector.clear();
    m_PQPendSaveQueue.clear();
    m_PQPendSaveSet.clear();

    nRetCode = (int)snprintf(szFilePath, sizeof(szFilePath), "%s/%s", SETTING_DIR, PQ_FILE_NAME);
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)sizeof(szFilePath));

    piTabFile = g_OpenTabFile(szFilePath);
    KGLOG_PROCESS_ERROR(piTabFile);

    nHeight = piTabFile->GetHeight();
    KGLOG_PROCESS_ERROR(nHeight > 1);

    memset(&DefaultPQInfo, 0, sizeof(DefaultPQInfo));

    nRetCode = LoadPQTemplate(piTabFile, 2, &DefaultPQInfo, DefaultPQInfo);
    KGLOG_PROCESS_ERROR(nRetCode);

    for (int nIndex = 3; nIndex <= nHeight; nIndex++)
    {
        KGPQTemplateInfo	PQTemplate;

        nRetCode = LoadPQTemplate(piTabFile, nIndex, &PQTemplate, DefaultPQInfo);
        KGLOG_PROCESS_ERROR(nRetCode);

        m_PQTemplateMap[PQTemplate.dwTemplateID] = PQTemplate;
    }

    sort(m_PQScheduleVector.begin(), m_PQScheduleVector.end(), _KGPGScheduleNodeSortComp);
    sort(m_PQSceneAutoCreateVector.begin(), m_PQSceneAutoCreateVector.end());

    m_bLoadCompleted = false;
    g_pSO3GameCenter->m_MiscDB.DoLoadPQList();

    bResult = true;
Exit0:
    if (!bResult)
    {
        m_PQTemplateMap.clear();
    }

    KG_COM_RELEASE(piTabFile);
    return bResult;
}

BOOL KGPQCenter::UnInit()
{
    BOOL    bResult     = false;
    int     nRetCode    = false;

    m_PQTemplateMap.clear();
    m_PQMap.clear();

    bResult = true;
//Exit0:
    return bResult;
}

BOOL KGPQCenter::Activate()
{
    BOOL    bResult     = false;
    int     nRetCode    = false;
    std::vector<DWORD>  DeleteVector;

    KG_PROCESS_SUCCESS(g_pSO3GameCenter->m_nWorkLoop % GAME_FPS == 0);

    for (PQ_MAP::iterator it = m_PQMap.begin(), itEnd = m_PQMap.end(); it != itEnd; NULL)
    {
        PQ_MAP::iterator itCurrent = it++;

        if (itCurrent->second.m_nExpiration < g_pSO3GameCenter->m_nTimeNow)
        {
            KGLogPrintf(
                KGLOG_WARNING, "[PQ] PQ(id = %lu, template = %lu) has passed its expiration date, force delete.", 
                itCurrent->second.m_dwPQID, itCurrent->second.m_pPQTemplate->dwTemplateID
            );
            DeleteVector.push_back(itCurrent->second.m_dwPQID);
        }

        if (itCurrent->second.m_bUpdate)
            PendSave(itCurrent->first);

        if (itCurrent->second.m_nEndTime && itCurrent->second.m_nEndTime < g_pSO3GameCenter->m_nTimeNow)
            itCurrent->second.Finish(); // Finish里面会调脚本，可能会删掉自己。在这之后的itCurrent可能失效
    }

    for (std::vector<DWORD>::iterator it = DeleteVector.begin(), itEnd = DeleteVector.end(); it != itEnd; ++it)
        DeletePQ(*it);

    if (!m_PQPendSaveQueue.empty())
    {
        DWORD   dwPQID  = m_PQPendSaveQueue.front();
        KGPQ*   pPQ     = NULL;

        pPQ = GetPQ(dwPQID);
        if (pPQ)
            pPQ->Save();

        m_PQPendSaveQueue.pop_front();
        m_PQPendSaveSet.erase(dwPQID);
    }

    KG_PROCESS_SUCCESS(!m_bLoadCompleted);

    while (!m_PQScheduleVector.empty())
    {
        const KGPQ_SCHEDULE_NODE* cpNode = &(m_PQScheduleVector.back());
        if (cpNode->nBeginTime > g_pSO3GameCenter->m_nTimeNow)
            break;

        CreatePQ(cpNode->dwPQTemplateID);
        m_PQScheduleVector.pop_back();
    }

Exit1:
    bResult = true;
//Exit0:
    return bResult;
}

BOOL KGPQCenter::OnGameServerCreate(int nConnIndex)
{
    for (PQ_MAP::iterator it = m_PQMap.begin(), itEnd = m_PQMap.end(); it != itEnd; ++it)
    {
        g_pSO3GameCenter->m_GameServer.DoSyncPQ(nConnIndex, it->second);
    }

    return true;
}

BOOL KGPQCenter::OnSceneCreate(DWORD dwMapID, int nMapCopyID)
{
    BOOL                                    bResult = false;
    PQ_SCENE_AUTO_CREATE_VECTOR::iterator   it;
    PQ_SCENE_AUTO_CREATE_VECTOR::iterator   itBegin;
    PQ_SCENE_AUTO_CREATE_VECTOR::iterator   itEnd;

    itBegin = std::lower_bound(m_PQSceneAutoCreateVector.begin(), m_PQSceneAutoCreateVector.end(), std::make_pair(dwMapID, (DWORD)0));
    KG_PROCESS_SUCCESS(itBegin == m_PQSceneAutoCreateVector.end());

    itEnd = std::lower_bound(m_PQSceneAutoCreateVector.begin(), m_PQSceneAutoCreateVector.end(), std::make_pair(dwMapID + 1, (DWORD)0));

    for (it = itBegin; it != itEnd; ++it)
    {
        DWORD dwPQTemplateID    = it->second;
        DWORD dwPQID            = 0;

        if (it->first != dwMapID)
            break;

        dwPQID = CreatePQ(dwPQTemplateID, pqotScene, dwMapID, nMapCopyID, 0);
        KGLOG_PROCESS_ERROR(dwPQID);
    }

Exit1:
    bResult = true;
Exit0:
    return bResult;
}

DWORD KGPQCenter::CreatePQ(DWORD dwPQTemplateID, KGPQ_OWNER_TYPE eOwnerType /* = pqotInvalid */, DWORD dwOwnerID1 /* = 0 */, DWORD dwOwnerID2 /* = 0 */, DWORD dwOwnerID3 /* = 0 */)
{
    DWORD                               dwResult    = 0;
    unsigned long long                  ullRetID    = 0;
    DWORD                               dwPQID      = 0;
    KGPQTemplateInfo*                   pPQTemplate = NULL;
    std::pair<PQ_MAP::iterator, bool>   RetPair;
    KGPQ*                               pPQ         = NULL;

    pPQTemplate = GetPQTemplate(dwPQTemplateID);
    KGLOG_PROCESS_ERROR(pPQTemplate);

    g_pSO3GameCenter->m_piSequence->GenerateID("PQID", 8, &ullRetID);
    KGLOG_PROCESS_ERROR(ullRetID);
    KGLOG_PROCESS_ERROR(ullRetID <= ULONG_MAX);
    dwPQID = (DWORD)ullRetID;

    RetPair = m_PQMap.insert(std::make_pair(dwPQID, KGPQ(dwPQID, pPQTemplate)));
    KGLOG_PROCESS_ERROR(RetPair.second);

    pPQ = &(RetPair.first->second);
    pPQ->m_bUpdate      = true;
    pPQ->m_eOwnerType   = eOwnerType;
    pPQ->m_dwOwnerID1   = dwOwnerID1;
    pPQ->m_dwOwnerID2   = dwOwnerID2;
    pPQ->m_dwOwnerID3   = dwOwnerID3;
    pPQ->m_nExpiration  = g_pSO3GameCenter->m_nTimeNow + pPQ->m_pPQTemplate->nExpiration;
    pPQ->m_nEndTime     = 0;
    pPQ->m_nExpiration  = 0;
    if (pPQ->m_pPQTemplate->nEndTime)
    {
        pPQ->m_nEndTime = g_pSO3GameCenter->m_nTimeNow + pPQ->m_pPQTemplate->nEndTime;
        if (pPQ->m_pPQTemplate->nBeginTime)
            pPQ->m_nEndTime -= (g_pSO3GameCenter->m_nTimeNow - pPQ->m_pPQTemplate->nBeginTime);
    }
    if (pPQ->m_pPQTemplate->nExpiration)
        pPQ->m_nExpiration = g_pSO3GameCenter->m_nTimeNow + pPQ->m_pPQTemplate->nExpiration;

    g_pSO3GameCenter->m_GameServer.DoBroadcastSyncPQ(*pPQ);

    dwResult = dwPQID;
Exit0:
    return dwResult;
}

struct _KGPGScheduleNodeFindTemplate
{
    BOOL operator() (const KGPQ_SCHEDULE_NODE& crPQ)
    {
        return crPQ.dwPQTemplateID == m_dwTemplateID;
    }

    DWORD   m_dwTemplateID;
};

BOOL KGPQCenter::LoadPQ(const KGPQ& crPQ)
{
    BOOL bResult = false;
    _KGPGScheduleNodeFindTemplate   FindTemplate;

    KGLOG_PROCESS_ERROR(crPQ.m_dwPQID);
    KGLOG_PROCESS_ERROR(crPQ.m_pPQTemplate);

    m_PQMap[crPQ.m_dwPQID] = crPQ;

    FindTemplate.m_dwTemplateID = crPQ.m_pPQTemplate->dwTemplateID;
    m_PQScheduleVector.erase(std::remove_if(m_PQScheduleVector.begin(), m_PQScheduleVector.end(), FindTemplate), m_PQScheduleVector.end());

    g_pSO3GameCenter->m_GameServer.DoBroadcastSyncPQ(crPQ);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KGPQCenter::DeletePQ(DWORD dwPQID) 
{
    BOOL                bResult = false;
    PQ_MAP::iterator    it;

    it = m_PQMap.find(dwPQID);
    KGLOG_PROCESS_ERROR(it != m_PQMap.end());

    it->second.m_bUpdate = true;
    it->second.m_bDelete = true;
    it->second.Save();
    KGLOG_CHECK_ERROR(!it->second.m_bUpdate);

    m_PQMap.erase(it);

    g_pSO3GameCenter->m_GameServer.DoBroadcastDeletePQ(dwPQID);

    bResult = true;
Exit0:
    return bResult;
}

KGPQ* KGPQCenter::GetPQ(DWORD dwPQID)
{
    KGPQ*               pResult = NULL;
    PQ_MAP::iterator    it;

    it = m_PQMap.find(dwPQID);
    KGLOG_PROCESS_ERROR(it != m_PQMap.end());

    pResult = &(it->second);
Exit0:
    return pResult;
}

void KGPQCenter::PendSave(DWORD dwPQID)
{
    PQ_SAVE_SET::iterator   itSet;
    
    itSet = m_PQPendSaveSet.find(dwPQID);
    KG_PROCESS_SUCCESS(itSet != m_PQPendSaveSet.end());

    m_PQPendSaveSet.insert(dwPQID);
    m_PQPendSaveQueue.push_back(dwPQID);    

Exit1:
    return;
}

void KGPQCenter::SaveAll()
{
    for (PQ_MAP::iterator it = m_PQMap.begin(), itEnd = m_PQMap.end(); it != itEnd; ++it)
    {
        it->second.Save();
    }
}

KGPQTemplateInfo* KGPQCenter::GetPQTemplate(DWORD dwPQTemplateID)
{
    KGPQTemplateInfo*           pResult = NULL;
    PQ_TEMPLATE_MAP::iterator   FindIt;

    FindIt = m_PQTemplateMap.find(dwPQTemplateID);
    KGLOG_PROCESS_ERROR(FindIt != m_PQTemplateMap.end());

    pResult = &(FindIt->second);
Exit0:
    return pResult;
}

BOOL KGPQCenter::LoadPQTemplate(ITabFile* piTabFile, int nIndex, KGPQTemplateInfo* pPQTemplateInfo, const KGPQTemplateInfo& rDefaultPQTemplate)
{
    BOOL bResult  = false;
    BOOL bRetCode = false;
    char szScript[MAX_PATH];

    bRetCode = piTabFile->GetInteger(
        nIndex, "PQTemplateID",
        rDefaultPQTemplate.dwTemplateID, (int*)&(pPQTemplateInfo->dwTemplateID)
    );
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = piTabFile->GetInteger(
        nIndex, "BeginTime",
        (int)rDefaultPQTemplate.nBeginTime, (int*)&(pPQTemplateInfo->nBeginTime)
    );
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = piTabFile->GetInteger(
        nIndex, "EndTime",
        (int)rDefaultPQTemplate.nEndTime, (int*)&(pPQTemplateInfo->nEndTime)
    );
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = piTabFile->GetInteger(
        nIndex, "Expiration",
        (int)rDefaultPQTemplate.nExpiration, (int*)&(pPQTemplateInfo->nExpiration)
    );
    KGLOG_PROCESS_ERROR(bRetCode);

    for (int i = 0; i < PQ_PARAM_COUNT; i++)
    {
        char szColName[_NAME_LEN];

        snprintf(szColName, _NAME_LEN, "KillNpcTemplateID_%d", i);
        szColName[_NAME_LEN - 1] = '\0';

        bRetCode = piTabFile->GetInteger(
            nIndex, szColName,
            rDefaultPQTemplate.dwKillNpcTemplateID[i], (int*)&(pPQTemplateInfo->dwKillNpcTemplateID[i])
        );
        KGLOG_PROCESS_ERROR(bRetCode);
    }

    for (int i = 0; i < PQ_PARAM_COUNT; i++)
    {
        char szColName[_NAME_LEN];

        snprintf(szColName, _NAME_LEN, "PQValue_%d", i);
        szColName[_NAME_LEN - 1] = '\0';

        bRetCode = piTabFile->GetInteger(
            nIndex, szColName,
            rDefaultPQTemplate.nPQValue[i], (int*)&(pPQTemplateInfo->nPQValue[i])
        );
        KGLOG_PROCESS_ERROR(bRetCode);
    }

    bRetCode = piTabFile->GetInteger(
        nIndex, "BindMapID",
        rDefaultPQTemplate.dwMapAutoCreate, (int*)&(pPQTemplateInfo->dwMapAutoCreate)
    );
    KGLOG_PROCESS_ERROR(bRetCode);

    if (pPQTemplateInfo->dwMapAutoCreate)
        m_PQSceneAutoCreateVector.push_back(std::make_pair(pPQTemplateInfo->dwMapAutoCreate, pPQTemplateInfo->dwTemplateID));

    pPQTemplateInfo->szScriptFile[0] = '\0';
    szScript[0] = '\0';
    bRetCode = piTabFile->GetString(
        nIndex, "Script",
        rDefaultPQTemplate.szScriptFile, szScript, sizeof(szScript)
    );
    KGLOG_PROCESS_ERROR(bRetCode);

    if (szScript[0] != '\0')
    {
        int nRetCode = snprintf(pPQTemplateInfo->szScriptFile, sizeof(pPQTemplateInfo->szScriptFile), "%s/%s", CENTER_SCRIPTS_DIR, szScript);
        KGLOG_PROCESS_ERROR(nRetCode > 0);
        KGLOG_PROCESS_ERROR(nRetCode < sizeof(pPQTemplateInfo->szScriptFile));
    }
    
    if (pPQTemplateInfo->nBeginTime)
    {
        KGPQ_SCHEDULE_NODE Node;
        Node.nBeginTime     = pPQTemplateInfo->nBeginTime;
        Node.dwPQTemplateID = pPQTemplateInfo->dwTemplateID;
        m_PQScheduleVector.push_back(Node);
    }

    bResult = true;
Exit0:
    return bResult;
}
