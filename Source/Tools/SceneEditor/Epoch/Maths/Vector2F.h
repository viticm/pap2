// File: Vector2F.h
// Desc: 2D float vector

#ifndef VECTOR2F_H
#define VECTOR2F_H


#include"MathsCommon.h"

namespace Maths
{

class DLLAPI Vector2F
{
public:
	union
	{
		struct
		{
			float x, y;
		};
		float v[2];
	};

public:
	Vector2F(float fx = 0, float fy = 0): x(fx), y(fy) {}
	Vector2F(const Vector2F& in): x(in.x), y(in.y) {}
	Vector2F& operator = (const Vector2F& in);
	~Vector2F() {}

	Vector2F(const D3DXVECTOR2& dxv2): x(dxv2.x), y(dxv2.y) {}

	operator D3DXVECTOR2 () { return D3DXVECTOR2(x, y); }

	Vector2F& operator += (const Vector2F& in);
	Vector2F& operator -= (const Vector2F& in);
	Vector2F& operator *= (const float& in);
	Vector2F& operator /= (const float& in);
	Vector2F operator - () const;

	float Square() const;  // 返回x、y平方和
	float Length() const;  // 返回x、y平方和的平方根

	void Normalize();  // 标准化
	bool IsNormalized() const;
};

inline const Vector2F operator + (const Vector2F& a, const Vector2F& b)
{
	return Vector2F(a.x + b.x, a.y + b.y);
}

inline const Vector2F operator - (const Vector2F& a, const Vector2F& b)
{
	return Vector2F(a.x - b.x, a.y - b.y);
}

inline const Vector2F operator * (const Vector2F& a, float b)
{
	return Vector2F(a.x * b, a.y * b);
}

inline const Vector2F operator * (float a, const Vector2F& b)
{
	return Vector2F(a * b.x, a * b.y);
}

inline const Vector2F operator / (const Vector2F& a, float b)
{
	float inv = 1.f / NotZero(b);

	return Vector2F(a.x * inv, a.y * inv);
}

// 点乘
inline const float operator * (const Vector2F& a, const Vector2F& b)
{
	return a.x * b.x + a.y * b.y;
}

// 相等
inline bool operator == (const Vector2F& a, const Vector2F& b)
{
	return (FEqual(a.x, b.x) && FEqual(a.y, b.y));
}

inline bool operator != (const Vector2F& a, const Vector2F& b)
{
	return (FNotEqual(a.x, b.x) || FNotEqual(a.y, b.y));
}

inline bool operator > (const Vector2F& a, const Vector2F& b)
{
	if(FCompare(a.x, b.x) == 1 && FCompare(a.y, b.y) == 1)
	{
		return true;
	}

	return false;
}

inline bool operator < (const Vector2F& a, const Vector2F& b)
{
	if(FCompare(a.x, b.x) == -1 && FCompare(a.y, b.y) == -1)
	{
		return true;
	}

	return false;
}

inline bool operator >= (const Vector2F& a, const Vector2F& b)
{
	return (a > b)? true: (a == b);
}

inline bool operator <= (const Vector2F& a, const Vector2F& b)
{
	return (a < b)? true: (a == b);
}

inline float DistanceBetweenPoints(const Vector2F& a, const Vector2F& b)  // 两点之间的距离
{
	float dx = a.x - b.x;
	float dy = a.y - b.y;
	return sqrtf(dx * dx + dy * dy);
}

inline Vector2F& Vector2F::operator = (const Vector2F& in)
{
	if(&in != this)
	{
		x = in.x;
		y = in.y;
	}
	
	return *this;
}

inline Vector2F& Vector2F::operator += (const Vector2F& in)
{
	x += in.x;
	y += in.y;

	return *this;
}

inline Vector2F& Vector2F::operator -= (const Vector2F& in)
{
	x -= in.x;
	y -= in.y;

	return *this;
}

inline Vector2F& Vector2F::operator *= (const float& in)
{
	x *= in;
	y *= in;

	return *this;
}

inline Vector2F& Vector2F::operator /= (const float& in)
{
	float temp = 1.f / NotZero(in);
	x *= temp;
	y *= temp;

	return *this;
}

inline Vector2F Vector2F::operator - () const
{
	return Vector2F(-x, -y);
}

inline float Vector2F::Square() const
{
	return x * x + y * y;
}

inline float Vector2F::Length() const
{
	return sqrtf(Square());
}

inline void Vector2F::Normalize()
{
	float inv = 1.f / Length();

	x *= inv;
	y *= inv;
}

inline bool Vector2F::IsNormalized() const
{
	return FEqual(Square(), 1.f);
}

}  // namespace


#endif