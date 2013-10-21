// File: ViewFrustum.h
// Desc: 真正的View frustum，只含6个裁减面(从矩阵中抽取)

#ifndef VIEWFRUSTUM_H
#define VIEWFRUSTUM_H

#include"Polyhedron.h"
#include"Matrix.h"

namespace Maths
{

class DLLAPI ViewFrustum: public Polyhedron
{
public:
	Matrix m_mtxCached;

public:
	ViewFrustum(): Polyhedron(6) { m_mtxCached.Identilize(); }
	ViewFrustum(const ViewFrustum& vf): Polyhedron(vf), m_mtxCached(vf.m_mtxCached) {}
	ViewFrustum& operator = (const ViewFrustum& vf);

	void SetPlanes(const Matrix& mtx);  // Extract the clipping planes from matrix which may be the projection matrix (in this case the clipping planes are located in view space), the combination of view matrix and projection matrix (in this case the clipping planes are located in world space), the combination of world matrix and view matrix and projection matrix (in this case the clipping planes are located in local space)
};

}  // namespace

#endif