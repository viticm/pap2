////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : SimpleSample.fx
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2009-7-9 9:45:02
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _RAIN_FX_INCLUDED
#define _RAIN_FX_INCLUDED

#include "Rain.h"
#include "CommonParams.h"
#include "RainEnvironment.h"

ShaderVariantsRainRepeat g_rainRepeat = 
{
	5,			//fGapBetweenStreaks
	{0,0},		//vCenterBias
	{1,0,0,1},	//vCameraRotation
	1,			//fRange
};

ShaderConstsRainStreak g_rainStreakConsts = 
{
	//1,	//fRange
	{400,-400,400},	//vMaxMovement
	{1.2f, 12.f, 1.2f},//vStretch
	{0,0,0},//vPosNoiseRange
	{0.15,0.15,0.15}, //vSpeedNoisePercent
	0.4f,	//fSizeNoisePercent
	1,		//fOscillationPeriod
	{1,1},	//vUVPerSubTex
	40 * 40,	//fNearFadeRangePow2
	0.5,		//fFarFadeMax
	0.1,		//fNearFadeMin

	3,		//Intensity
	0.2f,	//fMinRainStreakTexBrightWeight

	0.10,	//fHighlightGate
	5,		//fHighlightIntensity
};

ShaderVariantsRainStreak	g_rainStreakVariants = 
{
	{0,0,0},	//vCamPos
	{0,-260,0},	//vSpeed;
	{0,-1,0},	//vSpeedDir
	//5,			//fGapBetweenStreaks
	//{0,0},		//vCenterBias
	//{1,0,0,1},	//vCameraRotation
	{0,0,1},	//vEyeDir
};


float   g_fRainTime = 0;                   // App's time in seconds
float	g_fRainNoAniTime = 10.2;			//the Time used when animation is forbidden

float4x4 g_mWorldViewProjection = {
	1,0,0,0,
	0,1,0,0,
	0,0,1,0,
	0,0,0,1,
};    // World * View * Projection matrix

texture g_texInstance;
sampler2D g_samTexInstance = sampler_state{
	Texture = <g_texInstance>;
   	MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};

texture g_texRainStreak;
sampler2D g_samRainStreak = sampler_state{
	Texture = <g_texRainStreak>;
	MipFilter = LINEAR;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
};

texture g_texRainStreakMask;	//a mask with a dissolved edge for softing the edge of rain streaks
sampler2D g_samRainStreakMask = sampler_state{
	Texture = <g_texRainStreakMask>;
	MipFilter = LINEAR;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
};

struct vs_out
{
	float4 oPos : POSITION;
	float3 texCoords[SAMPLE_TIME] : TEXCOORD0;	//z is Weight, Last is TotalWeight. Here we use the syntax array gramma, please ref to the dx document, chapter "Writing HLSL shaders in Direct3D9"
	float3 fadeFactor0_nearFade1_totalWeight2 : COLOR0;
	float3 eyeDir : COLOR1;
};
#define PI 3.14159265

texture3D g_texSkyBox;
samplerCUBE g_samSkyBox = sampler_state
{
	Texture = <g_texSkyBox>;
	MipFilter = LINEAR;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
};

//<Height Field>
texture g_texHeightFeild;
sampler2D g_samHeightFeild = sampler_state 
{
	Texture = <g_texHeightFeild>;
	MipFilter = Linear;
	MinFilter = Linear;
	MagFilter = Linear;
	//AddressU = Border;
	//AddressV = Border;
};


ShaderConstsHeightFeild g_heightFeildConsts = {
	{0,0},	//vUnitToUV
	{1,1},	//vTexelMovement
};

//float GetZFromDepthZ(FLOAT fDepthZ)
//{
//	float zn = 10.0f;
//	float zf = g_fMaxSceneHeight * 1.5f;
//
//	FLOAT fScaleDepth = (zf - zn) *fDepthZ;
//	return (zn * zf) / (zf - fScaleDepth);
//
//	//return zn / (1 - fDepthZ + 0.0000001f);
//}

float GetHeightFieldHeight(float2 posXZ)
{
	float2 texCd = posXZ.xy * g_heightFeildConsts.vUnitToUV.xy;
	texCd.xy += g_heightFeildConsts.vTexelMovement.xy;


	float fValue = tex2Dlod(g_samHeightFeild, float4(texCd.xy, 0,0));
	return  fValue + 20;
}
//</Height Field>

