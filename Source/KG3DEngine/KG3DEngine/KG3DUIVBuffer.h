////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DUIVBuffer.h
//  Version     : 1.0
//  Creator     : dengchao
//  Create Date : 2006-08-28 16:43:32
//  Comment     : font def
//
////////////////////////////////////////////////////////////////////////////////


#ifndef _INCLUDE_KG3DUIVBUFFER_H_
#define _INCLUDE_KG3DUIVBUFFER_H_


#ifndef _D3D9_H_
#include <d3d9.h>
#endif



//////////////////////////////////////////////////////////////////////
//   class  :   KG3DUIVBuffer
//   brief  :   ui vertex buffer class
//   author :   dengchao
//   log    :     
//               
//////////////////////////////////////////////////////////////////////
template <typename TYPE_VERTEX>
class KG3DUIVBuffer
{
public:
	KG3DUIVBuffer(UINT uNums)
		: m_Span(sizeof(typename TYPE_VERTEX))
		, m_Nums(uNums)
		, m_Length(uNums * m_Span)
		, m_LockSize(0)
		, m_LockStart(0)
	    , m_lpD3DDevice(NULL)
	    , m_lpD3DVBuffer(NULL)
	{
	}

	~KG3DUIVBuffer()
	{
        ASSERT(m_lpD3DDevice == NULL);
	    ASSERT(m_lpD3DVBuffer == NULL);
	}

public:
	int Lock(UINT Nums, void** ppbData)
	{
        int nResult = false;
        HRESULT hr = E_FAIL;
        UINT LockOpts = 0;
        UINT LockStart = 0;

		*ppbData = NULL;

        KGLOG_PROCESS_ERROR(m_lpD3DDevice);

        hr = m_lpD3DDevice->TestCooperativeLevel();
        KG_COM_PROCESS_ERROR(hr);

        // ensure there is enough space in the vb for this data
        ASSERT(Nums <= GetNums());

        KGLOG_PROCESS_ERROR(m_lpD3DVBuffer);

        // If either user forced us to flush,
        // or there is not enough space for the vertex data,
        // then flush the buffer contents
        LockOpts = D3DLOCK_NOSYSLOCK;
        LockStart = m_LockStart;

        if (Nums + LockStart > GetNums())
        {
            LockStart = 0;
            LockOpts |= D3DLOCK_DISCARD;
        }
        else
        {
            LockOpts |= D3DLOCK_NOOVERWRITE;
        }

        hr = m_lpD3DVBuffer->Lock(LockStart * m_Span, Nums * m_Span, (void**)ppbData, LockOpts);
        KGLOG_COM_PROCESS_ERROR(hr);

        m_LockStart = LockStart;
        m_LockSize  = Nums;

        nResult = true;
Exit0:
		return nResult;
	}

	int Unlock()
	{
        int nResult  = false;
        HRESULT hr = E_FAIL;

        KGLOG_PROCESS_ERROR(m_lpD3DVBuffer);

        hr = m_lpD3DVBuffer->Unlock();
        KGLOG_COM_PROCESS_ERROR(hr);

        m_LockStart += m_LockSize;

        nResult = true;
Exit0:
		return nResult;
	}


	HRESULT DrawPrimitive( D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex,UINT PrimitiveCount )
	{
		HRESULT hr = E_FAIL;
        HRESULT hrResult = E_FAIL;

        KGLOG_PROCESS_ERROR(m_lpD3DDevice);

		hr = m_lpD3DDevice->SetStreamSource( 0, m_lpD3DVBuffer, 0, m_Span );
		KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

		hr = m_lpD3DDevice->SetFVF( typename TYPE_VERTEX::FVF );
		KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

		hr = m_lpD3DDevice->DrawPrimitive(PrimitiveType, StartVertex, PrimitiveCount);
		KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

		hrResult = S_OK;
	Exit0:
		return hrResult;
	}

