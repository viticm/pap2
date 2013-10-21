#include "stdafx.h"

#include "KAIControl.h"
#include "KCharacter.h"
#include "KSO3World.h"
#include "KNpc.h"
#include "KNpcTeam.h"
#include "KRegion.h"
#include "KAI_Player.h"
#include "KAI_Wood.h"

KAIControl::KAIControl()
{
	m_pAIBase = NULL;
	m_bRunning = FALSE;
	memset(m_AIBase, 0, sizeof(m_AIBase));
}

KAIControl::~KAIControl()
{
	
}

BOOL KAIControl::Init(AI_TYPE eAIType, KCharacter* pSelf)
{
	BOOL bRetCode = FALSE;

	KGLOG_PROCESS_ERROR(eAIType > aitInvalid && eAIType < aitTotal);

	m_pAIBase = (KAIBase*)(&m_AIBase[0]);
    bRetCode = SetAIType(eAIType);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = m_pAIBase->Init(pSelf);
	KGLOG_PROCESS_ERROR(bRetCode);

	m_pAIBase->m_eAIType = eAIType;

    //m_PathFinder.Init()

	m_bRunning = false;
Exit0:
	return bRetCode;
}

BOOL KAIControl::UnInit()
{
	BOOL bRetCode = FALSE;

	KG_PROCESS_ERROR(m_pAIBase);

	bRetCode = m_pAIBase->UnInit();
	KGLOG_CHECK_ERROR(bRetCode);

	m_pAIBase->~KAIBase();
	m_pAIBase = NULL;

	bRetCode = true;
Exit0:
	return bRetCode;
}

BOOL KAIControl::SetAIType(AI_TYPE eAIType)
{
	BOOL bRetCode = false;
	KAIBase* pResult = 0;
	
	switch(eAIType)
	{
	case aitBase:
		pResult = new(m_pAIBase) KAIBase();
		break;
	case aitPlayer:
		pResult = new(m_pAIBase) KAI_Player();
	    break;
	case aitWood:
		pResult = new(m_pAIBase) KAI_Wood();
		break;
	default:
		KGLogPrintf(KGLOG_ERR, "[AI] AI type error(%d), default to aitBase", eAIType);
		eAIType = aitBase;
		pResult = new(m_pAIBase) KAIBase();
	    break;
	}
	
	// 临时代码
	KGLOG_PROCESS_ERROR(pResult);

	m_pAIBase->m_eAIType = eAIType;

	bRetCode = true;
Exit0:
	return bRetCode;
}

AI_TYPE KAIControl::GetAIType()
{
	AI_TYPE eResult = aitInvalid;

	KGLOG_PROCESS_ERROR(m_pAIBase);
	eResult = m_pAIBase->GetAIType();
Exit0:
	return eResult;
}

BOOL KAIControl::SetMainState(KAI_STATE eAIMainState)
{
    BOOL bResult = false;

	KGLOG_PROCESS_ERROR(m_pAIBase);
	KG_PROCESS_ERROR(eAIMainState >= aisIdle && eAIMainState <= aisFollow);

	m_pAIBase->m_eAIMainState = eAIMainState;

#if defined(_SERVER)
	//激活Npc巡逻的时候同时也好激活场景
	if (m_pAIBase->m_eAIMainState == aisPatrol 
		&& m_pAIBase->m_pSelf 
		&& m_pAIBase->m_pSelf->m_pRegion)
	{
		m_pAIBase->m_pSelf->m_pRegion->SetActive();
	}
#endif

    bResult = true;
Exit0:
	return bResult;
}

KAI_STATE KAIControl::GetMainState()
{
	KGLOG_PROCESS_ERROR(m_pAIBase);

	return m_pAIBase->m_eAIMainState;
Exit0:
	return aisInvalid;
}

void KAIControl::SetOriginPoint(int nX, int nY, int nZ, int nDir)
{
	KGLOG_PROCESS_ERROR(m_pAIBase);

	m_pAIBase->SetOriginPoint(nX, nY, nZ, nDir);

Exit0:
	return ;
}

