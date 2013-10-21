///////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
//
// FileName : KDumpFile.cpp
// Creator  : chengzhaohui
// Date     : 
// Comment  :
//
///////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "KSG_MD5_String.h"
#include <DbgHelp.h>
#include <time.h>
#include "KPrivilege.h"
#include "KDumpFile.h"
#include "KFileVersionInfo.h"

#define UNKNOWN_MODULE_NAME _T("xxxx")
#define MAX_MODULE_COUNT 5
#define MAX_RECURSION_COUNT 1000

struct KDumpModuleInfo
{
	KDumpModuleInfo()
	{
		memset(this, 0, sizeof(KDumpModuleInfo));
	}

	int nFindDumpModuleFlag;
	TCHAR szModuleName[MAX_PATH];
	DWORD64 dwModuleBase;
	DWORD64 dwStackAddress;
};

typedef vector<KDumpModuleInfo> KDumpModuleInfoVector;
typedef KDumpModuleInfoVector::iterator KDumpModuleInfoVectorItor;

class KDumpModuleAnalyzer
{
public:
	int Init(HANDLE hExceptionThread, PEXCEPTION_POINTERS pExceptionInfo);
	int GetModuleInfo(KDumpModuleInfo *pRetDumpModuleInfo);
	int MoveStack();//返回false 既不用继续move
	void UnInit();

public:
	int static GetModules(HANDLE hExceptionThread, PEXCEPTION_POINTERS pExceptionInfo,  KDumpModuleInfoVector *pDumpModuleInfoVector);

private:
	//这两个函数可以用一个函数实现，但实现出来会很乱，KISS，拆开
	//第一步 GetFilteredModules 成功则返回，失败进行
	//第二步 获取第一个可以找到的模块 成功则返回，失败返回 未知模块UNKNOWN_MODULE_NAME 即 xxxx
	int static GetFilteredModules(HANDLE hExceptionThread, PEXCEPTION_POINTERS pExceptionInfo, KDumpModuleInfoVector *pDumpModuleInfoVector);
	int static GetFirstModule(HANDLE hExceptionThread, PEXCEPTION_POINTERS pExceptionInfo, KDumpModuleInfoVector *pDumpModuleInfoVector);

	//检查一个模块能否被过滤。true模块名在过滤名单中 false 不在
	int static ChackCanFilter(TCHAR szModuleName[]);

private:
	int m_nCurRecursionCount;
	HANDLE m_hProcess;
	HANDLE m_hThread;
	CONTEXT m_Context;
	STACKFRAME64 m_Stackframe; 
	DWORD64 m_dwStackAddress;

private:
	static BOOL WINAPI FindExceptionModule(PTSTR ModuleName,DWORD64 ModuleBase,ULONG ModuleSize,PVOID UserContext);
};

int KDumpModuleAnalyzer::Init(HANDLE hExceptionThread, PEXCEPTION_POINTERS pExceptionInfo)
{
	int nResult = false;
	int nRetCode = false;

	KG_PROCESS_ERROR(pExceptionInfo);

	m_hProcess = GetCurrentProcess();//The pseudohandle does not need to be closed when it is no longer needed. 
	m_hThread = hExceptionThread;//The pseudohandle does not need to be closed when it is no longer needed. 
	m_Context = *(pExceptionInfo->ContextRecord);

	memset(&m_Stackframe, 0, sizeof(m_Stackframe));
	m_Stackframe.AddrPC.Offset = m_Context.Eip;
	m_Stackframe.AddrPC.Mode = AddrModeFlat;
	m_Stackframe.AddrFrame.Offset = m_Context.Ebp;
	m_Stackframe.AddrFrame.Mode = AddrModeFlat;
	m_Stackframe.AddrStack.Offset = m_Context.Esp;
	m_Stackframe.AddrStack.Mode = AddrModeFlat;

	m_dwStackAddress = (DWORD64)pExceptionInfo->ExceptionRecord->ExceptionAddress;
	m_nCurRecursionCount = 0;
#ifdef _DEBUG
	nRetCode = SymInitialize(m_hProcess, NULL,TRUE);
#endif
	nResult = true;
Exit0:
	return nResult;
}

