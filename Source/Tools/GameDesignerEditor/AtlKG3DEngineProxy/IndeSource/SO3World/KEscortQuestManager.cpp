#include "stdafx.h"
#include "KEscortQuestManager.h"
#include "KPlayer.h"
#include "KNpc.h"
#include "KPlayerServer.h"
#include "KScene.h"

#ifdef _SERVER
#define SCRIPT_ON_END_ESCORT_QUEST	"OnEndEscortQuest"
#define ESCORT_RANGE CELL_LENGTH * REGION_GRID_WIDTH

BOOL KEscortQuestManager::Init(KScene* pScene)
{
    assert(pScene); 
    m_pScene = pScene; 
    return true;
}

void KEscortQuestManager::Activate()
{
    DWORD         dwQuestID    = 0;
    KEscortQuest* pEscortQuest = NULL;

    for (MAP_ESCORT_QUEST::iterator it = m_EscortQuestMap.begin(); it != m_EscortQuestMap.end(); NULL)
    {
        dwQuestID = it->first;
        pEscortQuest = &(it->second);

        switch (pEscortQuest->eQuestState)
        {
        case eqsWaitStart:
            OnQuestWaitStart(dwQuestID, pEscortQuest);
            break;

        case eqsRunning:
            OnQuestRunning(dwQuestID, pEscortQuest);
            break;

        case eqsFinished:
            if (g_pSO3World->m_nGameLoop > pEscortQuest->nEndFrames)
            {
                CloseEscortQuest(dwQuestID, 3 * GAME_FPS);
            }
            break;

        case eqsClosed:
            if (g_pSO3World->m_nGameLoop > pEscortQuest->nTimer)
            {
                EndEscortQuest(dwQuestID);
                m_EscortQuestMap.erase(it++);
                continue;
            }
            break;

        default:
            break;
        }

        ++it;
    }
}

