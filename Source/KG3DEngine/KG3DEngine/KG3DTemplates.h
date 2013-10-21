
#if !defined(AFX_KResourceManager_H__9C3F564E_0218_4D56_AF1D_1E90C01E8AD6__INCLUDED_)
#define AFX_KResourceManager_H__9C3F564E_0218_4D56_AF1D_1E90C01E8AD6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_PATH_LENGTH 256

#include "Engine/KGLog.h"
#include "Engine/KMutex.h"
#include "kg3dinterface.h"
#include "KG3DResourcePacker.h"
using namespace std;
using namespace stdext;

#define RELEASE_INQUEUE  0
#define RELEASE_ATONCE   1

extern DWORD g_dwMainThreadID;
enum
{
	KG3DGET1NEWRESOURCE_FILE_NAME_FOR_HASH = 0x12345678
};

struct KG3DGet1NewResourceOptionStruct 
{
	DWORD dwOptionIdentifier;///用这个来标志究竟是什么Option
	LPCTSTR czFileName;
};

//GetTypeInfoByFileName使用的参数
struct TypeInfo
{
	TCHAR strExt[MAX_PATH];//后缀名
	DWORD dwType;          //类型
	BOOL  bAlwaysNew;      //是否总是重建
};
extern float g_fMemoryCoefficient;
extern DWORD g_dwID2Find;
extern bool g_FindInReleaseList(IKG3DResourceBase *pRes);
extern KG3DEngineOption g_cEngineOption;

extern TypeInfo g_GloableDefaultResourceType;

template <class TResource>
class TKG3DResourceManagerBase : public IKG3DResourceManager
{
	friend  TResource;
public:	
	typedef std::map<DWORD,TResource*> TypeResourceMap;
	typedef std::list<TResource*> TypeExclusiveList;
	typedef std::list<TResource*> TypeReleaseQueue;
	struct ReleaseCmd
	{
		TResource* pT;
		DWORD dwOption;
		bool operator ==(const ReleaseCmd& A)
		{
			return pT == A.pT;
		}
	};
	
	TKG3DResourceManagerBase()
		:m_pDefaultType(NULL)
		,m_RefTable(NULL)
		,m_RefTableSize(0)
        ,m_dwReleaseQueueSize(200)
	{
	}

	virtual ~TKG3DResourceManagerBase()
	{
        ASSERT(m_lstReleaseQueue.empty());
        ASSERT(m_mapResource.empty());
        ASSERT(m_listExclusiveRes.empty());
	}

	virtual HRESULT Init()
	{
		UnInit();

        ASSERT(g_fMemoryCoefficient >= 0.0f);
        m_dwReleaseQueueSize = (unsigned)(m_dwReleaseQueueSize * g_fMemoryCoefficient);

		m_pDefaultType = &g_GloableDefaultResourceType;
		return S_OK;
	}

	virtual HRESULT UnInit()
	{
		m_Lock.Lock();

		TypeExclusiveList::iterator itExclusive = m_listExclusiveRes.begin();
		while (itExclusive != m_listExclusiveRes.end())
		{
			TResource *pResource = *itExclusive;

			if (pResource->m_scName.size())
			{
				KGLogPrintf(KGLOG_ERR, "资源%s未正常释放.", pResource->m_scName.c_str());
			}
			else
			{
				KGLogPrintf(KGLOG_ERR, "无名资源释放. Type = %d", pResource->GetType());
			}
			pResource->UnInit();
			itExclusive++;
		}

		TypeResourceMap::iterator m = m_mapResource.begin();
		while(m != m_mapResource.end())
		{
			TResource* pResource = m->second;
			if (FAILED(pResource->UnInit()))
			{
				KGLogPrintf(KGLOG_ERR,"释放资源失败");
				_KG3D_DEBUG_OUTPUT0("释放资源失败");
			}

			if (pResource->m_scName.size())
			{
				KGLogPrintf(KGLOG_ERR, "资源强制释放 %s",pResource->m_scName.c_str());
			}
			else
			{
				KGLogPrintf(KGLOG_ERR, "无名资源释放. Type = %d", pResource->GetType());
			}
			m++;
		}

		TypeReleaseQueue::iterator i = m_lstReleaseQueue.begin();
		while(i != m_lstReleaseQueue.end())
		{
			TResource* pResource = *i;
			if (FAILED(pResource->UnInit()))
			{
				if (pResource->m_scName.size())
				{
					KGLogPrintf(KGLOG_ERR,"释放资源%s 失败.", pResource->m_scName.c_str());
				}
				else
				{
					KGLogPrintf(KGLOG_ERR, "无名资源释放. Type = %d", pResource->GetType());
				}
				_KG3D_DEBUG_OUTPUT0("释放资源失败");
			}
			i++;
		}

		//这里是必须这样写的, 因为会有模型的引用关系, 可能导致强制释放本来可以正常释放的资源
		itExclusive = m_listExclusiveRes.begin();
		while (itExclusive != m_listExclusiveRes.end())
		{
			TResource *pResource = *itExclusive;
			SAFE_DELETE(pResource);
			itExclusive++;
		}

		m = m_mapResource.begin();
		while (m != m_mapResource.end())
		{
			TResource* pResource = m->second;
			SAFE_DELETE(pResource);
			m++;
		}
		m_mapResource.clear();

		i = m_lstReleaseQueue.begin();
		while(i != m_lstReleaseQueue.end())
		{
			TResource* pRes = *i;
			SAFE_DELETE(pRes);
			i++;
		}
		m_lstReleaseQueue.clear();
		m_listExclusiveRes.clear();

		/*i = m_listReleaseMultiThread.begin();
		while(i != m_listReleaseMultiThread.end())
		{
			TResource* pRes = *i;
			SAFE_DELETE(pRes);
			i++;
		}*/
		m_listReleaseMultiThread.clear();

		m_RefTable = NULL;
		m_RefTableSize = 0;
		m_pDefaultType = NULL;

		m_Lock.Unlock();
        return S_OK;
	}

