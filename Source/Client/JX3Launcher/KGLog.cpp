//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   KGLog.cpp
//  Version     :   1.0
//  Creater     :   Freeway Chen
//  Date        :   2004-12-22 17:39:09
//  Comment     :   
//
//////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <errno.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#ifdef __GNUC__
#include <unistd.h>
#endif
#include "KGPublic.h"
#include "KGLog.h"
#include <time.h>
#include <direct.h>
#include <io.h>
#include <sys/stat.h>

static int              gs_nPriorityMask    = 0xff;
static KGLOG_PARAM      gs_LogParam = { 0 };
static int              gs_nLogFile = -1;
int                     gs_nCurrentLine = 0;
KMutex                  gs_Lock;
static char             gs_szLogFileNamePrefix[PATH_MAX] = { 0 };
static int              gs_nIsFirstTimeToWriteLog = true;
struct tm               gs_tmLastChangeFile = { 0 }; 


//#if (defined(_MSC_VER) || defined(__ICL))
#define _KGLOG_OPENFILEFLAG_   (O_CREAT | O_APPEND | O_WRONLY | O_TRUNC | O_BINARY)
#define _KGLOG_OPENFILEMODE_   (S_IREAD | S_IWRITE)
// #else
// #define _KGLOG_OPENFILEFLAG_   (O_CREAT | O_APPEND | O_WRONLY | O_TRUNC)
// #define _KGLOG_OPENFILEMODE_   (S_IREAD | S_IWRITE |  S_IRGRP | S_IROTH)
// #endif

inline struct tm *localtime_r(const time_t *timep, struct tm *result)
{
	struct tm *ptm = localtime(timep);
	if (
		(result) &&
		(ptm)
		)
	{
		*result = *ptm;
	}

	return ptm;
};

inline void *KGThread_GetSelfId()
{
	return (void *)((char *)NULL + GetCurrentThreadId());   // for no 64 bit check warning
}

static int _ChangeLogFile()
{
    int nResult  = false;
    int nRetCode = false;
	time_t tmtNow = 0;

    char szLogFullPath[PATH_MAX];
    char szLogFileName[PATH_MAX];

    nRetCode = KG_mkdir(gs_LogParam.szPath);
    if ((nRetCode == -1) && (errno != EEXIST))
    {
        fprintf(stderr, "KGLog: KG_mkdir(\"%s\") failed = %s\n", gs_LogParam.szPath, strerror(errno));
        goto Exit0;
    }

    nRetCode = KG_mkdir(gs_szLogFileNamePrefix);
    if ((nRetCode == -1) && (errno != EEXIST))
    {
        fprintf(stderr, "KGLog: KG_mkdir(\"%s\") failed = %s\n", gs_szLogFileNamePrefix, strerror(errno));
        goto Exit0;
    }

    if (gs_nLogFile != -1)
    {
        _close(gs_nLogFile);
        gs_nLogFile = -1;
    }
    
    struct tm tmNow; 

    tmtNow = time(NULL);

    localtime_r(&tmtNow, &tmNow);

    nRetCode = _snprintf(
        szLogFullPath, sizeof(szLogFullPath) - 1,
        "%s%d_%2.2d_%2.2d/",
        gs_szLogFileNamePrefix, 
        tmNow.tm_year + 1900,
        tmNow.tm_mon + 1,
        tmNow.tm_mday
    );
    ASSERT((nRetCode != -1) && (nRetCode < (sizeof(szLogFullPath) - 1)));
    szLogFullPath[sizeof(szLogFullPath) - 1] = '\0';

    nRetCode = KG_mkdir(szLogFullPath);
    if ((nRetCode == -1) && (errno != EEXIST))
    {
        fprintf(stderr, "KGLog: KG_mkdir(\"%s\") failed = %s\n", szLogFullPath, strerror(errno));
    }
    
    nRetCode = _snprintf(
        szLogFileName, sizeof(szLogFileName) - 1,
        "%s%s_%d_%2.2d_%2.2d_%2.2d_%2.2d_%2.2d.log", 
        szLogFullPath, 
        gs_LogParam.szIdent,
        tmNow.tm_year + 1900,
        tmNow.tm_mon + 1,
        tmNow.tm_mday,
        tmNow.tm_hour,
        tmNow.tm_min,
        tmNow.tm_sec
    );
    ASSERT((nRetCode != -1) && (nRetCode < (sizeof(szLogFileName) - 1)));
    szLogFileName[sizeof(szLogFileName) - 1] = '\0';

    gs_nLogFile = _open(szLogFileName, _KGLOG_OPENFILEFLAG_, _KGLOG_OPENFILEMODE_);
    if (gs_nLogFile == -1)
    {
        fprintf(stderr, "KGLog: open(\"%s\") failed = %s\n", szLogFileName, strerror(errno));
    }
    KG_PROCESS_ERROR(gs_nLogFile != -1);

    nResult = true;
Exit0:
    if (!nResult)
    {
        if (gs_nLogFile != -1)
        {
            _close(gs_nLogFile);
            gs_nLogFile = -1;
        }
    }
    return nResult;
}

