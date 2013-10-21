////////////////////////////////////////////////////////////////////////////////
//
//  Author      : ZhaoXinyu (Chiyer)
//  CopyRight   : Kingsoft Corporation All rights Reserved
//  Create Date : 2006-12-4 14:53:41
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////


// ----------------- samplers ----------------------    

sampler s0 : register(s0);
sampler s1 : register(s1);
sampler s2 : register(s2);
sampler s3 : register(s3);
sampler s4 : register(s4);        
sampler s5 : register(s5); 
sampler s6 : register(s6);
sampler s7 : register(s7);
    
float4x4 g_MatViewPorjInv;
float4x4 g_MatViewPorj;
float4x4 g_PrvMatViewPorj;
float4x4 g_ProjInv;
float4x4 g_Proj;
float4x4 g_ViewInv;
float MaxU = 1.0;
float MaxV = 1.0;

const float PI2 = 6.2831853;
const float rt3 = 1.0 / sqrt(3);

int   UseMotionBlur = 0;    

#ifndef PACKTYPE
#define PACKTYPE 0
#endif

// ------------use to make  shock_wave -------------

float WavePower = 0.02f;

float2 shock_wave(float2 Tex : TEXCOORD0)
{
        float4 wave = tex2D(s1, Tex);
        float UOffset = (wave.r - 0.501960784313f) * WavePower;
        float VOffset = (wave.g - 0.501960784313f) * WavePower;
        Tex = Tex - float2(UOffset, VOffset);
       
    return Tex;
}

// ------------------- bloom -----------------------

float Exposure  = 0.5f;
float Exposure1 = 1.0f;
float Exposure2 = 0.5f;

float4 FinalPassBloomPS(in float2 Tex : TEXCOORD0) : COLOR
{
    float4 vColor = tex2D(s0, Tex);
    float4 vBloom = tex2D(s1, Tex * float2(1.0 / MaxU,1.0 / MaxV));   
            
    vColor.rgb = vColor.rgb * Exposure1 + vBloom.rgb * Exposure2;

    vColor.a = vColor.a;
    return vColor;
}
    
technique FinalPassBloom
{
    pass p0
    {
        PixelShader = compile ps_2_0 FinalPassBloomPS();
    }
}

// ------------------- detail  -----------------------

float detail_power = 1.f;
float blur_power = 0.f;
float blur_dis = 20000.f;

float4 FinalPassDetailPS(in float2 Tex : TEXCOORD0) : COLOR
{
    float4 vColor   = tex2D(s1, Tex);
    float4 vDetail2 = tex2D(s2, Tex);   
    float4 vDetail1 = tex2D(s3, Tex);   
    float4 vDetail0 = tex2D(s4, Tex);   
    //float depth     = tex2D(s5, Tex);
#if PACKTYPE == 1
	const float4 bitShifts = float4(1,1.0/256.0,1.0/(256.0*256.0),0);//float4(1.0/(256.0*256.0*256.0), 1.0/(256.0*256.0), 1.0/256.0, 1);
	float4 packDepth = tex2D(s5, Tex);
	float depth = dot(packDepth , bitShifts);
#else
	float depth = tex2D(s5, Tex);
#endif
    
    float4 hpos = float4(Tex.x * 2.f - 1.f, 1.f - Tex.y * 2.f, depth, 1.f);
    float4 vpos = mul(hpos, g_ProjInv);
    vpos /= vpos.w;
   
	float4 color1 = vColor + (vDetail2 * 2.f - 1.f) * 0.2f;
	float4 color2 = vColor 
	                + (vDetail2 * 2.f - 1.f) * 1.2f
	                + (vDetail1 * 2.f - 1.f) * 1.5f 
                    + (vDetail0 * 2.f - 1.f) * 2.0f;
                   
    if (vpos.z  < 40000.f)
    {
        float k = lerp(detail_power, blur_power, saturate(vpos.z / blur_dis));
        return lerp(color1, color2, k);
    }
    else
        return tex2D(s0, Tex);

                   
              /*      
    float k = 0.7f;
    
    if (depth < 0.9999f)
        k = lerp(0.f, 0.9f, saturate((1.f - depth) * 100.f));
   //     k = max(0, min(0.85, (1 - vDepth) * 1000));
    */
         
    
                      
}
    
technique FinalPassDetail
{
    pass p0
    {
        ADDRESSU[0]  = CLAMP;
        ADDRESSV[0]  = CLAMP;
        ADDRESSU[1]  = CLAMP;
        ADDRESSV[1]  = CLAMP;
        ADDRESSU[2]  = CLAMP;
        ADDRESSV[2]  = CLAMP;
        ADDRESSU[3]  = CLAMP;
        ADDRESSV[3]  = CLAMP;
        PixelShader = compile ps_2_0 FinalPassDetailPS();
    }
}
// ------------------- shockwave ------------------------    

/*
float4 FinalPassWavePS(in float2 Tex : TEXCOORD0) : COLOR
{
        return tex2D(s0, shock_wave(Tex));
}*/

/*
float4 FinalPassWavePS(in float2 Tex : TEXCOORD0) : COLOR
{
        float4 wave = tex2D(s1, Tex);
        
    //    return wave.r;
        
        float UOffset = (wave.r - 0.501960784313f) * WavePower;
        float VOffset = (wave.g - 0.501960784313f) * WavePower;
        float2 newTex = Tex - float2(UOffset, VOffset);
        return tex2D(s0, newTex);
}*/

