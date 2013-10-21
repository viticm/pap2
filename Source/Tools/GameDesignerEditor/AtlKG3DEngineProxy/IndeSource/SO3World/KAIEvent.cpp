#include "stdafx.h"

#include "KAIEvent.h"
#include "KSO3World.h"
#include "KAIBase.h"
#include "KNpc.h"

#ifdef _SERVER

//------- AI事件的条件 --------------------------------------------------->
BOOL KAIEventCondition::SetCondition(AI_EVENT_CONDITION eAIEventCondition)
{
	BOOL bResult = FALSE;
	KGLOG_PROCESS_ERROR(eAIEventCondition > aecInvalid && eAIEventCondition < aecTotal);

	this->m_eAIEventCondition = eAIEventCondition;

	bResult = TRUE;
Exit0:
	return bResult;
}

BOOL KAIEventCondition::CheckCondition(KCharacter* m_pSelf, AI_EVENT_CONDITION eCurrentEvent)
{
	BOOL			bResult				= FALSE;
	int				nCurrentLife		= 0;
	int				nMaxLife			= 0;
	double			fLifeRate			= 0.0;
	KAIBase*		pAIBase				= NULL;
	int				nRandom				= 0;
	int				nTurnToFightFrame	= 0;
	KThreatNode*	pTarget				= NULL;
	
	KGLOG_PROCESS_ERROR(m_pSelf);

	//eCurrentEvent由FireEvent设置,表示当前已经触发的事件
	KG_PROCESS_SUCCESS(eCurrentEvent != aecInvalid && eCurrentEvent == m_eAIEventCondition);

	switch(m_eAIEventCondition)
	{
	case aecSelfHPBelow:
		//自身血量低于n%
		nCurrentLife = m_pSelf->m_nCurrentLife;
		nMaxLife = m_pSelf->m_nMaxLife;

		fLifeRate = nCurrentLife / (double)nMaxLife;
		KG_PROCESS_ERROR(fLifeRate * 100.0 <= (double)m_nValue);
		break;
	case aecTargetHPBelow:
		//目标血量低于n%
		pTarget = m_pSelf->m_ThreatList.GetFirstThreat();
		KG_PROCESS_ERROR(pTarget);

		nCurrentLife = pTarget->pCharacter->m_nCurrentLife;
		nMaxLife = pTarget->pCharacter->m_nMaxLife;

		fLifeRate = nCurrentLife / (double)nMaxLife;
		KG_PROCESS_ERROR(fLifeRate * 100.0 <= (double)m_nValue);
	    break;
	case aecFrameInterval:
		//超过一定时间就返回TRUE，触发事件
		nTurnToFightFrame = m_pSelf->m_AIController.GetTurnToIdleFrame();
		KGLOG_PROCESS_ERROR(pAIBase);
		KGLOG_PROCESS_ERROR(m_nValue >= 0);
		KG_PROCESS_ERROR(nTurnToFightFrame > 0);
	
		KG_PROCESS_ERROR(g_pSO3World->m_nGameLoop >= nTurnToFightFrame + m_nValue);
		break;
	case aecRandom:
		//随机返回一个结果
		nRandom = g_Random(MILLION_NUM);
		KG_PROCESS_ERROR(nRandom < m_nValue);
		break;
	case aecSelfDead:
	case aecTargetDead:
	case aecReturnToOrigin:
	case aecTurnToFight:
	case aecTurnToIdle:
		KG_PROCESS_ERROR(FALSE);
		break;
	default:
		KGLOG_PROCESS_ERROR(FALSE && "Invalid AI Event Condition!\n");
	    break;
	}

Exit1:
	bResult = TRUE;
Exit0:
	return bResult;
}

BOOL KAIEventCondition::SetValue(int nValue)
{
	this->m_nValue = nValue;

	return TRUE;
}

//------- AI事件的行为 --------------------------------------------------->
BOOL KAIEventAction::SetAction(AI_EVENT_ACTION eAIEventAction)
{
	BOOL bResult = FALSE;

	KGLOG_PROCESS_ERROR(eAIEventAction > aeaInvalid && eAIEventAction < aeaTotal);

	this->m_eAIEventAction = eAIEventAction;
	m_nScriptID = 0;

	bResult = TRUE;
Exit0:
	return bResult;
}

BOOL KAIEventAction::RunAction(KCharacter* m_pSelf)
{
	BOOL bResult = FALSE;

	KGLOG_PROCESS_ERROR(m_pSelf);
		

	switch(m_eAIEventAction)
	{
	//TODO:
	case aeaEscape:
		//逃跑
		m_pSelf->m_AIController.DoEscape();
		break;
	case aeaEscapeForHelp:
		//逃向盟友
		break;
	case aeaCallHelp:
		//原地寻求帮助
		m_pSelf->m_AIController.CallHelp(m_nCallHelpRange);
	    break;
	case aeaDeath:
		//自杀
		m_pSelf->GoToHell();
	    break;
	case aeaRunScript:
		//跑脚本
		{
			if (IS_PLAYER(m_pSelf->m_dwID))
				break; // 暂时不支持player

			bResult = g_pSO3World->m_ScriptCenter.IsScriptExist(m_nScriptID);
			if (bResult && g_pSO3World->m_ScriptCenter.IsFuncExist(m_nScriptID, SCRIPT_ON_AI_EVENT))
			{
				int nTopIndex = 0;
				g_pSO3World->m_ScriptCenter.SafeCallBegin(&nTopIndex);

				g_pSO3World->m_ScriptCenter.PushValueToStack((KNpc*)m_pSelf);

				g_pSO3World->m_ScriptCenter.CallFunction(m_nScriptID, SCRIPT_ON_AI_EVENT, 0);
				g_pSO3World->m_ScriptCenter.SafeCallEnd(nTopIndex);
			}
		}
		break;
	default:
		KGLOG_PROCESS_ERROR(FALSE && "Invalid AI Event Action!\n");
	    break;
	}

	bResult = TRUE;
Exit0:
	return bResult;
}

