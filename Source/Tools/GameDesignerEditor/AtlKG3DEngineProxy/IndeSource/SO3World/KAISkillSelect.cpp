#include "stdafx.h"

#include "KAIBase.h"
#include "KMath.h"
#include "KSO3World.h"

#ifdef _SERVER
void KAIBase::InitSkillCD()
{
	BOOL bRetCode = FALSE;
	KNpc* pNpc = NULL;
	int nIndex = 0;
	KNpcTemplate* pNpcTemplate = NULL;

	KGLOG_PROCESS_ERROR(m_pSelf);

	bRetCode = IS_NPC(m_pSelf->m_dwID);
	KG_PROCESS_ERROR(bRetCode);

	pNpc = (KNpc*)m_pSelf;

	pNpcTemplate = pNpc->m_pTemplate;
	if (pNpcTemplate == NULL)
	{
		pNpcTemplate = g_pSO3World->m_Settings.m_NpcTemplateList.GetTemplate(pNpc->m_dwTemplateID);
		KGLOG_PROCESS_ERROR(pNpcTemplate);
	}

	for (nIndex = 0; nIndex < MAX_NPC_AI_SKILL; nIndex++)
	{
		if (pNpcTemplate->dwSkillIDList[nIndex] == 0
			|| pNpcTemplate->dwSkillLevelList[nIndex] == 0)
			break;

		switch(pNpcTemplate->nAISkillType[nIndex])
		{
		case aistPeriodSelf:
		case aistPeriodFriend:
		case aistPassiveRespond:
		case aistChannelingBreak:
		case aistTargetCount:
		case aistSelfHP:
			pNpc->m_nSkillCastFrame[nIndex] = 0; // A类CD
			break;
		case aistPeriodTarget:
			//技能的CD初始化为正常CD的50%到100%之间,保证随机性   C类CD
			pNpc->m_nSkillCastFrame[nIndex] = g_pSO3World->m_nGameLoop 
				+ pNpcTemplate->nSkillCastInterval[nIndex] / 2
				+ g_Random(pNpcTemplate->nSkillCastInterval[nIndex] / 2);
			break;
		//B类CD策划暂时没有需求
		default:
			KGLOG_PROCESS_ERROR(FALSE);
		    break;
		}
	}

	return;
Exit0:
	return;
}

void KAIBase::CheckAttackTarget()
{
	BOOL bRetCode = FALSE;


	KGLOG_PROCESS_ERROR(m_pSelf);

	

Exit0:
	return;
}

int KAIBase::GetSkillExpectation(AI_SKILL_TYPE eSkillType, DWORD dwSkillIndex, KTarget* pRetTarget)
{
	int nResult = 0;

	KGLOG_PROCESS_ERROR(m_pSelf);
	KGLOG_PROCESS_ERROR(eSkillType > aistInvalid && eSkillType < aistTotal);
    KGLOG_PROCESS_ERROR(pRetTarget);

	switch(eSkillType)
	{
	case aistPeriodTarget:
		nResult = GetSkillExp_PeriodTarget(dwSkillIndex, pRetTarget);
		break;
	case aistPeriodSelf:
		nResult = GetSkillExp_PeriodSelf(dwSkillIndex, pRetTarget);
		break;
	case aistPeriodFriend:
		nResult = GetSkillExp_PeriodFriend(dwSkillIndex, pRetTarget);
	    break;
	case aistPassiveRespond:
		nResult = GetSkillExp_PassiveRespond(dwSkillIndex, pRetTarget);
	    break;
	case aistChannelingBreak:
		nResult = GetSkillExp_ChannelingBreak(dwSkillIndex, pRetTarget);
		break;
	case aistTargetCount:
		nResult = GetSkillExp_TargetCount(dwSkillIndex, pRetTarget);
		break;
	case aistSelfHP:
		nResult = GetSkillExp_SelfHP(dwSkillIndex, pRetTarget);
	    break;
	default:
		KGLOG_PROCESS_ERROR(FALSE);
	    break;
	}

Exit0:
	return nResult;
}

// ---------------------------------- 技能选择函数 --------------------------------------

