#include "stdafx.h"
#include "./fontrenderer.h"
#include "./FontAdjust.h"

extern LPDIRECT3DDEVICE9 g_pd3dDevice;

namespace
{
	inline int kg3dNextPowerOf2(int in)
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
		*pOut = D3DXVECTOR4(0, 0, 0.0f, 0.0f);
	}
}



////////////////////////////////////////////////
// constructor / destructor
FontRenderer::FontRenderer()
	: m_TextureLast(0)
{
}


FontRenderer::~FontRenderer()
{
	CleanUp();
}



////////////////////////////////////////////////
// initialize / cleanup
bool  FontRenderer::Initialize()
{
	if (g_pd3dDevice == NULL)
		return false;

	// check d3d format
	checkD3DTextureCaps(g_pd3dDevice);
	checkD3DTextureFormat(g_pd3dDevice);


	// init member
	m_nNumIndexs	= 0;
	m_nNumTLIndexs  = 0;
	m_nNumVertexs	= 0;
	m_nNumTLVertexs	= 0;

	return true;
}

bool  FontRenderer::CleanUp()
{
	DestroyTexture();

	return true;
}


////////////////////////////////////////////////
// prepare / flush
bool  FontRenderer::Prepare()
{
	m_TextureLast	= 0;

	m_nNumIndexs	= 0;
	m_nNumTLIndexs  = 0;
	m_nNumVertexs	= 0;
	m_nNumTLVertexs	= 0;
	return true;
}

void  FontRenderer::Flush()
{
	flushVertex();
	flushTLVertex();

	m_TextureLast	= 0;
}



////////////////////////////////////////////////
// texture
void  FontRenderer::DestroyTexture()
{
	for (UINT i = 0; i < m_TextureVector.size(); ++ i)
	{
		this->DeleteTexture(i + 1);
	}


	m_TextureLast = 0;
	m_TextureVector.clear();
}

void  FontRenderer::DeleteTexture(UINT Index)
{
	if ((0 < Index) && (Index <= m_TextureVector.size()))
	{
		LPDIRECT3DTEXTURE9 lpTexture = m_TextureVector.at(Index - 1);

		if (NULL != lpTexture)
		{
		#if 0
			std::string temp("test0");

			temp += 'A' + (Index - 1);
			temp += ".TGA";

			D3DXSaveTextureToFile(temp.c_str(), D3DXIFF_TGA, lpTexture, NULL);
		#endif
			lpTexture->Release();
			m_TextureVector[ Index - 1 ] = NULL;
		}

	}

}


bool  FontRenderer::BuildTexture(UINT& Index, UINT& Width, UINT& Height, UINT glyphW, UINT glyphH)
{
	// get texture width / height
	INT  Padding =  KG3DFONT_DEFAULT_PADDING * 2;

	Width = kg3dNextPowerOf2(Width  + Padding);
	Width = Width > m_TextureWidth ? m_TextureWidth : Width;

	Padding = (Width - Padding) / glyphW;
	Height  = kg3dNextPowerOf2(((Height / Padding) + 1) * glyphH);
	Height  = Height > m_TextureHeight ? m_TextureHeight : Height;


	// make texture
	LPDIRECT3DTEXTURE9 lpTexture = NULL;
	HRESULT hr = D3D_OK;


	hr = D3DXCreateTexture(g_pd3dDevice, Width,Height, 1, D3DUSAGE_DYNAMIC, m_TextureFmt, D3DPOOL_DEFAULT, &lpTexture);
	if (D3D_OK == hr)
	{
		// clear
		D3DXFillTexture (lpTexture, kg3dColorFill, NULL);

		// push back
		static const LPDIRECT3DTEXTURE9 NULL_TEXTURE = NULL;
		typedef std::vector< LPDIRECT3DTEXTURE9 > TextureVectorT;
		typedef TextureVectorT::iterator TextureIterT;

		TextureIterT iter = std::find(m_TextureVector.begin(), m_TextureVector.end(), NULL_TEXTURE);
		if (iter != m_TextureVector.end())
		{
			*iter = lpTexture;
			Index = (iter - m_TextureVector.begin()) + 1;
		}
		else
		{
			m_TextureVector.push_back(lpTexture);
			Index = static_cast<UINT>(m_TextureVector.size());
		}

		return true;
	}


	Index = 0;
	return false;
}

