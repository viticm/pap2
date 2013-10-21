//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KMusic3D.h
// Date:	2000.08.08
// Code:	Daniel Wang
// Desc:	Header File
//---------------------------------------------------------------------------
#ifndef KMusic3D_H
#define KMusic3D_H

//---------------------------------------------------------------------------
#include "KWavFile.h"
#include "KMusic.h"
#include <dsound.h>
//---------------------------------------------------------------------------

class  KMusic3D : public KMusic
{
public:
	KMusic3D();
	virtual ~KMusic3D();

	HRESULT					SetPosition(D3DVECTOR* pvPosition, D3DVECTOR* pvVelocity);
	HRESULT					SetParameters(FLOAT fMinDistance, FLOAT fMaxDistance);

	void					Enable3DSound(BOOL bEnbale);

protected:
	virtual BOOL			InitSoundBuffer();
	virtual void			FreeSoundBuffer();

protected:
	LPDIRECTSOUND3DBUFFER	m_p3DBuffer;
	DS3DBUFFER				m_dsBufferParams;
	BOOL					m_bEnbale3DSound;

};

//---------------------------------------------------------------------------
#endif