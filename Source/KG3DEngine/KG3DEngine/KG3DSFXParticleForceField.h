////////////////////////////////////////////////////////////////////////////////
//
//  Collator : Chiyer (Zhao XinYu)
//  Date     : 2007-2-12 17:14:16
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DSFXPARTICLEFORCEFIELD_H_
#define _INCLUDE_KG3DSFXPARTICLEFORCEFIELD_H_

#include "KG3DKeyFrameControl.h"
#include "KG3DKeyFrameDataBase.h"
#include "IEKG3DSFX.h"

class KG3DSFXParticleForceField : public IEKG3DSFXParticleForceField
{
public :

    /* apply the force to the partical */
    HRESULT ApplyForce(D3DXVECTOR3* pvVelocity,           
        float        fPreFrame,            
        float        fCurrentFrame, 
        D3DXVECTOR3* pPosition, 
        DWORD        option,
        float*       pfLastFactor
    );


    /* just use to editor */
    HRESULT Render();


    enum {FORCT_ROTATION_MAX = 100000};
    
    /* set the force type 
        SFX_FFT_G
        SFX_FFT_WIND 
        SFX_FFT_WHIRLWIND
        SFX_FFT_ROTATION 
        */
    SFX_FORCE_FIELD_TYPE GetType() const             { return m_eType;             }
    void SetType(SFX_FORCE_FIELD_TYPE eType)         { m_eType = eType;            }


    /* editor */
    virtual BOOL IsHide()               { return m_nHide;           }
    virtual void Show(BOOL enable)      { m_nHide = enable;         }
    virtual LPCTSTR GetName()           { return m_scName.c_str();  }
    virtual void SetName(LPCTSTR szName){ m_scName = szName;        } 


     /* get the time line */
	virtual TKG3DTimeLinebase* GetForceTimeLine()    { return &m_ForceTimeLine;    }
    virtual TKG3DTimeLinebase* GetForcePosTimeLine() { return &m_ForcePosTimeLine; }
    virtual TKG3DTimeLinebase* GetForceGTimeLine()   { return &m_ForceGTimeLine;   }
    

     /* scaling all time line */
    void ScanlTimeLine(float scanl)                  { m_ForceTimeLine.Scanl(scanl);}

public :
    KG3DSFXParticleForceField();
    virtual ~KG3DSFXParticleForceField();


public :
    TKG3DTimeLine<FLOAT5>                   m_ForceTimeLine;
    TKG3DTimeLine<D3DXVECTOR3>              m_ForcePosTimeLine;
    std::vector<IEKG3DSFXParticleLauncher*> m_vecLauncherRef;
    TKG3DTimeLine<FLOAT>                    m_ForceGTimeLine;


    HRESULT AddLauncherRef(IEKG3DSFXParticleLauncher* pLauncher);
    HRESULT DelLauncherRef(IEKG3DSFXParticleLauncher* pLauncher);


private :
    SFX_FORCE_FIELD_TYPE    m_eType;

    // editor
    BOOL        m_nHide;
    std::string m_scName;
};

#endif //_INCLUDE_KG3DSFXPARTICLEFORCEFIELD_H_
