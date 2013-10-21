/*****************************************************************************************
//	播放video窗口
//	Copyright : Kingsoft 2003
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2003-7-14
*****************************************************************************************/
#include "KWin32.h"
#include "KWin32Wnd.h"
#include "KIniFile.h"
#include "../Elem/Wnds.h"
#include "../Elem/WndMessage.h"
#include "UiPlayVideo.h"
#include "../../../Represent/iRepresent/iRepresentShell.h"
#include "../../../Represent/iRepresent/KRepresentUnit.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

extern iRepresentShell*	g_pRepresentShell;

KUiPlayVideo* KUiPlayVideo::m_pSelf = NULL;

KUiPlayVideo::KUiPlayVideo()
{
	m_bProcessInput = true;
	m_bCloseWhenOver = true;
	m_bInitialized = false;
	m_pCaller = NULL;

	m_bnk = NULL;
	m_hVideoDll = NULL;
	m_KLVideoSoundFn = NULL;
    m_KLVideoSndtrackFn = NULL;
    m_KLVideoOpen = NULL;
    m_KLVideoClose = NULL;
    m_KLVideoWait = NULL;
    m_KLVideoToBuffer = NULL;
    m_KLVideoDoFrame = NULL;
    m_KLVideoNextFrame = NULL;
	m_KLVideoGetCurrentFrame = NULL;
	m_KLVideoSetSoundVolume = NULL;

	m_szBitmapName[0] = 0;
	m_uBitmapId = 0;
	m_uBitmapIsPosition = IMAGE_IS_POSITION_INIT;

	m_nBlackHeight = m_nBlackWidth = 0;
	m_nbAlreadyPaintBlack = false;
}

//--------------------------------------------------------------------------
//	功能：打开窗口，返回唯一的一个类对象实例
//--------------------------------------------------------------------------
KUiPlayVideo* KUiPlayVideo::OpenWindow()
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiPlayVideo;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
		m_pSelf->Show();
	return m_pSelf;
}

//--------------------------------------------------------------------------
//	功能：输入处理消息以及一些特定窗口消息的响应
//--------------------------------------------------------------------------
int KUiPlayVideo::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int nRet = 0;

	switch(uMsg)
	{
	case WM_KEYDOWN:
		if (m_bProcessInput && m_bnk &&
			(uParam == VK_ESCAPE || uParam == VK_RETURN || uParam == VK_SPACE))
		{
			CloseVideo();
		}
		break;
	case WM_LBUTTONUP:
		if (m_bProcessInput && m_bnk)
			CloseVideo();
		break;
	default:
		nRet = KWndWindow::WndProc(uMsg, uParam, nParam);
		break;
	}
	return nRet;
}

//关闭窗口，同时可以选则是否删除对象实例
void KUiPlayVideo::CloseWindow(bool bDestroy)
{
	if (m_pSelf)
	{
		m_pSelf->CloseVideo();
		if (bDestroy == false)
			m_pSelf->Hide();
		else
		{
			m_pSelf->Terminate();
			m_pSelf->Destroy();
			m_pSelf = NULL;
		}
	}
}

//载入界面方案
void KUiPlayVideo::LoadScheme(const char* pszSettingFile, const char* pszSection)
{
	if (m_pSelf && pszSettingFile && pszSection && pszSettingFile[0] && pszSection[0])
	{
		KIniFile	Ini;
		if (Ini.Load(pszSettingFile))
			m_pSelf->Init(&Ini, pszSection);
	}
}

