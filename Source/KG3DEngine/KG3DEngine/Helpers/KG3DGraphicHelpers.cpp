////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DGraphicHelpers.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-7-24 14:42:36
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KG3DGraphicHelpers.h"


////////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
namespace KG3D_CUSTOM_HELPERS
{
	HRESULT FillTexture( LPDIRECT3DSURFACE9 pSuface, CONST RECT* pRect, D3DCOLOR color )
	{
		if(NULL == pSuface)
			return E_INVALIDARG;

		D3DSURFACE_DESC SurfaceDesc;
		HRESULT hr = pSuface->GetDesc(&SurfaceDesc);
		KG_COM_PROCESS_ERROR(hr);
		//KG_PROCESS_ERROR(SurfaceDesc.Type == D3DRTYPE_SURFACE);

		if(SurfaceDesc.Usage == D3DUSAGE_RENDERTARGET)//ColorFill只能用在RT上
		{
			LPDIRECT3DDEVICE9 pDevice = NULL;
			hr = pSuface->GetDevice(&pDevice);
			KG_COM_PROCESS_ERROR(hr);

			hr = pDevice->ColorFill(pSuface, pRect, color);
			SAFE_RELEASE(pDevice);
			KG_COM_PROCESS_ERROR(hr);
		}
		else
		{
			//其实PoolDefault的也是可以填颜色的，不过要倒腾，超麻烦
			KG_PROCESS_ERROR(SurfaceDesc.Pool == D3DPOOL_MANAGED || SurfaceDesc.Pool == D3DPOOL_SYSTEMMEM || 
				SurfaceDesc.Pool == D3DPOOL_SCRATCH);
			KG_PROCESS_ERROR(SurfaceDesc.Format == D3DFMT_X8R8G8B8 || SurfaceDesc.Format == D3DFMT_A8R8G8B8);
			
			D3DLOCKED_RECT rect;
			hr = pSuface->LockRect(&rect, pRect, 0);
			KG_COM_PROCESS_ERROR(hr);

			UINT YStart = (NULL == pRect) ? 0 : pRect->top;
			UINT Height = (NULL == pRect) ? SurfaceDesc.Height : UINT(pRect->bottom - pRect->top);

			UINT XStart = (NULL == pRect) ? 0 : pRect->left;
			UINT Width = (NULL == pRect) ? SurfaceDesc.Width : UINT(pRect->right - pRect->left);

			CHAR* pBuffer= reinterpret_cast<CHAR*>(rect.pBits);

			for (YStart = 0; YStart < Height; ++YStart)
			{
				DWORD* pTexBufferLine = (DWORD*)(pBuffer);
				for (XStart = 0; XStart < Width; ++XStart)
				{
					*pTexBufferLine = color;
					++pTexBufferLine;
				}
				pBuffer += rect.Pitch;//DX会对非2Power的贴图进行32位补齐，所以在行上一定要这么干，参考GameDev上的文章
				_ASSERTE((CHAR*)pTexBufferLine <= pBuffer);
			}

			pSuface->UnlockRect();
		}
		return S_OK;
Exit0:
		return E_FAIL;
	}

	HRESULT FillTexture( LPDIRECT3DTEXTURE9 pTex, CONST RECT* pRect, D3DCOLOR color )
	{
		LPDIRECT3DSURFACE9 pSurface = NULL;
		KG_PROCESS_ERROR(NULL != pTex);
		{
			HRESULT hr = pTex->GetSurfaceLevel(0, &pSurface);
			KG_COM_PROCESS_ERROR(hr);

			hr = FillTexture(pSurface, pRect, color);
			KG_COM_PROCESS_ERROR(hr);

			KG3D_CUSTOM_HELPERS::GenerateMipSubLevels(pTex);

			SAFE_RELEASE(pSurface);
			return hr;
		}
Exit0:
		SAFE_RELEASE(pSurface);
		return E_FAIL;
	}

	HRESULT GenerateMipSubLevels( LPDIRECT3DTEXTURE9 pTex )
	{
		HRESULT hr = E_FAIL;
		D3DSURFACE_DESC desc;

		KG_PROCESS_ERROR(NULL != pTex);
		{
			DWORD dwLevelCount = pTex->GetLevelCount();
			KG_PROCESS_SUCCESS(dwLevelCount <= 1);

			hr = pTex->GetLevelDesc(0, &desc);
			KG_COM_PROCESS_ERROR(hr);
			if((desc.Usage & D3DUSAGE_RENDERTARGET) && !(desc.Usage & D3DUSAGE_AUTOGENMIPMAP))
			{
				goto Exit0;	//RT的不指定AUTOGENMIPMAP的话，是没有办法GENMIPMAP的
			}

			pTex->GenerateMipSubLevels();
		}		
Exit1:
		return S_OK;
Exit0:
		return E_FAIL;
	}
};