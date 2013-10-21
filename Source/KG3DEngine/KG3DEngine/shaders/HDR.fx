////////////////////////////////////////////////////////////////////////////////
//
//  Author      : ZhaoXinyu (Chiyer)
//  CopyRight   : Kingsoft Corporation All rights Reserved
//  Create Date : 2006-12-4 14:53:41
//  Comment     : hdr shader
//
////////////////////////////////////////////////////////////////////////////////


// -------------------------------------------------------------
sampler s0 : register(s0);
sampler s1 : register(s1);
sampler s2 : register(s2);
sampler s3 : register(s3);
sampler s4 : register(s4);
sampler s5 : register(s5);
sampler s6 : register(s6);
sampler s7 : register(s7);
	
	
static const int    MAX_SAMPLES            = 16;   
static const float  BRIGHT_PASS_THRESHOLD  = 5.0f;  
static const float  BRIGHT_PASS_OFFSET     = 10.0f; 

static const float3 LUMINANCE_VECTOR  = float3(0.2125f, 0.7154f, 0.0721f);
static const float3 BLUE_SHIFT_VECTOR = float3(1.05f, 0.97f, 1.27f); 	


float2 g_avSampleOffsets[MAX_SAMPLES];
float4 g_avSampleWeights[MAX_SAMPLES];

float  g_fMiddleGray;		
float  g_fElapsedTime;      





float4 DownScale4x4PS(in float2 vScreenPosition : TEXCOORD0) : COLOR
{
    float4 sample = 0.0f;

		for( int i = 0; i < 16; i++ )
		{
				sample += tex2D( s0, vScreenPosition + g_avSampleOffsets[i] );
		}
    
		return sample / 16;
}

technique DownScale4x4
{
    pass P0
    {
        PixelShader  = compile ps_2_0 DownScale4x4PS();
    }
}



float4 SampleLumInitial(in float2 vScreenPosition : TEXCOORD0) : COLOR
{
    float3 vSample = 0.0f;
    float  fLogLumSum = 0.0f;

    for(int iSample = 0; iSample < 9; iSample++)
    {
        vSample = tex2D(s0, vScreenPosition + g_avSampleOffsets[iSample]);
        fLogLumSum += log(dot(vSample, LUMINANCE_VECTOR) + 0.0001f);
    }
    
    fLogLumSum /= 9;

    return float4(fLogLumSum, fLogLumSum, fLogLumSum, 1.0f);
}

technique SampleAvgLum
{
    pass P0
    {
        PixelShader  = compile ps_2_0 SampleLumInitial();
    }
}






float4 SampleLumIterative(in float2 vScreenPosition : TEXCOORD0) : COLOR
{
    float fResampleSum = 0.0f; 
    
    for(int iSample = 0; iSample < 16; iSample++)
    {
        fResampleSum += tex2D(s0, vScreenPosition+g_avSampleOffsets[iSample]);
    }
    
    fResampleSum /= 16;

    return float4(fResampleSum, fResampleSum, fResampleSum, 1.0f);
}

technique ResampleAvgLum
{
    pass P0
    {
        PixelShader  = compile ps_2_0 SampleLumIterative();
    }
}




float4 SampleLumFinal(in float2 vScreenPosition : TEXCOORD0) : COLOR
{
    float fResampleSum = 0.0f;
    
    for(int iSample = 0; iSample < 16; iSample++)
    {
        fResampleSum += tex2D(s0, vScreenPosition + g_avSampleOffsets[iSample]);
    }
    
    fResampleSum = exp(fResampleSum / 16);
    
    return float4(fResampleSum, fResampleSum, fResampleSum, 1.0f);
}

technique ResampleAvgLumExp
{
    pass P0
    {
        PixelShader  = compile ps_2_0 SampleLumFinal();
    }
}





float4 BloomPS(in float2 vScreenPosition : TEXCOORD0) : COLOR
{
    float4 vSample = 0.0f;
    float4 vColor = 0.0f;
        
    float2 vSamplePosition;
    
    for(int iSample = 0; iSample < 15; iSample++)
    {
        vSamplePosition = vScreenPosition + g_avSampleOffsets[iSample];
        vColor = tex2D(s0, vSamplePosition);
        vSample += g_avSampleWeights[iSample] * vColor;
    }
    
    return vSample;
}

technique Bloom
{
    pass P0
    {        
        PixelShader  = compile ps_2_0 BloomPS();
    }

}





