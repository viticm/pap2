//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2004 by Kingsoft
//
// File:	KBmpFile24.cpp
// Date:	2000.08.08
// Code:	Daniel Wang(CHEN MIN 修改)
// Desc:	24bit BMP file class
//---------------------------------------------------------------------------
/*
#include "KWin32.h"
#include "KDebug.h"
#include "KMemClass.h"
#include "KPakFile.h"
*/

#include "Stdafx.h"
#include "./KColors.h"
#include "./KBmpFile24.h"

//---------------------------------------------------------------------------
// 函数:	Load
// 功能:	打开BMP文科
// 参数:	lpFileName	文件名
// 返回:	TRUE		成功
//			FALSE		失败
//---------------------------------------------------------------------------
BOOL KBmpFile24::Load(LPSTR lpFileName, KBitmap16* lpBitmap)
{
	BITMAPFILEHEADER	FileHeader;
	BITMAPINFOHEADER	InfoHeader;
	KSmartFile			pFile;
	//KMemClass			Buffer;
	void*				Buffer;

	// open the file
	if ((pFile = g_OpenFile(lpFileName)) == NULL)
		return FALSE;

	// read file head
	pFile->Read(&FileHeader, sizeof(FileHeader));

	// check BMP flag "BM"
	if (FileHeader.bfType != 0x4d42)
		return FALSE;

	// read info head
	pFile->Read(&InfoHeader, sizeof(InfoHeader));

	// check compression
	if (InfoHeader.biCompression != 0)
		return FALSE;

	// check bit count
	if (InfoHeader.biBitCount != 24)
		return FALSE;

	// get width and height 
	m_nWidth  = InfoHeader.biWidth;
	m_nHeight = InfoHeader.biHeight;

	// byte per line % 4 must = 0
	m_nBytesPerLine = m_nWidth * 3;
	if ((m_nBytesPerLine % 4) != 0)
		m_nBytesPerLine = m_nBytesPerLine + 4 - (m_nBytesPerLine % 4);

	// alloc buffer
	/*
	if (!Buffer.Alloc(m_nHeight * m_nBytesPerLine))
		return FALSE;
	*/

	Buffer = malloc(m_nHeight * m_nBytesPerLine);
	if(Buffer == NULL)
		return FALSE;
 
	// int bitmap with width & height
	if (!lpBitmap->Init(m_nWidth, m_nHeight))
		return FALSE;

	// set file pointer to data begin
	pFile->Seek(FileHeader.bfOffBits, FILE_BEGIN);

	// read bitmap bits data
	pFile->Read(Buffer, m_nBytesPerLine * m_nHeight);

	// decode bitmap
	Decode(Buffer, lpBitmap->GetBitmap());
	free(Buffer);
	return TRUE;
}
//---------------------------------------------------------------------------
// 函数:	Save
// 功能:	保存BMP文件
// 参数:	lpFileName	文件名
// 返回:	TRUE		成功
//			FALSE		失败
//---------------------------------------------------------------------------
BOOL KBmpFile24::Save(LPSTR lpFileName, KBitmap16* lpBitmap)
{
	BITMAPFILEHEADER	FileHeader;
	BITMAPINFOHEADER	InfoHeader;
	KSmartFile			pFile;
	DWORD				OffBits;
	void*				Buffer;

	// get width and height 
	m_nWidth  = lpBitmap->GetWidth();
	m_nHeight = lpBitmap->GetHeight();

	// byte per line % 4 must = 0
	m_nBytesPerLine = m_nWidth * 3;
	if ((m_nBytesPerLine % 4) != 0)
		m_nBytesPerLine = m_nBytesPerLine + 4 - (m_nBytesPerLine % 4);

	// alloc buffer
	Buffer = malloc(m_nHeight * m_nBytesPerLine);
	if(Buffer == NULL)
		return FALSE;

	// offset in bytes
	OffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	// file header
	FileHeader.bfType          = 0x4d42; // "BM"
	FileHeader.bfSize          = m_nBytesPerLine * m_nHeight + OffBits;
	FileHeader.bfReserved1     = 0;
	FileHeader.bfReserved2     = 0;
	FileHeader.bfOffBits       = OffBits;

	// info header
	InfoHeader.biSize          = sizeof(BITMAPINFOHEADER);
	InfoHeader.biWidth         = m_nWidth;
	InfoHeader.biHeight        = m_nHeight;
	InfoHeader.biPlanes        = 1;
	InfoHeader.biBitCount      = 24;
	InfoHeader.biCompression   = 0;
	InfoHeader.biSizeImage     = 0;
	InfoHeader.biXPelsPerMeter = 0xb40;
	InfoHeader.biYPelsPerMeter = 0xb40;
	InfoHeader.biClrUsed       = 0;
	InfoHeader.biClrImportant  = 0;

	// crete the file
	if ((pFile = g_OpenFile(lpFileName)) == NULL)
		return FALSE;

	// write file head
	pFile->Write(&FileHeader, sizeof(FileHeader));

	// write info head
	pFile->Write(&InfoHeader, sizeof(InfoHeader));

	// encode bitmap
	Encode(Buffer, lpBitmap->GetBitmap());

	// write bitmap bits data
	pFile->Write(Buffer, m_nBytesPerLine * m_nHeight);

	free(Buffer);
	return TRUE;
}
//---------------------------------------------------------------------------
// 函数:	Decode
// 功能:	解码
// 参数:	pBuffer		24位色位图
//			pBitmap		16位色位图
// 返回:	TRUE		成功
//			FALSE		失败
//---------------------------------------------------------------------------
BOOL KBmpFile24::Decode(PVOID pBuffer, PVOID pBitmap)
{
	LPWORD lpDes = (LPWORD)pBitmap;
	LPBYTE lpSrc = (LPBYTE)pBuffer;

	if (lpDes == NULL || lpSrc == NULL)
		return FALSE;

	lpSrc += (m_nHeight - 1) * m_nBytesPerLine;
	for (int i = 0; i < m_nHeight; i++)
	{
		for (int j = 0; j < m_nWidth; j++)
		{
			lpDes[j] = g_RGB(lpSrc[2], lpSrc[1], lpSrc[0]);
			lpSrc += 3;
		}
		lpSrc -= 3 * m_nWidth;
		lpSrc -= m_nBytesPerLine;
		lpDes += m_nWidth;
	}
	return TRUE;
}
//---------------------------------------------------------------------------
// 函数:	Encode
// 功能:	编码
// 参数:	pBuffer		24位色位图
//			pBitmap		16位色位图
// 返回:	TRUE		成功
//			FALSE		失败
//---------------------------------------------------------------------------
BOOL KBmpFile24::Encode(PVOID pBuffer, PVOID pBitmap)
{
	LPBYTE lpDes = (LPBYTE)pBuffer;
	LPWORD lpSrc = (LPWORD)pBitmap;

	if (lpDes == NULL || lpSrc == NULL)
		return FALSE;

	lpDes += (m_nHeight - 1) * m_nBytesPerLine;
	for (int i = 0; i < m_nHeight; i++)
	{
		for (int j = 0; j < m_nWidth; j++)
		{
			lpDes[0] = g_Blue(lpSrc[j]);
			lpDes[1] = g_Blue(lpSrc[j]);
			lpDes[2] = g_Blue(lpSrc[j]);
			lpDes += 3;
		}
		lpDes -= 3 * m_nWidth;
		lpDes -= m_nBytesPerLine;
		lpSrc += m_nWidth;
	}
	return TRUE;
}
//---------------------------------------------------------------------------

