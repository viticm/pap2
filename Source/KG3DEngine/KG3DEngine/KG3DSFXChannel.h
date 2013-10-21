////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DSFXChannel.h
//  Version     : 1.0
//  Creator     : Chiyer (Zhao XinYu)
//  Create Date : 2006-10-20 16:11:41
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DSFXCHANNEL_H_
#define _INCLUDE_KG3DSFXCHANNEL_H_

#include "KG3DKeyFrameDataBase.h"
#include "KG3DTexture.h"
#include "./Scene/KG3DTransformation.h"
#include "KG3DSFXParticle.h"
#include "IEEditor.h"

class KG3DCamera;
class KG3DSFX;
class KG3DSFXChannel : public IEKG3DSFXChannel
{
	friend KG3DSFX;

public :
    HRESULT Render(float fCurrentFrame);
    HRESULT FrameMove();
    HRESULT ReSet();   
    HRESULT UpdateEffectPos(D3DXVECTOR3 vSource, D3DXVECTOR3 vTarget);
    HRESULT SetTexture(LPCTSTR lpstrFileName);
    KG3DTexture* GetTexture();
	HRESULT GetTexture(IEKG3DTexture** pTexture);

    // sets
    HRESULT SetPieceCount(int nCount);
    HRESULT SetPieceWidth(float fWidte);
    HRESULT SetPieceLength(float fLength);
    HRESULT SetUSpeed(float fUSpeed);
    HRESULT SetVUpdateFcy(float fVUpdateFcy);
    HRESULT SetVPieceCount(int nCount);
    HRESULT SetBlendMode(int nMode); /*0 : alpah 1 : add 2 : bright add*/
    inline  int GetBlendMode(){return m_nBlendMode;}

    // gets
    int     GetPieceCount();
    float   GetPieceWidth();
    float   GetPieceLength();
    float   GetUSpeed();
    float   GetVUpdateFcy();
    int     GetVPieceCount();

public :
    virtual HRESULT OnLostDevice();
    virtual HRESULT OnResetDevice();

public :
    KG3DSFXChannel(KG3DSFX* pSFX, int nPieceCount = 1);
    virtual ~KG3DSFXChannel(void);

protected :
    LPDIRECT3DVERTEXBUFFER9  m_lpVertexBuffet;
    D3DXVECTOR3     m_vSourPos;
    D3DXVECTOR3     m_vDestPos;

    KG3DSFX*        m_pParentSFX;
    KG3DTexture*    m_pTexture;

    int             m_nBlendMode;
	DWORD		    m_dwPrevTime;

private :
    // data
    int             m_nPieceCount;      // piece count
    float           m_fWidth;           // piece width
    float           m_fLength;          // piece length 
    float           m_fUSpeed;          // the uv moving speed in direction U
    float           m_fUOffset;         // the U offset
    int             m_nCurVIndex;       // the current V piece index
    float           m_fVUpdateFcy;      // the uv update frequency in direction V       
    int             m_nVCount;          // the V piece count
    float           m_fCounter;         // calculagraph
    float           m_fVStride;         // the stride in V
    float           m_fAngelStride;     // the angle of between each piece
};

#endif //_INCLUDE_KG3DSFXCHANNEL_H_
