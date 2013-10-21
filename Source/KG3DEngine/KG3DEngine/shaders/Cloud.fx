int gTexCount = 2;
texture gTex0;
texture gTex1;

struct ShaderParams
{
    float texCoordBias;
    float alphaFactor;
    float3 sunDir;    //Transforming sunLight.dir to the texture coordinate, it's z should be reversed
    float4 diffuse;
    float4 ambient;
    float fTexFactor;
};
ShaderParams gShaderParams = {
    0.00195f,
    1,
    {0,-1,0},
    {1,1,1,1},
    {1,1,1,1},
    1.0,
}; 

sampler2D gSam0 = sampler_state{
    texture = <gTex0>;
    mipfilter = linear;
    minfilter = linear;
    magfilter = linear;
};

sampler2D gSam1 = sampler_state{
    texture = <gTex1>;
    mipfilter = linear;
    minfilter = linear;
    magfilter = linear;
};


float4x4 gWorldMatrix = {1,0,0,0,
                     0,1,0,0,
                     0,0,1,0,
                     0,0,0,1,};
float4x4 gViewProjMatrix= {1,0,0,0,
                     0,1,0,0,
                     0,0,1,0,
                     0,0,0,1,};
float4x4 gTexTransMatrix= {1,0,0,0,
                     0,1,0,0,
                     0,0,1,0,
                     0,0,0,1,};

float gCloudWidthDevideThickness = 30.f;

void vs(float3 iPos : POSITION
        , float2 iTexCd0 : TEXCOORD0
        , float2 iTexCd1 : TEXCOORD1
        , out float4 oPos : POSITION
        , out float2 oTexCd0 : TEXCOORD0
        , out float2 oTexCd1 : TEXCOORD1)
{
    float4 tempPos = mul(float4(iPos, 1),gWorldMatrix);
    oPos = mul(tempPos, gViewProjMatrix);
    
    oTexCd0 = mul(float3(iTexCd0, 1), gTexTransMatrix);
    oTexCd1 = iTexCd1;    
}

float4 ps(float2 iTexCd0 : TEXCOORD0,
          float2 iTexCd1 : TEXCOORD1,
          uniform float enableColorControl) : COLOR
{
    float4 colorgTex0 = tex2D(gSam0, iTexCd0);
    float4 colorgTex1 = tex2D(gSam1, iTexCd1);
    
    colorgTex0.rgb *= gShaderParams.fTexFactor;
    colorgTex0.rgb = saturate(colorgTex0.rgb);
    
    float4 colorRet = colorgTex0 *= gShaderParams.alphaFactor;
    colorRet.a *= colorgTex1.a;

    if(enableColorControl)
    {
        float normalDotLight = 1;
        
        float3 sunDirInTex = float3(gShaderParams.sunDir.xy, -gShaderParams.sunDir.z);
        
        //Calculate the normal of the texture as if generating normalMap from heightMap
        {   
            /*
            By CTH:
            
            Suppose the Cloud is "fWidth" wide, and "fThickness" thick.

            According to the algorithm of "HeightMap to NormalMap Generation",
            normal = -dx(z), -dy(z), t; //t is the gap between points;
            
            Let's scale the cloud alpha to "fThickness"
            
            And "t" should equal the texcood gap muls fWidth;

            So the normal should equal :

            dx(z) * fThickness, dy(z) * fThickness, sampleGap * fWidth;

            Devide 3 factor by fThickness;

            Finally we got: 

            dx(z), dy(z), sampleGap * fWidth / fThickness;  

            notice that in d3d, y is the vertical axis, and we get the following expressions
            */
     
            float fBias = gShaderParams.texCoordBias * 8;
            float height0 = tex2D(gSam0, iTexCd0 + float2(-fBias,0)).a;
            float height1 = tex2D(gSam0, iTexCd0 + float2(fBias,0)).a;

            float height2 = tex2D(gSam0, iTexCd0 + float2(0,-fBias)).a;
            float height3 = tex2D(gSam0, iTexCd0 + float2(0,fBias)).a;       

            

            float t = fBias * 2 * gCloudWidthDevideThickness ;
            float3 normalOfPoint = float3(height0 - height1 , -t, height3 - height2);
            normalize(normalOfPoint);        

            normalDotLight = dot(normalOfPoint, -gShaderParams.sunDir);
#if 0
            colorRet.rgb = 1; 
#endif
            
            //colorRet.rgb = max(dot(normalOfPoint, float3(0, 0,-1)), 0);         
        }
#if 1
        { 
            float2 sunPosInTex = float2(0.5f, 0.5f) - 0.5f * float2(sunDirInTex.x, sunDirInTex.z);
            float fDistanceFactor = (1.0f - length(iTexCd1 - sunPosInTex) / 1.0f);
            fDistanceFactor = lerp(0,1.f,fDistanceFactor);
        
            //colorRet.rgb = fDistanceFactor;
            
            colorRet.rgb = gShaderParams.ambient.rgb * fDistanceFactor;

            if(normalDotLight < -0.15f)
            {
                float ambientFactor =max(-normalDotLight, 0);
                ambientFactor = lerp(-0.05f, 0.4f, ambientFactor);
           
                colorRet.rgb -= gShaderParams.ambient.rgb * ambientFactor;
            }
            else
            {
                float diffFactor = max(normalDotLight,0);
                colorRet.rgb += gShaderParams.diffuse.rgb * 4 * diffFactor * fDistanceFactor;
            }

            float fTranslucenceFactor = abs(gShaderParams.sunDir.y);
            colorRet.rgb += (fTranslucenceFactor * 0.5f);
        }
#endif
    }
    return colorRet;
}

technique defaultTech
{
    pass p0
    {
        VertexShader = compile vs_1_1 vs();
        PixelShader = compile ps_2_0 ps(false);

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
        PixelShader = compile ps_2_0 ps(false);

        AlphaBlendEnable = True;
        SrcBlend = SrcAlpha;
        DestBlend = InvSrcAlpha;

        ZEnable = True;
        ZWriteEnable = True;
        FogEnable = False;
        FillMode = Solid;
        CullMode = None;
        ColorWriteEnable = 0;    
    }
}
technique advancedTech
{
    pass p0
    {
        VertexShader = compile vs_1_1 vs();
        PixelShader = compile ps_2_0 ps(true);

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
        PixelShader = compile ps_2_0 ps(false);
        
        AlphaBlendEnable = True;
        SrcBlend = SrcAlpha;
        DestBlend = InvSrcAlpha;

        ZEnable = True;
        ZWriteEnable = True;
        FogEnable = False;
        FillMode = Solid;
        CullMode = None;
        ColorWriteEnable = 0;   
    }
}