BOOL KAIControl::GetOriginPoint(int* pnRetX, int* pnRetY, int* pnRetZ, int* pnRetDir)
{
	BOOL bRetCode = false;
	KGLOG_PROCESS_ERROR(m_pAIBase);

    ASSERT(pnRetX);
    ASSERT(pnRetY);
    ASSERT(pnRetZ);
    ASSERT(pnRetDir);

	bRetCode = m_pAIBase->GetOriginPoint(pnRetX, pnRetY, pnRetZ, pnRetDir);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = true;
Exit0:
	return bRetCode;
}

int KAIControl::GetAIEventNextFrame(int nIndex)
{
	int nResult = -1;
	KGLOG_PROCESS_ERROR(m_pAIBase);
	KGLOG_PROCESS_ERROR(nIndex >=0 && nIndex < MAX_AI_EVENT);

	nResult = m_pAIBase->m_nAIEventTimer[nIndex];
Exit0:
	return nResult;
}

#if defined(_SERVER)
BOOL KAIControl::SetPatrolPath(int nPatrolPathID, int nNextPointIndex)
{
	BOOL bRetCode = false;
	KNpc* pNpc = NULL;

	KGLOG_PROCESS_ERROR(nPatrolPathID > 0);
	KGLOG_PROCESS_ERROR(m_pAIBase);
	KGLOG_PROCESS_ERROR(m_pAIBase->m_pSelf);
    
	KGLOG_PROCESS_ERROR(IS_NPC(m_pAIBase->m_pSelf->m_dwID));
	
	bRetCode = m_pAIBase->SetPatrolPath(nPatrolPathID, nNextPointIndex);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = true;
Exit0:
	return bRetCode;
}

BOOL KAIControl::SetNpcTeam(KNpcTeam* pNpcTeam)
{
	BOOL bRetCode = false;
	KGLOG_PROCESS_ERROR(m_pAIBase);

	bRetCode = m_pAIBase->SetNpcTeam(pNpcTeam);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = true;
Exit0:
	return bRetCode;
}

KNpcTeam* KAIControl::GetNpcTeam()
{
	KGLOG_PROCESS_ERROR(m_pAIBase);

	return m_pAIBase->m_pNpcTeam;
Exit0:
	return NULL;;
}
#endif //_SERVER

KAIBase* KAIControl::GetAIBase()
{
	return m_pAIBase;
}

BOOL KAIControl::SetAIParam(int nParamTemplateID)
{
	BOOL                bRetCode            = false;
	KAIParamTemplate*   pAIParamTemplate    = NULL;
	
	KGLOG_PROCESS_ERROR(m_pAIBase);
	KGLOG_PROCESS_ERROR(m_pAIBase->m_pSelf);
	KGLOG_PROCESS_ERROR(nParamTemplateID >= 0);

	pAIParamTemplate = g_pSO3World->m_Settings.m_AIParamTemplateList.GetTemplate(nParamTemplateID);
	KGLOG_PROCESS_ERROR(pAIParamTemplate);

	m_pAIBase->m_pAIParam = pAIParamTemplate;

	m_pAIBase->LoadParamFromTemplate();

	bRetCode = true;
Exit0:
	return bRetCode;
}

KAIParamTemplate* KAIControl::GetAIParam()
{
	KG_PROCESS_ERROR(m_AIBase);

	return m_pAIBase->GetAIParam();
Exit0:
	return NULL;
}

KAI_STATE KAIControl::GetAIState()
{
	KG_PROCESS_ERROR(m_pAIBase);

	return m_pAIBase->m_eAIState;
Exit0:
	return aisInvalid;
}

void KAIControl::Activate()
{
	KG_PROCESS_ERROR(m_bRunning);
	KG_PROCESS_ERROR(m_pAIBase);
	KG_PROCESS_ERROR(m_pAIBase->m_pSelf);

	m_pAIBase->Activate();
Exit0:
	return;
}

void KAIControl::Stop()
{
	m_bRunning = false;
}

void KAIControl::Restart()
{
	KGLOG_PROCESS_ERROR(m_pAIBase);
    KG_PROCESS_ERROR(m_pAIBase->m_eAIMainState != aisInvalid);
	m_pAIBase->m_nNextPartolIndex = 0;

    DoIdle(16);

    m_bRunning = true;

    return;
Exit0:
	return;
}

BOOL KAIControl::IsRunning()
{
    return m_bRunning;
}


