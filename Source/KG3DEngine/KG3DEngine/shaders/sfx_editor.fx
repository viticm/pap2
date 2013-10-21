////////////////////////////////////////////////////////////////////////////////
//
//  Author      : ZhaoXinyu (Chiyer)
//  CopyRight   : Kingsoft Corporation All rights Reserved
//  Create Date : 
//  Comment     : sfx editor shader
//
////////////////////////////////////////////////////////////////////////////////

sampler s0 : register(s0);
sampler s1 : register(s1);
    
texture SelTexture;
texture ColTexture;

sampler2D seltex = sampler_state
{
    Texture = <SelTexture>;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    AddressU = BORDER;
    AddressV = BORDER;
    BorderColor = 0;
};


sampler2D coltex = sampler_state
{
    Texture = <ColTexture>;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
};

float4x4 WorldViewProj; 

#define Height_Limit 1000
float   proj_size;
float3  proj_center;

struct vs_out 
{
    float4 hpos : POSITION;
    float4 diff : COLOR0;
    float2 tex0 : TEXCOORD0;
    float2 tex1 : TEXCOORD1;
};

vs_out vs(in float4 color : COLOR0, in float3 pos : POSITION, in float2 tex0 : TEXCOORD0)
{
    vs_out o = (vs_out)0;
    
    o.hpos = mul(float4(pos, 1.f), WorldViewProj);
    o.diff = color;
    o.tex0 = tex0;
    float3 proj_space = (pos - proj_center + float3(proj_size / 2.f, 0.f, proj_size / 2.f));
    o.tex1 = float2(proj_space.x / proj_size, proj_space.z / proj_size);
     
    return o;
} 

float4 ps(in float4 diff : COLOR0, in float2 tex0 : TEXCOORD0, in float2 tex1 : TEXCOORD1) : COLOR0
{ 
    float4 color = tex2D(coltex, tex0);
    float4 aoe = tex2D(seltex, tex1);
    return color * (1.f - aoe.a) + aoe * aoe.a;
}

technique tec
{
    pass p0
    {
        VertexShader = compile vs_2_0 vs();
        PixelShader  = compile ps_2_0 ps();
        
        Lighting = false;
    }
}