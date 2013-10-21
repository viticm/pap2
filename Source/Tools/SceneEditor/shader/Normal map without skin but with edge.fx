half4 ambient : MATERIALAMBIENT
<
    string UIName = "Ambient Color";
> = {0.25f, 0.25f, 0.25f, 1.0f};

half4 envambient: ENVAMB = { 1.0f, 1.0f, 1.0f, 1.0f};

half EdgeWidth 
<
    string UIName = "Edge Width";
>;

half4 edgecolor:COLOR
<
    string UIName = "Edge Color";
>;

half4 lightDiffuse0: LIGHTDIFF0
= { 1.0f, 1.0f, 1.0f, 1.0f };

half4 lightPos0: LIGHTPOS0
<
    string UIName = "Light Position";
> = {100.0f, 100.0f, 100.0f, 0.0f};

half4 surfColor : MATERIALDIFFUSE 
<
    string UIName = "Diffuse Color";
> = {1.0f, 1.0f, 1.0f, 1.0f};

texture colorTexture: DiffuseMap
<
    string UIName = "Diffuse Texture";
    string TextureType = "2D";
>;

texture specTexture: specularmap
<
    string UIName = "Specular Texture";
    string TextureType = "2D";
>;


texture AmbientMap: AmbientMap
<
    string UIName = "Ambient Map";
>;

half4 specularColor : MATERIALSPECULAR
<
    string UIName = "Specular Color";
> = { 0.2f, 0.2f, 0.2f, 1.0f };

half shininess :MATERIALPOWER 
<
    string UIName = "Shininess";
> = 10;


half4x4 wvp : WORLDVIEWPROJECTION;
half4x4 worldI : WORLDI ;
half4x4 viewInvTrans : VIEWI;
half4x4 world : WORLD;
half4x4 worldview: WORLDVIEW;
struct a2v {
    half4 position        : POSITION;
    half2 texCoord        : TEXCOORD0;
    half2 texCoord1       : TEXCOORD1;    
    half3 tangent        : TANGENT;
    half3 binormal        : BINORMAL;
    half3 normal        : NORMAL;
};

struct v2f {
        half4 position        : POSITION;
        half2 texCoord        : TEXCOORD0;
        half2 texCoord1       : TEXCOORD1;
        float3 eyeVec         : TEXCOORD2;
        half3 lightVec       : TEXCOORD3;
//        half     edge : texcoord4;
        half3 objNormal : texcoord4;
};


v2f v(a2v In,
        uniform half4x4 worldViewProj, // object to clip space
        uniform half4x4 WorldIMatrix,  //world to object space
        uniform half4 lightPosition,
        uniform half4x4 ViewInvTrans,
        uniform half4x4 world,        // object to world space
        uniform half3x3 WorldView
        )
{
    v2f Out;
    half3 objNormal;
         
    Out.position = mul(In.position, worldViewProj);
   
    Out.texCoord = In.texCoord;
    Out.texCoord1 = In.texCoord1;
    objNormal = normalize(mul(In.normal, WorldView));
  // compute the 3x3 tranform from tangent space to object space
     half3x3 objTangentXf;

    //Here we got some awful bug fix, theoretically should be
    //objTangentXf[0] = In.tangent.xyz;
    //objTangentXf[1] = In.binormal.xyz;, but the D3DXComputerTangent
    //has some bug, so have to do the shit fixing thing here, btw, 
    //DXSDK oct 2005 will get the almost right result.
    
    objTangentXf[0] = In.binormal.xyz;
    objTangentXf[1] = -In.tangent.xyz;
    objTangentXf[2] = In.normal.xyz;
    
    half4 objSpaceLightPos = mul(lightPosition, WorldIMatrix);
    half3 objLightVec = objSpaceLightPos.xyz - In.position.xyz;
    Out.lightVec = mul(objTangentXf, objLightVec );
    
  //compute the eye vector in world space and put it in object space
      half4 objSpaceEyePos = mul(ViewInvTrans[3], WorldIMatrix);
  // xform eye vector from obj space to tangent space
      half3 objEyeVec = objSpaceEyePos.xyz - In.position.xyz;
      Out.eyeVec = mul(objTangentXf, objEyeVec);
      Out.objNormal = objNormal;
//      Out.edge = pow(1.0f - max(0.0f, dot(normalize(objEyeVec),  normalize(objNormal))), EdgeWidth)/2;
    return Out;
}

half4 blinn2(half3 N,
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

float4 f(v2f In,
            uniform sampler2D colorTex,
            uniform sampler2D specTex,
            uniform sampler2D ambTex,
            uniform half4 ambient,
            uniform half4 diffuseColor,
            uniform half4 specularColor,
            uniform half shininess,
            uniform half4 lightColor
            ) : COLOR
{
    half4 colorMap = tex2D(colorTex, In.texCoord.xy);
    half4 specMap = tex2D(specTex, In.texCoord.xy);
    half4 Temp = tex2D(specTex, In.texCoord);
    half3 normal = Temp.rgb * 2.0 - 1.0;
    

    half3 amb  =  tex2D(ambTex, In.texCoord1.xy);
    half3 N = normalize(normal);
    half3 V = normalize(In.eyeVec);
    half3 L = normalize(In.lightVec);
  
    half4 C = envambient * ambient * amb.r * colorMap * 2;
    half4 specCol = specularColor*specMap;
    lightColor.xyz = lightColor.xyz * amb.b;
    
    C += lightColor * blinn2(N, L, V, colorMap*diffuseColor * 2, specularColor*Temp.a,shininess);
    float4 color = pow(1.0f - max(0.0f, dot(normalize(In.eyeVec), normal)),  EdgeWidth)  * edgecolor;
    C += color;
    C.a = colorMap.a;
    return C;
}

sampler2D colorTextureSampler = sampler_state
{
    Texture = <colorTexture>;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
};

sampler2D SpecularMapSampler = sampler_state
{
    Texture = <specTexture>;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
};

sampler2D AmbientMapSampler = sampler_state
{
    Texture = <AmbientMap>;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
};



                          
technique Complete
{ 
    pass envPass 
    {
    AlphaTestEnable = true;        
    SrcBlend = srcalpha;
    Destblend = invsrcalpha;
    ALPHABLENDENABLE = true;
    VertexShader = compile vs_1_1 v(wvp,worldI,lightPos0,viewInvTrans,world, worldview);
    PixelShader = compile ps_2_0 f(colorTextureSampler, SpecularMapSampler, AmbientMapSampler, ambient,surfColor,specularColor,shininess,lightDiffuse0);
    }
}

