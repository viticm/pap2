#include "CommonParams.h"
#include "PointLight.h"


float4x4 matWorld : WORLDMATIRX;
float4x4 matWVP : PROJECTION;

///////////////////////////////////////////////


float4 UVT[16];
float4 MtlFactor[8];
float4 vSceneRect = {0,0,12800,12800};


texture tDetail0;
texture tDetail1;
texture tDetail2;
texture tDetail3;
texture tDetail4;
texture tDetail5;
texture tDetail6;
texture tDetail7;
texture SelTexture;

texture tDetailNormal0;
texture tDetailNormal1;
texture tDetailNormal2;
texture tDetailNormal3;
texture tDetailNormal4;
texture tDetailNormal5;
texture tDetailNormal6;
texture tDetailNormal7;

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



sampler2D SamplerDetail0 = sampler_state
{
    Texture   = <tDetail0>;
    MipFilter = LINEAR;
    MagFilter = LINEAR;
    MinFilter = LINEAR;
    ADDRESSU = WRAP;
    ADDRESSV = WRAP;
};
sampler2D SamplerDetail1 = sampler_state
{
    Texture   = <tDetail1>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    ADDRESSU = WRAP;
    ADDRESSV = WRAP;
};
sampler2D SamplerDetail2 = sampler_state
{
    Texture   = <tDetail2>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    ADDRESSU = WRAP;
    ADDRESSV = WRAP;
};
sampler2D SamplerDetail3 = sampler_state
{
    Texture   = <tDetail3>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    ADDRESSU = WRAP;
    ADDRESSV = WRAP;
};

