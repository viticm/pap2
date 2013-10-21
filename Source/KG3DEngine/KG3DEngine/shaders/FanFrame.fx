float4 Color = float4(1,1,1,0.5);
float4x4 World = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
float4x4 ViewProj = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
float3   NormalCenterBegin[3] = {0,1,0,0,0,0,1,0,0};
float   Angle = 1;
float   ConeHeight = 500;

void vs(float3 iPos : POSITION
        , out float4 oPos : POSITION)
{
    /*3D Circle Equation : 
    P = R*cos(t)*VectorU + R * sin(t) cross(VectorNormal,VectorU) 
        + VectorCenter;
    Where R means Radius
    */
    
#if 1
    float3 Normal = NormalCenterBegin[0];
    float3 Center = NormalCenterBegin[1];
    float3 Begin = NormalCenterBegin[2];
    
    float3 VectorUNotNormalized = Begin - Center;
    float3 VectorU = normalize(VectorUNotNormalized);
    
    float3 VectorThirdAxis = cross(VectorU, Normal);//Remember that d3d obeys left-hand rule

    float RadiusPercent = iPos.x;
    float Radius = length(VectorUNotNormalized) * RadiusPercent;
    
    float AnglePercent = iPos.y;
    float t = -Angle * AnglePercent;//Because d3d obeys left-hand rule, so positive angle should be clockwise, 
    
    float3 P = Radius * cos(t) * VectorU 
        + Radius * sin(t) * VectorThirdAxis 
        + Center;

    P += Normal * (ConeHeight * (1 - RadiusPercent));//Heighten the point according to the RadiusPercent in order to form a cone

#else //For Testing Only
    float3 Center = NormalCenterBegin[1];
    float3 Begin = NormalCenterBegin[2];

    float3 VectorUNotNormalized = Begin - Center;


    float RadiusPercent = iPos.x;
    float Radius = length(VectorUNotNormalized ) * RadiusPercent;

    float t = Angle * iPos.y;
    float3 P = float3(cos(t), 0, sin(t)) * Radius;
    P.y += Center.y;
    P.x += Center.x;
    P.z += Center.z;
#endif
    
    float4 Temp = mul(float4(P, 1),World);
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