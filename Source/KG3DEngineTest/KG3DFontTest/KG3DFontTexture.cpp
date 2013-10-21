////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DFontTexture.cpp
//  Version     : 1.0
//  Creator     : dengchao
//  Create Date : 2006-02-13 16:43:32
//  Comment     : font texture
//
////////////////////////////////////////////////////////////////////////////////


#include  <ft2build.h>
#include  FT_FREETYPE_H
//#include  FT_INTERNAL_OBJECTS_H 

#include  "KG3DFontTexture.h"
#include  "KG3DFontRenderer.h"


namespace
{
	////////////////////////////////////////////////
	// wrapper vertex
	#define  KG3D_VERTEX( vertex, nx, ny, ncolor, nu, nv )	\
				vertex.x = nx;			\
				vertex.y = ny;			\
				vertex.z = 1.0f;		\
				vertex.color = ncolor;	\
				vertex.u = nu;			\
				vertex.v = nv;


	#define  KG3D_TLVERTEX( vertex, nx, ny, ncolor, nu, nv )	\
				vertex.x = nx;			\
				vertex.y = ny;			\
				vertex.z = 0.0f;		\
				vertex.w = 1.0f;		\
				vertex.color = ncolor;	\
				vertex.u = nu;			\
				vertex.v = nv;
			

}



////////////////////////////////////////////////
// constructor / destructor
KG3DFontTexture::KG3DFontTexture( KG3DFontRenderer* pFontRenderer )
		:	KG3DFontBase( pFontRenderer )
{
	m_fontFlagDraw		= FT_RENDER_MODE_NORMAL;
	m_fontFlagLoader	= FT_LOAD_NO_HINTING;

}


KG3DFontTexture::~KG3DFontTexture()
{

}


////////////////////////////////////////////////
// draw font member
void  KG3DFontTexture::drawText( FLOAT penX, FLOAT penY, WCHAR chText )
{
	KG3DGlyphItem_t* fontGlyph = (KG3DGlyphItem_t*)charToGlyphSlot( chText );
	if ( NULL != fontGlyph )
	{
		// prepare vertex buffer
		FLOAT const fontScalingW =  fontGlyph->uploadWidth * this->m_fontScalingX;
		FLOAT const fontScalingH =  fontGlyph->uploadHeight * this->m_fontScalingY;

		//penX += fontGlyph->xOffset * this->m_fontScalingX;
		//penY += ( m_fontAscender - fontGlyph->yOffset ) * this->m_fontScalingY;
		penX += fontGlyph->xOffset * this->m_fontScalingX;
		penY += fontGlyph->yOffset * this->m_fontScalingY;



		if ( ( m_fontVertical )
			&& ( ( ( 0x4E00 > chText ) || ( chText > 0x9FFF ) ) )
			//&& ( ( 0x0030 > chText ) || ( chText > 0x007A ) ) )
			)
		{
			KG3D_VERTEX( m_glyphVertex[0], penX, penY, m_fontColour, fontGlyph->textureUV[0][0], fontGlyph->textureUV[1][1] );
			KG3D_VERTEX( m_glyphVertex[1], penX + fontScalingH, penY, m_fontColour, fontGlyph->textureUV[0][0], fontGlyph->textureUV[0][1] );
			KG3D_VERTEX( m_glyphVertex[2], penX + fontScalingH, penY + fontScalingW, m_fontColour, fontGlyph->textureUV[1][0], fontGlyph->textureUV[0][1] );
			KG3D_VERTEX( m_glyphVertex[3], penX, penY + fontScalingW, m_fontColour, fontGlyph->textureUV[1][0], fontGlyph->textureUV[1][1] );
		}
		else
		{
			KG3D_VERTEX( m_glyphVertex[0], penX, penY, m_fontColour, fontGlyph->textureUV[0][0], fontGlyph->textureUV[0][1] );
			KG3D_VERTEX( m_glyphVertex[1], penX + fontScalingW, penY, m_fontColour, fontGlyph->textureUV[1][0], fontGlyph->textureUV[0][1] );
			KG3D_VERTEX( m_glyphVertex[2], penX + fontScalingW, penY + fontScalingH, m_fontColour, fontGlyph->textureUV[1][0], fontGlyph->textureUV[1][1] );
			KG3D_VERTEX( m_glyphVertex[3], penX, penY + fontScalingH, m_fontColour, fontGlyph->textureUV[0][0], fontGlyph->textureUV[1][1] );

		}


		m_fontRenderer->PrepareVeretex( fontGlyph->textureId, m_glyphVertex, _countof( m_glyphVertex ) );

	}

}

