// File: Movable.cpp
// Desc:

#include"Movable.h"

namespace Maths
{

Movable::Movable(): Sphere(), OrientedBox(), m_AxisX(1.f, 0.f, 0.f), m_AxisY(0.f, 1.f, 0.f), m_AxisZ(0.f, 0.f, 1.f)
{
	ResetMatrices();
}

Movable::Movable(const Movable& mvl): Sphere(mvl), OrientedBox(mvl), m_AxisX(mvl.m_AxisX), m_AxisY(mvl.m_AxisY), m_AxisZ(mvl.m_AxisZ)
{
	m_Rotation = mvl.m_Rotation;
	m_Scaling = mvl.m_Scaling;
	m_Translation = mvl.m_Translation;
	m_Revolution = mvl.m_Revolution;
}

Movable& Movable::operator = (const Movable& mvl)
{
	if(&mvl != this)
	{
		Sphere::operator = (mvl);
		OrientedBox::operator = (mvl);
		
		m_AxisX = mvl.m_AxisX;
		m_AxisY = mvl.m_AxisY;
		m_AxisZ = mvl.m_AxisZ;
		
		m_Rotation = mvl.m_Rotation;
		m_Scaling = mvl.m_Scaling;
		m_Translation = mvl.m_Translation;
		m_Revolution = mvl.m_Revolution;
	}

	return *this;
}

void Movable::MoveILS(float fdx, float fdy, float fdz)
{
	Vector3F temp = m_AxisX * fdx + m_AxisY * fdy + m_AxisZ * fdz;
	m_Location += temp;

	Matrix mtx;
	mtx.Translate(temp);

	m_Translation *= mtx;

	OnViewChanged();
}

void Movable::MoveIWS(float fdx, float fdy, float fdz)
{
	Vector3F temp(fdx, fdy, fdz);

	m_Location += temp;

	Matrix mtx;
	mtx.Translate(temp);

	m_Translation *= mtx;

	OnViewChanged();
}

void Movable::MoveILS(const Matrix& mtx)
{
	m_Location = m_Location * mtx;

	m_Translation *= mtx;

	OnViewChanged();
}

void Movable::MoveIWS(const Matrix& mtx)
{
	m_Location = m_Location * mtx;

	m_Translation *= mtx;

	OnViewChanged();
}

void Movable::XRotate(float angle)
{
	Matrix mtx;
	mtx.AxisRotate(m_AxisX, angle);

	m_AxisZ = m_AxisZ * mtx;
	m_AxisZ.Normalize();

	m_AxisY = m_AxisZ ^ m_AxisX;
	m_AxisY.Normalize();

	m_Rotation *= mtx;

	OnViewChanged();
}

void Movable::YRotate(float angle)
{
	Matrix mtx;
	mtx.AxisRotate(m_AxisY, angle);

	m_AxisX = m_AxisX * mtx;
	m_AxisX.Normalize();
	m_AxisZ = m_AxisX ^ m_AxisY;
	m_AxisZ.Normalize();

	m_Rotation *= mtx;

	OnViewChanged();
}

void Movable::ZRotate(float angle)
{
	Matrix mtx;
	mtx.AxisRotate(m_AxisZ, angle);

	m_AxisY = m_AxisY * mtx;
	m_AxisY.Normalize();
	m_AxisX = m_AxisY ^ m_AxisZ;
	m_AxisX.Normalize();

	m_Rotation *= mtx;

	OnViewChanged();
}

void Movable::AxisRotate(const Vector3F& axis, float angle)
{
	Matrix mtx;
	mtx.AxisRotate(axis, angle);

	m_AxisX = m_AxisX * mtx;  // The x axis
	m_AxisX.Normalize();
	m_AxisY = m_AxisY * mtx;  // The y axis
	m_AxisY.Normalize();
	m_AxisZ = m_AxisX ^ m_AxisY;  // The x axis
	m_AxisZ.Normalize();

	m_Rotation *= mtx;

	OnViewChanged();
}

void Movable::Revolution(const Vector3F& point, const Vector3F& axis, float angle)
{
	Matrix mtx;
	mtx.PointBasedAxisRotate(point, axis, angle);

	m_Location = m_Location * mtx;  // The location

	m_AxisX = m_AxisX * mtx;  // The x axis
	m_AxisX.Normalize();
	m_AxisY = m_AxisY * mtx;  // The y axis
	m_AxisY.Normalize();
	m_AxisZ = m_AxisX ^ m_AxisY;  // The x axis
	m_AxisZ.Normalize();

	m_Revolution *= mtx;

	OnViewChanged();
}

void Movable::XRotate(const Matrix& mtx)
{
	m_AxisZ = m_AxisZ * mtx;
	m_AxisZ.Normalize();

	m_AxisY = m_AxisZ ^ m_AxisX;
	m_AxisY.Normalize();

	m_Rotation *= mtx;

	OnViewChanged();
}

void Movable::YRotate(const Matrix& mtx)
{
	m_AxisX = m_AxisX * mtx;
	m_AxisX.Normalize();
	m_AxisZ = m_AxisX ^ m_AxisY;
	m_AxisZ.Normalize();

	m_Rotation *= mtx;

	OnViewChanged();
}

void Movable::ZRotate(const Matrix& mtx)
{
	m_AxisY = m_AxisY * mtx;
	m_AxisY.Normalize();
	m_AxisX = m_AxisY ^ m_AxisZ;
	m_AxisX.Normalize();

	m_Rotation *= mtx;

	OnViewChanged();
}

void Movable::AxisRotate(const Matrix& mtx)
{
	m_AxisX = m_AxisX * mtx;  // The x axis
	m_AxisX.Normalize();
	m_AxisY = m_AxisY * mtx;  // The y axis
	m_AxisY.Normalize();
	m_AxisZ = m_AxisX ^ m_AxisY;  // The x axis
	m_AxisZ.Normalize();

	m_Rotation *= mtx;

	OnViewChanged();
}

void Movable::Revolution(const Matrix& mtx)
{
	m_Location = m_Location * mtx;  // The location

	m_AxisX = m_AxisX * mtx;  // The x axis
	m_AxisX.Normalize();
	m_AxisY = m_AxisY * mtx;  // The y axis
	m_AxisY.Normalize();
	m_AxisZ = m_AxisX ^ m_AxisY;  // The x axis
	m_AxisZ.Normalize();

	m_Revolution *= mtx;

	OnViewChanged();
}

void Movable::Scale(float dx)
{
	Matrix mtx;
	mtx.Scale(Vector3F(dx, dx, dx));

	m_Scaling *= mtx;
}

void Movable::XScale(float dx)
{
	Matrix mtx;
	mtx.Scale(Vector3F(dx, 0.f, 0.f));

	m_Scaling *= mtx;
}

void Movable::YScale(float dx)
{
	Matrix mtx;
	mtx.Scale(Vector3F(0.f, dx, 0.f));

	m_Scaling *= mtx;
}

void Movable::ZScale(float dx)
{
	Matrix mtx;
	mtx.Scale(Vector3F(0.f, 0.f, dx));

	m_Scaling *= mtx;
}

void Movable::LookUp(float angle)
{
	if(FCompare(angle, 0.f) == -1)
	{
		angle = -angle;
	}

	XRotate(-angle);
}

void Movable::LookDown(float angle)
{
	if(FCompare(angle, 0.f) == -1)
	{
		angle = - angle;
	}

	XRotate(angle);
}

void Movable::TurnLeft(float angle)
{
	if(FCompare(angle, 0.f) == -1)
	{
		angle = -angle;
	}

	YRotate(-angle);
}

void Movable::TurnRight(float angle)
{
	if(FCompare(angle, 0.f) == -1)
	{
		angle = - angle;
	}

	YRotate(angle);
}

void Movable::MoveForward(float dz)
{
	if(FCompare(dz, 0.f) == -1)
	{
		dz = -dz;
	}

	MoveILS(0.f, 0.f, dz);
}

void Movable::MoveBackward(float dz)
{
	if(FCompare(dz, 0.f) == -1)
	{
		dz = -dz;
	}

	MoveILS(0.f, 0.f, -dz);
}

void Movable::StrafeLeft(float dx)
{
	if(FCompare(dx, 0.f) == -1)
	{
		dx = -dx;
	}

	MoveILS(-dx, 0.f, 0.f);
}

void Movable::StrafeRight(float dx)
{
	if(FCompare(dx, 0.f) == -1)
	{
		dx = -dx;
	}

	MoveILS(dx, 0.f, 0.f);
}

void Movable::ResetMatrices()
{
	m_Rotation.Identilize();
	m_Scaling.Identilize();
	m_Translation.Identilize();
	m_Revolution.Identilize();
}

void Movable::SetLocation(float fnx, float fny, float fnz)
{
	m_Location.x = fnx;
	m_Location.y = fny;
	m_Location.z = fnz;

	OnViewChanged();
}

}  // namespace