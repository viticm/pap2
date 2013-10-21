/*
Normal Check: If this file is included by another file, which has a header that set NORMAL_CHECK as 1
, then the normal-checking part in his shader file will be activated.
These is a benefit obviously that no matter how much we change the file, even the input structure,
the normal checking function remains available. 
*/
#ifndef NORMAL_CHECK
#define NORMAL_CHECK 0
#endif

#include "CommonParams.h"
#include "PointLight.h"
#include "ShadowMap.h"


float4x4 matWorld : WORLDMATIRX;
float4x4 matWVP : PROJECTION;

///////////////////////////////////////////////

float4 UVT[16];
float4 MtlFactor[8];
float4 vSceneRect = {0,0,12800,12800};

sampler s0 : register(s0);
sampler s1 : register(s1);
sampler s2 : register(s2);
sampler s3 : register(s3);  
sampler s4 : register(s4);  
sampler s5 : register(s5);  
sampler s6 : register(s6);  
sampler s7 : register(s7);  


float farsee = 12800 *2; //用两个section的距离来过渡高低精度贴图
bool blendHighNLow = false; //是否混合高低精度贴图,默认不开启 // by likan
texture tTerrainMap;  // by Likan
sampler2D SamplerTerrainMap = sampler_state
{
    Texture   = <tTerrainMap>;
    MipFilter = LINEAR;
    MagFilter = LINEAR;
    MinFilter = LINEAR;
    ADDRESSU = WRAP;
    ADDRESSV = WRAP;
};


//texture tDetail0;
//texture tDetail1;
//texture tDetail2;
//texture tDetail3;
//texture tDetail4;
//texture tDetail5;
//texture tDetail6;
//texture tDetail7;
texture SelTexture;

texture tDetailNormal0;

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

sampler2D SamplerDetailNormal0 = sampler_state
{
    Texture   = <tDetailNormal0>;
    MipFilter = LINEAR;
    MagFilter = LINEAR;
    MinFilter = LINEAR;
    ADDRESSU = WRAP;
    ADDRESSV = WRAP;
};
texture tMask;    //This texture should use point sampling
sampler2D SamplerMask = sampler_state
{
    Texture = <tMask>;
    MipFilter = POINT;
    MinFilter = POINT;
    MagFilter = POINT;
    ADDRESSU = WRAP;
    ADDRESSV = WRAP;
};

struct VS_INPUT
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
};
struct VS_INPUT_DETAIL
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float blendindex: TEXCOORD0;
};

struct VS_OUTPUT_TEX
{
    float4 pos : POSITION;
    float4 diff : COLOR0;
    float4 specular : COLOR1;
    float3 texcbase_fFog : TEXCOORD0; 
    float2 textcSelect : TEXCOORD2; 
};

struct VS_OUTPUT_DETAIL4
{
   // float4 diff : COLOR0;
#if SHADER_MODEL==3
    float4 specular : COLOR0;
#else
	float4 diff     : COLOR0;
#endif
    float4 texcbase : TEXCOORD0; 
    float4 detailblend1 : TEXCOORD1;
    float4 texcdetail01 : TEXCOORD2;  
    float4 texcdetail23 : TEXCOORD3;  
#if SHADER_MODEL==3
    float4 Normal_fFog : TEXCOORD4;
#else
    float  fFog : TEXCOORD4;
#endif

#if SHADER_MODEL == 2
#if NORMAL_CHECK == 1
	float3 norForNormalChecker : TEXCOORD5;
#endif
#endif
    
    float posToEye : COLOR1; //位置到摄像机的距离 by Likan
};

struct VS_OUTPUT_DETAIL8
{
    //float4 diff : COLOR0;
#if SHADER_MODEL==3
    float4 specular : COLOR0;
#else
	float4 diff     : COLOR0;
#endif
    float4 texcbase : TEXCOORD0; 
    float4 detailblend1 : TEXCOORD1;
    float4 detailblend2 : TEXCOORD2;
    float4 texcdetail01 : TEXCOORD3;  
    float4 texcdetail23 : TEXCOORD4;  
    float4 texcdetail45 : TEXCOORD5;  
    float4 texcdetail67 : TEXCOORD6;
#if SHADER_MODEL==3
    float4 Normal_fFog : TEXCOORD7;
#else
	#if NORMAL_CHECK != 1
    float  fFog : TEXCOORD7;
	#endif
#endif

#if SHADER_MODEL == 2
#if NORMAL_CHECK == 1
	float3 fFog : TEXCOORD7;
#endif
#endif

float posToEye : COLOR1; //位置到摄像机的距离 by Likan
};

