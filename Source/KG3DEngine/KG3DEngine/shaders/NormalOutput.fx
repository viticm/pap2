
sampler s0 : register(s0);

float4x4 WorldViewProj; 

struct VS_INPUT
{
    float3  pos:POSITION;
        float3  nor:NORMAL;
    float2 texcoord : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4    pos : POSITION;
    float2 texcoord : TEXCOORD0;
    float3 normal     : TEXCOORD1;
};

struct PS_OUTPUT
{
    float4 color : COLOR;
};

VS_OUTPUT VS_Main(VS_INPUT In)
{
     VS_OUTPUT Out;

    Out.pos = mul(float4(In.pos,1),WorldViewProj);

    Out.normal = In.nor;
    Out.texcoord = In.texcoord;
    return Out;
}

PS_OUTPUT PS_Main(VS_OUTPUT psin)
{
    PS_OUTPUT psout;
    float4 outcolor;
    psout.color.rgb = psin.normal*0.5+0.5;
    psout.color.a = 1;
    return psout;
}

technique tec0
{
    pass Pass0
    {
        VertexShader = compile vs_2_0 VS_Main();
        PixelShader = compile ps_2_0 PS_Main();
    }
}


