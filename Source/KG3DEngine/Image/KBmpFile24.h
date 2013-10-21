//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KBmpFile24.h
// Date:	2000.08.08
// Code:	Daniel Wang
// Desc:	Header File
//---------------------------------------------------------------------------
#ifndef KBmpFile24_H
#define KBmpFile24_H
//---------------------------------------------------------------------------
#include "KBitmap16.h"
//---------------------------------------------------------------------------
class KBmpFile24
{
private:
	long		m_nWidth;
	long		m_nHeight;
	long		m_nBytesPerLine;

	BOOL		Decode(PVOID pBuffer, PVOID pBitmap);
	BOOL		Encode(PVOID pBuffer, PVOID pBitmap);
	static BOOL SaveBuffer(LPSTR lpFileName, PVOID pBitmap, int nPitch, int nWidth, int nHeight, bool bIs24Bit);

public:
	BOOL		Load(LPSTR lpFileName, KBitmap16* lpBitmap);
	BOOL		Save(LPSTR lpFileName, KBitmap16* lpBitmap);
	static BOOL SaveBuffer24(LPSTR lpFileName, PVOID pBitmap, int nPitch, int nWidth, int nHeight);
	static BOOL SaveBuffer32(LPSTR lpFileName, PVOID pBitmap, int nPitch, int nWidth, int nHeight);
};
//---------------------------------------------------------------------------
#endif
