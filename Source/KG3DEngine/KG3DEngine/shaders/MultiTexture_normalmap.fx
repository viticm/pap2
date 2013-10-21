//-------------------------------------------
// Desc  : Speed Tree Shader
// Author: Zeb
// Data  : 2006-6-27
//-------------------------------------------
#include "CommonParams.h"
#include "PointLight.h"

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
sampler s9 : register(s9);
sampler s10 : register(s10);
sampler s11 : register(s11);
sampler s12 : register(s12);
sampler s13 : register(s13);
int nTextureStart = 0;
float HeightBase = 0;

float   Alpha = 1;
float e = 0.3;

//////////////////////////////////////////////////////////
// project texture 
texture SelTexture;
sampler2D SamplerSelectTexture = sampler_state
{   
    Texture   = <SelTexture>;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    AddressU = BORDER;
    AddressV = BORDER;
    BorderColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
};
#define Height_Limit 100
float  proj_size;
float3 proj_center;
float2 calc_proj_uv(float3 pos)
{
    float3 pos_wrd = mul(float4(pos, 1),(float4x4)mWorld);
    
    if (abs(pos_wrd.y - proj_center.y) > Height_Limit)
    {
        return float2(10000.f, 10000.f);
    }
    else
    {
        float3 proj_space = (pos_wrd - proj_center + float3(proj_size / 2.f, 0.f, proj_size / 2.f));
        return float2(proj_space.x / proj_size, proj_space.z / proj_size);
    }
}

