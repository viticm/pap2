//-----------------------------------------------------------------------------------
// File: ocean_surface.fx
//
// Desc: The effect file for the ocean surface rendering.
// 
// Revision history:
//   * 2005-1-12,  Created by Lin Nan
//-----------------------------------------------------------------------------------


//------------------------------------------------------------------------------------
// Global variables
//------------------------------------------------------------------------------------
//float        g_DeltaTime;

// XForm Matrix
float4x4    g_matWorldViewProj;
float4x4    g_matWorld;
float4x4    g_matViewProj;
float4x4    g_matTexCoord;

// Constant
float        g_GridLength;
float        g_GridLength_x2;
float3        g_LocalEye;
float        g_ChoppyScale_GridLen; // choppy_scale / (grid_len * 2)
int            g_AddressType;
float4        g_OneTexelOffset_Left;
float4        g_OneTexelOffset_Right;
float4        g_OneTexelOffset_Back;
float4        g_OneTexelOffset_Front;

// Ripple
// For vertex shader
#define        MAX_VS_RIPPLES 4
bool        g_HasRipple;
int            g_RippleNumVS0;
int            g_RippleNumVS1;
float4        g_RippleScaleOffsets0[MAX_VS_RIPPLES];
float4        g_RippleScaleOffsets1[MAX_VS_RIPPLES];
float4        g_RippleSelectors[MAX_VS_RIPPLES];
// For pixel shader
#define        MAX_PS_RIPPLES 8
int            g_RippleNumPS;
float4        g_RippleScaleOffsets[MAX_PS_RIPPLES];
int            g_S6_Address;
int            g_S7_Address;

// Color/Reflection/Refraction/Fog
bool        g_HasReflection;
bool        g_HasRefraction;
bool        g_HasCubeMap;
float3        g_ShallowColor;
float3        g_DeepColor;
float        g_ReflectScale;
float        g_ReflectIndex;
float        g_RefractIndex;
float        g_Transparent;
float        g_FogStart;
float        g_FogEnd;
float        g_FogDensity;
float3        g_FogColor;
bool        g_HasFog;

bool        g_HasHeightFog;
float        g_HeightFogStart;
float        g_HeightFogEnd;
float        g_HeightFogDensity;
float3        g_HeightFogColor;

// Foam
float        g_FoamScale;
float        g_FoamOffset;
float        g_FoamBlend;
bool        g_HasFoam;
bool        g_HasFoamPattern;
float        g_FoamPatternTile;
float        g_SubsurfaceScale;


//-----------------------------------------------------------------------------------
// Texture & Samplers
//-----------------------------------------------------------------------------------
texture        g_texHeightMap;
texture        g_texGradientMap;
texture        g_texFoldingMap;
texture1D    g_texColorMap;
textureCUBE    g_texCubemap;
texture        g_texRipple0;
texture        g_texRipple1;
texture        g_texReflectScene;
texture        g_texRefractScene;
texture        g_texFoamPattern;
texture        g_texDetiling;

// Displace map for height and choppy field
sampler g_samplerHeightMap =
sampler_state
{
    texture = (g_texHeightMap);
    MipFilter = None;
    MinFilter = Linear;
    MagFilter = Linear;
    
    AddressU = Wrap;
    AddressV = Wrap;
};

// Normal map for lighting
sampler g_samplerGradientMap =
sampler_state
{
    texture = (g_texGradientMap);
    MipFilter = None;
    MinFilter = Linear;
    MagFilter = Linear;
    
    AddressU = Wrap;
    AddressV = Wrap;
};

// Folding map for foam
sampler g_samplerFoldingMap =
sampler_state
{
    texture = (g_texFoldingMap);
    MipFilter = None;
    MinFilter = Linear;
    MagFilter = Linear;
    
    AddressU = Wrap;
    AddressV = Wrap;
};

// Blending map for ocean color
sampler g_samplerColorMap =
sampler_state
{
    texture = (g_texColorMap);
    MipFilter = None;
    MinFilter = Linear;
    MagFilter = Linear;
    
    AddressU = Clamp;
};

// Cube map for reflection
samplerCUBE g_samplerCubemap =
sampler_state
{
    texture = (g_texCubemap);
    MipFilter = None;
    MinFilter = Linear;
    MagFilter = Linear;
};

// Ripple map (vertex texture)
sampler g_samplerRipple0 =
sampler_state
{
    texture = (g_texRipple0);
    MipFilter = None;
    MinFilter = Linear;
    MagFilter = Linear;

    AddressU = Border;
    AddressV = Border;
    BorderColor = 0x00000000;
};

sampler g_samplerRipple1 =
sampler_state
{
    texture = (g_texRipple1);
    MipFilter = None;
    MinFilter = Linear;
    MagFilter = Linear;

    AddressU = Border;
    AddressV = Border;
    BorderColor = 0x00000000;
};

sampler g_samplerReflectScene =
sampler_state
{
    texture = (g_texReflectScene);
    MipFilter = None;
    MinFilter = Linear;
    MagFilter = Linear;

    AddressU = CLAMP;
    AddressV = CLAMP;
};

sampler g_samplerRefractScene =
sampler_state
{
    texture = (g_texRefractScene);
    MipFilter = None;
    MinFilter = Linear;
    MagFilter = Linear;

    AddressU = CLAMP;
    AddressV = CLAMP;
};

// Folding map for foam
sampler g_samplerFoamPattern =
sampler_state
{
    texture = (g_texFoamPattern);
    MipFilter = None;
    MinFilter = Linear;
    MagFilter = Linear;
    
    AddressU = Wrap;
    AddressV = Wrap;
};

// An additional texture helping to remove the tiling artifact.
sampler g_samplerDetiling =
sampler_state
{
    texture = (g_texDetiling);
    MipFilter = None;
    MinFilter = Linear;
    MagFilter = Linear;
    
    AddressU = Wrap;
    AddressV = Wrap;
};


// Texture array
texture        g_texSource0;
texture        g_texSource1;
texture        g_texSource2;
texture        g_texSource3;
texture        g_texSource4;
texture        g_texSource5;
texture        g_texSource6;
texture        g_texSource7;

sampler g_samplerSource0 =
sampler_state
{
    texture = (g_texSource0);
    MipFilter = None;
    MinFilter = Linear;
    MagFilter = Linear;

    AddressU = Border;
    AddressV = Border;
    BorderColor = 0x00000000;
};

sampler g_samplerSource1 =
sampler_state
{
    texture = (g_texSource1);
    MipFilter = None;
    MinFilter = Linear;
    MagFilter = Linear;

    AddressU = Border;
    AddressV = Border;
    BorderColor = 0x00000000;
};

sampler g_samplerSource2 =
sampler_state
{
    texture = (g_texSource2);
    MipFilter = None;
    MinFilter = Linear;
    MagFilter = Linear;

    AddressU = Border;
    AddressV = Border;
    BorderColor = 0x00000000;
};