float GetHightLight(float pixelValue)
{
#if SHOW_HIGHT_LIGHT == 1
	return saturate(pixelValue - g_rainStreakConsts.fHighlightGate) * g_rainStreakConsts.fHighlightIntensity;
#else
	return 0;
#endif
}

void calVH(float3 lightDir				 
				  , float3 eyeDir
				  , inout float3 v_h_flipTex)	//all vectors are normalized. v is [0 - PI/2], h is [0-PI], both are radians
{
	// Inputs: lightDir, eyeDir, dropDir
	float3 L = lightDir;
	float3 E = eyeDir;
	float3 N = g_rainStreakVariants.vSpeedDir;

	float hangle = 0;
	float LdotN = dot(L,N);
	float vangle = abs(acos(LdotN) - PI / 2 ); // 0 - PI/2	//i*/f LdotN < 0, vangle is [90,180],turn it to 0-90

	//float flipTexture = 1;	
	
	if(vangle < radians(88.0))
	{
		//project two vector to the plane which is vertical to N
		float3 Lp = normalize(L - LdotN * N);
		float3 Ep = normalize(E - dot(E,N)*N);
		float fDotLpEp = dot(Ep,Lp);
		hangle = acos(fDotLpEp); 

		//flipTexture = dot( N, cross(Ep,Lp)) > 0;	//flip texture is not necessary, because the rain streak sprites will always face the camera
	}
	v_h_flipTex.x = vangle;
	v_h_flipTex.y = hangle;
	v_h_flipTex.z = 0;//flipTexture;
}

void calOsc(float random, float fTime, inout float osc)//random and osc should be in [0,1]
{
	osc = frac(random + (fTime / g_rainStreakConsts.fOscillationPeriod));
}

void calDiscretization(float valueIn, float minValue, float stepValue, float scatterCount
					   , inout float2 texCoordIndex, inout float3 discValue01_mappedValue)	//discretize float point value to nearby integer
{
	discValue01_mappedValue.z = (valueIn - minValue) / stepValue;
	discValue01_mappedValue.x = floor(discValue01_mappedValue.z);
	discValue01_mappedValue.y = discValue01_mappedValue.x + 1;

	texCoordIndex.xy = discValue01_mappedValue.xy;

	texCoordIndex.x += step(texCoordIndex.x, -0.000001) * scatterCount;
	/*it equals "if(texCoordIndex.x < 0)
		texCoordIndex.x += scatterCount;"*/
	
	if(texCoordIndex.y >= scatterCount)
		texCoordIndex.y -= scatterCount;

	texCoordIndex.y -= step(scatterCount, texCoordIndex.y) * scatterCount;
	/*it equal "if(texCoordIndex.y >= scatterCount)
		texCoordIndex.y -= scatterCount;"*/
}

#define H_COUNT 9
#define H_STEP radians(20)
#define H_MIN	radians(10)
#define V_COUNT 5
#define V_STEP radians(20)
#define V_MIN radians(10)
#define OSC_COUNT 10

