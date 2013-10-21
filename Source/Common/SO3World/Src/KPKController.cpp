#include "stdafx.h"
#include "KPlayer.h"
#include "KPKController.h"
#include "KPlayerServer.h"
#include "KScene.h"
#include "KSO3World.h"

#ifdef _CLIENT
#include "SO3UI/SO3GameWorldUIHandler.h"
#include "SO3Represent/SO3GameWorldRepresentHandler.h"
#endif

#define MAX_DUELING_RANGE       (100 * CELL_LENGTH)

#define MAX_WAIT_DUEL_RESPOND_TIME	(30 * GAME_FPS)
#define MAX_DUEL_BEGIN_TIME	        (5 * GAME_FPS)
#define MAX_DULE_OUTRANGE_TIME	    (10 * GAME_FPS)
#define MAX_WAIT_SLAY_TIME          (5 * GAME_FPS)
#define MAX_CLOSE_SLAY_TIME	        (5 * GAME_FPS)

KPKController::KPKController()
{
	m_pPlayer = NULL;
}

KPKController::~KPKController()
{
	m_pPlayer           = NULL;
	m_dwTargetID        = ERROR_ID;
    m_dwDuelLoser       = ERROR_ID;
	m_eState            = pksIdle;

#ifdef _SERVER
    m_nTimer            = 0;
    m_nDuelCenterX      = 0;
    m_nDuelCenterY      = 0;
    m_nDuelCenterZ      = 0;
    m_dwMapID           = 0;
    m_dwFlagDoodadID    = 0;
#endif
}

BOOL KPKController::Init(KPlayer* pPlayer)
{
	BOOL bResult = false;
	
	assert(pPlayer);

	m_pPlayer           = pPlayer;
    m_dwTargetID        = ERROR_ID;
    m_dwDuelLoser       = ERROR_ID;
	m_eState            = pksIdle;

#ifdef _SERVER
	m_nTimer            = 0;
	m_nDuelCenterX      = 0;
	m_nDuelCenterY      = 0;
    m_nDuelCenterZ      = 0;
    m_dwMapID           = 0;
	m_dwFlagDoodadID    = 0;
#endif

	bResult = true;
Exit0:
	return bResult;
}

void KPKController::UnInit()
{
#ifdef _SERVER
    if (m_dwFlagDoodadID != ERROR_ID)
    {
        DeleteDuelFlag();
        m_dwFlagDoodadID = ERROR_ID;
    }
#endif
}

KPlayer* KPKController::GetTargetPlayer()
{
    KPlayer* pTargetPlayer = NULL;

    if (m_dwTargetID)
    {
        pTargetPlayer = g_pSO3World->m_PlayerSet.GetObj(m_dwTargetID);
    }

    return pTargetPlayer;
}


PK_RESULT_CODE KPKController::CanApplyDuel(KPlayer* pTarget)
{
	PK_RESULT_CODE  nResult         = pkrcFailed;
	BOOL            bRetCode        = false;
    int             nMinDuelLevel   = g_pSO3World->m_Settings.m_ConstList.nMinDuelLevel;

    assert(pTarget);
    assert(m_pPlayer);
    
    KG_PROCESS_ERROR(m_pPlayer->m_pScene && m_pPlayer->m_pScene->m_bCanPK);

    // 检查自己是否可以发起申请
	KG_PROCESS_ERROR_RET_CODE(m_pPlayer->m_eMoveState != cmsOnDeath, pkrcApplyDuelFailed);
	KG_PROCESS_ERROR_RET_CODE(m_eState == pksIdle, pkrcApplyDuelFailed);
	KG_PROCESS_ERROR_RET_CODE(m_pPlayer->m_nLevel >= nMinDuelLevel, pkrcLevelTooLow);
    
    bRetCode = m_pPlayer->IsFoe(pTarget->m_dwID);
    KG_PROCESS_ERROR(!bRetCode);

    KG_PROCESS_ERROR(!m_pPlayer->m_bFightState); // 战斗中不能发起切磋

    // 检查对方是否可以接受申请
	KG_PROCESS_ERROR_RET_CODE(pTarget->m_eMoveState != cmsOnDeath, pkrcApplyDuelFailed);
    KG_PROCESS_ERROR_RET_CODE(!pTarget->m_PK.IsDuel(), pkrcTargetIsDueling);

    KG_PROCESS_ERROR_RET_CODE(pTarget->m_PK.m_eState == pksIdle, pkrcApplyDuelFailed);
	KG_PROCESS_ERROR_RET_CODE(pTarget->m_nLevel >= nMinDuelLevel, pkrcLevelTooLow);
    
    bRetCode = pTarget->IsFoe(m_pPlayer->m_dwID);
    KG_PROCESS_ERROR(!bRetCode);
    
    KG_PROCESS_ERROR(!pTarget->m_bFightState); // 目标处于战斗状态不能切磋

	KG_PROCESS_ERROR_RET_CODE(
        m_pPlayer->m_dwTeamID == ERROR_ID || pTarget->m_dwTeamID == ERROR_ID ||
        m_pPlayer->m_dwTeamID != pTarget->m_dwTeamID, 
        pkrcInSameTeam
    );

	KG_PROCESS_ERROR_RET_CODE(m_pPlayer->m_pScene == pTarget->m_pScene, pkrcOutofRange);

    bRetCode = g_InRange(m_pPlayer, pTarget, COMMON_PLAYER_OPERATION_DISTANCE);
    KG_PROCESS_ERROR_RET_CODE(bRetCode, pkrcOutofRange);

	nResult = pkrcSuccess;
Exit0:
	return nResult;
}

