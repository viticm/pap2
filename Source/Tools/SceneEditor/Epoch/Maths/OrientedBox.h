// File: OrientedBox.h
// Desc: OrientedBox is defined in local space. DON'T ever transform it!

#ifndef ORIENTEDBOX_H
#define ORIENTEDBOX_H

#include"Vector3F.h"
#include"Matrix.h"

#include<vector>
using std::vector;

namespace Maths
{

class DLLAPI OrientedBox
{
public:
	Vector3F m_vMin;
	Vector3F m_vMax;

public:
	OrientedBox(): m_vMin(), m_vMax() {}
	OrientedBox(const Vector3F& vMin, const Vector3F& vMax, bool bCheckOverride = false);
	OrientedBox(float fMinX, float fMinY, float fMinZ, float fMaxX, float fMaxY, float fMaxZ): m_vMin(fMinX, fMinY, fMinZ), m_vMax(fMaxX, fMaxY, fMaxZ) {}
	OrientedBox(const OrientedBox& ob): m_vMin(ob.m_vMin), m_vMax(ob.m_vMax) {}
	OrientedBox& operator = (const OrientedBox& ob);

	void Transform(const Matrix& mtx);

	bool IfContain(const Vector3F& v, const Matrix& mtx) const;
	bool IfOverlap(const OrientedBox& ob, const Matrix& mtx) const;
	bool IfContain(const OrientedBox& ob, const Matrix& mtx) const;
	RelativeLocation TestAnother(const OrientedBox& ob, const Matrix& mtx) const;

	bool IsValidBox() const { return m_vMin < m_vMax; }
	float GetXLength() const { return m_vMax.x - m_vMin.x; }
	float GetYLength() const { return m_vMax.y - m_vMin.y; }
	float GetZLength() const { return m_vMax.z - m_vMin.z; }
	void GetCenter(Vector3F& vCenter) const { vCenter = (m_vMin + m_vMax) * 0.5f; }
	void GetClosestPoint(const Vector3F& v, Vector3F& vcp, const Matrix& mtx) const;
	void GetCorners(vector<Vector3F>& vCorners) const;

	void Intersect(const OrientedBox& oB, const Matrix& mtx);
	bool CollideLine(const Vector3F& vStart, const Vector3F& vEnd, float& ftx, float& fty, float& ftz, const Matrix& mtx) const;  // 碰撞检测是精确的，但碰撞点的计算却是近似的
	bool CollideLine(const Vector3F& vStart, const Vector3F& vEnd, const Matrix& mtx) const;
};

}  // namespace

#endif