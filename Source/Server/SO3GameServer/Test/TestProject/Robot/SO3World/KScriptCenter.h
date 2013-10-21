/************************************************************************/
/* 脚本中心								                                */
/* Copyright : Kingsoft 2004										    */
/* Author	 : Zhu Jianqiu												*/
/* History	 :															*/
/*		2005.03.02	Create												*/
/* Comment	 :															*/
/*		封装了对脚本的操作和存储										*/
/************************************************************************/
#ifndef _KSCRIPT_CENTER_H_
#define _KSCRIPT_CENTER_H_

#if defined(_SERVER) | defined(_EDITOR)
	#include <map>
#endif	//_SERVER or _EDITOR

#if defined(_CLIENT)
	#include "KIDCache.h"
#endif	//_CLIENT

class KSO3World;
class KCharacter;
class KPlayer;

class KScriptCenter
{
public:
	KScriptCenter(void);
	~KScriptCenter(void);

	BOOL Init(void);
	BOOL UnInit(void);

	KLuaScriptEx* GetScript(const char* pszFileName);
	KLuaScriptEx* GetScript(DWORD dwScriptID);

	//载入脚本文件
	BOOL LoadScript(const char* pszFileName);

#if defined(_SERVER) | defined(_EDITOR)
	//执行脚本命令字符串，主要给GM指令用
	BOOL ExecuteScript(KPlayer* pPlayer, const char* pszScript);
#endif	//_SERVER or _EDITOR

#if defined(_CLIENT)
	//执行脚本命令字符串
	BOOL ExecuteScript(const char* pszScript);
#endif	//_CLIENT

private:
	typedef std::map<DWORD, KLuaScriptEx> SCRIPT_LIST;
	SCRIPT_LIST					m_ScriptList;			//脚本列表
	//载入指定目录下的所有脚本
	void LoadScriptInDirectory(LPSTR lpszRootDir, LPSTR lpszSubDir);

	KLuaScriptEx	m_TempScript;
};

extern KScriptCenter	g_ScriptCenter;
#endif	//_KSCRIPT_CENTER_H_