#include "StdAfx.h"
#include "KSO3Client.h"
#include "./ClientString.h"
#include "IEKG3DScene.h"
#include "IEKG3DSceneSFXEditor.h"
#include "IEKG3DSFX.h"

static char s_szErrorFileName[] = "ErrorRecord.ini"; 
static char s_FilePath[] = "\\data\\source\\other\\特效\\*.*"/*"\\data\\source\\other\\特效\\技能\\SFX\\释放\\*.*"*/;

//////////////////////////////////////////////////////////////////////////
static HRESULT OnLostDevice()
{
    KGWin32App::Enable(FALSE);
    return S_OK;
}

static HRESULT OnResetDevice()
{
    KGWin32App::Enable(TRUE);
    return S_OK;
}

//////////////////////////////////////////////////////////////////////////
KSO3Client::KSO3Client()
: m_p3DEngine(NULL)
, m_pScene(NULL)
, m_pFile(NULL)
, m_nOutputWindowID(-1)
{
}

KSO3Client::~KSO3Client()
{
}

int KSO3Client::OnInit()
{
    int                     nResult             = false;
    int                     nRetCode            = false;
    HRESULT                 hr                  = S_OK;
    BOOL                    bAllocConsole       = false;
    BOOL                    bPakFirst           = true;
    BOOL                    bInitConsole        = false;
    BOOL                    bOleInitialize      = false;
    BOOL                    bInitPackage        = false;
    BOOL                    bInit3DEngine       = false;

    nRetCode = InitConfig(&bAllocConsole, &bPakFirst); 
    KGLOG_PROCESS_ERROR(nRetCode);

    if (bAllocConsole)
        bInitConsole = AllocConsole();

	SetMouseHoverTime(200);

    hr = OleInitialize(NULL);
    KGLOG_COM_PROCESS_ERROR(hr);
    bOleInitialize = true;

    InitPackage(bPakFirst);
    bInitPackage = true;

    nRetCode = Init3DEngine();
    KGLOG_PROCESS_ERROR(nRetCode);
    bInit3DEngine = true;

    nResult = true;
Exit0:
    if (!nResult)
    {
        if (bInit3DEngine)
            UnInit3DEngine();

        if (bInitPackage)
            UnInitPackage();

        if (bOleInitialize)
            OleUninitialize();

        if (bInitConsole)
            FreeConsole();

        KGLogPrintf(KGLOG_ERR, "%s\n", INIT_GAME_ERROR);
    }
    return nResult;
}

void KSO3Client::OnExit()
{
    UnInit3DEngine();

    UnInitPackage();

    OleUninitialize();

    FreeConsole();
    UnInitConfig();
}

int KSO3Client::OnLoop()
{
    RedrawWindow(KGWin32App::GetHWND(), NULL, NULL, RDW_INTERNALPAINT | RDW_NOERASE);
	
    return TRUE;
}

void KSO3Client::OnPaint()
{
    static unsigned s_uCount  = 0;
    unsigned uStopCount          = (unsigned)m_vecSFXList.size();
    if (uStopCount && s_uCount == uStopCount)
    {
        //::MessageBox(NULL, "检查结束……", "提示", MB_OK);
        ::PostQuitMessage(0);
        ++s_uCount;
    }
    if (s_uCount < uStopCount && m_p3DEngine)
    {
        SfxFrameMoveAndRender(/*"D:\\Kingsoft\\Game\\sword3-products\\trunk\\client\\data\\source\\other\\特效\\技能\\SFX\\释放\\施放_青色气圈01.Sfx"*/
            m_vecSFXList[s_uCount].c_str());
        ++s_uCount;
    }   
}

LRESULT	KSO3Client::OnMsgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return false;
}

