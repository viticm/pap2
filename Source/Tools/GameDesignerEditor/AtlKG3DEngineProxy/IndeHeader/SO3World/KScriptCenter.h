/************************************************************************/
/* 脚本中心								                                */
/* Copyright : Kingsoft 2004										    */
/* Author	 : Zhu Jianqiu												*/
/* History	 :															*/
/*		2005.03.02	Create												*/
/* Comment	 :															*/
/*		封装了对脚本的操作和存储										*/
/************************************************************************/
#ifndef _KSCRIPT_CENTER2_H_
#define _KSCRIPT_CENTER2_H_

#include <map>
#include <string>

#include "Engine/KLuaScriptEx.h"
#include "Engine/KMemory.h"
#include "Luna.h"

class KSO3World;
class KCharacter;
class KPlayer;

class KScriptCenter
{
public:
	KScriptCenter(void);
	~KScriptCenter(void);

	BOOL Init();
	void UnInit();

    void Activate();

    BOOL ReloadAll();

    BOOL ReloadScript(const char* pszFileName);

	void SafeCallBegin(int* pIndex);
	void SafeCallEnd(int nIndex);

	BOOL GetValuesFromStack(const char * pszFormat , ...);

	template<class T>
	void PushValueToStack(T value);

	BOOL CallFunction(DWORD dwScriptID, const char* pszFuncName, int nResults);
	BOOL CallFunction(const char* pszScriptName, const char* pszFuncName, int nResults);

	BOOL IsFuncExist(DWORD dwScriptID, const char* pszFuncName);
	BOOL IsFuncExist(const char* pszScriptName, const char* pszFuncName);

	BOOL IsScriptExist(DWORD dwScriptID);
	BOOL IsScriptExist(const char* pszScriptName);

	DWORD GetActiveScriptID();

    ILuaScriptEx* GetScriptHolder() { return m_piScript; }
	lua_State* GetLuaState(void); 

#ifdef _SERVER
	BOOL ExecutePlayerScript(KPlayer* pPlayer, const char* pszScript);
    BOOL ExecuteGMCScript(KPlayer* pPlayer, const char* pszGmName, const char* pszScript);
#endif

private:
	BOOL            LoadScriptList();
	BOOL            SearchForScriptList(const char cszDir[]);
	const char*     GetScriptNameByID(DWORD dwScriptID);
	BOOL            LoadScript(const char cszFileName[]);
    ILuaScriptEx*   CreateScriptHolder();

    typedef KMemory::KAllocator<std::pair<DWORD, kstring> > KSCRIPT_TABLE_ALLOCTOR;
    typedef std::map<DWORD, kstring, std::less<DWORD>, KSCRIPT_TABLE_ALLOCTOR> KSCRIPT_TABLE;
	KSCRIPT_TABLE   m_ScriptTable;

	ILuaScriptEx*   m_piScript;
};

template<class T>
void KScriptCenter::PushValueToStack(T value)
{
	assert(m_piScript);
	m_piScript->PushValueToStack(value);
}
#endif	//_KSCRIPT_CENTER2_H_
