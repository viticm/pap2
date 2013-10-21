/*****************************************************************************************
//	FileName:   UpdateDefine.h
//	Author	:   Zhong Guang Ze
//	CreateTime:	2008/12/18-11:36
//  Comment :
//  Copyright : Kingsoft corp
------------------------------------------------------------------------------------------
*****************************************************************************************/
#ifndef  _UPDATEDEFINE_H__
#define  _UPDATEDEFINE_H__

#define STRING_LENGTH 32

const int MAX_VERSION_LENGTH    = 32;
const int MAX_PACK_NAME_LENGTH  = 260;
const int MD5_LENGTH = 64;

#ifdef _DEBUG
const TCHAR GAMEUPDATER_EXE_NAME[] = _T("gameupdaterD.exe");
const TCHAR CLIENT_EXE_NAME[] = _T("JX3ClientD.exe");
const TCHAR CONFIG_EXE_NAME[] = _T("GameConfigD.exe");
#else
const TCHAR GAMEUPDATER_EXE_NAME[] = _T("gameupdater.exe");
const TCHAR CLIENT_EXE_NAME[] = _T("JX3Client.exe");
const TCHAR CONFIG_EXE_NAME[] = _T("GameConfig.exe");
#endif

const TCHAR PHYSX_NAME[] = _T("3rdParty\\PhysX_8.10.13_SystemSoftware.exe");
const TCHAR DIRECTX_NAME[] = _T("3rdParty\\DX9\\DXSETUP.exe");
const TCHAR FLASH_NAME[] = _T("3rdParty\\install_flash_player_10_active_x.exe");

const TCHAR CLIENT_START_STRING[] = _T("DOTNOTSTARTGAMEBYJX3CLIENT.EXE");

const char  SWF_EXTRACT_PWD[] = "Nubrefi3h78dfJKdf1DFJsw";
const TCHAR SWF_FILE_NAME[]   = _T("update.swf");
const TCHAR EXTRACT_PATH[]    = _T("update");

const TCHAR FLASH_WINDOWS_FLAG[] = _T("StartupWindows");
const TCHAR SELF_UPDATE_FLAG[]   = _T("StartupSelfUpdate");

const TCHAR SELF_UPDATE_SUCCESS_EVENT[] = _T("__SELF_UPDATE_SUCCESS__");
const TCHAR SELF_UPDATE_FAIL_EVENT[]    = _T("__SELF_UPDATE_FAIL__");

const TCHAR PACK_FILE_HEADER[] = _T("PACK");
const TCHAR PACK_START_PATH[] = _T("updatepack\\");
const TCHAR PACK_SUFFIX[] = _T(".pak");

//*****package.ini******************>>>

const TCHAR PACKAGE_INI_NAME[] = _T("package.ini");
const TCHAR SECTION_PACK_PATH[] = _T("Path");

//<<<*****package.ini******************

//*****updaterversion.txt******************>>>
const TCHAR REMOTE_SELF_UPDATE_FOLDER[] = _T("updaterV2");
const TCHAR REMOTE_SELF_UPDATE_FILE[]   = _T("updaterversion.txt");

const TCHAR SECTION_SELF_VERSION[]      = _T("version");
const TCHAR SECTION_SELF_FILE[]      = _T("filelist");
const TCHAR KEY_SELF_FILE_PREFIX[]      = _T("file_");
const TCHAR KEY_SELF_VERSION_PREFIX[]   = _T("LatestVersion");
//<<<*****updaterversion.txt******************

//***********version.cfg**************>>>
const TCHAR VERSION_FILE_NAME[] = _T("version.cfg");
const TCHAR PRODUCT_VERSION_KEY[] = _T("Sword3.version");
const TCHAR PRODUCT_VERSION_NAME_KEY[] = _T("Sword3.VersionLineName");
const TCHAR PRODUCT_VERSION_EX_KEY[] = _T("Sword3.versionex");
const TCHAR PRODUCT_VERSION_PRODUCT_NAME_KEY[] = _T("Sword3.versionName");
//<<<********version.cfg*****************

//***********gameupdater.ini**************>>>
const TCHAR CONFIG_FILE_NAME[] = _T("gameupdater.ini");
const TCHAR SECTION_SERVER_LIST_PREFIX[] = _T("server_");
const TCHAR SECTION_MASK_LIST_PREFIX[] = _T("ExceptDir_");

const TCHAR GAME_CONFIG_SECTION_NAME[]      = _T("server_1");
const TCHAR KEY_BULLETIN_URL[] = _T("bulletin");
const TCHAR KEY_IP[] = _T("ip");
const TCHAR KEY_PORT[] = _T("port");
const TCHAR KEY_USER[] = _T("user");
const TCHAR KEY_PWD[] = _T("pwd");
//<<<********gameupdater.ini*****************

//*****AutoUpdateEntry.txt********>>>
const TCHAR UPDATE_VERSION_FILE[] = _T("autoupdateentry.txt");

const TCHAR SECTION_ENTRY_VERSION[]  = _T("Version");
const TCHAR SECTION_ENTRY_PACK[]     = _T("PatchList");

const TCHAR KEY_ENTRY_VERSION[]        = _T("LatestVersion");
const TCHAR KEY_ENTRY_PACK_PREFIX[]    = _T("Patch_");
const TCHAR KEY_ENTRY_PACKMD5_PREFIX[] = _T("PatchMD5_");
//<<<*****AutoUpdateEntry.txt********

const TCHAR UPDATE_PACK_NAME_PREFIX[]  = _T("_c_");//_T("sword3_c_");  //jx3_c_xxxx

enum UPDATE_PROGRESS_STATUS
{
	UPDATE_STATUS_SELF_UPGRADING			 = 101,
	UPDATE_STATUS_DOWNLOADING_PACK			 = 102,
	UPDATE_STATUS_CLIENT_UPGRADING			 = 103,
	UPDATE_STATUS_CLIENT_UPGRADE_COMPLETE	 = 104,
	UPDATE_STATUS_NO_NEW_VERSION			 = 105
};

enum UPDATE_FLASH_SHOW_ERROR_MESSAGES
{
	UPDATE_ERR_OK						= 200,
	UPDATE_ERR_LACKING_NECESSARY_FILE   = 201,  //缺少必要的文件，如配置文件等
	UPDATE_ERR_CANNOT_CONNECT_SERVER    = 202,  //无法连接服务器，可能网络有错误
	UPDATE_ERR_DISCONNECTION            = 203,  //网络中断
	UPDATE_ERR_NOT_FOUND_DOWNLOAD_FILE  = 204,  //未找到文件，服务器可能无此更新包文件
	UPDATE_ERR_NO_ENOUGH_DISK_SPACE     = 205,  //磁盘空间不足
	UPDATE_ERR_WRITE_FILE_ERROR         = 206,  //写文件出错
	UPDATE_ERR_NO_AVAILABLE_UPDATE_PACK = 207,  //服务器无可用自动更新包，可能需要手动更新	
	UPDATE_ERR_UPDATE_FAILED            = 208,  //更新失败，如：pak文件不正确、MD5验证失败等
	UPDATE_ERR_MANUAL_PACK_NOT_MATCH    = 209,  //手动更新包不适合当前版本
    UPDATE_ERR_SELF_UPDATE_FAILED       = 210,  // Failed to self update.
};

#endif //  _UPDATEDEFINE_H__