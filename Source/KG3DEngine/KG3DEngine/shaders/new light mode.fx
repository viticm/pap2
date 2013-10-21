/////////////////////////////////////////////////////////////////////////////////
//    
//    FileName  : new light mode.fx
//    version   : 1.0
//    desc      : new light mode.fx  
//
/////////////////////////////////////////////////////////////////////////////////
/*
Normal Check: If this file is included by another file, which has a header that set NORMAL_CHECK as 1
, then the normal-checking part in his shader file will be activated.
These is a benefit obviously that no matter how much we change the file, even the input structure,
the normal checking function remains available. 
*/
#ifndef NORMAL_CHECK
#define NORMAL_CHECK 0
#endif

#include "CommonParams.h"
#include "PointLight.h"
#include "ShadowMap.h"

float4x4 WorldViewProj; 
float4x4 matWorld;

int bAlphaUseFactor = false;

int nIndex;
     
sampler s0 : register(s0);
sampler s1 : register(s1);
sampler s2 : register(s2);
sampler s3 : register(s3);    



texture SelTexture;
sampler2D selectTextureSampler = sampler_state
{
    Texture = <SelTexture>;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    AddressU = BORDER;
    AddressV = BORDER;
    BorderColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
};

struct vs_in
{
    float4 pos     : POSITION;
    float3 nor     : NORMAL;
    float4 weights : BLENDWEIGHT;
    float4 indices : BLENDINDICES;
    float2 tex0    : TEXCOORD0;
};

struct vs_out
{
        float4 hpos   : POSITION;
        float2 tex0   : TEXCOORD0;
        float4 texe_fFog  : TEXCOORD1;
        float4 texSel_Conver : TEXCOORD2;
#if SHADER_MODEL==3
        float3 wpos   : TEXCOORD3;
        float3 nol    : TEXCOORD4;
		float4 shadowPos :TEXCOORD5;
#endif
        float4 diff   : COLOR0;
        float4 spec   : COLOR1;
};

struct VS_Z_OUTPUT
{
    float4    Pos : POSITION;
    float2    Tex : TEXCOORD0;
    float2    rawpos : TEXCOORD1;
};

VS_Z_OUTPUT VS_ZBUFFER(float3 Pos  : POSITION, float2 Tex : TEXCOORD0) 
{
    VS_Z_OUTPUT Out;
    Out.Pos = mul(float4(Pos,1),WorldViewProj);
	Out.rawpos.x = Out.Pos.z;
	Out.rawpos.y = Out.Pos.w;
    Out.Tex = Tex;
    return Out;
}

float4 PS_Z(float2 tex0   : TEXCOORD0,
		   float2 pos : TEXCOORD1) : COLOR
{
	float4 color = tex2D(s0,   tex0); 
    if (bAlphaUseFactor)
    {
        color.a = 1.0f;
    }
    //color.rgb = PackingFloatToRGB(pos.x / pos.y).rgb;
    color.rgb = PackingFloatToRGB(pos.x / pos.y).rgb;
    return color;
}

float4 PS_Selector(float2 tex0 : TEXCOORD0): COLOR
{
    float4 color = tex2D(s0, tex0);
    color.rgb = nIndex / 255.0f;
    return color;
}

