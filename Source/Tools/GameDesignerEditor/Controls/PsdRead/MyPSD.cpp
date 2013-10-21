#include "StdAfx.h"
#include "MyPSD.h"

#include <string>

#include <assert.h>

namespace MyPSD
{

CPSD::CPSD()
{
	mhBitmap = 0;
	mbDetached = false;

	mbThumbNailFilled = false;
	mbDisplayInfoFilled = false;
	mbResolutionInfoFilled = false;
	mbResolutionInfoFilled_v2 = false;
	mnGlobalAngle = 30;
	mbCopyright = false;
	mnColourCount = -1;
	mnTransparentIndex = -1;
}
CPSD::~CPSD()
{
    if( mhBitmap && !mbDetached )
		::DeleteObject(mhBitmap);

	// free memory
	if ( 0 < colour_mode_data.nLength )
		delete[] colour_mode_data.ColourData;
	colour_mode_data.ColourData = 0;

	if ( image_resource.Name )
		delete[] image_resource.Name;
	image_resource.Name = 0;
}

int CPSD::Calculate(unsigned char* c, int nDigits)
{
	int nValue = 0;

	for(int n = 0; n < nDigits; ++n)
		nValue = ( nValue << 8 ) | *(c+n);

	return nValue;
};

void CPSD::XYZToRGB(const double X, const double Y, const double Z, int &R, int &G, int &B)
{
	// Standards used Observer = 2, Illuminant = D65
	// ref_X = 95.047, ref_Y = 100.000, ref_Z = 108.883
	const double ref_X = 95.047;
	const double ref_Y = 100.000;
	const double ref_Z = 108.883;

	double var_X = X / 100.0;
	double var_Y = Y / 100.0;
	double var_Z = Z / 100.0;

	double var_R = var_X * 3.2406 + var_Y * (-1.5372) + var_Z * (-0.4986);
	double var_G = var_X * (-0.9689) + var_Y * 1.8758 + var_Z * 0.0415;
	double var_B = var_X * 0.0557 + var_Y * (-0.2040) + var_Z * 1.0570;

	if ( var_R > 0.0031308 )
		var_R = 1.055 * ( pow(var_R, 1/2.4) ) - 0.055;
	else
		var_R = 12.92 * var_R;

	if ( var_G > 0.0031308 )
		var_G = 1.055 * ( pow(var_G, 1/2.4) ) - 0.055;
	else
		var_G = 12.92 * var_G;

	if ( var_B > 0.0031308 )
		var_B = 1.055 * ( pow(var_B, 1/2.4) )- 0.055;
	else
		var_B = 12.92 * var_B;

	R = (int)(var_R * 256.0);
	G = (int)(var_G * 256.0);
	B = (int)(var_B * 256.0);
};

void CPSD::LabToRGB(const int L, const int a, const int b, int &R, int &G, int &B )
{
	// For the conversion we first convert values to XYZ and then to RGB
	// Standards used Observer = 2, Illuminant = D65
	// ref_X = 95.047, ref_Y = 100.000, ref_Z = 108.883
	const double ref_X = 95.047;
	const double ref_Y = 100.000;
	const double ref_Z = 108.883;

	double var_Y = ( (double)L + 16.0 ) / 116.0;
	double var_X = (double)a / 500.0 + var_Y;
	double var_Z = var_Y - (double)b / 200.0;

	if ( pow(var_Y, 3) > 0.008856 )
		var_Y = pow(var_Y, 3);
	else
		var_Y = ( var_Y - 16 / 116 ) / 7.787;

	if ( pow(var_X, 3) > 0.008856 )
		var_X = pow(var_X, 3);
	else
		var_X = ( var_X - 16 / 116 ) / 7.787;

	if ( pow(var_Z, 3) > 0.008856 )
		var_Z = pow(var_Z, 3);
	else
		var_Z = ( var_Z - 16 / 116 ) / 7.787;

	double X = ref_X * var_X;
	double Y = ref_Y * var_Y;
	double Z = ref_Z * var_Z;

	XYZToRGB(X, Y, Z, R, G, B);
};

void CPSD::CMYKToRGB(const double C, const double M, const double Y, const double K, int &R, int &G, int &B )
{
	R = dti( ( 1.0f - ( C *( 1.0f - K ) + K ) ) * 255.0f );
	G = dti( ( 1.0f - ( M *( 1.0f - K ) + K ) ) * 255.0f );
	B = dti( ( 1.0f - ( Y *( 1.0f - K ) + K ) ) * 255.0f );
};
void CPSD::CreateDIBSection(	unsigned int cx,
												unsigned int cy,
												unsigned int ppm_x,
												unsigned int ppm_y,
												WORD BitCount
											)
{
	HDC hDC = ::GetDC(NULL);
	
	if (!hDC) return;

	LPVOID pvBits;
	BITMAPINFO BitmapInfo;

	BitmapInfo.bmiHeader.biSize               = sizeof(BITMAPINFOHEADER);
	BitmapInfo.bmiHeader.biWidth              = cx;
	BitmapInfo.bmiHeader.biHeight             = cy;
	BitmapInfo.bmiHeader.biPlanes             = 1;
	BitmapInfo.bmiHeader.biBitCount           = BitCount;
	BitmapInfo.bmiHeader.biCompression        = BI_RGB;
	BitmapInfo.bmiHeader.biSizeImage          = 0;
	BitmapInfo.bmiHeader.biXPelsPerMeter      = ppm_x; 
	BitmapInfo.bmiHeader.biYPelsPerMeter      = ppm_y;
	BitmapInfo.bmiHeader.biClrImportant       = 0;
	BitmapInfo.bmiHeader.biClrUsed            = 0;

	mhBitmap = ::CreateDIBSection( hDC,(BITMAPINFO*)&BitmapInfo, DIB_RGB_COLORS, &pvBits, NULL, 0L );

    if( mhBitmap == 0 )  
	{
		ReleaseDC(NULL,hDC);
		return;
	}
	else
	{
		HDC hdcMemory;
		HBITMAP hbmpOld;
		RECT rc;
		hdcMemory = ::CreateCompatibleDC(hDC);
		hbmpOld = (HBITMAP)::SelectObject( hdcMemory, mhBitmap );

		rc.left = rc.top = 0;
		rc.right = cx;
		rc.bottom = cy;
		::FillRect( hdcMemory, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH) );
		::SelectObject( hdcMemory, hbmpOld );
		::DeleteDC( hdcMemory );
	}
	::ReleaseDC( NULL,hDC );

	return;
};

int CPSD::Read4Bytes(FILE* pFile)
{
 	unsigned char data[4];
 	int nItemsRead = (int)(int)fread(&data, sizeof(data), 1, pFile);
 	return Calculate( data, sizeof(data) );	
}

