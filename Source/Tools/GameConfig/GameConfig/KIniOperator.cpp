#include "StdAfx.h"
#include "KIniOperator.h"
#include <io.h>
#include "kglog.h"

KIniOperator::KIniOperator(void)
{
	m_bInit = false;
}

KIniOperator::~KIniOperator(void)
{
}

int KIniOperator::Init(const TCHAR* cpszIniName)
{
	int nRetCode = false;
	int nResult  = false;
	int nStrLen  = 0;
	TCHAR* pTemp = NULL;

	KGLOG_PROCESS_ERROR(cpszIniName);

	nRetCode = _waccess_s(cpszIniName, 0);
	KGLOG_PROCESS_ERROR(nRetCode == 0);

	nStrLen = (int)_tcslen(cpszIniName);
	KGLOG_PROCESS_ERROR(nStrLen < MAX_PATH);

	_tcsncpy_s(m_szIniFileName, cpszIniName, MAX_PATH);
	m_szIniFileName[nStrLen] = _T('\0');

	nResult = true;
	m_bInit = true;
Exit0:
	return nResult;
}

int KIniOperator::Uninit()
{
	KGLOG_PROCESS_ERROR(m_bInit);
	memset(m_szIniFileName, 0xCD, sizeof(m_szIniFileName));
	m_bInit = false;

Exit0:
	return true;
}

int KIniOperator::IsSectionExist(const TCHAR* pszSection)
{
	int nRetCode = false;
	int nResult  = false;
	DWORD dwSize = 0;
	TCHAR szTemp[MAX_PATH];
	
	KGLOG_PROCESS_ERROR(pszSection);
	KGLOG_PROCESS_ERROR(m_bInit);
	
	nRetCode = GetPrivateProfileSection(pszSection, szTemp, dwSize, m_szIniFileName);
	KG_PROCESS_SUCCESS(dwSize == -2);
	KGLOG_PROCESS_ERROR(nRetCode);

Exit1:
	nResult = true;
Exit0:
	return nResult;
}

int KIniOperator::GetInteger(const TCHAR* cpszSection, const TCHAR* cpszKeyName, int nDefault, _OUT int* pnValue)
{
	int nRetCode = false;
	int nResult  = false;

	KGLOG_PROCESS_ERROR(m_bInit);
	KGLOG_PROCESS_ERROR(cpszSection);
	KGLOG_PROCESS_ERROR(cpszKeyName);
	KGLOG_PROCESS_ERROR(pnValue);

	*pnValue = GetPrivateProfileInt(cpszSection, cpszKeyName, nDefault, m_szIniFileName);
	
	nResult = true;
Exit0:
	return nResult;
}

int KIniOperator::GetString(
	const TCHAR* cpszSection, 
	const TCHAR* cpszKeyName, 
	const TCHAR* cpszDefault, 
	_OUT TCHAR* pszString, 
	unsigned uSize 
)
{
	int nRetCode = false;
	int nResult  = false;

	KGLOG_PROCESS_ERROR(m_bInit);
	KGLOG_PROCESS_ERROR(cpszSection);
	KGLOG_PROCESS_ERROR(cpszKeyName);
	KGLOG_PROCESS_ERROR(pszString);
	KGLOG_PROCESS_ERROR(uSize > 0);

	nRetCode = GetPrivateProfileString(
			cpszSection,
			cpszKeyName,
			cpszDefault,
			pszString,
			uSize,
			m_szIniFileName
		);
	KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	return nResult;
}

int KIniOperator::WriteInteger(const TCHAR* cpszSection, const TCHAR* cpszKeyName, int nValue)
{
	int nRetCode = false;
	int nResult  = false;
	TCHAR szTemp[MAX_PATH];

	KGLOG_PROCESS_ERROR(m_bInit);
	KGLOG_PROCESS_ERROR(cpszSection);
	KGLOG_PROCESS_ERROR(cpszKeyName);
	
	swprintf_s(szTemp, MAX_PATH, _T("%d"), nValue);
	//_itow_s(nValue, szTemp, MAX_PATH);
	nRetCode = WriteString(cpszSection, cpszKeyName, szTemp);
	
	nResult = true;
Exit0:
	return nResult;
}

int KIniOperator::WriteString(const TCHAR* cpszSection, const TCHAR* cpszKeyName, const TCHAR* cpszString)
{
	int nRetCode = false;
	int nResult  = false;

	KGLOG_PROCESS_ERROR(m_bInit);
	KGLOG_PROCESS_ERROR(cpszSection);
	KGLOG_PROCESS_ERROR(cpszKeyName);
	KGLOG_PROCESS_ERROR(cpszString);

	nRetCode = WritePrivateProfileString(cpszSection, cpszKeyName, cpszString, m_szIniFileName);
	KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	return nResult;
}