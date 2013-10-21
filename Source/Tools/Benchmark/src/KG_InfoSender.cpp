#include "stdafx.h"
#include <DbgHelp.h>
#include <string>
#include <tchar.h>
#ifndef INITGUID
#define INITGUID
#include <dxdiag.h>
#undef INITGUID
#else
#include <dxdiag.h>
#endif
#include <comdef.h>
#include <Wbemidl.h>

#include "KG_InfoSender.h"
#include "KG_HttpClient.h"
#include "KG_ContactDialog.h"
#include "Engine/md5.h"
#include "Engine/KG_MachineID.h"

# pragma comment(lib, "wbemuuid.lib")

#define CURRENT_SYSTEM_INFO_VERSION     "2"

//////////////////////////////////////////////////////////////////////////
TCHAR g_szHost[PATH_MAX];
int   g_nPort = INTERNET_DEFAULT_HTTP_PORT;
TCHAR g_szReceiveInfoPage[PATH_MAX];
TCHAR g_szReceiveFilePage[PATH_MAX];
TCHAR g_szSearchPage[PATH_MAX];
TCHAR g_szVersionPage[PATH_MAX];
int   g_nShowContactDiag = 1;
int   g_nVersion = 0;
//int   g_nHadContactInfo = false;
//////////////////////////////////////////////////////////////////////////
class KG_SystemInfoCollector
{
public:
    struct KG_SYSTEM_INFO
    {
        char szOS[16];
        char szOSPack[128];
        char szProcessor[128];
        char szSystemMemory[16];
        char szDXVersion[64];
        char szVGAChip[128];
        char szVGAMemory[16];
        char szVGADriverVersion[32];
        char szDisplayRatio[16];
        char szContactInfo[256];
    };

public:
    KG_SystemInfoCollector(void);
    ~KG_SystemInfoCollector(void);

    int QuerySystemInfo(KG_SYSTEM_INFO *pSystemInfo);

private:
    int  InitWbemService();
    void UnintWbemService();

    HRESULT GetEnumerator(IEnumWbemClassObject **ppEnumerator, char szEnumName[128]);

    int GetDXVersion(char szDXVersion[64]);

    BOOL m_bWbemServiceComInit;
    IWbemLocator *m_pWbemLocator;
    IWbemServices *m_pWbemServices;
};

//////////////////////////////////////////////////////////////////////////
KG_SystemInfoCollector::KG_SystemInfoCollector(void)
{
    m_bWbemServiceComInit = false;
    m_pWbemLocator = NULL;
    m_pWbemServices = NULL;
}

KG_SystemInfoCollector::~KG_SystemInfoCollector(void)
{
    ASSERT(!m_bWbemServiceComInit);
    ASSERT(!m_pWbemLocator);
    ASSERT(!m_pWbemServices);
}

