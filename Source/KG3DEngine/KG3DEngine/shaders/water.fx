////////////////////////////////////////////////////////////////////////////////
//
//  Author      : ZhaoXinyu (Chiyer)
//  CopyRight   : Kingsoft Corporation All rights Reserved
//  Create Date : 2009-02-20
//  Comment     : water shader
//
////////////////////////////////////////////////////////////////////////////////

#include "CommonParams.h"

sampler s0 : register(s0);
sampler s1 : register(s1);
sampler s2 : register(s2);
sampler s3 : register(s3);
sampler s4 : register(s4);        
sampler s5 : register(s5); 
sampler s6 : register(s6);
sampler s7 : register(s7);
    
float4x4 matWorldViewProj; 
float4x4 matWorldView;    
    
float4 WaterColor = float4(0.f, 0.2f, 0.25f, 1.f);
float4 SpecrColor = float4(0.5f, 0.5f, 0.5f, 1.f);
float4 LightColor = float4(1.f, 0.5f, 0.f, 1.f); 
float4 vWaveParam = float4(0.f, 0.f, 0.f, 0.f);
float4 vTextParam = float4(0.0001f, 0.0001f, 0.f, 0.f);
float4 vWaveParam2 = float4(0.f, 0.f, 0.f, 0.f);
float4 vLightDir  = float4(0.5f, 0.5f, 0.5f, 1.f);
float4 vCameraPos;

half fresnel(half ndtl, half bias, half fpow)
{
  return  max(bias + (1.f - bias) * pow(1.f - ndtl, fpow), 0.f);
}

// ------------------------- lower ---------------------------------

struct vs_out_lower
{
    float4 hpos : POSITION;
    float2 tex0 : TEXCOORD0;
    float4 diff : COLOR0;
    float  fFog  : TEXCOORD1;
};

vs_out_lower vs_lower(float3 pos : POSITION, float4 diff : COLOR0, float2 tex0 : TEXCOORD0)
{
    vs_out_lower o = (vs_out_lower)0;
    
    o.hpos = mul(float4(pos, 1.f), matWorldViewProj);
    o.tex0 = pos.xz * vTextParam.x + vWaveParam.zw;
    o.diff = diff;
               
	o.fFog = CalFogFactor(o.hpos.z);
    
    return o;
}

float4 ps_lower(vs_out_lower pIn, uniform bool bUnder) : COLOR
{
    float4 color = tex2D(s0, pIn.tex0) * WaterColor*5.0;    
    color.a = WaterColor.a * pIn.diff.a;
	color = CalFogColor(color, pIn.fFog);
    return color;
}


// ------------------------- medium ---------------------------------

struct vs_out_medium
{
    float4 hpos     : POSITION;    
    float4 eye     : TEXCOORD0;
    float4 wave0 : TEXCOORD1;
    float3 wave1_fFog : TEXCOORD2;
    float2 wave2 : TEXCOORD3;
    float2 wave3 : TEXCOORD4;        
    float4 spos  : TEXCOORD5;       
    float4 diff : COLOR0;        
};

vs_out_medium vs_medium(float3 pos : POSITION, float4 diff : COLOR0, float2 tex0 : TEXCOORD0)
{
    vs_out_medium o = (vs_out_medium)0;
    
    float2 vtex0;
    float4 hpos = mul(float4(pos, 1.f), (float4x4)matWorldViewProj);
    
    o.hpos = hpos;
    vtex0 = pos.xz * vTextParam.y;
    
    o.wave0.xy = pos.xz * vTextParam.x + vWaveParam.zw;//vtex0.xy + vWaveParam.zw * 2.f;
    o.wave1_fFog.xy = vtex0.xy * 2.f + vWaveParam.zw * 4.f;
    o.wave2.xy = vtex0.xy * 4.f + vWaveParam.zw * 2.f;
    o.wave3.xy = vtex0.xy * 8.f + vWaveParam.zw;  
    o.diff = diff;
    
    o.wave0.zw = hpos.w;
    
    hpos.y = -hpos.y;
    o.spos = float4((hpos.xy + hpos.w) * 0.5f, (-hpos.x + hpos.w) * 0.5f, hpos.w);
    
    o.eye = vCameraPos - float4(pos, 1.f);
    o.eye.xyz = o.eye.xzy;
    
    o.wave1_fFog.z = CalFogFactor(o.hpos.z);
    
    return o;
}


