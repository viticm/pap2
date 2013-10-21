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

#define SO3_ROLEDB_UPDATER_CONFIG_FILE     "SO3RoleDBUpdater.ini"

#define KG_ID_FIELD_NAME            "ID"
#define KG_NAME_FIELD_NAME          "RoleName"
#define KG_ACCOUNT_FIELD_NAME       "Account"
#define KG_BASE_INFO_FIELD_NAME     "BaseInfo"
#define KG_EXT_INFO_FIELD_NAME      "ExtInfo"
#define KG_LAST_MODIFY_FIELD_NAME   "LastModify"
#define KG_DELETE_TIME_FIELD_NAME   "DeleteTime"
#define KG_MAIL_BOX_INFO_FIELD_NAME "MailBoxInfo"

#define KG_PLAYER_ID_FIELD_NAME         "PlayerID"
#define KG_FELLOWSHIP_DATA_FIELD_NAME   "FellowshipData"

#define KG_MAP_ID_FIELD_NAME            "MapID"
#define KG_MAP_COPY_INDEX_FIELD_NAME    "MapCopyIndex"
#define KG_SCENE_DATA_FIELD_NAME        "SceneData"
#define KG_CREATE_TIME_FIELD_NAME       "CreateTime"
#define KG_LAST_MODIFY_TIME_FILED_NAME  "LastModifyTime"
//#define KG_NEXT_REFRESH_TIME_FILED_NAME "NextRefreshTime"

#define KG_DATA_FILED_NAME            "Data"

#define KG_FREEZETIME_NAME_FILED_NAME "FreezeTime"

#define KG_SEQNAME_FILED_NAME       "seqname"
#define KG_START_ID_FILED_NAME       "startid"
#define KG_CURRENT_ID_FILED_NAME    "currentid"
#define KG_MAX_ID_FILED_NAME        "maxid"
#define KG_GROUP_NAME_FILED_NAME    "groupname"
#define SEQUENCE_TABLE_NAME         "sequence"

#define KG_NAME_FILED_NAME              "Name"
#define KG_AUC_GENRE_FILED_NAME         "AucGenre"
#define KG_AUC_SUB_FILED_NAME           "AucSub"
#define KG_REQUIRE_LEVEL_FILED_NAME     "RequireLevel"
#define KG_QUALITY_FILED_NAME           "Quality"
#define KG_SELLER_NAME_FILED_NAME       "SellerName"
#define KG_PRICE_FILED_NAME             "Price"
#define KG_BUY_IT_NOW_PRICE_FILED_NAME  "BuyItNowPrice"
#define KG_BIDDER_ID_FILED_NAME         "BidderID"
#define KG_CUSTODY_CHARGES_FILED_NAME   "CustodyCharges"
#define KG_DURATION_TIME_FILED_NAME     "DurationTime"
#define KG_ITEM_DATE_FILED_NAME         "ItemData"

#define KG_VALEV_FILED_NAME             "Value"

#define KG_MAILE_INDEX_FILED_NAME       "MailIndex"
#define KG_END_TIME_FILED_NAME          "EndTime"
#define KG_MAIL_INFO_FILED_NAME         "MailInfo"

#define IP_LENGTH                   16
#define _NAME_LEN                   32
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