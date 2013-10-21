// File: Camera.h
// Desc:

#ifndef CAMERA_H
#define CAMERA_H

#include"ViewFrustum.h"
#include"Movable.h"
#include"Cone.h"

namespace Maths
{

class DLLAPI Camera: public ViewFrustum, public Movable
{
public:
	float m_NearPlaneDistance;
	float m_FarPlaneDistance;
	float m_FOV_h;  // 水平方向的视角
	float m_FOV_v;  // 垂直方向的视角

	Matrix m_mtxView;  // 视矩阵
	Matrix m_mtxProjection;  // 投影矩阵

	Vector3F m_ViewSphereLocation;
	float m_ViewSphereRadius;
	float m_ViewSphereRadiusSqr;

	Cone m_ViewCone;

public:
	Camera(): ViewFrustum(), Movable(), m_NearPlaneDistance(10.f), m_FarPlaneDistance(1000.f), m_FOV_h(PI/2), m_FOV_v(PI/2) {}
	Camera(const Vector3F& Viewpoint, const Vector3F& LookAt, const Vector3F& Up, float NearD, float FarD, float fov_v, float Aspect);  // 注意：Aspect是Viewport的纵横比，即高比宽；之所以采用fov_v(垂直方向上的视角)而不是fov_h(水平方向上的视角)，是为了与D3DXMatrixPerspectiveFovLH函数兼容
	Camera(const Camera& cam): ViewFrustum(cam), Movable(cam), m_NearPlaneDistance(cam.m_NearPlaneDistance), m_FarPlaneDistance(cam.m_FarPlaneDistance), m_FOV_h(cam.m_FOV_h), m_FOV_v(cam.m_FOV_v), m_mtxView(cam.m_mtxView), m_mtxProjection(cam.m_mtxProjection) {}
	Camera& operator = (const Camera& cam);

	void Init(const Vector3F& Viewpoint, const Vector3F& LookAt, const Vector3F& Up, float NearD, float FarD, float fov_v, float Aspect);  // 注意：Aspect是Viewport的纵横比，即高比宽

	void SetViewpoint(const Vector3F& Viewpoint) { Movable::SetLocation(Viewpoint.x, Viewpoint.y, Viewpoint.z); }
	void SetLookAt(const Vector3F& LookAt);

	void GetCachedViewMatrix(Matrix& mtx) const { mtx = m_mtxView; }
	void GetProjectionMatrix(Matrix& mtx) const { mtx = m_mtxProjection; }

	RelativeLocation SphereTestSphere(const Sphere& s) const;
	RelativeLocation ViewFrustumTestSphere(const Sphere& s) const { return ViewFrustum::TestSphere(s); }
	RelativeLocation SphereTestSphere(const Vector3F& Location, float RadiusSquare) const;
	RelativeLocation ViewFrustumTestSphere(const Vector3F& Location, float RadiusSquare) const { return ViewFrustum::TestSphere(Location, RadiusSquare); }

	// Functions derived from Movable
	virtual void OnViewChanged();
};

}  // namespace

#endif