//-------------------------------------------
// Desc  : Speed Tree Shader
// Author: Zeb
// Data  : 2006-6-27
//-------------------------------------------
#include "CommonParams.h"
#include "PointLight.h"
#include "Details.h"

float4x4 mWorld;
float4x4 mViewProj;

sampler s0 : register(s0);
sampler s1 : register(s1);
sampler s2 : register(s2);
sampler s3 : register(s3);
sampler s4 : register(s4);
sampler s5 : register(s5);
sampler s6 : register(s6);
sampler s7 : register(s7);
sampler s8 : register(s8);

int nTextureStart = 0;
float HeightBase = 0;

float   Alpha = 1;
float e = 0.3;

////////////////////////////////////////////////////////////////////////
struct VS_OUTPUT
{
    float4 Pos          : POSITION;
    float4 Color        : COLOR0;
	float4 Specular     : COLOR1;
    float4 TexCoord     : TEXCOORD0;
    float  TexIndex     : TEXCOORD1;
    float3 eye          : TEXCOORD2;
	float  fFog       : TEXCOORD3;
};

struct VS_OUTPUT_Z
{
    float4 Pos          : POSITION;
    float2 TexCoord     : TEXCOORD0;
    float  TexIndex     : TEXCOORD1;
    float  depth        : TEXCOORD2;
};

VS_OUTPUT Vertex_Shader(
    float3 Pos      : POSITION, 
    float3 Normal   : NORMAL,
    float3 UVW      : TEXCOORD0)
{
    VS_OUTPUT Out = (VS_OUTPUT)0;
    
    float3 wpos = mul(float4(Pos, 1), mWorld).xyz;
    Out.TexCoord.xy = UVW.xy;
    Out.TexCoord.zw = ConverUV(wpos.x,wpos.z); 
    Out.TexIndex    = UVW.z - nTextureStart;

    Out.Pos = mul(float4(wpos, 1), mViewProj);
        
    float3 n = normalize(mul(Normal, mWorld).xyz);
    float s = dot(n, -playerLight.m_dir);
    float i = max(0, s);

    float3 eye = normalize(eyes - wpos); 
    float3 HalfV = normalize(eye - playerLight.m_dir );
    float ref = max(0, dot(HalfV, n));

    float edge = 1 - max(0,dot(n,eye));
	Out.Color = float4(n,1);
	Out.Specular = float4(wpos,edge);
	Out.eye = mul(n,mViewProj);

	Out.fFog = CalFogFactor(Out.Pos.z);;

    return Out;
}


VS_OUTPUT_Z Vertex_Z(
    float3 Pos      : POSITION, 
    float3 Normal   : NORMAL,
    float3 UVW      : TEXCOORD0)
{
    VS_OUTPUT_Z Out = (VS_OUTPUT_Z)0;
    
    float3 wpos = mul(float4(Pos, 1), mWorld).xyz;
    Out.TexCoord.xy = UVW.xy;
    Out.TexIndex = UVW.z - nTextureStart;
    Out.Pos = mul(float4(wpos, 1), mViewProj);
    Out.depth = Out.Pos.z / Out.Pos.w; 
    return Out;
}

float4 PS_LightCalc(float4 Tex,float r,float s,float sky,float4 col1)
{
    float L = (Tex.r + Tex.g + Tex.b) * 0.3333;
    float4 light = lerp(playerLight.m_sceneAmbient,playerLight.m_sceneAmbient+playerLight.m_diffuse,L*r); 
    light.a = 1;
    return light;
}

float4 PS_LightCalc_PointLight(float4 Tex,float3 normal,float3 pos)
{
    float L = (Tex.r + Tex.g + Tex.b) * 0.3333;
	float k = max(0,dot(normal,-playerLight.m_dir));
	float side = max(0,dot(normal,-sky_light_dir));
	
    float4 light = side*playerLight.m_sky_light + lerp(playerLight.m_sceneAmbient,playerLight.m_sceneAmbient+playerLight.m_diffuse,k); 
	light.rgb += CalPointLight(normal, pos);
    return light;
}

