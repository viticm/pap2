// KInformationDlg.cpp : implementation file
//

#include "stdafx.h"

#include <Setupapi.h>
#include <devguid.h>
#include <winsock2.h>
#include <d3d9.h>
#include <ddraw.h>
#include <comutil.h>
#include <io.h>

#include "KGPublic.h"
#include "kglog.h"

#include "KClientEnvironment.h"

#include "KVersionNumber.h"
#include "KFileVersionInfo.h"

#define SAFE_RELEASE(a) if (a) {(a)->Release(); (a)=NULL;}

IMPLEMENT_DYNAMIC(KClientEnvironment, CDialog)

// class KOleInitialize
// {
// public:
// 	KOleInitialize(){OleInitialize(NULL); }
// 	~KOleInitialize(){OleUninitialize();}
// }g_OleInit;

struct DXVERSION
{
	TCHAR szVersion[MAX_PATH];
	TCHAR szDescription[MAX_PATH];
} DxVersionList[] = 
{
	{_T("4.08.01.0810"), _T("DirectX 8.1")},
	{_T("4.08.01.0881"), _T("DirectX 8.1")},
	{_T("4.08.01.0901"), _T("DirectX 8.1a")},
	{_T("4.08.02.0134"), _T("DirectX 8.2")},
	{_T("4.09.00.0900"), _T("DirectX 9.0")},
	{_T("4.09.00.0901"), _T("DirectX 9.0a")},
	{_T("4.09.00.0902"), _T("DirectX 9.0b")},
	{_T("4.09.00.0903"), _T("DirectX 9.0c")},
	{_T("4.09.00.0904"), _T("DirectX 9.0c")},
	//以下是Dxdiag.exe程序的版本号
	{_T("6.00.6000.16386"), _T("DirectX 10")},
	{_T("6.00.6001.18000"), _T("DirectX 10")},
	{_T("6.01.7000.0000"), _T("DirectX 11")}
};

KClientEnvironment::KClientEnvironment(CWnd* pParent /*=NULL*/)
	: CDialog(KClientEnvironment::IDD, pParent)
{
	m_nInited = false;
}

KClientEnvironment::~KClientEnvironment()
{
}

void KClientEnvironment::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

HRESULT KClientEnvironment::_GetEnumerator(IEnumWbemClassObject **ppEnumerator, char szEnumName[128])
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
	KG_COM_PROCESS_ERROR(hrRetCode);

	*ppEnumerator = pEnumWbemClassObject;
	pEnumWbemClassObject = NULL;

	hrResult = S_OK;
Exit0:
	SAFE_RELEASE(pEnumWbemClassObject);

	return hrResult;
}

int KClientEnvironment::Init()
{
	int nRetCode = false;
	int nResult  = false;

	KG_PROCESS_SUCCESS(m_nInited);

	nRetCode = _SetVideoRAM();
	//KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = _SetCPUName();
	//KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = _SetMemory();
	//KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode =_SetDisplayCard();
	//KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = _SetOSVersion();
	//KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = _SetBrowserName();
	//KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = _SetIP();
	//KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = _SetDxVersion();
	//KGLOG_PROCESS_ERROR(nRetCode);

Exit1:
	m_nInited = true;
	nResult = true;
Exit0:	
	return nResult;
}

BEGIN_MESSAGE_MAP(KClientEnvironment, CDialog)
END_MESSAGE_MAP()

int KClientEnvironment::_SetCPUName()
{
	int nRetCode = false;
	int nResult  = false;

	TCHAR szCpuName[MAX_PATH];
	HKEY  hKey = NULL;
	DWORD dwType = 0;
	DWORD dwSize = 0;

	nRetCode = RegOpenKeyEx(CPU_REG_KEY, CPU_REG_SUBKEY, 0, KEY_READ, &hKey);
	KGLOG_PROCESS_ERROR(nRetCode == S_OK);

	nRetCode = RegQueryValueEx(hKey, CPU_NAME, NULL, &dwType, NULL, &dwSize);
	KGLOG_PROCESS_ERROR(dwSize > 0);

	nRetCode = RegQueryValueEx(hKey, CPU_NAME, NULL, &dwType, (LPBYTE)szCpuName, &dwSize);
	KGLOG_PROCESS_ERROR(nRetCode == S_OK);

	GetDlgItem(IDC_INFO_CPU)->SetWindowText(szCpuName);
	
	nResult = true;
Exit0:
	if (!nResult)
		GetDlgItem(IDC_INFO_CPU)->SetWindowText(L"UnKnow");

	return nResult;
}

