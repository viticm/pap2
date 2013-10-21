//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KWavFile.h
// Date:	2000.08.08
// Code:	Daniel Wang
// Desc:	Header File
//---------------------------------------------------------------------------
#ifndef KWavFile_H
#define KWavFile_H
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#define WAVE_FORMAT_IMA_ADPCM	0x0011
//---------------------------------------------------------------------------
typedef struct {
	DWORD		RiffId;		// "RIFF"
	DWORD		dwSize;		// Size of data to follow
	DWORD		WaveId;		// "WAVE"
	DWORD		fmt_Id;		// "fmt "
	DWORD		wfSize;		// 是后面 WaveFormat 的长度(=16)
} WAVEHEADER;
//---------------------------------------------------------------------------
#ifndef _INC_MMSYSTEM		// <mmsystem.h>
typedef struct {
	WORD		wFormatTag; 
	WORD		nChannels; 
	DWORD		nSamplesPerSec; 
	DWORD		nAvgBytesPerSec; 
	WORD		nBlockAlign; 
	WORD		wBitsPerSample; 
	WORD		cbSize;
} WAVEFORMATEX, *PWAVEFORMATEX;
#endif
//---------------------------------------------------------------------------
class  KWavFile
{
private:
	IFile*			m_pFile;
	WAVEFORMATEX	m_WaveFormat;
	DWORD			m_nWaveSize;
	DWORD			m_nWaveData;
	PVOID			m_pImaBuffer;
	BYTE*           m_pWaveData;
	DWORD           m_dwLoadPosition;
	DWORD           m_dwReadPosition;

public:
	KWavFile();
	~KWavFile();
	BOOL			Open(LPSTR FileName);
	DWORD			Read(PVOID lpBuf, DWORD dwLen);
	void			Rewind();
	void			Close();
	BYTE			GetSilenceByte();
	void			GetPcmWavFormat(PWAVEFORMATEX pPcmWavFormat);
	DWORD			GetWaveSize(){ return m_nWaveSize; }
	WORD			GetFormatTag(){ return m_WaveFormat.wFormatTag; }
	WORD			GetBlockAlign(){ return m_WaveFormat.nBlockAlign; }
};
//---------------------------------------------------------------------------
#endif
