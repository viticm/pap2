#include "StdAfx.h"
#include "ReadConfig.h"
#include "KGLog.h"


ReadConfig::ReadConfig(void)
{
}


ReadConfig::~ReadConfig(void)
{
}


bool ReadConfig::Init()
{

	return true;
}

bool ReadConfig::UnInit()
{
	return true;
}

bool ReadConfig::ReadConfigFile(const char cszFileName[])
{
	bool bResult = false;
	bool bRetCode = false;
	char szSection[MAX_PATH];

	KGLOG_PROCESS_ERROR(cszFileName);

	memset(szSection, 0, sizeof(szSection));
	memcpy(szSection, "Path", sizeof(szSection));
	
	//get Pak file Path
	bRetCode = GetPrivateProfileString(
	(LPCTSTR)szSection, 
	_T("PakPath"),
	_T(""), 
	(LPTSTR )m_szPakPath, 
	sizeof(m_szPakPath),
	(LPCTSTR )cszFileName
	);
	KGLOG_PROCESS_ERROR(bRetCode > 0);

	//get extract File Path
	bRetCode = GetPrivateProfileString(
	(LPCTSTR)szSection, 
	_T("ExtractFilePath"),
	_T(""), 
	(LPTSTR )m_szExtractPath, 
	sizeof(m_szExtractPath),
	(LPCTSTR )cszFileName
	);
	KGLOG_PROCESS_ERROR(bRetCode > 0);

	bResult = true;
Exit0:
	return bResult;
}