#include "stdafx.h"
#include "DShowTextures.h"
#include "MovieTexture.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

#pragma warning(disable:4244)

KMovieTexture::KMovieTexture()
{
	m_pGB            = NULL;
	m_pMC            = NULL;
	m_pME            = NULL;
	m_pRenderer      = NULL;
	m_pMS            = NULL;

	m_lpTexture      = NULL;
	CoInitialize(NULL);
}

KMovieTexture::~KMovieTexture()
{
	CleanUp();
	KTexture::CleanUp();
	CoUninitialize();
}

HRESULT KMovieTexture::CleanUp()
{
	if (!(!m_pMC)) m_pMC->Stop();

	if (!(!m_pMC)) m_pMC.Release();
	if (!(!m_pME)) m_pME.Release();
	if (!(!m_pMS)) m_pMS.Release();
	if (!(!m_pGB)) m_pGB.Release();
	if (!(!m_pRenderer)) m_pRenderer.Release();
	return S_OK;

}

void KMovieTexture::Run()
{
	if ( m_pMC )
		m_pMC->Run();
}

void KMovieTexture::Seek(float fSecond, int nType /* = 0*/)
{
	if ( !m_pMS )
		return;

	LONGLONG To;
	LONGLONG Stop;
	m_pMS->GetPositions(&To, &Stop);
	To = fSecond * 10000000.0f;
	switch(nType)
	{
	case 0://Relative to current;
		{
			m_pMS->SetPositions(&To, AM_SEEKING_RelativePositioning, &Stop, AM_SEEKING_AbsolutePositioning);
		}
		break;
	case 1://Absolute
		{
			m_pMS->SetPositions(&To, AM_SEEKING_AbsolutePositioning, &Stop, AM_SEEKING_AbsolutePositioning);
		}
		break;
	}

}

HRESULT KMovieTexture::Initialize(TCHAR* strMovieName, BOOL bSound)
{
	HRESULT           hr          = S_OK;
	CComPtr<IBaseFilter>    pFSrc;          // Source Filter
	CComPtr<IPin>           pFSrcPinOut;    // Source Filter Output Pin
	CTextureRenderer* pCTR        = NULL;

	CleanUp();

	// Create the filter graph
	//if (FAILED(CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC, IID_IGraphBuilder, (void**)&m_pGB)))
	if (FAILED(m_pGB.CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC)))
		return E_FAIL;

	// Create the Texture Renderer object
	pCTR = new CTextureRenderer(NULL, &hr, &m_lpTexture);
	if (FAILED(hr) || !pCTR)
	{
		//Msg(TEXT("Could not create texture renderer object!  hr=0x%x"), hr);
		return E_FAIL;
	}

	// Get a pointer to the IBaseFilter on the TextureRenderer, add it to graph
	m_pRenderer = pCTR;
	if (FAILED(hr = m_pGB->AddFilter(m_pRenderer, L"TEXTURERENDERER")))
	{
		//Msg(TEXT("Could not add renderer filter to graph!  hr=0x%x"), hr);
		return hr;
	}

	// Determine the file to load based on DirectX Media path (from SDK)
	// Use a helper function included in DXUtils.cpp
	//TCHAR strFileName[MAX_PATH];
	WCHAR wFileName[MAX_PATH];

	//lstrcpyn( strFileName, DXUtil_GetDXSDKMediaPath(), MAX_PATH-1 );
	//lstrcat( strFileName, SOURCE_FILE );
	//strFileName[MAX_PATH-1] = 0;  // NULL-terminate
	wFileName[MAX_PATH-1] = 0;    // NULL-terminate

	USES_CONVERSION;
	wcsncpy(wFileName, T2W(strMovieName), NUMELMS(wFileName));

	// Add the source filter to the graph.
	hr = m_pGB->AddSourceFilter (wFileName, L"SOURCE", &pFSrc);

	// If the media file was not found, inform the user.
	if (hr == VFW_E_NOT_FOUND)
	{
		return hr;
	}
	else if(FAILED(hr))
		return hr;

	if (FAILED(hr = pFSrc->FindPin(L"Output", &pFSrcPinOut)))
		return hr;

	if (!bSound)
	{
		CComPtr<IPin> pFTRPinIn;      // Texture Renderer Input Pin

		// Find the source's output pin and the renderer's input pin
		if (FAILED(hr = pCTR->FindPin(L"In", &pFTRPinIn)))
		{
			//Msg(TEXT("Could not find input pin!  hr=0x%x"), hr);
			return hr;
		}

		// Connect these two filters
		if (FAILED(hr = m_pGB->Connect(pFSrcPinOut, pFTRPinIn)))
		{
			//Msg(TEXT("Could not connect pins!  hr=0x%x"), hr);
			return hr;
		}
	}
	else
	{

		if (FAILED(hr = m_pGB->Render(pFSrcPinOut)))
		{
			//Msg(TEXT("Could not render source output pin!  hr=0x%x"), hr);
			return hr;
		}
	}

	// Get the graph's media control, event & position interfaces
	m_pGB->QueryInterface(&m_pMC);
	//m_pGB->QueryInterface(&m_pMP);
	m_pGB->QueryInterface(&m_pMS);
	m_pGB->QueryInterface(&m_pME);

	//m_pMC->Run();

	return hr;
}

