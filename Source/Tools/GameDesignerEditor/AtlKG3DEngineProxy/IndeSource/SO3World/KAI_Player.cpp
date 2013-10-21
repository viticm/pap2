#include "stdafx.h"

#include "KAI_Player.h"
#include "KThreatList.h"
#include "KCharacter.h"
#include "KSO3World.h"

#if defined(_CLIENT) 
void KAI_Player::Activate()
{
	if (m_eAIType <= aitInvalid || m_eAIType >= aitTotal)
		return;

	if (g_pSO3World->m_nGameLoop < m_nNextActiveFrame)
		return;

	m_nNextActiveFrame = g_pSO3World->m_nGameLoop + g_pSO3World->m_Settings.m_ConstList.nAIInterval;

	switch (m_eAIState)
	{    
	case aisIdle:
		OnIdle();
		break;
	case aisWander:
		OnWander();
		break;
	case aisPatrol:
		OnPartol();
		break;
	case aisAlert:
		OnAlert();
		break;
	case aisFollow:
		OnFollow();
		break;
	case aisPursuit:
		OnPursuit();
		break;
	case aisKeepAway:
		OnKeepAway();
		break;
	case aisEscape:
		OnEscape();
		break;
	case aisReturn:
		OnReturn();
		break;
	case aisWait:
		OnWait();
		break;
	default:
		KGLOG_PROCESS_ERROR(FALSE);
		break;
	}
	return;
Exit0:
	return;
}

void KAI_Player::OnFollow()
{
	Follow();
}

void KAI_Player::OnPathResult(BOOL bSucceed)
{
	switch(m_eAIState)
	{
	case aisIdle:
		break;
	case aisWander:
		break;
	case aisPatrol:
		break;
	case aisAlert:
		break;
	case aisFollow:
		OnFollow();
		break;
	case aisPursuit:
		break;
	case aisEscape:
		break;
	case aisReturn:
		break;
	case aisWait:
		break;
	default:
		break;
	}
}
#endif

#ifdef _SERVER
void KAI_Player::OnIdle()
{

}

void KAI_Player::OnWander()
{
	
}

void KAI_Player::OnPartol()
{

}

void KAI_Player::OnAlert()
{

}

//void KAI_Player::OnFollow()
//{
//
//}

void KAI_Player::OnPursuit()
{

}

void KAI_Player::OnKeepAway()
{

}

void KAI_Player::OnEscape()
{
	// 决定是否还要逃跑
	BOOL bRetCode = FALSE;

	KG_PROCESS_ERROR(g_pSO3World->m_nGameLoop < m_EscapeData.nEscapeFrame);

	Escape();
	return;
Exit0:
	// 不跑了，开打
	DoIdle();
	return;
}

void KAI_Player::OnReturn()
{

}

void KAI_Player::OnWait()
{

}

BOOL KAI_Player::OnTryMove(int nRelativeZ, BOOL bIsWater)
{
    return TRUE;
}

void KAI_Player::OnAttacked(KCharacter* pAttacker, int nDamage)
{

}

//void KAI_Player::OnPathResult(BOOL bSucceed)
//{
//
//}

#endif //_SERVER
