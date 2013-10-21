//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KMp3Music3D.h
// Date:	2000.08.08
// Code:	Daniel Wang
// Desc:	Header File
//---------------------------------------------------------------------------
#ifndef KMp3Music3D_H
#define KMp3Music3D_H
//---------------------------------------------------------------------------
#include "KMp3LibClass.h"
#include "KMusic3D.h" 
//---------------------------------------------------------------------------
class  KMp3Music3D : public KMusic3D
{
public:
	KMp3Music3D();
	virtual ~KMp3Music3D();

	virtual BOOL Open(LPSTR FileName);
	virtual void Close();
	virtual void Seek(int nPercent);
	virtual void Rewind();
	void         Clear();
	virtual void Stop();

protected:
	virtual BOOL  Mp3Init();
	virtual BOOL  Mp3FillBuffer();
	virtual DWORD Mp3Decode(PBYTE lpPcmBuf, DWORD dwBufLen);
	virtual BOOL  Mp3FileOpen(LPSTR FileName);
	virtual DWORD Mp3FileRead(PBYTE pBuf, DWORD dwLen);
	virtual DWORD Mp3FileSeek(LONG lOffset, int nPosition = SEEK_SET);
	//---07.8.29---Add by Vonbo-------
	//移动到真正的数据头(如果开始有ID3标签，则跳过)
	virtual void Mp3FileSeekDataBegin();
	//------------------end of 07.8.29
	virtual void  InitSoundFormat();
	virtual DWORD ReadWaveData(LPBYTE lpBuf, DWORD dwLen);

protected:
	PBYTE	m_mp3buffer;
	PBYTE	m_mp3bufptr;
	int		m_mp3bufbytes;
	int		m_mp3frmbytes;

private:
	IFile*	m_pMp3File;
	char	m_szMp3FilePath[MAX_PATH];

	KMp3 m_Decoder;
};
//---------------------------------------------------------------------------
#endif