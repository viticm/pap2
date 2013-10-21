#include "stdafx.h"
#include "KStatDataManager.h"
#include "KSO3GameCenter.h"
#include "KMapInfo.h"

#include "KRole.h"

KStatDataManager::KStatDataManager()
{
    m_nStatTime  = 0;
    m_nMaxUsedID = 0;
    memset(m_StatValues, 0, sizeof(m_StatValues));
}

KStatDataManager::~KStatDataManager()
{
}

BOOL KStatDataManager::Init()
{
    time_t nTimeNow = time(NULL);

    m_nStatTime = (nTimeNow / 3600) * 3600;

    return true;
}

void KStatDataManager::UnInit()
{
    m_DataNameTable.clear();
}

void KStatDataManager::Activate()
{
    if (g_pSO3GameCenter->m_nWorkLoop % (GAME_FPS * 60) == 0)
    {
        time_t nCurrentTime = (g_pSO3GameCenter->m_nTimeNow / 3600) * 3600; // 对齐到小时

        SaveAll();

        if (nCurrentTime != m_nStatTime)
        {
            memset(m_StatValues, 0, sizeof(m_StatValues));
            m_nMaxUsedID = 0;

            m_nStatTime = nCurrentTime;
        }
    }
}

BOOL KStatDataManager::OnGSUpdate(KSTAT_DATA_MODIFY* pData, int nCount)
{
    KSTAT_DATA_MODIFY*  pNode   = pData;
    KSTAT_DATA_MODIFY*  pTail   = pData + nCount;

    while (pNode < pTail)
    {
        m_StatValues[pNode->nID - 1] += pNode->nValue;
        
        if (pNode->nID > m_nMaxUsedID)
            m_nMaxUsedID = pNode->nID;

        pNode++;
    }
    
    return true;
}

int KStatDataManager::GetNameID(const char cszName[])
{
    int                         nResult    = 0;
    int                         nRetCode   = 0;
    int                         nNameID    = 0;
    uint64_t                    uNewDataID = 0;
    KDATA_NAME_TABLE::iterator  it;

    it = m_DataNameTable.find(cszName);
    if (it == m_DataNameTable.end())
    {
        nRetCode = g_pSO3GameCenter->m_piSequence->GenerateID("StatDataName", 1, &uNewDataID);
        KGLOG_PROCESS_ERROR(nRetCode);

        nNameID = (int)uNewDataID;
        KGLOG_PROCESS_ERROR(nNameID <= MAX_STAT_DATA_COUNT);

        m_DataNameTable[cszName] = nNameID;

        g_pSO3GameCenter->m_MiscDB.DoSaveStatDataName((char*)cszName, nNameID);
    }
    else
    {
        nNameID = it->second;
    }

    nResult = nNameID;
Exit0:
    return nResult;
}

// 直接设置某项统计数据的值，慎用
BOOL KStatDataManager::SetDataValue(const char cszName[], int64_t nValue)
{
    BOOL                                        bResult     = false;
    int                                         nRetCode    = 0;
    int                                         nNameID     = 0;
    uint64_t                                    uNewDataID  = 0;

    nNameID = GetNameID(cszName);
    KGLOG_PROCESS_ERROR(nNameID != 0);

    m_StatValues[nNameID - 1] = nValue;
    
    if (nNameID > m_nMaxUsedID)
        m_nMaxUsedID = nNameID;

    bResult = true;
Exit0:
    return bResult;
}

