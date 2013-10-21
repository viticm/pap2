////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DFontRenderer.cpp
//  Version     : 1.0
//  Creator     : dengchao
//  Create Date : 2006-02-13 16:43:32
//  Comment     : font renderer
//
//				  注意下面会有两到三处地方代码是重复的只是调用与参数的不同。
//				  因为现在字体的绘制并没有完全需求化，所以每种类型的都存在有
//				  特例函数，依备扩展。请不要将此作为错误提交，谢谢。
////////////////////////////////////////////////////////////////////////////////


#include  "stdafx.h"

#include  <ft2build.h>
#include  FT_FREETYPE_H
#include  "KG3DFontTexture.h"
#include  "KG3DFontRenderer.h"

//#define KG3D_FONT_DUMP_TGA

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace
{
	inline int kg3dNextPowerOf2( int in )
	{
		in -= 1;

		in |= in >> 16;
		in |= in >> 8;
		in |= in >> 4;
		in |= in >> 2;
		in |= in >> 1;

		return in + 1;
	}

	VOID WINAPI kg3dColorFill(D3DXVECTOR4* pOut, const D3DXVECTOR2* pTexCoord, 
		const D3DXVECTOR2* pTexelSize, LPVOID pData)
	{
		_ASSERTE(pOut);
		*pOut = D3DXVECTOR4(0, 0, 0.0f, 0.0f);
	}
}


KG3DFontRenderer::KG3DFontRenderer()
: m_pD3DDevice(NULL)
, m_pFontIndexBuffer(NULL)
, m_uFlushTextureIndex(0)
{
}


KG3DFontRenderer::~KG3DFontRenderer()
{
    ASSERT(m_pFontIndexBuffer == NULL);
}


HRESULT KG3DFontRenderer::Initialize(LPDIRECT3DDEVICE9 pD3DDevice)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    BOOL bCreateFontIndexBuffer = FALSE;
    BOOL bCreateFontVertexbuffer = FALSE;

	KGLOG_PROCESS_ERROR(pD3DDevice);

	m_pD3DDevice = pD3DDevice;
    m_pD3DDevice->AddRef();

	hr = CheckD3DTextureCaps(pD3DDevice);
    KGLOG_COM_PROCESS_ERROR(hr);

	hr = CheckD3DTextureFormat(pD3DDevice);
    KGLOG_COM_PROCESS_ERROR(hr);

	m_pRenderBatch = NULL;

	memset(m_RenderBatch, 0, sizeof(m_RenderBatch));

	m_pRenderBatch = &m_RenderBatch[0];

    hr = CreateFontIndexBuffer();
    KGLOG_COM_PROCESS_ERROR(hr);

    bCreateFontIndexBuffer = TRUE;

    hr = CreateFontVertexBuffer();
	KGLOG_COM_PROCESS_ERROR(hr);

    bCreateFontVertexbuffer = TRUE;

    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        if (bCreateFontVertexbuffer)
        {
            hr = DestroyFontVertexBuffer();
            KGLOG_COM_CHECK_ERROR(hr);
        }
        if (bCreateFontIndexBuffer)
        {
            hr = DestroyFontIndexBuffer();
            KGLOG_COM_CHECK_ERROR(hr);
        }

        SAFE_RELEASE(m_pD3DDevice);
    }
	return hrResult;
}