PK_RESULT_CODE KPKController::CanApplySlay()
{
	PK_RESULT_CODE nResult          = pkrcFailed;
    int            nRetCode         = false;
    int            nMinSlayLevel    = g_pSO3World->m_Settings.m_ConstList.nMinSlayLevel;

    assert(m_pPlayer);
    
    KG_PROCESS_ERROR(m_pPlayer->m_pScene && m_pPlayer->m_pScene->m_bCanPK);

	KG_PROCESS_ERROR_RET_CODE(m_pPlayer->m_eMoveState != cmsOnDeath, pkrcApplySlayFailed);
	KG_PROCESS_ERROR_RET_CODE(m_eState == pksIdle, pkrcApplySlayFailed);
	KG_PROCESS_ERROR_RET_CODE(m_pPlayer->m_nLevel >= nMinSlayLevel, pkrcLevelTooLow);
    
    nRetCode = m_pPlayer->m_TimerList.CheckTimer(g_pSO3World->m_Settings.m_ConstList.nSlayCDIndex);
    KG_PROCESS_ERROR_RET_CODE(nRetCode, pkrcPKNotReady);

	nResult = pkrcSuccess;
Exit0:
	return nResult;
}

void KPKController::SetState(
    PK_STATE    eState, 
    int         nEndFrame/* = 0 */, 
    DWORD       dwTarget/* = ERROR_ID */, 
    BOOL        bRefuseDuel/* = false */
)
{
    BOOL        bRetCode        = false;
    PK_STATE    eOldState       = pksInvalid;
    DWORD       dwOldTargetID   = m_dwTargetID;

    assert(m_pPlayer);

    eOldState       = m_eState;

    m_eState        = eState;
    m_dwTargetID    = dwTarget;
    
#ifdef _SERVER
    m_nTimer = nEndFrame;
#endif

#ifdef _CLIENT
    KG_PROCESS_ERROR(g_pGameWorldUIHandler);
    KG_PROCESS_ERROR(g_pGameWorldRepresentHandler);

    if (eState == pksWaitForDuelRespond && m_dwTargetID)
    {
        KPlayer* pPlayer = g_pSO3World->m_PlayerSet.GetObj(m_dwTargetID);
        KGLOG_PROCESS_ERROR(pPlayer);

        KUIEventApplyDuel UIParam;
        UIParam.dwSrcPlayerID = m_pPlayer->m_dwID;
        UIParam.dwDstPlayerID = m_dwTargetID;
        g_pGameWorldUIHandler->OnApplyDuel(UIParam);
    }
    else if (eState == pksConfirmDuel && m_dwTargetID)
    {
        KPlayer* pPlayer = g_pSO3World->m_PlayerSet.GetObj(m_dwTargetID);
        KGLOG_PROCESS_ERROR(pPlayer);

        KUIEventApplyDuel UIParam;
        UIParam.dwSrcPlayerID = m_dwTargetID;
        UIParam.dwDstPlayerID = m_pPlayer->m_dwID;
        g_pGameWorldUIHandler->OnApplyDuel(UIParam);
    }
    else if (eOldState == pksWaitForDuelRespond && eState == pksPrepareDuel && m_dwTargetID)
    {
        KUIEventAcceptDuel UIParam;
        UIParam.dwSrcPlayerID = m_dwTargetID;
        UIParam.dwDstPlayerID = m_pPlayer->m_dwID;
        UIParam.nEndFrame     = nEndFrame;
        g_pGameWorldUIHandler->OnAcceptDuel(UIParam);
    }
    else if (eOldState == pksConfirmDuel && eState == pksPrepareDuel && m_dwTargetID)
    {
        KUIEventAcceptDuel UIParam;
        UIParam.dwSrcPlayerID = m_pPlayer->m_dwID;
        UIParam.dwDstPlayerID = m_dwTargetID;
        UIParam.nEndFrame     = nEndFrame;
        g_pGameWorldUIHandler->OnAcceptDuel(UIParam);
    }
    else if (eOldState == pksWaitForDuelRespond && eState == pksIdle && dwOldTargetID)
    {
        if (bRefuseDuel)
        {
            KUIEventRefuseDuel UIParam;
            UIParam.dwSrcPlayerID = dwOldTargetID;
            UIParam.dwDstPlayerID = m_pPlayer->m_dwID;
            g_pGameWorldUIHandler->OnRefuseDuel(UIParam);
        }
        else
        {
            KUIEventCancelDuel UIParam;
            UIParam.dwTargetPlayerID = m_pPlayer->m_dwID;
            g_pGameWorldUIHandler->OnCancelDuel(UIParam);
        }
    }
    else if (eOldState == pksConfirmDuel && eState == pksIdle && dwOldTargetID)
    {
        if (bRefuseDuel)
        {
            KUIEventRefuseDuel UIParam;
            UIParam.dwSrcPlayerID = m_pPlayer->m_dwID;
            UIParam.dwDstPlayerID = dwOldTargetID;
            g_pGameWorldUIHandler->OnRefuseDuel(UIParam);
        }
        else
        {
            KUIEventCancelDuel UIParam;
            UIParam.dwTargetPlayerID = m_pPlayer->m_dwID;
            g_pGameWorldUIHandler->OnCancelDuel(UIParam);
        }
    }
    else if (eOldState == pksPrepareDuel && eState == pksIdle)
    {
        KUIEventCancelDuel UIParam;
        UIParam.dwTargetPlayerID = m_pPlayer->m_dwID;
        g_pGameWorldUIHandler->OnCancelDuel(UIParam);
    }
    else if (eOldState == pksPrepareDuel && eState == pksDueling && m_dwTargetID)
    {
        KUIEventStartDuel UIEventStartDuel;
        UIEventStartDuel.dwTargetPlayerID = m_dwTargetID;
        g_pGameWorldUIHandler->OnStartDuel(UIEventStartDuel);

        KUIEventUpdateRelation UIEventUpdateRelation;
        UIEventUpdateRelation.dwCharacterID = m_dwTargetID;
        g_pGameWorldUIHandler->OnUpdateRelation(UIEventUpdateRelation);

        KCharacter* pTargetCharacter = NULL;
        if (IS_PLAYER(m_dwTargetID))
            pTargetCharacter = (KCharacter*)g_pSO3World->m_PlayerSet.GetObj(m_dwTargetID);
        else
            pTargetCharacter = (KCharacter*)g_pSO3World->m_NpcSet.GetObj(m_dwTargetID);

        KGLOG_PROCESS_ERROR(pTargetCharacter);

        g_pGameWorldRepresentHandler->OnCharacterUpdateRelation(pTargetCharacter);
    }
    else if (eState == pksDuelOutOfRange)
    {
        g_pGameWorldUIHandler->OnLeaveDuel();
    }
    else if (eOldState == pksDuelOutOfRange && eState == pksDueling)
    {
        g_pGameWorldUIHandler->OnReturnDuel();
    }
    else if (eOldState == pksDuelOutOfRange && eState == pksIdle)
    {
        KUIEventUpdateRelation UIEventUpdateRelation;
        UIEventUpdateRelation.dwCharacterID = dwOldTargetID;
        g_pGameWorldUIHandler->OnUpdateRelation(UIEventUpdateRelation);
        g_pGameWorldUIHandler->OnFinishDuel();

        KCharacter* pTargetCharacter = NULL;
        if (IS_PLAYER(dwOldTargetID))
            pTargetCharacter = (KCharacter*)g_pSO3World->m_PlayerSet.GetObj(dwOldTargetID);
        else
            pTargetCharacter = (KCharacter*)g_pSO3World->m_NpcSet.GetObj(dwOldTargetID);

        KGLOG_PROCESS_ERROR(pTargetCharacter);

        g_pGameWorldRepresentHandler->OnCharacterUpdateRelation(pTargetCharacter);
    }
    else if (eOldState == pksDueling && eState == pksIdle)
    {
        KUIEventUpdateRelation UIEventUpdateRelation;
        UIEventUpdateRelation.dwCharacterID = dwOldTargetID;
        g_pGameWorldUIHandler->OnUpdateRelation(UIEventUpdateRelation);
        g_pGameWorldUIHandler->OnFinishDuel();

        KCharacter* pTargetCharacter = NULL;
        if (IS_PLAYER(dwOldTargetID))
            pTargetCharacter = (KCharacter*)g_pSO3World->m_PlayerSet.GetObj(dwOldTargetID);
        else
            pTargetCharacter = (KCharacter*)g_pSO3World->m_NpcSet.GetObj(dwOldTargetID);

        KGLOG_PROCESS_ERROR(pTargetCharacter);

        g_pGameWorldRepresentHandler->OnCharacterUpdateRelation(pTargetCharacter);
    }
    else if (eState == pksStartSlay)
    {
        KUIEventSlayInfo UIParam;
        UIParam.dwPlayerID = m_pPlayer->m_dwID;
        UIParam.nSeconds   = g_pSO3World->m_Settings.m_ConstList.nWaitSlayTime;
        g_pGameWorldUIHandler->OnApplySlay(UIParam);
    }
    else if (eState == pksSlaying)
    {
        KUIEventSlayInfo UIParam;
        UIParam.dwPlayerID = m_pPlayer->m_dwID;
        g_pGameWorldUIHandler->OnStartSlay(UIParam);
        g_pGameWorldUIHandler->OnUpdateAllRelation();
        g_pGameWorldRepresentHandler->OnCharacterUpdateAllRelation();
    }
    else if (eState == pksExitSlay)
    {
        KUIEventSlayInfo UIParam;
        UIParam.dwPlayerID = m_pPlayer->m_dwID;
        UIParam.nSeconds   = g_pSO3World->m_Settings.m_ConstList.nCloseSlayTime;

        g_pGameWorldUIHandler->OnCloseSlay(UIParam);

        if (eOldState == pksIdle) // A登录游戏,B的状态是pksExitSlay,在A的客户端就会出现此情况  
        {
            KUIEventUpdateRelation UIEventUpdateRelation;
            UIEventUpdateRelation.dwCharacterID = m_pPlayer->m_dwID;
            g_pGameWorldUIHandler->OnUpdateRelation(UIEventUpdateRelation);
            g_pGameWorldRepresentHandler->OnCharacterUpdateRelation(m_pPlayer);
        }
    }
    else if ((eOldState == pksSlaying || eOldState == pksExitSlay)&& eState == pksIdle)
    {
        KUIEventSlayInfo UIParam;
        UIParam.dwPlayerID = m_pPlayer->m_dwID;
        g_pGameWorldUIHandler->OnSlayClosed(UIParam);
        g_pGameWorldUIHandler->OnUpdateAllRelation();
        g_pGameWorldRepresentHandler->OnCharacterUpdateAllRelation();
    }
#endif

Exit0:
	return;
}