vs_out vs(vs_in input
#if SHADER_MODEL==2
#if NORMAL_CHECK == 1
    , out float3 oNormal : TEXCOORD3
#endif
#endif
    , uniform bool b_spec, uniform bool b_emap)
{
    vs_out output = (vs_out)0;
      
    float3 pos = input.pos;
    float3 nor = input.nor;
    
	float3 pos_wrd = mul(float4(pos, 1), matWorld);

	output.hpos    = mul(float4(pos, 1), WorldViewProj);

	nor                   = normalize(mul(nor, matWorld));
	output.tex0    = input.tex0;
	output.texSel_Conver.zw  = ConverUV(pos_wrd.x,pos_wrd.z);
	float3 eye     = normalize(eyes - pos_wrd);

#if SHADER_MODEL==3
	output.wpos    = pos_wrd;
	output.nol     = nor; 
	output.shadowPos = output.hpos;
#endif

	if (b_emap)
		output.texe_fFog.rgb    = normalize(reflect(-eye, nor));
	else
		output.texe_fFog.rgb    = float3(0, 0, 0);
	output.texe_fFog.w = CalFogFactor(output.hpos.z);

	float comlight = max(0, dot(sunLight.m_dir, nor));
	float light    = max(0, dot(-sunLight.m_dir, nor));
	float skylight = max(0, dot(-sky_light_dir, nor));      

	float3 R              =  normalize(2 * light * nor + sunLight.m_dir);

#if SHADER_MODEL==3
	output.diff = float4(0,0,0,0);
#else
	output.diff    = materialCur.diff * (light * sunLight.m_diffuse + comlight * sunLight.m_com_light 
					+ skylight * sunLight.m_sky_light) + sunLight.m_sceneAmbient * materialCur.ambi;    
#endif
	output.diff.a  = materialCur.diff.a;
	if (b_spec)                       
		output.spec    = sunLight.m_specular * materialCur.spec * pow(max(0, dot(R, eye)), materialEx.spe_exp) * materialCur.specPower;
	else
		output.spec    = float4(0, 0, 0, 0);

#if SHADER_MODEL==2
#if NORMAL_CHECK == 1    
	oNormal = normalCheckVS(input.nor, matWorld);    
#endif
#endif
    return output;
}

#if SHADER_MODEL==3
float4 point_light(float4 diff, float3 normal, float3 pos)
{
    float4 light = diff; 
    
    light.rgb += CalPointLight(normal, pos);
    
    return light;
}
#endif

float4 ps(float2 tex0   : TEXCOORD0,
		  float4 texe_fFog   : TEXCOORD1,
		  float4 texSel : TEXCOORD2,
#if SHADER_MODEL==2
#if NORMAL_CHECK == 1
		  float3 iNormal : TEXCOORD3,
#endif
#endif
		  float4 diff   : COLOR0,
		  float4 spec   : COLOR1,
#if SHADER_MODEL==3
		  float3 wpos   : TEXCOORD3,
		  float3 nol    : TEXCOORD4,
		  float4 screenpos : TEXCOORD5,
#endif
		  uniform bool b_spec, 
		  uniform bool b_emap,
		  uniform bool b_Convermap
		  ) : COLOR
{
	float4 r;
	float4 col = tex2D(s0,   tex0);                // diffuse map
	float4 chn = tex2D(s1,   tex0);              // channle map (r : emissive,  g : color cast,  b : specluar,  a : reflect)
	if (bAlphaUseFactor)
	{
		col.a = 1.0f;
	}
	float a = diff.a * col.a;

	if (materialEx.bhightlight)
		col *= 2;
#if SHADER_MODEL==3
	screenpos.xyz /= screenpos.w;
	screenpos.x = screenpos.x * 0.5 + 0.5;
	screenpos.y = screenpos.y * (-0.5) + 0.5;
	float shadowmask = 0;
	if (shadowMapParam.bShadowMask)
		shadowmask = GetShadowMask(screenpos.xy);
	else
		shadowmask = 1.0f;
		
	//float lumvalue = 1.0;
	//lumvalue = dot(col.xyz, LUMINANCE_VECTOR) + 0.0001f;
 //   lumvalue = lerp(1,lumvalue,sharpValue);
	//shadowmask *= lumvalue;
	
	float comlight = max(0, dot(sunLight.m_dir, nol));
	float light    = max(0, dot(-sunLight.m_dir, nol));
	float skylight = max(0, dot(-sky_light_dir, nol));      

	//float3 R =  normalize(2 * light * nol + sunLight.m_dir);	
	//float3 eye     = normalize(eyes - wpos);
	
	//float4 specLight = light > 0; 
	//float4 specLight = sunLight.m_specular /** materialCur.spec*/ * pow(max(0, dot(R, eye)), /*materialEx.spe_exp*/20)/* * materialCur.specPower*/;
#if SPECULARENABLE == 1
	float4 specLight = sunLight.m_specular * computerSpecLight(eyes - wpos,nol,-sunLight.m_dir,20);
#endif

	diff    = materialCur.diff * (light * sunLight.m_diffuse + comlight * sunLight.m_com_light + skylight * sunLight.m_sky_light) * shadowmask+
		sunLight.m_sceneAmbient * materialCur.ambi;
#if SPECULARENABLE == 1
	diff += specLight * min(speValue,shadowmask);    
#endif

#endif                

	float4 cst = diff * chn.g * materialEx.color_cast;        
	float4 ems = col  * chn.r * materialEx.emssive_power ;

#if SHADER_MODEL==3
	diff = point_light(diff, nol, wpos);        
#endif
	diff = (diff + float4(cst.r, cst.g, cst.b, 0.0) + materialCur.emis) * col;  

	if(b_Convermap)
	{
		float4 converColor = tex2D(SamplerConver,texSel.zw) * 1.5f;
		r = diff * converColor;
	}
	else
	{
		r = diff;
	}
	r = CalFogColor(r, texe_fFog.w); 
	r.a =a;

#if SHADER_MODEL==2
#if NORMAL_CHECK == 1
	return float4(iNormal,1);
#endif
#endif
	return r;
}

