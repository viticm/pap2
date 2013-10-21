float g_fBiasX = (float)1/(float)256;
float g_fBiasY = (float)1/(float)128;
float g_fMaxShadow = 0.4;
float g_fScaleFactor = 1;
float3 g_vecPos = {0,0,0};
float4x4 g_matViewProj = {
    1,0,0,0,
    0,1,0,0,
    0,0,1,0,
    0,0,0,1
};
float4   g_vecPosScale = {1024,768, 1,1};
float    g_fAlpha = 1;

sampler2D g_samSlot0 : register(s0);


void vs_ShadowPreparation(in float3 iPos : POSITION
                          ,out float4 oPos : POSITION)
{
#if 0
    float fScale = g_vecPosScale.w;
    float3 PosCenter = g_vecPosScale.xyz;
    float4x4 matScale = 
    {
        fScale, 0,0,0
        ,0,fScale,0,0
        ,0,0,fScale,0
        ,PosCenter.x,PosCenter.y, PosCenter.z, 1
    };
    float4 posTemp = mul(float4(iPos, 1), matScale);
    posTemp = mul(posTemp, g_matViewProj);
    oPos = posTemp;
#else
    float4 vecPosScale = g_vecPosScale;
    float3 posTemp = iPos * vecPosScale .w;
    posTemp += vecPosScale.xyz;
    oPos = mul(float4(posTemp,1), g_matViewProj);
#endif
}
float4 ps_ShadowPreparation() : COLOR
{
    return float4(1,0,0,0.5);
}

technique tecShadowPreparation
{
    pass passDoubleStencil
    {
        //<Basic>
        VertexShader = compile vs_1_1 vs_ShadowPreparation();
        PixelShader = compile ps_2_0 ps_ShadowPreparation();        

        LightEnable[0] = FALSE;
        LightEnable[1] = FALSE;
        Texture[0] = NULL;
        
        ShadeMode = Flat;
        Lighting = FALSE;
        FogEnable = FALSE;
        
        ZEnable = TRUE;
        ZFunc = LessEqual;
        ZWriteEnable = FALSE;

        StencilEnable = TRUE;
        StencilFail = Keep;
        StencilPass = Keep;
        StencilFunc = Always;
        StencilRef = 0;
        StencilMask = 0xffffffff;
        StencilWriteMask = 0xffffffff;
        //</Basic>

#if 0    //Use to View the ShadowVolume
        ColorWriteEnable = RED | GREEN | BLUE | ALPHA;
        
        AlphaBlendEnable = TRUE;
        AlphaTestEnable = FALSE;
        SrcBlend = SrcAlpha;
        DestBlend = InvSrcAlpha;
#else
        ColorWriteEnable = 0;
        AlphaBlendEnable = FALSE;
        AlphaTestEnable = FALSE;  
#endif
        //</Basic>
    
        CullMode = None;
        StencilZFail = Decr;
        TwoSidedStencilMode = TRUE;
        CCW_StencilFail = Keep;
        CCW_StencilZFail = Incr;
        CCW_StencilPass = Keep;
        CCW_StencilFunc = Always;                  
    }
    pass passSingleStencil
    {
        //<Basic>
        VertexShader = compile vs_1_1 vs_ShadowPreparation();
        PixelShader = compile ps_2_0 ps_ShadowPreparation();

        LightEnable[0] = FALSE;
        LightEnable[1] = FALSE;
        Texture[0] = NULL;
        
        ShadeMode = Flat;
        Lighting = FALSE;
        FogEnable = FALSE;
        
        ZEnable = TRUE;
        ZFunc = LessEqual;
        ZWriteEnable = FALSE;

        StencilEnable = TRUE;
        StencilFail = Keep;
        StencilPass = Keep;
        StencilFunc = Always;
        StencilRef = 0;
        StencilMask = 0xffffffff;
        StencilWriteMask = 0xffffffff;
        //</Basic>

#if 0    //Use to View the ShadowVolume
        ColorWriteEnable = RED | GREEN | BLUE | ALPHA;
        ColorArg1[0] = TFactor;
        ColorOp[0] = SelectArg1;
        ColorOp[1] = Disable;
        AlphaArg1[0] = TFactor;
        AlphaOp[0] = SelectArg1;

        TextureFactor = 0x88ff0000;
        AlphaBlendEnable = TRUE;
        AlphaTestEnable = FALSE;
        SrcBlend = SrcAlpha;
        DestBlend = InvSrcAlpha;
#else
        ColorWriteEnable = 0;
        AlphaBlendEnable = FALSE;
        AlphaTestEnable = FALSE;  
#endif
        //</Basic>                         
    }
}