float4 ps_medium(vs_out_medium pIn, uniform bool bUnder) : COLOR
{    
    half3 eye = normalize(pIn.eye);
    half4 color = half4(0.f, 0.f, 0.f, 1.f);
        
    half3 tex1 = tex2D(s0, pIn.wave1_fFog.xy).xyz;
    half3 tex2 = tex2D(s0, pIn.wave2.xy).xyz;
    half3 tex3 = tex2D(s0, pIn.wave3.xy).xyz;
    
    half3 bump = normalize(2.f * (tex1.xyz + tex2.xyz + tex3.xyz) - 3.f); 
    
    half3 texl = bump.xyz * half3(vTextParam.z, vTextParam.z, 1.f);    
    half3 texr = bump.xyz * half3(vWaveParam.x, vWaveParam.x, 1.f);
    
    half4 proj = pIn.spos / pIn.spos.w;
    half4 refl = tex2D(s1, pIn.wave0.xy) * WaterColor * 10;
    half4 refr = tex2D(s2, proj.xy + texr.xy);
    half4 orgr = tex2D(s2, proj.xy);
    
    refr = lerp(orgr, refr, refr.w);
  
    half  scal = saturate(vTextParam.w / pIn.wave0.w);
        
    texl.z *= 0.08;
    texl = normalize(texl);
    
    half3 ldir = -vLightDir;
    
    if (bUnder)
    {
        eye.z = -eye.z;
        half4 litk = lit(dot(texl, ldir.xyz), dot(normalize(eye + ldir.xyz), texl), 100) * pIn.diff.a;
        half3 depth_color = refr.xyz;
        half3 water_color = WaterColor * litk.y + SpecrColor * litk.z + depth_color;
        color.xyz = WaterColor * litk.y + SpecrColor * litk.z + depth_color; 
    }
    else
    {
        half4 litk = lit(dot(texl, ldir.xyz), dot(normalize(eye + ldir.xyz), texl), 100) * pIn.diff.a;  
        half3 depth_color = refr.xyz * scal + (1.f - scal) * WaterColor;// half3(0.f, 0.1f, 0.125f);      
        half3 water_color = WaterColor * litk.y + SpecrColor * litk.z + depth_color;
        color = half4(lerp(water_color.xyz, refl.xyz * water_color.xyz, pIn.diff.a), 1.f); 
    }

	color = CalFogColor(color, pIn.wave1_fFog.z);

    return color;
    
}


// ------------------- high and very high ---------------------------------

struct vs_out_high
{
    float4 hpos  : POSITION;    
    float4 eye   : TEXCOORD0;
    float4 wave0 : TEXCOORD1;
    float3 wave1_fFog : TEXCOORD2;
    float2 wave2 : TEXCOORD3;
    float2 wave3 : TEXCOORD4;        
    float4 spos  : TEXCOORD5;     
    float4 diff  : COLOR0;  
};

vs_out_high vs_high(float3 pos : POSITION, float4 diff : COLOR0, float2 tex0 : TEXCOORD0)
{
    vs_out_high o = (vs_out_high)0;
    
    float2 vtran;
    float2 vtex0;
    float4 hpos = mul(float4(pos, 1.f), (float4x4)matWorldViewProj);
    
    o.hpos = hpos;
    vtex0 = pos.xz * vTextParam.y;
    
    o.wave0.xy = vtex0.xy + vWaveParam.zw * 2.f;
    o.wave1_fFog.xy = vtex0.xy * 2.f + vWaveParam.zw * 4.f;
    o.wave2.xy = vtex0.xy * 4.f + vWaveParam.zw * 2.f;
    o.wave3.xy = vtex0.xy * 8.f + vWaveParam.zw;  
    
    o.wave0.zw = hpos.w;
    o.diff = saturate(diff * 2);
    
    hpos.y = -hpos.y;
    o.spos = float4((hpos.xy + hpos.w) * 0.5f, (-hpos.x + hpos.w) * 0.5f, hpos.w);

    o.eye = vCameraPos - float4(pos, 1.f);
    o.eye.xyz = o.eye.xzy;
    
	o.wave1_fFog.z = CalFogFactor(o.hpos.z);
    
    return o;
}

