#include "stdafx.h"
#include <Shellapi.h>
#include <io.h>
#include "KNotificationDlg.h"
#include "KDumpTransport.h"
#include "zip.h"
#include "KGCrashReporter.h"
#include <process.h>
#include "KDumpType.h"

HINSTANCE g_hInst = NULL;

tstring g_strAppName;
tstring g_strReportFile;
vector<tstring> g_vecFileList;

DUMP_BASE g_DumpBase;
DUMP_FILE g_DumpFile;
DWORD g_dwDumpOp = DUMP_OP_NONE;

const int WAIT_TIME = 30 * 1000;

int LoadDumpData();
int SendCapturedDumpFile();
int SendNotCapturedDumpFile();
int SendPlayerLoginFile();
int Compress();
void Clearup();
int RestartApp();
unsigned __stdcall _DumpBaseDataReportThreadFunction(void *pv);

int APIENTRY _tWinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPTSTR    lpCmdLine,
    int       nCmdShow
)
{
    int nResult = EXCEPTION_EXECUTE_HANDLER;
    int nRetCode = FALSE;
    g_hInst = hInstance;

    ZeroMemory(&g_DumpBase, sizeof(DUMP_BASE));
    ZeroMemory(&g_DumpFile, sizeof(DUMP_FILE));   

    nRetCode = LoadDumpData();
    KG_PROCESS_ERROR(nRetCode);

    if (g_dwDumpOp & DUMP_OP_CAPTURE_BY_DUMP)
    {
        nResult = SendCapturedDumpFile();  
    } 
    else 
    {
        if (g_dwDumpOp & DUMP_OP_NOT_CAPTURE_BY_DUMP)
        {
            SendNotCapturedDumpFile();    
        }
        if (g_dwDumpOp & DUMP_OP_PLAYER_LOGIN)
        {
            SendPlayerLoginFile();
        }
    } 
        
    nResult = EXCEPTION_EXECUTE_HANDLER;
Exit0:
	Clearup();
    return nResult;
}

int SendCapturedDumpFile()
{
    int nResult = EXCEPTION_EXECUTE_HANDLER;
    int nRetCode = FALSE;
    KNotificationDlg NotifyDlg;
    OPERATION_TYPE nOprType = otSend;       //send the debug to dll
    KDumpTransport DumpTran;
    KGCrashReporter CrashReport;
    HANDLE hThreadHandle = NULL;
    unsigned uThreadID = 0;

    hThreadHandle = (HANDLE)_beginthreadex(
        NULL,
        0,
        &_DumpBaseDataReportThreadFunction, 
        NULL,
        0,
        &uThreadID
    );
    KG_PROCESS_ERROR(hThreadHandle);

    nRetCode = NotifyDlg.Notify(g_DumpBase.szDumpPath, &nOprType);
    KG_PROCESS_ERROR(nRetCode);

    switch (nOprType)
    {
    case otSend:
        {
            tstring strFileLevel = tstring(_T("0"));

            TCHAR *pFeedback = NotifyDlg.GetFeedbackInfo();
            if (pFeedback && pFeedback[0])
            {
                nRetCode = CrashReport.UpdateCrashReport(g_DumpFile.szEnvInfoFile, pFeedback);
                KG_PROCESS_ERROR(nRetCode);

                strFileLevel = tstring(_T("1"));
            }

            nRetCode = Compress();
            KG_PROCESS_ERROR(nRetCode);

            //上传dump文件
            nRetCode = DumpTran.TransportDumpFile(
                g_DumpBase.szServerIP, g_DumpBase.nServerPort, g_DumpBase.szDumpKey, g_DumpBase.szDumpModule, g_strAppName.c_str(), 
                g_DumpBase.szProductVersion, g_strReportFile.c_str(), strFileLevel.c_str(), g_DumpFile.szGUID
            );
            KG_PROCESS_ERROR(nRetCode);
        }
        break;
    case otQuit:     
        break;
    case otDebug:
        nResult = EXCEPTION_CONTINUE_SEARCH;
        goto Exit0;
    default:
        break;
    }  

    nResult = EXCEPTION_EXECUTE_HANDLER;
Exit0:
    if (hThreadHandle)
    {
        WaitForSingleObject(hThreadHandle, WAIT_TIME);

        CloseHandle(hThreadHandle);
        hThreadHandle = NULL;
    }
    if (nResult = EXCEPTION_EXECUTE_HANDLER)
    {
        nRetCode = NotifyDlg.GetRestartFlag();
        if (nRetCode)
        {
            RestartApp();
        }
    }
    return nResult;
}

