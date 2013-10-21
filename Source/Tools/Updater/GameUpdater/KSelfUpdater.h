
///////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
//
// FileName : KSelfUpdater.H
// Creator  : chengzhaohui, zhongguangze
// Date     : 
// Comment  : gameupdater自升级模块
//
///////////////////////////////////////////////////////////////

#ifndef _KSELFUPDATER_H_
#define _KSELFUPDATER_H_

#include "UpdateDefine.h"
#include "KHttpDownloader.h"
#include <vector>

class KSelfUpdater
{
public:
    KSelfUpdater(void);
    ~KSelfUpdater(void);

	//开始自更新
	//cpszRemotePath    远程服务器的URL
	//cpszUpdatePath    升级所需要的临时目录
	//cpszClientVersion 语言版本。目前只有zhcn
	//hWindows	        Flash界面的Handle
	int Start(const TCHAR cpszRemotePath[], const TCHAR cpszUpdatePath[], 
		const TCHAR cpszClientVersion[], HANDLE hWindows = INVALID_HANDLE_VALUE);

    int Stop();
    
private:
	//下载服务器上的 updaterversion.txt 文件
    int DownloadVersionFile();

	//分析下载的updaterversion.txt文件，检查里面所记录的gameupdater版本号是否大于本地的
    int ParseVersionFile();

	//下载updaterversion.txt文件中所记录的文件
    int DownloadUpdateFile();

	//把旧的gameupdater改名并移动到临时目录中
	int _MoveOldUpdaterToTemp();
	
	int _StartupFlashWindow();
	int _CopyNewFileToClientPath();

	//下载的错误处理
	void ProcessDownloadError(const TCHAR szFile[], int nError); 

private:
    KHttpDownloader m_HttpClient;

    tstring m_strApp;
    tstring m_strPath;
    tstring m_strLocalVersionFile;
	TCHAR   m_szRemotePath[MAX_PATH];
	TCHAR   m_szUpdatePath[MAX_PATH];
	TCHAR   m_szClientVersion[MAX_VERSION_LENGTH];
	int     m_nNeedUpdateFlag;

	typedef std::vector<tstring> STRING_VECT;
	STRING_VECT m_SelfFileList;
};

#endif