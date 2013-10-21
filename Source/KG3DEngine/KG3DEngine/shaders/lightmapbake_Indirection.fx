
#ifndef NORMAL_CHECK
#define NORMAL_CHECK 0
#endif
//#ifndef SHADER_MODEL
#define SHADER_MODEL 3
//#endif

#include "CommonParams.h"
#include "PointLight.h"
#include "ShadowMap.h"

float4x4 WorldViewProj; 
float4x4 matWorld;


float4 szPhotonsPosition[55];
float4 szPhotonsColor[55];
float4 szPhotonsDirect[55];
int nNumPhoton = 55;
float  fPhotonScale = 1.0;
float   fUVOffset = 0;

sampler s0 : register(s0);
sampler s1 : register(s1);
sampler s2 : register(s2);
sampler s3 : register(s3);    

struct vs_in
{
    float4 pos     : POSITION;
    float3 nor     : NORMAL;
    float2 tex0    : TEXCOORD0;
    float2 tex1 : TEXCOORD1;
};

struct vs_out
{
        float4 hpos   : POSITION;
        float4 tex0   : TEXCOORD0;
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

    output.hpos   = float4(input.tex1*2-1 + float2(fUVOffset,0),0,1);
    output.hpos.y *= -1;
    nor           = normalize(mul(nor, matWorld));
    output.tex0.xy    = input.tex0;
    output.tex0.zw    = input.tex1;
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

float4 ps(float4 tex0   : TEXCOORD0,
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
    r = 0;

#if SHADER_MODEL==3
   float3 ret = {0,0,0};
   for(int i=0;i<nNumPhoton;i++)
       {
        float3 V = wpos - szPhotonsPosition[i].xyz;
        float dis = max(0,1-length(V) * szPhotonsPosition[i].w);
        float t = max(0,dot(nol,V));
        ret += dis * fPhotonScale * szPhotonsColor[i].rgb * szPhotonsColor[i].a *t;
      }
      r.rgb += ret;
#endif
    return r;
}

technique DefaultTechnique
{
    pass p0
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
        FogEnable = False;    //Fog is calculated in the shader, do not use the d3d fog, it affects the final effect
        CullMode = NONE;
        AlphaTestEnable = False;
        AlphaBlendEnable = True;
        SrcBlend = ONE;
        DestBlend = ONE;
    }
}