int CPSD::Read2Bytes(FILE* pFile)
{
	unsigned char data[2];
	int nItemsRead = (int)(int)fread(&data, sizeof(data), 1, pFile);
	return Calculate( data, sizeof(data) );	
}

void CPSD::ReadSomeBytes(FILE* pFile, int nCount)
{
	unsigned char data[1];
	int nBytesRead = 0;
	int nItemsRead = 0;
	while( !feof( pFile ) && ( nBytesRead < nCount ) )
	{
		data[0] = '\0';
		nItemsRead = (int)(int)fread(&data, sizeof(data), 1, pFile);
		nBytesRead += nItemsRead * sizeof(data);
	}
}

void CPSD::ReadString(FILE* pFile)
{
	unsigned char data[1];
	int nBytesRead = 0;
	int nItemsRead = 0;
	while( !feof( pFile ) )
	{
		data[0] = '\0';
		nItemsRead = (int)(int)fread(&data, sizeof(data), 1, pFile);
		nBytesRead += nItemsRead * sizeof(data);
		int _result = Calculate( data, sizeof(data) );	
		if (_result == 0)
			break;
	}
	ReadSomeBytes(pFile, nBytesRead % 4);
}

void CPSD::GetLayerInfo(int* layerCount, int* layersPosBuf)
{
	*layerCount = mLayerCount;
	*layersPosBuf++ = mTopBig;
	*layersPosBuf++ = mLeftBig;
	*layersPosBuf++ = mBottomBig;
	*layersPosBuf++ = mRightBig;

	if (mbFound2ndLayer)
	{
		*layersPosBuf++ = mTopSmall;
		*layersPosBuf++ = mLeftSmall;
		*layersPosBuf++ = mBottomSmall;
		*layersPosBuf = mRightSmall;		
	}
}

