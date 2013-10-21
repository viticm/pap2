#include "StdAfx.h"
#include "KG3DModelLightMap.h"
#include "KG3DTexture.h"
#include "KG3DTextureTable.h"

BOOL g_bShowLightMapAsChessboard = FALSE;

KG3DModelLightMap::KG3DModelLightMap(void)
{
	m_lpLightMapTexture = NULL;//光照图
	m_bUseShareTexture = FALSE;//是否使用共享的光照图
	m_vUVScale = D3DXVECTOR4(0,0,1,1);//共享光照图的UV缩放系数

	m_lpLightMapForCompute = NULL;//用于计算的光照图
	m_lpLightMapForCompute2 = NULL;//用于计算的光照图
	m_bComputing = FALSE;
	m_bUseSecond = FALSE;

	m_nKey = -1;
}

KG3DModelLightMap::~KG3DModelLightMap(void)
{
	UnInit();
}

VOID WINAPI GenerateColorFillWrite(D3DXVECTOR4* pOut, const D3DXVECTOR2* pTexCoord, const D3DXVECTOR2* pTexelSize, LPVOID pData)
{
	ASSERT(pOut);
	*pOut = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 1.0f);
}

HRESULT KG3DModelLightMap::Init(int nWidth,BOOL bComputing,LPSTR pFileName)
{
	HRESULT hr = E_FAIL;

	m_bComputing = bComputing;

	if(bComputing)
	{
		if(!m_lpLightMapForCompute || (m_nLightmapWidth!=nWidth) )
		{
			SAFE_RELEASE(m_lpLightMapForCompute);
			hr = D3DXCreateTexture(g_pd3dDevice,nWidth,nWidth,1,0,D3DFMT_A8R8G8B8,D3DPOOL_MANAGED,&m_lpLightMapForCompute);
			KGLOG_COM_PROCESS_ERROR(hr);
		}
		m_nLightmapWidth = nWidth;
		//D3DXFillTexture(m_lpLightMapForCompute,GenerateColorFillWrite,NULL);
	}
	else
	{
		SAFE_RELEASE(m_lpLightMapTexture);
		hr = g_cTextureTable.LoadResourceFromFile(pFileName,0,0,(IKG3DResourceBase**)&m_lpLightMapTexture);
		KGLOG_COM_PROCESS_ERROR(hr);

		UINT uWidth=0,uHeight =0;
		m_lpLightMapTexture->GetSize(&uWidth,&uHeight);

		m_nLightmapWidth = uWidth;
	}

	hr = S_OK;
Exit0:
	return hr;
}

HRESULT KG3DModelLightMap::UnInit()
{
	SAFE_RELEASE(m_lpLightMapForCompute);
	SAFE_RELEASE(m_lpLightMapForCompute2);
	SAFE_RELEASE(m_lpLightMapTexture);
	return S_OK;
}

LPDIRECT3DTEXTURE9 KG3DModelLightMap::GetLightMap()
{
	if(g_bShowLightMapAsChessboard)
		return g_cTextureTable.GetChessboardTexture(m_nLightmapWidth,m_nLightmapWidth);
	else if(m_bComputing)
		return m_lpLightMapForCompute;
	else if(m_lpLightMapTexture)
		return m_lpLightMapTexture->GetTexture();
	else
		return NULL;
}

HRESULT KG3DModelLightMap::EnableUseLightmap2(BOOL bEnable,float ratioA,float ratioB)
{
	HRESULT hr = E_FAIL;

	if(bEnable)
	{
		if(!m_lpLightMapForCompute2)
		{
			hr = D3DXCreateTexture(g_pd3dDevice,m_nLightmapWidth,m_nLightmapWidth,1,0,D3DFMT_A8R8G8B8,D3DPOOL_MANAGED,&m_lpLightMapForCompute2);
			KGLOG_COM_PROCESS_ERROR(hr);
		}
	}
	else
	{
		//////////////////////////////////////////////////////////////////////////
		KGLOG_PROCESS_ERROR(m_lpLightMapForCompute);
		KGLOG_PROCESS_ERROR(m_lpLightMapForCompute2);

		D3DLOCKED_RECT src,dest;

		hr = m_lpLightMapForCompute->LockRect(0,&dest,NULL,0);
		KGLOG_COM_PROCESS_ERROR(hr);

		hr = m_lpLightMapForCompute2->LockRect(0,&src,NULL,0);
		KGLOG_COM_PROCESS_ERROR(hr);
		//////////////////////////////////////////////////////////////////////////

		D3DCOLOR* pDest = (D3DCOLOR*)dest.pBits;
		D3DCOLOR* pSrc  = (D3DCOLOR*) src.pBits;
		
		float M = ratioA;
		float N = ratioB;

		for (int Y=0;Y<m_nLightmapWidth;Y++)
		{
			for (int X=0;X<m_nLightmapWidth;X++)
			{
				int nIndex = Y * m_nLightmapWidth + X;
				D3DXCOLOR sum;
				D3DXCOLOR A = Color2Value(pDest[nIndex]);
				D3DXCOLOR B = Color2Value(pSrc [nIndex]);

				sum.a = A.a * M + B.a * N;
				sum.r = A.r * M + B.r * N;
				sum.g = A.g * M + B.g * N;
				sum.b = A.b * M + B.b * N;

				sum.a = min(1.0F,sum.a);
				sum.r = min(1.0F,sum.r);
				sum.g = min(1.0F,sum.g);
				sum.b = min(1.0F,sum.b);

				pDest[nIndex] =  Value2Color(sum);
			}
		}
		//////////////////////////////////////////////////////////////////////////
		hr = m_lpLightMapForCompute2->UnlockRect(0);
		KGLOG_COM_PROCESS_ERROR(hr);

		hr = m_lpLightMapForCompute->UnlockRect(0);
		KGLOG_COM_PROCESS_ERROR(hr);

		//////////////////////////////////////////////////////////////////////////
		//SAFE_RELEASE(m_lpLightMapForCompute2);
	}
	m_bUseSecond = bEnable;

	hr = E_FAIL;
Exit0:
	return hr;
}
