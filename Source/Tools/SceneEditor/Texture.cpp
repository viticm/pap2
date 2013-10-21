// Texture.cpp: implementation of the KTexture class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SceneEditor.h"
#include "Texture.h"
#include "Engine.h"



//#include "Public\KPakFile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
#define ARGBTO4444(a,r,g,b) (((((WORD)(a))<<8)&0xf000) + ((((WORD)(r))<<4)&0x0f00) + (((WORD)(g))&0x00f0) + (((WORD)(b))>>4))
extern WORD     g_Def_FontHeight;
extern TCHAR    g_Def_FontName[80];

LPDIRECT3DTEXTURE9 gMemTexsForUpdate [MAX_BACK_TEXTURE_NUM];

BYTE* g_pSprExchangeBuffer;

//KLogFile	g_LogFile;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction


KTexture::KTexture()
{
	ID = 0;
	m_dType = 0;
	m_lpFileName = NULL;
	m_lpTexture = NULL;
	m_dOption = 0;
}

KTexture::~KTexture()
{

}

void KTexture::Initialize(PVOID pTextureTable)
{
	;
}

HRESULT KTexture::LoadFromFile(LPSTR pFileName)
{
	HRESULT hr = S_OK;
	
	SAFE_RELEASE(m_lpTexture);

	if ( FAILED(hr = D3DXCreateTextureFromFile( g_pd3dDevice, pFileName,&m_lpTexture ) ) )
	{
		TCHAR str[256];
		TCHAR path_buffer[256];
		TCHAR drive[256];
		TCHAR dir[256];
		TCHAR fname[256];
		TCHAR ext[256];

		wsprintf(path_buffer,"%s",pFileName);
		_splitpath( path_buffer, drive, dir, fname, ext );
		wsprintf(str,"%s\\%s%s",g_Def_TextureDirectory,fname,ext);

		if ( FAILED(hr = D3DXCreateTextureFromFile( g_pd3dDevice, str,&m_lpTexture ) ) )
		{
			wsprintf(str,"%s\\%s%s",g_Def_ModelTextureDirectory,fname,ext);
			if ( FAILED(hr = D3DXCreateTextureFromFile( g_pd3dDevice, str,&m_lpTexture ) ) )
			{
				return hr;
			}
		}

	}

	WIN32_FIND_DATA Find;
	HANDLE hFind = FindFirstFile(pFileName,&Find);
	::GetFileTime(hFind,NULL,NULL,&m_FileTime);
	FindClose(hFind);

	if(!m_lpFileName)
	{
		m_lpFileName = new TCHAR[256];
	}

	wsprintf(m_lpFileName,pFileName);
	return S_OK;
}

HRESULT KTexture::CleanUp()
{
	//SAFE_RELEASE(m_lpTexture);        // release in sub class
	SAFE_DELETE_ARRAY(m_lpFileName);
	
	return S_OK;
}

HRESULT KAnimateTexture::CleanUp()
{
    for(DWORD i=0; i< m_dwNumFrame; i++)
    {
        SAFE_RELEASE(m_lpBackTextures[i]);
    }
    SAFE_DELETE_ARRAY(m_lpBackTextures);
    
    KTexture::CleanUp();

    return S_OK;
}

HRESULT KAnimateTexture::LoadBumpMap(LPSTR pFileName)
{
	//HRESULT hr = S_OK;
	TCHAR path_buffer[256];
	TCHAR drive[256];
	TCHAR dir[256];
	TCHAR fname[256];
	TCHAR ext[256];
	TCHAR str[256];

	wsprintf(path_buffer,"%s",pFileName);
	_splitpath( path_buffer, drive, dir, fname, ext );

	wsprintf(str,pFileName);
	FILE * pFile = fopen(pFileName,"rt");
	if (pFile==NULL)
	{
		wsprintf(str,"%s\\%s%s",g_Def_TextureDirectory,fname,ext);
		pFile = fopen(str,"rt");
		if (pFile==NULL)
		{
			wsprintf(str,"%s\\%s%s",g_Def_ModelTextureDirectory,fname,ext);
			pFile = fopen(str,"rt");
			if (pFile==NULL)
			{
				return E_FAIL;
			}
		}
	}

	DWORD NumFrame = 0;
	float TotalLength = 0;
	fscanf(pFile,"%d",&NumFrame);
	fscanf(pFile,"%f",&TotalLength);

	wsprintf(path_buffer,"%s",str);
	_splitpath( path_buffer, drive, dir, fname, ext );

	TCHAR Name[256];

	m_lpBackTextures = new LPDIRECT3DTEXTURE9[NumFrame];

	m_dwNumFrame = 0;
	for(DWORD i=0;i<NumFrame;i++)
	{
		fscanf(pFile,"%s",Name);
		if (SUCCEEDED(D3DXCreateTextureFromFile( g_pd3dDevice, Name,&m_lpBackTextures[m_dwNumFrame] ) ) )
		{
			Exchange2Bump(m_lpBackTextures[m_dwNumFrame]);
			m_dwNumFrame++;
		}
		else
		{
			wsprintf(str,"%s\\%s",g_Def_ModelDirectory,Name);
			if (SUCCEEDED(D3DXCreateTextureFromFile( g_pd3dDevice, str ,&m_lpBackTextures[m_dwNumFrame] ) ) )
			{
				Exchange2Bump(m_lpBackTextures[m_dwNumFrame]);
				m_dwNumFrame++;
			}
			else
			{
				wsprintf(str,"%s%s%s",drive,dir,Name);
				if (SUCCEEDED(D3DXCreateTextureFromFile( g_pd3dDevice, str ,&m_lpBackTextures[m_dwNumFrame] ) ) )
				{
					Exchange2Bump(m_lpBackTextures[m_dwNumFrame]);
					m_dwNumFrame++;
				}
			}
		}
	}
	m_lpFileName = new TCHAR[256];
	wsprintf(m_lpFileName,pFileName);
	m_lpTexture = m_lpBackTextures[0];
	m_fFrameLength = TotalLength/m_dwNumFrame;
	return S_OK;
}

HRESULT KAnimateTexture::LoadFromFile(LPSTR pFileName)
{
	//HRESULT hr = S_OK;
	TCHAR path_buffer[256];
	TCHAR drive[256];
	TCHAR dir[256];
	TCHAR fname[256];
	TCHAR ext[256];
	TCHAR str[256];

	wsprintf(path_buffer,"%s",pFileName);
	_splitpath( path_buffer, drive, dir, fname, ext );

	wsprintf(str,pFileName);
	FILE * pFile = fopen(pFileName,"rt");
	if (pFile==NULL)
	{
		wsprintf(str,"%s\\%s%s",g_Def_TextureDirectory,fname,ext);
		pFile = fopen(str,"rt");
		if (pFile==NULL)
		{
			wsprintf(str,"%s\\%s%s",g_Def_ModelTextureDirectory,fname,ext);
			pFile = fopen(str,"rt");
			if (pFile==NULL)
			{
				return E_FAIL;
			}
		}
	}

	DWORD NumFrame = 0;
	float TotalLength = 0;
	fscanf(pFile,"%d",&NumFrame);
	fscanf(pFile,"%f",&TotalLength);

	wsprintf(path_buffer,"%s",str);
	_splitpath( path_buffer, drive, dir, fname, ext );

	TCHAR Name[256];
	
	m_lpBackTextures = new LPDIRECT3DTEXTURE9[NumFrame];
	
	m_dwNumFrame = 0;
	for(DWORD i=0;i<NumFrame;i++)
	{
		fscanf(pFile,"%s",Name);
		if (SUCCEEDED(D3DXCreateTextureFromFile( g_pd3dDevice, Name,&m_lpBackTextures[m_dwNumFrame] ) ) )
		{
			m_dwNumFrame++;
		}
		else
		{
			wsprintf(str,"%s\\%s",g_Def_ModelDirectory,Name);
			if (SUCCEEDED(D3DXCreateTextureFromFile( g_pd3dDevice, str ,&m_lpBackTextures[m_dwNumFrame] ) ) )
			{
				m_dwNumFrame++;
			}
			else
			{
				wsprintf(str,"%s%s%s",drive,dir,Name);
				if (SUCCEEDED(D3DXCreateTextureFromFile( g_pd3dDevice, str ,&m_lpBackTextures[m_dwNumFrame] ) ) )
				{
					m_dwNumFrame++;
				}
			}
		}
	}
	m_lpFileName = new TCHAR[256];
	wsprintf(m_lpFileName,pFileName);
	m_lpTexture = m_lpBackTextures[0];
	m_fFrameLength = TotalLength/m_dwNumFrame;
	return S_OK;
}

HRESULT KTexture::Update(DWORD Method,float Time)
{
	Time = 0.0f;
	Method = 1;
	return S_OK;
}

HRESULT KAnimateTexture::Update(DWORD Method, float Time)
{
	int nFrame = 0;
	if (Method == PLAY_NONE)
	{
		nFrame = (int)(Time/m_fFrameLength);
		if ((DWORD) nFrame >= m_dwNumFrame) nFrame = m_dwNumFrame - 1;
	}
	else if (Method==PLAY_CIRCLE)
	{
		int T = (int) (Time/(m_fFrameLength * m_dwNumFrame));
		float m_Time = Time - (m_fFrameLength*m_dwNumFrame) * T;
		nFrame = (int)(m_Time/m_fFrameLength);
		if ((DWORD) nFrame == m_dwNumFrame) nFrame = 0;
	}
	else if (Method==PLAY_PINGPANG)
	{
		int T = (int) (Time/(m_fFrameLength * m_dwNumFrame));
		float m_Time = Time - (m_fFrameLength*m_dwNumFrame) * T;
		if (T%2==0)
		{
			nFrame = (int)(m_Time/m_fFrameLength);
			if ((DWORD) nFrame>=m_dwNumFrame) nFrame = m_dwNumFrame - 1;
			if (nFrame<0)
				nFrame = 0;
		}
		else
		{
			int K =  (int)(m_Time/m_fFrameLength);
			nFrame = m_dwNumFrame - K ;
			if ((DWORD) nFrame>=m_dwNumFrame) nFrame = m_dwNumFrame - 1;
			if (nFrame<0)
				nFrame = 0;
		}
	}

	if ((nFrame>=0)&&((DWORD)nFrame<m_dwNumFrame))
	{
		m_lpTexture = m_lpBackTextures[nFrame];
	}
	else
	{
		m_lpTexture = NULL;
	}
	return S_OK;
}

