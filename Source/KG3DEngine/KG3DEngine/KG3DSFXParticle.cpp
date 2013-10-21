////////////////////////////////////////////////////////////////////////////////
//
//  Collator : Chiyer (Zhao XinYu)
//  Date     : 2007-2-12 17:14:16
//
////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "KG3DSFXParticle.h"
#include "KG3DCamera.h"
#include "KG3DGraphicsTool.h"
#include "KG3DSFXParticleContainer.h"
#include "KG3DSFX.h"


#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


KG3DSFXParticle::KG3DSFXParticle(UINT uIndex)
{
    m_vPosition			= D3DXVECTOR3(0.f, 0.f, 0.f);
    m_vVelocity			= D3DXVECTOR3(0.f, 0.f, 0.f);
    m_vPreVelocity      = D3DXVECTOR3(0.f, 0.f, 0.f);
    m_fWidth			= 0.f;
    m_fHeight			= 0.f;
	m_fRotation			= 0.f;
    m_dwColor			= 0xFFFFFFFF;
    m_fLife				= 0;
    m_fPreFrame			= 0;
    m_fStartFrame		= 0;
    m_vPositionWorld    = m_vPosition;
    m_fRotationStep     = 0.f;
    m_fVelocity         = 0.f;

    m_eParticleUpdateMode	= SFX_PUM_SELF;
    m_eParticleType			= SFX_PT_BILLBOARD_POINT;
    m_eBlendMode			= SFX_BM_SRCALPHA_INVSRCALPHA;

    m_pLifeTimeLine		= NULL;
    m_pParentSFX		= NULL;

    m_uPoolIndex		= uIndex;
    m_dwTrackIndex      = 0xffffffff;

    m_pLauncher         = NULL;

    m_fRotationX		= GetRandom(0.f, 2 * 3.1415f);
    m_fRotationY		= GetRandom(0.f, 2 * 3.1415f);
    m_fRotationZ		= GetRandom(0.f, 2 * 3.1415f);
    m_vRotStep          = D3DXVECTOR3(m_fRotationX, m_fRotationY, m_fRotationZ);


    m_dwRandomTextureFrame = (DWORD)-1;

	m_dwRandTexDelay	= 100;
	m_dwLastRandTime	= 0;
    m_dwRandColDelay	= 100;
    m_dwLastRandColTime	= 0;
    m_dwRandRotDelay	= 100;
    m_dwLastRandRotTime	= 0;
    m_dwRandAlpDelay	= 0;
    m_dwLastRandAlpTime	= 0;
    m_dwRandFcRotDelay	= 0;
    m_dwLastRandFcRotTime = 0;
    m_fLastRorceFactor  = 0.f;
}


KG3DSFXParticle::~KG3DSFXParticle()
{
}


