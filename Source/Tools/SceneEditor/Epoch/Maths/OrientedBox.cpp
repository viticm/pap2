// File: OrientedBox.cpp
// Desc:

#include"OrientedBox.h"

namespace Maths
{

OrientedBox::OrientedBox(const Vector3F& vMin, const Vector3F& vMax, bool bCheckOverride): m_vMin(vMin), m_vMax(vMax)
{
	if(bCheckOverride)
	{
		SetMin(vMin, vMax, m_vMin);
		SetMax(vMin, vMax, m_vMax);
	}
}

OrientedBox& OrientedBox::operator = (const OrientedBox& ob)
{
	if(&ob != this)
	{
		m_vMin = ob.m_vMin;
		m_vMax = ob.m_vMax;
	}

	return *this;
}

void OrientedBox::Transform(const Matrix& mtx)
{
	vector<Vector3F> vCorners;
	GetCorners(vCorners);

	unsigned int NumOfCorners = vCorners.size();
	unsigned int i = 0;

	for(; i < NumOfCorners; ++i)
	{
		vCorners[i] = vCorners[i] * mtx;
	}

	m_vMin = vCorners[0];
	m_vMax = vCorners[0];
	for(i = 1; i < NumOfCorners; ++i)
	{
		Vector3F temp = vCorners[i];

		if(temp.x < m_vMin.x)
		{
			m_vMin.x = temp.x;
		}
		if(temp.y < m_vMin.y)
		{
			m_vMin.y = temp.y;
		}
		if(temp.z < m_vMin.z)
		{
			m_vMin.z = temp.z;
		}

		if(temp.x > m_vMax.x)
		{
			m_vMax.x = temp.x;
		}
		if(temp.y > m_vMax.y)
		{
			m_vMax.y = temp.y;
		}
		if(temp.z > m_vMax.z)
		{
			m_vMax.z = temp.z;
		}
	}
}

bool OrientedBox::IfContain(const Vector3F& v, const Matrix& mtx) const
{
	Vector3F temp = v * mtx;

	return temp >= m_vMin && temp <= m_vMax;
}

bool OrientedBox::IfOverlap(const OrientedBox& ob, const Matrix& mtx) const
{
	OrientedBox temp = ob;
	temp.Transform(mtx);

	if(temp.m_vMin.x > m_vMax.x || temp.m_vMin.y > m_vMax.y || temp.m_vMin.z > m_vMax.z)
	{
		return false;
	}

	if(temp.m_vMax.x < m_vMin.x || temp.m_vMax.y < m_vMin.y || temp.m_vMax.z < m_vMin.z)
	{
		return false;
	}

	return true;
}

bool OrientedBox::IfContain(const OrientedBox& ob, const Matrix& mtx) const
{
	OrientedBox temp = ob;
	temp.Transform(mtx);

	return (m_vMin <= temp.m_vMin) && (temp.m_vMax <= m_vMax);
}

RelativeLocation OrientedBox::TestAnother(const OrientedBox& ob, const Matrix& mtx) const
{
	OrientedBox temp = ob;
	temp.Transform(mtx);

	if(temp.m_vMin.x > m_vMax.x || temp.m_vMin.y > m_vMax.y || temp.m_vMin.z > m_vMax.z)
	{
		return RL_BACK;
	}

	if(temp.m_vMax.x < m_vMin.x || temp.m_vMax.y < m_vMin.y || temp.m_vMax.z < m_vMin.z)
	{
		return RL_BACK;
	}

	if((m_vMin <= temp.m_vMin) && (temp.m_vMax <= m_vMax))
	{
		return RL_FRONT;
	}

	return RL_SPLIT;
}

void OrientedBox::GetClosestPoint(const Vector3F& v, Vector3F& vcp, const Matrix& mtx) const
{
	Vector3F temp = v * mtx;

	vcp.x = (temp.x < m_vMin.x)? m_vMin.x: (temp.x > m_vMax.x)? m_vMax.x: v.x;
	vcp.y = (temp.y < m_vMin.y)? m_vMin.y: (temp.y > m_vMax.y)? m_vMax.y: v.y;
	vcp.x = (temp.z < m_vMin.z)? m_vMin.z: (temp.z > m_vMax.z)? m_vMax.z: v.z;
}

void OrientedBox::GetCorners(vector<Vector3F>& vCorners) const
{
	vCorners.clear();

	vCorners.push_back(m_vMin);
	vCorners.push_back(Vector3F(m_vMin.x, m_vMax.y, m_vMin.z));
	vCorners.push_back(Vector3F(m_vMax.x, m_vMax.y, m_vMin.z));
	vCorners.push_back(Vector3F(m_vMax.x, m_vMin.y, m_vMin.z));

	vCorners.push_back(m_vMax);
	vCorners.push_back(Vector3F(m_vMax.x, m_vMin.y, m_vMax.z));
	vCorners.push_back(Vector3F(m_vMin.x, m_vMin.y, m_vMax.z));
	vCorners.push_back(Vector3F(m_vMin.x, m_vMax.y, m_vMax.z));
}

void OrientedBox::Intersect(const OrientedBox& ob, const Matrix& mtx)
{
	OrientedBox temp;
	temp.Transform(mtx);

	Vector3F vMin = m_vMin, vMax = m_vMax;
	SetMin(vMin, temp.m_vMin, m_vMin);
	SetMax(vMax, temp.m_vMax, m_vMax);
}

bool OrientedBox::CollideLine(const Vector3F& vStart, const Vector3F& vEnd, float& ftx, float& fty, float& ftz, const Matrix& mtx) const
{
	float st, et;
	float fst = 0, fet = 1;
	const float* bmin = &m_vMin.x;
	const float* bmax = &m_vMax.x;

	Vector3F start = vStart * mtx;
	Vector3F end = vEnd * mtx;
	const float* si = &start.x;
	const float* ei = &end.x;

	static float t[3];

	for(int i = 0; i < 3; ++i)
	{
		float di = ei[i] - si[i];

		if(si[i] < ei[i])
		{
			if(si[i] > bmax[i] || ei[i] < bmin[i])
			{
				return false;
			}
			
			st = (si[i] < bmin[i])? (bmin[i] - si[i]) / di: 0;  // >0
			et = (ei[i] > bmax[i])? (bmax[i] - si[i]) / di: 1;  // >0
		}
		else
		{
			if(ei[i] > bmax[i] || si[i] < bmin[i])
			{
				return false;
			}

			st = (si[i] > bmax[i])? (bmax[i] - si[i]) / di: 0;  // >0
			et = (ei[i] < bmin[i])? (bmin[i] - si[i]) / di: 1;  // >0
		}

		t[i] = st;

		if(fst < st)
		{
			fst = st;
		}
		if(fet > et)
		{
			fet = et;
		}

		if(fet < fst)
		{
			return false;
		}
	}

	ftx = t[0];
	fty = t[1];
	ftz = t[2];

	return true;
}

bool OrientedBox::CollideLine(const Vector3F& vStart, const Vector3F& vEnd, const Matrix& mtx) const
{
	float tx, ty, tz;
	return CollideLine(vStart, vEnd, tx, ty, tz, mtx);
}

}  // namespace