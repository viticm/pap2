// JX3Activator.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "JX3Activator.h"
#include "KG_Activator.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CJX3ActivatorApp

BEGIN_MESSAGE_MAP(CJX3ActivatorApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CJX3ActivatorApp construction

CJX3ActivatorApp::CJX3ActivatorApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CJX3ActivatorApp object

CJX3ActivatorApp theApp;

KG_Activator *g_pActivator = NULL;

// CJX3ActivatorApp initialization

BOOL CJX3ActivatorApp::InitInstance()
{
    int nResult  = false;
    int nRetCode = false;
    int nKGLogInitFlag = false;
    HWND hWnd = NULL;
    KGLOG_PARAM LogParam  = {"Logs", "JX3Activator", KGLOG_OPTION_FILE, 65536};
    WSADATA wsaData;

#if 0
    nRetCode = KGLogInit(LogParam, NULL);
    KG_PROCESS_ERROR(nRetCode);
    nKGLogInitFlag = true;
#endif

    nRetCode = WSAStartup(MAKEWORD(2, 2), &wsaData) == 0;
    KGLOG_PROCESS_ERROR(nRetCode);

    hWnd = FindWindow(NULL, "½£Íø3¼«ÏÞ·â²â¼¤»îÉêÇë V1.0.0.2");
    if (hWnd)
    {
        SetForegroundWindow(hWnd);
        KGLOG_PROCESS_ERROR(false);
    }

    nRetCode = CWinApp::InitInstance();
    KGLOG_PROCESS_ERROR(nRetCode && "CWinApp::InitInstance()");

    g_pActivator = new KG_Activator();
    KGLOG_PROCESS_ERROR(g_pActivator);

    SetRegistryKey(_T("JX3Activator"));

    nRetCode = g_pActivator->Run();
    KGLOG_PROCESS_ERROR(nRetCode && "g_pActivator->Run()");
    
    nResult = true;
Exit0:
    KG_DELETE(g_pActivator);
    WSACleanup();
    if (nKGLogInitFlag)
    {
        KGLogUnInit(NULL);
        nKGLogInitFlag = false;
    }
    return nResult;
}
