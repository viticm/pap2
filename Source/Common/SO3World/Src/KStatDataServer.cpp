#include "stdafx.h"
#include "KStatDataServer.h"
#include "KSO3World.h"
#include "KPlayer.h"
#include "KScene.h"

#ifdef _SERVER

#include    "KRelayClient.h"

KStatDataServer::KStatDataServer()
{
}

KStatDataServer::~KStatDataServer()
{
}

BOOL KStatDataServer::Init()
{   
    return true;
}

void KStatDataServer::UnInit()
{
    KDATA_TABLE::iterator it;

    for (it = m_DataTable.begin(); it != m_DataTable.end(); ++it)
    {
        KMemory::Delete(it->second);
    }
    m_DataTable.clear();
}

void KStatDataServer::Activate()
{
    for (int i = 0; i < 16; i++)
    {
        if (m_Name2IDQueue.empty())
            break;

        const char* pszName = m_Name2IDQueue.begin()->c_str();

        g_RelayClient.DoQueryStatIDRequest(pszName);

        m_Name2IDQueue.erase(m_Name2IDQueue.begin());
    }

    if (g_pSO3World->m_nGameLoop % GAME_FPS == 0 && !m_DataList.empty())
        g_RelayClient.DoUpdateStatDataRequest();

    return;
}

void KStatDataServer::SendAllStatData()
{
    while (!m_DataList.empty())
    {
        g_RelayClient.DoUpdateStatDataRequest();
    }

    return;
}

int KStatDataServer::GetUpdateStatData(KSTAT_DATA_MODIFY* pData, int nMaxCount)
{
    KStatData*          pStatData   = NULL;
    KSTAT_DATA_MODIFY*  pNode       = pData;
    KSTAT_DATA_MODIFY*  pTail       = pData + nMaxCount;

    while (pNode < pTail && !m_DataList.empty())
    {
        pStatData = m_DataList.front();

        assert(pStatData);

        pNode->nID    = pStatData->m_nID;
        pNode->nValue = pStatData->m_nValue;

        pStatData->m_nValue  = 0;
        pStatData->m_bInList = false;

        pNode++;

        m_DataList.pop_front();
    }

    return (int)(pNode - pData);
}

