// File: Movable.h
// Desc: A base class that represents objects that can move in world space

#ifndef MOVABLE_H
#define MOVABLE_H

#include"Matrix.h"
#include"Sphere.h"
#include"OrientedBox.h"

namespace Maths
{

class DLLAPI Movable: public Sphere, public OrientedBox
{
public:
	Vector3F m_AxisX;
	Vector3F m_AxisY;
	Vector3F m_AxisZ;  // Direction of the sight-line

	// World matrices:
	Matrix m_Rotation;  // ×Ô×ª¾ØÕó
	Matrix m_Scaling;  // Ëõ·Å¾ØÕó
	Matrix m_Translation;  // Æ½ÒÆ¾ØÕó
	Matrix m_Revolution;  // ¹«×ª¾ØÕó

public:
	Movable();
	Movable(const Movable& mvl);
	Movable& operator = (const Movable& mvl);
	virtual ~Movable() {}


	//*************
	// Movements:
	//*************
	// Make'em virtual to give chances for subclasses to do something special.

	// Translation, where ILS means In Local Space, and IWS means In World Space
	void MoveILS(float fdx, float fdy, float fdz);  // fdx, fdy, fdz represent the delta value along x axis, y axis, z axis in local space respectively
	void MoveIWS(float fdx, float fdy, float fdz);  // fdx, fdy, fdz represent the delta value along x axis, y axis, z axis in world space respectively
	void MoveILS(const Matrix& mtx);  // mtx represents a translation in local space
	void MoveIWS(const Matrix& mtx);  // mtx represents a translation in world space

	// Rotation
	void XRotate(float angle);  // Rotate around the x axis in local space
	void YRotate(float angle);  // Rotate around the y axis in local space
	void ZRotate(float angle);  // Rotate around the z axis in local space
	void AxisRotate(const Vector3F& axis, float angle);  // Rotate around an arbitrary axis in world space
	void Revolution(const Vector3F& point, const Vector3F& axis, float angle);  // Rotate around an arbitrary axis in world space. The point is located in the axis
	void XRotate(const Matrix& mtx);  // mtx represents a rotation around the x axis in local space
	void YRotate(const Matrix& mtx);  // mtx represents a rotation around the y axis in local space
	void ZRotate(const Matrix& mtx);  // mtx represents a rotation around the z axis in local space
	void AxisRotate(const Matrix& mtx);  // mtx represents a rotation around an arbitrary axis in world space.
	void Revolution(const Matrix& mtx);  // mtx represents a rotation around an arbitrary axis in world space.

	// Scale
	void Scale(float dx);  // dx represents the delta value to be used to scale the object
	void XScale(float dx);  // dx represents the delta value to be used to scale the object along the x axis in local space
	void YScale(float dx);  // dx represents the delta value to be sued to scale the object along the y axis in local space
	void ZScale(float dx);  // dx represents the delta value to be used to sclae the object along the z axis in local space
	void Scale(const Matrix& mtx) { m_Scaling *= mtx; }  // mtx represents a scaling of the object
	void XScale(const Matrix& mtx) { m_Scaling *= mtx; }  // mtx represents a scaling of the object along the x axis in local space
	void YScale(const Matrix& mtx) { m_Scaling *= mtx; }  // mtx represents a Scaling of the object along the y axis in local space
	void ZScale(const Matrix& mtx) { m_Scaling *= mtx; }  // mtx represents a Scaling of the object along the z axis in local space

	// Some helper functions
	void LookUp(float angle);
	void LookDown(float angle);
	void TurnLeft(float angle);
	void TurnRight(float angle);
	void MoveForward(float dz);
	void MoveBackward(float dz);
	void StrafeLeft(float dx);
	void StrafeRight(float dx);

	void ResetMatrices();
	void SetLocation(float fnx, float fny, float fnz);

	virtual void OnViewChanged() {}

	void GetWorldMatrix(Matrix& mtx) const { mtx = ((m_Rotation * m_Scaling) * m_Translation) * m_Revolution; }
	void GetViewMatrix(Matrix& mtx) const { mtx.CreateViewMatrix(m_Location, m_AxisX, m_AxisY, m_AxisZ); }
};

}  // namespace

#endif