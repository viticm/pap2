////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DRepresentObjectDummy.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-7-18 10:05:40
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KG3DRepresentObjectDummy.h"
#include "KG3DGraphicsTool.h"
#include "KG3DIntersectionBase.h"

////////////////////////////////////////////////////////////////////////////////

KG3DRepresentObjectDummy::KG3DRepresentObjectDummy()
	:m_pBuffer(NULL)
	,m_BufferSize(0)
	,m_fSize(5)
{

}
KG3DRepresentObjectDummy::~KG3DRepresentObjectDummy()
{
	ClearBuffer();
}

HRESULT KG3DRepresentObjectDummy::Copy( KG3DRepresentObject* pSrcObject )
{
	_ASSERTE(pSrcObject->GetType() == this->GetType());
	HRESULT hr = KG3DRepresentObject::Copy(pSrcObject);
	if (SUCCEEDED(hr))
	{
		//Copy的话，要一起注册Receiver
		_KG3D_DEBUG_SUPPERSUB_POINTER_EQUAL(KG3DRepresentObject, KG3DRepresentObjectDummy);
		KG3DRepresentObjectDummy* pSrcDummy = static_cast<KG3DRepresentObjectDummy*>(pSrcObject);
		size_t iCount = pSrcDummy->GetReceiverCount();
		for (size_t i = 0; i < iCount; ++i)
		{
			this->RegisterReceiver(pSrcDummy->GetReceiver(i));
		}

		this->SetBuffer(pSrcDummy->m_pBuffer, pSrcDummy->m_BufferSize);

		m_fSize = pSrcDummy->m_fSize;
	}
	
	return hr;
}

HRESULT KG3DRepresentObjectDummy::Render( DWORD dwOption )
{
	GraphicsStruct::RenderState rsFog(D3DRS_FOGENABLE, FALSE);
	GraphicsStruct::RenderState rsCull(D3DRS_CULLMODE, D3DCULL_NONE);
	GraphicsStruct::RenderState rsZEnable(D3DRS_ZENABLE, TRUE);
	GraphicsStruct::RenderState rsAlpha(D3DRS_ALPHABLENDENABLE, FALSE);
	return g_cGraphicsTool.DrawAABBox(&m_BBox, 0xffffff00);
}

VOID KG3DRepresentObjectDummy::ClearBuffer()
{
	SAFE_DELETE_ARRAY(m_pBuffer);
	m_BufferSize = 0;
}

HRESULT KG3DRepresentObjectDummy::SetBuffer( LPVOID pData, size_t DataSize )
{
	if(NULL == pData || DataSize > max_buffer_size)
		return E_INVALIDARG;

	ClearBuffer();
	_ASSERTE(sizeof(m_ShortBuffer) == sizeof(char)*shot_buffer_size);
	if(DataSize > sizeof(m_ShortBuffer))	//如果都是小数据的话，内存碎片会很多，简单的做成2种Cache吧
	{
		m_pBuffer = new char[DataSize];
		KG_PROCESS_ERROR(NULL != m_pBuffer);
		errno_t err = memcpy_s(m_pBuffer, DataSize, pData, DataSize);
		KG_PROCESS_ERROR(err == 0);
	}
	else
	{
		errno_t err = memcpy_s(m_ShortBuffer, sizeof(m_ShortBuffer), pData, DataSize);
		KG_PROCESS_ERROR(err == 0);
	}

	m_BufferSize = DataSize;
	return S_OK;
Exit0:
	ClearBuffer();
	return E_FAIL;
}

HRESULT KG3DRepresentObjectDummy::GetBuffer( LPVOID pData, size_t DataSize )
{
	if(NULL == pData || DataSize < m_BufferSize)
		return E_INVALIDARG;
	KG_PROCESS_ERROR(m_BufferSize != 0);
	if (m_BufferSize > sizeof(m_ShortBuffer))
	{
		KG_PROCESS_ERROR(NULL != m_pBuffer);
		
		errno_t err = memcpy_s(pData, DataSize, m_pBuffer, m_BufferSize);
		KG_PROCESS_ERROR(err == 0);
	}
	else
	{
		errno_t  err = memcpy_s(pData, DataSize, m_ShortBuffer, m_BufferSize);
		KG_PROCESS_ERROR(err == 0);
	}
	return S_OK;	
Exit0:
	return E_FAIL;
}

