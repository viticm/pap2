////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DFontRenderer.cpp
//  Version     : 1.0
//  Creator     : dengchao
//  Create Date : 2006-02-13 16:43:32
//  Comment     : font renderer
//
//				  注意下面会有两到三处地方代码是重复的只是调用与参数的不同。
//				  因为现在字体的绘制并没有完全需求化，所以每种类型的都存在有
//				  特例函数，依备扩展。请不要将此作为错误提交，谢谢。
////////////////////////////////////////////////////////////////////////////////


#include  <algorithm>

#include  <ft2build.h>
#include  FT_FREETYPE_H
//#include  FT_INTERNAL_OBJECTS_H 


#include  "KG3DFontTexture.h"
#include  "KG3DFontRenderer.h"


namespace
{
	inline int kg3dNextPowerOf2( int in )
	{
		in -= 1;

		in |= in >> 16;
		in |= in >> 8;
		in |= in >> 4;
		in |= in >> 2;
		in |= in >> 1;

		return in + 1;
	}

	VOID WINAPI kg3dColorFill(D3DXVECTOR4* pOut, const D3DXVECTOR2* pTexCoord, 
		const D3DXVECTOR2* pTexelSize, LPVOID pData)
	{
		*pOut = D3DXVECTOR4(0, 0, 0.0f, 0.0f);
	}


}



////////////////////////////////////////////////
// constructor / destructor
KG3DFontRenderer::KG3DFontRenderer( )
	:	m_pD3DDevice( NULL ),
		m_TextureLast( 0 ),
		m_pEffectCur( NULL ),
		m_pEffectShader( NULL )
{

}


KG3DFontRenderer::~KG3DFontRenderer()
{
	CleanUp();
}



////////////////////////////////////////////////
// initialize / cleanup
bool  KG3DFontRenderer::Initialize( LPDIRECT3DDEVICE9 pD3DDevice )
{
	// check valid
	m_pD3DDevice = pD3DDevice;
	if ( NULL == pD3DDevice )
	{
		return false;
	}


	// check d3d format
	checkD3DTextureCaps( pD3DDevice );
	checkD3DTextureFormat( pD3DDevice );


	// effect load
	effectLoad();


	// init member
	m_nNumIndexs	= 0;
	m_nNumTLIndexs  = 0;
	m_nNumVertexs	= 0;
	m_nNumTLVertexs	= 0;

	return true;
}

bool  KG3DFontRenderer::CleanUp()
{
	// destroy texture
	DestroyTexture();


	// unload effect
	effectUnload();


	return true;
}


////////////////////////////////////////////////
// prepare / flush
bool  KG3DFontRenderer::Prepare( )
{
	m_TextureLast	= 0;
	m_pEffectCur	= NULL;

	m_nNumIndexs	= 0;
	m_nNumTLIndexs  = 0;
	m_nNumVertexs	= 0;
	m_nNumTLVertexs	= 0;
	return true;
}

void  KG3DFontRenderer::Flush( )
{
	// flush
	this->flushVertex( );
	this->flushTLVertex( );


	m_TextureLast	= 0;
}



////////////////////////////////////////////////
// texture
void  KG3DFontRenderer::DestroyTexture()
{
	for ( UINT i = 0; i < m_TextureVector.size(); ++ i )
	{
		this->DeleteTexture( i + 1 );
	}


	m_TextureLast = 0;
	m_TextureVector.clear();
}

void  KG3DFontRenderer::DeleteTexture( UINT Index )
{
	if ( ( 0 < Index ) && ( Index <= m_TextureVector.size() ) )
	{
		LPDIRECT3DTEXTURE9 lpTexture = m_TextureVector.at( Index - 1 );

		if ( NULL != lpTexture )
		{
		#ifdef _DEBUG
			std::string temp("test0");

			temp += 'A' + ( Index - 1 );
			temp += ".TGA";

			D3DXSaveTextureToFile( temp.c_str(), D3DXIFF_TGA, lpTexture, NULL );
		#endif
			lpTexture->Release();
			m_TextureVector[ Index - 1 ] = NULL;
		}

	}

}


