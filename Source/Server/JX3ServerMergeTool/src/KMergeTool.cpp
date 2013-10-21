#include "stdafx.h"
#include "KDBBase.h"
#include "KMergeTool.h"
#include "KFellowshipDef.h"
#include "KRoleDBDataDef.h"
#include "KRoleDef.h"
#include "KTongDef.h"
#include "KTongDB.h"
#include "KMailDef.h"
#include "Common/CRC32.h"

#ifdef WIN32
#define strtoull _strtoui64
#endif

KMergeTool::KMergeTool()
{
    m_pSrcDBHandle = NULL;
    m_pDstDBHandle = NULL;

    m_dwDstMaxRoleID = ERROR_ID;
    m_dwDstMaxTongID = ERROR_ID;
    m_dwDstMaxGlobalMailIndex = 0;
}

BOOL KMergeTool::Init()
{
    BOOL        bResult         = false;
    BOOL        bRetCode        = false;
    const char* pszLocale       = NULL;
    BOOL        bDBInitFlag     = false;

    bRetCode = m_Settings.Load();
    KGLOG_PROCESS_ERROR(bRetCode);

    pszLocale = setlocale(LC_ALL, m_Settings.m_szLocale);
    KGLOG_PROCESS_ERROR(pszLocale);

    KGLogPrintf(KGLOG_INFO, "Set locale: %s\n", pszLocale);

    m_pSrcDBHandle = MySQL_Connect(
        m_Settings.m_szSrcDBIP,
        m_Settings.m_szSrcDBName,
        m_Settings.m_nSrcDBPort,
        m_Settings.m_szSrcDBAcc, 
        m_Settings.m_szSrcDBPsw
    );
    KGLOG_PROCESS_ERROR(m_pSrcDBHandle);

    m_pDstDBHandle = MySQL_Connect(
        m_Settings.m_szDstDBIP,
        m_Settings.m_szDstDBName,
        m_Settings.m_nDstDBPort,
        m_Settings.m_szDstDBAcc, 
        m_Settings.m_szDstDBPsw
    );
    KGLOG_PROCESS_ERROR(m_pDstDBHandle);

    bRetCode = MySQL_CheckCharacterSetEqual(m_pSrcDBHandle, m_pDstDBHandle);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    if (!bResult)
    {
        if (m_pSrcDBHandle)
        {
            MySQL_Disconnect(m_pSrcDBHandle);
            m_pSrcDBHandle = NULL;
        }

        if (m_pDstDBHandle)
        {
            MySQL_Disconnect(m_pDstDBHandle);
            m_pDstDBHandle = NULL;
        }
    }
    return bResult;
}

void KMergeTool::UnInit()
{
    if (m_pSrcDBHandle)
    {
        MySQL_Disconnect(m_pSrcDBHandle);
        m_pSrcDBHandle = NULL;
    }

    if (m_pDstDBHandle)
    {
        MySQL_Disconnect(m_pDstDBHandle);
        m_pDstDBHandle = NULL;
    }
}

BOOL KMergeTool::Run()
{
    BOOL bResult  = false;
    BOOL bRetCode = false;

    bRetCode = LoadDstMaxID();
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = MergeSequence();
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = MergeActivity();
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = MergeAntiFarmer();
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = MergeAuction();
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = MergeFellowship();
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = MergeGameCard();
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = MergeMailBox();
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = MergeRenameRole();
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = MergeRole();
    KGLOG_PROCESS_ERROR(bRetCode);

    //bRetCode = MergeRestoreRole();
    //KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = MergeRoleBlackList();
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = MergeRoleDeleteList();
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = MergeTong();
    KGLOG_PROCESS_ERROR(bRetCode);

    KGLogPrintf(KGLOG_DEBUG, "Merge finish!");

    bResult = true;
Exit0:
    return bResult;
}

BOOL KMergeTool::DoSrcQuery(const char cszSQL[])
{
    BOOL        bResult             = false;
    BOOL        bRetCode            = false;

    bRetCode = MySQL_Query(m_pSrcDBHandle, cszSQL);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    if (!bResult)
    {
        MySQL_Disconnect(m_pSrcDBHandle);
        m_pSrcDBHandle = NULL;
    }
    return bResult;
}
    
BOOL KMergeTool::DoDstQuery(const char cszSQL[])
{
    BOOL        bResult             = false;
    BOOL        bRetCode            = false;

    bRetCode = MySQL_Query(m_pDstDBHandle, cszSQL);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    if (!bResult)
    {
        MySQL_Disconnect(m_pDstDBHandle);
        m_pDstDBHandle = NULL;
    }
    return bResult;
}

BOOL KMergeTool::LoadDstSequenceValue(const char cszSeqName[], DWORD* pdwRetValue)
{
    BOOL            bResult     = false;
    BOOL            bRetCode    = false;
    int             nRetCode    = 0;
    MYSQL_RES*      pQueryRes   = NULL;
    MYSQL_ROW       QueryRow    = NULL;
    my_ulonglong    uRowCount   = 0;
    MYSQL_ROW       Row;
    uint64_t        uSeqValue   = 0;
    
    assert(cszSeqName);
    assert(pdwRetValue);

    nRetCode = snprintf(
        m_szSQL,  
        sizeof(m_szSQL),
        "select %s from %s where %s = '%s'",
        SEQ_CURRENT_ID, SEQ_TABLE_NAME, SEQ_NAME,
        cszSeqName
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)sizeof(m_szSQL));

    bRetCode = DoDstQuery(m_szSQL);
    KGLOG_PROCESS_ERROR(nRetCode);

    pQueryRes = mysql_store_result(m_pDstDBHandle);
    KGLOG_PROCESS_ERROR(pQueryRes);

    uRowCount = mysql_num_rows(pQueryRes);
    KGLOG_PROCESS_ERROR(uRowCount == 0 || uRowCount == 1);
    
    if (uRowCount == 1)
    {
        Row = mysql_fetch_row(pQueryRes);
        KGLOG_PROCESS_ERROR(Row);
        
        assert(Row[0]);
        uSeqValue = (uint64_t)strtoull(Row[0], NULL, 10);
    }
    
    *pdwRetValue = (DWORD)uSeqValue;

    bResult = true;
Exit0:
    MYSQL_FREE_RESULT(pQueryRes);
    return bResult;
}

BOOL KMergeTool::LoadSrcSequenceValue(const char cszSeqName[], DWORD* pdwRetValue)
{
    BOOL            bResult     = false;
    BOOL            bRetCode    = false;
    int             nRetCode    = 0;
    MYSQL_RES*      pQueryRes   = NULL;
    MYSQL_ROW       QueryRow    = NULL;
    my_ulonglong    uRowCount   = 0;
    uint64_t        uSeqValue   = 0;
    MYSQL_ROW       Row;
    
    assert(cszSeqName);
    assert(pdwRetValue);

    nRetCode = snprintf(
        m_szSQL,  
        sizeof(m_szSQL),
        "select %s from %s where %s = '%s'",
        SEQ_CURRENT_ID, SEQ_TABLE_NAME, SEQ_NAME,
        cszSeqName
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)sizeof(m_szSQL));

    bRetCode = DoSrcQuery(m_szSQL);
    KGLOG_PROCESS_ERROR(nRetCode);

    pQueryRes = mysql_store_result(m_pSrcDBHandle);
    KGLOG_PROCESS_ERROR(pQueryRes);

    uRowCount = mysql_num_rows(pQueryRes);
    KGLOG_PROCESS_ERROR(uRowCount == 0 || uRowCount == 1);

    if (uRowCount == 1)
    {
        Row = mysql_fetch_row(pQueryRes);
        KGLOG_PROCESS_ERROR(Row);
        
        assert(Row[0]);
        uSeqValue = (uint64_t)strtoull(Row[0], NULL, 10);    
    }

    *pdwRetValue = (DWORD)uSeqValue;

    bResult = true;
Exit0:
    MYSQL_FREE_RESULT(pQueryRes);
    return bResult;
}

BOOL KMergeTool::LoadDstMaxID()
{
    BOOL bResult = false;
    BOOL bRetCode = false;
    
    bRetCode = LoadDstSequenceValue(SEQUENCE_ROLE_ID, &m_dwDstMaxRoleID);
    KGLOG_PROCESS_ERROR(bRetCode);
    if (m_dwDstMaxRoleID == 0)
    {
        KGLogPrintf(KGLOG_WARNING, "Destination DataBase role is empty.");
    }

    bRetCode = LoadDstSequenceValue(SEQUENCE_TONG_ID, &m_dwDstMaxTongID);
    KGLOG_PROCESS_ERROR(bRetCode);
    if (m_dwDstMaxTongID == 0)
    {
        KGLogPrintf(KGLOG_WARNING, "Destination DataBase tong is empty.");
    }

    bRetCode = LoadDstSequenceValue(SEQUENCE_GLOBAL_MAIL_INDEX, &m_dwDstMaxGlobalMailIndex);
    KGLOG_PROCESS_ERROR(bRetCode);
    if (m_dwDstMaxGlobalMailIndex == 0)
    {
        KGLogPrintf(KGLOG_WARNING, "Destination DataBase gloabmail is empty.");
    }

    KGLogPrintf(
        KGLOG_INFO, "Destination DataBase : MaxRoleID = %lu, MaxTongID = %lu, MaxGlobalMailIndex = %lu",
        m_dwDstMaxRoleID, m_dwDstMaxTongID, m_dwDstMaxGlobalMailIndex
    );

    bResult = true;
Exit0:
    return bResult;
}