struct VS_OUTPUT_Z
{
    float4 pos   : POSITION;
    float2 rawpos : TEXCOORD0;
};

float4 LightCac(float3 Normal)
{
	float k = max(0,dot(Normal,-sunLight.m_dir));
    return k;
}

float2 TexUVCalc(float3 Pos,int n)
{
    float2 Out;
   int m = n+1;
    Out.x = Pos.x*UVT[n].x + Pos.y*UVT[n].y + Pos.z*UVT[n].z;
   Out.y = Pos.x*UVT[m].x + Pos.y*UVT[m].y + Pos.z*UVT[m].z;
   return Out;
}

VS_OUTPUT_Z VS_ZBUFFER(VS_INPUT In)
{
    VS_OUTPUT_Z Out;
    Out.pos = float4(mul(float4(In.pos,1.0),matWVP));
    Out.rawpos.x = Out.pos.z;
    Out.rawpos.y = Out.pos.w;
    return Out;
}

VS_OUTPUT_Z VS_ZBUFFER_MRT(VS_INPUT In)
{
    VS_OUTPUT_Z Out = (VS_OUTPUT_Z)0;
    Out .pos = float4(mul(float4(In.pos,1.0),matWVP));
    Out.rawpos.x = Out.pos.z;
    Out.rawpos.y = Out.pos.w;
    return Out ;
}

struct PS_OUT_z
{
       float4 color0: COLOR0 ;
};

PS_OUT_z PS_Z(float2 rawpos : TEXCOORD0)
{
     PS_OUT_z Out;
     Out.color0 = PackingFloatToRGB(rawpos.x / rawpos.y);
     return Out;
}

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

VS_OUTPUT_TEX VS(VS_INPUT input)
{
    VS_OUTPUT_TEX output = (VS_OUTPUT_TEX)0;
    
    output.pos = float4(mul(float4(input.pos,1.0),matWVP));
    output.diff = LightCac(input.normal);
    float2 PosInRect;

    PosInRect.x = (input.pos.x - vConverRect.x)/vConverRect.z;
    PosInRect.y = (input.pos.z - vConverRect.y)/vConverRect.w;

    output.texcbase_fFog.xy = PosInRect;
    output.specular = 0;
    
    float3 R   =  normalize(2 * output.diff.r * input.normal + sunLight.m_dir);
    float3 eye = normalize(eyes - input.pos);
    output.diff.g = sunLight.m_specular * pow(max(0, dot(R, eye)), materialEx.spe_exp) * materialCur.specPower;  

    output.texcbase_fFog.z = CalFogFactor(output.pos.z);

    // project texture
    output.textcSelect = calc_proj_uv(input.pos);

    return output;
}
void VS_MASK(float3 pos : POSITION
            , out float4 oPos : POSITION
            , out float2 texCoord : TEXCOORD0)
{
    oPos = float4(mul(float4(pos,1.0),matWVP));
    float2 tCoord;
    tCoord.x = (pos.x - vSceneRect.x)/vSceneRect.z;
    tCoord.y = 1 - (pos.z - vSceneRect.y)/vSceneRect.w;
    texCoord = tCoord;
}
float4 PS_MASK(float2 texCoord : TEXCOORD0) : COLOR
{
    return tex2D(SamplerMask, texCoord);
}

