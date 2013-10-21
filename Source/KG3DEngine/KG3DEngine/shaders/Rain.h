////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : Rain.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2009-7-13 16:37:38
//  Comment     : 
/*
	This file is shared between shader and cpp files, be carefull that not to include any
	shader specified content. Usually, we only declare structs here, so that we can use them
	in out cpp. 

	Struct construction function is neccesary! We have to add this to ensure our shaders can work
	even if we do not initial the structs correctly in our cpp before pass them to shader. 
	There is no way to automatically garentee weather all elements in the structs are initialized,
	it is better to give every elem a zero value than to just let a uninitialized value be passed in
	to the shader. In the worst case, it at lease be easier to find which value is zero.
*/
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_RAIN_H_
#define _INCLUDE_RAIN_H_

#define	 SAMPLE_TIME	8

#ifndef SHOW_ANIMATION
#define SHOW_ANIMATION 1
#endif

#ifndef SHOW_FAR_RAIN_ANIMATION 
#define SHOW_FAR_RAIN_ANIMATION 1
#endif

#ifndef SHOW_SKYBOX
#define SHOW_SKYBOX 0
#endif

#ifndef SHOW_FAR_RAIN
#define SHOW_FAR_RAIN	1	//do not change this name unless you change the corresponding name in the cpp
#endif

#ifndef SHOW_PARTICLE_RAIN	
#define SHOW_PARTICLE_RAIN	1
#endif

#ifndef SHOW_JUMPING_SPLASH
#define SHOW_JUMPING_SPLASH	1
#endif

#ifndef RAIN_BLOCKED_BY_HEIGHT_FEILD
#define RAIN_BLOCKED_BY_HEIGHT_FEILD	1
#endif

#ifndef SPLASH_MOVE_TO_HEIGHT_FEILD
#define SPLASH_MOVE_TO_HEIGHT_FEILD		1
#endif

#ifndef SHOW_TEXTURE
#define SHOW_TEXTURE	1
#endif

#ifndef SHOW_HIGHT_LIGHT
#define SHOW_HIGHT_LIGHT	1
#endif

#ifndef SHOW_RAIN_DROP_TEX_ANIMATION
#define SHOW_RAIN_DROP_TEX_ANIMATION	1
#endif

#ifndef SHOW_STRETCH	 
#define SHOW_STRETCH	1
#endif

#ifndef POSITION_NOISE
#define POSITION_NOISE	1
#endif

#ifndef SPEED_NOISE
#define SPEED_NOISE 1
#endif

#ifndef SIZE_NOISE
#define SIZE_NOISE 1
#endif

#ifndef MOVE_XZ
#define MOVE_XZ	1
#endif

#ifndef MOVE_Y
#define MOVE_Y 1
#endif

#ifndef ENDLESS_XZ_REPEAT
#define ENDLESS_XZ_REPEAT	1
#endif

#ifndef ENDLESS_Y_REPEAT
#define ENDLESS_Y_REPEAT	1
#endif

#ifndef ENDLESS_ROTATION 
#define ENDLESS_ROTATION 1
#endif

#ifndef MULTIPLY_TEX_FACTOR
#define MULTIPLY_TEX_FACTOR 1
#endif

#ifndef SPLASH_ANIMATION
#define SPLASH_ANIMATION	1
#endif

#ifndef SHOW_JUMPING_SPLASH_TEX
#define SHOW_JUMPING_SPLASH_TEX	1
#endif

#ifndef SHOW_FADE
#define SHOW_FADE	0
#endif

#ifndef SHOW_ALPHA
#define SHOW_ALPHA	1
#endif

#ifndef ROTATE_TOWARDS_CAMERA 
#define	ROTATE_TOWARDS_CAMERA	1
#endif

#ifndef NOISE_TEX_RGBA
#define	NOISE_TEX_RGBA	1
#endif

#ifndef SHOW_FAR_RAIN_TEX
#define SHOW_FAR_RAIN_TEX	1
#endif

#ifndef ENDLESS_FARRAIN_REPEAT 
#define ENDLESS_FARRAIN_REPEAT 1
#endif

#ifndef FAR_RAIN_ROTATE
#define FAR_RAIN_ROTATE	1
#endif

#ifndef MY_DEBUG
#define MY_DEBUG	0
#endif

#include "ShaderTypeDefinitions.h"
////////////////////////////////////////////////////////////////////////////////
//parameters, which will not be changed every time in rendering, do not need to be packed
struct ShaderConstsRainStreak 
{
	//float fRange;
	float3	vMaxMovement;
	float3 vStretch;
	float3	vPosNoiseRange;
	float3	vSpeedNoisePercent;
	float fSizeNoisePercent;
	float fOscillationPeriod;
	float2 vUVPerSubTex;	//percentage of a sub texture
	float	fNearFadeRangePow2;	//rain streaks fade-out as the camera move to it
	float	fFarFadeMax;
	float	fNearFadeMin;

	float fIntensity;
	float fMinRainStreakTexBrightWeight;	//rain streak tex lib embed weight values to normalize the gray scale of the texes, however, they are just two low in some situation, we have to lerp them from a low value to 1

	float fHighlightGate;	//pixel which has a value higher than this will be hight-lighted
	float fHighlightIntensity;

#ifdef __cplusplus
	ShaderConstsRainStreak()	//Neccesary ! View the comments above.
	{
		memset(this, 0, sizeof(*this));
	}
#endif
};

struct ShaderVariantsRainRepeat 
{
	float		fGapBetweenStreaks;
	float2		vCenterBias;
	float4		vCameraRotation;//2x2 rotation matrix
	float		fRange;
};

struct ShaderVariantsRainStreak 
{
	float3		vCamPos;
	float3		vSpeed;	//can not be a zero-length vector
	float3		vSpeedDir;
	//float		fGapBetweenStreaks;
	//float2		vCenterBias;
	//float2x2	vCameraRotation;
	float3		vEyeDir;	//in High solution, eyeDir is calculated within every vertex	shader
#ifdef __cplusplus
	ShaderVariantsRainStreak()
	{
		memset(this, 0, sizeof(*this));
	}
#endif
};

struct ShaderConstsHeightFeild 
{
	float2 vUnitToUV;
	float2 vTexelMovement;//m/width, n/height
};

struct  ShaderConstsJumpingSplash
{
	float fPeriod;
	float fGap;
	float fSizeNoiseMax;	//Percentage
	float fScale;			
	float fNearCameraFadeRange;	//In how much range does the splash fade out
	float fFarCameraFadeRange;
	float fAlphaFactor;			//Final alpha will multiply this value
	float3 vSplashModelStretch;	
#ifdef __cplusplus
	ShaderConstsJumpingSplash()
	{
		memset(this, 0, sizeof(*this));
	}
#endif
};

struct ShaderConstsFarRain 
{
	float fPeriod;
	float fYRepeat;
	float fXRepeatHalf;	//the u value is identity in +u and -u direction around the circle, so the far rain tex can be seamless around the whole circle
	float3 vSize;
	float fDecay;		//Decay a little to control far rain tex explosion
#ifdef __cplusplus
	ShaderConstsFarRain()
	{
		memset(this, 0, sizeof(*this));
	}
#endif
};

struct ShaderVariantsFarRain
{
	float fDensity;	//1 for normal, upper 1 for denser rain
	float fFadeOut;	//fade out when the rain become less dense
	float3 vCamPos;	//This is exactly where the camera is, do not bias like rain streak technique
#ifdef __cplusplus
	ShaderVariantsFarRain()
	{
		memset(this, 0, sizeof(*this));
	}
#endif
};

#endif //_INCLUDE_RAIN_H_