HRESULT KG3DFontRenderer::CleanUp()
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

	for (UINT uIndex = 0; uIndex < (UINT)m_aTexture.size(); ++uIndex)
		DeleteTexture(uIndex);
	m_aTexture.clear();

	m_uFlushTextureIndex = 0;

    hr = DestroyFontVertexBuffer();
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = DestroyFontIndexBuffer();
    KGLOG_COM_PROCESS_ERROR(hr);

    SAFE_RELEASE(m_pD3DDevice);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KG3DFontRenderer::OnLostDevice()
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    hr = DestroyFontVertexBuffer();
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = DestroyFontIndexBuffer();
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KG3DFontRenderer::OnResetDevice()
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    BOOL bCreateFontIndexBuffer = FALSE;
    BOOL bCreateFontVertexbuffer = FALSE;

    hr = CreateFontIndexBuffer();
    KGLOG_COM_PROCESS_ERROR(hr);

    bCreateFontIndexBuffer = TRUE;

    hr = CreateFontVertexBuffer();
	KGLOG_COM_PROCESS_ERROR(hr);

    bCreateFontVertexbuffer = TRUE;

    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        if (bCreateFontVertexbuffer)
        {
            hr = DestroyFontVertexBuffer();
            KGLOG_COM_CHECK_ERROR(hr);
        }
        if (bCreateFontIndexBuffer)
        {
            hr = DestroyFontIndexBuffer();
            KGLOG_COM_CHECK_ERROR(hr);
        }
    }
    return hrResult;
}

HRESULT KG3DFontRenderer::CreateFontIndexBuffer()
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    USHORT* pIB = NULL;
    BOOL bLockFontIndexBuffer = FALSE;

	hr = m_pD3DDevice->CreateIndexBuffer(sizeof(USHORT) * MAX_FONT_VERTEX_INDEX, 
		D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_pFontIndexBuffer, NULL);
	KGLOG_COM_PROCESS_ERROR(hr);

    hr = m_pFontIndexBuffer->Lock(0, sizeof(USHORT) * MAX_FONT_VERTEX_INDEX, (void**)&pIB, 0);
    KGLOG_COM_PROCESS_ERROR(hr);

    bLockFontIndexBuffer = TRUE;

    for (USHORT nIndex = 0; nIndex < MAX_FONT; ++nIndex)
    {
        *pIB++ = nIndex * 4 + 2;
        *pIB++ = nIndex * 4 + 1;
        *pIB++ = nIndex * 4 + 0;
        *pIB++ = nIndex * 4 + 0;
        *pIB++ = nIndex * 4 + 3;
        *pIB++ = nIndex * 4 + 2;
    }

    hr = m_pFontIndexBuffer->Unlock();
    KGLOG_COM_PROCESS_ERROR(hr);

    bLockFontIndexBuffer = FALSE;

    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        if (bLockFontIndexBuffer)
        {
            hr = m_pFontIndexBuffer->Unlock();
        }

        SAFE_RELEASE(m_pFontIndexBuffer);
    }
    return hrResult;
}

HRESULT KG3DFontRenderer::DestroyFontIndexBuffer()
{
	SAFE_RELEASE(m_pFontIndexBuffer);
    return S_OK;
}

HRESULT KG3DFontRenderer::CreateFontVertexBuffer()
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    for (int nBatch = 0; nBatch < MAX_RENDER_BATCH; ++nBatch)
    {
        hr = m_pD3DDevice->CreateVertexBuffer(sizeof(KG3DFontTLVertex) * MAX_FONT_VERTEX, 
            D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &m_RenderBatch[nBatch].pVB, NULL);
        KGLOG_COM_PROCESS_ERROR(hr);
    }

    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        hr = DestroyFontVertexBuffer();
        KGLOG_COM_CHECK_ERROR(hr);
    }
    return hrResult;
}

HRESULT KG3DFontRenderer::DestroyFontVertexBuffer()
{
    for (int nBatch = 0; nBatch < MAX_RENDER_BATCH; ++nBatch)
    {
        SAFE_RELEASE(m_RenderBatch[nBatch].pVB);
        m_RenderBatch[nBatch].uTLVertex = 0;
    }

    return S_OK;
}

