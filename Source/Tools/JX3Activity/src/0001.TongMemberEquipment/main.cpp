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
#include <map>
#include <algorithm>
#include "Engine/FileType.h"

///////////////////////////////////////////////////////////////////////////////
const unsigned KG_ACTIVITY_ID      = 1;
enum EQUIPMENT_TYPE
{
    EQUIPMENT_TYPE_WEAPON  = 6,
    EQUIPMENT_TYPE_ARMOR   = 7,
    EQUIPMENT_TYPE_TRINKET = 8
};

///////////////////////////////////////////////////////////////////////////////
struct KG_TONG_INFO
{
    PARSEHANDLE hTongHandle;
    char        szTongName[64];
    char        szMasterName[64];
    char        szMasterAccount[64];
    int         nTotalMemberCount;
    int         nTotalEquipmentLevel;
};

bool CompareByTotalEquipmentLevel(const KG_TONG_INFO &crLeft, const KG_TONG_INFO &crRight)
{
    return crLeft.nTotalEquipmentLevel > crRight.nTotalEquipmentLevel;
}

typedef std::vector<KG_TONG_INFO> KG_TONG_INFO_VECTOR;

typedef std::map<int, float>                  KG_EQUIPMENT_LEVEL_MAP; // Map with the ID with equipment level.
typedef std::map<int, KG_EQUIPMENT_LEVEL_MAP> KG_EQUIPMENT_TYPE_MAP;  // Map with equipment type with KG_EQUIPMENT_LEVEL_MAP.

///////////////////////////////////////////////////////////////////////////////
int _LoadEquipmentInformation(KG_EQUIPMENT_TYPE_MAP *pRetEquipmentTypeMap);

int _LoadItemTabFile(const char cszTabFileName[], KG_EQUIPMENT_LEVEL_MAP *pRetEquipmentLevelMap);

int _GetTongInfo(
    MYSQL *pMySQL, const KG_EQUIPMENT_TYPE_MAP &crEquipmentTypeMap, PARSEHANDLE hTongHandle,
    KG_TONG_INFO *pRetTongInfo
);

int _GetRoleInfo(
    MYSQL *pMySQL, const KG_EQUIPMENT_TYPE_MAP &crEquipmentTypeMap, DWORD dwRoleID,
    string *pstrRetRoleName, string *pstrRetRoleAccount, float *pfEquipmentPoints
);

int _IsArmedEquipment(int nBoxIndex, int nItemIndex);

int _RewardTongMembers(MYSQL *pMySQL, PARSEHANDLE hTongHandle, int nRewardLevel);

