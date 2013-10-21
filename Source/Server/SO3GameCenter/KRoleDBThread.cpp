#include "stdafx.h"
#include "mysql.h"
#include "errmsg.h"
#include "KRoleDB.h"
#include "KRoleDBDataDef.h"
#include "Common/CRC32.h"
#include "KSO3GameCenter.h"

void KRoleDB::WorkThreadFunction(void* pvParam)
{
    KRoleDB* pThis = (KRoleDB*)pvParam;

    assert(pThis);
    
    pThis->ThreadFunction();
}

int KRoleDB::ThreadFunction()
{   
    uint64_t    uBeginTime          = KG_GetTickCount();
    uint64_t    uTimeNow            = 0;
    int         nIdleCount          = 0;

    while (m_bRunFlag)
    {
        uTimeNow = KG_GetTickCount();

        if ((uTimeNow - uBeginTime) * GAME_FPS < (uint64_t)m_nDBThreadWorkLoop * 1000)
        {
            nIdleCount++;
            KGThread_Sleep(10);
            continue;
        }

        m_nDBThreadIdleLoop = nIdleCount;
        nIdleCount  = 0;

        DBThreadActivate();
    }

    ProcessSaveQueue(INT_MAX);

    KGLogPrintf(KGLOG_INFO, "[KRoleDB] Working thread exit !");

    return 0;
}

void KRoleDB::DBThreadActivate()
{    
    m_nDBThreadWorkLoop++;

    // 定期ping数据库，防止过久没操作数据库导致连接断开
    if (g_pSO3GameCenter->m_nTimeNow > m_nNextPingTime)
    {
        if (m_pDBHandle)
        {
            mysql_ping(m_pDBHandle);
        }
        m_nNextPingTime = g_pSO3GameCenter->m_nTimeNow + PING_CYCLE;
    }

    if (!m_pDBHandle)
    {
        static  time_t  nNextReConnectTime  = 0;

        if (g_pSO3GameCenter->m_nTimeNow >= nNextReConnectTime)
        {
            KGLogPrintf(KGLOG_DEBUG, "Reconnecting to the database ... ...\n");

            Connect();
            nNextReConnectTime = g_pSO3GameCenter->m_nTimeNow + 10;

            if (m_pDBHandle)
            {
                KGLogPrintf(KGLOG_DEBUG, "Reconnecting to the database successfully!\n");
            }
        }
    }

    KG_PROCESS_ERROR(m_pDBHandle); // 如果没有链接，不应该执行数据库操作

    // 处理"保存"队列
    ProcessSaveQueue(1);

    // 处理"加载"队列
    ProcessLoadQueue();

    // 处理"创建角色"
    ProcessCreateQueue();

    // 处理"删除角色"
    ProcessDeleteQueue();

    // 处理"取角色基本信息"
    ProcessQueryRoleBaseInfoQueue();

    // 处理"角色更名"
    ProcessRenameQueue();

Exit0:
    return;
}

static inline BOOL CheckRoleDataCRC(const BYTE* pbyRoleData, size_t uDataLen)
{
    BOOL                bResult         = false;
    KRoleDataHeader*    pRoleData       = NULL;
    DWORD               dwCRC           = 0;

    assert(pbyRoleData);

    KGLOG_PROCESS_ERROR(uDataLen > sizeof(KRoleDataHeader));
    pRoleData = (KRoleDataHeader*)pbyRoleData;
    
    dwCRC = CRC32(0, pbyRoleData + sizeof(KRoleDataHeader), (DWORD)(uDataLen - sizeof(KRoleDataHeader)));
    KGLOG_PROCESS_ERROR(dwCRC == pRoleData->dwCRC);
    
    bResult = true;
Exit0:
    return bResult;
}

void KRoleDB::ProcessCreateQueue()
{
    BOOL            bRetCode        = false;
    BYTE            byCode          = 0;
    KCREATE_REQUEST Request;
    KCREATE_RESULT  Result;

    while (m_pDBHandle)
    {       
        bRetCode = GetCreateTask(&Request);
        KG_PROCESS_ERROR(bRetCode);
        
        byCode = CreateRole(&Request.Param, &Result.RoleListInfo);
        
        Result.byCode       = byCode;
        Result.ulIdentity   = Request.ulIdentity;

        m_CreateResultLock.Lock();        
        m_CreateResult.push_back(Result);
        m_CreateResultLock.Unlock();
    }

Exit0:
    return;
}

