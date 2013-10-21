//
#include "stdafx.h"
#include "CheckGameCheat.h"
#include "class/KGameRabotInfo.h"
#include <process.h>

#define KG_HANDLE_CLOSE(hHandle) \
	do  \
	{   \
	if ((hHandle))   \
		{   \
		::CloseHandle((hHandle));   \
		(hHandle) = NULL;   \
		}   \
	} while (false)

unsigned WINAPI GameRabotThread(void* lpVoid);

KGameRabotInfo GameRabotInfo;

BOOL HastaLaVistaBaby(LPCSTR account, LPCSTR role, LPCSTR region, LPCSTR server)
{
	int nResult  = false;
	int nRetCode = false;
	HANDLE hThread = NULL;

	KG_PROCESS_ERROR(account);
	KG_PROCESS_ERROR(role);
	KG_PROCESS_ERROR(region);
	KG_PROCESS_ERROR(server);

	static BOOL bInited = FALSE;

	if (!bInited)
	{
		GameRabotInfo.Init();
		bInited = TRUE;
	}

	GameRabotInfo.m_bCanQuitNow = FALSE;
	nRetCode = GameRabotInfo.SetPlayerInfo(account, role, region, server);
	KG_PROCESS_ERROR(nRetCode);

	// test test test test
	//nRetCode = GameRabotInfo.NotifyInfo(100, 101, L"C:/123", L"C:/123");
	//KG_PROCESS_ERROR(nRetCode);
	// end

	hThread = (HANDLE)_beginthreadex(NULL, 0, &GameRabotThread, NULL, 0, NULL);
	KG_PROCESS_ERROR(hThread);

	nResult = TRUE;
Exit0:	
	KG_HANDLE_CLOSE(hThread);
	
	return nResult;
}

unsigned WINAPI GameRabotThread(void*)
{
	int nResult  = false;
	int nRetCode = false;

	nRetCode = GameRabotInfo.RegCallback();
	KG_PROCESS_ERROR(nRetCode);

	for (int i = 0; i < 20; i++)
	{
		if (GameRabotInfo.m_bCanQuitNow)
		{
			break;
		}
		Sleep(1000);
	}

	nResult = true;
Exit0:
	_endthreadex(0);
	return nResult;
}
