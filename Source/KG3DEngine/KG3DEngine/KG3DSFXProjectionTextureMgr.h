////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DSFXProjectionTextureMgr.h
//  Version     : 1.0
//  Creator     : huang jinshou
//  Create Date : 2008-1-7 10:54:50
//  Comment     : 用于统一管理KG3DSFXSelectTexture对象。
//
////////////////////////////////////////////////////////////////////////////////
#pragma once
//#ifndef _INCLUDE_KG3DSFXPROJECTIONTEXTUREMGR_H_
//#define _INCLUDE_KG3DSFXPROJECTIONTEXTUREMGR_H_

////////////////////////////////////////////////////////////////////////////////
class KG3DTerrainBlock;
class KG3DModel;
class KG3DSFXSelectTexture;
class KG3DSFXSelectMark;
class KG3DTexture;
class KG3DCamera;
class KG3DSFXMagicParpet;

#define Proj_Tex_Effect_File TEXT("data\\public\\shader\\proj_tex.fx")

class KG3DProjTextureRender
{
public :

	KMutex m_Lock;

    HRESULT Init();
    HRESULT Clear();

    HRESULT CheckInputProjectSource(KG3DModel* mdl);
    HRESULT AddProjTextureSource(KG3DSFXMagicParpet* pAoe);
    HRESULT DelProjTextureSource(KG3DSFXMagicParpet* pAoe);
    HRESULT RemoveAllProjSource();

    HRESULT Update();

    size_t  TexNumber() { return m_aAoes.size();  }
    float*  Heights()   { return m_apHeight; }
    void    GetInfo(D3DXVECTOR4* info, size_t index);
    void    GetDiff(D3DXCOLOR* diff, size_t index);
    void    GetLimt(float* limt, size_t index);
    void    GetTexFrameInfo(D3DXVECTOR4* info, size_t index);
    LPDIRECT3DTEXTURE9 GetTexture(size_t index);

    bool NeedProject(AABBOX& CheckBox);
    HRESULT SetEffectParam(LPD3DXEFFECT pEffect);

    HRESULT OnLostDevice();
    HRESULT OnResetDevice();

    KG3DProjTextureRender();
    virtual ~KG3DProjTextureRender();

    enum { Proj_Tex_Size = 1024, Max_Tex_Num = 32 };

protected :
	vector<KG3DSFXMagicParpet*>m_aAoes/*[Max_Tex_Num]*/;
    KG3DSFXMagicParpet* m_pSelector;
    //UINT          m_uAoeNum;
    float         m_apHeight[Max_Tex_Num];
};

extern KG3DProjTextureRender g_ProjTextureRender;

//#endif //_INCLUDE_KG3DSFXPROJECTIONTEXTUREMGR_H_