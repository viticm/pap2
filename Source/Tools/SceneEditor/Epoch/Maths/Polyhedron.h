// File: Polyhedron.h
// Desc: (凸)多面体，用于碰撞监测、可视判断，各面的法向量均指向多面体内部
//       更类似于视锥的概念

#ifndef POLYHEDRON_H
#define POLYHEDRON_H

#include<vector>
using std::vector;

#include"Plane.h"
#include"Sphere.h"
#include"OrientedBox.h"
#include"AxisAlignedBoundingBox.h"

namespace Maths
{

class DLLAPI Polyhedron
{
public:
	vector< Plane > m_vPlanes;

public:
	Polyhedron(int iNumOfPlanes) { m_vPlanes.resize(iNumOfPlanes); }
	Polyhedron(const Polyhedron& phn): m_vPlanes(phn.m_vPlanes) {}
	Polyhedron& operator = (const Polyhedron& phn);
	~Polyhedron() { m_vPlanes.clear(); }

	void CreatePlanes(const vector<Vector3F>& vNearPlanePoints, const vector<Vector3F>& vFarPlanePoints);
	bool IsPointInside(const Vector3F& point) const;

	bool IfContain(const Vector3F& point) const { return IsPointInside(point); }
	RelativeLocation TestPoint(const Vector3F& point) const;

	bool IfContain(const Sphere& sphere) const;
	RelativeLocation TestSphere(const Sphere& sphere) const;
	RelativeLocation TestSphere(const Vector3F& Location, float SquareOfRadius) const;

	bool IfContain(const AxisAlignedBoundingBox& aabb) const;
	RelativeLocation TestAxisAlignedBoundingBox(const AxisAlignedBoundingBox& aabb) const;

	bool IfContain(const OrientedBox& ob, const Matrix& mtx) const;  // Although the wanted parameter is of OrientedBox, the algorithm treats it as an aabb. Cause OrientedBox is defined in local space, there's one more parameter, the Matrix pamameter, required to transform the OrientedBox's 8 vertices into the world space.
	RelativeLocation TestOrientedBox(const OrientedBox& ob, const Matrix& mtx) const;
};

}  // namespace

#endif