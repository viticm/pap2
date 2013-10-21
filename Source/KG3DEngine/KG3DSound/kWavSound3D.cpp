//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KWavSound3D.cpp
// Date:	2000.08.08
// Code:	Daniel Wang
// Desc:	Wav Sound Class
//---------------------------------------------------------------------------
#include "stdafx.h" 
#include "KWavFile.h"
#include "KWavSound3D.h"
#include "KDSound.h"

extern DWORD dwApplyMode; 
//---------------------------------------------------------------------------
// 函数:	KWavSound3D
// 功能:	购造函数
// 参数:	void
// 返回:	void
//---------------------------------------------------------------------------
KWavSound3D::KWavSound3D()
{
	m_pBuffer = NULL;
	m_p3DBuffer = NULL;
	m_dwID = 0;
	m_fOriginalVolume = 1.0f;
	m_nType = 0;
	m_bEnbale3DSound = TRUE;

	m_bLoop = FALSE;
	m_pSoundNotify = NULL;
	memset(m_PosNotify, 0, sizeof(m_PosNotify));
	memset(m_hEvents, 0, sizeof(m_hEvents));
	m_dwBufferSize = 0;

    m_bDelayStop = FALSE;

    memset(&m_Loading, 0, sizeof(m_Loading));
    m_Loading.bEnbale3DSound = m_bEnbale3DSound;

    ::InitializeCriticalSection(&m_SectionLoading);
    ::InitializeCriticalSection(&m_SectionSoundBuffer);
}

//---------------------------------------------------------------------------
// 函数:	~KWavSound3D
// 功能:	析造函数
// 参数:	void
// 返回:	void
//---------------------------------------------------------------------------
KWavSound3D::~KWavSound3D()
{
	Free();
	::DeleteCriticalSection(&m_SectionSoundBuffer);
    ::DeleteCriticalSection(&m_SectionLoading);
}

//---------------------------------------------------------------------------
// 函数:	Load
// 功能:	载入音效
// 参数:	FileName	文件名
// 返回:	音效索引
//---------------------------------------------------------------------------
HRESULT KWavSound3D::Load(LPSTR pszFileName)
{
	HRESULT 		hr = E_FAIL;
    HRESULT         hrResult = E_FAIL;
	LPDIRECTSOUND8	pDirectSound = NULL;
	BOOL			bRetCode = FALSE;

	ASSERT(g_pDirectSound);

	KG_PROCESS_ERROR(pszFileName && pszFileName[0] != '\0');

    pDirectSound = g_pDirectSound->GetDirectSound();
    KG_PROCESS_ERROR(pDirectSound);

	Free();

    ::EnterCriticalSection(&m_SectionLoading);
    memset(&m_Loading, 0, sizeof(m_Loading));
    m_Loading.bEnbale3DSound = m_bEnbale3DSound;
    strncpy(m_Loading.szFile, pszFileName, _countof(m_Loading.szFile));
    m_Loading.bLoading = true;
    ::LeaveCriticalSection(&m_SectionLoading);

    // create event handle
    bRetCode = InitEventHandle();
    KG_PROCESS_ERROR(bRetCode);

    // create play thread
    m_Thread.Create(ThreadFunction, this);

	hrResult = S_OK;
Exit0:
	return hrResult;
}

BOOL KWavSound3D::Init()
{
    BOOL bResult = FALSE;
    BOOL bRetCode = FALSE;
    
    // open the file and get ready to read
    bRetCode = m_WaveFile.Open(m_Loading.szFile);
    KG_PROCESS_ERROR(bRetCode);

    bRetCode = InitSoundBuffer();
    KG_PROCESS_ERROR(bRetCode);

    // create sound notify
    bRetCode = InitSoundNotify();
    KG_PROCESS_ERROR(bRetCode);

    ::EnterCriticalSection(&m_SectionLoading);
    m_Loading.bLoading = false;
    SetPosition(&m_Loading.vPosition, &m_Loading.vVelocity, m_Loading.dwFlag);
    SetParameters(m_Loading.fMinDistance, m_Loading.fMaxDistance);
    Enable3DSound(m_Loading.bEnbale3DSound);
    if (m_Loading.bPlay)
    {
        Play(m_Loading.fVolume, m_Loading.bLoop);
    }
    ::LeaveCriticalSection(&m_SectionLoading);

    bResult = TRUE;
Exit0:
    m_Loading.bLoading = false;
    return bResult;
}

