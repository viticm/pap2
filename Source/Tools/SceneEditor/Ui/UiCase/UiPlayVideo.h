/*****************************************************************************************
//	播放video窗口
//	Copyright : Kingsoft 2003
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2003-7-14
------------------------------------------------------------------------------------------
*****************************************************************************************/
#pragma once

#include "../Elem/WndWindow.h"
#include "..\..\..\KLVideo\KLVideo.h"

typedef u8 (*KLVideoSoundFn)(void PTR4* dd);
typedef HKLVIDEO (*KLVideoOpenFn)(const char PTR4* name,u32 flags, void PTR4* hWnd);
typedef void (*KLVideoCloseFn)(HKLVIDEO bnk);
typedef u32 (*KLVideoWaitFn)(HKLVIDEO bnk);
typedef void (*KLVideoToBufferFn)(HKLVIDEO bnk,void PTR4* buf,u32 left,u32 top,u32 Pitch,u32 destheight,u32 Flags);
typedef u32 (*KLVideoDoFrameFn)(HKLVIDEO bnk);
typedef void (*KLVideoNextFrameFn)(HKLVIDEO bnk);
typedef void (*KLVideoSetSndVolumeFn)(HKLVIDEO bnk,u32 nVolume);
typedef void (*KLVideoSndtrackFn)(u32 track);
typedef u32 (*KLVideoGetCurrentFrameFn)(HKLVIDEO bnk);

class KUiPlayVideo : protected KWndWindow
{
public:
	//----界面面板统一的接口函数----
	static KUiPlayVideo*	OpenWindow();							//打开窗口，返回唯一的一个类对象实例
	static void				CloseWindow(bool bDestroy);				//关闭窗口，同时可以选则是否删除对象实例
	static void				LoadScheme(const char* pszSettingFile, const char* pszSection);	//载入界面方案
	
	void					Setting(bool bProcessInput, bool bCloseWhenOver,
								KWndWindow* pCaller, unsigned int uParam = 0);
	int						OpenVideo(const char* pszVideoFile);
	void					CloseVideo(bool bFinished = true);
	void					SetPosition(int nLeft, int nTop);		//设置窗口位置，相对坐标
	void					SetSize(int nWidth, int nHeight);		//设置窗口大小

	KUiPlayVideo();

private:
	//活动函数
	void	Breathe();
	//输入处理消息以及一些特定窗口消息的响应
	int	WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	//绘制
	void	PaintWindow();
	//初始化
	int Initialize();
	//关闭
	void Terminate();
	//播放结束
	void OnPlayFinished();

	~KUiPlayVideo() {}
private:
	static KUiPlayVideo* m_pSelf;
	KWndWindow*		m_pCaller;
	unsigned int	m_uCallerParam;
	bool			m_bProcessInput;
	bool			m_bCloseWhenOver;

	bool			m_bInitialized;

	HKLVIDEO				  m_bnk;						//正在播放的Video句柄
	HMODULE					  m_hVideoDll;					//KLVideo.dll的模块句柄
	KLVideoSoundFn            m_KLVideoSoundFn;
    KLVideoSndtrackFn         m_KLVideoSndtrackFn;
    KLVideoOpenFn             m_KLVideoOpen;
    KLVideoCloseFn            m_KLVideoClose;
    KLVideoWaitFn             m_KLVideoWait;
    KLVideoToBufferFn         m_KLVideoToBuffer;
    KLVideoDoFrameFn          m_KLVideoDoFrame;
    KLVideoNextFrameFn        m_KLVideoNextFrame;
	KLVideoGetCurrentFrameFn  m_KLVideoGetCurrentFrame;
	KLVideoSetSndVolumeFn	  m_KLVideoSetSoundVolume;

	char					  m_szBitmapName[16];
	unsigned int			  m_uBitmapId;
	short					  m_uBitmapIsPosition;

	int			m_nBlackWidth;
	int			m_nBlackHeight;
	int			m_nbAlreadyPaintBlack;
};
