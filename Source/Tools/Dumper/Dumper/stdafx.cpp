// stdafx.cpp : source file that includes just the standard includes
// DumpClient_dll.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"
#include <time.h>
#include <stdio.h>
#define snprintf  _snprintf
// TODO: reference any additional headers you need in STDAFX.H
// and not in this file

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

int KLogPrintf(const char cszFormat[], ...)
{
	int nResult     = false;
	int nRetCode    = false;
	int nLockFlag   = false;
	int nBufferLen  = 0;
	char szBuffer[1024];
	time_t tmtNow = 0;
	struct tm tmNow; 

	static FILE * s_pLogFile =fopen("dump.log", "a+");//²»¹Ø±Õ
	KG_PROCESS_ERROR(s_pLogFile);

	tmtNow = time(NULL);

	localtime_r(&tmtNow, &tmNow);

	nRetCode = snprintf(
		szBuffer, sizeof(szBuffer) - 1,
		"%d%2.2d%2.2d-%2.2d%2.2d%2.2d: ",
		tmNow.tm_year + 1900,
		tmNow.tm_mon + 1,
		tmNow.tm_mday,
		tmNow.tm_hour,
		tmNow.tm_min,
		tmNow.tm_sec
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

	printf(szBuffer);
	fputs(szBuffer, s_pLogFile);
	fflush(s_pLogFile);

	nResult = true;
Exit0:
	return nResult;
}
