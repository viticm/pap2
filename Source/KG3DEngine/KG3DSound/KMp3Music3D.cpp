//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KMp3Music3D.cpp
// Date:	2000.08.08
// Code:	Daniel Wang
// Desc:	MP3 Stream Music Class
//---------------------------------------------------------------------------
#include "stdafx.h" 
#include "KMp3Music3D.h"

//---------------------------------------------------------------------------
#define MP3_BUFSIZE 60000 // mp3 data buffer

//---------------------------------------------------------------------------
// 函数:	Open
// 功能:	打开音乐文件
// 参数:	FileName	文件名
// 返回:	TRUE－成功	FALSE－失败
//---------------------------------------------------------------------------

KMp3Music3D::KMp3Music3D()
{
	m_mp3buffer=NULL;
	m_mp3bufptr=NULL;
	m_pMp3File=NULL;
	m_mp3bufbytes=0;
	m_mp3frmbytes=0;
	m_szMp3FilePath[0] = '\0';
}

BOOL KMp3Music3D::Open(LPSTR FileName)
{
	if (!FileName || !FileName[0])
		return FALSE;

	if (m_pSoundBuffer || m_mp3buffer || m_pMp3File)
		Close();

	if (!Mp3FileOpen(FileName))
		return FALSE;

	if (!Mp3Init())
		return FALSE;

	if (!Init())
		return FALSE;

	return TRUE;
}

void KMp3Music3D::Close()
{
	KMusic::Close();
	Clear();
}

//---------------------------------------------------------------------------
// 函数:	InitSoundFormat
// 功能:	
// 参数:	void
// 返回:	void
//---------------------------------------------------------------------------
void KMp3Music3D::InitSoundFormat()
{
	// align buffer size with mp3 frame 4608
	m_dwBufferSize = m_WaveFormat.nAvgBytesPerSec;
	m_dwBufferSize = (m_dwBufferSize + 4607) / 4608 * 4608;

	// 2 second sound buffer
	m_dwBufferSize *= 2;
}
//---------------------------------------------------------------------------
// 函数:	ReadWaveData
// 功能:	读取波形数据
// 参数:	lpBuf	缓存
//			dwLen	长度	
// 返回:	DWORD	实际读取的长度
//---------------------------------------------------------------------------
DWORD KMp3Music3D::ReadWaveData(LPBYTE lpBuf, DWORD dwLen)
{
	return Mp3Decode(lpBuf, dwLen);
}
//---------------------------------------------------------------------------
// 函数:	Seek
// 功能:	
// 参数:	void
// 返回:	void
//---------------------------------------------------------------------------
void KMp3Music3D::Seek(int nPercent)
{
}
//---------------------------------------------------------------------------
// 函数:	Rewind
// 功能:	从头播放音乐
// 参数:	void
// 返回:	void
//---------------------------------------------------------------------------
void KMp3Music3D::Rewind()
{
	Mp3FileSeekDataBegin();
	m_mp3bufbytes = 0;
}
//---------------------------------------------------------------------------
// 函数:	Mp3FileOpen
// 功能:	
// 参数:	FileName
// 返回:	BOOL
//---------------------------------------------------------------------------
BOOL KMp3Music3D::Mp3FileOpen(LPSTR szFileName)
{
    ASSERT(m_pMp3File == NULL);

	m_pMp3File = g_OpenFile(szFileName, false, false);

	if (m_pMp3File==NULL)
	{
		m_szMp3FilePath[0] = '\0';

        KGLogPrintf(KGLOG_ERR, "SoundShell\tKMp3Music3D::Mp3FileOpen(%s)\n", szFileName);
		return FALSE;
	}
	else
	{
		strcpy(m_szMp3FilePath, szFileName);
		return TRUE;
	}
}
//---------------------------------------------------------------------------
// 函数:	Mp3FileRead
// 功能:	
// 参数:	pBuf
//			dwlen
// 返回:	void
//---------------------------------------------------------------------------
DWORD KMp3Music3D::Mp3FileRead(PBYTE pBuf, DWORD dwLen)
{
	return m_pMp3File->Read(pBuf, dwLen);
}
//---------------------------------------------------------------------------
// 函数:	Mp3FileSeek
// 功能:	
// 参数:	lOffset
// 返回:	void
//---------------------------------------------------------------------------
DWORD KMp3Music3D::Mp3FileSeek(LONG lOffset,  int nPosition /*= SEEK_SET*/ )
{
	return m_pMp3File->Seek(lOffset, nPosition);
}

