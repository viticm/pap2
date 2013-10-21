////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DFontTexture.cpp
//  Version     : 1.0
//  Creator     : dengchao
//  Create Date : 2006-02-13 16:43:32
//  Comment     : font texture
//
////////////////////////////////////////////////////////////////////////////////


#include  "stdafx.h"

#include  <ft2build.h>
#include  FT_FREETYPE_H
//#include  FT_INTERNAL_OBJECTS_H 

#include "./KG3DFontTexture.h"
#include "./KG3DFontRenderer.h"
#include "./ksmallobjectpool.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define SCREEN_COORDINATE_AJUST
//#define KG3D_FONT_IN_PAK

namespace
{
	inline float FontFixDiv(FT_Pos pos)
	{
		return (float)pos / 64.0f;
	}

    inline FT_F26Dot6 FontFixMul(float fPixel)
    {
        return (FT_F26Dot6)(fPixel * 64.0f);
    }

	class KG3DFTFontLibrary
	{
	public:
		~KG3DFTFontLibrary();
		
		static KG3DFTFontLibrary* Instance();

	private:
		KG3DFTFontLibrary();
		KG3DFTFontLibrary( KG3DFTFontLibrary const& rhs );
		KG3DFTFontLibrary& operator=( KG3DFTFontLibrary const& rhs );

	public:
		FT_Library	m_library;

	private:
		static std::auto_ptr<KG3DFTFontLibrary> m_pImpl;
	};


	std::auto_ptr<KG3DFTFontLibrary>  KG3DFTFontLibrary::m_pImpl;

	KG3DFTFontLibrary::KG3DFTFontLibrary()
	{
	}

	KG3DFTFontLibrary::~KG3DFTFontLibrary()
	{
		if (m_library)
		{
			FT_Done_FreeType(m_library);
			m_library = NULL;
		}
	}

		// singleton
	KG3DFTFontLibrary* KG3DFTFontLibrary::Instance()
	{
		if ( NULL == m_pImpl.get() )
		{
			KG3DFTFontLibrary* pTemp = new KG3DFTFontLibrary;
			if (pTemp)
			{
				m_pImpl.reset(pTemp);
				if (FT_Init_FreeType(&m_pImpl->m_library))
				{
					m_pImpl.reset();
				}
			}
		}

		return m_pImpl.get();
	}


	inline void FillGlyph(KG3DTextureGlyph* pGlyph, FT_GlyphSlot pGlyphSlot)
	{
		ASSERT(pGlyph);
		ASSERT(pGlyphSlot);

		pGlyph->xAdvance = FontFixDiv(pGlyphSlot->advance.x);
		pGlyph->yAdvance = FontFixDiv(pGlyphSlot->advance.y);
	}

    inline void FillVertex(KG3DFontTLVertex& vertex, FLOAT x, FLOAT y, DWORD diffuse, float u, float v)
	{
		vertex.x = x;
		vertex.y = y;
		vertex.z = 0.0f;		
		vertex.diffuse = diffuse;
		vertex.u = u;			
		vertex.v = v;
	}		

	inline void FillRectVertex(KG3DFontTLVertex* pVertex, UINT& uVertex, FLOAT ux, FLOAT uy, FLOAT vx, FLOAT vy, FLOAT tux, FLOAT tuy, FLOAT tvx, FLOAT tvy, DWORD diffuse)
	{
		FillVertex(pVertex[uVertex++], ux, uy, diffuse, tux, tuy);
		FillVertex(pVertex[uVertex++], vx, uy, diffuse, tvx, tuy);
		FillVertex(pVertex[uVertex++], vx, vy, diffuse, tvx, tvy);
		FillVertex(pVertex[uVertex++], ux, vy, diffuse, tux, tvy);
	}

	inline void FillRectVertexVertical(KG3DFontTLVertex* pVertex, UINT& uVertex, FLOAT ux, FLOAT uy, FLOAT vx, FLOAT vy, FLOAT tux, FLOAT tuy, FLOAT tvx, FLOAT tvy, DWORD diffuse)
	{
		FillVertex(pVertex[uVertex++], ux, uy, diffuse, tux, tvy);
		FillVertex(pVertex[uVertex++], vx, uy, diffuse, tux, tuy);
		FillVertex(pVertex[uVertex++], vx, vy, diffuse, tvx, tuy);
		FillVertex(pVertex[uVertex++], ux, vy, diffuse, tvx, tvy);
	}

	KSmallObjectPool g_Allocator(_T("KG3DTextureGlyph"), sizeof(KG3DTextureGlyph));

	inline bool IsVerticalChar(WCHAR chText)
	{
		//  ISO10646所有字面中，目前僅有第0、第1和第2字面真正收容
		//	編碼字元。ISO10646的BMP和Unicode的編碼字元，依其UCS-2編
		//	碼序介紹如下：

		//	(1)0000～007Fh：基本拉丁字母區。其中0000～001Fh為C0控
		//	制碼，0020h為空格（space），0021～007Eh為ASCII圖形
		//	字元，007Fh為控制碼DEL。事實上，這128個字元碼只要
		//	把前8個位元去掉就可變成習見的8位元形式的ASCII碼。

		//	(2)0080～00A0h：控制碼區。其中0080～009Fh為C1控制碼，
		//	00A0h為不中斷空格（no-break space）。

		//	(3)00A1～1FFFh：拼音文字區。收容除基本拉丁字母以外的各種
		//	拼音文字字元，包括歐洲各國語言、希臘文、斯拉夫語文、
		//	希伯來文、阿拉伯文、亞美尼亞文、印度各地方言、馬來文、
		//	泰文、寮文、柬普寨文、滿文、蒙文、藏文、印地安語文等。

		//	(4)2000～28FFh：符號區。收容各種符號，包括標點符號、上下
		//	標、錢幣符號、數字、箭頭、數學符號、工程符號、光學辨
		//	識符號、帶圈或帶括符的文數字、表格繪製符號、地理圖示、
		//	盲用點字、裝飾圖形等。

		//	(5)2E80～33FFh：中日韓符號區。收容康熙字典部首、中日韓輔
		//	助部首、注音符號、日本假名、韓文音符，中日韓的符號、
		//	標點、帶圈或帶括符文數字、月份，以及日本的假名組合、
		//	單位、年號、月份、日期、時間等。

		//	(6)3400～4DFFh：中日韓認同表意文字擴充A區，總計收容6,582
		//	個中日韓漢字。

		//	(7)4E00～9FFFh：中日韓認同表意文字區，總計收容20,902個中
		//	日韓漢字。

		//	(8)A000～A4FFh：彝族文字區，收容中國南方彝族文字和字根。

		//	(9)AC00～D7FFh：韓文拼音組合字區，收容以韓文音符拼成的文
		//	字。

		//	(10)D800～DFFFh：S區，專用於UTF-16，詳情後敘。

		//	(11)E000～F8FFh：專用字區，其內容WG2不予規定，保留供使用
		//	者自行添加ISO10646未收容的字元。

		//	(12)F900～FAFFh：中日韓相容表意文字區，總計收容302個中日
		//	韓漢字。何謂相容表意文字，留待後敘。

		//	(13)FB00～FFFDh：文字表現形式區，收容組合拉丁文字、希伯來
		//	文、阿拉伯文、中日韓直式標點、小符號、半形符號、全形
		//	符號等。

		//return (chText < 0x4E00 || (chText > 0x9FFF && chText < 0xFB00) || (chText > 0xFFFD));

		switch (chText)
		{
		case L'<':
		case L'《':
		case L'>':
		case L'》':
		case L'[':
		case L'【':
		case L']':
		case L'】':
		case L'{':
		case L'｛':
		case L'}':
		case L'｝':
		case L'(':
		case L'（':
		case L')':
		case L'）':
		case L'-':
		case L'—':
		case L'…':
			return true;
		default:
			return false;
		}
		return false;
	}