int KG_SystemInfoCollector::QuerySystemInfo(KG_SYSTEM_INFO *pSystemInfo)
{
    int                 nResult          = false;
    int                 nRetCode         = false;
    HRESULT             hrRetCode        = S_FALSE;
    KG_SYSTEM_INFO      SystemInfo;
    BOOL                bWbemServiceInit = false;
    DWORD               dwMemorySize     = 0;
    DWORD               dwWidth          = 0;
    DWORD               dwHeight         = 0;
    OSVERSIONINFO       OSVersion;
    int                 nGetInfoFailed   = false;
    int                 nGetMemoryFailed = false;
    char                szProcessorName[64] = "n/a";
    LONG                lValue           = 0;
    IEnumWbemClassObject *pEnumerator = NULL;
    IWbemClassObject *pWbemClassObj = NULL;
    VARIANT vtProp;
    SYSTEM_INFO ProcessorInfo;

    KG_PROCESS_ERROR(pSystemInfo);

    nRetCode = InitWbemService();
    KGLOG_PROCESS_ERROR(nRetCode);

    bWbemServiceInit = true;

    ZeroMemory(&SystemInfo, sizeof(SystemInfo));

    // get operate system version
    ZeroMemory(&OSVersion, sizeof(OSVersion));
    OSVersion.dwOSVersionInfoSize = sizeof(OSVersion);

    nRetCode = ::GetVersionEx(&OSVersion);
    if (nRetCode)
    {
        nRetCode = snprintf(
            SystemInfo.szOS,
            sizeof(SystemInfo.szOS),
            "%lu%lu",
            OSVersion.dwMajorVersion, OSVersion.dwMinorVersion
        );
        KGLOG_PROCESS_ERROR(nRetCode > 0);

        nRetCode = snprintf(
            SystemInfo.szOSPack,
            sizeof(SystemInfo.szOSPack),
            "%s",
            OSVersion.szCSDVersion
        );
        KGLOG_PROCESS_ERROR(nRetCode >= 0);
    
        SystemInfo.szOSPack[sizeof(SystemInfo.szOSPack) - 1] = '\0';
    }
    else
    {
        nGetInfoFailed = true;
    }

    // get system physical memory size
    dwMemorySize = 0;
    nGetMemoryFailed = true;
    hrRetCode = GetEnumerator(&pEnumerator, "Win32_PhysicalMemory");
    while (SUCCEEDED(hrRetCode))
    {
        IWbemClassObject *pObject[32] = {0};
        char szMemorySize[32];
        hrRetCode = pEnumerator->Next(WBEM_INFINITE, 32, pObject, (ULONG *)&nRetCode);
        if (FAILED(hrRetCode))
        {
            nGetMemoryFailed = true;
            break;
        }

        nGetMemoryFailed = false;

        for (int i = 0; i < nRetCode; ++i)
        {
            VariantInit(&vtProp);
            hrRetCode = pObject[i]->Get(L"Capacity", 0, &vtProp, 0, 0);
            if (FAILED(hrRetCode))
            {
                nGetMemoryFailed = true;
                break;
            }

            wcstombs(szMemorySize, vtProp.bstrVal, sizeof(szMemorySize));
            szMemorySize[sizeof(szMemorySize) - 1] = '\0';
            
            VariantClear(&vtProp);

            dwMemorySize += atoi(szMemorySize) / 1024 / 1024;
        }

        for (int j = 0; j < nRetCode; ++j)
        {
            SAFE_RELEASE(pObject[j]);
        }
 
        break;
    }

    SAFE_RELEASE(pEnumerator);

    if (nGetMemoryFailed || (dwMemorySize == 0))
    {
        MEMORYSTATUS MemoryStatus;

        ZeroMemory(&MemoryStatus, sizeof(MemoryStatus));
        ::GlobalMemoryStatus(&MemoryStatus);
        dwMemorySize = (DWORD)(MemoryStatus.dwTotalPhys / 1024 / 1024);
    }

    nRetCode = snprintf(
        SystemInfo.szSystemMemory,
        sizeof(SystemInfo.szSystemMemory),
        "%d",
        dwMemorySize
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0);

    SystemInfo.szSystemMemory[sizeof(SystemInfo.szSystemMemory) - 1] = '\0';    

    // enum processor
    lValue = 0;
    hrRetCode = GetEnumerator(&pEnumerator, "Win32_Processor");
    while (SUCCEEDED(hrRetCode))
    {
        hrRetCode = pEnumerator->Next(WBEM_INFINITE, 1, &pWbemClassObj, (ULONG *)&nRetCode);
        if (FAILED(hrRetCode))
        {
            nGetInfoFailed = true;
            break;
        }

        // get processor name
        VariantInit(&vtProp);
        hrRetCode = pWbemClassObj->Get(L"Name", 0, &vtProp, 0, 0);
        if (FAILED(hrRetCode))
        {
            nGetInfoFailed = true;
            SAFE_RELEASE(pWbemClassObj);
            break;
        }

        wcstombs(szProcessorName, vtProp.bstrVal, sizeof(szProcessorName));
        szProcessorName[sizeof(szProcessorName) - 1] = '\0';

        VariantClear(&vtProp);

        ZeroMemory(&ProcessorInfo, sizeof(ProcessorInfo));    
        GetSystemInfo(&ProcessorInfo);

        lValue = ProcessorInfo.dwNumberOfProcessors;

        nRetCode = snprintf(
            szProcessorName, 
            sizeof(szProcessorName), 
            "%s %d CPU(s)", 
            szProcessorName, lValue
        );
        KGLOG_PROCESS_ERROR(nRetCode > 0);

        szProcessorName[sizeof(szProcessorName) - 1] = '\0';

        SAFE_RELEASE(pWbemClassObj);    

        break;
    }

    SAFE_RELEASE(pEnumerator);

    strncpy(SystemInfo.szProcessor, szProcessorName, sizeof(SystemInfo.szProcessor));
    SystemInfo.szProcessor[sizeof(SystemInfo.szProcessor) - 1] = '\0';
    
    // enum video graphic card
    hrRetCode = GetEnumerator(&pEnumerator, "Win32_VideoController");
    while (SUCCEEDED(hrRetCode))
    {
        hrRetCode = pEnumerator->Next(WBEM_INFINITE, 1, &pWbemClassObj, (ULONG *)&nRetCode);
        if (FAILED(hrRetCode))
        {
            nGetInfoFailed = true;
            break;
        }

        // get vga chip
        VariantInit(&vtProp);
        hrRetCode = pWbemClassObj->Get(L"Name", 0, &vtProp, 0, 0);
        if (SUCCEEDED(hrRetCode))
        {
            wcstombs(SystemInfo.szVGAChip, vtProp.bstrVal, sizeof(SystemInfo.szVGAChip));   
            SystemInfo.szVGAChip[sizeof(SystemInfo.szVGAChip) - 1] = '\0';
        }
        else
        {
            nGetInfoFailed = true;
            strncpy(SystemInfo.szVGAChip, "n/a", sizeof(SystemInfo.szVGAChip));
            SystemInfo.szVGAChip[sizeof(SystemInfo.szVGAChip) - 1] = '\0';
        }
        VariantClear(&vtProp);

        // get vga memory
        lValue = 0;
        VariantInit(&vtProp);
        hrRetCode = pWbemClassObj->Get(L"AdapterRAM", 0, &vtProp, 0, 0);
        if (SUCCEEDED(hrRetCode))
        {
            lValue = vtProp.lVal;
        }
        else
        {
            lValue = 0;
            nGetInfoFailed = true;
        }

        nRetCode = snprintf(SystemInfo.szVGAMemory, sizeof(SystemInfo.szVGAMemory), "%d", lValue / 1024 / 1024);
        KGLOG_PROCESS_ERROR(nRetCode > 0);
        SystemInfo.szVGAMemory[sizeof(SystemInfo.szVGAMemory) - 1] = '\0';
        
        VariantClear(&vtProp);

        // get vga driver version
        VariantInit(&vtProp);
        hrRetCode = pWbemClassObj->Get(L"DriverVersion", 0, &vtProp, 0, 0);
        if (SUCCEEDED(hrRetCode))
        {
            wcstombs(SystemInfo.szVGADriverVersion, vtProp.bstrVal, sizeof(SystemInfo.szVGADriverVersion));   
            SystemInfo.szVGADriverVersion[sizeof(SystemInfo.szVGADriverVersion) - 1] = '\0';
        }
        else
        {
            nGetInfoFailed = true;
            strncpy(SystemInfo.szVGADriverVersion, "n/a", sizeof(SystemInfo.szVGADriverVersion));
            SystemInfo.szVGADriverVersion[sizeof(SystemInfo.szVGADriverVersion) - 1] = '\0';
        }

        VariantClear(&vtProp);

        SAFE_RELEASE(pWbemClassObj);

        break;
    }
    SAFE_RELEASE(pEnumerator);

    // get display ratio
    hrRetCode = GetEnumerator(&pEnumerator, "Win32_DisplayConfiguration");
    while (SUCCEEDED(hrRetCode))
    {
        BOOL bOperationSuccess = true;
     
        hrRetCode = pEnumerator->Next(WBEM_INFINITE, 1, &pWbemClassObj, (ULONG *)&nRetCode);
        if (FAILED(hrRetCode))
        {
            nGetInfoFailed = true;
            break;
        }

        VariantInit(&vtProp);
        hrRetCode = pWbemClassObj->Get(L"PelsWidth", 0, &vtProp, 0, 0);
        if (SUCCEEDED(hrRetCode))
        {
            dwWidth = vtProp.lVal;
            VariantClear(&vtProp);
        }
        else
        {
            nGetInfoFailed = true;
            bOperationSuccess = false;
        }

        VariantInit(&vtProp);
        hrRetCode = pWbemClassObj->Get(L"PelsHeight", 0, &vtProp, 0, 0);
        if (SUCCEEDED(hrRetCode))
        {
            dwHeight = vtProp.lVal;
            VariantClear(&vtProp);
        }
        else
        {
            nGetInfoFailed = true;
            bOperationSuccess = false;
        }

        if (bOperationSuccess)
        {
            sprintf(SystemInfo.szDisplayRatio, "%u*%u", dwWidth, dwHeight);
            SystemInfo.szDisplayRatio[sizeof(SystemInfo.szDisplayRatio) - 1] = '\0';
        }
        else
        {
            strncpy(SystemInfo.szDisplayRatio, "n/a", sizeof(SystemInfo.szDisplayRatio));
            SystemInfo.szDisplayRatio[sizeof(SystemInfo.szDisplayRatio) - 1] = '\0';
        }

        SAFE_RELEASE(pWbemClassObj);

        break;
    }

    SAFE_RELEASE(pEnumerator);

    UnintWbemService();
    bWbemServiceInit = false;

    //
    nRetCode = GetDXVersion(SystemInfo.szDXVersion);
    if (!nRetCode)
    {
        nGetInfoFailed = true;
        strncpy(SystemInfo.szDXVersion, "n/a", sizeof(SystemInfo.szDXVersion));
    }

    *pSystemInfo = SystemInfo;

    nResult = true;
Exit0:
    VariantClear(&vtProp);
    SAFE_RELEASE(pWbemClassObj);
    SAFE_RELEASE(pEnumerator);

    if (g_nShowContactDiag && (!nResult || nGetInfoFailed))
    {
        nRetCode = ShowContactDialog(SystemInfo.szContactInfo, sizeof(SystemInfo.szContactInfo));
        SystemInfo.szContactInfo[sizeof(SystemInfo.szContactInfo) - 1] = '\0';

        strncpy(pSystemInfo->szContactInfo, SystemInfo.szContactInfo, sizeof(pSystemInfo->szContactInfo));
        pSystemInfo->szContactInfo[sizeof(pSystemInfo->szContactInfo) - 1] = '\0';
    }

    if (bWbemServiceInit)
    {
        UnintWbemService();
        bWbemServiceInit = false;
    }

    return nResult;
}

