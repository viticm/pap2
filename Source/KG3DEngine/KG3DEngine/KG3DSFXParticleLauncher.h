////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DSFXParticleLauncher.h
//  Version     : 1.0
//  Creator     : Chiyer (Zhao XinYu)
//  Create Date : 2006-11-22 14:46:14
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DSFXPARTICLELAUNCHER_H_
#define _INCLUDE_KG3DSFXPARTICLELAUNCHER_H_

#include "Scene/KG3DTransformation.h"
#include "KG3DSFXParticle.h"
#include "KG3DKeyFrameDataBase.h"
#include "IEKG3DSFX.h"

#define SPEED_INHERIT_ENABLE    1
#define SPEED_INHERIT_DISABLE   0

class KG3DSFXParticleLauncher : public IEKG3DSFXParticleLauncher, public KG3DTransformation
{
public :

    /* 
     * functional 
     */

    HRESULT LaunchParticle(float fCurrentFrame);
    HRESULT FrameMove();
    HRESULT EditorRender();
    HRESULT RenderSelectMark(DWORD dwColor);

    HRESULT BindToTrack(DWORD dwIndex);
    HRESULT UnBindToTrack();

    BOOL    IsBindToTrack() const;
    DWORD   GetBindTrackIndex() const;

    HRESULT Clone(KG3DSFXParticleLauncher **ppLauncher);
//    BOOL    IsRayIntersect(D3DXVECTOR3 &Inter, D3DXVECTOR3 Origin, D3DXVECTOR3 Direction);

    virtual HRESULT AddEffForce(IEKG3DSFXParticleForceField* pForce);
    virtual HRESULT DelEffForce(IEKG3DSFXParticleForceField* pForce);
    virtual BOOL    HasEffForce(IEKG3DSFXParticleForceField* pForce);


    /*
     * property
     */

    HRESULT EnableSpeedInherit(BOOL bInherit)                       { m_bSpeedInherit = bInherit; return S_OK; }
    BOOL    IsSpeedInherit()                                        { return m_bSpeedInherit; }

    SFX_LAUNCHER_BIND_TO_TRACK_MODE GetBindToTrackMode() const      { return m_eBindToTrackMode;  }
    void SetBindToTrackMode(SFX_LAUNCHER_BIND_TO_TRACK_MODE eMode)  { m_eBindToTrackMode = eMode; }

    SFX_PARTICLE_TYPE GetParticleType() const                       { return m_eParticleType; };
    void SetParticleType( SFX_PARTICLE_TYPE eParticleType)          { m_eParticleType = eParticleType; }

    SFX_LAUNCHER_SHAPE GetLauncherShape() const                     { return m_eLauncherShape; }
    void SetParticleShape( SFX_LAUNCHER_SHAPE eParticleShape)       { m_eLauncherShape = eParticleShape; }

    SFX_PARTICLE_UPDATE_MODE GetParticleUpdateMode() const  { return m_eParticleUpdateMode; }
    void SetParticleUpdateMode(SFX_PARTICLE_UPDATE_MODE eParticleUpdateMode) {  m_eParticleUpdateMode = eParticleUpdateMode; }
    
    HRESULT SetBlendMode(SFX_BLEND_MODE eBlendMode)                 { m_eBlendMode = eBlendMode; return S_OK; } 
    SFX_BLEND_MODE GetBlendMode()                                   {  return m_eBlendMode; }

    float GetShapeParam1() const                                    { return m_fLauncherShapeParam1;   }
    float GetShapeParam2() const                                    { return m_fLauncherShapeParam2;   }
    void  SetShapeParam1(float fValue)                              { m_fLauncherShapeParam1 = fValue; }
    void  SetShapeParam2(float fValue)                              { m_fLauncherShapeParam2 = fValue; }

    void  SetParticleLifeTimeLine(IEKG3DSFXParticleLifeTimeLine* pParticleLifeTimeLine);
    virtual IEKG3DSFXParticleLifeTimeLine* GetParticleLifeTime();
    KG3DSFXParticleLifeTimeLine* GetParticleLifeTimeDirect()        { return m_pParticleLifeTimeLine; };
    KG3DSFX* GetParentSFX() const;

    void GetAABBox(AABBOX *pBox);

    virtual HRESULT SetRandTexDelay(DWORD dwDelay)          { m_dwRandTexDelay = dwDelay; return S_OK; }
    virtual DWORD   GetRandTexDelay()                       { return m_dwRandTexDelay; }