// cszIdent is file name prefix
int KGLogInit(const KGLOG_PARAM& cLogParam, void *pvContext)
{
    int nResult  = false;
    int nRetCode = false;
    KG_USE_ARGUMENT(pvContext);

    KG_PROCESS_ERROR(cLogParam.szPath[0]  != '\0');
    KG_PROCESS_ERROR(cLogParam.szIdent[0] != '\0');

    gs_LogParam = cLogParam;

    if (gs_LogParam.Options == 0)
        gs_LogParam.Options = KGLOG_OPTION_FILE;

    if (gs_LogParam.nMaxLineEachFile <= 0)
        gs_LogParam.nMaxLineEachFile = 32 * 1024;
  
    gs_LogParam.szPath[sizeof(gs_LogParam.szPath) - 1]  = '\0';
    gs_LogParam.szIdent[sizeof(gs_LogParam.szIdent) - 1] = '\0';

    if (gs_LogParam.Options & KGLOG_OPTION_FILE)
    {
        nRetCode = (int)strlen(gs_LogParam.szPath);
        ASSERT((nRetCode > 0) && "Invalid log file path !");
        KG_PROCESS_ERROR(nRetCode < PATH_MAX);

        if (gs_LogParam.szPath[nRetCode - 1] == '\\')
        {
            gs_LogParam.szPath[nRetCode - 1] = '/';
        }
        
        if (gs_LogParam.szPath[nRetCode - 1] != '/')
        {            
            gs_LogParam.szPath[nRetCode] = '/';
            gs_LogParam.szPath[nRetCode + 1] = '\0';
        }

        nRetCode = (int)strlen(gs_LogParam.szIdent);
        ASSERT((nRetCode > 0) && "Invalid log file path !");
 
        if ((gs_LogParam.szIdent[nRetCode - 1] == '/') || (gs_LogParam.szIdent[nRetCode - 1] == '\\'))
        {            
            gs_LogParam.szIdent[nRetCode - 1] = '\0';
        }

        nRetCode = (int)_snprintf(
            gs_szLogFileNamePrefix, 
            sizeof(gs_szLogFileNamePrefix) - 1, 
            "%s%s/", 
            gs_LogParam.szPath, 
            gs_LogParam.szIdent
        );
        gs_szLogFileNamePrefix[sizeof(gs_szLogFileNamePrefix) - 1] = '\0';
        KG_PROCESS_ERROR((nRetCode != -1) && (nRetCode < (sizeof(gs_szLogFileNamePrefix) - 1)));
    }
    
    nResult = true;
Exit0:
    return nResult;
}

int KGLogUnInit(void *pvContext)
{
    int nResult = false;

    KG_USE_ARGUMENT(pvContext);

    if (gs_nLogFile != -1)
    {
        _close(gs_nLogFile);
        gs_nLogFile = -1;
    }

    nResult = true;
//Exit0:
    return nResult;
}

//  Set the log mask level if nPriorityMask != 0
int KGLogSetPriorityMask(int nPriorityMask)
{
//	if (nPriorityMask != 0)
	gs_nPriorityMask = nPriorityMask;

    return true;
}

