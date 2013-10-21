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
    int Init(KSO3RoleDBUpdater* pSO3RoleDBUpdater, KG_ROLEDBCONVERSION_CONFIG& refconfig);
    int LoadAndUpdateTable(char szTableName[], char szTimeFileName[]);
    int LoadMailBoxTable();
    int InsertRoleDateToNewDB(char szTableName[], char szTimeFileName[]);
    int InsertMailBoxDateToNewDB(DWORD dwID);
    void UnInit();
    int CheckRoleTable();
    BOOL CheckMailTable();
    int UpdateRoleTable(char szTableName[]);
    int UpdateMailBoxTable(DWORD dwID);
    int LoadAndCompareTable(char szTableName[], char szTimeFileName[]);
    int GetRoleAllInfoToCompare(BYTE* byBaseInfoData, unsigned long ulBaseInfoSize, BYTE* byExtInfoData, unsigned long ulExtInfoSize, char szTableName[], char szTimeFileName[], DWORD dwID);

private:
    char m_szSQL[MAX_SQL_COMMAND_LEN];
    KSO3RoleDBUpdater* m_pSO3RoleDBUpdater;
};
#endif //_KSO3ROLEDBTOOLS_H