	virtual HRESULT NewOneResourse(unsigned uType, IKG3DResourceBase **ppRetResource)
	{
		return Get1NewResource((TResource**)ppRetResource,uType,0);
	}

	virtual HRESULT Get1NewResource(TResource** ppT, DWORD dwType, DWORD_PTR Option)
	{
		HRESULT hRetCode = E_FAIL;
		HRESULT hResult  = E_FAIL;
		
		DWORD dwIDToSet  = 0;

		m_Lock.Lock();
		
		KG_PROCESS_ERROR(ppT);
		*ppT = NULL;

		hResult = Get1NewResourcePointer(ppT, dwType, Option);
		KG_COM_PROCESS_ERROR(hResult);

		(*ppT)->AddRef();
		/*
		统一外部资源和内部创建资源的统一管理，ID作为句柄使用，访问会更加安全，也很容易实现外部访问
		外部通过把Option变成KG3DGet1NewResourceOptionStruct*的结构，然后结构的dwOptionIdentifier设为
		KG3DGET1NEWRESOURCE_FILE_NAME_FOR_HASH（为了防止错误内存访问），这样就可以用Struct中的czFileName
		成员得到一个字符串了。这样就可以预先在map中塞入资源，而这个字符串也可以得到一个ID作为句柄使用
		*/
		if (Option)
		{
			typedef TypeResourceMap typehashmap;
			KG3DGet1NewResourceOptionStruct* pNROStruct = (KG3DGet1NewResourceOptionStruct*)Option;
			if (pNROStruct->dwOptionIdentifier == KG3DGET1NEWRESOURCE_FILE_NAME_FOR_HASH)
			{
				KG_PROCESS_ERROR(pNROStruct->czFileName);
				DWORD dwID = g_FileNameHash(pNROStruct->czFileName);
				std::pair<typehashmap::iterator, bool> pairRet 
					= m_mapResource.insert(typehashmap::value_type(dwID, *ppT));
				KG_PROCESS_ERROR(pairRet.second);///已存在的就会insert失败
				dwIDToSet = dwID;		
			}
		}
		(*ppT)->SetID(dwIDToSet);
		hRetCode = S_OK;
Exit0:
		if(FAILED(hRetCode) && ppT)
		{
			SAFE_RELEASE(*ppT);
		}

		m_Lock.Unlock();
		return hRetCode;
	}

	virtual HRESULT Get1NewResourcePointer(TResource** ppT, DWORD dwType, DWORD_PTR Option)
	{
		HRESULT hRetCode = E_FAIL;
		KG_PROCESS_ERROR(ppT);

		(*ppT) = new TResource();
		KGLOG_PROCESS_ERROR(*ppT);

		(*ppT)->Init();
		(*ppT)->SetType(dwType);
		hRetCode = S_OK;
Exit0:
		return hRetCode;
	}

