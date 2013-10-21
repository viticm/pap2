///////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
// 
// FileName : KSO3RoleDBTools.h
// Creator  : liuzhibiao
// Date     : 2009-5-26
// Comment  : v0.1
//            v0.1  2009-5-26   Create  	
///////////////////////////////////////////////////////////////

#ifndef _KSO3ROLEDBTOOLS_H
#define _KSO3ROLEDBTOOLS_H

#include "KDBBase.h"

#define MAX_SQL_COMMAND_LEN 102400

class KSO3RoleDBUpdater;

class KSO3RoleDBTools: public KDBBase
{
public:
    int  Init(KSO3RoleDBUpdater* pSO3RoleDBUpdater, KG_ROLEDBCONVERSION_CONFIG& refconfig);
    int  LoadAndUpdateTable(char szTableName[], char szTimeFileName[]);
    int  LoadMailBoxTable();
    int  InsertRoleDataToNewDB(char szTableName[], char szTimeFileName[]);
    int  InsertMailBoxDataToNewDB(DWORD dwID);
    int  InsertFellowShipDataToNewDB(DWORD dwID, BYTE* pbyFellowshipData, unsigned long ulDataLen);
    int  InsertMapCopyDataToNewDB(BYTE* byMapID, unsigned long ulMapIDLen,
        BYTE* byMapCopyIndex, unsigned long ulMapCopyIndexLen,
        BYTE* bySceneData, unsigned long ulSceneDataLen,
        BYTE* byCreateTime, unsigned long ulCreateTimeLen,
        BYTE* byLastModify, unsigned long ulLastModifyLen
    );
    int InsertPQDataToNewDB(DWORD dwID, BYTE* pbyData, unsigned long ulDataLen);
    int InsertRoleBlackListToNewDB(DWORD dwID, BYTE* pbyRoleBlackListData, unsigned long ulDataLen);
    int InsertSequenceDataToNewDB(BYTE* bySeqname, unsigned long ulSeqnameLen,
        BYTE* byStarid, unsigned long ulStaridLen,
        BYTE* byCurrentID, unsigned long ulCurrentIDLen,
        BYTE* byMaxID, unsigned long ulMaxIDLen,
        BYTE* byGroupName, unsigned long ulGroupName
    );
    int InsertTongToNewDB(DWORD dwID, BYTE* byData, unsigned long ulDataLen);
    int InsertToNewDBAuction(
        BYTE* byData0, unsigned long ulDataLen0,
        BYTE* byData1, unsigned long ulDataLen1,
        BYTE* byData2, unsigned long ulDataLen2,
        BYTE* byData3, unsigned long ulDataLen3,
        BYTE* byData4, unsigned long ulDataLen4,
        BYTE* byData5, unsigned long ulDataLen5,
        BYTE* byData6, unsigned long ulDataLen6,
        BYTE* byData7, unsigned long ulDataLen7,
        BYTE* byData8, unsigned long ulDataLen8,
        BYTE* byData9, unsigned long ulDataLen9,
        BYTE* byData10, unsigned long ulDataLen10,
        BYTE* byData11, unsigned long ulDataLen11,
        BYTE* byData12, unsigned long ulDataLen12
    );
    int InsertToNewDBGlobalCustomData(
        BYTE* byData0, unsigned long ulDataLen0,
        BYTE* byData1, unsigned long ulDataLen1
    );
    int InsertToNewDBGlobalMail(
        BYTE* byData0, unsigned long ulDataLen0,
        BYTE* byData1, unsigned long ulDataLen1,
        BYTE* byData2, unsigned long ulDataLen2
    );
    void UnInit();
    int  CheckRoleTable();
    BOOL CheckMailTable();
    int  UpdateRoleTable(char szTableName[]);
    int  UpdateMailBoxTable(DWORD dwID);
    
    BOOL LoadAndInsertFellowshipTable();
    BOOL LoadAndInsertMapCopyTable();
    BOOL LoadAndInsertPQTable();
    BOOL LoadAndInsertRoleBlackListTable();
    BOOL LoadAndInsertSequenceTable();
    BOOL LoadAndInsertTongTable();
    BOOL LoadAndInsertAuctionTable();
    //BOOL LoadAndInsertGlobalParamTable();
    BOOL LoadAndInsertGlobalMailTable();
    BOOL LoadAndInsertGlobalCustomDataTable();

    BOOL CheckDatabase();
    BOOL CheckPQTable();
    BOOL CheckAuctionTable();
    //BOOL CheckGlobalParamTable();
    BOOL CheckRoleBlakListTable();
    BOOL CheckMapCopyTable();
    BOOL CheckFellowshipTable();
    BOOL CheckTongTable();
    BOOL CheckGlobalMailTable();
    BOOL CheckSequenceTable();
    BOOL CheckGlobalCustomDataTable();
    BOOL CheckGlobalSystemValueTable();

private:
    char m_szSQL[MAX_SQL_COMMAND_LEN];
    KSO3RoleDBUpdater* m_pSO3RoleDBUpdater;
};
#endif //_KSO3ROLEDBTOOLS_H