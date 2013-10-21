// light direction (view space)
float3 lightDir : LIGHTPOS0 <  
    string UIName = "Light Direction"; 
    string Object = "TargetLight";
    > = {-0.577, -0.577, 0.577};

// light intensity
float4 I_a : ENVAMB = { 1.0f, 1.0f, 1.0f, 1.0f };    // ambient
float4 I_d : LIGHTDIFF0 = { 1.0f, 1.0f, 1.0f, 1.0f };    // diffuse
float4 I_s = { 1.0f, 1.0f, 1.0f, 1.0f };    // specular

float4 edgeColor : COLOR
<
		string UIName = "Edge Color";
>;

float edgeWidth
<
	string UIName = "Edge Width";
>;

// material reflectivity
float4 k_a : MATERIALAMBIENT
<
    string UIName = "Ambient";
> = float4( 0.47f, 0.47f, 0.47f, 1.0f );    // ambient
    
float4 k_d : MATERIALDIFFUSE
<
    string UIName = "Diffuse";
> = float4( 0.47f, 0.47f, 0.47f, 1.0f );    // diffuse
    
float4 k_s : MATERIALSPECULAR
<
    string UIName = "Specular";
> = float4( 1.0f, 1.0f, 1.0f, 1.0f );    // diffuse    // specular

float Shinness : MATERIALPOWER
<
    string UIName = "Specular Power";
    string UIType = "IntSpinner";
    float UIMin = 0.0f;
    float UIMax = 50.0f;    
> = 15.0f;


// transformations
float4x4 World      :         WORLD;
float4x4 View       :         VIEW;
float4x4 Projection :         PROJECTION;
float4x4 WorldViewProj :     WORLDVIEWPROJECTION;
float4x4 WorldView :         WORLDVIEW;

texture colorTexture : DiffuseMap
<
    string UIName = "Diffuse";
>;

texture ambientMap: AmbientMap
<
                string UIName = "Ambient";
>;

texture specMap: specularmap
<
    string UIName = "Specular";
>;
sampler2D colorTextureSampler = sampler_state
{
    Texture = <colorTexture>;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
};

sampler2D ambientTextureSampler = sampler_state
{
    Texture = <ambientMap>;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
};


struct VS_OUTPUT
{
    float4 Pos  : POSITION;
    float2 Tex : TEXCOORD0;
    float2 Tex1: TEXCOORD1;
    float4 col : COLOR0;
    float edge: texcoord2;
};

VS_OUTPUT VS(
    float3 Pos  : POSITION, 
    float3 Norm : NORMAL, 
    float2 Tex  : TEXCOORD0,
    float2 Tex1 : TEXCOORD1)
{
    VS_OUTPUT Out = (VS_OUTPUT)0;

    float3 L = normalize(lightDir);

    float3 P = mul(float4(Pos, 1),(float4x4)WorldView);  // position (view space)
    float3 N = normalize(mul(Norm,(float3x3)WorldView)); // normal (view space)
    
    float3 R = normalize(2 * dot(N, L) * N - L);          // reflection vector (view space)
    float3 V = normalize(P);                             // view direction (view space)

    Out.Pos  = mul(float4(Pos,1),WorldViewProj);    // position (projected)
    
    float4 Diff = I_d * k_d * max(0, dot(N, L)); // diffuse 
    float4 Spec = I_s * k_s * pow(max(0, dot(R, V)), Shinness/4);   // specular
    Out.edge = pow(1.0f - max(0.0f, dot(N, -V)), edgeWidth);
    Out.Tex = Tex;
    Out.Tex1 = Tex1;
    Out.col = Diff + Spec;

    return Out;
}



float4 PS(
    float4 Diff : COLOR0,
    float2 Tex  : TEXCOORD0,
    float2 Tex1 : TEXCOORD1,
    float4 Spec : COLOR1,
    float  edge : texcoord2
     ) : COLOR
{
    float4 colorMap = tex2D(colorTextureSampler, Tex.xy)*2;
    float4 ambientMap = tex2D(ambientTextureSampler, Tex1.xy);
    float4 color = Diff * colorMap + Spec + I_a * k_a * ambientMap * colorMap;
    color += edge * edgeColor;
    return  color ;
}

technique DefaultTechnique
{
    pass P0
    {
        // shaders
        VertexShader = compile vs_1_1 VS();
        PixelShader  = compile ps_2_0 PS();
    }  
}