void  KG3DFontTexture::drawTextEx( FLOAT penX, FLOAT penY, WCHAR chText )
{
	//KG3DPROFILE_RECORDEX( "drawTextEx" )
	KG3DGlyphItem_t* fontGlyph = (KG3DGlyphItem_t*)charToGlyphSlot( chText );
	if ( NULL != fontGlyph )
	{
		// prepare tl vertex buffer
		FLOAT const fontScalingW =  fontGlyph->uploadWidth * this->m_fontScalingX;
		FLOAT const fontScalingH =  fontGlyph->uploadHeight * this->m_fontScalingY;

		//penX += fontGlyph->xOffset * this->m_fontScalingX;
		//penY += ( m_fontAscender - fontGlyph->yOffset ) * this->m_fontScalingY;
		penX += fontGlyph->xOffset * this->m_fontScalingX;
		penY += fontGlyph->yOffset * this->m_fontScalingY;

		//  ISO10646所有字面中，目前僅有第0、第1和第2字面真正收容
		//	編碼字元。ISO10646的BMP和Unicode的編碼字元，依其UCS-2編
		//	碼序介紹如下：

		//	(1)0000～007Fh：基本拉丁字母區。其中0000～001Fh為C0控
		//	制碼，0020h為空格（space），0021～007Eh為ASCII圖形
		//	字元，007Fh為控制碼DEL。事實上，這128個字元碼只要
		//	把前8個位元去掉就可變成習見的8位元形式的ASCII碼。

		//	(2)0080～00A0h：控制碼區。其中0080～009Fh為C1控制碼，
		//	00A0h為不中斷空格（no-break space）。

		//	(3)00A1～1FFFh：拼音文字區。收容除基本拉丁字母以外的各種
		//	拼音文字字元，包括歐洲各國語言、希臘文、斯拉夫語文、
		//	希伯來文、阿拉伯文、亞美尼亞文、印度各地方言、馬來文、
		//	泰文、寮文、柬普寨文、滿文、蒙文、藏文、印地安語文等。

		//	(4)2000～28FFh：符號區。收容各種符號，包括標點符號、上下
		//	標、錢幣符號、數字、箭頭、數學符號、工程符號、光學辨
		//	識符號、帶圈或帶括符的文數字、表格繪製符號、地理圖示、
		//	盲用點字、裝飾圖形等。

		//	(5)2E80～33FFh：中日韓符號區。收容康熙字典部首、中日韓輔
		//	助部首、注音符號、日本假名、韓文音符，中日韓的符號、
		//	標點、帶圈或帶括符文數字、月份，以及日本的假名組合、
		//	單位、年號、月份、日期、時間等。

		//	(6)3400～4DFFh：中日韓認同表意文字擴充A區，總計收容6,582
		//	個中日韓漢字。

		//	(7)4E00～9FFFh：中日韓認同表意文字區，總計收容20,902個中
		//	日韓漢字。

		//	(8)A000～A4FFh：彝族文字區，收容中國南方彝族文字和字根。

		//	(9)AC00～D7FFh：韓文拼音組合字區，收容以韓文音符拼成的文
		//	字。

		//	(10)D800～DFFFh：S區，專用於UTF-16，詳情後敘。

		//	(11)E000～F8FFh：專用字區，其內容WG2不予規定，保留供使用
		//	者自行添加ISO10646未收容的字元。

		//	(12)F900～FAFFh：中日韓相容表意文字區，總計收容302個中日
		//	韓漢字。何謂相容表意文字，留待後敘。

		//	(13)FB00～FFFDh：文字表現形式區，收容組合拉丁文字、希伯來
		//	文、阿拉伯文、中日韓直式標點、小符號、半形符號、全形
		//	符號等。



		if ( ( m_fontVertical )
			&& ( ( ( 0x4E00 > chText ) || ( chText > 0x9FFF ) ) )
				//&& ( ( 0x0030 > chText ) || ( chText > 0x007A ) ) )
				)
		{
			KG3D_TLVERTEX( m_glyphTLVertex[0], penX, penY, m_fontColour, fontGlyph->textureUV[0][0], fontGlyph->textureUV[1][1] );
			KG3D_TLVERTEX( m_glyphTLVertex[1], penX + fontScalingH, penY, m_fontColour, fontGlyph->textureUV[0][0], fontGlyph->textureUV[0][1] );
			KG3D_TLVERTEX( m_glyphTLVertex[2], penX + fontScalingH, penY + fontScalingW, m_fontColour, fontGlyph->textureUV[1][0], fontGlyph->textureUV[0][1] );
			KG3D_TLVERTEX( m_glyphTLVertex[3], penX, penY + fontScalingW, m_fontColour, fontGlyph->textureUV[1][0], fontGlyph->textureUV[1][1] );
		}
		else
		{
			KG3D_TLVERTEX( m_glyphTLVertex[0], penX, penY, m_fontColour, fontGlyph->textureUV[0][0], fontGlyph->textureUV[0][1] );
			KG3D_TLVERTEX( m_glyphTLVertex[1], penX + fontScalingW, penY, m_fontColour, fontGlyph->textureUV[1][0], fontGlyph->textureUV[0][1] );
			KG3D_TLVERTEX( m_glyphTLVertex[2], penX + fontScalingW, penY + fontScalingH, m_fontColour, fontGlyph->textureUV[1][0], fontGlyph->textureUV[1][1] );
			KG3D_TLVERTEX( m_glyphTLVertex[3], penX, penY + fontScalingH, m_fontColour, fontGlyph->textureUV[0][0], fontGlyph->textureUV[1][1] );

		}


		m_fontRenderer->PrepareVeretex( fontGlyph->textureId, m_glyphTLVertex, _countof( m_glyphTLVertex ) );
	}

}


