////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DCommonObjectContainerNode.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-6-25 9:57:13
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KG3DCommonObjectContainerNode.h"


////////////////////////////////////////////////////////////////////////////////


ULONG STDMETHODCALLTYPE KG3DCommonObjectContainerNode::GetType()
{
	return KG3DTYPE_COMMON_OBJ_CONTAINER_NODE;
}

KG3DCommonObject* STDMETHODCALLTYPE KG3DCommonObjectContainerNode::Add( LPTSTR lpFileName, DWORD_PTR WParam, DWORD_PTR LParam, KG3DCOBJ_HANDLE* pHandle )
{
	KG_PROCESS_ERROR(NULL != m_Builder);
	{
		KG3DCommonObject* p = (*m_Builder)(lpFileName, WParam, LParam);
		KG_PROCESS_ERROR(NULL != p);
		HRESULT hr = GetCommonObjectPool().Push(p, pHandle);
		if (FAILED(hr))
		{
			SAFE_RELEASE(p);
			goto Exit0;
		}
		return p;
	}
Exit0:
	return NULL;
}
ULONG STDMETHODCALLTYPE KG3DCommonObjectContainerNode::GetCount()
{
	return GetCommonObjectPool().GetCount();
}

HRESULT STDMETHODCALLTYPE KG3DCommonObjectContainerNode::Del( KG3DCOBJ_HANDLE Handle )
{
	return GetCommonObjectPool().Del(Handle);
}

HRESULT STDMETHODCALLTYPE KG3DCommonObjectContainerNode::Clear()
{
	return GetCommonObjectPool().Clear();
}

KG3DCommonObject* STDMETHODCALLTYPE KG3DCommonObjectContainerNode::Get( const KG3DCOBJ_HANDLE& Handle )
{
	return GetCommonObjectPool().Get(Handle);
}