sampler g_samplerSource3 =
sampler_state
{
    texture = (g_texSource3);
    MipFilter = None;
    MinFilter = Linear;
    MagFilter = Linear;

    AddressU = Border;
    AddressV = Border;
    BorderColor = 0x00000000;
};

sampler g_samplerSource4 =
sampler_state
{
    texture = (g_texSource4);
    MipFilter = None;
    MinFilter = Linear;
    MagFilter = Linear;

    AddressU = Border;
    AddressV = Border;
    BorderColor = 0x00000000;
};

sampler g_samplerSource5 =
sampler_state
{
    texture = (g_texSource5);
    MipFilter = None;
    MinFilter = Linear;
    MagFilter = Linear;

    AddressU = Border;
    AddressV = Border;
    BorderColor = 0x00000000;
};

sampler g_samplerSource6 =
sampler_state
{
    texture = (g_texSource6);
    MipFilter = None;
    MinFilter = Linear;
    MagFilter = Linear;

    AddressU = <g_S7_Address>;
    AddressV = <g_S7_Address>;
    BorderColor = 0x00000000;
};

sampler g_samplerSource7 =
sampler_state
{
    texture = (g_texSource7);
    MipFilter = None;
    MinFilter = Linear;
    MagFilter = Linear;

    AddressU = <g_S7_Address>;
    AddressV = <g_S7_Address>;
    BorderColor = 0x00000000;
};



struct VS_OUTPUT
{
    float4 Position     : POSITION;        // vertex position 
    float2 TexCoord     : TEXCOORD0;        // patch UV
    float4 ScreenPos : TEXCOORD1;
    float3 LocalPos     : TEXCOORD2;
};

struct VS_RIPPLE_OUTPUT
{
    float4 Position     : POSITION;        // vertex position 
    float4 TexCoord     : TEXCOORD0;        // patch uv & Ripple uv
    float4 ScreenPos : TEXCOORD1;
    float3 LocalPos     : TEXCOORD2;
};

//-----------------------------------------------------------------------------
// Name: OceanWaveVS
// Type: Vertex shader                                      
// Desc: 
//-----------------------------------------------------------------------------
VS_OUTPUT OceanWaveVS(float4 vPos : POSITION, float2 TexCoord : TEXCOORD0)
{
    VS_OUTPUT Output;
    
    float2 uv = vPos.xz * 0.0001;
    // Displacement map
    float3 displacement = tex2Dlod(g_samplerHeightMap, float4(uv, 0, 0)).xyz;
    vPos.xyz += displacement.xyz;

    // Transform
    Output.Position = mul(vPos, g_matWorldViewProj);
    Output.LocalPos = vPos;
    
    // Pass thru texture coordinate
    Output.TexCoord = uv;
    Output.ScreenPos = Output.Position;
    
    return Output; 
}

VS_OUTPUT OceanWaveVS_Distant(float4 vPos : POSITION, float2 TexCoord : TEXCOORD0)
{
    VS_OUTPUT Output;
    
    // No displacement map

    // Transform
    Output.Position = mul(vPos, g_matWorldViewProj);
    Output.LocalPos = vPos;
    
    // Pass thru texture coordinate
    Output.TexCoord = TexCoord;
    Output.ScreenPos = Output.Position;
    
    return Output; 
}

VS_OUTPUT OceanWaveVS_Mesh(float4 vPos : POSITION, float2 TexCoord : TEXCOORD0)
{
    VS_OUTPUT Output;
    
    // Expand texture coordinate
    float2 uv = mul(vPos, g_matTexCoord).xy;
    Output.TexCoord = uv;
    
    // Displacement map
    float4 world_pos = mul(vPos, g_matWorld);
    float3 displacement = tex2Dlod(g_samplerHeightMap, float4(uv, 0, 0)).xyz;
    world_pos.xyz += displacement;

    // Transform
    Output.Position = mul(world_pos, g_matViewProj);
    Output.LocalPos = world_pos;
    Output.ScreenPos = Output.Position;
    
    return Output; 
}

VS_RIPPLE_OUTPUT OceanWaveVS_Ripple(float4 vPos : POSITION, float2 TexCoord : TEXCOORD0)
{
    VS_RIPPLE_OUTPUT Output;

    // Displacement map
    float3 displacement = tex2Dlod(g_samplerHeightMap, float4(TexCoord, 0, 0)).xyz;
    vPos.xyz += displacement;

    // Add interaction ripple ------------- Buffer 0
    for (int i = 0; i < g_RippleNumVS0; i++)
    {
        float4 ripple_coord = 0;
        // g_RippleScaleOffsets: (x, y) - xy offset, (z, w) - xy scale
        ripple_coord.xy = vPos.xy * g_RippleScaleOffsets0[i].zw + g_RippleScaleOffsets0[i].xy;
        float4 tex_value = tex2Dlod(g_samplerRipple0, ripple_coord);
        vPos.z += dot(tex_value, g_RippleSelectors[i]);
    }

    // Add interaction ripple ------------- Buffer 1
    for (int i = 0; i < g_RippleNumVS1; i++)
    {
        float4 ripple_coord = 0;
        ripple_coord.xy = vPos.xy * g_RippleScaleOffsets1[i].zw + g_RippleScaleOffsets1[i].xy;
        float4 tex_value = tex2Dlod(g_samplerRipple1, ripple_coord);
        vPos.z += dot(tex_value, g_RippleSelectors[i]);
    }

    // Transform
    Output.Position = mul(vPos, g_matWorldViewProj);
    Output.LocalPos = vPos;
    
    // Pass thru texture coordinate, plus local position
    Output.TexCoord = float4(TexCoord, vPos.xy);
    Output.ScreenPos = Output.Position;
    
    return Output; 
}

VS_RIPPLE_OUTPUT OceanWaveVS_MeshRipple(float4 vPos : POSITION, float2 TexCoord : TEXCOORD0)
{
    VS_RIPPLE_OUTPUT Output;
    
    // Expand texture coordinate
    float2 uv = mul(vPos, g_matTexCoord).xy;
    
    // Displacement map.
    // Notice: here ...
    float4 world_pos = mul(vPos, g_matWorld);
    float3 displacement = tex2Dlod(g_samplerHeightMap, float4(uv, 0, 0)).xyz;
    world_pos.xyz += displacement;

    // Add interaction ripple ------------- Buffer 0
    for (int i = 0; i < g_RippleNumVS0; i++)
    {
        float4 ripple_coord = 0;
        // g_RippleScaleOffsets: (x, y) - xy offset, (z, w) - xy scale
        ripple_coord.xy = world_pos.xy * g_RippleScaleOffsets0[i].zw + g_RippleScaleOffsets0[i].xy;
        float4 tex_value = tex2Dlod(g_samplerRipple0, ripple_coord);
        world_pos.z += dot(tex_value, g_RippleSelectors[i]);
    }

    // Add interaction ripple ------------- Buffer 1
    for (int i = 0; i < g_RippleNumVS1; i++)
    {
        float4 ripple_coord = 0;
        ripple_coord.xy = world_pos.xy * g_RippleScaleOffsets1[i].zw + g_RippleScaleOffsets1[i].xy;
        float4 tex_value = tex2Dlod(g_samplerRipple1, ripple_coord);
        world_pos.z += dot(tex_value, g_RippleSelectors[i]);
    }

    // Transform
    Output.Position = mul(world_pos, g_matViewProj);
    Output.LocalPos = world_pos;

    // Pass thru texture coordinate, plus world position
    Output.TexCoord = float4(uv, world_pos.xy);
    Output.ScreenPos = Output.Position;
    
    return Output; 
}