	HRESULT ReportResourceUse(LPSTR pTableName)
	{
		DWORD dwSize = GetResourceCount();
		KGLogPrintf(KGLOG_ERR, "=======================================================");
		KGLogPrintf(KGLOG_ERR, "%d %s Resource",dwSize,pTableName);

		int N = 0;
		TypeResourceMap::iterator m = m_mapResource.begin();
		while(m != m_mapResource.end())
		{
			TResource* pResource = m->second;
			if(pResource)
				KGLogPrintf(KGLOG_ERR, "%d %s",N,pResource->m_scName.c_str());
			m++;
			N++;
		}

		TypeExclusiveList::iterator itExclusive = m_listExclusiveRes.begin();
		while (itExclusive != m_listExclusiveRes.end())
		{
			TResource *pResource = *itExclusive;
			if(pResource)
				KGLogPrintf(KGLOG_ERR, "%d %s",N,pResource->m_scName.c_str());
			itExclusive++;
			N++;
		}
		return S_OK;
	}

	DWORD GetResourceCount()
	{
		return (DWORD)(m_mapResource.size() + m_listExclusiveRes.size());
	}

	virtual HRESULT SetBalance(unsigned uSizeMemeryKB, unsigned uSizeVideoKB)
	{
		return S_OK;
	}

	virtual HRESULT LoadResourceFromFile(const char cszFileName[], 
		unsigned uFileNameHash, 
		unsigned uOption, 
		IKG3DResourceBase **ppRetResource)
	{
		return LoadResourceFromFileEx(cszFileName, uFileNameHash, uOption, NULL, NULL, ppRetResource);
	}

	virtual HRESULT LoadResourceFromFileEx(const char cszFileName[], 
		unsigned uFileNameHash,
		unsigned uOption, 
		DWORD_PTR WParam, 
		DWORD_PTR LParam, 
		IKG3DResourceBase **ppRetResource)
	{
		TypeInfo* pTypeInfo = NULL;

		HRESULT hRetCode    = E_FAIL;
		HRESULT hResult     = E_FAIL;
		TResource* pTReturn = NULL;
		DWORD   dwID        = 0;
		typedef std::pair<TypeResourceMap::iterator, bool> TypePairRet;

		KG_PROCESS_ERROR(ppRetResource);
		KG_PROCESS_ERROR(cszFileName);

        ASSERT(*ppRetResource == NULL);

		m_Lock.Lock();

		hResult = GetTypeInfoByFileName(&pTypeInfo, cszFileName);
		KG_COM_PROCESS_ERROR(hResult);
		_ASSERTE(pTypeInfo);

		dwID = g_FileNameHash(cszFileName);

		if(! pTypeInfo->bAlwaysNew)
		{
			//搜索已经加载的资源
			map<DWORD, TResource*>::iterator h = m_mapResource.find(dwID);
			if (h != m_mapResource.end())
			{
				//找到了,直接使用
				pTReturn = h->second;
				KG_PROCESS_ERROR(pTReturn);

				pTReturn->RefreshOption(uOption);
				pTReturn->AddRef();

				ReleaseCmd cmd;//确保复用的资源从延迟卸载队列中删除
				cmd.pT = pTReturn;
				cmd.dwOption = 0;
				m_listReleaseMultiThread.remove(cmd);

				goto Exit1;
			}
			//查找是否在释放队列中
			g_dwID2Find = dwID;
			TypeReleaseQueue::iterator itErase = find_if(m_lstReleaseQueue.begin(), 
				m_lstReleaseQueue.end(), 
				&g_FindInReleaseList);
			if (itErase != m_lstReleaseQueue.end())
			{
				pTReturn = *itErase;
				KG_PROCESS_ERROR(pTReturn);

				pTReturn->RefreshOption(uOption);
				pTReturn->AddRef();
				m_lstReleaseQueue.erase(itErase);
				
				TypePairRet pairRet = m_mapResource.insert(map<DWORD, TResource*>::value_type(dwID, pTReturn));
				KG_PROCESS_ERROR(pairRet.second);///检查insert，重ID的话，Insert会失败

				goto Exit1;
			}
		}

		//AlwaysNew的资源
		hResult = Get1NewResourcePointer(&pTReturn, pTypeInfo->dwType, uOption);
		KG_COM_PROCESS_ERROR(hResult);
		
		pTReturn->AddRef();
		pTReturn->SetID(dwID);
		//请不要删除下面两行，这里表明了一种设计约定
		//pTReturn->SetType(dwType);//这个移动到Get1NewResourcePointer
		//pTReturn->m_scName = cszFileName;//这个不再在这里强制执行，每个Type自己在Load里面决定名字

		hResult = pTReturn->LoadFromFileEx(cszFileName, 0, uOption, WParam, LParam);
		if(FAILED(hResult))
		{
			if(pTReturn)
			{
				pTReturn->UnInit();
			}
			SAFE_DELETE(pTReturn);//注意，这个时候，这个对象没有放入到map里面，所以Release是不成功的，必须使用Delete
			pTReturn = NULL;
			goto Exit0;
		}

		if (pTypeInfo->bAlwaysNew)
		{
			m_listExclusiveRes.push_back(pTReturn);
		}
		else
		{
			m_mapResource[dwID] = pTReturn;
		}
Exit1:
		*((TResource**)ppRetResource) = pTReturn;
		hRetCode = S_OK;
Exit0:
		if(FAILED(hRetCode))
		{
			SAFE_RELEASE(pTReturn);	
		}

		m_Lock.Unlock();

		return hRetCode;
	}

