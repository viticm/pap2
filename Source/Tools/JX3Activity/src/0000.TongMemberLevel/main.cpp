////////////////////////////////////////////////////////////////////////////////
//  Copyright(c) Kingsoft
//
//  FileName    : main.cpp
//  Version     : 1.0
//  Creator     : Hu Hao
//  Create Date : 2009-9-7  11:10:14
//  Comment     :
////////////////////////////////////////////////////////////////////////////////

#include "KG_Public.h"
#include <vector>
#include <string>
#include <algorithm>

///////////////////////////////////////////////////////////////////////////////
const unsigned KG_ACTIVITY_ID      = 0;
const unsigned KG_TOP_LEVEL_NUMBER = 50;

///////////////////////////////////////////////////////////////////////////////
struct KG_TONG_INFO
{
    PARSEHANDLE hTongHandle;
    char        szTongName[64];
    char        szMasterName[64];
    char        szMasterAccount[64];
    int         nTotalMemberCount;
    int         nTopLevelMemberCount; // level >=KG_TOP_LEVEL_NUMBER
};

bool CompareByTopLevelMemberCount(const KG_TONG_INFO &crLeft, const KG_TONG_INFO &crRight)
{
    return crLeft.nTopLevelMemberCount > crRight.nTopLevelMemberCount;
}

typedef vector<KG_TONG_INFO> KG_TONG_INFO_VECTOR;

///////////////////////////////////////////////////////////////////////////////
int _GetTongInfo(MYSQL *pMySQL, PARSEHANDLE hTongHandle, KG_TONG_INFO *pRetTongInfo);

int _GetRoleInfo(
    MYSQL *pMySQL, unsigned long dwRoleID, string *pstrRetRoleName,
    string *pstrRetRoleAccount, int *pnRetLevel
);

int _RewardTongMembers(MYSQL *pMySQL, PARSEHANDLE hTongHandle, int nRewardLevel);

