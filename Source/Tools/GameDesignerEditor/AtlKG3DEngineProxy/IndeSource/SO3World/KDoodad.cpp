#include "stdafx.h"
#include "KDoodad.h"
#include "KScene.h"
#include "KPlayerServer.h"
#include "KLootList.h"
#include "KCraft.h"
#include "Engine/KMemory.h"
#include "KSO3World.h"

#ifdef _CLIENT
#include "SO3UI/SO3GameWorldUIHandler.h"
#include "SO3Represent/SO3GameWorldRepresentHandler.h"
#endif

BOOL KDoodad::Init(void)
{
    BOOL bResult  = false;
	BOOL bRetCode = false;

	bRetCode = KSceneObject::Init();
	KGLOG_PROCESS_ERROR(bRetCode);

	m_dwTemplateID          = 0;
	m_eState                = dsIdle;
    m_szName[0]             = '\0';
	m_eKind                 = dkInvalid;
	m_nOpenFrames           = 0;
	m_nDisappearFrames      = 0;
	m_bDisappearToReviveList = false;
	m_dwRepresentID		    = 0;
	m_dwNpcTemplateID		= 0;
	m_dwScriptID            = 0;
	m_bLooted				= false;
	m_pLootList             = NULL;
	m_dwReviveID            = 0;
	m_nCurrentReliveIndex   = -1;
    m_nObstacleGroup        = INVALID_OBSTACLE_GROUP;

#ifdef _SERVER
	m_nLastLootCount		= 0;
	m_dwOpenPlayerID        = ERROR_ID;
	m_dwOwnerID				= ERROR_ID;
#endif

#ifdef _CLIENT
	m_nUpdateMiniMapMarkFrame = 0;
#endif //_CLIENT

	m_pTemplate				= NULL;

	bResult = true;
Exit0:
	return bResult;
}

void KDoodad::UnInit()
{
	if (m_pLootList)
	{
		m_pLootList->UnInit();
		KMemory::Delete(m_pLootList);
        m_pLootList = NULL;
    }

	KSceneObject::UnInit();
}

BOOL KDoodad::Activate(void)
{
	BOOL bRetCode = false;

	bRetCode = KSceneObject::CheckGameLoop();
	if (!bRetCode)
		return true;

#ifdef _SERVER
	if (m_pLootList && (g_pSO3World->m_nGameLoop - m_dwID) % (GAME_FPS * 3) == 0)
	{
		m_pLootList->Activate(this);
	}
	
	if (m_dwOpenPlayerID != ERROR_ID && ((g_pSO3World->m_nGameLoop - m_dwID) % GAME_FPS == 0))
	{
		KPlayer* pOpener = g_pSO3World->m_PlayerSet.GetObj(m_dwOpenPlayerID);
        if (pOpener == NULL && m_eKind != dkDoor)
        {
            ChangeState(dsIdle);
        }
        else if (m_eKind == dkChair)
        {
            if (pOpener->m_eMoveState != cmsOnSit)
            {
                ChangeState(dsIdle);
            }
        }
    }

	if (m_nDisappearFrames != 0 && m_nDisappearFrames <= g_pSO3World->m_nGameLoop)
	{
		Remove(m_bDisappearToReviveList);
	}
		
#endif //_SERVER

#ifdef _CLIENT
	if (m_nUpdateMiniMapMarkFrame <= g_pSO3World->m_nGameLoop)
	{
		m_nUpdateMiniMapMarkFrame = g_pSO3World->m_nGameLoop + UPDATE_MINIMAP_MARK_INTERVAL;
		UpdateMiniMapMark(); //更新小地图标记
	}
#endif //_CLIENT

	return true;
}

