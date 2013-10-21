#include "stdafx.h"
#include "KGPQ.h"
#include "KScene.h"
#include "KSO3World.h"
#include "KRelayClient.h"

#define PQ_FILE_NAME            "PQ.tab"
#define PQ_FINISH_FUNC          "OnPQFinish"
#define PQ_VALUE_CHANGE_FUNC    "OnPQValueChange"

KGPQ::KGPQ()
{
    new (this) KGPQ(0, NULL);
}

KGPQ::KGPQ(DWORD dwPQID, KGPQTemplateInfo* pPQTemplate)
{
    m_dwPQID = dwPQID;
    m_pPQTemplate = pPQTemplate;

    memset(m_nPQValue, 0, sizeof(m_nPQValue));

    m_bFinished         = false;
    m_eOwnerType        = pqotInvalid;
    m_dwOwnerID1        = 0;
    m_dwOwnerID2        = 0;
    m_dwOwnerID3        = 0;
}

BOOL KGPQ::SetValue(int nIndex, int nValue)
{
    BOOL bResult    = false;
    BOOL bRetCode   = false;

    KGLOG_PROCESS_ERROR(nIndex >= 0);
    KGLOG_PROCESS_ERROR(nIndex < PQ_PARAM_COUNT);

    m_nPQValue[nIndex] = nValue;

    CallScriptFunc(PQ_VALUE_CHANGE_FUNC);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KGPQ::Finish()
{
    BOOL bResult = false;

    KG_PROCESS_ERROR(!m_bFinished);

    m_bFinished = true;

    CallScriptFunc(PQ_FINISH_FUNC);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KGPQ::CallScriptFunc(const char cszFunction[])
{
    BOOL    bResult         = false;
    BOOL    bRetCode        = false;
    int     nLuaTopIndex    = 0;

    KG_PROCESS_ERROR(m_pPQTemplate->szScriptFile[0] != '\0');

    bRetCode = g_pSO3World->m_ScriptCenter.IsFuncExist(m_pPQTemplate->szScriptFile, cszFunction);
    KG_PROCESS_ERROR(bRetCode);

    g_pSO3World->m_ScriptCenter.SafeCallBegin(&nLuaTopIndex);

    g_pSO3World->m_ScriptCenter.PushValueToStack(m_dwPQID);
    g_pSO3World->m_ScriptCenter.CallFunction(m_pPQTemplate->szScriptFile, cszFunction, 1);
    g_pSO3World->m_ScriptCenter.GetValuesFromStack("b", &bRetCode);

    g_pSO3World->m_ScriptCenter.SafeCallEnd(nLuaTopIndex);
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

int KGPQ::LuaSetStatistics(Lua_State* L)
{
    BOOL    bResult         = false;
    int     nLuaTopIndex    = 0;
    DWORD   dwPlayerID      = 0;
    int     nIndex          = 0;
    int     nAddValue       = 0;

    nLuaTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nLuaTopIndex == 3);

    dwPlayerID  = (DWORD)Lua_ValueToNumber(L, 1);
    nIndex      = (int)Lua_ValueToNumber(L, 2);
    nAddValue   = (int)Lua_ValueToNumber(L, 3);

    KGLOG_PROCESS_ERROR(dwPlayerID);
    KGLOG_PROCESS_ERROR(nIndex >= 0);
    KGLOG_PROCESS_ERROR(nIndex < psiTotal);

    m_StatisticsMap[dwPlayerID].nValueArray[nIndex] += nAddValue;

    bResult = true;
Exit0:
    Lua_PushBoolean(L, bResult);
    return 1;
}

int KGPQ::LuaGetStatistics(Lua_State* L)
{
    int     nResult         = 0;
    int     nLuaTopIndex    = 0;
    DWORD   dwPlayerID      = 0;
    int     nIndex          = 0;

    nLuaTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nLuaTopIndex == 2);

    dwPlayerID  = (DWORD)Lua_ValueToNumber(L, 1);
    nIndex      = (int)Lua_ValueToNumber(L, 2);

    KGLOG_PROCESS_ERROR(dwPlayerID);
    KGLOG_PROCESS_ERROR(nIndex >= 0);
    KGLOG_PROCESS_ERROR(nIndex < psiTotal);

    nResult = m_StatisticsMap[dwPlayerID].nValueArray[nIndex];
Exit0:
    Lua_PushNumber(L, nResult);
    return 1;    
}

int KGPQ::LuaGetStatisticsTable(Lua_State* L)
{
    int nLuaTopIndex = 0;

    nLuaTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nLuaTopIndex == 0);

    Lua_NewTable(L);

    for (KPQ_STATISTICS_MAP::iterator it = m_StatisticsMap.begin(), itEnd = m_StatisticsMap.end(); it != itEnd; ++it)
    {
        Lua_PushNumber(L, it->first);

        Lua_NewTable(L);
        for (int i = 0; i < psiTotal; i++)
        {
            Lua_PushNumber(L, i);
            Lua_PushNumber(L, it->second.nValueArray[i]);

            Lua_SetTable(L, -3);
        }

        Lua_SetTable(L, -3);
    }

Exit0:
    return 1;
}

