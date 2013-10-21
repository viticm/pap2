////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DCommonContainer.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-3-13 14:36:44
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "KG3DCommonContainer.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


STDMETHODIMP KG3DCommonContainerSimple::GetFirst( IEKG3DCommonObject** ppObj, KG3DCOBJ_HANDLE* pdwFirstHandle )
{
	KG_PROCESS_ERROR(ppObj != NULL);
	{
		KG3DCommonObject* p = this->GetFirst(pdwFirstHandle);
		KG_PROCESS_ERROR(p != NULL);
		*ppObj = p;
		return S_OK;
	}
Exit0:
	return E_FAIL;
}

STDMETHODIMP KG3DCommonContainerSimple::GetNext( KG3DCOBJ_HANDLE Handle, IEKG3DCommonObject** ppObj, KG3DCOBJ_HANDLE* pNextHandle ) /*/返回的是下一个的Handle */
{
	KG_PROCESS_ERROR(ppObj != NULL);
	{
		KG3DCommonObject* p = this->GetNext(Handle, pNextHandle);
		KG_PROCESS_ERROR(p != NULL);
		*ppObj = p;
		return S_OK;
	}
Exit0:
	return E_FAIL;
}

STDMETHODIMP KG3DCommonContainerSimple::Get( ULONG Index, IEKG3DCommonObject** ppObj )
{
	KG_PROCESS_ERROR(ppObj != NULL);
	{
		KG3DCommonObject* p = this->Get(Index);
		KG_PROCESS_ERROR(p != NULL);
		*ppObj = p;
		return S_OK;
	}
Exit0:
	return E_FAIL;
}

STDMETHODIMP KG3DCommonContainerSimple::Add( LPTSTR lpFileName, DWORD_PTR WParam, DWORD_PTR LParam , IEKG3DCommonObject** ppObj, KG3DCOBJ_HANDLE* pHandle )
{
	KG3DCommonObject* p = this->Add(lpFileName, WParam, LParam, pHandle);
	KG_PROCESS_ERROR(p != NULL);
	if(ppObj)
		*ppObj = p;
	return S_OK;
Exit0:
	return E_FAIL;
}

STDMETHODIMP KG3DCommonContainerSimple::GetMessageSenderInterface( IKG3DMessageSenderBase** ppRet )
{
	if (ppRet)
	{
		IKG3DMessageSenderBase* p = this->GetMessageSenderInterface();
		if (p)
		{
			*ppRet = p;
			return S_OK;
		}
	}
	return E_FAIL;
}

STDMETHODIMP_(ULONG) KG3DCommonContainerSimple::GetType()
{
	return KG3DTYPE_COMMON_OBJECT_CONTAINER;
}

ULONG STDMETHODCALLTYPE KG3DCommonContainerSimple::Release( void )
{
	::delete this;
	return 0;
}
ULONG STDMETHODCALLTYPE KG3DDummyCommonContainer::GetType()
{
	return KG3DTYPE_NONE;
}


#if defined(KG_ENABLE_TDD)
#include "KG3DMessenger.h"
KG_TEST_BEGIN(KG3DCommonContainer)
{
	{
		class AContainer : public KG3DDummyCommonContainer
		{

		};
		class AContainerWithSender : public KG3DDummyCommonContainer, public KG3DMessageSender
		{
		public:
			STDMETHOD_(IKG3DMessageSenderBase*,GetMessageSenderInterface)(){return this;}
		};

		AContainer A;
		_ASSERTE(NULL == A.GetMessageSenderInterface());
		AContainerWithSender B;
		_ASSERTE(NULL != B.GetMessageSenderInterface());

		IKG3DMessageSenderBase* p = NULL;
		KG3DCommonContainer& RefB = B;
		HRESULT hr =RefB.GetMessageSenderInterface(&p);
		_ASSERTE(SUCCEEDED(hr));


	}

	//::OutputDebugString("KG3DCommonContainer Runtime Test Ok ");
}
KG_TEST_END(KG3DCommonContainer)
#endif