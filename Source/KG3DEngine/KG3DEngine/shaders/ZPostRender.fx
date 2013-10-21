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
texture tMainDepth;
texture tMainColor;

texture tShadowmap1;
texture tShadowmap2;
texture tShadowmap3;

texture Jitter;

matrix matCameraVPInv;
matrix matLightVP1;
matrix matLightVP2;
matrix matLightVP3;

//float fbias = 0.0008;
//int bAreaShadow = 0;
float OffsetU = 1.0;
float OffsetV = 1.0;
float ScalU = 1.0;
float ScalV = 1.0;
float4  FilterSize, JitterScale; 

#ifndef PACKTYPE
#define PACKTYPE 0
#endif

sampler JitterSampler = sampler_state
{
    Texture = <Jitter>;
    MinFilter = Point;  
    MagFilter = Point;
    MipFilter = None;
    AddressU  = Wrap;
    AddressV  = Wrap;
};

sampler2D SamplerDepth = sampler_state
{
    Texture   = <tMainDepth>;
    MipFilter = NONE;
    MinFilter = Point;
    MagFilter = Point;
    ADDRESSU = Clamp;
    ADDRESSV = Clamp;
};

sampler2D SamplerColor= sampler_state
{
    Texture   = <tMainColor>;
    MipFilter = NONE;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    ADDRESSU = Clamp;
    ADDRESSV = Clamp;
};

sampler2D SamplerShadow1 = sampler_state
{
    Texture   = <tShadowmap1>;
    MipFilter = NONE;
    MinFilter = Point;
    MagFilter = Point;
    ADDRESSU = Clamp;
    ADDRESSV = Clamp;
    BORDERCOLOR = 0xFFFFFFFF;
};

sampler2D SamplerShadow2 = sampler_state
{
    Texture   = <tShadowmap2>;
    MipFilter = NONE;
    MinFilter = Point;
    MagFilter = Point;
    ADDRESSU = Clamp;
    ADDRESSV = Clamp;
    BORDERCOLOR = 0xFFFFFFFF;
};

sampler2D SamplerShadow3 = sampler_state
{
    Texture   = <tShadowmap3>;
    MipFilter = NONE;
    MinFilter = Point;
    MagFilter = Point;
    ADDRESSU = Clamp;
    ADDRESSV = Clamp;
    BORDERCOLOR = 0xFFFFFFFF;
};

#define M_PI 3.14159265358979323846
#define fmodp(x,n) ((n)*frac((x)/(n)))

/*
float4 UvProcess(float4 In) //convert -1~1 to 0~1
{
    In.xyz/=In.w;
    
    In.x =  0.5 * In.x + 0.5;
    In.y = -0.5 * In.y + 0.5;
    
    return In;
}

float AreaShadowCompute(float4 pos,sampler2D ShadowMap,float k)//, matrix matLightVPInv)
{
    float S = 0;     
    float Shadowmap;
    float dis0 = pos.z - tex2D(ShadowMap, pos.xy).a  - fbias*k;
    float dis;// = dis0 ;
    
    if (dis0 > 0) //in shadow
        dis = lerp(0, 8, min(1, dis0 * 18));
    else //no in shadow
    {
        dis = 4;//lerp(0, 4, min(1, (-dis0) * 4));
    }
    //dis = 6;
    for(int i=0;i<9;i++)
    {
        Shadowmap = tex2D(ShadowMap, pos.xy + (g_avSampleOffsets[i].xy * dis) ).a;
        if(Shadowmap < pos.z - fbias*k)
            S += 1;
    }
    S /= 9; //1 for shadow
    return (1-S); //0 for shadow
}
*/
float LowShadowCompute(float4 pos,sampler2D ShadowMap,bool bSampleMore,float2 ScreenPos,uniform int loopIterations)
{
 	float fsize = pos.w * FilterSize.w;
	float4 smcoord = {0, 0, pos.zw};
	float4 jcoord = {ScreenPos * JitterScale, 0, 0};
	float shadow = 0;
	float4 jitter;

	for(int i = 0; i < 4; i++) 
	{
		jitter = (2 * tex3Dlod(JitterSampler, jcoord) - 1.0);
		jcoord.z += 0.03125f;
		smcoord.xy = jitter.xy * fsize + pos.xy;
		shadow += tex2Dlod(ShadowMap, float4(smcoord.xyz / smcoord.w, 0));
		smcoord.xy = jitter.zw * fsize + pos.xy;
		shadow += tex2Dlod(ShadowMap, float4(smcoord.xyz / smcoord.w, 0));
	}	
	
	if( (shadow > 0) && (shadow < 8) && (bSampleMore)) 
	{		
		for(int i = 0; i < loopIterations; i++) 
		{
			jitter = (2 * tex3Dlod(JitterSampler, jcoord) - 1.0);
			jcoord.z += 0.03125f;
			smcoord.xy = jitter.xy * fsize + pos.xy;
			shadow += tex2Dlod(ShadowMap, float4(smcoord.xyz / smcoord.w, 0));
			smcoord.xy = jitter.zw * fsize + pos.xy;
			shadow += tex2Dlod(ShadowMap, float4(smcoord.xyz / smcoord.w , 0));
		}	

  	shadow /= (2 * (loopIterations + 4));
	} 
	else 
	{
	  shadow /= 8;
	}

	//shadow = tex2Dlod(ShadowMap, float4(pos.xyz / smcoord.w, 0));
	return shadow;
   /*
    float S = 0;     
    float Shadowmap;
    float dis = 2;
    //float4 posSample = {0,0,pos.z - fbias*k,1};
   	float4 posSample = {0,0,pos.z,1};
    for(int i=0;i<9;i++)
    {
        posSample.xy = pos.xy + (g_avSampleOffsets[i].xy * dis); 
        Shadowmap = tex2Dproj(ShadowMap, posSample);
        S += Shadowmap;
    }
    S /= 9; 
    return S;
    */
} 