bool CPSD::ReadLayerAndMaskInfoSection(FILE* pFile)	// Actually ignore it
{
	bool bSuccess = false;

	unsigned char DataLength[4];
	int nBytesRead = 0;
	int nItemsRead = (int)(int)fread(&DataLength, sizeof(DataLength), 1, pFile);
	
	int nTotalBytes = Calculate( DataLength, sizeof(DataLength) );

 	int nWhat = Read4Bytes(pFile);
 	mLayerCount = abs((short)Read2Bytes(pFile));

	if (mLayerCount == 0) // 只有1个图层，我kao
	{
		fpos_t FPOS = 0;
		fsetpos(pFile, (&FPOS));

		nWhat = Read4Bytes(pFile);
		nWhat = Read4Bytes(pFile);
		nWhat = Read4Bytes(pFile);
		nWhat = Read2Bytes(pFile);

		mTopBig = 0;
		mLeftBig = 0;
		mBottomBig = Read4Bytes(pFile);
		mRightBig = Read4Bytes(pFile);
		mLayerCount = 1;
		return false;
	}



 	int nTop = Read4Bytes(pFile);
 	int nLeft = Read4Bytes(pFile);
 	int nBottom = Read4Bytes(pFile);
 	int nRight = Read4Bytes(pFile);

	mTopBig = nTop;
	mLeftBig = nLeft;
	mBottomBig = nBottom;
	mRightBig = nRight;

// 	nWhat = Read2Bytes(pFile); // numbers of channels
// 	ReadSomeBytes(pFile, nWhat * 6);
// 	ReadSomeBytes(pFile, 16);
// 	nWhat = Read4Bytes(pFile);
// 	if (nWhat != 0)
// 		ReadSomeBytes(pFile, 20);
// 
// 	nWhat = Read4Bytes(pFile);
// 	ReadSomeBytes(pFile, 8 + 8 * nWhat);
// 	ReadString(pFile);

	mbFound2ndLayer = false;

	fpos_t old_found_pos = 0;

	while (!feof( pFile ))
	{
FIND_AG:
		while (!feof( pFile )) // 找签名
		{
			unsigned char data[4];
			fread(&data, sizeof(data), 1, pFile);
			int _result = Calculate( data, sizeof(data) );	
			if (_result == 943868237)
				break;
		}

		fpos_t FPOS = 0;
		fgetpos(pFile, (&FPOS));

		if (FPOS > old_found_pos)
		{
			old_found_pos = FPOS;
		}
		else if (feof( pFile ))
		{
			break;
		}
		else
		{
			goto FIND_AG;
		}

		FPOS -= 46;
		fsetpos(pFile, &FPOS);

		int thisTop = Read4Bytes(pFile);
		int thisLeft = Read4Bytes(pFile);
		int thisBottom = Read4Bytes(pFile);
		int thisRight = Read4Bytes(pFile);

		//if (thisTop < 0 && thisTop > -600)
		//	thisTop = 0;

		if (/*thisTop >= nTop &&*/ thisTop < thisBottom && thisBottom <= nBottom &&
			/*thisLeft >= nLeft &&*/ thisLeft < thisRight /*&& thisRight <= nRight*/ &&
			thisBottom < 10000 && thisRight < 10000
			)
		{
			mbFound2ndLayer = true;
			mTopBig = nTop;
			mLeftBig = nLeft;
			mBottomBig = nBottom;
			mRightBig = nRight;

			mTopSmall = thisTop;
			mLeftSmall = thisLeft>0 ? thisLeft : 0;
			mBottomSmall = thisBottom;
			mRightSmall = thisRight<mRightBig ? thisRight : mRightBig;
			return true;
		}

		if (feof( pFile ))
		{
			return false;
		}
	}

	if (!mbFound2ndLayer)
	{
		return false;
	}

	unsigned char data[2];
	int what = 0;
	while( !feof( pFile ) && ( nBytesRead < nTotalBytes ) )
		{
			data[0] = '\0';
			nItemsRead = (int)(int)fread(&data, sizeof(data), 1, pFile);
			nBytesRead += nItemsRead * sizeof(data);
			what = Calculate( data, sizeof(data) );
		}

	//assert ( nBytesRead == nTotalBytes );
	if ( nBytesRead == nTotalBytes )
		bSuccess = true;

	return bSuccess;
}
bool CPSD::ReadImageResource(FILE* pFile, IMAGE_RESOURCE& image_resource)
{
	bool bSuccess = false;

	unsigned char Length[4];
	int nItemsRead = (int)(int)fread(&Length, sizeof(Length), 1, pFile);

	image_resource.nLength = Calculate( Length, sizeof(image_resource.nLength) );

	int nBytesRead = 0;
	int nTotalBytes = image_resource.nLength;

	while( !feof( pFile ) && ( nBytesRead < nTotalBytes ) )
		{
			nItemsRead = 0;
			image_resource.Reset();

			nItemsRead = (int)(int)fread(&image_resource.OSType, sizeof(image_resource.OSType), 1, pFile);
			nBytesRead += nItemsRead * sizeof(image_resource.OSType);

			std::string strOSType;
			for(unsigned int nChar = 0; nChar < 4; ++nChar)
				strOSType.push_back(image_resource.OSType[nChar]);

			assert ( 0 == (nBytesRead % 2) );
			if ( 0 == strOSType.compare("8BIM") )
			{
				unsigned char ID[2];
				nItemsRead = (int)(int)fread(&ID, sizeof(ID), 1, pFile);
				nBytesRead += nItemsRead * sizeof(ID);

				image_resource.nID = (short)Calculate( ID, sizeof(ID) );

				unsigned char SizeOfName;
				nItemsRead = (int)(int)fread(&SizeOfName, sizeof(SizeOfName), 1, pFile);
				nBytesRead += nItemsRead * sizeof(SizeOfName);

				int nSizeOfName = Calculate( &SizeOfName, sizeof(SizeOfName) );
				if ( 0 < nSizeOfName )
				{
					image_resource.Name = new unsigned char[nSizeOfName];
					nItemsRead = (int)(int)fread(image_resource.Name, nSizeOfName, 1, pFile);
					nBytesRead += nItemsRead * nSizeOfName;
				}

				if ( 0 == (nSizeOfName % 2) )
				{
					nItemsRead = (int)(int)fread(&SizeOfName, sizeof(SizeOfName), 1, pFile);
					nBytesRead += nItemsRead * sizeof(SizeOfName);
				}

				unsigned char Size[4];
				nItemsRead = (int)(int)fread(&Size, sizeof(Size), 1, pFile);
				nBytesRead += nItemsRead * sizeof(Size);

				image_resource.nSize = Calculate( Size, sizeof(image_resource.nSize) );

				if ( 0 != (image_resource.nSize % 2) )	// resource data must be even
					image_resource.nSize++;
				if ( 0 < image_resource.nSize )
				{
					unsigned char IntValue[4];
					unsigned char ShortValue[2];

					switch( image_resource.nID )
					{
						case 1000:
							{
								// Obsolete - Photoshop 2.0
								mbResolutionInfoFilled_v2 = true;

								nItemsRead = (int)(int)fread(&ShortValue, sizeof(ShortValue), 1, pFile);
								nBytesRead += nItemsRead * sizeof(ShortValue);
								resolution_info_v2.nChannels = (short)Calculate(ShortValue, sizeof(resolution_info_v2.nChannels) );
								nItemsRead = (int)(int)fread(&ShortValue, sizeof(ShortValue), 1, pFile);
								nBytesRead += nItemsRead * sizeof(ShortValue);
								resolution_info_v2.nRows = (short)Calculate(ShortValue, sizeof(resolution_info_v2.nRows) );
								nItemsRead = (int)(int)fread(&ShortValue, sizeof(ShortValue), 1, pFile);
								nBytesRead += nItemsRead * sizeof(ShortValue);
								resolution_info_v2.nColumns = (short)Calculate(ShortValue, sizeof(resolution_info_v2.nColumns) );
								nItemsRead = (int)(int)fread(&ShortValue, sizeof(ShortValue), 1, pFile);
								nBytesRead += nItemsRead * sizeof(ShortValue);
								resolution_info_v2.nDepth = (short)Calculate(ShortValue, sizeof(resolution_info_v2.nDepth) );
								nItemsRead = (int)(int)fread(&ShortValue, sizeof(ShortValue), 1, pFile);
								nBytesRead += nItemsRead * sizeof(ShortValue);
								resolution_info_v2.nMode = (short)Calculate(ShortValue, sizeof(resolution_info_v2.nMode) );
							}
						break;
						case 1005:
						{
							mbResolutionInfoFilled = true;

							nItemsRead = (int)(int)fread(&ShortValue, sizeof(ShortValue), 1, pFile);
							nBytesRead += nItemsRead * sizeof(ShortValue);
							resolution_info.hRes = (short)Calculate(ShortValue, sizeof(resolution_info.hRes) );
							nItemsRead = (int)(int)fread(&IntValue, sizeof(IntValue), 1, pFile);
							nBytesRead += nItemsRead * sizeof(IntValue);
							resolution_info.hResUnit = Calculate(IntValue, sizeof(resolution_info.hResUnit) );
							nItemsRead = (int)(int)fread(&ShortValue, sizeof(ShortValue), 1, pFile);
							nBytesRead += nItemsRead * sizeof(ShortValue);
							resolution_info.widthUnit = (short)Calculate(ShortValue, sizeof(resolution_info.widthUnit) );

							nItemsRead = (int)(int)fread(&ShortValue, sizeof(ShortValue), 1, pFile);
							nBytesRead += nItemsRead * sizeof(ShortValue);
							resolution_info.vRes = (short)Calculate(ShortValue, sizeof(resolution_info.vRes) );
							nItemsRead = (int)(int)fread(&IntValue, sizeof(IntValue), 1, pFile);
							nBytesRead += nItemsRead * sizeof(IntValue);
							resolution_info.vResUnit = Calculate(IntValue, sizeof(resolution_info.vResUnit) );
							nItemsRead = (int)fread(&ShortValue, sizeof(ShortValue), 1, pFile);
							nBytesRead += nItemsRead * sizeof(ShortValue);
							resolution_info.heightUnit = (short)Calculate(ShortValue, sizeof(resolution_info.heightUnit) );
						}
						break;
						case 1007:
							{
								mbDisplayInfoFilled = true;

								nItemsRead = (int)fread(&ShortValue, sizeof(ShortValue), 1, pFile);
								nBytesRead += nItemsRead * sizeof(ShortValue);
								display_info.ColourSpace = (short)Calculate(ShortValue, sizeof(display_info.ColourSpace) );

								for ( unsigned int n = 0; n < 4; ++n )
									{
										nItemsRead = (int)fread(&ShortValue, sizeof(ShortValue), 1, pFile);
										nBytesRead += nItemsRead * sizeof(ShortValue);
										display_info.Colour[n] = (short)Calculate(ShortValue, sizeof(display_info.Colour[n]) );
									}
				
								nItemsRead = (int)fread(&ShortValue, sizeof(ShortValue), 1, pFile);
								nBytesRead += nItemsRead * sizeof(ShortValue);
								display_info.Opacity = (short)Calculate(ShortValue, sizeof(display_info.Opacity) );
								assert ( 0 <= display_info.Opacity );
								assert ( 100 >= display_info.Opacity );

								unsigned char c[1];
								nItemsRead = (int)fread(&c, sizeof(c), 1, pFile);
								nBytesRead += nItemsRead * sizeof(c);
								( 1 == Calculate(c, sizeof(c) ) ) ? display_info.kind = true : display_info.kind = false;

								nItemsRead = (int)fread(&c, sizeof(c), 1, pFile);
								nBytesRead += nItemsRead * sizeof(c);
								display_info.padding = (unsigned int)Calculate(c, sizeof(c) );
								assert ( 0 == display_info.padding );
							}
						break;
						case 1034:
							{
								nItemsRead = (int)fread(&ShortValue, sizeof(ShortValue), 1, pFile);
								nBytesRead += nItemsRead * sizeof(ShortValue);
								( 1 == Calculate(ShortValue, sizeof(ShortValue) ) ) ? mbCopyright = true : mbCopyright = false;
							}
						break;
						case 1033:
						case 1036:
							{
								mbThumbNailFilled = true;

								nItemsRead = (int)fread(&IntValue, sizeof(IntValue), 1, pFile);
								nBytesRead += nItemsRead * sizeof(IntValue);
								thumbnail.nFormat = Calculate(IntValue, sizeof(thumbnail.nFormat) );

								nItemsRead = (int)fread(&IntValue, sizeof(IntValue), 1, pFile);
								nBytesRead += nItemsRead * sizeof(IntValue);
								thumbnail.nWidth = Calculate(IntValue, sizeof(thumbnail.nWidth) );

								nItemsRead = (int)fread(&IntValue, sizeof(IntValue), 1, pFile);
								nBytesRead += nItemsRead * sizeof(IntValue);
								thumbnail.nHeight = Calculate(IntValue, sizeof(thumbnail.nHeight) );

								nItemsRead = (int)fread(&IntValue, sizeof(IntValue), 1, pFile);
								nBytesRead += nItemsRead * sizeof(IntValue);
								thumbnail.nWidthBytes = Calculate(IntValue, sizeof(thumbnail.nWidthBytes) );

								nItemsRead = (int)fread(&IntValue, sizeof(IntValue), 1, pFile);
								nBytesRead += nItemsRead * sizeof(IntValue);
								thumbnail.nSize = Calculate(IntValue, sizeof(thumbnail.nSize) );
								
								nItemsRead = (int)fread(&IntValue, sizeof(IntValue), 1, pFile);
								nBytesRead += nItemsRead * sizeof(IntValue);
								thumbnail.nCompressedSize = Calculate(IntValue, sizeof(thumbnail.nCompressedSize) );

								nItemsRead = (int)fread(&ShortValue, sizeof(ShortValue), 1, pFile);
								nBytesRead += nItemsRead * sizeof(ShortValue);
								thumbnail.nBitPerPixel = (short)Calculate(ShortValue, sizeof(thumbnail.nBitPerPixel) );

								nItemsRead = (int)fread(&ShortValue, sizeof(ShortValue), 1, pFile);
								nBytesRead += nItemsRead * sizeof(ShortValue);
								thumbnail.nPlanes = (short)Calculate(ShortValue, sizeof(thumbnail.nPlanes) );

								int nTotalData = image_resource.nSize - 28; // header
								unsigned char* buffer = new unsigned char[nTotalData];
								unsigned char c[1];
								if ( 1033 == image_resource.nID )
									{
									// In BGR format
										for (int n = 0; n < nTotalData; n = n +3 )
											{
												nItemsRead = (int)fread(&c, sizeof(unsigned char), 1, pFile);
												nBytesRead += nItemsRead * sizeof(unsigned char);
												buffer[n+2] = (unsigned char)Calculate(c, sizeof(unsigned char) );
												nItemsRead = (int)fread(&c, sizeof(unsigned char), 1, pFile);
												nBytesRead += nItemsRead * sizeof(unsigned char);
												buffer[n+1] = (unsigned char)Calculate(c, sizeof(BYTE) );
												nItemsRead = (int)fread(&c, sizeof(unsigned char), 1, pFile);
												nBytesRead += nItemsRead * sizeof(unsigned char);
												buffer[n] = (unsigned char)Calculate(c, sizeof(unsigned char) );
											}
									}
								else if ( 1036 == image_resource.nID )
									{
										// In RGB format										
										for (int n = 0; n < nTotalData; ++n )
											{
												nItemsRead = (int)fread(&c, sizeof(BYTE), 1, pFile);
												nBytesRead += nItemsRead * sizeof(BYTE);
												buffer[n] = (BYTE)Calculate(c, sizeof(BYTE) );
											}
									}

								delete[] buffer;
								buffer = 0;
							}
						break;
						case 1037:
							{
								nItemsRead = (int)fread(&IntValue, sizeof(IntValue), 1, pFile);
								nBytesRead += nItemsRead * sizeof(IntValue);
								mnGlobalAngle = Calculate(IntValue, sizeof(mnGlobalAngle) );
							}
						break;
						case 1046:
							{
								nItemsRead = (int)fread(&ShortValue, sizeof(ShortValue), 1, pFile);
								nBytesRead += nItemsRead * sizeof(ShortValue);
								mnColourCount = (short)Calculate(ShortValue, sizeof(ShortValue) );
							}
						break;
						case 1047:
							{
								nItemsRead = (int)fread(&ShortValue, sizeof(ShortValue), 1, pFile);
								nBytesRead += nItemsRead * sizeof(ShortValue);
								mnTransparentIndex = (short)Calculate(ShortValue, sizeof(ShortValue) );
							}
						break;

						default:
							{
							unsigned char c[1];
							for( int n = 0; n < image_resource.nSize; ++n )
								{
									nItemsRead = (int)fread(&c, sizeof(c), 1, pFile);
									nBytesRead += nItemsRead * sizeof(c);
								}
							}
						break;
					}
				}
			}
		}

	assert ( nBytesRead == nTotalBytes );
	if ( nBytesRead == nTotalBytes )
		bSuccess = true;

	return bSuccess;
}
bool CPSD::ReadColourModeData(FILE* pFile, COLOUR_MODE_DATA& colour_mode_data)
{
	// Only indexed colour and duotone have colour mode data,
	// for all other modes this section is 4 bytes length, the length field is set to zero

	// For indexed color images, the length will be equal to 768, and the color
	// will contain the color table for the image, in non杋nterleaved order.

	// For duotone images, the color data will contain the duotone specification,
	// the format of which is not documented. Other applications that read
	// Photoshop files can treat a duotone image as a grayscale image, and just
	// preserve the contents of the duotone information when reading and writing
	// the file.

	// free memory
	if ( 0 < colour_mode_data.nLength )
		delete[] colour_mode_data.ColourData;
	colour_mode_data.ColourData = 0;

	unsigned char Length[4];
	int nItemsRead = (int)fread(&Length, sizeof(Length), 1, pFile);

	colour_mode_data.nLength = Calculate( Length, sizeof(colour_mode_data.nLength) );
	if ( 0 < colour_mode_data.nLength )
		{
			colour_mode_data.ColourData = new unsigned char[colour_mode_data.nLength];
			nItemsRead = 0;
			memset(colour_mode_data.ColourData, 254, colour_mode_data.nLength);

			nItemsRead += (int)fread( colour_mode_data.ColourData, colour_mode_data.nLength, 1, pFile);

		}

	return true;
}

