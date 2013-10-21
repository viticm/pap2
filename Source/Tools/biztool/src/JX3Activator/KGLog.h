//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   KGLog.h
//  Version     :   1.0
//  Creater     :   Freeway Chen
//  Date        :   2004-12-22 17:39:39
//  Comment     :   
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _KGLOG_H_
#define _KGLOG_H_    1
//#include "EngineBase.h"
//#include "KGCRT.h"
//#include "KSUnknown.h"
#include <Windows.h>
#include "KGPublic.h"
#include <fcntl.h>
#include <stdio.h>
// priorities/facilities are encoded into a single 32-bit quantity, where the
// bottom 3 bits are the priority (0-7) and the top 28 bits are the facility
// (0-big number).  Both the priorities and the facilities map roughly
// one-to-one to strings in the syslogd(8) source code.  This mapping is
// included in this file.
// 
// priorities (these are ordered)

#pragma warning(disable: 4996)

// #ifdef UPDATECORE_EXPORTS
// #define ENGINE_API __declspec(dllexport)
// #else
// #define ENGINE_API __declspec(dllimport)
// #endif

#ifndef PATH_MAX
	#define PATH_MAX    1024
#endif

#define ENGINE_API

//ENGINE_API
class  KMutex
{
public:
	KMutex()        { InitializeCriticalSection(&m_CriticalSection);            }
	~KMutex()       { DeleteCriticalSection(&m_CriticalSection);                }
	int Lock()      { EnterCriticalSection(&m_CriticalSection); return true;    }
	int Unlock()    { LeaveCriticalSection(&m_CriticalSection); return true;    }

private:
	CRITICAL_SECTION m_CriticalSection;
};

#if (defined(_MSC_VER) || defined(__ICL))
	#define KG_mkdir _mkdir
#else   // if linux
inline int KG_mkdir(const char cszDir[])
{
	return mkdir(cszDir, 0777);
}
#endif

enum KGLOG_PRIORITY
{
    KGLOG_RESERVE0  =   0,  // KGLOG_EMERG  =   0,  // system is unusable
    KGLOG_RESERVE1  =   1,  // KGLOG_ALERT  =   1,  // action must be taken immediately
    KGLOG_RESERVE2  =   2,  // KGLOG_CRIT   =   2,  // critical conditions
    KGLOG_ERR		=   3,  // error conditions
    KGLOG_WARNING	=   4,  // warning conditions
    KGLOG_RESERVE3  =   5,  // KGLOG_NOTICE =   5,  // normal but significant condition
    KGLOG_INFO	    =   6,  // informational 
    KGLOG_DEBUG	    =   7,  // debug-level messages
    KGLOG_PRIORITY_MAX
};

enum KGLOG_OPTIONS
{
    KGLOG_OPTION_FILE      =   0x01,   // log on to file, default
    KGLOG_OPTION_CONSOLE   =   0x02,   // log on the console if errors in sending
    KGLOG_OPTION_STDERR    =   0x04,   // log on the stderr stream
    //KGLOG_UDP       =   0x08,   // log to syslogd server use UDP
};

#define KGLOG_PRIMASK    0x07

#define	KGLOG_PRI(pri)	((pri) & KGLOG_PRIMASK)

// arguments to setlogmask.
#define	KGLOG_MASK(pri) (1 << (pri))		    // mask for one priority
#define	KGLOG_UPTO(pri) ((1 << ((pri)+1)) - 1)	// all priorities through pri


typedef struct _KGLOG_PARAM
{
    char szPath[PATH_MAX];      // if KGLOG_FILE
    char szIdent[PATH_MAX];     // if KGLOG_FILE
    KGLOG_OPTIONS Options;          // 0 is default
    int  nMaxLineEachFile;

} KGLOG_PARAM;

// cszIdent is file name prefix
ENGINE_API int KGLogInit(const KGLOG_PARAM& cLogParam, void *pvContext);
ENGINE_API int KGLogUnInit(void *pvContext);

//  Set the log mask level if nPriorityMask != 0
ENGINE_API int KGLogSetPriorityMask(int nPriorityMask);

ENGINE_API int KGLogPrintf(KGLOG_PRIORITY Priority, const char cszFormat[], ...);


#define KGLOG_PROCESS_ERROR(Condition) \
    do  \
    {   \
        if (!(Condition))       \
        {                       \
            KGLogPrintf(        \
                KGLOG_DEBUG,    \
                "KGLOG_PROCESS_ERROR(%s) at line %d in %s\n", #Condition, __LINE__, KG_FUNCTION  \
            );                  \
            goto Exit0;         \
        }                       \
    } while (false)


