#include "StdAfx.h"

#include "KMath.h"
#include "KSO3World.h"
#include "KSkill.h"
#include "KAISearchTactics.h"
#include "KThreatList.h"
#include "KPlayerClient.h"
#include "KCharacter.h"
#include "KPlayer.h"
#include "KNpc.h"

#define PATROL_INTERVAL		(GAME_FPS * 8)

// 根据一个点和一个极坐标算出一个相对的点
void KAIBase::GetAdjustPoint(int nSrcX, int nSrcY, int nDistance, int nDir, OUT int& nDstX, OUT int& nDstY)
{
	//保证角度在[0, DIRECTION_COUNT)
	while (nDir < 0)
	{
		nDir += DIRECTION_COUNT;
	}

	if (nDir >= DIRECTION_COUNT)
	{
		nDir = nDir % DIRECTION_COUNT;
	}
	
	nDstX = nSrcX + nDistance * g_nCos[nDir] / SIN_COS_NUMBER;
	nDstY = nSrcY + nDistance * g_nSin[nDir] / SIN_COS_NUMBER;
}

void KAIBase::Follow(void)
{
	BOOL        bRetCode        = FALSE;
	KCharacter* pTarget         = NULL;
    int         nCurrentState   = m_pSelf->m_eMoveState;
    DWORD       dwMoveCommandID = 0;

	KGLOG_PROCESS_ERROR(m_FollowData.dwFollowTargetID);
	
	if (IS_PLAYER(m_FollowData.dwFollowTargetID))
		pTarget = g_pSO3World->m_PlayerSet.GetObj(m_FollowData.dwFollowTargetID);
	else
		pTarget = g_pSO3World->m_NpcSet.GetObj(m_FollowData.dwFollowTargetID);

	KG_PROCESS_SUCCESS(pTarget == NULL);

	int nTargetPosX = pTarget->m_nX;
	int nTargetPosY = pTarget->m_nY;

	int nMoveDirection = g_GetDirection(m_pSelf->m_nX, m_pSelf->m_nY, nTargetPosX, nTargetPosY);

	int nDistance2 = g_GetDistance2(m_pSelf->m_nX, m_pSelf->m_nY, nTargetPosX, nTargetPosY);
	int nDistance = (int)sqrt((double)nDistance2);

	KG_PROCESS_SUCCESS(nDistance > AI_FOLLOW_RANGE);
	KG_PROCESS_SUCCESS(nDistance <= AI_FOLLOW_KEEPAWAY);

	GetAdjustPoint(nTargetPosX, nTargetPosY, AI_FOLLOW_KEEPAWAY, nMoveDirection + 128, nTargetPosX, nTargetPosY);
	//转向
	bRetCode = m_pSelf->Turn(nMoveDirection, true, true);
	KGLOG_CHECK_ERROR(bRetCode);

#if defined(_CLIENT)
	bRetCode = g_PlayerClient.DoCharacterTurn(nMoveDirection);
	KGLOG_CHECK_ERROR(bRetCode);
#endif //_CLIENT

	int nSpeed = pTarget->m_nVelocityXY;

	//移动
	if (nDistance > AI_FOLLOW_RUN)
	{
		bRetCode = m_pSelf->RunTo(nTargetPosX, nTargetPosY, TRUE);
        KGLOG_CHECK_ERROR(bRetCode);
        
        #if defined(_CLIENT)
        dwMoveCommandID = g_pSO3World->m_CommandRedoList.Record(mctRun, nTargetPosX, nTargetPosY);
        bRetCode = g_PlayerClient.DoCharacterRun(m_pSelf, nCurrentState, nTargetPosX, nTargetPosY, dwMoveCommandID);
        KGLOG_CHECK_ERROR(bRetCode);
        #endif //_CLIENT
	}
	else
	{
		bRetCode = m_pSelf->WalkTo(nTargetPosX, nTargetPosY, TRUE);
        KGLOG_CHECK_ERROR(bRetCode);

        #if defined(_CLIENT)
        dwMoveCommandID = g_pSO3World->m_CommandRedoList.Record(mctWalk, nTargetPosX, nTargetPosY);
        bRetCode = g_PlayerClient.DoCharacterWalk(m_pSelf, nCurrentState, nTargetPosX, nTargetPosY, dwMoveCommandID);
        KGLOG_CHECK_ERROR(bRetCode);
        #endif //_CLIENT
	}

Exit1:
	return;
Exit0:
	//失败,切到站立状态
	m_eAIMainState = aisIdle;
	DoIdle();
	return;
}

void KAIBase::ClearTarget(void)
{
    m_pSelf->m_SelectTarget.ClearTarget();
}

#if defined(_SERVER)
void KAIBase::SaveReturnPoint(void)
{
	ASSERT(m_pSelf);
	m_nReturnX = m_pSelf->m_nX;
	m_nReturnY = m_pSelf->m_nY;
	m_nReturnZ = 0;
	if (m_pSelf->m_pCell)
	{
		m_nReturnZ = m_pSelf->m_pCell->m_wHighLayer * POINT_PER_ALTITUDE;
	}
	
	//KGLogPrintf(KGLOG_DEBUG, "Save Return Point x:%d, y:%d\n", m_nReturnX, m_nReturnY);
}

