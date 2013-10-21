////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGMathFunctions.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2007-6-22 17:23:30
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "KGMathFunctions.h"


BOOL D3DCOLOR2LPTSTR( D3DCOLOR color, LPTSTR czResult, int nBufferSize )
{
	if (!czResult || nBufferSize < 3*4+4+1)
	{
		return FALSE;
	}

	INT ARGB[4];
	D3DCOLOR2IntArray(color, ARGB);
	int nRet = _stprintf_s(czResult, nBufferSize, _T("%d,%d,%d,%d"), ARGB[0], ARGB[1], ARGB[2], ARGB[3]);
	return nRet > 4? TRUE : FALSE;///sprintf返回的是拷贝的字符长度
}

	BOOL LPCTSTR2D3DCOLOR( LPCTSTR czSource, int nMaxStringLen, D3DCOLOR& ResultColor )
	{
		KG_PROCESS_ERROR(czSource);
		int ColorElements[4] = {0, 0, 0, 0};
		TCHAR LocalSource[128];
		int nRet = _tcscpy_s(LocalSource, _countof(LocalSource), czSource);
		KG_PROCESS_ERROR(nRet == 0);
		TCHAR tczSeps[] = _T(" ,\t\n");
		TCHAR* pToken1 = NULL;
		TCHAR* pNext_Token = NULL;
		pToken1 = _tcstok_s(LocalSource, tczSeps, &pNext_Token);
		for(int i = 0; i < 4; i++ )
		{
			KG_PROCESS_ERROR(NULL != pToken1);///不能全部解释出来也是错误
			int nTemp = _ttoi(pToken1);
			KG_PROCESS_ERROR(nTemp >= 0 && nTemp <= 255);
			ColorElements[i] = nTemp;
			pToken1 = _tcstok_s(NULL, tczSeps, &pNext_Token);
		}
		ResultColor = D3DCOLOR_ARGB(ColorElements[0], ColorElements[1], ColorElements[2], ColorElements[3]);
		return TRUE;
Exit0:
		return FALSE;
	}
	//////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////

D3DCOLORVALUE Color2Value(D3DCOLOR Color)
{
	D3DCOLORVALUE nColor;
	nColor.a = ((float)(Color>>24))/255.0f;
	nColor.r = ((float)((Color&0x00ff0000)>>16))/255.0f;
	nColor.g = ((float)((Color&0x0000ff00)>>8))/255.0f;
	nColor.b = ((float)((Color&0x000000ff)))/255.0f;
	return nColor;
}

VOID Color2Value( D3DCOLOR Color, D3DCOLORVALUE& Value )
{
	Value.a = ((float)(Color>>24))/255.0f;
	Value.r = ((float)((Color&0x00ff0000)>>16))/255.0f;
	Value.g = ((float)((Color&0x0000ff00)>>8))/255.0f;
	Value.b = ((float)((Color&0x000000ff)))/255.0f;
}
D3DCOLOR ColorValue(D3DCOLORVALUE co)
{
	return D3DCOLOR_ARGB((int)(co.a*255),(int)(co.r*255),
		(int)(co.g*255),(int)(co.b*255));
}
COLORREF ColorValue2ref(D3DCOLORVALUE co)
{
	return RGB(co.r*255,co.g*255,co.b*255);
}
D3DCOLORVALUE Colorref2value(COLORREF Color,float a)
{
	D3DCOLORVALUE nColor;
	nColor.r = 1.0f;
	nColor.g = 1.0f;
	nColor.b = 1.0f;
	nColor.a = a;
	nColor.b = ((float)(Color>>16))/255.0f;
	nColor.g = ((float)((Color&0x00ff00)>>8))/255.0f;
	nColor.r = ((float)((Color&0x0000ff)))/255.0f;
	return nColor;
}
D3DCOLORVALUE Colorref2value_GDI(COLORREF Color,float a)
{
	D3DCOLORVALUE nColor;
	nColor.r = 1.0f;
	nColor.g = 1.0f;
	nColor.b = 1.0f;
	nColor.a = a;
	nColor.r = ((float)(Color>>16))/255.0f;
	nColor.g = ((float)((Color&0x00ff00)>>8))/255.0f;
	nColor.b = ((float)((Color&0x0000ff)))/255.0f;
	return nColor;
}
D3DCOLOR Value2Color( const D3DCOLORVALUE& D3DColorValue )
{
	return D3DCOLOR_COLORVALUE( D3DColorValue.r, D3DColorValue.g, D3DColorValue.b, D3DColorValue.a );
}
void Color2Ref( const D3DCOLOR &D3DColor, COLORREF &CRColor )//CTH Added
{
	CRColor = 0x00000000 //COLORREF的高位衡为0
		| ((D3DColor&0x00ff0000)>>16) //R
		| (D3DColor&0x0000ff00) 
		| ((D3DColor&0x000000ff)<<16);//B
}
/*
void Color2Ints( const D3DCOLOR& D3DColor, int& nRed, int& nGreen, int& nBlue, int& nAlpha )
{
nRed = D3DCOLOR_GETRED(D3DColor);
nGreen = D3DCOLOR_GETGREEN(D3DColor);
nBlue = D3DCOLOR_GETBLUE(D3DColor);
nAlpha = D3DCOLOR_GETALPHA(D3DColor);
}*/

