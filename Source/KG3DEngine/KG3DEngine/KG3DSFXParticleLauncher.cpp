////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DSFXParticleLauncher.cpp
//  Version     : 1.0
//  Creator     : Chiyer (Zhao XinYu)
//  Create Date : 2006-11-22 14:46:14
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "KG3DSFXParticleLauncher.h"
#include "KG3DSFXParticleContainer.h"
#include "KG3DGraphicsTool.h"
#include "KG3DSFXParticle.h"
#include "KG3DModel.h"
#include "KG3DMesh.h"
#include "KG3DSFX.h"
#include "MathTool.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


KG3DSFXParticleLauncher::KG3DSFXParticleLauncher(KG3DSFX*                   pParentSFX,
                                                 KG3DSFXParticleContainer*  pContainer,
                                                 SFX_PARTICLE_TYPE          eParticleType,
                                                 SFX_LAUNCHER_SHAPE         eLauncherShape,
                                                 SFX_PARTICLE_UPDATE_MODE   eParticleUpdateMode
)
{
    ASSERT(pContainer);
    ASSERT(pParentSFX);

    m_pParticleContainer        = pContainer;
    m_pParentSFX                = pParentSFX;
    m_pParticleLifeTimeLine     = NULL;
    m_eParticleType             = eParticleType;
    m_eLauncherShape            = eLauncherShape;
    m_eParticleUpdateMode       = eParticleUpdateMode;
    m_bSpeedInherit             = FALSE;
    m_fLastLauncherFrame        = 0;
    m_Translation               = D3DXVECTOR3(0.f, 0.f, 0.f);
    m_Scaling                   = D3DXVECTOR3(1.f, 1.f, 1.f);
    m_RotationCenter            = D3DXVECTOR3(0.f, 0.f, 0.f);
    m_ScalingCenter             = D3DXVECTOR3(0.f, 0.f, 0.f);
    m_fLauncherShapeParam1      = 100.f;
    m_fLauncherShapeParam2      = 100.f;
    m_dwBindTrackIndex          = SFX_NOT_BIND_TO_TRACK;
    m_eBindToTrackMode          = SFX_LBTTM_NORMAL;
    m_eBlendMode                = SFX_BM_SRCALPHA_INVSRCALPHA;
	m_dwRandTexDelay            = 100;
	m_dwRandColDelay            = 500;
	m_dwRandRotDelay            = 10000000;
    m_dwRandAlpDelay            = 0;
    m_dwMotionType              = SFX_MOTION_WROLD;
    m_dwForceOption             = SFX_FORCE_OPT_NORMAL;
    m_nHide                     = FALSE;
    m_scName                    = TEXT("Laucher");
    m_fParticleNumKeep          = 0.f;

    D3DXMatrixIdentity(&m_matWorld);
    D3DXMatrixIdentity(&m_Matrix);
    D3DXQuaternionIdentity(&m_Rotation);

    m_RtsTimeLine.m_TranslationTimeLine.InsertKeyFrame(0, D3DXVECTOR3(0.f, 0.f, 0.f));
    m_RtsTimeLine.m_RotationTimeLine.InsertKeyFrame(0, KG3DTransformation::IdentiQuaternion);
    m_RtsTimeLine.m_ScalingTimeLine.InsertKeyFrame(0, D3DXVECTOR3(1.f, 1.f, 1.f));
    m_RtsTimeLine.m_ScalRotationTimLine.InsertKeyFrame(0, KG3DTransformation::IdentiQuaternion);
    m_fParticleNumLine.InsertKeyFrame(0, 0.f);
    m_fParticleLifeMinLine.InsertKeyFrame(0, 0.f);
    m_fParticleLifeMaxLine.InsertKeyFrame(0, 0.f);
    m_fParticleSpeedMinLine.InsertKeyFrame(0, 0.f);
    m_fParticleSpeedMaxLine.InsertKeyFrame(0, 0.f);
    m_fParticleScatterMin.InsertKeyFrame(0, 0.f);
    m_fParticleScatterMax.InsertKeyFrame(0, 0.f);



}

