#ifndef _KSCRIPTMANAGER_H_ 
#define _KSCRIPTMANAGER_H_ 

#include "Engine/KLuaScriptEx.h"
#include "Luna.h"

#define CENTER_SCRIPTS_DIR "center_scripts"
#define MAIN_SCRIPT_FILE    CENTER_SCRIPTS_DIR"/main.lua"

class KScriptManager
{
public:
    KScriptManager();

    BOOL Init();
    void UnInit();

    void Activate();

    BOOL ExecuteGMCScript(const char cszGmName[], const char cszScript[]);

	void SafeCallBegin(int* pIndex);
	void SafeCallEnd(int nIndex);

    BOOL GetValuesFromStack(const char * pszFormat , ...);

	template<class T>
	void Push(T value)
    {
	    m_piScript->PushValueToStack(value);
    }

	BOOL Call(DWORD dwScriptID, const char* pszFuncName, int nResults);
	BOOL Call(const char* pszScriptName, const char* pszFuncName, int nResults);

	BOOL IsFuncExist(DWORD dwScriptID, const char* pszFuncName);
	BOOL IsFuncExist(const char* pszScriptName, const char* pszFuncName);

	BOOL IsScriptExist(DWORD dwScriptID);
	BOOL IsScriptExist(const char* pszScriptName);

    ILuaScriptEx* GetScriptHolder()
    {
        return m_piScript;
    }

    void Reload() { m_bReload = true; }

private:
    BOOL IsLuaScriptFile(const char cszFileName[]);
    BOOL LoadScripts(ILuaScriptEx* piScript, const char cszDir[]);    

    BOOL Load();

private:
    ILuaScriptEx*   m_piScript;
    BOOL            m_bReload;
};

#endif
