#include "CommonParams.h"

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
        float3 texCoord_fFog        : TEXCOORD0;
        float2 texCoord1       : TEXCOORD1;
        float3 eyeVec         : TEXCOORD2;
        float3 lightVec       : TEXCOORD3;
};


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

float3 l_pos;

float3 vec3EyePos;

texture specTexture : specTexture;
texture AmbientTexture;
texture colorTexture;

sampler2D ColorTextureSampler : register(s0) = sampler_state
{
    Texture = <colorTexture>;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
};

sampler2D SpecularTextureSampler : register(s2) = sampler_state
{
    Texture = <specTexture>;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
};

sampler2D AmbientTextureSampler : register(s1) = sampler_state
{
    Texture = <AmbientTexture>;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
};


float4x4 WorldViewProj;
float4x4 WorldInverse;
float4x4 ViewInverse;
float4x4 World;
float4x4 WorldView;


v2f v(a2v In)
{
    v2f Out;
    float3 Pos = 0.0f;
         
    Out.position = mul(In.position, WorldViewProj);
   
    Out.texCoord_fFog.xy = In.texCoord;
	Out.texCoord_fFog.z = CalFogFactor(Out.position.z);
    Out.texCoord1 = In.texCoord1;

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

	//float4 objSpaceLightPos = mul(l_pos, WorldInverse);
	//float3 objLightVec = l_pos.xyz - In.position.xyz;
	//Out.lightVec = mul(objTangentXf, objLightVec );

	float3 objLight = mul(-sunLight.m_dir, WorldInverse);
	Out.lightVec = mul(objTangentXf, objLight);

	//compute the eye vector in world space and put it in object space
	float4 objSpaceEyePos = mul(ViewInverse[3], WorldInverse);
	// xform eye vector from obj space to tangent space
	float3 objEyeVec = objSpaceEyePos - In.position.xyz;
	Out.eyeVec = mul(objTangentXf, objEyeVec);

	return Out;
}




float4 f(v2f In) : COLOR
{
    float4 colorMap = tex2D(ColorTextureSampler, In.texCoord_fFog.xy);
    float4 specMap = tex2D(SpecularTextureSampler, In.texCoord_fFog.xy);
    float4 Temp = specMap;
    float3 normal = Temp.rgb * 2.0 - 1.0;
    float3 amb = tex2D(AmbientTextureSampler, In.texCoord1.xy);
    float3 N = normalize(normal);
    float3 V = normalize(In.eyeVec);
    float3 L = normalize(In.lightVec);
  
    float4 C = amb.r * sunLight.m_sceneAmbient * materialCur.ambi * colorMap * 2;
    float4 specCol = materialCur.spec * specMap;
    
	float shinness = materialCur.specPower;
    C += blinn2(N, L, V, colorMap * sunLight.m_diffuse * materialCur.diff * 2, materialCur.spec * Temp.a, shinness) + materialCur.emis;
    C = CalFogColor(C, In.texCoord_fFog.z);
    C.a = colorMap.a * materialCur.diff.a;
    return C;
}




                          
technique TechNormalNoSkin
{ 
    pass P0 
    {
        VertexShader = compile vs_2_0 v();
        PixelShader = compile ps_2_0 f();
		FogEnable = False;	//Fog is calculated in the shader, do not use the d3d fog, it affects the final effect
    }
}
