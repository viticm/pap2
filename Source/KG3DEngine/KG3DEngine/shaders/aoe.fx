////////////////////////////////////////////////////////////////////////////////
//
//  Author      : ZhaoXinyu (Chiyer)
//  CopyRight   : Kingsoft Corporation All rights Reserved
//  Create Date : 
//  Comment     : aoe shader
//
////////////////////////////////////////////////////////////////////////////////

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

#define MaxTexNum 4


#ifndef PACKTYPE
#define PACKTYPE 0
#endif

float MaxU;
float MaxV;

float4 aoe_info[MaxTexNum];
float4 aoe_diff[MaxTexNum];
float  aoe_limt[MaxTexNum];
float4 tex_info[MaxTexNum];
    
float4 aoe_ps1(in float2 Tex : TEXCOORD0) : COLOR
{    
    //float z = tex2D(s0, Tex);
#if PACKTYPE == 1
	const float4 bitShifts = float4(1,1.0/256.0,1.0/(256.0*256.0),0);//float4(1.0/(256.0*256.0*256.0), 1.0/(256.0*256.0), 1.0/256.0, 1);
	float4 packDepth = tex2D(s0, Tex);
	float z = dot(packDepth , bitShifts);
#else
	float z = tex2D(s0, Tex);
#endif
    
    float4 p = float4(Tex.x * 2 / MaxU - 1, 1 - Tex.y * 2 / MaxV, z, 1.f);
    p = mul(p, g_MatViewPorjInv);
    p /= p.w;
	
	float4 color = float4(0, 0, 0, 0);
	float2 aoeTex;
	
	if (abs(p.y - aoe_info[0].y) <= aoe_limt[0]) 
    {
        aoeTex = float2(p.x - aoe_info[0].x + aoe_info[0].w, aoe_info[0].z - p.z + aoe_info[0].w);
        aoeTex /= aoe_info[0].w * 2;
    		
        if (aoeTex.x >= 0 && aoeTex.y >= 0 && aoeTex.x < 1 && aoeTex.y < 1)
        {
            aoeTex *= tex_info[0].xy;
            aoeTex += tex_info[0].zw;
            float4 aoeColor = tex2D(s1, aoeTex) * aoe_diff[0];
            color = aoeColor;
        }
    }
    
    return color;
}
    
float4 aoe_ps2(in float2 Tex : TEXCOORD0) : COLOR
{    
    //float z = tex2D(s0, Tex);
#if PACKTYPE == 1
	const float4 bitShifts = float4(1,1.0/256.0,1.0/(256.0*256.0),0);//float4(1.0/(256.0*256.0*256.0), 1.0/(256.0*256.0), 1.0/256.0, 1);
	float4 packDepth = tex2D(s0, Tex);
	float z = dot(packDepth , bitShifts);
#else
	float z = tex2D(s0, Tex);
#endif
    
    float4 p = float4(Tex.x * 2 / MaxU - 1, 1 - Tex.y * 2 / MaxV, z, 1.f);
    p = mul(p, g_MatViewPorjInv);
    p /= p.w;
		
	float4 color = float4(0, 0, 0, 0);
	float2 aoeTex;
	float4 aoeColor;
	
	if (abs(p.y - aoe_info[0].y) <= aoe_limt[0]) 
	{
        aoeTex = float2(p.x - aoe_info[0].x + aoe_info[0].w, aoe_info[0].z - p.z + aoe_info[0].w);
        aoeTex /= aoe_info[0].w * 2;
		
        if (aoeTex.x >= 0 && aoeTex.y >= 0 && aoeTex.x < 1 && aoeTex.y < 1)
        {
            aoeTex *= tex_info[0].xy;
            aoeTex += tex_info[0].zw;
            aoeColor = tex2D(s1, aoeTex) * aoe_diff[0];
            color = aoeColor;
        }
    }
    
    if (abs(p.y - aoe_info[1].y) <= aoe_limt[1]) 
    {
        aoeTex = float2(p.x - aoe_info[1].x + aoe_info[1].w, aoe_info[1].z  - p.z + aoe_info[1].w);
        aoeTex /= aoe_info[1].w * 2;
		
        if (aoeTex.x >= 0 && aoeTex.y >= 0 && aoeTex.x < 1 && aoeTex.y < 1)
        {
            aoeTex *= tex_info[1].xy;
            aoeTex += tex_info[1].zw;            
            aoeColor = tex2D(s2, aoeTex) * aoe_diff[1];
            color = lerp(color, aoeColor, aoeColor.a);
        }
    }
    
    return color;
}
    
