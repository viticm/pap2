////////////////////////////////////////////////////////////////////////////////
//
//  Author      : ZhaoXinyu (Chiyer)
//  CopyRight   : Kingsoft Corporation All rights Reserved
//  Create Date : 2006-12-4 14:53:41
//  Comment     : detial shader
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
sampler s6 : register(s6);


#define M_PI 3.14159265358979323846
#define fmodp(x,n) ((n)*frac((x)/(n)))

float2 g_avSampleOffsets[MAX_SAMPLES];
float4 g_avSampleWeights[MAX_SAMPLES];

float Gate = 0.4f;

float2 our_rand(float2 ij)
{
  const float4 a=float4(M_PI * M_PI * M_PI * M_PI, exp(4.0),  1.0, 0.0);
  const float4 b=float4(pow(13.0, M_PI / 2.0), sqrt(1997.0),  0.0, 1.0);

  float2 xy0    = ij/M_PI;
  float2 xym    = fmodp(xy0.xy,257.0)+1.0;
  float2 xym2   = frac(xym*xym);
  float4 pxy    = float4(xym.yx * xym2 , frac(xy0));
  float2 xy1    = float2(dot(pxy,a) , dot(pxy,b));
  float2 result = frac(xy1);
  
  return (result*256.0);
}

float4 LocalAverage_PS(in float2 vScreenPosition : TEXCOORD0) : COLOR
{
    float4 sample = 0.0f;

        for( int i = 0; i < 16; i++ )
        {
                sample += tex2D( s0, vScreenPosition + g_avSampleOffsets[i] );
        }
    
        sample = sample / 16;
        
        //sample = max(0.0f, sample - Gate);
        
        float gray = (sample.r + sample.g + sample.b) / 3.0f;
        
        gray = max(0.0f, gray - Gate);
        
        return sample ;//float4(gray, gray, gray, 1.0f);
}

technique LocalAverage
{
    pass P0
    {
        ADDRESSU[0]  = CLAMP;
        ADDRESSV[0]  = CLAMP;
        ADDRESSU[1]  = CLAMP;
        ADDRESSV[1]  = CLAMP;
        ADDRESSU[2]  = CLAMP;
        ADDRESSV[2]  = CLAMP;
        ADDRESSU[3]  = CLAMP;
        ADDRESSV[3]  = CLAMP;
        PixelShader  = compile ps_2_0 LocalAverage_PS();
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////
float4 Textureness_PS(in float2 vScreenPosition : TEXCOORD0) : COLOR
{
    float4 color = tex2D( s0,vScreenPosition);
    float4 avg  = tex2D( s1,vScreenPosition);

    return (color - avg + 1.f) / 2.f;
/*
    float4 K = (color - avg)+0.5  ;
    //float2 T = our_rand(K .xz*0.01) / 256;
    return K ;
    */
}

technique Textureness
{
    pass P0
    {
        ADDRESSU[0]  = CLAMP;
        ADDRESSV[0]  = CLAMP;
        ADDRESSU[1]  = CLAMP;
        ADDRESSV[1]  = CLAMP;
        ADDRESSU[2]  = CLAMP;
        ADDRESSV[2]  = CLAMP;
        ADDRESSU[3]  = CLAMP;
        ADDRESSV[3]  = CLAMP;
        PixelShader  = compile ps_2_0 Textureness_PS();
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


float4 Detail_PS(in float2 vScreenPosition : TEXCOORD0) : COLOR
{
    float4 vSample = 0.0f;
    float4 vColor = 0.0f;
        
    float2 vSamplePosition;
    
    for(int iSample = 0; iSample <= 15; iSample++)
    {
        vSamplePosition = vScreenPosition + g_avSampleOffsets[iSample];
        vColor = tex2D(s0, vSamplePosition);
        vSample += g_avSampleWeights[iSample] * vColor;
    }
    
    return vSample;
}

technique Detail
{
    pass P0
    {        
        PixelShader  = compile ps_2_0 Detail_PS();
    }
}