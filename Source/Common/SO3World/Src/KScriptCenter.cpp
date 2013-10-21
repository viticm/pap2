#include "stdafx.h"
#include "KScriptFuncList.h"
#include "Global.h"
#include "KRegion.h"
#include "KScene.h"
#include "KSO3World.h"
#include "KRecipe.h"
#include "KProfession.h"
#include "KPlayer.h"
#include "KNpc.h"
#include "KDoodad.h"
#include "KItemInfo.h"
#include "KTeamClient.h"
#include "KCenterRemote.h"
#include "KScriptServer.h"
#include "KScriptClient.h"
#include "Engine/KG_Time.h"

#ifndef WIN32
	#include <unistd.h>
	#include <sys/types.h>
	#include <dirent.h>
#else 
	#include <io.h>
	#include <direct.h>
#endif

#define	SCRIPT_MAX_CACHE	256

extern KLuaConstList g_LuaConstList[];

KScriptCenter::KScriptCenter(void)
{
	m_piScript         = NULL;
    m_bStatFlag        = false;
    m_bReloadAllFlag   = false;
}

KScriptCenter::~KScriptCenter(void)
{
    assert(m_piScript == NULL);
}

BOOL KScriptCenter::Init()
{
    BOOL        bResult         = false;
	BOOL        bRetCode        = false;

    m_piScript = CreateScriptHolder();
    KGLOG_PROCESS_ERROR(m_piScript);

#ifdef _SERVER
    KGLogPrintf(KGLOG_INFO, "Search for scripts files ... ...");

    bRetCode = SearchForScriptList(SCRIPT_DIR);
    KGLOG_PROCESS_ERROR(bRetCode);
    
    if (!g_pSO3World->m_bFastBootMode)
    {
        DWORD                   dwScriptID      = 0;
        KSCRIPT_TABLE::iterator it              = m_ScriptTable.begin();
        int                     nScriptCount    = (int)m_ScriptTable.size();
        int                     nIndex          = 1;

        for (;it != m_ScriptTable.end(); ++it, ++nIndex)
        {
            #if (defined(_MSC_VER) || defined(__ICL))   //WIN32
            cprintf("******************************>: Load scripts : %d/%d\r", nIndex, nScriptCount);
            #endif

            dwScriptID = m_piScript->LoadFromFile(it->second.c_str());
            KGLOG_PROCESS_ERROR(dwScriptID == it->first);
        }

        #if (defined(_MSC_VER) || defined(__ICL))   //WIN32
        cprintf("\n");
        #endif
    }

    KGLogPrintf(KGLOG_INFO, "%u scripts searched !", m_ScriptTable.size());
#else
    bRetCode = LoadScriptList();
    KGLOG_PROCESS_ERROR(bRetCode);
#endif

	bResult = true;
Exit0:
    if (!bResult)
    {
        KG_COM_RELEASE(m_piScript);
    }
	return bResult;
}

void KScriptCenter::UnInit()
{
    StatEnd();

    KG_COM_RELEASE(m_piScript);
    m_ScriptTable.clear();
}

void KScriptCenter::Activate()
{
    if (m_bReloadAllFlag)
    {
        ReloadAll();

        m_bReloadAllFlag = false;
    }

    if (g_pSO3World->m_nGameLoop % (GAME_FPS * 8) == 0)
    {
        lua_State* pLuaState = m_piScript->GetLuaState();
        lua_gc(pLuaState, LUA_GCCOLLECT, 0);
    }
}

#if defined(_SERVER)
BOOL KScriptCenter::ExecutePlayerScript(KPlayer* pPlayer, const char* pszScript)
{
    BOOL        bResult     = false;
	BOOL        bRetCode    = false;
	Lua_State*  L           = NULL;

	assert(pPlayer);
	assert(pszScript);
	assert(m_piScript);

	L = m_piScript->GetLuaState();
	KGLOG_PROCESS_ERROR(L);

	lua_pushstring(L, "player");
	pPlayer->LuaGetObj(L);
	lua_settable(L, LUA_GLOBALSINDEX);

	bRetCode = m_piScript->LoadFromBuffer(0, pPlayer->m_szName, pszScript, (DWORD)strlen(pszScript));
	KGLOG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
	return bResult;
}

