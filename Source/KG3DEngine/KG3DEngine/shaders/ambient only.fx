#include "CommonParams.h"


// transformations
float4x4 View       :         VIEW;
float4x4 WorldViewProj :     WORLDVIEWPROJECTION;
float4x4 WorldView :         WORLDVIEW;

texture colorTexture : DiffuseMap
<
    string UIName = "Diffuse";
>;

texture ambientMap: AmbientMap
<
    string UIName = "Ambient";
>;

texture specMap: specularmap
<
    string UIName = "Specular";
>;
sampler2D colorTextureSampler : register(s0) = sampler_state
{
    Texture = <colorTexture>;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
};

sampler2D ambientTextureSampler : register(s1) = sampler_state
{
    Texture = <ambientMap>;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
};

struct VS_OUTPUT
{
    float4 Pos  : POSITION;
    float2 Tex : TEXCOORD0;
    float2 Tex1: TEXCOORD1;
    float4 col : COLOR0;
    float fFog : TEXCOORD2;
};

VS_OUTPUT VS(
    float3 Pos  : POSITION, 
    float3 Norm : NORMAL, 
    float2 Tex  : TEXCOORD0,
    float2 Tex1 : TEXCOORD1)
{
    VS_OUTPUT Out = (VS_OUTPUT)0;

    //float3 L = normalize(lightDir);
    float3 L = normalize(mul(-sunLight.m_dir,  (float3x3)View));

    float3 P = mul(float4(Pos, 1),(float4x4)WorldView);  // position (view space)
    float3 N = normalize(mul(Norm,(float3x3)WorldView)); // normal (view space)
    
    float3 R = normalize(2 * dot(N, L) * N - L);          // reflection vector (view space)
    float3 V = normalize(P);                             // view direction (view space)

    Out.Pos  = mul(float4(Pos,1),WorldViewProj);    // position (projected)
    
    float4 Diff = max(0, dot(N, L));//I_d * k_d * max(0, dot(N, L)); // diffuse 
	float shinness = materialCur.specPower;
    float4 Spec = sunLight.m_specular * materialCur.spec * pow(max(0, dot(R, -V)), shinness/4);   // specular
    Out.Tex = Tex;
    Out.Tex1 = Tex1;
    Out.col = Diff + Spec;
    Out.col.a = 1.0f;
        
    Out.fFog = CalFogFactor(Out.Pos.z);
    
    return Out;
}



float4 PS(
    float4 Diff : COLOR0,
    float2 Tex  : TEXCOORD0,
    float2 Tex1 : TEXCOORD1,
	float fFog : TEXCOORD2,
    float4 Spec : COLOR1
     ) : COLOR
{
    float4 ambientMap = tex2D(ambientTextureSampler, Tex1.xy);
	ambientMap = CalFogColor(ambientMap, fFog);
    return ambientMap;
}

technique DefaultTechnique
{
    pass P0
    {
        // shaders
        VertexShader = compile vs_1_1 VS();
        PixelShader  = compile ps_2_0 PS();
        //alphablendenable = false;
        //alphatestenable = false;

		FogEnable = False;	//Fog is calculated in the shader, do not use the d3d fog, it affects the final effect
    }  
}