// ------------- 状态切换函数 -----------------------------
void KAIControl::DoIdle(int nIdleFrame)
{
    BOOL        bRetCode    = false;
    KNpcTeam*   pNpcTeam    = NULL;
    KNpc*       pNpc        = NULL;
    int         nNpcCount   = 0;

	KGLOG_PROCESS_ERROR(m_pAIBase);

	//当KAIControl::DoIdle不传参数时,认为是切换成永久站立
	if (nIdleFrame == DEFAULT_IDLE_FRAME)
	{
		m_pAIBase->m_eAIMainState = aisIdle;
	}
	
	m_pAIBase->DoIdle(nIdleFrame);

#ifdef _SERVER
    pNpcTeam = GetNpcTeam();
    KG_PROCESS_SUCCESS(!pNpcTeam);

    nNpcCount = pNpcTeam->GetMemberCount();
    for (int i = 0; i < nNpcCount; i++)
    {
        pNpc = pNpcTeam->GetMember(i);

        ASSERT(pNpc->m_dwID != ERROR_ID);

        if (pNpc->m_dwID == m_pAIBase->m_pSelf->m_dwID)
            continue;

        bRetCode = pNpc->m_AIController.IsRunning();
        if (!bRetCode)
            continue;

        pNpc->m_AIController.m_pAIBase->DoIdle(nIdleFrame);
    }
#endif // _SERVER

Exit1:
Exit0:
	return;
}

void KAIControl::DoWander()
{
	KGLOG_PROCESS_ERROR(m_pAIBase);
	m_pAIBase->m_eAIMainState = aisWander;
	m_pAIBase->DoWander();
Exit0:
	return;
}

void KAIControl::DoPatrol()
{
	KGLOG_PROCESS_ERROR(m_pAIBase);
	m_pAIBase->m_eAIMainState = aisPatrol;
	m_pAIBase->DoPatrol();
Exit0:
	return;
}

void KAIControl::DoAlert()
{
    KGLOG_PROCESS_ERROR(m_pAIBase);
	m_pAIBase->DoAlert();
Exit0:
	return;
}

void KAIControl::DoFollow(KCharacter* pTarget)
{
	KGLOG_PROCESS_ERROR(m_pAIBase);
	if (pTarget)
	{
		m_pAIBase->m_FollowData.dwFollowTargetID = pTarget->m_dwID;
		m_pAIBase->m_eAIMainState = aisFollow;
		m_pAIBase->DoFollow();
	}
	else if (m_pAIBase->m_eAIState == aisFollow)
	{
		m_pAIBase->m_FollowData.dwFollowTargetID = 0;
		m_pAIBase->m_eAIMainState = aisIdle;
		m_pAIBase->DoIdle();
	}
Exit0:
	return;
}

void KAIControl::DoPursuit()
{
	KGLOG_PROCESS_ERROR(m_pAIBase);
	m_pAIBase->DoPursuit();
Exit0:
	return;
}

void KAIControl::DoKeepAway()
{
	KGLOG_PROCESS_ERROR(m_pAIBase);
	m_pAIBase->DoKeepAway();
Exit0:
	return;
}

void KAIControl::DoEscape(int nEscapeRange, int nEscapeFrame)
{
	KGLOG_PROCESS_ERROR(m_pAIBase);
	KGLOG_PROCESS_ERROR(m_pAIBase->m_pAIParam);
	KGLOG_PROCESS_ERROR(m_pAIBase->m_pSelf);
	KGLOG_PROCESS_ERROR(nEscapeRange >= DEFAULT_ESCAPE_RANGE);
	KGLOG_PROCESS_ERROR(nEscapeFrame >= DEFAULT_ESCAPE_FRAME);

#ifdef _SERVER
	//保存逃跑点的坐标
	m_pAIBase->SaveReturnPoint();
	m_pAIBase->m_pSelf->m_ThreatList.UpdateKeepThreatField();
#endif //_SERVER

	//设置逃跑的开始事件，不能在KAIBase中设，因为可能在逃跑中反复调用KAIBase::DoEscape
	if (nEscapeRange == DEFAULT_ESCAPE_RANGE)
		m_pAIBase->m_EscapeData.nEscapeRange = g_pSO3World->m_nGameLoop + m_pAIBase->m_pAIParam->nEscapeRange;
	else
		m_pAIBase->m_EscapeData.nEscapeRange = g_pSO3World->m_nGameLoop + nEscapeRange;

	if (nEscapeFrame == DEFAULT_ESCAPE_FRAME)
		m_pAIBase->m_EscapeData.nEscapeFrame = g_pSO3World->m_nGameLoop + m_pAIBase->m_pAIParam->nEscapeTime;
	else
		m_pAIBase->m_EscapeData.nEscapeFrame = g_pSO3World->m_nGameLoop + nEscapeFrame;

	m_pAIBase->DoEscape();
Exit0:
	return;
}