void LPCTSTR2IntInRange( LPCTSTR czSource, int* pnDes, int nLowerBound, int nUpperBound )//CTH Added
{
	_ASSERTE( nLowerBound <= nUpperBound );
	int nTemp = _tstoi( czSource );
	if ( nTemp <= nLowerBound )
	{nTemp = nLowerBound;}
	else if( nTemp >= nUpperBound )
	{nTemp = nUpperBound;}
	if ( NULL !=  pnDes )
	{
		*pnDes = nTemp;
	}
}

void TwoColorBlend( const D3DCOLOR& Color1, const D3DCOLOR& Color2, D3DCOLOR& ColorResult, float fPercent, bool bBlendAlpha /* false*/)
{
	int RAlpha = D3DCOLOR_GETALPHA(Color1);
	if (fPercent <= 0.0)
	{
		ColorResult = Color1;
		goto Exit1;
	}
	else if (fPercent >= 1.0)
	{
		ColorResult = Color2;
		goto Exit1;
	}
	if (Color1 == Color2)
	{
		ColorResult = Color2;
		goto Exit1;
	}
	int C1Red, C1Green, C1Blue, C1Alpha, C2Red, C2Green, C2Blue, C2Alpha;
	C1Red = D3DCOLOR_GETRED(Color1);
	C1Green = D3DCOLOR_GETGREEN(Color1);
	C1Blue = D3DCOLOR_GETBLUE(Color1);
	C1Alpha = D3DCOLOR_GETALPHA(Color1);
	C2Red = D3DCOLOR_GETRED(Color2);
	C2Green = D3DCOLOR_GETGREEN(Color2);
	C2Blue = D3DCOLOR_GETBLUE(Color2);
	C2Alpha = D3DCOLOR_GETALPHA(Color2);
	int RRed, RGreen, RBlue;
	RRed = (int)(C1Red*(1.0-fPercent) + C2Red*fPercent);
	RGreen = (int)(C1Green*(1.0-fPercent) + C2Green*fPercent);
	RBlue = (int)(C1Blue*(1.0-fPercent) + C2Blue*fPercent);
	MT_LIMIT_D3DCOLOR_RANGE(RRed);
	MT_LIMIT_D3DCOLOR_RANGE(RGreen);
	MT_LIMIT_D3DCOLOR_RANGE(RBlue);
	if (bBlendAlpha == true)
	{
		RAlpha = (int)(C1Alpha*(1.0-fPercent) + C2Alpha*fPercent);
		MT_LIMIT_D3DCOLOR_RANGE(RAlpha);
	}
	ColorResult = D3DCOLOR_ARGB(RAlpha, RRed, RGreen, RBlue);
	return;
Exit1:
	if (!bBlendAlpha)
	{
		ColorResult &= 0x00ffffff;///清掉最高位
		DWORD dwRAlpha = static_cast<DWORD>(RAlpha);
		dwRAlpha = dwRAlpha << 24;
		ColorResult |= dwRAlpha;
	}
	return;
}
