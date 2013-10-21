#include "CommonParams.h"
#include "PointLight.h"
#include "ShadowMap.h"

float4x4 matWorld : WORLDMATIRX;
float4x4 matWVP : PROJECTION;

texture tex1;

texture SelTexture;


sampler2D SamplerTex1 = sampler_state
{
    
    Texture   = <tex1>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    //ADDRESSU = BORDER;
  ADDRESSV = BORDER;
  BORDERCOLOR = {1.0,1.0,1.0,0.0};
};

sampler2D SamplerSelectTexture = sampler_state
{   
    Texture   = <SelTexture>;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    AddressU = BORDER;
    AddressV = BORDER;
    BorderColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
};

struct VS_INPUT
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float4 color : COLOR;
    float2 tex : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 pos : POSITION;
    float4 color : COLOR;
#if SHADER_MODEL==3
	float4 specular : COLOR1;
#else
	float4 diff : COLOR1;
#endif
    float4 texcbase : TEXCOORD0; 
    float2 tex : TEXCOORD1;
#if SHADER_MODEL==3
	float4 Normal_fFog : TEXCOORD2;
#else
	float  fFog : TEXCOORD2;
#endif

};

// project texture 
#define Height_Limit 100
float  proj_size;
float3 proj_center;
float2 calc_proj_uv(float3 pos)
{
    float3 pos_wrd = mul(float4(pos, 1),(float4x4)matWorld);
    
    if (abs(pos_wrd.y - proj_center.y) > Height_Limit)
    {
        return float2(10000.f, 10000.f);
    }
    else
    {
        float3 proj_space = (pos_wrd - proj_center + float3(proj_size / 2.f, 0.f, proj_size / 2.f));
        return float2(proj_space.x / proj_size, proj_space.z / proj_size);
    }
}


#if SHADER_MODEL==3
float4 PS_PointLight(float4 diff,float3 normal,float3 pos)
{
	float4 light = diff; 

	light.rgb += CalPointLight(normal, pos);
	return light;
}
float4 PS_LightCalc(float4 diff,float4 color,float4 base,float3 normal,float3 pos)
{
	float4 light = PS_PointLight(diff,normal,pos);    
	float4 o = lerp(color * base * light *3,color,0);
	return o;
}
#else
float4 PS_LightCalc(float4 diff,float4 color,float4 base)
{
	float4 o = lerp(color * base * diff *3,color,0);
	return o;
}
#endif

VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT Out;

	Out.pos = mul(float4(input.pos, 1), matWVP);             // position (projected)
	Out.texcbase.xy = ConverUV(input.pos.x,input.pos.z);
	Out.tex = input.tex;
	Out.color = input.color;
	////////////////////////////////////
#if SHADER_MODEL==3
	Out.Normal_fFog.xyz  = input.normal;
	Out.specular = float4(input.pos,1);
	Out.Normal_fFog.w = CalFogFactor(Out.pos.z);
#else        
	float comlight = max(0, dot(sunLight.m_dir, input.normal));
	float light    = max(0, dot(-sunLight.m_dir, input.normal));
	float skylight = max(0, dot(-sky_light_dir, input.normal));  
	Out.diff = (light * sunLight.m_diffuse + comlight * sunLight.m_com_light + skylight * sunLight.m_sky_light) + sunLight.m_sceneAmbient;
	Out.fFog = CalFogFactor(Out.pos.z);
#endif


	// project texture
	Out.texcbase.zw = calc_proj_uv(input.pos);
/////////////////////////

	return Out;
}

float4 PS(VS_OUTPUT In) : COLOR
{
	float4 color,ReturnColor;
	float4 base = tex2D(SamplerConver ,In.texcbase.xy);
	color = tex2D(SamplerTex1, In.tex); 

	ReturnColor = color * base;  
	float a = color.a * In.color.a; 
	// project texture

#if SHADER_MODEL==3
	float4 screenpos = mul(In.specular,matWVP);
	screenpos.xyz /= screenpos.w;
	screenpos.x = screenpos.x * 0.5 + 0.5;
	screenpos.y = screenpos.y * (-0.5) + 0.5;
	float shadowmask = 1.0f;
	if (shadowMapParam.bShadowMask)
		shadowmask = GetShadowMask(screenpos.xy);

	float lumvalue = dot(color.xyz, LUMINANCE_VECTOR) + 0.0001f;
	lumvalue = lerp(1,lumvalue,sharpValue);
	shadowmask *= lumvalue;

	float comlight = max(0, dot(sunLight.m_dir, In.Normal_fFog.xyz));
	float light    = max(0, dot(-sunLight.m_dir, In.Normal_fFog.xyz));
	float skylight = max(0, dot(-sky_light_dir, In.Normal_fFog.xyz)); 

	float3 wpos = mul(In.specular, matWorld);

	float4 specLight = sunLight.m_specular * computerSpecLight(eyes - wpos,In.Normal_fFog.xyz,-sunLight.m_dir,60);

	//float4 envLight = texCUBE(SameplerEnv, In.Normal_fFog.xyz ) * mtl.x;
	//float4 emsLight = color * mtl.z * shadowmask;

	float4 diff = (light * sunLight.m_diffuse + comlight * sunLight.m_com_light + skylight * sunLight.m_sky_light) * shadowmask + sunLight.m_sceneAmbient;  
	ReturnColor =  PS_LightCalc(diff,color,base,In.Normal_fFog.xyz,In.specular.xyz) + specLight * shadowmask;              
#else
	ReturnColor = PS_LightCalc(In.diff ,color,base);
#endif

	if (In.texcbase.z >= 0 && In.texcbase.z < 1 && In.texcbase.w >= 0 && In.texcbase.w < 1)
	{
		float4 aoe = tex2D(SamplerSelectTexture, In.texcbase.zw);
		ReturnColor = ReturnColor * (1.f - aoe.a) + aoe * aoe.a; 
	}
#if SHADER_MODEL==3
	ReturnColor = CalFogColor(ReturnColor, In.Normal_fFog.w);
#else
	ReturnColor = CalFogColor(ReturnColor,In.fFog);
#endif

	ReturnColor.a = a;
	return ReturnColor;
} 

technique tec0
{
    pass Pass0
    {
        VertexShader = compile VS_PROFILE VS();
        PixelShader  = compile PS_PROFILE PS();
		FogEnable = False;	//Fog is calculated in the shader, do not use the d3d fog, it affects the final effect
    }
}