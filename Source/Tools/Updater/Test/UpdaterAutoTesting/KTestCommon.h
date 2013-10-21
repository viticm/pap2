//////////////////////////////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
//
//  FileName    :   KTestCommon.h    
//  Version     :   1.0
//  Creater     :   dinglong
//  Date        :   2009-2-23 15:00
//  Comment     :   测试工程的公共头文件,主要包含一些公用的字符常量定义和接口
//
//////////////////////////////////////////////////////////////////////////////////////
#ifndef _KTestCommon_
#define _KTestCommon_

//#include <Windows.h>
#include "stdafx.h"
#include <tchar.h>
#include "kglog.h"
#include "UpdateDefine.h"
#include "KFileMD5Maker.h"

//注册表项
const HKEY REG_ROOT_KEY					= HKEY_LOCAL_MACHINE;					//主键
const TCHAR REG_SUB_KEY[]				= _T("SOFTWARE\\Kingsoft\\jx3\\zhcn");	//子键
const TCHAR REG_INSTALL_PATH_NAME[]		= _T("installPath");					//值项，用于记录安装路径
const TCHAR REG_CURRENT_VERSION_NAME[]  = _T("version");						//值项，用于记录版本信息

const TCHAR GAMEUPDATER_PROC_NAME[]	 = _T("gameupdater.exe");//更新程序的进程名

const TCHAR PACKAGE_CONFIG[]		 = _T("package.ini");	//包配置文件名称
const TCHAR PACKAGE_CONFIG_SECTION[] = _T("SO3Client");		//包配置文件section
const TCHAR PACKAGE_CONFIG_KEY0[]	 = _T("0");				//记录pak包名称的key
const TCHAR PACKAGE_CONFIG_KEY1[]	 = _T("1");				//记录pak包名称的key

const TCHAR UPDATE_DLL[]  = _T("update.dll");				//更新接口动态库
const TCHAR UPDATER_SWF[] = _T("update.swf");				//Flash资源文件
const TCHAR TEST_PACK_1[] = _T("update_1.pak");				//合并目标包名称
const TCHAR TEST_PACK_2[] = _T("update_2.pak");				//测试合并包名称
const TCHAR TEST_PACK_3[] = _T("update_3.pak");				//测试分包名称(2G大小)

const TCHAR TEST_RESOURCE_DIR[] = _T("TestingRes");			//测试资源目录名称
const TCHAR TEST_SPACE_DIR[]	= _T("TestingSpace");		//测试空间目录名称
const TCHAR TEMP_UPDATE_DIR[]	= _T("update");				//更新时的临时目录
const TCHAR PAK_DIR[]			= _T("pak");				//更新时的pak包目录

const TCHAR SERVER_NAME[]		= _T("httpsrv.exe");			//自建Http服务器程序名
const TCHAR REMOTE_PATH[]		= _T("http://127.0.0.1/jx3/");	//自建Http服务器根目录
const TCHAR VERSION_LINE_NAME[]	= _T("zhcn");					//版本语言项
const TCHAR DOWNLOAD_URL[]		= _T("http://127.0.0.1/jx3/zhcn/");

const int MAX_TIMEOUT	 = 1000 * 60 * 30;					//等待超时时间
const int MAX_URL_LANGTH = 255;								//URL长度最大值

extern TCHAR g_szAppPath[MAX_PATH];							//程序运行路径
extern TCHAR g_szTestingResDir[MAX_PATH];					//测试资源路径
extern TCHAR g_szTestingSpaceDir[MAX_PATH];					//测试空间路径
extern TCHAR g_szRegInstallPath[MAX_PATH];					//注册表中记录的原始安装目录
extern TCHAR g_szRegVersion[MAX_VERSION_LENGTH];			//注册表中记录的原始版本信息

