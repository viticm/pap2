//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KDSound.h
// Date:	2000.08.08
// Code:	WangWei(Daphnis)
// Desc:	Header File
//---------------------------------------------------------------------------
#ifndef KDSound_H
#define KDSound_H
//---------------------------------------------------------------------------

class  KDirectSound
{
public:
	KDirectSound();
	~KDirectSound();
	HRESULT					Init(HWND hWnd);
	HRESULT					Exit();
	LPDIRECTSOUND8			GetDirectSound(){return m_pDirectSound;}

	HRESULT					SetListenerPosition(D3DVECTOR* pvPosition, D3DVECTOR* pvVelocity);
	HRESULT					GetListenerPosition(D3DVECTOR* pvPosition, D3DVECTOR* pvVelocity);
	HRESULT					SetOrientation(D3DVECTOR* pvFront, D3DVECTOR* pvTop);
	HRESULT					SetListenerParameters(FLOAT fDopplerFactor, FLOAT fRolloffFactor);
	HRESULT                 CommitDeferredSettings();
	WAVEFORMATEX			*GetWaveFormatEx() {return &m_Wfx; };
private:
	HRESULT					CreateDirectSound(HWND hWnd);
	HRESULT					CreatePrimarySoundBuffer();

public:
    HWND                    m_hWnd;

private:
	LPDIRECTSOUND8			m_pDirectSound;
	LPDIRECTSOUNDBUFFER		m_pPrimarySoundBuffer;
	LPDIRECTSOUND3DLISTENER m_pDSListener;
	DS3DLISTENER			m_dsListenerParams;
	WAVEFORMATEX			m_Wfx;
};

extern  KDirectSound* g_pDirectSound;
//---------------------------------------------------------------------------
#endif