int KDumpModuleAnalyzer::GetModuleInfo(KDumpModuleInfo *pRetDumpModuleInfo)
{
	int nResult = false;
	int nRetCode = false;

#ifdef _DEBUG
	struct IMAGEHLP_SYMBOL64_EX : IMAGEHLP_SYMBOL64
	{
		TCHAR _EX[MAX_PATH];
	};

	DWORD64 dwDisplacement;
	IMAGEHLP_SYMBOL64_EX Symbol;
	Symbol.MaxNameLength = sizeof(Symbol._EX) / sizeof(Symbol._EX[0]);
	nRetCode = SymGetSymFromAddr64(m_hProcess, m_Stackframe.AddrPC.Offset, &dwDisplacement, &Symbol);
	if (nRetCode)
	{
#ifdef OPEN_LOG_AND_BREAK
		KLogPrintf("%s\t%d\t%x\n", Symbol.Name, 0, m_Stackframe.AddrPC.Offset);
#endif
		printf("%s\t%d\t%x\n", Symbol.Name, 0, m_Stackframe.AddrPC.Offset);
	}
#endif

	KG_PROCESS_ERROR(pRetDumpModuleInfo);
	memset(pRetDumpModuleInfo, 0, sizeof(KDumpModuleInfo));
	
	pRetDumpModuleInfo->dwStackAddress = m_dwStackAddress;
	nRetCode = EnumerateLoadedModules64(m_hProcess, KDumpModuleAnalyzer::FindExceptionModule, (void *)pRetDumpModuleInfo);
	KG_PROCESS_ERROR(nRetCode);//EnumerateLoadedModules64 执行失败
	KG_PROCESS_ERROR(pRetDumpModuleInfo->nFindDumpModuleFlag);//FindExceptionModule 没有查找到
	
#ifdef OPEN_LOG_AND_BREAK
	KLogPrintf(
		"Name:%s\tBase:%llp\tOffset:%llp\tStack:%llp", 
		pRetDumpModuleInfo->szModuleName, 
		pRetDumpModuleInfo->dwModuleBase, 
		pRetDumpModuleInfo->dwStackAddress,
		m_dwStackAddress
	);
#endif

	nResult = true;
Exit0:
	return nResult;
}

int KDumpModuleAnalyzer::MoveStack()
{
	int nResult = false;
	int nRetCode = false;

	nRetCode = StackWalk64(
		IMAGE_FILE_MACHINE_I386, 
		m_hProcess, 
		m_hThread, 
		&m_Stackframe, 
		&m_Context, 
		NULL,//ReadProcessMemory, 
		SymFunctionTableAccess64, 
		SymGetModuleBase64, 
		NULL
	);
	KG_PROCESS_ERROR(nRetCode);
	if (m_Stackframe.AddrPC.Offset != m_Stackframe.AddrReturn.Offset)
	{
		m_nCurRecursionCount = 0;
	}
	else
	{
		m_nCurRecursionCount++;
		KG_PROCESS_ERROR(m_nCurRecursionCount < MAX_RECURSION_COUNT);
	}


	m_dwStackAddress = m_Stackframe.AddrPC.Offset;
	nResult = true;
Exit0:
#ifdef OPEN_LOG_AND_BREAK
	KLogPrintf("MoveStack:%s", nResult ? "Success" : "Fail");
#endif
	return nResult;
}