float4 main(in float2 vScreenPosition : TEXCOORD0,float2 vPos : VPOS,uniform int loopIterations) : COLOR
{
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
    float4 pos_w = mul(pos_trans,matCameraVPInv);//convert to world cord
    float4 pos_l1 = mul(pos_w,matLightVP1);         //contert to light cord
    float4 pos_l2 = mul(pos_w,matLightVP2);
    float4 pos_l3 = mul(pos_w,matLightVP3);

    //pos_l1 = UvProcess(pos_l1);
    //pos_l2 = UvProcess(pos_l2);
    //pos_l3 = UvProcess(pos_l3);  
   
    float s_1 = 1;
    float s_2 = 1;
    float s_3 = 1;
    //
    float4 pos_l1_line = pos_l1 / pos_l1.w;
    float4 pos_l2_line = pos_l2 / pos_l2.w;
    float4 pos_l3_line = pos_l3 / pos_l3.w;
  
    if(pos_l1_line.x >= 0 && pos_l1_line.y >= 0 && pos_l1_line.x <= 1 && pos_l1_line.y <= 1)
    {	
       	s_1 = LowShadowCompute(pos_l1,SamplerShadow1,true,vPos,loopIterations);
    } 
	
    else if(pos_l2_line.x >= 0 && pos_l2_line.y >= 0 && pos_l2_line.x <= 1 && pos_l2_line.y <= 1)
    {
        s_2 = LowShadowCompute(pos_l2,SamplerShadow2,true,vPos,loopIterations);
    }
	
    else if(pos_l3_line.x >= 0 && pos_l3_line.y >= 0 && pos_l3_line.x <= 1 && pos_l3_line.y <= 1)
    {
        s_3 = LowShadowCompute(pos_l3,SamplerShadow3,false,vPos,loopIterations);
    }
	
    //s_1 = LowShadowCompute(pos_l1,SamplerShadow1,true,vPos,loopIterations);
    //s_2 = LowShadowCompute(pos_l2,SamplerShadow2,false,vPos,loopIterations);
    //s_3 = LowShadowCompute(pos_l3,SamplerShadow3,false,vPos,loopIterations);
    float s = s_1 * s_2 * s_3;
    return float4(s,s,s,1);
    //return float4(depth,depth,depth,1);
	//return float4(0,0,0,0);
	//return float4(1,1,1,1);
}
texture tShadowMask;
sampler2D SamplerShadowMask = sampler_state
{
    Texture   = <tShadowMask>;
    MipFilter = NONE;
    MinFilter = Linear;
    MagFilter = Linear;
    ADDRESSU = Clamp;
    ADDRESSV = Clamp;
    BORDERCOLOR = 0xFFFFFFFF;
};

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
      vAccum += tex2D( SamplerShadowMask, Tex + g_vSampleOffsets[i] ) * g_fSampleWeights[i];
   }
   return vAccum;
   
   /*
   float minColor = 1.0f;
   for (int i = 0; i < 9; i++)
   {
	   float4 TempColor = tex2D( SamplerShadowMask, Tex + g_vSampleOffsets[i]);
	   minColor = min(minColor, TempColor.r);
   }
   */
   /*
   float4 TempColor = tex2D(SamplerShadowMask, Tex);
   return TempColor;
   */
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
      vAccum += tex2D( SamplerShadowMask, Tex + g_vSampleOffsets[i] ) * g_fSampleWeights[i];
   }
   return vAccum;
   
   /*
   float minColor = 1.0f;
   for (int i = 0; i < 9; i++)
   {
	   float4 TempColor = tex2D( SamplerShadowMask, Tex + g_vSampleOffsets[i]);
	   minColor = min(minColor, TempColor.r);
   }
   return float4(minColor,minColor,minColor,1.0);   
   */
}


technique tec0
{
    pass P0
    {
        //VertexShader = NONE;
        PixelShader  = compile ps_3_0 main(10);
        ZEnable          = False;
        AlphaBlendEnable = True;
        AlphaTestEnable = False;
        SrcBlend        = SrcAlpha;          
        DestBlend       = InvSrcAlpha;
        Lighting        = False;
        CullMode        = NONE;
     }
	 pass p1
	 {
		PixelShader  = compile ps_3_0 PS_BlurH(); 
	 }
	 pass p2
	 {
		PixelShader  = compile ps_3_0 PS_BlurV(); 
	 }
}
