///////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
// 
// FileName :   Jx3RoleItemCompensateToActivityTable.h
// Creator  :   liuzhibiao
// Date     :   2009-11-23
// Comment  :   v0.5
//              2009-11-23 V0.5 Create   Jx3RoleItemCompensateToActivityTable.h
///////////////////////////////////////////////////////////////

#ifndef KG_ROLEDATAEXPORT2LUA
#define KG_ROLEDATAEXPORT2LUA

#include "KGD_BackupBrowser.h"
#include <string>
#include <vector>

using namespace std;

const int  MAX_FIELD_LEN                = 256;
const int  CHAR_BUFFER_MAX_SIZE         = 2048;
const int  MYSQL_STRING_MAX_SIZE        = 64;
const int  FILENAMELEN                  = 256;
//const char KG_DBROLE2FILE_CONFIG_FILE[]   = "Jx3RoleItemCompensateToActivityTable.ini";
const char KG_SEVERINFOLIST_FILE[]      = "ServerList.tab";
const char KG_COMPENSATEITEM_FILE[]     = "CompensateItemList.tab";
const char KG_ERRORSERVERLISTFILE[]     = "ErrorServerList";

struct KGD_MYSQL_CONNECT_PARAM
{
    char szServer[MYSQL_STRING_MAX_SIZE];
    char szUserName[MYSQL_STRING_MAX_SIZE];
    char szPassword[MYSQL_STRING_MAX_SIZE];
};

struct KG_DBROLEFILE2LUATABLE_PARAM
{
    KGD_MYSQL_CONNECT_PARAM ConnectParam;
    char        szDatabaseName[MYSQL_STRING_MAX_SIZE];
    char        szBaseFolderName[PATH_MAX];             // Incremental backup parameter.
    char        szImportFilePath[PATH_MAX];
};

struct KG_COMPENSATE_ITEM_INFO
{
    int     nTabType;
    int     nIndex;
    char    szItemName[MYSQL_STRING_MAX_SIZE];
};

struct KG_SERVERINFO
{
    char szZoneName[MYSQL_STRING_MAX_SIZE];
    char szServerName[MYSQL_STRING_MAX_SIZE];
    char szLogServerName[MYSQL_STRING_MAX_SIZE];
    char szBackUpFilePath[PATH_MAX];
};

#define  MAX_ROLEEXTINFO_SIZE 90000
#define  MAX_ROLEBASE_SIZE 1024


class Jx3RoleItemCompensateToActivityTable
{
public:
    int Init();
    void UnInit();

public:
    int GetGetInfoIndex(const char cszGetFieldName[], bool& refbHadFind, int& refnOutIndex);
    //int SetGetInfo(const char szGetFieldName[], const char szSearchFieldName[], const char szSearchKey[]);
    int Run();
    int Analyze(const KG_SERVERINFO& refServerInfo, const char szTableName[]);
    int DealData(unsigned char* pucData, unsigned int unDataSize, const KG_SERVERINFO& refServerInfo ,const char cszRoleName[]);

private:
    int                m_nInitBackupBrowserEnvironmentFlag;
    IKGD_TableBrowser* m_piTableBrowser;
    
    //char         m_szGetFieldName[MAX_FIELD_LEN];
    //int          m_nGetFieldNameIndex;
    //char         m_szSearchFieldName[MAX_FIELD_LEN];
    //int          m_nSearchFieldNameIndex;
    FILE*        m_pOutFile;
    int          m_FileInitFlag;

private:
    KG_DBROLEFILE2LUATABLE_PARAM  m_Parameter;
    //char         m_szSearchKey[MAX_FIELD_LEN];

    vector<KG_SERVERINFO>           m_ServerList;
    vector<KG_COMPENSATE_ITEM_INFO> m_CompensateItemList;
    vector<KG_SERVERINFO>           m_ErrorServerList;
    vector<int>                     m_CompensateItemCountList;

    unsigned char*  m_pucData;
    unsigned int    unUseDataSize;
};

#endif //KG_ROLEDATAEXPORT2LUA