void KDumpModuleAnalyzer::UnInit()
{
#ifdef _DEBUG
	SymCleanup(m_hProcess);
#endif
	memset(this, 0, sizeof(KDumpModuleAnalyzer));
}
BOOL WINAPI KDumpModuleAnalyzer::FindExceptionModule(
	PTSTR ModuleName,
	DWORD64 ModuleBase,
	ULONG ModuleSize,
	PVOID UserContext
)
{    
	BOOL bResult =false;
	BOOL bRetCode =false;
	KDumpModuleInfo *pDumpModuleInfo = NULL;

	KG_PROCESS_ERROR(ModuleName);
	KG_PROCESS_ERROR(UserContext);

	pDumpModuleInfo = (KDumpModuleInfo *)UserContext;

#ifdef OPEN_LOG_AND_BREAK
	KLogPrintf(
		"FindExceptionModule=>Name:%s\tBase:%p\tSize:%lld\tSearch:%llp", 
		ModuleName, 
		ModuleBase, 
		ModuleSize,
		pDumpModuleInfo->dwStackAddress
	);
#endif

	bRetCode = pDumpModuleInfo->dwStackAddress >= ModuleBase && pDumpModuleInfo->dwStackAddress <= ModuleBase + ModuleSize;
	KG_PROCESS_ERROR(bRetCode);

	pDumpModuleInfo->dwModuleBase = ModuleBase;
	TCHAR *pSlash = _tcsrchr(ModuleName, _T('\\'));
	if (!pSlash)
		pSlash = ModuleName;
	else
		pSlash++;

	_tcsncpy(pDumpModuleInfo->szModuleName, pSlash, sizeof(pDumpModuleInfo->szModuleName) / sizeof(pDumpModuleInfo->szModuleName[0]));
	pDumpModuleInfo->szModuleName[sizeof(pDumpModuleInfo->szModuleName) / sizeof(pDumpModuleInfo->szModuleName[0]) - 1] = _T('\0');
	_tcslwr_s(pDumpModuleInfo->szModuleName, sizeof(pDumpModuleInfo->szModuleName) / sizeof(pDumpModuleInfo->szModuleName[0]));

	pDumpModuleInfo->nFindDumpModuleFlag = true;
	bResult = TRUE;
Exit0:
	return !bResult;
}

int KDumpModuleAnalyzer::GetModules(HANDLE hExceptionThread, PEXCEPTION_POINTERS pExceptionInfo, KDumpModuleInfoVector *pDumpModuleInfoVector)
{
	int nResult = false;
	int nRetCode = false;
	KDumpModuleInfo DefaultModuleInfo;

	KG_PROCESS_ERROR(hExceptionThread);
	KG_PROCESS_ERROR(pExceptionInfo);
	KG_PROCESS_ERROR(pDumpModuleInfoVector);

	nRetCode = GetFilteredModules(hExceptionThread, pExceptionInfo, pDumpModuleInfoVector);
	KG_PROCESS_SUCCESS(nRetCode);

	nRetCode = GetFirstModule(hExceptionThread, pExceptionInfo, pDumpModuleInfoVector);
	KG_PROCESS_SUCCESS(nRetCode);

	DefaultModuleInfo.dwModuleBase = 0;
	DefaultModuleInfo.dwStackAddress = (DWORD64)pExceptionInfo->ExceptionRecord->ExceptionAddress;
	strncpy(DefaultModuleInfo.szModuleName, UNKNOWN_MODULE_NAME, sizeof(DefaultModuleInfo.szModuleName) / sizeof(DefaultModuleInfo.szModuleName[0]));
	DefaultModuleInfo.szModuleName[sizeof(DefaultModuleInfo.szModuleName)  / sizeof(DefaultModuleInfo.szModuleName[0]) - 1] = '\0';
	 
	pDumpModuleInfoVector->push_back(DefaultModuleInfo);

Exit1:
	nResult = true;
Exit0:
	return nResult;
}

int KDumpModuleAnalyzer::ChackCanFilter(TCHAR szModuleName[])
{
	ASSERT(szModuleName);

	int nRetCode = false;
	//需要过滤得dll列表
	const static TCHAR *szFilterModules[] = {
		_T("kernel32.dll"), _T("ntdll.dll"), _T("msvcrt.dll"), _T("dbghelp.dll"),  _T("user32.dll"), 
		//vc80
		_T("vcompd.dll"), _T("vcomp.dll"), _T("msvcm80.dll"), _T("msvcp80.dll"), _T("msvcr80.dll"), _T("mfc80.dll"), _T("mfc80u.dll"), _T("mfcm80.dll"), _T("mfcm80u.dll"),
		_T("atl80.dll"), _T("msvcm80d.dll"), _T("msvcp80d.dll"), _T("msvcr80d.dll"), _T("mfc80d.dll"), _T("mfc80ud.dll"), _T("mfcm80d.dll"), _T("mfcm80ud.dll"), _T("mfc80chs.dll"),
		_T("mfc80cht.dll"), _T("mfc80deu.dll"), _T("mfc80enu.dll"), _T("mfc80esp.dll"), _T("mfc80fra.dll"), _T("mfc80ita.dll"), _T("mfc80jpn.dll"), _T("mfc80kor.dll"),
		//dx
		_T("d3d9.dll"), _T("d3d9_40.dll")
	};
	const static int nFilterModuleCount = sizeof(szFilterModules) / sizeof(szFilterModules[0]);

	for (int i = 0; i < nFilterModuleCount; i++)
	{
		const TCHAR *pszFilterModule = szFilterModules[i];
		nRetCode = _tcsicmp(pszFilterModule, szModuleName);
		if (!nRetCode)
			return true;
	}
	return false;
}

