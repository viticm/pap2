#include "stdafx.h"
#include "./krlanimationtable.h"
#include "./ktabletransform.h"
#include "../../SO3Represent.h"

DWORD GetBuffAdjustAnimationIndexID(DWORD dwAnimationIndexID, DWORD const dwSkillBuffs[SKILL_BUFF_PART_COUNT])
{
	// 输入dwAnimationID为原始动作编号,输出经过BuffID修正的动作编号

	KPlayerBuffAnimation const* pPlayerBuffAnimation = NULL;

	for (int nIndex = SKILL_BUFF_PART_COUNT - 1; nIndex >= 0; --nIndex)
	{
		pPlayerBuffAnimation = g_pRL->m_TableList.GetPlayerBuffAnimation(nIndex, dwSkillBuffs[nIndex], dwAnimationIndexID);
		if (pPlayerBuffAnimation)
		{
			dwAnimationIndexID = pPlayerBuffAnimation->dwBuffAniID;
			break;
		}
	}

	return dwAnimationIndexID;
}

static DWORD GetTerrainlessAdjustAnimationIndexID(DWORD dwAnimationIndexID, KRLAdjustAnimationParam const* pParam)
{
	KRLAdjustAnimationIndex AdjustAnimation;

	KG_ASSERT_EXIT(pParam);

	dwAnimationIndexID = GetBuffAdjustAnimationIndexID(dwAnimationIndexID, pParam->dwSkillBuffs);

	if (!GetCharacterAdjustAnimationIndex(&pParam->CharacterParam, dwAnimationIndexID, &AdjustAnimation))
		return dwAnimationIndexID;

	if (pParam->bRide)
	{
		// 骑马奔跑有两个速率动作
		if (pParam->bSpeedup && AdjustAnimation.dwSpeedupAnimationID != 0)
		{
			return AdjustAnimation.dwSpeedupAnimationID;
		}

		if (AdjustAnimation.dwRidesAnimationID != 0)
		{
			return AdjustAnimation.dwRidesAnimationID;
		}
	}   

	if (pParam->bJump)
	{
		if (pParam->bAboveWater)
		{
			if (AdjustAnimation.dwWaterFlyAnimationID != 0)
			{
				return AdjustAnimation.dwWaterFlyAnimationID;
			}
		}
		else
		{
			if (AdjustAnimation.dwJumpAnimationID != 0)
			{
				return AdjustAnimation.dwJumpAnimationID;
			}
		}
	}

	if (pParam->bSwim && AdjustAnimation.dwSwimAnimationID != 0)
		return AdjustAnimation.dwSwimAnimationID;

Exit0:
	return dwAnimationIndexID;
}

static DWORD GetTerrainAdjustAnimationIndexID(DWORD dwAnimationIndexID, KRLAdjustAnimationParam const* pParam)
{
	DWORD dwAdjustAnimationIndexID = dwAnimationIndexID;

	KG_ASSERT_EXIT(pParam);

	if (pParam->CharacterParam.bPlayer)
	{
		KRLTerrainAdjustAnimation const* pTerrainAdjustAnimation = NULL;

		KG_PROCESS_ERROR(pParam->CharacterParam.nRoleType != rtInvalid);

		pTerrainAdjustAnimation = g_pRL->m_TableList.GetPlayerTerrainAdjustAnimation(
			dwAnimationIndexID, pParam->dwTerrainID, pParam->CharacterParam.nRoleType);
		KG_PROCESS_ERROR(pTerrainAdjustAnimation);

		dwAdjustAnimationIndexID = pTerrainAdjustAnimation->dwAdjustAnimationID;
	}

Exit0:
	return dwAdjustAnimationIndexID;
}

DWORD GetAdjustAnimationIndexID(DWORD dwAnimationIndexID, KRLAdjustAnimationParam const* pParam)
{
	KG_ASSERT_EXIT(pParam);

	dwAnimationIndexID = GetTerrainlessAdjustAnimationIndexID(dwAnimationIndexID, pParam);
	dwAnimationIndexID = GetTerrainAdjustAnimationIndexID(dwAnimationIndexID, pParam);

Exit0:
	return dwAnimationIndexID;
}

RL_ANIMATION_TABLE GetSwimAnimationTable(RL_ADVANCE nAdvance)
{
	static RL_ANIMATION_TABLE const s_nAnimationTable[RL_ADVANCE_COUNT] =
	{
		RL_ANIMATION_TABLE_SWIM_LEFT,
		RL_ANIMATION_TABLE_SWIM_RIGHT,
		RL_ANIMATION_TABLE_SWIM_FORWARD,
		RL_ANIMATION_TABLE_SWIM_BACKWARD,
	};

	return s_nAnimationTable[nAdvance];
}

RL_ANIMATION_TABLE GetDashAnimationTable(RL_ADVANCE nAdvance)
{
	static RL_ANIMATION_TABLE const s_nAnimationTable[RL_ADVANCE_COUNT] =
	{
		RL_ANIMATION_TABLE_DASH_LEFT,
		RL_ANIMATION_TABLE_DASH_RIGHT,
		RL_ANIMATION_TABLE_DASH_FORWARD,
		RL_ANIMATION_TABLE_DASH_BACKWARD,
	};

	return s_nAnimationTable[nAdvance];
}

RL_ANIMATION_TABLE GetRepulsedAnimationTable(RL_ADVANCE nAdvance)
{
	static RL_ANIMATION_TABLE const s_nAnimationTable[RL_ADVANCE_COUNT] =
	{
		RL_ANIMATION_TABLE_REPULSED_LEFT,
		RL_ANIMATION_TABLE_REPULSED_RIGHT,
		RL_ANIMATION_TABLE_REPULSED_FORWARD,
		RL_ANIMATION_TABLE_REPULSED_BACKWARD,
	};

	return s_nAnimationTable[nAdvance];
}

RL_ANIMATION_TABLE GetSkidAnimationTable(RL_ADVANCE nAdvance)
{
	static RL_ANIMATION_TABLE const s_nAnimationTable[RL_ADVANCE_COUNT] =
	{
		RL_ANIMATION_TABLE_SKID_LEFT,
		RL_ANIMATION_TABLE_SKID_RIGHT,
		RL_ANIMATION_TABLE_SKID_FORWARD,
		RL_ANIMATION_TABLE_SKID_BACKWARD,
	};

	return s_nAnimationTable[nAdvance];
}