BOOL KPKController::IsDuel()
{
    return (
        m_eState == pksWaitForDuelRespond || m_eState == pksConfirmDuel || 
        m_eState == pksPrepareDuel || m_eState == pksDueling || m_eState == pksDuelOutOfRange
    );
}

BOOL KPKController::IsOnSlay()
{
    return (m_eState == pksSlaying || m_eState == pksExitSlay);
}

PK_STATE KPKController::GetPKState()
{
    return m_eState;
}

#ifdef _SERVER
int KPKController::GetCloseSlayLeftTime()
{
    int nResult = -1;
    
    KG_PROCESS_ERROR(m_eState == pksExitSlay);

    nResult = (m_nTimer - g_pSO3World->m_nGameLoop) / GAME_FPS;
    KG_PROCESS_ERROR(nResult >= 0);

Exit0:
    return nResult;
}

void KPKController::Activate()
{
	switch(m_eState)
	{
    case pksIdle:
        break;

	case pksWaitForDuelRespond:
    case pksConfirmDuel:
        OnWaitConfirmDuel();
		break;

	case pksPrepareDuel:
		OnPrepareDuel();
		break;

	case pksDueling:
		OnDueling();
		break;

	case pksDuelOutOfRange:
		OnDuelOutOfRange();
		break;

	case pksStartSlay:
		OnStartSlay();
		break;

	case pksSlaying:
		OnSlaying();
		break;

    case pksExitSlay:
        OnExitSlay();
        break;

	default:
		break;
	}
}

