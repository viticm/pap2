#include "StdAfx.h"

#if defined(_SERVER)

#include "KAIController.h"
#include "KMath.h"
#include "KSO3World.h"
#include "KObjectManager.h"
#include "KWorldSettings.h"
#include "KSkill.h"

#define AI_WOLF_SEARCH_RANGE			(CELL_LENGTH * 32)
#define AI_WOLF_WANDER_RANGE			(CELL_LENGTH * 32)
#define AI_WOLF_ATTACK_RANGE			(CELL_LENGTH * 8)
#define AI_WOLF_FOLLOW_ATTACK_RANGE		(CELL_LENGTH * 32)
#define AI_WOLF_ALERT_RANGE				(CELL_LENGTH * 16)
/*
//ÆÕÍ¨ÀÇ
void KAIController::OnWolfIdle(void)
{
	BOOL bRetCode = FALSE;

	ASSERT(m_pSelf);

	KG_PROCESS_SUCCESS(m_Variables.Wolf.bFoundKing);

	bRetCode = CheckReturn();
	KG_PROCESS_SUCCESS(bRetCode);

	int nOriginDistance2 = g_GetDistance2(m_pSelf->m_nX, m_pSelf->m_nY, m_nOriginX, m_nOriginY);

	bRetCode = CheckWanderRange(nOriginDistance2, AI_WOLF_WANDER_RANGE);
	KG_PROCESS_SUCCESS(bRetCode);

	bRetCode = CheckAttacked();
	KG_PROCESS_SUCCESS(bRetCode);

	int nTargetDistance3;
	KCharacter* pTarget = Search<KGetEnemy>(AI_WOLF_SEARCH_RANGE, nTargetDistance3);

	if (pTarget)
	{
		bRetCode = CheckTargetInAttackRange(pTarget, nTargetDistance3, AI_WOLF_ATTACK_RANGE);
		KG_PROCESS_SUCCESS(bRetCode);

		Alert(pTarget);
		KG_PROCESS_SUCCESS(TRUE);
	}

	Wander(AI_WOLF_WANDER_RANGE, GAME_FPS * 2);

Exit1:

	return;
}

void KAIController::OnWolfFight(void)
{
	BOOL bRetCode = FALSE;
	ASSERT(m_pSelf);

	KG_PROCESS_SUCCESS(m_Variables.Wolf.bFoundKing);

	KThreatNode* pThreatNode = m_pSelf->m_ThreatList.GetFirstThreat();
	KG_PROCESS_ERROR(pThreatNode);

	KCharacter* pTarget = pThreatNode->pCharacter;
	ASSERT(pTarget);

	int nOriginDistance2 = g_GetDistance2(m_pSelf->m_nX, m_pSelf->m_nY, m_nOriginX, m_nOriginY);
	KG_PROCESS_ERROR(nOriginDistance2 < AI_WOLF_FOLLOW_ATTACK_RANGE * AI_WOLF_FOLLOW_ATTACK_RANGE);

	if (m_pSelf->m_OTActionParam.eActionType == otActionIdle)
	{
		ASSERT(m_pSelf->m_eKind != ckPlayer);

		KNpcTemplate* pTemplate = g_WorldSettings.m_NpcTemplateList.GetTemplate(((KNpc*)m_pSelf)->m_dwTemplateID);
		ASSERT(pTemplate);

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
#endif	//_SERVER