BOOL KScriptCenter::ExecuteGMCScript(KPlayer* pPlayer, const char* pszGmName, const char* pszScript)
{
    BOOL        bResult     = false;
	BOOL        bRetCode    = false;
	Lua_State*  L           = NULL;

    assert(pszGmName);
	assert(pszScript);
	assert(m_piScript);

	L = m_piScript->GetLuaState();
	KGLOG_PROCESS_ERROR(L);

    lua_pushstring(L, "player");
    if (pPlayer)
        pPlayer->LuaGetObj(L);
    else
        lua_pushnil(L);
    lua_settable(L, LUA_GLOBALSINDEX);

    lua_pushstring(L, "gm_name");
    lua_pushstring(L, pszGmName);
    lua_settable(L, LUA_GLOBALSINDEX);

    bRetCode = m_piScript->LoadFromBuffer(0, "_GMC_", pszScript, (DWORD)strlen(pszScript));
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}
#endif	//_SERVER

void KScriptCenter::StatBegin()
{
    m_bStatFlag       = true;
    m_StatTable.clear();
    return;
}

void KScriptCenter::StatEnd()
{
    if (!m_bStatFlag)
        return;

    OutputStatInfo();

    m_StatTable.clear();
    m_bStatFlag = false;

    return;
}


BOOL KScriptCenter::ReloadAll()
{
    BOOL            bResult     = false;
    BOOL            bRetCode    = false;
    ILuaScriptEx*   piScript    = NULL;

    StatEnd();

    piScript = CreateScriptHolder();
    KGLOG_PROCESS_ERROR(piScript);

    m_ScriptTable.clear();
    
#ifdef _SERVER
    bRetCode = SearchForScriptList(SCRIPT_DIR);
    KGLOG_PROCESS_ERROR(bRetCode);
#else
    bRetCode = LoadScriptList();
    KGLOG_PROCESS_ERROR(bRetCode);
#endif

    KG_COM_RELEASE(m_piScript);
    m_piScript = piScript;

    bResult = true;
Exit0:
    if (!bResult)
    {
        KG_COM_RELEASE(piScript);
    }
    return bResult;
}

BOOL KScriptCenter::ReloadScript(const char* pszFileName)
{
    BOOL            bResult                 = false;
    BOOL            bRetCode                = false;
    IFile*          piFile                  = NULL;
    char*           pszBuffer               = NULL;
    DWORD           dwScriptID              = 0;
    DWORD           dwFileSize              = 0;
    DWORD           dwReadSize              = 0;
    KLuaScriptData* pScriptData             = NULL;
    char            szPathName[MAX_PATH];
    char            szFileName[MAX_PATH];

    assert(pszFileName);

    piFile = g_OpenFile(pszFileName);
    KG_PROCESS_ERROR(piFile);

    dwFileSize = piFile->Size();

    pszBuffer = new char[dwFileSize];
    KG_PROCESS_ERROR(pszBuffer);

    dwReadSize = piFile->Read(pszBuffer, dwFileSize);
    KG_PROCESS_ERROR(dwReadSize == dwFileSize);

    g_GetFullPath(szPathName, pszFileName);

    bRetCode = g_GetFilePathFromFullPath(szFileName, szPathName);
    KG_PROCESS_ERROR(bRetCode);

    dwScriptID = g_FileNameHash(szFileName);

    bRetCode = IsScriptExist(dwScriptID);
    KG_PROCESS_ERROR(bRetCode);

    bRetCode = m_piScript->LoadFromBuffer(dwScriptID, szPathName, pszBuffer, dwFileSize);
    KG_PROCESS_ERROR(bRetCode);

    pScriptData = m_piScript->GetScriptData(dwScriptID);
    if (pScriptData)
    {
        std::vector<DWORD>::iterator it = pScriptData->vecIncludeScriptID.begin();
        std::vector<DWORD>::iterator itEnd = pScriptData->vecIncludeScriptID.end();

        for (NULL; it != itEnd; ++it)
        {
            KLuaScriptData* pNewScriptData = NULL;

            pNewScriptData = m_piScript->GetScriptData(*it);
            if (pNewScriptData)
                ReloadScript(pNewScriptData->szName);
        }
    }

    bResult = true;
Exit0:
    if (!bResult)
    {
        KGLogPrintf(KGLOG_ERR, "[Lua] Failed to reload file \"%s\" !\n", pszFileName);
    }
    KG_DELETE_ARRAY(pszBuffer);
    KG_COM_RELEASE(piFile);
    return bResult;
}

