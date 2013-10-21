//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KWavSound.h
// Date:	2000.08.08
// Code:	Daniel Wang
// Desc:	Header File
//---------------------------------------------------------------------------
#ifndef KWavSound_H
#define KWavSound_H
#include <dsound.h>
//---------------------------------------------------------------------------

enum
{
	KVAVESOUND_EVENT_BUFFER_PART0,
	KVAVESOUND_EVENT_BUFFER_PART1,
	KVAVESOUND_EVENT_EXIT,

	KVAVESOUND_NUMEVENTS,
};

#define MAX_WAVE_SOUND_BUFFER_SIZE  1024 * 22
#define LOOP_WAVE_SOUND_BUFFER_SIZE  1024 * 10

class  KWavSound
{
private:
    struct LOADING_PARAM 
    {
        char                    szFile[MAX_PATH];
        float                   fVolume;
        BOOL                    bLoading;
        BOOL                    bPlay;
        BOOL                    bLoop;
        int                     nPan;
    };

public:
	KWavSound();
	~KWavSound();
	HRESULT					Load(LPSTR pszFileName);
	HRESULT					Free();
	HRESULT					Play(int nPan, float fVolume, BOOL bLoop);
	HRESULT					Stop();
	HRESULT					StopAfterThisTime();
	HRESULT					SetPan(int nPan);
	HRESULT					SetVolume(float fVolume);
	BOOL					IsPlaying();

	BOOL					IsLoop();

	DWORD					GetID() { return m_dwID; };
	void					SetID(DWORD dwID) {m_dwID = dwID; };
	float					GetOriginalVolume() { return m_fOriginalVolume; };
	void					SetOriginalVolume(float fVolume) { m_fOriginalVolume = fVolume; };
	int						GetType() { return m_nType; };
	void					SetType(int nType) { m_nType = nType; };

private:
    BOOL                    Init();
	BOOL					InitSoundBuffer();
	void					FreeSoundBuffer();
	BOOL					InitSoundNotify();
	void					FreeSoundNotify();
	BOOL					InitEventHandle();
	void					FreeEventHandle();
	BOOL					HandleNotify();
	BOOL					FillBufferWithSound(DWORD dwPos);

	static	void			ThreadFunction(void* lpParam);
private:
	LPDIRECTSOUNDBUFFER 	m_pBuffer;
	DWORD					m_dwID;
	float					m_fOriginalVolume;
	int						m_nType;

	BOOL					m_bLoop;
	KWavFile				m_WaveFile;
	KThread					m_Thread;
	LPDIRECTSOUNDNOTIFY 	m_pSoundNotify;
	DSBPOSITIONNOTIFY		m_PosNotify[2];
	HANDLE					m_hEvents[KVAVESOUND_NUMEVENTS];
	DWORD					m_dwBufferSize;
	CRITICAL_SECTION		m_SectionSoundBuffer;
	BOOL					m_bDelayStop;

    CRITICAL_SECTION		m_SectionLoading;
    LOADING_PARAM           m_Loading;
};
//---------------------------------------------------------------------------
#endif