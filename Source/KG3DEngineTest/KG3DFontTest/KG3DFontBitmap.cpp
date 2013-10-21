////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DFontBitmap.cpp
//  Version     : 1.0
//  Creator     : dengchao
//  Create Date : 2006-02-13 16:43:32
//  Comment     : font texture
//
////////////////////////////////////////////////////////////////////////////////


#include  <ft2build.h>
#include  FT_FREETYPE_H
//#include  FT_INTERNAL_OBJECTS_H 

#include  "KG3DFontBitmap.h"
#include  "KG3DFontTexture.h"
#include  "KG3DFontRenderer.h"


////////////////////////////////////////////////
// constructor / destructor
KG3DFontBitmap::KG3DFontBitmap( KG3DFontRenderer* pFontRenderer )
		:	KG3DFontTexture( pFontRenderer )
{
	m_fontFlagDraw		= FT_RENDER_MODE_MONO;
	m_fontFlagLoader	= FT_LOAD_DEFAULT | FT_LOAD_IGNORE_GLOBAL_ADVANCE_WIDTH;
	m_fontFlagLoader   |= FT_LOAD_NO_HINTING;
}


KG3DFontBitmap::~KG3DFontBitmap()
{

}