HRESULT KG3DFontRenderer::Prepare()
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

	m_uFlushTextureIndex = (UINT)m_aTexture.size();

	hr = m_pD3DDevice->SetIndices(m_pFontIndexBuffer);
	KGLOG_COM_PROCESS_ERROR(hr);

    hr = m_pD3DDevice->SetFVF(KG3DFontTLVertex::FVF);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KG3DFontRenderer::Flush()
{
	m_uFlushTextureIndex = (UINT)m_aTexture.size();
	for (UINT uIndex = 0; uIndex < (UINT)m_aTexture.size(); ++uIndex)
	{
		FlushTexture(uIndex);
		FlushTLVertex();
	}

    return S_OK;
}

void KG3DFontRenderer::DeleteTexture(UINT uIndex)
{
    if (uIndex >= m_aTexture.size())
        return;
	//ASSERT(uIndex < m_aTexture.size());

	LPDIRECT3DTEXTURE9 lpTexture = m_aTexture[uIndex];
	if (lpTexture)
	{
#ifdef KG3D_FONT_DUMP_TGA
		char szFile[MAX_PATH];

		sprintf(szFile, "../dump/font_%.4d.tga", uIndex);
		D3DXSaveTextureToFile(szFile, D3DXIFF_TGA, lpTexture, NULL);
#endif
        SAFE_RELEASE(m_aTexture[uIndex]);
	}
}


size_t KG3DFontRenderer::GetTexturePixelByte() const
{
    size_t nPixelByte = 0;
    switch (m_TextureFormat)
    {
    case D3DFMT_A8:
        nPixelByte = 1;
        break;
    case D3DFMT_A8L8:
        nPixelByte = 2;
        break;
    case D3DFMT_A4R4G4B4:
        nPixelByte = 2;
        break;
    case D3DFMT_A8R8G8B8:
        nPixelByte = 4;
        break;
    default:
        ASSERT(0);
    }
    return nPixelByte;
}


BOOL KG3DFontRenderer::BuildTexture(UINT& uIndex, UINT& uWidth, UINT& uHeight, UINT uGlyphWidth, UINT uGlyphHeight, UINT uMipmapLevel)
{
	LPDIRECT3DTEXTURE9 lpTexture = NULL;
	HRESULT hr = D3D_OK;
    D3DSURFACE_DESC desc;
    size_t nPixelByte = 0;
	KTextureArray::iterator it;

	// get texture width / height

	uWidth = 512;
	uHeight = 512;

	hr = D3DXCreateTexture(m_pD3DDevice, uWidth, uHeight, uMipmapLevel, 0, m_TextureFormat, D3DPOOL_MANAGED, &lpTexture);
	KGLOG_COM_PROCESS_ERROR(hr);
	
	KGLOG_PROCESS_ERROR(lpTexture);

    nPixelByte = GetTexturePixelByte();

	for (UINT uLevel = 0; uLevel < uMipmapLevel; ++uLevel)
	{
		if (SUCCEEDED(lpTexture->GetLevelDesc(uLevel, &desc)))
		{
			D3DLOCKED_RECT rect;

			if (SUCCEEDED(lpTexture->LockRect(uLevel, &rect, NULL, D3DLOCK_NOSYSLOCK)))
			{
				for (UINT uHeight = 0; uHeight < desc.Height; ++uHeight)
					memset((PBYTE)rect.pBits + rect.Pitch * uHeight, 0, nPixelByte * desc.Width);

				lpTexture->UnlockRect(uLevel);
			}
		}
	}

	it = std::find(m_aTexture.begin(), m_aTexture.end(), (LPDIRECT3DTEXTURE9)NULL);
	if (it != m_aTexture.end())
	{
		*it = lpTexture;
		uIndex = (UINT)(std::distance(m_aTexture.begin(), it));
	}
	else
	{
		m_aTexture.push_back(lpTexture);
		uIndex = (UINT)m_aTexture.size() - 1;
	}

	return TRUE;
Exit0:
	uIndex = (UINT)INVALID_TEXTURE_INDEX;
	return FALSE;
}

