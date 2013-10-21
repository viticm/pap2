//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   KJx3RoleModifyDBPQTools.h
//  Version     :   1.0
//  Creater     :   Liuzhibiao
//  Date        :   2009-10-14
//  Comment     :   
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _KJX3ROLEMODIFYDBPQTOOLS_H_
#define _KJX3ROLEMODIFYDBPQTOOLS_H_

#include "KDBTools.h"
#define MYSQL_STRING_MAX_SIZE 64
#include<vector>
using namespace std;

const int  FILENAMELEN                       = 256;
const char KJX3SERVERANDSTARTTIMEFILE[]      = "ServerListAndStartTime.tab";
const char KJX3ERRORSERVERLISTFILE[]         = "ErrorServerList";

struct KGD_MYSQL_CONNECT_PARAM
{
    char szZoneName[MYSQL_STRING_MAX_SIZE];
    char szSeverName[MYSQL_STRING_MAX_SIZE];
    char szServerIP[MYSQL_STRING_MAX_SIZE];
    char szUserName[MYSQL_STRING_MAX_SIZE];
    char szPassword[MYSQL_STRING_MAX_SIZE];
    char szDatabaseName[MYSQL_STRING_MAX_SIZE];
    //char szModifyTime[MYSQL_STRING_MAX_SIZE];
    int  nModifyYear;
    int  nModifyMonth;
    int  nModifyDay;

    int nHadModify;
    int nHadCheckOK;
};

class KJx3RoleModifyDBPQTools
{
public:
    KJx3RoleModifyDBPQTools(void);
    ~KJx3RoleModifyDBPQTools(void);
public:
    int Init();
    void UnInit();
    int Run();
    int IsDBHadError();

private:
    int _CheckModifyResult();
    int _ModifyPQData();
    int _WriteErrorServerListToFile();

private:
    KDBTools m_dbLoadTools;
    KDBTools m_dbSaveTools;
    vector< KGD_MYSQL_CONNECT_PARAM > m_ConnectParamList;
    vector< KGD_MYSQL_CONNECT_PARAM > m_ErrorList;
    int      m_nDBHadError;
};

#endif // _KJX3ROLEMODIFYDBPQTOOLS_H_
