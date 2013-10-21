#ifndef __MyPSD_H__
#define __MyPSD_H__

#include "stdio.h"
#include <math.h>

namespace MyPSD {

inline int dti(double value) { return (int)floor(value+.5f); }

class CPSD
{
public:
	// construction, destruction
	CPSD();
	~CPSD();

	int Load(LPCTSTR szPathName);
	void GetLayerInfo(int* layerCount, int* layersPosBuf);

	bool ThumbNailIncluded() const { return mbThumbNailFilled; }
	void DPI(int &x, int &y) const { x = resolution_info.hRes; y = resolution_info.vRes; }
	void Dimensions(int &cx, int &cy) const { cx = header_info.nWidth; cy = header_info.nHeight; }
	int BitsPerPixel() const { return header_info.nBitsPerPixel; }
	int GlobalAngle() const { return mnGlobalAngle; }
	bool IsCopyright() const { return mbCopyright; }
	HBITMAP Detach();

private:
	class HEADER_INFO
	{
//Table 2-12: HeaderInfo Color spaces
//	Color-ID	Name	Description
//-------------------------------------------
//		0		Bitmap			// Probably means black & white
//		1		Grayscale		The first value in the color data is the gray value, from 0...10000.
//		2		Indexed
//		3		RGB				The first three values in the color data are red, green, and blue.
//								They are full unsigned 16–bit values as in Apple’s RGBColor data
//								structure. Pure red=65535,0,0.
//		4		CMYK			The four values in the color data are cyan, magenta, yellow, and
//								black. They are full unsigned 16–bit values. 0=100% ink. Pure
//								cyan=0,65535,65535,65535.
//		7		Multichannel	// Have no idea
//		8		Duotone
//		9		Lab				The first three values in the color data are lightness, a chrominance,
//								and b chrominance.
//								Lightness is a 16–bit value from 0...100. The chromanance components
//								are each 16–bit values from ?28...127. Gray values
//								are represented by chrominance components of 0. Pure
//								white=100,0,0.


		public:
			short nChannels;
			int nHeight;
			int nWidth;
			short nBitsPerPixel;
			short nColourMode;

		HEADER_INFO()
		{
			nChannels = -1;
			nHeight = -1;
			nWidth = -1;
			nBitsPerPixel = -1;
			nColourMode = -1;
		};
	};

	class COLOUR_MODE_DATA
	{
		public:
			int nLength;
			unsigned char* ColourData;

		COLOUR_MODE_DATA()
		{
			nLength = -1;
			ColourData = 0;
		};
	};


	class IMAGE_RESOURCE
	{
// Table 2?: Image resource block
//	Type		Name	Description
//-------------------------------------------
//	OSType		Type	Photoshop always uses its signature, 8BIM
//	int16		ID		Unique identifier
//	PString		Name	A pascal string, padded to make size even (a null name consists of two bytes of 0)
//						Pascal style string where the first byte gives the length of the
//						string and the content bytes follow.
//	int32		Size	Actual size of resource data. This does not include the
//						Type, ID, Name, or Size fields.
//	Variable	Data	Resource data, padded to make size even

		public:
			int nLength;
			char OSType[4];
			short nID;
			unsigned char* Name;
			int	nSize;

		IMAGE_RESOURCE()
		{
			Name = 0;
			Reset();
		};

		void Reset()
		{
			nLength = -1;
			memset( OSType, '\0', sizeof(OSType) );
			nID = -1;
			if ( Name )
				delete[] Name;
			Name = 0;
			nSize = -1;
		};
	};

	class RESOLUTION_INFO
		{
// Table A-6: ResolutionInfo structure
//	Type		Name	Description
//-------------------------------------------
//	Fixed		hRes		Horizontal resolution in pixels per inch.
//	int			hResUnit	1=display horizontal resolution in pixels per inch;
//							2=display horizontal resolution in pixels per cm.
//	short		widthUnit	Display width as 1=inches; 2=cm; 3=points; 4=picas; 5=columns.
//	Fixed		vRes		Vertical resolution in pixels per inch.
//	int			vResUnit	1=display vertical resolution in pixels per inch;
//							2=display vertical resolution in pixels per cm.
//	short		heightUnit	Display height as 1=inches; 2=cm; 3=points; 4=picas; 5=columns.
			public:
				short hRes;
				int hResUnit;
				short widthUnit;

				short vRes;
				int vResUnit;
				short heightUnit;
			RESOLUTION_INFO()
					{
						hRes = -1;
						hResUnit = -1;
						widthUnit = -1;
						vRes = -1;
						vResUnit = -1;
						heightUnit = -1;
					}
		};
	class RESOLUTION_INFO_v2	// Obsolete - Photoshop 2.0
		{
		public:
			short nChannels;
			short nRows;
			short nColumns;
			short nDepth;
			short nMode;
		RESOLUTION_INFO_v2()
			{
				nChannels = -1;
				nRows = -1;
				nColumns = -1;
				nDepth = -1;
				nMode = -1;
			}
		};