void KRoleDB::ProcessSaveQueue(int nSaveCount)
{
    BOOL            bRetCode        = false;
    DWORD           dwPlayerID      = ERROR_ID;
    KSAVE_REQUEST   Request;

    memset(&Request, 0, sizeof(Request));

    while (m_pDBHandle && nSaveCount-- > 0)
    {
        KG_COM_RELEASE(Request.piExtInfo);

        bRetCode = GetSaveTask(&dwPlayerID, &Request);
        KG_PROCESS_ERROR(bRetCode);

        SaveRoleData(dwPlayerID, Request.BaseInfo, Request.piExtInfo);
    }

Exit0:
    KG_COM_RELEASE(Request.piExtInfo);
    return;
}

void KRoleDB::ProcessLoadQueue()
{
    BOOL                        bRetCode        = false;
    IKG_Buffer*                 piRoleData      = NULL;
    KLOAD_REQUEST               Request;
    KLOAD_RESULT                Result;

    while (m_pDBHandle)
    {
        bRetCode = GetLoadTask(&Request);
        KG_PROCESS_ERROR(bRetCode);
        
        KG_COM_RELEASE(piRoleData);
        bRetCode = LoadRoleData(Request.dwPlayerID, &piRoleData);
        
        m_LoadRetLock.Lock();
        
        Result.bSucceed   = bRetCode;
        Result.piRoleData = piRoleData;
        Result.dwPlayerID = Request.dwPlayerID;
        Result.nGSIndex   = Request.nGSIndex;

        if (Result.piRoleData)
        {
            Result.piRoleData->AddRef();
        }

        m_LoadRetQueue.push_back(Result);

        m_LoadRetLock.Unlock();
    }
    
Exit0:
    KG_COM_RELEASE(piRoleData);
    return;
}

void KRoleDB::ProcessDeleteQueue()
{
    BOOL            bRetCode = false;
    KDELETE_REQUEST Request;
    
    while (m_pDBHandle)
    {
        bRetCode = GetDeleteTask(&Request);
        KG_PROCESS_ERROR(bRetCode);

        DeleteRole(Request.dwRoleID);
    }

Exit0:
    return;
}

void KRoleDB::ProcessQueryRoleBaseInfoQueue()
{
    BOOL                            bRetCode        = false;
    KQUERY_ROLE_BASE_INFO_REQUEST   Request;
    KQUERY_ROLE_BASE_INFO_RESULT    Respond;

    while (m_pDBHandle)
    {
        bRetCode = GetQueryRoleBaseInfoTask(&Request);
        KG_PROCESS_ERROR(bRetCode);
        
        ZeroMemory(&Respond, sizeof(Respond));

        bRetCode = QueryRoleBaseInfo(Request.dwRoleID, &Respond.BaseInfo);
        
        Respond.dwRoleID        = Request.dwRoleID;
        Respond.nPlayerIndex    = Request.nPlayerIndex;
        Respond.nRoleIndex      = Request.nRoleIndex;
        Respond.nRoleCount      = Request.nRoleCount;
        Respond.bSucceed        = bRetCode;

        m_QueryRoleBaseResultQueueLock.Lock();
        m_QueryRoleBaseResultQueue.push_back(Respond);
        m_QueryRoleBaseResultQueueLock.Unlock();
    }

Exit0:
    return;
}

void KRoleDB::ProcessRenameQueue()
{
    BOOL            bRetCode    = false;
    KRole*          pRole       = NULL;
    KRENAME_REQUEST Request;
    KRENAME_RESULT  Respond;

    while (m_pDBHandle)
    {
        bRetCode = GetRenameTask(&Request);
        KG_PROCESS_ERROR(bRetCode);

        ZeroMemory(&Respond, sizeof(Respond));

        Respond.byCode = Rename(
            Request.dwRoleID, Request.szNewName
        );

        bRetCode = QueryRoleBaseInfoFromDB(
            Request.dwRoleID, &Respond.RoleListInfo.BaseInfo
        );
        KG_PROCESS_ERROR(bRetCode);

        pRole = g_pSO3GameCenter->m_RoleManager.GetRole(Request.dwRoleID);
        KGLOG_PROCESS_ERROR(pRole);

        Respond.RoleListInfo.dwPlayerID   = Request.dwRoleID;

        strncpy(Respond.RoleListInfo.szAccount, pRole->m_pszAccount, sizeof(Respond.RoleListInfo.szAccount));
        Respond.RoleListInfo.szAccount[sizeof(Respond.RoleListInfo.szAccount) - 1] = '\0';

        strncpy(Respond.RoleListInfo.szRoleName, Request.szNewName, sizeof(Respond.RoleListInfo.szRoleName));
        Respond.RoleListInfo.szRoleName[sizeof(Respond.RoleListInfo.szRoleName) - 1] = '\0';

        Respond.nPlayerIndex = Request.nPlayerIndex;
        Respond.dwRoleID     = Request.dwRoleID;

        m_RenameResultQueueLock.Lock();
        m_RenameResultQueue.push_back(Respond);
        m_RenameResultQueueLock.Unlock();
    }

Exit0:
    return;
}