KG3DSFXParticleLauncher::~KG3DSFXParticleLauncher(void)
{
}


void KG3DSFXParticleLauncher::GetAABBox(AABBOX *pBox)
{
    D3DXVECTOR3 a, b;
    switch (m_eLauncherShape)
    {
    case SFX_LS_RECTANGLE:
        {
            a.x = -m_fLauncherShapeParam1 * 0.5f;
            b.x = m_fLauncherShapeParam1 * 0.5f;
            a.y = -10;
            b.y = 10;
            a.z = -m_fLauncherShapeParam2 * 0.5f;
            b.z = m_fLauncherShapeParam2 * 0.5f;
        }
        break;
    case SFX_LS_CIRCLE:
        {
            a.x = -m_fLauncherShapeParam1;
            b.x = m_fLauncherShapeParam1;
            a.y = -10;
            b.y = 10;
            a.z = -m_fLauncherShapeParam1;
            b.z = m_fLauncherShapeParam1;
        }
        break;
    case SFX_LS_SPHERE:
        {
            a.x = -m_fLauncherShapeParam1;
            b.x = m_fLauncherShapeParam1;
            a.y = -m_fLauncherShapeParam1;
            b.y = m_fLauncherShapeParam1;
            a.z = -m_fLauncherShapeParam1;
            b.z = m_fLauncherShapeParam1;
        }
        break;
    case SFX_LS_CYLINDER:
        {
            a.x = -m_fLauncherShapeParam1;
            b.x = m_fLauncherShapeParam1;
            a.y = -m_fLauncherShapeParam2 * 0.5f;
            b.y = m_fLauncherShapeParam2 * 0.5f;
            a.z = -m_fLauncherShapeParam1;
            b.z = m_fLauncherShapeParam1;
        }
        break;
    default:
        ASSERT(0 && "ERROR: Invalidate launcher shape!");
    }

    UpdateTransformation();
    pBox->AddPosition(a, b, &m_Matrix);
}

HRESULT KG3DSFXParticleLauncher::FrameMove()
{
    m_matWorld = m_Matrix * m_pParentSFX->m_matWorld;
    return S_OK;
}

inline D3DXVECTOR3 operator* (const D3DXVECTOR3& l, const D3DXVECTOR3& r)
{
    return D3DXVECTOR3(l.x * r.x, l.y * r.y, l.z * r.z);
}

