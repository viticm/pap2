// File: Cone.cpp
// Desc:

#include"Cone.h"

namespace Maths
{

Cone& Cone::operator = (const Cone& c)
{
	if(&c != this)
	{
		m_Location = c.m_Location;
		m_Direction = c.m_Direction;
		m_AngleSinSqr = c.m_AngleSinSqr;
		m_AngleSinReciprocal = c.m_AngleSinReciprocal;
		m_AngleCosSqr = c.m_AngleCosSqr;
	}

	return *this;
}

void Cone::Init(const Vector3F& location, const Vector3F& direction, float angle)
{
	m_Location = location;
	m_Direction = direction;
	
	float s = sinf(angle);
	float c = cosf(angle);
	
	m_AngleSinSqr = s * s;
	m_AngleSinReciprocal = 1.f / s;
	m_AngleCosSqr = c * c;
}

bool Cone::ConeIntersectSphere(const Sphere& s) const
{
	Vector3F U = m_Location - (s.m_Radius * m_AngleSinReciprocal) * m_Direction;
	Vector3F D = s.m_Location - U;
	float dsqr = D * D;
	float e = m_Direction * D;
	if(e > 0 && e * e >= dsqr * m_AngleCosSqr)
	{
		D = s.m_Location - m_Location;
		dsqr = D * D;
		e = -m_Direction * D;
		if(e > 0 && e * e >= dsqr * m_AngleSinSqr)
		{
			return dsqr <= s.m_Radius * s.m_Radius;
		}
		else
		{
			return true;
		}
	}

	return false;
}

}  // namespace