int KDumpModuleAnalyzer::GetFilteredModules(HANDLE hExceptionThread, PEXCEPTION_POINTERS pExceptionInfo, KDumpModuleInfoVector *pDumpModuleInfoVector)
{
	int nResult = false;
	int nRetCode = false;
	int nFindModuleFlag = FALSE;
	KDumpModuleInfo ModuleInfo;
	KDumpModuleAnalyzer Analytics;
	int nAnalyticsInitFlag = false;

	KG_PROCESS_ERROR(hExceptionThread);
	KG_PROCESS_ERROR(pExceptionInfo);
	KG_PROCESS_ERROR(pDumpModuleInfoVector);

	nRetCode = Analytics.Init(hExceptionThread, pExceptionInfo);
	KG_PROCESS_ERROR(nRetCode);
	nAnalyticsInitFlag = true;

	//找到三个连续不被过滤得模块
	while (true)
	{
		//堆栈顶端是异常，其次是异常所在的函数，所以这儿先做一次移动
		nRetCode = Analytics.MoveStack();
		if (!nRetCode)
			break;

		nRetCode = Analytics.GetModuleInfo(&ModuleInfo);
		if (nRetCode)
		{
			nRetCode = ChackCanFilter(ModuleInfo.szModuleName);
			if (!nRetCode)
			{
				nFindModuleFlag = true;
				pDumpModuleInfoVector->push_back(ModuleInfo);
				if (pDumpModuleInfoVector->size() >= MAX_MODULE_COUNT)
					break;	
			}
			else
			{
				if (nFindModuleFlag)
					break;
			}
		}
		else
		{
			if (nFindModuleFlag)
				break;
		}			
	}

	KG_PROCESS_ERROR(nFindModuleFlag);

	nResult = true;
Exit0:
	if (nAnalyticsInitFlag)
	{
		Analytics.UnInit();
		nAnalyticsInitFlag = false;
	}
	return nResult;
}

int KDumpModuleAnalyzer::GetFirstModule(HANDLE hExceptionThread, PEXCEPTION_POINTERS pExceptionInfo, KDumpModuleInfoVector *pDumpModuleInfoVector)
{
	int nResult = false;
	int nRetCode = false;
	int nFindModuleFlag = FALSE;
	KDumpModuleInfo ModuleInfo;
	KDumpModuleAnalyzer Analytics;
	int nAnalyticsInitFlag = false;

	KG_PROCESS_ERROR(hExceptionThread);
	KG_PROCESS_ERROR(pExceptionInfo);
	KG_PROCESS_ERROR(pDumpModuleInfoVector);

	nRetCode = Analytics.Init(hExceptionThread, pExceptionInfo);
	KG_PROCESS_ERROR(nRetCode);
	nAnalyticsInitFlag = true;

	while (true)
	{		
		nRetCode = Analytics.GetModuleInfo(&ModuleInfo);
		if (nRetCode)
		{
			pDumpModuleInfoVector->push_back(ModuleInfo);
			nFindModuleFlag = true;
			break;
		}

		nRetCode = Analytics.MoveStack();
		if (!nRetCode)
			break;
	}

	KG_PROCESS_ERROR(nFindModuleFlag);

	nResult = true;
Exit0:
	if (nAnalyticsInitFlag)
	{
		Analytics.UnInit();
		nAnalyticsInitFlag = false;
	}
	return nResult;
}

