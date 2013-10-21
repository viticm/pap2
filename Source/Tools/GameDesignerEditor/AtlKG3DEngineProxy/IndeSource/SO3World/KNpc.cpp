#include "stdafx.h"
#include "KNpc.h"
#include "KRegion.h"
#include "KScene.h"
#include "KSO3World.h"
#include "KMath.h"
#include "KNpcTemplateList.h"
#include "KPlayerServer.h"
#include "KThreatList.h"
#include "KAttrModifier.h"
#include "KPlayer.h"
#include "KLootList.h"
#include <vector>

#ifdef _CLIENT
#include "KPlayerClient.h"
#include "SO3Represent/SO3GameWorldRepresentHandler.h"
#include "SO3UI/SO3GameWorldUIHandler.h"
#endif

#define NPC_ACTION_RANGE	(REGION_GRID_WIDTH * CELL_LENGTH * 3 / 4)

BOOL KNpc::Init()
{
    BOOL bResult  = false;
	BOOL bRetCode = false;

	bRetCode = KCharacter::Init();
	KGLOG_PROCESS_ERROR(bRetCode);

    m_nIntensity                = 0;
    m_dwScriptID                = 0;
    m_pShop                     = NULL;
	m_dwTemplateID              = 0;
    m_nReviveTime               = 100;
	m_nDisappearFrames	        = 0; 
	m_bDisappearToReviveList    = false;

	m_eKind                     = ckInvalid;
	m_nMaxLife                  = 1024;
	m_nMaxMana                  = 1024;
	m_nCurrentLife              = 1024;
	m_nCurrentMana              = 1024;

    m_eSpecies                  = ensInvalid;
	m_bDialogFlag		        = false;
	m_bDisableAutoTurn          = false;
    m_eMoveMode                 = mmGround;

    m_dwReliveID		        = 0;
    m_dwRandomID                = 0;

#ifdef _SERVER
	m_bBank				        = false;
    m_bMailbox                  = false;
    m_dwSkillMasterID           = 0;
    m_dwCraftMasterID           = 0;
    m_bAuction                  = false;
    m_nPathFindAstar            = 0;
    
    ClearDropTarget();

	m_nNpcTeamIndex		        = -1;
	m_nNpcTeamID		        = 0;
	m_nSkillCommomCD	        = 0;
    memset(m_nSkillCastFrame, 0, sizeof(m_nSkillCastFrame));
    m_nSkillSelectIndex         = -1;
#endif

#ifdef _CLIENT
    m_dwDropTargetPlayerID    = ERROR_ID;
	m_nUpdateMiniMapMarkFrame = 0;
#endif //_CLIENT

	m_pTemplate			= NULL;
    m_dwEmployer        = ERROR_ID;

    bResult = true;
Exit0:
	return bResult;
}

void KNpc::UnInit()
{
    if (m_pShop)
    {
#ifdef _SERVER
        g_pSO3World->m_ShopCenter.UnbindNpcShop(this);
#endif
        m_pShop = NULL;
    }

	KCharacter::UnInit();
}

BOOL KNpc::Activate(void)
{
	BOOL    bResult  = false;
	BOOL    bRetCode = false;

	bRetCode = KSceneObject::CheckGameLoop();
	KG_PROCESS_SUCCESS(!bRetCode);

	KGLOG_PROCESS_ERROR(m_pRegion);

#ifdef _SERVER
	if (m_bFightState || m_AIData.nPatrolPathID > 0)
    {
		m_pRegion->SetActive();
	}

    if (((DWORD)g_pSO3World->m_nGameLoop - m_dwID) % GAME_FPS == 0)
    {
        g_PlayerServer.DoBroadcastCharacterLife(this);
    }

	KG_PROCESS_ERROR(m_pScene);

#endif	//_SERVER

#ifdef _CLIENT
	if (m_nUpdateMiniMapMarkFrame <= g_pSO3World->m_nGameLoop)
	{
		m_nUpdateMiniMapMarkFrame = g_pSO3World->m_nGameLoop + UPDATE_MINIMAP_MARK_INTERVAL;
		UpdateMiniMapMark(); //更新小地图标记
	}
#endif //_CLIENT

	KCharacter::Activate();
    // m_pScene指针为空表示自己被删除了
    KG_PROCESS_ERROR(m_pScene);

#ifdef _CLIENT
    if (g_pGameWorldRepresentHandler)
        g_pGameWorldRepresentHandler->OnUpdateCharacterFrameData(this);
#endif

#ifdef _SERVER
	// 判断是不是到了消失时间
	if (m_nDisappearFrames != 0 && g_pSO3World->m_nGameLoop > m_nDisappearFrames)
	{
		if (m_bDisappearToReviveList)
		{
			GotoReviveList();
		}
		else
		{
			bRetCode = g_pSO3World->DeleteNpc(this);
			KGLOG_PROCESS_ERROR(bRetCode);
		}

		m_nDisappearFrames = 0;
	}
#endif //_SERVER

Exit1:
	bResult = true;
Exit0:
	return bResult;
}

#ifdef _CLIENT
void KNpc::UpdateMiniMapMark()
{
	BOOL        bRetCode    = false;
	KPlayer*    pPlayer     = NULL;

	pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
	KGLOG_PROCESS_ERROR(pPlayer);

	bRetCode = pPlayer->CanFinishQuestOnNpc(this);
	if (bRetCode || ((pPlayer->m_eRadarType == mrtFindNpc) && (pPlayer->m_nRadarParam == (int)m_eSpecies)))
	{
        if (g_pGameWorldUIHandler)
        {
            KUIEventUpdateMiniMapNpcMark UIParam;
            UIParam.dwNpcID = m_dwID;
            UIParam.nX      = m_nX;
            UIParam.nY      = m_nY;

            g_pGameWorldUIHandler->OnUpdateMiniMapNpcMark(UIParam);
        }
	}

Exit0:
    return;
}
#endif

BOOL KNpc::Load(const KNPC_DATA& crNpcData)
{
	BOOL bResult	= false;
	BOOL bRetCode	= false;

    bRetCode = g_pSO3World->m_Settings.m_NpcTemplateList.LoadFromTemplate(crNpcData.dwTemplateID, this);
    KGLOG_PROCESS_ERROR(bRetCode);

    assert(m_pTemplate);

    m_dwTemplateID			= crNpcData.dwTemplateID;
    m_nX					= crNpcData.nX;
	m_nY					= crNpcData.nY;
	m_nZ					= crNpcData.nZ;
    m_nFaceDirection		= crNpcData.byFaceDirection;
	m_eKind					= (CHARACTER_KIND)crNpcData.byKind;
	m_dwReliveID			= crNpcData.dwReliveID;
    m_dwRandomID            = crNpcData.dwRandomID;
	m_nNpcTeamID			= crNpcData.dwThreatLinkID;
	m_nNpcTeamIndex			= crNpcData.nOrderIndex;
    if (crNpcData.dwAIType)
    {
        m_AIData.nAIType = crNpcData.dwAIType;
    }

    m_bDisableAutoTurn		= false;

    if (crNpcData.dwRepresentID)
    {
	    m_dwModelID	= crNpcData.dwRepresentID;
    }

    if (crNpcData.dwScriptID)
    {
        m_dwScriptID = crNpcData.dwScriptID;
    }

    if (crNpcData.szNickName[0] != '\0')
    {
		strncpy(m_szName, crNpcData.szNickName, sizeof(m_szName));
        m_szName[sizeof(m_szName) - 1] = '\0';        
    }

	bResult = true;
Exit0:
	return bResult;
}