sampler2D SamplerDetail4 = sampler_state
{
    Texture   = <tDetail4>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    ADDRESSU = WRAP;
    ADDRESSV = WRAP;
};
sampler2D SamplerDetail5 = sampler_state
{
    Texture   = <tDetail5>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    ADDRESSU = WRAP;
    ADDRESSV = WRAP;
};
sampler2D SamplerDetail6 = sampler_state
{
    Texture   = <tDetail6>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    ADDRESSU = WRAP;
    ADDRESSV = WRAP;
};
sampler2D SamplerDetail7 = sampler_state
{
    Texture   = <tDetail7>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    ADDRESSU = WRAP;
    ADDRESSV = WRAP;
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
sampler2D SamplerDetailNormal1 = sampler_state
{
    Texture   = <tDetailNormal1>;
    MipFilter = LINEAR;
    MagFilter = LINEAR;
    MinFilter = LINEAR;
    ADDRESSU = WRAP;
    ADDRESSV = WRAP;
};
sampler2D SamplerDetailNormal2 = sampler_state
{
    Texture   = <tDetailNormal2>;
    MipFilter = LINEAR;
    MagFilter = LINEAR;
    MinFilter = LINEAR;
    ADDRESSU = WRAP;
    ADDRESSV = WRAP;
};
sampler2D SamplerDetailNormal3 = sampler_state
{
    Texture   = <tDetailNormal3>;
    MipFilter = LINEAR;
    MagFilter = LINEAR;
    MinFilter = LINEAR;
    ADDRESSU = WRAP;
    ADDRESSV = WRAP;
};
sampler2D SamplerDetailNormal4 = sampler_state
{
    Texture   = <tDetailNormal4>;
    MipFilter = LINEAR;
    MagFilter = LINEAR;
    MinFilter = LINEAR;
    ADDRESSU = WRAP;
    ADDRESSV = WRAP;
};
sampler2D SamplerDetailNormal5 = sampler_state
{
    Texture   = <tDetailNormal5>;
    MipFilter = LINEAR;
    MagFilter = LINEAR;
    MinFilter = LINEAR;
    ADDRESSU = WRAP;
    ADDRESSV = WRAP;
};
sampler2D SamplerDetailNormal6 = sampler_state
{
    Texture   = <tDetailNormal6>;
    MipFilter = LINEAR;
    MagFilter = LINEAR;
    MinFilter = LINEAR;
    ADDRESSU = WRAP;
    ADDRESSV = WRAP;
};
sampler2D SamplerDetailNormal7 = sampler_state
{
    Texture   = <tDetailNormal7>;
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
    float blendindex : TEXCOORD0;
   // float2 textcSelect : TEXCOORD1; 
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
   float4 pos : POSITION;
   float4 diff : COLOR0;
   float4 specular : COLOR1;
   float4 texcbase : TEXCOORD0; 
   float4 detailblend1 : TEXCOORD1;
  // float4 detailblend2 : TEXCOORD2;
   float4 texcdetail01 : TEXCOORD2;  
   float4 texcdetail23 : TEXCOORD3;  
   float4 Normal_fFog : TEXCOORD4;
   // float2 textcSelect : TEXCOORD7; use texcbase.zw
};

struct VS_OUTPUT_DETAIL8
{
    float4 diff : COLOR0;
    float4 specular : COLOR1;
    float4 texcbase : TEXCOORD0; 
    float4 detailblend1 : TEXCOORD1;
    float4 detailblend2 : TEXCOORD2;
    float4 texcdetail01 : TEXCOORD3;  
    float4 texcdetail23 : TEXCOORD4;  
    float4 texcdetail45 : TEXCOORD5;  
    float4 texcdetail67 : TEXCOORD6;
    float4 Normal_fFog : TEXCOORD7;
    float4 pos : POSITION;
   // float2 textcSelect : TEXCOORD7; use texcbase.zw
};

struct PS_OUTPUT
{
    float4 color : COLOR;
};

struct VS_OUTPUT_Z
{
    float4 pos   : POSITION;
    float  depth : TEXCOORD0;
};

float4 LightCac(float3 Normal)
{
     float k = max(0,dot(Normal,-sunLight.m_dir));//*0.5+0.5;
       return k;//LightDiffuse * k  + LightAmbient ;//lerp(LightAmbient,LightDiffuse,k);     
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
    Out.depth = Out.pos.z / Out.pos.w;
    return Out;
}

VS_OUTPUT_Z VS_ZBUFFER_MRT(VS_INPUT In)
{
    VS_OUTPUT_Z Out = (VS_OUTPUT_Z)0;
        Out .pos = float4(mul(float4(In.pos,1.0),matWVP));
    Out .depth = Out .pos.z / Out .pos.w;
        return Out ;
}

struct PS_OUT_z
{
       float4 color0: COLOR0 ;
};

PS_OUT_z PS_Z(float depth : TEXCOORD0)
{
     PS_OUT_z Out;
     Out.color0 = depth;
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
VS_OUTPUT_DETAIL4 VS_DETAIL_4(VS_INPUT_DETAIL input)
{
    
    VS_OUTPUT_DETAIL4 output = (VS_OUTPUT_DETAIL4)0;
    
    output.pos = float4(mul(float4(input.pos,1.0),matWVP));
    //output.diff = LightCac(input.normal);

    output.texcbase.xy = ConverUV(input.pos.x,input.pos.z);
   
    output.detailblend1 = GetBlendFactor(input.blendindex );//input.blend0;
    output.texcdetail01.xy = TexUVCalc(input.pos,0);
    output.texcdetail01.zw = TexUVCalc(input.pos,2);
    output.texcdetail23.xy = TexUVCalc(input.pos,4);
    output.texcdetail23.zw = TexUVCalc(input.pos,6);
   
    float3 R   =  normalize(2 * output.diff.r * input.normal + sunLight.m_dir);
    float3 eye = normalize(eyes - input.pos);
    float3 nor = input.normal;
    float comlight = max(0, dot(sunLight.m_dir, nor));
    float light    = max(0, dot(-sunLight.m_dir, nor));
    float skylight = max(0, dot(-sky_light_dir, nor));  
    output.diff = (light * sunLight.m_diffuse + comlight * sunLight.m_com_light + skylight * sunLight.m_sky_light) + sunLight.m_sceneAmbient;  
    output.Normal_fFog.xyz  = input.normal;
    output.specular = float4(input.pos,1);
    output.Normal_fFog.w = CalFogFactor(output.pos.z);

    // project texture
    output.texcbase.zw = calc_proj_uv(input.pos);
    
    float3 tangent = float3(0,-1,0);
    float3 binormal = normalize(cross(input.normal,tangent));
    half3x3 objTangentXf;
    objTangentXf[0] = binormal;
    objTangentXf[1] = tangent;
    objTangentXf[2] = input.normal;
    
    output.diff.rgb = mul(objTangentXf, -sunLight.m_dir);
    output.specular.rgb  = mul(objTangentXf,  eye );

    
    return output;
}

VS_OUTPUT_DETAIL8 VS_DETAIL_8(VS_INPUT_DETAIL input)
{
   VS_OUTPUT_DETAIL8 output = (VS_OUTPUT_DETAIL8)0;
    
    output.pos = float4(mul(float4(input.pos,1.0),matWVP));
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

    float3 R   =  normalize(2 * output.diff.r * input.normal + sunLight.m_dir);
    float3 eye = normalize(eyes - input.pos);
    float3 nor = input.normal;
    float comlight = max(0, dot(sunLight.m_dir, nor));
    float light    = max(0, dot(-sunLight.m_dir, nor));
    float skylight = max(0, dot(-sky_light_dir, nor));  
    output.diff = (light * sunLight.m_diffuse + comlight * sunLight.m_com_light + skylight * sunLight.m_sky_light) + sunLight.m_sceneAmbient;  
    output.Normal_fFog.xyz  = input.normal;
    output.specular = float4(input.pos,1);
    output.Normal_fFog.w = CalFogFactor(output.pos.z);

    // project texture
    output.texcbase.zw = calc_proj_uv(input.pos);
        
    float3 tangent = float3(0,-1,0);
    float3 binormal = normalize(cross(input.normal,tangent));
    half3x3 objTangentXf;
    objTangentXf[0] = binormal;
    objTangentXf[1] = tangent;
    objTangentXf[2] = input.normal;
    
    output.diff.rgb = mul(objTangentXf, -sunLight.m_dir );
    output.specular.rgb  = mul(objTangentXf,  eye );
    return output;
}

PS_OUTPUT PS_Blend(VS_OUTPUT_TEX In)
{
   PS_OUTPUT psout;
   float4 base = tex2D(SamplerConver ,In.texcbase_fFog.xy);
   psout.color = base;
   return psout;
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
        VertexShader = compile vs_2_0  VS();
        PixelShader = compile ps_2_0 PS_Blend();
		FogEnable = False;	//Fog is calculated in the shader, do not use the d3d fog, it affects the final effect
    } 
    pass Pass2
    {
        VertexShader = compile vs_1_1 VS_MASK();
        PixelShader = compile ps_2_0 PS_MASK();
		FogEnable = False;	//Fog is calculated in the shader, do not use the d3d fog, it affects the final effect
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
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

float4 light_default(float3 light,float3 normal,float3 view,float specularfactor)
{
   float  k = max(0,dot(normal,light));
   float3 R =  normalize(2 * k * normal + sunLight.m_dir);
   float4 sp = sunLight.m_specular* pow(max(0, dot(R, view)), materialEx.spe_exp) * materialCur.specPower * specularfactor * 0.1; 
   return sunLight.m_sceneAmbient+sunLight.m_diffuse*k+sp;
}

PS_OUTPUT PS_DETAILBLEND_4(VS_OUTPUT_DETAIL4 In)
{
   PS_OUTPUT psout;
   float4 base = tex2D(SamplerConver ,In.texcbase.xy);
   float4 detail0 = tex2D(SamplerDetail0,In.texcdetail01.xy );
   float4 detail1 = tex2D(SamplerDetail1,In.texcdetail01.zw );
   float4 detail2 = tex2D(SamplerDetail2,In.texcdetail23.xy );
   float4 detail3 = tex2D(SamplerDetail3,In.texcdetail23.zw );
   
   float4 normal0 = tex2D(SamplerDetailNormal0,In.texcdetail01.xy );
   float4 normal1 = tex2D(SamplerDetailNormal1,In.texcdetail01.zw );
   float4 normal2 = tex2D(SamplerDetailNormal2,In.texcdetail23.xy ); 
   float4 normal3 = tex2D(SamplerDetailNormal3,In.texcdetail23.zw ); 
   
   float4 color = In.detailblend1.x * detail0 + 
                     In.detailblend1.y * detail1 +
                  In.detailblend1.z * detail2 + 
                  In.detailblend1.w * detail3;
   float4 mtl   = In.detailblend1.x * MtlFactor[0] + 
                  In.detailblend1.y * MtlFactor[1] +
                  In.detailblend1.z * MtlFactor[2] + 
                  In.detailblend1.w * MtlFactor[3];
   float4 normal = In.detailblend1.x * normal0 + 
                   In.detailblend1.y * normal1 +
                   In.detailblend1.z * normal2 + 
                   In.detailblend1.w * normal3;

   psout.color = light_default(In.diff,normal.rgb*2-1,In.specular,color.a) *color;
   // project texture
   if (In.texcbase.z >= 0 && In.texcbase.z < 1 && In.texcbase.w >= 0 && In.texcbase.w < 1)
   {
	   float4 aoe = tex2D(SamplerSelectTexture, In.texcbase.zw);
	   psout.color = psout.color * (1.f - aoe.a) + aoe * aoe.a; 
   }
	
   psout.color = CalFogColor(psout.color,In.Normal_fFog.w);   
   psout.color.a =1;
   return psout;
}

PS_OUTPUT PS_DETAILBLEND_8(VS_OUTPUT_DETAIL8 In)
{
   PS_OUTPUT psout;
   float4 base = tex2D(SamplerConver ,In.texcbase.xy);
   float4 detail0 = tex2D(SamplerDetail0,In.texcdetail01.xy );
   float4 detail1 = tex2D(SamplerDetail1,In.texcdetail01.zw );
   float4 detail2 = tex2D(SamplerDetail2,In.texcdetail23.xy );
   float4 detail3 = tex2D(SamplerDetail3,In.texcdetail23.zw );
   float4 detail4 = tex2D(SamplerDetail4,In.texcdetail45.xy );
   float4 detail5 = tex2D(SamplerDetail5,In.texcdetail45.zw );
   float4 detail6 = tex2D(SamplerDetail6,In.texcdetail67.xy );
   float4 detail7 = tex2D(SamplerDetail7,In.texcdetail67.zw );
   float4 normal0 = tex2D(SamplerDetailNormal0,In.texcdetail01.xy );
   float4 normal1 = tex2D(SamplerDetailNormal1,In.texcdetail01.zw );
   float4 normal2 = tex2D(SamplerDetailNormal2,In.texcdetail23.xy ); 
   float4 normal3 = tex2D(SamplerDetailNormal3,In.texcdetail23.zw ); 
   float4 normal4 = tex2D(SamplerDetailNormal4,In.texcdetail45.xy ); 
   float4 normal5 = tex2D(SamplerDetailNormal5,In.texcdetail45.zw ); 
   float4 normal6 = tex2D(SamplerDetailNormal6,In.texcdetail67.xy ); 
   float4 normal7 = tex2D(SamplerDetailNormal6,In.texcdetail67.zw ); 
   
   float4 color = In.detailblend1.x * detail0 + 
                  In.detailblend1.y * detail1 +
                  In.detailblend1.z * detail2 +  
                  In.detailblend1.w * detail3 +
                  In.detailblend2.x * detail4 + 
                  In.detailblend2.y * detail5 +
                  In.detailblend2.z * detail6 +  
                  In.detailblend2.w * detail7 ;
   float4 mtl   = In.detailblend1.x * MtlFactor[0] + 
                  In.detailblend1.y * MtlFactor[1] +
                  In.detailblend1.z * MtlFactor[2] +  
                  In.detailblend1.w * MtlFactor[3] +
                  In.detailblend2.x * MtlFactor[4] +
                  In.detailblend2.y * MtlFactor[5] +
                  In.detailblend2.z * MtlFactor[6] +  
                  In.detailblend2.w * MtlFactor[7] ;
   float4 normal = In.detailblend1.x * normal0 +
                   In.detailblend1.y * normal1 +
                   In.detailblend1.z * normal2 + 
                   In.detailblend1.w * normal3 + 
                   In.detailblend2.x * normal4 + 
                   In.detailblend2.y * normal5 + 
                   In.detailblend2.z * normal6 +
                   In.detailblend2.w * normal7;
   
   psout.color = light_default(In.diff,normal.rgb*2-1,In.specular,color.a) *color;

   // project texture
   if (In.texcbase.z >= 0 && In.texcbase.z < 1 && In.texcbase.w >= 0 && In.texcbase.w < 1)
   {
	   float4 aoe = tex2D(SamplerSelectTexture, In.texcbase.zw);
	   psout.color = psout.color * (1.f - aoe.a) + aoe * aoe.a; 
   }

   psout.color = CalFogColor(psout.color,In.Normal_fFog.w);
   psout.color.a =1;
   return psout;
}


technique DetailBlend
{
	pass Pass0
	{
		VertexShader = compile vs_3_0 VS_DETAIL_4();
		PixelShader = compile ps_3_0 PS_DETAILBLEND_4();
		FogEnable = False;	//Fog is calculated in the shader, do not use the d3d fog, it affects the final effect
	} 
	pass Pass1
	{
		VertexShader = compile vs_3_0 VS_DETAIL_8();
		PixelShader = compile ps_3_0 PS_DETAILBLEND_8();
		FogEnable = False;	//Fog is calculated in the shader, do not use the d3d fog, it affects the final effect
	}
}


/////////////