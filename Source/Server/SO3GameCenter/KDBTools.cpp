#include "stdafx.h"
#include "mysql.h"
#include "KDBBase.h"
#include "KDBTools.h"
#include "KSO3GameCenter.h"

#ifdef WIN32
#define atoll _atoi64
#endif
KDBTools::KDBTools()
{
    m_pDBHandle         = NULL;
}

BOOL KDBTools::Init()
{
    BOOL bResult    = false;
    BOOL bRetCode   = false;
    
    m_pDBHandle = MySQL_Connect(
        g_pSO3GameCenter->m_Settings.m_szDBIP,
        g_pSO3GameCenter->m_Settings.m_szDBName,
        g_pSO3GameCenter->m_Settings.m_szDBAcc, 
        g_pSO3GameCenter->m_Settings.m_szDBPsw
    );
    KGLOG_PROCESS_ERROR(m_pDBHandle);

    bResult = true;
Exit0:
    if (!bResult)
    {
        if (m_pDBHandle)
        {
            MySQL_Disconnect(m_pDBHandle);
            m_pDBHandle = NULL;
        }
    }
    return bResult;
}

void KDBTools::UnInit()
{
    if (m_pDBHandle)
    {
        MySQL_Disconnect(m_pDBHandle);
        m_pDBHandle = NULL;
    }
}