int KSO3Client::InitConfig(BOOL *pbAllocConsole, BOOL *pbPakFirst)
{
    int                     nResult     = false;
    IIniFile*               pIni        = NULL;
    KSO3ClientFilePath*     pFilePath   = NULL;

    pFilePath = GetFilePath();
    KGLOG_PROCESS_ERROR(pFilePath);
    pIni = g_OpenIniFile(pFilePath->szConfig);
    KGLOG_PROCESS_ERROR(pIni);
  
    pIni->GetInteger("Debug", "Console", 0, pbAllocConsole);
    pIni->GetInteger("Debug", "PakFirst", *pbPakFirst, pbPakFirst);
    pIni->GetInteger("ENGINEOPTION", "UseLogical", TRUE, &m_bUseLogicalSave);

    SAFE_RELEASE(pIni);

    pIni = g_OpenIniFile(pFilePath->szConfig, false, true);
    KGLOG_PROCESS_ERROR(pIni);
         
    pIni->WriteInteger("ENGINEOPTION", "UseLogical", FALSE);
    pIni->Save(pFilePath->szConfig);
    
Exit1:
    nResult = true;
Exit0:
    SAFE_RELEASE(pIni);
    return nResult;
}

int KSO3Client::UnInitConfig()
{
    int                     nResult             = false;
    IIniFile*               pIni                = NULL;
    KSO3ClientFilePath*     pFilePath   = NULL;

    pFilePath = GetFilePath();
    KGLOG_PROCESS_ERROR(pFilePath);

    pIni = g_OpenIniFile(pFilePath->szConfig, false, true);
    KGLOG_PROCESS_ERROR(pIni);

    pIni->WriteInteger("ENGINEOPTION", "UseLogical", m_bUseLogicalSave);
    pIni->Save(pFilePath->szConfig);

    nResult = true;
Exit0:
    SAFE_RELEASE(pIni);
    return nResult;
}

BOOL KSO3Client::Init3DEngine()
{
    typedef HRESULT (*fnGet3DEngineInterface)(void**);

    int nResult                                     = false;
    int nRetCode                                    = false;
    HRESULT hr                                      = E_FAIL;
    fnGet3DEngineInterface pfnGet3DEngineInterface  = NULL;
    IKG3DUI* p3DUI                                  = NULL;
    HWND hWnd                                       = NULL;
    BOOL bInitModule3DEngine                        = FALSE;
    BOOL bInit3DEngine                              = FALSE;
    BOOL bOpenFile                                  = FALSE;
    BOOL bInitLostDeviceFunc                        = FALSE;
    BOOL bInitResetDeviceFunc                       = FALSE;
    char szDirPath[MAX_PATH]                        = "";

    hWnd = KGWin32App::GetHWND();
    KGLOG_PROCESS_ERROR(hWnd);

#ifdef _DEBUG
    nRetCode = m_Module3DEngine.Init("KG3DEngineD.dll");
    KGLOG_PROCESS_ERROR(nRetCode);
#else
    nRetCode = m_Module3DEngine.Init("KG3DEngine.dll");
    KGLOG_PROCESS_ERROR(nRetCode);
#endif

    bInitModule3DEngine = TRUE;

    pfnGet3DEngineInterface = (fnGet3DEngineInterface)GetProcAddress(m_Module3DEngine.GetDllHandle(), "Get3DEngineInterface");
    KGLOG_PROCESS_ERROR(pfnGet3DEngineInterface);

    hr = pfnGet3DEngineInterface((void**)&m_p3DEngine);
    KGLOG_COM_PROCESS_ERROR(hr);
    KGLOG_PROCESS_ERROR(m_p3DEngine);

    p3DUI = m_p3DEngine->Get3DUI();
    KGLOG_PROCESS_ERROR(p3DUI);

    hr = p3DUI->SetLostDeviceFunc(OnLostDevice);
    KGLOG_COM_PROCESS_ERROR(hr);
    bInitLostDeviceFunc = TRUE;

    hr = p3DUI->SetResetDeviceFunc(OnResetDevice);
    KGLOG_COM_PROCESS_ERROR(hr);
    bInitResetDeviceFunc = TRUE;

    hr = m_p3DEngine->Init(0, KG3DENG_DESIGN, hWnd, hWnd);
    KGLOG_COM_PROCESS_ERROR(hr);
    bInit3DEngine = TRUE;

    //hr = p3DUI->OnCreateDevice();
    //KGLOG_COM_PROCESS_ERROR(hr);

    nRetCode = GetCurrentDirectory(MAX_PATH, szDirPath);
    KG_PROCESS_ERROR(nRetCode > 0 && nRetCode < MAX_PATH + 1);

    strcat(szDirPath, s_FilePath);
    szDirPath[MAX_PATH - 1] = '\0';

    GetFilterFileList();

    nRetCode = GetFileList(szDirPath, /*&m_vecSFXList, */EXT_SFX);
    KGLOG_PROCESS_ERROR(nRetCode);

    m_pFile = g_OpenIniFile(s_szErrorFileName, false, true);
    KGLOG_PROCESS_ERROR(m_pFile);

    bOpenFile = TRUE;

    LPDIRECT3DDEVICE9 pDevice = m_p3DEngine->GetDevice();
    int k = pDevice->AddRef();
    k = pDevice->Release();

    nResult = true;
Exit0:
    if (!nResult)
    {
        if (bOpenFile)
        {
            KG_COM_RELEASE(m_pFile);
        }

        if (bInit3DEngine)
        {
            m_p3DEngine->UnInit();
            m_p3DEngine = NULL;
        }
        if (bInitResetDeviceFunc)
        {
            p3DUI->SetResetDeviceFunc(NULL);
        }
        if (bInitLostDeviceFunc)
        {
            p3DUI->SetLostDeviceFunc(NULL);
        }
        if (bInitModule3DEngine)
        {
            m_Module3DEngine.UnInit();
        }
    }
    return nResult;

}