HRESULT KTexture::ComputeNormal()
{
	HRESULT hr = S_OK;
	LPDIRECT3DTEXTURE9 m_lpTextureNormal = NULL;
	D3DSURFACE_DESC Desc;
	if (FAILED(m_lpTexture->GetLevelDesc(0,&Desc)))
		return E_FAIL;
    if (FAILED(D3DXCreateTexture(g_pd3dDevice,Desc.Width,Desc.Height,
		0,0,D3DFMT_A8R8G8B8,D3DPOOL_MANAGED, &m_lpTextureNormal)))
		return E_FAIL;

	if (FAILED(hr = D3DXComputeNormalMap(m_lpTextureNormal,m_lpTexture,
									NULL,0,D3DX_CHANNEL_RED,15)))
		return hr;
	SAFE_RELEASE(m_lpTexture);
	m_lpTexture = m_lpTextureNormal;

	return S_OK;
}

HRESULT KTexture::Exchange2Bump(LPDIRECT3DTEXTURE9& lpTexture)
{
	LPDIRECT3DTEXTURE9 psBumpSrc = lpTexture;
	LPDIRECT3DTEXTURE9 m_psBumpMap = NULL;

	D3DSURFACE_DESC    d3dsd;
	D3DLOCKED_RECT     d3dlr;

	D3DFORMAT     m_BumpMapFormat = D3DFMT_V8U8; 

	psBumpSrc->GetLevelDesc( 0, &d3dsd );
	// Create the bumpmap's surface and texture objects
	if( FAILED( g_pd3dDevice->CreateTexture( d3dsd.Width, d3dsd.Height, 1, 0,
		m_BumpMapFormat, D3DPOOL_MANAGED, &m_psBumpMap, NULL ) ) )
	{
		return E_FAIL;
	}

	// Fill the bits of the new texture surface with bits from
	// a private format.
	psBumpSrc->LockRect( 0, &d3dlr, 0, 0 );
	DWORD dwSrcPitch = (DWORD)d3dlr.Pitch;
	BYTE* pSrcTopRow = (BYTE*)d3dlr.pBits;
	BYTE* pSrcCurRow = pSrcTopRow;
	BYTE* pSrcBotRow = pSrcTopRow + (dwSrcPitch * (d3dsd.Height - 1) );

	m_psBumpMap->LockRect( 0, &d3dlr, 0, 0 );
	DWORD dwDstPitch = (DWORD)d3dlr.Pitch;
	BYTE* pDstTopRow = (BYTE*)d3dlr.pBits;
	BYTE* pDstCurRow = pDstTopRow;

	for( DWORD y=0; y<d3dsd.Height; y++ )
	{
		BYTE* pSrcB0; // addr of current pixel
		BYTE* pSrcB1; // addr of pixel below current pixel, wrapping to top if necessary
		BYTE* pSrcB2; // addr of pixel above current pixel, wrapping to bottom if necessary
		BYTE* pDstT;  // addr of dest pixel;

		pSrcB0 = pSrcCurRow;

		if( y == d3dsd.Height - 1)
			pSrcB1 = pSrcTopRow;
		else
			pSrcB1 = pSrcCurRow + dwSrcPitch;

		if( y == 0 )
			pSrcB2 = pSrcBotRow;
		else
			pSrcB2 = pSrcCurRow - dwSrcPitch;

		pDstT = pDstCurRow;

		for( DWORD x=0; x<d3dsd.Width; x++ )
		{
			LONG v00; // Current pixel
			LONG v01; // Pixel to the right of current pixel, wrapping to left edge if necessary
			LONG vM1; // Pixel to the left of current pixel, wrapping to right edge if necessary
			LONG v10; // Pixel one line below.
			LONG v1M; // Pixel one line above.

			v00 = *(pSrcB0+0);

			if( x == d3dsd.Width - 1 )
				v01 = *(pSrcCurRow);
			else
				v01 = *(pSrcB0+4);

			if( x == 0 )
				vM1 = *(pSrcCurRow + (4 * (d3dsd.Width - 1)));
			else
				vM1 = *(pSrcB0-4);
			v10 = *(pSrcB1+0);
			v1M = *(pSrcB2+0);

			LONG iDu = (vM1-v01); // The delta-u bump value
			LONG iDv = (v1M-v10); // The delta-v bump value

			// The luminance bump value (land masses are less shiny)
			WORD uL = ( v00>1 ) ? 63 : 127;

			switch( m_BumpMapFormat )
			{
			case D3DFMT_V8U8:
				*pDstT++ = (BYTE)iDu;
				*pDstT++ = (BYTE)iDv;
				break;

			case D3DFMT_L6V5U5:
				*(WORD*)pDstT  = (WORD)( ( (iDu>>3) & 0x1f ) <<  0 );
				*(WORD*)pDstT |= (WORD)( ( (iDv>>3) & 0x1f ) <<  5 );
				*(WORD*)pDstT |= (WORD)( ( ( uL>>2) & 0x3f ) << 10 );
				pDstT += 2;
				break;

			case D3DFMT_X8L8V8U8:
				*pDstT++ = (BYTE)iDu;
				*pDstT++ = (BYTE)iDv;
				*pDstT++ = (BYTE)uL;
				*pDstT++ = (BYTE)0L;
				break;
			}

			// Move one pixel to the right (src is 32-bpp)
			pSrcB0+=4;
			pSrcB1+=4;
			pSrcB2+=4;
		}

		// Move to the next line
		pSrcCurRow += dwSrcPitch;
		pDstCurRow += dwDstPitch;
	}

	m_psBumpMap->UnlockRect(0);
	psBumpSrc->UnlockRect(0);

	SAFE_RELEASE(psBumpSrc);
	lpTexture = m_psBumpMap;

	return S_OK;
}

HRESULT KTexture::LoadBumpMap(LPSTR pFileName)
{
	if(FAILED(LoadFromFile(pFileName)))
	{
		return E_FAIL;
	}

	if(FAILED(Exchange2Bump(m_lpTexture)))
	{
		return E_FAIL;
	}

    return S_OK;
}

HRESULT KTexture::LoadCubeMap(LPSTR pFileName)
{
	HRESULT hr = S_OK;
	if ( FAILED(hr = D3DXCreateCubeTextureFromFile( g_pd3dDevice, pFileName,(LPDIRECT3DCUBETEXTURE9*)(&m_lpTexture) ) ) )
        return hr;
	m_lpFileName = new TCHAR[256];
	wsprintf(m_lpFileName,pFileName);
	return S_OK;
}

HRESULT KTexture::LoadVolumeMap(LPSTR pFileName)
{
	HRESULT hr = S_OK;
	if ( FAILED(hr = D3DXCreateVolumeTextureFromFile( g_pd3dDevice, pFileName,(LPDIRECT3DVOLUMETEXTURE9*)(&m_lpTexture) ) ) )
		return hr;
	m_lpFileName = new TCHAR[256];
	wsprintf(m_lpFileName,pFileName);
	return S_OK;
}