float4 FinalPassWavePS(in float2 Tex : TEXCOORD0) : COLOR
{
        float4 wave = tex2D(s1, Tex);
        //float4 bump = tex2D(s2, Tex * 0.5f + vWaveParam.xy * 4) * 2.f - 1.f ;
        float power = (wave.r + wave.g + wave.b) / 3.f * 0.08f;
        float4 colr = tex2D(s0, Tex - float2(power, power));        

        return colr;
}

technique FinalPassWave
{
    pass p0
    {
        PixelShader = compile ps_2_0 FinalPassWavePS();
        MinFilter[0] = LINEAR;
        MagFilter[0] = LINEAR;
        MipFilter[0] = LINEAR;
        MinFilter[1] = LINEAR;
        MagFilter[1] = LINEAR;
        MipFilter[1] = LINEAR;
        MinFilter[2] = LINEAR;
        MagFilter[2] = LINEAR;
        MipFilter[2] = LINEAR;
    }
}

float4 vWaveParam = float4(0.f, 0.f, 0.f, 0.f);

float4 FinalPassWaveUnderWaterPS(in float2 tex_src : TEXCOORD0) : COLOR
{
    half2 wave = vWaveParam.xy * float2(40.f, 40.f);
    
    half2 wave0 = tex_src.xy + wave * 2.f;
    half2 wave1 = tex_src.xy * 2.f + wave * 4.f;
    half2 wave2 = tex_src.xy * 4.f + wave * 2.f;
    half2 wave3 = tex_src.xy * 8.f + wave;  
    
    half3 tex0 = tex2D(s2, wave0.xy * 0.02).xyz;
    half3 tex1 = tex2D(s2, wave1.xy * 0.02).xyz;
    half3 tex2 = tex2D(s2, wave2.xy * 0.02).xyz;
    half3 tex3 = tex2D(s2, wave3.xy * 0.02).xyz;
    
    half3 bump = normalize(2.f * (tex0.xyz + tex1.xyz + tex2.xyz + tex3.xyz) - 4.f);
    
    half3 texw = bump.xyz * half3(0.038f, 0.028f, 1.f);    
    half4 colr = tex2D(s0, tex_src + texw.xy);

    return colr;

}

technique FinalPassWaveUnderWater
{
    pass p0
    {
        PixelShader = compile ps_2_0 FinalPassWaveUnderWaterPS();
        AddressU[0] = CLAMP;
        AddressV[0] = CLAMP;
        MinFilter[0] = LINEAR;
        MagFilter[0] = LINEAR;
        MipFilter[0] = LINEAR;
        MinFilter[1] = LINEAR;
        MagFilter[1] = LINEAR;
        MipFilter[1] = LINEAR;
        MinFilter[2] = LINEAR;
        MagFilter[2] = LINEAR;
        MipFilter[2] = LINEAR;
    }
}



// --------------------- HSI --------------------------    

float HueScale                  = 1.0f;
float HueBias                = 0.0f;
float SaturationScale  = 1.0f;
float SaturationBias   = 0.0f;
float IntensityScale   = 1.0f;
float IntensityBias    = 0.0f;

float4 FinalPassHSIPS(float2 texCoord: TEXCOORD) : COLOR {
        
   float4 rgba = tex2D(s0, texCoord);

     // --------- rgb to hsi ----------
     
   float r = rgba.r;
   float g = rgba.g;
   float b = rgba.b;

   float rg = r - g;
   float rb = r - b;
   
   // Hue
   float h = acos((rg + rb) / (2 * sqrt(rg * rg + rb * (g - b)))) / PI2;
   if (b > g) h = 1 - h;

   // Intensity
   float i = (r + g + b) / 3;
   
   // Saturation
   float s = 1 - min(min(r, g), b) / i;
   
   
   
   // -------- hsi to rgb --------------
   
   // Scale and bias our components
   h = saturate(h * HueScale + HueBias);
   s = saturate(s * SaturationScale + SaturationBias);
   i = saturate(i * IntensityScale + IntensityBias);

   float h3 = 3 * h;
   float x = (2 * floor(h3) + 1) / 6;

   // Get our rgb components
   r = (1 - s) * i;
   float H = rt3 * tan((h - x) * PI2);
   b = ((3 + 3 * H) * i - (1 + 3 * H) * r) / 2;
   g = 3 * i - b - r;

   // Put them in right order
   float3 rgb;
   if (h3 < 1){
      rgb = float3(g, b, r);
   } else if (h3 < 2){
      rgb = float3(r, g, b);
   } else {
      rgb = float3(b, r, g);
   }

   return float4(rgb, rgba.a);
   
}

technique FinalPassHSI
{
    pass p0
    {
          ADDRESSU[0]  = CLAMP;
          ADDRESSV[0]  = CLAMP;
        MinFilter[0] = Linear;
        MagFilter[0] = Linear;
        MipFilter[0] = Linear;

        PixelShader  = compile ps_3_0 FinalPassHSIPS();
    }
}

float cmyk_kc = 0.f;
float cmyk_km = 0.f;
float cmyk_ky = 0.f;

float4 FinalPassCurveCMYK_PS(float2 texCoord: TEXCOORD) : COLOR {
    
    float4 color = tex2D(s0, texCoord);
    
    // color curve
    color.r = tex2D(s1, float2(color.r, 0.f)).r;
    color.g = tex2D(s1, float2(color.g, 0.f)).g;
    color.b = tex2D(s1, float2(color.b, 0.f)).b;
    
    
    // rgb to cmyk
    float k = min(min(1.f - color.r, 1.f - color.g), 1.f - color.b);
    float c = (1.f - color.r - k) / (1.f - k);
    float m = (1.f - color.g - k) / (1.f - k);
    float y = (1.f - color.b - k) / (1.f - k);
    
    c = min(max(c + cmyk_kc, 0.f), 1.f);
    m = min(max(m + cmyk_km, 0.f), 1.f);
    y = min(max(y + cmyk_ky, 0.f), 1.f); 
    
    // cmyk to rgb
    color.r = (1.f - k) * (1.f - c);
    color.g = (1.f - k) * (1.f - m);
    color.b = (1.f - k) * (1.f - y);
    
    return color;
}

