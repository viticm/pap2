#include "stdafx.h"
#include "KSceneLogicRegionCellBuf.h"
#include "../KG3DEngineManager.h"

#define	MAX_LOGIC_TERRIAN_CELL_IN_REGION 4096
#define	MAX_LOGIC_ITEM_CELL_IN_REGION 4096 * 16

KG3DLogicRegionCellBuf::KG3DLogicRegionCellBuf()
{
	m_lpLogicalTerrian = 0;
	m_lpLogicalTerrianFace = 0;
	m_lpLogicalItem = 0;
	m_lpLogicalItemFace = 0;

	m_nCount1 = 0;
	m_nCount2 = 0;
	m_bAvailable = FALSE;
}

KG3DLogicRegionCellBuf::~KG3DLogicRegionCellBuf()
{
   if(m_lpLogicalTerrian)
   {
     m_lpLogicalTerrian->Release();
	 m_lpLogicalTerrian = 0;
   }

   if(m_lpLogicalTerrianFace)
   {
	   m_lpLogicalTerrianFace->Release();
	   m_lpLogicalTerrianFace = 0;
   }

   if(m_lpLogicalItem)
   {
	   m_lpLogicalItem->Release();
	   m_lpLogicalItem = 0;
   }

   if(m_lpLogicalItemFace)
   {
	   m_lpLogicalItemFace->Release();
	   m_lpLogicalItemFace = 0;
   }
}


//是否有效的CellBuf，失效的情况是鼠标抓取的范围不在地图之内
BOOL KG3DLogicRegionCellBuf::IsAvailable()
{
	return m_bAvailable;
}

