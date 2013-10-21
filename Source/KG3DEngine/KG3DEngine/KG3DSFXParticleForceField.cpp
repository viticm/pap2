////////////////////////////////////////////////////////////////////////////////
//
//  Collator : Chiyer (Zhao XinYu)
//  Date     : 2007-2-12 17:14:16
//
////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "KG3DSFXParticleForceField.h"
#include "KG3DSFX.h"
#include "KG3DGraphicsTool.h"


#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace {

    inline bool HazySameVertex(D3DXVECTOR3 v1, D3DXVECTOR3 v2) {
        float l1 = D3DXVec3Length(&v1);
        float l2 = D3DXVec3Length(&v2);
        if(fabs(l1) - fabs(l2) > 0.1f)
            return false;
        v1 /= l1;
        v2 /= l2;
        if (D3DXVec3Dot(&v1, &v2) < 0.9f)
            return false;
        return true;
    }

}

KG3DSFXParticleForceField::KG3DSFXParticleForceField()
{
    m_eType = SFX_FFT_G;
    m_ForceTimeLine.InsertKeyFrame(0, FLOAT5(0, -1, 0, 0, 0));
    m_ForcePosTimeLine.InsertKeyFrame(0, D3DXVECTOR3(0, 0, 0));
    m_ForceGTimeLine.InsertKeyFrame(0, 0);
}

KG3DSFXParticleForceField::~KG3DSFXParticleForceField(void)
{
}

HRESULT KG3DSFXParticleForceField::ApplyForce(D3DXVECTOR3* pvVelocity, 
                                              float        fPreFrame, 
                                              float        fCurrentFrame, 
                                              D3DXVECTOR3* pPosition,
                                              DWORD        option,
                                              float*       pLastFactor
                                              )
{

    FLOAT5      vpf;
    FLOAT5      vcf;
    D3DXVECTOR3 vPreForce;
    D3DXVECTOR3 vCurForce;
    FLOAT       fFactor = 0.f;
    D3DXVECTOR3 vForce;
    D3DXVECTOR3 vCetner;

    HRESULT hr = m_ForceTimeLine.GetData(&vpf, fPreFrame);
    KG_COM_PROCESS_ERROR(hr);

    hr = m_ForceTimeLine.GetData(&vcf, fCurrentFrame);
    KG_COM_PROCESS_ERROR(hr);

    vPreForce = D3DXVECTOR3(vpf.x, vpf.y, vpf.z);
    vCurForce = D3DXVECTOR3(vcf.x, vcf.y, vcf.z);

    if (m_eType != SFX_FFT_BROWNIAN)
    {
        if (*pLastFactor >= FORCT_ROTATION_MAX)
        {
            float fFactorMin = (vcf.u + vpf.u) / 2;
            float fFactorMax = (vcf.v + vpf.v) / 2;
            *pLastFactor = GetRandom(fFactorMin, fFactorMax);
        }
        fFactor = *pLastFactor * (fCurrentFrame - fPreFrame);
    }

    switch (m_eType)
    {
    case SFX_FFT_G :
        {
            vForce = (vPreForce + vCurForce) * 0.5f;
        }
        break;
    case SFX_FFT_WIND :
        {
            float fRandomX = GetRandom(-fFactor, fFactor);
            float fRandomY = GetRandom(-fFactor, fFactor);
            float fRandomZ = GetRandom(-fFactor, fFactor);
            D3DXVECTOR3 vRan(fRandomX, fRandomY, fRandomZ);
            vForce = (vPreForce + vCurForce) * 0.5f + vRan;
        }
        break;
    case SFX_FFT_WHIRLWIND :
        {
            vForce = (vPreForce + vCurForce) * 0.5f;
            float y = vForce.y;
            vForce -= (*pPosition);
            D3DXVec3Normalize(&vForce, &vForce);
            vForce *= y;
            vForce.y = 0;
        }
        break;
    case SFX_FFT_ROTATION :
        {
            hr = m_ForcePosTimeLine.GetData(&vCetner, fCurrentFrame);
            KG_COM_PROCESS_ERROR(hr);

            float fG = 0.f;
            hr = m_ForceGTimeLine.GetData(&fG, fCurrentFrame);
            KG_COM_PROCESS_ERROR(hr);

            fG *= (fCurrentFrame - fPreFrame);

            D3DXQUATERNION q;
            D3DXMATRIX     m;
            
            D3DXQuaternionRotationAxis(&q, &vCurForce, fFactor);
            D3DXMatrixAffineTransformation(&m, 1.f, &vCetner, &q, &D3DXVECTOR3(0.f, 0.f, 0.f));

            D3DXVec3TransformCoord(&vForce, pPosition, &m);
            D3DXVec3TransformNormal(pvVelocity, pvVelocity, &m);
            vForce = vForce - (*pPosition);

            if (fabs(fG) > FLT_EPSILON)
            {
                D3DXVECTOR3 v1 = vCetner + vCurForce * 100000.f;
                D3DXVECTOR3 v2 = vCetner - vCurForce * 100000.f;
                D3DXVECTOR3 v3;
                D3DXPLANE plane;
                D3DXPlaneFromPointNormal(&plane, pPosition, &vCurForce);
                D3DXPlaneIntersectLine(&v3, &plane, &v1, &v2);
                v3 = *pPosition - v3;
                D3DXVec3Normalize(&v3, &v3);
                v3 *= -fG;
                vForce += v3;
            }
        }
        break;
    default:
        break;
    }

    switch (m_eType)
    {
    case SFX_FFT_ROTATION :
        {
            if (option == SFX_FORCE_OPT_NORMAL)
                (*pPosition) += vForce;
            else
                (*pvVelocity) += vForce;
        }
        break;
    case SFX_FFT_BROWNIAN :
        {
            if (*pLastFactor < FORCT_ROTATION_MAX)
                break;
            (*pLastFactor) = 0.f;
            pvVelocity->x = GetRandom(-vcf.u, vcf.u);
            pvVelocity->y = GetRandom(-vcf.u, vcf.u);
            pvVelocity->z = GetRandom(-vcf.u, vcf.u);
        }
        break;
    default :
        (*pvVelocity) += (vForce * (fCurrentFrame - fPreFrame));
        break;
    }

    hr = S_OK;
Exit0 :
    return hr;
}


