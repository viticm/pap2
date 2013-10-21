////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : kareatest.cpp
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2007-8-3 16:29:32
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "kareatest.h"


////////////////////////////////////////////////////////////////////////////////

KAreaTester g_AreaTester;

KAreaTester::KAreaTester()
{

}

KAreaTester::~KAreaTester()
{
	for (InfoArray::iterator it = m_aInfo.begin(); it != m_aInfo.end(); ++it)
	{
		delete *it;
	}
	m_aInfo.clear();
}

int KAreaTester::GetID(LPCSTR pcszFile)
{
	int nResult = -1;
	int nRetCode = false;
	KAreaInfo *pInfo = NULL;
	KFileToIndex::iterator it;
	KGLOG_PROCESS_ERROR(pcszFile);

	it = m_File.find(pcszFile);
	if (it != m_File.end())
	{
		nResult = it->second;
		goto Exit0;
	}

	pInfo = new KAreaInfo;
	KGLOG_PROCESS_ERROR(pInfo);

	nRetCode = pInfo->FromFile(pcszFile);
	KGLOG_PROCESS_ERROR(nRetCode);

	m_aInfo.push_back(pInfo);
	nResult = (int)(m_aInfo.size()) - 1;

	m_File.insert(make_pair(pcszFile, nResult));

Exit0:
	if (nResult == -1)
	{
		delete pInfo;
	}
	return nResult;
}

int KAreaTester::PtInArea(int nID, float fX, float fY)
{
	int nResult = false;
	KG_PROCESS_ERROR(nID >= 0 && nID < (int)(m_aInfo.size()));

	return m_aInfo[nID]->PtInArea(fX, fY);
Exit0:
	return nResult;
}