void calTexCoords(float4 h_v_osc_flipTex, float2 basicTexCd
						, inout float3 texCoords[SAMPLE_TIME]
						, inout float sampleTotleWeight)
{
	int i = 0;

	float2 vIndex; float3 vDisc;
	calDiscretization(h_v_osc_flipTex.x, V_MIN, V_STEP, V_COUNT, vIndex, vDisc);

	float2 hIndex; float3 hDisc;
	calDiscretization(h_v_osc_flipTex.y, H_MIN, H_STEP, H_COUNT, hIndex, hDisc);
	
	float2 oscIndex; float3 oscDisc;
	calDiscretization(h_v_osc_flipTex.z * OSC_COUNT, 0, 1, OSC_COUNT, oscIndex, oscDisc);

	
	/*
	Assuming that h, v, osc form a 3D parameter space, so if we are now in point(x,y,z), the value 
	can be deduced as the average value of the sam of all 8 nearby discretized values.
	The average value could use the distances between the point and the discretized points as weights

	The following indexes follow the pattern of 001, 010, 011... , which is like binary plus.
	*/
	float2 indexCoords[SAMPLE_TIME];
	indexCoords[0] = float2(hIndex.x * OSC_COUNT + oscIndex.x, vIndex.x);
	indexCoords[1] = float2(hIndex.x * OSC_COUNT + oscIndex.x, vIndex.y);
	indexCoords[2] = float2(hIndex.x * OSC_COUNT + oscIndex.y, vIndex.x);
	indexCoords[3] = float2(hIndex.x * OSC_COUNT + oscIndex.y, vIndex.y);
	indexCoords[4] = float2(hIndex.y * OSC_COUNT + oscIndex.x, vIndex.x);
	indexCoords[5] = float2(hIndex.y * OSC_COUNT + oscIndex.x, vIndex.y);
	indexCoords[6] = float2(hIndex.y * OSC_COUNT + oscIndex.y, vIndex.x);
	indexCoords[7] = float2(hIndex.y * OSC_COUNT + oscIndex.y, vIndex.y);

	float sampleWeights[8];
	
	//map them to {V,H,OSC} space, and calculate their weight according to the distance between the descretized value and the former value
	float2 vDistances = vDisc.xy - vDisc.z;	
	float2 hDistances = hDisc.xy - hDisc.z;	
	float2 oscDistances = oscDisc.xy - oscDisc.z;	

	/*
	in the {V,H,OSC} space, v axis is half the length of h and osc, so mul its length value with 2 to balance this axis asymmetric.

	But remember, the more the point is close to, the more should be the weight
	, so we have to use the distance to the opposite point as the weight.
	*/
	sampleWeights[0] = 2 * length(float3(hDistances.y, oscDistances.y, vDistances.y));
	sampleWeights[1] = 2 * length(float3(hDistances.y, oscDistances.y, vDistances.x));
	sampleWeights[2] = length(float3(hDistances.y, oscDistances.x, vDistances.y));
	sampleWeights[3] = length(float3(hDistances.y, oscDistances.x, vDistances.x));
	sampleWeights[4] = length(float3(hDistances.x, oscDistances.y, vDistances.y));
	sampleWeights[5] = length(float3(hDistances.x, oscDistances.y, vDistances.x));
	sampleWeights[6] = length(float3(hDistances.x, oscDistances.x, vDistances.y));
	sampleWeights[7] = length(float3(hDistances.x, oscDistances.x, vDistances.x));

	sampleTotleWeight = 0;
	
	for (i = 0; i < SAMPLE_TIME; ++i)
	{
		sampleTotleWeight += sampleWeights[i];
		texCoords[i].z = sampleWeights[i];
	}

	for (i = 0; i < SAMPLE_TIME; ++i)
	{
		/*if (h_v_osc_flipTex.w > 0)	//no need for flip, point sprite turns with camera moves
		{
			indexCoords[i].x += 1;
			indexCoords[i].x -= basicTexCd.x;
			indexCoords[i].y += basicTexCd.y;
		}
		else*/
		{
			indexCoords[i].xy += basicTexCd.xy;
		}

		texCoords[i].xy = indexCoords[i].xy * g_rainStreakConsts.vUVPerSubTex.xy;
		texCoords[i].y = 1 - texCoords[i].y; //tex Coord orgin is left-bottom, so reverse it
	}
}

float2 ExtendVBEndless(float2 vBasicPos, inout float4 vNoiseTexIn)
{
	float2 vTrans;
	vTrans.xy = vBasicPos.xy * g_rainRepeat.fGapBetweenStreaks;

#if ENDLESS_ROTATION == 1
	//Rotation according to camera angle
	vTrans.xy = mul(vTrans.xy, (float2x2)g_rainRepeat.vCameraRotation);
#endif

#if ENDLESS_XZ_REPEAT == 1	
	//Move according to camera pos
	vTrans.xy += g_rainRepeat.vCenterBias.xy;
#endif

	float4 vNoiseTex = tex2Dlod(g_samTexInstance, float4(vTrans.xy / g_rainRepeat.fRange, 0, 0));
#if NOISE_TEX_RGBA == 1
	vNoiseTex *= 2;
	vNoiseTex -= 1;
#endif

	vNoiseTexIn = vNoiseTex;

	return vTrans.xy;
}

