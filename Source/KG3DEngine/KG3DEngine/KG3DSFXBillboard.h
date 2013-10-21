////////////////////////////////////////////////////////////////////////////////
//
//  Collator : Chiyer (Zhao XinYu)
//  Date     : 2007-2-12 17:14:16
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DSFXBILLBOARD_H_
#define _INCLUDE_KG3DSFXBILLBOARD_H_

#include "KG3DKeyFrameDataBase.h"
#include "KG3DTexture.h"
#include "Scene/KG3DTransformation.h"
#include "KG3DSFXParticle.h"
#include "IEKG3DSFX.h"

class KG3DSFX;

class KG3DSFXBillboard : public IEKG3DSFXBillboard
{
public :
    KG3DSFXBillboard(KG3DSFX *pSFX);
    virtual ~KG3DSFXBillboard(void);

    HRESULT Init();

    HRESULT Render(float fCurrentFrame);
    HRESULT RenderSelectMark();

    void GetAABBox(AABBOX *pBox);

    HRESULT LoadTexture(const char szTextureName[]);
    HRESULT SetTextureCutNum(DWORD dwCuttingNum);

    HRESULT LoadTexture2(const char szTextureName[]);
    HRESULT SetTexture2CutNum(DWORD dwCuttingNum);

    IEKG3DTexture* GetTexture();
    IEKG3DTexture* GetTexture2();

    DWORD GetTextureCutNum();
    DWORD GetTexture2CutNum();

    //HRESULT EnableLockAxis(BOOL bEnable);
    //HRESULT EnableRotation(BOOL bEnable);

    //BOOL IsLockAxis();
    //BOOL IsRotation();

    
    void GetTranslation(D3DXVECTOR3* pValue);
    void GetRotation(float* pValue);

    void SetTranslation(D3DXVECTOR3* pValue);
    void SetRotation(float* pValue);
    

    HRESULT BindToBone(const char cszBoneName[]);
    HRESULT UnBind();

    BOOL IsBindToBone() const;
    std::string GetBindName() const;

    HRESULT Clone(KG3DSFXBillboard **ppBillboard);

    // Well, bind to track has a higher priority.
    HRESULT BindToTrack(DWORD dwIndex);
    HRESULT UnBindToTrack();
    BOOL IsBindToTrack() const;
    DWORD GetBindTrackIndex() const;

    SFX_BLEND_MODE GetBlendMode();
    void SetBlendMode(SFX_BLEND_MODE bm);

   /* BOOL IsRayIntersect(D3DXVECTOR3 &Inter, D3DXVECTOR3 Origin, D3DXVECTOR3 Direction) {
        Inter = Origin;
        return true;
    }*/

    virtual HRESULT DiffuseTimeLineGetKeyFrameByIndex( int *pnFrame, D3DXCOLOR *pData, DWORD dwIndex ){
        return m_DiffuseTimeLine.GetKeyFrameByIndex(pnFrame,pData,dwIndex); 
    }
    virtual HRESULT WidthTimeLineGetKeyFrameByIndex( int *pnFrame, float *pData, DWORD dwIndex ) {
        return m_fWidthTimeLine.GetKeyFrameByIndex(pnFrame,pData,dwIndex); 
    }
    virtual HRESULT PositionTimeLineGetKeyFrameByIndex( int *pnFrame, D3DXVECTOR3 *pData, DWORD dwIndex ){
        return m_vPositionLine.GetKeyFrameByIndex(pnFrame,pData,dwIndex);
    }
    virtual HRESULT HeightTimeLineGetKeyFrameByIndex( int *pnFrame, float *pData, DWORD dwIndex ) {
        return m_fHeightTimeLine.GetKeyFrameByIndex(pnFrame,pData,dwIndex);
    }
    virtual HRESULT TextureFrameIndexGetKeyFrameByIndex( int *pnFrame, INT *pData, DWORD dwIndex ) {
        return m_nTextureFrameIndex.GetKeyFrameByIndex(pnFrame,pData,dwIndex);
    }
    virtual HRESULT Texture2FrameIndexGetKeyFrameByIndex( int *pnFrame, INT *pData, DWORD dwIndex ) {
        return m_nTexture2FrameIndex.GetKeyFrameByIndex(pnFrame,pData,dwIndex);
    }
    virtual HRESULT RotationTimeLineGetKeyFrameByIndex( int *pnFrame, float *pData, DWORD dwIndex ) {
        return m_fRotationTimeLine.GetKeyFrameByIndex(pnFrame,pData,dwIndex);
    }
    virtual HRESULT UVTimeLineGetKeyFrameByIndex( int *pnFrame, D3DXVECTOR2 *pData, DWORD dwIndex ) {
        return m_vUVTimeLine.GetKeyFrameByIndex(pnFrame,pData,dwIndex);
    }

