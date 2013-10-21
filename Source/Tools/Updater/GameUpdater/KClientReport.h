#ifndef __KCLIENT_REPORT_H__
#define __KCLIENT_REPORT_H__

#include "SystemInfo.h"
#include "KThread.h"
#include <vector>
#include <string>
#include <set>

#define _PATH_MAX 260
#define _NEW_INSTALL    _T("http://clientreport.jx3.xoyo.com/Client.php?uuid=%s&mode=%d&ver=%s&card=%s&other=%s&type=1")
#define _CHANGE_VERSION _T("http://clientreport.jx3.xoyo.com/Client.php?uuid=%s&ver=%s&type=2")

class KClientReport
{
	enum GAME_NAME
	{	
		GAME_NAME_BEGIN = 0,
		
		GAME_NAME_SD_GAN_DA_OL = GAME_NAME_BEGIN,
		GAME_NAME_CHU_QIU_Q_ZHUAN_OL,
		//GAME_NAME_FS2,
		//GAME_NAME_FS_INTER,
		GAME_NAME_SWORLD1,
		GAME_NAME_SWORLD2,
		GAME_NAME_JXSJ,
		GAME_NAME_MENG_HUAN_XI_YOU,
		GAME_NAME_WOW,
		GAME_NAME_MO_YU,
		GAME_NAME_QI_JI_SHI_JIE,
		GAME_NAME_TIAN_XIA2,
		GAME_NAME_XIN_LONG_QI_SHI_OL,
		//GAME_NAME_XIN_LONG_YING,
		GAME_NAME_AION,
		GAME_NAME_ZHU_XIAN,
		GAME_NAME_DNF,
		GAME_NAME_XIAN_JIAN_OL,
		GAME_NAME_RE_XUE_CHUAN_QI,
		GAME_NAME_TIAN_LONG_BA_BU,
		GAME_NAME_JX2_WZ,

		GAME_NAME_END
	};

public:
	KClientReport(void);
	~KClientReport(void);

public:
	int ReportClientInfo(const TCHAR* pszCurrentVersion);   //check this client is new install?
	int ReportUpdateInfo(const TCHAR* pszCurrentVersion);   //summit client.

private:
	typedef struct _tagGAMECLIENTREGINFO 
	{
		HKEY   RegKey;
		TCHAR* RegSubKey;
		TCHAR* RegValue;
		TCHAR* TestVaule;
	}GAMECLIENTREGINFO, *PGAMECLIENTREGINFO;

	typedef struct _tagGAMEINFO
	{
		GAMECLIENTREGINFO GameRegisterInfo;
		std::vector<std::string> GameExeName;
	}GAMEINFO, *PGAMEINFO;

private:
	int _CheckRegValueExist(PGAMECLIENTREGINFO pRegInfo);
	int _SearchInstalledGames();
	int _GetInstallMode();
	int _WriteUUIDToRegistry();

	int _GetDesktopAndStartmenuShortcutsPoint();
	int _GetPathShortcuts(unsigned uFullPathFileNameLen);
	int _GetShortcutsPoint(TCHAR szShortcutsName[]);

	int DoReport(CString &strUrl);
	static void DoReportFunction(void *pvParam);

private:
	enum INSTALL_MODE{imInstall, imXCopy, imUnknown};
	INSTALL_MODE      m_nInstallMode;  // 0表示正常安装，1标示拷贝安装
	int				  m_nIsFirstRun;

	CSystemInfo       m_SystemInfo;
	GAMEINFO          m_ClientFlags[GAME_NAME_END];	
	CString           m_strVersion;
	CString           m_strVideoCard;
	CString           m_strOtherGame;
	CString           m_strUUID;

	struct THREAD_DATA
	{
		KThread  Thread;
		CString  strParam;
	};
	std::vector<THREAD_DATA *> m_Threads;
	std::set<std::string>      m_ShortcutsPointList;
	TCHAR                      m_szCurrentPath[MAX_PATH + 40];
};


#endif  /* __KCLIENT_REPORT_H__ */`
