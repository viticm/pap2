// File: Sphere.h
// Desc:

#ifndef SPHERE_H
#define SPHERE_H

#include"Vector3F.h"

namespace Maths
{

class DLLAPI Sphere
{
public:
	Vector3F m_Location;
	float m_Radius;

public:
	Sphere(): m_Location(Vector3F(0.f, 0.f, 0.f)), m_Radius(0) {}
	Sphere(const Vector3F& Location, float Radius): m_Location(Location), m_Radius(Radius) {}
	Sphere(const Vector3F& Location, const Vector3F* pVList, int NumOfVector3Fs);
	Sphere(const Sphere& ms): m_Location(ms.m_Location), m_Radius(ms.m_Radius) {}
	Sphere& operator = (const Sphere& ms);
	~Sphere() {}

	Sphere& operator += (const Vector3F& mv);
	Sphere& operator -= (const Vector3F& mv);

	float DistanceToPoint(const Vector3F& mvPoint) const;  // 空间一点到球面的距离
	bool IfContain(const Sphere& sphere) const;  // 判断是否与另一Sphere相交
	RelativeLocation TestAnother(const Sphere& sphere) const;
};

// 全局函数：

inline const Sphere operator + (const Sphere& ms, const Vector3F& mv)
{
	return Sphere(ms.m_Location + mv, ms.m_Radius);
}

inline const Sphere operator - (const Sphere& ms, const Vector3F& mv)
{
	return Sphere(ms.m_Location - mv, ms.m_Radius);
}

// 成员函数

inline Sphere& Sphere::operator = (const Sphere& ms)
{
	if(&ms != this)
	{
		m_Location = ms.m_Location;
		m_Radius = ms.m_Radius;
	}
	
	return *this;
}

inline Sphere& Sphere::operator += (const Vector3F& mv)
{
	m_Location += mv;

	return *this;
}

inline Sphere& Sphere::operator -= (const Vector3F& mv)
{
	m_Location -= mv;

	return *this;
}

inline float Sphere::DistanceToPoint(const Vector3F& mvPoint) const
{
	Vector3F mvTemp = mvPoint - m_Location;

	return mvTemp.Length() - m_Radius;
}

}

#endif