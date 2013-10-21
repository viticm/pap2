////////////////////////////////////////////////////////////////////////////////
//  Copyright(c) Kingsoft
//
//  FileName    : main.cpp
//  Version     : 1.0
//  Creator     : Cheng Zhaohui, Zhong Guangze, Hu Hao
//  Create Date : 2009-7-2 10:31:27
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <io.h>
#include "KGU_GameUpdater.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
int _SetCurrentDirectory();
int _GetApplicationPath(tstring *pstrRetApplicationPath);
int _GetParentPath(const TCHAR cszApplicationPath[], tstring *pstrRetParentPath);
int _IsParentVersionExist(const TCHAR cszParentPath[]);

////////////////////////////////////////////////////////////////////////////////////////////////////
int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
    int nResult  = false;
    int nRetCode = false;
    int nKGLogInitFlag = false;
    KGLOG_PARAM KGLogParm = {"logs", "GameUpdater", KGLOG_OPTION_FILE, 1000 * 10};
    int nGameUpdaterInitFlag = false;
    KGU_GameUpdater GameUpdater;
    TCHAR szApplicationName[MAX_PATH];
    TCHAR szCurrentDir[MAX_PATH];    

    nRetCode = _SetCurrentDirectory();
    KG_PROCESS_ERROR(nRetCode);

    nRetCode = KGLogInit(KGLogParm, NULL);
    KG_PROCESS_ERROR(nRetCode);
    nKGLogInitFlag = true;

    // Log the build time which is useful for debugging.
    KGLogPrintf(KGLOG_INFO, "The gameupdater.exe is built at "__DATE__" "__TIME__"\n");

    // Log the application name, which could be distinguished the client is main version or child version.
    nRetCode = GetModuleFileName(NULL, szApplicationName, _countof(szApplicationName));
    KGLOG_PROCESS_ERROR(nRetCode > 0);
    KGLOG_PROCESS_ERROR(nRetCode < _countof(szApplicationName));
    KGLogPrintf(KGLOG_INFO, "The full file name is `%s`", szApplicationName);

    // Log the current working path, which is useful for debugging.
    nRetCode = GetCurrentDirectory(_countof(szCurrentDir), szCurrentDir);
    KGLOG_PROCESS_ERROR(nRetCode > 0);
    KGLOG_PROCESS_ERROR(nRetCode < _countof(szCurrentDir));
    KGLogPrintf(KGLOG_INFO, "The current working path is `%s`", szCurrentDir);

    nRetCode = GameUpdater.Init(lpCmdLine, NULL);
    KGLOG_PROCESS_ERROR(nRetCode && "Init()");
    nGameUpdaterInitFlag = true;

    nRetCode = GameUpdater.Run();
    KGLOG_PROCESS_ERROR(nRetCode && "Run()");
    
    nResult = true;
Exit0:
    if (nGameUpdaterInitFlag)
    {
        GameUpdater.UnInit(NULL);
        nGameUpdaterInitFlag = false;
    }
    if (nKGLogInitFlag)
    {
        KGLogUnInit(NULL);
        nKGLogInitFlag = false;
    }
    return nResult ? 0 : 1;
}

int _SetCurrentDirectory()
{
	int nResult  = false;
	int nRetCode = false;
    int nSetParentDirFlag = false;
    tstring strApplicationPath;
    tstring strParentPath;
    tstring *pstrCurrentDir = NULL;

    nRetCode = _GetApplicationPath(&strApplicationPath);
    KG_PROCESS_ERROR(nRetCode);

    // Set parent directory as current directory on if parent directory exist and parent directory
    // is a based jx3 client.
    nSetParentDirFlag = _GetParentPath(strApplicationPath.c_str(), &strParentPath);
    if (nSetParentDirFlag)
        nSetParentDirFlag = _IsParentVersionExist(strParentPath.c_str());

    if (nSetParentDirFlag)
        pstrCurrentDir = &strParentPath;
    else
        pstrCurrentDir = &strApplicationPath;

    nRetCode = SetCurrentDirectory(pstrCurrentDir->c_str());
    KG_PROCESS_ERROR(nRetCode);       

	nResult = true;
Exit0:
	return nResult;
}

int _GetApplicationPath(tstring *pstrRetApplicationPath)
{
	int nResult  = false;
	int nRetCode = false;
    TCHAR szApplicationName[MAX_PATH];

    ASSERT(pstrRetApplicationPath);

    nRetCode = GetModuleFileName(NULL, szApplicationName, _countof(szApplicationName));
    KG_PROCESS_ERROR(nRetCode > 0);
    KG_PROCESS_ERROR(nRetCode < _countof(szApplicationName));

    *pstrRetApplicationPath = szApplicationName;
    nRetCode = (int)pstrRetApplicationPath->find_last_of(_T('\\'));
    KG_PROCESS_ERROR(nRetCode != (int)tstring::npos);
    pstrRetApplicationPath->erase(nRetCode + 1);
    
	nResult = true;
Exit0:
    if (!nResult)
        pstrRetApplicationPath->clear();
	return nResult;
}

int _GetParentPath(const TCHAR cszApplicationPath[], tstring *pstrRetParentPath)
{
	int nResult  = false;
	int nRetCode = false;
    tstring strParentPath;

    ASSERT(cszApplicationPath);
    ASSERT(cszApplicationPath[0]);
    ASSERT(pstrRetParentPath);

    strParentPath = cszApplicationPath;
    nRetCode = (int)strParentPath.length();
    KG_PROCESS_ERROR(nRetCode > 0);
    if (strParentPath[nRetCode - 1] == _T('\\'))
        strParentPath.erase(nRetCode - 1);

    nRetCode = (int)strParentPath.find_last_of(_T('\\'));
    KG_PROCESS_ERROR(nRetCode != (int)tstring::npos);
    strParentPath.erase(nRetCode + 1);

    *pstrRetParentPath = strParentPath;
	nResult = true;
Exit0:
    if (!nResult)
        pstrRetParentPath->clear();
	return nResult;
}

int _IsParentVersionExist(const TCHAR cszParentPath[])
{
	int nResult  = false;
	int nRetCode = false;
    tstring strPakFileName;

    ASSERT(cszParentPath);
    ASSERT(cszParentPath[0]);

    strPakFileName = tstring(cszParentPath) + _T("pak\\Data_1.pak");
    nRetCode = _taccess(strPakFileName.c_str(), 0);
    KG_PROCESS_ERROR(nRetCode == 0);

	nResult = true;
Exit0:
	return nResult;
}