RL_ANIMATION_TABLE GetDashIdleAnimationTable(RL_ADVANCE nAdvance)
{
	static RL_ANIMATION_TABLE const s_nAnimationTable[RL_ADVANCE_COUNT] =
	{
		RL_ANIMATION_TABLE_DASH_LEFT_IDLE,
		RL_ANIMATION_TABLE_DASH_RIGHT_IDLE,
		RL_ANIMATION_TABLE_DASH_FORWARD_IDLE,
		RL_ANIMATION_TABLE_DASH_BACKWARD_IDLE,
	};

	return s_nAnimationTable[nAdvance];
}

static RL_ANIMATION_TABLE GetSlipAnimationTable(RL_ADVANCE nAdvance)
{
	static RL_ANIMATION_TABLE const s_nAnimationTable[RL_ADVANCE_COUNT] =
	{
		RL_ANIMATION_TABLE_SLIP_LEFT,
		RL_ANIMATION_TABLE_SLIP_RIGHT,
		RL_ANIMATION_TABLE_SLIP_FORWARD,
		RL_ANIMATION_TABLE_SLIP_BACKWARD,
	};

	return s_nAnimationTable[nAdvance];
}

RL_ANIMATION_TABLE GetIdleAnimationTable(RL_ADVANCE nAdvance, RL_FIGHT_FLAG nFightFlag, BOOL bSheathFlag, BOOL bSlip)
{
	RL_ANIMATION_TABLE nAnimationTable = RL_ANIMATION_TABLE_NONE;

	if (bSlip)
	{
		nAnimationTable = GetSlipAnimationTable(nAdvance);
	}
	else
	{
		switch (nFightFlag)
		{
		case RL_FIGHT_FLAG_NONE:
			if (bSheathFlag)
				nAnimationTable = RL_ANIMATION_TABLE_IDLE;
			else
				nAnimationTable = RL_ANIMATION_TABLE_UNSHEATH_IDLE;
			break;
		case RL_FIGHT_FLAG_FIGHT:
			nAnimationTable = RL_ANIMATION_TABLE_FIGHT_IDLE;
			break;
		default:
			ASSERT(0);
			break;
		}
	}

	return nAnimationTable;
}

RL_ANIMATION_TABLE GetWalkAnimationTable(RL_ADVANCE nAdvance, RL_FIGHT_FLAG nFightFlag)
{
	static RL_ANIMATION_TABLE const s_aModelAnimationWalk[RL_FIGHT_FLAG_COUNT][RL_ADVANCE_COUNT] =
	{
		{ 
			RL_ANIMATION_TABLE_WALK_FORWARD, 
				RL_ANIMATION_TABLE_WALK_FORWARD,
				RL_ANIMATION_TABLE_WALK_FORWARD, 
				RL_ANIMATION_TABLE_WALK_BACKWARD, 
		},

		{ 
			RL_ANIMATION_TABLE_WALK_FIGHT_FORWARD, 
				RL_ANIMATION_TABLE_WALK_FIGHT_FORWARD,
				RL_ANIMATION_TABLE_WALK_FIGHT_FORWARD,
				RL_ANIMATION_TABLE_WALK_FIGHT_BACKWARD, 
			},
	};

	return s_aModelAnimationWalk[nFightFlag][nAdvance];
}

RL_ANIMATION_TABLE GetRunAnimationTable(RL_ADVANCE nAdvance, RL_FIGHT_FLAG nFightFlag)
{
	static RL_ANIMATION_TABLE const s_nAnimationTable[RL_FIGHT_FLAG_COUNT][RL_ADVANCE_COUNT] =
	{
		{ 
			RL_ANIMATION_TABLE_RUN_FORWARD, 
				RL_ANIMATION_TABLE_RUN_FORWARD,
				RL_ANIMATION_TABLE_RUN_FORWARD, 
				RL_ANIMATION_TABLE_RUN_BACKWARD, 
		},

		{
			RL_ANIMATION_TABLE_RUN_FIGHT_FORWARD, 
				RL_ANIMATION_TABLE_RUN_FIGHT_FORWARD,
				RL_ANIMATION_TABLE_RUN_FIGHT_FORWARD, 
				RL_ANIMATION_TABLE_RUN_FIGHT_BACKWARD, 
			},
	};

	return s_nAnimationTable[nFightFlag][nAdvance];
}

RL_ANIMATION_TABLE GetSheathAnimationTable(RL_SHEATH nSheathType, BOOL bSheathFlag, int nPhase)
{
	static RL_ANIMATION_TABLE const s_aSheathAnimation[2][2] = 
	{
		{ RL_ANIMATION_TABLE_BEGIN_SHEATH_SIDE,     RL_ANIMATION_TABLE_END_SHEATH_SIDE },
		{ RL_ANIMATION_TABLE_BEGIN_SHEATH_BACK,     RL_ANIMATION_TABLE_END_SHEATH_BACK },
	};

	static RL_ANIMATION_TABLE const s_aUnsheathAnimation[2][2] = 
	{        
		{ RL_ANIMATION_TABLE_BEGIN_UNSHEATH_SIDE,   RL_ANIMATION_TABLE_END_UNSHEATH_SIDE },
		{ RL_ANIMATION_TABLE_BEGIN_UNSHEATH_BACK,   RL_ANIMATION_TABLE_END_UNSHEATH_BACK },
	};

	RL_ANIMATION_TABLE nAnimationTable = RL_ANIMATION_TABLE_NONE;

	if (bSheathFlag)
		nAnimationTable = s_aSheathAnimation[nSheathType % 2][nPhase];
	else
		nAnimationTable = s_aUnsheathAnimation[nSheathType % 2][nPhase];

	return nAnimationTable;
}

