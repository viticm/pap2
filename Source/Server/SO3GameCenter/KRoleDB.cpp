//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   KRoleDB.cpp
//  Version     :   1.0
//  Creater     :   Zhao Chunfeng
//  Date        :   2008-06-02 09:14:39
//  Comment     :   角色数据库相关操作
//////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "KRoleDB.h"
#include "KSO3GameCenter.h"

#define MAX_SQL_SIZE 1024
#define PING_CYCLE   30

KRoleDB::KRoleDB()
{
    m_pDBHandle             = NULL;
    m_nNextPingTime         = 0;
    m_bRunFlag              = false;
    m_pszSQL                = NULL;
    m_nDBThreadWorkLoop     = 0;
    m_nDBThreadIdleLoop     = 0;
}

KRoleDB::~KRoleDB()
{
    assert(m_pszSQL == NULL);
}

BOOL KRoleDB::Init()
{
    BOOL    bResult         = false;
    BOOL    bRetCode        = false;
    BOOL    bConnected      = false;

    m_pszSQL = new char[MAX_ROLE_DB_SQL_SIZE];
    KGLOG_PROCESS_ERROR(m_pszSQL);

    bRetCode = Connect();
    KGLOG_PROCESS_ERROR(bRetCode);
    bConnected = true;
    
    m_fRoleSaveTimeCost = 0.0f;
    m_fRoleLoadTimeCost = 0.0f;

    m_bRunFlag = true;

    bRetCode = m_WorkThread.Create(WorkThreadFunction, this);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    if (!bResult)
    {
        KG_DELETE_ARRAY(m_pszSQL);
        
        if (bConnected)
        {
            Disconnect();
            bConnected = false;
        }
    }
    return bResult;
}


void KRoleDB::UnInit()
{
    while (true)
    {   
        size_t uLeftCount = 0;

        m_SaveQueueLock.Lock();
        uLeftCount = m_SaveQueue.size();
        m_SaveQueueLock.Unlock();

        if (uLeftCount == 0)
            break;

        KGLogPrintf(KGLOG_INFO, "Waiting for save role data: %u\n", uLeftCount);
        KGThread_Sleep(1000);
    }

    m_bRunFlag = false;
    m_WorkThread.Destroy();
    
    Disconnect();
    
    assert(m_SaveDataTable.empty());

    for (KLOAD_RESULT_QUEUE::iterator it = m_LoadRetQueue.begin(); it != m_LoadRetQueue.end(); ++it)
    {
        KG_COM_RELEASE(it->piRoleData);
    }
    
    KG_DELETE_ARRAY(m_pszSQL);
}

void KRoleDB::Activate()
{
    DealWithLoadRoleDataRespond();

    DealWithCreateRoleRespond();
    
    DealWithQueryRoleBaseDataRespond();

    DealWithRenameRespond();
}

void KRoleDB::DoCreateRole(KDB_ROLE_GENERATOR_PARAM* pRoleParam, unsigned long ulIdentity)
{
    KCREATE_REQUEST CreateRequest;

    assert(pRoleParam);

    CreateRequest.Param      = *pRoleParam;
    CreateRequest.ulIdentity = ulIdentity;

    m_CreateQueueLock.Lock();
    m_CreateQueue.push_back(CreateRequest);
    m_CreateQueueLock.Unlock();
    
    return;
}