BOOL KDBTools::CheckDatabase()
{
    BOOL    bResult     = false;
    BOOL    bRetCode    = false;

    bRetCode = CheckRoleTable();
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = CheckFellowshipTable();
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = CheckMapCopyTable();
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = CheckRoleBlakListTable();
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = CheckTongTable();
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = CheckMailTable();
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = CheckGlobalMailTable();
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = CheckPQTable();
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = CheckAuctionTable();
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = CheckGlobalCustomDataTable();
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = CheckGlobalSystemValueTable();
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = CheckStatDataTable();
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = CheckActivityTable();
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = CheckGameCardTable();
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = CheckRoleDeleteTable();
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = CheckAntiFarmerTable();
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = CheckMentorTable();
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = CheckRenameRoleTable();
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KDBTools::LoadGameData()
{
    BOOL bResult    = false;
    BOOL bRetCode   = false;

    bRetCode = LoadRoleTable();
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = LoadMapCopyInfo();
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = LoadRoleBlackList();
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = LoadTongTable();
    KGLOG_PROCESS_ERROR(bRetCode);
    
    bRetCode = LoadGlobalMailTable();
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = LoadGlobalSystemValue();
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = LoadGlobalCustomDataTable();
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = LoadStatDataTable();
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = LoadActivityTable();
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = LoadRoleDeleteTable();
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = LoadAntiFarmerTable();
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = LoadMentorTable();
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = LoadRenameRoleTable();
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KDBTools::CheckRoleTable()
{
    BOOL                bResult     = false;
    BOOL                bRetCode    = 0;
    int                 nStrLen     = 0;
    
    nStrLen = snprintf(m_szSQL, sizeof(m_szSQL), 
        "create table if not exists %s "
        "("
        "   %s bigint not null,"
        "   %s varchar(32) not null,"
        "   %s varchar(32) not null,"
        "   %s blob not null,"
        "   %s mediumblob,"
        "   %s datetime not null,"
        "   primary key (%s), index (%s), unique (%s)"
        ");",
        ROLE_TABLE_NAME,            KG_ID_FIELD_NAME,         KG_NAME_FIELD_NAME, 
        KG_ACCOUNT_FIELD_NAME,      KG_BASE_INFO_FIELD_NAME,  KG_EXT_INFO_FIELD_NAME, 
        KG_LAST_MODIFY_FIELD_NAME,  KG_ID_FIELD_NAME,         KG_ACCOUNT_FIELD_NAME,    KG_NAME_FIELD_NAME
    );
    KGLOG_PROCESS_ERROR(nStrLen > 0 && nStrLen < (int)sizeof(m_szSQL));

    bRetCode = DoQuery(m_szSQL);
    if (!bRetCode)
    {
        KGLogPrintf(
            KGLOG_ERR, 
            "DB ERROR when try to create table \'%s\'\n", 
            ROLE_TABLE_NAME
        );
        goto Exit0;
    }
    
    nStrLen = snprintf(m_szSQL, sizeof(m_szSQL), 
        "create table if not exists %s "
        "("
        "   %s bigint not null auto_increment,"
        "   %s varchar(32) not null,"
        "   %s varchar(32) not null,"
        "   %s blob not null,"
        "   %s mediumblob,"
        "   %s datetime not null,"
        "   primary key (%s), index (%s)"
        ");",
        RESTORE_ROLE_TABLE_NAME,    KG_ID_FIELD_NAME,         KG_NAME_FIELD_NAME, 
        KG_ACCOUNT_FIELD_NAME,      KG_BASE_INFO_FIELD_NAME,  KG_EXT_INFO_FIELD_NAME, 
        KG_DELETE_TIME_FIELD_NAME,  KG_ID_FIELD_NAME,         KG_ACCOUNT_FIELD_NAME
    );
    KGLOG_PROCESS_ERROR(nStrLen > 0 && nStrLen < (int)sizeof(m_szSQL));

    bRetCode = DoQuery(m_szSQL);
    if (!bRetCode)
    {
        KGLogPrintf(
            KGLOG_ERR, 
            "DB ERROR when try to create table \'%s\'\n", 
            RESTORE_ROLE_TABLE_NAME
        );
        goto Exit0;
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KDBTools::CheckFellowshipTable()
{
    BOOL                bResult     = false;
    BOOL                bRetCode    = 0;
    int                 nStrLen     = 0;

    nStrLen = snprintf(m_szSQL, sizeof(m_szSQL), 
        "create table if not exists "
        "    %s "
        "    ( "
        "        PlayerID               integer unsigned    not null, "
        "        FellowshipData         blob, "
        "        primary key (PlayerID) "
        "    ) "
        ";",
        FELLOWSHIP_TABLE_NAME
    );
    KGLOG_PROCESS_ERROR(nStrLen > 0 && nStrLen < (int)sizeof(m_szSQL));

    bRetCode = DoQuery(m_szSQL);
    if (!bRetCode)
    {
        KGLogPrintf(
            KGLOG_ERR, 
            "DB ERROR when try to create table \'%s\'\n", 
            FELLOWSHIP_TABLE_NAME
        );
        goto Exit0;
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KDBTools::CheckMapCopyTable()
{
    BOOL                bResult     = false;
    BOOL                bRetCode    = 0;
    int                 nStrLen     = 0;

    nStrLen = snprintf(m_szSQL, sizeof(m_szSQL), 
        "create table if not exists %s( "
        "   MapID integer unsigned, "
        "   MapCopyIndex integer, "
        "   SceneData blob, "
        "   CreateTime datetime not null, "
        "   LastModifyTime datetime not null, "
        "   primary key  (MapID, MapCopyIndex) "
        "); ",
        MAP_COPY_TABLE_NAME
    );
    KGLOG_PROCESS_ERROR(nStrLen > 0 && nStrLen < (int)sizeof(m_szSQL));

    bRetCode = DoQuery(m_szSQL);
    if (!bRetCode)
    {
        KGLogPrintf(
            KGLOG_ERR, 
            "DB ERROR when try to create table \'%s\'\n", 
            MAP_COPY_TABLE_NAME
        );
        goto Exit0;
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KDBTools::CheckRoleBlakListTable()
{
    BOOL                bResult     = false;
    BOOL                bRetCode    = 0;
    int                 nStrLen     = 0;

    nStrLen = snprintf(m_szSQL, sizeof(m_szSQL), 
        "create table if not exists %s "
        "("
        "   ID bigint not null,"
        "   FreezeTime datetime not null,"
        "   primary key (ID), index (ID)"
        ");",
        ROLE_BLACK_LIST_TABLE_NAME
    );
    KGLOG_PROCESS_ERROR(nStrLen > 0);
    KGLOG_PROCESS_ERROR(nStrLen < (int)sizeof(m_szSQL));

    bRetCode = DoQuery(m_szSQL);
    if (!bRetCode)
    {
        KGLogPrintf(
            KGLOG_ERR, 
            "DB ERROR when try to create table \'%s\'\n", 
            ROLE_BLACK_LIST_TABLE_NAME
        );
        goto Exit0;
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KDBTools::CheckTongTable()
{
    BOOL            bResult     = false;
    BOOL            bRetCode    = 0;
    int             nStrLen     = 0;

    nStrLen = snprintf(m_szSQL, sizeof(m_szSQL), 
        "create table if not exists %s "
        "("
        "   ID              bigint not null,"
        "   Data            mediumblob,"
        "   primary         key (ID) "
        ");",
        TONG_TABLE_NAME
    );
    KGLOG_PROCESS_ERROR(nStrLen > 0);
    KGLOG_PROCESS_ERROR(nStrLen < (int)sizeof(m_szSQL));

    bRetCode = DoQuery(m_szSQL);
    if (!bRetCode)
    {
        KGLogPrintf(
            KGLOG_ERR, 
            "DB ERROR when try to create table \'%s\'\n", 
            TONG_TABLE_NAME
        );
        goto Exit0;
    }


    bResult = true;
Exit0:
    return bResult;
}

BOOL KDBTools::CheckGlobalMailTable()
{
    BOOL                bResult     = false;
    BOOL                bRetCode    = 0;
    int                 nStrLen     = 0;

    nStrLen = snprintf(m_szSQL, sizeof(m_szSQL), 
        "create table if not exists %s( "
        "   Version      integer    not null, "
        "   MailIndex    bigint     not null, "
        "   EndTime      datetime   not null, "
        "   MailInfo     mediumblob not null, "
        "   primary key (MailIndex) "
        "); ",
        GLOBAL_MAIL_TABLE_NAME
    );
    KGLOG_PROCESS_ERROR(nStrLen > 0 && nStrLen < (int)sizeof(m_szSQL));
    
    bRetCode = DoQuery(m_szSQL);
    if (!bRetCode)
    {
        KGLogPrintf(
            KGLOG_ERR, 
            "DB ERROR when try to create table \'%s\'\n", 
            GLOBAL_MAIL_TABLE_NAME
        );
        goto Exit0;
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KDBTools::CheckMailTable()
{
    BOOL                bResult     = false;
    BOOL                bRetCode    = 0;
    int                 nStrLen     = 0;

    nStrLen = snprintf(m_szSQL, sizeof(m_szSQL), 
        "create table if not exists %s( "
        "   ID           bigint     not null, "
        "   BaseTime     datetime   not null, "
        "   MailBoxInfo  mediumblob not null, "
        "   primary key (ID), index (ID, BaseTime) "
        "); ",
        MAIL_TABLE_NAME
    );
    KGLOG_PROCESS_ERROR(nStrLen > 0 && nStrLen < (int)sizeof(m_szSQL));

    bRetCode = DoQuery(m_szSQL);
    if (!bRetCode)
    {
        KGLogPrintf(
            KGLOG_ERR, 
            "DB ERROR when try to create table \'%s\'\n", 
            MAIL_TABLE_NAME
        );
        goto Exit0;
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KDBTools::CheckPQTable()
{
    BOOL                bResult     = false;
    BOOL                bRetCode    = 0;
    int                 nStrLen     = 0;

    nStrLen = snprintf(m_szSQL, sizeof(m_szSQL), 
        "create table if not exists %s "
        "("
        "   ID integer unsigned not null, "
        "   Data blob not null, "
        "   primary key (ID), index (ID) "
        ");",
        PQ_TABLE_NAME
    );
    KGLOG_PROCESS_ERROR(nStrLen > 0);
    KGLOG_PROCESS_ERROR(nStrLen < (int)sizeof(m_szSQL));

    bRetCode = DoQuery(m_szSQL);
    if (!bRetCode)
    {
        KGLogPrintf(
            KGLOG_ERR, 
            "DB ERROR when try to create table \'%s\'\n", 
            PQ_TABLE_NAME
        );
        goto Exit0;
    }

    bResult = true;
Exit0:
    return bResult;

}

BOOL KDBTools::CheckAuctionTable()
{
    BOOL                bResult         = false;
    BOOL                bRetCode        = 0;
    int                 nStrLen         = 0;

    nStrLen = snprintf(m_szSQL, sizeof(m_szSQL), 
        "create table if not exists %s( "
        "   ID              bigint      not null auto_increment, "
        "   Name            varchar(32) not null, "
        "   AucGenre        integer     not null, "
        "   AucSub          integer     not null, "
        "   RequireLevel    integer     not null, "
        "   Quality         integer     not null, "
        "   SellerID        bigint      not null, "
        "   SellerName      varchar(32) not null, "
        "   Price           integer     not null, "
        "   BuyItNowPrice   integer     not null, "
        "   BidderID        bigint      not null, "
        "   CustodyCharges  integer     not null, "
        "   DurationTime    datetime    not null, "
        "   ItemData        blob        not null, "
        "   primary key (ID) "
        ");",
        AUCTION_TABLE_NAME
    );
    KGLOG_PROCESS_ERROR(nStrLen > 0 && nStrLen < (int)sizeof(m_szSQL));

    bRetCode = DoQuery(m_szSQL);
    if (!bRetCode)
    {
        KGLogPrintf(
            KGLOG_ERR, 
            "DB ERROR when try to create table \'%s\'\n", 
            AUCTION_TABLE_NAME
        );
        goto Exit0;
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KDBTools::CheckGlobalCustomDataTable()
{
    BOOL    beResult = false;
    BOOL    bRetCode = 0;
    int     nStrLen  = 0;

    nStrLen = snprintf(m_szSQL, sizeof(m_szSQL), 
        "create table if not exists %s( "
        "   Name      varchar(32)   not null, "
        "   Value     mediumblob    not null, "
        "   primary key (Name) "
        ");",
        GLOBAL_CUSTOM_DATA_TABLE_NAME
	);
    KGLOG_PROCESS_ERROR(nStrLen > 0 && nStrLen < (int)sizeof(m_szSQL));

    bRetCode = DoQuery(m_szSQL);
    if (!bRetCode)
    {
        KGLogPrintf(
            KGLOG_ERR, 
            "DB ERROR when try to create table \'%s\'\n", 
            GLOBAL_CUSTOM_DATA_TABLE_NAME
        );
        goto Exit0;
    }

    beResult = true;
Exit0:
    return beResult;
}

BOOL KDBTools::CheckGlobalSystemValueTable()
{
    BOOL    beResult = false;
    BOOL    bRetCode = 0;
    int     nStrLen  = 0;

    nStrLen = snprintf(
        m_szSQL, sizeof(m_szSQL), 
        "create table if not exists %s( "
        "   Name      varchar(32)   not null, "
        "   Value     integer       not null, "
        "   primary key (Name) "
        ");",
        GLOBAL_SYSTEM_VALUE_TABLE_NAME
    );
    KGLOG_PROCESS_ERROR(nStrLen > 0 && nStrLen < (int)sizeof(m_szSQL));

    bRetCode = DoQuery(m_szSQL);
    if (!bRetCode)
    {
        KGLogPrintf(
            KGLOG_ERR, 
            "DB ERROR when try to create table \'%s\'\n", 
            GLOBAL_SYSTEM_VALUE_TABLE_NAME
        );
        goto Exit0;
    }

    beResult = true;
Exit0:
    return beResult;
}

BOOL KDBTools::CheckStatDataTable()
{
    BOOL    bResult  = false;
    BOOL    bRetCode = 0;
    int     nStrLen  = 0;

    nStrLen = snprintf(
        m_szSQL, sizeof(m_szSQL), 
        "create table if not exists %s( "
        "   Name      varchar(64)   not null, "
        "   ID        integer       not null, "
        "   primary key (Name) "
        ");",
        STAT_DATA_NAME_INDEX_TABLE_NAME
    );
    KGLOG_PROCESS_ERROR(nStrLen > 0 && nStrLen < (int)sizeof(m_szSQL));

    bRetCode = DoQuery(m_szSQL);
    if (!bRetCode)
    {
        KGLogPrintf(
            KGLOG_ERR, 
            "DB ERROR when try to create table \'%s\'\n", 
            STAT_DATA_NAME_INDEX_TABLE_NAME
        );
        goto Exit0;
    }

    nStrLen = snprintf(
        m_szSQL, sizeof(m_szSQL), 
        "create table if not exists %s( "
        "   StatTime    datetime        not null, "
        "   StatData    mediumblob      not null, "
        "   primary key (StatTime) "
        ");",
        STAT_DATA_TABLE_NAME
    );
    KGLOG_PROCESS_ERROR(nStrLen > 0 && nStrLen < (int)sizeof(m_szSQL));

    bRetCode = DoQuery(m_szSQL);
    if (!bRetCode)
    {
        KGLogPrintf(
            KGLOG_ERR, 
            "DB ERROR when try to create table \'%s\'\n", 
            STAT_DATA_TABLE_NAME
        );
        goto Exit0;
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KDBTools::CheckActivityTable()
{
    BOOL    bResult  = false;
    BOOL    bRetCode = 0;
    int     nStrLen  = 0;

    nStrLen = snprintf(
        m_szSQL, sizeof(m_szSQL), 
        "create table if not exists %s( "
        "   ID      integer     not null, "
        "   Type    integer     not null, "
        "   Time    datetime    not null, "
        "   Value0  integer     not null, "
        "   Value1  integer     not null, "
        "   Value2  integer     not null, "
        "   Value3  integer     not null, "
        "   Value4  integer     not null, "
        "   Value5  integer     not null, "
        "   primary key (ID, Type) "
        ");",
        ACTIVITY_TABLE_NAME
    );
    KGLOG_PROCESS_ERROR(nStrLen > 0 && nStrLen < (int)sizeof(m_szSQL));

    bRetCode = DoQuery(m_szSQL);
    if (!bRetCode)
    {
        KGLogPrintf(
            KGLOG_ERR, 
            "DB ERROR when try to create table \'%s\'\n", 
            ACTIVITY_TABLE_NAME
        );
        goto Exit0;
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KDBTools::CheckGameCardTable()
{
    BOOL    bResult  = false;
    BOOL    bRetCode = 0;
    int     nStrLen  = 0;

    nStrLen = snprintf(
        m_szSQL, sizeof(m_szSQL), 
        "create table if not exists %s( "
        "   ID          bigint      not null auto_increment, "
        "   SellerID    bigint      not null, "
        "   Coin        integer     not null, "
        "   GameTime    integer     not null, "
        "   Type        integer     not null, "
        "   Price       integer     not null, "
        "   EndTime     datetime    not null, "
        "   primary key (ID) "
        ");",
        GAME_CARD_TABLE_NAME
    );
    KGLOG_PROCESS_ERROR(nStrLen > 0 && nStrLen < (int)sizeof(m_szSQL));

    bRetCode = DoQuery(m_szSQL);
    if (!bRetCode)
    {
        KGLogPrintf(
            KGLOG_ERR, 
            "DB ERROR when try to create table \'%s\'\n", 
            GAME_CARD_TABLE_NAME
        );
        goto Exit0;
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KDBTools::CheckRoleDeleteTable()
{
    BOOL    bResult  = false;
    BOOL    bRetCode = 0;
    int     nStrLen  = 0;

    nStrLen = snprintf(
        m_szSQL, sizeof(m_szSQL), 
        "create table if not exists %s( "
        "   ID          bigint      not null, "
        "   EndTime     datetime    not null, "
        "   primary key (ID) "
        ");",
        ROLE_DELETE_LIST_TABLE_NAME
    );
    KGLOG_PROCESS_ERROR(nStrLen > 0 && nStrLen < (int)sizeof(m_szSQL));

    bRetCode = DoQuery(m_szSQL);
    if (!bRetCode)
    {
        KGLogPrintf(
            KGLOG_ERR, 
            "DB ERROR when try to create table \'%s\'\n", 
            ROLE_DELETE_LIST_TABLE_NAME
        );
        goto Exit0;
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KDBTools::CheckAntiFarmerTable()
{
    BOOL    bResult  = false;
    BOOL    bRetCode = 0;
    int     nStrLen  = 0;

    nStrLen = snprintf(
        m_szSQL, sizeof(m_szSQL), 
        "create table if not exists %s( "
        "   Account         varchar(32) not null, "
        "   PunishEndTime   datetime    not null, "
        "   primary key (Account) "
        ");",
        ANTI_FARMER_TABLE_NAME
    );
    KGLOG_PROCESS_ERROR(nStrLen > 0 && nStrLen < (int)sizeof(m_szSQL));

    bRetCode = DoQuery(m_szSQL);
    if (!bRetCode)
    {
        KGLogPrintf(
            KGLOG_ERR, 
            "DB ERROR when try to create table \'%s\'\n", 
            ANTI_FARMER_TABLE_NAME
        );
        goto Exit0;
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KDBTools::CheckMentorTable()
{
    BOOL bResult    = false;
    BOOL bRetCode   = false;
    int  nStrLen    = 0;

    nStrLen = snprintf(
        m_szSQL, sizeof(m_szSQL), 
        "create table if not exists %s( "
        "   MentorID      bigint    not null, "
        "   ApprenticeID  bigint    not null, "
        "   Data          tinyblob  not null, "
        "   primary key (MentorID, ApprenticeID) "
        ");",
        MENTOR_TABLE_NAME
    );
    KGLOG_PROCESS_ERROR(nStrLen > 0 && nStrLen < (int)sizeof(m_szSQL));

    bRetCode = DoQuery(m_szSQL);
    if (!bRetCode)
    {
        KGLogPrintf(
            KGLOG_ERR,
            "DB ERROR when try to create table \'%s\'\n",
            MENTOR_TABLE_NAME
        );
        goto Exit0;
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KDBTools::CheckRenameRoleTable()
{
    BOOL bResult    = false;
    BOOL bRetCode   = false;
    int  nStrLen    = 0;
    
    nStrLen = snprintf(
        m_szSQL, sizeof(m_szSQL),
        "create table if not exists %s("
        " RoleID bigint not null,"
        " primary key(RoleID));",
        RENAME_ROLE_TABLE_NAME
    );
    KGLOG_PROCESS_ERROR(nStrLen > 0 && nStrLen < (int)sizeof(m_szSQL));

    bRetCode = DoQuery(m_szSQL);
    if (!bRetCode)
    {
        KGLogPrintf(
            KGLOG_ERR,
            "DB ERROR when try to create table \'%s\'\n",
            RENAME_ROLE_TABLE_NAME
        );
        goto Exit0;
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KDBTools::LoadRoleTable()
{
    BOOL             bResult         = false;
    BOOL             bRetCode        = false;
    int              nRetCode        = false;
    MYSQL_RES*       pQueryRes       = NULL;
    DWORD            dwRowCount      = 0;
    BYTE*            pbyData         = NULL;
    unsigned long*   pulDataLeng     = NULL;
    DWORD            dwRoleCount     = 0;
    KRoleBaseInfo*   pBaseInfo       = NULL;
    MYSQL_ROW        QueryRow;

    assert(m_pDBHandle);

    KGLogPrintf(KGLOG_INFO, "[DB] Role data loading ... ...");

    nRetCode = snprintf(
        m_szSQL, sizeof(m_szSQL), "select %s, %s, %s, %s from %s", 
        KG_ID_FIELD_NAME,           KG_NAME_FIELD_NAME,     KG_ACCOUNT_FIELD_NAME, 
        KG_BASE_INFO_FIELD_NAME,    ROLE_TABLE_NAME
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)sizeof(m_szSQL));

    bRetCode = DoQuery(m_szSQL);
    KGLOG_PROCESS_ERROR(bRetCode);

    pQueryRes = mysql_use_result(m_pDBHandle);
    KGLOG_PROCESS_ERROR(pQueryRes);

    while (true)
    {
        KRole*          pRetRole    = NULL;
        DWORD           dwRoleID    = ERROR_ID;
        unsigned long*  ulLengths   = NULL;

        QueryRow = mysql_fetch_row(pQueryRes);
        if (QueryRow == NULL)
        {
            break;
        }
        
        ulLengths = mysql_fetch_lengths(pQueryRes);
        KGLOG_PROCESS_ERROR(ulLengths);
        
        KGLOG_PROCESS_ERROR(ulLengths[1] < _NAME_LEN);
        KGLOG_PROCESS_ERROR(ulLengths[2] < _NAME_LEN);
        KGLOG_PROCESS_ERROR(ulLengths[3] == sizeof(KRoleBaseInfo));

        assert(QueryRow[0]);
        assert(QueryRow[1]);
        assert(QueryRow[2]);
        assert(QueryRow[3]);
        
        dwRoleID = (DWORD)strtoul(QueryRow[0], NULL, 10);
        pBaseInfo = (KRoleBaseInfo*)QueryRow[3];

        KGLOG_PROCESS_ERROR(pBaseInfo->nVersion == 0);

        pRetRole = g_pSO3GameCenter->m_RoleManager.AddRole(
            QueryRow[2], QueryRow[1], dwRoleID,
            pBaseInfo->byLevel, (BYTE)pBaseInfo->cRoleType, (KCAMP)pBaseInfo->byCamp, pBaseInfo->byForceID, pBaseInfo->nLastSaveTime, pBaseInfo->nCreateTime
        );
        KGLOG_PROCESS_ERROR(pRetRole);

        ++dwRoleCount;
    }

    bResult = true;
Exit0:

    KGLogPrintf(KGLOG_INFO, "[DB] %u roles loaded !", dwRoleCount);

    if (pQueryRes)
    {
        mysql_free_result(pQueryRes);
        pQueryRes = NULL;
    }
    return bResult;
}

BOOL KDBTools::LoadMapCopyInfo()
{
    BOOL                bResult             = false;
    BOOL                bRetCode            = false;
    int                 nRetCode            = false;
    MYSQL_RES*          pQueryRes           = NULL;
    unsigned long*      pulFieldLen         = NULL;
    DWORD               dwMapID             = 0;
    int                 nMapCopyIndex       = 0;
    KMapInfo*           pMapInfo            = NULL;
    KMapCopy*           pMapCopy            = NULL;
    BYTE*               pbySceneData        = NULL;
    size_t              uSceneDataLen       = 0;
    time_t              nCreateTime         = 0;
    time_t              nLastSaveTime       = 0;
    time_t              nNextRefreshTime    = 0;
    BYTE*               pbyData             = NULL;
    int                 nMapCopyCount       = 0;
    BOOL                bAutoDelete         = false;
    KMapManager*        pMapManager         = &(g_pSO3GameCenter->m_MapManager);
    MYSQL_ROW           QueryRow;

    assert(pMapManager);

    assert(m_pDBHandle);

    KGLogPrintf(KGLOG_DEBUG, "[DB] Map copys loading ... ...");

    nRetCode = snprintf(
        m_szSQL, sizeof(m_szSQL),
        "select " 
        "   MapID, MapCopyIndex, SceneData, "
        "   UNIX_TIMESTAMP(CreateTime), UNIX_TIMESTAMP(LastModifyTime) "
        "from " 
        "   %s;",
        MAP_COPY_TABLE_NAME
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)sizeof(m_szSQL));

    bRetCode = DoQuery(m_szSQL);
    KGLOG_PROCESS_ERROR(bRetCode);

    pQueryRes = mysql_use_result(m_pDBHandle);
    KGLOG_PROCESS_ERROR(pQueryRes);

    while (true)
    {
        QueryRow = mysql_fetch_row(pQueryRes);
        if (!QueryRow)
            break;

        pulFieldLen = mysql_fetch_lengths(pQueryRes);
        KGLOG_PROCESS_ERROR(pulFieldLen);

        dwMapID             = (DWORD)strtoul(QueryRow[0], NULL, 10);
        nMapCopyIndex       = atoi(QueryRow[1]);
        pbySceneData        = (BYTE*)QueryRow[2];
        uSceneDataLen       = pulFieldLen[2];
        nCreateTime         = strtol(QueryRow[3], NULL, 10);
        nLastSaveTime       = strtol(QueryRow[4], NULL, 10);
        
        KGLOG_PROCESS_ERROR(uSceneDataLen > 0);

        pMapInfo = pMapManager->GetMapInfo(dwMapID);
        KGLOG_PROCESS_ERROR(pMapInfo);
        
        if (pMapInfo->m_nRefreshCycle > 0)
        {
            pMapInfo->m_nNextRefreshTime = (nLastSaveTime / pMapInfo->m_nRefreshCycle + 1) * (pMapInfo->m_nRefreshCycle);
        }

        bRetCode = pMapManager->IsMapCopyInPreloadGroup(dwMapID, nMapCopyIndex); // 在预加载表中的地图不是自动删除的
        bAutoDelete = !bRetCode;

        pMapCopy = pMapInfo->CreateMapCopy(nMapCopyIndex, nCreateTime, nLastSaveTime, 0, bAutoDelete);
        KGLOG_PROCESS_ERROR(pMapCopy);

        bRetCode = pMapCopy->Load(pbySceneData, uSceneDataLen);
        KGLOG_PROCESS_ERROR(bRetCode);

        pMapCopy->m_eState          = eMapStateOffline;
        pMapCopy->m_bIsChanged      = false;
        pMapCopy->m_nCreateTime     = nCreateTime;
        pMapCopy->m_nLastSaveTime   = nLastSaveTime;

        ++nMapCopyCount;
    }

    KGLogPrintf(KGLOG_DEBUG, "[DB] %d map copys loaded !", nMapCopyCount);

    bResult = true;
Exit0:
    if (pQueryRes)
    {
        mysql_free_result(pQueryRes);
        pQueryRes = NULL;
    }
    return bResult;
}

BOOL KDBTools::LoadRoleBlackList()
{
    BOOL        bResult         = false;
    BOOL        bRetCode        = false;
    int         nRetCode        = false;
    MYSQL_RES*  pQueryRes       = NULL;
    MYSQL_ROW   QueryRow        = NULL;
    DWORD       dwRoleID        = 0;
    time_t      nFreezeTime     = 0;
    int         nBlackListCount = 0;

    nRetCode = snprintf(m_szSQL, sizeof(m_szSQL), "select ID, UNIX_TIMESTAMP(FreezeTime) from %s", 
        ROLE_BLACK_LIST_TABLE_NAME
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)sizeof(m_szSQL));

    bRetCode = DoQuery(m_szSQL);
    KGLOG_PROCESS_ERROR(bRetCode);

    pQueryRes = mysql_use_result(m_pDBHandle);
    KGLOG_PROCESS_ERROR(pQueryRes);

    while (true)
    {
        QueryRow = mysql_fetch_row(pQueryRes);
        if (!QueryRow)
            break;

        dwRoleID = strtol(QueryRow[0], NULL, 10);
        nFreezeTime = strtol(QueryRow[1], NULL, 10);

        g_pSO3GameCenter->m_RoleManager.FreezeRole(dwRoleID, nFreezeTime);

        nBlackListCount++;
    }

    KGLogPrintf(KGLOG_DEBUG, "[DB] %d roles in black list loaded !", nBlackListCount);

    bResult = true;
Exit0:
    if (pQueryRes)
    {
        mysql_free_result(pQueryRes);
        pQueryRes = NULL;
    }
    return bResult;
}

BOOL KDBTools::LoadRoleDeleteTable()
{
    BOOL        bResult     = false;
    BOOL        bRetCode    = false;
    int         nRetCode    = false;
    MYSQL_RES*  pQueryRes   = NULL;
    MYSQL_ROW   QueryRow    = NULL;
    DWORD       dwID        = 0;
    time_t      nEndTime    = 0;
    int         nCount      = 0;

    nRetCode = snprintf(
        m_szSQL, sizeof(m_szSQL),
        "select ID, UNIX_TIMESTAMP(EndTime) from %s", 
        ROLE_DELETE_LIST_TABLE_NAME
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)sizeof(m_szSQL));

    bRetCode = DoQuery(m_szSQL);
    KGLOG_PROCESS_ERROR(bRetCode);

    pQueryRes = mysql_use_result(m_pDBHandle);
    KGLOG_PROCESS_ERROR(pQueryRes);

    while (true)
    {
        QueryRow = mysql_fetch_row(pQueryRes);
        if (!QueryRow)
            break;

        dwID        = strtoul(QueryRow[0], NULL, 10);
        nEndTime    = strtol(QueryRow[1], NULL, 10);

        g_pSO3GameCenter->m_RoleManager.RegisterDelRole(dwID, nEndTime);

        nCount++;
    }

    KGLogPrintf(KGLOG_DEBUG, "[DB] %d roles in delete queue !", nCount);

    bResult = true;
Exit0:
    if (pQueryRes)
    {
        mysql_free_result(pQueryRes);
        pQueryRes = NULL;
    }
    return bResult;
}

BOOL KDBTools::LoadTongTable()
{
    BOOL            bResult     = false;
    BOOL            bRetCode    = false;
    int             nRetCode    = 0;
    MYSQL_RES*      pQueryRes   = NULL;
    MYSQL_ROW       QueryRow    = NULL;
    DWORD           dwTongID    = ERROR_ID;
    unsigned long*  pulFieldLen = NULL;
    int             nTongCount  = 0;

    KGLogPrintf(KGLOG_DEBUG, "[DB] tong table loading ... ...");

    nRetCode = snprintf(m_szSQL, sizeof(m_szSQL),
        "select ID, Data from %s",
        TONG_TABLE_NAME
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)sizeof(m_szSQL));

    bRetCode = DoQuery(m_szSQL);
    KGLOG_PROCESS_ERROR(bRetCode);

    pQueryRes = mysql_use_result(m_pDBHandle);
    KGLOG_PROCESS_ERROR(pQueryRes);

    while (true)
    {
        QueryRow = mysql_fetch_row(pQueryRes);
        if (!QueryRow)
            break;

        pulFieldLen = mysql_fetch_lengths(pQueryRes);
        KGLOG_PROCESS_ERROR(pulFieldLen);

        dwTongID = (DWORD)strtoul(QueryRow[0], NULL, 10);
        bRetCode = g_pSO3GameCenter->m_TongManager.LoadTongData(
            dwTongID, 
            (BYTE*)QueryRow[1], (size_t)pulFieldLen[1]
        );

        KGLOG_PROCESS_ERROR(bRetCode);

        ++nTongCount;
    }

    KGLogPrintf(KGLOG_DEBUG, "[DB] %d tongs loaded !", nTongCount);

    bResult = true;
Exit0:
    if (pQueryRes)
    {
        mysql_free_result(pQueryRes);
        pQueryRes = NULL;
    }
    return bResult;
}

#define LOAD_GLOBAL_SYSTEM_VALUE(name) \
    GetGlobalSystemValue(&(g_pSO3GameCenter->m_GlobalSystemValueManager.m_##name.nValue), #name)

BOOL KDBTools::LoadGlobalSystemValue()
{
    BOOL    bResult     = false;
    int     nRetCode    = 0;
    int     nCampScore  = 0;

    nRetCode = GetGlobalSystemValue(&nCampScore, CAMP_SCORE_IN_DB);
    KGLOG_PROCESS_ERROR(nRetCode != -1);
    if (nRetCode == 1)
    {
        g_pSO3GameCenter->m_CampManager.AddScore(nCampScore);
    }

    nRetCode = LOAD_GLOBAL_SYSTEM_VALUE(GameWorldStartTime);
    KGLOG_PROCESS_ERROR(nRetCode != -1);
    if (nRetCode == 0)
    {
        g_pSO3GameCenter->m_GlobalSystemValueManager.m_GameWorldStartTime.nValue = (int)time(NULL);
        g_pSO3GameCenter->m_GlobalSystemValueManager.m_GameWorldStartTime.bUpdate = true;
    }

    nRetCode = LOAD_GLOBAL_SYSTEM_VALUE(MaxPlayerLevel);
    KGLOG_PROCESS_ERROR(nRetCode != -1);

    nRetCode = LOAD_GLOBAL_SYSTEM_VALUE(GameWorldChargeTime);
    KGLOG_PROCESS_ERROR(nRetCode != -1);

    bResult = true;
Exit0:
    return bResult;
}

#undef LOAD_GLOBAL_SYSTEM_VALUE

BOOL KDBTools::LoadGlobalMailTable()
{
    BOOL            bResult          = false;
    BOOL            bRetCode         = false;
    int             nRetCode         = 0;
    MYSQL_RES*      pQueryRes        = NULL;
    MYSQL_ROW       QueryRow         = NULL;
    int             nVersion         = 0;
    DWORD           dwMailID         = 0;
    unsigned long*  pulFieldLen      = NULL;
    time_t          nEndTime         = 0;
    int             nGlobalMailCount = 0;

    KGLogPrintf(KGLOG_DEBUG, "[DB] GlobalMail loading ... ...");

    nRetCode = snprintf(m_szSQL, sizeof(m_szSQL),
        "select Version, MailIndex, UNIX_TIMESTAMP(EndTime), MailInfo from %s",
        GLOBAL_MAIL_TABLE_NAME
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)sizeof(m_szSQL));

    bRetCode = DoQuery(m_szSQL);
    KGLOG_PROCESS_ERROR(bRetCode);

    pQueryRes = mysql_use_result(m_pDBHandle);
    KGLOG_PROCESS_ERROR(pQueryRes);

    while (true)
    {
        QueryRow = mysql_fetch_row(pQueryRes);
        if (!QueryRow)
            break;

        pulFieldLen = mysql_fetch_lengths(pQueryRes);
        KGLOG_PROCESS_ERROR(pulFieldLen);

        nVersion = atoi(QueryRow[0]);
        dwMailID = (DWORD)strtoul(QueryRow[1], NULL, 10);
        nEndTime = strtol(QueryRow[2], NULL, 10);

        bRetCode = g_pSO3GameCenter->m_MailManager.LoadGlobalMail(
            nVersion, dwMailID, nEndTime, (BYTE*)QueryRow[3], (size_t)pulFieldLen[3]
        );
        KGLOG_PROCESS_ERROR(bRetCode);

        nGlobalMailCount++;
    }

    KGLogPrintf(KGLOG_DEBUG, "[DB] %d GlobalMails loaded !", nGlobalMailCount);

    bResult = true;
Exit0:
    if (pQueryRes)
    {
        mysql_free_result(pQueryRes);
        pQueryRes = NULL;
    }
    return bResult;
}

BOOL KDBTools::LoadGlobalCustomDataTable()
{
    BOOL            bResult          = false;
    BOOL            bRetCode         = false;
    int             nRetCode         = 0;
    MYSQL_RES*      pQueryRes        = NULL;
    MYSQL_ROW       QueryRow         = NULL;
    unsigned long*  pulFieldLen      = NULL;
    int             nGlobalDataCount = 0;

    KGLogPrintf(KGLOG_DEBUG, "[DB] GlobalCustomData loading ... ...");

    nRetCode = snprintf(m_szSQL, sizeof(m_szSQL),
        "select Name, Value from %s",
        GLOBAL_CUSTOM_DATA_TABLE_NAME
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)sizeof(m_szSQL));

    bRetCode = DoQuery(m_szSQL);
    KGLOG_PROCESS_ERROR(bRetCode);

    pQueryRes = mysql_use_result(m_pDBHandle);
    KGLOG_PROCESS_ERROR(pQueryRes);

    while (true)
    {
        QueryRow = mysql_fetch_row(pQueryRes);
        if (!QueryRow)
            break;

        pulFieldLen = mysql_fetch_lengths(pQueryRes);
        KGLOG_PROCESS_ERROR(pulFieldLen);

        bRetCode = g_pSO3GameCenter->m_GlobalCustomDataManager.Add(
            QueryRow[0], (BYTE*)QueryRow[1], (size_t)pulFieldLen[1], true
        );
        KGLOG_PROCESS_ERROR(bRetCode);

        nGlobalDataCount++;
    }

    KGLogPrintf(KGLOG_DEBUG, "[DB] %d GlobalCustomData loaded !", nGlobalDataCount);

    bResult = true;
Exit0:
    if (pQueryRes)
    {
        mysql_free_result(pQueryRes);
        pQueryRes = NULL;
    }
    return bResult;
}

BOOL KDBTools::LoadStatDataTable()
{
    BOOL            bResult          = false;
    BOOL            bRetCode         = false;
    int             nRetCode         = 0;
    MYSQL_RES*      pQueryRes        = NULL;
    MYSQL_ROW       QueryRow         = NULL;
    unsigned long*  pulFieldLen      = NULL;
    int             nNameCount       = 0;
    int             nNameID          = 0;
    time_t          nTime            = 0;
    time_t          nStatTime        = 0;

    KGLogPrintf(KGLOG_DEBUG, "[DB] StatDataNameIndex loading ... ...");

    nRetCode = snprintf(m_szSQL, sizeof(m_szSQL),
        "select Name, ID from %s",
        STAT_DATA_NAME_INDEX_TABLE_NAME
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)sizeof(m_szSQL));

    bRetCode = DoQuery(m_szSQL);
    KGLOG_PROCESS_ERROR(bRetCode);

    pQueryRes = mysql_use_result(m_pDBHandle);
    KGLOG_PROCESS_ERROR(pQueryRes);

    while (true)
    {
        QueryRow = mysql_fetch_row(pQueryRes);
        if (!QueryRow)
            break;

        nNameID = (int)strtol(QueryRow[1], NULL, 10);
        bRetCode = g_pSO3GameCenter->m_StatDataManager.LoadDataName(QueryRow[0], nNameID);
        KGLOG_PROCESS_ERROR(bRetCode);

        nNameCount++;
    }

    KGLogPrintf(KGLOG_DEBUG, "[DB] %d StatDataName loaded !", nNameCount);

    nTime = time(NULL);
    nStatTime = (nTime / 3600) * 3600;

    nRetCode = snprintf(m_szSQL, sizeof(m_szSQL),
        "select StatTime, StatData from %s where StatTime = FROM_UNIXTIME(%ld)",
        STAT_DATA_TABLE_NAME, nStatTime
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)sizeof(m_szSQL));

    bRetCode = DoQuery(m_szSQL);
    KGLOG_PROCESS_ERROR(bRetCode);

    pQueryRes = mysql_use_result(m_pDBHandle);
    KGLOG_PROCESS_ERROR(pQueryRes);

    while (true)
    {
        QueryRow = mysql_fetch_row(pQueryRes);
        if (!QueryRow)
            break;

        pulFieldLen = mysql_fetch_lengths(pQueryRes);
        KGLOG_PROCESS_ERROR(pulFieldLen);

        bRetCode = g_pSO3GameCenter->m_StatDataManager.LoadData(
            nStatTime, (BYTE*)QueryRow[1], (size_t)pulFieldLen[1]
        );
        KGLOG_PROCESS_ERROR(bRetCode);
    }

    bResult = true;
Exit0:
    if (pQueryRes)
    {
        mysql_free_result(pQueryRes);
        pQueryRes = NULL;
    }
    return bResult;
}

BOOL KDBTools::LoadActivityTable()
{
    BOOL            bResult         = false;
    BOOL            bRetCode        = false;
    int             nRetCode        = 0;
    MYSQL_RES*      pQueryRes       = NULL;
    MYSQL_ROW       QueryRow        = NULL;
    unsigned long*  pulFieldLen     = NULL;
    int             nActivityCount  = 0;
    KActivityRecord Record;

    KGLogPrintf(KGLOG_DEBUG, "[DB] Activity data loading ... ...");

    nRetCode = snprintf(m_szSQL, sizeof(m_szSQL),
        " select ID, Type, UNIX_TIMESTAMP(Time), Value0, Value1, Value2, Value3, Value4, Value5 from %s; ",
        ACTIVITY_TABLE_NAME
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)sizeof(m_szSQL));

    bRetCode = DoQuery(m_szSQL);
    KGLOG_PROCESS_ERROR(bRetCode);

    pQueryRes = mysql_use_result(m_pDBHandle);
    KGLOG_PROCESS_ERROR(pQueryRes);

    while (true)
    {
        QueryRow = mysql_fetch_row(pQueryRes);
        if (!QueryRow)
            break;

        pulFieldLen = mysql_fetch_lengths(pQueryRes);
        KGLOG_PROCESS_ERROR(pulFieldLen);

        Record.dwRoleID     = strtoul(QueryRow[0], NULL, 10);
        Record.nType        = strtol(QueryRow[1], NULL, 10);
        Record.nEndTime     = strtol(QueryRow[2], NULL, 10);
        Record.anValue[0]   = strtol(QueryRow[3], NULL, 10);
        Record.anValue[1]   = strtol(QueryRow[4], NULL, 10);
        Record.anValue[2]   = strtol(QueryRow[5], NULL, 10);
        Record.anValue[3]   = strtol(QueryRow[6], NULL, 10);
        Record.anValue[4]   = strtol(QueryRow[7], NULL, 10);
        Record.anValue[5]   = strtol(QueryRow[8], NULL, 10);
        Record.bUpdate      = false;

        bRetCode = g_pSO3GameCenter->m_ActivityManager.Add(Record);
        KGLOG_PROCESS_ERROR(bRetCode);

        nActivityCount++;
    }

    KGLogPrintf(KGLOG_DEBUG, "[DB] %d Activity data loaded !", nActivityCount);

    bResult = true;
Exit0:
    if (pQueryRes)
    {
        mysql_free_result(pQueryRes);
        pQueryRes = NULL;
    }
    return bResult;
}

BOOL KDBTools::LoadAntiFarmerTable()
{
    BOOL            bResult         = false;
    BOOL            bRetCode        = false;
    int             nRetCode        = 0;
    MYSQL_RES*      pQueryRes       = NULL;
    MYSQL_ROW       QueryRow        = NULL;
    unsigned long*  pulFieldLen     = NULL;
    const char*     pszAccount      = NULL;
    size_t          uStrLen         = 0;
    time_t          nPunishEndTime  = 0;
    int             nCount          = 0;

    KGLogPrintf(KGLOG_DEBUG, "[DB] AntiFarmer data loading ... ...");

    nRetCode = snprintf(m_szSQL, sizeof(m_szSQL),
        " select Account, unix_timestamp(PunishEndTime) from %s; ",
        ANTI_FARMER_TABLE_NAME
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)sizeof(m_szSQL));

    bRetCode = DoQuery(m_szSQL);
    KGLOG_PROCESS_ERROR(bRetCode);

    pQueryRes = mysql_use_result(m_pDBHandle);
    KGLOG_PROCESS_ERROR(pQueryRes);

    while (true)
    {
        QueryRow = mysql_fetch_row(pQueryRes);
        if (!QueryRow)
            break;

        pulFieldLen = mysql_fetch_lengths(pQueryRes);
        KGLOG_PROCESS_ERROR(pulFieldLen);

        pszAccount = QueryRow[0];
        KGLOG_PROCESS_ERROR(pszAccount);

        uStrLen = strlen(pszAccount);
        KGLOG_PROCESS_ERROR(uStrLen < NAME_LEN);

        nPunishEndTime = strtol(QueryRow[1], NULL, 10);

        bRetCode = g_pSO3GameCenter->m_AntiFarmerManager.AddFarmerRecord(pszAccount, nPunishEndTime);
        KGLOG_PROCESS_ERROR(bRetCode);

        ++nCount;
    }

    KGLogPrintf(KGLOG_DEBUG, "[DB] %d AntiFarmer data loaded !", nCount);

    bResult = true;
Exit0:
    if (pQueryRes)
    {
        mysql_free_result(pQueryRes);
        pQueryRes = NULL;
    }
    return bResult;
}

BOOL KDBTools::LoadMentorTable()
{
    BOOL            bResult         = false;
    int             nRetCode        = 0;
    MYSQL_RES*      pQueryRes       = NULL;
    MYSQL_ROW       QueryRow        = NULL;
    unsigned long*  pulFieldLen     = NULL;
    DWORD           dwMentorID      = ERROR_ID;
    DWORD           dwApprenticeID  = ERROR_ID;
    int             nMentorCount    = 0;

    KGLogPrintf(KGLOG_DEBUG, "[DB] Mentor data loading ... ...");

    nRetCode = snprintf(m_szSQL, sizeof(m_szSQL),
        " select MentorID, ApprenticeID, Data from %s; ",
        MENTOR_TABLE_NAME
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)sizeof(m_szSQL));

    nRetCode = DoQuery(m_szSQL);
    KGLOG_PROCESS_ERROR(nRetCode);

    pQueryRes = mysql_use_result(m_pDBHandle);
    KGLOG_PROCESS_ERROR(pQueryRes);

    while (true)
    {
        BYTE*   pbyData     = NULL;
        size_t  uDataLen    = 0;

        QueryRow = mysql_fetch_row(pQueryRes);
        if (!QueryRow)
            break;

        pulFieldLen = mysql_fetch_lengths(pQueryRes);
        KGLOG_PROCESS_ERROR(pulFieldLen);

        uDataLen = (size_t)pulFieldLen[2];

        dwMentorID      = (DWORD)strtoul(QueryRow[0], NULL, 10);
        dwApprenticeID  = (DWORD)strtoul(QueryRow[1], NULL, 10);
        pbyData         = (BYTE*)QueryRow[2];

        nRetCode = g_pSO3GameCenter->m_MentorCenter.Load(
            dwMentorID, dwApprenticeID, pbyData, uDataLen
        );
        KGLOG_PROCESS_ERROR(nRetCode);

        nMentorCount++;
    }

    KGLogPrintf(KGLOG_DEBUG, "[DB] %d pairs of Mentor records loaded !", nMentorCount);

    bResult = true;
Exit0:
    if (pQueryRes)
    {
        mysql_free_result(pQueryRes);
        pQueryRes = NULL;
    }
    return bResult;
}

BOOL KDBTools::LoadRenameRoleTable()
{
    BOOL        bResult     = false;
    int         nRetCode    = 0;
    MYSQL_RES*  pQueryRes   = NULL;
    MYSQL_ROW   QueryRow    = NULL;
    DWORD       dwRoleID    = ERROR_ID;
    int         nCount      = 0;

    KGLogPrintf(KGLOG_DEBUG, "[DB] RenameRole data loading ... ...");

    nRetCode = snprintf(
        m_szSQL, sizeof(m_szSQL), " select RoleID from %s; ",
        RENAME_ROLE_TABLE_NAME
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)sizeof(m_szSQL));

    nRetCode = DoQuery(m_szSQL);
    KGLOG_PROCESS_ERROR(nRetCode);

    pQueryRes = mysql_use_result(m_pDBHandle);
    KGLOG_PROCESS_ERROR(pQueryRes);

    while (true)
    {
        BYTE*   pbyData     = NULL;
        size_t  uDataLen    = 0;

        QueryRow = mysql_fetch_row(pQueryRes);
        if (!QueryRow)
            break;

        dwRoleID = (DWORD)strtoul(QueryRow[0], NULL, 10);
        g_pSO3GameCenter->m_RoleManager.RecordRenameRole(dwRoleID);

        nCount++;
    }

    KGLogPrintf(KGLOG_DEBUG, "[DB] %d RenameRole records loaded !", nCount);
    bResult = true;
Exit0:
    if (pQueryRes)
    {
        mysql_free_result(pQueryRes);
        pQueryRes = NULL;
    }
    return bResult;
}

int KDBTools::GetGlobalSystemValue(int* pnRetValue, const char cszKey[])
{
    int             nResult     = -1;
    int             nRetCode    = 0;
    MYSQL_RES*      pQueryRes   = NULL;
    MYSQL_ROW       QueryRow    = NULL;
    my_ulonglong    ullRows     = 0;

    assert(pnRetValue);

    nRetCode = snprintf(
        m_szSQL, sizeof(m_szSQL),
        " select Value from %s where Name = '%s'; ",
        GLOBAL_SYSTEM_VALUE_TABLE_NAME, cszKey
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)sizeof(m_szSQL));

    nRetCode = DoQuery(m_szSQL);
    KGLOG_PROCESS_ERROR(nRetCode);

    pQueryRes = mysql_store_result(m_pDBHandle);
    KGLOG_PROCESS_ERROR(pQueryRes);

    ullRows = mysql_num_rows(pQueryRes);
    if (ullRows == 0)
    {
        nResult = 0;
        goto Exit0;
    }

    KGLOG_PROCESS_ERROR(ullRows == 1);
    
    QueryRow = mysql_fetch_row(pQueryRes);
    KGLOG_PROCESS_ERROR(QueryRow);
    
    *pnRetValue = (int)strtol(QueryRow[0], NULL, 10);

    nResult = 1;
Exit0:
    if (pQueryRes)
    {
        mysql_free_result(pQueryRes);
        pQueryRes = NULL;
    }
    return nResult;
}

BOOL KDBTools::DoQuery(const char cszSQL[])
{
    BOOL     bResult            = false;
    BOOL     bRetCode           = false;
    unsigned ulMySQLErrorCode   = 0;

    assert(cszSQL);
    assert(m_pDBHandle);
    
    bRetCode = MySQL_Query(m_pDBHandle, cszSQL, &ulMySQLErrorCode);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    if (ulMySQLErrorCode != 0 && ulMySQLErrorCode != ER_DUP_ENTRY) // 主键冲突不用断开链接
    {
        MySQL_Disconnect(m_pDBHandle);
        m_pDBHandle = NULL;
    }
    return bResult;
}