bool CPSD::ReadHeader(FILE* pFile, HEADER_INFO& header_info)
{
	bool bSuccess = false;

	struct HEADER
	{
		char Signature[4];	// always equal 8BPS, do not read file if not
		unsigned char Version[2];	// always equal 1, do not read file if not
		char Reserved[6];	// must be zero
		unsigned char Channels[2];	// numer of channels including any alpha channels, supported range 1 to 24
		unsigned char Rows[4];		// height in PIXELS, supported range 1 to 30000
		unsigned char Columns[4];	// width in PIXELS, supported range 1 to 30000
		unsigned char Depth[2];		// number of bpp
		unsigned char Mode[2];		// colour mode of the file,
							// Btmap=0, Grayscale=1, Indexed=2, RGB=3,
							// CMYK=4, Multichannel=7, Duotone=8, Lab=9
	};

	HEADER header;
	int nItemsRead = (int)fread(&header, sizeof(HEADER), 1, pFile);
	if ( nItemsRead )
	{
		std::string strSignature;
		for(unsigned int nChar = 0; nChar < 4; ++nChar)
			strSignature.push_back(header.Signature[nChar]);

		if ( 0 == strSignature.compare("8BPS") )
		{
			int nVersion = Calculate( header.Version, sizeof(header.Version) );

			if ( 1 == nVersion )
			{
				unsigned int n = 0;
				bool bOK = true;
				while ( (n < 6) && bOK )
					{
						if ( '\0' != header.Reserved[n] )
							bOK = false;
						n++;
					}
				bSuccess = bOK;

				if ( bSuccess )
				{
					header_info.nChannels = (short)Calculate( header.Channels, sizeof(header.Channels) );
					header_info.nHeight = Calculate( header.Rows, sizeof(header.Rows) );
					header_info.nWidth = Calculate( header.Columns, sizeof(header.Columns) );
					header_info.nBitsPerPixel = (short)Calculate( header.Depth, sizeof(header.Depth) );
					header_info.nColourMode = (short)Calculate( header.Mode, sizeof(header.Mode) );
				}
			}
		}
	}

	return bSuccess;
}