vs_out vs_rain_streak(in float3 iPos : POSITION
			, in float4 vBasicTexCd : COLOR0
			, in float2 iBasicPos : TEXCOORD0
			, in float iOscillation : TEXCOORD1)
{	
	float fTime = g_fRainTime;
#if SHOW_ANIMATION != 1
	fTime = g_fRainNoAniTime;
#endif
	
	vs_out vsOut;
	float3 vTrans = 0;
	float4 vNoiseTex;
	//xz is stored in second stream
	vTrans.xz = ExtendVBEndless(iBasicPos, vNoiseTex);

#if SPEED_NOISE == 1
	float3 fSpeedBias = g_rainStreakConsts.vSpeedNoisePercent * vNoiseTex.w;
	fSpeedBias += 1;
#else
	float3 fSpeedBias = 1;
#endif

	//move it along x and z
	{
#if POSITION_NOISE == 1	//texture xz noise movement, Basic xz scatter		
		vTrans.xz += vNoiseTex.xz * g_rainStreakConsts.vPosNoiseRange.xz;	
#endif

#if MOVE_XZ == 1
		float2 vXZSpeed = g_rainStreakVariants.vSpeed.xz * fSpeedBias.xz;
		float2 vXZMovement = vXZSpeed * fTime;

		float2 vSymbolOfSpeedXZ = sign(vXZSpeed);
		float2 vUpperXZ = - vSymbolOfSpeedXZ * g_rainStreakConsts.vMaxMovement.xz / 2;
		vXZMovement =  vUpperXZ
			+ fmod(vXZMovement - vUpperXZ, g_rainStreakConsts.vMaxMovement.xz + float2(0.00001,0.00001));//in case they are zero, the shader corrupts

		vTrans.xz += vXZMovement;
#endif
	}

	//trans y
#if 1
	{
		//texture y noise movement
		float fMaxYMovement = abs(g_rainStreakConsts.vMaxMovement.y);

		float yMovement = 0;
#if POSITION_NOISE == 1			//Basic y scatter
		yMovement += ((vNoiseTex.y + 1) / 2) * g_rainStreakConsts.vPosNoiseRange.y;		
#endif
		
#if MOVE_Y == 1
		//speed y move
		float ySpeed = g_rainStreakVariants.vSpeed.y * fSpeedBias.y;
		yMovement += ySpeed * fTime;
#endif

		//normalize the movement and remove to the center
		float fUpper = fMaxYMovement / 2;		
#if ENDLESS_Y_REPEAT == 1
		fUpper += g_rainStreakVariants.vCamPos.y;
#endif
		yMovement = fUpper - fmod(fUpper - yMovement, fMaxYMovement);	//make sure x > 0 in fmod(x,y)

		vTrans.y += yMovement;

	}
#endif

	//if lower than height feild the rain is blocked, erase the vertex
#if RAIN_BLOCKED_BY_HEIGHT_FEILD == 1
	{
		float heightValue = GetHeightFieldHeight(vTrans.xz);
		if(vTrans.y < heightValue)
		{
			vsOut = (vs_out)0;
			vsOut.oPos = float4(0,0,0,0.1);//w can not be zero, that would cause problem

			//do not worry about efficiency, hardware can erase 0 size triangles instantly and automatically, and 
			//zero triangles like the one we collaped here will not arouse any ps
			return vsOut;
		}
	}
#endif

#if SHOW_TEXTURE == 1
	//Calculate V,H,O, Very complicated
	{
		float4 v_h_osc_flipTex;

#if 0
		float3 vEyeDir = g_rainStreakVariants.vEyeDir;
#else
		float3 vEyeDir = vTrans.xyz - g_rainStreakVariants.vCamPos.xyz;
		vEyeDir = normalize(vEyeDir);
#endif

		vsOut.eyeDir = vEyeDir;

#if SHOW_RAIN_DROP_TEX_ANIMATION == 1
		calVH(sunLight.m_dir, vEyeDir, v_h_osc_flipTex.xyw);
		calOsc((vNoiseTex.z + 1)/2, fTime, v_h_osc_flipTex.z);
#else
		v_h_osc_flipTex.xyzw = float4(2,2,0.5,0);
#endif

		float samTotleWeight;

		calTexCoords(v_h_osc_flipTex, vBasicTexCd.rg, vsOut.texCoords, samTotleWeight);

		vsOut.fadeFactor0_nearFade1_totalWeight2.z = samTotleWeight;
	}
#else
	vsOut.eyeDir = 0;
	for(int i = 0; i < SAMPLE_TIME; ++i)
	{
		vsOut.texCoords[i] = 0.f;
	}
	vsOut.fadeFactor0_nearFade1_totalWeight2.z = 1;
#endif	//SHOW_TEXTURE == 1
	
	//begin finial adjustment to the input vertices
	float3 posIn = iPos;

#if SHOW_STRETCH == 1
	//strentch
	posIn *= g_rainStreakConsts.vStretch.xyz;
#endif


#if ROTATE_TOWARDS_CAMERA == 1	//rotate it according to the rain movement direction and let it face to camera
	{
		float3 vDirMovement = normalize(-g_rainStreakVariants.vSpeedDir);
		float3 vEyeDir = normalize(vTrans.xyz - g_rainStreakVariants.vCamPos.xyz);
		float3 vNewX = normalize(cross(vDirMovement, vEyeDir));
		float3 vNewZ = normalize(cross(vNewX, vDirMovement));
		//vNewX = normalize(cross(vDirMovement, vNewZ));
		posIn.xyz = posIn.y * vDirMovement + posIn.x * vNewX;
	}
#endif

#if SIZE_NOISE == 1
	posIn *= (float3(1,1,1) + vNoiseTex.xyz * 0.4f);//size noise
#endif

	posIn.xyz += vTrans.xyz;

	vsOut.oPos = mul(float4(posIn, 1), g_mWorldViewProjection);

	// fade factor caculaton
#if SHOW_FADE == 1
	{
		float disTmp = length(vTrans.xyz - g_rainStreakVariants.vCamPos.xyz);
		float disPow2 = disTmp * disTmp;
		float farFadeFactor = saturate(disPow2 / (g_rainRepeat.fRange * g_rainRepeat.fRange));
		farFadeFactor = lerp(1, g_rainStreakConsts.fFarFadeMax, farFadeFactor);

		float nearFadeFactor = saturate(disPow2 / g_rainStreakConsts.fNearFadeRangePow2);
		nearFadeFactor = lerp(g_rainStreakConsts.fNearFadeMin, 1, nearFadeFactor);

		vsOut.fadeFactor0_nearFade1_totalWeight2.x = farFadeFactor * nearFadeFactor;
		vsOut.fadeFactor0_nearFade1_totalWeight2.y = nearFadeFactor;
	}
#else
	vsOut.fadeFactor0_nearFade1_totalWeight2.xy = 1;
#endif

#if SHOW_PARTICLE_RAIN != 1
	vsOut.oPos = float4(0,0,0,0);
#endif

	return vsOut;
}

