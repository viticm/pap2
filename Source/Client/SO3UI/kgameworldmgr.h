////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : kgameworldmgr.h
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2007-11-29 15:45:35
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KGAMEWORLDMGR_H_
#define _INCLUDE_KGAMEWORLDMGR_H_

////////////////////////////////////////////////////////////////////////////////

#include "SO3UI/ISO3UI.h"

class KGameWorldMgr
{
public:
	KGameWorldMgr();
	virtual ~KGameWorldMgr();

	int Init(fnStringToID fnAttribute, fnStringToID fnRequire);
	void Exit();

	int AttributeStringToID(LPCSTR pcszString, int &nID);
	int RequireStringToID(LPCSTR pcszString, int &nID);

protected:
private:
	fnStringToID m_fnAttribute;
	fnStringToID m_fnRequire;
};

extern KGameWorldMgr g_GameWorldMgr;

#endif //_INCLUDE_KGAMEWORLDMGR_H_
