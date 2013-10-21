#include "StdAfx.h"

#if defined(_SERVER)

#include "KAIController.h"
#include "KMath.h"
#include "KSO3World.h"
#include "KObjectManager.h"
#include "KWorldSettings.h"
#include "KSkill.h"

void KAIController::_Construct_Retardate()
{
    m_eAIState = aisIdle;
}

void KAIController::_Activate_Retardate()
{
    _Activate();
}

void KAIController::_OnStateIdle_Retardate()
{
}

void KAIController::_OnStatePatrol_Retardate()
{
}

void KAIController::_OnStateAlert_Retardate()
{
}

void KAIController::_OnStateFollow_Retardate()
{
}

void KAIController::_OnStatePursuit_Retardate()
{
}

void KAIController::_OnStateKeepAway_Retardate()
{
}


void KAIController::_OnStateEscape_Retardate()
{
}

void KAIController::_OnStateReturn_Retardate()
{
}


void KAIController::_OnEventAttacked_Retardate(KCharacter* pAttacker, int nDamage)
{
}

void KAIController::_OnEventPathResult_Retardate(BOOL)
{
}


#endif	//_SERVER