	virtual HRESULT GetResource(TResource** ppT, DWORD dwID)//会加引用
	{
		HRESULT hRetCode = E_FAIL;
		TypeResourceMap::iterator i;
		
		KG_PROCESS_ERROR(ppT);

		m_Lock.Lock();

        i = m_mapResource.find(dwID);
		KG_PROCESS_ERROR(i != m_mapResource.end());

		(*ppT) = i->second;
		(*ppT)->AddRef();

		ReleaseCmd cmd;//确保复用的资源从延迟卸载队列中删除
		cmd.pT = (*ppT);
		cmd.dwOption = 0;
		m_listReleaseMultiThread.remove(cmd);

		
		hRetCode = S_OK;
Exit0:
		m_Lock.Unlock();
		return hRetCode;
	}

	virtual HRESULT CheckReleaseMultiThread()
	{
		DWORD dwThreadId = ::GetCurrentThreadId();
		KGLOG_PROCESS_ERROR(dwThreadId == g_dwMainThreadID);

		m_Lock.Lock();
		if(m_listReleaseMultiThread.size())
		{
			std::list<ReleaseCmd>::iterator i = m_listReleaseMultiThread.begin();
			std::list<ReleaseCmd>::iterator iend = m_listReleaseMultiThread.end();
			while (i!=iend)
			{
				ReleaseCmd& cmd = *i;
				_ReleaseResource(cmd.pT,cmd.dwOption);
				i++;
			}
			m_listReleaseMultiThread.clear();

		}
		m_Lock.Unlock();
Exit0:
		return S_OK;
	}

	virtual HRESULT ReleaseResource(TResource* pT, DWORD dwOption = RELEASE_INQUEUE)
	{
		HRESULT hr = E_FAIL;

		DWORD dwThreadId = ::GetCurrentThreadId();
		m_Lock.Lock();

		if(dwThreadId == g_dwMainThreadID)
		{
			ReleaseCmd cmd;
			cmd.pT = pT;
			cmd.dwOption = dwOption;
			m_listReleaseMultiThread.remove(cmd);
			hr = _ReleaseResource(pT,dwOption);
		}
		else
		{			
			ReleaseCmd cmd;
			cmd.pT = pT;
			cmd.dwOption = dwOption;
			m_listReleaseMultiThread.push_back(cmd);
			hr = S_OK;
		}
		m_Lock.Unlock();
		return hr;
	}

	virtual HRESULT _ReleaseResource(TResource* pT, DWORD dwOption = RELEASE_INQUEUE)
	{
		HRESULT hRetCode   = E_FAIL;
		HRESULT hResult    = E_FAIL;
		TypeInfo* pTypeInfo = NULL;

		DWORD   dwID       = 0;
		TypeResourceMap::iterator itErease;

		KG_PROCESS_ERROR(pT);

		dwID = pT->GetID();

		hResult = GetTypeInfoByResourceType(&pTypeInfo, pT->GetType());
		KG_COM_PROCESS_ERROR(hResult);
		_ASSERTE(pTypeInfo);

		if (pTypeInfo->bAlwaysNew)
		{
			TypeExclusiveList::iterator itErease = find(m_listExclusiveRes.begin(), 
				m_listExclusiveRes.end(), 
				pT);
			if(itErease != m_listExclusiveRes.end())//;KG_PROCESS_ERROR(itErease != m_listExclusiveRes.end());
				m_listExclusiveRes.erase(itErease);	

			pT->UnInit();
			SAFE_DELETE(pT);
			goto Exit1;
		}
				
		if (dwID == 0)
		{
			pT->UnInit();
			SAFE_DELETE(pT);
			goto Exit1;
		}

		itErease = m_mapResource.find(dwID);
		KG_PROCESS_ERROR(itErease != m_mapResource.end());

		m_mapResource.erase(itErease);
		
		if (dwOption == RELEASE_INQUEUE)
		{
			TypeReleaseQueue::iterator itFind = find(m_lstReleaseQueue.begin(), m_lstReleaseQueue.end(), pT);
			//不可能出现在释放队列中， 同时又被再次加入队列的情况
			KG_PROCESS_ERROR(itFind == m_lstReleaseQueue.end());

			m_lstReleaseQueue.push_back(pT);

			if (m_lstReleaseQueue.size() > m_dwReleaseQueueSize)
			{
				//队列长度超过了预设值，清理掉时间最久的
				TResource *pResource = *(m_lstReleaseQueue.begin());
				pResource->UnInit();
				SAFE_DELETE(pResource);
				m_lstReleaseQueue.pop_front();
			}
			goto Exit1;
		}
		pT->UnInit();
		SAFE_DELETE(pT);
Exit1:
		hRetCode = S_OK;
Exit0:
		return hRetCode;
	}