HRESULT KG3DLogicRegionCellBuf::FetchCellDataFromLogicRegion(IELogicalSceneConnection* pLogicalScene, int nRegionX, int nRegionZ)
{
    //清0
	m_nCount1 = 0;
	m_nCount2 = 0;
	m_bAvailable = FALSE;
	HRESULT hr = S_OK;

	const KG3DLogicalConstances& lgC = g_GetLogicalConstants();

	GraphicsStruct::RenderStateCullMode CullMode(D3DCULL_NONE);

	if( m_lpLogicalTerrian == NULL)
	{
		if( FAILED( hr = g_pd3dDevice->CreateVertexBuffer( MAX_LOGIC_TERRIAN_CELL_IN_REGION * 8 * sizeof(IELogicalVertex),
			0, D3DFVF_XYZ|D3DFVF_DIFFUSE,
			D3DPOOL_MANAGED, &m_lpLogicalTerrian,NULL ) ) )
		{
			m_bAvailable = FALSE;
			return E_FAIL;
		}
	}

	if( m_lpLogicalTerrianFace == NULL)
	{
		if( FAILED( hr = g_pd3dDevice->CreateVertexBuffer( MAX_LOGIC_TERRIAN_CELL_IN_REGION * 12 * sizeof(IELogicalVertex),
			0, D3DFVF_XYZ|D3DFVF_DIFFUSE,
			D3DPOOL_MANAGED, &m_lpLogicalTerrianFace, NULL ) ) )
		{
			m_bAvailable = FALSE;
			return E_FAIL;
		}
	}

	if (m_lpLogicalItem == NULL)
	{
		if( FAILED( hr = g_pd3dDevice->CreateVertexBuffer( MAX_LOGIC_ITEM_CELL_IN_REGION * 8 * sizeof(IELogicalVertex),
			0, D3DFVF_XYZ|D3DFVF_DIFFUSE,
			D3DPOOL_MANAGED, &m_lpLogicalItem, NULL ) ) )
		{
			m_bAvailable = FALSE;
			return E_FAIL;
		}
	}

	if (m_lpLogicalItemFace == NULL)
	{ 
		if( FAILED( hr = g_pd3dDevice->CreateVertexBuffer( MAX_LOGIC_ITEM_CELL_IN_REGION * 12 * sizeof(IELogicalVertex),
			0, D3DFVF_XYZ|D3DFVF_DIFFUSE,
			D3DPOOL_MANAGED, &m_lpLogicalItemFace, NULL ) ) )
		{
			m_bAvailable = FALSE;
			return E_FAIL;
		}
	}


	KRegion* pLogicalRegion = pLogicalScene->GetRegion(nRegionX, nRegionZ);

	if(!pLogicalRegion)
	{
		m_bAvailable = FALSE;
		return E_FAIL;
	}



	IELogicalVertex* pL1 = NULL;
	IELogicalVertex* pL2 = NULL;
	IELogicalVertex* pL3 = NULL;
	IELogicalVertex* pL4 = NULL;

	if( m_lpLogicalTerrian == NULL)
	{
		if( FAILED( hr = g_pd3dDevice->CreateVertexBuffer( MAX_LOGIC_TERRIAN_CELL_IN_REGION * 8 * sizeof(IELogicalVertex),
			0, D3DFVF_XYZ|D3DFVF_DIFFUSE,
			D3DPOOL_MANAGED, &m_lpLogicalTerrian,NULL ) ) )
		{
			return E_FAIL;
		}
	}

	if( m_lpLogicalTerrianFace == NULL)
	{
		if( FAILED( hr = g_pd3dDevice->CreateVertexBuffer( MAX_LOGIC_TERRIAN_CELL_IN_REGION * 12 * sizeof(IELogicalVertex),
			0, D3DFVF_XYZ|D3DFVF_DIFFUSE,
			D3DPOOL_MANAGED, &m_lpLogicalTerrianFace, NULL ) ) )
		{
			return E_FAIL;
		}
	}

	if (m_lpLogicalItem == NULL)
	{
		if( FAILED( hr = g_pd3dDevice->CreateVertexBuffer( MAX_LOGIC_ITEM_CELL_IN_REGION * 8 * sizeof(IELogicalVertex),
			0, D3DFVF_XYZ|D3DFVF_DIFFUSE,
			D3DPOOL_MANAGED, &m_lpLogicalItem, NULL ) ) )
		{
			return E_FAIL;
		}
	}

	if (m_lpLogicalItemFace == NULL)
	{ 
		if( FAILED( hr = g_pd3dDevice->CreateVertexBuffer( MAX_LOGIC_ITEM_CELL_IN_REGION * 12 * sizeof(IELogicalVertex),
			0, D3DFVF_XYZ|D3DFVF_DIFFUSE,
			D3DPOOL_MANAGED, &m_lpLogicalItemFace, NULL ) ) )
		{
			return E_FAIL;
		}
	}

	if( FAILED( m_lpLogicalTerrian->Lock( 0, 0, (void**)&pL1, 0 ) ) )
		return E_FAIL;

	if (FAILED(m_lpLogicalTerrianFace->Lock(0, 0, (void**)&pL2, 0)))
	{
		m_lpLogicalTerrian->Unlock();
		return E_FAIL;
	}

	if (FAILED(m_lpLogicalItem->Lock(0, 0, (void**)&pL3, 0)))
	{
		m_lpLogicalTerrian->Unlock();
		m_lpLogicalTerrianFace->Unlock();
		return E_FAIL;
	}

	if (FAILED(m_lpLogicalItemFace->Lock(0, 0, (void**)&pL4, 0)))
	{
		m_lpLogicalTerrian->Unlock();
		m_lpLogicalTerrianFace->Unlock();
		m_lpLogicalItem->Unlock();
		return E_FAIL;
	}

	//3200是一个逻辑Region的大小，也是一个地形region的两倍

	D3DXVECTOR3 vStart(3200.0f * nRegionX, 0.0f, 3200.0f * nRegionZ);

	for (int z=0;z<lgC.regionGridHeight;z++)
	{
		for (int x=0;x<lgC.regionGridWidth;x++)
		{
			if (m_nCount1 > MAX_LOGIC_TERRIAN_CELL_IN_REGION || m_nCount2 > MAX_LOGIC_ITEM_CELL_IN_REGION)
			{
				hr = E_FAIL;
				break;
			}


			{
				KCell* pCell = pLogicalScene->GetCellOfRegion(pLogicalRegion, (INT)lgC.regionGridWidth * z + x);//&pLogicalRegion->m_pCells[REGION_GRID_WIDTH * z + x];

				hr = pLogicalScene->FillLogicalCellsInfo(
					vStart, x, z, m_nCount1, pCell,
					pL1, pL2,	TRUE, FALSE, TRUE, MAX_LOGIC_TERRIAN_CELL_IN_REGION);
				if (FAILED(hr))
					break;
			}

			
			{
				KCell* pCell = pLogicalScene->GetCellOfRegion(pLogicalRegion, (INT)lgC.regionGridWidth * z + x);//&pLogicalRegion->m_pCells[REGION_GRID_WIDTH * z + x];

				hr = pLogicalScene->FillLogicalCellsInfo(
					vStart, x, z, m_nCount2, pCell,
					pL3, pL4, FALSE, TRUE, TRUE, MAX_LOGIC_ITEM_CELL_IN_REGION);
				if (FAILED(hr))
					break;
			}
		}
	}

	m_lpLogicalItemFace->Unlock();
	m_lpLogicalItem->Unlock();
	m_lpLogicalTerrianFace->Unlock();
	m_lpLogicalTerrian->Unlock();

	//抓取数据过程没有问题
	if(hr==S_OK)
	  m_bAvailable = TRUE;

	return hr;
}


