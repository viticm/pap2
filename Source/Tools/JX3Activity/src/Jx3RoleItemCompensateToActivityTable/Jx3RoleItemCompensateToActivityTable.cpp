///////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
// 
// FileName :   Jx3RoleItemCompensateToActivityTable.cpp
// Creator  :   liuzhibiao
// Date     :   2009-11-23
// Comment  :   v0.5
//              2009-11-23 V0.5 Create   Jx3RoleItemCompensateToActivityTable.cpp
///////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Jx3RoleItemCompensateToActivityTable.h"
#include "KCommon.h"

int Jx3RoleItemCompensateToActivityTable::Init()
{
    int nResult = false;
    int nRetCode = false;

    int         nTabFileInit       = false;
    int         nLoadItemCount     = 0;

    ITabFile*                   piTabFile               = NULL;
    KG_SERVERINFO               tempServerInfo;
    KG_COMPENSATE_ITEM_INFO     tempCompensateItemInfo;

    m_pucData           = NULL;
    unUseDataSize       = 0;
    m_FileInitFlag      = false;

    m_pucData = new unsigned char[900000];
    KGLOG_PROCESS_ERROR(m_pucData);

    nRetCode = KGD_InitBackupBrowserEnvironment(NULL);
    KGLOG_PROCESS_ERROR(nRetCode);
    m_nInitBackupBrowserEnvironmentFlag = true;

    piTabFile = g_OpenTabFile(KG_SEVERINFOLIST_FILE);
    KGLOG_PROCESS_ERROR(piTabFile);
    nTabFileInit = true;

    for (int i = 2; i <= piTabFile->GetHeight(); i++)
    {
        nRetCode = piTabFile->GetString(i, "ZoneName", "", tempServerInfo.szZoneName, sizeof(tempServerInfo.szZoneName));
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = piTabFile->GetString(i, "ServerName", "", tempServerInfo.szServerName, sizeof(tempServerInfo.szServerName));
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = piTabFile->GetString(i, "LogServerName", "", tempServerInfo.szLogServerName, sizeof(tempServerInfo.szLogServerName));
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = piTabFile->GetString(i, "BackUpFilePath", "", tempServerInfo.szBackUpFilePath, sizeof(tempServerInfo.szBackUpFilePath));
        KGLOG_PROCESS_ERROR(nRetCode);

        m_ServerList.push_back(tempServerInfo);
    }

    if (nTabFileInit)
    {
        KG_COM_RELEASE(piTabFile);
        nTabFileInit = false;
    }

    piTabFile = g_OpenTabFile(KG_COMPENSATEITEM_FILE);
    KGLOG_PROCESS_ERROR(piTabFile);
    nTabFileInit = true;

    for (int i = 2; i <= piTabFile->GetHeight(); i++)
    {
        nRetCode = piTabFile->GetInteger(i, "ItemTabType", 0, &tempCompensateItemInfo.nTabType);
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = piTabFile->GetInteger(i, "ItemIndex", 0, &tempCompensateItemInfo.nIndex);
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = piTabFile->GetString(i, "ItemName", "", tempCompensateItemInfo.szItemName, sizeof(tempCompensateItemInfo.szItemName));
        KGLOG_PROCESS_ERROR(nRetCode);

        m_CompensateItemList.push_back(tempCompensateItemInfo);
        m_CompensateItemCountList.push_back(0);
    }

    m_pOutFile = fopen("activity.txt", "w");
    KGLOG_PROCESS_ERROR(m_pOutFile);
    m_FileInitFlag = true;

    // 判断是不是读入了16个道具
    nLoadItemCount = m_CompensateItemList.size();
    KGLOG_PROCESS_ERROR(nLoadItemCount == 16);

    nResult = true;
Exit0:
    if (!nResult)
    {
        if (m_FileInitFlag)
        {
            fclose(m_pOutFile);
            m_pOutFile = NULL;
            m_FileInitFlag = false;
        }
    }
    if (nTabFileInit)
    {
        KG_COM_RELEASE(piTabFile);
        nTabFileInit = false;
    }
    if (!nResult)
    {
        if (m_nInitBackupBrowserEnvironmentFlag)
        {
            KGD_UnInitBackupBrowserEnvironment(NULL);
            m_nInitBackupBrowserEnvironmentFlag = false;
        }
        KG_DELETE_ARRAY(m_pucData);
    }

    return nResult;
}

