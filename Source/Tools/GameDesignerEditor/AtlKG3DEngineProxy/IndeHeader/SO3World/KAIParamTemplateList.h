#ifndef _KAI_PARAM_TEMPLATE_LIST_
#define _KAI_PARAM_TEMPLATE_LIST_

#include <map>
#include "Global.h"

#ifdef _SERVER
#include "KAIEvent.h"
#endif //_SERVER

// AI的模板参数，不可在运行中动态修改的参数
struct KAIParamTemplate
{
	BOOL bIsPositive;			//是否主动怪
	KAI_STATE eAIMainState;		//NPC默认行为

	int nAlertRange;			//警戒范围
	int nAlertOverTime;			//警戒超时时间（超时就转为战斗）
	int nAttackRange;			//攻击范围
	int nPursuitRange;			//追击距离

	int nEscapeTime;			//每次逃跑的持续时间
	int nEscapeRange;			//逃跑每次行走的最大距离
	int nEscapeInterval;		//逃跑间隔

	int nWanderRange;			//闲逛的范围
	int nWanderDistance;		//闲逛每次行走的最大距离
	int nWanderInterval;		//闲逛的间隔

	//int nPatrolPathID;			//巡逻路径ID

#ifdef _SERVER
	//AI模板的初始化脚本
	char szScriptName[MAX_PATH];
	KAIEvent AIEvent[MAX_AI_EVENT];
#endif //_SERVER
};

class KAIParamTemplateList
{
public:
	KAIParamTemplateList(void);
	~KAIParamTemplateList(void);

	BOOL Init();
	void UnInit();

	BOOL LoadTemplate(int nIndex, ITabFile* pTabFile, KAIParamTemplate* pAITemplate);
	KAIParamTemplate* GetTemplate(DWORD dwID);

private:
	typedef std::map<DWORD, KAIParamTemplate*>  AI_PARAM_TEMPLATE_MAP;
	AI_PARAM_TEMPLATE_MAP m_mapAIParamTemplate;
	KAIParamTemplate* m_pAITemplate;
	KAIParamTemplate m_DefaultTemplate;
};

#endif //_KAI_PARAM_TEMPLATE_LIST_
