
////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DSFXMagicParpet.h
//  Version     : 1.0
//  Creator     : Chiyer (Zhao XinYu)
//  Create Date : 2008-07-21 17:27:00
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _KG3DSFX_MAGICPARPET_H__
#define _KG3DSFX_MAGICPARPET_H__

#include "KG3DTexture.h"
#include "IEKG3DSFX.h"

class KG3DSFX;
class KG3DCamera;

class KG3DSFXMagicParpet : public IEKG3DSFXMagicParpet
{
    friend class KG3DSFX;
private :
	typedef struct _Footprint
	{
		D3DXVECTOR3 vPosition;
		D3DXVECTOR4 vParam;
		int         nRadius;
		int         nCellCountLine;
		float       fUVStep;
		float       fUVZoom;
		float       fAngel;
		float       fLife;
		DWORD       dwStartTime;
		std::vector<float> CellVertexHeight;
	} FOOTPRINT;

public :
    KG3DSFXMagicParpet(KG3DSFX *pSFX, DWORD dwType);
    virtual ~KG3DSFXMagicParpet(void);

    HRESULT Render();
    HRESULT Update(float fCurFrame);
	void FrameMove();
    void Clear(bool bClearTex);

    HRESULT SetTexture(const char cszTextureName[]);
    void SetRadius(int nRadius, int bBaseRadius = false);
    void SetOffset(float fOffset);
    void SetHeightLimit(float fLimit);
    void CalcParam();

    IEKG3DTexture* GetTexture();
	int   GetBaseRaduis();
    int   GetRadius();
    float GetOffset();
    float GetHeightLimit();
	int   IsFaceCamera();
	void  EnableFaceCamera(int nFlag = TRUE);
	void  SetAngle(float fAngle);
	float GetAngle();
	void  SetLife(float fLife);
	float GetLife();
	HRESULT NewOneFootprint();
    virtual HRESULT OnLostDevice();
    virtual HRESULT OnResetDevice();
    virtual void SetTexCull(int x, int y);
    virtual void SetTexSpeed(float speed);
    virtual void GetTexCull(int* x, int* y);
    virtual float GetTexSpeed();

    D3DXVECTOR3 GetWorldPos();
    AABBOX CalcBBox();
    void GetInfo(D3DXVECTOR4* info);
    void GetTexFrameInfo(D3DXVECTOR4* info);

    virtual DWORD GetType() { return m_dwType; }
    virtual void SetType(DWORD t, bool bForce);
    virtual D3DXCOLOR GetCurrColor() { return m_CurrColor; }

    virtual DWORD GetFaceType() { return m_dwFaceType; }
    virtual void SetFaceType(DWORD t) { m_dwFaceType = t; }

    virtual IKG3DTimeLine* GetRadiusTimeLine() { return &m_tlRadius; }
    virtual IKG3DTimeLine* GetColorsTimeLine() { return &m_tlColor;  }

    /* edit */
    virtual BOOL IsHide()               { return m_nHide;           }
    virtual void Show(BOOL enable)      { m_nHide = enable;         }
    virtual LPCTSTR GetName()           { return m_scName.c_str();  }
    virtual void SetName(LPCTSTR szName){ m_scName = szName;        } 
	
private :
    HRESULT CreateBuffer();
    void FillVertexBuffer(VFormat::_Faces_Diffuse_Texture1* pData,DWORD dwMaxVertex);
    void FillIndexBuffer(WORD *pData,int nCellCountLine);

	void  UpdateFootprint();
	HRESULT RenderFootprint(FOOTPRINT& footprint);
	HRESULT RenderAOE();
	void  FillVertexBuffer(VFormat::_Faces_Diffuse_Texture1* pData,FOOTPRINT& Footprint);
    bool NeedFillVB();

private :
    KG3DSFX*        m_pSFX;
    KG3DTexture*    m_pTexture;

    LPDIRECT3DVERTEXBUFFER9 m_lpVB;
    LPDIRECT3DINDEXBUFFER9  m_lpIB;

    D3DXVECTOR3 m_vWorldPosSave;
    D3DXVECTOR3 m_vCameraPosSave;
    D3DXVECTOR3 m_vCameraTagSave;

    DWORD   m_dwType;
    DWORD   m_dwFaceType;

    int     m_nCellCountLine;
	int		m_nRadius;
	int		m_nRelRadius;
    float   m_fOffset;
    float   m_fHeightLimit;
	int		m_nBaseRadius;

	float	m_fUVZoom;
	float   m_fUVStep;

    D3DXCOLOR m_CurrColor;
	float   m_fUVRotation;
	D3DXMATRIX m_matSaveSFXWorld;

	/* footprint */
	list<FOOTPRINT> m_listFootprint;
	float m_fAngel;
	float m_fLife; 	
	//D3DXVECTOR3 m_vPosition;
	int m_nCurCellCountLine;

	/* edit*/
    BOOL        m_nHide;
    std::string m_scName;

    int m_nCurrTexX;
    int m_nCurrTexY;
    int m_nTexCullX;
    int m_nTexCullY;
    float m_texSpeed;


    TKG3DTimeLine<D3DXCOLOR> m_tlColor;
    TKG3DTimeLine<int> m_tlRadius;
};

#endif