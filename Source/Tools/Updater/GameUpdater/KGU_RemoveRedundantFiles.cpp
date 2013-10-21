////////////////////////////////////////////////////////////////////////////////
//  Copyright(c) Kingsoft
//
//  FileName    : KGU_RemoveRedundantFiles.cpp
//  Version     : 1.0
//  Creator     : Hu Hao
//  Create Date : 2009-12-29  16:44:55
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KGU_RemoveRedundantFiles.h"
#include <list>
#include <direct.h>

////////////////////////////////////////////////////////////////////////////////
int _RemoveRedundantLogFile(const TCHAR cszLogFilePath[]);
int _RemoveDirectory(const TCHAR cszDirPath[]);
int _RemoveFile(const TCHAR cszFileName[]);

////////////////////////////////////////////////////////////////////////////////
int KGU_RemoveRedundantFiles()
{
	int nResult  = false;
	int nRetCode = false;
    TCHAR szCurrentDir[MAX_PATH];
    tstring strCurrentDir;
    tstring strLogFilePath;
    tstring strDirPath;
    tstring strFileName;

    // log and 3rdParty directory is in current directory, not exe file path, because of multi-version.
    nRetCode = GetCurrentDirectory(_countof(szCurrentDir), szCurrentDir);
    KGLOG_PROCESS_ERROR(nRetCode > 0);
    KGLOG_PROCESS_ERROR(nRetCode < _countof(szCurrentDir));

    strCurrentDir = szCurrentDir;
    nRetCode = (int)strCurrentDir.length();
    ASSERT(nRetCode > 0);
    if (strCurrentDir[nRetCode - 1] != _T('\\') && strCurrentDir[nRetCode - 1] != _T('/'))
        strCurrentDir += _T('\\');

    strLogFilePath = strCurrentDir + _T("logs\\JX3Launcher\\");
    nRetCode = _RemoveRedundantLogFile(strLogFilePath.c_str());
    // KGLOG_PROCESS_ERROR(nRetCode && "_RemoveRedundantLogFile()");

    strLogFilePath = strCurrentDir + _T("logs\\JX3Client\\");
    nRetCode = _RemoveRedundantLogFile(strLogFilePath.c_str());
    // KGLOG_PROCESS_ERROR(nRetCode && "_RemoveRedundantLogFile()");

    strLogFilePath = strCurrentDir + _T("logs\\GameUpdater\\");
    nRetCode = _RemoveRedundantLogFile(strLogFilePath.c_str());
    // KGLOG_PROCESS_ERROR(nRetCode && "_RemoveRedundantLogFile()");

    strLogFilePath = strCurrentDir + _T("logs\\GameConfig\\");
    nRetCode = _RemoveRedundantLogFile(strLogFilePath.c_str());
    // KGLOG_PROCESS_ERROR(nRetCode && "_RemoveRedundantLogFile()");

    strDirPath = strCurrentDir + _T("3rdParty\\DX9\\");
    nRetCode = _RemoveDirectory(strDirPath.c_str());
    // KGLOG_PROCESS_ERROR(nRetCode && "_RemoveDirectory()");

    strFileName = strCurrentDir + _T("3rdParty\\PhysX_8.10.13_SystemSoftware.exe");
    nRetCode = _RemoveFile(strFileName.c_str());
    // KGLOG_PROCESS_ERROR(nRetCode && "_RemoveFile()");

	nResult = true;
Exit0:
	return nResult;
}

int _RemoveRedundantLogFile(const TCHAR cszLogFilePath[])
{
	int nResult  = false;
	int nRetCode = false;
    tstring strDirPath;
    tstring strFindFileName;
    tstring strLogFilePath;
    HANDLE hFindFile = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATA FileData;
    std::list<WIN32_FIND_DATA> FileDataList;
    std::list<WIN32_FIND_DATA>::iterator it;
    LONGLONG llCreateTime       = 0;
    LONGLONG llLatestCreateTime = 0;
    const TCHAR IGNORE_DIR_PREFIX[] = _T(".");
    const LONGLONG REDUNDANT_TIME_INTERVAL = (LONGLONG)10000000 * 60 * 60 * 24 * 2;
    
    ASSERT(cszLogFilePath);
    ASSERT(cszLogFilePath[0]);

    strDirPath = cszLogFilePath;
    // Append '\\' if need
    nRetCode = (int)strDirPath.length();
    ASSERT(nRetCode > 0);
    if (strDirPath[nRetCode - 1] != _T('\\') && strDirPath[nRetCode - 1] != _T('/'))
        strDirPath += _T('\\');

    strFindFileName = strDirPath + _T("*");
    hFindFile = FindFirstFile(strFindFileName.c_str(), &FileData);
    KGLOG_PROCESS_ERROR(hFindFile != INVALID_HANDLE_VALUE);

    while (true)
    {
        if (FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            nRetCode = _tcsnicmp(FileData.cFileName, IGNORE_DIR_PREFIX, _tcslen(IGNORE_DIR_PREFIX));
            if (nRetCode)
            {
                FileDataList.push_back(FileData);

                llCreateTime = *(LONGLONG *)(&FileData.ftCreationTime);
                if (llCreateTime > llLatestCreateTime)
                    llLatestCreateTime = llCreateTime;
            }
        }

        nRetCode = FindNextFile(hFindFile, &FileData);
        if (nRetCode == false)
        {
            nRetCode = GetLastError();
            if (nRetCode == ERROR_NO_MORE_FILES)
                break;

            KGLOG_PROCESS_ERROR(false && "FindNextFile()");
        }
    }

    for (it = FileDataList.begin(); it != FileDataList.end(); ++it)
    {
        llCreateTime = *(LONGLONG *)(&it->ftCreationTime);
        if (llLatestCreateTime - llCreateTime >= REDUNDANT_TIME_INTERVAL)
        {
            strLogFilePath = strDirPath + it->cFileName;
            nRetCode = _RemoveDirectory(strLogFilePath.c_str());
            // KGLOG_PROCESS_ERROR(nRetCode && "_RemoveDirectory()");
        }
    }

	nResult = true;
Exit0:
    if (!nResult)
    {
        KGLogPrintf(
            KGLOG_DEBUG, "Failed to remove redundant log file in directory `%s`, Last Error: %d",
            cszLogFilePath, GetLastError()
        );
    }
    if (hFindFile != INVALID_HANDLE_VALUE)
    {
        FindClose(hFindFile);
        hFindFile = INVALID_HANDLE_VALUE;
    }
	return nResult;
}

