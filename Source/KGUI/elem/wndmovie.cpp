#include "stdafx.h"
#include "./wndmovie.h"
#include "../common/tblock.h"
#include "../KGUI.h"

namespace
{
	typedef unsigned char (*KLVideoSetSoundSystemFn)(void* dd);
	typedef void (*KLVideoSetSoundVolumeFn)(HKLVIDEO bnk, unsigned int nVolume);
	typedef void (*KLVideoSetSoundTrackFn)(unsigned int track);
	typedef HKLVIDEO (*KLVideoOpenFn)(char* name, unsigned int flags, void* hWnd);
	typedef void (*KLVideoCloseFn)(HKLVIDEO bnk);
	typedef unsigned int (*KLVideoWaitFn)(HKLVIDEO bnk);
	typedef void (*KLVideoCopyToBufferFn)(HKLVIDEO bnk, void* buf, unsigned int left, unsigned int top, unsigned int Pitch, unsigned int destheight, unsigned int Flags);
	typedef unsigned int (*KLVideoDoFrameFn)(HKLVIDEO bnk);
	typedef void (*KLVideoNextFrameFn)(HKLVIDEO bnk);
	typedef unsigned int (*KLVideoGetCurrentFrameFn)(HKLVIDEO bnk);

	KLVideoSetSoundSystemFn VideoSetSoundSystem = NULL;
	KLVideoSetSoundVolumeFn VideoSetSoundVolume = NULL;
	KLVideoSetSoundTrackFn VideoSetSoundTrack = NULL;
	KLVideoOpenFn VideoOpen = NULL;
	KLVideoCloseFn VideoClose = NULL;
	KLVideoWaitFn VideoWait = NULL;
	KLVideoCopyToBufferFn VideoCopyToBuffer = NULL;
	KLVideoDoFrameFn VideoDoFrame = NULL;
	KLVideoNextFrameFn VideoNextFrame = NULL;
	KLVideoGetCurrentFrameFn VideoGetCurrentFrame = NULL;

	HMODULE g_hVideoDll = NULL;
}

