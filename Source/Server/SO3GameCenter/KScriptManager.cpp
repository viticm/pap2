#include "stdafx.h"
#include "KScriptFuncList.h"
#include "KScriptManager.h"
#include "KSO3GameCenter.h"

extern KLuaConstList g_LuaCenterConstList[];

KScriptManager::KScriptManager()
{
    m_piScript = NULL;
    m_bReload  = false;
}

BOOL KScriptManager::Init()
{
    BOOL bResult    = false;
    BOOL bRetCode   = false;

    bRetCode = Load();
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

void KScriptManager::UnInit()
{
    KG_COM_RELEASE(m_piScript);
}

void KScriptManager::Activate()
{
    if (m_bReload)
    {
        Load();
        m_bReload = false;
    }

    if (g_pSO3GameCenter->m_nWorkLoop % (GAME_FPS * 8) == 0)
    {
        lua_gc(m_piScript->GetLuaState(), LUA_GCCOLLECT, 0);
    }
}

BOOL KScriptManager::ExecuteGMCScript(const char cszGmName[], const char cszScript[])
{
    BOOL        bResult     = false;
	BOOL        bRetCode    = false;
	Lua_State*  L           = NULL;
    size_t      uScriptLen  = 0;

    assert(cszGmName);
    assert(cszScript);
    assert(m_piScript);

	L = m_piScript->GetLuaState();
	KGLOG_PROCESS_ERROR(L);

    lua_pushstring(L, "gm_name");
    lua_pushstring(L, cszGmName);
    lua_settable(L, LUA_GLOBALSINDEX);

    uScriptLen = strlen(cszScript);
    KGLOG_PROCESS_ERROR(uScriptLen > 0);

	bRetCode = m_piScript->LoadFromBuffer(0, "_GMC_", cszScript, (DWORD)uScriptLen);
	KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
	return bResult;
}

void KScriptManager::SafeCallBegin(int* pIndex)
{
	assert(m_piScript);
	m_piScript->SafeCallBegin(pIndex);
}

void KScriptManager::SafeCallEnd(int nIndex)
{
	assert(m_piScript);
	m_piScript->SafeCallEnd(nIndex);
}

BOOL KScriptManager::GetValuesFromStack(const char * pszFormat , ...)
{
    BOOL bResult = false;

    assert(m_piScript);

    va_list vlist;
    va_start(vlist, pszFormat);

    bResult = m_piScript->GetValuesFromStack(pszFormat, vlist);

    va_end(vlist);

    return bResult;
}

BOOL KScriptManager::Call(DWORD dwScriptID, const char* pszFuncName, int nResults)
{
    BOOL bResult  = false;
	BOOL bRetCode = false;

	assert(nResults >= 0);
    assert(pszFuncName);

	bRetCode = m_piScript->CallFunction(dwScriptID, pszFuncName, nResults);
	KG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
	return bResult;
}

BOOL KScriptManager::Call(const char* pszScriptName, const char* pszFuncName, int nResults)
{
    BOOL    bResult     = false;
	BOOL    bRetCode    = false;
	DWORD   dwScriptID  = 0;
	
	assert(pszScriptName);
    assert(nResults >= 0);

	dwScriptID = g_FileNameHash(pszScriptName);

	bRetCode = Call(dwScriptID, pszFuncName, nResults);
	KG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
	return bResult;
}

BOOL KScriptManager::IsFuncExist(DWORD dwScriptID, const char* pszFuncName)
{
	assert(m_piScript);

	return m_piScript->IsFuncExist(dwScriptID, pszFuncName);
}

BOOL KScriptManager::IsFuncExist(const char* pszScriptName, const char* pszFuncName)
{
	DWORD dwScriptID = 0;

	assert(m_piScript);

	dwScriptID = g_FileNameHash(pszScriptName);

	return m_piScript->IsFuncExist(dwScriptID, pszFuncName);
}

BOOL KScriptManager::IsScriptExist(DWORD dwScriptID)
{
	return m_piScript->IsScriptExist(dwScriptID);
}

BOOL KScriptManager::IsScriptExist(const char cszScriptName[])
{
	BOOL    bResult     = false;
	DWORD   dwScriptID  = 0;

	assert(cszScriptName);

	dwScriptID = g_FileNameHash(cszScriptName);

	bResult = IsScriptExist(dwScriptID);
	KG_PROCESS_ERROR(bResult);

	bResult = true;
Exit0:
	return bResult;
}


// 通过文件扩展名判断是否是Game center需要加载的Lua脚本
BOOL KScriptManager::IsLuaScriptFile(const char cszFileName[])
{
    BOOL        bResult     = false;
    int         nRetCode    = 0;
    char        szExtName[] = ".lua";
    size_t      uExtNameLen = sizeof(szExtName) - 1;
    size_t      uLength     = 0;
    const char* pszPos      = NULL;

    uLength = strlen(cszFileName);
    KG_PROCESS_ERROR(uLength > uExtNameLen);

    pszPos = cszFileName + uLength - uExtNameLen;

    nRetCode = stricmp(pszPos, szExtName);
    KG_PROCESS_ERROR(nRetCode == 0);

    bResult = true;
Exit0:
    return bResult;
}

#if (defined(_MSC_VER) || defined(__ICL))   //WIN32
BOOL KScriptManager::LoadScripts(ILuaScriptEx* piScript, const char cszDir[])
{
    BOOL            bResult     = false;
	BOOL			bRetCode    = false;
    HANDLE          hFind       = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATA FindFileData;
    char            szPath[MAX_PATH];

    snprintf(szPath, sizeof(szPath), "%s\\*", cszDir);
    szPath[sizeof(szPath) - 1] = '\0';

    hFind = FindFirstFile(szPath, &FindFileData);
    KGLOG_PROCESS_ERROR(hFind != INVALID_HANDLE_VALUE);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		KGLogPrintf(KGLOG_ERR, "[Lua] Failed to file file: %s\n", szPath);
	}
	

    do 
    {
        if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            if (strcmp(FindFileData.cFileName, ".") != 0 && strcmp(FindFileData.cFileName, "..") != 0)
            {
                snprintf(szPath, sizeof(szPath), "%s\\%s", cszDir, FindFileData.cFileName);
                szPath[sizeof(szPath) - 1] = '\0';

                LoadScripts(piScript, szPath);
            }
        }
        else
        {
            snprintf(szPath, sizeof(szPath), "%s\\%s", cszDir, FindFileData.cFileName);
            szPath[sizeof(szPath) - 1] = '\0';

            bRetCode = IsLuaScriptFile(szPath);
            if (bRetCode)
            {
                bRetCode = piScript->LoadFromFile(szPath);
                if (!bRetCode)
                {
                    KGLogPrintf(KGLOG_ERR, "[Lua] Failed to load file: %s\n", szPath);
                }
            }
        }

    } while (FindNextFile(hFind, &FindFileData));

    bResult = true;
