//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KColors.h
// Date:	2000.08.08
// Code:	Daniel Wang
// Desc:	Header File
//---------------------------------------------------------------------------
#ifndef KColors_H
#define KColors_H

//---------------------------------------------------------------------------
extern BYTE	g_Red(WORD wColor);
extern BYTE	g_Green(WORD wColor);
extern BYTE	g_Blue(WORD wColor);
extern WORD	g_RGB555(int nRed, int nGreen, int nBlue);
extern WORD	g_RGB565(int nRed, int nGreen, int nBlue);
extern void	g_555To565(int nWidth, int nHeight, void* lpBitmap);
extern void	g_565To555(int nWidth, int nHeight, void* lpBitmap);
extern WORD	(*g_RGB)(int nRed, int nGreen, int nBlue);
//---------------------------------------------------------------------------
#endif