void KAIBase::TurnToFight(void)
{
	BOOL bRetCode = FALSE;
	KCharacter* pTarget = NULL;
	KThreatNode* pThreatNode = NULL;
	KNpc* pNpc = NULL;
	int nIndex = 0;

	KGLOG_PROCESS_ERROR(m_pSelf);
	KG_PROCESS_ERROR(IS_NPC(m_pSelf->m_dwID));

	pNpc = (KNpc*)m_pSelf;

    if (!pNpc->IsInSpecialMoveLimit())
    {
        pNpc->Stop();
    }

	//取得第一仇恨
	pThreatNode = m_pSelf->m_ThreatList.GetFirstThreat();
	KG_PROCESS_ERROR(pThreatNode);

	//保存返回的点
	SaveReturnPoint();

	//更新仇恨范围
	m_pSelf->m_ThreatList.UpdateKeepThreatField();

	//重置计数器
	memset(m_nAIEventTimer, 0, sizeof(m_nAIEventTimer));

	//重置公共CD
	pNpc->m_nSkillCommomCD = 0;

	//重置技能CD计数器
	InitSkillCD();

	//-------- AI Event:TurnToFight --------------------------
	{
		KTarget rTarget;
		rTarget.ClearTarget();

		if (pThreatNode && pThreatNode->pCharacter)
		{
			rTarget.SetTarget(pThreatNode->pCharacter);
		}
		g_pSO3World->m_Settings.m_SmartDialogList.FireEvent((KNpc*)m_pSelf, ntatTurnToFight, rTarget);

		FireAIEvent(aecTurnToFight);
	}
	//--------------------------------------------------------

	//追击
	DoPursuit();

	//临时代码,呼救
	CallHelp(256);

	//在NpcTeam中就自动拷贝仇恨
	if (m_pNpcTeam)
	{
		pTarget = pThreatNode->pCharacter;
		KGLOG_PROCESS_ERROR(pTarget);

		int nMemberCount = m_pNpcTeam->GetMemberCount();
		for (nIndex = 0; nIndex < nMemberCount; nIndex++)
		{
			KNpc* pNpc = m_pNpcTeam->GetMember(nIndex);
			if (pNpc && pNpc != m_pSelf)
			{
				pNpc->m_ThreatList.ModifyThreat(pTarget, 0);
			}
		}
	}

	return;
Exit0:
	return;
}

void KAIBase::TurnToIdle()
{
	int nIndex = 0;

	//清除AI时间计时器
	for (nIndex = 0; nIndex < MAX_AI_EVENT; nIndex++)
	{
		m_nAIEventTimer[nIndex] = 0;
	}

	//-------- AI Event:TurnToIdle ---------------------------
	{
		KTarget rTarget;
		rTarget.ClearTarget();

		KThreatNode* pThreatNode = m_pSelf->m_ThreatList.GetFirstThreat();
		if (pThreatNode && pThreatNode->pCharacter)
		{
			rTarget.SetTarget(pThreatNode->pCharacter);
		}
		g_pSO3World->m_Settings.m_SmartDialogList.FireEvent((KNpc*)m_pSelf, ntatTurnToIdle, rTarget);

		FireAIEvent(aecTurnToIdle);
	}
	//--------------------------------------------------------

}

void KAIBase::Alert(KCharacter* pTarget)
{
	BOOL bRetCode = FALSE;

	ASSERT(m_pSelf);
	ASSERT(pTarget);

	if (m_pSelf->m_eMoveState != cmsOnStand)
	{
		bRetCode = m_pSelf->Stop();
		KGLOG_PROCESS_ERROR(bRetCode);
	}

	int nDirection = g_GetDirection(m_pSelf->m_nX, m_pSelf->m_nY, 
		pTarget->m_nX, pTarget->m_nY);

	bRetCode = m_pSelf->Turn(nDirection, true, true);
	KGLOG_PROCESS_ERROR(bRetCode);

Exit0:

	return;
}

void KAIBase::Wander(BOOL bForceBack /* = false */)
{
    BOOL    bResult     = false;
	int     nRetCode    = m_pSelf->m_eMoveState;

	KGLOG_PROCESS_ERROR(m_pAIParam->nWanderRange > 0);
    KGLOG_PROCESS_ERROR(m_pAIParam->nWanderDistance > 0);
    KGLOG_PROCESS_ERROR(m_pSelf->m_nWalkSpeed > 0);

	if (nRetCode == cmsOnStand)
	{
        int nDestX  = 0;
        int nDestY  = 0;
        int nFrame  = 0;
        int nAngel  = 0;
        int nDeltaX = 0;
        int nDeltaY = 0;

        //尝试走一段距离
		int nDistance = (int)g_Random(m_pAIParam->nWanderDistance) + 1;

		//至少逛1/4的距离
		if (nDistance < (m_pAIParam->nWanderDistance / 4) )
			nDistance = m_pAIParam->nWanderDistance / 4;

        if (bForceBack)
        {
            nAngel = (m_pSelf->m_nDirectionXY + DIRECTION_COUNT / 2) % DIRECTION_COUNT;
        }
		else
        {
            nAngel = (int)g_Random(DIRECTION_COUNT);
        }
		KGLOG_PROCESS_ERROR(nAngel >= 0 && nAngel < DIRECTION_COUNT);

		nDeltaX = nDistance * g_nCos[nAngel] / SIN_COS_NUMBER;
		nDeltaY = nDistance * g_nSin[nAngel] / SIN_COS_NUMBER;

		nDistance = g_GetDistance2(m_nOriginX, m_nOriginY, 
			m_pSelf->m_nX + nDeltaX, m_pSelf->m_nY + nDeltaY);
		if (nDistance > (m_pAIParam->nWanderRange * m_pAIParam->nWanderRange))
		{
			//超出范围了，改成往原点方向走
            nDestX = m_nOriginX;
            nDestY = m_nOriginY;
		}
		else
		{
			//往目标点走
            nDestX = m_pSelf->m_nX + nDeltaX;
            nDestY = m_pSelf->m_nY + nDeltaY;
		}

        nDistance = g_GetDistance2(nDestX, nDestY, m_pSelf->m_nX, m_pSelf->m_nY);

        ASSERT(m_pSelf->m_nWalkSpeed > 0);
        nFrame = (int)sqrt((double)nDistance) / m_pSelf->m_nWalkSpeed;
        nFrame = nFrame * 2 + GAME_FPS;

        m_WanderData.nWanderFrame = g_pSO3World->m_nGameLoop + nFrame;
        m_pSelf->WalkTo(nDestX, nDestY, true);
    }

    if (m_WanderData.nWanderFrame && g_pSO3World->m_nGameLoop > m_WanderData.nWanderFrame)
    {
        DoIdle(m_pAIParam->nWanderInterval);
        m_WanderData.nWanderFrame = 0;
    }

    bResult = true;
Exit0:
    if (!bResult)
    {
        KGLogPrintf(
            KGLOG_ERR, "[AI] AI stopped, because Wander failed. Character coordinate(%d, %d, %d).", 
            m_pSelf->m_nX, m_pSelf->m_nY, m_pSelf->m_nZ
        );

        m_pSelf->m_AIController.Stop();
    }

	return;
}