technique FinalPassCurveCMYK
{
    pass p0
    {
        ADDRESSU[0]  = CLAMP;
        ADDRESSV[0]  = CLAMP;
        ADDRESSU[1]  = CLAMP;
        ADDRESSV[1]  = CLAMP;
        MinFilter[0] = LINEAR;
        MagFilter[0] = LINEAR;
        MipFilter[0] = LINEAR;
        MinFilter[1] = POINT;
        MagFilter[1] = POINT;
        MipFilter[1] = POINT;

        PixelShader  = compile ps_2_0 FinalPassCurveCMYK_PS();
    }
}



// ------------------- bloom hsi -----------------------
float4 FinalPassBloomHSIPS(in float2 Tex : TEXCOORD0) : COLOR
{
    float4 rgba   = tex2D(s0, Tex);
    float4 vBloom = tex2D(s1, Tex);   
            
    rgba.rgb += vBloom * Exposure;

    
   // --------- rgb to hsi ----------
     
   float r = rgba.r;
   float g = rgba.g;
   float b = rgba.b;

   float rg = r - g;
   float rb = r - b;
   
   // Hue
   float h = acos((rg + rb) / (2 * sqrt(rg * rg + rb * (g - b)))) / PI2;
   if (b > g) h = 1 - h;

   // Intensity
   float i = (r + g + b) / 3;
   
   // Saturation
   float s = 1 - min(min(r, g), b) / i;
   
   
   // -------- hsi to rgb --------------
   
   // Scale and bias our components
   h = saturate(h * HueScale + HueBias);
   s = saturate(s * SaturationScale + SaturationBias);
   i = saturate(i * IntensityScale + IntensityBias);

   float h3 = 3 * h;
   float x = (2 * floor(h3) + 1) / 6;

   // Get our rgb components
   r = (1 - s) * i;
   float H = rt3 * tan((h - x) * PI2);
   b = ((3 + 3 * H) * i - (1 + 3 * H) * r) / 2;
   g = 3 * i - b - r;

   // Put them in right order
   float3 rgb;
   if (h3 < 1){
      rgb = float3(g, b, r);
   } else if (h3 < 2){
      rgb = float3(r, g, b);
   } else {
      rgb = float3(b, r, g);
   }

   return float4(rgb, rgba.a);
}
    
technique FinalPassBloomHSI
{
    pass p0
    {
        PixelShader = compile ps_3_0 FinalPassBloomHSIPS();
    }
}

// ------------------- bloom shock wave -----------------------

float4 FinalPassBloomWavePS(in float2 Tex : TEXCOORD0) : COLOR
{
    float4 vColor = tex2D(s0, shock_wave(Tex));
    float4 vBloom = tex2D(s1, Tex);   
            
    vColor.rgb += vBloom * Exposure;

    vColor.a = vColor.a;
    return vColor;
}
    
technique FinalPassBloomWave
{
    pass p0
    {
        PixelShader = compile ps_2_0 FinalPassBloomWavePS();
    }
}


// ------------------- shock wave hsi -----------------------
float4 FinalPassWaveHSIPS(in float2 Tex : TEXCOORD0) : COLOR
{
    float4 rgba   = tex2D(s0, shock_wave(Tex));
    
   // --------- rgb to hsi ----------
     
   float r = rgba.r;
   float g = rgba.g;
   float b = rgba.b;

   float rg = r - g;
   float rb = r - b;
   
   // Hue
   float h = acos((rg + rb) / (2 * sqrt(rg * rg + rb * (g - b)))) / PI2;
   if (b > g) h = 1 - h;

   // Intensity
   float i = (r + g + b) / 3;
   
   // Saturation
   float s = 1 - min(min(r, g), b) / i;
   
   
   // -------- hsi to rgb --------------
   
   // Scale and bias our components
   h = saturate(h * HueScale + HueBias);
   s = saturate(s * SaturationScale + SaturationBias);
   i = saturate(i * IntensityScale + IntensityBias);

   float h3 = 3 * h;
   float x = (2 * floor(h3) + 1) / 6;

   // Get our rgb components
   r = (1 - s) * i;
   float H = rt3 * tan((h - x) * PI2);
   b = ((3 + 3 * H) * i - (1 + 3 * H) * r) / 2;
   g = 3 * i - b - r;

   // Put them in right order
   float3 rgb;
   if (h3 < 1){
      rgb = float3(g, b, r);
   } else if (h3 < 2){
      rgb = float3(r, g, b);
   } else {
      rgb = float3(b, r, g);
   }

   return float4(rgb, rgba.a);
}
    
technique FinalPassWaveHSI
{
    pass p0
    {
        PixelShader = compile ps_3_0 FinalPassWaveHSIPS();
    }
}

