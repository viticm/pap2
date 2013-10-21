#include "stdafx.h"
#include "CheckGameCheat.h"
#include "md5.h"


#define SS_MAP std::map<TSTRING, TSTRING>
#define SS_MAP_ITERATOR std::map<TSTRING, TSTRING>::iterator
#define SS_PAIR std::pair<TSTRING, TSTRING>

#define MAX_MSG_LEN 512

//初始化盗号模块的黑名单
BOOL InitBlackNameListMap(SS_MAP &mapBlackNameList);

//反初始化盗号模块的黑名单
BOOL UnInitBlackNameListMap(SS_MAP &mapBlackNameList);

//检查指定模块是否在名单里（模块名称，和md5都相等）
BOOL IsModuleInBlackNameList(LPMODULEENTRY32 lpme, SS_MAP mapBlackNameList);

//检查当前进程中的模块是否有在名单里的
BOOL HasOneModuleInBlackNameList(SS_MAP mapBlackNameList, LPMODULEENTRY32 lpme);

//获取模块的md5
TSTRING GetModuleMD5(LPMODULEENTRY32 lpme);

//是否存在盗号木马
BOOL ExistMaliciousModule();

//检测到存在盗号木马后的处理（向玩家报告木马模块文件路径）
BOOL ProcessTorjanModule(LPMODULEENTRY32 lpmeTorjan);



BOOL ExistMaliciousModule()
{
	BOOL bExist = FALSE;

	SS_MAP mapBlackNameList;

	MODULEENTRY32 me32 = {0};

	InitBlackNameListMap(mapBlackNameList);

	bExist = HasOneModuleInBlackNameList(mapBlackNameList, &me32);

	if (bExist)
	{
		ProcessTorjanModule(&me32);
	}

	UnInitBlackNameListMap(mapBlackNameList);

	return bExist;
}

BOOL HasOneModuleInBlackNameList(SS_MAP mapBlackNameList, LPMODULEENTRY32 lpme)
{
	BOOL bHas = FALSE;

	DWORD         PID    = GetCurrentProcessId();
	DWORD		  dwSize = 0;
	BOOL		  isMod  = 0;

	HANDLE hSnapshotModule = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, PID);
	if (hSnapshotModule) 
	{
		lpme->dwSize = sizeof(MODULEENTRY32);
		isMod = Module32First(hSnapshotModule, lpme);
		while (isMod) 
		{
			BOOL bIsModuleInBlackNameList = FALSE;
			bIsModuleInBlackNameList = IsModuleInBlackNameList(lpme, mapBlackNameList);
			if (bIsModuleInBlackNameList)
			{
				bHas = TRUE;
				break;
			}
			isMod = Module32Next(hSnapshotModule, lpme);
		}
	}
	CloseHandle(hSnapshotModule);

	return bHas;
}

BOOL IsModuleInBlackNameList(LPMODULEENTRY32 lpme, SS_MAP mapBlackNameList)
{
	BOOL bIsIn = FALSE;

	SS_MAP_ITERATOR itr;

	TSTRING strModeName = TSTRING(lpme->szModule);
	itr = mapBlackNameList.find(strModeName);

	if (itr != mapBlackNameList.end())
	{
		TSTRING modmd5 = GetModuleMD5(lpme);

		if (modmd5 == itr->second)
		{
			bIsIn = TRUE;
		}
	}

	return bIsIn;
}

BOOL InitBlackNameListMap(SS_MAP &mapBlackNameList)
{
	BOOL bRet = TRUE;
	mapBlackNameList.clear();
	mapBlackNameList.insert(SS_PAIR("midimap.dll.v","354add79bc27506cfa5385bb43986831"));
	mapBlackNameList.insert(SS_PAIR("midimap.dll","354add79bc27506cfa5385bb43986831"));

	return bRet;
}

BOOL UnInitBlackNameListMap(SS_MAP &mapBlackNameList)
{
	BOOL bRet = TRUE;
	mapBlackNameList.clear();
	return bRet;
}

TSTRING GetModuleMD5(LPMODULEENTRY32 lpme)
{
	BYTE* modBegin = lpme->modBaseAddr;
	
	TSTRING modmd5 = MD5(modBegin + 0x80, 0x80).toString();

	return modmd5;
}

BOOL ProcessTorjanModule(LPMODULEENTRY32 lpmeTorjan)
{
	BOOL bRet = TRUE;

	CHAR szReportText[MAX_MSG_LEN];
	sprintf(szReportText, "%s可能是盗号程序，建议删除之再登录游戏。", lpmeTorjan->szExePath);
	::MessageBox(NULL, szReportText, "重要提示", MB_ICONWARNING);

	return bRet;
}