float4 GetBlendFactor(int Index)
{
    float4 blendfactor = {0,0,0,0};
    if(Index >= 0 )
        {  if(Index < 0.5 )
          blendfactor = float4(1,0,0,0);
   else if(Index < 1.5)
         blendfactor = float4(0,1,0,0);
    else if(Index < 2.5)
           blendfactor = float4(0,0,1,0);
     else if(Index < 3.5)
        blendfactor = float4(0,0,0,1);
    }
return blendfactor ;
}
VS_OUTPUT_DETAIL4 VS_DETAIL_4(VS_INPUT_DETAIL input
			, out float4 outPos : POSITION)
{
    VS_OUTPUT_DETAIL4 output = (VS_OUTPUT_DETAIL4)0;
    
    outPos = mul(float4(input.pos,1.0),matWVP);
    
    output.texcbase.xy = ConverUV(input.pos.x,input.pos.z);
   
    output.detailblend1 = GetBlendFactor(input.blendindex );
    output.texcdetail01.xy = TexUVCalc(input.pos,0);
    output.texcdetail01.zw = TexUVCalc(input.pos,2);
    output.texcdetail23.xy = TexUVCalc(input.pos,4);
    output.texcdetail23.zw = TexUVCalc(input.pos,6);
    output.posToEye = length(eyes - input.pos); // by Likan

    float3 nor = input.normal;
#if SHADER_MODEL==3
   // output.diff = float4(input.normal,0);
    output.Normal_fFog.xyz  = input.normal;
    output.specular = float4(input.pos,1);
    output.Normal_fFog.w = CalFogFactor(outPos.z);
#else        
    float comlight = max(0, dot(sunLight.m_dir, nor));
    float light    = max(0, dot(-sunLight.m_dir, nor));
    float skylight = max(0, dot(-sky_light_dir, nor));  
    output.diff = (light * sunLight.m_diffuse + comlight * sunLight.m_com_light + skylight * sunLight.m_sky_light) + sunLight.m_sceneAmbient;
    output.fFog = CalFogFactor(outPos.z);
#endif

    // project texture
    output.texcbase.zw = calc_proj_uv(input.pos);

#if SHADER_MODEL == 2
#if NORMAL_CHECK == 1
	output.norForNormalChecker = normalCheckVS(input.normal, matWorld);
#endif
#endif
        
    return output;
}
VS_OUTPUT_DETAIL8 VS_DETAIL_8(VS_INPUT_DETAIL input
			, out float4 outPos : POSITION)
{
    VS_OUTPUT_DETAIL8 output = (VS_OUTPUT_DETAIL8)0;
    
    outPos = float4(mul(float4(input.pos,1.0),matWVP));
    
    output.texcbase.xy = ConverUV(input.pos.x,input.pos.z);
   
    output.detailblend1 = GetBlendFactor(input.blendindex );//input.blend0;
    output.detailblend2 = GetBlendFactor(input.blendindex -4 );//input.blend1;
    output.texcdetail01.xy = TexUVCalc(input.pos,0);
    output.texcdetail01.zw = TexUVCalc(input.pos,2);
    output.texcdetail23.xy = TexUVCalc(input.pos,4);
    output.texcdetail23.zw = TexUVCalc(input.pos,6);
    output.texcdetail45.xy = TexUVCalc(input.pos,8);
    output.texcdetail45.zw = TexUVCalc(input.pos,10);
    output.texcdetail67.xy = TexUVCalc(input.pos,12);
    output.texcdetail67.zw = TexUVCalc(input.pos,14);
    output.posToEye = length(eyes - input.pos); // by Likan

    float3 nor = input.normal;
#if SHADER_MODEL==3
    //output.diff = float4(input.normal,0);
    output.Normal_fFog.xyz  = input.normal;
    output.specular = float4(input.pos,1);
    output.Normal_fFog.w = CalFogFactor(outPos.z);
#else
    float comlight = max(0, dot(sunLight.m_dir, nor));
    float light    = max(0, dot(-sunLight.m_dir, nor));
    float skylight = max(0, dot(-sky_light_dir, nor));  
    output.diff = (light * sunLight.m_diffuse + comlight * sunLight.m_com_light + skylight * sunLight.m_sky_light) + sunLight.m_sceneAmbient;  
    output.fFog = CalFogFactor(outPos.z);
#endif        

    // project texture
    output.texcbase.zw = calc_proj_uv(input.pos);

#if SHADER_MODEL == 2
#if NORMAL_CHECK == 1
	output.fFog = normalCheckVS(input.normal, matWorld);
#endif
#endif
        
    return output;
}

float4 PS_Blend(VS_OUTPUT_TEX In) : COLOR
{
   float4 base = tex2D(SamplerConver ,In.texcbase_fFog.xyz);
   return base;
}

technique  tecZ
{
    pass Pass0
    {
        VertexShader = compile vs_2_0 VS_ZBUFFER();
        PixelShader = compile ps_2_0 PS_Z();
    }
}

technique tecZ_MRT
{
    pass Pass0
    {
        VertexShader = compile vs_2_0 VS_ZBUFFER_MRT();
        PixelShader = compile ps_2_0 PS_Z();
    }
}

