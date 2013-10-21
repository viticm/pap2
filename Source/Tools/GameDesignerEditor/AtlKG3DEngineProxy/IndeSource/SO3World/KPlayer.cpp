#include "stdafx.h"
#include "KPlayer.h"
#include "KScene.h"
#include "KAttrModifier.h"
#include "KSkill.h"
#include "KTradingBox.h"
#include "KRecipe.h"
#include "KCraft.h"
#include "KRecipeMaster.h"
#include "KSO3World.h"

#ifdef _CLIENT
#include "SO3Represent/SO3GameWorldRepresentHandler.h"
#include "SO3UI/SO3GameWorldUIHandler.h"
#include "KPlayerClient.h"
#endif

#ifdef _SERVER
#include "KRoleDBDataDef.h"
#include "KPlayerServer.h"
#include "KRelayClient.h"
#include "Common/CRC32.h"
#endif

#ifdef _CLIENT
BYTE     KPlayer::s_byTalkData[];
unsigned KPlayer::s_uTalkDataSize = 0;
BOOL     KPlayer::s_bFilterTalkText = false;
#endif

KPlayer::KPlayer()
{
	m_dwTeamID              = ERROR_ID;
	m_dwTongID              = ERROR_ID;
	m_nContribution         = 0;
	m_pTradingBox           = NULL;
	m_nMaxLevel             = 0;
	m_bHideHat              = false;
}

KPlayer::~KPlayer()
{
    assert(!m_pTradingBox);
}

BOOL KPlayer::Init(void)
{
    BOOL bResult                    = false;
	BOOL bRetCode                   = false;
    BOOL bCharacterInitFlag         = false;
    BOOL bQuestListInitFlag         = false;
    BOOL bTimerListInitFlag         = false;
    BOOL bItemListInitFlag          = false;
    BOOL bUserPreferencesInitFlag   = false;
    BOOL bBookListInitFlag          = false;
    BOOL bPKInitFlag                = false;
    BOOL bScriptTimerListInitFlag   = false;
    BOOL bSkillRecipeListInitFlag   = false;
    BOOL bProfessionListInitFlag    = false;
    BOOL bRecipeListInitFlag        = false;

	bRetCode = KCharacter::Init();
	KGLOG_PROCESS_ERROR(bRetCode);
    bCharacterInitFlag = true;

	bRetCode = m_QuestList.Init(this);
	KGLOG_PROCESS_ERROR(bRetCode);
    bQuestListInitFlag = true;

    bRetCode = m_TimerList.Init(this);
	KGLOG_PROCESS_ERROR(bRetCode);
    bTimerListInitFlag = true;

	bRetCode = m_ItemList.Init(this);
	KGLOG_PROCESS_ERROR(bRetCode);
    bItemListInitFlag = true;

	bRetCode = m_UserPreferences.Init(this);
	KGLOG_PROCESS_ERROR(bRetCode);
    bUserPreferencesInitFlag = true;

	bRetCode = m_BookList.Init(this);
	KGLOG_PROCESS_ERROR(bRetCode);
    bBookListInitFlag = true;

	bRetCode = m_PK.Init(this);
	KGLOG_PROCESS_ERROR(bRetCode);
    bPKInitFlag = true;

	m_eKind                 = ckPlayer;

    memset(&m_MoveCtrl, 0, sizeof(m_MoveCtrl));

#ifdef _CLIENT
    m_szTongName[0] = '\0';
#endif

#ifdef _SERVER
	m_nConnIndex            = -1;
	m_nTimer                = 0;
    m_nlLeftTime            = 0;
    m_nLastClientFrame      = 0;
    m_nNextSaveFrame        = 0;
	m_dwRoundRollCount      = 0;
    m_dwLastScriptID        = 0;
    m_nBanishTime           = 0;
    m_nLastLoginTime        = 0;
    m_nCurrentLoginTime     = 0;
    m_nCreateTime           = 0;
    m_nTotalGameTime        = 0;
    memset(&m_LastEntry, 0, sizeof(m_LastEntry));
    
    m_nExpPercent           = 0;
    m_nReputationPercent    = 0;
#endif

	m_eCamp                 = cNeutral;
    m_nCurrentPrestige      = 0;
#ifdef _SERVER
    m_nClientCampInfoVersion = -1;
#endif
    m_dwSystemTeamID        = ERROR_ID;
    m_dwTeamID              = ERROR_ID;
    m_nBattleFieldSide      = INVALID_BATTLE_SIDE;
	m_eGameStatus           = gsInvalid;

	m_eRoleType	            = rtInvalid;
    m_bFightState           = false;
    m_nLevel                = 0;

	m_nExperience	        = 0;

	m_dwBankNpcID			= ERROR_ID;
    
    m_pTradingBox           = NULL;
    m_dwTradingInviteDst    = ERROR_ID;
    m_dwTradingInviteSrc    = ERROR_ID;

    m_bOnHorse              = false;

    m_bOnPracticeRoom           = false;     
    m_nCurrentTrainValue        = 0;
    m_nMaxTrainValue            = 0;
    m_nUsedTrainValue           = 0;
#ifdef _SERVER
    m_nAddTrainTimeInToday      = 0;
#endif
    m_nBaseVenationCof          = 0;
    m_nPhysicsVenationCof       = 0;
    m_nMagicVenationCof         = 0;
    m_nAssistVenationCof        = 0;

#ifdef _SERVER
    memset(m_nBuffDecayCount, 0, sizeof(m_nBuffDecayCount));
    memset(m_nBuffDecayNextFrame, 0, sizeof(m_nBuffDecayNextFrame));
#endif

    m_SkillRecipeList.Clear();

#ifdef _CLIENT
	m_eRadarType				    = mrtNoRadar;
	m_nRadarParam				    = 0;
#endif
	m_nCurrentStamina		        = INT_MAX / 2;
    m_nMaxStamina                   = 0;
    m_nCurrentThew		            = INT_MAX / 2;
    m_nMaxThew                      = 0;

#ifdef _SERVER
	m_nLastSaveTime                 = 0;
	
    bRetCode = m_ScriptTimerList.Init(this);
    KGLOG_PROCESS_ERROR(bRetCode);
    bScriptTimerListInitFlag = true;
#endif

	memset(m_wRepresentId, 0, sizeof(m_wRepresentId));
    m_dwRepresentIdLock = 0;

    m_dwSchoolID = 0;

    m_SkillList.Init(this);

    bRetCode = m_SkillRecipeList.Init(this);
    KGLOG_PROCESS_ERROR(bRetCode);
    bSkillRecipeListInitFlag = true;

	bRetCode = m_ProfessionList.Init(this);
	KGLOG_PROCESS_ERROR(bRetCode);
    bProfessionListInitFlag = true;

	bRetCode = m_RecipeList.Init(this);
	KGLOG_PROCESS_ERROR(bRetCode);
    bRecipeListInitFlag = true;

	bRetCode = m_ReputeList.Init(this);
	KGLOG_PROCESS_ERROR(bRetCode);

#ifdef _SERVER
    m_dwKillerID                                = ERROR_ID;         
    m_nVirtualFrame                             = g_pSO3World->m_nGameLoop;   
    m_nRecordCount                              = 0;
	m_nThreatTime			                    = PLAYER_THREAT_TIME;

	m_nLastSyncSelfMaxLife                      = -1;
	m_nLastSyncSelfMaxMana                      = -1; 
    m_nLastSyncSelfMaxStamina                   = -1;
	m_nLastSyncSelfCurrentLife                  = -1;
	m_nLastSyncSelfCurrentMana                  = -1; 
    m_nLastSyncSelfCurrentStamina               = -1;
    m_nLastSyncSelfCurrentThew                  = -1;
    m_nLastSyncSelfUpperWeak                    = -1;
    m_nLastSyncSelfMiddleWeak                   = -1;
    m_nLastSyncSelfLowerWeak                    = -1;
    
	m_nLastSyncTargetMaxLife                    = -1;
	m_nLastSyncTargetMaxMana                    = -1; 
	m_nLastSyncTargetLifePercent                = -1;
	m_nLastSyncTargetManaPercent                = -1;
    m_nLastSyncTargetUpperWeak                  = -1;
    m_nLastSyncTargetMiddleWeak                 = -1;
    m_nLastSyncTargetLowerWeak                  = -1;

    m_dwLastSyncTargetTargetID                  = ERROR_ID;
    m_nLastSyncTargetTargetMaxLife              = -1;
    m_nLastSyncTargetTargetMaxMana              = -1;
    m_nLastSyncTargetTargetLifePercent          = -1;
    m_nLastSyncTargetTargetManaPercent          = -1;
    
    m_dwTargetDropID                            = ERROR_ID;
    m_dwTargetTargetBuffCRC                     = 0;
	m_dwTargetBuffCRC                           = 0;

    ResetPartyLastSyncParamRecord();
#endif

#ifdef _CLIENT
    m_dwFormationEffectID               = 0;
    m_nFormationEffectLevel             = 0;
#endif

    memset(&m_ReviveParam, 0, sizeof(m_ReviveParam));
    memset(&m_ReviveCtrl, 0, sizeof(m_ReviveCtrl));

    m_OpenRouteNodeList.clear();

    m_nCurrentKillPoint         = 0;
#ifdef _SERVER
    m_nLastKillPointReduceTime  = 0;
    m_nBanTime                  = 0;
  
    m_bAbradeEquipmentDouble    = false;
    m_bAbradeEquipmentAll       = false;
    m_bAttackByGuard            = false;
#endif
    m_bOnlyReviveInSitu         = false;            
    m_bCannotDialogWithNPC      = false;
    m_bRedName                  = false;
    
    m_bHeroFlag                 = false;

    m_nVitalityToParryValueCof              = 0;
    m_nVitalityToMaxLifeCof                 = 0;
    m_nVitalityToPhysicsAttackPowerCof      = 0;
    m_nVitalityToSolarAttackPowerCof        = 0;
    m_nVitalityToLunarAttackPowerCof        = 0;
    m_nVitalityToNeutralAttackPowerCof      = 0;
    m_nVitalityToPoisonAttackPowerCof       = 0;
    m_nVitalityToTherapyPowerCof            = 0;
    m_nVitalityToPhysicsShieldCof           = 0;
    m_nSpiritToMaxManaCof                   = 0;
    m_nSpiritToSolarCriticalStrikeCof       = 0;
    m_nSpiritToLunarCriticalStrikeCof       = 0;
    m_nSpiritToNeutralCriticalStrikeCof     = 0;
    m_nSpiritToPoisonCriticalStrikeCof      = 0;
    m_nSpiritToPhysicsAttackPowerCof        = 0;
    m_nSpiritToPhysicsCriticalStrikeCof     = 0;
    m_nSpiritToManaReplenishCof             = 0;
    m_nSpiritToSolarMagicShieldCof          = 0;
    m_nSpiritToLunarMagicShieldCof          = 0;
    m_nSpiritToNeutralMagicShieldCof        = 0;
    m_nSpiritToPoisonMagicShieldCof         = 0;
    m_nSpunkToManaReplenishCof              = 0;
    m_nSpunkToSolarAttackPowerCof           = 0;
    m_nSpunkToLunarAttackPowerCof           = 0;
    m_nSpunkToNeutralAttackPowerCof         = 0;
    m_nSpunkToPoisonAttackPowerCof          = 0;
    m_nStrengthToPhysicsCriticalStrikeCof   = 0;
    m_nStrengthToPhysicsAttackPowerCof      = 0;
    m_nAgilityToPhysicsCriticalStrikeCof    = 0;

	bResult = true;
Exit0:
    if (!bResult)
    {
        if (bRecipeListInitFlag)
        {
            m_RecipeList.UnInit();
            bRecipeListInitFlag = false;
        }

        if (bProfessionListInitFlag)
        {
            m_ProfessionList.UnInit();
            bProfessionListInitFlag = false;
        }
        
        if (bSkillRecipeListInitFlag)
        {
            m_SkillRecipeList.UnInit();
            bSkillRecipeListInitFlag = false;
        }

#ifdef _SERVER
        if (bScriptTimerListInitFlag)
        {
            m_ScriptTimerList.UnInit();
            bScriptTimerListInitFlag = false;
        }
#endif
        
        if (bPKInitFlag)
        {
            m_PK.UnInit();
            bPKInitFlag = false;
        }
        
        if (bBookListInitFlag)
        {
            m_BookList.UnInit();
            bBookListInitFlag = false;
        }
            
        if (bUserPreferencesInitFlag)
        {
            m_UserPreferences.UnInit();
            bUserPreferencesInitFlag = false;
        }

        if (bItemListInitFlag)
        {
            m_ItemList.UnInit();
            bItemListInitFlag = false;
        }

        if (bTimerListInitFlag)
        {
            m_TimerList.UnInit();
            bTimerListInitFlag = false;
        }
        
        if (bQuestListInitFlag)
        {
            m_QuestList.UnInit();
            bQuestListInitFlag = false;
        }

        if (bCharacterInitFlag)
        {
            KCharacter::UnInit();
            bCharacterInitFlag = false;
        }
    }
	return bResult;
}

void KPlayer::UnInit(void)
{
    if (m_pTradingBox)
    {
        KPlayer*        pOtherPlayer     = NULL;
        KTradingBox*    pOtherTradingBox = NULL;

        pOtherPlayer = m_pTradingBox->GetOtherSide();
        assert(pOtherPlayer);

        pOtherTradingBox = pOtherPlayer->m_pTradingBox;
        assert(pOtherTradingBox);

        pOtherTradingBox->UnInit();
        KMemory::Delete(pOtherTradingBox);
        pOtherTradingBox = NULL;

        pOtherPlayer->m_pTradingBox = NULL;
        pOtherPlayer->m_dwTradingInviteSrc = ERROR_ID;
        pOtherPlayer->m_dwTradingInviteDst = ERROR_ID;

        m_pTradingBox->UnInit();
        KMemory::Delete(m_pTradingBox);
        m_pTradingBox = NULL;

#ifdef _SERVER
	    g_PlayerServer.DoSyncTradingConfirm(pOtherPlayer->m_nConnIndex, m_dwID, false);
#endif	//_SERVER
    }

#ifdef _SERVER
    g_pSO3World->m_FellowshipMgr.UnloadPlayerFellowship(m_dwID);

	m_ScriptTimerList.UnInit();
#endif //_SERVER

    m_dwTradingInviteSrc = ERROR_ID;
    m_dwTradingInviteDst = ERROR_ID;

    m_SkillList.UnInit();
    m_SkillRecipeList.UnInit();

	m_RecipeList.UnInit();
	m_ProfessionList.UnInit();    
	m_PK.UnInit();
	m_BookList.UnInit();
	m_UserPreferences.UnInit();
	m_ItemList.UnInit();
    m_TimerList.UnInit();
	KCharacter::UnInit();

	return;
}

BOOL KPlayer::Activate(void)
{
    BOOL bResult  = false;
	BOOL bRetCode = false;

Exit0:
	return bResult;
}

#ifdef _SERVER
#define ACTIVATE "Activate"
void KPlayer::ScriptActivate()
{
    BOOL    bRetCode        = false;
    int     nLuaTopIndex    = 0;

    bRetCode = g_pSO3World->m_ScriptCenter.IsScriptExist(PLAYER_SCRIPT);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = g_pSO3World->m_ScriptCenter.IsFuncExist(PLAYER_SCRIPT, ACTIVATE);
    KGLOG_PROCESS_ERROR(bRetCode);

	g_pSO3World->m_ScriptCenter.SafeCallBegin(&nLuaTopIndex);

	g_pSO3World->m_ScriptCenter.PushValueToStack(this);

	g_pSO3World->m_ScriptCenter.CallFunction(PLAYER_SCRIPT, ACTIVATE, 0);

	g_pSO3World->m_ScriptCenter.SafeCallEnd(nLuaTopIndex);

Exit0:
    return;
}
#endif

BOOL KPlayer::OpenDoodad(KDoodad* pDoodad)
{
    BOOL             bResult            = false;
	BOOL             bRetCode           = false;
    KDoodadTemplate* pDoodadTemplate    = NULL;
    int              nOpenFrames        = 0;

    assert(pDoodad);

#ifdef _SERVER
	bRetCode = pDoodad->CheckOpen(this);
	KG_PROCESS_ERROR(bRetCode);
#endif
    
    KGLOG_PROCESS_ERROR(pDoodad->m_pTemplate);
    pDoodadTemplate = pDoodad->m_pTemplate;

    nOpenFrames = pDoodad->GetOpenFrame(this);
    bRetCode    = DoPickPrepare(pDoodad, nOpenFrames);
    KG_PROCESS_SUCCESS(!bRetCode); // 为false表示OTAction不能覆盖,直接返回

#ifdef _SERVER
    if (nOpenFrames > 0 && m_bOnHorse)
    {
        DownHorse();
    }
#else
    if (m_dwID == g_pSO3World->m_dwClientPlayerID)
    {
        if (g_pGameWorldUIHandler)
        {
            KUIEventPickProgress param;
            param.nTotalFrame = nOpenFrames;
            param.dwDoodadID = pDoodad->m_dwID;
            g_pGameWorldUIHandler->OnPickPrepareProgress(param);
        }
    }
#endif
    
#ifdef _SERVER
	g_PlayerServer.DoCharacterOpen(this, pDoodad);
    pDoodad->m_dwOpenPlayerID = m_dwID;
	pDoodad->ChangeState(dsBeHit);
#endif 

#ifdef _CLIENT
	if (g_pGameWorldRepresentHandler)
        g_pGameWorldRepresentHandler->OnCharacterOpenDoodad(this, pDoodad);
#endif

Exit1:
	bResult = true;
Exit0:
	return bResult;
}

void KPlayer::MoveCtrl()
{
    KG_PROCESS_ERROR(m_pScene);

#ifdef _SERVER
    if (m_MoveCtrl.bUpdate)
    {
        g_PlayerServer.DoSyncMoveCtrl(this);
        m_MoveCtrl.bUpdate = false;
    }
#endif

    KG_PROCESS_ERROR(!m_MoveCtrl.bDisable);

    if (m_MoveCtrl.nTurn)
    {
        if (
            m_eMoveState == cmsOnStand || m_eMoveState == cmsOnWalk || m_eMoveState == cmsOnRun ||
            m_eMoveState == cmsOnJump || m_eMoveState == cmsOnSwimJump || m_eMoveState == cmsOnSwim ||
            m_eMoveState == cmsOnFloat || m_eMoveState == cmsOnSit
        )
        {
            m_nFaceDirection += m_MoveCtrl.nTurn * DIRECTION_TURN_SPEED;
            m_nFaceDirection = m_nFaceDirection % DIRECTION_COUNT;
            if (m_nFaceDirection < 0)
            {
                m_nFaceDirection += DIRECTION_COUNT;
            }
        }

        if (m_eMoveState == cmsOnSit)
        {
            m_nConvergenceSpeed = 0;
            m_nDestX            = m_nX;
            m_nDestY            = m_nY;

            SetMoveState(cmsOnStand);
        }
    }

    if (m_MoveCtrl.bMove)
    {
        int nMoveDirection = m_nFaceDirection + m_MoveCtrl.nDirection8 * DIRECTION_COUNT / 8;
        int nDestX         = m_nX + MOVE_DEST_RANGE * g_Cos(nMoveDirection) / SIN_COS_NUMBER;
        int nDestY         = m_nY + MOVE_DEST_RANGE * g_Sin(nMoveDirection) / SIN_COS_NUMBER;

        if (
            m_eMoveState == cmsOnStand || m_eMoveState == cmsOnWalk || 
            m_eMoveState == cmsOnRun || m_eMoveState == cmsOnJump || m_eMoveState == cmsOnSit
        )
        {
            if (m_MoveCtrl.bWalk)
            {
                WalkTo(nDestX, nDestY, false);
            }
            else
            {
                RunTo(nDestX, nDestY, false);
            }
        }
        else if (m_eMoveState == cmsOnSwim || m_eMoveState == cmsOnFloat)
        {
            SwimTo(nDestX, nDestY, false);
        }
    }
    else
    {
        if (m_eMoveState == cmsOnWalk || m_eMoveState == cmsOnRun || m_eMoveState == cmsOnSwim)
        {
            m_nConvergenceSpeed = 0;
            m_nDestX            = m_nX;
            m_nDestY            = m_nY;

            if (m_eMoveState == cmsOnSwim)
            {
                m_nVelocityXY   = 0; 
                m_nVelocityZ    = 0;

                SetMoveState(cmsOnFloat);
            }
            else
            {
                int nGradientDirection = 0;

                if (m_pCell->m_BaseInfo.dwCellType == ctWater)
                {
                    KCell* pCell = m_pRegion->GetPreCell(m_nXCell, m_nYCell, m_pCell);
                    KGLOG_PROCESS_ERROR(pCell);

                    nGradientDirection = pCell->GetGradientDirection();
                }
                else 
                {
                    nGradientDirection = m_pCell->GetGradientDirection();
                }

                if ((!m_bSlip) || g_Cos(nGradientDirection - m_nDirectionXY) < 0)
                {
                    m_nVelocityXY = 0;
                }

                SetMoveState(cmsOnStand);
            }
        }
    }

Exit0:
    return;
}

