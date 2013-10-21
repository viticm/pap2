//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KWavSound.h
// Date:	2000.08.08
// Code:	Daniel Wang
// Desc:	Header File
//---------------------------------------------------------------------------
#ifndef KWavSound3D_H
#define KWavSound3D_H
#include <dsound.h>
//---------------------------------------------------------------------------

enum
{
	KVAVESOUND3D_EVENT_BUFFER_PART0,
	KVAVESOUND3D_EVENT_BUFFER_PART1,
	KVAVESOUND3D_EVENT_EXIT,

	KVAVESOUND3D_NUMEVENTS,
};

class  KWavSound3D
{
private:
    struct LOADING_PARAM 
    {
        char                    szFile[MAX_PATH];
        float                   fVolume;
        BOOL                    bLoading;
        BOOL                    bPlay;
        BOOL                    bLoop;
        BOOL                    bEnbale3DSound;
        D3DVECTOR               vPosition;
        D3DVECTOR               vVelocity;
        DWORD                   dwFlag;
        FLOAT                   fMinDistance;
        FLOAT                   fMaxDistance;
    };

public:
	KWavSound3D();
	~KWavSound3D();

	HRESULT					Load(LPSTR pszFileName);
	HRESULT					Free();
	HRESULT					Play(float fVolume, BOOL bLoop);
	HRESULT					Stop();
	HRESULT					StopAfterThisTime();
	HRESULT					SetVolume(float fVolume);

	HRESULT					SetPosition(D3DVECTOR* pvPosition, D3DVECTOR* pvVelocity, DWORD dwFlag);
	HRESULT					SetParameters(FLOAT fMinDistance, FLOAT fMaxDistance);

	BOOL					IsPlaying();
	BOOL					IsLoop();

	DWORD					GetID(){ return m_dwID; };
	void					SetID(DWORD dwID) { m_dwID = dwID; };
	float					GetOriginalVolume() { return m_fOriginalVolume; };
	void					SetOriginalVolume(float fVolume) { m_fOriginalVolume = fVolume; };
	int						GetType() { return m_nType; };
	void					SetType(int nType) { m_nType = nType; };

	void					Enable3DSound(BOOL bEnbale);
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
	LPDIRECTSOUND3DBUFFER	m_p3DBuffer;
	DS3DBUFFER				m_dsBufferParams;
	DWORD					m_dwID;
	float					m_fOriginalVolume;
	int						m_nType;
	BOOL					m_bEnbale3DSound;

	BOOL					m_bLoop;
	KWavFile				m_WaveFile;
	KThread					m_Thread;
	LPDIRECTSOUNDNOTIFY 	m_pSoundNotify;
	DSBPOSITIONNOTIFY		m_PosNotify[2];
	HANDLE					m_hEvents[KVAVESOUND3D_NUMEVENTS];
	DWORD					m_dwBufferSize;
	CRITICAL_SECTION		m_SectionSoundBuffer;
	BOOL                    m_bDelayStop;

    CRITICAL_SECTION		m_SectionLoading;
    LOADING_PARAM           m_Loading;
};
//---------------------------------------------------------------------------
#endif