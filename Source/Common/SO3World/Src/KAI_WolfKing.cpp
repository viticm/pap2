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

#define TEAM_SPACE				(CELL_LENGTH)

int g_nTeamOrderList[totTotal][MAX_TEAM_MEMBER_COUNT][2] =
{
	//totArray,
	{
		{0, -2},
		{0, -4},
		{0, -6},
		{0, -8},
		{0, -10},
		{0, -12},
		{0, -14},
		{0, -16},
		{0, -18},
		{0, -20},
		{0, -22},
		{0, -24},
		{0, -26},
		{0, -28},
		{0, -30},
		{0, -32},
	},
	//totOneRow,
	{
		{	- 1, -2},
		{	  1, -2},
		{-2 - 1, -2},
		{ 2 + 1, -2},
		{-4 - 1, -2},
		{ 4 + 1, -2},
		{-6 - 1, -2},
		{ 6 + 1, -2},
		{-8 - 1, -2},
		{ 8 + 1, -2},
		{-10 - 1, -2},
		{ 10 + 1, -2},
		{-12 - 1, -2},
		{ 12 + 1, -2},
		{-14 - 1, -2},
		{ 14 + 1, -2},
		},
		//totTwoRow,
		{
			{	- 1, -2},
			{	  1, -2},
			{	- 1, -4},
			{	  1, -4},
			{-2 - 1, -2},
			{ 2 + 1, -2},
			{-2 - 1, -4},
			{ 2 + 1, -4},
			{-4 - 1, -2},
			{ 4 + 1, -2},
			{-4 - 1, -4},
			{ 4 + 1, -4},
			{-6 - 1, -2},
			{ 6 + 1, -2},
			{-6 - 1, -4},
			{ 6 + 1, -4},
		},
		//totGoose,
		{
			{	- 1, -2},
			{     1, -2},
			{-2 - 1, -4},
			{ 2 + 1, -4},
			{-4 - 1, -6},
			{ 4 + 1, -6},
			{-6 - 1, -8},
			{ 6 + 1, -8},
			{-8 - 1, -10},
			{ 8 + 1, -10},
			{-10 - 1, -12},
			{ 10 + 1, -12},
			{-12 - 1, -14},
			{ 12 + 1, -14},
			{-14 - 1, -16},
			{ 14 + 1, -16},
			},
			//totAntiGoose,
			{
				{	- 1, 2},
				{     1, 2},
				{-2 - 1, 4},
				{ 2 + 1, 4},
				{-4 - 1, 6},
				{ 4 + 1, 6},
				{-6 - 1, 8},
				{ 6 + 1, 8},
				{-8 - 1, 10},
				{ 8 + 1, 10},
				{-10 - 1, 12},
				{ 10 + 1, 12},
				{-12 - 1, 14},
				{ 12 + 1, 14},
				{-14 - 1, 16},
				{ 14 + 1, 16},
			},
			//totSurround,
			{
				{-2,  0},
				{ 2,  0},
				{ 0,  2},
				{-2, -2},
				{ 2, -2},
				{-2,  2},
				{ 2,  2},
				{-1, -2},
				{ 1, -2},
				{-2, -1},
				{ 2, -1},
				{-2,  1},
				{ 2,  1},
				{-1,  2},
				{ 1,  2},
				{ 0, -3},
				},
				//totLetterS,
				{
					{-4,  1},
					{-2,  0},
					{ 2,  1},
					{ 4,  3},
					{ 4,  5},
					{ 3,  7},
					{ 1,  8},
					{ 0,  8},
					{-1,  8},
					{-3,  9},
					{-4, 11},
					{-4, 13},
					{-2, 15},
					{ 0, 16},
					{ 2, 16},
					{ 4, 15},
				},
				//totLetterB,
				{
					{-3,  4},
					{-3,  0},
					{ 3,  1},
					{ 5,  3},
					{ 5,  5},
					{ 4,  7},
					{ 1,  8},
					{-1,  8},
					{-3,  8},
					{ 4,  9},
					{ 5, 11},
					{ 5, 13},
					{-3, 12},
					{-3, 16},
					{ 0, 16},
					{ 3, 15},
					},
};

