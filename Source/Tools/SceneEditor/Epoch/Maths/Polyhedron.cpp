// File: Polyhedron.cpp
// Desc:

#include"Polyhedron.h"
#include"..\Common\Exception.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


namespace Maths
{

Polyhedron& Polyhedron::operator = (const Polyhedron& phn)
{
	if(&phn != this)
	{
		m_vPlanes = phn.m_vPlanes;
	}

	return *this;
}

void Polyhedron::CreatePlanes(const vector<Vector3F>& vNearPlanePoints, const vector<Vector3F>& vFarPlanePoints)
{
	unsigned int iNumNearPlanePoints = vNearPlanePoints.size();
	unsigned int iNumFarPlanePoints = vFarPlanePoints.size();
	unsigned int iNumPlanes = m_vPlanes.size();
	if(iNumNearPlanePoints != iNumFarPlanePoints || iNumNearPlanePoints + 2 != iNumPlanes || iNumNearPlanePoints < 3)
	{
		throw Common::Exception(Common::EL_BAD, "Polyheadron::CreatePlanes()", "Input parameters are illegal! Please check out your program.");
	}

	// Create a point array which will be used for creating planes...
	static vector<Vector3F> vPoints;
	vPoints.resize(3 * iNumPlanes);  // 3 points per plane

	// Near plane:
	vPoints[0] = vNearPlanePoints[0];
	vPoints[1] = vNearPlanePoints[2];
	vPoints[2] = vNearPlanePoints[1];
	// Far plane:
	vPoints[3] = vFarPlanePoints[0];
	vPoints[4] = vFarPlanePoints[1];
	vPoints[5] = vFarPlanePoints[2];

	unsigned int vpi = 6, vpn = 0, vpf = 0, j = 0;
	for(; j < iNumPlanes - 2; ++j)
	{
		vPoints[vpi++] = vNearPlanePoints[vpn++];
		vpn = (vpn == iNumNearPlanePoints)? 0: vpn;
		vPoints[vpi++] = vNearPlanePoints[vpn];
		vPoints[vpi++] = vFarPlanePoints[vpf++];
	}

	// Create our planes
	for(j = 0; j < iNumPlanes; ++j)
	{
		m_vPlanes[j] = Plane(vPoints[j * 3], vPoints[j * 3 + 1], vPoints[j * 3 + 2]);
	}
}

bool Polyhedron::IsPointInside(const Vector3F& point) const
{
	unsigned int NumOfPlanes = m_vPlanes.size();
	for(unsigned int i = 0; i < NumOfPlanes; ++i)
	{
		if(m_vPlanes[i].TestPoint(point) != RL_FRONT)
		{
			return false;
		}
	}

	return true;
}

RelativeLocation Polyhedron::TestPoint(const Vector3F& point) const
{
	unsigned int NumOfPlanes = m_vPlanes.size();
	for(unsigned int i = 0; i < NumOfPlanes; ++i)
	{
		if(m_vPlanes[i].TestPoint(point) != RL_FRONT)
		{
			return RL_BACK;
		}
	}

	return RL_FRONT;
}

bool Polyhedron::IfContain(const Sphere& sphere) const
{
	unsigned int NumOfPlanes = m_vPlanes.size();
	for(unsigned int i = 0; i < NumOfPlanes; ++i)
	{
		RelativeLocation rl = m_vPlanes[i].TestSphere(sphere);

		if(rl == RL_BACK)
		{
			return false;
		}
		else if(rl == RL_SPLIT)
		{
			return false;
		}
	}

	return true;
}

RelativeLocation Polyhedron::TestSphere(const Sphere& sphere) const
{
	unsigned int NumOfPlanes = m_vPlanes.size();
	for(unsigned int i = 0; i < NumOfPlanes; ++i)
	{
		RelativeLocation rl = m_vPlanes[i].TestSphere(sphere);

		if(rl == RL_BACK)
		{
			return RL_BACK;
		}
		else if(rl == RL_SPLIT)
		{
			return RL_SPLIT;
		}
	}

	return RL_FRONT;
}

RelativeLocation Polyhedron::TestSphere(const Vector3F& Location, float SquareOfRadius) const
{
	unsigned int NumOfPlanes = m_vPlanes.size();
	for(unsigned int i = 0; i < NumOfPlanes; ++i)
	{
		RelativeLocation rl = m_vPlanes[i].TestSphere(Location, SquareOfRadius);

		if(rl == RL_BACK)
		{
			return RL_BACK;
		}
		else if(rl == RL_SPLIT)
		{
			return RL_SPLIT;
		}
	}

	return RL_FRONT;
}

bool Polyhedron::IfContain(const AxisAlignedBoundingBox& aabb) const
{
	unsigned int iTotalIn = 0;

	vector<Vector3F> vCorners;
	aabb.GetCorners(vCorners);

	// test all 8 corners against the sides
	// if all points are behind 1 specific plane, we are out
	// if we are in with all points, then we are fully in
	unsigned int NumOfPlanes = m_vPlanes.size();
	for(unsigned int p = 0; p < NumOfPlanes; ++p)
	{
		unsigned int iInCount = 8;
		unsigned int iPtIn = 1;

		for(unsigned int i = 0; i < 8; ++i)
		{
			// test this point against the planes
			if(m_vPlanes[p].TestPoint(vCorners[i]) == RL_BACK)
			{
				iPtIn = 0;
				--iInCount;
			}
		}

		// were all the points outside of plane p?
		if(iInCount == 0)
		{
			return false;
		}

		// check if they were all on the right side of the plane
		iTotalIn += iPtIn;
	}

	// so if iTotalIn is equal to the NumOfPlanes, then all are inside the view
	if(iTotalIn == NumOfPlanes)
	{
		return true;
	}

	// we must be partly in then otherwise
	return false;
}

RelativeLocation Polyhedron::TestAxisAlignedBoundingBox(const AxisAlignedBoundingBox& aabb) const
{
	unsigned int iTotalIn = 0;

	vector<Vector3F> vCorners;
	aabb.GetCorners(vCorners);

	// test all 8 corners against the 6 sides
	// if all points are behind 1 specific plane, we are out
	// if we are in with all points, then we are fully in
	unsigned int NumOfPlanes = m_vPlanes.size();
	for(unsigned int p = 0; p < NumOfPlanes; ++p)
	{
		unsigned int iInCount = 8;
		unsigned int iPtIn = 1;
		Plane ptemp = m_vPlanes[p];

		for(unsigned int i = 0; i < 8; ++i)
		{
			// test this point against the planes
			if(m_vPlanes[p].TestPoint(vCorners[i]) == RL_BACK)
			{
				iPtIn = 0;
				--iInCount;
			}
		}

		// were all the points outside of plane p?
		if(iInCount == 0)
		{
			return RL_BACK;
		}

		// check if they were all on the right side of the plane
		iTotalIn += iPtIn;
	}

	// so if iTotalIn is 6, then all are inside the view
	if(iTotalIn == NumOfPlanes)
	{
		return RL_FRONT;
	}

	// BACK or SPLIT ?
	/*
	bool bBack = false;
	bool bFront = false;
	for(unsigned int i = 0; i < 8; ++i)  // ÒÀ´Î²âÊÔ
	{
		Vector3F corner = vCorners[i];

		RelativeLocation rl = TestPoint(corner);
		switch(rl)
		{
		case RL_BACK:
			bBack = true;
			if(bFront)
			{
				return RL_SPLIT;
			}
			break;

		case RL_FRONT:
			bFront = true;
			if(bBack)
			{
				return RL_SPLIT;
			}
			break;
		}
	}

	return bBack? RL_BACK
	*/

	// we must be partly in then otherwise; no, not very sure
	return RL_SPLIT;
}

bool Polyhedron::IfContain(const OrientedBox& ob, const Matrix& mtx) const
{
	unsigned int iTotalIn = 0;

	vector<Vector3F> vCorners;
	ob.GetCorners(vCorners);  // Get the original 8 vertices of the OrientedBox
	for(unsigned int i = 0; i < 8; ++i)
	{
		vCorners[i] = vCorners[i] * mtx;
	}

	// test all 8 corners against the sides
	// if all points are behind 1 specific plane, we are out
	// if we are in with all points, then we are fully in
	unsigned int NumOfPlanes = m_vPlanes.size();
	for(unsigned int p = 0; p < NumOfPlanes; ++p)
	{
		unsigned int iInCount = 8;
		unsigned int iPtIn = 1;

		for(unsigned int i = 0; i < 8; ++i)
		{
			// test this point against the planes
			if(m_vPlanes[p].TestPoint(vCorners[i]) == RL_BACK)
			{
				iPtIn = 0;
				--iInCount;
			}
		}

		// were all the points outside of plane p?
		if(iInCount == 0)
		{
			return false;
		}

		// check if they were all on the right side of the plane
		iTotalIn += iPtIn;
	}

	// so if iTotalIn is equal to the NumOfPlanes, then all are inside the view
	if(iTotalIn == NumOfPlanes)
	{
		return true;
	}

	// we must be partly in then otherwise
	return false;
}

RelativeLocation Polyhedron::TestOrientedBox(const OrientedBox& ob, const Matrix& mtx) const
{
	unsigned int iTotalIn = 0;

	vector<Vector3F> vCorners;
	ob.GetCorners(vCorners);
	for(unsigned int i = 0; i < 8; ++i)
	{
		vCorners[i] = vCorners[i] * mtx;
	}

	// test all 8 corners against the 6 sides
	// if all points are behind 1 specific plane, we are out
	// if we are in with all points, then we are fully in
	unsigned int NumOfPlanes = m_vPlanes.size();
	for(unsigned int p = 0; p < NumOfPlanes; ++p)
	{
		unsigned int iInCount = 8;
		unsigned int iPtIn = 1;

		for(unsigned int i = 0; i < 8; ++i)
		{
			// test this point against the planes
			if(m_vPlanes[p].TestPoint(vCorners[i]) == RL_BACK)
			{
				iPtIn = 0;
				--iInCount;
			}
		}

		// were all the points outside of plane p?
		if(iInCount == 0)
		{
			return RL_BACK;
		}

		// check if they were all on the right side of the plane
		iTotalIn += iPtIn;
	}

	// so if iTotalIn is 6, then all are inside the view
	if(iTotalIn == NumOfPlanes)
	{
		return RL_FRONT;
	}

	// we must be partly in then otherwise
	return RL_SPLIT;
}

}  // namespace