/////////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{
    int nResult  = false;
    int nRetCode = false;
    int nKGLogInitFlag = false;
    KGLOG_PARAM LogParam  = {"Logs", "0001.TongMemberEquipment", KGLOG_OPTION_FILE, 100000};
    KG_MYSQL_PARAM MySQLParam;
    MYSQL *pMySQL = NULL;
    KG_TONG_HANDLE_LIST TongHandleList;
    KG_TONG_HANDLE_LIST::iterator itTongHandle;
    KG_TONG_INFO TongInfo;
    KG_TONG_INFO_VECTOR TongInfoVector;
    KG_TONG_INFO_VECTOR::iterator itTongInfo;
    KG_EQUIPMENT_TYPE_MAP EquipmentTypeMap;
    string strResultFileName;
    FILE *fpResult = NULL;
    int nTongIndex = 0;
    int nPreTongTotalEquipmentLevel = 0;
    int nPreTongRank = 0;
    int nTongRank    = 0;
    int nRewardLevel = -1;
    string strRewardInfo;

    puts("Initializing runtime environment ...");

    nRetCode = KG_SetRuntimeEnvironment(argv[0]);
    KG_PROCESS_ERROR(nRetCode);

    nRetCode = KGLogInit(LogParam, NULL);
    KG_PROCESS_ERROR(nRetCode);
    nKGLogInitFlag = true;

    nRetCode = _LoadEquipmentInformation(&EquipmentTypeMap);
    KGLOG_PROCESS_ERROR(nRetCode && "_LoadEquipmentInformation()");

    nRetCode = KG_LoadMySQLDatabaseParameter("0001.TongMemberEquipment.ini", &MySQLParam);
    KGLOG_PROCESS_ERROR(nRetCode && "KG_LoadMySQLDatabaseParameter()");

    pMySQL = KG_ConnectMySQLDatabase(MySQLParam);
    KGLOG_PROCESS_ERROR(pMySQL);

    nRetCode = KG_CreateRewardTable(pMySQL);
    KGLOG_PROCESS_ERROR(nRetCode && "KG_CreateRewardTable()");

    nRetCode = KG_RemoveOldRewardData(pMySQL, KG_ACTIVITY_ID);
    KGLOG_PROCESS_ERROR(nRetCode && "KG_RemoveOldRewardData()");

    nRetCode = KG_GetTongHandleList(pMySQL, &TongHandleList);
    KGLOG_PROCESS_ERROR(nRetCode && "KG_GetTongHandleList()");

    TongInfoVector.reserve(TongHandleList.size());
    for (itTongHandle = TongHandleList.begin(); itTongHandle != TongHandleList.end(); ++itTongHandle)
    {
        nRetCode = _GetTongInfo(pMySQL, EquipmentTypeMap, *itTongHandle, &TongInfo);
        KGLOG_PROCESS_ERROR(nRetCode && "_GetTongInfo()");

        TongInfoVector.push_back(TongInfo);
        printf("Get Tong `%s` information successfully.\n", TongInfo.szTongName);
    }

    // Sort the tong vector by total item level.
    sort(TongInfoVector.begin(), TongInfoVector.end(), CompareByTotalEquipmentLevel);

    //------------------------- Output the result and reward tong members --------------------------
    strResultFileName = string("0001-") + MySQLParam.szServer + "-" + MySQLParam.szDatabase + ".txt";
    fpResult = fopen(strResultFileName.c_str(), "wb");
    KGLOG_PROCESS_ERROR(fpResult);

    nRetCode = fprintf(
        fpResult,
        "帮会名称             帮主名称             帮主账号                       帮会总人数   帮会总得分   帮会排名    所获奖励\r\n"
        "-----------------------------------------------------------------------------------------------------------------------\r\n"
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0);

    nPreTongRank = 1;   // the rank is start with 1.
    nTongIndex   = 1;   // the rank is start with 1.
    for (itTongInfo = TongInfoVector.begin(); itTongInfo != TongInfoVector.end(); ++itTongInfo)
    {
        if (itTongInfo->nTotalEquipmentLevel == nPreTongTotalEquipmentLevel)
            // With same level with previous tong, then the rank is same with previous tong.
            nTongRank = nPreTongRank;
        else 
            // Not with same level with previous tong, then rank is the index.
            nTongRank = nTongIndex;

        nPreTongTotalEquipmentLevel = itTongInfo->nTotalEquipmentLevel;
        nPreTongRank = nTongRank;
        nTongIndex++;
        
        if (nTongRank <= 5)
        {
            nRewardLevel = 0;
            strRewardInfo = "帮主获特殊表现性衣服“天尊赤火”、称号后缀<龙尊>及易容道具；帮众获称号后缀<战龙>及易容道具";
        }
        else if (nTongRank <= 10)
        {
            nRewardLevel = 1;
            strRewardInfo = "帮主获特殊表现性衣服“武尊玄冰”及称号后缀<虎君>；帮众获称号后缀<霄虎>";
        }
        else if (nTongRank <= 30)
        {
            nRewardLevel = 2;
            strRewardInfo = "帮会所有成员将获得称号后缀<金狮>";
        }
        else if (nTongRank <= 50)
        {
            nRewardLevel = 3;
            strRewardInfo = "帮会所有成员将获得称号后缀<赤狼>";
        }
        else if (nTongRank <= 100)
        {
            nRewardLevel = 4;
            strRewardInfo = "帮会所有成员将获得称号后缀<玄狐>";
        }
        else
        {
            nRewardLevel = -1;
            strRewardInfo = "无奖励";
        }

        nRetCode = fprintf(
            fpResult, "%-20s %-20s %-30s %-12d %-12d %-10d %-20s\r\n",
            itTongInfo->szTongName, itTongInfo->szMasterName, itTongInfo->szMasterAccount,
            itTongInfo->nTotalMemberCount, itTongInfo->nTotalEquipmentLevel, nTongRank, strRewardInfo.c_str()
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
        puts("Operation failed!!");
        KGLogPrintf(KGLOG_INFO, "Operation failed!!");
    }
    puts("Press any key to quit.");
    getchar();
    return nResult ? 0 : 1;
}

