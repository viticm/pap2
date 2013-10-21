////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KAILogic.h 
//  Version     : 1.0
//  Creator     : Chen Jie, zhaochunfeng
//  Comment     : AI logic, 一个完整的AI逻辑设定
//
////////////////////////////////////////////////////////////////////////////////
#ifndef _KAI_LOGIC_H_ 
#define _KAI_LOGIC_H_ 

#include "KAIState.h"
#include "KAIAction.h"

class KAIState;
class KCharacter;

class KAILogic
{
public:
    KAILogic();

    BOOL        Setup(int nType, DWORD dwScriptID);

    KAIAction*  GetAction(int nActionID);
    KAIState*   GetState(int nState);
    int         GetInitState();
    DWORD       GetScriptID();

    int         CallAction(KCharacter* pCharacter, int nActionID);

private:
    int     m_nAIType;      // AI type的ID
    DWORD   m_dwScriptID;
    int     m_nInitState;

    typedef std::map<int, KAIAction>  KAI_ACTION_TABLE;
    KAI_ACTION_TABLE      m_ActionTable;

    typedef std::map<int, KAIState> KAI_STATE_TABLE;
    KAI_STATE_TABLE       m_StateTable;

    typedef std::map<int, std::string> KUSER_ACTION_TABLE;
    KUSER_ACTION_TABLE    m_UserActionTable;

public:
    DECLARE_LUA_CLASS(KAILogic);

    DECLARE_LUA_INTEGER(AIType);

    int LuaNewAction(Lua_State* L);
    int LuaNewState(Lua_State* L);
    int LuaSetInitState(Lua_State* L);
    int LuaRegisterUserAction(Lua_State* L);
};

#endif
