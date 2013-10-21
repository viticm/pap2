//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KWavMusic.h
// Date:	2000.08.08
// Code:	Daniel Wang
// Desc:	Header File
//---------------------------------------------------------------------------
#ifndef KWavMusic_H
#define KWavMusic_H
//---------------------------------------------------------------------------
#include "KMusic.h"
#include "KWavFile.h"
//---------------------------------------------------------------------------
class  KWavMusic : public KMusic
{
private:
	KWavFile		m_WavFile;
public:
	BOOL			Open(LPSTR FileName);
	void			Rewind();
protected:
	void			InitSoundFormat();
	DWORD			ReadWaveData(LPBYTE lpBuf, DWORD dwLen);
};
//---------------------------------------------------------------------------
#endif