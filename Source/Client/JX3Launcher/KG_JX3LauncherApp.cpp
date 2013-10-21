////////////////////////////////////////////////////////////////////////////////
//  Copyright(c) Kingsoft
//
//  FileName    : KG_JX3LauncherApp.cpp
//  Version     : 1.0
//  Creator     : Hu Hao
//  Create Date : 2009-10-29  15:08:11
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KG_JX3LauncherApp.h"
#include "KG_JX3LauncherDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


BEGIN_MESSAGE_MAP(KG_JX3LauncherApp, CWinApp)
	// ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

KG_JX3LauncherApp::KG_JX3LauncherApp()
{
}

KG_JX3LauncherApp theApp;

BOOL KG_JX3LauncherApp::InitInstance()
{
    int nResult  = false;
    int nRetCode = false;
    TCHAR szApplicationName[MAX_PATH];
    tstring strApplicationPath;
    int nKGLogInitFlag = false;
    KGLOG_PARAM KGLogParam  = {"Logs", "JX3Launcher", KGLOG_OPTION_FILE, 100000};
    KG_JX3LauncherDlg JX3LauncherDialog;

    nRetCode = GetModuleFileName(NULL, szApplicationName, _countof(szApplicationName));
    KG_PROCESS_ERROR(nRetCode > 0);
    KG_PROCESS_ERROR(nRetCode < _countof(szApplicationName));

    strApplicationPath = szApplicationName;
    nRetCode = (int)strApplicationPath.find_last_of(_T('\\'));
    KG_PROCESS_ERROR(nRetCode != (int)tstring::npos);
    strApplicationPath.erase(nRetCode + 1);

    nRetCode = SetCurrentDirectory(strApplicationPath.c_str());
    KG_PROCESS_ERROR(nRetCode);

    nRetCode = KGLogInit(KGLogParam, NULL);
    KG_PROCESS_ERROR(nRetCode);
    nKGLogInitFlag = true;

    KGLogPrintf(KGLOG_INFO, "The program is built at "__DATE__" "__TIME__"\n");

    CWinApp::InitInstance();

    JX3LauncherDialog.DoModal();

    nResult = true;
Exit0:
    if (nKGLogInitFlag)
    {
        KGLogUnInit(NULL);
        nKGLogInitFlag = false;
    }
    return nResult;
}
