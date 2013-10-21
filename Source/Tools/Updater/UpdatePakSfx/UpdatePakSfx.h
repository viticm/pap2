#ifndef _UPDATE_PAK_SFX_H_
#define _UPDATE_PAK_SFX_H_


const TCHAR GAMEUPDATER_FILE_LIST[][MAX_PATH] = {
	_T("gameupdater.exe"), 
	_T("update.dll"),
	_T("gameupdater.ini")
};
const TCHAR GAMEUPDATER_PROCESS[] = _T("gameupdater.exe");
const TCHAR GAME_VERSION_FILE[] = _T("version.cfg");
const TCHAR PACK_UPDATE_FILE_PATH[] = _T("updatepack");

const TCHAR PRODUCT_VERSION_KEY[] = _T("Sword3.version");
const TCHAR PRODUCT_VERSION_LINE_NAME_KEY[] = _T("Sword3.VersionLineName");
const TCHAR PRODUCT_VERSION_FULL_NAME_KEY[] = _T("Sword3.VersionLineFullName");
const TCHAR PRODUCT_VERSION_EX_KEY[] = _T("Sword3.VersionEx");


TCHAR g_szUpdatePakFile[MAX_PATH] = {0};
TCHAR g_szGameInstallPath[MAX_PATH] = {0};
TCHAR g_szGameupdaterFile[MAX_PATH] = {0};
TCHAR g_szPackVersionLineName[MAX_PATH] = {0};
TCHAR g_szPackVersionEx[MAX_PATH] = {0};

//从更新包中解压 updatepack/version.cfg,以确定本更新包所适用的版本
int GetPackVersionInfo();
//从注册表中查找游戏安装目录
int LocateInstallPath();
//检查本更新包是否适合以安装的游戏版本
int CheckVersion();
//解压gameupdater
int ExtractGameupdater();
//启动更新
int Update();
int ShowPackInfo();

int ParseVersionInfo(const TCHAR szVersionFile[], TCHAR *pszVersionLineName, TCHAR *pszVersionEx);
void StringTrim(TCHAR szString[]);


#endif