int _LoadEquipmentInformation(KG_EQUIPMENT_TYPE_MAP *pRetItemTypeMap)
{
	int nResult  = false;
	int nRetCode = false;
    KG_EQUIPMENT_LEVEL_MAP EquipmentLevelMap;
    pair<KG_EQUIPMENT_TYPE_MAP::iterator, bool> InsertResult;

    ASSERT(pRetItemTypeMap);

    pRetItemTypeMap->clear();

    nRetCode = _LoadItemTabFile("item\\Custom_Weapon.tab", &EquipmentLevelMap);
    KGLOG_PROCESS_ERROR(nRetCode && "_LoadItemTabFile()");

    InsertResult = pRetItemTypeMap->insert(make_pair(EQUIPMENT_TYPE_WEAPON, EquipmentLevelMap));
    ASSERT(InsertResult.second);

    nRetCode = _LoadItemTabFile("item\\Custom_Armor.tab", &EquipmentLevelMap);
    KGLOG_PROCESS_ERROR(nRetCode && "_LoadItemTabFile()");

    InsertResult = pRetItemTypeMap->insert(make_pair(EQUIPMENT_TYPE_ARMOR, EquipmentLevelMap));
    ASSERT(InsertResult.second);

    nRetCode = _LoadItemTabFile("item\\Custom_Trinket.tab", &EquipmentLevelMap);
    KGLOG_PROCESS_ERROR(nRetCode && "_LoadItemTabFile()");

    InsertResult = pRetItemTypeMap->insert(make_pair(EQUIPMENT_TYPE_TRINKET, EquipmentLevelMap));
    ASSERT(InsertResult.second);

	nResult = true;
Exit0:
	return nResult;
}

