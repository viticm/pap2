#include "StdAfx.h"
#include ".\kg3dtransformation.h"
#include "mathtool.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

const D3DXQUATERNION KG3DTransformation::IdentiQuaternion(0, 0, 0, 1);

KG3DTransformation::KG3DTransformation(void)
{
	ReSet();
}

KG3DTransformation::~KG3DTransformation(void)
{
}

void KG3DTransformation::ReSet()
{
    D3DXMatrixIdentity(&m_Matrix);
    m_ScalingCenter = D3DXVECTOR3(0,0,0);
    D3DXQuaternionIdentity(&m_ScalingRotation);
    m_Scaling = D3DXVECTOR3(1,1,1);
    m_RotationCenter = D3DXVECTOR3(0,0,0);
    D3DXQuaternionIdentity(&m_Rotation);
    m_Translation = D3DXVECTOR3(0,0,0);
    m_bSleep = FALSE;

    //m_fAngelY = 0.0f;
    //m_fAngelX = 0.0f;
    //m_fAngelZ = 0.0f;
}
void KG3DTransformation::UpdateTransformation()
{
	D3DXMatrixTransformation(&m_Matrix,
		&m_ScalingCenter,
		&m_ScalingRotation,
		&m_Scaling,
		&m_RotationCenter,
		&m_Rotation,
		&m_Translation);
    ASSERT_FLOAT_IS_VALIED(m_Matrix._11);
}


void KG3DTransformation::UpdateByMatrix(const D3DXMATRIX& mat) 
{
    AffineParts affineParts;
    
    ReSet();
    MatrixDecomposeAffine(mat, affineParts);
    
    m_ScalingRotation.x = affineParts.u.x;
    m_ScalingRotation.y = affineParts.u.y;
    m_ScalingRotation.z = affineParts.u.z;
    m_ScalingRotation.w = affineParts.u.w;

    m_Scaling.x = affineParts.k.x;
    m_Scaling.y = affineParts.k.y;
    m_Scaling.z = affineParts.k.z;

    m_Rotation.x = affineParts.q.x;
    m_Rotation.y = affineParts.q.y;
    m_Rotation.z = affineParts.q.z;
    m_Rotation.w = affineParts.q.w;
    ASSERT_FLOAT_IS_VALIED(m_Rotation.x);

    m_Translation.x = affineParts.t.x;
    m_Translation.y = affineParts.t.y;
    m_Translation.z = affineParts.t.z;

	m_bSleep = FALSE;

    UpdateTransformation();
}


void  KG3DTransformation::SetTranslation(const D3DXVECTOR3* pValue)
{
	m_Translation = *pValue;
	m_bSleep = FALSE;
	UpdateTransformation();
}

void KG3DTransformation::SetRotation(const D3DXQUATERNION* pValue)
{
	m_Rotation = *pValue;
    ASSERT_FLOAT_IS_VALIED(m_Rotation.x);

	m_bSleep = FALSE;
	UpdateTransformation();
}
void KG3DTransformation::SetRotationCenter(const D3DXVECTOR3* pValue)
{
	m_RotationCenter = *pValue;
	m_bSleep = FALSE;
	UpdateTransformation();
}
void KG3DTransformation::SetScaling(const D3DXVECTOR3* pValue)
{
	m_Scaling = *pValue;
	m_bSleep = FALSE;
	UpdateTransformation();
}
void KG3DTransformation::SetScalingRotation(const D3DXQUATERNION* pValue)
{
	m_ScalingRotation= *pValue;
	m_bSleep = FALSE;
	UpdateTransformation();
}
void KG3DTransformation::SetScalingCenter(const D3DXVECTOR3* pValue)
{
	m_ScalingCenter = *pValue;
	m_bSleep = FALSE;
	UpdateTransformation();
}


void  KG3DTransformation::GetTranslation(D3DXVECTOR3* pValue)
{
	*pValue = m_Translation;
}

void KG3DTransformation::GetRotation(D3DXQUATERNION* pValue)
{
	*pValue = m_Rotation;
}
void KG3DTransformation::GetRotationCenter(D3DXVECTOR3* pValue)
{
	*pValue =m_RotationCenter;
}
void KG3DTransformation::GetScaling(D3DXVECTOR3* pValue)
{
	*pValue  = m_Scaling;
}
void KG3DTransformation::GetScalingRotation(D3DXQUATERNION* pValue)
{
	*pValue = m_ScalingRotation;
}
void KG3DTransformation::GetScalingCenter(D3DXVECTOR3* pValue)
{
	*pValue = m_ScalingCenter;
}

