// ximage.cpp : main implementation file
/* 07/08/2001 v1.00 - ing.davide.pizzolato@libero.it
 * CxImage version 5.00 23/Aug/2002
 */
#include "ximage.h"

////////////////////////////////////////////////////////////////////////////////
// CxImage 
////////////////////////////////////////////////////////////////////////////////
// Initialize the internal structures
void CxImage::Startup(DWORD imagetype)
{
	//init pointers
	pDib=pSelection=pAlpha=NULL;
	//init structures
	memset(&head,0,sizeof(BITMAPINFOHEADER));
    head.biSize = sizeof(BITMAPINFOHEADER);
	memset(&info,0,sizeof(CXIMAGEINFO));
	//init default attributes
    info.dwType = imagetype;
	info.nQuality=75;
	info.nAlphaMax=255;
	info.nBkgndIndex=-1;
	info.bEnabled=1;
	SetXDPI(72);
	SetYDPI(72);
}
////////////////////////////////////////////////////////////////////////////////
// Empty image constructor
// > imagetype: (optional) set the image format (for future use)
CxImage::CxImage(DWORD imagetype)
{
	Startup(imagetype);
}
////////////////////////////////////////////////////////////////////////////////
CxImage::~CxImage()
{
	Destroy();
}
////////////////////////////////////////////////////////////////////////////////
void CxImage::Destroy()
{
	//free this only if it's valid and it's not a ghost
	if ((pDib)&&(info.pGhost==NULL)){
		if (pSelection) {free(pSelection); pSelection=0;}
		if (pAlpha) {free(pAlpha); pAlpha=0;}
		free(pDib); pDib=0;
	}
}
////////////////////////////////////////////////////////////////////////////////
// Sized image constructor
// > dwWidth: width
// > dwHeight: height
// > wBpp: bit per pixel
// > imagetype: (optional) set the image format (for future use)
CxImage::CxImage(DWORD dwWidth, DWORD dwHeight, long wBpp, long imagetype)
{
	Startup(imagetype);
	Create(dwWidth,dwHeight,wBpp,imagetype);
}
////////////////////////////////////////////////////////////////////////////////
// image constructor from existing source
// > src: source image.
// > copypixels: copy the pixels from the source image into the new image.
// > copyselection: copy the selection from src
CxImage::CxImage(const CxImage &src, bool copypixels, bool copyselection, bool copyalpha)
{
	Startup(src.GetType());
	Copy(src,copypixels,copyselection,copyalpha);
}
////////////////////////////////////////////////////////////////////////////////
void CxImage::Copy(const CxImage &src, bool copypixels, bool copyselection, bool copyalpha)
{
	//copy the attributes
	memcpy(&info,&src.info,sizeof(CXIMAGEINFO));
	//rebuild the image
	Create(src.GetWidth(),src.GetHeight(),src.GetBpp(),src.GetType());
	//copy the pixels and the palette, or at least copy the palette only.
	if (copypixels && pDib && src.pDib) memcpy(pDib,src.pDib,GetSize());
	else SetPalette(src.GetPalette());
	long nSize = head.biWidth * head.biHeight;
	//copy the selection
	if (copyselection && src.pSelection){
		if (pSelection) free(pSelection);
		pSelection = (BYTE*)malloc(nSize);
		memcpy(pSelection,src.pSelection,nSize);
	}
	//copy the alpha channel
	if (copyalpha && src.pAlpha){
		if (pAlpha) free(pAlpha);
		pAlpha = (BYTE*)malloc(nSize);
		memcpy(pAlpha,src.pAlpha,nSize);
	}
}
////////////////////////////////////////////////////////////////////////////////
// Use it before Create()
void CxImage::CopyInfo(const CxImage &src)
{
	if (pDib==NULL) memcpy(&info,&src.info,sizeof(CXIMAGEINFO));
}
////////////////////////////////////////////////////////////////////////////////
CxImage& CxImage::operator = (const CxImage& isrc)
{
	if (this != &isrc) Copy(isrc);
	return *this;
}
////////////////////////////////////////////////////////////////////////////////
// Initializes or rebuilds the image.
// Returns the pointer to the internal pDib object
// > dwWidth: width
// > dwHeight: height
// > wBpp: bit per pixel
// > imagetype: (optional) set the image format (for future use)
void* CxImage::Create(DWORD dwWidth, DWORD dwHeight, long wBpp, long imagetype)
{
	// destroy the existing image (if any)
	Destroy();

    // Make sure bits per pixel is valid
    if		(wBpp <= 1)	wBpp = 1;
    else if (wBpp <= 4)	wBpp = 4;
    else if (wBpp <= 8)	wBpp = 8;
    else				wBpp = 24;

	// set the correct bpp value
    switch (wBpp){
        case 1:
            head.biClrUsed = 2;	break;
        case 4:
            head.biClrUsed = 16; break;
        case 8:
            head.biClrUsed = 256; break;
        default:
            head.biClrUsed = 0;
    }

	//set the common image informations
    info.dwEffWidth = ((((wBpp * dwWidth) + 31) / 32) * 4);
    info.dwType = imagetype;

    // initialize BITMAPINFOHEADER
    head.biWidth = dwWidth;		// fill in width from parameter
    head.biHeight = dwHeight;	// fill in height from parameter
    head.biPlanes = 1;			// must be 1
    head.biBitCount = (WORD)wBpp;		// from parameter
    head.biCompression = BI_RGB;    
    head.biSizeImage = info.dwEffWidth * dwHeight;
//    head.biXPelsPerMeter = 0; See SetXDPI
//    head.biYPelsPerMeter = 0; See SetYDPI
    head.biClrImportant = 0;

	pDib = malloc(GetSize()); // alloc memory block to store our bitmap
    if (!pDib) return NULL;

	//clear the palette
	RGBQUAD* pal=GetPalette();
	if (pal) memset(pal,0,GetPaletteSize());
	//Destroy the existing selection
#if CXIMAGE_SUPPORT_SELECTION
	if (pSelection) SelectionDelete();
#endif //CXIMAGE_SUPPORT_SELECTION
	//Destroy the existing alpha channel
#if CXIMAGE_SUPPORT_ALPHA
	if (pAlpha) AlphaDelete();
#endif //CXIMAGE_SUPPORT_ALPHA

    // use our bitmap info structure to fill in first part of
    // our DIB with the BITMAPINFOHEADER
    BITMAPINFOHEADER*  lpbi;
	lpbi = (BITMAPINFOHEADER*)(pDib);
    *lpbi = head;

	info.pImage=GetBits();

    return pDib; //return handle to the DIB
}
////////////////////////////////////////////////////////////////////////////////
// returns the pointer to the image pixels
BYTE* CxImage::GetBits()
{ 
	if (pDib)	return ((BYTE*)pDib + *(DWORD*)pDib + GetPaletteSize()); 
	return NULL;
}
////////////////////////////////////////////////////////////////////////////////
// returns the whole pDib dimension
long CxImage::GetSize()
{
	return head.biSize + head.biSizeImage + GetPaletteSize();
}
////////////////////////////////////////////////////////////////////////////////
bool CxImage::IsInside(long x, long y)
{
  return (0<=y && y<head.biHeight && 0<=x && x<head.biWidth);
}
////////////////////////////////////////////////////////////////////////////////
void CxImage::Clear(BYTE bval)
{
	if (pDib) memset(info.pImage,bval,head.biSizeImage);
}
////////////////////////////////////////////////////////////////////////////////
// moves everything from (*from) to (this), (*from) become empty
void CxImage::Transfer(CxImage &from)
{
	if (pDib) free(pDib);
	if (pSelection) free(pSelection);
	if (pAlpha) free(pAlpha);

	memcpy(&head,&from.head,sizeof(BITMAPINFOHEADER));
	memcpy(&info,&from.info,sizeof(CXIMAGEINFO));

	pDib = from.pDib;
	pSelection= from.pSelection;
	pAlpha= from.pAlpha;

	memset(&from.head,0,sizeof(BITMAPINFOHEADER));
	memset(&from.info,0,sizeof(CXIMAGEINFO));
	from.pDib=NULL;
}
////////////////////////////////////////////////////////////////////////////////
// (this) points to the same pDib owned by (*from), the image remains in (*from)
// but (this) has the access to the pixels. Use carefully !!!
void CxImage::Ghost(CxImage *from)
{
	if (from){
		memcpy(&head,&from->head,sizeof(BITMAPINFOHEADER));
		memcpy(&info,&from->info,sizeof(CXIMAGEINFO));
		pDib = from->pDib;
		pSelection = from->pSelection;
		pAlpha = from->pAlpha;
		info.pGhost=from;
	}
}

