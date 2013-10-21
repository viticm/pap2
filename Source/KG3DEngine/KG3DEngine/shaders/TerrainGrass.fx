//-------------------------------------------
// Desc  : Speed Tree Shader
// Author: Zeb
// Data  : 2006-6-27
//-------------------------------------------
float4x4 matWVP : PROJECTION;

///////////////////////////////////////////////
#include "CommonParams.h"
#include "ShadowMap.h"
#include "PointLight.h"

bool   bAni = true;

sampler s0 : register(s0);
sampler s1 : register(s1);

float Time = 0;

struct VS_OUTPUT
{
    float4 pos          : POSITION;
    float4 diff         : COLOR0;
    float3 texUV     : TEXCOORD0;
    float2 texConver : TEXCOORD1;
	float4 normal_fFog    : TEXCOORD2;
	float4 hpos      : TEXCOORD3;
	float4 wpos      : TEXCOORD4;
#if SHADER_MODEL==3
	float4 pointlight: TEXCOORD5;
#endif
};
struct PS_INPUT
{
	float4 diff      : COLOR0;
    float3 texUV     : TEXCOORD0;
    float2 texConver : TEXCOORD1;
	float4 normal_fFog : TEXCOORD2;
	float4 screenpos : TEXCOORD3;
	float4 wpos      : TEXCOORD4;
#if SHADER_MODEL==3
	float4 pointlight: TEXCOORD5;
#endif
};


#if SHADER_MODEL==3
float4 point_light(float4 diff, float3 normal, float3 pos)
{
	float4 light = diff; 

	light.rgb += CalPointLight(normal, pos);

	return light;
}
#endif



//开启风
bool bEnableWind = false;

//波1
bool bEnableWave1 = true;

//波2
bool bEnableWave2 = true;

//风的方向
float3 directDelta = float3(0.1f, 2.0f, 1.0f);

//顶点扰动方式
int sway = 0;

//震幅(两个波形的影响，表现更丰富)
// amplitude
float amplitude[2] = {0.5f, 0.8f};

//波距
//Angular wave length.
float wavelen[2] = {1000.0, 500.0f};

//风速
// speed
float windspeed[2] = {1.0f, 2.0f};

//波峰放大系数
//waveUp
float waveup[2] = {1.0f, 1.0f};

//波谷放大系数
//waveDown
float wavedown[2] = {0.1f, 0.1f};

//Phase shifts.
float offset[2] = {0.0f, 0.0f};

float SumOfDirectionSineWaves(float x, float z, float dirx, float dirz)
{
	// Distance of vertex from source of waves (which we set
	// as the origin of the local space).

        float d = sqrt(x*x + z*z); 
	
	float3 f0=normalize(float3(x,0,z));
	float3 f1=normalize(float3(dirx,0,dirz));

	float s = d * dot(f0, f1);

	// Sum the waves.
	float sum = 0.0f;
	float sintime = sin(Time);

	//开启波1
	if(bEnableWave1)
	{
	  float f = amplitude[0]*sin(s / wavelen[0]- Time*windspeed[0]) + sintime *offset[0];

	  if(f >= 0.0f)
	     f = f * waveup[0];
	  else
	     f = f * wavedown[0];
	  sum += f;
	}

        //开启波2
	if(bEnableWave2)
	{
	  float f = amplitude[1]*sin(s / wavelen[1]- Time*windspeed[1]) + sintime*offset[1];

	  if(f >= 0.0f)
	     f = f * waveup[1];
	  else
	     f = f * wavedown[1];
	  sum += f;
	}

	return sum * sintime *20.0f;
}


float3 effectPos(float3 Pos, float3 UV)
{
   float swayoffset = UV.z;
   float ta = Time+Pos*0.01F+Pos*0.312;
   if(sway == 0)
   {
     ta +=UV.x;
   }
   else if(sway ==1)
   {
      ta +=UV.z;
   }
   else
   {
      ta = Time;
   }
   //新的方法
   if(bEnableWind)
   {
          float delta =  SumOfDirectionSineWaves(Pos.x, Pos.z, directDelta.x, directDelta.z)* (1- swayoffset);
	  Pos += 10 * sin(ta) * (1- swayoffset);
	  Pos += delta * directDelta *(1- UV.z);
	  return Pos;
   }
   else
   {
         //原来的
	 Pos +=10 * sin(ta) * (1- swayoffset) ;  
         return  Pos;
   }
}

VS_OUTPUT Leaf_Vertex_Shader(
    float3 Pos       : POSITION, 
    float4 normalZip : NORMAL,
    float3 UV        : TEXCOORD0)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    float3 Normal = normalize(normalZip.xyz * 2 - 1); // unzip normal

    float3 vPosNew = Pos;
	if(bAni)
	{
		//vPosNew += 10 * sin(Time+Pos*0.01F+Pos*0.312) * (1- UV.z) ;

                 vPosNew =effectPos(Pos, UV);
		
	
	}
    output.pos = mul(float4(vPosNew , 1), matWVP);
	output.hpos = output.pos;
	output.wpos = float4(vPosNew, 1);

