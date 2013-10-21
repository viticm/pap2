#include "stdafx.h"

#if defined(_SERVER)

#include "KAIController.h"
#include "KMath.h"
#include "KSO3World.h"
#include "KObjectManager.h"
#include "KWorldSettings.h"
#include "KSkill.h"
/*
//不攻击小动物的状态处理函数
void KAIController::OnCritterIdle(void)
{
	BOOL bRetCode = FALSE;

	ASSERT(m_pSelf);

	bRetCode = CheckReturn();
	KG_PROCESS_SUCCESS(bRetCode);

	int nOriginDistance2 = g_GetDistance2(m_pSelf->m_nX, m_pSelf->m_nY, m_nOriginX, m_nOriginY);

	bRetCode = CheckWanderRange(nOriginDistance2, m_Params.CritterParams.nWanderMaxRange);
	KG_PROCESS_SUCCESS(bRetCode);

	bRetCode = CheckAttacked();
	KG_PROCESS_SUCCESS(bRetCode);

	int nTargetDistance3;
	KCharacter* pTarget = Search<KGetEnemy>(m_Params.CritterParams.nSearchRange, nTargetDistance3);

	if (pTarget)
	{
		Follow(pTarget, CELL_LENGTH * 8);

		return;
	}

	Wander(m_Params.CritterParams.nWanderRange, m_Params.CritterParams.nWanderDuration);

Exit1:

	return;
}

void KAIController::OnCritterFight(void)
{
	BOOL bRetCode = FALSE;
	ASSERT(m_pSelf);

	KThreatNode* pThreatNode = m_pSelf->m_ThreatList.GetFirstThreat();
	KG_PROCESS_ERROR(pThreatNode);

	KCharacter* pTarget = pThreatNode->pCharacter;
	ASSERT(pTarget);

	int nOriginDistance2 = g_GetDistance2(m_pSelf->m_nX, m_pSelf->m_nY, m_nReturnX, m_nReturnY);
	KG_PROCESS_ERROR(nOriginDistance2 < m_Params.CritterParams.nFollowAttackRange * m_Params.CritterParams.nFollowAttackRange);

	if (m_pSelf->m_OTActionParam.eActionType == otActionIdle)
	{
		ASSERT(m_pSelf->m_eKind != ckPlayer);

		KNpcTemplate* pTemplate = g_WorldSettings.m_NpcTemplateList.GetTemplate(((KNpc*)m_pSelf)->m_dwTemplateID);
		ASSERT(pTemplate);

		if (m_pSelf->m_nCurrentLife < (m_pSelf->m_nMaxLife / 2))
		{
			DWORD dwSkillID = pTemplate->dwSkillIDList[nstHeal];
			DWORD dwSkillLevel = pTemplate->dwSkillLevelList[nstHeal];
			if (dwSkillID != 0)
			{
				Attack(m_pSelf, dwSkillID, dwSkillLevel);
				return;
			}
		}

		DWORD dwSkillID = pTemplate->dwSkillIDList[nstNormalAttack];
		DWORD dwSkillLevel = pTemplate->dwSkillLevelList[nstNormalAttack];
		if (dwSkillID != 0)
		{
			Attack(pTarget, dwSkillID, dwSkillLevel);
		}
	}

Exit1:

	return;

Exit0:
	TurnToIdle();

	return;
}
*/
#endif
