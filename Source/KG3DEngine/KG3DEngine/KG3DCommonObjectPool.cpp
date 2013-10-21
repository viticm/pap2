////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DCommonObjectPool.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-4-10 17:00:13
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KG3DCommonObjectPool.h"


#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


////////////////////////////////////////////////////////////////////////////////

KG3DCommonObjectPool& g_GetKG3DCommonObjectPoolDummy()
{
	static KG3DCommonObjectPoolDummy DummyPool;
	return DummyPool;
}

KG3DCommonObjectPool::~KG3DCommonObjectPool()
{
	Clear();
	SAFE_DELETE(m_pAccelaratorData);
}

ULONG STDMETHODCALLTYPE KG3DCommonObjectPool::GetType()
{
	return KG3DTYPE_COMMON_OBJECT_POOL;
}
ULONG STDMETHODCALLTYPE KG3DCommonObjectPool::GetContainedType()
{
	return KG3DTYPE_COMMON_OBJECT;
}


HRESULT STDMETHODCALLTYPE KG3DCommonObjectPool::Clear()
{
	for (TypeCon::iterator it = m_Con.begin(); it != m_Con.end(); ++it)
	{
		TypeVec& Vec = it->second;
		for (size_t i = 0; i < Vec.size(); ++i)
		{
			KG3DCommonObject* p = Vec[i];
			p->Release();
		}
	}
	m_Con.clear();
	m_Count = 0;

	if (m_pAccelaratorData)
	{
		size_t Count = m_pAccelaratorData->m_vecObj.size();
		for(size_t i = 0; i < Count; ++i)
			m_pAccelaratorData->m_vecObj[i] = NULL;
	}

	return S_OK;
}

ULONG STDMETHODCALLTYPE KG3DCommonObjectPool::GetCount()
{
	return m_Count;
}

HRESULT STDMETHODCALLTYPE KG3DCommonObjectPool::Push( KG3DCommonObject* p, KG3DCOBJ_HANDLE* pObjectRet )
{
	if (p == NULL)
		return E_FAIL;
	DWORD dwType = p->GetType();
	TypeVec* pVec = NULL;
	try
	{
		TypeVec& Vec = m_Con[dwType];
		pVec = &Vec;		
	}
	catch (...)
	{
		return E_FAIL;
	}
	
	if (NULL== pVec)
		return E_FAIL;
	
	pVec->push_back(p);
	m_Count++;

	KG3DCOBJ_HANDLE HandleOfTheObject(dwType, (INT)(pVec->size() - 1));

	//需要加速的话，同时放入加速表
	if (NULL != m_pAccelaratorData)
	{
		TypeAccelaratorPoses::iterator it 
			= m_pAccelaratorData->m_mapOfPos.find(HandleOfTheObject);	
		if (it != m_pAccelaratorData->m_mapOfPos.end())
		{
			size_t uPos = it->second;
			_ASSERTE(uPos < m_pAccelaratorData->m_vecObj.size());
			_ASSERTE(m_pAccelaratorData->m_vecObj[uPos] == NULL);
			m_pAccelaratorData->m_vecObj[uPos] = p;	//在指定的地方再存放一份
		}		
	}

	if (NULL != pObjectRet)
	{
		*pObjectRet = HandleOfTheObject;
	}

	return S_OK;
}

KG3DCommonObject* STDMETHODCALLTYPE KG3DCommonObjectPool::Get(const KG3DCOBJ_HANDLE& Handle)
{
	TypeCon::iterator it = m_Con.find(Handle.m_dwType);
	if (it != m_Con.end())
	{
		TypeVec& Vec = it->second;
		if ((INT)Vec.size() > Handle.m_nIndex && Handle.m_nIndex >= 0)
		{
			return Vec[Handle.m_nIndex];			
		}
	}
	return NULL;
}

KG3DCommonObjectPool::KG3DCommonObjectPool()
:m_Count(0)
,m_pAccelaratorData(NULL)
{
	
}

HRESULT STDMETHODCALLTYPE KG3DCommonObjectPool::InitAccelaratorTable( KG3DCOBJ_HANDLE* pTable, size_t uTableCount )
{
	KGLOG_PROCESS_ERROR(NULL == m_pAccelaratorData);
	m_pAccelaratorData = new AccelaratorData;
	KG_PROCESS_ERROR(NULL != m_pAccelaratorData);
	for (size_t i = 0; i < uTableCount; ++i)
	{
		m_pAccelaratorData->m_mapOfPos[pTable[i]] = i;
	}
	m_pAccelaratorData->m_vecObj.assign(uTableCount, NULL);	//预先分配那么多空间
	return S_OK;
Exit0:
	return E_FAIL;
}

KG3DCommonObject* STDMETHODCALLTYPE KG3DCommonObjectPool::GetFast( size_t iPos )
{
	_ASSERTE(NULL != m_pAccelaratorData && m_pAccelaratorData->m_vecObj.size() > iPos);
	return m_pAccelaratorData->m_vecObj[iPos];
}

HRESULT STDMETHODCALLTYPE KG3DCommonObjectPool::Traverse( KG3DCommonObjectVisitor& visitor )
{
	HRESULT hr = E_FAIL;
	TypeCon::iterator itEnd = m_Con.end();
	for (TypeCon::iterator it = m_Con.begin(); it != itEnd; ++it)
	{
		TypeVec& Vec = it->second;
		size_t Size = Vec.size();
		for (size_t i = 0; i < Size; ++i)
		{
			KG3DCommonObject* p = Vec[i];
			_ASSERTE(NULL != p);
			hr = visitor.Accept(*p);
			KG_COM_PROCESS_ERROR(hr);
		}
	}
	return S_OK;
Exit0:
	return hr;
}

HRESULT STDMETHODCALLTYPE KG3DCommonObjectPool::Del( const KG3DCOBJ_HANDLE& Handle )
{
	TypeCon::iterator it = m_Con.find(Handle.m_dwType);
	KG_PROCESS_ERROR(it != m_Con.end());
	{
		TypeVec& Vec = it->second;
		KG_PROCESS_ERROR(Vec.size() > (size_t)Handle.m_nIndex && Handle.m_nIndex >= 0);

		KG3DCommonObject* p = Vec[Handle.m_nIndex];
		p->Release();
		TypeVec::iterator itErase = Vec.begin()+Handle.m_nIndex;
		Vec.erase(itErase);

		if (NULL != m_pAccelaratorData)
		{
			TypeVec& VecAccet = m_pAccelaratorData->m_vecObj;
			for (size_t i = 0; i < VecAccet.size(); ++i)
			{
				if(VecAccet[i] == p)
					VecAccet[i] = NULL;
			}
		}

	}
	return S_OK;
Exit0:
	return E_FAIL;
}
