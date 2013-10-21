///////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
//
// FileName : KDumpFile.h
// Creator  : chengzhaohui
// Date     : 
// Comment  :
//
///////////////////////////////////////////////////////////////

#ifndef _KDUMPFILE_H_
#define _KDUMPFILE_H_

#include <Windows.h>
#include <DbgHelp.h>
//#include "KProtocol.h"
#define  MINIDUMP_CONFIG       _T(".\\Dumper.ini")
const int KEY_LENGTH = 33;

class KDumpFile
{
public:
    KDumpFile(
		HANDLE hExceptionThread, 
		DWORD dwExceptionThreadId, 
		EXCEPTION_POINTERS *pException, 
		MINIDUMP_CALLBACK_INFORMATION *pCallBack
	);
    ~KDumpFile();
    
    int MakeDump(const TCHAR szVersion[], const TCHAR szPath[]);
    TCHAR *GetDumpKey(){return m_szDumpKey;}
	TCHAR *GetDumpFileName(){return m_szDumpFileName;}
	TCHAR *GetExceptionModuleName(){return m_szExceptionModuleName;}

private:
    int WriteMiniDump(const TCHAR szPath[]);
    int MakeDumpKey(const TCHAR szVersion[]);
    int InitMiniDumpType();
private:    
    EXCEPTION_POINTERS *m_pExceptionInfo;
    MINIDUMP_CALLBACK_INFORMATION *m_pCallBack;

    TCHAR m_szDumpKey[KEY_LENGTH];
    TCHAR m_szAppPath[MAX_PATH];
    TCHAR m_szAppName[MAX_PATH];
	TCHAR m_szDumpFileName[MAX_PATH];
	TCHAR m_szExceptionModuleName[MAX_PATH];
    int   m_nFullMemoryFlag;

    HMODULE m_hHelpDll;
	HANDLE m_hExceptionThread;
	DWORD m_dwExceptionThreadId;
};


#endif // _KDUMPFILE_H_
