//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KWavMusic.cpp
// Date:	2000.08.08
// Code:	Daniel Wang
// Desc:	Wave Stream Music Class
//---------------------------------------------------------------------------
#include "stdafx.h" 
#include "KWavMusic.h"
//---------------------------------------------------------------------------
// 函数:	Open
// 功能:	打开音乐文件
// 参数:	FileName	文件名
// 返回:	TRUE－成功	FALSE－失败
//---------------------------------------------------------------------------
BOOL KWavMusic::Open(LPSTR FileName)
{
	if (m_pSoundBuffer)
		Close();

	if (!m_WavFile.Open(FileName))
		return FALSE;

	return Init();
}
//---------------------------------------------------------------------------
// 函数:	InitSoundFormat
// 功能:	
// 参数:	void
// 返回:	void
//---------------------------------------------------------------------------
void KWavMusic::InitSoundFormat()
{
	// get wave format pointer
	m_WavFile.GetPcmWavFormat(&m_WaveFormat);

	// one second sound buffer
	m_dwBufferSize = m_WaveFormat.nAvgBytesPerSec;

	// size align for IMA-ADPCM format
	if (m_WavFile.GetFormatTag() == WAVE_FORMAT_IMA_ADPCM)
	{
		int nBlockSize = (m_WavFile.GetBlockAlign() - 7) * 4;
		m_dwBufferSize = (m_dwBufferSize + nBlockSize - 1)
			/ nBlockSize * nBlockSize;
	}

	// two second sound buffer
	m_dwBufferSize *= 2;
}
//---------------------------------------------------------------------------
// 函数:	ReadWaveData
// 功能:	读取波形数据
// 参数:	lpBuf	缓存
//			dwLen	长度	
// 返回:	DWORD	实际读取的长度
//---------------------------------------------------------------------------
DWORD KWavMusic::ReadWaveData(LPBYTE lpBuf, DWORD dwLen)
{
	return m_WavFile.Read(lpBuf, dwLen);
}
//---------------------------------------------------------------------------
// 函数:	Rewind
// 功能:	从头播放音乐
// 参数:	void
// 返回:	void
//---------------------------------------------------------------------------
void KWavMusic::Rewind()
{
	m_WavFile.Rewind();
}
//---------------------------------------------------------------------------