void KAIBase::Escape(void)
{
	KG_PROCESS_ERROR(g_pSO3World->m_nGameLoop > m_EscapeData.nEscapeIdleFrame);
	ASSERT(m_pSelf);

	int nRetCode = m_pSelf->m_eMoveState;
	int nX = m_pSelf->m_nX;
	int nY = m_pSelf->m_nY;

	if (nRetCode == cmsOnStand)
	{
		int nAngel = (int)g_Random(DIRECTION_COUNT);
		KGLOG_PROCESS_ERROR(nAngel >= 0 && nAngel < DIRECTION_COUNT);

		int nDeltaX = m_EscapeData.nEscapeRange * g_nCos[nAngel] / SIN_COS_NUMBER;
		int nDeltaY = m_EscapeData.nEscapeRange * g_nSin[nAngel] / SIN_COS_NUMBER;

		int nThreatPointX = m_pSelf->m_ThreatList.m_nThreatPointX;
		int nThreatPointY = m_pSelf->m_ThreatList.m_nThreatPointY;

		int nDir = g_GetDirection(nX, nY, m_pSelf->m_ThreatList.m_nThreatPointX + nDeltaX, 
									m_pSelf->m_ThreatList.m_nThreatPointY + nDeltaY);

		m_pSelf->Turn(nDir, true, true);
		m_pSelf->RunTo(nThreatPointX + nDeltaX, nThreatPointY + nDeltaY, TRUE);
	}

Exit0:
	return;
}

BOOL KAIBase::CallHelp(int nRange)
{
	BOOL bRetCode	= FALSE;
	int  nIndex		= 0;
	int	 nSize		= 0;

	KSearchForCharacter Tactic;
	Tactic.m_pSelf = m_pSelf;
	Tactic.m_nDistance2 = nRange * nRange;
	Tactic.m_nAngle = DIRECTION_COUNT;
	Tactic.m_bIgnoreNegative = TRUE;
	Tactic.m_nRelation = sortAlly;

	KGLOG_PROCESS_ERROR(Tactic.m_nDistance2 >= 0);

	AISearchCharacter(Tactic);

	nSize = (int)Tactic.m_Result.size();
	for (nIndex = 0; nIndex < nSize; nIndex ++)
	{
		KCharacter* pCharacter = Tactic.m_Result[nIndex].m_pCharacter;
		KGLOG_PROCESS_ERROR(pCharacter);

		pCharacter->m_ThreatList.CopyFromAllies(&(m_pSelf->m_ThreatList));
		pCharacter->m_AIController.OnAttacked(NULL, 0);
	}

	bRetCode = TRUE;
Exit0:
	return bRetCode;
}

BOOL KAIBase::CallHeal(int nRange)
{
	BOOL bRetCode = FALSE;
	KNpc* pNpc = NULL;
	KCharacter* pEnemy = NULL;
	KThreatNode* pFirstThreat = NULL;
	KThreatNode* pNode = NULL;
	int nRandom = 0;
	int nRelation = 0;
	BOOL bCallHealFlag = FALSE;
	
	KGLOG_PROCESS_ERROR(m_pSelf);
	KG_PROCESS_ERROR(IS_NPC(m_pSelf->m_dwID));

	pFirstThreat = m_pSelf->m_ThreatList.GetFirstThreat();
	KG_PROCESS_ERROR(pFirstThreat);
	
	pEnemy = pFirstThreat->pCharacter;
	KG_PROCESS_ERROR(pEnemy);
	
	pNode = pEnemy->m_ThreatList.GetFirstThreat();
	while (pNode)
	{
		KThreatNode* pNext = (KThreatNode*)(pEnemy->m_ThreatList.GetNextNode(pNode));
		ASSERT(pNode->pPairNode);

		bCallHealFlag = TRUE;

		KGLOG_PROCESS_ERROR(pNode->pCharacter);

		if (IS_NPC(pNode->pCharacter->m_dwID))
		{
			pNpc = (KNpc*)pNode->pCharacter;

			//随机
			nRandom = g_Random(CALL_HEAL_MAX_RATE);
			if (nRandom >= CALL_HEAL_RATE)
				bCallHealFlag = FALSE;
		
			//判断距离
			//TODO:

			//判断阵营
			nRelation = ((KNpc*)m_pSelf)->GetNpcRelation(pNpc);
			if (((nRelation & sortAlly) == 0) 
				&& ((nRelation & sortParty) == 0)
				&& ((nRelation & sortSelf) == 0))
			{
				bCallHealFlag = FALSE;
			}
	
			if (bCallHealFlag)
			{
				pNpc->m_AIController.SetCallHealer(m_pSelf->m_dwID);
			}
		}

		pNode = pNext;
	}
	

	bRetCode = TRUE;
Exit0:
	return bRetCode;
}