float4 GaussBlur5x5PS(in float2 vScreenPosition : TEXCOORD0) : COLOR
{
	
    float4 sample = 0.0f;

		for( int i = 0; i < 12; i++ )
		{
				sample += g_avSampleWeights[i] * tex2D( s0, vScreenPosition + g_avSampleOffsets[i] );
		}

		return sample;
}

technique GaussBlur5x5
{
    pass P0
    {
        PixelShader  = compile ps_2_0 GaussBlur5x5PS();
    }
}





float4 DownScale2x2PS(in float2 vScreenPosition : TEXCOORD0) : COLOR
{
    float4 sample = 0.0f;

		for( int i=0; i < 4; i++ )
		{
				sample += tex2D( s0, vScreenPosition + g_avSampleOffsets[i] );
		}
    
		return sample / 4;
}

technique DownScale2x2
{
    pass P0
    {
        PixelShader  = compile ps_2_0 DownScale2x2PS();
    }
}





float4 CalculateAdaptedLumPS(in float2 vScreenPosition : TEXCOORD0) : COLOR
{
    float fAdaptedLum = tex2D(s0, float2(0.5f, 0.5f));
    float fCurrentLum = tex2D(s1, float2(0.5f, 0.5f));
    
    float fNewAdaptation = fAdaptedLum + (fCurrentLum - fAdaptedLum) * ( 1 - pow( 0.98f, 30 * g_fElapsedTime ) );
    return float4(fNewAdaptation, fNewAdaptation, fNewAdaptation, 1.0f);
}

technique CalculateAdaptedLum
{
    pass P0
    {
        PixelShader  = compile ps_2_0 CalculateAdaptedLumPS();
    }
}






float BrightGate = 0.1f;		
float Rectify		 = 0.8f;
float GrayLevel = 0.15f;
float4 BrightPassFilterPS(in float2 vScreenPosition : TEXCOORD0) : COLOR
{
	float4 vSample = tex2D( s0, vScreenPosition );
	float  fAdaptedLum = tex2D( s1, float2(0.5f, 0.5f) );	

	vSample.rgb *= GrayLevel / (fAdaptedLum + 0.001f);
	
	vSample.rgb -= BrightGate;
	
	vSample = max(vSample, 0.0f);
	
	vSample.rgb /= (Rectify + vSample);
    
	return vSample;
}

technique BrightPassFilter
{
    pass P0
    {
        PixelShader  = compile ps_2_0 BrightPassFilterPS();
    }
}



float4 StarPS(in float2 vScreenPosition : TEXCOORD0) : COLOR
{
    float4 vSample = 0.0f;
    float4 vColor = 0.0f;
        
    float2 vSamplePosition;
    
    // Sample from eight points along the star line
    for(int iSample = 0; iSample < 8; iSample++)
    {
        vSamplePosition = vScreenPosition + g_avSampleOffsets[iSample];
        vSample = tex2D(s0, vSamplePosition);
        vColor += g_avSampleWeights[iSample] * vSample;
    }
    	
    return vColor;
}

technique Star
{
    pass P0
    {        
        PixelShader  = compile ps_2_0 StarPS();
    }

}


float4 MergeTextures_1PS(in float2 vScreenPosition : TEXCOORD0) : COLOR
{
	float4 vColor = 0.0f;
	
	vColor += g_avSampleWeights[0] * tex2D(s0, vScreenPosition);
		
	return vColor;
}


float4 MergeTextures_2PS(in float2 vScreenPosition : TEXCOORD0) : COLOR
{
	float4 vColor = 0.0f;
	
	vColor += g_avSampleWeights[0] * tex2D(s0, vScreenPosition);
	vColor += g_avSampleWeights[1] * tex2D(s1, vScreenPosition);
		
	return vColor;
}

float4 MergeTextures_3PS(in float2 vScreenPosition : TEXCOORD0) : COLOR
{
	float4 vColor = 0.0f;
	
	vColor += g_avSampleWeights[0] * tex2D(s0, vScreenPosition);
	vColor += g_avSampleWeights[1] * tex2D(s1, vScreenPosition);
	vColor += g_avSampleWeights[2] * tex2D(s2, vScreenPosition);
		
	return vColor;
}

