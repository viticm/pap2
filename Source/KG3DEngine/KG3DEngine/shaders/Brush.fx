struct VS_INPUT
{
    float3 pos : POSITION;
    float2 texcoord : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 pos : POSITION;
    float2 texcoord : TEXCOORD0;
    float4 projpos : TEXCOORD1;
};

struct PS_OUTPUT
{
    float4 color : COLOR;
};

float WeightPower = 0.1f;
float4x4 worldViewProj,worldView,view;
texture texCpoy,texBlend;

sampler samplerCopyTex= sampler_state
{
    Texture   = (texCpoy);
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};

sampler samplerBlendTex= sampler_state
{
    Texture   = (texBlend);
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};

VS_OUTPUT vs(VS_INPUT In1)
{
    VS_OUTPUT out1;

    out1.pos = mul(float4(In1.pos,1.0) ,worldViewProj);
    out1.texcoord = In1.texcoord;
    out1.projpos = out1.pos;
    
    return out1;
}

PS_OUTPUT PS_AddWeight(VS_OUTPUT psin)
{
    PS_OUTPUT psout;
    float4 outcolor;
    float2 copytexcd;
    float weight,v;
    
    weight = tex2D(samplerBlendTex,psin.texcoord).x * WeightPower;
    copytexcd = float2(psin.projpos.x / psin.projpos.w * 0.5f + 0.5f,0.5f - psin.projpos.y / psin.projpos.w * 0.5f);
    outcolor = tex2D(samplerCopyTex,copytexcd);
    v = min(outcolor.x + weight,1.0f);
    outcolor.x = v;
    
    psout.color = outcolor;
    
    return psout;
}

technique AddWeight
{
    pass Pass0
    {
        Lighting = FALSE;
    
        VertexShader = compile vs_1_1 vs();
        PixelShader = compile ps_2_0 PS_AddWeight();
    }
}