bool  KG3DFontRenderer::BuildTexture( UINT& Index, UINT& Width, UINT& Height, UINT glyphW, UINT glyphH )
{
	// get texture width / height
	INT  Padding =  KG3DFONT_DEFAULT_PADDING * 2;

	Width = kg3dNextPowerOf2( Width  + Padding );
	Width = Width > m_TextureWidth ? m_TextureWidth : Width;

	Padding = ( Width - Padding ) / glyphW;
	Height  = kg3dNextPowerOf2( ( ( Height / Padding ) + 1 ) * glyphH );
	Height  = Height > m_TextureHeight ? m_TextureHeight : Height;



	// make texture
	LPDIRECT3DTEXTURE9 lpTexture = NULL;
	HRESULT hr = D3D_OK;


	hr = D3DXCreateTexture( m_pD3DDevice, Width,Height, 1, D3DUSAGE_DYNAMIC, m_TextureFmt, D3DPOOL_DEFAULT, &lpTexture );
	if ( D3D_OK == hr )
	{
		// clear
		D3DXFillTexture (lpTexture, kg3dColorFill, NULL);

		// push back
		static const LPDIRECT3DTEXTURE9 NULL_TEXTURE = NULL;
		typedef std::vector< LPDIRECT3DTEXTURE9 > TextureVectorT;
		typedef TextureVectorT::iterator TextureIterT;

		TextureIterT iter = std::find( m_TextureVector.begin(), m_TextureVector.end(), NULL_TEXTURE );
		if ( iter != m_TextureVector.end() )
		{
			*iter = lpTexture;
			Index = ( iter - m_TextureVector.begin() ) + 1;
		}
		else
		{
			m_TextureVector.push_back( lpTexture );
			Index = static_cast<UINT>( m_TextureVector.size() );
		}


		return true;
	}


	Index = 0;
	return false;
}

bool  KG3DFontRenderer::UploadTexture( UINT Index, UINT offX, UINT offY, UINT sizeW, UINT sizeH, BYTE const* pUpload )
{
	LPDIRECT3DTEXTURE9 lpTexture = m_TextureVector.at( Index - 1 );
	HRESULT hr = D3D_OK;

	if ( NULL != lpTexture )
	{
		D3DLOCKED_RECT rectLock;
		RECT	rectUpload = { offX, offY, offX + sizeW, offY + sizeH };

		hr = lpTexture->LockRect( 0, &rectLock, &rectUpload, D3DLOCK_NOSYSLOCK );
		if ( D3D_OK == hr )
		{
			UCHAR* pPixBit = static_cast< UCHAR* >( rectLock.pBits );

			for ( register UINT y = 0; y < sizeH; ++ y )
			{
				UCHAR* pPixWrite = reinterpret_cast< UCHAR* >( pPixBit + y * rectLock.Pitch );
				for ( register UINT x = 0; x < sizeW; ++ x )
				{
					for ( register UINT i = 0; i < m_TextureFmtSkip; ++ i )
					{
						* pPixWrite ++ = 0;
					}

					*pPixWrite ++ = *pUpload ++;
				}

			}


			lpTexture->UnlockRect(0);
		}
	
	}
	

	return true;
}

bool  KG3DFontRenderer::UploadMonoTexture( UINT Index, UINT offX, UINT offY, UINT sizeW, UINT sizeH, UINT pitchSrc, BYTE const* pUpload )
{
	LPDIRECT3DTEXTURE9 lpTexture = m_TextureVector.at( Index - 1 );
	HRESULT hr = D3D_OK;

	if ( NULL != lpTexture )
	{
		D3DLOCKED_RECT rectLock;
		RECT	rectUpload = { offX, offY, offX + sizeW, offY + sizeH };

		hr = lpTexture->LockRect( 0, &rectLock, &rectUpload, D3DLOCK_NOSYSLOCK );
		if ( D3D_OK == hr )
		{
			UCHAR* pPixBit = static_cast< UCHAR* >( rectLock.pBits );
			INT    nPixOffset = 0, nPixRShift = 0;

			for ( register UINT y = 0; y < sizeH; ++ y )
			{
				BYTE* pPixWrite = reinterpret_cast< BYTE* >( pPixBit + y * rectLock.Pitch );
				BYTE const* pPixReader = reinterpret_cast< BYTE const* >( pUpload + y * pitchSrc );

				for ( register UINT x = 0; x < sizeW; x += nPixOffset, ++ pPixReader )
				{
					//nPixOffset = MAX( sizeW - x, 8 );//MIN( sizeW - x, 8 );
					nPixOffset = min( sizeW - x, 8 );
					nPixRShift = 7;

					for ( register int k = nPixOffset - 1; k >= 0; -- k, -- nPixRShift )
					{
			//#ifdef _DEBUG
			//			*pPixWrite ++ = ( ( ( *pPixReader ) >> k ) & 1 ) * 255;
			//			for ( register UINT i = 0; i < m_TextureFmtSkip; ++ i )
			//			{
			//				* pPixWrite ++ = 0xFF;
			//			}
			//#else
						for ( register UINT i = 0; i < m_TextureFmtSkip; ++ i )
						{
							* pPixWrite ++ = 0;
						}
						*pPixWrite ++ = ( ( ( *pPixReader ) >> nPixRShift ) & 1 ) * 255;


			//#endif
					}

				}

			}


			lpTexture->UnlockRect(0);
		}

	}


	return true;
}