HRESULT KG3DSFXParticleLauncher::LaunchParticle(float fCurrentFrame)
{
    /*
     * check need laucher
     */


    if (m_nHide)
        return S_OK;

    if (m_pParentSFX->GetMaxParticleNum() != 0 && (m_pParentSFX->GetParticleNum() >= m_pParentSFX->GetMaxParticleNum()))
        return S_OK;


    /*
     * get rtx frame data
     */

    m_RtsTimeLine.m_TranslationTimeLine.GetData(&m_Translation, fCurrentFrame);
    m_RtsTimeLine.m_RotationTimeLine.GetData(&m_Rotation, fCurrentFrame);
    m_RtsTimeLine.m_ScalingTimeLine.GetData(&m_Scaling, fCurrentFrame);
    m_RtsTimeLine.m_ScalRotationTimLine.GetData(&m_ScalingRotation, fCurrentFrame);

    ASSERT(m_pParentSFX);

    UpdateTransformation();


    /*
     * get track info and update the matrix
     */

    if (m_dwBindTrackIndex != SFX_NOT_BIND_TO_TRACK && m_eBindToTrackMode == SFX_LBTTM_NORMAL)
    {
        KG3DSFXTrack *pTrack = NULL;
        D3DXMATRIX matTrack;
        if (SUCCEEDED(m_pParentSFX->GetTrack(m_dwBindTrackIndex, &pTrack)))
        {
            pTrack->GetTransform(fCurrentFrame, &matTrack);
            m_Matrix *= matTrack;
            ASSERT_FLOAT_IS_VALIED(m_Matrix._11);
        }
    }

    float   fNum        = 0;
    float   fLifeMax    = 0;
    float   fLifeMin    = 0;
    float   fSpeedMax   = 0;
    float   fSpeedMin   = 0;
    float   fScatterMax = 0;
    float   fScatterMin = 0;

    m_fParticleNumLine.GetData(&fNum, fCurrentFrame);

    float fElapsed = 0.f;
    if (fCurrentFrame < m_fLastLauncherFrame)
        fElapsed = fCurrentFrame;
    else
        fElapsed =(fCurrentFrame - m_fLastLauncherFrame);
    m_fParticleNumKeep += fElapsed * fNum;

    int nNum = (int)m_fParticleNumKeep;
    m_fParticleNumKeep = 0;//max(0.f, m_fParticleNumKeep - (float)nNum);

    if (nNum <= 0)
        return S_OK;

    m_fLastLauncherFrame = fCurrentFrame;

    m_fParticleLifeMaxLine.GetData(&fLifeMax,   fCurrentFrame);
    m_fParticleLifeMinLine.GetData(&fLifeMin,   fCurrentFrame);
    m_fParticleSpeedMaxLine.GetData(&fSpeedMax, fCurrentFrame);
    m_fParticleSpeedMinLine.GetData(&fSpeedMin, fCurrentFrame);
    m_fParticleScatterMax.GetData(&fScatterMax, fCurrentFrame);
    m_fParticleScatterMin.GetData(&fScatterMin, fCurrentFrame);

    if (m_dwBindTrackIndex != SFX_NOT_BIND_TO_TRACK && m_eBindToTrackMode == SFX_LBTTM_PARTICLE_TO_TRACK)
    {
        for (int i = 0; i < nNum; i++)
        {
            float fLife = GetRandom(fLifeMin, fLifeMax);
            if (fLife < fCurrentFrame && m_eParticleUpdateMode == SFX_PUM_FOLLOW_SFX)
                continue;

            float fSpeed = GetRandom(fSpeedMin, fSpeedMax);

            D3DXMATRIX *pMatrix = m_pParentSFX->GetCurrentWorldMatrix();
            D3DXVECTOR3 vPosition(pMatrix->_41, pMatrix->_42, pMatrix->_43);
            D3DXVECTOR3 vVelocity(fSpeed, 0, 0);
            MakeParticle(fLife, &vPosition, &vVelocity, fCurrentFrame);
        }
    }
    else
    {
        for (int i = 0; i < nNum; i++)
        {
            float fLife = GetRandom(fLifeMin, fLifeMax);
            if (fLife < fCurrentFrame && m_eParticleUpdateMode == SFX_PUM_FOLLOW_SFX)
                continue;

            float fSpeed = GetRandom(fSpeedMin, fSpeedMax);
            D3DXVECTOR3 vPosition;
            D3DXVECTOR3 vDirection;
            GetParticleDirectionAndPos(fScatterMin, fScatterMax, &vDirection, &vPosition);
            D3DXVECTOR3 vVelocity = vDirection * (m_pParentSFX->m_Srt.vScanl * fSpeed);
            D3DXVec3TransformCoord(&vPosition, &vPosition, m_pParentSFX->GetCurrentWorldMatrix());
            MakeParticle(fLife, &vPosition, &vVelocity, fCurrentFrame);
        }
    }
    
    return S_OK;
}


KG3DSFX* KG3DSFXParticleLauncher::GetParentSFX() const
{
    return m_pParentSFX;
}

namespace {
    inline DWORD F2DW(float f) {
        return *((DWORD*)&f);
    }
    inline int sgn(float f) {
        if (f > 0.f)
            return  1;
        else
            return -1;
    }
}

HRESULT KG3DSFXParticleLauncher::RenderSelectMark(DWORD dwColor)
{
    m_nSelFlag = true;
    return S_OK;
}