static const char *gs_caszPriorityString[KGLOG_PRIORITY_MAX] =
{
    "RESR0",
    "RESR1",
    "RESR2",
    "ERROR",
    "WARN ",
    "RESR3",
    "INFO ",
    "DEBUG"
};

int KGLogPrintf(KGLOG_PRIORITY Priority, const char cszFormat[], ...)
{
    if ((KGLOG_MASK(KGLOG_PRI(Priority)) & gs_nPriorityMask) == 0)
        return true;

    int nResult     = false;
    int nRetCode    = false;
    int nLockFlag   = false;
    int nBufferLen  = 0;
    char szBuffer[1024];

    time_t tmtNow = 0;
    struct tm tmNow; 


    tmtNow = time(NULL);

    localtime_r(&tmtNow, &tmNow);
    
    nRetCode = _snprintf(
        szBuffer, sizeof(szBuffer) - 1,
        "%d%2.2d%2.2d-%2.2d%2.2d%2.2d<%s:%p>: ",
        tmNow.tm_year + 1900,
        tmNow.tm_mon + 1,
        tmNow.tm_mday,
        tmNow.tm_hour,
        tmNow.tm_min,
        tmNow.tm_sec,
        gs_caszPriorityString[KGLOG_PRI(Priority)],
        KGThread_GetSelfId()
    );
    szBuffer[sizeof(szBuffer) - 1] = '\0';

    if ((nRetCode != -1) && (nRetCode < (sizeof(szBuffer) - 1)))
        nBufferLen = nRetCode;
    else
        nBufferLen = sizeof(szBuffer) - 1;       

    va_list marker;
	va_start(marker, cszFormat);

    nRetCode = vsnprintf(szBuffer + nBufferLen, sizeof(szBuffer) - 1 - nBufferLen, cszFormat, marker);

    va_end(marker);

    szBuffer[sizeof(szBuffer) - 1] = '\0';

    if ((nRetCode != -1) && (nRetCode < (sizeof(szBuffer) - 1)))
        nBufferLen += nRetCode;
    else
        nBufferLen = sizeof(szBuffer) - 1;       

    if (
        (szBuffer[nBufferLen - 1] != '\n') && 
        (szBuffer[nBufferLen - 1] != '\r')
    )
    {
        if (nBufferLen >= (sizeof(szBuffer) - 1))   // if full
            nBufferLen--;

        szBuffer[nBufferLen] = '\n';
        szBuffer[nBufferLen + 1] = '\0';
        nBufferLen++;
    }
    
    if (gs_LogParam.Options & KGLOG_OPTION_CONSOLE)
    {
        fputs(szBuffer, stdout);
    }
    if (gs_LogParam.Options & KGLOG_OPTION_STDERR)
    {
        fputs(szBuffer, stderr);
    }

    if (gs_LogParam.Options & KGLOG_OPTION_FILE)
    {
        gs_Lock.Lock();
        nLockFlag = true;        

        if (
            gs_nIsFirstTimeToWriteLog ||
            (gs_nCurrentLine >= gs_LogParam.nMaxLineEachFile) ||
            (!(
                (gs_tmLastChangeFile.tm_mday == tmNow.tm_mday) &&
                (gs_tmLastChangeFile.tm_mon  == tmNow.tm_mon)  &&
                (gs_tmLastChangeFile.tm_year == tmNow.tm_year)
            ))
        )
        {            
            gs_nIsFirstTimeToWriteLog = false;
            gs_tmLastChangeFile = tmNow;
            
            nRetCode = _ChangeLogFile();
            KG_PROCESS_ERROR(nRetCode);
            gs_nCurrentLine = 0;
        }

        if (gs_nLogFile != -1)
        {
            nRetCode = _write(gs_nLogFile, szBuffer, nBufferLen);
            if (nRetCode == -1)
            {
                puts("warning : error on write log file, please check your hard disk!");
            }
        }        
        ++gs_nCurrentLine;
    }

    nResult = true;
Exit0:
    if (nLockFlag)
    {
        gs_Lock.Unlock();
        nLockFlag = false;
    }
    return nResult;
}