/////////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{
    int nResult  = false;
    int nRetCode = false;
    int nKGLogInitFlag = false;
    KGLOG_PARAM LogParam  = {"Logs", "0000.TongMemberLevel", KGLOG_OPTION_FILE, 100000};
    KG_MYSQL_PARAM MySQLParam;
    MYSQL *pMySQL = NULL;
    KG_TONG_HANDLE_LIST TongHandleList;
    KG_TONG_HANDLE_LIST::iterator itTongHandle;
    KG_TONG_INFO TongInfo;
    KG_TONG_INFO_VECTOR TongInfoVector;
    KG_TONG_INFO_VECTOR::iterator itTongInfo;
    string strResultFileName;
    FILE *fpResult = NULL;
    int nRewardLevel = -1;
    string strRewardInfo;

    puts("Initializing runtime environment ...");

    nRetCode = KG_SetRuntimeEnvironment(argv[0]);
    KG_PROCESS_ERROR(nRetCode);

    nRetCode = KGLogInit(LogParam, NULL);
    KG_PROCESS_ERROR(nRetCode);
    nKGLogInitFlag = true;

    nRetCode = KG_LoadMySQLDatabaseParameter("0000.TongMemberLevel.ini", &MySQLParam);
    KGLOG_PROCESS_ERROR(nRetCode && "KG_LoadMySQLDatabaseParameter()");

    pMySQL = KG_ConnectMySQLDatabase(MySQLParam);
    KGLOG_PROCESS_ERROR(pMySQL);

    nRetCode = KG_CreateRewardTable(pMySQL);
    KGLOG_PROCESS_ERROR(nRetCode && "KG_CreateRewardTable()");

    nRetCode = KG_RemoveOldRewardData(pMySQL, KG_ACTIVITY_ID);
    KGLOG_PROCESS_ERROR(nRetCode && "KG_RemoveOldRewardData()");

    nRetCode = KG_GetTongHandleList(pMySQL, &TongHandleList);
    KGLOG_PROCESS_ERROR(nRetCode && "_GetTongInfoList()");

    puts("Get Tong list successfully.");

    TongInfoVector.reserve(TongHandleList.size());
    for (itTongHandle = TongHandleList.begin(); itTongHandle != TongHandleList.end(); ++itTongHandle)
    {
        nRetCode = _GetTongInfo(pMySQL, *itTongHandle, &TongInfo);
        KGLOG_PROCESS_ERROR(nRetCode && "_GetTongInfo()");
        
        TongInfoVector.push_back(TongInfo);
        printf("Get Tong `%s` information successfully.\n", TongInfo.szTongName);
    }

    // Sort the vector by the top level member count.
    sort(TongInfoVector.begin(), TongInfoVector.end(), CompareByTopLevelMemberCount);

    //------------------------- Output the result and reward tong members ------------------------------
    strResultFileName = string("0000-") + MySQLParam.szServer + "-" + MySQLParam.szDatabase + ".txt";
    fpResult = fopen(strResultFileName.c_str(), "wb");
    KGLOG_PROCESS_ERROR(fpResult);

    nRetCode = fprintf(
        fpResult,
        "帮会名称             帮主名称             帮主账号                       帮会总人数   满级帮众人数   所获奖励\r\n"
        "-------------------------------------------------------------------------------------------------------------\r\n"
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0);

    for (itTongInfo = TongInfoVector.begin(); itTongInfo != TongInfoVector.end(); ++itTongInfo)
    {
        if (itTongInfo->nTopLevelMemberCount >= 50)
        {
            nRewardLevel = 3;
            strRewardInfo = "帮会所有成员将会获得称号前缀<灭世>";
        }
        else if (itTongInfo->nTopLevelMemberCount >= 40)
        {
            nRewardLevel = 2;
            strRewardInfo = "帮会所有成员将会获得称号前缀<平天>";
        }
        else if (itTongInfo->nTopLevelMemberCount >= 30)
        {
            nRewardLevel = 1;
            strRewardInfo = "帮会所有成员将会获得称号前缀<极武>";
        }
        else if (itTongInfo->nTopLevelMemberCount >= 20)
        {
            nRewardLevel = 0;
            strRewardInfo = "帮会所有成员将会获得称号前缀<霸天>";
        }
        else
        {
            nRewardLevel = -1;
            strRewardInfo = "无";
        }
        
        nRetCode = fprintf(
            fpResult, "%-20s %-20s %-30s %-12d %-14d %-20s\r\n",
            itTongInfo->szTongName, itTongInfo->szMasterName, itTongInfo->szMasterAccount,
            itTongInfo->nTotalMemberCount, itTongInfo->nTopLevelMemberCount, strRewardInfo.c_str()
        );
        KGLOG_PROCESS_ERROR(nRetCode > 0);

        if (nRewardLevel >= 0)
        {
            nRetCode = _RewardTongMembers(pMySQL, itTongInfo->hTongHandle, nRewardLevel);
            KGLOG_PROCESS_ERROR(nRetCode && "_RewardTongMembers()");
        }
        
        printf("Reward Tong `%s`: %s.\n", itTongInfo->szTongName, strRewardInfo.c_str());
    }
    //----------------------------------------------------------------------------------------------
    nResult = true;
Exit0:
    for (itTongHandle = TongHandleList.begin(); itTongHandle != TongHandleList.end(); ++itTongHandle)
    {
        PARSEHANDLE hTongHandle = *itTongHandle;
        if (hTongHandle)
        {
            EndTongParse(hTongHandle);
            hTongHandle = NULL;
        }
    }
    if (fpResult)
    {
        fclose(fpResult);
        fpResult = NULL;
    }
    if (pMySQL)
    {
        mysql_close(pMySQL);
        pMySQL = NULL;
    }
    if (nKGLogInitFlag)
    {
        KGLogUnInit(NULL);
        nKGLogInitFlag = false;
    }
    if (nResult)
    {
        puts("Operation had been done successfully.");
        KGLogPrintf(KGLOG_INFO, "Operation had been done successfully.");
    }
    else
    {
        remove(strResultFileName.c_str());
        puts("Operation failed!!");
        KGLogPrintf(KGLOG_INFO, "Operation failed!!");
    }
    puts("Press any key to quit.");
    getchar();
    return nResult ? 0 : 1;
}

