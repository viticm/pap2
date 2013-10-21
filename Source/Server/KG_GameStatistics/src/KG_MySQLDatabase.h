////////////////////////////////////////////////////////////////////////////////
//  Copyright(c) Kingsoft
//
//  FileName    : KG_MySQLDatabase.h
//  Version     : 1.0
//  Creator     : Hu Hao
//  Create Date : 2009-12-11  14:33:06
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _KG_MYSQL_DATABASE_H_
#define _KG_MYSQL_DATABASE_H_

#include "mysql.h"

///////////////////////////////////////////////////////////////////////////////
struct KG_MYSQL_PARAM
{
    char szServer[64];
    char szCharacterSet[64];
    char szDatabase[64];
    char szUserName[64];
    char szPassword[64];
    int  nPort;
};

///////////////////////////////////////////////////////////////////////////////
#define KGD_MYSQL_PROCESS_ERROR(Condition, pMySQL, pszMsg)  \
do                                                          \
{                                                           \
    if (!(Condition))                                       \
    {                                                       \
        KGLogPrintf(                                        \
            KGLOG_DEBUG,                                    \
            "KGD_MYSQL_PROCESS_ERROR at line %d in %s(), \"%s\", \"%s\"",   \
            __LINE__, KG_FUNCTION, mysql_error(pMySQL), (pszMsg)            \
        );                                                  \
        goto Exit0;                                         \
    }                                                       \
} while (false)

///////////////////////////////////////////////////////////////////////////////
MYSQL *KG_ConnectMySQLDatabase(const KG_MYSQL_PARAM &crParam);


#endif  // _KG_MYSQL_DATABASE_H_
