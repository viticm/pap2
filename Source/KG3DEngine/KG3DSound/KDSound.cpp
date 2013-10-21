//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KDSound.cpp
// Date:	2000.08.08
// Code:	WangWei(Daphnis)
// Desc:	Direct Sound Related Functions
//---------------------------------------------------------------------------
#include "stdafx.h" 
#include "KDSound.h"
#include <dsound.h>
DWORD dwApplyMode = DS3D_DEFERRED;
//---------------------------------------------------------------------------
KDirectSound* g_pDirectSound = NULL;
//---------------------------------------------------------------------------
// 函数:	KDirectSound
// 功能:	购造函数
// 参数:	void
// 返回:	void
//---------------------------------------------------------------------------
KDirectSound::KDirectSound()
{
	g_pDirectSound			= this;

    m_hWnd                  = NULL;
	m_pDirectSound			= NULL;
	m_pPrimarySoundBuffer	= NULL;
	m_pDSListener			= NULL;

	memset(&m_Wfx, 0, sizeof(m_Wfx));
	m_Wfx.cbSize = sizeof(m_Wfx);
	m_Wfx.wFormatTag = WAVE_FORMAT_PCM;
	m_Wfx.nChannels = 2;
	m_Wfx.nSamplesPerSec = 22050;
	m_Wfx.wBitsPerSample = 16;
	m_Wfx.nBlockAlign = (WORD)(m_Wfx.wBitsPerSample / 8 * m_Wfx.nChannels);
	m_Wfx.nAvgBytesPerSec = m_Wfx.nSamplesPerSec * m_Wfx.nBlockAlign;

}
//---------------------------------------------------------------------------
// 函数:	~KDirectSound
// 功能:	析造函数
// 参数:	void
// 返回:	void
//---------------------------------------------------------------------------
KDirectSound::~KDirectSound()
{
	Exit();
}
//---------------------------------------------------------------------------
// 函数:	Init
// 功能:	初始化DirectSound
// 参数:	void
// 返回:	S_OK－成功 E_FAIL－失败
//---------------------------------------------------------------------------
HRESULT KDirectSound::Init(HWND hWnd)
{
	HRESULT hr = E_FAIL;

	KGLOG_PROCESS_ERROR(m_pDirectSound == NULL);

	hr = CreateDirectSound(hWnd);
	KGLOG_COM_PROCESS_ERROR(hr);

	hr = CreatePrimarySoundBuffer();
	KGLOG_COM_PROCESS_ERROR(hr);

    m_hWnd = hWnd;

	return S_OK;
Exit0:
	Exit();
	return E_FAIL;
}

//---------------------------------------------------------------------------
// 函数:	Exit
// 功能:	释放DirectSound
// 参数:	void
// 返回:	S_OK
//---------------------------------------------------------------------------
HRESULT KDirectSound::Exit()
{
    m_hWnd = NULL;

	KG_COM_RELEASE(m_pDSListener);
	KG_COM_RELEASE(m_pPrimarySoundBuffer);
	KG_COM_RELEASE(m_pDirectSound);

	return S_OK;
}

//---------------------------------------------------------------------------
// 函数:	CreateDirectSound
// 功能:	创建DirectSound
// 参数:	void
// 返回:	TRUE－成功 FALSE－失败
//---------------------------------------------------------------------------
HRESULT KDirectSound::CreateDirectSound(HWND hWnd)
{
	HRESULT	hr = E_FAIL;

	ASSERT(!m_pDirectSound);

	KGLOG_PROCESS_ERROR(hWnd);

	hr = DirectSoundCreate8(NULL, &m_pDirectSound, NULL);
	KGLOG_COM_PROCESS_ERROR(hr);
	ASSERT(m_pDirectSound);
	
	hr = m_pDirectSound->SetCooperativeLevel(hWnd, DSSCL_PRIORITY);
	KGLOG_COM_PROCESS_ERROR(hr);

	return S_OK;
Exit0:
	KG_COM_RELEASE(m_pDirectSound);
	return E_FAIL;
}