HRESULT KG3DRepresentObjectDummy::SetTranslation( const D3DXVECTOR3* pTrans )
{
	HRESULT hr = KG3DRepresentObject::SetTranslation(pTrans);
	if (SUCCEEDED(hr))
	{
		ComputeBBox();
		_ASSERTE(NULL != pTrans);
		this->PostMessage(KG3DMessage(KM_TRANSLATION, reinterpret_cast<DWORD_PTR>(pTrans), EM_EVENT_STATE_END));
	}
	return hr;
}

HRESULT KG3DRepresentObjectDummy::SetRotation( const D3DXQUATERNION* pRotaion )
{
	HRESULT hr = KG3DRepresentObject::SetRotation(pRotaion);
	if (SUCCEEDED(hr))
	{
		_ASSERTE(NULL != pRotaion);
		this->PostMessage(KG3DMessage(KM_ROTATION, reinterpret_cast<DWORD_PTR>(pRotaion), EM_EVENT_STATE_END));
	}
	return hr;
}

HRESULT KG3DRepresentObjectDummy::SetScaling( const D3DXVECTOR3* pScale )
{
	HRESULT hr = KG3DRepresentObject::SetScaling(pScale);
	if (SUCCEEDED(hr))
	{
		_ASSERTE(NULL != pScale);
		this->PostMessage(KG3DMessage(KM_SCALING, reinterpret_cast<DWORD_PTR>(pScale), NULL));
	}
	return hr;
}

HRESULT KG3DRepresentObjectDummy::OnSelect()
{
	this->PostMessage(KG3DMessage(KM_SELECT, NULL, EM_EVENT_STATE_END));
	return S_OK;
}

BOOL KG3DRepresentObjectDummy::RayIntersection( const D3DXVECTOR3& vSrc , const D3DXVECTOR3& vNormalizedDir , FLOAT* pfRet, BOOL bIntersectBoxOnly, KG3DIntersectionFilter* pFilter /*= NULL*/ )
{
	if (NULL != pFilter)
	{
		HRESULT hr = pFilter->Accept(*this);
		if (FAILED(hr))
			return FALSE;
	}
	//只有Box的，bIntersectBoxOnly就不用管了
	return m_BBox.RayIntersection(vSrc, vNormalizedDir, pfRet);
}

HRESULT KG3DRepresentObjectDummy::ComputeBBox()
{
	FLOAT fGap = m_fSize/2;
	D3DXVECTOR3 vSize(fGap, fGap, fGap);
	m_BBox = AABBOX(m_Translation - vSize, m_Translation + vSize);
	return S_OK;
}