BOOL KDoodad::Load(const KDOODAD_DATA& crDoodadData)
{
	BOOL	bResult  = false;
	BOOL	bRetCode = false;

	bRetCode = g_pSO3World->m_Settings.m_DoodadTemplateList.LoadFromTemplate(crDoodadData.dwTemplateID, this);
	KGLOG_PROCESS_ERROR(bRetCode);

	m_dwTemplateID	  = crDoodadData.dwTemplateID;
	m_nX			  = crDoodadData.nX;
	m_nY		      = crDoodadData.nY;
	m_nZ			  = crDoodadData.nZ;
	m_nFaceDirection  = crDoodadData.byFaceDirection;
    m_nObstacleGroup  = crDoodadData.wObstacleGroup;
	m_dwReviveID	  = crDoodadData.dwDoodadReliveID;

    if (crDoodadData.dwScriptID)
    {
	    m_dwScriptID  = crDoodadData.dwScriptID;
    }

    if (crDoodadData.szNickName[0] != '\0')
    {
	    strncpy(m_szName, crDoodadData.szNickName, sizeof(m_szName));
	    m_szName[sizeof(m_szName) - 1] = 0;
    }

	bResult = true;
Exit0:
	return bResult;
}

void KDoodad::ChangeState(DOODAD_STATE eState)
{
    BOOL bRetCode = false;

    KG_PROCESS_ERROR(m_eState != eState);

    switch(eState)
    {
    case dsIdle:
#ifdef _SERVER
        m_dwOpenPlayerID = ERROR_ID;
#endif
        DoIdle();
        break;
    case dsBeHit:
        DoBeHit();
        break;
    case dsAfterHit:
        DoAfterHit();
        break;
    default:
        assert(false);
    }

#ifdef _SERVER
    g_PlayerServer.DoSyncDoodadState(this);
#endif

Exit0:
    return;
}

BOOL KDoodad::CheckOpen(KPlayer* pOpener)
{	
    BOOL             bResult            = false;
    BOOL             bRetCode		    = false;
    BOOL             bScriptRetCode		= false;
    std::multimap<DWORD, DWORD>::iterator it;

    assert(pOpener);

#ifdef _CLIENT
    if (pOpener->m_eMoveState != cmsOnStand && pOpener->m_eMoveState != cmsOnFloat)
    {
        if (g_pGameWorldUIHandler)
        {
            g_pGameWorldUIHandler->OnOpenDoodadMoveStateError();
        }
        goto Exit0;
    }
#else
    KG_PROCESS_ERROR(pOpener->m_eMoveState == cmsOnStand || pOpener->m_eMoveState == cmsOnFloat);
#endif

    KGLOG_PROCESS_ERROR(m_pTemplate);

    bRetCode = CheckDistance(pOpener);
    KG_PROCESS_ERROR(bRetCode);

    switch (m_eKind)
    {
    case dkNormal:		// 普通型的Doodad,只显示Tip
    case dkOrnament:	// 装饰类的Doodad,所有人都不能操作
    case dkGuide:
        goto Exit0;
        break;
    case dkQuest:
        // 任务,这个点之前要判断下玩家身上的任务状态,有激活并且未完成的任务才能点	
        bRetCode = HaveQuest(pOpener);
        KG_PROCESS_ERROR(bRetCode);
        break;
    case dkRead:		// 可阅读的Doodad
    case dkDialog:		// 可对话的Doodad
    case dkAcceptQuest:	// 可以接任务的Doodad
    case dkTreasure:	// 宝箱类Doodad,都可以操作
    case dkChair:		// 椅子也是谁都能操作的
        break;
    case dkCorpse:
    case dkCraftTarget:
        if (m_eKind == dkCraftTarget || (m_eKind == dkCorpse && m_bLooted))
        {
            CRAFT_RESULT_CODE   eResult = crcFailed;
            DWORD               dwRecipeID = 0;
            KTarget             Target;

            KGLOG_PROCESS_ERROR(m_pTemplate->m_dwCraftID);
            KG_PROCESS_ERROR(IS_PLAYER(pOpener->m_dwID));

            bRetCode = Target.SetTarget(this);
            KGLOG_PROCESS_ERROR(bRetCode);

            dwRecipeID = GetRecipeID();
            KGLOG_PROCESS_ERROR(dwRecipeID != 0);

            eResult = pOpener->CanCastProfessionSkill(m_pTemplate->m_dwCraftID, dwRecipeID, 0, Target);
            KG_PROCESS_ERROR(eResult == crcSuccess);
        }
        break;
    case dkDoor:		// 门是可以操作的
        break;
    default:
        KGLOG_PROCESS_ERROR(false);
    }

#ifdef _SERVER
    // m_bCanOperateEach决定一个Doodad能不能被多人同时操作  门在AfterHit状态下也可以操作
    if (!m_pTemplate->m_bCanOperateEach && m_eState == dsAfterHit && m_eKind != dkDoor)
        goto Exit0;

    bRetCode = g_pSO3World->m_ScriptCenter.IsScriptExist(m_dwScriptID);
    if (bRetCode && g_pSO3World->m_ScriptCenter.IsFuncExist(m_dwScriptID, SCRIPT_ON_OPEN_DOODAD))
    {
        int nTopIndex = 0;
        g_pSO3World->m_ScriptCenter.SafeCallBegin(&nTopIndex);

        g_pSO3World->m_ScriptCenter.PushValueToStack(this);
        g_pSO3World->m_ScriptCenter.PushValueToStack(pOpener);

        g_pSO3World->m_ScriptCenter.CallFunction(m_dwScriptID, SCRIPT_ON_OPEN_DOODAD, 1);
        bRetCode = g_pSO3World->m_ScriptCenter.GetValuesFromStack("b", &bScriptRetCode);
        g_pSO3World->m_ScriptCenter.SafeCallEnd(nTopIndex);

        KGLOG_PROCESS_ERROR(bRetCode);
        KG_PROCESS_ERROR(bScriptRetCode);
    }
#endif

    bResult = true;
Exit0:
    return bResult;
}