void KWavSound3D::ThreadFunction(void* lpParam)
{
	KWavSound3D* pWavSound3D = (KWavSound3D*)lpParam;
    if (pWavSound3D->Init())
    {
	    while (TRUE)
	    {
		    if (!pWavSound3D->HandleNotify())
			    break;
	    }
    }
}

BOOL KWavSound3D::HandleNotify()
{
	BOOL bResult = TRUE;
	DWORD dwEvent = WaitForMultipleObjects(KVAVESOUND3D_NUMEVENTS, m_hEvents, FALSE, INFINITE);

	switch (dwEvent)
	{
	case WAIT_OBJECT_0 + KVAVESOUND3D_EVENT_BUFFER_PART0:
		FillBufferWithSound(KVAVESOUND3D_EVENT_BUFFER_PART0);
		bResult = TRUE;
		break;
	case WAIT_OBJECT_0 + KVAVESOUND3D_EVENT_BUFFER_PART1:
		FillBufferWithSound(KVAVESOUND3D_EVENT_BUFFER_PART1);
		bResult = TRUE;
		break;
	case WAIT_OBJECT_0 + KVAVESOUND3D_EVENT_EXIT:
		bResult = FALSE;
		break;
	}

	return bResult;
}



BOOL KWavSound3D::FillBufferWithSound(DWORD dwPos)
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
	BOOL		bEnter = FALSE;

	KG_PROCESS_ERROR(m_pBuffer);
	// check sound buffer

	if (m_bDelayStop)
	{
		::EnterCriticalSection(&m_SectionSoundBuffer);
		m_pBuffer->Stop();
		::LeaveCriticalSection(&m_SectionSoundBuffer);
		goto Exit1;
	}

	// decide start offset in sound buffer
	if (dwPos == KVAVESOUND3D_EVENT_BUFFER_PART1)
		dwStartOfs = 0;
	else
		dwStartOfs = m_PosNotify[1].dwOffset + 1;

	// decide number bytes to write to buffer
	lNumToWrite = m_dwBufferSize / 2;

	// lock buffer to obtain buffer pointer
	::EnterCriticalSection(&m_SectionSoundBuffer);
	bEnter = TRUE;
	hres = m_pBuffer->Lock(
		dwStartOfs,       // Offset of lock start
		lNumToWrite,      // Number of bytes to lock
		&lpvPtr1,         // Address of lock start
		&dwBytes1,        // Count of bytes locked
		&lpvPtr2,         // Address of wrap around
		&dwBytes2,        // Count of wrap around bytes
		0);               // Flags
	KG_COM_PROCESS_ERROR(hres);

	// Read wave data from the file into the buffer
	dwBytesRead = m_WaveFile.Read(lpvPtr1, dwBytes1);

	// if reached end of file
	if (m_bLoop)
	{
		while (dwBytesRead < dwBytes1)
		{
			if (m_bLoop)
			{
				m_WaveFile.Rewind();
				dwBytesRead += m_WaveFile.Read((PBYTE)lpvPtr1 + dwBytesRead, dwBytes1 - dwBytesRead);
			}
		}
	}
	else
	{
		if (dwBytesRead < dwBytes1)
		{
			WAVEFORMATEX wtf;
			m_WaveFile.GetPcmWavFormat(&wtf);
			memset((PBYTE)lpvPtr1 + dwBytesRead, wtf.wBitsPerSample == 8 ? 128 : 0 , dwBytes1 - dwBytesRead);
			if (dwBytesRead == 0)
			{
				m_bDelayStop = TRUE;
			}
		}
	}

	// unlock sound buffer
	m_pBuffer->Unlock(lpvPtr1, dwBytes1, lpvPtr2, dwBytes2);

	::LeaveCriticalSection(&m_SectionSoundBuffer);
	bEnter = FALSE;
