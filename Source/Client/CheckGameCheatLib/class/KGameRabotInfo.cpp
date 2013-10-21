#include "StdAfx.h"

#include "KGameRabotInfo.h"
#include "KG_HttpClient.h"


#define _TEST 1

#if _TEST
#define REPORT_MACHINE_NAME		"10.20.86.70"
#define REPORT_MACHINE_PHP		"/dumptest/reportgameplug.php"
#define REPORT_MACHINE_PORT		80
#else
#define REPORT_MACHINE_NAME		"clientreport.jx3.xoyo.com"
#define REPORT_MACHINE_PHP		"/reportgameplug.php"
#define REPORT_MACHINE_PORT		80
#endif

std::string WcsToMbs( const std::wstring& wcs );

KGameRabotInfo::KGameRabotInfo(void)
{
	m_pGamePt = NULL;
	m_bCanQuitNow = FALSE;
}

KGameRabotInfo::~KGameRabotInfo(void)
{
	Uninit();
}

BOOL KGameRabotInfo::Uninit()
{
	BOOL bRet = FALSE;

	if (m_pGamePt)
	{  
		m_pGamePt->Uninit();
		m_pGamePt->Release();
		m_pGamePt = NULL;
	}

	return bRet;
}

BOOL KGameRabotInfo::SetPlayerInfo(LPCSTR account, LPCSTR role, LPCSTR region, LPCSTR server)
{
	int nResult  = false;

	KG_PROCESS_ERROR(account);
	KG_PROCESS_ERROR(role);
	KG_PROCESS_ERROR(region);
	KG_PROCESS_ERROR(server);

	KG_PROCESS_ERROR(strlen(account) < sizeof(m_szAccount));
	KG_PROCESS_ERROR(strlen(role) < sizeof(m_szRoleName));
	KG_PROCESS_ERROR(strlen(region) < sizeof(m_szRegionName));
	KG_PROCESS_ERROR(strlen(server) < sizeof(m_szServerName));

	strncpy(m_szAccount, account, strlen(account));
	m_szAccount[strlen(account)] = 0;

	strncpy(m_szRoleName, role, strlen(role));
	m_szRoleName[strlen(role)] = 0;

	strncpy(m_szRegionName, region, strlen(region));
	m_szRegionName[strlen(region)] = 0;

	strncpy(m_szServerName, server, strlen(server));
	m_szServerName[strlen(server)] = 0;

	nResult = true;
Exit0:
	return nResult;
}

BOOL KGameRabotInfo::Init()
{
	HRESULT hr = E_FAIL;

	int nResult  = false;
	int nRetCode = false;

	nRetCode = InitGamePt();
	KG_PROCESS_ERROR(nRetCode);
	
	hr = m_pGamePt->Init(JX3GAMEID);
	nRetCode = SUCCEEDED(hr);
	KG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	return nResult;
}

BOOL KGameRabotInfo::RegCallback()
{
	HRESULT hr   = E_FAIL;
	int nResult  = false;
	int nRetCode = false;

	hr =  m_pGamePt->RegCallback(this);
	nRetCode = SUCCEEDED(hr);
	KG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	return nResult;
}

std::string KGameRabotInfo::StrReplace(std::string &str, const char *string_to_replace, const char *new_string)
{   
	// Find the first string to replace   
	size_t index = str.find(string_to_replace);   
	// while there is one   
	while(index != std::string::npos)   
	{
		// Replace it   
		str.replace(index, strlen(string_to_replace), new_string);   
		// Find the next one   
		index = str.find(string_to_replace, index + strlen(new_string));   
	}
	return str;   
}

