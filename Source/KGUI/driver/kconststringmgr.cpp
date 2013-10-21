////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : kconststringmgr.cpp
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2007-9-18 10:08:19
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "./kconststringmgr.h"


////////////////////////////////////////////////////////////////////////////////

namespace UI
{

KConstStringMgr::KConstStringMgr(DWORD dwBufferSize)
{
	KGLOG_PROCESS_ERROR(dwBufferSize > 0);
	m_dwBufferSize = dwBufferSize;

	m_pszBuffer = new char[m_dwBufferSize]; 
	KGLOG_PROCESS_ERROR(m_pszBuffer);

	m_aString.reserve(m_dwBufferSize / 32);
	*m_pszBuffer = '\0';
	m_aString.push_back(m_pszBuffer);
	m_dwOffSet = 1;

	return;
Exit0:
	m_pszBuffer = NULL;
	m_dwBufferSize = 0;
	m_dwOffSet = 0;
	return;
}

KConstStringMgr::~KConstStringMgr()
{
	m_dwBufferSize = 0;
	SAFE_DELETE_ARRAY(m_pszBuffer);
}

int KConstStringMgr::Append(LPCSTR pcszString)
{
	if (pcszString && m_pszBuffer)
	{
		if (pcszString[0] == '\0')
			return 0;

		if (m_dwOffSet >= m_dwBufferSize - 1)
			return -1;

		LPSTR pszDest = m_pszBuffer + m_dwOffSet;
		LPCSTR pcszEnd = m_pszBuffer + m_dwBufferSize - 1;
		m_aString.push_back(pszDest);
		while (*pcszString)
		{
			if (pszDest == pcszEnd)
			{
				KGLogPrintf(KGLOG_ERR, "KGUI KConstStringMgr::Append error: string table reach the end and need more space!");
				break;
			}
			*pszDest++ = *pcszString++;
		}
		*pszDest++ = '\0';
		m_dwOffSet = DWORD(pszDest - m_pszBuffer);

		return (int)(m_aString.size() - 1);
	}
	return -1;
}

LPCSTR KConstStringMgr::GetAt(int nIndex)
{
	if (nIndex >= 0 && nIndex < (int)(m_aString.size()))
		return m_aString[nIndex];
	return NULL;
}

void KConstStringMgr::Clear()
{
	m_aString.clear();
	if (m_pszBuffer)
	{
		*m_pszBuffer = '\0';
		m_aString.push_back(m_pszBuffer);
		m_dwOffSet = 1;
	}
}

}