HRESULT KG3DRepresentObjectDummy::SetSize( FLOAT fSize )
{
	if(fSize < FLT_EPSILON)
		return E_FAIL;

	m_fSize = fSize;
	ComputeBBox();
	return S_OK;
}
#if defined(KG_ENABLE_TDD)
KG_TEST_BEGIN(KG3DRepresentObjectDummy)
{
	class KG3DRepresentObjectDummyTest : public KG3DRepresentObjectDummy
	{
	public:
		using KG3DRepresentObjectDummy::m_BufferSize;
		using KG3DRepresentObjectDummy::m_ShortBuffer;
		using KG3DRepresentObjectDummy::m_pBuffer;
		using KG3DMessageSender::GetReceiver;
	};
	//检查动态Buffer
	{
		//准备资源
		
		

		char ShortData[sizeof(DWORD_PTR)*2];
		ZeroMemory(ShortData, sizeof(ShortData));

		_ASSERTE(ShortData[_countof(ShortData)-1] == 0);

		char LongData[] = "longlongagothereisnocomputer";
		memcpy_s(ShortData, sizeof(ShortData), LongData, sizeof(ShortData));

		KG3DRepresentObjectDummyTest TestObj;
		char RetBuffer[128];
		{
			_ASSERTE(TestObj.m_pBuffer == NULL && TestObj.m_BufferSize == 0);
			HRESULT hr = TestObj.GetBuffer(RetBuffer, sizeof(RetBuffer));
			_ASSERTE(FAILED(hr));
		}
		{
			HRESULT hr = TestObj.SetBuffer(ShortData, sizeof(ShortData));
			_ASSERTE(SUCCEEDED(hr));
			_ASSERTE(NULL == TestObj.m_pBuffer && 0 == memcmp(TestObj.m_ShortBuffer, ShortData, sizeof(ShortData)));
			_ASSERTE(TestObj.m_BufferSize == sizeof(ShortData));
		}

		{
			HRESULT hr = TestObj.GetBuffer(NULL, sizeof(RetBuffer));
			_ASSERTE(FAILED(hr));
			hr = TestObj.GetBuffer(RetBuffer, 1);
			_ASSERTE(FAILED(hr));

			hr = TestObj.GetBuffer(RetBuffer, sizeof(RetBuffer));
			_ASSERTE(SUCCEEDED(hr));
			_ASSERTE(0 == memcmp(RetBuffer, ShortData, sizeof(ShortData)));
		}

		{
			HRESULT hr = TestObj.SetBuffer(LongData, sizeof(LongData));
			_ASSERTE(SUCCEEDED(hr));
			_ASSERTE(NULL != TestObj.m_pBuffer && 0 == memcmp(TestObj.m_pBuffer, LongData, sizeof(LongData)));
			_ASSERTE(TestObj.m_BufferSize == sizeof(LongData));

			hr = TestObj.GetBuffer(RetBuffer, sizeof(RetBuffer));
			_ASSERTE(SUCCEEDED(hr));
			_ASSERTE(0 == memcmp(RetBuffer, LongData, sizeof(LongData)));

			INT aIn = 10;
			hr = TestObj.SetBuffer(&aIn, sizeof(aIn));
			_ASSERTE(SUCCEEDED(hr));
			_ASSERTE(TestObj.m_BufferSize == sizeof(aIn));
		}
	}
	//检查复制
	{

		KG3DRepresentObjectDummyTest TestObj;
		class AReceiverClass : public KG3DMessageReceiver
		{
		public:
			using KG3DMessageReceiver::GetSender;
		};
		AReceiverClass aReceiver;

		bool bRet = TestObj.RegisterReceiver(&aReceiver); 
		_ASSERTE(bRet && aReceiver.GetSender(0) == &TestObj && TestObj.GetReceiver(0) == &aReceiver);
		char TestBuffer[] = "123456absdjfoj";
		TestObj.SetBuffer(TestBuffer, sizeof(TestBuffer));

		KG3DRepresentObjectDummyTest AnotherObj;
		HRESULT hr = AnotherObj.Copy(&TestObj);
		_ASSERTE(SUCCEEDED(hr));

		_ASSERTE(aReceiver.GetSender(0) == &TestObj && TestObj.GetReceiver(0) == &aReceiver);
		_ASSERTE(aReceiver.GetSender(1) == &AnotherObj && AnotherObj.GetReceiver(0) == &aReceiver);
		_ASSERTE(aReceiver.GetSender(3) == NULL && TestObj.GetReceiver(1) == NULL 
			&& AnotherObj.GetReceiver(1) == NULL);

		char RetBuffer[100];
		AnotherObj.GetBuffer(RetBuffer, sizeof(RetBuffer));
		_ASSERTE(0 == memcmp(RetBuffer, TestBuffer, sizeof(TestBuffer)));
	}
}
KG_TEST_END(KG3DRepresentObjectDummy)
#endif