float4 ps_rain_streak(vs_out psIn) : COLOR
{
	/*float3 texCoords[SAMPLE_TIME] = {
		psIn.texCoord0.xyz,
		psIn.texCoord1.xyz,
		psIn.texCoord2.xyz,
		psIn.texCoord3.xyz,
		psIn.texCoord4.xyz,
		psIn.texCoord5.xyz,
		psIn.texCoord6.xyz,
		psIn.texCoord7.xyz,
	};*/
#if SHOW_TEXTURE != 1	
	return float4(1, 1, 1, 0.5);//psIn.oFadeFactor * 0.5f);
#endif

	float totalValue = 0;

	for (int i = 0; i < SAMPLE_TIME; ++i)
	{
		float4 texValue = tex2D(g_samRainStreak, psIn.texCoords[i].xy);
		float valueTemp = texValue.r; 
#if MULTIPLY_TEX_FACTOR == 1
		valueTemp *= lerp(g_rainStreakConsts.fMinRainStreakTexBrightWeight, 1, texValue.g);	//r is origin tex, g is the brightness embedded in the rain libray, they should be multiplied;
#endif
		valueTemp = abs(valueTemp);
		valueTemp = pow(valueTemp, 1 / g_rainStreakConsts.fIntensity);	//this is the intencity

		valueTemp *= psIn.texCoords[i].z;	//z is distance weight
		totalValue += valueTemp;
	}
	totalValue /= psIn.fadeFactor0_nearFade1_totalWeight2.z;

	saturate(totalValue);

#if SHOW_FADE == 1
	totalValue *= psIn.fadeFactor0_nearFade1_totalWeight2.x;
#endif
	
	float fHightLightMask = tex2D(g_samRainStreak, psIn.texCoords[1]).a;
	totalValue += GetHightLight(totalValue) * fHightLightMask;

	float2 texCoordBase;
	texCoordBase.x = fmod(psIn.texCoords[0].x, g_rainStreakConsts.vUVPerSubTex.x);
	texCoordBase.y = fmod(psIn.texCoords[0].y, g_rainStreakConsts.vUVPerSubTex.y);
	float fEdgeMaskValue = tex2Dlod(g_samRainStreak, float4(saturate(texCoordBase.xy),0,0)).b;	//first sub tex is edge mask. tex2Dlod is neccesary
	totalValue *= fEdgeMaskValue;

	saturate(totalValue);

	float4 retColor = float4(totalValue, totalValue, totalValue, totalValue);

	/*float4 envValue = texCUBE(g_samSkyBox, psIn.eyeDir);
	retColor.rgb *= envValue.rgb;*/

	return retColor;
}

