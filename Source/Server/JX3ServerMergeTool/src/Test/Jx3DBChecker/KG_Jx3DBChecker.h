//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   KG_Jx3DBChecker.h
//  Version     :   1.0
//  Creater     :   Liuzhibiao
//  Date        :   2009-12-8
//  Comment     :   
//////////////////////////////////////////////////////////////////////////////////////

#ifndef KG_JX3DBCHECKER_H
#define KG_JX3DBCHECKER_H

#include "KDBTools.h"
#include "KG_DBCheckRule.h"

const int CN_THREAD_COUNT = 2;
const int CN_DBTOOLS_COUNT = 3;

const char DBCHECKER_CONFIG_FILE[] = "Jx3DBChecker.ini";

struct KGD_MYSQL_CONNECT_PARAM
{
    char szServer[MYSQL_STRING_MAX_SIZE];
    char szUserName[MYSQL_STRING_MAX_SIZE];
    char szPassword[MYSQL_STRING_MAX_SIZE];
    char szDBName[MYSQL_STRING_MAX_SIZE];
};

enum TACK_STATE
{
    ts_Ready,
    ts_Running,
    ts_HadErrorDone,
    ts_HadDoneWithoutError
};

struct KG_TASK_INFO
{
    //char szMessage[512];
    string strMessage;
    int  nState;
    KDBTools DBTools[CN_DBTOOLS_COUNT];
    //char szTableName[512];
    string strTableName;
    KG_DBCheckRule* pDBChenkRule;
};

class KG_Jx3DBChecker
{
public:
    int Init();
    void UnInit();

    int Run();

private:
    int _LoadSettings();

private:
    BOOL        m_bRunFlag;
    KThread     m_WorkThread[CN_THREAD_COUNT];
    int         ThreadFunction();
    static void WorkThreadFunction(void* pvParam);

private:
    int m_nTaskCount;
    vector<KG_TASK_INFO> m_Tasks;

    KG_DBCheckRule m_dbCheckRule;

    KGD_MYSQL_CONNECT_PARAM m_mySqlConnectParan[CN_DBTOOLS_COUNT];
};

#endif//KG_JX3DBCHECKER_H