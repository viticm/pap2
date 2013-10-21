//<ShadowMap>
SHARED ShadowMapParam shadowMapParam = 
{
	false,
	{0, 0},
};
SHARED texture ShadowTexture;
sampler2D SamplerShadowTexture = sampler_state
{   
	Texture   = <ShadowTexture>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
	AddressU = BORDER;
	AddressV = BORDER;
	BorderColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
};

float GetShadowMask(float2 vScreenPos)
{
	return tex2D(SamplerShadowTexture, vScreenPos + 2 * float2(shadowMapParam.vBiasUV.x,shadowMapParam.vBiasUV.y)).r * 0.7 + 0.3;
}
//</ShadowMap>