void KPKController::SetDuelPostion(DWORD dwMapID, int nX, int nY, int nZ)
{
    m_dwMapID       = dwMapID;
    m_nDuelCenterX  = nX;
    m_nDuelCenterY  = nY;
    m_nDuelCenterZ  = nZ;
}

PK_RESULT_CODE KPKController::ApplyDuel(DWORD dwTargetID)
{
    PK_RESULT_CODE	nResult     = pkrcFailed;
    int             nRetCode    = false;
    KPlayer*		pTarget     = NULL;
    int             nXA         = 0;
    int             nYA         = 0;
    int				nZA	        = 0;
    int             nXB         = 0;
    int             nYB         = 0;
    int				nZB	        = 0;
    int             nX          = 0;
    int             nY          = 0;
    int             nZ          = 0;

    assert(m_pPlayer);
    KGLOG_PROCESS_ERROR(dwTargetID != m_pPlayer->m_dwID);

    pTarget = g_pSO3World->m_PlayerSet.GetObj(dwTargetID);
    KG_PROCESS_ERROR_RET_CODE(pTarget, pkrcApplyDuelFailed);

    KG_PROCESS_ERROR_RET_CODE(pTarget->m_pScene, pkrcApplyDuelFailed);

    nRetCode = CanApplyDuel(pTarget);
    KG_PROCESS_ERROR_RET_CODE(nRetCode == pkrcSuccess, (PK_RESULT_CODE)nRetCode);

    nRetCode = m_pPlayer->GetAbsoluteCoordinate(&nXA, &nYA, &nZA);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = pTarget->GetAbsoluteCoordinate(&nXB, &nYB, &nZB);
    KGLOG_PROCESS_ERROR(nRetCode);

    nX = (nXA + nXB) / 2;
    nY = (nYA + nYB) / 2;
    nZ = (nZA + nZB) / 2;

    SetDuelPostion(pTarget->m_pScene->m_dwMapID, nX, nY, nZ);
    pTarget->m_PK.SetDuelPostion(pTarget->m_pScene->m_dwMapID, nX, nY, nZ);

    nRetCode = CreateDuelFlag(nX, nY, nZ);
    KGLOG_PROCESS_ERROR(nRetCode);

    SetState(pksWaitForDuelRespond, g_pSO3World->m_nGameLoop + MAX_WAIT_DUEL_RESPOND_TIME, dwTargetID);
    pTarget->m_PK.SetState(pksConfirmDuel, g_pSO3World->m_nGameLoop + MAX_WAIT_DUEL_RESPOND_TIME, m_pPlayer->m_dwID);

    g_PlayerServer.DoSyncPKState(m_pPlayer->m_nConnIndex, m_pPlayer);
    g_PlayerServer.DoSyncPKState(pTarget->m_nConnIndex, pTarget);

    nResult = pkrcSuccess;
Exit0:
    return nResult;
}

