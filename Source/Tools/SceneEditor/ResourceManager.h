// KResourceManager.h: interface for the KResourceManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_KResourceManager_H__9C3F564E_0218_4D56_AF1D_1E90C01E8AD6__INCLUDED_)
#define AFX_KResourceManager_H__9C3F564E_0218_4D56_AF1D_1E90C01E8AD6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_PATH_LENGTH 256
#include <string.h>
#include <hash_map>

using namespace std;
using namespace stdext;

struct eqstr
{
	bool operator()(const char* s1, const char* s2) const
	{
		return strcmp(s1, s2) == 0;
	}
};

typedef class KResourceHandle
{
public:
	string	m_szFileName;
	WORD	m_nRefCount;						// 引用计数
	DWORD	m_dwLastTimeUsed;					// 最近使用时间

	KResourceHandle()
	{
		m_nRefCount = 0;
		m_dwLastTimeUsed = 0;
	}

	virtual ~KResourceHandle()
	{
	};

	void IncRefCount(){ m_nRefCount++; }
	void DecRefCount(){ m_nRefCount--; }
	void SetRefCount(WORD nRefCount){ m_nRefCount = nRefCount; }

	virtual HRESULT LoadFromFile(const char *szFilename,DWORD Option = 0,DWORD Operator=0)//从文件中载入
	{
		return S_OK;
	}
	virtual HRESULT Release()//释放
	{
		return S_OK;
	}
	virtual HRESULT Cleanup()//清除
	{
		return S_OK;
	}

	virtual HRESULT OnLostDevice(void)
	{
		return S_OK;
	}

	virtual HRESULT OnResetDevice(void)
	{
		return S_OK;
	}
}* HRESOURCE;


template <class HResourceType>
class KResourceManager  
{
public:

	typedef hash_map<string, HResourceType >_ResourceHashMap;//定义存储资源的哈西表类型

	_ResourceHashMap m_ResourceHashmap;//实际存储资源的哈西表

protected:
	virtual HRESULT LoadResource(HResourceType* HResource, const char *szFilename,DWORD Option = 0,DWORD Operator=0)
	{
		if (!HResource)
		{
			HResource = new HResourceType;
		}
		if(SUCCEEDED(HResource->LoadFromFile(szFilename,Option,Operator)))
		{
			return S_OK;
		}
		else
		{
			return E_FAIL;
		}
	}
public:
	virtual HRESULT FindResourceByName(HResourceType** ppHandle,const char *szFilename)
	{
		_ResourceHashMap :: const_iterator i;
		i = m_ResourceHashmap.find(szFilename);
		if( i == m_ResourceHashmap.end() )
			return E_FAIL;
		else
		{
			(*ppHandle) = (HResourceType*) (&((*i).second));
			// 增加引用计数
			(*ppHandle)->IncRefCount();
			return S_OK;
		}
	}
	
	HRESULT GetResource(HResourceType** pHResource, const char *szFilename,DWORD Option = 0,DWORD Operator=0)
	{
		if(FAILED(FindResourceByName(pHResource,szFilename)))
		{
			HResourceType* HResource = new HResourceType;
			if(SUCCEEDED(LoadResource(HResource,szFilename,Option,Operator)))
			{
				m_ResourceHashmap[szFilename] = *HResource;

				_ResourceHashMap :: const_iterator i = m_ResourceHashmap.find(szFilename);
				HResourceType* pHandle = (HResourceType*)(&(i->second));
				int s = 0;

				(*pHResource) = HResource;
			}
			else
			{
				SAFE_DELETE(HResource);
				(*pHResource) = NULL;
			}
		}
		return S_OK;
	}
	

	HRESULT OnLostDevice(void)
	{
		_ResourceHashMap :: const_iterator i = m_ResourceHashmap.begin();
		while (i!=m_ResourceHashmap.end())
		{
			HResourceType HResource = (*i).second;
			HResource.OnLostDevice();

			i++;
		}
		return S_OK;
	}

	HRESULT OnResetDevice(void)
	{
		_ResourceHashMap :: const_iterator i = m_ResourceHashmap.begin();
		while (i!=m_ResourceHashmap.end())
		{
			HResourceType HResource = (*i).second;
			HResource.OnResetDevice();

			i++;
		}
		return S_OK;
	}


	KResourceManager()
	{
		;
	}
	virtual ~KResourceManager()
	{
		;
	}
};


class KCacheBase
{
public:
	DWORD m_dwLastUseTime;
	int   m_nReference;
	KCacheBase()
	{
		m_dwLastUseTime = 0;
		m_nReference = 0;
	}
	virtual HRESULT Clear()
	{
		return S_OK;
	}
	virtual ~KCacheBase()
	{
		;
	}
	HRESULT AddRef()
	{
		m_nReference++;
		return S_OK;
	}
	virtual HRESULT Release()
	{
		m_nReference--;
		if(m_nReference<0)
			return E_FAIL;
		else
			return S_OK;
	}
};

