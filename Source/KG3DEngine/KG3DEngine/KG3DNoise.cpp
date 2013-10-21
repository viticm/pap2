#include "stdafx.h"
#include "KG3DNoise.h"


#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

float *KG3DNoise::MakeNoise_2D(const int Height)
{
	float total = 0;
	int i,j,k,temp,ii,jj;
	int frequency;
	float amplitude,t;
	float *OutputData;
	//	float **rendom,**Octaves;
	float f1,f2,f3,f4,v1,v2,v3;
	int wavelenghX,wavelenghY;
	std::vector<float *> Octaves(m_nNumberOfOctaves);
	std::vector<float *> rendom(m_nNumberOfOctaves);


	for(i = 0;i < m_nNumberOfOctaves;++i)
	{
		frequency = 1 << i;
		Octaves[i] = new float[m_nWidth * m_nHeight];
		rendom[i] = new float[(frequency + 1) * (frequency + 1)];

	}
	OutputData = new float[m_nWidth * m_nHeight];

	//init rendom
	for(i = 0;i < m_nNumberOfOctaves;++i)
	{
		frequency = 1 << i;
		for(j = 0; j < (frequency + 1) * (frequency + 1);++j)
		{
			srand((UINT)(time(NULL) + i * m_nWidth + j));
			temp = rand();
			t = Noise(temp);
			rendom[i][j] = t;
		}
	}

	for(i = 0;i < m_nNumberOfOctaves;++i)
	{
		frequency = 1 << i;
		amplitude = pow(m_fPersistence,i);
		wavelenghX = m_nWidth / frequency;
		wavelenghY = m_nHeight / frequency;

		for(j = 0;j < frequency;++j)
		{
			for(k = 0;k < frequency;++k)
			{
				f1 = rendom[i][j * frequency + k];
				f2 = rendom[i][j * frequency + k + 1];
				f3 = rendom[i][(j + 1) * frequency + k];
				f4 = rendom[i][(j + 1) * frequency + k + 1];

				for(ii = j * wavelenghY;ii < (j + 1) * wavelenghY;++ii)
				{
					for(jj = k * wavelenghX;jj < (k + 1) * wavelenghY;++jj)
					{
						t = (float)jj / (k + 1) / wavelenghX;
						v1 = Cosine_Interpolate(f1,f2,t);

						t = (float)jj / (k + 1) / wavelenghX;
						v2 = Cosine_Interpolate(f3,f4,t);

						t = (float)ii / (j + 1) / wavelenghY;
						v3 = Cosine_Interpolate(v1,v2,t);

						Octaves[i][ii * m_nWidth + jj] = v3 * amplitude;
					}
				}
			}
		}
	}

	for(i = 0;i < m_nHeight;++i)
	{
		for(j = 0;j < m_nWidth;++j)
		{
			total = 0.0f;

			for(k = 0;k < m_nNumberOfOctaves;++k)
				total += Octaves[k][i * m_nWidth + j];

			total = total >= 0.0001f ? total : 0.0f;
			OutputData[i * m_nWidth + j] = total * Height;
		}
	}

	for(i = 0;i < m_nNumberOfOctaves;++i)
	{
		SAFE_DELETE_ARRAY(Octaves[i]);
		SAFE_DELETE_ARRAY(rendom[i]);
	}

	return OutputData;
}