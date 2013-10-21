// ------------------------------------------------------------------------
// ---------------- 默认被动AI ---------------------------------

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

const int IDLE_FRAME = 8;

void KAIController::_Construct_Negative()
{
    ASSERT(m_pSelf);
    m_eAIState = aisIdle;
    m_nFrameCount = g_SO3World.m_nGameLoop + IDLE_FRAME;
    // 不要调DoIdle,这时候相关初始化尚未完成
}

void KAIController::_Activate_Negative()
{
    _Activate();
}

void KAIController::_OnStateIdle_Negative()
{
    // 检查是否被攻击,如果被攻击,者转入追击状态
	KThreatNode* pThreatNode = m_pSelf->m_ThreatList.GetFirstThreat();
	if (pThreatNode)
	{
        AI_TRACE("有人打我,拼了 !");
        DoPursuit();
        return;
	}

    if (g_SO3World.m_nGameLoop > m_nFrameCount)
    {
        AI_TRACE("别发呆了,继续走 !");
        DoPatrol();
        return;
    }
}

void KAIController::_OnStatePatrol_Negative()
{
    int nRetCode = false;

    // 检查是否被攻击,如果被攻击,者转入追击状态
	KThreatNode* pThreatNode = m_pSelf->m_ThreatList.GetFirstThreat();
	if (pThreatNode)
	{
        AI_TRACE("有人打我,拼了 !");
        DoPursuit();
        return;
	}

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

Exit0:
    return;
}

void KAIController::_OnStateAlert_Negative()
{
}

void KAIController::_OnStateFollow_Negative()
{
}

void KAIController::_OnStatePursuit_Negative()
{
	int nRetCode = false;

    // 检查仇恨表是否为空
	KThreatNode* pThreatNode = m_pSelf->m_ThreatList.GetFirstThreat();
	if (!pThreatNode)
	{
        AI_TRACE("仇恨表空的,不打了,发会儿呆!");
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

	KNpcTemplate* pTemplate = g_WorldSettings.m_NpcTemplateList.GetTemplate(((KNpc*)m_pSelf)->m_dwTemplateID);
	KGLOG_PROCESS_ERROR(pTemplate);

	DWORD dwSkillID = pTemplate->dwSkillIDList[0];
	DWORD dwSkillLevel = pTemplate->dwSkillLevelList[0];

    char szMsg[64];

    KGLOG_PROCESS_ERROR(pTarget != m_pSelf);

    sprintf(szMsg, "试图用技能(%u, %u)攻击\"%s\"", dwSkillID, dwSkillLevel, pTarget->m_szName);
    AI_TRACE(szMsg);

    KGLOG_PROCESS_ERROR(dwSkillID != INVALID_SKILL_ID);

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

    nRetCode = m_pSelf->CastSkill(dwSkillID, dwSkillLevel, Target);

    switch (nRetCode)
    {
    case srcSuccess:
        AI_TRACE("哈哈哈,老娘打中了 !");
        break;
    case srcTooCloseTarget:
        sprintf(szMsg, "靠,太近了 !\n", nRetCode);
        AI_TRACE(szMsg);
        break;
    case srcTooFarTarget:
        sprintf(szMsg, "靠,太远了 !\n", nRetCode);
        AI_TRACE(szMsg);
        break;
    case srcOutOfAngle:
        AI_TRACE("角度不对 !");
        m_pSelf->Turn(m_pSelf->m_nFaceDirection + 17);
        break;
    case srcTargetInvisible:
        AI_TRACE("看不到目标 !");
        break;
    case srcNotEnoughLife:
    case srcNotEnoughMana:
    case srcNotEnoughStamina:
    case srcNotEnoughRage:
    case srcNotEnoughItem:
        AI_TRACE("不会吧,消耗不够了 !");
        break;
    default:
        sprintf(szMsg, "他妈的,怎么打不了啊 (%d). \n", nRetCode);
        AI_TRACE(szMsg);
        break;
    }

    // 维持距离
    KSkill* pSkill = g_SkillManager.GetSkill_S(dwSkillID, dwSkillLevel);
    KGLOG_PROCESS_ERROR(pSkill);

    KeepAttackRange(pTarget, pSkill);

Exit0:
    return;
}

void KAIController::_OnStateKeepAway_Negative()
{
}


void KAIController::_OnStateEscape_Negative()
{
}

void KAIController::_OnStateReturn_Negative()
{
    int nRetCode = false;

    nRetCode = m_pSelf->GetMoveState();
    if (nRetCode == cmsOnStand)
    {
        m_pSelf->WalkTo(m_nOriginX, m_nOriginY);
    }

	KThreatNode* pThreatNode = m_pSelf->m_ThreatList.GetFirstThreat();
	if (pThreatNode)
	{
        DoPursuit();
        return;
	}
}

void KAIController::_OnEventAttacked_Negative(KCharacter* pAttacker, int nDamage)
{
}

void KAIController::_OnEventPathResult_Negative(BOOL bSucceed)
{
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
            //_OnStatePursuit_Negative();
        }
        break;
    case aisEscape:
        break;
    case aisReturn:
        if (!bSucceed)
        {
            m_pSelf->m_nX = m_nOriginX;
            m_pSelf->m_nY = m_nOriginY;
        }
        DoIdle(IDLE_FRAME);
        break;
    default:
        break;
    }

Exit0:
    return;
}


#endif