int KG_SystemInfoCollector::InitWbemService()
{
    int nResult = false;
    HRESULT hrRetCode = S_FALSE;

    ASSERT(!m_bWbemServiceComInit);

    hrRetCode =  ::CoInitializeEx(0, COINIT_MULTITHREADED);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    m_bWbemServiceComInit = true;

    hrRetCode = ::CoInitializeSecurity(
        NULL, 
        -1,                          // COM authentication
        NULL,                        // Authentication services
        NULL,                        // Reserved
        RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication 
        RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation  
        NULL,                        // Authentication info
        EOAC_NONE,                   // Additional capabilities 
        NULL                         // Reserved
    );
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    hrRetCode = ::CoCreateInstance(
        CLSID_WbemLocator,             
        0, 
        CLSCTX_INPROC_SERVER, 
        IID_IWbemLocator, 
        (LPVOID *)&m_pWbemLocator
    );
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    // Connect to the root\cimv2 namespace with
    // the current user and obtain pointer pSvc
    // to make IWbemServices calls.
    hrRetCode = m_pWbemLocator->ConnectServer(
         _bstr_t(L"ROOT\\CIMV2"), // Object path of WMI namespace
         NULL,                    // User name. NULL = current user
         NULL,                    // User password. NULL = current
         0,                       // Locale. NULL indicates current
         NULL,                    // Security flags.
         0,                       // Authority (e.g. Kerberos)
         0,                       // Context object 
         &m_pWbemServices           // pointer to IWbemServices proxy
    );
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    nResult = true;
Exit0:
    if (!nResult)
    {
        SAFE_RELEASE(m_pWbemServices);
        SAFE_RELEASE(m_pWbemLocator);

        if (m_bWbemServiceComInit)
        {
            ::CoUninitialize();
            m_bWbemServiceComInit = false;
        }
    }
    return nResult;
}

void KG_SystemInfoCollector::UnintWbemService()
{
    SAFE_RELEASE(m_pWbemServices);
    SAFE_RELEASE(m_pWbemLocator);

    if (m_bWbemServiceComInit)
    {
        ::CoUninitialize();
        m_bWbemServiceComInit = false;
    }
}

HRESULT KG_SystemInfoCollector::GetEnumerator(IEnumWbemClassObject **ppEnumerator, char szEnumName[128])
{
    HRESULT hrResult  = S_FALSE;
    HRESULT hrRetCode = S_FALSE;
    IEnumWbemClassObject *pEnumWbemClassObject = NULL;
    bstr_t strQueryStr;

    strQueryStr = "SELECT * FROM ";
    strQueryStr += szEnumName;

    KGLOG_PROCESS_ERROR(m_pWbemServices);

    hrRetCode = m_pWbemServices->ExecQuery(
        bstr_t("WQL"), 
        strQueryStr,
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, 
        NULL,
        &pEnumWbemClassObject
    );
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    *ppEnumerator = pEnumWbemClassObject;
    pEnumWbemClassObject = NULL;

    hrResult = S_OK;
Exit0:
    SAFE_RELEASE(pEnumWbemClassObject);

    return hrResult;
}