DWORD KRoleDB::GetCurrentRoleID()
{
    DWORD               dwRetRoleID         = ERROR_ID;
    BOOL                bRetCode            = false;
    unsigned long long  ullCurrentRoleID    = 0;
    
    bRetCode = g_pSO3GameCenter->m_piSequence->GenerateID("RoleID", 1, &ullCurrentRoleID);
    KGLOG_PROCESS_ERROR(bRetCode);

    KGLOG_PROCESS_ERROR((ullCurrentRoleID > 0) && "ID Error!\n");
    KGLOG_PROCESS_ERROR(ullCurrentRoleID < UINT_MAX);

    dwRetRoleID = (DWORD)ullCurrentRoleID;
Exit0:
    return dwRetRoleID;
}

BOOL KRoleDB::GetRoleBaseInfo(KDB_ROLE_GENERATOR_PARAM* pParam, DWORD dwRoleID, KRoleBaseInfo* pRetBaseInfo)
{
    BOOL      bResult  = false;

    assert(pParam);
    assert(pRetBaseInfo);

    // ---------------- Set Base Information ----------------->
    KGLOG_PROCESS_ERROR(pParam->UserParam.cRoleType > rtInvalid && pParam->UserParam.cRoleType < rtTotal);
    
    ZeroMemory(pRetBaseInfo, sizeof(KRoleBaseInfo));

    pRetBaseInfo->cRoleType                    = pParam->UserParam.cRoleType;
    pRetBaseInfo->CurrentPos.dwMapID           = pParam->UserParam.dwMapID;
    pRetBaseInfo->CurrentPos.nMapCopyIndex     = pParam->UserParam.nMapCopyIndex;
    pRetBaseInfo->CurrentPos.nX                = pParam->nX;
    pRetBaseInfo->CurrentPos.nY                = pParam->nY;
    pRetBaseInfo->CurrentPos.nZ                = pParam->nZ;

    pRetBaseInfo->byLevel                      = 1;
    pRetBaseInfo->byCamp                       = cNeutral;
    pRetBaseInfo->byForceID                    = 0;
    pRetBaseInfo->CurrentPos.byFaceDirection   = pParam->byDirection;
    pRetBaseInfo->nCreateTime                  = g_pSO3GameCenter->m_nTimeNow;

    memcpy(pRetBaseInfo->wRepresentId, pParam->UserParam.wRepresentId, sizeof(pRetBaseInfo->wRepresentId));
    
    bResult = true;
Exit0:
    return bResult;
}

BOOL KRoleDB::GetCreateTask(KCREATE_REQUEST* pTask)
{
    BOOL bResult = false;

    assert(pTask);

    m_CreateQueueLock.Lock();

    KG_PROCESS_ERROR(!m_CreateQueue.empty());

    *pTask = m_CreateQueue.front();
    m_CreateQueue.pop_front();

    bResult = true;
Exit0:
    m_CreateQueueLock.Unlock();

    return bResult;
}

BOOL KRoleDB::CheckRoleCountInAccount(KDB_ROLE_GENERATOR_PARAM* pCreateRoleParam)
{
    BOOL            bResult         = false;
    BOOL            bRetCode        = false;
    int             nRetCode        = 0;
    my_ulonglong    ullRowCount     = 0;             
    unsigned long   ulLength        = 0;
    char*           pszPos          = NULL;
    size_t          uLeftBufferSize = 0;
    MYSQL_RES*      pQueryRes       = NULL;   
    MYSQL_ROW       QueryRow;
    
    pszPos          = m_pszSQL;
    uLeftBufferSize = MAX_ROLE_DB_SQL_SIZE;

    nRetCode = snprintf(
        m_pszSQL, uLeftBufferSize, "select count(*) from %s where %s = '",
        ROLE_TABLE_NAME, KG_ACCOUNT_FIELD_NAME
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    ulLength = (unsigned long)(strlen(pCreateRoleParam->szAccountName));
    nRetCode = mysql_real_escape_string(m_pDBHandle, pszPos, pCreateRoleParam->szAccountName, ulLength);
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;  

    nRetCode = snprintf(pszPos, uLeftBufferSize, "' ;");
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);

    bRetCode = DoQuery(m_pszSQL);
    KGLOG_PROCESS_ERROR(bRetCode);
    
    pQueryRes = mysql_store_result(m_pDBHandle);
    KGLOG_PROCESS_ERROR(pQueryRes);

    nRetCode = (int)mysql_num_rows(pQueryRes);
    KG_PROCESS_ERROR(nRetCode == 1);

    QueryRow = mysql_fetch_row(pQueryRes);
    KG_PROCESS_ERROR(QueryRow);

#ifdef WIN32
    ullRowCount = (my_ulonglong)_atoi64(QueryRow[0]);
#else
    ullRowCount = (my_ulonglong)atoll(QueryRow[0]);
#endif
    
    assert(g_pSO3GameCenter);
    KGLOG_PROCESS_ERROR(ullRowCount < (my_ulonglong)(g_pSO3GameCenter->m_Settings.m_nMaxRoleCount));

    bResult = true;
Exit0:
    if (pQueryRes)
    {
        mysql_free_result(pQueryRes);
        pQueryRes = NULL;
    }

    return bResult;      
}