	HRESULT DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, IDirect3DBaseTexture9* pTexture, UINT StartVertex,UINT PrimitiveCount)
	{
		HRESULT hr = E_FAIL;
        HRESULT hrResult = E_FAIL;

        KG_PROCESS_ERROR(pTexture);

        pTexture->PreLoad();

		hr = m_lpD3DDevice->SetTexture(0, pTexture);
		KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

		hr = m_lpD3DDevice->SetStreamSource( 0, m_lpD3DVBuffer, 0, m_Span );
		KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

		hr = m_lpD3DDevice->SetFVF( typename TYPE_VERTEX::FVF );
		KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

		hr = m_lpD3DDevice->DrawPrimitive(PrimitiveType, StartVertex, PrimitiveCount);
		KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

		hrResult = S_OK;
Exit0:
		return hrResult;
	}

	ULONG Release()
	{
        VERIFY_RELEASE(m_lpD3DVBuffer);

		if (NULL == m_lpD3DVBuffer)
		{
			m_LockStart = 0;
			m_LockSize = 0;
		}

        SAFE_RELEASE(m_lpD3DDevice);

		return 0;
	}

	HRESULT Restore(LPDIRECT3DDEVICE9 pD3D)
	{
        HRESULT hr = E_FAIL;
        HRESULT hrResult = E_FAIL;

		if (m_lpD3DDevice != pD3D)
		{
			Release();

			m_lpD3DDevice = pD3D;
            m_lpD3DDevice->AddRef();
		}

        memset(&m_D3DDesc, 0, sizeof(m_D3DDesc));

        m_D3DDesc.Format = D3DFMT_VERTEXDATA;
        m_D3DDesc.Size   = m_Length;
        m_D3DDesc.Type   = D3DRTYPE_VERTEXBUFFER;
        m_D3DDesc.Pool   = D3DPOOL_DEFAULT;
        m_D3DDesc.FVF    = typename TYPE_VERTEX::FVF;
        m_D3DDesc.Usage  = D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY;

        ASSERT(m_lpD3DVBuffer == NULL);

        hr = m_lpD3DDevice->CreateVertexBuffer( 
            m_D3DDesc.Size,
            m_D3DDesc.Usage,
            m_D3DDesc.FVF,
            m_D3DDesc.Pool,
            &m_lpD3DVBuffer,
            NULL);
        KGLOG_COM_PROCESS_ERROR(hr);

        if (hr == D3D_OK && m_lpD3DVBuffer)
        {
            D3DVERTEXBUFFER_DESC aDesc;

            m_lpD3DVBuffer->GetDesc(&aDesc);

            ASSERT(memcmp(&aDesc, &m_D3DDesc, sizeof(aDesc)) == 0);
        }

        hrResult = S_OK;
Exit0:
		return hrResult;
	}

    virtual HRESULT OnLostDevice() 
    {
        Release();

        return S_OK;
    }

    virtual HRESULT OnResetDevice() 
    {
        return Restore(g_pd3dDevice);
    }


	//////////////////////////////////////////////////
	// get operator
	UINT GetNums() const		{	return m_Nums;		}
	UINT GetLength() const		{	return m_Length;	}

	UINT GetLockStart() const	{	return m_LockStart;	}
	UINT GetLockSize() const	{	return m_LockSize;	}

private:
	KG3DUIVBuffer(KG3DUIVBuffer const& rhs);
	KG3DUIVBuffer& operator=(KG3DUIVBuffer const&);

private:
	//////////////////////////////////////////////////
	// member
	UINT		m_Span;
	UINT		m_Nums;
	UINT		m_Length;

	UINT		m_LockSize;
	UINT		m_LockStart;

	BOOL		m_bFlush;


	//////////////////////////////////////////////////
	// d3d member
	D3DVERTEXBUFFER_DESC		m_D3DDesc;
	LPDIRECT3DDEVICE9			m_lpD3DDevice;
	LPDIRECT3DVERTEXBUFFER9		m_lpD3DVBuffer;
};




#endif // _INCLUDE_KG3DUIVBUFFER_H_
