////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : kglobalstringvaluablemrg.cpp
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2006-2-13 19:38:14
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "./kglobalstringvaluablemrg.h"
#include "../KGUI.h"

////////////////////////////////////////////////////////////////////////////////

namespace UI
{

KGlobalStrValuableMgr::KGlobalStrValuableMgr() : m_StrMgr( 128 * 1024) //128K
{
}

KGlobalStrValuableMgr::~KGlobalStrValuableMgr()
{
    Clear();
}

int KGlobalStrValuableMgr::Clear()
{
    m_StrMgr.Clear();
    m_ValueToIndex.clear();
    return true;
}

int KGlobalStrValuableMgr::Append(LPCSTR pcszKey, LPCSTR pcszValue)
{
	int nResult = false;
	int nRetcode = false;

	KG_PROCESS_ERROR(pcszKey);
	KG_PROCESS_ERROR(pcszValue);

	nRetcode = m_StrMgr.Append(pcszValue);
	KG_PROCESS_ERROR(nRetcode >= 0);

	m_ValueToIndex.insert(make_pair(pcszKey, nRetcode));

	nResult = true;
Exit0:
	return nResult;
}

LPCSTR KGlobalStrValuableMgr::GetAt(LPCSTR pcszKey)
{
	if (!pcszKey)
		return NULL;
	KeyToIndex::iterator it = m_ValueToIndex.find(pcszKey);
	if (it == m_ValueToIndex.end())
		return NULL;
	return m_StrMgr.GetAt((*it).second);
}

LPCSTR KGlobalStrValuableMgr::GetAt(int nIndex)
{
	return m_StrMgr.GetAt(nIndex);
}

int KGlobalStrValuableMgr::GetIndexAt(LPCSTR pcszKey)
{
	if (!pcszKey)
		return -1;
	KeyToIndex::iterator it = m_ValueToIndex.find(pcszKey);
	if (it == m_ValueToIndex.end())
		return -1;
	return (*it).second;
}

int KGlobalStrValuableMgr::RecodeValue(IN OUT LPSTR pszValue)
{
	//TODO:可能在解析某些中文字符时会出错!
	int nResult = false;
	int nSpecial = false;
	LPSTR pszTemp = NULL;
	KG_PROCESS_ERROR(pszValue);

	pszTemp = pszValue;
	while (true)
	{
		if (*pszValue == '\\')
		{
			nSpecial = true;
			switch (*(pszValue + 1))
			{
			case 'n':
				*pszTemp = '\n';
				break;
			case 't':
				*pszTemp = '\t';
				break;
			case '\\':
				*pszTemp = '\\';
				break;
			case '\"':
				*pszTemp = '\"';
				break;
			default:
				nSpecial = false;
			}
			if (nSpecial)
			{
				++pszTemp;
				pszValue += 2;
				continue;
			}
		}
		if (*pszValue == '\0')
			break;
		LPCSTR pcszNext = ::CharNextA(pszValue);
		if (!pcszNext)
			break;
		while (pcszNext != pszValue)
		{
			*pszTemp = *pszValue;
			++pszValue;
			++pszTemp;
		}
	}
	*pszTemp = '\0';

	nResult = true;
Exit0:
	return nResult;
}

} //namespace UI