BYTE KRoleDB::CreateRole(
    KDB_ROLE_GENERATOR_PARAM* pCreateRoleParam, KROLE_LIST_INFO* pRetRoleListInfo
)
{
    BYTE            byResult        = eCreateRoleUnableToCreate;
    BOOL            bRetCode        = false;
    int             nRetCode        = 0;
    DWORD           dwRoleID        = ERROR_ID;
    time_t          nCurrentTime    = time(NULL);
    my_ulonglong    lAffectedRow    = 0;
    char*           pszPos          = NULL;
    size_t          uLeftBufferSize = 0;
    unsigned long   ulLength        = 0;
    KRoleBaseInfo   BaseInfo;

    assert(pCreateRoleParam);
    assert(pRetRoleListInfo);
    
    bRetCode = CheckRoleCountInAccount(pCreateRoleParam);
    KG_PROCESS_ERROR(bRetCode);

    dwRoleID = GetCurrentRoleID();
    KGLOG_PROCESS_ERROR(dwRoleID != ERROR_ID);

    bRetCode = GetRoleBaseInfo(pCreateRoleParam, dwRoleID, &BaseInfo);
    KGLOG_PROCESS_ERROR(bRetCode);

    uLeftBufferSize = MAX_ROLE_DB_SQL_SIZE;
    pszPos = m_pszSQL;

    nRetCode = snprintf(
        pszPos, uLeftBufferSize, "insert into %s (%s, %s, %s, %s, %s, %s) "
        "values (%lu, from_unixtime(%ld), '",
        ROLE_TABLE_NAME, KG_ID_FIELD_NAME, KG_LAST_MODIFY_FIELD_NAME, KG_NAME_FIELD_NAME, KG_ACCOUNT_FIELD_NAME, 
        KG_BASE_INFO_FIELD_NAME, KG_EXT_INFO_FIELD_NAME,
        dwRoleID, nCurrentTime
    );

    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    ulLength = (unsigned long)strlen(pCreateRoleParam->UserParam.szRoleName);
    KGLOG_PROCESS_ERROR(uLeftBufferSize > ulLength * 2);
    nRetCode = mysql_real_escape_string(m_pDBHandle, pszPos, pCreateRoleParam->UserParam.szRoleName, ulLength);
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    nRetCode = snprintf(pszPos, uLeftBufferSize, "', '");
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    ulLength = (unsigned long)strlen(pCreateRoleParam->szAccountName);
    KGLOG_PROCESS_ERROR(uLeftBufferSize > ulLength * 2);
    nRetCode = mysql_real_escape_string(m_pDBHandle, pszPos, pCreateRoleParam->szAccountName, ulLength);
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    nRetCode = snprintf(pszPos, uLeftBufferSize, "', '");
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    ulLength = (unsigned long)sizeof(KRoleBaseInfo);
    KGLOG_PROCESS_ERROR(uLeftBufferSize > ulLength * 2);
    nRetCode = mysql_real_escape_string(m_pDBHandle, pszPos, (const char*)&BaseInfo, ulLength);
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    nRetCode = snprintf(pszPos, uLeftBufferSize, "', null);");
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;
    
    bRetCode = DoQuery(m_pszSQL);
    if (!bRetCode) 
    {
        if (m_pDBHandle) // 执行失败的情况下且数据库连接没有断开
        {
            byResult = eCreateRoleNameAlreadyExist;
        }
        goto Exit0;
    }

    lAffectedRow = mysql_affected_rows(m_pDBHandle);
    KGLOG_PROCESS_ERROR(lAffectedRow == 1);
    
    pRetRoleListInfo->BaseInfo       = BaseInfo;
    pRetRoleListInfo->nFreezeTime    = 0;
    pRetRoleListInfo->nDeleteTime    = 0;
    pRetRoleListInfo->dwPlayerID     = dwRoleID;

    strncpy(pRetRoleListInfo->szRoleName, pCreateRoleParam->UserParam.szRoleName, sizeof(pRetRoleListInfo->szRoleName));
    pRetRoleListInfo->szRoleName[sizeof(pRetRoleListInfo->szRoleName) - 1] = '\0';

    strncpy(pRetRoleListInfo->szAccount, pCreateRoleParam->szAccountName, sizeof(pRetRoleListInfo->szAccount));
    pRetRoleListInfo->szAccount[sizeof(pRetRoleListInfo->szAccount) - 1] = '\0';

    KGLogPrintf(
        KGLOG_INFO, "[KRoleDB] Created role %s, RoleID=%u succeed\n",
        pCreateRoleParam->UserParam.szRoleName, dwRoleID
    );

    byResult = eCreateRoleSucceed;
Exit0:
    return byResult;      
}

