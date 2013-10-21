////////////////////////////////////////////////////////////////////////////////
//  Copyright(c) Kingsoft
//
//  FileName    : KG_PostDatabase.h
//  Version     : 1.0
//  Creator     : Hu Hao
//  Create Date : 2009-12-11  17:53:20
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _KG_POST_DATABASE_H_
#define _KG_POST_DATABASE_H_

#include <list>
#include "KG_MySQLDatabase.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
struct KG_SERVER_INFO
{
    int  nServerType;       // 0:电信   1:网通
    char szServerName[64];  // 例如 "长安城"     
};

struct KG_POST_STAT_TONG_DATA
{
    char    szTongName[64];
    char    szMasterName[64];
    int     nTongCampID;
    int     nMaxMemberCount;
    int     nValue;             // 排名数值
    int     nRank;              // 排名名次
};
typedef std::list<KG_POST_STAT_TONG_DATA> KG_POST_STAT_TONG_DATA_LIST;

struct KG_POST_STAT_ROLE_DATA
{
    char    szRoleName[64];
    int     nRoleLevel;
    int     nRoleForceID;
    int     nRoleCampID;
    int     nValue;             // 排名数值
    int     nRank;              // 排名名次
};
typedef std::list<KG_POST_STAT_ROLE_DATA> KG_POST_STAT_ROLE_DATA_LIST;

////////////////////////////////////////////////////////////////////////////////////////////////////
class KG_PostDatabase
{
public:
    KG_PostDatabase();
    ~KG_PostDatabase();

    int Init(
        const KG_MYSQL_PARAM &crMySQLParam, const KG_SERVER_INFO &crServerInfo, void *pvContext
    );
    int UnInit(void *pvContext);

    int InsertTongStatData(
        const char cszTableName[], const KG_POST_STAT_TONG_DATA_LIST &crTongDataList
    );

    int InsertRoleStatData(
        const char cszTableName[], const KG_POST_STAT_ROLE_DATA_LIST &crRoleDataList
    );
private:
    KG_SERVER_INFO  m_ServerInfo;
    MYSQL          *m_pMySQL;
};

#endif  // _KG_POST_DATABASE_H_