// ------------------- bloom shock wave hsi -----------------------
float4 FinalPassBloomWaveHISPS(in float2 Tex : TEXCOORD0) : COLOR
{
    float4 rgba = tex2D(s0, shock_wave(Tex));
    float4 vBloom = tex2D(s1, Tex);   
            
    rgba.rgb += vBloom * Exposure;
    
   // --------- rgb to hsi ----------
     
   float r = rgba.r;
   float g = rgba.g;
   float b = rgba.b;

   float rg = r - g;
   float rb = r - b;
   
   // Hue
   float h = acos((rg + rb) / (2 * sqrt(rg * rg + rb * (g - b)))) / PI2;
   if (b > g) h = 1 - h;

   // Intensity
   float i = (r + g + b) / 3;
   
   // Saturation
   float s = 1 - min(min(r, g), b) / i;
   
   
   // -------- hsi to rgb --------------
   
   // Scale and bias our components
   h = saturate(h * HueScale + HueBias);
   s = saturate(s * SaturationScale + SaturationBias);
   i = saturate(i * IntensityScale + IntensityBias);

   float h3 = 3 * h;
   float x = (2 * floor(h3) + 1) / 6;

   // Get our rgb components
   r = (1 - s) * i;
   float H = rt3 * tan((h - x) * PI2);
   b = ((3 + 3 * H) * i - (1 + 3 * H) * r) / 2;
   g = 3 * i - b - r;

   // Put them in right order
   float3 rgb;
   if (h3 < 1){
      rgb = float3(g, b, r);
   } else if (h3 < 2){
      rgb = float3(r, g, b);
   } else {
      rgb = float3(b, r, g);
   }

   return float4(rgb, rgba.a);
}
    
technique FinalPassBloomWaveHIS
{
    pass p0
    {
        PixelShader = compile ps_3_0 FinalPassBloomWaveHISPS();
    }
}

// ------------------- motionblur -----------------------

float4 FinalPassMotionBlurPS(in float2 Tex : TEXCOORD0) : COLOR
{
    float4 color = tex2D(s0, Tex);
    //float  depth = tex2D(s1, Tex);

#if PACKTYPE == 1
	const float4 bitShifts = float4(1,1.0/256.0,1.0/(256.0*256.0),0);//float4(1.0/(256.0*256.0*256.0), 1.0/(256.0*256.0), 1.0/256.0, 1);
	float4 packDepth = tex2D(s1, Tex);
	float depth = dot(packDepth , bitShifts);
#else
	float depth = tex2D(s1, Tex);
#endif

    //float4 H = float4(Tex.x * 2 - MaxU, (MaxV - Tex.y) * 2 - MaxV, depth, 1);
    float4 H = float4(Tex.x * 2 / MaxU - 1, 1 - Tex.y * 2 / MaxV, depth, 1.f);
    float4 D = mul(H, g_MatViewPorjInv);

    D = D / D.w;

    float4 P = mul(D, g_PrvMatViewPorj);
   
    P = P / P.w;

    float2 cur;
    float2 pev;

    cur.x =  0.5 * H.x + 0.5;
    cur.y = -0.5 * H.y + 0.5;
    
    pev.x =  0.5 * P.x + 0.5;
    pev.y = -0.5 * P.y + 0.5;

    float2 V = (cur - pev) / 100;
    //200;

    Tex -= V;
  	//float valueT = 1.0 / 136.0;
	//color = 16 * valueT * color;
    for (int i = 0; i < 15; ++i, Tex -= V)
        color += tex2D(s0, Tex); //* valueT * (i + 1); 
      
    return color / 16.0;
}


technique MotionBlur
{
    pass p0
    {
        PixelShader = compile ps_2_0 FinalPassMotionBlurPS();
    }
}

// ------------------- final shadow -----------------------

float4 FinalShadowPS(in float2 Tex : TEXCOORD0) : COLOR
{ 
	float4 color = tex2D(s0 ,Tex);
	return color;
	color.a = 0;
    float Mask = 1-color.a;
    float Shadow1 = tex2D(s2 , Tex * float2(1.0 / MaxU,1.0 / MaxV)).r * Mask;
    float depth = tex2D(s1, Tex);
    float K = min(1,(1-depth) * 1000);
    float S = lerp(1,(Shadow1 * 0.5 + 0.5)+color.a*0.5,K);
    return S * color;	
	//return tex2D(s2 , Tex);
}
    

technique FinalShadow
{
    pass p0
    {
        PixelShader = compile ps_2_0 FinalShadowPS();
        ZEnable          = False;
        AlphaBlendEnable = False;
        Lighting         = False;
        CullMode         = NONE;
    }
}


// ------------------- god ray -----------------------

#define  NUM_SAMPLES 18
float    Density = 0.8;
float    Weight  = 0.58;
float    Decay   = 0.2;
float    GodRayExposure = 0.29;
float    fScal = 1.f;

float4 vSunDirScreen = float4(0.5, 0.2, 0, 0);

float4 FinalGodRayPSP1(in float2 Tex : TEXCOORD0) : COLOR
{       
    float4 color = tex2D(s0, Tex);
    //float depth = tex2D(s1, Tex);
#if PACKTYPE == 1
	const float4 bitShifts = float4(1,1.0/256.0,1.0/(256.0*256.0),0);//float4(1.0/(256.0*256.0*256.0), 1.0/(256.0*256.0), 1.0/256.0, 1);
	float4 packDepth = tex2D(s1, Tex);
	float depth = dot(packDepth , bitShifts);
#else
	float depth = tex2D(s1, Tex);
#endif
    
    float gray = (color.r + color.g + color.b) / 3;
     
    if (depth >= 1.f)
	     return color;
	  else
	     return 0;
}