PK_RESULT_CODE KPKController::RefuseDuel()
{	
    PK_RESULT_CODE  nResult     = pkrcFailed;

    KGLOG_PROCESS_ERROR(m_eState == pksConfirmDuel);
    
	CancelDuel(true);

    nResult = pkrcSuccess;
Exit0:
    return nResult;
}

PK_RESULT_CODE KPKController::AcceptDuel()
{
	PK_RESULT_CODE  nResult         = pkrcFailed;
    BOOL            bRetCode        = false;
    KPlayer*        pTarget         = GetTargetPlayer();
    int             nDualEndFrame   = g_pSO3World->m_nGameLoop + MAX_DUEL_BEGIN_TIME;

	assert(m_pPlayer); 
	KG_PROCESS_ERROR(m_eState == pksConfirmDuel);
	KG_PROCESS_ERROR(m_pPlayer->m_eMoveState != cmsOnDeath);

	KGLOG_PROCESS_ERROR(pTarget);
	KG_PROCESS_ERROR(pTarget->m_PK.m_eState == pksWaitForDuelRespond);
	KG_PROCESS_ERROR(pTarget->m_eMoveState != cmsOnDeath);

    bRetCode = CheckDuelDistance(MAX_DUELING_RANGE);
	KG_PROCESS_ERROR_RET_CODE(bRetCode, pkrcOutofRange);

	SetState(pksPrepareDuel, nDualEndFrame, pTarget->m_dwID);
    g_PlayerServer.DoSyncPKState(m_pPlayer->m_nConnIndex, m_pPlayer, nDualEndFrame);
    m_dwDuelLoser = ERROR_ID;

	pTarget->m_PK.SetState(pksPrepareDuel, nDualEndFrame, m_pPlayer->m_dwID);
    g_PlayerServer.DoSyncPKState(pTarget->m_nConnIndex, pTarget, nDualEndFrame);

	nResult = pkrcSuccess;
Exit0:
    if (nResult == pkrcSuccess)
    {
        CallDuelAcceptScript();
    }
    else
	{
		CancelDuel();
	}

	return nResult;
}

PK_RESULT_CODE KPKController::LossDuel()
{
    PK_RESULT_CODE  nResult   = pkrcFailed;

    assert(m_pPlayer);
    KGLOG_PROCESS_ERROR(m_eState == pksDueling || m_eState == pksDuelOutOfRange);

    g_PlayerServer.DoWinDuelBroadcast(m_dwTargetID, m_pPlayer->m_dwID);

    CancelDuel();

    nResult = pkrcSuccess;
Exit0:
    return nResult;
}

PK_RESULT_CODE KPKController::ApplySlay()
{
	PK_RESULT_CODE  nResult     = pkrcFailed;
    PK_RESULT_CODE  nRetCode    = pkrcFailed;

	nRetCode = CanApplySlay();
	KG_PROCESS_ERROR_RET_CODE(nRetCode == pkrcSuccess, nRetCode);

	SetState(pksStartSlay, g_pSO3World->m_nGameLoop + g_pSO3World->m_Settings.m_ConstList.nWaitSlayTime * GAME_FPS);

    g_PlayerServer.DoBroadcastPKState(m_pPlayer);
    
	nResult = pkrcSuccess;
Exit0:
	return nResult;
}

PK_RESULT_CODE KPKController::CloseSlay()
{
	PK_RESULT_CODE  nResult = pkrcFailed;

    KGLOG_PROCESS_ERROR(m_eState == pksSlaying);

	SetState(pksExitSlay, g_pSO3World->m_nGameLoop + g_pSO3World->m_Settings.m_ConstList.nCloseSlayTime * GAME_FPS);
    
    g_PlayerServer.DoBroadcastPKState(m_pPlayer);

	nResult = pkrcSuccess;
Exit0:
	return nResult;
}

BOOL KPKController::CheckDuelDistance(int nDistance)
{
	BOOL    bResult     = false;
    BOOL    bRetCode    = false;
    int     nPlayerX    = 0;
    int     nPlayerY    = 0;
    int     nPlayerZ    = 0;

    assert(m_pPlayer);

    bRetCode = m_pPlayer->GetAbsoluteCoordinate(&nPlayerX, &nPlayerY, &nPlayerZ);
    KG_PROCESS_ERROR(bRetCode);

    nPlayerZ = ZPOINT_TO_XYPOINT(nPlayerZ);

    bRetCode = g_InRange(
        nPlayerX, nPlayerY, nPlayerZ, 
        m_nDuelCenterX, m_nDuelCenterY, ZPOINT_TO_XYPOINT(m_nDuelCenterZ), 
        nDistance
    );
	KG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
	return bResult;
}