	virtual INT DiffuseTimeLineGetSize()                        { return (INT)m_DiffuseTimeLine.GetSize(); }
	virtual HRESULT DiffuseTimeLineRemoveKeyFrame(INT nFrame)   { return m_DiffuseTimeLine.RemoveKeyFrame(nFrame); }
	virtual HRESULT DiffuseTimeLineGetData(D3DXCOLOR *pData,float fFrame) { return m_DiffuseTimeLine.GetData(pData,fFrame); }
	virtual HRESULT DiffuseTimeLineIsKeyFrame(int nFrame)       { return m_DiffuseTimeLine.IsKeyFrame(nFrame); }
	virtual HRESULT DiffuseTimeLineInsertKeyFrame(int nFrame,D3DXCOLOR Data) { return m_DiffuseTimeLine.InsertKeyFrame(nFrame,Data); }
	virtual INT WidthTimeLineGetSize()                          { return (INT)m_fWidthTimeLine.GetSize(); }
	virtual HRESULT WidthTimeLineRemoveKeyFrame(INT nFrame)     { return m_fWidthTimeLine.RemoveKeyFrame(nFrame); }
	virtual HRESULT WidthTimeLineGetData(float *pData,float fFrame) { return m_fWidthTimeLine.GetData(pData,fFrame); }
	virtual HRESULT WidthTimeLineIsKeyFrame(int nFrame)         { return m_fWidthTimeLine.IsKeyFrame(nFrame); }
	virtual HRESULT WidthTimeLineInsertKeyFrame(int nFrame,float Data) { return m_fWidthTimeLine.InsertKeyFrame(nFrame,Data); }
	virtual INT PositionTimeLineGetSize()                       { return (INT)m_vPositionLine.GetSize(); }
	virtual HRESULT PositionTimeLineRemoveKeyFrame(INT nFrame)  { return m_vPositionLine.RemoveKeyFrame(nFrame); }
	virtual HRESULT PositionTimeLineGetData(D3DXVECTOR3 *pData,float fFrame) { return m_vPositionLine.GetData(pData,fFrame); }
	virtual HRESULT PositionTimeLineIsKeyFrame(int nFrame)      { return m_vPositionLine.IsKeyFrame(nFrame); }
	virtual HRESULT PositionTimeLineInsertKeyFrame(int nFrame,D3DXVECTOR3 Data) { return m_vPositionLine.InsertKeyFrame(nFrame,Data); }
	virtual INT HeightTimeLineGetSize()                         { return (INT)m_fHeightTimeLine.GetSize(); }
	virtual HRESULT HeightTimeLineRemoveKeyFrame(INT nFrame)    { return m_fHeightTimeLine.RemoveKeyFrame(nFrame); }
	virtual HRESULT HeightTimeLineGetData(float *pData,float fFrame) { return m_fHeightTimeLine.GetData(pData,fFrame); }
	virtual HRESULT HeightTimeLineIsKeyFrame(int nFrame)        { return m_fHeightTimeLine.IsKeyFrame(nFrame); }
	virtual HRESULT HeightTimeLineInsertKeyFrame(int nFrame,float Data) { return m_fHeightTimeLine.InsertKeyFrame(nFrame,Data); }
	virtual INT TextureFrameIndexGetSize()                      { return (INT)m_nTextureFrameIndex.GetSize(); }
	virtual HRESULT TextureFrameIndexRemoveKeyFrame(INT nFrame) { return m_nTextureFrameIndex.RemoveKeyFrame(nFrame); }
	virtual HRESULT TextureFrameIndexGetData(int *pData,float fFrame) { return m_nTextureFrameIndex.GetData(pData,fFrame); }
	virtual HRESULT TextureFrameIndexIsKeyFrame(int nFrame)     { return m_nTextureFrameIndex.IsKeyFrame(nFrame); }
    virtual HRESULT TextureFrameIndexInsertKeyFrame(int nFrame,int Data) { return m_nTextureFrameIndex.InsertKeyFrame(nFrame,Data); }
    virtual INT Texture2FrameIndexGetSize()                     { return (INT)m_nTexture2FrameIndex.GetSize(); }
	virtual HRESULT Texture2FrameIndexRemoveKeyFrame(INT nFrame){ return m_nTexture2FrameIndex.RemoveKeyFrame(nFrame); }
	virtual HRESULT Texture2FrameIndexGetData(int *pData,float fFrame) { return m_nTexture2FrameIndex.GetData(pData,fFrame); }
	virtual HRESULT Texture2FrameIndexIsKeyFrame(int nFrame)    { return m_nTexture2FrameIndex.IsKeyFrame(nFrame); }
	virtual HRESULT Texture2FrameIndexInsertKeyFrame(int nFrame,int Data) { return m_nTexture2FrameIndex.InsertKeyFrame(nFrame,Data); }
	virtual INT RotationTimeLineGetSize()                       { return (INT)m_fRotationTimeLine.GetSize(); }
	virtual HRESULT RotationTimeLineRemoveKeyFrame(INT nFrame)  { return m_fRotationTimeLine.RemoveKeyFrame(nFrame); }
	virtual HRESULT RotationTimeLineGetData(float *pData,float fFrame) { return m_fRotationTimeLine.GetData(pData,fFrame); }
	virtual HRESULT RotationTimeLineIsKeyFrame(int nFrame)      { return m_fRotationTimeLine.IsKeyFrame(nFrame); }
	virtual HRESULT RotationTimeLineInsertKeyFrame(int nFrame,float& Data) { return m_fRotationTimeLine.InsertKeyFrame(nFrame,Data); }
	virtual INT UVTimeLineGetSize()                             { return (INT)m_vUVTimeLine.GetSize(); }
	virtual HRESULT UVTimeLineRemoveKeyFrame(INT nFrame)        { return m_vUVTimeLine.RemoveKeyFrame(nFrame); }
	virtual HRESULT UVTimeLineGetData(D3DXVECTOR2 *pData,float fFrame) { return m_vUVTimeLine.GetData(pData,fFrame); }
	virtual HRESULT UVTimeLineIsKeyFrame(int nFrame)            { return m_vUVTimeLine.IsKeyFrame(nFrame); }
	virtual HRESULT UVTimeLineInsertKeyFrame(int nFrame,D3DXVECTOR2 Data) { return m_vUVTimeLine.InsertKeyFrame(nFrame,Data); }
	virtual TKG3DTimeLinebase* GetWidthTimeLine()               { return &m_fWidthTimeLine; }
	virtual TKG3DTimeLinebase* GetHeightTimeLine()              { return &m_fHeightTimeLine; }
	virtual TKG3DTimeLinebase* GetDiffuseTimeLine()             { return &m_DiffuseTimeLine; }
	virtual TKG3DTimeLinebase* GetTextureFrameIndex()           { return &m_nTextureFrameIndex; }
	virtual TKG3DTimeLinebase* GetTexture2FrameIndex()          { return &m_nTexture2FrameIndex; }
	virtual TKG3DTimeLinebase* GetRotationTimeLine()            { return &m_fRotationTimeLine; }
	virtual TKG3DTimeLinebase* GetUVTimeLine()                  { return &m_vUVTimeLine; }
	virtual TKG3DTimeLinebase* GetPositionLine()                { return &m_vPositionLine; }
    virtual void               SetWidht(float fValue)           { m_fWidth = fValue; }
    virtual float              GetWidht()                       { return m_fWidth; }
    virtual void               SetHeight(float fValue)          { m_fHeight = fValue; }
    virtual float              GetHeight()                      { return m_fHeight; }
    virtual void               InsertRtsKeyFrame(int nFrame, DWORD dwFlag);
    virtual void               SetBillboardType(DWORD dwType)   { m_dwBillBoradType = dwType; }
    virtual DWORD              GetBillboardType()               { return m_dwBillBoradType; }