int KG_SystemInfoCollector::GetDXVersion(char szDXVersion[64])
{
    int nResult = false;
    int nRetCode = false;
    HRESULT hrRetCode = S_FALSE;
    DXDIAG_INIT_PARAMS dxDiagInitParam;
    IDxDiagProvider *pDxDiagProvider = NULL;
    IDxDiagContainer *pDxDiagRoot = NULL;
    IDxDiagContainer *pObject = NULL;
    char szTempDxVersion[64];
    int i = 0;
    BOOL bComInit = false;
    VARIANT var;

    hrRetCode =  CoInitialize(NULL);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    bComInit = true;
    
    hrRetCode = ::CoCreateInstance(
        CLSID_DxDiagProvider, 
        NULL, 
        CLSCTX_INPROC_SERVER, 
        IID_IDxDiagProvider, 
        (LPVOID *)&pDxDiagProvider
    );
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    ZeroMemory(&dxDiagInitParam, sizeof(DXDIAG_INIT_PARAMS));

    dxDiagInitParam.dwSize                  = sizeof(DXDIAG_INIT_PARAMS);
    dxDiagInitParam.dwDxDiagHeaderVersion   = DXDIAG_DX9_SDK_VERSION;
    dxDiagInitParam.bAllowWHQLChecks        = false;
    dxDiagInitParam.pReserved               = NULL;

    hrRetCode = pDxDiagProvider->Initialize(&dxDiagInitParam);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    hrRetCode = pDxDiagProvider->GetRootContainer(&pDxDiagRoot);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    hrRetCode = pDxDiagRoot->GetChildContainer(L"DxDiag_SystemInfo", &pObject);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    hrRetCode = pObject->GetProp(L"szDirectXVersionLongEnglish", &var);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    wcstombs(szTempDxVersion, var.bstrVal, sizeof(szTempDxVersion));   
    szTempDxVersion[sizeof(szTempDxVersion) - 1] = '\0';

    VariantClear(&var);

    nRetCode = (int)strlen(szTempDxVersion);
    KGLOG_PROCESS_ERROR(nRetCode > 8);  // szDXVersion should equal "DirectX **"

    for (i = 8; i < nRetCode; ++i)
    {
        if (szTempDxVersion[i] == ' ' ||  szTempDxVersion[i] == '(')
        {
            szDXVersion[i - 8] = '\0';
            break;
        }
        szDXVersion[i - 8] = szTempDxVersion[i];
    }

    if (i == nRetCode)
        szDXVersion[i - 8] = '\0';

    nResult = true;
Exit0:
    SAFE_RELEASE(pDxDiagRoot);
    SAFE_RELEASE(pDxDiagProvider);

    if (bComInit)
    {
        ::CoUninitialize();
        bComInit = false;
    }
    return nResult;
}
//////////////////////////////////////////////////////////////////////////
int SetShowContactDiagFlag(int nFlag)
{
    int nResult = false;
    int nRetCode = false;
    const char BENCHMARK_CONFIG_FILE[] = "BenchmarkConfig.ini";
    KSmartIniFile pFile;

    pFile = g_OpenIniFile(BENCHMARK_CONFIG_FILE);
    KGLOG_PROCESS_ERROR(pFile);

    nRetCode = pFile->WriteInteger("main", "ShowContactDiag", nFlag);
    KGLOG_PROCESS_ERROR(nRetCode);

    pFile->Save(BENCHMARK_CONFIG_FILE);

    nResult = true;
Exit0:
    return nResult;
}
//////////////////////////////////////////////////////////////////////////
enum FILE_VERSION
{
    eLastSampleFileVersion = 1,
    eLasteDumpFileVersion = 2,
    eSampleFileVersion = 3,
    eDumpFileVersion = 4,
};
//////////////////////////////////////////////////////////////////////////
int KG_MD5(char *pszOutput, unsigned uOutputLen, BYTE *pbyInput, unsigned uInputLen)
{
    int nResult = false;
    unsigned char szMD5Code[16];
    const char cszHexCode[] = "0123456789ABCDEF";
    md5_state_t md5_state;

    KGLOG_PROCESS_ERROR(pszOutput);
    KGLOG_PROCESS_ERROR(uOutputLen >= 32);
    KGLOG_PROCESS_ERROR(pbyInput);

    md5_init(&md5_state);

    md5_append(&md5_state, (unsigned char *)pbyInput, uInputLen);

    md5_finish(&md5_state, szMD5Code);

    for (int i = 0; i < 16; ++i)
    {
        pszOutput[2 * i] = cszHexCode[(int)szMD5Code[i] / 16];
        pszOutput[2 * i + 1] = cszHexCode[(int)szMD5Code[i] % 16];
    }

    nResult = true;
Exit0:
    return nResult;
}

