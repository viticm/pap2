texture tMainDepth;

sampler2D SamplerDepth = sampler_state
{
    Texture   = <tMainDepth>;
    MipFilter = NONE;
    MinFilter = Point;
    MagFilter = Point;
    ADDRESSU = Clamp;
    ADDRESSV = Clamp;
};

#ifndef PACKTYPE
#define PACKTYPE 0
#endif

static const int    MAX_SAMPLES            = 16; 
float ScalU = 1.0;
float ScalV = 1.0;
float OffsetU = 0.0;
float OffsetV = 0.0;
matrix matCameraPInv;
//x = near y = focal z = far w= cutoff
float4 vDofParams;

float4 ComputeDOFDepth(in float2 vScreenPosition : TEXCOORD0) : COLOR
{
    //float depth = tex2D(SamplerDepth, (vScreenPosition + float2(OffsetU, OffsetV)) * float2(ScalU,ScalV));
#if PACKTYPE == 1
	const float4 bitShifts = float4(1,1.0/256.0,1.0/(256.0*256.0),0);//float4(1.0/(256.0*256.0*256.0), 1.0/(256.0*256.0), 1.0/256.0, 1);
	float4 packDepth = tex2D(SamplerDepth, (vScreenPosition + float2(OffsetU, OffsetV)) * float2(ScalU,ScalV));
	float depth = dot(packDepth , bitShifts);
#else
	float depth = tex2D(SamplerDepth, (vScreenPosition + float2(OffsetU, OffsetV)) * float2(ScalU,ScalV));
#endif
    float2 uv;
    uv.x =  vScreenPosition.x * 2 - 1;
    uv.y = -vScreenPosition.y * 2 + 1;
	
    float4 pos_trans = {uv,depth,1}; 
    float4 pos_v = mul(pos_trans,matCameraPInv);//convert to View cord
	depth = pos_v.z / pos_v.w;
	float f = 0;
	if (depth < vDofParams.y)
	{
		f = (depth - vDofParams.y) / (vDofParams.y - vDofParams.x);
	}
	else
	{
		f = (depth - vDofParams.y) / (vDofParams.z - vDofParams.y);
		f = clamp(f,0,vDofParams.w);
	}
	
	float fColor = f * 0.5f + 0.5f;
	float4 DofDepth = float4(fColor,fColor,fColor,1.0);
	return DofDepth;
}

technique DOF
{
    pass ComputeDOFDepth
    {
        PixelShader  = compile ps_3_0 ComputeDOFDepth();
    }
}

texture tDOFDepth;
sampler2D SamplerDOFDepth = sampler_state
{
    Texture   = <tDOFDepth>;
    MipFilter = NONE;
    MinFilter = Point;
    MagFilter = Point;
    ADDRESSU = Clamp;
    ADDRESSV = Clamp;
};
texture tMainColor;
sampler2D SamplerMainColor = sampler_state
{
    Texture   = <tMainColor>;
    MipFilter = NONE;
    MinFilter = Point;
    MagFilter = Point;
    ADDRESSU = Clamp;
    ADDRESSV = Clamp;
};

float2 g_avSampleOffsets[MAX_SAMPLES];
float4 g_avSampleWeights[MAX_SAMPLES];

float4 DownScale4x4PS(in float2 vScreenPosition : TEXCOORD0,in float2 vScreenPosition2 : TEXCOORD1) : COLOR
{
    float4 sample = 0.0f;

		for( int i = 0; i < 16; i++ )
		{
				sample.rgb += tex2D( SamplerMainColor, vScreenPosition + g_avSampleOffsets[i] );
				sample.a += tex2D( SamplerDOFDepth, vScreenPosition2 + g_avSampleWeights[i] );
		}
    
		return sample / 16;
}

technique DownScale4x4
{
    pass P0
    {
        PixelShader  = compile ps_3_0 DownScale4x4PS();
    }
}


texture tSceneScale;
sampler2D SamplertSceneScale = sampler_state
{
    Texture   = <tSceneScale>;
    MipFilter = NONE;
    MinFilter = Linear;
    MagFilter = Linear;
    ADDRESSU = Clamp;
    ADDRESSV = Clamp;
};
/*
float2 g_vSampleOffsets[15];
float g_fSampleWeights[15];
// Horizontal blur pixel shader
float4 PS_BlurH(in float2 Tex : TEXCOORD0 ): COLOR0
{
   
   // Accumulated color
   float4 vAccum = float4( 0.0f, 0.0f, 0.0f, 0.0f );
   // Sample the taps (g_vSampleOffsets holds the texel offsets
   // and g_fSampleWeights holds the texel weights)
   for(int i = 0; i < 15; i++ )
   {
      vAccum += tex2D( SamplertSceneScale, Tex + g_vSampleOffsets[i] ) * g_fSampleWeights[i];
   }
   return vAccum;  
}
// Vertical blur pixel shader
float4 PS_BlurV(in float2 Tex : TEXCOORD0): COLOR0
{
   
   // Accumulated color
   float4 vAccum = float4( 0.0f, 0.0f, 0.0f, 0.0f );
   // Sample the taps (g_vSampleOffsets holds the texel offsets and
   // g_fSampleWeights holds the texel weights)
   for( int i = 0; i < 15; i++ )
   {
      vAccum += tex2D( SamplertSceneScale, Tex + g_vSampleOffsets[i] ) * g_fSampleWeights[i];
   }
   return vAccum;
}

technique Gussblur
{
	 pass p0
	 {
		PixelShader  = compile ps_3_0 PS_BlurH(); 
	 }
	 pass p1
	 {
		PixelShader  = compile ps_3_0 PS_BlurV(); 
	 }
}
*/
float4 pixSize;
float4 GaussianBlur(in float2 Tex : TEXCOORD0):COLOR0
{
	float4 result = float4(0.0f,0.0f,0.0f,0.0f);
	float2 texCoord;
	float3x3 matBlur= 
	{
		1.0,2.0,1.0,
		2.0,4.0,2.0,
		1.0,2.0,1.0
	};
	// stepU = 1.0 / screenResolutionWidth | stepV = 1.0 / screenResolutionHeight
	for(int i=0;i<3;i++) {
		for(int j=0;j<3;j++) {
			texCoord = Tex + float2((i-1)*pixSize.x,(j-1)*pixSize.y);
			result += matBlur[i][j] * tex2D(SamplertSceneScale,texCoord);
		}
	}
	result /= 16.0;

	return result;
}
technique Gussblur
{
	 pass p0
	 {
		PixelShader  = compile ps_3_0 GaussianBlur(); 
	 }
}