////////////////////////////////////////////////
// vertex buffer
void  KG3DFontRenderer::PrepareVeretex( UINT TexIndex, KG3DFontVertex_t* pVertex, USHORT nums )
{
	if ( ( 0 < TexIndex ) && ( NULL != pVertex ) && ( 0 < nums ) )
	{
		// flush
		//if ( TexIndex != m_TextureLast )
		//{
		//	this->flushVertex( );
		//	this->flushTLVertex( );
		//	this->flushTexture( TexIndex );
		//}

		this->flushTexture( TexIndex );
		if ( ( m_nNumVertexs + nums ) >= KG3DFONT_NUMS_VERTEX )
		{
			this->flushVertex( );
		}


		// write vertex buffer
		USHORT const primNums = nums >> 2;

		for ( register UINT i = 0; i < primNums; ++ i )
		{
			m_pIndexBuffer[m_nNumIndexs++] = m_nNumVertexs + 0;
			m_pIndexBuffer[m_nNumIndexs++] = m_nNumVertexs + 1;
			m_pIndexBuffer[m_nNumIndexs++] = m_nNumVertexs + 2;
			m_pIndexBuffer[m_nNumIndexs++] = m_nNumVertexs + 3;
			m_pIndexBuffer[m_nNumIndexs++] = m_nNumVertexs + 0;
			m_pIndexBuffer[m_nNumIndexs++] = m_nNumVertexs + 2;

			memcpy( m_pVertexBuffer + m_nNumVertexs, pVertex, sizeof(KG3DFontVertex_t) * nums);
			m_nNumVertexs += nums;
		}

	}

}

void  KG3DFontRenderer::PrepareVeretex( UINT TexIndex, KG3DFontTLVertex_t* pVertex, USHORT nums )
{
	if ( ( 0 < TexIndex ) && ( NULL != pVertex ) && ( 0 < nums ) )
	{
		// flush
		//if ( TexIndex != m_TextureLast )
		//{
		//	this->flushVertex( );
		//	this->flushTLVertex( );
		//	this->flushTexture( TexIndex );
		//}
		
		this->flushTexture( TexIndex );
		if ( ( m_nNumTLVertexs + nums ) >= KG3DFONT_NUMS_TLVERTEX )
		{
			this->flushTLVertex( );
		}


		// write vertex buffer
		USHORT const primNums = nums >> 2;
		USHORT* pIndexBuffer = m_pTLIndexBuffer + m_nNumTLIndexs;

		memcpy( m_pTLVertexBuffer + m_nNumTLVertexs, pVertex, sizeof(KG3DFontTLVertex_t) * nums);
		for ( register UINT i = 0; i < primNums; ++ i )
		{
			*pIndexBuffer ++ = m_nNumTLVertexs + 0;
			*pIndexBuffer ++ = m_nNumTLVertexs + 1;
			*pIndexBuffer ++ = m_nNumTLVertexs + 2;

			*pIndexBuffer ++ = m_nNumTLVertexs + 3;
			*pIndexBuffer ++ = m_nNumTLVertexs + 0;
			*pIndexBuffer ++ = m_nNumTLVertexs + 2;

		}

		
		m_nNumTLIndexs  += primNums * 6;
		m_nNumTLVertexs += nums;

	}

}


////////////////////////////////////////////////
// effect
void  KG3DFontRenderer::ApplyNoneEffect( )
{
	m_pEffectCur = NULL;
}

