#include "StdAfx.h"
#include "KSO3Client.h"
#include "./ClientString.h"
#include "IEEditor.h"
//#include "IEKG3DSceneSFXEditor.h"
//#include "IEKG3DSFX.h"
#include "KG3DFileLoader.h"

static char s_FilePath[] = "\\data\\source\\*.*"/*"\\data\\source\\other\\特效\\技能\\SFX\\释放\\*.*"*/;

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

void KSO3Client::OnTest()
{
    int nRetCode                = false;
    HRESULT hrRetCode           = false;
    static unsigned s_uCount    = 0;
    unsigned uStopCount         = (unsigned)m_vecMeshList.size();

    if (uStopCount && s_uCount == uStopCount)
    {
        //::MessageBox(NULL, "检查结束……", "提示", MB_OK);
        ::PostQuitMessage(0);
        ++s_uCount;
    }
    if (s_uCount < uStopCount && m_p3DEngine)
    {
        ++s_uCount;
        hrRetCode = TestMeshFile(m_vecMeshList[s_uCount - 1].c_str());
        KGLOG_COM_PROCESS_ERROR(hrRetCode);
        hrRetCode = GetMeshData(m_vecMeshList[s_uCount - 1].c_str());
        KGLOG_COM_PROCESS_ERROR(hrRetCode);
        nRetCode = PrintMeshData(m_vecMeshList[s_uCount - 1].c_str());
        KGLOG_PROCESS_ERROR(nRetCode);
    }   
Exit0:
    return;
}

LRESULT	KSO3Client::OnMsgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return false;
}

int KSO3Client::InitConfig(BOOL *pbAllocConsole, BOOL *pbPakFirst)
{
    int                     nResult = false;
    IIniFile*               pIni    = NULL;
    ITabFile *pTabFile              = NULL;
    char szFileName[MAX_PATH]       = "";

    pIni = g_OpenIniFile("\\config.ini");
    KGLOG_PROCESS_ERROR(pIni);
  
    pIni->GetInteger("Debug", "Console", 0, pbAllocConsole);
    pIni->GetInteger("Debug", "PakFirst", *pbPakFirst, pbPakFirst);
    pIni->GetInteger("ENGINEOPTION", "UseLogical", TRUE, &m_bUseLogicalSave);

    SAFE_RELEASE(pIni);

    pIni = g_OpenIniFile("\\config.ini", false, true);
    KGLOG_PROCESS_ERROR(pIni);
         
    pIni->WriteInteger("ENGINEOPTION", "UseLogical", FALSE);
    pIni->Save("\\config.ini");

	extern char g_szMessDataTab[];
    snprintf(szFileName, sizeof(szFileName), "%s", g_szMessDataTab);
    szFileName[sizeof(szFileName) - 1] = '\0';

    if (g_IsFileExist(szFileName))
    {
        KG_PROCESS_SUCCESS(TRUE);
    }

    pTabFile = g_CreateTabFile();
    KG_PROCESS_ERROR(pTabFile);

    pTabFile->InsertNewCol("MeshName");
    pTabFile->InsertNewCol("NumVertices");
    pTabFile->InsertNewCol("NumFaces");
    pTabFile->InsertNewCol("NumSubset");
    pTabFile->InsertNewCol("NumBone");

    pTabFile->Save(szFileName);
    SAFE_RELEASE(pTabFile);
    
Exit1:
    nResult = true;
Exit0:
    SAFE_RELEASE(pTabFile);
    SAFE_RELEASE(pIni);
    return nResult;
}