BOOL KNpc::ChangeRegion(KRegion* pDestRegion)
{
	BOOL bRetCode = false;

	assert(m_pRegion);
	assert(pDestRegion);

#ifdef _SERVER
	if (m_bFightState || m_AIData.nPatrolPathID > 0)
	{
		pDestRegion->SetActive();
	}

    RegisterToViewRegion(m_pRegion, pDestRegion);
#endif

	m_RegionObjNode.Remove();
	pDestRegion->m_NpcList.AddTail((KNode*)&m_RegionObjNode);      
	m_pRegion = pDestRegion;

	return true;
}

#ifdef _SERVER
void KNpc::RegisterToViewRegion(KRegion* pFromRegion, KRegion* pToRegion)
{
    int                 nLeft               = 0;
    int                 nRight              = 0;
    int                 nBottom             = 0;
    int                 nTop                = 0;
    KRegisterNewNpcFunc Func(this); 

    assert(m_pScene);
    assert(pFromRegion);
    assert(pToRegion);

    // 1. 将新Region及其邻接Region标记为true
    nLeft       = pToRegion->m_nRegionX - m_pScene->m_nBroadcastRegion;
    nRight      = pToRegion->m_nRegionX + m_pScene->m_nBroadcastRegion;
    nBottom     = pToRegion->m_nRegionY - m_pScene->m_nBroadcastRegion;
    nTop        = pToRegion->m_nRegionY + m_pScene->m_nBroadcastRegion;

    for (int nX = nLeft; nX <= nRight; nX++)
    {
        for (int nY = nBottom; nY <= nTop; nY++)
        {
            KRegion* pRegion = m_pScene->GetRegion(nX, nY);
            if (pRegion)
            {
                pRegion->m_bRegionView = true;
            }
        }
    }

    // 2. 将旧Region及其邻接Region标记为false,新旧视野的交集会被覆盖为false
    nLeft       = pFromRegion->m_nRegionX - m_pScene->m_nBroadcastRegion;
    nRight      = pFromRegion->m_nRegionX + m_pScene->m_nBroadcastRegion;
    nBottom     = pFromRegion->m_nRegionY - m_pScene->m_nBroadcastRegion;
    nTop        = pFromRegion->m_nRegionY + m_pScene->m_nBroadcastRegion;
    
    for (int nX = nLeft; nX <= nRight; nX++)
    {
        for (int nY = nBottom; nY <= nTop; nY++)
        {
            KRegion* pRegion = m_pScene->GetRegion(nX, nY);
            if (pRegion)
            {
                pRegion->m_bRegionView = false;
            }
        }
    }

    // 3. 目标Region视野中,标志为true的即是新进入视野的Region
    nLeft       = pToRegion->m_nRegionX - m_pScene->m_nBroadcastRegion;
    nRight      = pToRegion->m_nRegionX + m_pScene->m_nBroadcastRegion;
    nBottom     = pToRegion->m_nRegionY - m_pScene->m_nBroadcastRegion;
    nTop        = pToRegion->m_nRegionY + m_pScene->m_nBroadcastRegion;

    for (int nX = nLeft; nX <= nRight; nX++)
    {
        for (int nY = nBottom; nY <= nTop; nY++)
        {
            KRegion* pRegion = m_pScene->GetRegion(nX, nY);
            if (pRegion && pRegion->m_bRegionView)
            {
                // 将我注册到新Region内的Player对象身上
                pRegion->TraversePlayer(Func);
            }
        }
    }
}
#endif