//-----------------------------------------------------------------------------
// Name: FlatTextureVS
// Type: Vertex shader                                      
// Desc: Flat texture filling
//-----------------------------------------------------------------------------
VS_OUTPUT FlatTextureVS(float4 vPos : POSITION, float2 TexCoord : TEXCOORD0)
{
    VS_OUTPUT Output;
  
    // No need to do matrix transform.
    Output.Position = vPos;
    
    // Pass through general texture coordinate.
    // Notice: the screen pixels have a float pixel offset to the screen-space
    //   vertex coordinates. Here the screen size and texture size are same, so
    //   the float texel size is equal to float screen pixel size.
    //Output.TexCoord.x = 0.5f + vPos.x * 0.5f + g_HalfTexel.x;
    //Output.TexCoord.y = 0.5f - vPos.y * 0.5f + g_HalfTexel.y;
    Output.TexCoord = TexCoord;
    Output.ScreenPos = 0;
    
    // No use.
    Output.LocalPos = 0;
    
    return Output;
}











//-----------------------------------------------------------------------------
// Name: OceanWavePS
// Type: Pixel shader                                      
// Desc: 
//-----------------------------------------------------------------------------
float4 OceanWavePS(VS_OUTPUT In) : COLOR
{
    float4 out_color;

    // Calculate eye vector.
    // Beware: 'eye_vec' is a large number, 32bit floating point required.
    float3 eye_vec = g_LocalEye - In.LocalPos;
    float3 eye_dir = normalize(eye_vec);
    float dist = length(eye_vec);
    

    // --------------- Water body color
    float4 grad_fold = tex2D(g_samplerGradientMap, In.TexCoord);

    // Calculate normal here.
    float3 normal = grad_fold.xyz ;//float3(grad_fold.xy, g_GridLength_x2);
    normal = normalize(normal);

    // dot(N, V)    
    float cos_angle = dot(normal, eye_dir);
    float tex_coord = max(0.02f, cos_angle);
    // R channel is interpolation weight, G channel is reflective fresnel, B channel is refractive fresnel.
    float3 ramp = tex1D(g_samplerColorMap, tex_coord).xyz;
    float3 water_color = lerp(g_ShallowColor, g_DeepColor, ramp.x);
    return float4(water_color, 1);

/*    // --------------- Reflected color
    float3 reflect_color;
    if (g_HasReflection)
    {
        // Calc screen position.
        float2 screen_coord = -In.ScreenPos.xy / In.ScreenPos.w * 0.5f + 0.5f;

        // Calc distortion.
        float4 eye_2d = float4(-eye_dir.y, eye_dir.x, eye_dir.x, eye_dir.y * -4);
        float4 distort = eye_2d.xyzw * normal.xyxy;
        distort.xy = (distort.xz + distort.yw) * g_ReflectIndex;
        distort.y = max(0, distort.y);
        // Blend
        reflect_color = tex2D(g_samplerReflectScene, screen_coord + distort.xy) * g_ReflectScale;
        water_color = lerp(water_color, reflect_color, ramp.y);
    }
    else if (g_HasCubeMap)
    {
        float3 reflect_vec = reflect(-eye_vec, normal);
        reflect_vec.xy *= -1;
        reflect_color = texCUBE(g_samplerCubemap, reflect_vec) * g_ReflectScale;
        
        // Blend
        water_color.rgb = lerp(water_color, reflect_color, ramp.y);
    }
    
    
    // --------------- Refracted color
    float3 refract_color;

    if (g_HasRefraction)
    {
        // Calc screen position.
        float2 screen_coord = In.ScreenPos.xy / In.ScreenPos.w;
        screen_coord.x = 0.5f + screen_coord.x * 0.5f;
        screen_coord.y = 0.5f - screen_coord.y * 0.5f;

        // Calc distortion.
        float4 eye_2d = float4(-eye_dir.y, eye_dir.x, eye_dir.x, eye_dir.y);
        float4 distort = eye_2d.xyzw * normal.xyxy;
        distort.xy = (distort.xz + distort.yw) * g_RefractIndex;
        //distort.y = max(0, distort.y);
        // Blend
        refract_color = tex2D(g_samplerRefractScene, screen_coord + distort.xy);
        water_color = lerp(refract_color, water_color, ramp.z);
    }
    // -------------- Foam
    if (g_HasFoam)
    {
        water_color += grad_fold.z * g_SubsurfaceScale;
        float3 foam_pattern = g_HasFoamPattern ? tex2D(g_samplerFoamPattern, In.TexCoord * g_FoamPatternTile) : 1.0f;
        water_color += foam_pattern * grad_fold.w * g_FoamScale;
    }


    // -------------- Transparency
    out_color.rgb = water_color;
    out_color.a = ramp.z;
    

    // -------------- Fog
    if (g_HasFog)
    {
        float3 fog_weight = (dist - g_FogStart) / (g_FogEnd - g_FogStart);
        fog_weight = clamp(fog_weight, 0.0f, 1.0f) * g_FogDensity;
        out_color.rgb = lerp(out_color.rgb, g_FogColor, fog_weight);
    }
//    if (g_HasHeightFog)
//    {
//        float3 fog_weight = (dist - g_FogStart) / (g_FogEnd - g_FogStart);
//        fog_weight = clamp(fog_weight, 0.0f, 1.0f) * g_FogDensity;
//        out_color.rgb = lerp(out_color.rgb, g_FogColor, fog_weight);
//    }

    return out_color;*/
}

