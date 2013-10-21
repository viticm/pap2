// Sample effect file

string XFile = "tiger.x";   // model
// transformations
float4x4 World      : WORLD;
float4x4 View       : VIEW;
float4x4 Projection : PROJECTION;

// texture
texture Tex0 < string name = "tiger.bmp"; >;
texture Tex1 < string name = "tiger.bmp"; >;
texture Tex2 < string name = "tiger.bmp"; >;
texture Lightmap < string name = "tiger.bmp"; >;

// light direction (view space)
float3 lightDir <  string UIDirectional = "Light Direction"; > = {0.577, -0.577, 0.577};

float4 Ambient = { 0.3f, 0.3f, 0.3f,1.0F};    // ambient
float4 LightDiffuse;

float3 CouldPos = { 0.0f, 0.0f, 0.0f};

float4  FogInfo = {5000,10000,0,0};//start,length
float4  FogColor = {1,1,1,1};

sampler SamplerLightMap = sampler_state
{
    Texture   = (Lightmap );
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = NONE;
AddressU = CLAMP;
AddressV = CLAMP;
};

sampler SamplerText0 = sampler_state
{
    Texture   = (Tex0);
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = LINEAR;
};
sampler SamplerText1 = sampler_state
{
    Texture   = (Tex1);
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = LINEAR;
};
sampler SamplerText2 = sampler_state
{
    Texture   = (Tex2);
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = LINEAR;
};
sampler SamplerSky = sampler_state
{
    Texture   = (Tex1 );
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = LINEAR;
};

struct VS_OUTPUT
{
    float4 Pos  : POSITION;
    float4 Diff : COLOR0;
    float4 Spec : COLOR1;
    float3 DiffuseMap  : TEXCOORD0;
    float3 SkyMap      : TEXCOORD1;
    float3 Normal     : TEXCOORD2;
    float FogReg : FOG;
};


VS_OUTPUT VS_HIGH(
    float3 Pos  : POSITION, 
    float3 Norm : NORMAL, 
    float4 Diffuse : COLOR0,
    float3 Tex  : TEXCOORD0)
{
    VS_OUTPUT Out = (VS_OUTPUT)0;

    float4x4 WorldView = mul(World, View);
    float3 P = mul(float4(Pos, 1), (float4x3)WorldView);  // position (view space)
    Out.Pos  = mul(float4(P, 1), Projection);             // position (projected)
    float K = max(0,dot(Norm,-lightDir));
    Out.Spec = 0;
    Out.DiffuseMap = Pos.xzy*4/1600;   
    Out.SkyMap.x =  Pos.x*0.0000625f + CouldPos.x ;   
    Out.SkyMap.y =  Pos.z*0.0000625f + CouldPos.y ;  
    Out.SkyMap.z  = 0;   

    Out.Diff = Diffuse;
    Out.Normal = Norm;
    
    float Fog = (Out.Pos.z - FogInfo.x)/(FogInfo.y - FogInfo.x);
    Out.FogReg = 1-Fog*FogInfo.z;

    return Out;
}

float4 PS_0(
    float4 Diff : COLOR0,
    float4 Spec : COLOR1,
    float4 DiffuseMap  : TEXCOORD0,
    float4 CloudMap   : TEXCOORD1,
        float3 Normal : TEXCOORD2
) : COLOR
{
   float2 LightmapUV = DiffuseMap.xy/4;
   LightmapUV.y = 1 - LightmapUV.y;
   float4 Diffuse = tex2D(SamplerText0,DiffuseMap.xy);
   float K = max(0,dot(Normal,-lightDir));
   float4 Light = 0.5*Ambient + 0.6*LightDiffuse * K *tex2D(SamplerLightMap,frac(LightmapUV));
   Light .w  = 1.0;
 return 2*Light*Diffuse;
   //return lerp(2*Light*Diffuse,FogColor,Spec.x);
}

float4 PS_1(
    float4 Diff : COLOR0,
    float4 Spec : COLOR1,
    float4 DiffuseMap  : TEXCOORD0,
    float4 CloudMap   : TEXCOORD1,
        float3 Normal : TEXCOORD2
) : COLOR
{
     float2 LightmapUV = DiffuseMap.xy/4;
   LightmapUV.y = 1 - LightmapUV.y;
  float4 Diffuse0 = tex2D(SamplerText0,DiffuseMap.xy);
   float4 Diffuse1 = tex2D(SamplerText1,DiffuseMap.xy);
   float K = max(0,dot(Normal,-lightDir));
      
   float4 Light = 0.5F*Ambient + 0.6*LightDiffuse* K*tex2D(SamplerLightMap,frac(LightmapUV));
   Light .w  = 1.0;
   return 2*Light*lerp(Diffuse1,Diffuse0,Diff.w);
   //return lerp(2*Light*lerp(Diffuse1,Diffuse0,Diff.w),FogColor,Spec.x);
}

float4 PS_2(
    float4 Diff : COLOR0,
    float4 Spec : COLOR1,
    float4 DiffuseMap  : TEXCOORD0,
    float4 CloudMap   : TEXCOORD1,
        float3 Normal : TEXCOORD2
) : COLOR
{
  float2 LightmapUV = DiffuseMap.xy/4;
   LightmapUV.y = 1 - LightmapUV.y;
   float4 Diffuse0 = tex2D(SamplerText0,DiffuseMap.xy);
   float4 Diffuse1 = tex2D(SamplerText1,DiffuseMap.xy);
   float4    Diffuse2 = tex2D(SamplerText2,DiffuseMap.xy);
   float4 Diffuse01 = lerp(Diffuse1,Diffuse0,Diff.w);
   float K = max(0,dot(Normal,-lightDir));
   float4 Light = 0.5*Ambient + 0.6*LightDiffuse * K*tex2D(SamplerLightMap,frac(LightmapUV));
   Light .w  = 1.0;
return 2*Light*lerp(Diffuse2,Diffuse01,Diff.x);
   //return lerp(2*Light*lerp(Diffuse2,Diffuse01,Diff.x),FogColor,Spec.x);
}

technique tec0
{
    pass p0
    {

        FogStart = 0;

        VertexShader = compile vs_1_1 VS_HIGH();
        PixelShader  = compile ps_2_0 PS_0();
    }
    pass p1
    {
        VertexShader = compile vs_1_1 VS_HIGH();
        PixelShader  = compile ps_2_0 PS_1();
    }
    pass p2
    {
        VertexShader = compile vs_1_1 VS_HIGH();
        PixelShader  = compile ps_2_0 PS_2();
    }
}
