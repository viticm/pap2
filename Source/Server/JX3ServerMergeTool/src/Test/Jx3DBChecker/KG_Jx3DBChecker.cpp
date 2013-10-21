//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   KG_Jx3DBChecker.cpp
//  Version     :   1.0
//  Creater     :   Liuzhibiao
//  Date        :   2009-12-8
//  Comment     :   
//////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KG_Jx3DBChecker.h"

#define REGISTER_TASK(TaskInfo, TableName, dbCheckRule)     \
    do                                                      \
    {                                                       \
        TaskInfo.nState = ts_Ready;                         \
        TaskInfo.strTableName = TableName;                  \
        TaskInfo.pDBChenkRule = &dbCheckRule;               \
    } while (false)

int KG_Jx3DBChecker::_LoadSettings()
{
    int nResult = false;
    int nRetCode = false;

    IIniFile*   piIniFile   = NULL;

    piIniFile = g_OpenIniFile(DBCHECKER_CONFIG_FILE);
    KGLOG_PROCESS_ERROR(piIniFile);

    nRetCode = piIniFile->GetString("SrcDatabaseServer1", "Server", "", m_mySqlConnectParan[0].szServer, sizeof(m_mySqlConnectParan[0].szServer));
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piIniFile->GetString("SrcDatabaseServer1", "UserName", "", m_mySqlConnectParan[0].szUserName, sizeof(m_mySqlConnectParan[0].szUserName));
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piIniFile->GetString("SrcDatabaseServer1", "Password", "", m_mySqlConnectParan[0].szPassword, sizeof(m_mySqlConnectParan[0].szPassword));
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piIniFile->GetString("SrcDatabaseServer1", "DBName", "", m_mySqlConnectParan[0].szDBName, sizeof(m_mySqlConnectParan[0].szDBName));
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piIniFile->GetString("SrcDatabaseServer2", "Server", "", m_mySqlConnectParan[1].szServer, sizeof(m_mySqlConnectParan[1].szServer));
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piIniFile->GetString("SrcDatabaseServer2", "UserName", "", m_mySqlConnectParan[1].szUserName, sizeof(m_mySqlConnectParan[1].szUserName));
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piIniFile->GetString("SrcDatabaseServer2", "Password", "", m_mySqlConnectParan[1].szPassword, sizeof(m_mySqlConnectParan[1].szPassword));
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piIniFile->GetString("SrcDatabaseServer2", "DBName", "", m_mySqlConnectParan[1].szDBName, sizeof(m_mySqlConnectParan[1].szDBName));
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piIniFile->GetString("DstDatabaseServer", "Server", "", m_mySqlConnectParan[2].szServer, sizeof(m_mySqlConnectParan[2].szServer));
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piIniFile->GetString("DstDatabaseServer", "UserName", "", m_mySqlConnectParan[2].szUserName, sizeof(m_mySqlConnectParan[2].szUserName));
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piIniFile->GetString("DstDatabaseServer", "Password", "", m_mySqlConnectParan[2].szPassword, sizeof(m_mySqlConnectParan[2].szPassword));
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piIniFile->GetString("DstDatabaseServer", "DBName", "", m_mySqlConnectParan[2].szDBName, sizeof(m_mySqlConnectParan[2].szDBName));
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    KG_COM_RELEASE(piIniFile);
    return nResult;
}