Exit0:
    if (hFind != INVALID_HANDLE_VALUE)
    {
        FindClose(hFind);
        hFind = INVALID_HANDLE_VALUE;
    }
    return bResult;
}
#else
BOOL KScriptManager::LoadScripts(ILuaScriptEx* piScript, const char cszDir[])
{
    BOOL            bResult     = false;
	BOOL			bRetCode    = false;
	DIR*            pDir        = NULL;
	struct dirent*  pDirNode    = NULL;
    char            szPath[MAX_PATH];

	pDir = opendir(cszDir);
    KGLOG_PROCESS_ERROR(pDir);

	while (pDirNode = readdir(pDir)) 
    {
		if(strcmp(pDirNode->d_name, ".") == 0 || strcmp(pDirNode->d_name, "..") == 0) 
            continue;

		if(pDirNode->d_type == DT_DIR) 
        {
            snprintf(szPath, sizeof(szPath), "%s/%s", cszDir, pDirNode->d_name);
            szPath[sizeof(szPath) - 1] = '\0';

			LoadScripts(piScript, szPath);

            continue;
		}

        snprintf(szPath, sizeof(szPath), "%s/%s", cszDir, pDirNode->d_name);
        szPath[sizeof(szPath) - 1] = '\0';

        bRetCode = IsLuaScriptFile(szPath);
        if (bRetCode)
        {
            bRetCode = piScript->LoadFromFile(szPath);
            if (!bRetCode)
            {
                KGLogPrintf(KGLOG_ERR, "[Lua] Failed to load file: %s\n", szPath);
            }
        }
	}
    
    bResult = true;
Exit0:
    if (pDir != NULL)
    {
        closedir(pDir);
        pDir = NULL;
    }
    return bResult;
}

#endif

BOOL KScriptManager::Load()
{
    BOOL            bResult    = false;
    BOOL            bRetCode   = false;
    ILuaScriptEx*   piScript   = NULL;

	piScript = CreateLuaInterface(&KMemory::LuaAllocator, NULL);
	KGLOG_PROCESS_ERROR(piScript);

    bRetCode = RegisterScriptFunctions(piScript);
    KGLOG_PROCESS_ERROR(bRetCode);

    piScript->RegisterConstList(g_LuaCenterConstList);

    Luna<KGPQ>::Register(piScript->GetLuaState());
    Luna<KGlobalCustomDataManager>::Register(piScript->GetLuaState());
    Luna<KGlobalSystemValueManager>::Register(piScript->GetLuaState());
    Luna<KRole>::Register(piScript->GetLuaState());
    Luna<KTong>::Register(piScript->GetLuaState());
    Luna<KActivityManager>::Register(piScript->GetLuaState());
    Luna<KMentorCenter>::Register(piScript->GetLuaState());
    Luna<KMapInfo>::Register(piScript->GetLuaState());
    Luna<KMapCopy>::Register(piScript->GetLuaState());

    bRetCode = LoadScripts(piScript, CENTER_SCRIPTS_DIR);
    KGLOG_PROCESS_ERROR(bRetCode);

    lua_gc(piScript->GetLuaState(), LUA_GCRESTART, 0);
    lua_gc(piScript->GetLuaState(), LUA_GCSETPAUSE, 100);
    lua_gc(piScript->GetLuaState(), LUA_GCSETSTEPMUL, 150);

    KG_COM_RELEASE(m_piScript);
    m_piScript = piScript;
    piScript = NULL;

    bResult = true;
Exit0:
    KG_COM_RELEASE(piScript);
    return bResult;
}
