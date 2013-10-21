
////////////////////////////////////////////////////////////////////////////////
//
//  Author      : ZhaoXinyu (Chiyer)
//  CopyRight   : Kingsoft Corporation All rights Reserved
//  Create Date : 2009-05-18 14:53:41
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

sampler s0 : register(s0);
sampler s1 : register(s1);
sampler s2 : register(s2);
sampler s3 : register(s3);
sampler s4 : register(s4);        
sampler s5 : register(s5); 
sampler s6 : register(s6);
sampler s7 : register(s7);

#define box_size        50.f

#define BOX_PROJ_Y      0
#define BOX_PROJ_X      1
#define BOX_PROJ_DURM   2

float4x4 g_matWorldViewProj;
float4   g_Color = float4(1.f, 1.f, 1.f, 1.f);
float4   g_UV = float4(0.f, 0.f, 1.f, 1.f);
float4   g_UV_range = float4(0.f, 0.f, 1.f, 1.f);

struct vs_out
{
    float4 hpos : POSITION;
    float3 pos  : TEXCOORD1;
};

vs_out box_proj_vs(float3 pos : POSITION)
{
    vs_out o = (vs_out)0;
    
    o.hpos = mul(float4(pos, 1.f), g_matWorldViewProj);
    o.pos = pos;

    return o;
}

float4 box_proj_ps(vs_out psIn, uniform int proj_type) : COLOR
{
    float4 color = 0;

    if (psIn.pos.x > -box_size && psIn.pos.x < box_size &&
        psIn.pos.y > -box_size && psIn.pos.y < box_size &&
        psIn.pos.z > -box_size && psIn.pos.z < box_size)
    {
        float2 tex0;
        
        if (proj_type == BOX_PROJ_Y)
        {
            tex0.x =  0.5f * psIn.pos.x / box_size / g_UV.z + 0.5f - g_UV.x;
            tex0.y = -0.5f * psIn.pos.z / box_size / g_UV.w + 0.5f - g_UV.y;
        }
        else if (proj_type == BOX_PROJ_X)
        {
            tex0.x =  0.5f * psIn.pos.z / box_size / g_UV.z + 0.5f - g_UV.x;
            tex0.y = -0.5f * psIn.pos.y / box_size / g_UV.w + 0.5f - g_UV.y;
        }
        else
        {
            float2 xy = normalize(psIn.pos.xy); 
            float  an = acos(dot(xy, float2(0.f, -1.f)));
            float2 f = xy - float2(0.f, -1.f);

            if (f.x > 0)
                an = 6.283185308f - an;

            tex0.x = an / 6.283185308f;
            tex0.y = abs(psIn.pos.z - box_size) / box_size * 2;
        }

        color = tex2D(s0, tex0) * g_Color;
    }
    else
    {
        clip(-1.f); 
    }

    return color;
}

technique tec0
{
    pass p0
    {
        VertexShader = compile vs_2_0 box_proj_vs();
        PixelShader  = compile ps_2_0 box_proj_ps(BOX_PROJ_Y);

        AddressU[0] = BORDER;
        AddressV[0] = BORDER;
        AddressU[1] = BORDER;
        AddressV[1] = BORDER;
    }

    pass p1
    {
        VertexShader = compile vs_2_0 box_proj_vs();
        PixelShader  = compile ps_2_0 box_proj_ps(BOX_PROJ_X);

        AddressU[0] = BORDER;
        AddressV[0] = BORDER;
        AddressU[1] = BORDER;
        AddressV[1] = BORDER;
    }

    pass p2
    {
        VertexShader = compile vs_2_0 box_proj_vs();
        PixelShader  = compile ps_2_0 box_proj_ps(BOX_PROJ_DURM);

        AddressU[0] = BORDER;
        AddressV[0] = BORDER;
        AddressU[1] = BORDER;
        AddressV[1] = BORDER;
    }
}