Exit1:
	bResult = TRUE;
Exit0:
	if (bEnter)
		::LeaveCriticalSection(&m_SectionSoundBuffer);
	return bResult;
}


//---------------------------------------------------------------------------
// 函数:	Free
// 功能:	释放音效
// 参数:	void
// 返回:	void
//---------------------------------------------------------------------------
HRESULT KWavSound3D::Free()
{

	Stop();

	if (m_hEvents[KVAVESOUND3D_EVENT_EXIT])
		SetEvent(m_hEvents[KVAVESOUND3D_EVENT_EXIT]);

	m_Thread.Destroy(); // WaitForExit
	FreeSoundNotify();
	FreeSoundBuffer();
	FreeEventHandle();

	::EnterCriticalSection(&m_SectionSoundBuffer);
	m_WaveFile.Close();
	::LeaveCriticalSection(&m_SectionSoundBuffer);

    m_Loading.bLoading = false;

	return S_OK;
}

//---------------------------------------------------------------------------
// 函数:	Play
// 功能:	播放音效
//			nVolume		音量 [0, -10000]
//					  	DSBVOLUME_MAX = 0
//						DSBVOLUME_MIN = -10000
//			bLoop		循环 [0,1]
//						0 : 不循环播放
//						1 : 循环播放
// 返回:	void
//---------------------------------------------------------------------------
HRESULT KWavSound3D::Play(float fVolume, BOOL bLoop)
{
    HRESULT hr = E_FAIL;
    BOOL bEnter = FALSE;

    if (m_Loading.bLoading)
    {
        ::EnterCriticalSection(&m_SectionLoading);
        m_Loading.bPlay = TRUE;
        m_Loading.fVolume = fVolume;
        m_Loading.bLoop = bLoop;
        ::LeaveCriticalSection(&m_SectionLoading);
        return S_OK;
    }

	KG_PROCESS_ERROR(m_pBuffer);

	m_bLoop = bLoop;
	m_bDelayStop = FALSE;

	::EnterCriticalSection(&m_SectionSoundBuffer);
    bEnter = TRUE;
	m_WaveFile.Rewind();
	::LeaveCriticalSection(&m_SectionSoundBuffer);
    bEnter = FALSE;

	FillBufferWithSound(KVAVESOUND3D_EVENT_BUFFER_PART1);

	::EnterCriticalSection(&m_SectionSoundBuffer);
    bEnter = TRUE;
	hr = m_pBuffer->SetCurrentPosition(0);
    KG_COM_PROCESS_ERROR(hr);
	hr = m_pBuffer->SetVolume(VolumeToDecibels(fVolume));
    KG_COM_PROCESS_ERROR(hr);
	hr = m_pBuffer->Play(0, 0, DSBPLAY_LOOPING);
    KG_COM_PROCESS_ERROR(hr);
	::LeaveCriticalSection(&m_SectionSoundBuffer);
    bEnter = FALSE;

	return S_OK;
Exit0:
    if (bEnter)
        ::LeaveCriticalSection(&m_SectionSoundBuffer);
	return E_FAIL;
}

