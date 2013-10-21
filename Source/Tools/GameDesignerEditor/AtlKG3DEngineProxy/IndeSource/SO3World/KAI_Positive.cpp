// ------------------------------------------------------------------------
// ---------------- 默认主动AI ---------------------------------

#include "stdafx.h"

#if defined(_SERVER)

#include "KAIController.h"
#include "KMath.h"
#include "KSO3World.h"
#include "KObjectManager.h"
#include "KWorldSettings.h"
#include "KSkill.h"
#include "KSkillManager.h"
#include "KAISearchTactics.h"

static const int IDLE_FRAME  =  8;

void KAIController::_Construct_Positive()
{
    int     nRetCode = false;
    KNpc*   pNpc     = NULL;

    KG_ASSERT_EXIT(m_pSelf);

    nRetCode = g_IsPlayer(m_pSelf->m_dwID);
    KG_ASSERT_EXIT(!nRetCode);

    pNpc = (KNpc*)m_pSelf;

    m_eAIState      = aisIdle;
    m_nFrameCount   = g_SO3World.m_nGameLoop + IDLE_FRAME;

    // 不要调DoIdle,这时候相关初始化尚未完成
	KNpcTemplate* pTemplate = g_WorldSettings.m_NpcTemplateList.GetTemplate(pNpc->m_dwTemplateID);
	KG_ASSERT_EXIT(pTemplate);

    m_nAlarmRange = pTemplate->nAlarmRange;


Exit0:
    return;
}

void KAIController::_Activate_Positive()
{
    _Activate();
}

void KAIController::_OnStateIdle_Positive()
{
	BOOL bRetCode = FALSE;

	bRetCode = CheckAttacked();
	KG_PROCESS_SUCCESS(bRetCode);

	bRetCode = CheckTargetInAttackRange();
	KG_PROCESS_SUCCESS(bRetCode);

    if (g_SO3World.m_nGameLoop > m_nFrameCount)
    {
        AI_TRACE("别发呆了,继续走 !");
        DoPatrol();
        return;
    }
Exit1:
	return;
}

void KAIController::_OnStatePatrol_Positive()
{
    int nRetCode = false;
	BOOL bRetCode = FALSE;

	bRetCode = CheckAttacked();
	KG_PROCESS_SUCCESS(bRetCode);

	bRetCode = CheckTargetInAttackRange();
	KG_PROCESS_SUCCESS(bRetCode);

    nRetCode = m_pSelf->GetMoveState();
    if (nRetCode == cmsOnStand)
    {
        int         nAngel = m_pSelf->m_nFaceDirection - 40 + (int)g_Random(80);        
        const int   nRange = 32 * 2 * 16; // 16米
        if (nAngel < 0)
            nAngel += DIRECTION_COUNT;
        if (nAngel >= DIRECTION_COUNT)
            nAngel -= DIRECTION_COUNT;
        KGLOG_PROCESS_ERROR(nAngel >= 0 && nAngel < DIRECTION_COUNT);
        int nDeltaX = nRange * g_nCos[nAngel] / SIN_COS_NUMBER;
        int nDeltaY = nRange * g_nSin[nAngel] / SIN_COS_NUMBER;
		m_pSelf->WalkTo(m_pSelf->m_nX + nDeltaX, m_pSelf->m_nY + nDeltaY);
		return;        
    }

    KeepWanderRange(CELL_LENGTH * 2 * 100);

Exit1:
	return ;
Exit0:
    return ;
}

void KAIController::_OnStateAlert_Positive()
{
}

void KAIController::_OnStateFollow_Positive()
{
}