KG3DTransformation KG3DTransformation::operator + ( CONST KG3DTransformation& A)
{
	KG3DTransformation B;
	B.m_Rotation = m_Rotation * A.m_Rotation;
    ASSERT_FLOAT_IS_VALIED(B.m_Rotation.x);

	B.m_RotationCenter = m_RotationCenter + A.m_RotationCenter;
	B.m_Scaling = m_Scaling + A.m_Scaling;
	B.m_ScalingCenter = m_ScalingCenter + A.m_ScalingCenter;
	B.m_ScalingRotation = m_ScalingRotation + A.m_ScalingRotation;
	B.m_Translation = m_Translation + A.m_Translation;
	B.UpdateTransformation();
	return B;
}
KG3DTransformation KG3DTransformation::operator - ( CONST KG3DTransformation& A)
{
	KG3DTransformation B;
	B.m_Rotation = m_Rotation - A.m_Rotation;
    ASSERT_FLOAT_IS_VALIED(B.m_Rotation.x);

	B.m_RotationCenter = m_RotationCenter - A.m_RotationCenter;
	B.m_Scaling = m_Scaling - A.m_Scaling;
	B.m_ScalingCenter = m_ScalingCenter - A.m_ScalingCenter;
	B.m_ScalingRotation = m_ScalingRotation - A.m_ScalingRotation;
	B.m_Translation = m_Translation - A.m_Translation;
	B.UpdateTransformation();
	return B;
}
KG3DTransformation KG3DTransformation::operator * ( FLOAT A)
{
	KG3DTransformation B;
	B.m_Rotation = m_Rotation * A;
    ASSERT_FLOAT_IS_VALIED(B.m_Rotation.x);

	B.m_RotationCenter = m_RotationCenter * A;
	B.m_Scaling = m_Scaling * A;
	B.m_ScalingCenter = m_ScalingCenter* A;
	B.m_ScalingRotation = m_ScalingRotation * A;
	B.m_Translation = m_Translation * A;
	B.UpdateTransformation();
	return B;
}
KG3DTransformation KG3DTransformation::operator / ( FLOAT A)
{
	KG3DTransformation B;
	B.m_Rotation = m_Rotation / A;
    ASSERT_FLOAT_IS_VALIED(B.m_Rotation.x);

	B.m_RotationCenter = m_RotationCenter / A;
	B.m_Scaling = m_Scaling / A;
	B.m_ScalingCenter = m_ScalingCenter/ A;
	B.m_ScalingRotation = m_ScalingRotation / A;
	B.m_Translation = m_Translation / A;
	B.UpdateTransformation();
	return B;
}

void KG3DTransformation::NormalizeRotation()
{
    D3DXQuaternionNormalize(&m_Rotation, &m_Rotation);
    ASSERT_FLOAT_IS_VALIED(m_Rotation.x);

    D3DXQuaternionNormalize(&m_ScalingRotation, &m_ScalingRotation);
    UpdateTransformation();
}


void KG3DTransformation::Interpolation(KG3DTransformation &NextTrans, float fFactor)
{
    m_ScalingCenter += (NextTrans.m_ScalingCenter - m_ScalingCenter) * fFactor;
    m_Scaling += (NextTrans.m_Scaling - m_Scaling) * fFactor;
    m_RotationCenter += (NextTrans.m_RotationCenter - m_RotationCenter) * fFactor;
    m_Translation += (NextTrans.m_Translation - m_Translation) * fFactor;
    D3DXQuaternionSlerp(&m_ScalingRotation, &m_ScalingRotation, &NextTrans.m_ScalingRotation, fFactor);
    D3DXQuaternionSlerp(&m_Rotation, &m_Rotation, &NextTrans.m_Rotation, fFactor);
    ASSERT_FLOAT_IS_VALIED(m_Rotation.x);

    UpdateTransformation();
}

VOID KG3DTransformation::SetMatrix( const D3DXMATRIX& mat )
{
	m_Matrix = mat;
    ASSERT_FLOAT_IS_VALIED(m_Matrix._11);
}

const D3DXMATRIX& KG3DTransformation::GetMatrix()
{
	return m_Matrix;
}
//BOOL KG3DTransformation::IsTransformated()
//{
//	return !m_bSleep;
//}
//
//void KG3DTransformation::SetTransformated(BOOL bTran)
//{
//	m_bSleep = !bTran;
//}

/*//这部分工作交给Entity完成，这个类只管处理一个SetRotation就好 Last modify by Chen Tianhong: 2007-8-16 10:44:20

HRESULT KG3DTransformation::RotationY(FLOAT fAngelY)
{
    D3DXQUATERNION q;
   // m_fAngelY += fAngelY;
    D3DXQuaternionRotationYawPitchRoll(
        &q, fAngelY, 0.0f, 0.0f
        );
    m_Rotation *= q;
    m_bTransformated = TRUE;
    UpdateTransformation();
    return S_OK;
}


HRESULT KG3DTransformation::RotationZ(FLOAT fAngelZ)
{
    D3DXQUATERNION q;
   // m_fAngelZ += fAngelZ;
    D3DXQuaternionRotationYawPitchRoll(
        &q, 0.0f, 0.0f, fAngelZ
        );
    m_Rotation *= q;
    m_bTransformated = TRUE;
    UpdateTransformation();
    return S_OK;
}


HRESULT KG3DTransformation::RotationX(FLOAT fAngelX)
{
    D3DXQUATERNION q;
    //m_fAngelX += fAngelX;
    D3DXQuaternionRotationYawPitchRoll(
        &q, 0.0f, fAngelX, 0.0f
        );
    m_Rotation *= q;
    m_bTransformated = TRUE;
    UpdateTransformation();
    return S_OK;
}
*/