HRESULT KG3DSFXParticleLauncher::EditorRender()
{
    KG3DRenderState R;
    R.SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
    R.SetRenderState(D3DRS_ZENABLE, TRUE);

    D3DXMATRIX matWorldSave;
    g_pd3dDevice->GetTransform(D3DTS_WORLD, &matWorldSave);
    g_pd3dDevice->SetTransform(D3DTS_WORLD, &m_matWorld);

    DWORD dwColor = 0xFFEEEEEE;
    if (m_nSelFlag)
        dwColor = 0xFFFF0000;
    m_nSelFlag = false;

    float fHalfParam1 = m_fLauncherShapeParam1 / 2;
    float fHalfParam2 = m_fLauncherShapeParam2 / 2;

    switch (m_eLauncherShape)
    {
    case SFX_LS_RECTANGLE :
        {
            D3DXVECTOR3 v1 = D3DXVECTOR3(-fHalfParam1, 0.f,  fHalfParam2);
            D3DXVECTOR3 v2 = D3DXVECTOR3( fHalfParam1, 0.f,  fHalfParam2);
            D3DXVECTOR3 v3 = D3DXVECTOR3( fHalfParam1, 0.f, -fHalfParam2);
            D3DXVECTOR3 v4 = D3DXVECTOR3(-fHalfParam1, 0.f, -fHalfParam2);

            g_cGraphicsTool.DrawLine(&v1, &v2, dwColor, dwColor);
            g_cGraphicsTool.DrawLine(&v2, &v3, dwColor, dwColor);
            g_cGraphicsTool.DrawLine(&v3, &v4, dwColor, dwColor);
            g_cGraphicsTool.DrawLine(&v4, &v1, dwColor, dwColor);

            v1 = D3DXVECTOR3(0.f,   0.f, 0.f);
            v2 = D3DXVECTOR3(0.f, 50.f, 0.f);
            v3 = D3DXVECTOR3(-10.f, 43.f, 0.f);
            v4 = D3DXVECTOR3( 10.f, 43.f, 0.f);

            g_cGraphicsTool.DrawLine(&v1, &v2, 0xFFAAAAAA, 0xFFAAAAAA);
            g_cGraphicsTool.DrawLine(&v3, &v2, 0xFFAAAAAA, 0xFFAAAAAA);
            g_cGraphicsTool.DrawLine(&v4, &v2, 0xFFAAAAAA, 0xFFAAAAAA);
            
        }
        break;
    case SFX_LS_CIRCLE :
        {
            g_cGraphicsTool.DrawCircle(D3DXVECTOR3(0.f, 0.f, 0.f), D3DXVECTOR3(0.f, 1.f, 0.f), m_fLauncherShapeParam1, dwColor);
            g_cGraphicsTool.DrawCircle(D3DXVECTOR3(0.f, 0.f, 0.f), D3DXVECTOR3(0.f, 1.f, 0.f), m_fLauncherShapeParam2, dwColor);

            D3DXVECTOR3 v1 = D3DXVECTOR3(0.f,  0.f, 0.f);
            D3DXVECTOR3 v2 = D3DXVECTOR3(0.f, 50.f, 0.f);
            D3DXVECTOR3 v3 = D3DXVECTOR3(-10.f, 43.f, 0.f);
            D3DXVECTOR3 v4 = D3DXVECTOR3( 10.f, 43.f, 0.f);

            g_cGraphicsTool.DrawLine(&v1, &v2, 0xFFAAAAAA, 0xFFAAAAAA);
            g_cGraphicsTool.DrawLine(&v3, &v2, 0xFFAAAAAA, 0xFFAAAAAA);
            g_cGraphicsTool.DrawLine(&v4, &v2, 0xFFAAAAAA, 0xFFAAAAAA);
        }
        break;
    case SFX_LS_SPHERE :
        {
            float fParam1 = m_fLauncherShapeParam1;
            if (fParam1 <= 0.f)
                fParam1  = 0.5f;


            for (float i = 0.f; i < D3DX_PI * 2; i += D3DX_PI / 5)
            {
                g_cGraphicsTool.DrawCircle(
                    D3DXVECTOR3(0.f, 0.f, 0.f), 
                    D3DXVECTOR3(sinf(i), 0.f, cosf(i)), 
                    fParam1, 
                    dwColor
                    );
            }

            for (float i = -fParam1; i <= fParam1; i += fParam1 / 5)
            {
                g_cGraphicsTool.DrawCircle(
                    D3DXVECTOR3(0.f, i, 0.f), 
                    D3DXVECTOR3(0.f, 1.f, 0.f), 
                    sqrtf(fParam1 * fParam1 - i * i),
                    dwColor
                    );
            }
        }
        break;
    case SFX_LS_CYLINDER :
        {
            g_cGraphicsTool.DrawCircle(
                D3DXVECTOR3(0.f, -fHalfParam2, 0.f), 
                D3DXVECTOR3(0.f, 1.f, 0.f), 
                m_fLauncherShapeParam1, 
                dwColor
                );
            g_cGraphicsTool.DrawCircle(
                D3DXVECTOR3(0.f, fHalfParam2, 0.f), 
                D3DXVECTOR3(0.f, 1.f, 0.f), 
                m_fLauncherShapeParam1, 
                dwColor
                );
            for (float i = 0.f; i < D3DX_PI * 2; i += D3DX_PI / 5)
            {
                D3DXVECTOR3 s = D3DXVECTOR3(sinf(i) * m_fLauncherShapeParam1, -fHalfParam2, cosf(i) * m_fLauncherShapeParam1);
                D3DXVECTOR3 d = D3DXVECTOR3(sinf(i) * m_fLauncherShapeParam1,  fHalfParam2, cosf(i) * m_fLauncherShapeParam1);
                g_cGraphicsTool.DrawLine(&s, &d, dwColor, dwColor);
            }
            
            if (fHalfParam2 <= 0.f)
                fHalfParam2 = 0.5f;

            for (float i = -fHalfParam2; i <= fHalfParam2; i += fHalfParam2 / 5)
            {
                g_cGraphicsTool.DrawCircle(
                    D3DXVECTOR3(0.f, i, 0.f), 
                    D3DXVECTOR3(0.f, 1.f, 0.f), 
                    m_fLauncherShapeParam1,
                    dwColor
                    );
            }
        }
        break;
    default :
        ASSERT(false);
        break;
    }

    g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorldSave);

    return S_OK;
}