void KAIBase::Patrol()
{
	BOOL        bResult         = false;
    BOOL        bRetCode        = false;
	KNpc*       pNpc            = NULL;
    KPatrolNode PatrolNode;

    int         nIndex          = 0;
    int         nMemberCount    = 0;
    int         nTargetX        = 0;
    int         nTargetY        = 0;
    int         nFinalTargetX   = 0;
    int         nFinalTargetY   = 0;
    int         nTeamCenterX    = 0;
    int         nTeamCenterY    = 0;
    int         nRadius         = 0;
    int         nAngle          = 0;
    int         nDir            = 0;
    int         nDistance       = 0;
    int         nDistance2      = 0;
    int         nAdjustDis      = 0;
    KNpc*       pLeader         = NULL;
    BOOL        bWalkToTarget   = false;

	ASSERT(m_pSelf);

	bRetCode = IS_PLAYER(m_pSelf->m_dwID);
	KG_PROCESS_SUCCESS(bRetCode);

	pNpc = (KNpc*)m_pSelf;

	if (m_pPatrolPath == NULL && pNpc->m_AIData.m_nPatrolPathID != 0)
	{
		bRetCode = SetPatrolPath(pNpc->m_AIData.m_nPatrolPathID);
        KG_PROCESS_ERROR(bRetCode);
	}

    if (!m_pPatrolPath)
    {
        KGLogPrintf(
            KGLOG_DEBUG, 
            "巡逻路径未设置, NpcTemplateID = %d Position = [%d, %d]\n", 
            pNpc->m_pTemplate->dwTemplateID, pNpc->m_nX, pNpc->m_nY
        );
        goto Exit0;
    }

    if (!m_pNpcTeam)    // 只有一个人的情况
    {
        KG_PROCESS_SUCCESS(pNpc->m_eMoveState != cmsOnStand);

        bRetCode = m_pPatrolPath->GetPoint(m_nNextPartolIndex, PatrolNode);
        KG_PROCESS_ERROR(bRetCode);

        if (PatrolNode.bIsRun)
        {
            pNpc->RunTo(PatrolNode.nPatrolPointX, PatrolNode.nPatrolPointY, true);
        }
        else
        {
            pNpc->WalkTo(PatrolNode.nPatrolPointX, PatrolNode.nPatrolPointY, true);
        }

        goto Exit1;
    }

    // 以下是小队巡逻
    pLeader = m_pNpcTeam->m_pLeader;

    // 只处理队长，队员跟着队长
    KG_PROCESS_SUCCESS(pLeader != pNpc);

	bResult = m_pPatrolPath->GetPoint(m_nNextPartolIndex, PatrolNode);
	KG_PROCESS_ERROR(bResult);

	// 得到自身相对阵型中点的偏移
	bResult = m_pNpcTeam->GetMemberPostion(pNpc->m_nNpcTeamIndex, &nRadius, &nAngle);
	KGLOG_PROCESS_ERROR(bResult);

	// Npc正确的朝向,这步不一定精确,会有较大误差
	nDir = g_GetDirection(pLeader->m_nX, pLeader->m_nY, PatrolNode.nPatrolPointX, PatrolNode.nPatrolPointY);

	// 队长朝着终点走
	if (m_pSelf->m_eMoveState == cmsOnStand)
	{
		if (PatrolNode.bIsRun)
		{
			m_pSelf->RunTo(PatrolNode.nPatrolPointX, PatrolNode.nPatrolPointY, true);
		}
		else
		{
			m_pSelf->WalkTo(PatrolNode.nPatrolPointX, PatrolNode.nPatrolPointY, true);
		}
	}

	// 队员朝着队长的方向走
	nMemberCount = m_pNpcTeam->GetMemberCount();
	for (nIndex = 0; nIndex < nMemberCount; nIndex++)
	{
		int     nNpcTeamIndex = nIndex;
		KNpc*   pMemberNpc = m_pNpcTeam->GetMember(nNpcTeamIndex);

		if (pMemberNpc == NULL || pMemberNpc == pLeader || pMemberNpc->m_pScene == NULL)
        {
			continue;
        }

		if (
            pMemberNpc->m_eMoveState != cmsOnStand && pMemberNpc->m_eMoveState != cmsOnSit && 
            pMemberNpc->m_eMoveState != cmsOnWalk && pMemberNpc->m_eMoveState != cmsOnRun
        )
		{
			continue;
		}

		// 把队长的位置换到0点
		if (nNpcTeamIndex == 0)
		{
			nNpcTeamIndex = pLeader->m_nNpcTeamIndex;
		}

		bResult = m_pNpcTeam->GetMemberPostion(nNpcTeamIndex, &nRadius, &nAngle);
		KGLOG_PROCESS_ERROR(bResult);

		//让队员也往终点走
		if (pMemberNpc->m_eMoveState == cmsOnStand)
		{
			GetAdjustPoint(
                PatrolNode.nPatrolPointX, PatrolNode.nPatrolPointY, 
                nRadius, nAngle + nDir, nFinalTargetX, nFinalTargetY
            );

			bWalkToTarget = true;
		}

		//超过一定范围就追上去
		GetAdjustPoint(pLeader->m_nX, pLeader->m_nY, nRadius, nAngle + pLeader->m_nFaceDirection, nTargetX, nTargetY);

		nDistance2 = g_GetDistance2(pMemberNpc->m_nX, pMemberNpc->m_nY, nTargetX, nTargetY);
		nDistance = (int)sqrt((double)nDistance2);

		if (nDistance > (AI_FOLLOW_KEEPAWAY / 4))
		{
			nAdjustDis = (nDistance * m_pNpcTeam->m_pLeader->m_pTemplate->nWalkSpeed) / pMemberNpc->m_pTemplate->nRunSpeed;
			GetAdjustPoint(nTargetX, nTargetY, nAdjustDis, nDir, nFinalTargetX, nFinalTargetY);

			bResult = pMemberNpc->RunTo(nFinalTargetX, nFinalTargetY, true);
			KG_PROCESS_ERROR(bResult);
		}
		else if (bWalkToTarget)
		{
			bResult = pMemberNpc->WalkTo(nFinalTargetX, nFinalTargetY, true);
			KG_PROCESS_ERROR(bResult);
		}
	}

Exit1:
	bResult = true;
Exit0:
	if (!bResult && pNpc)
	{
		//巡逻出错,可能是路径设置的问题,先切到站立状态
		pNpc->m_AIController.SetMainState(aisIdle);
		pNpc->m_AIController.DoIdle(INT_MAX);
	}
	return;
}