HRESULT KMovieTexture::AdjustTextureCoord(IDirect3DVertexBuffer9* pDest, int nOffset, int nVerCnt, int nStride)
{
	HRESULT hr = S_OK;
	BYTE* pSrc = 0;

	int i;
	CTextureRenderer* pRender = static_cast<CTextureRenderer*>(m_pRenderer.p);
	hr = pDest->Lock(0, 0, (void**)&pSrc, 0);
	if ( FAILED(hr) )
		return hr;
	for ( i = 0; i < nVerCnt; i++ )
	{
		float* pTexCoord = (float*)(pSrc + i * nStride + nOffset);
		*pTexCoord = ((1.0f - *pTexCoord) * pRender->m_fResize[0]);
		*pTexCoord++;
		*pTexCoord = ((1.0f - *pTexCoord) * pRender->m_fResize[1]);

	}
	pDest->Unlock();
	return hr;
}

void KMovieTexture::SetRate(float fRate)
{
	if ( !m_pMS )
		return;

	if ( fRate <= 0.0f )
	{
		fRate = 1.0f;
	}

	m_pMS->SetRate(fRate);
}

float KMovieTexture::GetRate()
{
	if ( !m_pMS )
		return 1.0f;
	double fRate;
	m_pMS->GetRate(&fRate);
	return fRate;
}

HRESULT KMovieTexture::RestoreDeviceObjects()
{
	HRESULT hr = 0;
	if ( !m_pRenderer )
		return S_OK;
	CTextureRenderer* pRender = (CTextureRenderer*)m_pRenderer.p;
	hr = pRender->RecreateTexture(m_descSave);
	return hr;
}

void KMovieTexture::OnDeleteDevice()
{
	if ( !m_pRenderer)
		return;
	SAFE_RELEASE(m_lpTexture);
	CleanUp();
}

HRESULT KMovieTexture::InvalidateDeviceObjects()
{
	if ( !m_pRenderer )
		return S_OK;
	CTextureRenderer* pRender = (CTextureRenderer*)m_pRenderer.p;
	if ( pRender->m_bUseDynamicTextures )
	{
		m_lpTexture->GetLevelDesc(0, &m_descSave);
		SAFE_RELEASE(m_lpTexture);
		pRender->m_ppTexture = NULL;
	}
	return S_OK;
}

void KMovieTexture::CheckMovieStatus(void)
{
	long lEventCode;
	long lParam1;
	long lParam2;
	HRESULT hr;

	if (!m_pME)
		return;

	// Check for completion events
	hr = m_pME->GetEvent(&lEventCode, (LONG_PTR *) &lParam1, (LONG_PTR *) &lParam2, 0);
	if (SUCCEEDED(hr))
	{
		// If we have reached the end of the media file, reset to beginning
		if (EC_COMPLETE == lEventCode)
		{
			Seek(0, 1);
		}

		// Free any memory associated with this event
		hr = m_pME->FreeEventParams(lEventCode, lParam1, lParam2);
	}
}