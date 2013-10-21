////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DTextureHelpers.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2007-6-22 16:38:49
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "KG3DTextureHelpers.h"
#include "KG3DTexture.h"
#include "KGCommonWidgets.h"
#include "MathTool.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace KG3D_CUSTOM_HELPERS
{	
	TextureSampler::TextureSampler( KG3DTexture* pTex
		, DWORD* pdwResultBuffer
		, INT nResultBufferSize
		, INT nSampleTimes
		, INT nTargetWidth
		, INT nTargetHeight
		, BOOL bSampleAlpha)
		: m_bResult(FALSE)
	{
		///检查参数
		_ASSERTE(pTex && pdwResultBuffer);
		_ASSERTE( pTex->GetFormat() == D3DFMT_A8R8G8B8 );
		HRESULT hr = E_FAIL;
		BOOL bRet = FALSE;
		LPDIRECT3DTEXTURE9 pD3DTex = pTex->GetTexture();
		KG_PROCESS_ERROR(pD3DTex);
		{
			unsigned int uTexHeight = 0, uTexWidth = 0;
			hr = pTex->GetSize(&uTexWidth, &uTexHeight);
			KG_COM_PROCESS_ERROR(hr);
			INT nTexHeight = static_cast<INT>(uTexHeight);
			INT nTexWidth = static_cast<INT>(uTexWidth);

			KG_PROCESS_ERROR(nTexHeight > 0 && nTexWidth > 0 
				&&nTexWidth <= TEX_MAX_SIZE && nTexHeight <= TEX_MAX_SIZE 
				&& nSampleTimes >=1 && nSampleTimes <=3);

			D3DLOCKED_RECT	Rect;
			hr = pD3DTex->LockRect(0, &Rect, NULL, 0);
			KG_COM_PROCESS_ERROR(hr);
			{
				DWORD* pPixels = (DWORD*)Rect.pBits;
				{
					bRet = SampleDWORDArrayEx(pPixels
						, nTexWidth
						, nTexHeight
						, pdwResultBuffer
						, nResultBufferSize
						, nSampleTimes
						, nTargetWidth
						, nTargetHeight
						, bSampleAlpha);///
				}

				pD3DTex->UnlockRect(0);
				KG_PROCESS_ERROR(bRet);
			}
		}

		m_bResult = TRUE;
		return;
Exit0:
		_KG3D_DEBUG_SELF_REPORT("TextureSampler");
		m_bResult = FALSE;
		return;
	}
	BOOL TextureSampler::SampleDWORDArrayEx( DWORD* pTexPixels
		, INT nTexWidth
		, INT nTexHeight
		, DWORD* pdwResultBuffer
		, INT nResultBufferSize
		, INT nSampleTimes
		, INT nTargetWidth
		, INT nTargetHeight
		, BOOL bSampleAlpha/*= FALSE*/)
	{
		_ASSERTE(pTexPixels&& pdwResultBuffer);		

		///检查参数
		KG_PROCESS_ERROR(nTexHeight > 0 && nTexWidth > 0 
			&&nTexWidth <= TEX_MAX_SIZE && nTexHeight <= TEX_MAX_SIZE
			&& nSampleTimes >=1 && nSampleTimes <=3);///nSample == 0的时候采用中点采样

		KG_PROCESS_ERROR(nTargetWidth > 0 && nTargetHeight >0
			&& nResultBufferSize >= nTargetWidth * nTargetHeight);

		///设定采样点参数
		{
			ScaleGroup SrcToDesScaleGroup((float)nTexWidth/(float)nTargetWidth, (float)nTexHeight/(float)nTargetHeight);
			ScaleGroup SampleGap(SrcToDesScaleGroup.first/nSampleTimes, SrcToDesScaleGroup.second/nSampleTimes);
			TexSize	   SrcSize(nTexWidth, nTexHeight);
			TexSize    DesSize(nTargetWidth, nTargetHeight);
//			INT		nTotalSamplePoint = nSampleTimes*nSampleTimes;
			///分析贴图
			DWORD* pPixels = pTexPixels;
			INT nResultWrited = 0;
			///下面把贴图分成一个Region一个Region的格子，然后每个格子当二位数组进行采样
			///当nSamleTime = 2的时候实际是采样了4次而已。
			for (int nZ = 0; nZ < nTargetHeight; nZ++)
			{
				/////////////////////////////////////////////////////////////////////////			
				///每个Region的采样
				for (int nX = 0; nX < nTargetWidth; nX++)
				{
					INT nTotalR = 0, nTotalG = 0, nTotalB = 0, nTotalA = 0;

					PixelPos BegionPos = CalculateFirstPixelPosInSampleGroup(PixelPos(nX, nZ), SrcToDesScaleGroup);
					for (int nGroupY = 0; nGroupY < nSampleTimes; nGroupY++)
					{
						for (int nGroupX = 0; nGroupX < nSampleTimes; nGroupX++)
						{
							PixelPos SamplePointPos = CalculateSamplePixelPos(BegionPos, PixelPos(nGroupX, nGroupY), SampleGap);
							D3DCOLOR SamplePointColor = GetColorAtPos(pPixels, SrcSize, SamplePointPos);
							nTotalR += D3DCOLOR_GETRED(SamplePointColor);
							nTotalG += D3DCOLOR_GETGREEN(SamplePointColor);
							nTotalB += D3DCOLOR_GETBLUE(SamplePointColor);
							nTotalA += D3DCOLOR_GETALPHA(SamplePointColor);
						}
					}

					nTotalR/=nSampleTimes;
					nTotalG/=nSampleTimes;
					nTotalB/=nSampleTimes;
					nTotalA/=nSampleTimes;
					if (bSampleAlpha)
					{
						SetColorAtPos(pdwResultBuffer, nResultBufferSize, nResultWrited, D3DCOLOR_ARGB(nTotalA, nTotalR, nTotalG, nTotalB));
					}
					else
					{
						SetColorAtPos(pdwResultBuffer, nResultBufferSize, nResultWrited, D3DCOLOR_XRGB(nTotalR, nTotalG, nTotalB));
					}
					nResultWrited++;
				}///End of Region 采样

			}///for (int nZ = 0; nZ < nTargetHeight; nZ++)

			return TRUE;
		}		
Exit0:
		_KG3D_DEBUG_SELF_REPORT("失败：TextureSampler::ToSmallerSize");
		return FALSE;
	}


	//////////////////////////////////////////////////////////////////////////
	TextureToGrayArray::TextureToGrayArray(DWORD* pdwSource, INT nArraySize,UCHAR* pResult, INT nResultArraySize)
		:m_bResult(FALSE)
	{
		KG_PROCESS_ERROR(pResult && nResultArraySize >= nArraySize);

		UCHAR* pTarget = pResult;
		DWORD* pSourcePointer = pdwSource;
		for (INT i = 0; i < nArraySize; i++, pSourcePointer++, pTarget++/*pTarget+=nGapBetweenWrite*/)
		{
			*pTarget = D3DCOLOR2Gray(*pSourcePointer);						
		}
		m_bResult = TRUE;
		return;
Exit0:
		_KG3D_DEBUG_SELF_REPORT("失败：TextureToGrayArray");
		m_bResult = FALSE;
		return;
	}

	TextureToGrayArray::TextureToGrayArray( DWORD* pdwSource, INT nArraySize, DWORD* pResult, INT nResultArraySize, BOOL bAlphaTo255 /*= TRUE*/ )
	{
		KG_PROCESS_ERROR(pResult && nResultArraySize >= nArraySize);

		DWORD* pTarget = pResult;
		DWORD* pSourcePointer = pdwSource;
		for (INT i = 0; i < nArraySize; i++, pSourcePointer++, pTarget++/*pTarget+=nGapBetweenWrite*/)
		{
			*pTarget = D3DCOLOR2GrayColor(*pSourcePointer, bAlphaTo255);						
		}
		m_bResult = TRUE;
		return;
Exit0:
		_KG3D_DEBUG_SELF_REPORT("失败：TextureToGrayArray");
		m_bResult = FALSE;
		return;
	}
	HRESULT TextureDWORDBufferToTex( KG3DTexture* pTexture, DWORD* pdwArray, INT nInputArraySize )
	{
		HRESULT hr = E_FAIL;
		unsigned uTexWidth = 0, uTexHeight = 0;
		INT nTotalPixels = 0;
		KG_PROCESS_ERROR(pTexture && pdwArray);
		KG_PROCESS_ERROR(pTexture->GetFormat() == D3DFMT_A8R8G8B8); 
		hr = pTexture->GetSize(&uTexWidth, &uTexHeight);
		KG_COM_PROCESS_ERROR(hr);
		nTotalPixels = (INT)(uTexWidth * uTexHeight);
		KG_PROCESS_ERROR(uTexWidth <= 2048 && uTexHeight <= 2048 && nInputArraySize >= nTotalPixels);
		{
			D3DLOCKED_RECT TempRect;
			LPDIRECT3DTEXTURE9 pD3DTex = pTexture->GetTexture();
			KG_PROCESS_ERROR(pD3DTex);
			hr = pD3DTex->LockRect(0, &TempRect, NULL, 0);
			KG_COM_PROCESS_ERROR(hr);
			{
				DWORD* pPixels = (DWORD*)TempRect.pBits;
				memcpy_s(pPixels, sizeof(DWORD)*nTotalPixels, pdwArray, sizeof(DWORD)*nTotalPixels);
				pD3DTex->UnlockRect(0);
			}
			return S_OK;
		}
Exit0:
		_KG3D_DEBUG_SELF_REPORT("失败：TextureDWORDBufferToTex");
		return E_FAIL;
	}

	BOOL TextureGrayBufferToDWORDBuffer( UCHAR* pGray, INT nGrayArraySize, DWORD* pColor, INT nColorArraySize )
	{
		KG_PROCESS_ERROR(nColorArraySize >= nGrayArraySize);
		UCHAR* pSource = pGray;
		DWORD* pRet = pColor;
		for (int i = 0; i < nGrayArraySize; i++, pSource++, pRet++)
		{
			UCHAR nGray = *pSource;
			*pRet = D3DCOLOR_XRGB(nGray, nGray, nGray);
		}
		return TRUE;
Exit0:
		return FALSE;
	}

	HRESULT TextureTexToDWORDBuffer( KG3DTexture* pTexture, DWORD* pdwArray, INT nInputArraySize )
	{
		HRESULT hr = E_FAIL;
		unsigned uTexWidth = 0, uTexHeight = 0;
		INT nTotalPixels = 0;
		KG_PROCESS_ERROR(pTexture && pdwArray);
		KG_PROCESS_ERROR(pTexture->GetFormat() == D3DFMT_A8R8G8B8); 
		hr = pTexture->GetSize(&uTexWidth, &uTexHeight);
		KG_COM_PROCESS_ERROR(hr);
		nTotalPixels = (INT)(uTexWidth * uTexHeight);
		KG_PROCESS_ERROR(uTexWidth <= 2048 && uTexHeight <= 2048 && nInputArraySize >= nTotalPixels);
		{
			D3DLOCKED_RECT TempRect;
			LPDIRECT3DTEXTURE9 pD3DTex = pTexture->GetTexture();
			KG_PROCESS_ERROR(pD3DTex);
			hr = pD3DTex->LockRect(0, &TempRect, NULL, 0);
			KG_COM_PROCESS_ERROR(hr);
			{
				DWORD* pPixels = (DWORD*)TempRect.pBits;
				memcpy_s(pdwArray, sizeof(DWORD)*nTotalPixels, pPixels, sizeof(DWORD)*nTotalPixels);
				pD3DTex->UnlockRect(0);
			}
			return S_OK;
		}
Exit0:
		_KG3D_DEBUG_SELF_REPORT("失败：TextureTexToDWORDBuffer");
		return E_FAIL;
	}
};
////////////////////////////////////////////////////////////////////////////////