int KClientEnvironment::_SetMemory()
{
	double var = 0.0;
	CString strMemory;
	MEMORYSTATUSEX statex;

	memset(&statex, 0, sizeof(MEMORYSTATUSEX));
	statex.dwLength = sizeof(statex);
	
	GlobalMemoryStatusEx(&statex);
	var = (((double)statex.ullTotalPhys) / 1024) / 1024;
	strMemory.Format(L"%7.0f", var);
	strMemory += _T(" MB");
	strMemory.TrimLeft();

	GetDlgItem(IDC_INFO_MEMORY)->SetWindowText(strMemory);

	return true;
}

int KClientEnvironment::_SetDisplayCard()
{
	int nRetCode = false;
	int nResult  = false;
	HDEVINFO hDevInfo = NULL;
	DWORD dwIndex = 1;
	int   nIndex = 0;
	SP_DEVINFO_DATA DevInfo_data;

	ZeroMemory(&DevInfo_data, sizeof(SP_DEVINFO_DATA));
	DevInfo_data.cbSize = sizeof(SP_DEVINFO_DATA);

	hDevInfo = SetupDiGetClassDevs(
		(LPGUID) &GUID_DEVCLASS_DISPLAY,
		0, // Enumerator
		0,
		DIGCF_PRESENT
	);
	KGLOG_COM_PROCESS_ERROR(hDevInfo == INVALID_HANDLE_VALUE);

	while (true)
	{
		DWORD DataT;
		LPTSTR buffer = NULL;
		DWORD buffersize = 0;
		CString strFinder;

		nRetCode = SetupDiEnumDeviceInfo(hDevInfo, nIndex, &DevInfo_data);
		if (!nRetCode)
			break;

		while (true)
		{
			long lRetCode = false;
			nRetCode = SetupDiGetDeviceRegistryProperty(
				hDevInfo,
				&DevInfo_data,
				SPDRP_DEVICEDESC,
				&DataT,
				(PBYTE)buffer,
				buffersize,
				&buffersize);
			if (nRetCode)
				break;

			lRetCode = GetLastError();
			if (lRetCode == ERROR_INSUFFICIENT_BUFFER)
			{
				if (buffer) 
					LocalFree(buffer);
				buffer = (LPTSTR)LocalAlloc(LPTR, buffersize);
			}
			else
			{
				if (buffer) 
					LocalFree(buffer);

				break;
			}
		}

		if (nIndex < 1)
			GetDlgItem(IDC_INFO_DISPLAY_CARD)->SetWindowText(CString(buffer));
		else
			GetDlgItem(IDC_INFO_DISPLAY_CARD2)->SetWindowText(buffer);

		if (buffer) 
			LocalFree(buffer);

		nIndex++;
	}

	nResult = true;
Exit0:
	if (!nResult)
		GetDlgItem(IDC_INFO_DISPLAY_CARD)->SetWindowText(L"UnKnow");

	return nResult;
}

