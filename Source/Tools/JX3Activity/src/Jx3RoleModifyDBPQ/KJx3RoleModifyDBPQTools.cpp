//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   KJx3RoleModifyDBPQTools.cpp
//  Version     :   1.0
//  Creater     :   Liuzhibiao
//  Date        :   2009-10-14
//  Comment     :   
//////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KJx3RoleModifyDBPQTools.h"
#include "KCommon.h"

KJx3RoleModifyDBPQTools::KJx3RoleModifyDBPQTools(void)
{
}

KJx3RoleModifyDBPQTools::~KJx3RoleModifyDBPQTools(void)
{
}

int KJx3RoleModifyDBPQTools::Run()
{
    int nResult = false;    
    int nRetCode = false;
    
    nRetCode = _ModifyPQData();
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = _CheckModifyResult();
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = _WriteErrorServerListToFile();
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}

int KJx3RoleModifyDBPQTools::_ModifyPQData()
{
    int nResult = false;    
    int nRetCode = false;

    int nLoadDBInitFlag = false;
    int nSaveDBInitFlag = false;

    for (int i = 0; i < (int)m_ConnectParamList.size(); i++)
    {
        if (nLoadDBInitFlag)
        {
            m_dbLoadTools.UnInit();
            nLoadDBInitFlag = false;
        }
        if (nSaveDBInitFlag)
        {
            m_dbSaveTools.UnInit();
            nSaveDBInitFlag = false;
        }

        nRetCode = m_dbLoadTools.Init(m_ConnectParamList[i].szServerIP, m_ConnectParamList[i].szUserName, m_ConnectParamList[i].szPassword, m_ConnectParamList[i].szDatabaseName);
        if (!nRetCode)
        {
            m_nDBHadError = true;
            continue;
        }
        nLoadDBInitFlag = true;

        nRetCode = m_dbSaveTools.Init(m_ConnectParamList[i].szServerIP, m_ConnectParamList[i].szUserName, m_ConnectParamList[i].szPassword, m_ConnectParamList[i].szDatabaseName);
        if (!nRetCode)
        {
            m_nDBHadError = true;
            continue;
        }
        nSaveDBInitFlag = true;

        //[BEGIN]   Modify BeginTime
        nRetCode = m_dbLoadTools.ModifyPQTableStartTime(m_ConnectParamList[i].nModifyYear, m_ConnectParamList[i].nModifyMonth, m_ConnectParamList[i].nModifyDay, &m_dbSaveTools);
        if (!nRetCode)
        {
            m_nDBHadError = true;
            continue;
        }
        m_ConnectParamList[i].nHadModify = true;
        //[END] Modify BeginTime End

        m_dbSaveTools.UnInit();
        nSaveDBInitFlag = false;

        m_dbLoadTools.UnInit();
        nLoadDBInitFlag = false;
    }

    nResult = true;
//Exit0:
    if (!nResult)
    {
        if (nSaveDBInitFlag)
        {
            m_dbSaveTools.UnInit();
            nSaveDBInitFlag = false;
        }
        if (nLoadDBInitFlag)
        {
            m_dbLoadTools.UnInit();
            nLoadDBInitFlag = false;
        }
    }
    return nResult;
}

