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
#include "Engine/md5.h"
#include "Engine/KG_MachineID.h"

# pragma comment(lib, "wbemuuid.lib")

#define CURRENT_SYSTEM_INFO_VERSION     "1"

//////////////////////////////////////////////////////////////////////////
TCHAR g_szHost[PATH_MAX];
int   g_nPort;
TCHAR g_szReceiveInfoPage[PATH_MAX];
TCHAR g_szReceiveFilePage[PATH_MAX];
TCHAR g_szSearchPage[PATH_MAX];
TCHAR g_szVersionPage[PATH_MAX];
int   g_nVersion;

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
        char szVGAChip[64];
        char szVGAMemory[16];
        char szVGADriverVersion[32];
        char szDisplayRatio[16];
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
    char                szProcessorName[64];
    int                 nCoreNum        = 0;
    IEnumWbemClassObject *pEnumerator = NULL;
    IWbemClassObject *pWbemClassObj = NULL;
    VARIANT vtProp;

    KG_PROCESS_ERROR(pSystemInfo);

    nRetCode = InitWbemService();
    KGLOG_PROCESS_ERROR(nRetCode);

    bWbemServiceInit = true;

    ZeroMemory(&SystemInfo, sizeof(SystemInfo));

    // get operate system version
    ZeroMemory(&OSVersion, sizeof(OSVersion));
    OSVersion.dwOSVersionInfoSize = sizeof(OSVersion);

    nRetCode = ::GetVersionEx(&OSVersion);
    KGLOG_PROCESS_ERROR(nRetCode);

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

    // get system physical memory size

    hrRetCode = GetEnumerator(&pEnumerator, "Win32_PhysicalMemory");
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    dwMemorySize = 0;
    while (true)
    {
        char szMemorySize[32];
        hrRetCode = pEnumerator->Next(WBEM_INFINITE, 1, &pWbemClassObj, (ULONG *)&nRetCode);
        if (hrRetCode == WBEM_S_FALSE)
            break;
        KGLOG_COM_PROCESS_ERROR(hrRetCode);

        hrRetCode = pWbemClassObj->Get(L"Capacity", 0, &vtProp, 0, 0);
        KGLOG_COM_PROCESS_ERROR(hrRetCode);

        wcstombs(szMemorySize, vtProp.bstrVal, sizeof(szMemorySize));
        szMemorySize[sizeof(szMemorySize) - 1] = '\0';
        
        VariantClear(&vtProp);

        dwMemorySize += atoi(szMemorySize) / 1024 / 1024;

        SAFE_RELEASE(pWbemClassObj);
    }
    SAFE_RELEASE(pEnumerator);

    nRetCode = snprintf(
        SystemInfo.szSystemMemory,
        sizeof(SystemInfo.szSystemMemory),
        "%d",
        dwMemorySize
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0);

    SystemInfo.szSystemMemory[sizeof(SystemInfo.szSystemMemory) - 1] = '\0';

    // enum processor
    hrRetCode = GetEnumerator(&pEnumerator, "Win32_Processor");
    KGLOG_COM_PROCESS_ERROR(hrRetCode);
    
    hrRetCode = pEnumerator->Next(WBEM_INFINITE, 1, &pWbemClassObj, (ULONG *)&nRetCode);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    // get processor name
    hrRetCode = pWbemClassObj->Get(L"Name", 0, &vtProp, 0, 0);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    wcstombs(szProcessorName, vtProp.bstrVal, sizeof(szProcessorName));
    szProcessorName[sizeof(szProcessorName) - 1] = '\0';

    VariantClear(&vtProp);
    
    hrRetCode = pWbemClassObj->Get(L"NumberOfCores", 0, &vtProp, 0, 0);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);
    
    nCoreNum = vtProp.lVal;
    VariantClear(&vtProp);

    nRetCode = snprintf(
        SystemInfo.szProcessor, 
        sizeof(SystemInfo.szProcessor), 
        "%s %d Core(s)", 
        szProcessorName, nCoreNum
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0);
    
    SystemInfo.szProcessor[sizeof(SystemInfo.szProcessor) - 1] = '\0';

    SAFE_RELEASE(pWbemClassObj);
    SAFE_RELEASE(pEnumerator);

    // enum video graphic card
    hrRetCode = GetEnumerator(&pEnumerator, "Win32_VideoController");
    KGLOG_COM_PROCESS_ERROR(hrRetCode);
    KGLOG_PROCESS_ERROR(pEnumerator);

    hrRetCode = pEnumerator->Next(WBEM_INFINITE, 1, &pWbemClassObj, (ULONG *)&nRetCode);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    // get vga memory
    hrRetCode = pWbemClassObj->Get(L"AdapterRAM", 0, &vtProp, 0, 0);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    nRetCode = snprintf(SystemInfo.szVGAMemory, sizeof(SystemInfo.szVGAMemory), "%d", vtProp.lVal / 1024 / 1024);
    KGLOG_PROCESS_ERROR(nRetCode > 0);
    SystemInfo.szVGAMemory[sizeof(SystemInfo.szVGAMemory) - 1] = '\0';
    
    VariantClear(&vtProp);

    // get vga chip
    hrRetCode = pWbemClassObj->Get(L"VideoProcessor", 0, &vtProp, 0, 0);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);
    
    wcstombs(SystemInfo.szVGAChip, vtProp.bstrVal, sizeof(SystemInfo.szVGAChip));   
    SystemInfo.szVGAChip[sizeof(SystemInfo.szVGAChip) - 1] = '\0';

    VariantClear(&vtProp);

    // get vga driver version
    hrRetCode = pWbemClassObj->Get(L"DriverVersion", 0, &vtProp, 0, 0);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);
    
    wcstombs(SystemInfo.szVGADriverVersion, vtProp.bstrVal, sizeof(SystemInfo.szVGADriverVersion));   
    SystemInfo.szVGADriverVersion[sizeof(SystemInfo.szVGADriverVersion) - 1] = '\0';

    VariantClear(&vtProp);

    SAFE_RELEASE(pWbemClassObj);
    SAFE_RELEASE(pEnumerator);

    // get display ratio
    hrRetCode = GetEnumerator(&pEnumerator, "Win32_DisplayConfiguration");
    KGLOG_COM_PROCESS_ERROR(hrRetCode);
    KGLOG_PROCESS_ERROR(pEnumerator);

    hrRetCode = pEnumerator->Next(WBEM_INFINITE, 1, &pWbemClassObj, (ULONG *)&nRetCode);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    hrRetCode = pWbemClassObj->Get(L"PelsWidth", 0, &vtProp, 0, 0);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);
    
    dwWidth = vtProp.lVal;
    VariantClear(&vtProp);

    hrRetCode = pWbemClassObj->Get(L"PelsHeight", 0, &vtProp, 0, 0);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);
    
    dwHeight = vtProp.lVal;
    VariantClear(&vtProp);

    sprintf(SystemInfo.szDisplayRatio, "%u*%u", dwWidth, dwHeight);
    
    SAFE_RELEASE(pWbemClassObj);
    SAFE_RELEASE(pEnumerator);
    
    UnintWbemService();
    bWbemServiceInit = false;

    //
    nRetCode = GetDXVersion(SystemInfo.szDXVersion);
    KGLOG_PROCESS_ERROR(nRetCode);

    *pSystemInfo = SystemInfo;

    nResult = true;
Exit0:
    VariantClear(&vtProp);
    SAFE_RELEASE(pWbemClassObj);
    SAFE_RELEASE(pEnumerator);

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
    IDxDiagContainer *pContainer = NULL;
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

