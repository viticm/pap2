#include "stdafx.h"

#include "KAIParamTemplateList.h"

#define AI_TEMPLATE_FILE		"AITemplate.tab"

#define AI_TEMPLATE_ID			"ID"

#define AI_IS_POSITIVE			"IsPositive"
#define AI_MAIN_STATE			"MainState"

#define AI_NPC_ALERT_RANGE		"AlertRange"
#define AI_ALERT_OVER_TIME		"AlertOverTime"
#define AI_ATTACK_RANGE			"AttackRange"
#define AI_PURSUIT_RANGE		"PursuitRange"

#define AI_ESCAPE_TIME			"EscapeTime"
#define AI_ESCAPE_RANGE			"EscapeRange"
#define AI_ESCAPE_INTERVAL		"EscapeInterval"

#define AI_WANDER_RANGE			"WanderRange"
#define AI_WANDER_DISTANCE		"WanderDistance"
#define AI_WANDER_INTERVAL		"WanderInterval"

#define AI_PATROL_PATH_ID		"PatrolPathID"

#define AI_INIT_SCRIPT_NAME		"ScriptName"

KAIParamTemplateList::KAIParamTemplateList(void)
{
    m_pAITemplate = NULL;
	memset(&m_DefaultTemplate, 0, sizeof(m_DefaultTemplate));
}

KAIParamTemplateList::~KAIParamTemplateList(void)
{

}

BOOL KAIParamTemplateList::Init()
{
    BOOL        bResult                 = false;
	BOOL        bRetCode                = false;
	ITabFile*   pTabFile	            = NULL;
    int         nSize                   = 0;
    char        szFileName[MAX_PATH];

	snprintf(szFileName, sizeof(szFileName), "%s/%s", SETTING_DIR, AI_TEMPLATE_FILE);
    szFileName[sizeof(szFileName) - 1] = '\0';

	pTabFile = g_OpenTabFile(szFileName);
	KGLOG_PROCESS_ERROR(pTabFile);

	nSize = pTabFile->GetHeight() - 2;
	KGLOG_PROCESS_ERROR(nSize > 0);

	m_pAITemplate = new KAIParamTemplate[nSize];
	KGLOG_PROCESS_ERROR(m_pAITemplate);

	memset(m_pAITemplate, 0, sizeof(KAIParamTemplate) * nSize);

	bRetCode = LoadTemplate(2, pTabFile, &m_DefaultTemplate);
	KGLOG_PROCESS_ERROR(bRetCode);

	for (int nIndex = 0; nIndex < nSize; nIndex++)
	{
		bRetCode = LoadTemplate(nIndex + 3, pTabFile, m_pAITemplate + nIndex);
		KGLOG_PROCESS_ERROR(bRetCode);
	}

	bResult = true;
Exit0:
    if (!bResult)
    {
        SAFE_DELETE_ARRAY(m_pAITemplate);
    }
	KG_COM_RELEASE(pTabFile);
	return bResult;
}

void KAIParamTemplateList::UnInit()
{
    SAFE_DELETE_ARRAY(m_pAITemplate);
    return;
}