technique techRain
{
	pass p0
	{
		vertexShader = compile vs_3_0 vs_rain_streak();
		pixelShader = compile ps_3_0 ps_rain_streak();
		
		FogEnable = False;
		FogColor = 0xffffffff;
#if SHOW_ALPHA == 1
		AlphaBlendEnable = True;
		SrcBlend = InvDestColor;	//rain drops are less visible when background is brighter
		DestBlend = One;
#else
		AlphaBlendEnable = False;
#endif

		SeparateAlphaBlendEnable = False;
		//SrcBlendAlpha = One;
		//DestBlendAlpha = Zero;

		CullMode = CCW;
		ZEnable = True;
		Zfunc = LessEqual;
		ZWriteEnable = False;

		ColorWriteEnable = Alpha | Red | Green | Blue;
	}
}

ShaderConstsJumpingSplash g_jumpingSplashConsts = {
	1,	//fPeriod
	1,	//fGap
	0.4,	//fSizeNoiseMax
	1.f / 200.f,	//fScale
	30,		//fNearCameraFadeRange
	100,	//fFarCameraFadeRange
	0.5f,	//fAlphaFactor
	{3,1,3},	//vSplashModelStretch
};

struct vsJumpingSlashOut 
{
	float4 pos : POSITION;
	float2 texCd : TEXCOORD0;
	float  visibility : TEXCOORD1;
};

texture g_texJumpingSplash;
sampler2D g_samJumpingSplash = sampler_state 
{
	Texture = <g_texJumpingSplash>;
	MipFilter = LINEAR;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
};