HRESULT KG3DSFXParticle::OnLaunch(  KG3DSFX*                     pParentSFX,
                                    KG3DSFXParticleLifeTimeLine* pLifeTimeLine,
                                    SFX_PARTICLE_TYPE            eParticleType,
                                    SFX_PARTICLE_UPDATE_MODE     eParticleUpdateMode,
                                    SFX_BLEND_MODE               eBlendMode,
                                    DWORD                        dwTrackIndex, 
                                    float                        fLife,
                                    D3DXVECTOR3*                 pvPosition,
                                    D3DXVECTOR3*                 pvVelocity,
                                    float                        fFrame,
	                                DWORD						 dwRandTexDelay,
                                    KG3DSFXParticleLauncher*     pLauncher
)
{
    ASSERT(pLifeTimeLine);
    ASSERT(pParentSFX);
	ASSERT(pvVelocity);
	ASSERT(pvPosition);
    ASSERT(pLauncher);

    m_eParticleType         = eParticleType;
    m_pParentSFX	        = pParentSFX;
    m_pLifeTimeLine         = pLifeTimeLine;
    m_fLife			        = fLife;
    m_fPreFrame		        = fFrame;
    m_eParticleUpdateMode   = eParticleUpdateMode;
	m_dwRandTexDelay	    = dwRandTexDelay;
    m_fStartFrame	        = fFrame;
    m_eBlendMode	        = eBlendMode;
    m_dwTrackIndex          = dwTrackIndex;
    m_pLauncher             = pLauncher;
    m_dwLastRandTime        = 0;
    m_dwLastRandColTime     = 0;
    m_dwLastRandRotTime     = 0;
    m_dwLastRandAlpTime     = 0;
    m_dwLastRandFcRotTime   = 0;
    m_vPositionWorld        = *pvPosition;
    m_dwLastRandColTime     = 0;
    m_dwRandColDelay        = pLauncher->GetRandColDelay();
    m_dwRandRotDelay        = pLauncher->GetRandRotDelay();
    m_dwRandAlpDelay        = pLauncher->GetRandAlpDelay();
    m_dwRandFcRotDelay      = pLauncher->GetRandForceRotDelay();
    m_fRotationStep         = 0.f;
    m_fRotation			    = 0.f;
    m_fLastRorceFactor      = 0.f;

    switch (pLauncher->m_dwMotionType)
    {
    case SFX_MOTION_WROLD :
        {
            m_vPosition  = (*pvPosition);
            m_vVelocity  = (*pvVelocity);
        }
        break;
    case SFX_MOTION_SFX :
        {
            D3DXMATRIX matSfxInv;
            D3DXMatrixInverse(&matSfxInv, NULL, pParentSFX->GetCurrentWorldMatrix());
            
            D3DXVec3TransformCoord(&m_vPosition, pvPosition, &matSfxInv);
            D3DXVec3TransformNormal(&m_vVelocity, pvVelocity, &matSfxInv);
        }
        break;
    case SFX_MOTION_LAUCHER :
        {
            D3DXMATRIX matlauInv;
            D3DXMatrixInverse(&matlauInv, NULL, &pLauncher->m_matWorld);
            D3DXVec3TransformCoord(&m_vPosition, pvPosition, &matlauInv);
            D3DXVec3TransformNormal(&m_vVelocity, pvVelocity, &matlauInv);
        }
        break;
    default :
        ASSERT(false);
        break;
    }

    float fHScal =  m_pParentSFX->m_Srt.vScanl.y;
    float fWScal = (m_pParentSFX->m_Srt.vScanl.x + m_pParentSFX->m_Srt.vScanl.z) * 0.5f;

    switch (eParticleUpdateMode)
    {
    case SFX_PUM_FOLLOW_SFX :
        {
            D3DXCOLOR fColorL;
            D3DXCOLOR fColorR;
            D3DXCOLOR fColorC;

            m_pLifeTimeLine->m_ColorTimeLineL.GetData(&fColorL, fFrame);
            m_pLifeTimeLine->m_ColorTimeLineR.GetData(&fColorR, fFrame);

            /*
            fColorC.a = GetRandom(fColorL.a, fColorR.a);
            fColorC.r = GetRandom(fColorL.r, fColorR.r);
            fColorC.g = GetRandom(fColorL.g, fColorR.g);
            fColorC.b = GetRandom(fColorL.b, fColorR.b);
            */

            fColorC = Random(fColorL, fColorR);

            m_dwColor = (DWORD)fColorC;
   
            m_pLifeTimeLine->m_fWidthTimeLine.GetData(&m_fWidth, fFrame);
            m_pLifeTimeLine->m_fHeightTimeLine.GetData(&m_fHeight, fFrame);
            
		    m_fWidth  *=  fWScal;
            m_fHeight *=  fHScal;

        }
        break;
    case SFX_PUM_SELF :
        {
            D3DXCOLOR fColorL;
            D3DXCOLOR fColorR;
            D3DXCOLOR fColorC;
    
            m_pLifeTimeLine->m_ColorTimeLineL.GetData(&fColorL, 0);
            m_pLifeTimeLine->m_ColorTimeLineR.GetData(&fColorR, 0);

            /*
            fColorC.a = GetRandom(fColorL.a, fColorR.a);
            fColorC.r = GetRandom(fColorL.r, fColorR.r);
            fColorC.g = GetRandom(fColorL.g, fColorR.g);
            fColorC.b = GetRandom(fColorL.b, fColorR.b);
            */

            fColorC = Random(fColorL, fColorR);

            m_dwColor = (DWORD)fColorC;


            m_pLifeTimeLine->m_fWidthTimeLine.GetData(&m_fWidth, 0);
            m_pLifeTimeLine->m_fHeightTimeLine.GetData(&m_fHeight, 0);
            
		    m_fWidth  *= fWScal;
            m_fHeight *= fHScal;

        }
        break;
    default :
        break;
    }

    m_fVelocity = D3DXVec3Length(&m_vVelocity);
    m_vPreVelocity = m_vVelocity;

    return S_OK;
}

