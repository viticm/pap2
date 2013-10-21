//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KMusic.h
// Date:	2000.08.08
// Code:	Daniel Wang
// Desc:	Header File
//---------------------------------------------------------------------------
#ifndef KMusic_H
#define KMusic_H

//---------------------------------------------------------------------------
#include "KWavFile.h"
#include <dsound.h>
//---------------------------------------------------------------------------

#define KMUSIC_TIME_OUT_TIME 50  //50ms
#define KMUSIC_FADE_OUT_TIME (160 * KMUSIC_TIME_OUT_TIME) //4S

enum
{
    KMUSIC_EVENT_BUFFER_PART0,
    KMUSIC_EVENT_BUFFER_PART1,
    KMUSIC_EVENT_EXIT,

    NUMEVENTS,
};

enum
{
	KMUSIC_FADE_IN,
	KMUSIC_FADE_OUT,
	KMUSIC_PLAYING,
	KMUSIC_STOPED,
	KMUSIC_PAUSED,
};

class  KMusic
{
protected:
	WAVEFORMATEX			m_WaveFormat;
	LPDIRECTSOUNDBUFFER 	m_pSoundBuffer;
	LPDIRECTSOUNDNOTIFY 	m_pSoundNotify;
	DSBPOSITIONNOTIFY		m_PosNotify[2];
	HANDLE					m_hEvents[NUMEVENTS];
	DWORD					m_dwBufferSize;
	BOOL					m_bLoop;
	KThread					m_Thread;
	DWORD					m_dwID;
	float					m_fOriginalVolume;
	int						m_nType;

	CRITICAL_SECTION		m_SectionFade;
	CRITICAL_SECTION		m_SectionSoundBuffer;
	float					m_fVolume; //真正播放的音量
	int						m_nPlayState; //播放状态
	float					m_fFadePercent; //淡入淡出的进度, 淡出到<=0.0时结束，淡入到>=1.0时结束。
	float					m_nPauseCount; //pause的计数

public:
	KMusic();
	virtual ~KMusic();
	virtual BOOL			Open(LPSTR FileName);
	virtual void			Close();
	virtual void			Play(BOOL bLoop, BOOL bFadeIn);
	virtual void			Stop();
	virtual void			Rewind();
	virtual void			SetFullVolume(float fVolume);
	virtual void			SetVolume(float fVolume);
	virtual void			SetPan(LONG lPan);
	virtual BOOL			HandleNotify();
	virtual BOOL			IsPlaying();

	void					SetLoop(BOOL bLoop) { m_bLoop = bLoop; };
	BOOL					IsLoop() { return m_bLoop; };
	DWORD					GetID() { return m_dwID; };
	void					SetID(DWORD dwID) { m_dwID = dwID; };
	float					GetOriginalVolume() { return m_fOriginalVolume; };
	void					SetOriginalVolume(float fVolume) { m_fOriginalVolume = fVolume; };
	int						GetType() { return m_nType; };
	void					SetType(int nType) { m_nType = nType; };

	void					FadeIn();
	int						IsFadeIn();
	void					FadeOut();
	int						IsFadeOut();
protected:
	virtual BOOL			Init();
	virtual void			InitSoundFormat();
	virtual BOOL			InitSoundBuffer();
	virtual void			FreeSoundBuffer();
	virtual BOOL			InitSoundNotify();
	virtual void			FreeSoundNotify();
	virtual BOOL			InitEventHandle();
	virtual void			FreeEventHandle();
	virtual BOOL			FillBufferWithSound(DWORD dwPos);
	virtual BOOL			FillBufferWithSilence();
	virtual DWORD			ReadWaveData(LPBYTE lpBuf, DWORD dwLen);
	static	void			ThreadFunction(void* lpParam);

	void					OnFade();
};
//---------------------------------------------------------------------------
#endif