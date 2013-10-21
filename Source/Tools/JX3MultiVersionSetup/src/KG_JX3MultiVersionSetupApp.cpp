////////////////////////////////////////////////////////////////////////////////
//  Copyright(c) Kingsoft
//
//  FileName    : KG_JX3MultiVersionSetupApp.cpp
//  Version     : 1.0
//  Creator     : Hu Hao
//  Create Date : 2009-10-22  16:31:10
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KG_JX3MultiVersionSetupApp.h"
#include "KG_JX3MultiVersionSetupDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(KG_JX3MultiVersionSetupApp, CWinApp)
	// ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

KG_JX3MultiVersionSetupApp::KG_JX3MultiVersionSetupApp()
{
}

KG_JX3MultiVersionSetupApp theApp;

BOOL KG_JX3MultiVersionSetupApp::InitInstance()
{
    int nResult  = false;
    int nRetCode = false;
    int nKGLogInitFlag = false;
    KGLOG_PARAM KGLogParam  = {"Logs", "JX3MultiVersionSetup", KGLOG_OPTION_FILE, 100000};
    KG_JX3MultiVersionSetupDlg SetupDialog;

#ifdef _DEBUG   // Log only if debug version
    nRetCode = KGLogInit(KGLogParam, NULL);
    KG_PROCESS_ERROR(nRetCode);
    nKGLogInitFlag = true;

    KGLogPrintf(KGLOG_INFO, "The program is built at "__DATE__" "__TIME__"\n");
#endif

    CWinApp::InitInstance();

    SetupDialog.DoModal();

    nResult = true;
Exit0:
    if (nKGLogInitFlag)
    {
        KGLogUnInit(NULL);
        nKGLogInitFlag = false;
    }
    return nResult;
}