////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DFontTexture.h
//  Version     : 1.0
//  Creator     : dengchao
//  Create Date : 2006-02-13 16:43:32
//  Comment     : font texture 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DFONTTEXTURE_H_
#define _INCLUDE_KG3DFONTTEXTURE_H_

#include "../../../Include/KG3DEngine/KG3DInterface.h"
#include "./KG3DFontDescr.h"

struct FT_FaceRec_;
struct KG3DTextureGlyph;
class KG3DFontRenderer;

struct KG3DTextureGlyph
{
    FLOAT xAdvance;
    FLOAT yAdvance;

    INT	nTextureID;
    INT	nBoderTextureID;

    D3DXVECTOR2 vFontU;
    D3DXVECTOR2 vFontV;

    D3DXVECTOR2 vBorderU;
    D3DXVECTOR2 vBorderV;

    FLOAT xOffset;
    FLOAT yOffset;

    FLOAT cx;
    FLOAT cy;
};

class KG3DFontTexture
{
private:
    typedef std::vector<UINT>  KTextureIndexArray;

public:
    KG3DFontTexture( KG3DFontRenderer* pFontRenderer );
    ~KG3DFontTexture();

    BOOL Initialize(LPCSTR szName, float fPixel, DWORD dwStyle);
    BOOL RestoreDevice(KG3DFontRenderer* pFontRender);
    BOOL CleanUp();

    HRESULT OnLostDevice();
    HRESULT OnResetDevice();

    void LoadText(LPCWSTR wszText);
    void Draw2DText(FLOAT x, FLOAT y, LPCWSTR wszText, FLOAT cchSpace);

    void SetTextAlpha( BYTE alpha );
    void SetTextColor(DWORD dwColor);
    void SetTextScaling(FLOAT fScalingX, FLOAT fScalingY);

    BOOL GetTextExtent(LPCWSTR wszText, int cchText, FLOAT &fWidth, FLOAT &fHeight, FLOAT cchSpace = 0.0f, BOOL bBorder = FALSE);
    BOOL GetTextPosExtent(LPCWSTR wszText, int cchText, FLOAT fCursorPos, LPINT lpCharPos, FLOAT cchSpace = 0.0f, BOOL bBorder = FALSE);

    void SetTextEffect(DWORD dwEffect);
    void SetTextBorder(DWORD dwColor, DWORD dwWeight);
    void SetTextShadow(DWORD dwColor, DWORD dwOffset);

    FLOAT GetCharWidth() const;
    FLOAT GetCharHeight() const;

private:
    bool InitGlyph(UINT charCode, void* ftGlyph, KG3DTextureGlyph* fontGlyph, BOOL bBorder);

    bool InitGlyphBorder(UINT charCode, void* ftGlyph, KG3DTextureGlyph* fontGlyph);
    void DrawChar(FLOAT penX, FLOAT penY, WCHAR chText);
	void DrawCharVertical(FLOAT penX, FLOAT penY, WCHAR chText);

    bool IsLoaded(UINT charCode, BOOL bBorder);

    void DrawString(FLOAT x, FLOAT y, FLOAT xSpace, FLOAT ySpace, LPCWSTR wszText);

    bool charGlyphHit( UINT charCode);
    bool GetGlyphAdvance(UINT chText, UINT chNext, FLOAT& fX, FLOAT& fY);

    bool GetGlyphAdvance(UINT chText, FLOAT& fX, FLOAT& fY);
    bool charGlyphKernAdvance( UINT chText, UINT chNext, FLOAT& advX, FLOAT& advY );

private:
    void LoadAlphaAdjustTable(LPCSTR szFontName, INT nFontSize);

    UINT GetTextureIndex();

protected:
    KG3DFontRenderer* m_pRenderer;

    struct FT_FaceRec_* m_pFontFace;

    struct
    {
        char szName[MAX_PATH];
        float fPixel;
        DWORD dwStyle;
        DWORD dwRenderMode;
        DWORD dwLoadFlag;
    } m_InitParams;

    BOOL m_bVertical;
    UINT m_uMipmapLevel;

    UCHAR m_uchAdjustBuffer[256];

    ////////////////////////////////////////////////
    // size member
    FLOAT			 m_fontWidth;
    FLOAT			 m_fontHeight;

    FLOAT			 m_fontAscender;
    FLOAT			 m_fontDescender;


    DWORD m_dwFontColor;

    FLOAT m_fSpaceX;
    FLOAT m_fSpaceY;

    FLOAT m_fScalingX;
    FLOAT m_fScalingY;

    DWORD m_dwEffect;
    DWORD m_dwBorderColor;
    DWORD m_dwBorderWeight;

    DWORD m_dwShadowColor;
    DWORD m_dwShadowOffset;

    UINT m_uEmptyTextureX;
    UINT m_uEmptyTextureY;

    UINT m_uTextureWidth;
    UINT m_uTextureHeight;

    KTextureIndexArray	 m_aTextureIndex;

    KG3DTextureGlyph* m_pGlyphs[KG3DFONT_GLYPH_LIMIT];

    KG3DFontTLVertex m_aVertex[KG3DUI_TEXT_LEN * 4 * 24];
    INT m_nTextureID;
    UINT m_uVertex;
};



#endif //_INCLUDE_KG3DFONTTEXTURE_H_