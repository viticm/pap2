#include "StdAfx.h"
#include "SystemInfo.h"
#include "toolhelp.h"
#include "my_md5.h"
#include <Wbemidl.h>
#include <comutil.h>
#include <atlbase.h>

#pragma comment(lib, "comsuppw.lib")

#define MAX_DISPLAY_NUM 2

CSystemInfo::CSystemInfo(void) :
    m_strKey("92A73EB6-FA72-40f9-AFD2-BD0376F40FD0")
{
}

CSystemInfo::~CSystemInfo(void)
{
}

HRESULT CSystemInfo::GetSystemVersion(CString& strVersion)
{
    OSVERSIONINFOEX osvi;
    CString         verNumber;
    HRESULT         hResult = E_FAIL;

    ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

    if( !::GetVersionEx((OSVERSIONINFO*)&osvi) )
    {
        goto Exit0;
    }

    // 必须是windows 2000 及以上操作系统
    if (VER_PLATFORM_WIN32_NT != osvi.dwPlatformId)
    {
        goto Exit0;
    }

    // windows 产品类型
    if ( 5 == osvi.dwMajorVersion && 0 == osvi.dwMinorVersion )
    {
        strVersion.Append("Microsoft Windows 2000 ");
    }
    else if ( 5 == osvi.dwMajorVersion && 1 == osvi.dwMinorVersion )
    {
        strVersion.Append("Microsoft Windows XP ");
    }
    else if ( 6 == osvi.dwMajorVersion && 0 == osvi.dwMinorVersion )
    {
        strVersion.Append("Microsoft Windows Vista ");
    }
    else
    {
        strVersion.Append("Microsoft Windows NT ");
    }

    // windows 版本类型
    if ( osvi.wProductType == VER_NT_WORKSTATION )
    {
        if( osvi.wSuiteMask & VER_SUITE_PERSONAL )
        {
            strVersion.Append("Personal ");
        }
        else
        {
            strVersion.Append("Professional ");
        }
    }
    else if ( osvi.wProductType == VER_NT_SERVER )
    {
        if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
        {
            strVersion.Append("DataCenter Server ");
        }
        else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
        {
            strVersion.Append("Advanced Server ");
        }
        else
        {
            strVersion.Append("Server ");
        }
    }
    
    // 版本号
    verNumber.Format("%s", osvi.szCSDVersion);

    strVersion.Append(verNumber);

    hResult = S_OK;

Exit0:

    return hResult;
}

HRESULT CSystemInfo::GetSystemId(CString& SystemId)
{
	OSVERSIONINFOEX osvi;
	HRESULT         hResult = E_FAIL;
	double          Id;
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	if( !::GetVersionEx((OSVERSIONINFO*)&osvi) )
	{
		goto Exit0;
	}

	// 必须是windows 2000 及以上操作系统
	if (VER_PLATFORM_WIN32_NT != osvi.dwPlatformId)
	{
		goto Exit0;
	}
	Id = osvi.dwMajorVersion + (double)osvi.dwMinorVersion / 10.0;
	Id += 0.05;
	SystemId.Format("%.1f", Id);
	
    hResult = S_OK;

Exit0:
	return hResult;
}

HRESULT CSystemInfo::GetSystemMemInfo(CString& strMem)
{
    MEMORYSTATUSEX    Meminfo;
    DOUBLE            dSize;
    HRESULT           hResult = E_FAIL;


    ZeroMemory(&Meminfo, sizeof(Meminfo));
    Meminfo.dwLength = sizeof(Meminfo);

    if ( !::GlobalMemoryStatusEx(&Meminfo) )
    {
        goto Exit0;
    }

    dSize = (DOUBLE)Meminfo.ullTotalPhys;
	dSize /= (1024.0);
    dSize /= (1024.0);
    dSize += 0.5;

    strMem.Format("%.0f", dSize);

    hResult = S_OK;

Exit0:

    return hResult;
}

HRESULT CSystemInfo::GetSystemCpuInfo(CString& strCpu)
{
    CRegKey regKey;
    CString strRegOpen;
    UINT    uRegIndex = 0;
    ULONG   uMaxValue = 0;
    CHAR    szCpuInfo[MAX_PATH] = { 0 };
    CString strRegKey = "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\%d";

    while (TRUE)
    {
        strRegOpen.Format(strRegKey, uRegIndex);

        if ( ERROR_SUCCESS != regKey.Open(HKEY_LOCAL_MACHINE, strRegOpen, KEY_READ) )
        {
            break;
        }

        uMaxValue = sizeof(szCpuInfo);        
        ZeroMemory(szCpuInfo, uMaxValue);

        if ( ERROR_SUCCESS == regKey.QueryStringValue("ProcessorNameString", szCpuInfo, &uMaxValue))
        {
            if (uRegIndex > 0)
            {
                strCpu.Append("&");
            }
            // 去掉字符串首尾空格
            strCpu.Append(trim(szCpuInfo));
        }
        uRegIndex++;
        regKey.Close();
    }

    regKey.Close();

    return S_OK;
}