BOOL KRoleDB::GetSaveTask(DWORD* pdwRetRoleID, KSAVE_REQUEST* pTask)
{
    BOOL                        bResult     = false;
    DWORD                       dwPlayerID  = ERROR_ID;
    KSAVE_DATA_TABLE::iterator  it;
    
    assert(pTask);
    assert(pdwRetRoleID);

    m_SaveQueueLock.Lock();
    
    KG_PROCESS_ERROR(!m_SaveQueue.empty());

    dwPlayerID = m_SaveQueue.front();
    m_SaveQueue.pop_front();

    it = m_SaveDataTable.find(dwPlayerID);
    KGLOG_PROCESS_ERROR(it != m_SaveDataTable.end());

    *pTask = it->second;
    m_SaveDataTable.erase(it);

    *pdwRetRoleID = dwPlayerID;

    bResult = true;
Exit0:
    m_SaveQueueLock.Unlock();
    return bResult;
}

BOOL KRoleDB::SaveRoleData(DWORD dwPlayerID, const KRoleBaseInfo& crBaseInfo, IKG_Buffer* piExtInfo)
{
    BOOL            bResult             = false;
    int             nRetCode            = false;
    BYTE*           pbyExtInfo          = NULL;
    unsigned        uExtInfoDataLen     = 0;
    time_t          nCurrentTime        = time(NULL);
    my_ulonglong    lAffectedRow        = 0;
    unsigned long   ulLength            = 0;
    char*           pszPos              = NULL;
    size_t          uLeftBufferSize     = 0;
    DWORD           dwTime              = 0;
    
    assert(piExtInfo);

    dwTime = KG_GetTickCount();
    
    pbyExtInfo = (BYTE*)piExtInfo->GetData();
    KGLOG_PROCESS_ERROR(pbyExtInfo);
    
    uExtInfoDataLen = piExtInfo->GetSize();

    nRetCode = CheckRoleDataCRC(pbyExtInfo, uExtInfoDataLen);
    KGLOG_PROCESS_ERROR(nRetCode);

    uLeftBufferSize = MAX_ROLE_DB_SQL_SIZE;
    pszPos = m_pszSQL;

    nRetCode = snprintf(
        pszPos, uLeftBufferSize, "update %s set %s = from_unixtime(%ld), %s = '",
        ROLE_TABLE_NAME, KG_LAST_MODIFY_FIELD_NAME, nCurrentTime, KG_BASE_INFO_FIELD_NAME
    );

    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    ulLength = (unsigned long)sizeof(KRoleBaseInfo);
    KGLOG_PROCESS_ERROR(uLeftBufferSize > ulLength * 2);
    nRetCode = mysql_real_escape_string(m_pDBHandle, pszPos, (const char*)&crBaseInfo, ulLength);
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    nRetCode = snprintf(pszPos, uLeftBufferSize, "', %s = '", KG_EXT_INFO_FIELD_NAME);
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    ulLength = (unsigned long)uExtInfoDataLen;
    KGLOG_PROCESS_ERROR(uLeftBufferSize > ulLength * 2);
    nRetCode = mysql_real_escape_string(m_pDBHandle, pszPos, (const char*)pbyExtInfo, ulLength);
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    pszPos          += nRetCode;
    uLeftBufferSize -= nRetCode;

    nRetCode = snprintf(pszPos, uLeftBufferSize, "' where %s = %lu", KG_ID_FIELD_NAME, dwPlayerID);
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);

    nRetCode = DoQuery(m_pszSQL);
    KGLOG_PROCESS_ERROR(nRetCode);

    lAffectedRow = mysql_affected_rows(m_pDBHandle);
    KGLOG_PROCESS_ERROR(lAffectedRow <= 1);

    dwTime  = KG_GetTickCount() - dwTime;

    m_fRoleSaveTimeCost = m_fRoleSaveTimeCost * 0.95f + (float)dwTime * 0.05f;

    bResult = true;