int _LoadItemTabFile(const char cszTabFileName[], KG_EQUIPMENT_LEVEL_MAP *pRetEquipmentLevelMap)
{
	int nResult  = false;
	int nRetCode = false;
    ITabFile *piTabFile = NULL;
    int nTotalRowCount = 0;
    int nDefaultID = 0;
    int nDefaultLevel  =0;
    int nDefaultQuality = 0;
    int nItemID = 0;
    int nLevel = 0;
    int nQuality = 0;
    float fItemLevel = 0.0;
    pair<KG_EQUIPMENT_LEVEL_MAP::iterator, bool> InsertResult;
    static const float LEVEL_FACTOR_ARRAY[] = {0.0f, 0.8f, 1.0f, 1.4f, 1.8f};

    ASSERT(cszTabFileName);
    ASSERT(cszTabFileName[0]);
    ASSERT(pRetEquipmentLevelMap);

    pRetEquipmentLevelMap->clear();

    piTabFile = g_OpenTabFile(cszTabFileName);
    KGLOG_PROCESS_ERROR(piTabFile);

    nTotalRowCount = piTabFile->GetHeight();
    KGLOG_PROCESS_ERROR(nTotalRowCount > 1);

    // The second row is the default value.
    nRetCode = piTabFile->GetInteger(2, "ID", 0, &nDefaultID);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piTabFile->GetInteger(2, "Level", 0, &nDefaultLevel);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = piTabFile->GetInteger(2, "Quality", 0, &nDefaultQuality);
    KGLOG_PROCESS_ERROR(nRetCode);

    for (int i = 3; i <= nTotalRowCount; i++)
    {
        nRetCode = piTabFile->GetInteger(i, "ID", nDefaultID, &nItemID);
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = piTabFile->GetInteger(i, "Level", nDefaultLevel, &nLevel);
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = piTabFile->GetInteger(i, "Quality", nDefaultQuality, &nQuality);
        KGLOG_PROCESS_ERROR(nRetCode);

        KGLOG_PROCESS_ERROR(nQuality >= 1);
        KGLOG_PROCESS_ERROR(nQuality < sizeof(LEVEL_FACTOR_ARRAY));
        fItemLevel = nLevel * LEVEL_FACTOR_ARRAY[nQuality];

        InsertResult = pRetEquipmentLevelMap->insert(make_pair(nItemID, fItemLevel));
        KGLOG_PROCESS_ERROR(InsertResult.second);
    }

	nResult = true;
Exit0:
    KG_COM_RELEASE(piTabFile);
    if (!nResult)
        KGLogPrintf(KGLOG_DEBUG, "Failed to load tab file : %s", cszTabFileName);
	return nResult;
}

int _GetTongInfo(
    MYSQL *pMySQL, const KG_EQUIPMENT_TYPE_MAP &crEquipmentTypeMap, PARSEHANDLE hTongHandle,
    KG_TONG_INFO *pRetTongInfo
)
{
	int nResult  = false;
	int nRetCode = false;
    DWORD dwRoleID   = 0;
    DWORD dwMasterID = 0;
    string strRoleName;
    string strRoleAccount;
    float fMemberEquipmentsLevel = 0.0f;
    float fTotalEquipmentLevel = 0.0f;
    int nFund = 0;                  // No use
    int nLevel = 0;                 // No use
    int nDevelopmentPoint = 0;      // No use
    WORD wMaxMemeberCount = 0;      // No use
    BYTE byTongState = 0;           // No use
    time_t lStateTimer = 0;         // No use
    BYTE byCamp = 0;                // No use
    int nGroupID = 0;               // No use
    time_t lJoinTime = 0;           // No use
    time_t lLastOfflineTime = 0;    // No use
    char szRemark[64];              // No use
    int nSalary = 0;                // No use

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

    // Get total equipment points and master name.
    pRetTongInfo->szMasterName[0]    = '\0';
    pRetTongInfo->szMasterAccount[0] = '\0';
    for (int i = 0; i < pRetTongInfo->nTotalMemberCount; i++)
    {
        nRetCode = GetTongMemberInfo(
            hTongHandle, i, dwRoleID, nGroupID, lJoinTime, lLastOfflineTime,
            szRemark, sizeof(szRemark), nSalary
        );
        KGLOG_PROCESS_ERROR(nRetCode && "GetTongMemberInfo()");

        nRetCode = _GetRoleInfo(
            pMySQL, crEquipmentTypeMap, dwRoleID, &strRoleName, &strRoleAccount, &fMemberEquipmentsLevel
        );
        KGLOG_PROCESS_ERROR(nRetCode && "_GetRoleInfo()");

        fTotalEquipmentLevel += fMemberEquipmentsLevel;

        if (dwRoleID == dwMasterID)
        {
            ASSERT(pRetTongInfo->szMasterName[0] == '\0'); // Only one master.
            strncpy(pRetTongInfo->szMasterName, strRoleName.c_str(), sizeof(pRetTongInfo->szMasterName));
            KGLOG_PROCESS_ERROR(pRetTongInfo->szMasterName[sizeof(pRetTongInfo->szMasterName) - 1] == '\0');

            ASSERT(pRetTongInfo->szMasterAccount[0] == '\0'); // Only one master.
            strncpy(pRetTongInfo->szMasterAccount, strRoleAccount.c_str(), sizeof(pRetTongInfo->szMasterAccount));
            KGLOG_PROCESS_ERROR(pRetTongInfo->szMasterAccount[sizeof(pRetTongInfo->szMasterAccount) - 1] == '\0');
        }
    }
    pRetTongInfo->nTotalEquipmentLevel = (int)fTotalEquipmentLevel;

    // Get tong handle.
    pRetTongInfo->hTongHandle = hTongHandle;

    nResult = true;
Exit0:
	return nResult;
}