BOOL KMergeTool::LoadDstRoleName()
{
    BOOL            bResult         = false;
    BOOL            bRetCode        = false;
    int             nRetCode        = 0;
    MYSQL_RES*      pQueryRes       = NULL;
    MYSQL_ROW       QueryRow        = NULL;
    DWORD           dwRoleID        = ERROR_ID;
    const char*     pcszRoleName    = NULL;
    KRoleBaseInfo*  pBaseInfo       = NULL;
    unsigned long*  pulFieldLen     = NULL;
    KDstDBRoleInfo  DstRoleInfo;

    KGLogPrintf(KGLOG_DEBUG, "Load Dst RoleName ... ...");

    nRetCode = snprintf(
        m_szSQL, sizeof(m_szSQL),
        "select %s, %s, %s from %s;",
        KG_ID_FIELD_NAME, KG_NAME_FIELD_NAME, KG_BASE_INFO_FIELD_NAME,
        ROLE_TABLE_NAME
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < sizeof(m_szSQL));

    bRetCode = DoDstQuery(m_szSQL);
    KGLOG_PROCESS_ERROR(bRetCode);

    pQueryRes = mysql_use_result(m_pDstDBHandle);
    KGLOG_PROCESS_ERROR(pQueryRes);

    while(true)
    {
        QueryRow = mysql_fetch_row(pQueryRes);
        if (!QueryRow)
            break;

        pulFieldLen = mysql_fetch_lengths(pQueryRes);
        KGLOG_PROCESS_ERROR(pulFieldLen);

        dwRoleID = strtoul(QueryRow[0], NULL, 10);
        KGLOG_PROCESS_ERROR(dwRoleID != ERROR_ID);

        pcszRoleName = (const char*)QueryRow[1];
        KGLOG_PROCESS_ERROR(pcszRoleName);

        KGLOG_PROCESS_ERROR(pulFieldLen[2] == sizeof(KRoleBaseInfo));
        pBaseInfo = (KRoleBaseInfo*)QueryRow[2];
        KGLOG_PROCESS_ERROR(pBaseInfo);

        DstRoleInfo.dwID = dwRoleID;
        DstRoleInfo.nLastSaveTime = pBaseInfo->nLastSaveTime;

        m_RoleNameTable.insert(make_pair(pcszRoleName, DstRoleInfo));
    }

    KGLogPrintf(KGLOG_DEBUG, "Load Dst RoleName done!");

    bResult = true;
Exit0:
    MYSQL_FREE_RESULT(pQueryRes);
    return bResult;
}

const char* KMergeTool::GetTongName(BYTE* pbyData, size_t uDataLen)
{
    const char*         pszResult       = NULL;
    BOOL                bRetCode        = false;
    int                 nRetCode        = 0;
    KTONG_DB_HEADER*    pHeader         = NULL;
    BYTE*               pbyOffSet       = pbyData;
    size_t              uLeftSize       = uDataLen;
    size_t              uSectionLen     = 0;
    int                 nTongNameCount  = 0;
    const char*         pszTongName     = NULL;

    assert(pbyData);

    KGLOG_PROCESS_ERROR(uDataLen >= sizeof(KTONG_DB_HEADER));
    pHeader = (KTONG_DB_HEADER*)pbyOffSet;

    KGLOG_PROCESS_ERROR(pHeader->nVersion <= TONG_DB_VERSION);

    pbyOffSet += sizeof(KTONG_DB_HEADER);
    uLeftSize -= sizeof(KTONG_DB_HEADER);

    while (uLeftSize)
    {
        KTONG_DB_SECTION* pSection = (KTONG_DB_SECTION*)pbyOffSet;
        pbyOffSet += sizeof(KTONG_DB_SECTION);
        uLeftSize -= sizeof(KTONG_DB_SECTION);

        uSectionLen = pSection->uDataLen;
        KGLOG_PROCESS_ERROR(uLeftSize >= uSectionLen);

        if (pSection->nType != eTongBaseInfo)
            continue;

        if (pHeader->nVersion == 1)
        {
            KTONG_DB_BASEINFO_VERSION1* pBaseInfo   = NULL;

            KGLOG_PROCESS_ERROR(uSectionLen == sizeof(KTONG_DB_BASEINFO_VERSION1));
            pBaseInfo = (KTONG_DB_BASEINFO_VERSION1*)pbyOffSet;
            pszTongName = pBaseInfo->szName;
        }
        else
        {
            KTONG_DB_BASEINFO*  pBaseInfo   = NULL;

            KGLOG_PROCESS_ERROR(uSectionLen == sizeof(KTONG_DB_BASEINFO));
            pBaseInfo = (KTONG_DB_BASEINFO*)pbyOffSet;

            pszTongName = pBaseInfo->szName;
        }
        break;
    }

    pszResult = pszTongName;
Exit0:
    return pszResult;
}

BOOL KMergeTool::LoadDstTongName()
{
    BOOL            bResult         = false;
    BOOL            bRetCode        = false;
    int             nRetCode        = 0;
    MYSQL_RES*      pQueryRes       = NULL;
    MYSQL_ROW       QueryRow        = NULL;
    int             nTongNameNum    = 0;
    unsigned long*  pulFieldLen     = NULL;
    int             nTongNameCount  = 0;
    const char*     pszTongName     = NULL;

    KGLogPrintf(KGLOG_DEBUG, "Load Dst Tong Name ... ...");

    nRetCode = snprintf(
        m_szSQL, sizeof(m_szSQL), "select Data from %s;", TONG_TABLE_NAME
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < sizeof(m_szSQL));

    bRetCode = DoDstQuery(m_szSQL);
    KGLOG_PROCESS_ERROR(bRetCode);

    pQueryRes = mysql_use_result(m_pDstDBHandle);
    KGLOG_PROCESS_ERROR(pQueryRes);

    while(true)
    {
        QueryRow = mysql_fetch_row(pQueryRes);
        if (!QueryRow)
            break;
        
        pulFieldLen = mysql_fetch_lengths(pQueryRes);
        KGLOG_PROCESS_ERROR(pulFieldLen);

        pszTongName = GetTongName((BYTE*)QueryRow[0], pulFieldLen[0]);
        KGLOG_PROCESS_ERROR(pszTongName);

        m_TongNameTable.insert(pszTongName);
    }

    KGLogPrintf(KGLOG_DEBUG, "Load Dst Tong Name done!");

    bResult = true;
Exit0:
    MYSQL_FREE_RESULT(pQueryRes);
    return bResult;
}

