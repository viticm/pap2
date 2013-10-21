/************************************************************************/
/* 数据集合                                                             */
/* Copyright : Kingsoft 2004										    */
/* Author	 : Zhu Jianqiu, Freeway Chen								*/
/* History	 :															*/
/*		2005.01.01	Create												*/
/*		2006.12.23	use vector for speed and space						*/
/* Comment	 :															*/
/*		功能要求：														*/
/*			1	根据ID快速随机访问										*/
/*			2	快速遍历												*/
/*			3	快速删除和插入											*/
/************************************************************************/
#ifndef _KBASEOBJECTSET_H_
#define _KBASEOBJECTSET_H_

#include <stdlib.h>
#include <hash_map>
#include <vector>
#include "SO3GlobalDef.h"
#include "Global.h"

using namespace std;
using namespace stdext;



template <class T>
class KBaseObjectSet
{
public:
	KBaseObjectSet(void);
	~KBaseObjectSet(void);

	//初始化，需要指定最大对象个数，和ID的前缀，前缀由Relay分配，整个游戏世界全局唯一
	BOOL	Init(DWORD dwSize, DWORD dwPrefix);

	//产生新对象
	inline T *New(DWORD dwId = ERROR_ID);

	//释放对象
	inline BOOL Delete(T *pObj);

	//根据ID定位Obj
	inline T *GetObj(DWORD dwId);

	//遍历已分配的Obj
	template <class TFunc>
	inline BOOL Traverse(TFunc &Func);

	//获取存储数组的大小
	inline DWORD GetSize(void);

	inline DWORD GetUsedSize(void);

private:
	//数组，基本存储
	DWORD					m_dwSize;
	//空闲表
	typedef vector<T *>	    FREE_VECTOR, HEAD_VECTOR;
	FREE_VECTOR				m_FreeVector;
	HEAD_VECTOR				m_HeadVector;
	//ID字段的Map
	typedef hash_map<DWORD, T *> MAP_ID_2_OBJ;	
	MAP_ID_2_OBJ			m_mapId2Obj;

	DWORD					m_dwIdGenerator;	//从一开始，零表示无效值
												//32bit中，高4位由Relay分配
};

template <class T>
KBaseObjectSet<T>::KBaseObjectSet(void)
{
	m_dwIdGenerator = ERROR_ID;
	m_dwSize = 0;

	return;
}

template <class T>
KBaseObjectSet<T>::~KBaseObjectSet(void)
{
	//内存泄漏检测
	ASSERT(m_mapId2Obj.size() == 0);
	ASSERT(m_FreeVector.size() == m_dwSize);

	HEAD_VECTOR::iterator	it;
    for (it = m_HeadVector.begin(); it != m_HeadVector.end(); ++it)
    {
		T *pHead = *it;
		KG_DELETE_ARRAY(pHead);
    }
	m_HeadVector.clear();

	m_FreeVector.clear();
	m_mapId2Obj.clear();

	return;
}

template <class T>
BOOL KBaseObjectSet<T>::Init(DWORD dwSize, DWORD dwPrefix)
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
	m_mapId2Obj.clear();
	m_FreeVector.clear();
	m_dwSize = 0;
	m_dwIdGenerator = ERROR_ID;

	//分配的大小不能超过ID的最大值
	KGLOG_PROCESS_ERROR(dwSize < ((1 << (sizeof(m_dwIdGenerator) * 8 - ID_PREFIX_BIT_NUM)) - 1));

	//分配空间及初始化
	m_dwSize = dwSize;
	T *pObjectSet = new T[dwSize];
	KGLOG_PROCESS_ERROR(pObjectSet);

	m_HeadVector.push_back(pObjectSet);
    m_FreeVector.reserve(m_dwSize);
    m_FreeVector.resize(dwSize);

    T **ppDest = &m_FreeVector[0];
    T *pObjEnd = pObjectSet + dwSize;
	for (T *pObj = pObjectSet; pObj < pObjEnd; pObj++)
	{ 
        pObj->m_dwID = ERROR_ID;
        *ppDest++ = pObj;
	}

	m_dwIdGenerator = (DWORD)(dwPrefix << (sizeof(m_dwIdGenerator) * 8 - ID_PREFIX_BIT_NUM)) + 1;

	nResult = true;
