////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : kgameworldmgr.cpp
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2007-11-29 15:45:40
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "./kgameworldmgr.h"

////////////////////////////////////////////////////////////////////////////////

KGameWorldMgr g_GameWorldMgr;

KGameWorldMgr::KGameWorldMgr()
{
	m_fnAttribute = NULL;
	m_fnRequire = NULL;
}

KGameWorldMgr::~KGameWorldMgr()
{

}

int KGameWorldMgr::Init(fnStringToID fnAttribute, fnStringToID fnRequire)
{
	int nResult = false;

	KGLOG_PROCESS_ERROR(fnAttribute);
	KGLOG_PROCESS_ERROR(fnRequire);

	m_fnAttribute = fnAttribute;
	m_fnRequire = fnRequire;

	nResult = TRUE;
Exit0:
	return nResult;
}

void KGameWorldMgr::Exit()
{
}

int KGameWorldMgr::AttributeStringToID(LPCSTR pcszString, int &nID)
{
	int nResult = false;
	int nRetCode = false;

	KG_PROCESS_ERROR(pcszString);
	KG_PROCESS_ERROR(m_fnAttribute);

	nRetCode = m_fnAttribute(pcszString, nID);
	KG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	if (!nResult)
		KGLogPrintf(KGLOG_ERR, "SO3UI AttributeStringToID(%s) failed!", pcszString);
	return nResult;
}
int KGameWorldMgr::RequireStringToID(LPCSTR pcszString, int &nID)
{
	int nResult = false;
	int nRetCode = false;

	KG_PROCESS_ERROR(pcszString);
	KG_PROCESS_ERROR(m_fnRequire);

	nRetCode = m_fnRequire(pcszString, nID);
	KG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	if (!nResult)
		KGLogPrintf(KGLOG_ERR, "SO3UI RequireStringToID(%s) failed!", pcszString);
	return nResult;
}