//---------------------------------------------------------------------------
// 函数:	Mp3FileSeekDataBegin()
// 功能:	将Mp3文件的数据定位到音频开始处(跳过开始的ID3标签)
// 参数:	lOffset
// 返回:	void
//---------------------------------------------------------------------------
void KMp3Music3D::Mp3FileSeekDataBegin()
{	
	BYTE byteBegin = 0;
	Mp3FileRead(&byteBegin, 1);	// Read the First Byte
	Mp3FileSeek(0);					// Set to beginning
	if (byteBegin != 0xFF)			// ID3v2 Tag
	{		
		BYTE bySize[4] = {0};
		LONG longTagSize = 0;			
		Mp3FileSeek(6);
		Mp3FileRead((PBYTE)bySize, 4);	// Read the tag header
		longTagSize = (bySize[0]&0x7F)<<21 | (bySize[1]&0x7F)<<14 
							| (bySize[2]&0x7F)<<7 | (bySize[3]&0x7F);
		Mp3FileSeek(longTagSize, SEEK_CUR);
	}
}

//---------------------------------------------------------------------------
// 函数:	Mp3Init
// 功能:	初始化MP3文件
// 参数:	void
// 返回:	void
//---------------------------------------------------------------------------
BOOL KMp3Music3D::Mp3Init()
{
	MPEG_HEAD mpeg_head;
	DEC_INFO  dec_info;

	// load mp3lib.dll
	//if (!LoadMp3Lib())
	//	return FALSE;

	// reduction_code:	sample rate reduction code
	//		0 = full rate
	//		1 = half rate
	//		2 = quarter rate
	int reduction_code = 0;

	// transform_code:	ignored
	int transform_code = 0;

	// convert_code:	channel conversion
	//		0 = two chan output
	//		1 = convert two chan to mono
	//		2 = convert two chan to left chan
	//		3 = convert two chan to right chan
	int convert_code = 0;

	// freq_limit:	limits bandwidth of pcm output to specified
	//		frequency. Special use. Set to 24000 for normal use.
	int freq_limit = 24000;

	// alloc mp3 stream buffer
	//	if (!m_Mp3Buffer.Alloc(MP3_BUFSIZE))
	//		return FALSE;
	//	m_mp3buffer = (PBYTE)m_Mp3Buffer.GetMemPtr();

	if (m_mp3buffer)
	{
		free(m_mp3buffer);
		m_mp3buffer=NULL;
	}
	m_mp3buffer=(PBYTE) malloc(MP3_BUFSIZE*sizeof(BYTE));
	if (m_mp3buffer==NULL)
	{
		KGLogPrintf(KGLOG_ERR, "%s", "SoundShell\tKMp3Music3D::m_mp3buffer Fail To Alloc Memory");
		return FALSE;
	}

	// init mp3 buffer
	m_mp3bufptr = m_mp3buffer;

	// seek to begin of mp3 file
	Mp3FileSeekDataBegin();

	// fill mp3 buffer
	if (!Mp3FillBuffer())
	{
		free(m_mp3buffer);
		m_mp3buffer=NULL;
		return FALSE;
	}

	// parse mpeg header
	m_mp3frmbytes = m_Decoder.DecodeHead(m_mp3buffer, &mpeg_head);
	if (m_mp3frmbytes == 0)
	{
		KGLogPrintf(KGLOG_ERR, "%s : %s", "SoundShell\tKMp3Music3D::mp3 decode head fail", m_szMp3FilePath);
		free(m_mp3buffer);
		m_mp3buffer=NULL;
		return FALSE;
	}

	// init mp3 decoder
	if (!m_Decoder.Init(&mpeg_head, m_mp3frmbytes,
		reduction_code, transform_code, convert_code, freq_limit))
	{
		KGLogPrintf(KGLOG_ERR, "%s : %s", "SoundShell\tKMp3Music3D::mp3 decode init fail", m_szMp3FilePath);
		free(m_mp3buffer);
		m_mp3buffer=NULL;
		return FALSE;
	}

	// get mp3 info
	m_Decoder.DecodeInfo(&dec_info);

	// set pcm wave format
	m_WaveFormat.wFormatTag		 = WAVE_FORMAT_PCM;
	m_WaveFormat.nChannels		 = dec_info.channels;
	m_WaveFormat.nSamplesPerSec	 = dec_info.samprate;
	m_WaveFormat.nAvgBytesPerSec = (dec_info.channels * dec_info.samprate * dec_info.bits + 7) / 8;
	m_WaveFormat.nBlockAlign	 = (dec_info.channels * dec_info.bits + 7) / 8;
	m_WaveFormat.wBitsPerSample	 = dec_info.bits;
	m_WaveFormat.cbSize			 = 0;

	return TRUE;
}
//---------------------------------------------------------------------------
// 函数:	Mp3Decode
// 功能:	解码MP3
// 参数:	lpPcmBuf		PCM buffer pointer
//			dwBufLen		buffer size in bytes
// 返回:	void
//---------------------------------------------------------------------------
DWORD KMp3Music3D::Mp3Decode(PBYTE lpPcmBuf, DWORD dwBufLen)
{
	PBYTE	pcm_buffer = lpPcmBuf;
	int		pcm_trigger = dwBufLen;
	int		pcm_bufbytes = 0;
	int		mp3_size = 0;
	int		pcm_size = 0;
	IN_OUT	res = {0, 0};

	while (TRUE)
	{
		// fill mp3 buffer
		if (!Mp3FillBuffer())
			break;

		// end of mp3 file
		if (m_mp3bufbytes < m_mp3frmbytes)
			break;

		// decode one frame to pcm buffer
		res = m_Decoder.DecodeFrame(m_mp3bufptr, pcm_buffer);
		if (res.in_bytes <= 0)
		{
			g_DebugLog("KMp3Music3D: bad sync in mp3 file");
			break;
		}

		// update buffer pointer and buffer size
		m_mp3bufptr += res.in_bytes;
		m_mp3bufbytes -= res.in_bytes;
		pcm_buffer += res.out_bytes;
		pcm_bufbytes += res.out_bytes;

		// is pcm buffer full
		if (pcm_bufbytes >= pcm_trigger)
			break;
	}
	return pcm_bufbytes;
}
//---------------------------------------------------------------------------
// 函数:	Mp3FillBuffer
// 功能:	
// 参数:	void
// 返回:	void
//---------------------------------------------------------------------------
BOOL KMp3Music3D::Mp3FillBuffer()
{
	static int mp3_trigger = 2500;
	DWORD dwRead;

	// signed var could be negative
	if (m_mp3bufbytes < 0)
		m_mp3bufbytes = 0;

	// read mp3 data to buffer
	if (m_mp3bufbytes < mp3_trigger)
	{
		memmove(m_mp3buffer, m_mp3bufptr, m_mp3bufbytes);
		dwRead = Mp3FileRead(m_mp3buffer + m_mp3bufbytes, 
			MP3_BUFSIZE - m_mp3bufbytes);
		m_mp3bufbytes += dwRead;
		m_mp3bufptr = m_mp3buffer;
	}

	return TRUE;
}
//---------------------------------------------------------------------------
KMp3Music3D::~KMp3Music3D()
{
	Close();
}

void KMp3Music3D::Clear()
{
	if (m_mp3buffer!=NULL)
	{
		free(m_mp3buffer);
		m_mp3buffer=NULL;
	}
	m_mp3bufptr = NULL;
	m_mp3bufbytes = 0;
	m_mp3frmbytes = 0;
	if (m_pMp3File)
	{
		m_pMp3File->Release();
		m_pMp3File = NULL;
	}
	m_szMp3FilePath[0] = '\0';
}

void KMp3Music3D::Stop()
{
	KMusic::Stop();
	m_mp3bufbytes=0;
	m_mp3frmbytes=0;
}