void KG3DSFXParticleLauncher::GetParticleDirectionAndPos(float		  fScatterMin,
                                                         float		  fScatterMax,
                                                         D3DXVECTOR3 *pvDirection,
                                                         D3DXVECTOR3 *pvPosition
)
{
    switch (m_eLauncherShape)
    {
    case SFX_LS_SPHERE:
        {
            pvPosition->x = GetRandom(-1, 1);
            pvPosition->y = GetRandom(-1, 1);
            pvPosition->z = GetRandom(-1, 1);
            D3DXVec3Normalize(pvPosition, pvPosition);
            (*pvDirection) = (*pvPosition);
            pvPosition->x *= m_fLauncherShapeParam1;
            pvPosition->y *= m_fLauncherShapeParam1;
            pvPosition->z *= m_fLauncherShapeParam1;
            D3DXMATRIX matWorld = *(m_pParentSFX->GetCurrentWorldMatrix());
            matWorld._41 = matWorld._42 = matWorld._43 = 0;
            matWorld = m_Matrix * matWorld;
            D3DXVec3TransformNormal(pvDirection, pvDirection, &matWorld);
            D3DXVec3TransformCoord(pvPosition, pvPosition, &m_Matrix);
        }
        break;
    case SFX_LS_RECTANGLE:
    case SFX_LS_CIRCLE:
        {
            if (m_eLauncherShape == SFX_LS_RECTANGLE)
            {
                float fHalfWidth = m_fLauncherShapeParam1 * 0.5f;
                float fHalfHeight = m_fLauncherShapeParam2 * 0.5f;
                pvPosition->x = GetRandom(-fHalfWidth, fHalfWidth);
                pvPosition->y = 0;
                pvPosition->z = GetRandom(-fHalfHeight, fHalfHeight);

                ASSERT(pvPosition->x >= -fHalfWidth);
                ASSERT(pvPosition->x <= fHalfWidth);
                ASSERT(pvPosition->z >= -fHalfHeight);
                ASSERT(pvPosition->z <= fHalfHeight);
            }
            else if (m_eLauncherShape == SFX_LS_CIRCLE)
            {
				float fRadiusB  = m_fLauncherShapeParam1;
				float fRadiusS	= m_fLauncherShapeParam2;

				if (fRadiusS > fRadiusB)
					fRadiusS = 0.0f;
				if (fRadiusB <=  0)
					fRadiusB  = 100.0f;
				if (fRadiusS < 0)
					fRadiusS  = 0;

				D3DXVECTOR2 vPos = GetRandVec2D();
				float fLen = GetRandom(fRadiusS, fRadiusB);

				vPos = vPos * fLen;

				pvPosition->x = vPos.x;
				pvPosition->z = vPos.y;
				pvPosition->y = 0.0f;
            }
            
            D3DXVec3TransformCoord(pvPosition, pvPosition,
                &m_Matrix
            );

            D3DXMATRIX mat;
            (*pvDirection).x = 0;
            (*pvDirection).y = 1;
            (*pvDirection).z = 0;
            float fScatter = GetRandom(fScatterMin, fScatterMax);
            float Yaw = GetRandom(-D3DX_PI * 0.5f, D3DX_PI * 0.5f);  // y
            float Pitch = GetRandomSgn() * fScatter;          // x
            float Roll = 0;    // z
            D3DXMatrixRotationYawPitchRoll( &mat, Yaw, Pitch, Roll );
            mat = mat * m_Matrix;
            mat._41 = mat._42 = mat._43 = 0;
            D3DXMATRIX matWorld = *(m_pParentSFX->GetCurrentWorldMatrix());
            matWorld._41 = matWorld._42 = matWorld._43 = 0;
            mat *=  matWorld;
            D3DXVec3TransformNormal( pvDirection, pvDirection, &mat );
            D3DXVec3Normalize(pvDirection, pvDirection);
        }
        break;

    case SFX_LS_CYLINDER:
        {
			float fRadius = m_fLauncherShapeParam1;
			float fHalfHg = m_fLauncherShapeParam2 * 0.5f;

			D3DXVECTOR2 vDirection = GetRandVec2D();
			pvDirection->x = vDirection.x;
			pvDirection->z = vDirection.y;
			pvDirection->y = 0.0f;

			vDirection     = vDirection * fRadius;
			pvPosition->y  = GetRandom(-fHalfHg, fHalfHg);
			pvPosition->x  = vDirection.x;
			pvPosition->z  = vDirection.y;

            D3DXMATRIX matWorld = *(m_pParentSFX->GetCurrentWorldMatrix());
            matWorld._41 = matWorld._42 = matWorld._43 = 0;
            matWorld = m_Matrix * matWorld;
			D3DXVec3TransformNormal(pvDirection, pvDirection, &matWorld);
			D3DXVec3TransformCoord(pvPosition, pvPosition, &m_Matrix);
			D3DXVec3Normalize(pvDirection, pvDirection);
        }
        break;

    default:
        {
            ASSERT(0);
            (*pvPosition).x = 0;
            (*pvPosition).y = 0;
            (*pvPosition).z = 0;

            (*pvDirection).x = 0;
            (*pvDirection).y = 1;
            (*pvDirection).z = 0;
        }
        break;
    }
}