BOOL KDoodad::HaveQuest(KPlayer* pPlayer)
{
    BOOL            bResult         = false;
    BOOL            bRetCode        = false;
    BOOL            bDropFlag       = false;
    KQuestInfo*     pQuestInfo      = NULL;
    int             nQuestPhase     = 0;
    std::multimap<DWORD, DWORD>::iterator it;
    std::multimap<DWORD, DWORD>::iterator UpperBoundIt;

    assert(pPlayer);

    it = g_pSO3World->m_Settings.m_QuestInfoList.m_mapRDoodadID2QuestID.lower_bound(m_dwTemplateID);
    UpperBoundIt = g_pSO3World->m_Settings.m_QuestInfoList.m_mapRDoodadID2QuestID.upper_bound(m_dwTemplateID);
    for (; it != UpperBoundIt; ++it)
    {
        nQuestPhase = pPlayer->m_QuestList.GetQuestPhase(it->second);
        KG_PROCESS_SUCCESS(nQuestPhase == 1);
    }

    it = g_pSO3World->m_Settings.m_QuestInfoList.m_mapDoodadID2QuestID.lower_bound(m_dwTemplateID);
    UpperBoundIt = g_pSO3World->m_Settings.m_QuestInfoList.m_mapDoodadID2QuestID.upper_bound(m_dwTemplateID);
    for (; it != UpperBoundIt; ++it)
    {
        nQuestPhase = pPlayer->m_QuestList.GetQuestPhase(it->second);
        if (nQuestPhase != 1)
            continue;

        pQuestInfo = g_pSO3World->m_Settings.m_QuestInfoList.GetQuestInfo(it->second);
        KG_PROCESS_ERROR(pQuestInfo);

        for (int nIndex = 0; nIndex < QUEST_END_ITEM_COUNT; nIndex++)
        {
            if (pQuestInfo->dwDropItemDoodadTemplateID[nIndex] == m_dwTemplateID)
            {
                bDropFlag = true;

                bRetCode = pPlayer->m_QuestList.IsQuestEndItemEnoughByIndex(pQuestInfo, nIndex, true);
                KG_PROCESS_SUCCESS(!bRetCode);
            }
        }

        // 如果不是掉落类Doodad,那么始终是可以点击的; 这种doodad通常是用来设置任务变量的
        KG_PROCESS_SUCCESS(!bDropFlag);
    }

    goto Exit0; // 没找到;该类doodad上的任务掉落已经足够

Exit1:
    bResult = true;
Exit0:
    return bResult;
}