    virtual HRESULT SetRandColDelay(DWORD dwDelay)          { m_dwRandColDelay = dwDelay; return S_OK; }
    virtual DWORD   GetRandColDelay()                       { return m_dwRandColDelay; }

    virtual HRESULT SetRandRotDelay(DWORD dwDelay)          { m_dwRandRotDelay = dwDelay; return S_OK; }
    virtual DWORD   GetRandRotDelay()                       { return m_dwRandRotDelay; }

    virtual HRESULT SetRandAlpDelay(DWORD dwDelay)          { m_dwRandAlpDelay = dwDelay; return S_OK; }
    virtual DWORD   GetRandAlpDelay()                       { return m_dwRandAlpDelay; }

    virtual HRESULT SetRandForceRotDelay(DWORD dwDelay)     { m_dwRandForceRotDelay = dwDelay; return S_OK; }
    virtual DWORD   GetRandForceRotDelay()                  { return m_dwRandForceRotDelay; }

    /*
     * transformation
     */

    virtual void SetTranslation(D3DXVECTOR3* pValue)            { return KG3DTransformation::SetTranslation(pValue); }
    virtual void SetRotation(D3DXQUATERNION* pValue)            { return KG3DTransformation::SetRotation(pValue); }
    virtual void SetRotationCenter(D3DXVECTOR3* pValue)         { return KG3DTransformation::SetRotationCenter(pValue); }
    virtual void SetScaling(D3DXVECTOR3* pValue)                { return KG3DTransformation::SetScaling(pValue); }
    virtual void SetScalingRotation(D3DXQUATERNION* pValue)     { return KG3DTransformation::SetScalingRotation(pValue); }
    virtual void SetScalingCenter(D3DXVECTOR3* pValue)          { return KG3DTransformation::SetScalingCenter(pValue); }

    virtual void GetTranslation(D3DXVECTOR3* pValue)            { return KG3DTransformation::GetTranslation(pValue); }
    virtual void GetRotation(D3DXQUATERNION* pValue)            { return KG3DTransformation::GetRotation(pValue); }
    virtual void GetRotationCenter(D3DXVECTOR3* pValue)         { return KG3DTransformation::GetRotationCenter(pValue); }
    virtual void GetScaling(D3DXVECTOR3* pValue)                { return KG3DTransformation::GetScaling(pValue); }
    virtual void GetScalingRotation(D3DXQUATERNION* pValue)     { return KG3DTransformation::GetScalingRotation(pValue); }
    virtual void GetScalingCenter(D3DXVECTOR3* pValue)          { return KG3DTransformation::GetScalingCenter(pValue); }

    virtual void UpdateTransformation()                         { return KG3DTransformation::UpdateTransformation(); }
    virtual void UpdateByMatrix(const D3DXMATRIX& mat)          { return KG3DTransformation::UpdateByMatrix(mat); }

    virtual DWORD GetMotionType()                               { return m_dwMotionType;  }
    virtual void  SetMotionType(DWORD dwType)                   { m_dwMotionType = dwType;}

    virtual DWORD GetForceOption()                              { return m_dwForceOption; }
    virtual void  SetForceOption(DWORD optn)                    { m_dwForceOption = optn; }

    /*
	virtual TKG3DTimeLinebase* GetTranslationTimeLine()         { return &m_TranslationTimeLine; }
	virtual TKG3DTimeLinebase* GetScalingTimeLine()             { return &m_ScalingTimeLine; }
	virtual TKG3DTimeLinebase* GetRotationTimeLine()            { return &m_RotationTimeLine; }
    */
    virtual KGRTSTimeLine*     GetRtsTimeLine()                 { return &m_RtsTimeLine;            }
	virtual TKG3DTimeLinebase* GetParticleNumLine()             { return &m_fParticleNumLine;       }
	virtual TKG3DTimeLinebase* GetParticleSpeedMinLine()        { return &m_fParticleSpeedMinLine;  }
	virtual TKG3DTimeLinebase* GetParticleSpeedMaxLine()        { return &m_fParticleSpeedMaxLine;  }
	virtual TKG3DTimeLinebase* GetParticleLifeMinLine()         { return &m_fParticleLifeMinLine;   }
	virtual TKG3DTimeLinebase* GetParticleLifeMaxLine()         { return &m_fParticleLifeMaxLine;   }
	virtual TKG3DTimeLinebase* GetParticleScatterMin()          { return &m_fParticleScatterMin;    }
	virtual TKG3DTimeLinebase* GetParticleScatterMax()          { return &m_fParticleScatterMax;    }

