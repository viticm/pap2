#if POINTLIGHT_ENABLE==1
#define MaxPointLightNum 8

int nPointLightCount;
float4 PointLightPosition[MaxPointLightNum];
float4 PointLightColor[MaxPointLightNum];  
#endif

float3 CalPointLight(float3 normal, float3 pos)
{
#if POINTLIGHT_ENABLE==1
	float3 ret = {0,0,0};
	for(int i=0;i<nPointLightCount;i++)
	{
		float3 V = PointLightPosition[i].xyz - pos;
		float dis = max(0,1-length(V) * PointLightPosition[i].w);
		float t = max(0,dot(normal,normalize(V)));
		ret += dis * t * PointLightColor[i].rgb * PointLightColor[i].a;
	}
	return ret;
#else
	return float3(0,0,0);
#endif
}