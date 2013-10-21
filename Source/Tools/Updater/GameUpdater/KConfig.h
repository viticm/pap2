/*****************************************************************************************
//	FileName:   KConfig.h
//	Author	:   Zhong Guang Ze
//	CreateTime:	2008/12/22-15:06
//  Comment :
//  Copyright : Kingsoft corp
------------------------------------------------------------------------------------------
*****************************************************************************************/
#ifndef _KGAMEPUDATERINI_H__
#define _KGAMEPUDATERINI_H__

#include <vector>
#include "KLastError.h"
#include "UpdateDefine.h"

typedef struct _UPDATE_SERVER
{
	TCHAR szIP[MAX_PATH];
	int   nPort;
	TCHAR szUser[STRING_LENGTH];
	TCHAR szPwd[STRING_LENGTH];
}UPDATE_SERVER;

class KConfig
{
public:
	KConfig(void);
	~KConfig(void);

public:
	int LoadConfig();
	int ReLoadVersion();
	int SaveVersionAndPath();

	const TCHAR *GetCurrentVersion(){return m_szCurrVersion;}
	const TCHAR *GetVersionLineName(){return m_szVersionLineName;}
	const TCHAR *GetUpdatePath(){return m_szSavePath;}
	const TCHAR *GetAppPath(){return m_szAppPath;}
	const TCHAR	*GetRemotePath(){return m_szDownloadURL;}
	const TCHAR *GetVersionName() {return m_szVersionName;}
	const TCHAR *GetBulletinURL() {return m_szBulletinURL;}

private:	
	int  _ReadVersionInfo();
	int  _ReadProfile();
	void _FormatVersion(TCHAR szVersion[], TCHAR cFormat);
	void _StringTrim(TCHAR szString[]);

private:
	UPDATE_SERVER m_UpdateServer;
	TCHAR         m_szAppPath[MAX_PATH];
	TCHAR         m_szSavePath[MAX_PATH];
	TCHAR         m_szCurrVersion[MAX_PATH];
	TCHAR         m_szVersionLineName[MAX_PATH];
	TCHAR         m_szVersionFullName[MAX_PATH];
	TCHAR         m_szVersionEx[MAX_PATH];
	TCHAR         m_szVersionName[MAX_PATH];
	TCHAR         m_szBulletinURL[MAX_PATH];

    TCHAR         m_szDownloadURL[MAX_PATH];
};
#endif // _KGAMEPUDATERINI_H__