float4 FinalGodRayPSP2(in float2 Tex : TEXCOORD0) : COLOR
{	    
 		float4 color = tex2D(s0, Tex);
 		
		float2 deltaTexCoord = Tex - vSunDirScreen.xy;
				
		deltaTexCoord *= 1.0f / NUM_SAMPLES * Density;
		
		if (vSunDirScreen.w < 0.f)
		   deltaTexCoord *= -1;
		   
		//deltaTexCoord = normalize(deltaTexCoord) * Density;
		
    
		float illuminationDecay = 1.0f;

		for (int i = 0; i < NUM_SAMPLES; i++)
		{
				Tex -= deltaTexCoord;
			
				float4 sample = tex2D(s0, Tex);
				
				sample *= illuminationDecay * Weight;
			
				color += sample;	
				
				illuminationDecay *= Decay;   		
		}
				  	
		color *= GodRayExposure;
		
		return color;
}
    
    
float4 FinalGodRayPSP0(in float2 Tex : TEXCOORD0) : COLOR
{    
    float4 color = tex2D(s0, Tex);
    float4 ray = tex2D(s1, Tex);

    color += ray;
    
    return color;
}
    

technique FinalGodRay
{
    pass p0
    {
        PixelShader = compile ps_3_0 FinalGodRayPSP0();
        ZEnable          = False;
        AlphaBlendEnable = False;
        Lighting         = False;
        CullMode         = NONE;
    }
		pass p1
    {
        PixelShader = compile ps_3_0 FinalGodRayPSP1();
        ZEnable          = True;
        AlphaBlendEnable = False;
        Lighting         = False;
        CullMode         = NONE;
    }
		pass p2
    {
        PixelShader = compile ps_3_0 FinalGodRayPSP2();
        ZEnable          = False;
        AlphaBlendEnable = False;
        Lighting         = False;
        CullMode         = NONE;
    }
}



// ------------------- SSAO -----------------------


float3 random[32];
float  radius = 20.f;
float  dark   = 0.1;
float  fWidth;
float  fHeight;
float2	vBiasUV;
/*
texture texNoise;

sampler NoiseSampler = sampler_state
{
    Texture = <texNoise>;
    MinFilter = Point;  
    MagFilter = Point;
    MipFilter = None;
    AddressU  = Wrap;
    AddressV  = Wrap;
};
*/
float4 FinalSSAOPS(in float2 Tex : TEXCOORD0) : COLOR
{
		float4 color = tex2D(s0, Tex);
		float4 ssao  = tex2D(s1, Tex);
		
		//return ssao;
		//float fValue = lerp(0.3,1,ssao);
		//return color - 1 + ssao;		
		return color * (0.5 * ssao + 0.5);
		//return float4(fValue,fValue,fValue,1);
		//return ssao;
		
}

float4 BlurVel(in float2 Tex : TEXCOORD0) : COLOR
{
		float color = tex2D(s0, Tex);
		
		for (int i = 0; i < 10; ++i)
		{
				float2 tex_bias = Tex;
				tex_bias.x += (i - 5) * vBiasUV.x;
				float c = tex2D(s0, tex_bias);
				
				if (c < color)
						color = color * 0.3f + c * 0.7f;
		}
		
		
		return saturate(color);
}

float4 BlurHor(in float2 Tex : TEXCOORD0) : COLOR
{
		float color = tex2D(s0, Tex);
		
		for (int i = 0; i < 10; ++i)
		{
				float2 tex_bias = Tex;
				tex_bias.y += (i - 5) * vBiasUV.y;
				float c = tex2D(s0, tex_bias);
				
				if (c < color)
						color = color * 0.3f + c * 0.7f;
		}
		
		
		return saturate(color);
}