BOOL KStatDataServer::Update(const char szName[], int64_t nValue)
{
    BOOL                                    bResult   = false;
    size_t                                  uNameLen  = 0;
    KStatData*                              pStatData = NULL;
    KDATA_TABLE::iterator                   it;
    std::pair<KDATA_TABLE::iterator, BOOL>  InsRet;

    uNameLen = strlen(szName) + 1;
    KGLOG_PROCESS_ERROR(uNameLen <= STAT_DATA_NAME_LEN);

    it = m_DataTable.find((char*)szName);

    if (it == m_DataTable.end())
    {
        pStatData = KMemory::New<KStatData>();
        KGLOG_PROCESS_ERROR(pStatData);

        strncpy(pStatData->m_szName, szName, sizeof(pStatData->m_szName));
        pStatData->m_szName[sizeof(pStatData->m_szName) - 1] = '\0';

        InsRet = m_DataTable.insert(std::make_pair(pStatData->m_szName, pStatData));
        KGLOG_PROCESS_ERROR(InsRet.second);

        it = InsRet.first;
    }
    
    pStatData = it->second;
    pStatData->m_nValue += nValue;

    if (pStatData->m_nID == 0)
    {
        m_Name2IDQueue.insert(szName);
    }
    else
    {
        // 只有已获得ID的数据才放入m_DataList中
        if (!pStatData->m_bInList)
        {
            // 不在队列里说明是已发送过的数据，此次再次修改
            m_DataList.push_back(pStatData);
            pStatData->m_bInList = true;
        }
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KStatDataServer::SetNameID(const char szName[], int nID)
{
    BOOL                    bResult   = false;
    KStatData*              pStatData = NULL;
    KDATA_TABLE::iterator   it;
    
    it = m_DataTable.find((char*)szName);
    KGLOG_PROCESS_ERROR(it != m_DataTable.end());

    pStatData = it->second;
    KGLOG_PROCESS_ERROR(pStatData);

    KG_PROCESS_SUCCESS(pStatData->m_nID != 0);
        
    pStatData->m_nID = nID;
    m_DataList.push_back(pStatData);
    pStatData->m_bInList = true;

Exit1:
    bResult = true;
Exit0:
    return bResult;
}

void KStatDataServer::UpdateMoneyStat(KPlayer* pPlayer, int nMoney, const char cszMethod[])
{
    const char* pszGain = (nMoney >= 0 ? "GAIN" : "COST");
    char        szVarName[STAT_DATA_NAME_LEN];

    KGLOG_PROCESS_ERROR(pPlayer->m_pScene);
    KG_PROCESS_ERROR(nMoney != 0);

    snprintf(szVarName, sizeof(szVarName), "MONEY|%s|%lu|%d|%s", pszGain, pPlayer->m_pScene->m_dwMapID, pPlayer->m_nLevel, cszMethod);
    szVarName[sizeof(szVarName) - 1] = '\0';

    if (nMoney < 0)
        nMoney = -nMoney;

    Update(szVarName, nMoney);

Exit0:
    return;
}

void KStatDataServer::UpdateContributeStat(KPlayer* pPlayer, int nContribute, const char cszMethod[])
{
    const char* pszGain = (nContribute >= 0 ? "GAIN" : "COST");
    char        szVarName[STAT_DATA_NAME_LEN];

    KGLOG_PROCESS_ERROR(pPlayer->m_pScene);

    snprintf(szVarName, sizeof(szVarName), "CONTRIBUTE|%s|%lu|%d|%s", pszGain, pPlayer->m_pScene->m_dwMapID, pPlayer->m_nLevel, cszMethod);
    szVarName[sizeof(szVarName) - 1] = '\0';

    if (nContribute < 0)
        nContribute = -nContribute;

    Update(szVarName, nContribute);

Exit0:
    return;
}

void KStatDataServer::UpdateSkillStat(DWORD dwSkillID, DWORD dwSkillLevel)
{
    char szVarName[STAT_DATA_NAME_LEN];

    snprintf(szVarName, sizeof(szVarName), "SKILL|%lu|%lu", dwSkillID, dwSkillLevel);
    szVarName[sizeof(szVarName) - 1] = '\0';

    Update(szVarName, 1);
}

void KStatDataServer::UpdateFormationStat(DWORD dwFormationID, DWORD dwFormationLevel)
{
    char szVarName[STAT_DATA_NAME_LEN];

    snprintf(szVarName, sizeof(szVarName), "FORMATION|%lu|%lu", dwFormationID, dwFormationLevel);
    szVarName[sizeof(szVarName) - 1] = '\0';

    Update(szVarName, 1);
}

void KStatDataServer::UpdateAutoFlyStat(int nValue, const char cszMethod[])
{
    char szVarName[STAT_DATA_NAME_LEN];

    snprintf(szVarName, sizeof(szVarName), "AUTOFLY|%s", cszMethod);
    szVarName[sizeof(szVarName) - 1] = '\0';

    Update(szVarName, nValue);
}

void KStatDataServer::UpdateLearnProfessionLevelState(DWORD dwProfessionID, DWORD dwLevel)
{
    char szVarName[STAT_DATA_NAME_LEN];

    snprintf(szVarName, sizeof(szVarName), "LEARN_PROFESSION_LEVEL|%lu|%lu", dwProfessionID, dwLevel);
    szVarName[sizeof(szVarName) - 1] = '\0';

    Update(szVarName, 1);
}

void KStatDataServer::UpdateLearnProfessionBranchState(DWORD dwProfessionID, DWORD dwBranchID)
{
    char szVarName[STAT_DATA_NAME_LEN];

    snprintf(szVarName, sizeof(szVarName), "LEARN_PROFESSION_BRANCH|%lu|%lu", dwProfessionID, dwBranchID);
    szVarName[sizeof(szVarName) - 1] = '\0';

    Update(szVarName, 1);
}

void KStatDataServer::UpdateRecipeCastState(DWORD dwCraftID, DWORD dwRecipeID)
{
    char szVarName[STAT_DATA_NAME_LEN];

    snprintf(szVarName, sizeof(szVarName), "RECIPE_CAST|%lu|%lu", dwCraftID, dwRecipeID);
    szVarName[sizeof(szVarName) - 1] = '\0';

    Update(szVarName, 1);
}

void KStatDataServer::UpdateEnterScene(KScene* pScene, KPlayer* pPlayer)
{
    char szVarName[STAT_DATA_NAME_LEN];

    if (pScene->m_nType == emtDungeon)
    {
        snprintf(szVarName, sizeof(szVarName), "MAP|%lu|ENTER_COUNT|%d", pScene->m_dwMapID, pPlayer->m_nLevel);
        szVarName[sizeof(szVarName) - 1] = '\0';

        Update(szVarName, 1);

        snprintf(szVarName, sizeof(szVarName), "MAP|%lu|KUNGFU|%lu", pScene->m_dwMapID, pPlayer->m_SkillList.m_dwMountKungfuID);
        szVarName[sizeof(szVarName) - 1] = '\0';

        Update(szVarName, 1);
    }

    if (pScene->m_nType == emtBattleField)
    {
        snprintf(szVarName, sizeof(szVarName), "MAP|%lu|ENTER_COUNT", pScene->m_dwMapID);
        szVarName[sizeof(szVarName) - 1] = '\0';

        Update(szVarName, 1);

        snprintf(szVarName, sizeof(szVarName), "MAP|%lu|KUNGFU|%lu", pScene->m_dwMapID, pPlayer->m_SkillList.m_dwMountKungfuID);
        szVarName[sizeof(szVarName) - 1] = '\0';

        Update(szVarName, 1);
    }
}

void KStatDataServer::UpdateItemDrop(DWORD dwItemTab, DWORD dwItemIndex)
{
    char szVarName[STAT_DATA_NAME_LEN];

    snprintf(szVarName, sizeof(szVarName), "ITEM_DROP|%lu|%lu", dwItemTab, dwItemIndex);
    szVarName[sizeof(szVarName) - 1] = '\0';

    Update(szVarName, 1);
}

void KStatDataServer::UpdateTrainState(int nTrain)
{
    const char* pszType = (nTrain >= 0 ? "PRODUCE" : "CONSUME");
    char        szVarName[STAT_DATA_NAME_LEN];

    snprintf(szVarName, sizeof(szVarName), "TRAIN|%s", pszType);
    szVarName[sizeof(szVarName) - 1] = '\0';

    Update(szVarName, nTrain);
}


void KStatDataServer::UpdateQuestStat(DWORD dwQuestID, const char cszState[])
{
    char szVarName[STAT_DATA_NAME_LEN];

    snprintf(szVarName, sizeof(szVarName), "QUEST|%lu|%s", dwQuestID, cszState);
    szVarName[sizeof(szVarName) - 1] = '\0';

    Update(szVarName, 1);
}

void KStatDataServer::UpdateQuestRepeat(DWORD dwQuestID)
{
    char szVarName[STAT_DATA_NAME_LEN];

    snprintf(szVarName, sizeof(szVarName), "QUEST_REPEAT|%lu", dwQuestID);
    szVarName[sizeof(szVarName) - 1] = '\0';

    Update(szVarName, 1);
}

void KStatDataServer::UpdateAssistQuest(DWORD dwQuestID)
{
    char szVarName[STAT_DATA_NAME_LEN];

    snprintf(szVarName, sizeof(szVarName), "QUEST_ASSIST|%lu", dwQuestID);
    szVarName[sizeof(szVarName) - 1] = '\0';

    Update(szVarName, 1);
}

void KStatDataServer::UpdateMentorAssistQuest(DWORD dwQuestID)
{
    char szVarName[STAT_DATA_NAME_LEN];

    snprintf(szVarName, sizeof(szVarName), "QUEST_MENTOR_ASSIST|%lu", dwQuestID);
    szVarName[sizeof(szVarName) - 1] = '\0';

    Update(szVarName, 1);
}

void KStatDataServer::UpdatePVEStat(DWORD dwVictimID, DWORD dwKillerID)
{
    BOOL        bVictimIsNpc    = false;
    BOOL        bKillerIsNpc    = false;
    KNpc*       pNpc            = NULL;
    const char* pszDescription  = NULL;
    char        szVarName[STAT_DATA_NAME_LEN];

    KG_PROCESS_ERROR(dwKillerID);

    bVictimIsNpc = (IS_NPC(dwVictimID) ? true : false);
    bKillerIsNpc = (IS_NPC(dwKillerID) ? true : false);
    KG_PROCESS_ERROR(bVictimIsNpc != bKillerIsNpc);

    if (bVictimIsNpc)
    {
        pszDescription = "PLAYER_KILL_NPC";
        pNpc = g_pSO3World->m_NpcSet.GetObj(dwVictimID);
        KGLOG_PROCESS_ERROR(pNpc);
    }
    else
    {
        pszDescription = "NPC_KILL_PLAYER";
        pNpc = g_pSO3World->m_NpcSet.GetObj(dwKillerID);
        KGLOG_PROCESS_ERROR(pNpc);
    }

    snprintf(szVarName, sizeof(szVarName), "%s|%lu", pszDescription, pNpc->m_dwTemplateID);
    szVarName[sizeof(szVarName) - 1] = '\0';
    Update(szVarName, 1);

Exit0:
    return;
}

void KStatDataServer::UpdateTalkStat(KPlayer* pPlayer, const char* cszMsgType)
{
    char        szVarName[STAT_DATA_NAME_LEN];

    snprintf(szVarName, sizeof(szVarName), "TALK|%s|%d", cszMsgType, pPlayer->m_nLevel);
    szVarName[sizeof(szVarName) - 1] = '\0';

    Update(szVarName, 1);

}

void KStatDataServer::UpdatePrestigeStat(KPlayer* pPlayer, int nPrestige, const char* cszMethod)
{
    const char* pszGain = (nPrestige >= 0 ? "GAIN" : "COST");
    char        szVarName[STAT_DATA_NAME_LEN];

    KG_PROCESS_ERROR(nPrestige == 0);

    KGLOG_PROCESS_ERROR(pPlayer->m_pScene);

    snprintf(szVarName, sizeof(szVarName), "PRESTIGE|%s|%lu|%d|%s", 
        pszGain, pPlayer->m_pScene->m_dwMapID, pPlayer->m_nLevel, cszMethod);
    szVarName[sizeof(szVarName) - 1] = '\0';

    if (nPrestige < 0)
        nPrestige = -nPrestige;

    Update(szVarName, nPrestige);

Exit0:
    return;
}

void KStatDataServer::UpdateAchievementPointStat(KPlayer* pPlayer, int nPoint)
{
    const char* pszGain = nPoint >= 0 ? "GAIN" : "COST";
    char        szVarName[STAT_DATA_NAME_LEN];

    KGLOG_PROCESS_ERROR(pPlayer->m_pScene);

    snprintf(szVarName, sizeof(szVarName), "ACHIEVEMENT_POINT|%s|%lu|%d", 
        pszGain, pPlayer->m_pScene->m_dwMapID, pPlayer->m_nLevel);
    szVarName[sizeof(szVarName) - 1] = '\0';

    if (nPoint < 0)
        nPoint = -nPoint;

    Update(szVarName, nPoint);

Exit0:
    return;
}

void KStatDataServer::UpdateAchievementStat(int nID)
{
    char szVarName[STAT_DATA_NAME_LEN];

    snprintf(szVarName, sizeof(szVarName), "ACHIEVEMENT|%d", nID);
    szVarName[sizeof(szVarName) - 1] = '\0';

    Update(szVarName, 1);
}

void KStatDataServer::UpdateDesignationStat(BOOL bIsPrefix, int nID)
{
    const char* pszFix = bIsPrefix ? "PREFIX" : "POSTFIX"; // bIsPrefix为true表示前缀，false表示后缀
    char        szVarName[STAT_DATA_NAME_LEN];

    snprintf(szVarName, sizeof(szVarName), "DESIGNATION|%s|%d", pszFix, nID);
    szVarName[sizeof(szVarName) - 1] = '\0';

    Update(szVarName, 1);
}

void KStatDataServer::UpdateShopRepairItemStat(KPlayer* pPlayer, BOOL bIsAllRepair)
{
    const char* pszRepairStyle = bIsAllRepair ? "ALL" : "SINGLE";
    char        szVarName[STAT_DATA_NAME_LEN];

    snprintf(szVarName, sizeof(szVarName), "SHOP_REPAIR|%s|%d", pszRepairStyle, pPlayer->m_nLevel);
    szVarName[sizeof(szVarName) - 1] = '\0';

    Update(szVarName, 1);
}

void KStatDataServer::UpdateExpStat(KPlayer* pPlayer, int nAddExp, const char* cszMethod)
{
    char szVarName[STAT_DATA_NAME_LEN];

    KGLOG_PROCESS_ERROR(pPlayer->m_pScene);

    snprintf(szVarName, sizeof(szVarName), "EXP|%ld|%d|%s", 
        pPlayer->m_pScene->m_dwMapID, pPlayer->m_nLevel, cszMethod);
    szVarName[sizeof(szVarName) - 1] = '\0';

    Update(szVarName, nAddExp);

Exit0:
    return;
}

void KStatDataServer::UpdateClientNormalLogout()
{
    Update("CLIENT_NORMAL_LOGOUT", 1);
}

void KStatDataServer::UpdateClientErrorLogout()
{
    Update("CLIENT_ERROR_LOGOUT", 1);
}

void KStatDataServer::UpdateClientLoginPermit()
{
    Update("CLIENT_LOGIN_PERMIT", 1);
}

void KStatDataServer::UpdateClientTimeout()
{
    Update("CLIENT_TIMEOUT", 1);
}

#endif // _SERVER
