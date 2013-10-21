////////////////////////////////////////////////////////////////////////////////
//  Copyright(c) Kingsoft
//
//  FileName    : KG_Public.h
//  Version     : 1.0
//  Creator     : Hu Hao
//  Create Date : 2009-9-9  17:55:41
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _KG_PUBLIC_H_
#define _KG_PUBLIC_H_

#define	_CRT_SECURE_NO_DEPRECATE
#define	_CRT_NONSTDC_NO_DEPRECATE

#include <list>
#include "KGPublic.h"
#include "Engine/KGLog.h"
#include "Common/KG_Memory.h"
#include "KCommon.h"
#include "mysql.h"

//////////////////////////////////////////////////////////////////////////////
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

/////////////////////////////////////////////////////////////////////////////
struct KG_MYSQL_PARAM
{
    char szServer[64];
    char szDatabase[64];
    char szUserName[64];
    char szPassword[64];
};

typedef std::list<PARSEHANDLE> KG_TONG_HANDLE_LIST;

///////////////////////////////////////////////////////////////////////////////
#if (defined(_MSC_VER) || defined(__ICL))
#define snprintf  _snprintf
#endif 

///////////////////////////////////////////////////////////////////////////////
int KG_SetRuntimeEnvironment(const char cszAppFullPathName[MAX_PATH]);

int KG_LoadMySQLDatabaseParameter(const char cszConfigFileName[], KG_MYSQL_PARAM *pRetParam);

// @brief : Connect the MySQL database and set current database and connection character.
MYSQL *KG_ConnectMySQLDatabase(const KG_MYSQL_PARAM &crParam);

// @brief : Create the table `Activity` if not exits.
int KG_CreateRewardTable(MYSQL *pMySQL);

int KG_RemoveOldRewardData(MYSQL *pMySQL, int nActivityID);

// @brief : Insert an record into table `Activity`.
int KG_RewardRole(
    MYSQL *pMySQL, int nRoleID, int nActivityID, const char szDeadline[],
    int nValue0, int nValue1, int nValue2, int nValue3, int nValue4, int nValue5
);

int KG_GetTongHandleList(MYSQL *pMySQL, KG_TONG_HANDLE_LIST *pRetTongHandleList);



#endif //_KG_PUBLIC_H_



