// BaseLib.cpp : Implementation of CBaseLib

#include "stdafx.h"
#include "BaseLib.h"
#include "IEOther.h"
#include "Engine.h"

// CBaseLib

IKG3DEngineManager* g_p3DEngine;
HMODULE g_hModule;
IEKG3DGraphicsTool* g_p3DTools;

// CBaseLib

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void InitLog()
{
    int         nRetCode        = false;
    int         nLogInitFlag    = false;
    int         nMemoryInitFlag = false;
    char		szLogsPath[MAX_PATH] = { 0 };
    char		szRootPath[MAX_PATH] = { 0 };
    KGLOG_PARAM LogParam;

    memset(&LogParam, 0, sizeof(LogParam));
    GetCurrentDirectoryA(MAX_PATH, szRootPath);
    _snprintf(LogParam.szPath, _countof(LogParam.szPath), "%s\\%s", szRootPath, "logs");

    strncpy(LogParam.szIdent, "AtlRecordeOperator", _countof(LogParam.szIdent));
    LogParam.nMaxLineEachFile = 65536 * 2;
    LogParam.Options = (KGLOG_OPTIONS)KGLOG_OPTION_FILE;

    nRetCode = KGLogInit(LogParam, NULL);
    KG_PROCESS_ERROR(nRetCode);
    nLogInitFlag = true;
Exit0:
    return;
}

STDMETHODIMP CBaseLib::Init(int nhWnd)
{
    InitLog();

    // TODO: Add your implementation code here
    typedef HRESULT (*fnGet3DEngineInterface)(void**);

    HRESULT hResult = E_FAIL;
    HRESULT hRetCode = E_FAIL;
    fnGet3DEngineInterface pfnGet3DEngineInterface; 

    g_SetRootPath(NULL);
    g_SetFindFileMode(TRUE);
    g_SetFilePath("");

    KGLOG_PROCESS_ERROR(nhWnd);
    m_hWnd = (HWND)nhWnd;

    // TODO: Add your implementation code here
    #ifdef _DEBUG   
        g_hModule = ::LoadLibraryA("KG3DEngineD.dll");
    #else
    g_hModule = ::LoadLibraryA("KG3DEngine.dll");
    #endif
    KGLOG_PROCESS_ERROR(g_hModule);

    pfnGet3DEngineInterface = (fnGet3DEngineInterface)GetProcAddress(g_hModule, "Get3DEngineInterface");
    KGLOG_PROCESS_ERROR(pfnGet3DEngineInterface);

    hRetCode = pfnGet3DEngineInterface((void**)&g_p3DEngine);
    KGLOG_PROCESS_ERROR(SUCCEEDED(hRetCode));
    KGLOG_PROCESS_ERROR(g_p3DEngine);

    IEKG3DEngineManager* pEngine = static_cast<IEKG3DEngineManager*>(g_p3DEngine);
    KGLOG_PROCESS_ERROR(pEngine);

    pEngine->GetGraphicsTool(&g_p3DTools);
    KGLOG_PROCESS_ERROR(g_p3DTools);

    hRetCode = g_p3DEngine->Init(0, KG3DENG_CLIENT | KG3DENG_RECORD, m_hWnd, m_hWnd);
    KGLOG_PROCESS_ERROR(SUCCEEDED(hRetCode));

    hResult = S_OK;
Exit0:
    return S_OK;
}

STDMETHODIMP CBaseLib::UnInit(void)
{
    // TODO: Add your implementation code here

    g_p3DEngine->UnInit();

    return S_OK;
}