HRESULT KG3DSFXParticleForceField::Render()
{
    /*D3DXVECTOR3 vCenter = D3DXVECTOR3(0.f, 0.f, 0.f);

    switch (m_eType)
    {
    case SFX_FFT_G :
        {
            D3DXVECTOR3 v1 = (vPreForce + vCurForce) * 0.5f;
            D3DXVECTOR3 v1 = vCenter + vForce;
            g_pGraphicsTool->DrawLine()

        }
        break;
    case SFX_FFT_WIND :
        {
            float fRandomX = GetRandom(-fFactor, fFactor);
            float fRandomY = GetRandom(-fFactor, fFactor);
            float fRandomZ = GetRandom(-fFactor, fFactor);
            D3DXVECTOR3 vRan(fRandomX, fRandomY, fRandomZ);
            vForce = (vPreForce + vCurForce) * 0.5f + vRan;
        }
        break;
    case SFX_FFT_WHIRLWIND :
        {
            vForce = (vPreForce + vCurForce) * 0.5f;
            float y = vForce.y;
            vForce -= (*pPosition);
            D3DXVec3Normalize(&vForce, &vForce);
            vForce *= y;
            vForce.y = 0;
        }
        break;
    case SFX_FFT_ROTATION :
        {
            hr = m_ForcePosTimeLine.GetData(&vCetner, fCurrentFrame);
            KG_COM_PROCESS_ERROR(hr);

            float fG = 0.f;
            hr = m_ForceGTimeLine.GetData(&fG, fCurrentFrame);
            KG_COM_PROCESS_ERROR(hr);

            fG *= (fCurrentFrame - fPreFrame);

            D3DXQUATERNION q;
            D3DXMATRIX     m;

            D3DXQuaternionRotationAxis(&q, &vCurForce, fFactor);
            D3DXMatrixAffineTransformation(&m, 1.f, &vCetner, &q, &D3DXVECTOR3(0.f, 0.f, 0.f));

            D3DXVec3TransformCoord(&vForce, pPosition, &m);
            vForce = vForce - (*pPosition);

            if (fabs(fG) > FLT_EPSILON)
            {
                D3DXVECTOR3 v1 = vCetner + vCurForce * 100000.f;
                D3DXVECTOR3 v2 = vCetner - vCurForce * 100000.f;
                D3DXVECTOR3 v3;
                D3DXPLANE plane;
                D3DXPlaneFromPointNormal(&plane, pPosition, &vCurForce);
                D3DXPlaneIntersectLine(&v3, &plane, &v1, &v2);
                v3 = *pPosition - v3;
                D3DXVec3Normalize(&v3, &v3);
                v3 *= -fG;
                vForce += v3;
            }
        }
        break;
    default:
        break;
    }*/
    return S_OK;
}

HRESULT KG3DSFXParticleForceField::AddLauncherRef(IEKG3DSFXParticleLauncher* pLauncher)
{
    size_t uLauNum = m_vecLauncherRef.size();

    for (size_t i = 0; i < uLauNum; i++)
    {
        if (m_vecLauncherRef[i] == pLauncher)
            return E_FAIL;
    }

    m_vecLauncherRef.push_back(pLauncher);

    return S_OK;
}

HRESULT KG3DSFXParticleForceField::DelLauncherRef(IEKG3DSFXParticleLauncher* pLauncher)
{
    std::vector<IEKG3DSFXParticleLauncher*>::iterator itr   = m_vecLauncherRef.begin();
    std::vector<IEKG3DSFXParticleLauncher*>::iterator del = m_vecLauncherRef.end();

    while (itr != m_vecLauncherRef.end())
    {
        if (*itr == pLauncher)
        {
            del = itr;
            break;
        }
        itr++;
    }

    if (del == m_vecLauncherRef.end())
        return E_FAIL;

    m_vecLauncherRef.erase(del);

    return S_OK;
}