Exit0:
	return nResult;
}

template <class T>
T *KBaseObjectSet<T>::New(DWORD dwId/* = ERROR_ID*/)
{
    T *pResult   = NULL;
    T *pObj      = NULL;
    int nRetCode = false;
	pair<MAP_ID_2_OBJ::iterator, bool> InsRet;

    nRetCode = m_FreeVector.empty();
	KGLOG_CHECK_ERROR(!nRetCode);
	if (nRetCode)
	{
		DWORD dwSize = m_dwSize / 8;
		if (!dwSize)
			dwSize = 1;

		DWORD dwAllSize = m_dwSize + dwSize;
		
		KGLOG_PROCESS_ERROR(
			dwAllSize < ((1 << (sizeof(m_dwIdGenerator) * 8 - ID_PREFIX_BIT_NUM)) - 1));

		T *pObjectSet = new T[dwSize];
		KGLOG_PROCESS_ERROR(pObjectSet);

		m_dwSize = dwAllSize;

		m_HeadVector.push_back(pObjectSet);
        m_FreeVector.reserve(m_dwSize);
        m_FreeVector.resize(dwSize);

        T **ppDest = &m_FreeVector[0];
        T *pObjEnd = pObjectSet + dwSize;
	    for (T *pObj = pObjectSet; pObj < pObjEnd; pObj++)
	    { 
			pObj->m_dwID = ERROR_ID;
            *ppDest++ = pObj;
	    }
	}

	pObj = m_FreeVector.back();

	if (dwId == ERROR_ID)
		pObj->m_dwID = ++m_dwIdGenerator;
	else
		pObj->m_dwID = dwId;

	InsRet = m_mapId2Obj.insert(make_pair(pObj->m_dwID, pObj));
	KGLOG_PROCESS_ERROR(InsRet.second);

	m_FreeVector.pop_back();

    pResult = pObj;

Exit0:
	return pResult;
}

template <class T>
BOOL KBaseObjectSet<T>::Delete(T* pObj)
{
    int nResult = false;

	KGLOG_PROCESS_ERROR(pObj);
	KGLOG_PROCESS_ERROR(pObj->m_dwID != ERROR_ID);

	m_mapId2Obj.erase(pObj->m_dwID);
	
    pObj->m_dwID = ERROR_ID;
	m_FreeVector.push_back(pObj);

	nResult = true;

Exit0:
	return nResult;
}

template <class T>
inline DWORD KBaseObjectSet<T>::GetSize(void)
{
	return m_dwSize;
}

template <class T>
inline DWORD KBaseObjectSet<T>::GetUsedSize(void)
{
	return (DWORD)m_mapId2Obj.size();
}

template <class T>
T *KBaseObjectSet<T>::GetObj(DWORD dwId)
{
	MAP_ID_2_OBJ::iterator it = m_mapId2Obj.find(dwId);

	if (it == m_mapId2Obj.end())
		return NULL;

	return it->second;
}

template <class T>
template <class TFunc>
inline BOOL KBaseObjectSet<T>::Traverse(TFunc &Func)
{
	BOOL bRetCode = FALSE;

	for (
        MAP_ID_2_OBJ::iterator it = m_mapId2Obj.begin();
		it != m_mapId2Obj.end();
        NULL
    )
	{
		MAP_ID_2_OBJ::iterator itNext = it;
		++itNext;
		bRetCode = Func(it->first, it->second);
		KG_PROCESS_ERROR(bRetCode);
		it = itNext;
	}     

Exit0:

	return TRUE;
}

#endif	//_KBASEOBJECTSET_H_