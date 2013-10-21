/////////////////////////////////////////////////////////////////////////////////
//    
//    FileName  : shaderskin_2.fx
//    version        : 1.0
//  
//  desc      : skin shader vs2.0
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
#include "Details.h"

static const int MAX_NUM_BONE_PER_SET = 68;

float4x3 matBones[MAX_NUM_BONE_PER_SET];
float4x4 WorldViewProj;
float4x4 matWorld;

int nNumBlend = 0;

int bAlphaUseFactor = false;

int nIndex;
    
sampler s0 : register(s0);
sampler s1 : register(s1);
sampler s2 : register(s2);
sampler s3 : register(s3);    

//////////////////////////////////////////////////////////////
struct vs_in
{
    float4 pos     : POSITION0;
    float4 pos1    : POSITION1;
    float3 nor     : NORMAL0;
    float3 nor1    : NORMAL1;
    float4 weights : BLENDWEIGHT;
    float4 indices : BLENDINDICES;
    float2 tex0    : TEXCOORD0;
};

struct vs_out
{
  float4 hpos   : POSITION;
  float4 tex0   : TEXCOORD0;
  float4 texe_fFog : TEXCOORD1;
#if SHADER_MODEL==3
  float3 wpos   : TEXCOORD2;
  float3 nol    : TEXCOORD3;
#endif
  float4 diff   : COLOR0;
  float4 spec   : COLOR1;
};

struct vs_outSelector
{
  float4 hpos   : POSITION;
  float2 tex0   : TEXCOORD0;
};


vs_out vs(vs_in input
#if SHADER_MODEL==2
#if NORMAL_CHECK == 1
        , out float3 oNormal : TEXCOORD2
#endif
#endif
        , uniform bool b_spec, uniform bool b_emap)
{
    vs_out output ;
      
    int4      IndexVector = D3DCOLORtoUBYTE4(input.indices);
    
    float3 pos = 0.0f;
    float3 nor = 0.0f;
    float3 inNor[2];
    float4 inPos[2];
    inPos[0] = input.pos;
    inPos[1] = input.pos1;
    inNor[0] = input.nor;
    inNor[1] = input.nor1;
    
    float matweight[4] = (float[4])input.weights;
    int   matIndex[4]  = (int[4])IndexVector;
    float fLastWeight  = 0.0f;
    
	if(nNumBlend == 2)
	{
		const int i = 0;
		pos += mul(inPos[i], matBones[matIndex[i]]) * matweight[i];
	        nor += mul(inNor[i], matBones[matIndex[i]]) * matweight[i];
	        fLastWeight += matweight[i];
	}   
    
    fLastWeight = 1.0f - fLastWeight;
    
    pos += mul(inPos[nNumBlend - 1], matBones[matIndex[nNumBlend - 1]]) * fLastWeight;
    nor += mul(inNor[nNumBlend - 1], matBones[matIndex[nNumBlend - 1]]) * fLastWeight;
    
    float3 pos_wrd = mul(float4(pos, 1), matWorld);
    
    output.hpos    = mul(float4(pos, 1), WorldViewProj);
    nor                   = normalize(mul(nor, matWorld));
    output.tex0.xy    = input.tex0;
    output.tex0.zw   = ConverUV(pos_wrd.x,pos_wrd.z);

    float3 eye     = normalize(eyes - pos_wrd);
    
#if SHADER_MODEL==3
    output.wpos = pos_wrd;
    output.nol = nor;
#endif
	if (b_emap)
	{
		output.texe_fFog.xyz    = normalize(reflect(-eye, nor));
	}
	else
		output.texe_fFog.xyz    = float3(0, 0, 0);
           
    float comlight = max(0, dot( sunLight.m_dir, nor));
    float light    = max(0, dot(-sunLight.m_dir, nor));
    float skylight = max(0, dot(-sky_light_dir, nor));      
  
    float3 R              =  normalize(2 * light * nor - sunLight.m_dir);
    
    output.diff    = (materialCur.diff * (light * sunLight.m_diffuse + comlight * sunLight.m_com_light + skylight * sunLight.m_sky_light) +
                                   sunLight.m_sceneAmbient * materialCur.ambi);    
#if SHADER_MODEL==3                                   
	output.diff += materialCur.emis;
#endif
                                  
    output.diff.a  = materialCur.diff.a;                           
            
	if (b_spec)                       
		output.spec    = sunLight.m_specular * materialCur.spec * pow(max(0, dot(R, eye)), materialEx.spe_exp) * materialCur.specPower;       
	else
		output.spec    = float4(0, 0, 0, 0);


    output.texe_fFog.w  =  CalFogFactor(output.hpos.z);
    
#if SHADER_MODEL==2
#if NORMAL_CHECK == 1    
	oNormal = normalCheckVS(input.nor, matWorld);    
#endif
#endif
    return output;
}

