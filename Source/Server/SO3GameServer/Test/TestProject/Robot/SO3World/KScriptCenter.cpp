#include "stdafx.h"
#include "KScriptCenter.h"
//#include "KScriptFuncList.h"
#include "Global.h"
#include "KSO3World.h"
#include "KObjectManager.h"
#include "KRegion.h"
#include "KScene.h"

#ifndef WIN32
	#include <unistd.h>
	#include <sys/types.h>
	#include <dirent.h>
#else 
	#include <io.h>
	#include <direct.h>
#endif

#define	SCRIPT_MAX_CACHE	256

KScriptCenter g_ScriptCenter;
//extern KLuaConstList g_LuaConstList[];

KScriptCenter::KScriptCenter(void)
{
}

KScriptCenter::~KScriptCenter(void)
{
}

//初始化
BOOL KScriptCenter::Init()
{
	BOOL bRetCode = FALSE;
	
	char szOldRootPath[MAX_PATH];
	char szRootPath[MAX_PATH];

	m_ScriptList.clear();

	g_SetFilePath("/");
	getcwd(szOldRootPath, MAX_PATH);
	getcwd(szRootPath, MAX_PATH);

#if defined(_SERVER) | defined(_EDITOR)
	LoadScriptInDirectory(szRootPath, SCRIPT_DIR);
#endif	//_SERVER or _EDITOR

	chdir(szOldRootPath);

	//初始化临时脚本
	bRetCode = m_TempScript.Init();
	KGLOG_PROCESS_ERROR(bRetCode);

	//KLuaFunc* pFunList = NULL;
	//int nFuncNum = KScriptFuncList::GetBaseFuncList(pFunList);
	//m_TempScript.RegisterFunctions(pFunList, nFuncNum);

#if defined(_SERVER)
	bRetCode = m_TempScript.RegisterConstList(g_LuaConstList);
	KGLOG_PROCESS_ERROR(bRetCode);

	Luna<KPlayer>::Register(m_TempScript.GetLuaState());
	Luna<KScene>::Register(m_TempScript.GetLuaState());

	bRetCode = m_TempScript.LoadFromFile("Scripts/GM.lua");
	KGLOG_CHECK_ERROR(bRetCode);
#endif	//_SERVER

	return TRUE;

Exit0:

	return FALSE;
}

BOOL KScriptCenter::UnInit(void)
{
	m_TempScript.UnInit();
	m_ScriptList.clear();

	return TRUE;
}

KLuaScriptEx* KScriptCenter::GetScript(const char* pszFileName)
{
	BOOL bRetCode = FALSE;
	SCRIPT_LIST::iterator it;

	KGLOG_PROCESS_ERROR(pszFileName);

	it = m_ScriptList.find(g_FileNameHash(pszFileName));
	if (it == m_ScriptList.end())
	{
		bRetCode = LoadScript(pszFileName);
		KGLOG_PROCESS_ERROR(bRetCode);

		it = m_ScriptList.find((g_FileNameHash(pszFileName)));
		KGLOG_PROCESS_ERROR(it != m_ScriptList.end());
	}

	return &(it->second);

Exit0:

	return NULL;
}

KLuaScriptEx* KScriptCenter::GetScript(DWORD dwScriptID)
{
	BOOL bRetCode = FALSE;
	SCRIPT_LIST::iterator it;

	KG_PROCESS_ERROR(dwScriptID != 0);

	it = m_ScriptList.find(dwScriptID);
	KGLOG_PROCESS_ERROR(it != m_ScriptList.end());

	return &(it->second);

Exit0:

	return NULL;
}

#if defined(_SERVER) | defined(_EDITOR)
//执行脚本命令字符串
BOOL KScriptCenter::ExecuteScript(KPlayer* pPlayer, const char* pszScript)
{
	BOOL bRetCode = FALSE;

	KGLOG_PROCESS_ERROR(pPlayer);
	KGLOG_PROCESS_ERROR(pszScript);

	lua_pushstring(m_TempScript.GetLuaState(), "player");
	pPlayer->LuaGetObj(m_TempScript.GetLuaState());
	lua_settable(m_TempScript.GetLuaState(), LUA_GLOBALSINDEX);

	Luna<KPlayer>::Register(m_TempScript.GetLuaState());

	lua_pushstring(m_TempScript.GetLuaState(), "scene");
	pPlayer->m_pScene->LuaGetObj(m_TempScript.GetLuaState());
	lua_settable(m_TempScript.GetLuaState(), LUA_GLOBALSINDEX);

	Luna<KScene>::Register(m_TempScript.GetLuaState());

	bRetCode = m_TempScript.LoadFromBuffer(pPlayer->m_szName, pszScript, (DWORD)strlen(pszScript));
	KGLOG_PROCESS_ERROR(bRetCode);

	return TRUE;

Exit0:

	return FALSE;
}
#endif	//_SERVER or _EDITOR


//执行脚本命令
#if defined(_CLIENT)
BOOL KScriptCenter::ExecuteScript(const char* pszScript)
{
	BOOL bRetCode = FALSE;

	KGLOG_PROCESS_ERROR(pszScript);

	bRetCode = m_TempScript.LoadFromBuffer("Client Script", pszScript, (DWORD)strlen(pszScript));
	KGLOG_PROCESS_ERROR(bRetCode);

	return TRUE;

Exit0:

	return FALSE;
}
#endif	//_CLIENT

