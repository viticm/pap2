////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DSFXMeteor.h
//  Version     : 1.0
//  Creator     : Chiyer (Zhao XinYu)
//  Create Date : 
//  Comment     : 关键帧加密刀光
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DSFXMETEOR_H_
#define _INCLUDE_KG3DSFXMETEOR_H_

#include "Scene/KG3DTransformation.h"
#include "KG3DSFXParticle.h"

#define  KG3DSFX_METEOR_LINE_MAX_SIZE 512
#define  KG3DSFX_MAXMAL_PRECISION     0.05f


class KG3DTransformation;
class KG3DSFX;
class KG3DModelST;
class KG3DModel;
class KG3DAni;
class KG3DBip;
class KG3DAnimationController;
class KG3DClip;

class KG3DSFXMeteor : public IEKG3DSFXMeteor, public KG3DTransformation
{
public:
    KG3DSFXMeteor(KG3DSFX* pSFX);
    ~KG3DSFXMeteor(void);

    HRESULT Update(float fCurrentFrame);
   
    HRESULT Render();

    // Properties
	void SetSampling(BOOL bSampling);
	BOOL GetSampling();

    void SetLife(float fLife);
    float GetLife();

    void SetEmitMinInterval(float fInterval);
    float GetEmitMinInterval();

    void SetWidth(float fWidth);
    float GetWidth();

    void SetBindTrackIdx(int nTrackIdx);
    int GetBindTrackIdx();

    void SetTextureIdx(DWORD idx);
    DWORD GetTextureIdx();

    void EnableRandomOffset(BOOL bEnable);
    BOOL IsEnableRandomOffset();

    void SetOffset(D3DXVECTOR3 vOffset);
    D3DXVECTOR3 GetOffset();

    SFX_BLEND_MODE GetBlendMode();
    void SetBlendMode(SFX_BLEND_MODE BlendMode);

    virtual HRESULT OnLostDevice();
    virtual HRESULT OnResetDevice();

    virtual void SetDuration(float dur) { m_fDuration = dur; }
    virtual float GetDuration() { return m_fDuration; }

    virtual KGRTSTimeLine*     GetRtsTimeLine()                 { return &m_RtsTimeLine;            }
    virtual void InsertRtsKeyFrame(int nFrame, DWORD dwFlag)    { m_RtsTimeLine.InsertKeyFrameSmark(*this, nFrame, dwFlag);}

    // editor
    virtual BOOL IsHide()               { return m_nHide;           }
    virtual void Show(BOOL enable)      { m_nHide = enable;         }
    virtual LPCTSTR GetName()           { return m_scName.c_str();  }
    virtual void SetName(LPCTSTR szName){ m_scName = szName;        } 

public:
    //BOOL IsRayIntersect(D3DXVECTOR3 &Inter, D3DXVECTOR3 Origin, D3DXVECTOR3 Direction);
    HRESULT EditorRender();
    HRESULT RenderSelectMark();
    HRESULT GetAABBox(AABBOX *pBox);
    HRESULT SetRallyingScale(float fScanl);
    float   GetRallyingScale();
	void    ClearListNode();//清除所有的顶点
private:
    typedef struct _MeteorNode
    {
        D3DXVECTOR3 PosBegin;
        D3DXVECTOR3 PosEnd;
        D3DXVECTOR3 TanBegin;
        D3DXVECTOR3 TanEnd;
        D3DXVECTOR3 vVelocity;
        int         nFlag;

        float   fLife;
    } METEOR_NODE;

    void InterpolateAppend(METEOR_NODE& node);
    HRESULT Hermite(
        D3DXVECTOR3 *pOut,
        CONST D3DXVECTOR3 *pV1,
        CONST D3DXVECTOR3 *pT1,
        CONST D3DXVECTOR3 *pV2,
        CONST D3DXVECTOR3 *pT2,
        FLOAT s
        );

    // 
    HRESULT UpdatePrecisionMatrix(vector<D3DXMATRIX>& vetMatrixs, FLOAT fPrecision, D3DXMATRIX matTrack);                                                 
    HRESULT UpdateBindInfo();                                   // 更新当前绑定骨架

    FLOAT              m_fPrevFrame;                            // 上一次加密帧

    struct _BindInfo
    {
        KG3DAnimationController* pAinmationController;
        enuModelBindType eBindType;

        KG3DModelST*     pDepdModelST;
        KG3DModel*       pDependModel;                              
        KG3DModel*       pSFXDepModel;
        KG3DModel*       pImmDepModel;

        string           strBindName;

        D3DXMATRIX       matSockOffset;

        KG3DBip*         pBip;
        KG3DClip*        pClip;

        int              nBoneIndex;
        int              nSockIndex;

        vector<INT>      vecSkeletons;     

        _BindInfo()
        {
            clear();
        }

        void clear();
    };
    _BindInfo            m_PrecisionBindInfo;

public :
    void SetPrecisionAnimationController(KG3DAnimationController *pController);
	HRESULT SetMeteorInterpolate(float fInterpolate);
    float   GetMeteorInterpolate();
	void    AddListNode();
    int     m_nTrackIndex;
    float   m_fPrevTrackFrame;

    KGRTSTimeLine   m_RtsTimeLine;

private:
    KG3DSFX*    m_pSFX;
    float       m_fDuration;


   // list<METEOR_NODE> m_listNode;
	typedef  list<list<METEOR_NODE>>  KGMETEORLIST;    //by huangjinshou 2007 8.3
	KGMETEORLIST  m_MeteorList;//by huangjinshou 2007 8.3

    float   m_fLife;
    float   m_fEmitMinInterval;

    float   m_fLastFrame;
    float   m_fLastEmitFrame;

    float   m_fWidth;

    D3DXVECTOR3 m_vOffset;
    BOOL        m_bRandomOffset;

	BOOL        m_bSampling;  //是否采样 by huangjinshou 2007 8.3

    DWORD   m_dwTextureIdx;

    SFX_BLEND_MODE  m_BlendMode;
    LPDIRECT3DINDEXBUFFER9  m_lpIndexsBuffer;
    LPDIRECT3DVERTEXBUFFER9 m_lpVertexBuffer;
    INT                     m_nLineNumber;
    UINT                    m_uVertexCnt;
    float                   m_fInterpolateLength;
    float                   m_fRallyingScale;       // 贴图聚集点（比例）

    // editor
    BOOL    m_nHide;
    std::string m_scName;
};

#endif //_INCLUDE_KG3DSFXMETEOR_H_