int KUiPlayVideo::Initialize()
{
	if (m_bInitialized)
		return true;

	m_hVideoDll = LoadLibrary("KLVideo.dll");

	if(!m_hVideoDll)
		return false;

	// connect bink dll functions
	m_KLVideoSoundFn         = (KLVideoSoundFn)GetProcAddress(m_hVideoDll,         "KLVideoSetSoundSystem");
	m_KLVideoSndtrackFn      = (KLVideoSndtrackFn)GetProcAddress(m_hVideoDll,      "KLVideoSetSoundTrack");
	m_KLVideoOpen            = (KLVideoOpenFn)GetProcAddress(m_hVideoDll,          "KLVideoOpen");
	m_KLVideoClose           = (KLVideoCloseFn)GetProcAddress(m_hVideoDll,         "KLVideoClose");
	m_KLVideoWait            = (KLVideoWaitFn)GetProcAddress(m_hVideoDll,          "KLVideoWait");
	m_KLVideoToBuffer        = (KLVideoToBufferFn)GetProcAddress(m_hVideoDll,      "KLVideoCopyToBuffer");
	m_KLVideoDoFrame         = (KLVideoDoFrameFn)GetProcAddress(m_hVideoDll,       "KLVideoDoFrame");
	m_KLVideoNextFrame       = (KLVideoNextFrameFn)GetProcAddress(m_hVideoDll,     "KLVideoNextFrame");
	m_KLVideoGetCurrentFrame = (KLVideoGetCurrentFrameFn)GetProcAddress(m_hVideoDll, "KLVideoGetCurrentFrame");
	m_KLVideoSetSoundVolume	 = (KLVideoSetSndVolumeFn)GetProcAddress(m_hVideoDll,	 "KLVideoSetSoundVolume");

	// make sure functions connected //
	if( !m_KLVideoSoundFn || !m_KLVideoOpen || !m_KLVideoClose || !m_KLVideoWait ||
		!m_KLVideoToBuffer || !m_KLVideoDoFrame || !m_KLVideoNextFrame || !m_KLVideoSndtrackFn)
	{
		FreeLibrary(m_hVideoDll);
		m_hVideoDll = NULL;
		return false;
	}

	Wnd_AddWindow(this, WL_TOPMOST);

	m_bInitialized = true;
	return true;
}

void KUiPlayVideo::Terminate()
{
	CloseVideo();

	if(m_hVideoDll)
	{
		FreeLibrary(m_hVideoDll);
		m_hVideoDll = NULL;
	}
	m_bInitialized = false;
}

int	KUiPlayVideo::OpenVideo(const char* pszVideoFile)
{
	if (m_bInitialized == false || m_Width == 0 || m_Height == 0)
		return false;
	m_nbAlreadyPaintBlack = false;
	CloseVideo(false);
	m_bnk = m_KLVideoOpen(pszVideoFile, 0, g_GetMainHWnd());
	if(!m_bnk)
		return false;

	if(m_Width < (int)m_bnk->Width || m_Height < (int)m_bnk->Height)
	{
		CloseVideo();
		return false;
	}

	m_KLVideoSetSoundVolume(m_bnk, 100);

	for (int i = 0; i < 10000; i++)
	{
		sprintf(m_szBitmapName, "VideoBitmap_%d", i);
		m_uBitmapId = g_pRepresentShell->CreateImage(m_szBitmapName, m_bnk->Width, m_bnk->Height, ISI_T_BITMAP16);
		if (m_uBitmapId)
			break;
	}

	if (m_uBitmapId)
	{
		m_uBitmapIsPosition = IMAGE_IS_POSITION_INIT;
		g_pRepresentShell->ClearImageData(m_szBitmapName, m_uBitmapId, m_uBitmapIsPosition);

		m_nBlackHeight = m_bnk->Height  * m_Width / m_bnk->Width;
		if (m_nBlackHeight <= m_Height)
		{
			m_nBlackHeight = (m_Height - m_nBlackHeight) / 2;
		}
		else
		{
			m_nBlackHeight = 0;
			m_nBlackWidth = m_bnk->Width * m_Height / m_bnk->Height;
			if (m_nBlackWidth < m_Width)
			{
				m_nBlackWidth = (m_Width - m_nBlackWidth) / 2;
			}
			else
			{
				m_nBlackWidth = 0;
			}
		}

		return true;
	}

	CloseVideo();
	return false;
}

void KUiPlayVideo::CloseVideo(bool bFinished)
{
	m_nbAlreadyPaintBlack = false;
	m_nBlackHeight = m_nBlackWidth = 0;
	if(m_bnk)
	{
		m_KLVideoClose( m_bnk );
		m_bnk = NULL;
		if (m_uBitmapId)
		{
			g_pRepresentShell->FreeImage(m_szBitmapName);
			m_uBitmapId = 0;
		}
		if (bFinished)
			OnPlayFinished();
	}
}