void KScriptCenter::SafeCallBegin(int* pIndex)
{
	assert(m_piScript);

	m_piScript->SafeCallBegin(pIndex);
}

void KScriptCenter::SafeCallEnd(int nIndex)
{
	assert(m_piScript);

	m_piScript->SafeCallEnd(nIndex);
}

BOOL KScriptCenter::GetValuesFromStack(const char * pszFormat , ...)
{
	BOOL bResult = false;

	assert(m_piScript);

	va_list vlist;
	va_start(vlist, pszFormat);

	bResult = m_piScript->GetValuesFromStack(pszFormat, vlist);

	va_end(vlist);

	return bResult;
}

BOOL KScriptCenter::CallFunction(DWORD dwScriptID, const char* pszFuncName, int nResults)
{
    BOOL                                bResult             = false;
	BOOL                                bRetCode            = false;
    uint64_t                            uTime               = 0;
    BOOL                                bStatFlag           = m_bStatFlag;

	assert(m_piScript);
    assert(pszFuncName);
	assert(nResults >= 0);

    bRetCode = IsScriptExist(dwScriptID);
    KGLOG_PROCESS_ERROR(bRetCode);

    if (bStatFlag)
    {
        uTime = RDTSC();
    }

	bRetCode = m_piScript->CallFunction(dwScriptID, pszFuncName, nResults);
	KGLOG_PROCESS_ERROR(bRetCode);

    if (bStatFlag)
    {
        uTime = RDTSC() - uTime;

        UpdateStatInfo(dwScriptID, pszFuncName, uTime);
    }

    bResult = true;
Exit0:
	return bResult;
}

BOOL KScriptCenter::CallFunction(const char* pszScriptName, const char* pszFuncName, int nResults)
{
    BOOL bResult  = false;
	BOOL bRetCode = false;
	DWORD dwScriptID = 0;
	
	assert(m_piScript);
	assert(pszScriptName);

	dwScriptID = g_FileNameHash(pszScriptName);

	bRetCode = CallFunction(dwScriptID, pszFuncName, nResults);
	KG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
	return bResult;
}

