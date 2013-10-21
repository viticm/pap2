#pragma once

#ifndef _KGAME_RABOT_INFO_H
#define _KGAME_RABOT_INFO_H


#include "kiscom_v2/KISCOM_v2.h"
#include "interface/igameptcallback.h"
#include "interface/igamept.h"

#define JX3GAMEID 1120
#define MAX_LEN 256

// {2DB25D40-707C-4da4-8C5B-602422191861}
__declspec(selectany) GUID CLSID_GamePtRevImpl = 
{ 0x2db25d40, 0x707c, 0x4da4, { 0x8c, 0x5b, 0x60, 0x24, 0x22, 0x19, 0x18, 0x61 } };

class KGameRabotInfo :	public IGamePtCallback
{
public:
	KGameRabotInfo(void);
	~KGameRabotInfo(void);
public:
	KIS_DEFINE_GETCLSID(CLSID_GamePtRevImpl);

	DECLARE_KISCOM_OBJECT();
	KISCOM_QUERY_BEGIN()
		KISCOM_QUERY_ENTRY(IGamePtCallback)
		KISCOM_QUERY_END()
public:
	BOOL Init();

	BOOL SetPlayerInfo(LPCSTR account, LPCSTR role, LPCSTR region, LPCSTR server);

	BOOL RegCallback();

	BOOL Uninit();

	virtual STDMETHODIMP NotifyInfo(
		DWORD dwGameID, 
		DWORD dwRabotID, 
		const WCHAR* pszGamePath,
		const WCHAR* pszRabotPath);

public:
	BOOL m_bCanQuitNow;

private:
	IGamePt* m_pGamePt;
	CKISUseComDll_v2 m_GamePtLoad;

	CHAR m_szAccount[MAX_LEN];
	CHAR m_szRoleName[MAX_LEN];
	CHAR m_szRegionName[MAX_LEN];
	CHAR m_szServerName[MAX_LEN];
	
private:
	
	int ReportCheat(LPCSTR account, LPCSTR role, LPCSTR region, LPCSTR server, DWORD rabotId, LPCSTR lpszGamePath, LPCSTR lpszRabotPath);
	void PrintReportInfo(LPCSTR account, LPCSTR role, LPCSTR region, LPCSTR server, DWORD dwRabotID, LPCSTR lpszGamePath, LPCSTR lpszRabotPath);
	BOOL InitGamePt();
	std::string StrReplace(std::string &str, const char *string_to_replace, const char *new_string);

};
#endif//#ifndef _KGAME_RABOT_INFO_H