float4 MultiColor_4(float TexIndex,float2 UV)
{
        float4 diffuse = 0;
    if(abs(TexIndex )<=e)
    {
        diffuse += tex2D(s0, UV);
    }
    else if(abs(TexIndex-1)<=e)
    {
        diffuse += tex2D(s1 , UV);
    }
    else if(abs(TexIndex -2)<=e)
    {
        diffuse += tex2D(s2 , UV);
    }
    else if(abs(TexIndex -3)<=e)
    {
        diffuse += tex2D(s3 , UV);
    }
    return diffuse;
}

float4 MultiColor_8(float TexIndex,float2 UV)
{
        float4 diffuse = 0;
    if(abs(TexIndex )<=e)
    {
        diffuse += tex2D(s0, UV);
    }
    else if(abs(TexIndex-1)<=e)
    {
        diffuse += tex2D(s1 , UV);
    }
    else if(abs(TexIndex -2)<=e)
    {
        diffuse += tex2D(s2 , UV);
    }
    else if(abs(TexIndex -3)<=e)
    {
        diffuse += tex2D(s3 , UV);
    }
    else if(abs(TexIndex-4)<=e)
    {
        diffuse += tex2D(s4 , UV);
    }
    else if(abs(TexIndex -5)<=e)
    {
        diffuse += tex2D(s5 , UV);
    }
    else if(abs(TexIndex -6)<=e)
    {
        diffuse += tex2D(s6 , UV);
    }
    else if(abs(TexIndex -7)<=e)
    {
        diffuse += tex2D(s7 , UV);
    }
    return diffuse;
}

float4 MultiColor_12(float TexIndex,float2 UV)
{
        float4 diffuse = 0;
    if(abs(TexIndex )<=e)
    {
        diffuse += tex2D(s0, UV);
    }
    else if(abs(TexIndex-1)<=e)
    {
        diffuse += tex2D(s1 , UV);
    }
    else if(abs(TexIndex -2)<=e)
    {
        diffuse += tex2D(s2 , UV);
    }
    else if(abs(TexIndex -3)<=e)
    {
        diffuse += tex2D(s3 , UV);
    }
    else if(abs(TexIndex-4)<=e)
    {
        diffuse += tex2D(s4 , UV);
    }
    else if(abs(TexIndex -5)<=e)
    {
        diffuse += tex2D(s5 , UV);
    }
    else if(abs(TexIndex -6)<=e)
    {
        diffuse += tex2D(s6 , UV);
    }
    else if(abs(TexIndex -7)<=e)
    {
        diffuse += tex2D(s7 , UV);
    }
  
    return diffuse;
}
////////////////////////////////////////////////////////////////////////
float4 ps_4(float4 Tex0:TEXCOORD0,
    float       TexIndex : TEXCOORD1,
    float4 incolor : color0,
	float fFog : TEXCOORD3) : color
{
    float4 diffuse = MultiColor_4(TexIndex,Tex0.xy);  
    float4 col1 = tex2D(SamplerConver , Tex0.zw);

    float4 light = PS_LightCalc(diffuse,incolor.r,incolor.g,incolor.b,col1); 
    float4 Out = diffuse * 2 * light * col1 * 1.5;
	Out = CalFogColor(Out, fFog);
    Out.a = diffuse.a;
    return Out;
}
float4 ps_8(float4 Tex0:TEXCOORD0,
    float       TexIndex : TEXCOORD1,
    float4 incolor : color0,
	float fFog : TEXCOORD3) : color
{
    float4 diffuse = MultiColor_4(TexIndex,Tex0.xy); 
    float4 col1 = tex2D(SamplerConver , Tex0.zw);
    float4 light = PS_LightCalc(diffuse,incolor.r,incolor.g,incolor.b,col1); 
    float4 Out = diffuse * 2 * light * col1 * 1.5;
	Out = CalFogColor(Out, fFog);
    Out.a = diffuse.a;
    return Out;
}
float4 ps_12(float4 Tex0:TEXCOORD0,
    float       TexIndex : TEXCOORD1,
    float4 incolor : color0,
	float fFog : TEXCOORD3) : color
{
    float4 diffuse = MultiColor_4(TexIndex,Tex0.xy);  
    float4 col1 = tex2D(SamplerConver , Tex0.zw);
    float4 light = PS_LightCalc(diffuse,incolor.r,incolor.g,incolor.b,col1); 
    float4 Out = diffuse * 2 * light * col1 * 1.5;
	Out = CalFogColor(Out, fFog);
    Out.a = diffuse.a;
    return Out;
}
/////////////////////////////////////////////////////////////////////////////////////
float4 ps_4Z(float2 Tex0:TEXCOORD0,
    float       TexIndex : TEXCOORD1,
    float       depth       : TEXCOORD2) : color
{
    float4 diffuse = MultiColor_4(TexIndex,Tex0.xy);  
    float4 Out = depth;
    Out.a = diffuse.a;
    return Out;
}
float4 ps_8Z(float2 Tex0:TEXCOORD0,
    float       TexIndex : TEXCOORD1,
    float       depth       : TEXCOORD2) : color
{
    float4 diffuse = MultiColor_4(TexIndex,Tex0.xy); 
    float4 Out = depth;
    Out.a = diffuse.a;
    return Out;
}

