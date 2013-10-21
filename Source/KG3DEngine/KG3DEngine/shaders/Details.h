/*
This file use for the "detail" feature, and preserved for other future extra shader parameters.
*/


texture tChannel;
sampler2D SamplerChannel = sampler_state
{
	Texture   = <tChannel>;
	MipFilter = LINEAR;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	ADDRESSU = WRAP;
	ADDRESSV = WRAP;
};
texture tDetail0;
sampler2D SamplerDetail0 = sampler_state
{
	Texture   = <tDetail0>;
	MipFilter = LINEAR;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	ADDRESSU = WRAP;
	ADDRESSV = WRAP;
};
texture tDetail1;
sampler2D SamplerDetail1 = sampler_state
{
	Texture   = <tDetail1>;
	MipFilter = LINEAR;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	ADDRESSU = WRAP;
	ADDRESSV = WRAP;
};
#if SHADER_MODEL==3

texture tFlow;
sampler2D SamplerFlow = sampler_state
{
	Texture   = <tFlow>;
	MipFilter = LINEAR;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	ADDRESSU = WRAP;
	ADDRESSV = WRAP;
};
#endif
//this struct must exactly match a certain struct declared in the kg3dengine
struct DetailParam 
{
	float4 vDetailScale;//x is detail0 uvscale,y is detail0 color scale;
	float4 vDetailMtl;

	float4 vDetail0Color;
	float4 vDetail1Color;
	float4 vBodyColor;
	float4 vBodyColor1;
	float4 vBodyScales;
	float4 vEnvSpecular;
};

DetailParam detailParam;
#if SHADER_MODEL==3

float4 vFlow;
float4 vFlowColor;
#endif

struct DetailResult 
{
	float noSkin_SkinPercent;
	float4 comColor;
	float4 skinTex;
	float4 speculardetail;
};