float4 aoe_ps3(in float2 Tex : TEXCOORD0) : COLOR
{    
    //float z = tex2D(s0, Tex);
#if PACKTYPE == 1
	const float4 bitShifts = float4(1,1.0/256.0,1.0/(256.0*256.0),0);//float4(1.0/(256.0*256.0*256.0), 1.0/(256.0*256.0), 1.0/256.0, 1);
	float4 packDepth = tex2D(s0, Tex);
	float z = dot(packDepth , bitShifts);
#else
	float z = tex2D(s0, Tex);
#endif
    
    float4 p = float4(Tex.x * 2 / MaxU - 1, 1 - Tex.y * 2 / MaxV, z, 1.f);
    p = mul(p, g_MatViewPorjInv);
    p /= p.w;
		
	float4 color = float4(0, 0, 0, 0);
    float2 aoeTex;
	float4 aoeColor;
	
	
	if (abs(p.y - aoe_info[0].y) <= aoe_limt[0]) 
	{
        aoeTex = float2(p.x - aoe_info[0].x + aoe_info[0].w, aoe_info[0].z  - p.z + aoe_info[0].w);
        aoeTex /= aoe_info[0].w * 2;
		
        if (aoeTex.x >= 0 && aoeTex.y >= 0 && aoeTex.x < 1 && aoeTex.y < 1)
        {
            aoeTex *= tex_info[0].xy;
            aoeTex += tex_info[0].zw;            
            aoeColor = tex2D(s1, aoeTex) * aoe_diff[0];
            color = aoeColor;
        }
    }
    
    if (abs(p.y - aoe_info[1].y) <= aoe_limt[1]) 
    {
        aoeTex = float2(p.x - aoe_info[1].x + aoe_info[1].w, aoe_info[1].z  - p.z + aoe_info[1].w);
        aoeTex /= aoe_info[1].w * 2;
		
        if (aoeTex.x >= 0 && aoeTex.y >= 0 && aoeTex.x < 1 && aoeTex.y < 1)
        {
            aoeTex *= tex_info[1].xy;
            aoeTex += tex_info[1].zw;            
            aoeColor = tex2D(s2, aoeTex) * aoe_diff[1];
            color = lerp(color, aoeColor, aoeColor.a);
        }
    }
    
    if (abs(p.y - aoe_info[2].y) <= aoe_limt[2]) 
    {
        aoeTex = float2(p.x - aoe_info[2].x + aoe_info[2].w, aoe_info[2].z  - p.z + aoe_info[2].w);
        aoeTex /= aoe_info[2].w * 2;
		
        if (aoeTex.x >= 0 && aoeTex.y >= 0 && aoeTex.x < 1 && aoeTex.y < 1)
        {
            aoeTex *= tex_info[2].xy;
            aoeTex += tex_info[2].zw;            
            aoeColor = tex2D(s3, aoeTex) * aoe_diff[2];
            color = lerp(color, aoeColor, aoeColor.a);
        }
    }
    
    return color;
}