void KG3DSFXParticleLauncher::MakeParticle(
    float fLife, 
    D3DXVECTOR3 *pvPosition,
    D3DXVECTOR3 *pvVelocity,
    float fFrame
)
{
    if( m_pParticleLifeTimeLine == NULL )
        return;

    KG3DSFXParticle* pParticle = m_pParticleContainer->NewParticle(m_pParticleLifeTimeLine->GetTextureIndex(), 
        m_eBlendMode, m_pParticleLifeTimeLine->m_wType);

    ASSERT(pParticle);

    if (m_dwBindTrackIndex != SFX_NOT_BIND_TO_TRACK &&
        m_eBindToTrackMode == SFX_LBTTM_PARTICLE_TO_TRACK)
    {
        KG3DSFXTrack *pTrack = NULL;
        m_pParentSFX->GetTrack(m_dwBindTrackIndex, &pTrack);
        ASSERT(pTrack);

        D3DXMATRIX mat;
        pTrack->GetTransform(0, &mat);
        pvPosition->x += mat._41;
        pvPosition->y += mat._42;
        pvPosition->z += mat._43;
        pParticle->OnLaunch(
            m_pParentSFX,
            m_pParticleLifeTimeLine,
            m_eParticleType,
            m_eParticleUpdateMode/*SFX_PUM_SELF*/,
            m_eBlendMode,
            m_dwBindTrackIndex,
            fLife,
            pvPosition,
            pvVelocity,
            fFrame,
			m_dwRandTexDelay,
            this
        );
    }
    else
    {
        pParticle->OnLaunch(
            m_pParentSFX,
            m_pParticleLifeTimeLine,
            m_eParticleType,
            m_eParticleUpdateMode/*SFX_PUM_SELF*/,
            m_eBlendMode,
            0xffffffff,
            fLife,
            pvPosition,
            pvVelocity,
            fFrame,
			m_dwRandTexDelay,
            this
        );
    }

    //m_pParticleContainer->AddRectangleParticle(pParticle);
}