int KGPQ::LuaGetScene(Lua_State* L)
{
    int     nLuaTopIndex    = 0;
    DWORD   dwMapID         = 0;
    int     nMapCopyIndex   = 0;
    KScene* pScene          = NULL;

    nLuaTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nLuaTopIndex == 0);

    KGLOG_PROCESS_ERROR(m_eOwnerType == pqotScene || m_eOwnerType == pqotBattleField);

    dwMapID 	  = m_dwOwnerID1;
    nMapCopyIndex = m_dwOwnerID2;

    pScene = g_pSO3World->GetScene(dwMapID, nMapCopyIndex);
    KG_PROCESS_ERROR(pScene);

    return pScene->LuaGetObj(L);
Exit0:
    return 0;
}

DEFINE_LUA_CLASS_BEGIN(KGPQ)
    REGISTER_LUA_DWORD_READONLY(KGPQ, PQID)
    REGISTER_LUA_DWORD_READONLY(KGPQ, TemplateID)
    REGISTER_LUA_BOOL_READONLY(KGPQ, Finished)

    REGISTER_LUA_ENUM_READONLY(KGPQ, OwnerType)
    REGISTER_LUA_DWORD_READONLY(KGPQ, OwnerID1)
    REGISTER_LUA_DWORD_READONLY(KGPQ, OwnerID2)
    REGISTER_LUA_DWORD_READONLY(KGPQ, OwnerID3)

    REGISTER_LUA_FUNC(KGPQ, GetPQValue)

    REGISTER_LUA_FUNC(KGPQ, SetStatistics)
    REGISTER_LUA_FUNC(KGPQ, GetStatistics)
    REGISTER_LUA_FUNC(KGPQ, GetStatisticsTable)

    REGISTER_LUA_FUNC(KGPQ, GetScene)
DEFINE_LUA_CLASS_END(KGPQ)