//1.技能CD:C类CD
//2.释放时机:一旦CD到了就会尝试释放技能
//3.释放目标:填写技能的目标表(*)确定
//4.释放几率:填写几率表项
//5.尝试释放:
//	a)	尝试释放成功后开始计算CD
//	b)	尝试释放失败后,尝试间隔是n秒
int KAIBase::GetSkillExp_PeriodTarget(DWORD dwSkillIndex, KTarget* pTarget)
{
	int  nResult     = 0;
    BOOL bRetCode    = FALSE;

    ASSERT(pTarget);

	//TODO:目标表选择,按几率释放,暂时用第一仇恨代替
	KThreatNode* pThreatNode = m_pSelf->m_ThreatList.GetFirstThreat();
	KG_PROCESS_ERROR(pThreatNode);

	KCharacter* pCharacter = pThreatNode->pCharacter;
	KGLOG_PROCESS_ERROR(pCharacter);

    bRetCode = pTarget->SetTarget(pCharacter);
    KGLOG_PROCESS_ERROR(bRetCode);

	nResult = AI_IMMEDIATELY_CAST_EXP;
Exit0:
	return nResult;
}

//1.技能CD:A类CD
//2.释放时机:一旦CD到了就会尝试释放技能
//3.释放目标:释放者自己
//4.释放几率:填写几率表项
//5.尝试释放:
//	a)	尝试释放成功后开始计算CD
//	b)	尝试释放失败后,尝试间隔是n秒
int KAIBase::GetSkillExp_PeriodSelf(DWORD dwSkillIndex, KTarget* pTarget)
{

	int				nResult		= 0;
	BOOL			bRetCode	= FALSE;
	int				nRandom		= 0;
	KNpcTemplate*	pNpcTemplate = NULL;
	
    ASSERT(pTarget);

    pNpcTemplate = ((KNpc*)m_pSelf)->m_pTemplate;
	if (pNpcTemplate == NULL)
	{
		pNpcTemplate = g_pSO3World->m_Settings.m_NpcTemplateList.GetTemplate(((KNpc*)m_pSelf)->m_dwTemplateID);
		KGLOG_PROCESS_ERROR(pNpcTemplate);
	}
	
	//计算几率
	nRandom = g_Random(MAX_SKILL_RATE);
	KG_PROCESS_ERROR(nRandom < pNpcTemplate->nSkillRate[dwSkillIndex]);

	bRetCode = pTarget->SetTarget(m_pSelf);
	KGLOG_PROCESS_ERROR(bRetCode);

	nResult = AI_IMMEDIATELY_CAST_EXP;
Exit0:
	return nResult;
}

//1.技能CD:A类CD
//2.释放时机:一旦CD到了就会尝试释放技能
//3.释放目标:当前目标的仇恨列表中的前8个目标中随机取的释放目标
//4.释放几率:填写几率表项
//5.尝试释放:
//	a)	尝试释放成功后开始计算CD
//	b)	尝试释放失败后,尝试间隔是n秒
int KAIBase::GetSkillExp_PeriodFriend(DWORD dwSkillIndex, KTarget* pTarget)
{
	int				nResult		    = 0;
	BOOL			bRetCode	    = FALSE;
	int				nRandom		    = 0;
	int				nCount		    = MAX_SELECT_TARGET_COUNT;
	KCharacter*		pEnemy		    = NULL;
	KThreatNode*	pNode		    = NULL;
	KCharacter*		pCharacter	    = NULL;
	KNpcTemplate*   pNpcTemplate    = NULL;

    ASSERT(pTarget);

	pNpcTemplate = ((KNpc*)m_pSelf)->m_pTemplate;
	if (pNpcTemplate == NULL)
	{
		pNpcTemplate = g_pSO3World->m_Settings.m_NpcTemplateList.GetTemplate(((KNpc*)m_pSelf)->m_dwTemplateID);
		KGLOG_PROCESS_ERROR(pNpcTemplate);
	}

	pNode = m_pSelf->m_ThreatList.GetFirstThreat();
	KG_PROCESS_ERROR(pNode);

	pEnemy = pNode->pCharacter;
	KG_PROCESS_ERROR(pEnemy);

	pNode = pEnemy->m_ThreatList.GetFirstThreat();
	while (pNode && nCount)
	{
		nRandom = g_Random(MAX_SKILL_RATE);
		if (nRandom < pNpcTemplate->nSkillRate[dwSkillIndex])
		{
			pCharacter = pNode->pCharacter;
			KG_PROCESS_ERROR(pTarget);

			break;
		}

		nCount--;
		pNode = pEnemy->m_ThreatList.GetNextNode(pNode);
	}

	if (pCharacter)
	{
		bRetCode = pTarget->SetTarget(pCharacter);
		KGLOG_PROCESS_ERROR(bRetCode);

		nResult = AI_IMMEDIATELY_CAST_EXP;
	}

Exit0:
	return nResult;
}