//---------------------------------------------------------------------------
// 函数:	Stop
// 功能:	停止播放声效
// 参数:	void
// 返回:	void
//---------------------------------------------------------------------------
HRESULT KWavSound3D::Stop()
{
	::EnterCriticalSection(&m_SectionLoading);

    HRESULT hr = E_FAIL;

    if (m_Loading.bLoading)
    {    
        m_Loading.bPlay = FALSE;
        ::LeaveCriticalSection(&m_SectionLoading);
        return S_OK;
    }

	KG_PROCESS_ERROR(m_pBuffer);

	::EnterCriticalSection(&m_SectionSoundBuffer);
	hr = m_pBuffer->Stop();
	::LeaveCriticalSection(&m_SectionSoundBuffer);

	KG_COM_PROCESS_ERROR(hr);

	::LeaveCriticalSection(&m_SectionLoading);
	return S_OK;
Exit0:
	::LeaveCriticalSection(&m_SectionLoading);
	return E_FAIL;
}

HRESULT	KWavSound3D::StopAfterThisTime()
{
    if (m_Loading.bLoading)
    {
        ::EnterCriticalSection(&m_SectionLoading);
        m_Loading.bLoop = FALSE;
        ::LeaveCriticalSection(&m_SectionLoading);
        return S_OK;
    }

	m_bLoop = FALSE;
	return S_OK;
}