BOOL KMergeTool::ConvertTong(BYTE* pbyData, size_t uDataLen)
{
    BOOL                        bResult         = false;
    BOOL                        bRetCode        = false;
    int                         nRetCode        = 0;
    KTONG_DB_HEADER*            pHeader         = NULL;
    BYTE*                       pbyOffSet       = pbyData;
    size_t                      uLeftSize       = uDataLen;
    size_t                      uSectionLen     = 0;
    char                        szMergeTongName[_NAME_LEN];
    KTONG_STAT_TABLE::iterator  it;
    
    assert(pbyData);

    KGLOG_PROCESS_ERROR(uDataLen >= sizeof(KTONG_DB_HEADER));
    pHeader = (KTONG_DB_HEADER*)pbyOffSet;

    KGLOG_PROCESS_ERROR(pHeader->nVersion <= TONG_DB_VERSION);

    pbyOffSet += sizeof(KTONG_DB_HEADER);
    uLeftSize -= sizeof(KTONG_DB_HEADER);

    while (uLeftSize)
    {
        KTONG_DB_SECTION* pSection = (KTONG_DB_SECTION*)pbyOffSet;
        pbyOffSet += sizeof(KTONG_DB_SECTION);
        uLeftSize -= sizeof(KTONG_DB_SECTION);

        uSectionLen = pSection->uDataLen;
        KGLOG_PROCESS_ERROR(uLeftSize >= uSectionLen);

        switch (pSection->nType)
        {
        case eTongBaseInfo:
            {
                char* pszTongName = NULL;
                size_t uTongNameSize = 0;

                if (pHeader->nVersion == 1)
                {
                    KTONG_DB_BASEINFO_VERSION1* pBaseInfo   = NULL;

                    KGLOG_PROCESS_ERROR(uSectionLen == sizeof(KTONG_DB_BASEINFO_VERSION1));
                    pBaseInfo = (KTONG_DB_BASEINFO_VERSION1*)pbyOffSet;

                    pBaseInfo->dwMaster += m_dwDstMaxRoleID;

                    pszTongName = pBaseInfo->szName;
                    uTongNameSize = sizeof(pBaseInfo->szName);
                }
                else
                {
                    KTONG_DB_BASEINFO*  pBaseInfo   = NULL;

                    KGLOG_PROCESS_ERROR(uSectionLen == sizeof(KTONG_DB_BASEINFO));
                    pBaseInfo = (KTONG_DB_BASEINFO*)pbyOffSet;

                    pBaseInfo->dwMaster += m_dwDstMaxRoleID;

                    pszTongName = pBaseInfo->szName;
                    uTongNameSize = sizeof(pBaseInfo->szName);
                }

                it = m_TongNameTable.find(pszTongName);
                if (it != m_TongNameTable.end())
                {
                    nRetCode = snprintf(
                        szMergeTongName, sizeof(szMergeTongName), "%s@%s",
                        pszTongName, m_Settings.m_szSrcServerName
                    );
                    szMergeTongName[sizeof(szMergeTongName) - 1] = '\0';

                    strncpy(pszTongName, szMergeTongName, uTongNameSize);
                    assert(uTongNameSize == _NAME_LEN);
                    pszTongName[uTongNameSize - 1] = '\0';
                }
            }
            break;
        case eTongRoster:
            if (pHeader->nVersion == 1)
            {
                KTONG_DB_ROSTER*                pRosterSection  = NULL;
                int                             nMemberCount    = 0;
                KTONG_DB_MEMBER_DATA_VERSION1*  pMemberData     = NULL;

                KGLOG_PROCESS_ERROR(uSectionLen >= sizeof(KTONG_DB_ROSTER));

                pRosterSection = (KTONG_DB_ROSTER*)pbyOffSet;
                nMemberCount = pRosterSection->nMemberCount;

                KGLOG_PROCESS_ERROR(nMemberCount >= 0);
                KGLOG_PROCESS_ERROR(uSectionLen == sizeof(KTONG_DB_ROSTER) + sizeof(KTONG_DB_MEMBER_DATA_VERSION1) * nMemberCount);
    
                pMemberData = (KTONG_DB_MEMBER_DATA_VERSION1*)pRosterSection->byData;

                for (int i = 0; i < nMemberCount; i++)
                    pMemberData[i].dwID += m_dwDstMaxRoleID;
            }
            else
            {
                KTONG_DB_ROSTER*        pRosterSection  = NULL;
                int                     nMemberCount    = 0;
                KTONG_DB_MEMBER_DATA*   pMemberData     = NULL;

                KGLOG_PROCESS_ERROR(uSectionLen >= sizeof(KTONG_DB_ROSTER));

                pRosterSection = (KTONG_DB_ROSTER*)pbyOffSet;
                nMemberCount = pRosterSection->nMemberCount;

                KGLOG_PROCESS_ERROR(nMemberCount >= 0);
                KGLOG_PROCESS_ERROR(uSectionLen == sizeof(KTONG_DB_ROSTER) + sizeof(KTONG_DB_MEMBER_DATA) * nMemberCount);
    
                pMemberData = (KTONG_DB_MEMBER_DATA*)pRosterSection->byData;

                for (int i = 0; i < nMemberCount; i++)
                    pMemberData[i].dwID += m_dwDstMaxRoleID;
            }
            break;
        default:
            KGLOG_PROCESS_ERROR(pSection->nType > eTongDataBegin && pSection->nType < eTongDataEnd);
        }
        pbyOffSet += uSectionLen;
        uLeftSize -= uSectionLen;
    }
    bResult = true;
Exit0:
    return bResult;
}

BOOL KMergeTool::SetDstSequenceValue(const char szSequenceName[], DWORD dwSeqCount)
{
    BOOL    bResult    = false;
    BOOL    bRetCode   = false;
    int     nRetCode   = 0;

    assert(szSequenceName);
    nRetCode = snprintf(
        m_szSQL, sizeof(m_szSQL),
        " replace %s (seqname, currentid)"
        " values('%s', %lu);",
        SEQUENCE_TABLE_NAME, szSequenceName, dwSeqCount
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)sizeof(m_szSQL));

    bRetCode = DoDstQuery(m_szSQL);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KMergeTool::MergeSequence()
{
    BOOL    bResult    = false;
    BOOL    bRetCode   = false;
    int     nRetCode   = 0;
    DWORD   dwSrcMaxRoleID = ERROR_ID;
    DWORD   dwSrcMaxTongID = ERROR_ID;
    
    KGLogPrintf(KGLOG_DEBUG, "Merge Sequence data ... ...");

    bRetCode = LoadSrcSequenceValue(SEQUENCE_ROLE_ID, &dwSrcMaxRoleID);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = SetDstSequenceValue(SEQUENCE_ROLE_ID, m_dwDstMaxRoleID + dwSrcMaxRoleID);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = LoadSrcSequenceValue(SEQUENCE_TONG_ID, &dwSrcMaxTongID);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = SetDstSequenceValue(SEQUENCE_TONG_ID, m_dwDstMaxTongID + dwSrcMaxTongID);
    KGLOG_PROCESS_ERROR(bRetCode);

    KGLogPrintf(KGLOG_DEBUG, "Merge Sequence data finish !");

    bResult = true;
Exit0:
    return bResult;
}

BOOL KMergeTool::MergeActivity()
{
    BOOL            bResult         = false;
    BOOL            bRetCode        = false;
    int             nRetCode        = 0;
    MYSQL_RES*      pQueryRes       = NULL;
    MYSQL_ROW       QueryRow        = NULL;
    int             nActivityCount  = 0;

    PROGRESS_MONITER_INIT();
    KGLogPrintf(KGLOG_DEBUG, "Merge Activity data ... ...");
    
    PROGRESS_MONITER_START(ACTIVITY_TABLE_NAME);

    nRetCode = snprintf(
        m_szSQL, sizeof(m_szSQL),
        " select ID, Type, Time, Value0, Value1, Value2, Value3, Value4, Value5 from %s; ",
        ACTIVITY_TABLE_NAME
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)sizeof(m_szSQL));

    bRetCode = DoSrcQuery(m_szSQL);
    KGLOG_PROCESS_ERROR(bRetCode);

    pQueryRes = mysql_use_result(m_pSrcDBHandle);
    KGLOG_PROCESS_ERROR(pQueryRes);

    while (true)
    {
        DWORD dwRoleID = ERROR_ID;

        QueryRow = mysql_fetch_row(pQueryRes);
        if (!QueryRow)
            break;

        PROGRESS_MONITER_INCREASE();

        dwRoleID = strtoul(QueryRow[0], NULL, 10);
        KGLOG_PROCESS_ERROR(dwRoleID != ERROR_ID);

        dwRoleID += m_dwDstMaxRoleID;

        nRetCode = snprintf(
            m_szSQL, MAX_DB_SQL_SIZE,
            " insert into %s "
            "   (ID, Type, Time, Value0, Value1, Value2, Value3, Value4, Value5) "
            " values(%lu, %s, '%s', %s, %s, %s, %s, %s, %s);",
            ACTIVITY_TABLE_NAME,
            dwRoleID, QueryRow[1], QueryRow[2], 
            QueryRow[3], QueryRow[4], QueryRow[5], QueryRow[6], QueryRow[7], QueryRow[8]
        );
        KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)sizeof(m_szSQL));
        
        bRetCode = DoDstQuery(m_szSQL);
        KGLOG_PROCESS_ERROR(bRetCode);

        nActivityCount++;
    }

    PROGRESS_MONITER_FINISH();
    KGLogPrintf(KGLOG_DEBUG, "%d Activity data Merged !", nActivityCount);

    bResult = true;
Exit0:
    MYSQL_FREE_RESULT(pQueryRes);
    return bResult;
}