void KRoleDB::DoSaveRoleData(int nGSIndex, DWORD dwPlayerID, const KRoleBaseInfo& crBaseInfo, IKG_Buffer* piRoleData, int nUserValue)
{
    
    assert(piRoleData);

    m_SaveQueueLock.Lock();

    KSAVE_DATA_TABLE::iterator it = m_SaveDataTable.find(dwPlayerID);

    if (it != m_SaveDataTable.end())
    {
        KSAVE_REQUEST* pRecord = &it->second;

        pRecord->nGSIndex   = nGSIndex;
        pRecord->nUserValue = nUserValue;
        pRecord->BaseInfo   = crBaseInfo;

        KG_COM_RELEASE(pRecord->piExtInfo);

        pRecord->piExtInfo = piRoleData;
        pRecord->piExtInfo->AddRef();
    }
    else
    {
        KSAVE_REQUEST SaveRequest;

        SaveRequest.nGSIndex    = nGSIndex;
        SaveRequest.nUserValue  = nUserValue;
        SaveRequest.BaseInfo    = crBaseInfo;

        SaveRequest.piExtInfo = piRoleData;
        SaveRequest.piExtInfo->AddRef();

        m_SaveDataTable[dwPlayerID] = SaveRequest;
        m_SaveQueue.push_back(dwPlayerID);
    }

    m_SaveQueueLock.Unlock();
    
    return;
}

void KRoleDB::DoLoadRoleData(int nGSIndex, DWORD dwPlayerID)
{
    KLOAD_REQUEST   Request;
    
    Request.dwPlayerID  = dwPlayerID;
    Request.nGSIndex    = nGSIndex;

    m_LoadQueueLock.Lock();
    m_LoadQueue.push_back(Request);
    m_LoadQueueLock.Unlock();

    return;
}

void KRoleDB::DoDeleteRole(DWORD dwRoleID)
{
    KDELETE_REQUEST   Request;
        
    Request.dwRoleID   = dwRoleID;

    m_DeleteQueueLock.Lock();
    m_DeleteQueue.push_back(Request);
    m_DeleteQueueLock.Unlock();

    g_pSO3GameCenter->m_StatDataManager.UpdataCreateOrDeleteRole(false);

    return;
}


void KRoleDB::DoQueryRoleBaseData(DWORD dwRoleID, int nPlayerIndex, int nRoleIndex, int nRoleCount)
{
    KQUERY_ROLE_BASE_INFO_REQUEST Request;
    
    Request.dwRoleID        = dwRoleID;
    Request.nPlayerIndex    = nPlayerIndex;
    Request.nRoleIndex      = nRoleIndex;
    Request.nRoleCount      = nRoleCount;

    m_QueryRoleBaseInfoQueueLock.Lock();
    m_QueryRoleBaseInfoQueue.push_back(Request);
    m_QueryRoleBaseInfoQueueLock.Unlock();

    return;
}

void KRoleDB::DoRename(DWORD dwRoleID, int nPlayerIndex, char szNewName[])
{
    KRENAME_REQUEST Request;

    Request.dwRoleID        = dwRoleID;
    Request.nPlayerIndex    = nPlayerIndex;

    strncpy(Request.szNewName, szNewName, sizeof(Request.szNewName));
    Request.szNewName[sizeof(Request.szNewName) - 1] = '\0';

    m_RenameQueueLock.Lock();
    m_RenameQueue.push_back(Request);
    m_RenameQueueLock.Unlock();

    return;
}

BOOL KRoleDB::Connect()
{
    BOOL    bResult    = false;
    
    assert(m_pDBHandle == NULL);

    m_pDBHandle = MySQL_Connect(
        g_pSO3GameCenter->m_Settings.m_szDBIP,
        g_pSO3GameCenter->m_Settings.m_szDBName, 
        g_pSO3GameCenter->m_Settings.m_szDBAcc,
        g_pSO3GameCenter->m_Settings.m_szDBPsw    
    );
    KG_PROCESS_ERROR(m_pDBHandle);

    bResult = true;
Exit0:
    if (!bResult)
    {
        if (m_pDBHandle)
        {
            MySQL_Disconnect(m_pDBHandle);
            m_pDBHandle = NULL;
        }
    }
    return bResult;
}

void KRoleDB::Disconnect()
{
    if (m_pDBHandle)
    {
        MySQL_Disconnect(m_pDBHandle);
        m_pDBHandle = NULL;
    }
}