void  KG3DFontRenderer::ApplyBorderEffect( UINT color, UINT weight )
{
	//m_pEffectCur = NULL;
	//KG3DFONT_MSG_INFO( "NOT IMPLEMENT" );
	/*m_pEffectCur = m_pEffectShader;
	m_pEffectCur->ValidateTechnique( "TECH_BORDER" );
	m_pEffectCur->SetTechnique( "TECH_BORDER" );
	
	m_pEffectCur->SetFloat( "COLOR_BORDER", color );
	m_pEffectCur->SetFloat( "WEIGHT_BORDER", weight );*/
}


////////////////////////////////////////////////
// check d3d format
void  KG3DFontRenderer::checkD3DTextureCaps( LPDIRECT3DDEVICE9 pD3DDevice )
{
	// get d3d caps
	D3DCAPS9 d3dCaps;

	m_pD3DDevice->GetDeviceCaps( &d3dCaps );
	m_TextureWidth = d3dCaps.MaxTextureWidth;
	m_TextureHeight = d3dCaps.MaxTextureHeight;

	if ( m_TextureWidth > 256 )   m_TextureWidth = 256;
	if ( m_TextureHeight > 256 )  m_TextureHeight = 256;

	if ( d3dCaps.MaxTextureAspectRatio < ( m_TextureWidth / m_TextureHeight ) )
	{
		m_TextureHeight = kg3dNextPowerOf2( m_TextureWidth / d3dCaps.MaxTextureAspectRatio );
	}

}

void  KG3DFontRenderer::checkD3DTextureFormat( LPDIRECT3DDEVICE9 pD3DDevice )
{
	HRESULT hr = D3D_OK;


	// get d3d9 & display mode
	D3DDISPLAYMODE AdapterFormat;
	LPDIRECT3D9 pD3D9 = NULL;

	hr = pD3DDevice->GetDirect3D( &pD3D9 );
	hr = pD3DDevice->GetDisplayMode( 0, &AdapterFormat );


	// each texture format
	D3DFORMAT  d3dFmt[] = { D3DFMT_A8, D3DFMT_A8L8, D3DFMT_A4R4G4B4, D3DFMT_A8R8G8B8 };
	UINT       d3dFmtSkip[] = { 0, 1, 1, 3 };


	m_TextureFmt = D3DFMT_A8R8G8B8;
	m_TextureFmtSkip = 3;

#ifndef _DEBUG
	for ( int i = 0; i < _countof( d3dFmt ); ++ i )
	{
		hr = pD3D9->CheckDeviceFormat(  D3DADAPTER_DEFAULT, 
			D3DDEVTYPE_HAL, 
			AdapterFormat.Format,
			0, D3DRTYPE_TEXTURE, d3dFmt[i] );
		if ( D3D_OK == hr )
		{
			m_TextureFmt = d3dFmt[i];
			m_TextureFmtSkip = d3dFmtSkip[i];

			break ;
		}

	}
#endif
}



////////////////////////////////////////////////
// effect

void  KG3DFontRenderer::effectLoad()
{
	LPD3DXBUFFER  ppCompilationErrors;
	HRESULT hr = E_FAIL;
	TCHAR   name[ MAX_PATH ];


	//_sntprintf( name, sizeof( name ), "%sData\\public\\shader\\font_shader.fx", g_Def_AppDirectory );
	//if ( FAILED( hr = D3DXCreateEffectFromFile( m_pD3DDevice, name, NULL, NULL, 0/* D3DXSHADER_DEBUG  | D3DXSHADER_SKIPOPTIMIZATION
	//																			 | D3DXSHADER_FORCE_VS_SOFTWARE_NOOPT  | D3DXSHADER_FORCE_PS_SOFTWARE_NOOPT*/ ,
	//																			 NULL, &m_pEffectShader, &ppCompilationErrors ) ) )
	//{
	//	// If there was an error lets get the errors so we can display them in an message box.
	//	if (ppCompilationErrors)
	//	{
	//		KG3DFONT_MSG_INFO( ppCompilationErrors->GetBufferPointer() );
	//	}

	//}

}

void  KG3DFontRenderer::effectUnload()
{
	//SAFE_RELEASE( m_pEffectShader );

}

UINT  KG3DFontRenderer::effectBegin( )
{
	if ( NULL != m_pEffectCur )
	{
		UINT passs = 0;

		if ( D3D_OK == m_pEffectCur->Begin( &passs, 0 ) )
		{
			return passs;
		}

	}
	

	return 0;
}

void  KG3DFontRenderer::effectEnd( )
{
	if ( NULL != m_pEffectCur )
	{
		m_pEffectCur->End();
	}

}