RL_ANIMATION_TABLE GetKnowDownIdleBeHitAnimationTable(RL_ANIMATION_TABLE nAnimationTable)
{
	switch (nAnimationTable)
	{
	case RL_ANIMATION_TABLE_NONE:

	case RL_ANIMATION_TABLE_SLIP_FORWARD:
	case RL_ANIMATION_TABLE_SLIP_BACKWARD:
	case RL_ANIMATION_TABLE_SLIP_LEFT:
	case RL_ANIMATION_TABLE_SLIP_RIGHT:

	case RL_ANIMATION_TABLE_SIT_UP:
	case RL_ANIMATION_TABLE_SIT_DOWN:
	case RL_ANIMATION_TABLE_DEATH:
	case RL_ANIMATION_TABLE_RISE:
	case RL_ANIMATION_TABLE_KNOCK_DOWN:
	case RL_ANIMATION_TABLE_KNOCK_DOWN_IDLE:
	case RL_ANIMATION_TABLE_KNOCK_DOWN_BE_HIT:
	case RL_ANIMATION_TABLE_KNOCK_BACK:
	case RL_ANIMATION_TABLE_KNOCK_OFF:
	case RL_ANIMATION_TABLE_HALT:
	case RL_ANIMATION_TABLE_FREEZE:
	case RL_ANIMATION_TABLE_ENTRAP:
	case RL_ANIMATION_TABLE_PULL:

	case RL_ANIMATION_TABLE_DASH_FORWARD:
	case RL_ANIMATION_TABLE_DASH_BACKWARD:
	case RL_ANIMATION_TABLE_DASH_LEFT:
	case RL_ANIMATION_TABLE_DASH_RIGHT:

	case RL_ANIMATION_TABLE_DASH_FORWARD_IDLE:
	case RL_ANIMATION_TABLE_DASH_BACKWARD_IDLE:
	case RL_ANIMATION_TABLE_DASH_LEFT_IDLE:
	case RL_ANIMATION_TABLE_DASH_RIGHT_IDLE:

	case RL_ANIMATION_TABLE_REPULSED_FORWARD:
	case RL_ANIMATION_TABLE_REPULSED_BACKWARD:
	case RL_ANIMATION_TABLE_REPULSED_LEFT:
	case RL_ANIMATION_TABLE_REPULSED_RIGHT:

	case RL_ANIMATION_TABLE_SKID_FORWARD:
	case RL_ANIMATION_TABLE_SKID_BACKWARD:
	case RL_ANIMATION_TABLE_SKID_LEFT:
	case RL_ANIMATION_TABLE_SKID_RIGHT:

	case RL_ANIMATION_TABLE_TURN_LEFT:
	case RL_ANIMATION_TABLE_TURN_RIGHT:

		// 跳跃
	case RL_ANIMATION_TABLE_BEGIN_JUMP:
	case RL_ANIMATION_TABLE_JUMPING:
	case RL_ANIMATION_TABLE_END_JUMP_IDLE:
	case RL_ANIMATION_TABLE_BEGIN_DOUBLE_JUMP:
	case RL_ANIMATION_TABLE_DOUBLE_JUMPING:
	case RL_ANIMATION_TABLE_END_DOUBLE_JUMP_IDLE:

		// 游泳中跳跃
	case RL_ANIMATION_TABLE_BEGIN_SWIM_JUMP:
	case RL_ANIMATION_TABLE_SWIM_JUMPING:
	case RL_ANIMATION_TABLE_END_SWIM_JUMP_IDLE:
	case RL_ANIMATION_TABLE_BEGIN_SWIM_DOUBLE_JUMP:
	case RL_ANIMATION_TABLE_SWIM_DOUBLE_JUMPING:
	case RL_ANIMATION_TABLE_END_SWIM_DOUBLE_JUMP_IDLE:

		// 插拔武器
	case RL_ANIMATION_TABLE_BEGIN_SHEATH_BACK:
	case RL_ANIMATION_TABLE_BEGIN_SHEATH_SIDE:
	case RL_ANIMATION_TABLE_BEGIN_UNSHEATH_BACK:
	case RL_ANIMATION_TABLE_BEGIN_UNSHEATH_SIDE:
	case RL_ANIMATION_TABLE_END_SHEATH_BACK:
	case RL_ANIMATION_TABLE_END_SHEATH_SIDE:
	case RL_ANIMATION_TABLE_END_UNSHEATH_BACK:
	case RL_ANIMATION_TABLE_END_UNSHEATH_SIDE:

		// 待机
	case RL_ANIMATION_TABLE_IDLE:
	case RL_ANIMATION_TABLE_UNSHEATH_IDLE:
	case RL_ANIMATION_TABLE_FIGHT_IDLE:

		// 移动
	case RL_ANIMATION_TABLE_SWIM_STAND:
	case RL_ANIMATION_TABLE_SWIM_FORWARD:
	case RL_ANIMATION_TABLE_SWIM_BACKWARD:
	case RL_ANIMATION_TABLE_SWIM_LEFT:
	case RL_ANIMATION_TABLE_SWIM_RIGHT:
	case RL_ANIMATION_TABLE_WALK_FORWARD:
	case RL_ANIMATION_TABLE_WALK_BACKWARD:
	case RL_ANIMATION_TABLE_WALK_FIGHT_FORWARD:
	case RL_ANIMATION_TABLE_WALK_FIGHT_BACKWARD:
	case RL_ANIMATION_TABLE_RUN_FORWARD:
	case RL_ANIMATION_TABLE_RUN_BACKWARD:
	case RL_ANIMATION_TABLE_RUN_FIGHT_FORWARD:
	case RL_ANIMATION_TABLE_RUN_FIGHT_BACKWARD:
		ASSERT(0);
		nAnimationTable = RL_ANIMATION_TABLE_KNOCK_DOWN_IDLE;
		break;

		// 被击
	case RL_ANIMATION_TABLE_PHYSICS_BLOCK:
	case RL_ANIMATION_TABLE_SOLAR_MAGIC_BLOCK:
	case RL_ANIMATION_TABLE_NEUTRAL_MAGIC_BLOCK:
	case RL_ANIMATION_TABLE_LUNAR_MAGIC_BLOCK:
	case RL_ANIMATION_TABLE_POISON_BLOCK:
	case RL_ANIMATION_TABLE_BE_HIT:
		nAnimationTable = RL_ANIMATION_TABLE_KNOCK_DOWN_BE_HIT;
		break;
	case RL_ANIMATION_TABLE_DODGE:
		nAnimationTable = RL_ANIMATION_TABLE_KNOCK_DOWN_IDLE;
		break;
	case RL_ANIMATION_TABLE_PARRY:
	case RL_ANIMATION_TABLE_CRITICAL_STRIKE:
	case RL_ANIMATION_TABLE_PHYSICS_DAMAGE:
	case RL_ANIMATION_TABLE_SOLAR_MAGIC_DAMAGE:
	case RL_ANIMATION_TABLE_NEUTRAL_MAGIC_DAMAGE:
	case RL_ANIMATION_TABLE_LUNAR_MAGIC_DAMAGE:
	case RL_ANIMATION_TABLE_POISON_DAMAGE:
		nAnimationTable = RL_ANIMATION_TABLE_KNOCK_DOWN_BE_HIT;
		break;
	case RL_ANIMATION_TABLE_REFLECT_DAMAGE:
	case RL_ANIMATION_TABLE_HEAL:
	case RL_ANIMATION_TABLE_STEAL_LIFE:
	case RL_ANIMATION_TABLE_ABSORB_DAMAGE:
	case RL_ANIMATION_TABLE_SHIELD_DAMAGE:
		nAnimationTable = RL_ANIMATION_TABLE_KNOCK_DOWN_IDLE;
		break;
	default:
		ASSERT(0);
		nAnimationTable = RL_ANIMATION_TABLE_KNOCK_DOWN_IDLE;
		break;
	}

	return nAnimationTable;
}