Exit0:
    if (!bResult)
    {
        KGLogPrintf(KGLOG_ERR, "Save role data failed: ID = %u", dwPlayerID);
    }
    return bResult;
}


BOOL KRoleDB::GetLoadTask(KLOAD_REQUEST* pTask)
{
    BOOL bResult = false;

    assert(pTask);

    m_LoadQueueLock.Lock();
    
    KG_PROCESS_ERROR(!m_LoadQueue.empty());

    *pTask = m_LoadQueue.front();
    m_LoadQueue.pop_front();

    bResult = true;
Exit0:
    m_LoadQueueLock.Unlock();
    return bResult;
}

BOOL KRoleDB::LoadRoleData(DWORD dwRoleID, IKG_Buffer** ppiRoleExtInfo)
{
    BOOL      bResult     = false;
    BOOL      bRetCode    = false;
    DWORD     dwTime      = 0;
    
    assert(ppiRoleExtInfo);
   
    bRetCode = LoadRoleDataFromSaveQueue(dwRoleID, ppiRoleExtInfo);
    KG_PROCESS_SUCCESS(bRetCode);

    dwTime = KG_GetTickCount();

    bRetCode = LoadRoleDataFromDB(dwRoleID, ppiRoleExtInfo);
    KGLOG_PROCESS_ERROR(bRetCode);

    dwTime = KG_GetTickCount() - dwTime;

    m_fRoleLoadTimeCost  = m_fRoleLoadTimeCost * 0.95f + (float)dwTime * 0.05f;

Exit1:
    bResult = true;
Exit0:
    return bResult;
}

BOOL KRoleDB::LoadRoleDataFromSaveQueue(DWORD dwRoleID, IKG_Buffer** ppiRoleExtInfo)
{
    BOOL                        bResult             = false;
    IKG_Buffer*                 piExtInfo           = NULL;
    KSAVE_DATA_TABLE::iterator  it;

    assert(ppiRoleExtInfo);

    m_SaveQueueLock.Lock();
    it = m_SaveDataTable.find(dwRoleID);
    if (it != m_SaveDataTable.end())
    {
        piExtInfo = it->second.piExtInfo;
        piExtInfo->AddRef();
    }
    m_SaveQueueLock.Unlock();

    KG_PROCESS_ERROR(piExtInfo);

    *ppiRoleExtInfo = piExtInfo;
    //(*ppiRoleExtInfo)->AddRef();

    bResult = true;
Exit0:
    //KG_COM_RELEASE(piExtInfo);
    return bResult;
}

BOOL KRoleDB::LoadRoleDataFromDB(DWORD dwRoleID, IKG_Buffer** ppiRoleExtInfo)
{
    BOOL                        bResult         = false;
    int                         nRetCode        = false;
    HRESULT                     hRetCode        = E_FAIL;
    unsigned long long          ullRoleCount    = 0;
    IKG_Buffer*                 piRoleExtInfo   = NULL;
    void*                       pvRoleExtInfo  = NULL;
    unsigned long*              pulRoleDataSize = NULL;
    MYSQL_RES*                  pQueryRes       = NULL;
    MYSQL_ROW                   QueryRow;
    char                        szSQL[1024];
    
    assert(ppiRoleExtInfo);

    nRetCode =  snprintf(
        szSQL, sizeof(szSQL), "select %s from %s where %s = %lu;",
        KG_EXT_INFO_FIELD_NAME, ROLE_TABLE_NAME, KG_ID_FIELD_NAME, dwRoleID
    );
    KGLOG_PROCESS_ERROR(nRetCode < (int)sizeof(szSQL));

    nRetCode = DoQuery(szSQL);
    KGLOG_PROCESS_ERROR(nRetCode);

    pQueryRes = mysql_store_result(m_pDBHandle);
    KGLOG_PROCESS_ERROR(pQueryRes);

    ullRoleCount = mysql_num_rows(pQueryRes);
    KGLOG_PROCESS_ERROR(ullRoleCount == 1);

    QueryRow = mysql_fetch_row(pQueryRes);
    KGLOG_PROCESS_ERROR(QueryRow);
    
    *ppiRoleExtInfo = NULL;

    if (QueryRow[0] == NULL) // 刚创建的角色
    {
        goto Exit1;
    }
    
    pulRoleDataSize = mysql_fetch_lengths(pQueryRes);
    KGLOG_PROCESS_ERROR(pulRoleDataSize);

    piRoleExtInfo = KG_MemoryCreateBuffer(*pulRoleDataSize);
    KGLOG_PROCESS_ERROR(piRoleExtInfo);

    pvRoleExtInfo = piRoleExtInfo->GetData();
    KGLOG_PROCESS_ERROR(pvRoleExtInfo);

    memcpy(pvRoleExtInfo, QueryRow[0], *pulRoleDataSize);

    *ppiRoleExtInfo = piRoleExtInfo;
    (*ppiRoleExtInfo)->AddRef();

Exit1:
    bResult = true;
Exit0:
    KG_COM_RELEASE(piRoleExtInfo);

    if(pQueryRes)
    {
        mysql_free_result(pQueryRes);
        pQueryRes = NULL;
    }
    return bResult;
}