BOOL KMergeTool::MergeAntiFarmer()
{
    BOOL            bResult     = false;
    BOOL            bRetCode    = false;
    int             nRetCode    = 0;
    MYSQL_RES*      pQueryRes   = NULL;
    MYSQL_ROW       QueryRow    = NULL;

    PROGRESS_MONITER_INIT();
    KGLogPrintf(KGLOG_DEBUG, "Merge AntiFarmer data ... ...");
    
    PROGRESS_MONITER_START(ANTI_FARMER_TABLE_NAME);

    nRetCode = snprintf(
        m_szSQL, sizeof(m_szSQL),
        " select Account, PunishEndTime from %s; ",
        ANTI_FARMER_TABLE_NAME
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)sizeof(m_szSQL));

    bRetCode = DoSrcQuery(m_szSQL);
    KGLOG_PROCESS_ERROR(bRetCode);

    pQueryRes = mysql_use_result(m_pSrcDBHandle);
    KGLOG_PROCESS_ERROR(pQueryRes);

    while (true)
    {
        QueryRow = mysql_fetch_row(pQueryRes);
        if (!QueryRow)
            break;

        PROGRESS_MONITER_INCREASE();

        nRetCode = snprintf(
            m_szSQL, sizeof(m_szSQL),
            " replace into %s "
            " (Account, PunishEndTime) "
            " values('%s', '%s');",
            ANTI_FARMER_TABLE_NAME, QueryRow[0], QueryRow[1]
        );
        KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)sizeof(m_szSQL));

        bRetCode = DoDstQuery(m_szSQL);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

    PROGRESS_MONITER_FINISH();
    KGLogPrintf(KGLOG_DEBUG, "Merge AntiFarmer data finish !");

    bResult = true;
Exit0:
    MYSQL_FREE_RESULT(pQueryRes);
    return bResult;
}


BOOL KMergeTool::MergeAuction()
{
    BOOL            bResult     = false;
    BOOL            bRetCode    = false;
    int             nRetCode    = 0;
    MYSQL_RES*      pQueryRes   = NULL;
    MYSQL_ROW       QueryRow    = NULL;
    unsigned long*  pulFieldLen = NULL;
    char*           pszPos      = NULL;
    size_t          uLeftSize   = 0;

    PROGRESS_MONITER_INIT();
    KGLogPrintf(KGLOG_DEBUG, "Merge Auction data ... ...");
    
    PROGRESS_MONITER_START(AUCTION_TABLE_NAME);

    nRetCode = snprintf(
        m_szSQL, sizeof(m_szSQL), 
        "select SellerID, BidderID, "
        "   SellerName, Name, DurationTime, "
        "   AucGenre, AucSub, RequireLevel, Quality, "
        "   Price, BuyItNowPrice, CustodyCharges,"
        "   ItemData "
        "   from %s;",
        AUCTION_TABLE_NAME
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)sizeof(m_szSQL));

    bRetCode = DoSrcQuery(m_szSQL);
    KGLOG_PROCESS_ERROR(bRetCode);

    pQueryRes = mysql_use_result(m_pSrcDBHandle);
    KGLOG_PROCESS_ERROR(pQueryRes);

    while (true)
    {
        DWORD dwSellerID = ERROR_ID;
        DWORD dwBidderID = ERROR_ID;

        QueryRow = mysql_fetch_row(pQueryRes);
        if (!QueryRow)
            break;

        pulFieldLen = mysql_fetch_lengths(pQueryRes);
        KGLOG_PROCESS_ERROR(pulFieldLen);
        PROGRESS_MONITER_INCREASE();

        dwSellerID = strtoul(QueryRow[0], NULL, 10);
        KGLOG_PROCESS_ERROR(dwSellerID != ERROR_ID);
        
        dwBidderID = strtoul(QueryRow[1], NULL, 10);

        dwSellerID += m_dwDstMaxRoleID;

        if (dwBidderID != ERROR_ID)
            dwBidderID += m_dwDstMaxRoleID;

        pszPos = m_szSQL;
        uLeftSize = sizeof(m_szSQL);

        nRetCode = snprintf(
            pszPos, uLeftSize, 
            "insert into %s "
            "( "
            "   SellerID, BidderID, "
            "   SellerName, Name, DurationTime, "
            "   AucGenre, AucSub, RequireLevel, Quality, "
            "   Price, BuyItNowPrice, CustodyCharges, "
            "   ItemData "
            ") "
            "values("
            "   %lu, %lu, "
            "   '%s', '%s', '%s', "
            "   %s, %s, %s, %s, "
            "   %s, %s, %s, "
            "   '",
            AUCTION_TABLE_NAME,
            dwSellerID, dwBidderID,
            QueryRow[2], QueryRow[3], QueryRow[4],
            QueryRow[5], QueryRow[6], QueryRow[7], QueryRow[8],
            QueryRow[9], QueryRow[10], QueryRow[11]
        );
        KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftSize);
        pszPos      += nRetCode;
        uLeftSize   -= nRetCode;

        MYSQL_ESCAPE_BINARY_DATA(m_pDstDBHandle, QueryRow[12], pulFieldLen[12]);
        MYSQL_ESCAPE_END();

        bRetCode = DoDstQuery(m_szSQL);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

    PROGRESS_MONITER_FINISH();
    KGLogPrintf(KGLOG_DEBUG, "Merge Auction data finish !");

    bResult = true;
Exit0:
    MYSQL_FREE_RESULT(pQueryRes);
    return bResult;
}

BOOL KMergeTool::MergeFellowship()
{
    BOOL                    bResult             = false;
    BOOL                    bRetCode            = false;
    int                     nRetCode            = 0;
    MYSQL_RES*              pQueryRes           = NULL;
    MYSQL_ROW               QueryRow            = NULL;
    unsigned long*          pulFieldLen         = NULL;
    char*                   pszPos              = NULL;
    size_t                  uLeftSize           = 0;
    DWORD                   dwRoleID            = ERROR_ID;
    BYTE*                   pbyOffset           = NULL;
    BYTE*                   pbyBuffer           = NULL;
    size_t                  uDataSize           = 0;
    KG_FELLOWSHIP_DB_V1*    pFellowshipDBv1     = NULL;
    KG_FELLOWSHIP_DB_V1*    pFellowshipDataCopy = NULL;
    size_t                  uExceptedSize       = 0;
    
    PROGRESS_MONITER_INIT();
    KGLogPrintf(KGLOG_DEBUG, "Merge Fellowship data ... ...");
    
    PROGRESS_MONITER_START(FELLOWSHIP_TABLE_NAME);

    nRetCode = snprintf(
        m_szSQL, sizeof(m_szSQL), 
        "select PlayerID, FellowshipData from %s;", FELLOWSHIP_TABLE_NAME
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < sizeof(m_szSQL));

    bRetCode = DoSrcQuery(m_szSQL);
    KGLOG_PROCESS_ERROR(bRetCode);

    pQueryRes = mysql_use_result(m_pSrcDBHandle);
    KGLOG_PROCESS_ERROR(pQueryRes);

    pbyBuffer = (BYTE*)KMemory::Alloc(
        sizeof(KG_FELLOWSHIP_DB_V1) +
        sizeof(KG_FELLOWSHIP_ENTRY_V1) * KG_FELLOWSHIP_MAX_RECORD +
        sizeof(KG_FELLOWSHIP_FOE_ENTRY_V1) * KG_FELLOWSHIP_MAX_FOE_RECORD +
        sizeof(KG_FELLOWSHIP_BLACKLIST_ENTRY_V1) * KG_FELLOWSHIP_MAX_BLACKLIST_RECORD
    );
    KGLOG_PROCESS_ERROR(pbyBuffer);

    while (true)
    {
        dwRoleID = ERROR_ID;

        QueryRow = mysql_fetch_row(pQueryRes);
        if (!QueryRow)
            break;

        pulFieldLen = mysql_fetch_lengths(pQueryRes);
        KGLOG_PROCESS_ERROR(pulFieldLen);

        PROGRESS_MONITER_INCREASE();

        dwRoleID = strtoul(QueryRow[0], NULL, 10);
        KGLOG_PROCESS_ERROR(dwRoleID != ERROR_ID);

        dwRoleID += m_dwDstMaxRoleID;

        uDataSize = pulFieldLen[1];
        KGLOG_PROCESS_ERROR(uDataSize >= sizeof(KG_FELLOWSHIP_DB_V1));
        pFellowshipDBv1 = (KG_FELLOWSHIP_DB_V1*)QueryRow[1];
                
        KGLOG_PROCESS_ERROR(pFellowshipDBv1->nFriendCount <= KG_FELLOWSHIP_MAX_RECORD);
        KGLOG_PROCESS_ERROR(pFellowshipDBv1->nFoeCount <= KG_FELLOWSHIP_MAX_FOE_RECORD);
        KGLOG_PROCESS_ERROR(pFellowshipDBv1->nBlackListCount <= KG_FELLOWSHIP_MAX_BLACKLIST_RECORD);
        
        uExceptedSize  = sizeof(KG_FELLOWSHIP_DB_V1);
        uExceptedSize += pFellowshipDBv1->nFriendCount * sizeof(KG_FELLOWSHIP_ENTRY_V1);
        uExceptedSize += pFellowshipDBv1->nFoeCount * sizeof(KG_FELLOWSHIP_FOE_ENTRY_V1);
        uExceptedSize += pFellowshipDBv1->nBlackListCount * sizeof(KG_FELLOWSHIP_BLACKLIST_ENTRY_V1);
        KGLOG_PROCESS_ERROR(uDataSize == uExceptedSize);

        memcpy(pbyBuffer, QueryRow[1], uDataSize);

        pFellowshipDataCopy = (KG_FELLOWSHIP_DB_V1*)pbyBuffer;
        pbyOffset = pFellowshipDataCopy->byData;

        // Friend
        for (int i = 0; i < pFellowshipDBv1->nFriendCount; i++)
        {
            KG_FELLOWSHIP_ENTRY_V1* pFellowshipEntry = (KG_FELLOWSHIP_ENTRY_V1*)pbyOffset;

            pFellowshipEntry->dwAlliedPlayerID += m_dwDstMaxRoleID;
            pbyOffset += sizeof(KG_FELLOWSHIP_ENTRY_V1);
        }

        // Foe
        for (int i = 0; i < pFellowshipDBv1->nFoeCount; i++)
        {
            KG_FELLOWSHIP_FOE_ENTRY_V1* pFoeEntry = (KG_FELLOWSHIP_FOE_ENTRY_V1*)pbyOffset;

            pFoeEntry->dwAlliedPlayerID += m_dwDstMaxRoleID;
            pbyOffset += sizeof(KG_FELLOWSHIP_FOE_ENTRY_V1);
        }

        // BlackList
        for (int i = 0; i < pFellowshipDBv1->nBlackListCount; i++)
        {
            KG_FELLOWSHIP_BLACKLIST_ENTRY_V1*   pBlackListEntry = (KG_FELLOWSHIP_BLACKLIST_ENTRY_V1*)pbyOffset;

            pBlackListEntry->dwAlliedPlayerID += m_dwDstMaxRoleID;
            pbyOffset += sizeof(KG_FELLOWSHIP_BLACKLIST_ENTRY_V1);
        }

        pszPos = m_szSQL;
        uLeftSize = sizeof(m_szSQL);

        nRetCode = snprintf(
            pszPos, uLeftSize, 
            "insert into %s "
            "(PlayerID, FellowshipData) "
            "values(%lu, '",
            FELLOWSHIP_TABLE_NAME, dwRoleID
        );
        KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)sizeof(m_szSQL));
        pszPos      += nRetCode;
        uLeftSize   -= nRetCode;

        MYSQL_ESCAPE_BINARY_DATA(m_pDstDBHandle, (const char*)pbyBuffer, (unsigned long)uDataSize);
        MYSQL_ESCAPE_END();

        bRetCode = DoDstQuery(m_szSQL);
        KGLOG_PROCESS_ERROR(bRetCode);
    }
    
    PROGRESS_MONITER_FINISH();
    KGLogPrintf(KGLOG_DEBUG, "Merge Fellowship data finish !");
    
    bResult = true;
