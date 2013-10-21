//#include "Types.h"

#ifndef SHADER_MODEL
#define SHADER_MODEL 2
#endif

/*
VS_PROFILE and VS_PROFILE are designed for pass parameters like "vertexshader = compile VS_PROFILE VS_ZBUFFER();"
. In this case, VS_PROFILE will be automatically changed according to the macro SHADER_MODEL, and the fx file
can fit both usage in SM3 and SM2.
*/
#ifndef VS_PROFILE
#if SHADER_MODEL == 3 
#define VS_PROFILE	vs_3_0
#else
#define VS_PROFILE	vs_2_0
#endif
#endif

#ifndef PS_PROFILE
#if SHADER_MODEL == 3
#define PS_PROFILE	ps_3_0
#else
#define PS_PROFILE	ps_2_0
#endif
#endif

#define	SHARED shared

#ifndef PACKTYPE
#define PACKTYPE 0
#endif

#ifndef SPECULARENABLE
#define SPECULARENABLE 0
#endif


//The elements of lightParam is initiated

//Do not change orders of the members, they correspond to a certern struct in engine with the same bit-order;
struct LightParam	
{
	float3 m_dir;
	float4 m_diffuse;
	float4 m_specular;
	float4 m_sceneAmbient;	//this is different from "ambient", the previous "ambient" denotes the ambient of light
	float4 m_sky_light;
	float4 m_com_light;
};

//Do not change orders of the members, they correspond to a certern struct in engine with the same bit-order;
struct MaterialParam
{
	float4 diff;
	float4 ambi;
	float4 spec;
	float4 emis;
	float  specPower;
};

struct FogParam 
{
	float4 color;
	float  start;
	float  densityDevidedBy_endMinusStart;	

	/*Because fogFactor = 1 - ((posZ - start) / (end - start)) * density
	So. fogFactor = 1 - (posZ - start) * A;
	A = density / (end - start)

	We only need A 

	It can also deduce fogFactor = 1 + start * A - posZ * A，
	but it also need 2 params, and few performence can be increased
	*/
};

struct MaterialParamEx 
{
	int   bhightlight;
	float4 color_cast;
	float  emssive_power;
	float  spe_exp;
};

struct ShadowMapParam 
{
	int bShadowMask;	//use shadow map or not
	float2 vBiasUV;
};

/*These params will not be compiled as shader asm if you do not use them in any ps or vs.
Therefore, no matter how many params you declare here, it will not add any burden to the 
graphic cards. It is the same situation while any C codes are compiled by a qualified complier
, and for any unused codes, they are just declarations. In another aspect, be awared that too 
many declarations do add burden to the compiler itself.
*/

MaterialParamEx materialEx = {
	false,
	{0.f, 0.f, 1.f, 0.f},
	20.0f,
	1.0f,
};	//this is not shared

MaterialParam materialCur = 
{
	{0,0,0,0},
	{0,0,0,0},
	{0,0,0,0},
	{0,0,0,0},
	1,
};	//this is not shared


SHARED LightParam sunLight;
SHARED LightParam playerLight;

SHARED FogParam	fogParam;

SHARED const float3 sky_light_dir  = {0.f,-1.f, 0.f};
SHARED float3 eyes = {0,0,0};		//camera pos

float3 LUMINANCE_VECTOR  =float3(0.2125f, 0.7154f, 0.0721f);;
SHARED float sharpValue = 0;

SHARED float speValue = 0;

//<Env>
SHARED texture tEnv;	//shared Environement Map, used in reflection
samplerCUBE SameplerEnv = sampler_state	//Sampler can not be shared in DX9
{
	Texture   = <tEnv>;
	MipFilter = LINEAR;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	ADDRESSU = WRAP;
	ADDRESSV = WRAP;
};
//</Env>

//<Conver>
SHARED float4  vConverRect= {0,0,12800,12800};
SHARED texture tConver;
sampler2D SamplerConver = sampler_state
{
	Texture   = <tConver>;
	MipFilter = NONE;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	ADDRESSU = WRAP;
	ADDRESSV = WRAP;
};