void KUiPlayVideo::Breathe()
{
	if(!m_bnk)
		return;

	KBitmapDataBuffInfo bufInfo;
	void *pBuf = g_pRepresentShell->GetBitmapDataBuffer(m_szBitmapName, &bufInfo);
	if(!pBuf)
		return;

	unsigned int uFormat;
	if (bufInfo.eFormat == DBDF_24BIT)
		uFormat = KLVIDEOSURFACE24;
	else if (bufInfo.eFormat == BDBF_16BIT_565)
		uFormat = KLVIDEOSURFACE565;
	else// if (bufInfo.eFormat == BDBF_16BIT_555)
		uFormat = KLVIDEOSURFACE555;
	m_KLVideoToBuffer(m_bnk, pBuf, 0, 0, bufInfo.nPitch, m_bnk->Height, uFormat);

	g_pRepresentShell->ReleaseBitmapDataBuffer(m_szBitmapName,pBuf);

	if(m_bnk->FrameNum >= m_bnk->Frames)
	{
		CloseVideo();
	}
}

//--------------------------------------------------------------------------
//	功能：绘制游戏世界
//--------------------------------------------------------------------------
void KUiPlayVideo::PaintWindow()
{
	if(m_bnk == NULL || g_pRepresentShell == NULL)
		return;

	if (m_nbAlreadyPaintBlack == false)
	{
		m_nbAlreadyPaintBlack = true;
		KRUShadow	Black;
		Black.oPosition.nX = m_nAbsoluteLeft;
		Black.oPosition.nY = m_nAbsoluteTop;
		Black.Color.Color_dw = 0;
		if (m_nBlackHeight)
		{
			Black.oEndPos.nX = Black.oPosition.nX + m_Width;
			Black.oEndPos.nY = Black.oPosition.nY + m_nBlackHeight;
			g_pRepresentShell->DrawPrimitives(1, &Black, RU_T_SHADOW, true);
			Black.oEndPos.nY = Black.oPosition.nY + m_Height;
			Black.oPosition.nY = Black.oEndPos.nY - m_nBlackHeight;
			g_pRepresentShell->DrawPrimitives(1, &Black, RU_T_SHADOW, true);
		}
		else if (m_nBlackWidth)
		{
			Black.oEndPos.nX = Black.oPosition.nX + m_nBlackWidth;
			Black.oEndPos.nY = Black.oPosition.nY + m_Height;
			g_pRepresentShell->DrawPrimitives(1, &Black, RU_T_SHADOW, true);
			Black.oEndPos.nX = Black.oPosition.nX + m_Width;
			Black.oPosition.nX = Black.oEndPos.nX - m_nBlackWidth;
			g_pRepresentShell->DrawPrimitives(1, &Black, RU_T_SHADOW, true);
		}
	}

	KRUImageStretch img;
	strcpy(img.szImage, m_szBitmapName);
	img.nType = ISI_T_BITMAP16;
	img.oPosition.nX = m_nAbsoluteLeft + m_nBlackWidth;
	img.oPosition.nY = m_nAbsoluteTop + m_nBlackHeight;
	img.oEndPos.nX = m_nAbsoluteLeft + m_Width - m_nBlackWidth;
	img.oEndPos.nY = m_nAbsoluteTop + m_Height - m_nBlackHeight;
	img.bRenderFlag = 0;
	img.bRenderStyle = IMAGE_RENDER_STYLE_OPACITY;
	img.nFrame = 0;
	img.nISPosition = m_uBitmapIsPosition;
	img.uImage = m_uBitmapId;
	g_pRepresentShell->DrawPrimitives(1, &img, RU_T_IMAGE_STRETCH, true);
	m_uBitmapIsPosition = img.nISPosition;
}

//播放结束
void KUiPlayVideo::OnPlayFinished()
{
	KWndWindow* pCaller = m_pCaller;
	unsigned int uParam = m_uCallerParam;
	m_pCaller = NULL;
	if (m_bCloseWhenOver)
		CloseWindow(false);
	if (pCaller)
		pCaller->WndProc(WND_M_OTHER_WORK_RESULT, uParam, 0);
}

//设置窗口位置，相对坐标
void KUiPlayVideo::SetPosition(int nLeft, int nTop)
{
	KWndWindow::SetPosition(nLeft, nTop);
}

//设置窗口大小
void KUiPlayVideo::SetSize(int nWidth, int nHeight)
{
	if (m_bnk == NULL)
		KWndWindow::SetSize(nWidth, nHeight);
}

void KUiPlayVideo::Setting(bool bProcessInput, bool bCloseWhenOver,
								KWndWindow* pCaller, unsigned int uParam)
{
	m_bProcessInput = bProcessInput;
	m_bCloseWhenOver = bCloseWhenOver;
	m_pCaller = pCaller;
	m_uCallerParam = uParam;
}