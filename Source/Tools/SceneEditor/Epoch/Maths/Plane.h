// File: Plane.h
// Desc:

#ifndef PLANE_H
#define PLANE_H

#include"Vector3F.h"
#include"Polygon.h"
#include"Sphere.h"
#include"LineSeg.h"
#include"AxisAlignedBoundingBox.h"

#include<vector>
using std::vector;

namespace Maths
{

class DLLAPI Plane
{
public:
	Vector3F m_vNormal;  // 平面的法向量，上文“前”、“后”的说法以此为参照
	float m_fDistanceToOrigin;  // 平面到坐标原点的距离

public:
	Plane(): m_vNormal(1.f, 0.f, 0.f), m_fDistanceToOrigin(0.f) {}
	Plane(float fx, float fy, float fz, float fd, bool bNormalized = true);
	Plane(const Vector3F& vNormal, float fDistanceToOrigin): m_vNormal(vNormal), m_fDistanceToOrigin(fDistanceToOrigin) {}
	Plane(const Plane& p): m_vNormal(p.m_vNormal), m_fDistanceToOrigin(p.m_fDistanceToOrigin) {}
	Plane& operator = (const Plane& p);
	~Plane() {}

	// 另外一些实用的构造函数：
	Plane(const Vector3F& a, const Vector3F& b, const Vector3F& c);  // 3个(不共线的)空间点 确定一个平面；注意传入参数的顺序，这决定了平面的朝向
	Plane(const Vector3F& vNormal, const Vector3F& vPoint);  // 一个法向量和法向量外一点 确定一个平面
	Plane(const Polygon<Vector3F>& polygon);  // 一个Polygon 确定一个平面

	// 定位函数：
	RelativeLocation TestPoint(const Vector3F& mvPoint) const;  // 点定位
	RelativeLocation TestPolygon(const Polygon<Vector3F>& mpPolygon) const;  // 点列(多边形)定位
	RelativeLocation TestPointList(const Vector3F* pmvPointList, int iNum) const;  // 点列定位
	RelativeLocation TestSphere(const Sphere& msSphere) const;  // 球定位
	RelativeLocation TestSphere(const Vector3F& Location, float SquareOfRadius) const;  // 球定位
	RelativeLocation TestAABB(const AxisAlignedBoundingBox& aabb) const;  // AABB定位
	RelativeLocation TestAABB(const vector<Vector3F>& vCorners) const;  // AABB定位

	Vector3F SplitLine(const Vector3F& a, const Vector3F& b) const;  // 求线段ab与平面的交点
	Vector3F ProjectionPoint(const Vector3F& v) const;  // 求空间任一点在平面上的投影点
	float DistanceToPoint(const Vector3F& vPoint) const;  // 求平面到空间一点的距离

	bool Clip(const Polygon<Vector3F>& polyIn, Polygon<Vector3F>* ppolyOut) const;  // 裁减可视三角形

	// 用于创建BSP
	bool Split(const Polygon<Vector3F>& mpIn, Polygon<Vector3F>* pmpFront, Polygon<Vector3F>* pmpBack) const;
	bool Split(const LineSeg& mlsIn, LineSeg* pmlsFront, LineSeg* pmlsBack) const;
	bool Split(const vector<Vector3F>& vIn, vector<Vector3F>& vFront, vector<Vector3F>& vBack) const;

	void Flip();  // 翻转此平面
};

// 全局函数：

inline bool operator == (const Plane& a, const Plane& b)
{
	if(FastFabs(a.m_fDistanceToOrigin - b.m_fDistanceToOrigin) < EPSILON)
	{
		if(a.m_vNormal == b.m_vNormal)
		{
			return true;
		}
	}

	return false;
}

inline bool operator != (const Plane& a, const Plane& b)
{
	if(FastFabs(a.m_fDistanceToOrigin - b.m_fDistanceToOrigin) > EPSILON || a.m_vNormal != b.m_vNormal)
	{
		return true;
	}

	return false;
}

// 成员函数：

inline Plane& Plane::operator = (const Plane& mp)
{
	if(&mp != this)
	{
		m_vNormal = mp.m_vNormal;
		m_fDistanceToOrigin = mp.m_fDistanceToOrigin;
	}
	
	return *this;
}

inline Plane::Plane(const Vector3F& a, const Vector3F& b, const Vector3F& c)
{
	m_vNormal = (b - a) ^ (c - a);
	m_vNormal.Normalize();

	m_fDistanceToOrigin = - (m_vNormal * a);
}

inline Plane::Plane(const Vector3F& mvNormal, const Vector3F& mvPoint): m_vNormal(mvNormal), m_fDistanceToOrigin(-(m_vNormal * mvPoint))
{
}

inline Plane::Plane(const Polygon<Vector3F>& mpPolygon)
{
	Vector3F a = mpPolygon.m_vList[0];
	Vector3F b = mpPolygon.m_vList[1];
	Vector3F c = mpPolygon.m_vList[2];

	m_vNormal = (b - a) ^ (c - a);
	m_vNormal.Normalize();

	m_fDistanceToOrigin = - (m_vNormal * a);
}

inline RelativeLocation Plane::TestPoint(const Vector3F& mvPoint) const
{
	float dp = mvPoint * m_vNormal + m_fDistanceToOrigin;

	if(dp > EPSILON)
	{
		return RL_FRONT;
	}
	else if(dp < -EPSILON)
	{
		return RL_BACK;
	}
	else
	{
		return RL_COPLANAR;
	}
}

inline Vector3F Plane::SplitLine(const Vector3F& a, const Vector3F& b) const
{
	float aDot = a * m_vNormal;
	float bDot = b * m_vNormal;

	float scale = (-m_fDistanceToOrigin - aDot) / (bDot - aDot);

	return (a + (scale * (b - a)));
}

inline Vector3F Plane::ProjectionPoint(const Vector3F& mv) const
{
	float fDistToPlane = mv * m_vNormal;

	return Vector3F(mv - (m_fDistanceToOrigin + fDistToPlane) * m_vNormal);
}

inline float Plane::DistanceToPoint(const Vector3F& mvPoint) const
{
	return FastFabs(mvPoint * m_vNormal + m_fDistanceToOrigin);
}

inline void Plane::Flip()
{
	m_vNormal = - m_vNormal;
	m_fDistanceToOrigin = - m_fDistanceToOrigin;
}

}

#endif