////////////////////////////////////////////////
// glyph font member
bool  KG3DFontTexture::charGlyphMake( KG3DFontGlyph_t** fontGlyph )
{
	( *fontGlyph ) = new KG3DGlyphItem_t;
	return true;
}

bool  KG3DFontTexture::charGlyphInfill( UINT charCode, void* ftGlyph, KG3DFontGlyph_t* fontGlyph )
{
	// render glyph bitmap
	FT_GlyphSlot pGlyphSlot = (FT_GlyphSlot)ftGlyph;
	KG3DGlyphItem_t* pGlyphFont = ( KG3DGlyphItem_t *)fontGlyph;

	FT_Error     ftErr   = 0;
	KG3DFONT_ASSERT( NULL != pGlyphSlot );
	KG3DFONT_ASSERT( NULL != pGlyphFont );

	if ( NULL != ftGlyph )
	{   
		ftErr = FT_Render_Glyph( pGlyphSlot, FT_Render_Mode( m_fontFlagDraw ) );
		if ( ftErr || pGlyphSlot->format != FT_GLYPH_FORMAT_BITMAP )
		{
			return false;
		}

	}
	else
	{
		return false;
	}


	// request glyph texture
	pGlyphFont->textureId = glyphTextureRequest();
	if ( 0 != pGlyphFont->textureId )
	{
		// upload sub texture
		FT_Bitmap glyphBitmap = pGlyphSlot->bitmap;


		//pGlyphFont->xOffset   = pGlyphSlot->bitmap_left;
		//pGlyphFont->yOffset   = pGlyphSlot->bitmap_top;
		pGlyphFont->xOffset   = static_cast< FLOAT >( pGlyphSlot->bitmap_left );
		pGlyphFont->yOffset   = static_cast< FLOAT >( m_fontAscender - pGlyphSlot->bitmap_top );

		if ( m_fontVertical )
		{
			std::swap( pGlyphFont->xOffset, pGlyphFont->yOffset );
		}


		pGlyphFont->uploadTop  = static_cast< FLOAT >( m_glyphOffY );
		pGlyphFont->uploadLeft = static_cast< FLOAT >( m_glyphOffX );

		pGlyphFont->uploadWidth = static_cast< FLOAT >( glyphBitmap.width );
		pGlyphFont->uploadHeight = static_cast< FLOAT >( glyphBitmap.rows );

		pGlyphFont->textureUV[0][0] = static_cast<float>( m_glyphOffX ) / static_cast<float>( m_glyphTextureWidth );
		pGlyphFont->textureUV[0][1] = static_cast<float>( m_glyphOffY ) / static_cast<float>( m_glyphTextureHeight );

		pGlyphFont->textureUV[1][0] = static_cast<float>( m_glyphOffX + glyphBitmap.width ) / static_cast<float>( m_glyphTextureWidth );
		pGlyphFont->textureUV[1][1] = static_cast<float>( m_glyphOffY + glyphBitmap.rows  ) / static_cast<float>( m_glyphTextureHeight );


		//if ( FT_GLYPH_FORMAT_BITMAP == pGlyphSlot->format )
		//{
		//	pGlyphFont->glyphFormat = KG3DFONT_BITMAP;
		//}
		//if ( FT_GLYPH_FORMAT_OUTLINE == pGlyphSlot->format )
		//{
		//	pGlyphFont->glyphFormat = KG3DFONT_ANTIALIAS;
		//}



		// empty glyph?
		// 如果轮廓线没有被上载，FreeType 使用的位图渲染模式为 1bit/pixel
		if ( ( ( 0 == pGlyphSlot->outline.n_contours )
			&& ( 0 == pGlyphSlot->outline.n_points ) )
			|| ( FT_PIXEL_MODE_MONO == glyphBitmap.pixel_mode ) )
			//if ( FT_GLYPH_FORMAT_BITMAP == pGlyphSlot->format )
		{
			if ( !m_fontRenderer->UploadMonoTexture( pGlyphFont->textureId, m_glyphOffX, m_glyphOffY, 
				static_cast< UINT >( pGlyphFont->uploadWidth ),
				static_cast< UINT >( pGlyphFont->uploadHeight ), 
				static_cast< UINT >( glyphBitmap.pitch ),
				glyphBitmap.buffer ) 
				)
			{
				KG3DFONT_ASSERT( !"UploadMonoTexture()" );
			}

		}
		else
		{
			if ( !m_fontRenderer->UploadTexture( pGlyphFont->textureId, m_glyphOffX, m_glyphOffY, 
				static_cast< UINT >( pGlyphFont->uploadWidth ),
				static_cast< UINT >( pGlyphFont->uploadHeight ), 
				glyphBitmap.buffer ) 
				)
			{
				KG3DFONT_ASSERT( !"UploadTexture()" );
			}

		}

	}


	m_glyphOffX += static_cast<INT>( pGlyphFont->uploadWidth );//static_cast<INT>( fontGlyph->glyphBUpper[ KG3DFONT_SLOT_X ] - fontGlyph->glyphBLower[ KG3DFONT_SLOT_X ] );
	m_glyphOffX += KG3DFONT_DEFAULT_PADDING;

	-- m_fontUploads;


	return true;
}