BOOL KG3DFontRenderer::UploadTexture(UINT uIndex, UINT uLevel, UINT uX, UINT uY, UINT uWidth, UINT uHeight, BYTE const* pUpload, UCHAR* puchAdjust)
{
	HRESULT hr = E_FAIL;

	ASSERT(puchAdjust);

	//todo:临时改为这个样子，防止设备丢失后的宕机问题。
	if (uIndex < (UINT)m_aTexture.size())
	{
		LPDIRECT3DTEXTURE9 lpTexture = m_aTexture[uIndex];
		if (lpTexture)
		{
			D3DLOCKED_RECT rectLock;
			RECT rectUpload = { uX, uY, uX + uWidth, uY + uHeight };

			hr = lpTexture->LockRect(uLevel, &rectLock, &rectUpload, D3DLOCK_NOSYSLOCK);
			if (hr == D3D_OK)
			{
				UCHAR* pPixBit = (UCHAR*)(rectLock.pBits);

				for (UINT y = 0; y < uHeight; ++y)
				{
					UCHAR* pPixWrite = (UCHAR*)(pPixBit + y * rectLock.Pitch);
					for (UINT x = 0; x < uWidth; ++x)
					{
						for (UINT i = 0; i < m_TextureFmtSkip; ++i)
							*pPixWrite++ = 0;
						*pPixWrite++ = puchAdjust[*pUpload++];
					}
				}

				lpTexture->UnlockRect(uLevel);
			}
		}
	}

	return TRUE;
}


BOOL KG3DFontRenderer::UploadMonoTexture(UINT uIndex, UINT uLevel, UINT uX, UINT uY, UINT uWidth, UINT uHeight, UINT uUploadPitch, BYTE const* pUpload)
{
	HRESULT hr = D3D_OK;

	LPDIRECT3DTEXTURE9 lpTexture = m_aTexture[uIndex];
	if (lpTexture && uWidth > 0 && uHeight > 0)
	{
		D3DLOCKED_RECT rectLock;
		RECT rectUpload = { uX, uY, uX + uWidth, uY + uHeight };

		hr = lpTexture->LockRect(uLevel, &rectLock, &rectUpload, D3DLOCK_NOSYSLOCK);
		if (hr == D3D_OK)
		{
			UCHAR* pPixBit = (UCHAR*)(rectLock.pBits);
			INT nPixOffset = 0;
			INT nPixRShift = 0;

			for (UINT y = 0; y < uHeight; ++y)
			{
				BYTE* pPixWrite = (UCHAR*)(pPixBit + y * rectLock.Pitch);
				BYTE const* pPixReader = (UCHAR*)(pUpload + y * uUploadPitch);

				for (UINT x = 0; x < uWidth; x += nPixOffset, ++pPixReader)
				{
					nPixOffset = min(uWidth - x, 8u);
					nPixRShift = 7;

					for (int k = nPixOffset - 1; k >= 0; --k, --nPixRShift)
					{
						for (UINT i = 0; i < m_TextureFmtSkip; ++i)
							*pPixWrite++ = 0;
						*pPixWrite++ = (UCHAR)(((*pPixReader >> nPixRShift) & 1) * 255);
					}
				}
			}

			lpTexture->UnlockRect(uLevel);
		}
	}

	return TRUE;
}