void Jx3RoleItemCompensateToActivityTable::UnInit()
{
    if (m_FileInitFlag)
    {
        fclose(m_pOutFile);
        m_pOutFile = NULL;
        m_FileInitFlag = false;
    }
    KG_COM_RELEASE(m_piTableBrowser);
    if (m_nInitBackupBrowserEnvironmentFlag)
    {
        KGD_UnInitBackupBrowserEnvironment(NULL);
        m_nInitBackupBrowserEnvironmentFlag = false;
    }
    KG_DELETE_ARRAY(m_pucData);
}

int Jx3RoleItemCompensateToActivityTable::GetGetInfoIndex(const char cszGetFieldName[], bool& refbHadFind, int& refnOutIndex)
{
    int nResult = false;
    int nRetCode = false;

    unsigned    uFieldCount     = 0;
    const char* pcszFieldName   = NULL;

    KGLOG_PROCESS_ERROR(cszGetFieldName[0]);
    
    refbHadFind = false;

    nRetCode = m_piTableBrowser->GetFieldCount(&uFieldCount);
    KGLOG_PROCESS_ERROR(nRetCode);

    for (unsigned i = 0; i < uFieldCount; i++)
    {
        nRetCode = m_piTableBrowser->GetFieldName(i, &pcszFieldName);
        KGLOG_PROCESS_ERROR(nRetCode);

        if (strcmp(pcszFieldName, cszGetFieldName) == 0)
        {
            refbHadFind  = true;
            refnOutIndex = i;
            break;
        }
    }

    nResult = true;
Exit0:
    return nResult;
}

