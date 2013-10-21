#include "StdAfx.h"
#include <windows.h>
#include <winsock2.h>
#include "KDumpTransport.h"
#include "toolhelp.h"
#include "KG_HttpClient.h"

KDumpTransport::KDumpTransport(void)
{
}

KDumpTransport::~KDumpTransport(void)
{
}

int KDumpTransport::ReportCrash(
				const TCHAR szServer[], 
				int nPort, 
				const TCHAR szDumpKey[],
				const TCHAR szExceptionModuleName[],
				const TCHAR szProcessName[], 					  
				const TCHAR szFileVersion[],
                const TCHAR szFileVersionEx[],
                const TCHAR szAccount[],
                const TCHAR szRoleName[],
                const TCHAR szRegionName[],
                const TCHAR szServerName[],
                const TCHAR szVideoCard[], 
                const TCHAR szDriver[], 
                const TCHAR szDriverVersion[], 
                const TCHAR szCPUBrand[], 
                DWORD dwTotalPhysMem,
                const TCHAR szGUID[]
)
{
	int nResult  = false;
	int nRetCode = false;
	KG_HttpClient HttpClient;
	char szUUID[64] = {0};
    TCHAR szTotalPhysMem[16] = {0};

    _snprintf(szTotalPhysMem, sizeof(szTotalPhysMem) / sizeof(TCHAR), _T("%d"), dwTotalPhysMem);
    szTotalPhysMem[sizeof(szTotalPhysMem) / sizeof(TCHAR) - 1] = _T('\0');

	GetUUID(szUUID);

	nRetCode = HttpClient.AddPostParam(_T("uuid"), szUUID);
	KG_PROCESS_ERROR(nRetCode);

	nRetCode = HttpClient.AddPostParam(_T("key"), szDumpKey);
	KG_PROCESS_ERROR(nRetCode);

	nRetCode = HttpClient.AddPostParam(_T("exceptionmodule"), szExceptionModuleName);
	KG_PROCESS_ERROR(nRetCode);

	nRetCode = HttpClient.AddPostParam(_T("clientname"), szProcessName);
	KG_PROCESS_ERROR(nRetCode);

	nRetCode = HttpClient.AddPostParam(_T("ver"), szFileVersion);
	KG_PROCESS_ERROR(nRetCode);

    nRetCode = HttpClient.AddPostParam(_T("verex"), szFileVersionEx);
    KG_PROCESS_ERROR(nRetCode);

    nRetCode = HttpClient.AddPostParam(_T("account"), szAccount);
    KG_PROCESS_ERROR(nRetCode);

    nRetCode = HttpClient.AddPostParam(_T("rolename"), szRoleName);
    KG_PROCESS_ERROR(nRetCode);

    nRetCode = HttpClient.AddPostParam(_T("regionname"), szRegionName);
    KG_PROCESS_ERROR(nRetCode);

    nRetCode = HttpClient.AddPostParam(_T("servername"), szServerName);
    KG_PROCESS_ERROR(nRetCode);

    nRetCode = HttpClient.AddPostParam(_T("videocard"), szVideoCard);
    KG_PROCESS_ERROR(nRetCode);

    nRetCode = HttpClient.AddPostParam(_T("driver"), szDriver);
    KG_PROCESS_ERROR(nRetCode);

    nRetCode = HttpClient.AddPostParam(_T("driverversion"), szDriverVersion);
    KG_PROCESS_ERROR(nRetCode);

    nRetCode = HttpClient.AddPostParam(_T("cpubrand"), szCPUBrand);
    KG_PROCESS_ERROR(nRetCode);

    nRetCode = HttpClient.AddPostParam(_T("totalphysmem"), szTotalPhysMem);
    KG_PROCESS_ERROR(nRetCode);

    nRetCode = HttpClient.AddPostParam(_T("guid"), szGUID);
    KG_PROCESS_ERROR(nRetCode);

	nRetCode = HttpClient.AddPostParam(_T("type"), _T("5"));
	KG_PROCESS_ERROR(nRetCode);

	nRetCode = HttpClient.RequestPost(_T("Dumper"), szServer, _T("/dumpinfo.php"), nPort);
	KG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	return nResult;
}