int KDoodad::GetOpenFrame(KPlayer* pOpener)
{
    int                 nResult         = 0;
    BOOL                bRetCode        = false;

    assert(pOpener);

    KGLOG_PROCESS_ERROR(m_pTemplate);

    if (m_eKind == dkCraftTarget || (m_eKind == dkCorpse && m_bLooted))
    {
        KCraft*         pCraft              = NULL;
        KRecipeBase*    pRecipe             = NULL;
        DWORD           dwRecipeID          = 0;
        int             nProfessionLevel    = 0;
        int             nRecipeLevel        = 0;
        int             nDiffLevel          = 0;

        KG_PROCESS_ERROR(m_pTemplate->m_dwCraftID);

        pCraft = g_pSO3World->m_ProfessionManager.GetCraft(m_pTemplate->m_dwCraftID);
        KGLOG_PROCESS_ERROR(pCraft);

        dwRecipeID = pCraft->GetRecipeIDByDoodadTemplateID(m_dwTemplateID);
        KGLOG_PROCESS_ERROR(dwRecipeID != 0);

        pRecipe = pCraft->GetRecipe(dwRecipeID);
        KGLOG_PROCESS_ERROR(pRecipe);

        nProfessionLevel = pOpener->m_ProfessionList.GetProfessionLevel(pRecipe->dwProfessionID);
        nRecipeLevel = pRecipe->dwRequireProfessionLevel;
        nDiffLevel = nProfessionLevel - nRecipeLevel;

        if (nDiffLevel < 0)
            nDiffLevel = 0;
        else if (nDiffLevel > 10)
            nDiffLevel = 10;

        nResult = (int)((pRecipe->nPrepareFrame) * (1.0 - 0.06 * nDiffLevel));
    }
    else
    {
        nResult = m_pTemplate->m_dwOpenPrepareFrame;
    }

Exit0:
    return nResult;
}

DWORD KDoodad::GetRecipeID()
{
    DWORD               dwRecipeID      = 0;
    KCraft*             pCraft          = NULL;

    KGLOG_PROCESS_ERROR(m_pTemplate);

    pCraft = g_pSO3World->m_ProfessionManager.GetCraft(m_pTemplate->m_dwCraftID);
    KGLOG_PROCESS_ERROR(pCraft);

    dwRecipeID = pCraft->GetRecipeIDByDoodadTemplateID(m_dwTemplateID);

Exit0:
    return dwRecipeID;
}

BOOL KDoodad::CheckDistance(KPlayer* pOpener)
{
    BOOL bResult    = false;
    return bResult;
}

void KDoodad::DoIdle()
{
    m_eState = dsIdle;

#ifdef _CLIENT
    if (g_pGameWorldRepresentHandler)
    {
        g_pGameWorldRepresentHandler->OnDoodadIdle(this);
    }
#endif
    return;
}

void KDoodad::DoBeHit()
{
    m_eState = dsBeHit;

#ifdef _CLIENT
    if (g_pGameWorldRepresentHandler)
    {
        g_pGameWorldRepresentHandler->OnDoodadBeHit(this);
    }
#endif
    return;
}

void KDoodad::DoAfterHit()
{
    m_eState = dsAfterHit;

#ifdef _CLIENT
    if (g_pGameWorldRepresentHandler)
    {
        g_pGameWorldRepresentHandler->OnDoodadAfterHit(this);
    }
#endif

    return;
}

#ifdef _CLIENT
void KDoodad::OpenLootList(KCharacter* pCharacter)
{
	if (m_pLootList && pCharacter)
	{
        if (g_pGameWorldUIHandler)
        {
            KUIEventOpenDoodad OpenDoodad;
            OpenDoodad.dwDoodadID = m_dwID;
            OpenDoodad.dwPlayerID = pCharacter->m_dwID;
            g_pGameWorldUIHandler->OnOpenDoodad(OpenDoodad);
        }
	}

	return;
}

void KDoodad::UpdateMiniMapMark()
{
    KPlayer*    pPlayer     = NULL;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);

    if (pPlayer->m_eRadarType == mrtFindCraftDoodad)
    {
        KGLOG_PROCESS_ERROR(m_pTemplate);
        if ((int)(m_pTemplate->m_dwCraftID) == pPlayer->m_nRadarParam)
        {
            if (g_pGameWorldUIHandler)
            {
                KUIEventUpdateMiniMapDoodadMark UIParam;
                UIParam.dwDoodadID = m_dwID;
                UIParam.nX = m_nX;
                UIParam.nY = m_nY;
                UIParam.nParam = pPlayer->m_nRadarParam;

                g_pGameWorldUIHandler->OnUpdateMiniMapDoodadMark(UIParam);
            }
        }
    }