KDumpFile::KDumpFile(HANDLE hExceptionThread, DWORD dwExceptionThreadId, _EXCEPTION_POINTERS *pException, MINIDUMP_CALLBACK_INFORMATION *pCallBack)
{
    m_szAppPath[0] = _T('\0');
    m_szAppName[0] = _T('\0');

	m_hExceptionThread = hExceptionThread;
    _tcsncpy(m_szExceptionModuleName, UNKNOWN_MODULE_NAME, sizeof(m_szExceptionModuleName) / sizeof(TCHAR));    
    m_szExceptionModuleName[sizeof(m_szExceptionModuleName) / sizeof(TCHAR) - 1] = _T('\0');

    m_pExceptionInfo = pException;
    m_pCallBack = pCallBack;
    m_nFullMemoryFlag = 0;
	m_dwExceptionThreadId = dwExceptionThreadId;

    m_hHelpDll = NULL;
    
    if (GetModuleFileName(NULL, m_szAppPath, MAX_PATH))
    {
        TCHAR *pSlash = _tcsrchr(m_szAppPath, _T('\\'));
        if (pSlash)
        {
            _tcscpy(m_szAppName, pSlash + 1);
            *(pSlash + 1) = _T('\0');
        }
    }

    InitMiniDumpType();
}

KDumpFile::~KDumpFile()
{
    if (m_hHelpDll)
    {
        FreeLibrary(m_hHelpDll);
        m_hHelpDll = NULL;
    }
}

int KDumpFile::InitMiniDumpType()
{
    int nResult = FALSE;
    int nValue  = 0;

    nValue = (int)GetPrivateProfileInt(
        _T("MiniDumpType"), 
        _T("EnableFullMemory"), 
        0, 
        MINIDUMP_CONFIG
    );   
    KG_PROCESS_ERROR(nValue == 0 || nValue == 1);
    m_nFullMemoryFlag = nValue;

    nResult = TRUE;
Exit0:
    return nResult;
}

int KDumpFile::MakeDump(const TCHAR szVersion[], const TCHAR szPath[])
{
    int nResult  = FALSE;
    int nRetCode = FALSE;

    ASSERT(szVersion && szPath);

    nRetCode = MakeDumpKey(szVersion);
    KG_PROCESS_ERROR(nRetCode);

    nRetCode = WriteMiniDump(szPath);
    KG_PROCESS_ERROR(nRetCode);

    nResult = TRUE;
Exit0:    
    return nResult;
}

int KDumpFile::WriteMiniDump(const TCHAR szPath[])
{
    int nResult  = FALSE;
    int nRetCode = FALSE;
    KPrivilege Privilege;
    HANDLE hFile = NULL;    
    _MINIDUMP_EXCEPTION_INFORMATION ExInfo;
	struct tm      today;
	time_t         tTime;

	ASSERT(szPath && szPath[0]);
	
    time(&tTime);
	localtime_s(&today, &tTime);

	_sntprintf(
		m_szDumpFileName,
		sizeof(m_szDumpFileName) / sizeof(TCHAR),
		_T("%s\\%s_%04d%02d%02d%02d%02d%02d_%s_%s.dmp"),
		szPath,
		m_szAppName,
		today.tm_year + 1900, 
		today.tm_mon + 1, 
		today.tm_mday,
        today.tm_hour,
        today.tm_min,
        today.tm_sec,
		m_szExceptionModuleName,
		m_szDumpKey
    );
	m_szDumpFileName[sizeof(m_szDumpFileName) / sizeof(TCHAR) - 1] = '\0';

    
	hFile = CreateFile(
        m_szDumpFileName, 
        GENERIC_WRITE, 
        FILE_SHARE_WRITE, 
        NULL, 
        CREATE_ALWAYS, 
        FILE_ATTRIBUTE_NORMAL, 
        NULL
    );
    
    KG_PROCESS_ERROR(hFile != INVALID_HANDLE_VALUE);    
	
	ExInfo.ThreadId          = m_dwExceptionThreadId;//GetThreadId(m_hExceptionThread);
	ExInfo.ExceptionPointers = m_pExceptionInfo;
	ExInfo.ClientPointers    = FALSE;

    Privilege.EnablePrivilege(SE_DEBUG_NAME);

    nRetCode = MiniDumpWriteDump(
        GetCurrentProcess(),
        GetCurrentProcessId(),
        hFile,
        (MINIDUMP_TYPE)(m_nFullMemoryFlag ? MiniDumpWithFullMemory : (MiniDumpWithDataSegs | MiniDumpWithIndirectlyReferencedMemory)),
        m_pExceptionInfo ? &ExInfo : NULL,
        NULL,
        m_pCallBack
	);
    KG_PROCESS_ERROR(nRetCode);

    nResult = TRUE;
Exit0:
    if (hFile != INVALID_HANDLE_VALUE)
    {
        CloseHandle(hFile);
        hFile = INVALID_HANDLE_VALUE;
    }
    Privilege.RestorePrivilege();
	return nResult;
}