BOOL KGPQList::AddPQ(DWORD dwPQID)
{
    BOOL                bResult         = false;
    KGPQ*               pPQ             = NULL;
    DWORD               dwPQTemplateID  = 0;
    KGPQTemplateInfo*   pPQTemplate     = NULL;

    pPQ = g_pSO3World->m_PQManager.GetPQ(dwPQID);
    KGLOG_PROCESS_ERROR(pPQ);

    dwPQTemplateID = pPQ->m_pPQTemplate->dwTemplateID;
    KGLOG_PROCESS_ERROR(dwPQTemplateID);

    pPQTemplate = g_pSO3World->m_PQManager.GetPQTemplate(dwPQTemplateID);
    KGLOG_PROCESS_ERROR(pPQTemplate);

    for (int i = 0; i < PQ_PARAM_COUNT; i++)
    {
        DWORD dwNpcTemplateID = pPQTemplate->dwKillNpcTemplateID[i];

        if (!dwNpcTemplateID)
            continue;

        m_Npc2PQMap.insert(std::make_pair(dwNpcTemplateID, std::make_pair(dwPQID, i)));
    }

    m_PQVector.push_back(dwPQID);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KGPQList::DelPQ(DWORD dwPQID)
{
    BOOL                    bResult = false;
    NPC_2_PQ_MAP::iterator  it;
    NPC_2_PQ_MAP::iterator  itEnd;

    m_PQVector.erase(
        std::remove(m_PQVector.begin(), m_PQVector.end(), dwPQID), 
        m_PQVector.end()
    );

    itEnd = m_Npc2PQMap.end();
    for (it = m_Npc2PQMap.begin(); it != itEnd; NULL)
    {
        if ((it->second).first == dwPQID)
            m_Npc2PQMap.erase(it++);
        else
            ++it;
    }

    bResult = true;
Exit0:
    return bResult;
}

#ifdef _SERVER
BOOL KGPQList::Load(BYTE* pbyData, size_t uDataLen)
{
    BOOL            bResult     = false;
    BOOL            bRetCode    = false;
    KGPQ_DB_LIST*   pDBList     = NULL;

    assert(pbyData);

    KGLOG_PROCESS_ERROR(uDataLen >= sizeof(KGPQ_DB_LIST));
    pDBList = (KGPQ_DB_LIST*)pbyData;

    KGLOG_PROCESS_ERROR(uDataLen == sizeof(KGPQ_DB_LIST) + sizeof(DWORD) * pDBList->wCount);

    for (int i = 0; i < pDBList->wCount; i++)
    {
        bRetCode = AddPQ(pDBList->dwPQID[i]);
        KGLOG_CHECK_ERROR(bRetCode);
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KGPQList::Save(size_t* puUsedSize, BYTE* pbyBuffer, size_t uBufferSize)
{
    BOOL            bResult     = false;
    size_t          uPQCount    = m_PQVector.size();
    size_t          uSize       = sizeof(KGPQ_DB_LIST) + sizeof(DWORD) * uPQCount;
    KGPQ_DB_LIST*   pDBList     = NULL;

    // 存储当前所接任务列表
    KGLOG_PROCESS_ERROR(uBufferSize >= uSize);

    pDBList = (KGPQ_DB_LIST*)pbyBuffer;

    KGLOG_PROCESS_ERROR(uPQCount < SHRT_MAX);
    pDBList->wCount = (WORD)uPQCount;

    for (int i = 0; i < (int)uPQCount; i++)
    {
        pDBList->dwPQID[i] = m_PQVector[i];
    }

    *puUsedSize = uSize;

    bResult = true;
Exit0:
    return bResult;
}

BOOL KGPQList::OnKillNpc(DWORD dwNpcTemplateID)
{
    BOOL                    bResult = false;
    NPC_2_PQ_MAP::iterator  it;
    NPC_2_PQ_MAP::iterator  itLower;
    NPC_2_PQ_MAP::iterator  itUpper;

    itLower = m_Npc2PQMap.lower_bound(dwNpcTemplateID);
    itUpper = m_Npc2PQMap.upper_bound(dwNpcTemplateID);

    for (it = itLower; it != itUpper; ++it)
    {
        DWORD   dwPQID = (it->second).first;
        int     nIndex = (it->second).second;

        g_RelayClient.DoApplyChangePQValue(dwPQID, nIndex, 1);
    }

    bResult = true;
Exit0:
    return bResult;
}
#endif

BOOL KGPQManager::Init()
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

    bResult = true;
Exit0:
    KG_COM_RELEASE(piTabFile);
    return bResult;
}

BOOL KGPQManager::UnInit()
{
    BOOL    bResult     = false;
    int     nRetCode    = false;

    m_PQTemplateMap.clear();
    m_PQMap.clear();

    bResult = true;
//Exit0:
    return bResult;
}

BOOL KGPQManager::LoadPQ(const KGPQ& crPQ)
{
    BOOL bResult = false;

    KGLOG_PROCESS_ERROR(crPQ.m_dwPQID);
    KGLOG_PROCESS_ERROR(crPQ.m_pPQTemplate);

    m_PQMap[crPQ.m_dwPQID] = crPQ;

    bResult = true;
Exit0:
    return bResult;
}

BOOL KGPQManager::DeletePQ(DWORD dwPQID)
{
    BOOL                bResult = false;
    PQ_MAP::iterator    it;

    it = m_PQMap.find(dwPQID);
    KGLOG_PROCESS_ERROR(it != m_PQMap.end());

    m_PQMap.erase(it);

    bResult = true;
Exit0:
    return bResult;
}

KGPQ* KGPQManager::GetPQ(DWORD dwPQID)
{
    KGPQ*               pResult = NULL;
    PQ_MAP::iterator    it;

    it = m_PQMap.find(dwPQID);
    KG_PROCESS_ERROR(it != m_PQMap.end());

    pResult = &(it->second);
Exit0:
    return pResult;
}

KGPQTemplateInfo* KGPQManager::GetPQTemplate(DWORD dwPQTemplateID)
{
    KGPQTemplateInfo*           pResult = NULL;
    PQ_TEMPLATE_MAP::iterator   itFind;

    itFind = m_PQTemplateMap.find(dwPQTemplateID);
    KGLOG_PROCESS_ERROR(itFind != m_PQTemplateMap.end());

    pResult = &(itFind->second);
Exit0:
    return pResult;
}

BOOL KGPQManager::LoadPQTemplate(ITabFile* piTabFile, int nIndex, KGPQTemplateInfo* pPQTemplateInfo, const KGPQTemplateInfo& rDefaultPQTemplate)
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
        nIndex, "Statistics",
        rDefaultPQTemplate.bStatistics, (int*)&pPQTemplateInfo->bStatistics
    );
    KGLOG_PROCESS_ERROR(bRetCode);

    pPQTemplateInfo->szScriptFile[0] = '\0';
    szScript[0] = '\0';
    bRetCode = piTabFile->GetString(
        nIndex, "Script",
        rDefaultPQTemplate.szScriptFile, szScript, sizeof(szScript)
        );
    KGLOG_PROCESS_ERROR(bRetCode);

    if (szScript[0] != '\0')
    {
        int nRetCode = snprintf(pPQTemplateInfo->szScriptFile, sizeof(pPQTemplateInfo->szScriptFile), "%s/%s", SCRIPT_DIR, szScript);
        KGLOG_PROCESS_ERROR(nRetCode > 0);
        KGLOG_PROCESS_ERROR(nRetCode < sizeof(pPQTemplateInfo->szScriptFile));
    }

    bResult = true;
Exit0:
    return bResult;
}