    struct KG3DFontFacePoolItem
    {
        ULONG ulRefCount;
        FT_Library library;
        FT_Long nFaceIndex;
        FT_Face face;
        TCHAR szFilePath[MAX_PATH];

#ifdef KG3D_FONT_IN_PAK
        LPBYTE pFontBuffer;
#endif
    };

    typedef std::vector<KG3DFontFacePoolItem> KG3DFontFacePoolItemList;

    class KG3DFontFacePool
    {
    public:
        ~KG3DFontFacePool()
        {
            ASSERT(m_Items.empty());
        }

        int Load(FT_Library library, LPCSTR szFilePath, FT_Long nFaceIndex, FT_Face* pFace)
        {
            int nRetCode = false;
            int nResult = false;
#ifdef KG3D_FONT_IN_PAK
            FT_Long nFontBufferSize = 0;
            PBYTE pFontBuffer = NULL;
            KSmartFile File;
#else
            char szFullPath[MAX_PATH];
#endif

            ASSERT(pFace);
            ASSERT(*pFace == NULL);

            ASSERT(szFilePath);

            for (size_t nIndex = 0; nIndex < m_Items.size(); ++nIndex)
            {
                KG3DFontFacePoolItem& Item = m_Items[nIndex];

                if (Item.library == library && Item.nFaceIndex == nFaceIndex && _tcscmp(Item.szFilePath, szFilePath) == 0)
                {
                    *pFace = Item.face;

                    ++Item.ulRefCount;

                    break;
                }
            }

            if (*pFace == NULL)
            {
                KG3DFontFacePoolItem Item;

#ifndef KG3D_FONT_IN_PAK
                g_GetFullPath(szFullPath, szFilePath);

                nRetCode = FT_New_Face(library, szFullPath, nFaceIndex, &Item.face) == 0;
                KGLOG_PROCESS_ERROR(nRetCode);
#else
                File = g_OpenFile(szFilePath);
                KGLOG_PROCESS_ERROR(File);

                nFontBufferSize = (FT_Long)File->Size();
                KGLOG_PROCESS_ERROR(nFontBufferSize > 0);

                pFontBuffer = (PBYTE)malloc(nFontBufferSize);
                KGLOG_PROCESS_ERROR(pFontBuffer);

                nRetCode = (FT_Long)File->Read(pFontBuffer, nFontBufferSize) == nFontBufferSize;
                KGLOG_PROCESS_ERROR(nRetCode);

                nRetCode = FT_New_Memory_Face(library, pFontBuffer, nFontBufferSize, nFaceIndex, &Item.face) == 0;
                KGLOG_PROCESS_ERROR(nRetCode);

                Item.pFontBuffer = pFontBuffer;
                pFontBuffer = NULL;
#endif

                Item.ulRefCount = 1;
                Item.library = library;
                Item.nFaceIndex = nFaceIndex;

                _tcsncpy(Item.szFilePath, szFilePath, _countof(Item.szFilePath));
                Item.szFilePath[_countof(Item.szFilePath) - 1] = _T('\0');

                m_Items.push_back(Item);

                *pFace = Item.face;
            }

            nResult = true;
Exit0:
            if (!nResult)
            {
                KGLogPrintf(KGLOG_ERR, "KG3DFontFacePool::Load(0x%.8x, %s, %d, 0x%.8x)\n", library, szFilePath, nFaceIndex, pFace ? *pFace : NULL);

#ifdef KG3D_FONT_IN_PAK
                SAFE_FREE(pFontBuffer);
#endif
            }
            return nResult;
        }

        int Unload(FT_Face face)
        {
            int nResult = false;
            int nFound = false;
            FT_Error nError = 0;

            for (KG3DFontFacePoolItemList::iterator it = m_Items.begin(); it != m_Items.end(); ++it)
            {
                KG3DFontFacePoolItem& Item = *it;

                if (Item.face == face)
                {
                    nFound = true;

                    if (--Item.ulRefCount == 0)
                    {
                        nError = FT_Done_Face(face);
                        KGLOG_PROCESS_ERROR(nError == 0);

#ifdef KG3D_FONT_IN_PAK
                        SAFE_FREE(Item.pFontBuffer);
#endif

                        m_Items.erase(it); 
                    }

                    break;
                }
            }

            nResult = nFound;
Exit0:
            if (!nResult)
            {
                KGLogPrintf(KGLOG_ERR, "KG3DFontFacePool::Unload(0x%.8x)\n", face);
            }
            return nResult;
        }

    private:
        KG3DFontFacePoolItemList m_Items;
    };

    KG3DFontFacePool s_FontFacePool;
}


