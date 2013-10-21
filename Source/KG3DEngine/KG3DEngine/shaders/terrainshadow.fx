
struct VS_TERRAINZ_INPUT
{
    float3 pos : POSITION;
  //  float3 color : COLOR;
};

struct VS_TERRAINZ_OUTPUT
{    
    float4 pos : POSITION;
    float2 depth : TEXCOORD0;    
};

struct PS_TERRAINZ_OUTPUT
{
    float4 color : COLOR;
};

struct VS_SHADOW_INPUT
{
    float3 pos : POSITION;
 //   float3 normal : NORMAL;
  //  float2 tex1 : TEXCOORD0;
};

struct VS_SHADOW_OUTPUT
{
    float4 pos : POSITION;
    float4 realpos : TEXCOORD0;
 //   float3 normal : TEXCOORD3;
};

struct PS_SHADOW_OUTPUT
{
    float4 color : COLOR;
};

float4x4 sunworldViewProj,projectwvpmat;
float3 sundirc = float3(0.0,0.0,-1.0);
float size;
float screenw = 128.0,screenh = 128.0;
float ambient = 0.7;
float Specular = 0.3;

texture ShadowMap;
sampler2D g_samShadow =
sampler_state
{
    Texture = <ShadowMap>;
    MinFilter = Point;
    MagFilter = Point;
    MipFilter = Point;
    AddressU = Clamp;
    AddressV = Clamp;
};


VS_TERRAINZ_OUTPUT VSDepth(VS_TERRAINZ_INPUT In1)
{
    VS_TERRAINZ_OUTPUT out1;
    
    out1.pos = float4(mul(float4(In1.pos,1.0),sunworldViewProj));
    out1.depth = float2(out1.pos.zw);//out1.pos.z / out1.pos.w;//float4(mul(float4(In1.pos,1.0),ligworldView)).z;///**/
    
    return out1;
}

PS_TERRAINZ_OUTPUT PSDepth(VS_TERRAINZ_OUTPUT psin)
{
    PS_TERRAINZ_OUTPUT psout;
    
    
//    f /= 2.0;
    psout.color = psin.depth.x / psin.depth.y;//float4(f,f,f,1.0);
    
    return psout;
}

///////////////////////////////////////////////////////////////////////////////////////


VS_SHADOW_OUTPUT VSShadow(VS_SHADOW_INPUT In1)
{
    VS_SHADOW_OUTPUT out1;
    
    out1.pos = mul(float4(In1.pos,1.0),projectwvpmat);
    out1.realpos = mul(float4(In1.pos,1.0),sunworldViewProj);
  
    
    //    float4 propos= mul(float4(In1.pos,1.0),projectwvpmat);
    
//    float2 uv = float2(propos.x,propos.y);
//    out1.pos = float4(uv.x * screenw,uv.y * screenh,0.0,1.0);
//    out1.normal = In1.normal;
    
    return out1;
}

PS_SHADOW_OUTPUT PSShadow(VS_SHADOW_OUTPUT psin)
{
    PS_SHADOW_OUTPUT psout;
    
    float s;
    float2 tc = float2(psin.realpos.x  * 0.5 + 0.5,
                    psin.realpos.y  * 0.5 + 0.5);
        tc.y = 1.0 - tc.y;
        
        float v,zmapv;
        float4 v4;
		  
	zmapv = tex2D(g_samShadow,tc);
	v = zmapv > (psin.realpos.z ) ? 1.0 : ambient;
   
       
    psout.color = float4(v,v,v,1.0);
//    psout.color = float4(1.0,1.0,0.0,1.0);
    
    return psout;
}

/////////////////////////////////////////////////////////////////////

VS_TERRAINZ_OUTPUT VS_PLANESHADOW(VS_TERRAINZ_INPUT input)
{
    VS_TERRAINZ_OUTPUT out1;
    
    out1.pos = float4(mul(float4(input.pos,1.0),sunworldViewProj));
    out1.depth = float2(out1.pos.zw);
    
    return out1;
}

PS_SHADOW_OUTPUT PS_PLANESHADOW(VS_TERRAINZ_OUTPUT psinput)
{
    PS_SHADOW_OUTPUT psout;
    
    psout.color = float4(0.7,0.7,0.7,1.0);
    
    return psout;
}

/////////////////////////////////////////////////////////////////////

technique effectdepth
{
    pass Pass0
    {
        Lighting = FALSE;
    
        VertexShader = compile vs_1_1 VSDepth();
        PixelShader = compile ps_2_0 PSDepth();
    }
}

technique effectshadow
{
    pass Pass0
    {
        Lighting = FALSE;
    
        VertexShader = compile vs_2_0 VSShadow();
        PixelShader = compile ps_2_0 PSShadow();
    }
}

technique effectplaneshadow
{
    pass Pass0
    {
        Lighting = FALSE;
    
        VertexShader = compile vs_1_1 VS_PLANESHADOW();
        PixelShader = compile ps_2_0 PS_PLANESHADOW();
    }
}