void KAIController::_OnStatePursuit_Positive()
{
	int nRetCode = FALSE;
	BOOL bSkipKeepRange = FALSE;

    // 检查仇恨表是否为空
	KThreatNode* pThreatNode = m_pSelf->m_ThreatList.GetFirstThreat();
	if (!pThreatNode)
	{
        AI_TRACE("仇恨表空的,不打了回去!");
        DoReturn();
        return;
	}

	KCharacter* pTarget = pThreatNode->pCharacter;
	KG_ASSERT_EXIT(pTarget);
    KGLOG_PROCESS_ERROR(pTarget->m_pScene);    
    KGLOG_PROCESS_ERROR(pTarget->m_pCell);    

    // 检查技能状态
	if (m_pSelf->m_OTActionParam.eActionType != otActionIdle)
    {
        AI_TRACE("正在做其他事情,暂不攻击!");
        return;
    }

	KGLOG_PROCESS_ERROR(m_pSelf->m_eKind != ckPlayer);

	KNpc* pNpc = dynamic_cast<KNpc*>(m_pSelf);
	KGLOG_PROCESS_ERROR(pNpc);

	// 释放普通攻击，测试代码
	KNpcTemplate* pTemplate = g_WorldSettings.m_NpcTemplateList.GetTemplate(((KNpc*)m_pSelf)->m_dwTemplateID);
	KGLOG_PROCESS_ERROR(pTemplate);

	DWORD dwSkillID = pTemplate->dwSkillIDList[0];
	DWORD dwSkillLevel = pTemplate->dwSkillLevelList[0];

    char szMsg[64];

    KGLOG_PROCESS_ERROR(pTarget != m_pSelf);

    sprintf(szMsg, "试图用技能(%u, %u)攻击\"%s\"", dwSkillID, dwSkillLevel, pTarget->m_szName);
    AI_TRACE(szMsg);

    KG_PROCESS_ERROR(dwSkillID != INVALID_SKILL_ID);

    KTarget Target;
    if (g_IsPlayer(pTarget->m_dwID))
    {
        Target.eTargetType = ttPlayerPointer;
    }
    else
    {
        Target.eTargetType = ttNpcPointer;
    }
    Target.pCharacter = pTarget;

    int   nDirection  = g_GetDirection(m_pSelf->m_nX, m_pSelf->m_nY, pTarget->m_nX, pTarget->m_nY);

    if (m_pSelf->m_nFaceDirection != nDirection)
    {
        m_pSelf->Turn(nDirection);
    }

	// 检测技能释放频率
	// 临时代码
	if ((g_SO3World.m_nGameLoop - pNpc->m_nSkillCastFrame[0]) >= AI_ATTACK_INTERVAL)
	{
		nRetCode = m_pSelf->CastSkill(dwSkillID, dwSkillLevel, Target);

		switch (nRetCode)
		{
		case srcSuccess:
			//KGLogPrintf(KGLOG_DEBUG, "哈哈哈,老娘打中了 !");

			// 测试代码：判断目标的移动状态
			if (pTarget && pTarget->m_eMoveState == cmsOnStand)
			{
				m_pSelf->m_ThreatList.UpdateKeepThreatField();
				bSkipKeepRange = TRUE;
			}
			break;
		case srcTooCloseTarget:
			sprintf(szMsg, "靠,太近了 !\n", nRetCode);
			//KGLogPrintf(KGLOG_DEBUG, szMsg);
			break;
		case srcTooFarTarget:
			sprintf(szMsg, "靠,太远了 !\n", nRetCode);
			//KGLogPrintf(KGLOG_DEBUG, szMsg);
			break;
		case srcOutOfAngle:
			//KGLogPrintf(KGLOG_DEBUG, "角度不对 !");
			m_pSelf->Turn(m_pSelf->m_nFaceDirection + 17);
			break;
		case srcTargetInvisible:
			//KGLogPrintf(KGLOG_DEBUG, "看不到目标 !");
			break;
		case srcNotEnoughLife:
		case srcNotEnoughMana:
		case srcNotEnoughStamina:
		case srcNotEnoughRage:
		case srcNotEnoughItem:
			//KGLogPrintf(KGLOG_DEBUG, "不会吧,消耗不够了 !");
			break;
		default:
			sprintf(szMsg, "他妈的,怎么打不了啊 (%d). \n", nRetCode);
			//KGLogPrintf(KGLOG_DEBUG, szMsg);
			break;
		}

		//刷新技能的释放时间
		//KGLogPrintf(KGLOG_DEBUG, "CastSkill \t\t\t\t In %d\n", g_SO3World.m_nGameLoop);
		pNpc->m_nSkillCastFrame[0] = g_SO3World.m_nGameLoop;
	}

	if (bSkipKeepRange == FALSE)
	{
		// 维持距离
		KSkill* pSkill = g_SkillManager.GetSkill_S(dwSkillID, dwSkillLevel);
		KGLOG_PROCESS_ERROR(pSkill);

		KeepAttackRange(pTarget, pSkill);
	}
Exit0:
    return;
}