RL_ANIMATION_TABLE GetAnimationTable(KRLAnimationTableParam const& Param)
{
	// TODO: outer table driven

	RL_ANIMATION_TABLE nAnimationTable = RL_ANIMATION_TABLE_NONE;

	switch (Param.nDownType)
	{
	case RL_ANIMATION_DOWN_NONE:
	case RL_ANIMATION_DOWN_OTACTION_SKILL_PREPARE:
	case RL_ANIMATION_DOWN_OTACTION_SKILL_CHANNEL:
	case RL_ANIMATION_DOWN_OTACTION_RECIPE_PREPARE:
	case RL_ANIMATION_DOWN_OTACTION_PICK_PREPARE:
	case RL_ANIMATION_DOWN_OTACTION_PICKING:
	case RL_ANIMATION_DOWN_OTACTION_ITEM_SKILL:
	case RL_ANIMATION_DOWN_OTACTION_CUSTOM_PREPARE:
	case RL_ANIMATION_DOWN_OTACTION_CUSTOM_CHANNEL:
		return RL_ANIMATION_TABLE_NONE;
	case RL_ANIMATION_DOWN_STAND:
		return GetIdleAnimationTable(Param.nAdvance, Param.nFightFlag, Param.bSheathFlag, Param.bSlip);
	case RL_ANIMATION_DOWN_TURN_LEFT:
		return RL_ANIMATION_TABLE_TURN_LEFT;
	case RL_ANIMATION_DOWN_TURN_RIGHT:
		return RL_ANIMATION_TABLE_TURN_RIGHT;
	case RL_ANIMATION_DOWN_WALK:
		return GetWalkAnimationTable(Param.nAdvance, Param.nFightFlag);
	case RL_ANIMATION_DOWN_RUN:
		return GetRunAnimationTable(Param.nAdvance, Param.nFightFlag);
	case RL_ANIMATION_DOWN_SWIM:
		return GetSwimAnimationTable(Param.nAdvance);
	case RL_ANIMATION_DOWN_FLOAT:
		return RL_ANIMATION_TABLE_SWIM_STAND;
	case RL_ANIMATION_DOWN_JUMP:
		return RL_ANIMATION_TABLE_JUMPING;
	case RL_ANIMATION_DOWN_DOUBLE_JUMP:
		return RL_ANIMATION_TABLE_DOUBLE_JUMPING;
	case RL_ANIMATION_DOWN_SWIM_JUMP:
		return RL_ANIMATION_TABLE_SWIM_JUMPING;
	case RL_ANIMATION_DOWN_SWIM_DOUBLE_JUMP:
		return RL_ANIMATION_TABLE_SWIM_DOUBLE_JUMPING;
	case RL_ANIMATION_DOWN_SIT_DOWN:
		return RL_ANIMATION_TABLE_SIT_DOWN;
	case RL_ANIMATION_DOWN_DEATH:
		return RL_ANIMATION_TABLE_DEATH;
	case RL_ANIMATION_DOWN_DASH:
		return GetDashAnimationTable(Param.nAdvance);
	case RL_ANIMATION_DOWN_KNOCK_DOWN:
		return RL_ANIMATION_TABLE_KNOCK_DOWN_IDLE;
	case RL_ANIMATION_DOWN_KNOCK_BACK:
		return RL_ANIMATION_TABLE_KNOCK_BACK;
	case RL_ANIMATION_DOWN_KNOCK_OFF:
		return RL_ANIMATION_TABLE_KNOCK_OFF;
	case RL_ANIMATION_DOWN_HALT:
		return RL_ANIMATION_TABLE_HALT;
	case RL_ANIMATION_DOWN_FREEZE:
		return RL_ANIMATION_TABLE_FREEZE;
	case RL_ANIMATION_DOWN_ENTRAP:
		return RL_ANIMATION_TABLE_ENTRAP;
	case RL_ANIMATION_DOWN_AUTOFLY:
		ASSERT(0);
		return RL_ANIMATION_TABLE_NONE;
	case RL_ANIMATION_DOWN_PULL:
		return RL_ANIMATION_TABLE_PULL;
	case RL_ANIMATION_DOWN_REPULSED:
		return GetRepulsedAnimationTable(Param.nAdvance);
	case RL_ANIMATION_DOWN_RISE:
		return RL_ANIMATION_TABLE_RISE;
	case RL_ANIMATION_DOWN_SKID:
		return GetSkidAnimationTable(Param.nAdvance);
	default:
		ASSERT(0);
		return RL_ANIMATION_TABLE_NONE;
	}
}

