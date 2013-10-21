// File: Plane.cpp
// Desc:

#include"Plane.h"

namespace Maths
{

Plane::Plane(float fx, float fy, float fz, float fd, bool bNormalized): m_vNormal(fx, fy, fz), m_fDistanceToOrigin(fd)
{
	if(!bNormalized)
	{
		float fl = m_vNormal.Length();
		if(Maths::FEqualZero(fl))
		{
			fl = 1.f;
		}

		m_fDistanceToOrigin /= fl;
		m_vNormal.x /= fl;
		m_vNormal.y /= fl;
		m_vNormal.z /= fl;
	}
}

RelativeLocation Plane::TestPolygon(const Polygon<Vector3F>& mpPolygon) const
{
	return TestPointList(mpPolygon.m_vList, mpPolygon.m_iCurrentNumVectors);
}

RelativeLocation Plane::TestPointList(const Vector3F* pmvPointList, int iNum) const
{
	bool bAllFront = true, bAllBack = true;

	RelativeLocation pl;
	for(int i = 0; i < iNum; ++i)
	{
		pl = TestPoint(pmvPointList[i]);
		if(pl == RL_BACK)
		{
			bAllFront = false;

			if(!bAllBack)
			{
				break;
			}
		}
		else if(pl == RL_FRONT)
		{
			bAllBack = false;

			if(!bAllFront)
			{
				break;
			}
		}
	}

	if(bAllFront && !bAllBack)
	{
		return RL_FRONT;
	}
	else if(!bAllFront && bAllBack)
	{
		return RL_BACK;
	}
	else if(!bAllFront && !bAllBack)
	{
		return RL_SPLIT;
	}

	return RL_COPLANAR;
}

RelativeLocation Plane::TestSphere(const Sphere& msSphere) const
{
	float dp = (msSphere.m_Location * m_vNormal) + m_fDistanceToOrigin;

	if((dp - msSphere.m_Radius) > EPSILON)
	{
		return RL_FRONT;
	}
	else if((dp + msSphere.m_Radius) < -EPSILON)
	{
		return RL_BACK;
	}

	return RL_SPLIT;
}

RelativeLocation Plane::TestSphere(const Vector3F& Location, float SquareOfRadius) const
{
	float dp = (Location * m_vNormal) + m_fDistanceToOrigin;
	dp *= dp;
	
	if((dp - SquareOfRadius) > EPSILON)
	{
		return RL_FRONT;
	}
	else if((dp + SquareOfRadius) < -EPSILON)
	{
		return RL_BACK;
	}

	return RL_SPLIT;
}

RelativeLocation Plane::TestAABB(const AxisAlignedBoundingBox& aabb) const
{
	bool bFront = false;;
	bool bBack = false;

	vector<Vector3F> vCorners;
	aabb.GetCorners(vCorners);

	unsigned int NumOfCorners = vCorners.size();
	for(unsigned int i = 0; i < NumOfCorners; ++i)
	{
		RelativeLocation rl = TestPoint(vCorners[i]);
		switch(rl)
		{
		case RL_FRONT:
			bFront = true;
			if(bBack)
			{
				return RL_SPLIT;
			}

			break;

		case RL_BACK:
			bBack = true;
			if(bFront)
			{
				return RL_SPLIT;
			}
			break;

		default:
			break;
		}
	}

	return bFront? RL_FRONT: RL_BACK;
}

RelativeLocation Plane::TestAABB(const vector<Vector3F>& vCorners) const
{
	bool bFront = false;;
	bool bBack = false;

	unsigned int NumOfCorners = vCorners.size();
	for(unsigned int i = 0; i < NumOfCorners; ++i)
	{
		RelativeLocation rl = TestPoint(vCorners[i]);
		switch(rl)
		{
		case RL_FRONT:
			bFront = true;
			if(bBack)
			{
				return RL_SPLIT;
			}

			break;

		case RL_BACK:
			bBack = true;
			if(bFront)
			{
				return RL_SPLIT;
			}

			break;

		default:
			break;
		}
	}

	return bFront? RL_FRONT: RL_BACK;
}

bool Plane::Clip(const Polygon<Vector3F>& mpIn, Polygon<Vector3F>* pmpOut) const
{
	if(!pmpOut || mpIn.m_iCurrentNumVectors <= 2)
	{
		return false;
	}

	int iThisIndex = mpIn.m_iCurrentNumVectors - 1;
	int iNextIndex = 0;

	RelativeLocation plThis = TestPoint(mpIn.m_vList[iThisIndex]);
	RelativeLocation plNext;

	pmpOut->m_iCurrentNumVectors = 0;

	for(iNextIndex = 0; iNextIndex < mpIn.m_iCurrentNumVectors; ++iNextIndex)
	{
		plNext = TestPoint(mpIn.m_vList[iNextIndex]);

		if(plThis == RL_FRONT || plThis == RL_COPLANAR)
		{
			pmpOut->m_vList[pmpOut->m_iCurrentNumVectors++] = mpIn.m_vList[iThisIndex];
		}

		if((plThis == RL_BACK && plNext == RL_FRONT) || (plThis == RL_FRONT && plNext == RL_BACK))
		{
			pmpOut->m_vList[pmpOut->m_iCurrentNumVectors++] = SplitLine(mpIn.m_vList[iThisIndex], mpIn.m_vList[iNextIndex]);
		}

		iThisIndex = iNextIndex;
		plThis = plNext;
	}

	if(pmpOut->m_iCurrentNumVectors >= 3)
	{
		return true;
	}
	return false;
}

bool Plane::Split(const Polygon<Vector3F>& mpIn, Polygon<Vector3F>* pmpFront, Polygon<Vector3F>* pmpBack) const
{
	if(!pmpFront || !pmpBack || mpIn.m_iCurrentNumVectors <= 2)
	{
		return false;
	}

	pmpFront->m_iCurrentNumVectors = 0;
	pmpBack->m_iCurrentNumVectors = 0;

	int iThisIndex = mpIn.m_iCurrentNumVectors - 1;
	int iNextIndex = 0;

	RelativeLocation plThis = TestPoint(mpIn.m_vList[iThisIndex]);
	RelativeLocation plNext;

	for(; iNextIndex < mpIn.m_iCurrentNumVectors; ++iNextIndex)
	{
		plNext = TestPoint(mpIn.m_vList[iNextIndex]);

		if(plThis == RL_FRONT)
		{
			pmpFront->m_vList[pmpFront->m_iCurrentNumVectors++] = mpIn.m_vList[iThisIndex];
		}
		else if(plThis == RL_BACK)
		{
			pmpBack->m_vList[pmpBack->m_iCurrentNumVectors++] = mpIn.m_vList[iThisIndex];
		}
		else
		{
			pmpFront->m_vList[pmpFront->m_iCurrentNumVectors++] = mpIn.m_vList[iThisIndex];
			pmpBack->m_vList[pmpBack->m_iCurrentNumVectors++] = mpIn.m_vList[iThisIndex];
		}

		if((plThis == RL_FRONT && plNext == RL_BACK) || (plThis == RL_BACK && plNext == RL_FRONT))
		{
			Vector3F mvSplit = SplitLine(mpIn.m_vList[iThisIndex], mpIn.m_vList[iNextIndex]);
			pmpFront->m_vList[pmpFront->m_iCurrentNumVectors++] = mvSplit;
			pmpBack->m_vList[pmpBack->m_iCurrentNumVectors++] = mvSplit;
		}

		iThisIndex = iNextIndex;
		plThis = plNext;
	}

	if(pmpFront->m_iCurrentNumVectors >= 3 && pmpBack->m_iCurrentNumVectors >= 3)
	{
		return true;
	}
	return false;
}

bool Plane::Split(const LineSeg& mlsIn, LineSeg* pmlsFront, LineSeg* pmlsBack) const
{
	Vector3F temp = SplitLine(mlsIn.v0, mlsIn.v1);

	if(TestPoint(mlsIn.v0) == RL_FRONT)
	{
		pmlsFront->v0 = mlsIn.v0;
		pmlsFront->v1 = temp;
		pmlsBack->v0 = temp;
		pmlsBack->v1 = mlsIn.v1;
		return true;
	}
	else
	{
		pmlsFront->v0 = temp;
		pmlsFront->v1 = mlsIn.v1;
		pmlsBack->v0 = mlsIn.v0;
		pmlsBack->v1 = temp;
		return true;
	}

	return false;
}

bool Plane::Split(const vector<Vector3F>& vIn, vector<Vector3F>& vFront, vector<Vector3F>& vBack) const
{
	if(vIn.empty())
	{
		return false;
	}

	unsigned int NumOfInVectors = vIn.size();
	if(NumOfInVectors <= 2)
	{
		return false;
	}

	vFront.clear();
	vBack.clear();

	int iThisIndex = NumOfInVectors - 1;
	int iNextIndex = 0;

	RelativeLocation plThis = TestPoint(vIn[iThisIndex]);
	RelativeLocation plNext;

	for(iNextIndex = 0; iNextIndex < NumOfInVectors; ++iNextIndex)
	{
		plNext = TestPoint(vIn[iNextIndex]);

		if(plThis == RL_FRONT)
		{
			vFront.push_back(vIn[iThisIndex]);
		}
		else if(plThis == RL_BACK)
		{
			vBack.push_back(vIn[iThisIndex]);
		}
		else
		{
			vFront.push_back(vIn[iThisIndex]);
			vBack.push_back(vIn[iThisIndex]);
		}

		if((plThis == RL_FRONT && plNext == RL_BACK) || (plThis == RL_BACK && plNext == RL_FRONT))
		{
			Vector3F vSplit = SplitLine(vIn[iThisIndex], vIn[iNextIndex]);
			vFront.push_back(vSplit);
			vBack.push_back(vSplit);
		}

		iThisIndex = iNextIndex;
		plThis = plNext;
	}

	if(vFront.size() >= 3 && vBack.size() >= 3)
	{
		return true;
	}
	return false;
}

}