void KPKController::CancelDuel(BOOL bRefuseDuelFlag /*= false*/)
{
    KPlayer* pTarget = NULL;

    assert(m_pPlayer);

    pTarget = GetTargetPlayer();

    if (pTarget)
    {
        m_pPlayer->m_BuffList.CleanBuffByCaster(pTarget->m_dwID);

        m_pPlayer->ClearBullet();
        
        m_pPlayer->m_SimpThreatList.ClearThreat(pTarget);

        pTarget->m_BuffList.CleanBuffByCaster(m_pPlayer->m_dwID);

        pTarget->ClearBullet();
    
        pTarget->m_SimpThreatList.ClearThreat(m_pPlayer);
    }
    
    SetState(pksIdle, g_pSO3World->m_nGameLoop);
    DeleteDuelFlag();
    g_PlayerServer.DoSyncPKState(m_pPlayer->m_nConnIndex, m_pPlayer, 0, bRefuseDuelFlag);
    
    if (pTarget)
    {
        pTarget->m_PK.SetState(pksIdle, g_pSO3World->m_nGameLoop);
        pTarget->m_PK.DeleteDuelFlag();
        g_PlayerServer.DoSyncPKState(pTarget->m_nConnIndex, pTarget, 0, bRefuseDuelFlag);
    }
    
    return;
}

BOOL KPKController::CreateDuelFlag(int nX, int nY, int nZ)
{
    BOOL        bResult         = false;
    BOOL        bRetCode        = false;
    KDoodad*    pDoodad         = NULL;
	BOOL        bDoodadRegFlag  = false;
	BOOL        bDoodadInitFlag = false;
    KPlayer*    pTargetPlayer   = NULL;

    pDoodad = KMemory::New<KDoodad>();
    KGLOG_PROCESS_ERROR(pDoodad);

	bRetCode = g_pSO3World->m_DoodadSet.Register(pDoodad, ERROR_ID);
	KGLOG_PROCESS_ERROR(bRetCode);
	bDoodadRegFlag = true;

    bRetCode = pDoodad->Init();
    KGLOG_PROCESS_ERROR(bRetCode);
    bDoodadInitFlag = true;

    bRetCode = g_pSO3World->m_Settings.m_DoodadTemplateList.LoadFromTemplate(
        g_pSO3World->m_Settings.m_ConstList.nPKFlagDoodadTemplateID, pDoodad
    );
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = g_pSO3World->AddDoodad(pDoodad, m_pPlayer->m_pScene, nX, nY, nZ);
    KGLOG_PROCESS_ERROR(bRetCode);

    m_dwFlagDoodadID = pDoodad->m_dwID;
    
    bResult = true;
Exit0:
    if (!bResult)
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
    return bResult;
}

void KPKController::DeleteDuelFlag()
{
    KDoodad* pDoodad = NULL;

    KG_PROCESS_ERROR(m_dwFlagDoodadID != ERROR_ID);	

    pDoodad = g_pSO3World->m_DoodadSet.GetObj(m_dwFlagDoodadID);
    KG_PROCESS_ERROR(pDoodad);

    g_pSO3World->DelDoodad(pDoodad);

Exit0:
    m_dwFlagDoodadID    = ERROR_ID;
    return;
}

void KPKController::OnWaitConfirmDuel()
{
    BOOL     bResult    = false;
    BOOL     bRetCode   = false;
    KPlayer* pTarget    = GetTargetPlayer();

    assert(m_pPlayer);
        
    KG_PROCESS_ERROR(g_pSO3World->m_nGameLoop < m_nTimer);
    KG_PROCESS_ERROR(pTarget != NULL);
    KG_PROCESS_ERROR(pTarget->m_dwID == m_dwTargetID);
    KG_PROCESS_ERROR(m_pPlayer->m_pScene == pTarget->m_pScene);
    KG_PROCESS_ERROR(m_pPlayer->m_eMoveState != cmsOnDeath);
    KG_PROCESS_ERROR(!m_pPlayer->m_bFightState);

    KG_PROCESS_ERROR(
        m_pPlayer->m_dwTeamID == ERROR_ID || m_pPlayer->m_dwTeamID != pTarget->m_dwTeamID
    );
    
    bRetCode = m_pPlayer->IsFoe(pTarget->m_dwID);
    KG_PROCESS_ERROR(!bRetCode);

    bResult = true;
Exit0:
    if (!bResult)
    {
        CancelDuel();
    }
    return;
}