RL_ANIMATION_TABLE GetStateInAnimationTable(RL_ANIMATION_DOWN nCurrentAnimationDown, RL_ANIMATION_DOWN nPreviousAnimationDown, RL_ADVANCE nAdvance)
{
	RL_ANIMATION_TABLE nAnimationTable = RL_ANIMATION_TABLE_NONE;

	switch (nCurrentAnimationDown)
	{
	case RL_ANIMATION_DOWN_UNKNOWN:

	case RL_ANIMATION_DOWN_NONE:
	case RL_ANIMATION_DOWN_STAND:
	case RL_ANIMATION_DOWN_TURN_LEFT:
	case RL_ANIMATION_DOWN_TURN_RIGHT:
	case RL_ANIMATION_DOWN_WALK:
	case RL_ANIMATION_DOWN_RUN:
	case RL_ANIMATION_DOWN_SWIM:
		break;
	case RL_ANIMATION_DOWN_SWIM_JUMP:
		if (nPreviousAnimationDown != RL_ANIMATION_DOWN_DASH)
			nAnimationTable = RL_ANIMATION_TABLE_BEGIN_SWIM_JUMP;
		break;
	case RL_ANIMATION_DOWN_SWIM_DOUBLE_JUMP:
		if (nPreviousAnimationDown != RL_ANIMATION_DOWN_DASH)
			nAnimationTable = RL_ANIMATION_TABLE_BEGIN_SWIM_DOUBLE_JUMP;
		break;
	case RL_ANIMATION_DOWN_FLOAT:
		break;
	case RL_ANIMATION_DOWN_JUMP:
		if (nPreviousAnimationDown != RL_ANIMATION_DOWN_DASH)
			nAnimationTable = RL_ANIMATION_TABLE_BEGIN_SWIM_JUMP;
		break;
	case RL_ANIMATION_DOWN_DOUBLE_JUMP:
		if (nPreviousAnimationDown != RL_ANIMATION_DOWN_DASH)
			nAnimationTable = RL_ANIMATION_TABLE_BEGIN_DOUBLE_JUMP;
		break;
	case RL_ANIMATION_DOWN_SIT_DOWN:
		nAnimationTable = RL_ANIMATION_TABLE_SIT_DOWN;
		break;
	case RL_ANIMATION_DOWN_DEATH:
		nAnimationTable = RL_ANIMATION_TABLE_DEATH;
		break;
	case RL_ANIMATION_DOWN_DASH:
		nAnimationTable = GetDashAnimationTable(nAdvance);
		break;
	case RL_ANIMATION_DOWN_KNOCK_DOWN:
		nAnimationTable = RL_ANIMATION_TABLE_KNOCK_DOWN;
		break;
	case RL_ANIMATION_DOWN_KNOCK_BACK:
		nAnimationTable = RL_ANIMATION_TABLE_KNOCK_BACK;
		break;
	case RL_ANIMATION_DOWN_KNOCK_OFF:
		nAnimationTable = RL_ANIMATION_TABLE_KNOCK_OFF;
		break;
	case RL_ANIMATION_DOWN_HALT:
	case RL_ANIMATION_DOWN_FREEZE:
	case RL_ANIMATION_DOWN_ENTRAP:
	case RL_ANIMATION_DOWN_AUTOFLY:
	case RL_ANIMATION_DOWN_PULL:
		break;
	case RL_ANIMATION_DOWN_REPULSED:
		nAnimationTable = GetRepulsedAnimationTable(nAdvance);
		break;
	case RL_ANIMATION_DOWN_RISE:
		nAnimationTable = RL_ANIMATION_TABLE_RISE;
		break;
	case RL_ANIMATION_DOWN_SKID:
		nAnimationTable = GetSkidAnimationTable(nAdvance);
		break;
	case RL_ANIMATION_DOWN_OTACTION_SKILL_PREPARE:
	case RL_ANIMATION_DOWN_OTACTION_SKILL_CHANNEL:
	case RL_ANIMATION_DOWN_OTACTION_RECIPE_PREPARE:
	case RL_ANIMATION_DOWN_OTACTION_PICK_PREPARE:
	case RL_ANIMATION_DOWN_OTACTION_PICKING:
	case RL_ANIMATION_DOWN_OTACTION_ITEM_SKILL:
	case RL_ANIMATION_DOWN_OTACTION_CUSTOM_PREPARE:
	case RL_ANIMATION_DOWN_OTACTION_CUSTOM_CHANNEL:
		break;
	default:
		ASSERT(0);
	};

	return nAnimationTable;
}

RL_ANIMATION_TABLE GetStateOutAnimationTable(RL_ANIMATION_DOWN nCurrentAnimationDown, RL_ANIMATION_DOWN nNextAnimationDown, RL_ADVANCE nAdvance)
{
	RL_ANIMATION_TABLE nAnimationTable = RL_ANIMATION_TABLE_NONE;

	switch (nCurrentAnimationDown)
	{
	case RL_ANIMATION_DOWN_UNKNOWN:

	case RL_ANIMATION_DOWN_NONE:
	case RL_ANIMATION_DOWN_STAND:
	case RL_ANIMATION_DOWN_TURN_LEFT:
	case RL_ANIMATION_DOWN_TURN_RIGHT:
	case RL_ANIMATION_DOWN_WALK:
	case RL_ANIMATION_DOWN_RUN:
	case RL_ANIMATION_DOWN_SWIM:
		break;
	case RL_ANIMATION_DOWN_SWIM_JUMP:
	case RL_ANIMATION_DOWN_SWIM_DOUBLE_JUMP:
	case RL_ANIMATION_DOWN_FLOAT:
		break;
	case RL_ANIMATION_DOWN_JUMP:
	case RL_ANIMATION_DOWN_DOUBLE_JUMP:
		if (nNextAnimationDown == RL_ANIMATION_DOWN_SWIM || nNextAnimationDown == RL_ANIMATION_DOWN_FLOAT)
			nAnimationTable = RL_ANIMATION_TABLE_NONE;
		else if (nCurrentAnimationDown == RL_ANIMATION_DOWN_JUMP && nNextAnimationDown == RL_ANIMATION_DOWN_DOUBLE_JUMP)
			nAnimationTable = RL_ANIMATION_TABLE_NONE;
		else
			nAnimationTable = RL_ANIMATION_TABLE_END_JUMP_IDLE;
		break;
	case RL_ANIMATION_DOWN_SIT_DOWN:
		nAnimationTable = RL_ANIMATION_TABLE_SIT_UP;
		break;
	case RL_ANIMATION_DOWN_DEATH:
		nAnimationTable = RL_ANIMATION_TABLE_RISE;
		break;
	case RL_ANIMATION_DOWN_DASH:
		nAnimationTable = GetDashIdleAnimationTable(nAdvance);
		break;
	case RL_ANIMATION_DOWN_KNOCK_DOWN:
	case RL_ANIMATION_DOWN_KNOCK_BACK:
	case RL_ANIMATION_DOWN_KNOCK_OFF:
	case RL_ANIMATION_DOWN_HALT:
	case RL_ANIMATION_DOWN_FREEZE:
	case RL_ANIMATION_DOWN_ENTRAP:
	case RL_ANIMATION_DOWN_AUTOFLY:
	case RL_ANIMATION_DOWN_PULL:
	case RL_ANIMATION_DOWN_REPULSED:
	case RL_ANIMATION_DOWN_RISE:
	case RL_ANIMATION_DOWN_SKID:
	case RL_ANIMATION_DOWN_OTACTION_SKILL_PREPARE:
	case RL_ANIMATION_DOWN_OTACTION_SKILL_CHANNEL:
	case RL_ANIMATION_DOWN_OTACTION_RECIPE_PREPARE:
	case RL_ANIMATION_DOWN_OTACTION_PICK_PREPARE:
	case RL_ANIMATION_DOWN_OTACTION_PICKING:
	case RL_ANIMATION_DOWN_OTACTION_ITEM_SKILL:
	case RL_ANIMATION_DOWN_OTACTION_CUSTOM_PREPARE:
	case RL_ANIMATION_DOWN_OTACTION_CUSTOM_CHANNEL:
		break;
	default:
		ASSERT(0);
	};

	return nAnimationTable;
}