BOOL KG3DFontRenderer::UploadBoderTexture(UINT uIndex, UINT uLevel, UINT uX, UINT uY, UINT uWidth, UINT uHeight, BYTE const* pUpload, UCHAR *puchAdjust)
{
	HRESULT hr = E_FAIL;

	ASSERT(puchAdjust);

	//todo:临时改为这个样子，防止设备丢失后的宕机问题。
	if (uIndex < (UINT)m_aTexture.size())
	{
		LPDIRECT3DTEXTURE9 lpTexture = m_aTexture[uIndex];
		if (lpTexture)
		{
			D3DLOCKED_RECT rectLock;
			RECT rectUpload = { uX - 1, uY - 1, uX + uWidth + 1, uY + uHeight + 1};

			hr = lpTexture->LockRect(uLevel, &rectLock, &rectUpload, D3DLOCK_NOSYSLOCK);
			if (hr == D3D_OK)
			{
				UCHAR* pPixBit = (UCHAR*)(rectLock.pBits);
				const BYTE* pUploadC = pUpload;
				UINT uLen = m_TextureFmtSkip + 1;
				for (UINT y = 0; y < uHeight; ++y)
				{
					UCHAR* pWrite = pPixBit + y * rectLock.Pitch + m_TextureFmtSkip;
					for (UINT x = 0; x < uWidth; ++x)
					{
						if (*pUpload++)
						{
							pWrite += rectLock.Pitch; //2(1, 0)
							*pWrite = 255;
							pWrite += rectLock.Pitch; //7(2, 0)
							if (*pWrite < 128)
								*pWrite = 128;
							pWrite += uLen; //1(2, 1)
							*pWrite = 255;
							pWrite += uLen; //6(2, 2)
							if (*pWrite < 128)
								*pWrite = 128;
							pWrite -= rectLock.Pitch; //3(1, 2)
							*pWrite = 255;
							pWrite -= rectLock.Pitch; //5(0, 2)
							if (*pWrite < 128)
								*pWrite = 128;
							pWrite -= uLen; //0(0, 1)
							*pWrite = 255;
							pWrite -= uLen; //4(0, 0)
							if (*pWrite < 128) 
								*pWrite = 128;
						}
						pWrite += uLen;
					}
				}

				for (UINT y = 0; y < uHeight; ++y)
				{
					UCHAR* pWrite = pPixBit + (y + 1) * rectLock.Pitch + m_TextureFmtSkip;
					for (UINT x = 0; x < uWidth; ++x)
					{
						pWrite += uLen;
						UCHAR uMe = puchAdjust[*pUploadC++];
						if (uMe)
							*pWrite = 255 - uMe;
					}
				}
				lpTexture->UnlockRect(uLevel);
			}
		}
	}

	return TRUE;
}

BOOL KG3DFontRenderer::UploadMonoBoderTexture(UINT uIndex, UINT uLevel, UINT uX, UINT uY, UINT uWidth, UINT uHeight, UINT uUploadPitch, BYTE const* pUpload)
{
	HRESULT hr = D3D_OK;
	LPDIRECT3DTEXTURE9 lpTexture = m_aTexture[uIndex];
	if (lpTexture)
	{
		D3DLOCKED_RECT rectLock;
		RECT rectUpload = { uX - 1, uY - 1, uX + uWidth + 1, uY + uHeight + 1};

		hr = lpTexture->LockRect(uLevel, &rectLock, &rectUpload, D3DLOCK_NOSYSLOCK);
		if (hr == D3D_OK)
		{
			UCHAR* pPixBit = (UCHAR*)(rectLock.pBits);
//			const BYTE* pUploadC = pUpload;
			UINT uLen = m_TextureFmtSkip + 1;
			for (UINT y = 0; y < uHeight; ++y)
			{
				UCHAR* pWrite = pPixBit + y * rectLock.Pitch + m_TextureFmtSkip;
				for (UINT x = 0; x < uWidth; ++x)
				{
					if ((*(pUpload + y * uUploadPitch + x / 8)) & (1 << (7 - x % 8)))
					{
						pWrite += rectLock.Pitch; //2(1, 0)
						*pWrite = 255;
						pWrite += rectLock.Pitch; //7(2, 0)
						if (*pWrite < 128)
							*pWrite = 128;
						pWrite += uLen; //1(2, 1)
						*pWrite = 255;
						pWrite += uLen; //6(2, 2)
						if (*pWrite < 128)
							*pWrite = 128;
						pWrite -= rectLock.Pitch; //3(1, 2)
						*pWrite = 255;
						pWrite -= rectLock.Pitch; //5(0, 2)
						if (*pWrite < 128)
							*pWrite = 128;
						pWrite -= uLen; //0(0, 1)
						*pWrite = 255;
						pWrite -= uLen; //4(0, 0)
						if (*pWrite < 128) 
							*pWrite = 128;
					}
					pWrite += uLen;
				}
			}
			for (UINT y = 0; y < uHeight; ++y)
			{
				UCHAR* pWrite = pPixBit + (y + 1) * rectLock.Pitch + m_TextureFmtSkip;
				for (UINT x = 0; x < uWidth; ++x)
				{
					pWrite += uLen;
					if ((*(pUpload + y * uUploadPitch + x / 8)) & (1 << (7 - x % 8)))
						*pWrite = 0;
				}
			}

			lpTexture->UnlockRect(uLevel);
		}
	}

	return TRUE;
}