	HRESULT OnLostDevice()
	{
		m_Lock.Lock();

        TypeResourceMap::iterator itResource       = m_mapResource.begin();
		TypeResourceMap::iterator itResourceEnd    = m_mapResource.end();
		TypeExclusiveList::iterator itExclusive    = m_listExclusiveRes.begin();
		TypeExclusiveList::iterator itExclusiveEnd = m_listExclusiveRes.end();
		TypeReleaseQueue::iterator  itRelease      = m_lstReleaseQueue.begin();
		TypeReleaseQueue::iterator  itReleaseEnd   = m_lstReleaseQueue.end();

		while (itResource != itResourceEnd)
		{
			ASSERT(itResource->second);
			itResource->second->OnLostDevice();
			itResource++;
		}

		while (itExclusive != itExclusiveEnd)
		{
			ASSERT(*itExclusive);
			(*itExclusive)->OnLostDevice();
			itExclusive++;
		}

		while (itRelease != itReleaseEnd)
		{
			ASSERT(*itRelease);
			(*itRelease)->OnLostDevice();
			itRelease++;
		}

        m_Lock.Unlock();
        return S_OK;
	}

	HRESULT OnResetDevice()
	{
		m_Lock.Lock();

        TypeResourceMap::iterator itResource       = m_mapResource.begin();
		TypeResourceMap::iterator itResourceEnd    = m_mapResource.end();
		TypeExclusiveList::iterator itExclusive    = m_listExclusiveRes.begin();
		TypeExclusiveList::iterator itExclusiveEnd = m_listExclusiveRes.end();
		TypeReleaseQueue::iterator  itRelease      = m_lstReleaseQueue.begin();
		TypeReleaseQueue::iterator  itReleaseEnd   = m_lstReleaseQueue.end();

		while (itResource != itResourceEnd)
		{
			ASSERT(itResource->second);
			itResource->second->OnResetDevice();
			itResource++;
		}

		while (itExclusive != itExclusiveEnd)
		{
			ASSERT(*itExclusive);
			(*itExclusive)->OnResetDevice();
			itExclusive++;
		}

		while (itRelease != itReleaseEnd)
		{
			ASSERT(*itRelease);
			(*itRelease)->OnResetDevice();
			itRelease++;
		}

		m_Lock.Unlock();
        return S_OK;
	}

public :
	virtual HRESULT GetTypeInfoByFileName(TypeInfo** ppTypeInfo, const char cszFileName[])
	{
		HRESULT hRetCode = E_FAIL;
		TCHAR strExt[MAX_PATH];
		KG_PROCESS_ERROR(ppTypeInfo);

		_splitpath_s(cszFileName, 
			NULL,
			0, 
			NULL,
			0, 
			NULL,
			0, 
			strExt, 
			MAX_PATH);
		_strlwr_s(strExt, MAX_PATH * sizeof(TCHAR));

		for (size_t i = 0; i < m_RefTableSize; i++)
		{
			if (_tcscmp(strExt, m_RefTable[i].strExt) == 0) //这里应该用小写比较
			{
				*ppTypeInfo = &m_RefTable[i];
				hRetCode = S_OK;
				break;
			}
		}

		if (FAILED(hRetCode) && m_pDefaultType)
		{
			*ppTypeInfo = m_pDefaultType;
			hRetCode = S_OK;
		}

#if defined(DEBUG) | defined(_DEBUG)
		if (m_RefTableSize == 0)
		{
			_ASSERTE(m_pDefaultType);//如果没有定义TypeInfo表，那么至少要在Table的Init函数中定义DefaultType，不然会造成这个函数失败，从而资源不能释放
		}
#endif
		if (FAILED(hRetCode) && m_RefTableSize == 0 && ! m_pDefaultType)
		{
			*ppTypeInfo = &g_GloableDefaultResourceType;
		}
Exit0:
		return hRetCode;
	}