float ComputeValue(float dz)
{
	//float dz = (p.z - ap.z);
	//if (dz > 10 || dz < -10)
	//	dz = 0;
	float fValue;
	float dk = 1.f / (1.f + pow(dz, 2));
	float db = 1.f +  pow(dz * 0.1, 2);
	
	if (dz < 0.f || dz > 5.0 * 3)
			fValue = db;
	else
			fValue = dk;
	return fValue;
}
float4 SSAOPS(in float2 Tex : TEXCOORD0, uniform int SampNum) : COLOR
{
    /*
	float z = tex2D(s1, Tex);
	float rad = 2.0f * 3.1415926f / 6;
	float totalcalc = 0;
	float4 posInView = float4(Tex.x * 2 - 1,1 - Tex.y * 2,z,1);
	float4 posReal = mul(posInView / 32.0,g_ProjInv);
	posReal /= posReal.w;
	
	float4 jitter;
	float4 jcoord = {posInView.xy, 0, 0};
	for(float i=0; i<4; i+=1.0f)
	{
    	jitter = (2 * tex3Dlod(JitterSampler, jcoord) - 1.0);
		jcoord.z += 0.03125f;
		
		float currRad = rad * i;
    	float sinv, cosv;
    	sincos(currRad, sinv, cosv);
    	float2 tc;
    	//tc.x = Tex.x + sinv * MaxU * 2.0;
    	//tc.y = Tex.y + cosv * MaxV * 2.0;
		tc.x = jitter.x * MaxU * 2.0 + Tex.x;
		tc.y = jitter.y * MaxV * 2.0 + Tex.y;
    	tc = clamp(tc, 0.0, 1.0);
    	float sampleZ = tex2D(s1, tc);
    	float4 posSampleView = float4(tc.x * 2 - 1,1 - tc.y * 2,sampleZ,1);
    	float4 posSampleReal = mul(posSampleView,g_ProjInv);
    	posSampleReal /= posSampleReal.w;
    	float ratio = (posSampleReal.b - posReal.b)/length(posSampleReal - posReal);
    	if (length(posSampleReal - posReal) > 50)
    		ratio = 0;
    	totalcalc += ratio;
	}
	
	for(i=0; i<4; i+=1.0f)
	{
    	jitter = (2 * tex3Dlod(JitterSampler, jcoord) - 1.0);
		jcoord.z += 0.03125f;
		float currRad = rad * i + 0.11f;
    	float sinv, cosv;
    	sincos(currRad, sinv, cosv);
    	float2 tc;
    	//tc.x = Tex.x + sinv * MaxU * 6.0;
    	//tc.y = Tex.y + cosv * MaxV * 6.0;
		tc.x = jitter.x * MaxU * 6.0 + Tex.x;
		tc.y = jitter.y * MaxV * 6.0 + Tex.y;
    	tc = clamp(tc, 0.0, 1.0);
    	float sampleZ = tex2D(s1, tc);
    	float4 posSampleView = float4(tc.x * 2 - 1,1 - tc.y * 2,sampleZ,1);
    	float4 posSampleReal = mul(posSampleView,g_ProjInv);
    	posSampleReal /= posSampleReal.w;
    	float ratio = (posSampleReal.b - posReal.b)/length(posSampleReal - posReal);
    	if (length(posSampleReal - posReal) > 50)
    		ratio = 0;
		totalcalc += ratio;
	}
	
	for(i=0; i<4; i+=1.0f)
	{
    	
		jitter = (2 * tex3Dlod(JitterSampler, jcoord) - 1.0);
		jcoord.z += 0.03125f;
		float currRad = rad * i + 0.27f;
    	float sinv, cosv;
    	sincos(currRad, sinv, cosv);
    	float2 tc;
    	//tc.x = Tex.x + sinv * MaxU * 10.0;
    	//tc.y = Tex.y + cosv * MaxV * 10.0;
		tc.x = jitter.x * MaxU * 10.0 + Tex.x;
		tc.y = jitter.y * MaxV * 10.0 + Tex.y;
    	tc = clamp(tc, 0.0, 1.0);
     	float sampleZ = tex2D(s1, tc);
    	float4 posSampleView = float4(tc.x * 2 - 1,1 - tc.y * 2,sampleZ,1);
    	float4 posSampleReal = mul(posSampleView,g_ProjInv);
    	posSampleReal /= posSampleReal.w;
    	float ratio = (posSampleReal.b - posReal.b)/length(posSampleReal - posReal);
    	if (length(posSampleReal - posReal) > 50)
    		ratio = 0;
	
    	totalcalc += ratio;
	} 
	
	float v = totalcalc / 12;
	v = (v + 1) / 2;
	v = lerp(0.4,1.0,v);
	float4 finalColor = float4(v, v, v, v);
	//float4 color = tex2D(s0, Tex);
	//return color * ((1 - z * 0.5) * finalColor + z * 0.5);
	//return color ;//* v;
	return finalColor;
    */
  
	//float4 color = tex2D(s0, Tex);
		//float  z = tex2D(s1, Tex);
		#if PACKTYPE == 1
			const float4 bitShifts = float4(1,1.0/256.0,1.0/(256.0*256.0),0);//float4(1.0/(256.0*256.0*256.0), 1.0/(256.0*256.0), 1.0/256.0, 1);
			float4 packDepth = tex2D(s1, Tex);
			float z = dot(packDepth , bitShifts);
		#else
			float z = tex2D(s1, Tex);
		#endif
				
		float4 p = float4(Tex.x * 2 / MaxU - 1, 1 - Tex.y * 2 / MaxV, z, 1.f);
		p = mul(p, g_ProjInv);
		p /= p.w;
		
		//if (p.z > 30000)
		//	return 1;
		float2 scaleValue = float2(fWidth / 4.0f,fHeight / 4.0f);
		scaleValue = scaleValue * float2(1.0 / MaxU,1.0 / MaxV);
		float3 rotSample = tex2D(s2, Tex.xy * scaleValue).rgb;
		rotSample = (2.0 * rotSample - 1.0);
		
		float occ = 0.f;
		int actualSampNum = 0;
		
		for (int i = 0; i < SampNum; ++i)
		{
			float3 sp = p.xyz + /*radius*/ p.z * 0.005 * reflect(random[i].xyz,rotSample.xyz);// * 0.1;
			float4 sv = mul(float4(sp, 1.f), g_Proj);
			sv /= sv.w;
			float2 st = float2(sv.x * MaxU / 2 + MaxU / 2, MaxV / 2 - sv.y * MaxV / 2);
		  
			st = saturate(st);
				
			//float sz = tex2D(s1, st);
			#if PACKTYPE == 1
						const float4 bitShifts = float4(1,1.0/256.0,1.0/(256.0*256.0),0);//float4(1.0/(256.0*256.0*256.0), 1.0/(256.0*256.0), 1.0/256.0, 1);
						float4 packDepth = tex2D(s1, st);
						float sz = dot(packDepth , bitShifts);
			#else
						float sz = tex2D(s1, st);
			#endif
			float4 ap = float4(sv.x, sv.y, sz, 1.f);
			ap = mul(ap, g_ProjInv);
			ap /= ap.w;
																	
			//float zd = max(p.z - ap.z, 0.0);
			float zd = p.z - ap.z;
			//if (zd > 50)
			//	zd = 0;
			float fRangeIsInvalid = (saturate( abs(zd) ) + saturate( zd ))/2;  
           // float Value = lerp(saturate((-vDistance)*fDepthTestSoftness), 0.5, fRangeIsInvalid);
            //occ += 1.0/(1.0+zd*zd);  
			occ += 1.0/(1.0+fRangeIsInvalid*fRangeIsInvalid);
			++actualSampNum;
			/*
			if (ap.z > 0)
			{
					
				float dz = (p.z - ap.z);
				//if (dz > 10 || dz < -10)
				//	dz = 0;
				
				float dk = 1.f / (1.f + pow(dz * dark, 2));
				float db = 1.f +  pow(dz * 0.1, 2);
				
				if (dz < 0.f || dz > radius * 3)
						occ += db;
				else
						occ += dk;
									
				++actualSampNum;
			}
			*/
		}
		

		float fact = occ / float(actualSampNum);
		return fact;	
		// return color - 1.f + saturate(fact);
		
		/*
		if (fact > 0.5)
				fact = 0.5;
		if (fact < 0)
				fact = 0;
		if (1)
				return fact;
		return saturate(color - float4(fact, fact, fact, 0));
	  */
		
		/*
		float fact = occ / float(actualSampNum);
		if (fact < 0.5)
				fact = 0.5;
		if (fact > 1)
				fact = 1;
		return color - 1 + fact;
		*/
		
		/*
		float fact = saturate(occ / float(actualSampNum));
		fact = 1 - pow(fact, 2);
				
	  return color - fact;
	  */
}