//////////////////////////////////////////////////////////
struct VS_OUTPUT
{
    float4 Pos          : POSITION;
    float4 Color        : COLOR0;
	float4 Specular     : COLOR1;
    float4 TexCoord     : TEXCOORD0;
    float  TexIndex     : TEXCOORD1;
    float2 textcSelect  : TEXCOORD2;
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
    float3 UVW      : TEXCOORD0,
	float4 tangent  : TANGENT)
{
    VS_OUTPUT Out = (VS_OUTPUT)0;
    
    float3 wpos = mul(float4(Pos, 1), mWorld).xyz;
    Out.TexCoord.xy = UVW.xy;
    Out.TexCoord.zw = ConverUV(wpos.x,wpos.z); 
    Out.TexIndex    = UVW.z - nTextureStart;

    Out.Pos = mul(float4(wpos, 1), mViewProj);
        
    float3 n = normalize(mul(Normal, mWorld).xyz);
    float s = dot(n, -sunLight.m_dir);
    float i = max(0, s);
    Out.Color.rgb = n;//LightDiffuse * i + LightAmbient;
    Out.Color.a = Alpha; 
	Out.Specular = float4(wpos,1);

    float3 eye     = normalize(eyes - wpos); 
    float3   HalfV = normalize(eye - sunLight.m_dir );
    float ref = max(0, dot(HalfV, n));

    Out.fFog = CalFogFactor(Out.Pos.z);;
        
    Out.textcSelect = calc_proj_uv(Pos);
	
	float3 binormal = normalize(cross(n,tangent));
	half3x3 objTangentXf;
    objTangentXf[0] = binormal;
    objTangentXf[1] = tangent;
    objTangentXf[2] = n;
	
	Out.Color.rgb = mul(objTangentXf, -sunLight.m_dir );
	Out.Specular.rgb  = mul(objTangentXf,  eye );
	
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

float4 PS_LightCalc_PointLight(float4 Tex,float3 normal,float3 pos)
{
    float L = (Tex.r + Tex.g + Tex.b) * 0.3333;
	float k = max(0,dot(normal,-sunLight.m_dir));
	float side = max(0,dot(normal,-sky_light_dir));  
	
	float4 light = side*sunLight.m_sky_light + lerp(sunLight.m_sceneAmbient,sunLight.m_sceneAmbient+sunLight.m_diffuse,k*L); 

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

float4 MultiNormal_4(float TexIndex,float2 UV)
{
     float4 normal = 0;
    if(abs(TexIndex )<=e)
    {
        normal += tex2D(s4, UV);
    }
    else if(abs(TexIndex-1)<=e)
    {
        normal += tex2D(s5 , UV);
    }
    else if(abs(TexIndex -2)<=e)
    {
        normal += tex2D(s6 , UV);
    }
    else if(abs(TexIndex -3)<=e)
    {
        normal += tex2D(s7 , UV);
    }
    return normal;
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
    else if(abs(TexIndex-8)<=e)
    {
        diffuse += tex2D(s8 , UV);
    }
    else if(abs(TexIndex -9)<=e)
    {
        diffuse += tex2D(s9 , UV);
    }
    else if(abs(TexIndex -10)<=e)
    {
        diffuse += tex2D(s10 , UV);
    }
    else if(abs(TexIndex -11)<=e)
    {
        diffuse += tex2D(s11 , UV);
    }
    return diffuse;
}

float4 light_default(float3 light,float3 normal,float3 view,float specularfactor)
{
   float  k = max(0,dot(normal,light));
   float  s = max(0,dot(normal,view));
   float3 R =  normalize(2 *s* normal + light);
   float4 sp = sunLight.m_specular * pow(max(0, dot(R, view)), materialEx.spe_exp) * materialCur.specPower * specularfactor * 0.1; 
   return sunLight.m_sceneAmbient+sunLight.m_diffuse*k*2+sp;
}

////////////////////////////////////////////////////////////////////////
float4 ps_4(float4 Tex0:TEXCOORD0,
    float  TexIndex : TEXCOORD1,
    float4 incolor : color0,
	float4 specular: color1,
	float fFog : TEXCOORD3) : color
{
    float4 diffuse = MultiColor_4(TexIndex,Tex0.xy); 
	float4 normal  = MultiNormal_4(TexIndex,Tex0.xy);
	
    float4 col1 = tex2D(SamplerConver , Tex0.zw);

    float4 Out = diffuse * light_default(incolor.rgb,normal.rgb*2-1,specular.rgb,normal.a);
	Out = CalFogColor(Out, fFog);
    Out.a = diffuse.a;
    return Out;
}

float4 ps_8(float4 Tex0:TEXCOORD0,
    float       TexIndex : TEXCOORD1,
    float4 incolor : color0,
	float4 inspecular : color1,
    float2 textcSelect : TEXCOORD2,
	float fFog : TEXCOORD3) : color
{
    float4 diffuse = MultiColor_8(TexIndex,Tex0.xy); 
    float4 col1 = tex2D(SamplerConver , Tex0.zw);
    float4 light = PS_LightCalc_PointLight(diffuse,incolor.rgb,inspecular.rgb); 
	float4 Out = diffuse * 2 * light * col1 * 1.5;
	// project texture 
	if (textcSelect.x >= 0 && textcSelect.x < 1 && textcSelect.y >= 0 && textcSelect.y < 1)
	{
		float4 aoe = tex2D(SamplerSelectTexture, textcSelect.xy);
		Out = Out * (1.f - aoe.a) + aoe * aoe.a; 
	}
	Out = CalFogColor(Out, fFog);
	Out.a = diffuse.a;
    return Out;
}

float4 ps_12(float4 Tex0:TEXCOORD0,
    float       TexIndex : TEXCOORD1,
    float4 incolor : color0,
	float4 inspecular : color1,
    float2 textcSelect : TEXCOORD2,
	float fFog : TEXCOORD3) : color
{
    float4 diffuse = MultiColor_12(TexIndex,Tex0.xy);  
    float4 col1 = tex2D(SamplerConver , Tex0.zw);
    float4 light = PS_LightCalc_PointLight(diffuse,incolor.rgb,inspecular.rgb); 
    float4 Out = diffuse * 2 * light * col1 * 1.5;
	// project texture 
	if (textcSelect.x >= 0 && textcSelect.x < 1 && textcSelect.y >= 0 && textcSelect.y < 1)
	{
		float4 aoe = tex2D(SamplerSelectTexture, textcSelect.xy);
		Out = Out * (1.f - aoe.a) + aoe * aoe.a; 
	}
	Out = CalFogColor(Out, fFog);
	Out.a = diffuse.a;
	return Out;
}

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
    float4 diffuse = MultiColor_8(TexIndex,Tex0.xy); 
    float4 Out = depth;
    Out.a = diffuse.a;
    return Out;
}

float4 ps_12Z(float2 Tex0:TEXCOORD0,
    float       TexIndex : TEXCOORD1,
    float       depth       : TEXCOORD2) : color
{
    float4 diffuse = MultiColor_12(TexIndex,Tex0.xy); 
    float4 Out = depth;
    Out.a = diffuse.a;
    return Out;
}

technique Tech
{
    pass p0
    {
        VertexShader = compile vs_2_0 Vertex_Shader();
        pixelshader  = compile ps_3_0 ps_4();
		FogEnable = False;	//Fog is calculated in the shader, do not use the d3d fog, it affects the final effect
    }
    pass p1
    {
        VertexShader = compile vs_2_0 Vertex_Shader();
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
