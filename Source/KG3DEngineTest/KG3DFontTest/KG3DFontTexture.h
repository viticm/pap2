////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DFontTexture.h
//  Version     : 1.0
//  Creator     : dengchao
//  Create Date : 2006-02-13 16:43:32
//  Comment     : font texture 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DFONTTEXTURE_H_
#define _INCLUDE_KG3DFONTTEXTURE_H_


#include  "KG3DFontBase.h"



//////////////////////////////////////////////////////////////////////
//   class  :   KG3DFontTexture
//   brief  :   texture font class
//   author :   dengchao
//   log    :     
//               
//////////////////////////////////////////////////////////////////////
class KG3DFontRenderer;
class KG3DFontTexture : public KG3DFontBase
{
public:
	////////////////////////////////////////////////
	// constructor / destructor
	KG3DFontTexture( KG3DFontRenderer* pFontRenderer );
	virtual ~KG3DFontTexture();


protected:
	////////////////////////////////////////////////
	// draw font member
	void  drawText( FLOAT penX, FLOAT penY, WCHAR chText );
	void  drawTextEx( FLOAT penX, FLOAT penY, WCHAR chText );


	////////////////////////////////////////////////
	// glyph font member
	bool  charGlyphMake( KG3DFontGlyph_t** fontGlyph );
	bool  charGlyphInfill( UINT charCode, void* ftGlyph, KG3DFontGlyph_t* fontGlyph );
	bool  charGlyphDestroy( );


	////////////////////////////////////////////////
	// font texture
	UINT  glyphTextureRequest( );
	void  glyphTextureDelete( );


protected:
	////////////////////////////////////////////////
	// glyph item
	struct KG3DGlyphItem_t : public KG3DFontGlyph_t
	{
		// glyph texture member
		INT		textureId;
		FLOAT	textureUV[2][2];
		FLOAT	borderUV[2][2];

		FLOAT	xOffset;
		FLOAT	yOffset;

		FLOAT	uploadTop, uploadLeft;
		FLOAT	uploadWidth, uploadHeight;


		// glyph effect
	
	};


	//typedef	std::vector< KG3DGlyphItem_t >  GlyphVector_t;
	//typedef	std::map< UINT, UINT >  GlyphMap_t;


	//GlyphMap_t			 m_glyphMap;
	//GlyphVector_t		 m_glyphVector;


	////////////////////////////////////////////////
	// texture buffer
	typedef std::vector< UINT >  GlyphTexVector_t;

	INT					 m_glyphOffX;
	INT					 m_glyphOffY;

	UINT				 m_glyphTextureLast;
	GlyphTexVector_t	 m_glyphTextureVector;

	UINT				 m_glyphTextureWidth;
	UINT				 m_glyphTextureHeight;


	////////////////////////////////////////////////
	// vertex buffer
	KG3DFontVertex_t	 m_glyphVertex[4];
	KG3DFontTLVertex_t	 m_glyphTLVertex[4];


};



#endif //_INCLUDE_KG3DFONTTEXTURE_H_