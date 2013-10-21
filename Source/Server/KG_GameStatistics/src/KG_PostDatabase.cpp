////////////////////////////////////////////////////////////////////////////////
//  Copyright(c) Kingsoft
//
//  FileName    : KG_PostDatabase.cpp
//  Version     : 1.0
//  Creator     : Hu Hao
//  Create Date : 2009-12-11  18:04:51
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "Stdafx.h"
#include "KG_PostDatabase.h"

KG_PostDatabase::KG_PostDatabase()
{
    memset(&m_ServerInfo, 0, sizeof(m_ServerInfo));
    m_pMySQL = NULL;
}

KG_PostDatabase::~KG_PostDatabase()
{
}

int KG_PostDatabase::Init(
    const KG_MYSQL_PARAM &crMySQLParam, const KG_SERVER_INFO &crServerInfo, void *pvContext
)
{
	int nResult  = false;
	int nRetCode = false;

    KGLOG_PROCESS_ERROR(crMySQLParam.szServer[0]);
    KGLOG_PROCESS_ERROR(crMySQLParam.szDatabase[0]);
    KGLOG_PROCESS_ERROR(crMySQLParam.szUserName[0]);
    KGLOG_PROCESS_ERROR(crMySQLParam.szPassword[0]);
    KGLOG_PROCESS_ERROR(crServerInfo.szServerName[0]);

    ASSERT(m_pMySQL == NULL);
    m_pMySQL = KG_ConnectMySQLDatabase(crMySQLParam);
    KGLOG_PROCESS_ERROR(m_pMySQL && "KG_ConnectMySQLDatabase()");

    m_ServerInfo = crServerInfo;

	nResult = true;
Exit0:
    if (!nResult)
    {
        if (m_pMySQL)
        {
            mysql_close(m_pMySQL);
            m_pMySQL = NULL;
        }
    }
	return nResult;
}

int KG_PostDatabase::UnInit(void *pvContext)
{
    KG_USE_ARGUMENT(pvContext);
    
    if (m_pMySQL)
    {
        mysql_close(m_pMySQL);
        m_pMySQL = NULL;
    }

    return true;
}

int KG_PostDatabase::InsertTongStatData(
    const char cszTableName[], const KG_POST_STAT_TONG_DATA_LIST &crTongDataList
)
{
    int nResult  = false;
    int nRetCode = false;
    char szQuery[1024];
    char *pchWritePos = NULL;
    char *pchTailPos  = NULL;
    int nDataLength = 0;
    KG_POST_STAT_TONG_DATA_LIST::const_iterator cit;

    KGLOG_PROCESS_ERROR(cszTableName);
    KGLOG_PROCESS_ERROR(cszTableName[0]);

    ASSERT(m_pMySQL);
    nRetCode = mysql_query(m_pMySQL, "begin");
    KGD_MYSQL_PROCESS_ERROR(nRetCode == 0, m_pMySQL, "begin");

    // insert into `xsj_rank_organization`
    // (`groups_name`, `groups_master`, `groups_id`, `MaxMemberCount`, `quantity `, `rank`, `server_k`, `server_name`)
    // values ('花间游', '柳随风', '1', '200', '6789', '2', '0', '洛阳城')

    for (cit = crTongDataList.begin(); cit != crTongDataList.end(); ++cit)
    {
        pchWritePos = szQuery;
        pchTailPos  = szQuery + sizeof(szQuery);

        nRetCode = snprintf(
            pchWritePos, pchTailPos - pchWritePos,
            "insert into `%s`"
            " (`groups_name`, `groups_master`, `groups_id`, `maxmembermount`, `quantity`, `rank`, `server_k`, `server_name`)"
            " values ('",
            cszTableName
        );
        KGLOG_PROCESS_ERROR(nRetCode > 0);
        KGLOG_PROCESS_ERROR(nRetCode < pchTailPos - pchWritePos);
        pchWritePos += nRetCode;

        // groups_name
        nDataLength = (int)strlen(cit->szTongName);
        KGLOG_PROCESS_ERROR(nDataLength * 2 + 1 <= pchTailPos - pchWritePos);
        nRetCode = (int)mysql_real_escape_string(
            m_pMySQL, pchWritePos, cit->szTongName, nDataLength
        );
        pchWritePos += nRetCode;

        nDataLength = (int)strlen("', '");
        KGLOG_PROCESS_ERROR(nDataLength <= pchTailPos - pchWritePos);
        memcpy(pchWritePos, "', '", nDataLength);
        pchWritePos += nDataLength;

        // groups_master
        nDataLength = (int)strlen(cit->szMasterName);
        KGLOG_PROCESS_ERROR(nDataLength * 2 + 1 <= pchTailPos - pchWritePos);
        nRetCode = (int)mysql_real_escape_string(
            m_pMySQL, pchWritePos, cit->szMasterName, nDataLength
        );
        pchWritePos += nRetCode;

        // groups_id, MaxMemberCount, quantity, rank, server_k
        nRetCode = snprintf(
            pchWritePos, pchTailPos - pchWritePos, "', '%d', '%d', '%d', '%d', '%d', '",
            cit->nTongCampID, cit->nMaxMemberCount, cit->nValue, cit->nRank, m_ServerInfo.nServerType
        );
        KGLOG_PROCESS_ERROR(nRetCode > 0);
        KGLOG_PROCESS_ERROR(nRetCode < pchTailPos - pchWritePos);
        pchWritePos += nRetCode;

        // server_name
        nDataLength = (int)strlen(m_ServerInfo.szServerName);
        KGLOG_PROCESS_ERROR(nDataLength * 2 + 1 <= pchTailPos - pchWritePos);
        nRetCode = (int)mysql_real_escape_string(
            m_pMySQL, pchWritePos, m_ServerInfo.szServerName, nDataLength
        );
        pchWritePos += nRetCode;

        nDataLength = (int)strlen("')");
        KGLOG_PROCESS_ERROR(nDataLength <= pchTailPos - pchWritePos);
        memcpy(pchWritePos, "')", nDataLength);
        pchWritePos += nDataLength;

        nRetCode = mysql_real_query(m_pMySQL, szQuery, (unsigned long)(pchWritePos - szQuery));
        KGD_MYSQL_PROCESS_ERROR(nRetCode == 0, m_pMySQL, szQuery);
    }    

	nResult = true;
Exit0:
    if (nResult)
    {
        mysql_commit(m_pMySQL);
    }
    else
    {
        mysql_rollback(m_pMySQL);
    }
	return nResult;
}