void KG3DSFXParticle::GetWorldPos(D3DXVECTOR3* pWorldPos, const D3DXVECTOR3& vLocalPos)
{
    ASSERT(m_pLauncher);
    ASSERT(pWorldPos);

    switch (m_pLauncher->m_dwMotionType)
    {
    case SFX_MOTION_LAUCHER :
        D3DXVec3TransformCoord(pWorldPos, &vLocalPos, &m_pLauncher->m_matWorld);
        break;
    case SFX_MOTION_SFX :
        D3DXVec3TransformCoord(pWorldPos, &vLocalPos, m_pParentSFX->GetCurrentWorldMatrix());
        break;
    case SFX_MOTION_WROLD :
        (*pWorldPos) = vLocalPos;
        break;
    default :
        break;
    }
}


HRESULT KG3DSFXParticle::Update(float fCurrentFrame)
{
    ASSERT(m_pParentSFX);
    ASSERT(m_pLauncher);

    /*
     * calc the scaling 
     */

    float fHScal =  m_pParentSFX->m_Srt.vScanl.y;
    float fWScal = (m_pParentSFX->m_Srt.vScanl.x + m_pParentSFX->m_Srt.vScanl.z) * 0.5f;


    /* 
     * calc the frame 
     */

    if (fCurrentFrame < m_fPreFrame)
    {
        fCurrentFrame += (int)(m_fPreFrame / m_pParentSFX->GetTotalFrameNum()) * m_pParentSFX->GetTotalFrameNum();
        if (fCurrentFrame < m_fPreFrame)
            fCurrentFrame += m_pParentSFX->GetTotalFrameNum();
    }
    float fElapsedFrame = fCurrentFrame - m_fPreFrame;


    ASSERT(fElapsedFrame >= 0.f);
    if (fElapsedFrame < 0.f)
        return S_OK;

    m_vPrevPosition = m_vPosition;


    /* 
     * update track data, if the particle be bind to track  
     * it no be apply by the force
     */

    if (m_dwTrackIndex != 0xffffffff)
    {
        KG3DSFXTrack* pTrack = NULL;
        D3DXMATRIX mat;
        m_vVelocity.y += (m_vVelocity.x * fElapsedFrame);

        m_pParentSFX->GetTrack(m_dwTrackIndex, &pTrack);
        if (pTrack)
        {
            pTrack->GetTransform(m_vVelocity.y, &mat);
            mat *= (*m_pParentSFX->GetCurrentWorldMatrix());
            m_vPosition.x = mat._41;
            m_vPosition.y = mat._42;
            m_vPosition.z = mat._43;
        }
        m_vPositionWorld = m_vPosition;
    }
    else
    {
        /*
         * apply the force and update velocity
         */

        DWORD dwNowTime = g_cGraphicsTool.GetNowTime();
        if (dwNowTime - m_dwLastRandFcRotTime > m_dwRandFcRotDelay || m_dwLastRandFcRotTime == 0)
        {
            m_fLastRorceFactor = KG3DSFXParticleForceField::FORCT_ROTATION_MAX + 100;
            m_dwLastRandFcRotTime = dwNowTime;
        }

        KG3DSFXParticleForceField* pForceField = NULL;
        DWORD dwForceFieldNum = m_pParentSFX->GetForceFieldNum();

        for (int i = 0; i < (int)dwForceFieldNum; i++)
        {
            m_pParentSFX->GetForceField(i, (IEKG3DSFXParticleForceField**)&pForceField);   
            ASSERT(pForceField);
            if (m_pParentSFX->IsLauncherBeing(m_pLauncher) && m_pLauncher->HasEffForce(pForceField))
                pForceField->ApplyForce(&m_vVelocity, m_fPreFrame, fCurrentFrame, 
                    &m_vPosition, m_pLauncher->m_dwForceOption, &m_fLastRorceFactor
                 );
        }

        
        /*
         * update particle's position
         */

        m_vPosition += (m_vVelocity * fElapsedFrame);
        GetWorldPos(&m_vPositionWorld, m_vPosition);
    }


    m_fPreFrame += fElapsedFrame;


    if (m_eParticleUpdateMode == SFX_PUM_FOLLOW_SFX)
    {
        m_fLife -= fElapsedFrame;
        if (m_fLife <= 0.0f)
            return E_FAIL;   

        D3DXCOLOR fColorL;
        D3DXCOLOR fColorR;
        D3DXCOLOR fColorC;

        m_pLifeTimeLine->m_ColorTimeLineL.GetData(&fColorL, fCurrentFrame);
        m_pLifeTimeLine->m_ColorTimeLineR.GetData(&fColorR, fCurrentFrame);

        /*fColorC.a = GetRandom(fColorL.a, fColorR.a);
        fColorC.r = GetRandom(fColorL.r, fColorR.r);
        fColorC.g = GetRandom(fColorL.g, fColorR.g);
        fColorC.b = GetRandom(fColorL.b, fColorR.b); */

        fColorC = Random(fColorL, fColorR);

        float a = fColorC.a;

        DWORD dwNowTime = g_cGraphicsTool.GetNowTime();
        if (dwNowTime - m_dwLastRandColTime > m_dwRandColDelay || m_dwLastRandColTime == 0)
        {
            fColorC.a = (m_dwColor >> 24 & 0xff) / 255.f;
            m_dwColor = (DWORD)fColorC;
            m_dwLastRandColTime = dwNowTime;
        }

        if (dwNowTime - m_dwLastRandAlpTime > m_dwRandAlpDelay || m_dwLastRandAlpTime == 0)
        {
            m_dwColor &= 0x00ffffff;
            m_dwColor |= ((DWORD)(a * 255)) << 24;
            m_dwLastRandAlpTime = dwNowTime;
        }

        m_pLifeTimeLine->m_fWidthTimeLine.GetData(&m_fWidth, fCurrentFrame);
        m_pLifeTimeLine->m_fHeightTimeLine.GetData(&m_fHeight, fCurrentFrame);
        m_fWidth  *= fWScal;
        m_fHeight *= fHScal;

        if (dwNowTime - m_dwLastRandRotTime > m_dwRandRotDelay || m_dwLastRandRotTime == 0)
        {
            float fRotationMin;
            float fRotationMax;
            m_pLifeTimeLine->m_fRotationTimeLineMin.GetData(&fRotationMin, fCurrentFrame);
            m_pLifeTimeLine->m_fRotationTimeLineMax.GetData(&fRotationMax, fCurrentFrame);
            m_fRotationStep = GetRandom(fRotationMin, fRotationMax);
            m_vRotStep = D3DXVECTOR3(
                GetRandom(-m_fRotationStep, m_fRotationStep),
                GetRandom(-m_fRotationStep, m_fRotationStep),
                GetRandom(-m_fRotationStep, m_fRotationStep)
                );
            m_dwLastRandRotTime = dwNowTime;
        }
        m_fRotation += m_fRotationStep * fElapsedFrame;
        m_fRotationX += m_vRotStep.x * fElapsedFrame;
        m_fRotationY += m_vRotStep.y * fElapsedFrame;
        m_fRotationZ += m_vRotStep.z * fElapsedFrame;
    }
    else if (m_eParticleUpdateMode == SFX_PUM_SELF)
    {
        m_fLife -= fElapsedFrame;
        if (m_fLife <= 0.0f)
            return E_FAIL;    

        float fCurFrame = fCurrentFrame - m_fStartFrame;

        ASSERT(fCurFrame >= 0);

        D3DXCOLOR fColorL;
        D3DXCOLOR fColorR;
        D3DXCOLOR fColorC;

        m_pLifeTimeLine->m_ColorTimeLineL.GetData(&fColorL, fCurFrame);
        m_pLifeTimeLine->m_ColorTimeLineR.GetData(&fColorR, fCurFrame);

        //float f = GetRandom(0.f, 1.f);
        //fColorC = fColorL * f + fColorR * (1 - f);

        //fColorC.a = //GetRandom(fColorL.a, fColorR.a);
        //fColorC.r = //GetRandom(fColorL.r, fColorR.r);
        //fColorC.g = //GetRandom(fColorL.g, fColorR.g);
        //fColorC.b = //GetRandom(fColorL.b, fColorR.b);

        fColorC = Random(fColorL, fColorR);

        float a = fColorC.a;

        DWORD dwNowTime = g_cGraphicsTool.GetNowTime();
        if (dwNowTime - m_dwLastRandColTime > m_dwRandColDelay || m_dwLastRandColTime == 0)
        {
            fColorC.a = (m_dwColor >> 24 & 0xff) / 255.f;
            m_dwColor = (DWORD)fColorC;
            m_dwLastRandColTime = dwNowTime;
        }

        if (dwNowTime - m_dwLastRandAlpTime > m_dwRandAlpDelay || m_dwLastRandAlpTime == 0)
        {
            m_dwColor &= 0x00ffffff;
            m_dwColor |= ((DWORD)(a * 255)) << 24;
            m_dwLastRandAlpTime = dwNowTime;
        }

        m_pLifeTimeLine->m_fWidthTimeLine.GetData(&m_fWidth, fCurFrame);
        m_pLifeTimeLine->m_fHeightTimeLine.GetData(&m_fHeight, fCurFrame);
        m_fWidth  *= fWScal;
        m_fHeight *= fHScal;

        if (dwNowTime - m_dwLastRandRotTime > m_dwRandRotDelay || m_dwLastRandRotTime == 0)
        {
            float fRotationMin;
            float fRotationMax;
            m_pLifeTimeLine->m_fRotationTimeLineMin.GetData(&fRotationMin, fCurFrame);
            m_pLifeTimeLine->m_fRotationTimeLineMax.GetData(&fRotationMax, fCurFrame);
            m_fRotationStep = GetRandom(fRotationMin, fRotationMax);
            m_vRotStep = D3DXVECTOR3(
                GetRandom(-m_fRotationStep, m_fRotationStep),
                GetRandom(-m_fRotationStep, m_fRotationStep),
                GetRandom(-m_fRotationStep, m_fRotationStep)
                );
            m_dwLastRandRotTime = dwNowTime;
        }
        m_fRotation += m_fRotationStep * fElapsedFrame;
        m_fRotationX += m_vRotStep.x * fElapsedFrame;
        m_fRotationY += m_vRotStep.y * fElapsedFrame;
        m_fRotationZ += m_vRotStep.z * fElapsedFrame;
    }
   
    return S_OK;
}