//void KAIBase::Patrol()
//{
//	//巡逻到某一个点
//	int nRetCode = 0;
//
//	ASSERT(m_pSelf);
//
//	if (m_pPatrolPath == NULL && ((KNpc*)m_pSelf)->m_nPatrolPathID != 0)
//	{
//		KGLOG_PROCESS_ERROR(m_pSelf);
//		KGLOG_PROCESS_ERROR(IS_NPC(m_pSelf->m_dwID));
//		
//		SetPatrolPath(((KNpc*)m_pSelf)->m_nPatrolPathID);
//	}
//
//	if (m_pNpcTeam)
//	{
//		if (m_pNpcTeam->m_pLeader == m_pSelf)
//		{
//			KPatrolNode PatrolNode;
//			KNpc* pLeader = m_pNpcTeam->m_pLeader;
//			int nMemberCount = 0;
//			int nIndex = 0;
//			int nTargetX = 0;
//			int nTargetY = 0;
//			int nFinalTargetX = 0;
//			int nFinalTargetY = 0;
//			int nRadius = 0;
//			int nAngle = 0;
//			int nDistance2 = 0;
//			int nDistance = 0;
//			int nAdjustDis = 0; //追赶的调整距离
//			int nDir = 0;
//			BOOL bWalkToTarget = FALSE;
//
//			KGLOG_PROCESS_ERROR(m_pPatrolPath);
//
//			nRetCode = m_pPatrolPath->GetPoint(m_nNextPartolIndex, PatrolNode);
//			KG_PROCESS_ERROR(nRetCode);
//
//			//假如是队长，就正常巡逻
//			nRetCode = m_pSelf->m_eMoveState;
//			if (nRetCode == cmsOnStand)
//			{
//				if (PatrolNode.bIsRun)
//				{
//					m_pSelf->RunTo(PatrolNode.nPatrolPointX, PatrolNode.nPatrolPointY, TRUE);
//				}
//				else
//				{
//					m_pSelf->WalkTo(PatrolNode.nPatrolPointX, PatrolNode.nPatrolPointY, TRUE);
//				}
//			}
//
//			//检测队员位置
//			nMemberCount = m_pNpcTeam->GetMemberCount();
//			for (nIndex = 0; nIndex < nMemberCount; nIndex++)
//			{
//				int nNpcTeamIndex = 0;
//
//				KNpc* pMemberNpc = m_pNpcTeam->GetMember(nIndex);
//				if (pMemberNpc == NULL)
//					continue;
//
//				if (pMemberNpc == pLeader)
//					continue;
//
//				bWalkToTarget = FALSE;
//
//				nNpcTeamIndex = pMemberNpc->m_nNpcTeamIndex;
//				if (nNpcTeamIndex == 0)
//				{
//					// 如果队长不是小队的第一个Index,这个人和队长位置互换,临时代码
//					nNpcTeamIndex = pLeader->m_nNpcTeamIndex;
//				}
//
//				nRetCode = m_pNpcTeam->GetMemberPostion(nNpcTeamIndex, nRadius, nAngle);
//				KGLOG_PROCESS_ERROR(nRetCode);
//
//				//让队员也往终点走
//				nRetCode = pMemberNpc->m_eMoveState;
//				if (nRetCode == cmsOnStand)
//				{
//					nDir = g_GetDirection(pLeader->m_nX, pLeader->m_nY, PatrolNode.nPatrolPointX, PatrolNode.nPatrolPointY);
//					GetAdjustPoint(PatrolNode.nPatrolPointX, PatrolNode.nPatrolPointY, 
//						nRadius, nAngle + nDir, nFinalTargetX, nFinalTargetY);
//
//					bWalkToTarget = TRUE;
//					SetAIInterval(0.1);
//				}
//				else if (nRetCode == cmsOnRun)
//				{
//					SetAIInterval(0.1);
//				}
//				else
//				{
//					SetAIInterval(1.0);
//				}
//							
//				//超过一定范围就追上去
//				GetAdjustPoint(pLeader->m_nX, pLeader->m_nY, 
//								nRadius, nAngle + pLeader->m_nFaceDirection, nTargetX, nTargetY);
//					
//				nDistance2 = g_GetDistance2(pMemberNpc->m_nX, pMemberNpc->m_nY, nTargetX, nTargetY);
//				nDistance = (int)sqrt((double)nDistance2);
//		
//				if (nDistance > (AI_FOLLOW_KEEPAWAY / 4))
//				{
//					nAdjustDis = (nDistance * pLeader->m_pTemplate->nWalkSpeed) / pMemberNpc->m_pTemplate->nRunSpeed;
//					GetAdjustPoint(nTargetX, nTargetY, nAdjustDis, pLeader->m_nFaceDirection, nFinalTargetX, nFinalTargetY);
//
//					nRetCode = pMemberNpc->RunTo(nFinalTargetX, nFinalTargetY, TRUE);
//					KG_PROCESS_ERROR(nRetCode);
//				}
//				else if (bWalkToTarget)
//				{
//					nRetCode = pMemberNpc->WalkTo(nFinalTargetX, nFinalTargetY, TRUE);
//					KG_PROCESS_ERROR(nRetCode);
//				}
//			}
//		}
//	}
//	else
//	{
//		KPatrolNode PatrolNode;
//
//		KGLOG_PROCESS_ERROR(m_pPatrolPath);
//
//		nRetCode = m_pSelf->m_eMoveState;
//		if (nRetCode == cmsOnStand)
//		{
//			nRetCode = m_pPatrolPath->GetPoint(m_nNextPartolIndex, PatrolNode);
//			KG_PROCESS_ERROR(nRetCode);
//
//			if (PatrolNode.bIsRun)
//			{
//				m_pSelf->RunTo(PatrolNode.nPatrolPointX, PatrolNode.nPatrolPointY);
//			}
//			else
//			{
//				m_pSelf->WalkTo(PatrolNode.nPatrolPointX, PatrolNode.nPatrolPointY);
//			}
//		}
//	}
//
//	return;
//Exit0:
//	if (IS_NPC(m_pSelf->m_dwID))
//	{
//		KGLogPrintf(KGLOG_DEBUG, "巡逻路径未设置, NpcTemplateID = %d Position = [%d, %d]\n", 
//			((KNpc*)m_pSelf)->m_pTemplate->dwTemplateID, m_pSelf->m_nX, m_pSelf->m_nY);
//	}
//	
//
//	//巡逻出错,可能是路径设置的问题,先切到站立状态
//	m_pSelf->m_AIController.SetMainState(aisIdle);
//	m_pSelf->m_AIController.DoIdle(INT_MAX);
//
//	return;
//}