KG3DFontTexture::KG3DFontTexture( KG3DFontRenderer* pFontRenderer )
: m_pRenderer(pFontRenderer)
, m_pFontFace(NULL)
, m_bVertical(FALSE)
, m_uMipmapLevel(1)
, m_fontAscender(0.0f)
, m_fontDescender(0.0f)
, m_dwFontColor(0xFFFFFFFF)
, m_fScalingX(1.0f)
, m_fScalingY(1.0f)
, m_dwEffect(KG3DUI_TEXT_STYLE_NONE)
, m_dwBorderColor(0xFF000000)
, m_dwBorderWeight(1)
, m_dwShadowColor(0xFF000000)
, m_dwShadowOffset(1)
, m_nTextureID(-1)
, m_uVertex(0)
{
	memset(m_pGlyphs, 0, sizeof(m_pGlyphs));

	for (size_t nIndex = 0; nIndex < _countof(m_uchAdjustBuffer); ++nIndex)
	{
		m_uchAdjustBuffer[nIndex] = (UCHAR)nIndex;
	}
}

KG3DFontTexture::~KG3DFontTexture()
{
}


BOOL KG3DFontTexture::RestoreDevice(KG3DFontRenderer* pFontRender)
{
    m_pRenderer = pFontRender;
	return Initialize(m_InitParams.szName, m_InitParams.fPixel, m_InitParams.dwStyle);
}

BOOL KG3DFontTexture::Initialize(LPCSTR szName, float fPixel, DWORD dwStyle)
{
	static FT_Long const DEFAULT_FACE_INDEX = 0;

	int nRetCode = false;
    int nResult = false;
	KG3DFTFontLibrary* pFontLibrary = NULL;
	FT_Face pFontFace = NULL;
	FT_Error nError = 0;
    FT_F26Dot6 FontSize = 0;
	BOOL bUnicode = FALSE;
	UINT uEncoding = FT_ENCODING_UNICODE;

	if (dwStyle & KG3DUI_TEXT_STYLE_MONO)
	{
		m_InitParams.dwRenderMode = FT_RENDER_MODE_MONO;
		m_InitParams.dwLoadFlag = FT_LOAD_DEFAULT | FT_LOAD_IGNORE_GLOBAL_ADVANCE_WIDTH | FT_LOAD_NO_HINTING;
	}
	else
	{
		m_InitParams.dwRenderMode = FT_RENDER_MODE_LIGHT;
		m_InitParams.dwLoadFlag = FT_LOAD_TARGET_LIGHT | FT_LOAD_MONOCHROME;
	}

    _tcsncpy(m_InitParams.szName, szName, _countof(m_InitParams.szName));
    m_InitParams.szName[_countof(m_InitParams.szName) - 1] = _T('\0');

	m_InitParams.fPixel = fPixel;
    m_InitParams.dwStyle = dwStyle;

	pFontLibrary = KG3DFTFontLibrary::Instance();
	KGLOG_PROCESS_ERROR(pFontLibrary);

    nRetCode = s_FontFacePool.Load(pFontLibrary->m_library, m_InitParams.szName, DEFAULT_FACE_INDEX, &pFontFace);
    KGLOG_PROCESS_ERROR(nRetCode);

	ASSERT(pFontFace);

	nRetCode = FT_IS_SCALABLE(pFontFace);
	KGLOG_PROCESS_ERROR(nRetCode);

	KGLOG_PROCESS_ERROR(pFontFace->charmap);

	for (int i = 0; i < pFontFace->num_charmaps; ++i)
	{
		if (FT_Encoding(FT_ENCODING_UNICODE) == pFontFace->charmaps[i]->encoding)
		{
			bUnicode = TRUE;
			break;
		}
	}

	if (!bUnicode)
	{
		uEncoding = pFontFace->charmaps[0]->encoding;
		bUnicode = uEncoding == FT_ENCODING_UNICODE;
	}

	nError = FT_Select_Charmap(pFontFace, FT_Encoding(uEncoding));
	KGLOG_PROCESS_ERROR(nError == 0);

    FontSize = FontFixMul(fPixel);
    nError = FT_Set_Char_Size(pFontFace, FontSize, 0, 0, 0);
    KGLOG_PROCESS_ERROR(nError == 0);

	LoadAlphaAdjustTable(szName, (INT)fPixel);

	m_pFontFace	= pFontFace;
    pFontFace = NULL;

	m_dwEffect = dwStyle & ~KG3DUI_TEXT_STYLE_VERTICAL;
	m_bVertical = dwStyle & KG3DUI_TEXT_STYLE_VERTICAL;
	if (m_bVertical)
		m_InitParams.dwLoadFlag |= FT_LOAD_VERTICAL_LAYOUT;

    m_uMipmapLevel = (dwStyle & KG3DUI_TEXT_STYLE_MIPMAP) ? 8 : 1;

	if (FT_IS_SCALABLE(m_pFontFace))
	{
		m_fontWidth = (m_pFontFace->bbox.xMax - m_pFontFace->bbox.xMin)
			* ((FLOAT)(m_pFontFace->size->metrics.x_ppem) / (FLOAT)(m_pFontFace->units_per_EM));
		m_fontHeight = (m_pFontFace->bbox.yMax - m_pFontFace->bbox.yMin)
			* ((FLOAT)m_pFontFace->size->metrics.y_ppem / (FLOAT)m_pFontFace->units_per_EM);
	}
	else
	{
		m_fontWidth = FontFixDiv(m_pFontFace->size->metrics.max_advance);
		m_fontHeight = FontFixDiv(m_pFontFace->size->metrics.height);
	}
	
	m_fontWidth = ceil(m_fontWidth);
	m_fontHeight = ceil(m_fontHeight);

	m_fontAscender = FontFixDiv(m_pFontFace->size->metrics.ascender);
	m_fontDescender = FontFixDiv(m_pFontFace->size->metrics.descender);

	m_fontAscender = ceil(m_fontAscender);
	m_fontDescender = ceil(m_fontDescender);

	nResult = true;
Exit0:
    if (!nResult)
    {
        if (pFontFace)
        {
            nRetCode = s_FontFacePool.Unload(pFontFace);
            KGLOG_CHECK_ERROR(nRetCode);

            pFontFace = NULL;
        }

        CleanUp();
    }
	return nResult;
}


