// File: Sphere.cpp
// Desc:

#include"Sphere.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


namespace Maths
{

Sphere::Sphere(const Vector3F& Location, const Vector3F* pVList, int NumOfVector3Fs)
{
	m_Location = Location;
	m_Radius = 0.f;

	float fCurrRadius = 0.f;
	for(int i = 0; i < NumOfVector3Fs; ++i)
	{
		fCurrRadius = pVList[i].Length();

		if(m_Radius < fCurrRadius)
		{
			m_Radius = fCurrRadius;
		}
	}
}

bool Sphere::IfContain(const Sphere& sphere) const
{
	float SquareOfDistance = (m_Location - sphere.m_Location).Square();
	if(SquareOfDistance >= (m_Radius + sphere.m_Radius) * (m_Radius + sphere.m_Radius))
	{
		return false;
	}
	else if(SquareOfDistance <= (m_Radius - sphere.m_Radius) * (m_Radius - sphere.m_Radius))
	{
		return true;
	}

	return false;
}

RelativeLocation Sphere::TestAnother(const Sphere& sphere) const
{
	float SquareOfDistance = (m_Location - sphere.m_Location).Square();
	if(SquareOfDistance >= (m_Radius + sphere.m_Radius) * (m_Radius + sphere.m_Radius))
	{
		return RL_BACK;
	}
	else if(m_Radius > sphere.m_Radius && SquareOfDistance <= (m_Radius - sphere.m_Radius) * (m_Radius - sphere.m_Radius))
	{
		return RL_FRONT;
	}

	return RL_SPLIT;
}

}