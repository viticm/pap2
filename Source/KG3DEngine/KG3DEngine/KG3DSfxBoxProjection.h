
////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DSfxBoxProjection.h
//  Version     : 1.0
//  Creator     : Chiyer (Zhao XinYu)
//  Create Date : 2009-50-12 16:11:41
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _KG3DSfxBoxProjection_H_
#define _KG3DSfxBoxProjection_H_

#include "Scene/KG3DTransformation.h"
#include "KG3DKeyFrameDataBase.h"
#include "IEKG3DSFX.h"
#include "KG3DRenderState.h"

#define Local_Box_Size 50.f

class KG3DTexture;
class KG3DSFX;

class KG3DSfxBoxProjection : public IKG3DSfxBoxProjection, public KG3DTransformation
{
    struct _Vertex
    {
        D3DXVECTOR3 p;
        enum { FVF = D3DFVF_XYZ | D3DFVF_TEX1 };
    };

public :

    HRESULT Create();
    HRESULT Clear();

    HRESULT Render();
    HRESULT FrameMove();

    HRESULT GetAABBox(AABBOX* pBox);

    virtual DWORD GetProjType()                 { return m_dwProjType;   }
    virtual void  SetProjType(DWORD dwType)     { m_dwProjType = dwType; }
    virtual IKG3DTimeLine* GetColorTimeLine()   { return &m_tlColor;     }
    virtual IKG3DTimeLine* GetUVTimeLine()      { return &m_tlUV;        }
    virtual SFX_BLEND_MODE GetBlendMode()       { return m_eBlend;       }
    virtual void SetBlendMode(SFX_BLEND_MODE eBlend)    { m_eBlend = eBlend;     }

    void SetRenderState(KG3DRenderState& R, SFX_BLEND_MODE eBlendMode);

    virtual void SetTexture(char szFileName[]);
    virtual void GetTexture(char szFileName[]);
    virtual void SetTexClip(int u, int v)       { m_nClipU = u; m_nClipV = v;   }
    virtual void GetTexClip(int& u, int& v)     { u = m_nClipU; v = m_nClipV;   }
    virtual void SetUVFrameSpeed(float fSpeed)  { m_fUVFrameSpeed = fSpeed;     }
    virtual float GetUVFrameSpeed()             { return m_fUVFrameSpeed;       }

    virtual HRESULT OnLostDevice();
    virtual HRESULT OnResetDevice();

    KG3DSfxBoxProjection(KG3DSFX* pParentSfx);
    virtual ~KG3DSfxBoxProjection();

    static D3DXVECTOR3 m_sBox[8];

public :
    KG3DSFX*                 m_pSfx;
    IDirect3DVertexBuffer9 * m_pVB;
    _Vertex*                 m_pSysVB;
    int                      m_nMaxFaceNum;
    int                      m_nFaceNum;
    KG3DModel*               m_pCurrentOpModel;
    D3DXMATRIX               m_matWorld;
    KG3DTexture*             m_pTexture;
    AABBOX                   m_BBox;
    DWORD                    m_dwProjType;
    SFX_BLEND_MODE           m_eBlend;
    int                      m_nClipU;
    int                      m_nClipV;
    float                    m_fUVFrameSpeed;

    TKG3DTimeLine<D3DXCOLOR>   m_tlColor;
    TKG3DTimeLine<D3DXVECTOR4> m_tlUV;
};

#endif