///////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
//
// FileName : KDumper.cpp
// Creator  : chengzhaohui
// Date     : 
// Comment  :
//
///////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "KDumpFile.h"
#include "KCrashReporter.h"
#include "KDumper.h"
#include "KProcess.h"
#include "KSG_MD5_String.h"
#include "toolhelp.h"
#include <io.h>
#include <time.h>
#include <intrin.h>
#include <algorithm>

#define BUFFER_SIZE 1024
#define NOT_CAPTURE_FILE_WRITE_TIME (60 * 100)//单位10毫秒--1分钟
#define EXCEPTION_NOT_CAPTURE_FILE_NAME _T("ExceptionNotCapture.dat")
#define EXCEPTION_NOT_CAPTURE_FILE_TIME_OVERTIME (60 * 30)//单位秒--30分钟
#define OS_DUMP_FILE_TIME_OVERTIME (60 * 5)//单位秒--5分钟

double FileTimeSub(LPFILETIME lpTime1, LPFILETIME lpTime2)
{
	__int64 *pnTime1 = (__int64 *)lpTime1;
	__int64 *pnTime2 = (__int64 *)lpTime2;
	double dResult = (double(*pnTime1 - *pnTime2)) / 10000000;
	return dResult;
}

KDumper KDumper::ms_Dumper;
KMutex KDumper::ms_Lock;

KDumper::KDumper(void)
{    
    ZeroMemory(&m_DumpBase, sizeof(DUMP_BASE));
    ZeroMemory(&m_DumpFile, sizeof(DUMP_FILE));
    ZeroMemory(&m_tmPriorLoginInfo, sizeof(PLAYER_LOGIN_REMARK_PACKAGE));
    ZeroMemory(&m_tmCurrentLoginInfo, sizeof(PLAYER_LOGIN_REMARK_PACKAGE));

    m_dwDumpRemark = EXCEPTION_NONE;
    m_bHaveLoginTime = false;
    m_bIsNotCapture = false;

    m_strServerIP = tstring(SERVER_IP);
    m_nServerPort = SERVER_PORT;

    m_strLogPath = tstring(LOG_PATH);

    LPTOP_LEVEL_EXCEPTION_FILTER pRet = ::SetUnhandledExceptionFilter(UnhandledExceptionHandler);
    m_bGameMonitorExitFlag = false;
	m_hGameMonitorThread = NULL;
    InitDumpServerIP();
}

KDumper::~KDumper(void)
{
}

int KDumper::InitDumpServerIP()
{
    int nResult = FALSE;
    int nRetCode = FALSE;
    TCHAR szRetString[IP_LENGTH];

    nRetCode = (int)GetPrivateProfileString(
        _T("Server"), 
        _T("ip"), 
        NULL,
        szRetString,
        sizeof(szRetString) / sizeof(TCHAR),
        MINIDUMP_CONFIG
    );   
    KG_PROCESS_ERROR(nRetCode > 0);
    KG_PROCESS_ERROR(szRetString[0]);
    szRetString[sizeof(szRetString) / sizeof(TCHAR) - 1] = _T('\0');
    
    m_strServerIP = tstring(szRetString);

    nResult = TRUE;
Exit0:
    return nResult;
}

int KDumper::SetProductVersion(const TCHAR szVersion[])
{
    if (szVersion && szVersion[0])
    {
        _tcsncpy(m_DumpBase.szProductVersion, szVersion, sizeof(m_DumpBase.szProductVersion) / sizeof(TCHAR));
        m_DumpBase.szProductVersion[sizeof(m_DumpBase.szProductVersion) / sizeof(TCHAR) - 1] = _T('\0');
        return TRUE;
    }
    return FALSE;
}

int KDumper::SetServerAddress(const TCHAR szIP[], int nPort)
{
    if (szIP && szIP[0])
    {
        _tcsncpy(m_DumpBase.szServerIP, szIP, sizeof(m_DumpBase.szServerIP) / sizeof(TCHAR));
        m_DumpBase.szServerIP[sizeof(m_DumpBase.szServerIP) / sizeof(TCHAR) - 1] = _T('\0');

        m_DumpBase.nServerPort = nPort;
        return TRUE;
    }
    return FALSE;
}

int KDumper::SetLogPath(const TCHAR szPath[], TCHAR szIdent[])
{
    int nResult = FALSE;
    int nRetCode = FALSE;
    int nLen = 0;
    TCHAR strPath[MAX_PATH];
    TCHAR strIdent[MAX_PATH];
    strPath[0] = _T('\0');
    strIdent[0] = _T('\0');

    m_strLogPath.clear();
    
    KG_PROCESS_ERROR(szPath && szPath[0]);
    KG_PROCESS_ERROR(szIdent && szIdent[0]);

    _tcsncpy(strPath, szPath, sizeof(strPath) / sizeof(TCHAR));
    strPath[sizeof(strPath) / sizeof(TCHAR) - 1] = _T('\0');
    
    _tcsncpy(strIdent, szIdent, sizeof(strIdent) / sizeof(TCHAR));
    strIdent[sizeof(strIdent) / sizeof(TCHAR) - 1] = _T('\0');

    nLen = (int)_tcslen(strPath);
    if (strPath[nLen - 1] == _T('\\') || strPath[nLen - 1] == _T('/'))
    {
        strPath[nLen - 1] = _T('\0');
    }

    nLen = (int)_tcslen(strIdent);
    if (strIdent[nLen - 1] == _T('\\') || strIdent[nLen - 1] == _T('/'))
    {
        strIdent[nLen - 1] = _T('\0');
    }

    m_strLogPath = tstring(strPath) + tstring(_T("\\")) + tstring(strIdent) + tstring(_T("\\"));
    
    nResult = TRUE;
Exit0:
    return nResult;
}

//-----------------------------------------------------------------------------
// Name: unhandledExceptionHandler()
// Desc: Call-back filter function for unhandled exceptions
//-----------------------------------------------------------------------------
LONG KDumper::UnhandledExceptionHandler(_EXCEPTION_POINTERS *pExceptionInfo)
{
#ifdef OPEN_LOG_AND_BREAK
	//DebugBreak();	
#endif
#ifdef _DEBUG
	printf("Press any key to continue!");
	getch();
#endif

	ms_Lock.Lock();

	int nRetCode = false; 
	HANDLE hProcessHandle = GetCurrentProcess();
	HANDLE hThreadHandle = GetCurrentThread();
	HANDLE hProcessExceptionThread = NULL;
	unsigned uThreadID = 0;
	ms_Dumper.m_hExceptionThread = NULL;
	

	nRetCode = DuplicateHandle(hProcessHandle, hThreadHandle, hProcessHandle, &ms_Dumper.m_hExceptionThread, 0, TRUE, DUPLICATE_SAME_ACCESS);
	KG_PROCESS_ERROR(nRetCode);

	ms_Dumper.m_dwExceptionThreadId = GetCurrentThreadId();

	hProcessExceptionThread = (HANDLE)_beginthreadex( NULL, 0, &ProcessExceptionThreadFunc, pExceptionInfo, 0, &uThreadID);
	KG_PROCESS_ERROR(hProcessExceptionThread);

	WaitForSingleObject(hProcessExceptionThread,  INFINITE);

Exit0:
	if (hProcessExceptionThread)
	{
		CloseHandle(hProcessExceptionThread);
		hProcessExceptionThread = NULL;
	}

	if (ms_Dumper.m_hExceptionThread)
	{
		CloseHandle(ms_Dumper.m_hExceptionThread);
		ms_Dumper.m_hExceptionThread = NULL;
	}
	ms_Lock.Unlock();

	TerminateProcess(GetCurrentProcess(), 0);
    
	return EXCEPTION_EXECUTE_HANDLER;
}

