////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : RainEnvironment.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2009-8-21 16:34:11
//  Comment     : Effect for other objects when it is rain
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_RAINENVIRONMENT_H_
#define _INCLUDE_RAINENVIRONMENT_H_

////////////////////////////////////////////////////////////////////////////////
struct  ShaderVariantsRainSplash
{
	float fCircle;
	float2 vUVRandom;
	float fWorldToUV;
#ifdef __cplusplus
	ShaderVariantsRainSplash()
	{
		memset(this, 0, sizeof(*this));
	}
#endif
};



#ifndef __cplusplus	//The following  parameters can be shared between shaders
#include "CommonParams.h"

#ifndef FLAT_SPLASH_ANIMATION
#define FLAT_SPLASH_ANIMATION 1
#endif

#ifndef RAIN_WET
#define RAIN_WET	1
#endif

SHARED	texture3D g_texRainSplashBump;
sampler3D g_samRainSplashBump = sampler_state{
	texture = <g_texRainSplashBump>;
	AddressU = Mirror;
	AddressV = Mirror;
	MipFilter = LINEAR;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
};

SHARED	bool	g_bEnableWet = false;	//It must be false by default

SHARED ShaderVariantsRainSplash g_rainSplashVariants;

float3 GetSplashBumpValue(float3 vWorldPos)
{
	float3 splashUVW = float3(vWorldPos.xz * g_rainSplashVariants.fWorldToUV, 0.5f);
#if FLAT_SPLASH_ANIMATION == 1
	splashUVW.z = g_rainSplashVariants.fCircle;
	splashUVW.xy += g_rainSplashVariants.vUVRandom.xy;//because the values would be set randomly from -1 to 1,so do not need to worry about overflow
#endif

	float4 bumpValue = tex3Dlod(g_samRainSplashBump, float4(splashUVW, 0));
	bumpValue = 2 * (bumpValue - 0.5f);//remap to [-1,1]
	return bumpValue;
}

float3 ModifyBumpBySplashBump(float3 inNormal, float3 vWorldPos)
{
	float3 splashBump = GetSplashBumpValue(vWorldPos);
	return normalize(inNormal + splashBump);
}

float3 AddRainSplashRelection(float3 rgbIn, float3 iNormal, float3 vWorldPos)
{
	if (! g_bEnableWet)
	{
		return rgbIn;
	}
	float3 splashBump = GetSplashBumpValue(vWorldPos);
	return rgbIn + max(0, dot(sunLight.m_dir, splashBump));
}

#endif

#endif //_INCLUDE_RAINENVIRONMENT_H_