// for project texture 
#define Height_Limit 100
float  proj_size;
float3 proj_center;

vs_out vsWithSelTexture(vs_in input
#if SHADER_MODEL==2
#if NORMAL_CHECK == 1
		  , out float3 oNormal : TEXCOORD3
#endif
#endif
		  , uniform bool b_spec, uniform bool b_emap)
{
	vs_out output = (vs_out)0;

	float3 pos = input.pos;
	float3 nor = input.nor;

	float3 pos_wrd = mul(float4(pos, 1), matWorld);

	output.hpos    = mul(float4(pos, 1), WorldViewProj);

	nor                   = normalize(mul(nor, matWorld));
	output.tex0    = input.tex0;
	//output.texSel_Conver.zw  = ConverUV(pos_wrd.x,pos_wrd.z);
	float3 eye     = normalize(eyes - pos_wrd);

#if SHADER_MODEL==3
	output.wpos    = pos_wrd;
	output.nol     = nor; 
	output.shadowPos = output.hpos;
#endif

	if (b_emap)
		output.texe_fFog.rgb    = normalize(reflect(-eye, nor));
	else
		output.texe_fFog.rgb    = float3(0, 0, 0);
	output.texe_fFog.w = CalFogFactor(output.hpos.z);

	float comlight = max(0, dot(sunLight.m_dir, nor));
	float light    = max(0, dot(-sunLight.m_dir, nor));
	float skylight = max(0, dot(-sky_light_dir, nor));      

	float3 R              =  normalize(2 * light * nor + sunLight.m_dir);

#if SHADER_MODEL==3
	output.diff = float4(0,0,0,0);
#else
	output.diff    = materialCur.diff * (light * sunLight.m_diffuse + comlight * sunLight.m_com_light 
		+ skylight * sunLight.m_sky_light) + sunLight.m_sceneAmbient * materialCur.ambi;    
#endif
	output.diff.a  = materialCur.diff.a;
	if (b_spec)                       
		output.spec    = sunLight.m_specular * materialCur.spec * pow(max(0, dot(R, eye)), materialEx.spe_exp) * materialCur.specPower;
	else
		output.spec    = float4(0, 0, 0, 0);

#if SHADER_MODEL==2
#if NORMAL_CHECK == 1    
	oNormal = normalCheckVS(input.nor, matWorld);    
#endif
#endif
    // project texture
    if (abs(pos_wrd.y - proj_center.y) < Height_Limit)
    {
        float3 proj_space = (pos_wrd - proj_center + float3(proj_size / 2.f, 0.f, proj_size / 2.f));
        output.texSel_Conver.x = proj_space.x / proj_size;
        output.texSel_Conver.y = proj_space.z / proj_size;
        output.texe_fFog.z = pos_wrd.y;
    }
    else
    {
        output.texSel_Conver.x = 10000.f;
        output.texSel_Conver.y = 10000.f;
        output.texe_fFog.z = pos_wrd.y;
    }
        
    output.texSel_Conver.zw = ConverUV(pos_wrd.x,pos_wrd.z);
	return output;
}