namespace UI
{

int KWndMovie::Init()
{
	int nRetCode = false;
    int nResult = false;

    g_hVideoDll = ::LoadLibraryA(g_pUI->m_szKLVideoDllPath);
	KGLOG_PROCESS_ERROR(g_hVideoDll);

	VideoSetSoundSystem = (KLVideoSetSoundSystemFn)::GetProcAddress(g_hVideoDll, "KLVideoSetSoundSystem");
	KGLOG_PROCESS_ERROR(VideoSetSoundSystem);

	VideoSetSoundTrack = (KLVideoSetSoundTrackFn)::GetProcAddress(g_hVideoDll, "KLVideoSetSoundTrack");
	KGLOG_PROCESS_ERROR(VideoSetSoundTrack);

	VideoSetSoundVolume = (KLVideoSetSoundVolumeFn)::GetProcAddress(g_hVideoDll, "KLVideoSetSoundVolume");
	KGLOG_PROCESS_ERROR(VideoSetSoundVolume);

	VideoOpen = (KLVideoOpenFn)::GetProcAddress(g_hVideoDll, "KLVideoOpen");
	KGLOG_PROCESS_ERROR(VideoOpen);

	VideoClose = (KLVideoCloseFn)::GetProcAddress(g_hVideoDll, "KLVideoClose");
	KGLOG_PROCESS_ERROR(VideoClose);

	VideoWait = (KLVideoWaitFn)::GetProcAddress(g_hVideoDll, "KLVideoWait");
	KGLOG_PROCESS_ERROR(VideoWait);

	VideoCopyToBuffer = (KLVideoCopyToBufferFn)::GetProcAddress(g_hVideoDll, "KLVideoCopyToBuffer");
	KGLOG_PROCESS_ERROR(VideoCopyToBuffer);

	VideoDoFrame = (KLVideoDoFrameFn)::GetProcAddress(g_hVideoDll, "KLVideoDoFrame");
	KGLOG_PROCESS_ERROR(VideoDoFrame);

	VideoNextFrame = (KLVideoNextFrameFn)::GetProcAddress(g_hVideoDll, "KLVideoNextFrame");
	KGLOG_PROCESS_ERROR(VideoNextFrame);

	VideoGetCurrentFrame = (KLVideoGetCurrentFrameFn)::GetProcAddress(g_hVideoDll, "KLVideoGetCurrentFrame");
	KGLOG_PROCESS_ERROR(VideoGetCurrentFrame);

	nResult = true;
Exit0:
    if (!nResult)
    {
        KGLogPrintf(KGLOG_ERR, "KGUI LoadLibrary(%s) failed.\n", g_pUI->m_szKLVideoDllPath);
        Exit();
    }
	return nResult;
}


void KWndMovie::Exit()
{
	VideoSetSoundSystem = NULL;
	VideoSetSoundTrack = NULL;
	VideoSetSoundVolume = NULL;
	VideoOpen = NULL;
	VideoClose = NULL;
	VideoWait = NULL;
	VideoCopyToBuffer = NULL;
	VideoDoFrame = NULL;
	VideoNextFrame = NULL;
	VideoGetCurrentFrame = NULL;

	if (g_hVideoDll)
	{
		::FreeLibrary(g_hVideoDll);
		g_hVideoDll = NULL;
	}
}

#ifdef KGUI_LUA_LOADER
int KWndMovie::Create(IKUIObject **ppObj, Lua_State *pLua, int nTableIndex)
{
    int nResult  = false;
    int nRetCode = false;

    KWndWindow *pWnd = NULL;

    KGLOG_PROCESS_ERROR(ppObj);

    pWnd = new(std::nothrow) KWndMovie;
    KGLOG_PROCESS_ERROR(pWnd);

    if (pLua)
    {
        nRetCode = pWnd->Init(pLua, nTableIndex);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    *ppObj = pWnd;
    // Exit1:
    nResult = true;
Exit0:
    if (!nResult)
    {
        KG_DELETE(pWnd);
    }
    return nResult;
}
#endif

KWndMovie::KWndMovie()
: m_hVideo(NULL)
{
	memset(&m_Image, 0, sizeof(m_Image));
}

KWndMovie::~KWndMovie()
{
	Stop();
}

void* KWndMovie::operator new(size_t nSize, std::nothrow_t const&) throw()
{ 
	ASSERT(nSize == sizeof(KWndMovie));
	return KBlockPool::GetSelf().Alloc(sizeof(KWndMovie)); 
}

void KWndMovie::operator delete(void* pData, std::nothrow_t const&) throw()
{ 
	KBlockPool::GetSelf().Free(pData, sizeof(KWndMovie));
}

void* KWndMovie::operator new(size_t nSize)
{ 
	ASSERT(nSize == sizeof(KWndMovie));
	return KBlockPool::GetSelf().Alloc(sizeof(KWndMovie)); 
}

void KWndMovie::operator delete(void* pData) throw()
{ 
	KBlockPool::GetSelf().Free(pData, sizeof(KWndMovie));
}

#ifdef KGUI_LUA_LOADER
int KWndMovie::Init(Lua_State *pLua, int nTableIndex)
{
	int nResult  = false;
	int nRetCode = false;

    KGLOG_PROCESS_ERROR(pLua);

    nRetCode = KWndWindow::Init(pLua, nTableIndex);
    KGLOG_PROCESS_ERROR(nRetCode);

    UpdateMovieViewport();

// Exit1:
	nResult = true;
Exit0:
	return nResult;
}
#endif

void KWndMovie::OnBreathe()
{
	DrawMovie();

	KWndWindow::OnBreathe();
}

void KWndMovie::OnPaint()
{
	g_pUI->m_p3DUI->DrawMultiImage(&m_Image, 1);

	KWndWindow::OnPaint();
}

void KWndMovie::OnSize()
{
	KWndWindow::OnSize();

	UpdateMovieViewport();
}

void KWndMovie::OnScale(float fScaleX, float fScaleY)
{
	KWndWindow::OnScale(fScaleX, fScaleY);

	UpdateMovieViewport();
}


void KWndMovie::OnMove()
{
	KWndWindow::OnMove();

	UpdateMovieViewport();
}

int KWndMovie::CanCastTo(LPCSTR szType) const
{
	return szType == "WndMovie" || szType == "WndWindow";
}


LPCSTR KWndMovie::GetWndType() const
{
	return "WndMovie";
}


bool KWndMovie::Play(LPCSTR szVideoFile)
{
	BOOL bRetCode = FALSE;
	HRESULT hr = E_FAIL;

	KG_PROCESS_ERROR(VideoOpen);
	KGLOG_PROCESS_ERROR(szVideoFile && szVideoFile[0] != '\0');

	Stop();

	m_hVideo = VideoOpen((char*)szVideoFile, 0, g_pUI->m_hWnd);
	KGLOG_PROCESS_ERROR(m_hVideo);

	bRetCode = m_hVideo->Width > 0 && m_hVideo->Height > 0;
	KGLOG_PROCESS_ERROR(bRetCode);

	hr = g_pUI->m_p3DUI->CreateImage(m_hVideo->Width, m_hVideo->Height, &m_Image.dwImageID, 0);
	KGLOG_COM_PROCESS_ERROR(hr);

	m_Image.v2UVMin = D3DXVECTOR2(0.0f, 0.0f);
	m_Image.v2UVMax = D3DXVECTOR2(1.0f, 1.0f);
	m_Image.crDiffuse = m_dwAlpha << 24;

	SetVolume(100);

	return true;
Exit0:
	Stop();
	return false;
}


void KWndMovie::Stop()
{
	if (m_hVideo)
	{
		if (VideoClose)
			VideoClose(m_hVideo);
		m_hVideo = NULL;
	}

	if (m_Image.dwImageID != 0)
	{
		g_pUI->m_p3DUI->UnloadImage(m_Image.dwImageID);
		m_Image.dwImageID = 0;
	}
}


void KWndMovie::SetVolume(unsigned int uVolume)
{
	if (m_hVideo == NULL)
		return;
	if (VideoSetSoundVolume == NULL)
		return;
	VideoSetSoundVolume(m_hVideo, uVolume);
}


void KWndMovie::UpdateMovieViewport()
{
	m_Image.v2Min = D3DXVECTOR2(m_fAbsX, m_fAbsY);
    m_Image.v2Max = D3DXVECTOR2(m_fAbsX + m_fWidth, m_fAbsY + m_fHeight);
}


void KWndMovie::DrawMovieCallback(BYTE* pBits, INT nPitch, UINT uWidth, UINT uHeight, D3DFORMAT Format, void* pUserdata)
{
	unsigned int uVideoFormat = 0;
	HKLVIDEO hVideo = NULL;

	ASSERT(pBits);
	ASSERT(pUserdata);

	hVideo = reinterpret_cast<HKLVIDEO>(pUserdata);

	switch (Format)
	{
	case D3DFMT_A8R8G8B8:
		uVideoFormat = KLVIDEOSURFACE32;
		break;
	default:
		ASSERT(0);
		break;
	}

	VideoCopyToBuffer(hVideo, pBits, 0, 0, (unsigned int)nPitch, uHeight, uVideoFormat);

	UINT uColumnCount = uWidth * 4;

	for (UINT uRow = 0; uRow < uHeight; ++uRow)
	{
		for (UINT uColumn = 0; uColumn < uColumnCount; uColumn += 4)
			pBits[uColumn + 3] = 0xFF;
		pBits += nPitch;
	}
}


void KWndMovie::DrawMovie()
{
	HRESULT hr = E_FAIL;

	if (VideoCopyToBuffer == NULL)
		return;

	if (m_hVideo == NULL)
		return;

	hr = g_pUI->m_p3DUI->DrawUserdataToImage(m_Image.dwImageID, DrawMovieCallback, m_hVideo);
	KGLOG_COM_PROCESS_ERROR(hr);

	if (m_hVideo->FrameNum >= m_hVideo->Frames)
		return;

	return;
Exit0:
	return;
}


bool KWndMovie::IsFinished() const
{
	return m_hVideo && m_hVideo->FrameNum == m_hVideo->Frames;
}

} // namespace UI