int KG_PostDatabase::InsertRoleStatData(
    const char cszTableName[], const KG_POST_STAT_ROLE_DATA_LIST &crRoleDataList
)
{
    int nResult  = false;
    int nRetCode = false;
    char szQuery[1024];
    char *pchWritePos = NULL;
    char *pchTailPos  = NULL;
    int nDataLength = 0;
    KG_POST_STAT_ROLE_DATA_LIST::const_iterator cit;

    KGLOG_PROCESS_ERROR(cszTableName);
    KGLOG_PROCESS_ERROR(cszTableName[0]);

    ASSERT(m_pMySQL);
    nRetCode = mysql_query(m_pMySQL, "begin");
    KGD_MYSQL_PROCESS_ERROR(nRetCode == 0, m_pMySQL, "begin");

    // insert into `xsj_rank_money`
    // (`nick_name`, `level`, `factions`, `groups_id`, `quantity`, `rank`, `server_k`, `server_name`)
    // values ('柳随风', '70', '1', '1', '75650', '2', '0', '洛阳城')

    for (cit = crRoleDataList.begin(); cit != crRoleDataList.end(); ++cit)
    {
        pchWritePos = szQuery;
        pchTailPos  = szQuery + sizeof(szQuery);

        nRetCode = snprintf(
            pchWritePos, pchTailPos - pchWritePos,
            "insert into `%s`"
            " (`nick_name`, `level`, `factions`, `groups_id`, `quantity`, `rank`, `server_k`, `server_name`)"
            " values ('",
            cszTableName
        );
        KGLOG_PROCESS_ERROR(nRetCode > 0);
        KGLOG_PROCESS_ERROR(nRetCode < pchTailPos - pchWritePos);
        pchWritePos += nRetCode;

        // nick_name
        nDataLength = (int)strlen(cit->szRoleName);
        KGLOG_PROCESS_ERROR(nDataLength * 2 + 1 <= pchTailPos - pchWritePos);
        nRetCode = (int)mysql_real_escape_string(
            m_pMySQL, pchWritePos, cit->szRoleName, nDataLength
        );
        pchWritePos += nRetCode;

        // level, factions, groups_id, quantity, rank, server_k
        nRetCode = snprintf(
            pchWritePos, pchTailPos - pchWritePos,
            "', '%d', '%d', '%d', '%d', '%d', '%d', '",
            cit->nRoleLevel, cit->nRoleForceID, cit->nRoleCampID, cit->nValue, cit->nRank,
            m_ServerInfo.nServerType
        );
        KGLOG_PROCESS_ERROR(nRetCode > 0);
        KGLOG_PROCESS_ERROR(nRetCode < pchTailPos - pchWritePos);
        pchWritePos += nRetCode;

        // server_name
        nDataLength = (int)strlen(m_ServerInfo.szServerName);
        KGLOG_PROCESS_ERROR(nDataLength * 2 + 1 <= pchTailPos - pchWritePos);
        nRetCode = (int)mysql_real_escape_string(
            m_pMySQL, pchWritePos, m_ServerInfo.szServerName, nDataLength
        );
        pchWritePos += nRetCode;

        nDataLength = (int)strlen("')");
        KGLOG_PROCESS_ERROR(nDataLength <= pchTailPos - pchWritePos);
        memcpy(pchWritePos, "')", nDataLength);
        pchWritePos += nDataLength;

        nRetCode = mysql_real_query(m_pMySQL, szQuery, (unsigned long)(pchWritePos - szQuery));
        KGD_MYSQL_PROCESS_ERROR(nRetCode == 0, m_pMySQL, szQuery);
    }   

	nResult = true;
Exit0:
    if (nResult)
    {
        mysql_commit(m_pMySQL);
    }
    else
    {
        mysql_rollback(m_pMySQL);
    }
	return nResult;
}