HRESULT KTexture::CreateFromText(TCHAR* strFontName,
								 TCHAR* Texts,
								 WORD   FontHeight,
								 DWORD* pTextureWidth,
								 DWORD* pTextureHeight,
								 DWORD  nFlag)
{
	nFlag = 0;
    float fTextScale  = 1.0f; // Draw fonts into texture without scaling
	TCHAR  FontName[200] ;
	TCHAR  m_Text[300];
	wsprintf(m_Text,"%s",Texts);

	if (strFontName==NULL)
		wsprintf(FontName,"%s",g_Def_FontName);
	else
		wsprintf(FontName,"%s",strFontName);
	if (FontHeight==0)
		FontHeight = g_Def_FontHeight; 
	WORD Strlen = (WORD)(strlen(m_Text));

	DWORD T_Width  = (DWORD)( FontHeight * Strlen * 0.68f);
    DWORD T_Height = (DWORD)( FontHeight * 1.6f);
	(*pTextureWidth) = T_Width;
	(*pTextureHeight) = T_Height;
    // If requested texture is too big, use a smaller texture and smaller font,
    // and scale up when rendering.
    D3DCAPS9 d3dCaps;
    g_pd3dDevice->GetDeviceCaps( &d3dCaps );

    if ( T_Width > d3dCaps.MaxTextureWidth )
    {
        fTextScale = (FLOAT)d3dCaps.MaxTextureWidth / (FLOAT)T_Width;
        T_Width = T_Height = (WORD) d3dCaps.MaxTextureWidth;
    }
    // Create a new texture for the font
	LPDIRECT3DTEXTURE9 pTexture = NULL;
    /*HRESULT hr = g_pd3dDevice->CreateTexture( T_Width, T_Height, 1,
                                      0, D3DFMT_A4R4G4B4,
                                      D3DPOOL_MANAGED, &pTexture,NULL );*/
	HRESULT hr = D3DXCreateTexture(g_pd3dDevice,T_Width,T_Height,1,0,D3DFMT_A8R8G8B8,D3DPOOL_MANAGED,&pTexture);
    if ( FAILED(hr) )
        return hr;
    // Prepare to create a bitmap
    DWORD*      pBitmapBits;
    BITMAPINFO bmi;
    ZeroMemory( &bmi.bmiHeader,  sizeof(BITMAPINFOHEADER) );
    bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth       =  (int)T_Width;
    bmi.bmiHeader.biHeight      = -(int)T_Height;
    bmi.bmiHeader.biPlanes      = 1;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biBitCount    = 32;
   
	// Create a DC and a bitmap for the font
    HDC     hDC       = CreateCompatibleDC( NULL );
    HBITMAP hbmBitmap = CreateDIBSection( hDC, &bmi, DIB_RGB_COLORS,
                                          (VOID**)&pBitmapBits, NULL, 0 );

    SetMapMode( hDC, MM_TEXT );
    // Create a font.  By specifying ANTIALIASED_QUALITY, we might get an
    // antialiased font, but this is not guaranteed.
    INT nHeight    = -MulDiv( FontHeight, 
        (INT)(GetDeviceCaps(hDC, LOGPIXELSY) * fTextScale), 72 );
    //(nFlag&D3DFONT_BOLD)   ? FW_BOLD :(nFlag&D3DFONT_ITALIC) ? TRUE    : 
	DWORD dwBold   = FW_NORMAL;
    DWORD dwItalic = FALSE;
    HFONT hFont    = CreateFont( nHeight, 0, 0, 0, dwBold, dwItalic,
                          FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                          CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
                          VARIABLE_PITCH, FontName );
    if ( NULL==hFont )
        return E_FAIL;

    SelectObject( hDC, hbmBitmap );
    SelectObject( hDC, hFont );
    // Set text properties
    SetTextColor( hDC, RGB(255,255,255) );
    SetBkColor(   hDC, 0x00000000 );
    SetTextAlign( hDC, TA_TOP );

    // Loop through all printable character and output them to the bitmap..
    // Meanwhile, keep track of the corresponding tex coords for each character.
    DWORD x = 0;
    DWORD y = 0;
    SIZE size;

	GetTextExtentPoint32( hDC, m_Text, 1, &size );
	ExtTextOut( hDC, x+0, y+0, ETO_OPAQUE, NULL, m_Text , Strlen, NULL );

    // Lock the surface and write the alpha values for the set pixels
    D3DLOCKED_RECT d3dlr;
    pTexture->LockRect( 0, &d3dlr, 0, 0 );
    DWORD* pDst32 = (DWORD*)d3dlr.pBits;
    BYTE bAlpha; // 4-bit measure of pixel intensity
    for( y=0; y < T_Height; y++ )
    {
        for( x=0; x < T_Width; x++ )
        {
            bAlpha = (BYTE)((pBitmapBits[T_Width*y + x] & 0xff) >> 4);
            if (bAlpha > 0)
            {
				bAlpha = 0xFF;
                *pDst32++ = (bAlpha << 24) | 0x00FFFFFF;
            }
            else
            {
                *pDst32++ = 0x00000000;
            }
        }
    }

    // Done updating texture, so clean up used objects
    pTexture->UnlockRect(0);

    DeleteObject( hbmBitmap );
    DeleteDC( hDC );
    DeleteObject( hFont );
	SAFE_RELEASE(m_lpTexture);
	m_lpTexture = pTexture;
	return S_OK;
}

HRESULT KTexture::CreateFromText(TCHAR* strFontName,
								 TCHAR* Texts,
								 WORD   FontHeight,
								 DWORD* pTextureWidth,
								 DWORD* pTextureHeight,
								 DWORD  nFlag,
								 DWORD  nChar,
								 DWORD& nCharPos
								 )
{
	nFlag = 0;
	float fTextScale  = 1.0f; // Draw fonts into texture without scaling
	TCHAR  FontName[200] ;
	TCHAR  m_Text[300];
	wsprintf(m_Text,"%s",Texts);

	if (strFontName==NULL)
		wsprintf(FontName,"%s",g_Def_FontName);
	else
		wsprintf(FontName,"%s",strFontName);
	if (FontHeight==0)
		FontHeight = g_Def_FontHeight; 
	WORD Strlen = (WORD)(strlen(m_Text));

	DWORD T_Width  = (DWORD)( FontHeight * Strlen * 0.68f);
	DWORD T_Height = (DWORD)( FontHeight * 1.6f);
	(*pTextureWidth) = T_Width;
	(*pTextureHeight) = T_Height;
	// If requested texture is too big, use a smaller texture and smaller font,
	// and scale up when rendering.
	D3DCAPS9 d3dCaps;
	g_pd3dDevice->GetDeviceCaps( &d3dCaps );

	if ( T_Width > d3dCaps.MaxTextureWidth )
	{
		fTextScale = (FLOAT)d3dCaps.MaxTextureWidth / (FLOAT)T_Width;
		T_Width = T_Height = (WORD) d3dCaps.MaxTextureWidth;
	}
	// Create a new texture for the font
	LPDIRECT3DTEXTURE9 pTexture = NULL;
	/*HRESULT hr = g_pd3dDevice->CreateTexture( T_Width, T_Height, 1,
	0, D3DFMT_A4R4G4B4,
	D3DPOOL_MANAGED, &pTexture,NULL );*/
	HRESULT hr = D3DXCreateTexture(g_pd3dDevice,T_Width,T_Height,1,0,D3DFMT_A8R8G8B8,D3DPOOL_MANAGED,&pTexture);
	if ( FAILED(hr) )
		return hr;
	// Prepare to create a bitmap
	DWORD*      pBitmapBits;
	BITMAPINFO bmi;
	ZeroMemory( &bmi.bmiHeader,  sizeof(BITMAPINFOHEADER) );
	bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth       =  (int)T_Width;
	bmi.bmiHeader.biHeight      = -(int)T_Height;
	bmi.bmiHeader.biPlanes      = 1;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biBitCount    = 32;

	// Create a DC and a bitmap for the font
	HDC     hDC       = CreateCompatibleDC( NULL );
	HBITMAP hbmBitmap = CreateDIBSection( hDC, &bmi, DIB_RGB_COLORS,
		(VOID**)&pBitmapBits, NULL, 0 );

	SetMapMode( hDC, MM_TEXT );
	// Create a font.  By specifying ANTIALIASED_QUALITY, we might get an
	// antialiased font, but this is not guaranteed.
	INT nHeight    = -MulDiv( FontHeight, 
		(INT)(GetDeviceCaps(hDC, LOGPIXELSY) * fTextScale), 72 );
	//(nFlag&D3DFONT_BOLD)   ? FW_BOLD :(nFlag&D3DFONT_ITALIC) ? TRUE    : 
	DWORD dwBold   = FW_NORMAL;
	DWORD dwItalic = FALSE;
	HFONT hFont    = CreateFont( nHeight, 0, 0, 0, dwBold, dwItalic,
		FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
		VARIABLE_PITCH, FontName );
	if ( NULL==hFont )
		return E_FAIL;

	SelectObject( hDC, hbmBitmap );
	SelectObject( hDC, hFont );
	// Set text properties
	SetTextColor( hDC, RGB(255,255,255) );
	SetBkColor(   hDC, 0x00000000 );
	SetTextAlign( hDC, TA_TOP );

	// Loop through all printable character and output them to the bitmap..
	// Meanwhile, keep track of the corresponding tex coords for each character.
	DWORD x = 0;
	DWORD y = 0;
	SIZE size;

	GetTextExtentPoint32( hDC, m_Text, 1, &size );
	ExtTextOut( hDC, x+0, y+0, ETO_OPAQUE, NULL, m_Text , Strlen, NULL );

	// Lock the surface and write the alpha values for the set pixels
	D3DLOCKED_RECT d3dlr;
	pTexture->LockRect( 0, &d3dlr, 0, 0 );
	DWORD* pDst32 = (DWORD*)d3dlr.pBits;
	BYTE bAlpha; // 4-bit measure of pixel intensity
	for( y=0; y < T_Height; y++ )
	{
		for( x=0; x < T_Width; x++ )
		{
			bAlpha = (BYTE)((pBitmapBits[T_Width*y + x] & 0xff) >> 4);
			if (bAlpha > 0)
			{
				bAlpha = 0xFF;
				*pDst32++ = (bAlpha << 24) | 0x00FFFFFF;
			}
			else
			{
				*pDst32++ = 0x00000000;
			}
		}
	}

	// Done updating texture, so clean up used objects
	pTexture->UnlockRect(0);

	DeleteObject( hbmBitmap );
	DeleteDC( hDC );
	DeleteObject( hFont );
	SAFE_RELEASE(m_lpTexture);
	m_lpTexture = pTexture;
	return S_OK;
}
HRESULT KTexture::SetAlpha(D3DCOLOR Color,BYTE Alpha)
{
	HRESULT hr = S_OK;
	D3DSURFACE_DESC Desc;
	if (FAILED(hr = m_lpTexture->GetLevelDesc(0,&Desc)))
		return hr;
	D3DFORMAT  Format = Desc.Format ;
	if (Format==D3DFMT_A8R8G8B8)
		return S_OK;
	UINT       Width  = Desc.Width  ;
	UINT       Height = Desc.Height ;

	LPDIRECT3DTEXTURE9 pTempText = NULL;
	if (FAILED(D3DXCreateTexture(g_pd3dDevice,Width,Height,
		1,0,D3DFMT_A8R8G8B8,D3DPOOL_MANAGED,&pTempText)))
		return E_FAIL;

	BYTE R =(BYTE)((Color&0x00FF0000)>>16);
	BYTE G =(BYTE)((Color&0x0000FF00)>>8);
	BYTE B =(BYTE)((Color&0x000000FF));

	D3DLOCKED_RECT LockRect ;
	if (FAILED(hr = m_lpTexture->LockRect(0,&LockRect,NULL,0)))
		return hr;
	BYTE * pPixs = (BYTE*) LockRect.pBits;

	D3DLOCKED_RECT LockRectDest ;
	if (FAILED(hr = pTempText->LockRect(0,&LockRectDest,NULL,0)))
		return hr;
	BYTE * pPixsDest = (BYTE*) LockRectDest.pBits;

	if (Format==D3DFMT_X8R8G8B8)
	{
		for(UINT i=0;i<Height;i++)
		{
			for(UINT j=0;j<Width ;j++)
			{
				BYTE* pAlpha = &pPixs[(i*Width+j)*4+3];
				BYTE  RR = pPixs[(i*Width+j)*4+2];
				BYTE  GG = pPixs[(i*Width+j)*4+1];
				BYTE  BB = pPixs[(i*Width+j)*4+0];

				pPixsDest[(i*Width+j)*4+2] = RR;
				pPixsDest[(i*Width+j)*4+1] = GG;
				pPixsDest[(i*Width+j)*4+0] = BB;

				if ((RR==R)&&(GG==G)&&(BB==B))
				{
					pPixsDest[(i*Width+j)*4+3] = Alpha;
					(*pAlpha) = Alpha;
				}
				else
				{
					pPixsDest[(i*Width+j)*4+3] = *pAlpha;
				}
			}
		}
		//Format = D3DFMT_A8R8G8B8;
	}
	m_lpTexture->UnlockRect(0);
	pTempText->UnlockRect(0);

	SAFE_RELEASE(m_lpTexture);
	m_lpTexture = pTempText;
	return S_OK;
}