Exit0:
    MYSQL_FREE_RESULT(pQueryRes);
    KMEMORY_FREE(pbyBuffer);

    return bResult;
}

BOOL KMergeTool::MergeGameCard()
{
    BOOL            bResult     = false;
    BOOL            bRetCode    = false;
    int             nRetCode    = 0;
    MYSQL_RES*      pQueryRes   = NULL;
    MYSQL_ROW       QueryRow    = NULL;
    DWORD           dwSellerID  = ERROR_ID;

    PROGRESS_MONITER_INIT();
    KGLogPrintf(KGLOG_DEBUG, "Merge GameCard data ... ...");
    
    PROGRESS_MONITER_START(GAME_CARD_TABLE_NAME);

    nRetCode = snprintf(
        m_szSQL, sizeof(m_szSQL), 
        "select SellerID, Coin, GameTime, Type, Price, EndTime from %s;",
        GAME_CARD_TABLE_NAME
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)sizeof(m_szSQL));

    bRetCode = DoSrcQuery(m_szSQL);
    KGLOG_PROCESS_ERROR(bRetCode);

    pQueryRes = mysql_use_result(m_pSrcDBHandle);
    KGLOG_PROCESS_ERROR(pQueryRes);

    while (true)
    {
        QueryRow = mysql_fetch_row(pQueryRes);
        if (!QueryRow)
            break;

        PROGRESS_MONITER_INCREASE();

        dwSellerID = ERROR_ID;
        dwSellerID = strtoul(QueryRow[0], NULL, 10);
        KGLOG_PROCESS_ERROR(dwSellerID != ERROR_ID);

        dwSellerID += m_dwDstMaxRoleID;

        nRetCode = snprintf(
            m_szSQL, sizeof(m_szSQL),
            "insert into %s (SellerID, Coin, GameTime, Type, Price, EndTime) "
            "values(%lu, %s, %s, %s, %s, '%s');",
            GAME_CARD_TABLE_NAME, 
            dwSellerID, QueryRow[1], QueryRow[2], QueryRow[3], QueryRow[4], QueryRow[5]
        );
        KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)sizeof(m_szSQL));

        bRetCode = DoDstQuery(m_szSQL);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

    PROGRESS_MONITER_FINISH();
    KGLogPrintf(KGLOG_DEBUG, "Merge GameCard data finish !");

    bResult = true;
Exit0:
    MYSQL_FREE_RESULT(pQueryRes);
    return bResult;
}

BOOL KMergeTool::MergeMailBox()
{
    BOOL            bResult         = false;
    BOOL            bRetCode        = false;
    int             nRetCode        = 0;
    MYSQL_RES*      pQueryRes       = NULL;
    MYSQL_ROW       QueryRow        = NULL;
    unsigned long*  pulFieldLen     = NULL;
    DWORD           dwMailOwner     = ERROR_ID;
    char*           pszPos          = NULL;
    size_t          uLeftSize       = 0;
    BYTE*           pbyMailData     = NULL;
    size_t          uMailDataLen    = 0;
    KMailBoxDBHead* pHead           = NULL;
    KMail*          pMail           = NULL;
    BYTE*           pbyOffset       = NULL;
    size_t          uMailLeftSize   = 0;
    size_t          uMailSize       = 0;

    PROGRESS_MONITER_INIT();
    KGLogPrintf(KGLOG_DEBUG, "Merge MailBox data ... ...");
    
    PROGRESS_MONITER_START(MAIL_TABLE_NAME);

    nRetCode = snprintf(
        m_szSQL, sizeof(m_szSQL),
        "select ID, BaseTime, MailBoxInfo from %s;",
        MAIL_TABLE_NAME
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)sizeof(m_szSQL));

    bRetCode = DoSrcQuery(m_szSQL);
    KGLOG_PROCESS_ERROR(bRetCode);

    pQueryRes = mysql_use_result(m_pSrcDBHandle);
    KGLOG_PROCESS_ERROR(pQueryRes);

    while (true)
    {
        QueryRow = mysql_fetch_row(pQueryRes);
        if (!QueryRow)
            break;

        pulFieldLen = mysql_fetch_lengths(pQueryRes);
        KGLOG_PROCESS_ERROR(pulFieldLen);

        PROGRESS_MONITER_INCREASE();

        dwMailOwner = ERROR_ID;
        dwMailOwner = strtoul(QueryRow[0], NULL, 10);
        KGLOG_PROCESS_ERROR(dwMailOwner != ERROR_ID);

        dwMailOwner += m_dwDstMaxRoleID;

        uMailDataLen = pulFieldLen[2];
        pbyMailData = (BYTE*)KMemory::Alloc(uMailDataLen);
        KGLOG_PROCESS_ERROR(pbyMailData);
        memcpy(pbyMailData, QueryRow[2], uMailDataLen);

        pbyOffset = pbyMailData;
        uMailLeftSize = uMailDataLen;

        KGLOG_PROCESS_ERROR(uMailLeftSize >= sizeof(KMailBoxDBHead));
        pHead = (KMailBoxDBHead*)pbyOffset;
        pbyOffset += sizeof(KMailBoxDBHead);
        uMailLeftSize -= sizeof(KMailBoxDBHead);

        KGLOG_PROCESS_ERROR(pHead->wVersion == MAILSYS_VERSION);
        pHead->dwLastGlobalMailIndex = m_dwDstMaxGlobalMailIndex;

        while(uMailLeftSize > 0)
        {
            KGLOG_PROCESS_ERROR(uMailLeftSize >= sizeof(KMail));
            pMail = (KMail*)pbyOffset;
            uMailSize = sizeof(KMail) + pMail->wTextLen;
            for (int i = 0; i < KMAIL_MAX_ITEM_COUNT; i++)
            {
                uMailSize += pMail->ItemDesc[i].byDataLen;
            }
            KGLOG_PROCESS_ERROR(uMailLeftSize >= uMailSize);
            pbyOffset += uMailSize;
            uMailLeftSize -= uMailSize;

            if (pMail->byType != eMailType_PlayerMessage)
                continue;

            pMail->dwSenderID += m_dwDstMaxRoleID;
        }

        pszPos      = m_szSQL;
        uLeftSize   = sizeof(m_szSQL);

        nRetCode = snprintf(
            pszPos, uLeftSize,
            "insert into %s "
            "(ID, BaseTime, MailBoxInfo) "
            "values(%lu, '%s', '",
            MAIL_TABLE_NAME, dwMailOwner, QueryRow[1]
        );
        KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftSize);
        pszPos      += nRetCode;
        uLeftSize   -= nRetCode;

        MYSQL_ESCAPE_BINARY_DATA(m_pDstDBHandle, (const char*)pbyMailData, (unsigned long)uMailDataLen);
        MYSQL_ESCAPE_END();
        
        bRetCode = DoDstQuery(m_szSQL);
        KGLOG_PROCESS_ERROR(bRetCode);

        KMEMORY_FREE(pbyMailData);
    }

    PROGRESS_MONITER_FINISH();
    KGLogPrintf(KGLOG_DEBUG, "Merge MailBox data finish !");

    bResult = true;
