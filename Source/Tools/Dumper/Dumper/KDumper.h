///////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
//
// FileName : KDumper.h
// Creator  : chengzhaohui
// Date     : 
// Comment  :
//
///////////////////////////////////////////////////////////////

#ifndef _KDUMPER_H_
#define _KDUMPER_H_

#include "KMutex.h"
#include <vector>
#include <d3d9.h>
#include <ddraw.h>
#include "KDumpType.h"

#define ENV_INFO_FILE         _T("EnvInfo.xml")
#define PRODUCT_VERSION_FILE  _T("version.cfg")
#define PRODUCT_VERSION_KEY   _T("Sword3.version")
#define PRODUCT_VERSIONEX_KEY _T("Sword3.versionex")
#define PRODUCT_VERSIONLINENAME_KEY _T("Sword3.VersionLineName")
#define CONFIG_FILE           _T("config.ini")
#define LOG_PATH              _T("logs\\jx3client\\")

#ifdef _DEBUG
    #define SERVER_IP             _T("10.20.80.131")
#else
    #define SERVER_IP             _T("clientreport.jx3.xoyo.com")
#endif

#define SERVER_PORT           80

#define FILE_BEGIN_MASK     0x01020304
#define DUMP_REMARK_FILE    _T("DumpRemark.dump")
#define DUMP_PARAMETER_FILE _T("DumpParameter.dat")
enum DUMP_REMARK
{
    EXCEPTION_NONE = 0x0000,
    EXCEPTION_ATTACH_BY_OS = 0x0001,
    EXCEPTION_ATTACH_BY_DUMP = 0x0001 << 1    
};
enum PLAYER_LOGIN_REMARK
{
    PLAYER_NONE = 0x0000,
    PLAYER_LOGIN = 0x0001
};

#ifdef _DEBUG
#define DUMP_REPORTER         _T("DumpReportD.exe")
#else
#define DUMP_REPORTER         _T("DumpReport.exe")
#endif

class KDumper
{
private:
    KDumper(void); 
    ~KDumper(void);    
public:       
    static KDumper ms_Dumper;
    int SetProductVersion(const TCHAR szVersion[]);
    int SetServerAddress(const TCHAR szIP[], int nPort);
    int SetLogPath(const TCHAR szPath[], TCHAR szIdent[]);
    int OnProcessAttach();
    int OnProcessDetach();       
private:
    static LONG WINAPI UnhandledExceptionHandler(_EXCEPTION_POINTERS *pExceptionInfo);    

    int InitDumpServerIP();
    int ProcessException(_EXCEPTION_POINTERS *pExceptionInfo);
    int SendReport();
    void SetApplicationDirectory();

    //DUMP文件预处理
    int Prepare();
    void Clearup();
    
    //获取日志相关函数
    int MakeLogFile(const TCHAR cszPath[]);
    int CopyLogFile(const TCHAR cszDst[], const TCHAR cszSrc[]);
    int FindAllFiles(const TCHAR szSearchPath[], TCHAR szKey[], vector<tstring>& vecRetFileList, int nIsOnlySearchCurrentDir = false);
    int GetLatestFile(const vector<tstring>& vecFileList, int& nRetFindIndex);  
    int GetFileByFileTime(const vector<tstring>& vecFileList, FILETIME ftTime, int subTimeMix, int subTimeMax, int& nRetFindIndex);

    //获取config.ini文件，版本号
    int MakeConfigFile();
    int GetProductVersion(const TCHAR cszVersionFile[]);

    //获取玩家信息 
    int PlayerInfoManager();
    int GetPlayerInfo(const TCHAR szLoginPath[]);
    
    //没有被DUMP捕获相关处理函数
    int WriteDumpData(DWORD dwDumpOP);
    int LoadDumpRemark();
    int SaveDumpRemark();
    int SetDumpRemark(DWORD dwDumpRemark) { m_dwDumpRemark = dwDumpRemark; return TRUE; }   
    DWORD GetDumpRemark(){ return m_dwDumpRemark; }
	int CheckOSDumpFile(FILETIME LastModifiedTime);
	int CheckJX3LogForOSDump(FILETIME LastModifiedTime);
	int CopyToDumpFile(tstring& szSource, TCHAR szDest[], int nDestSize);
	
    int IsSingleProcess();
    int CreateNotCaptureFile();
    int DeleteNotCaptureFile();
    int MakeNotCaptureDumpKey();
	int GetFileLastModified(const TCHAR szFileName[], LPFILETIME lpTime);
	int ProcessNotCaptureException(const TCHAR szCurrentDir[]);
	static unsigned __stdcall GameMonitorThreadFunc(void* pArguments);
	static unsigned __stdcall ProcessExceptionThreadFunc(void* pArguments);

    typedef struct _EXCEPTION_REMARK_PACKAGE  
    {
        DWORD dwFileRemark;
        DWORD dwDumpRemark;
        _EXCEPTION_REMARK_PACKAGE()
        {
            ZeroMemory(this, sizeof(_EXCEPTION_REMARK_PACKAGE));
        }
    } EXCEPTION_REMARK_PACKAGE;
    typedef struct _PLAYER_LOGIN_REMARK_PACKAGE
    {
        DWORD dwPlayerLoginRemark;
        WORD wYear;
        WORD wMonth;
        WORD wDay;
        _PLAYER_LOGIN_REMARK_PACKAGE()
        {
            ZeroMemory(this, sizeof(_PLAYER_LOGIN_REMARK_PACKAGE));
        }
    } PLAYER_LOGIN_REMARK_PACKAGE;

    //玩家登录信息相关处理函数
    int IsPlayerLoginToday();

    //系统信息相关函数（显卡，内存，CPU等）
    int GetDumpSystemInfo();
    int GetVideoCard();
    int GetPhysMem();
    int GetCPUInfo();
private:
    DUMP_BASE m_DumpBase;
    DUMP_FILE m_DumpFile;
    PLAYER_LOGIN_REMARK_PACKAGE m_tmPriorLoginInfo;
    PLAYER_LOGIN_REMARK_PACKAGE m_tmCurrentLoginInfo;
    DWORD m_dwDumpRemark;
    bool m_bHaveLoginTime;

    //上次异常退出
    bool m_bIsNotCapture;
    tstring m_strOSDumpPath;
    tstring m_strLatestLogName;//only for NotCapture

    tstring m_strApplicationDirectory;   
    tstring m_strLogPath;   
    tstring m_strServerIP;
    tstring m_strNotCaptureFile;
    int m_nServerPort;
    static KMutex ms_Lock;

    HANDLE m_hNotCaptureFile;
    HANDLE m_hGameMonitorThread;
    bool m_bGameMonitorExitFlag;

    HANDLE m_hExceptionThread;
    DWORD m_dwExceptionThreadId;
};
#endif //_KDUMPER_H_