void KAIBase::CheckAIEvent(void)
{
	if (!m_pAIParam)
        return;

	//执行AI事件
	for (int nIndex = 0; nIndex < MAX_AI_EVENT; nIndex++)
	{
		BOOL bRetCode = m_pAIParam->AIEvent[nIndex].CheckEvent(m_pSelf, nIndex);

		if (!bRetCode)
            continue;

        m_pAIParam->AIEvent[nIndex].RunEvent(m_pSelf);

		if (m_pAIParam->AIEvent[nIndex].IsRunOnce())
		{
			//执行后Timer置为负数，就不在执行了
			m_nAIEventTimer[nIndex] = -1;
		}
		else
		{
			m_nAIEventTimer[nIndex] = m_pAIParam->AIEvent[nIndex].GetCDFrame() + g_pSO3World->m_nGameLoop;
		}
	}
}

void KAIBase::FireAIEvent(AI_EVENT_CONDITION eAIEvent)
{
	KGLOG_PROCESS_ERROR((eAIEvent >= aecEvent) && (eAIEvent < aecTotal) && m_pAIParam);

	for (int nIndex = 0; nIndex < MAX_AI_EVENT; nIndex++)
	{
		m_pAIParam->AIEvent[nIndex].FireEvent(m_pSelf, nIndex, eAIEvent);
	}

Exit0:
	return;
}

BOOL KAIBase::CheckReturn(void)
{
	if (m_ReturnData.nReturnFrame < 0)
	{
		//已经返回成功
		m_pSelf->Stop();
		m_pSelf->m_ThreatList.ClearAllThreat();
	}
	else if (g_pSO3World->m_nGameLoop >= m_ReturnData.nReturnFrame)
	{
		//超时返回
		m_pSelf->MoveTo(m_nReturnX, m_nReturnY, m_nReturnZ);
		m_pSelf->Stop();
		m_pSelf->m_ThreatList.ClearAllThreat();
		m_ReturnData.nReturnFrame = -1;
	}
	else
	{
		return FALSE;
	}

	// ------------- AI Event:ReturnToOrigin --------------------
	FireAIEvent(aecReturnToOrigin);
	// ----------------------------------------------------------

	return TRUE;
}

void KAIBase::CheckCallHeal(void)
{
	KG_PROCESS_ERROR(m_eAIState == aisPursuit);
	KG_PROCESS_SUCCESS(g_pSO3World->m_nGameLoop < m_PursuitData.nCallHealFrame);

	//检测血量
	int nMaxLife = m_pSelf->m_nMaxLife;
	int nCurrentLife = m_pSelf->m_nCurrentLife;
	KGLOG_PROCESS_ERROR(nMaxLife);

	double fLostLifeRate = (nMaxLife - nCurrentLife) / (double)nMaxLife;

	KG_PROCESS_ERROR(fLostLifeRate >= CALL_HEAL_LIMIT);

	CallHeal(CALL_HEAL_RANGE);
	m_PursuitData.nCallHealFrame = g_pSO3World->m_nGameLoop + CALL_HEAL_INTERVAL;

Exit1:
Exit0:
	return ;
}

