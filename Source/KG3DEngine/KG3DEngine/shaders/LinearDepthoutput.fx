
sampler s0 : register(s0);

float4 vUVoffset1;
float4 vUVoffset2;

float4x4 invProj; 

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
    float depth = tex2D(s0 ,psin.texcoord).r;
    float3 pos;
    pos.x = psin.texcoord.x * 2 -1;
    pos.y =  -psin.texcoord.y * 2 +1;
    pos.z = depth;

    float4 pos_View = mul(float4(pos ,1),invProj);
    pos_View.xyz /= pos_View.w;
    float L = length(pos_View.xyz);
    psout.color.rgb = L;
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