template <class TCache>
class KCacheManager
{
public:
	DWORD m_dwMaxCacheSize;
	vector<TCache*>m_vecCache;
public:
	TCache* GetLastUnUsed()
	{
		if(m_vecCache.size()<m_dwMaxCacheSize)
		{
			TCache* pNewCatch = new TCache;
			m_vecCache.push_back(pNewCatch);
			pNewCatch->m_dwLastUseTime = timeGetTime();
			return pNewCatch;
		}
		else
		{
			DWORD LastTime = 0;
			DWORD MaxIndex = 0;
			for(DWORD i=0;i<m_vecCache.size();i++)
			{
				TCache* pCache = m_vecCache[i];
				if(i==0)
				{
					LastTime = pCache->m_dwLastUseTime;
					MaxIndex =  i;
				}
				if(LastTime > pCache->m_dwLastUseTime)
				{
					LastTime = pCache->m_dwLastUseTime;
					MaxIndex =  i;
				}
			}
			TCache* pCache = m_vecCache[MaxIndex];
			pCache->m_dwLastUseTime = timeGetTime();
			return pCache;
		}
	}
	TCache* GetUnUsed()
	{
		for(DWORD i=0;i<m_vecCache.size();i++)
		{
			TCache* pCache = m_vecCache[i];
			if(pCache->m_nReference==0)
			{
				pCache->m_dwLastUseTime = timeGetTime();
				pCache->AddRef();
				return pCache;
			}
		}

		if(m_vecCache.size()<m_dwMaxCacheSize)
		{
			TCache* pNewCatch = new TCache;
			m_vecCache.push_back(pNewCatch);
			pNewCatch->m_dwLastUseTime = timeGetTime();
			pNewCatch->AddRef();
			return pNewCatch;
		}
		else
		{
			return NULL;
		}
	}
	void SetMaxSize(DWORD Size)
	{
		m_dwMaxCacheSize = Size;
	}

	KCacheManager()
	{
		m_dwMaxCacheSize = 0;
	}

	virtual ~KCacheManager()
	{
		for(DWORD i=0;i<m_vecCache.size();i++)
		{
			TCache* pCache = m_vecCache[i];
			SAFE_DELETE(pCache);
		}
		m_vecCache.clear();
	}

	virtual HRESULT Clear()
	{
		for(DWORD i=0;i<m_vecCache.size();i++)
		{
			TCache* pCache = m_vecCache[i];
			pCache->Clear();
		}
		return S_OK;
	}
};





template <class TResource>
class KTResourceManagerBase
{
	friend  TResource;
protected:

	hash_map<string,TResource*>m_hashmapResource;

	DWORD  m_dwNextID;//下一个分配的ID
	DWORD  m_dwReleaseQueueSzie;

	list<TResource*>m_listReleaseQueue;//需要释放的资源队列
	map<DWORD,TResource*>m_mapResource;


public:
	virtual HRESULT Get1NewResourcePointer(TResource** ppT,DWORD Type,DWORD Option)
	{
		(*ppT) = new TResource();
		if(!*ppT)
			return E_FAIL;

		return S_OK;
	}

	virtual HRESULT Get1NewResource(TResource** ppT,DWORD Type,DWORD Option = 0)
	{
		if(FAILED(Get1NewResourcePointer(ppT,Type,Option)))
		{
			return E_FAIL;
		}

		TResource* pT = (*ppT);
		pT->Init(m_dwNextID++);
		pT->AddRef();

		m_mapResource.insert(map<DWORD,TResource*>::value_type(pT->m_dwID,pT));
		m_hashmapResource.insert(hash_map<string,TResource*>::value_type(pT->m_scName,pT));
		return S_OK;
	}

	virtual HRESULT GetTypeByFileName(DWORD* pType,BOOL* pAwaysNew,LPSTR pFileName)
	{
		(pType) = 0;
		(*pAwaysNew) = FALSE;
		return S_OK;
	}

	virtual HRESULT LoadFromFile(TResource** ppT,LPSTR pFileName,DWORD Option = 0)
	{
		DWORD dwType = 0;
		BOOL   bAwaysNew = FALSE;

		if(FAILED(GetTypeByFileName(&dwType,&bAwaysNew,pFileName)))
			return E_FAIL;

		if(bAwaysNew)//是否总是创建新资源
		{
			if(FAILED(Get1NewResourcePointer(ppT,dwType,Option)))
			{
				return E_FAIL;
			}

			TResource* pT = (*ppT);
			pT->Init(m_dwNextID++);
			pT->AddRef();
			pT->m_scName = pFileName;

			m_mapResource.insert(map<DWORD,TResource*>::value_type(pT->m_dwID,pT));
			m_hashmapResource.insert(hash_map<string,TResource*>::value_type(pT->m_scName,pT));
		}
		else//先搜索以前的资源,相同文件名的
		{
			hash_map<string,TResource*>::iterator h = m_hashmapResource.find(pFileName);
			if(h!=m_hashmapResource.end())//找到了,直接使用
			{
				(*ppT) = h->second;
				(*ppT)->AddRef();
			}
			else
			{
				if(FAILED(Get1NewResourcePointer(ppT,dwType,Option)))
				{
					return E_FAIL;
				}

				TResource* pT = (*ppT);
				pT->Init(m_dwNextID++);
				pT->AddRef();
				pT->m_scName = pFileName;

				m_mapResource.insert(map<DWORD,TResource*>::value_type(pT->m_dwID,pT));
				m_hashmapResource.insert(hash_map<string,TResource*>::value_type(pT->m_scName,pT));
			}
		}

		TResource* pT = (*ppT);
		return pT->LoadFromFile(pFileName,Option);
	}