#if SHADER_MODEL!=3   
	float comlight = max(0, dot(sunLight.m_dir, Normal));
	float light    = max(0, dot(-sunLight.m_dir, Normal));
	float skylight = max(0, dot(-sky_light_dir, Normal));  

	output.diff = (light * sunLight.m_diffuse + comlight * sunLight.m_com_light + skylight * sunLight.m_sky_light) + sunLight.m_sceneAmbient;
#else
	float comlight = max(0, dot(sunLight.m_dir, Normal));
	float light    = max(0, dot(-sunLight.m_dir, Normal));
	float skylight = max(0, dot(-sky_light_dir, Normal));  
    output.diff.r = comlight;
	output.diff.g = light;
	output.diff.b = skylight;
	output.pointlight = point_light(float4(0,0,0,0), Normal, output.wpos); 
#endif
	//output.diff = float4(0,0,0,0);
	
    output.texUV = UV ;  
    output.texConver = ConverUV(vPosNew.x,vPosNew.z);
	output.normal_fFog.xyz = Normal;
    output.normal_fFog.w = CalFogFactor(output.pos.z);

    return output;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

float4 ps(PS_INPUT input) : COLOR
{
    float4 colOrg = tex2D(s0, input.texUV.xy);
    float  fAlpha = colOrg.a;
    float4 base = tex2D(SamplerConver, input.texConver);
    float4 diff = input.diff;

#if SHADER_MODEL==3	
   input.screenpos.xyz /= input.screenpos.w;
   input.screenpos.x = input.screenpos.x * 0.5 + 0.5;
   input.screenpos.y = input.screenpos.y * (-0.5) + 0.5;
   float shadowmask = 1.0f;
   if (shadowMapParam.bShadowMask)
   	   shadowmask = GetShadowMask(input.screenpos.xy);
   
    //float lumvalue = 1.0;
    //lumvalue = dot(colOrg.xyz, LUMINANCE_VECTOR) + 0.0001f;
    //lumvalue = lerp(1,lumvalue,sharpValue);
    //shadowmask *= lumvalue;
       
    //float comlight = max(0, dot(sunLight.m_dir, input.normal_fFog.xyz));
    //float light    = max(0, dot(-sunLight.m_dir, input.normal_fFog.xyz));
    //float skylight = max(0, dot(-sky_light_dir, input.normal_fFog.xyz));  
    
    //float3 R =  normalize(2 * light * input.normal_fFog.xyz + sunLight.m_dir);	
    //float3 eye     = normalize(eyes - input.wpos);
    //float4 specLight    = sunLight.m_specular /** materialCur.spec*/ * pow(max(0, dot(R, eye)), /*materialEx.spe_exp*/20)/* * materialCur.specPower*/;
#if SPECULARENABLE == 1
	float4 specLight    = sunLight.m_specular * computerSpecLight(eyes - input.wpos,input.normal_fFog.xyz,-sunLight.m_dir,20);
#endif
	diff = (input.diff.g * sunLight.m_diffuse + input.diff.r * sunLight.m_com_light + input.diff.b * sunLight.m_sky_light) * shadowmask + sunLight.m_sceneAmbient;
#if SPECULARENABLE == 1
	diff += specLight * min(speValue,shadowmask); 
#endif
	//diff = point_light(diff, input.normal_fFog.xyz, input.wpos);     
	diff += input.pointlight;
#endif 

    float4 colorOut = colOrg * base * diff * 3;
    colorOut.a = fAlpha; 

	colorOut = CalFogColor(colorOut, input.normal_fFog.w);

    return colorOut;
}

technique Tech
{
    pass p0
    {
        VertexShader = compile VS_PROFILE Leaf_Vertex_Shader();
        pixelshader = compile PS_PROFILE ps();

		FogEnable = False;	//Fog is calculate in the shader, do not use the d3d fog, it affect the final effect
    }
}

struct VS_Z_OUTPUT
{
    float4    Pos : POSITION;
    float2    rawpos : TEXCOORD0;
	float3    tex : TEXCOORD1;
};

VS_Z_OUTPUT vs_z(
    float3 Pos      : POSITION, 
    float3 UV       : TEXCOORD0)
{
    VS_Z_OUTPUT Out = (VS_Z_OUTPUT)0;
    
	float3 vPosNew = Pos;
	if(bAni)
	{
		//vPosNew += 10 * sin(Time+Pos*0.01F+Pos*0.312) * (1- UV.z) ;
		vPosNew =effectPos(Pos, UV);
		
	}
   
    Out.Pos = mul(float4(vPosNew , 1), matWVP);
    Out.rawpos.x = Out.Pos.z;
    Out.rawpos.y = Out.Pos.w;
    Out.tex = UV ; 
    return Out;
}


float4 ps_z(float2 pos:TEXCOORD0,float3 tex0: TEXCOORD1) : color
{
    float4 lastcolor;
	lastcolor.a = tex2D(s0, tex0.xy).a;
	lastcolor.rgb =  PackingFloatToRGB(pos.x / pos.y).rgb;
    return lastcolor;
}

technique  tecZ
{
    pass Pass0
    {
        vertexshader = compile VS_PROFILE vs_z();
        pixelshader =  compile PS_PROFILE ps_z();
    }
}
