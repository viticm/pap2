////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : SkyBox.fx
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2009-7-16 16:05:52
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_SKYBOX_H_
#define _INCLUDE_SKYBOX_H_

#include "ShaderDefinitions.h"

struct vs_out 
{
	float4 pos : POSITION;
	float2 texCd : TEXCOORD0;
};

ShaderConstsSkyBox g_shaderConstsSkyBox;
ShaderVariantsSkyBox g_shaderVariantsSkyBox;

texture g_texMain;
texture g_texSub;
sampler2D g_samMain = sampler_state{
	texture = <g_texMain>;
	MagFilter = Linear;
	MipFilter = Linear;
	MinFilter = Linear;
};
sampler2D g_samSub = sampler_state{
	texture =  <g_texSub>;
	MagFilter = Linear;
	MipFilter = Linear;
	MinFilter = Linear;
};

void vs_func(float3 iPos : POSITION
			 ,out vs_out outRet)
{
	float3 posRotated;; //a sphere which's radius is 1

	posRotated.x = iPos.x;
	posRotated.y = iPos.z;
	posRotated.z = -iPos.y;

	float3 posTmp = posRotated;
	posTmp *= g_shaderConstsSkyBox.fScale;
	
	posTmp += g_shaderVariantsSkyBox.vCamPos;//Move with the camera

	outRet.pos = mul(float4(posTmp, 1), g_shaderVariantsSkyBox.matViewProj);
	
	outRet.texCd.x = asin(sign(posRotated.x) * saturate(abs(posRotated.x)));

	float fTmp = ((posRotated.y + 1) / 2);	//it is z aligned, mat will transform it to y aligned
	outRet.texCd.y = 1 - fTmp;	
}

float4 ps_func(vs_out psIn) : COLOR
{
	float fBottomColorPercent = ((1 - psIn.texCd.y) - g_shaderVariantsSkyBox.fBottomBlendEnd) / (g_shaderVariantsSkyBox.fBottomBlendStartMinusEnd);

	fBottomColorPercent = saturate(1 - fBottomColorPercent);

	float4 texValueMain = tex2D(g_samMain, psIn.texCd);
	float4 texValueSub = tex2D(g_samSub, psIn.texCd);

	float3 texColor = texValueMain.rgb * (1 - g_shaderVariantsSkyBox.fBlendPercent) 
		+ texValueSub.rgb * g_shaderVariantsSkyBox.fBlendPercent;

	float3 retColor = texColor * (1-fBottomColorPercent) + g_shaderVariantsSkyBox.vBottomColor * fBottomColorPercent;

	return float4(retColor,1);
}

technique techDefault
{
	pass p0
	{
		VertexShader = compile vs_2_0 vs_func();
		PixelShader = compile ps_2_0 ps_func();

		ZEnable = False;
		ZWriteEnable = False;
		CullMode = CW;
		FogEnable = False;

		//FillMode = WireFrame;

		AlphaBlendEnable = False;
	}
};

#endif
