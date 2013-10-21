// Windows specific types and constants:
#pragma once

#include <windows.h>

struct COLOR {unsigned char b,g,r,x;};	// Windows GDI expects 4bytes per color
#define ALIGN sizeof(int)				// Windows GDI expects all int-aligned

// ****************************************************************************
// * C_Image                                                                  *
// *    Storage class for single images                                       *
// ****************************************************************************
class C_Image {
public:
	// standard members:
	int Width, Height;			// Dimensions in pixels
	int BPP;					// Bits Per Pixel	
	char * Raster;				// Bits of Raster Data (Byte Aligned)
	COLOR * Palette;			// Color Map
	int BytesPerRow;			// Width (in bytes) including alignment!
	int Transparent;			// Index of Transparent color (-1 for none)
	// Extra members for animations:
	int xPos, yPos;				// Relative Position
	int Delay;					// Delay after image in 1/1000 seconds.
	int Transparency;			// Animation Transparency.
	// Windows GDI specific:
	BITMAPINFO * pbmi;			// BITMAPINFO structure

	// constructor and destructor:
	C_Image() { Raster=0; Palette=0; pbmi=0; }
	~C_Image() { delete[]pbmi; delete[]Raster; }

	// operator= (object copy)
	C_Image& operator= (C_Image& rhs);

	// Image initializer (allocates space for raster and palette):
	void Init (int iWidth, int iHeight, int iBPP);

	inline char& Pixel (const int& x, const int& y)
		{return Raster[y*BytesPerRow+x];}

	// Windows GDI Specific function to paint the image on a DC:
	int GDIPaint (HDC hdc, int xDest, int yDest);

	// File Formats:
	int LoadBMP (char* szFile);
	int SaveBMP (char* szFile);
};
