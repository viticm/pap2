/*
 * File:	ximage.h
 * Purpose:	General Purpose Image Class 
 */
/* === C R E D I T S  &  D I S C L A I M E R S ==============
 * Permission is given by the author to freely redistribute and include
 * this code in any program as long as this credit is given where due.
 *
 * CxImage (c)  07/Aug/2001 <ing.davide.pizzolato@libero.it>
 * CxImage version 5.00 23/Aug/2002
 * See the file history.htm for the complete bugfix and news report.
 *
 * Special thanks to Troels Knakkergaard for new features, enhancements and bugfixes
 *
 * original CImage and CImageIterator implementation are:
 * Copyright:	(c) 1995, Alejandro Aguilar Sierra <asierra@servidor.unam.mx>
 *
 * COVERED CODE IS PROVIDED UNDER THIS LICENSE ON AN "AS IS" BASIS, WITHOUT WARRANTY
 * OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, WITHOUT LIMITATION, WARRANTIES
 * THAT THE COVERED CODE IS FREE OF DEFECTS, MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE
 * OR NON-INFRINGING. THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE COVERED
 * CODE IS WITH YOU. SHOULD ANY COVERED CODE PROVE DEFECTIVE IN ANY RESPECT, YOU (NOT
 * THE INITIAL DEVELOPER OR ANY OTHER CONTRIBUTOR) ASSUME THE COST OF ANY NECESSARY
 * SERVICING, REPAIR OR CORRECTION. THIS DISCLAIMER OF WARRANTY CONSTITUTES AN ESSENTIAL
 * PART OF THIS LICENSE. NO USE OF ANY COVERED CODE IS AUTHORIZED HEREUNDER EXCEPT UNDER
 * THIS DISCLAIMER.
 *
 * Use at your own risk!
 * ==========================================================
 */
#if !defined(__MYFXIMAGE_H)
#define __MYFXIMAGE_H

#if _MSC_VER > 1000
#pragma once
#endif 

/////////////////////////////////////////////////////////////////////////////
// CxImage supported features
#define CXIMAGE_SUPPORT_ALPHA          1
#define CXIMAGE_SUPPORT_SELECTION      0 
#define CXIMAGE_SUPPORT_TRANSFORMATION 1
#define CXIMAGE_SUPPORT_DSP            1

#define CXIMAGE_SUPPORT_DECODE	1
#define CXIMAGE_SUPPORT_ENCODE	1	 	//<vho><T.Peck>
#define	CXIMAGE_SUPPORT_WINDOWS 1
#define	CXIMAGE_SUPPORT_WINCE   0		//<T.Peck>

/////////////////////////////////////////////////////////////////////////////
// CxImage supported formats
#define CXIMAGE_SUPPORT_BMP 0
#define CXIMAGE_SUPPORT_GIF 0
#define CXIMAGE_SUPPORT_JPG 1 
#define CXIMAGE_SUPPORT_PNG 0
#define CXIMAGE_SUPPORT_MNG 0
#define CXIMAGE_SUPPORT_ICO 0
#define CXIMAGE_SUPPORT_TIF 0
#define CXIMAGE_SUPPORT_TGA 0
#define CXIMAGE_SUPPORT_PCX 0
#define CXIMAGE_SUPPORT_WBMP 0
#define CXIMAGE_SUPPORT_WMF 0
#define CXIMAGE_SUPPORT_J2K 0			// Beta
#define CXIMAGE_SUPPORT_JBG 0			// Patented! see ../jbig/patents.htm
/////////////////////////////////////////////////////////////////////////////
#include "xfile.h"
#include "xiofile.h"
#include "xmemfile.h"

#include "ximadefs.h"	//<vho> adjust some #define