BOOL KRoleDB::GetDeleteTask(KDELETE_REQUEST* pTask)
{
    BOOL bResult = false;

    assert(pTask);

    m_DeleteQueueLock.Lock();

    KG_PROCESS_ERROR(!m_DeleteQueue.empty());

    *pTask = m_DeleteQueue.front();
    m_DeleteQueue.pop_front();

    bResult = true;
Exit0:
    m_DeleteQueueLock.Unlock();
    return bResult;
}

BOOL KRoleDB::DeleteRole(DWORD dwRoleID)
{
    BOOL                bResult         = false;
    int                 nRetCode        = false;
    unsigned long       ulLength        = 0;
    my_ulonglong        lAffectedRow    = 0;
    time_t              nCurrentTime    = time(NULL);
    char                szSQL[1024];

    nRetCode = snprintf(
        szSQL, sizeof(szSQL), 
        "insert into %s (%s,%s,%s,%s,%s) "
        "   (select %s, %s, %s, %s, from_unixtime(%ld) from %s where %s = %lu);",
        RESTORE_ROLE_TABLE_NAME, 
        KG_NAME_FIELD_NAME, KG_ACCOUNT_FIELD_NAME, KG_BASE_INFO_FIELD_NAME, KG_EXT_INFO_FIELD_NAME, KG_DELETE_TIME_FIELD_NAME,
        KG_NAME_FIELD_NAME, KG_ACCOUNT_FIELD_NAME, KG_BASE_INFO_FIELD_NAME, KG_EXT_INFO_FIELD_NAME, nCurrentTime, 
        ROLE_TABLE_NAME, KG_ID_FIELD_NAME, dwRoleID
    );
    KGLOG_PROCESS_ERROR(nRetCode < (int)sizeof(szSQL));
    
    nRetCode = DoQuery(szSQL);
    KGLOG_PROCESS_ERROR(nRetCode);

    lAffectedRow = mysql_affected_rows(m_pDBHandle);
    KGLOG_PROCESS_ERROR(lAffectedRow == 1);
    
    nRetCode = snprintf(
        szSQL, sizeof(szSQL), "delete from %s where %s = %lu;",
        ROLE_TABLE_NAME, KG_ID_FIELD_NAME, dwRoleID
    );
    KGLOG_PROCESS_ERROR(nRetCode < (int)sizeof(szSQL));
    
    nRetCode = DoQuery(szSQL);
    KGLOG_PROCESS_ERROR(nRetCode);

    lAffectedRow = mysql_affected_rows(m_pDBHandle);
    KGLOG_PROCESS_ERROR(lAffectedRow == 1);

    KGLogPrintf(KGLOG_DEBUG, "Delete role %u successfully.", dwRoleID);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KRoleDB::GetQueryRoleBaseInfoTask(KQUERY_ROLE_BASE_INFO_REQUEST* pTask)
{
    BOOL bResult = false;

    assert(pTask);

    m_QueryRoleBaseInfoQueueLock.Lock();

    KG_PROCESS_ERROR(!m_QueryRoleBaseInfoQueue.empty());

    *pTask = m_QueryRoleBaseInfoQueue.front();
    m_QueryRoleBaseInfoQueue.pop_front();

    bResult = true;
Exit0:
    m_QueryRoleBaseInfoQueueLock.Unlock();
    return bResult;
}

BOOL KRoleDB::QueryRoleBaseInfo(DWORD dwRoleID, KRoleBaseInfo* pRetBaseInfo)
{
    BOOL    bResult     = false;
    BOOL    bRetCode    = false;

    assert(pRetBaseInfo);
    
    bRetCode = QueryRoleBaseInfoFromSaveQueue(dwRoleID, pRetBaseInfo);
    KG_PROCESS_SUCCESS(bRetCode);
    
    bRetCode = QueryRoleBaseInfoFromDB(dwRoleID, pRetBaseInfo);
    KGLOG_PROCESS_ERROR(bRetCode);
    
Exit1:
    bResult = true;
Exit0:
    return bResult;
}

BOOL KRoleDB::QueryRoleBaseInfoFromSaveQueue(DWORD dwRoleID, KRoleBaseInfo* pBaseInfo)
{
    BOOL                        bResult         = false;
    KSAVE_DATA_TABLE::iterator  it;

    m_SaveQueueLock.Lock();

    it = m_SaveDataTable.find(dwRoleID);
    KG_PROCESS_ERROR(it != m_SaveDataTable.end());
    
    *pBaseInfo = it->second.BaseInfo;

    bResult = true;
Exit0:
    m_SaveQueueLock.Unlock();
    return bResult;
}

BOOL KRoleDB::QueryRoleBaseInfoFromDB(DWORD dwRoleID, KRoleBaseInfo* pBaseInfo)
{
    BOOL                        bResult         = false;
    int                         nRetCode        = false;
    unsigned long long          ullRecordCount  = 0;
    unsigned long               ulLength        = 0;
    MYSQL_RES*                  pQueryRes       = NULL;
    MYSQL_ROW                   QueryRow        = NULL;
    unsigned long*              QueryLen        = NULL;
    KRole*                      pRole           = NULL;
    char                        szSQL[1024];

    assert(pBaseInfo);
    
    nRetCode = snprintf(
        szSQL, sizeof(szSQL), "select %s from %s where %s = %lu",
        KG_BASE_INFO_FIELD_NAME, ROLE_TABLE_NAME, KG_ID_FIELD_NAME, dwRoleID
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)sizeof(szSQL));

    nRetCode = DoQuery(szSQL);
    KGLOG_PROCESS_ERROR(nRetCode);

    pQueryRes = mysql_store_result(m_pDBHandle);
    KGLOG_PROCESS_ERROR(pQueryRes);
    
    ullRecordCount = mysql_num_rows(pQueryRes);
    KGLOG_PROCESS_ERROR(ullRecordCount == 1);

    QueryRow = mysql_fetch_row(pQueryRes);
    KG_PROCESS_ERROR(QueryRow);

    QueryLen = mysql_fetch_lengths(pQueryRes);
    KG_PROCESS_ERROR(QueryLen);

    KGLOG_PROCESS_ERROR(QueryLen[0] == sizeof(KRoleBaseInfo));

    memcpy(pBaseInfo, QueryRow[0], QueryLen[0]);

    bResult = true;
Exit0:
    if (pQueryRes)
    {
        mysql_free_result(pQueryRes);
        pQueryRes = NULL;
    }
    return bResult;
}