float4 ps_high(vs_out_high pIn, uniform bool bUnder) : COLOR
{
    half3 eye = normalize(pIn.eye);
    half4 color = half4(0.f, 0.f, 0.f, 1.f);
    
    half3 tex0 = tex2D(s0, pIn.wave0.xy).xyz;
    half3 tex1 = tex2D(s0, pIn.wave1_fFog.xy).xyz;
    half3 tex2 = tex2D(s0, pIn.wave2.xy).xyz;
    half3 tex3 = tex2D(s0, pIn.wave3.xy).xyz;
    
    half3 bump = normalize(2.f * (tex0.xyz + tex1.xyz + tex2.xyz + tex3.xyz) - 4.f);

    half3 texl = bump.xyz * half3(vTextParam.z, vTextParam.z, 1.f);    
    half3 texr = bump.xyz * half3(vWaveParam.x, vWaveParam.x, 1.f);

    half4 proj = pIn.spos / pIn.spos.w;
    half4 wave = tex2D(s3, proj.xy);
    half4 refl = 0;
    half4 refr = tex2D(s2, proj.xy + texr.xy - wave * 0.02f);
    half4 orgr = tex2D(s2, proj.xy);
    
    if (!bUnder)
        refl = tex2D(s1, proj.zy + texl.xy - wave * 0.2f);
    
    refr = lerp(orgr, refr, refr.w);
    
    half  ndtl = max(dot(eye, texl), 0.f);
    half  frsl = fresnel(ndtl, 0.2f, vWaveParam.y);
    half  facg = 1.f - ndtl;
    
    half  scal = saturate(vTextParam.w / pIn.wave0.w);
    
    texl.z *= vTextParam.z;
    texl = normalize(texl);
    
    if (bUnder)
    {
        eye.z = -eye.z;
        half3 ldir = -vLightDir;
        half4 litk = lit(dot(texl, ldir.xyz), dot(normalize(eye + ldir.xyz), texl), 100);
        litk *= 0.2f;
        half3 depth_color = refr.xyz;        
        color.xyz = WaterColor * litk.y + SpecrColor * litk.z + depth_color;
    }
    else
    {
        half3 ldir = -vLightDir;
        half4 litk = lit(dot(texl, ldir.xyz), dot(normalize(eye + ldir.xyz), texl), 100);               
        half3 depth_color = refr.xyz * scal + (1.f - scal) * half3(0.f, 0.1f, 0.125f);        
        
        half3 water_color = WaterColor * litk.y * facg + SpecrColor * litk.z + depth_color * (1.f - facg);
        water_color = lerp(depth_color, water_color, pIn.diff.a);
        refl = frsl * refl * pIn.diff.a;
          
        color.xyz = half4(refl + water_color + wave * 0.5f, 1.f);  
		color = CalFogColor(color, pIn.wave1_fFog.z);        
    }
    
	

    return color; 
}


// -------------------------- full effective -----------------------------

struct vs_out_full
{
    float4 hpos     : POSITION;    
    float4 eye     : TEXCOORD0;
    float4 wave0 : TEXCOORD1;
    float3 wave1_fFog : TEXCOORD2;
    float2 wave2 : TEXCOORD3;
    float2 wave3 : TEXCOORD4;        
    float4 spos  : TEXCOORD5;     
    float4 diff  : COLOR0;  
};

vs_out_full vs_full(float3 pos : POSITION, float4 diff : COLOR0, float2 tex0 : TEXCOORD0)
{
    vs_out_full o = (vs_out_full)0;
    
    float2 vtran;
    float2 vtex0;
    float4 hpos = mul(float4(pos, 1.f), (float4x4)matWorldViewProj);
    
    o.hpos = hpos;
    vtex0 = pos.xz * vTextParam.y;
    
    o.wave0.xy = vtex0.xy + vWaveParam.zw * 2.f;
    o.wave1_fFog.xy = vtex0.xy * 2.f + vWaveParam.zw * 4.f;
    o.wave2.xy = vtex0.xy * 4.f + vWaveParam.zw * 2.f;
    o.wave3.xy = vtex0.xy * 8.f + vWaveParam.zw;  
    
    o.wave0.zw = hpos.w;
    o.diff = saturate(diff * 2);
    
    hpos.y = -hpos.y;
    o.spos = float4((hpos.xy + hpos.w) * 0.5f, (-hpos.x + hpos.w) * 0.5f, hpos.w);

    o.eye = vCameraPos - float4(pos, 1.f);
    o.eye.xyz = o.eye.xzy;
    
    o.wave1_fFog.z = CalFogFactor(o.hpos.z);
    
    return o;
}