//bool  KG3DFontTexture::charGlyphInfill( UINT charCode, void* ftGlyph, KG3DFontGlyph_t* fontGlyph )
//{
//	// render glyph bitmap
//	FT_GlyphSlot pGlyphSlot = (FT_GlyphSlot)ftGlyph;
//	KG3DGlyphItem_t* pGlyphFont = ( KG3DGlyphItem_t *)fontGlyph;
//
//	FT_Error     ftErr   = 0;
//	KG3DFONT_ASSERT( NULL != pGlyphSlot );
//	KG3DFONT_ASSERT( NULL != pGlyphFont );
//
//	if ( NULL != ftGlyph )
//	{   
//		ftErr = FT_Render_Glyph( pGlyphSlot, m_fontFlagDraw );
//		if ( ftErr || pGlyphSlot->format != FT_GLYPH_FORMAT_BITMAP )
//		{
//			return false;
//		}
//
//	}
//	else
//	{
//		return false;
//	}
//
//
//	// request glyph texture
//	pGlyphFont->textureId = glyphTextureRequest();
//	if ( 0 != pGlyphFont->textureId )
//	{
//		// upload sub texture
//		FT_Bitmap glyphBitmap = pGlyphSlot->bitmap;
//
//
//		pGlyphFont->xOffset   = pGlyphSlot->bitmap_left;
//		pGlyphFont->yOffset   = pGlyphSlot->bitmap_top;
//
//		pGlyphFont->uploadTop  = m_glyphOffY;
//		pGlyphFont->uploadLeft = m_glyphOffX;
//
//		pGlyphFont->uploadWidth = glyphBitmap.width;
//		pGlyphFont->uploadHeight = glyphBitmap.rows;
//
//		pGlyphFont->textureUV[0][0] = static_cast<float>( m_glyphOffX ) / static_cast<float>( m_glyphTextureWidth );
//		pGlyphFont->textureUV[0][1] = static_cast<float>( m_glyphOffY ) / static_cast<float>( m_glyphTextureHeight );
//
//		pGlyphFont->textureUV[1][0] = static_cast<float>( m_glyphOffX + glyphBitmap.width ) / static_cast<float>( m_glyphTextureWidth );
//		pGlyphFont->textureUV[1][1] = static_cast<float>( m_glyphOffY + glyphBitmap.rows  ) / static_cast<float>( m_glyphTextureHeight );
//
//
//		//if ( FT_GLYPH_FORMAT_BITMAP == pGlyphSlot->format )
//		//{
//		//	pGlyphFont->glyphFormat = KG3DFONT_BITMAP;
//		//}
//		//if ( FT_GLYPH_FORMAT_OUTLINE == pGlyphSlot->format )
//		//{
//		//	pGlyphFont->glyphFormat = KG3DFONT_ANTIALIAS;
//		//}
//
//
//
//		// empty glyph?
//		// 如果轮廓线没有被上载，FreeType 使用的位图渲染模式为 1bit/pixel
//		if ( ( 0 == pGlyphSlot->outline.n_contours )
//			&& ( 0 == pGlyphSlot->outline.n_points ) )
//		//if ( FT_GLYPH_FORMAT_BITMAP == pGlyphSlot->format )
//		{
//			if ( !m_fontRenderer->UploadMonoTexture( pGlyphFont->textureId, m_glyphOffX, m_glyphOffY, 
//				pGlyphFont->uploadWidth, pGlyphFont->uploadHeight, glyphBitmap.pitch, glyphBitmap.buffer ) )
//			{
//				KG3DFONT_ASSERT( !"UploadMonoTexture()" );
//			}
//		
//		}
//		else
//		{
//			if ( !m_fontRenderer->UploadTexture( pGlyphFont->textureId, m_glyphOffX, m_glyphOffY, 
//				pGlyphFont->uploadWidth, pGlyphFont->uploadHeight, glyphBitmap.buffer ) )
//			{
//				KG3DFONT_ASSERT( !"UploadTexture()" );
//			}
//		
//		}
//
//	}
//
//
//	m_glyphOffX += pGlyphFont->uploadWidth;//static_cast<INT>( fontGlyph->glyphBUpper[ KG3DFONT_SLOT_X ] - fontGlyph->glyphBLower[ KG3DFONT_SLOT_X ] );
//	m_glyphOffX += KG3DFONT_DEFAULT_PADDING;
//
//	-- m_fontUploads;
//
//
//	return true;
//}
//
bool  KG3DFontTexture::charGlyphDestroy( )
{
	glyphTextureDelete();
	return true;
}



