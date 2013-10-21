////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DCommonObject.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-4-1 14:21:07
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "KG3DCommonObject.h"
#include "../../../Include/KG3DEngine/KG3DTypes.h"

////////////////////////////////////////////////////////////////////////////////

//ÏÂÃæÓÐplacement new
#ifndef KG_ENABLE_TDD
#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

//ULONG STDMETHODCALLTYPE KG3DCommonObjectSimple::GetType() 
//{
//	return KG3DTYPE_COMMON_OBJECT_SIMPLE;
//}

ULONG STDMETHODCALLTYPE KG3DCommonObjectSimple::Release( void )
{
	this->UnInit();
	::delete this; 
	return 0;
}

ULONG STDMETHODCALLTYPE KG3DCommonObjectSimpleRef::Release( void )
{
    ULONG uNewRefCount = (ULONG)KG_InterlockedDecrement((long *)&m_ulRefCount);
    if (uNewRefCount == 0)
    {
        UnInit();
        ::delete this; 
    }
    return uNewRefCount;
}

ULONG STDMETHODCALLTYPE KG3DCommonObjectSimpleRef::AddRef( void )
{
	return (ULONG)KG_InterlockedIncrement((long *)&m_ulRefCount);
}



ULONG STDMETHODCALLTYPE KG3DCommonObjectDummy::GetType()
{
	return KG3DTYPE_NONE;
}

#if defined(KG_ENABLE_TDD)
KG_TEST_BEGIN(KG3DCommonObject)
	{
		{
			KG3DCOBJ_HANDLE HandleArray[] = 
			{
				KG3DCOBJ_HANDLE(0, INT_MIN),
				KG3DCOBJ_HANDLE(1, INT_MIN),
				KG3DCOBJ_HANDLE(1, INT_MIN + 1),
				KG3DCOBJ_HANDLE(UINT_MAX, INT_MIN),
				KG3DCOBJ_HANDLE(UINT_MAX, INT_MAX),
			};

			for (size_t i = 0; i < _countof(HandleArray) -1; ++i)
			{
				_ASSERTE(HandleArray[i] < HandleArray[i + 1]);
			}
			
		}
		
		{
			static size_t stAObjClassInstanceCount = 0;
			class AObjClass : public KG3DCommonObjectSimple
			{
			public:
				STDMETHOD_(ULONG, GetType)(){return KG3DTYPE_NONE;}
				AObjClass(){++stAObjClassInstanceCount;}
				~AObjClass(){--stAObjClassInstanceCount;}
				using KG3DCommonObjectSimple::operator delete;
			};

			AObjClass* p = new AObjClass;
			_ASSERTE(NULL != p && stAObjClassInstanceCount == 1);

			SAFE_RELEASE(p);
			_ASSERTE(stAObjClassInstanceCount == 0);
		}
		{
			static size_t stBObjClassInstanceCount = 0;
			class BObjClass : public KG3DCommonObjectSimpleRef
			{
			public:
				STDMETHOD_(ULONG, GetType)(){return KG3DTYPE_NONE;}
				BObjClass(){++stBObjClassInstanceCount;}
				~BObjClass(){--stBObjClassInstanceCount;}
				using KG3DCommonObjectSimpleRef::operator delete;
			};

			BObjClass* p = new BObjClass;
			_ASSERTE(NULL != p && stBObjClassInstanceCount == 1);

			ULONG Ref = p->AddRef();
			_ASSERTE(Ref == 2);
			Ref = p->Release();
			_ASSERTE(Ref == 1);
			Ref = p->Release();
			_ASSERTE(Ref == 0 && stBObjClassInstanceCount == 0);
		}

	}
KG_TEST_END(KG3DCommonObject)
#endif