BOOL KScriptCenter::IsFuncExist(DWORD dwScriptID, const char* pszFuncName)
{
    BOOL bResult    = false;
    BOOL bRetCode   = false;

	assert(m_piScript);
    assert(pszFuncName);

    bRetCode = IsScriptExist(dwScriptID);
    KG_PROCESS_ERROR(bRetCode);

	bRetCode = m_piScript->IsFuncExist(dwScriptID, pszFuncName);
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KScriptCenter::IsFuncExist(const char* pszScriptName, const char* pszFuncName)
{
	DWORD   dwScriptID = 0;

	assert(m_piScript);
    assert(pszScriptName);
    assert(pszFuncName);

	dwScriptID = g_FileNameHash(pszScriptName);

	return IsFuncExist(dwScriptID, pszFuncName);
}

BOOL KScriptCenter::IsScriptExist(DWORD dwScriptID)
{
	BOOL        bResult     = false;
    BOOL        bRetCode    = false;
    const char* pszFileName = NULL;

	assert(m_piScript);

    pszFileName = GetScriptNameByID(dwScriptID);
    KG_PROCESS_ERROR(pszFileName);

	bRetCode = m_piScript->IsScriptExist(dwScriptID);
	if (!bRetCode)
	{
		dwScriptID = m_piScript->LoadFromFile(pszFileName);
		KGLOG_PROCESS_ERROR(dwScriptID);
	}

    bResult = true;
Exit0:
	return bResult;
}

BOOL KScriptCenter::IsScriptExist(const char cszScriptName[])
{
	BOOL    bResult     = false;
	DWORD   dwScriptID  = 0;

	assert(m_piScript);
	assert(cszScriptName);

	dwScriptID = g_FileNameHash(cszScriptName);

	return IsScriptExist(dwScriptID);
}

// 获取当前正在被调用的脚本
DWORD KScriptCenter::GetActiveScriptID()
{
	assert(m_piScript);

	return m_piScript->GetActiveScriptID();
}

lua_State* KScriptCenter::GetLuaState(void)
{
	assert(m_piScript);

	return m_piScript->GetLuaState();
}

BOOL KScriptCenter::LoadScriptList()
{
    BOOL        bResult     = false;
	BOOL        bRetCode    = false;
	ITabFile*   piTabFile   = NULL;
	int         nHeight     = 0;
	int         nIndex      = 0;
    DWORD       dwScriptID  = 0;
	char        szScriptFilePath[MAX_SCIRPT_NAME_LEN];

	piTabFile = g_OpenTabFile(SCRIPT_DIR"/ScriptList.tab");
	KGLOG_PROCESS_ERROR(piTabFile);

	nHeight = piTabFile->GetHeight();
	KGLOG_PROCESS_ERROR(nHeight >= 1);
	
	for (nIndex = 2; nIndex <= nHeight; nIndex++)
	{
		bRetCode = piTabFile->GetString(nIndex, "FilePath", "", szScriptFilePath, MAX_SCIRPT_NAME_LEN);
		KGLOG_PROCESS_ERROR(bRetCode == 1);
	
	    dwScriptID = g_FileNameHash(szScriptFilePath);

        m_ScriptTable[dwScriptID] = szScriptFilePath;
	}

	bResult = true;
Exit0:
    KG_COM_RELEASE(piTabFile);
	return bResult;
}

static BOOL IsLuaFile(const char cszFileName[])
{
    BOOL        bResult    = false;
    const char* pcExtPos   = NULL;
    const char* pcPos      = NULL;
	char        szExtName[8];

    pcPos = cszFileName;
    while (*pcPos)
    {
        if (*pcPos == '.')
        {
            pcExtPos = pcPos + 1;
        }
        pcPos++;
    }

    KG_PROCESS_ERROR(pcExtPos != NULL && pcPos - pcExtPos < sizeof(szExtName));

    strcpy(szExtName, pcExtPos);

    if (
        stricmp(szExtName, "LUA") == 0 ||
        stricmp(szExtName, "LH") == 0 ||
        stricmp(szExtName, "LS") == 0 
    )
    {
        bResult = true;
    }

Exit0:
	return bResult;
}

#if (defined(_MSC_VER) || defined(__ICL))   //WIN32
BOOL KScriptCenter::SearchForScriptList(const char cszDir[])
{
    BOOL            bResult     = false;
	int			    nRetCode    = false;
    HANDLE          hFind       = INVALID_HANDLE_VALUE;
    DWORD           dwScriptID  = 0;
    WIN32_FIND_DATA FindFileData;
    char            szPath[MAX_PATH];

    nRetCode = snprintf(szPath, sizeof(szPath), "%s\\*", cszDir);
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)sizeof(szPath));

    hFind = FindFirstFile(szPath, &FindFileData);
    KGLOG_PROCESS_ERROR(hFind != INVALID_HANDLE_VALUE);

    do 
    {
        if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            if (
                strcmp(FindFileData.cFileName, ".") != 0 && 
                strcmp(FindFileData.cFileName, "..") != 0 &&
                strcmp(FindFileData.cFileName, ".svn") != 0
            )
            {
                nRetCode = snprintf(szPath, sizeof(szPath), "%s\\%s", cszDir, FindFileData.cFileName);
                KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)sizeof(szPath));

                nRetCode = SearchForScriptList(szPath);
                KGLOG_PROCESS_ERROR(nRetCode);
            }
        }
        else
        {
            nRetCode = snprintf(szPath, sizeof(szPath), "%s\\%s", cszDir, FindFileData.cFileName);
            KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)sizeof(szPath));

            nRetCode = IsLuaFile(szPath);
            if (nRetCode)
            {
	            dwScriptID = g_FileNameHash(szPath);
                m_ScriptTable[dwScriptID] = szPath;
            }
        }
    } while(FindNextFile(hFind, &FindFileData));

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
BOOL KScriptCenter::SearchForScriptList(const char cszDir[])
{
    BOOL            bResult     = false;
	int			    nRetCode    = false;
	DIR*            pDir        = NULL;
    DWORD           dwScriptID  = 0;
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
            nRetCode = snprintf(szPath, sizeof(szPath), "%s/%s", cszDir, pDirNode->d_name);
            KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)sizeof(szPath));

			nRetCode = SearchForScriptList(szPath);
            KGLOG_PROCESS_ERROR(nRetCode);

            continue;
		}

        nRetCode = snprintf(szPath, sizeof(szPath), "%s/%s", cszDir, pDirNode->d_name);
        KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)sizeof(szPath));

        nRetCode = IsLuaFile(szPath);
        if (nRetCode)
        {
            dwScriptID = g_FileNameHash(szPath);
            m_ScriptTable[dwScriptID] = szPath;
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

const char* KScriptCenter::GetScriptNameByID(DWORD dwScriptID)
{
    KSCRIPT_TABLE::iterator it = m_ScriptTable.find(dwScriptID);
    if (it != m_ScriptTable.end())
    {
        return it->second.c_str();
    }

	return NULL;
}

BOOL KScriptCenter::LoadScript(const char cszFileName[])
{
    BOOL    bResult     = false;
	DWORD   dwScriptID  = 0;
	int     nTopIndex   = 0;

	assert(m_piScript);

	dwScriptID = m_piScript->LoadFromFile(cszFileName);
	KGLOG_PROCESS_ERROR(dwScriptID != 0);

	SafeCallBegin(&nTopIndex);
	if (IsFuncExist(dwScriptID, SCRIPT_ON_LOAD))
	{
		CallFunction(dwScriptID, SCRIPT_ON_LOAD, 0);
	}
	SafeCallEnd(nTopIndex);

	bResult = true;
Exit0:
    if (!bResult)
    {
        KGLogPrintf(KGLOG_ERR, "[Lua] Failed to Load file \"%s\" !\n", cszFileName);
    }
	return bResult;
}

ILuaScriptEx* KScriptCenter::CreateScriptHolder()
{
    ILuaScriptEx*   piResult        = NULL;
	BOOL            bRetCode        = false;
    ILuaScriptEx*   piScript        = NULL;
	KLuaFunc*       pFunList        = NULL;
    int             nFuncNum        = 0;
    lua_State*      pLuaState       = NULL;

    piScript = CreateLuaInterface(&KMemory::LuaAllocator, NULL);
	KGLOG_PROCESS_ERROR(piScript);

	nFuncNum = KScriptFuncList::GetBaseFuncList(pFunList);
	piScript->RegisterFunctions(pFunList, nFuncNum);

	piScript->RegisterConstList(g_LuaConstList);

    pLuaState = piScript->GetLuaState();
    KGLOG_PROCESS_ERROR(pLuaState);

	Luna<KScene>::Register(pLuaState);
	Luna<KCell>::Register(pLuaState);
	Luna<KPlayer>::Register(pLuaState);
	Luna<KNpc>::Register(pLuaState);
	Luna<KNpcTemplate>::Register(pLuaState);
	Luna<KDoodad>::Register(pLuaState);
	Luna<KDoodadTemplate>::Register(pLuaState);
	Luna<KItem>::Register(pLuaState);
	Luna<KSkill>::Register(pLuaState);
	Luna<KQuestInfo>::Register(pLuaState);
	Luna<KShop>::Register(pLuaState);
	Luna<KProfession>::Register(pLuaState);
	Luna<KCraftRecipe>::Register(pLuaState);
	Luna<KCraftEnchant>::Register(pLuaState);
	Luna<KCraftCollection>::Register(pLuaState);
    Luna<KCraftRead>::Register(pLuaState);
    Luna<KCraftCopy>::Register(pLuaState);
    Luna<KROUTE_NODE>::Register(pLuaState);
    Luna<KCustomEquipInfo>::Register(pLuaState);
    Luna<KOtherItemInfo>::Register(pLuaState);

    Luna<KAILogic>::Register(pLuaState);
    Luna<KAIState>::Register(pLuaState);
    Luna<KAIAction>::Register(pLuaState);

    Luna<KCampInfo>::Register(pLuaState);

    Luna<KGPQ>::Register(pLuaState);

#ifdef _SERVER
    Luna<KBUFF_LIST_NODE>::Register(pLuaState);
#endif

#ifdef _CLIENT
    Luna<KTeamClient>::Register(pLuaState);
#endif

#ifdef _CLIENT
    Luna<KAuctionClient>::Register(pLuaState);
    Luna<KTongClient>::Register(pLuaState);
    Luna<KGameCardClient>::Register(pLuaState);
    Luna<KMailClient>::Register(pLuaState);
	Luna<KMailInfo>::Register(pLuaState);
#endif

#ifdef _SERVER
    Luna<KScriptServer>::Register(pLuaState);
    Luna<KGlobalSystemValueCache>::Register(pLuaState);
    Luna<KMentorCache>::Register(pLuaState);
#endif
    Luna<KTarget>::Register(pLuaState);

    lua_gc(pLuaState, LUA_GCRESTART, 0);
    lua_gc(pLuaState, LUA_GCSETPAUSE, 100);
    lua_gc(pLuaState, LUA_GCSETSTEPMUL, 150);

    piResult = piScript;
Exit0:
    if (!piResult)
    {
        KG_COM_RELEASE(piScript);
    }
    return piResult;
}

BOOL KScriptCenter::UpdateStatInfo(DWORD dwScriptID, const char cszFuncName[], uint64_t uCostTime)
{
    BOOL                bResult         = false;
    KStatInfoNode*      pNode           = NULL;
    char                szKey[128];
    KFUNC_STAT_TABLE::iterator it;

    snprintf(szKey, sizeof(szKey), "%u:%s", dwScriptID, cszFuncName);
    szKey[sizeof(szKey) - 1] = '\0';
    
    it = m_StatTable.find(szKey);
    if (it == m_StatTable.end())
    {
        std::pair<KFUNC_STAT_TABLE::iterator, BOOL> InsRet = m_StatTable.insert(
            std::make_pair(kstring(szKey), KStatInfoNode())
        );
        KGLOG_PROCESS_ERROR(InsRet.second);

        it = InsRet.first;

        it->second.dwScriptID = dwScriptID;
        it->second.sFunction  = cszFuncName;
    }

    pNode = &it->second;

    pNode->dwCount++;
    pNode->uTime += uCostTime;

    bResult = true;
Exit0:
    return true;
}

void KScriptCenter::OutputStatInfo()
{
    FILE* pFile = NULL;

    pFile = fopen("script_stat.txt", "w");
    KGLOG_PROCESS_ERROR(pFile);

    fprintf(pFile, "Function\tTime\tCount\tFile\n");

    for (KFUNC_STAT_TABLE::iterator it = m_StatTable.begin(); it != m_StatTable.end(); ++it)
    {
        const char* pszFile = GetScriptNameByID(it->second.dwScriptID);
        const char* pszFunc = it->second.sFunction.c_str();

        assert(pszFunc);

        if (pszFile == NULL)
        {
            pszFile = "???";
        }

        fprintf(pFile, "%s\t%llu\t%u\t%s\n", pszFunc, it->second.uTime, it->second.dwCount, pszFile);
    }

Exit0:
    if (pFile)
    {
        fclose(pFile);
        pFile = NULL;
    }
    return;
}
