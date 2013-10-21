sampler s0 : register(s0);

float4 gray(in float2 v : TEXCOORD0) : COLOR
{
    float4 c = tex2D(s0, v);
    float gray = (c.r * 0.299 + c.g * 0.587 + c.b * 0.144);
    return float4(gray, gray, gray, c.a);
}

technique tec0
{
    pass p0
    {
        PixelShader  = compile ps_2_0 gray();
    }
}
