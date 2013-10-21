#include "stdafx.h"
#include "./krlsceneobject.h"
#include "../../common/kmathtools.h"
#include "../../SO3Represent.h"

namespace
{
	size_t const ROAD_PATH_POSITION_COUNT = 64;
}

int KRLSceneObject::ms_nRoadPath = 0;

KRLSceneObject::KRLSceneObject()
: m_dwObject(static_cast<DWORD>(INVALID_OBJECT_ID))
, m_dwObjectType(RL_OBJECT_UNKNOWN)
, m_fFadeAlpha(1.0f)
, m_nFadeType(FADE_NONE)
, m_nRoadPath(-1)
, m_nRoadPathPosition(0)
{
	m_vPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	
	m_fDirectionLH = 0.0f;
}

KRLSceneObject::~KRLSceneObject()
{
}

HRESULT KRLSceneObject::AttachObject(DWORD dwObject)
{
	KGLOG_CHECK_ERROR(dwObject != INVALID_OBJECT_ID);

	m_dwObject = dwObject;
	return S_OK;
}

HRESULT KRLSceneObject::SetObjectType(DWORD dwObjectType)
{
	m_dwObjectType = dwObjectType;
	return S_OK;
}

BOOL KRLSceneObject::IsFadeOut() const
{
	return m_nFadeType == FADE_OUT;
}

BOOL KRLSceneObject::IsFinished() const
{
	return m_nFadeType == FADE_OUT_END;
}

void KRLSceneObject::SetDirectionLH(D3DXQUATERNION const& qDirectionLH)
{
    OnRotationLHChange(qDirectionLH);
}

void KRLSceneObject::GetPosition(D3DXVECTOR3& vPosition) const
{
	vPosition = m_vPosition;
}

void KRLSceneObject::SetPosition(D3DXVECTOR3 const& vPosition)
{
    m_vPosition = vPosition;
    OnPositionChange(vPosition);
}

void KRLSceneObject::SetDirectionLH(float fDirectionLH)
{
    m_fDirectionLH = fDirectionLH;

    D3DXQUATERNION qDirectionLH;

    YawToQuatLH(qDirectionLH, YawLHToModelLH(fDirectionLH));
    SetDirectionLH(qDirectionLH);
}

DWORD KRLSceneObject::GetObjectID() const
{
	return m_dwObject;
}

DWORD KRLSceneObject::GetObjectType() const
{
	return m_dwObjectType;
}

KSceneObject* KRLSceneObject::GetGameWorldSceneObject() const
{
	return GetSceneObject(m_dwObjectType, m_dwObject);
}

float KRLSceneObject::GetDirectionLH() const
{
	return m_fDirectionLH;
}

HRESULT KRLSceneObject::SetAlpha(float fAlpha)
{
	SetFadeAlpha(fAlpha);
	return OnAlphaChange(fAlpha);
}

HRESULT KRLSceneObject::GetBBox(D3DXVECTOR3& vMin, D3DXVECTOR3& vMax) const
{
    return OnGetBBox(vMin, vMax); 
}

void KRLSceneObject::FadeIn()
{
    m_nFadeType = FADE_IN;
}

void KRLSceneObject::FadeOut()
{
    m_nFadeType = FADE_OUT;
}

void KRLSceneObject::SetFadeAlpha(float fAlpha)
{
    m_fFadeAlpha = fAlpha;
}

void KRLSceneObject::UpdateFade()
{
    switch (m_nFadeType)
	{
	case FADE_NONE:
		break;
	case FADE_IN:
        if (m_fFadeAlpha < 1.0f)
        {
			m_fFadeAlpha += 0.2f;
            m_fFadeAlpha = m_fFadeAlpha > 1.0f ? 1.0f : m_fFadeAlpha;
            SetAlpha(m_fFadeAlpha);
        }
        else
        {
            m_nFadeType = FADE_NONE;
        }
		break;
	case FADE_OUT:
        if (m_fFadeAlpha > 0.0f)
        {
			m_fFadeAlpha -= 0.2f;
            m_fFadeAlpha = m_fFadeAlpha < 0.0f ? 0.0f : m_fFadeAlpha;
            SetAlpha(m_fFadeAlpha);
        }
        else
        {
            m_nFadeType = FADE_OUT_END;
        }
		break;
	case FADE_OUT_END:
		break;
	}
}