float4 MergeTextures_4PS(in float2 vScreenPosition : TEXCOORD0) : COLOR
{
	float4 vColor = 0.0f;
	
	vColor += g_avSampleWeights[0] * tex2D(s0, vScreenPosition);
	vColor += g_avSampleWeights[1] * tex2D(s1, vScreenPosition);
	vColor += g_avSampleWeights[2] * tex2D(s2, vScreenPosition);
	vColor += g_avSampleWeights[3] * tex2D(s3, vScreenPosition);
		
	return vColor;
}


float4 MergeTextures_5PS(in float2 vScreenPosition : TEXCOORD0) : COLOR
{
	float4 vColor = 0.0f;
	
	vColor += g_avSampleWeights[0] * tex2D(s0, vScreenPosition);
	vColor += g_avSampleWeights[1] * tex2D(s1, vScreenPosition);
	vColor += g_avSampleWeights[2] * tex2D(s2, vScreenPosition);
	vColor += g_avSampleWeights[3] * tex2D(s3, vScreenPosition);
	vColor += g_avSampleWeights[4] * tex2D(s4, vScreenPosition);
		
	return vColor;
}


float4 MergeTextures_6PS(in float2 vScreenPosition : TEXCOORD0) : COLOR
{
	float4 vColor = 0.0f;
	
	vColor += g_avSampleWeights[0] * tex2D(s0, vScreenPosition);
	vColor += g_avSampleWeights[1] * tex2D(s1, vScreenPosition);
	vColor += g_avSampleWeights[2] * tex2D(s2, vScreenPosition);
	vColor += g_avSampleWeights[3] * tex2D(s3, vScreenPosition);
	vColor += g_avSampleWeights[4] * tex2D(s4, vScreenPosition);
	vColor += g_avSampleWeights[5] * tex2D(s5, vScreenPosition);
		
	return vColor;
}

float4 MergeTextures_7PS(in float2 vScreenPosition : TEXCOORD0) : COLOR
{
	float4 vColor = 0.0f;
	
	vColor += g_avSampleWeights[0] * tex2D(s0, vScreenPosition);
	vColor += g_avSampleWeights[1] * tex2D(s1, vScreenPosition);
	vColor += g_avSampleWeights[2] * tex2D(s2, vScreenPosition);
	vColor += g_avSampleWeights[3] * tex2D(s3, vScreenPosition);
	vColor += g_avSampleWeights[4] * tex2D(s4, vScreenPosition);
	vColor += g_avSampleWeights[5] * tex2D(s5, vScreenPosition);
	vColor += g_avSampleWeights[6] * tex2D(s6, vScreenPosition);
		
	return vColor;
}


float4 MergeTextures_8PS(in float2 vScreenPosition : TEXCOORD0) : COLOR
{
	float4 vColor = 0.0f;
	
	vColor += g_avSampleWeights[0] * tex2D(s0, vScreenPosition);
	vColor += g_avSampleWeights[1] * tex2D(s1, vScreenPosition);
	vColor += g_avSampleWeights[2] * tex2D(s2, vScreenPosition);
	vColor += g_avSampleWeights[3] * tex2D(s3, vScreenPosition);
	vColor += g_avSampleWeights[4] * tex2D(s4, vScreenPosition);
	vColor += g_avSampleWeights[5] * tex2D(s5, vScreenPosition);
	vColor += g_avSampleWeights[6] * tex2D(s6, vScreenPosition);
	vColor += g_avSampleWeights[7] * tex2D(s7, vScreenPosition);
		
	return vColor;
}

technique MergeTextures_1
{
    pass P0
    {        
        PixelShader  = compile ps_2_0 MergeTextures_1PS();
    }

}


technique MergeTextures_2
{
    pass P0
    {        
        PixelShader  = compile ps_2_0 MergeTextures_2PS();
    }

}

technique MergeTextures_3
{
    pass P0
    {        
        PixelShader  = compile ps_2_0 MergeTextures_3PS();
    }

}

technique MergeTextures_4
{
    pass P0
    {        
        PixelShader  = compile ps_2_0 MergeTextures_4PS();
    }

}

technique MergeTextures_5
{
    pass P0
    {        
        PixelShader  = compile ps_2_0 MergeTextures_5PS();
    }

}

technique MergeTextures_6
{
    pass P0
    {        
        PixelShader  = compile ps_2_0 MergeTextures_6PS();
    }

}

technique MergeTextures_7
{
    pass P0
    {        
        PixelShader  = compile ps_2_0 MergeTextures_7PS();
    }

}


technique MergeTextures_8
{
    pass P0
    {        
        PixelShader  = compile ps_2_0 MergeTextures_8PS();
    }

}