BOOL KAIBase::CheckAttacked(void)
{
    BOOL            bResult     = false;
    KThreatNode*    pThreatNode = NULL;

	ASSERT(m_pSelf);
    pThreatNode = m_pSelf->m_ThreatList.GetFirstThreat();

    if (!pThreatNode)
    {
        AI_TRACE("怎么没人打我!");
        goto Exit0;
    }

	AI_TRACE("靠,被攻击了, 打架!");
	TurnToFight();

    bResult = true;
Exit0:
	return bResult;
}

BOOL KAIBase::CheckTargetInAlertRange(void)
{
    BOOL                    bResult     = false;
    KSearchForAnyCharacter  Tactic;

    KG_PROCESS_ERROR(m_pAIParam->nAlertRange);

	Tactic.m_pSelf                      = m_pSelf;
	Tactic.m_nDistance2                 = m_pAIParam->nAlertRange * m_pAIParam->nAlertRange;
	Tactic.m_nAngle                     = DIRECTION_COUNT;
	Tactic.m_nRelation                  = (int)sortEnemy;
	Tactic.m_bAdjustRangeByLevelDiff    = false;
	Tactic.m_bAdjustByAttribute         = true;

	// 不是主动怪，看见人也不会打
    KG_PROCESS_ERROR(m_pAIParam->bIsPositive);

	AISearchCharacter(Tactic);
    KG_PROCESS_ERROR(Tactic.m_pResult);

	AI_TRACE("小样，我在看着你!");
    
	if (IS_PLAYER(Tactic.m_pResult->m_dwID))
	{
		m_pSelf->m_SelectTarget.SetTarget(ttPlayer, Tactic.m_pResult->m_dwID);
	}
	else
	{
		m_pSelf->m_SelectTarget.SetTarget(ttNpc, Tactic.m_pResult->m_dwID);
	}

    if (!m_pSelf->IsInSpecialMoveLimit())
    {
        m_pSelf->Stop();
    }

	DoAlert();
	//KGLogPrintf(KGLOG_DEBUG, "[AI] Find Target in frame : %d\n", g_pSO3World->m_nGameLoop);

    bResult = true;
Exit0:
	return bResult;
}

BOOL KAIBase::CheckTargetInAttackRange(void)
{
    BOOL                    bResult     = false;
	KSearchForAnyCharacter  Tactic;

    Tactic.m_pSelf                      = m_pSelf;
	Tactic.m_nDistance2                 = m_pAIParam->nAlertRange * m_pAIParam->nAlertRange; //攻击范围用警戒范围按等级差算出
	Tactic.m_nAngle                     = DIRECTION_COUNT;
	Tactic.m_nRelation                  = (int)sortEnemy;
	Tactic.m_bAdjustRangeByLevelDiff    = true;
	Tactic.m_bAdjustByAttribute         = true;

	// 不是主动怪，看见人也不会打
    KG_PROCESS_ERROR(m_pAIParam->bIsPositive);

 	AISearchCharacter(Tactic);
    KG_PROCESS_ERROR(Tactic.m_pResult);

	AI_TRACE("哇,帅哥,老娘喜欢你,站住!");
	m_pSelf->m_ThreatList.ModifyThreat(Tactic.m_pResult, 0);

	TurnToFight();

    bResult = true;
Exit0:
	return bResult;
}

void KAIBase::KeepAttackRange(KCharacter* pTarget, KSkill* pSkill)
{
    int		                nRetCode            = 0;
    int                     nSelfDstX           = 0;
    int                     nSelfDstY           = 0;
    int                     nMoveDistance       = 0;
    int                     nMoveDirection      = 0;
    int                     nCurrentDistance    = 0;
    DWORD	                dwKeepDisDivisor    = m_PursuitData.dwKeepDisDivisor;
    CHARACTER_MOVE_STATE    eTargetMoveState    = cmsInvalid;
    BOOL                    bIsTargetMoving     = false;

    KGLOG_PROCESS_ERROR(pTarget);

    if (dwKeepDisDivisor == 0)
    {
        dwKeepDisDivisor = 1;
    }

    nCurrentDistance = g_GetDistance2(m_pSelf->m_nX, m_pSelf->m_nY, pTarget->m_nX, pTarget->m_nY);
    nCurrentDistance = (int)sqrt((double)nCurrentDistance);

    eTargetMoveState = pTarget->m_eMoveState;
    if (eTargetMoveState == cmsOnRun || eTargetMoveState == cmsOnWalk || eTargetMoveState == cmsOnJump)
    {
        bIsTargetMoving = true;
    }

    if (bIsTargetMoving)
    {
        int nMarginRange = (pSkill->m_nMaxRadius - pSkill->m_nMinRadius) / dwKeepDisDivisor;
        int nCurrentMargin = nCurrentDistance - pTarget->m_nTouchRange - pSkill->m_nMinRadius;

        if ((nCurrentMargin  > nMarginRange * 100 / 128) || (nCurrentMargin < nMarginRange * 30 / 128))
        {
            // 向60 / 128靠近
            nMoveDistance = nCurrentDistance - (pTarget->m_nTouchRange + pSkill->m_nMinRadius + nMarginRange * 60 / 128) + 32/*修正值*/;
            nMoveDirection = g_GetDirection(m_pSelf->m_nX, m_pSelf->m_nY, pTarget->m_nX, pTarget->m_nY);
        }
    }
    else
    {
        int nKeepRange = (m_pSelf->m_nTouchRange + pTarget->m_nTouchRange + 32/*修正值*/) / dwKeepDisDivisor;
        int nMaxRange = (pSkill->m_nMaxRadius + m_pSelf->m_nTouchRange + pTarget->m_nTouchRange - 32) / dwKeepDisDivisor;

        nKeepRange = max(nKeepRange, pSkill->m_nMinRadius);
        nMaxRange = max(nMaxRange, nKeepRange);

        if (nCurrentDistance < nKeepRange)
        {
            nMoveDistance = nKeepRange;
            nMoveDirection = g_GetDirection(pTarget->m_nX, pTarget->m_nY, m_pSelf->m_nX, m_pSelf->m_nY);
        }
        else if (nCurrentDistance > nMaxRange)
        {
            nMoveDistance = nCurrentDistance - nMaxRange;
            nMoveDirection = g_GetDirection(m_pSelf->m_nX, m_pSelf->m_nY, pTarget->m_nX, pTarget->m_nY);
        }
    }

    nSelfDstX = m_pSelf->m_nX + nMoveDistance * g_nCos[nMoveDirection] / SIN_COS_NUMBER;
    nSelfDstY = m_pSelf->m_nY + nMoveDistance * g_nSin[nMoveDirection] / SIN_COS_NUMBER;
    m_pSelf->RunTo(nSelfDstX, nSelfDstY);

Exit0:
    return;
}