Exit0:
    return;
}
#endif

#ifdef _SERVER
void KDoodad::LootOver()
{
	--m_nLastLootCount;
	m_bLooted = true;
    
	if (m_nLastLootCount <= 0)
	{
		Remove();
        goto Exit0;
	}
    
    if (m_pLootList)
    {
	    m_pLootList->UnInit();
	    KMemory::Delete(m_pLootList);
	    m_pLootList = NULL;
    }

    g_PlayerServer.DoDoodadLootOver(this); // 广播该尸体可以搜索了

Exit0:
    return;
}

BOOL KDoodad::SetDelayRemove()
{
    BOOL    bResult             = false;
	int     nDisappearFrames    = GetDisappearFrames();
		
	if (nDisappearFrames == 0) // 只有未被设置延时删除的Doodad可以设置
	{
        KGLOG_PROCESS_ERROR(m_pTemplate);
		SetDisappearFrames(m_pTemplate->m_nRemoveDelay);
	}

    bResult = true;
Exit0:
	return bResult;
}

void KDoodad::SetDisappearFrames(int nDisappearFrames, BOOL bDisappearToReviveList)
{
	KGLOG_PROCESS_ERROR(nDisappearFrames >= 0);

    m_nDisappearFrames = (nDisappearFrames == 0) ? 0 : (nDisappearFrames + g_pSO3World->m_nGameLoop);
	m_bDisappearToReviveList = bDisappearToReviveList;
Exit0:
	return;
}

int KDoodad::GetDisappearFrames()
{
	return m_nDisappearFrames;
}

void KDoodad::Revive()
{
    BOOL            bRetCode            = false;
    const char*     pszFunctionName     = "OnDoodadRevive";
    int             nLuaTopIndex        = 0;

    ChangeState(dsIdle);

	m_nLastLootCount = 1;

    KG_PROCESS_ERROR(m_dwScriptID);

    bRetCode = g_pSO3World->m_ScriptCenter.IsScriptExist(m_dwScriptID);
    KG_PROCESS_ERROR(bRetCode);

    bRetCode = g_pSO3World->m_ScriptCenter.IsFuncExist(m_dwScriptID, pszFunctionName);
    KG_PROCESS_ERROR(bRetCode);

    g_pSO3World->m_ScriptCenter.SafeCallBegin(&nLuaTopIndex);

    g_pSO3World->m_ScriptCenter.PushValueToStack(this);

    g_pSO3World->m_ScriptCenter.CallFunction(m_dwScriptID, pszFunctionName, 0);

    g_pSO3World->m_ScriptCenter.SafeCallEnd(nLuaTopIndex);

Exit0:
    return;
}