int KClientEnvironment::_SetOSVersion()
{
	int nRetCode = false;
	OSVERSIONINFOEX osvi;
	BOOL bOsVersionInfoEx = FALSE;
	CString strVerion;

	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	bOsVersionInfoEx = GetVersionEx((OSVERSIONINFO *)&osvi);
	if( !bOsVersionInfoEx)
	{
		osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
		nRetCode = GetVersionEx((OSVERSIONINFO *) &osvi);
		if (!nRetCode) 
			return false;
	}

	switch (osvi.dwPlatformId)
	{
	case VER_PLATFORM_WIN32_NT:
		if ( osvi.dwMajorVersion <= 4 )
			strVerion = L"Microsoft Windows NT ";

		if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0 )
			strVerion = L"Microsoft Windows 2000 ";

		if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1 )
			strVerion = L"Microsoft Windows XP ";

		if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2 )
			if (GetSystemMetrics(SM_SERVERR2) != 0)
				strVerion = L"Microsoft Windows 2003 RC2 ";
			else
				strVerion = L"Microsoft Windows 2003 ";

		if (osvi.dwMajorVersion == 6)
        {
            if(osvi.dwMinorVersion == 0)
            {
			    if (osvi.wProductType == VER_NT_WORKSTATION)
				    strVerion = _T("Microsoft Windows Vista");
			    else if (osvi.wProductType != VER_NT_WORKSTATION)
				    strVerion = _T("Microsoft Windows 2008");
            }
            else if (osvi.dwMinorVersion == 1)
            {
                strVerion = _T("Windows 7");
            }
        }

		if( bOsVersionInfoEx )
		{
			if ( osvi.wProductType == VER_NT_SERVER )
			{
				if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
					strVerion += L"DataCenter Server ";
				else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
					strVerion += L"Advanced Server ";
				else
					strVerion += L"Server ";
			}
		}
		else
		{
			HKEY hKey = NULL;
			char szProductType[80];
			DWORD dwBufLen = 0;

			RegOpenKeyEx( HKEY_LOCAL_MACHINE,
				L"SYSTEM\\CurrentControlSet\\Control\\ProductOptions",
				0, KEY_QUERY_VALUE, &hKey );
			RegQueryValueEx( hKey, L"ProductType", NULL, NULL,
				(LPBYTE) szProductType, &dwBufLen);

			RegCloseKey(hKey);
			hKey = NULL;

			if ( lstrcmpi( L"WINNT", (LPCWSTR)szProductType) == 0 )
				strVerion += L"Professional ";
			if ( lstrcmpi( L"LANMANNT", (LPCWSTR)szProductType) == 0 )
				strVerion += L"Server ";
			if ( lstrcmpi( L"SERVERNT", (LPCWSTR)szProductType) == 0 )
				strVerion += L"Advanced Server ";
		}

		if ( osvi.dwMajorVersion <= 4 )
		{
			CString str;
			str.Format(L"%s version %d.%d %s (Build %u)\n",
				strVerion, 
				osvi.dwMajorVersion,
				osvi.dwMinorVersion,
				osvi.szCSDVersion,
				osvi.dwBuildNumber & 0xFFFF);
			strVerion = str;
		}
		else
		{ 
			CString str;
			str.Format(L"%s %s (Build %u)\n",
				strVerion, 
				osvi.szCSDVersion,
				osvi.dwBuildNumber & 0xFFFF);
			strVerion = str;
		}
		break;

	case VER_PLATFORM_WIN32_WINDOWS:

		if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 0)
		{
			strVerion = L"Microsoft Windows 95 ";
			if ( osvi.szCSDVersion[1] == 'C' || osvi.szCSDVersion[1] == 'B' )
				strVerion += L"OSR2 ";
		} 

		if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 10)
		{
			strVerion = L"Microsoft Windows 98 ";
			if ( osvi.szCSDVersion[1] == 'A' )
				strVerion += L"SE ";
		} 

		if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 90)
		{
			strVerion = L"Microsoft Windows Me ";
		} 
		break;

	case VER_PLATFORM_WIN32s:
		strVerion = L"Microsoft Win32s ";
		break;
	default:
		strVerion = L"UnKnow OS";
		break;
	}
	
	GetDlgItem(IDC_INFO_OS)->SetWindowText(strVerion);

	return true;
}