void KAIBase::KeepAttackRangeAStar(KCharacter* pTarget, KSkill* pSkill)
{
    int		                nRetCode            = 0;
    int                     nSelfDstX           = 0;
    int                     nSelfDstY           = 0;
    int                     nSelfDstZ           = 0;
    int                     nMoveDistance       = 0;
    int                     nMoveDirection      = 0;
    int                     nCurrentDistance    = 0;
    int                     nSrcAbsoluteX       = 0;
    int                     nSrcAbsoluteY       = 0;
    int                     nSrcAbsoluteZ       = 0;
    int                     nDstAbsoluteX       = 0;
    int                     nDstAbsoluteY       = 0;
    int                     nDstAbsoluteZ       = 0;
    int                     nMinRange           = 0;
    int                     nMaxRange           = 0;
    DWORD	                dwKeepDisDivisor    = m_PursuitData.dwKeepDisDivisor;
    CHARACTER_MOVE_STATE    eTargetMoveState    = cmsInvalid;
    BOOL                    bIsTargetMoving     = false;
    BOOL                    bNeedPathFinding    = false;
    PATH_FINDER_STATE       ePathFinderState    = pfsInvalid;


    KGLOG_PROCESS_ERROR(pTarget);

    if (dwKeepDisDivisor == 0)
    {
        dwKeepDisDivisor = 1;
    }


    eTargetMoveState = pTarget->m_eMoveState;
    if (eTargetMoveState == cmsOnRun || eTargetMoveState == cmsOnWalk || eTargetMoveState == cmsOnJump)
    {
        bIsTargetMoving = true;
    }

    nCurrentDistance = g_GetDistance2(m_pSelf->m_nX, m_pSelf->m_nY, pTarget->m_nX, pTarget->m_nY);

    nMinRange = (m_pSelf->m_nTouchRange + pTarget->m_nTouchRange + 32/*修正值*/);
    nMaxRange = (pSkill->m_nMaxRadius + m_pSelf->m_nTouchRange + pTarget->m_nTouchRange - 32);

    nMinRange = max(nMinRange, pSkill->m_nMinRadius);
    nMaxRange = max(nMaxRange, nMinRange);

    if (bIsTargetMoving)
    {
        nMaxRange = nMinRange + (nMaxRange - nMinRange) * 6 / 8;
    }

    KG_PROCESS_SUCCESS(nCurrentDistance >= nMinRange * nMinRange && nCurrentDistance <= nMaxRange * nMaxRange);

    ASSERT(m_pSelf->m_eMoveState != cmsOnSwim);
    KG_PROCESS_SUCCESS(m_pSelf->m_eMoveState == cmsOnRun || m_pSelf->m_eMoveState == cmsOnJump);

    nRetCode = m_PathFinder.GetPathNextPoint(&nSelfDstX, &nSelfDstY, &nSelfDstZ);
    if (!nRetCode)
    {
        KG_PROCESS_ERROR(pTarget->m_pCell->m_BaseInfo.dwCellType != ctWater || pTarget->m_nZ >= 0);
        KG_PROCESS_SUCCESS(g_pSO3World->m_nGameLoop < m_nNextAStarFrame);

        m_nNextAStarFrame = g_pSO3World->m_nGameLoop + GAME_FPS * 2;

        m_pSelf->GetAbsoluteCoordinate(&nSrcAbsoluteX, &nSrcAbsoluteY, &nSrcAbsoluteZ);
        pTarget->GetAbsoluteCoordinate(&nDstAbsoluteX, &nDstAbsoluteY, &nDstAbsoluteZ);

        nRetCode = m_PathFinder.FindQuickPath(
            m_pSelf->m_pScene, 
            nSrcAbsoluteX, nSrcAbsoluteY, nSrcAbsoluteZ, 
            nDstAbsoluteX, nDstAbsoluteY, nDstAbsoluteZ - pTarget->m_nZ, 
            nMinRange, nMaxRange, pTarget->m_nHeight
        );
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = m_PathFinder.GetPathFirstPoint(&nSelfDstX, &nSelfDstY, &nSelfDstZ);
        KG_PROCESS_ERROR(nRetCode);
    }

    m_pSelf->RunTo(nSelfDstX, nSelfDstY);

Exit1:
Exit0:
    return;
}

void KAIBase::KeepWanderRange(int nRange)
{
    int nCurrentDistance = g_GetDistance2(m_pSelf->m_nX, m_pSelf->m_nY, m_nOriginX, m_nOriginY);

    if (nCurrentDistance > nRange * nRange)
    {
        m_pSelf->WalkTo(m_nOriginX, m_nOriginY, true);
    }
}

/************************************************************************/

#endif	//_SERVER
