//-------------------------------------------
// Desc  : Speed Tree Shader
// Author: Zeb
// Data  : 2006-6-27
//-------------------------------------------
#include "CommonParams.h"
#include "PointLight.h"
#include "ShadowMap.h"

bool bAnimation = true;

float4x4 mWorld;
float4x4 mViewProj;

float3 vCameraLeft; 
float3 vCameraUp;
float3 vCameraFront;
float  Scale = 0.5f;
float  fTrans = 0;
float Time = 0;

sampler s0 : register(s0);
sampler s1 : register(s1);

float   Alpha = 1;

/////////////////////////////////////////////////////////

struct VS_OUTPUT
{
    float4 Pos          : POSITION;
    float4 Color        : COLOR0;
    float4 TexCoord     : TEXCOORD0;
    float4 wpos_fFog    : TEXCOORD1;
    float3 nol          : TEXCOORD2;
    float4 shadowPos    : TEXCOORD3;
#if SHADER_MODEL == 3 
	float4 pointlight   : TEXCOORD4;
#endif
};
struct VS_OUTPUT_Z
{
    float4 Pos          : POSITION;
    float2 TexCoord     : TEXCOORD0;
    float2  depth         : TEXCOORD1;
};

float4 LightCalc_PointLight(float k,float side,float3 normal,float shadowmask)
{
	//float k = lightpar.r;//max(0,dot(normal,-sunLight.m_dir));
	//float side = lightpar.g;//max(0,dot(normal,-sky_light_dir));
	
    float4 light = side*sunLight.m_sky_light + lerp(sunLight.m_sceneAmbient,sunLight.m_sceneAmbient+sunLight.m_diffuse * shadowmask,k); 
	
    return light;
}

float4 point_light(float4 diff, float3 normal, float3 pos)
{
	float4 light = diff; 

	light.rgb += CalPointLight(normal, pos);   

	return light;
}


VS_OUTPUT Leaf_Vertex_Shader(
    float3 Pos      : POSITION, 
    float3 Normal   : NORMAL,
    float2 Offset   : TEXCOORD0,
    float3 UV        : TEXCOORD1)
{
    VS_OUTPUT Out = (VS_OUTPUT)0;
    
    float3 wpos = mul(float4(Pos, 1), mWorld).xyz;
    float2 FF = Offset;
    if (bAnimation)
    {
    	FF += sin(Time + wpos.xz * 0.01F)*10 / Scale;
    }
    wpos = wpos + vCameraLeft * FF.x * Scale + vCameraUp * FF.y * Scale;
    Out.TexCoord.xy = UV.xy;
    Out.TexCoord.zw = ConverUV(wpos.x,wpos.z); 

    Out.Pos = mul(float4(wpos, 1), mViewProj);
    Out.shadowPos = Out.Pos;

    float3 n = normalize(mul(Pos, mWorld).xyz);
#if SHADER_MODEL!=3   
    float k = max(0, dot(n, -sunLight.m_dir));
	float side = max(0,dot(n,-sky_light_dir));
    Out.Color = LightCalc_PointLight(k,side,n,1.0f);
#else
	Out.Color.r = max(0,dot(n,-sunLight.m_dir));
	Out.Color.g = max(0,dot(n,-sky_light_dir));
	Out.pointlight = point_light(float4(0,0,0,0),n,wpos);
#endif
    Out.Color.a = Alpha;    
    
    
    Out.wpos_fFog.xyz = wpos;
	Out.wpos_fFog.w = CalFogFactor(Out.Pos.z);

    Out.nol = n;
    
    

    return Out;
}


VS_OUTPUT_Z Leaf_Vertex_Shader_Z(
    float3 Pos      : POSITION, 
    float3 Normal   : NORMAL,
    float2 Offset   : TEXCOORD0,
    float3 UV        : TEXCOORD1)
{
    VS_OUTPUT_Z Out = (VS_OUTPUT_Z)0;
    
    float3 wpos = mul(float4(Pos, 1), mWorld).xyz;
    float2 FF = Offset;
    if (bAnimation)
    {
    	FF += sin(Time + wpos.xz * 0.01F)*10/Scale;
    }
    wpos = wpos + vCameraLeft * FF.x * Scale + vCameraUp * FF.y * Scale + Offset.x * Scale * fTrans * vCameraFront;
    Out.TexCoord.xy = UV.xy;
    Out.Pos = mul(float4(wpos, 1), mViewProj);
    Out.depth.x = Out.Pos.z / Out.Pos.w;
    Out.depth.y = 1;

    return Out;
}