#ifdef _SERVER
void KNpc::LootExp(KPlayer* pDropTarget)
{
    BOOL                    bRetCode                    = false;
    KTeam*	                pTeam                       = NULL;
    int					    nTotalLevel                 = 0;
	const float			    fPersonalCoefficientList[]  = {1.2f, 1.2f, 1.2f, 1.2f, 1.2f, 1.2f, 1.16f, 1.10f, 1.05f, 1.0f, 0.96f, 0.92f, 0.8f, 0.77f, 0.74f};
    float				    fPersonalCoefficient        = 0.0f; 
    int                     nExp                        = 0;
    int                     nAddExp                     = 0;
    int                     nPlayerCount                = 0;
    int                     nReputePercent              = KILO_NUM;
    std::vector<KPlayer*>   vecMemberList;

    assert(pDropTarget);

    KGLOG_PROCESS_ERROR(m_pScene);
	KGLOG_PROCESS_ERROR(m_pCell);
    
    nExp = m_pTemplate->nExp;
    if (nExp < 0)
    {
        KGLogPrintf(KGLOG_ERR, "[LOOTEXP] NpcExp < 0, id:%u, TemplateID:%u.\n", m_dwID, m_dwTemplateID);
        goto Exit0;
    }
    
    nExp = nExp * m_nLevel / (m_nLevel + m_nAllAttackerLevel);

    if (pDropTarget->m_dwTeamID == ERROR_ID)
    {
        int nDiffLevel = pDropTarget->m_nLevel - m_nLevel + 9;
		if (nDiffLevel > 14)
		{
			fPersonalCoefficient = 0;
		}
		else if (nDiffLevel < 0)
		{
			fPersonalCoefficient = 0;
		}
		else
		{
			fPersonalCoefficient = fPersonalCoefficientList[nDiffLevel];
		}

		// 获得经验
        nAddExp = (int)(nExp * fPersonalCoefficient * (1024 + pDropTarget->m_nExpPercent) / (float)1024); 
        pDropTarget->AddExp(nAddExp);

		// 获得声望
		for (int nReputationIndex = 0; nReputationIndex < MAX_NPC_REPUTATION; nReputationIndex++)
		{
			DWORD dwForceID = m_pTemplate->dwReputeID[nReputationIndex];
			int nReputation = m_pTemplate->nReputeValue[nReputationIndex];
			int nLowLimit = m_pTemplate->nReputeLowLimit[nReputationIndex];
			int nHighLimit = m_pTemplate->nReputeHighLimit[nReputationIndex];

			if (dwForceID == 0 || nReputation == 0)
				continue;

            // 获得声望调整
            nReputation = nReputation * (KILO_NUM + pDropTarget->m_nReputationPercent) / KILO_NUM;
            
            int nCampLevel = g_pSO3World->m_CampInfo.GetCampLevel();
            nReputePercent = g_pSO3World->m_CampInfo.GetReputePercent(pDropTarget->m_eCamp, nCampLevel);
            nReputation = nReputation * nReputePercent / KILO_NUM;

			bRetCode = pDropTarget->m_ReputeList.ModifyReputation(dwForceID, nReputation, nLowLimit, nHighLimit);
			KGLOG_PROCESS_ERROR(bRetCode);
		}
        
        goto Exit0;
    }

    pTeam = g_pSO3World->m_TeamServer.GetTeam(pDropTarget->m_dwTeamID);
    KGLOG_PROCESS_ERROR(pTeam);

	for (int i = 0; i < pTeam->nGroupNum; i++)
	{
        for (
            KTEAM_MEMBER_LIST::iterator it = pTeam->MemberGroup[i].MemberList.begin();
            it != pTeam->MemberGroup[i].MemberList.end(); ++it
        )
        {
            KPlayer* pMember = g_pSO3World->m_PlayerSet.GetObj(it->dwMemberID);

            if (!pMember)
                continue;

            if (pMember->m_pScene != m_pScene)
                continue;

            if (pMember->m_eMoveState == cmsOnDeath)
                continue;

            bRetCode = g_InRange(this, pMember, MAX_LOOT_RANGE);
            if (!bRetCode)
            {
                continue;
            }

            vecMemberList.push_back(pMember);
            nTotalLevel += pMember->m_nLevel;
        }
	}

    nPlayerCount = (int)vecMemberList.size();

    for (int i = 0; i < nPlayerCount; i++)
    {
        int nDiffLevel = 0;
		assert(nTotalLevel);

        KPlayer* pMember = vecMemberList[i];
		assert(pMember);
        
		nDiffLevel = pMember->m_nLevel - m_nLevel + 9;
		if (nDiffLevel > 14)
		{
			fPersonalCoefficient = 0;
		}
		else if (nDiffLevel < 0)
		{
			fPersonalCoefficient = 0;
		}
		else
		{
			fPersonalCoefficient = fPersonalCoefficientList[nDiffLevel];
		}

		// 获得经验
		nAddExp = (int)(nExp * fPersonalCoefficient * (1 + 0.1f * ((float)nPlayerCount - 1)) * 
            (pMember->m_nLevel / (float)nTotalLevel) * ((1024 + (float)pMember->m_nExpPercent) / (float)1024));
        
        pMember->AddExp(nAddExp);

		// 获得声望
		for (int nReputationIndex = 0; nReputationIndex < MAX_NPC_REPUTATION; nReputationIndex++)
		{
			DWORD dwForceID = m_pTemplate->dwReputeID[nReputationIndex];
			int nReputation = m_pTemplate->nReputeValue[nReputationIndex];
			int nLowLimit = m_pTemplate->nReputeLowLimit[nReputationIndex] ;
			int nHighLimit = m_pTemplate->nReputeHighLimit[nReputationIndex];
            int nCampLevel = 0;

			if (dwForceID == 0 || nReputation == 0)
				continue;

            nReputation = nReputation * (KILO_NUM + pMember->m_nReputationPercent) / KILO_NUM;
            
            nCampLevel = g_pSO3World->m_CampInfo.GetCampLevel();
            nReputePercent = g_pSO3World->m_CampInfo.GetReputePercent(pMember->m_eCamp, nCampLevel);
            nReputation = nReputation * nReputePercent / KILO_NUM;
            
            if (nReputation < 0) // 玩家在仇恨表中才会减声望
            {
                bRetCode = m_SimpThreatList.IsInThreatList(pMember);
                if (!bRetCode)
                    continue;
            }

			bRetCode = pMember->m_ReputeList.ModifyReputation(dwForceID, nReputation, nLowLimit, nHighLimit);
			KGLOG_PROCESS_ERROR(bRetCode);
		}
    }
        
Exit0:
    return;
}

void KNpc::LootPrestige(KPlayer* pDropTarget)
{
    BOOL                    bRetCode               = false;
    int                     nAddPrestige           = 0;
    float			        fPersonalCoefficient   = 1.0f; 
    KTeam*	                pTeam                  = NULL;
    int                     nPlayerCount           = 0;
    int					    nTotalLevel            = 0;
    std::vector<KPlayer*>   vecMemberList;

    assert(pDropTarget);

    KGLOG_PROCESS_ERROR(m_pTemplate);

    nAddPrestige = m_pTemplate->nPrestige;
	KG_PROCESS_ERROR(nAddPrestige > 0);
	
    if (pDropTarget->m_dwTeamID == ERROR_ID) // 单人情况下
    {
        int nDiffLevel = pDropTarget->m_nLevel - m_nLevel + 9;
		if (nDiffLevel > 14)
		{
			fPersonalCoefficient = 0;
		}
		else if (nDiffLevel < 0)
		{
			fPersonalCoefficient = 0;
		}

        nAddPrestige = (int)(nAddPrestige * fPersonalCoefficient);
        
        KG_PROCESS_ERROR(m_pTemplate->nCampLootPrestige & (0x1 << (int)pDropTarget->m_eCamp));

        pDropTarget->AddPrestige(nAddPrestige);

        goto Exit0;
    }
    
    // 组队情况
    pTeam = g_pSO3World->m_TeamServer.GetTeam(pDropTarget->m_dwTeamID);
    KGLOG_PROCESS_ERROR(pTeam);

    for (int i = 0; i < pTeam->nGroupNum; i++)
	{
        for (
            KTEAM_MEMBER_LIST::iterator it = pTeam->MemberGroup[i].MemberList.begin();
            it != pTeam->MemberGroup[i].MemberList.end(); ++it
        )
        {
            KPlayer* pMember = g_pSO3World->m_PlayerSet.GetObj(it->dwMemberID);

            if (!pMember)
                continue;

            if (pMember->m_pScene != m_pScene)
                continue;

            if (pMember->m_eMoveState == cmsOnDeath)
                continue;

            bRetCode = g_InRange(this, pMember, MAX_LOOT_RANGE);
            if (!bRetCode)
            {
                continue;
            }

            if (pMember->m_eCamp == cNeutral)
            {
                continue;
            }

            if (!(m_pTemplate->nCampLootPrestige & (0x1 << (int)pMember->m_eCamp)))
            {
                continue;
            }

            vecMemberList.push_back(pMember);
            nTotalLevel += pMember->m_nLevel;
        }
	}

    nPlayerCount = (int)vecMemberList.size();

    for (int i = 0; i < nPlayerCount; i++)
    {
        int nDiffLevel = 0;
        KPlayer* pMember = vecMemberList[i];
        
		nDiffLevel = pMember->m_nLevel - m_nLevel + 9;
		if (nDiffLevel > 14)
		{
			fPersonalCoefficient = 0;
		}
		else if (nDiffLevel < 0)
		{
			fPersonalCoefficient = 0;
		}

		nAddPrestige = (int)(
            nAddPrestige * (pMember->m_nLevel / (float)nTotalLevel) * 
            fPersonalCoefficient * (1 + 0.01f * ((float)(nPlayerCount - 1)))
        );
        
        pMember->AddPrestige(nAddPrestige);
    }

Exit0:
    return;
}