void CPSD::ProccessBuffer(unsigned char* pData )
{
	assert ( 0 != pData );
	if ( !pData )
		return;

	assert ( 0 != mhBitmap );
	if ( !mhBitmap )
		return;

	HBITMAP hBitmap = mhBitmap;

	HDC hdcMemory = 0;
	HBITMAP hbmpOld = 0;

	int nHeight = header_info.nHeight;
	int nWidth = header_info.nWidth;
	short bytesPerPixelPerChannel = header_info.nBitsPerPixel / 8;
	int nPixels = nWidth * nHeight;
	int nTotalBytes = nPixels * bytesPerPixelPerChannel * header_info.nChannels;

	switch ( header_info.nColourMode )
	{
		case 1:		// Grayscale
		case 8:		// Duotone
		{
			hdcMemory = ::CreateCompatibleDC(NULL);
			hbmpOld = (HBITMAP)::SelectObject( hdcMemory, hBitmap );

			unsigned int nValue = 0;

			int nRow = 0;
			int nCol = 0;

			for( int nCounter = 0; nCounter < nTotalBytes; nCounter = nCounter + header_info.nChannels * bytesPerPixelPerChannel )
			{
				nValue = (unsigned int)Calculate(pData+nCounter, bytesPerPixelPerChannel );
				if ( 16 == header_info.nBitsPerPixel )
					nValue = nValue / 256;

				::SetPixel( hdcMemory, nCol, nRow, RGB(nValue, nValue, nValue) );
				nCol++;
				if ( nWidth <= nCol )
				{
					nCol = 0;
					nRow++;
				}
			}

			::SelectObject( hdcMemory, hbmpOld );
			::DeleteDC( hdcMemory );                                      
		}
		break;
		case 2:		// Indexed
		{
			hdcMemory = ::CreateCompatibleDC(NULL);
			hbmpOld = (HBITMAP)::SelectObject( hdcMemory, hBitmap );
			// pData holds the indices of loop through the palette and set the correct RGB
			// 8bpp are supported
			assert ( (0!=colour_mode_data.ColourData) && (768==colour_mode_data.nLength) && (0<mnColourCount) );
			if ( (0!=colour_mode_data.ColourData) && (768==colour_mode_data.nLength) && (0<mnColourCount) )
			{
				int nRow = 0;
				int nCol = 0;
				int nRed = 0;
				int nGreen = 0;
				int nBlue = 0;
				unsigned int nIndex = 0;

				for( int nCounter = 0; nCounter < nTotalBytes; ++nCounter )
				{
					nIndex = (unsigned int)Calculate(pData+nCounter, 1);
					nRed = (int)Calculate(colour_mode_data.ColourData+nIndex, 1 );
					nGreen = (int)Calculate(colour_mode_data.ColourData+nIndex+256, 1 );
					nBlue = (int)Calculate(colour_mode_data.ColourData+nIndex+2*256, 1 );

					::SetPixel( hdcMemory, nCol, nRow, RGB(nRed, nGreen, nBlue) );
					nCol++;
					if ( nWidth <= nCol )
					{
						nCol = 0;
						nRow++;
					}
				}
			}

			::SelectObject( hdcMemory, hbmpOld );
			::DeleteDC( hdcMemory );                                      

		}
		break;
		case 3:	// RGB
		{
			hdcMemory = ::CreateCompatibleDC(NULL);
			hbmpOld = (HBITMAP)::SelectObject( hdcMemory, hBitmap );

			int nBytesToRead = header_info.nBitsPerPixel / 8;
			if ( 2 == nBytesToRead )
				nBytesToRead = 1;

			int nRow = 0;
			int nCol = 0;
			int nRed = 0;
			int nGreen = 0;
			int nBlue = 0;

			for( int nCounter = 0; nCounter < nTotalBytes; nCounter = nCounter + header_info.nChannels * nBytesToRead )
			{
				nRed = (int)Calculate(pData+nCounter, nBytesToRead);
				nGreen = (int)Calculate(pData+nCounter+nBytesToRead, nBytesToRead);
				nBlue = (int)Calculate(pData+nCounter+2*nBytesToRead, nBytesToRead);

				::SetPixel( hdcMemory, nCol, nRow, RGB(nRed, nGreen, nBlue) );
				nCol++;
				if ( nWidth <= nCol )
				{
					nCol = 0;
					nRow++;
				}
			}

			::SelectObject( hdcMemory, hbmpOld );
			::DeleteDC( hdcMemory );                                      
		}
		break;
		case 4:	// CMYK
		{
			hdcMemory = ::CreateCompatibleDC(NULL);
			hbmpOld = (HBITMAP)::SelectObject( hdcMemory, hBitmap );

			int nBytesToRead = header_info.nBitsPerPixel / 8;

			int nRed = 0;
			int nGreen = 0;
			int nBlue = 0;
			double C, M, Y, K;

			int nRow = 0;
			int nCol = 0;

			double dMaxColours = pow( 2.0f, header_info.nBitsPerPixel);

			for( int nCounter = 0; nCounter < nTotalBytes; nCounter = nCounter + header_info.nChannels * bytesPerPixelPerChannel )
			{
				C = (1.0 - (double)Calculate(pData+nCounter, bytesPerPixelPerChannel ) / dMaxColours );
				M = (1.0 - (double)Calculate(pData+nCounter+bytesPerPixelPerChannel, bytesPerPixelPerChannel ) / dMaxColours );
				Y = (1.0 - (double)Calculate(pData+nCounter+2*bytesPerPixelPerChannel, bytesPerPixelPerChannel ) / dMaxColours );
				K = (1.0 - (double)Calculate(pData+nCounter+3*bytesPerPixelPerChannel, bytesPerPixelPerChannel ) / dMaxColours );

				CMYKToRGB(C, M, Y, K, nRed, nGreen, nBlue );

				if ( 0 > nRed )
					nRed = 0;
				else if ( 255 < nRed )
					nRed = 255;
				if ( 0 > nGreen )
					nGreen = 0;
				else if ( 255 < nGreen )
					nGreen = 255;
				if ( 0 > nBlue )
					nBlue = 0;
				else if ( 255 < nBlue )
					nBlue = 255;

				::SetPixel( hdcMemory, nCol, nRow, RGB(nRed, nGreen, nBlue) );
				nCol++;
				if ( nWidth <= nCol )
				{
					nCol = 0;
					nRow++;
				}
			}

			::SelectObject( hdcMemory, hbmpOld );
			::DeleteDC( hdcMemory );
		}
		break;
		case 7:		// Multichannel
		{
			hdcMemory = ::CreateCompatibleDC(NULL);
			hbmpOld = (HBITMAP)::SelectObject( hdcMemory, hBitmap );

			int nBytesToRead = header_info.nBitsPerPixel / 8;

			int nRed = 0;
			int nGreen = 0;
			int nBlue = 0;
			double C, M, Y, K;

			int nRow = 0;
			int nCol = 0;

			double dMaxColours = pow( 2.0f, header_info.nBitsPerPixel);

			// assume format is in either CMY or CMYK
			assert ( 3 <= header_info.nChannels );
			if ( 3 <= header_info.nChannels )
			{

				for( int nCounter = 0; nCounter < nTotalBytes; nCounter = nCounter + header_info.nChannels * bytesPerPixelPerChannel )
				{
					C = (1.0 - (double)Calculate(pData+nCounter, bytesPerPixelPerChannel ) / dMaxColours );
					M = (1.0 - (double)Calculate(pData+nCounter+bytesPerPixelPerChannel, bytesPerPixelPerChannel ) / dMaxColours );
					Y = (1.0 - (double)Calculate(pData+nCounter+2*bytesPerPixelPerChannel, bytesPerPixelPerChannel ) / dMaxColours );
					K = 0;

					if ( 4 <= header_info.nChannels )
						K = (1.0 - (double)Calculate(pData+nCounter+header_info.nChannels*bytesPerPixelPerChannel, bytesPerPixelPerChannel ) / dMaxColours );

					CMYKToRGB(C, M, Y, K, nRed, nGreen, nBlue );

					if ( 0 > nRed )
						nRed = 0;
					else if ( 255 < nRed )
						nRed = 255;
					if ( 0 > nGreen )
						nGreen = 0;
					else if ( 255 < nGreen )
						nGreen = 255;
					if ( 0 > nBlue )
						nBlue = 0;
					else if ( 255 < nBlue )
						nBlue = 255;

					::SetPixel( hdcMemory, nCol, nRow, RGB(nRed, nGreen, nBlue) );
					nCol++;
					if ( nWidth <= nCol )
					{
						nCol = 0;
						nRow++;
					}
				}
			}

			::SelectObject( hdcMemory, hbmpOld );
			::DeleteDC( hdcMemory );
		}
		break;
		case 9:	// Lab
		{
			hdcMemory = ::CreateCompatibleDC(NULL);
			hbmpOld = (HBITMAP)::SelectObject( hdcMemory, hBitmap );

			int nBytesToRead = header_info.nBitsPerPixel / 8;

			int nRed = 0;
			int nGreen = 0;
			int nBlue = 0;
			int L, a, b;

			int nRow = 0;
			int nCol = 0;

			double L_coef, a_coef, b_coef;
			double dMaxColours = pow( 2.0f, header_info.nBitsPerPixel);
			L_coef = dMaxColours / 100.0;
			a_coef = dMaxColours / 256.0;
			b_coef = dMaxColours / 256.0;
			for( int nCounter = 0; nCounter < nTotalBytes; nCounter = nCounter + 3 * bytesPerPixelPerChannel )
			{
				L = (int)( (float)Calculate(pData+nCounter, bytesPerPixelPerChannel ) / L_coef );
				a = (int)( (float)Calculate(pData+nCounter+bytesPerPixelPerChannel, bytesPerPixelPerChannel ) / a_coef - 128.0);
				b = (int)( (float)Calculate(pData+nCounter+2*bytesPerPixelPerChannel, bytesPerPixelPerChannel ) / b_coef - 128.0);

				LabToRGB(L, a, b, nRed, nGreen, nBlue );

				if ( 0 > nRed )
					nRed = 0;
				else if ( 255 < nRed )
					nRed = 255;
				if ( 0 > nGreen )
					nGreen = 0;
				else if ( 255 < nGreen )
					nGreen = 255;
				if ( 0 > nBlue )
					nBlue = 0;
				else if ( 255 < nBlue )
					nBlue = 255;

				::SetPixel( hdcMemory, nCol, nRow, RGB(nRed, nGreen, nBlue) );
				nCol++;
				if ( nWidth <= nCol )
				{
					nCol = 0;
					nRow++;
				}
			}

			::SelectObject( hdcMemory, hbmpOld );
			::DeleteDC( hdcMemory );
		}
		break;
	}
}