float2 ConverUV(float x,float z)
{ 
	float2 uv;
	uv.x  = (x - vConverRect.x)/vConverRect.z;
	uv.y  = 1 - (z - vConverRect.y)/vConverRect.w;
	return uv;
}

//</Conver>

float4 PS_LightCalc_Skin(float4 Tex,float3 normal,float3 pos,float4 skinTex,float edge,float shadowmask)
{
    float k = dot(normal,-sunLight.m_dir) * 0.5 + 0.5;
	float side = max(0,dot(normal,-sky_light_dir));
	float4 light = (side*playerLight.m_sky_light + playerLight.m_sceneAmbient + lerp(playerLight.m_com_light,playerLight.m_diffuse * shadowmask/*(shadowmask * 0.5 + 0.5)*/,k))
	* lerp(1, 1.3,edge * edge);
	light.a = 1;
    return light;
	//float comlight = max(0, dot(sunLight.m_dir, normal));
	//float light    = max(0, dot(-sunLight.m_dir, normal));
	//float skylight = max(0, dot(-sky_light_dir, normal));   
	//float4 diff = (light * playerLight.m_diffuse + comlight * playerLight.m_com_light + skylight * playerLight.m_sky_light) * shadowmask + playerLight.m_sceneAmbient;  
	//return diff;

}

float CalFogFactor(float fZInView)	//fZInView 只需要传递pos * world * view * proj之后的那个float4的z就可以了，那个z因为是在float4中，所以没有正规化，就是view的z，（proj矩阵一般不改z，所以不用单独求view坐标的z），只有除了w之后那个才是0-1的
{
	float fTemp = 1 - (fZInView - fogParam.start) * fogParam.densityDevidedBy_endMinusStart;
	return saturate(fTemp);
}

float4 CalFogColor(float4 color, float fFogFactor)
{
	return float4(lerp(fogParam.color.rgb, color.rgb, fFogFactor).rgb, color.a);
}

float4 PackingFloatToRGB(float fValue)
{
	float4 color = float4(0,0,0,0);
#if PACKTYPE == 1
	//float4 bitSh = float4(256*256*256,256*256,256,1);
	//float4 bitMsk = float4(0,1.0/256.0,1.0/256.0,1.0/256.0);
	float4 bitSh = float4(1,256,256 * 256, 256*256*256);
	float4 bitMsk = float4(1.0/256.0,1.0/256.0,1.0/256.0,0);
	color	= fValue * bitSh;
	color	= frac(color);
	//if (abs(fValue - 1) < 0.00000000001)
	//	color = float4(1,0,0,0);

	color	-= color.yzww * bitMsk;
#else
    color.rgb = fValue;
#endif
   return color;
}

float3 normalCheckVS(float3 iNormal, float4x4 matWorld)
{
	return abs(normalize(iNormal));
	//return abs(normalize(mul(iNormal, (float3x3)matWorld))); 
}

float computerSpecLight(float3 V,float3 N,float3 L,float shininess)
{
	V = normalize(V);
	N = normalize(N);
	L = normalize(L);
	//float3 H = normalize(L + V);
	//float specularLight = pow(max(dot(N, H), 0),shininess);
	//float diffuseLight = max(dot(N, L), 0);
	//if (diffuseLight <= 0) 
	//	specularLight = 0;
	float specularLight = 0;
	float diffuseLight = max(dot(N, L), 0);

	float3 ReflectanceRay = 2 * diffuseLight * N - L;
	specularLight = pow(saturate(dot(ReflectanceRay, V)), shininess); 

	return specularLight;
}

//float4 computerFresnel(float4 color,float3 V,float3 N,float FresAmount,float4 skycolor)
//{
//	V = normalize(V);
//	N = normalize(N);
//	float FresnelTerm = pow((1 - dot(N, V)), 1);
//	float4 FresnelColor = color + float4(0.2,0.2,0.2,0.2) * FresnelTerm;//color * (1 + FresnelTerm);
//	return FresnelColor;
//}

//float4  FogInfo : FOGINFO = {5000,10000,0,0};
//float4  FogColor : FOGCOLOR = {1,1,1,1};