float4 psWithSelTexture(float2 tex0   : TEXCOORD0,
		  float4 texe_fFog   : TEXCOORD1,
		  float4 texSel : TEXCOORD2,
#if SHADER_MODEL==2
#if NORMAL_CHECK == 1
		  float3 iNormal : TEXCOORD3,
#endif
#endif
		  float4 diff   : COLOR0,
		  float4 spec   : COLOR1,
#if SHADER_MODEL==3
		  float3 wpos   : TEXCOORD3,
		  float3 nol    : TEXCOORD4,
		  float4 screenpos : TEXCOORD5,
#endif
		  uniform bool b_spec, 
		  uniform bool b_emap,
		  uniform bool b_Convermap
		  ) : COLOR
{
	float4 r;
	float4 col = tex2D(s0,   tex0);                // diffuse map
	float4 chn = tex2D(s1,   tex0);              // channle map (r : emissive,  g : color cast,  b : specluar,  a : reflect)
	float4 aoe = tex2D(selectTextureSampler, texSel); // SelectTexture Map
	if (bAlphaUseFactor)
	{
		col.a = 1.0f;
	}
	float a = diff.a * col.a;

	if (materialEx.bhightlight)
		col *= 2;
#if SHADER_MODEL==3
	screenpos.xyz /= screenpos.w;
	screenpos.x = screenpos.x * 0.5 + 0.5;
	screenpos.y = screenpos.y * (-0.5) + 0.5;
	float shadowmask = 0;
	if (shadowMapParam.bShadowMask)
		shadowmask = GetShadowMask(screenpos.xy);
	else
		shadowmask = 1.0f;

	//float lumvalue = 1.0;
	//lumvalue = dot(col.xyz, LUMINANCE_VECTOR) + 0.0001f;
	//lumvalue = lerp(1,lumvalue,sharpValue);
	//shadowmask *= lumvalue;

	float comlight = max(0, dot(sunLight.m_dir, nol));
	float light    = max(0, dot(-sunLight.m_dir, nol));
	float skylight = max(0, dot(-sky_light_dir, nol));      

	//float3 R =  normalize(2 * light * nol + sunLight.m_dir);	
	//float3 eye     = normalize(eyes - wpos);

	//float4 specLight = light > 0; 
	//float4 specLight = sunLight.m_specular /** materialCur.spec*/ * pow(max(0, dot(R, eye)), /*materialEx.spe_exp*/20)/* * materialCur.specPower*/;
#if SPECULARENABLE == 1
	float4 specLight = sunLight.m_specular * computerSpecLight(eyes - wpos,nol,-sunLight.m_dir,20);
#endif

	diff    = materialCur.diff * (light * sunLight.m_diffuse + comlight * sunLight.m_com_light + skylight * sunLight.m_sky_light) * shadowmask+
		sunLight.m_sceneAmbient * materialCur.ambi;// + specLight * min(speValue,shadowmask);    
#if SPECULARENABLE == 1
	diff += specLight * min(speValue,shadowmask);
#endif
#endif                

	float4 cst = diff * chn.g * materialEx.color_cast;        
	float4 ems = col  * chn.r * materialEx.emssive_power ;

#if SHADER_MODEL==3
	diff = point_light(diff, nol, wpos);        
#endif
	diff = (diff + float4(cst.r, cst.g, cst.b, 0.0) + materialCur.emis) * col;  


	if(b_Convermap)
	{
		float4 converColor = tex2D(SamplerConver,texSel.zw) * 1.5f;
		r = diff * converColor;
	}
	else
	{
		r = diff;
	}
	r = CalFogColor(r, texe_fFog.w); 

	// project texture  
	r = r * (1.f - aoe.a) + aoe * aoe.a;

	r.a =a;

#if SHADER_MODEL==2
#if NORMAL_CHECK == 1
	return float4(iNormal,1);
#endif
#endif
	return r;
}