void KNpc::LootContribution(KPlayer* pDropTarget)
{
    BOOL                    bRetCode               = false;
    int                     nAddContribution       = 0;
    float			        fPersonalCoefficient   = 1.0f; 
    KTeam*	                pTeam                  = NULL;
    int                     nPlayerCount           = 0;
    std::vector<KPlayer*>   vecMemberList;

    assert(pDropTarget);

    KGLOG_PROCESS_ERROR(m_pTemplate);

    nAddContribution = m_pTemplate->nContribution;
	KG_PROCESS_ERROR(nAddContribution > 0);
	
    if (pDropTarget->m_dwTeamID == ERROR_ID) // 单人情况下
    {
        if (pDropTarget->m_dwTongID != ERROR_ID)
        {
            vecMemberList.push_back(pDropTarget);
        }
    }
    else
    {
        pTeam = g_pSO3World->m_TeamServer.GetTeam(pDropTarget->m_dwTeamID);
        KGLOG_PROCESS_ERROR(pTeam);

        for (int i = 0; i < pTeam->nGroupNum; i++)
	    {
            for (
                KTEAM_MEMBER_LIST::iterator it = pTeam->MemberGroup[i].MemberList.begin();
                it != pTeam->MemberGroup[i].MemberList.end(); ++it
            )
            {
                KPlayer* pMember = NULL;

                pMember = g_pSO3World->m_PlayerSet.GetObj(it->dwMemberID);
                if (!pMember)
                    continue;

                if (pMember->m_pScene != m_pScene)
                    continue;

                if (pMember->m_eMoveState == cmsOnDeath)
                    continue;

                bRetCode = g_InRange(this, pMember, MAX_LOOT_RANGE);
                if (!bRetCode)
                {
                    continue;
                }

                if (pMember->m_dwTongID == ERROR_ID)
                {
                    continue;
                }

                vecMemberList.push_back(pMember);
            }
	    }
    }
    
    nPlayerCount = (int)vecMemberList.size();
    KG_PROCESS_ERROR(nPlayerCount > 0);

    for (int i = 0; i < nPlayerCount; i++)
    {
        int         nDiffLevel           = 0;
        int         nRealAddContribution = nAddContribution;
        KPlayer*    pMember              = vecMemberList[i];
        
        nDiffLevel = pMember->m_nLevel - m_nLevel;
        MAKE_IN_RANGE(nDiffLevel, 0, 15);
        
        if (nDiffLevel > 5)
        {
            nRealAddContribution = nAddContribution * (100 - (nDiffLevel - 5) * 10) / 100;
        }
		
        pMember->AddContribution(nRealAddContribution);
    }

Exit0:
    return;
}

void KNpc::LootAchievement(KPlayer* pDropTarget)
{
    BOOL                    bRetCode               = false;
    int                     nAchievementID         = 0;
    float			        fPersonalCoefficient   = 1.0f; 
    KTeam*	                pTeam                  = NULL;
    int                     nPlayerCount           = 0;
    std::vector<KPlayer*>   vecMemberList;

    assert(pDropTarget);

    KGLOG_PROCESS_ERROR(m_pTemplate);

    nAchievementID = m_pTemplate->nAchievementID;
    KG_PROCESS_ERROR(nAchievementID >= 0 && nAchievementID < MAX_ACHIEVEMENT_ID);

    if (pDropTarget->m_dwTeamID == ERROR_ID)
    {
        pDropTarget->AcquireAchievement(nAchievementID);
    }
    else
    {
        pTeam = g_pSO3World->m_TeamServer.GetTeam(pDropTarget->m_dwTeamID);
        KGLOG_PROCESS_ERROR(pTeam);

        for (int i = 0; i < pTeam->nGroupNum; i++)
        {
            for (
                KTEAM_MEMBER_LIST::iterator it = pTeam->MemberGroup[i].MemberList.begin();
                it != pTeam->MemberGroup[i].MemberList.end(); ++it
            )
            {
                KPlayer* pMember = NULL;

                pMember = g_pSO3World->m_PlayerSet.GetObj(it->dwMemberID);
                if (!pMember)
                    continue;

                if (pMember->m_pScene != m_pScene)
                    continue;

                if (pMember->m_eMoveState == cmsOnDeath)
                    continue;

                bRetCode = g_InRange(this, pMember, MAX_LOOT_RANGE);
                if (!bRetCode)
                {
                    continue;
                }

                pMember->AcquireAchievement(nAchievementID);
            }
        }
    }

Exit0:
    return;
}

BOOL KNpc::SetScript(const char* pszFileName)
{
	BOOL bRetCode = false;
	DWORD dwScriptID = g_FileNameHash(pszFileName);

	bRetCode = g_pSO3World->m_ScriptCenter.IsScriptExist(dwScriptID);
	KGLOG_PROCESS_ERROR(bRetCode);

	m_dwScriptID = dwScriptID;

    InitDialogFlag();

	return true;
Exit0:
	return false;
}

BOOL KNpc::AdjustLevel()
{
    BOOL            bResult         = false;
	BOOL            bRetCode        = false;
	int             nRandomLevel    = 0;
	int             nOriginLevel    = 0;
    int             nPercent        = 0;

	KGLOG_PROCESS_ERROR(m_pTemplate);
    
    switch(m_pTemplate->nAdjustLevel)
    {
    case 1: // 向上浮动
        nRandomLevel = g_Random(2);
    	break;
    case 2: // 向下浮动
        nRandomLevel -= g_Random(2);
    	break;
    case 3: // 上下浮动
        nRandomLevel = g_Random(3) - 1;
        break;
    default:
        nRandomLevel = 0;
        break;
    }

	nOriginLevel = m_nLevel;
	m_nLevel = m_pTemplate->nLevel + nRandomLevel;
	
	// 等级必须大于0
	if (m_nLevel <= 0)
    {
		m_nLevel = m_pTemplate->nLevel;
    }

    nPercent = (int)(0.04f * (m_nLevel - nOriginLevel) * KILO_NUM);    

	CallAttributeFunction(atMaxLifePercentAdd, true, nPercent, 0);

	bResult = true;
Exit0:
	return bResult;
}

#endif //_SERVER

int KNpc::GetNpcRelation(KNpc* pTarget)
{
	int nRelation   = 0;
	int nValue      = 0;
    
    nValue = g_pSO3World->m_Settings.m_RelationForceList.GetRelation(m_dwForceID, pTarget->m_dwForceID);

	switch(nValue)
	{
	case -1:
		nRelation |= sortEnemy;
		break;

	case 1:
		nRelation |= sortAlly;
		break;

	default:
        nRelation |= sortNone;
		break;
	}

#if defined(_SERVER)
	if (m_SimpThreatList.IsInThreatList(pTarget))
#endif
#if defined(_CLIENT)
	if (m_ThreatList.IsInThreatList(pTarget->m_dwID))
#endif
	{
		nRelation |= sortEnemy;
		nRelation &= ~sortAlly;
	}

	if (this == pTarget)
	{
		nRelation = sortSelf;
	}	

	return nRelation;
}