DetailResult CalDetail(float4 chan,float4 TexMain,float4 texColorNoSkin,float4 texColorSKin, float2 vDetailUV, samplerCUBE envTex, float3 envDir, float fSpecFactor)
{
	DetailResult dResult = (DetailResult)0;

#if SHADER_MODEL==3
	//float4 chan = tex2D(SamplerChannel,vDetailUV.xy);
	dResult.noSkin_SkinPercent = min(0.8,(TexMain.r+TexMain.g+TexMain.b)*0.3333);//chan.r;
	float4 diffuse= lerp(texColorNoSkin,texColorSKin,dResult.noSkin_SkinPercent)*TexMain;

	float4 tdetail0 = tex2D(SamplerDetail0,vDetailUV.xy * detailParam.vDetailScale.x);
	float4 tdetail1 = tex2D(SamplerDetail1,vDetailUV.xy *  detailParam.vDetailScale.z);

	float L = min(detailParam.vBodyScales.y,(diffuse.r + diffuse.g + diffuse.b)*0.3333 * detailParam.vBodyScales.x) + detailParam.vBodyScales.z;


	float4 bodycons0 = detailParam.vBodyColor  * lerp(diffuse,L,detailParam.vBodyColor.a);
	float4 bodycons1 = detailParam.vBodyColor1 * lerp(diffuse,L,detailParam.vBodyColor1.a);
	float4 bodycons2 = lerp(diffuse,bodycons0,chan.g);
	float4 body = lerp(bodycons2,bodycons1,chan.b);


	float4 detail0,detail1;
	detail0.rgb = tdetail0.rgb * detailParam.vDetail0Color * detailParam.vDetailScale.y * L;
	detail1.rgb = tdetail1.rgb * detailParam.vDetail1Color * detailParam.vDetailScale.w * L;
	detail0.a = chan.g*tdetail0.a * detailParam.vDetailScale.y * detailParam.vDetail0Color.a;
	detail1.a = chan.b*tdetail1.a * detailParam.vDetailScale.w * detailParam.vDetail1Color.a;

	float4 blend1 = lerp(body,detail0,detail0.a);
	
	dResult.comColor = lerp(blend1,detail1,detail1.a);//body * (1-detail0.a)*(1-detail1.a) + detail0 * detail0.a+detail1* detail1.a;
	//dResult.skinTex = lerp(lerp(diffuse,detail0,detail0.a*chan.r),detail1,detail1.a*chan.r);//*0.8 + float4(0.3,0.3,1,1)*0.2;
	dResult.skinTex = lerp(lerp(diffuse,detail0,detail0.a * dResult.noSkin_SkinPercent),detail1,detail1.a * dResult.noSkin_SkinPercent);

	float4 fac0 = detail0 * chan.g * detail0.a;
	
	float4 speculardetail = fSpecFactor * (detailParam.vDetailMtl.y * fac0);


	float4 fac1 = detail1 * chan.b * detail1.a;
	float4 ref = texCUBE(envTex, envDir);
	float4 env0 = detailParam.vDetailMtl.x * ref * fac0;
	float4 env1 = detailParam.vDetailMtl.z * ref * fac1;

	speculardetail += detailParam.vDetailMtl.w * fac1;
	float4 env = texCUBE(envTex, envDir+fac0 * 0.3) * chan.a * detailParam.vEnvSpecular.x + env0 + env1;
	float4 flow = tex2D(SamplerFlow,envDir.xy + vDetailUV.xy * vFlow.y + vFlow.zw) * vFlow.x * chan.a * vFlowColor;
	speculardetail += env;
	dResult.speculardetail = speculardetail;
	dResult.comColor += flow;
#else	//To many instructions that ps_2_0 would not efford, so we simply igonor detail blending in ps_2_0, otherwise the shader is unable to be compiled
	//float4 chan = tex2D(SamplerChannel,vDetailUV.xy);
	//dResult.noSkin_SkinPercent = chan.r;
	//dResult.noSkin_SkinPercent = min(0.8,(TexMain.r+TexMain.g+TexMain.b)*0.3333);
	//dResult.comColor = texColorNoSkin*TexMain;
	//dResult.skinTex = texColorSKin*TexMain;
	//float4 chan = tex2D(SamplerChannel,vDetailUV.xy);
	//dResult.noSkin_SkinPercent = min(0.8,(TexMain.r+TexMain.g+TexMain.b)*0.3333);//chan.r;

	//float4 diffuse= lerp(texColorNoSkin,texColorSKin,dResult.noSkin_SkinPercent)*TexMain;
	float4 diffuse = texColorNoSkin * TexMain;

	float4 tdetail0 = tex2D(SamplerDetail0,vDetailUV.xy * detailParam.vDetailScale.x);
	float4 tdetail1 = tex2D(SamplerDetail1,vDetailUV.xy *  detailParam.vDetailScale.z);

	float L = min(detailParam.vBodyScales.y,(diffuse.r + diffuse.g + diffuse.b)*0.3333 * detailParam.vBodyScales.x) + detailParam.vBodyScales.z;


	float4 bodycons0 = detailParam.vBodyColor  * lerp(diffuse,L,detailParam.vBodyColor.a);
	float4 bodycons1 = detailParam.vBodyColor1 * lerp(diffuse,L,detailParam.vBodyColor1.a);
	float4 bodycons2 = lerp(diffuse,bodycons0,chan.g);
	float4 body = lerp(bodycons2,bodycons1,chan.b);

	float4 detail0,detail1;
	detail0.rgb = tdetail0.rgb * detailParam.vDetail0Color * detailParam.vDetailScale.y * L;
	detail1.rgb = tdetail1.rgb * detailParam.vDetail1Color * detailParam.vDetailScale.w * L;
	detail0.a = chan.g*tdetail0.a * detailParam.vDetailScale.y * detailParam.vDetail0Color.a;
	detail1.a = chan.b*tdetail1.a * detailParam.vDetailScale.w * detailParam.vDetail1Color.a;

	//dResult.comColor = lerp(body,detail0,detail0.a);
	float4 blend1 = lerp(body,detail0,detail0.a);

	dResult.comColor = lerp(blend1,detail1,detail1.a);//body * (1-detail0.a)*(1-detail1.a) + detail0 * detail0.a+detail1* detail1.a;
	dResult.speculardetail = float4(0,0,0,0);

	//float4 fac0 = detail0 * chan.g * detail0.a;

	//float4 speculardetail = fSpecFactor * (detailParam.vDetailMtl.y * fac0);


	//float4 fac1 = detail1 * chan.b * detail1.a;
	//float4 ref = texCUBE(envTex, envDir);
	//float4 env0 = detailParam.vDetailMtl.x * ref * fac0;
	//float4 env1 = detailParam.vDetailMtl.z * ref * fac1;

	//speculardetail += detailParam.vDetailMtl.w * fac1;
	//float4 env = texCUBE(envTex, envDir+fac0 * 0.3) * chan.a * detailParam.vEnvSpecular.x + env0 + env1;
	//speculardetail += env;
	//dResult.speculardetail = speculardetail;
#endif

	return dResult;
}