void vs_jumping_splash(float3 iPos : POSITION
			   , float2 iTexCd : TEXCOORD0 
			   , float2 iPosBase : TEXCOORD1	//Instancing
			   , float iRandom : TEXCOORD2
			   , out vsJumpingSlashOut vsOut)
{
	float fTime = g_fRainTime;
#if SPLASH_ANIMATION != 1
	fTime = g_fRainNoAniTime;
#endif

	float3 vTrans = 0;
	float4 vNoiseTex;

	vTrans.xz = ExtendVBEndless(iPosBase, vNoiseTex);

	//Random Life
	float lifeValue = 0.5;
	{
		float fTmp = vNoiseTex.w + (fTime / g_jumpingSplashConsts.fPeriod);
		lifeValue = frac(fTmp);
	
		float fRandom = fmod(floor(fTmp), 10);

		vTrans.xz += fRandom * g_rainRepeat.fGapBetweenStreaks;
	}

	//movement noise and scale noise
	float scaleNoise;
	float2x2	randomXZrotation;
	{
		vTrans.xz += vNoiseTex.xz * g_rainRepeat.fGapBetweenStreaks;

		scaleNoise = 1 + g_jumpingSplashConsts.fSizeNoiseMax * saturate(vNoiseTex.y) 
					- 0.25 * g_jumpingSplashConsts.fSizeNoiseMax * saturate(-vNoiseTex.y);

		float rotationValue = PI * vNoiseTex.y;	//0-PI rotation
		randomXZrotation[0][0] = cos(rotationValue);
		randomXZrotation[0][1] = -sin(rotationValue);
		randomXZrotation[1][0] = sin(rotationValue);
		randomXZrotation[1][1] = cos(rotationValue);
	}

#if SPLASH_MOVE_TO_HEIGHT_FEILD == 1
	//Move splashes to Height Field
	vTrans.y = GetHeightFieldHeight(vTrans.xz);
#endif

	float alphaValue;
	float3 lifeTimeScale;
	{
		lifeTimeScale.xz = 0.2f + 0.8f * lifeValue;	//this form a line which starts from (0,0.2)
		lifeTimeScale.y = sin((lifeValue) * PI);
		alphaValue =  sin((lifeValue) * PI);

		//camera waste
		float fDistance = length(g_rainStreakVariants.vCamPos.xyz - vTrans.xyz);
		float cameraNearAlpha = saturate(fDistance / 300);//g_jumpingSplashConsts.fNearCameraFadeRange);

		float cameraFarAlpha = fDistance / g_jumpingSplashConsts.fFarCameraFadeRange;
		cameraFarAlpha = saturate(1 - cameraFarAlpha * cameraFarAlpha);

		alphaValue *= cameraNearAlpha;
		alphaValue *= cameraFarAlpha;
		alphaValue *= g_jumpingSplashConsts.fAlphaFactor;	//alpha control factor;
	}
	
	float3 posTmp = iPos;
	
	//origin transform
	posTmp *= g_jumpingSplashConsts.fScale;
	posTmp *= g_jumpingSplashConsts.vSplashModelStretch;
	posTmp *= scaleNoise;
	posTmp *= lifeTimeScale;

	//random rotation
	posTmp.xz = mul(posTmp.xz, randomXZrotation);
	/*{
		float3 posForRotation = posTmp;
		posTmp.x = posForRotation.x * randomXZrotation.x + posForRotation.z * randomXZrotation.z;
		posTmp.z = posForRotation.x * randomXZrotation.y + posForRotation.z * randomXZrotation.w;
	}*/

	posTmp += vTrans;

	vsOut.pos = mul(float4(posTmp, 1), g_mWorldViewProjection);
	vsOut.texCd = iTexCd;
	vsOut.visibility = alphaValue;
}

float4 ps_jumping_splash(vsJumpingSlashOut psIn) : COLOR
{
#if SHOW_JUMPING_SPLASH_TEX != 1
	return float4(1,1,1,1);
#endif
	float valueTex = tex2D(g_samJumpingSplash, psIn.texCd);
	valueTex *= psIn.visibility;
	return float4(valueTex,valueTex,valueTex,1);
}

technique techJumpingSplash
{
	pass p0
	{
		vertexShader = compile vs_3_0 vs_jumping_splash();
		pixelShader = compile ps_3_0 ps_jumping_splash();

#if SHOW_JUMPING_SPLASH_TEX == 1
		AlphaBlendEnable = True;
		SrcBlend = InvDestColor;	//rain drops are less visible when background is brighter
		DestBlend = One;
#else
		AlphaBlendEnable = False;
#endif
		ZEnable = True;
		ZWriteEnable = FALSE;

#if SHOW_JUMPING_SPLASH == 0
		ColorWriteEnable = 0;
#endif
	}
};

ShaderConstsFarRain	g_farRainConsts = 
{
	2,	//fPeriod
	2,	//fYRepeat
	2,	//fXRepeatHalf
	{120,400,120},	//vSize
	
	0.7,	//fDecay
};
ShaderVariantsFarRain	g_farRainVariants = 
{
	1,	//fDensity
	1,	//fFadeOut
	{0,0,0},//vCamPos
};

struct vs_far_rain_out
{
	float4 pos : POSITION;
	float2 texCd : TEXCOORD0;
	float  originY : TEXCOORD1;
};

texture g_texFarRain;
sampler2D g_samFarRain = sampler_state
{
	Texture = <g_texFarRain>;
	MipFilter = LINEAR;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
};

float g_fFarRainCurLayer = 0;