STDMETHODIMP KGameRabotInfo::NotifyInfo(DWORD dwGameID, DWORD dwRabotID, const WCHAR* pszGamePath, const WCHAR* pszRabotPath)
{
	HRESULT hResult = S_FALSE;	
	int nRetCode = false;

	std::string strGamePath = "";
	std::string strRabotPath = "";

	KG_PROCESS_ERROR(!m_bCanQuitNow);

	strGamePath = WcsToMbs(pszGamePath);
	strGamePath = StrReplace(strGamePath, "\\", "/");
	strRabotPath = WcsToMbs(pszRabotPath);
	strRabotPath = StrReplace(strRabotPath, "\\", "/");

#ifdef _TEST
	PrintReportInfo( m_szAccount, m_szRoleName, m_szRegionName, m_szServerName, dwRabotID,strGamePath.c_str(), strRabotPath.c_str());
#endif
	
	nRetCode = ReportCheat(m_szAccount, m_szRoleName, m_szRegionName, m_szServerName, dwRabotID, strGamePath.c_str(), strRabotPath.c_str());
	KG_PROCESS_ERROR(nRetCode);

	hResult = S_OK;	
Exit0:
	m_bCanQuitNow = TRUE;
	return hResult;
}

int KGameRabotInfo::ReportCheat(LPCSTR account, LPCSTR role, LPCSTR region, LPCSTR server, DWORD rabotId, LPCSTR lpszGamePath, LPCSTR lpszRabotPath)
{
	int nResult  = false;
	int nRetCode = false;

	KG_HttpClient HttpClient;
	CHAR szRabotId[64] = {0};
	
	KG_PROCESS_ERROR(account);
	KG_PROCESS_ERROR(role);
	KG_PROCESS_ERROR(region);
	KG_PROCESS_ERROR(server);

	nRetCode = HttpClient.AddPostParam("account", account);
	KG_PROCESS_ERROR(nRetCode);

	nRetCode = HttpClient.AddPostParam("role", role);
	KG_PROCESS_ERROR(nRetCode);

	nRetCode = HttpClient.AddPostParam("region", region);
	KG_PROCESS_ERROR(nRetCode);

	nRetCode = HttpClient.AddPostParam("server", server);
	KG_PROCESS_ERROR(nRetCode);
	
	nRetCode = _snprintf(
		szRabotId, 
		sizeof(szRabotId),
		"%d\0", 
		rabotId
		);
	KG_PROCESS_ERROR(nRetCode > 0);

	nRetCode = HttpClient.AddPostParam("robot_id", szRabotId);
	KG_PROCESS_ERROR(nRetCode);

	nRetCode = HttpClient.AddPostParam("game_path", lpszGamePath);
	KG_PROCESS_ERROR(nRetCode);

	nRetCode = HttpClient.AddPostParam("robot_path", lpszRabotPath);
	KG_PROCESS_ERROR(nRetCode);

	nRetCode = HttpClient.RequestPost(TEXT("checkgameplug"), TEXT(REPORT_MACHINE_NAME), TEXT(REPORT_MACHINE_PHP), REPORT_MACHINE_PORT);
	KG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	return nResult;
}

void KGameRabotInfo::PrintReportInfo(LPCSTR account, LPCSTR role, LPCSTR region, LPCSTR server, DWORD dwRabotID, LPCSTR lpszGamePath, LPCSTR lpszRabotPath)
{
	printf("-------------------------\n"		   
		   "\taccount:%s\n"
		   "\trole:%s\n"
		   "\tregion:%s\n"
		   "\tserver:%s\n"
		   "\trabotid:%d\n"
		   "\tgamepath:%s\n"
		   "\trabotpath:%s\n"		   
		   "-------------------------\n", 
		   account, role, region, server, dwRabotID, lpszGamePath, lpszRabotPath);
}

BOOL KGameRabotInfo::InitGamePt()
{
	int nResult  = false;
	int nRetCode = false;
	HRESULT hr = E_FAIL;

	hr = DllGetClassObject(CLSID_GamePtImpl, __uuidof(IGamePt), (LPVOID*)&m_pGamePt);
	nRetCode = SUCCEEDED(hr) && m_pGamePt;
	KG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	return nResult;
}

std::string WcsToMbs( const std::wstring& wcs ) 
{  
	int lengthOfMbs = WideCharToMultiByte( CP_ACP, 0, wcs.c_str(), -1, NULL, 0, NULL, NULL);
	char* mbs = new char[ lengthOfMbs ];
	WideCharToMultiByte( CP_ACP, 0, wcs.c_str(), -1, mbs, lengthOfMbs, NULL, NULL);
	std::string result = mbs;
	delete mbs;
	mbs = NULL;
	return result;
}