float GetAnimationSpeed(RL_ANIMATION_DOWN nDownType, RL_ADVANCE nAdvance, int nVelocityXY, BOOL bRide)
{
	float fSpeed = 1.0f;

	switch (nDownType)
	{
	case RL_ANIMATION_DOWN_SWIM:
		if (nAdvance == RL_ADVANCE_BACKWARD)
			fSpeed = (float)nVelocityXY / (g_pRL->m_TableList.GetNumber(RL_NUMBER_CHARACTER_SWIM_SPEED) / 2.0f);
		else
			fSpeed = (float)nVelocityXY / g_pRL->m_TableList.GetNumber(RL_NUMBER_CHARACTER_SWIM_SPEED);
		break;
	case RL_ANIMATION_DOWN_WALK:
		if (bRide)
			fSpeed = (float)nVelocityXY / g_pRL->m_TableList.GetNumber(RL_NUMBER_CHARACTER_RIDE_WALK_SPEED);
		else
			fSpeed = (float)nVelocityXY / g_pRL->m_TableList.GetNumber(RL_NUMBER_CHARACTER_WALK_SPEED);
		break;
	case RL_ANIMATION_DOWN_RUN:
		if (bRide)
		{
			if ((float)nVelocityXY >= g_pRL->m_TableList.GetNumber(RL_NUMBER_CHARACTER_RIDE_RUN_FASTER_SPEED))
			{
				if (nAdvance == RL_ADVANCE_BACKWARD)
					fSpeed = (float)nVelocityXY / (g_pRL->m_TableList.GetNumber(RL_NUMBER_CHARACTER_RIDE_RUN_FASTER_SPEED) / 2.0f);
				else
					fSpeed = (float)nVelocityXY / g_pRL->m_TableList.GetNumber(RL_NUMBER_CHARACTER_RIDE_RUN_FASTER_SPEED);
			}
			else
			{
				if (nAdvance == RL_ADVANCE_BACKWARD)
					fSpeed = (float)nVelocityXY / (g_pRL->m_TableList.GetNumber(RL_NUMBER_CHARACTER_RIDE_RUN_SPEED) / 2.0f);
				else
					fSpeed = (float)nVelocityXY / g_pRL->m_TableList.GetNumber(RL_NUMBER_CHARACTER_RIDE_RUN_SPEED);
			}
		}
		else
		{
			if (nAdvance == RL_ADVANCE_BACKWARD)
				fSpeed = (float)nVelocityXY / (g_pRL->m_TableList.GetNumber(RL_NUMBER_CHARACTER_RUN_SPEED) / 2.0f);
			else
				fSpeed = (float)nVelocityXY / g_pRL->m_TableList.GetNumber(RL_NUMBER_CHARACTER_RUN_SPEED);
		}
		break;
	default:
		fSpeed = 1.0f;
		break;
	}

	if (abs(fSpeed) <= FLT_EPSILON)
		fSpeed = 1.0f;

	return fSpeed;
}

float GetYawTurnSpeed(BOOL bRide)
{
	float fSpeed = 0.0f;

	if (bRide)
		fSpeed = g_pRL->m_TableList.GetNumber(RL_NUMBER_CHARACTER_RIDE_YAW_TURN_SPEED);
	else
		fSpeed = g_pRL->m_TableList.GetNumber(RL_NUMBER_CHARACTER_YAW_TURN_SPEED);

	return fSpeed;
}

float GetYawTurnResetSpeed(BOOL bRide)
{
	float fSpeed = 0.0f;

	if (bRide)
		fSpeed = g_pRL->m_TableList.GetNumber(RL_NUMBER_CHARACTER_RIDE_YAW_TURN_RESET_SPEED);
	else
		fSpeed = g_pRL->m_TableList.GetNumber(RL_NUMBER_CHARACTER_YAW_TURN_RESET_SPEED);

	return fSpeed;
}

LPCTSTR GetAnimationDownName(RL_ANIMATION_DOWN nAnimationDown)
{
	switch (nAnimationDown)
	{
	case RL_ANIMATION_DOWN_UNKNOWN:
		return _T("unknown");
	case RL_ANIMATION_DOWN_NONE:
		return _T("none");
	case RL_ANIMATION_DOWN_STAND:
		return _T("stand");
	case RL_ANIMATION_DOWN_TURN_LEFT:
		return _T("turn left");
	case RL_ANIMATION_DOWN_TURN_RIGHT:
		return _T("turn right");
	case RL_ANIMATION_DOWN_WALK:
		return _T("walk");
	case RL_ANIMATION_DOWN_RUN:
		return _T("run");
	case RL_ANIMATION_DOWN_SWIM:
		return _T("swim");
	case RL_ANIMATION_DOWN_SWIM_JUMP:
		return _T("swim jump");
	case RL_ANIMATION_DOWN_SWIM_DOUBLE_JUMP:
		return _T("swim double jump");
	case RL_ANIMATION_DOWN_FLOAT:
		return _T("float");
	case RL_ANIMATION_DOWN_JUMP:
		return _T("jump");
	case RL_ANIMATION_DOWN_DOUBLE_JUMP:
		return _T("double jump");
	case RL_ANIMATION_DOWN_SIT_DOWN:
		return _T("sit down");
	case RL_ANIMATION_DOWN_DEATH:
		return _T("death");
	case RL_ANIMATION_DOWN_DASH:
		return _T("dash");
	case RL_ANIMATION_DOWN_KNOCK_DOWN:
		return _T("knock down");
	case RL_ANIMATION_DOWN_KNOCK_BACK:
		return _T("knock back");
	case RL_ANIMATION_DOWN_KNOCK_OFF:
		return _T("knock off");
	case RL_ANIMATION_DOWN_HALT:
		return _T("halt");
	case RL_ANIMATION_DOWN_FREEZE:
		return _T("freeze");
	case RL_ANIMATION_DOWN_ENTRAP:
		return _T("entrap");
	case RL_ANIMATION_DOWN_AUTOFLY:
		return _T("autofly");
	case RL_ANIMATION_DOWN_PULL:
		return _T("pull");
	case RL_ANIMATION_DOWN_REPULSED:
		return _T("repulsed");
	case RL_ANIMATION_DOWN_RISE:
		return _T("rise");
	case RL_ANIMATION_DOWN_SKID:
		return _T("skid");
	case RL_ANIMATION_DOWN_OTACTION_SKILL_PREPARE:
		return _T("skill prepare");
	case RL_ANIMATION_DOWN_OTACTION_SKILL_CHANNEL:
		return _T("skill channel");
	case RL_ANIMATION_DOWN_OTACTION_RECIPE_PREPARE:
		return _T("recipe prepare");
	case RL_ANIMATION_DOWN_OTACTION_PICK_PREPARE:
		return _T("pick prepare");
	case RL_ANIMATION_DOWN_OTACTION_PICKING:
		return _T("picking");
	case RL_ANIMATION_DOWN_OTACTION_ITEM_SKILL:
		return _T("item skill");
	case RL_ANIMATION_DOWN_OTACTION_CUSTOM_PREPARE:
		return _T("custom prepare");
	case RL_ANIMATION_DOWN_OTACTION_CUSTOM_CHANNEL:
		return _T("custom channel");
	default:
		return _T("error");
	}
}

