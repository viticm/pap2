float4 Color = float4(1,1,1,0.5);
float4x4 World = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
float4x4 ViewProj = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
float3   XVecZVec[2] = {0,1,0,1,0,0};
float3   Center = {0,0,0};
float3   LengthXYZ = {100,100,100};

void vs(float3 iPos : POSITION
        , out float4 oPos : POSITION)
{
    float3 XVec = XVecZVec[0];
    float3 ZVec = XVecZVec[1];
    float3 YVec = cross(ZVec, XVec);
    
    float3 xyzPercent = iPos;
    float3 P = Center;
    P += XVec * xyzPercent.x * LengthXYZ.x;
    P += YVec * xyzPercent.y * LengthXYZ.y;
    P += ZVec * xyzPercent.z * LengthXYZ.z;

    float4 vRet = mul(float4(P, 1), World);
    vRet = mul(vRet, ViewProj);
    oPos = vRet;
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