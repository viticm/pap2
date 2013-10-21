#include "StdAfx.h"
#include <Psapi.h>
#include "KProcess.h"
#include "kglog.h"

KProcess::KProcess(void)
{
}

KProcess::~KProcess(void)
{
}

int KProcess::FindProcess(const TCHAR cszProcessPath[], int* nNum)
{
    int     nResult                 = FALSE;
    int     nRetCode                = FALSE;
    int     nProcessNumber          = 0;
    int     nFindFlag               = FALSE;

    DWORD   dwAllProcessID[1024]    = {0};
    DWORD   dwProcessesNeed         = 0;
    DWORD   dwModlesNeed            = 0;

    HANDLE  hProcess                = NULL;
    HMODULE hModules[1024]          = {0};

    TCHAR   szProcessPath[1024];
    szProcessPath[0] = _T('\0');

    KGLOG_PROCESS_ERROR(cszProcessPath);
	KGLOG_PROCESS_ERROR(nNum);

    nRetCode = EnumProcesses(dwAllProcessID, sizeof(dwAllProcessID), &dwProcessesNeed);
    KGLOG_PROCESS_ERROR(nRetCode);

    nProcessNumber = (int)(dwProcessesNeed / sizeof(DWORD));
    for (int i = 0; i < nProcessNumber; i++)
    {
        hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwAllProcessID[i]);
        if (!hProcess)
            continue; 

        nRetCode = EnumProcessModules(hProcess, hModules, sizeof(hModules), &dwModlesNeed);
        if (!nRetCode)
            continue;

        nRetCode = GetModuleFileNameEx(hProcess, hModules[0], szProcessPath, sizeof(szProcessPath) / sizeof(TCHAR));
        if (!nRetCode)
            continue;
        szProcessPath[sizeof(szProcessPath) / sizeof(TCHAR) - 1] = _T('\0');

        nRetCode = _tcsicmp(szProcessPath, cszProcessPath);
        if (nRetCode == 0)
        {
            nFindFlag = TRUE;
            //break;
			(*nNum)++;
        }
    }

    nResult = nFindFlag;
Exit0:
    if (hProcess)
    {
        CloseHandle(hProcess);
        hProcess = NULL;
    }
    return nResult;
}