LPCTSTR GetAnimationUpName(RL_ANIMATION_UP nAnimationUp)
{
	switch (nAnimationUp)
	{
	case RL_ANIMATION_UP_UNKNOWN:
		return _T("unknown");
	case RL_ANIMATION_UP_NONE:
		return _T("none");
	case RL_ANIMATION_UP_SKILL_PREPARE:
		return _T("skill prepare");
	case RL_ANIMATION_UP_SKILL_CHANNEL:
		return _T("skill channel");
	case RL_ANIMATION_UP_RECIPE_PREPARE:
		return _T("recipe prepare");
	case RL_ANIMATION_UP_PICK_PREPARE:
		return _T("pick prepare");
	case RL_ANIMATION_UP_OPEN_DOODAD:
		return _T("open doodad");
	case RL_ANIMATION_UP_BEGIN_PICK_DOODAD:
		return _T("begin pick doodad");
	case RL_ANIMATION_UP_PICKING_DOODAD:
		return _T("picking doodad");
	case RL_ANIMATION_UP_END_PICK_DOODAD:
		return _T("end pick doodad");
	case RL_ANIMATION_UP_ITEM_SKILL:
		return _T("item skill");
	case RL_ANIMATION_UP_CUSTOM_PREPARE:
		return _T("custom prepare");
	case RL_ANIMATION_UP_CUSTOM_CHANNEL:
		return _T("custom channel");
	case RL_ANIMATION_UP_BEGIN_SHEATH:
		return _T("begin sheath");
	case RL_ANIMATION_UP_END_SHEATH:
		return _T("end sheath");
	case RL_ANIMATION_UP_BEGIN_UNSHEATH:
		return _T("begin unsheath");
	case RL_ANIMATION_UP_END_UNSHEATH:
		return _T("end unsheath");
	case RL_ANIMATION_UP_DODGE:
		return _T("dodge");
	case RL_ANIMATION_UP_BEHIT:
		return _T("behit");
	case RL_ANIMATION_UP_SERIAL_BEGIN:
		return _T("serial begin");
	case RL_ANIMATION_UP_SERIAL_PROCESSING:
		return _T("serial processing");
	case RL_ANIMATION_UP_SERIAL_END:
		return _T("serial end");
	default:
		return _T("error");
	}
}

RL_ADVANCE GetAdvance(RL_ADVANCE_TYPE nAdvanceType, float fFootFaceDeltaYaw)
{
	enum
	{
		BIAS_MIN,
		BIAS_MAX,

		BIAS_COUNT,
	};

	static float const ADVANCE_BIAS = D3DX_PI / 32.0f;

	static float s_fBias[RL_ADVANCE_TYPE_COUNT][RL_ADVANCE_COUNT][BIAS_COUNT] =
	{   
		// 前后区间大(normal)
		{
			{ -D3DX_PI * 0.75f + ADVANCE_BIAS, -D3DX_PI * 0.25f - ADVANCE_BIAS }, //RL_ADVANCE_STRAFE_LEFT
			{  D3DX_PI * 0.25f + ADVANCE_BIAS,  D3DX_PI * 0.75f - ADVANCE_BIAS }, //RL_ADVANCE_STRAFE_RIGHT
			{ -D3DX_PI * 0.25f - ADVANCE_BIAS,  D3DX_PI * 0.25f + ADVANCE_BIAS }, //RL_ADVANCE_FORWARD
			{ 0.0f, 0.0f },                                                       //RL_ADVANCE_BACKWARD
		},

		// 左右区间大(Swim)
		{
			{ -D3DX_PI * 0.75f - ADVANCE_BIAS, -D3DX_PI * 0.25f + ADVANCE_BIAS }, //RL_ADVANCE_STRAFE_LEFT
			{  D3DX_PI * 0.25f - ADVANCE_BIAS,  D3DX_PI * 0.75f + ADVANCE_BIAS }, //RL_ADVANCE_STRAFE_RIGHT
			{ -D3DX_PI * 0.25f + ADVANCE_BIAS,  D3DX_PI * 0.25f - ADVANCE_BIAS }, //RL_ADVANCE_FORWARD
			{ 0.0f, 0.0f },                                                       //RL_ADVANCE_BACKWARD
			},
	};

	for (int nIndex = RL_ADVANCE_STRAFE_LEFT; nIndex < RL_ADVANCE_BACKWARD; ++nIndex)
	{
		if (fFootFaceDeltaYaw > s_fBias[nAdvanceType][nIndex][BIAS_MIN] && fFootFaceDeltaYaw <= s_fBias[nAdvanceType][nIndex][BIAS_MAX])
		{
			return (RL_ADVANCE)nIndex;
		}
	}

	return RL_ADVANCE_BACKWARD;
}

DWORD GetAnimationTrackID(enuAnimationJointType nJointType, DWORD dwAnimationKindID)
{
	DWORD dwAnimationTrackID = 0;
	RL_ANIMATION_TRACK nAnimationTrack = RL_ANIMATION_TRACK_DOWN;

	if (nJointType == ANI_JOINT_TOP)
		nAnimationTrack = RL_ANIMATION_TRACK_UP;
	else
		nAnimationTrack = RL_ANIMATION_TRACK_DOWN;

	// 轨道优先级
	dwAnimationTrackID = g_pRL->m_TableList.GetPlayerAnimationTrackType(dwAnimationKindID, nAnimationTrack);

	return dwAnimationTrackID;
}

static RL_ANIMATION_DOWN GetTurningAnimationDown(RL_TURNING nTurning)
{
	RL_ANIMATION_DOWN nAnimationDown = RL_ANIMATION_DOWN_UNKNOWN;

	switch (nTurning)
	{
	case RL_TURNING_STAND_NONE:
	case RL_TURNING_STAND_FACE_TURN_LEFT:
	case RL_TURNING_STAND_FACE_TURN_RIGHT:
		nAnimationDown = RL_ANIMATION_DOWN_STAND;
		break;
	case RL_TURNING_STAND_FOOT_TURN_LEFT:
		nAnimationDown = RL_ANIMATION_DOWN_TURN_LEFT;
		break;
	case RL_TURNING_STAND_FOOT_TURN_RIGHT:
		nAnimationDown = RL_ANIMATION_DOWN_TURN_RIGHT;
		break;
	case RL_TURNING_MOVING_NONE:
	case RL_TURNING_MOVING_FACE_TURN_LEFT:
	case RL_TURNING_MOVING_FACE_TURN_RIGHT:
		nAnimationDown = RL_ANIMATION_DOWN_NONE;
		break;
	default:
		ASSERT(0);
		break;
	}

	return nAnimationDown;
}