BOOL KG3DFontTexture::CleanUp()
{
    int nRetCode = false;

	for (size_t nIndex = 0; nIndex < m_aTextureIndex.size(); ++nIndex)
		m_pRenderer->DeleteTexture(m_aTextureIndex[nIndex]);
	m_aTextureIndex.clear();

	if (m_pFontFace)
	{
        nRetCode = s_FontFacePool.Unload(m_pFontFace);
        KGLOG_CHECK_ERROR(nRetCode);

        m_pFontFace = NULL;
	}

	for (size_t nIndex = 0; nIndex < _countof(m_pGlyphs); ++nIndex)
	{
		KG3DTextureGlyph* pGlyph = m_pGlyphs[nIndex];
		if (pGlyph == NULL)
			continue;
		g_Allocator.Free(pGlyph); 
	}

	m_fontAscender = 0.0f;
	m_fontDescender = 0.0f;

	m_dwFontColor = 0xFFFFFFFF;

	m_fScalingX	= 1.0f;
	m_fScalingY = 1.0f;

	memset(m_pGlyphs, 0, sizeof(m_pGlyphs));

	return TRUE;
}

HRESULT KG3DFontTexture::OnLostDevice()
{
    return S_OK;
}

HRESULT KG3DFontTexture::OnResetDevice()
{
    return S_OK;
}

void KG3DFontTexture::LoadAlphaAdjustTable(LPCSTR szFontName, INT nFontSize)
{
	BOOL bAdjust = FALSE;
	char szFileName[MAX_PATH];
	LPSTR szSep = NULL;
	int nLen = _countof(szFileName) - 16;

	strncpy(szFileName, szFontName, nLen);
	szFileName[nLen - 1] = '\0';

	szSep = strrchr(szFileName, '.');
	if (szSep)
	{
		sprintf(szSep, "%d.prf", nFontSize);
		if (g_IsFileExist(szFileName))
		{
			IFile* pFile = g_OpenFile(szFileName);
			if (pFile)
			{
				unsigned int nCount = pFile->Read(m_uchAdjustBuffer, sizeof(m_uchAdjustBuffer));
				if (nCount == sizeof(m_uchAdjustBuffer))
					bAdjust = TRUE;

                KG_COM_RELEASE(pFile);
			}
		}
	}
	if (!bAdjust)
	{
		for (size_t nIndex = 0; nIndex < _countof(m_uchAdjustBuffer); ++nIndex)
		{
			m_uchAdjustBuffer[nIndex] = (UCHAR)nIndex;
		}
	}
}

void KG3DFontTexture::LoadText(LPCWSTR wszText)
{
	for (; *wszText != L'\0'; ++wszText)
	{
		charGlyphHit(*wszText);
	}
}

void KG3DFontTexture::Draw2DText(FLOAT x, FLOAT y, LPCWSTR wszText, FLOAT cchSpace)
{
	if (wszText == NULL)
		return;

	if (!m_bVertical)
	{
		DrawString(x, y, cchSpace, 0.0f, wszText);
	}
	else
	{
		DrawString(x, y, 0.0f, cchSpace, wszText);
	}
}

BOOL KG3DFontTexture::GetTextExtent(LPCWSTR wszText, int cchText, FLOAT &fWidth, FLOAT &fHeight, FLOAT cchSpace, BOOL bBorder)
{
//	ASSERT(((m_dwEffect & KG3DUI_TEXT_STYLE_BORDER) != 0) == (bBorder != 0));

	if (wszText == NULL || -1 > cchText || 0  == cchText)
		return FALSE;

	FLOAT penX = 0.0f;
	FLOAT penY = 0.0f;
	FLOAT spaceX = cchSpace * m_fScalingX;
	FLOAT spaceY = cchSpace * m_fScalingY;

    if (cchText == -1)
        cchText = (int)wcslen(wszText);

    if (cchText > 0)
    {
        for (int nIndex = 0; nIndex < cchText; ++nIndex)
        {
            WCHAR wcCurrent = wszText[nIndex];
            WCHAR wcNext = wszText[nIndex + 1];

            FLOAT glyphAdvX = 0.0f;
            FLOAT glyphAdvY = 0.0f;

            if (charGlyphHit(wcCurrent))
                GetGlyphAdvance(wcCurrent, wcNext, glyphAdvX, glyphAdvY);

            penX += glyphAdvX * m_fScalingX + spaceX;
            penY += glyphAdvY * m_fScalingY + spaceY;
        }
    }


	fWidth = m_bVertical ? GetCharWidth() * m_fScalingX: penX;
	fHeight = !m_bVertical ? GetCharHeight() * m_fScalingY: penY;

	return TRUE;
}

BOOL KG3DFontTexture::GetTextPosExtent(LPCWSTR wszText, int cchText, FLOAT fCursorPos, LPINT lpCharPos, FLOAT cchSpace, BOOL bBorder)
{
//	ASSERT(((m_dwEffect & KG3DUI_TEXT_STYLE_BORDER) != 0) == (bBorder != 0));

	if (NULL == lpCharPos || NULL == wszText || -1 > cchText || 0 == cchText)
		return FALSE;

	FLOAT penX = 0.0f;
	FLOAT penEndX = fCursorPos;
	FLOAT scalingX = m_bVertical ? m_fScalingY : m_fScalingX;
	FLOAT spaceX = cchSpace * scalingX;

    LPCWSTR wszToken = wszText;

    for (; *wszToken; ++wszToken)
	{
		WCHAR const charCode = *wszToken;
		WCHAR const charNextCode = *(wszToken + 1);

		// hinting and draw
		FLOAT glyphAdvX = 0.0f;
		FLOAT glyphAdvY = 0.0f;

		if (charGlyphHit(charCode))
			GetGlyphAdvance(charCode, charNextCode, glyphAdvX, glyphAdvY);

		penX += (m_bVertical ? glyphAdvY : glyphAdvX) * scalingX + spaceX;

		if (penX > penEndX + 0.0001f)
			break;
	}

	*lpCharPos = (INT)(wszToken - wszText);
	return TRUE;
}


