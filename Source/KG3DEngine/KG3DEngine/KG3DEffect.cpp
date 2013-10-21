#include "stdafx.h"
#include "KG3DEffect.h"
#include "KG3DGraphicsTool.h"


#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

using namespace std;

KG3DEffect::KG3DEffect(void)
{
	m_pEffect = NULL;
	m_ulRefCount = 0;
}

KG3DEffect::~KG3DEffect(void)
{
	UnInit();
}

HRESULT KG3DEffect::OnLostDevice()
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

	if (m_pEffect)
    {
		hr = m_pEffect->OnLostDevice();
        KGLOG_COM_PROCESS_ERROR(hr);
    }

    hrResult = S_OK;
Exit0:
	return hrResult;
}

HRESULT KG3DEffect::OnResetDevice()
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

	if (m_pEffect)
    {
		hr = m_pEffect->OnResetDevice();
        KGLOG_COM_PROCESS_ERROR(hr);
    }

    hrResult = S_OK;
Exit0:
	return hrResult;
}

HRESULT KG3DEffect::LoadFromFile( const char cszFilename[], unsigned uOption, unsigned uOperator )
{
	HRESULT hr = S_OK;
	
	SAFE_RELEASE(m_pEffect);

	DWORD dwShaderFlags = 0;
#ifdef DEBUG_VS
	dwShaderFlags |= D3DXSHADER_FORCE_VS_SOFTWARE_NOOPT;
#endif
#ifdef DEBUG_PS
	dwShaderFlags |= D3DXSHADER_FORCE_PS_SOFTWARE_NOOPT;
#endif

	dwShaderFlags |= D3DXSHADER_DEBUG ;//| D3DXSHADER_SKIPOPTIMIZATION;
	hr = KG3DD3DXCreateEffectFromFile(g_pd3dDevice, cszFilename, NULL, NULL, 
		dwShaderFlags, NULL, &m_pEffect, NULL );

	if( SUCCEEDED( hr ) )
		m_szFileName = cszFilename;
	return hr;
}

ULONG STDMETHODCALLTYPE KG3DEffect::AddRef()
{
    return (ULONG)KG_InterlockedIncrement((long *)&m_ulRefCount);
}
ULONG STDMETHODCALLTYPE KG3DEffect::Release()
{
    ULONG uNewRefCount = (ULONG)KG_InterlockedDecrement((long *)&m_ulRefCount);
    if (uNewRefCount == 0)
    {
        UnInit();
        delete this; 
    }
    return uNewRefCount;
}

HRESULT STDMETHODCALLTYPE KG3DEffect::QueryInterface( 
	/* [in] */ REFIID riid,
	/* [iid_is][out] */ void **ppvObject)
{
	return E_FAIL;
}

HRESULT KG3DEffect::Init()
{
	SAFE_RELEASE( m_pEffect );
	m_szFileName = "";
	return S_OK;
}

HRESULT KG3DEffect::UnInit()
{
	SAFE_RELEASE( m_pEffect );
	m_szFileName = "";
	return S_OK;
}

HRESULT KG3DEffect::PrepareRender(unsigned uOption) 
{
	HRESULT hr = S_OK;
	return hr;
}

HRESULT KG3DEffect::PostRender()
{
	HRESULT hr = S_OK;
	return hr;
}
HRESULT KG3DEffect::GetDateInformation(int *pnMemory, int *pnVideoMemory, int *pnCompressMem, int *pnCompressVideoMem)
{
	HRESULT hr = S_OK;
	return hr;
}

unsigned KG3DEffect::GetLastUseTime()
{
	return 0;
}

unsigned KG3DEffect::GetUseFrequency()
{
	return 0;
}

unsigned KG3DEffect::GetType()
{
	return m_uEffectType;
}

HRESULT KG3DEffect::BegineEffect(unsigned int& uNumPasses)
{
	HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

	if (m_pEffect)
	{
		hr = m_pEffect->Begin(&uNumPasses, 0);
        KGLOG_COM_PROCESS_ERROR(hr);
	}

    hrResult = S_OK;
Exit0:
	return hrResult;
}

HRESULT KG3DEffect::EndEffect()
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

	if (m_pEffect)
	{
		hr = m_pEffect->End();
        KGLOG_COM_PROCESS_ERROR(hr);
	}

    hrResult = S_OK;
Exit0:
	return hrResult;
}

HRESULT KG3DEffect::SaveToFile(const char cszFileName[], unsigned uOption)
{
	HRESULT hr = E_FAIL;
	return hr;
}

HRESULT KG3DEffect::LoadFromFileEx(const char cszFileName[], unsigned uFileNameHash, unsigned uOption, DWORD_PTR WParam, DWORD_PTR LParam)
{
    return LoadFromFile(cszFileName, uFileNameHash, uOption);
}
