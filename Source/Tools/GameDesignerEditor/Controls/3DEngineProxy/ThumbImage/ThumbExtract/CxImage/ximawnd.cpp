// xImaWnd.cpp : Windows functions
/* 07/08/2001 v1.00 - ing.davide.pizzolato@libero.it
 * CxImage version 5.00 23/Aug/2002
 */

#include "ximage.h"


////////////////////////////////////////////////////////////////////////////////
#if CXIMAGE_SUPPORT_WINCE
////////////////////////////////////////////////////////////////////////////////
long CxImage::Blt(HDC pDC, long x, long y)
{
	if((pDib==0)||(pDC==0)||(info.bEnabled==0)) return 0;

    HBRUSH brImage = CreateDIBPatternBrushPt(pDib, DIB_RGB_COLORS);
    HBRUSH brOld = (HBRUSH) SelectObject(pDC, brImage);
    PatBlt(pDC, 0, 0, head.biWidth, head.biHeight, PATCOPY);
    SelectObject(pDC, brOld);
    DeleteObject(brImage);
	return 1;
}
#endif //CXIMAGE_SUPPORT_WINCE
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
#if CXIMAGE_SUPPORT_WINDOWS
////////////////////////////////////////////////////////////////////////////////
// Transfer the image in a  bitmap handle
HBITMAP CxImage::MakeBitmap(HDC hdc)
{
	if (!pDib)
		return NULL;

	if (!hdc)
		// Create a device-independent bitmap <CSC>
		return CreateBitmap(head.biWidth,head.biHeight,
							1, head.biBitCount, GetBits());

	// this single line seems to work very well
	HBITMAP bmp = CreateDIBitmap(hdc, (LPBITMAPINFOHEADER)pDib, CBM_INIT,
		GetBits(), (LPBITMAPINFO)pDib, DIB_RGB_COLORS);
	return bmp;
}
////////////////////////////////////////////////////////////////////////////////
// Bitmap resource constructor
// > hbmp: bitmap resource handle
void CxImage::CreateFromHBITMAP(HBITMAP hbmp)
{
	if (pDib) {free(pDib); pDib=NULL;}

	if (hbmp) { 
        BITMAP bm;
		// get informations about the bitmap
        GetObject(hbmp, sizeof(BITMAP), (LPSTR) &bm);
		// create the image
        Create(bm.bmWidth, bm.bmHeight, bm.bmBitsPixel);
		// create a device context for the bitmap
        HDC dc = ::GetDC(NULL);
		// copy the pixels
        if (GetDIBits(dc, hbmp, 0, head.biHeight, info.pImage,
			(LPBITMAPINFO)pDib, DIB_RGB_COLORS) == 0){ //replace &head with pDib <Wil Stark>
            strcpy(info.szLastError,"GetDIBits failed");
			::ReleaseDC(NULL, dc);
			return;
        }
        ::ReleaseDC(NULL, dc);
    }
}
////////////////////////////////////////////////////////////////////////////////
// Draws (stretch) the image with transparency & alpha support
// > hdc: destination device context
// > x,y: (optional) offset
// > cx,cy: (optional) size.
long CxImage::Draw(HDC hdc, long x, long y, long cx, long cy, RECT* pClipRect)
{
	if((pDib==0)||(hdc==0)||(cx==0)||(cy==0)||(info.bEnabled==0)) return 0;

	if (cx < 0) cx = head.biWidth;
	if (cy < 0) cy = head.biHeight;
	bool bTransparent = info.nBkgndIndex != -1;
	bool bAlpha = pAlpha != 0;
	bool bAlphaPalette = info.bAlphaPaletteEnabled != 0;

	RECT mainbox; // (experimental) 
	if (pClipRect){
		GetClipBox(hdc,&mainbox);
		HRGN rgn = CreateRectRgnIndirect(pClipRect);
		HRGN oldrgn = (HRGN)SelectObject(hdc,rgn);
		ExtSelectClipRgn(hdc,rgn,RGN_AND);
		DeleteObject(SelectObject(hdc,oldrgn));
	}

	if (!(bTransparent || bAlpha || bAlphaPalette)){
		if (cx==head.biWidth && cy==head.biHeight){ //NORMAL
			SetStretchBltMode(hdc,COLORONCOLOR);	
			SetDIBitsToDevice(hdc, x, y, cx, cy, 0, 0, 0, cy,
						info.pImage,(BITMAPINFO*)pDib,DIB_RGB_COLORS);
		} else { //STRETCH
			RECT clipbox,paintbox;
			GetClipBox(hdc,&clipbox);
			paintbox.left = min(clipbox.right,max(clipbox.left,x));
			paintbox.right = max(clipbox.left,min(clipbox.right,x+cx));
			paintbox.top = min(clipbox.bottom,max(clipbox.top,y));
			paintbox.bottom = max(clipbox.top,min(clipbox.bottom,y+cy));
			long destw = paintbox.right - paintbox.left;
			long desth = paintbox.bottom - paintbox.top;
			//pixel informations
			RGBQUAD c={0,0,0,0};
			//Preparing Bitmap Info
			BITMAPINFO bmInfo;
			memset(&bmInfo.bmiHeader,0,sizeof(BITMAPINFOHEADER));
			bmInfo.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
			bmInfo.bmiHeader.biWidth=destw;
			bmInfo.bmiHeader.biHeight=desth;
			bmInfo.bmiHeader.biPlanes=1;
			bmInfo.bmiHeader.biBitCount=24;
			BYTE *pbase;	//points to the final dib
			BYTE *pdst;		//current pixel from pbase
			BYTE *ppix;		//current pixel from image
			//get the background
			HDC TmpDC=CreateCompatibleDC(hdc);
			HBITMAP TmpBmp=CreateDIBSection(hdc,&bmInfo,DIB_RGB_COLORS,(void**)&pbase,0,0);
			HGDIOBJ TmpObj=SelectObject(TmpDC,TmpBmp);

			if (pbase){
				long xx,yy,yoffset;
				long ew = ((((24 * destw) + 31) / 32) * 4);
				long ymax = paintbox.bottom;
				long xmin = paintbox.left;
				float fx=(float)head.biWidth/(float)cx;
				float fy=(float)head.biHeight/(float)cy;
				long sx,sy;
				for(yy=0;yy<desth;yy++){
					sy=max(0L,head.biHeight-(long)ceil(((ymax-yy-y)*fy)));
					yoffset=sy*head.biWidth;
					pdst=pbase+yy*ew;
					for(xx=0;xx<destw;xx++){
						sx=(long)floor(((xx+xmin-x)*fx));
						if (head.biClrUsed){
							c=GetPaletteColor(GetPixelIndex(sx,sy));
						} else {
							ppix=info.pImage+sy*info.dwEffWidth+sx*3;
							c.rgbBlue = *ppix++;
							c.rgbGreen= *ppix++;
							c.rgbRed  = *ppix;
						}
						*pdst++=c.rgbBlue;
						*pdst++=c.rgbGreen;
						*pdst++=c.rgbRed;
					}
				}
			}
			//paint the image & cleanup
			SetDIBitsToDevice(hdc,paintbox.left,paintbox.top,destw,desth,0,0,0,desth,pbase,&bmInfo,0);
			DeleteObject(SelectObject(TmpDC,TmpObj));
			DeleteDC(TmpDC);
		}
	} else {	// draw image with transparent/alpha blending
	//////////////////////////////////////////////////////////////////
		//Alpha blend - Thanks to Florian Egel

		//find the smallest area to paint
		RECT clipbox,paintbox;
		GetClipBox(hdc,&clipbox);
		paintbox.left = min(clipbox.right,max(clipbox.left,x));
		paintbox.right = max(clipbox.left,min(clipbox.right,x+cx));
		paintbox.top = min(clipbox.bottom,max(clipbox.top,y));
		paintbox.bottom = max(clipbox.top,min(clipbox.bottom,y+cy));
		long destw = paintbox.right - paintbox.left;
		long desth = paintbox.bottom - paintbox.top;

		//pixel informations
		RGBQUAD c={0,0,0,0};
		RGBQUAD ct = GetTransColor();
		long* pc = (long*)&c;
		long* pct= (long*)&ct;

		//Preparing Bitmap Info
		BITMAPINFO bmInfo;
		memset(&bmInfo.bmiHeader,0,sizeof(BITMAPINFOHEADER));
		bmInfo.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
		bmInfo.bmiHeader.biWidth=destw;
		bmInfo.bmiHeader.biHeight=desth;
		bmInfo.bmiHeader.biPlanes=1;
		bmInfo.bmiHeader.biBitCount=24;

		BYTE *pbase;	//points to the final dib
		BYTE *pdst;		//current pixel from pbase
		BYTE *ppix;		//current pixel from image

		//get the background
		HDC TmpDC=CreateCompatibleDC(hdc);
		HBITMAP TmpBmp=CreateDIBSection(hdc,&bmInfo,DIB_RGB_COLORS,(void**)&pbase,0,0);
		HGDIOBJ TmpObj=SelectObject(TmpDC,TmpBmp);
		BitBlt(TmpDC,0,0,destw,desth,hdc,paintbox.left,paintbox.top,SRCCOPY);

		if (pbase){
			long xx,yy,yoffset,ix,iy;
			BYTE a,a1;
			long ew = ((((24 * destw) + 31) / 32) * 4);
			long ymax = paintbox.bottom;
			long xmin = paintbox.left;

			if (cx!=head.biWidth || cy!=head.biHeight){
				//STRETCH
				float fx=(float)head.biWidth/(float)cx;
				float fy=(float)head.biHeight/(float)cy;
				long sx,sy;
				
				for(yy=0;yy<desth;yy++){
					sy=max(0L,head.biHeight-(long)ceil(((ymax-yy-y)*fy)));
					yoffset=sy*head.biWidth;
					pdst=pbase+yy*ew;
					for(xx=0;xx<destw;xx++){
						sx=(long)floor(((xx+xmin-x)*fx));

						if (bAlpha) a=pAlpha[yoffset+sx]; else a=255;
						a =(BYTE)((a*(1+info.nAlphaMax))>>8);

						if (head.biClrUsed){
							c=GetPaletteColor(GetPixelIndex(sx,sy));
							if (info.bAlphaPaletteEnabled){
								a= (BYTE)((a*(1+c.rgbReserved))>>8);
							}
						} else {
							ppix=info.pImage+sy*info.dwEffWidth+sx*3;
							c.rgbBlue = *ppix++;
							c.rgbGreen= *ppix++;
							c.rgbRed  = *ppix;
						}
						if (*pc!=*pct || !bTransparent){
							// DJT, assume many pixels are fully transparent or opaque and thus avoid multiplication
							if (a == 0) {			// Transparent, retain dest 
								pdst+=3; 
							} else if (a == 255) {	// opaque, ignore dest 
								*pdst++= c.rgbBlue; 
								*pdst++= c.rgbGreen; 
								*pdst++= c.rgbRed; 
							} else {				// semi transparent 
								a1=(BYTE)~a;
								*pdst++=(BYTE)((*pdst * a1 + a * c.rgbBlue)>>8); 
								*pdst++=(BYTE)((*pdst * a1 + a * c.rgbGreen)>>8); 
								*pdst++=(BYTE)((*pdst * a1 + a * c.rgbRed)>>8); 
							} 
						} else {
							pdst+=3;
						}
					}
				}
			} else {
				//NORMAL
				iy=head.biHeight-ymax+y;
				for(yy=0;yy<desth;yy++,iy++){
					yoffset=iy*head.biWidth;
					ix=xmin-x;
					pdst=pbase+yy*ew;
					ppix=info.pImage+iy*info.dwEffWidth+ix*3;
					for(xx=0;xx<destw;xx++,ix++){

						if (bAlpha) a=pAlpha[yoffset+ix]; else a=255;
						a = (BYTE)((a*(1+info.nAlphaMax))>>8);

						if (head.biClrUsed){
							c=GetPaletteColor(GetPixelIndex(ix,iy));
							if (info.bAlphaPaletteEnabled){
								a= (BYTE)((a*(1+c.rgbReserved))>>8);
							}
						} else {
							c.rgbBlue = *ppix++;
							c.rgbGreen= *ppix++;
							c.rgbRed  = *ppix++;
						}

						if (*pc!=*pct || !bTransparent){
							// DJT, assume many pixels are fully transparent or opaque and thus avoid multiplication
							if (a == 0) {			// Transparent, retain dest 
								pdst+=3; 
							} else if (a == 255) {	// opaque, ignore dest 
								*pdst++= c.rgbBlue; 
								*pdst++= c.rgbGreen; 
								*pdst++= c.rgbRed; 
							} else {				// semi transparent 
								a1=(BYTE)~a;
								*pdst++=(BYTE)((*pdst * a1 + a * c.rgbBlue)>>8); 
								*pdst++=(BYTE)((*pdst * a1 + a * c.rgbGreen)>>8); 
								*pdst++=(BYTE)((*pdst * a1 + a * c.rgbRed)>>8); 
							} 
						} else {
							pdst+=3;
						}
					}
				}
			}
		}
		//paint the image & cleanup
		SetDIBitsToDevice(hdc,paintbox.left,paintbox.top,destw,desth,0,0,0,desth,pbase,&bmInfo,0);
		DeleteObject(SelectObject(TmpDC,TmpObj));
		DeleteDC(TmpDC);
	}

	if (pClipRect){  // (experimental)
		HRGN rgn = CreateRectRgnIndirect(&mainbox);
		HRGN oldrgn = (HRGN)SelectObject(hdc,rgn);
		ExtSelectClipRgn(hdc,rgn,RGN_OR);
		DeleteObject(SelectObject(hdc,oldrgn));
	}

	return 1;
}
////////////////////////////////////////////////////////////////////////////////
// Draws (stretch) the image with single transparency support
// > hdc: destination device context
// > x,y: offset
// > cx,cy: (optional) size.
long CxImage::Draw2(HDC hdc, long x, long y, long cx, long cy)
{
	if((pDib==0)||(hdc==0)||(cx==0)||(cy==0)||(info.bEnabled==0)) return 0;
	if (cx < 0) cx = head.biWidth;
	if (cy < 0) cy = head.biHeight;
	bool bTransparent = (info.nBkgndIndex != -1);

	if (!bTransparent){
		SetStretchBltMode(hdc,COLORONCOLOR);	
		StretchDIBits(hdc, x, y, cx, cy, 0, 0, head.biWidth, head.biHeight,
						info.pImage,(BITMAPINFO*)pDib, DIB_RGB_COLORS,SRCCOPY);
	} else {
		// draw image with transparent background
		const int safe = 0; // or else GDI fails in the following - sometimes 
		RECT rcDst = {x+safe, y+safe, x+cx, y+cy};
		if (RectVisible(hdc, &rcDst)){
		/////////////////////////////////////////////////////////////////
			// True Mask Method - Thanks to Paul Reynolds and Ron Gery
			int nWidth = head.biWidth;
			int nHeight = head.biHeight;
			// Create two memory dcs for the image and the mask
			HDC dcImage=CreateCompatibleDC(hdc);
			HDC dcTrans=CreateCompatibleDC(hdc);
			// Select the image into the appropriate dc
			HBITMAP bm = CreateCompatibleBitmap(hdc, nWidth, nHeight);
			HBITMAP pOldBitmapImage = (HBITMAP)SelectObject(dcImage,bm);
			SetStretchBltMode(dcImage,COLORONCOLOR);
			StretchDIBits(dcImage, 0, 0, nWidth, nHeight, 0, 0, nWidth, nHeight,
							info.pImage,(BITMAPINFO*)pDib,DIB_RGB_COLORS,SRCCOPY);

			// Create the mask bitmap
			HBITMAP bitmapTrans = CreateBitmap(nWidth, nHeight, 1, 1, NULL);
			// Select the mask bitmap into the appropriate dc
			HBITMAP pOldBitmapTrans = (HBITMAP)SelectObject(dcTrans, bitmapTrans);
			// Build mask based on transparent colour
			RGBQUAD rgbBG;
			if (head.biBitCount<24) rgbBG = GetPaletteColor((BYTE)info.nBkgndIndex);
			else rgbBG = info.nBkgndColor;
			COLORREF crColour = RGB(rgbBG.rgbRed, rgbBG.rgbGreen, rgbBG.rgbBlue);
			COLORREF crOldBack = SetBkColor(dcImage,crColour);
			BitBlt(dcTrans,0, 0, nWidth, nHeight, dcImage, 0, 0, SRCCOPY);

			// Do the work - True Mask method - cool if not actual display
			StretchBlt(hdc,x, y,cx,cy, dcImage, 0, 0, nWidth, nHeight, SRCINVERT);
			StretchBlt(hdc,x, y,cx,cy, dcTrans, 0, 0, nWidth, nHeight, SRCAND);
			StretchBlt(hdc,x, y,cx,cy, dcImage, 0, 0, nWidth, nHeight, SRCINVERT);

			// Restore settings
			SelectObject(dcImage,pOldBitmapImage);
			SelectObject(dcTrans,pOldBitmapTrans);
			SetBkColor(hdc,crOldBack);
			DeleteObject( bitmapTrans );  // RG 29/01/2002
			DeleteDC(dcImage);
			DeleteDC(dcTrans);
			DeleteObject(bm);
		}
	}
	return 1;
}
////////////////////////////////////////////////////////////////////////////////
// Stretch the image. Obsolete: use Draw() or Draw2()
long CxImage::Stretch(HDC hdc, long xoffset, long yoffset, long xsize, long ysize)
{
	if((pDib)&&(hdc)) {
		//palette must be correctly filled
		SetStretchBltMode(hdc,COLORONCOLOR);	
		StretchDIBits(hdc, xoffset, yoffset,
					xsize, ysize, 0, 0, head.biWidth, head.biHeight,
					info.pImage,(BITMAPINFO*)pDib,DIB_RGB_COLORS,SRCCOPY);
		return 1;
	}
	return 0;
}
////////////////////////////////////////////////////////////////////////////////
// Tiles the device context in the specified rectangle with the image.
long CxImage::Tile(HDC hdc, RECT *rc)
{
	if((pDib)&&(hdc)&&(rc)) {
		int w = rc->right - rc->left;
		int	h = rc->bottom - rc->top;
		int x,y,z;
		int	bx=head.biWidth;
		int	by=head.biHeight;
		for (y = rc->top ; y < h ; y += by){
			if ((y+by)>h) by=h-y;
			z=bx;
			for (x = rc->left ; x < w ; x += z){
				if ((x+z)>w) z=w-x;
				Draw(hdc,x, y);
			}
		}
		return 1;
	}
	return 0;
}
////////////////////////////////////////////////////////////////////////////////
#endif //CXIMAGE_SUPPORT_WINDOWS
////////////////////////////////////////////////////////////////////////////////
