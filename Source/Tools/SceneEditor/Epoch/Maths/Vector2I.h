// File: Vector2I.h
// Desc: 2D integer vector; used for overlays

#ifndef VECTOR2I_H
#define VECTOR2I_H


#include"MathsCommon.h"

namespace Maths
{

class DLLAPI Vector2I
{
public:
	union
	{
		struct
		{
			int x, y;
		};
		int v[2];
	};

public:
	Vector2I(int ix = 0, int iy = 0): x(ix), y(iy) {}
	Vector2I(const Vector2I& in): x(in.x), y(in.y) {}
	Vector2I& operator = (const Vector2I& in);
	~Vector2I() {}

	Vector2I(const D3DXVECTOR2& dxv2): x(static_cast<int>(dxv2.x)), y(static_cast<int>(dxv2.y)) {}

	operator D3DXVECTOR2 () { return D3DXVECTOR2(static_cast<float>(x), static_cast<float>(y)); }

	Vector2I& operator += (const Vector2I& in);
	Vector2I& operator -= (const Vector2I& in);
	Vector2I& operator *= (const float& in);
	Vector2I& operator /= (const float& in);
	Vector2I operator - () const;

	unsigned int Square() const;  // 返回x、y平方和
	unsigned int Length() const;  // 返回x、y平方和的平方根
};

inline const Vector2I operator + (const Vector2I& a, const Vector2I& b)
{
	return Vector2I(a.x + b.x, a.y + b.y);
}

inline const Vector2I operator - (const Vector2I& a, const Vector2I& b)
{
	return Vector2I(a.x - b.x, a.y - b.y);
}

inline const Vector2I operator * (const Vector2I& a, float b)
{
	return Vector2I((int)Round(a.x * b), (int)Round(a.y * b));
}

inline const Vector2I operator * (float a, const Vector2I& b)
{
	return Vector2I((int)Round(a * b.x), (int)Round(a * b.y));
}

inline const Vector2I operator / (const Vector2I& a, float b)
{
	float inv = 1.f / NotZero(b);

	return Vector2I((int)Round(a.x * inv), (int)Round(a.y * inv));
}

// 点乘
inline const int operator * (const Vector2I& a, const Vector2I& b)
{
	return a.x * b.x + a.y * b.y;
}

// 相等
inline bool operator == (const Vector2I& a, const Vector2I& b)
{
	if(a.x == b.x && a.y == b.y)
	{
		return true;
	}

	return false;
}

inline bool operator != (const Vector2I& a, const Vector2I& b)
{
	return !(a == b);
}

inline bool operator > (const Vector2I& a, const Vector2I& b)
{
	if(a.x > b.x && a.y > b.y)
	{
		return true;
	}

	return false;
}

inline bool operator < (const Vector2I& a, const Vector2I& b)
{
	if(a.x < b.x && a.y < b.y)
	{
		return true;
	}

	return false;
}

inline bool operator >= (const Vector2I& a, const Vector2I& b)
{
	return (a > b)? true: (a == b);
}

inline bool operator <= (const Vector2I& a, const Vector2I& b)
{
	return (a < b)? true: (a == b);
}

inline int DistanceBetweenPoints(const Vector2I& a, const Vector2I& b)  // 两点之间的距离
{
	int dx = a.x - b.x;
	int dy = a.y - b.y;
	return (int)sqrtf(dx * dx + dy * dy);
}

inline Vector2I& Vector2I::operator = (const Vector2I& in)
{
	if(&in != this)
	{
		x = in.x;
		y = in.y;
	}
	
	return *this;
}

inline Vector2I& Vector2I::operator += (const Vector2I& in)
{
	x += in.x;
	y += in.y;

	return *this;
}

inline Vector2I& Vector2I::operator -= (const Vector2I& in)
{
	x -= in.x;
	y -= in.y;

	return *this;
}

inline Vector2I& Vector2I::operator *= (const float& in)
{
	int temp = (int)Round(in);
	x *= temp;
	y *= temp;

	return *this;
}

inline Vector2I& Vector2I::operator /= (const float& in)
{
	int temp = NotZero((int)Round(in));
	x /= temp;
	y /= temp;

	return *this;
}

inline Vector2I Vector2I::operator - () const
{
	return Vector2I(-x, -y);
}

inline unsigned int Vector2I::Square() const
{
	return x * x + y * y;
}

inline unsigned int Vector2I::Length() const
{
	return (unsigned int)sqrtf(Square());
}

}  // namespace


#endif