int CPSD::Load(LPCTSTR szPathName)
{
	int nErrorCode = 0;	// No errors
	std::string cstrFileName(szPathName);

	FILE* pFile = 0;
	pFile = _fsopen( cstrFileName.c_str(), "rb", _SH_DENYWR );

	if ( 0 != pFile )
	{
		bool bSuccessHeader = false;
		bool bSuccessColourModeData = false;
		bool bSuccessImageResource = false;
		bool bSuccessLayerMaskInfo = false;

		try
		{
			bSuccessHeader = ReadHeader(pFile, header_info);
			if ( false == bSuccessHeader )
				nErrorCode = -2;	// Error in header
		}
		catch (...)
		{
				bSuccessHeader = false;
				nErrorCode = -2;
		}

		if ( bSuccessHeader )
		{
			try
			{
				bSuccessColourModeData = ReadColourModeData(pFile, colour_mode_data);
				if ( false == bSuccessColourModeData)
					nErrorCode = -3;		// Error in ColourMode Data
			}
			catch (...)
			{
				bSuccessColourModeData = false;
				nErrorCode = -3;
			}
		}

		if ( bSuccessColourModeData )
		{
			try
			{
				bSuccessImageResource = ReadImageResource(pFile, image_resource);
				if ( false == bSuccessImageResource )
					nErrorCode = -4;	// Error in Image Resource
			}
			catch (...)
			{
				bSuccessImageResource = false;
				nErrorCode = -4;
			}
		}

		if ( bSuccessImageResource )
		{
			try
			{
				bSuccessLayerMaskInfo = ReadLayerAndMaskInfoSection(pFile);
				if ( false == bSuccessLayerMaskInfo )
					nErrorCode = -5;	// Error in Mask Info
			}
			catch (...)
			{
				bSuccessLayerMaskInfo = false;
				nErrorCode = -5;
			}
		}

		if ( bSuccessImageResource )
		{
			try
			{
				nErrorCode = ReadImageData(pFile);
			}
			catch (...)
			{
				nErrorCode = -6;	// Error in Image Data
			}
		}
	}
	else
		nErrorCode = -1;	// Cannot open file

	return nErrorCode;
}

