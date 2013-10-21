
sampler s0 : register(s0);

float4 vUVoffset1;
float4 vUVoffset2;

struct VS_OUTPUT
{
    float2 texcoord : TEXCOORD0;
};

struct PS_OUTPUT
{
    float4 color : COLOR;
};

PS_OUTPUT PS_Main(VS_OUTPUT psin)
{
    PS_OUTPUT psout;
    float4 outcolor = tex2D(s0 ,psin.texcoord);
    float4 o1 = tex2D(s0 ,psin.texcoord+vUVoffset1.xy);
    float4 o2 = tex2D(s0 ,psin.texcoord-vUVoffset1.xy);
    float4 o3 = tex2D(s0 ,psin.texcoord+vUVoffset1.zw);
    float4 o4 = tex2D(s0 ,psin.texcoord-vUVoffset1.zw);
    float4 o5 = tex2D(s0 ,psin.texcoord+vUVoffset2.xy);
    float4 o6 = tex2D(s0 ,psin.texcoord-vUVoffset2.xy);
    float4 o7 = tex2D(s0 ,psin.texcoord+vUVoffset2.zw);
    float4 o8 = tex2D(s0 ,psin.texcoord-vUVoffset2.zw);
    float4 S = 0.125*(o1+o2+o3+o4+o5+o6+o7+o8);
    psout.color = (outcolor+S)*0.5;
    psout.color.a = 1;
    return psout;
}

technique tec0
{
    pass Pass0
    {
        VertexShader = NULL;
        PixelShader = compile ps_2_0 PS_Main();
    }
}


