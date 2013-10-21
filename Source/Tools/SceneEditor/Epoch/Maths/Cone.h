// File: Cone.h
// Desc: Բ׶

#ifndef CONE_H
#define CONE_H

#include"Vector3F.h"
#include"Sphere.h"

namespace Maths
{

class DLLAPI Cone
{
public:
	Vector3F m_Location;
	Vector3F m_Direction;

	float m_AngleSinSqr, m_AngleSinReciprocal;
	float m_AngleCosSqr;

public:
	Cone(): m_Location(0.f, 0.f, 0.f), m_Direction(0.f, 0.f, 1.f), m_AngleSinSqr(0.f), m_AngleSinReciprocal(0.f), m_AngleCosSqr(0.f) {}
	Cone(const Vector3F& location, const Vector3F& direction, float angle) { Init(location, direction, angle); }
	Cone(const Cone& c): m_Location(c.m_Location), m_Direction(c.m_Direction), m_AngleSinSqr(c.m_AngleSinSqr), m_AngleSinReciprocal(c.m_AngleSinReciprocal), m_AngleCosSqr(c.m_AngleCosSqr) {}
	Cone& operator = (const Cone& c);

	void Init(const Vector3F& location, const Vector3F& direction, float angle);
	bool ConeIntersectSphere(const Sphere& s) const;
};

}  // namespace

#endif