//
//vs_out vsWithSelTexture(vs_in input	, uniform bool b_spec, uniform bool b_emap)
//{
//    vs_out output ;
//      
//    float3 pos = input.pos;
//    float3 nor = input.nor;
//    
//    float3 pos_wrd = mul(float4(pos, 1), matWorld);
//    
//    output.hpos    = mul(float4(pos, 1), WorldViewProj);
//#if SHADER_MODEL==3
//
//	output.wpos = pos;
//	output.nol = nor;
//	output.shadowPos = output.hpos;
//#endif
//    nor    = normalize(mul(nor, matWorld));
//    output.tex0    = input.tex0;
//        
//    float3 eye= normalize(eyes - pos_wrd);
//    
//    if (b_emap)
//        output.texe_fFog.rgb = normalize(reflect(-eye, nor));
//    else
//        output.texe_fFog.rgb = float3(0, 0, 0);
//	 output.texe_fFog.a  = CalFogFactor(output.hpos.z);
//           
//    float comlight = max(0, dot(sunLight.m_dir, nor));
//    float light    = max(0, dot(-sunLight.m_dir, nor));
//    float skylight = max(0, dot(-sky_light_dir, nor));      
//  
//    float3 R = normalize(2 * light * nor + sunLight.m_dir);
//    
//    output.diff = saturate(materialCur.diff * (light * sunLight.m_diffuse + comlight * sunLight.m_com_light + skylight * sunLight.m_sky_light) + sunLight.m_sceneAmbient * materialCur.ambi);    
//                                                       
//    output.diff.a  = materialCur.diff.a;                           
//            
//    if (b_spec)                       
//        output.spec = sunLight.m_specular * materialCur.spec * pow(max(0, dot(R, eye)), materialEx.spe_exp) * materialCur.specPower;       
//    else
//        output.spec = float4(0, 0, 0, 0);        
//    
//    // project texture
//    if (abs(pos_wrd.y - proj_center.y) < Height_Limit)
//    {
//        float3 proj_space = (pos_wrd - proj_center + float3(proj_size / 2.f, 0.f, proj_size / 2.f));
//        output.texSel_Conver.x = proj_space.x / proj_size;
//        output.texSel_Conver.y = proj_space.z / proj_size;
//        output.texe_fFog.z = pos_wrd.y;
//    }
//    else
//    {
//        output.texSel_Conver.x = 10000.f;
//        output.texSel_Conver.y = 10000.f;
//        output.texe_fFog.z = pos_wrd.y;
//    }
//        
//    output.texSel_Conver.zw = ConverUV(pos_wrd.x,pos_wrd.z);
//    
//    return output;
//}
//
//float4 psWithSelTexture(float2 tex0   : TEXCOORD0,
//                 float4 texe_fFog   : TEXCOORD1,
//                 float4 texSel : TEXCOORD2,
//                 float4 diff   : COLOR0,
//                 float4 spec   : COLOR1,
//                 uniform bool b_spec, 
//                 uniform bool b_emap
//              ) : COLOR
//{
//	float4 r;
//	float4 col = tex2D(s0,   tex0);                // diffuse map
//	float4 chn = tex2D(s1,   tex0);              // channle map (r : emissive,  g : color cast,  b : specluar,  a : reflect)
//	float4 aoe = tex2D(selectTextureSampler, texSel); // SelectTexture Map
//	float a = diff.a * col.a;
//	if (bAlphaUseFactor)
//	{
//		col.a = 1.0f;
//	}    
//	if (materialEx.bhightlight)
//		col *= 2;
//
//	float4 cst = diff * chn.g * materialEx.color_cast;        
//	float4 ems = col  * chn.r * materialEx.emssive_power ;     
//
//	diff = (diff + float4(cst.r, cst.g, cst.b, 0.0) + materialCur.emis) * col;
//	float4 converColor = tex2D(SamplerConver,texSel.zw);       
//
//	// project texture  
//	r = diff * (1.f - aoe.a) + aoe * aoe.a;
//
//    r *= converColor * 1.5;    
//    r.a =a;
//	CalFogColor(r, texe_fFog.w);
//    return r;
//
//}
technique DefaultTechnique
{
    pass p0//spec
    {
        MinFilter[0] = LINEAR;
        MagFilter[0] = LINEAR;
        MipFilter[0] = LINEAR;
            
        MinFilter[1] = LINEAR;
        MagFilter[1] = LINEAR;
        MipFilter[1] = LINEAR;
            
        MinFilter[2] = LINEAR;
        MagFilter[2] = LINEAR;
        MipFilter[2] = LINEAR;
            
        vertexshader = compile VS_PROFILE vs(true, false);
        pixelshader  = compile PS_PROFILE ps(true, false,false);
		FogEnable = False;	//Fog is calculated in the shader, do not use the d3d fog, it affects the final effect
    }
    pass p0WithSelTexture
    {
         MinFilter[0] = LINEAR;
      MagFilter[0] = LINEAR;
        MipFilter[0] = LINEAR;
            
            MinFilter[1] = LINEAR;
        MagFilter[1] = LINEAR;
        MipFilter[1] = LINEAR;
            
            MinFilter[2] = LINEAR;
        MagFilter[2] = LINEAR;
        MipFilter[2] = LINEAR;
            
      vertexshader = compile VS_PROFILE vsWithSelTexture(true, false);
      pixelshader  = compile PS_PROFILE psWithSelTexture(true, false,false);
    }
    pass p1//emap
    {
        MinFilter[0] = LINEAR;
        MagFilter[0] = LINEAR;
        MipFilter[0] = LINEAR;
            
        MinFilter[1] = LINEAR;
        MagFilter[1] = LINEAR;
        MipFilter[1] = LINEAR;
            
        MinFilter[2] = LINEAR;
        MagFilter[2] = LINEAR;
        MipFilter[2] = LINEAR;
        vertexshader = compile VS_PROFILE vs(false, true);
        pixelshader  = compile PS_PROFILE ps(false, true,false);
		FogEnable = False;	//Fog is calculated in the shader, do not use the d3d fog, it affects the final effect
    }
    pass p2//diff
    {
        MinFilter[0] = LINEAR;
        MagFilter[0] = LINEAR;
        MipFilter[0] = LINEAR;
            
        MinFilter[1] = LINEAR;
        MagFilter[1] = LINEAR;
        MipFilter[1] = LINEAR;
            
        MinFilter[2] = LINEAR;
        MagFilter[2] = LINEAR;
        MipFilter[2] = LINEAR;
            
        vertexshader = compile VS_PROFILE vs(false, false);
        pixelshader  = compile PS_PROFILE ps(false, false,false);
		FogEnable = False;	//Fog is calculated in the shader, do not use the d3d fog, it affects the final effect
    }
	pass p3//all
	{
		MinFilter[0] = LINEAR;
		MagFilter[0] = LINEAR;
		MipFilter[0] = LINEAR;

		MinFilter[1] = LINEAR;
		MagFilter[1] = LINEAR;
		MipFilter[1] = LINEAR;

		MinFilter[2] = LINEAR;
		MagFilter[2] = LINEAR;
		MipFilter[2] = LINEAR;

		vertexshader = compile VS_PROFILE vs(true, true);
		pixelshader  = compile PS_PROFILE ps(true, true,false);
		FogEnable = False;	//Fog is calculated in the shader, do not use the d3d fog, it affects the final effect
	}
}