HRESULT KG3DFontRenderer::PrepareVertex(UINT uIndex, KG3DFontTLVertex* pVertex, UINT uVertex)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    KG3DFontTLVertex* pVB = NULL;
    UINT uRenderVertex = 0;

	KGLOG_PROCESS_ERROR(pVertex);
	
    hr = FlushTexture(uIndex);
    KGLOG_COM_PROCESS_ERROR(hr);

    KG_ASSERT_EXIT(m_pRenderBatch);

    while (uVertex > 0)
    {
        uRenderVertex = uVertex > MAX_FONT_VERTEX ? MAX_FONT_VERTEX : uVertex;
        uVertex -= uRenderVertex;

        if (m_pRenderBatch->uTLVertex + uRenderVertex >= MAX_FONT_VERTEX)
        {
            hr = FlushTLVertex();
            KGLOG_COM_PROCESS_ERROR(hr);
        }

		if(m_pRenderBatch && m_pRenderBatch->pVB)
		{
            hr = m_pD3DDevice->TestCooperativeLevel();
            KG_COM_PROCESS_ERROR(hr);

			hr = m_pRenderBatch->pVB->Lock(sizeof(KG3DFontTLVertex) * m_pRenderBatch->uTLVertex, sizeof(KG3DFontTLVertex) * uRenderVertex, (void**)&pVB, 
				m_pRenderBatch->uTLVertex > 0 ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);
			if (SUCCEEDED(hr))
			{
				memcpy(pVB, pVertex, sizeof(KG3DFontTLVertex) * uRenderVertex);

				hr = m_pRenderBatch->pVB->Unlock();
				KGLOG_COM_CHECK_ERROR(hr);
			}
		}

        pVertex += uRenderVertex;
        m_pRenderBatch->uTLVertex += uRenderVertex;

        KG_ASSERT_EXIT(m_pRenderBatch->uTLVertex <= MAX_FONT_VERTEX);
    }

    hrResult = S_OK;
Exit0:
    return hrResult;
}


