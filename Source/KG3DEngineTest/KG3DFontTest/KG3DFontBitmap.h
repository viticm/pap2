////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DFontBitmap.h
//  Version     : 1.0
//  Creator     : dengchao
//  Create Date : 2006-02-13 16:43:32
//  Comment     : font bitmap
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DFONTBITMAP_H_
#define _INCLUDE_KG3DFONTBITMAP_H_


#include  "KG3DFontBase.h"
#include  "KG3DFontTexture.h"



//////////////////////////////////////////////////////////////////////
//   class  :   KG3DFontBitmap
//   brief  :   bitmap font class
//   author :   dengchao
//   log    :     
//               
//////////////////////////////////////////////////////////////////////
class KG3DFontRenderer;
class KG3DFontBitmap : public KG3DFontTexture
{
public:
	////////////////////////////////////////////////
	// constructor / destructor
	KG3DFontBitmap( KG3DFontRenderer* pFontRenderer );
	virtual ~KG3DFontBitmap();


};



#endif //_INCLUDE_KG3DFONTBITMAP_H_