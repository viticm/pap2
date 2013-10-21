
#include "CommonParams.h"
#include "PointLight.h"
#include "ShadowMap.h"

float4x4 WorldViewProj; 
float4x4 matWorld;

sampler2D samp0 : register(s0);
sampler2D samp1 : register(s1);

void vsColor(in   float3 iPos: POSITION
            , out float4 oPos : POSITION
            , in  float2 iTexCoord : TEXCOORD0
            , out float3 oTexCoord : TEXCOORD0)
{
    float4 PosTemp = mul(float4(iPos,1), WorldViewProj);
    oPos = PosTemp;
    oTexCoord.xy = iTexCoord;
    oTexCoord.z = CalFogFactor(PosTemp.z);
}

float4 psColor(float3 iTexCoord : TEXCOORD0) : COLOR
{
    float4 f = tex2D(samp0, iTexCoord);
    float3 nol = 1 - tex2D(samp1, iTexCoord).rgb * 2;

    float comlight = max(0, dot(sunLight.m_dir, nol));
    float light    = max(0, dot(-sunLight.m_dir, nol));
    float skylight = max(0, dot(-sky_light_dir, nol));      

    float4 diff = (light * sunLight.m_diffuse + comlight * sunLight.m_com_light + skylight * sunLight.m_sky_light)+
        sunLight.m_sceneAmbient;  

    float4 o = diff*f*2;
    o.a = f.a;
    CalFogColor(o, iTexCoord .z);
    return o;
}

technique tec0
{
    pass p0
    {
        MinFilter[0] = LINEAR;
        MagFilter[0] = LINEAR;
        MipFilter[0] = LINEAR;

        MinFilter[1] = LINEAR;
        MagFilter[1] = LINEAR;
        MipFilter[1] = LINEAR;

        VertexShader = compile vs_2_0 vsColor();
        PixelShader = compile ps_2_0 psColor();        
        ZEnable = TRUE;
        ZFunc = LessEqual;
        AlphaBlendEnable = FALSE;
        AlphaFunc = Greater;
        AlphaTestEnable = TRUE;
        CullMode = None;
    }
}