float4 ps_full(vs_out_full pIn, uniform bool bUnder) : COLOR
{
    half3 eye = normalize(pIn.eye);
    half4 color = half4(0.f, 0.f, 0.f, 1.f);
    
    half3 tex0 = tex2D(s0, pIn.wave0.xy).xyz;
    half3 tex1 = tex2D(s0, pIn.wave1_fFog.xy).xyz;
    half3 tex2 = tex2D(s0, pIn.wave2.xy).xyz;
    half3 tex3 = tex2D(s0, pIn.wave3.xy).xyz;
   
    half3 bump = normalize(2.f * (tex0.xyz + tex1.xyz + tex2.xyz + tex3.xyz) - 4.f);

    half3 texl = bump.xyz * half3(vTextParam.z, vTextParam.z, 1.f);    
    half3 texr = bump.xyz * half3(vWaveParam.x, vWaveParam.x, 1.f);

    half4 proj = pIn.spos / pIn.spos.w;
    half4 wave = tex2D(s3, proj.xy);
    half4 refl = 0;
    half4 refr = tex2D(s2, proj.xy + texr.xy - wave * 0.02f);
    half4 orgr = tex2D(s2, proj.xy);
    
    if (!bUnder)
        refl = tex2D(s1, proj.zy + texl.xy - wave * 0.2f);
        
    refr = lerp(orgr, refr, refr.w);
        
    half  ndtl = max(dot(eye, texl), 0.f);
    half  frsl = fresnel(ndtl, 0.2f, vWaveParam.y);
    half  facg = 1.f - ndtl;
    
    half  scal = saturate(vTextParam.w / pIn.wave0.w);
   
    texl.z *= vTextParam.z;
    texl = normalize(texl);

    if (bUnder)
    {
        eye.z = -eye.z;
        half3 ldir = -vLightDir;
        half4 litk = lit(dot(texl, ldir.xyz), dot(normalize(eye + ldir.xyz), texl), 100);
        litk *= 0.2f;
        half3 depth_color = refr.xyz;        
        color.xyz = WaterColor * litk.y + SpecrColor * litk.z + depth_color;

    }
    else
    {
        half3 ldir = -vLightDir;
        half4 litk = lit(dot(texl, ldir.xyz), dot(normalize(eye + ldir.xyz), texl), 100);        
   
        half3 depth_color = refr.xyz * scal + (1.f - scal) * half3(0.f, 0.1f, 0.125f);          
        half3 water_color = WaterColor * litk.y * facg + SpecrColor * litk.z + depth_color * (1.f - facg);
        water_color = lerp(depth_color, water_color, pIn.diff.a);
        refl = frsl * refl * pIn.diff.a;
        color = half4(refl + water_color + wave * 0.5f, 1.f);  
        color = CalFogColor(color, pIn.wave1_fFog.z);
    }
   
    return color;
}

vs_out_lower vs_river(float3 pos : POSITION, float4 diff : COLOR0, float2 tex0 : TEXCOORD0)
{
	vs_out_lower o = (vs_out_lower)0;

	o.hpos = mul(float4(pos, 1.f), matWorldViewProj);
	o.tex0 = tex0.xy /** vTextParam.x*/ + vWaveParam.zw; /** vTextParam.x *///pos.xz * vTextParam.x + vWaveParam.zw;
	o.tex0.x = -tex0.x;
	o.diff = diff;

	o.fFog = CalFogFactor(o.hpos.z);

	return o;
}

