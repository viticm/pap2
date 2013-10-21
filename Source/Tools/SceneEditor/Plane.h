// File: Plane.h
// Desc:

#ifndef PLANE_H
#define PLANE_H

#ifndef EPSILON
#define EPSILON 0.00001f  // 用于判断浮点数是否等于0
#endif

enum RelativeLocation
{
	RL_FRONT,  // 对平面来说是前，对包围球(盒)来说是内
	RL_BACK,  // 对平面来说是后，对包围球(盒)来说是外
	RL_SPLIT,  // 相交
	RL_COPLANAR  // 共面
};

#include<vector>
using std::vector;


inline bool FEqualZero(const float& f)
{
	return (f >= -EPSILON)? (f <= EPSILON): false;
}

class Plane
{
public:
	D3DXVECTOR3 m_vNormal;  // 平面的法向量，上文“前”、“后”的说法以此为参照
	float m_fDistanceToOrigin;  // 平面到坐标原点的距离

public:
	Plane(): m_vNormal(1.f, 0.f, 0.f), m_fDistanceToOrigin(0.f) {}
	Plane(float fx, float fy, float fz, float fd, bool bNormalized = true);
	Plane(const D3DXVECTOR3& vNormal, float fDistanceToOrigin): m_vNormal(vNormal), m_fDistanceToOrigin(fDistanceToOrigin) {}
	Plane(const Plane& p): m_vNormal(p.m_vNormal), m_fDistanceToOrigin(p.m_fDistanceToOrigin) {}
	Plane& operator = (const Plane& p);
	~Plane() {}

	// 另外一些实用的构造函数：
	Plane(const D3DXVECTOR3& a, const D3DXVECTOR3& b, const D3DXVECTOR3& c);  // 3个(不共线的)空间点 确定一个平面；注意传入参数的顺序，这决定了平面的朝向
	Plane(const D3DXVECTOR3& vNormal, const D3DXVECTOR3& vPoint);  // 一个法向量和法向量外一点 确定一个平面
	//Plane(const Polygon<D3DXVECTOR3>& polygon);  // 一个Polygon 确定一个平面

	// 定位函数：
	RelativeLocation TestPoint(const D3DXVECTOR3& mvPoint) const;  // 点定位
	//RelativeLocation TestPolygon(const Polygon<D3DXVECTOR3>& mpPolygon) const;  // 点列(多边形)定位
	///RelativeLocation TestPointList(const D3DXVECTOR3* pmvPointList, int iNum) const;  // 点列定位
	//RelativeLocation TestSphere(const Sphere& msSphere) const;  // 球定位
	//RelativeLocation TestSphere(const D3DXVECTOR3& Location, float SquareOfRadius) const;  // 球定位
	//RelativeLocation TestAABB(const AxisAlignedBoundingBox& aabb) const;  // AABB定位
	//RelativeLocation TestAABB(const vector<D3DXVECTOR3>& vCorners) const;  // AABB定位

	D3DXVECTOR3 SplitLine(const D3DXVECTOR3& a, const D3DXVECTOR3& b) const;  // 求线段ab与平面的交点
	D3DXVECTOR3 ProjectionPoint(const D3DXVECTOR3& v) const;  // 求空间任一点在平面上的投影点
	float DistanceToPoint(const D3DXVECTOR3& vPoint) const;  // 求平面到空间一点的距离

	//bool Clip(const Polygon<D3DXVECTOR3>& polyIn, Polygon<D3DXVECTOR3>* ppolyOut) const;  // 裁减可视三角形

	// 用于创建BSP
	//bool Split(const Polygon<D3DXVECTOR3>& mpIn, Polygon<D3DXVECTOR3>* pmpFront, Polygon<D3DXVECTOR3>* pmpBack) const;
	//bool Split(const LineSeg& mlsIn, LineSeg* pmlsFront, LineSeg* pmlsBack) const;
	bool Split(const vector<D3DXVECTOR3>& vIn, vector<D3DXVECTOR3>& vFront, vector<D3DXVECTOR3>& vBack) const;

	void Flip();  // 翻转此平面
};

// 全局函数：

inline bool operator == (const Plane& a, const Plane& b)
{
	if(fabs(a.m_fDistanceToOrigin - b.m_fDistanceToOrigin) < EPSILON)
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
	if(fabs(a.m_fDistanceToOrigin - b.m_fDistanceToOrigin) > EPSILON || a.m_vNormal != b.m_vNormal)
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

inline Plane::Plane(const D3DXVECTOR3& a, const D3DXVECTOR3& b, const D3DXVECTOR3& c)
{
	D3DXVec3Cross(&m_vNormal, &(b - a), &(c - a));
	D3DXVec3Normalize(&m_vNormal, &m_vNormal);

	m_fDistanceToOrigin = - D3DXVec3Dot(&m_vNormal, &a);
}

inline Plane::Plane(const D3DXVECTOR3& mvNormal, const D3DXVECTOR3& mvPoint): m_vNormal(mvNormal), m_fDistanceToOrigin(-D3DXVec3Dot(&m_vNormal, &mvPoint))
{
}

//inline Plane::Plane(const Polygon<D3DXVECTOR3>& mpPolygon)
//{
//	D3DXVECTOR3 a = mpPolygon.m_vList[0];
//	D3DXVECTOR3 b = mpPolygon.m_vList[1];
//	D3DXVECTOR3 c = mpPolygon.m_vList[2];
//
//	m_vNormal = (b - a) ^ (c - a);
//	m_vNormal.Normalize();
//
//	m_fDistanceToOrigin = - (m_vNormal * a);
//}

inline RelativeLocation Plane::TestPoint(const D3DXVECTOR3& mvPoint) const
{
	float dp = D3DXVec3Dot(&mvPoint, &m_vNormal) + m_fDistanceToOrigin;

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

inline D3DXVECTOR3 Plane::SplitLine(const D3DXVECTOR3& a, const D3DXVECTOR3& b) const
{
	float aDot = D3DXVec3Dot(&a, &m_vNormal);
	float bDot = D3DXVec3Dot(&b, &m_vNormal);

	float scale = (-m_fDistanceToOrigin - aDot) / (bDot - aDot);

	return (a + (scale * (b - a)));
}

inline D3DXVECTOR3 Plane::ProjectionPoint(const D3DXVECTOR3& mv) const
{
	float fDistToPlane = D3DXVec3Dot(&mv, &m_vNormal);

	return D3DXVECTOR3(mv - (m_fDistanceToOrigin + fDistToPlane) * m_vNormal);
}

inline float Plane::DistanceToPoint(const D3DXVECTOR3& mvPoint) const
{
	return fabs(D3DXVec3Dot(&mvPoint, &m_vNormal) + m_fDistanceToOrigin);
}

inline void Plane::Flip()
{
	m_vNormal = - m_vNormal;
	m_fDistanceToOrigin = - m_fDistanceToOrigin;
}



#endif