//-----------------------------------------------------------------------------
// Name: UnderWaterPS
// Type: Pixel shader                                      
// Desc: 
//-----------------------------------------------------------------------------
float4 UnderWaterPS(VS_OUTPUT In) : COLOR
{
    float4 out_color;

    float4 grad_fold = tex2D(g_samplerGradientMap, In.TexCoord);
    
    // Calculate normal here.
    float3 normal = float3(grad_fold.xy, g_GridLength_x2);
    normal = -normalize(normal);
    
    // Calculate eye vector.
    // Beware: 'eye_vec' is a large number, 32bit floating point required.
    float3 eye_vec = g_LocalEye - In.LocalPos;
    float3 eye_dir = normalize(eye_vec);
    float cos_angle = dot(normal, eye_dir);
    float tex_coord = max(0.02f, cos_angle);
    // R channel - interpolation weight
    // G channel - reflective fresnel
    // B channel - refractive fresnel
    // A channel - underwater fresnel
    float ramp = tex1D(g_samplerColorMap, tex_coord).w;

    // Calc screen position.
    float2 screen_coord = In.ScreenPos.xy / In.ScreenPos.w;
    screen_coord.x = 0.5f + screen_coord.x * 0.5f;
    screen_coord.y = 0.5f - screen_coord.y * 0.5f;

    // Calc distortion.
    float4 eye_2d = float4(-eye_dir.y, eye_dir.x, eye_dir.x, eye_dir.y);
    float4 distort = eye_2d.xyzw * normal.xyxy;
    distort.xy = (distort.xz + distort.yw) * g_RefractIndex;
    //distort.y = max(0, distort.y);
    // Blend
    out_color = tex2D(g_samplerRefractScene, screen_coord + distort.xy);
    
    float4 water_color = float4(g_DeepColor, 1);
    out_color = lerp(out_color, water_color, ramp);

    // -------------- Fog
    if (g_HasFog)
    {
        float dist = length(eye_vec);
        float3 fog_weight = (dist - g_FogStart) / (g_FogEnd - g_FogStart);
        fog_weight = clamp(fog_weight, 0.0f, 1.0f) * g_FogDensity;
        out_color.rgb = lerp(out_color.rgb, g_FogColor, fog_weight);
    }
    
    return out_color;
}


//sampler g_samplerRippleBuffers[MAX_PS_RIPPLES];

// Return value: accumulating gradient.
float2 SampleRipplePS(float2 local_pos)
{
    // Add interaction ripple ------ Buffer 0 ~ 7
    float2 ripple_coord;
    float2 grad_xy = 0;

//    for (int i = 0; i < g_RippleNumPS; i++)
//    {
//        // g_RippleScaleOffsets: (x, y) - xy offset, (z, w) - xy scale
//        ripple_coord = local_pos * g_RippleScaleOffsets[i].zw + g_RippleScaleOffsets[i].xy;
//        grad_xy += tex2D(g_samplerRippleBuffers[i], ripple_coord).zw * 2 - 1;
//    }

    // g_RippleScaleOffsets: (x, y) - xy offset, (z, w) - xy scale
    ripple_coord = local_pos * g_RippleScaleOffsets[0].zw + g_RippleScaleOffsets[0].xy;
    grad_xy += tex2D(g_samplerSource0, ripple_coord).zw;
    
    ripple_coord = local_pos * g_RippleScaleOffsets[1].zw + g_RippleScaleOffsets[1].xy;
    grad_xy += tex2D(g_samplerSource1, ripple_coord).zw;                                   
                                                                                       
    ripple_coord = local_pos * g_RippleScaleOffsets[2].zw + g_RippleScaleOffsets[2].xy;
    grad_xy += tex2D(g_samplerSource2, ripple_coord).zw;                                   
                                                                                       
    ripple_coord = local_pos * g_RippleScaleOffsets[3].zw + g_RippleScaleOffsets[3].xy;
    grad_xy += tex2D(g_samplerSource3, ripple_coord).zw;                                   
                                                                                       
    ripple_coord = local_pos * g_RippleScaleOffsets[4].zw + g_RippleScaleOffsets[4].xy;
    grad_xy += tex2D(g_samplerSource4, ripple_coord).zw;                                   
                                                                                       
    ripple_coord = local_pos * g_RippleScaleOffsets[5].zw + g_RippleScaleOffsets[5].xy;
    grad_xy += tex2D(g_samplerSource5, ripple_coord).zw;                                   
                                                                                       
    ripple_coord = local_pos * g_RippleScaleOffsets[6].zw + g_RippleScaleOffsets[6].xy;
    grad_xy += tex2D(g_samplerSource6, ripple_coord).zw;                                   
                                                                                       
    ripple_coord = local_pos * g_RippleScaleOffsets[7].zw + g_RippleScaleOffsets[7].xy;
    grad_xy += tex2D(g_samplerSource7, ripple_coord).zw;
    
    return grad_xy;
}

float4 OceanWavePS_Ripple(VS_RIPPLE_OUTPUT In) : COLOR
{
    float4 out_color = 0;

    float4 grad_fold = tex2D(g_samplerGradientMap, In.TexCoord.xy);
    grad_fold.xy += SampleRipplePS(In.TexCoord.zw);
    
    // Calculate normal from gradient
    float3 normal = float3(grad_fold.xy, g_GridLength_x2);
    normal = normalize(normal);
    
    // Calculate eye vector.
    // Beware: 'eye_vec' is a large number, 32bit floating point required.
    float3 eye_vec = g_LocalEye - In.LocalPos;
    float3 eye_dir = normalize(eye_vec);
    float cos_angle = dot(normal, eye_dir);
    
    float tex_coord = max(0.02f, cos_angle);
    // R channel is interpolation weight, G channel is reflective fresnel, B channel is refractive fresnel.
    float3 ramp = tex1D(g_samplerColorMap, tex_coord).xyz;
    float3 water_color = lerp(g_ShallowColor, g_DeepColor, ramp.x);


    // --------------- Reflected color
    float3 reflect_color;
    if (g_HasReflection)
    {
        // Calc screen position.
        float2 screen_coord = -In.ScreenPos.xy / In.ScreenPos.w * 0.5f + 0.5f;

        // Calc distortion.
        float4 eye_2d = float4(-eye_dir.y, eye_dir.x, eye_dir.x, eye_dir.y);
        float4 distort = eye_2d.xyzw * normal.xyxy;
        distort.xy = (distort.xz + distort.yw) * g_ReflectIndex;
        distort.y = max(0, distort.y);
        // Blend
        reflect_color = tex2D(g_samplerReflectScene, screen_coord + distort.xy) * g_ReflectScale;
        water_color = lerp(water_color, reflect_color, ramp.y);
    }
    else if (g_HasCubeMap)
    {
        // sky cubemap
        float3 reflect_vec = reflect(eye_vec, normal);
        reflect_color = texCUBE(g_samplerCubemap, reflect_vec) * g_ReflectScale;
        
        // Blend
        water_color = lerp(water_color, reflect_color, ramp.y);
    }
        
        
    // --------------- Refracted color
    float3 refract_color;
    
    if (g_HasRefraction)
    {
        // Calc screen position.
        float2 screen_coord = In.ScreenPos.xy / In.ScreenPos.w;
        screen_coord.x = 0.5f + screen_coord.x * 0.5f;
        screen_coord.y = 0.5f - screen_coord.y * 0.5f;

        // Calc distortion.
        float4 eye_2d = float4(-eye_dir.y, eye_dir.x, eye_dir.x, eye_dir.y);
        float4 distort = eye_2d.xyzw * normal.xyxy;
        distort.xy = (distort.xz + distort.yw) * g_RefractIndex;
        //distort.y = max(0, distort.y);
        // Blend
        refract_color = tex2D(g_samplerRefractScene, screen_coord + distort.xy);
        water_color = lerp(refract_color, water_color, ramp.z);
    }


    // -------------- Foam
    if (g_HasFoam)
    {
        water_color += grad_fold.z * g_SubsurfaceScale;
        float3 foam_pattern = g_HasFoamPattern ? tex2D(g_samplerFoamPattern, In.TexCoord * g_FoamPatternTile) : 1.0f;
        water_color += foam_pattern * grad_fold.w * g_FoamScale;
    }


    // -------------- Transparency
    out_color.rgb = water_color;
    out_color.a = ramp.z;
    

    // -------------- Fog
    if (g_HasFog)
    {
        float dist = length(eye_vec);
        float3 fog_weight = (dist - g_FogStart) / (g_FogEnd - g_FogStart);
        fog_weight = clamp(fog_weight, 0.0f, 1.0f) * g_FogDensity;
        out_color.rgb = lerp(out_color.rgb, g_FogColor, fog_weight);
    }

    return out_color;
}