////////////////////////////////////////////////
// font texture
UINT  KG3DFontTexture::glyphTextureRequest( )
{
	// get glyph width / height
	INT  glyphW = static_cast<INT>( this->GetCharWidth() );
	INT  glyphH = static_cast<INT>( this->GetCharHeight() );


	// build texture
	UINT  uploadIndex = 0;

	if ( m_glyphTextureVector.empty() )
	{
		//m_glyphTextureWidth = ( m_fontUploads * glyphW );
		//m_glyphTextureHeight = m_fontGlyphs;
		m_glyphTextureWidth = ( KG3DFONT_GLYPH_NUMS * glyphW );
		m_glyphTextureHeight = KG3DFONT_GLYPH_NUMS;//m_fontGlyphs;


		if ( !m_fontRenderer->BuildTexture( uploadIndex, m_glyphTextureWidth, m_glyphTextureHeight, glyphW, glyphH ) )
		{
			KG3DFONT_ASSERT( !"BuildTexture()" );
		}


		m_glyphOffX = m_glyphOffY = KG3DFONT_DEFAULT_PADDING;
		m_glyphTextureLast = uploadIndex;
		m_glyphTextureVector.push_back( m_glyphTextureLast );

	}
	else if ( m_glyphOffX >= ( static_cast< INT >( m_glyphTextureWidth ) - glyphW ) )
	{
		m_glyphOffX = KG3DFONT_DEFAULT_PADDING;
		m_glyphOffY += glyphH + KG3DFONT_DEFAULT_PADDING;

		if ( m_glyphOffY >= ( static_cast< INT >( m_glyphTextureHeight ) - glyphH ) )
		{
			if ( !m_fontRenderer->BuildTexture( uploadIndex, m_glyphTextureWidth, m_glyphTextureHeight, glyphW, glyphH ) )
			{
				KG3DFONT_ASSERT( !"BuildTexture TWO()" );
			}


			m_glyphOffY = KG3DFONT_DEFAULT_PADDING;
			m_glyphTextureLast = uploadIndex;
			m_glyphTextureVector.push_back( m_glyphTextureLast );
		}

	}


	return m_glyphTextureLast;
}


void  KG3DFontTexture::glyphTextureDelete( )
{
	for ( size_t i = 0; i < m_glyphTextureVector.size(); ++ i )
	{
		m_fontRenderer->DeleteTexture( m_glyphTextureVector.at(i) );
	}

	m_glyphTextureLast = 0;
	m_glyphTextureVector.clear();
}