void KG3DSFXParticleLauncher::SetParticleLifeTimeLine(
    IEKG3DSFXParticleLifeTimeLine* pParticleLifeTimeLine )
{
    m_pParticleLifeTimeLine = static_cast<KG3DSFXParticleLifeTimeLine*>(pParticleLifeTimeLine);
}

IEKG3DSFXParticleLifeTimeLine* KG3DSFXParticleLauncher::GetParticleLifeTime()
{
    return (IEKG3DSFXParticleLifeTimeLine*)m_pParticleLifeTimeLine;
}

HRESULT KG3DSFXParticleLauncher::BindToTrack(DWORD dwIndex)
{
    if (dwIndex < m_pParentSFX->GetTrackNum())
    {
        m_dwBindTrackIndex = dwIndex;
        return S_OK;
    }

    return E_FAIL;
}

HRESULT KG3DSFXParticleLauncher::UnBindToTrack()
{
    m_dwBindTrackIndex = SFX_NOT_BIND_TO_TRACK;
    return S_OK;
}


BOOL KG3DSFXParticleLauncher::IsBindToTrack() const
{
    return m_dwBindTrackIndex != SFX_NOT_BIND_TO_TRACK;
}

DWORD KG3DSFXParticleLauncher::GetBindTrackIndex() const
{
    return m_dwBindTrackIndex;
}


HRESULT KG3DSFXParticleLauncher::AddEffForce(IEKG3DSFXParticleForceField* pForce)
{
    size_t uForceNum = m_vecEffForces.size();

    for (size_t i = 0; i < uForceNum; i++)
    {
        if (m_vecEffForces[i] == pForce)
            return E_FAIL;
    }

    m_vecEffForces.push_back(pForce);

    return S_OK;
}

BOOL KG3DSFXParticleLauncher::HasEffForce(IEKG3DSFXParticleForceField* pForce)
{
    size_t uForceNum = m_vecEffForces.size();

    for (size_t i = 0; i < uForceNum; i++)
    {
        if (m_vecEffForces[i] == pForce)
            return TRUE; 
    }
    return FALSE;
}


