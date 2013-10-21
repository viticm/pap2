#ifndef _KAI_CONTROL_
#define _KAI_CONTROL_

#include "Global.h"
#include "SO3Result.h"
#include "KAIBase.h"

class KNpcTeam;

class KAIControl
{
public:
	KAIControl(void);
	~KAIControl(void);

	BOOL Init(AI_TYPE eAIType, KCharacter* pSelf);
	BOOL UnInit();

	void Activate();

	void Stop();
	void Restart();
    BOOL IsRunning();

	BOOL SetAIType(AI_TYPE eAIType);
	AI_TYPE GetAIType();

	BOOL		SetMainState(KAI_STATE eAIMainState);
	KAI_STATE	GetMainState();
	void		SetOriginPoint(int nX, int nY, int nZ, int nDir);
	BOOL		GetOriginPoint(int* pnRetX, int* pnRetY, int* pnRetZ, int* pnRetDir);
	BOOL		SetAIParam(int nParamTemplateID);
	int			GetAIEventNextFrame(int nIndex);
	KAIBase*	GetAIBase();
	KAI_STATE	GetAIState();
	KAIParamTemplate* GetAIParam();

#if defined(_SERVER)
	BOOL		SetNpcTeam(KNpcTeam* pNpcTeam);
	KNpcTeam*	GetNpcTeam();
	BOOL		SetPatrolPath(int nPatrolPathID, int nNextPointIndex = 0);
#endif //_SERVER
	// ------------- ×´Ì¬ÇÐ»»º¯Êý -----------------------------
	void DoIdle(int nIdleFrame = DEFAULT_IDLE_FRAME);
	void DoWander();
	void DoPatrol();
	void DoAlert();
	void DoFollow(KCharacter* pTarget);
	void DoPursuit();
	void DoKeepAway();
	void DoEscape(int nEscapeRange = DEFAULT_ESCAPE_RANGE, int nEscapeFrame = DEFAULT_ESCAPE_FRAME);
	void DoReturn();
	void DoWait();

	// ------------- AIÊÂ¼þ -----------------------------
	BOOL OnTryMove(int nRelativeZ, BOOL bIsWater);
	void OnAttacked(KCharacter* pAttacker, int nDamage);
	void OnClearThreat(KCharacter* pCharacter);
	void OnPathResult(BOOL bSucceed);

	// ------------- AIÊôÐÔ ------------------------------
	BOOL IsPositive();
	int GetTurnToIdleFrame();

	// ------------- AIÃüÁî ------------------------------
#ifdef _SERVER
	BOOL FireAIEvent(AI_EVENT_CONDITION eAIEvent);
	BOOL CallHelp(int nRange);
	BOOL SetCallHealer(DWORD dwNpcID);
#endif //_SERVER

private:
	unsigned char m_AIBase[sizeof(KAIBase)];
	KAIBase* m_pAIBase;

	BOOL m_bRunning;
};

double g_GetRangeByLevelDiff(int nLevelDiff);

#endif //_KAI_CONTROL_