    virtual void InsertRtsKeyFrame(int nFrame, DWORD dwFlag)    { m_RtsTimeLine.InsertKeyFrameSmark(*this, nFrame, dwFlag);}

    void ScanlTimeLine(float scanl) {
        m_fParticleNumLine.Scanl(scanl);
        m_fParticleLifeMaxLine.Scanl(scanl);
        m_fParticleLifeMinLine.Scanl(scanl);
        m_fParticleSpeedMaxLine.Scanl(scanl);
        m_fParticleSpeedMinLine.Scanl(scanl);
        m_fParticleScatterMax.Scanl(scanl);
        m_RtsTimeLine.Scanl(scanl);
        /*
        m_TranslationTimeLine.Scanl(scanl);
        m_RotationTimeLine.Scanl(scanl);
        m_ScalingTimeLine.Scanl(scanl);
        */
    }

    // editor
    // editor
    virtual BOOL IsHide()               { return m_nHide;           }
    virtual void Show(BOOL enable)      { m_nHide = enable;         }
    virtual LPCTSTR GetName()           { return m_scName.c_str();  }
    virtual void SetName(LPCTSTR szName){ m_scName = szName;        } 

public :
    TKG3DTimeLine<float>                m_fParticleNumLine;
    TKG3DTimeLine<float>                m_fParticleLifeMaxLine;
    TKG3DTimeLine<float>                m_fParticleLifeMinLine;
    TKG3DTimeLine<float>                m_fParticleSpeedMaxLine;
    TKG3DTimeLine<float>                m_fParticleSpeedMinLine;
    TKG3DTimeLine<float>                m_fParticleScatterMin;
    TKG3DTimeLine<float>                m_fParticleScatterMax;

    KGRTSTimeLine                       m_RtsTimeLine;

    std::vector<IEKG3DSFXParticleForceField*> m_vecEffForces;
    std::vector<DWORD>                        m_vecEffForcesIDs; // use to load and save

    D3DXMATRIX                          m_matWorld;

    int                                 m_nSelFlag;
    DWORD                               m_dwMotionType;
    DWORD                               m_dwForceOption;

    float                               m_fParticleNumKeep;

public :
    KG3DSFXParticleLauncher(
        KG3DSFX*                  pParentSFX,
        KG3DSFXParticleContainer* pContainer,
        SFX_PARTICLE_TYPE         eParticleType,
        SFX_LAUNCHER_SHAPE        eLauncherShape,
        SFX_PARTICLE_UPDATE_MODE  eParticleUpdateMode
    );

    virtual ~KG3DSFXParticleLauncher();

private :
    void GetParticleDirectionAndPos(
        float fScatterMin,
        float fScatterMax,
        D3DXVECTOR3 *pvDirection,
        D3DXVECTOR3 *pvPosition
    );

    void MakeParticle(
        float fLife,
        D3DXVECTOR3 *pvPosition,
        D3DXVECTOR3 *pDirection,
        float fFrame
    );

protected :
    SFX_BLEND_MODE                      m_eBlendMode;
    BOOL                                m_bSpeedInherit;
    SFX_PARTICLE_TYPE                   m_eParticleType;
    SFX_LAUNCHER_SHAPE                  m_eLauncherShape;
    SFX_PARTICLE_UPDATE_MODE            m_eParticleUpdateMode;

    float                               m_fLastLauncherFrame;
    float                               m_fLauncherShapeParam1;
    float                               m_fLauncherShapeParam2;

    KG3DSFXParticleLifeTimeLine         *m_pParticleLifeTimeLine;
    KG3DSFXParticleContainer            *m_pParticleContainer;
    KG3DSFX                             *m_pParentSFX;

    DWORD                              m_dwBindTrackIndex;
    SFX_LAUNCHER_BIND_TO_TRACK_MODE     m_eBindToTrackMode;


	//
	DWORD	m_dwRandTexDelay; // delay time when use random texture frame
    DWORD   m_dwRandColDelay;
    DWORD   m_dwRandRotDelay;
    DWORD   m_dwRandAlpDelay;
    DWORD   m_dwRandForceRotDelay;

    // editor
    BOOL    m_nHide;
    std::string m_scName;
};

#endif //_INCLUDE_KG3DSFXPARTICLELAUNCHER_H_