int Jx3RoleItemCompensateToActivityTable::Run()
{
    int nResult  = false;
    int nRetCode = false;

    ITabFile*       piErrorTabFile          = NULL;
    int             nErrorTabFileInitFlag   = false;
    int             nErrorCount             = 0;
    //char            szBuffer[2048];
    char*           pszPos                  = NULL;
    //size_t          uLeftBufferSize         = 2048;

    char       szErrorFileName[FILENAMELEN];

    int         nStrLen         = 0;
    time_t      lStartTime      = 0;
    struct tm*  ptmTime         = NULL;
    time_t      lStopTime       = 0;
    unsigned    uRunningSeconds = 0;

    lStartTime = time(NULL);
    ptmTime = localtime(&lStartTime);
    KG_PROCESS_ERROR(ptmTime);

    nStrLen = snprintf(szErrorFileName,
        sizeof(szErrorFileName),
        "%s_%d_%d_%d_%d%d%d.tab", 
        KG_ERRORSERVERLISTFILE,
        ptmTime->tm_year + 1900, 
        ptmTime->tm_mon + 1, 
        ptmTime->tm_mday,
        ptmTime->tm_hour, 
        ptmTime->tm_min, 
        ptmTime->tm_sec
    );
    KGLOG_PROCESS_ERROR(nStrLen > 0 && nStrLen < (int)sizeof(szErrorFileName));

    nRetCode = fputs("Logserver名称\t区服中文名\t角色名\tValue0\tValue1\tValue2\tValue3\tValue4\tValue5\n", m_pOutFile);
    KGLOG_PROCESS_ERROR(nRetCode != EOF);

    //nRetCode = fputs("Logserver名称\t区服中文名\t角色名\tValue0\tValue1\tValue2\tValue3\tValue4\tValue5", m_pOutFile);
    //KGLOG_PROCESS_ERROR(nRetCode != EOF);

    //memset(szBuffer,'\0', sizeof(szBuffer));
    //pszPos = szBuffer;

    //for (int i = 0; i < m_CompensateItemList.size(); i++)
    //{
    //    nRetCode = snprintf(
    //        pszPos, uLeftBufferSize, "\t%s", m_CompensateItemList[i].szItemName
    //    );
    //    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
    //    pszPos          += nRetCode;
    //    uLeftBufferSize -= nRetCode;
    //}

    //szBuffer[sizeof(szBuffer) - 1] = '\0';
    //nRetCode = fputs(szBuffer, m_pOutFile);
    //KGLOG_PROCESS_ERROR(nRetCode != EOF);

    //nRetCode = fputs("\n", m_pOutFile);
    //KGLOG_PROCESS_ERROR(nRetCode != EOF);

    for (int i = 0; i < m_ServerList.size(); i++)
    {
        lStartTime = time(NULL);
        ptmTime = localtime(&lStartTime);
        KG_PROCESS_ERROR(ptmTime);

        KGLogPrintf(
            KGLOG_INFO, 
            "Analyze %s ::Start Time: %d-%d-%d %d:%d:%d\n",
            m_ServerList[i].szServerName,
            ptmTime->tm_year + 1900, 
            ptmTime->tm_mon + 1, 
            ptmTime->tm_mday,
            ptmTime->tm_hour, 
            ptmTime->tm_min, 
            ptmTime->tm_sec
        );

        nRetCode = Analyze(m_ServerList[i], "role");
        if (!nRetCode)
        {
            KGLogPrintf(
                KGLOG_INFO, 
                "Analyze %s Error",m_ServerList[i].szServerName
            );

            if (!nErrorTabFileInitFlag)
            {
                piErrorTabFile = g_CreateTabFile();
                KGLOG_PROCESS_ERROR(piErrorTabFile);
                nErrorTabFileInitFlag = true;

                nRetCode = piErrorTabFile->InsertNewCol("ZoneName");
                KGLOG_PROCESS_ERROR(nRetCode);

                nRetCode = piErrorTabFile->InsertNewCol("ServerName");
                KGLOG_PROCESS_ERROR(nRetCode);

                nRetCode = piErrorTabFile->InsertNewCol("LogServerName");
                KGLOG_PROCESS_ERROR(nRetCode);

                nRetCode = piErrorTabFile->InsertNewCol("BackUpFilePath");
                KGLOG_PROCESS_ERROR(nRetCode);
            }

            nRetCode = piErrorTabFile->WriteString(nErrorCount + 2, "ZoneName", m_ServerList[i].szZoneName);
            KGLOG_PROCESS_ERROR(nRetCode);

            nRetCode = piErrorTabFile->WriteString(nErrorCount + 2, "ServerName", m_ServerList[i].szServerName);
            KGLOG_PROCESS_ERROR(nRetCode);

            nRetCode = piErrorTabFile->WriteString(nErrorCount + 2, "LogServerName", m_ServerList[i].szLogServerName);
            KGLOG_PROCESS_ERROR(nRetCode);

            nRetCode = piErrorTabFile->WriteString(nErrorCount + 2, "BackUpFilePath", m_ServerList[i].szBackUpFilePath);
            KGLOG_PROCESS_ERROR(nRetCode);

            nErrorCount++;

            m_ErrorServerList.push_back(m_ServerList[i]);
        }

        lStopTime = time(NULL);
        ptmTime = localtime(&lStopTime);
        KGLOG_PROCESS_ERROR(ptmTime);

        KGLogPrintf(
            KGLOG_INFO, 
            "Analyze %s ::Stop Time: %d-%d-%d %d:%d:%d\n",
            m_ServerList[i].szServerName,
            ptmTime->tm_year + 1900, 
            ptmTime->tm_mon + 1, 
            ptmTime->tm_mday,
            ptmTime->tm_hour, 
            ptmTime->tm_min, 
            ptmTime->tm_sec
       );

       uRunningSeconds = (unsigned)(lStopTime - lStartTime);
       KGLogPrintf(
            KGLOG_INFO,
            "Analyze %s Running Time: %d days ,%d hours ,%d minutes, %d seconds.\n", 
            m_ServerList[i].szServerName,
            (uRunningSeconds / (60 * 60 * 24)),
            ((uRunningSeconds % (60 * 60 * 24)) / (60 * 60)),
            ((uRunningSeconds % (60 * 60)) / 60),
            (uRunningSeconds % 60)
       );
    }

    nResult = true;
Exit0:
    if (nErrorTabFileInitFlag)
    {
        KG_COM_RELEASE(piErrorTabFile);
        nErrorTabFileInitFlag = false;
    }
    return nResult;
}

