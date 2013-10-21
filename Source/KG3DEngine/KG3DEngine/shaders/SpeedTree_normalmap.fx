//-------------------------------------------
// Desc  : Speed Tree Shader
// Author: Zeb
// Data  : 2006-6-27
//-------------------------------------------
#include "CommonParams.h"

float4x4 mWorld;
float4x4 mViewProj;

float3 vCameraLeft; 
float3 vCameraUp;
float  Scale = 0.5f;
float Time = 0;
bool bAnimation = true;

sampler s0 : register(s0);
sampler s1 : register(s1);

float   Alpha = 1;

/////////////////////////////////////////////////////////
texture tNormal;
sampler2D SamplerNormal = sampler_state
{
    Texture   = <tNormal>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    ADDRESSU = WRAP;
    ADDRESSV = WRAP;
};

struct VS_OUTPUT
{
    float4 Pos          : POSITION;
    float4 Color        : COLOR0;
	float4 specular     : COLOR1;
    float4 TexCoord     : TEXCOORD0;
    float  fFog       : TEXCOORD1;
};
struct VS_OUTPUT_Z
{
    float4 Pos          : POSITION;
    float2 TexCoord     : TEXCOORD0;
    float2  depth         : TEXCOORD1;
};

VS_OUTPUT Leaf_Vertex_Shader(
    float3 Pos      : POSITION, 
    float3 Normal   : NORMAL,
    float2 Offset   : TEXCOORD0,
    float3 UV        : TEXCOORD1)
{
    VS_OUTPUT Out = (VS_OUTPUT)0;
    
    float3 wpos = mul(float4(Pos, 1), mWorld).xyz;
    float2 FF = Offset;
    if (bAnimation)
    {
    	FF += sin(Time + wpos.xz * 0.01F)*5 / Scale;
    }
    wpos = wpos + vCameraLeft * FF.x * Scale + vCameraUp * FF.y * Scale;
    Out.TexCoord.xy = UV.xy;
    Out.TexCoord.zw = ConverUV(wpos.x,wpos.z); 

    Out.Pos = mul(float4(wpos, 1), mViewProj);
        
    float3 n = normalize(mul(Pos, mWorld).xyz);
    float i = max(0, dot(n, -sunLight.m_dir));
    Out.Color.rgb = sunLight.m_diffuse * i + sunLight.m_sceneAmbient;
    Out.Color.a = Alpha;    
 
    Out.fFog = CalFogFactor(Out.Pos.z);

    float3 tangent = normalize(wpos);
	float3 binormal = normalize(cross(n,tangent));
	half3x3 objTangentXf;
    objTangentXf[0] = binormal;
    objTangentXf[1] = tangent;
    objTangentXf[2] = n;
	Out.Color.rgb = mul(objTangentXf, -sunLight.m_dir );
	Out.specular.rgb = mul(objTangentXf,normalize(eyes - wpos));
	
    return Out;
}


VS_OUTPUT_Z Leaf_Vertex_Shader_Z(
    float3 Pos      : POSITION, 
    float3 Normal   : NORMAL,
    float2 Offset   : TEXCOORD0,
    float3 UV        : TEXCOORD1)
{
    VS_OUTPUT_Z Out = (VS_OUTPUT_Z)0;
    
    float3 wpos = mul(float4(Pos, 1), mWorld).xyz;
    float2 FF = Offset;
    if (bAnimation)
    {
    	FF += sin(Time + wpos.xz * 0.01F)*5 / Scale;
    }
    wpos = wpos + vCameraLeft * FF.x * Scale + vCameraUp * FF.y * Scale;
    Out.TexCoord.xy = UV.xy;
    Out.Pos = mul(float4(wpos, 1), mViewProj);
    Out.depth.x = Out.Pos.z;
    Out.depth.y = Out.Pos.w;

    return Out;
}

float4 light_default(float3 light,float3 normal,float3 view,float specularfactor)
{
   float  k = max(0,dot(normal,light));
   float3 H =  normalize(view + light);
   float4 sp = sunLight.m_specular * pow(max(0, dot(H, normal)), materialEx.spe_exp) * materialCur.specPower * specularfactor; 
   return sunLight.m_sceneAmbient + sunLight.m_diffuse * k + sp;
}

