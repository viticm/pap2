// File: Vector3I.h
// Desc: 3D integer vector; used for bounding boxes and overlays which have z-indices

#ifndef VECTOR3I_H
#define VECTOR3I_H


#include"MathsCommon.h"

namespace Maths
{

class DLLAPI Vector3I
{
public:
	union
	{
		struct
		{
			int x, y, z;
		};
		int v[3];
	};

public:
	Vector3I(int ix = 0, int iy = 0, int iz = 0) : x(ix), y(iy), z(iz) {}
	Vector3I(const Vector3I& in): x(in.x), y(in.y), z(in.z) {}
	Vector3I& operator = (const Vector3I& in);
	~Vector3I() {}

	Vector3I(const D3DVECTOR& dv): x(static_cast<int>(dv.x)), y(static_cast<int>(dv.y)), z(static_cast<int>(dv.z)) {}
	Vector3I(const D3DXVECTOR3& dxv3): x(static_cast<int>(dxv3.x)), y(static_cast<int>(dxv3.y)), z(static_cast<int>(dxv3.z)) {}

	operator D3DVECTOR ();
	operator D3DXVECTOR3 () { return D3DXVECTOR3(static_cast<float>(x), static_cast<float>(y), static_cast<int>(z)); }

	Vector3I& operator += (const Vector3I& in);
	Vector3I& operator -= (const Vector3I& in);
	Vector3I& operator *= (const float& in);
	Vector3I& operator /= (const float& in);
	Vector3I operator - () const;

	void AssignFromSphere(float theta, float rho, float phi);  // 从球面坐标创建向量

	unsigned int Square() const;  // 返回x、y、z平方和
	unsigned int Length() const;  // 返回向量的长度——也就是x、y、z平方和的平方根

	void AddRespectively(const Vector3I& v, float tx, float ty, float tz);

	static const Vector3I Zero();
	static const Vector3I i();
	static const Vector3I j();
	static const Vector3I k();
};

inline const Vector3I operator + (const Vector3I& a, const Vector3I& b)
{
	return Vector3I(a.x + b.x, a.y + b.y, a.z + b.z);
}

inline const Vector3I operator - (const Vector3I& a, const Vector3I& b)
{
	return Vector3I(a.x - b.x, a.y - b.y, a.z - b.z);
}

inline const Vector3I operator * (const Vector3I& a, float b)
{
	return Vector3I((int)Round(a.x * b), (int)Round(a.y * b), (int)Round(a.z * b));
}

inline const Vector3I operator * (float a, const Vector3I& b)
{
	return Vector3I((int)Round(a * b.x), (int)Round(a * b.y), (int)Round(a * b.z));
}

inline const Vector3I operator / (const Vector3I& a, float b)
{
	float inv = 1.f / NotZero(b);

	return Vector3I((int)Round(a.x * inv), (int)Round(a.y * inv), (int)Round(a.z * inv));
}

// 向量叉乘函数
// 得到的是一个同时垂直于a、b的向量(记为c)：令左手四指指向b方向，拇指指向a方向，则手心朝向的方向就是c的方向
inline const Vector3I operator ^ (const Vector3I& a, const Vector3I& b)
{
	return Vector3I(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}

// 向量点乘函数：若其中一个为某平面的标准法向量，另一个为平面外一点，则点乘之积加上平面到原点的距离即为点到面的距离。
// 若另一个为平面内一点，则点乘之积为此平面到原点的距离
inline const int operator * (const Vector3I& a, const Vector3I& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

// 相等
inline bool operator == (const Vector3I& a, const Vector3I& b)
{
	if(a.x == b.x && a.y == b.y && a.z == b.z)
	{
		return true;
	}

	return false;
}

inline bool operator != (const Vector3I& a, const Vector3I& b)
{
	return !(a == b);
}

inline bool operator > (const Vector3I& a, const Vector3I& b)
{
	if(a.x > b.x && a.y > b.y && a.z > b.z)
	{
		return true;
	}

	return false;
}

inline bool operator < (const Vector3I& a, const Vector3I& b)
{
	if(a.x < b.x && a.y < b.y && a.z < b.z)
	{
		return true;
	}

	return false;
}

inline bool operator >= (const Vector3I& a, const Vector3I& b)
{
	return (a > b)? true: (a == b);
}

inline bool operator <= (const Vector3I& a, const Vector3I& b)
{
	return (a < b)? true: (a == b);
}

inline int DistanceBetweenPoints(const Vector3I& a, const Vector3I& b)  // 两点之间的距离
{
	int dx = a.x - b.x;
	int dy = a.y - b.y;
	int dz = a.z - b.z;

	return (int)sqrtf(dx * dx + dy * dy + dz * dz);
}

// 成员函数

inline Vector3I& Vector3I::operator = (const Vector3I& in)
{
	if(&in != this)
	{
		x = in.x; y = in.y; z = in.z;
	}
	
	return *this;
}

inline Vector3I& Vector3I::operator += (const Vector3I& in)
{
	x += in.x;
	y += in.y;
	z += in.z;

	return *this;
}

inline Vector3I& Vector3I::operator -= (const Vector3I& in)
{
	x -= in.x;
	y -= in.y;
	z -= in.z;

	return *this;
}

inline Vector3I& Vector3I::operator *= (const float& in)
{
	int temp = (int)Round(in);

	x *= temp;
	y *= temp;
	z *= temp;

	return *this;
}

inline Vector3I& Vector3I::operator /= (const float& in)
{
	int temp = NotZero((int)Round(in));

	x /= temp;
	y /= temp;
	z /= temp;

	return *this;
}

inline Vector3I Vector3I::operator - () const
{
	return Vector3I(-x, -y, -z);
}

inline void Vector3I::AssignFromSphere(float theta, float rho, float phi)
{
	x = (int)(phi * cos(rho) * sin(theta));
	y = (int)(phi * sin(rho));
	z = (int)(phi * cos(rho) * cos(theta));
}

inline unsigned int Vector3I::Square() const
{
	return x * x + y * y + z * z;
}

inline unsigned int Vector3I::Length() const
{
	return (unsigned int)sqrtf(Square());
}

inline void Vector3I::AddRespectively(const Vector3I& v, float tx, float ty, float tz)
{
	x *= (int)((v.x - x) * Round(tx));
	y *= (int)((v.y - y) * Round(ty));
	z *= (int)((v.z - z) * Round(tz));
}

}  // namespace


#endif