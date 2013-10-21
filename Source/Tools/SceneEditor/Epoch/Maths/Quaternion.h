// File: Quaternion.h
// Desc: 四元数；算法来自OGRE

#ifndef QUATERNION_H
#define QUATERNION_H

#include"Matrix.h"
#include"Vector3F.h"

namespace Maths
{

class DLLAPI Quaternion
{
public:
	float w, x, y, z;

	// 常量：
	static const Quaternion Zero;
	static const Quaternion Identity;

public:
	Quaternion(float fw = 1.f, float fx = 0.f, float fy = 0.f, float fz = 0.f): w(fw), x(fx), y(fy), z(fz) {}
	Quaternion(const Quaternion& qtn): w(qtn.w), x(qtn.x), y(qtn.y), z(qtn.z) {}
	Quaternion& operator = (const Quaternion& qtn);
	~Quaternion() {}

	Quaternion operator - () const;

	float Dot(const Quaternion& qtn) const;  // 点乘
	float Squared() const;  // 长度的平方
	void Normalize();  // 标准化
	Quaternion Inverse() const;
	Quaternion UnitInverse () const;
	Quaternion Exp() const;
	Quaternion Log() const;

	void FromRotationMatrix(const Matrix& mtxRot);
	void ToRotationMatrix (Matrix& mtxRot) const;
	void FromAngleAxis(float fAngle, const Vector3F& vAxis);
	void ToAngleAxis (float& fAngle, Vector3F& vAxis) const;
	void FromAxes(const Vector3F* pvAxis);
	void ToAxes(Vector3F* pvAxis) const;
	void FromAxes(const Vector3F& vXAxis, const Vector3F& vYAxis, const Vector3F& vZAxis);
	void ToAxes(Vector3F& vXAxis, Vector3F& vYAxis, Vector3F& vZAxis) const;
	
	Vector3F operator * (const Vector3F& vVector) const;  // return the rotateed vector
	
	// spherical linear interpolation
	static Quaternion Slerp(float fT, const Quaternion& rkP, const Quaternion& rkQ, bool shortestPath = false);
	static Quaternion SlerpExtraSpins (float fT, const Quaternion& rkP, const Quaternion& rkQ, int iExtraSpins);
	
	static void Intermediate(const Quaternion& rkQ0, const Quaternion& rkQ1, const Quaternion& rkQ2, Quaternion& rkA, Quaternion& rkB);  // setup for spherical quadratic interpolation
	static Quaternion Squad(float fT, const Quaternion& rkP, const Quaternion& rkA, const Quaternion& rkB, const Quaternion& rkQ, bool shortestPath = false);  // spherical quadratic interpolation
};

// 全局函数：

inline const Quaternion operator + (const Quaternion& a, const Quaternion& b)
{
	return Quaternion(a.w + b.w, a.x + b.x, a.y + b.y, a.z + b.z);
}

inline const Quaternion operator - (const Quaternion& a, const Quaternion& b)
{
	return Quaternion(a.w - b.w, a.x - b.x, a.y - b.y, a.z - b.z);
}

inline const Quaternion operator * (const Quaternion& a, const Quaternion& b)
{
	return Quaternion(a.w * b.w, a.x * b.x, a.y * b.y, a.z * b.z);
}

inline const Quaternion operator * (const Quaternion& qtn, float fScalor)
{
	return Quaternion(qtn.w * fScalor, qtn.x * fScalor, qtn.y * fScalor, qtn.z * fScalor);
}

inline const Quaternion operator * (float fScalor, const Quaternion& qtn)
{
	return qtn * fScalor;
}

inline bool operator == (const Quaternion& a, const Quaternion& b)
{
	return a.w == b.w && a.x == b.x && a.y == b.y && a.z == b.z;
}

inline bool operator != (const Quaternion& a, const Quaternion& b)
{
	return a.w != b.w || a.x != b.x || a.y != b.y || a.z != b.z;
}

// 成员函数：

inline Quaternion& Quaternion::operator = (const Quaternion& qtn)
{
	if(&qtn != this)
	{
		w = qtn.w;
		x = qtn.x;
		y = qtn.y;
		z = qtn.z;
	}

	return *this;
}

inline Quaternion Quaternion::operator - () const
{
	return Quaternion(-w, -x, -y, -z);
}

inline float Quaternion::Dot(const Quaternion& qtn) const
{
	return w * qtn.w + x * qtn.x + y * qtn.y + z * qtn.z;
}

inline float Quaternion::Squared() const
{
	return w * w + x * x + y * y + z * z;
}

inline Quaternion Quaternion::UnitInverse () const
{
	return Quaternion(w, -x, -y, -z);
}

}  // namespace

#endif