HRESULT KG3DSFXParticle::GetRectInfo(D3DXVECTOR3* pv1, 
                                     D3DXVECTOR3* pv2, 
                                     D3DXVECTOR3* pv3, 
                                     D3DXVECTOR3* pv4, 
                                     FRECT*       pRectfText,
                                     const D3DXVECTOR3& vWorldPos
                                     )
{
    KG3DCamera* pCamera = g_cGraphicsTool.GetCamera();

    ASSERT(pCamera);
    if (!pCamera)
        return E_FAIL;

    float HalfWidth  = m_fWidth  * 0.5f;
    float HalfHeight = m_fHeight * 0.5f;

    switch (m_eParticleType)
    {
    case SFX_PT_BILLBOARD_POINT:
        {
            D3DXVECTOR3 vRight = pCamera->GetCameraRight();
            D3DXVECTOR3 vUp = pCamera->GetCameraUp();

            if (m_fRotation != 0.f)
            {
                D3DXVECTOR3 vFront = pCamera->GetCameraFront();
			    D3DXMATRIX	matRotation;

			    D3DXMatrixRotationAxis(&matRotation, &vFront, -m_fRotation);
			    D3DXVec3TransformNormal(&vRight, &vRight, &matRotation);
			    D3DXVec3TransformNormal(&vUp, &vUp, &matRotation);
            }

            /*
			D3DXVec3Normalize(&vRight, &vRight);
			D3DXVec3Normalize(&vUp, &vUp);
            */

            vRight *= HalfWidth;
            vUp *= HalfHeight;

            (*pv1) = vWorldPos - vRight + vUp;
            (*pv2) = vWorldPos + vRight + vUp;
            (*pv3) = vWorldPos + vRight - vUp;
            (*pv4) = vWorldPos - vRight - vUp;
        }
        break;

    case SFX_PT_BILLBOARD_ORIENTSELF:
        {
            /* 
             * The normal of the particle surface should be on the surface
             * which constructed by crossing product speed vector and
             * view direction vector.
             */

            D3DXVECTOR3 vecEye;
            D3DXVECTOR3 vecWidth;
            D3DXVECTOR3 vecVelocity = m_vPosition - m_vPrevPosition;
            float fVelocity = D3DXVec3Length(&vecVelocity);
            
            if (fVelocity < 0.0001)
                vecVelocity = m_vPreVelocity;
            else
                m_vPreVelocity = vecVelocity;
            

            switch (m_pLauncher->m_dwMotionType)
            {
            case SFX_MOTION_LAUCHER :
                D3DXVec3TransformNormal(&vecVelocity, &vecVelocity, &m_pLauncher->m_matWorld);
                break;
            case SFX_MOTION_SFX :
                D3DXVec3TransformNormal(&vecVelocity, &vecVelocity, m_pParentSFX->GetCurrentWorldMatrix());
                break;
            case SFX_MOTION_WROLD :
                break;
            default :
                break;
            }

            D3DXVec3Normalize(&vecVelocity, &vecVelocity);
            
			pCamera->GetPosition(&vecEye);

            D3DXVECTOR3 vecEyeDirection = vecEye - vWorldPos;
            D3DXVec3Cross(&vecWidth, &vecEyeDirection, &vecVelocity);
            D3DXVec3Normalize(&vecWidth, &vecWidth);

            (*pv1) = vWorldPos - vecWidth * HalfWidth + vecVelocity * HalfHeight;
            (*pv2) = vWorldPos + vecWidth * HalfWidth + vecVelocity * HalfHeight;
            (*pv3) = vWorldPos + vecWidth * HalfWidth - vecVelocity * HalfHeight;
            (*pv4) = vWorldPos - vecWidth * HalfWidth - vecVelocity * HalfHeight;
        }
        break;

    case SFX_PT_BILLBOARD_LOCK_WORLD_Y :
    case SFX_PT_BILLBOARD_LOCK_LOCAL_Y :
        {
            D3DXVECTOR3 vRight = pCamera->GetCameraRight();
            D3DXVECTOR3 vUp = D3DXVECTOR3(0.f, 1.f, 0.f); 
            D3DXVECTOR3 vFront = pCamera->GetCameraFront();

            vFront.y = 0.f;
            D3DXVec3Normalize(&vFront, &vFront);

            // rotation
            D3DXMATRIX	matRotation;
            D3DXMatrixRotationAxis(&matRotation, &vFront, -m_fRotation);

            if (m_eParticleType == SFX_PT_BILLBOARD_LOCK_LOCAL_Y)
            {
                matRotation *= m_pLauncher->m_matWorld;
            }

            D3DXVec3TransformNormal(&vRight, &vRight, &matRotation);
            D3DXVec3TransformNormal(&vUp, &vUp, &matRotation);

            /*
            D3DXVec3Normalize(&vRight, &vRight);
            D3DXVec3Normalize(&vUp, &vUp);
            */

            vRight *= HalfWidth;
            vUp *= HalfHeight;

            (*pv1) = vWorldPos - vRight + vUp;
            (*pv2) = vWorldPos + vRight + vUp;
            (*pv3) = vWorldPos + vRight - vUp;
            (*pv4) = vWorldPos - vRight - vUp;
        }
        break;

    case SFX_PT_BILLBOARD_LOCK_WORLD_Z :
    case SFX_PT_BILLBOARD_LOCK_LOCAL_Z :
        {
            D3DXVECTOR3 vRight = pCamera->GetCameraRight();
            D3DXVECTOR3 vUp    = pCamera->GetCameraFront();     
            D3DXVECTOR3 vFront = pCamera->GetCameraUp() * -1.f;  
            D3DXVECTOR3 vY     = D3DXVECTOR3(0.f, 1.f, 0.f);

            vUp.y = 0.f;
            vRight.y = 0.f;

            D3DXMATRIX	matRotation;
            D3DXMatrixRotationAxis(&matRotation, &vY, -m_fRotation);

            if (m_eParticleType == SFX_PT_BILLBOARD_LOCK_LOCAL_Z)
            {
                matRotation *= m_pLauncher->m_matWorld;
            }

            D3DXVec3TransformNormal(&vRight, &vRight, &matRotation);
            D3DXVec3TransformNormal(&vUp, &vUp, &matRotation);

            D3DXVec3Normalize(&vRight, &vRight);
            D3DXVec3Normalize(&vUp, &vUp);

            vRight *= HalfWidth;
            vUp *= HalfHeight;

            (*pv1) = vWorldPos - vRight + vUp;
            (*pv2) = vWorldPos + vRight + vUp;
            (*pv3) = vWorldPos + vRight - vUp;
            (*pv4) = vWorldPos - vRight - vUp;
        }
        break;

    case SFX_PT_LEAVES:
        {
            D3DXVECTOR3 vRight(1, 0, 0);
            D3DXVECTOR3 vUp(0, 1, 0);

            D3DXMATRIX mat;
            
			D3DXMatrixRotationYawPitchRoll(&mat, m_fRotationY, m_fRotationX, m_fRotationZ);
            
            D3DXVec3TransformNormal(&vRight, &vRight, &mat);
            D3DXVec3TransformNormal(&vUp, &vUp, &mat);

            vRight *= HalfWidth;
            vUp *= HalfHeight;

            (*pv1) = vWorldPos - vRight + vUp;
            (*pv2) = vWorldPos + vRight + vUp;
            (*pv3) = vWorldPos + vRight - vUp;
            (*pv4) = vWorldPos - vRight - vUp;
        }
        break;

    default:
        ASSERT(0);
        return E_FAIL;
    }

    if (pRectfText)
    {
	    int nTextureFrame = 0;

        float fCurFrame = m_fPreFrame;
        if (m_eParticleUpdateMode == SFX_PUM_SELF)
            fCurFrame = m_fPreFrame - m_fStartFrame;


	    if (m_pLifeTimeLine->m_nTextureFrameTimeLine.GetSize())
	    {
	        m_pLifeTimeLine->m_nTextureFrameTimeLine.GetData(
	            &nTextureFrame,
	            fCurFrame
	        );
	    }
	    else
	    {
            nTextureFrame = 0;
	    }

	    UINT  uTextureIndex = m_pLifeTimeLine->m_uTextureIndex;
	    UINT  uNumber = 0;

	    m_pParentSFX->GetTextureCutNum(uTextureIndex, &uNumber);

	    if (nTextureFrame < 0 || nTextureFrame >= (int)(uNumber * uNumber))
	    {
		    DWORD dwNowTime = g_cGraphicsTool.GetNowTime();
		    if (dwNowTime - m_dwLastRandTime > m_dwRandTexDelay || m_dwLastRandTime == 0)
		    {
			    nTextureFrame = m_pParentSFX->GetRandTexFrame(uTextureIndex);
			    m_dwRandomTextureFrame = nTextureFrame;
			    m_dwLastRandTime = dwNowTime;
		    }
		    else
		    {
			    nTextureFrame = m_dwRandomTextureFrame;
		    }
	    }

        m_pParentSFX->GetTextureCoord(pRectfText, uTextureIndex, nTextureFrame);

    }

    return S_OK;	
}

HRESULT KG3DSFXParticle::PrepareVertexBuffer(KG3DSFXParticleContainer *pContainer)
{
    D3DXVECTOR3 v1;
    D3DXVECTOR3 v2;
    D3DXVECTOR3 v3;
    D3DXVECTOR3 v4;
    FRECT       rcTex;
    
    if (FAILED(GetRectInfo(&v1, &v2, &v3, &v4, &rcTex, m_vPositionWorld)))
        return E_FAIL;
    return pContainer->PushRectangleToVB(&v1, &v2, &v3, &v4, m_dwColor, &rcTex);
}