// File: ViewFrustum.cpp
// Desc:

#include"ViewFrustum.h"

namespace Maths
{

ViewFrustum& ViewFrustum::operator = (const ViewFrustum& vf)
{
	if(&vf != this)
	{
		Polyhedron::operator = (vf);

		m_mtxCached = vf.m_mtxCached;
	}

	return *this;
}

void ViewFrustum::SetPlanes(const Matrix& mtx)
{
	if(m_mtxCached == mtx)
	{
		return;
	}

	m_mtxCached = mtx;

	m_vPlanes[0] = Plane(mtx._13, mtx._23, mtx._33, mtx._43, false);  // Near plane
	m_vPlanes[1] = Plane(mtx._14 - mtx._13, mtx._24 - mtx._23, mtx._34 - mtx._33, mtx._44 - mtx._43, false);  // Far plane
	m_vPlanes[2] = Plane(mtx._14 + mtx._11, mtx._24 + mtx._21, mtx._34 + mtx._31, mtx._44 + mtx._41, false);  // Left plane
	m_vPlanes[3] = Plane(mtx._14 - mtx._11, mtx._24 - mtx._21, mtx._34 - mtx._31, mtx._44 - mtx._41, false);  // Right plane
	m_vPlanes[4] = Plane(mtx._14 - mtx._12, mtx._24 - mtx._22, mtx._34 - mtx._32, mtx._44 - mtx._42, false);  // Top plane
	m_vPlanes[5] = Plane(mtx._14 + mtx._12, mtx._24 + mtx._22, mtx._34 + mtx._32, mtx._44 + mtx._42, false);  // Bottom plane
}

}  // namespace