void KPKController::OnPrepareDuel()
{
    BOOL     bResult        = false;
    BOOL     bRetCode       = false;
    KPlayer* pTarget        = GetTargetPlayer();

    assert(m_pPlayer);
    
    if (
        pTarget == NULL || pTarget->m_dwID != m_dwTargetID || m_pPlayer->m_pScene != pTarget->m_pScene || 
        m_pPlayer->m_eMoveState == cmsOnDeath || !CheckDuelDistance(MAX_DUELING_RANGE) || m_pPlayer->m_bFightState
    )
    {
        if (pTarget == NULL)
        {
            CallDuelEndScript(pkecOffline);
        }
        else if (m_pPlayer->m_pScene != NULL && m_pPlayer->m_pScene->m_dwMapID != m_dwMapID)
        {
            CallDuelEndScript(pkecSwitchMap);
        }
        else if (m_pPlayer->m_eMoveState == cmsOnDeath)
        {
            CallDuelEndScript(pkecUnexpectedDie);
        }

        goto Exit0;
    }
    
    bRetCode = m_pPlayer->IsFoe(pTarget->m_dwID);
    KG_PROCESS_ERROR(!bRetCode);

    if (
        pTarget != NULL && m_pPlayer->m_dwTeamID != ERROR_ID && pTarget->m_dwTeamID != ERROR_ID &&
        m_pPlayer->m_dwTeamID == pTarget->m_dwTeamID
    )
    {
        goto Exit0;
    }

    if (g_pSO3World->m_nGameLoop >= m_nTimer)
    {
        KGFellowship*   pFellowship  = NULL;
        KGFellowship*   pFellowship2 = NULL;
        
        SetState(pksDueling, g_pSO3World->m_nGameLoop, m_dwTargetID);
        g_PlayerServer.DoSyncPKState(m_pPlayer->m_nConnIndex, m_pPlayer);

        pFellowship = g_pSO3World->m_FellowshipMgr.GetFellowship(m_pPlayer->m_dwID, m_dwTargetID);
        pFellowship2 = g_pSO3World->m_FellowshipMgr.GetFellowship(m_dwTargetID, m_pPlayer->m_dwID);

        if (
            pFellowship && pFellowship2 && pFellowship->m_BitFlag.nDuelDailyCount &&
            pFellowship->m_nAttraction < g_pSO3World->m_Settings.m_ConstList.nDuelAttractiveUpLimit
        )
        {
            g_pSO3World->m_FellowshipMgr.AddFellowshipAttraction(
                m_pPlayer->m_dwID, m_dwTargetID, 
                g_pSO3World->m_Settings.m_ConstList.nDuelAttractiveAdd, 
                false
            );
            pFellowship->m_BitFlag.nDuelDailyCount--;
        }

        if (pTarget->m_PK.GetState() == pksDueling)
            CallDuelBeginScript();
    }

    bResult = true;
Exit0:
    if (!bResult)
    {
        CancelDuel();
    }
    return;
}

void KPKController::OnDueling()
{
    BOOL        bResult     = false;
	BOOL        bRetCode    = false;
    KPlayer*    pTarget     = GetTargetPlayer();

	assert(m_pPlayer);
    
	if (pTarget == NULL || pTarget->m_dwID != m_dwTargetID || m_pPlayer->m_eMoveState == cmsOnDeath)
	{
        // 正常切磋是不会死的，这里死掉是因为其它原因导致的，所以不能算输，只能取消切磋
        if (pTarget == NULL)
        {
            CallDuelEndScript(pkecOffline);
        }
        else if (m_pPlayer->m_eMoveState == cmsOnDeath)
        {
            CallDuelEndScript(pkecUnexpectedDie);
        }

        goto Exit0;
	}
    
    if (
        pTarget != NULL && m_pPlayer->m_dwTeamID != ERROR_ID && pTarget->m_dwTeamID != ERROR_ID &&
        m_pPlayer->m_dwTeamID == pTarget->m_dwTeamID
    )
    {
        goto Exit0;
    }

    if (m_pPlayer->m_pScene == NULL || m_pPlayer->m_pScene->m_dwMapID != m_dwMapID)
    {	
        if (m_pPlayer->m_pScene != NULL && m_pPlayer->m_pScene->m_dwMapID != m_dwMapID)
        {
            CallDuelEndScript(pkecSwitchMap);
        }

        LossDuel();
        goto Exit1;
    }
    
    bRetCode = CheckDuelDistance(MAX_DUELING_RANGE);
    if (!bRetCode)
    {
        SetState(pksDuelOutOfRange, g_pSO3World->m_nGameLoop + MAX_DULE_OUTRANGE_TIME, m_dwTargetID);
        g_PlayerServer.DoSyncPKState(m_pPlayer->m_nConnIndex, m_pPlayer);
    }
    
    bRetCode = m_pPlayer->IsFoe(pTarget->m_dwID);
    KG_PROCESS_ERROR(!bRetCode);

Exit1:
    bResult = true;
Exit0:
    if (!bResult)
    {
        CancelDuel();
    }
	return;
}

void KPKController::OnDuelOutOfRange()
{
    BOOL        bResult     = false;
	BOOL        bRetCode    = false;
    KPlayer*    pTarget     = GetTargetPlayer();

    assert(m_pPlayer);

    if (pTarget == NULL || pTarget->m_dwID != m_dwTargetID || m_pPlayer->m_eMoveState == cmsOnDeath)
    {
        // 正常切磋是不会死的，这里死掉是因为其它原因导致的，所以不能算输，只能取消切磋
        if (pTarget == NULL)
        {
            CallDuelEndScript(pkecOffline);
        }
        else if (m_pPlayer->m_eMoveState == cmsOnDeath)
        {
            CallDuelEndScript(pkecUnexpectedDie);
        }

        goto Exit0;
    }
    
    if (
        pTarget != NULL && m_pPlayer->m_dwTeamID != ERROR_ID && pTarget->m_dwTeamID != ERROR_ID &&
        m_pPlayer->m_dwTeamID == pTarget->m_dwTeamID
    )
    {
        goto Exit0;
    }

    if (g_pSO3World->m_nGameLoop >= m_nTimer || m_pPlayer->m_pScene == NULL || m_pPlayer->m_pScene->m_dwMapID != m_dwMapID)
    {	
        if (g_pSO3World->m_nGameLoop >= m_nTimer)
        {
            CallDuelEndScript(pkecOutofRangeTimeUp);
        }
        else if (m_pPlayer->m_pScene != NULL && m_pPlayer->m_pScene->m_dwMapID != m_dwMapID)
        {
            CallDuelEndScript(pkecSwitchMap);
        }

        LossDuel();
        goto Exit1;
    }

	bRetCode = CheckDuelDistance(MAX_DUELING_RANGE);
    if (bRetCode)
    {
        SetState(pksDueling, g_pSO3World->m_nGameLoop, m_dwTargetID);
        g_PlayerServer.DoSyncPKState(m_pPlayer->m_nConnIndex, m_pPlayer);
    }

Exit1:
    bResult = true;
Exit0:
    if (!bResult)
    {
        CancelDuel();
    }
	return;
}