//1.技能CD:A类CD
//2.释放时机:CD到后，如果有呼叫者，那么对呼叫者释放技能。
//3.释放目标:呼叫者
//4.释放几率:被呼叫响应几率
//5.尝试释放:
//	a)	尝试释放成功后开始计算CD
//	b)	尝试释放失败后,尝试间隔是n秒
int KAIBase::GetSkillExp_PassiveRespond(DWORD dwSkillIndex, KTarget* pTarget)
{
	int			nResult			= 0;
	BOOL        bRetCode        = FALSE; 
	int         nMaxLife        = 0;
	int         nCurrentLife    = 0;
	double      fLostLifeRate   = 0;
	KCharacter* pCharacter      = NULL;

    ASSERT(pTarget);

    if (m_PursuitData.dwCallByNpcID && IS_NPC(m_PursuitData.dwCallByNpcID))
	{
		pCharacter = g_pSO3World->m_NpcSet.GetObj(m_PursuitData.dwCallByNpcID);
		KG_PROCESS_ERROR(pCharacter);

		//死了就不加了
		KG_PROCESS_ERROR(pCharacter->m_eMoveState != cmsOnDeath);

		//判断距离
		//TODO:

		nMaxLife = pCharacter->m_nMaxLife;
		nCurrentLife = pCharacter->m_nCurrentLife;
		KGLOG_PROCESS_ERROR(nMaxLife);

		fLostLifeRate = (nMaxLife - nCurrentLife) / (double)nMaxLife;

		KG_PROCESS_ERROR(fLostLifeRate >= CALL_HEAL_LIMIT);

		bRetCode = pTarget->SetTarget(pCharacter);
		KG_PROCESS_ERROR(bRetCode);

		nResult = AI_IMMEDIATELY_CAST_EXP;
	}

Exit0:
	m_PursuitData.dwCallByNpcID = 0; //清除被呼叫记录
	return nResult;
}

//1.技能CD:A类CD
//2.释放时机:当自己仇恨列表中的前8个目标中任意一个开始吟唱和通道的时候，尝试释放
//3.释放目标:吟唱或者通道的目标
//4.释放几率:填写几率表项
//5.尝试释放:
//	a)	尝试释放成功后开始计算CD
//	b)	尝试释放失败后,尝试间隔是n秒
int KAIBase::GetSkillExp_ChannelingBreak(DWORD dwSkillIndex, KTarget* pTarget)
{
    int				nResult		    = 0;
	BOOL			bRetCode	    = FALSE;
	int				nRandom		    = 0;
	int				nCount		    = MAX_SELECT_TARGET_COUNT;
	KThreatNode*	pNode		    = NULL;
	KCharacter*		pCharacter		= NULL;
	KNpcTemplate*	pNpcTemplate    = NULL;

    ASSERT(pTarget);

	pNpcTemplate = ((KNpc*)m_pSelf)->m_pTemplate;
	if (pNpcTemplate == NULL)
	{
		pNpcTemplate = g_pSO3World->m_Settings.m_NpcTemplateList.GetTemplate(((KNpc*)m_pSelf)->m_dwTemplateID);
		KGLOG_PROCESS_ERROR(pNpcTemplate);
	}

	nRandom = g_Random(MAX_SKILL_RATE);
	KG_PROCESS_ERROR(nRandom < pNpcTemplate->nSkillRate[dwSkillIndex]);

	pNode = m_pSelf->m_ThreatList.GetFirstThreat();
	while (pNode && nCount)
	{
		KGLOG_PROCESS_ERROR(pNode->pCharacter);

		if (pNode->pCharacter->m_OTActionParam.eActionType == otActionSkillPrepare
			|| pNode->pCharacter->m_OTActionParam.eActionType == otActionSkillChannel
			|| pNode->pCharacter->m_OTActionParam.eActionType == otActionItemSkill)
		{
			//目标正在释放技能,用这招打他
			pCharacter = pNode->pCharacter;
			break;
		}

		nCount--;
		pNode = m_pSelf->m_ThreatList.GetNextNode(pNode);
	}

	if (pCharacter)
	{
		bRetCode = pTarget->SetTarget(pCharacter);
		KGLOG_PROCESS_ERROR(bRetCode);

		nResult = AI_IMMEDIATELY_CAST_EXP;
	}

Exit0:
	return nResult;
}