int SendNotCapturedDumpFile()
{
    int nResult = FALSE;
    int nRetCode = FALSE;
    KDumpTransport DumpTran;
    HANDLE hThreadHandle = NULL;
    unsigned uThreadID = 0;
    tstring strFileLevel = tstring(_T("0"));

    hThreadHandle = (HANDLE)_beginthreadex(
        NULL,
        0,
        &_DumpBaseDataReportThreadFunction, 
        NULL,
        0,
        &uThreadID
    );
    KG_PROCESS_ERROR(hThreadHandle);

    nRetCode = Compress();
    KG_PROCESS_ERROR(nRetCode);

    if (g_DumpFile.szDumpFile[0])
    {
        strFileLevel = tstring(_T("1"));
    }

    //上传dump文件
    nRetCode = DumpTran.TransportDumpFile(
        g_DumpBase.szServerIP, g_DumpBase.nServerPort, g_DumpBase.szDumpKey, g_DumpBase.szDumpModule, g_strAppName.c_str(), 
        g_DumpBase.szProductVersion, g_strReportFile.c_str(), strFileLevel.c_str(), g_DumpFile.szGUID
    );
    KG_PROCESS_ERROR(nRetCode);    

    nResult = TRUE;
Exit0:
    if (hThreadHandle)
    {
        WaitForSingleObject(hThreadHandle, WAIT_TIME);

        CloseHandle(hThreadHandle);
        hThreadHandle = NULL;
    }
    return nResult;
}

int SendPlayerLoginFile()
{
	KDumpTransport DumpTran;

	DumpTran.ReportPlayerLoginBase(
		g_DumpBase.szServerIP, g_DumpBase.nServerPort, 
		g_DumpBase.szProductVersion, g_DumpBase.szProductVersionEx,
		g_DumpBase.szVideoCard, g_DumpBase.szDriver, g_DumpBase.szDriverVersion,
		g_DumpBase.szCPUBrand, g_DumpBase.dwTotalPhysMem
		);

	return true;
}

int LoadDumpData()
{
    int nResult = FALSE;
    int nRetCode = FALSE;
    long lFileLength = 0;
    LPTSTR pszCommandList = NULL;
    TCHAR szDumpParameterFile[MAX_PATH];
    FILE *pFile = NULL;
    DUMP_MESSAGE_HEAD DumpHead;
    TCHAR szFileName[MAX_PATH];

    ZeroMemory(&DumpHead, sizeof(DUMP_MESSAGE_HEAD));

    pszCommandList = GetCommandLine();
    KG_PROCESS_ERROR(pszCommandList);

    _tcsncpy(szDumpParameterFile, pszCommandList, sizeof(szDumpParameterFile) / sizeof(TCHAR));
    szDumpParameterFile[sizeof(szDumpParameterFile) / sizeof(TCHAR) - 1] = _T('\0');

    pFile = _tfopen(szDumpParameterFile, _T("rb"));
    KG_PROCESS_ERROR(pFile);
    
    nRetCode = fseek(pFile, 0, SEEK_END);
    KG_PROCESS_ERROR(!nRetCode);

    lFileLength = ftell(pFile);
    KG_PROCESS_SUCCESS(lFileLength == 0);

    nRetCode = fseek(pFile, 0, SEEK_SET);
    KG_PROCESS_ERROR(!nRetCode);

    nRetCode = (int)fread(&DumpHead, sizeof(DUMP_MESSAGE_HEAD), 1, pFile);
    KG_PROCESS_ERROR(nRetCode == 1);

    KG_PROCESS_ERROR(DumpHead.dwBufferSize == lFileLength);
    g_dwDumpOp = DumpHead.dwDumpOP;
        
    nRetCode = (int)fread(&g_DumpBase, sizeof(DUMP_BASE), 1, pFile);
    KG_PROCESS_ERROR(nRetCode == 1);    

    nRetCode = (int)fread(&g_DumpFile, sizeof(DUMP_FILE), 1, pFile);
    KG_PROCESS_ERROR(nRetCode == 1);

    g_vecFileList.push_back(g_DumpFile.szDumpFile);
    g_vecFileList.push_back(g_DumpFile.szLogFile);
    g_vecFileList.push_back(g_DumpFile.szConfigFile);
    g_vecFileList.push_back(g_DumpFile.szEnvInfoFile);

    nRetCode = _splitpath_s(
        g_DumpBase.szAppFullName,
        NULL,
        0,
        NULL,
        0,
        szFileName,
        sizeof(szFileName) / sizeof(TCHAR) - 1,
        NULL,
        0
    );
    KG_PROCESS_ERROR(nRetCode == 0);
    szFileName[sizeof(szFileName) / sizeof(TCHAR) - 1] = _T('\0');

    g_strAppName = tstring(szFileName);

Exit1:
    nResult = TRUE;
Exit0:
    if (pFile)
    {
        fclose(pFile);
        pFile = NULL;
    }
    DeleteFile(szDumpParameterFile);
    return nResult;
}