//绘制逻辑cell
void KG3DLogicRegionCellBuf::DrawLogicCell(BOOL bTerrian, BOOL bItem, BOOL bSelected)
{
	HRESULT hr;

	DWORD Light =0;
	g_pd3dDevice->GetRenderState(D3DRS_LIGHTING,&Light);
	g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );	
	g_pd3dDevice->SetTexture(0,NULL);

	if(bTerrian)
	{	
		GraphicsStruct::RenderStateAlpha Alpha(0x20,FALSE,TRUE,D3DBLEND_SRCALPHA,D3DBLEND_INVSRCALPHA);
		hr = g_pd3dDevice->SetStreamSource( 0, m_lpLogicalTerrian, 0, sizeof(IELogicalVertex ) );
		KGLOG_COM_CHECK_ERROR(hr);
		hr = g_pd3dDevice->SetFVF( D3DFVF_XYZ |D3DFVF_DIFFUSE );
		KGLOG_COM_CHECK_ERROR(hr);
		hr = g_pd3dDevice->DrawPrimitive( D3DPT_LINELIST , 0 ,4 * m_nCount1);
		KGLOG_COM_CHECK_ERROR(hr);

		hr = g_pd3dDevice->SetStreamSource( 0, m_lpLogicalTerrianFace, 0, sizeof(IELogicalVertex ) );
		KGLOG_COM_CHECK_ERROR(hr);
		hr = g_pd3dDevice->SetFVF( D3DFVF_XYZ |D3DFVF_DIFFUSE );
		KGLOG_COM_CHECK_ERROR(hr);
		hr = g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0 ,4 * m_nCount1);
		KGLOG_COM_CHECK_ERROR(hr);
	}
	


	if(bItem)
	{
		GraphicsStruct::RenderStateAlpha Alpha(0x20,FALSE,TRUE,D3DBLEND_SRCALPHA,D3DBLEND_INVSRCALPHA);
		hr = g_pd3dDevice->SetStreamSource( 0, m_lpLogicalItem, 0, sizeof(IELogicalVertex ) );
		KGLOG_COM_CHECK_ERROR(hr);
		hr = g_pd3dDevice->SetFVF( D3DFVF_XYZ |D3DFVF_DIFFUSE);
		KGLOG_COM_CHECK_ERROR(hr);
		hr = g_pd3dDevice->DrawPrimitive( D3DPT_LINELIST , 0, 4 * m_nCount2);
		KGLOG_COM_CHECK_ERROR(hr);

		hr = g_pd3dDevice->SetStreamSource( 0, m_lpLogicalItemFace, 0, sizeof(IELogicalVertex ) );
		KGLOG_COM_CHECK_ERROR(hr);
		hr = g_pd3dDevice->SetFVF( D3DFVF_XYZ |D3DFVF_DIFFUSE );
		KGLOG_COM_CHECK_ERROR(hr);
		hr = g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0 ,4 * m_nCount2);
		KGLOG_COM_CHECK_ERROR(hr);
	}

	g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, Light );
}