technique ConverMapTechnique
{
    pass p0//spec
    {
        MinFilter[0] = LINEAR;
        MagFilter[0] = LINEAR;
        MipFilter[0] = LINEAR;
            
        MinFilter[1] = LINEAR;
        MagFilter[1] = LINEAR;
        MipFilter[1] = LINEAR;
            
        MinFilter[2] = LINEAR;
        MagFilter[2] = LINEAR;
        MipFilter[2] = LINEAR;
            
        vertexshader = compile VS_PROFILE vs(true, false);
        pixelshader  = compile PS_PROFILE ps(true, false,true);
		FogEnable = False;	//Fog is calculated in the shader, do not use the d3d fog, it affects the final effect
    }
    pass p0WithSelTexture
	{
		MinFilter[0] = LINEAR;
		MagFilter[0] = LINEAR;
		MipFilter[0] = LINEAR;

		MinFilter[1] = LINEAR;
		MagFilter[1] = LINEAR;
		MipFilter[1] = LINEAR;

		MinFilter[2] = LINEAR;
		MagFilter[2] = LINEAR;
		MipFilter[2] = LINEAR;

		vertexshader = compile VS_PROFILE vsWithSelTexture(true, false);
		pixelshader  = compile PS_PROFILE psWithSelTexture(true, false,true);
		FogEnable = False;	//Fog is calculated in the shader, do not use the d3d fog, it affects the final effect
	}
	pass p1//emap
	{
        MinFilter[0] = LINEAR;
        MagFilter[0] = LINEAR;
        MipFilter[0] = LINEAR;
            
        MinFilter[1] = LINEAR;
        MagFilter[1] = LINEAR;
        MipFilter[1] = LINEAR;
            
        MinFilter[2] = LINEAR;
        MagFilter[2] = LINEAR;
        MipFilter[2] = LINEAR;

        vertexshader = compile VS_PROFILE vs(false, true);
        pixelshader  = compile PS_PROFILE ps(false, true,true);
		FogEnable = False;	//Fog is calculated in the shader, do not use the d3d fog, it affects the final effect
    }
    pass p2//diff
    {
        MinFilter[0] = LINEAR;
        MagFilter[0] = LINEAR;
        MipFilter[0] = LINEAR;
            
        MinFilter[1] = LINEAR;
        MagFilter[1] = LINEAR;
        MipFilter[1] = LINEAR;
            
        MinFilter[2] = LINEAR;
        MagFilter[2] = LINEAR;
        MipFilter[2] = LINEAR;
            
        vertexshader = compile VS_PROFILE vs(false, false);
        pixelshader  = compile PS_PROFILE ps(false, false,true);
		FogEnable = False;	//Fog is calculated in the shader, do not use the d3d fog, it affects the final effect
    }
	pass p3//all
	{
		MinFilter[0] = LINEAR;
		MagFilter[0] = LINEAR;
		MipFilter[0] = LINEAR;

		MinFilter[1] = LINEAR;
		MagFilter[1] = LINEAR;
		MipFilter[1] = LINEAR;

		MinFilter[2] = LINEAR;
		MagFilter[2] = LINEAR;
		MipFilter[2] = LINEAR;

		vertexshader = compile VS_PROFILE vs(true, true);
		pixelshader  = compile PS_PROFILE ps(true, true,true);
		FogEnable = False;	//Fog is calculated in the shader, do not use the d3d fog, it affects the final effect
	}
}

technique  tecZ
{
    pass Pass0
    {
        vertexshader = compile VS_PROFILE VS_ZBUFFER();
        pixelshader =  compile  PS_PROFILE PS_Z();
    }
}

technique Selector
{
	pass Pass0
	{
		vertexshader = compile VS_PROFILE VS_ZBUFFER();
		pixelshader =  compile  PS_PROFILE PS_Selector();
		FogEnable = False;	//Fog is calculated in the shader, do not use the d3d fog, it affects the final effect
	}
}
