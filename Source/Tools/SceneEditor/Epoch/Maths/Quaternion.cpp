// File: Quaternion.cpp
// Desc: Ëã·¨À´×ÔOGRE

#include"Quaternion.h"
#include"MathsCommon.h"
#include<math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


namespace Maths
{

const Quaternion Quaternion::Zero(0.f, 0.f, 0.f, 0.f);
const Quaternion Quaternion::Identity(1.f, 0.f, 0.f, 0.f);

void Quaternion::Normalize()
{
	float fFactor = 1.f / sqrtf(Squared());

	w *= fFactor;
	x *= fFactor;
	y *= fFactor;
	z *= fFactor;
}

Quaternion Quaternion::Inverse() const
{
	float fSquared = Squared();

	if(fSquared > 0.f)
	{
		float fFactor = 1.f / fSquared;
		return *this * fFactor;
	}
	else
	{
		return Zero;
	}
}

Quaternion Quaternion::Exp() const
{
	Quaternion qtnRet;

	float fAngle = sqrtf(Squared());
	float fSin = sinf(fAngle);

	qtnRet.w = cosf(fAngle);

	if(FastFabs(fSin) > EPSILON)
	{
		float fCoeff = fAngle / fSin;
		qtnRet.x = x * fCoeff;
		qtnRet.y = y * fCoeff;
		qtnRet.z = z * fCoeff;
	}
	else
	{
		qtnRet.x = x;
		qtnRet.y = y;
		qtnRet.z = z;
	}

	return qtnRet;
}

Quaternion Quaternion::Log() const
{
	Quaternion qtnRet;
	qtnRet.w = 0.f;

	if(FastFabs(w) < 1.f)
	{
		float fAngle = acosf(w);
		float fSin = sinf(fAngle);

		if(FastFabs(fSin) >= EPSILON)
		{
			float fCoeff = fAngle / fSin;

			qtnRet.x = x * fCoeff;
			qtnRet.y = y * fCoeff;
			qtnRet.z = z * fCoeff;

			return qtnRet;
		}
	}

	qtnRet.x = x;
	qtnRet.y = y;
	qtnRet.z = z;
	return qtnRet;
}

void Quaternion::FromRotationMatrix(const Matrix& mtxRot)
{
	// Algorithm in Ken Shoemake's article in 1987 SIGGRAPH course notes
	// article "Quaternion Calculus and Fast Animation".

	float fTrace = mtxRot[0][0] + mtxRot[1][1] + mtxRot[2][2];
	float fRoot;

	if(fTrace > 0.0)
	{
		// |w| > 1/2, may as well choose w > 1/2
		fRoot = sqrtf(fTrace + 1.f);  // 2w
		w = 0.5f * fRoot;
		fRoot = 0.5f / fRoot;  // 1/(4w)
		x = (mtxRot[2][1] - mtxRot[1][2]) * fRoot;
		y = (mtxRot[0][2] - mtxRot[2][0]) * fRoot;
		z = (mtxRot[1][0] - mtxRot[0][1]) * fRoot;
	}
	else
	{
		// |w| <= 1/2
		static int s_iNext[3] = {1, 2, 0};

		int i = 0;
		if(mtxRot[1][1] > mtxRot[0][0])
		{
			i = 1;
		}
		if(mtxRot[2][2] > mtxRot[i][i])
		{
			i = 2;
		}
		int j = s_iNext[i];
		int k = s_iNext[j];

		fRoot = sqrtf(mtxRot[i][i] - mtxRot[j][j] - mtxRot[k][k] + 1.f);
		float* apkQuat[3] = {&x, &y, &z};
		*apkQuat[i] = 0.5f * fRoot;
		fRoot = 0.5f / fRoot;
		w = (mtxRot[k][j] - mtxRot[j][k]) * fRoot;
		*apkQuat[j] = (mtxRot[j][i] + mtxRot[i][j]) * fRoot;
		*apkQuat[k] = (mtxRot[k][i] + mtxRot[i][k]) * fRoot;
	}
}

void Quaternion::ToRotationMatrix (Matrix& mtxRot) const
{
	float fTx  = 2.f * x;
	float fTy  = 2.f * y;
	float fTz  = 2.f * z;
	float fTwx = fTx * w;
	float fTwy = fTy * w;
	float fTwz = fTz * w;
	float fTxx = fTx * x;
	float fTxy = fTy * x;
	float fTxz = fTz * x;
	float fTyy = fTy * y;
	float fTyz = fTz * y;
	float fTzz = fTz * z;

	mtxRot[0][0] = 1.f - (fTyy + fTzz);
	mtxRot[0][1] = fTxy - fTwz;
	mtxRot[0][2] = fTxz + fTwy;
	mtxRot[1][0] = fTxy + fTwz;
	mtxRot[1][1] = 1.f - (fTxx + fTzz);
	mtxRot[1][2] = fTyz - fTwx;
	mtxRot[2][0] = fTxz - fTwy;
	mtxRot[2][1] = fTyz + fTwx;
	mtxRot[2][2] = 1.f - (fTxx + fTyy);
}

void Quaternion::FromAngleAxis(float fAngle, const Vector3F& vAxis)
{
	float fHalfAngle = 0.5f * fAngle;
	float fSin = sinf(fHalfAngle);
	w = cosf(fHalfAngle);
	x = fSin * vAxis.x;
	y = fSin * vAxis.y;
	z = fSin * vAxis.z;
}

void Quaternion::ToAngleAxis (float& fAngle, Vector3F& vAxis) const
{
	float fSqrLength = x * x + y * y + z * z;
	if(fSqrLength > 0.f)
	{
		fAngle = 2.f * acosf(w);
		float fInvLength = 1.f / sqrtf(fSqrLength);
		vAxis.x = x * fInvLength;
		vAxis.y = y * fInvLength;
		vAxis.z = z * fInvLength;
	}
	else
	{
		// angle is 0 (mod 2*pi), so any axis will do
		fAngle = 0.0;
		vAxis.x = 1.0;
		vAxis.y = 0.0;
		vAxis.z = 0.0;
	}
}

void Quaternion::FromAxes(const Vector3F* pvAxis)
{
	Matrix mtxRot;

	for(int iCol = 0; iCol < 3; iCol++)
	{
		mtxRot[0][iCol] = pvAxis[iCol].x;
		mtxRot[1][iCol] = pvAxis[iCol].y;
		mtxRot[2][iCol] = pvAxis[iCol].z;
	}

	FromRotationMatrix(mtxRot);
}

void Quaternion::ToAxes(Vector3F* pvAxis) const
{
	Matrix mtxRot;

	ToRotationMatrix(mtxRot);

	for (int iCol = 0; iCol < 3; iCol++)
	{
		pvAxis[iCol].x = mtxRot[0][iCol];
		pvAxis[iCol].y = mtxRot[1][iCol];
		pvAxis[iCol].z = mtxRot[2][iCol];
	}
}

void Quaternion::FromAxes(const Vector3F& vXAxis, const Vector3F& vYAxis, const Vector3F& vZAxis)
{
	Matrix mtxRot;

	mtxRot[0][0] = vXAxis.x;
	mtxRot[1][0] = vXAxis.y;
	mtxRot[2][0] = vXAxis.z;

	mtxRot[0][1] = vYAxis.x;
	mtxRot[1][1] = vYAxis.y;
	mtxRot[2][1] = vYAxis.z;

	mtxRot[0][2] = vZAxis.x;
	mtxRot[1][2] = vZAxis.y;
	mtxRot[2][2] = vZAxis.z;

	FromRotationMatrix(mtxRot);
}

void Quaternion::ToAxes(Vector3F& vXAxis, Vector3F& vYAxis, Vector3F& vZAxis) const
{
	Matrix mtxRot;

	ToRotationMatrix(mtxRot);

	vXAxis.x = mtxRot[0][0];
	vXAxis.y = mtxRot[1][0];
	vXAxis.z = mtxRot[2][0];

	vYAxis.x = mtxRot[0][1];
	vYAxis.y = mtxRot[1][1];
	vYAxis.z = mtxRot[2][1];

	vZAxis.x = mtxRot[0][2];
	vZAxis.y = mtxRot[1][2];
	vZAxis.z = mtxRot[2][2];
}

Vector3F Quaternion::operator * (const Vector3F& vVector) const
{
	// nVidia SDK implementation
	Vector3F uv, uuv;
	Vector3F qvec(x, y, z);
	uv = qvec ^ vVector;
	uuv = qvec ^ uv;
	uv *= (2.f * w);
	uuv *= 2.f;

	return vVector + uv + uuv;
}

Quaternion Quaternion::Slerp(float fT, const Quaternion& rkP, const Quaternion& rkQ, bool shortestPath)
{
	float fCos = rkP.Dot(rkQ);
	float fAngle = acosf(fCos);

	if(FastFabs(fAngle) < EPSILON)
	{
		return rkP;
	}

	float fSin = sinf(fAngle);
	float fInvSin = 1.f / fSin;
	float fCoeff0 = sinf((1.f - fT) * fAngle) * fInvSin;
	float fCoeff1 = sinf(fT * fAngle) * fInvSin;
	// Do we need to invert rotation?
	if (fCos < 0.f && shortestPath)
	{
		fCoeff0 = -fCoeff0;
		// taking the complement requires renormalisation
		Quaternion t(fCoeff0 * rkP + fCoeff1 * rkQ);
		t.Normalize();
		return t;
	}
	else
	{
		return fCoeff0 * rkP + fCoeff1 * rkQ;
	}
}

Quaternion Quaternion::SlerpExtraSpins(float fT, const Quaternion& rkP, const Quaternion& rkQ, int iExtraSpins)
{
	float fCos = rkP.Dot(rkQ);
	float fAngle = acosf(fCos);

	if (FastFabs(fAngle) < EPSILON)
	{
		return rkP;
	}

	float fSin = sinf(fAngle);
	float fPhase = PI * iExtraSpins * fT;
	float fInvSin = 1.f / fSin;
	float fCoeff0 = sinf((1.f - fT) * fAngle - fPhase) * fInvSin;
	float fCoeff1 = sinf(fT * fAngle + fPhase) * fInvSin;
	return fCoeff0 * rkP + fCoeff1 * rkQ;
}

void Quaternion::Intermediate(const Quaternion& rkQ0, const Quaternion& rkQ1, const Quaternion& rkQ2, Quaternion& rkA, Quaternion& rkB)
{
	// assert:  q0, q1, q2 are unit quaternions

	Quaternion kQ0inv = rkQ0.UnitInverse();
	Quaternion kQ1inv = rkQ1.UnitInverse();
	Quaternion rkP0 = kQ0inv * rkQ1;
	Quaternion rkP1 = kQ1inv * rkQ2;
	Quaternion kArg = 0.25f * (rkP0.Log() - rkP1.Log());
	Quaternion kMinusArg = -kArg;

	rkA = rkQ1 * kArg.Exp();
	rkB = rkQ1 * kMinusArg.Exp();
}

Quaternion Quaternion::Squad(float fT, const Quaternion& rkP, const Quaternion& rkA, const Quaternion& rkB, const Quaternion& rkQ, bool shortestPath)
{
	float fSlerpT = 2.f * fT * (1.f - fT);
	Quaternion kSlerpP = Slerp(fT, rkP, rkQ, shortestPath);
	Quaternion kSlerpQ = Slerp(fT, rkA, rkB);
	return Slerp(fSlerpT, kSlerpP ,kSlerpQ);
}

}  // namespace