void KAIController::_OnStateKeepAway_Positive()
{
}


void KAIController::_OnStateEscape_Positive()
{
}

void KAIController::_OnStateReturn_Positive()
{
    int nRetCode = false;

	// TODO: 把NPC移出战斗，并且给予无敌状态

	// 跑回ReturnPoint
	m_pSelf->RunTo(m_nReturnX, m_nReturnY);
}

void KAIController::_OnEventAttacked_Positive(KCharacter* pAttacker, int nDamage)
{
	BOOL bRetCode = FALSE;

	switch(m_eAIState)
	{
	case aisIdle:
	case aisPatrol:
	case aisAlert:
	case aisFollow:
		TurnToFight();
		DoPursuit();
		m_pSelf->m_ThreatList.UpdateKeepThreatField();
	    break;
	case aisPursuit:
		break;
	case aisEscape:
		break;
	case aisReturn:
		break;
	default:
		KGLOG_PROCESS_ERROR(FALSE);
	    break;
	}

Exit0:
	return;
}

void KAIController::_OnEventPathResult_Positive(BOOL bSucceed)
{
    BOOL bRetCode = false;

	//KGLogPrintf(KGLOG_DEBUG, "[AI] path event : %d , frame : %d\n", bSucceed, g_SO3World.m_nGameLoop);

    switch (m_eAIState)
    {
    case aisIdle:        
        break;
    case aisPatrol:
        if (bSucceed)
        {
            // 如果成功了,则发一会儿呆再说
            DoIdle(IDLE_FRAME);
        }
        else
        {
            // 如果寻路失败了,则换个方向再来
            int nDirection = m_pSelf->m_nFaceDirection + 40;
            if (nDirection >= DIRECTION_COUNT)
                nDirection -= DIRECTION_COUNT;
            KGLOG_PROCESS_ERROR(nDirection >= 0 && nDirection < DIRECTION_COUNT);
            m_pSelf->Turn(nDirection);
            DoPatrol();
        }
        break;
    case aisAlert:
        break;
    case aisFollow:
        break;
    case aisPursuit:
        if (bSucceed)
        {
            _OnStatePursuit_Positive();
        }
        break;
    case aisEscape:
        break;
	case aisReturn:
        if (!bSucceed)
        {
            //bRetCode = g_IsPlayer(m_pSelf->m_dwID);
            //KG_ASSERT_EXIT(!bRetCode);
            //KG_ASSERT_EXIT(m_pSelf->m_pScene);
            //m_pSelf->m_pScene->MoveNpc((KNpc*)m_pSelf, m_nOriginX,  m_nOriginY);
        }
		else
		{
			//KGLogPrintf(KGLOG_DEBUG, "ARRIVED Point x:%d y:%d \t IN %d\nSavePoint     x:%d y:%d\n", 
			//	m_pSelf->m_nX, m_pSelf->m_nY, g_SO3World.m_nGameLoop, m_nReturnX, m_nReturnY);
			DoIdle(IDLE_FRAME);
		}
        break;
    default:
		KGLOG_PROCESS_ERROR(FALSE);
        break;
    }

Exit0:
    return;
}


#endif
