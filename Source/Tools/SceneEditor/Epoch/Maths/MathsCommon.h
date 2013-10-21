// File: MathsCommon.h
// Desc:

#ifndef MATHSCOMMON_H
#define MATHSCOMMON_H

#include"..\Common\DLLAPI.h"

#include<stdlib.h>
#include<math.h>

#include<d3d9.h>
#include<d3d9types.h>
#include<d3dx9math.h>

#ifndef EPSILON
#define EPSILON 0.00001f  // 用于判断浮点数是否等于0
#endif

#ifndef PI
#define PI 3.1415926f
#endif

#ifndef SIGN_BIT
#define SIGN_BIT 0x80000000  // 负位
#endif

#ifndef INV_SIGN_BIT
#define INV_SIGN_BIT 0x7FFFFFFF  // 正位
#endif

namespace Maths
{

// 快速求浮点数绝对值
inline float FastFabs(const float& in)
{
	int* temp = (int*)&in;
	int out = *temp & INV_SIGN_BIT;
	return *((float*)&out);
}

// 产生随机数，位于-1到1之间
inline float FRand()
{
	return (float)(rand() - RAND_MAX/2) / (RAND_MAX/2);
}

inline bool FEqual(const float& a, const float& b)
{
	return FastFabs(a - b) <= EPSILON;
}

inline bool FNotEqual(const float& a, const float& b)
{
	return FastFabs(a - b) > EPSILON;
}

inline bool FEqualZero(const float& f)
{
	return (f >= -EPSILON)? (f <= EPSILON): false;
}

inline bool FNotEqualZero(const float& f)
{
	return (f < -EPSILON)? true: (f > EPSILON);
}

inline int FCompare(const float& a, const float& b)
{
	if(FastFabs(a - b) <= EPSILON)  // 相等
	{
		return 0;
	}

	return (a < b)? -1: 1;
}

inline int NotZero(int i)
{
	return (i == 0)? 1: i;
}

inline float NotZero(float f)
{
	if(FastFabs(f) < EPSILON)
	{
		f = (f > 0)? (f + EPSILON): (f - EPSILON);
	}
	return f;
}

inline float Round(float f)
{
	return (f > 0)? ceilf(f): floorf(f);
}

inline unsigned int ModColor(unsigned int uiOriginalColor, unsigned int uiNumBits)  // It's weird! Who need to mod a color?
{
	unsigned int uiNumColors = pow(2, uiNumBits);
	return (uiOriginalColor < uiNumColors)? uiOriginalColor: uiOriginalColor % uiNumColors;
}

inline unsigned int ClampColor(int iOriginalColor, unsigned int uiNumBits)
{
	unsigned int uiMaxColor = pow(2, uiNumBits) - 1;
	return (iOriginalColor < 0)? 0: (iOriginalColor > uiMaxColor)? uiMaxColor: iOriginalColor;
}

inline float ClampColor(float fOriginalColor, float fMaxColor = 1.f)
{
	return (fOriginalColor < 0.f)? 0.f: (fOriginalColor > fMaxColor)? fMaxColor: fOriginalColor;
}

DLLAPI enum RelativeLocation
{
	RL_FRONT,  // 对平面来说是前，对包围球(盒)来说是内
	RL_BACK,  // 对平面来说是后，对包围球(盒)来说是外
	RL_SPLIT,  // 相交
	RL_COPLANAR  // 共面
};

inline unsigned int CalNumOfPower(unsigned int Num)
{
	unsigned int ret = 0;
	while(Num > 0)
	{
		++ret;
		Num /= 2;
	}

	return ret;
}

inline int GetRandom(int min, int max)  // 随机的得到min、max之间的某个数，包括min和max
{
	return rand() % (max - min + 1) + min;
}

inline float GetRandom(float min, float max, int span = 5)
{
	if(span < 0)
	{
		span = -span;
	}
	int t = GetRandom(0, span);

	return min + t * (max - min) / span;
}

}  // namespace


#endif