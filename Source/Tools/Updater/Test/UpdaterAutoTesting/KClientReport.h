#ifndef __KCLIENT_REPORT_H__
#define __KCLIENT_REPORT_H__

/*#include "SystemInfo.h"*/
/*#include "KVersionNumber.h"*/
#include "atlstr.h"

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
		//GAME_NAME_TIAN_LONG_BA_BU,
		GAME_NAME_TIAN_XIA2,
		GAME_NAME_XIN_LONG_QI_SHI_OL,
		//GAME_NAME_XIN_LONG_YING,
		GAME_NAME_AION,
		GAME_NAME_ZHU_XIAN,
		GAME_NAME_DNF,
		GAME_NAME_RE_XUE_CHUAN_QI,
		GAME_NAME_XIAN_JIAN_OL,

		GAME_NAME_END
	};

public:
	KClientReport(void);
	~KClientReport(void);

public:
	
// 	int Init();
// 	int GetInstallMode() {return m_nInstallMode;}
// 	int GetCpuCoreNum() {return m_SystemInfo.GetCpuCoreNum();}
// 	int BeforeUpdate(const TCHAR* pszCurrentVersion);  //check this client is new install?
// 	int AfterUpdate(const TCHAR* pszCurrentVersion);   //summit client.


/**************************丁隆添加的测试接口*************************************/

	CString GetOtherGameFlag(){return m_strOtherGame;}
	int SerchClient(){return _SearchClient();}

/**************************丁隆添加的测试接口*************************************/

private:
	typedef struct _tagGAMECLIENTREGINFO 
	{
		HKEY   RegKey;
		TCHAR* RegSubKey;
		TCHAR* RegValue;
		TCHAR* TestVaule;
	}GAMECLIENTREGINFO, *PGAMECLIENTREGINFO;

private:
	int _TestRegValueExist(PGAMECLIENTREGINFO pRegInfo);
	int _SearchClient();
// 	int _TestInstallMode();
// 	int _WriteUUIDToRegedit();

private:
/*	CSystemInfo       m_SystemInfo;*/
	GAMECLIENTREGINFO m_ClientFlags[GAME_NAME_END];
	int               m_nInstallMode;  // 0表示正常安装，1标示拷贝安装
	CString           m_strVersion;
	CString           m_strVideoCard;
	CString           m_strOtherGame;
	CString           m_strUUID;
	CString           m_strCPUInfo;
/*	KVersionNumber    m_LastVersion;*/
	//int               m_nType;
};


#endif  /* __KCLIENT_REPORT_H__ */`