Exit0:
    KMEMORY_FREE(pbyMailData);
    MYSQL_FREE_RESULT(pQueryRes);
    return bResult;
}

BOOL KMergeTool::MergeRenameRole()
{
    BOOL        bResult     = false;
    BOOL        bRetCode    = false;
    int         nRetCode    = 0;
    MYSQL_RES*  pQueryRes   = NULL;
    MYSQL_ROW   QueryRow    = NULL;
    DWORD       dwRoleID    = ERROR_ID;

    PROGRESS_MONITER_INIT();

    KGLogPrintf(KGLOG_DEBUG, "Merge RenameRole data ... ...");
    PROGRESS_MONITER_START(RENAME_ROLE_TABLE_NAME);

    nRetCode = snprintf(
        m_szSQL, sizeof(m_szSQL),
        "select RoleID from %s;",
        RENAME_ROLE_TABLE_NAME
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < sizeof(m_szSQL));

    bRetCode = DoSrcQuery(m_szSQL);
    KGLOG_PROCESS_ERROR(bRetCode);

    pQueryRes = mysql_use_result(m_pSrcDBHandle);
    KGLOG_PROCESS_ERROR(pQueryRes);

    while (true)
    {
        QueryRow = mysql_fetch_row(pQueryRes);
        if (!QueryRow)
            break;

        PROGRESS_MONITER_INCREASE();

        dwRoleID = ERROR_ID;
        dwRoleID = strtoul(QueryRow[0], NULL, 10);
        KGLOG_PROCESS_ERROR(dwRoleID != ERROR_ID);

        dwRoleID += m_dwDstMaxRoleID;

        nRetCode = snprintf(
            m_szSQL, sizeof(m_szSQL),
            "insert into %s (RoleID) values(%lu);",
            RENAME_ROLE_TABLE_NAME, dwRoleID
        );
        KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < sizeof(m_szSQL));

        bRetCode = DoDstQuery(m_szSQL);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

    PROGRESS_MONITER_FINISH();
    KGLogPrintf(KGLOG_DEBUG, "Merge RenameRole data finish !");

    bResult = true;
Exit0:
    MYSQL_FREE_RESULT(pQueryRes);
    return bResult;
}

static BOOL ConvertRoleExtInfoData(
    BYTE* pbyDstBuffer, size_t uDstBufferSize, size_t* puRetUsedSize, 
    BYTE* pbySrcData, size_t uSrcDataLen
)
{
    BOOL    bResult         = false;
    BOOL    bRetCode        = false;
    size_t  uSrcLeftSize    = uSrcDataLen;
    size_t  uDstLeftSize    = uDstBufferSize;
    BYTE*   pbySrcOffset    = pbySrcData;
    BYTE*   pbyDstOffset    = pbyDstBuffer;
    BYTE*   pbyBlockData    = NULL;
    size_t  uBlockSize      = 0;
    KRoleDataHeader*  pGlobalHeader = (KRoleDataHeader*)pbyDstOffset;

    assert(pbyDstBuffer);
    assert(puRetUsedSize);
    assert(pbySrcData);
    
    KGLOG_PROCESS_ERROR(uSrcLeftSize >= sizeof(KRoleDataHeader));
    KGLOG_PROCESS_ERROR(uDstLeftSize >= sizeof(KRoleDataHeader));
    
    memcpy(pbyDstOffset, pbySrcOffset, sizeof(KRoleDataHeader));

    pbySrcOffset += sizeof(KRoleDataHeader);
    uSrcLeftSize -= sizeof(KRoleDataHeader);
    
    pbyDstOffset += sizeof(KRoleDataHeader);
    uDstLeftSize -= sizeof(KRoleDataHeader);

    while (uSrcLeftSize > 0)
    {
        KRoleBlockHeader* pBlock = NULL;

        KGLOG_PROCESS_ERROR(uSrcLeftSize >= sizeof(KRoleBlockHeader));
        pBlock = (KRoleBlockHeader*)pbySrcOffset;

        uBlockSize = pBlock->dwLen + sizeof(KRoleBlockHeader);
        KGLOG_PROCESS_ERROR(uSrcLeftSize >= uBlockSize);

        if (pBlock->nType != rbtPQList)
        {
            KGLOG_PROCESS_ERROR(uDstLeftSize >= uBlockSize);
            memcpy(pbyDstOffset, pbySrcOffset, uBlockSize);

            pbyDstOffset += uBlockSize;
            uDstLeftSize -= uBlockSize;
        }

        pbySrcOffset += uBlockSize;
        uSrcLeftSize -= uBlockSize;
    }
    
    KGLOG_PROCESS_ERROR(uSrcLeftSize == 0);
    
    pGlobalHeader->dwLen = (DWORD)(uDstBufferSize - uDstLeftSize - sizeof(KRoleDataHeader));
    pGlobalHeader->dwCRC = CRC32(0, pbyDstBuffer + sizeof(KRoleDataHeader), pGlobalHeader->dwLen);

    *puRetUsedSize = uDstBufferSize - uDstLeftSize;

    bResult = true;
Exit0:
    return bResult;
}