int KDumpFile::MakeDumpKey(const TCHAR szVersion[])
{
	int nResult = FALSE;
	int nRetCode = FALSE;
    TCHAR szAppPathName[MAX_PATH];
	TCHAR szModuleInfos[MAX_PATH * MAX_MODULE_COUNT];	
    TCHAR szDumpKey[sizeof(szModuleInfos) / sizeof(szModuleInfos[0])  + 512];
    TCHAR szTmpKey[64];
	
	KFileVersionInfo FileVersion(szAppPathName);
	KDumpModuleInfoVector DumpModuleInfoVector;
	KDumpModuleInfoVectorItor itor;
	KDumpModuleInfoVectorItor itorEnd;
	KDumpModuleInfo *pModuleInfo = NULL;

    _tcscpy(szAppPathName, m_szAppPath);
    _tcscat(szAppPathName, m_szAppName);

	nRetCode = KDumpModuleAnalyzer::GetModules(m_hExceptionThread, m_pExceptionInfo, &DumpModuleInfoVector);
	KG_PROCESS_ERROR(nRetCode);
	KG_PROCESS_ERROR(DumpModuleInfoVector.size() > 0);

	_tcsncpy(m_szExceptionModuleName, DumpModuleInfoVector[0].szModuleName, sizeof(m_szExceptionModuleName) / sizeof(m_szExceptionModuleName[0]));
	m_szExceptionModuleName[sizeof(m_szExceptionModuleName) / sizeof(m_szExceptionModuleName[0]) - 1] = _T('\0');

	szModuleInfos[0] = _T('\0');
	for (
		itor = DumpModuleInfoVector.begin(), itorEnd = DumpModuleInfoVector.end();
		itor != itorEnd;
		itor++
	)
	{
		TCHAR szModuleName[MAX_PATH];
		nRetCode = _sntprintf(
			szModuleName,
			sizeof(szModuleName) / sizeof(szModuleName[0]),
			_T("%s%llp"), 
			itor->szModuleName,
			itor->dwStackAddress - itor->dwModuleBase
		);
		KG_PROCESS_ERROR(nRetCode >= 0 && nRetCode < sizeof(szModuleName) / sizeof(szModuleName[0]));
		szModuleName[sizeof(szModuleName) / sizeof(szModuleName[0]) - 1] = _T('\0');

		_tcscat(szModuleInfos, szModuleName);
		szModuleInfos[sizeof(szModuleInfos) / sizeof(szModuleInfos[0]) - 1] = _T('\0');
	}

    _sntprintf(szDumpKey, sizeof(szDumpKey) / sizeof(TCHAR),
        _T("%s%s%s%s%s%x"),
        szVersion,
        FileVersion.OriginalFilename(),
        FileVersion.ProductVersion(),
        FileVersion.FileVersion(),
		szModuleInfos,
        m_pExceptionInfo->ExceptionRecord->ExceptionCode
    );
    szDumpKey[sizeof(szDumpKey) / sizeof(TCHAR) - 1] = _T('\0');
    
    KG_EDStringToMD5String(szTmpKey, szDumpKey);
    _tcsncpy(m_szDumpKey, szTmpKey, sizeof(m_szDumpKey) / sizeof(TCHAR));
    m_szDumpKey[sizeof(m_szDumpKey) / sizeof(TCHAR) - 1] = _T('\0');

#ifdef OPEN_LOG_AND_BREAK
	KLogPrintf(
		"MD5:%s\tDumpKey:%s\tModuleName:%s", 
		szTmpKey, 
		szDumpKey, 
		m_szExceptionModuleName
	);
#endif

	nResult = TRUE;
Exit0:
    return nResult;
}