technique tec0
{
    pass Pass0
    {
        VertexShader = compile vs_2_0 VS();
        PixelShader = compile ps_2_0 PS_Blend();
		FogEnable = False;	//Fog is calculated in the shader, do not use the d3d fog, it affects the final effect
    }  
    pass Pass1
    {
        VertexShader = compile vs_1_1 VS_MASK();
        PixelShader = compile ps_2_0 PS_MASK();
		FogEnable = False;	//Fog is calculated in the shader, do not use the d3d fog, it affects the final effect
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
#if SHADER_MODEL==3
float4 PS_PointLight(float4 diff,float3 normal,float3 pos)
{
    float4 light = diff; 
    
	light.rgb += CalPointLight(normal, pos);
    return light;
}
float4 PS_LightCalc(float4 diff,float4 color,float4 base,float4 mtl,float3 normal,float3 pos)
{
   float4 light = PS_PointLight(diff,normal,pos);    
   float4 o = lerp(color * base * light *3,color,color.a * mtl.z);//+envmap;
     return o;
}
#else
float4 PS_LightCalc(float4 diff,float4 color,float4 base,float4 mtl)
{
   float4 o = lerp(color * base * diff *3,color,color.a * mtl.z);
   return o;
}
#endif
float4 PS_DETAILBLEND_4(VS_OUTPUT_DETAIL4 In) : COLOR
{
   float4 colorOut;
   float4 base = tex2D(SamplerConver ,In.texcbase.xy);
   float4 detail0 = tex2D(s0,In.texcdetail01.xy );
   float4 detail1 = tex2D(s1,In.texcdetail01.zw );
   float4 detail2 = tex2D(s2,In.texcdetail23.xy );
   float4 detail3 = tex2D(s3,In.texcdetail23.zw );


   float4 color = (In.detailblend1.x * detail0 + 
                  In.detailblend1.y * detail1 +
                  In.detailblend1.z * detail2 +  
                  In.detailblend1.w * detail3);

   // by likan
   if(blendHighNLow)
   {
     float2 texUV = In.texcbase.xy; 
     texUV.x = 1.0f - In.texcbase.x;
     float4 colorTerrainMap = tex2D(SamplerTerrainMap, texUV.xy );
     float lp = min(In.posToEye / farsee, 1);
     color = color * (1 - lp)  + colorTerrainMap * lp; 
   }


   float4 mtl   = In.detailblend1.x * MtlFactor[0] + 
                  In.detailblend1.y * MtlFactor[1] +
                  In.detailblend1.z * MtlFactor[2] +  
                  In.detailblend1.w * MtlFactor[3];
#if SHADER_MODEL==3
   float4 screenpos = mul(In.specular,matWVP);
   screenpos.xyz /= screenpos.w;
   screenpos.x = screenpos.x * 0.5 + 0.5;
   screenpos.y = screenpos.y * (-0.5) + 0.5;
   float shadowmask = 1.0f;
   if (shadowMapParam.bShadowMask)
	   shadowmask = GetShadowMask(screenpos.xy);

   //float lumvalue = dot(color.xyz, LUMINANCE_VECTOR) + 0.0001f;
   //lumvalue = lerp(1,lumvalue,sharpValue);
   //shadowmask *= lumvalue;
   
   float comlight = max(0, dot(sunLight.m_dir, In.Normal_fFog.xyz));
   float light    = max(0, dot(-sunLight.m_dir, In.Normal_fFog.xyz));
   float skylight = max(0, dot(-sky_light_dir, In.Normal_fFog.xyz)); 
#if SPECULARENABLE == 1 
   float3 wpos = mul(In.specular, matWorld);
 
   float4 specLight = sunLight.m_specular * computerSpecLight(eyes - wpos,In.Normal_fFog.xyz,-sunLight.m_dir,60);

   ////specLight   *= mtl.y;
   specLight *= (detail0.a * MtlFactor[0].y * In.detailblend1.x + detail1.a * MtlFactor[1].y * In.detailblend1.y + detail2.a * MtlFactor[2].y * In.detailblend1.z + detail3.a * MtlFactor[3].y * In.detailblend1.w);
#endif
   float4 envLight = texCUBE(SameplerEnv, In.Normal_fFog.xyz ) * mtl.x;
   float4 emsLight = color * mtl.z * shadowmask;
    
   float4 diff = (light * sunLight.m_diffuse + comlight * sunLight.m_com_light + skylight * sunLight.m_sky_light) * shadowmask + sunLight.m_sceneAmbient;  
   colorOut =  PS_LightCalc(diff,color,base,mtl,In.Normal_fFog.xyz,In.specular.xyz) + envLight + emsLight; 
#if SPECULARENABLE == 1 
   colorOut += specLight * shadowmask;
#endif  

#else
   colorOut = PS_LightCalc(In.diff ,color,base,mtl);
#endif
   // project texture
   if (In.texcbase.z >= 0 && In.texcbase.z < 1 && In.texcbase.w >= 0 && In.texcbase.w < 1)
   {
        float4 aoe = tex2D(SamplerSelectTexture, In.texcbase.zw);
        colorOut = colorOut * (1.f - aoe.a) + aoe * aoe.a; 
   }

#if SHADER_MODEL==3
   colorOut = CalFogColor(colorOut, In.Normal_fFog.w);
#else
   colorOut = CalFogColor(colorOut,In.fFog);
#endif

   colorOut.a =1;

#if SHADER_MODEL == 2
#if NORMAL_CHECK == 1
   colorOut.rgb = In.norForNormalChecker;
#endif
#endif

   return colorOut;
}
float4 PS_DETAILBLEND_8(VS_OUTPUT_DETAIL8 In) : COLOR
{
	float4 colorOut;
   float4 base = tex2D(SamplerConver ,In.texcbase.xy);
   float4 detail0 = tex2D(s0,In.texcdetail01.xy );
   float4 detail1 = tex2D(s1,In.texcdetail01.zw );
   float4 detail2 = tex2D(s2,In.texcdetail23.xy );
   float4 detail3 = tex2D(s3,In.texcdetail23.zw );
   float4 detail4 = tex2D(s4,In.texcdetail45.xy );
   float4 detail5 = tex2D(s5,In.texcdetail45.zw );
   float4 detail6 = tex2D(s6,In.texcdetail67.xy );
   float4 detail7 = tex2D(s7,In.texcdetail67.zw );

   float4 color = In.detailblend1.x * detail0 + 
                  In.detailblend1.y * detail1 +
                  In.detailblend1.z * detail2 +  
                  In.detailblend1.w * detail3 +
                  In.detailblend2.x * detail4 + 
                  In.detailblend2.y * detail5 +
                  In.detailblend2.z * detail6 +  
                  In.detailblend2.w * detail7 ;

   // by likan
   if(blendHighNLow)
   {
     float2 texUV = In.texcbase.xy; 
     texUV.x = 1.0f - In.texcbase.x;
     float4 colorTerrainMap = tex2D(SamplerTerrainMap, texUV.xy );
     float lp = min(In.posToEye / farsee, 1);
     color = color * (1 - lp)  + colorTerrainMap * lp; 
   }

   float4 mtl   = In.detailblend1.x * MtlFactor[0] + 
                  In.detailblend1.y * MtlFactor[1] +
                  In.detailblend1.z * MtlFactor[2] +  
                  In.detailblend1.w * MtlFactor[3] +
                  In.detailblend2.x * MtlFactor[4] +
                  In.detailblend2.y * MtlFactor[5] +
                  In.detailblend2.z * MtlFactor[6] +  
                  In.detailblend2.w * MtlFactor[7] ;
                  
#if SHADER_MODEL==3
   float4 screenpos = mul(In.specular,matWVP);
   screenpos.xyz /= screenpos.w;
   screenpos.x = screenpos.x * 0.5 + 0.5;
   screenpos.y = screenpos.y * (-0.5) + 0.5;
   float shadowmask = 1.0f;
   if (shadowMapParam.bShadowMask)
	   shadowmask = GetShadowMask(screenpos.xy);
	   
   //float lumvalue = dot(color.xyz, LUMINANCE_VECTOR) + 0.0001f;
   //lumvalue = lerp(1,lumvalue,sharpValue);
   //shadowmask *= lumvalue;
   
   float comlight = max(0, dot(sunLight.m_dir, In.Normal_fFog.xyz));
   float light    = max(0, dot(-sunLight.m_dir, In.Normal_fFog.xyz));
   float skylight = max(0, dot(-sky_light_dir, In.Normal_fFog.xyz)); 
#if SPECULARENABLE == 1    
   float3 wpos = mul(In.specular, matWorld);
   float4 specLight    = sunLight.m_specular * computerSpecLight(eyes - wpos,In.Normal_fFog.xyz,-sunLight.m_dir,60);
   //specLight   *= mtl.y;
   specLight *= (detail0.a * MtlFactor[0].y * In.detailblend1.x + detail1.a * MtlFactor[1].y * In.detailblend1.y + detail2.a * MtlFactor[2].y * In.detailblend1.z + detail3.a * MtlFactor[3].y * In.detailblend1.w
	             + detail4.a * MtlFactor[4].y * In.detailblend2.x + detail5.a * MtlFactor[5].y * In.detailblend2.y + detail6.a * MtlFactor[6].y * In.detailblend2.z +detail7.a * MtlFactor[7].y * In.detailblend2.w);
#endif
   float4 envLight = texCUBE(SameplerEnv, In.Normal_fFog.xyz ) * mtl.x;
   float4 emsLight = color * mtl.z * shadowmask;
	 
   float4 diff = (light * sunLight.m_diffuse + comlight * sunLight.m_com_light + skylight * sunLight.m_sky_light) * shadowmask + sunLight.m_sceneAmbient;  
   colorOut =  PS_LightCalc(diff,color,base,mtl,In.Normal_fFog.xyz,In.specular.xyz) + envLight + emsLight;// + specLight * shadowmask;  
#if SPECULARENABLE == 1
   colorOut += specLight * shadowmask; 
#endif

#else
   colorOut = PS_LightCalc(In.diff ,color,base,mtl);
#endif
   // project texture
   if (In.texcbase.z >= 0 && In.texcbase.z < 1 && In.texcbase.w >= 0 && In.texcbase.w < 1)
   {
	   float4 aoe = tex2D(SamplerSelectTexture, In.texcbase.zw);
	   colorOut = colorOut * (1.f - aoe.a) + aoe * aoe.a; 
   }

#if SHADER_MODEL==3
   colorOut = CalFogColor(colorOut, In.Normal_fFog.w);
#else
   colorOut = CalFogColor(colorOut,In.fFog.r);
#endif
   
   colorOut.a =1;

#if SHADER_MODEL == 2
#if NORMAL_CHECK == 1
   colorOut.rgb = In.fFog;
#endif
#endif


   return colorOut;
}


technique DetailBlend
{
	pass Pass0
	{
		VertexShader = compile VS_PROFILE VS_DETAIL_4();
		PixelShader = compile PS_PROFILE PS_DETAILBLEND_4();
		FogEnable = False;	//Fog is calculated in the shader, do not use the d3d fog, it affects the final effect
	}  
	pass Pass1
	{
		VertexShader = compile VS_PROFILE VS_DETAIL_8();
		PixelShader = compile PS_PROFILE PS_DETAILBLEND_8();
		FogEnable = False;	//Fog is calculated in the shader, do not use the d3d fog, it affects the final effect
	}
}


/////////////


struct VS_OUTPUT_TEXTURE4
{
	float4 outPos : POSITION;
	float4 detailblend1 : TEXCOORD0;
	float4 texcdetail01 : TEXCOORD1;  
	float4 texcdetail23 : TEXCOORD2;
};

struct VS_OUTPUT_TEXTURE8
{
	float4 outPos : POSITION;
	float4 detailblend1 : TEXCOORD0;
	float4 detailblend2 : TEXCOORD1;
	float4 texcdetail01 : TEXCOORD2;  
	float4 texcdetail23 : TEXCOORD3;  
	float4 texcdetail45 : TEXCOORD4;  
	float4 texcdetail67 : TEXCOORD5;
};

VS_OUTPUT_TEXTURE4 VS_TEXTRUE_4(VS_INPUT_DETAIL input)
{
	VS_OUTPUT_TEXTURE4 output = (VS_OUTPUT_TEXTURE4)0;

	output.outPos = mul(float4(input.pos,1.0),matWVP);

	output.detailblend1 = GetBlendFactor(input.blendindex );
	output.texcdetail01.xy = TexUVCalc(input.pos,0);
	output.texcdetail01.zw = TexUVCalc(input.pos,2);
	output.texcdetail23.xy = TexUVCalc(input.pos,4);
	output.texcdetail23.zw = TexUVCalc(input.pos,6);

	return output;
}
VS_OUTPUT_TEXTURE8 VS_TEXTRUE_8(VS_INPUT_DETAIL input)
{
	VS_OUTPUT_TEXTURE8 output = (VS_OUTPUT_TEXTURE8)0;

	output.outPos = float4(mul(float4(input.pos,1.0),matWVP));

	output.detailblend1 = GetBlendFactor(input.blendindex );//input.blend0;
	output.detailblend2 = GetBlendFactor(input.blendindex -4 );//input.blend1;
	output.texcdetail01.xy = TexUVCalc(input.pos,0);
	output.texcdetail01.zw = TexUVCalc(input.pos,2);
	output.texcdetail23.xy = TexUVCalc(input.pos,4);
	output.texcdetail23.zw = TexUVCalc(input.pos,6);
	output.texcdetail45.xy = TexUVCalc(input.pos,8);
	output.texcdetail45.zw = TexUVCalc(input.pos,10);
	output.texcdetail67.xy = TexUVCalc(input.pos,12);
	output.texcdetail67.zw = TexUVCalc(input.pos,14);
	return output;
}

float4 PS_TEXTRUE_4(VS_OUTPUT_TEXTURE4 In) : COLOR
{
	float4 detail0 = tex2D(s0,In.texcdetail01.xy );
	float4 detail1 = tex2D(s1,In.texcdetail01.zw );
	float4 detail2 = tex2D(s2,In.texcdetail23.xy );
	float4 detail3 = tex2D(s3,In.texcdetail23.zw );

	float4 color = In.detailblend1.x * detail0 + 
		In.detailblend1.y * detail1 +
		In.detailblend1.z * detail2 +  
		In.detailblend1.w * detail3;

	return color;
}
float4 PS_TEXTRUE_8(VS_OUTPUT_TEXTURE8 In) : COLOR
{
	float4 detail0 = tex2D(s0,In.texcdetail01.xy );
	float4 detail1 = tex2D(s1,In.texcdetail01.zw );
	float4 detail2 = tex2D(s2,In.texcdetail23.xy );
	float4 detail3 = tex2D(s3,In.texcdetail23.zw );
	float4 detail4 = tex2D(s4,In.texcdetail45.xy );
	float4 detail5 = tex2D(s5,In.texcdetail45.zw );
	float4 detail6 = tex2D(s6,In.texcdetail67.xy );
	float4 detail7 = tex2D(s7,In.texcdetail67.zw );

	float4 color = In.detailblend1.x * detail0 + 
		In.detailblend1.y * detail1 +
		In.detailblend1.z * detail2 +  
		In.detailblend1.w * detail3 +
		In.detailblend2.x * detail4 + 
		In.detailblend2.y * detail5 +
		In.detailblend2.z * detail6 +  
		In.detailblend2.w * detail7 ;
	return color;
}


technique RenderTexture
{
	pass Pass0
	{
		VertexShader = compile VS_PROFILE VS_TEXTRUE_4();
		PixelShader = compile PS_PROFILE PS_TEXTRUE_4();
		FogEnable = False;	//Fog is calculated in the shader, do not use the d3d fog, it affects the final effect
	}  
	pass Pass1
	{
		VertexShader = compile VS_PROFILE VS_TEXTRUE_8();
		PixelShader = compile PS_PROFILE PS_TEXTRUE_8();
		FogEnable = False;	//Fog is calculated in the shader, do not use the d3d fog, it affects the final effect
	}
}



struct VS_OUTPUT_DETAIL_TEX
{
	float4 diff : COLOR0;
#if SHADER_MODEL==3
	float4 specular : COLOR1;
#endif
	float4 texcbase : TEXCOORD0; 
#if SHADER_MODEL==3
	float4 Normal_fFog : TEXCOORD1;
#else
	float  fFog : TEXCOORD1;
#endif

#if SHADER_MODEL == 2
#if NORMAL_CHECK == 1
	float3 norForNormalChecker : TEXCOORD2;
#endif
#endif
};

VS_OUTPUT_DETAIL_TEX VS_DETAIL_TEX(VS_INPUT_DETAIL input , out float4 outPos : POSITION)
{
	VS_OUTPUT_DETAIL_TEX output = (VS_OUTPUT_DETAIL_TEX)0;

	outPos = mul(float4(input.pos,1.0),matWVP);

	output.texcbase.xy = ConverUV(input.pos.x,input.pos.z);

	float3 R   =  normalize(2 * output.diff.r * input.normal + sunLight.m_dir);
	float3 eye = normalize(eyes - input.pos);

	float3 nor = input.normal;
#if SHADER_MODEL==3
	output.diff = float4(input.normal,0);
	output.Normal_fFog.xyz  = input.normal;
	output.specular = float4(input.pos,1);
	output.Normal_fFog.w = CalFogFactor(outPos.z);
#else        
	float comlight = max(0, dot(sunLight.m_dir, nor));
	float light    = max(0, dot(-sunLight.m_dir, nor));
	float skylight = max(0, dot(-sky_light_dir, nor));  
	output.diff = (light * sunLight.m_diffuse + comlight * sunLight.m_com_light + skylight * sunLight.m_sky_light) + sunLight.m_sceneAmbient;
	output.fFog = CalFogFactor(outPos.z);
#endif

	// project texture
	output.texcbase.zw = calc_proj_uv(input.pos);

#if SHADER_MODEL == 2
#if NORMAL_CHECK == 1
	output.norForNormalChecker = normalCheckVS(input.normal, matWorld);
#endif
#endif

	return output;
}


float4 PS_DETAIL_TEX(VS_OUTPUT_DETAIL_TEX In) : COLOR
{
	float4 colorOut;
	float4 base = tex2D(SamplerConver ,In.texcbase.xy);
	float2 texUV = In.texcbase.xy;
	texUV.x = 1.0f - In.texcbase.x;
	float4 color = tex2D(s0,texUV.xy );
	float4 mtl = float4(0,0,0,0);
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

	float comlight = max(0, dot(sunLight.m_dir, In.diff.xyz));
	float light    = max(0, dot(-sunLight.m_dir, In.diff.xyz));
	float skylight = max(0, dot(-sky_light_dir, In.diff.xyz)); 

	float3 wpos = mul(In.specular, matWorld);
 	float4 specLight    = 0;//sunLight.m_specular * computerSpecLight(eyes - wpos,In.diff.xyz,-sunLight.m_dir,60);
	//specLight   *= mtl.y;
	//specLight *= (color.a * ( MtlFactor[0].y * In.detailblend1.x + MtlFactor[1].y * In.detailblend1.y + MtlFactor[2].y * In.detailblend1.z + MtlFactor[3].y * In.detailblend1.w
		//+ MtlFactor[4].y * In.detailblend2.x + MtlFactor[5].y * In.detailblend2.y + MtlFactor[6].y * In.detailblend2.z +MtlFactor[7].y * In.detailblend2.w));
	float4 envLight = 0;//texCUBE(SameplerEnv, In.Normal_fFog.xyz ) * mtl.x;
	float4 emsLight = 0;//color * mtl.z * shadowmask;

	float4 diff = (light * sunLight.m_diffuse + comlight * sunLight.m_com_light + skylight * sunLight.m_sky_light) * shadowmask + sunLight.m_sceneAmbient;  
	colorOut =  PS_LightCalc(diff,color,base,mtl,In.Normal_fFog.xyz,In.specular.xyz) + envLight + emsLight + specLight * shadowmask;                        
#else
	colorOut = PS_LightCalc(In.diff ,color,base,mtl);
#endif
	// project texture
	if (In.texcbase.z >= 0 && In.texcbase.z < 1 && In.texcbase.w >= 0 && In.texcbase.w < 1)
	{
		float4 aoe = tex2D(SamplerSelectTexture, In.texcbase.zw);
		colorOut = colorOut * (1.f - aoe.a) + aoe * aoe.a; 
	}

#if SHADER_MODEL==3
	colorOut = CalFogColor(colorOut, In.Normal_fFog.w);
#else
	colorOut = CalFogColor(colorOut,In.fFog.r);
#endif

	colorOut.a =1;

#if SHADER_MODEL == 2
#if NORMAL_CHECK == 1
	colorOut.rgb = In.fFog;
#endif
#endif
	return colorOut;
}

technique OneMap
{
	pass Pass0
	{
		VertexShader = compile VS_PROFILE VS_DETAIL_TEX();
		PixelShader = compile PS_PROFILE PS_DETAIL_TEX();
		FogEnable = False;	//Fog is calculated in the shader, do not use the d3d fog, it affects the final effect
	}  
}