int _GetRoleInfo(
    MYSQL *pMySQL, const KG_EQUIPMENT_TYPE_MAP &crEquipmentTypeMap, DWORD dwRoleID,
    string *pstrRetRoleName, string *pstrRetRoleAccount, float *pfRetEquipmentsLevel
)
{
	int nResult  = false;
	int nRetCode = false;
    char szQuery[128];
    MYSQL_RES *pRoleInfo = NULL;
    MYSQL_ROW Row = NULL;
    unsigned long *pulLengths = NULL;
    PARSEHANDLE hRoleHandle = NULL;
    WORD wTotalItemCount = 0;
    BYTE byBox = 0;
    BYTE byPos = 0;
    BYTE byTabType = 0;
    WORD wTabIndex = 0;
    KG_EQUIPMENT_TYPE_MAP::const_iterator citType;
    const KG_EQUIPMENT_LEVEL_MAP *pcEquipmentLevelMap = NULL;
    KG_EQUIPMENT_LEVEL_MAP::const_iterator citLevel;
    BYTE byVersion = 0;         // No use
    BYTE byBind = 0;            // No use
    WORD wDurability = 0;       // No use
    time_t lGenTime = 0;        // No use
    int nIsEqui = false;        // No use
    DWORD dwRandSeed = 0;       // No use
    WORD wLeftEnchantTime = 0;  // No use
    BYTE byColorID = 0;         // No use
    WORD wPermanentEnchant = 0; // No use
    WORD wTemporaryEnchant = 0; // No use
    WORD wMount1 = 0;           // No use
    WORD wMount2 = 0;           // No use
    WORD wMount3 = 0;           // No use
    WORD wMount4 = 0;           // No use

    ASSERT(pMySQL);
    ASSERT(pstrRetRoleName);
    ASSERT(pstrRetRoleAccount);
    ASSERT(pfRetEquipmentsLevel);

    nRetCode = snprintf(
        szQuery, sizeof(szQuery),
        "select `RoleName`, `Account`, `ExtInfo` from `Role` where `ID` = %d", dwRoleID
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

    hRoleHandle = ParseRoleExtData((BYTE *)Row[2], pulLengths[2]);
    KGLOG_PROCESS_ERROR(hRoleHandle && "ParseRoleBaseData()");

    nRetCode = GetRoleItemCount(hRoleHandle, wTotalItemCount);
    KGLOG_PROCESS_ERROR(nRetCode && "GetRoleItemCount()");

    *pfRetEquipmentsLevel = 0.0;
    for (WORD i = 0; i < wTotalItemCount; i++)
    {
        nRetCode = GetRoleItemInfo(
            hRoleHandle, i, byBox, byPos, byVersion, byTabType, wTabIndex, byBind, wDurability,
            lGenTime, nIsEqui, dwRandSeed, wLeftEnchantTime, byColorID, wPermanentEnchant, 
            wTemporaryEnchant, wMount1, wMount2, wMount3, wMount4
        );
        KGLOG_PROCESS_ERROR(nRetCode && "GetRoleItemInfo()");

        nRetCode = _IsArmedEquipment(byBox, byPos);
        if (!nRetCode)
            continue;

        citType = crEquipmentTypeMap.find(byTabType);
        if (citType == crEquipmentTypeMap.end())
        {
            KGLogPrintf(
                KGLOG_DEBUG, "Failed to find the item, TabType : %d, TabIndex : %d",
                byTabType, wTabIndex
            );
            continue;   // Ignore the equipment not found.
        }

        pcEquipmentLevelMap = &(citType->second);

        citLevel = pcEquipmentLevelMap->find(wTabIndex);
        if (citLevel == pcEquipmentLevelMap->end())
        {
            KGLogPrintf(
                KGLOG_DEBUG, "Failed to find the item, TabType : %d, TabIndex : %d",
                byTabType, wTabIndex
            );
            continue;   // Ignore the equipment not found.
        }

        *pfRetEquipmentsLevel += citLevel->second;
    }

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

int _IsArmedEquipment(int nBoxIndex, int nItemIndex)
{
    // 只统计玩家身上的：(上装、下装、帽子、鞋子、腰带、护手、武器、暗器囊、暗器、项链一、腰坠、2个戒指）
    if (nBoxIndex != 0) // indicate the item is armed.
        return false;

    switch (nItemIndex)
    {
    case 0:     // 近战武器
    case 1:     // 远程武器
    case 2:     // 上装
    case 3:     // 帽子
    case 4:     // 项链
    case 5:     // 左手戒指
    case 6:     // 右手戒指
    case 7:     // 腰带
    case 8:     // 腰坠
    case 9:     // 下装
    case 10:    // 鞋子
    case 11:    // 护手
    case 22:    // 暗器
        return true;
    default:
        return false;
    }
}

int _RewardTongMembers(MYSQL *pMySQL, PARSEHANDLE hTongHandle, int nRewardLevel)
{
    int nResult  = false;
    int nRetCode = false;
    int nTotalMemberCount = 0;
    DWORD dwRoleID = 0;
    DWORD dwMasterID = 0;
    int nIsMasterFlag = false;
    char szTongName[64];            // No use
    int nFund = 0;                  // No use
    int nLevel = 0;                 // No use
    int nDevelopmentPoint = 0;      // No use
    WORD wMaxMemeberCount = 0;      // No use
    BYTE byTongState = 0;           // No use
    time_t lStartTimer = 0;         // No use
    BYTE byCamp = 0;                // No use
    int nGroupID = 0;               // No use
    time_t lJoinTime = 0;           // No use
    time_t lLastOfflineTime = 0;    // No use
    char szRemark[64];              // No use
    int nSalary = 0;                // No use

    ASSERT(pMySQL);
    ASSERT(hTongHandle);

    // Get tong name and master ID.
    nRetCode = GetTongBaseInfo(
        hTongHandle, szTongName, sizeof(szTongName), dwMasterID,nFund, nLevel,
        nDevelopmentPoint, wMaxMemeberCount, byTongState, lStartTimer, byCamp
    );
    KGLOG_PROCESS_ERROR(nRetCode && "GetTongBaseInfo()");

    // Get total member count.
    nRetCode = GetTongMemberCount(hTongHandle, nTotalMemberCount);
    KGLOG_PROCESS_ERROR(nRetCode && "GetTongMemberCount()");

    for (int i = 0; i < nTotalMemberCount; i++)
    {
        nRetCode = GetTongMemberInfo(
            hTongHandle, i, dwRoleID, nGroupID, lJoinTime, lLastOfflineTime, 
            szRemark, sizeof(szRemark), nSalary
        );
        KGLOG_PROCESS_ERROR(nRetCode && "GetTongMemberInfo()");

        if (dwRoleID == dwMasterID)
            nIsMasterFlag = true;
        else
            nIsMasterFlag = false;

        nRetCode = KG_RewardRole(
            pMySQL, dwRoleID, KG_ACTIVITY_ID, "2009-10-29 08:00:00", nRewardLevel, nIsMasterFlag, 0, 0, 0, 0
        );
        KGLOG_PROCESS_ERROR(nRetCode && "KG_RewardRole()");
    }

	nResult = true;
Exit0:
	return nResult;
}