int KNpc::GetPlayerRelation(KPlayer* pTarget)
{
	int nRelation = 0;
	int nValue = pTarget->m_ReputeList.GetReputeLevel(m_dwForceID);

	if (nValue <= ENEMY_REPUTE_LEVEL)
	{
		nRelation |= sortEnemy;
		nRelation &= ~sortAlly;
		nRelation &= ~sortNeutrality;
	}
	else if (nValue >= ALLY_REPUTE_LEVEL)
	{
		nRelation |= sortAlly;
		nRelation &= ~sortEnemy;
		nRelation &= ~sortNeutrality;
	}
	else
	{
		nRelation |= sortNeutrality;
		nRelation &= ~sortAlly;
		nRelation &= ~sortEnemy;
	}
#if defined(_SERVER)
	if (pTarget->m_SimpThreatList.IsInThreatenList(this))
#endif
#if defined(_CLIENT)
	if (pTarget->m_ThreatList.IsInThreatenList(m_dwID))
#endif
	{
		nRelation |= sortEnemy;
		nRelation &= ~sortAlly;
		nRelation &= ~sortNeutrality;
	}

	return nRelation;
}

size_t PainText2TalkData(size_t uBuffSize, BYTE* pbyBuff, const char cszText[])
{
    size_t  uResult     = 0;
    BOOL    bRetCode    = false;
    size_t  uTextLen    = 0;
    size_t  uBuffLen    = 0;

    assert(uBuffSize);

    uTextLen = strlen(cszText);
    uBuffLen = sizeof(BYTE) + uTextLen + sizeof('\0');

    KG_PROCESS_ERROR(uBuffLen <= uBuffSize);

    assert(tdbTotal <= UCHAR_MAX);

    *pbyBuff = (BYTE)tdbText;
    strcpy((char*)pbyBuff + sizeof(BYTE), cszText);

    uResult = uBuffLen;
Exit0:
    return uResult;
}

// 说话（屏幕近聊）
BOOL KNpc::Say(char* pszText, int nRange /* = 0 */)
{
    BOOL        bResult     = false;
#ifdef _SERVER
    BOOL        bRetCode    = false;
    size_t      uBuffLen    = 0;
    const char* pcszName    = NULL;
    BYTE        byBuff[MAX_CHAT_TEXT_LEN];

    assert(nRange >= 0);

    pcszName = GetName();
    KGLOG_PROCESS_ERROR(pcszName);

    uBuffLen = PainText2TalkData(MAX_CHAT_TEXT_LEN, byBuff, pszText);
    KGLOG_PROCESS_ERROR(uBuffLen);

    bRetCode = g_PlayerServer.DoTalkMessage(trNpcNearby, m_dwID, pcszName, ERROR_ID, NULL, uBuffLen, byBuff, nRange);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
#endif
	return bResult;
}

// 说话(场景喊话)
BOOL KNpc::Yell(char* pszText)
{
    BOOL        bResult     = false;
#ifdef _SERVER
    BOOL        bRetCode    = false;
    size_t      uBuffLen    = 0;
    const char* pcszName    = NULL;
    BYTE        byBuff[MAX_CHAT_TEXT_LEN];

    pcszName = GetName();
    KGLOG_PROCESS_ERROR(pcszName);

    uBuffLen = PainText2TalkData(MAX_CHAT_TEXT_LEN, byBuff, pszText);
    KGLOG_PROCESS_ERROR(uBuffLen);

    bRetCode = g_PlayerServer.DoTalkMessage(trNpcSence, m_dwID, pcszName, ERROR_ID, NULL, uBuffLen, byBuff);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
#endif
    return bResult;
}

// 说话(密语)
BOOL KNpc::Whisper(KPlayer* pTarget, char* pszText)
{
    BOOL        bResult     = false;
#ifdef _SERVER
    BOOL        bRetCode    = false;
    size_t      uBuffLen    = 0;
    const char* pcszName    = NULL;
    BYTE        byBuff[MAX_CHAT_TEXT_LEN];

    pcszName = GetName();
    KGLOG_PROCESS_ERROR(pcszName);

    uBuffLen = PainText2TalkData(MAX_CHAT_TEXT_LEN, byBuff, pszText);
    KGLOG_PROCESS_ERROR(uBuffLen);

    bRetCode = g_PlayerServer.DoTalkMessage(trNpcWhisper, m_dwID, pcszName, pTarget->m_dwID, NULL, uBuffLen, byBuff);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
#endif
    return bResult;
}

// 和指定玩家说话
BOOL KNpc::SayTo(KPlayer* pTarget, char* pszText)
{
    BOOL        bResult     = false;
#ifdef _SERVER
    BOOL        bRetCode    = false;
    size_t      uBuffLen    = 0;
    const char* pcszName    = NULL;
    BYTE        byBuff[MAX_CHAT_TEXT_LEN];

    pcszName = GetName();
    KGLOG_PROCESS_ERROR(pcszName);

    uBuffLen = PainText2TalkData(MAX_CHAT_TEXT_LEN, byBuff, pszText);
    KGLOG_PROCESS_ERROR(uBuffLen);

    bRetCode = g_PlayerServer.DoTalkMessage(trNpcSayTo, m_dwID, pcszName, pTarget->m_dwID, NULL, uBuffLen, byBuff);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
#endif
    return bResult;
}

// 向指定玩家大喊
BOOL KNpc::YellTo(KPlayer* pTarget, char* pszText)
{
    BOOL        bResult     = false;
#ifdef _SERVER
    BOOL        bRetCode    = false;
    size_t      uBuffLen    = 0;
    const char* pcszName    = NULL;
    BYTE        byBuff[MAX_CHAT_TEXT_LEN];

    pcszName = GetName();
    KGLOG_PROCESS_ERROR(pcszName);

    uBuffLen = PainText2TalkData(MAX_CHAT_TEXT_LEN, byBuff, pszText);
    KGLOG_PROCESS_ERROR(uBuffLen);

    bRetCode = g_PlayerServer.DoTalkMessage(trNpcYellTo, m_dwID, pcszName, pTarget->m_dwID, NULL, uBuffLen, byBuff);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
#endif
    return bResult;
}

BOOL KNpc::SayToTargetUseSentenceID(KPlayer* pTarget, DWORD dwSentenceID)
{
    BOOL        bResult     = false;
#ifdef _SERVER
    BOOL        bRetCode    = false;
    const char* pcszName    = NULL;

    pcszName = GetName();
    KGLOG_PROCESS_ERROR(pcszName);

    bRetCode = g_PlayerServer.DoTalkMessage(trNpcSayToTargetUseSentenceID, m_dwID, pcszName, pTarget->m_dwID, NULL, sizeof(DWORD), (BYTE*)&dwSentenceID);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
#endif
    return bResult;
}

BOOL KNpc::SetRevivePoint(int nX, int nY, int nZ, int nDirection)
{
    BOOL bResult = false;

    m_AIData.nOriginPosX = nX;
    m_AIData.nOriginPosY = nY;
    m_AIData.nOriginPosZ = nZ;
    m_AIData.nOriginDirection = nDirection;

    bResult = true;
Exit0:
    return bResult;
}