void KRoleDB::DealWithLoadRoleDataRespond()
{
    m_LoadRetLock.Lock();

    for (KLOAD_RESULT_QUEUE::iterator it = m_LoadRetQueue.begin(); it != m_LoadRetQueue.end(); ++it)
    {
        BYTE*       pbyRoleData     = NULL;
        unsigned    uRoleDataLen    = 0;

        if (it->piRoleData != NULL)
        {
            pbyRoleData     = (BYTE*)it->piRoleData->GetData();
            uRoleDataLen    = it->piRoleData->GetSize();
        }

        g_pSO3GameCenter->m_GameServer.LoadRoleData(
            it->nGSIndex, it->dwPlayerID, it->bSucceed, pbyRoleData, uRoleDataLen
        );

        g_pSO3GameCenter->m_GameServer.DoSyncTongOnlineMessage(it->nGSIndex, it->dwPlayerID);

        KG_COM_RELEASE(it->piRoleData);
    }

    m_LoadRetQueue.clear();
    m_LoadRetLock.Unlock();

    return;
}

void KRoleDB::DealWithCreateRoleRespond()
{
    m_CreateResultLock.Lock();
    
    for (KCREATE_RESULT_QUEUE::iterator it = m_CreateResult.begin(); it != m_CreateResult.end(); ++it)
    {
        if (it->byCode == eCreateRoleSucceed)
        {
            g_pSO3GameCenter->m_RoleManager.AddRole(
                it->RoleListInfo.szAccount,
                it->RoleListInfo.szRoleName,
                it->RoleListInfo.dwPlayerID,
                it->RoleListInfo.BaseInfo.byLevel,
                (BYTE)it->RoleListInfo.BaseInfo.cRoleType,
                (KCAMP)it->RoleListInfo.BaseInfo.byCamp,
                it->RoleListInfo.BaseInfo.byForceID,
                it->RoleListInfo.BaseInfo.nLastSaveTime,
                it->RoleListInfo.BaseInfo.nCreateTime
            );

            g_pSO3GameCenter->m_StatDataManager.UpdataCreateOrDeleteRole(true);
        }

        g_pSO3GameCenter->m_Gateway.DoCreateRoleRespond(it->byCode, it->ulIdentity, &it->RoleListInfo);
    }
    m_CreateResult.clear();
    m_CreateResultLock.Unlock();

    return;
}

void KRoleDB::DealWithQueryRoleBaseDataRespond()
{
    m_QueryRoleBaseResultQueueLock.Lock();

    for (
        KQUERY_ROLE_BASE_INFO_RESULT_QUEUE::iterator it = m_QueryRoleBaseResultQueue.begin(); 
        it != m_QueryRoleBaseResultQueue.end(); ++it
    )
    {
        KRoleBaseInfo* pBaseInfo = NULL;

        if (it->bSucceed)
        {
            pBaseInfo = &it->BaseInfo;
        }

        g_pSO3GameCenter->m_Gateway.DoSyncRoleList(
            it->nPlayerIndex, it->nRoleIndex, it->nRoleCount, it->dwRoleID, pBaseInfo
        );
    }

    m_QueryRoleBaseResultQueue.clear();

    m_QueryRoleBaseResultQueueLock.Unlock();
    return;
}

void KRoleDB::DealWithRenameRespond()
{
    m_RenameResultQueueLock.Lock();

    for (
        KRENAME_RESULT_QUEUE::iterator it = m_RenameResultQueue.begin();
        it != m_RenameResultQueue.end(); ++it
    )
    {
        KROLE_LIST_INFO* pRoleListInfo = NULL;

        if (it->byCode == eRenameSucceed)
        {
            pRoleListInfo = &it->RoleListInfo;

            g_pSO3GameCenter->m_RoleManager.RoleRename(it->dwRoleID, pRoleListInfo->szRoleName);

            g_pSO3GameCenter->m_RoleManager.RemoveRenameRole(it->dwRoleID);
        }

        g_pSO3GameCenter->m_Gateway.DoRenameRespond(
            it->byCode, it->nPlayerIndex, it->dwRoleID, pRoleListInfo
        );
    }

    m_RenameResultQueue.clear();

    m_RenameResultQueueLock.Unlock();
    return;
}