float4 aoe_ps4(in float2 Tex : TEXCOORD0) : COLOR
{    
    //float z = tex2D(s0, Tex);
#if PACKTYPE == 1
	const float4 bitShifts = float4(1,1.0/256.0,1.0/(256.0*256.0),0);//float4(1.0/(256.0*256.0*256.0), 1.0/(256.0*256.0), 1.0/256.0, 1);
	float4 packDepth = tex2D(s0, Tex);
	float z = dot(packDepth , bitShifts);
#else
	float z = tex2D(s0, Tex);
#endif
    
    float4 p = float4(Tex.x * 2 / MaxU - 1, 1 - Tex.y * 2 / MaxV, z, 1.f);
    p = mul(p, g_MatViewPorjInv);
    p /= p.w;
		
	float4 color = float4(0, 0, 0, 0);
    float2 aoeTex;
	float4 aoeColor;
	
	
	if (abs(p.y - aoe_info[0].y) <= aoe_limt[0]) 
	{
        aoeTex = float2(p.x - aoe_info[0].x + aoe_info[0].w, aoe_info[0].z  - p.z + aoe_info[0].w);
        aoeTex /= aoe_info[0].w * 2;
		
        if (aoeTex.x >= 0 && aoeTex.y >= 0 && aoeTex.x < 1 && aoeTex.y < 1)
        {
            aoeTex *= tex_info[0].xy;
            aoeTex += tex_info[0].zw;            
            aoeColor = tex2D(s1, aoeTex) * aoe_diff[0];
            color = aoeColor;
        }
    }
    
    if (abs(p.y - aoe_info[1].y) <= aoe_limt[1]) 
    {
        aoeTex = float2(p.x - aoe_info[1].x + aoe_info[1].w, aoe_info[1].z  - p.z + aoe_info[1].w);
        aoeTex /= aoe_info[1].w * 2;
		
        if (aoeTex.x >= 0 && aoeTex.y >= 0 && aoeTex.x < 1 && aoeTex.y < 1)
        {
            aoeTex *= tex_info[1].xy;
            aoeTex += tex_info[1].zw;            
            aoeColor = tex2D(s2, aoeTex) * aoe_diff[1];
            color = lerp(color, aoeColor, aoeColor.a);
        }
    }
    
    if (abs(p.y - aoe_info[2].y) <= aoe_limt[2]) 
    {
        aoeTex = float2(p.x - aoe_info[2].x + aoe_info[2].w, aoe_info[2].z  - p.z + aoe_info[2].w);
        aoeTex /= aoe_info[2].w * 2;
		
        if (aoeTex.x >= 0 && aoeTex.y >= 0 && aoeTex.x < 1 && aoeTex.y < 1)
        {
            aoeTex *= tex_info[2].xy;
            aoeTex += tex_info[2].zw;            
            aoeColor = tex2D(s3, aoeTex) * aoe_diff[2];
            color = lerp(color, aoeColor, aoeColor.a);
        }
    }
    
    if (abs(p.y - aoe_info[3].y) <= aoe_limt[3]) 
    {
        aoeTex = float2(p.x - aoe_info[3].x + aoe_info[3].w, aoe_info[3].z  - p.z + aoe_info[3].w);
        aoeTex /= aoe_info[3].w * 2;
		
        if (aoeTex.x >= 0 && aoeTex.y >= 0 && aoeTex.x < 1 && aoeTex.y < 1)
        {
            aoeTex *= tex_info[3].xy;
            aoeTex += tex_info[3].zw;            
            aoeColor = tex2D(s4, aoeTex) * aoe_diff[3];
            color = lerp(color, aoeColor, aoeColor.a);
        }
    }
    
    return color;
}

technique aoe
{
    pass p0
    {
        PixelShader = compile ps_3_0 aoe_ps1();
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
    
    pass p1
    {
        PixelShader = compile ps_3_0 aoe_ps2();
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
    
    pass p2
    {
        PixelShader = compile ps_3_0 aoe_ps3();
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
    
    pass p3
    {
        PixelShader = compile ps_3_0 aoe_ps4();
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