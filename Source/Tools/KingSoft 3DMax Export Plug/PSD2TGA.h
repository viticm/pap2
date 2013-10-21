// PSD2TGA.h: interface for the CPSD2TGA class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PSD2TGA_H__B4DDF54D_903D_409C_B335_7D319466F191__INCLUDED_)
#define AFX_PSD2TGA_H__B4DDF54D_903D_409C_B335_7D319466F191__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CPSD2TGA  
{
public:
	CPSD2TGA();
	virtual ~CPSD2TGA();
	HRESULT PSD2TGA(LPSTR pPSDFileNane,LPSTR pTGAFileName);
private:
	
#pragma pack(push,1)
	struct _TGAHeader 
	{
		unsigned char   Id ;
		unsigned char   ColourMapType ;
		unsigned char   ImageType ;
		unsigned short  ColourMapOrigin ;
		unsigned short  ColourMapLength ;
		unsigned char   ColourMapDepth ;
		unsigned short  x_origin ;
		unsigned short  y_origin ;
		unsigned short  width ;
		unsigned short  height ;
		unsigned char   bitsperpixel ;
		unsigned char   imagedescriptor ;
	};
#pragma pack(pop)

	void WriteChannel(BYTE Data, int Count, BYTE *Start, int Step);
	void Reverse(BYTE *data, int, int);
	HRESULT Save2Tga(LPSTR strFilename,
						   DWORD dwWidth,     // dwWidth、dwHeight为宽高，
						   DWORD dwHeight,
						   BYTE* pixels      // pixels为解包目标指针，
						   );

	DWORD Read32(FILE* pFile);
	WORD Read16(FILE* pFile);
	HRESULT UnPackPSD(FILE *fp,          // fp为PSD文件指针，
					  DWORD dwWidth,     // dwWidth、dwHeight为宽高，
					  DWORD dwHeight,
					  BYTE* pixels,      // pixels为解包目标指针，
					  WORD ChannelCnt,   // ChannelCnt为通道数
					  WORD Compression );

};

#endif // !defined(AFX_PSD2TGA_H__B4DDF54D_903D_409C_B335_7D319466F191__INCLUDED_)
