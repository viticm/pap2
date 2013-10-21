////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KRecordSound.h
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2008-1-21 16:46:48
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KRECORDSOUND_H_
#define _INCLUDE_KRECORDSOUND_H_

////////////////////////////////////////////////////////////////////////////////
#include <vector>

enum
{
	KRECORDSOUND_EVENT_BUFFER_PART0,
	KRECORDSOUND_EVENT_BUFFER_PART1,
	KRECORDSOUND_EVENT_EXIT,

	KRECORDSOUND_NUMEVENTS,
};

#define RECORDSOUND_BUFFERR_SIZE 1024 * 256 //256K

class KRecordSound
{
public:
	HRESULT StartRecordSound();
	HRESULT StopRecordSound();
	HRESULT LockRecordSoundBuffer(void **ppBuffer, DWORD *pdwSize);
	HRESULT ClearRecordSoundBuffer();
	HRESULT UnlockRecordSoundBuffer();
	WAVEFORMATEX *GetWaveFormatEx();

	KRecordSound();
	virtual ~KRecordSound();
protected:
	static	void ThreadFunction(void* lpParam);
	
	HRESULT RecordSound(DWORD dwPart);
private:
	HANDLE m_hEvents[KRECORDSOUND_NUMEVENTS];
	LPDIRECTSOUNDCAPTURE8 m_pDSCapture;
	LPDIRECTSOUNDCAPTUREBUFFER m_pDSCaptureBuff;
	LPDIRECTSOUNDNOTIFY8 m_pDSNotify;
	char m_szBuffer[RECORDSOUND_BUFFERR_SIZE];
	DWORD m_dwBufferSize;
	DWORD m_dwNBotifyBufferSize;
	CRITICAL_SECTION m_SectionBuffer;
	KThread	m_Thread;
	WAVEFORMATEX m_WaveFormatEx;
	int		m_nSkip;
};


#endif //_INCLUDE_KRECORDSOUND_H_