/////////////////////////////////////////////////////////////////////////////
// CxImage formats enumerator
enum ENUM_CXIMAGE_FORMATS{
#if CXIMAGE_SUPPORT_BMP
CXIMAGE_FORMAT_BMP=0,
#endif
#if CXIMAGE_SUPPORT_GIF
CXIMAGE_FORMAT_GIF,
#endif
#if CXIMAGE_SUPPORT_JPG
CXIMAGE_FORMAT_JPG,
#endif
#if CXIMAGE_SUPPORT_PNG
CXIMAGE_FORMAT_PNG,
#endif
#if CXIMAGE_SUPPORT_MNG
CXIMAGE_FORMAT_MNG,
#endif
#if CXIMAGE_SUPPORT_ICO
CXIMAGE_FORMAT_ICO,
#endif
#if CXIMAGE_SUPPORT_TIF
CXIMAGE_FORMAT_TIF,
#endif
#if CXIMAGE_SUPPORT_TGA
CXIMAGE_FORMAT_TGA,
#endif
#if CXIMAGE_SUPPORT_PCX
CXIMAGE_FORMAT_PCX,
#endif
#if CXIMAGE_SUPPORT_WBMP
CXIMAGE_FORMAT_WBMP,
#endif
#if CXIMAGE_SUPPORT_WMF	// <vho> - Windows Metafile
CXIMAGE_FORMAT_WMF,
#endif
#if CXIMAGE_SUPPORT_J2K
CXIMAGE_FORMAT_J2K,
#endif
#if CXIMAGE_SUPPORT_JBG
CXIMAGE_FORMAT_JBG,
#endif
CMAX_IMAGE_FORMATS
};

// needed for png, gif, tga

#define RGB2GRAY(r,g,b) ((b*11 + g*59 + r*30)/100) //color to grey mapping

struct rgb_color { BYTE r,g,b; };

/////////////////////////////////////////////////////////////////////////////
// CxImage class
/////////////////////////////////////////////////////////////////////////////
class DLL_EXP CxImage
{
//extensible information collector
typedef struct tagCxImageInfo {
	DWORD	dwEffWidth;			//DWORD aligned scan line width
	BYTE*	pImage;				//THE IMAGE BITS
	void*	pGhost;				//if this is a ghost, pGhost point to the body
	DWORD	dwType;				//original image format
	char	szLastError[256];	//debugging
	long	nProgress;			//monitor
	long	nEscape;			//escape
	long	nBkgndIndex;		//used for GIF, PNG, MNG
	RGBQUAD nBkgndColor;		//used for RGB transparency
	BYTE	nQuality;			//used for JPEG
	long	nFrame;				//used for TIF, GIF, MNG : actual frame
	long	nNumFrames;			//used for TIF, GIF, MNG : total number of frames
	DWORD	dwFrameDelay;		//used for GIF, MNG
	long	xDPI;				//horizontal resolution
	long	yDPI;				//vertical resolution
	RECT	rSelectionBox;		//bounding rectangle
	BYTE	nAlphaMax;			//max opacity (fade)
	BYTE	bAlphaPaletteEnabled; //true if alpha values in the palette are enabled.
	BYTE	bEnabled;			//enables the painting functions
	long	xOffset;
	long	yOffset;
} CXIMAGEINFO;

public:
	//constructors
	CxImage(DWORD imagetype = 0);
	CxImage(DWORD dwWidth, DWORD dwHeight, long wBpp, long imagetype = 0);
	CxImage(const CxImage &src, bool copypixels = true, bool copyselection = true, bool copyalpha = true);
	CxImage(const char * filename, DWORD imagetype);
	CxImage(FILE * stream, DWORD imagetype);
	CxImage(CxFile * stream, DWORD imagetype);
	CxImage(BYTE * buffer, DWORD size, DWORD imagetype);
	virtual ~CxImage();
	CxImage& operator = (const CxImage&);

	//initializzation
	void	Destroy();
	void*	Create(DWORD dwWidth, DWORD dwHeight, long wBpp, long imagetype = 0);
	bool	CreateFromHANDLE(HANDLE hMem);		//Windows objects (clipboard)
	void	Clear(BYTE bval=0);
	void	Copy(const CxImage &src, bool copypixels = true, bool copyselection = true, bool copyalpha = true);
	HANDLE	CopyToHandle();
	void	Transfer(CxImage &from);

	//Attributes
	long	GetSize();
	BYTE*	GetBits();
	BYTE	GetColorType();
	HANDLE	GetDIB()		const {return (HANDLE)pDib;}
	DWORD	GetHeight()		const {return head.biHeight;}
	DWORD	GetWidth()		const {return head.biWidth;}
	DWORD	GetEffWidth()	const {return info.dwEffWidth;}
	DWORD	GetNumColors()	const {return head.biClrUsed;}
	WORD	GetBpp()		const {return head.biBitCount;}
	DWORD	GetType()		const {return info.dwType;}
	char*	GetLastError()	{return info.szLastError;}
	const char* GetVersion();