float4 UnderWaterPS_Ripple(VS_RIPPLE_OUTPUT In) : COLOR
{
    float4 out_color;

    float4 grad_fold = tex2D(g_samplerGradientMap, In.TexCoord);
    grad_fold.xy += SampleRipplePS(In.TexCoord.zw);
    
    // Calculate normal here.
    float3 normal = float3(grad_fold.xy, g_GridLength_x2);
    normal = -normalize(normal);
    
    // Calculate eye vector.
    // Beware: 'eye_vec' is a large number, 32bit floating point required.
    float3 eye_vec = g_LocalEye - In.LocalPos;
    float3 eye_dir = normalize(eye_vec);
    float cos_angle = dot(normal, eye_dir);
    float tex_coord = max(0.02f, cos_angle);
    // R channel - interpolation weight
    // G channel - reflective fresnel
    // B channel - refractive fresnel
    // A channel - underwater fresnel
    float ramp = tex1D(g_samplerColorMap, tex_coord).w;

    // Calc screen position.
    float2 screen_coord = In.ScreenPos.xy / In.ScreenPos.w;
    screen_coord.x = 0.5f + screen_coord.x * 0.5f;
    screen_coord.y = 0.5f - screen_coord.y * 0.5f;

    // Calc distortion.
    float4 eye_2d = float4(-eye_dir.y, eye_dir.x, eye_dir.x, eye_dir.y);
    float4 distort = eye_2d.xyzw * normal.xyxy;
    distort.xy = (distort.xz + distort.yw) * g_RefractIndex;
    //distort.y = max(0, distort.y);
    // Blend
    out_color = tex2D(g_samplerRefractScene, screen_coord + distort.xy);
    
    float4 water_color = float4(g_DeepColor, 1);
    out_color = lerp(out_color, water_color, ramp);
    
    // -------------- Fog
    if (g_HasFog)
    {
        float dist = length(eye_vec);
        float3 fog_weight = (dist - g_FogStart) / (g_FogEnd - g_FogStart);
        fog_weight = clamp(fog_weight, 0.0f, 1.0f) * g_FogDensity;
        out_color.rgb = lerp(out_color.rgb, g_FogColor, fog_weight);
    }
    
    return out_color;
}


float4 OceanWireframePS(VS_OUTPUT In) : COLOR
{
    return float4(0.1f, 0.2f, 0.4f, 1.0f);
}

//-----------------------------------------------------------------------------
// Name: CalcGradientFoldingPS
// Type: Pixel shader                                      
// Desc: Calculate normal map in one pass.
//-----------------------------------------------------------------------------
float4 CalcGradientFoldingPS(VS_OUTPUT In) : COLOR
{
    // Sample neighbour texels
    float3 displace_left    = tex2D(g_samplerHeightMap, In.TexCoord.xy + g_OneTexelOffset_Left);
    float3 displace_right    = tex2D(g_samplerHeightMap, In.TexCoord.xy + g_OneTexelOffset_Right);
    float3 displace_back    = tex2D(g_samplerHeightMap, In.TexCoord.xy + g_OneTexelOffset_Back);
    float3 displace_front    = tex2D(g_samplerHeightMap, In.TexCoord.xy + g_OneTexelOffset_Front);
    
    // -------- Do not store the actual normal value, instead, it preserves two differential values.
    float2 gradient = {-(displace_right.z - displace_left.z), -(displace_front.z - displace_back.z)};
    

    // Calculate Jacobian corelation from the partial differential of height field
    float2 Dx = (displace_right.xy - displace_left.xy) * g_ChoppyScale_GridLen;
    float2 Dy = (displace_front.xy - displace_back.xy) * g_ChoppyScale_GridLen;
    float J = (1.0f + Dx.x) * (1.0f + Dy.y) - Dx.y * Dy.x;
    // Practical subsurface scale calculation: max[0, (1 - J) + Amplitude * (2 * Coverage - 1)].
    float fold = max(1.0f - J, 0);

    float3 A = {1,displace_right.y - displace_left.y,0};
    float3 B = {0,displace_front.y - displace_back.y,1};
    float3 Normal = cross(A,B);

    // Output
    //return float4(gradient, 0, fold);
    return float4(Normal , 0);

}

//-----------------------------------------------------------------------------
// Name: CalcFoldingPS
// Type: Pixel shader                                      
// Desc: Calculate folding map in one pass.
//-----------------------------------------------------------------------------
float4 CalcFoldingPS(VS_OUTPUT In) : COLOR
{
    // Sample neighbour texels
    float3 displace_left    = tex2D(g_samplerHeightMap, In.TexCoord.xy + g_OneTexelOffset_Left);
    float3 displace_right    = tex2D(g_samplerHeightMap, In.TexCoord.xy + g_OneTexelOffset_Right);
    float3 displace_back    = tex2D(g_samplerHeightMap, In.TexCoord.xy + g_OneTexelOffset_Back);
    float3 displace_front    = tex2D(g_samplerHeightMap, In.TexCoord.xy + g_OneTexelOffset_Front);
    
    // Calculate Jacobian corelation from the partial differential of height field
    float2 Dx = (displace_right.xy - displace_left.xy) * g_ChoppyScale_GridLen;
    float2 Dy = (displace_front.xy - displace_back.xy) * g_ChoppyScale_GridLen;
    float J = (1.0f + Dx.x) * (1.0f + Dy.y) - Dx.y * Dy.x;

    // Practical foam calculation: max[0, (1 - J) + Amplitude * (2 * Coverage - 1)].
    // Blending: src_foam + dst_foam * g_FoamBlend.
    float foam_amount = max(1 - J + g_FoamOffset, 0);

    // Output
    return float4(foam_amount, foam_amount, foam_amount, g_FoamBlend);
}










