float4 Color = float4(1,1,1,1);
float4x4 World;
float4x4 ViewProj;

void vs(float3 iPos : POSITION
        ,out float4 oPos : POSITION)
{
    float4 Temp = mul(float4(iPos, 1),World);
    oPos = mul(Temp, ViewProj);
}

float4 ps() : COLOR
{
    return Color;
}

technique DefaultTech
{
    pass p0
    {
        VertexShader = compile vs_1_1 vs();
        PixelShader = compile ps_2_0 ps();

        AlphaBlendEnable = True;
        SrcBlend = SrcAlpha;
        DestBlend = InvSrcAlpha;

        ZEnable = False;
        ZWriteEnable = False;
        FogEnable = False;
        FillMode = Solid;
        CullMode = None;
        ColorWriteEnable = Red|Green|Blue|Alpha;

    }
    pass p1
    {
        VertexShader = compile vs_1_1 vs();
        PixelShader = compile ps_2_0 ps();
    }
}