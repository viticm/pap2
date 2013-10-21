
float4x4 mWorldViewProj;  // Composite World-View-Projection Matrix
float4x4 matWorldView;

float fBumpReflectFactor = 10.0f;
float fBumpRefractFactor = 5.0f;
float fTexCoordVel1 = 0.003f;
float fTexCoordVel2 = 0.043f;
float fTexCoordScale1 = 0.008f;
float fTexCoordScale2 = 0.0038f;

float4 crReflectColor = {1.0f, 1.0f, 1.0f, 1.0f};
float4 crRefractColor = {1.0f, 1.0f, 1.0f, 1.0f};



float3 vEyePoint; 
//float fTime;           // Time parameter. This keeps increasing

float4 vTime;

//float4 vec3LightPos = (0.0f, 100.0f, 0.0f, 1.0f);
float fEdgeWidth = 3.0f;
float4 crSpecularColor = (1.0f, 1.0f, 1.0f, 1.0f);
float fSurfaceDetailScale = 0.5f;
float fShinness = 100.0f;

// texture
texture BumpTexture1;
texture BumpTexture2;

texture ReflectTexture;
texture RefractTexture;
texture SurfaceTexture;

// light direction (view space)
float3 lightDir = {0.577, 0.577, 0.577};

float4 vWaterColor = { 0.6f, 0.8f, 0.7f,1.0f};

float4  FogInfo = {5000,10000,0,0};//start,length


sampler2D SamplerBump1 = sampler_state
{
    Texture   = <BumpTexture1>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};
sampler2D SamplerBump2 = sampler_state
{
    Texture   = <BumpTexture2>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};

sampler2D SamplerSurface = sampler_state
{
    Texture   = <SurfaceTexture>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};

sampler2D SamplerReflection = sampler_state
{
    Texture   = <ReflectTexture>;
    MipFilter = NONE;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    AddressU  = CLAMP;
    AddressV   = CLAMP;
};

sampler2D SamplerRefraction= sampler_state
{
    Texture   = <RefractTexture>;
    MipFilter = NONE;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};


struct VS_OUTPUT
{
    float4 Pos  : POSITION;//Vertex Position
    float4 Diff : COLOR0;//Vertex Diffuse
    //float4 Spec : COLOR1;//Specular = 0
    float2 DiffuseMap  : TEXCOORD0;
    float2 DiffuseMap2   : TEXCOORD1;
    float4 Position    : TEXCOORD3;
    float3 vec3Eye     : TEXCOORD4;
    float  FogReg  : FOG;
};


VS_OUTPUT VS_HIGH(
    float3 Pos  : POSITION, 
    float4 Diffuse : COLOR0,
    float4 Tex1  : TEXCOORD0)
{
    VS_OUTPUT Out = (VS_OUTPUT)0;
    Out.vec3Eye = vEyePoint - Pos.xyz;
    Out.Pos  = mul(float4(Pos, 1), (float4x4)mWorldViewProj);
    Out.Diff = Diffuse ;
    
    Out.DiffuseMap.x = Pos.x * fTexCoordScale1;
    Out.DiffuseMap.y = -Pos.z * fTexCoordScale1;
    Out.DiffuseMap.x += vTime.x * fTexCoordVel1;
    Out.DiffuseMap.y += vTime.y * fTexCoordVel1;
       
   
    Out.Position = Out.Pos;
    
    Out.DiffuseMap2.x = Pos.x * fTexCoordScale2;
    Out.DiffuseMap2.y = -Pos.z * fTexCoordScale2;
    Out.DiffuseMap2.x += vTime.x * fTexCoordVel2;
    Out.DiffuseMap2.y += vTime.y * fTexCoordVel2;
        
    half Fog = (Out.Pos.z - FogInfo.x) / (FogInfo.y - FogInfo.x);
    Out.FogReg = 1.0f - Fog * FogInfo.z;

   return Out;
}

half4 blinn(half3 N,
        half3 L,
        half3 V,
        uniform half4 diffuseColor,
        uniform half4 specularColor,
        uniform half shininess
        )
 {
    half3 H = normalize(V+L);
    half4 lighting = lit(dot(L,N), dot(H,N), shininess);
    return diffuseColor*lighting.y + specularColor*lighting.z;
 }
 
float4 PS(
    float4 Diff : COLOR0,
    //float4 Spec : COLOR1,
    float4 DiffuseMap  : TEXCOORD0,
    float2 DiffuseMap2 : TEXCOORD1,
    float3 Normal      : TEXCOORD2,
    float4 Position     : TEXCOORD3,
    float3 vec3Eye       : TEXCOORD4
) : COLOR
{
   half4 SurfaceColor = tex2D(SamplerSurface, DiffuseMap.xy);
   half4 SpecularColor = tex2D(SamplerBump1, DiffuseMap.xy);
   half4 NormalMap = tex2D(SamplerBump1, DiffuseMap2.xy);
   
   
   half  Temp = SpecularColor.a;
   half3 normal = SpecularColor.rbg * 2.0 - 1.0;
   half3 normal2 = NormalMap.rbg * 2.0f - 1.0f;
   
   
   half3 N = normalize(normal + normal2);
   half3 V = normalize(vec3Eye);
   half3 L = normalize(-lightDir);
      
   float4 UV;
   UV.x = (Position.x + Position.w) * 0.5f;
   UV.y = 0.5*Position.w - Position.y*0.5;
   UV.z = Position.z;
   UV.w = Position.w;
   
   
   
   float4 UVW = UV + float4(N, 1.0f) * fBumpReflectFactor;
   float4 UVW2 = UV + float4(N, 1.0f) * fBumpRefractFactor;
     
     //UVW.xyz /= UVW.w;      
     //UVW2.xyz /= UVW2.w;
   //float4 refl = tex2D(SamplerReflection, UVW);
   //float4 refract = tex2D(SamplerRefraction,UVW2);   
   
   float4 refl = tex2Dproj(SamplerReflection, UVW) * crReflectColor;
   float4 refract = tex2Dproj(SamplerRefraction,UVW2) * crRefractColor;
   
   half3 H = normalize(V+L);
   
   float4 C = pow(max(0, dot(H, N)), fShinness) * crSpecularColor;
   //blinn(N, L, V, vWaterColor, crSpecularColor, fShinness);
   
   float RefPower = pow(1.0f - max(0.0f, dot(V, N)), fEdgeWidth);
   refract = lerp(refract, vWaterColor * SurfaceColor, Diff.a);
   //C += lerp(refl, refract, 1.0f - RefPower);
   //C += vWaterColor * SurfaceColor;
   C += lerp(refract, refl, RefPower);
   
   C.a = 1.0f;//vWaterColor.a;
   
   return C;
}

technique tec0
{
    pass p0
    {
        VertexShader = compile vs_1_1 VS_HIGH();
        PixelShader  = compile ps_2_0 PS();
    }
}
