////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DGraphicHelpers.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-7-24 14:39:53
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DGRAPHICHELPERS_H_
#define _INCLUDE_KG3DGRAPHICHELPERS_H_

////////////////////////////////////////////////////////////////////////////////
namespace KG3D_CUSTOM_HELPERS
{
	HRESULT FillTexture(LPDIRECT3DSURFACE9 pSuface, CONST RECT* pRect, D3DCOLOR color);//D3D的ColorFill有使用限制，这里可以通用，pRect是Fill的范围，可以传NULL，Fill整个Texture。非RenderTarget只支持D3DFMT_A8R8G8B8和D3DFMT_X8R8G8B8
	HRESULT FillTexture(LPDIRECT3DTEXTURE9 pTex, CONST RECT* pRect, D3DCOLOR color);//注意使用方式，Tex如果多层，Fill需要重新GenerateMipSubLevels，所以如果不想GenerateMipSubLevels，直接得到Surface，然后调用上面的版本
	HRESULT GenerateMipSubLevels(LPDIRECT3DTEXTURE9 pTex);	//解决某些贴图不是D3DUSAGE_AUTOGENMIPMAP不能调用GenerateMipSubLevels的问题
};


#endif //_INCLUDE_KG3DGRAPHICHELPERS_H_