	DWORD	GetFrameDelay() const {return info.dwFrameDelay;}
	void	SetFrameDelay(DWORD d) {info.dwFrameDelay=d;}

	void	GetOffset(long *x,long *y) {*x=info.xOffset; *y=info.yOffset;}
	void	SetOffset(long x,long y) {info.xOffset=x; info.yOffset=y;}

	BYTE	GetJpegQuality() const {return info.nQuality;}
	void	SetJpegQuality(BYTE q) {info.nQuality = q;}

	long	GetXDPI()		const {return info.xDPI;}
	long	GetYDPI()		const {return info.yDPI;}
	void	SetXDPI(long dpi);
	void	SetYDPI(long dpi);

	long	GetProgress()	const {return info.nProgress;}
	long	GetEscape()     const {return info.nEscape;}
	void	SetProgress(long p) {info.nProgress = p;}
	void	SetEscape(long i)   {info.nEscape = i;}

	long	GetTransIndex()	const {return info.nBkgndIndex;}
	RGBQUAD	GetTransColor();
	void	SetTransIndex(long idx) {info.nBkgndIndex = idx;}
	void	SetTransColor(RGBQUAD rgb) {info.nBkgndColor = rgb;}
	bool	IsTransparent() const {return info.nBkgndIndex>=0;} // <vho>

	//palette operations
	bool	IsGrayScale();
	bool	IsIndexed() {return head.biClrUsed!=0;}
	DWORD	GetPaletteSize();
	RGBQUAD* GetPalette() const;
	RGBQUAD GetPaletteColor(BYTE idx);
	bool	GetRGB(int i, BYTE* r, BYTE* g, BYTE* b);
	BYTE	GetNearestIndex(RGBQUAD c);
	void	BlendPalette(COLORREF cr,long perc);
	void	SetGrayPalette();
	void	SetPalette(DWORD n, BYTE *r, BYTE *g, BYTE *b);
	void	SetPalette(RGBQUAD* pPal,DWORD nColors=256);
	void	SetPalette(rgb_color *rgb,DWORD nColors=256);
	void	SetPaletteIndex(BYTE idx, BYTE r, BYTE g, BYTE b, BYTE alpha=0);
	void	SetPaletteIndex(BYTE idx, RGBQUAD c);
	void	SetPaletteIndex(BYTE idx, COLORREF cr);
	void	SwapIndex(BYTE idx1, BYTE idx2);
	void	SetStdPalette();

	//pixel operations
	bool	IsInside(long x, long y);
	BYTE	GetPixelIndex(long x,long y);
	RGBQUAD GetPixelColor(long x,long y);
	void	SetPixelColor(long x,long y,RGBQUAD c);
	void	SetPixelIndex(long x,long y,BYTE i);
	void	SetPixelColor(long x,long y,COLORREF cr);

	//painting operations
#if CXIMAGE_SUPPORT_WINCE
	long	Blt(HDC pDC, long x=0, long y=0);
#endif
#if CXIMAGE_SUPPORT_WINDOWS
	HBITMAP MakeBitmap(HDC hdc = NULL);
	void	CreateFromHBITMAP(HBITMAP hbmp);	//Windows resource
	long	Draw(HDC hdc, long x=0, long y=0, long cx = -1, long cy = -1, RECT* pClipRect = 0);
	long	Draw(HDC hdc, const RECT& rect, RECT* pClipRect=NULL) { return Draw(hdc, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, pClipRect); }
	long	Stretch(HDC hdc, long xoffset, long yoffset, long xsize, long ysize);
	long	Stretch(HDC hdc, const RECT& rect) { return Stretch(hdc, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top); }
	long	Tile(HDC hdc, RECT *rc);
	long	Draw2(HDC hdc, long x=0, long y=0, long cx = -1, long cy = -1);
	long	Draw2(HDC hdc, const RECT& rect) { return Draw2(hdc, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top); }
#endif //CXIMAGE_SUPPORT_WINDOWS