HRESULT KG3DFontRenderer::CheckD3DTextureCaps(LPDIRECT3DDEVICE9 pD3DDevice)
{
    static UINT const TEXTURE_SIZE = 512;

    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
	D3DCAPS9 d3dCaps;
	DWORD dwTextureWidth = 0;
	DWORD dwTextureHeight = 0;

	hr = m_pD3DDevice->GetDeviceCaps(&d3dCaps);
    KGLOG_COM_PROCESS_ERROR(hr);

	dwTextureWidth = d3dCaps.MaxTextureWidth;
	dwTextureHeight = d3dCaps.MaxTextureHeight;

	// TODO:根据字号调整贴图大小贴图
	if (dwTextureWidth > TEXTURE_SIZE)
		dwTextureWidth = TEXTURE_SIZE;
	if (dwTextureHeight > TEXTURE_SIZE)
		dwTextureHeight = TEXTURE_SIZE;

    KG_ASSERT_EXIT(dwTextureHeight);

    if (d3dCaps.MaxTextureAspectRatio > 0 && d3dCaps.MaxTextureAspectRatio < (dwTextureWidth / dwTextureHeight))
        dwTextureHeight = kg3dNextPowerOf2(dwTextureWidth / d3dCaps.MaxTextureAspectRatio);

	KG_ASSERT_EXIT(d3dCaps.MaxPrimitiveCount >= MAX_FONT_VERTEX);
    KG_ASSERT_EXIT(d3dCaps.MaxVertexIndex >= MAX_FONT_VERTEX_INDEX);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KG3DFontRenderer::CheckD3DTextureFormat(LPDIRECT3DDEVICE9 pD3DDevice)
{
	HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

	// get d3d9 & display mode
	D3DDISPLAYMODE AdapterFormat;
	LPDIRECT3D9 pD3D9 = NULL;

	hr = pD3DDevice->GetDirect3D(&pD3D9);
    KGLOG_COM_PROCESS_ERROR(hr);

	hr = pD3DDevice->GetDisplayMode(0, &AdapterFormat);
    KGLOG_COM_PROCESS_ERROR(hr);

	m_TextureFormat = D3DFMT_A8R8G8B8;
	m_TextureFmtSkip = 3;

#ifndef _DEBUG
	D3DFORMAT  d3dFmt[] = { D3DFMT_A8, D3DFMT_A8L8, D3DFMT_A4R4G4B4, D3DFMT_A8R8G8B8 };
	UINT       d3dFmtSkip[] = { 0, 1, 1, 3 };

	for (int i = 0; i < _countof(d3dFmt); ++i)
	{
		hr = pD3D9->CheckDeviceFormat(D3DADAPTER_DEFAULT, 
			D3DDEVTYPE_HAL, 
			AdapterFormat.Format,
			0, D3DRTYPE_TEXTURE, d3dFmt[i]);
		if (D3D_OK == hr)
		{
			m_TextureFormat = d3dFmt[i];
			m_TextureFmtSkip = d3dFmtSkip[i];

			break ;
		}
	}
#endif

    hrResult = S_OK;
Exit0:
	if (pD3D9)
    {
        pD3D9->Release();
    }
    return hrResult;
}


HRESULT KG3DFontRenderer::FlushTexture(UINT uIndex)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    KG_ASSERT_EXIT(m_pRenderBatch);

	if (uIndex != m_uFlushTextureIndex)
	{
		KG_ASSERT_EXIT(uIndex < (UINT)m_aTexture.size());

		m_uFlushTextureIndex = uIndex;

		m_pRenderBatch = &m_RenderBatch[uIndex % MAX_RENDER_BATCH];

		ASSERT(m_pRenderBatch);

		if (m_pRenderBatch->uTLVertex > 0)
        {
			hr = FlushTLVertex();
            KGLOG_COM_PROCESS_ERROR(hr);
        }

		m_pRenderBatch->pTexture = m_aTexture[uIndex];
	}

    hrResult = S_OK;
Exit0:
	return hrResult;
}

HRESULT KG3DFontRenderer::FlushTLVertex()
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

	KG_ASSERT_EXIT(m_pRenderBatch);

	KG_PROCESS_SUCCESS(m_pRenderBatch->uTLVertex == 0);

    hr = m_pD3DDevice->SetTexture(0, m_pRenderBatch->pTexture);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = m_pD3DDevice->SetStreamSource(0, m_pRenderBatch->pVB, 0, sizeof(KG3DFontTLVertex));
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = m_pD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_pRenderBatch->uTLVertex, 0, m_pRenderBatch->uTLVertex / 2);
    KGLOG_COM_PROCESS_ERROR(hr);

    m_pRenderBatch->uTLVertex = 0;

Exit1:
    hrResult = S_OK;
Exit0:
	return hrResult;
}