void vs_ToBackBuffer(float2 iPos : POSITION                    
                    , out float4 oPos : POSITION                    
)
{
    //Screen Coord to PostProj Coord
    float2 PosTemp = float2(iPos.x*2 - 1, 1-iPos.y*2);
    oPos = float4(PosTemp, 0, 1);
}

float4 ps_ToBackBuffer() : COLOR
{
    return float4(0,0,0,g_fMaxShadow);
}

technique tecToBackBuffer
{
    pass FullSizeNoSoft
    {
        //<Base>
        VertexShader = compile vs_1_1 vs_ToBackBuffer();
        PixelShader = compile ps_2_0 ps_ToBackBuffer();

        ZEnable = FALSE;
        ZWriteEnable = FALSE;
        ZFunc = LessEqual;
        CullMode = CCW;

        FogEnable = FALSE;
        LightEnable[0] = FALSE;
        LightEnable[1] = FALSE;
        ColorWriteEnable = RED | GREEN | BLUE | ALPHA;

        StencilEnable = TRUE;
        StencilFunc = Less;
        StencilRef = 0x00000000;
        StencilFail = Keep;
        StencilZFail = Keep;
        StencilPass = Keep;
        //</Base>

        //AlphaBlendEnable = FALSE;
        AlphaTestEnable = FALSE;

        //FillMode = WireFrame;        
    }    
}
void vs_FinalBlend(float3 iPos : POSITION
                    , float2 iTC : TEXCOORD0
                    , out float4 oPos : POSITION
                    , out float2 oTC : TEXCOORD0
                    )
{
    //Screen Coord to PostProj Coord
    float2 PosTemp = float2(iPos.x*2 - 1, 1-iPos.y*2);
    oPos = float4(PosTemp, 0, 1);

    oTC = iTC; 
}

float4 ps_FinalBlend(float2 iTC: TEXCOORD0):COLOR
{
#if 1    
    float  fSam = tex2D(g_samSlot0, iTC + float2(g_fBiasX, 0)).r;
    float  fSam1 = tex2D(g_samSlot0, iTC + float2(-g_fBiasX, 0)).r;
    float  fSam2 = tex2D(g_samSlot0, iTC + float2(0, g_fBiasY)).r;
    float  fSam3 = tex2D(g_samSlot0, iTC + float2(0, -g_fBiasY)).r;
    float fPercent = fSam+fSam1+fSam2+fSam3;
    fPercent/=4;
    return float4(0,0,0,(1-fPercent) * g_fMaxShadow);
#else
    float fAlpha = tex2D(g_samSlot0, iTC);
    float dxAlpha = ddx(fAlpha);
    float dyAlpha = ddy(fAlpha);
    fAlpha -= (dxAlpha + dyAlpha)/3;//Equals the average value of the tree pixels nearby;
    return float4(0,0,0,(1-fAlpha)*g_fMaxShadow);
#endif
}

technique tecShadowFinalBlend
{
    pass p0
    {
        VertexShader = compile vs_1_1 vs_FinalBlend();
        PixelShader = compile ps_2_0 ps_FinalBlend();

        ZEnable = FALSE;
        ZWriteEnable = FALSE;
        StencilEnable = FALSE;//Should Not Use
        StencilFunc = Less;
        CullMode = CCW;

        FogEnable = FALSE;
        ColorWriteEnable = RED | GREEN | BLUE | ALPHA;
        AlphaBlendEnable = TRUE;
        SrcBlend = SRCALPHA;
        DestBlend = INVSRCALPHA;
    }
}