////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KRecordSound.cpp
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2008-1-21 16:46:54
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "KDSound.h"
#include "KRecordSound.h"


////////////////////////////////////////////////////////////////////////////////

KRecordSound::KRecordSound()
{
	for (int i = 0; i < KRECORDSOUND_NUMEVENTS; ++i)
		m_hEvents[i] = NULL;
	m_pDSCapture = NULL;
	m_pDSCaptureBuff = NULL;
	m_pDSNotify = NULL;
	m_dwBufferSize = 0;
	m_dwNBotifyBufferSize = 0;
	m_nSkip = 2;
	::InitializeCriticalSection(&m_SectionBuffer);
	memset(&m_WaveFormatEx, 0, sizeof(m_WaveFormatEx));
}

KRecordSound::~KRecordSound()
{
	StopRecordSound();
    ::DeleteCriticalSection(&m_SectionBuffer);
}

HRESULT KRecordSound::StartRecordSound()
{
	int nRetCode = false;
	HRESULT hRetCode = E_FAIL;
	DSCBUFFERDESC dsBufferDesc;
	DSBPOSITIONNOTIFY PosNotify[2];

	KG_PROCESS_SUCCESS(m_pDSCapture);

	for (int i = 0; i < KRECORDSOUND_NUMEVENTS; ++i)
	{
		m_hEvents[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
		KGLOG_PROCESS_ERROR(m_hEvents[i]);
	}

	hRetCode = DirectSoundCaptureCreate(NULL, &m_pDSCapture, NULL);
	KGLOG_COM_PROCESS_ERROR(hRetCode);

	memset(&dsBufferDesc, 0, sizeof(dsBufferDesc)); 
	dsBufferDesc.dwSize = sizeof(DSCBUFFERDESC);
	dsBufferDesc.dwFlags = 0;
	dsBufferDesc.dwBufferBytes = g_pDirectSound->GetWaveFormatEx()->nAvgBytesPerSec;
	dsBufferDesc.dwReserved = 0;
	dsBufferDesc.lpwfxFormat = g_pDirectSound->GetWaveFormatEx();
	dsBufferDesc.dwFXCount = 0;
	dsBufferDesc.lpDSCFXDesc = NULL;

	hRetCode = m_pDSCapture->CreateCaptureBuffer(&dsBufferDesc, &m_pDSCaptureBuff, NULL);
	KGLOG_COM_PROCESS_ERROR(hRetCode);

	hRetCode = m_pDSCaptureBuff->QueryInterface(IID_IDirectSoundNotify, (VOID**)&m_pDSNotify);
	KGLOG_COM_PROCESS_ERROR(hRetCode);

	m_dwNBotifyBufferSize = dsBufferDesc.dwBufferBytes / 2;

	PosNotify[0].dwOffset = 0;
	PosNotify[0].hEventNotify = m_hEvents[KRECORDSOUND_EVENT_BUFFER_PART0];
	PosNotify[1].dwOffset = m_dwNBotifyBufferSize;
	PosNotify[1].hEventNotify = m_hEvents[KRECORDSOUND_EVENT_BUFFER_PART1];
	hRetCode = m_pDSNotify->SetNotificationPositions(2, PosNotify);
	KGLOG_COM_PROCESS_ERROR(hRetCode);

	m_nSkip = 1;

	hRetCode = m_pDSCaptureBuff->Start(DSCBSTART_LOOPING);
	KGLOG_COM_PROCESS_ERROR(hRetCode);

	nRetCode = m_Thread.Create(ThreadFunction, this);
	KGLOG_PROCESS_ERROR(nRetCode);

Exit1:
	return S_OK;
Exit0:
	StopRecordSound();
	return E_FAIL;
}

HRESULT KRecordSound::StopRecordSound()
{
	if (m_pDSCaptureBuff)
		m_pDSCaptureBuff->Stop();
    if (m_hEvents[KRECORDSOUND_EVENT_EXIT])
	    SetEvent(m_hEvents[KRECORDSOUND_EVENT_EXIT]);

	m_Thread.Destroy();

	KG_COM_RELEASE(m_pDSNotify);
	KG_COM_RELEASE(m_pDSCaptureBuff);
	KG_COM_RELEASE(m_pDSCapture);
	for (int i = 0; i < KRECORDSOUND_NUMEVENTS; ++i)
	{
		CloseHandle(m_hEvents[i]);
		m_hEvents[i] = NULL;
	}
	return S_OK;
}

void KRecordSound::ThreadFunction(void* lpParam)
{
	KRecordSound* pRecordSound = (KRecordSound*)lpParam;
	while (true)
	{
		BOOL bExit = FALSE;
		DWORD dwEvent = WaitForMultipleObjects(KRECORDSOUND_NUMEVENTS, pRecordSound->m_hEvents, FALSE, INFINITE);
		switch (dwEvent)
		{
		case WAIT_FAILED:
			break;
		case WAIT_OBJECT_0 + KRECORDSOUND_EVENT_BUFFER_PART0:
			if (pRecordSound->m_nSkip > 0)
			{
				pRecordSound->m_nSkip--;
				break;
			}
			pRecordSound->RecordSound(0);
			break;
		case WAIT_OBJECT_0 + KRECORDSOUND_EVENT_BUFFER_PART1:
			if (pRecordSound->m_nSkip > 0)
			{
				pRecordSound->m_nSkip--;
				break;
			}
			pRecordSound->RecordSound(1);
			break;
		case WAIT_OBJECT_0 + KRECORDSOUND_EVENT_EXIT:
			bExit = TRUE;
			break;
		default:
			break;
		}

		if (bExit)
			break;
	}
}

HRESULT KRecordSound::RecordSound(DWORD dwPart)
{
	HRESULT hRetCode = E_FAIL;
	void *pLockBuffer = NULL;
	DWORD dwLockSize = m_dwNBotifyBufferSize;
	KG_PROCESS_ERROR(dwPart == 0 || dwPart == 1);
	KG_PROCESS_ERROR(m_pDSCaptureBuff);

	hRetCode = m_pDSCaptureBuff->Lock((1 - dwPart) * m_dwNBotifyBufferSize, m_dwNBotifyBufferSize, &pLockBuffer, &dwLockSize, NULL, NULL, 0);
	if (SUCCEEDED(hRetCode))
	{
		::EnterCriticalSection(&m_SectionBuffer);
		if (m_dwBufferSize + dwLockSize < RECORDSOUND_BUFFERR_SIZE)
		{
			memcpy(m_szBuffer + m_dwBufferSize, pLockBuffer, dwLockSize);
			m_dwBufferSize += dwLockSize;
		}
		::LeaveCriticalSection(&m_SectionBuffer);
		m_pDSCaptureBuff->Unlock(pLockBuffer, dwLockSize, NULL, NULL);
	}

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KRecordSound::LockRecordSoundBuffer(void **ppBuffer, DWORD *pdwSize)
{
	::EnterCriticalSection(&m_SectionBuffer);

	*ppBuffer = m_szBuffer;
	*pdwSize = m_dwBufferSize;

	return S_OK;
}

HRESULT KRecordSound::ClearRecordSoundBuffer()
{
	m_dwBufferSize = 0;
	return S_OK;
}

HRESULT KRecordSound::UnlockRecordSoundBuffer()
{
	::LeaveCriticalSection(&m_SectionBuffer);
	return S_OK;
}

WAVEFORMATEX *KRecordSound::GetWaveFormatEx()
{
	if (m_pDSCaptureBuff)
	{
		m_pDSCaptureBuff->GetFormat(&m_WaveFormatEx, sizeof(m_WaveFormatEx), NULL);
		return &m_WaveFormatEx;
	}
	return g_pDirectSound->GetWaveFormatEx();
}