BOOL KBmpFile24::SaveBuffer24(LPSTR lpFileName, PVOID pBitmap, int nPitch, int nWidth, int nHeight)
{
	return SaveBuffer(lpFileName, pBitmap, nPitch, nWidth, nHeight, true);
}

BOOL KBmpFile24::SaveBuffer32(LPSTR lpFileName, PVOID pBitmap, int nPitch, int nWidth, int nHeight)
{
	return SaveBuffer(lpFileName, pBitmap, nPitch, nWidth, nHeight, false);
}

//---------------------------------------------------------------------------
// 函数:	BOOL SaveBuffer(LPSTR lpFileName, PVOID pBitmap, int nWidth, int nHeight, bool bIs24Bit)
// 功能:	编码
// 参数:	lpFileName	文件名
//			pBitmap		16位色位图
// 返回:	TRUE		成功
//			FALSE		失败
//---------------------------------------------------------------------------
BOOL KBmpFile24::SaveBuffer(LPSTR lpFileName, PVOID pBitmap, int nPitch, int nWidth, int nHeight, bool bIs24Bit)
{
	if(!pBitmap)
		return FALSE;

	BITMAPFILEHEADER	FileHeader;
	BITMAPINFOHEADER	InfoHeader;
	KSmartFile			pFile;
	DWORD				OffBits;
	void*				Buffer;

	// byte per line % 4 must = 0
	int nBytesPerLine = nWidth * 3;
	if ((nBytesPerLine % 4) != 0)
		nBytesPerLine = nBytesPerLine + 4 - (nBytesPerLine % 4);

	// alloc buffer
	if ((Buffer = malloc(nHeight * nBytesPerLine)) == NULL)
		return FALSE;

	// offset in bytes
	OffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	// file header
	FileHeader.bfType          = 0x4d42; // "BM"
	FileHeader.bfSize          = nBytesPerLine * nHeight + OffBits;
	FileHeader.bfReserved1     = 0;
	FileHeader.bfReserved2     = 0;
	FileHeader.bfOffBits       = OffBits;

	// info header
	InfoHeader.biSize          = sizeof(BITMAPINFOHEADER);
	InfoHeader.biWidth         = nWidth;
	InfoHeader.biHeight        = nHeight;
	InfoHeader.biPlanes        = 1;
	InfoHeader.biBitCount      = 24;
	InfoHeader.biCompression   = 0;
	InfoHeader.biSizeImage     = 0;
	InfoHeader.biXPelsPerMeter = 0xb40;
	InfoHeader.biYPelsPerMeter = 0xb40;
	InfoHeader.biClrUsed       = 0;
	InfoHeader.biClrImportant  = 0;

	// crete the file
	if ((pFile = g_CreateFile(lpFileName)) == NULL)
	{
		free(Buffer);
		return FALSE;
	}

	// write file head
	pFile->Write(&FileHeader, sizeof(FileHeader));

	// write info head
	pFile->Write(&InfoHeader, sizeof(InfoHeader));

	// encode bitmap
	LPBYTE lpDes = (LPBYTE)Buffer;
	LPBYTE lpSrc = (LPBYTE)pBitmap;

	int nLineAdd = nPitch - nWidth * 4;
	lpDes += (nHeight - 1) * nBytesPerLine;
	if(bIs24Bit)
	{
		for (int i = 0; i < nHeight; i++)
		{
			for (int j = 0; j < nWidth; j++)
			{
				lpDes[0] = lpSrc[0];
				lpDes[1] = lpSrc[1];
				lpDes[2] = lpSrc[2];
				lpDes += 3;
				lpSrc += 3;
			}
			lpDes -= 3 * nWidth;
			lpDes -= nBytesPerLine;
		}
	}
	else
	{
		for (int i = 0; i < nHeight; i++)
		{
			for (int j = 0; j < nWidth; j++)
			{
				lpDes[0] = lpSrc[0];
				lpDes[1] = lpSrc[1];
				lpDes[2] = lpSrc[2];
				lpDes += 3;
				lpSrc += 4;
			}
			lpDes -= 3 * nWidth;
			lpDes -= nBytesPerLine;
			lpSrc += nLineAdd;
		}
	}

	// write bitmap bits data
	pFile->Write(Buffer, nBytesPerLine * nHeight);
	free(Buffer);
	return TRUE;
}