int KG_Jx3DBChecker::Init()
{
    int nResult = false;
    int nRetCode = false;
    
    KG_TASK_INFO task_info;

    m_nTaskCount = 0;

    nRetCode = _LoadSettings();
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_dbCheckRule.InitDBCheckRule();
    KGLOG_PROCESS_ERROR(nRetCode);

    REGISTER_TASK(task_info, "sequence", m_dbCheckRule); // 必须的
    m_Tasks.push_back(task_info);
    m_nTaskCount++;

    REGISTER_TASK(task_info, "antifarmer", m_dbCheckRule); 
    m_Tasks.push_back(task_info);
    m_nTaskCount++;

    REGISTER_TASK(task_info, "auction", m_dbCheckRule);
    m_Tasks.push_back(task_info);
    m_nTaskCount++;

    REGISTER_TASK(task_info, "fellowship", m_dbCheckRule);
    m_Tasks.push_back(task_info);
    m_nTaskCount++;

    REGISTER_TASK(task_info, "gamecard", m_dbCheckRule);
    m_Tasks.push_back(task_info);
    m_nTaskCount++;

    REGISTER_TASK(task_info, "globalcustomdata", m_dbCheckRule);
    m_Tasks.push_back(task_info);
    m_nTaskCount++;

    REGISTER_TASK(task_info, "globalsystemvalue", m_dbCheckRule);
    m_Tasks.push_back(task_info);
    m_nTaskCount++;

    REGISTER_TASK(task_info, "globalmailex", m_dbCheckRule);
    m_Tasks.push_back(task_info);
    m_nTaskCount++;

    REGISTER_TASK(task_info, "mailbox", m_dbCheckRule);
    m_Tasks.push_back(task_info);
    m_nTaskCount++;

    REGISTER_TASK(task_info, "mapcopy", m_dbCheckRule);
    m_Tasks.push_back(task_info);
    m_nTaskCount++;

    REGISTER_TASK(task_info, "pq", m_dbCheckRule);
    m_Tasks.push_back(task_info);
    m_nTaskCount++;

    REGISTER_TASK(task_info, "role", m_dbCheckRule);
    m_Tasks.push_back(task_info);
    m_nTaskCount++;

    REGISTER_TASK(task_info, "restorerole", m_dbCheckRule); // 处理exInfo???
    m_Tasks.push_back(task_info);
    m_nTaskCount++;

    REGISTER_TASK(task_info, "roleblacklist", m_dbCheckRule); 
    m_Tasks.push_back(task_info);
    m_nTaskCount++;

    REGISTER_TASK(task_info, "roledeletelist", m_dbCheckRule);
    m_Tasks.push_back(task_info);
    m_nTaskCount++;

    REGISTER_TASK(task_info, "statdatanameindex", m_dbCheckRule);
    m_Tasks.push_back(task_info);
    m_nTaskCount++;

    REGISTER_TASK(task_info, "statdata", m_dbCheckRule);
    m_Tasks.push_back(task_info);
    m_nTaskCount++;

    REGISTER_TASK(task_info, "tong", m_dbCheckRule);
    m_Tasks.push_back(task_info);
    m_nTaskCount++;

    REGISTER_TASK(task_info, "renamerole", m_dbCheckRule);
    m_Tasks.push_back(task_info);
    m_nTaskCount++;

    m_bRunFlag = true;

    nResult = true;
Exit0:
    return nResult;
}

void KG_Jx3DBChecker::UnInit()
{
    m_bRunFlag = false;
    for (int i = 0; i < CN_THREAD_COUNT; i++)
    {
        m_WorkThread[i].Destroy();
    }
    m_dbCheckRule.UnInitDBCheckRule();
}

//单线程run
int KG_Jx3DBChecker::Run()
{
    int nResult       = false;
    int nRetCode      = false;
    int nErrorIndex   = -1;

    int nInitDBTools0 = false;
    int nInitDBTools1 = false;
    int nInitDBTools2 = false;

    for (int i = 0; i < m_nTaskCount; i++)
    {
        if (m_Tasks[i].nState == ts_Ready)
        {
            KGLogPrintf(KGLOG_INFO, "Start Checking %s Table", m_Tasks[i].strTableName.c_str());
            nErrorIndex = i;
            m_Tasks[i].nState = ts_Running;

            nRetCode = m_Tasks[i].DBTools[0].Init(
                m_mySqlConnectParan[0].szServer, 
                m_mySqlConnectParan[0].szUserName, 
                m_mySqlConnectParan[0].szPassword, 
                m_mySqlConnectParan[0].szDBName
            );
            KGLOG_PROCESS_ERROR(nRetCode);
            nInitDBTools0 = true;

            nRetCode = m_Tasks[i].DBTools[1].Init(
                m_mySqlConnectParan[1].szServer, 
                m_mySqlConnectParan[1].szUserName,
                m_mySqlConnectParan[1].szPassword,
                m_mySqlConnectParan[1].szDBName
            );
            KGLOG_PROCESS_ERROR(nRetCode);
            nInitDBTools1 = true;

            nRetCode = m_Tasks[i].DBTools[2].Init(
                m_mySqlConnectParan[2].szServer, 
                m_mySqlConnectParan[2].szUserName,
                m_mySqlConnectParan[2].szPassword, 
                m_mySqlConnectParan[2].szDBName
            );
            KGLOG_PROCESS_ERROR(nRetCode);
            nInitDBTools2 = true;

            nRetCode = m_Tasks[i].pDBChenkRule->RuleCheck(m_Tasks[i].strTableName, &(m_Tasks[i]));
            if (!nRetCode)
            {
                KGLogPrintf(KGLOG_ERR, "Check %s Table Had Error", m_Tasks[i].strTableName.c_str());
                m_Tasks[i].nState = ts_HadErrorDone;
            }
            else
            {
                m_Tasks[i].nState = ts_HadDoneWithoutError;
            }

            if (nInitDBTools2)
            {
                m_Tasks[i].DBTools[2].UnInit();
                nInitDBTools2 = false;
            }
            if (nInitDBTools1)
            {
                m_Tasks[i].DBTools[1].UnInit();
                nInitDBTools1 = false;
            }
            if (nInitDBTools0)
            {
                m_Tasks[i].DBTools[0].UnInit();
                nInitDBTools0 = false;
            }
            KGLogPrintf(KGLOG_INFO, "End Checking %s Table", m_Tasks[i].strTableName.c_str());
        }
    }

    nErrorIndex = -1;
    nResult = true;
Exit0:
    if (nErrorIndex != -1)
    {
        KGLogPrintf(KGLOG_ERR, "Check %s Table Had Error", m_Tasks[nErrorIndex].strTableName.c_str());
        //m_Tasks[nErrorIndex].nState = ts_HadErrorDone;
        
        if (nInitDBTools2)
        {
            m_Tasks[nErrorIndex].DBTools[2].UnInit();
        }
        
        if (nInitDBTools1)
        {
            m_Tasks[nErrorIndex].DBTools[1].UnInit();
        }
        if (nInitDBTools0)
        {
            m_Tasks[nErrorIndex].DBTools[0].UnInit();
        }
    }
    return nResult;
}

