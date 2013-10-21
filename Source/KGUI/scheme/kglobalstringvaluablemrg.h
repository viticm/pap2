////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : kglobalstringvaluablemrg.h
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2006-2-13 19:38:19
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KGLOBALSTRINGVALUABLEMRG_H_
#define _INCLUDE_KGLOBALSTRINGVALUABLEMRG_H_

////////////////////////////////////////////////////////////////////////////////
#include "../driver/kconststringmgr.h"


////////////////////////////////////////////////////////////////////////////////

namespace UI
{

class KConstStringMgr;

class KGlobalStrValuableMgr
{
private:
    typedef stdext::hash_map<std::string, int> KeyToIndex;

public:
	KGlobalStrValuableMgr();
	~KGlobalStrValuableMgr();

public:
	int Append(LPCSTR pcszKey, LPCSTR pcszvalue);
	LPCSTR GetAt(LPCSTR pcszKey);
	LPCSTR GetAt(int nIndex);
	int GetIndexAt(LPCSTR pcszKey);

	int RecodeValue(IN OUT LPSTR pszValue);
    int Clear();

private:
	KeyToIndex m_ValueToIndex;
	KConstStringMgr m_StrMgr;
};

} //namespace UI



#endif //_INCLUDE_KGLOBALSTRINGVALUABLEMRG_H_