bool  FontRenderer::UploadTexture(UINT Index, UINT offX, UINT offY, UINT sizeW, UINT sizeH, BYTE const* pUpload)
{
	LPDIRECT3DTEXTURE9 lpTexture = m_TextureVector.at(Index - 1);
	HRESULT hr = D3D_OK;

	const UCHAR* pChange = g_FontAjust.GetUch();

	if (NULL != lpTexture)
	{
		D3DLOCKED_RECT rectLock;
		RECT	rectUpload = { offX, offY, offX + sizeW, offY + sizeH };

		hr = lpTexture->LockRect(0, &rectLock, &rectUpload, D3DLOCK_NOSYSLOCK);
		if (D3D_OK == hr)
		{
			UCHAR* pPixBit = static_cast< UCHAR* >(rectLock.pBits);

			for (register UINT y = 0; y < sizeH; ++ y)
			{
				UCHAR* pPixWrite = reinterpret_cast< UCHAR* >(pPixBit + y * rectLock.Pitch);
				for (register UINT x = 0; x < sizeW; ++ x)
				{
					for (register UINT i = 0; i < m_TextureFmtSkip; ++ i)
					{
						* pPixWrite ++ = 0;
					}

					*pPixWrite ++ = pChange[*pUpload ++];

//					*pPixWrite ++ = *pUpload ++;
				}

			}


			lpTexture->UnlockRect(0);
		}
	
	}
	

	return true;
}

bool  FontRenderer::UploadMonoTexture(UINT Index, UINT offX, UINT offY, UINT sizeW, UINT sizeH, UINT pitchSrc, BYTE const* pUpload)
{
	LPDIRECT3DTEXTURE9 lpTexture = m_TextureVector.at(Index - 1);
	HRESULT hr = D3D_OK;

	if (NULL != lpTexture)
	{
		D3DLOCKED_RECT rectLock;
		RECT	rectUpload = { offX, offY, offX + sizeW, offY + sizeH };

		hr = lpTexture->LockRect(0, &rectLock, &rectUpload, D3DLOCK_NOSYSLOCK);
		if (D3D_OK == hr)
		{
			UCHAR* pPixBit = static_cast< UCHAR* >(rectLock.pBits);
			INT    nPixOffset = 0, nPixRShift = 0;

			for (register UINT y = 0; y < sizeH; ++ y)
			{
				BYTE* pPixWrite = reinterpret_cast< BYTE* >(pPixBit + y * rectLock.Pitch);
				BYTE const* pPixReader = reinterpret_cast< BYTE const* >(pUpload + y * pitchSrc);

				for (register UINT x = 0; x < sizeW; x += nPixOffset, ++ pPixReader)
				{
					//nPixOffset = MAX(sizeW - x, 8);//MIN(sizeW - x, 8);
					nPixOffset = min(sizeW - x, 8);
					nPixRShift = 7;

					for (register int k = nPixOffset - 1; k >= 0; -- k, -- nPixRShift)
					{
						for (UINT i = 0; i < m_TextureFmtSkip; ++ i)
						{
							* pPixWrite ++ = 0;
						}
						*pPixWrite ++ = (((*pPixReader) >> nPixRShift) & 1) * 255;
					}
				}
			}

			lpTexture->UnlockRect(0);
		}
	}


	return true;
}

void FontRenderer::PrepareVeretex(UINT TexIndex, FontVertex* pVertex, USHORT nums)
{
	_ASSERT(TexIndex > 0);
	_ASSERT(nums > 0);
	_ASSERT(pVertex);

	flushTexture(TexIndex);

	if (m_nNumVertexs + nums >= KG3DFONT_NUMS_VERTEX)
		flushVertex();

	USHORT const primNums = nums / 4;

	memcpy(m_pVertexBuffer + m_nNumVertexs, pVertex, sizeof(FontVertex) * nums);

	for (UINT i = 0; i < primNums; ++i)
	{
		m_pIndexBuffer[m_nNumIndexs++] = m_nNumVertexs + 0;
		m_pIndexBuffer[m_nNumIndexs++] = m_nNumVertexs + 1;
		m_pIndexBuffer[m_nNumIndexs++] = m_nNumVertexs + 2;
		m_pIndexBuffer[m_nNumIndexs++] = m_nNumVertexs + 3;
		m_pIndexBuffer[m_nNumIndexs++] = m_nNumVertexs + 0;
		m_pIndexBuffer[m_nNumIndexs++] = m_nNumVertexs + 2;

		m_nNumVertexs += nums;
	}
}


