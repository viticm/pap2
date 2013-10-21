// File: Camera.cpp
// Desc:

#include"Camera.h"

namespace Maths
{

Camera::Camera(const Vector3F& Viewpoint, const Vector3F& LookAt, const Vector3F& Up, float NearD, float FarD, float fov_v, float Aspect): ViewFrustum(), Movable()
{
	Init(Viewpoint, LookAt, Up, NearD, FarD, fov_v, Aspect);
}

Camera& Camera::operator = (const Camera& cam)
{
	if(&cam != this)
	{
		ViewFrustum::operator = (cam);
		Movable::operator = (cam);

		m_NearPlaneDistance = cam.m_NearPlaneDistance;
		m_FarPlaneDistance = cam.m_FarPlaneDistance;
		m_FOV_h = cam.m_FOV_h;
		m_FOV_v = cam.m_FOV_v;
		m_mtxView = cam.m_mtxView;
		m_mtxProjection = cam.m_mtxProjection;
	}

	return *this;
}

void Camera::Init(const Vector3F& Viewpoint, const Vector3F& LookAt, const Vector3F& Up, float NearD, float FarD, float fov_v, float Aspect)
{
	m_Location = Viewpoint;

	m_AxisZ = LookAt - Viewpoint;
	m_AxisZ.Normalize();
	m_AxisX = Up ^ m_AxisZ;
	m_AxisX.Normalize();
	m_AxisY = m_AxisZ ^ m_AxisX;
	m_AxisY.Normalize();

	m_NearPlaneDistance = NearD;
	m_FarPlaneDistance = FarD;

	m_FOV_v = fov_v;

	float npHalfHeight = m_NearPlaneDistance * tanf(m_FOV_v/2);
	float npHalfWidth = npHalfHeight / Aspect;
	m_FOV_h = 2 * atanf(npHalfWidth / m_NearPlaneDistance);

	m_mtxView.CreateViewMatrix(m_Location, m_AxisX, m_AxisY, m_AxisZ);
	m_mtxProjection.CreateProjectionMatrixByFOV(m_NearPlaneDistance, m_FarPlaneDistance, m_FOV_h, m_FOV_v);

	SetPlanes(m_mtxView * m_mtxProjection);

	float fpHalfHeight = m_FarPlaneDistance * tanf(m_FOV_v/2);
	float fpHalfWidth = fpHalfHeight / Aspect;
	Vector3F FP_TL_Corner = m_Location + m_AxisZ * m_FarPlaneDistance - m_AxisX * fpHalfWidth + m_AxisY * fpHalfHeight;
	/*
	Vector3F FP_TR_Corner = m_Location + m_AxisZ * m_FarPlaneDistance + m_AxisX * fpHalfWidth + m_AxisY * fpHalfHeight;
	Vector3F FP_BR_Corner = m_Location + m_AxisZ * m_FarPlaneDistance + m_AxisX * fpHalfWidth - m_AxisY * fpHalfHeight;
	Vector3F FP_BL_Corner = m_Location + m_AxisZ * m_FarPlaneDistance - m_AxisX * fpHalfWidth - m_AxisY * fpHalfHeight;

	Vector3F NP_TL_Corner = m_Location + m_AxisZ * m_NearPlaneDistance - m_AxisX * npHalfWidth + m_AxisY * npHalfHeight;
	Vector3F NP_TR_Corner = m_Location + m_AxisZ * m_NearPlaneDistance + m_AxisX * npHalfWidth + m_AxisY * npHalfHeight;
	Vector3F NP_BR_Corner = m_Location + m_AxisZ * m_NearPlaneDistance + m_AxisX * npHalfWidth - m_AxisY * npHalfHeight;
	Vector3F NP_BL_Corner = m_Location + m_AxisZ * m_NearPlaneDistance - m_AxisX * npHalfWidth - m_AxisY * npHalfHeight;

	m_ViewSphereLocation = (FP_TL_Corner + FP_TR_Corner + FP_BR_Corner + FP_BL_Corner + NP_TL_Corner + NP_TR_Corner + NP_BR_Corner + NP_BL_Corner)/8;
	
	m_ViewSphereRadius = (FP_TL_Corner - m_ViewSphereLocation).Length();
	m_ViewSphereRadiusSqr = (FP_TL_Corner - m_ViewSphereLocation).Square();
	*/
	m_ViewSphereRadius = m_FarPlaneDistance / 2;
	m_ViewSphereRadiusSqr = m_ViewSphereRadius * m_ViewSphereRadius;
	m_ViewSphereLocation = m_Location + m_AxisZ * m_ViewSphereRadius;
	
	float Distance_VP_TL = (FP_TL_Corner - m_Location).Length();
	m_ViewCone.Init(m_Location, m_AxisZ, acosf(m_FarPlaneDistance / Distance_VP_TL));
}

void Camera::SetLookAt(const Vector3F& LookAt)
{
	Vector3F nzaxis = LookAt - m_Location;
	nzaxis.Normalize();

	if(nzaxis == m_AxisZ)
	{
		return;
	}

	if(nzaxis == m_AxisX)
	{
		m_AxisX = -m_AxisZ;
		m_AxisZ = nzaxis;

		OnViewChanged();
	}
	else if(nzaxis == m_AxisY)
	{
		m_AxisY = -m_AxisZ;
		m_AxisZ = nzaxis;

		OnViewChanged();
	}
	else
	{
		Vector3F RotationAxis = m_AxisZ ^ nzaxis;
		float angle = acosf(nzaxis * m_AxisZ);
		if(angle < -EPSILON)
		{
			angle += 2 * PI;  // Well, nothing different, however...
		}

		AxisRotate(RotationAxis, angle);
	}
}

RelativeLocation Camera::SphereTestSphere(const Sphere& sphere) const
{
	float SquareOfDistance = (m_ViewSphereLocation - sphere.m_Location).Square();
	if(SquareOfDistance >= (m_ViewSphereRadius + sphere.m_Radius) * (m_ViewSphereRadius + sphere.m_Radius))
	{
		return RL_BACK;
	}
	else if(m_ViewSphereRadius > sphere.m_Radius && SquareOfDistance <= (m_ViewSphereRadius - sphere.m_Radius) * (m_ViewSphereRadius - sphere.m_Radius))
	{
		return RL_FRONT;
	}

	return RL_SPLIT;
}

RelativeLocation Camera::SphereTestSphere(const Vector3F& Location, float RadiusSquare) const
{
	Vector3F direction = Location - m_ViewSphereLocation;  // From self sphere's location to parameter sphere's location
	float DistanceBetweenLocationsSqr = direction.Square();

	direction.Normalize();
	Vector3F NearestPoint = m_ViewSphereLocation + direction * m_ViewSphereRadius;
	float NearestDistanceSqr = (Location - NearestPoint).Square();

	switch(FCompare(DistanceBetweenLocationsSqr, m_ViewSphereRadiusSqr))
	{
	case 0:  // DistanceBetweenLocationsSqr == m_ViewSphereRadiusSqr
		return RL_SPLIT;

	case 1:  // DistanceBetweenLocationsSqr > m_ViewSphereRadiusSqr
		switch(FCompare(NearestDistanceSqr, RadiusSquare))
		{
		case 1:
		case 0:
			return RL_BACK;
			
		case -1:
			return RL_SPLIT;
		}

	case -1:  // DistanceBetweenLocationsSqr < m_ViewSphereRadiusSqr
		switch(FCompare(NearestDistanceSqr, RadiusSquare))
		{
		case -1:
			return RL_SPLIT;

		case 1:
		case 0:
			return RL_FRONT;
		}

	default:
		return RL_BACK;
	}
}

void Camera::OnViewChanged()
{
	//D3DXMATRIX dxmtx;
	//Matrix mtxWorld;
	//GetWorldMatrix(mtxWorld);

	//Matrix mtxView;
	//GetViewMatrix(mtxView);
	//Vector3F templ = m_Location * mtxView;
	//m_LookAt = m_LookAt * mtxView;
	//m_LookAt = m_LookAt * mtxWorld;
	m_mtxView.CreateViewMatrix(m_Location, m_AxisX, m_AxisY, m_AxisZ);
	//mtxWorld.CreateViewMatrix(m_Location, m_LookAt, m_Up);

	//D3DXVECTOR3 dx3Eye, dx3At, dx3Up;
	//m_Location.ToD3DXVECTOR3(dx3Eye);
	//m_LookAt.ToD3DXVECTOR3(dx3At);
	//m_Up.ToD3DXVECTOR3(dx3Up);
	//D3DXMatrixLookAtLH(&dxmtx, &dx3Eye, &dx3At, &dx3Up);


	SetPlanes(m_mtxView * m_mtxProjection);

	m_ViewSphereLocation = m_Location + m_AxisZ * m_ViewSphereRadius;

	m_ViewCone.m_Location = m_Location;
	m_ViewCone.m_Direction = m_AxisZ;
}

}  // namespace