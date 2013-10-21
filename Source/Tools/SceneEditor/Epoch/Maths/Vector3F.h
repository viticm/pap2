// File: Vector3F.h
// Desc: 3D float vector

#ifndef VECTOR3F_H
#define VECTOR3F_H

#include"MathsCommon.h"

namespace Maths
{

class DLLAPI Vector3F
{
public:
	union
	{
		struct
		{
			float x, y, z;
		};
		float v[3];
	};

public:
	Vector3F(float fx = 0.f, float fy = 0.f, float fz = 0.f) : x(fx), y(fy), z(fz) {}
	Vector3F(const Vector3F& in): x(in.x), y(in.y), z(in.z) {}
	Vector3F& operator = (const Vector3F& in);
	~Vector3F() {}

	Vector3F(const D3DVECTOR& dv): x(dv.x), y(dv.y), z(dv.z) {}
	Vector3F(const D3DXVECTOR3& dxv3): x(dxv3.x), y(dxv3.y), z(dxv3.z) {}

	Vector3F& operator = (const D3DVECTOR& dv);
	Vector3F& operator = (const D3DXVECTOR3& dxv3);

	operator D3DVECTOR ();
	operator D3DXVECTOR3 () { return D3DXVECTOR3(x, y, z); }

	operator D3DVECTOR* ();
	operator D3DXVECTOR3* ();
	operator D3DVECTOR& ();
	operator D3DXVECTOR3& ();

	void ToD3DVECTOR(D3DVECTOR& dv) const;
	void ToD3DXVECTOR3(D3DXVECTOR3& dv) const;
	
	Vector3F& operator += (const Vector3F& in);
	Vector3F& operator -= (const Vector3F& in);
	Vector3F& operator *= (const float& in);
	Vector3F& operator /= (const float& in);
	Vector3F operator - () const;

	void AssignFromSphere(float theta, float rho, float phi);  // 从球面坐标创建向量

	float Square() const;  // 返回x、y、z平方和
	float Length() const;  // 返回向量的长度——也就是x、y、z平方和的平方根
	void Normalize();  // 标准化
	bool IsNormalized() const;

	void AddRespectively(const Vector3F& v, float tx, float ty, float tz);

	static const Vector3F Zero();
	static const Vector3F XAxis();
	static const Vector3F YAxis();
	static const Vector3F ZAxis();
};


// ******
// 与Vector3F有关的全局函数：

inline const Vector3F operator + (const Vector3F& a, const Vector3F& b)
{
	return Vector3F(a.x + b.x, a.y + b.y, a.z + b.z);
}

inline const Vector3F operator - (const Vector3F& a, const Vector3F& b)
{
	return Vector3F(a.x - b.x, a.y - b.y, a.z - b.z);
}

inline const Vector3F operator * (const Vector3F& a, float b)
{
	return Vector3F(a.x * b, a.y * b, a.z * b);
}

inline const Vector3F operator * (float a, const Vector3F& b)
{
	return Vector3F(a * b.x, a * b.y, a * b.z);
}

inline const Vector3F operator / (const Vector3F& a, float b)
{
	float inv = 1.f / NotZero(b);

	return Vector3F(a.x * inv, a.y * inv, a.z * inv);
}

// 向量叉乘函数
// 得到的是一个同时垂直于a、b的向量(记为c)：令左手四指指向b方向，拇指指向a方向，则手心朝向的方向就是c的方向
inline const Vector3F operator ^ (const Vector3F& a, const Vector3F& b)
{
	return Vector3F(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}

// 向量点乘函数：若其中一个为某平面的标准法向量，另一个为平面外一点，则点乘之积加上平面到原点的距离即为点到面的距离。
// 若另一个为平面内一点，则点乘之积为此平面到原点的距离
inline const float operator * (const Vector3F& a, const Vector3F& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

// 相等
inline bool operator == (const Vector3F& a, const Vector3F& b)
{
	return (FEqual(a.x, b.x) && FEqual(a.y, b.y) && FEqual(a.z, b.z));
}

inline bool operator != (const Vector3F& a, const Vector3F& b)
{
	return (FNotEqual(a.x, b.x) || FNotEqual(a.y, b.y) || FNotEqual(a.z, b.z));
}

inline bool operator > (const Vector3F& a, const Vector3F& b)
{
	return (FCompare(a.x, b.x) == 1 && FCompare(a.y, b.y) == 1 && FCompare(a.z, b.z) == 1);
}

inline bool operator < (const Vector3F& a, const Vector3F& b)
{
	return (FCompare(a.x, b.x) == -1 && FCompare(a.y, b.y) == -1 && FCompare(a.z, b.z) == -1);
}

inline bool operator >= (const Vector3F& a, const Vector3F& b)
{
	return (a > b)? true: (a == b);
}

inline bool operator <= (const Vector3F& a, const Vector3F& b)
{
	return (a < b)? true: (a == b);
}

inline float DistanceBetweenPoints(const Vector3F& a, const Vector3F& b)  // 两点之间的距离
{
	float dx = a.x - b.x;
	float dy = a.y - b.y;
	float dz = a.z - b.z;

	return sqrtf(dx * dx + dy * dy + dz * dz);
}

inline void SetMin(const Vector3F& a, const Vector3F& b, Vector3F& vOut)
{
	vOut.x = (a.x < b.x)? a.x: b.x;
	vOut.y = (a.y < b.y)? a.y: b.y;
	vOut.z = (a.z < b.z)? a.z: b.z;
}

inline void SetMax(const Vector3F& a, const Vector3F& b, Vector3F& vOut)
{
	vOut.x = (a.x > b.x)? a.x: b.x;
	vOut.y = (a.y > b.y)? a.y: b.y;
	vOut.z = (a.z > b.z)? a.z: b.z;
}

// 成员函数

inline Vector3F& Vector3F::operator = (const Vector3F& in)
{
	if(&in != this)
	{
		x = in.x; y = in.y; z = in.z;
	}
	
	return *this;
}

inline Vector3F& Vector3F::operator += (const Vector3F& in)
{
	x += in.x;
	y += in.y;
	z += in.z;

	return *this;
}

inline Vector3F& Vector3F::operator -= (const Vector3F& in)
{
	x -= in.x;
	y -= in.y;
	z -= in.z;

	return *this;
}

inline Vector3F& Vector3F::operator *= (const float& in)
{
	x *= in;
	y *= in;
	z *= in;

	return *this;
}

inline Vector3F& Vector3F::operator /= (const float& in)
{
	float inv = 1.f / NotZero(in);
	x *= inv;
	y *= inv;
	z *= inv;

	return *this;
}

inline Vector3F Vector3F::operator - () const
{
	return Vector3F(-x, -y, -z);
}

inline void Vector3F::AssignFromSphere(float theta, float rho, float phi)
{
	x = (float)(phi * cos(rho) * sin(theta));
	y = (float)(phi * sin(rho));
	z = (float)(phi * cos(rho) * cos(theta));
}

inline float Vector3F::Square() const
{
	return x * x + y * y + z * z;
}

inline float Vector3F::Length() const
{
	return sqrtf(Square());
}

inline void Vector3F::Normalize()
{
	float inv = 1.f / NotZero(Length());

	x *= inv; y *= inv; z *= inv;
}

inline bool Vector3F::IsNormalized() const
{
	return FEqual(Square(), 1.f);
}

inline void Vector3F::AddRespectively(const Vector3F& v, float tx, float ty, float tz)
{
	x += (v.x - x) * tx;
	y += (v.y - y) * ty;
	z += (v.z - z) * tz;
}

}


#endif