	class DISPLAY_INFO
		{
// This structure contains display information about each channel.
//Table A-7: DisplayInfo Color spaces
//	Color-ID	Name	Description
//-------------------------------------------
//		0		RGB			The first three values in the color data are red, green, and blue.
//							They are full unsigned 16–bit values as in Apple’s RGBColor data
//							structure. Pure red=65535,0,0.
//		1		HSB			The first three values in the color data are hue, saturation, and
//							brightness. They are full unsigned 16–bit values as in Apple’s
//							HSVColor data structure. Pure red=0,65535, 65535.
//		2		CMYK		The four values in the color data are cyan, magenta, yellow, and
//							black. They are full unsigned 16–bit values. 0=100% ink. Pure
//							cyan=0,65535,65535,65535.
//		7		Lab			The first three values in the color data are lightness, a chrominance,
//							and b chrominance.
//							Lightness is a 16–bit value from 0...10000. The chromanance components
//							are each 16–bit values from ?2800...12700. Gray values
//							are represented by chrominance components of 0. Pure
//							white=10000,0,0.
//		8		grayscale	The first value in the color data is the gray value, from 0...10000.
		public:
			short ColourSpace;
			short Colour[4];
			short Opacity;			// 0..100
			bool kind;				// selected = 0, protected = 1
			unsigned char padding;	// should be zero
		DISPLAY_INFO()
			{
				ColourSpace = -1;
				for ( unsigned int n = 0; n < 4; ++n)
					Colour[n] = 0;
				Opacity = -1;
				kind = false;
				padding = '0';
			}
		};
	class THUMBNAIL
		{
// Adobe Photoshop 5.0 and later stores thumbnail information for preview
// display in an image resource block. These resource blocks consist of an
// 28 byte header, followed by a JFIF thumbnail in RGB (red, green, blue)
// for both Macintosh and Windows. Adobe Photoshop 4.0 stored the
// thumbnail information in the same format except the data section is
// (blue, green, red). The Adobe Photoshop 4.0 format is at resource ID
// and the Adobe Photoshop 5.0 format is at resource ID 1036.
// Table 2?: Thumnail resource header
//	Type		Name		Description
//-------------------------------------------
//	4 bytes		format			= 1 (kJpegRGB). Also supports kRawRGB (0).
//	4 bytes		width			Width of thumbnail in pixels.
//	4 bytes		height			Height of thumbnail in pixels.
//	4 bytes		widthbytes		Padded row bytes as (width * bitspixel + 31) / 32 * 4.
//	4 bytes		size			Total size as widthbytes * height * planes
//	4 bytes		compressedsize	Size after compression. Used for consistentcy check.
//	2 bytes		bitspixel		= 24. Bits per pixel.
//	2 bytes		planes			= 1. Number of planes.
//	Variable	Data			JFIF data in RGB format.
//								Note: For resource ID 1033 the data is in BGR format.
		public:
			int		nFormat;
			int		nWidth;
			int		nHeight;
			int		nWidthBytes;
			int		nSize;
			int		nCompressedSize;
			short	nBitPerPixel;
			short	nPlanes;
			unsigned char* Data;
			THUMBNAIL()
				{
					nFormat = -1;
					nWidth = -1;
					nHeight = -1;
					nWidthBytes = -1;
					nSize = -1;
					nCompressedSize = -1;
					nBitPerPixel = -1;
					nPlanes = -1;
					Data = 0;
				};
		};


private:
	void CreateDIBSection(	unsigned int cx,
								unsigned int cy,
								unsigned int ppm_x = 3780,
								unsigned int ppm_y = 3780,
								WORD BitCount = 24
							);

	int Calculate(unsigned char* c, int nDigits);
	void XYZToRGB(const double X, const double Y, const double Z, int &R, int &G, int &B);
	void LabToRGB(const int L, const int a, const int b, int &R, int &G, int &B );
	void CMYKToRGB(const double C, const double M, const double Y, const double K, int &R, int &G, int &B);

	bool ReadHeader(FILE* pFile, HEADER_INFO& header_info);
	bool ReadColourModeData(FILE* pFile, COLOUR_MODE_DATA& colour_mode_data);
	bool ReadImageResource(FILE* pFile, IMAGE_RESOURCE& image_resource);
	bool ReadLayerAndMaskInfoSection(FILE* pFile); // Actually ignore it
	int ReadImageData(FILE* pFile);
	int Read4Bytes(FILE* pFile);
	int Read2Bytes(FILE* pFile);
	void ReadSomeBytes(FILE* pFile, int nCount);
	void ReadString(FILE* pFile);

	void ProccessBuffer(unsigned char* pData = 0);

private:
	HBITMAP mhBitmap;
	bool mbDetached;

	HEADER_INFO header_info;

	COLOUR_MODE_DATA colour_mode_data;
	short mnColourCount;
	short mnTransparentIndex;

	IMAGE_RESOURCE image_resource;

	int		mnGlobalAngle;

	RESOLUTION_INFO resolution_info;
	bool	mbResolutionInfoFilled;

	RESOLUTION_INFO_v2 resolution_info_v2;
	bool	mbResolutionInfoFilled_v2;

	DISPLAY_INFO display_info;
	bool	mbDisplayInfoFilled;

	THUMBNAIL thumbnail;
	bool	mbThumbNailFilled;

	bool	mbCopyright;

	// layer-info
	bool    mbFound2ndLayer;
	int		mLayerCount;
	int		mTopBig;
	int		mLeftBig;
	int		mBottomBig;
	int		mRightBig;
	int		mTopSmall;
	int		mLeftSmall;
	int		mBottomSmall;
	int		mRightSmall;
};

};

#endif // __MyPSD_H__