void FontRenderer::PrepareVeretex(UINT TexIndex, FontTLVertex* pVertex, USHORT nums)
{
	_ASSERT(TexIndex > 0);
	_ASSERT(nums > 0);
	_ASSERT(pVertex);

	flushTexture(TexIndex);

	if ((m_nNumTLVertexs + nums) >= KG3DFONT_NUMS_TLVERTEX)
		flushTLVertex();

	USHORT const primNums = nums / 4;
	USHORT* pIndexBuffer = m_pTLIndexBuffer + m_nNumTLIndexs;

	memcpy(m_pTLVertexBuffer + m_nNumTLVertexs, pVertex, sizeof(FontTLVertex) * nums);

	for (UINT i = 0; i < primNums; ++i)
	{
		*pIndexBuffer++ = m_nNumTLVertexs + 0;
		*pIndexBuffer++ = m_nNumTLVertexs + 1;
		*pIndexBuffer++ = m_nNumTLVertexs + 2;

		*pIndexBuffer++ = m_nNumTLVertexs + 3;
		*pIndexBuffer++ = m_nNumTLVertexs + 0;
		*pIndexBuffer++ = m_nNumTLVertexs + 2;
	}

	m_nNumTLIndexs += primNums * 6;
	m_nNumTLVertexs += nums;
}


void  FontRenderer::checkD3DTextureCaps(LPDIRECT3DDEVICE9 pD3DDevice)
{
	D3DCAPS9 d3dCaps;

	g_pd3dDevice->GetDeviceCaps(&d3dCaps);
	m_TextureWidth = d3dCaps.MaxTextureWidth;
	m_TextureHeight = d3dCaps.MaxTextureHeight;

	if (m_TextureWidth > 256)   m_TextureWidth = 256;
	if (m_TextureHeight > 256)  m_TextureHeight = 256;

	if (d3dCaps.MaxTextureAspectRatio < (m_TextureWidth / m_TextureHeight))
	{
		m_TextureHeight = kg3dNextPowerOf2(m_TextureWidth / d3dCaps.MaxTextureAspectRatio);
	}
}

void  FontRenderer::checkD3DTextureFormat(LPDIRECT3DDEVICE9 pD3DDevice)
{
	HRESULT hr = D3D_OK;

	D3DDISPLAYMODE AdapterFormat;
	LPDIRECT3D9 pD3D9 = NULL;

	hr = pD3DDevice->GetDirect3D(&pD3D9);
	hr = pD3DDevice->GetDisplayMode(0, &AdapterFormat);

	D3DFORMAT  d3dFmt[] = { D3DFMT_A8, D3DFMT_A8L8, D3DFMT_A4R4G4B4, D3DFMT_A8R8G8B8 };
	UINT       d3dFmtSkip[] = { 0, 1, 1, 3 };

	m_TextureFmt = D3DFMT_A8R8G8B8;
	m_TextureFmtSkip = 3;

	for (int i = 0; i < _countof(d3dFmt); ++i)
	{
		hr = pD3D9->CheckDeviceFormat( D3DADAPTER_DEFAULT, 
			D3DDEVTYPE_HAL, 
			AdapterFormat.Format,
			0, D3DRTYPE_TEXTURE, d3dFmt[i]);

		if (D3D_OK == hr)
		{
			m_TextureFmt = d3dFmt[i];
			m_TextureFmtSkip = d3dFmtSkip[i];

			break ;
		}
	}
}


void  FontRenderer::flushTexture(UINT TexIndex)
{
	if (TexIndex != m_TextureLast)
	{
		flushVertex();
		flushTLVertex();

		m_TextureLast = TexIndex;
		g_pd3dDevice->SetTexture(0, m_TextureVector.at(TexIndex - 1));
	}
}


void  FontRenderer::flushVertex()
{
	if (0 != m_nNumIndexs)
	{
		g_pd3dDevice->SetFVF(KG3DFONT_FVF_VERTEX);
		g_pd3dDevice->DrawIndexedPrimitiveUP(
			D3DPT_TRIANGLELIST,
			0,
			m_nNumVertexs,
			m_nNumVertexs >> 1,
			m_pIndexBuffer,
			D3DFMT_INDEX16,
			m_pVertexBuffer,
			sizeof(FontVertex));

		m_nNumIndexs  = 0;
		m_nNumVertexs = 0;
	}
}

void  FontRenderer::flushTLVertex()
{
	if (0 != m_nNumTLIndexs)
	{
		g_pd3dDevice->SetFVF(KG3DFONT_FVF_TLVERTEX);

		g_pd3dDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, m_nNumTLVertexs, m_nNumTLVertexs >> 1, 
			m_pTLIndexBuffer, D3DFMT_INDEX16, m_pTLVertexBuffer, sizeof(FontTLVertex));

		m_nNumTLIndexs  = 0;
		m_nNumTLVertexs = 0;
	}
}