BOOL KDoodad::PerOpen(KPlayer* pPlayer)
{
    BOOL             bResult         = false;
    BOOL             bRetCode        = false;	

    KGLOG_PROCESS_ERROR(pPlayer);

    KGLOG_PROCESS_ERROR(m_pTemplate);
    KG_PROCESS_ERROR(m_pTemplate->m_bCanOperateEach || m_eState != dsAfterHit || m_eKind == dkDoor);

    // 除尸体外每次都调用OnPerOpen脚本
    if (m_eKind != dkCorpse)
    {
        bRetCode = g_pSO3World->m_ScriptCenter.IsScriptExist(m_dwScriptID);
        if (bRetCode && g_pSO3World->m_ScriptCenter.IsFuncExist(m_dwScriptID, SCRIPT_ON_PER_OPEN_DOODAD))
        {
            int nTopIndex = 0;
            g_pSO3World->m_ScriptCenter.SafeCallBegin(&nTopIndex);

            g_pSO3World->m_ScriptCenter.PushValueToStack(this);
            g_pSO3World->m_ScriptCenter.PushValueToStack(pPlayer);

            g_pSO3World->m_ScriptCenter.CallFunction(m_dwScriptID, SCRIPT_ON_PER_OPEN_DOODAD, 0);	
            g_pSO3World->m_ScriptCenter.SafeCallEnd(nTopIndex);

            KGLOG_PROCESS_ERROR(m_pRegion);
        }
    }

    // 椅子的特殊处理
    if (m_eKind == dkChair)
    {
        pPlayer->DoSitDown(this);
    }

    KG_PROCESS_SUCCESS(!m_pTemplate->m_bCanPick);	// 之后只有有拾取的Doodad才走

    if (m_pLootList == NULL)
    {
        if (m_eKind == dkCorpse || m_eKind == dkCraftTarget)
        {
            KCraft*             pCraft      = NULL;
            CRAFT_RESULT_CODE   eRetCode    = crcFailed;
            DWORD               dwRecipeID  = 0;
            KTarget             Target;
            
            KG_PROCESS_ERROR(m_pTemplate->m_dwCraftID != 0);

            bRetCode = Target.SetTarget(this);
            KGLOG_PROCESS_ERROR(bRetCode);

            pCraft = g_pSO3World->m_ProfessionManager.GetCraft(m_pTemplate->m_dwCraftID);
            KGLOG_PROCESS_ERROR(pCraft);

            dwRecipeID = GetRecipeID();
            KGLOG_PROCESS_ERROR(dwRecipeID != 0);

            eRetCode = pCraft->Cast(pPlayer, dwRecipeID, 0, Target);
            KGLOG_PROCESS_ERROR(eRetCode == crcSuccess);
        }
        else
        {
            // 生活技能相关的Doodad除外(CraftTarget或者Corpse)
            m_pLootList = GenerateLootList(pPlayer);
            KGLOG_PROCESS_ERROR(m_pLootList);
        }

        bRetCode = g_pSO3World->m_ScriptCenter.IsScriptExist(m_dwScriptID);
        if (bRetCode && g_pSO3World->m_ScriptCenter.IsFuncExist(m_dwScriptID, SCRIPT_ON_PICK_DOODAD))
        {
            // 后走脚本
            int nTopIndex = 0;
            g_pSO3World->m_ScriptCenter.SafeCallBegin(&nTopIndex);

            g_pSO3World->m_ScriptCenter.PushValueToStack(this);
            g_pSO3World->m_ScriptCenter.PushValueToStack(pPlayer);

            g_pSO3World->m_ScriptCenter.CallFunction(m_dwScriptID, SCRIPT_ON_PICK_DOODAD, 0);	
            g_pSO3World->m_ScriptCenter.SafeCallEnd(nTopIndex);

            KGLOG_PROCESS_ERROR(m_pRegion);
        }
    }

    if (m_eKind != dkCorpse || m_bLooted) // 尸体掉落在生成的时候已经同步过
    {
        g_PlayerServer.DoSyncLootList(pPlayer, this);
    }

    if (m_pLootList)
    {
        m_pLootList->OnOpen(this);
    }

Exit1:
    bResult = true;
Exit0:
    return bResult;
}

BOOL KDoodad::SetScript(const char* pcszFileName)
{
    BOOL  bResult       = false;
	BOOL  bRetCode      = false;
    DWORD dwScriptID    = g_FileNameHash(pcszFileName);
	
	bRetCode = g_pSO3World->m_ScriptCenter.IsScriptExist(dwScriptID);
	KGLOG_PROCESS_ERROR(bRetCode);

	m_dwScriptID = dwScriptID;

	bResult = true;
Exit0:
	return bResult;
}