int KDumpTransport::ReportPlayerLoginBase( 
    const TCHAR szServer[], 
    int nPort, 
    const TCHAR szFileVersion[], 
    const TCHAR szFileVersionEx[], 
    const TCHAR szVideoCard[], 
    const TCHAR szDriver[], 
    const TCHAR szDriverVersion[], 
    const TCHAR szCPUBrand[], 
    DWORD dwTotalPhysMem
)
{
    int nResult  = false;
    int nRetCode = false;
    KG_HttpClient HttpClient;
    char szUUID[64] = {0};
    TCHAR szTotalPhysMem[16] = {0};

    _snprintf(szTotalPhysMem, sizeof(szTotalPhysMem) / sizeof(TCHAR), _T("%d"), dwTotalPhysMem);
    szTotalPhysMem[sizeof(szTotalPhysMem) / sizeof(TCHAR) - 1] = _T('\0');

    GetUUID(szUUID);
    KG_PROCESS_ERROR(szUUID[0]);

    nRetCode = HttpClient.AddPostParam(_T("uuid"), szUUID);
    KG_PROCESS_ERROR(nRetCode);

    nRetCode = HttpClient.AddPostParam(_T("ver"), szFileVersion);
    KG_PROCESS_ERROR(nRetCode);

    nRetCode = HttpClient.AddPostParam(_T("verex"), szFileVersionEx);
    KG_PROCESS_ERROR(nRetCode);

    nRetCode = HttpClient.AddPostParam(_T("videocard"), szVideoCard);
    KG_PROCESS_ERROR(nRetCode);

    nRetCode = HttpClient.AddPostParam(_T("driver"), szDriver);
    KG_PROCESS_ERROR(nRetCode);

    nRetCode = HttpClient.AddPostParam(_T("driverversion"), szDriverVersion);
    KG_PROCESS_ERROR(nRetCode);

    nRetCode = HttpClient.AddPostParam(_T("cpubrand"), szCPUBrand);
    KG_PROCESS_ERROR(nRetCode);

    nRetCode = HttpClient.AddPostParam(_T("totalphysmem"), szTotalPhysMem);
    KG_PROCESS_ERROR(nRetCode);

    nRetCode = HttpClient.AddPostParam(_T("type"), _T("5"));
    KG_PROCESS_ERROR(nRetCode);

    nRetCode = HttpClient.RequestPost(_T("Dumper"), szServer, _T("/playerloginbase.php"), nPort);
    KG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}

int KDumpTransport::TransportDumpFile(
					  const TCHAR szServer[],
					  int nPort,
					  const TCHAR szDumpKey[], 
					  const TCHAR szExceptionModuleName[],
					  const TCHAR szProcessName[], 					  
					  const TCHAR szFileVersion[],
					  const TCHAR szFilePathName[],
                      const TCHAR szFileLevel[],
                      const TCHAR szGUID[]
)
{
	int nResult  = false;
	int nRetCode = false;
	KG_HttpClient HttpClient;
	char szUUID[64];
	char szFileName[_MAX_PATH];

    KG_PROCESS_ERROR(szGUID[0]);

	sprintf(szFileName, "%s_%s_%s", szProcessName, szFileVersion, szDumpKey);

	GetUUID(szUUID);
	
	nRetCode = HttpClient.AddPostParam("uuid", szUUID);
	KG_PROCESS_ERROR(nRetCode);

	nRetCode = HttpClient.AddPostParam("key", szDumpKey);
	KG_PROCESS_ERROR(nRetCode);

	nRetCode = HttpClient.AddPostParam("exceptionmodule", szExceptionModuleName);
	KG_PROCESS_ERROR(nRetCode);

	nRetCode = HttpClient.AddPostParam("filename", szProcessName);
	KG_PROCESS_ERROR(nRetCode);

	nRetCode = HttpClient.AddPostParam("ver", szFileVersion);	
	KG_PROCESS_ERROR(nRetCode);

    nRetCode = HttpClient.AddPostParam("filelevel", szFileLevel);
    KG_PROCESS_ERROR(nRetCode);

    nRetCode = HttpClient.AddPostParam("guid", szGUID);
    KG_PROCESS_ERROR(nRetCode);

	nRetCode = HttpClient.AddPostParam("File", szFilePathName, KG_HttpClient::eBinaryFile);
	KG_PROCESS_ERROR(nRetCode);

	nRetCode = HttpClient.RequestPost("Dumper", szServer, "/dumpfile.php", nPort);
	KG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	return nResult;
}