int _RemoveDirectory(const TCHAR cszDirPath[])
{
	int nResult  = false;
	int nRetCode = false;
    tstring strDirPath;
    tstring strFindFileName;
    tstring strSubDirPath;
    tstring strFileFullName;
    HANDLE hFindFile = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATA FileData;
    const TCHAR IGNORE_DIR_PREFIX[] = _T(".");  // Ignore repeat visit.

    ASSERT(cszDirPath);
    ASSERT(cszDirPath[0]);

    strDirPath = cszDirPath;
    // Append '\\' if need
    nRetCode = (int)strDirPath.length();
    ASSERT(nRetCode > 0);
    if (strDirPath[nRetCode - 1] != _T('\\') && strDirPath[nRetCode - 1] != _T('/'))
        strDirPath += _T('\\');

    strFindFileName = strDirPath + _T("*");
    hFindFile = FindFirstFile(strFindFileName.c_str(), &FileData);
    KGLOG_PROCESS_ERROR(hFindFile != INVALID_HANDLE_VALUE);

    while (true)
    {
        if (FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            nRetCode = _tcsnicmp(FileData.cFileName, IGNORE_DIR_PREFIX, _tcslen(IGNORE_DIR_PREFIX));
            if (nRetCode)
            {
                strSubDirPath = strDirPath + FileData.cFileName;
                nRetCode = _RemoveDirectory(strSubDirPath.c_str());
                // KGLOG_PROCESS_ERROR(nRetCode && "_RemoveDirectory()");
            }
        }
        else
        {
            strFileFullName = strDirPath + FileData.cFileName;
            nRetCode = _RemoveFile(strFileFullName.c_str());
            // KGLOG_PROCESS_ERROR(nRetCode && "_RemoveFile()");
        }

        nRetCode = FindNextFile(hFindFile, &FileData);
        if (nRetCode == false)
        {
            nRetCode = GetLastError();
            if (nRetCode == ERROR_NO_MORE_FILES)
                break;

            KGLOG_PROCESS_ERROR(false && "FindNextFile()");
        }
    }

    // Close the handle, or RemoveDirectory() failed.
    if (hFindFile != INVALID_HANDLE_VALUE)
    {
        FindClose(hFindFile);
        hFindFile = INVALID_HANDLE_VALUE;
    }
    
    nRetCode = RemoveDirectory(strDirPath.c_str());
    KGLOG_PROCESS_ERROR(nRetCode && "RemoveDirectory()");

	nResult = true;
Exit0:
    if (!nResult)
    {
        KGLogPrintf(
            KGLOG_DEBUG, "Failed to remove directory `%s`, Last Error: %d",
            cszDirPath, GetLastError()
        );
    }
    if (hFindFile != INVALID_HANDLE_VALUE)
    {
        FindClose(hFindFile);
        hFindFile = INVALID_HANDLE_VALUE;
    }
	return nResult;
}

int _RemoveFile(const TCHAR cszFileName[])
{
	int nResult  = false;
	int nRetCode = false;
    const int RETRY_TIME = 3;

    ASSERT(cszFileName);
    ASSERT(cszFileName[0]);

    for (int i = 0; i < RETRY_TIME; i++)
    {
        SetFileAttributes(cszFileName, FILE_ATTRIBUTE_NORMAL);
        nRetCode = _tremove(cszFileName);
        KG_PROCESS_SUCCESS(nRetCode == 0);
    }

    KG_PROCESS_ERROR(false);    // here indicate failed.

Exit1:
	nResult = true;
Exit0:
    if (!nResult)
        KGLogPrintf(KGLOG_DEBUG, "Failed to remove file `%s`, Last Error: %d", cszFileName, errno);
	return nResult;
}