void KAIControl::DoReturn()
{
	KGLOG_PROCESS_ERROR(m_pAIBase);
	m_pAIBase->DoReturn();
Exit0:
	return;
}

void KAIControl::DoWait()
{
	KGLOG_PROCESS_ERROR(m_pAIBase);
	m_pAIBase->DoWait();
Exit0:
	return;
}

BOOL KAIControl::OnTryMove(int nRelativeZ, BOOL bIsWater)
{
	BOOL bRetCode = false;

	KG_PROCESS_ERROR(m_pAIBase);
	bRetCode = m_pAIBase->OnTryMove(nRelativeZ, bIsWater);
	KG_PROCESS_ERROR(bRetCode);

	bRetCode = true;
Exit0:
	return bRetCode;
}

void KAIControl::OnAttacked(KCharacter *pAttacker, int nDamage)
{
	KG_PROCESS_ERROR(m_pAIBase);
	m_pAIBase->OnAttacked(pAttacker, nDamage);
Exit0:
	return;
}

void KAIControl::OnClearThreat(KCharacter *pCharacter)
{
	KG_PROCESS_ERROR(m_pAIBase);
	m_pAIBase->OnClearThreat(pCharacter);
Exit0:
	return;
}

void KAIControl::OnPathResult(BOOL bSucceed)
{	
	KG_PROCESS_ERROR(m_pAIBase);
	m_pAIBase->OnPathResult(bSucceed);
Exit0:
	return;
}

BOOL KAIControl::IsPositive()
{
	BOOL bResult = false;
	KG_PROCESS_ERROR(m_pAIBase);

	bResult = m_pAIBase->m_pAIParam->bIsPositive;
Exit0:
	return bResult;
}

int KAIControl::GetTurnToIdleFrame()
{
	int nResult = 0;
		
	KG_PROCESS_ERROR(m_pAIBase);
	KG_PROCESS_ERROR(m_pAIBase->m_eAIState == aisPursuit);

	nResult = m_pAIBase->m_PursuitData.nTurnToFightFrame;
Exit0:
	return nResult;
}



//---- AI命令 -----------------------------------------
#if defined(_SERVER)
BOOL KAIControl::FireAIEvent(AI_EVENT_CONDITION eAIEvent)
{
	BOOL bResult = false;
	KG_PROCESS_ERROR(m_pAIBase);

	m_pAIBase->FireAIEvent(eAIEvent);

	bResult = true;
Exit0:
	return bResult;
}

BOOL KAIControl::CallHelp(int nRange)
{
	BOOL bResult = false;
	KG_PROCESS_ERROR(m_pAIBase);

	bResult = m_pAIBase->CallHelp(nRange);
Exit0:
	return bResult;
}

BOOL KAIControl::SetCallHealer(DWORD dwNpcID)
{
	BOOL bResult = false;
	KG_PROCESS_ERROR(m_pAIBase);
	KG_PROCESS_ERROR(m_pAIBase->m_eAIState == aisPursuit);
	KG_PROCESS_ERROR(dwNpcID);

	m_pAIBase->m_PursuitData.dwCallByNpcID = dwNpcID;

	bResult = true;
Exit0:
	return bResult;
}
#endif //_SERVER


double g_GetRangeByLevelDiff(int nLevelDiff)
{
	double fRangePercent = 1.0;

	if (nLevelDiff > 8)
		nLevelDiff = 8;
	else if (nLevelDiff < -8)
		nLevelDiff = -8;

	fRangePercent -= 0.1 * (8 - nLevelDiff) / 2;

Exit0:
	return fRangePercent;
}

