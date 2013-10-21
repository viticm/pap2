
////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DSfxExp.h
//  Version     : 1.0
//  Creator     : Chiyer (Zhao XinYu)
//  Create Date : 2008-07-21 17:27:00
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "KG3DKeyFrameDataBase.h"
#include "IEKG3DSFX.h"

class KG3DSfxExp : public IEKG3DSfxExp
{
public :
    HRESULT FrameMove(float fCurrentFrame);

public :
    KG3DSfxExp();
    virtual ~KG3DSfxExp();

public :
    virtual IKG3DTimeLine* GetTimeControlTimeLine() { return &m_tlTimeScal; }
    virtual IKG3DTimeLine* GetCameraSwingTimeLine() { return &m_tlCameraSwing; }
    virtual IKG3DTimeLine* GetCameraFrequencyTimeLine() { return &m_tlCameraFrequency; }
    virtual IKG3DTimeLine* GetModelScanlTimeLine()  { return &m_tlModelScanl; }

    virtual BOOL IsValid();
    virtual BOOL HaveTimeScaling();

    virtual void OnSfxBind(KG3DModel* pModel);
    virtual void OnSfxUnbind();

public :

    /* time control */
    TKG3DTimeLine<float> m_tlTimeScal; 

    /* camera control */
    TKG3DTimeLine<D3DXVECTOR3> m_tlCameraSwing;
    TKG3DTimeLine<D3DXVECTOR3> m_tlCameraFrequency;
    float m_fPassTime;

    /* model control */
    TKG3DTimeLine<D3DXVECTOR3> m_tlModelScanl;
    D3DXVECTOR3                m_vScanlSave;
    KG3DModel*                 m_pBindModel;
};