BOOL KMergeTool::MergeRole()
{
    BOOL                        bResult             = false;
    BOOL                        bRetCode            = false;
    int                         nRetCode            = 0;
    MYSQL_RES*                  pQueryRes           = NULL;
    MYSQL_ROW                   QueryRow            = NULL;
    unsigned long*              pulFieldLen         = NULL;
    char*                       pszPos              = NULL;
    size_t                      uLeftSize           = 0;
    DWORD                       dwRoleID            = ERROR_ID;
    DWORD                       dwDstTargetID       = ERROR_ID;
    int                         nRoleNameNum        = 0;
    size_t                      uExtDataLen         = 0;
    BYTE*                       pbyExtInfoBuffer    = NULL;
    size_t                      uDataUsedSize       = 0;
    const char*                 pcszRoleName        = NULL;
    KRoleBaseInfo*              pBaseInfo           = NULL;
    const char*                 pcszSrcRoleName     = NULL;
    char                        szMergeRoleName[_NAME_LEN];
    time_t                      nSrcRoleLastAliveTime   = 0;
    time_t                      nDstRoleLastAliveTime   = 0;
    KROLE_STAT_TABLE::iterator  it;

    PROGRESS_MONITER_INIT();
    bRetCode = LoadDstRoleName();
    KGLOG_PROCESS_ERROR(bRetCode);

    KGLogPrintf(KGLOG_DEBUG, "Merge Role data ... ...");

    PROGRESS_MONITER_START(ROLE_TABLE_NAME);

    nRetCode = snprintf(
        m_szSQL, sizeof(m_szSQL),
        "select %s, %s, %s, %s, %s, %s from %s;",
        KG_ID_FIELD_NAME, KG_NAME_FIELD_NAME, 
        KG_ACCOUNT_FIELD_NAME, KG_LAST_MODIFY_FIELD_NAME,
        KG_BASE_INFO_FIELD_NAME, KG_EXT_INFO_FIELD_NAME, 
        ROLE_TABLE_NAME
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < sizeof(m_szSQL));

    bRetCode = DoSrcQuery(m_szSQL);
    KGLOG_PROCESS_ERROR(bRetCode);

    pQueryRes = mysql_use_result(m_pSrcDBHandle);
    KGLOG_PROCESS_ERROR(pQueryRes);

    while (true)
    {
        DWORD dwRenameRoleID = ERROR_ID;

        QueryRow = mysql_fetch_row(pQueryRes);
        if (!QueryRow)
            break;

        pulFieldLen = mysql_fetch_lengths(pQueryRes);
        KGLOG_PROCESS_ERROR(pulFieldLen);

        PROGRESS_MONITER_INCREASE();

        dwRoleID = strtoul(QueryRow[0], NULL, 10);
        KGLOG_PROCESS_ERROR(dwRoleID != ERROR_ID);

        dwRoleID += m_dwDstMaxRoleID;

        pcszRoleName = (const char*)QueryRow[1];
        KGLOG_PROCESS_ERROR(pcszRoleName);

        pBaseInfo = (KRoleBaseInfo*)QueryRow[4];
        KGLOG_PROCESS_ERROR(pBaseInfo);

        it = m_RoleNameTable.find(pcszRoleName);

        if (it != m_RoleNameTable.end())
        {
            nDstRoleLastAliveTime = it->second.nLastSaveTime;
            nSrcRoleLastAliveTime = pBaseInfo->nLastSaveTime;
        
            if (nDstRoleLastAliveTime < nSrcRoleLastAliveTime)
            {
                dwDstTargetID = it->second.dwID;
                dwRenameRoleID = dwDstTargetID; // for insert into rename role table
                pcszSrcRoleName = pcszRoleName; // Src角色名保持不变

                // update dst db rolename -> 'rolename@servername'
                nRetCode = snprintf(szMergeRoleName, sizeof(szMergeRoleName), "%s@%s",
                    pcszRoleName, m_Settings.m_szDstServerName
                );
                KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < sizeof(szMergeRoleName));

                // 修改Dst中的角色名
                nRetCode = snprintf(
                    m_szSQL, sizeof(m_szSQL),
                    "update %s set %s = '%s' where %s = %lu;",
                    ROLE_TABLE_NAME, KG_NAME_FIELD_NAME, szMergeRoleName,
                    KG_ID_FIELD_NAME, dwDstTargetID
                );
                KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < sizeof(m_szSQL));

                it = m_RoleNameTable.find(szMergeRoleName);
                if (it != m_RoleNameTable.end())
                {
                    KGLogPrintf(KGLOG_WARNING, "%s is exists in Destination DataBase(%s)", szMergeRoleName, m_Settings.m_szDstServerName);
                    continue;
                }

                bRetCode = DoDstQuery(m_szSQL);
                KGLOG_PROCESS_ERROR(bRetCode);
            }
            else
            {
                dwRenameRoleID = dwRoleID; // for insert into rename role table
                nRetCode = snprintf(szMergeRoleName, sizeof(szMergeRoleName), "%s@%s",
                    pcszRoleName, m_Settings.m_szSrcServerName
                );
                KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < sizeof(szMergeRoleName));

                it = m_RoleNameTable.find(szMergeRoleName);
                if (it != m_RoleNameTable.end())
                {
                    KGLogPrintf(KGLOG_WARNING, "%s is exists in Source DataBase(%s)", szMergeRoleName, m_Settings.m_szSrcServerName);
                    continue;
                }

                pcszSrcRoleName = szMergeRoleName;  // Src角色名加"@服务器"后缀
            }
        }
        else
        {
            pcszSrcRoleName = pcszRoleName;  // Src角色名保持不变
        }

        assert(pcszSrcRoleName);

        pszPos = m_szSQL;
        uLeftSize = sizeof(m_szSQL);

        // 修改Src中的角色名，插入Dst中
        nRetCode = snprintf(
            pszPos, uLeftSize,
            "insert into %s "
            "(%s, %s, %s, %s, %s, %s) "
            "values(%lu, '%s', '%s', '%s', '",
            ROLE_TABLE_NAME,
            KG_ID_FIELD_NAME, KG_NAME_FIELD_NAME, 
            KG_ACCOUNT_FIELD_NAME, KG_LAST_MODIFY_FIELD_NAME,
            KG_BASE_INFO_FIELD_NAME, KG_EXT_INFO_FIELD_NAME, 
            dwRoleID, pcszSrcRoleName,  
            QueryRow[2], QueryRow[3]
        );
        KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftSize);
        pszPos      += nRetCode;
        uLeftSize   -= nRetCode;

        MYSQL_ESCAPE_BINARY_DATA(m_pDstDBHandle, QueryRow[4], pulFieldLen[4]);

        if (QueryRow[5])
        {
            MYSQL_LINK_ESCAPE_FILED();
            uExtDataLen = pulFieldLen[5];
            pbyExtInfoBuffer = (BYTE*)KMemory::Alloc(uExtDataLen);
            KGLOG_PROCESS_ERROR(pbyExtInfoBuffer);

            bRetCode = ConvertRoleExtInfoData(
                pbyExtInfoBuffer, uExtDataLen, &uDataUsedSize, (BYTE*)QueryRow[5], uExtDataLen
            );
            KGLOG_PROCESS_ERROR(bRetCode);

            MYSQL_ESCAPE_BINARY_DATA(m_pDstDBHandle, (const char*)pbyExtInfoBuffer, (unsigned long)uDataUsedSize);
            MYSQL_ESCAPE_END();
        }
        else
        {
            nRetCode = snprintf(pszPos, uLeftSize, "',null);");
            KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftSize);
            pszPos      += nRetCode;
            uLeftSize   -= nRetCode;
        }

        bRetCode = DoDstQuery(m_szSQL);
        KGLOG_PROCESS_ERROR(bRetCode);

        KMEMORY_FREE(pbyExtInfoBuffer);

        if (dwRenameRoleID != ERROR_ID)
        {
            // insert into rename role table
            nRetCode = snprintf(
                m_szSQL, sizeof(m_szSQL),
                "insert ignore into %s (RoleID) values(%lu);",
                RENAME_ROLE_TABLE_NAME, dwRenameRoleID
            );
            KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < sizeof(m_szSQL));

            bRetCode = DoDstQuery(m_szSQL);
            KGLOG_PROCESS_ERROR(bRetCode);
        }
    }

    PROGRESS_MONITER_FINISH();
    KGLogPrintf(KGLOG_DEBUG, "Merge Role data finish !");

    bResult = true;
Exit0:
    KMEMORY_FREE(pbyExtInfoBuffer);
    MYSQL_FREE_RESULT(pQueryRes);
    return bResult;
}

BOOL KMergeTool::MergeRestoreRole()
{
    BOOL            bResult             = false;
    BOOL            bRetCode            = false;
    int             nRetCode            = 0;
    MYSQL_RES*      pQueryRes           = NULL;
    MYSQL_ROW       QueryRow            = NULL;
    unsigned long*  pulFieldLen         = NULL;
    char*           pszPos              = NULL;
    size_t          uLeftSize           = 0;
    int             nRoleNameNum        = 0;
    size_t          uExtDataLen         = 0;
    BYTE*           pbyExtInfoBuffer    = NULL;
    size_t          uDataUsedSize       = 0;

    PROGRESS_MONITER_INIT();
    KGLogPrintf(KGLOG_DEBUG, "Merge RestoreRole data ... ...");
    
    PROGRESS_MONITER_START(RESTORE_ROLE_TABLE_NAME);

    nRetCode = snprintf(m_szSQL, sizeof(m_szSQL),
        "select %s, %s, %s, %s, %s from %s;",
        KG_NAME_FIELD_NAME, KG_ACCOUNT_FIELD_NAME, KG_DELETE_TIME_FIELD_NAME,
        KG_BASE_INFO_FIELD_NAME, KG_EXT_INFO_FIELD_NAME, 
        RESTORE_ROLE_TABLE_NAME
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < sizeof(m_szSQL));

    bRetCode = DoSrcQuery(m_szSQL);
    KGLOG_PROCESS_ERROR(bRetCode);

    pQueryRes = mysql_use_result(m_pSrcDBHandle);
    KGLOG_PROCESS_ERROR(pQueryRes);

    while (true)
    {
        QueryRow = mysql_fetch_row(pQueryRes);
        if (!QueryRow)
            break;
        
        pulFieldLen = mysql_fetch_lengths(pQueryRes);
        KGLOG_PROCESS_ERROR(pulFieldLen);

        PROGRESS_MONITER_INCREASE();
        
        pszPos = m_szSQL;
        uLeftSize = sizeof(m_szSQL);

        nRetCode = snprintf(pszPos, uLeftSize,
            "insert into %s "
            "(%s, %s, %s, %s, %s) "
            "values('%s', '%s', '%s', '",
            RESTORE_ROLE_TABLE_NAME,
            KG_NAME_FIELD_NAME, KG_ACCOUNT_FIELD_NAME, KG_DELETE_TIME_FIELD_NAME,
            KG_BASE_INFO_FIELD_NAME, KG_EXT_INFO_FIELD_NAME, 
            QueryRow[0], QueryRow[1], QueryRow[2]
        );
        KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftSize);
        pszPos      += nRetCode;
        uLeftSize   -= nRetCode;

        MYSQL_ESCAPE_BINARY_DATA(m_pDstDBHandle, QueryRow[3], pulFieldLen[3]);

        if (QueryRow[4])
        {
            MYSQL_LINK_ESCAPE_FILED();
            uExtDataLen = pulFieldLen[4];
            pbyExtInfoBuffer = (BYTE*)KMemory::Alloc(uExtDataLen);
            KGLOG_PROCESS_ERROR(pbyExtInfoBuffer);

            bRetCode = ConvertRoleExtInfoData(
                pbyExtInfoBuffer, uExtDataLen, &uDataUsedSize, (BYTE*)QueryRow[4], uExtDataLen
            );
            KGLOG_PROCESS_ERROR(bRetCode);
       
            MYSQL_ESCAPE_BINARY_DATA(m_pDstDBHandle, (const char*)pbyExtInfoBuffer, (unsigned long)uDataUsedSize);
            MYSQL_ESCAPE_END();
        }
        else
        {
            nRetCode = snprintf(pszPos, uLeftSize, "',null);");
            KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftSize);
            pszPos      += nRetCode;
            uLeftSize   -= nRetCode;
        }

        bRetCode = DoDstQuery(m_szSQL);
        KGLOG_PROCESS_ERROR(bRetCode);

        KMEMORY_FREE(pbyExtInfoBuffer);
    }

    PROGRESS_MONITER_FINISH();
    KGLogPrintf(KGLOG_DEBUG, "Merge RestoreRole data finish !");

    bResult = true;