BOOL KEscortQuestManager::AddEscortQuest(DWORD dwQuestID, KPlayer* pPlayerAccept, KNpc* pNpcAccept)
{
    BOOL            bResult    = false;
    BOOL            bRetCode   = false;
    KEscortQuest    EscortQuest;
    MAP_ESCORT_QUEST::iterator it;

    assert(pPlayerAccept);
    assert(pNpcAccept);
    
    it = m_EscortQuestMap.find(dwQuestID);
    KGLOG_PROCESS_ERROR(it == m_EscortQuestMap.end());

    ZeroMemory(&EscortQuest, sizeof(EscortQuest));

    EscortQuest.dwNpcAcceptID   = pNpcAccept->m_dwID;
    EscortQuest.nTimer          = g_pSO3World->m_nGameLoop + START_ESCORT_QUEST_DELAY;
    EscortQuest.eQuestState     = eqsWaitStart;
    EscortQuest.nEndFrames      = g_pSO3World->m_nGameLoop + 600 * GAME_FPS;

    EscortQuest.dwPlayerID[0] = pPlayerAccept->m_dwID;
    EscortQuest.bValid[0]     = true;

    bRetCode = InvitePlayerInParty(dwQuestID, pPlayerAccept);
    KGLOG_PROCESS_ERROR(bRetCode);
        
    m_EscortQuestMap[dwQuestID] = EscortQuest;
    
    pNpcAccept->m_bDialogFlag = false;
	g_PlayerServer.DoSyncNpcDialogFlag(pNpcAccept);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KEscortQuestManager::CanAcceptEscortQuest(DWORD dwQuestID)
{
    BOOL            bResult    = false;
    MAP_ESCORT_QUEST::iterator it;
    
    it = m_EscortQuestMap.find(dwQuestID);
    KG_PROCESS_ERROR(it == m_EscortQuestMap.end());

    bResult = true;
Exit0:
    return bResult;
}

BOOL KEscortQuestManager::AddPlayer(DWORD dwQuestID, DWORD dwPlayerID)
{
    BOOL            bResult         = false;
    BOOL            bRetCode        = false;
	KEscortQuest*   pEscortQuest    = NULL;
    int             nIndex          = 0;
    MAP_ESCORT_QUEST::iterator it;

	KGLOG_PROCESS_ERROR(dwPlayerID != ERROR_ID);

	it = m_EscortQuestMap.find(dwQuestID);
    KGLOG_PROCESS_ERROR(it != m_EscortQuestMap.end());

	KG_PROCESS_ERROR(it->second.eQuestState == eqsWaitStart);

	for (nIndex = 0;nIndex < MAX_PARTY_SIZE; ++nIndex)
	{
        if (it->second.dwPlayerID[nIndex] != ERROR_ID)
        {
            continue;
        }

        it->second.dwPlayerID[nIndex] = dwPlayerID;
        it->second.bValid[nIndex]     = true;

        break;
	}
    
    KGLOG_PROCESS_ERROR(nIndex < MAX_PARTY_SIZE);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KEscortQuestManager::StartEscortQuest(DWORD dwQuestID)
{
    BOOL        bResult     = false;
	BOOL        bRetCode    = false;
	KQuestInfo* pQuestInfo  = NULL;
	KNpc*       pAcceptNpc  = NULL;
    MAP_ESCORT_QUEST::iterator it;
    
    it = m_EscortQuestMap.find(dwQuestID);
    KGLOG_PROCESS_ERROR(it != m_EscortQuestMap.end());
    
    pAcceptNpc = g_pSO3World->m_NpcSet.GetObj(it->second.dwNpcAcceptID);
    KGLOG_PROCESS_ERROR(pAcceptNpc);

	pQuestInfo = g_pSO3World->m_Settings.m_QuestInfoList.GetQuestInfo(dwQuestID);
	KGLOG_PROCESS_ERROR(pQuestInfo);

	bRetCode = g_pSO3World->m_ScriptCenter.IsScriptExist(pQuestInfo->szScriptName);
	if (bRetCode && g_pSO3World->m_ScriptCenter.IsFuncExist(pQuestInfo->szScriptName, SCRIPT_ON_START_QUEST))
	{
        int nTopIndex = 0;

		g_pSO3World->m_ScriptCenter.SafeCallBegin(&nTopIndex);

		g_pSO3World->m_ScriptCenter.PushValueToStack(m_pScene);
		g_pSO3World->m_ScriptCenter.PushValueToStack(pAcceptNpc);

		g_pSO3World->m_ScriptCenter.CallFunction(pQuestInfo->szScriptName, SCRIPT_ON_START_QUEST, 0);
		g_pSO3World->m_ScriptCenter.SafeCallEnd(nTopIndex);
	}

    bResult = true;
Exit0:
    return bResult;
}

BOOL KEscortQuestManager::SetEscortNpc(DWORD dwQuestID, DWORD dwNpcEscortID)
{
    BOOL bResult = false;
    MAP_ESCORT_QUEST::iterator it;
    
    it = m_EscortQuestMap.find(dwQuestID);
    KGLOG_PROCESS_ERROR(it != m_EscortQuestMap.end());

    it->second.dwNpcEscortID = dwNpcEscortID;

    bResult = true;
Exit0:
    return bResult;
}

BOOL KEscortQuestManager::SetEscortQuestEndFrames(DWORD dwQuestID, int nFrames)
{
    BOOL bResult = false;
    MAP_ESCORT_QUEST::iterator it;
    
    KGLOG_PROCESS_ERROR(nFrames > 0);

    it = m_EscortQuestMap.find(dwQuestID);
    KGLOG_PROCESS_ERROR(it != m_EscortQuestMap.end());
    
    it->second.nEndFrames = g_pSO3World->m_nGameLoop + nFrames;

    bResult = true;
Exit0:
    return bResult;
}

BOOL KEscortQuestManager::SetEscortQuestValue(DWORD dwQuestID, int nIndex, int nValue)
{
    BOOL bResult     = false;
    BOOL bRetCode    = false;
    int  nQuestIndex = -1;
    MAP_ESCORT_QUEST::iterator it;
    
    it = m_EscortQuestMap.find(dwQuestID);
    KGLOG_PROCESS_ERROR(it != m_EscortQuestMap.end());

    for (int i = 0; i < MAX_PARTY_SIZE; ++i)
    {
        KPlayer* pPlayer = NULL;

        if (it->second.dwPlayerID[i] == ERROR_ID || !(it->second.bValid[i]))
        {
            continue;
        }
        
        pPlayer = g_pSO3World->m_PlayerSet.GetObj(it->second.dwPlayerID[i]);
        if (pPlayer == NULL || pPlayer->m_pScene == NULL)
        {
            it->second.dwPlayerID[i] = ERROR_ID;
            it->second.bValid[i] = false;
            continue;
        }
        
        nQuestIndex = pPlayer->m_QuestList.GetQuestIndex(dwQuestID);
        KGLOG_CHECK_ERROR(nQuestIndex != -1);

        if (nQuestIndex != -1)
        {
            bRetCode = pPlayer->m_QuestList.SetQuestValue(nQuestIndex, nIndex, nValue);
            KGLOG_PROCESS_ERROR(bRetCode);
        }
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KEscortQuestManager::AddPlayerBuff(DWORD dwQuestID, DWORD dwBuffID, int nBuffLevel)
{
    BOOL                bResult     = false;
    BOOL                bRetCode    = false;
    KBUFF_RECIPE_KEY    BuffRecipeKey;
    MAP_ESCORT_QUEST::iterator it;
    
    it = m_EscortQuestMap.find(dwQuestID);
    KGLOG_PROCESS_ERROR(it != m_EscortQuestMap.end());
    
    memset(&BuffRecipeKey, 0, sizeof(BuffRecipeKey));
    
    BuffRecipeKey.dwID      = dwBuffID;
    BuffRecipeKey.nLevel    = nBuffLevel;

    for (int i = 0; i < MAX_PARTY_SIZE; ++i)
    {
        KPlayer*            pPlayer = NULL;

        if (it->second.dwPlayerID[i] == ERROR_ID || !(it->second.bValid[i]))
        {
            continue;
        }
        
        pPlayer = g_pSO3World->m_PlayerSet.GetObj(it->second.dwPlayerID[i]);
        if (pPlayer == NULL || pPlayer->m_pScene == NULL)
        {
            continue;
        }
        
        bRetCode = pPlayer->m_BuffList.CallBuff(
            pPlayer->m_dwID, pPlayer->m_nLevel, BuffRecipeKey, 0, 0
        );
        KGLOG_CHECK_ERROR(bRetCode);
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KEscortQuestManager::DelPlayerBuff(DWORD dwQuestID, DWORD dwBuffID, int nBuffLevel)
{
    BOOL bResult = false;
    BOOL bRetCode = false;
    MAP_ESCORT_QUEST::iterator it;
    
    it = m_EscortQuestMap.find(dwQuestID);
    KGLOG_PROCESS_ERROR(it != m_EscortQuestMap.end());

    for (int i = 0; i < MAX_PARTY_SIZE; ++i)
    {
        KPlayer* pPlayer = NULL;

        if (it->second.dwPlayerID[i] == ERROR_ID)
        {
            continue;
        }
        
        pPlayer = g_pSO3World->m_PlayerSet.GetObj(it->second.dwPlayerID[i]);
        if (pPlayer == NULL || pPlayer->m_pScene == NULL)
        {
            continue;
        }

        pPlayer->m_BuffList.DelSingleBuff(dwBuffID, nBuffLevel);
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KEscortQuestManager::SetPlayerFailed(KPlayer* pPlayer, DWORD dwQuestID)
{
    BOOL                            bResult     = false;
    MAP_ESCORT_QUEST::iterator      it;
    
    assert(pPlayer);

    it = m_EscortQuestMap.find(dwQuestID);
    KG_PROCESS_ERROR(it != m_EscortQuestMap.end());

    for (int i = 0; i < MAX_PARTY_SIZE; ++i)
    {
        if (it->second.dwPlayerID[i] == pPlayer->m_dwID)
        {
            it->second.bValid[i] = false;
            break;
        }
    }
    
    bResult = true;
Exit0:
    return bResult;
}

BOOL KEscortQuestManager::CloseEscortQuest(DWORD dwQuestID, int nFrames)
{
    BOOL        bResult     = false;
    BOOL        bRetCode    = false;
    MAP_ESCORT_QUEST::iterator  it;
    
    it = m_EscortQuestMap.find(dwQuestID);
    KGLOG_PROCESS_ERROR(it != m_EscortQuestMap.end());
    
    if (nFrames < 1)
    {
        nFrames = 1; 
        // 至少要延后一帧。
        // 某些NPC死亡脚本中会调用Close接口，如果立刻调用OnEnd接口删NPC，
        // 则接下来的访问NPC的操作会宕机。
    }

    it->second.eQuestState = eqsClosed;
    it->second.nTimer      = g_pSO3World->m_nGameLoop + nFrames; 

    bResult = true;
Exit0:
    return bResult;
}

BOOL KEscortQuestManager::EndEscortQuest(DWORD dwQuestID)
{
    BOOL        bResult     = false;
    BOOL        bRetCode    = false;
    KQuestInfo* pQuestInfo  = NULL;
    KNpc*       pEscortNpc  = NULL;
    KNpc*       pAcceptNpc  = NULL;
    MAP_ESCORT_QUEST::iterator it;
    
    it = m_EscortQuestMap.find(dwQuestID);
    KGLOG_PROCESS_ERROR(it != m_EscortQuestMap.end());

	pQuestInfo = g_pSO3World->m_Settings.m_QuestInfoList.GetQuestInfo(dwQuestID);
	KGLOG_PROCESS_ERROR(pQuestInfo);

	bRetCode = g_pSO3World->m_ScriptCenter.IsScriptExist(pQuestInfo->szScriptName);
	if (bRetCode && g_pSO3World->m_ScriptCenter.IsFuncExist(pQuestInfo->szScriptName, SCRIPT_ON_END_ESCORT_QUEST))
	{
        int nTopIndex = 0;

		g_pSO3World->m_ScriptCenter.SafeCallBegin(&nTopIndex);
		g_pSO3World->m_ScriptCenter.PushValueToStack(m_pScene);
		g_pSO3World->m_ScriptCenter.CallFunction(pQuestInfo->szScriptName, SCRIPT_ON_END_ESCORT_QUEST, 0);
		g_pSO3World->m_ScriptCenter.SafeCallEnd(nTopIndex);
	}
    
    pEscortNpc = g_pSO3World->m_NpcSet.GetObj(it->second.dwNpcEscortID);
    if (pEscortNpc)
    {
        if (pEscortNpc->m_pScene)
        {
            pEscortNpc->GotoReviveList();
        }
        m_pScene->m_NpcReviveManager.ForceReviveNpc(pEscortNpc);
    }

    pAcceptNpc = g_pSO3World->m_NpcSet.GetObj(it->second.dwNpcAcceptID);
    KGLOG_PROCESS_ERROR(pAcceptNpc);
    
    pAcceptNpc->m_bDialogFlag = true;

    if (pAcceptNpc->m_pScene)
    {
    	g_PlayerServer.DoSyncNpcDialogFlag(pAcceptNpc);
    }

    bResult = true;
Exit0:
    return bResult;
}

const KEscortQuest* KEscortQuestManager::GetEscortQuest(DWORD dwQuestID)
{
    KEscortQuest* pResult = NULL;
    MAP_ESCORT_QUEST::iterator it;
    
    it = m_EscortQuestMap.find(dwQuestID);
    KGLOG_PROCESS_ERROR(it != m_EscortQuestMap.end());
    
    pResult = &(it->second);
Exit0:
    return pResult;
}

BOOL KEscortQuestManager::InvitePlayerInParty(DWORD dwQuestID, KPlayer* pPlayerAccept)
{
    BOOL                         bResult = false;
    BOOL                         bRetCode = false;
    KQuestInfo*                  pQuestInfo = NULL;
    KTeam*                       pTeam      = NULL;
    KTarget                      Target;

    assert(pPlayerAccept);
    
    KG_PROCESS_SUCCESS(pPlayerAccept->m_dwTeamID == ERROR_ID);
        
    pQuestInfo = g_pSO3World->m_Settings.m_QuestInfoList.GetQuestInfo(dwQuestID);
    KGLOG_PROCESS_ERROR(pQuestInfo);
    
    bRetCode = Target.SetTarget(pPlayerAccept);
    KGLOG_PROCESS_ERROR(bRetCode);

    pTeam = g_pSO3World->m_TeamServer.GetTeam(pPlayerAccept->m_dwTeamID);
    KGLOG_PROCESS_ERROR(pTeam);
    
    for (int i = 0; i < pTeam->nGroupNum; i++)
    {
        for (
            KTEAM_MEMBER_LIST::iterator it = pTeam->MemberGroup[i].MemberList.begin();
            it != pTeam->MemberGroup[i].MemberList.end(); ++it
        )
        {
            QUEST_RESULT_CODE   eRetCode    = qrcFailed;
            KPlayer*            pMember     = NULL;

            if (it->dwMemberID == pPlayerAccept->m_dwID)
            {
                continue;
            }

            pMember = g_pSO3World->m_PlayerSet.GetObj(it->dwMemberID);
            if (pMember == NULL || pMember->m_pScene == NULL)
            {
                continue;
            }

            if (pMember->m_pScene != pPlayerAccept->m_pScene)
            {
                continue;
            }

            bRetCode = g_InRange(pPlayerAccept, pMember, ESCORT_RANGE);
            if (!bRetCode)
            {
                continue;
            }

            eRetCode = pMember->m_QuestList.CanAccept(pQuestInfo, &Target);
            if (eRetCode != qrcSuccess)
            {
                continue;
            }

            g_PlayerServer.DoStartEscortQuest(pMember->m_nConnIndex, pPlayerAccept->m_dwID, dwQuestID);
        }
    }

Exit1:
    bResult = true;
Exit0:
    return bResult;
}

void KEscortQuestManager::OnQuestWaitStart(DWORD dwQuestID, KEscortQuest* pEscortQuest)
{
    BOOL bRetCode = false;

    assert(pEscortQuest);

    if (g_pSO3World->m_nGameLoop > pEscortQuest->nTimer)
	{
		pEscortQuest->eQuestState = eqsRunning;

		bRetCode = StartEscortQuest(dwQuestID);
	    KGLOG_PROCESS_ERROR(bRetCode);
    }

Exit0:
    return;
}

void KEscortQuestManager::OnQuestRunning(DWORD dwQuestID, KEscortQuest* pEscortQuest)
{   
    BOOL     bRetCode           = false;
    KPlayer* pPlayer            = NULL;
    KNpc*    pEscortNpc         = NULL;
    BOOL     bAllPlayerFailed   = true;
    int      nQuestPhase        = 0;
    int      nQuestIndex        = -1; 

    assert(pEscortQuest);
    
    for (int i = 0; i < MAX_PARTY_SIZE; ++i)
    {
        if (pEscortQuest->dwPlayerID[i] == ERROR_ID || !pEscortQuest->bValid[i])
        {
            continue;
        }

        assert(g_pSO3World);
        pPlayer = g_pSO3World->m_PlayerSet.GetObj(pEscortQuest->dwPlayerID[i]);
        if (pPlayer == NULL || pPlayer->m_pScene == NULL)
        {
            pEscortQuest->dwPlayerID[i] = ERROR_ID;
            pEscortQuest->bValid[i] = false;
            continue;
        }
        
        nQuestPhase = pPlayer->m_QuestList.GetQuestPhase(dwQuestID);
        if (nQuestPhase == 2 || nQuestPhase == 3)
        {
            pEscortQuest->eQuestState = eqsFinished;
            goto Exit0;
        }
    }

    pEscortNpc = g_pSO3World->m_NpcSet.GetObj(pEscortQuest->dwNpcEscortID);    
    if (pEscortNpc == NULL || pEscortNpc->m_pScene == NULL || g_pSO3World->m_nGameLoop > pEscortQuest->nEndFrames)
    {
        for (int i = 0; i < MAX_PARTY_SIZE; ++i)
        {     
            if (pEscortQuest->dwPlayerID[i] == ERROR_ID || !pEscortQuest->bValid[i])
            {
                continue;
            }

            pPlayer = g_pSO3World->m_PlayerSet.GetObj(pEscortQuest->dwPlayerID[i]);
            if (pPlayer == NULL || pPlayer->m_pScene == NULL)
            {
                pEscortQuest->dwPlayerID[i] = ERROR_ID;
                pEscortQuest->bValid[i] = false;
                continue;
            }

            nQuestIndex = pPlayer->m_QuestList.GetQuestIndex(dwQuestID);
            if (nQuestIndex == -1)
            {
                pEscortQuest->dwPlayerID[i] = ERROR_ID;
                pEscortQuest->bValid[i] = false;
                continue;
            }
            
            pPlayer->m_QuestList.SetQuestFailedFlag(nQuestIndex, true);            
            continue;
        }

        CloseEscortQuest(dwQuestID, 3 * GAME_FPS);

        goto Exit0;
    }
    
    for (int i = 0; i < MAX_PARTY_SIZE; ++i)
    {
        if (pEscortQuest->dwPlayerID[i] == ERROR_ID || !pEscortQuest->bValid[i])
        {
            continue;
        }
        
        pPlayer = g_pSO3World->m_PlayerSet.GetObj(pEscortQuest->dwPlayerID[i]);
        if (pPlayer == NULL || pPlayer->m_pScene == NULL)
        {
            pEscortQuest->dwPlayerID[i] = ERROR_ID;
            pEscortQuest->bValid[i] = false;
            continue;
        }

        bRetCode = g_InRange(pEscortNpc, pPlayer, ESCORT_RANGE);
        if (!bRetCode)
        {
            nQuestIndex = pPlayer->m_QuestList.GetQuestIndex(dwQuestID);
            if (nQuestIndex == -1)
            {
                pEscortQuest->dwPlayerID[i] = ERROR_ID;
                pEscortQuest->bValid[i] = false;
                continue;
            }
            
            pPlayer->m_QuestList.SetQuestFailedFlag(nQuestIndex, true);
            continue;
        }
        
        bAllPlayerFailed = false; // 还有玩家没有失败
    }
        
    if (bAllPlayerFailed) // 所有玩家都失败了
    {
        CloseEscortQuest(dwQuestID, 3 * GAME_FPS);
    }

Exit0:
    return;
}
#endif
