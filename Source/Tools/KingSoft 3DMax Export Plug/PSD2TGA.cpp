#include "header.h"
#include "asciiexp.h"
#include "PSD2TGA.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
#endif

#define  MAX_PSD_CHANNELS 24
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPSD2TGA::CPSD2TGA()
{

}

CPSD2TGA::~CPSD2TGA()
{

} 

//DEL HRESULT CPSD2TGA::LoadPSD(LPSTR strFilename)
//DEL {
//DEL 	DWORD dwWidth, dwHeight; // 宽高
//DEL 	//long lSurfWidth = m_Rect.right - m_Rect.left;
//DEL 	//long lSurfHeight = m_Rect.bottom - m_Rect.top;
//DEL 	WORD CompressionType; // 压缩类型
//DEL 	//HDC hDC;
//DEL 	FILE *fpPSD;
//DEL 	WORD ChannelCount; // 通道数
//DEL 	BYTE* m_pbAlphaMask = NULL;
//DEL 	
//DEL 	// 打开PSD文件
//DEL 	if ( ( fpPSD = fopen ( strFilename, "rb" ) ) == NULL )
//DEL 	{
//DEL 		MessageBox(NULL, "错误", "文件不存在", MB_OK);
//DEL 		return E_FAIL;
//DEL 	}
//DEL 	// 头四个字节为"8BPS"
//DEL 	char signature[5];
//DEL 	signature[0] = fgetc( fpPSD );
//DEL 	signature[1] = fgetc( fpPSD );
//DEL 	signature[2] = fgetc( fpPSD );
//DEL 	signature[3] = fgetc( fpPSD );
//DEL 	signature[4] = '\0';
//DEL 	if ( strcmp( signature,"8BPS" ) != 0 )
//DEL 	{
//DEL 		return E_FAIL;
//DEL 	}
//DEL 	
//DEL 	// 版本必须为1
//DEL 	if ( Read16( fpPSD ) != 1 ) 
//DEL 	{
//DEL 		return E_FAIL;
//DEL 	}
//DEL 	
//DEL 	// 跳过一些数据 (总是0)
//DEL 	Read32( fpPSD );
//DEL 	Read16( fpPSD );
//DEL 	
//DEL 	// 读取通道数
//DEL 	ChannelCount = Read16( fpPSD );
//DEL 	
//DEL 	// 确定至少有一个通道
//DEL 	if ( ( ChannelCount < 0 ) || ( ChannelCount > MAX_PSD_CHANNELS ) ) 
//DEL 	{
//DEL 		return E_FAIL;
//DEL 	}
//DEL 	
//DEL 	// 读入宽和高
//DEL 	dwHeight = Read32( fpPSD );
//DEL 	dwWidth = Read32( fpPSD );
//DEL 	
//DEL 	// 只读入8位通道
//DEL 	if ( Read16( fpPSD ) != 8 ) 
//DEL 	{
//DEL 		return E_FAIL;
//DEL 	}
//DEL 	
//DEL 	// 确定模式为RGB.
//DEL 	// 可能值：
//DEL 	// 0: 位图
//DEL 	// 1: 灰阶
//DEL 	// 2: 索引
//DEL 	// 3: RGB
//DEL 	// 4: CMYK
//DEL 	// 7: Multichannel
//DEL 	// 8: Duotone
//DEL 	// 9: Lab
//DEL 	if ( Read16( fpPSD ) != 3 )
//DEL 	{
//DEL 		return E_FAIL;
//DEL 	}
//DEL 
//DEL 		
//DEL 	// 跳过数据（如调色板）
//DEL 	int ModeDataCount = Read32( fpPSD );
//DEL 	if ( ModeDataCount )
//DEL 		fseek( fpPSD, ModeDataCount, SEEK_CUR );
//DEL 	
//DEL 	// 跳过数据（如：pen tool paths, etc）
//DEL 	int ResourceDataCount = Read32( fpPSD );
//DEL 	if ( ResourceDataCount )
//DEL 		fseek( fpPSD, ResourceDataCount, SEEK_CUR );
//DEL 	
//DEL 	// 条过layer and mask数据
//DEL 	int ReservedDataCount = Read32( fpPSD );
//DEL 	if ( ReservedDataCount )
//DEL 		fseek( fpPSD, ReservedDataCount, SEEK_CUR );
//DEL 	
//DEL 	// 0: 非压缩
//DEL 	// 1: RLE压缩
//DEL 	CompressionType = Read16( fpPSD );
//DEL 	if ( CompressionType > 1 )
//DEL 	{
//DEL 		return E_FAIL;
//DEL 	}
//DEL 	int length ;
//DEL 	length = ftell(fpPSD);
//DEL 	fseek(fpPSD, 0, SEEK_END);
//DEL 	length = ftell(fpPSD) - length;
//DEL 	fseek(fpPSD, -length, SEEK_END);
//DEL 	
//DEL 	
//DEL 	BYTE* PSDPixels = new BYTE[ ( dwHeight * dwWidth ) * 4 ];
//DEL 	
//DEL 	// 解包数据
//DEL 	UnPackPSD( fpPSD, dwWidth, dwHeight, PSDPixels, ChannelCount, CompressionType );
//DEL 	fclose( fpPSD );
//DEL 	
//DEL 	Reverse(PSDPixels,  dwWidth,dwHeight);
//DEL 	Save2Tga("Kill.Tga",dwWidth,dwHeight,PSDPixels);
//DEL 	SAFE_DELETE_ARRAY( PSDPixels );
//DEL 	
//DEL 	return S_OK;
//DEL }