DWORD KPlayer::GetCurrentTeamID()
{
    if (m_dwSystemTeamID != ERROR_ID)
        return m_dwSystemTeamID;

    return m_dwTeamID;
}

int KPlayer::GetCurrentTeamGroupIndex()
{
    int     nResult     = INVALID_TEAM_GROUP;
    DWORD   dwTeamID    = ERROR_ID;
#ifdef _SERVER
    KTeam*  pTeam       = NULL;
#endif
    dwTeamID = GetCurrentTeamID();
    KG_PROCESS_ERROR(dwTeamID != ERROR_ID);

#ifdef _SERVER
    pTeam = g_pSO3World->m_TeamServer.GetTeam(dwTeamID);
    KGLOG_PROCESS_ERROR(pTeam);

    for (int i = 0; i < pTeam->nGroupNum; i++)
    {
        for (
            KTEAM_MEMBER_LIST::iterator it = pTeam->MemberGroup[i].MemberList.begin();
            it != pTeam->MemberGroup[i].MemberList.end(); ++it
        )
        {
            if (it->dwMemberID == m_dwID)
            {
                nResult = i;
                goto Exit0;
            }
        }
    }
#else
    for (int i = 0; i < g_pSO3World->m_TeamClient.m_nGroupNum; i++)
    {
        for (
            KTEAM_MEMBER_LIST::iterator it = g_pSO3World->m_TeamClient.m_TeamGroup[i].MemberList.begin();
            it != g_pSO3World->m_TeamClient.m_TeamGroup[i].MemberList.end(); ++it
        )
        {
            if (it->dwMemberID == m_dwID)
            {
                nResult = i;
                goto Exit0;
            }
        }
    }
#endif

Exit0:
    return nResult;
}

#ifdef _SERVER

#define MAX_VIEW_OBJECT_COUNT   2

void KPlayer::SyncObjectView()
{
    int nCount = 0;     // 对每个循环同步的对象个数做限制

    assert(m_pScene);
    assert(m_pRegion);

    while (nCount++ < MAX_VIEW_OBJECT_COUNT && !m_PlayerSyncQueue.empty())
    {
        DWORD       dwID    = m_PlayerSyncQueue.front().dwObjectID;
        int         nFrame  = m_PlayerSyncQueue.front().nViewFrame;
        KPlayer*    pPlayer = NULL;

        if (g_pSO3World->m_nGameLoop < nFrame)
            break;

        m_PlayerSyncTable.erase(dwID);
        m_PlayerSyncQueue.pop_front();

        pPlayer = g_pSO3World->m_PlayerSet.GetObj(dwID);

        if (pPlayer == NULL || pPlayer->m_pScene != m_pScene)
            continue;

        assert(pPlayer->m_pRegion);

        if (abs(pPlayer->m_pRegion->m_nRegionX - m_pRegion->m_nRegionX) > m_pScene->m_nBroadcastRegion)
            continue;

        if (abs(pPlayer->m_pRegion->m_nRegionY - m_pRegion->m_nRegionY) > m_pScene->m_nBroadcastRegion)
            continue;

        g_PlayerServer.DoSyncNewPlayer(m_nConnIndex, pPlayer);
        g_PlayerServer.DoSyncNewPlayer(pPlayer->m_nConnIndex, this);
    }

    nCount = 0;

    while (nCount++ < MAX_VIEW_OBJECT_COUNT && !m_NpcSyncQueue.empty())
    {
        DWORD       dwID    = m_NpcSyncQueue.front().dwObjectID;
        int         nFrame  = m_NpcSyncQueue.front().nViewFrame;
        KNpc*       pNpc    = NULL;

        if (g_pSO3World->m_nGameLoop < nFrame)
            break;

        m_NpcSyncTable.erase(dwID);
        m_NpcSyncQueue.pop_front();

        pNpc = g_pSO3World->m_NpcSet.GetObj(dwID);

        if (pNpc == NULL || pNpc->m_pScene != m_pScene)
            continue;

        if (pNpc->m_dwOwner != ERROR_ID && pNpc->m_dwOwner != m_dwID)
            continue;

        assert(pNpc->m_pRegion);

        if (abs(pNpc->m_pRegion->m_nRegionX - m_pRegion->m_nRegionX) > m_pScene->m_nBroadcastRegion)
            continue;

        if (abs(pNpc->m_pRegion->m_nRegionY - m_pRegion->m_nRegionY) > m_pScene->m_nBroadcastRegion)
            continue;

        g_PlayerServer.DoSyncNewNpc(m_nConnIndex, pNpc);
    }

    nCount = 0;

    while (nCount++ < MAX_VIEW_OBJECT_COUNT && !m_DoodadSyncQueue.empty())
    {
        DWORD       dwID    = m_DoodadSyncQueue.front().dwObjectID;
        int         nFrame  = m_DoodadSyncQueue.front().nViewFrame;
        KDoodad*    pDoodad = NULL;

        if (g_pSO3World->m_nGameLoop < nFrame)
            break;

        m_DoodadSyncTable.erase(dwID);
        m_DoodadSyncQueue.pop_front();

        pDoodad = g_pSO3World->m_DoodadSet.GetObj(dwID);

        if (pDoodad == NULL || pDoodad->m_pScene != m_pScene)
            continue;

        assert(pDoodad->m_pRegion);

        if (abs(pDoodad->m_pRegion->m_nRegionX - m_pRegion->m_nRegionX) > m_pScene->m_nBroadcastRegion)
            continue;

        if (abs(pDoodad->m_pRegion->m_nRegionY - m_pRegion->m_nRegionY) > m_pScene->m_nBroadcastRegion)
            continue;

        g_PlayerServer.DoSyncNewDoodad(m_nConnIndex, pDoodad);
    }
}

struct KPlayerViewFunc
{
    KPlayerViewFunc(KPlayer* pPlayer) : m_pSelf(pPlayer) {};

    BOOL operator () (KPlayer* pPlayer)
    {
        if (pPlayer == m_pSelf)
            return true;

        // 如果对方已经把我注册了,我就不用再注册他了(因为同步时双向的)
        KPlayer::KVIEW_SYNC_TABLE::iterator it = pPlayer->m_PlayerSyncTable.find(m_pSelf->m_dwID);
        if (it != pPlayer->m_PlayerSyncTable.end())
            return true;

        std::pair<KPlayer::KVIEW_SYNC_TABLE::iterator, BOOL> InsRet = 
            m_pSelf->m_PlayerSyncTable.insert(pPlayer->m_dwID);
        if (InsRet.second)
        {
            m_pSelf->m_PlayerSyncQueue.push_back(KPlayer::KViewObject(pPlayer->m_dwID, g_pSO3World->m_nGameLoop + VIEW_SYNC_DELAY));
        }

        return true;
    }

    KPlayer*    m_pSelf;
};


struct KNpcViewFunc
{
    KNpcViewFunc(KPlayer* pPlayer) : m_pSelf(pPlayer) {};

    BOOL operator () (KNpc* pNpc)
    {
        if (pNpc->m_dwOwner != ERROR_ID && pNpc->m_dwOwner != m_pSelf->m_dwID)
            return true;

        std::pair<KPlayer::KVIEW_SYNC_TABLE::iterator, BOOL> InsRet = 
            m_pSelf->m_NpcSyncTable.insert(pNpc->m_dwID);
        if (InsRet.second)
        {
            m_pSelf->m_NpcSyncQueue.push_back(KPlayer::KViewObject(pNpc->m_dwID, g_pSO3World->m_nGameLoop + VIEW_SYNC_DELAY));
        }

        return true;
    }

    KPlayer*    m_pSelf;
};

struct KDoodadViewFunc
{
    KDoodadViewFunc(KPlayer* pPlayer) : m_pSelf(pPlayer) {};

    BOOL operator () (KDoodad* pDoodad)
    {
        std::pair<KPlayer::KVIEW_SYNC_TABLE::iterator, BOOL> InsRet = 
            m_pSelf->m_DoodadSyncTable.insert(pDoodad->m_dwID);
        if (InsRet.second)
        {
            m_pSelf->m_DoodadSyncQueue.push_back(KPlayer::KViewObject(pDoodad->m_dwID, g_pSO3World->m_nGameLoop));
        }

        return true;
    }

    KPlayer*    m_pSelf;
};

void KPlayer::RegisterViewObject(KRegion* pFromRegion, KRegion* pToRegion)
{
    int             nLeft               = 0;
    int             nRight              = 0;
    int             nBottom             = 0;
    int             nTop                = 0;
    KPlayerViewFunc PlayerViewFunc(this);
    KNpcViewFunc    NpcViewFunc(this);
    KDoodadViewFunc DoodadViewFunc(this);

    assert(m_pScene);
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

    if (pFromRegion)
    {
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
                // 将新Region内的对象都登记到我的同步表中
                pRegion->TraversePlayer(PlayerViewFunc);
                pRegion->TraverseNpc(NpcViewFunc);
                pRegion->TraverseDoodad(DoodadViewFunc);
            }
        }
    }
}
#endif

#ifdef _CLIENT

void KPlayer::SetDisplayData(S2C_PLAYER_DISPLAY_DATA& rPlayerDisplayData)
{
    KLevelUpData* pLevelUpData = NULL;

	m_eRoleType = (ROLE_TYPE)rPlayerDisplayData.byRoleType;
    m_nLevel    = rPlayerDisplayData.byLevel;

    SetGenderByRoleType(rPlayerDisplayData.byRoleType);

    pLevelUpData = g_pSO3World->m_Settings.m_LevelUpList.GetLevelUpData(m_eRoleType, m_nLevel);   
    KGLOG_PROCESS_ERROR(pLevelUpData);

    m_nHeight   = pLevelUpData->nHeight;
    m_eCamp     = (KCAMP)rPlayerDisplayData.byCamp;
    m_dwTongID  = rPlayerDisplayData.dwTongID;

    g_pSO3World->m_TongClient.TryGetTongName(m_dwTongID, m_szTongName, sizeof(m_szTongName));

	SetForceID((DWORD)rPlayerDisplayData.byForceID);

	strncpy(m_szName, rPlayerDisplayData.szRoleName, _NAME_LEN);
	m_szName[_NAME_LEN - 1] = 0;

    m_dwModelID                             = rPlayerDisplayData.wModelID;
    m_nBattleFieldSide                      = (int)rPlayerDisplayData.byBattleFieldSide;

    m_wRepresentId[perFaceStyle]            = rPlayerDisplayData.uFaceStyle;
    m_wRepresentId[perHairStyle]            = rPlayerDisplayData.uHairStyle;
    m_wRepresentId[perHelmStyle]            = rPlayerDisplayData.uHelmStyle;
    m_wRepresentId[perHelmColor]            = rPlayerDisplayData.uHelmColor;
    m_wRepresentId[perHelmEnchant]          = rPlayerDisplayData.uHelmEnchant;
    m_wRepresentId[perChestStyle]           = rPlayerDisplayData.uChestStyle;
    m_wRepresentId[perChestColor]           = rPlayerDisplayData.uChestColor;
    m_wRepresentId[perChestEnchant]         = rPlayerDisplayData.uChestEnchant;
    m_wRepresentId[perWaistStyle]           = rPlayerDisplayData.uWaistStyle;
    m_wRepresentId[perWaistColor]           = rPlayerDisplayData.uWaistColor;
    m_wRepresentId[perWaistEnchant]         = rPlayerDisplayData.uWaistEnchant;
    
    m_wRepresentId[perBangleStyle]          = rPlayerDisplayData.uBangleStyle;
    m_wRepresentId[perBangleColor]          = rPlayerDisplayData.uBangleColor;
    m_wRepresentId[perBangleEnchant]        = rPlayerDisplayData.uBangleEnchant;
    m_wRepresentId[perBootsStyle]           = rPlayerDisplayData.uBootsStyle;
    m_wRepresentId[perBootsColor]           = rPlayerDisplayData.uBootsColor;
    m_wRepresentId[perWeaponStyle]          = rPlayerDisplayData.uWeaponStyle;
    m_wRepresentId[perWeaponEnchant1]       = rPlayerDisplayData.uWeaponEnchant1;
    m_wRepresentId[perWeaponEnchant2]       = rPlayerDisplayData.uWeaponEnchant2;
    m_wRepresentId[perBackExtend]           = rPlayerDisplayData.uBackExtend;
    m_wRepresentId[perWaistExtend]          = rPlayerDisplayData.uWaistExtend;

    m_wRepresentId[perHorseStyle]           = rPlayerDisplayData.uHorseStyle;      
    m_wRepresentId[perHorseAdornment1]      = rPlayerDisplayData.uHorseAdornment1;
    m_wRepresentId[perHorseAdornment2]      = rPlayerDisplayData.uHorseAdornment2; 
    m_wRepresentId[perHorseAdornment3]      = rPlayerDisplayData.uHorseAdornment3; 
    m_wRepresentId[perHorseAdornment4]      = rPlayerDisplayData.uHorseAdornment4;
    m_dwSchoolID                            = (DWORD)rPlayerDisplayData.uSchoolID;

	if (g_pGameWorldRepresentHandler)
        g_pGameWorldRepresentHandler->OnCharacterDisplayData(this);

    if (g_pGameWorldUIHandler)
	{
		KUIEventPlayerDisplayDataUpdate Param = { m_dwID };
		g_pGameWorldUIHandler->OnPlayerDisplayDataUpdate(Param);
	}

Exit0:
    return;
}
#endif

void KPlayer::SetGenderByRoleType(int nRoleType)
{
    m_eGender = cgGay;

    if (nRoleType == rtStandardMale || nRoleType == rtStrongMale || nRoleType == rtLittleBoy)
    {
        m_eGender = cgMale;
    }
    else if (nRoleType == rtStandardFemale || nRoleType == rtSexyFemale || nRoleType == rtLittleGirl)
    {
        m_eGender = cgFemale;
    }
}

void KPlayer::AddExp(int nExpIncrement)
{
	BOOL            bRetCode         = false;
    KLevelUpData*   pLevelUpData     = NULL;

    KG_PROCESS_ERROR(nExpIncrement > 0);
    
	// 如果达到最高等级，则不允许加经验
	KG_PROCESS_ERROR(m_nLevel < m_nMaxLevel);

	m_nExperience += nExpIncrement;

	pLevelUpData = g_pSO3World->m_Settings.m_LevelUpList.GetLevelUpData(m_eRoleType, m_nLevel);
	KGLOG_PROCESS_ERROR(pLevelUpData);

	while (m_nExperience >= pLevelUpData->nExperience && m_nLevel < m_nMaxLevel)
	{
		m_nExperience -= pLevelUpData->nExperience;

		SetLevel(m_nLevel + 1);

#ifdef _SERVER
        g_LogClient.LogPlayerLevelup(m_nLevel, m_szAccount, m_szName);
#endif

        m_nCurrentLife = m_nMaxLife;
        m_nCurrentMana = m_nMaxMana;

		pLevelUpData = g_pSO3World->m_Settings.m_LevelUpList.GetLevelUpData(m_eRoleType, m_nLevel);
		KGLOG_PROCESS_ERROR(pLevelUpData);

#ifdef _SERVER
        m_SkillList.OnPlayerLevelUp();
        g_PlayerServer.DoPlayerLevelUp(this);
#endif
	}

#ifdef _SERVER
	g_PlayerServer.DoSyncExperience(this, nExpIncrement);
#endif

Exit0:
	return;
}

#ifdef _SERVER
BOOL KPlayer::AddContribution(int nAddContribution)
{
    BOOL    bResult             = false;
    int     nPreContribution    = m_nContribution;
    
    if (nAddContribution >= 0)
    {
        KGLOG_PROCESS_ERROR(m_dwTongID != ERROR_ID);
        if (m_nContribution > CONTRIBUTION_LIMIT -  nAddContribution)
        {
            m_nContribution = CONTRIBUTION_LIMIT;
        }
        else
        {
            m_nContribution += nAddContribution;
        }
    }
    else
    {
        KGLOG_PROCESS_ERROR(m_nContribution >= -nAddContribution);
        m_nContribution += nAddContribution;
    }

    if (nPreContribution != m_nContribution)
    {
        g_PlayerServer.DoSyncContribution(m_nConnIndex, m_nContribution);
    }

    bResult = true;
Exit0:
    return bResult;
}
#endif

