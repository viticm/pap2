////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : FarMountain.fx
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2009-7-23 11:10:05
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_FARMOUNTAIN_H_
#define _INCLUDE_FARMOUNTAIN_H_

#include "ShaderDefinitions.h"

struct vs_out 
{
	float4 pos : POSITION;
	float2 texCd : TEXCOORD0;
	float	posYPercent : TEXCOORD1;
};

ShaderConstsFarmountain g_shaderConsts;
ShaderVariantsFarMountain g_shaderVariants;

texture g_texMain;
sampler2D g_samMain = sampler_state{
	texture = <g_texMain>;
	MagFilter = Linear;
	MipFilter = Linear;
	MinFilter = Linear;
};

void vs_func(float3 iPos : POSITION
			 , float iTexCoordFactor: TEXCOORD0
			 ,out vs_out outRet)
{
	//iPos should be a cylinder, which is 1 unit tall, 1 unit radius, base face setted on xz plane.
	float3 posTmp = iPos;	//a sphere which's radius is 1
	posTmp *= g_shaderVariants.fRadius;

	posTmp.xyz = mul(posTmp.xyz, (float3x3)g_shaderVariants.matZRotation);	//y static rotation
	posTmp.y = g_shaderVariants.fZMin + g_shaderVariants.fHeight * iPos.y;	//stretch though y direction

	posTmp = mul(float4(posTmp, 1), g_shaderVariants.matAnimation);

	posTmp += g_shaderVariants.vCamPos;//Move with the camera

	outRet.pos = mul(float4(posTmp, 1), g_shaderVariants.matViewProj);
	outRet.texCd.y = 1 - iPos.y;
	outRet.texCd.x = iTexCoordFactor * g_shaderVariants.fUTiles;
	outRet.posYPercent = iPos.y;
}

float4 ps_func(vs_out psIn) : COLOR
{
	float4 texValue = tex2D(g_samMain, psIn.texCd);
	texValue.rgb *= g_shaderVariants.fTexFactor;
    texValue.rgb = saturate(texValue.rgb);
    
	float4 alphaOnlyValue = float4(g_shaderVariants.vColor.rgb, texValue.a);
	float4 retValue = g_shaderVariants.fEnableTex * texValue + (1 - g_shaderVariants.fEnableTex) * alphaOnlyValue;
	float fadeValue = saturate(psIn.posYPercent / g_shaderVariants.fBotmFadePercent);
	retValue.a = min(retValue.a, fadeValue);
	return retValue;
	//return float4(0,0,0,1);
}

technique techDefault
{
	pass p0
	{
		VertexShader = compile vs_2_0 vs_func();
		PixelShader = compile ps_2_0 ps_func();

		ZEnable = False;
		ZWriteEnable = False;
		CullMode = CCW;
		FogEnable = False;

		AlphaBlendEnable = True;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;
	}
};

#endif