float4 ps_12Z(float2 Tex0:TEXCOORD0,
    float       TexIndex : TEXCOORD1,
    float       depth       : TEXCOORD2) : color
{
    float4 diffuse = MultiColor_4(TexIndex,Tex0.xy); 
    float4 Out = depth;
    Out.a = diffuse.a;
    return Out;
}
//////////////////////////////////////////////////////////////////////////
float4 ps_Channel_4(float4 Tex0:TEXCOORD0,
    float       TexIndex : TEXCOORD1,
	float3      eye      : TEXCOORD2,
	float		fFog	: TEXCOORD3,
    float4 incolor : color0
	,float4 inspecular : color1
	) : color
{
	float4 diffusebloom0 = tex2Dlod(s0, float4(Tex0.xy,0,1.5));
    float4 diffuse = MultiColor_4(TexIndex,Tex0.xy);  
    float4 col1 = tex2D(SamplerConver , Tex0.zw);
    float4 chan = tex2D(SamplerChannel,Tex0.xy);
	DetailResult dResult = CalDetail(chan,diffuse,diffuse,diffuse, Tex0.xy, SameplerEnv, eye, incolor.g);
	
	float4 light = PS_LightCalc_PointLight(dResult.comColor,incolor.rgb,inspecular.rgb); 
 	float4 noskin = light * dResult.comColor * 2 * col1 * 1.5;

    float4 lightskin = PS_LightCalc_Skin(diffuse,incolor.rgb,inspecular.rgb,dResult.skinTex,inspecular.a,1.0);
	float4 skinColor = 1.5 * lightskin*float4(0.5,0.5,0.6,1) + diffusebloom0*float4(0.53,0.51,0.50,1);
	
    float4 Out = lerp(noskin,skinColor,dResult.noSkin_SkinPercent) + dResult.speculardetail; 
   
	Out = CalFogColor(Out, fFog);
    
    Out.a = diffuse.a;
    return Out;
}


////////////////////////////////////////////////////////////////////////////
technique Tech
{
    pass p0
    {
        VertexShader = compile vs_3_0 Vertex_Shader();
        pixelshader  = compile ps_3_0 ps_4();
		FogEnable = False;	//Fog is calculated in the shader, do not use the d3d fog, it affects the final effect
    }
    pass p1
    {
        VertexShader = compile vs_3_0 Vertex_Shader();
        pixelshader  = compile ps_3_0 ps_8();
		FogEnable = False;	//Fog is calculated in the shader, do not use the d3d fog, it affects the final effect
    }
    pass p2
    {
        VertexShader = compile vs_2_0 Vertex_Shader();
        pixelshader  = compile ps_3_0 ps_12();
		FogEnable = False;	//Fog is calculated in the shader, do not use the d3d fog, it affects the final effect
    }
}

technique TechZ
{
    pass p0
    {
        VertexShader = compile vs_2_0 Vertex_Z();
        pixelshader  = compile ps_2_0 ps_4Z();
    }
    pass p1
    {
        VertexShader = compile vs_2_0 Vertex_Z();
        pixelshader  = compile ps_2_0 ps_8Z();
    }
    pass p2
    {
        VertexShader = compile vs_2_0 Vertex_Z();
        pixelshader  = compile ps_3_0 ps_12Z();
    }
}

technique Tech_Channel
{
    pass p0
    {
        VertexShader = compile vs_3_0 Vertex_Shader();
        pixelshader  = compile ps_3_0 ps_Channel_4();
		FogEnable = False;	//Fog is calculated in the shader, do not use the d3d fog, it affects the final effect
    }
}