BOOL KScriptCenter::LoadScript(const char* pszFileName)
{
	BOOL bRetCode = FALSE;
	std::pair<SCRIPT_LIST::iterator, bool> InsRet;
	SCRIPT_LIST::iterator it;

	KGLOG_PROCESS_ERROR(pszFileName);

	char szFileName[MAX_PATH];
//	g_ExtractFileName(szFileName, pszFileName);
	bRetCode = g_GetFilePathFromFullPath(szFileName, pszFileName);

	if (!bRetCode)
	{
		strcpy(szFileName, pszFileName);
	}

	DWORD dwScriptID = g_FileNameHash(szFileName);

	it = m_ScriptList.find(dwScriptID);
	if (it != m_ScriptList.end())
		m_ScriptList.erase(it);

	KLuaScriptEx& rScript = m_ScriptList[dwScriptID];

	bRetCode = rScript.Init();
	KGLOG_PROCESS_ERROR(bRetCode);

	//KLuaFunc* pFunList = NULL;
	//int nFuncNum = KScriptFuncList::GetBaseFuncList(pFunList);
	//rScript.RegisterFunctions(pFunList, nFuncNum);

    //bRetCode = rScript.RegisterConstList(g_LuaConstList);
    //KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = rScript.LoadFromFile(szFileName);
	KGLOG_PROCESS_ERROR(bRetCode);

	//记录栈顶信息
	int nTopIndex = 0;
	rScript.SafeCallBegin(&nTopIndex);
	//查询加载函数是否定义
	if (rScript.IsFuncExist(SCRIPT_ON_LOAD))
	{
		//调用函数
		rScript.CallFunction(SCRIPT_ON_LOAD, 0, "");
	}
	//恢复栈顶
	rScript.SafeCallEnd(nTopIndex);

	return TRUE;

Exit0:
	m_ScriptList.erase(dwScriptID);

	return FALSE;
}

void KScriptCenter::LoadScriptInDirectory(LPSTR lpszRootDir, LPSTR lpszSubDir)
{
	int				nFlag;
	char			szRealDir[MAX_PATH];
#ifdef WIN32
	sprintf(szRealDir, "%s/%s", lpszRootDir, lpszSubDir);
#else
	sprintf(szRealDir, "%s/%s", lpszRootDir, lpszSubDir);
	char *ptr = szRealDir;
	while(*ptr) { if(*ptr == '\\') *ptr = '/';  ptr++;  }
#endif

#ifdef WIN32
	if(chdir(szRealDir)) return;
	_finddata_t FindData;
	long dir = (long)_findfirst("*.*", &FindData);
	while(dir != -1) {
		if(strcmp(FindData.name, ".") == 0 || strcmp(FindData.name, "..") == 0)	{
			if(_findnext(dir, &FindData)) break;
			continue;
		}
		if(FindData.attrib == _A_SUBDIR)
		{
			LoadScriptInDirectory(szRealDir, FindData.name);
		}
		else
		{	
			nFlag = 0;
			for (int i = 0; i < (int)strlen(FindData.name);  i++)
			{
				if (FindData.name[i] == '.')
					break;
				if (FindData.name[i] == '\\')
				{
					nFlag = 1;
					break;
				}
			}
			if (nFlag == 1)
			{
				LoadScriptInDirectory(szRealDir, FindData.name);
			}
			else
			{
				char szExt[50];
				if (strlen(FindData.name) >= 4) 
				{
					strcpy(szExt, FindData.name + strlen(FindData.name) - 4);
					_strupr(szExt);
					if (!strcmp(szExt, ".LUA"))
					{
						char szTaskName[MAX_PATH];
						sprintf(szTaskName, "%s/%s", szRealDir, FindData.name);
						if (!LoadScript(szTaskName))
							printf("Failed to load script : %s\n", FindData.name);
						printf(".");
					}
				}
			}
		}
		if(_findnext(dir, &FindData)) break;
	} 
	_findclose(dir);
	chdir(lpszRootDir);
#else
	DIR *dp;
	int i;
	struct dirent *ep;
	if(chdir(szRealDir)) return;
	dp = opendir(".");
	if(dp) {
		while(ep = readdir(dp)) {
			if(strcmp(ep->d_name, ".") == 0 || strcmp(ep->d_name, "..") == 0) continue;

			if(ep->d_type ==4) {
				LoadScriptInDirectory(szRealDir, ep->d_name);
			}
			else {
				nFlag = 0;
				for (i = 0; i < (int)strlen(ep->d_name);  i++)
				{
					if (ep->d_name[i] == '.')
						break;
					if (ep->d_name[i] == '\\')
					{
						nFlag = 1;
						break;
					}
				}
				if (nFlag == 1)
				{
					LoadScriptInDirectory(szRealDir,ep->d_name);
				}
				else
				{
					char szExt[50];
					if (strlen(ep->d_name) >= 4)
					{
						strcpy(szExt, ep->d_name + strlen(ep->d_name) - 4);
						g_StrUpper(szExt);
						///					_strupr(szExt);
						if (!strcmp(szExt, ".LUA"))
						{
							char szTaskName[MAX_PATH];
							strcpy(szTaskName, szRealDir);
							strcat(szTaskName, FindData.name);
							if (!LoadScript(szTaskName))
								printf("Failed to load script : %s\n", FindData.name);
							printf(".");
						}
					}
				}
			}
		}
		closedir(dp);
	}
	chdir(lpszRootDir);
#endif
	printf("\n");
}
