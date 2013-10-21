#include "Stdafx.h"
#include "./ktabledefine.h"

// Npc
void KRLNpcModel::Init(KRLNpcModel* pData, size_t nCount)
{
    ASSERT(pData && nCount > 0);

    for (size_t nIndex = 0; nIndex < nCount; ++nIndex)
    {
        KRLNpcModel& data = pData[nIndex];

        // Idle
        for (size_t nAnimationIndex = 1; nAnimationIndex < RL_NPC_IDLE_ANIMATION_COUNT; ++nAnimationIndex)
        {
            data.IdleAnimation.dwRatio[nAnimationIndex] += data.IdleAnimation.dwRatio[nAnimationIndex - 1];
        }

        // Others
        for (int nKindID = 0; nKindID < _countof(data.NormalAnimations); ++nKindID)
        {
            for (size_t nAnimationIndex = 1; nAnimationIndex < RL_NPC_NORMAL_ANIMATION_COUNT; ++nAnimationIndex)
            {
                data.NormalAnimations[nKindID].dwRatio[nAnimationIndex] += data.NormalAnimations[nKindID].dwRatio[nAnimationIndex - 1];
            }
        }
    }
}

static DWORD GetIdleAnimationID(KRLNpcModel const* pData)
{
    DWORD dwAnimationID = 0;

    if (pData->IdleAnimation.dwRatio[RL_NPC_IDLE_ANIMATION_COUNT - 1] > 0)
    {
        DWORD dwRatio = KG_Rand() % pData->IdleAnimation.dwRatio[RL_NPC_IDLE_ANIMATION_COUNT - 1];

        for (size_t nIndex = 0; nIndex < RL_NPC_IDLE_ANIMATION_COUNT; ++nIndex)
        {
            if (dwRatio < pData->IdleAnimation.dwRatio[nIndex])
            {
                dwAnimationID = pData->IdleAnimation.dwAnimationID[nIndex];
                break;
            }
        }
    }
    return dwAnimationID;
}

static DWORD GetNormalAnimationID(KRLNpcModel const* pData, int nKindID)
{
    DWORD dwAnimationID = 0;

    if (pData->NormalAnimations[nKindID].dwRatio[RL_NPC_NORMAL_ANIMATION_COUNT - 1] > 0)
    {
        DWORD dwRatio = KG_Rand() % pData->NormalAnimations[nKindID].dwRatio[RL_NPC_NORMAL_ANIMATION_COUNT - 1];

        for (size_t nIndex = 0; nIndex < RL_NPC_NORMAL_ANIMATION_COUNT; ++nIndex)
        {
            if (dwRatio < pData->NormalAnimations[nKindID].dwRatio[nIndex])
            {
                dwAnimationID = pData->NormalAnimations[nKindID].dwAnimationID[nIndex];
                break;
            }
        }
    }

    return dwAnimationID;
}

DWORD KRLNpcModel::GetAnimationID(KRLNpcModel const* pData, RL_NPC_ANIMATION_TABLE nAnimationTable)
{
    DWORD dwAnimationID = 0;

    if (pData)
    {
        switch (nAnimationTable)
        {
        case RL_NPC_ANIMATION_TABLE_NONE:
            // Do Nothing
            break;
        case RL_NPC_ANIMATION_TABLE_IDLE:
            dwAnimationID = GetIdleAnimationID(pData);
            break;
        case RL_NPC_ANIMATION_TABLE_SIT_DOWN:
        case RL_NPC_ANIMATION_TABLE_SIT_UP:
        case RL_NPC_ANIMATION_TABLE_DEATH:
        case RL_NPC_ANIMATION_TABLE_RISE:
        case RL_NPC_ANIMATION_TABLE_KNOCK_DOWN:
        case RL_NPC_ANIMATION_TABLE_KNOCK_DOWN_IDLE:
        case RL_NPC_ANIMATION_TABLE_KNOCK_DOWN_BE_HIT:
        case RL_NPC_ANIMATION_TABLE_KNOCK_BACK:
        case RL_NPC_ANIMATION_TABLE_KNOCK_OFF:
        case RL_NPC_ANIMATION_TABLE_HALT:
        case RL_NPC_ANIMATION_TABLE_FREEZE:
        case RL_NPC_ANIMATION_TABLE_ENTRAP:
        case RL_NPC_ANIMATION_TABLE_PULL:
        case RL_NPC_ANIMATION_TABLE_DASH_FORWARD:
        case RL_NPC_ANIMATION_TABLE_DASH_BACKWARD:
        case RL_NPC_ANIMATION_TABLE_DASH_LEFT:
        case RL_NPC_ANIMATION_TABLE_DASH_RIGHT:
        case RL_NPC_ANIMATION_TABLE_DASH_FORWARD_IDLE:
        case RL_NPC_ANIMATION_TABLE_DASH_BACKWARD_IDLE:
        case RL_NPC_ANIMATION_TABLE_DASH_LEFT_IDLE:
        case RL_NPC_ANIMATION_TABLE_DASH_RIGHT_IDLE:
        case RL_NPC_ANIMATION_TABLE_REPULSED_FORWARD:
        case RL_NPC_ANIMATION_TABLE_REPULSED_BACKWARD:
        case RL_NPC_ANIMATION_TABLE_REPULSED_LEFT:
        case RL_NPC_ANIMATION_TABLE_REPULSED_RIGHT:
        case RL_NPC_ANIMATION_TABLE_SKID_FORWARD:
        case RL_NPC_ANIMATION_TABLE_SKID_BACKWARD:
        case RL_NPC_ANIMATION_TABLE_SKID_LEFT:
        case RL_NPC_ANIMATION_TABLE_SKID_RIGHT:
        case RL_NPC_ANIMATION_TABLE_TURN_LEFT:
        case RL_NPC_ANIMATION_TABLE_TURN_RIGHT:
        case RL_NPC_ANIMATION_TABLE_BEGIN_JUMP:
        case RL_NPC_ANIMATION_TABLE_JUMPING:
        case RL_NPC_ANIMATION_TABLE_END_JUMP_IDLE:
        case RL_NPC_ANIMATION_TABLE_BEGIN_DOUBLE_JUMP:
        case RL_NPC_ANIMATION_TABLE_DOUBLE_JUMPING:
        case RL_NPC_ANIMATION_TABLE_END_DOUBLE_JUMP_IDLE:
        case RL_NPC_ANIMATION_TABLE_FIGHT_IDLE:
        case RL_NPC_ANIMATION_TABLE_SWIM_STAND:
        case RL_NPC_ANIMATION_TABLE_SWIM_FORWARD:
        case RL_NPC_ANIMATION_TABLE_SWIM_BACKWARD:
        case RL_NPC_ANIMATION_TABLE_SWIM_LEFT:
        case RL_NPC_ANIMATION_TABLE_SWIM_RIGHT:
        case RL_NPC_ANIMATION_TABLE_WALK_FORWARD:
        case RL_NPC_ANIMATION_TABLE_WALK_BACKWARD:
        case RL_NPC_ANIMATION_TABLE_RUN_FORWARD:
        case RL_NPC_ANIMATION_TABLE_RUN_BACKWARD:
        case RL_NPC_ANIMATION_TABLE_BE_HIT:
        case RL_NPC_ANIMATION_TABLE_SKILL_DODGE:
        case RL_NPC_ANIMATION_TABLE_SKILL_PARRY:
            dwAnimationID = GetNormalAnimationID(pData, nAnimationTable - RL_NPC_ANIMATION_TABLE_SIT_DOWN);
            break;
        default:
            ASSERT(0);
            break;
        }
    }
    return dwAnimationID;
}