unsigned __stdcall KDumper::ProcessExceptionThreadFunc( void* pArguments)
{
	int nResult = FALSE;
	int nRetCode = FALSE;

	_EXCEPTION_POINTERS *pExceptionInfo = (_EXCEPTION_POINTERS *)pArguments;
	nRetCode = ms_Dumper.ProcessException(pExceptionInfo);
	KG_PROCESS_ERROR(nRetCode);

	nResult = TRUE;
Exit0:
	return !nResult;
}
int KDumper::ProcessException(_EXCEPTION_POINTERS *pExceptionInfo)
{
    int nRetCode = FALSE;
    int nResult  = EXCEPTION_EXECUTE_HANDLER;
    int nCreateProcessFlag = FALSE;
    DWORD dwRetCode = 0;
    DWORD dwExitCode = 0;      
    
    ZeroMemory(&m_DumpBase, sizeof(DUMP_BASE));
    ZeroMemory(&m_DumpFile, sizeof(DUMP_FILE));
    
    SetDumpRemark(EXCEPTION_ATTACH_BY_DUMP);
    nRetCode = SaveDumpRemark();
    //KG_PROCESS_ERROR(nRetCode);

    nRetCode = DeleteNotCaptureFile();
    //KG_PROCESS_ERROR(nRetCode);

    KCrashReporter CrashReporter(pExceptionInfo);
    
    MINIDUMP_CALLBACK_INFORMATION CallBack;
    CallBack.CallbackParam = &CrashReporter;
    CallBack.CallbackRoutine = KCrashReporter::DumpRptCallback;

    KDumpFile DumpFile(m_hExceptionThread, m_dwExceptionThreadId, pExceptionInfo, &CallBack);

    nRetCode = Prepare();
    KG_PROCESS_ERROR(nRetCode);
    
    nRetCode = DumpFile.MakeDump(m_DumpBase.szProductVersion, m_DumpBase.szDumpPath);
    KG_PROCESS_ERROR(nRetCode);

	_tcsncpy(m_DumpFile.szDumpFile, DumpFile.GetDumpFileName(), sizeof(m_DumpFile.szDumpFile) / sizeof(TCHAR));
    m_DumpFile.szDumpFile[sizeof(m_DumpFile.szDumpFile) / sizeof(TCHAR) - 1] = _T('\0');

    nRetCode = CrashReporter.MakeReport(m_DumpBase.szProductVersion, m_DumpFile.szEnvInfoFile);
    KG_PROCESS_ERROR(nRetCode);

    nRetCode = MakeLogFile(m_DumpBase.szDumpPath);
    //KG_PROCESS_ERROR(nRetCode);

    nRetCode = MakeConfigFile();
    //KG_PROCESS_ERROR(nRetCode);

    _tcsncpy(m_DumpBase.szDumpKey, DumpFile.GetDumpKey(), sizeof(m_DumpBase.szDumpKey) / sizeof(TCHAR));
    m_DumpBase.szDumpKey[sizeof(m_DumpBase.szDumpKey) / sizeof(TCHAR) - 1] = _T('\0');

    _tcsncpy(m_DumpBase.szDumpModule, DumpFile.GetExceptionModuleName(), sizeof(m_DumpBase.szDumpModule) / sizeof(TCHAR));
    m_DumpBase.szDumpModule[sizeof(m_DumpBase.szDumpModule) / sizeof(TCHAR) - 1] = _T('\0');

    nRetCode = PlayerInfoManager();
    //KG_PROCESS_ERROR(nRetCode);

    nRetCode = GetDumpSystemInfo();
    KG_PROCESS_ERROR(nRetCode);

    nRetCode = WriteDumpData(DUMP_OP_CAPTURE_BY_DUMP);
    KG_PROCESS_ERROR(nRetCode);

    nRetCode = SendReport();
    KG_PROCESS_ERROR(nRetCode);

    nResult = EXCEPTION_EXECUTE_HANDLER;
Exit0:
    return nResult;
}

int KDumper::SendReport()
{
    int nResult = FALSE;
    int nRetCode = FALSE;
    STARTUPINFO StartInfo = {sizeof(STARTUPINFO)};
    PROCESS_INFORMATION ProcessInfo;  
    tstring strDumpReporterName = m_strApplicationDirectory + DUMP_REPORTER;
    tstring strDumpParameterName = m_strApplicationDirectory + DUMP_PARAMETER_FILE;
    
    ZeroMemory(&ProcessInfo, sizeof(PROCESS_INFORMATION));
    
    try
    {
        GetStartupInfo(&StartInfo);
    }
    catch (...)
    {
#ifdef OPEN_LOG_AND_BREAK
        KLogPrintf("GetStartupInfo Error");
#endif	
        goto Exit0;
    }

    nRetCode = CreateProcess(
        strDumpReporterName.c_str(),//DumpReport.exe 程序所在目录
         (LPSTR)strDumpParameterName.c_str(),//DumpParameter.dat 程序所在目录
        NULL, 
        NULL,
        FALSE,
        NORMAL_PRIORITY_CLASS,
        NULL,
        m_strApplicationDirectory.c_str(),//NULL, //工作目录为程序所在目录
        &StartInfo, 
        &ProcessInfo
    );
#ifdef OPEN_LOG_AND_BREAK
    if (!nRetCode) 
        KLogPrintf("调用CreateProcess失败 %d",  GetLastError());
#endif	
    KG_PROCESS_ERROR(nRetCode);

    if (ProcessInfo.hThread)
    {
        CloseHandle(ProcessInfo.hThread);
        ProcessInfo.hThread = NULL;
    }
    if (ProcessInfo.hProcess)
    {
        CloseHandle(ProcessInfo.hProcess); 
        ProcessInfo.hProcess = NULL;
    }

     nResult = TRUE;
Exit0:
#ifdef OPEN_LOG_AND_BREAK
    KLogPrintf(
        "SendReport Result %d, %s %s 工作目录%s", 
        nResult,
        strDumpReporterName.c_str(),//DumpReport.exe 程序所在目录
        (LPSTR)strDumpParameterName.c_str(),//DumpParameter.dat 程序所在目录
        m_strApplicationDirectory.c_str()//NULL, //工作目录为程序所在目录
    );
#endif
    return nResult;
}

int KDumper::Prepare()
{  
    int nRetCode = FALSE;
    int nResult  = FALSE;    
    TCHAR szAppPath[MAX_PATH];

#ifdef OPEN_LOG_AND_BREAK
    {
        char sz[MAX_PATH];
        GetCurrentDirectory(sizeof(sz), sz);
        KLogPrintf("当前程序工作目录%s", sz);
    }
#endif	

    nRetCode = GetModuleFileName(NULL, szAppPath, MAX_PATH);
    KG_PROCESS_ERROR(nRetCode);

    _tcsncpy(m_DumpBase.szAppFullName, szAppPath, sizeof(m_DumpBase.szAppFullName) / sizeof(TCHAR) - 1);
    m_DumpBase.szAppFullName[sizeof(m_DumpBase.szAppFullName) / sizeof(TCHAR) - 1] = _T('\0');    
 
    //version.cfg 在exe同级目录
    //pak文件夹在工作目录
    nRetCode = GetProductVersion((m_strApplicationDirectory + tstring(PRODUCT_VERSION_FILE)).c_str());
    //KG_PROCESS_ERROR(nRetCode);----不需要

    nRetCode = _sntprintf(
        m_DumpBase.szDumpPath,
        sizeof(m_DumpBase.szDumpPath) / sizeof(TCHAR),
        _T("%sDumpFile"),
        m_strApplicationDirectory.c_str()
    );
    KG_PROCESS_ERROR(nRetCode > 0);
    m_DumpBase.szDumpPath[sizeof(m_DumpBase.szDumpPath) / sizeof(TCHAR) - 1] = _T('\0');
    
    nRetCode = _sntprintf(
        m_DumpFile.szEnvInfoFile,
        sizeof(m_DumpFile.szEnvInfoFile) / sizeof(TCHAR),
        _T("%s\\%s"),
        m_DumpBase.szDumpPath,
        ENV_INFO_FILE
    );
    KG_PROCESS_ERROR(nRetCode > 0);
    m_DumpFile.szEnvInfoFile[sizeof(m_DumpFile.szEnvInfoFile) / sizeof(TCHAR) - 1] = _T('\0');

    nRetCode = _sntprintf(
        m_DumpFile.szConfigFile,
        sizeof(m_DumpFile.szConfigFile) / sizeof(TCHAR),
        _T("%s\\%s"),
        m_DumpBase.szDumpPath,
        CONFIG_FILE
    );
    KG_PROCESS_ERROR(nRetCode > 0);
    m_DumpFile.szConfigFile[sizeof(m_DumpFile.szConfigFile) / sizeof(TCHAR) - 1] = _T('\0');
	
    Clearup();

    nRetCode = CreateDirectory(m_DumpBase.szDumpPath, NULL);
    //KG_PROCESS_ERROR(nRetCode);

    nResult = TRUE;
Exit0:    
    return nResult;
}

void KDumper::Clearup()
{
    DeleteFile(m_DumpFile.szDumpFile);
    DeleteFile(m_DumpFile.szEnvInfoFile);
    DeleteFile(m_DumpFile.szConfigFile);
    DeleteFile(m_DumpFile.szLogFile);
    RemoveDirectory(m_DumpBase.szDumpPath);
}