static const int UPDATE_FILE_COUNT = 10;
//更新的文件名称
static const TCHAR* UPDATE_FILE_NAME[] = 
{
	_T("Dumper.dll"),
	_T("GameConfig.exe"),
	_T("IPPVideo.dll"),
	_T("KG3DEngine.dll"),
	_T("KG3DSound.dll"),
	_T("KGUI.dll"),
	_T("JX3Client.exe"),
	_T("JX3Interaction.dll"),
	_T("JX3Represent.dll"),
	_T("JX3UI.dll")
};
//更新的文件名称对应的MD5值
static const TCHAR* UPDATE_FILE_MD5[] = 
{
	_T("f242e43d6e75f918bed7158977b944d7"),
	_T("b6dbcd1ce99205d6be08a9732466a0cf"),
	_T("b96d3bb1c4a63b018d16541119c277cf"),
	_T("c1dddb6d05f0fbc69884dea8cd253275"),
	_T("9e6eb29d0ea5489c8dc68ff376b75b91"),
	_T("0c986896999eeeb404b220103fb0eb4d"),
	_T("51795e8b0131f3aa760548132d079723"),
	_T("06d212204fb7736ea1567fd91bfa79ef"),
	_T("abfca18725ec5eb68b15e3c35801e7a2"),
	_T("b639db83001bccdcaf16fe185d60134c")
};

namespace KTestCommon
{
	//修改版本信息文件version.cfg中的版本号，即Sword3.version项的值
	int ModifyVersionFile(const TCHAR cszFilePath[], const TCHAR cszVersion[]);
	// 获取当前的版本号
	int GetCurVersion(const TCHAR cszFilePath[], OUT TCHAR* pRetCurVersion);

	//将所需的测试资源从资源目录复制到测试空间指定的目录下
	//cszResName 测试资源文件在资源文件夹下的名称
	//cszTestName 复制后的测试资源文件名（可包含目录，如"pak\\update_1"），如果为空串，则以原资源文件名复制到测试空间根目录下
	int PrepareTestResoure(const TCHAR cszResName[], const TCHAR cszTestName[] = "");

	//清空目录
	int CleanPath(const TCHAR szPath[]);
	int CleanPathExceptLogAndPak(const TCHAR cszPath[]);

	//把目录下所有文件属性修改为只读
	//dwFileAttributes : FILE_ATTRIBUTE_HIDDEN ,FILE_ATTRIBUTE_NORMAL ,FILE_ATTRIBUTE_READONLY, 
	//					 FILE_ATTRIBUTE_SYSTEM,FILE_ATTRIBUTE_TEMPORARY
	int SetAllFilesAttributes(const TCHAR cszPath[], DWORD dwFileAttributes);

	//注册表操作
	int SaveRegData();					//保存版本信息和安装目录
	int RestoreRegData();				//还原版本信息和安装目录
	int ModifyRegKey(const TCHAR cszValueName[], const TCHAR cszValue[]);
	int DelRegValue(const TCHAR cszValueName[]);

	//等待进程启动完毕
	int WaitForProcessStarted(HANDLE hProcess);

	//关闭进程并等待其结束
	int CloseAndWaitForProcess(HANDLE hProcess);

	//验证文件的MD5值
	int ValidateFileMD5(const TCHAR cszFilePath[], const TCHAR cszMD5[MD5_LENGTH]);

	//验证分包后的package.ini的修改
	int ValidatePackageFile();

	//判断一个文件是否存在
	int IsFileExists(const TCHAR* cpszFileName);

	//关闭进程
	enum enumTERMINATEPROCESS
	{
		enumSuccessful = 0,
		enumProcessNameIsEmpty = 1,
		enumGetProccessIDByNameFail = 2,
		enumTerminateProcessFail = 3
	};
	int CloseProcess(const TCHAR* lpProcName);
	bool _GetPidByName(IN const TCHAR* lpName, OUT DWORD& dwPid);
	bool _ExitOtherProcess(IN DWORD dwPid);
}

#endif ///_KTestCommon_