void KSO3Client::UnInit3DEngine()
{
    IKG3DUI* p3DUI  = NULL;
    
    KG_COM_RELEASE(m_pFile);

    if (m_pScene)
    {
        m_p3DEngine->RemoveScene(m_pScene);
        m_pScene = NULL;
    }

    p3DUI = m_p3DEngine->Get3DUI();
    if (p3DUI)
    {
        p3DUI->Exit();
    }

    if (m_p3DEngine)
    {
        m_p3DEngine->UnInit();
        m_p3DEngine = NULL;
    }

    m_Module3DEngine.UnInit();
}


void KSO3Client::InitPackage(BOOL bPakFirst)
{
    g_SetFindFileMode(bPakFirst);

    if (g_IsFileExist("\\package.ini"))
    {
        g_LoadPackageFiles("\\package.ini", "SO3Client");
    }
}


void KSO3Client::UnInitPackage()
{
    g_ClearPackageFiles();
}

int KSO3Client::PushValidFile(const char *pcszFileName, TYPE_EXT ExtType)
{
    int nRetCode                = false;
    int nResult                 = false;
    int nLength                 = 0; 
    char szExt[MAX_PATH]        = "";
    const char *pcszString             = NULL;
    set<string>::iterator it            = m_setFilterFileList.begin();

    while (it != m_setFilterFileList.end())
    { 
        const char *pcszFind = NULL;
        pcszString = (*it).c_str();
        pcszFind = strstr(pcszFileName, pcszString);
        if (pcszFind)
        {
            nRetCode = strlen(pcszFind) == strlen(pcszString);
            KG_PROCESS_SUCCESS(nRetCode);
        }
        ++it;
    }

    _splitpath_s(pcszFileName, 
        NULL, 
        0,
        NULL, 
        0, 
        NULL, 
        0, 
        szExt,
        MAX_PATH);

    nRetCode = stricmp(szExt, g_cszExtName[ExtType]);
    KG_PROCESS_ERROR(!nRetCode);

    nLength = (int)strlen(g_cszExtName[ExtType]);
    if (!szExt[nLength])
    {
        m_vecSFXList.push_back(pcszFileName);
    }
Exit1:
    nResult = true;
Exit0:
    return nResult;
};