void KPlayer::SetLevel(int nLevel)
{    
    KLevelUpData* pLevelUpData = NULL;

    KGLOG_PROCESS_ERROR(nLevel > 0);
    
    if (m_nLevel > 0)
    {
	    pLevelUpData = g_pSO3World->m_Settings.m_LevelUpList.GetLevelUpData(m_eRoleType, m_nLevel);   
        KGLOG_PROCESS_ERROR(pLevelUpData);

        CallAttributeFunction(atStrengthBase, false, pLevelUpData->nStrength, 0);
        CallAttributeFunction(atAgilityBase, false, pLevelUpData->nAgility, 0);
        CallAttributeFunction(atVitalityBase, false, pLevelUpData->nVigor, 0);
        CallAttributeFunction(atSpiritBase, false, pLevelUpData->nSpirit, 0);
        CallAttributeFunction(atSpunkBase, false, pLevelUpData->nSpunk, 0);

        CallAttributeFunction(atMaxLifeBase, false, pLevelUpData->nMaxLife, 0);
        CallAttributeFunction(atMaxManaBase, false, pLevelUpData->nMaxMana, 0);

        CallAttributeFunction(atRunSpeedBase, false, pLevelUpData->nRunSpeed, 0);
        CallAttributeFunction(atJumpSpeedBase, false, pLevelUpData->nJumpSpeed, 0);

        // ------------ 属性回复 ----------------------------
        CallAttributeFunction(atLifeReplenish, false, pLevelUpData->nLifeReplenish, 0);
        CallAttributeFunction(atLifeReplenishPercent, false, pLevelUpData->nLifeReplenishPercent, 0);
        CallAttributeFunction(atLifeReplenishExt, false, pLevelUpData->nLifeReplenishExt, 0);
        CallAttributeFunction(atManaReplenish, false, pLevelUpData->nManaReplenish, 0);
        CallAttributeFunction(atManaReplenishPercent, false, pLevelUpData->nManaReplenishPercent, 0);
        CallAttributeFunction(atManaReplenishExt, false, pLevelUpData->nManaReplenishExt, 0);

        // ------------- 技能命中值 -----------------------------
        CallAttributeFunction(atPhysicsHitBaseRate, false, pLevelUpData->nHitBase, 0);
        CallAttributeFunction(atSolarHitBaseRate, false, pLevelUpData->nHitBase, 0);
        CallAttributeFunction(atNeutralHitBaseRate, false, pLevelUpData->nHitBase, 0);
        CallAttributeFunction(atLunarHitBaseRate, false, pLevelUpData->nHitBase, 0);
        CallAttributeFunction(atPoisonHitBaseRate, false, pLevelUpData->nHitBase, 0);

        // ------------- 技能暴击值 -----------------------------
        CallAttributeFunction(atPhysicsCriticalStrike, false, pLevelUpData->nPhysicsCriticalStrike, 0);
        CallAttributeFunction(atSolarCriticalStrike, false, pLevelUpData->nSolarCriticalStrike, 0);
        CallAttributeFunction(atNeutralCriticalStrike, false, pLevelUpData->nNeutralCriticalStrike, 0);
        CallAttributeFunction(atLunarCriticalStrike, false, pLevelUpData->nLunarCriticalStrike, 0);
        CallAttributeFunction(atPoisonCriticalStrike, false, pLevelUpData->nPoisonCriticalStrike, 0);
        
        CallAttributeFunction(atNoneWeaponAttackSpeedBase, false, pLevelUpData->nNoneWeaponAttackSpeedBase, 0);
        
        CallAttributeFunction(atPhysicsDefenceMax, false, pLevelUpData->nMaxPhysicsDefence, 0);
    }
	pLevelUpData = g_pSO3World->m_Settings.m_LevelUpList.GetLevelUpData(m_eRoleType, nLevel);
	KGLOG_PROCESS_ERROR(pLevelUpData);

    CallAttributeFunction(atStrengthBase, true, pLevelUpData->nStrength, 0);
    CallAttributeFunction(atAgilityBase, true, pLevelUpData->nAgility, 0);
    CallAttributeFunction(atVitalityBase, true, pLevelUpData->nVigor, 0);
    CallAttributeFunction(atSpiritBase, true, pLevelUpData->nSpirit, 0);
    CallAttributeFunction(atSpunkBase, true, pLevelUpData->nSpunk, 0);

    CallAttributeFunction(atMaxLifeBase, true, pLevelUpData->nMaxLife, 0);
    CallAttributeFunction(atMaxManaBase, true, pLevelUpData->nMaxMana, 0);

    CallAttributeFunction(atRunSpeedBase, true, pLevelUpData->nRunSpeed, 0);
    CallAttributeFunction(atJumpSpeedBase, true, pLevelUpData->nJumpSpeed, 0);

    // ------------ 属性回复 ----------------------------
    CallAttributeFunction(atLifeReplenish, true, pLevelUpData->nLifeReplenish, 0);
    CallAttributeFunction(atLifeReplenishPercent, true, pLevelUpData->nLifeReplenishPercent, 0);
    CallAttributeFunction(atLifeReplenishExt, true, pLevelUpData->nLifeReplenishExt, 0);
    CallAttributeFunction(atManaReplenish, true, pLevelUpData->nManaReplenish, 0);
    CallAttributeFunction(atManaReplenishPercent, true, pLevelUpData->nManaReplenishPercent, 0);
    CallAttributeFunction(atManaReplenishExt, true, pLevelUpData->nManaReplenishExt, 0);

    // ------------- 技能命中值 -------------------------
    CallAttributeFunction(atPhysicsHitBaseRate, true, pLevelUpData->nHitBase, 0);
    CallAttributeFunction(atSolarHitBaseRate, true, pLevelUpData->nHitBase, 0);
    CallAttributeFunction(atNeutralHitBaseRate, true, pLevelUpData->nHitBase, 0);
    CallAttributeFunction(atLunarHitBaseRate, true, pLevelUpData->nHitBase, 0);
    CallAttributeFunction(atPoisonHitBaseRate, true, pLevelUpData->nHitBase, 0);

    // --------------------------------------------------
    CallAttributeFunction(atPhysicsCriticalStrike, true, pLevelUpData->nPhysicsCriticalStrike, 0);
    CallAttributeFunction(atSolarCriticalStrike, true, pLevelUpData->nSolarCriticalStrike, 0);
    CallAttributeFunction(atNeutralCriticalStrike, true, pLevelUpData->nNeutralCriticalStrike, 0);
    CallAttributeFunction(atLunarCriticalStrike, true, pLevelUpData->nLunarCriticalStrike, 0);
    CallAttributeFunction(atPoisonCriticalStrike, true, pLevelUpData->nPoisonCriticalStrike, 0);
    
    CallAttributeFunction(atNoneWeaponAttackSpeedBase, true, pLevelUpData->nNoneWeaponAttackSpeedBase, 0);

    CallAttributeFunction(atPhysicsDefenceMax, true, pLevelUpData->nMaxPhysicsDefence, 0);

	// -----------------------------------------------------------
    m_nHeight = pLevelUpData->nHeight;
	m_nLevel  = nLevel;
    
    m_nMaxStamina = pLevelUpData->nMaxStamina;
    m_nMaxThew    = pLevelUpData->nMaxThew;

#ifdef _SERVER
    g_RelayClient.DoChangeRoleLevelRequest(this);
#endif // _SERVER

Exit0:
	return;
}