	virtual HRESULT FindResource(TResource** ppT,LPSTR pFileName)
	{
		hash_map<string,TResource*>::iterator h = m_hashmapResource.find(pFileName);
		if(h!=m_hashmapResource.end())//找到了,直接使用
		{
			(*ppT) = h->second;
			(*ppT)->AddRef();
			return S_OK;
		}
		else
		{
			return E_FAIL;
		}
	}

	virtual HRESULT GetResource(TResource** ppT,DWORD dwID)
	{
		map<DWORD,TResource*>::iterator i = m_mapResource.find(dwID);
		if(i!=m_mapResource.end())
		{
			(*ppT) = i->second;
			return S_OK;
		}
		return E_FAIL;
	}

	virtual HRESULT ReleaseResource(DWORD dwID)
	{
		TResource* lpResource = NULL;
		if(SUCCEEDED(GetResource(&lpResource,dwID)))
		{
			return lpResource->Release();
		}
		return E_FAIL;
	}

	virtual HRESULT CleanUp()
	{
		map<DWORD,TResource*>::iterator m = m_mapResource.begin();
		while(m!=m_mapResource.end())
		{
			TResource* pResource = m->second;
			pResource->CleanUp();
			SAFE_DELETE(pResource);
			m++;
		}
		m_mapResource.clear();
		m_hashmapResource.clear();
		m_listReleaseQueue.clear();
		return S_OK;
	}

	KTResourceManagerBase()
	{
		m_dwReleaseQueueSzie = 20;
		m_dwNextID = 1;
	}
	virtual ~KTResourceManagerBase()
	{
		;
	}
protected:
	virtual HRESULT ReleaseResource(TResource* pT,DWORD dwOption = RELEASE_INQUEUE)
	{
		if(dwOption==RELEASE_INQUEUE)
		{
			m_listReleaseQueue.push_back(pT);
			if(m_listReleaseQueue.size()>m_dwReleaseQueueSzie)
			{
				list<TResource*>::iterator i = m_listReleaseQueue.begin();
				TResource* pResource = *i;

				m_listReleaseQueue.pop_front();

				map<DWORD,TResource*>::iterator m = m_mapResource.find(pT->m_dwID);
				if(m!=m_mapResource.end())
				{
					m_mapResource.erase(m);
				}

				hash_map<string,TResource*>::iterator h = m_hashmapResource.find(pT->m_scName);
				if(h!=m_hashmapResource.end())
				{
					m_hashmapResource.erase(h);
				}

				pResource->CleanUp();
				SAFE_DELETE(pResource);

			}
		}
		else
		{
			pT->CleanUp();

			map<DWORD,TResource*>::iterator i = m_mapResource.find(pT->m_dwID);
			if(i!=m_mapResource.end())
			{
				m_mapResource.erase(i);
			}

			SAFE_DELETE(pT);
		}
		return S_OK;
	}
};

class KResourceBase
{
protected:
	int    m_nRefrence;//物品使用计数

public:
	DWORD  m_dwID;//物品ID
	string m_scName;

public:

	virtual HRESULT LoadFromFile(LPSTR pFileName,DWORD dwOption = 0)
	{
		return S_OK;
	}

	virtual HRESULT CleanUp()//释放资源
	{
		return S_OK;
	}

	virtual HRESULT AddRef()
	{
		m_nRefrence++; 
		return S_OK;
	}

	virtual HRESULT Release()
	{
		m_nRefrence--; 
		if(m_nRefrence<=0)
		{
			return OnRefrenceZero();
		}
		return S_OK;
	}

	virtual HRESULT OnRefrenceZero()//当引用个数降低到0时,默认把自己放到MGR的删除队列里
	{
		return S_OK;
	}

	void Init(DWORD dwID)//初始化
	{
		m_dwID = dwID;

		TCHAR Name[256];
		wsprintfA(Name,"Resource%d",m_dwID);
		m_scName = Name;
	}

	KResourceBase()
	{
		m_nRefrence = 0;
	}
	virtual ~KResourceBase()
	{
		m_nRefrence = 0;
	}
};


#endif // !defined(AFX_KResourceManager_H__9C3F564E_0218_4D56_AF1D_1E90C01E8AD6__INCLUDED_)