int KSO3Client::GetFileList(const char *pcszDirPath, TYPE_EXT ExtType)
{
    int nRetCode    = false;
    int nResult     = false;
    int nLength     = 0;
    DWORD dwError   = 0;
    DWORD dwAttriID = 0;
    HANDLE hFind    = INVALID_HANDLE_VALUE;
    char szDirPath[MAX_PATH]        = "";
    char szFileName[MAX_PATH]       = "";
    char szErrorInfo[MAX_PATH]      = "";
    WIN32_FIND_DATA FindFileData;

    hFind = FindFirstFile(pcszDirPath, &FindFileData);
    if (hFind == INVALID_HANDLE_VALUE) 
    {
        dwError = GetLastError();
        sprintf_s (szErrorInfo, "Invalid file handle. Error is %u\n", dwError);
        KG_PROCESS_ERROR(false);
    }

    strncpy(szDirPath, pcszDirPath, MAX_PATH);
    nLength = (int)strlen(pcszDirPath);
    szDirPath[nLength - 3] = '\0';

    while (true)
    {
        if (strcmp(FindFileData.cFileName, ".") && strcmp(FindFileData.cFileName, ".."))
        {
            strncpy(szFileName, szDirPath, MAX_PATH);
            strcat(szFileName, FindFileData.cFileName);
            dwAttriID = GetFileAttributes(szFileName);
            if (dwAttriID == FILE_ATTRIBUTE_DIRECTORY)
            {
                strcat(szFileName, "\\*.*");
                GetFileList(szFileName, ExtType);
            }
            else
            {
                PushValidFile(szFileName, ExtType);
            }
        }
        nRetCode = FindNextFile(hFind, &FindFileData);
        if (nRetCode)
        {
            continue;
        }

        dwError = GetLastError();
        if (dwError != ERROR_NO_MORE_FILES) 
        {
            sprintf_s (szErrorInfo, "FindNextFile error. Error is %u\n", dwError);
            KG_PROCESS_ERROR(false);
        }
        break;

    }

    nResult = true;
Exit0:
    if (hFind != INVALID_HANDLE_VALUE)
    {
        FindClose(hFind);
        hFind = INVALID_HANDLE_VALUE;
    }

    return nResult;
};

HRESULT KSO3Client::SfxFrameMoveAndRender(LPCTSTR pcszFileName)
{
    HRESULT hrResult                    = E_FAIL;
    HRESULT hrRetCode                   = E_FAIL;
    int nLoopCount                      = 3000;
    unsigned uTotalCount                = 0;
    char szDirPath[MAX_PATH]            = "";
    char szKeyName[32]                  = "";
    IEKG3DScene *pScene                 = NULL;
    IEKG3DSceneSFXEditor *pSFXScene     = NULL;
    IEKG3DSFX *pSFX                     = NULL;
    static int s_nKey                   = 0;
    static int s_nCount                 = 0;
    static FLOAT s_fPreFPS              = 0;

    hrRetCode = m_p3DEngine->NewOneScene(SCENETYPE_SFXEDITOR, &m_pScene);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    hrRetCode = m_pScene->AddOutputWindow("Login Scene", KGWin32App::GetHWND(), 0, 0, &m_nOutputWindowID);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    pScene =(IEKG3DScene*)(m_pScene);
    pScene->GetSceneSFXEditor(&pSFXScene);
    KGLOG_PROCESS_ERROR(pSFXScene);

    GetVideoMemeryInfo();


    hrRetCode = pSFXScene->OpenSFX(pcszFileName);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    KGLogPrintf(KGLOG_INFO,"SFX: %s",pcszFileName);

    pSFXScene->GetSFX(&pSFX);
    pSFX->PlayAnimation(SFX_AP_LOOP, 1.f, 0);


    for (int i = 0; i < nLoopCount; i++)
    {
        m_p3DEngine->FrameMove();
        m_p3DEngine->Render();

        ++s_nCount;
        float fFPS = pSFXScene->GetFps();
        if (s_fPreFPS != fFPS)
        {        
            KGLogPrintf(KGLOG_INFO,"FPS : %.2f Count %d", fFPS, s_nCount);
            s_fPreFPS = fFPS;
        }
    }
    s_nCount = 0;

    GetVideoMemeryInfo();
    KGLogPrintf(KGLOG_INFO,"Render SFX Over");
    if (m_pScene)
    {
        m_p3DEngine->RemoveScene(m_pScene);
        m_pScene = NULL;
    }   

        /*TryRender();
        GetVideoMemeryInfo();
        if (m_pScene)
        {
            m_p3DEngine->RemoveScene(m_pScene);
            m_pScene = NULL;

            }*/

    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult) && m_pFile)
    {
        ++s_nKey;
        itoa(s_nKey, szKeyName, 10);
        m_pFile->WriteString("SFX Render Exception",szKeyName, pcszFileName);
        m_pFile->Save(s_szErrorFileName);
    }

    if (m_pScene)
    {
        m_p3DEngine->RemoveScene(m_pScene);
        m_pScene = NULL;
    }

    return hrResult;
};

