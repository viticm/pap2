///////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
// 
// FileName : KG_SO3RoleDBUpdaterDef.h
// Creator  : liuzhibiao
// Date     : 2009-5-26
// Comment  : v0.1
//            v0.1  2009-5-26   Create  	
///////////////////////////////////////////////////////////////

#ifndef _KG_SO3ROLEDBUPDATERDEF_H
#define _KG_SO3ROLEDBUPDATERDEF_H

#define SO3_ROLEDB_UPDATER_CONFIG_FILE     "SO3RoleDBUpdaterTest.ini"

#define KG_ID_FIELD_NAME            "ID"
#define KG_NAME_FIELD_NAME          "RoleName"
#define KG_ACCOUNT_FIELD_NAME       "Account"
#define KG_BASE_INFO_FIELD_NAME     "BaseInfo"
#define KG_EXT_INFO_FIELD_NAME      "ExtInfo"
#define KG_LAST_MODIFY_FIELD_NAME   "LastModify"
#define KG_DELETE_TIME_FIELD_NAME   "DeleteTime"
#define KG_MAIL_BOX_INFO_FIELD_NAME "MailBoxInfo"

#define IP_LENGTH               16
#define _NAME_LEN               32
#define MAIL_BOX_INFO_MAX_SIZE      204800

#pragma pack(1)

struct KG_ROLEDBCONVERSION_CONFIG
{
    char    szConnectToIP[IP_LENGTH];
    char    szDBName[_NAME_LEN];
    char    szDBAcc[_NAME_LEN];
    char    szDBPsw[_NAME_LEN];
    int     nPort;
};

#pragma pack()

#endif//_KG_SO3ROLEDBUPDATERDEF_H