// Simulation parameters
float  g_DropStrength;
float2 g_ScrollOffset;
float  g_DisperseWeight0        = 0.5f;
float  g_DisperseWeight1        = 0.5f;
float  g_DisperseDampen        = 0.85f;
float  g_Equilibrium            = 0.2f;
// Combine parameters
float  g_MaxRippleAmplitude  = 20.0f;
float4x4 g_matCombine;
int   g_RippleSimAddress;

// Tex-op parameters
float2 g_OneTexLeft;
float2 g_OneTexRight;
float2 g_OneTexBack;
float2 g_OneTexFront;
//float2 g_OneTexLeftBack;
//float2 g_OneTexRightBack;
//float2 g_OneTexLeftFront;
//float2 g_OneTexRightFront;

// Droplet shape texture
texture g_texShape;
sampler g_samplerShape =
sampler_state
{
    texture = <g_texShape>;
    MipFilter = None;
    MinFilter = Linear;
    MagFilter = Linear;
    
    AddressU = Clamp;
    AddressV = Clamp;
};

// Iteration texture
texture g_texRippleHeightVel;
sampler g_samplerRippleHeightVel =
sampler_state
{
    texture = <g_texRippleHeightVel>;
    MipFilter = None;
    MinFilter = Linear;
    MagFilter = Linear;
    
    AddressU = <g_RippleSimAddress>;
    AddressV = <g_RippleSimAddress>;
};

texture g_texSource;
sampler g_samplerSource =
sampler_state
{
    texture = <g_texSource>;
    MipFilter = None;
    MinFilter = Linear;
    MagFilter = Linear;
    
    AddressU = Clamp;
    AddressV = Clamp;
};

texture g_texRippleDampen;
sampler g_samplerDampen =
sampler_state
{
    texture = <g_texRippleDampen>;
    MipFilter = None;
    MinFilter = Linear;
    MagFilter = Linear;
    
    AddressU = Clamp;
    AddressV = Clamp;
};



struct VS_QUAD_OUTPUT
{
    float4 Position    : POSITION;
    float2 TexCoord    : TEXCOORD0;
};




VS_QUAD_OUTPUT DrawDropletVS(float4 vPos : POSITION, float2 TexCoord : TEXCOORD0)
{
    VS_QUAD_OUTPUT Output;

    // Transform
    Output.Position = mul(vPos, g_matWorldViewProj);
    
    // Pass thru texture coordinate
    Output.TexCoord = TexCoord;
    
    return Output; 
}

float4 DrawDropletPS(VS_QUAD_OUTPUT In) : COLOR
{
    float drop_shape = tex2D(g_samplerShape, In.TexCoord).r;
    float height = drop_shape * g_DropStrength;
    
    return float4(height, 0, 0, 0);
}

VS_QUAD_OUTPUT SimpleVS(float4 vPos : POSITION, float2 TexCoord : TEXCOORD0)
{
    VS_QUAD_OUTPUT Output;

    // Transform
    Output.Position = vPos;
    // Pass thru texture coordinate
    Output.TexCoord = TexCoord;
    
    return Output; 
}

//VS_QUAD_OUTPUT RippleMergeVS(float4 vPos : POSITION, float2 TexCoord : TEXCOORD0)
//{
//    VS_QUAD_OUTPUT Output;
//
//    // Transform
//    Output.Position = mul(vPos, g_matCombine);
//    // Pass thru texture coordinate
//    Output.TexCoord = TexCoord;
//    
//    return Output; 
//}




float4 DispersePS(VS_QUAD_OUTPUT In) : COLOR
{
    float2 uv = In.TexCoord + g_ScrollOffset.xy;
    
    float  h_left   = tex2D(g_samplerRippleHeightVel, uv + g_OneTexLeft).x;
    float  h_right  = tex2D(g_samplerRippleHeightVel, uv + g_OneTexRight).x;
    float  h_back   = tex2D(g_samplerRippleHeightVel, uv + g_OneTexBack).x;
    float  h_front  = tex2D(g_samplerRippleHeightVel, uv + g_OneTexFront).x;

    // R - height field
    // G - velocity field
    float2 center   = tex2D(g_samplerRippleHeightVel, uv).xy;
    float  h_center = center.x;
    float  v_center = center.y;

    // Ripple disperse from each pixel.
    float  force    = (h_left + h_right + h_back + h_front) - (4 + g_Equilibrium) * h_center;
    float  v_new    = force * g_DisperseWeight0 + v_center;
    float  h_new    = (v_new * g_DisperseWeight1 + h_center) * g_DisperseDampen;

    // R - height field
    // G - velocity field
    // B, A - reserved for gradient
    float  dampen   = tex2D(g_samplerDampen, In.TexCoord).x;
    return float4(h_new, v_new, 0, 0) * dampen;
}

// Smooth the height field, calculate the (x,y) gradient and preserve the velocity field.
float4 CalcGradientPS(VS_QUAD_OUTPUT In) : COLOR
{
    float  h_left   = tex2D(g_samplerRippleHeightVel, In.TexCoord + g_OneTexLeft).x;
    float  h_right  = tex2D(g_samplerRippleHeightVel, In.TexCoord + g_OneTexRight).x;
    float  h_back   = tex2D(g_samplerRippleHeightVel, In.TexCoord + g_OneTexBack).x;
    float  h_front  = tex2D(g_samplerRippleHeightVel, In.TexCoord + g_OneTexFront).x;

    // Gradient can be summed up later to get the combined normal value.
    // Notice: here Y gradient must be reversed.
    float2 gradient = {-(h_right - h_left), h_front - h_back};
    
    // Height and velocity value.
    float2 center   = tex2D(g_samplerRippleHeightVel, In.TexCoord).xy;
    // Slightly blur out the height field.
    float  h_smooth = (h_left + h_right + h_front + h_back) * 0.05f + center.x * 0.8f;

    return float4(h_smooth, center.y, gradient);
}


float4 SimplePS(VS_QUAD_OUTPUT In) : COLOR
{
    return tex2D(g_samplerSource, In.TexCoord);
}

float4 RippleMergePS(VS_QUAD_OUTPUT In) : COLOR
{
    float value_0 = tex2D(g_samplerSource0, In.TexCoord).x;
    float value_1 = tex2D(g_samplerSource1, In.TexCoord).x;
    float value_2 = tex2D(g_samplerSource2, In.TexCoord).x;
    float value_3 = tex2D(g_samplerSource3, In.TexCoord).x;
    
    return float4(value_0, value_1, value_2, value_3);
}

float4 ConstantBlackPS() : COLOR
{
    return float4(0.0f, 0.0f, 0.0f, 1.0f);
}