	// file operations
#if CXIMAGE_SUPPORT_DECODE
#ifdef WIN32
	bool Load(LPCWSTR filename, DWORD imagetype=0);
	bool LoadResource(HRSRC hRes, DWORD imagetype, HMODULE hModule=NULL);
#endif
	bool Load(const char * filename, DWORD imagetype=0);
	bool Decode(FILE * hFile, DWORD imagetype);
	bool Decode(CxFile * hFile, DWORD imagetype);
	bool Decode(BYTE * buffer, DWORD size, DWORD imagetype);
#endif //CXIMAGE_SUPPORT_DECODE

#if CXIMAGE_SUPPORT_ENCODE
#ifdef WIN32
	bool Save(LPCWSTR filename, DWORD imagetype=0);
#endif
	bool Save(const char * filename, DWORD imagetype=0);
	bool Encode(FILE * hFile, DWORD imagetype);
	bool Encode(CxFile * hFile, DWORD imagetype);
	bool Encode(CxFile * hFile, CxImage ** pImages, int pagecount, DWORD imagetype);
	bool Encode(FILE *hFile, CxImage ** pImages, int pagecount, DWORD imagetype);
	bool Encode(BYTE * &buffer, long &size, DWORD imagetype);
#endif //CXIMAGE_SUPPORT_ENCODE

	//misc.
	bool IsValid() const {return pDib!=0;}
	bool IsEnabled() const {return info.bEnabled!=0;}
	void Enable(bool enable=true){info.bEnabled=enable;}