HRESULT KTexture::InvalidateDeviceObjects(void)
{
	return E_NOTIMPL;
}

HRESULT KTexture::RestoreDeviceObjects(void)
{
	return E_NOTIMPL;
}

HRESULT KTexture::ReloadFromFile()
{
	WIN32_FIND_DATA Find;
	HANDLE hFind = FindFirstFile(m_lpFileName,&Find);
	FILETIME Time;
	::GetFileTime(hFind,NULL,NULL,&Time);
	FindClose(hFind);

	if((m_FileTime.dwHighDateTime!=Time.dwHighDateTime)||
	   (m_FileTime.dwLowDateTime!=Time.dwLowDateTime))
	{
		LoadFromFile(m_lpFileName);
	}

	return S_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////////

// 根据贴图的长宽gMemTexsForUpdate
//int TexSizeToIndex(int nWidth, int nHeight)
//{
//	if( nWidth < 1 || nWidth > 1024 )
//		return -1;
//	if( nHeight < 1 || nHeight > 1024 )
//		return -1;
//
//	int nW = 0, nH = 0;
//	for(;;)
//	{
//		if( nWidth == 1 )
//			break;
//		nWidth /= 2;
//		nW++;
//	}
//
//	for(;;)
//	{
//		if( nHeight == 1 )
//			break;
//		nHeight /= 2;
//		nH++;
//	}
//
//	int nIdx = nH * 11 + nW;
//	assert(nIdx >= 0 && nIdx < MAX_BACK_TEXTURE_NUM);
//	return nIdx;
//}
//
//inline void RenderToA4R4G4B4(WORD* pDest, int nPitch, BYTE* pSrc, int width, int height, BYTE* pPalette)
//{
//	int nLineAdd = nPitch - width * 2;
//	__asm
//	{
//
//		//设置源与目的地址至esi和edi
//		mov		edi, pDest
//			mov		esi, pSrc
//
//loc_DrawSprite_0100:
//
//		//edx存放宽度	
//		mov		edx, width
//
//loc_DrawSprite_0101:
//
//		//压缩格式为 n个alpha [rgba,rgba,.......],alpha为0时，无rgba各值
//
//		movzx	eax, byte ptr[esi]//ax存放n
//		inc		esi
//			movzx	ebx, byte ptr[esi]//bx存放alpha值
//			//mov		nAlpha, ebx
//			inc		esi
//				or		ebx, ebx
//				jnz		loc_DrawSprite_0102
//
//				push	eax	
//				mov		ecx, eax
//
//loc_FillZeroAlpha:
//
//			//romandou
//
//			mov		word ptr[edi], 0
//				inc		edi
//				inc		edi
//				dec		ecx
//				jnz     loc_FillZeroAlpha
//				pop     eax
//
//				sub		edx, eax
//				jg		loc_DrawSprite_0101
//				add		edi, nLineAdd
//				dec		height
//				jnz		loc_DrawSprite_0100
//				jmp		loc_DrawSprite_exit
//
//
//				//alpha值不为0时
//loc_DrawSprite_0102:
//
//			push	eax // n值
//				push	edx // 当前行还剩多长未处理 
//
//				and		bx, 0x00f0
//				shl		bx, 8
//				push	ebx	// nAlpha值
//
//				mov		ecx, eax
//
//
//loc_DrawSprite_0103:
//
//			mov     ebx, pPalette
//				movzx	eax, byte ptr[esi]
//				inc		esi
//					mov		dx, [ebx + eax * 2]  //获得 16位 r4g4b4a4 a = 0
//
//					pop     ebx   //ebx = nAlpha
//						push	ebx  
//
//						or		dx, bx //生成最终的r4g4b4a4值
//						mov		[edi], dx
//						inc		edi
//						inc		edi
//						dec		ecx
//						jnz		loc_DrawSprite_0103
//						pop     ebx   //ebx = nAlpha
//						pop		edx   //edx = 该行剩于
//						pop		eax   //eax = n
//						sub		edx, eax
//						jg		loc_DrawSprite_0101
//						add		edi, nLineAdd
//						dec		height
//						jnz		loc_DrawSprite_0100
//						jmp		loc_DrawSprite_exit
//
//loc_DrawSprite_exit:
//	}
//}
//
//KTextureResSpr::KTextureResSpr()
//{
//	ResetVar();
//}
//
//KTextureResSpr::~KTextureResSpr()
//{
//	Clear();
//}
//
//// 将成员变量置为初始值
//void KTextureResSpr::ResetVar()
//{
//	m_nWidth		= 0;
//	m_nHeight		= 0;
//	m_nFrameNum		= 1;
//	m_nCenterX		= 0;
//	m_nCenterY		= 0;
//	m_nColors		= 0;
//	m_nDirections	= 1;
//	m_nInterval		= 1;
//
//	//m_pPal24		= NULL;
//	//m_pPal16		= NULL;
//	m_pFrameInfo	= NULL;
//	m_pHeader		= NULL;
//
//	m_nTexMemUsed = 0;
//	m_nSprMemUsed = 0;
//	//m_bLastFrameUsed = false;
//	m_bPacked = false;
//	//psprHead = NULL;
//	m_bCacheable = false;
//	m_bNeedReDraw = true;
//}
//
//// 创建内存资源
//bool KTextureResSpr::CreateImage(const char* szImage, int nWidth, int nHeight, uint32 nType)
//{
//	return false;
//}
//
//// 从文件载入资源
//int KTextureResSpr::LoadImage(char* szImage, uint32 nType)
//{
//	if (!szImage || !szImage[0] || nType != ISI_T_SPR)
//		return REPRESENT_ERR_FILENAME;
//
//	Clear();
//	m_nType = nType;
//	strcpy(m_szFileName , szImage);
//	return LoadSprFile(szImage);
//}
//
//int KTextureResSpr::LoadSprFile(char* szImage)
//{
//	if(!szImage)
//		return REPRESENT_ERR_FILENAME;
//
//	int i;
//	PBYTE palette;
//	char *pFrameData;
//	SPRFRAME *pFrame;
//	SPROFFS *pOffset;
//	PBYTE pSprite;
//
//	SPRHEAD *psprHead;
//	m_pHeader = (char *)SprGetHeader(szImage, (SPROFFS*&)pFrameData);
//	if(!m_pHeader)
//		return REPRESENT_ERR_FILE_OPEN;
//
//	psprHead = (SPRHEAD*)m_pHeader;
//	if(psprHead->Frames == 0)
//	{
//		SprReleaseHeader(psprHead);
//		Clear();
//		return REPRESENT_ERR_FILE_CONTENT;
//	}
//
//	//读取调色板
//	palette = ((PBYTE)psprHead);
//	palette += sizeof(SPRHEAD);
//	//m_pPal24 = new KPAL24[psprHead->Colors];
//	memcpy(m_pPal24, palette, psprHead->Colors * sizeof(KPAL24));
//	//m_pPal16 = new WORD[psprHead->Colors];
//	for(i = 0; i < psprHead->Colors; i++)
//	{
//		m_pPal16[i] = ARGBTO4444(0, m_pPal24[i].Red, m_pPal24[i].Green, m_pPal24[i].Blue);
//	}
//
//	m_nWidth		= psprHead->Width;
//	m_nHeight		= psprHead->Height;
//	m_nCenterX		= psprHead->CenterX;
//	m_nCenterY		= psprHead->CenterY;
//	m_nColors		= psprHead->Colors;
//	m_nDirections	= psprHead->Directions;
//	m_nInterval		= psprHead->Interval;
//	m_nFrameNum		= psprHead->Frames;
//
//
//	m_pFrameInfo = new FrameToTexture[m_nFrameNum];
//	if(m_pFrameInfo == NULL)
//	{
//		SprReleaseHeader(psprHead);
//		Clear();
//		return REPRESENT_ERR_FILE_CONTENT;
//	}
//
//	ZeroMemory(m_pFrameInfo, sizeof(FrameToTexture) * m_nFrameNum);
//
//	//判断分帧压缩的标志
//	if(pFrameData)  //是整体压缩
//	{
//		// 整体压缩，拷贝所有帧的原始数据
//		pSprite = (LPBYTE)pFrameData + m_nFrameNum * sizeof(SPROFFS);
//		pOffset = (SPROFFS*)pFrameData;
//		for(i = 0; i < m_nFrameNum; i++)
//		{
//			pFrame = (SPRFRAME*)(pSprite + pOffset[i].Offset); 
//			m_pFrameInfo[i].nWidth = pFrame->Width;
//			m_pFrameInfo[i].nHeight = pFrame->Height;
//			m_pFrameInfo[i].nTexNum = 0;
//			m_pFrameInfo[i].nOffX = pFrame->OffsetX;
//			m_pFrameInfo[i].nOffY = pFrame->OffsetY;
//			m_pFrameInfo[i].nRawDataLen = pOffset[i].Length - sizeof(SPRFRAME);
//			//m_pFrameInfo[i].pRawData = new BYTE[m_pFrameInfo[i].nRawDataLen];
//			//memcpy(m_pFrameInfo[i].pRawData, pFrame->Sprite, m_pFrameInfo[i].nRawDataLen);
//			m_pFrameInfo[i].pRawData = pFrame->Sprite;
//			m_pFrameInfo[i].pFrame = NULL;
//
//			m_nSprMemUsed += m_pFrameInfo[i].nRawDataLen;
//		}
//		m_bPacked = false;
//	}
//	else
//	{
//		m_bPacked = true;	// 按帧压缩
//	}
//
//	return REPRESENT_OK;
//}
//
//int KTextureResSpr::PrepareFrameData(const char* szImage, int32 nFrame, bool bPrepareTex)
//{
//	if(szImage == NULL || szImage[0] == NULL)
//		return REPRESENT_ERR_FILENAME;
//
//	if(nFrame < 0 || nFrame >= m_nFrameNum)
//		return REPRESENT_ERR;
//
//	if(m_pFrameInfo[nFrame].texInfo[0].pTexture)
//		return REPRESENT_OK;
//
//	//if(!m_pFrameInfo[nFrame].pRawData)
//	if(m_bPacked)	//是按帧压缩的
//	{
//		if(!m_pHeader)
//			return REPRESENT_ERR;
//
//		unsigned uFrameSize;
//		SPRFRAME *pFrame = (SPRFRAME *)SprGetFrame(szImage, (SPRHEAD*)m_pHeader, nFrame, uFrameSize);
//
//		if(!pFrame)
//			return REPRESENT_ERR;
//
//		m_pFrameInfo[nFrame].nWidth = pFrame->Width;
//		m_pFrameInfo[nFrame].nHeight = pFrame->Height;
//		m_pFrameInfo[nFrame].nTexNum = 0;
//		m_pFrameInfo[nFrame].nOffX = pFrame->OffsetX;
//		m_pFrameInfo[nFrame].nOffY = pFrame->OffsetY;
//
//		m_pFrameInfo[nFrame].nRawDataLen = uFrameSize - sizeof(SPRFRAME);
//		m_pFrameInfo[nFrame].pRawData = pFrame->Sprite;
//		m_nSprMemUsed += m_pFrameInfo[nFrame].nRawDataLen;
//		m_pFrameInfo[nFrame].pFrame = pFrame;
//	}
//
//	//if(bPrepareTex)
//	//	return CreateTexture16Bit(nFrame);
//
//	return REPRESENT_OK;
//}
//
//LPDIRECT3DTEXTURE9 KTextureResSpr::GetTexture(int32 nFrame, int nIdx)
//{
//	//assert(m_pFrameInfo[nFrame].nTexNum);
//	//assert(nIdx >= 0 && nIdx < m_pFrameInfo[nFrame].nTexNum);
//	if(m_pFrameInfo[nFrame].nTexNum <= 0)
//	{
///*
//#ifdef _DEBUG
//		g_LogFile.LogRecordVar(LOG_ATTENTION, "spr文件取贴图帧错误 %d", nFrame);
//#endif
//*/
//		return NULL;
//	}
//
//	if(nIdx < 0 || nIdx >= m_pFrameInfo[nFrame].nTexNum)
//	{
///*
//#ifdef _DEBUG
//		g_LogFile.LogRecordVar(LOG_ATTENTION, "spr文件取贴图帧,索引错误 %d %d", nFrame, nIdx);
//#endif
//*/
//		return NULL;
//	}
//
//	if(nFrame < 0 || nFrame >= m_nFrameNum)
//		return NULL;
//
//	return m_pFrameInfo[nFrame].texInfo[nIdx].pTexture;
//}
//
//int KTextureResSpr::CreateTexture16Bit(int32 nFrame,LPDIRECT3DDEVICE9 pD3DDevice)
//{
//	if(nFrame < 0 || nFrame >= m_nFrameNum)
//		return REPRESENT_ERR;
//
//	if(m_pFrameInfo[nFrame].texInfo[0].pTexture)
//		return REPRESENT_OK;
//
//	if(m_pFrameInfo[nFrame].pRawData == NULL)
//		return REPRESENT_ERR;
//
//	if(!g_pSprExchangeBuffer)	//全局变量用来加速
//		return REPRESENT_ERR_OUTMEMORY;
//
//	SplitTexture(nFrame);
//
//	RenderToA4R4G4B4((WORD*)g_pSprExchangeBuffer, 
//		m_pFrameInfo[nFrame].nWidth * 2, 
//		m_pFrameInfo[nFrame].pRawData, 
//		m_pFrameInfo[nFrame].nWidth,
//		m_pFrameInfo[nFrame].nHeight, 
//		(BYTE*)m_pPal16);
//
//	int i;
//	/*
//	WORD* pData = (WORD*)pTempData;
//	for(i = 0; i < m_pFrameInfo[nFrame].nHeight; i++)
//	for(int j = 0; j < m_pFrameInfo[nFrame].nWidth; j++)
//	{
//	WORD d = *(pData + i * m_pFrameInfo[nFrame].nWidth + j);
//	WORD r,g,b,a;
//
//	a = (d & 0xF000);
//	r = (d & 0x0F00) >> 8;
//	g = (d & 0x00F0) >> 4;
//	b = (d & 0x000F);
//	WORD ray =WORD((float)r * 0.3f + (float)g * 0.59 + (float)b * 0.11);
//
//	d = a | ray<<8 | ray << 4 | ray;
//
//	*(pData + i * m_pFrameInfo[nFrame].nWidth + j) = d;
//	}
//	*/
//	// 创建并填充贴图	
//
//	for(i = 0 ; i < m_pFrameInfo[nFrame].nTexNum ; i++)
//	{
//		SAFE_RELEASE(m_pFrameInfo[nFrame].texInfo[i].pTexture);
//		///创建在显存中
//		if (FAILED(D3DXCreateTexture(pD3DDevice , 
//			m_pFrameInfo[nFrame].texInfo[i].nWidth , 
//			m_pFrameInfo[nFrame].texInfo[i].nHeight , 
//			1 , 
//			0 , 
//			D3DFMT_A4R4G4B4 , 
//			D3DPOOL_DEFAULT , 
//			&m_pFrameInfo[nFrame].texInfo[i].pTexture)))
//		{
//			//删除临时分配的内存
//			//SAFE_DELETE_ARRAY(pTempData);
//			return REPRESENT_ERR_OUTMEMORY;
//		}
//		m_nTexMemUsed += m_pFrameInfo[nFrame].texInfo[i].nWidth * m_pFrameInfo[nFrame].texInfo[i].nHeight * 2;
//
//		///创建了备份贴图
//		int nIdx = TexSizeToIndex(m_pFrameInfo[nFrame].texInfo[i].nWidth, m_pFrameInfo[nFrame].texInfo[i].nHeight);
//		if( gMemTexsForUpdate[nIdx] == NULL )
//		{
//			if (FAILED(D3DXCreateTexture(pD3DDevice , 
//				m_pFrameInfo[nFrame].texInfo[i].nWidth , 
//				m_pFrameInfo[nFrame].texInfo[i].nHeight , 
//				1 , 
//				0 , 
//				D3DFMT_A4R4G4B4 , 
//				D3DPOOL_SYSTEMMEM , 
//				&gMemTexsForUpdate[nIdx])))
//			{
//				gMemTexsForUpdate[nIdx] = NULL;
//				//删除临时分配的内存
//				//SAFE_DELETE_ARRAY(pTempData);
//				return REPRESENT_ERR_OUTMEMORY;
//			}
//		}
//
//		D3DLOCKED_RECT LockedRect;
//		if (FAILED(gMemTexsForUpdate[nIdx]->LockRect(0, &LockedRect, NULL, D3DLOCK_NOSYSLOCK)))
//		{
//			//删除临时分配的内存
//			//SAFE_DELETE_ARRAY(pTempData);
//			return REPRESENT_ERR_OUTMEMORY;
//		}
//
//		// 向贴图拷贝数据
//		BYTE* pTexData = (BYTE*)LockedRect.pBits;		
//		// 进行一个图形偏移的运算
//		BYTE* pTp = g_pSprExchangeBuffer + (m_pFrameInfo[nFrame].texInfo[i].nFrameY * m_pFrameInfo[nFrame].nWidth + 
//			m_pFrameInfo[nFrame].texInfo[i].nFrameX) * 2;
//
//		for(int j = 0 ; j < m_pFrameInfo[nFrame].texInfo[i].nFrameHeight ; j++)
//		{
//			memcpy(pTexData , pTp , m_pFrameInfo[nFrame].texInfo[i].nFrameWidth * 2);
//			pTexData += LockedRect.Pitch;
//			pTp += m_pFrameInfo[nFrame].nWidth * 2;
//		}
//
//		gMemTexsForUpdate[nIdx]->UnlockRect(0);
//		pD3DDevice->UpdateTexture(gMemTexsForUpdate[nIdx], m_pFrameInfo[nFrame].texInfo[i].pTexture);
//		m_bCacheable = true;
//	}
//
//	// 已经创建贴图，如果为可按单帧载入的资源则释放这一帧的spr数据
//	if(m_bPacked)
//	{
//		SprReleaseFrame((SPRFRAME*)m_pFrameInfo[nFrame].pFrame);
//		m_pFrameInfo[nFrame].pFrame = NULL;
//		m_pFrameInfo[nFrame].pRawData = NULL;
//		m_nSprMemUsed -= m_pFrameInfo[nFrame].nRawDataLen;
//	}
//
//	//SAFE_DELETE_ARRAY(pTempData);
//	return REPRESENT_OK;
//}

/*抽点
LPDIRECT3DTEXTURE8 KTextureResSpr::CreateTexture16Bit(uint32 nFrame)
{
if(nFrame >= m_nFrameNum)
return NULL;

if(m_pTextureInfo[nFrame].pTexture)
return m_pTextureInfo[nFrame].pTexture;

if(!m_pFrameInfo[nFrame].pRawData)
return NULL;

// 创建贴图
if (FAILED(PD3DDEVICE->CreateTexture(m_pTextureInfo[nFrame].nWidth, m_pTextureInfo[nFrame].nHeight, 1,
0, D3DFMT_A4R4G4B4, D3DPOOL_MANAGED, &m_pTextureInfo[nFrame].pTexture)))
return NULL;

D3DLOCKED_RECT LockedRect;
if (FAILED(m_pTextureInfo[nFrame].pTexture->LockRect(0, &LockedRect, NULL, 0)))
{
SAFE_RELEASE(m_pTextureInfo[nFrame].pTexture);
return NULL;
}

m_nTexMemUsed += m_pTextureInfo[nFrame].nWidth * m_pTextureInfo[nFrame].nHeight * 2;
m_nMemUseful += m_pFrameInfo[nFrame].nWidth * m_pFrameInfo[nFrame].nHeight * 2;

// 填充贴图数据
uint32 nByteCount = 0;							// 总字节计数
uint32 nPixelCount = 0;							// 总象素计数
uint32 nWidth = m_pFrameInfo[nFrame].nWidth;
uint32 nHeight = m_pFrameInfo[nFrame].nHeight;
uint32 nFrameLen = m_pFrameInfo[nFrame].nRawDataLen;
BYTE *pData = m_pFrameInfo[nFrame].pRawData;
BYTE pixelNum, alpha, pixelColor;
BYTE *pTexData = (BYTE*)LockedRect.pBits;		// 贴图数据
BYTE *pTexLine = pTexData;						// 贴图每一行数据
int nTp2 = 0;
for(;;)
{
pixelNum = *pData++;
alpha = *pData++;
nByteCount += 2;
nPixelCount += pixelNum;

if(alpha == 0)
{
if((nTp2%2) == 1)
for(int l=0; l<pixelNum; l++)
{

*((WORD*)pTexLine) = 0;
pTexLine += 2;
}
}
else
{
if((nTp2%2) == 1)
for(int l=0; l<pixelNum; l++)
{
pixelColor = *pData++;
//	*((WORD*)pTexLine) = ARGBTO4444(alpha, m_pPal24[pixelColor].Red, m_pPal24[pixelColor].Green, m_pPal24[pixelColor].Blue);
*((WORD*)pTexLine) = ((((WORD)alpha)<<8)&0xf000) | m_pPal16[pixelColor];
pTexLine += 2;
}
else
pData += pixelNum;
nByteCount += pixelNum;
}

if(nPixelCount == nWidth)
{
// 如果一帧数据解完则停止
if(nByteCount >= nFrameLen)
break;
if((nTp2%2) == 1)
pTexLine = pTexData = pTexData + LockedRect.Pitch;
nPixelCount = 0;
nTp2++;
}
}

m_pTextureInfo[nFrame].pTexture->UnlockRect(0);

SAFE_DELETE_ARRAY(m_pFrameInfo[nFrame].pRawData);
return m_pTextureInfo[nFrame].pTexture;
}
*/

//void KTextureResSpr::CreateTexture32Bit(uint32 nFrame)
//{
//	return;
//	/*	if(nFrame >= m_nFrameNum)
//	return NULL;
//
//	if(m_pTextureInfo[nFrame].pTexture)
//	return m_pTextureInfo[nFrame].pTexture;
//
//	if(!m_pFrameInfo[nFrame].pRawData)
//	return NULL;
//
//	// 创建贴图
//	if (FAILED(PD3DDEVICE->CreateTexture(m_pTextureInfo[nFrame].nWidth, m_pTextureInfo[nFrame].nHeight, 1,
//	0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &m_pTextureInfo[nFrame].pTexture)))
//	return NULL;
//
//	D3DSURFACE_DESC desc;
//	m_pTextureInfo[nFrame].pTexture->GetLevelDesc(0, &desc);
//
//	D3DLOCKED_RECT LockedRect;
//	if (FAILED(m_pTextureInfo[nFrame].pTexture->LockRect(0, &LockedRect, NULL, 0)))
//	{
//	SAFE_RELEASE(m_pTextureInfo[nFrame].pTexture);
//	return NULL;
//	}
//
//	m_nTexMemUsed += m_pTextureInfo[nFrame].nWidth * m_pTextureInfo[nFrame].nHeight * 4;
//	m_nMemUseful += m_pFrameInfo[nFrame].nWidth * m_pFrameInfo[nFrame].nHeight * 4;
//
//	// 填充贴图数据
//	uint32 nByteCount = 0;							// 总字节计数
//	uint32 nPixelCount = 0;							// 总象素计数
//	uint32 nWidth = m_pFrameInfo[nFrame].nWidth;
//	uint32 nHeight = m_pFrameInfo[nFrame].nHeight;
//	uint32 nTotlePixelNum = nWidth * nHeight;						// 总象素数目
//	uint32 nFrameLen = m_pFrameInfo[nFrame].nRawDataLen;
//	BYTE *pData = m_pFrameInfo[nFrame].pRawData;
//	BYTE pixelNum, alpha, pixelColor;
//	BYTE *pTexData = (BYTE*)LockedRect.pBits;		// 贴图数据
//	BYTE *pTexLine = pTexData;						// 贴图每一行数据
//	for(;;)
//	{
//	pixelNum = *pData++;
//	alpha = *pData++;
//	nByteCount += 2;
//	nPixelCount += pixelNum;
//
//	if(alpha == 0)
//	{
//	for(int l=0; l<pixelNum; l++)
//	{
//	*((DWORD*)pTexLine) = 0;
//	pTexLine += 4;
//	}
//	}
//	else
//	{
//	for(int l=0; l<pixelNum; l++)
//	{
//	pixelColor = *pData++;
//	*pTexLine++ = m_pPal24[pixelColor].Blue;
//	*pTexLine++ = m_pPal24[pixelColor].Green;
//	*pTexLine++ = m_pPal24[pixelColor].Red;
//	*pTexLine++ = alpha;
//	}
//	nByteCount += pixelNum;
//	}
//
//	if(nPixelCount == nWidth)
//	{
//	// 如果一帧数据解完则停止
//	if(nByteCount >= nFrameLen)
//	break;
//	pTexLine = pTexData = pTexData + LockedRect.Pitch;
//	nPixelCount = 0;
//	}
//	}
//
//	m_pTextureInfo[nFrame].pTexture->UnlockRect(0);
//
//	SAFE_DELETE_ARRAY(m_pFrameInfo[nFrame].pRawData);
//	return m_pTextureInfo[nFrame].pTexture;*/
//}
//
///*压缩
//LPDIRECT3DTEXTURE8 TextureResSpr::CreateTexture16Bit(uint32 nFrame)
//{
//if(nFrame >= m_nFrameNum)
//return NULL;
//
//if(m_pTextureInfo[nFrame].pTexture)
//return m_pTextureInfo[nFrame].pTexture;
//
//if(!m_pFrameInfo[nFrame].pRawData)
//return NULL;
//
//LPDIRECT3DTEXTURE8 pTexture = NULL;
//
//// 创建贴图
//if (FAILED(D3DXCreateTexture(PD3DDEVICE, m_pTextureInfo[nFrame].nWidth, m_pTextureInfo[nFrame].nHeight, 1,
//0, D3DFMT_A4R4G4B4, D3DPOOL_SYSTEMMEM, &pTexture)))
//return NULL;
//
//D3DLOCKED_RECT LockedRect;
//if (FAILED(pTexture->LockRect(0, &LockedRect, NULL, 0)))
//{
//SAFE_RELEASE(pTexture);
//return NULL;
//}
//
//m_nTexMemUsed += m_pTextureInfo[nFrame].nWidth * m_pTextureInfo[nFrame].nHeight * 2;
//m_nMemUseful += m_pFrameInfo[nFrame].nWidth * m_pFrameInfo[nFrame].nHeight * 2;
//
//// 填充贴图数据
//uint32 nByteCount = 0;							// 总字节计数
//uint32 nPixelCount = 0;							// 总象素计数
//uint32 nWidth = m_pFrameInfo[nFrame].nWidth;
//uint32 nHeight = m_pFrameInfo[nFrame].nHeight;
//uint32 nFrameLen = m_pFrameInfo[nFrame].nRawDataLen;
//BYTE *pData = m_pFrameInfo[nFrame].pRawData;
//BYTE pixelNum, alpha, pixelColor;
//BYTE *pTexData = (BYTE*)LockedRect.pBits;		// 贴图数据
//BYTE *pTexLine = pTexData;						// 贴图每一行数据
////	uint32 nTotlePixelNum = m_pTextureInfo[nFrame].nWidth * m_pTextureInfo[nFrame].nHeight;	
//for(;;)
//{
//pixelNum = *pData++;
//alpha = *pData++;
//nByteCount += 2;
//nPixelCount += pixelNum;
//
//// 如果超出贴图数据范围则停止
////		if(nPixelCount > nTotlePixelNum)
////			break;
//
//if(alpha == 0)
//{
//for(int l=0; l<pixelNum; l++)
//{
//*((WORD*)pTexLine) = 0;
//pTexLine += 2;
//}
//}
//else
//{
//for(int l=0; l<pixelNum; l++)
//{
//pixelColor = *pData++;
////*((WORD*)pTexLine) = ARGBTO4444(alpha, m_pPal24[pixelColor].Red, m_pPal24[pixelColor].Green, m_pPal24[pixelColor].Blue);
//*((WORD*)pTexLine) = ((((WORD)alpha)<<8)&0xf000) | m_pPal16[pixelColor];
//pTexLine += 2;
//}
//nByteCount += pixelNum;
//}
//
//if(nPixelCount % nWidth == 0)
//{
//// 如果一帧数据解完则停止
//if(nByteCount >= nFrameLen)
//break;
//pTexLine = pTexData = pTexData + LockedRect.Pitch;
////	nPixelCount = 0;
//}
//}
//
//pTexture->UnlockRect(0);
//
//IDirect3DSurface8* pSrcSurface = NULL, *pDesSurface = NULL;
//
//// 创建贴图
//if (FAILED(D3DXCreateTexture(PD3DDEVICE, m_pTextureInfo[nFrame].nWidth, m_pTextureInfo[nFrame].nHeight, 1,
//0, D3DFMT_DXT5, D3DPOOL_MANAGED, &m_pTextureInfo[nFrame].pTexture)))
//goto error;
//
//if ( pTexture->GetSurfaceLevel( 0, &pSrcSurface) != D3D_OK )
//goto error;
//if ( m_pTextureInfo[nFrame].pTexture->GetSurfaceLevel( 0, &pDesSurface) != D3D_OK )
//goto error;
//
//D3DXLoadSurfaceFromSurface( pDesSurface, NULL, NULL, pSrcSurface, NULL, NULL, D3DX_FILTER_NONE, 0);
//
//SAFE_RELEASE(pSrcSurface);
//SAFE_RELEASE(pDesSurface);
//
//SAFE_DELETE_ARRAY(m_pFrameInfo[nFrame].pRawData);
//SAFE_RELEASE(pTexture);
//return m_pTextureInfo[nFrame].pTexture;
//
//error:
//SAFE_RELEASE(pSrcSurface);
//SAFE_RELEASE(pDesSurface);
//SAFE_RELEASE(pTexture);
//SAFE_RELEASE(m_pTextureInfo[nFrame].pTexture);
//return NULL;
//}
//
//LPDIRECT3DTEXTURE8 TextureResSpr::CreateTexture32Bit(uint32 nFrame)
//{
//if(nFrame >= m_nFrameNum)
//return NULL;
//
//if(m_pTextureInfo[nFrame].pTexture)
//return m_pTextureInfo[nFrame].pTexture;
//
//if(!m_pFrameInfo[nFrame].pRawData)
//return NULL;
//
//LPDIRECT3DTEXTURE8 pTexture = NULL;
//
//// 创建贴图
//if (FAILED(D3DXCreateTexture(PD3DDEVICE, m_pTextureInfo[nFrame].nWidth, m_pTextureInfo[nFrame].nHeight, 1,
//0, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &pTexture)))
//return NULL;
//
//D3DLOCKED_RECT LockedRect;
//if (FAILED(pTexture->LockRect(0, &LockedRect, NULL, 0)))
//{
//SAFE_RELEASE(pTexture);
//return NULL;
//}
//
//m_nTexMemUsed += m_pTextureInfo[nFrame].nWidth * m_pTextureInfo[nFrame].nHeight * 4;
//m_nMemUseful += m_pFrameInfo[nFrame].nWidth * m_pFrameInfo[nFrame].nHeight * 4;
//
//// 填充贴图数据
//uint32 nByteCount = 0;							// 总字节计数
//uint32 nPixelCount = 0;							// 总象素计数
//uint32 nWidth = m_pFrameInfo[nFrame].nWidth;
//uint32 nHeight = m_pFrameInfo[nFrame].nHeight;
//uint32 nTotlePixelNum = nWidth * nHeight;						// 总象素数目
//uint32 nFrameLen = m_pFrameInfo[nFrame].nRawDataLen;
//BYTE *pData = m_pFrameInfo[nFrame].pRawData;
//BYTE pixelNum, alpha, pixelColor;
//BYTE *pTexData = (BYTE*)LockedRect.pBits;		// 贴图数据
//BYTE *pTexLine = pTexData;						// 贴图每一行数据
//for(;;)
//{
//pixelNum = *pData++;
//alpha = *pData++;
//nByteCount += 2;
//nPixelCount += pixelNum;
//
//if(alpha == 0)
//{
//for(int l=0; l<pixelNum; l++)
//{
//*((DWORD*)pTexLine) = 0;
//pTexLine += 4;
//}
//}
//else
//{
//for(int l=0; l<pixelNum; l++)
//{
//pixelColor = *pData++;
//*pTexLine++ = m_pPal24[pixelColor].Blue;
//*pTexLine++ = m_pPal24[pixelColor].Green;
//*pTexLine++ = m_pPal24[pixelColor].Red;
//*pTexLine++ = alpha;
//}
//nByteCount += pixelNum;
//}
//
//if(nPixelCount == nWidth)
//{
//// 如果一帧数据解完则停止
//if(nByteCount >= nFrameLen)
//break;
//pTexLine = pTexData = pTexData + LockedRect.Pitch;
//nPixelCount = 0;
//}
//}
//
//pTexture->UnlockRect(0);
//
//IDirect3DSurface8* pSrcSurface = NULL, *pDesSurface = NULL;
//
//// 创建贴图
//if (FAILED(D3DXCreateTexture(PD3DDEVICE, m_pTextureInfo[nFrame].nWidth, m_pTextureInfo[nFrame].nHeight, 1,
//0, D3DFMT_DXT5, D3DPOOL_MANAGED, &m_pTextureInfo[nFrame].pTexture)))
//goto error;
//
//if ( pTexture->GetSurfaceLevel( 0, &pSrcSurface) != D3D_OK )
//goto error;
//if ( m_pTextureInfo[nFrame].pTexture->GetSurfaceLevel( 0, &pDesSurface) != D3D_OK )
//goto error;
//
//D3DXLoadSurfaceFromSurface( pDesSurface, NULL, NULL, pSrcSurface, NULL, NULL, D3DX_FILTER_NONE, 0);
//
//SAFE_RELEASE(pSrcSurface);
//SAFE_RELEASE(pDesSurface);
//
//SAFE_DELETE_ARRAY(m_pFrameInfo[nFrame].pRawData);
//SAFE_RELEASE(pTexture);
//return m_pTextureInfo[nFrame].pTexture;
//
//error:
//SAFE_RELEASE(pSrcSurface);
//SAFE_RELEASE(pDesSurface);
//SAFE_RELEASE(pTexture);
//SAFE_RELEASE(m_pTextureInfo[nFrame].pTexture);
//return NULL;
//}
//*/
//int32 KTextureResSpr::GetPixelAlpha(const char* szImage, int32 nFrame, int32 nX, int32 nY)
//{
//	int nRet = 0;
//	void *pSprite;
//	SPRFRAME *pFrame = NULL;
//
//	nX -= m_pFrameInfo[nFrame].nOffX;
//	nY -= m_pFrameInfo[nFrame].nOffY;
//	if (nX >= 0  && nX < m_pFrameInfo[nFrame].nWidth && nY >= 0 && nY < m_pFrameInfo[nFrame].nHeight)
//	{
//		int	nNumPixels = m_pFrameInfo[nFrame].nWidth;
//
//		if(!m_bPacked)  //如果是整体压缩
//		{
//			if(m_pFrameInfo[nFrame].pRawData)
//				pSprite =  m_pFrameInfo[nFrame].pRawData;
//			else
//				return nRet;
//		}
//		else
//		{
//			unsigned int uFrameSize;
//			pFrame = (SPRFRAME *)SprGetFrame(szImage, (SPRHEAD*)m_pHeader, nFrame, uFrameSize);
//			if(!pFrame)
//				return nRet;
//			pSprite = pFrame->Sprite;
//		}
//
//		nY++;
//		_asm
//		{
//			//使SDI指向sprite中的图形数据位置
//			mov		esi, pSprite
//dec_line:
//			dec		nY				//减掉一行
//				jz		last_line
//
//				mov		edx, nNumPixels
//skip_line:
//			movzx	eax, byte ptr[esi]
//			inc		esi
//				movzx	ebx, byte ptr[esi]
//				inc		esi
//					or		ebx, ebx
//					jz		skip_line_continue
//					add		esi, eax
//skip_line_continue:
//				sub		edx, eax
//					jg		skip_line
//					jmp		dec_line
//
//last_line:
//				mov		edx, nX
//last_line_alpha_block:
//				movzx	eax, byte ptr[esi]
//				inc		esi
//					movzx	ebx, byte ptr[esi]
//					inc		esi
//						or		ebx, ebx
//						jz		last_line_continue
//						add		esi, eax
//last_line_continue:
//					sub		edx, eax
//						jg		last_line_alpha_block
//
//						mov		nRet, ebx
//		}
//	}
//
//	if( pFrame )
//		SprReleaseFrame((SPRFRAME*)m_pFrameInfo[nFrame].pFrame);
//	return nRet;
//}
//
//HRESULT KTextureResSpr::InvalidateDeviceObjects()
//{
//	// 释放所有贴图和spr数据
//	if(m_pFrameInfo)
//	{
//		for(int i=0; i<m_nFrameNum; i++)
//		{
//			// 释放第i帧的贴图
//			if(m_pFrameInfo[i].nTexNum)
//			{
//				SAFE_RELEASE(m_pFrameInfo[i].texInfo[0].pTexture);
//				SAFE_RELEASE(m_pFrameInfo[i].texInfo[1].pTexture);
//				SAFE_RELEASE(m_pFrameInfo[i].texInfo[2].pTexture);
//				SAFE_RELEASE(m_pFrameInfo[i].texInfo[3].pTexture);
//			}
//		}
//	}
//	m_bCacheable = S_FALSE;
//	return S_OK;
//}
//
//HRESULT KTextureResSpr::RestoreDeviceObjects()
//{
//	if(m_pFrameInfo)
//	{
//		for(int i = 0 ; i < m_nFrameNum ; i++)
//		{
//			CreateTexture16Bit(i);
//		}
//	}
//	return S_OK;
//}
//
//// 释放内存
//void KTextureResSpr::Clear()
//{
//	//SAFE_DELETE_ARRAY(m_pPal24);
//	//SAFE_DELETE_ARRAY(m_pPal16);
//
//	// 释放所有贴图和spr数据
//	if(m_pFrameInfo)
//	{
//		for(int i = 0; i < m_nFrameNum; i++)
//		{
//			if(m_bPacked)
//			{
//				if(m_pFrameInfo[i].pFrame)
//				{
//					SprReleaseFrame((SPRFRAME*)m_pFrameInfo[i].pFrame);
//					m_pFrameInfo[i].pFrame = NULL;
//					//m_pFrameInfo[i].pRawData = NULL;
//				}
//			}
//
//			// 释放第i帧的贴图
//			if(m_pFrameInfo[i].nTexNum)
//			{
//				SAFE_RELEASE(m_pFrameInfo[i].texInfo[0].pTexture);
//				SAFE_RELEASE(m_pFrameInfo[i].texInfo[1].pTexture);
//				SAFE_RELEASE(m_pFrameInfo[i].texInfo[2].pTexture);
//				SAFE_RELEASE(m_pFrameInfo[i].texInfo[3].pTexture);
//			}
//		}
//	}
//
//	SAFE_DELETE_ARRAY(m_pFrameInfo);
//
//	if(m_pHeader)
//	{
//		SprReleaseHeader((SPRHEAD*)m_pHeader);
//	}
//	ResetVar();
//}
//
//bool KTextureResSpr::ReleaseAFrameData()
//{
//	Clear();
//
//	return true;
//
//	if(m_pFrameInfo)
//	{
//		for(int i=0; i<m_nFrameNum; i++)
//		{
//			// 释放第i帧的贴图
//			if(m_pFrameInfo[i].texInfo[0].pTexture)
//			{
//				for(int j=0; j<4; j++)
//				{
//					m_nTexMemUsed -= m_pFrameInfo[i].texInfo[j].nWidth * m_pFrameInfo[i].texInfo[j].nHeight * 2;
//					SAFE_RELEASE(m_pFrameInfo[i].texInfo[j].pTexture);
//				}
//				return true;
//			}
//		}
//		return false;
//	}
//	return false;
//}
//
//int KTextureResSpr::SplitTexture(uint32 nFrame)
//{
//	int nWidth = m_pFrameInfo[nFrame].nWidth;
//	int nHeight = m_pFrameInfo[nFrame].nHeight;
//	// 计算原始贴图尺寸
//	int nTexWidth = FitTextureSize(nWidth);
//	int nTexHeight = FitTextureSize(nHeight);
//	if(nTexWidth == 0 || nTexHeight == 0)
//		return 0;
//
//	int nHalfWidth = nTexWidth / 2;
//	int nHalfHeight = nTexHeight / 2;
//	int nFourThreeWidth = nHalfWidth + nHalfWidth / 2;
//	int nFourThreeHeight = nHalfHeight + nHalfHeight / 2;
//	int nMem = 0;
//
//	int ntw = FitTextureSize(nWidth - nHalfWidth);
//	int nth = FitTextureSize(nHeight - nHalfHeight);
//
//	// 按不同情况将贴图分为4，2，1份，填写相关参数
//	if(nWidth > nHalfWidth && nWidth <= nFourThreeWidth && nHeight > nHalfHeight && nHeight <= nFourThreeHeight)
//	{
//		// 拆为4份
//		m_pFrameInfo[nFrame].nTexNum = 4;
//		m_pFrameInfo[nFrame].texInfo[0].nWidth = nHalfWidth;
//		m_pFrameInfo[nFrame].texInfo[0].nHeight = nHalfHeight;
//		m_pFrameInfo[nFrame].texInfo[0].nFrameX = 0;
//		m_pFrameInfo[nFrame].texInfo[0].nFrameY = 0;
//		m_pFrameInfo[nFrame].texInfo[0].nFrameWidth = nHalfWidth;
//		m_pFrameInfo[nFrame].texInfo[0].nFrameHeight = nHalfHeight;
//
//		m_pFrameInfo[nFrame].texInfo[1].nWidth = ntw;
//		m_pFrameInfo[nFrame].texInfo[1].nHeight = nHalfHeight;
//		m_pFrameInfo[nFrame].texInfo[1].nFrameX = nHalfWidth;
//		m_pFrameInfo[nFrame].texInfo[1].nFrameY = 0;
//		m_pFrameInfo[nFrame].texInfo[1].nFrameWidth = nWidth - nHalfWidth;
//		m_pFrameInfo[nFrame].texInfo[1].nFrameHeight = nHalfHeight;
//
//		m_pFrameInfo[nFrame].texInfo[2].nWidth = nHalfWidth;
//		m_pFrameInfo[nFrame].texInfo[2].nHeight = nth;
//		m_pFrameInfo[nFrame].texInfo[2].nFrameX = 0;
//		m_pFrameInfo[nFrame].texInfo[2].nFrameY = nHalfHeight;
//		m_pFrameInfo[nFrame].texInfo[2].nFrameWidth = nHalfWidth;
//		m_pFrameInfo[nFrame].texInfo[2].nFrameHeight = nHeight - nHalfHeight;
//
//		m_pFrameInfo[nFrame].texInfo[3].nWidth = ntw;
//		m_pFrameInfo[nFrame].texInfo[3].nHeight = nth;
//		m_pFrameInfo[nFrame].texInfo[3].nFrameX = nHalfWidth;
//		m_pFrameInfo[nFrame].texInfo[3].nFrameY = nHalfHeight;
//		m_pFrameInfo[nFrame].texInfo[3].nFrameWidth = nWidth - nHalfWidth;
//		m_pFrameInfo[nFrame].texInfo[3].nFrameHeight = nHeight - nHalfHeight;
//
//		nMem = ((nHalfWidth + ntw) * nHalfHeight + (nHalfWidth + ntw) * nth) * 2;
//	}
//	else if(nWidth > nHalfWidth && nWidth <= nFourThreeWidth)
//	{
//		// 拆为左右两份
//		m_pFrameInfo[nFrame].nTexNum = 2;
//		m_pFrameInfo[nFrame].texInfo[0].nWidth = nHalfWidth;
//		if(nHeight <= nHalfHeight)
//			m_pFrameInfo[nFrame].texInfo[0].nHeight = nHalfHeight;
//		else
//			m_pFrameInfo[nFrame].texInfo[0].nHeight = nTexHeight;
//		m_pFrameInfo[nFrame].texInfo[0].nFrameX = 0;
//		m_pFrameInfo[nFrame].texInfo[0].nFrameY = 0;
//		m_pFrameInfo[nFrame].texInfo[0].nFrameWidth = nHalfWidth;
//		m_pFrameInfo[nFrame].texInfo[0].nFrameHeight = nHeight;
//
//		m_pFrameInfo[nFrame].texInfo[1].nWidth = ntw;
//		if(nHeight <= nHalfHeight)
//			m_pFrameInfo[nFrame].texInfo[1].nHeight = nHalfHeight;
//		else
//			m_pFrameInfo[nFrame].texInfo[1].nHeight = nTexHeight;
//		m_pFrameInfo[nFrame].texInfo[1].nFrameX = nHalfWidth;
//		m_pFrameInfo[nFrame].texInfo[1].nFrameY = 0;
//		m_pFrameInfo[nFrame].texInfo[1].nFrameWidth = nWidth - nHalfWidth;
//		m_pFrameInfo[nFrame].texInfo[1].nFrameHeight = nHeight;
//
//		nMem = (nHalfWidth + ntw) * nHalfHeight * 2;
//	}
//	else if(nHeight > nHalfHeight && nHeight <= nFourThreeHeight)
//	{
//		// 拆为上下两份
//		m_pFrameInfo[nFrame].nTexNum = 2;
//		if(nWidth <= nHalfWidth)
//			m_pFrameInfo[nFrame].texInfo[0].nWidth = nHalfWidth;
//		else
//			m_pFrameInfo[nFrame].texInfo[0].nWidth = nTexWidth;
//		m_pFrameInfo[nFrame].texInfo[0].nHeight = nHalfHeight;
//		m_pFrameInfo[nFrame].texInfo[0].nFrameX = 0;
//		m_pFrameInfo[nFrame].texInfo[0].nFrameY = 0;
//		m_pFrameInfo[nFrame].texInfo[0].nFrameWidth = nWidth;
//		m_pFrameInfo[nFrame].texInfo[0].nFrameHeight = nHalfHeight;
//
//		if(nWidth <= nHalfWidth)
//			m_pFrameInfo[nFrame].texInfo[1].nWidth = nHalfWidth;
//		else
//			m_pFrameInfo[nFrame].texInfo[1].nWidth = nTexWidth;
//		m_pFrameInfo[nFrame].texInfo[1].nHeight = nth;
//		m_pFrameInfo[nFrame].texInfo[1].nFrameX = 0;
//		m_pFrameInfo[nFrame].texInfo[1].nFrameY = nHalfHeight;
//		m_pFrameInfo[nFrame].texInfo[1].nFrameWidth = nWidth;
//		m_pFrameInfo[nFrame].texInfo[1].nFrameHeight = nHeight - nHalfHeight;
//
//		nMem = nHalfWidth * (nHalfHeight + nth) * 2;
//	}
//	else
//	{
//		// 不拆，一份
//		m_pFrameInfo[nFrame].nTexNum = 1;
//		if(nWidth <= nHalfWidth)
//			m_pFrameInfo[nFrame].texInfo[0].nWidth = nHalfWidth;
//		else
//			m_pFrameInfo[nFrame].texInfo[0].nWidth = nTexWidth;
//		if(nHeight <= nHalfHeight)
//			m_pFrameInfo[nFrame].texInfo[0].nHeight = nHalfHeight;
//		else
//			m_pFrameInfo[nFrame].texInfo[0].nHeight = nTexHeight;
//		m_pFrameInfo[nFrame].texInfo[0].nFrameX = 0;
//		m_pFrameInfo[nFrame].texInfo[0].nFrameY = 0;
//		m_pFrameInfo[nFrame].texInfo[0].nFrameWidth = nWidth;
//		m_pFrameInfo[nFrame].texInfo[0].nFrameHeight = nHeight;
//
//		nMem = m_pFrameInfo[nFrame].texInfo[0].nWidth * m_pFrameInfo[nFrame].texInfo[0].nHeight * 2;
//	}
//
//	return nMem;
//}