	virtual HRESULT GetTypeInfoByResourceType(TypeInfo** ppTypeInfo, DWORD dwType)
	{
		HRESULT hRetCode = E_FAIL;
		KG_PROCESS_ERROR(ppTypeInfo);

		for (size_t i = 0; i < m_RefTableSize; i++)
		{
			if (m_RefTable[i].dwType == dwType)
			{
				*ppTypeInfo = &m_RefTable[i];
				hRetCode = S_OK;
				break;
			}
		}

#if defined(DEBUG) | defined(_DEBUG)
		if (m_RefTableSize == 0)
		{
			_ASSERTE(m_pDefaultType);//如果没有定义TypeInfo表，那么至少要在Table的Init函数中定义DefaultType，不然会造成这个函数失败，从而资源不能释放
		}
#endif
		if (FAILED(hRetCode) && m_pDefaultType)
		{
			*ppTypeInfo = m_pDefaultType;
			hRetCode = S_OK;
		}

Exit0:
		return hRetCode;
	}

protected :
	KMutex            m_Lock;
    DWORD             m_dwReleaseQueueSize;
	TypeReleaseQueue  m_lstReleaseQueue;//需要释放的资源队列
	TypeResourceMap	  m_mapResource;
	TypeExclusiveList m_listExclusiveRes;
	TypeInfo*         m_RefTable; //GetTypeInfoByFileName的参考表, 需要在实例的Init函数中初始化, 但是不需要在UnInit中付空
	size_t            m_RefTableSize;//参考表的大小
	TypeInfo*         m_pDefaultType;//在参考表项都不匹配时候的处理, 如果是空则返回失败,写数组的时候，TypeInfo的strExt要小写，且要有.
	std::list<ReleaseCmd>m_listReleaseMultiThread;
};

template <class TResource>
	class TKG3DResourcePool
	{
		friend  TResource;
	public:	
		KMutex m_Lock;
		DWORD m_dwMaxPool;
		std::list<TResource*>m_listResourcePool;
		std::set<TResource*>m_setResourceUnDelete;//记录没有被释放的资源

		TResource* RequestResource()
		{
			m_Lock.Lock();
			TResource* pRes = NULL;
			if(m_listResourcePool.size()>0)
			{
				pRes = *m_listResourcePool.begin();
				m_listResourcePool.pop_front();
			}
			else
			{
				pRes = new TResource();
				m_setResourceUnDelete.insert(pRes);
			}
			m_Lock.Unlock();
			return pRes;
		}

		void FreeResource(TResource* pRes)
		{
			m_Lock.Lock();
			if(m_listResourcePool.size() < m_dwMaxPool)
			{
				pRes->UnInit();
				m_listResourcePool.push_back(pRes);
			}
			else
			{
				pRes->UnInit();

				std::set<TResource*>::iterator i = m_setResourceUnDelete.find(pRes);
				if(i!=m_setResourceUnDelete.end())
				{
					m_setResourceUnDelete.erase(i);
					SAFE_DELETE(pRes);
				}
				else
				{
					ASSERT(0);
				}
			}
			m_Lock.Unlock();
		}

		HRESULT UnInit()
		{
			std::list<TResource*>::iterator i = m_listResourcePool.begin();
			std::list<TResource*>::iterator iend = m_listResourcePool.end();
			while (i!=iend)
			{
				TResource* pRes = *i;

				std::set<TResource*>::iterator Find = m_setResourceUnDelete.find(pRes);
				if(Find != m_setResourceUnDelete.end())
				{
					m_setResourceUnDelete.erase(pRes);
					SAFE_DELETE(pRes);
				}
				else
				{
					ASSERT(0);
				}
				i++;
			}
			m_listResourcePool.clear();

			{
				std::set<TResource*>::iterator i = m_setResourceUnDelete.begin();
				std::set<TResource*>::iterator iend = m_setResourceUnDelete.end();
				while (i!=iend)
				{
					TResource* pRes = *i;

					KGLogPrintf(KGLOG_ERR, "资源%x泄露",pRes);
					i++;
				}
			}
			return S_OK;
		};

		HRESULT Init(DWORD dwMaxPool,DWORD dwInitPoolCount)
		{
			m_dwMaxPool = dwMaxPool;
			DWORD dwCount = min(dwMaxPool,dwInitPoolCount);
			for (DWORD i=0;i<dwCount;i++)
			{
				TResource* pResource = RequestResource();
				FreeResource(pResource);
			}
			return S_OK;
		}

		TKG3DResourcePool()
		{
			m_dwMaxPool = 400;
		}

		virtual ~TKG3DResourcePool()
		{
			UnInit();
		}

		HRESULT OnLostDevice()
		{
			std::set<TResource*>::iterator i = m_setResourceUnDelete.begin();
			std::set<TResource*>::iterator iend = m_setResourceUnDelete.end();
			while (i!=iend)
			{
				TResource* pRes = *i;
				if(pRes)
					pRes->OnLostDevice();
				i++;
			}
			return S_OK;
		}

		HRESULT OnResetDevice()
		{
			std::set<TResource*>::iterator i = m_setResourceUnDelete.begin();
			std::set<TResource*>::iterator iend = m_setResourceUnDelete.end();
			while (i!=iend)
			{
				TResource* pRes = *i;
				if(pRes)
					pRes->OnResetDevice();
				i++;
			}
			return S_OK;
		}

		HRESULT GetState(DWORD& dwInUse,DWORD& dwTotal)
		{
			dwTotal = (DWORD)m_setResourceUnDelete.size();
			dwInUse = dwTotal - (DWORD)(m_listResourcePool.size());
			return S_OK;
		}
	};