#ifdef _SERVER
BOOL KPlayer::LoadSkillRecipeList(BYTE* pbyData, size_t uDataLen)
{
    BOOL    bResult     = false;
    BOOL    bRetCode    = false;
    size_t  uLeftSize   = uDataLen;
    size_t  uUsedSize   = 0;
    BYTE*   pbyOffset   = pbyData;

    bRetCode = m_SkillRecipeList.Load(&uUsedSize, pbyOffset, uLeftSize);
    KGLOG_PROCESS_ERROR(bRetCode);
    uLeftSize -= uUsedSize;
    pbyOffset += uUsedSize;

    KGLOG_PROCESS_ERROR(uLeftSize >= 0);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayer::LoadQuestData(BYTE* pbyData, size_t uDataLen)
{
    BOOL    bResult     = false;
    BOOL    bRetCode    = false;
    size_t  uLeftSize   = uDataLen;
    size_t  uUsedSize   = 0;
    BYTE*   pbyOffset   = pbyData;

    bRetCode = m_QuestList.LoadQuestState(&uUsedSize, pbyOffset, uLeftSize);
	KGLOG_PROCESS_ERROR(bRetCode);

    g_PlayerServer.DoSyncQuestData(m_nConnIndex, m_dwID, pbyOffset, uUsedSize, (BYTE)qdtQuestState);

    uLeftSize -= uUsedSize;
    pbyOffset += uUsedSize;

	bRetCode = m_QuestList.LoadQuestList(&uUsedSize, pbyOffset, uLeftSize);
	KGLOG_PROCESS_ERROR(bRetCode);

    g_PlayerServer.DoSyncQuestData(m_nConnIndex, m_dwID, pbyOffset, uUsedSize, (BYTE)qdtQuestList);

    uLeftSize -= uUsedSize;
    pbyOffset += uUsedSize;

    bRetCode = m_QuestList.LoadAssistQuestList(&uUsedSize, pbyOffset, uLeftSize);
	KGLOG_PROCESS_ERROR(bRetCode);

    uLeftSize -= uUsedSize;
    pbyOffset += uUsedSize;
    
    bRetCode = m_QuestList.LoadDailyQuest(&uUsedSize, pbyOffset, uLeftSize);
    KGLOG_PROCESS_ERROR(bRetCode);

    g_PlayerServer.DoSyncQuestData(m_nConnIndex, m_dwID, pbyOffset, uUsedSize, (BYTE)qdtDailyQuest);

    uLeftSize -= uUsedSize;
    pbyOffset += uUsedSize;

    KGLOG_PROCESS_ERROR(uLeftSize == 0);

    bResult = true;
Exit0:
    return bResult; 
}

BOOL KPlayer::LoadStateInfo(BYTE* pbyData, size_t uDataLen)
{
    BOOL                bResult         = false;
    KROLE_STATE_INFO*   pRoleStateInfo  = NULL;
    size_t              uLeftSize       = uDataLen;
    BYTE*               pbyOffset       = pbyData;

    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(KROLE_STATE_INFO));
    pRoleStateInfo = (KROLE_STATE_INFO*)pbyOffset;
    uLeftSize -= sizeof(KROLE_STATE_INFO);
    pbyOffset += sizeof(KROLE_STATE_INFO);

    m_eMoveState                        = (CHARACTER_MOVE_STATE)pRoleStateInfo->byMoveState;
    m_nCurrentTrack                     = pRoleStateInfo->wCurrentTrack;
    m_nMoveFrameCounter                 = pRoleStateInfo->nMoveFrameCounter;
    m_nFromNode                         = pRoleStateInfo->wFromFlyNode;
    m_nTargetCity                       = pRoleStateInfo->wTargetCityID;
    m_nExperience                       = pRoleStateInfo->nExperience;

    m_nCurrentLife                      = pRoleStateInfo->nCurrentLife;
    m_nCurrentMana                      = pRoleStateInfo->nCurrentMana;
    m_nCurrentStamina                   = pRoleStateInfo->nCurrentStamina;
    m_nCurrentThew                      = pRoleStateInfo->nCurrentThew;

    m_nCurrentTrainValue                = pRoleStateInfo->nCurrentTrainValue;
    m_nUsedTrainValue                   = pRoleStateInfo->nUsedTrainValue;
    m_nAddTrainTimeInToday              = pRoleStateInfo->nAddTrainTimeInToday;

    m_ReviveParam.nSituReviveFrame      = g_pSO3World->m_nGameLoop + pRoleStateInfo->wLeftReviveFrame;
    m_ReviveParam.nLastSituReviveTime	= pRoleStateInfo->nLastSituReviveTime;
    m_ReviveParam.nSituReviveCount	    = pRoleStateInfo->bySituReviveCount;

    if (g_pSO3World->m_Settings.m_ConstList.bUsePlayerReviveCtrl)
    {
        int nLeftFrame = pRoleStateInfo->wLeftReviveFrame - (int)(g_pSO3World->m_nCurrentTime - m_nLastSaveTime) * GAME_FPS;

        m_ReviveCtrl.nReviveFrame           = max(nLeftFrame, 0);
        m_ReviveCtrl.nLastSituReviveTime    = pRoleStateInfo->nLastSituReviveTime;
        m_ReviveCtrl.nSituReviveCount       = pRoleStateInfo->bySituReviveCount;
        m_ReviveCtrl.nNextCheckReviveFrame  = 0;

        if (m_eMoveState == cmsOnDeath)
            m_ReviveCtrl.nNextCheckReviveFrame = g_pSO3World->m_nGameLoop + 1;
    }

    m_dwKillerID						= pRoleStateInfo->dwKillerID;  

    switch (m_eMoveState)
    {
    case cmsOnAutoFly:
    case cmsOnFloat:
        break;

    case cmsOnDeath:
        m_ReviveParam.eState = crsWaitingForRevive;
        break;

    case cmsOnSwim:
        m_eMoveState = cmsOnFloat;
        break;

    default:
        m_eMoveState = cmsOnStand;
        m_nCurrentTrack = 0;
        break;
    }

    if (pRoleStateInfo->byPKState == pksSlaying)
    {
        m_PK.SetState(pksSlaying);
        g_PlayerServer.DoSyncPKState(m_nConnIndex, this);
    }
    else if (pRoleStateInfo->byPKState == pksExitSlay)
    {
        int nEndFrame = 0;
        int nCloseSlayLeftTime = pRoleStateInfo->wCloseSlayLeftTime;
        MAKE_IN_RANGE(nCloseSlayLeftTime, 0, g_pSO3World->m_Settings.m_ConstList.nCloseSlayTime);
        
        nEndFrame = g_pSO3World->m_nGameLoop + nCloseSlayLeftTime * GAME_FPS;

        m_PK.SetState(pksExitSlay, nEndFrame);
        g_PlayerServer.DoSyncPKState(m_nConnIndex, this, nEndFrame);
    }
    
    m_nCurrentKillPoint         = pRoleStateInfo->wCurrentKillPoint;
    m_nLastKillPointReduceTime  = pRoleStateInfo->nLastKillPointReduceTime;
    m_nCurrentPrestige          = pRoleStateInfo->nCurrentPrestige;
    m_nBanTime                  = pRoleStateInfo->nBanTime > g_pSO3World->m_nCurrentTime ? pRoleStateInfo->nBanTime : 0;
    m_nContribution             = pRoleStateInfo->nContribution;
    m_nMaxLevel                 = pRoleStateInfo->nMaxLevel;
    m_bHideHat                  = pRoleStateInfo->bHideHat;

    KGLOG_PROCESS_ERROR(uLeftSize == 0);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayer::LoadRoadOpenNode(BYTE* pbyData, size_t uDataLen)
{
    BOOL                bResult     = false;
    BOOL                bRetCode    = false;
    KROLE_ROAD_INFO*    pLoadRoad   = NULL;
    BYTE*               pbyOffset   = pbyData;
    size_t              uLeftSize   = uDataLen;

    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(KROLE_ROAD_INFO));
    pLoadRoad = (KROLE_ROAD_INFO*)pbyOffset;
    pbyOffset += sizeof(KROLE_ROAD_INFO);
    uLeftSize -= sizeof(KROLE_ROAD_INFO);

    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(int) * pLoadRoad->nOpenNodeCount);
    pbyOffset += sizeof(int) * pLoadRoad->nOpenNodeCount;
    uLeftSize -= sizeof(int) * pLoadRoad->nOpenNodeCount;

    for (int i = 0; i < pLoadRoad->nOpenNodeCount; i++)
    {
        m_OpenRouteNodeList.push_back(pLoadRoad->OpenNodeList[i]);
    }

    KGLOG_PROCESS_ERROR(uLeftSize == 0);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayer::LoadHeroData(BYTE* pbyData, size_t uDataLen)
{
    BOOL                bResult     = false;
    BOOL                bRetCode    = false;
    KHERO_DATA*         pHeroData   = NULL;

    KGLOG_PROCESS_ERROR(uDataLen >= sizeof(KHERO_DATA));
    pHeroData = (KHERO_DATA*)pbyData;
    
    KGLOG_PROCESS_ERROR(pHeroData->byDataLen <= MAX_MAP_ID_DATA_SIZE);

    m_bHeroFlag = pHeroData->byHeroFlag;
    
    bRetCode = m_HeroMapCopyOpenFlag.Load(pHeroData->byData, pHeroData->byDataLen);
    KGLOG_PROCESS_ERROR(bRetCode);
    
    g_PlayerServer.DoSyncHeroFlag(this);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayer::LoadAchievementData(BYTE* pbyData, size_t uDataLen)
{
    BOOL                bResult                 = false;
    BOOL                bRetCode                = false;
    size_t              uLeftSize               = uDataLen;
    BYTE*               pbyOffset               = pbyData;

    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(int));
    m_nAchievementPoint = *(int*)pbyOffset;
    uLeftSize -= sizeof(int);
    pbyOffset += sizeof(int);

    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(int));
    m_nAchievementRecord = *(int*)pbyOffset;
    uLeftSize -= sizeof(int);
    pbyOffset += sizeof(int);

    bRetCode = m_AchievementSet.Load(pbyOffset, uLeftSize);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayer::Load(BYTE* pbyData, size_t uDataLen)
{
    BOOL    bResult     = false;
    BOOL    bRetCode    = false;

    if (uDataLen != 0)
    {
        bRetCode = LoadExtRoleData(pbyData, uDataLen);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

    // 同步状态
    g_PlayerServer.DoSyncPlayerStateInfo(this);
    
    g_PlayerServer.DoSyncKillPoint(this);

    g_pSO3World->m_FellowshipMgr.LoadFellowshipData(m_dwID);

    // 向客户端同步其所属队伍数据
    g_PlayerServer.DoLoginPartySync(this);

    // Buff同步:
    g_PlayerServer.DoSyncBuffList(this, this);

    // 同步场景访问记录
    g_PlayerServer.DoSyncVisitMapInfo(this);

    if (!m_OpenRouteNodeList.empty())
    {
        g_PlayerServer.DoSyncRouteNodeOpenList(
            m_nConnIndex, (int)m_OpenRouteNodeList.size(), &m_OpenRouteNodeList[0]
        );
    }
    
    g_PlayerServer.DoSyncCampInfo(this);

    g_PlayerServer.DoSyncAchievementData(this);

    m_nNextSaveFrame = g_pSO3World->m_nGameLoop 
        + g_pSO3World->m_Settings.m_ConstList.nSaveInterval * GAME_FPS;

    bResult = true;
Exit0:
    if (!bResult)
    {
        KGLogPrintf(KGLOG_ERR, "Role data error, ID(%u), Name(%s)\n", m_dwID, m_szName);
    }
    return bResult;
}

BOOL KPlayer::LoadExtRoleData(BYTE* pbyData, size_t uDataLen)
{
    BOOL                bResult         = false;
    BOOL                bRetCode        = false;
    KRoleDataHeader*    pGlobalHeader   = NULL;
    size_t              uLeftSize       = uDataLen;
    BYTE*               pbyOffset       = pbyData;
    DWORD               dwCRC           = 0;
    
    assert(pbyData);

    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(KRoleDataHeader));
    pGlobalHeader = (KRoleDataHeader*)pbyOffset;

    uLeftSize -= sizeof(KRoleDataHeader);
    pbyOffset += sizeof(KRoleDataHeader);

    KGLOG_PROCESS_ERROR(pGlobalHeader->dwVer == 0);
    KGLOG_PROCESS_ERROR(pGlobalHeader->dwLen == uLeftSize);
       
    dwCRC = CRC32(0, pbyOffset, (DWORD)uLeftSize);
    KGLOG_PROCESS_ERROR(dwCRC == pGlobalHeader->dwCRC);

    while (uLeftSize > 0)
    {
        KRoleBlockHeader* pBlock = NULL;

        KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(KRoleBlockHeader));
        pBlock = (KRoleBlockHeader*)pbyOffset;

        uLeftSize -= sizeof(KRoleBlockHeader);
        pbyOffset += sizeof(KRoleBlockHeader);

        KGLOG_PROCESS_ERROR(uLeftSize >= pBlock->dwLen);

        switch (pBlock->nType)
        {            
        case rbtSkillList:
            bRetCode = m_SkillList.Load(pbyOffset, pBlock->dwLen);
            KGLOG_PROCESS_ERROR(bRetCode);
            break;

        case rbtSkillRecipeList:
            bRetCode = LoadSkillRecipeList(pbyOffset, pBlock->dwLen);
            KGLOG_PROCESS_ERROR(bRetCode);
            break;

        case rbtItemList:
            bRetCode = m_ItemList.Load(pbyOffset, pBlock->dwLen);
		    KGLOG_PROCESS_ERROR(bRetCode);
			break;

        case rbtQuestList:
            bRetCode = LoadQuestData(pbyOffset, pBlock->dwLen);
            KGLOG_PROCESS_ERROR(bRetCode);
            break;

        case rbtProfessionList:
            bRetCode = m_ProfessionList.Load(pbyOffset, pBlock->dwLen);
            KGLOG_PROCESS_ERROR(bRetCode);
            break;

        case rbtRecipeList:
            bRetCode = m_RecipeList.Load(pbyOffset, pBlock->dwLen);
            KGLOG_PROCESS_ERROR(bRetCode);
            break;

        case rbtStateInfo:
            bRetCode = LoadStateInfo(pbyOffset, pBlock->dwLen);
            KGLOG_PROCESS_ERROR(bRetCode);
            break;

        case rbtBuffList:
            bRetCode = m_BuffList.Load(pbyOffset, pBlock->dwLen);
            KGLOG_PROCESS_ERROR(bRetCode);
            break;

        case rbtRepute:
            bRetCode = m_ReputeList.Load(pbyOffset, pBlock->dwLen);
            KGLOG_PROCESS_ERROR(bRetCode);
            break;

        case rbtUserPreferences:
            bRetCode = m_UserPreferences.Load(pbyOffset, pBlock->dwLen);
            KGLOG_PROCESS_ERROR(bRetCode);
            break;

        case rbtBookState:
            bRetCode = m_BookList.Load(pbyOffset, pBlock->dwLen);
            KGLOG_PROCESS_ERROR(bRetCode);
            break;

        case rbtCoolDownTimer:
            bRetCode = m_TimerList.Load(pbyOffset, pBlock->dwLen);
            KGLOG_PROCESS_ERROR(bRetCode);
            break;
            
        case rbtRoadOpenList:
            bRetCode = LoadRoadOpenNode(pbyOffset, pBlock->dwLen);
            KGLOG_PROCESS_ERROR(bRetCode);
            break;

        case rbtCustomData:
            bRetCode = m_CustomData.Load(pbyOffset, pBlock->dwLen);
            KGLOG_PROCESS_ERROR(bRetCode);
            break;

        case rbtVisitedMap:
            bRetCode = m_SceneVisitRecord.Load(pbyOffset, pBlock->dwLen);
            KGLOG_PROCESS_ERROR(bRetCode);
            break;

        case rbtPQList:
            bRetCode = m_PQList.Load(pbyOffset, pBlock->dwLen);
            KGLOG_PROCESS_ERROR(bRetCode);
            break;
        
        case rbtHeroData:
            bRetCode = LoadHeroData(pbyOffset, pBlock->dwLen);
            KGLOG_PROCESS_ERROR(bRetCode);
            break;

        case rbtAchievementData:
            //bRetCode = LoadAchievementData(pbyOffset, pBlock->dwLen);
            //KGLOG_PROCESS_ERROR(bRetCode);
            break;

        default:
            KGLogPrintf(
                KGLOG_ERR, "Unexpected role data block(%d, %u), Account: %s, Name: %s, ID: %u\n", 
                pBlock->nType, pBlock->dwLen, m_szAccount, m_szName, m_dwID
            );
        }

        uLeftSize -= pBlock->dwLen;
        pbyOffset += pBlock->dwLen;
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayer::CallLoginScript()
{
    BOOL    bResult         = false;
    BOOL    bRetCode        = false;
    int     nLuaTopIndex    = 0;

    bRetCode = g_pSO3World->m_ScriptCenter.IsScriptExist(PLAYER_SCRIPT);
    KGLOG_PROCESS_ERROR(bRetCode);

	g_pSO3World->m_ScriptCenter.SafeCallBegin(&nLuaTopIndex);

	g_pSO3World->m_ScriptCenter.PushValueToStack(this);

	g_pSO3World->m_ScriptCenter.CallFunction(PLAYER_SCRIPT, "OnPlayerLogin", 0);

	g_pSO3World->m_ScriptCenter.SafeCallEnd(nLuaTopIndex);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayer::RefreshDailyVariable(int nDays)
{
    BOOL    bResult     = false;
    BOOL    bRetCode    = false;

    KG_PROCESS_ERROR(nDays > 0);

    g_pSO3World->m_FellowshipMgr.AttractionFalloff(m_dwID, nDays);
    g_pSO3World->m_FellowshipMgr.RefreshDailyCount(m_dwID);
    m_QuestList.ResetAssistDailyCount();

    bResult = true;
Exit0:
    return bResult;
}

#endif	// _SERVER

#if defined(_SERVER)
BOOL KPlayer::SaveSkillRecipeList(size_t* puUsedSize, BYTE* pbyBuffer, size_t uBufferSize)
{
    BOOL    bResult     = false;
    BOOL    bRetCode    = false;
    size_t  uUsedSize   = 0;
    size_t  uLeftSize   = uBufferSize;
    BYTE*   pbyOffset   = pbyBuffer;

    bRetCode = m_SkillRecipeList.Save(&uUsedSize, pbyOffset, uLeftSize);
	KGLOG_PROCESS_ERROR(bRetCode);
    uLeftSize -= uUsedSize;
    pbyOffset += uUsedSize;

    *puUsedSize = uBufferSize - uLeftSize;

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayer::SaveQuestStateAndList(size_t* puUsedSize, BYTE* pbyBuffer, size_t uBufferSize)
{
    BOOL    bResult     = false;
    BOOL    bRetCode    = false;
    size_t  uUsedSize   = 0;
    size_t  uLeftSize   = uBufferSize;
    BYTE*   pbyOffset   = pbyBuffer;

    bRetCode = m_QuestList.SaveQuestState(&uUsedSize, pbyOffset, uLeftSize);
	KGLOG_PROCESS_ERROR(bRetCode);
    uLeftSize -= uUsedSize;
    pbyOffset += uUsedSize;

	bRetCode = m_QuestList.SaveQuestList(&uUsedSize, pbyOffset, uLeftSize);
	KGLOG_PROCESS_ERROR(bRetCode);
    uLeftSize -= uUsedSize;
    pbyOffset += uUsedSize;
    
    bRetCode = m_QuestList.SaveAssistQuestList(&uUsedSize, pbyOffset, uLeftSize);
	KGLOG_PROCESS_ERROR(bRetCode);
    uLeftSize -= uUsedSize;
    pbyOffset += uUsedSize;

    bRetCode = m_QuestList.SaveDailyQuest(&uUsedSize, pbyOffset, uLeftSize);
	KGLOG_PROCESS_ERROR(bRetCode);
    uLeftSize -= uUsedSize;
    pbyOffset += uUsedSize;

    *puUsedSize = uBufferSize - uLeftSize;

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayer::SaveStateInfo(size_t* puUsedSize, BYTE* pbyBuffer, size_t uBufferSize)
{
    BOOL                bResult         = false;
    BOOL                bRetCode        = false;
    size_t              uLeftSize       = uBufferSize;
    BYTE*               pbyOffset       = pbyBuffer;
    KROLE_STATE_INFO*   pRoleStateInfo  = NULL;

    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(KROLE_STATE_INFO));
    pRoleStateInfo = (KROLE_STATE_INFO*)pbyOffset;
    uLeftSize -= sizeof(KROLE_STATE_INFO);
    pbyOffset += sizeof(KROLE_STATE_INFO);

    assert(cmsTotal < UCHAR_MAX);

    pRoleStateInfo->byMoveState         = (BYTE)m_eMoveState;
    pRoleStateInfo->wCurrentTrack       = (WORD)m_nCurrentTrack;
    pRoleStateInfo->nMoveFrameCounter   = m_nMoveFrameCounter;
    
    KGLOG_CHECK_ERROR(m_nFromNode < USHRT_MAX);
    KGLOG_CHECK_ERROR(m_nTargetCity < USHRT_MAX);

    pRoleStateInfo->wFromFlyNode        = (WORD)m_nFromNode;
    pRoleStateInfo->wTargetCityID       = (WORD)m_nTargetCity;

    pRoleStateInfo->nExperience         = m_nExperience;

    pRoleStateInfo->nCurrentLife        = m_nCurrentLife;
    pRoleStateInfo->nCurrentMana        = m_nCurrentMana;
    pRoleStateInfo->nCurrentStamina     = m_nCurrentStamina;
    pRoleStateInfo->nCurrentThew        = m_nCurrentThew;

    pRoleStateInfo->nUsedTrainValue     = m_nUsedTrainValue;

    pRoleStateInfo->nCurrentTrainValue  = m_nCurrentTrainValue;
    pRoleStateInfo->nAddTrainTimeInToday= m_nAddTrainTimeInToday;
    
    pRoleStateInfo->byPKState           = (BYTE)m_PK.GetPKState();
    pRoleStateInfo->wCloseSlayLeftTime  = (WORD)m_PK.GetCloseSlayLeftTime();
    
    pRoleStateInfo->wLeftReviveFrame    = 0;
    if (m_ReviveParam.nSituReviveFrame > g_pSO3World->m_nGameLoop)
        pRoleStateInfo->wLeftReviveFrame = (WORD)(m_ReviveParam.nSituReviveFrame - g_pSO3World->m_nGameLoop);
    
    pRoleStateInfo->nLastSituReviveTime = m_ReviveParam.nLastSituReviveTime;
    pRoleStateInfo->bySituReviveCount	= (BYTE)m_ReviveParam.nSituReviveCount;

    if (g_pSO3World->m_Settings.m_ConstList.bUsePlayerReviveCtrl)
    {
        pRoleStateInfo->wLeftReviveFrame    = (WORD)m_ReviveCtrl.nReviveFrame;
        pRoleStateInfo->nLastSituReviveTime = m_ReviveCtrl.nLastSituReviveTime;
        pRoleStateInfo->bySituReviveCount   = (BYTE)m_ReviveCtrl.nSituReviveCount;
    }

    pRoleStateInfo->dwKillerID			= m_dwKillerID;

    pRoleStateInfo->wCurrentKillPoint   = (WORD)m_nCurrentKillPoint;
    pRoleStateInfo->nLastKillPointReduceTime = m_nLastKillPointReduceTime;
    
    pRoleStateInfo->nCurrentPrestige    = m_nCurrentPrestige;
    pRoleStateInfo->nBanTime            = m_nBanTime > g_pSO3World->m_nCurrentTime ? m_nBanTime : 0;
    pRoleStateInfo->nContribution       = m_nContribution;
    pRoleStateInfo->nMaxLevel           = m_nMaxLevel;
    pRoleStateInfo->bHideHat            = m_bHideHat;

    memset(pRoleStateInfo->nReserved, 0, sizeof(pRoleStateInfo->nReserved));


    *puUsedSize = uBufferSize - uLeftSize;

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayer::SaveRoadOpenNode(size_t* puUsedSize, BYTE* pbyBuffer, size_t uBufferSize)
{
    BOOL                            bResult     = false;
    size_t                          uLeftSize   = uBufferSize;
    BYTE*                           pbyOffset   = pbyBuffer;
    KROLE_ROAD_INFO*                pSaveRoad   = NULL;

    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(KROLE_ROAD_INFO) + (sizeof(int) * m_OpenRouteNodeList.size()));
    pSaveRoad = (KROLE_ROAD_INFO*)pbyOffset;
    pbyOffset += sizeof(KROLE_ROAD_INFO) + (sizeof(int) * m_OpenRouteNodeList.size());
    uLeftSize -= sizeof(KROLE_ROAD_INFO) + (sizeof(int) * m_OpenRouteNodeList.size());

    pSaveRoad->nOpenNodeCount = (int)m_OpenRouteNodeList.size();

    for (int i = 0; i < pSaveRoad->nOpenNodeCount; i++)
    {
        pSaveRoad->OpenNodeList[i] = m_OpenRouteNodeList[i];
    }

    *puUsedSize = uBufferSize - uLeftSize;

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayer::SaveHeroData(size_t* puUsedSize, BYTE* pbyBuffer, size_t uBufferSize)
{
    BOOL                bResult                 = false;
    BOOL                bRetCode                = false;
    size_t              uLeftSize               = uBufferSize;
    BYTE*               pbyOffset               = pbyBuffer;
    KHERO_DATA*         pHeroData               = NULL;
    size_t              uHeroDataLen            = sizeof(KHERO_DATA) + MAX_MAP_ID_DATA_SIZE;
    size_t              uHeroMapCopyOpenDataLen = 0;

    KGLOG_PROCESS_ERROR(uLeftSize >= uHeroDataLen);
    pHeroData = (KHERO_DATA*)pbyOffset;

    pbyOffset += uHeroDataLen;
    uLeftSize -= uHeroDataLen;
    
    pHeroData->byHeroFlag = (BYTE)m_bHeroFlag;
    pHeroData->byDataLen  = MAX_MAP_ID_DATA_SIZE;
    
    bRetCode = m_HeroMapCopyOpenFlag.Save(&uHeroMapCopyOpenDataLen, pHeroData->byData, MAX_MAP_ID_DATA_SIZE);
    KGLOG_PROCESS_ERROR(bRetCode);

    *puUsedSize = uBufferSize - uLeftSize;

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayer::SaveAchievementData(size_t* puUsedSize, BYTE* pbyBuffer, size_t uBufferSize)
{
    BOOL                bResult                 = false;
    BOOL                bRetCode                = false;
    size_t              uUsedSize               = 0;
    size_t              uLeftSize               = uBufferSize;
    BYTE*               pbyOffset               = pbyBuffer;

    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(int));
    *(int*)pbyOffset = m_nAchievementPoint;
    uLeftSize -= sizeof(int);
    pbyOffset += sizeof(int);

    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(int));
    *(int*)pbyOffset = m_nAchievementRecord;
    uLeftSize -= sizeof(int);
    pbyOffset += sizeof(int);

    bRetCode = m_AchievementSet.Save(&uUsedSize, pbyOffset, uLeftSize);
    KGLOG_PROCESS_ERROR(bRetCode);
    uLeftSize -= uUsedSize;
    pbyOffset += uUsedSize;

    *puUsedSize = sizeof(int) + sizeof(int) + uUsedSize;

    bResult = true;
Exit0:
    return bResult;
}

#define SAVE_ROLE_BLOCK(Func, BlockType, Version)   \
    do                                                                                  \
    {                                                                                   \
        KRoleBlockHeader* pBlock    = NULL;                                             \
        size_t            uDataLen  = 0;                                                \
                                                                                        \
        KGLOG_PROCESS_ERROR(pbyTail - pbyOffset >= sizeof(KRoleBlockHeader));           \
        pBlock = (KRoleBlockHeader*)pbyOffset;                                          \
        pbyOffset += sizeof(KRoleBlockHeader);                                          \
                                                                                        \
        bRetCode = Func(&uDataLen, pbyOffset, pbyTail - pbyOffset);                     \
        KGLOG_PROCESS_ERROR(bRetCode);                                                  \
        pBlock->nType = BlockType;                                                      \
        pBlock->dwVer = Version;                                                        \
        pBlock->dwLen = (DWORD)uDataLen;                                                \
        pbyOffset    += uDataLen;                                                       \
    } while (false)

BOOL KPlayer::Save(size_t* puUsedSize, BYTE* pbyBuffer, size_t uBufferSize)
{
    BOOL                bResult	        = false;
    BOOL                bRetCode        = false;
    BYTE*               pbyOffset       = pbyBuffer;
    BYTE*               pbyTail         = pbyBuffer + uBufferSize;
    KRoleDataHeader*    pGlobalHeader   = NULL;
    DWORD               dwExtDataLen    = 0;
    time_t              nOldDay         = (m_nLastSaveTime - timezone) / (24 * 3600);
    time_t              nNewDay         = (g_pSO3World->m_nCurrentTime - timezone) / (24 * 3600);

    if (nNewDay != nOldDay)
    {
        bRetCode = RefreshDailyVariable((int)(nNewDay - nOldDay));
        KGLOG_CHECK_ERROR(bRetCode);
    }

    KGLOG_PROCESS_ERROR(pbyTail - pbyOffset >= sizeof(KRoleDataHeader));
    pGlobalHeader = (KRoleDataHeader*)pbyOffset;

    pbyOffset += sizeof(KRoleDataHeader);

    SAVE_ROLE_BLOCK(SaveStateInfo, rbtStateInfo, 0);
    SAVE_ROLE_BLOCK(m_ItemList.Save, rbtItemList, 0);
    SAVE_ROLE_BLOCK(m_ProfessionList.Save, rbtProfessionList, 0);
    SAVE_ROLE_BLOCK(m_RecipeList.Save, rbtRecipeList, 0);
    SAVE_ROLE_BLOCK(m_SkillList.Save, rbtSkillList, 0);
    SAVE_ROLE_BLOCK(SaveSkillRecipeList, rbtSkillRecipeList, 0);
    SAVE_ROLE_BLOCK(SaveQuestStateAndList, rbtQuestList, 0);
    SAVE_ROLE_BLOCK(m_BuffList.Save, rbtBuffList, 0);
    SAVE_ROLE_BLOCK(m_ReputeList.Save, rbtRepute, 0);
    SAVE_ROLE_BLOCK(m_UserPreferences.Save, rbtUserPreferences, 0);
    SAVE_ROLE_BLOCK(m_BookList.Save, rbtBookState, 0);
    SAVE_ROLE_BLOCK(m_TimerList.Save, rbtCoolDownTimer, 0);
    SAVE_ROLE_BLOCK(SaveRoadOpenNode, rbtRoadOpenList, 0);
    SAVE_ROLE_BLOCK(SaveHeroData, rbtHeroData, 0);
    SAVE_ROLE_BLOCK(m_CustomData.Save, rbtCustomData, 0);
    SAVE_ROLE_BLOCK(m_SceneVisitRecord.Save, rbtVisitedMap, 0);
    SAVE_ROLE_BLOCK(m_PQList.Save, rbtPQList, 0);
    //SAVE_ROLE_BLOCK(SaveAchievementData, rbtAchievementData, 0);

    dwExtDataLen = (DWORD)(pbyOffset - pbyBuffer - sizeof(KRoleDataHeader));

    pGlobalHeader->dwVer = 0;
    pGlobalHeader->dwCRC = CRC32(0, pbyBuffer + sizeof(KRoleDataHeader), dwExtDataLen);
    pGlobalHeader->dwLen = dwExtDataLen;

    *puUsedSize = pbyOffset - pbyBuffer;

    bRetCode = g_pSO3World->m_FellowshipMgr.SaveFellowshipData(m_dwID);
    KGLOG_PROCESS_ERROR(bRetCode);

    m_nLastSaveTime = g_pSO3World->m_nCurrentTime;

    m_nNextSaveFrame = g_pSO3World->m_nGameLoop 
        + g_pSO3World->m_Settings.m_ConstList.nSaveInterval * GAME_FPS;

    bResult = true;
Exit0:
    return bResult;
}

#undef SAVE_ROLE_BLOCK

BOOL KPlayer::SavePosition()
{
    assert(m_pScene);
    assert(m_pCell);

    m_SavePosition.dwMapID         = m_pScene->m_dwMapID;
    m_SavePosition.nMapCopyIndex   = m_pScene->m_nCopyIndex;
    m_SavePosition.nX              = m_nX;
    m_SavePosition.nY              = m_nY;
    m_SavePosition.nZ              = m_nZ;
    m_SavePosition.nFaceDirection  = m_nFaceDirection;

    return true;
}

BOOL KPlayer::SaveBaseInfo(KRoleBaseInfo* pBaseInfo)
{
    BOOL    bResult         = false;

    pBaseInfo->nVersion                     = 0;
	pBaseInfo->cRoleType                    = (char)m_eRoleType;
	pBaseInfo->byLevel                      = (BYTE)m_nLevel;
    pBaseInfo->byCamp                       = (BYTE)m_eCamp;
	pBaseInfo->byForceID                    = (BYTE)m_dwForceID;
	pBaseInfo->nLastSaveTime                = g_pSO3World->m_nCurrentTime;
	pBaseInfo->CurrentPos.nX                = m_SavePosition.nX;
	pBaseInfo->CurrentPos.nY                = m_SavePosition.nY;
	pBaseInfo->CurrentPos.nZ                = m_SavePosition.nZ;
	pBaseInfo->CurrentPos.dwMapID           = m_SavePosition.dwMapID;
	pBaseInfo->CurrentPos.nMapCopyIndex     = m_SavePosition.nMapCopyIndex;
    pBaseInfo->CurrentPos.byFaceDirection   = (BYTE)m_nFaceDirection;
    pBaseInfo->LastEntry.dwMapID            = m_LastEntry.dwMapID;
    pBaseInfo->LastEntry.nMapCopyIndex      = m_LastEntry.nMapCopyIndex;
    pBaseInfo->LastEntry.nX                 = m_LastEntry.nX;
    pBaseInfo->LastEntry.nY                 = m_LastEntry.nY;
    pBaseInfo->LastEntry.nZ                 = m_LastEntry.nZ;
    pBaseInfo->LastEntry.byFaceDirection    = (BYTE)m_LastEntry.nFaceDirection;
    pBaseInfo->nLastLoginTime               = m_nCurrentLoginTime;
    pBaseInfo->nCreateTime                  = m_nCreateTime;

    m_nTotalGameTime                        += g_pSO3World->m_nCurrentTime - max(m_nLastSaveTime, m_nCurrentLoginTime);
    pBaseInfo->nTotalGameTime               = m_nTotalGameTime;

    memcpy(pBaseInfo->wRepresentId, m_wRepresentId, sizeof(pBaseInfo->wRepresentId));

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayer::LoadBaseInfo(KRoleBaseInfo* pBaseInfo)
{
    BOOL        bResult         = false;
    BOOL        bRetCode        = false;
    int         nMaxX           = 0;
    int         nMaxY           = 0;
    KScene*     pScene          = NULL;
    
    assert(pBaseInfo);

    pScene = g_pSO3World->GetScene(pBaseInfo->CurrentPos.dwMapID, pBaseInfo->CurrentPos.nMapCopyIndex);
    KGLOG_PROCESS_ERROR(pScene);

    nMaxX = pScene->GetRegionWidth() * REGION_GRID_WIDTH * CELL_LENGTH - 1;
    nMaxY = pScene->GetRegionHeight() * REGION_GRID_HEIGHT * CELL_LENGTH - 1;

    MAKE_IN_RANGE(pBaseInfo->CurrentPos.nX, 0, nMaxX);
    MAKE_IN_RANGE(pBaseInfo->CurrentPos.nY, 0, nMaxY);
    MAKE_IN_RANGE(pBaseInfo->CurrentPos.nZ, 0, MAX_Z_POINT);

    // 玩家登录过程中,Load的时候,还没有真正的把玩家添加到场景中去,到客户端AckData的时候才会真正添加
    m_SavePosition.dwMapID          = pBaseInfo->CurrentPos.dwMapID;
    m_SavePosition.nMapCopyIndex    = pBaseInfo->CurrentPos.nMapCopyIndex;
    m_SavePosition.nX               = pBaseInfo->CurrentPos.nX;
    m_SavePosition.nY               = pBaseInfo->CurrentPos.nY;
    m_SavePosition.nZ               = pBaseInfo->CurrentPos.nZ;
    m_SavePosition.nFaceDirection   = pBaseInfo->CurrentPos.byFaceDirection;

    m_LastEntry.dwMapID             = pBaseInfo->LastEntry.dwMapID;
    m_LastEntry.nMapCopyIndex       = pBaseInfo->LastEntry.nMapCopyIndex;
    m_LastEntry.nX                  = pBaseInfo->LastEntry.nX;
    m_LastEntry.nY                  = pBaseInfo->LastEntry.nY;
    m_LastEntry.nZ                  = pBaseInfo->LastEntry.nZ;
    m_LastEntry.nFaceDirection      = pBaseInfo->LastEntry.byFaceDirection;

    m_nFaceDirection                = pBaseInfo->CurrentPos.byFaceDirection;
    m_eRoleType                     = (ROLE_TYPE)pBaseInfo->cRoleType;
    m_nLastSaveTime                 = pBaseInfo->nLastSaveTime;
    m_nLastLoginTime                = pBaseInfo->nLastLoginTime;
    m_nCurrentLoginTime             = g_pSO3World->m_nCurrentTime;
    m_nTotalGameTime                = pBaseInfo->nTotalGameTime;
    m_nCreateTime                   = pBaseInfo->nCreateTime;

    SetGenderByRoleType(pBaseInfo->cRoleType);
    
    SetLevel(pBaseInfo->byLevel);
    
    m_eCamp     = (KCAMP)pBaseInfo->byCamp;
    m_dwForceID = pBaseInfo->byForceID;

    memcpy(m_wRepresentId, pBaseInfo->wRepresentId, sizeof(m_wRepresentId));

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayer::SwitchMap(DWORD dwMapID, int nCopyIndex, int nX, int nY, int nZ)
{
    BOOL                     bResult        = false;
    BOOL                     bRetCode       = false;


    KGLOG_PROCESS_ERROR(m_eGameStatus == gsPlaying);

    // 这里将原来的位置记录下来,Search map失败的话还要用这个记录来恢复原来的位置
    bRetCode = SavePosition();
    KGLOG_PROCESS_ERROR(bRetCode);

    // 停止移动，否则会导致客户端和服务器状态不一致。
    if (m_eMoveState != cmsOnDeath && m_eMoveState != cmsOnAutoFly)
    {
        m_nConvergenceSpeed = 0;
        Stop();
    }

    m_nRecordCount   = 0;

    m_MoveCtrl.bMove = false;
    m_MoveCtrl.nTurn = 0;

	bRetCode = g_pSO3World->RemovePlayer(this);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = g_RelayClient.SaveRoleData(this);
    KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = g_RelayClient.DoSearchMapRequest(m_dwID, dwMapID, nCopyIndex, nX, nY, nZ);
	KGLOG_PROCESS_ERROR(bRetCode);

    m_eGameStatus = gsSearchMap;

    m_nBanishTime = 0; // 倒计时结束；或者玩家在倒计时内主动离开副本，这时候结束倒计时

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayer::CheckRevive(BOOL bFirstCheck)
{
    BOOL            bResult             = false;
    BOOL            bRetCode            = false;
    const char*     pszFunctionName     = "CheckPlayerRevive";
    int             nLuaTopIndex        = 0;

    assert(m_pScene);

    bRetCode = g_pSO3World->m_ScriptCenter.IsScriptExist(m_pScene->m_dwScriptID);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = g_pSO3World->m_ScriptCenter.IsFuncExist(m_pScene->m_dwScriptID, pszFunctionName);
    KGLOG_PROCESS_ERROR(bRetCode);

    g_pSO3World->m_ScriptCenter.SafeCallBegin(&nLuaTopIndex);

    g_pSO3World->m_ScriptCenter.PushValueToStack(this);
    g_pSO3World->m_ScriptCenter.PushValueToStack((bool)bFirstCheck);

    g_pSO3World->m_ScriptCenter.CallFunction(m_pScene->m_dwScriptID, pszFunctionName, 0);

    g_pSO3World->m_ScriptCenter.SafeCallEnd(nLuaTopIndex);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayer::Revive(int nMode)
{
    BOOL            bResult             = false;
    BOOL            bRetCode            = false;
    const char*     pszFunctionName     = "OnPlayerRevive";
    int             nLuaTopIndex        = 0;

    assert(m_pScene);

    SetMoveState(cmsOnStand);

    InvalidateMoveState(0, true);

	m_nPositiveShield = 0;
	m_nNegativeShield = 0;

    bRetCode = g_pSO3World->m_ScriptCenter.IsScriptExist(m_pScene->m_dwScriptID);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = g_pSO3World->m_ScriptCenter.IsFuncExist(m_pScene->m_dwScriptID, pszFunctionName);
    KGLOG_PROCESS_ERROR(bRetCode);

    g_pSO3World->m_ScriptCenter.SafeCallBegin(&nLuaTopIndex);

	g_pSO3World->m_ScriptCenter.PushValueToStack(this);
    g_pSO3World->m_ScriptCenter.PushValueToStack(nMode);

    g_pSO3World->m_ScriptCenter.CallFunction(m_pScene->m_dwScriptID, pszFunctionName, 0);

    g_pSO3World->m_ScriptCenter.SafeCallEnd(nLuaTopIndex);

    bResult = true;
Exit0:
    m_dwKillerID = ERROR_ID;
    return bResult;
}

#endif	//_SERVER


BOOL KPlayer::ChangeRegion(KRegion* pDestRegion)
{
    BOOL bResult  = false;
	BOOL bRetCode = false;

	assert(m_pRegion);
	assert(pDestRegion);

#ifdef _CLIENT
    if (m_dwID == g_pSO3World->m_dwClientPlayerID)
    {
        bRetCode = m_pScene->ValidateRegions(pDestRegion->m_nRegionX, pDestRegion->m_nRegionY);
        KGLOG_PROCESS_ERROR(bRetCode);
    }
#else
    bRetCode = m_pScene->ValidateRegions(pDestRegion->m_nRegionX, pDestRegion->m_nRegionY);
    KGLOG_PROCESS_ERROR(bRetCode);

    RegisterViewObject(m_pRegion, pDestRegion);
#endif

	m_RegionObjNode.Remove();
	pDestRegion->m_PlayerList.AddTail((KNode*)&m_RegionObjNode);
	m_pRegion = pDestRegion;

    bResult = true;
Exit0:
	return bResult;
}

#ifdef _SERVER
void KPlayer::SetCell(KCell* pDstCell)
{
    DWORD dwNewScriptID = 0;
    DWORD dwOldScriptID = m_dwLastScriptID;

    assert(m_pScene);
    assert(m_pRegion);

    if (pDstCell)
    {
        dwNewScriptID = m_pRegion->GetScriptIDByIndex(pDstCell->m_BaseInfo.dwScriptIndex);
    }

    if (m_dwLastScriptID != dwNewScriptID && m_dwLastScriptID)
    {
        CallTrapScript(m_dwLastScriptID, SCRIPT_ON_LEAVE_TRAP);
    }

    m_pCell = pDstCell;

    m_dwLastScriptID = dwNewScriptID;

    if (dwNewScriptID != dwOldScriptID && dwNewScriptID)
    {
        CallTrapScript(dwNewScriptID, SCRIPT_ON_ENTER_TRAP);
    }

    return;
}

BOOL KPlayer::CallTrapScript(DWORD dwScript, const char* szFunction)
{
    BOOL    bResult         = false;
    BOOL    bRetCode        = false;
    int     nLuaTopIndex    = 0;

	bRetCode = g_pSO3World->m_ScriptCenter.IsScriptExist(dwScript);
    KG_PROCESS_ERROR(bRetCode);

    bRetCode = g_pSO3World->m_ScriptCenter.IsFuncExist(dwScript, szFunction);
    KG_PROCESS_ERROR(bRetCode);

	g_pSO3World->m_ScriptCenter.SafeCallBegin(&nLuaTopIndex);

	g_pSO3World->m_ScriptCenter.PushValueToStack(this);

	g_pSO3World->m_ScriptCenter.CallFunction(dwScript, szFunction, 0);

	g_pSO3World->m_ScriptCenter.SafeCallEnd(nLuaTopIndex);

    bResult = true;
Exit0:
    return bResult;
}

#endif

BOOL KPlayer::CostStamina(int nStamina)
{
    BOOL bResult        = false;
	BOOL bRetCode       = false;
	int  nOldStamina    = m_nCurrentStamina;

	KGLOG_PROCESS_ERROR(nStamina >= 0);
	KG_PROCESS_SUCCESS(nStamina == 0);
	KG_PROCESS_ERROR(nStamina <= m_nCurrentStamina);

	m_nCurrentStamina -= nStamina;

#ifdef _SERVER
    g_PlayerServer.DoSyncSelfCurrentST(this);
#endif

Exit1:
	bResult = true;
Exit0:
	return bResult;
}

BOOL KPlayer::CostThew(int nThew)
{
    BOOL bResult        = false;
    BOOL bRetCode       = false;
    int  nOldThew       = m_nCurrentThew;

    KGLOG_PROCESS_ERROR(nThew >= 0);
    KG_PROCESS_SUCCESS(nThew == 0);
    KG_PROCESS_ERROR(nThew <= m_nCurrentThew);

    m_nCurrentThew -= nThew;

#ifdef _SERVER    
    g_PlayerServer.DoSyncSelfCurrentST(this);
#endif

Exit1:
    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayer::LearnProfession(DWORD dwProfessionID)
{
    BOOL bResult  = false;
	BOOL bRetCode = false;

	bRetCode = m_ProfessionList.LearnProfession(dwProfessionID);
	KG_PROCESS_ERROR(bRetCode);

#ifdef _SERVER
	g_PlayerServer.DoLearnProfessionNotify(m_nConnIndex, dwProfessionID);
#endif

	bResult = true;
Exit0:
	return bResult;
}

BOOL KPlayer::LearnBranch(DWORD dwProfessionID, DWORD dwBranchID)
{
    BOOL bResult  = false;
	BOOL bRetCode = false;

	bRetCode = m_ProfessionList.LearnBranch(dwProfessionID, dwBranchID);
	KG_PROCESS_ERROR(bRetCode);

#if defined(_SERVER)
	g_PlayerServer.DoLearnBranchNotify(m_nConnIndex, dwProfessionID, dwBranchID);
#endif

	bResult = true;
Exit0:
	return bResult;
}

BOOL KPlayer::ForgetProfession(DWORD dwProfessionID)
{
    BOOL bResult  = false;
	BOOL bRetCode = false;

	bRetCode = m_ProfessionList.ForgetProfession(dwProfessionID);
	KG_PROCESS_ERROR(bRetCode);

#if defined(_SERVER)
	g_PlayerServer.DoForgetProfessionNotify(m_nConnIndex, dwProfessionID);
#endif

	bResult = true;
Exit0:
	return bResult;
}

BOOL KPlayer::AddProfessionProficiency(DWORD dwProfessionID, DWORD dwExp)
{
    BOOL bResult  = false;
	BOOL bRetCode = false;
    
	bRetCode = m_ProfessionList.AddProfessionProficiency(dwProfessionID, dwExp);
	KG_PROCESS_ERROR(bRetCode);

#ifdef _SERVER
	g_PlayerServer.DoAddProfessionProficiency(m_nConnIndex, dwProfessionID, dwExp);
#endif

	bResult = true;
Exit0:
	return bResult;
}

BOOL KPlayer::SetMaxProfessionLevel(DWORD dwProfessionID, DWORD dwLevel)
{
    BOOL bResult  = false;
	BOOL bRetCode = false;

	bRetCode = m_ProfessionList.SetMaxLevel(dwProfessionID, dwLevel);
	KG_PROCESS_ERROR(bRetCode);

#if defined(_SERVER)
	g_PlayerServer.DoSetMaxProfessionLevelNotify(m_nConnIndex,dwProfessionID, dwLevel);
#endif

	bResult = true;
Exit0:
	return bResult;
}

BOOL KPlayer::CanLearnRecipe(DWORD dwCraftID, DWORD dwRecipeID, DWORD dwMasterID)
{
    BOOL                 bResult        = false;
	BOOL				 bRetCode		= false;
	KRecipeMaster*		 pRecipeMaster	= NULL;
	KRecipeLearningInfo* pLearnInfo		= NULL;

	pRecipeMaster = g_pSO3World->m_ProfessionManager.GetRecipeMaster(dwMasterID);
	KG_PROCESS_ERROR(pRecipeMaster);

	pLearnInfo = pRecipeMaster->GetLearnInfo(dwCraftID, dwRecipeID);
	KG_PROCESS_ERROR(pLearnInfo);

	bRetCode = m_RecipeList.CanLearnRecipe(dwCraftID, dwRecipeID, pLearnInfo);
	KG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
	return bResult;
}

BOOL KPlayer::LearnRecipe(DWORD dwCraftID, DWORD dwRecipeID)
{
    BOOL bResult  = false;
	BOOL bRetCode = false;

	bRetCode = m_RecipeList.LearnRecipe(dwCraftID, dwRecipeID);
	KG_PROCESS_ERROR(bRetCode);

#if defined(_SERVER)
	g_PlayerServer.DoLearnRecipeNotify(m_nConnIndex, dwCraftID, dwRecipeID);
#endif

	bResult = true;
Exit0:
	return bResult;
}

BOOL KPlayer::ForgetRecipe(DWORD dwProfessionID)
{
    BOOL    bResult     = false;
	BOOL    bRetCode    = false;
	int     nIndex      = 0;
	int     nSize       = 0;
	DWORD   dwCraftID   = 0;
	std::vector<DWORD> vecCraftIDList;

	bRetCode = g_pSO3World->m_ProfessionManager.GetCraftIDList(dwProfessionID, vecCraftIDList);
	KG_PROCESS_ERROR(bRetCode);

	nSize = (int)vecCraftIDList.size();
	for (nIndex = 0; nIndex < nSize; nIndex++)
	{
		dwCraftID = vecCraftIDList[nIndex];
		if (dwCraftID == 0)
			continue;

		bRetCode = m_RecipeList.ForgetRecipe(dwCraftID);
		KGLOG_CHECK_ERROR(bRetCode);

#ifdef _SERVER
        g_PlayerServer.DoForgetRecipeNotify(m_nConnIndex, dwCraftID);
#endif
	}

	bResult = true;
Exit0:
	return bResult;
}

CRAFT_RESULT_CODE KPlayer::CanCastProfessionSkill(DWORD dwCraftID, DWORD dwRecipeID, DWORD dwRBookItemID, KTarget& Target)
{
	CRAFT_RESULT_CODE   nResult     = crcFailed;
    CRAFT_RESULT_CODE   eRetCode    = crcFailed;
	KCraft*             pCraft      = NULL;
    
    KG_PROCESS_ERROR_RET_CODE(m_OTActionParam.eActionType == otActionIdle, crcDoingOTAction);

	pCraft = g_pSO3World->m_ProfessionManager.GetCraft(dwCraftID);
	KGLOG_PROCESS_ERROR(pCraft);
	
	eRetCode = pCraft->CanCast(this, dwRecipeID, dwRBookItemID, Target);
	KG_PROCESS_ERROR_RET_CODE(eRetCode == crcSuccess, eRetCode);

	nResult = crcSuccess;
Exit0:
	if (nResult != crcSuccess)
	{
		//出错时通知界面
#ifdef _SERVER
		KCraftCastState CraftCastState;
		CraftCastState.dwCraftID    = dwCraftID;
		CraftCastState.dwRecipeID   = dwRecipeID;
		CraftCastState.eTargetType  = Target.GetTargetType();
		CraftCastState.dwTargetID   = Target.GetTargetID();

		g_PlayerServer.DoMessageNotify(m_nConnIndex, ectCraftErrorCode, nResult, (void*)&CraftCastState, sizeof(CraftCastState));
#endif

#ifdef _CLIENT
        if (g_pGameWorldUIHandler)
        {
            KUIEventCraftCastRespond UIParam;
            UIParam.nCraftCastRespond   = nResult;
            UIParam.dwCraftID           = dwCraftID;
            UIParam.dwRecipeID          = dwRecipeID;
            UIParam.dwTargetType        = Target.GetTargetType();
            UIParam.dwTargetID          = Target.GetTargetID();
            g_pGameWorldUIHandler->OnCraftCastRespond(UIParam);
        }
#endif
	}

	return nResult;
}

CRAFT_RESULT_CODE KPlayer::CastProfessionSkill(DWORD dwCraftID, DWORD dwRecipeID, DWORD dwRBookItemID, KTarget& Target)
{
	CRAFT_RESULT_CODE   nResult         = crcFailed;
    CRAFT_RESULT_CODE   eRetCode        = crcFailed;
	BOOL                bRetCode        = false;
	KCraft*             pCraft          = NULL;
	KRecipeBase*        pRecipeBase     = NULL;
	KProfessionSkill    ProfessionSkill;

	pCraft = g_pSO3World->m_ProfessionManager.GetCraft(dwCraftID);
	KGLOG_PROCESS_ERROR(pCraft);

	pRecipeBase = pCraft->GetRecipe(dwRecipeID);
	KGLOG_PROCESS_ERROR(pRecipeBase);

#ifdef _SERVER
	eRetCode = CanCastProfessionSkill(dwCraftID, dwRecipeID, dwRBookItemID, Target);
	KG_PROCESS_ERROR_RET_CODE(eRetCode == crcSuccess, eRetCode);

    pCraft->CallBeginScriptFunction(this, dwRecipeID);
    
    if (pRecipeBase->nPrepareFrame == 0)
    {
    	eRetCode = pCraft->Cast(this, dwRecipeID, dwRBookItemID, Target);
        KG_PROCESS_ERROR_RET_CODE(eRetCode == crcSuccess, eRetCode);
        goto Exit1;
    }
#endif

    KGLOG_PROCESS_ERROR(pRecipeBase->nPrepareFrame > 0);

    ProfessionSkill.Target          = Target;
    ProfessionSkill.dwCraftID       = dwCraftID;
    ProfessionSkill.dwRecipeID      = dwRecipeID;
    ProfessionSkill.dwSourceItemID  = dwRBookItemID;
    
    bRetCode = DoRecipePrepare(&ProfessionSkill, pRecipeBase->nPrepareFrame);
    KG_PROCESS_ERROR(bRetCode);

#ifdef _SERVER
    if (m_bOnHorse)
    {
        DownHorse();
    }

    g_PlayerServer.DoStartUseRecipe(this, dwCraftID, dwRecipeID, Target);
#else
    if (m_dwID == g_pSO3World->m_dwClientPlayerID)
    {
	    //通知UI事件,开始走进度
        if (g_pGameWorldUIHandler)
        {
            KUIEventRecipeProgress UIParam;
            UIParam.nTotalFrame = pRecipeBase->nPrepareFrame;
            UIParam.dwCraftID = dwCraftID;
            UIParam.dwRecipeID = dwRecipeID;
            g_pGameWorldUIHandler->OnRecipePrepareProgress(UIParam);
        }
    }

    if (g_pGameWorldRepresentHandler)
    {
        g_pGameWorldRepresentHandler->OnCharacterBeginCraftAnimation((KCharacter*)this, dwCraftID);
    }
#endif
    
Exit1:
	nResult = crcSuccess;
Exit0:
	return nResult;
}

int KPlayer::GetCDValue(DWORD dwCooldownID)
{
    int nResult  = 0;

    nResult = g_pSO3World->m_Settings.m_CoolDownList.GetCoolDownValue(dwCooldownID);
    //KG_ASSERT_EXIT(nDuration > 0);

    // TODO: 查玩家天赋,某些天赋可以减少某些Cool down时间
    // ... ...
    
    return nResult;
}

BOOL KPlayer::MountKungfu(DWORD dwID, DWORD dwLevel)
{
    BOOL            bResult     = false;
    int             nRetCode    = false;
    KSkill*         pKungfu     = NULL;

    KGLOG_PROCESS_ERROR(m_SkillList.m_dwMountKungfuID == INVALID_SKILL_ID);

    pKungfu = g_pSO3World->m_SkillManager.GetSkill_K(dwID, dwLevel);
    KGLOG_PROCESS_ERROR(pKungfu);

    KGLOG_PROCESS_ERROR(pKungfu->m_pBaseInfo->bIsMountable);

    ApplyAttribute(pKungfu->m_pAttrbuteEffectToSelfAndRollback);

    m_SkillList.m_dwMountKungfuID       = dwID;
    m_SkillList.m_dwMountKungfuLevel    = dwLevel;

    m_dwSchoolID = pKungfu->m_pBaseInfo->dwBelongSchool;

#ifdef _CLIENT
    if (g_pGameWorldUIHandler)
	{
        KUIEventSkillUpdate Param;
        KUIEventUpdatePlayerSchoolID UpdateSchoolID;

		Param.dwSkillID     = pKungfu->m_pBaseInfo->dwSkillID;
		Param.dwSkillLevel  = pKungfu->m_dwLevel;
		g_pGameWorldUIHandler->OnSkillMountKungFu(Param);

        UpdateSchoolID.dwPlayerID = m_dwID;
        UpdateSchoolID.dwSchoolID = m_dwSchoolID;
        g_pGameWorldUIHandler->OnUpdatePlayerSchoolID(UpdateSchoolID);
	}	
#endif

#ifdef _SERVER
    nRetCode = g_PlayerServer.DoSyncKungfuMount(m_nConnIndex, dwID, dwLevel);
    KGLOG_CHECK_ERROR(nRetCode);

    g_PlayerServer.DoBroadcastPlayerSchoolID(this);
#endif

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayer::UmountKungfu()
{
    BOOL    bResult  = false;
    BOOL    bRetCode = false;
    KSkill* pKungfu  = NULL;

#ifdef _CLIENT
	KUIEventSkillUpdate Param;
	Param.dwSkillID = m_SkillList.m_dwMountKungfuID;
	Param.dwSkillLevel = m_SkillList.m_dwMountKungfuLevel;
	int nNotify = m_SkillList.m_dwMountKungfuID != INVALID_SKILL_ID;
#endif

    if (m_SkillList.m_dwMountKungfuID != INVALID_SKILL_ID)
    {
        pKungfu = g_pSO3World->m_SkillManager.GetSkill_K(m_SkillList.m_dwMountKungfuID, m_SkillList.m_dwMountKungfuLevel);
        KGLOG_PROCESS_ERROR(pKungfu);

        UnApplyAttribute(pKungfu->m_pAttrbuteEffectToSelfAndRollback);
    }

    m_SkillList.m_dwMountKungfuID       = INVALID_SKILL_ID;
    m_SkillList.m_dwMountKungfuLevel    = 0;

    m_dwSchoolID = 0;

#ifdef _CLIENT
	if (nNotify)
    {
        if (g_pGameWorldUIHandler)
        {
		    g_pGameWorldUIHandler->OnSkillUnmountKungfu(Param);
        }
    }
#endif
    
#ifdef _SERVER
    bRetCode = g_PlayerServer.DoSyncKungfuMount(m_nConnIndex, INVALID_SKILL_ID, 0);
    KGLOG_CHECK_ERROR(bRetCode);
#endif

    bResult = true;
Exit0:
    return bResult;
}

#ifdef _SERVER
BOOL KPlayer::CallDeathScript(DWORD dwKiller)
{
    BOOL        bResult             = false;
	BOOL        bRetCode            = false;
	int         nTopIndex           = 0;
	KPlayer*    pKiller             = NULL;

    assert(m_pScene);

	pKiller = g_pSO3World->m_PlayerSet.GetObj(dwKiller);

	bRetCode = g_pSO3World->m_ScriptCenter.IsScriptExist(m_pScene->m_dwScriptID);
    KGLOG_PROCESS_ERROR(bRetCode);

	g_pSO3World->m_ScriptCenter.SafeCallBegin(&nTopIndex);

	g_pSO3World->m_ScriptCenter.PushValueToStack(this);

	if (pKiller)
    {
		g_pSO3World->m_ScriptCenter.PushValueToStack(pKiller);
    }

	g_pSO3World->m_ScriptCenter.CallFunction(m_pScene->m_dwScriptID, SCRIPT_ON_PLAYER_DEATH, 0);

	g_pSO3World->m_ScriptCenter.SafeCallEnd(nTopIndex);

	bResult = true;
Exit0:
	return bResult;
}
#endif

#ifdef _CLIENT
BOOL KPlayer::CanFinishQuestOnNpc(KNpc* pNpc)
{   
    BOOL       bResult      = false;
    BOOL       bRetCode     = false;
    int        nQuestCounet = 0;
    std::vector<DWORD>  vecQuestID;

    assert(pNpc);

	bRetCode = g_pSO3World->m_Settings.m_QuestInfoList.GetNpcQuest(pNpc->m_dwTemplateID, &vecQuestID);
    KGLOG_PROCESS_ERROR(bRetCode);
    
    nQuestCounet = (int)vecQuestID.size();
	for (int i = 0; i < nQuestCounet; ++i)
	{
		bRetCode = CanFinishTheQuestOnNpc(vecQuestID[i], pNpc);
        if (bRetCode)
        {
            goto Exit1;
        }
	}
    
	goto Exit0;

Exit1:
    bResult = true;
Exit0:
	return bResult;
}

BOOL KPlayer::CanFinishTheQuestOnNpc(DWORD dwQuestID, KNpc* pNpc)
{
    BOOL                bResult             = false;
    BOOL                bRetCode            = false;
	QUEST_RESULT_CODE   eCanFinishQuest	    = qrcInvalid;
	KQuestInfo*         pQuestInfo			= NULL;
    int                 nRelation           = sortNone;
    KTarget             Target;

    assert(pNpc);

	KG_PROCESS_ERROR(pNpc->m_bDialogFlag);

    nRelation = pNpc->GetPlayerRelation(this);
    KG_PROCESS_ERROR(!(nRelation & sortEnemy));
    
    pQuestInfo = g_pSO3World->m_Settings.m_QuestInfoList.GetQuestInfo(dwQuestID);
	KGLOG_PROCESS_ERROR(pQuestInfo);

    bRetCode = Target.SetTarget(pNpc);
    KGLOG_PROCESS_ERROR(bRetCode);

	eCanFinishQuest = m_QuestList.CanFinish(pQuestInfo, &Target);
    KG_PROCESS_ERROR(eCanFinishQuest == qrcSuccess);
	
    bResult = true;
Exit0:
	return bResult;
}

QUEST_DIFFICULTY_LEVEL KPlayer::GetQuestDiffcultyLevel(DWORD dwQuestID)
{
    QUEST_DIFFICULTY_LEVEL      nResult				        = qdlInvalid;
	QUEST_DIFFICULTY_LEVEL      nTempResult			        = qdlInvalid;
    BOOL                        bRetCode                    = false;
    int*                        pnPlayerAndQuestDiffLevel   = NULL;
    int                         nDiffLevel                  = 0;
    KQuestInfo*                 pQuestInfo                  = NULL;

    QUEST_DIFFICULTY_LEVEL eQuestLevel[PLAYER_AND_QUEST_DIFF_LEVEL_COUNT - 1] = {
        qdlHighLevel, qdlProperLevel, qdlLowLevel
    };
    
    pQuestInfo = g_pSO3World->m_Settings.m_QuestInfoList.GetQuestInfo(dwQuestID);
    KGLOG_PROCESS_ERROR(pQuestInfo);

    nDiffLevel = m_nLevel - pQuestInfo->nLevel;

    pnPlayerAndQuestDiffLevel = &(g_pSO3World->m_Settings.m_ConstList.nPlayerAndQuestDiffLevel[0]);

    if (nDiffLevel < pnPlayerAndQuestDiffLevel[0])
    {
        nTempResult = qdlHigherLevel;
        goto Exit1;
    }

    for (int i = 0; i < PLAYER_AND_QUEST_DIFF_LEVEL_COUNT - 1; ++i)
    {
        if (nDiffLevel >= pnPlayerAndQuestDiffLevel[i] && nDiffLevel < pnPlayerAndQuestDiffLevel[i + 1])
        {
            nTempResult = eQuestLevel[i];
            goto Exit1;
        }
    }
    
    if (nDiffLevel >= pnPlayerAndQuestDiffLevel[PLAYER_AND_QUEST_DIFF_LEVEL_COUNT - 1])
    {
        nTempResult = qdlLowerLevel;
        goto Exit1;
    }

Exit1:
    nResult = nTempResult;
Exit0:
	return nResult;
}
#endif //_CLIENT

// ------ Talk about --------------------------------------------------->
#ifdef _CLIENT
BOOL KPlayer::CopyTalkData(DWORD dwTalkerID, size_t uSize, BYTE* pbyData)
{
    BOOL bResult = false;

    KGLOG_PROCESS_ERROR(m_dwID == g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(uSize > 0 && uSize <= MAX_CHAT_TEXT_LEN);
    KGLOG_PROCESS_ERROR(pbyData);

    memcpy(s_byTalkData, pbyData, uSize);
    s_uTalkDataSize = (unsigned)uSize;

    s_bFilterTalkText = IS_PLAYER(dwTalkerID); // 只有玩家的聊天信息需要过滤

    bResult = true;
Exit0:
    return bResult;
}
#endif

int KPlayer::GetNpcRelation(KNpc* pTarget)
{
	int nRelation = 0;

	BOOL bFightFlag = false;
	int nReputeLevel = 0;

	assert(IS_NPC(pTarget->m_dwID));

	nReputeLevel = m_ReputeList.GetReputeLevel(pTarget->m_dwForceID);

	if (nReputeLevel == NEUTRAL_REPUTE_LEVEL)
	{
		nRelation |= sortNeutrality;
	}

	if (nReputeLevel >= ALLY_REPUTE_LEVEL)
	{
		nRelation |= sortAlly;
		nRelation &= ~sortNeutrality;
	}

#ifdef _SERVER
	if (m_SimpThreatList.IsInThreatList(pTarget))
	{
		nRelation |= sortEnemy;
		nRelation &= ~sortAlly;
		nRelation &= ~sortNeutrality;
	}
#endif

#ifdef _CLIENT
	if (m_ThreatList.IsInThreatList(pTarget->m_dwID))
	{
		nRelation |= sortEnemy;
		nRelation &= ~sortAlly;
		nRelation &= ~sortNeutrality;
	}
#endif

	bFightFlag = m_ReputeList.GetFightFlag(pTarget->m_dwForceID);
	if (bFightFlag)
	{
		nRelation |= sortEnemy;
		nRelation &= ~sortAlly;
		nRelation &= ~sortNeutrality;
	}

	return nRelation;
}

inline BOOL IsBattleEnemy(int nSelfSide, int nDestSide, DWORD dwRelationMask)
{
    BOOL bResult = false;
    int  nPos    = 0;
    
    KG_PROCESS_ERROR(nSelfSide >= 0);
    KG_PROCESS_ERROR(nDestSide >= 0);

    if (nSelfSide > nDestSide)  // 这个函数，是把一个4*4的矩阵压缩到一个DWORD里。
    {                           // 由于矩阵的数据是对称的，我们只取X <= Y的部分。
        int nTemp = nSelfSide;

        nSelfSide = nDestSide;
        nDestSide = nTemp;
    }

    for (int i = 1; i <= nDestSide; i++)
    {
        nPos += i;
    }
    nPos += nSelfSide; // 0开始的

    assert(nPos < sizeof(DWORD) * CHAR_BIT);

    bResult = (dwRelationMask & (1 << nPos));
Exit0:
    return bResult;
}

int KPlayer::GetPlayerRelation(KPlayer* pTarget)
{
	int             nRelation           = 0;
    BOOL            bRetCode            = false;
    int             nMinBeSlayedLevel   = g_pSO3World->m_Settings.m_ConstList.nMinBeSlayedLevel;
    int             nValue              = 0;

    assert(pTarget);
	assert(IS_PLAYER(pTarget->m_dwID));
    assert(m_pScene);
    assert(pTarget->m_pScene);

	nValue = g_pSO3World->m_Settings.m_RelationCampList.GetRelation(m_eCamp, pTarget->m_eCamp);
	switch(nValue)
	{
	case -1:
		nRelation |= sortEnemy;
		nRelation &= ~sortAlly;
		nRelation &= ~sortNeutrality;
		break;
	case 0:
		nRelation |= sortNeutrality;
		nRelation &= ~sortAlly;
		nRelation &= ~sortEnemy;
		break;
	case 1:
		nRelation |= sortAlly;
		nRelation &= ~sortNeutrality;
		nRelation &= ~sortEnemy;
		break;
	default:
		break;
	}

#ifdef _SERVER
	if (m_SimpThreatList.IsInThreatList(pTarget))
	{
		nRelation |= sortEnemy;
		nRelation &= ~sortAlly;
		nRelation &= ~sortNeutrality;
	}
#endif

#ifdef _CLIENT
	if (m_ThreatList.IsInThreatList(pTarget->m_dwID))
	{
        nRelation |= sortEnemy;
        nRelation &= ~sortAlly;
        nRelation &= ~sortNeutrality;
	}
#endif
    
    if (m_pScene->m_bCanPK)
    {
        if (g_pSO3World->m_FellowshipMgr.GetFoe(m_dwID, pTarget->m_dwID) != NULL)
        {
            nRelation |= sortEnemy;
            nRelation &= ~sortAlly;
            nRelation &= ~sortNeutrality;
        }

	    if (
            ((m_PK.GetState() == pksSlaying || m_PK.GetState() == pksExitSlay) && pTarget->m_nLevel >= nMinBeSlayedLevel) ||
            ((pTarget->m_PK.GetState() == pksSlaying || pTarget->m_PK.GetState() == pksExitSlay) && m_nLevel >= nMinBeSlayedLevel)
        )
	    {
		    nRelation |= sortEnemy;
		    nRelation &= ~sortAlly;
		    nRelation &= ~sortNeutrality;
	    }

	    if (
            (m_PK.GetTargetPlayer() == pTarget && ((m_PK.GetState() == pksDueling) || (m_PK.GetState() == pksDuelOutOfRange))) ||
             pTarget->m_PK.GetTargetPlayer() == this && ((pTarget->m_PK.GetState() == pksDueling) || (pTarget->m_PK.GetState() == pksDuelOutOfRange))
        )
	    {
		    nRelation |= sortEnemy;
		    nRelation &= ~sortAlly;
		    nRelation &= ~sortNeutrality;
	    }
        
        if (pTarget->m_bRedName)
        {
            nRelation |= sortEnemy;
		    nRelation &= ~sortAlly;
		    nRelation &= ~sortNeutrality;
        }
    }

	if (m_dwTeamID != ERROR_ID)
	{
#ifdef _SERVER
        bRetCode = g_pSO3World->m_TeamServer.IsPlayerInParty(m_dwTeamID, pTarget->m_dwID);
#else
        bRetCode = g_pSO3World->m_TeamClient.IsPlayerInParty(pTarget->m_dwID);
#endif
        if (bRetCode)
        {
		    nRelation |= sortParty;
		    nRelation |= sortAlly;
		    nRelation &= ~sortEnemy;
		    nRelation &= ~sortNeutrality;
        }
	}
	
    if (
        (m_nBattleFieldSide > INVALID_BATTLE_SIDE && m_nBattleFieldSide < MAX_BATTLE_SIDE) ||
        (pTarget->m_nBattleFieldSide > INVALID_BATTLE_SIDE && pTarget->m_nBattleFieldSide < MAX_BATTLE_SIDE)
    ) // 只要有一个人在战场中，就应该进入判断。
    { // 也许是GM之类的人，在战场里观看。
        bRetCode = IsBattleEnemy(m_nBattleFieldSide, pTarget->m_nBattleFieldSide, m_pScene->m_dwBattleRelationMask);
        if (bRetCode)
        {
            nRelation |= sortEnemy;
            nRelation &= ~sortAlly;
            nRelation &= ~sortNeutrality;
        }
        else
        {
            nRelation |= sortAlly;
            nRelation &= ~sortEnemy;
            nRelation &= ~sortNeutrality;
        }
    }

	if (this == pTarget)
	{
		nRelation = sortSelf;
	}

	return nRelation;
}

#ifdef _CLIENT
BOOL KPlayer::SwitchSelectTarget(DWORD dwSkillSrcID)
{
    BOOL        bResult     = false;
    BOOL        bRetCode    = false;
    TARGET_TYPE eTargetType = ttInvalid;
    KCharacter* pCaster     = NULL;

    if (IS_PLAYER(dwSkillSrcID))
    {
        int nRelation = 0;
        pCaster = (KCharacter*)g_pSO3World->m_PlayerSet.GetObj(dwSkillSrcID);
        KG_PROCESS_ERROR(pCaster);
        nRelation = ((KPlayer*)pCaster)->GetPlayerRelation(this);
        KG_PROCESS_ERROR(nRelation & sortEnemy);

        eTargetType = ttPlayer;
    }
    else    
    {
        int nRelation = 0;
        pCaster = (KCharacter*)g_pSO3World->m_NpcSet.GetObj(dwSkillSrcID);
        KG_PROCESS_ERROR(pCaster);
        nRelation = ((KNpc*)pCaster)->GetPlayerRelation(this);
        KG_PROCESS_ERROR(nRelation & sortEnemy);

        eTargetType = ttNpc;
    }

    bRetCode = SelectTarget(eTargetType, dwSkillSrcID);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}
#endif

#if defined(_SERVER)
void KPlayer::RecordPosition()
{
    int     nRecordIndex = m_nVirtualFrame % MAX_POSITION_RECORD;

    m_PositionRecord[nRecordIndex].nFrame                  = m_nVirtualFrame;
    m_PositionRecord[nRecordIndex].eMoveState              = m_eMoveState;
    m_PositionRecord[nRecordIndex].nX                      = m_nX;
    m_PositionRecord[nRecordIndex].nY                      = m_nY;
    m_PositionRecord[nRecordIndex].nZ                      = m_nZ;
    m_PositionRecord[nRecordIndex].nDestX                  = m_nDestX;
    m_PositionRecord[nRecordIndex].nDestY                  = m_nDestY;
    m_PositionRecord[nRecordIndex].nVelocityXY             = m_nVelocityXY;
    m_PositionRecord[nRecordIndex].nDirectionXY            = m_nDirectionXY;
    m_PositionRecord[nRecordIndex].nVelocityZ              = m_nVelocityZ;
    m_PositionRecord[nRecordIndex].nConvergenceSpeed       = m_nConvergenceSpeed;
    m_PositionRecord[nRecordIndex].nCurrentGravity         = m_nCurrentGravity;
    m_PositionRecord[nRecordIndex].nJumpCount              = m_nJumpCount;	
    m_PositionRecord[nRecordIndex].nFaceDirection          = m_nFaceDirection;
    m_PositionRecord[nRecordIndex].nDivingCount            = m_nDivingCount;  
    m_PositionRecord[nRecordIndex].bTryPathState           = m_bTryPathState;
    m_PositionRecord[nRecordIndex].nTryPathSide            = m_nTryPathSide;
    m_PositionRecord[nRecordIndex].bSlip                   = m_bSlip;
    m_PositionRecord[nRecordIndex].nJumpLimitFrame         = m_nJumpLimitFrame;
    m_PositionRecord[nRecordIndex].nCurrentTrack           = m_nCurrentTrack;
    m_PositionRecord[nRecordIndex].nFromNode               = m_nFromNode;
    m_PositionRecord[nRecordIndex].nTargetCity             = m_nTargetCity;
    m_PositionRecord[nRecordIndex].nMoveFrameCount         = m_nMoveFrameCounter;

    m_nVirtualFrame++;

    if (m_nRecordCount < MAX_POSITION_RECORD)
    {
        m_nRecordCount++;
    }
}

BOOL KPlayer::ReversePosition(int nFrame)
{
    BOOL                bResult         = false;
	BOOL                bRetCode        = false;
    int                 nReverseFrame   = 0;
    int                 nRecordIndex    = 0;
    KPOSITION_RECORD*   pRecord         = NULL;

    assert(nFrame > 0);

    nReverseFrame = min(m_nRecordCount, nFrame);
    KG_PROCESS_ERROR(nReverseFrame > 0);

	nRecordIndex = (m_nVirtualFrame - nReverseFrame) % MAX_POSITION_RECORD;

    pRecord = &m_PositionRecord[nRecordIndex];

    assert(pRecord->nFrame == m_nVirtualFrame - nReverseFrame);

    bRetCode = MoveTo(pRecord->nX, pRecord->nY, pRecord->nZ, true, false);
    KGLOG_PROCESS_ERROR(bRetCode);

    // 其实MoveTo可能导致自己被从场景移除
    KGLOG_PROCESS_ERROR(m_pScene);

    m_eMoveState          =   pRecord->eMoveState;  
    m_nDestX              =   pRecord->nDestX;
    m_nDestY              =   pRecord->nDestY;
    m_nVelocityXY         =   pRecord->nVelocityXY;
    m_nDirectionXY        =   pRecord->nDirectionXY;
    m_nVelocityZ          =   pRecord->nVelocityZ;
    m_nConvergenceSpeed   =   pRecord->nConvergenceSpeed;
    m_nCurrentGravity     =   pRecord->nCurrentGravity;
    m_nJumpCount          =	  pRecord->nJumpCount;   
    m_nFaceDirection      =   pRecord->nFaceDirection;   
    m_nDivingCount        =   pRecord->nDivingCount;    
    m_bTryPathState       =   pRecord->bTryPathState;
    m_nTryPathSide        =   pRecord->nTryPathSide;
    m_bSlip               =   pRecord->bSlip;
    m_nJumpLimitFrame     =   pRecord->nJumpLimitFrame;
    m_nCurrentTrack       =   pRecord->nCurrentTrack;
    m_nFromNode           =   pRecord->nFromNode;
    m_nTargetCity         =   pRecord->nTargetCity;
    m_nMoveFrameCounter   =   pRecord->nMoveFrameCount;

    m_nVirtualFrame -= nReverseFrame;

    bResult = true;
Exit0:
    // 发生一次回滚以后,历史记录即可作废,不会再回滚到这之前
    m_nRecordCount = 0;
	return bResult;
}

BOOL KPlayer::CheckMoveAdjust(int nClientFrame, const KMOVE_CRITICAL_PARAM& crParam)
{
    BOOL bResult        = true;

    if (m_nVirtualFrame > nClientFrame)
    {
        ReversePosition(m_nVirtualFrame - nClientFrame);
    }
    else if (m_nVirtualFrame < nClientFrame)
    {
        ForwardPosition(nClientFrame - m_nVirtualFrame);
    }

    KG_PROCESS_ERROR(m_eMoveState == crParam.byMoveState);
    KG_PROCESS_ERROR(m_nRunSpeed == crParam.nRunSpeed);
    KG_PROCESS_ERROR(m_nCurrentGravity == crParam.nGravity);
    KG_PROCESS_ERROR(m_nJumpSpeed == crParam.nJumpSpeed);
    KG_PROCESS_ERROR(m_nX == crParam.nX);
    KG_PROCESS_ERROR(m_nY == crParam.nY);
    KG_PROCESS_ERROR(m_nZ == crParam.nZ);

    bResult = false; // 返回false表示无须修正
Exit0:
    return bResult;
}
#endif

BOOL KPlayer::ForwardPosition(int nFrame)
{
    BOOL bResult = false;

	while (nFrame-- > 0)
	{
        RunStateMachine();
        KG_PROCESS_ERROR(m_pScene);

        ProcessMove();        
        KG_PROCESS_ERROR(m_pScene);

        MoveCtrl();

        #ifdef _SERVER
        m_nVirtualFrame++;
        #endif
	}

    bResult = true;
Exit0:
#ifdef _SERVER
    // 只要发生了快进,就不可能再次回滚,所以清楚掉历史记录
    m_nRecordCount = 0;
#endif
	return bResult;
}


#ifdef _SERVER
void KPlayer::DoCycleSynchronous()
{
    int nFrame = g_pSO3World->m_nGameLoop - (int)m_dwID;

    // 向自己同步最大血量
    if (nFrame % 5 == 0)
    {
        g_PlayerServer.DoSyncSelfMaxLMRS(this);
    }

    // 向自己同步当前血量
    if (nFrame % 3 == 0)
    {
        g_PlayerServer.DoSyncSelfCurrentLMRS(this);
    }

    // 向自己同步当前破绽
    if (nFrame % 5 == 0)
    {
        g_PlayerServer.DoSyncSelfWeakInfo(this);
    }

    // 向自己同步目标的最大血量
    if (nFrame % 8 == 0)
    {
        g_PlayerServer.DoSyncTargetMaxLMR(this);
    }

    // 向自己同步目标的当前血量
    if (nFrame % 4 == 0)
    {
        g_PlayerServer.DoSyncTargetCurrentLMR(this);
    }

    // 向自己同步目标的当前破绽
    if (nFrame % 4 == 0)
    {
        g_PlayerServer.DoSyncTargetWeakInfo(this);
    }

    // 同步玩家选择目标的Buff list
    if (nFrame % GAME_FPS == 0)
    {
        g_PlayerServer.DoSyncTargetBuffList(this);
    }

    // 同步玩家选择目标掉落所属
    if (nFrame % GAME_FPS == 0)
    {
        g_PlayerServer.DoSyncTargetDropID(this);
    }

    // 同步玩家选择的目标的目标的最大血量
    if (nFrame % GAME_FPS == 0)
    {
        g_PlayerServer.DoSyncTargetTargetMaxLMR(this);
    }

    // 同步玩家选择的目标的目标的当前血量
    if (nFrame % 8 == 0)
    {
        g_PlayerServer.DoSyncTargetTargetCurrentLMR(this);
    }

    // 同步玩家选择的目标的目标的Buff List
    if (nFrame % GAME_FPS == 0)
    {
        g_PlayerServer.DoSyncTargetTargetBuffList(this);
    }

    // 全局同步队友最大血量等
    if (nFrame % GAME_FPS == 0)
    {
	    g_RelayClient.DoTeamSyncMemberMaxLMR(this);
    }

    // 跨服同步队友当前血量等
    if (nFrame % GAME_FPS == 0)
    {
	    g_RelayClient.DoTeamSyncMemberCurrentLMR(this);
    }

    // 同步附近队友当前生命,注意,这个同步的频率会被跨服同步覆盖
    if (nFrame % 7 == 0)
    {
	    g_PlayerServer.DoSyncTeamMemberCurrentLMRLocal(this);
    }

    // 跨服同步队友当前位置
    if (nFrame % (GAME_FPS * 2) == 0)
    {
	    g_RelayClient.DoSyncTeamMemberPosition(this);
    }

    // 本地同步队友当前位置,注意,这个同步的频率会被跨服同步覆盖
    if (nFrame % GAME_FPS == 0)
    {
	    g_PlayerServer.DoSyncTeamMemberPositionLocal(this);
    }

    // 全局同步队友杂项参数
    if (nFrame % (2 * GAME_FPS) == 0)
    {
	    g_RelayClient.DoPartySyncMemberMisc(this);
    }

    // 向周围广播当前血量
    if (nFrame % GAME_FPS == 0)
    {
        g_PlayerServer.DoBroadcastCharacterLife(this);
    }

    // 同步精力体力
    if (nFrame % GAME_FPS == 0)
    {
	    g_PlayerServer.DoSyncSelfCurrentST(this);
    }

    // 同步阵营信息
    if (nFrame % (GAME_FPS * 30) == 0)
    {
        g_PlayerServer.DoSyncCampInfo(this);
    }
}

void KPlayer::ResetPartyLastSyncParamRecord()
{
    m_nLastSyncLocalPartyLifePercent            = -1;
    m_nLastSyncLocalPartyManaPercent            = -1;
    m_dwLastSyncLocalPartyMemberMapID           = 0;
    m_nLastSyncLocalPartyMemberMapCopyIndex     = 0;
    m_nLastSyncLocalPartyMemberPosX             = -1;
    m_nLastSyncLocalPartyMemberPosY             = -1;

    m_nLastSyncGlobalPartyLifePercent           = -1;
    m_nLastSyncGlobalPartyManaPercent           = -1;
    m_dwLastSyncGlobalPartyMemberMapID          = 0;
    m_nLastSyncGlobalPartyMemberMapCopyIndex    = 0;
    m_nLastSyncGlobalPartyMemberPosX            = -1;
    m_nLastSyncGlobalPartyMemberPosY            = -1;
}

#endif

BOOL KPlayer::RideHorse()
{
    int             nResult     = false;
    int             nRetCode    = false;
    KItem*          pHorse      = NULL;

    pHorse = m_ItemList.GetItem(ibEquip, eitHorse);
    KGLOG_PROCESS_ERROR(pHorse);
    
    KG_PROCESS_ERROR(!pHorse->m_bApplyed);
    KG_PROCESS_ERROR(!m_bOnHorse);

    nRetCode = m_ItemList.ApplyAttrib(pHorse);
    KGLOG_PROCESS_ERROR(nRetCode);

    m_bOnHorse = true;

#ifdef _SERVER
    g_PlayerServer.DoSyncHorseFlag(this);
#endif
    
    nResult = true;
Exit0:
    return nResult;    
}

BOOL KPlayer::DownHorse()
{
    int             nResult     = false;
    int             nRetCode    = false;
    KItem*          pHorse      = NULL;

    pHorse = m_ItemList.GetItem(ibEquip, eitHorse);
    KGLOG_PROCESS_ERROR(pHorse);
    
    KG_PROCESS_ERROR(pHorse->m_bApplyed);
    KG_PROCESS_ERROR(m_bOnHorse);

    nRetCode = m_ItemList.UnApplyAttrib(pHorse);
    KGLOG_PROCESS_ERROR(nRetCode);

    m_bOnHorse = false;

#ifdef _SERVER
    g_PlayerServer.DoSyncHorseFlag(this);
#endif

    nResult = true;
Exit0:
    return nResult;    
}

#ifdef _SERVER
BOOL KPlayer::AddTrain(int nTrain)
{
    int             nResult         = false;
    int             nOldTrainValue  = m_nCurrentTrainValue;

    KG_PROCESS_ERROR(nOldTrainValue + nTrain >= 0);

    m_nCurrentTrainValue += nTrain;

    if (nTrain >= 0)
    {
        m_nCurrentTrainValue = min(m_nMaxTrainValue, m_nCurrentTrainValue); // 是否超过上限
        m_nCurrentTrainValue = max(m_nCurrentTrainValue, nOldTrainValue);   // 已经超过上限的，只是不增加
    }
    // else 其他情况不做处理。

    g_LogClient.LogTrainvalueChange(m_nCurrentTrainValue - nOldTrainValue, m_szAccount, m_szName);

    g_PlayerServer.DoSyncPlayerTrain(this);

    nResult = true;
Exit0:
    return nResult;
}

BOOL KPlayer::AddTrainNoLimit(int nTrain)
{
    int nResult = false;

    KG_PROCESS_ERROR(m_nCurrentTrainValue + nTrain >= 0);

    m_nCurrentTrainValue += nTrain;

    g_LogClient.LogTrainvalueChange(nTrain, m_szAccount, m_szName);

    g_PlayerServer.DoSyncPlayerTrain(this);

    nResult = true;
Exit0:
    return nResult;
}

BOOL KPlayer::ShareQuest(DWORD dwQuestID)
{
    BOOL                bResult         = false;
    BOOL                bRetCode        = false;
    int                 nCode           = 0;
    QUEST_RESULT_CODE   eQuestRetCode   = qrcFailed;
    KQuestInfo*         pQuestInfo      = NULL;
    KTeam*              pTeam           = NULL;
    KPlayer*            pMember         = NULL;
    KTarget             Target;
    KSHARE_QUEST_RESULT ShareQuestResult;

	pQuestInfo = g_pSO3World->m_Settings.m_QuestInfoList.GetQuestInfo(dwQuestID);
	KGLOG_PROCESS_ERROR(pQuestInfo);

	KG_PROCESS_ERROR(pQuestInfo->bShare);
    
	KGLOG_PROCESS_ERROR(m_dwTeamID != ERROR_ID);

	pTeam = g_pSO3World->m_TeamServer.GetTeam(m_dwTeamID);
	KGLOG_PROCESS_ERROR(pTeam);
    
    Target.SetTarget(this);
    
    ShareQuestResult.dwQuestID = dwQuestID;

    for (int i = 0; i < pTeam->nGroupNum; i++)
	{
        for (
            KTEAM_MEMBER_LIST::iterator it = pTeam->MemberGroup[i].MemberList.begin();
            it != pTeam->MemberGroup[i].MemberList.end(); ++it
        )
        {
            if (it->dwMemberID == m_dwID)
            {
                continue;
            }

            pMember = g_pSO3World->m_PlayerSet.GetObj(it->dwMemberID);
            if (pMember == NULL)
            {
                continue;
            }

            ShareQuestResult.dwTargetPlayerID = pMember->m_dwID;

            bRetCode = g_InRange(this, pMember, CELL_LENGTH * REGION_GRID_WIDTH);
            if (!bRetCode)
            {
                g_PlayerServer.DoMessageNotify(m_nConnIndex, ectShareQuestErrorCode, sqrcTooFar, &ShareQuestResult, sizeof(ShareQuestResult));
                continue;
            }

            eQuestRetCode = pMember->m_QuestList.CanAccept(pQuestInfo, &Target);

            switch (eQuestRetCode)
            {
            case qrcSuccess:
                nCode = sqrcSuccess;
                g_PlayerServer.DoShareQuest(pMember, this, dwQuestID);
                break;
            case qrcAlreadyAcceptQuest:
                nCode = sqrcAlreadyAcceptQuest;
                break;
            case qrcAlreadyFinishedQuest:
                nCode = sqrcAlreadyFinishedQuest;
                break;
            case qrcQuestListFull:
                nCode = sqrcQuestListFull;
                break;
            default:
                nCode = sqrcFailed;
                break;
            }

            g_PlayerServer.DoMessageNotify(m_nConnIndex, ectShareQuestErrorCode, nCode, &ShareQuestResult, sizeof(ShareQuestResult));
        }
	}

    bResult = true;
Exit0:
    return bResult;
}
#endif

#ifdef _SERVER
BOOL KPlayer::IsFormationLeader()
{
    BOOL    bResult             = false;
    KTeam*  pTeam               = NULL;

    KG_PROCESS_ERROR(m_dwTeamID != ERROR_ID);
    
    pTeam = g_pSO3World->m_TeamServer.GetTeam(m_dwTeamID);
    KGLOG_PROCESS_ERROR(pTeam);

    for (int i = 0; i < pTeam->nGroupNum; i++)
    {
        if (m_dwID == pTeam->MemberGroup[i].dwFormationLeader)
        {
            bResult = true;
            goto Exit0;
        }
    }

Exit0:
    return bResult;
}
#endif

#ifdef _SERVER
BOOL KPlayer::CanSetFormationLeader()
{
    BOOL    bResult             = false;
    BOOL    bISFormationLeader  = false;
    KTeam*  pTeam               = NULL;

    KG_PROCESS_ERROR(m_dwTeamID != ERROR_ID);
    
    pTeam = g_pSO3World->m_TeamServer.GetTeam(m_dwTeamID);
    KGLOG_PROCESS_ERROR(pTeam);

    bISFormationLeader = IsFormationLeader();

    KG_PROCESS_ERROR(bISFormationLeader || m_dwID == pTeam->dwAuthority[tatLeader]);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayer::SyncFormationCoefficient()
{
    BOOL                            bResult = false;
    KTeam*                          pTeam   = NULL;

    KG_PROCESS_SUCCESS(m_dwTeamID == ERROR_ID); // 没有队伍就不用同步了

    pTeam = g_pSO3World->m_TeamServer.GetTeam(m_dwTeamID);
    if (!pTeam)
    {
        KGLogPrintf(KGLOG_ERR, "[SyncFormationCoefficient] Team:%lu can't found.\n", m_dwTeamID);
        goto Exit0;
    }

    for (int i = 0; i < pTeam->nGroupNum; i++)
    {
        for (
            KTEAM_MEMBER_LIST::iterator it = pTeam->MemberGroup[i].MemberList.begin();
            it != pTeam->MemberGroup[i].MemberList.end(); ++it
        )
        {
            KPlayer*        pMember         = NULL;
            KGFellowship*   pFellowship     = NULL;
            int             nCoefficient    = 0;

            if (m_dwID == it->dwMemberID)
                continue;

            pMember = g_pSO3World->m_PlayerSet.GetObj(it->dwMemberID);

            if (!pMember)
                continue;

            pFellowship = g_pSO3World->m_FellowshipMgr.GetFellowship(m_dwID, it->dwMemberID);
            if (pFellowship)
            {
                nCoefficient += pFellowship->m_nAttraction;
            }

            pFellowship = g_pSO3World->m_FellowshipMgr.GetFellowship(it->dwMemberID, m_dwID);
            if (pFellowship)
            {
                nCoefficient += pFellowship->m_nAttraction;
            }

            if (nCoefficient > 0)
            {
                g_PlayerServer.DoSyncFormationCoefficient(pMember, m_dwID, nCoefficient);
            }
        }
    }

Exit1:
    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayer::ProcessTeamMemberAttraction()
{
    BOOL    bResult     = false;
    BOOL    bRetCode    = false;
    int     nGroupIndex = INVALID_TEAM_GROUP;
    KTeam*  pTeam       = NULL;

    KGLOG_PROCESS_ERROR(m_pScene);

    pTeam = g_pSO3World->m_TeamServer.GetTeam(m_dwTeamID);
    KG_PROCESS_ERROR(pTeam);

    nGroupIndex = GetCurrentTeamGroupIndex();
    KGLOG_PROCESS_ERROR(nGroupIndex != INVALID_TEAM_GROUP);

    for (int i = 0; i < pTeam->nGroupNum; i++)
    {
        if (i == nGroupIndex)
            continue;

        for (
            KTEAM_MEMBER_LIST::iterator it = pTeam->MemberGroup[i].MemberList.begin();
            it != pTeam->MemberGroup[i].MemberList.end(); ++it
        )
        {
            KTEAM_MEMBER_INFO*  pMemberInfo = &(*it);
            KPlayer*            pMember     = NULL;
            KGFellowship*       pFellowship = NULL;

            if (pMemberInfo->dwMemberID == m_dwID)
                continue;

            if (pMemberInfo->dwMapID != m_pScene->m_dwMapID)
                continue;

            pMember = g_pSO3World->m_PlayerSet.GetObj(pMemberInfo->dwMemberID);
            KG_PROCESS_ERROR(pMember);

            bRetCode = g_InRange(this, pMember, g_pSO3World->m_Settings.m_ConstList.nPartyAttractiveRange);
            if (!bRetCode)
                continue;

            pFellowship = g_pSO3World->m_FellowshipMgr.GetFellowship(m_dwID, pMemberInfo->dwMemberID);
            if (!pFellowship)
                continue;

            if (pFellowship->m_nAttraction >= g_pSO3World->m_Settings.m_ConstList.nPartyAttractiveUpLimit)
                continue;

            g_pSO3World->m_FellowshipMgr.AddFellowshipAttraction(
                m_dwID, pMemberInfo->dwMemberID, g_pSO3World->m_Settings.m_ConstList.nPartyAttractiveAdd, false
            );
        }
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayer::ProcessTeamMemberOnlyGroupAttraction()
{
    BOOL    bResult     = false;
    BOOL    bRetCode    = false;
    int     nGroupIndex = INVALID_TEAM_GROUP;
    KTeam*  pTeam       = NULL;

    KGLOG_PROCESS_ERROR(m_pScene);

    pTeam = g_pSO3World->m_TeamServer.GetTeam(m_dwTeamID);
    KG_PROCESS_ERROR(pTeam);

    nGroupIndex = GetCurrentTeamGroupIndex();

    for (
        KTEAM_MEMBER_LIST::iterator it = pTeam->MemberGroup[nGroupIndex].MemberList.begin();
        it != pTeam->MemberGroup[nGroupIndex].MemberList.end(); ++it
    )
    {
        KTEAM_MEMBER_INFO*  pMemberInfo = &(*it);
        KPlayer*            pMember     = NULL;
        KGFellowship*       pFellowship = NULL;

        if (pMemberInfo->dwMemberID == m_dwID)
            continue;

        if (pMemberInfo->dwMapID != m_pScene->m_dwMapID)
            continue;

        pMember = g_pSO3World->m_PlayerSet.GetObj(pMemberInfo->dwMemberID);
        KG_PROCESS_ERROR(pMember);

        bRetCode = g_InRange(this, pMember, g_pSO3World->m_Settings.m_ConstList.nPartyAttractiveRangeOnlyGroup);
        if (!bRetCode)
            continue;

        pFellowship = g_pSO3World->m_FellowshipMgr.GetFellowship(m_dwID, pMemberInfo->dwMemberID);
        if (!pFellowship)
            continue;

        if (pFellowship->m_nAttraction >= g_pSO3World->m_Settings.m_ConstList.nPartyAttractiveUpLimitOnlyGroup)
            continue;

        g_pSO3World->m_FellowshipMgr.AddFellowshipAttraction(
            m_dwID, pMemberInfo->dwMemberID, g_pSO3World->m_Settings.m_ConstList.nPartyAttractiveAddOnlyGroup, false
        );
    }

    bResult = true;
Exit0:
    return bResult;
}
#endif

BOOL KPlayer::CanAddFoe()
{
    BOOL        bResult             = false;
    int         nMinRevengeLevel    = g_pSO3World->m_Settings.m_ConstList.nMinRevengeLevel;
    
    KG_PROCESS_ERROR(m_nLevel >= nMinRevengeLevel);
    KG_PROCESS_ERROR(!m_bFightState);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayer::IsFoe(DWORD dwDestPlayerID)
{
    BOOL    bResult     = false;    
    KGFoe*  pFoe        = NULL;

    pFoe = g_pSO3World->m_FellowshipMgr.GetFoe(m_dwID, dwDestPlayerID);
    KG_PROCESS_ERROR(pFoe);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayer::SetCamp(KCAMP eNewCamp)
{
    BOOL    bResult     = false;

    KGLOG_PROCESS_ERROR(m_eCamp != eNewCamp);

    m_eCamp = eNewCamp;
    
#ifdef _SERVER
    m_nCurrentPrestige = 0;
    g_PlayerServer.DoSyncCurrentPrestige(this);
    
    g_PlayerServer.DoSetCamp(this);

    m_ItemList.OnChangeCamp();

    CallChangeCampScript();
#endif

#ifdef _CLIENT
    if (g_pGameWorldUIHandler && g_pGameWorldRepresentHandler)
    {
	    if (m_dwID == g_pSO3World->m_dwClientPlayerID)
	    {
		    KPlayer* pClientPlayer = (KPlayer*)this;
		    KGLOG_PROCESS_ERROR(pClientPlayer);

            pClientPlayer->m_QuestList.UpdateNpcQuestMark(-1); // 更新任务周围Npc的任务标记
            
            g_pGameWorldUIHandler->OnUpdateAllRelation();
            g_pGameWorldRepresentHandler->OnCharacterUpdateAllRelation();
	    }
        else
        {
            KUIEventUpdateRelation param = { m_dwID };
            
            g_pGameWorldUIHandler->OnUpdateRelation(param);
            g_pGameWorldRepresentHandler->OnCharacterUpdateRelation(this);
        }
        
        KUIEventChangeCamp param = { m_dwID };
        g_pGameWorldUIHandler->OnChangeCamp(param);
    }
#endif

    bResult = true;
Exit0:
    return bResult;
}

#ifdef _SERVER
BOOL KPlayer::AddPrestige(int nAddPrestige)
{
    BOOL        bResult             = false;
    long long   llPrestigePercent   = KILO_NUM;

    KG_PROCESS_SUCCESS(m_eCamp == cNeutral);
    KG_PROCESS_SUCCESS(nAddPrestige == 0);

    if (nAddPrestige > 0)
    {        
        int nCampLevel = g_pSO3World->m_CampInfo.GetCampLevel();
        llPrestigePercent = g_pSO3World->m_CampInfo.GetPrestigePercent(m_eCamp, nCampLevel);
        nAddPrestige = (int)(nAddPrestige * llPrestigePercent / KILO_NUM);

        KGLOG_PROCESS_ERROR(m_nCurrentPrestige < (m_nCurrentPrestige + nAddPrestige)); // 防止上溢
    }
    
    m_nCurrentPrestige += nAddPrestige;
    if (m_nCurrentPrestige < 0)
    {
        m_nCurrentPrestige = 0; // 威望值非负
    }

    g_PlayerServer.DoSyncCurrentPrestige(this);

Exit1:
    bResult = true;
Exit0:
    return bResult;
}

void KPlayer::ProcessCampPK(DWORD dwKillerID)
{
    BOOL                    bRetCode                = false;
    int                     nReducePrestigeOnDeath  = 0;
    int                     nAddPrestige            = 0;
    const KSimpThreatNode*  cpThreatNode            = NULL;
    size_t                  uKillerCount            = 0;
    KPlayer*                pKiller                 = NULL;
    int                     nDiffLevelPercent       = 0;
    KGWConstList*           pConstList              = &g_pSO3World->m_Settings.m_ConstList;
    std::vector<KPlayer*>   vecAllKillerID;
    
    KG_PROCESS_ERROR(m_eCamp != cNeutral); // 中立阵营不做处理
    
    // 死亡扣威望
    nReducePrestigeOnDeath = g_pSO3World->m_CampInfo.GetReducePrestigeOnDeath(m_eCamp, g_pSO3World->m_CampInfo.GetCampLevel());
    nAddPrestige = pConstList->nReducePrestigeOnDeath * nReducePrestigeOnDeath / KILO_NUM;

    bRetCode = AddPrestige(nAddPrestige);
    KGLOG_PROCESS_ERROR(bRetCode);
    
    cpThreatNode = m_SimpThreatList.GetFirstThreat(thtMainThreat);
    KG_PROCESS_ERROR(cpThreatNode);

    while (cpThreatNode)
    {
        if (cpThreatNode->pCharacter && IS_PLAYER(cpThreatNode->pCharacter->m_dwID))
        {
            vecAllKillerID.push_back((KPlayer*)cpThreatNode->pCharacter);
        }

        cpThreatNode = m_SimpThreatList.GetNextThreat(thtMainThreat, cpThreatNode);
    }
    
    uKillerCount = vecAllKillerID.size();
    
    // 处理相同阵营的惩罚
    for (size_t i = 0; i < uKillerCount; ++i)
    {
        KPlayer* pPlayer = vecAllKillerID[i];
        if (pPlayer->m_eCamp != m_eCamp)
        {
            continue;
        }
        
        nAddPrestige = pConstList->nSameCampAssistKill;
        if (pPlayer->m_dwID == dwKillerID)
        {
            nAddPrestige = pConstList->nSameCampKill;
        }

        bRetCode = pPlayer->AddPrestige(nAddPrestige);
        KGLOG_PROCESS_ERROR(bRetCode);
    }
    
    KG_PROCESS_ERROR(dwKillerID != ERROR_ID && IS_PLAYER(dwKillerID));

    pKiller = g_pSO3World->m_PlayerSet.GetObj(m_dwKillerID);
    KG_PROCESS_ERROR(pKiller);
    
    // 给Killer加威望
    if (pKiller->m_eCamp != m_eCamp)
    {
        if (uKillerCount == 1)
        {
            if ((vecAllKillerID[0]->m_dwID == dwKillerID)) // solo kill
            {
                nAddPrestige = pConstList->nSoloKill;
            }
        }
        else
        {
            std::vector<KPlayer*>::iterator it;
            it = std::find(vecAllKillerID.begin(), vecAllKillerID.end(), pKiller);
            
            if (it != vecAllKillerID.end()) // last hit
            {
                nAddPrestige = pConstList->nLastHit;
            }
        }

        nDiffLevelPercent = GetDiffLevelPercent(pKiller->m_nLevel);
        nAddPrestige = nAddPrestige * nDiffLevelPercent / 100;
        
        bRetCode = pKiller->AddPrestige(nAddPrestige);
        KGLOG_PROCESS_ERROR(bRetCode);
    }
    
    // 给同步范围内的队友加威望
    if (pKiller->m_dwTeamID != ERROR_ID && pKiller->m_eCamp != m_eCamp)
    {
        KTeam* pTeam = NULL;

        pTeam = g_pSO3World->m_TeamServer.GetTeam(pKiller->m_dwTeamID);
        KGLOG_PROCESS_ERROR(pTeam);

        for (int i = 0; i < pTeam->nGroupNum; i++)
        {
            for (
                KTEAM_MEMBER_LIST::iterator it = pTeam->MemberGroup[i].MemberList.begin();
                it != pTeam->MemberGroup[i].MemberList.end(); ++it
            )
            {
                KPlayer* pMember = NULL;

                if (it->dwMemberID == dwKillerID)
                {
                    continue;
                }

                pMember = g_pSO3World->m_PlayerSet.GetObj(it->dwMemberID);
                if (pMember == NULL)
                {
                    continue;
                }

                if (pMember->m_eCamp == m_eCamp)
                {
                    continue;
                }

                bRetCode = g_InRange(pKiller, pMember, CELL_LENGTH * REGION_GRID_WIDTH);
                if (!bRetCode)
                {
                    continue;
                }

                nDiffLevelPercent = GetDiffLevelPercent(pMember->m_nLevel);
                nAddPrestige = pConstList->nKillerPartyMember * nDiffLevelPercent / 100;

                bRetCode = pMember->AddPrestige(nAddPrestige);
                KGLOG_PROCESS_ERROR(bRetCode);
            }
        }
    }
    
    // 给AssistKiller加威望,可能同时也是队友
    for (size_t i = 0; i < uKillerCount; ++i)
    {
        KPlayer* pPlayer = vecAllKillerID[i];
        if (pPlayer->m_eCamp == m_eCamp)
        {
            continue;
        }
        
        if (pPlayer->m_dwID == dwKillerID)
        {
            continue;
        }

        nDiffLevelPercent = GetDiffLevelPercent(pPlayer->m_nLevel);
        nAddPrestige = pConstList->nCommonKill * nDiffLevelPercent / 100;

        bRetCode = pPlayer->AddPrestige(nAddPrestige);
        KGLOG_PROCESS_ERROR(bRetCode);
    }
    
Exit0:
    return;
}

#define PVPSCRIPTS SCRIPT_DIR"/player/PVPScript.lua"

void KPlayer::CallChangeCampScript()
{
    BOOL        bRetCode    = false;
    int         nTopIndex   = 0;
    const char* pszFuncName = "OnChangeCamp";

    bRetCode = g_pSO3World->m_ScriptCenter.IsScriptExist(PVPSCRIPTS);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = g_pSO3World->m_ScriptCenter.IsFuncExist(PVPSCRIPTS, pszFuncName);
    KG_PROCESS_ERROR(bRetCode);

    g_pSO3World->m_ScriptCenter.SafeCallBegin(&nTopIndex);

    g_pSO3World->m_ScriptCenter.PushValueToStack(this);

    g_pSO3World->m_ScriptCenter.CallFunction(PVPSCRIPTS, pszFuncName, 0);

    g_pSO3World->m_ScriptCenter.SafeCallEnd(nTopIndex);

Exit0:
    return;
}

int KPlayer::GetDiffLevelPercent(int nKillerLevel)
{
    int anDiffLevelPercent[]    = { 200, 175, 150, 125, 100, 100, 100, 75, 50, 25, 0 };
	int nDiffLevel              = nKillerLevel - m_nLevel;
	
    MAKE_IN_RANGE(nDiffLevel, -5, 5);
    
    return anDiffLevelPercent[nDiffLevel + 5];
}
#endif

void KPlayer::setHeroFlag(int nValue)
{
    KG_PROCESS_ERROR(m_bHeroFlag != nValue);
    
    m_bHeroFlag = (BOOL)nValue;

#ifdef _CLIENT
    g_PlayerClient.DoSetHeroFlag(nValue);
#endif

#ifdef _SERVER
    g_PlayerServer.DoSyncHeroFlag(this);
#endif

Exit0:
    return;
}

#ifdef _SERVER
BOOL KPlayer::AcquireAchievement(int nID)
{
    BOOL                bResult         = false;
    BOOL                bRetCode        = false;
    BOOL                bValue          = false;
    KACHIEVEMENT_INFO*  pInfo           = NULL;
    BYTE                byType          = aatInvalid;

    KGLOG_PROCESS_ERROR(nID >= 0 && nID < MAX_ACHIEVEMENT_ID);

    bRetCode = m_AchievementSet.GetBit(nID, &bValue);
    KGLOG_PROCESS_ERROR(bRetCode);
    KG_PROCESS_SUCCESS(bValue); // 成就不能重复获取

    pInfo = g_pSO3World->m_Settings.m_AchievementInfoList.GetInfo(nID);
    KGLOG_PROCESS_ERROR(pInfo);

    bRetCode = m_AchievementSet.SetBit(nID, true);
    KGLOG_PROCESS_ERROR(bRetCode);

    m_nAchievementPoint  += pInfo->nPoint;
    m_nAchievementRecord += pInfo->nPoint;

    AddExp(pInfo->nExp);

    g_PlayerServer.DoNewAchievementNotify(this, nID);

    if (pInfo->nPoint <= aalNearbyPoint)
    {
        byType = aatNearby;
    }
    else if (pInfo->nPoint <= aalScenePoint)
    {
        byType = aatScene;
    }
    else //if (pInfo->nPoint <=aalGlobalPoint)
    {
        byType = aatGlobal;
    }

    if (byType == aatGlobal)
    {
        g_RelayClient.DoAchievementGlobalAnnounceRequest(m_dwID, nID);
    }
    else
    {
        g_PlayerServer.BroadcastAchievementAnnounce(m_dwID, byType, m_szName, nID);
    }

Exit1:
    bResult = true;
Exit0:
    return bResult;
}

BOOL KPlayer::AddAchievementPoint(int nDeltaPoint)
{
    BOOL bResult = false;

    KGLOG_PROCESS_ERROR(nDeltaPoint >= 0 || m_nAchievementPoint >= -nDeltaPoint);

    m_nAchievementPoint += nDeltaPoint;

    g_PlayerServer.DoSyncAchievementPoint(this, nDeltaPoint);

    bResult = true;
Exit0:
    return bResult;
}
#endif // _SERVER