void KG3DFontTexture::DrawString(FLOAT x, FLOAT y, FLOAT xSpace, FLOAT ySpace, LPCWSTR wszText)
{
	ASSERT(wszText);

	while (*wszText != L'\0')
	{
		WCHAR wcCurrent = *wszText++;
		WCHAR wcNext = *wszText;

		FLOAT xAdvance = 0.0f;
        FLOAT yAdvance = 0.0f;

		if (charGlyphHit(wcCurrent)	&& GetGlyphAdvance(wcCurrent, wcNext, xAdvance, yAdvance))
		{
            if (m_uVertex + 12 >= _countof(m_aVertex))
            {
                ASSERT(m_nTextureID != -1);

                m_pRenderer->PrepareVertex(m_nTextureID, m_aVertex, m_uVertex);
                m_uVertex = 0;
                m_nTextureID = -1;
            }

			if (m_bVertical && IsVerticalChar(wcCurrent))
				DrawCharVertical(x, y, wcCurrent);
			else
				DrawChar(x, y, wcCurrent);

            x += (xAdvance + xSpace) * m_fScalingX;
            y += (yAdvance + ySpace) * m_fScalingY;
		}
	}

    if (m_uVertex)
    {
        ASSERT(m_nTextureID != -1);

        m_pRenderer->PrepareVertex(m_nTextureID, m_aVertex, m_uVertex);
        m_uVertex = 0;
        m_nTextureID = -1;
    }
}

bool KG3DFontTexture::InitGlyph(UINT charCode, void* ftGlyph, KG3DTextureGlyph* fontGlyph, BOOL bBorder)
{
	BOOL bRetCode = FALSE;
	FT_GlyphSlot pGlyphSlot = (FT_GlyphSlot)ftGlyph;
	FT_Error nError = 0;
	KG3DTextureGlyph* pGlyphFont = (KG3DTextureGlyph*)fontGlyph;
	FT_UInt nGlyph = 0;
	BOOL bBit = FALSE;
	UINT uX = 0;
	UINT uY = 0;
	int nDivisor = 1;
    FT_F26Dot6 FontSize = 0;

	ASSERT(pGlyphSlot);
	ASSERT(pGlyphFont);

	KGLOG_PROCESS_ERROR(pGlyphSlot);

	nGlyph = FT_Get_Char_Index(m_pFontFace, charCode);
	KG_PROCESS_ERROR(nGlyph != 0);

	// request glyph texture
	pGlyphFont->nTextureID = GetTextureIndex();
	KGLOG_PROCESS_ERROR(pGlyphFont->nTextureID != KG3DFontRenderer::INVALID_TEXTURE_INDEX);

    uX = m_uEmptyTextureX;
    uY = m_uEmptyTextureY;

	for (UINT uMipmapLevel = 0; uMipmapLevel < m_uMipmapLevel; ++uMipmapLevel)
	{
        FontSize = FontFixMul(m_InitParams.fPixel);
        nError = FT_Set_Char_Size(m_pFontFace, FontSize, 0, 0, 0);
		KGLOG_PROCESS_ERROR(nError == 0);

		nError = FT_Load_Glyph(m_pFontFace, nGlyph, m_InitParams.dwLoadFlag);
		KGLOG_PROCESS_ERROR(nError == 0);

        nError = FT_Render_Glyph(m_pFontFace->glyph, (FT_Render_Mode)m_InitParams.dwRenderMode);
		KGLOG_PROCESS_ERROR(nError == 0);
		KGLOG_PROCESS_ERROR(m_pFontFace->glyph->format == FT_GLYPH_FORMAT_BITMAP);

        // empty glyph?
        // 如果轮廓线没有被上载，FreeType 使用的位图渲染模式为 1bit/pixel
        bBit = pGlyphSlot->outline.n_contours == 0 && pGlyphSlot->outline.n_points == 0 || FT_PIXEL_MODE_MONO == m_pFontFace->glyph->bitmap.pixel_mode;
        if (bBit)
        {
            bRetCode = m_pRenderer->UploadMonoTexture(pGlyphFont->nTextureID, uMipmapLevel, uX / nDivisor, uY / nDivisor, 
                m_pFontFace->glyph->bitmap.width, m_pFontFace->glyph->bitmap.rows, 
                m_pFontFace->glyph->bitmap.pitch, m_pFontFace->glyph->bitmap.buffer);
            KGLOG_PROCESS_ERROR(bRetCode);
        }
        else
        {
            bRetCode = m_pRenderer->UploadTexture(pGlyphFont->nTextureID, uMipmapLevel, uX / nDivisor, uY / nDivisor, 
                m_pFontFace->glyph->bitmap.width, m_pFontFace->glyph->bitmap.rows, m_pFontFace->glyph->bitmap.buffer, m_uchAdjustBuffer);
            KGLOG_PROCESS_ERROR(bRetCode);
        }

		nDivisor *= 2;

		if (uMipmapLevel == 0)
		{
            pGlyphFont->xOffset = (FLOAT)m_pFontFace->glyph->bitmap_left;
            pGlyphFont->yOffset = (FLOAT)m_fontAscender - m_pFontFace->glyph->bitmap_top;

            pGlyphFont->cx = (FLOAT)m_pFontFace->glyph->bitmap.width;
            pGlyphFont->cy = (FLOAT)m_pFontFace->glyph->bitmap.rows;

            pGlyphFont->vFontU.x = (float)m_uEmptyTextureX / (float)m_uTextureWidth;
            pGlyphFont->vFontU.y = (float)m_uEmptyTextureY / (float)m_uTextureHeight;

            pGlyphFont->vFontV.x = (float)(m_uEmptyTextureX + m_pFontFace->glyph->bitmap.width) / (float)m_uTextureWidth;
            pGlyphFont->vFontV.y = (float)(m_uEmptyTextureY + m_pFontFace->glyph->bitmap.rows) / (float)m_uTextureHeight;

            m_uEmptyTextureX += static_cast<INT>(pGlyphFont->cx);
            m_uEmptyTextureX += KG3DFONT_DEFAULT_PADDING;
        }
	}

    FontSize = FontFixMul(m_InitParams.fPixel);
    nError = FT_Set_Char_Size(m_pFontFace, FontSize, 0, 0, 0);
    KGLOG_PROCESS_ERROR(nError == 0);

	if (bBorder && m_uMipmapLevel == 1)
	{
		pGlyphFont->nBoderTextureID = GetTextureIndex();
		KGLOG_PROCESS_ERROR(pGlyphFont->nBoderTextureID != KG3DFontRenderer::INVALID_TEXTURE_INDEX);

		pGlyphFont->vBorderU.x = (float)(m_uEmptyTextureX - 1) / (float)m_uTextureWidth;
		pGlyphFont->vBorderU.y = (float)(m_uEmptyTextureY - 1) / (float)m_uTextureHeight;

		pGlyphFont->vBorderV.x = (float)(m_uEmptyTextureX + m_pFontFace->glyph->bitmap.width + 1) / (float)m_uTextureWidth;
		pGlyphFont->vBorderV.y = (float)(m_uEmptyTextureY + m_pFontFace->glyph->bitmap.rows + 1) / (float)m_uTextureHeight;

		if (bBit)
		{
			bRetCode = m_pRenderer->UploadMonoBoderTexture(pGlyphFont->nBoderTextureID, 0, m_uEmptyTextureX, m_uEmptyTextureY, 
				(UINT)pGlyphFont->cx,
				(UINT)pGlyphFont->cy,
				(UINT)m_pFontFace->glyph->bitmap.pitch,
				m_pFontFace->glyph->bitmap.buffer);
			KGLOG_PROCESS_ERROR(bRetCode);
		}
		else
		{
			bRetCode = m_pRenderer->UploadBoderTexture(pGlyphFont->nBoderTextureID, 0, m_uEmptyTextureX, m_uEmptyTextureY, 
				(UINT)pGlyphFont->cx,
				(UINT)pGlyphFont->cy, 
				m_pFontFace->glyph->bitmap.buffer, m_uchAdjustBuffer);
			KGLOG_PROCESS_ERROR(bRetCode);
		}

		m_uEmptyTextureX += static_cast<INT>( pGlyphFont->cx );
		m_uEmptyTextureX += KG3DFONT_DEFAULT_PADDING;
	}

	return true;
Exit0:
	return false;
}