template <class TResource>
	class TKG3DResourceLimitSharePool
	{
	protected:
		UINT GetCurrentID()
		{
			return m_uCurrentID++;
		}

		HRESULT NewOneShareResource(TResource** ppShareRes)
		{
			TResource* pNewRes = new TResource();
			KGLOG_PROCESS_ERROR(pNewRes);

			pNewRes->Init();
			pNewRes->pMgr = this;

			(*ppShareRes) = pNewRes;
			return S_OK;
Exit0:
			SAFE_DELETE(pNewRes);

			(*ppShareRes) = NULL;
			return E_FAIL;
		}
	public:
		UINT  m_uUnFreshCount;
		UINT  m_uCurrentID;
		DWORD m_dwNumMaxShare;

		map<UINT,TResource*>m_mapShareResource;
	public:

		BOOL IsValid(PVOID pUser,TResource* pResource)
		{
			return pResource->pUser == pUser;
		}

		HRESULT Init(DWORD dwMaxResource,UINT uUnFreshCount)
		{
			UnInit();
			m_dwNumMaxShare = dwMaxResource;
			m_uUnFreshCount = uUnFreshCount;
			return S_OK;
		}

		HRESULT UnInit()
		{
			map<UINT,TResource*>::iterator i = m_mapShareResource.begin();
			map<UINT,TResource*>::iterator iend = m_mapShareResource.end();

			while(i!=iend)
			{
				TResource* pRes = i->second;
				SAFE_DELETE(pRes);
				i++;
			}
			m_mapShareResource.clear();
			return S_OK;
		}

		HRESULT GetOneShareResource(TResource** ppShareRes,PVOID pUser)
		{
			HRESULT hr = S_OK;
			DWORD dwNowTime = timeGetTime();

			if(m_mapShareResource.size()<m_dwNumMaxShare)
			{
				hr = NewOneShareResource(ppShareRes);
				KGLOG_COM_PROCESS_ERROR(hr);

				UINT uID = GetCurrentID();
				m_mapShareResource[uID] = *ppShareRes;

				(*ppShareRes)->uID = uID;
			}
			else
			{
				map<UINT,TResource*>::iterator i = m_mapShareResource.begin();

				TResource* pText = i->second;
				KGLOG_PROCESS_ERROR(pText);

				KG_PROCESS_ERROR(dwNowTime - pText->dwLastTime >= 100);


				(*ppShareRes) = pText;

				m_mapShareResource.erase(i);

				UINT uID = GetCurrentID();
				m_mapShareResource[uID] = *ppShareRes;
				(*ppShareRes)->uID = uID;
			}

			(*ppShareRes)->dwLastTime = dwNowTime;
			(*ppShareRes)->pUser = pUser;

			return S_OK;
Exit0:
			return E_FAIL;
		}

		TKG3DResourceLimitSharePool(void)
		{
			m_dwNumMaxShare = 200;
			m_uCurrentID = 0;
			m_uUnFreshCount = 100;
		};

		~TKG3DResourceLimitSharePool(void)
		{
			UnInit();
		}

		HRESULT OnLostDevice()
		{
			map<UINT,TResource*>::iterator i    = m_mapShareResource.begin();
			map<UINT,TResource*>::iterator iend = m_mapShareResource.end();

			while(i!=iend)
			{
				TResource* pText = i->second;
				if(pText)
				{
					pText->OnLostDevice();
				}
				i++;
			}
			return S_OK;
		}

		HRESULT OnResetDevice()
		{
			map<UINT,TResource*>::iterator i    = m_mapShareResource.begin();
			map<UINT,TResource*>::iterator iend = m_mapShareResource.end();

			while(i!=iend)
			{
				TResource* pText = i->second;
				if(pText)
				{
					pText->OnResetDevice();
				}
				i++;
			}
			return S_OK;
		}

		DWORD GetResourceUse()
		{
			DWORD dwUse = 0;
			map<UINT,TResource*>::iterator i    = m_mapShareResource.begin();
			map<UINT,TResource*>::iterator iend = m_mapShareResource.end();

			while(i!=iend)
			{
				TResource* pText = i->second;
				if(pText)
				{
					dwUse += pText->GetResourceUse();
				}
				i++;
			}
			return dwUse;
		}

		HRESULT OnResourceUse(TResource* pShareRes)
		{
			KGLOG_PROCESS_ERROR(pShareRes);

			if( m_uCurrentID - pShareRes->uID >= m_uUnFreshCount)
			{
				map<UINT,TResource*>::iterator i = m_mapShareResource.find(pShareRes->uID);

				m_mapShareResource.erase(i);

				UINT uID = GetCurrentID();
				m_mapShareResource[uID] = pShareRes;
				pShareRes->uID = uID;
			}

			pShareRes->dwLastTime = timeGetTime();

			return S_OK;
Exit0:
			return E_FAIL;
		}

		HRESULT FreeResource(PVOID pUser,TResource* pShareRes)
		{
			map<UINT,TResource*>::iterator i;
			KGLOG_PROCESS_ERROR(pShareRes);
			KG_PROCESS_ERROR(pShareRes->pUser == pUser);

			i = m_mapShareResource.find(pShareRes->uID);

			KGLOG_PROCESS_ERROR(i!=m_mapShareResource.end());

			pShareRes->OnFreeResource();

			return S_OK;
Exit0:
			return E_FAIL;
		}
	};

