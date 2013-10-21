////////////////////////////////////////////////////////////////////////////////
//  Copyright(c) Kingsoft
//
//  FileName    : KGU_Public.cpp
//  Version     : 1.0
//  Creator     : Hu Hao
//  Create Date : 2009-5-18 17:02:28
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <stdio.h>
#include <Windows.h>
#include <WinInet.h>
#include <Psapi.h>
#include "KGU_Public.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// Used to parse version number.
const unsigned KGU_VERSION_NUMBER_SECTION_COUNT = 4;
struct _KGU_VERSION_NUMBER
{
    unsigned uSectionValueArrray[KGU_VERSION_NUMBER_SECTION_COUNT];
    TCHAR    chPartition;
};

int _ParseVersionNumber(const TCHAR cszVersionNumber[], _KGU_VERSION_NUMBER *pRetVersionNumber);
int _GetPartitionChar(const TCHAR cszVersionNumber[], TCHAR *pchRetPartitionChar);

////////////////////////////////////////////////////////////////////////////////////////////////////
int KGU_DownloadHttpFile(const TCHAR cszURL[], const TCHAR cszFileName[])
{
    int nResult  = false;
    int nRetCode = false;
    HINTERNET hSession      = NULL;
    HINTERNET hInternetFile = NULL;
    DWORD dwStatusCode = 0;
    DWORD dwBufferSize = 0;
    DWORD dwReadSize   = 0;
    DWORD dwTotalReadSize = 0;
    DWORD dwTotalFileSize = 0;
    unsigned char byBuffer[1024 * 64];
    FILE *fpLocalFile = NULL;
    const unsigned RECEIVE_TIMEOUT = 10000;

    KGLOG_PROCESS_ERROR(cszURL);
    KGLOG_PROCESS_ERROR(cszURL[0]);
    KGLOG_PROCESS_ERROR(cszFileName);
    KGLOG_PROCESS_ERROR(cszFileName[0]);

    // Initialize an application's use of the WinINet functions.
    hSession = InternetOpen(
        _T("Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1)"), 
        INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0
    );
    KGLOG_PROCESS_ERROR(hSession);

    // Set receive timeout for response. 
    // warning: INTERNET_OPTION_CONNECT_TIMEOUT is 20s and can not be changed 
    // because of WinINet BUG.
    nRetCode = InternetSetOption(
        hSession, INTERNET_OPTION_RECEIVE_TIMEOUT, (void *)&RECEIVE_TIMEOUT,  sizeof(RECEIVE_TIMEOUT)
    );
    KGLOG_PROCESS_ERROR(nRetCode && "InternetSetOption()");

    hInternetFile = InternetOpenUrl(
        hSession, cszURL, _T("Accept: */*\r\n\r\n"), (DWORD)-1, 
        INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_RELOAD, NULL
    );
    KGLOG_PROCESS_ERROR(hInternetFile);

    // Check the response status code, which should be HTTP_STATUS_OK.
    dwBufferSize = sizeof(dwStatusCode);
    nRetCode = HttpQueryInfo(
        hInternetFile, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER,
        &dwStatusCode, &dwBufferSize, NULL
    );
    KGLOG_PROCESS_ERROR(nRetCode && "HttpQueryInfo()");
    if (dwStatusCode != HTTP_STATUS_OK)
    {
        KGLogPrintf(KGLOG_DEBUG, "Response status code : %d\n", dwStatusCode);
        KGLOG_PROCESS_ERROR(false);
    }

    fpLocalFile = fopen(cszFileName, "wb");
    KGLOG_PROCESS_ERROR(fpLocalFile);

    while (true)
    {
        nRetCode = InternetReadFile(hInternetFile, byBuffer, sizeof(byBuffer), &dwReadSize);
        KGLOG_PROCESS_ERROR(nRetCode && "InternetReadFile()");

        if (dwReadSize == 0)    // No more data.
            break;

        nRetCode = (int)fwrite(byBuffer, 1, dwReadSize, fpLocalFile);
        KGLOG_PROCESS_ERROR(nRetCode == (int)dwReadSize);

        dwTotalReadSize += dwReadSize;
    }

    // If HTTP server stop when reading data, the file is incomplete and no error
    // occur, so check the file size equal to the value of the Content-Length,
    // if which is included in response header.
    // (If respond is chunked encoded, there is no Content-Length in the header).
    dwBufferSize = sizeof(dwTotalFileSize);
    nRetCode = HttpQueryInfo(
        hInternetFile, HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER,
        &dwTotalFileSize, &dwBufferSize, NULL
    );
    if (nRetCode)
    {
        KGLOG_PROCESS_ERROR((dwTotalFileSize == dwTotalReadSize) && "Incomplete file.");
    }    
    
    nResult = true;
Exit0:
    if (!nResult && cszURL && cszFileName)
    {
        KGLogPrintf(
            KGLOG_DEBUG, "Failed to download `%s`, save as `%s`, Error Code %d.",
            cszURL, cszFileName, GetLastError()
        );
    }
    if (fpLocalFile)
    {
        fclose(fpLocalFile);
        fpLocalFile = NULL;
    }
    if (!nResult)
    {   // Remove incomplete file.
        _tremove(cszFileName);
    }
    if (hInternetFile)
    {
        InternetCloseHandle(hInternetFile);
        hInternetFile = NULL;
    }
    if (hSession)
    {
        InternetCloseHandle(hSession);
        hSession = NULL;
    }

    return nResult;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
int KGU_GetRunningProcessCount(const TCHAR cszProcessName[], unsigned *puRetCount)
{
    int nResult  = false;
    int nRetCode = false;
    DWORD dwPIDArray[1024];
    DWORD dwPIDCountBytes = 0;
    unsigned uProcessCount = 0;
    HANDLE hCurrentProcess = NULL;
    TCHAR szProcessName[MAX_PATH];

    KGLOG_PROCESS_ERROR(cszProcessName);
    KGLOG_PROCESS_ERROR(cszProcessName[0]);
    KGLOG_PROCESS_ERROR(puRetCount);

    *puRetCount = 0;

    nRetCode = EnumProcesses(dwPIDArray, sizeof(dwPIDArray), &dwPIDCountBytes);
    KGLOG_PROCESS_ERROR(nRetCode);
    uProcessCount = dwPIDCountBytes / sizeof(DWORD);

    for (unsigned i = 0; i < uProcessCount; i++)
    {
        if (hCurrentProcess)
        {
            CloseHandle(hCurrentProcess);
            hCurrentProcess = NULL;
        }

        hCurrentProcess = OpenProcess(
            PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, dwPIDArray[i]
        );
        if (hCurrentProcess == NULL)    // Ignore the process that can't open.
            continue;

        nRetCode = GetModuleFileNameEx(
            hCurrentProcess, NULL, szProcessName, sizeof(szProcessName) / sizeof(szProcessName[0])
        );
        if (nRetCode <= 0)  // Ignore the process that can't get name.
            continue;

        nRetCode = _tcsicmp(szProcessName, cszProcessName);
        if (nRetCode == 0)
            (*puRetCount)++;
    }
    
    nResult = true;
Exit0:
    if (!nResult && cszProcessName)
    {
        KGLogPrintf(
            KGLOG_DEBUG, "Failed to get process count of `%s`, Error Code : %d", 
            cszProcessName, GetLastError()
        );
    }
    if (hCurrentProcess)
    {
        CloseHandle(hCurrentProcess);
        hCurrentProcess = NULL;
    }
    return nResult;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
int KGU_CompareVersionNumber(const TCHAR cszLeft[], const TCHAR cszRight[], int *pnRetResult)
{
    int nResult  = false;
    int nRetCode = false;
    _KGU_VERSION_NUMBER LeftVersionNumber;
    _KGU_VERSION_NUMBER RightVersionNumber;

    KGLOG_PROCESS_ERROR(cszLeft);
    KGLOG_PROCESS_ERROR(cszLeft[0]);
    KGLOG_PROCESS_ERROR(cszRight);
    KGLOG_PROCESS_ERROR(cszRight[0]);
    KGLOG_PROCESS_ERROR(pnRetResult);

    nRetCode = _ParseVersionNumber(cszLeft, &LeftVersionNumber);
    KGLOG_PROCESS_ERROR(nRetCode && "Left is not a version number.");

    nRetCode = _ParseVersionNumber(cszRight, &RightVersionNumber);
    KGLOG_PROCESS_ERROR(nRetCode && "Right is not a version number.");

    for (unsigned i = 0; i < KGU_VERSION_NUMBER_SECTION_COUNT; i++)
    {
        if (LeftVersionNumber.uSectionValueArrray[i] > RightVersionNumber.uSectionValueArrray[i])
        {
            *pnRetResult = 1;
            KG_PROCESS_SUCCESS(true);
        }

        if (LeftVersionNumber.uSectionValueArrray[i] < RightVersionNumber.uSectionValueArrray[i])
        {
            *pnRetResult = -1;
            KG_PROCESS_SUCCESS(true);
        }
    }
    
    *pnRetResult = 0;   // Here indicate two version number equals.
Exit1:
    nResult = true;
Exit0:
    if (!nResult && cszLeft && cszRight)
    {
        KGLogPrintf(
            KGLOG_DEBUG, "Failed to compare two version number, Left is `%s`, Right is `%s`.",
            cszLeft, cszRight
        );
    }
    return nResult;
}

int _ParseVersionNumber(const TCHAR cszVersionNumber[], _KGU_VERSION_NUMBER *pRetVersionNumber)
{
    int nResult  = false;
    int nRetCode = false;
    TCHAR szVersionNumber[256];
    TCHAR chPartition = _T('\0');
    TCHAR *pchSetciton = NULL;
    TCHAR *pchPartition = NULL;

    ASSERT(cszVersionNumber);
    ASSERT(cszVersionNumber[0]);
    ASSERT(pRetVersionNumber);

    nRetCode = _GetPartitionChar(cszVersionNumber, &chPartition);
    KGLOG_PROCESS_ERROR(nRetCode && "_GetPartitionChar()");

    pRetVersionNumber->chPartition = chPartition;

    _tcsncpy(szVersionNumber, cszVersionNumber, sizeof(szVersionNumber) / sizeof(szVersionNumber[0]));
    KGLOG_PROCESS_ERROR(szVersionNumber[sizeof(szVersionNumber) / sizeof(szVersionNumber[0]) - 1] == _T('\0'));
    
    pchSetciton = szVersionNumber;
    for (unsigned i = 0; i < KGU_VERSION_NUMBER_SECTION_COUNT - 1; i++)
    {
        pchPartition = _tcschr(pchSetciton, chPartition);
        KGLOG_PROCESS_ERROR(pchPartition);
        *pchPartition = _T('\0');

        pRetVersionNumber->uSectionValueArrray[i] = _tstoi(pchSetciton);
        pchSetciton = pchPartition + 1;
    }    
    pRetVersionNumber->uSectionValueArrray[KGU_VERSION_NUMBER_SECTION_COUNT - 1] = _tstoi(pchSetciton);
    
    nResult = true;
Exit0:
    return nResult;
}

int _GetPartitionChar(const TCHAR cszVersionNumber[], TCHAR *pchRetPartitionChar)
{
    const TCHAR *pchIndex = NULL;
    const TCHAR PARTITION[] = {_T('.'), _T('-'), _T(',')};

    ASSERT(cszVersionNumber);
    ASSERT(cszVersionNumber[0]);
    ASSERT(pchRetPartitionChar);

    for (unsigned i = 0; i < sizeof(PARTITION) / sizeof(PARTITION[0]); i++)
    {
        pchIndex = _tcschr(cszVersionNumber, PARTITION[i]);
        if (pchIndex)
        {
            *pchRetPartitionChar = PARTITION[i];
            return true;
        }
    }

    return false;   // Here indicate the cszVersionNumber not include any partition char.
}