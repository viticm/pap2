////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KItemInfoList.h
//  Version     : 1.0
//  Creator     : 
//  Create Date : 2008-8-6 17:15:46
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KITEMINFOLIST_H_
#define _INCLUDE_KITEMINFOLIST_H_

////////////////////////////////////////////////////////////////////////////////
#include "KItemInfo.h"

class KOtherItemInfoList
{
public:
    KOtherItemInfoList(){};
    ~KOtherItemInfoList(){};

	BOOL	        Load(char* pszFile);
	void            Clear();
	KOtherItemInfo*	GetItemInfo(DWORD dwID);

private:
    BOOL            LoadLine(ITabFile* piTabFile, int nLine, KOtherItemInfo* pItemInfo, const KOtherItemInfo& crDefaultInfo);

private:
	typedef std::map<DWORD, KOtherItemInfo>	ITEM_INFO_LIST;
	ITEM_INFO_LIST				            m_ItemInfoList;
};

//////////////////////////////////////////////////////////////////////////

class KAttribInfoList
{
public:
    KAttribInfoList(){};
    ~KAttribInfoList(){};
    
    BOOL	        Load(char* pszFile);
	void            Clear();
	KAttribInfo*	GetItemInfo(DWORD dwID);

private:
    typedef std::map<DWORD, KAttribInfo>	ITEM_INFO_LIST;
	ITEM_INFO_LIST				            m_ItemInfoList;
};

//////////////////////////////////////////////////////////////////////////

class KSetInfoList
{
public:
    KSetInfoList(){};
    ~KSetInfoList(){};

    BOOL	        Load(char* pszFile);
	void            Clear();
	KSetInfo*	    GetItemInfo(DWORD dwID);

private:
    typedef std::map<DWORD, KSetInfo>	    ITEM_INFO_LIST;
	ITEM_INFO_LIST				            m_ItemInfoList;
};

#endif //_INCLUDE_KITEMINFOLIST_H_