int KSO3Client::UnInitConfig()
{
    int                     nResult             = false;
    IIniFile*               pIni                = NULL;

    pIni = g_OpenIniFile("\\config.ini", false, true);
    KGLOG_PROCESS_ERROR(pIni);

    pIni->WriteInteger("ENGINEOPTION", "UseLogical", m_bUseLogicalSave);
    pIni->Save("\\config.ini");

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

    nRetCode = GetFileList(szDirPath, EXT_MESH);
    KGLOG_PROCESS_ERROR(nRetCode);


    LPDIRECT3DDEVICE9 pDevice = m_p3DEngine->GetDevice();
    int k = pDevice->AddRef();
    k = pDevice->Release();

    nResult = true;
Exit0:
    if (!nResult)
    {
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
        m_vecMeshList.push_back(pcszFileName);
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
            if (strstr(szFileName, "data\\source\\maps"))
            {
                NULL;
            }
            else if (dwAttriID == FILE_ATTRIBUTE_DIRECTORY)
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

int KSO3Client::PrintMeshData(LPCTSTR pcszFileName)
{
    int nResult     = false;
    int nRetCode    = false;
    static int s_nRowIndex        = 2;
    ITabFile *pTabFile          = NULL;
    char szFileName[MAX_PATH]   = "";

    snprintf(szFileName, sizeof(szFileName), "%s", MESH_TEST_OUT_FILE);
    szFileName[sizeof(szFileName) - 1] = '\0';

    pTabFile = g_OpenTabFile(szFileName, false, true);
    KGLOG_PROCESS_ERROR(pTabFile);

    pcszFileName = strstr(pcszFileName, "data\\");
   
    nRetCode = pTabFile->WriteString(s_nRowIndex, "MeshName", pcszFileName);
    KGLOG_PROCESS_ERROR(nRetCode);
    nRetCode = pTabFile->WriteInteger(s_nRowIndex, "NumVertices", m_MeshData.MeshHead.dwNumVertices);
    KGLOG_PROCESS_ERROR(nRetCode);
    nRetCode = pTabFile->WriteInteger(s_nRowIndex, "NumFaces", m_MeshData.MeshHead.dwNumFaces);
    KGLOG_PROCESS_ERROR(nRetCode);
    nRetCode = pTabFile->WriteInteger(s_nRowIndex, "NumSubset", m_MeshData.MeshHead.dwNumSubset);
    KGLOG_PROCESS_ERROR(nRetCode);
    nRetCode = pTabFile->WriteInteger(s_nRowIndex, "NumBone", m_MeshData.nNumBone);
    KGLOG_PROCESS_ERROR(nRetCode);

    pTabFile->Save(szFileName);

    ++s_nRowIndex;


    nResult = true;
Exit0:
    KG_COM_RELEASE(pTabFile);
    return nResult;
}

HRESULT KSO3Client::TestMeshFile(LPCTSTR pcszFileName)
{
    HRESULT hrResult    = E_FAIL;
    HRESULT hrRetCode   = E_FAIL;
    int nRetCode        = false;
    IKG_Buffer *piMeshFile = NULL;

    KG_PROCESS_ERROR(pcszFileName);

    ZeroMemory(&m_MeshData, sizeof(m_MeshData));

    nRetCode = g_FileLoader.LoadImmediately(pcszFileName, &piMeshFile);
    KGLOG_PROCESS_ERROR(nRetCode && "File not exist");

    hrRetCode = LoadMeshHeadData(piMeshFile);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    hrResult = S_OK;
Exit0:
    KG_COM_RELEASE(piMeshFile);
    if (FAILED(hrResult) && pcszFileName)
    {
        KGLogPrintf(KGLOG_ERR, "Load file %s failed.", pcszFileName);
    }
    return hrResult;
};

HRESULT KSO3Client::LoadMeshHeadData(IKG_Buffer *piMeshFile)
{
    HRESULT hrRetCode = E_FAIL;
    HRESULT hrResult = E_FAIL;
    int nRetCode = false;
    _MeshFileHead *pFileHeader = NULL;
    _MeshHead *pMeshHead = NULL;
    KG3DBufferReader BufferReader;

    ASSERT(piMeshFile);

    nRetCode = BufferReader.SetBuffer(piMeshFile);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = BufferReader.ReferenceRead((void **)&pFileHeader, sizeof(_MeshFileHead));
    KGLOG_PROCESS_ERROR(nRetCode);

    KGLOG_PROCESS_ERROR(pFileHeader->dwFileMask == 0x4D455348);
    KGLOG_PROCESS_ERROR(pFileHeader->dwVersion == 0);

    nRetCode = BufferReader.ReferenceRead((void **)&pMeshHead, sizeof(_MeshHead));
    KGLOG_PROCESS_ERROR(nRetCode);

    m_MeshData.MeshHead.dwNumVertices = pMeshHead->dwNumVertices;
    m_MeshData.MeshHead.dwNumFaces = pMeshHead->dwNumFaces;
    m_MeshData.MeshHead.dwNumSubset = pMeshHead->dwNumSubset;

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KSO3Client::GetMeshData(LPCTSTR pcszFileName)
{
    HRESULT hrResult    = E_FAIL;
    HRESULT hrRetCode   = E_FAIL;
    int nRetCode        = false;
    char szMessage[MAX_PATH]            = "";
    char szKeyName[32]                  = "";
    static unsigned uFailedCount        = 0;
    IIniFile *pFile                     = NULL;
    IEKG3DModel *piModel                 = NULL;
    IKG3DResourceManager* pModelTable   = NULL;

    KG_PROCESS_ERROR(pcszFileName);

    pModelTable = m_p3DEngine->Get3DModelTable();
    
    snprintf(
        szMessage, sizeof(szMessage), 
        "Mesh模型加载检查文件：%s", 
        pcszFileName
        );
    szMessage[sizeof(szMessage) - 1] = '\0';
    SetWindowText(GetHWND(), szMessage);

    pFile = g_OpenIniFile(g_cszErrorFileName[EXT_MESH],false,true);
    KGLOG_PROCESS_ERROR(pFile);

    hrRetCode = pModelTable->LoadResourceFromFile(pcszFileName, 0, 0, (IKG3DResourceBase **)&piModel);
    if (FAILED(hrRetCode))
    {
        itoa(uFailedCount, szKeyName, 10);
        pFile->WriteString("ErrorMesh",szKeyName, pcszFileName);
        ++uFailedCount;
    }
    pFile->WriteInteger("ErrorMesh", "ErrorNum", uFailedCount);
    pFile->Save(g_cszErrorFileName[EXT_MESH]);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    hrRetCode = piModel->GetNumBones(&(m_MeshData.nNumBone));  
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    hrResult = S_OK;
Exit0:
    KG_COM_RELEASE(piModel);
    m_p3DEngine->FrameMove();
    KG_COM_RELEASE(pFile);
    if (FAILED(hrResult) && pcszFileName)
    {
        KGLogPrintf(KGLOG_ERR, "Load file %s failed.", pcszFileName);
    }
    return hrResult;
};

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