	// frame operations
	long GetNumFrames() const {return info.nNumFrames;}
	long GetFrame() const {return info.nFrame;}
	void SetFrame(long nFrame) {info.nFrame=nFrame;}

#if CXIMAGE_SUPPORT_BASICTRANSFORMATIONS
	bool GrayScale();
	bool Flip();
	bool Mirror();
	bool RotateLeft(CxImage* iDst = NULL);
	bool RotateRight(CxImage* iDst = NULL);
#endif //CXIMAGE_SUPPORT_BASICTRANSFORMATIONS

#if CXIMAGE_SUPPORT_TRANSFORMATION
	// image operations
	bool Negative();
	bool Rotate(float angle, CxImage* iDst = NULL);
	bool Rotate180(CxImage* iDst = NULL);
	bool Resample(long newx, long newy, int fast = 1);
	bool DecreaseBpp(DWORD nbit, bool errordiffusion, RGBQUAD* ppal = 0);
	bool IncreaseBpp(DWORD nbit);
	bool Dither(long method = 0);
	bool Crop(long left, long top, long right, long bottom, CxImage* iDst = NULL);
	bool Crop(const RECT& rect, CxImage* iDst = NULL) { return Crop(rect.left, rect.top, rect.right, rect.bottom, iDst); }
#endif //CXIMAGE_SUPPORT_TRANSFORMATION

#if CXIMAGE_SUPPORT_DSP
	bool Repair(float radius = 0.25f, long niterations = 1, long colorspace = 0);
	bool Combine(CxImage* r,CxImage* g,CxImage* b,CxImage* a, long colorspace = 0);
	bool FFT2(CxImage* srcReal, CxImage* srcImag, CxImage* dstReal, CxImage* dstImag, long direction = 1, bool bForceFFT = true, bool bMagnitude = true);
	long Histogram(long* red, long* green = 0, long* blue = 0, long* gray = 0, long colorspace = 0);
	bool Noise(long level);
	bool Median(long Ksize=3);
	bool Gamma(float gamma);
	bool ShiftRGB(long r, long g, long b);
	bool Threshold(BYTE level);
	bool Colorize(BYTE hue, BYTE sat);
	bool Light(long level, long contrast=100);
	float Mean();
	bool Filter(long* kernel, long Ksize, long Kfactor, long Koffset);
	bool Erode(long Ksize=2);
	bool Dilate(long Ksize=2);
	void HuePalette(float correction=1);
	enum ImageOpType { OpAdd, OpAnd, OpXor, OpOr, OpMask, OpSrcCopy, OpDstCopy, OpSub, OpSrcBlend };
	void Mix(CxImage & imgsrc2, ImageOpType op, long lXOffset = 0, long lYOffset = 0);
protected:
	bool IsPowerof2(long x);
	bool FFT(int dir,int m,double *x,double *y);
	bool DFT(int dir,long m,double *x1,double *y1,double *x2,double *y2);
	bool RepairChannel(CxImage *ch, float radius);
public:
	//color conversion utilities
	bool SplitRGB(CxImage* r,CxImage* g,CxImage* b);
	bool SplitYUV(CxImage* y,CxImage* u,CxImage* v);
	bool SplitHSL(CxImage* h,CxImage* s,CxImage* l);
	bool SplitYIQ(CxImage* y,CxImage* i,CxImage* q);
	bool SplitXYZ(CxImage* x,CxImage* y,CxImage* z);
	RGBQUAD HSLtoRGB(COLORREF cHSLColor);
	RGBQUAD RGBtoHSL(RGBQUAD lRGBColor);
	RGBQUAD HSLtoRGB(RGBQUAD lHSLColor);
	RGBQUAD YUVtoRGB(RGBQUAD lYUVColor);
	RGBQUAD RGBtoYUV(RGBQUAD lRGBColor);
	RGBQUAD YIQtoRGB(RGBQUAD lYIQColor);
	RGBQUAD RGBtoYIQ(RGBQUAD lRGBColor);
	RGBQUAD XYZtoRGB(RGBQUAD lXYZColor);
	RGBQUAD RGBtoXYZ(RGBQUAD lRGBColor);
#endif //CXIMAGE_SUPPORT_DSP
	RGBQUAD RGBtoRGBQUAD(COLORREF cr);
	COLORREF RGBQUADtoRGB (RGBQUAD c);

#if CXIMAGE_SUPPORT_SELECTION
	//selection
	void SelectionClear();
	void SelectionCreate();
	void SelectionDelete();
	void SelectionInvert();
	void SelectionAddRect(RECT r);
	void SelectionAddEllipse(RECT r);
	void SelectionAddPolygon(POINT *points, long npoints);
	bool SelectionCopy(CxImage &from);
	bool IsInsideSelection(long x, long y);
	bool HasSelection(){return pSelection!=0;}
	void GetSelectionBox(RECT& r){memcpy(&r,&info.rSelectionBox,sizeof(RECT));}
#endif //CXIMAGE_SUPPORT_SELECTION

#if CXIMAGE_SUPPORT_ALPHA
	//Alpha
	void AlphaClear();
	void AlphaCreate();
	void AlphaDelete();
	void AlphaInvert();
	bool AlphaMirror();
	bool AlphaFlip();
	bool AlphaCopy(CxImage &from);
	bool AlphaSplit(CxImage *dest);
	void AlphaStrip();
	void AlphaSet(BYTE level);
	void AlphaSet(long x,long y,BYTE level);
	BYTE AlphaGet(long x,long y);
	BYTE AlphaGetMax() const {return info.nAlphaMax;}
	void AlphaSetMax(BYTE nAlphaMax) {info.nAlphaMax=nAlphaMax;}
	bool HasAlpha(){return pAlpha!=0;}

	void AlphaPaletteClear();
	void AlphaPaletteEnable(bool enable=true){info.bAlphaPaletteEnabled=enable;}
	bool IsAlphaPaletteEnabled(){return info.bAlphaPaletteEnabled!=0;}
	bool HasAlphaPalette();
#endif //CXIMAGE_SUPPORT_ALPHA

protected:
	void Startup(DWORD imagetype = 0);
	void CopyInfo(const CxImage &src);
	void Ghost(CxImage *src);
	void RGBtoBGR(BYTE *buffer, int length);
	float HueToRGB(float n1,float n2, float hue);
	void Bitfield2RGB(BYTE *src, WORD redmask, WORD greenmask, WORD bluemask, BYTE bpp);
	static int CompareColors(const void *elem1, const void *elem2);

	void*				pDib; //contains the header, the palette, the pixels
    BITMAPINFOHEADER    head; //stadnard header
	CXIMAGEINFO			info; //extended information
	BYTE*				pSelection;	//selected region
	BYTE*				pAlpha; //alpha channel
};
////////////////////////////////////////////////////////////////////////////
#endif // !defined(__MYFXIMAGE_H)