BOOL KAIEventAction::SetEscapeChance(int nEscapeChance)
{
	this->m_nEscapeChance = nEscapeChance;

	return TRUE;
}

BOOL KAIEventAction::SetCallHelpRange(int nCallHelpRange)
{
	this->m_nCallHelpRange = nCallHelpRange;

	return TRUE;
}

BOOL KAIEventAction::SetScriptName(const char* szScriptName)
{
	BOOL bResult = FALSE;

	KGLOG_PROCESS_ERROR(szScriptName);
	KGLOG_PROCESS_ERROR(szScriptName[0] != '\0');

	m_nScriptID = g_FileNameHash(szScriptName);
	KGLOG_PROCESS_ERROR(m_nScriptID);

	bResult = TRUE;
Exit0:
	return bResult;
}

//------- AI事件 --------------------------------------------------------->
KAIEvent::KAIEvent(void)
{
	m_bIsActive = FALSE;
	m_bConditionLogic = FALSE;	//1表示或，0表示与
	m_bRunOnce = FALSE;			//1表示只能触发一次，0表示只要CD到了就能重复触发
	m_nCDFrame = 0;

	m_vecCondition.clear();
}

KAIEvent::~KAIEvent(void)
{

}

BOOL KAIEvent::Init(AI_EVENT_ACTION eAction, BOOL bConditionLogic, BOOL bRunOnce)
{
	BOOL bResult = FALSE;

	KGLOG_PROCESS_ERROR(eAction > aeaInvalid && eAction < aeaTotal);

	m_bConditionLogic = bConditionLogic;
	m_bRunOnce = bRunOnce;
	m_bIsActive = TRUE;

	m_Action.SetAction(eAction);

	bResult = TRUE;
Exit0:
	return bResult;
}

BOOL KAIEvent::SetCDFrame(int nCDFrame)
{
	BOOL bResult = FALSE;

	KGLOG_PROCESS_ERROR(m_bIsActive);

	m_nCDFrame = nCDFrame;

	bResult = TRUE;
Exit0:
	return bResult;
}

BOOL KAIEvent::AddCondition(AI_EVENT_CONDITION eCondition, int nValue)
{
	BOOL bRetCode = FALSE;

	KAIEventCondition AIEventCondition;

	bRetCode = AIEventCondition.SetCondition(eCondition);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = AIEventCondition.SetValue(nValue);
	KGLOG_PROCESS_ERROR(bRetCode);

	m_vecCondition.push_back(AIEventCondition);

	bRetCode = TRUE;
Exit0:
	return bRetCode;
}

BOOL KAIEvent::CheckEvent(KCharacter* pSelf, int nEventIndex, AI_EVENT_CONDITION eCurrentEvent)
{	
	BOOL bRetCode = FALSE;
	int nIndex = 0;
	int nSize = (int)m_vecCondition.size();

	KGLOG_PROCESS_ERROR(pSelf);
	KG_PROCESS_ERROR(m_bIsActive);
	if (nSize == 0)
	{
		m_bIsActive = FALSE;
		goto Exit0;
	}

	int nEventNextFrame = pSelf->m_AIController.GetAIEventNextFrame(nEventIndex);
	if (eCurrentEvent < aecEvent)
	{
		KG_PROCESS_ERROR(nEventNextFrame >= 0);
		KG_PROCESS_ERROR(g_pSO3World->m_nGameLoop >= nEventNextFrame);
	}

	if (m_bConditionLogic)
	{
		//条件为或的关系
		for (nIndex = 0; nIndex < nSize; nIndex++)
		{
			bRetCode = m_vecCondition[nIndex].CheckCondition(pSelf, eCurrentEvent);
			if (bRetCode)
				break;
		}
		KG_PROCESS_ERROR(nIndex < nSize);
	}
	else
	{
		//条件为与的关系
		for (nIndex = 0; nIndex < nSize; nIndex++)
		{
			bRetCode = m_vecCondition[nIndex].CheckCondition(pSelf, eCurrentEvent);
			if (!bRetCode)
				break;
		}
		KG_PROCESS_ERROR(nIndex == nSize);
	}

	bRetCode = TRUE;
Exit0:
	return bRetCode;
}

BOOL KAIEvent::FireEvent(KCharacter* pSelf, int nEventIndex, AI_EVENT_CONDITION eCurrentEvent)
{
	BOOL bRetCode = FALSE;

	KGLOG_PROCESS_ERROR(pSelf);
	KGLOG_PROCESS_ERROR(eCurrentEvent != aecInvalid);
	KG_PROCESS_ERROR(m_bIsActive);

	bRetCode = CheckEvent(pSelf, nEventIndex, eCurrentEvent);
	if (bRetCode)
	{
		bRetCode = RunEvent(pSelf);
		KG_PROCESS_ERROR(bRetCode);
	}

	bRetCode = TRUE;
Exit0:
	return bRetCode;
}

BOOL KAIEvent::RunEvent(KCharacter* pSelf)
{
	BOOL bRetCode = FALSE;

	KGLOG_PROCESS_ERROR(pSelf);
	KGLOG_PROCESS_ERROR(m_bIsActive);

	bRetCode = m_Action.RunAction(pSelf);
	KG_PROCESS_ERROR(bRetCode);

	bRetCode = TRUE;
Exit0:
	return bRetCode;
}

#endif //_SERVER