bool KG3DFontTexture::InitGlyphBorder(UINT charCode, void* ftGlyph, KG3DTextureGlyph* fontGlyph)
{
	BOOL bRetCode = FALSE;
	FT_GlyphSlot pGlyphSlot = (FT_GlyphSlot)ftGlyph;
	FT_Error nError = 0;
	FT_Bitmap bitmap;
	FT_UInt nGlyph = 0;
    FT_F26Dot6 FontSize = 0;
    KG3DTextureGlyph* pGlyphFont = (KG3DTextureGlyph*)fontGlyph;

	ASSERT(pGlyphSlot);
	ASSERT(pGlyphFont);

	KGLOG_PROCESS_ERROR(pGlyphSlot);

	nGlyph = FT_Get_Char_Index(m_pFontFace, charCode);
	KGLOG_PROCESS_ERROR(nGlyph != 0);

    FontSize = FontFixMul(m_InitParams.fPixel);
    nError = FT_Set_Char_Size(m_pFontFace, FontSize, 0, 0, 0);
	KGLOG_PROCESS_ERROR(nError == 0);

	nError = FT_Load_Glyph(m_pFontFace, nGlyph, m_InitParams.dwLoadFlag);
	KGLOG_PROCESS_ERROR(nError == 0);

	nError = FT_Render_Glyph(pGlyphSlot, (FT_Render_Mode)m_InitParams.dwRenderMode);
	KGLOG_PROCESS_ERROR(nError == 0);
	KGLOG_PROCESS_ERROR(pGlyphSlot->format == FT_GLYPH_FORMAT_BITMAP);

	// request glyph texture
	pGlyphFont->nBoderTextureID = GetTextureIndex();
	KGLOG_PROCESS_ERROR(pGlyphFont->nBoderTextureID != KG3DFontRenderer::INVALID_TEXTURE_INDEX);

	// upload sub texture
	bitmap = pGlyphSlot->bitmap;

	pGlyphFont->vBorderU.x = (float)(m_uEmptyTextureX - 1) / (float)m_uTextureWidth;
	pGlyphFont->vBorderU.y = (float)(m_uEmptyTextureY - 1) / (float)m_uTextureHeight;

	pGlyphFont->vBorderV.x = (float)(m_uEmptyTextureX + bitmap.width + 1) / (float)m_uTextureWidth;
	pGlyphFont->vBorderV.y = (float)(m_uEmptyTextureY + bitmap.rows + 1) / (float)m_uTextureHeight;

	// empty glyph?
	// 如果轮廓线没有被上载，FreeType 使用的位图渲染模式为 1bit/pixel
	if (0 == pGlyphSlot->outline.n_contours && 0 == pGlyphSlot->outline.n_points || FT_PIXEL_MODE_MONO == bitmap.pixel_mode)
	{
		bRetCode = m_pRenderer->UploadMonoBoderTexture(pGlyphFont->nBoderTextureID, 0, m_uEmptyTextureX, m_uEmptyTextureY, 
			(UINT)pGlyphFont->cx,
			(UINT)pGlyphFont->cy,
			(UINT)bitmap.pitch,
			bitmap.buffer);
		KGLOG_PROCESS_ERROR(bRetCode);
	}
	else
	{
		bRetCode = m_pRenderer->UploadBoderTexture(pGlyphFont->nBoderTextureID, 0, m_uEmptyTextureX, m_uEmptyTextureY, 
			(UINT)pGlyphFont->cx,
			(UINT)pGlyphFont->cy, 
			bitmap.buffer, m_uchAdjustBuffer);
		KGLOG_PROCESS_ERROR(bRetCode);
	}

	m_uEmptyTextureX += static_cast<INT>( pGlyphFont->cx );
	m_uEmptyTextureX += KG3DFONT_DEFAULT_PADDING;

	return true;
Exit0:
	return false;
}

bool KG3DFontTexture::charGlyphHit(UINT charCode)
{
	BOOL bRetCode = FALSE;
	FT_UInt nGlyph = 0;
	FT_Error nError = 0;
	KG3DTextureGlyph* pGlyph = NULL;
	FT_GlyphSlot pGlyphSlot = NULL;
    FT_F26Dot6 FontSize = 0;

	BOOL bBorder = (m_dwEffect & KG3DUI_TEXT_STYLE_BORDER) != 0;

	KG_PROCESS_SUCCESS(IsLoaded(charCode, bBorder));

	nGlyph = FT_Get_Char_Index(m_pFontFace, charCode);
	KG_PROCESS_ERROR(nGlyph != 0);

    FontSize = FontFixMul(m_InitParams.fPixel);
    nError = FT_Set_Char_Size(m_pFontFace, FontSize, 0, 0, 0);
	KGLOG_PROCESS_ERROR(nError == 0);

	nError = FT_Load_Glyph(m_pFontFace, nGlyph, m_InitParams.dwLoadFlag);
	KGLOG_PROCESS_ERROR(nError == 0);

	ASSERT(m_pFontFace);
	ASSERT(m_pFontFace->glyph);

	pGlyphSlot = m_pFontFace->glyph;

	pGlyph = (KG3DTextureGlyph*)g_Allocator.Alloc();
	KGLOG_PROCESS_ERROR(pGlyph);
	
	pGlyph->nBoderTextureID = KG3DFontRenderer::INVALID_TEXTURE_INDEX;

	FillGlyph(pGlyph, pGlyphSlot);

	bRetCode = InitGlyph(charCode, pGlyphSlot, pGlyph, bBorder);
	KG_PROCESS_ERROR(bRetCode);

	m_pGlyphs[charCode] = pGlyph;

Exit1:
	return true;
Exit0:
	if (pGlyph)
		g_Allocator.Free(pGlyph); 
	return false;
}

