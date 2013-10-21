////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : kconststringmgr.h
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2007-9-18 10:08:15
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KCONSTSTRINGMGR_H_
#define _INCLUDE_KCONSTSTRINGMGR_H_

////////////////////////////////////////////////////////////////////////////////

namespace UI
{

class KConstStringMgr
{
private:
	typedef std::vector<LPCSTR> KStringArray;

public:
	KConstStringMgr(DWORD dwBufferSize = 256 * 1024);
	virtual ~KConstStringMgr();

	int Append(LPCSTR pcszString);
	LPCSTR GetAt(int nIndex);

	void Clear();

private:
	LPSTR m_pszBuffer;
	DWORD m_dwBufferSize;
	DWORD m_dwOffSet;

	KStringArray m_aString;
};

} //namespace UI

#endif //_INCLUDE_KCONSTSTRINGMGR_H_