HRESULT KSO3Client::GetVideoMemeryInfo()
{
    HRESULT hrResult                    = E_FAIL;
    HRESULT hrRetCode                   = E_FAIL;
    IKG3DUI* p3DUI                      = NULL;
    float fLocalTotal                   = 0.f;
    float fLocalFree                    = 0.f;
    float fNonLocalTotal                = 0.f;
    float fNonLocalFree               = 0.f;
    float fTextureTotal                 = 0.f;
    float fTextureFree                  = 0.f;
    float fD3DTotal                     = 0.f;
    float fD3DFree                      = 0.f;
    static float s_fUnit                = 1024.f * 1024.f;
    VideoMemoryInfo Info;

    p3DUI = m_p3DEngine->Get3DUI();
    KGLOG_PROCESS_ERROR(p3DUI);

    Info = p3DUI->GetVideoMemeryInfo();

    fLocalTotal = Info.dwLocalTatal / s_fUnit;
    fLocalFree = Info.dwLocalFree / s_fUnit;
    fNonLocalTotal = Info.dwNonLocalTatal / s_fUnit;
    fNonLocalFree = Info.dwNonLocalFree / s_fUnit;
    fTextureTotal = Info.dwTextureTatal / s_fUnit;
    fTextureFree = Info.dwTexturnFree / s_fUnit;
    fD3DTotal = Info.dwD3DTotal / s_fUnit;
    fD3DFree = Info.dwD3DFree/ s_fUnit;

    KGLogPrintf(KGLOG_DEBUG,"Local Video: Total %.2fM  Free %.2fM", fLocalTotal, fLocalFree);
    KGLogPrintf(KGLOG_DEBUG,"NonLocal: Total %.2fM  Free %.2fM", fNonLocalTotal, fNonLocalFree);
    KGLogPrintf(KGLOG_DEBUG,"3D Texture: Total %.2fM  Free %.2fM", fTextureTotal, fTextureFree);
    KGLogPrintf(KGLOG_DEBUG,"3D Texture: Total %.2fM  Free %.2fM", fD3DTotal, fD3DFree);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

void KSO3Client::TryRender()
{
    int nLoopCount = 3000;
    __try
    {
        for (int i = 0; i < nLoopCount; i++)
        {
            m_p3DEngine->FrameMove();
            m_p3DEngine->Render();
        }
    }
    __except(1)
    {
        if (m_pScene)
        {
            m_p3DEngine->RemoveScene(m_pScene);
            m_pScene = NULL;
        }
    }    
}

void KSO3Client::GetFilterFileList()
{
    int nRetCode            = false;
    int nHeight             = 0;
    ITabFile* pFile         = NULL;
    char szFile[MAX_PATH]   = "";

    pFile = g_OpenTabFile("FilterFile.tab");
    KG_PROCESS_ERROR(pFile);

    nHeight = pFile->GetHeight();
    for (int nLine = 2; nLine <= nHeight; ++nLine)
    {
        nRetCode =  nRetCode = pFile->GetString(nLine, "FileName", "", szFile, sizeof(szFile));
        KGLOG_PROCESS_ERROR(nRetCode > 0);
        m_setFilterFileList.insert(szFile);
    }

Exit0:
    KG_COM_RELEASE(pFile);
    return;
}

void KSO3Client::MemoryLeakDetect()
{
#ifdef _SO3_MEMORY_LEAK_DETECT_
    DWORD dwTickCount = KG_GetTickCount();
    if (dwTickCount > g_dwLastMemoryCheckTickCount + 10000)
    {        
        long lAllocCounterBackBuffer[KALLOC_RECORD_COUNT];

        for (int i = 0; i < KALLOC_RECORD_COUNT; ++i)
            lAllocCounterBackBuffer[i] = g_lAllocCounter[i] - g_lAllocCounterBackBuffer[i];

        memcpy(g_lAllocCounterBackBuffer, g_lAllocCounter, sizeof(g_lAllocCounterBackBuffer));

        cprintf("------------------ Total %d M ------------------------\n", g_lTotalAlloc / 1024 / 1024);

        for (int i = 0; i < KALLOC_RECORD_COUNT; i++)
        {
            int nDeltaBlock = lAllocCounterBackBuffer[i];

            if (nDeltaBlock > 4)
            {
                cprintf("size(%d) --> %d\n", i, nDeltaBlock);
            }
        }

        cprintf("\n");

        g_dwLastMemoryCheckTickCount = dwTickCount;
    }
#endif // #ifdef _SO3_MEMORY_LEAK_DETECT_
}