static RL_ANIMATION_DOWN GetRLAnimationDownFromOTAction(KOT_ACTION_TYPE nOTActionType)
{
	switch (nOTActionType)
	{
	case otActionIdle:
		return RL_ANIMATION_DOWN_NONE;
	case otActionSkillPrepare:
		return RL_ANIMATION_DOWN_OTACTION_SKILL_PREPARE;
	case otActionSkillChannel:
		return RL_ANIMATION_DOWN_OTACTION_SKILL_CHANNEL;
	case otActionRecipePrepare:
		return RL_ANIMATION_DOWN_OTACTION_RECIPE_PREPARE;
	case otActionPickPrepare:
		return RL_ANIMATION_DOWN_OTACTION_PICK_PREPARE;
	case otActionPicking:
		return RL_ANIMATION_DOWN_OTACTION_PICKING;
	case otActionUseItemPrepare:
		return RL_ANIMATION_DOWN_OTACTION_ITEM_SKILL;
	case otActionCustomPrepare:
		return RL_ANIMATION_DOWN_OTACTION_CUSTOM_PREPARE;
	case otActionCustomChannel:
		return RL_ANIMATION_DOWN_OTACTION_CUSTOM_CHANNEL;
	default:
		ASSERT(0);
		return RL_ANIMATION_DOWN_NONE;
	}
}

RL_ANIMATION_DOWN GetRLAnimationDown(CHARACTER_MOVE_STATE nMoveState, KOT_ACTION_TYPE nOTActionType, int nJumpCount, RL_TURNING nTurning)
{
	RL_ANIMATION_DOWN nAnimationDown = RL_ANIMATION_DOWN_NONE;

	switch (nMoveState)
	{
	case cmsOnStand:
		nAnimationDown = GetRLAnimationDownFromOTAction(nOTActionType);
		if (nAnimationDown == RL_ANIMATION_DOWN_NONE)
			nAnimationDown = GetTurningAnimationDown(nTurning);
		break;
	case cmsOnWalk:
		nAnimationDown = RL_ANIMATION_DOWN_WALK;
		break;
	case cmsOnRun:
		nAnimationDown = RL_ANIMATION_DOWN_RUN;
		break;
	case cmsOnSwim:
		nAnimationDown = RL_ANIMATION_DOWN_SWIM;
		break;
	case cmsOnSwimJump:
		if (nJumpCount > 1)
			nAnimationDown = RL_ANIMATION_DOWN_SWIM_DOUBLE_JUMP;
		else
			nAnimationDown = RL_ANIMATION_DOWN_SWIM_JUMP;
		break;
	case cmsOnFloat:
		nAnimationDown = GetRLAnimationDownFromOTAction(nOTActionType);
		if (nAnimationDown == RL_ANIMATION_DOWN_NONE)
			nAnimationDown = RL_ANIMATION_DOWN_FLOAT;
		break;
	case cmsOnJump:
		if (nJumpCount > 1)
			nAnimationDown = RL_ANIMATION_DOWN_DOUBLE_JUMP;
		else
			nAnimationDown = RL_ANIMATION_DOWN_JUMP;
		break;
	case cmsOnSit:
		nAnimationDown = RL_ANIMATION_DOWN_SIT_DOWN;
		break;
	case cmsOnDeath:
		nAnimationDown = RL_ANIMATION_DOWN_DEATH;
		break;
	case cmsOnDash:
		nAnimationDown = RL_ANIMATION_DOWN_DASH;
		break;
	case cmsOnKnockedDown:
		nAnimationDown = RL_ANIMATION_DOWN_KNOCK_DOWN;
		break;
	case cmsOnKnockedBack:
		nAnimationDown = RL_ANIMATION_DOWN_KNOCK_BACK;
		break;
	case cmsOnKnockedOff:
		nAnimationDown = RL_ANIMATION_DOWN_KNOCK_OFF;
		break;
	case cmsOnHalt:
		nAnimationDown = RL_ANIMATION_DOWN_HALT;
		break;
	case cmsOnFreeze:
		nAnimationDown = RL_ANIMATION_DOWN_FREEZE;
		break;
	case cmsOnEntrap:
		nAnimationDown = RL_ANIMATION_DOWN_ENTRAP;
		break;
	case cmsOnAutoFly:
		nAnimationDown = RL_ANIMATION_DOWN_AUTOFLY;
		break;
	case cmsOnPull:
		nAnimationDown = RL_ANIMATION_DOWN_PULL;
		break;
	case cmsOnRepulsed:
		nAnimationDown = RL_ANIMATION_DOWN_REPULSED;
		break;
	case cmsOnRise:
		nAnimationDown = RL_ANIMATION_DOWN_RISE;
		break;
	case cmsOnSkid:
		nAnimationDown = RL_ANIMATION_DOWN_SKID;
		break;
	default:
		ASSERT(0);
		nAnimationDown = RL_ANIMATION_DOWN_UNKNOWN;
		break;
	}

	return nAnimationDown;
}

RL_ANIMATION_UP GetRLAnimationUp(KOT_ACTION_TYPE nOTActionType)
{
	switch (nOTActionType)
	{
	case otActionIdle:
		return RL_ANIMATION_UP_NONE;
	case otActionSkillPrepare:
		return RL_ANIMATION_UP_SKILL_PREPARE;
	case otActionSkillChannel:
		return RL_ANIMATION_UP_SKILL_CHANNEL;
	case otActionRecipePrepare:
		return RL_ANIMATION_UP_RECIPE_PREPARE;
	case otActionPickPrepare:
		return RL_ANIMATION_UP_PICK_PREPARE;
	case otActionPicking:
		return RL_ANIMATION_UP_PICKING_DOODAD;
	case otActionUseItemPrepare:
		return RL_ANIMATION_UP_ITEM_SKILL;
	case otActionCustomPrepare:
		return RL_ANIMATION_UP_CUSTOM_PREPARE;
	case otActionCustomChannel:
		return RL_ANIMATION_UP_CUSTOM_CHANNEL;
	default:
		ASSERT(0);
		return RL_ANIMATION_UP_UNKNOWN;
	}
}

