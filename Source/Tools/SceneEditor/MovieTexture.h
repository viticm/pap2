#pragma once

#include <atlbase.h>
#include "dshowTextures.h"
#include "texture.h"

class KMovieTexture:public KTexture
{
public:
	KMovieTexture();
	~KMovieTexture();
	HRESULT CleanUp();
	HRESULT Initialize(TCHAR* strMovieName, BOOL bSound);
	void Run();
	void Seek(float fSecond, int nType = 0);
	void SetRate(float fRate);
	float GetRate();
	HRESULT GetStatus(long* evCode){};
	IDirect3DTexture9* GetTexture() { if ( m_pRenderer ) return ((CTextureRenderer*)m_pRenderer.p)->GetTexture(); else return 0;}
	HRESULT AdjustTextureCoord(IDirect3DVertexBuffer9* pDest, int nOffset, int nVerCnt, int nStride);
	HRESULT InvalidateDeviceObjects();
	HRESULT RestoreDeviceObjects();
	void OnDeleteDevice();
	void CheckMovieStatus(void);
private:
	CComPtr<IGraphBuilder>     m_pGB;
    CComPtr<IMediaControl>     m_pMC;          // Media Control
	CComPtr<IMediaEvent>       m_pME;          // Media Event
	CComPtr<IBaseFilter>       m_pRenderer;    // our custom renderer
	CComPtr<IMediaSeeking>     m_pMS;          //Media Seeking

	D3DSURFACE_DESC      m_descSave;     //Save for restore
};