struct VS_OUTPUT_Detail
{
    float4 Pos          : POSITION;
    float4 Color        : COLOR0;
    float4 Specular     : COLOR1;
    float4 TexCoord     : TEXCOORD0;
    float4 eye_fFog     : TEXCOORD1;
#if SHADER_MODEL==3
    float3 wpos   : TEXCOORD2;
    float3 nol    : TEXCOORD3;
	float4 rawpos : TEXCOORD4;
#endif
};

VS_OUTPUT_Detail vs_Detail(vs_in input
#if SHADER_MODEL==2
#if NORMAL_CHECK == 1
        , out float3 oNormal : TEXCOORD2
#endif
#endif
        )
{
    VS_OUTPUT_Detail output ;
      
    int4      IndexVector = D3DCOLORtoUBYTE4(input.indices);
    
    float3 pos = 0.0f;
    float3 nor = 0.0f;
    float3 inNor[2];
    float4 inPos[2];
    inPos[0] = input.pos;
    inPos[1] = input.pos1;
    inNor[0] = input.nor;
    inNor[1] = input.nor1;
    
    float matweight[4] = (float[4])input.weights;
    int   matIndex[4]  = (int[4])IndexVector;
    float fLastWeight  = 0.0f;

    if(nNumBlend == 2)
    {
        const int i = 0;
        pos += mul(inPos[i], matBones[matIndex[i]]) * matweight[i];
        nor += mul(inNor[i], matBones[matIndex[i]]) * matweight[i];
        fLastWeight += matweight[i];
    } 
    
    fLastWeight = 1.0f - fLastWeight;
    
    pos += mul(inPos[nNumBlend - 1], matBones[matIndex[nNumBlend - 1]]) * fLastWeight;
    nor += mul(inNor[nNumBlend - 1], matBones[matIndex[nNumBlend - 1]]) * fLastWeight;
    
    float3 pos_wrd = mul(float4(pos, 1), matWorld);
    
    output.Pos    = mul(float4(pos, 1), WorldViewProj);
#if SHADER_MODEL==3
	output.rawpos = output.Pos;
#endif
    nor           = normalize(mul(nor, matWorld));
    output.TexCoord.xy   = input.tex0;
    output.TexCoord.zw   = ConverUV(pos_wrd.x,pos_wrd.z);
#if SHADER_MODEL==3
    output.wpos = pos_wrd;
    output.nol = nor;
#endif
    float3 eye     = normalize(eyes - pos_wrd);
    float edge = max(0,1 - max(0,dot(nor,eye)));
    output.Color = float4(nor,1);
    output.Specular = float4(pos_wrd,edge);
    output.eye_fFog.xyz = mul(nor,WorldViewProj);
  
    output.eye_fFog.w  =  CalFogFactor(output.Pos.z);

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

float4 ps(float4 tex0  : TEXCOORD0,
                  float4 texe_fFog  : TEXCOORD1,
#if SHADER_MODEL==3
                  float3 wpos  : TEXCOORD2,
                  float3 nol   : TEXCOORD3,
#endif
#if SHADER_MODEL==2
#if NORMAL_CHECK == 1
                  float3 iNormal : TEXCOORD2,
#endif
#endif
                  float4 diff  : COLOR0,
                  float4 spec  : COLOR1,

				  uniform bool b_spec, 
                  uniform bool b_emap,
                  uniform bool b_Convermap
              ) : COLOR
{

	float4 col = tex2D(s0,   tex0.xy);                // diffuse map
	if (bAlphaUseFactor)
	{
		col.a = 1.0f;
	}
	float4 chn = tex2D(s1,   tex0.xy);              // channle map (r : emissive,  g : color cast,  b : specluar,  a : reflect)
	float4 indiff = diff;
	float a = diff.a * col.a;
	if (materialEx.bhightlight)
		col *= 2;

	float4 cst = diff * chn.g * materialEx.color_cast;        
	float4 ems = col  * chn.r * materialEx.emssive_power ;     

#if SHADER_MODEL==3
	diff = point_light(diff, nol, wpos);  
#endif
	diff = (diff + float4(cst.r, cst.g, cst.b, 0.0) + materialCur.emis) * col;

	float4 r;

	if (b_spec && b_emap) 
	{
		float4 e = texCUBE(s2, texe_fFog.xyz) * chn.a;
		r = diff + ems + spec * chn.b + e;                
	}
	else if (b_spec)
	{
		r =  diff + ems + spec * chn.b;
	}
	else if (b_emap)
	{
		float4 e = texCUBE(s2, texe_fFog.xyz) * chn.a;
		r = diff + ems + e;
	}            
	else 
	{
		r = diff + ems;
	}

	if(b_Convermap)
	{
		float4 converColor = tex2D(SamplerConver,tex0.zw);
		r *= converColor * 1.5;
	}

	r = CalFogColor(r, texe_fFog.w); 
	r.a = a;

#if SHADER_MODEL==2
#if NORMAL_CHECK == 1
	return float4(iNormal, 1);
#endif
#endif
	return r;
}

float4 PS_LightCalc_PointLight(float4 Tex,float3 normal,float3 pos,float shadowmask)
{
 //   float k = max(0,dot(normal,-sunLight.m_dir));
 //   float side = max(0,dot(normal,sky_light_dir));
 //   
	//float4 diffPart = sunLight.m_diffuse * shadowmask*2;
 //   float4 light = lerp(sunLight.m_sceneAmbient,sunLight.m_sceneAmbient+diffPart,k) + materialCur.emis; 
 //   
 //   return light;
	float comlight = max(0, dot(sunLight.m_dir, normal));
	float light    = max(0, dot(-sunLight.m_dir, normal));
	float skylight = max(0, dot(-sky_light_dir, normal));   
	float4 diff = (light * sunLight.m_diffuse + comlight * sunLight.m_com_light + skylight * sunLight.m_sky_light) * shadowmask + sunLight.m_sceneAmbient;  
	return diff;
}

float4 ps_detail(float4 Tex0  : TEXCOORD0,
                  float4 eye_fFog : TEXCOORD1,
                  float4 incolor  : COLOR0,
                  float4 inspecular  : COLOR1
#if SHADER_MODEL==3
                  ,float3 wpos  : TEXCOORD2,
                  float3 nol   : TEXCOORD3,
				  float4 screenpos : TEXCOORD4
#endif

#if SHADER_MODEL==2
#if NORMAL_CHECK == 1
                  ,float3 iNormal : TEXCOORD2    //NormalChecker Function, it is activated while macro NORMAL_CHECK is set as 1
#endif
#endif
              ) : COLOR
{
	//float4 diffusebloom0 = tex2Dlod(s0, float4(Tex0.xy,0,1.5));
	float4 diffuse = tex2D(s0,Tex0.xy)*2;  
	float4 covermap = tex2D(SamplerConver , Tex0.zw);
    float4 col1 = lerp(1,covermap *1.5, covermap.a);
	float shadowmask = 1.0f;
	float4 chan = tex2D(SamplerChannel,Tex0.xy);
#if SHADER_MODEL==3	
	screenpos.xyz /= screenpos.w;
	screenpos.x = screenpos.x * 0.5 + 0.5;
	screenpos.y = screenpos.y * (-0.5) + 0.5;
	if (shadowMapParam.bShadowMask)
		shadowmask = GetShadowMask(screenpos.xy);
		
	float lumvalue = 1.0;
    lumvalue = dot(diffuse.xyz, LUMINANCE_VECTOR) + 0.0001f;
    lumvalue = lerp(1,lumvalue,sharpValue);
    shadowmask *= lumvalue;
    
    //float lightvec    = max(0, dot(-sunLight.m_dir, nol));
    //float3 R =  normalize(2 * lightvec * nol + sunLight.m_dir);	
    //float3 eye     = normalize(eyes - wpos);
    //specLight    = sunLight.m_specular /** materialCur.spec*/ * pow(max(0, dot(R, eye)), /*materialEx.spe_exp*/20)/* * materialCur.specPower*/;
#if SPECULARENABLE == 1  
	float4 specLight = sunLight.m_specular * computerSpecLight(eyes - inspecular.rgb,incolor.rgb,-sunLight.m_dir,20);
	float4 specLightSkin = playerLight.m_specular * computerSpecLight(eyes - inspecular.rgb,incolor.rgb,-playerLight.m_dir,20);
#endif
#endif

	float4 light = PS_LightCalc_PointLight(diffuse,incolor.rgb,inspecular.rgb,shadowmask);
	float4 skinColor = float4(0,0,0,0);
#if SHADER_MODEL==3
	skinColor = PS_LightCalc_Skin(diffuse,incolor.rgb,inspecular.rgb,diffuse,inspecular.a,shadowmask) + materialCur.emis*0.3;
#if SPECULARENABLE==1
	light += specLight * min(chan.r,shadowmask);
	skinColor += specLightSkin * min(chan.r,shadowmask);
#endif
	float4 colorPoint = float4(0,0,0,0);
	colorPoint = point_light(colorPoint, incolor.rgb, inspecular.rgb);
    light += colorPoint;
	skinColor += colorPoint;
#endif

	DetailResult dResult = CalDetail(chan,diffuse,light,skinColor, Tex0.xy, SameplerEnv, eye_fFog.xyz, incolor.g);
	float4 Out = (dResult.comColor + dResult.speculardetail) * col1 ;

	//Out = computerFresnel(Out,eyes - inspecular.rgb,incolor.rgb,8,sunLight.m_diffuse);
	Out = CalFogColor(Out, eye_fFog.w);

	Out.a = diffuse.a * materialCur.diff.a;

#if SHADER_MODEL==2
#if NORMAL_CHECK == 1
	return float4(iNormal, 1);
#endif
#endif
	return Out;
}

vs_outSelector vsSelector(vs_in input)
{
    vs_outSelector output ;
    int4      IndexVector = D3DCOLORtoUBYTE4(input.indices);
    float3 pos = 0.0f;
    float3 nor = 0.0f;
    float3 inNor[2];
    float4 inPos[2];
    inPos[0] = input.pos;
    inPos[1] = input.pos1;
    
    float matweight[4] = (float[4])input.weights;
    int   matIndex[4]  = (int[4])IndexVector;
    float fLastWeight  = 0.0f;
    
    if(nNumBlend == 2)
    {
		const int i = 0;
        pos += mul(inPos[i], matBones[matIndex[i]]) * matweight[i];
        fLastWeight += matweight[i];
    }
    
    fLastWeight = 1.0f - fLastWeight;
    
    pos += mul(inPos[nNumBlend - 1], matBones[matIndex[nNumBlend - 1]]) * fLastWeight;
    output.hpos    = mul(float4(pos, 1), WorldViewProj);
    output.tex0 = input.tex0;
    return output;
}


float4 psSelector(float4 tex0  : TEXCOORD0) : COLOR
{
    float4 col = tex2D(s0,   tex0.xy);
    if (bAlphaUseFactor)
    {
        col.a = 1.0f;
    }
    col.rgb = nIndex / 255.0f;
    return col;
}


technique DefaultTechnique
{
    pass p0
    {            
        vertexshader = compile VS_PROFILE vs(false, false);
        pixelshader  = compile PS_PROFILE ps(false, false,false);
		FogEnable = False;	//Fog is calculated in the shader, do not use the d3d fog, it affects the final effect
    }
	pass p1//spec
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
	pass p2//emap
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
	pass p3//diff
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
	pass p4//all
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
    pass p0
    {            
        vertexshader = compile VS_PROFILE vs(false, false);
        pixelshader  = compile PS_PROFILE ps(false, false,true);
		FogEnable = False;	//Fog is calculated in the shader, do not use the d3d fog, it affects the final effect
    }
    pass p1//spec
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
    pass p2//emap
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
    pass p3//diff
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
    pass p4//all
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

technique Selector
{
	pass p0
	{
		vertexshader = compile VS_PROFILE vsSelector();
		pixelshader  = compile PS_PROFILE psSelector();
		FogEnable = False;	//Fog is calculated in the shader, do not use the d3d fog, it affects the final effect
	}
}


technique ConverMapTechnique_Detail
{
    pass p0
    {            
        vertexshader = compile VS_PROFILE vs_Detail();
        pixelshader  = compile PS_PROFILE ps_detail();
		FogEnable = False;	//Fog is calculated in the shader, do not use the d3d fog, it affects the final effect
    }
    pass p1//spec
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
    pass p2//emap
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
    pass p3//diff
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
    pass p4//all
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

struct VS_Z_OUTPUT
{
    float4    Pos : POSITION;
    float2    rawpos : TEXCOORD0;
	float2    tex : TEXCOORD1;
};

VS_Z_OUTPUT VS_ZBUFFER(vs_in input) 
{
    VS_Z_OUTPUT Out;
      
    int4      IndexVector = D3DCOLORtoUBYTE4(input.indices);
    
    float3 pos = 0.0f;
    float3 nor = 0.0f;
    float3 inNor[2];
    float4 inPos[2];
    inPos[0] = input.pos;
    inPos[1] = input.pos1;
    inNor[0] = input.nor;
    inNor[1] = input.nor1;
    
    float matweight[4] = (float[4])input.weights;
    int   matIndex[4]  = (int[4])IndexVector;
    float fLastWeight  = 0.0f;
    
	if(nNumBlend == 2)
	{
		const int i = 0;
		pos += mul(inPos[i], matBones[matIndex[i]]) * matweight[i];
	    fLastWeight += matweight[i];
	}   
    
    fLastWeight = 1.0f - fLastWeight;
    
    pos += mul(inPos[nNumBlend - 1], matBones[matIndex[nNumBlend - 1]]) * fLastWeight;
    
       
    Out.Pos    = mul(float4(pos, 1), WorldViewProj);
	Out.rawpos.x = Out.Pos.z;
	Out.rawpos.y = Out.Pos.w;
	Out.tex = input.tex0;
    return Out;
}

float4 PS_Z(float2 pos : TEXCOORD0,float2 tex:TEXCOORD1) : COLOR
{
	float4 color = tex2D(s0,   tex.xy);
    if (bAlphaUseFactor)
    {
        color.a = 1.0f;
    }
    color.rgb = PackingFloatToRGB(pos.x / pos.y).rgb;
    return color;
}
technique  tecZ
{
    pass Pass0
    {
        vertexshader = compile VS_PROFILE VS_ZBUFFER();
        pixelshader =  compile PS_PROFILE PS_Z();
    }
}