HRESULT KG3DSFXParticleLauncher::DelEffForce(IEKG3DSFXParticleForceField* pForce)
{
    vector<IEKG3DSFXParticleForceField*>::iterator i    = m_vecEffForces.begin();
    vector<IEKG3DSFXParticleForceField*>::iterator del  = m_vecEffForces.end();

    while (i != m_vecEffForces.end())
    {
        if (*i == pForce)
        {
            del = i;
            break;
        }
        i++;
    }
    
    if (del == m_vecEffForces.end())
        return E_FAIL;

    m_vecEffForces.erase(del);

    return S_OK;
}

HRESULT KG3DSFXParticleLauncher::Clone(KG3DSFXParticleLauncher **ppLauncher)
{
    ASSERT(ppLauncher);

    m_pParentSFX->AddParticleLauncher(m_eParticleType, m_eLauncherShape, m_eParticleUpdateMode, ppLauncher);

    (*ppLauncher)->m_bSpeedInherit         = m_bSpeedInherit;
    (*ppLauncher)->m_fLauncherShapeParam1  = m_fLauncherShapeParam1;
    (*ppLauncher)->m_fLauncherShapeParam2  = m_fLauncherShapeParam2;
    (*ppLauncher)->m_pParticleLifeTimeLine = m_pParticleLifeTimeLine;
    (*ppLauncher)->m_fParticleNumLine      = m_fParticleNumLine;
    (*ppLauncher)->m_fParticleLifeMaxLine  = m_fParticleLifeMaxLine;
    (*ppLauncher)->m_fParticleLifeMinLine  = m_fParticleLifeMinLine;
    (*ppLauncher)->m_fParticleScatterMin   = m_fParticleScatterMin;
    (*ppLauncher)->m_fParticleScatterMax   = m_fParticleScatterMax;
    (*ppLauncher)->m_fParticleSpeedMaxLine = m_fParticleSpeedMaxLine;
    (*ppLauncher)->m_fParticleSpeedMinLine = m_fParticleSpeedMinLine;

    (*ppLauncher)->m_RtsTimeLine.m_TranslationTimeLine = m_RtsTimeLine.m_TranslationTimeLine;
    (*ppLauncher)->m_RtsTimeLine.m_RotationTimeLine    = m_RtsTimeLine.m_RotationTimeLine;
    (*ppLauncher)->m_RtsTimeLine.m_ScalingTimeLine     = m_RtsTimeLine.m_ScalingTimeLine;
    (*ppLauncher)->m_RtsTimeLine.m_ScalRotationTimLine     = m_RtsTimeLine.m_ScalRotationTimLine;

    (*ppLauncher)->m_vecEffForces    = m_vecEffForces;
    (*ppLauncher)->m_vecEffForcesIDs = m_vecEffForcesIDs;
    (*ppLauncher)->m_eBlendMode      = m_eBlendMode;
    (*ppLauncher)->m_dwForceOption   = m_dwForceOption;
    (*ppLauncher)->m_dwMotionType    = m_dwMotionType;
    (*ppLauncher)->m_dwRandAlpDelay  = m_dwRandAlpDelay;
    (*ppLauncher)->m_dwRandColDelay  = m_dwRandColDelay;
    (*ppLauncher)->m_dwRandForceRotDelay  = m_dwRandForceRotDelay;
    (*ppLauncher)->m_dwRandRotDelay  = m_dwRandRotDelay;
    (*ppLauncher)->m_dwRandTexDelay  = m_dwRandTexDelay;

    size_t uEffForceNum = m_vecEffForces.size();
    for (size_t i = 0; i < uEffForceNum; i++)
        (*ppLauncher)->m_vecEffForces[i]->AddLauncherRef(*ppLauncher);

    return S_OK;
}

//BOOL KG3DSFXParticleLauncher::IsRayIntersect(D3DXVECTOR3 &Inter, D3DXVECTOR3 Origin, D3DXVECTOR3 Direction)
//{
//    // this function only be called when ray is through its AABB.
//    // so always return TRUE.
//    Inter = Origin;
//    return TRUE;
//}
