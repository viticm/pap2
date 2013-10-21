//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KMpgMusic.h
// Date:	2000.08.08
// Code:	Daniel Wang
// Desc:	Header File
//---------------------------------------------------------------------------
#ifndef KMpgMusic_H
#define KMpgMusic_H
//---------------------------------------------------------------------------
#include "KMusic.h"

//---------------------------------------------------------------------------
class  KMpgMusic : public KMusic
{
private:
	IFile   *m_pMp3File;
    PVOID   m_pMp3Struct;
	PBYTE	mp3_buffer;
	PBYTE	mp3_bufptr;
	long	mp3_bufbytes;
	BOOL	Mp3Init();
	BOOL	Mp3FillBuffer();
	DWORD	Mp3Decode(PBYTE lpPcmBuf, DWORD dwBufLen);
public:
	virtual BOOL	Open(LPSTR FileName);
	virtual void	Seek(int nPercent);
	virtual void	Rewind();
protected:
	virtual BOOL	Mp3FileOpen(LPSTR FileName);
	virtual DWORD	Mp3FileRead(PBYTE pBuf, DWORD dwLen);
	virtual DWORD	Mp3FileSeek(LONG lOffset, int nPosition = SEEK_SET);
	//---07.8.29---Add by Vonbo-------
	//移动到真正的数据头(如果开始有ID3标签，则跳过)
	virtual void Mp3FileSeekDataBegin();
	//------------------end of 07.8.29
	virtual void	InitSoundFormat();
	virtual DWORD	ReadWaveData(LPBYTE lpBuf, DWORD dwLen);
	virtual ~KMpgMusic();
};
//---------------------------------------------------------------------------
#endif