BOOL KAIParamTemplateList::LoadTemplate(int nIndex, ITabFile* pTabFile, KAIParamTemplate* pAITemplate)
{
    BOOL    bResult     = false;
	BOOL    bRetCode    = false;
	DWORD   dwID        = 0;

	KGLOG_PROCESS_ERROR(pTabFile);

	bRetCode = pTabFile->GetInteger(nIndex, AI_TEMPLATE_ID, 0, (int*)&dwID);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = pTabFile->GetInteger(nIndex, AI_IS_POSITIVE, 
		m_DefaultTemplate.bIsPositive, (int*)&(pAITemplate->bIsPositive));
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = pTabFile->GetInteger(nIndex, AI_MAIN_STATE,
		m_DefaultTemplate.eAIMainState, (int*)&(pAITemplate->eAIMainState));
	KGLOG_PROCESS_ERROR(bRetCode);
	KGLOG_PROCESS_ERROR(pAITemplate->eAIMainState >= aisIdle && pAITemplate->eAIMainState <= aisFollow);

	bRetCode = pTabFile->GetInteger(nIndex, AI_NPC_ALERT_RANGE, 
		m_DefaultTemplate.nAlertRange, (int*)&(pAITemplate->nAlertRange));
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = pTabFile->GetInteger(nIndex, AI_ALERT_OVER_TIME,
		m_DefaultTemplate.nAlertOverTime, (int*)&(pAITemplate->nAlertOverTime));
	KGLOG_PROCESS_ERROR(bRetCode);
	
	bRetCode = pTabFile->GetInteger(nIndex, AI_ATTACK_RANGE, 
		m_DefaultTemplate.nAttackRange, (int*)&(pAITemplate->nAttackRange));
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = pTabFile->GetInteger(nIndex, AI_PURSUIT_RANGE, 
		m_DefaultTemplate.nPursuitRange, (int*)&(pAITemplate->nPursuitRange));
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = pTabFile->GetInteger(nIndex, AI_ESCAPE_TIME, 
		m_DefaultTemplate.nEscapeTime, (int*)&(pAITemplate->nEscapeTime));
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = pTabFile->GetInteger(nIndex, AI_ESCAPE_RANGE, 
		m_DefaultTemplate.nEscapeRange, (int*)&(pAITemplate->nEscapeRange));
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = pTabFile->GetInteger(nIndex, AI_ESCAPE_INTERVAL, 
		m_DefaultTemplate.nEscapeInterval, (int*)&(pAITemplate->nEscapeInterval));
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = pTabFile->GetInteger(nIndex, AI_WANDER_RANGE, 
		m_DefaultTemplate.nWanderRange, (int*)&(pAITemplate->nWanderRange));
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = pTabFile->GetInteger(nIndex, AI_WANDER_DISTANCE, 
		m_DefaultTemplate.nWanderDistance, (int*)&(pAITemplate->nWanderDistance));
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = pTabFile->GetInteger(nIndex, AI_WANDER_INTERVAL, 
		m_DefaultTemplate.nWanderInterval, (int*)&(pAITemplate->nWanderInterval));
	KGLOG_PROCESS_ERROR(bRetCode);

#ifdef _SERVER
	/*
	bRetCode = pTabFile->GetString(nIndex, AI_INIT_SCRIPT_NAME,
		"", pAITemplate->szScriptName, MAX_PATH);
	KGLOG_PROCESS_ERROR(bRetCode);

	if (pAITemplate->szScriptName[0] != '\0')
	{
		char szFuncName[MAX_PATH];
		char szFullFileName[MAX_PATH];
		DWORD dwScriptID = 0;

		//调用初始化AI模板脚本
		snprintf(szFullFileName, sizeof(szFullFileName), "%s/%s", "Scripts", pAITemplate->szScriptName);
        szFullFileName[sizeof(szFullFileName) - 1] = '\0';

		dwScriptID = KLuaScriptEx::ScriptNameToID(pAITemplate->szScriptName);
		bRetCode = g_pSO3World->m_ScriptCenter.IsScriptExist(dwScriptID);
		if (bRetCode)
		{
			DWORD dwIndex = 0;
			for (dwIndex = 0; dwIndex < MAX_AI_EVENT; dwIndex++)
			{
				snprintf(szFuncName, sizeof(szFuncName), "%s%d", INIT_AI_EVENT_FUNC_NAME, dwIndex + 1);
                szFuncName[sizeof(szFuncName) - 1] = '\0';

				if (g_pSO3World->m_ScriptCenter.IsFuncExist(dwScriptID, szFuncName))
				{
					int nTopIndex = 0;
					g_pSO3World->m_ScriptCenter.SafeCallBegin(&nTopIndex);

					pAITemplate->AIEvent[dwIndex].LuaGetObj(g_pSO3World->m_ScriptCenter.GetLuaState());

					g_pSO3World->m_ScriptCenter.CallFunction(dwScriptID, szFuncName, 0, "u");
					g_pSO3World->m_ScriptCenter.SafeCallEnd(nTopIndex);
				}
			}
		}
	}
	*/
#endif //_SERVER

	m_mapAIParamTemplate[dwID] = pAITemplate;

	bResult = true;
Exit0:
	return bResult;
}

KAIParamTemplate* KAIParamTemplateList::GetTemplate(DWORD dwID)
{
	AI_PARAM_TEMPLATE_MAP::iterator iter;
	KGLOG_PROCESS_ERROR(dwID >= 0);

	iter = m_mapAIParamTemplate.find(dwID);
	KGLOG_PROCESS_ERROR(iter != m_mapAIParamTemplate.end());

	return iter->second;
Exit0:
	return NULL;
}