//// 多线程run
//int KG_Jx3DBChecker::Run()
//{
//    int nResult = false;
//    int nRetCode = false;
//
//    for (int i = 0; i < CN_THREAD_COUNT; i++)
//    {
//        nRetCode = m_WorkThread[i].Create(WorkThreadFunction, this);
//        KGLOG_PROCESS_ERROR(nRetCode);
//    }
//
//    nResult = true;
//Exit0:
//    if (!nResult)
//    {
//        m_bRunFlag = false;
//    }
//    return nResult;
//}

int KG_Jx3DBChecker::ThreadFunction()
{
    int     nRetCode            = false;

    int nErrorIndex = -1;
    int nInitDBTools0 = false;
    int nInitDBTools1 = false;
    int nInitDBTools2 = false;
    while (m_bRunFlag)
    {
        for (int i = 0; i < m_nTaskCount; i++)
        {
            nErrorIndex = i;
            if (m_Tasks[i].nState == ts_Ready)
            {
                m_Tasks[i].nState = ts_Running;

                nRetCode = m_Tasks[i].DBTools[0].Init(
                    m_mySqlConnectParan[0].szServer, 
                    m_mySqlConnectParan[0].szUserName, 
                    m_mySqlConnectParan[0].szPassword, 
                    m_mySqlConnectParan[0].szDBName);
                if (!nRetCode)
                {
                    nErrorIndex = i;
                    break;
                }
                nInitDBTools0 = true;

                nRetCode = m_Tasks[i].DBTools[1].Init(
                    m_mySqlConnectParan[1].szServer, 
                    m_mySqlConnectParan[1].szUserName,
                    m_mySqlConnectParan[1].szPassword,
                    m_mySqlConnectParan[1].szDBName
                );
                if (!nRetCode)
                {
                    nErrorIndex = i;
                    break;
                }
                nInitDBTools1 = true;

                nRetCode = m_Tasks[i].DBTools[2].Init(
                    m_mySqlConnectParan[2].szServer, 
                    m_mySqlConnectParan[2].szUserName,
                    m_mySqlConnectParan[2].szPassword, 
                    m_mySqlConnectParan[2].szDBName
                );
                if (!nRetCode)
                {
                    nErrorIndex = i;
                    break;
                }
                nInitDBTools2 = true;

                nRetCode = m_Tasks[i].pDBChenkRule->RuleCheck(m_Tasks[i].strTableName, &(m_Tasks[i]));
                if (!nRetCode)
                {
                    KGLogPrintf(KGLOG_ERR, "Check %s Table Had Error", m_Tasks[i].strTableName.c_str());
                    m_Tasks[i].nState = ts_HadErrorDone;
                }
                else
                {
                    m_Tasks[i].nState = ts_HadDoneWithoutError;
                }
            }
        }

    }

    if (nErrorIndex != -1)
    {
        m_Tasks[nErrorIndex].nState = ts_HadDoneWithoutError;
    }

//Exit1:

    if (nInitDBTools2)
    {
        m_Tasks[nErrorIndex].DBTools[2].UnInit();
    }
    if (nInitDBTools1)
    {
        m_Tasks[nErrorIndex].DBTools[1].UnInit();
    }
    if (nInitDBTools0)
    {
        m_Tasks[nErrorIndex].DBTools[0].UnInit();
    }
   
    return 0;
}

void KG_Jx3DBChecker::WorkThreadFunction(void* pvParam)
{
    KG_Jx3DBChecker* pThis = (KG_Jx3DBChecker*)pvParam;

    assert(pThis);

    pThis->ThreadFunction();
}