//---------------------------------------------------------------------------
// 函数:	CreatePrimarySoundBuffer
// 功能:	创建主声音缓冲
// 参数:	void
// 返回:	TRUE－成功 FALSE－失败
//---------------------------------------------------------------------------
HRESULT KDirectSound::CreatePrimarySoundBuffer()
{
	HRESULT			hr = E_FAIL;
	DSBUFFERDESC	desc;

	ASSERT(m_pDirectSound);
	ASSERT(!m_pPrimarySoundBuffer);
	ASSERT(!m_pDSListener);
	
	// setup primary sound buffer desc
	memset(&desc, 0,sizeof(DSBUFFERDESC));
	desc.dwSize = sizeof(DSBUFFERDESC);
	desc.dwFlags = DSBCAPS_CTRL3D | DSBCAPS_PRIMARYBUFFER;

	// create primary sound buffer
	hr = m_pDirectSound->CreateSoundBuffer(&desc, &m_pPrimarySoundBuffer, NULL);
	KGLOG_COM_PROCESS_ERROR(hr);

	// set primary sound buffer format
	hr = m_pPrimarySoundBuffer->SetFormat(&m_Wfx);
	KGLOG_COM_PROCESS_ERROR(hr);

	//获取3D 模拟世界中的听者指针。
	hr = m_pPrimarySoundBuffer->QueryInterface(IID_IDirectSound3DListener, (VOID**)&m_pDSListener);
	KGLOG_COM_PROCESS_ERROR(hr);

	m_dsListenerParams.dwSize = sizeof(DS3DBUFFER);
	hr = m_pDSListener->GetAllParameters(&m_dsListenerParams);
	KGLOG_COM_PROCESS_ERROR(hr);

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT	KDirectSound::SetListenerPosition(D3DVECTOR* pvPosition, D3DVECTOR* pvVelocity)
{
	if (pvPosition)
		memcpy(&m_dsListenerParams.vPosition, pvPosition, sizeof(D3DVECTOR));
	if (pvVelocity)
		memcpy(&m_dsListenerParams.vVelocity, pvVelocity, sizeof(D3DVECTOR));
	if (m_pDSListener)
		m_pDSListener->SetAllParameters(&m_dsListenerParams, dwApplyMode);
	return S_OK;
}

HRESULT	KDirectSound::GetListenerPosition(D3DVECTOR* pvPosition, D3DVECTOR* pvVelocity)
{
	if (pvPosition)
		memcpy(pvPosition, &m_dsListenerParams.vPosition, sizeof(D3DVECTOR));
	if (pvVelocity)
		memcpy(pvVelocity, &m_dsListenerParams.vVelocity, sizeof(D3DVECTOR));
	return S_OK;
}

HRESULT	KDirectSound::SetOrientation(D3DVECTOR* pvFront, D3DVECTOR* pvTop)
{
	if (pvFront)
		memcpy(&m_dsListenerParams.vOrientFront, pvFront, sizeof(D3DVECTOR));
	if (pvTop)
		memcpy(&m_dsListenerParams.vOrientTop, pvTop, sizeof(D3DVECTOR));
	if (m_pDSListener)
		m_pDSListener->SetAllParameters(&m_dsListenerParams, dwApplyMode);
	return S_OK;
}

HRESULT	KDirectSound::SetListenerParameters(FLOAT fDopplerFactor, FLOAT fRolloffFactor)
{
	m_dsListenerParams.flDopplerFactor = fDopplerFactor;
	m_dsListenerParams.flRolloffFactor = fRolloffFactor;
	if (m_pDSListener)
		m_pDSListener->SetAllParameters(&m_dsListenerParams, dwApplyMode);
	return S_OK;
}

HRESULT KDirectSound::CommitDeferredSettings()
{
	if (m_pDSListener)
		return m_pDSListener->CommitDeferredSettings();
	return E_FAIL;
}
