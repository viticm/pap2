#ifndef _KAI_EVENT_H_
#define _KAI_EVENT_H_

#ifdef _SERVER 

#include <vector>
#include "Luna.h"
#include "KCharacter.h"
#include "Global.h"



struct KAIEventCondition
{
	AI_EVENT_CONDITION  m_eAIEventCondition;
	int                 m_nValue;

	BOOL CheckCondition(KCharacter* m_pSelf, AI_EVENT_CONDITION eCurrentEvent);
	BOOL SetCondition(AI_EVENT_CONDITION eAIEventCondition);
	BOOL SetValue(int nValue);
};

struct KAIEventAction
{
	AI_EVENT_ACTION m_eAIEventAction;
	union
	{
		int         m_nEscapeChance;
		int         m_nCallHelpRange;
		int         m_nScriptID;
	};

	BOOL SetAction(AI_EVENT_ACTION eAIEventAction);
	BOOL RunAction(KCharacter* m_pSelf);
	BOOL SetEscapeChance(int nEscapeChance);
	BOOL SetCallHelpRange(int nCallHelpRange);
	BOOL SetScriptName(const char* szScriptName);
};

class KAIEvent
{
public:
	KAIEvent(void);
	~KAIEvent(void);

	BOOL Init(AI_EVENT_ACTION eAction, BOOL bConditionLogic = FALSE, BOOL bRunOnce = FALSE);
	BOOL SetCDFrame(int nCDFrame);
	BOOL GetCDFrame() { return m_nCDFrame; }

	BOOL AddCondition(AI_EVENT_CONDITION eCondition, int nValue);
	int GetConditionCount() { return (int)m_vecCondition.size(); }
	KAIEventAction* GetAction() { return &m_Action; }
	BOOL CheckEvent(KCharacter* pSelf, int nEventIndex, AI_EVENT_CONDITION eCurrentEvent = aecInvalid);
	BOOL FireEvent(KCharacter* pSelf, int nEventIndex, AI_EVENT_CONDITION eCurrentEvent);
	BOOL RunEvent(KCharacter* pSelf);

	BOOL IsRunOnce() { return m_bRunOnce; }

private:
	BOOL	m_bIsActive;
	BOOL	m_bConditionLogic;  // true 表示或，false 表示与
	BOOL	m_bRunOnce;			// true 表示只能触发一次，false 表示只要CD到了就能重复触发
	int		m_nCDFrame;

	std::vector<KAIEventCondition> m_vecCondition;
	KAIEventAction m_Action;

public:
	DECLARE_LUA_CLASS(KAIEvent);
	DECLARE_LUA_BOOL(IsActive);

	int LuaInit(Lua_State* L);
	int LuaSetCDFrame(Lua_State* L);

	int LuaAddCondition(Lua_State* L);
	int LuaGetConditionCount(Lua_State* L);

	int LuaSetEscapeChance(Lua_State* L);
	int LuaSetCallHelpRange(Lua_State* L);
	int LuaSetScriptName(Lua_State* L);
};

#endif //_SERVER

#endif //_KAI_EVENT_H_
