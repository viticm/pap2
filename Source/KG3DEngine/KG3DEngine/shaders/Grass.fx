//-------------------------------------------
// Desc  : Speed Tree Shader
// Author: Zeb
// Data  : 2006-6-27
//-------------------------------------------
#include "CommonParams.h"
float4x4 mViewProj;

float3 vCameraLeft; 
float3 vCameraUp;
float  Scale = 1.0f;


float4  LightAmbient = {0.4f, 0.4f, 0.4f, 0.4f};
float4  LightDiffuse = {1.0f, 1.0f, 1.0f, 1.0f};
float3  LightDirection = {0.577, 0.577, 0.577};
float   Alpha = 1;
float   Time = 0;
float   cloudscale = 0.0001F;
float2  vCloudSpeed = 0;

sampler s0 : register(s0);
sampler s1 : register(s1);

struct VS_OUTPUT
{
    float4 Pos          : POSITION;
    float4 Color        : COLOR0;
    float2 TexCoord     : TEXCOORD0;
    float2 TexCoord2    : TEXCOORD1;
	float fFog : TEXCOORD2;
};


VS_OUTPUT Leaf_Vertex_Shader(
    float3 Pos      : POSITION, 
    float3 Normal   : NORMAL,
    half4  DColor   : COLOR0,
    float2 UV   : TEXCOORD0,
    float2 UV2   : TEXCOORD1)
{
    VS_OUTPUT Out = (VS_OUTPUT)0;
    
    float3 vPosNew = Pos;// + 7 * sin(Time+Pos*0.01F) * (1- UV2.y) ;
    Out.Pos = mul(float4(vPosNew , 1), mViewProj);
        
    float3 n = Normal;//normalize(mul(Normal, mWorld).xyz);
    float i = max(0, dot(n, -LightDirection));
    Out.Color = LightDiffuse * i * DColor + LightAmbient ;
    Out.Color.a = Alpha;
    
    Out.TexCoord = UV ; 
    Out.TexCoord2= Pos.xz * cloudscale + vCloudSpeed ;
 
    Out.fFog = CalFogFactor(Out.Pos.z);

    return Out;
}

float4 ps(float2 tex0: texcoord0,
    float2 tex1: texcoord1,
	float fFog : TEXCOORD2,
    float4 inColor : color) : color
{
    float4 col0 = tex2D(s0, tex0) * 2;
    float4 fTemp = inColor * col0;
	fTemp = CalFogColor(fTemp, fFog);
	return fTemp;
}
 
technique Tech
{
    pass p0
    {
        VertexShader = compile vs_1_1 Leaf_Vertex_Shader();
        PixelShader = compile ps_2_0 ps();
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

        ColorOp[0] = Modulate2x;
        ColorArg1[0] = Diffuse;
        ColorArg2[0] = Texture;
        
        AlphaOp[0] = SelectArg1;
        AlphaArg1[0] = Texture;
        AlphaArg2[0] = Current;

        MinFilter[0] = Linear;
        MagFilter[0] = Linear;
        MipFilter[0] = Linear;

        ColorOp[1] = Modulate2x;
        ColorArg1[1] = Texture;
        ColorArg2[1] = Current;

        AlphaOp[1] = Disable;
        
        TextureTransformFlags[0] = Disable;
    }
}