HRESULT CSystemInfo::GetDisplayInfo(CString& strDisplay)
{
	DISPLAY_DEVICE displayInfo;
	DWORD		   dwIndex = 0;

	displayInfo.cb		   = sizeof(DISPLAY_DEVICE);
	displayInfo.StateFlags = 0;

	while (EnumDisplayDevices(NULL, dwIndex, &displayInfo, NULL))
	{
		dwIndex++;
		
		if (0 == _tcslen(displayInfo.DeviceID))
		{
			continue;
		}

		if (strDisplay.GetLength() > 0)
		{
			strDisplay.Append("&");
		}

		strDisplay.Append(displayInfo.DeviceString);
	}
	
	return S_OK;
}

HRESULT CSystemInfo::GetHardDiskId(CString& strHardMd5)
{
	CString strHardId;
	CHAR    szMD5[33] = { 0 };

	int nNet = getnet_mac(strHardId);

	bool bRet = StringMD5(CT2A(strHardId), szMD5, sizeof(szMD5));

	if (bRet)
	{
		strHardMd5.Append(CA2T(szMD5));
	}
	return S_OK;
}

HRESULT CSystemInfo::EncryptString(const CString strSrc, CString& strOut)
{
	HRESULT hResult = E_FAIL;
	CString strMd5Key;
	CHAR    szMD5[33] = { 0 };

	strMd5Key.Append(strSrc);
	strMd5Key.Append(m_strKey);

	bool bRet = StringMD5(CT2A(strMd5Key), szMD5, sizeof(szMD5));

	if (bRet)
	{
		strOut.Append(CA2T(szMD5));
		hResult = S_OK;
	}

	return hResult;
}


HRESULT CSystemInfo::GetDisplayMem(CString& strMem)
{
    HRESULT hResult  = E_FAIL;
	HRESULT hRetCode = E_FAIL;
    int nCoInitFlag = false;
    IWbemLocator  *pWbemLocator = NULL;
    IWbemServices *pWbemServices = NULL;
	IEnumWbemClassObject *pEnumerator = NULL;
    ULONG ulDispalyCardCount = 0;
    IWbemClassObject *pWbemClassObj[MAX_DISPLAY_NUM] = {0};
	VARIANT vtProp;
    CString strNumber;

    hRetCode = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
    KG_COM_PROCESS_ERROR(hRetCode);
    nCoInitFlag = true;

    hRetCode = ::CoCreateInstance(
        CLSID_WbemLocator,
        0, 
        CLSCTX_INPROC_SERVER, 
        IID_IWbemLocator, 
        (LPVOID *)&pWbemLocator
    );
    KG_COM_PROCESS_ERROR(hRetCode);
    KG_PROCESS_ERROR(pWbemLocator);

    hRetCode = pWbemLocator->ConnectServer(
         _bstr_t("\\\\.\\root\\cimv2"), // Object path of WMI namespace
         NULL,                          // User name. NULL = current user
         NULL,                          // User password. NULL = current
         0,                             // Locale. NULL indicates current
         NULL,                          // Security flags.
         0,                             // Authority (e.g. Kerberos)
         0,                             // Context object 
         &pWbemServices               // pointer to IWbemServices proxy
    );
    KG_COM_PROCESS_ERROR(hRetCode);
    KG_PROCESS_ERROR(pWbemServices);

	hRetCode = CoSetProxyBlanket(
		pWbemServices,             // Indicates the proxy to set
		RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
		RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
		NULL,                        // Server principal name 
		RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
		RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
		NULL,                        // client identity
		EOAC_NONE                    // proxy capabilities 
    );
    KG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = pWbemServices->ExecQuery(
        bstr_t("WQL"), 
        L"SELECT * FROM Win32_VideoController",
        WBEM_FLAG_RETURN_IMMEDIATELY | WBEM_FLAG_FORWARD_ONLY, 
        NULL,
        &pEnumerator
    );
    KG_COM_PROCESS_ERROR(hRetCode);
    KG_PROCESS_ERROR(pEnumerator);
    
    hRetCode = pEnumerator->Next(
        WBEM_INFINITE, MAX_DISPLAY_NUM, pWbemClassObj, &ulDispalyCardCount
    );
    KG_COM_PROCESS_ERROR(hRetCode);

	for (unsigned i =0; i < ulDispalyCardCount; i++)
	{
		 hRetCode = pWbemClassObj[i]->Get(L"AdapterRAM", 0, &vtProp, 0, 0);
		 if (FAILED(hRetCode))
			 break;
         strNumber.Format("%d", vtProp.lVal / 1024 / 1024);

         if (i != 0)
			 strMem.Append("&");		 
		 strMem.Append(strNumber);
	}

	hResult = S_OK;
Exit0:
	for (int i = 0; i < MAX_DISPLAY_NUM; i++)
	{
        KG_COM_RELEASE(pWbemClassObj[i]);
	}
    KG_COM_RELEASE(pEnumerator);
    KG_COM_RELEASE(pWbemServices);
    KG_COM_RELEASE(pWbemLocator);
    if (nCoInitFlag)
    {
        CoUninitialize();
        nCoInitFlag = false;
    }
	return hResult;
}