BOOL KRoleDB::GetRenameTask(KRENAME_REQUEST* pTask)
{
    BOOL    bResult = false;

    assert(pTask);

    m_RenameQueueLock.Lock();

    KG_PROCESS_ERROR(!m_RenameQueue.empty());

    *pTask = m_RenameQueue.front();
    m_RenameQueue.pop_front();

    bResult = true;
Exit0:
    m_RenameQueueLock.Unlock();
    return bResult;
}

BYTE KRoleDB::Rename(DWORD dwRoleID, char szNewName[])
{
    BYTE                        byResult        = eRenameUnknownError;
    int                         nRetCode        = false;
    KRole*                      pRole           = NULL;
    char                        szSQL[1024];

    nRetCode = snprintf(
        szSQL, sizeof(szSQL), "update %s set %s = '%s' where %s = %lu",
        ROLE_TABLE_NAME, KG_NAME_FIELD_NAME, szNewName, KG_ID_FIELD_NAME, dwRoleID
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)sizeof(szSQL));

    nRetCode = DoQuery(szSQL);
    if (!nRetCode)
    {
        if (m_pDBHandle)
        {
            byResult = eRenameNameAlreadyExist;
        }
        goto Exit0;
    }

    byResult = eRenameSucceed;
Exit0:
    return byResult;
}

BOOL KRoleDB::DoQuery(const char cszSQL[])
{
    BOOL     bResult            = false;
    BOOL     bRetCode           = false;
    unsigned ulMySQLErrorCode   = 0;

    assert(cszSQL);
    assert(m_pDBHandle);
    
    bRetCode = MySQL_Query(m_pDBHandle, cszSQL, &ulMySQLErrorCode);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    if (ulMySQLErrorCode != 0 && ulMySQLErrorCode != ER_DUP_ENTRY) // 主键冲突不用断开链接
    {
        MySQL_Disconnect(m_pDBHandle);
        m_pDBHandle = NULL;
    }
    return bResult;
}