bool  KG3DFontRenderer::effectBeginPass( UINT pass )
{
	if ( ( NULL != m_pEffectCur )
		&& ( D3D_OK == m_pEffectCur->BeginPass( pass ) ) )
	{
		return true;
	}


	return false;
}

void  KG3DFontRenderer::effectEndPass( )
{
	if ( NULL != m_pEffectCur )
	{
		m_pEffectCur->EndPass();
	}

}


////////////////////////////////////////////////
// flush
void  KG3DFontRenderer::flushTexture( UINT TexIndex )
{
	// flush
	if ( TexIndex != m_TextureLast )
	{
		if ( NULL != m_pEffectCur )
		{
			m_pEffectCur->SetTexture( "FONT_TEXTURE", m_TextureVector.at( TexIndex - 1 ) );
		}

		this->flushVertex( );
		this->flushTLVertex( );


		m_TextureLast = TexIndex;
		m_pD3DDevice->SetTexture( 0, m_TextureVector.at( TexIndex - 1 ) );

	}

}


void  KG3DFontRenderer::flushVertex()
{
	if ( 0 != m_nNumIndexs )
	{
		if ( NULL != m_pEffectCur )
		{
			flushEffectVertex();
		}
		else
		{
			m_pD3DDevice->SetFVF( KG3DFONT_FVF_VERTEX );
			m_pD3DDevice->DrawIndexedPrimitiveUP( 
				D3DPT_TRIANGLELIST,
				0,
				m_nNumVertexs,
				m_nNumVertexs >> 1,
				m_pIndexBuffer,
				D3DFMT_INDEX16,
				m_pVertexBuffer,
				sizeof( KG3DFontVertex_t )
				);
		
		}


		m_nNumIndexs  = 0;
		m_nNumVertexs = 0;
	}

}

void  KG3DFontRenderer::flushTLVertex()
{
	if ( 0 != m_nNumTLIndexs )
	{
		if ( NULL != m_pEffectCur )
		{
			flushEffectTLVertex();
		}
		else
		{
			m_pD3DDevice->SetFVF( KG3DFONT_FVF_TLVERTEX );
			m_pD3DDevice->DrawIndexedPrimitiveUP( 
				D3DPT_TRIANGLELIST,
				0,
				m_nNumTLVertexs,
				m_nNumTLVertexs >> 1,
				m_pTLIndexBuffer,
				D3DFMT_INDEX16,
				m_pTLVertexBuffer,
				sizeof( KG3DFontTLVertex_t )
				);
		
		}


		m_nNumTLIndexs  = 0;
		m_nNumTLVertexs = 0;
	}
}


void  KG3DFontRenderer::flushEffectVertex()
{
	UINT nPasss = this->effectBegin();
	KG3DFONT_ASSERT( 0 < nPasss );

	if ( 0 < nPasss )
	{
		for ( UINT i = 0; i < nPasss; ++ i )
		{
			if ( this->effectBeginPass( i ) )
			{
				m_pD3DDevice->SetFVF( KG3DFONT_FVF_VERTEX );
				m_pD3DDevice->DrawIndexedPrimitiveUP( 
					D3DPT_TRIANGLELIST,
					0,
					m_nNumVertexs,
					m_nNumVertexs >> 1,
					m_pIndexBuffer,
					D3DFMT_INDEX16,
					m_pVertexBuffer,
					sizeof( KG3DFontVertex_t )
					);


				this->effectEndPass();
			}

		}

	}


	this->effectEnd();

}



void  KG3DFontRenderer::flushEffectTLVertex()
{
	UINT nPasss = this->effectBegin();
	KG3DFONT_ASSERT( 0 < nPasss );

	if ( 0 < nPasss )
	{
		for ( UINT i = 0; i < nPasss; ++ i )
		{
			if ( this->effectBeginPass( i ) )
			{
				m_pD3DDevice->SetFVF( KG3DFONT_FVF_TLVERTEX );
				m_pD3DDevice->DrawIndexedPrimitiveUP( 
					D3DPT_TRIANGLELIST,
					0,
					m_nNumTLVertexs,
					m_nNumTLVertexs >> 1,
					m_pTLIndexBuffer,
					D3DFMT_INDEX16,
					m_pTLVertexBuffer,
					sizeof( KG3DFontTLVertex_t )
					);

			
				this->effectEndPass();
			}

		}

	}


	this->effectEnd();
}


