// File: AxisAlignedBoundingBox.h
// Desc: Defines AABB(Axis Aligned Bounding Box) for collision and visibility determination
//       AABB的意思是平行于3个轴，因此绝对不能对AABB做旋转操作，只可以做平移和缩放

#ifndef AXISALIGNEDBOUNDINGBOX_H
#define AXISALIGNEDBOUNDINGBOX_H

#include"Vector3F.h"
#include"Matrix.h"

#include<vector>
using std::vector;

namespace Maths
{

class DLLAPI AxisAlignedBoundingBox
{
public:
	Vector3F m_vMin;
	Vector3F m_vMax;

public:
	AxisAlignedBoundingBox(): m_vMin(), m_vMax() {}
	AxisAlignedBoundingBox(const Vector3F& vMin, const Vector3F& vMax, bool bCheckOverride = false);
	AxisAlignedBoundingBox(float fMinX, float fMinY, float fMinZ, float fMaxX, float fMaxY, float fMaxZ): m_vMin(fMinX, fMinY, fMinZ), m_vMax(fMaxX, fMaxY, fMaxZ) {}
	AxisAlignedBoundingBox(const AxisAlignedBoundingBox& aabb): m_vMin(aabb.m_vMin), m_vMax(aabb.m_vMax) {}
	AxisAlignedBoundingBox& operator = (const AxisAlignedBoundingBox& aabb);

	bool IfContain(const Vector3F& v) const { return v >= m_vMin && v <= m_vMax; }
	bool IfOverlap(const AxisAlignedBoundingBox& aabb) const;
	bool IfContain(const AxisAlignedBoundingBox& aabb) const { return (m_vMin <= aabb.m_vMin) && (aabb.m_vMax <= m_vMax); }
	RelativeLocation TestAnother(const AxisAlignedBoundingBox& aabb) const;

	bool IsValidBox() const { return m_vMin < m_vMax; }
	float GetXLength() const { return m_vMax.x - m_vMin.x; }
	float GetYLength() const { return m_vMax.y - m_vMin.y; }
	float GetZLength() const { return m_vMax.z - m_vMin.z; }
	void GetCenter(Vector3F& vCenter) const { vCenter = (m_vMin + m_vMax) * 0.5f; }
	void GetClosestPoint(const Vector3F& v, Vector3F& vcp) const;
	void GetCorners(vector<Vector3F>& vCorners) const;

	void Intersect(const AxisAlignedBoundingBox& Box);
	bool CollideLine(const Vector3F& vStart, const Vector3F& vEnd, float& ftx, float& fty, float& ftz) const;  // 碰撞检测是精确的，但碰撞点的计算却是近似的
	bool CollideLine(const Vector3F& vStart, const Vector3F& vEnd) const;
	bool CollideOrientedBox(const AxisAlignedBoundingBox& aabb, const Matrix& mtx) const { return IfOverlap(AxisAlignedBoundingBox(aabb.m_vMin * mtx, aabb.m_vMax * mtx)); }
};

}  // namespace

#endif