int _GetTongInfo(MYSQL *pMySQL, PARSEHANDLE hTongHandle, KG_TONG_INFO *pRetTongInfo)
{
	int nResult  = false;
	int nRetCode = false;
    unsigned long dwMasterID = 0;
    unsigned long dwRoleID = 0;
    string strRoleName;
    string strRoleAccount;
    int nRoleLevel = 0;
    int nFund = 0;                          // No use
    int nLevel = 0;                         // No use
    int nDevelopmentPoint = 0;              // No use
    unsigned short wMaxMemeberCount = 0;    // No use
    unsigned char byTongState = 0;          // No use
    time_t lStateTimer = 0;                 // No use
    unsigned char byCamp = 0;               // No use
    int nGroupID = 0;                       // No use
    time_t lJoinTime = 0;                   // No use
    time_t lLastOfflineTime = 0;            // No use
    char szRemark[64];                      // No use
    int nSalary = 0;                        // No use

    ASSERT(pMySQL);
    ASSERT(hTongHandle);
    ASSERT(pRetTongInfo);

    // Get tong name and master ID.
    nRetCode = GetTongBaseInfo(
        hTongHandle, pRetTongInfo->szTongName, sizeof(pRetTongInfo->szTongName), dwMasterID, 
        nFund, nLevel, nDevelopmentPoint, wMaxMemeberCount, byTongState, lStateTimer, byCamp
    );
    KGLOG_PROCESS_ERROR(nRetCode && "GetTongBaseInfo()");

    // Get total member count.
    nRetCode = GetTongMemberCount(hTongHandle, pRetTongInfo->nTotalMemberCount);
    KGLOG_PROCESS_ERROR(nRetCode && "GetTongMemberCount()");

    // Get top level member count and master name.
    pRetTongInfo->nTopLevelMemberCount = 0;
    pRetTongInfo->szMasterName[0]    = '\0';
    pRetTongInfo->szMasterAccount[0] = '\0';
    for (int i = 0; i < pRetTongInfo->nTotalMemberCount; i++)
    {
        nRetCode = GetTongMemberInfo(
            hTongHandle, i, dwRoleID, nGroupID, lJoinTime, lLastOfflineTime,
            szRemark, sizeof(szRemark), nSalary
        );
        KGLOG_PROCESS_ERROR(nRetCode && "GetTongMemberInfo()");

        nRetCode = _GetRoleInfo(pMySQL, dwRoleID, &strRoleName, &strRoleAccount, &nRoleLevel);
        KGLOG_PROCESS_ERROR(nRetCode && "_GetRoleInfo()");

        if (nRoleLevel >= KG_TOP_LEVEL_NUMBER)
            pRetTongInfo->nTopLevelMemberCount++;

        if (dwRoleID == dwMasterID)
        {
            ASSERT(pRetTongInfo->szMasterName[0] == '\0');
            strncpy(pRetTongInfo->szMasterName, strRoleName.c_str(), sizeof(pRetTongInfo->szMasterName));
            KGLOG_PROCESS_ERROR(pRetTongInfo->szMasterName[sizeof(pRetTongInfo->szMasterName) - 1] == '\0');

            ASSERT(pRetTongInfo->szMasterAccount[0] == '\0');
            strncpy(pRetTongInfo->szMasterAccount, strRoleAccount.c_str(), sizeof(pRetTongInfo->szMasterAccount));
            KGLOG_PROCESS_ERROR(pRetTongInfo->szMasterAccount[sizeof(pRetTongInfo->szMasterAccount) - 1] == '\0');
        }
    }

    // Get tong handle.
    pRetTongInfo->hTongHandle = hTongHandle;

	nResult = true;
Exit0:
	return nResult;
}