//-----------------------------------------------------------------------------
// Name: OceanWaveTech
// Type: Technique
// Desc: 
//-----------------------------------------------------------------------------
technique RenderOceanSurfTech
{
    pass Pass_TileVS_WavePS
    {
        VertexShader = compile vs_3_0 OceanWaveVS();
        PixelShader  = compile ps_3_0 OceanWavePS();

        AlphaBlendEnable    = <g_Transparent>;
        SrcBlend            = SrcAlpha;
        DestBlend            = InvSrcAlpha;

        ZEnable                = True;
        ZFunc                = LessEqual;
        ZWriteEnable        = True; 
        FillMode            = Solid;
        CullMode            = None;
        AlphaTestEnable        = False;
        ColorWriteEnable    = 0x0f;
        StencilEnable        = False;
    }

    // For ocean mesh
    pass Pass_MeshVS_WavePS
    {
        VertexShader = compile vs_3_0 OceanWaveVS_Mesh();
        PixelShader  = compile ps_3_0 OceanWavePS();

        AlphaBlendEnable    = <g_Transparent>;
        SrcBlend            = SrcAlpha;
        DestBlend            = InvSrcAlpha;

        ZEnable                = True;
        ZFunc                = LessEqual;
        ZWriteEnable        = True; 
        FillMode            = Solid;
        CullMode            = None;
        AlphaTestEnable        = False;
        ColorWriteEnable    = 0x0f;
        StencilEnable        = False;
    }

    // Under water
    pass Pass_TileVS_UnderwaterPS
    {
        VertexShader = compile vs_3_0 OceanWaveVS();
        PixelShader  = compile ps_3_0 UnderWaterPS();

        StencilEnable        = True;

        AlphaBlendEnable    = False;
        ZEnable                = True;
        ZFunc                = LessEqual;
        ZWriteEnable        = True; 
        FillMode            = Solid;
        CullMode            = None;
        AlphaTestEnable        = False;
        ColorWriteEnable    = 0x0f;
    }

    // For ocean mesh, underwater
    pass Pass_MeshVS_UnderwaterPS
    {
        VertexShader = compile vs_3_0 OceanWaveVS_Mesh();
        PixelShader  = compile ps_3_0 UnderWaterPS();

        StencilEnable        = True;

        AlphaBlendEnable    = False;
        ZEnable                = True;
        ZFunc                = LessEqual;
        ZWriteEnable        = True; 
        FillMode            = Solid;
        CullMode            = None;
        AlphaTestEnable        = False;
        ColorWriteEnable    = 0x0f;
    }
    
    // For tiling surface with ripple
    pass Pass_TileRippleVS_WaveRipplePS
    {
        VertexShader = compile vs_3_0 OceanWaveVS_Ripple();
        PixelShader  = compile ps_3_0 OceanWavePS_Ripple();

        AlphaBlendEnable    = <g_Transparent>;
        SrcBlend            = SrcAlpha;
        DestBlend            = InvSrcAlpha;

        ZEnable                = True;
        ZFunc                = LessEqual;
        ZWriteEnable        = True; 
        FillMode            = Solid;
        CullMode            = None;
        AlphaTestEnable        = False;
        ColorWriteEnable    = 0x0f;
        StencilEnable        = False;
    }

    // For ocean mesh with ripple
    pass Pass_MeshRippleVS_WaveRipplePS
    {
        VertexShader = compile vs_3_0 OceanWaveVS_MeshRipple();
        PixelShader  = compile ps_3_0 OceanWavePS_Ripple();

        AlphaBlendEnable    = <g_Transparent>;
        SrcBlend            = SrcAlpha;
        DestBlend            = InvSrcAlpha;

        ZEnable                = True;
        ZFunc                = LessEqual;
        ZWriteEnable        = True; 
        FillMode            = Solid;
        CullMode            = None;
        AlphaTestEnable        = False;
        ColorWriteEnable    = 0x0f;
        StencilEnable        = False;
    }
    
    // Under water with ripple
    pass Pass_TileRippleVS_UnderwaterRipplePS
    {
        VertexShader = compile vs_3_0 OceanWaveVS_Ripple();
        PixelShader  = compile ps_3_0 UnderWaterPS_Ripple();

        StencilEnable        = True;

        AlphaBlendEnable    = False;
        ZEnable                = True;
        ZFunc                = LessEqual;
        ZWriteEnable        = True; 
        FillMode            = Solid;
        CullMode            = None;
        AlphaTestEnable        = False;
        ColorWriteEnable    = 0x0f;
    }
    
    // For ocean mesh, underwater with ripple
    pass Pass_MeshRippleVS_UnderwaterRipplePS
    {
        VertexShader = compile vs_3_0 OceanWaveVS_MeshRipple();
        PixelShader  = compile ps_3_0 UnderWaterPS_Ripple();

        StencilEnable        = True;

        AlphaBlendEnable    = False;
        ZEnable                = True;
        ZFunc                = LessEqual;
        ZWriteEnable        = True; 
        FillMode            = Solid;
        CullMode            = None;
        AlphaTestEnable        = False;
        ColorWriteEnable    = 0x0f;
    }

    // Calculate normal value
    pass Pass_GradientFolding
    {
        VertexShader = compile vs_3_0 FlatTextureVS();
        PixelShader  = compile ps_3_0 CalcGradientFoldingPS();

        ZEnable                = False;
        ZWriteEnable        = False; 
        FillMode            = Solid;
        CullMode            = None;
        AlphaBlendEnable    = False;
        AlphaTestEnable        = False;
        ColorWriteEnable    = 0x0f;
        StencilEnable        = False;
    }

    // Calculate folding value
    pass Pass_Folding
    {
        VertexShader = compile vs_3_0 FlatTextureVS();
        PixelShader  = compile ps_3_0 CalcFoldingPS();

        AlphaBlendEnable    = True;
        SrcBlend            = One;
        DestBlend            = SrcAlpha;

        ZEnable                = False;
        ZWriteEnable        = True; 
        FillMode            = Solid;
        CullMode            = None;
        AlphaTestEnable        = False;
        ColorWriteEnable    = 0x0f;
        StencilEnable        = False;
    }

    // Wireframe
    pass Pass_TileWireframe
    {
        VertexShader = compile vs_3_0 OceanWaveVS();
        PixelShader  = compile ps_3_0 OceanWireframePS();

        AlphaBlendEnable    = False;
        ZEnable                = False;
        FillMode            = Wireframe;
        CullMode            = None;
        AlphaTestEnable        = False;
        ColorWriteEnable    = 0x0f;
        StencilEnable        = False;
    }
    
    // For ocean mesh, wireframe
    pass Pass_MeshWireframe
    {
        VertexShader = compile vs_3_0 OceanWaveVS_Mesh();
        PixelShader  = compile ps_3_0 OceanWireframePS();

        AlphaBlendEnable    = <g_Transparent>;
        SrcBlend            = SrcAlpha;
        DestBlend            = InvSrcAlpha;

        ZEnable                = True;
        ZFunc                = LessEqual;
        ZWriteEnable        = True; 
        FillMode            = Wireframe;
        CullMode            = None;
        AlphaTestEnable        = False;
        ColorWriteEnable    = 0x0f;
        StencilEnable        = False;
    }

    pass Pass_Distant
    {
        VertexShader = compile vs_3_0 OceanWaveVS_Distant();
        PixelShader  = compile ps_3_0 OceanWavePS();

        AlphaBlendEnable    = <g_Transparent>;
        SrcBlend            = SrcAlpha;
        DestBlend            = InvSrcAlpha;
        
        SlopeScaleDepthBias    = 5.0f;
        DepthBias            = 5e-5f;
        ZWriteEnable        = True;//False; 

        ZEnable                = True;
        ZFunc                = LessEqual;
        FillMode            = Solid;
        CullMode            = None;
        AlphaTestEnable        = False;
        ColorWriteEnable    = 0x0f;
        StencilEnable        = False;
    }
    
    pass Pass_TileVS_ConstantBlackPS
    {
        VertexShader = compile vs_3_0 OceanWaveVS();
        PixelShader  = compile ps_3_0 ConstantBlackPS();
        
        AlphaBlendEnable    = <g_Transparent>;
        SrcBlend            = SrcAlpha;
        DestBlend            = InvSrcAlpha;

        ZEnable                = True;
        ZFunc                = LessEqual;
        ZWriteEnable        = True; 
        FillMode            = Solid;
        CullMode            = None;
        AlphaTestEnable        = False;
        ColorWriteEnable    = 0x0f;
        StencilEnable        = False;        
    }

    pass Pass_MeshVS_ConstantBlackPS
    {
        VertexShader = compile vs_3_0 OceanWaveVS_Mesh();
        PixelShader  = compile ps_3_0 ConstantBlackPS();
        
        AlphaBlendEnable    = <g_Transparent>;
        SrcBlend            = SrcAlpha;
        DestBlend            = InvSrcAlpha;

        ZEnable                = True;
        ZFunc                = LessEqual;
        ZWriteEnable        = True; 
        FillMode            = Solid;
        CullMode            = None;
        AlphaTestEnable        = False;
        ColorWriteEnable    = 0x0f;
        StencilEnable        = False;        
    } 
}