KLootList* KDoodad::GenerateLootList(KPlayer* pOpener)
{
    BOOL				bRetCode		        = false;
    int					nResult			        = 0;
    int                 nMoney                  = 0;
    KLootList*			pLootList		        = NULL;
    KDoodadClass*		pDoodadClass	        = NULL;

    assert(pOpener);

    KGLOG_PROCESS_ERROR(m_pTemplate);

    pLootList = KMemory::New<KLootList>();
    KGLOG_PROCESS_ERROR(pLootList);

    bRetCode = pLootList->Init(m_pTemplate->m_nOverLootFrame);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = pLootList->GenerateLooterList(pOpener, this);
    KGLOG_PROCESS_ERROR(bRetCode);

    // doodad模板掉落
    for (DWORD dwDropIndex = 0; dwDropIndex < MAX_DROP_PER_DOODAD; ++dwDropIndex)
    {
        DWORD dwItemCount = m_pTemplate->m_dwDropCount[dwDropIndex];
        for (DWORD dwCountIndex = 0; dwCountIndex < dwItemCount; ++dwCountIndex)
        {
            bRetCode = g_pSO3World->m_DropCenter.DoodadTemplateDrop(
                pLootList, pOpener, m_dwTemplateID, dwDropIndex
            );
            KGLOG_PROCESS_ERROR(bRetCode);
        }
    }

    pDoodadClass = g_pSO3World->m_Settings.m_DoodadClassList.GetData(m_pTemplate->m_dwClassID);
    KGLOG_PROCESS_ERROR(pDoodadClass);

    // doodad分类掉落
    for (DWORD dwDropIndex = 0; dwDropIndex < MAX_DROP_PER_DOODAD; ++dwDropIndex)
    {
        DWORD dwItemCount = pDoodadClass->dwDropCount[dwDropIndex];
        for (DWORD dwCountIndex = 0; dwCountIndex < dwItemCount; ++dwCountIndex)
        {
            bRetCode = g_pSO3World->m_DropCenter.DoodadClassDrop(
                pLootList, pOpener, m_pTemplate->m_dwClassID, dwDropIndex, m_pTemplate->m_nLevel
            );
            KGLOG_PROCESS_ERROR(bRetCode);
        }
    }

    nMoney = g_pSO3World->m_DropCenter.DoodadDropMoney(m_dwTemplateID);

    bRetCode = pLootList->AddMoney(nMoney);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = g_pSO3World->m_DropCenter.DoodadDropQuestItem(m_dwTemplateID, pOpener, pLootList);
    KGLOG_PROCESS_ERROR(bRetCode);
    
    return pLootList;
Exit0:
    if (pLootList)
    {
        pLootList->UnInit();
        KMemory::Delete(pLootList);
        pLootList = NULL;
    }
    return NULL;
}

KLootList* KDoodad::GenerateEmptyLootList(void)
{
    KLootList*          pResult             = NULL;
    BOOL                bRetCode            = false;
    KLootList*          pLootList           = NULL;

    KGLOG_PROCESS_ERROR(m_pTemplate);

    pLootList = KMemory::New<KLootList>();
    KGLOG_PROCESS_ERROR(pLootList);

    bRetCode = pLootList->Init(m_pTemplate->m_nOverLootFrame);
    KGLOG_PROCESS_ERROR(bRetCode);

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

void KDoodad::Close()
{
    KG_PROCESS_ERROR(m_pLootList);

    if (m_pLootList->IsEmpty())
    {
        LootOver(); //这个函数里可能删掉Doodad,也可能删掉LootList,所以之后不要做什么操作
    }

Exit0:
    return;
}

void KDoodad::Remove(BOOL bGotoReviveList)
{
    BOOL bRetCode = false;

    KScene* pScene = m_pScene;
    KGLOG_PROCESS_ERROR(pScene);

    // 因为重生的时候使用的坐标为绝对坐标，
    // 所以在死之前，先把自己身上的坐标转换为绝对坐标
    bRetCode = GetAbsoluteCoordinate(&m_nX, &m_nY, &m_nZ);
    KGLOG_PROCESS_ERROR(bRetCode);

    // 重生时间为“0”的Doodad不加入重生队列
    if (bGotoReviveList == false || !(m_pTemplate->m_nReviveDelay))
    {
        g_pSO3World->DelDoodad(this);
        goto Exit0;
    }

    //重置Doodad成员
    if (m_pLootList)
    {
        m_pLootList->UnInit();
        KMemory::Delete(m_pLootList);
        m_pLootList = NULL;
    }
    m_nDisappearFrames = 0;

    bRetCode = g_pSO3World->RemoveDoodad(this);
    KGLOG_CHECK_ERROR(bRetCode);

    bRetCode = pScene->AddReviveDoodad(this);
    if (!bRetCode)
    {
        g_pSO3World->DelDoodad(this);
        goto Exit0;
    }

Exit0:
    return;
}
#endif