int KClientEnvironment::_SetBrowserName()
{
	int nRetCode = false;
	int nResult  = false;

	TCHAR szBrowserName[MAX_PATH];
	CString strTemp = L"Internet Explorer ";
	HKEY hKey = NULL;
	DWORD dwType = 0;
	DWORD dwSize = 0;
	LONG lRetCode = 0;

	lRetCode = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Internet Explorer", 
		0, KEY_READ, &hKey);
	KGLOG_PROCESS_ERROR(lRetCode == S_OK);

	lRetCode = RegQueryValueEx(hKey, L"Version", NULL, &dwType, NULL, &dwSize);
	KGLOG_PROCESS_ERROR(dwSize > 0);

	lRetCode = RegQueryValueEx(hKey, L"Version", NULL, &dwType, (LPBYTE)szBrowserName, &dwSize);
	KGLOG_PROCESS_ERROR(lRetCode == S_OK);

	GetDlgItem(IDC_INFO_BROWSER)->SetWindowText(strTemp + szBrowserName);

	nResult = true;
Exit0:
	if (!nResult)
		GetDlgItem(IDC_INFO_BROWSER)->SetWindowText(L"UnKnow");

	return nResult;
}

BOOL KClientEnvironment::PreTranslateMessage(MSG* pMsg)     
{//注意在屏蔽ESC的时候要提示是否有修改
	if (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN)      
	{     
		return TRUE;
	}    
	return CDialog::PreTranslateMessage(pMsg); 

}   

int KClientEnvironment::_SetVideoRAM()
{
	HRESULT hrRetCode = S_FALSE;
	int nResult  = false;
	int nRetCode = false;
	TCHAR szVideoRam[32] = {0};
	VARIANT vtProp;
	long lValue = 0;
	IWbemClassObject *pWbemClassObj = NULL;
	IEnumWbemClassObject *pEnumerator = NULL;

	hrRetCode = ::CoInitializeEx(0, COINIT_MULTITHREADED);
	KG_COM_PROCESS_ERROR(hrRetCode);
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
	KG_COM_PROCESS_ERROR(hrRetCode);

	hrRetCode = ::CoCreateInstance(
		CLSID_WbemLocator,             
		0, 
		CLSCTX_INPROC_SERVER, 
		IID_IWbemLocator, 
		(LPVOID *)&m_pWbemLocator
		);
	KG_COM_PROCESS_ERROR(hrRetCode);

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
	KG_COM_PROCESS_ERROR(hrRetCode);

	hrRetCode = _GetEnumerator(&pEnumerator, "Win32_VideoController");
	KG_COM_PROCESS_ERROR(hrRetCode);
	hrRetCode = pEnumerator->Next(WBEM_INFINITE, 1, &pWbemClassObj, (ULONG *)&nRetCode);
	KG_COM_PROCESS_ERROR(hrRetCode);
	hrRetCode = pWbemClassObj->Get(L"AdapterRAM", 0, &vtProp, 0, 0);
	KG_COM_PROCESS_ERROR(hrRetCode);
	if (SUCCEEDED(hrRetCode))
	{
		lValue = vtProp.lVal;
	}
	else
	{
		lValue = 0;
	}

	swprintf_s(szVideoRam, 32, _T("%dMB"), lValue / 1024 / 1024);
	GetDlgItem(IDC_INFO_DISPLAY_MEMORY)->SetWindowText(szVideoRam);

	nResult = true;
Exit0:
	if (nResult)
	{
		SAFE_RELEASE(m_pWbemServices);
		SAFE_RELEASE(m_pWbemLocator);
	}
	::CoUninitialize();

	return nResult;
}

int KClientEnvironment::_SetIP()
{
	WSADATA wsaData;
	char name[MAX_PATH];
	char* pszIp = NULL; 
	PHOSTENT hostinfo = 0; 

	if (WSAStartup(MAKEWORD(2,2), &wsaData) == 0 ) 
	{ 
		if(gethostname(name, sizeof(name)) == 0) 
		{ 
			if((hostinfo = gethostbyname(name)) != NULL)
			{ 
				pszIp = inet_ntoa (*(struct in_addr *)*hostinfo->h_addr_list); 
			}
		}
	}

	if (pszIp)
		GetDlgItem(IDC_INFO_IP)->SetWindowText(CString(pszIp));

	WSACleanup();
	return true;
}

