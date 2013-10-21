float4 ambient : MATERIALAMBIENT
<
    string UIName = "Ambient Color";
> = {0.25f, 0.25f, 0.25f, 1.0f};

float4 envambient: ENVAMB = { 1.0f, 1.0f, 1.0f, 1.0f};

float EdgeWidth 
<
    string UIName = "Edge Width";
>;

float4 edgecolor:COLOR
<
    string UIName = "Edge Color";
>;

float4 lightDiffuse0: LIGHTDIFF0
= { 1.0f, 1.0f, 1.0f, 1.0f };

float4 lightPos0: LIGHTPOS0
<
    string UIName = "Light Position";
> = {100.0f, 100.0f, 100.0f, 0.0f};

float4 surfColor : MATERIALDIFFUSE 
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

float4 specularColor : MATERIALSPECULAR
<
    string UIName = "Specular Color";
> = { 0.2f, 0.2f, 0.2f, 1.0f };

float shininess :MATERIALPOWER 
<
    string UIName = "Shininess";
> = 10;


float4x4 wvp : WORLDVIEWPROJECTION;
float4x4 worldI : WORLDI ;
float4x4 viewInvTrans : VIEWI;
float4x4 world : WORLD;
float4x4 worldview: WORLDVIEW;
struct a2v {
    float4 position        : POSITION;
    float2 texCoord        : TEXCOORD0;
    float2 texCoord1       : TEXCOORD1;    
    float3 tangent        : TANGENT;
    float3 binormal        : BINORMAL;
    float3 normal        : NORMAL;
};

struct v2f {
        float4 position        : POSITION;
        float2 texCoord        : TEXCOORD0;
        float2 texCoord1       : TEXCOORD1;
        float3 eyeVec         : TEXCOORD2;
        float3 lightVec       : TEXCOORD3;
//        float     edge : texcoord4;
        float3 objNormal : texcoord4;
};


v2f v(a2v In,
        uniform float4x4 worldViewProj, // object to clip space
        uniform float4x4 WorldIMatrix,  //world to object space
        uniform float4 lightPosition,
        uniform float4x4 ViewInvTrans,
        uniform float4x4 world,        // object to world space
        uniform float3x3 WorldView
        )
{
    v2f Out;
    float3 objNormal;
         
    Out.position = mul(In.position, worldViewProj);
   
    Out.texCoord = In.texCoord;
    Out.texCoord1 = In.texCoord1;
    objNormal = normalize(mul(In.normal, WorldView));
  // compute the 3x3 tranform from tangent space to object space
     float3x3 objTangentXf;

    //Here we got some awful bug fix, theoretically should be
    //objTangentXf[0] = In.tangent.xyz;
    //objTangentXf[1] = In.binormal.xyz;, but the D3DXComputerTangent
    //has some bug, so have to do the shit fixing thing here, btw, 
    //DXSDK oct 2005 will get the almost right result.
    
    objTangentXf[0] = In.binormal.xyz;
    objTangentXf[1] = -In.tangent.xyz;
    objTangentXf[2] = In.normal.xyz;
    
    float4 objSpaceLightPos = mul(lightPosition, WorldIMatrix);
    float3 objLightVec = objSpaceLightPos.xyz - In.position.xyz;
    Out.lightVec = mul(objTangentXf, objLightVec );
    
  //compute the eye vector in world space and put it in object space
      float4 objSpaceEyePos = mul(ViewInvTrans[3], WorldIMatrix);
  // xform eye vector from obj space to tangent space
      float3 objEyeVec = objSpaceEyePos.xyz - In.position.xyz;
      Out.eyeVec = mul(objTangentXf, objEyeVec);
      Out.objNormal = objNormal;
//      Out.edge = pow(1.0f - max(0.0f, dot(normalize(objEyeVec),  normalize(objNormal))), EdgeWidth)/2;
    return Out;
}

float4 blinn2(float3 N,
        float3 L,
        float3 V,
        uniform float4 diffuseColor,
        uniform float4 specularColor,
        uniform float shininess
        )
 {
    float3 H = normalize(V+L);
    float4 lighting = lit(dot(L,N), dot(H,N), shininess);
    return diffuseColor*lighting.y + specularColor*lighting.z;
 }

float4 f(v2f In,
            uniform sampler2D colorTex,
            uniform sampler2D specTex,
            uniform sampler2D ambTex,
            uniform float4 ambient,
            uniform float4 diffuseColor,
            uniform float4 specularColor,
            uniform float shininess,
            uniform float4 lightColor
            ) : COLOR
{
    float4 colorMap = tex2D(colorTex, In.texCoord.xy);
    float4 specMap = tex2D(specTex, In.texCoord.xy);
    float4 Temp = tex2D(specTex, In.texCoord);
    float3 normal = Temp.rgb * 2.0 - 1.0;
    

    float3 amb  =  tex2D(ambTex, In.texCoord1.xy);
    float3 N = normalize(normal);
    float3 V = normalize(In.eyeVec);
    float3 L = normalize(In.lightVec);
  
    float4 C = envambient * ambient * amb.r * colorMap * 2;
    float4 specCol = specularColor*specMap;
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
		FogEnable = False;	//Fog is calculated in the shader, do not use the d3d fog, it affects the final effect
	}
}

