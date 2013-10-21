#include "StdAfx.h"
#include "SystemInfo.h"
#include "toolhelp.h"
#include "my_md5.h"

CSystemInfo::CSystemInfo(void) :
m_strKey(L"92A73EB6-FA72-40f9-AFD2-BD0376F40FD0")
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
    BOOL            bOsVersionInfoEx;

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
        strVersion.Append(L"Microsoft Windows 2000 ");
    }
    else if ( 5 == osvi.dwMajorVersion && 1 == osvi.dwMinorVersion )
    {
        strVersion.Append(L"Microsoft Windows XP ");
    }
    else if ( 6 == osvi.dwMajorVersion && 0 == osvi.dwMinorVersion )
    {
        strVersion.Append(L"Microsoft Windows Vista ");
    }
    else
    {
        strVersion.Append(L"Microsoft Windows NT ");
    }

    // windows 版本类型
    if ( osvi.wProductType == VER_NT_WORKSTATION )
    {
        if( osvi.wSuiteMask & VER_SUITE_PERSONAL )
        {
            strVersion.Append(L"Personal ");
        }
        else
        {
            strVersion.Append(L"Professional ");
        }
    }
    else if ( osvi.wProductType == VER_NT_SERVER )
    {
        if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
        {
            strVersion.Append(L"DataCenter Server ");
        }
        else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
        {
            strVersion.Append(L"Advanced Server ");
        }
        else
        {
            strVersion.Append(L"Server ");
        }
    }
    
    // 版本号
    verNumber.Format(L"%s", osvi.szCSDVersion);

    strVersion.Append(verNumber);

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
    dSize /= 1024.0;
    dSize += 0.5;

    strMem.Format(L"%.0f", dSize);

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
    WCHAR   szCpuInfo[MAX_PATH] = { 0 };
    CString strRegKey = L"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\%d";

    while (TRUE)
    {
        strRegOpen.Format(strRegKey, uRegIndex);

        if ( ERROR_SUCCESS != regKey.Open(HKEY_LOCAL_MACHINE, strRegOpen) )
        {
            break;
        }

        uMaxValue = sizeof(szCpuInfo);        
        ZeroMemory(szCpuInfo, uMaxValue);

        if ( ERROR_SUCCESS == regKey.QueryStringValue(L"ProcessorNameString", szCpuInfo, &uMaxValue))
        {
            if (uRegIndex > 0)
            {
                strCpu.Append(L"&");
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
			strDisplay.Append(L"&");
		}

		strDisplay.Append(displayInfo.DeviceString);
	}
	
	return S_OK;
}

HRESULT CSystemInfo::GetHardDiskId(CString& strHardMd5)
{
	HRESULT hResult = E_FAIL;
	CString strHardId;
	CHAR    szMD5[33] = { 0 };

	int nNet = getnet_mac(strHardId);

	bool bRet = StringMD5(CT2A(strHardId), szMD5, sizeof(szMD5));

	if (bRet)
	{
		strHardMd5.Append(CA2T(szMD5));
		hResult = S_OK;
	}

	return hResult;
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