//-----------------------------------------------------------------------------
// Name: RenderRippleTech
// Type: Technique
// Desc: 
//-----------------------------------------------------------------------------
technique RenderRippleTech
{
    // Adding droplet
    pass Pass_AddDroplet
    {
        VertexShader = compile vs_3_0 DrawDropletVS();
        PixelShader  = compile ps_3_0 DrawDropletPS();

        AlphaBlendEnable    = True;
        SrcBlend            = One;
        DestBlend            = One;
        
        ZEnable                = False;
        ZWriteEnable        = False; 
        FillMode            = Solid;
        CullMode            = None;
        AlphaTestEnable        = False;
        ColorWriteEnable    = 0x0f;
        StencilEnable        = False;
    }
    
    // Ripple dispersing
    pass Pass_Disperse
    {
        VertexShader = compile vs_3_0 SimpleVS();
        PixelShader  = compile ps_3_0 DispersePS();

        ZEnable                = False;
        ZWriteEnable        = False; 
        FillMode            = Solid;
        CullMode            = None;
        AlphaBlendEnable    = False;
        AlphaTestEnable        = False;
        ColorWriteEnable    = 0x0f;
        StencilEnable        = False;
    }
    
    // Merge textures
    pass Pass_Merge
    {
        VertexShader = compile vs_3_0 SimpleVS();
        PixelShader  = compile ps_3_0 RippleMergePS();

        ZEnable                = False;
        ZWriteEnable        = True; 
        FillMode            = Solid;
        CullMode            = None;
        AlphaBlendEnable    = False;
        AlphaTestEnable        = False;
        ColorWriteEnable    = 0x0f;
        StencilEnable        = False;
    }

    // Copy texture, blur height field and calculate gradient
    pass Pass_CalcGradient
    {
        VertexShader = compile vs_3_0 SimpleVS();
        PixelShader  = compile ps_3_0 CalcGradientPS();

        ZEnable                = False;
        ZWriteEnable        = True; 
        FillMode            = Solid;
        CullMode            = None;
        AlphaBlendEnable    = False;
        AlphaTestEnable        = False;
        ColorWriteEnable    = 0x0f;
        StencilEnable        = False;
    }
}













// Buffer selector
int g_BufferType;

//-----------------------------------------------------------------------------
// Following only for debug
//-----------------------------------------------------------------------------
struct VS_DEBUG_OUTPUT
{
    float4 Position : POSITION;   
    float2 TexCoord    : TEXCOORD1;  
};

//-----------------------------------------------------------------------------
// Name: DebugTextureVS                                        
// Type: Vertex shader
//-----------------------------------------------------------------------------
VS_DEBUG_OUTPUT DisplayBufferVS(float4 vPos : POSITION, float2 TexCoord : TEXCOORD0)
{
    VS_DEBUG_OUTPUT Output;

    Output.Position = vPos;
    Output.TexCoord = TexCoord;

    return Output;
}

//-----------------------------------------------------------------------------
// Name: DisplayBufferPS                                        
// Type: Pixel shader
//-----------------------------------------------------------------------------
float4 DisplayBufferPS(VS_DEBUG_OUTPUT In) : COLOR
{
    if (g_BufferType == 1)
        return tex2D(g_samplerHeightMap, In.TexCoord) * 0.008f + 0.5f;
    else if (g_BufferType == 2)
    {
        float2 grad = tex2D(g_samplerGradientMap, In.TexCoord).xy;
        float3 normal = float3(grad, g_GridLength_x2);
        return float4(normalize(normal) * 0.5f + 0.5f, 0);
    }
    else if (g_BufferType == 3)
    {
        float fold = tex2D(g_samplerGradientMap, In.TexCoord).w;
        return fold * 0.5f;
    }
    else
        return 0;
}

//-----------------------------------------------------------------------------
// Name: DisplayBufferTech
// Type: Technique                                     
// Desc: For debug and performance tuning purpose: outputs a floating-point
//    on screen.
//-----------------------------------------------------------------------------
technique DisplayBufferTech
{
    pass P0
    {
        VertexShader = compile vs_3_0 DisplayBufferVS();
        PixelShader  = compile ps_3_0 DisplayBufferPS();

        StencilEnable = True;

        ZEnable = True;
        ZWriteEnable = True;
        ZFunc = Always;
        FillMode = Solid;
        CullMode = None;
        AlphaBlendEnable = False;
        AlphaTestEnable = False;
        ColorWriteEnable = 0x0f;
    }
}
