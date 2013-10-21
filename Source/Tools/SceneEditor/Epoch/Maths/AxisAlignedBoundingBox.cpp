// File: AxisAlignedBoundingBox.cpp
// Desc:

#include"AxisAlignedBoundingBox.h"

namespace Maths
{

AxisAlignedBoundingBox::AxisAlignedBoundingBox(const Vector3F& vMin, const Vector3F& vMax, bool bCheckOverride): m_vMin(vMin), m_vMax(vMax)
{
	if(bCheckOverride)
	{
		SetMin(vMin, vMax, m_vMin);
		SetMax(vMin, vMax, m_vMax);
	}
}

AxisAlignedBoundingBox& AxisAlignedBoundingBox::operator = (const AxisAlignedBoundingBox& aabb)
{
	if(&aabb != this)
	{
		m_vMin = aabb.m_vMin;
		m_vMax = aabb.m_vMax;
	}

	return *this;
}

bool AxisAlignedBoundingBox::IfOverlap(const AxisAlignedBoundingBox& aabb) const
{
	if(aabb.m_vMin.x > m_vMax.x || aabb.m_vMin.y > m_vMax.y || aabb.m_vMin.z > m_vMax.z)
	{
		return false;
	}

	if(aabb.m_vMax.x < m_vMin.x || aabb.m_vMax.y < m_vMin.y || aabb.m_vMax.z < m_vMin.z)
	{
		return false;
	}

	return true;
}

RelativeLocation AxisAlignedBoundingBox::TestAnother(const AxisAlignedBoundingBox& aabb) const
{
	if(aabb.m_vMin.x > m_vMax.x || aabb.m_vMin.y > m_vMax.y || aabb.m_vMin.z > m_vMax.z)
	{
		return RL_BACK;
	}

	if(aabb.m_vMax.x < m_vMin.x || aabb.m_vMax.y < m_vMin.y || aabb.m_vMax.z < m_vMin.z)
	{
		return RL_BACK;
	}

	if((m_vMin <= aabb.m_vMin) && (aabb.m_vMax <= m_vMax))
	{
		return RL_FRONT;
	}

	return RL_SPLIT;
}

void AxisAlignedBoundingBox::GetClosestPoint(const Vector3F& v, Vector3F& vcp) const
{
	vcp.x = (v.x < m_vMin.x)? m_vMin.x: (v.x > m_vMax.x)? m_vMax.x: v.x;
	vcp.y = (v.y < m_vMin.y)? m_vMin.y: (v.y > m_vMax.y)? m_vMax.y: v.y;
	vcp.x = (v.z < m_vMin.z)? m_vMin.z: (v.z > m_vMax.z)? m_vMax.z: v.z;
}

void AxisAlignedBoundingBox::GetCorners(vector<Vector3F>& vCorners) const
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

void AxisAlignedBoundingBox::Intersect(const AxisAlignedBoundingBox& aabb)
{
	Vector3F vMin = m_vMin, vMax = m_vMax;
	SetMin(vMin, aabb.m_vMin, m_vMin);
	SetMax(vMax, aabb.m_vMax, m_vMax);
}

bool AxisAlignedBoundingBox::CollideLine(const Vector3F& vStart, const Vector3F& vEnd, float& ftx, float& fty, float& ftz) const
{
	float st, et;
	float fst = 0, fet = 1;
	const float* bmin = &m_vMin.x;
	const float* bmax = &m_vMax.x;
	const float* si = &vStart.x;
	const float* ei = &vEnd.x;
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

bool AxisAlignedBoundingBox::CollideLine(const Vector3F& vStart, const Vector3F& vEnd) const
{
	float tx, ty, tz;
	return CollideLine(vStart, vEnd, tx, ty, tz);
}

}  // namespace