////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DCommonObjectNode.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-5-28 11:11:51
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KG3DCommonObjectNode.h"
#include "KG3DCommonObjectBuilder.h"
#include "KG3DCommonObjectPool.h"
#include "KG3DTypes.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
////////////////////////////////////////////////////////////////////////////////
KG3DCommonObjectPool& STDMETHODCALLTYPE KG3DCommonObjectNodeDummy::GetCommonObjectPool()
{
	return g_GetKG3DCommonObjectPoolDummy();
}

KG3DCommonObject* STDMETHODCALLTYPE KG3DCommonObjectNodeSimple::Get( const KG3DCOBJ_HANDLE& Handle )
{
	return GetCommonObjectPool().Get(Handle);
}

KG3DCommonObject* STDMETHODCALLTYPE KG3DCommonObjectNodeSimple::Request( const KG3DCOBJ_HANDLE& Handle, LPCTSTR FileName
																  , DWORD_PTR wParam, DWORD_PTR lParam, KG3DCOBJ_HANDLE* pHandle )
{
	KG3DCommonObject* pRet = NULL;
	KG3DCommonObjectPool& Pool = GetCommonObjectPool();
	{
		pRet = Pool.Get(Handle);
		if (NULL == pRet)
		{
			KG3DCommonObjectBuilder* pBuilder = GetCommonObjectBuilder();
			KG_PROCESS_ERROR(NULL != pBuilder);
			KG3DCommonObject* pNew = pBuilder->Build(Handle.m_dwType, FileName, wParam, lParam);
			KG_PROCESS_ERROR(NULL != pNew);
			HRESULT hr = Pool.Push(pNew, pHandle);
			if (FAILED(hr))
			{
				SAFE_RELEASE(pNew);
				goto Exit0;
			}
			pRet = pNew;
		}
	}
Exit0:
	return pRet;
}

HRESULT STDMETHODCALLTYPE KG3DCommonObjectNodeSimple::Save( const KG3DCOBJ_HANDLE& Handle, LPCTSTR FileName, DWORD_PTR wParam, DWORD_PTR lParam )
{
	KG3DCommonObjectBuilder* pBuilder = GetCommonObjectBuilder();
	KG_PROCESS_ERROR(NULL != pBuilder);
	{
		KG3DCommonObjectPool& Pool = GetCommonObjectPool();

		KG3DCommonObject* pObject = Pool.Get(Handle);
		KG_PROCESS_ERROR(NULL != pObject);

		return pBuilder->Save(pObject, FileName, wParam, lParam);
	}
Exit0:
	return E_FAIL;
}
ULONG STDMETHODCALLTYPE KG3DCommonObjectNodeSimple::GetType()
{
	return KG3DTYPE_COMMON_OBJECT_NODE;
}

KG3DCommonObject* STDMETHODCALLTYPE KG3DCommonObjectNodeSimple::GetFast( size_t iPos )
{
	return GetCommonObjectPool().GetFast(iPos);
}

HRESULT STDMETHODCALLTYPE KG3DCommonObjectNodeSimple::Traverse(KG3DCommonObjectVisitor& visitor )
{	
	return GetCommonObjectPool().Traverse(visitor);
}
#ifndef	SPECIAL_VISITOR
#define SPECIAL_VISITOR(className, method)	\
class className##Visitor##1 : public className##Visitor	\
{	\
	virtual HRESULT Accept(className& ObjVisited)	\
	{	\
		ObjVisited.method();	\
		return S_OK;	\
	}	\
};	\
	className##Visitor##1 visitor;	\
	NULL
#endif

HRESULT STDMETHODCALLTYPE KG3DCommonObjectNodeSimple::OnLostDevice()
{
	SPECIAL_VISITOR(KG3DCommonObject, OnLostDevice);	
	return Traverse(visitor);
}

HRESULT STDMETHODCALLTYPE KG3DCommonObjectNodeSimple::OnResetDevice()
{
	SPECIAL_VISITOR(KG3DCommonObject, OnResetDevice);	
	return Traverse(visitor);
}

HRESULT STDMETHODCALLTYPE KG3DCommonObjectNodeSimple::Render()
{
	SPECIAL_VISITOR(KG3DCommonObject, Render);	
	return Traverse(visitor);
}

HRESULT STDMETHODCALLTYPE KG3DCommonObjectNodeSimple::FrameMove()
{
	SPECIAL_VISITOR(KG3DCommonObject, FrameMove);	
	return Traverse(visitor);
}

class KG3DCommonObjectNodeSimpleSolid : public KG3DCommonObjectNodeSimple
{
public:
	STDMETHOD_(ULONG, GetType)(){return KG3DTYPE_COMMON_OBJECT_NODE;}

	STDMETHOD_(KG3DCommonObjectBuilder*, GetCommonObjectBuilder)(){return &m_Builder;}
	STDMETHOD_(KG3DCommonObjectPool&, GetCommonObjectPool)(){return m_Pool;}

	KG3DCommonObjectNodeSimpleSolid(KG3DCOBJ_HANDLE AcceleratorTable[], size_t uElemCount)
	{
		HRESULT hr = m_Pool.InitAccelaratorTable(AcceleratorTable, uElemCount);
		_ASSERTE(SUCCEEDED(hr));
		hr;
	}
private:
	KG3DCommonObjectBuilder m_Builder;
	KG3DCommonObjectPool	m_Pool;
};

KG3DCommonObjectNodeSimple* KG3DCommonObjectNodeSimple::Build( LPCTSTR, DWORD_PTR pKG3DCOBJ_HANDLE_AccelerationTable, DWORD_PTR AccelerationTableCount )
{
	return new KG3DCommonObjectNodeSimpleSolid(reinterpret_cast<KG3DCOBJ_HANDLE*>(pKG3DCOBJ_HANDLE_AccelerationTable)
		, _kg_to_smaller_size_cast<size_t>(AccelerationTableCount));
}