float4 ps(float4 Tex0:TEXCOORD0, float4 wpos_fFog : TEXCOORD1, float3 nol : TEXCOORD2,
		  float4 incolor : color0,float4 screenpos : TEXCOORD3
#if SHADER_MODEL == 3
		  ,float4 pointlight : TEXCOORD4
#endif
		  ) : color
{
	float shadowmask = 1.0;
	float4 colorOrg = tex2D(s0, Tex0.xy);
	float4 diff = float4(0,0,0,0);
#if SHADER_MODEL==3
	//float3 n = normalize(mul(screenpos, mWorld).xyz);
    //float i = max(0, dot(n, -sunLight.m_dir));
	screenpos.xyz /= screenpos.w;
	screenpos.x = screenpos.x * 0.5 + 0.5;
	screenpos.y = screenpos.y * (-0.5) + 0.5;
	if (shadowMapParam.bShadowMask)
		shadowmask = GetShadowMask(screenpos.xy);
	else
		shadowmask = 1.0f;
	//float lumvalue = 1.0;
 //   lumvalue = dot(colorOrg.xyz, LUMINANCE_VECTOR) + 0.0001f;
 //   lumvalue = lerp(1,lumvalue,sharpValue);
 //   shadowmask *= lumvalue;
	

    //float light    = max(0, dot(-sunLight.m_dir, nol));
    //float3 R =  normalize(2 * light * nol + sunLight.m_dir);	
    //float3 eye     = normalize(eyes - wpos_fFog.xyz);
    //float4 specLight    = sunLight.m_specular /** materialCur.spec*/ * pow(max(0, dot(R, eye)), /*materialEx.spe_exp*/20)/* * materialCur.specPower*/;
    //
#if SPECULARENABLE == 1  
	float4 specLight = sunLight.m_specular * computerSpecLight(eyes - wpos_fFog.xyz,nol,-sunLight.m_dir,20);
#endif
    incolor.rgb = LightCalc_PointLight(incolor.r,incolor.g,nol,shadowmask).rgb;
#if SPECULARENABLE == 1  
	incolor.rgb += specLight * min(speValue,shadowmask); 
#endif
   diff = pointlight;
#endif                

    diff +=  incolor;//point_light(incolor, nol, wpos_fFog.xyz);        
    float4 col1 = tex2D(SamplerConver , Tex0.zw);

    float4 colorOut = colorOrg * 2 * diff * col1 * 1.5;
	colorOut = CalFogColor(colorOut, wpos_fFog.w);
	return colorOut;
}


float4 ps_Z(float2 Tex0:TEXCOORD0,
    float2 depth : TEXCOORD1) : color
{
      
    float4 color = tex2D(s0, Tex0.xy) * 3;
    color.rgb = PackingFloatToRGB(depth.x).rgb;
    return color;
}

technique Tech
{
    pass p0
    {
        VertexShader = compile VS_PROFILE Leaf_Vertex_Shader();
        pixelshader = compile  PS_PROFILE ps();
        CullMode        = None;                                    
        ZEnable            = True;
        
        Lighting        = True;
        SpecularEnable = False;

        AlphaBlendEnable = False;
        SrcBlend        = SrcAlpha;          
        DestBlend       = InvSrcAlpha;
        AlphaTestEnable = True;
        AlphaFunc = GREATEREQUAL;
        AlphaRef        = 0x90;

        ColorArg1[0] = TEXTURE;
        ColorArg2[0] = DIFFUSE;
        ColorOp[0] = MODULATE2X;
        
        AlphaArg1[0] = TEXTURE;
        AlphaArg2[0] = CURRENT;
        AlphaOp[0] = SELECTARG1;

		FogEnable = False;	//Fog is calculated in the shader, do not use the d3d fog, it affects the final effect
    }
    pass p1//RENDER FOR DEPTH ONLY
    {
        VertexShader = compile VS_PROFILE Leaf_Vertex_Shader_Z();
        pixelshader = compile  PS_PROFILE ps_Z();
        FogEnable     = FALSE;
        CullMode    = None;                                    
        ZEnable     = True;
        ZFunc = LESSEQUAL;
        ZWriteEnable = True;
        //ColorWriteEnable = RED;

        AlphaBlendEnable = False;
        SrcBlend        = SrcAlpha;          
        DestBlend       = InvSrcAlpha;
        
        AlphaTestEnable = True;
        AlphaFunc = GREATEREQUAL;
        AlphaRef        = 0xF0;
    }
    
    pass p2//Render color only
    {
        VertexShader = compile VS_PROFILE Leaf_Vertex_Shader();
        pixelshader = compile  PS_PROFILE ps();
        CullMode    = None;                                    
        ZEnable     = True;
        ZWriteEnable = False;
        ZFunc = EQUAL;

        AlphaBlendEnable = False;
        AlphaTestEnable = False;
        AlphaFunc = GREATEREQUAL;
        AlphaRef        = 0x70;

        ColorArg1[0] = TEXTURE;
        ColorArg2[0] = DIFFUSE;
        ColorOp[0] = MODULATE2X ;
        
        AlphaArg1[0] = TEXTURE;
        AlphaArg2[0] = CURRENT;
        AlphaOp[0]   = SELECTARG1;

		FogEnable = False;	//Fog is calculated in the shader, do not use the d3d fog, it affects the final effect
    }
    pass p3//Render color and alpha blend only
    {
        VertexShader = compile VS_PROFILE Leaf_Vertex_Shader();
        pixelshader = compile  PS_PROFILE ps();

		FogEnable = False;	//Fog is calculated in the shader, do not use the d3d fog, it affects the final effect

        ColorArg1[0] = TEXTURE;
        ColorArg2[0] = DIFFUSE;
        ColorOp[0] = MODULATE2X ;

        AlphaBlendEnable = False;
        AlphaTestEnable = False;

        AlphaArg1[0] = TEXTURE;
        AlphaArg2[0] = CURRENT;
        AlphaOp[0]   = SELECTARG1;

    }
}