BOOL KNpc::GetRevivePoint(KScene* pScene, int* pnRetX, int* pnRetY, int* pnRetZ, int* pnRetDirection)
{
    BOOL    bResult     = false;
    int     nRetCode    = false;

    assert(pnRetX);
    assert(pnRetY);
    assert(pnRetZ);
    assert(pnRetDirection);

    *pnRetX = m_AIData.nOriginPosX;
    *pnRetY = m_AIData.nOriginPosY;
    *pnRetZ = m_AIData.nOriginPosZ;
    *pnRetDirection = m_AIData.nOriginDirection;

#ifdef _SERVER
    if (m_AIData.nPatrolPathID && pScene)
    {
        KPatrolGroup*   pGroup = NULL; 

        pGroup = pScene->m_AIPatrolGroupManager.GetPatrolGroup(m_AIData.nPatrolPathID);
        if (pGroup)
        {
            if (pGroup->nCurrentX || pGroup->nCurrentY || pGroup->nCurrentZ)
            {
                *pnRetX = pGroup->nCurrentX;
                *pnRetY = pGroup->nCurrentY;
                *pnRetZ = pGroup->nCurrentZ;
            }
        }
    }
#endif

Exit1:
    bResult = true;
Exit0:
    return bResult;
}

#ifdef _SERVER

