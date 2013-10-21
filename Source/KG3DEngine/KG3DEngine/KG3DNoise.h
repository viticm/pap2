#pragma once

#include <math.h>

class KG3DNoise
{
public:

	KG3DNoise(){};
	KG3DNoise(const int Oct,const float Per,const int W,const int H)
	{
		m_nNumberOfOctaves = Oct;
		m_fPersistence = Per;
		m_nWidth = W;
		m_nHeight = H;
	}

	void SetOctaves_Persistence(const int Oct,const float Per)
	{
		m_nNumberOfOctaves = Oct;
		m_fPersistence = Per;
	}
	float Noise(const int r)
	{
		int n;
		n = (r<<13) ^ r;
		return ( 1.0f - ( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f);  
	}
	float Linear_Interpolate(float a,float b,float x)
	{
		return a * (1.0f - x) + x * b; 
	}
	float Cosine_Interpolate(float a,float b,float x)
	{
		float ft,f;
		ft = x * 3.1415927f;
		f = (1 - cos(ft)) * .5f;

		return  a * (1 - f) + b * f;
	}

	void SetWidth_Height(const int W,const int H)
	{
		m_nWidth = W;
		m_nHeight = H;
	}

	float		*MakeNoise_1D();
	float		*MakeNoise_2D(const int Height);

private:

	int			m_nNumberOfOctaves; 
	float		m_fPersistence;
	int			m_nWidth,m_nHeight;
};