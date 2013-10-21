/************************************************************************/
/* 针对ID索引的数据缓存管理模板类                                       */
/* Copyright : Kingsoft 2004										    */
/* Author	 : Zhu Jianqiu												*/
/* History	 :															*/
/*		2005.04.21	Create												*/
/* Comment	 :															*/
/*		原来Engine中的Cache类是针对字符串索引的，这里改成了根据DWORD索引*/
/*		Cache的更新机制是：Cache未满时，不删除任何数据					*/
/*			Cache满时，优先删除最长时间未使用的数据						*/
/*		注意：Cache中不可以存储指针，因为里面的数据可能在任何时候被删除 */
/************************************************************************/
#ifndef _KIDCACHE_H_
#define _KIDCACHE_H_

#include <stdlib.h>
#include <map>

class KIDCache
{
public:
	KIDCache(void);
	~KIDCache(void);

	//初始化，需要指定最大对象个数
	BOOL Init(DWORD dwMaxSize);
	//加入新对象
	void* AddData(DWORD dwID, void* pData, DWORD dwSize);
	//根据ID在Cache中查找对象，查不到返回NULL
	void* GetData(DWORD dwID, DWORD& rdwSize);
	//清空缓存
	BOOL Clear(void);
	BOOL LoadFromBuffer(char* pData, DWORD dwSize);
	BOOL SaveToBuffer(char* pData, DWORD dwSize);

private:
	struct KIDCacheNode : KNode
	{
		DWORD			m_dwID;
		DWORD			m_dwSize;
		char			m_szData[0];
	};

	DWORD				m_dwMaxSize;
	KList				m_CacheList;

	typedef std::map<DWORD, KIDCacheNode*>	MAP_ID_2_NODE;	
	MAP_ID_2_NODE		m_mapId2Node;
};

#endif	//_KIDCACHE_H_