Exit0:
    KMEMORY_FREE(pbyExtInfoBuffer);
    MYSQL_FREE_RESULT(pQueryRes);
    return bResult;
}

BOOL KMergeTool::MergeRoleBlackList()
{
    BOOL            bResult     = false;
    BOOL            bRetCode    = false;
    int             nRetCode    = 0;
    MYSQL_RES*      pQueryRes   = NULL;
    MYSQL_ROW       QueryRow    = NULL;
    DWORD           dwRoleID    = ERROR_ID;

    PROGRESS_MONITER_INIT();
    KGLogPrintf(KGLOG_DEBUG, "Merge RoleBlackList data ... ...");
    
    PROGRESS_MONITER_START(ROLE_BLACK_LIST_TABLE_NAME);

    nRetCode = snprintf(
        m_szSQL, sizeof(m_szSQL),
        "select ID, FreezeTime from %s;", ROLE_BLACK_LIST_TABLE_NAME
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode <  sizeof(m_szSQL));

    bRetCode = DoSrcQuery(m_szSQL);
    KGLOG_PROCESS_ERROR(bRetCode);

    pQueryRes = mysql_use_result(m_pSrcDBHandle);
    KGLOG_PROCESS_ERROR(pQueryRes);

    while (true)
    {
        QueryRow = mysql_fetch_row(pQueryRes);
        if (!QueryRow)
            break;

        PROGRESS_MONITER_INCREASE();
        
        dwRoleID = strtoul(QueryRow[0], NULL, 10);
        KGLOG_PROCESS_ERROR(dwRoleID != ERROR_ID);

        dwRoleID += m_dwDstMaxRoleID;

        nRetCode = snprintf(
            m_szSQL, sizeof(m_szSQL),
            "insert into %s (ID, FreezeTime) "  
            "values(%lu, '%s');",
            ROLE_BLACK_LIST_TABLE_NAME,
            dwRoleID, QueryRow[1]
        );
        KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < sizeof(m_szSQL));

        bRetCode = DoDstQuery(m_szSQL);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

    PROGRESS_MONITER_FINISH();
    KGLogPrintf(KGLOG_DEBUG, "Merge RoleBlackList data finish !");

    bResult = true;
Exit0:
    MYSQL_FREE_RESULT(pQueryRes);
    return bResult;
}

BOOL KMergeTool::MergeRoleDeleteList()
{
    BOOL            bResult     = false;
    BOOL            bRetCode    = false;
    int             nRetCode    = 0;
    MYSQL_RES*      pQueryRes   = NULL;
    MYSQL_ROW       QueryRow    = NULL;
    DWORD           dwRoleID    = ERROR_ID;

    PROGRESS_MONITER_INIT();
    KGLogPrintf(KGLOG_DEBUG, "Merge RoleDeleteList data ... ...");
    
    PROGRESS_MONITER_START(ROLE_DELETE_LIST_TABLE_NAME);

    nRetCode = snprintf(
        m_szSQL, sizeof(m_szSQL),
        "select ID, EndTime from %s;", ROLE_DELETE_LIST_TABLE_NAME
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode <  sizeof(m_szSQL));

    bRetCode = DoSrcQuery(m_szSQL);
    KGLOG_PROCESS_ERROR(bRetCode);

    pQueryRes = mysql_use_result(m_pSrcDBHandle);
    KGLOG_PROCESS_ERROR(pQueryRes);

    while (true)
    {
        QueryRow = mysql_fetch_row(pQueryRes);
        if (!QueryRow)
            break;

        PROGRESS_MONITER_INCREASE();
        
        dwRoleID = strtoul(QueryRow[0], NULL, 10);
        KGLOG_PROCESS_ERROR(dwRoleID != ERROR_ID);

        dwRoleID += m_dwDstMaxRoleID;

        nRetCode = snprintf(
            m_szSQL, sizeof(m_szSQL),
            "insert into %s (ID, EndTime) "  
            "values(%lu, '%s');",
            ROLE_DELETE_LIST_TABLE_NAME,
            dwRoleID, QueryRow[1]
        );
        KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < sizeof(m_szSQL));

        bRetCode = DoDstQuery(m_szSQL);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

    PROGRESS_MONITER_FINISH();
    KGLogPrintf(KGLOG_DEBUG, "Merge RoleDeleteList data finish !");

    bResult = true;
Exit0:
    MYSQL_FREE_RESULT(pQueryRes);
    return bResult;
}

BOOL KMergeTool::MergeTong()
{
    BOOL            bResult     = false;
    BOOL            bRetCode    = false;
    int             nRetCode    = 0;
    MYSQL_RES*      pQueryRes   = NULL;
    MYSQL_ROW       QueryRow    = NULL;
    unsigned long*  pulFieldLen = NULL;
    char*           pszPos      = NULL;
    size_t          uLeftSize   = 0;
    DWORD           dwTongID    = 0;
    size_t          uDataLen    = 0;
    BYTE*           pbyDataBuffer = 0;

    PROGRESS_MONITER_INIT();
    KGLogPrintf(KGLOG_DEBUG, "Merge Tong data ... ...");

    bRetCode = LoadDstTongName();
    KGLOG_PROCESS_ERROR(bRetCode);

    PROGRESS_MONITER_START(TONG_TABLE_NAME);

    nRetCode = snprintf(
        m_szSQL, sizeof(m_szSQL), 
        "select ID, Data from %s;", TONG_TABLE_NAME    
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < sizeof(m_szSQL));

    bRetCode = DoSrcQuery(m_szSQL);
    KGLOG_PROCESS_ERROR(bRetCode);

    pQueryRes = mysql_use_result(m_pSrcDBHandle);
    KGLOG_PROCESS_ERROR(pQueryRes);

    while (true)
    {
        QueryRow = mysql_fetch_row(pQueryRes);
        if (!QueryRow)
            break;

        pulFieldLen = mysql_fetch_lengths(pQueryRes);
        KGLOG_PROCESS_ERROR(pulFieldLen);

        PROGRESS_MONITER_INCREASE();

        dwTongID = strtoul(QueryRow[0], NULL, 10);
        KGLOG_PROCESS_ERROR(dwTongID != ERROR_ID);

        dwTongID += m_dwDstMaxTongID;
        uDataLen = pulFieldLen[1];
        pbyDataBuffer = (BYTE*)KMemory::Alloc(uDataLen);
        KGLOG_PROCESS_ERROR(pbyDataBuffer);

        memcpy(pbyDataBuffer, QueryRow[1], uDataLen);

        bRetCode = ConvertTong(pbyDataBuffer, uDataLen);
        KGLOG_PROCESS_ERROR(bRetCode);

        pszPos = m_szSQL;
        uLeftSize = sizeof(m_szSQL);

        nRetCode = snprintf(
            pszPos, uLeftSize,
            "insert into %s (ID, Data) values(%lu, '",
            TONG_TABLE_NAME, dwTongID
        );
        KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftSize);
        pszPos      += nRetCode;
        uLeftSize   -= nRetCode;

        MYSQL_ESCAPE_BINARY_DATA(m_pDstDBHandle, (const char*)pbyDataBuffer, (unsigned long)uDataLen);
        MYSQL_ESCAPE_END();

        bRetCode = DoDstQuery(m_szSQL);
        KGLOG_PROCESS_ERROR(bRetCode);

        KMEMORY_FREE(pbyDataBuffer);
    }

    PROGRESS_MONITER_FINISH();
    KGLogPrintf(KGLOG_DEBUG, "Merge Tong data finish !");
    
    bResult = true;
Exit0:
    KMEMORY_FREE(pbyDataBuffer);
    MYSQL_FREE_RESULT(pQueryRes);
    return bResult;
}
