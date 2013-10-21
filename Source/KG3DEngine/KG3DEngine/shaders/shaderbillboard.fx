#include "CommonParams.h"

texture g_txScene : TEXTURE;        // texture for scene rendering
float4x4 WorldViewProj;


// only used for vs_2_0 Shader Instancing
//vs_2_0 has at least 256 constant register, 
//( 256 - 14 ) / 2 = 121

#define g_nNumBatchInstance 220 //used be 240
float4 vec4BillboardPos[g_nNumBatchInstance];
float3 vec3Up;
float3 vec3Left;
float3 vec3LightDir;
float fScaleX = 40.0f;
float fScaleY = 40.0f;
float4 Ambient; 
float4 LightDiffuse;

float   cloudscale = 0.0001F;
float2  vCloudSpeed = 0;

texture g_texture : TEXTURE;
texture cloudtex;

sampler texturesample =
sampler_state
{
    Texture = <g_texture>;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
};


float Lighting(float3 vec3Normal, float3 vec3LightDir)
{
    float fReturn = max(0, dot(vec3Normal, vec3LightDir));
    return fReturn;    
}

//-----------------------------------------------------------------------------
// Name: VS_ShaderInstancing
// Type: Vertex shader (Shader Instancing)
// Desc: This shader computes standard transform and lighting for unlit, texture-mapped triangles.
//-----------------------------------------------------------------------------
void VS_ShaderInstancing(float3 vec3PosIn :POSITION,
                        float3 vec3Normal : NORMAL,
                        float2 vec2Tex: TEXCOORD0,
                        float2 vec2Size: TEXCOORD1,
                        float fIntanceIndex : TEXCOORD2,
                        float fVertexIndex : TEXCOORD3,
                        float4 inColor : COLOR0,
                        out float4 oPos : POSITION,
                        out float4 oColor : COLOR0,
                        out float3 oTex_fFog:  TEXCOORD0,
                        out float2 oTex2:  TEXCOORD1
                        )
{
	float3 vec3Pos  = vec4BillboardPos[fIntanceIndex].xyz ;
	oTex2 = vec3Pos.xz * cloudscale + vCloudSpeed ;
	//vec3Pos.xz += 10 * sin( Time  );

	if (fVertexIndex == 0)	//LeftUpper
	{
		vec3Pos += vec3Up * vec2Size.y - vec3Left * vec2Size.x;
	}
	else if (fVertexIndex == 1)//RightUpper
	{
		vec3Pos += vec3Up * vec2Size.y + vec3Left * vec2Size.x;
	}
	else if (fVertexIndex == 2)//RightLower
	{
		vec3Pos += -vec3Up * vec2Size.y + vec3Left * vec2Size.x;
	}
	else //LeftLower
	{
		vec3Pos += -vec3Up * vec2Size.y - vec3Left * vec2Size.x;
	}   
	oTex_fFog.xy = vec2Tex; 
	oPos = mul(float4(vec3Pos, 1.0f), WorldViewProj);
	oColor.rgb = LightDiffuse.rgb * inColor.rgb * Lighting(vec3Normal, -vec3LightDir) + Ambient.rgb;
	oColor.a = inColor.a;
	oTex_fFog.z = CalFogFactor(oPos.z);;
}

float4 PS(float3 vTex0_fFog : TEXCOORD0,
          float4 vColor : COLOR0 ) : COLOR0
{
    // Lookup texture and modulate it with diffuse
	float4 colorOut = tex2D( texturesample, vTex0_fFog.xy) * vColor * 2;
	return CalFogColor(colorOut, vTex0_fFog.z);
}

technique TShader_Instancing
{
    pass P0
    {
        VertexShader = compile vs_2_0 VS_ShaderInstancing();
        PixelShader  = compile ps_2_0 PS();

		FogEnable = False;	//Fog is calculated in the shader, do not use the d3d fog, it affects the final effect

        CullMode        = None;                                    
        ZEnable         = True;
        
        Lighting        = True;
        SpecularEnable = False;

        AlphaBlendEnable = False;
        SrcBlend        = SrcAlpha;          
        DestBlend       = InvSrcAlpha;
        AlphaTestEnable = True;
        AlphaFunc = GREATEREQUAL;
        AlphaRef        = 0x90;

        ColorArg1[0] = Texture;
        ColorArg2[0] = Diffuse;
        ColorOp[0] = Modulate2x;
        
        AlphaArg1[0] = Texture;
        AlphaArg2[0] = Current;
        AlphaOp[0] = SelectArg1;

        MinFilter[0] = Linear;
        MagFilter[0] = Linear;
        MipFilter[0] = Linear;

        ColorOp[1] = Modulate2x;
        ColorArg1[1] = Texture;
        ColorArg2[1] = Current;
        AlphaOp[1] = Disable;    
        
        TextureTransformFlags[0] = Disable;
     }
}