int Jx3RoleItemCompensateToActivityTable::Analyze(const KG_SERVERINFO& refServerInfo, const char szTableName[])
{
    int nResult  = false;
    int nRetCode = false;

    char                szRoleName[MYSQL_STRING_MAX_SIZE];
    
    int                 nInfoIndex      = 0;
    bool                bHadFind        = false;
    int                 nRoleNameIndex  = 0;
    int                 nExtInfoIndex   = 0;
    KGD_FETCH_RESULT    eFetchResult;
    unsigned            uFieldDataSize  = 0;
    const void*         pcvFieldData    = NULL;
    
    m_piTableBrowser = KGD_OpenTableBackupFile(
        szTableName,
        refServerInfo.szBackUpFilePath
    );
    KGLOG_PROCESS_ERROR(m_piTableBrowser);

    nRetCode = GetGetInfoIndex("RoleName", bHadFind, nInfoIndex);
    KGLOG_PROCESS_ERROR(nRetCode);
    KGLOG_PROCESS_ERROR(bHadFind);

    nRoleNameIndex = nInfoIndex;

    nRetCode = GetGetInfoIndex("ExtInfo", bHadFind, nInfoIndex);
    KGLOG_PROCESS_ERROR(nRetCode);
    KGLOG_PROCESS_ERROR(bHadFind);

    nExtInfoIndex = nInfoIndex;

    while(true)
    {
        nRetCode = m_piTableBrowser->FetchNextRecord(&eFetchResult);
        KGLOG_PROCESS_ERROR(nRetCode);

        if (eFetchResult == KGD_FETCH_NO_MORE_RECORD)
            break;

        if (eFetchResult == KGD_FETCH_CORRUPT_RECORD)
            continue;

        ASSERT(eFetchResult == KGD_FETCH_SUCCESS);

        nRetCode = m_piTableBrowser->GetCurrentRecordField(
            nRoleNameIndex, &uFieldDataSize, &pcvFieldData
        );// 获取角色名
        KGLOG_PROCESS_ERROR(nRetCode);
        
        memset(szRoleName, '\0', sizeof(szRoleName));
        strncpy(szRoleName, (char*)pcvFieldData, uFieldDataSize);
        szRoleName[uFieldDataSize] = '\0';

        nRetCode = m_piTableBrowser->GetCurrentRecordField(
            nExtInfoIndex, &uFieldDataSize, &pcvFieldData
        );// 获取扩展信息

        memcpy(m_pucData, pcvFieldData, uFieldDataSize);
        unUseDataSize = uFieldDataSize;

        if (unUseDataSize == 0)
        {
            continue;
        }

        nRetCode = DealData(m_pucData, unUseDataSize, refServerInfo, szRoleName);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    nResult = true;
Exit0:
    KG_COM_RELEASE(m_piTableBrowser);
    return nResult;
}

// 指针对16个道具做处理
int Jx3RoleItemCompensateToActivityTable::DealData(unsigned char* pucData, unsigned int unDataSize, const KG_SERVERINFO& refServerInfo, const char cszRoleName[])
{
    int nResult = false;
    int nRetCode = false;

    PARSEHANDLE     pParseHandle    = NULL;
    WORD            wItemCount      = 0;
    char            szBuffer[CHAR_BUFFER_MAX_SIZE];
    char*           pszPos          = NULL;
    size_t          uLeftBufferSize = CHAR_BUFFER_MAX_SIZE;

    int          nHadCompensateItem     = false;
    
    BYTE        byBox               =   0;
    BYTE        byPos               =   0;
    BYTE        byVersion           =   0;
    BYTE        byTabType           =   0; 
    WORD        wTabIndex           =   0;
    BYTE        byBind              =   0;
    WORD        wDurability         =   0;
    time_t      nGenTime            =   0;
    int         nIsEqui             =   0;
    DWORD       dwRandSeed          =   0; 
    WORD        wLeftEnchantTime    =   0;
    BYTE        byColorID           =   0;
    WORD        wPermanentEnchant   =   0;
    WORD        wTemporaryEnchant   =   0;
    WORD        wMount1             =   0;
    WORD        wMount2             =   0; 
    WORD        wMount3             =   0; 
    WORD        wMount4             =   0;

    DWORD dwYu1     = 0x3ff00000;
    DWORD dwYu2     = 0x3ffffc00;
    DWORD dw_1      = 0;
    DWORD dw_2      = 0;
    DWORD dw_3      = 0;
    DWORD dwNeed1   = 0;
    DWORD dwNeed2   = 0;
    DWORD dwNeed3   = 0;
    DWORD dwNeed4   = 0;
    DWORD dwNeed5   = 0;
    DWORD dwNeed6   = 0;
    // 处理
    pParseHandle = ParseRoleExtData(pucData, unDataSize);
    KGLOG_PROCESS_ERROR(pParseHandle);

    for (int i = 0; i < m_CompensateItemCountList.size(); i++)
    {
        m_CompensateItemCountList[i] = 0;
    }

    nRetCode = GetRoleItemCount(pParseHandle, wItemCount);
    KGLOG_PROCESS_ERROR(nRetCode);

    for (DWORD dwIndex = 0; dwIndex < wItemCount; dwIndex++)
    {
        nRetCode = GetRoleItemInfo(pParseHandle, dwIndex, byBox, 
            byPos, byVersion, byTabType, 
            wTabIndex, byBind, wDurability,
            nGenTime, nIsEqui, dwRandSeed, 
            wLeftEnchantTime, byColorID,
            wPermanentEnchant, wTemporaryEnchant,
            wMount1, wMount2, wMount3, wMount4
        );
        KGLOG_PROCESS_ERROR(nRetCode);

        for (int i = 0; i < m_CompensateItemList.size(); i++)
        {
            if (byTabType == m_CompensateItemList[i].nTabType && 
                wTabIndex == m_CompensateItemList[i].nIndex)
            {
                if (wDurability >= 2)
                {
                    m_CompensateItemCountList[i] = m_CompensateItemCountList[i] + wDurability - 1;
                    nHadCompensateItem = true;
                }
            }

        }
    }

    if (nHadCompensateItem)
    {
        dw_1    = 0;
        dw_2    = 0;
        dw_3    = 0;
        dwNeed1 = 0;
        dwNeed2 = 0;
        dwNeed3 = 0;
        dwNeed4 = 0;
        dwNeed5 = 0;
        dwNeed6 = 0;

        memset(szBuffer,'\0', sizeof(szBuffer));
        pszPos = szBuffer;

        //nRetCode = snprintf(
        //    pszPos, uLeftBufferSize, "%s\t%s\t%s", refServerInfo.szLogServerName, refServerInfo.szServerName, cszRoleName
        //);
        //KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
        //pszPos          += nRetCode;
        //uLeftBufferSize -= nRetCode;

        // 把16个Item数目分6组,每组3个,3个合成一个DWORD, 最后一组(第6组只保存一个)
        dw_1    = m_CompensateItemCountList[0];
        dw_2    = m_CompensateItemCountList[1];
        dw_3    = m_CompensateItemCountList[2];

        dwNeed1 = dw_1 << 20;
        dwNeed1 = dwNeed1 & dwYu1;
        dwNeed1 = dwNeed1 | (dw_2 << 10);
        dwNeed1 = dwNeed1 & dwYu2;
        dwNeed1 = dwNeed1 | dw_3;

        dw_1    = m_CompensateItemCountList[3];
        dw_2    = m_CompensateItemCountList[4];
        dw_3    = m_CompensateItemCountList[5];

        dwNeed2 = dw_1 << 20;
        dwNeed2 = dwNeed2 & dwYu1;
        dwNeed2 = dwNeed2 | (dw_2 << 10);
        dwNeed2 = dwNeed2 & dwYu2;
        dwNeed2 = dwNeed2 | dw_3;

        dw_1    = m_CompensateItemCountList[6];
        dw_2    = m_CompensateItemCountList[7];
        dw_3    = m_CompensateItemCountList[8];

        dwNeed3 = dw_1 << 20;
        dwNeed3 = dwNeed3 & dwYu1;
        dwNeed3 = dwNeed3 | (dw_2 << 10);
        dwNeed3 = dwNeed3 & dwYu2;
        dwNeed3 = dwNeed3 | dw_3;

        dw_1    = m_CompensateItemCountList[9];
        dw_2    = m_CompensateItemCountList[10];
        dw_3    = m_CompensateItemCountList[11];

        dwNeed4 = dw_1 << 20;
        dwNeed4 = dwNeed4 & dwYu1;
        dwNeed4 = dwNeed4 | (dw_2 << 10);
        dwNeed4 = dwNeed4 & dwYu2;
        dwNeed4 = dwNeed4 | dw_3;

        dw_1    = m_CompensateItemCountList[12];
        dw_2    = m_CompensateItemCountList[13];
        dw_3    = m_CompensateItemCountList[14];

        dwNeed5 = dw_1 << 20;
        dwNeed5 = dwNeed5 & dwYu1;
        dwNeed5 = dwNeed5 | (dw_2 << 10);
        dwNeed5 = dwNeed5 & dwYu2;
        dwNeed5 = dwNeed5 | dw_3;

        dw_1    = m_CompensateItemCountList[15];

        dwNeed6 = dw_1 << 20;
        dwNeed6 = dwNeed6 & dwYu1;

        nRetCode = snprintf(
            pszPos, uLeftBufferSize, "%s\t%s\t%s\t%ld\t%ld\t%ld\t%ld\t%ld\t%ld", 
            refServerInfo.szLogServerName,
            refServerInfo.szServerName, 
            cszRoleName,
            dwNeed1,
            dwNeed2,
            dwNeed3,
            dwNeed4,
            dwNeed5,
            dwNeed6
        );
        KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
        pszPos          += nRetCode;
        uLeftBufferSize -= nRetCode;

        //for (int i = 0; i < m_CompensateItemCountList.size(); i++)
        //{
        //    nRetCode = snprintf(
        //        pszPos, uLeftBufferSize, "\t%d", m_CompensateItemCountList[i]
        //    );
        //    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftBufferSize);
        //    pszPos          += nRetCode;
        //    uLeftBufferSize -= nRetCode;
        //}

        szBuffer[sizeof(szBuffer) - 1] = '\0';
        nRetCode = fputs(szBuffer, m_pOutFile);
        KGLOG_PROCESS_ERROR(nRetCode != EOF);

        nRetCode = fputs("\n", m_pOutFile);
        KGLOG_PROCESS_ERROR(nRetCode != EOF);
    }

    nResult = true;
Exit0:
    EndRoleParse(pParseHandle);
    return nResult;
}