technique FinalSSAO
{
    pass p0
    {
        VertexShader = NULL;
        PixelShader  = compile ps_2_0 FinalSSAOPS();
    }
    
    pass p1
    {
        VertexShader = NULL;
        PixelShader  = compile ps_3_0 SSAOPS(8);
    }
    
		pass p2
    {
        VertexShader = NULL;
        PixelShader  = compile ps_3_0 SSAOPS(16);
    }
    
		pass p3
    {
        VertexShader = NULL;
        PixelShader  = compile ps_3_0 SSAOPS(32);
    }
    
	  pass p4
    {
        VertexShader = NULL;
        PixelShader  = compile ps_2_0 BlurVel();
    }
    
 	  pass p5
    {
        VertexShader = NULL;
        PixelShader  = compile ps_2_0 BlurHor();
    }   
}

// ------------------- fog -----------------------

float4 cameraWorldPos;
float fogDensityV = 0.001;
float SlopeThreshold = 0.1;
float heightFalloff = 0.004;
float4 fogcolors = float4(0.7,0.7,0.7,1);
float4 FogPS(in float2 Tex: TEXCOORD0):COLOR
{	
	//float z = tex2D(s1, Tex);
#if PACKTYPE == 1
	const float4 bitShifts = float4(1,1.0/256.0,1.0/(256.0*256.0),0);//float4(1.0/(256.0*256.0*256.0), 1.0/(256.0*256.0), 1.0/256.0, 1);
	float4 packDepth = tex2D(s1, Tex);
	float z = dot(packDepth , bitShifts);
#else
	float z = tex2D(s1, Tex);
#endif

	float4 posInView = float4(Tex.x * 2 - 1,1 - Tex.y * 2,z,1);
	float4 posReal = mul(posInView,g_MatViewPorjInv);
	posReal /= posReal.w;
	posReal.w = 1;
	//posReal = mul(posReal,g_ViewInv);
	float fogDistance = length(posReal.xyz - cameraWorldPos.xyz);
	float VolFogHeightDensityAtViewer = exp(-heightFalloff * cameraWorldPos.y);
	float fogInt = fogDistance * VolFogHeightDensityAtViewer;
	
	if (abs(posReal.z) > SlopeThreshold)	
	{		
		float t = heightFalloff * (posReal.y - cameraWorldPos.y);		
		fogInt *= (1.0 - exp(-t)) / t;	
	}			
	float fogFactor = exp(-fogDensityV * fogInt);
	//float fogFactor = exp2(-abs(fogDistance * fogDensityV));
	
	//return float4(fogFactor,fogFactor,fogFactor,fogFactor);
	//float4 sum = tex2D(s0, Tex);
	return lerp(fogcolors,tex2D(s0,Tex),fogFactor);
	//return float4(z,z,z,z);
}


technique FinalFog
{
    pass p0
    {
        VertexShader = NULL;
        PixelShader  = compile ps_2_0 FogPS();
    } 
}

// ------------------- aoe -----------------------
float4 FinalAOE_PS(in float2 Tex : TEXCOORD0) : COLOR
{    
    float4 color = tex2D(s0, Tex);
    float4 aoe = tex2D(s1, Tex);
    
    color = color * (1.f - aoe.a) + aoe * aoe.a;
    return color;
}
    
technique FinalAOE
{
    pass p0
    {
        PixelShader = compile ps_2_0 FinalAOE_PS();
        ZEnable          = False;
        AlphaBlendEnable = False;
        Lighting         = False;
        CullMode         = NONE;
        MinFilter[0]     = LINEAR;
        MagFilter[0]     = LINEAR;
        MinFilter[1]     = LINEAR;
        MagFilter[1]     = LINEAR;
        MipFilter[0]     = LINEAR;
        MipFilter[1]     = LINEAR;
    }
}

//----------------------hdr--------------------------
float  g_fMiddleGray;       // The middle gray key value
float  g_fBloomScale;       // Bloom process multiplier
float  g_fStarScale;        // Star process multiplier
bool g_bEnableBlueShift = false;

// The per-color weighting to be used for luminance calculations in RGB order.
static const float3 LUMINANCE_VECTOR  = float3(0.2125f, 0.7154f, 0.0721f);

// The per-color weighting to be used for blue shift under low light.
static const float3 BLUE_SHIFT_VECTOR = float3(1.05f, 0.97f, 1.27f); 