int CPSD::ReadImageData(FILE* pFile)
{
	int nErrorCode = 0;	// No Errors
	
	if ( !feof(pFile) )
		{
			unsigned char ShortValue[2];
			int nBytesRead = 0;
			int nItemsRead = (int)fread(&ShortValue, sizeof(ShortValue), 1, pFile);
			short nCompression = (short)Calculate( ShortValue, sizeof(ShortValue) );

			switch ( nCompression )
				{
				case 0:	// raw data
					{
						int nWidth = header_info.nWidth;
						int nHeight = header_info.nHeight;
						int bytesPerPixelPerChannel = header_info.nBitsPerPixel / 8;
						
						int nPixels = nWidth * nHeight;
						int nTotalBytes = nPixels * bytesPerPixelPerChannel * header_info.nChannels;

						unsigned char* pData = 0;
						unsigned char* ImageData = 0;

						int nBytesRead = 0;
						switch ( header_info.nColourMode )
							{
							case 1:		// Grayscale
							case 8:		// Duotone
							{
								unsigned int nValue = 0;
								pData = new unsigned char[nTotalBytes];
								ImageData = new unsigned char[bytesPerPixelPerChannel];
								memset(pData, 254, nTotalBytes);
								memset(ImageData, 254, bytesPerPixelPerChannel);

								while( !feof( pFile ) && ( nBytesRead < nTotalBytes ) )
								{
									memset(ImageData, 254, bytesPerPixelPerChannel);
									nItemsRead = (int)fread(ImageData, bytesPerPixelPerChannel, 1, pFile);
									memcpy( pData+nBytesRead, ImageData, bytesPerPixelPerChannel );
									nBytesRead += nItemsRead * bytesPerPixelPerChannel;
								}

								if ( ImageData )
									delete[] ImageData;
								ImageData = 0;
							}
							break;
							case 2:		// Indexed
							{
								assert ( (-1 != mnColourCount) && (0 < mnColourCount) );
								assert ( 0 != colour_mode_data.ColourData );

								unsigned int nValue = 0;
								pData = new unsigned char[nTotalBytes];
								ImageData = new unsigned char[bytesPerPixelPerChannel];
								memset(pData, 254, nTotalBytes);
								memset(ImageData, 254, bytesPerPixelPerChannel);

								while( !feof( pFile ) && ( nBytesRead < nTotalBytes ) )
								{
									memset(ImageData, 254, bytesPerPixelPerChannel);
									nItemsRead = (int)fread(ImageData, bytesPerPixelPerChannel, 1, pFile);
									memcpy( pData+nBytesRead, ImageData, bytesPerPixelPerChannel );
									nBytesRead += nItemsRead * bytesPerPixelPerChannel;
								}

								if ( ImageData )
									delete[] ImageData;
								ImageData = 0;
							}
							break;
							case 3:		// RGB
							{
								int nBytesToReadPerPixelPerChannel = bytesPerPixelPerChannel;
								if ( 2 == nBytesToReadPerPixelPerChannel )
								{
									nBytesToReadPerPixelPerChannel = 1;
									nTotalBytes = nPixels * nBytesToReadPerPixelPerChannel * header_info.nChannels;
								}

								ImageData = new unsigned char[nBytesToReadPerPixelPerChannel];
								pData = new unsigned char[nTotalBytes];
								memset(pData, 254, nTotalBytes);
								memset(ImageData, 254, nBytesToReadPerPixelPerChannel);

								int nPixelCounter = 0;
								for( int nColour = 0; nColour < 3; ++nColour )
								{
									nPixelCounter = nColour;
									for ( int nPos = 0; nPos < nPixels; ++nPos )
									{
										if ( !feof( pFile ) )
										{
											nItemsRead = (int)fread(ImageData, nBytesToReadPerPixelPerChannel, 1, pFile);
											memcpy( pData + nPixelCounter, ImageData, nBytesToReadPerPixelPerChannel );
											nBytesRead += nItemsRead * nBytesToReadPerPixelPerChannel;
											nPixelCounter += 3;
											if ( 2 == bytesPerPixelPerChannel )
												(int)fread(ImageData, nBytesToReadPerPixelPerChannel, 1, pFile);
										}
									}
								}
								if ( ImageData )
									delete[] ImageData;
								ImageData = 0;
							}
							break;
							case 4:	// CMYK
							{
								ImageData = new unsigned char[bytesPerPixelPerChannel];
								pData = new unsigned char[nTotalBytes];
								memset(pData, 254, nTotalBytes);
								memset(ImageData, 254, bytesPerPixelPerChannel);

								int nPixelCounter = 0;

								for( int nColour = 0; nColour < header_info.nChannels; ++nColour )
								{
									nPixelCounter = nColour*bytesPerPixelPerChannel;
									for ( int nPos = 0; nPos < nPixels; ++nPos )
									{
										if ( !feof( pFile ) )
										{
											nItemsRead = (int)fread(ImageData, bytesPerPixelPerChannel, 1, pFile);
											memcpy( pData + nPixelCounter, ImageData, bytesPerPixelPerChannel );
											nBytesRead += nItemsRead * bytesPerPixelPerChannel;

											nPixelCounter += header_info.nChannels*bytesPerPixelPerChannel;
										}
									}
								}

								if ( ImageData )
									delete[] ImageData;
								ImageData = 0;
							}
							break;
							case 9:	// Lab
							{
								ImageData = new unsigned char[bytesPerPixelPerChannel];
								pData = new unsigned char[nTotalBytes];
								memset(pData, 254, nTotalBytes);
								memset(ImageData, 254, bytesPerPixelPerChannel);

								int nPixelCounter = 0;
								for( int nColour = 0; nColour < 3; ++nColour )
								{
									nPixelCounter = nColour*bytesPerPixelPerChannel;
									for ( int nPos = 0; nPos < nPixels; ++nPos )
									{
										if ( !feof( pFile ) )
										{
											nItemsRead = (int)fread(ImageData, bytesPerPixelPerChannel, 1, pFile);
											memcpy( pData + nPixelCounter, ImageData, bytesPerPixelPerChannel );
											nBytesRead += nItemsRead * bytesPerPixelPerChannel;
											nPixelCounter += 3*bytesPerPixelPerChannel;
										}
									}
								}

								if ( ImageData )
									delete[] ImageData;
								ImageData = 0;
							}
							break;
						}

						assert ( nBytesRead == nTotalBytes );
						if  ( nBytesRead == nTotalBytes )
						{
							if ( pData )
							{
									int ppm_x = 3780;	// 96 dpi
									int ppm_y = 3780;	// 96 dpi

									if ( mbResolutionInfoFilled )
									{
										int nHorResolution = (int)resolution_info.hRes;
										int nVertResolution = (int)resolution_info.vRes;

										ppm_x = ( nHorResolution * 10000 ) / 254;
										ppm_y = ( nVertResolution * 10000 ) / 254;
									}

									switch (header_info.nBitsPerPixel)
									{
										case 1:
										{
											nErrorCode = -7; // Not yet implemented
										}
										break;
										case 8:
										case 16:
										{
											CreateDIBSection(nWidth, nHeight, ppm_x, ppm_y, 24);
											break;
										}
										break;
										default:
										{
											nErrorCode = -8;	// Unsupported format
										}
									}

									HBITMAP hBitmap = mhBitmap;

									if ( !hBitmap )
									{
										if ( pData )
											delete[] pData;
										pData = 0;

										if ( ImageData )
											delete[] ImageData;
										ImageData = 0;

										nErrorCode = -9;	// Cannot create hBitmap
										return nErrorCode;
									}
									ProccessBuffer(pData);
							}
						}
						if ( pData )
							delete[] pData;
						pData = 0;
					}
				break;
				case 1:	// RLE compression
				{
					int nWidth = header_info.nWidth;
					int nHeight = header_info.nHeight;
					int bytesPerPixelPerChannel = header_info.nBitsPerPixel / 8;
					
					int nPixels = nWidth * nHeight;
					int nTotalBytes = nPixels * bytesPerPixelPerChannel * header_info.nChannels;

					unsigned char* pData = new unsigned char[nTotalBytes];
					memset(pData, 254, nTotalBytes);

					unsigned char* p = pData;
					int nValue = 0;

					unsigned char ByteValue[1];

					int Count = 0;

					// The RLE-compressed data is preceeded by a 2-byte data count for each row in the data,
					// which we're going to just skip.
					int nRet = fseek(pFile, nHeight * header_info.nChannels * 2, SEEK_CUR);

					for (int channel = 0; channel < header_info.nChannels; channel++)
					{
						// Read the RLE data.
						Count = 0;
						while (Count < nPixels)
						{
							(int)fread(&ByteValue, sizeof(ByteValue), 1, pFile);

							int len = Calculate( ByteValue, sizeof(ByteValue) );
							if ( 128 > len )
							{
								len++;
								Count += len;

								while (len)
								{
									(int)fread(&ByteValue, sizeof(ByteValue), 1, pFile);

									nValue = Calculate( ByteValue, sizeof(ByteValue) );

									*p = nValue;
									p += sizeof(ByteValue);
									len--;
								}
							}
							else if ( 128 < len )
							{
								// Next -len+1 bytes in the dest are replicated from next source byte.
								// (Interpret len as a negative 8-bit int.)
								len ^= 0x0FF;
								len += 2;
								(int)fread(&ByteValue, sizeof(ByteValue), 1, pFile);

								nValue = Calculate( ByteValue, sizeof(ByteValue) );

								Count += len;

								while (len)
								{
									*p = nValue;
									p += sizeof(ByteValue);
									len--;
								}
							}
							else if ( 128 == len )
							{
							// Do nothing
							}
						}
					}

					unsigned char* pSource = pData;
					unsigned char* pDest = new unsigned char[nTotalBytes];
					memset(pDest, 254, nTotalBytes);

					int nPixelCounter = 0;
					for( int nColour = 0; nColour < header_info.nChannels; ++nColour )
					{
						nPixelCounter = nColour*bytesPerPixelPerChannel;
						for ( int nPos = 0; nPos < nPixels; ++nPos )
						{
								memcpy( pDest + nPixelCounter, pSource, bytesPerPixelPerChannel );
								pSource++;

								nPixelCounter += header_info.nChannels*bytesPerPixelPerChannel;
						}
					}
					if ( pData )
						delete[] pData;
					pData = pDest;

					if ( pData )
					{
						int ppm_x = 3780;	// 96 dpi
						int ppm_y = 3780;	// 96 dpi

						if ( mbResolutionInfoFilled )
						{
							int nHorResolution = (int)resolution_info.hRes;
							int nVertResolution = (int)resolution_info.vRes;

							ppm_x = ( nHorResolution * 10000 ) / 254;
							ppm_y = ( nVertResolution * 10000 ) / 254;
						}

						switch (header_info.nBitsPerPixel)
							{
							case 1:
								{
									nErrorCode = -7; // Not yet implemented
								}
							break;
							case 8:
							case 16:
								{
									CreateDIBSection(nWidth, nHeight, ppm_x, ppm_y, 24);
									break;
								}
							break;
							default:
								{
									nErrorCode = -8;	// Unsupported format
								}
							}

						HBITMAP hBitmap = mhBitmap;

						if ( !hBitmap )
						{
							if ( pData )
								delete[] pData;
							pData = 0;

							nErrorCode = -9;	// Cannot create hBitmap
							return nErrorCode;
						}

						ProccessBuffer(pData);

					}

					if ( pData )
						delete[] pData;
					pData = 0;
				}
				break;
				case 2:	// ZIP without prediction
					{
						nErrorCode = -10;	// ZIP without prediction, no specification
					}
				break;
				case 3:	// ZIP with prediction
					{
						nErrorCode = -11;	// ZIP with prediction, no specification
					}
				break;

				default:
					nErrorCode = -12;	// Unknown format
				}
		}

	return nErrorCode;
}

HBITMAP CPSD::Detach()
{
	mbDetached = true;

	return mhBitmap;
}

};	// MyPSD