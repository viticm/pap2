////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGPropertyEditUtilityFunctions.h
//  Version     : 1.0
//  Author      : ZhaoXinyu (Chiyer)
//  CopyRight   : Kingsoft Corporation All rights Reserved
//  History     : 2006-3-17 17:21:26
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _KGPRPRERYTEDITUTILITYFUNCTION_H_
#define _KGPRPRERYTEDITUTILITYFUNCTION_H_

#include "vector"
#include "list"

#define  FREE_SIZE   100
#define  PROESS_SETP 1000
#define  MAX_KEY   INT_MAX
#define  MIN_KEY   INT_MIN

template<class T> class stack
{
public :
	stack(int nSize = 100)
	{
		m_pObject = NULL;
		resize(nSize);
	}
	virtual ~stack()
	{
		if (m_pObject)
			delete[] m_pObject;
		m_pObject = NULL;
	}
	int resize(int nSize)
	{
		m_nSize    = nSize;
		if (m_pObject)
			delete[] m_pObject;
		m_pObject  = new T[m_nSize]; 
		m_nCurPos  = 0;
		return true;
	}
	int push_back(T object)
	{
		if (++m_nCurPos < m_nSize)
			m_pObject[m_nCurPos] = object;
		else
			return false;
		return true;
	}
	int pop_back()
	{
		if (m_nCurPos >= 0)
			m_nCurPos--;
		else 
			return false;
		return true;
	}
	int empty()
	{
		return m_nCurPos == -1 ? true : false;
	}
	T back()
	{
		if (m_nCurPos >= 0)
			return m_pObject[m_nCurPos];
		else
			return (T)0;
	}
	int clear()
	{
		m_nCurPos = -1;
		return true;
	}
	int erase(T object)
	{
		int nPos = find(object);
		if (nPos != -1)
		{
			for (int i = nPos + 1; i <= m_nCurPos;i++)
				m_pObject[i - 1] = m_pObject[i];
			m_nCurPos--;
		}
		else
			return false;
		return true;
	}
	int find(T object)
	{
		for (int i = 0; i <= m_nCurPos; i++)
		{
			if (m_pObject[i] == object)
				return i;
		}
		return -1;
	}

public :
	T*  m_pObject;
	int m_nSize;
	int m_nCurPos;
	
};

class KGPrimaryKeyManager  
{
public :
	int GetKey(int *pnKey);
	int AddKey(int  nKey, int nCheckFlag = false);
	int DelKey(int  nKey);
	int Refrush(int nMinKey, int nMaxKey, CProgressCtrl* pProgress = NULL, CStatusBar* pStatusBar = NULL);

public :
	KGPrimaryKeyManager();
	KGPrimaryKeyManager(int nMinKey, int nMaxKey);
	~KGPrimaryKeyManager();

private :
	int m_nMinKey;
	int m_nMaxKey;
	/*vector<int> m_vectorKeys; */
	stack<int> m_vectorKeys;
};

int g_IniPrimaryKesManager(
	KGPrimaryKeyManager* pKeyManager, ITabFile* ppTabFile[], int nFileCount,
	const char pcszManagerKey[], CProgressCtrl* pPrpgress = NULL
);

//
// Tab File Operation
//

int g_UniteTabFile(ITabFile** ppUnitFile, ITabFile** pImportFile, int nUniteCount);

int g_UnUniteTabFile(ITabFile* pUnitFile, ITabFile** ppExportFile, int nExportCount);

int g_GetPosByPrimaryKey(
	ITabFile* pTabFile, const char szPrimaryKeyField[], int nKeyValue, int nFlag = true
);

int g_CopyOneRow(ITabFile* pTabFIle, int nSourRow, int nDestRow);

int FindInSubset(
	IIniFile* pIniFile, CString szPrimaryKey, CString& szReturnSectionName
);

int BuildPrepertyFile(
	ITabFile* pTabFile, IIniFile* pIniFile,const char pcszSectionName[],
	int* pnInsertNumber, CProgressCtrl* pProCtrl = NULL
);

int AutoFillTable(
	ITabFile* pTabFile, int nRow, IIniFile* pRuleTable, LPCSTR szPath, LPCSTR szPrefix
);

int BusyCursor(CWnd* pWnd);
int FreeCursor(CWnd* pWnd);
//
// 
//
typedef int (*TreeOpFun)(CTreeCtrl* pTree, HTREEITEM hTreeItem);

int operation_tree_item(
	CTreeCtrl* pTree, HTREEITEM hTreeItem, TreeOpFun operator_fun, int nFirstCallFlag = true
);

#endif //_KGPRPRERYTEDITUTILITYFUNCTION_H_