int _GetRoleInfo(
    MYSQL *pMySQL, unsigned long dwRoleID, string *pstrRetRoleName,
    string *pstrRetRoleAccount, int *pnRetLevel        
)
{
	int nResult  = false;
	int nRetCode = false;
    char szQuery[128];
    MYSQL_RES *pRoleInfo = NULL;
    MYSQL_ROW Row = NULL;
    unsigned long *pulLengths = NULL;
    PARSEHANDLE hRoleHandle = NULL;

    ASSERT(pMySQL);
    ASSERT(pstrRetRoleName);
    ASSERT(pstrRetRoleAccount);
    ASSERT(pnRetLevel);

    nRetCode = snprintf(
        szQuery, sizeof(szQuery),
        "select `RoleName`, `Account`, `BaseInfo` from `Role` where `ID` = %d", dwRoleID
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0);
    KGLOG_PROCESS_ERROR(nRetCode < sizeof(szQuery));

    nRetCode = mysql_real_query(pMySQL, szQuery, (unsigned long)strlen(szQuery));
    KGD_MYSQL_PROCESS_ERROR(nRetCode == 0, pMySQL, szQuery);

    pRoleInfo = mysql_store_result(pMySQL);
    KGD_MYSQL_PROCESS_ERROR(pRoleInfo, pMySQL, "mysql_store_result()");

    nRetCode = (int)mysql_num_rows(pRoleInfo);
    KGLOG_PROCESS_ERROR(nRetCode == 1 && "the role not found.");

    Row = mysql_fetch_row(pRoleInfo);
    KGD_MYSQL_PROCESS_ERROR(Row, pMySQL, "mysql_fetch_row()");

    *pstrRetRoleName    = Row[0];
    *pstrRetRoleAccount = Row[1];

    pulLengths  = mysql_fetch_lengths(pRoleInfo);
    KGD_MYSQL_PROCESS_ERROR(pulLengths, pMySQL, "mysql_fetch_lengths()");

    hRoleHandle = ParseRoleBaseData((unsigned char *)Row[2], pulLengths[2]);
    KGLOG_PROCESS_ERROR(hRoleHandle && "ParseRoleBaseData()");

    nRetCode = GetRoleLevel(hRoleHandle, *pnRetLevel);
    KGLOG_PROCESS_ERROR(nRetCode && "GetRoleLevel()");

	nResult = true;
Exit0:
    if (hRoleHandle)
    {
        EndRoleParse(hRoleHandle);
        hRoleHandle = NULL;
    }
    if (pRoleInfo)
    {
        mysql_free_result(pRoleInfo);
        pRoleInfo = NULL;
    }
	return nResult;
}

int _RewardTongMembers(MYSQL *pMySQL, PARSEHANDLE hParseHandle, int nRewardLevel)
{
    int nResult  = false;
    int nRetCode = false;
    int nTotalMemberCount = 0;
    unsigned long dwRoleID = 0;
    unsigned long dwMasterID = 0;           // No use
    int nFund = 0;                          // No use
    int nLevel = 0;                         // No use
    int nDevelopmentPoint = 0;              // No use
    unsigned short wMaxMemeberCount = 0;    // No use
    unsigned char byTongState = 0;          // No use
    unsigned char byCamp = 0;               // No use
    int nGroupID = 0;                       // No use
    time_t lJoinTime = 0;                   // No use
    time_t lLastOfflineTime = 0;            // No use
    char szRemark[64];                      // No use
    int nSalary = 0;                        // No use

    ASSERT(pMySQL);
    ASSERT(hParseHandle);

    // Get total member count.
    nRetCode = GetTongMemberCount(hParseHandle, nTotalMemberCount);
    KGLOG_PROCESS_ERROR(nRetCode && "GetTongMemberCount()");

    for (int i = 0; i < nTotalMemberCount; i++)
    {
        nRetCode = GetTongMemberInfo(
            hParseHandle, i, dwRoleID, nGroupID, lJoinTime, lLastOfflineTime, 
            szRemark, sizeof(szRemark), nSalary
        );
        KGLOG_PROCESS_ERROR(nRetCode && "GetTongMemberInfo()");

        nRetCode = KG_RewardRole(
            pMySQL, dwRoleID, KG_ACTIVITY_ID, "2009-10-29 08:00:00", nRewardLevel, 0, 0, 0, 0, 0
        );
        KGLOG_PROCESS_ERROR(nRetCode && "KG_RewardRole()");
    }

	nResult = true;
Exit0:
	return nResult;
}
