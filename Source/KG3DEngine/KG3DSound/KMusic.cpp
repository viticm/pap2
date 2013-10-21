//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KMusic.cpp
// Date:	2000.08.08
// Code:	Daniel Wang
// Desc:	Wave Stream Music Class
//---------------------------------------------------------------------------
#include "StdAfx.h"
#include "KDSound.h"
#include "KMusic.h"
#include "IKG3DSoundShell.h"

//---------------------------------------------------------------------------
// 函数:	KMusic
// 功能:	购造函数
// 参数:	void
// 返回:	void
//---------------------------------------------------------------------------
KMusic::KMusic()
{
	m_pSoundBuffer = NULL;
	m_pSoundNotify = NULL;
	m_dwBufferSize = 0;
	m_bLoop        = FALSE;
	m_dwID		   = 0;
	m_fOriginalVolume = 1.0f;
	m_nType = 0;

	m_fVolume = 1.0f;
	m_nPlayState = KMUSIC_STOPED;
	m_fFadePercent = 0.0f;
	m_nPauseCount = 0;

    memset(m_hEvents, 0, sizeof(m_hEvents));
	::InitializeCriticalSection(&m_SectionFade);
	::InitializeCriticalSection(&m_SectionSoundBuffer);
}

//---------------------------------------------------------------------------
// 函数:	~KMusic
// 功能:	析造函数
// 参数:	void
// 返回:	void
//---------------------------------------------------------------------------
KMusic::~KMusic()
{
	Close();
	::DeleteCriticalSection(&m_SectionSoundBuffer);
	::DeleteCriticalSection(&m_SectionFade);
}
//---------------------------------------------------------------------------
// 函数:	ThreadFunction
// 功能:	播放线程
// 参数:	void
// 返回:	void
//---------------------------------------------------------------------------
void KMusic::ThreadFunction(void* lpParam)
{
	KMusic* pMusic = (KMusic*)lpParam;
	while (TRUE)
	{
		if (!pMusic->HandleNotify())
			break;
	}
}
//---------------------------------------------------------------------------
// 函数:	Open
// 功能:	打开音乐文件
// 参数:	FileName	WAV文件名
// 返回:	TRUE－成功	FALSE－失败
//---------------------------------------------------------------------------
BOOL KMusic::Open(LPSTR FileName)
{
	return FALSE;
}
//---------------------------------------------------------------------------
// 函数:	Init
// 功能:	初始化
// 参数:	void
// 返回:	TRUE－成功	FALSE－失败
//---------------------------------------------------------------------------
BOOL KMusic::Init()
{
	// init sound format
	InitSoundFormat();

	// init sound buffer
	if (!InitSoundBuffer())
		return FALSE;

	// create event handle
	if (!InitEventHandle())
		return FALSE;

	// create sound notify
	if (!InitSoundNotify())
		return FALSE;

	// fill buffer with silence
	FillBufferWithSilence();

	// set buffer play position
	::EnterCriticalSection(&m_SectionSoundBuffer);
	m_pSoundBuffer->SetCurrentPosition(0);
	::LeaveCriticalSection(&m_SectionSoundBuffer);

	// create play thread
	m_Thread.Create(ThreadFunction, this);

	return TRUE;
}
//---------------------------------------------------------------------------
// 函数:	Close
// 功能:	释放音乐
// 参数:	void
// 返回:	void
//---------------------------------------------------------------------------
void KMusic::Close()
 {
	// 必须先等线程结束，再释放内存，防止非法操作
 	Stop();

    if (m_hEvents[KMUSIC_EVENT_EXIT])
        SetEvent(m_hEvents[KMUSIC_EVENT_EXIT]);

    m_Thread.Destroy(); // WaitForExit
	FreeSoundNotify();
	FreeSoundBuffer();
	FreeEventHandle();
}
//---------------------------------------------------------------------------
// 函数:	InitSoundFormat
// 功能:	
// 参数:	void
// 返回:	void
//---------------------------------------------------------------------------
void KMusic::InitSoundFormat()
{
}
//---------------------------------------------------------------------------
// 函数:	InitSoundBuffer
// 功能:	创建声音Buffer
// 参数:	void
// 返回:	TRUE－成功 FALSE－失败
//---------------------------------------------------------------------------
BOOL KMusic::InitSoundBuffer()
{
	DSBUFFERDESC		dsbdesc;
	HRESULT				hres;
	LPDIRECTSOUND		pDirectSound;
	
	// get direct sound
	pDirectSound = g_pDirectSound->GetDirectSound();
	if (pDirectSound == NULL)
		return FALSE;
	
	// close any open file and release interfaces
	FreeSoundBuffer();
	
	// set streaming sound buffer desc
	memset(&dsbdesc,0, sizeof(DSBUFFERDESC));
	dsbdesc.dwSize = sizeof(DSBUFFERDESC);
	dsbdesc.dwFlags = DSBCAPS_CTRLPOSITIONNOTIFY // 播放位置通知
			| DSBCAPS_GETCURRENTPOSITION2 // 可以取得当前位置
			| DSBCAPS_CTRLVOLUME // 使音量可以控制
			| DSBCAPS_LOCSOFTWARE // 使用软件混音
			| DSBCAPS_STICKYFOCUS; // 可以在后台播放
	
	// The size of the buffer is arbitrary, but should be at least
	// two seconds, to keep data writes well ahead of the play position.
	dsbdesc.dwBufferBytes = m_dwBufferSize;
	dsbdesc.lpwfxFormat = &m_WaveFormat;
	
	// create streaming sound buffer
	::EnterCriticalSection(&m_SectionSoundBuffer);
	hres = pDirectSound->CreateSoundBuffer(&dsbdesc, &m_pSoundBuffer, NULL);
	::LeaveCriticalSection(&m_SectionSoundBuffer);
	if (hres != DS_OK)
	{
		KGLogPrintf(KGLOG_ERR, "%s", "SoundShell\tKWavMusic : Create sound buffer failed");
		return FALSE;
	}
	return TRUE;
}
//---------------------------------------------------------------------------
// 函数:	FreeSoundBuffer
// 功能:	释放音乐Buffer
// 参数:	void
// 返回:	void
//---------------------------------------------------------------------------
void KMusic::FreeSoundBuffer()
{
	if (g_pDirectSound->GetDirectSound() == NULL)
		return;
	::EnterCriticalSection(&m_SectionSoundBuffer);
	SAFE_RELEASE(m_pSoundBuffer);
	::LeaveCriticalSection(&m_SectionSoundBuffer);
}
//---------------------------------------------------------------------------
// 函数:	InitEventHandle
// 功能:	创建事件句柄
// 参数:	void
// 返回:	TRUE－成功 FALSE－失败
//---------------------------------------------------------------------------
BOOL KMusic::InitEventHandle()
{
	for (int i = 0; i < NUMEVENTS; i++)
	{
		if (m_hEvents[i] == NULL)
			m_hEvents[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
		if (m_hEvents[i] == NULL)
		{
			KGLogPrintf(KGLOG_ERR, "%s", "SoundShell\tKMusic : Create event failed");
			return FALSE;
		}
	}
	return TRUE;
}
//---------------------------------------------------------------------------
// 函数:	FreeEventHandle
// 功能:	创建事件句柄
// 参数:	void
// 返回:	TRUE－成功 FALSE－失败
//---------------------------------------------------------------------------
void KMusic::FreeEventHandle()
{
	for (int i = 0; i < NUMEVENTS; i++)
	{
		if (m_hEvents[i])
		{
			CloseHandle(m_hEvents[i]);
			m_hEvents[i] = NULL;
		}
	}
}
//---------------------------------------------------------------------------
// 函数:	InitSoundNotify
// 功能:	创建SoundNotify
// 参数:	void
// 返回:	TRUE－成功 FALSE－失败
//---------------------------------------------------------------------------
BOOL KMusic::InitSoundNotify()
{
	HRESULT		hres = E_FAIL;
	
	// setup pos notify
	m_PosNotify[0].dwOffset		= 0;
	m_PosNotify[0].hEventNotify	= m_hEvents[KMUSIC_EVENT_BUFFER_PART0];
	m_PosNotify[1].dwOffset		= m_dwBufferSize / 2;
	m_PosNotify[1].hEventNotify	= m_hEvents[KMUSIC_EVENT_BUFFER_PART1];
	
	// query interface
	::EnterCriticalSection(&m_SectionSoundBuffer);
	if (m_pSoundBuffer)
		hres = m_pSoundBuffer->QueryInterface(IID_IDirectSoundNotify, (void**)&m_pSoundNotify);
	::LeaveCriticalSection(&m_SectionSoundBuffer);
	if (hres != S_OK)
	{
		KGLogPrintf(KGLOG_ERR, "%s", "SoundShell\tKMusic : QueryInterface failed");
		return FALSE;
	}
	
	// set notify position
	hres = m_pSoundNotify->SetNotificationPositions(2, m_PosNotify);
	if (hres != DS_OK)
	{
		KGLogPrintf(KGLOG_ERR, "%s", "SoundShell\tKMusic : Set notification positions failed");
		return FALSE;
	}
	
	return TRUE;
}
//---------------------------------------------------------------------------
// 函数:	FreeSoundNotify
// 功能:	释放播放通告
// 参数:	void
// 返回:	void
//---------------------------------------------------------------------------
void KMusic::FreeSoundNotify()
{
	if (g_pDirectSound->GetDirectSound() == NULL)
		return;
	SAFE_RELEASE(m_pSoundNotify);
}
//---------------------------------------------------------------------------
// 函数:	HandleNotify
// 功能:	处理音乐播放中的通告消息
// 参数:	void
// 返回:	void
//---------------------------------------------------------------------------
/*
BOOL KMusic::HandleNotify()
{
	DWORD dwEvent;
	
	// wait for event[0..3] to be signaled
	dwEvent = WaitForMultipleObjects(
		NUMEVENTS,		// How many possible events
		m_hEvents,		// Location of handles
		FALSE,			// Wait for all?
		INFINITE);		// How long to wait
	
    // WAIT_OBJECT_0 == 0 but is properly treated as an arbitrary
    // index value assigned to the first event, therefore we subtract
    // it from dwEvent to get the zero-based index of the event.
	dwEvent -= WAIT_OBJECT_0;
	
	// If the event was set by the buffer,
	// there's input to process.
    switch (dwEvent)
    {
    case KMUSIC_EVENT_BUFFER_PART0:
    case KMUSIC_EVENT_BUFFER_PART1:
		FillBufferWithSound(dwEvent);
        break;
    }

	// If the event[2] was set, exit thread
	return dwEvent != KMUSIC_EVENT_EXIT;
}
*/
BOOL KMusic::HandleNotify()
{
	BOOL bResult = TRUE;
	DWORD dwEvent;

	// wait for event[0..3] to be signaled
	dwEvent = WaitForMultipleObjects(
		NUMEVENTS,		// How many possible events
		m_hEvents,		// Location of handles
		FALSE,			// Wait for all?
		KMUSIC_TIME_OUT_TIME);		// How long to wait

	// WAIT_OBJECT_0 == 0 but is properly treated as an arbitrary
	// index value assigned to the first event, therefore we subtract
	// it from dwEvent to get the zero-based index of the event.

	// If the event was set by the buffer,
	// there's input to process.
	switch (dwEvent)
	{
	case WAIT_FAILED:
		bResult = TRUE;
		break;
	case WAIT_TIMEOUT:
		OnFade();
		bResult = TRUE;
		break;
	case WAIT_OBJECT_0 + KMUSIC_EVENT_BUFFER_PART0:
		OnFade();
		FillBufferWithSound(KMUSIC_EVENT_BUFFER_PART0);
		bResult = TRUE;
		break;
	case WAIT_OBJECT_0 + KMUSIC_EVENT_BUFFER_PART1:
		OnFade();
		FillBufferWithSound(KMUSIC_EVENT_BUFFER_PART1);
		bResult = TRUE;
		break;
	case WAIT_OBJECT_0 + KMUSIC_EVENT_EXIT:
		bResult = FALSE;
		break;
	}

	return bResult;
}

void KMusic::OnFade()
{
	::EnterCriticalSection(&m_SectionFade);

	switch(m_nPlayState)
	{
	case KMUSIC_FADE_IN:
		m_fFadePercent += (1.0f * KMUSIC_TIME_OUT_TIME / KMUSIC_FADE_OUT_TIME);
		::EnterCriticalSection(&m_SectionSoundBuffer);
		if (m_pSoundBuffer)
			m_pSoundBuffer->SetVolume(VolumeToDecibels(m_fVolume * m_fFadePercent));
		::LeaveCriticalSection(&m_SectionSoundBuffer);
		if (m_fFadePercent >= 1.0f)
		{
			m_fFadePercent = 1.0f;
			m_nPlayState = KMUSIC_PLAYING;
		}
		break;
	case KMUSIC_FADE_OUT:
		m_fFadePercent -= (1.0f * KMUSIC_TIME_OUT_TIME / KMUSIC_FADE_OUT_TIME);
		::EnterCriticalSection(&m_SectionSoundBuffer);
		if (m_pSoundBuffer)
			m_pSoundBuffer->SetVolume(VolumeToDecibels(m_fVolume * m_fFadePercent));
		::LeaveCriticalSection(&m_SectionSoundBuffer);
		if (m_fFadePercent <= 0.0f)
		{
			m_fFadePercent = 0.0f;
			m_nPlayState = KMUSIC_STOPED;
			Rewind();
			::EnterCriticalSection(&m_SectionSoundBuffer);
			if (m_pSoundBuffer)
				m_pSoundBuffer->Stop();
			::LeaveCriticalSection(&m_SectionSoundBuffer);
		}
		break;
	case KMUSIC_PAUSED:
		m_nPauseCount--;
		if (m_nPauseCount < 0)
		{
			m_nPauseCount = 0;
			m_nPlayState = KMUSIC_PLAYING;
			::EnterCriticalSection(&m_SectionSoundBuffer);
			if (m_pSoundBuffer)
				m_pSoundBuffer->Play(0, 0, DSBPLAY_LOOPING);
			::LeaveCriticalSection(&m_SectionSoundBuffer);
		}
		break;
	case KMUSIC_PLAYING:
	case KMUSIC_STOPED:
		break;
	}

	::LeaveCriticalSection(&m_SectionFade);
}

//---------------------------------------------------------------------------
// 函数:	FillBufferWithSound
// 功能:	填充音乐Buffer
// 参数:	dwPos	位置
// 返回:	TRUE－成功 FALSE－失败
//---------------------------------------------------------------------------
BOOL KMusic::FillBufferWithSound(DWORD dwPos)
{
	DWORD		dwStartOfs;
	LONG		lNumToWrite;
	PVOID		lpvPtr1;
	PVOID		lpvPtr2;
	DWORD		dwBytes1;
	DWORD		dwBytes2;
	DWORD		dwBytesRead;
	HRESULT		hres;
	BOOL		bResult = FALSE;

	// check sound buffer

	// decide start offset in sound buffer
	if (dwPos == 0)
		dwStartOfs = m_PosNotify[1].dwOffset;
	else
		dwStartOfs = m_PosNotify[0].dwOffset;

	// decide number bytes to write to buffer
	lNumToWrite = m_dwBufferSize / 2;

	// lock buffer to obtain buffer pointer
	::EnterCriticalSection(&m_SectionSoundBuffer);
	if (m_pSoundBuffer)
	{
		hres = m_pSoundBuffer->Lock(
			dwStartOfs,       // Offset of lock start
			lNumToWrite,      // Number of bytes to lock
			&lpvPtr1,         // Address of lock start
			&dwBytes1,        // Count of bytes locked
			&lpvPtr2,         // Address of wrap around
			&dwBytes2,        // Count of wrap around bytes
			0);               // Flags

		// check return value
		if (SUCCEEDED(hres))
		{
			// Read wave data from the file into the buffer
			dwBytesRead = ReadWaveData((PBYTE)lpvPtr1, dwBytes1);

			// if reached end of file
			if (dwBytesRead < dwBytes1)
			{
				memset((PBYTE)lpvPtr1 + dwBytesRead,(BYTE)0&0xff,
					dwBytes1 - dwBytesRead);
			}

			// unlock sound buffer
			m_pSoundBuffer->Unlock(lpvPtr1, dwBytes1, lpvPtr2, dwBytes2);
			bResult = TRUE;
		}
	}
	::LeaveCriticalSection(&m_SectionSoundBuffer);
	if (!bResult)
		return FALSE;
	
	// if reached end of file
	if (dwBytesRead < dwBytes1)
	{
		Rewind();
		::EnterCriticalSection(&m_SectionFade);

		::EnterCriticalSection(&m_SectionSoundBuffer);
		if (m_pSoundBuffer)
			m_pSoundBuffer->Stop();
		::LeaveCriticalSection(&m_SectionSoundBuffer);
		if (m_nPlayState == KMUSIC_FADE_OUT)
		{
			m_fFadePercent = 0.0f;
			m_nPlayState = KMUSIC_STOPED;
		}
		else
		{
			m_nPauseCount = 400; //20s
			if (!m_bLoop)
				m_nPauseCount = 2400;//60s
			m_nPlayState = KMUSIC_PAUSED;
		}
		::LeaveCriticalSection(&m_SectionFade);
	}

	return TRUE;
}
//---------------------------------------------------------------------------
// 函数:	FillMusicBufferWithSilence
// 功能:	用空白填充音乐Buffer
// 参数:	void
// 返回:	TRUE－成功 FALSE－失败
//---------------------------------------------------------------------------
BOOL KMusic::FillBufferWithSilence()
{
	PVOID   pb1, pb2;
	DWORD   cb1, cb2;
	HRESULT	hres;
	BOOL bResult = FALSE;
	
	// check sound buffer
	::EnterCriticalSection(&m_SectionSoundBuffer);
	if (m_pSoundBuffer)
	{
		hres = m_pSoundBuffer->Lock(0, m_dwBufferSize, &pb1, &cb1, &pb2, &cb2, 0);
		if (SUCCEEDED(hres))
		{
			memset(pb1, (BYTE)0&0xff, cb1);
			m_pSoundBuffer->Unlock(pb1, cb1, pb2, cb2);
			bResult = TRUE;
		}	
	}
	::LeaveCriticalSection(&m_SectionSoundBuffer);
	
	return bResult;
}
//---------------------------------------------------------------------------
// 函数:	ReadWaveData
// 功能:	读取波形数据
// 参数:	lpBuf	缓存
//			dwLen	长度	
// 返回:	DWORD	实际读取的长度
//---------------------------------------------------------------------------
DWORD KMusic::ReadWaveData(LPBYTE lpBuf, DWORD dwLen)
{
	return 0;
}
//---------------------------------------------------------------------------
// 函数:	Play
// 功能:	播放音乐
// 参数:	bLoop		是否循环播放
// 返回:	void
//---------------------------------------------------------------------------
void KMusic::Play(BOOL bLoop, BOOL bFadeIn)
{
	::EnterCriticalSection(&m_SectionFade);
	::EnterCriticalSection(&m_SectionSoundBuffer);
	if (m_nPlayState != KMUSIC_PAUSED)
	{
		if (m_pSoundBuffer)
			m_pSoundBuffer->Play(0, 0, DSBPLAY_LOOPING);
	}
	m_bLoop = bLoop;
	m_nPlayState = KMUSIC_PLAYING;
	if (bFadeIn)
	{
		if (m_nPlayState != KMUSIC_FADE_IN && m_nPlayState != KMUSIC_FADE_OUT)
		{
			m_fFadePercent = 0.0f;
			if (m_pSoundBuffer)
				m_pSoundBuffer->SetVolume(VolumeToDecibels(m_fVolume * m_fFadePercent));
		}
		m_nPlayState = KMUSIC_FADE_IN;
	}
	::LeaveCriticalSection(&m_SectionSoundBuffer);
	::LeaveCriticalSection(&m_SectionFade);
}
//---------------------------------------------------------------------------
// 函数:	Stop
// 功能:	停止音乐播放
// 参数:	void
// 返回:	void
//---------------------------------------------------------------------------
void KMusic::Stop()
{
	::EnterCriticalSection(&m_SectionFade);
	::EnterCriticalSection(&m_SectionSoundBuffer);
	if (m_pSoundBuffer)
		m_pSoundBuffer->Stop();
	m_nPlayState = KMUSIC_STOPED;
	::LeaveCriticalSection(&m_SectionSoundBuffer);
	::LeaveCriticalSection(&m_SectionFade);
}

//---------------------------------------------------------------------------
// 函数:	IsPlaying		zroc add 不知道正不正确
// 功能:	判断音乐是否正在播放
// 参数:	void
// 返回:	TRUE 正在播放 FALSE 播完了
//---------------------------------------------------------------------------
BOOL KMusic::IsPlaying()
{
	BOOL bResult = FALSE;

	::EnterCriticalSection(&m_SectionSoundBuffer);
	if (m_pSoundBuffer)
	{
		DWORD	Status = 0;
		m_pSoundBuffer->GetStatus(&Status);
		if (Status & DSBSTATUS_PLAYING)
			bResult = TRUE;
	}
	::LeaveCriticalSection(&m_SectionSoundBuffer);

	return bResult;
}

//---------------------------------------------------------------------------
// 函数:	SetVolume
// 功能:	设置音乐的音量
// 参数:	lVolume		音量 [0, -10000]
//					  	DSBVOLUME_MAX = 0
//						DSBVOLUME_MIN = -10000
// 返回:	void
//---------------------------------------------------------------------------
void KMusic::SetVolume(float fVolume)
{
	::EnterCriticalSection(&m_SectionFade);
	::EnterCriticalSection(&m_SectionSoundBuffer);
	if (m_pSoundBuffer)
	{
		m_fVolume = fVolume;
		if (m_nPlayState == KMUSIC_FADE_OUT || m_nPlayState == KMUSIC_FADE_IN)
			fVolume = m_fVolume * m_fFadePercent;

		LONG lVolume = VolumeToDecibels(fVolume);
		if (lVolume == DSBVOLUME_MAX)
			m_pSoundBuffer->SetVolume(DSBVOLUME_MAX - 1);
		else
			m_pSoundBuffer->SetVolume(lVolume + 1);
		m_pSoundBuffer->SetVolume(lVolume);
	}
	::LeaveCriticalSection(&m_SectionSoundBuffer);
	::LeaveCriticalSection(&m_SectionFade);
}

void KMusic::SetFullVolume(float fVolume)
{
	::EnterCriticalSection(&m_SectionFade);
	::EnterCriticalSection(&m_SectionSoundBuffer);
	m_fVolume = fVolume;
	::LeaveCriticalSection(&m_SectionSoundBuffer);
	::LeaveCriticalSection(&m_SectionFade);
}


void KMusic::SetPan(LONG lPan)
{
	::EnterCriticalSection(&m_SectionSoundBuffer);
	if (m_pSoundBuffer)
		m_pSoundBuffer->SetPan(lPan);
	::LeaveCriticalSection(&m_SectionSoundBuffer);
}

//---------------------------------------------------------------------------
// 函数:	Rewind
// 功能:	从头播放音乐
// 参数:	void
// 返回:	void
//---------------------------------------------------------------------------
void KMusic::Rewind()
{
}

void KMusic::FadeIn()
{
	::EnterCriticalSection(&m_SectionFade);
	if (m_nPlayState != KMUSIC_FADE_IN && m_nPlayState != KMUSIC_FADE_OUT)
		m_fFadePercent = 0.0f;
	m_nPlayState = KMUSIC_FADE_IN;
	::LeaveCriticalSection(&m_SectionFade);
}

int KMusic::IsFadeIn()
{
	int nResult = false;
	::EnterCriticalSection(&m_SectionFade);
	if (m_nPlayState == KMUSIC_FADE_IN)
		nResult = true;
	::LeaveCriticalSection(&m_SectionFade);
	return nResult;
}

void KMusic::FadeOut()
{
	::EnterCriticalSection(&m_SectionFade);
	if (m_nPlayState != KMUSIC_FADE_IN && m_nPlayState != KMUSIC_FADE_OUT)
		m_fFadePercent = 1.0f;
	m_nPlayState = KMUSIC_FADE_OUT;
	::LeaveCriticalSection(&m_SectionFade);
}


int KMusic::IsFadeOut()
{
	int nResult = false;
	::EnterCriticalSection(&m_SectionFade);
	if (m_nPlayState == KMUSIC_FADE_OUT)
		nResult = true;
	::LeaveCriticalSection(&m_SectionFade);
	return nResult;
}