void vs_far_rain(in float3 iPos : POSITION
				 , in float2 iTexCd : TEXCOORD0
				 , out vs_far_rain_out vsOut)
{
	float fTime = g_fRainTime;
#if SHOW_FAR_RAIN_ANIMATION != 1
	fTime = g_fRainNoAniTime;
#endif

	float3 posTmp = iPos;
	posTmp *= g_farRainConsts.vSize;
	posTmp.xz *= (g_fFarRainCurLayer + 1);

	//rotation according to rain movement
#if FAR_RAIN_ROTATE == 1
	{
		float3 vDirMovement = -g_rainStreakVariants.vSpeedDir;
		if(abs(vDirMovement.x) + abs(vDirMovement.z) > 0.1)
		{
			float3 vNewX = {g_rainStreakVariants.vSpeed.z, 0, -g_rainStreakVariants.vSpeed.x};
			vNewX = normalize(vNewX);

			float3 vNewZ = normalize(cross(vNewX, vDirMovement));

			float3x3 matRotation = {vNewX, vDirMovement, vNewZ};

			posTmp = mul(posTmp, matRotation);
		}	
	}
#endif

#if ENDLESS_FARRAIN_REPEAT == 1
	posTmp += g_farRainVariants.vCamPos.xyz;
#endif

	vsOut.pos = mul(float4(posTmp, 1), g_mWorldViewProjection);

#if SHOW_FAR_RAIN != 1
	vsOut.pos = float4(0,0,0,0);
#endif

	float fU = iTexCd.x;	
	fU *= g_farRainConsts.fXRepeatHalf; 
	fU *= g_farRainVariants.fDensity;

	float fV = iTexCd.y * g_farRainConsts.fYRepeat;
	vsOut.texCd = float2(fU, fV);

	{
		float fPeriod = g_farRainConsts.fPeriod;
		float fVMovement = frac(fTime / fPeriod);
		vsOut.texCd.y += fVMovement;
	}

	vsOut.originY = iPos.y;
}
float4 ps_far_rain(in vs_far_rain_out psIn):COLOR
{
#if SHOW_FAR_RAIN_TEX != 1
	return float4(psIn.texCd.xy, 0, 0);
#endif
	float valueTex = tex2Dlod(g_samFarRain, float4(psIn.texCd, 0,0));
	float heightAlpha = 1 - pow(abs(psIn.originY), 4);	//decay the cylinder along the y vector
	valueTex *= heightAlpha;
	valueTex *= g_farRainConsts.fDecay;

	valueTex += 0.5 * GetHightLight(valueTex);

	valueTex *= g_farRainVariants.fFadeOut;

	//if valueTex < 0.1 do not render

	return float4(valueTex,valueTex,valueTex,valueTex);
}

technique techFarRain
{
	pass p0
	{
		vertexShader = compile vs_3_0 vs_far_rain();
		pixelShader = compile ps_3_0 ps_far_rain();

#if SHOW_FAR_RAIN_TEX == 1
		AlphaBlendEnable = True;
		SrcBlend = InvDestColor;
		DestBlend = One;
#else
		AlphaBlendEnable = False;
#endif
		SeparateAlphaBlendEnable = False;

		FogEnable = FALSE;
		CullMode = CCW;
		ZEnable = False;
		Zfunc = LessEqual;
		ZWriteEnable = False;

		ColorWriteEnable = Alpha | Red | Green | Blue;
	}
};

struct vs_out_sky_box
{
	float4 pos : POSITION;
	float3 texCd : TEXCOORD0;
};

void vs_sky_box(in float3 iPos : POSITION,
				in float3 iNormal : NORMAL,
				out vs_out_sky_box vsOut)
{
	float3 posTmp = iPos * 100;
	posTmp += g_rainStreakVariants.vCamPos;
	vsOut.pos = mul(float4(posTmp, 1), g_mWorldViewProjection);

#if SHOW_SKYBOX != 1
	vsOut.pos = float4(0,0,0,0);
#endif

	vsOut.texCd = iPos;
}

float4 ps_sky_box(vs_out_sky_box psIn) : COLOR
{
	float3 texCd = normalize(psIn.texCd);
	float4 texValue = texCUBE(g_samSkyBox, texCd);
	return texValue;
}

technique techSkyBox
{
	pass p0
	{
		vertexShader = compile vs_3_0 vs_sky_box();
		pixelShader = compile ps_3_0 ps_sky_box();

		AlphaBlendEnable = FALSE;
		CullMode = CW;
		ZEnable = TRUE;
		ZWriteEnable = FALSE;
	}
};
#endif		//_RAIN_FX_INCLUDED