#ifdef WIN32
////////////////////////////////////////////////////////////////////////////////
// Transfer the image in a global bitmap handle (clipboard copy)
HANDLE CxImage::CopyToHandle()
{
	HANDLE hMem=NULL;
	if (pDib){
		hMem= GlobalAlloc(GHND, GetSize());
		BYTE* pDst=(BYTE*)GlobalLock(hMem);
		memcpy(pDst,pDib,GetSize());
		GlobalUnlock(hMem);
	}
	return hMem;
}
////////////////////////////////////////////////////////////////////////////////
// Global object (clipboard paste) constructor
// § the clipboard format must be CF_DIB.
// > hMem: source bitmap object
bool CxImage::CreateFromHANDLE(HANDLE hMem)
{
	if (pDib) {free(pDib); pDib=NULL;}

	BYTE *lpVoid;						//pointer to the bitmap
	lpVoid = (BYTE *)GlobalLock(hMem);
	BITMAPINFOHEADER *pHead;			//pointer to the bitmap header
	pHead = (BITMAPINFOHEADER *)lpVoid;
	if (lpVoid){
		//copy the bitmap header
		memcpy(&head,pHead,sizeof(BITMAPINFOHEADER));
		//create the image
		if(!Create(head.biWidth,head.biHeight,head.biBitCount)){
			GlobalUnlock(lpVoid);
			return false;
		}
		//preserve DPI
		SetXDPI((long)floor(head.biXPelsPerMeter * 254.0 / 10000.0 + 0.5));
		SetYDPI((long)floor(head.biYPelsPerMeter * 254.0 / 10000.0 + 0.5));
		//copy the pixels
		if((pHead->biCompression != BI_RGB) || (pHead->biBitCount == 32)){ //<Jörgen Alfredsson>
			// BITFIELD case
			// set the internal header in the dib
			memcpy(pDib,&head,sizeof(head));
			// get the bitfield masks
			DWORD bf[3];
			memcpy(bf,lpVoid+pHead->biSize,12);
			// transform into RGB
			Bitfield2RGB(lpVoid+pHead->biSize+12,(WORD)bf[0],(WORD)bf[1],(WORD)bf[2],(BYTE)pHead->biBitCount);
		} else { //normal bitmap
			memcpy(pDib,lpVoid,GetSize());
		}
		GlobalUnlock(lpVoid);
		return true;
	}
	return false;
}
#endif //WIN32
////////////////////////////////////////////////////////////////////////////////
// turns a 16 or 32 bit bitfield image into a RGB image
void CxImage::Bitfield2RGB(BYTE *src, WORD redmask, WORD greenmask, WORD bluemask, BYTE bpp)
{
	switch (bpp){
	case 16:
	{
		DWORD ns[3]={0,0,0};
		// compute the number of shift for each mask
		for (int i=0;i<16;i++){
			if ((redmask>>i)&0x01) ns[0]++;
			if ((greenmask>>i)&0x01) ns[1]++;
			if ((bluemask>>i)&0x01) ns[2]++;
		}
		ns[1]+=ns[0]; ns[2]+=ns[1];	ns[0]=8-ns[0]; ns[1]-=8; ns[2]-=8;
		// dword aligned width for 16 bit image
		long effwidth2=(((head.biWidth + 1) / 2) * 4);
		WORD w;
		long y2,y3,x2,x3;
		BYTE *p=info.pImage;
		// scan the buffer in reverse direction to avoid reallocations
		for (long y=head.biHeight-1; y>=0; y--){
			y2=effwidth2*y;
			y3=info.dwEffWidth*y;
			for (long x=head.biWidth-1; x>=0; x--){
				x2 = 2*x+y2;
				x3 = 3*x+y3;
				w = (WORD)(src[x2]+256*src[1+x2]);
				p[  x3]=(BYTE)((w & bluemask)<<ns[0]);
				p[1+x3]=(BYTE)((w & greenmask)>>ns[1]);
				p[2+x3]=(BYTE)((w & redmask)>>ns[2]);
			}
		}
		break;
	}
	case 32:
	{
		// dword aligned width for 32 bit image
		long effwidth4 = head.biWidth * 4;
		long y4,y3,x4,x3;
		BYTE *p=info.pImage;
		// scan the buffer in reverse direction to avoid reallocations
		for (long y=head.biHeight-1; y>=0; y--){
			y4=effwidth4*y;
			y3=info.dwEffWidth*y;
			for (long x=head.biWidth-1; x>=0; x--){
				x4 = 4*x+y4;
				x3 = 3*x+y3;
				p[  x3]=src[  x4];
				p[1+x3]=src[1+x4];
				p[2+x3]=src[2+x4];
			}
		}
	}

	}
	return;
}
////////////////////////////////////////////////////////////////////////////////
int CxImage::CompareColors(const void *elem1, const void *elem2)
{
	RGBQUAD* c1 = (RGBQUAD*)elem1;
	RGBQUAD* c2 = (RGBQUAD*)elem2;

	int g1 = RGB2GRAY(c1->rgbRed,c1->rgbGreen,c1->rgbBlue);
	int g2 = RGB2GRAY(c2->rgbRed,c2->rgbGreen,c2->rgbBlue);
	
	if     (g1==g2) return 0;
	else if (g1>g2) return 1;
    else            return -1;
}
////////////////////////////////////////////////////////////////////////////////
RGBQUAD	CxImage::GetTransColor()
{
	if (head.biBitCount<24 && info.nBkgndIndex != -1) return GetPaletteColor((BYTE)info.nBkgndIndex);
	return info.nBkgndColor;
}
////////////////////////////////////////////////////////////////////////////////
BYTE CxImage::GetColorType()
{
	BYTE b = (BYTE)((head.biBitCount>8) ? 2 /*COLORTYPE_COLOR*/ : 1 /*COLORTYPE_PALETTE*/);
#if CXIMAGE_SUPPORT_ALPHA
	if (HasAlpha()) b = 4 /*COLORTYPE_ALPHA*/;
#endif //CXIMAGE_SUPPORT_ALPHA
	return b;
}
////////////////////////////////////////////////////////////////////////////////
void CxImage::SetXDPI(long dpi)
{
	info.xDPI = dpi;
	head.biXPelsPerMeter = (long) floor(dpi * 10000.0 / 254.0 + 0.5);
}
////////////////////////////////////////////////////////////////////////////////
void CxImage::SetYDPI(long dpi)
{
	info.yDPI = dpi;
	head.biYPelsPerMeter = (long) floor(dpi * 10000.0 / 254.0 + 0.5);
}
////////////////////////////////////////////////////////////////////////////////
const char* CxImage::GetVersion()
{
	static const char CxImageVersion[] = "CxImage 5.00";
	return (CxImageVersion);
}
//EOF
