#include "stdafx.h"

/*#include "toolhelp.h"*/
#include "KClientReport.h"
#include "kglog.h"
#include "md5.h"

#define GAME_CLIENT_REG_KEY(nID, pRegKey, pRegSubKey, pRegValue, pTestVaule)   \
		m_ClientFlags[(nID)].RegKey     = (pRegKey);    \
		m_ClientFlags[(nID)].RegSubKey  = (pRegSubKey); \
		m_ClientFlags[(nID)].RegValue   = (pRegValue);  \
		m_ClientFlags[(nID)].TestVaule  = (pTestVaule);

KClientReport::KClientReport(void)
{
	GAME_CLIENT_REG_KEY(GAME_NAME_SD_GAN_DA_OL,	HKEY_LOCAL_MACHINE,
		_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\"), _T("DisplayName"), _T("SD敢达Online"));

	GAME_CLIENT_REG_KEY(GAME_NAME_CHU_QIU_Q_ZHUAN_OL, HKEY_LOCAL_MACHINE, 
		_T("SOFTWARE\\Kingsoft\\cqonline"), _T("Version"), NULL);

	GAME_CLIENT_REG_KEY(GAME_NAME_SWORLD1, HKEY_LOCAL_MACHINE, 
		_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\剑侠情缘网络版客户端"), _T("URLInfoAbout"), _T("http://www.kingsoft.com"));

	GAME_CLIENT_REG_KEY(GAME_NAME_SWORLD2, HKEY_LOCAL_MACHINE, 
		_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\剑侠情缘网络版2"), _T("DisplayName"), _T("剑侠情缘网络版2"));

	GAME_CLIENT_REG_KEY(GAME_NAME_JXSJ, HKEY_LOCAL_MACHINE, 
		_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\剑侠世界"), _T("DisplayName"), _T("剑侠世界"));

	GAME_CLIENT_REG_KEY(GAME_NAME_MENG_HUAN_XI_YOU, HKEY_LOCAL_MACHINE, 
		_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\梦幻西游_is1"), _T("URLInfoAbout"), _T("http://xyq.163.com/"));

	GAME_CLIENT_REG_KEY(GAME_NAME_WOW, HKEY_LOCAL_MACHINE, 
		_T("SOFTWARE\\Blizzard Entertainment\\World of Warcraft"), _T("GamePath"), NULL);

	GAME_CLIENT_REG_KEY(GAME_NAME_MO_YU, HKEY_LOCAL_MACHINE, 
		_T("SOFTWARE\\Tqdigital\\Soul"), _T("Path"), NULL);

	GAME_CLIENT_REG_KEY(GAME_NAME_QI_JI_SHI_JIE, HKEY_USERS, 
		_T("S-1-5-21-3755760448-3218920860-3279771573-500\\Software\\Ahnlab\\HShield"), _T("packver"), NULL);

	GAME_CLIENT_REG_KEY(GAME_NAME_TIAN_XIA2, HKEY_LOCAL_MACHINE,
		_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\天下贰"), _T("URLInfoAbout"), _T("http://tx2.163.com"));

	GAME_CLIENT_REG_KEY(GAME_NAME_XIN_LONG_QI_SHI_OL, HKEY_LOCAL_MACHINE,
		_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\新龙骑士Online"), _T("URLInfoAbout"), _T("http://www.lqsonline.com.cn"));

	GAME_CLIENT_REG_KEY(GAME_NAME_AION, HKEY_LOCAL_MACHINE,
		_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\AION"), _T("URLInfoAbout"), _T("http://aion.sdo.com"));

	GAME_CLIENT_REG_KEY(GAME_NAME_DNF, HKEY_LOCAL_MACHINE,
		_T("SOFTWARE\\Tencent\\DNF"), _T("Path"), NULL);

	GAME_CLIENT_REG_KEY(GAME_NAME_RE_XUE_CHUAN_QI, HKEY_LOCAL_MACHINE,
		_T("SOFTWARE\\snda\\Legend of mir"), _T("Path"), NULL);

	GAME_CLIENT_REG_KEY(GAME_NAME_XIAN_JIAN_OL, HKEY_LOCAL_MACHINE,
		_T("SOFTWARE\\Microsoft\\WINDOWS\\CURRENTVERSION\\Uninstall\\{0650D9A9-07CC-4297-9040-140BA1084526}"), _T("URLInfoAbou"), _T("http://www.9you.com"));
	
	GAME_CLIENT_REG_KEY(GAME_NAME_ZHU_XIAN, HKEY_CLASSES_ROOT,
		_T("ZPWUpdatePack"), _T("DefaultIcon"), NULL);

	m_nInstallMode = 0;

}

KClientReport::~KClientReport(void)
{
}

int KClientReport::_SearchClient()
{
	int nRetCode = false;

	for (int i = GAME_NAME_BEGIN; i < GAME_NAME_END; ++i)
	{
		nRetCode = _TestRegValueExist(&m_ClientFlags[i]);
		m_strOtherGame.AppendChar(nRetCode + _T('0'));
	}

	return true;
}

int KClientReport::_TestRegValueExist(PGAMECLIENTREGINFO pRegInfo)
{ 
	int   nRetCode  = false;
	int   nResult   = false;
	HKEY  hKey      = NULL;
	DWORD dwSizeReaded = 0;
	TCHAR szValue[_PATH_MAX];

	KG_PROCESS_ERROR(pRegInfo);
	
	nRetCode = RegOpenKeyEx(pRegInfo->RegKey, pRegInfo->RegSubKey, 0, KEY_READ, &hKey);
	KG_PROCESS_ERROR(nRetCode == ERROR_SUCCESS);

	if (pRegInfo->RegValue)
	{
		dwSizeReaded = _PATH_MAX * sizeof(TCHAR);
		nRetCode = RegQueryValueEx(hKey, pRegInfo->RegValue, NULL, NULL, (LPBYTE)szValue, &dwSizeReaded);
		KG_PROCESS_ERROR(nRetCode == ERROR_SUCCESS);
	}

	if (pRegInfo->TestVaule)
	{
		nRetCode = _tcsnicmp(szValue, pRegInfo->TestVaule, _tcslen(pRegInfo->TestVaule));
		KG_PROCESS_ERROR(nRetCode == 0);
	}

	nResult = true;
Exit0:

	RegCloseKey(hKey);
	hKey = NULL;
	return nResult;
}

// int KClientReport::BeforeUpdate(const TCHAR* pszCurrentVersion)
// {
// 	int nResult  = false;
// 	int nRetCode = false;
// 	CString szFullURL;
// 	
// 	KG_PROCESS_ERROR(pszCurrentVersion);
// 	m_LastVersion = pszCurrentVersion;
// 
// 	if (m_nInstallMode == 0)
// 		return true;
// 
// 	szFullURL.Format(_NEW_INSTALL, m_strUUID, 
// 		m_nInstallMode, pszCurrentVersion, m_strVideoCard, m_strOtherGame);
// 
// 	URLDownloadToFile(NULL, szFullURL, _T("a.tmp"), 0, 0);
// 
// 	_tremove(_T("a.tmp"));
// 	
// 	nResult = true;
// Exit0:
// 	return nResult;
// }
// 
// int KClientReport::Init()
// {
// 	HRESULT hRetCode = S_FALSE;
// 	int nRetCode = false;
// 	int nResult = false;
// 
// 	_SearchClient(); //取得other
// 
// 	_TestInstallMode(); //测试安装方式，默认是正常安装的
//  
// 	hRetCode = m_SystemInfo.GetHardDiskId(m_strUUID); //取得UUID
// 	KG_PROCESS_ERROR(hRetCode == S_OK);
// 
// 	_WriteUUIDToRegedit();
// 
// 	hRetCode = m_SystemInfo.GetDisplayInfo(m_strVideoCard); //取得UUID
// 	KG_PROCESS_ERROR(hRetCode == S_OK);
// 
// 	m_SystemInfo.GetSystemCpuInfo(m_strCPUInfo);
// 
// 	nResult = true;
// Exit0:
// 	return nResult;
// }
// 
// int KClientReport::_TestInstallMode()
// {
// 	int nRetCode = false;
// 
// 	GAMECLIENTREGINFO ClientInfo = {HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Kingsoft\\JX3"), NULL, NULL};
// 	GAMECLIENTREGINFO Jx3UnInstall = {HKEY_LOCAL_MACHINE, _T("Microsoft\\Windows\\CurrentVersion\\Uninstall\\剑侠情缘网络版叁"), NULL, NULL};
// 
// 	nRetCode = _TestRegValueExist(&ClientInfo);
// 	if (!nRetCode)
// 	{//返回真就证明是新安装 or 拷贝过来 or 没用过gameupdater的.
// 		nRetCode = _TestRegValueExist(&Jx3UnInstall);
// 		if (!nRetCode)
// 			m_nInstallMode =  1;  //拷贝的
// 	}
// 
// 	return true;
// }
// 
// int KClientReport::AfterUpdate(const TCHAR* pszCurrentVersion)
// {
// 	int nResult  = false;
// 	CString szFullURL;
// 	KVersionNumber NewVersion;
// 
// 	KG_PROCESS_ERROR(pszCurrentVersion);
// 	NewVersion = pszCurrentVersion;
// 
// 	if (NewVersion > m_LastVersion)
// 	{
// 		szFullURL.Format(_CHANGE_VERSION, m_strUUID, pszCurrentVersion);
// 
// 		URLDownloadToFile(NULL, szFullURL, _T("a.tmp"), 0, 0);
// 		_tremove(_T("a.tmp"));
// 	}
// 
// 	nResult = true;
// Exit0:
// 
// 	return nResult;
// }
// 
// int KClientReport::_WriteUUIDToRegedit()
// {
// 	int   nRetCode = false;
// 	int   nResult  = false;
// 	HKEY  hKey     = NULL;
// 	DWORD dwDisp   = 0;
// 	
// 	nRetCode = RegCreateKeyEx(
// 		HKEY_LOCAL_MACHINE, 
// 		"SOFTWARE\\Kingsoft\\JX3", 
// 		0, NULL, REG_OPTION_NON_VOLATILE,
// 		KEY_WRITE, NULL, &hKey, &dwDisp
// 		);
// 	KG_PROCESS_ERROR(nRetCode == ERROR_SUCCESS);
// 
// 	nRetCode = RegSetValueEx(
// 		hKey, "UUID", 0, REG_SZ, (BYTE *)m_strUUID.GetBuffer(), 
// 		m_strUUID.GetLength()
// 		);
// 	KG_PROCESS_ERROR(nRetCode == ERROR_SUCCESS);
// 
// 	nResult = true;
// Exit0:
// 
// 	RegCloseKey(hKey);
// 	hKey = NULL;
// 	return nResult;
// }