//---------------------------------------------------------------------------
// 函数:	SetVolume
// 功能:	设置音量
// 参数:	nVolume		音量 [0, -10000]
//					  	DSBVOLUME_MAX = 0
//						DSBVOLUME_MIN = -10000
// 返回:	void
//---------------------------------------------------------------------------
HRESULT KWavSound3D::SetVolume(float fVolume)
{
	HRESULT hr = E_FAIL;

    if (m_Loading.bLoading)
    {
        ::EnterCriticalSection(&m_SectionLoading);
        m_Loading.fVolume = fVolume;
        ::LeaveCriticalSection(&m_SectionLoading);
        
        return S_OK;
    }

	KG_PROCESS_ERROR(m_pBuffer);

	::EnterCriticalSection(&m_SectionSoundBuffer);
	hr = m_pBuffer->SetVolume(VolumeToDecibels(fVolume));
	::LeaveCriticalSection(&m_SectionSoundBuffer);
	KG_COM_PROCESS_ERROR(hr);

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT	KWavSound3D::SetPosition(D3DVECTOR* pvPosition, D3DVECTOR* pvVelocity, DWORD dwFlag)
{
    if (m_Loading.bLoading)
    {
        ::EnterCriticalSection(&m_SectionLoading);
        if (pvPosition)
            memcpy(&m_Loading.vPosition, pvPosition, sizeof(D3DVECTOR));
        if(pvVelocity)
            memcpy(&m_Loading.vVelocity, pvVelocity, sizeof(D3DVECTOR));
        ::LeaveCriticalSection(&m_SectionLoading);
        return S_OK;
    }

    ::EnterCriticalSection(&m_SectionSoundBuffer);
	if (pvPosition)
	{
		memcpy(&m_dsBufferParams.vPosition, pvPosition, sizeof(D3DVECTOR));
		if (m_p3DBuffer)
				m_p3DBuffer->SetPosition(pvPosition->x, pvPosition->y, pvPosition->z, dwFlag);
	}
	if (pvVelocity)
	{
		memcpy(&m_dsBufferParams.vVelocity, pvVelocity, sizeof(D3DVECTOR));
		if (m_p3DBuffer)
			m_p3DBuffer->SetVelocity(pvVelocity->x, pvVelocity->y, pvVelocity->z, dwFlag);
	}
    ::LeaveCriticalSection(&m_SectionSoundBuffer);

	return S_OK;
}

HRESULT	KWavSound3D::SetParameters(FLOAT fMinDistance, FLOAT fMaxDistance)
{
    if (m_Loading.bLoading)
    {
        ::EnterCriticalSection(&m_SectionLoading);
        m_Loading.fMinDistance = fMinDistance;
        m_Loading.fMaxDistance = fMaxDistance;
        ::LeaveCriticalSection(&m_SectionLoading);
        return S_OK;
    }

	m_dsBufferParams.flMinDistance = fMinDistance;
	m_dsBufferParams.flMaxDistance = fMaxDistance;
    ::EnterCriticalSection(&m_SectionSoundBuffer);
    if (m_p3DBuffer)
        m_p3DBuffer->SetAllParameters(&m_dsBufferParams, dwApplyMode);
    ::LeaveCriticalSection(&m_SectionSoundBuffer);
	return S_OK;
}


void KWavSound3D::Enable3DSound(BOOL bEnbale)
{
    if (m_Loading.bLoading)
    {
        ::EnterCriticalSection(&m_SectionLoading);
        m_Loading.bEnbale3DSound = bEnbale;
        ::LeaveCriticalSection(&m_SectionLoading);
        return;
    }

	if (m_p3DBuffer)
	{
		if (bEnbale)
		{
			if (!m_bEnbale3DSound)
			{
				m_dsBufferParams.dwMode = DS3DMODE_NORMAL;
                ::EnterCriticalSection(&m_SectionSoundBuffer);
				m_p3DBuffer->SetAllParameters(&m_dsBufferParams, dwApplyMode);
                ::LeaveCriticalSection(&m_SectionSoundBuffer);
			}
		}
		else
		{
			if (m_bEnbale3DSound)
			{
				m_dsBufferParams.dwMode = DS3DMODE_DISABLE;
                ::EnterCriticalSection(&m_SectionSoundBuffer);
                m_p3DBuffer->SetAllParameters(&m_dsBufferParams, dwApplyMode);
                ::LeaveCriticalSection(&m_SectionSoundBuffer);
			}
		}
	}
	m_bEnbale3DSound = bEnbale;
}

BOOL KWavSound3D::InitSoundBuffer()
{
	DSBUFFERDESC		dsbdesc;
	HRESULT				hr = E_FAIL;
	LPDIRECTSOUND		pDirectSound = NULL;
	WAVEFORMATEX		PcmWavFmt;
	BOOL				bEnter = FALSE;

	// get direct sound
	pDirectSound = g_pDirectSound->GetDirectSound();
	KG_PROCESS_ERROR(pDirectSound);

	// close any open file and release interfaces
	FreeSoundBuffer();

    m_WaveFile.GetPcmWavFormat(&PcmWavFmt);
    DWORD nBlockAlign = (DWORD)PcmWavFmt.nBlockAlign;
    m_dwBufferSize = PcmWavFmt.nSamplesPerSec * BUFFER_LENGTH * nBlockAlign / NUM_PLAY_NOTIFICATIONS;
    m_dwBufferSize -= m_dwBufferSize % nBlockAlign;
    m_dwBufferSize *= NUM_PLAY_NOTIFICATIONS;

	// set streaming sound buffer desc
	memset(&dsbdesc,0, sizeof(DSBUFFERDESC));
	dsbdesc.dwSize = sizeof(DSBUFFERDESC);
	dsbdesc.dwFlags = DSBCAPS_CTRL3D| DSBCAPS_CTRLVOLUME | DSBCAPS_LOCSOFTWARE;
	dsbdesc.dwFlags |= (DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_GETCURRENTPOSITION2);

	// The size of the buffer is arbitrary, but should be at least
	// two seconds, to keep data writes well ahead of the play position.
	dsbdesc.dwBufferBytes = m_dwBufferSize;
	m_WaveFile.GetPcmWavFormat(&PcmWavFmt);
	dsbdesc.lpwfxFormat = &PcmWavFmt;
	KGLOG_PROCESS_ERROR(PcmWavFmt.nChannels == 1);

	// create streaming sound buffer
	::EnterCriticalSection(&m_SectionSoundBuffer);
	bEnter = TRUE;
	hr = pDirectSound->CreateSoundBuffer(&dsbdesc, &m_pBuffer, NULL);
	KGLOG_COM_PROCESS_ERROR(hr);

	//通过辅助缓冲区，来获取3D buffer的指针。
	hr = m_pBuffer->QueryInterface(IID_IDirectSound3DBuffer, (VOID**)&m_p3DBuffer);
	KGLOG_COM_PROCESS_ERROR(hr);

	m_dsBufferParams.dwSize = sizeof(DS3DBUFFER);

	hr = m_p3DBuffer->GetAllParameters(&m_dsBufferParams);
	KGLOG_COM_PROCESS_ERROR(hr);

	if (!m_bEnbale3DSound)
	{
		m_dsBufferParams.dwMode = DS3DMODE_DISABLE;

		hr = m_p3DBuffer->SetAllParameters(&m_dsBufferParams, dwApplyMode);
		KG_COM_PROCESS_ERROR(hr);
	}

	::LeaveCriticalSection(&m_SectionSoundBuffer);
	bEnter = FALSE;

	return TRUE;
Exit0:
	if (bEnter)
		::LeaveCriticalSection(&m_SectionSoundBuffer);
	return FALSE;
}

void KWavSound3D::FreeSoundBuffer()
{
	if (g_pDirectSound->GetDirectSound() == NULL)
		return;
	::EnterCriticalSection(&m_SectionSoundBuffer);
	SAFE_RELEASE(m_p3DBuffer);
	SAFE_RELEASE(m_pBuffer);
	::LeaveCriticalSection(&m_SectionSoundBuffer);
}


BOOL KWavSound3D::InitSoundNotify()
{
	HRESULT		hres = E_FAIL;

	// setup pos notify
	m_PosNotify[0].dwOffset		= 0;
	m_PosNotify[0].hEventNotify	= m_hEvents[KVAVESOUND3D_EVENT_BUFFER_PART0];
	m_PosNotify[1].dwOffset		= m_dwBufferSize / 2 - 1;
	m_PosNotify[1].hEventNotify	= m_hEvents[KVAVESOUND3D_EVENT_BUFFER_PART1];

	// query interface
	::EnterCriticalSection(&m_SectionSoundBuffer);
	if (m_pBuffer)
		hres = m_pBuffer->QueryInterface(IID_IDirectSoundNotify, (void**)&m_pSoundNotify);
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

void KWavSound3D::FreeSoundNotify()
{
	if (g_pDirectSound->GetDirectSound() == NULL)
		return;
	SAFE_RELEASE(m_pSoundNotify);
}

BOOL KWavSound3D::InitEventHandle()
{
	for (int i = 0; i < KVAVESOUND3D_NUMEVENTS; i++)
	{
		if (m_hEvents[i] == NULL)
			m_hEvents[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
		if (m_hEvents[i] == NULL)
		{
			KGLogPrintf(KGLOG_ERR, "%s", "SoundShell KWavSound3D : Create event failed");
			return FALSE;
		}
	}
	return TRUE;
}

void KWavSound3D::FreeEventHandle()
{
	for (int i = 0; i < KVAVESOUND3D_NUMEVENTS; i++)
	{
		if (m_hEvents[i])
		{
			CloseHandle(m_hEvents[i]);
			m_hEvents[i] = NULL;
		}
	}
}


//---------------------------------------------------------------------------
// 函数:	IsPlaying
// 功能:	是否在播放
// 参数:	void
// 返回:	void
//---------------------------------------------------------------------------
BOOL KWavSound3D::IsPlaying()
{
	HRESULT hr = E_FAIL;
	DWORD	Status = 0;

    if(m_Loading.bLoading)
        return TRUE;

	KG_PROCESS_ERROR(m_pBuffer);

	hr = m_pBuffer->GetStatus(&Status);
	KG_COM_PROCESS_ERROR(hr);

	if (Status & DSBSTATUS_PLAYING)
		return TRUE;

Exit0:
	return FALSE;
}

BOOL KWavSound3D::IsLoop()
{
	return m_bLoop;
}