/*
BOOL KAIController::KGetFriend::operator()(KCharacter* pCharacter)
{
	ASSERT(pCharacter);
	ASSERT(m_pSelf);	

	KCell* pFriendCell = pCharacter->m_pCell;
	KCell* pSelfCell = m_pSelf->m_pCell;

	ASSERT(pFriendCell);
	ASSERT(pSelfCell);

	if (pCharacter == m_pSelf)
		return TRUE;

	int nDistance3 = g_GetDistance3(m_pSelf->m_nX, m_pSelf->m_nY, (int)(m_pSelf->m_nZ / ALTITUDE_2_CELL),
		pCharacter->m_nX, pCharacter->m_nY, (int)(pCharacter->m_nZ / ALTITUDE_2_CELL));

	//检测距离在指定范围内
	if (nDistance3 >= m_nDistance3)
		return TRUE;

	//临时代码
	if (pCharacter->m_AIController.GetAIType() == aitWolf &&
		!pCharacter->m_AIController.m_Variables.Wolf.bFoundKing)
	{
		m_pTarget = pCharacter;
		m_nDistance3 = nDistance3;
		return FALSE;
	}

	return TRUE;
}

//狼王
void KAIController::OnWolfKingIdle(void)
{
	BOOL bRetCode = FALSE;
	KCharacter* pTarget = NULL;

	ASSERT(m_pSelf);

	bRetCode = CheckAllReturn(m_Variables.WolfKing.pChildList, AI_WOLF_KING_CHILD_COUNT);
	KG_PROCESS_SUCCESS(bRetCode);

	bRetCode = CheckAttacked();
	KG_PROCESS_SUCCESS(bRetCode);

	int nTargetDistance3;
	pTarget = Search<KGetFriend>(AI_WOLF_SEARCH_RANGE, nTargetDistance3);
	if (pTarget)
	{
		for (int nIndex = 0; nIndex < AI_WOLF_KING_CHILD_COUNT; nIndex++)
		{
			if (m_Variables.WolfKing.pChildList[nIndex] == NULL)
			{
				m_Variables.WolfKing.pChildList[nIndex] = pTarget;
				pTarget->m_AIController.m_Variables.Wolf.bFoundKing = TRUE;
				pTarget->m_ThreatList.SetKing(m_pSelf);

				break;
			}
		}
	}

	pTarget = Search<KGetEnemy>(AI_WOLF_SEARCH_RANGE, nTargetDistance3);
	if (pTarget)
	{
		bRetCode = CheckTargetInAttackRange(pTarget, nTargetDistance3, AI_WOLF_ATTACK_RANGE);
		KG_PROCESS_SUCCESS(bRetCode);

		Alert(pTarget);
		int nTargetDirection = g_GetDirection(m_pSelf->m_nX, m_pSelf->m_nY, pTarget->m_nX, pTarget->m_nY);
		if (nTargetDistance3 < AI_WOLF_ALERT_RANGE * AI_WOLF_ALERT_RANGE)
		{
			ApplyTeamOrder(pTarget, m_Variables.WolfKing.pChildList, AI_WOLF_KING_CHILD_COUNT, 
				g_nTeamOrderList[totSurround], nTargetDirection, (int)sqrt((double)nTargetDistance3) / 2,
				(int)sqrt((double)nTargetDistance3) / 2);
		}
		else
		{
			static int nCounter = 0;
			nCounter++;
			if (nCounter == 10)
			{
				ApplyTeamOrder(m_pSelf, m_Variables.WolfKing.pChildList, AI_WOLF_KING_CHILD_COUNT, 
					g_nTeamOrderList[totLetterS], nTargetDirection, CELL_LENGTH, CELL_LENGTH);
			}
			else if (nCounter == 20)
			{
				ApplyTeamOrder(m_pSelf, m_Variables.WolfKing.pChildList, AI_WOLF_KING_CHILD_COUNT, 
					g_nTeamOrderList[totLetterB], nTargetDirection, -CELL_LENGTH, CELL_LENGTH);
				nCounter = 0;
			}
		}

		KG_PROCESS_SUCCESS(TRUE);
	}

	Patrol(1, prsCircle);

	static int nOrderType = totArray;
	static int nLastState = cmsOnWalk;
	if (m_pSelf->GetMoveState() == cmsOnStand && nLastState == cmsOnWalk)
	{
		nOrderType++;
		if (nOrderType > totAntiGoose)
			nOrderType = totArray;
	}
	nLastState = m_pSelf->GetMoveState();

	ApplyTeamOrder(m_pSelf, m_Variables.WolfKing.pChildList, AI_WOLF_KING_CHILD_COUNT, 
		g_nTeamOrderList[nOrderType], m_pSelf->m_nDirectionXY, CELL_LENGTH, CELL_LENGTH);

Exit1:

	return;
}

void KAIController::OnWolfKingFight(void)
{
	BOOL bRetCode = FALSE;
	ASSERT(m_pSelf);

	KThreatNode* pThreatNode = m_pSelf->m_ThreatList.GetFirstThreat();
	KG_PROCESS_ERROR(pThreatNode);

	KCharacter* pTarget = pThreatNode->pCharacter;
	ASSERT(pTarget);

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

	KCharacter* pSecondTarget = NULL;
	pThreatNode = m_pSelf->m_ThreatList.GetNextNode(pThreatNode);
	if (pThreatNode)
	{
		pSecondTarget = pThreatNode->pCharacter;
	}

	for (int nIndex = 0; nIndex < AI_WOLF_KING_CHILD_COUNT; nIndex++)
	{
		KCharacter* pChild = m_Variables.WolfKing.pChildList[nIndex];
		if (pChild == NULL || pChild->m_OTActionParam.eActionType == otActionIdle)
			continue;

		ASSERT(pChild->m_eKind != ckPlayer);

		KNpcTemplate* pTemplate = g_WorldSettings.m_NpcTemplateList.GetTemplate(((KNpc*)pChild)->m_dwTemplateID);
		ASSERT(pTemplate);

		DWORD dwSkillID = pTemplate->dwSkillIDList[nstNormalAttack];
		DWORD dwSkillLevel = pTemplate->dwSkillLevelList[nstNormalAttack];
		if (dwSkillID != 0)
		{
			if (nIndex % 2 == 0 || pSecondTarget == NULL)
			{
				Attack(pChild, pTarget, dwSkillID, dwSkillLevel, nIndex * DIRECTION_COUNT / AI_WOLF_KING_CHILD_COUNT);
			}
			else
			{
				Attack(pChild, pSecondTarget, dwSkillID, dwSkillLevel, nIndex * DIRECTION_COUNT / AI_WOLF_KING_CHILD_COUNT);
			}
		}
	}

Exit1:

	return;

Exit0:
	TurnToIdle();
	for (int nIndex = 0; nIndex < AI_WOLF_KING_CHILD_COUNT; nIndex++)
	{
		KCharacter* pChild = m_Variables.WolfKing.pChildList[nIndex];
		if (pChild == NULL)
			continue;

		pChild->m_AIController.Return();
	}

	return;
}
*/
#endif	//_SERVER