void KPKController::OnStartSlay()
{
    if (g_pSO3World->m_nGameLoop >= m_nTimer)
    {
        int nCDInterval = g_pSO3World->m_Settings.m_CoolDownList.GetCoolDownValue(g_pSO3World->m_Settings.m_ConstList.nSlayCDIndex);

        m_pPlayer->m_TimerList.ResetTimer(g_pSO3World->m_Settings.m_ConstList.nSlayCDIndex, nCDInterval);

        SetState(pksSlaying);
        g_PlayerServer.DoBroadcastPKState(m_pPlayer);
    }
    return;
}

void KPKController::OnSlaying()
{
    return;
}

void KPKController::OnExitSlay()
{
    assert(m_pPlayer);
    if (g_pSO3World->m_nGameLoop >= m_nTimer)
    {
        SetState(pksIdle, g_pSO3World->m_nGameLoop);
        g_PlayerServer.DoBroadcastPKState(m_pPlayer);
    }
    return;
}

BOOL KPKController::CallDuelAcceptScript()
{
    BOOL        bResult     = false;
    BOOL        bRetCode    = false;
    int         nTopIndex   = 0;
    const char* pszFuncName = "OnDuelAccept";

    assert(m_pPlayer);

    bRetCode = g_pSO3World->m_ScriptCenter.IsScriptExist(PK_SCRIPT);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = g_pSO3World->m_ScriptCenter.IsFuncExist(PK_SCRIPT, pszFuncName);
    KG_PROCESS_ERROR(bRetCode);

    g_pSO3World->m_ScriptCenter.SafeCallBegin(&nTopIndex);

    g_pSO3World->m_ScriptCenter.PushValueToStack(m_pPlayer->m_dwID);

    g_pSO3World->m_ScriptCenter.PushValueToStack(m_dwTargetID);

    g_pSO3World->m_ScriptCenter.CallFunction(PK_SCRIPT, pszFuncName, 0);

    g_pSO3World->m_ScriptCenter.SafeCallEnd(nTopIndex);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPKController::CallDuelBeginScript()
{
    BOOL        bResult     = false;
    BOOL        bRetCode    = false;
    int         nTopIndex   = 0;
    const char* pszFuncName = "OnDuelBegin";

    assert(m_pPlayer);

    bRetCode = g_pSO3World->m_ScriptCenter.IsScriptExist(PK_SCRIPT);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = g_pSO3World->m_ScriptCenter.IsFuncExist(PK_SCRIPT, pszFuncName);
    KG_PROCESS_ERROR(bRetCode);

    g_pSO3World->m_ScriptCenter.SafeCallBegin(&nTopIndex);

    g_pSO3World->m_ScriptCenter.PushValueToStack(m_pPlayer->m_dwID);

    g_pSO3World->m_ScriptCenter.PushValueToStack(m_dwTargetID);

    g_pSO3World->m_ScriptCenter.CallFunction(PK_SCRIPT, pszFuncName, 0);

    g_pSO3World->m_ScriptCenter.SafeCallEnd(nTopIndex);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPKController::CallDuelEndScript(int nCode)
{
    BOOL        bResult     = false;
    BOOL        bRetCode    = false;
    int         nTopIndex   = 0;
    const char* pszFuncName = "OnDuelEnd";
    DWORD       dwPlayerID1 = m_pPlayer->m_dwID;
    DWORD       dwPlayerID2 = m_dwTargetID;

    if (nCode == pkecOffline)
    {
        dwPlayerID1 = ERROR_ID;
        dwPlayerID2 = m_pPlayer->m_dwID;
    }

    bRetCode = g_pSO3World->m_ScriptCenter.IsScriptExist(PK_SCRIPT);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = g_pSO3World->m_ScriptCenter.IsFuncExist(PK_SCRIPT, pszFuncName);
    KG_PROCESS_ERROR(bRetCode);

    g_pSO3World->m_ScriptCenter.SafeCallBegin(&nTopIndex);

    g_pSO3World->m_ScriptCenter.PushValueToStack(dwPlayerID1);

    g_pSO3World->m_ScriptCenter.PushValueToStack(dwPlayerID2);

    g_pSO3World->m_ScriptCenter.PushValueToStack(nCode);

    g_pSO3World->m_ScriptCenter.CallFunction(PK_SCRIPT, pszFuncName, 0);

    g_pSO3World->m_ScriptCenter.SafeCallEnd(nTopIndex);

    bResult = true;
Exit0:
    return bResult;
}

#endif