BOOL KStatDataManager::SaveAll()
{
    BOOL    bResult         = false;
    int     nFarmerCount    = 0;
    int     nTeamCount      = 0;

    UpdateRoleStat();

    nFarmerCount = g_pSO3GameCenter->m_AntiFarmerManager.GetFarmerCount();

    SetDataValue("ANTI_FARMER", nFarmerCount);
    
    nTeamCount = g_pSO3GameCenter->m_TeamCenter.GetTeamCount();
    SetDataValue("TEAM_COUNT", nTeamCount);

    KG_PROCESS_ERROR(m_nMaxUsedID > 0);

    g_pSO3GameCenter->m_MiscDB.DoSaveStatData(m_nStatTime, m_nMaxUsedID, &m_StatValues[0]);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KStatDataManager::LoadDataName(const char szName[], int nID)
{
    BOOL                                        bResult = false;
    KDATA_NAME_TABLE::iterator                  it;
    std::pair<KDATA_NAME_TABLE::iterator, BOOL> InsRet;

    assert(szName);
    KGLOG_PROCESS_ERROR(nID > 0 && nID <= MAX_STAT_DATA_COUNT);
    
    it = m_DataNameTable.find(szName);
    KGLOG_PROCESS_ERROR(it == m_DataNameTable.end());

    InsRet = m_DataNameTable.insert(std::make_pair(kstring(szName), nID));
    KGLOG_PROCESS_ERROR(InsRet.second);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KStatDataManager::LoadData(time_t nTime, BYTE* pbyData, size_t uDataLen)
{
    BOOL            bResult = false;
    KStatData_DB*   pDataDB = (KStatData_DB*)pbyData;
    
    assert(pDataDB);
    KGLOG_PROCESS_ERROR(nTime <= g_pSO3GameCenter->m_nTimeNow);

    KGLOG_PROCESS_ERROR(uDataLen == sizeof(KStatData_DB) + pDataDB->nDataCount * sizeof(int64_t));

    memcpy(&m_StatValues[0], pDataDB->nData, uDataLen - sizeof(pDataDB->nDataCount));

    m_nStatTime  = nTime;
    m_nMaxUsedID = pDataDB->nDataCount;

    bResult = true;
Exit0:
    return bResult;
}

BOOL KStatDataManager::Update(const char cszName[], int64_t nValue)
{
    BOOL    bResult = false;
    int     nNameID = 0;

    nNameID = GetNameID(cszName);
    KGLOG_PROCESS_ERROR(nNameID);

    m_StatValues[nNameID - 1] += nValue;

    if (nNameID > m_nMaxUsedID)
        m_nMaxUsedID = nNameID;

    bResult = true;
Exit0:
    return bResult;
}

struct KROLE_GROUP_MAP_KEY 
{
    BOOL    m_bOnline;
    BOOL    m_bInTong;
    KCAMP   m_eCamp;

    KROLE_GROUP_MAP_KEY(BOOL bOnline, BOOL bInTong, KCAMP eCamp)
    {
        m_bOnline   = bOnline;
        m_bInTong   = bInTong;
        m_eCamp     = eCamp;
    };
};

inline bool operator < (const KROLE_GROUP_MAP_KEY& crLeft, const KROLE_GROUP_MAP_KEY& crRight)
{
    if (crLeft.m_bOnline == crRight.m_bOnline)
    {
        if (crLeft.m_bInTong == crRight.m_bInTong)
        {
            return crLeft.m_eCamp < crRight.m_eCamp;
        }
        return !crLeft.m_bInTong;
    }
    return !crLeft.m_bOnline;
}

struct KUPDATE_ROLE_STAT 
{
    BOOL operator () (DWORD dwRoleID, KRole& rRole)
    {
        BOOL    bRetCode    = false;
        time_t  nActiveTime = rRole.m_nLastSaveTime > 0 ? rRole.m_nLastSaveTime : rRole.m_nCreateTime;
        int     nOffDays    = (int)((g_pSO3GameCenter->m_nTimeNow - nActiveTime) / (24 * 3600));
        KROLE_GROUP_MAP_KEY         GroupKey(rRole.IsOnline(), !!rRole.m_dwTongID, rRole.m_eCamp);
        KROLE_DIST_MAP::iterator    itDist;
        KROLE_GROUP_MAP::iterator   itGroup;
        KROLE_ACTIVE_MAP::iterator  itActive;

        // 在线角色分布
        bRetCode = rRole.IsOnline();
        if (bRetCode)
        {
            itDist = m_RoleDistMap.find(std::make_pair(rRole.m_dwMapID, rRole.m_byLevel));
            if (itDist == m_RoleDistMap.end())
                m_RoleDistMap.insert(std::make_pair(std::make_pair(rRole.m_dwMapID, rRole.m_byLevel), 1));
            else
                (itDist->second)++;
        }

        // 群体
        itGroup = m_RoleGroupMap.find(GroupKey);
        if (itGroup == m_RoleGroupMap.end())
            m_RoleGroupMap.insert(std::make_pair(GroupKey, 1));
        else
            (itGroup->second)++;

        // 流失率
        if (nOffDays > 100)
            nOffDays = 100;

        itActive = m_RoleActiveMap.find(std::make_pair(nOffDays, rRole.m_byLevel));
        if (itActive == m_RoleActiveMap.end())
            m_RoleActiveMap.insert(std::make_pair(std::make_pair(nOffDays, rRole.m_byLevel), 1));
        else
            (itActive->second)++;

        return true;
    };

    typedef KMemory::KAllocator<std::pair<std::pair<DWORD, int>, unsigned int> >    KROLE_DIST_MAP_ALLOCATOR;
    typedef KMemory::KAllocator<std::pair<KROLE_GROUP_MAP_KEY, unsigned int> >      KROLE_GROUP_MAP_ALLOCATOR;
    typedef KMemory::KAllocator<std::pair<std::pair<int, int>, unsigned int> >      KROLE_ACTIVE_MAP_ALLOCATOR;

    typedef std::map<std::pair<DWORD, int>, unsigned int, std::less<std::pair<DWORD, int> >, KROLE_DIST_MAP_ALLOCATOR>  KROLE_DIST_MAP;
    typedef std::map<KROLE_GROUP_MAP_KEY, unsigned int, std::less<KROLE_GROUP_MAP_KEY>, KROLE_GROUP_MAP_ALLOCATOR>      KROLE_GROUP_MAP;
    typedef std::map<std::pair<int, int>, unsigned int, std::less<std::pair<int, int> >, KROLE_ACTIVE_MAP_ALLOCATOR>    KROLE_ACTIVE_MAP;
    
    KROLE_DIST_MAP      m_RoleDistMap;
    KROLE_GROUP_MAP     m_RoleGroupMap;
    KROLE_ACTIVE_MAP    m_RoleActiveMap;
};

void KStatDataManager::UpdateRoleStat()
{
    int                 nRoleCount      = 0;
    int                 nAccountCount   = 0;
    KUPDATE_ROLE_STAT   UpdateRoleStat;
    char                szVarName[STAT_DATA_NAME_LEN];

    // 角色信息统计
    g_pSO3GameCenter->m_RoleManager.Traverse(UpdateRoleStat);

    for (
        KUPDATE_ROLE_STAT::KROLE_DIST_MAP::iterator it = UpdateRoleStat.m_RoleDistMap.begin(), itEnd = UpdateRoleStat.m_RoleDistMap.end();
        it != itEnd;
        ++it
    )
    {
        snprintf(szVarName, sizeof(szVarName), "ROLE_DIST|%lu|%d", it->first.first, it->first.second);
        szVarName[sizeof(szVarName) - 1] = '\0';

        g_pSO3GameCenter->m_StatDataManager.SetDataValue(szVarName, it->second);
    }

    for (
        KUPDATE_ROLE_STAT::KROLE_GROUP_MAP::iterator it = UpdateRoleStat.m_RoleGroupMap.begin(), itEnd = UpdateRoleStat.m_RoleGroupMap.end();
        it != itEnd;
        ++it
    )
    {
        const char* pszCamp     = NULL;
        const char* pszOnline   = (it->first.m_bOnline ? "ONLINE" : "OFFLINE");
        const char* pszTong     = (it->first.m_bInTong ? "IN_TONG" : "OUT_TONG");

        switch (it->first.m_eCamp)
        {
        case cGood:
            pszCamp = "GOOD";
            break;
        case cEvil:
            pszCamp = "EVIL";
            break;
        default:
            pszCamp = "NEUTRAL";
            break;
        }

        snprintf(szVarName, sizeof(szVarName), "ROLE_GROUP|%s|%s|%s", pszOnline, pszCamp, pszTong);
        szVarName[sizeof(szVarName) - 1] = '\0';

        g_pSO3GameCenter->m_StatDataManager.SetDataValue(szVarName, it->second);
    }

    for (
        KUPDATE_ROLE_STAT::KROLE_ACTIVE_MAP::iterator it = UpdateRoleStat.m_RoleActiveMap.begin(), itEnd = UpdateRoleStat.m_RoleActiveMap.end();
        it != itEnd;
        ++it
    )
    {
        snprintf(szVarName, sizeof(szVarName), "ROLE_ACTIVE|%d|%d", it->first.first, it->first.second);
        szVarName[sizeof(szVarName) - 1] = '\0';

        g_pSO3GameCenter->m_StatDataManager.SetDataValue(szVarName, it->second);
    }

    // 角色总数
    snprintf(szVarName, sizeof(szVarName), "ROLE_TOTAL");
    szVarName[sizeof(szVarName) - 1] = '\0';

    nRoleCount = g_pSO3GameCenter->m_RoleManager.GetRoleCountTotal();
    g_pSO3GameCenter->m_StatDataManager.SetDataValue(szVarName, nRoleCount);

    // 账号总数
    snprintf(szVarName, sizeof(szVarName), "ACCOUNT_TOTAL");
    szVarName[sizeof(szVarName) - 1] = '\0';

    nAccountCount = g_pSO3GameCenter->m_RoleManager.GetAccountCountTotal();
    g_pSO3GameCenter->m_StatDataManager.SetDataValue(szVarName, nAccountCount);
}

void KStatDataManager::UpdateCreateMap(KMapInfo* pMapInfo)
{
    char szVarName[STAT_DATA_NAME_LEN];

    if (pMapInfo->m_nType != emtDungeon && pMapInfo->m_nType != emtBattleField)
        return;

    snprintf(szVarName, sizeof(szVarName), "MAP|%lu|CREATE", pMapInfo->m_dwMapID);
    szVarName[sizeof(szVarName) - 1] = '\0';

    Update(szVarName, 1);
}

void KStatDataManager::UpdateCampStat(KCAMP eCamp, int nLevel)
{
    const char* pszCamp = NULL;
    char        szVarName[STAT_DATA_NAME_LEN];
    
    if (eCamp == cNeutral)
        return;

    pszCamp = (eCamp == cGood ? "GOOD" : "EVIL"); 

    snprintf(szVarName, sizeof(szVarName), "CAMP|%s|%d", pszCamp, nLevel);
    szVarName[sizeof(szVarName) - 1] = '\0';

    Update(szVarName, 1);
}

void KStatDataManager::UpdataLeaveCampStat(KCAMP eCamp, int nLevel)
{
    const char* pszCamp = NULL;
    char        szVarName[STAT_DATA_NAME_LEN];

    if (eCamp == cNeutral)
        return;

    pszCamp = (eCamp == cGood ? "GOOD" : "EVIL"); 

    snprintf(szVarName, sizeof(szVarName), "LEAVE_CAMP|%s|%d", pszCamp, nLevel);
    szVarName[sizeof(szVarName) - 1] = '\0';

    Update(szVarName, 1);
}

void KStatDataManager::UpdataAuctionItemStat(DWORD dwTabType, DWORD dwTabIndex, int nMoney)
{
    char szVarName[STAT_DATA_NAME_LEN];

    snprintf(szVarName, sizeof(szVarName), "AUCTION_COUNT|%lu|%lu", dwTabType, dwTabIndex);
    szVarName[sizeof(szVarName) - 1] = '\0';

    Update(szVarName, 1); // 统计拍卖行道具交易成功次数

    snprintf(szVarName, sizeof(szVarName), "AUCTION_MONEY|%lu|%lu", dwTabType, dwTabIndex);
    szVarName[sizeof(szVarName) - 1] = '\0';

    Update(szVarName, nMoney); // 统计拍卖行道具交易金额
}

void KStatDataManager::UpdateMoneyStat(KRole* pRole, int nMoney, const char cszMethod[])
{
    const char* pszGain = (nMoney >= 0 ? "GAIN" : "COST");
    char szVarName[STAT_DATA_NAME_LEN];

    KG_PROCESS_ERROR(nMoney != 0);

    snprintf(szVarName, sizeof(szVarName), "MONEY|%s|%lu|%d|%s", pszGain, pRole->m_dwMapID, pRole->m_byLevel, cszMethod);
    szVarName[sizeof(szVarName) - 1] = '\0';

    if (nMoney < 0)
        nMoney = -nMoney;

    Update(szVarName, nMoney);

Exit0:
    return;
}

void KStatDataManager::UpdataCreateOrDeleteRole(BOOL bCreate)
{
    const char* pszCreate = (bCreate ? "CREATE" : "DELETE");
    char szVarName[STAT_DATA_NAME_LEN];

    snprintf(szVarName, sizeof(szVarName), "ROLE|%s", pszCreate);
    szVarName[sizeof(szVarName) - 1] = '\0';

    Update(szVarName, 1);
}

void KStatDataManager::UpdateGameCardDealAmmount(int nCoin, int nMoney)
{
    Update("GAMECARD_COIN", nCoin);

    Update("GAMECARD_MONEY", nMoney);
}

void KStatDataManager::UpdateMentorCreateStat(DWORD dwMentorID, DWORD dwApprenticeID)
{
    KRole*  pMRole  = g_pSO3GameCenter->m_RoleManager.GetRole(dwMentorID);
    KRole*  pARole  = g_pSO3GameCenter->m_RoleManager.GetRole(dwApprenticeID);
    char    szVarName[STAT_DATA_NAME_LEN];

    KGLOG_PROCESS_ERROR(pMRole);
    KGLOG_PROCESS_ERROR(pARole);

    Update("MENTOR_NEW", 1);

    snprintf(szVarName, sizeof(szVarName), "MENTOR_CREATE_MENTOR_FORCE|%lu", pMRole->m_dwForceID);
    szVarName[sizeof(szVarName) - 1] = '\0';

    Update(szVarName, 1);

    snprintf(szVarName, sizeof(szVarName), "MENTOR_CREATE_APPRENTICE_FORCE|%lu", pARole->m_dwForceID);
    szVarName[sizeof(szVarName) - 1] = '\0';

    Update(szVarName, 1);

Exit0:
    return;
}

void KStatDataManager::UpdateMentorDeleteStat()
{
    Update("MENTOR_DEL", 1);
}

void KStatDataManager::UpdateMentorGraduateStat(DWORD dwMentorID, DWORD dwApprenticeID)
{
    KRole* pMRole = g_pSO3GameCenter->m_RoleManager.GetRole(dwMentorID);
    KRole* pARole = g_pSO3GameCenter->m_RoleManager.GetRole(dwApprenticeID);

    Update("MENTOR_GRADUATE", 1);

    KG_PROCESS_ERROR(pMRole);
    KG_PROCESS_ERROR(pARole);

    if (pMRole->m_dwTongID != 0 && pMRole->m_dwTongID == pARole->m_dwTongID)
    {
        Update("MENTOR_GRADUATE_SAME_TONG", 1);
    }

Exit0:
    return;
}

void KStatDataManager::UpdateMentorBrokenStat(DWORD dwMentorID, DWORD dwApprenticeID)
{
    KRole* pMRole = g_pSO3GameCenter->m_RoleManager.GetRole(dwMentorID);
    KRole* pARole = g_pSO3GameCenter->m_RoleManager.GetRole(dwApprenticeID);
    char szVarName[STAT_DATA_NAME_LEN];

    if (pMRole)
    {
        snprintf(szVarName, sizeof(szVarName), "MENTOR_BROKEN_MENTOR_LEVEL|%u", pMRole->m_byLevel);
        szVarName[sizeof(szVarName) - 1] = '\0';

        Update(szVarName, 1);
    }

    if (pARole)
    {
        snprintf(szVarName, sizeof(szVarName), "MENTOR_BROKEN_APPRENTICE_LEVEL|%u", pARole->m_byLevel);
        szVarName[sizeof(szVarName) - 1] = '\0';

        Update(szVarName, 1);
    }
}