    void ScanlTimeLine(float scanl) {
        m_vPositionLine.Scanl(scanl);
        m_fWidthTimeLine.Scanl(scanl);
        m_fHeightTimeLine.Scanl(scanl);
        m_DiffuseTimeLine.Scanl(scanl);
        m_nTextureFrameIndex.Scanl(scanl);
        m_nTexture2FrameIndex.Scanl(scanl);
        m_vUVTimeLine.Scanl(scanl);
        m_fRotationTimeLine.Scanl(scanl);
    }

    virtual HRESULT OnLostDevice();
    virtual HRESULT OnResetDevice();

    // editor
    virtual BOOL IsHide()               { return m_nHide;           }
    virtual void Show(BOOL enable)      { m_nHide = enable;         }
    virtual LPCTSTR GetName()           { return m_scName.c_str();  }
    virtual void SetName(LPCTSTR szName){ m_scName = szName;        } 


	HRESULT CreateVertexBuffer();

public:
    TKG3DTimeLine<D3DXVECTOR3>          m_vPositionLine;
    TKG3DTimeLine<float>                m_fWidthTimeLine;
    TKG3DTimeLine<float>                m_fHeightTimeLine;
    TKG3DTimeLine<D3DXCOLOR>            m_DiffuseTimeLine;
    TKG3DTimeLine<int>                  m_nTextureFrameIndex;
    TKG3DTimeLine<int>                  m_nTexture2FrameIndex;
    TKG3DTimeLine<D3DXVECTOR2>          m_vUVTimeLine;
    TKG3DTimeLine<float>                m_fRotationTimeLine;

private:
    IDirect3DVertexBuffer9* m_pVB;
    KG3DTexture*    m_pTexture;
    DWORD           m_dwCuttingNum;

    KG3DTexture*    m_pTexture2;
    DWORD           m_dwCuttingNum2;

    D3DXVECTOR3     m_Translation;
    float           m_Rotation;
    float           m_fHeight;
    float           m_fWidth;
    D3DXCOLOR       m_Color;
    
    //BOOL            m_bLockAxis;
    //BOOL            m_bRotation;

    float           m_fCurrentFrame;

    BOOL            m_bBindToBone;
    std::string     m_strBoneName;

    KG3DSFX         *m_pSFX;

    DWORD           m_dwBindTrackIndex;

    SFX_BLEND_MODE  m_eBlendMode;

    // editor
    BOOL    m_nHide;
    std::string m_scName;

	AABBOX          m_AABBox;
public :
    DWORD           m_dwRenderTargetTexFlag;
    D3DXMATRIX      m_matWorld;
    D3DXMATRIX      m_Matrix;
    
    DWORD           m_dwBillBoradType;

	virtual DWORD GetRenderTargetTexFlag() { return m_dwRenderTargetTexFlag; }
	virtual void  SetRenderTargetTexFlag(DWORD val) { m_dwRenderTargetTexFlag = val; }
};


#endif //_INCLUDE_KG3DSFXBILLBOARD_H_