float4 FinalHDR(in float2 vScreenPosition : TEXCOORD0) : COLOR
{
    float4 vSample = tex2D(s0, vScreenPosition);
    float4 vBloom = tex2D(s1, vScreenPosition * float2(1.0 / MaxU,1.0 / MaxV));
    float4 vStar = tex2D(s2, vScreenPosition * float2(1.0 / MaxU,1.0 / MaxV));
	float fAdaptedLum = tex2D(s3, float2(0.5f, 0.5f));
	//fAdaptedLum *= 0.8;
	
	// For very low light conditions, the rods will dominate the perception
    // of light, and therefore color will be desaturated and shifted
    // towards blue.
  
    if( g_bEnableBlueShift )
	
    {
		// Define a linear blending from -1.5 to 2.6 (log scale) which
		// determines the lerp amount for blue shift
        float fBlueShiftCoefficient = 1.0f - (fAdaptedLum + 1.5)/4.1;
        fBlueShiftCoefficient = saturate(fBlueShiftCoefficient);

		// Lerp between current color and blue, desaturated copy
        float3 vRodColor = dot( (float3)vSample, LUMINANCE_VECTOR ) * BLUE_SHIFT_VECTOR;
        vSample.rgb = lerp( (float3)vSample, vRodColor, fBlueShiftCoefficient );
    }
    
	
    // Map the high range of color values into a range appropriate for
    // display, taking into account the user's adaptation level, and selected
    // values for for middle gray and white cutoff.
    //if( g_bEnableToneMap )
    {
		//vSample.rgb *= g_fMiddleGray/(fAdaptedLum + 0.001f);
		//vSample.rgb /= (1.0f+vSample);
		/*
		float fLum = dot(vSample, LUMINANCE_VECTOR) + 0.0001f;
		float fKey = max(0,1.5 - 1.5 / (fAdaptedLum * 0.1 +1)) + 0.8;
		vSample.rgb *= fKey * fLum/(fAdaptedLum + 0.0001f);
		//vSample.rgb = vSample * (1 + vSample / 2.5 * 2.5) / (1.0f+vSample) ;
		vSample.rgb = vSample * (1 + vSample / 1.1 * 1.1) / (1.0f+vSample) ;
		*/
	
		float fLum = dot(vSample, LUMINANCE_VECTOR) + 0.0001f;
		float fKey = max(0,1.5 - 1.5 / (fAdaptedLum * 0.1 +1)) + 0.8;
		float fScaled = fKey * fLum / fAdaptedLum;
		float fColor = (fScaled * (1 + fScaled / (1.5 * 1.5))) / (1 + fScaled);
		vSample.rgb *= fColor;
	
    }  
    
    // Add the star and bloom post processing effects

    vSample += g_fStarScale * vStar;
    vSample += g_fBloomScale * vBloom;    
    return vSample;//vSample;
}

technique FinalSceneHDR
{
    pass P0
    {
        PixelShader  = compile ps_2_0 FinalHDR();
    }
}

// --------------------- DOF -------------------------- 
float4 pixelSizes;
float4 dofParamsBlur = float4(5,10,0,0);
float dofMinThreshold = 0.0f;
float radiusScale = 0.4;


float4 FinalDOF(in float2 vScreenPosition : TEXCOORD0) : COLOR
{
  float4 OUT;
  const int tapCount = 8;

  float2 poisson[8] =
  {
       0.0,    0.0,
     0.527, -0.085,
    -0.040,  0.536,
    -0.670, -0.179,
    -0.419, -0.616,
     0.440, -0.639,
    -0.757,  0.349,
     0.574,  0.685,
  };
  
  float4 cOut=0;
  float discRadius;
  float discRadiusLow;
  float centerDepth;
  float4 mypixelSizes;
        
  // fetch center tap from blured low res image
  centerDepth=tex2D(s1, vScreenPosition * float2(1.0 / MaxU,1.0 / MaxV)).r;    
  discRadius=(centerDepth*(float)dofParamsBlur.y-(float)dofParamsBlur.x); 
  discRadiusLow = discRadius*radiusScale;

  mypixelSizes.xy=pixelSizes.xy*discRadius;
  mypixelSizes.zw=pixelSizes.zw*discRadiusLow;
  for(int t=0; t<tapCount; t++)
  { 
    float2 depthTex = vScreenPosition * float2(1.0 / MaxU,1.0 / MaxV);
	depthTex = depthTex + poisson[t] * mypixelSizes.xy;
	float4 tapHigh;
	tapHigh.w = tex2D(s1,depthTex).r;        
	tapHigh.xyz= tex2D(s0, vScreenPosition 
	                + (poisson[t] * mypixelSizes.xy) * float2(MaxU,MaxV)).xyz;
	float4 tapLow;
	tapLow = tex2D(s2,vScreenPosition * float2(1.0 / MaxU,1.0 / MaxV) + poisson[t] * mypixelSizes.zw);
    float tapBlur = abs(tapHigh.w * 2.0 - 1.0);      
    float4 tap = lerp(tapHigh,tapLow,tapBlur);    
    // Apply leak reduction. Make sure only to reduce on focused areas            
    tap.w=(tapHigh.w-centerDepth+dofMinThreshold>0.0)? 1: abs(tap.w*2.0-1.0);    
	//tap.w = 1.0;
    cOut.xyz += (tap.w * tap.xyz);
    cOut.w += tap.w;
  }
                            
  OUT = cOut/cOut.w;
  //OUT = cOut;
  //OUT.a = tex2D(s0, vScreenPosition).a;
  //OUT = tex2D(s0, vScreenPosition);
  return OUT;
}

technique FinalSceneDOF
{
    pass P0
    {
        PixelShader  = compile ps_3_0 FinalDOF();
    }
}