//jx3 only
int KDumper::GetProductVersion(const TCHAR cszVersionFile[])
{
    int nRetCode = FALSE;
    int nResult = FALSE;
    FILE *pVerFile = NULL;
    TCHAR szBuffer[256];
    TCHAR szVersionFile[MAX_PATH];
    TCHAR szVersionEX[VERSION_LENGTH / 2];
    TCHAR szVersionLineName[VERSION_LENGTH / 2];
    tstring strPAKPath;
    int nPAKPackageFlag = FALSE;
    szBuffer[0] = _T('\0');
    szVersionFile[0] = _T('\0');
    szVersionEX[0] = _T('\0');
    szVersionLineName[0] = _T('\0');

    ASSERT(cszVersionFile);

    KG_PROCESS_SUCCESS(m_DumpBase.szProductVersion[0]);//jx3 only

	_tcsncpy(m_DumpBase.szProductVersion, "0.0.0.0", sizeof(m_DumpBase.szProductVersion) / sizeof(TCHAR));
    m_DumpBase.szProductVersion[sizeof(m_DumpBase.szProductVersion) / sizeof(TCHAR) - 1] = _T('\0');

    _tcsncpy(szVersionFile, cszVersionFile, sizeof(szVersionFile) / sizeof(TCHAR));
    szVersionFile[sizeof(szVersionFile) / sizeof(TCHAR) - 1] = _T('\0');

    nRetCode = _taccess( _T("pak"), 0);//pak文件夹 在工作目录下
    if (nRetCode == 0)
        nPAKPackageFlag = TRUE;
    
    pVerFile = _tfopen(cszVersionFile, _T("r"));//版本文件在程序目录下
    KG_PROCESS_ERROR(pVerFile);

    while (TRUE)
    {
        size_t uLength = 0;
        TCHAR *pRet = _fgetts(szBuffer, sizeof(szBuffer) / sizeof(TCHAR), pVerFile);
        if (!pRet)
        {
            if (ferror(pVerFile))
                goto Exit0;
            else
                break;
        }
        szBuffer[sizeof(szBuffer) / sizeof(TCHAR) - 1] = _T('\0');
        if (szBuffer[0] == _T('#'))
            continue;

        uLength = _tcslen(PRODUCT_VERSION_KEY);
        nRetCode = _tcsncicmp(szBuffer, PRODUCT_VERSION_KEY, uLength);
        if (!nRetCode && szBuffer[uLength] == _T('='))
        {
            _tcsncpy(m_DumpBase.szProductVersion, szBuffer + uLength + 1, sizeof(m_DumpBase.szProductVersion) / sizeof(TCHAR));
            m_DumpBase.szProductVersion[sizeof(m_DumpBase.szProductVersion) / sizeof(TCHAR) - 1] = _T('\0');
			
			if (
				m_DumpBase.szProductVersion[_tcslen(m_DumpBase.szProductVersion) - 1] == _T('\n') || 
				m_DumpBase.szProductVersion[_tcslen(m_DumpBase.szProductVersion) - 1] == _T('\r')
			)
			{
				m_DumpBase.szProductVersion[_tcslen(m_DumpBase.szProductVersion) - 1] = _T('\0');
			}
            if (nPAKPackageFlag)
            {
                nRetCode = _sntprintf(
                    m_DumpBase.szProductVersion, 
                    sizeof(m_DumpBase.szProductVersion) / sizeof(TCHAR) - 1,
                    _T("%s_PAK"),
                    m_DumpBase.szProductVersion
                );
                KG_PROCESS_ERROR(nRetCode > 0);
                m_DumpBase.szProductVersion[sizeof(m_DumpBase.szProductVersion) / sizeof(TCHAR) - 1] = _T('\0');
            }
            else
            {
                nRetCode = _sntprintf(
                    m_DumpBase.szProductVersion, 
                    sizeof(m_DumpBase.szProductVersion) / sizeof(TCHAR) - 1,
                    _T("%s_SVN"),
                    m_DumpBase.szProductVersion
                );
                KG_PROCESS_ERROR(nRetCode > 0);
                m_DumpBase.szProductVersion[sizeof(m_DumpBase.szProductVersion) / sizeof(TCHAR) - 1] = _T('\0');
            }
            continue;
        }
        
        uLength = _tcslen(PRODUCT_VERSIONEX_KEY);
        nRetCode = _tcsncicmp(szBuffer, PRODUCT_VERSIONEX_KEY, uLength);
        if (!nRetCode && szBuffer[uLength] == _T('=') && (_tcslen(szBuffer) > uLength + 1))
        {
            //Sword3.versionex= 不为空情况
            if (
                (szBuffer[uLength + 1] != _T('\n')) &&
                (szBuffer[uLength + 1] != _T('\r'))
            )
            {
                _tcsncpy(szVersionEX, szBuffer + uLength + 1, sizeof(szVersionEX) / sizeof(TCHAR));
                szVersionEX[sizeof(szVersionEX) / sizeof(TCHAR) - 1] = _T('\0');
                if (
                    szVersionEX[_tcslen(szVersionEX) - 1] == _T('\n') || 
                    szVersionEX[_tcslen(szVersionEX) - 1] == _T('\r')
                    )
                {
                    szVersionEX[_tcslen(szVersionEX) - 1] = _T('\0');
                }
                
                tstring strVersionEx = tstring(szVersionEX);
                nRetCode = _sntprintf(
                    szVersionEX, sizeof(szVersionEX) / sizeof(TCHAR) - 1,
                    _T("-%s"),
                    strVersionEx.c_str()
                );
                KG_PROCESS_ERROR(nRetCode > 0);
            }
            continue;
        }

        uLength = _tcslen(PRODUCT_VERSIONLINENAME_KEY);
        nRetCode = _tcsncicmp(szBuffer, PRODUCT_VERSIONLINENAME_KEY, uLength);
        if (!nRetCode && szBuffer[uLength] == _T('=') && (_tcslen(szBuffer) > uLength + 1))
        {
            //Sword3.VersionLineName= 不为空情况
            if (
                (szBuffer[uLength + 1] != _T('\n')) &&
                (szBuffer[uLength + 1] != _T('\r'))
                )
            {
                _tcsncpy(szVersionLineName, szBuffer + uLength + 1, sizeof(szVersionLineName) / sizeof(TCHAR));
                szVersionLineName[sizeof(szVersionLineName) / sizeof(TCHAR) - 1] = _T('\0');
                if (
                    szVersionLineName[_tcslen(szVersionLineName) - 1] == _T('\n') || 
                    szVersionLineName[_tcslen(szVersionLineName) - 1] == _T('\r')
                    )
                {
                    szVersionLineName[_tcslen(szVersionLineName) - 1] = _T('\0');
                }
            }
        }
    }    

    KG_PROCESS_SUCCESS(!szVersionEX[0] && !szVersionLineName[0]);

    nRetCode = _sntprintf(
        m_DumpBase.szProductVersionEx, 
        sizeof(m_DumpBase.szProductVersionEx) / sizeof(TCHAR) - 1,
        _T("%s%s"),
        szVersionLineName,
        szVersionEX
    );
    KG_PROCESS_ERROR(nRetCode > 0);
    m_DumpBase.szProductVersionEx[sizeof(m_DumpBase.szProductVersionEx) / sizeof(TCHAR) - 1] = _T('\0');

Exit1:
    nResult = TRUE;
Exit0:    
    if (pVerFile)
    {
        fclose(pVerFile);
        pVerFile = NULL;
    }
    return nResult;
}

int KDumper::MakeLogFile(const TCHAR cszDumpPath[])
{
    int nResult = FALSE;
    int nRetCode = FALSE;
    TCHAR szCurrentPath[MAX_PATH];
    TCHAR *pSlash = NULL;
    TCHAR szLatestLogName[_MAX_FNAME];
    TCHAR szSearchPath[MAX_PATH];
    vector<tstring> vecLogFileList;
    int nRetLatestFileIndex = -1;

    KG_PROCESS_ERROR(cszDumpPath && cszDumpPath[0]);

    nRetCode = GetCurrentDirectory(sizeof(szCurrentPath) / sizeof(TCHAR) - 1, szCurrentPath);
    KG_PROCESS_ERROR(nRetCode);

    nRetCode = _sntprintf(
        szSearchPath,
        sizeof(szSearchPath) / sizeof(TCHAR),
        _T("%s\\%s"),
        szCurrentPath,
        m_strLogPath.c_str()
    );
    KG_PROCESS_ERROR(nRetCode > 0);
    szSearchPath[sizeof(szSearchPath) / sizeof(TCHAR) - 1] = _T('\0');

    nRetCode = FindAllFiles(szSearchPath, _T(".log"), vecLogFileList);//这个是在工作目录下
    KG_PROCESS_ERROR(nRetCode);

    nRetCode = GetLatestFile(vecLogFileList, nRetLatestFileIndex);
    KG_PROCESS_ERROR(nRetCode);
    KG_PROCESS_ERROR(nRetLatestFileIndex != -1);

    _tsplitpath(vecLogFileList[nRetLatestFileIndex].c_str(), NULL, NULL, szLatestLogName, NULL);

    nRetCode = _sntprintf(
        m_DumpFile.szLogFile,
        sizeof(m_DumpFile.szLogFile) / sizeof(TCHAR),
        _T("%s\\%s.log"),
        cszDumpPath,
        szLatestLogName        
    );
    KG_PROCESS_ERROR(nRetCode > 0);
    m_DumpFile.szLogFile[sizeof(m_DumpFile.szLogFile) / sizeof(TCHAR) - 1] = _T('\0');

    nRetCode = CopyLogFile(m_DumpFile.szLogFile, vecLogFileList[nRetLatestFileIndex].c_str());
    KG_PROCESS_ERROR(nRetCode);

    nResult = TRUE;
Exit0:
#ifdef OPEN_LOG_AND_BREAK
    KLogPrintf(
        "MakeLogFile Result %d, %s=>%s", 
        nResult,         
        nRetLatestFileIndex != -1 ? vecLogFileList[nRetLatestFileIndex].c_str() : "",
        m_DumpFile.szLogFile
    );
#endif
    return nResult;
}