#define KGLOG_OUTPUT_ERROR(Condition) \
    do  \
    {   \
        if (!(Condition))       \
        {                       \
            KGLogPrintf(        \
                KGLOG_DEBUG,    \
                "KGLOG_PROCESS_ERROR(%s) at line %d in %s\n", #Condition, __LINE__, KG_FUNCTION  \
            );                  \
        }                       \
    } while (false)

#define KGLOG_PROCESS_SUCCESS(Condition) \
    do  \
    {   \
        if (Condition)          \
        {                       \
            KGLogPrintf(        \
                KGLOG_DEBUG,    \
                "KGLOG_PROCESS_SUCCESS(%s) at line %d in %s\n", #Condition, __LINE__, KG_FUNCTION  \
            );                  \
            goto Exit1;         \
        }                       \
    } while (false)

#define KGLOG_PROCESS_ERROR_RET_CODE(Condition, Code) \
    do  \
    {   \
        if (!(Condition))       \
        {                       \
            KGLogPrintf(        \
                KGLOG_DEBUG,    \
                "KGLOG_PROCESS_ERROR_RET_CODE(%s, %d) at line %d in %s\n", \
                #Condition, (Code), __LINE__, KG_FUNCTION                  \
            );                  \
            nResult = (Code);   \
            goto Exit0;         \
        }                       \
    } while (false)

#define KGLOG_PROCESS_ERROR_RET_COM_CODE(Condition, Code) \
    do  \
    {   \
        if (!(Condition))       \
        {                       \
            KGLogPrintf(        \
                KGLOG_DEBUG,    \
                "KGLOG_PROCESS_ERROR_RET_CODE(%s, %d) at line %d in %s\n", \
                #Condition, (Code), __LINE__, KG_FUNCTION                  \
            );                  \
            hrResult = (Code);  \
            goto Exit0;         \
        }                       \
    } while (false)


#define KGLOG_COM_PROCESS_ERROR(Condition) \
    do  \
    {   \
        if (FAILED(Condition))  \
        {                       \
            KGLogPrintf(        \
                KGLOG_DEBUG,    \
                "KGLOG_COM_PROCESS_ERROR(0x%X) at line %d in %s\n", (Condition), __LINE__, KG_FUNCTION  \
            );                  \
            goto Exit0;         \
        }                       \
    } while (false)


#define KGLOG_COM_PROCESS_SUCCESS(Condition)   \
    do  \
    {   \
        if (SUCCEEDED(Condition))   \
        {                           \
            KGLogPrintf(            \
                KGLOG_DEBUG,        \
                "KGLOG_COM_PROCESS_SUCCESS(0x%X) at line %d in %s\n", (Condition), __LINE__, KG_FUNCTION  \
            );                      \
            goto Exit1;             \
        }                           \
    } while (false)


// KG_COM_PROCESS_ERROR_RETURN_ERROR
#define KGLOG_COM_PROC_ERR_RET_ERR(Condition)  \
    do  \
    {   \
        if (FAILED(Condition))      \
        {                           \
            KGLogPrintf(            \
                KGLOG_DEBUG,        \
                "KGLOG_COM_PROC_ERR_RET_ERR(0x%X) at line %d in %s\n", (Condition), __LINE__, KG_FUNCTION  \
            );                      \
            hrResult = (Condition); \
            goto Exit0;             \
        }                           \
    } while (false)


#define KGLOG_COM_PROC_ERROR_RET_CODE(Condition, Code)     \
    do  \
    {   \
        if (FAILED(Condition))      \
        {                           \
            KGLogPrintf(            \
                KGLOG_DEBUG,        \
                "KGLOG_COM_PROC_ERROR_RET_CODE(0x%X, 0x%X) at line %d in %s\n", (Condition), (Code), __LINE__, KG_FUNCTION  \
            );                      \
            hrResult = (Code);      \
            goto Exit0;             \
        }                           \
    } while (false)

#define KGLOG_CHECK_ERROR(Condition) \
    do  \
    {   \
        if (!(Condition))       \
        {                       \
            KGLogPrintf(        \
                KGLOG_DEBUG,    \
                "KGLOG_CHECK_ERROR(%s) at line %d in %s\n", #Condition, __LINE__, KG_FUNCTION  \
            );                  \
        }                       \
    } while (false)

#define KGLOG_COM_CHECK_ERROR(Condition) \
    do  \
    {   \
        if (FAILED(Condition))       \
        {                       \
            KGLogPrintf(        \
                KGLOG_DEBUG,    \
                "KGLOG_COM_CHECK_ERROR(0x%X) at line %d in %s\n", (Condition), __LINE__, KG_FUNCTION  \
            );                  \
        }                       \
    } while (false)

#define KGLOG_PRINT_MESSAGE(Condition) \
	do  \
	{   \
		KGLogPrintf(        \
		KGLOG_INFO,    \
		Condition \
		);                       \
	} while (false)

#endif  // _KGLOGGER_H_