int KJx3RoleModifyDBPQTools::_WriteErrorServerListToFile()
{
    int nResult = false;    
    int nRetCode = false;

    int        nInitTabFileFlag = false;
    int        nHadErrorList    = false;
    ITabFile*  piTabFile        = NULL;
    char       szErrorFileName[FILENAMELEN];

    int         nStrLen    = 0;
    time_t      lStartTime = 0;
    struct tm*  ptmTime    = NULL;

    lStartTime = time(NULL);
    ptmTime = localtime(&lStartTime);
    KG_PROCESS_ERROR(ptmTime);

    nStrLen = snprintf(szErrorFileName,
        sizeof(szErrorFileName),
        "%s_%d_%d_%d_%d%d%d.tab", 
        KJX3ERRORSERVERLISTFILE,
        ptmTime->tm_year + 1900, 
        ptmTime->tm_mon + 1, 
        ptmTime->tm_mday,
        ptmTime->tm_hour, 
        ptmTime->tm_min, 
        ptmTime->tm_sec
    );
    KGLOG_PROCESS_ERROR(nStrLen > 0 && nStrLen < (int)sizeof(szErrorFileName));

    if (m_ErrorList.size() != 0)
    {
        nHadErrorList = true;
    }

    if (nHadErrorList)
    {
        piTabFile = g_CreateTabFile();
        KGLOG_PROCESS_ERROR(piTabFile);
        nInitTabFileFlag = true;

        nRetCode = piTabFile->InsertNewCol("ZoneName");
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = piTabFile->InsertNewCol("ServerName");
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = piTabFile->InsertNewCol("IP");
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = piTabFile->InsertNewCol("Account");
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = piTabFile->InsertNewCol("Password");
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = piTabFile->InsertNewCol("DBName");
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = piTabFile->InsertNewCol("ModifyYear");
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = piTabFile->InsertNewCol("ModifyMonth");
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = piTabFile->InsertNewCol("ModifyDay");
        KGLOG_PROCESS_ERROR(nRetCode);

        for (int i = 0; i < (int)m_ErrorList.size(); i++)
        {
            nRetCode = piTabFile->WriteString(i + 2, "ZoneName", m_ErrorList[i].szZoneName);
            KGLOG_PROCESS_ERROR(nRetCode);

            nRetCode = piTabFile->WriteString(i + 2, "ServerName", m_ErrorList[i].szSeverName);
            KGLOG_PROCESS_ERROR(nRetCode);

            nRetCode = piTabFile->WriteString(i + 2, "IP", m_ErrorList[i].szServerIP);
            KGLOG_PROCESS_ERROR(nRetCode);

            nRetCode = piTabFile->WriteString(i + 2, "Account", m_ErrorList[i].szUserName);
            KGLOG_PROCESS_ERROR(nRetCode);

            nRetCode = piTabFile->WriteString(i + 2, "Password", m_ErrorList[i].szPassword);
            KGLOG_PROCESS_ERROR(nRetCode);

            nRetCode = piTabFile->WriteString(i + 2, "DBName", m_ErrorList[i].szDatabaseName);
            KGLOG_PROCESS_ERROR(nRetCode);

            nRetCode = piTabFile->WriteInteger(i + 2, "ModifyYear", m_ErrorList[i].nModifyYear);
            KGLOG_PROCESS_ERROR(nRetCode);

            nRetCode = piTabFile->WriteInteger(i + 2, "ModifyMonth", m_ErrorList[i].nModifyMonth);
            KGLOG_PROCESS_ERROR(nRetCode);

            nRetCode = piTabFile->WriteInteger(i + 2, "ModifyDay", m_ErrorList[i].nModifyDay);
            KGLOG_PROCESS_ERROR(nRetCode);
        }

        nRetCode = piTabFile->Save(szErrorFileName);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    nResult = true;
Exit0:
    if (nInitTabFileFlag)
    {
        KG_COM_RELEASE(piTabFile);
        nInitTabFileFlag = false;
    }
    return nResult;
}

int KJx3RoleModifyDBPQTools::_CheckModifyResult()
{
    int nResult = false;    
    int nRetCode = false;

    int nSaveDBInitFlag = false;
    int nIsOK           = false;

    //[BEGIN]   Check Modify is correct
    for (int i = 0; i < (int)m_ConnectParamList.size(); i++)
    {
        nIsOK           = false;

        if (nSaveDBInitFlag)
        {
            m_dbSaveTools.UnInit();
            nSaveDBInitFlag = false;
        }

        nRetCode = m_dbSaveTools.Init(m_ConnectParamList[i].szServerIP, m_ConnectParamList[i].szUserName, 
            m_ConnectParamList[i].szPassword, m_ConnectParamList[i].szDatabaseName
        );
        if (!nRetCode)
        {
            m_nDBHadError = true;
            m_ErrorList.push_back(m_ConnectParamList[i]);
            continue;
        }
        nSaveDBInitFlag = true;

        nRetCode = m_dbSaveTools.CheckPQBeginTime(m_ConnectParamList[i].nModifyYear, m_ConnectParamList[i].nModifyMonth, 
            m_ConnectParamList[i].nModifyDay, nIsOK
        );
        if (!nRetCode)
        {
            m_nDBHadError = true;
            KGLogPrintf(KGLOG_ERR, "Check PQ BeginTime Error in %s,%s Database", 
                m_ConnectParamList[i].szServerIP,
                m_ConnectParamList[i].szDatabaseName
            );
        }

        //m_ErrorList.push_back(m_ConnectParamList[i]);
        if (!nIsOK)
        {
            m_nDBHadError = true;

            m_ConnectParamList[i].nHadCheckOK = false;
            m_ErrorList.push_back(m_ConnectParamList[i]);
        }
        else
        {
            m_ConnectParamList[i].nHadCheckOK = true;
        }

        m_dbSaveTools.UnInit();
        nSaveDBInitFlag = false;
    }
    //[END] Check Modify is correct

    nResult = true;
//Exit0:
    if (!nResult)
    {
        if (nSaveDBInitFlag)
        {
            m_dbSaveTools.UnInit();
            nSaveDBInitFlag = false;
        }
    }
    return nResult;
}

int KJx3RoleModifyDBPQTools::Init()
{
    int nResult = false;
    int nRetCode = false;

    ITabFile*               piTabFile = NULL;
    KGD_MYSQL_CONNECT_PARAM cp;
    
    m_nDBHadError = false;

    piTabFile = g_OpenTabFile(KJX3SERVERANDSTARTTIMEFILE);
    KGLOG_PROCESS_ERROR(piTabFile);

    for (int i = 2; i <= piTabFile->GetHeight(); i++)
    {
        nRetCode = piTabFile->GetString(i, "ZoneName", "", cp.szZoneName, sizeof(cp.szZoneName));
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = piTabFile->GetString(i, "ServerName", "", cp.szSeverName, sizeof(cp.szSeverName));
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = piTabFile->GetString(i, "IP", "", cp.szServerIP, sizeof(cp.szServerIP));
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = piTabFile->GetString(i, "Account", "", cp.szUserName, sizeof(cp.szUserName));
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = piTabFile->GetString(i, "Password", "", cp.szPassword, sizeof(cp.szPassword));
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = piTabFile->GetString(i, "DBName", "", cp.szDatabaseName, sizeof(cp.szDatabaseName));
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = piTabFile->GetInteger(i, "ModifyYear", 2009, &cp.nModifyYear);
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = piTabFile->GetInteger(i, "ModifyMonth", 2009, &cp.nModifyMonth);
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = piTabFile->GetInteger(i, "ModifyDay", 2009, &cp.nModifyDay);
        KGLOG_PROCESS_ERROR(nRetCode);

        cp.nHadModify   = false;
        cp.nHadCheckOK  = false;

        m_ConnectParamList.push_back(cp);
    }

    nResult = true;
Exit0:
    KG_COM_RELEASE(piTabFile);
    return nResult;
}

void KJx3RoleModifyDBPQTools::UnInit()
{
}

int KJx3RoleModifyDBPQTools::IsDBHadError()
{
    return m_nDBHadError;
}
