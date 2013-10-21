//Author : Tianhong Chen

#include "ShaderDefinitions.h"

sampler2D samp0 : register(s0);

ShaderConstsLensflare	g_shaderConsts;

void vertexShader(     in     float3 iVB : POSITION
                      ,out    float4 oPos : POSITION
                      ,in     half2 iTexCoordInfo : TEXCOORD0
                      ,out    half2 oTexCoord : TEXCOORD0
)
{
    float2 PosRet = iVB.xy;//Scale is included in the xy  
    PosRet.y *= g_shaderConsts.fScreenAspect;
    float2 vTrans = g_shaderConsts.vSunPosPostProj * iVB.z;    //Height is contained in the z element
    PosRet += vTrans;
    oPos = float4(PosRet,0,1); 

    oTexCoord = g_shaderConsts.vTexSplit * iTexCoordInfo;
}


float4 pixelShaderFirst(    in float2 iTexCoord : TEXCOORD0) : COLOR
{
    float4 p0 = tex2D(samp0, iTexCoord); 
    return float4(p0.rgb, g_shaderConsts.fBlendFactor * p0.a);
}

void vertexShaderOther(     in     float3 iVB : POSITION
                          ,out    float4 oPos : POSITION
                          ,in     half2 iTexCoordInfo : TEXCOORD0
                          ,out    half2 oTexCoord : TEXCOORD0
)
{
    float3 PosRet = float3(iVB.xy,1);    //Scale is included in the xy
    PosRet = mul(PosRet,(float3x3)g_shaderConsts.matRotation);  //Rotate the Lens Frame
    PosRet.y *= g_shaderConsts.fScreenAspect;
    float2 vTrans = g_shaderConsts.vSunPosPostProj * iVB.z;    //Height is contained in the z element
    PosRet.xy += vTrans;
    oPos = float4(PosRet.xy,0,1);

    oTexCoord = g_shaderConsts.vTexSplit * iTexCoordInfo;
}

float4 pixelShaderOther(    in float2 iTexCoord : TEXCOORD0) : COLOR
{
    float4 p0 = tex2D(samp0, iTexCoord);
    p0.rgb *= g_shaderConsts.fBlendFactor;
    return float4(p0.rgb, 0);
}

void vertexShaderOC(      in    float3 iVB : POSITION
                        ,out    float4 oPos : POSITION
)
{
    float2 PosTemp = iVB.xy;
    PosTemp.y *= g_shaderConsts.fScreenAspect;
    oPos = float4(PosTemp + g_shaderConsts.vSunPosPostProj, 1, 1);
}

float4 pixelShaderOC() : COLOR
{
    return float4(0,0,0,g_shaderConsts.fOCFactor);
}

technique tec0
{
    pass pFirst
    {
        VertexShader = compile vs_1_1 vertexShader();
        PixelShader = compile ps_2_0 pixelShaderFirst();
        
        ZEnable = FALSE;
        FogEnable = FALSE;
        CullMode = CCW;
        ZWriteEnable = FALSE;

        AlphaBlendEnable = TRUE;
        SrcBlend = SrcAlpha;
        DestBlend = InvSrcAlpha;  
    }
    pass pFlare
    {
        VertexShader = compile vs_1_1 vertexShaderOther();
        PixelShader = compile ps_2_0 pixelShaderOther();
        
        ZEnable = FALSE;
        FogEnable = FALSE;
        ZWriteEnable = FALSE;
		CullMode = CCW;
        
        AlphaBlendEnable = TRUE;
        SrcBlend = One;
        DestBlend = One;          //Add Color     
    }
    pass pOC
    {
        VertexShader = compile vs_1_1 vertexShaderOC();
        PixelShader = compile ps_2_0 pixelShaderOC();

        ZEnable = TRUE;
        ZWriteEnable = FALSE;
        ZFunc = LessEqual;
        
        FogEnable = FALSE;
        
        AlphaBlendEnable = TRUE;
        SrcBlend = SrcAlpha;
        DestBlend = InvSrcAlpha;  


    }
}
 