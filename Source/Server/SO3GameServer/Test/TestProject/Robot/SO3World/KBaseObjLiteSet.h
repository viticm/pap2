/************************************************************************/
/* 简化的数据集合                                                       */
/* Copyright : Kingsoft 2004										    */
/* Author	 : Zhu Jianqiu, Freeway Chen    							*/
/* History	 :															*/
/*		2005.01.14	Create												*/
/*		2006.12.23	use vector for speed and space						*/
/* Comment	 :															*/
/*		功能要求：														*/
/*			1	快速遍历												*/
/*			2	快速删除和插入											*/
/*			这个类是KBaseObjSet的简化类，取消了ID的Map，取消了自动分配ID*/
/************************************************************************/
#ifndef _KBASEOBJLITESET_H_
#define _KBASEOBJLITESET_H_

#include <stdlib.h>
#include <vector>

using namespace std;

template <class T>
class KBaseObjLiteSet
{
public:
	KBaseObjLiteSet(void);
	~KBaseObjLiteSet(void);

	//初始化，需要指定最大对象个数
	BOOL	Init(DWORD dwSize);

	//产生新对象
	T *New();

	//释放对象
	BOOL Delete(T *pObj);

    BOOL NewArray(int nNewCount, T **ppRetArray);
    BOOL DeleteArray(int nCount, T **ppArray);

	//获取存储数组的大小
	DWORD GetSize();

private:
	//数组，基本存储
	DWORD					m_dwSize;
	//空闲表
	typedef vector<T *>	    FREE_VECTOR, HEAD_VECTOR;
	FREE_VECTOR				m_FreeVector;
	HEAD_VECTOR				m_HeadVector;
};

template <class T>
inline KBaseObjLiteSet<T>::KBaseObjLiteSet(void)
{
	m_dwSize = 0;

	return;
}

template <class T>
inline KBaseObjLiteSet<T>::~KBaseObjLiteSet(void)
{
	//内存泄漏检测
	ASSERT(m_FreeVector.size() == m_dwSize);

	HEAD_VECTOR::iterator	it;
    for (it = m_HeadVector.begin(); it != m_HeadVector.end(); ++it)
    {
		T *pHead = *it;
		KG_DELETE_ARRAY(pHead);
    }
	m_HeadVector.clear();
	m_FreeVector.clear();

	return;
}

template <class T>
inline BOOL KBaseObjLiteSet<T>::Init(DWORD dwSize)
{
    int nResult = false;
	//释放空间及清空数据
	HEAD_VECTOR::iterator	it;
	for (it = m_HeadVector.begin(); it != m_HeadVector.end(); ++it)
	{
		T *pHead = *it;
		KG_DELETE_ARRAY(pHead);
	}
	m_HeadVector.clear();
	m_FreeVector.clear();
	m_dwSize = 0;

	//分配空间及初始化
	m_dwSize = dwSize;
	T *pObjectSet = new T[dwSize];
	KG_PROCESS_ERROR(pObjectSet);

	m_HeadVector.push_back(pObjectSet);

    m_FreeVector.reserve(m_dwSize);
    m_FreeVector.resize(dwSize);

    T **ppDest = &m_FreeVector[0];
    T *pObjEnd = pObjectSet + dwSize;
	for (T *pObj = pObjectSet; pObj < pObjEnd; pObj++)
	{ 
        *ppDest++ = pObj;
	}

    nResult = true;
Exit0:

	return nResult;
}

template <class T>
inline T *KBaseObjLiteSet<T>::New()
{
    T *pResult = NULL;
    int nRetCode = false;

    nRetCode = m_FreeVector.empty();
	KGLOG_CHECK_ERROR(!nRetCode);
	if (nRetCode)
	{
		DWORD dwSize = m_dwSize;	// LiteSet空间不足时加一倍大小
		
		T *pObjectSet = new T[dwSize];
		KGLOG_PROCESS_ERROR(pObjectSet);

		m_dwSize += dwSize;

		m_HeadVector.push_back(pObjectSet);

        m_FreeVector.reserve(m_dwSize);
        m_FreeVector.resize(dwSize);

        T **ppDest = &m_FreeVector[0];
        T *pObjEnd = pObjectSet + dwSize;
	    for (T *pObj = pObjectSet; pObj < pObjEnd; pObj++)
	    { 
            *ppDest++ = pObj;
	    }
	}

	pResult = m_FreeVector.back();
	m_FreeVector.pop_back();

Exit0:
	return pResult;
}

template <class T>
inline BOOL KBaseObjLiteSet<T>::Delete(T *pObj)
{
    int nResult = false;

	KGLOG_PROCESS_ERROR(pObj);

	m_FreeVector.push_back(pObj);

	nResult = true;

Exit0:
	return nResult;
}

template <class T>
inline BOOL KBaseObjLiteSet<T>::NewArray(int nNewCount, T **ppRetArray)
{
    int nResult = false;
    int nRetCode = false;
    int nFreeCount = 0;
    
    KG_PROCESS_ERROR(nNewCount > 0);
    KG_PROCESS_ERROR(ppRetArray);

    nFreeCount = (int)m_FreeVector.size();
    if (nNewCount <= nFreeCount)
    {
        memcpy(ppRetArray, &m_FreeVector[nFreeCount - nNewCount], nNewCount * sizeof(T *));

        m_FreeVector.resize(nFreeCount - nNewCount);
        KG_PROCESS_SUCCESS(TRUE);
    }

    DWORD dwSize = nNewCount;

	T *pObjectSet = new T[dwSize];
	KGLOG_PROCESS_ERROR(pObjectSet);

	m_dwSize += dwSize;
	m_HeadVector.push_back(pObjectSet);
    m_FreeVector.reserve(m_dwSize);

    T *pObjEnd = pObjectSet + dwSize;
    for (T *pObj = pObjectSet; pObj < pObjEnd; pObj++)
    { 
	    *ppRetArray++ = pObj;
    }

Exit1:
    nResult = true;
Exit0:
	return nResult;
}

template <class T>
inline BOOL KBaseObjLiteSet<T>::DeleteArray(int nCount, T **ppArray)
{
    int nResult = false;

    KGLOG_PROCESS_ERROR(nCount > 0);
	KGLOG_PROCESS_ERROR(ppArray);

    m_FreeVector.insert(m_FreeVector.end(), ppArray, ppArray + nCount);

    nResult = true;

Exit0:
	return nResult;
}


template <class T>
inline DWORD KBaseObjLiteSet<T>::GetSize()
{
	return m_dwSize;
}

#endif	//_KBASEOBJLITESET_H_