HRESULT CPSD2TGA::UnPackPSD(FILE *fp,          // fp为PSD文件指针，
							DWORD dwWidth,     // dwWidth、dwHeight为宽高，
							DWORD dwHeight,
							BYTE* pixels,      // pixels为解包目标指针，
							WORD ChannelCnt,   // ChannelCnt为通道数
							WORD Compression ) // Compression位压缩类型。 　　　　　　　　　　　　　　　　
{
	int Default[4] = { 0, 0, 0, 255 };
	int chn[4] = { 2, 1, 0, 3};
	int PixelCount = dwWidth * dwHeight;
	BYTE * Channels[4];
	char Header;
	BYTE Pixel;
	unsigned int Count;
	int Length = 0;
	Channels[0] = pixels + 2;
	Channels[1] = pixels + 1;
	Channels[2] = pixels ;
	Channels[3] = pixels + 3;
	if( ChannelCnt > 4)
	{
		ChannelCnt = 4;
		MessageBox(NULL, "贴图通道超过4个", "Warning", MB_OK);
	}
	if ( Compression ) 
	{
		fseek( fp, dwHeight * ChannelCnt * 2, SEEK_CUR );

		for (int cn = 0; cn < ChannelCnt; cn ++)
		{
			unsigned int i = 0 ;
			while ( i < dwHeight * dwWidth)
			{
				Header = fgetc(fp);
				Pixel = fgetc(fp);
				if ( Header == -128)
					continue;
				else if ( Header < 0 )
				{
					for (Count = -Header + 1, i += Count; Count >0; Count --)
					{
						*(Channels[cn]) = Pixel;
						Channels[cn] += 4;
					}
				}
				else
				{
					*(Channels[cn]) = Pixel;
					Channels[cn] += 4;
					for (Count = Header , i += Count + 1; Count > 0 ; Count --)
					{
						Pixel = fgetc(fp);
						*(Channels[cn]) = Pixel;
						Channels[cn] += 4;
					}
				}
			}
			
		}
		if(ChannelCnt<=3)
		{
			int i =0;
			while ( i < dwHeight * dwWidth)
			{
				(*Channels[3]) = 255;
				Channels[3] += 4;
				i++;
			}
		}
	}	
	else//没有压缩的处理
	{
		for ( int c=0; c < 4; c++ )
		{
			int channel = chn[c];
			if ( channel > ChannelCnt ) 
			{
				for( int pn = 0; pn < PixelCount; pn++ ) 
				{
					pixels[ ( pn * 4 ) + channel ] = Default[ channel ];
				}
			}
			else 
			{
				for( int n = 0; n < PixelCount; n++ ) 
				{
					pixels[ ( n * 4 ) + channel ] = fgetc( fp );
				}
			}
		}
	}
	return S_OK;
}

WORD CPSD2TGA::Read16(FILE *pFile)
{
	WORD A = 0;
	WORD B = 0;
	A = fgetc(pFile);
	B = fgetc(pFile);
	WORD S = (A<<8)|B;
	return S;
}

DWORD CPSD2TGA::Read32(FILE *pFile)
{
	DWORD A,B;
	A = Read16(pFile);
	B = Read16(pFile);
	DWORD S =(A<<16)|B;
	return S;
}

HRESULT CPSD2TGA::Save2Tga(LPSTR strFilename,
						   DWORD dwWidth,     // dwWidth、dwHeight为宽高，
						   DWORD dwHeight,
						   BYTE* pixels      // pixels为解包目标指针，
						   )
{
	_TGAHeader Head;
	ZeroMemory(&Head,sizeof(_TGAHeader));
	Head.Id =0;
	Head.ColourMapType = 0;
	Head.ImageType = 2;
	Head.ColourMapOrigin = 0;
	Head.ColourMapLength = 0;
	Head.ColourMapType   = 0;
	Head.x_origin = 0;
	Head.y_origin = 0;
	Head.width  = (unsigned short) dwWidth;
	Head.height = (unsigned short) dwHeight;
	Head.bitsperpixel = 32;
	Head.imagedescriptor = 8;
	FILE* pFile = fopen(strFilename,"wb");
	if(!pFile)
		return E_FAIL;
	fwrite(&Head,sizeof(_TGAHeader),1,pFile);
	fwrite(pixels,1,dwWidth*dwHeight*4,pFile);
	fclose(pFile);

	return S_OK;

}