int KClientEnvironment::_SetDxVersion()
{
	int nRetCode = false;
	int nResult  = false;
	int nVersionID = -1;
	TCHAR szDxVersion[MAX_PATH];

	nRetCode = _GetDxdiagVersion(szDxVersion);	
	if (nRetCode)
	{
		nRetCode = _FindDxVersion(szDxVersion, nVersionID);
		KGLOG_PROCESS_ERROR(nRetCode);
	}

	if (nVersionID == -1)
	{
		nRetCode = _GetDxVersionInRegister(szDxVersion);
		KGLOG_PROCESS_ERROR(nRetCode);
		
		nRetCode = _FindDxVersion(szDxVersion, nVersionID);
		//KGLOG_PROCESS_ERROR(nRetCode);
		KGLOG_PROCESS_ERROR(nVersionID != -1);
	}

	GetDlgItem(IDC_INFO_DX)->SetWindowText(CString(DxVersionList[nVersionID].szDescription));
	nResult = true;
Exit0:
	if (!nResult)
		GetDlgItem(IDC_INFO_DX)->SetWindowText(CString("UnKnow"));

	return nResult;
}

int KClientEnvironment::_GetDxdiagVersion(TCHAR szFileVersion[])
{
	int nRetCode = false;
	int nResult  = false;
	TCHAR szDxdiagPath[MAX_PATH];
	int  nVerLen = 0;
	KFileVersionInfo FileVer;

	::GetSystemDirectory(szDxdiagPath, MAX_PATH);
	_tcscat(szDxdiagPath, _T("\\dxdiag.exe"));

	nRetCode = _taccess(szDxdiagPath, 0);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	
	nRetCode = FileVer.GetFileVerInfo(szDxdiagPath);
	KGLOG_PROCESS_ERROR(nRetCode);

	nVerLen = _tcslen(FileVer.FileVersion());

	_tcsncpy(szFileVersion, FileVer.FileVersion(), nVerLen);
	szFileVersion[nVerLen] = _T('\0');
	
	nResult = true;
Exit0:
	return nResult;
}

int KClientEnvironment::_FindDxVersion(const TCHAR szDxVersion[], int& nID)
{
	int nRetCode = false;
	int nResult  = false;
	int nCount   = 0;
	KVersionNumber FileVer(szDxVersion);
	KVersionNumber DxVer;

	nID = -1; //not find
	nCount = sizeof(DxVersionList) / sizeof(DxVersionList[0]);

	for (int i = 0; i < nCount; ++i)
	{
		DxVer = DxVersionList[i].szVersion;
		if (DxVer == FileVer)
		{
			nID = i;
			break;
		}
	}	

	nResult = true;
Exit0:
	return nResult;
}

int KClientEnvironment::_GetDxVersionInRegister(TCHAR szDxVersion[])
{
	int nRetCode = false;
	int nResult  = false;
	int nLen     = 0;
	DWORD dwSize = MAX_PATH;
	TCHAR szDxInfo[MAX_PATH];
	HKEY  hKey   = NULL;

	nRetCode = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("Software\\Microsoft\\DirectX"), 0, KEY_QUERY_VALUE, &hKey);
	KGLOG_PROCESS_ERROR(nRetCode == ERROR_SUCCESS);

	nRetCode = RegQueryValueEx(hKey, _T("Version"), NULL, NULL, (LPBYTE)szDxInfo, &dwSize);
	KGLOG_PROCESS_ERROR(!nRetCode);

	nLen = _tcslen(szDxInfo);
	_tcsncpy(szDxVersion, szDxInfo, nLen);
	szDxVersion[nLen] = _T('\0');

	nResult = true;
Exit0:
	if (hKey)
	{
		RegCloseKey(hKey);
		hKey = NULL;
	}
	return nResult;
}