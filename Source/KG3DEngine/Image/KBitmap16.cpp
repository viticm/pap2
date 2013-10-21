//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KBitmap16.cpp
// Date:	2000.08.08
// Code:	Daniel Wang
// Desc:	256 Color bitmap file class
//---------------------------------------------------------------------------
/*
#include "KWin32.h"
#include "KDebug.h"
#include "KDDraw.h"
#include "KMemBase.h"
#include "KPakFile.h"
#include "KColors.h"
#include "KCanvas.h"
*/
#include "Stdafx.h"
#include "./KBitmap16.h"
//---------------------------------------------------------------------------
// 函数:	KBitmap16
// 功能:	购造函数
// 参数:	void
// 返回:	void
//---------------------------------------------------------------------------
KBitmap16::KBitmap16()
{
	m_nWidth  = 0;
	m_nHeight = 0;
}
//---------------------------------------------------------------------------
// 函数:	Init
// 功能:	创建新位图
// 参数:	nWidth		宽度
//			nHeight		高度
// 返回:	void
//---------------------------------------------------------------------------
BOOL KBitmap16::Init(int nWidth, int nHeight)
{
	m_nWidth  = nWidth;
	m_nHeight = nHeight;
	return (m_pBitmap = malloc(m_nWidth * m_nHeight * 2)) != NULL;
}
//---------------------------------------------------------------------------
// 函数:	Free
// 功能:	释放位图
// 参数:	void
// 返回:	void
//---------------------------------------------------------------------------
void KBitmap16::Free()
{
	delete m_pBitmap;
}
//---------------------------------------------------------------------------
// 函数:	Clear
// 功能:	位图清零
// 参数:	void
// 返回:	void
//---------------------------------------------------------------------------
void KBitmap16::Clear()
{
	ZeroMemory(m_pBitmap , m_nWidth * m_nHeight * 2);
}
//---------------------------------------------------------------------------
// 函数:	Draw
// 功能:	绘制位图
// 参数:	nX		X坐标
//			nY		Y坐标
// 返回:	void
//---------------------------------------------------------------------------
void KBitmap16::Draw(int nX, int nY)
{
	/*
	LPVOID lpBitmap = m_pBitmap;
	if (lpBitmap)
	{
		//g_DrawBitmap16(nX, nY, m_nWidth, m_nHeight, lpBitmap);
	}
	*/
}
//---------------------------------------------------------------------------
// 函数:	DrawLum
// 功能:	绘制位图
// 参数:	nX		X坐标
//			nY		Y坐标
//			nLum	亮度
// 返回:	void
//---------------------------------------------------------------------------
void KBitmap16::DrawLum(int nX, int nY, int nLum)
{
	/*
	LPVOID lpBitmap = m_pBitmap;
	if (lpBitmap)
	{
		//g_DrawBitmap16Lum(nX, nY, m_nWidth, m_nHeight, nLum, lpBitmap);
	}
	*/
}
//---------------------------------------------------------------------------
// 函数:	PutPixel
// 功能:	画点
// 参数:	nX		X 坐标
//			nY		Y 坐标
//			wColor	颜色值
// 返回:	void
//---------------------------------------------------------------------------
void KBitmap16::PutPixel(int nX, int nY, WORD wColor)
{
	if ((nX < 0) || (nY < 0) || (nX >= m_nWidth) || (nY >= m_nHeight))
		return;
	LPWORD lpBitmap = (LPWORD)m_pBitmap;
	if (NULL == lpBitmap)
		return;
	lpBitmap[nY * m_nWidth + nX] = wColor;
}
//---------------------------------------------------------------------------
// 函数:	GetPixel
// 功能:	读点
// 参数:	nX		X 坐标
//			nY		Y 坐标
// 返回:	颜色值
//---------------------------------------------------------------------------
WORD KBitmap16::GetPixel(int nX, int nY)
{
	if ((nX < 0) || (nY < 0) || (nX >= m_nWidth) || (nY >= m_nHeight))
		return 0;
	LPWORD lpBitmap = (LPWORD)m_pBitmap;
	if (NULL == lpBitmap)
		return 0;
	return lpBitmap[nY * m_nWidth + nX];
}
//---------------------------------------------------------------------------
// 函数:	GetPointer
// 功能:	取得点的指针
// 参数:	nX		X 坐标
//			nY		Y 坐标
// 返回:	指向点的指针
//---------------------------------------------------------------------------
LPWORD KBitmap16::GetPointer(int nX, int nY)
{
	LPWORD lpBitmap = (LPWORD)m_pBitmap;
	return &lpBitmap[nY * m_nWidth + nX];
}
//---------------------------------------------------------------------------
// 函数:	Load
// 功能:	加载位图
// 参数:	lpFileName	文件名
// 返回:	TRUE		成功
//			FALSE		失败
//---------------------------------------------------------------------------
BOOL KBitmap16::Load(LPSTR lpFileName)
{
	/*
	IFile*			pFile;
	KBMPHEADER16	Header;

	pFile = g_OpenFile(lpFileName);
	if(pFile == NULL)
		return FALSE;
	pFile->Read(&Header, sizeof(Header));
	
	if (!g_MemComp(Header.Id, "BM16", 4))
		return FALSE;

	if (!Init(Header.Width, Header.Height))
		return FALSE;

	File.Read(m_Bitmap.GetMemPtr(), m_nWidth * m_nHeight * 2);
	
	if (g_pDirectDraw->GetRGBBitMask16() != (DWORD)Header.RGBMask)
	{
		if (RGB_565 == Header.RGBMask)
			g_565To555(m_nWidth, m_nHeight, m_Bitmap.GetMemPtr());
		else
			g_555To565(m_nWidth, m_nHeight, m_Bitmap.GetMemPtr());
	}
	*/
	return TRUE;
}
//---------------------------------------------------------------------------
// 函数:	Save
// 功能:	保存位图
// 参数:	lpFileName	文件名
// 返回:	TRUE－成功 FALSE－失败
//---------------------------------------------------------------------------
BOOL KBitmap16::Save(LPSTR lpFileName)
{
	KSmartFile		pFile;
	KBMPHEADER16	Header;

	pFile = g_CreateFile(lpFileName);
	if(pFile == NULL)
		return FALSE;

	ZeroMemory(&Header, sizeof(Header));
	CopyMemory(Header.Id, "BM16", 4);
	Header.Width = m_nWidth;
	Header.Height = m_nHeight;
//	Header.RGBMask = g_pDirectDraw->GetRGBBitMask16();

	pFile->Write(&Header, sizeof(Header));
	pFile->Write(m_pBitmap, m_nWidth * m_nHeight * 2);
	return TRUE;
}
//---------------------------------------------------------------------------