//1.技能CD:A类CD
//2.释放时机:周期性判断，根据自己仇恨列表中的成员个数来确定释放几率尝试释放
//3.释放目标:填写技能的目标表(*)确定
//4.释放几率:填写几率表项
//5.尝试释放:
//	a)	尝试释放成功后开始计算CD
//	b)	尝试释放失败后,尝试间隔是n秒
int KAIBase::GetSkillExp_TargetCount(DWORD dwSkillIndex, KTarget* pTarget)
{
	int             nResult         = 0;
	BOOL            bRetCode        = FALSE;
	int             nThreatCount    = 0;
	int             nCount          = MAX_SELECT_TARGET_COUNT;
	int             nRandom         = 0;
	KCharacter*     pCharacter      = NULL;
	KThreatNode*    pNode           = NULL;
	KNpcTemplate*	pNpcTemplate    = NULL;

    ASSERT(pTarget);

    pNpcTemplate = ((KNpc*)m_pSelf)->m_pTemplate;
	if (pNpcTemplate == NULL)
	{
		pNpcTemplate = g_pSO3World->m_Settings.m_NpcTemplateList.GetTemplate(((KNpc*)m_pSelf)->m_dwTemplateID);
		KGLOG_PROCESS_ERROR(pNpcTemplate);
	}

	pNode = m_pSelf->m_ThreatList.GetFirstThreat();
	while (pNode && nCount)
	{
		nThreatCount++;		
		nCount--;
		pNode = m_pSelf->m_ThreatList.GetNextNode(pNode);
	}

	nRandom = g_Random(MAX_SKILL_RATE);
	KG_PROCESS_ERROR(nRandom < (nThreatCount * pNpcTemplate->nSkillRate[dwSkillIndex]));

	//TODO:目标列编未完成
	pNode = m_pSelf->m_ThreatList.GetFirstThreat();
	KG_PROCESS_ERROR(pNode);

	pCharacter = pNode->pCharacter;
	KG_PROCESS_ERROR(pCharacter);

	bRetCode = pTarget->SetTarget(pCharacter);
	KG_PROCESS_ERROR(bRetCode);

	nResult = AI_IMMEDIATELY_CAST_EXP;
Exit0:
	return nResult;
}

//1.技能CD:A类CD
//2.释放时机:HP值少于某个固定值的时候
//3.释放目标:自己
//4.释放几率:填写几率表项
//5.尝试释放:
//	a)	尝试释放成功后开始计算CD
//	b)	尝试释放失败后,尝试间隔是n秒
int KAIBase::GetSkillExp_SelfHP(DWORD dwSkillIndex, KTarget* pTarget)
{
	int             nResult         = 0;
	BOOL            bRetCode        = FALSE;
	int             nMaxLife        = 0;
	int             nCurrentLife    = 0;
	double          fLifeRate		= 0;
	KNpcTemplate*	pNpcTemplate    = NULL;

    ASSERT(pTarget);

	pNpcTemplate = ((KNpc*)m_pSelf)->m_pTemplate;
	if (pNpcTemplate == NULL)
	{
		pNpcTemplate = g_pSO3World->m_Settings.m_NpcTemplateList.GetTemplate(((KNpc*)m_pSelf)->m_dwTemplateID);
		KGLOG_PROCESS_ERROR(pNpcTemplate);
	}

	nMaxLife = m_pSelf->m_nMaxLife;
	nCurrentLife = m_pSelf->m_nCurrentLife;
	KGLOG_PROCESS_ERROR(nMaxLife);

	fLifeRate = (nCurrentLife) / (double)nMaxLife;

	KG_PROCESS_ERROR((int)(fLifeRate * 1024) < pNpcTemplate->nSkillRate[dwSkillIndex]);

	bRetCode = pTarget->SetTarget(m_pSelf);
	KG_PROCESS_ERROR(bRetCode);

	nResult = AI_IMMEDIATELY_CAST_EXP;
Exit0:
	return nResult;
}

#endif //_SERVER