//////////////////////////////////////////////////////////////////////////
int ReadIniFile()
{
    int nResult = false;
    int nRetCode = false;
    TCHAR szAppDir[PATH_MAX] = {0};
    TCHAR szFilePath[PATH_MAX] = {0};
    KSmartIniFile pFile;

    GetCurrentDirectory(sizeof(szAppDir), szAppDir);

    _stprintf(szFilePath, "%s\\BenchmarkConfig.ini", szAppDir);

    pFile = g_OpenIniFile(szFilePath);
    KGLOG_PROCESS_ERROR(pFile);

    nRetCode = pFile->GetString("main", "Host", NULL, g_szHost, sizeof(g_szHost));
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = pFile->GetInteger("main", "Port", INTERNET_DEFAULT_HTTP_PORT, &g_nPort);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = pFile->GetString("main", "ReceiveInfoPage", NULL, g_szReceiveInfoPage, sizeof(g_szReceiveInfoPage));
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = pFile->GetString("main", "ReceiveFilePage", NULL, g_szReceiveFilePage, sizeof(g_szReceiveFilePage));
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = pFile->GetString("main", "SearchPage", NULL, g_szSearchPage, sizeof(g_szSearchPage));
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = pFile->GetString("main", "CheckVersionPage", NULL, g_szVersionPage, sizeof(g_szVersionPage));
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = pFile->GetInteger("main", "ShowContactDiag", 1, &g_nShowContactDiag);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = pFile->GetInteger("version", "Version", 0, &g_nVersion);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}
//////////////////////////////////////////////////////////////////////////
int CheckVersion(int* pnExitFlag)
{
    int nResult  = false;
    int nRetCode = false;
    BYTE *pbyBuffer = NULL;
    DWORD dwBufferSize = 0;
    KG_HttpClient HttpClient;
    const char cszTempFile[] = "qwertyuiop_benchmarkconfig.tmp";
    KSmartIniFile pFile;
    HANDLE hFile = INVALID_HANDLE_VALUE;
    DWORD dwBytesWrite = 0;
    int nFileCreate = false;
    TCHAR szAppDir[PATH_MAX] = {0};
    TCHAR szFilePath[PATH_MAX] = {0};
    int nVersion = 0;
    TCHAR *pszDescription = NULL;
    TCHAR szDownloadUrl[PATH_MAX] = {0};
    int nExitFlag = false;

    nRetCode = HttpClient.RequestPost("JX3 Benchmark", g_szHost, g_szVersionPage, g_nPort);
    KG_PROCESS_SUCCESS(!nRetCode);

    nRetCode = HttpClient.GetResponse(&pbyBuffer, &dwBufferSize);
    KGLOG_PROCESS_ERROR(nRetCode);
    KGLOG_PROCESS_ERROR(dwBufferSize > 0);

    hFile = ::CreateFile(
        cszTempFile, 
        GENERIC_WRITE, 
        FILE_SHARE_WRITE, 
        NULL, 
        CREATE_ALWAYS, 
        FILE_ATTRIBUTE_NORMAL, 
        NULL
    );
    KGLOG_PROCESS_ERROR(hFile != INVALID_HANDLE_VALUE);
    
    nFileCreate = true;

    nRetCode = ::WriteFile(hFile, pbyBuffer, dwBufferSize, &dwBytesWrite, NULL);
    KGLOG_PROCESS_ERROR(nRetCode);
    KGLOG_PROCESS_ERROR(dwBufferSize == dwBytesWrite);

    CloseHandle(hFile);
    hFile = INVALID_HANDLE_VALUE;

    GetCurrentDirectory(sizeof(szAppDir), szAppDir);

    nRetCode = _stprintf(szFilePath, "%s\\%s", szAppDir, cszTempFile);
    KGLOG_PROCESS_ERROR(nRetCode > 0);

    pFile = g_OpenIniFile(szFilePath);
    KGLOG_PROCESS_ERROR(pFile);

    nRetCode = pFile->GetInteger("version", "Version", 0, &nVersion);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = pFile->GetString("version", "DownloadUrl", NULL, szDownloadUrl, sizeof(szDownloadUrl) - 1);
    KGLOG_PROCESS_ERROR(nRetCode);
    szDownloadUrl[sizeof(szDownloadUrl) - 1] = '\0';

    pszDescription = new char[dwBytesWrite];
    KGLOG_PROCESS_ERROR(pszDescription);

    nRetCode = pFile->GetString("version", "Description", NULL, (char *)pszDescription, dwBytesWrite - 1);
    KGLOG_PROCESS_ERROR(nRetCode);
    pszDescription[dwBytesWrite - 1] = '\0';

    KG_PROCESS_SUCCESS(g_nVersion >= nVersion);

    nRetCode = ::MessageBox(NULL, (LPCSTR)pszDescription, "发现新版本", MB_YESNO);
    if (nRetCode == IDYES)
    {
        nRetCode = (int)::ShellExecute(NULL, "open", "autoupdate.exe", szDownloadUrl, NULL, SW_SHOWNORMAL);
        KGLOG_PROCESS_ERROR(nRetCode > 32);

        nExitFlag = true;
        goto Exit1;
    }

Exit1:
    *pnExitFlag = nExitFlag;

    nResult = true;
Exit0:

    if (hFile != INVALID_HANDLE_VALUE)
    {
        CloseHandle(hFile);
        hFile = INVALID_HANDLE_VALUE;
    }

    if (nFileCreate)
    {
        ::DeleteFile(cszTempFile);
        nFileCreate = false;
    }

    KG_DELETE_ARRAY(pszDescription);
    KG_DELETE_ARRAY(pbyBuffer);
    return nResult;
}