float4 ps_river(vs_out_lower pIn) : COLOR
{
	float4 color = tex2D(s0, pIn.tex0) * WaterColor * 10.0;    
	color.a = /*WaterColor.a **/ pIn.diff.a;
	color = CalFogColor(color, pIn.fFog);
	return color;
}
technique water
{
    // water surface
    
    // lower
    pass p0
    {
        VertexShader = compile vs_2_0 vs_lower();
        PixelShader  = compile ps_2_0 ps_lower(false);
        AlphaBlendEnable = True;
        SrcBlend = SrcAlpha;
        DestBlend = InvSrcAlpha; 
        AddressU[0] = WRAP;
        AddressV[0] = WRAP;
        MipFilter[0] = LINEAR;
        MinFilter[0] = LINEAR;
    }
    
    // medium
    pass p1
    {
        VertexShader = compile vs_2_0 vs_medium();
        PixelShader  = compile ps_2_0 ps_medium(false);
        AlphaBlendEnable = false;
        AlphaTestEnable  = false;
        AddressU[0] = WRAP;
        AddressV[0] = WRAP;
        AddressU[1] = MIRROR;
        AddressV[1] = MIRROR;
        AddressU[2] = MIRROR;
        AddressV[2] = MIRROR;
        MipFilter[0] = LINEAR;
        MipFilter[0] = LINEAR;
        MinFilter[0] = LINEAR;
        MagFilter[0] = LINEAR;
        MipFilter[1] = LINEAR;
        MinFilter[1] = LINEAR;
        MagFilter[1] = LINEAR;
        MipFilter[2] = LINEAR;
        MinFilter[2] = LINEAR;
        MagFilter[2] = LINEAR;
    }
    
    // high
    pass p2
    {
        VertexShader = compile vs_3_0 vs_high();
        PixelShader  = compile ps_3_0 ps_high(false);
        AlphaBlendEnable = false;
        AlphaTestEnable  = false;
        AddressU[0] = WRAP;
        AddressV[0] = WRAP;
        AddressU[1] = CLAMP;
        AddressV[1] = CLAMP;
        AddressU[2] = CLAMP;
        AddressV[2] = CLAMP;
        MipFilter[0] = LINEAR;
        MinFilter[0] = LINEAR;
        MagFilter[0] = LINEAR;
        MipFilter[1] = LINEAR;
        MinFilter[1] = LINEAR;
        MagFilter[1] = LINEAR;
        MipFilter[2] = LINEAR;
        MinFilter[2] = LINEAR;
        MagFilter[2] = LINEAR;
    }
    
    // very high
    pass p3
    {        
        VertexShader = compile vs_3_0 vs_high();
        PixelShader  = compile ps_3_0 ps_high(false);
        AlphaBlendEnable = false;
        AlphaTestEnable  = false;
        AddressU[0] = WRAP;
        AddressV[0] = WRAP;
        AddressU[1] = CLAMP;
        AddressV[1] = CLAMP;
        AddressU[2] = CLAMP;
        AddressV[2] = CLAMP;
        MipFilter[0] = LINEAR;
        MinFilter[0] = LINEAR;
        MagFilter[0] = LINEAR;
        MipFilter[1] = LINEAR;
        MinFilter[1] = LINEAR;
        MagFilter[1] = LINEAR;
        MipFilter[2] = LINEAR;
        MinFilter[2] = LINEAR;
        MagFilter[2] = LINEAR;
    }
    
    // full effective
    pass p4
    {        
        VertexShader = compile vs_3_0 vs_full();
        PixelShader  = compile ps_3_0 ps_full(false);
        AlphaBlendEnable = false;
        AlphaTestEnable  = false;
        AddressU[0] = WRAP;
        AddressV[0] = WRAP;
        AddressU[1] = CLAMP;
        AddressV[1] = CLAMP;
        AddressU[2] = CLAMP;
        AddressV[2] = CLAMP;
        AddressU[3] = CLAMP;
        AddressV[3] = CLAMP;
        MipFilter[0] = LINEAR;
        MinFilter[0] = LINEAR;
        MagFilter[0] = LINEAR;
        MipFilter[1] = LINEAR;
        MinFilter[1] = LINEAR;
        MagFilter[1] = LINEAR;
        MipFilter[2] = LINEAR;
        MinFilter[2] = LINEAR;
        MagFilter[2] = LINEAR;
        MipFilter[3] = LINEAR;
        MinFilter[3] = LINEAR;
        MagFilter[3] = LINEAR;
    }   
    
    
    // under water
    
     // lower
    pass p5
    {
        VertexShader = compile vs_2_0 vs_lower();
        PixelShader  = compile ps_2_0 ps_lower(true);
        AlphaBlendEnable = True;
        SrcBlend = SrcAlpha;
        DestBlend = InvSrcAlpha; 
        AddressU[0] = WRAP;
        AddressV[0] = WRAP;
        MipFilter[0] = LINEAR;
        MinFilter[0] = LINEAR;
        //FogEnable = false;
    }
    
    // medium
    pass p6
    {
        VertexShader = compile vs_2_0 vs_medium();
        PixelShader  = compile ps_2_0 ps_medium(true);
        AlphaBlendEnable = false;
        AlphaTestEnable  = false;
        AddressU[0] = WRAP;
        AddressV[0] = WRAP;
        MipFilter[0] = LINEAR;
        MinFilter[0] = LINEAR;
        //FogEnable = false;
    }
    
    // high
    pass p7
    {
        VertexShader = compile vs_2_0 vs_high();
        PixelShader  = compile ps_2_0 ps_high(true);
        AlphaBlendEnable = false;
        AlphaTestEnable  = false;
        AddressU[0] = WRAP;
        AddressV[0] = WRAP;
        AddressU[1] = CLAMP;
        AddressV[1] = CLAMP;
        AddressU[2] = CLAMP;
        AddressV[2] = CLAMP;
        MipFilter[0] = LINEAR;
        MinFilter[0] = LINEAR;
        MagFilter[0] = LINEAR;
        MipFilter[1] = LINEAR;
        MinFilter[1] = LINEAR;
        MagFilter[1] = LINEAR;
        MipFilter[2] = LINEAR;
        MinFilter[2] = LINEAR;
        MagFilter[2] = LINEAR;
       //FogEnable = false;
    }
    
    // very high
    pass p8
    {        
        VertexShader = compile vs_2_0 vs_high();
        PixelShader  = compile ps_2_0 ps_high(true);
        AlphaBlendEnable = false;
        AlphaTestEnable  = false;
        AddressU[0] = WRAP;
        AddressV[0] = WRAP;
        AddressU[1] = CLAMP;
        AddressV[1] = CLAMP;
        AddressU[2] = CLAMP;
        AddressV[2] = CLAMP;
        MipFilter[0] = LINEAR;
        MinFilter[0] = LINEAR;
        MagFilter[0] = LINEAR;
        MipFilter[1] = LINEAR;
        MinFilter[1] = LINEAR;
        MagFilter[1] = LINEAR;
        MipFilter[2] = LINEAR;
        MinFilter[2] = LINEAR;
        MagFilter[2] = LINEAR;
        //FogEnable = false;
    }
    
    // full effective
    pass p9
    {        
        VertexShader = compile vs_2_0 vs_full();
        PixelShader  = compile ps_2_0 ps_full(true);
        AlphaBlendEnable = false;
        AlphaTestEnable  = false;
        AddressU[0] = WRAP;
        AddressV[0] = WRAP;
        AddressU[1] = CLAMP;
        AddressV[1] = CLAMP;
        AddressU[2] = CLAMP;
        AddressV[2] = CLAMP;
        AddressU[3] = CLAMP;
        AddressV[3] = CLAMP;
        MipFilter[0] = LINEAR;
        MinFilter[0] = LINEAR;
        MagFilter[0] = LINEAR;
        MipFilter[1] = LINEAR;
        MinFilter[1] = LINEAR;
        MagFilter[1] = LINEAR;
        MipFilter[2] = LINEAR;
        MinFilter[2] = LINEAR;
        MagFilter[2] = LINEAR;
        MipFilter[3] = LINEAR;
        MinFilter[3] = LINEAR;
        MagFilter[3] = LINEAR;
        //FogEnable = false;
    }   
	pass p10
	{
		VertexShader = compile vs_2_0 vs_river();
		PixelShader  = compile ps_2_0 ps_river();
		AlphaBlendEnable = True;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha; 
		AddressU[0] = WRAP;
		AddressV[0] = WRAP;
		MipFilter[0] = LINEAR;
		MinFilter[0] = LINEAR;
	}
}
