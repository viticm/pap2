////////////////////////////////////////////////////////////////////////////////
//
//  Author      : ZhaoXinyu (Chiyer)
//  CopyRight   : Kingsoft Corporation All rights Reserved
//  Create Date : 2006-12-4 14:53:41
//  Comment     : hdr shader
//
////////////////////////////////////////////////////////////////////////////////


// -------------------------------------------------------------
static const int    MAX_SAMPLES            = 16;   

sampler s0 : register(s0);
sampler s1 : register(s1);
sampler s2 : register(s2);
sampler s3 : register(s3);
sampler s4 : register(s4);
sampler s5 : register(s5);


float2 g_avSampleOffsets[MAX_SAMPLES];
float4 g_avSampleWeights[MAX_SAMPLES];

float Gate = 0.4f;

float MaxU = 1.0;
float MaxV = 1.0;
float4 DownScale4x4_PS(in float2 vScreenPosition : TEXCOORD0) : COLOR
{
    float4 sample = 0.0f;

		for( int i = 0; i < 16; i++ )
		{
				sample += tex2D( s0, (vScreenPosition + g_avSampleOffsets[i]) * float2(MaxU,MaxV));
		}
    
		sample = sample / 16;
		
		//sample = max(0.0f, sample - Gate);
		
		float gray = (sample.r + sample.g + sample.b) / 3.0f;
		
		gray = max(0.0f, gray - Gate);
		
		return float4(gray, gray, gray, 1.0f);
}

technique DownScale4x4
{
    pass P0
    {
        PixelShader  = compile ps_3_0 DownScale4x4_PS();
    }
}

float4 GaussBlur5x5_PS(in float2 vScreenPosition : TEXCOORD0) : COLOR
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
        PixelShader  = compile ps_2_0 GaussBlur5x5_PS();
    }
}

float4 DownScale2x2_PS(in float2 vScreenPosition : TEXCOORD0) : COLOR
{
    float4 sample = 0.0f;

		for( int i=0; i < 4; i++ )
		{
				sample += tex2D( s0, vScreenPosition + g_avSampleOffsets[i] );
		}
    
    sample /= 4;
    
		return sample;
}

technique DownScale2x2
{
    pass P0
    {
        PixelShader  = compile ps_2_0 DownScale2x2_PS();
    }
}

float4 Bloom_PS(in float2 vScreenPosition : TEXCOORD0, uniform int SampNum) : COLOR
{
    float4 vSample = 0.0f;
    float4 vColor = 0.0f;
        
    float2 vSamplePosition;
    
    for(int iSample = 0; iSample < SampNum; iSample++)
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
        PixelShader  = compile ps_2_0 Bloom_PS(2);
    }
    
    pass P1
    {        
        PixelShader  = compile ps_2_0 Bloom_PS(4);
    }
    
    pass P2
    {        
        PixelShader  = compile ps_2_0 Bloom_PS(6);
    }
    
    pass P3
    {        
        PixelShader  = compile ps_2_0 Bloom_PS(8);
    }
    
    pass P4
    {        
        PixelShader  = compile ps_2_0 Bloom_PS(10);
    }
    
    pass P5
    {        
        PixelShader  = compile ps_2_0 Bloom_PS(12);
    }
    
    pass P6
    {        
        PixelShader  = compile ps_2_0 Bloom_PS(14);
    }
    
    pass P7
    {        
        PixelShader  = compile ps_2_0 Bloom_PS(16);
    }
}