void CPSD2TGA::Reverse(BYTE *data, int dwWidth, int dwHeight)
{
	BYTE temp ;
	int iHeight = dwHeight >> 1;
	int iWidth = dwWidth << 2;
	for (int i = 0 ; i < iHeight ; i ++)
		for ( int j = 0 ; j < iWidth; j++)
		{
			temp = *(data + i * iWidth  + j);
			*(data + i * iWidth  + j) = *(data + (dwHeight -i - 1) * iWidth + j);
			*(data + (dwHeight -i - 1) * iWidth  + j) = temp;
			
		}
}

void CPSD2TGA::WriteChannel(BYTE Data, int Count, BYTE *Start, int Step)
{
	for ( int i =0; i < Count; i ++)
	{
		*Start = Data;
		Start += Step;
	}

}

HRESULT CPSD2TGA::PSD2TGA(LPSTR pPSDFileNane, LPSTR pTGAFileName)
{
	DWORD dwWidth, dwHeight; // 宽高
	//long lSurfWidth = m_Rect.right - m_Rect.left;
	//long lSurfHeight = m_Rect.bottom - m_Rect.top;
	WORD CompressionType; // 压缩类型
	//HDC hDC;
	FILE *fpPSD;
	WORD ChannelCount; // 通道数
	BYTE* m_pbAlphaMask = NULL;
	
	// 打开PSD文件
	if ( ( fpPSD = fopen ( pPSDFileNane, "rb" ) ) == NULL )
	{
		TCHAR ErrorInfo[1024];
		sprintf(ErrorInfo, "文件%s不存在", pPSDFileNane );
		MessageBox(NULL, ErrorInfo, "错误", MB_OK);
		return E_FAIL;
	}
	// 头四个字节为"8BPS"
	char signature[5];
	signature[0] = fgetc( fpPSD );
	signature[1] = fgetc( fpPSD );
	signature[2] = fgetc( fpPSD );
	signature[3] = fgetc( fpPSD );
	signature[4] = '\0';
	if ( strcmp( signature,"8BPS" ) != 0 )
	{
		return E_FAIL;
	}
	
	// 版本必须为1
	if ( Read16( fpPSD ) != 1 ) 
	{
		return E_FAIL;
	}
	
	// 跳过一些数据 (总是0)
	Read32( fpPSD );
	Read16( fpPSD );
	
	// 读取通道数
	ChannelCount = Read16( fpPSD );
	
	// 确定至少有一个通道
	if ( ( ChannelCount < 0 ) || ( ChannelCount > MAX_PSD_CHANNELS ) ) 
	{
		return E_FAIL;
	}
	
	// 读入宽和高
	dwHeight = Read32( fpPSD );
	dwWidth = Read32( fpPSD );
	
	// 只读入8位通道
	if ( Read16( fpPSD ) != 8 ) 
	{
		return E_FAIL;
	}
	
	// 确定模式为RGB.
	// 可能值：
	// 0: 位图
	// 1: 灰阶
	// 2: 索引
	// 3: RGB
	// 4: CMYK
	// 7: Multichannel
	// 8: Duotone
	// 9: Lab
	if ( Read16( fpPSD ) != 3 )
	{
		return E_FAIL;
	}

		
	// 跳过数据（如调色板）
	int ModeDataCount = Read32( fpPSD );
	if ( ModeDataCount )
		fseek( fpPSD, ModeDataCount, SEEK_CUR );
	
	// 跳过数据（如：pen tool paths, etc）
	int ResourceDataCount = Read32( fpPSD );
	if ( ResourceDataCount )
		fseek( fpPSD, ResourceDataCount, SEEK_CUR );
	
	// 条过layer and mask数据
	int ReservedDataCount = Read32( fpPSD );
	if ( ReservedDataCount )
		fseek( fpPSD, ReservedDataCount, SEEK_CUR );
	
	// 0: 非压缩
	// 1: RLE压缩
	CompressionType = Read16( fpPSD );
	if ( CompressionType > 1 )
	{
		return E_FAIL;
	}
	int length ;
	length = ftell(fpPSD);
	fseek(fpPSD, 0, SEEK_END);
	length = ftell(fpPSD) - length;
	fseek(fpPSD, -length, SEEK_END);
	
	
	BYTE* PSDPixels = new BYTE[ ( dwHeight * dwWidth ) * 4 ];
	
	// 解包数据
	UnPackPSD( fpPSD, dwWidth, dwHeight, PSDPixels, ChannelCount, CompressionType );
	fclose( fpPSD );
	
	Reverse(PSDPixels,  dwWidth,dwHeight);
	Save2Tga(pTGAFileName,dwWidth,dwHeight,PSDPixels);
	SAFE_DELETE_ARRAY(PSDPixels);
	
	return S_OK;
}