int KDumper::MakeConfigFile()
{
    int nResult = FALSE;
    int nRetCode = FALSE;
    TCHAR *pSlash = NULL;
    tstring strConfigFile;

    KG_PROCESS_ERROR(m_DumpFile.szConfigFile[0]);  

    strConfigFile = m_strApplicationDirectory + CONFIG_FILE;//在程序所在目录

    nRetCode = CopyFile(strConfigFile.c_str(), m_DumpFile.szConfigFile, FALSE);
    KG_PROCESS_ERROR(nRetCode);

    nResult = TRUE;
Exit0:
#ifdef OPEN_LOG_AND_BREAK
    KLogPrintf(
        "MakeConfigFile Result %d, %s=>%s", 
        nResult, 
        strConfigFile.c_str(), 
        m_DumpFile.szConfigFile
        );
#endif
    return nResult;
}

int KDumper::CopyLogFile(const TCHAR cszDst[], const TCHAR cszSrc[])
{
    int nResult = FALSE;
    int nRetCode = FALSE;

    HANDLE hDstFile = NULL;
    HANDLE hSrcFile = NULL;
    DWORD dwFileSize = 0;
    DWORD dwReadSize = 0;
    TCHAR *pszBuffer = NULL;

    KG_PROCESS_ERROR(cszDst && cszDst[0]);
    KG_PROCESS_ERROR(cszSrc && cszSrc[0]);

    hSrcFile = CreateFile(
        cszSrc,
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_TEMPORARY,
        NULL        
    );
    KG_PROCESS_ERROR(hSrcFile);
    KG_PROCESS_ERROR(hSrcFile != INVALID_HANDLE_VALUE);

    dwFileSize = GetFileSize(hSrcFile, NULL);
    
    pszBuffer = (TCHAR *)malloc(dwFileSize * sizeof(TCHAR) * 2);
    KG_PROCESS_ERROR(pszBuffer);
    
    nRetCode = ReadFile(hSrcFile, pszBuffer, dwFileSize, &dwReadSize, NULL);
    KG_PROCESS_ERROR(nRetCode);

    hDstFile = CreateFile(
        cszDst,
        GENERIC_WRITE,
        FILE_SHARE_READ,
        NULL,
        OPEN_ALWAYS,
        FILE_ATTRIBUTE_TEMPORARY,
        NULL        
    );
    KG_PROCESS_ERROR(hDstFile);
    KG_PROCESS_ERROR(hDstFile != INVALID_HANDLE_VALUE);

    nRetCode = WriteFile(hDstFile, pszBuffer, dwFileSize, &dwReadSize, NULL);
    KG_PROCESS_ERROR(nRetCode);

    nResult = TRUE;
Exit0:
    if (hDstFile)
    {
        CloseHandle(hDstFile);
        hDstFile = NULL;
    }
    if (pszBuffer)
    {
        free(pszBuffer);
        pszBuffer = NULL;
    }
    if (hSrcFile)
    {
        CloseHandle(hSrcFile);
        hSrcFile = NULL;
    }
#ifdef OPEN_LOG_AND_BREAK
    KLogPrintf(
        "CopyLogFile Result %d, %s=>%s", 
        nResult, 
        cszSrc, 
        cszDst
    );
#endif
    return nResult;
}

int KDumper::PlayerInfoManager()
{
    int nResult = FALSE;
    int nRetCode = FALSE;
    int nFileIndex = -1;
    TCHAR szSearchPath[MAX_PATH];  
    vector<tstring> vecLoginFiles;

    nRetCode = _sntprintf(
        szSearchPath,
        sizeof(szSearchPath) / sizeof(TCHAR) - 1,
        _T("%s\\userdata"),
        m_strApplicationDirectory.c_str()
    );
    KG_PROCESS_ERROR(nRetCode > 0);
    szSearchPath[sizeof(szSearchPath) / sizeof(TCHAR) - 1] = _T('\0');

    nRetCode = FindAllFiles(szSearchPath, _T("login.ini"), vecLoginFiles);//在工作目录下
    KG_PROCESS_ERROR(nRetCode);
    
    KG_PROCESS_SUCCESS(vecLoginFiles.empty());

    nRetCode = GetLatestFile(vecLoginFiles, nFileIndex);
    KG_PROCESS_ERROR(nRetCode);

    _ASSERTE(nFileIndex != -1);
    KG_PROCESS_ERROR(nFileIndex != -1);

    nRetCode = GetPlayerInfo(vecLoginFiles[nFileIndex].c_str());
    KG_PROCESS_ERROR(nRetCode);


Exit1:
    nResult = TRUE;
Exit0:
#ifdef OPEN_LOG_AND_BREAK
    KLogPrintf(
        "PlayerInfoManager Result %d, %s", 
        nResult, 
        szSearchPath
    );
#endif
    return nResult;
}