int Compress()
{
    int nResult = FALSE;
    int nRetCode = FALSE;
    int nPos = 0;
    tstring strSubStr;
    ZRESULT zRetCode = ZR_FAILED;
    HZIP hz = NULL;
    vector<tstring>::iterator it;
	int nZipedFileCount = 0;
    
    KG_PROCESS_ERROR(!g_vecFileList.empty());

    g_strReportFile = tstring(g_DumpBase.szDumpPath) + g_strAppName + tstring(_T("_Crash.zip"));

    hz = CreateZip(g_strReportFile.c_str(), NULL);
    KG_PROCESS_ERROR(hz);

    for (it = g_vecFileList.begin(); it != g_vecFileList.end(); it++)
    {
        if ((*it).empty())
            continue;

        nRetCode = _access((*it).c_str(), 0);
        if (nRetCode != 0)
            continue;

        nPos = (int)(*it).find_last_of(_T('\\'));
        strSubStr = (*it).substr(nPos + 1);

        zRetCode = ZipAdd(hz, strSubStr.c_str(), (*it).c_str());
        KG_PROCESS_ERROR(zRetCode == ZR_OK);

		++nZipedFileCount;
    }
	KG_PROCESS_ERROR(nZipedFileCount > 0);

    nResult = TRUE;
Exit0:
    if (hz)
    {
        CloseZip(hz);
        hz = NULL;
    }
    return nResult;
}

void Clearup()
{
    vector<tstring>::iterator it;

    DeleteFile(g_strReportFile.c_str());

    for (it = g_vecFileList.begin(); it != g_vecFileList.end(); it++)
    {
        if ((*it).empty())
            continue;

        DeleteFile((*it).c_str());
    }
    RemoveDirectory(g_DumpBase.szDumpPath);
}

static unsigned __stdcall _DumpBaseDataReportThreadFunction(void *pv)
{
    KDumpTransport DumpTran;

    DumpTran.ReportCrash(
        g_DumpBase.szServerIP, g_DumpBase.nServerPort, 
        g_DumpBase.szDumpKey, g_DumpBase.szDumpModule, g_strAppName.c_str(), 
        g_DumpBase.szProductVersion, g_DumpBase.szProductVersionEx, 
        g_DumpBase.szAccount, g_DumpBase.szRoleName, g_DumpBase.szRegionName, g_DumpBase.szServerName,
        g_DumpBase.szVideoCard, g_DumpBase.szDriver, g_DumpBase.szDriverVersion,
        g_DumpBase.szCPUBrand, g_DumpBase.dwTotalPhysMem, g_DumpBase.szGUID
    );

    return 0;
}

int RestartApp()
{
    int nResult = FALSE;
    int nRetCode = FALSE;
    STARTUPINFO StartInfo;  
    PROCESS_INFORMATION ProcessInfo;

    KG_PROCESS_ERROR(g_DumpBase.szAppFullName);
    
    ZeroMemory(&StartInfo, sizeof(STARTUPINFO));
    ZeroMemory(&ProcessInfo, sizeof(PROCESS_INFORMATION));

    try
    {
        GetStartupInfo(&StartInfo);
    }
    catch (...)
    {
        goto Exit0;
    }

    nRetCode = CreateProcess(
        NULL,
        (LPSTR)g_DumpBase.szAccount,
        NULL,
        NULL,
        FALSE,
        NORMAL_PRIORITY_CLASS,
        NULL,
        NULL,
        &StartInfo,
        &ProcessInfo
    );
    KG_PROCESS_ERROR(nRetCode);

    CloseHandle(ProcessInfo.hThread);
    CloseHandle(ProcessInfo.hProcess);

    nResult = TRUE;
Exit0:
    return nResult;
}
