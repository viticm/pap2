// LuaRuntime.cpp : 定义 DLL 的初始化例程。
//
#include "stdafx.h"
#include "LuaRuntime.h"
#include "LuaPlus.h"
using namespace LuaPlus;

LuaStateOwner lua(true); 
BOOL __stdcall LuaInit(const char * szLua, char * * szOutput)
{
	try
	{
		if (lua->DoString(szLua) != 0)
		{
			* szOutput = (char *)lua->Stack(-1).GetString();
			return FALSE;
		}
	}
	catch (LuaException* e)
	{
		* szOutput = (char *)e->GetErrorMessage();
		return FALSE;
	}
	return TRUE;
}

BOOL __stdcall LuaRun(char * szFuncName, char * szCode, int nPos, int nIsDelete, char * szSetting, char * * szOutput)
{
	LuaObject luaObj = lua->GetGlobal(szFuncName);
	if (!luaObj.IsFunction())
	{
		* szOutput = "找不到指定函数";
		return FALSE;
	}
	try
	{
		lua->DoString("byrunrime_szCode='' byruntime_nPos=0 byruntime_nIsDelete=0 byruntime_szSetting=''");
		lua->GetGlobals().Set("byrunrime_szCode", szCode);
		lua->GetGlobals().Set("byruntime_nPos", nPos);
		lua->GetGlobals().Set("byruntime_nIsDelete", nIsDelete);
		lua->GetGlobals().Set("byruntime_szSetting", szSetting);
		char acbuffer[256] = {0};
		char * pcbuffer = acbuffer;
		strcpy(acbuffer, "byruntime_szRet =");
		pcbuffer += strlen(pcbuffer);
		strcpy(pcbuffer, szFuncName);
		pcbuffer += strlen(pcbuffer);
		strcpy(pcbuffer, "(byrunrime_szCode, byruntime_nPos, byruntime_nIsDelete, byruntime_szSetting)");
		if(lua->DoString(acbuffer) != 0)
		{
			* szOutput = (char *)lua->Stack(-1).GetString();
			return FALSE;
		}
		LuaObject loRet = lua->GetGlobal("byruntime_szRet");
		if(!loRet.IsString())
		{
			* szOutput = "函数返回值必须为字符串";
			return FALSE;
		}
		* szOutput = (char *)loRet.GetString();
	}
	catch (LuaException* e)
	{
		* szOutput = (char *)e->GetErrorMessage();
		return FALSE;
	}
	return TRUE;
}


// CLuaRuntimeApp

BEGIN_MESSAGE_MAP(CLuaRuntimeApp, CWinApp)
END_MESSAGE_MAP()


// CLuaRuntimeApp 构造

CLuaRuntimeApp::CLuaRuntimeApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CLuaRuntimeApp 对象

CLuaRuntimeApp theApp;


// CLuaRuntimeApp 初始化

BOOL CLuaRuntimeApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}
