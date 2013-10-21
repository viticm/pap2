//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   KDBBase.h
//  Version     :   1.0
//  Creater     :   Xia Yong
//  Date        :   2008-01-11 09:34:39
//  Comment     :   ∑√Œ  ˝æ›ø‚
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _KDBBASE_H_
#define _KDBBASE_H_

#include "mysql.h"

#define SEQ_NAME                    "id_sequence"
#define ROLE_TABLE_NAME             "Role"
#define RESTORE_ROLE_TABLE_NAME     "RestoreRole"
#define DELETE_ROLE_TABLE_NAME      "deleterole"
#define MAIL_TABLE_NAME             "MailBox"
#define MAIL_BASE_TIME_NAME         "BaseTime"
#define MAP_COPY_TABLE_NAME         "MapCopy"
#define FELLOWSHIP_TABLE_NAME       "Fellowship"
#define ROLE_BLACK_LIST_TABLE_NAME  "RoleBlackList"
#define TONG_TABLE_NAME             "Tong"
#define PQ_TABLE_NAME               "PQ"
#define AUCTION_TABLE_NAME          "Auction"
//#define GLOBAL_PARAM_TABLE_NAME     "GlobalParam"
#define GLOBAL_MAIL_TABLE_NAME      "GlobalMail"
#define GLOBAL_CUSTOM_DATA_TABLE_NAME   "GlobalCustomData"
#define GLOBAL_SYSTEM_VALUE_TABLE_NAME  "GlobalSystemValue"

#define MAX_ROLE_DB_SQL_SIZE    (1024 * 128)
#define PING_CYCLE   30

enum eDB_State
{
    dbs_not_connect,
    dbs_new_Create,
    dbs_Exist
};

class KDBBase
{
public:
    KDBBase();
    virtual ~KDBBase();

protected:

    BOOL Connect(
        const char cszDBAddr[], const char cszDBName[], const char cszUserName[], const char cszPassword[]
    );
    void Disconnect();

    BOOL DoQuery(const char cszSQL[]);

    BOOL CheckCaseInsensitive();
    BOOL CheckCharacterSet();

    MYSQL*      m_pDBHandle;
    BOOL        m_bConnected;
    time_t      m_nNextPingTime;

public:
    int         m_nDBState;
};

#endif