BOOL KNpc::Revive()
{
    BOOL            bResult             = false;
    BOOL            bRetCode            = false;
    const char*     pszFunctionName     = "OnNpcRevive";
    int             nLuaTopIndex        = 0;

    SetMoveState(cmsOnStand);

    InvalidateMoveState(0, true);

    m_nCurrentLife          = m_nMaxLife;
    m_nCurrentMana          = m_nMaxMana;
	m_nPositiveShield       = 0;
	m_nNegativeShield       = 0;
    m_bSystemShield         = false;
    if (m_pTemplate)
    {
        m_dwForceID             = m_pTemplate->dwForceID;
    }

    ClearDropTarget();

    SelectTarget(ttNoTarget, 0);

    m_AIData.nEscapeCount = 0;
    m_AIData.nCannotAttactCount = 0;
    m_AIData.nSkillFailedCount = 0;
    m_AIData.nSkillType = 0;
    m_AIData.dwRespondCharacterID = 0;

    KG_PROCESS_ERROR(m_dwScriptID);

    bRetCode = g_pSO3World->m_ScriptCenter.IsScriptExist(m_dwScriptID);
    KG_PROCESS_ERROR(bRetCode);

    bRetCode = g_pSO3World->m_ScriptCenter.IsFuncExist(m_dwScriptID, pszFunctionName);
    KG_PROCESS_ERROR(bRetCode);

	g_pSO3World->m_ScriptCenter.SafeCallBegin(&nLuaTopIndex);

	g_pSO3World->m_ScriptCenter.PushValueToStack(this);

	g_pSO3World->m_ScriptCenter.CallFunction(m_dwScriptID, pszFunctionName, 0);

	g_pSO3World->m_ScriptCenter.SafeCallEnd(nLuaTopIndex);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KNpc::CallDeathScript(DWORD dwKiller)
{
    BOOL            bResult     = false;
    BOOL            bRetCode    = false;
    int             nTopIndex   = 0;
    
    bRetCode = g_pSO3World->m_ScriptCenter.IsScriptExist(m_dwScriptID);
    KG_PROCESS_SUCCESS(!bRetCode);

    bRetCode = g_pSO3World->m_ScriptCenter.IsFuncExist(m_dwScriptID, SCRIPT_ON_NPC_DEATH);
    KG_PROCESS_SUCCESS(!bRetCode);

	g_pSO3World->m_ScriptCenter.SafeCallBegin(&nTopIndex);

	g_pSO3World->m_ScriptCenter.PushValueToStack(this);

    if (dwKiller != ERROR_ID)
    {
	    if (IS_PLAYER(dwKiller))
        {
            KPlayer* pPlayer = g_pSO3World->m_PlayerSet.GetObj(dwKiller);
            if (pPlayer)
            {
				g_pSO3World->m_ScriptCenter.PushValueToStack(pPlayer);
            }
        }
	    else
        {
            KNpc* pNpc = g_pSO3World->m_NpcSet.GetObj(dwKiller);
            if (pNpc)
            {
				g_pSO3World->m_ScriptCenter.PushValueToStack(pNpc);
            }
        }
    }

	g_pSO3World->m_ScriptCenter.CallFunction(m_dwScriptID, SCRIPT_ON_NPC_DEATH, 0);
	g_pSO3World->m_ScriptCenter.SafeCallEnd(nTopIndex);

Exit1:
    bResult = true;
Exit0:
    return bResult;
}

#endif

#if defined(_SERVER)
BOOL KNpc::SetDisappearFrames(int nFrames, BOOL bDisappearToReviveList)
{
	BOOL bResult = false;
	KGLOG_PROCESS_ERROR(nFrames >= 0);

	if (nFrames == 0)
	{
		m_nDisappearFrames = 0;
	}
	else
	{
		m_nDisappearFrames = nFrames + g_pSO3World->m_nGameLoop;
	}

	m_bDisappearToReviveList = bDisappearToReviveList;

	bResult = true;
Exit0:
	return bResult;
}

BOOL KNpc::GotoReviveList()
{
    BOOL     bResult        = false;
	BOOL     bRetCode       = false;
    BOOL     bNpcDeleteFalg = false;
    KScene*  pScene         = NULL;

	KGLOG_PROCESS_ERROR(m_pScene);

	bRetCode = m_SimpThreatList.ClearAllThreat();
	KGLOG_CHECK_ERROR(bRetCode);

	bRetCode = m_SimpThreatList.ClearAllThreaten();
	KGLOG_CHECK_ERROR(bRetCode);

	m_nConvergenceSpeed = 0;
	m_eMoveState        = cmsOnDeath;
	m_nDivingCount      = 0;

    m_BuffList.CleanBuffByPersist();

    pScene = m_pScene;
    KGLOG_PROCESS_ERROR(pScene);

    if (m_AIData.nPatrolPathID)
        m_pScene->m_AIPatrolGroupManager.LeaveGroup(m_AIData.nPatrolPathID, m_dwID);

    g_pSO3World->RemoveNpc(this, false);

    bNpcDeleteFalg = true;

	// 重生时间为0的,默认不加入重生队列
	if (m_nReviveTime)
	{
	    bRetCode = pScene->AddReviveNpc(this, m_nReviveTime);
	    KG_PROCESS_ERROR(bRetCode);
        bNpcDeleteFalg = false;
	}

	bResult = true;
Exit0:
    if (bNpcDeleteFalg)
	{	
		g_pSO3World->DeleteNpc(this);
	}
	return bResult;
}

BOOL KNpc::GetAutoDialogString(KPlayer* pPlayer, char* szBuffer, size_t uBufferSize)
{
	BOOL	    bResult         = false;
	int		    nResult         = 0;
	int		    nIndex          = 0;
    int         nReputeLevel    = 0;
    QUEST_STATE eQuestState     = qsUnfinished;
    size_t	    uStrLen         = 0;
	char	    szTempStr[256];

	assert(pPlayer);
	assert(szBuffer);
	assert(uBufferSize > 0);
	KGLOG_PROCESS_ERROR(m_pTemplate);

    nReputeLevel = pPlayer->m_ReputeList.GetReputeLevel(m_pTemplate->dwForceID);

	//		判断任务是否完成
	if (m_pTemplate->dwDialogQuestID)
	{
		eQuestState = pPlayer->m_QuestList.GetQuestState(m_pTemplate->dwDialogQuestID);	
	}

	if (eQuestState == qsFinished)
	{
		// 显示任务完成的阶段对话
		uStrLen = strlen(m_pTemplate->szDialogAfterQuest);
		KGLOG_PROCESS_ERROR(uStrLen <= uBufferSize);

		strncpy(szBuffer, m_pTemplate->szDialogAfterQuest, uBufferSize);
		szBuffer += uStrLen;
		uBufferSize -= uStrLen;
	}
	else if (m_pTemplate->nTotalRate > 0)
	{
		// 在3个待机对话中随机一个
		int nRandom = g_Random(m_pTemplate->nTotalRate);

		for (nIndex = 0; nIndex < MAX_NPC_DIALOG_COUNT; nIndex++)
		{
			if (nRandom < m_pTemplate->nDialogRate[nIndex])
				break;
			
			nRandom -= m_pTemplate->nDialogRate[nIndex];
		}

		assert(nIndex < MAX_NPC_DIALOG_COUNT);

		uStrLen = strlen(m_pTemplate->szDialog[nIndex]);
		KGLOG_PROCESS_ERROR(uStrLen <= uBufferSize);

		strncpy(szBuffer, m_pTemplate->szDialog[nIndex], uBufferSize);
		szBuffer += uStrLen;
		uBufferSize -= uStrLen;
	}

	// 插入\n<H28>
	snprintf(szTempStr, 256, "\n<H28>");
	uStrLen = strlen(szTempStr);
	KGLOG_PROCESS_ERROR(uStrLen <= uBufferSize);

	strncpy(szBuffer, szTempStr, uBufferSize);
	szBuffer += uStrLen;
	uBufferSize -= uStrLen;
	
	// 商店
	if (m_pShop && m_pTemplate->szShopOptionText[0] != '\0' && nReputeLevel >= m_pTemplate->nShopRequireReputeLevel)
	{
		snprintf(szTempStr, 256, "<M %lu %s>", m_pShop->m_dwShopID, m_pTemplate->szShopOptionText);
		uStrLen = strlen(szTempStr);
		KGLOG_PROCESS_ERROR(uStrLen <= uBufferSize);

		strncpy(szBuffer, szTempStr, uBufferSize);
		szBuffer += uStrLen;
		uBufferSize -= uStrLen;
	}
	
	// 银行
	if (m_pTemplate->bHasBank && m_pTemplate->szBankOptionText[0] != '\0' && nReputeLevel >= m_pTemplate->nBankRequireReputeLevel)
	{
		snprintf(szTempStr, 256, "<B%s>", m_pTemplate->szBankOptionText);
		uStrLen = strlen(szTempStr);
		KGLOG_PROCESS_ERROR(uStrLen <= uBufferSize);

		strncpy(szBuffer, szTempStr, uBufferSize);
		szBuffer += uStrLen;
		uBufferSize -= uStrLen;
	}

	// 邮箱
	if (m_pTemplate->bHasMailBox && m_pTemplate->szMailBoxOptionText[0] != '\0' && nReputeLevel >= m_pTemplate->nMailBoxRequireReputeLevel)
	{
		snprintf(szTempStr, 256, "<L%s>", m_pTemplate->szMailBoxOptionText);
		uStrLen = strlen(szTempStr);
		KGLOG_PROCESS_ERROR(uStrLen <= uBufferSize);

		strncpy(szBuffer, szTempStr, uBufferSize);
		szBuffer += uStrLen;
		uBufferSize -= uStrLen;
	}

	// 技能训练师
	if (m_pTemplate->dwMasterID && m_pTemplate->szMasterOptionText[0] != '\0' && nReputeLevel >= m_pTemplate->nMasterRequireReputeLevel)
	{
		snprintf(szTempStr, 256, "<K %lu %s>", m_pTemplate->dwMasterID, m_pTemplate->szMasterOptionText);
		uStrLen = strlen(szTempStr);
		KGLOG_PROCESS_ERROR(uStrLen <= uBufferSize);

		strncpy(szBuffer, szTempStr, uBufferSize);
		szBuffer += uStrLen;
		uBufferSize -= uStrLen;
	}

	// 生活技能学习师
	if (m_pTemplate->dwCraftMasterID && m_pTemplate->szCraftMasterOptionText[0] != '\0' && nReputeLevel >= m_pTemplate->nCraftMasterRequireReputeLevel)
	{
		snprintf(szTempStr, 256, "<E %lu %s>", m_pTemplate->dwCraftMasterID, m_pTemplate->szCraftMasterOptionText);
		uStrLen = strlen(szTempStr);
		KGLOG_PROCESS_ERROR(uStrLen <= uBufferSize);

		strncpy(szBuffer, szTempStr, uBufferSize);
		szBuffer += uStrLen;
		uBufferSize -= uStrLen;
	}

    // 寄卖行
    if (m_pTemplate->bHasAuction && m_pTemplate->szAuctionOptionText[0] != '\0' && nReputeLevel >= m_pTemplate->nAuctionRequireReputeLevel)
    {
        snprintf(szTempStr, 256, "<Y%s>", m_pTemplate->szAuctionOptionText);
        uStrLen = strlen(szTempStr);
        KGLOG_PROCESS_ERROR(uStrLen <= uBufferSize);

        strncpy(szBuffer, szTempStr, uBufferSize);
        szBuffer += uStrLen;
        uBufferSize -= uStrLen;
    }
	
	// 插入\n\n
	snprintf(szTempStr, 256, "<H28>");
	uStrLen = strlen(szTempStr);
	KGLOG_PROCESS_ERROR(uStrLen <= uBufferSize);

	strncpy(szBuffer, szTempStr, uBufferSize);
	szBuffer += uStrLen;
	uBufferSize -= uStrLen;

	// 任务
    if (nReputeLevel >= m_pTemplate->nQuestRequireReputeLevel)
    {
        nResult = g_pSO3World->m_Settings.m_QuestInfoList.GetNpcQuestString(m_pScene->m_dwMapID, m_dwTemplateID, szBuffer, (int)uBufferSize);
        KGLOG_PROCESS_ERROR(nResult <= (int)uBufferSize);
        szBuffer += nResult;
        uBufferSize -= nResult;
    }
	
	KGLOG_PROCESS_ERROR(uBufferSize > 0);
	szBuffer[0] = '\0';

	bResult = true;
Exit0:
	return bResult;
}

KDoodad* KNpc::GenerateCorpse(void)
{
    KDoodad*        pResult         = NULL;
	BOOL            bRetCode        = false;
	KDoodad*        pDoodad         = NULL;
	BOOL            bDoodadRegFlag  = false;
    BOOL            bDoodadInitFlag = false;

    pDoodad = KMemory::New<KDoodad>();
    KGLOG_PROCESS_ERROR(pDoodad);

	bRetCode = g_pSO3World->m_DoodadSet.Register(pDoodad, ERROR_ID);
	KGLOG_PROCESS_ERROR(bRetCode);
	bDoodadRegFlag = true;

    bRetCode = pDoodad->Init();
    KGLOG_PROCESS_ERROR(bRetCode);
    bDoodadInitFlag = true;
	
	// 尸体可以被生活技能操作,所以设成可以拣两次;
	pDoodad->m_nLastLootCount = 2;
    	
	KGLOG_PROCESS_ERROR(m_pTemplate);

	bRetCode = g_pSO3World->m_Settings.m_DoodadTemplateList.LoadFromTemplate(m_pTemplate->nCorpseDoodadID, pDoodad);
	KGLOG_PROCESS_ERROR(bRetCode);

	strncpy(pDoodad->m_szName, m_pTemplate->szName, _NAME_LEN); 
	pDoodad->m_dwRepresentID    = m_dwModelID;
	pDoodad->m_dwNpcTemplateID  = m_dwTemplateID;

	pDoodad->m_nFaceDirection   = m_nFaceDirection;

	bRetCode = pDoodad->SetDelayRemove();
	KGLOG_PROCESS_ERROR(bRetCode);
	
	bRetCode = g_pSO3World->AddDoodad(pDoodad, m_pScene, m_nX, m_nY, m_nZ);
	KGLOG_PROCESS_ERROR(bRetCode);

	pResult = pDoodad;
Exit0:
    if (pResult == NULL)
    {
        if (bDoodadInitFlag)
        {
            pDoodad->UnInit();
            bDoodadInitFlag = false;
        }

		if (bDoodadRegFlag)
		{
			g_pSO3World->m_DoodadSet.Unregister(pDoodad);
			bDoodadRegFlag = false;
		}

        if (pDoodad)
        {
            KMemory::Delete(pDoodad);
            pDoodad = NULL;
        }
    }
	return pResult;
}

KLootList* KNpc::GenerateLootList(KPlayer* pDropTarget, KDoodad* pNpcCorpse)
{
    KLootList*          pResult         = NULL;
    KLootList*          pLootList       = NULL;
	BOOL                bRetCode        = false;
	DWORD               dwParentID      = 0;
	DWORD               dwDropClassID   = 0;
    int                 nMoney          = 0;
    std::vector<DWORD>  vecLooterList;
    
    assert(pDropTarget);
    assert(pNpcCorpse);

    KGLOG_PROCESS_ERROR(m_pTemplate);
    KGLOG_PROCESS_ERROR(pNpcCorpse->m_pTemplate);
    KGLOG_PROCESS_ERROR(m_pScene);

    pLootList = KMemory::New<KLootList>();
	KGLOG_PROCESS_ERROR(pLootList);

	bRetCode = pLootList->Init(pNpcCorpse->m_pTemplate->m_nOverLootFrame);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = pLootList->GenerateLooterList(pDropTarget, pNpcCorpse);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = pLootList->GetNearByLooterList(pNpcCorpse, vecLooterList);
	KGLOG_PROCESS_ERROR(bRetCode);    

    if (m_bDropNotQuestItemFlag)
    {
        // 地图掉落
        bRetCode = g_pSO3World->m_DropCenter.NpcMapDrop(pLootList, vecLooterList, m_pScene->m_dwMapID);
	    KGLOG_PROCESS_ERROR(bRetCode);
    	
	    // 分类掉落
	    if (m_pTemplate->nDropClassID != 0)
	    {
		    dwParentID = m_pTemplate->nDropClassID;
		    do 
		    {
			    dwDropClassID = dwParentID;

			    bRetCode = g_pSO3World->m_DropCenter.NpcClassDrop(pLootList, vecLooterList, dwDropClassID, m_nLevel);
			    KGLOG_PROCESS_ERROR(bRetCode);

			    dwParentID = g_pSO3World->m_Settings.m_NpcClassList.GetParentID(dwDropClassID);
		    } while(dwParentID != dwDropClassID);
	    }

	    // NPC模板掉落
	    for (int nDropIndex = 0; nDropIndex < MAX_DROP_PER_NPC; nDropIndex++)
	    {
		    int nItemCount = m_pTemplate->nDropCount[nDropIndex];
		    for (int nCountIndex = 0; nCountIndex < nItemCount; nCountIndex++)
		    {		
			    bRetCode = g_pSO3World->m_DropCenter.NpcTemplateDrop(pLootList, vecLooterList, m_dwTemplateID, nDropIndex);
			    KGLOG_PROCESS_ERROR(bRetCode);
		    }
	    }
    }
	
	// 任务掉落
    bRetCode = g_pSO3World->m_DropCenter.NpcDropQuestItem(m_dwTemplateID, pDropTarget, pLootList, vecLooterList);
    KGLOG_PROCESS_ERROR(bRetCode);

	// 注册刷怪数量
	for (int nLooterIndex = 0; nLooterIndex < (int)vecLooterList.size(); nLooterIndex++)
	{
		KPlayer* pPlayer = g_pSO3World->m_PlayerSet.GetObj(vecLooterList[nLooterIndex]);
		if (pPlayer == NULL)
			continue;
	
		pPlayer->m_QuestList.OnKillNpc(m_dwTemplateID);
        pPlayer->m_PQList.OnKillNpc(m_dwTemplateID);
	}

	// 金钱掉落
	nMoney = g_pSO3World->m_DropCenter.NpcDropMoney(m_dwTemplateID);

	bRetCode = pLootList->AddMoney(nMoney);
	KGLOG_PROCESS_ERROR(bRetCode);

	pLootList->ProcessNpcDropItems();

	pResult = pLootList;
Exit0:
	if (pResult == NULL)
	{
        if (pLootList)
        {
            pLootList->UnInit();
		    KMemory::Delete(pLootList);
            pLootList = NULL;
        }
	}
	return pResult;
}

void KNpc::ClearDropTarget()
{
    m_dwDropTargetPlayerID      = ERROR_ID;
    m_bDropNotQuestItemFlag     = true;
    m_nAllAttackerLevel         = 0;
}

void KNpc::InitDialogFlag()
{
    BOOL bRetCode    = false;
    BOOL bDialogFlag = false;

    // 存在脚本OnDialogue接口
    bRetCode = g_pSO3World->m_ScriptCenter.IsScriptExist(m_dwScriptID);
    if (bRetCode &&
        g_pSO3World->m_ScriptCenter.IsFuncExist(m_dwScriptID, SCRIPT_ON_DIALOGUE)
    )
    {
        bDialogFlag = true;
        goto Exit1;
    }

    
    if (
        (m_pTemplate->dwDialogQuestID)              || // 待机任务ID非空
        (m_pTemplate->nTotalRate > 0)               || // 待机对话不为空
        (m_pTemplate->szShopOptionText[0] != '\0')  || // 是商店
        (m_pTemplate->bHasBank)                     || // 是银行
        (m_pTemplate->bHasMailBox)                  || // 是邮箱
        (m_pTemplate->dwMasterID)                   || // 是技能训练师
        (m_pTemplate->dwCraftMasterID)                 // 生活技能训练师
    )
    {
        bDialogFlag = true;
        goto Exit1;
    }

    
    // 任务
    bRetCode = g_pSO3World->m_Settings.m_QuestInfoList.IsNpcQuestExist(m_pTemplate->dwTemplateID);
    if (bRetCode)
    {
        bDialogFlag = true;
        goto Exit1;
    }

Exit1:
    m_bDialogFlag = bDialogFlag;
//Exit0:
    return;
}
#endif //_SERVER