//////////////////////////////////////////////////////////////////////////
int GetEngineVersion(char *pszVersionBuffer, unsigned uBufferLen)
{
    int nResult  = false;
    int nRetCode = false;
    char *pszEngineVersion = NULL;
    DWORD dwFileVersionInfoSize = 0;
    BYTE *pbyFileVersionInfoBuffer = NULL;
    char szSubBlock[128];
    UINT uLen = 0;
    struct LANG_CODEPAGE
    {
        WORD wLanguage;
        WORD wCodePage;
    } *pLangCodepage;

    KGLOG_PROCESS_ERROR(pszVersionBuffer);
    KGLOG_PROCESS_ERROR(uBufferLen >= 32);

#ifdef _DEBUG
    dwFileVersionInfoSize = ::GetFileVersionInfoSize("KG3DEngineD.dll", NULL);
#else
    dwFileVersionInfoSize = ::GetFileVersionInfoSize("KG3DEngine.dll", NULL);
#endif
    pbyFileVersionInfoBuffer = new BYTE[dwFileVersionInfoSize];
    KGLOG_PROCESS_ERROR(pbyFileVersionInfoBuffer);

    ZeroMemory(pbyFileVersionInfoBuffer, dwFileVersionInfoSize);

#ifdef _DEBUG
    nRetCode = ::GetFileVersionInfo("KG3DEngineD.dll", 0, dwFileVersionInfoSize, pbyFileVersionInfoBuffer);
#else
    nRetCode = ::GetFileVersionInfo("KG3DEngine.dll", 0, dwFileVersionInfoSize, pbyFileVersionInfoBuffer);
#endif
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = ::VerQueryValue(
        (LPVOID)pbyFileVersionInfoBuffer, 
        "\\VarFileInfo\\Translation", 
        (LPVOID *)&pLangCodepage, 
        &uLen
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = sprintf(
        szSubBlock,
        "\\StringFileInfo\\%04x%04x\\FileVersion",
        pLangCodepage->wLanguage,
        pLangCodepage->wCodePage
    );
    KGLOG_PROCESS_ERROR(nRetCode <= sizeof(szSubBlock));

    szSubBlock[sizeof(szSubBlock) - 1] = '\0';

    nRetCode = ::VerQueryValue(pbyFileVersionInfoBuffer, szSubBlock, (LPVOID *)&pszEngineVersion, &uLen);
    KGLOG_PROCESS_ERROR(nRetCode);

    ZeroMemory(pszVersionBuffer, uBufferLen);
    strncpy(pszVersionBuffer, pszEngineVersion, uBufferLen);
    pszVersionBuffer[uBufferLen - 1] = '\0';

    nResult = true;
Exit0:
    KG_DELETE_ARRAY(pbyFileVersionInfoBuffer);

    return nResult;
}
//////////////////////////////////////////////////////////////////////////

int GetFileMD5(char *pszOutputBuffer, unsigned uOutputBufferLen, const char cszFileName[])
{
    int nResult  = false;
    int nRetCode = false;
    HANDLE hFile = INVALID_HANDLE_VALUE;
    BYTE *pbyFileBuffer = NULL;
    DWORD dwFileSize = 0;
    DWORD dwReadBytes = 0;
    DWORD dwTotalRead = 0;
    const DWORD BUFFER_SIZE = 64 * 1024;
    md5_state_t md5_state;
    unsigned char byMD5Value[16];

    KGLOG_PROCESS_ERROR(pszOutputBuffer);
    KGLOG_PROCESS_ERROR(uOutputBufferLen >= 32);

    hFile = ::CreateFile(
        cszFileName,
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
        NULL
    );
    KGLOG_PROCESS_ERROR(hFile != INVALID_HANDLE_VALUE);

    dwFileSize = ::GetFileSize(hFile, NULL);

    pbyFileBuffer = new BYTE[BUFFER_SIZE];

    md5_init(&md5_state);

    for (dwTotalRead = 0; dwTotalRead < dwFileSize; dwTotalRead += dwReadBytes)
    {
        nRetCode = ::ReadFile(hFile, pbyFileBuffer, BUFFER_SIZE, &dwReadBytes, NULL);
        KGLOG_PROCESS_ERROR(nRetCode);
        KGLOG_PROCESS_ERROR(dwReadBytes <= BUFFER_SIZE);

        md5_append(&md5_state, pbyFileBuffer, dwReadBytes);
    }

    md5_finish(&md5_state, byMD5Value);

    nRetCode = sprintf(
        pszOutputBuffer,
        "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X",
        byMD5Value[0],  byMD5Value[1],  byMD5Value[2],  byMD5Value[3],
        byMD5Value[4],  byMD5Value[5],  byMD5Value[6],  byMD5Value[7],
        byMD5Value[8],  byMD5Value[9],  byMD5Value[10], byMD5Value[11],
        byMD5Value[12], byMD5Value[13], byMD5Value[14], byMD5Value[15]
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0);

    uOutputBufferLen = nRetCode;

    nResult = true;
Exit0:
    KG_DELETE_ARRAY(pbyFileBuffer);
    if (hFile != INVALID_HANDLE_VALUE)
    {
        ::CloseHandle(hFile);
        hFile = INVALID_HANDLE_VALUE;
    }
    return nResult;
}
//////////////////////////////////////////////////////////////////////////
int SendSystemInfo()
{
    int nResult  = false;
    int nRetCode = false;
    KG_HttpClient HttpClient;
    KG_SystemInfoCollector SystemInfoCollector;
    KG_SystemInfoCollector::KG_SYSTEM_INFO SystemInfo;
    unsigned char byMachineID[16];
    char szUUID[64];
    DWORD dwBufferSize = 0;
    BYTE *pbyBuffer = NULL;
    string strContent;
    char szContentMD5[64];

    g_nGetContactInfo = false;

    nRetCode = SystemInfoCollector.QuerySystemInfo(&SystemInfo);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = KG_GetMachineID(byMachineID);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = sprintf(
        szUUID,
        "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X",
        byMachineID[0],  byMachineID[1],  byMachineID[2],  byMachineID[3],
        byMachineID[4],  byMachineID[5],  byMachineID[6],  byMachineID[7],
        byMachineID[8],  byMachineID[9],  byMachineID[10], byMachineID[11],
        byMachineID[12], byMachineID[13], byMachineID[14], byMachineID[15]
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0);

    strContent.append(szUUID);
    strContent.append(SystemInfo.szOS);
    strContent.append(SystemInfo.szOSPack);
    strContent.append(SystemInfo.szProcessor);
    strContent.append(SystemInfo.szSystemMemory);
    strContent.append(SystemInfo.szDXVersion);
    strContent.append(SystemInfo.szVGAChip);
    strContent.append(SystemInfo.szVGAMemory);
    strContent.append(SystemInfo.szDisplayRatio);
    strContent.append(SystemInfo.szContactInfo);

    dwBufferSize = (DWORD)strContent.length();
    pbyBuffer = new BYTE[dwBufferSize];
    CopyMemory(pbyBuffer, strContent.c_str(), strContent.length());

    ZeroMemory(szContentMD5, sizeof(szContentMD5));
    nRetCode = KG_MD5(szContentMD5, sizeof(szContentMD5), pbyBuffer, dwBufferSize);
    KGLOG_PROCESS_ERROR(nRetCode);

    szContentMD5[sizeof(szContentMD5) - 1] = '\0';

    nRetCode = HttpClient.AddPostParam("Type", CURRENT_SYSTEM_INFO_VERSION);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = HttpClient.AddPostParam("UUID", szUUID);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = HttpClient.AddPostParam("OS", SystemInfo.szOS);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = HttpClient.AddPostParam("OSPack", SystemInfo.szOSPack);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = HttpClient.AddPostParam("Processor", SystemInfo.szProcessor);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = HttpClient.AddPostParam("SystemMemory", SystemInfo.szSystemMemory);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = HttpClient.AddPostParam("DXVersion", SystemInfo.szDXVersion);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = HttpClient.AddPostParam("VGAChip", SystemInfo.szVGAChip);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = HttpClient.AddPostParam("VGAMemory", SystemInfo.szVGAMemory);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = HttpClient.AddPostParam("VGADriverVersion", SystemInfo.szVGADriverVersion);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = HttpClient.AddPostParam("DisplayRatio", SystemInfo.szDisplayRatio);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = HttpClient.AddPostParam("ContactInfo", SystemInfo.szContactInfo);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = HttpClient.AddPostParam("MD5", szContentMD5);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = HttpClient.RequestPost("Benchmark", g_szHost, g_szReceiveInfoPage, g_nPort);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    if (nResult && g_nGetContactInfo)
    {
        SetShowContactDiagFlag(0);
    }
    KG_DELETE_ARRAY(pbyBuffer);

    return nResult;
}
//////////////////////////////////////////////////////////////////////////
int SendDumpFile(const char cszFileName[])
{
    int nResult  = false;
    int nRetCode = false;
    DWORD dwBufferSize = 0;
    KG_HttpClient HttpClient;
    unsigned char byMachineID[16];
    char szTypeValue[16];
    char szUUID[64];
    char szFileMD5[64];
    BYTE *pbyRespondBuffer = NULL;

    ZeroMemory(szFileMD5, sizeof(szFileMD5));
    nRetCode = GetFileMD5(szFileMD5, (unsigned)sizeof(szFileMD5), cszFileName);
    KGLOG_PROCESS_ERROR(nRetCode);

    szFileMD5[sizeof(szFileMD5) - 1] = '\0';

    itoa(eDumpFileVersion, szTypeValue, 10);

    nRetCode = HttpClient.AddPostParam("FileMD5", szFileMD5);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = HttpClient.AddPostParam("Type", szTypeValue);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = HttpClient.RequestPost("Benchmark", g_szHost, g_szSearchPage, g_nPort);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = HttpClient.GetResponse(&pbyRespondBuffer, &dwBufferSize);
    KGLOG_PROCESS_ERROR(nRetCode);
    KGLOG_PROCESS_ERROR(pbyRespondBuffer);
    KGLOG_PROCESS_ERROR(dwBufferSize > 0);

    // pbyRespondBuffer == "0", means nobody had uploaded the file
    nRetCode = stricmp((const char *)pbyRespondBuffer, "0");
    KG_PROCESS_SUCCESS(nRetCode);

    nRetCode = KG_GetMachineID(byMachineID);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = sprintf(
        szUUID,
        "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X",
        byMachineID[0],  byMachineID[1],  byMachineID[2],  byMachineID[3],
        byMachineID[4],  byMachineID[5],  byMachineID[6],  byMachineID[7],
        byMachineID[8],  byMachineID[9],  byMachineID[10], byMachineID[11],
        byMachineID[12], byMachineID[13], byMachineID[14], byMachineID[15]
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0);

    nRetCode = HttpClient.AddPostParam("Type", szTypeValue);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = HttpClient.AddPostParam("UUID", szUUID);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = HttpClient.AddPostParam("FileMD5", szFileMD5);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = HttpClient.AddPostParam("File", cszFileName, KG_HttpClient::eBinaryFile);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = HttpClient.RequestPost("Benchmark", g_szHost, g_szReceiveFilePage, g_nPort);
    KGLOG_PROCESS_ERROR(nRetCode);

Exit1:
    nResult = true;
Exit0:
    KG_DELETE_ARRAY(pbyRespondBuffer);

    return nResult;
}
//////////////////////////////////////////////////////////////////////////
int SendSampleFile(const char cszSampleFileName[])
{
    int nResult  = false;
    int nRetCode = false;
    KG_HttpClient HttpClient;
    char szTypeValue[16];
    unsigned char byMachineID[16];
    char szUUID[64];
    char szSampleFileMD5[64];
    BYTE *pbyRespondBuffer = NULL;

    ZeroMemory(szSampleFileMD5, sizeof(szSampleFileMD5));
    nRetCode = GetFileMD5(szSampleFileMD5, (unsigned)sizeof(szSampleFileMD5), cszSampleFileName);
    KGLOG_PROCESS_ERROR(nRetCode);

    szSampleFileMD5[sizeof(szSampleFileMD5) - 1] = '\0';

    nRetCode = KG_GetMachineID(byMachineID);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = sprintf(
        szUUID,
        "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X",
        byMachineID[0],  byMachineID[1],  byMachineID[2],  byMachineID[3],
        byMachineID[4],  byMachineID[5],  byMachineID[6],  byMachineID[7],
        byMachineID[8],  byMachineID[9],  byMachineID[10], byMachineID[11],
        byMachineID[12], byMachineID[13], byMachineID[14], byMachineID[15]
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0);

    itoa(eSampleFileVersion, szTypeValue, 10);

    nRetCode = HttpClient.AddPostParam("Type", szTypeValue);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = HttpClient.AddPostParam("UUID", szUUID);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = HttpClient.AddPostParam("FileMD5", szSampleFileMD5);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = HttpClient.AddPostParam("File", cszSampleFileName, KG_HttpClient::eBinaryFile);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = HttpClient.RequestPost("Benchmark", g_szHost, g_szReceiveFilePage, g_nPort);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:

    KG_DELETE_ARRAY(pbyRespondBuffer);

    return nResult;
}
//////////////////////////////////////////////////////////////////////////
int ReadFileBuffer(const char cszFileName[], BYTE **ppbyFileBuffer, DWORD *pdwBufferSize)
{
    int     nResult     = false;
    int     nRetCode    = false;
    HANDLE  hFile       = INVALID_HANDLE_VALUE;
    DWORD   dwFileSize  = 0;
    DWORD   dwBytesRead = 0;
    BYTE   *pbyBuffer   = NULL;

    hFile = ::CreateFile(
        cszFileName,
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
        NULL
    );
    KGLOG_PROCESS_ERROR(hFile != INVALID_HANDLE_VALUE);

    dwFileSize = ::GetFileSize(hFile, NULL);
    
    pbyBuffer = new BYTE[dwFileSize];
    KGLOG_PROCESS_ERROR(pbyBuffer);

    nRetCode = ::ReadFile(hFile, pbyBuffer, dwFileSize, &dwBytesRead, NULL);
    KGLOG_PROCESS_ERROR(nRetCode);
    KGLOG_PROCESS_ERROR(dwFileSize == dwBytesRead);

    *ppbyFileBuffer = pbyBuffer;
    *pdwBufferSize = dwFileSize;

    nResult = true;
Exit0:
    if (!nResult)
    {
        KG_DELETE_ARRAY(pbyBuffer);
    }

    if (hFile != INVALID_HANDLE_VALUE)
    {
        ::CloseHandle(hFile);
        hFile = INVALID_HANDLE_VALUE;
    }

    return nResult;
}
//////////////////////////////////////////////////////////////////////////
LONG WINAPI CreateDumpFile(_EXCEPTION_POINTERS *pExceptionInfo)
{
    LONG    lResult             = false;
    int     nRetCode            = false;
    int     nTempDumpFileInit   = false;
    int     nDumpFileInit       = false;
    HANDLE  hTempDumpFile       = INVALID_HANDLE_VALUE;
    HANDLE  hDumpFile           = INVALID_HANDLE_VALUE;
    HANDLE  hProcess            = 0;
    DWORD   dwProcessID         = 0;
    char    szConfigFile[32]    = "config.ini";
    BYTE   *pbyConfigFileBuffer = NULL;
    DWORD   dwConfigFileSize    = 0;
    BYTE   *pbyDumpFileBuffer   = NULL;
    DWORD   dwDumpFileSize      = 0;
    DWORD   dwWriteBytes        = 0;
    string  strBuffer;
    MINIDUMP_EXCEPTION_INFORMATION ExceptionParam;
    char    szEngineVersion[33];

    nRetCode = SendSystemInfo();
    //KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = GetEngineVersion(szEngineVersion, sizeof(szEngineVersion));
    KGLOG_PROCESS_ERROR(nRetCode);
    szEngineVersion[sizeof(szEngineVersion) - 1] = '\0';

    nRetCode = (int)strlen(szEngineVersion);
    strBuffer.append(szEngineVersion);
    strBuffer.append(32 - nRetCode, ' ');

    nRetCode = ReadFileBuffer(szConfigFile, &pbyConfigFileBuffer, &dwConfigFileSize);
    KGLOG_PROCESS_ERROR(nRetCode);
    KGLOG_PROCESS_ERROR(pbyConfigFileBuffer);

    nRetCode = dwConfigFileSize > (5 * 1024) ? (5 * 1024) : dwConfigFileSize;

    strBuffer.append((const char *)pbyConfigFileBuffer, nRetCode);

    strBuffer.append(5 * 1024 - nRetCode, ' ');

    hTempDumpFile = ::CreateFile(
        "Dump.tmp", 
        GENERIC_WRITE, 
        FILE_SHARE_WRITE, 
        NULL, 
        CREATE_ALWAYS, 
        FILE_ATTRIBUTE_NORMAL, 
        NULL
    );
    KGLOG_PROCESS_ERROR(hTempDumpFile != INVALID_HANDLE_VALUE);

    nTempDumpFileInit = true;

    ExceptionParam.ThreadId = GetCurrentThreadId();
    ExceptionParam.ExceptionPointers = pExceptionInfo;
    ExceptionParam.ClientPointers = true;

    hProcess = ::GetCurrentProcess();
    dwProcessID = ::GetCurrentProcessId();

    nRetCode = ::MiniDumpWriteDump(
        hProcess,
        dwProcessID,
        hTempDumpFile,
        MiniDumpNormal,
        &ExceptionParam,
        NULL,
        NULL
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    ::CloseHandle(hTempDumpFile);
    hTempDumpFile = INVALID_HANDLE_VALUE;

    nRetCode = ReadFileBuffer("Dump.tmp", &pbyDumpFileBuffer, &dwDumpFileSize);
    KGLOG_PROCESS_ERROR(nRetCode);
    KGLOG_PROCESS_ERROR(pbyDumpFileBuffer);

    hDumpFile = ::CreateFile(
        "BenchmarkDumpFile.dmp", 
        GENERIC_WRITE, 
        FILE_SHARE_WRITE, 
        NULL, 
        CREATE_ALWAYS, 
        FILE_ATTRIBUTE_NORMAL, 
        NULL
    );
    KGLOG_PROCESS_ERROR(hDumpFile != INVALID_HANDLE_VALUE);

    nDumpFileInit = true;

    nRetCode = ::WriteFile(hDumpFile, strBuffer.c_str(), (DWORD)strBuffer.length(), &dwWriteBytes, NULL);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = ::WriteFile(hDumpFile, pbyDumpFileBuffer, dwDumpFileSize, &dwWriteBytes, NULL);
    KGLOG_PROCESS_ERROR(nRetCode);

    ::CloseHandle(hDumpFile);
    hDumpFile = INVALID_HANDLE_VALUE;

    nRetCode = SendDumpFile("BenchmarkDumpFile.dmp");
    KGLOG_PROCESS_ERROR(nRetCode);


    lResult = true;
Exit0:
    KG_DELETE_ARRAY(pbyDumpFileBuffer);
    KG_DELETE_ARRAY(pbyConfigFileBuffer);
   
    if (hDumpFile != INVALID_HANDLE_VALUE)
    {
        ::CloseHandle(hDumpFile);
        hDumpFile = INVALID_HANDLE_VALUE;
    }

    if (hTempDumpFile != INVALID_HANDLE_VALUE)
    {
        ::CloseHandle(hTempDumpFile);
        hTempDumpFile = INVALID_HANDLE_VALUE;
    }

    if (nDumpFileInit)
    {
        ::DeleteFile("BenchmarkDumpFile.dmp");
        nDumpFileInit = false;
    }

    if (nTempDumpFileInit)
    {
        ::DeleteFile("Dump.tmp");
        nTempDumpFileInit = false;
    }

    return lResult;
}
//////////////////////////////////////////////////////////////////////////
