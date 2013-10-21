////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : ShaderDefinitions.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2009-7-16 17:40:31
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_SHADERDEFINITIONS_H_
#define _INCLUDE_SHADERDEFINITIONS_H_

////////////////////////////////////////////////////////////////////////////////

//!!!!!!! Do not write comment in Chinese in this file, because that could cause failing in compiling shaders

#ifdef __cplusplus
typedef D3DXVECTOR4 float4;
typedef D3DXVECTOR3 float3;
typedef D3DXVECTOR2 float2;

typedef float		(float3x3)[3*3];	//this is a float 2D array typedef gramma
typedef float		(float2x2)[2*2];
typedef D3DXMATRIX float4x4;
#endif

//////////////////////////////////////////////////////////////////////////
struct ShaderConstsSkyBox 
{
	float fScale;
#ifdef __cplusplus
	ShaderConstsSkyBox()
	{
		fScale = 1;
	}
#endif
};

struct ShaderVariantsSkyBox
{
	float4x4	matViewProj;
	float	fBlendPercent;
	float3	vCamPos;

	float	fBottomBlendStartMinusEnd;//it can't be zero, make sure the app check it before apply it to shader
	float	fBottomBlendEnd;

	float3	vBottomColor;
#ifdef __cplusplus
	ShaderVariantsSkyBox()
	{
		D3DXMatrixIdentity(&matViewProj);
		fBlendPercent = 0;
	}
#endif
};
//////////////////////////////////////////////////////////////////////////
struct ShaderConstsFarmountain 
{
	float fScale;
#ifdef __cplusplus
	ShaderConstsFarmountain()
	{
		fScale = 30000;
	}
#endif
};

struct ShaderVariantsFarMountain
{
	float4x4	matViewProj;
	float3		vColor;
	float4x4	matAnimation;
	float3		vCamPos;

	float		fRadius;
	float		fZMin;
	float		fHeight;
	float		fUTiles;
	float		fEnableTex;
	float4x4	matZRotation;
	float		fBotmFadePercent;


	float3		vBlendColor;
	float		fBlendFactor;

    float       fTexFactor;

#ifdef __cplusplus
	ShaderVariantsFarMountain()
	{
		ZeroMemory(this, sizeof(*this));
		//D3DXMatrixIdentity(&matViewProj);
	}
#endif
};
//////////////////////////////////////////////////////////////////////////
struct ShaderConstsLensflare
{
	float4x4     matRotation;
	float2       vSunPosPostProj;
	float2       vTexSplit;
	float        fScreenAspect;    //Width/Height    To adjust the sun shape
	float        fBlendFactor;
	float        fOCFactor;
#ifdef __cplusplus
	ShaderConstsLensflare()
	{
		memset(this, 0, sizeof(*this));
	}
#endif
};

#endif //_INCLUDE_SHADERDEFINITIONS_H_