int KDumper::FindAllFiles(const TCHAR szSearchPath[], TCHAR szKey[], vector<tstring>& vecRetFileList, int nIsOnlySearchCurrentDir)
{
    int nResult = FALSE;
    int nRetCode = FALSE;
    TCHAR szExecutePath[MAX_PATH];
    WIN32_FIND_DATA FileInfo;
    HANDLE hFile = NULL;
    DWORD dwLastError = 0;

    KG_PROCESS_ERROR(szSearchPath && szSearchPath[0]);
    KG_PROCESS_ERROR(szKey && szKey[0]);

    nRetCode = _sntprintf(
        szExecutePath,
        sizeof(szExecutePath) / sizeof(TCHAR) - 1,
        _T("%s\\*"),            
        szSearchPath
    );
    KG_PROCESS_ERROR(nRetCode > 0);
    szExecutePath[sizeof(szExecutePath) / sizeof(TCHAR) - 1] = _T('\0');

    hFile = FindFirstFile(szExecutePath, &FileInfo);
    KG_PROCESS_ERROR(hFile != INVALID_HANDLE_VALUE);

    do 
    {
        if ((FileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
        {
            if (
                _tcsicmp(FileInfo.cFileName, _T(".")) && 
                _tcsicmp(FileInfo.cFileName, _T("..")) &&
                !nIsOnlySearchCurrentDir
            )
            {
                TCHAR szPath[MAX_PATH];
                nRetCode = _sntprintf(
                    szPath,
                    sizeof(szPath) / sizeof(TCHAR) - 1,
                    _T("%s\\%s"),
                    szSearchPath,
                    FileInfo.cFileName
                );
                KG_PROCESS_ERROR(nRetCode > 0);
                szPath[sizeof(szPath) / sizeof(TCHAR) - 1] = _T('\0');
                    
                FindAllFiles(szPath, szKey, vecRetFileList);
            }
        }
        else
        {
            TCHAR *pFind = NULL;
            pFind = _tcsstr(FileInfo.cFileName, szKey);
            if (pFind)
            {
                tstring strPath = tstring(szSearchPath) + tstring(_T("\\"))+ tstring(FileInfo.cFileName);
                vecRetFileList.push_back(strPath);
            }
        }

        nRetCode = FindNextFile(hFile, &FileInfo);
        if (!nRetCode)
        {
            dwLastError = GetLastError();
            if (dwLastError == ERROR_NO_MORE_FILES)
                break;
            else
                KG_PROCESS_ERROR(false && _T("FindNextFile error!"));
        }

    } while (nRetCode);

    nResult = TRUE;
Exit0:
    if (hFile)
    {
        FindClose(hFile);
        hFile = NULL;
    }
    return nResult;
}

int KDumper::GetLatestFile(const vector<tstring>& vecFileList, int& nRetFindIndex)
{    
    int nResult = FALSE;
    int nRetCode = FALSE;
    int nSize = 0;
    int nFileIndex = -1;
    HANDLE hFile = NULL;
    FILETIME ftMaxTime = {0};
    FILETIME ftWrite;

    KG_PROCESS_ERROR(!vecFileList.empty());

    nSize = (int)vecFileList.size();
    for (int i = 0; i < nSize; ++i)
    {
        hFile = CreateFile(
            vecFileList[i].c_str(),
            GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_TEMPORARY,
            NULL 
        );
        KG_PROCESS_ERROR(hFile != INVALID_HANDLE_VALUE);  

        nRetCode = GetFileTime(hFile, NULL, NULL, &ftWrite);
        KG_PROCESS_ERROR(nRetCode);

        nRetCode = CompareFileTime(&ftMaxTime, &ftWrite);
        if (nRetCode == -1)
        {
            ftMaxTime = ftWrite;
            nFileIndex = i;
        }
        CloseHandle(hFile);
        hFile = NULL;
    }

    _ASSERTE(nFileIndex != -1);
    KG_PROCESS_ERROR(nFileIndex >= 0 && nFileIndex < nSize);
    nRetFindIndex = nFileIndex;

    nResult = TRUE;
Exit0:
    if (hFile)
    {
        CloseHandle(hFile);
        hFile = NULL;
    }
    return nResult;
}

//和ftTime最接近且差值在[subTimeMix， subTimeMax]范围内的
int KDumper::GetFileByFileTime(const vector<tstring>& vecFileList, FILETIME ftTime, int subTimeMix, int subTimeMax, int& nRetFindIndex)
{
    int nResult = FALSE;
    int nRetCode = FALSE;
    int nSize = 0;
    HANDLE hFile = INVALID_HANDLE_VALUE;
    FILETIME ftWrite;
	double dMixTimeSub = 0;
    int nFindFirstFlag = false;
	nRetFindIndex = -1;

    KG_PROCESS_ERROR(!vecFileList.empty());

    nSize = (int)vecFileList.size();
    for (int i = 0; i < nSize; ++i)
    {
		double dTimeSub = 0;
		nRetCode = GetFileLastModified(vecFileList[i].c_str(), &ftWrite);
		KG_PROCESS_ERROR(nRetCode);
		dTimeSub = FileTimeSub(&ftWrite, &ftTime);
		if (dTimeSub < subTimeMix || dTimeSub > subTimeMax)
			continue;	

		if (!nFindFirstFlag ||  dMixTimeSub > dTimeSub)
		{
			dMixTimeSub = dTimeSub;
			nRetFindIndex = i;		
		}
	}

    KG_PROCESS_ERROR(nRetFindIndex >= 0 && nRetFindIndex < nSize);

	nResult = TRUE;
Exit0:
    return nResult;
}

int KDumper::GetPlayerInfo(const TCHAR szLoginPath[])
{
    int nResult = FALSE;
    int nRetCode = FALSE;
    const TCHAR *pFind = NULL;
    const TCHAR *pSlash = NULL;
    const TCHAR *pNextSlash = NULL;
    TCHAR szValue[64] = {0};
    TCHAR szFileName[MAX_PATH] = {0};

    KG_PROCESS_ERROR(szLoginPath);

    pSlash = _tcsrchr(szLoginPath, _T('\\'));
    KG_PROCESS_ERROR(pSlash);

    _tcsncpy(szFileName, pSlash + 1, sizeof(szFileName) / sizeof(TCHAR) - 1);   
    nRetCode = _tcsicmp(szFileName, _T("login.ini"));
    KG_PROCESS_ERROR(nRetCode == 0);

    pFind = _tcsstr(szLoginPath, _T("userdata\\"));
    KG_PROCESS_ERROR(pFind);

    pSlash = _tcschr(pFind, _T('\\'));
    KG_PROCESS_ERROR(pSlash);

    pNextSlash = (++pSlash);
    while (*pNextSlash != _T('\\') && *pNextSlash != _T('\0'))
    {
        pNextSlash++;
    }

    KG_PROCESS_ERROR((pNextSlash - pSlash) < (sizeof(m_DumpBase.szAccount) / sizeof(TCHAR)));    
    _tcsncpy(m_DumpBase.szAccount, pSlash, pNextSlash - pSlash);
    
    nRetCode = GetPrivateProfileString(_T("LastLogin"), _T("RoleName"), NULL, szValue, sizeof(szValue) / sizeof(TCHAR), szLoginPath);
    KG_PROCESS_ERROR(nRetCode > 0);
    szValue[sizeof(szValue) / sizeof(TCHAR) - 1] = _T('\0');
    _tcsncpy(m_DumpBase.szRoleName, szValue, sizeof(m_DumpBase.szRoleName) / sizeof(TCHAR) - 1);

    nRetCode = GetPrivateProfileString(_T("LastLogin"), _T("RegionName"), NULL, szValue, sizeof(szValue) / sizeof(TCHAR), szLoginPath);
    KG_PROCESS_ERROR(nRetCode > 0);
    szValue[sizeof(szValue) / sizeof(TCHAR) - 1] = _T('\0');
    _tcsncpy(m_DumpBase.szRegionName, szValue, sizeof(m_DumpBase.szRegionName) / sizeof(TCHAR) - 1);

    nRetCode = GetPrivateProfileString(_T("LastLogin"), _T("ServerName"), NULL, szValue, sizeof(szValue) / sizeof(TCHAR), szLoginPath);
    KG_PROCESS_ERROR(nRetCode > 0);
    szValue[sizeof(szValue) / sizeof(TCHAR) - 1] = _T('\0');
    _tcsncpy(m_DumpBase.szServerName, szValue, sizeof(m_DumpBase.szServerName) / sizeof(TCHAR) - 1);

    nResult = TRUE;
Exit0:
    return nResult;
}

int KDumper::WriteDumpData(DWORD dwDumpOP)
{
    int nResult = FALSE;
    int nRetCode = FALSE;
    FILE *pFile = NULL;
    DUMP_MESSAGE_HEAD DumpHead;
    TCHAR szGUID[64] = {0};
    tstring strDumpParameterName = m_strApplicationDirectory + DUMP_PARAMETER_FILE;
    
    nRetCode = GetGUID(szGUID);
    KG_PROCESS_ERROR(nRetCode && szGUID[0]);    

    pFile = _tfopen(strDumpParameterName.c_str(), _T("wb+"));//DumpParameter.dat在程序目录下
    KG_PROCESS_ERROR(pFile);

    ZeroMemory(&DumpHead, sizeof(DUMP_MESSAGE_HEAD));

    //DUMP_MESSAGE_HEAD
    DumpHead.dwDumpOP = dwDumpOP;
    DumpHead.dwBufferSize = sizeof(DUMP_MESSAGE_HEAD) + sizeof(DUMP_BASE) + sizeof(DUMP_FILE);

    _tcsncpy(m_DumpBase.szServerIP, m_strServerIP.c_str(), sizeof(m_DumpBase.szServerIP) / sizeof(TCHAR) - 1);
    m_DumpBase.nServerPort = m_nServerPort;
    _tcsncpy(m_DumpBase.szGUID, szGUID, sizeof(m_DumpBase.szGUID) / sizeof(TCHAR) - 1);
    _tcsncpy(m_DumpFile.szGUID, szGUID, sizeof(m_DumpFile.szGUID) / sizeof(TCHAR) - 1);

    nRetCode = (int)fwrite(&DumpHead, sizeof(DUMP_MESSAGE_HEAD), 1, pFile);
    KG_PROCESS_ERROR(nRetCode == 1);

    nRetCode = (int)fwrite(&m_DumpBase, sizeof(DUMP_BASE), 1, pFile);
    KG_PROCESS_ERROR(nRetCode == 1);

    nRetCode = (int)fwrite(&m_DumpFile, sizeof(DUMP_FILE), 1, pFile);
    KG_PROCESS_ERROR(nRetCode == 1);

    nResult = TRUE;
Exit0:
    if (pFile)
    {
        fclose(pFile);
        pFile = NULL;
    }
#ifdef OPEN_LOG_AND_BREAK
    KLogPrintf(
        "WriteDumpData Result %d, %s", 
        nResult, 
        strDumpParameterName.c_str()
    );
#endif
    return nResult;
}

int KDumper::GetDumpSystemInfo()
{
    int nResult = FALSE;
    int nRetCode = FALSE;
    
    nRetCode = GetVideoCard();
    KG_PROCESS_ERROR(nRetCode);

    nRetCode = GetPhysMem();
    KG_PROCESS_ERROR(nRetCode);
    
    nRetCode = GetCPUInfo();
    KG_PROCESS_ERROR(nRetCode);

    nResult = TRUE;
Exit0:
    return nResult;
}

void KDumper::SetApplicationDirectory()
{   
    int nResult = FALSE;
    int nRetCode = FALSE;
    TCHAR szAppPath[MAX_PATH];
    TCHAR *pSlash = NULL;

    nRetCode = GetModuleFileName(NULL, szAppPath, sizeof(szAppPath) / sizeof(szAppPath[0]));
    KG_PROCESS_ERROR(nRetCode);

    pSlash = _tcsrchr(szAppPath, _T('\\'));
    KG_PROCESS_ERROR(pSlash);    
    *(pSlash + 1) = _T('\0');

    m_strApplicationDirectory = szAppPath;
    nResult = TRUE;
Exit0:
    return;
}

//not capture by dump
int KDumper::OnProcessAttach()
{
    int nRetCode = FALSE;
    int nResult = FALSE;  
    int nProcessNum = 0;
    DWORD dwDumpOp = DUMP_OP_NONE;    

    ZeroMemory(&m_DumpBase, sizeof(DUMP_BASE));
    ZeroMemory(&m_DumpFile, sizeof(DUMP_FILE));    

    nRetCode = IsSingleProcess();
    if (!nRetCode)
    {
        //不是第一个进程，不做后面的判断操作
        goto Exit1;
    }

    SetApplicationDirectory();

    nRetCode = LoadDumpRemark();
    KG_PROCESS_ERROR(nRetCode);

    //传入的是程序所在目录
    nRetCode = ProcessNotCaptureException(m_strApplicationDirectory.c_str());
    KG_PROCESS_ERROR(nRetCode);

    nRetCode = IsPlayerLoginToday();
    if (!nRetCode)
    {
        //没有登录
        dwDumpOp = (dwDumpOp | DUMP_OP_PLAYER_LOGIN);
        m_tmCurrentLoginInfo.dwPlayerLoginRemark = PLAYER_LOGIN;
    }

    //系统捕获的异常
	//if ((m_dwDumpRemark & EXCEPTION_ATTACH_BY_OS) || m_bIsNotCapture)
	if (m_bIsNotCapture)
    {
        dwDumpOp = (dwDumpOp | DUMP_OP_NOT_CAPTURE_BY_DUMP);
    }
    
#ifdef OPEN_LOG_AND_BREAK
    KLogPrintf("OnProcessAttach dwDumpOp=%d", dwDumpOp);
#endif	
    KG_PROCESS_SUCCESS(dwDumpOp == DUMP_OP_NONE);  

    nRetCode = Prepare();
    KG_PROCESS_ERROR(nRetCode);

    nRetCode = GetDumpSystemInfo();
    KG_PROCESS_ERROR(nRetCode);

    if (dwDumpOp & DUMP_OP_NOT_CAPTURE_BY_DUMP)
    {
        nRetCode = PlayerInfoManager();
        //KG_PROCESS_ERROR(nRetCode);

        _tcsncpy(m_DumpBase.szDumpModule, _T("EXCEPTION_NOT_CAPTURE"), sizeof(m_DumpBase.szDumpModule) / sizeof(TCHAR) - 1);

        nRetCode = MakeNotCaptureDumpKey();
        KG_PROCESS_ERROR(nRetCode);

		nRetCode = CopyToDumpFile(m_strOSDumpPath, m_DumpFile.szDumpFile, sizeof(m_DumpFile.szDumpFile) / sizeof(m_DumpFile.szDumpFile[0]));
		//KG_PROCESS_ERROR(nRetCode);

		nRetCode = CopyToDumpFile(m_strLatestLogName, m_DumpFile.szLogFile, sizeof(m_DumpFile.szLogFile) / sizeof(m_DumpFile.szLogFile[0]));  
		//KG_PROCESS_ERROR(nRetCode);
    }

    nRetCode = WriteDumpData(dwDumpOp);
    KG_PROCESS_ERROR(nRetCode);

    nRetCode = SendReport();
    KG_PROCESS_ERROR(nRetCode);

Exit1:
    nResult = TRUE;
Exit0:
    SetDumpRemark(EXCEPTION_ATTACH_BY_OS);
    SaveDumpRemark();
    CreateNotCaptureFile();
    return nResult;
}

int KDumper::OnProcessDetach()
{
    int nRetCode = FALSE;

    SetDumpRemark(EXCEPTION_ATTACH_BY_DUMP);    

    nRetCode = SaveDumpRemark();
    KG_PROCESS_ERROR(nRetCode);

    nRetCode = DeleteNotCaptureFile();
    KG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

int KDumper::CreateNotCaptureFile()
{
    int nResult = FALSE;
    int nRetCode = FALSE;
    DWORD dwProcessID = 0;
    HANDLE hFile = INVALID_HANDLE_VALUE;
    TCHAR szFileName[MAX_PATH];
	unsigned uThreadID;
    tstring strNotCaptureName; 

	m_hGameMonitorThread = NULL;
    dwProcessID = GetCurrentProcessId();   
    
    nRetCode = _snprintf(
        szFileName,
        sizeof(szFileName) / sizeof(TCHAR),
        _T("%d_%s"),
        dwProcessID,
		EXCEPTION_NOT_CAPTURE_FILE_NAME
    );
    KG_PROCESS_ERROR(nRetCode > 0);
    szFileName[sizeof(szFileName) / sizeof(TCHAR) - 1] = _T('\0');

    //程序目录下
    strNotCaptureName = m_strApplicationDirectory + szFileName;

    hFile = CreateFile(strNotCaptureName.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    KG_PROCESS_ERROR(hFile != INVALID_HANDLE_VALUE);

    m_strNotCaptureFile = tstring(szFileName);
	m_bGameMonitorExitFlag = false;
	m_hNotCaptureFile = hFile;
	m_hGameMonitorThread = (HANDLE)_beginthreadex( NULL, 0, &GameMonitorThreadFunc, this, 0, &uThreadID);
	KG_PROCESS_ERROR(m_hGameMonitorThread);

#ifdef OPEN_LOG_AND_BREAK
	KLogPrintf("线程开启成功\t%d", uThreadID);
#endif	

    nResult = TRUE;
Exit0:
	if (!nResult)
	{	
		if (m_hGameMonitorThread)
		{
			m_bGameMonitorExitFlag = true;
			TerminateThread(m_hGameMonitorThread, 0);
			CloseHandle(m_hGameMonitorThread);
			m_hGameMonitorThread = NULL;

#ifdef OPEN_LOG_AND_BREAK
			KLogPrintf("CreateNotCaptureFile执行失败，线程关闭成功");
#endif	
		}

		if (hFile != INVALID_HANDLE_VALUE)
		{
			CloseHandle(hFile);
			hFile = INVALID_HANDLE_VALUE;
			m_hNotCaptureFile = INVALID_HANDLE_VALUE;
		}	
	}    
#ifdef OPEN_LOG_AND_BREAK
    KLogPrintf("CreateNotCaptureFile Result %d %s", nResult, strNotCaptureName.c_str());
#endif	
    return nResult;
}

int KDumper::DeleteNotCaptureFile()
{
    int nResult = FALSE;
    int nRetCode = FALSE;

	if (m_hGameMonitorThread)
	{
		m_bGameMonitorExitFlag = true;
		TerminateThread(m_hGameMonitorThread, 0);
#ifdef OPEN_LOG_AND_BREAK
		KLogPrintf("线程关闭成功");
#endif	
	}

	if (m_hNotCaptureFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hNotCaptureFile);
		m_hNotCaptureFile = INVALID_HANDLE_VALUE;
	}	

    KG_PROCESS_ERROR(!m_strNotCaptureFile.empty());

    nRetCode = DeleteFile(m_strNotCaptureFile.c_str());
    KG_ASSERT_EXIT(nRetCode);

    nResult = TRUE;
Exit0:
    return nResult;
}

unsigned __stdcall KDumper::GameMonitorThreadFunc( void* pArguments)
{
	int nResult = FALSE;
	int nRetCode = FALSE;
	DWORD64 nCount = 0;
	TCHAR szBuffer[BUFFER_SIZE];
	DWORD nNumberOfBytesToWrite;
	DWORD nNumberOfBytesWritten;

	KDumper *pDumper = (KDumper *)pArguments;

	while (!pDumper->m_bGameMonitorExitFlag)
	{
		Sleep(10);

		nCount++;
		if (nCount % NOT_CAPTURE_FILE_WRITE_TIME)
			continue;	

		KG_PROCESS_ERROR(pDumper->m_hNotCaptureFile != INVALID_HANDLE_VALUE);

     	nRetCode = _snprintf(
			szBuffer,
			sizeof(szBuffer) / sizeof(szBuffer[0]),
			_T("%lld\n"),
			 time(0)
		);
		KG_PROCESS_ERROR(nRetCode >= 0 && nRetCode < sizeof(szBuffer) / sizeof(szBuffer[0]));
		szBuffer[sizeof(szBuffer) / sizeof(szBuffer[0])- 1] = _T('\0');
		nNumberOfBytesToWrite = (DWORD)strlen(szBuffer);

		nRetCode = WriteFile(pDumper->m_hNotCaptureFile, szBuffer, nNumberOfBytesToWrite, &nNumberOfBytesWritten, NULL);
		KG_PROCESS_ERROR(nRetCode);	

		nRetCode = FlushFileBuffers(pDumper->m_hNotCaptureFile);
		KG_PROCESS_ERROR(nRetCode);	
	}

	nResult = TRUE;
Exit0:
	return !nResult;
} 


int KDumper::IsPlayerLoginToday()
{
    SYSTEMTIME tmCurrentTime = {0};

    GetSystemTime(&tmCurrentTime);
    
    m_tmCurrentLoginInfo.wYear = tmCurrentTime.wYear;
    m_tmCurrentLoginInfo.wMonth = tmCurrentTime.wMonth;
    m_tmCurrentLoginInfo.wDay = tmCurrentTime.wDay;

    //当天已经登录了
    if (
        m_bHaveLoginTime &&
        (m_tmCurrentLoginInfo.wYear == m_tmPriorLoginInfo.wYear) &&
        (m_tmCurrentLoginInfo.wMonth == m_tmPriorLoginInfo.wMonth) &&
        (m_tmCurrentLoginInfo.wDay == m_tmPriorLoginInfo.wDay) &&
        (m_tmPriorLoginInfo.dwPlayerLoginRemark & PLAYER_LOGIN)
    )
    {
        m_tmCurrentLoginInfo.dwPlayerLoginRemark = PLAYER_LOGIN;
        return TRUE;
    }

    return FALSE;
}

int KDumper::SaveDumpRemark()
{
    int nResult = FALSE;
    int nRetCode = FALSE;
    FILE *pFile = NULL;
    EXCEPTION_REMARK_PACKAGE RemarkPackage;
    tstring strDumpRemarkName = m_strApplicationDirectory + DUMP_REMARK_FILE;
    
    pFile = _tfopen(strDumpRemarkName.c_str(), _T("wb+"));//DumpRemark.dump 在程序目录下
    KG_PROCESS_ERROR(pFile);

    RemarkPackage.dwFileRemark = FILE_BEGIN_MASK;
    RemarkPackage.dwDumpRemark = m_dwDumpRemark;

    nRetCode = (int)fwrite(&RemarkPackage, sizeof(EXCEPTION_REMARK_PACKAGE), 1, pFile);
    KG_PROCESS_ERROR(nRetCode == 1);

    nRetCode = (int)fwrite(&m_tmCurrentLoginInfo, sizeof(PLAYER_LOGIN_REMARK_PACKAGE), 1, pFile);
    KG_PROCESS_ERROR(nRetCode == 1);

    nResult = TRUE;
Exit0:
    if (pFile)
    {
        fclose(pFile);
        pFile = NULL;
    }
    return nResult;
}

int  KDumper::GetFileLastModified(const TCHAR szFileName[], LPFILETIME lpTime)
{
	int nResult = FALSE;
	int nRetCode = FALSE;
	HANDLE hFile = INVALID_HANDLE_VALUE;

	KG_PROCESS_ERROR(lpTime);

	hFile = CreateFile(
		szFileName,
		GENERIC_READ,
		NULL,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);
	KG_PROCESS_ERROR(hFile != INVALID_HANDLE_VALUE);  

	nRetCode = GetFileTime(hFile, NULL, NULL, lpTime);
	KG_PROCESS_ERROR(nRetCode);

	nResult = TRUE;
Exit0:
	if (hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hFile);
		hFile = INVALID_HANDLE_VALUE;
	}
	return nResult;
}

//传入的是程序所在目录
int KDumper::ProcessNotCaptureException(const TCHAR szCurrentDir[])
{
	int nResult = FALSE;
	int nRetCode = FALSE;
	vector<tstring> vecFileList;
	FILETIME CurrentTime;	 
	double dTimeSubtract = 0;
	FILETIME LastModifiedTime;

	KG_PROCESS_ERROR(szCurrentDir);

	nRetCode = FindAllFiles(szCurrentDir, EXCEPTION_NOT_CAPTURE_FILE_NAME, vecFileList, true);
	KG_PROCESS_ERROR(nRetCode);

	m_bIsNotCapture = vecFileList.empty() ? false : true;

	KG_PROCESS_SUCCESS(!m_bIsNotCapture);

#ifdef OPEN_LOG_AND_BREAK
    KLogPrintf("查找到EXCEPTION_NOT_CAPTURE_FILE_NAME文件");
#endif	

	for (int i = 0; i < (int)vecFileList.size(); ++i)
	{
		FILETIME ModifiedTime;
		const TCHAR *pszFileName = vecFileList[i].c_str();

		nRetCode = GetFileLastModified(pszFileName, &ModifiedTime);
		KG_ASSERT_EXIT(nRetCode);

		if (i == 0)
		{
			LastModifiedTime = ModifiedTime;
		}
		else
		{
			nRetCode = CompareFileTime(&LastModifiedTime, &ModifiedTime);
			if (nRetCode > 0)
				LastModifiedTime = ModifiedTime;
		}			

		nRetCode = DeleteFile(pszFileName);            
		KG_ASSERT_EXIT(nRetCode);
	}

	//查看有没有系统dump
    //如果存在系统异常，则反馈
    //否则看宕机时间是否在规定范围内
	m_bIsNotCapture = CheckOSDumpFile(LastModifiedTime) ? true : false;
#ifdef OPEN_LOG_AND_BREAK
    KLogPrintf("查找minidump:%d\t%s", m_bIsNotCapture, m_strOSDumpPath.c_str());
#endif	
	if(!m_bIsNotCapture)
	{
		GetSystemTimeAsFileTime(&CurrentTime);	
		dTimeSubtract = FileTimeSub(&CurrentTime, &LastModifiedTime);
		m_bIsNotCapture = dTimeSubtract < EXCEPTION_NOT_CAPTURE_FILE_TIME_OVERTIME;

#ifdef OPEN_LOG_AND_BREAK
        KLogPrintf("检查异常时间是否过期：%d", m_bIsNotCapture);
#endif	
	}

	//如果成功的话 继续去找系统log
	if (m_bIsNotCapture)
	{
		CheckJX3LogForOSDump(LastModifiedTime);
#ifdef OPEN_LOG_AND_BREAK
        KLogPrintf("获取对应的Log目录:%s", m_strLogPath.c_str());
        KLogPrintf("获取对应的Log文件:%s", m_strLatestLogName.c_str());
#endif	
	}
Exit1:
	nResult = TRUE;
Exit0:
	return nResult;
}

int KDumper::LoadDumpRemark()
{
    int nResult = FALSE;
    int nRetCode = FALSE;
    long lFileLength = 0;
    FILE *pFile = NULL;
    EXCEPTION_REMARK_PACKAGE RemarkPackage;    
    tstring strDumpRemarkName = m_strApplicationDirectory + DUMP_REMARK_FILE;

    nRetCode = _access(strDumpRemarkName.c_str(), 0);//在程序所在目录
    KG_PROCESS_SUCCESS(nRetCode == -1);

    pFile = _tfopen(strDumpRemarkName.c_str(), _T("rb"));//在程序所在目录
    KG_PROCESS_ERROR(pFile);

    nRetCode = fseek(pFile, 0, SEEK_END);
    KG_PROCESS_ERROR(!nRetCode);

    lFileLength = ftell(pFile);
    KG_PROCESS_SUCCESS(lFileLength == 0);

    nRetCode = fseek(pFile, 0, SEEK_SET);
    KG_PROCESS_ERROR(!nRetCode);

    nRetCode = (int)fread(&RemarkPackage, sizeof(EXCEPTION_REMARK_PACKAGE), 1, pFile);
    KG_PROCESS_ERROR(nRetCode == 1);

    if (RemarkPackage.dwFileRemark & FILE_BEGIN_MASK)
    {
        m_dwDumpRemark = RemarkPackage.dwDumpRemark;
    }

    //新文件
    if (lFileLength > sizeof(EXCEPTION_REMARK_PACKAGE))
    {
        ZeroMemory(&m_tmPriorLoginInfo, sizeof(PLAYER_LOGIN_REMARK_PACKAGE));

        nRetCode = (int)fread(&m_tmPriorLoginInfo, sizeof(PLAYER_LOGIN_REMARK_PACKAGE), 1, pFile);
        KG_PROCESS_ERROR(nRetCode == 1);

        m_bHaveLoginTime = true;
    }    

Exit1:
    nResult = TRUE;
Exit0:
    if (pFile)
    {
        fclose(pFile);
        pFile = NULL;
    }
    return nResult;
}

int KDumper::GetVideoCard()
{
    int nRetCode = FALSE;
    TCHAR szBuffer[64];
    LPDIRECT3D9 pD3D = NULL;
    LPDIRECT3DDEVICE9 pD3DDevice = NULL;

    if(NULL == (pD3D = Direct3DCreate9(D3D_SDK_VERSION)))
        return FALSE;

    D3DADAPTER_IDENTIFIER9 cardInfo;
    pD3D->GetAdapterIdentifier(D3DADAPTER_DEFAULT, 0, &cardInfo);    

    nRetCode = _sntprintf(
        m_DumpBase.szDriverVersion, sizeof(szBuffer) / sizeof(TCHAR), 
        _T("%d.%d.%d.%d"),
        HIWORD(cardInfo.DriverVersion.HighPart),
        LOWORD(cardInfo.DriverVersion.HighPart),
        HIWORD(cardInfo.DriverVersion.LowPart),
        LOWORD(cardInfo.DriverVersion.LowPart)
    );
    KG_PROCESS_ERROR(nRetCode > 0);
    m_DumpBase.szDriverVersion[sizeof(m_DumpBase.szDriverVersion) / sizeof(TCHAR) - 1] = _T('\0');

    _tcsncpy(m_DumpBase.szDriver, cardInfo.Driver, sizeof(m_DumpBase.szDriver) / sizeof(TCHAR) - 1);
    _tcsncpy(m_DumpBase.szVideoCard, cardInfo.Description, sizeof(m_DumpBase.szVideoCard) / sizeof(TCHAR) - 1);

    return TRUE;
Exit0:
    return FALSE;
}

int KDumper::GetPhysMem()
{
    int nRetCode = FALSE;

    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(memInfo);
    nRetCode = GlobalMemoryStatusEx(&memInfo);
    KG_PROCESS_ERROR(nRetCode);

    m_DumpBase.dwTotalPhysMem = (DWORD)memInfo.ullTotalPhys / (1024 * 1024);

    return TRUE;
Exit0:
    return FALSE;
}

int KDumper::GetCPUInfo()
{
    int cpuInfo[4] = {-1};
    char cpuBrandString[0x40] = {0};
    unsigned exIds;

    __cpuid(cpuInfo, 0x80000000);
    exIds = cpuInfo[0];
    if (0x80000002 <= exIds) {
        __cpuid(cpuInfo, 0x80000002);
        memcpy(cpuBrandString, cpuInfo, sizeof(cpuInfo));
        __cpuid(cpuInfo, 0x80000003);
        memcpy(cpuBrandString + 16, cpuInfo, sizeof(cpuInfo));
        __cpuid(cpuInfo, 0x80000004);
        memcpy(cpuBrandString + 32, cpuInfo, sizeof(cpuInfo));
    }

    _tcsncpy(m_DumpBase.szCPUBrand, cpuBrandString, sizeof(m_DumpBase.szCPUBrand) / sizeof(TCHAR) - 1);

    return TRUE;
}

int KDumper::CheckOSDumpFile(FILETIME LastModifiedTime)
{
	int nResult = FALSE;
	int nRetCode = FALSE;
	int nRetFileIndex = -1;       
	TCHAR szSystemRoot[MAX_PATH];
	TCHAR *pRet = NULL;
	vector<tstring> vecFileList;

	KG_PROCESS_ERROR(m_bIsNotCapture);

	nRetCode = GetSystemWindowsDirectory(szSystemRoot, sizeof(szSystemRoot) / sizeof(TCHAR));
	KG_PROCESS_ERROR(nRetCode);

	pRet = _tcscat(szSystemRoot, _T("\\minidump"));
	KG_PROCESS_ERROR(pRet);

	nRetCode = _access(szSystemRoot, 0);
	KG_PROCESS_ERROR(nRetCode == 0);

	nRetCode = FindAllFiles(szSystemRoot, _T(".dmp"), vecFileList);
	KG_PROCESS_ERROR(nRetCode);

	KG_PROCESS_ERROR(!vecFileList.empty());

	nRetCode = GetFileByFileTime(vecFileList, LastModifiedTime, 0, OS_DUMP_FILE_TIME_OVERTIME, nRetFileIndex);
	KG_PROCESS_ERROR(nRetCode);
	KG_PROCESS_ERROR(nRetFileIndex != -1);    

	m_strOSDumpPath = vecFileList[nRetFileIndex];
	nResult = TRUE;
Exit0:
	return nResult;
}

int KDumper::CopyToDumpFile(tstring& szSource, TCHAR szDest[], int nDestSize)
{
	int nResult = FALSE;
	int nRetCode = FALSE;
	const TCHAR *pFullFileName = NULL;

	KG_PROCESS_ERROR(!szSource.empty());
	KG_PROCESS_ERROR(m_DumpBase.szDumpPath[0]);
	KG_PROCESS_ERROR(szDest);
	KG_PROCESS_ERROR(nDestSize > 0);

	pFullFileName = _tcsrchr(szSource.c_str(), _T('\\'));
	KG_PROCESS_ERROR(pFullFileName);

	nRetCode = _sntprintf(
		szDest,
		nDestSize,
		_T("%s\\%s"),
		m_DumpBase.szDumpPath,
		pFullFileName + 1
	);
	KG_PROCESS_ERROR(nRetCode > -1 && nRetCode < nDestSize);

	nRetCode = CopyFile(szSource.c_str(), szDest, FALSE);
	KG_PROCESS_ERROR(nRetCode);

#ifdef OPEN_LOG_AND_BREAK
    KLogPrintf("拷贝文件%s\t%s", szSource.c_str(), szDest);
#endif	

	nResult = TRUE;
Exit0:
	return nResult;
}

int KDumper::CheckJX3LogForOSDump(FILETIME LastModifiedTime)
{
	int nResult = FALSE;
	int nRetCode = FALSE;
	TCHAR szCurrentPath[MAX_PATH];
	const TCHAR *pFullFileName = NULL;
	TCHAR szSearchPath[MAX_PATH];
	vector<tstring> vecLogFileList;
	int nRetFileIndex = -1;

	nRetCode = GetCurrentDirectory(sizeof(szCurrentPath) / sizeof(TCHAR) - 1, szCurrentPath);
	KG_PROCESS_ERROR(nRetCode);

	nRetCode = _sntprintf(
		szSearchPath,
		sizeof(szSearchPath) / sizeof(TCHAR),
		_T("%s\\%s"),
		szCurrentPath,
		m_strLogPath.c_str()
	);
	KG_PROCESS_ERROR(nRetCode > 0);
	szSearchPath[sizeof(szSearchPath) / sizeof(TCHAR) - 1] = _T('\0');

	nRetCode = FindAllFiles(szSearchPath, _T(".log"), vecLogFileList);//工作目录下
	KG_PROCESS_ERROR(nRetCode);

	nRetCode = GetFileByFileTime(
		vecLogFileList, 
		LastModifiedTime, 
		-OS_DUMP_FILE_TIME_OVERTIME, 
		OS_DUMP_FILE_TIME_OVERTIME, 
		nRetFileIndex
	);
	KG_PROCESS_ERROR(nRetCode);
	KG_PROCESS_ERROR(nRetFileIndex != -1);    

	m_strLatestLogName = vecLogFileList[nRetFileIndex];
	nResult = TRUE;
Exit0:
	return nResult;
}

int KDumper::IsSingleProcess()
{
    int nResult = FALSE;
    int nRetCode = FALSE;
    int nProcessNum = 0;
    TCHAR szAppPath[MAX_PATH];
    
    nRetCode = GetModuleFileName(NULL, szAppPath, MAX_PATH);
    KG_PROCESS_ERROR(nRetCode);

    nRetCode = KProcess::FindProcess(szAppPath, &nProcessNum);
    KG_PROCESS_ERROR(nRetCode);

    KG_PROCESS_ERROR(nProcessNum == 1);

    nResult = TRUE;
Exit0:
    return nResult;
}

int KDumper::MakeNotCaptureDumpKey()
{
    int nResult = FALSE;
    int nRetCode = FALSE;
    TCHAR szDumpKey[128] = {0};
    TCHAR szTempKey[128] = {0};

    KG_PROCESS_ERROR(m_DumpBase.szProductVersion[0]);

    nRetCode = _sntprintf(szDumpKey, sizeof(szDumpKey) / sizeof(TCHAR),
        _T("%s%s%s"),
        _T("EXCEPTION_NOT_CAPTURE"),
        m_DumpBase.szProductVersion,
        m_DumpBase.szProductVersionEx
    );
    KG_PROCESS_ERROR(nRetCode > 0);
    szDumpKey[sizeof(szDumpKey) / sizeof(TCHAR) - 1] = _T('\0');

    KG_EDStringToMD5String(szTempKey, szDumpKey);
    _tcsncpy(m_DumpBase.szDumpKey, szTempKey, sizeof(m_DumpBase.szDumpKey) / sizeof(TCHAR));

    nResult = TRUE;
Exit0:
    return nResult;
}

