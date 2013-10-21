///////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
//
// FileName : KVersionNumber.cpp
// Creator  : chengzhaohui
// Date     : 2008-3-27
// Comment  :
//
///////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "KVersionNumber.h"
#include "kglog.h"

KVersionNumber::KVersionNumber(const TCHAR szVersion[])
{
	m_nVersionSect1 = 0;
	m_nVersionSect2 = 0;
	m_nVersionSect3 = 0;
	m_nVersionSect4 = 0;
	m_cType = _T('.');

	Parse(szVersion);
}

KVersionNumber::KVersionNumber()
{
	m_nVersionSect1 = 0;
	m_nVersionSect2 = 0;
	m_nVersionSect3 = 0;
	m_nVersionSect4 = 0;
	m_cType = _T('.');
}
KVersionNumber::~KVersionNumber(void)
{
}

int KVersionNumber::operator==(const KVersionNumber &rVersion)
{
	if (
		(m_nVersionSect1 == rVersion.m_nVersionSect1) &&
		(m_nVersionSect2 == rVersion.m_nVersionSect2) &&
		(m_nVersionSect3 == rVersion.m_nVersionSect3) &&
		(m_nVersionSect4 == rVersion.m_nVersionSect4)\
	)
	{
		return TRUE;
	}
	return FALSE;
}

int KVersionNumber::operator>(const KVersionNumber &rVersion)
{
	if (m_nVersionSect1 > rVersion.m_nVersionSect1)
		return TRUE;
	if (m_nVersionSect2 > rVersion.m_nVersionSect2)
		return TRUE;
	if (m_nVersionSect3 > rVersion.m_nVersionSect3)
		return TRUE;
	if (m_nVersionSect4 > rVersion.m_nVersionSect4)
		return TRUE;

	return FALSE;
}

int KVersionNumber::Parse(const TCHAR szVersion[])
{
	int nRetCode = FALSE;
	int nResult  = FALSE;
	TCHAR szVersionCopy[MAX_VERSION_LENGTH];
	TCHAR *pTempVersion = NULL;
	TCHAR *pDotPos = NULL;

	nRetCode = IdentifyType(szVersion);
	KG_PROCESS_ERROR(nRetCode);

	_tcsncpy(szVersionCopy, szVersion, sizeof(szVersionCopy) / sizeof(TCHAR));
	szVersionCopy[sizeof(szVersionCopy) / sizeof(TCHAR) - 1] = _T('\0');

	pTempVersion = szVersionCopy;

	pDotPos = _tcschr(pTempVersion, m_cType);
	KG_PROCESS_ERROR(pDotPos);
	*pDotPos = _T('\0');
	m_nVersionSect1 = _tstoi(pTempVersion);

	pTempVersion = pDotPos + 1;
	pDotPos = _tcschr(pTempVersion, m_cType);
	KG_PROCESS_ERROR(pDotPos);
	*pDotPos = _T('\0');
	m_nVersionSect2 = _tstoi(pTempVersion);

	pTempVersion = pDotPos + 1;
	pDotPos = _tcschr(pTempVersion, m_cType);
	KG_PROCESS_ERROR(pDotPos);
	*pDotPos = _T('\0');
	m_nVersionSect3 = _tstoi(pTempVersion);

	pTempVersion = pDotPos + 1;
	m_nVersionSect4 = _tstoi(pTempVersion);

	nResult = TRUE;
Exit0:
	return nResult;
}

int KVersionNumber::IdentifyType(const TCHAR szVersion[])
{
	const TCHAR *pTemp = NULL;

	m_cType = _T('.');
	pTemp = _tcschr(szVersion, m_cType);
	KG_PROCESS_SUCCESS(pTemp);

	m_cType = _T(',');
	pTemp = _tcschr(szVersion, m_cType);
	KG_PROCESS_SUCCESS(pTemp);

	m_cType = _T('-');
	pTemp = _tcschr(szVersion, m_cType);
	KG_PROCESS_SUCCESS(pTemp);

	return FALSE;
Exit1:
	return TRUE;
}

int KVersionNumber::operator=(const TCHAR szVersion[])
{
	int nRetCode = false;
	int nResult  = false;

	KGLOG_PROCESS_ERROR(szVersion);

	m_nVersionSect1 = 0;
	m_nVersionSect2 = 0;
	m_nVersionSect3 = 0;
	m_nVersionSect4 = 0;
	m_cType = _T('.');

	Parse(szVersion);

	nResult = true;
Exit0:
	return nResult;
}