class RecordResourceManager : public IKG3DResourceManager
{
protected:	
	IKG3DResourceManager* m_pResourceManager;
public:
	RecordResourceManager()
	{		
		m_pResourceManager = NULL;
	}
	~RecordResourceManager()
	{		

	}
	void SetResourceManager(IKG3DResourceManager* pResourceManager)
	{
		m_pResourceManager = pResourceManager;
	}
	virtual HRESULT Init()
	{
		if (m_pResourceManager)
			return m_pResourceManager->Init();
		else
			return E_FAIL;
	}
	virtual HRESULT UnInit()
	{
		if (m_pResourceManager)
			return m_pResourceManager->UnInit();
		else
			return E_FAIL;
	}

	//////////////////////////////////////////////////////////////////////////
	virtual HRESULT LoadResourceFromFile(const char cszFileName[], unsigned uFileNameHash, unsigned uOption, IKG3DResourceBase **ppRetResource)
	{
		if (m_pResourceManager)
			return m_pResourceManager->LoadResourceFromFile(cszFileName,uFileNameHash,uOption | MLO_MODEL_RECORD,ppRetResource);
		else
			return E_FAIL;
	}
	virtual HRESULT NewOneResourse(unsigned uType,                 IKG3DResourceBase **ppRetResource)
	{
		if (m_pResourceManager)
			return m_pResourceManager->NewOneResourse(uType,ppRetResource);
		else
			return E_FAIL;
	}
	//////////////////////////////////////////////////////////////////////////
	virtual HRESULT SetBalance(unsigned uSizeMemeryKB, unsigned uSizeVideoKB)
	{
		if (m_pResourceManager)
			return m_pResourceManager->SetBalance(uSizeMemeryKB,uSizeVideoKB);
		else
			return E_FAIL;
	}

};
#endif // !defined(AFX_KResourceManager_H__9C3F564E_0218_4D56_AF1D_1E90C01E8AD6__INCLUDED_)