float4 ps(float4 Tex0:TEXCOORD0,
    float4 incolor : color0,
	float4 specular: color1,
	float fFog : TEXCOORD1) : color
{
	float4 diffuse = tex2D(s0, Tex0.xy);
    float4 N = tex2D(SamplerNormal,Tex0.xy);
	float3 normal = N.rgb*2-1;
	float4 light = light_default(incolor.rgb,normal,specular.rgb,N.a);
    float4 col1 = tex2D(SamplerConver , Tex0.zw);
    float4 colorOut = light*diffuse * 2 * col1 * 1.5 ;
	colorOut = CalFogColor(colorOut, fFog);
	return colorOut;
}

float4 ps_Z(float2 Tex0:TEXCOORD0,
    float2 depth : TEXCOORD1) : color
{
      
    float4 color = tex2D(s0, Tex0.xy) * 3;
    color.rgb = PackingFloatToRGB(depth.x / depth.y);
    return color;
}

technique Tech
{
    pass p0
    {
        VertexShader = compile vs_1_1 Leaf_Vertex_Shader();
        pixelshader = compile  ps_2_0 ps();
        CullMode        = None;                                    
        ZEnable            = True;
        
        Lighting        = True;
        SpecularEnable = False;

        AlphaBlendEnable = False;
        SrcBlend        = SrcAlpha;          
        DestBlend       = InvSrcAlpha;
        AlphaTestEnable = True;
        AlphaFunc = GREATEREQUAL;
        AlphaRef        = 0x90;

        ColorArg1[0] = TEXTURE;
        ColorArg2[0] = DIFFUSE;
        ColorOp[0] = MODULATE2X;
        
        AlphaArg1[0] = TEXTURE;
        AlphaArg2[0] = CURRENT;
        AlphaOp[0] = SELECTARG1;

		FogEnable = False;	//Fog is calculated in the shader, do not use the d3d fog, it affects the final effect
    }
    pass p1//RENDER FOR DEPTH ONLY
    {
        VertexShader = compile vs_1_1 Leaf_Vertex_Shader_Z();
        pixelshader = compile  ps_2_0 ps_Z();
        FogEnable     = FALSE;
        CullMode    = None;                                    
        ZEnable     = True;
        ZFunc = LESSEQUAL;
        ZWriteEnable = True;
        ColorWriteEnable = RED;

        AlphaBlendEnable = False;
        SrcBlend        = SrcAlpha;          
        DestBlend       = InvSrcAlpha;
        
        AlphaTestEnable = True;
        AlphaFunc = GREATEREQUAL;
        AlphaRef        = 0xF0;
    }
    
    pass p2//Render color only
    {
        VertexShader = compile vs_1_1 Leaf_Vertex_Shader();
        pixelshader = compile  ps_2_0 ps();
        CullMode    = None;                                    
        ZEnable     = True;
        ZWriteEnable = False;
        ZFunc = EQUAL;

        AlphaBlendEnable = False;
        AlphaTestEnable = False;
        AlphaFunc = GREATEREQUAL;
        AlphaRef        = 0x70;

        ColorArg1[0] = TEXTURE;
        ColorArg2[0] = DIFFUSE;
        ColorOp[0] = MODULATE2X ;
        
        AlphaArg1[0] = TEXTURE;
        AlphaArg2[0] = CURRENT;
        AlphaOp[0]   = SELECTARG1;

		FogEnable = False;	//Fog is calculated in the shader, do not use the d3d fog, it affects the final effect
    }
    pass p3//Render color and alpha blend only
    {
        VertexShader = compile vs_1_1 Leaf_Vertex_Shader();
        pixelshader = compile  ps_2_0 ps();

        ColorArg1[0] = TEXTURE;
        ColorArg2[0] = DIFFUSE;
        ColorOp[0] = MODULATE2X ;

        AlphaBlendEnable = False;
        AlphaTestEnable = False;

        AlphaArg1[0] = TEXTURE;
        AlphaArg2[0] = CURRENT;
        AlphaOp[0]   = SELECTARG1;

		FogEnable = False;	//Fog is calculated in the shader, do not use the d3d fog, it affects the final effect
    }
}
