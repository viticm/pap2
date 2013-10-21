////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : IEKG3DTextureTable.h
//  Version     : 1.0
//  Creator     : Xia Yong
//  Create Date : 2006-11-9 14:14:29
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_IEKG3DTEXTURETABLE_H_
#define _INCLUDE_IEKG3DTEXTURETABLE_H_

////////////////////////////////////////////////////////////////////////////////
class IEKG3DTexture;
interface IEKG3DTextureTable
{
    virtual HRESULT ReloadFromFile() = 0;
    virtual HRESULT AutoReload() = 0;
    virtual HRESULT GetIEKG3DTexture(IEKG3DTexture** ppTexture, DWORD dwID) = 0;
    virtual HRESULT IELoadResourceFromFile(
        const CHAR cszFileName[], 
        DWORD uFileNameHash,
        DWORD uOption, 
        IKG3DResourceBase **ppRetResource
    ) = 0;
	virtual IEKG3DTexture* GetRenderTargetTexture(UINT uWidth,UINT uHeight, D3DFORMAT Format) = 0;

	virtual int Detail_GetDefineTableCount() = 0;
	virtual HRESULT Detail_GetDefineTableName(int nTable,LPSTR pName) = 0;
	virtual HRESULT Detail_SetDefineTableName(int nTable,LPSTR pName) = 0;

	virtual HRESULT Detail_AddDefineTable() = 0;
	virtual HRESULT Detail_DeleteDefineTable(int nTable) = 0;

	virtual int Detail_GetBodyDefineCount(int nTable) = 0;

	virtual HRESULT Detail_AddBodyDefine(int nTable,int nIndex) = 0;
	virtual HRESULT Detail_DeleteBodyDefine(int nTable,int nIndex) = 0;

	virtual HRESULT Detail_GetBodyColor(int nTable,int nIndex,D3DCOLORVALUE& vBodyColor,D3DCOLORVALUE& vBodyColor1,float& fMax,float& fMin,float& fScale,float& fEnv,float& fSpecular,LPSTR pName,
		D3DCOLORVALUE& vColor0,float& fColorScale0,float& fUVScale0,float& fDetail0_Env,float& fDetail0_Specular,LPSTR pFileName0,
		D3DCOLORVALUE& vColor1,float& fColorScale1,float& fUVScale1,float& fDetail1_Env,float& fDetail1_Specular,LPSTR pFileName1,
		D3DCOLORVALUE* pFlow_Color,float* pfFlow_ColorScale,float* pfFlow_UVScale,D3DXVECTOR2* pvSpeed,LPSTR pFlowTexture
		) = 0;
	virtual HRESULT Detail_SetBodyColor(int nTable,int nIndex,D3DCOLORVALUE  vBodyColor,D3DCOLORVALUE  vBodyColor1,float  fMax,float  fMin,float  fScale,float  fEnv,float  fSpecular,LPSTR pName,
		D3DCOLORVALUE  vColor0,float  fColorScale0,float  fUVScale0,float fDetail0_Env,float fDetail0_Specular,LPSTR pFileName0,
		D3DCOLORVALUE  vColor1,float  fColorScale1,float  fUVScale1,float fDetail1_Env,float fDetail1_Specular,LPSTR pFileName1,
		D3DCOLORVALUE Flow_Color,float fFlow_ColorScale,float fFlow_UVScale,D3DXVECTOR2 vSpeed,LPSTR pFlowTexture
		) = 0;
	
	virtual HRESULT SaveDetailDefines() = 0;


};
#endif //_INCLUDE_IEKG3DTEXTURETABLE_H_