bool KG3DFontTexture::GetGlyphAdvance(UINT chText, UINT chNext, FLOAT& fX, FLOAT& fY)
{
	FLOAT glyphAdvX = 0.0f;
	FLOAT glyphAdvY = 0.0f;

	if (GetGlyphAdvance(chText, glyphAdvX, glyphAdvY))
	{
		FLOAT glyphKernX = 0.0f;
		FLOAT glyphKernY = 0.0f;
		if (charGlyphKernAdvance(chText, chNext, glyphKernX, glyphKernY))
		{
			glyphAdvX += glyphKernX;
			glyphAdvY += glyphKernY;
		}
	}
	fX = glyphAdvX;
	fY = glyphAdvY;
	return true;
}


bool KG3DFontTexture::GetGlyphAdvance(UINT chText, FLOAT& fX, FLOAT& fY)
{
	KG3DTextureGlyph* pGlyph = m_pGlyphs[chText];
	if (pGlyph == NULL)
	{
		fX = 0.0f;
		fY = 0.0f;
		return false;
	}

	fX = pGlyph->xAdvance;
	fY = pGlyph->yAdvance;
	return true;
}


bool KG3DFontTexture::charGlyphKernAdvance(UINT chText, UINT chNext, FLOAT& advX, FLOAT& advY)
{
	FT_Error err = 0;

	if (FT_HAS_KERNING(m_pFontFace) && chText && chNext )
	{
		FT_Vector kernAdvance = { 0, 0 } ;

		err = FT_Get_Kerning(m_pFontFace, chText, chNext, FT_KERNING_UNFITTED, &kernAdvance);
		if ( 0 == err )
		{   
			advX = FontFixDiv(kernAdvance.x);
			advY = FontFixDiv(kernAdvance.y);
		}
	}
	else
	{
		advX = 0.0f;
		advY = 0.0f;
	}

	return ( 0 == err );
}

void KG3DFontTexture::DrawChar(FLOAT x, FLOAT y, WCHAR wc)
{
	KG3DTextureGlyph* pGlyph = m_pGlyphs[wc];
	if (pGlyph)
	{
        // prepare tl vertex buffer
		FLOAT cx = pGlyph->cx * m_fScalingX;
		FLOAT cy = pGlyph->cy * m_fScalingY;

		x += pGlyph->xOffset * m_fScalingX;
		y += pGlyph->yOffset * m_fScalingY;

#ifdef SCREEN_COORDINATE_AJUST
		x = floor(x + 0.5f) - 0.5f;
		y = floor(y + 0.5f) - 0.5f;
#endif

		FLOAT ux = pGlyph->vFontU.x;
		FLOAT uy = pGlyph->vFontU.y;
		FLOAT vx = pGlyph->vFontV.x;
		FLOAT vy = pGlyph->vFontV.y;

        if (m_dwEffect & KG3DUI_TEXT_STYLE_BORDER)
		{
            if (m_nTextureID != pGlyph->nBoderTextureID)
            {
                if (m_uVertex)
                {
                    ASSERT(m_nTextureID != -1);

                    m_pRenderer->PrepareVertex(m_nTextureID, m_aVertex, m_uVertex);
                    m_uVertex = 0;
                }
                m_nTextureID = pGlyph->nBoderTextureID;
            }

            FLOAT uxB = pGlyph->vBorderU.x;
            FLOAT uyB = pGlyph->vBorderU.y;
            FLOAT vxB = pGlyph->vBorderV.x;
            FLOAT vyB = pGlyph->vBorderV.y;

			FillRectVertex(m_aVertex, m_uVertex, x - 1.0f, y - 1.0f, x + cx + 1.0f, y + cy + 1.0f, uxB, uyB, vxB, vyB, m_dwBorderColor);
        }

        if (pGlyph->nTextureID != m_nTextureID)
        {
            if (m_uVertex)
            {
                ASSERT(m_nTextureID != -1);

                m_pRenderer->PrepareVertex(m_nTextureID, m_aVertex, m_uVertex);
                m_uVertex = 0;
            }

            m_nTextureID = pGlyph->nTextureID;
        }

		if (m_dwEffect & KG3DUI_TEXT_STYLE_SHADOW)
		{
			FLOAT xs = x + (FLOAT)m_dwShadowOffset;
			FLOAT ys = y + (FLOAT)m_dwShadowOffset;

			FillRectVertex(m_aVertex, m_uVertex, xs, ys, xs + cx, ys + cy, ux, uy, vx, vy, m_dwShadowColor);
		}

		FillRectVertex(m_aVertex, m_uVertex, x, y, x + cx, y + cy, ux, uy, vx, vy, m_dwFontColor);
	}
}

