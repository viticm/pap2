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
#include "mysqld_error.h"

#define SEQ_GROUP_NAME                  "id_sequence"
#define ROLE_TABLE_NAME                 "Role"
#define RESTORE_ROLE_TABLE_NAME         "RestoreRole"
#define GLOBAL_MAIL_TABLE_NAME          "GlobalMailEx"
#define GLOBAL_MAIL_TABLE_NAME_OLD      "GlobalMail"
#define MAIL_TABLE_NAME                 "MailBox"
#define MAP_COPY_TABLE_NAME             "MapCopy"
#define FELLOWSHIP_TABLE_NAME           "Fellowship"
#define ROLE_BLACK_LIST_TABLE_NAME      "RoleBlackList"
#define TONG_TABLE_NAME                 "Tong"
#define PQ_TABLE_NAME                   "PQ"
#define AUCTION_TABLE_NAME              "Auction"
#define GLOBAL_CUSTOM_DATA_TABLE_NAME   "GlobalCustomData"
#define GLOBAL_SYSTEM_VALUE_TABLE_NAME  "GlobalSystemValue"
#define STAT_DATA_TABLE_NAME            "StatData"
#define STAT_DATA_NAME_INDEX_TABLE_NAME "StatDataNameIndex"
#define ACTIVITY_TABLE_NAME             "Activity"
#define GAME_CARD_TABLE_NAME            "GameCard"
#define ROLE_DELETE_LIST_TABLE_NAME     "RoleDeleteList"
#define ANTI_FARMER_TABLE_NAME          "AntiFarmer"
#define MENTOR_TABLE_NAME               "Mentor"
#define RENAME_ROLE_TABLE_NAME          "RenameRole"

#define PING_CYCLE   30

MYSQL* MySQL_Connect(
    const char cszDBAddr[], const char cszDBName[], const char cszUserName[], const char cszPassword[]
);
void   MySQL_Disconnect(MYSQL* pHandle);

BOOL   MySQL_Query(MYSQL* pHandle, const char cszSQL[], unsigned* puRetMySQLErrorCode = NULL);

#endif