void KG3DFontTexture::DrawCharVertical(FLOAT x, FLOAT y, WCHAR wc)
{
	KG3DTextureGlyph* pGlyph = m_pGlyphs[wc];
	if (pGlyph)
	{
		// prepare tl vertex buffer
		FLOAT cx = pGlyph->cx * m_fScalingX;
		FLOAT cy = pGlyph->cy * m_fScalingY;

		x += pGlyph->yOffset * m_fScalingY;
		y += pGlyph->xOffset * m_fScalingX;

#ifdef SCREEN_COORDINATE_AJUST
		x = floor(x + 0.5f) - 0.5f;
		y = floor(y + 0.5f) - 0.5f;
#endif

		FLOAT ux = pGlyph->vFontU.x;
		FLOAT uy = pGlyph->vFontU.y;
		FLOAT vx = pGlyph->vFontV.x;
		FLOAT vy = pGlyph->vFontV.y;

		if (m_dwEffect & KG3DUI_TEXT_STYLE_BORDER)
		{
			if (m_nTextureID != pGlyph->nBoderTextureID)
			{
				if (m_uVertex)
				{
					ASSERT(m_nTextureID != -1);

					m_pRenderer->PrepareVertex(m_nTextureID, m_aVertex, m_uVertex);
					m_uVertex = 0;
				}
				m_nTextureID = pGlyph->nBoderTextureID;
			}

			FLOAT ux = pGlyph->vBorderU.x;
			FLOAT uy = pGlyph->vBorderU.y;
			FLOAT vx = pGlyph->vBorderV.x;
			FLOAT vy = pGlyph->vBorderV.y;

			FillRectVertexVertical(m_aVertex, m_uVertex, x - 1.0f, y - 1.0f, x + cy + 1.0f, y + cx + 1.0f, ux, uy, vx, vy, m_dwBorderColor);
		}

		if (pGlyph->nTextureID != m_nTextureID)
		{
			if (m_uVertex)
			{
				ASSERT(m_nTextureID != -1);

				m_pRenderer->PrepareVertex(m_nTextureID, m_aVertex, m_uVertex);
				m_uVertex = 0;
			}

			m_nTextureID = pGlyph->nTextureID;
		}

		if (m_dwEffect & KG3DUI_TEXT_STYLE_SHADOW)
		{
			FLOAT xs = x + (FLOAT)m_dwShadowOffset;
			FLOAT ys = y + (FLOAT)m_dwShadowOffset;

			FillRectVertexVertical(m_aVertex, m_uVertex, xs, ys, xs + cy, ys + cx, ux, uy, vx, vy, m_dwShadowColor);
		}

		FillRectVertexVertical(m_aVertex, m_uVertex, x, y, x + cy, y + cx, ux, uy, vx, vy, m_dwFontColor);
	}
}

bool KG3DFontTexture::IsLoaded(UINT charCode, BOOL bBorder)
{
	if (charCode < L' ')
		return true;

	KG3DTextureGlyph* pGlyph = m_pGlyphs[charCode];
	if (pGlyph)
	{
		if (bBorder && pGlyph->nBoderTextureID == KG3DFontRenderer::INVALID_TEXTURE_INDEX)
			InitGlyphBorder(charCode, m_pFontFace->glyph, pGlyph);
		return true;
	}
	return false;
}


UINT KG3DFontTexture::GetTextureIndex()
{
	BOOL bRetCode = FALSE;

	UINT uCharWidth = (UINT)(m_fontWidth + 0.5f);
	UINT uCharHeight = (UINT)(m_fontHeight + 0.5f);

	UINT uIndex = (UINT)KG3DFontRenderer::INVALID_TEXTURE_INDEX;

	if (m_aTextureIndex.empty())
	{
		m_uTextureWidth = KG3DFONT_GLYPH_NUMS * uCharWidth;
		m_uTextureHeight = KG3DFONT_GLYPH_NUMS;

        bRetCode = m_pRenderer->BuildTexture(uIndex, m_uTextureWidth, m_uTextureHeight, uCharWidth, uCharHeight, m_uMipmapLevel);
		KG_PROCESS_ERROR(bRetCode);

		m_uEmptyTextureX = m_uEmptyTextureY = KG3DFONT_DEFAULT_PADDING;

		m_aTextureIndex.push_back(uIndex);
	}
	else if (m_uEmptyTextureX >= (INT)m_uTextureWidth - uCharWidth)
	{
		m_uEmptyTextureX = KG3DFONT_DEFAULT_PADDING;
		m_uEmptyTextureY += uCharHeight + KG3DFONT_DEFAULT_PADDING;

		if (m_uEmptyTextureY >= (INT)m_uTextureHeight - uCharHeight)
		{
			bRetCode = m_pRenderer->BuildTexture(uIndex, m_uTextureWidth, m_uTextureHeight, uCharWidth, uCharHeight, m_uMipmapLevel);
			KGLOG_PROCESS_ERROR(bRetCode);

			m_uEmptyTextureY = KG3DFONT_DEFAULT_PADDING;

			m_aTextureIndex.push_back(uIndex);
		}
		else
		{
			uIndex = *m_aTextureIndex.rbegin();
		}
	}
	else
	{
		uIndex = *m_aTextureIndex.rbegin();
	}

	return uIndex;
Exit0:
	return (UINT)KG3DFontRenderer::INVALID_TEXTURE_INDEX;
}

void KG3DFontTexture::SetTextAlpha( BYTE alpha )
{
	DWORD dwMask = ( ( ( alpha ) & 0xFF) << 24 );

	m_dwFontColor &= 0x00FFFFFF;
	m_dwFontColor |= dwMask;
	m_dwBorderColor &= 0x00FFFFFF;
	m_dwBorderColor |= dwMask;
	m_dwShadowColor &= 0x00FFFFFF;
	m_dwShadowColor |= dwMask;
}

void KG3DFontTexture::SetTextColor(DWORD dwColor)
{
	m_dwFontColor &= 0xFF000000;
	m_dwFontColor |= (dwColor & 0x00FFFFFF);
}

void KG3DFontTexture::SetTextScaling(FLOAT fScalingX, FLOAT fScalingY)
{
	m_fScalingX = fScalingX;
	m_fScalingY = fScalingY;
}

void KG3DFontTexture::SetTextEffect(DWORD dwEffect)
{
	m_dwEffect = (dwEffect & (~KG3DUI_TEXT_STYLE_VERTICAL));
}

void KG3DFontTexture::SetTextBorder(DWORD dwColor, DWORD dwWeight)
{
	// 限制边框大小
	_ASSERT(dwWeight <= 2);

	m_dwBorderColor &= 0xFF000000;
	m_dwBorderColor |= (dwColor & 0x00FFFFFF);
	m_dwBorderWeight = dwWeight;
}

void KG3DFontTexture::SetTextShadow(DWORD dwColor, DWORD dwOffset)
{
	m_dwShadowColor &= 0xFF000000;
	m_dwShadowColor	|= (dwColor & 0x00FFFFFF);
	m_dwShadowOffset = dwOffset;
}

FLOAT KG3DFontTexture::GetCharWidth() const
{
	return m_fontWidth;
}

FLOAT KG3DFontTexture::GetCharHeight() const
{
	return m_fontHeight;
}

