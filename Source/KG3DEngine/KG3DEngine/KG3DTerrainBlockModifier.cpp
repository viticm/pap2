#include "StdAfx.h"
#include "KG3DTerrainBlockModifier.h"
#include "KG3DTerrainBlock.h"
#include "KG3DTerrainBlockGrass.h"
#include "KG3DMemoryFile.h"
#include "KG3DSceneDataInterface.h"
#include "KG3DSceneBlock.h"
#include "KG3DHeightMap.h"
#include "KG3DSceneOutDoorSpaceMgr.h"
#include "KG3DTerrainDetailTexture.h"
#include "KG3DGraphicsTool.h"
#include "KG3DGraphiceEngine.h"
#include "../../EditInterface/IEKG3DTerrain.h"
#include "KG3DTextureTable.h"
#include "KG3DIntersection.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


KG3DTerrainBlockModifier g_cTerrainBlockModifier;

extern int GetSceneRegionIndex(float In);
extern int GetSceneSectionIndex(float In);

extern int g_RatotionAngle[20];
extern int g_RandRatotionAngle;
extern int g_nCurrentIndex;
extern int g_nLastIndex; 

extern bool    g_bIsTurnToMouseL;


KG3DTerrainBlockModifier::KG3DTerrainBlockModifier(void)
{
	m_bInvertOperation = FALSE;
	//m_bIsUseOldLightMap = false;
	//m_nBrushShape = 0;
	m_fAlpha = 20;
	m_fHeight = 10;
	//m_nSmallTextureValue = 0;
	m_bIsGetAbsoluteHeight = true;
	m_fDiShu = 2.0f;
	m_fAlphaTest = 0.2f;
	//m_nCurPaintTexture = -1;

	//m_nBrushCenterX = 0;
	//m_nBrushCenterZ = 0;
	m_Brush.wBrushSize = 10;
	m_fOppositeHeightValue = 0.0f;
	m_Brush.wOuterHeightBrushSize = 20;
	m_Brush.wInsideHeightBrushSize = 10;
	//m_SelectProjectionPlane = D3DXPLANE(0.0,1.0,0.0,0.0);
	m_PickRayIntersection = D3DXVECTOR3(0,0,0);
	//m_Brush.Init();	
	m_dwState = 0;
	m_fWidthMin = 100;
	m_fWidthMax = 200; 
	m_fHeightMin = 100;
	m_fHeightMax = 200;
	m_fNumPerM2 = 0.1F;
	m_dwType = 0;

	m_byPaintTexIndex = 0;
	m_pTextureIndex = NULL;

//	m_nStrideX = 1;
//	m_nStrideY = 1; 

	m_nGrassDensity = 1;
	m_bAddAutoGrass = FALSE;
	m_pHeightMap = NULL;
	m_pDetailTextureIndexMap = NULL;
	m_nHegihtMapStartIndex_X = -10;
	m_nHegihtMapStartIndex_Z = -10;
	m_bFillHeightMap = TRUE;
	
}

KG3DTerrainBlockModifier::~KG3DTerrainBlockModifier(void)
{
	ASSERT(m_pTextureIndex == NULL);

	ASSERT(m_pHeightMap == NULL);
	ASSERT(m_pDetailTextureIndexMap == NULL);
}

HRESULT KG3DTerrainBlockModifier::Init()
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    BOOL bInitBrush = FALSE;

	ASSERT(m_pHeightMap == NULL);

    m_pHeightMap = new KG3DHeightMap;
    KGLOG_PROCESS_ERROR(m_pHeightMap);

    hr = m_Brush.Init();
    KGLOG_COM_PROCESS_ERROR(hr);

    bInitBrush = TRUE;

    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        if (bInitBrush)
        {
            hr = m_Brush.UnInit();
            KGLOG_COM_CHECK_ERROR(hr);
        }

        SAFE_DELETE(m_pHeightMap);
    }
    return hrResult;
}

HRESULT KG3DTerrainBlockModifier::UnInit()
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
	
	m_bAddAutoGrass = FALSE;

    hr = m_Brush.UnInit();
    KGLOG_COM_PROCESS_ERROR(hr);

    SAFE_DELETE(m_pHeightMap);

	SAFE_DELETE_ARRAY(m_pDetailTextureIndexMap);
	SAFE_DELETE_ARRAY(m_pTextureIndex);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KG3DTerrainBlockModifier::OnLostDevice()
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    hr = m_Brush.OnLostDevice();
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KG3DTerrainBlockModifier::OnResetDevice()
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    hr = m_Brush.OnResetDevice();
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

//HRESULT KG3DTerrainBlockModifier::SetBlockConverTexture(int nLevel,POINT Index,UINT uTerrainInfo,D3DXVECTOR2 A,D3DXVECTOR2 C,KG3DSceneDataInterface* pDataInterface)
//{
//	float X_Len = C.x - A.x;
//	float Z_Len = A.y - C.y;
//
//	D3DXVECTOR4 m_vConverUV,m_vBlockRect;
//	m_vBlockRect = GetSceneBlockRect(nLevel,Index.x,Index.y);
//
//	KGLOG_PROCESS_ERROR(pDataInterface);
//
//	m_vConverUV.x = (m_vBlockRect.x - A.x) / X_Len;
//	m_vConverUV.y = (m_vBlockRect.y - C.y) / Z_Len;
//	m_vConverUV.z =  m_vBlockRect.z        / X_Len;
//	m_vConverUV.w =  m_vBlockRect.w        / Z_Len;
//
//	//////////////////////////////////////////////////////////////////////////
//	DWORD dwSize = 0;
//	{
//		g_cMemoryFile.Reset();
//		g_cMemoryFile.write(&uTerrainInfo,sizeof(UINT));
//		g_cMemoryFile.write(&m_vConverUV,sizeof(D3DXVECTOR4));
//		dwSize = g_cMemoryFile.GetLength();
//	}
//
//	pDataInterface->Request_ModifyTerrainBlock(nLevel,Index,Terrain_Block_Convermap,dwSize,g_cMemoryFile.GetBuffer());
//
//	return S_OK;
//Exit0:
//	return E_FAIL;
//}

//HRESULT KG3DTerrainBlockModifier::SetBlockConverTexture(int nLevel,vector<POINT>*pvecTerrainBlockIndex,UINT uTerrainInfo,D3DXVECTOR2 A,D3DXVECTOR2 C,KG3DSceneDataInterface* pDataInterface)
//{
//	for (size_t i=0;i<pvecTerrainBlockIndex->size();i++)
//	{
//		POINT Index = (*pvecTerrainBlockIndex)[i];
//
//		SetBlockConverTexture(nLevel,Index,uTerrainInfo,A,C,pDataInterface);
//	}
//	return S_OK;
//}

HRESULT KG3DTerrainBlockModifier::SetBlockWaterHeight(int nLevel,POINT Index,KG3DHeightMap* pHeightMap,D3DXVECTOR2 A,D3DXVECTOR2 C,KG3DSceneDataInterface* pDataInterface)
{
	float U_Len = C.x - A.x;
	float V_Len = C.y - A.y;

	VFormat::_Faces_Normal_Diffuse_Texture0 WaterVertexs[17*17];//顶点

	float CellLength = 0;
	float BlockLength = 0;
	float fHeightMin,fHeightMax;

	KGLOG_PROCESS_ERROR(pHeightMap);
	KGLOG_PROCESS_ERROR(pDataInterface);

	SceneBlockFindLength(nLevel,BlockLength,CellLength);

	int K = 0;
	for (int nZ = 0;nZ <= BLOCK_CELL_COUNT;nZ++ )
	{
		for (int nX = 0;nX <= BLOCK_CELL_COUNT;nX++ )
		{
			int nIndex = nZ * (BLOCK_CELL_COUNT+1) + nX;

			WaterVertexs[nIndex].p.x = Index.x * BlockLength + nX * CellLength;
			WaterVertexs[nIndex].p.z = Index.y * BlockLength + nZ * CellLength;

			float U = (WaterVertexs[nIndex].p.x - A.x) / U_Len;
			float V = (WaterVertexs[nIndex].p.z - A.y) / V_Len;

			float H = WaterVertexs[nIndex].p.y = pHeightMap->GetHeight(U,V);
			WaterVertexs[nIndex].Normal = pHeightMap->GetNormal(U,V);

			if(K==0)
			{
				fHeightMin = fHeightMax = H;
				K++;
			}
			else
			{
				fHeightMax = max(H,fHeightMax);
				fHeightMin = min(H,fHeightMin);
			}
		}
	}


	//////////////////////////////////////////////////////////////////////////
	{
		DWORD dwVersion = 0;
		DWORD dwLength = 0;

		KG3DMemoryFile* pFile = g_cMemoryFilePool.RequestResource();
		if(pFile)
		{
			pFile->write(&dwVersion,sizeof(DWORD));
			pFile->write(&fHeightMax,sizeof(float));
			pFile->write(&fHeightMin,sizeof(float));
			pFile->write(WaterVertexs,sizeof(WaterVertexs));

			dwLength = pFile->GetLength();

			pDataInterface->Request_ModifyTerrainBlock(nLevel,Index,Terrain_Block_Water,dwLength,pFile->GetBuffer());
			g_cMemoryFilePool.FreeResource(pFile);
			pFile = NULL;
		}
	}

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DTerrainBlockModifier::SetBlockWaterHeight(int nLevel,vector<POINT>*pvecTerrainBlockIndex,KG3DHeightMap* pHeightMap,D3DXVECTOR2 A,D3DXVECTOR2 C,KG3DSceneDataInterface* pDataInterface)
{
	for (size_t i=0;i<pvecTerrainBlockIndex->size();i++)
	{
		POINT Index = (*pvecTerrainBlockIndex)[i];
		SetBlockWaterHeight(nLevel,Index,pHeightMap,A,C,pDataInterface);
	}
	return S_OK;
}
HRESULT KG3DTerrainBlockModifier::RenderBrush()
{
	GraphicsStruct::RenderStateCullMode cull(D3DCULL_NONE);
	GraphicsStruct::RenderStateAlpha Alpha(0x20,TRUE,TRUE,D3DBLEND_SRCALPHA,D3DBLEND_INVSRCALPHA);
	GraphicsStruct::RenderState Light(D3DRS_LIGHTING,FALSE);
	DWORD dwTextureStateValue[6];

	//add by zhangzhixiong(2009.11.19)
	g_pd3dDevice->GetTextureStageState(0, D3DTSS_COLOROP, &dwTextureStateValue[0]);
	g_pd3dDevice->GetTextureStageState(0, D3DTSS_ALPHAOP, &dwTextureStateValue[1]);
	g_pd3dDevice->GetTextureStageState(0, D3DTSS_COLORARG1, &dwTextureStateValue[2]);
	g_pd3dDevice->GetTextureStageState(0, D3DTSS_COLORARG2, &dwTextureStateValue[3]);
	g_pd3dDevice->GetTextureStageState(0, D3DTSS_ALPHAARG1, &dwTextureStateValue[4]);
	g_pd3dDevice->GetTextureStageState(0, D3DTSS_ALPHAARG2, &dwTextureStateValue[5]);
	//end

	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);

    if (m_Brush.m_lpOuterXMesh)
    {
        DWORD Light = 0;

        g_pd3dDevice->GetRenderState(D3DRS_LIGHTING,&Light);
        g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );	

        g_pd3dDevice->SetTexture(0,m_Brush.m_lpTexture1);

        g_pd3dDevice->SetTexture(1,NULL);

        m_Brush.m_lpOuterXMesh->DrawSubset(0);
        if(m_dwState!=MAP_PAINT_GROUND)
            m_Brush.m_lpInsideXMesh->DrawSubset(0);

        g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, Light );	
        g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
        g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
    }

	//add by zhangzhixiiong(2009.11.19)
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, dwTextureStateValue[0]);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, dwTextureStateValue[1]);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, dwTextureStateValue[2]);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, dwTextureStateValue[3]);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, dwTextureStateValue[4]);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, dwTextureStateValue[5]);
	//end
	return S_OK;
}
HRESULT KG3DTerrainBlockModifier::SetBrushPosition(KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr)
{
	D3DXVECTOR2 Pos2D(m_PickRayIntersection.x,m_PickRayIntersection.z);
	if((m_dwState == WATER_UP)||(m_dwState == WATER_DOWN)||(m_dwState == WATER_FLAT)||(m_dwState == WATER_NOISE)||
	   (m_dwState == WATER_SET)||(m_dwState ==  WATER_GET)||(m_dwState ==  WATER_FOLOW))
		m_Brush.SetPosition(Pos2D,m_Brush.wOuterHeightBrushSize*100.0F,m_Brush.wInsideHeightBrushSize*100.0F,m_Brush.wPaintTextureBrushSize * 100.0f,pOutDoorSpaceMgr,TRUE);
	else
		m_Brush.SetPosition(Pos2D,m_Brush.wOuterHeightBrushSize*100.0F,m_Brush.wInsideHeightBrushSize*100.0F,m_Brush.wPaintTextureBrushSize * 100.0f,pOutDoorSpaceMgr);
	return S_OK;
}
//////////////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DTerrainBlockModifier::_Brush::OnLostDevice()
{
    HRESULT hr = S_OK;

    ASSERT(m_lpTargetTexture == NULL || QueryRef(m_lpTargetTexture) == 1);

	SAFE_RELEASE(m_lpTargetTexture);

    SAFE_RELEASE(m_lpTargetDepthStencilSurface);

	return hr;
}

HRESULT KG3DTerrainBlockModifier::_Brush::OnResetDevice()
{
    HRESULT hrResult = E_FAIL;
	HRESULT hr = E_FAIL;

    ASSERT(m_lpTargetTexture == NULL);
    
    hr = g_pd3dDevice->CreateTexture(MAX_BRUSH_SIZE1, MAX_BRUSH_SIZE1, 1,
		D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_lpTargetTexture, NULL);
    KGLOG_COM_PROCESS_ERROR(hr);

    ASSERT(m_lpTargetDepthStencilSurface == NULL);

    hr  = g_pd3dDevice->CreateDepthStencilSurface(MAX_BRUSH_SIZE1, MAX_BRUSH_SIZE1, 
        D3DFMT_D24S8,D3DMULTISAMPLE_NONE, 0, TRUE, &m_lpTargetDepthStencilSurface, NULL);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    if (FAILED(hr))
    {
        SAFE_RELEASE(m_lpTargetDepthStencilSurface);
        SAFE_RELEASE(m_lpTargetTexture);
    }
	return hrResult;
}

HRESULT KG3DTerrainBlockModifier::_Brush::Init()
{
	HRESULT hrResult  = E_FAIL;
	HRESULT hrRetCode = E_FAIL;

	m_fWidth = 100.0F;
	wBrushSize = wlastBeushSize = 10;
	wOuterHeightBrushSize = 20;
	wInsideHeightBrushSize = 10;
	wPaintTextureBrushSize = 10;
	strCurrentBrushMap = " ";


	m_Center = D3DXVECTOR3(0,0,0);

	DWORD m_dwNumCell = 32;

	DWORD m_dNumPloy     = m_dwNumCell * m_dwNumCell;
	DWORD m_dNumFaces    = m_dNumPloy * 2;
	DWORD m_dNumVertices = (m_dwNumCell+1)*(m_dwNumCell+1);

	VFormat::_Faces_Texture1 *pPaintVertex  = NULL;
	WORD	*pPaintIndex                    = NULL;

	VFormat::FACES_DIFFUSE_TEXTURE1 *pVers = NULL;
	VFormat::FACES_DIFFUSE_TEXTURE1 *pVers2 = NULL;
	DWORD *pIndex   = NULL;
	DWORD *pAttrib  = NULL;
	DWORD *pIndex2   = NULL;
	DWORD *pAttrib2  = NULL;


	ASSERT(m_lpPaintTextureBrushVertex == NULL);

	hrRetCode = g_pd3dDevice->CreateVertexBuffer(
		m_dNumVertices * sizeof(VFormat::_Faces_Texture1), 
		0, 0,
		D3DPOOL_MANAGED, 
		&m_lpPaintTextureBrushVertex, NULL
		);
	KGLOG_COM_PROC_ERR_RET_ERR(hrRetCode);


    ASSERT(m_lpPaintTextureBrushIndex == NULL);

	hrRetCode = g_pd3dDevice->CreateIndexBuffer(
		sizeof(WORD) * m_dNumPloy * 6,
		D3DUSAGE_WRITEONLY, 
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED, 
		&m_lpPaintTextureBrushIndex,
		NULL
		);
	KGLOG_COM_PROC_ERR_RET_ERR(hrRetCode);


	hrRetCode = m_lpPaintTextureBrushVertex->Lock(
		0, 
		m_dNumVertices * sizeof(VFormat::_Faces_Texture1),
		(void **)&pPaintVertex,
		0
		);
	KGLOG_COM_PROC_ERR_RET_ERR(hrRetCode);

	hrRetCode = m_lpPaintTextureBrushIndex->Lock(
		0,
		sizeof(WORD) * m_dNumPloy * 6,
		(void **)&pPaintIndex,
		0
		);
	KGLOG_COM_PROC_ERR_RET_ERR(hrRetCode);

	ASSERT(m_lpOuterXMesh == NULL);

    hrRetCode = D3DXCreateMeshFVF(
		m_dNumFaces,
		m_dNumVertices,
		D3DXMESH_MANAGED | D3DXMESH_32BIT,
		D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1,
		g_pd3dDevice,
		&m_lpOuterXMesh
		);
	KGLOG_COM_PROC_ERR_RET_ERR(hrRetCode);

	ASSERT(m_lpInsideXMesh == NULL);

    hrRetCode = D3DXCreateMeshFVF(
		m_dNumFaces,
		m_dNumVertices,
		D3DXMESH_MANAGED | D3DXMESH_32BIT,
		D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1,
		g_pd3dDevice,
		&m_lpInsideXMesh
		);
	KGLOG_COM_PROC_ERR_RET_ERR(hrRetCode);


	hrRetCode = m_lpOuterXMesh->LockVertexBuffer(0, (void**)&pVers);
	KGLOG_COM_PROC_ERR_RET_ERR(hrRetCode);
	hrRetCode = m_lpInsideXMesh->LockVertexBuffer(0, (void**)&pVers2);
	KGLOG_COM_PROC_ERR_RET_ERR(hrRetCode);

	hrRetCode = m_lpOuterXMesh->LockIndexBuffer(0, (void**)&pIndex);
	KGLOG_COM_PROC_ERR_RET_ERR(hrRetCode);
	hrRetCode = m_lpInsideXMesh->LockIndexBuffer(0, (void**)&pIndex2);
	KGLOG_COM_PROC_ERR_RET_ERR(hrRetCode);

	hrRetCode = m_lpOuterXMesh->LockAttributeBuffer(0, (DWORD**)&pAttrib);
	KGLOG_COM_PROC_ERR_RET_ERR(hrRetCode);
	hrRetCode = m_lpInsideXMesh->LockAttributeBuffer(0, (DWORD**)&pAttrib2);
	KGLOG_COM_PROC_ERR_RET_ERR(hrRetCode);

	DWORD i = 0;

	float m_fCellWidth  = m_fWidth / m_dwNumCell;
	float m_fCellHeight = m_fWidth / m_dwNumCell;
	DWORD X = 0;

	for( X =0;X<=m_dwNumCell;X++)
	{
		for(DWORD Y =0;Y<=m_dwNumCell;Y++)
		{
			float PX = 0 + X * m_fCellWidth;
			float PZ = 0  + Y * m_fCellHeight;

			D3DXVECTOR2 Pos(PX,PZ);
			pVers[i].p = D3DXVECTOR3(PX,0,PZ);
			pVers2[i].p = D3DXVECTOR3(PX,0,PZ);

			pVers[i].diffuse = 0x40A0A0A0;
			pVers[i].tu1     = X*1.0f/m_dwNumCell;
			pVers[i].tv1     = Y*1.0f/m_dwNumCell;

			pVers2[i].diffuse = 0x40A0A0A0;
			pVers2[i].tu1     = X*1.0f/m_dwNumCell;
			pVers2[i].tv1     = Y*1.0f/m_dwNumCell;

			pPaintVertex[i].tu1 = pVers[i].tu1;
			pPaintVertex[i].tv1 = pVers[i].tv1;

			i++;
		}
	}

	DWORD Weight = m_dwNumCell +1;
	for(X =0;X<m_dwNumCell;X++)
	{
		for(DWORD Y =0;Y<m_dwNumCell;Y++)
		{
			DWORD PolyMaterialID = 0;
			DWORD PloyIndex = Y*m_dwNumCell +X;

			DWORD Vertex_A = X    *Weight+ Y;
			DWORD Vertex_B = (X+1)*Weight+ Y;
			DWORD Vertex_C = (X+1)*Weight+(Y+1);
			DWORD Vertex_D = X    *Weight+(Y+1);

			DWORD Faces_A1 = (PloyIndex*2)*3;
			DWORD Faces_B1 = Faces_A1 + 1;
			DWORD Faces_C1 = Faces_B1 + 1;

			pIndex[Faces_A1] = pIndex2[Faces_A1] = Vertex_A;
			pIndex[Faces_C1] = pIndex2[Faces_C1] = Vertex_B;
			pIndex[Faces_B1] = pIndex2[Faces_B1] = Vertex_D;
			pAttrib[PloyIndex*2] = pAttrib2[PloyIndex*2] = PolyMaterialID;

			pPaintIndex[Faces_A1] = (WORD)Vertex_A;
			pPaintIndex[Faces_C1] = (WORD)Vertex_B;
			pPaintIndex[Faces_B1] = (WORD)Vertex_D;

			DWORD Faces_A2 = (PloyIndex*2+1)*3;
			DWORD Faces_B2 = Faces_A2 + 1;
			DWORD Faces_C2 = Faces_B2 + 1;

			pIndex[Faces_A2] = pIndex2[Faces_A2] = Vertex_D;
			pIndex[Faces_C2] = pIndex2[Faces_C2] = Vertex_B;
			pIndex[Faces_B2] = pIndex2[Faces_B2] = Vertex_C;
			pAttrib[PloyIndex*2+1] = pAttrib2[PloyIndex*2+1] = PolyMaterialID;

			pPaintIndex[Faces_A2] = (WORD)Vertex_D;
			pPaintIndex[Faces_C2] = (WORD)Vertex_B;
			pPaintIndex[Faces_B2] = (WORD)Vertex_C;

		}
	}

	hrRetCode = InitBrushTexture();

	m_nShape = 0;

	hrRetCode = InitVertexBuffer();
	KGLOG_COM_PROC_ERR_RET_ERR(hrRetCode);

	hrRetCode = FillDataPool(m_lpRotationBrushMap, m_fPaintTexturePool, wOuterHeightBrushSize);
	KGLOG_COM_PROC_ERR_RET_ERR(hrRetCode);

	ReSizeQuads(wOuterHeightBrushSize);

	hrRetCode = RotationBrushMap(m_lpTerrainHeightBrushTexture,0);
	KGLOG_COM_PROC_ERR_RET_ERR(hrRetCode);

	hrRetCode = FillDataPool(m_lpTerrainHeightBrushTexture, m_fTerrainHeightPool, wOuterHeightBrushSize);
	KGLOG_COM_PROC_ERR_RET_ERR(hrRetCode);

	//	RotationTexture();
	hrResult = S_OK;
Exit0:
	if (pAttrib)
	{
		hrRetCode = m_lpOuterXMesh->UnlockAttributeBuffer();
		ASSERT(SUCCEEDED(hrRetCode));
		pAttrib = NULL;
	}


	if (pIndex)
	{
		hrRetCode = m_lpOuterXMesh->UnlockIndexBuffer();
		ASSERT(SUCCEEDED(hrRetCode));
		pIndex = NULL;
	}

	if (pVers)
	{
		hrRetCode = m_lpOuterXMesh->UnlockVertexBuffer();
		ASSERT(SUCCEEDED(hrRetCode));
		pVers = NULL;
	}

	if (pAttrib2)
	{
		hrRetCode = m_lpInsideXMesh->UnlockAttributeBuffer();
		ASSERT(SUCCEEDED(hrRetCode));
		pAttrib2 = NULL;
	}


	if (pIndex2)
	{
		hrRetCode = m_lpInsideXMesh->UnlockIndexBuffer();
		ASSERT(SUCCEEDED(hrRetCode));
		pIndex2 = NULL;
	}

	if (pVers2)
	{
		hrRetCode = m_lpInsideXMesh->UnlockVertexBuffer();
		ASSERT(SUCCEEDED(hrRetCode));
		pVers2 = NULL;
	}

	if (pPaintIndex)
	{
		hrRetCode = m_lpPaintTextureBrushIndex->Unlock();
		ASSERT(SUCCEEDED(hrRetCode));
		pPaintIndex = NULL;
	}

	if (pPaintVertex)
	{
		hrRetCode = m_lpPaintTextureBrushVertex->Unlock();
		ASSERT(SUCCEEDED(hrRetCode));
		pPaintVertex = NULL;
	}

	if (FAILED(hrResult))
	{
		SAFE_RELEASE(m_lpOuterXMesh);
		SAFE_RELEASE(m_lpInsideXMesh);
		SAFE_RELEASE(m_lpPaintTextureBrushIndex);
		SAFE_RELEASE(m_lpPaintTextureBrushVertex);	
	}
	return hrResult;
}

HRESULT KG3DTerrainBlockModifier::_Brush::InitVertexBuffer()
{
	HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
	VFormat::_Faces_Texture1* pv = NULL;
    BOOL bLockDataQuadsVertex = FALSE;

    ASSERT(m_lpDataQuadsVertex == NULL);

	hr = g_pd3dDevice->CreateVertexBuffer(6 * sizeof(VFormat::_Faces_Texture1), 0, D3DFVF_Faces_Texture1, D3DPOOL_MANAGED, &m_lpDataQuadsVertex,NULL);
    KGLOG_COM_PROCESS_ERROR(hr);

	hr = m_lpDataQuadsVertex->Lock(0, 6 * sizeof(VFormat::_Faces_Texture1), (void**)&pv, 0);
    KGLOG_COM_PROCESS_ERROR(hr);

    bLockDataQuadsVertex = TRUE;

	pv[0].p = D3DXVECTOR3( - (float)MAX_BRUSH_SIZE1 / 2.0f, - (float)MAX_BRUSH_SIZE1 / 2.0f,0.0f);
    pv[0].tu1 = 0.0f;
    pv[0].tv1 = 1.0f;
	pv[1].p = D3DXVECTOR3( - (float)MAX_BRUSH_SIZE1 / 2.0f,(float)wOuterHeightBrushSize - MAX_BRUSH_SIZE1 / 2.0f,0.0f);	
    pv[1].tu1 = 0.0f;
    pv[1].tv1 = 0.0f;
	pv[2].p = D3DXVECTOR3((float)wOuterHeightBrushSize - MAX_BRUSH_SIZE1 / 2.0f,(float)wOuterHeightBrushSize - MAX_BRUSH_SIZE1 / 2.0f,0.0f);
    pv[2].tu1 = 1.0f;	
    pv[2].tv1 = 0.0f;

	pv[3].p = D3DXVECTOR3((float)wOuterHeightBrushSize - MAX_BRUSH_SIZE1 / 2.0f,(float)wOuterHeightBrushSize - MAX_BRUSH_SIZE1 / 2.0f,0.0f);
    pv[3].tu1 = 1.0f;
    pv[3].tv1 = 0.0f;
	pv[4].p = D3DXVECTOR3((float)wOuterHeightBrushSize - MAX_BRUSH_SIZE1 / 2.0f, - (float)MAX_BRUSH_SIZE1 / 2.0f,0.0f);
    pv[4].tu1 = 1.0f;
    pv[4].tv1 = 1.0f;
	pv[5].p = D3DXVECTOR3( - (float)MAX_BRUSH_SIZE1 / 2.0f, - (float)MAX_BRUSH_SIZE1 / 2.0f,0.0f);
    pv[5].tu1 = 0.0f;
    pv[5].tv1 = 1.0f;

    hrResult = S_OK;
Exit0:
    if (bLockDataQuadsVertex)
    {
        hr = m_lpDataQuadsVertex->Unlock();
        KGLOG_COM_CHECK_ERROR(hr);
    }
	return hrResult;
}

HRESULT KG3DTerrainBlockModifier::_Brush::ChangeBrushMap(const TCHAR *FileDirc)
{
	HRESULT hr;

	/*LPDIRECT3DSURFACE9 lpDesS = NULL;
	hr = lpBrushMap->GetSurfaceLevel(0,&lpDesS);
	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

	hr = D3DXLoadSurfaceFromFile(lpDesS,NULL,NULL,FileDirc,NULL,D3DX_FILTER_LINEAR,NULL,NULL);
	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

	lpDesS->Release();*/
	SAFE_RELEASE(m_lpBrushMap);

	hr = KG3DD3DXCreateTextureFromFile(g_pd3dDevice, FileDirc, &m_lpBrushMap);
	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

	/*SAFE_RELEASE(lpRotationBrushMap);

	hr = KG3DD3DXCreateTextureFromFile(g_pd3dDevice,FileDirc,&lpRotationBrushMap);
	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);*/

	hr = RotationBrushMap(m_lpBrushMap, 0);
	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

	hr = FillDataPool(m_lpRotationBrushMap, m_fPaintTexturePool, wOuterHeightBrushSize);
	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

	return S_OK;
Exit0:
	return hr;
}

HRESULT KG3DTerrainBlockModifier::_Brush::ReSizeHeightData(const WORD BrushSize)
{
	wOuterHeightBrushSize = BrushSize;
	ReSizeQuads(BrushSize);
	//    RotationBrushMap(m_lpTerrainHeightBrushTexture,0);
	FillDataPool(m_lpTerrainHeightBrushTexture, m_fTerrainHeightPool, BrushSize);

	return S_OK;
}
HRESULT KG3DTerrainBlockModifier::_Brush::ReSizeQuads(const int BrushSize)
{
	HRESULT hr;

	VFormat::_Faces_Texture1 *pv = NULL;

	if(!m_lpDataQuadsVertex)
	{
		hr = g_pd3dDevice->CreateVertexBuffer(6 * sizeof(VFormat::_Faces_Texture1),0,D3DFVF_Faces_Texture1,D3DPOOL_MANAGED,
			&m_lpDataQuadsVertex,NULL);
		KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);
	}

	m_lpDataQuadsVertex->Lock(0,6 * sizeof(VFormat::_Faces_Texture1),(void **)&pv,0);

	pv[0].p=  D3DXVECTOR3( - (float)MAX_BRUSH_SIZE1 / 2.0f, - (float)MAX_BRUSH_SIZE1 / 2.0f,0.0f);	
	pv[1].p=  D3DXVECTOR3( - (float)MAX_BRUSH_SIZE1 / 2.0f,(float)wOuterHeightBrushSize - MAX_BRUSH_SIZE1 / 2.0f,0.0f);	
	pv[2].p=  D3DXVECTOR3((float)BrushSize - MAX_BRUSH_SIZE1 / 2.0f,(float)BrushSize - MAX_BRUSH_SIZE1 / 2.0f,0.0f);	

	pv[3].p=  D3DXVECTOR3((float)BrushSize - MAX_BRUSH_SIZE1 / 2.0f,(float)BrushSize - MAX_BRUSH_SIZE1 / 2.0f,0.0f);	
	pv[4].p=  D3DXVECTOR3((float)BrushSize - MAX_BRUSH_SIZE1 / 2.0f, - (float)MAX_BRUSH_SIZE1 / 2.0f,0.0f);	
	pv[5].p=  D3DXVECTOR3( - (float)MAX_BRUSH_SIZE1 / 2.0f, - (float)MAX_BRUSH_SIZE1 / 2.0f,0.0f);	
	hr = m_lpDataQuadsVertex->Unlock();

	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

	return S_OK;
Exit0:
	return hr;
} 

HRESULT KG3DTerrainBlockModifier::_Brush::InitBrushTexture()
{
    HRESULT hrResult = E_FAIL;
	HRESULT hr = E_FAIL;
	LPDIRECT3DSURFACE9 lpS1 = NULL;
    LPDIRECT3DSURFACE9 lpS2 = NULL;

    ASSERT(m_lpTexture1 == NULL);
//    ASSERT(m_lpTexture2 == NULL);
    ASSERT(m_lpBrushMap == NULL);
    ASSERT(m_lpRotationBrushMap == NULL);
    ASSERT(m_lpTerrainHeightBrushTexture == NULL);
    ASSERT(m_lpErrorTexture == NULL);
    ASSERT(m_lpTargetCopyTexture == NULL);
    ASSERT(m_lpTargetDepthStencilSurface == NULL);

	hr = KG3DD3DXCreateTextureFromFile(g_pd3dDevice, "Data\\editor\\brush\\baush2.tga", &m_lpTexture1);
    KGLOG_COM_PROCESS_ERROR(hr);

//    hr = KG3DD3DXCreateTextureFromFile(g_pd3dDevice, "Data\\editor\\brush\\baush1.tga", &m_lpTexture2);
//    KGLOG_COM_PROCESS_ERROR(hr);

    hr = KG3DD3DXCreateTextureFromFile(g_pd3dDevice, "Data\\editor\\brush\\笔刷\\笔刷1.tga", &m_lpBrushMap);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = KG3DD3DXCreateTextureFromFile(g_pd3dDevice, "Data\\editor\\brush\\笔刷\\笔刷1.tga", &m_lpRotationBrushMap);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = KG3DD3DXCreateTextureFromFile(g_pd3dDevice, "Data\\editor\\brush\\brushterrain.tga", &m_lpTerrainHeightBrushTexture);
	if (FAILED(hr))
		KGLogPrintf(KGLOG_ERR, "缺少资源 Data\\editor\\brush\\brushterrain.tga");
//		MessageBox(NULL, "缺少资源 Data\\editor\\brush\\brushterrain.tga",0, 0);
    KGLOG_COM_PROCESS_ERROR(hr);

	hr = KG3DD3DXCreateTextureFromFile(g_pd3dDevice, "Data\\editor\\brush\\error.tga", &m_lpErrorTexture);
    KGLOG_COM_PROCESS_ERROR(hr);

	hr = g_pd3dDevice->CreateTexture(MAX_BRUSH_SIZE1, MAX_BRUSH_SIZE1, 1,
        D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_lpTargetTexture, NULL);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr  = g_pd3dDevice->CreateDepthStencilSurface(MAX_BRUSH_SIZE1, MAX_BRUSH_SIZE1, 
        g_cGraphicsEngine.GetPresentParameters().AutoDepthStencilFormat, D3DMULTISAMPLE_NONE, 0, TRUE, &m_lpTargetDepthStencilSurface, NULL);
    KGLOG_COM_PROCESS_ERROR(hr);

	hr = g_pd3dDevice->CreateTexture(MAX_BRUSH_SIZE1, MAX_BRUSH_SIZE1, 1, 
        D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &m_lpTargetCopyTexture, NULL);
    KGLOG_COM_PROCESS_ERROR(hr);

	if (m_lpTargetTexture && m_lpBrushMap)
	{
		hr = m_lpTargetTexture->GetSurfaceLevel(0, &lpS1);
        KGLOG_COM_PROCESS_ERROR(hr);

        hr = m_lpBrushMap->GetSurfaceLevel(0, &lpS2);
        KGLOG_COM_PROCESS_ERROR(hr);

        hr = D3DXLoadSurfaceFromSurface(lpS1, NULL, NULL, lpS2, NULL, NULL, D3DX_FILTER_LINEAR, NULL);
        KGLOG_COM_PROCESS_ERROR(hr);
	}

    hrResult = S_OK;
Exit0:
    SAFE_RELEASE(lpS1);
    SAFE_RELEASE(lpS2);

    if (FAILED(hrResult))
    {
        UnInitBrushTexture();
    }
	return hrResult;
}

HRESULT	KG3DTerrainBlockModifier::_Brush::UnInitBrushTexture()
{
    HRESULT hr = S_OK;

    SAFE_RELEASE(m_lpTexture1);
	SAFE_RELEASE(m_lpTexture2);
    SAFE_RELEASE(m_lpBrushMap);
    SAFE_RELEASE(m_lpRotationBrushMap);
    SAFE_RELEASE(m_lpTerrainHeightBrushTexture);
	SAFE_RELEASE(m_lpErrorTexture);
	SAFE_RELEASE(m_lpTargetTexture);
    SAFE_RELEASE(m_lpTargetDepthStencilSurface);
	SAFE_RELEASE(m_lpTargetCopyTexture);

    return hr;
}

HRESULT KG3DTerrainBlockModifier::_Brush::UnInit()
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

	SAFE_RELEASE(m_lpOuterXMesh);
	SAFE_RELEASE(m_lpInsideXMesh);

	SAFE_RELEASE(m_lpPaintTextureBrushVertex);
	SAFE_RELEASE(m_lpPaintTextureBrushIndex);
	SAFE_RELEASE(m_lpDataQuadsVertex);

    hr = UnInitBrushTexture();
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
	return hrResult;
}

KG3DTerrainBlockModifier::_Brush::_Brush()
{
	m_lpTexture1 = NULL;
	//m_lpTexture2 = NULL;
	m_lpOuterXMesh = NULL;
    m_lpInsideXMesh = NULL;
	m_lpPaintTextureBrushVertex = NULL;
	m_lpPaintTextureBrushIndex = NULL;
	m_lpDataQuadsVertex = NULL;
	m_lpTargetTexture = NULL;
    m_lpTargetDepthStencilSurface = NULL;
    m_lpTargetCopyTexture = NULL;
	m_lpBrushMap = NULL;
	m_lpErrorTexture = NULL;
	m_lpRotationBrushMap = NULL;
	m_lpTerrainHeightBrushTexture = NULL;

	m_nBrushState = 0;
}

KG3DTerrainBlockModifier::_Brush::~_Brush()
{
}

HRESULT KG3DTerrainBlockModifier::_Brush::SetPosition(D3DXVECTOR2 Pos,float OuterSize,float InsideSize,float PaintSize,KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr,BOOL bWater)
{
	HRESULT hr = S_OK;

	FRECT OuterXZRect,InsideXZRect;
	OuterXZRect.Left = Pos.x - OuterSize/2;
	OuterXZRect.Bottom  = Pos.y - OuterSize/2;
	InsideXZRect.Left = Pos.x - InsideSize/2;
	InsideXZRect.Bottom  = Pos.y - InsideSize/2;
	const float PaintLeft = Pos.x - PaintSize/8;
	const float PaintBottom = Pos.y - PaintSize/8;

	OuterXZRect.Width = OuterSize;
	OuterXZRect.Height = OuterSize;
	InsideXZRect.Width = InsideSize;
	InsideXZRect.Height = InsideSize;

	VFormat::FACES_DIFFUSE_TEXTURE1 * pVers = NULL;
	VFormat::FACES_DIFFUSE_TEXTURE1 * pVers2 = NULL;

	VFormat::_Faces_Texture1 *pPaintVertex = NULL;

	KGLOG_PROCESS_ERROR(m_lpPaintTextureBrushVertex);
	KGLOG_PROCESS_ERROR(m_lpOuterXMesh);
	KGLOG_PROCESS_ERROR(m_lpInsideXMesh);

	hr = m_lpPaintTextureBrushVertex->Lock(0,33 * 33 * sizeof(VFormat::_Faces_Texture1),
		(void **)&pPaintVertex,0);
	KGLOG_COM_PROCESS_ERROR(hr);


	hr = m_lpOuterXMesh->LockVertexBuffer(0,(void**)&pVers);
	if(FAILED(hr))
	{
		m_lpPaintTextureBrushVertex->Unlock();
	}
	KGLOG_COM_PROCESS_ERROR(hr);

	hr = m_lpInsideXMesh->LockVertexBuffer(0,(void**)&pVers2);
	if(FAILED(hr))
	{
		m_lpPaintTextureBrushVertex->Unlock();
		m_lpOuterXMesh->UnlockVertexBuffer();
	}
	KGLOG_COM_PROCESS_ERROR(hr);

	DWORD i = 0;

	DWORD m_dwNumCell = 32;

	const float m_fOuterCellWidth  = OuterXZRect.Width  / m_dwNumCell;
	const float m_fOuterCellHeight = OuterXZRect.Height / m_dwNumCell;
	const float m_fInsideCellWidth  = InsideXZRect.Width  / m_dwNumCell;
	const float m_fInsideCellHeight = InsideXZRect.Height / m_dwNumCell;
	const float PaintTextureCellWidth = PaintSize / 4 / m_dwNumCell;
	const float PaintTextureCellHeight = PaintSize / 4 / m_dwNumCell;

	for(DWORD X =0;X<=m_dwNumCell;X++)
	{
		for(DWORD Y =0;Y<=m_dwNumCell;Y++)
		{
			float OuterPX = OuterXZRect.Left + X * m_fOuterCellWidth;
			float OuterPZ = OuterXZRect.Bottom  + Y * m_fOuterCellHeight;
			float InsidePX = InsideXZRect.Left + X * m_fInsideCellWidth;
			float InsidePZ = InsideXZRect.Bottom  + Y * m_fInsideCellHeight;
			float PX2 = PaintLeft + X * PaintTextureCellWidth;
			float PZ2 = PaintBottom  + Y * PaintTextureCellHeight;

			D3DXVECTOR2 Pos(OuterPX,OuterPZ);
			D3DXVECTOR2 Pos2(PX2,PZ2);
			float H =0;
			if(bWater)
			{
				pOutDoorSpaceMgr->GetWaterHeight(H,OuterPX,OuterPZ);
				pVers[i].p = D3DXVECTOR3(OuterPX,H,OuterPZ);
				pOutDoorSpaceMgr->GetWaterHeight(H,InsidePX,InsidePZ);
				pVers2[i].p = D3DXVECTOR3(InsidePX,H,InsidePZ);
				pOutDoorSpaceMgr->GetWaterHeight(H,PX2,PZ2);
				pPaintVertex[i].p = D3DXVECTOR3(PX2,H,PZ2);
			}
			else
			{
				H = pOutDoorSpaceMgr->GetGroundHeight(OuterPX,OuterPZ);
				pVers[i].p = D3DXVECTOR3(OuterPX,H,OuterPZ);
				H = pOutDoorSpaceMgr->GetGroundHeight(InsidePX,InsidePZ);
				pVers2[i].p = D3DXVECTOR3(InsidePX,H,InsidePZ);
				H = pOutDoorSpaceMgr->GetGroundHeight(PX2,PZ2);
				pPaintVertex[i].p = D3DXVECTOR3(PX2,H,PZ2);
			}
			pVers[i].p.y += 10.0f;
			pVers2[i].p.y += 10.0f;
			pPaintVertex[i].p.y += 10.0f;

			i++;
		}
	}

	hr = m_lpOuterXMesh->UnlockVertexBuffer();
	hr = m_lpPaintTextureBrushVertex->Unlock();
	hr = m_lpInsideXMesh->UnlockVertexBuffer();

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT	KG3DTerrainBlockModifier::_Brush::RotationBrushMap(LPDIRECT3DTEXTURE9 lpTexture, int nAngle)
{
    HRESULT hrResult = E_FAIL;
	HRESULT hr = E_FAIL;
	D3DXMATRIX matView;
    D3DXMATRIX matProj;
    D3DXMATRIX matSaveView;
    D3DXMATRIX matSaveProj;
    D3DXMATRIX matRT;
    D3DXMATRIX matSaveRT;
	D3DVIEWPORT9 NewViewPort;
    D3DVIEWPORT9 SaveViewPort;
	LPDIRECT3DSURFACE9 lpOldRenderTargetSurface = NULL;
    LPDIRECT3DSURFACE9 lpOldDepthStencilSurface = NULL;
    LPDIRECT3DSURFACE9 lpNewRenderTargetSurface = NULL;
    LPDIRECT3DSURFACE9 lpRotationBrushMapSurface = NULL;
	LPDIRECT3DSURFACE9 lpTargetCopySurface = NULL;
    LPDIRECT3DSURFACE9 lpTargetSurface = NULL;
    D3DXVECTOR2 A;
    D3DXVECTOR2 B;
    D3DXVECTOR2 C;
    D3DXVECTOR2 D;
    D3DXVECTOR2 vCenter;
	float fAngle = D3DXToRadian((float)nAngle);
    float fRadius = 0.0f; // 贴图外接圆半径，也就是贴图对角线一半
	D3DSURFACE_DESC Desc;
	BOOL bPrepareRender = FALSE;
    BOOL bBeginScene = FALSE;
	GraphicsStruct::RenderState fog(D3DRS_FOGENABLE,FALSE);
	GraphicsStruct::RenderState RsZ(D3DRS_ZWRITEENABLE, FALSE);

	KG_PROCESS_ERROR(m_lpTargetTexture);
    KGLOG_PROCESS_ERROR(m_lpTargetDepthStencilSurface);

	hr = m_lpTargetTexture->GetSurfaceLevel(0, &lpNewRenderTargetSurface);
	KGLOG_COM_PROCESS_ERROR(hr);

	KGLOG_PROCESS_ERROR(lpNewRenderTargetSurface);

	hr = m_lpTargetTexture->GetLevelDesc(0, &Desc);
	KGLOG_COM_PROCESS_ERROR(hr);

	NewViewPort.Width = Desc.Width;
	NewViewPort.Height = Desc.Height;
	NewViewPort.X = 0;
    NewViewPort.Y = 0;
	NewViewPort.MinZ = 0.0f;
	NewViewPort.MaxZ = 1.0;

	D3DXMatrixIdentity(&matView);
	D3DXMatrixIdentity(&matProj);
	D3DXMatrixRotationZ(&matRT, fAngle);

	g_pd3dDevice->GetViewport(&SaveViewPort);
	g_pd3dDevice->GetTransform(D3DTS_VIEW, &matSaveView);
	g_pd3dDevice->GetTransform(D3DTS_PROJECTION, &matSaveProj);
	g_pd3dDevice->GetTransform(D3DTS_TEXTURE0, &matSaveRT);

	g_pd3dDevice->SetViewport(&NewViewPort);
	g_pd3dDevice->SetTransform(D3DTS_VIEW,&matView);
	g_pd3dDevice->SetTransform(D3DTS_PROJECTION,&matProj);
	
	hr = g_pd3dDevice->GetRenderTarget(0, &lpOldRenderTargetSurface);
	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

	hr = g_pd3dDevice->GetDepthStencilSurface(&lpOldDepthStencilSurface);
	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

	hr = g_pd3dDevice->SetRenderTarget(0, lpNewRenderTargetSurface);
	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

	hr = g_pd3dDevice->SetDepthStencilSurface(m_lpTargetDepthStencilSurface);
	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

	
    /*hr = g_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);*/

	bPrepareRender = TRUE;

	hr = g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0, 1.0f, 0);
	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

	hr = g_pd3dDevice->BeginScene();
	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

    bBeginScene = TRUE;

    hr = g_pd3dDevice->SetTransform(D3DTS_VIEW, &matRT);
	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

    hr = g_pd3dDevice->SetTexture(0, lpTexture);
	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

    hr = g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

    vCenter.x = NewViewPort.Width * 0.5f;
    vCenter.y = NewViewPort.Height * 0.5f;

    fRadius = sqrtf((float)NewViewPort.Width * NewViewPort.Width + (float)NewViewPort.Height * NewViewPort.Height) / 2.0f;

    A = D3DXVECTOR2(sinf(fAngle + 0.75f * D3DX_PI) * fRadius, cosf(fAngle + 0.75f * D3DX_PI) * fRadius) + vCenter;
    B = D3DXVECTOR2(sinf(fAngle + 0.25f * D3DX_PI) * fRadius, cosf(fAngle + 0.25f * D3DX_PI) * fRadius) + vCenter;
    C = D3DXVECTOR2(sinf(fAngle - 0.25f * D3DX_PI) * fRadius, cosf(fAngle - 0.25f * D3DX_PI) * fRadius) + vCenter;
    D = D3DXVECTOR2(sinf(fAngle - 0.75f * D3DX_PI) * fRadius, cosf(fAngle - 0.75f * D3DX_PI) * fRadius) + vCenter;

    hr = g_cGraphicsTool.DrawScreenRect(&A, &B,&C, &D, 0, 0xFFFFFFFF);
    KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

    hr = m_lpTargetTexture->GetSurfaceLevel(0, &lpTargetSurface);
	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

    hr = m_lpTargetCopyTexture->GetSurfaceLevel(0, &lpTargetCopySurface);
	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

    hr = m_lpRotationBrushMap->GetSurfaceLevel(0, &lpRotationBrushMapSurface);
	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

	hr = g_pd3dDevice->GetRenderTargetData(lpTargetSurface, lpTargetCopySurface);
	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

	hr = D3DXLoadSurfaceFromSurface(lpRotationBrushMapSurface, NULL, NULL, lpTargetCopySurface, NULL, NULL, D3DX_FILTER_LINEAR, NULL);
	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

    hr = D3DXFilterTexture(m_lpRotationBrushMap, NULL, 0, D3DX_FILTER_BOX);
	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);


	hr = g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

    hr = g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

	hrResult = S_OK;
Exit0:
    if (bBeginScene)
    {
        hr = g_pd3dDevice->EndScene();
        KGLOG_COM_CHECK_ERROR(hr);
    }
	if (bPrepareRender)
	{
        // 恢复

		hr = g_pd3dDevice->SetTransform(D3DTS_VIEW, &matSaveView);
        KGLOG_COM_CHECK_ERROR(hr);

		hr = g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &matSaveProj);
        KGLOG_COM_CHECK_ERROR(hr);

        hr = g_pd3dDevice->SetTransform(D3DTS_TEXTURE0, &matSaveRT);
        KGLOG_COM_CHECK_ERROR(hr);

        hr = g_pd3dDevice->SetDepthStencilSurface(lpOldDepthStencilSurface);
        KGLOG_COM_CHECK_ERROR(hr);

        hr = g_pd3dDevice->SetRenderTarget(0, lpOldRenderTargetSurface);
        KGLOG_COM_CHECK_ERROR(hr);

        hr = g_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
        KGLOG_COM_CHECK_ERROR(hr);

        hr = g_pd3dDevice->SetViewport(&SaveViewPort);
        KGLOG_COM_CHECK_ERROR(hr);
	}

	KG_COM_RELEASE(lpNewRenderTargetSurface);
    KG_COM_RELEASE(lpOldDepthStencilSurface);
	KG_COM_RELEASE(lpOldRenderTargetSurface);
    KG_COM_RELEASE(lpRotationBrushMapSurface);
    KG_COM_RELEASE(lpTargetCopySurface);
    KG_COM_RELEASE(lpTargetSurface);

    return hrResult;
}

HRESULT KG3DTerrainBlockModifier::_Brush::FillDataPool(LPDIRECT3DTEXTURE9 lpSrcTex,float *pDesData,const WORD BrushSize)
{
	HRESULT hr = E_FAIL;
	D3DSURFACE_DESC    d3dsd;
	D3DLOCKED_RECT     d3dlr;
	D3DSURFACE_DESC    d3dsd2;
	D3DLOCKED_RECT     d3dlr2;
	UINT i,j,ii;
	float fv;
	byte bv;
	const float step = 128.0f / BrushSize;
	const int inte = (int)step;
	const float sp = step - inte;
	float sumspz = 0.0f,sumspx = 0.0f;
	DWORD sumx = 0;
	LPDIRECT3DSURFACE9 DestSurface = NULL;
    LPDIRECT3DSURFACE9 TargetSurface = NULL;

	memset(pDesData,0,sizeof(float) * MAX_BRUSH_SIZE1 * MAX_BRUSH_SIZE1);

	KGLOG_PROCESS_ERROR(lpSrcTex);
	KGLOG_PROCESS_ERROR(m_lpTargetCopyTexture);

	
	hr = lpSrcTex->GetSurfaceLevel(0,&TargetSurface);
	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);
	hr = m_lpTargetCopyTexture->GetSurfaceLevel(0,&DestSurface);
	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

	hr = TargetSurface->GetDesc( &d3dsd );
	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);
	hr = TargetSurface->LockRect(&d3dlr,NULL,0);
	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

	hr = DestSurface->GetDesc( &d3dsd2 );
	if(FAILED(hr))
		TargetSurface->UnlockRect();
	
	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);
	hr = DestSurface->LockRect(&d3dlr2,NULL,0);
	if(FAILED(hr))
		TargetSurface->UnlockRect();
	
	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

	const DWORD dwDstPitch = (DWORD)d3dlr.Pitch;
	BYTE* pData = (BYTE*)d3dlr.pBits;
	BYTE *pBase = pData + (d3dsd.Height - 1) * dwDstPitch;

	const DWORD dwDstPitch2 = (DWORD)d3dlr2.Pitch;
	BYTE* pData2 = (BYTE*)d3dlr2.pBits;
	BYTE *pBase2 = pData2 + (d3dsd2.Height - 1) * dwDstPitch2;

	for(i = d3dsd.Height,ii = 0; i > d3dsd.Height - BrushSize; --i,++ii)
	{
		sumx = 0;
		for(j = 0;j < BrushSize; ++j)
		{

			if(sumspx >= 1.0f)
			{
				sumx += (inte + 1) * 4;
				sumspx -= 1.0f;
			}
			else
				sumx += inte * 4;

			bv = pBase[sumx];

			fv = (float)bv / 255;
			pDesData[ii * MAX_BRUSH_SIZE1 + j] = fv;
			pBase2[j * 4 + 0] = pBase2[j * 4 + 1] = pBase2[j * 4 + 2] = bv;
			pBase2[j * 4 + 3] = 255;

			sumspx += sp;
		}

		if(sumspz >= 1.0f)
		{
			pBase -= dwDstPitch * (inte + 1);
			sumspz -= 1.0f;
		}
		else
			pBase -= dwDstPitch * inte;

		sumspz += sp;

		pBase2 -= dwDstPitch2;
	}



	hr = DestSurface->UnlockRect();
	hr =TargetSurface->UnlockRect();

	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

	hr = S_OK;
Exit0:
    KG_COM_RELEASE(TargetSurface);
	KG_COM_RELEASE(DestSurface);
	return hr;
}

HRESULT KG3DTerrainBlockModifier::OnBrush(KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr)
{
	//(by dengzhihui 2006年9月19日 17:10:40
	//增加反向操作的支持
	DWORD dwstate = m_dwState;
	if (m_bInvertOperation)
	{
		switch(m_dwState)
		{
		case GROUND_UP:
			dwstate = GROUND_DOWN;
			break;
		case GROUND_DOWN:
			dwstate = GROUND_UP;
			break;
		}
	}
	//)
	switch(dwstate)
	{
	case MAP_PAINT_GROUND:
		{	
			if(m_Brush.wOuterHeightBrushSize != m_Brush.wlastBeushSize)
			{
				m_Brush.ReSizeQuads(m_Brush.wOuterHeightBrushSize);
				m_Brush.FillDataPool(m_Brush.m_lpRotationBrushMap, m_Brush.m_fPaintTexturePool, m_Brush.wOuterHeightBrushSize);
				m_Brush.wlastBeushSize = m_Brush.wOuterHeightBrushSize;
			}
			PaintGroundTexture(pOutDoorSpaceMgr);
			break;
		}
	case MAP_PAINT_COLOR:
		{	
			break;
		}
	case GROUND_UP:
		{
			ModifyGroundHeight(pOutDoorSpaceMgr,m_fAlpha);
			break;
		}
	case GROUND_DOWN:
		{
			ModifyGroundHeight(pOutDoorSpaceMgr,-m_fAlpha);
			break;
		}
	case GROUND_FLAT:
		{
			FlatTerrain(pOutDoorSpaceMgr);
			break;
		}
	case GROUND_NOISE:
		{
			
			break;	
		}
	case GROUND_SET:
		{
			SetGroundHeight(pOutDoorSpaceMgr);
			break;
		}
	case WATER_UP:
	case WATER_DOWN:
	case WATER_FLAT:
	case WATER_NOISE:
	case WATER_SET:
		{
			ModifyWaterHeight(pOutDoorSpaceMgr);
			break;
		}
	case WATER_FOLOW:
		{
			//OnBrushWater(m_dwState);
			break;
		}
	case ROAD_FLAT:
		{
			//UpdateSelectRegionHeightMap(100,16,m_Brush.wOuterHeightBrushSize);
			//FlatTerrain();
			//FlatCurrentRoad();
			break;
		}
	case MAP_CELLLOGICAL:
		{
			break;
		}

	case MAP_GRASS_SET:
		{
			DeleteTerrainObject(pOutDoorSpaceMgr,FALSE);
			AddTerrainObject(pOutDoorSpaceMgr);
			break;
		}
	case MAP_GRASS_DELETE:
		{
			DeleteTerrainObject(pOutDoorSpaceMgr,TRUE);
			break;
		}
	case MAP_AUTOGRASS_MANUALADD:
		{
			if(m_Brush.wOuterHeightBrushSize != m_Brush.wlastBeushSize)
			{
				m_Brush.ReSizeQuads(m_Brush.wOuterHeightBrushSize);
				m_Brush.FillDataPool(m_Brush.m_lpRotationBrushMap, m_Brush.m_fPaintTexturePool, m_Brush.wOuterHeightBrushSize);
				m_Brush.wlastBeushSize = m_Brush.wOuterHeightBrushSize;
			}
			AddAutoGrassByBrush(pOutDoorSpaceMgr);
			break;
		}
	case MAP_AUTOGRASS_DELETE:
		{
			DeleteAutoGrassByBrush(pOutDoorSpaceMgr);
			break;
		}	
	case MAP_TERRAINBLOCK_DELETE:
		{
			POINT Index;
			Index.x = GetSceneSectionIndex(m_PickRayIntersection.x);
			Index.y = GetSceneSectionIndex(m_PickRayIntersection.z);
			list<POINT> listScetion;
			listScetion.push_back(Index);
			OnDeleteTerrainBlock(pOutDoorSpaceMgr,listScetion);
			break;
		}
	case MAP_TERRAINBLOCK_ADD:
		{
			POINT Index;
			Index.x = GetSceneSectionIndex(m_PickRayIntersection.x);
			Index.y = GetSceneSectionIndex(m_PickRayIntersection.z);
			list<POINT> listScetion;
			listScetion.push_back(Index);
			OnAddTerrainBlock(pOutDoorSpaceMgr,listScetion);
			break;
		}
	}

	return S_OK;
}
HRESULT KG3DTerrainBlockModifier::OnDeleteTerrainBlock(KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr,list<POINT> listSection)
{
	HRESULT hr = E_FAIL;
	KG_PROCESS_ERROR(pOutDoorSpaceMgr);
	{
		list<POINT>::iterator it = listSection.begin();
		while (it != listSection.end())
		{
			POINT Index = (*it);
			KG3DSceneSection* pSection = pOutDoorSpaceMgr->GetSection(Index.x,Index.y);
			if(pSection)
			{
				vector<POINT> vecRegion;
				list<POINT> listRegion;

				D3DXVECTOR2 A,C;
				AABBOX box = pSection->GetAABBox();
				box.GetXZRect(A,C);
				C.x -= 100;
				C.y -= 100;
				pOutDoorSpaceMgr->SelectRegion(&vecRegion,A,C,TRUE);
				for (int i=0;i<(int)vecRegion.size();i++)
				{
					listRegion.push_back(vecRegion[i]);
				}
				pOutDoorSpaceMgr->RequestDeleteSceneBlock(BLOCK_TYPE_REGION,&listRegion,TRUE);
			}		
			++it;
		}
		pOutDoorSpaceMgr->RequestDeleteSceneBlock(BLOCK_TYPE_SECTION,&listSection,TRUE);
		
	}
	hr = S_OK;
Exit0:
	return hr;
}
HRESULT KG3DTerrainBlockModifier::OnAddTerrainBlock(KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr,list<POINT> listSection)
{
	HRESULT hr = E_FAIL;
	KG_PROCESS_ERROR(pOutDoorSpaceMgr);
	{
		list<POINT>::iterator it = listSection.begin();
		while(it != listSection.end())
		{
			int nX = (*it).x;
			int nZ = (*it).y;

			int PosX = nX * SCENESECTIONWIDTH + SCENESECTIONWIDTH / 2;
			int PosY = nZ * SCENESECTIONWIDTH + SCENESECTIONWIDTH / 2;
			int nZoneX = GetSceneZoneIndex((float)PosX);
			int nZoneZ = GetSceneZoneIndex((float)PosY);
			KG3DSceneZone* pZone = pOutDoorSpaceMgr->GetZone(nZoneX,nZoneZ);
			if(!pZone)
			{
				pOutDoorSpaceMgr->NewOneZone(&pZone,nZoneX,nZoneZ);
			}


			TryAddSection(pOutDoorSpaceMgr,nX,nZ);

			for(int Z = 0;Z<8;Z++)
			{
				for(int X = 0;X<8;X++)
				{
					int nIndex_X = nX*8 + X;
					int nIndex_Z = nZ*8 + Z;

					TryAddRegion(pOutDoorSpaceMgr,nIndex_X,nIndex_Z);
				}
			}
			++it;
		}
	}
	hr = S_OK;
Exit0:
	return hr;
}
HRESULT KG3DTerrainBlockModifier::TryAddSection(KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr,int nX,int nZ)
{
	list<POINT>listSection;
	POINT Index;
	Index.x = nX;
	Index.y = nZ;

	BOOL bTerrain = pOutDoorSpaceMgr->CheckTerrainBlockExist(BLOCK_TYPE_SECTION,Index);
	if(bTerrain)
		return S_OK;

	listSection.push_back(Index);
	D3DXVECTOR2 A((FLOAT)nX * 12800.0f, (FLOAT)nZ * 12800.0f);
	D3DXVECTOR2 C = A + D3DXVECTOR2(12800.0f, 12800.0f);
	
	

	pOutDoorSpaceMgr->RequestCreateSceneBlock(BLOCK_TYPE_SECTION,&listSection,TRUE);
	KG3DHeightMap vWaterMap;
	vWaterMap.Init(128,128);
	vWaterMap.SetHeightScale(0,1);
	for (int z=0;z<128;z++)
		for (int x=0;x<128;x++)
			(*vWaterMap.GetVertexPointer(x,z)) = -50;
	SetBlockWaterHeight(BLOCK_TYPE_SECTION,Index,&vWaterMap,A,C,pOutDoorSpaceMgr->GetSceneDataInterface());
	BYTE byteDetailTextureIndex[TERRAIN_BLOCK_MAX_DETAILMAP] = {0};
	BYTE byteTextureIndex[17 * 17] = {0};
	DWORD dwVersion = 0;
	int nNumDetail = 1;

	KG3DMemoryFile* pFile = g_cMemoryFilePool.RequestResource();
	if(pFile)
	{
		pFile->write(&dwVersion,sizeof(DWORD));
		pFile->write(&pOutDoorSpaceMgr->m_lpDetailMtls->m_uHandle,sizeof(UINT));
		pFile->write(byteDetailTextureIndex,sizeof(byteDetailTextureIndex));
		pFile->write(&nNumDetail, sizeof(int));
		pFile->write(byteTextureIndex,sizeof(BYTE)*17*17);

		DWORD dwLength = pFile->GetLength();
		pOutDoorSpaceMgr->m_lpDataInterface->Request_ModifyTerrainBlock(
			BLOCK_TYPE_SECTION, Index,Terrain_Block_DetailMtl, dwLength,pFile->GetBuffer()
			);
		g_cMemoryFilePool.FreeResource(pFile);
		pFile = NULL;
	}
	return S_OK;
}

HRESULT KG3DTerrainBlockModifier::TryAddRegion(KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr,int nX,int nZ)
{
	list<POINT>listRegion;
	POINT Index;
	Index.x = nX;
	Index.y = nZ;

	BOOL bTerrain = pOutDoorSpaceMgr->CheckTerrainBlockExist(BLOCK_TYPE_REGION,Index);
	if(bTerrain)
		return S_OK;

	listRegion.push_back(Index);
	D3DXVECTOR2 A((FLOAT)nX * 1600.0f, (FLOAT)nZ * 1600.0f);
	D3DXVECTOR2 C = A + D3DXVECTOR2(1600.0f, 1600.0f);
	pOutDoorSpaceMgr->RequestCreateSceneBlock(BLOCK_TYPE_REGION,&listRegion,TRUE);
	KG3DHeightMap vWaterMap;
	vWaterMap.Init(16,16);
	vWaterMap.SetHeightScale(0,1);
	for (int z=0;z<16;z++)
		for (int x=0;x<16;x++)
			(*vWaterMap.GetVertexPointer(x,z)) = -50;
	SetBlockWaterHeight(BLOCK_TYPE_REGION,Index,&vWaterMap,A,C,pOutDoorSpaceMgr->GetSceneDataInterface());

	BYTE byteDetailTextureIndex[TERRAIN_BLOCK_MAX_DETAILMAP] = {0};
	BYTE byteTextureIndex[17 * 17] = {0};
	DWORD dwVersion = 0;
	int nNumDetail = 1;

	KG3DMemoryFile* pFile = g_cMemoryFilePool.RequestResource();
	if(pFile)
	{
		pFile->write(&dwVersion,sizeof(DWORD));
		pFile->write(&pOutDoorSpaceMgr->m_lpDetailMtls->m_uHandle,sizeof(UINT));
		pFile->write(byteDetailTextureIndex,sizeof(byteDetailTextureIndex));
		pFile->write(&nNumDetail, sizeof(int));
		pFile->write(byteTextureIndex,sizeof(BYTE)*17*17);

		DWORD dwLength = pFile->GetLength();
		pOutDoorSpaceMgr->m_lpDataInterface->Request_ModifyTerrainBlock(
			BLOCK_TYPE_REGION, Index,Terrain_Block_DetailMtl, dwLength,pFile->GetBuffer()
			);

		g_cMemoryFilePool.FreeResource(pFile);
		pFile = NULL;
	}

	return S_OK;
}

HRESULT KG3DTerrainBlockModifier::OnBrushStart(KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr)
{
	switch(m_dwState)
	{
	case GROUND_UP:
	case GROUND_DOWN:
	case GROUND_FLAT:
	case GROUND_SET:
		if(m_bFillHeightMap)
		{
			D3DXVECTOR2 A(m_PickRayIntersection.x - 3 * SCENESECTIONWIDTH,m_PickRayIntersection.z - 3 * SCENESECTIONWIDTH);
			D3DXVECTOR2 C(A.x +  7 * SCENESECTIONWIDTH, A.y + 7 * SCENESECTIONWIDTH);
			//float fStartX = m_PickRayIntersection.x - 3 * SCENESECTIONWIDTH;
			//float fStartZ = m_PickRayIntersection.z - 3 * SCENESECTIONWIDTH; 
			FillHeightMap(A,C,pOutDoorSpaceMgr);
			m_bFillHeightMap = FALSE;
		}
		//m_dwCurrentState = BRUSH_HEIGHT_BEGION;
		//m_dwLastState = BRUSH_HEIGHT_BEGION;

		break;
	case GROUND_GET:
		{		
			if (pOutDoorSpaceMgr)
			{
				m_fHeight = pOutDoorSpaceMgr->GetGroundHeight(m_PickRayIntersection.x,m_PickRayIntersection.z);
			}
			//GetGroundHeight(m_fHeight,m_PickRayIntersection.x,m_PickRayIntersection.z);
			break;
		}
	case WATER_GET:
		{
			pOutDoorSpaceMgr->GetWaterHeight(m_fHeight,m_PickRayIntersection.x,m_PickRayIntersection.z); //m_HeightMap.GetWaterVertexHeight(m_PickRayIntersection.x,m_PickRayIntersection.z,this);
			break;
		}
	default:
		break;
	}
	return S_OK;
}

HRESULT KG3DTerrainBlockModifier::OnBrushEnd(KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr)
{
	if(m_dwState != MAP_PAINT_GROUND)
	{
		switch(m_dwState)
		{
		case GROUND_UP:
		case GROUND_DOWN:
		case GROUND_FLAT:
		case GROUND_SET:
			{	
				m_bFillHeightMap = TRUE;
				if (pOutDoorSpaceMgr && m_pHeightMap)
				{
					D3DXVECTOR2 A((float)m_nHegihtMapStartIndex_X* SCENESECTIONWIDTH,(float)m_nHegihtMapStartIndex_Z* SCENESECTIONWIDTH);
					D3DXVECTOR2 C(A.x +(m_pHeightMap->GetMapWidth())*100.0F,A.y + (m_pHeightMap->GetMapHeight())*100.0F);
					D3DXVECTOR2 vSelA(A.x + 100, A.y +100),vSelC(C.x - 200, C.y - 200);
					vector<POINT>vecZone,vecSection;
					pOutDoorSpaceMgr->SelectSection(&vecSection,vSelA,vSelC,TRUE);
					pOutDoorSpaceMgr->SelectZone(&vecZone,vSelA,vSelC,TRUE);

					SetBlockHeight(BLOCK_TYPE_SECTION,&vecSection,m_pHeightMap,A,C,pOutDoorSpaceMgr);//Section要在Zone前面，不然计算Terrain最高最低高度时,Zone的最高最低高度值可能有误差
					SetBlockHeight(BLOCK_TYPE_ZONE,&vecZone,m_pHeightMap,A,C,pOutDoorSpaceMgr);

				}
			}
			break;
		default:
			break;
		}
	}
	else
	{
		
		// rand r brush

		/*if(GetIsRandRatotionBrush())
		{
			srand( (unsigned)time( NULL ) ); 
			g_nCurrentIndex = rand() % 20;
			while (g_nCurrentIndex == g_nLastIndex)
			{
				g_nCurrentIndex = rand() % 20;
			}
			int angle = g_RatotionAngle[g_nCurrentIndex];
			g_nLastIndex = g_nCurrentIndex;

			angle -= 180;
			g_RandRatotionAngle = angle;
			SetBrushMapRatotionAngle(angle);
			m_Brush.RotationBrushMap(m_Brush.lpBrushMap,angle);
			m_Brush.FillDataPool(m_Brush.lpRotationBrushMap,m_Brush.m_fPaintTexturePool,
				m_Brush.wPaintTextureBrushSize);
		}*/
	}
	return S_OK;
}

HRESULT KG3DTerrainBlockModifier::SetObjectDataToTerrainBlock(int nLevel,POINT Index,DWORD dwDataType,KG3DTerrainBlockGrass* pBlock,UINT uHandleSet,KG3DSceneDataInterface* pDataInterface)
{
	HRESULT hr = E_FAIL;
	vector<KG3DTerrainGrass> vecObject;
	DWORD dwNum = 0;
	KG_PROCESS_ERROR(pBlock);
	if(!pBlock->m_bAutoGrass)
	{	
		pBlock->GetVecGrass(&vecObject);
		dwNum = (DWORD)vecObject.size();
	}
	//if(dwNum || pBlock->m_bAutoGrass)
	{
		KG3DMemoryFile* pFile = g_cMemoryFilePool.RequestResource();
		if(pFile)
		{
			DWORD dwVersion = 0;
			if(pBlock->m_bAutoGrass)
				dwVersion = 1;

			pFile->write(&dwVersion,sizeof(DWORD));
			pFile->write(&uHandleSet,sizeof(UINT));
			pFile->write(&dwNum,sizeof(DWORD));
			if(pBlock->m_bAutoGrass)
			{
				pFile->write(pBlock->m_lpAutoGrassData,sizeof(DWORD)*GRASS_NUM * GRASS_NUM);
				pFile->write(pBlock->m_lpAutoGrassWidth,sizeof(float)*GRASS_NUM * GRASS_NUM);
				pFile->write(pBlock->m_lpAutoGrassHeight,sizeof(float)*GRASS_NUM * GRASS_NUM);
			}
			else
			{
				vector<KG3DTerrainGrass>::iterator i = vecObject.begin();
				vector<KG3DTerrainGrass>::iterator iend = vecObject.end();
				while (i!=iend)
				{
					KG3DTerrainGrass ObjectDest = *i;
					pFile->write(&ObjectDest,sizeof(KG3DTerrainGrass));
					i++;
				}
			}
			DWORD deLength = pFile->GetLength();
			pDataInterface->Request_ModifyTerrainBlock(nLevel,Index,dwDataType,deLength,pFile->GetBuffer());
			g_cMemoryFilePool.FreeResource(pFile);
			pFile = NULL;
		}
	}
	hr = S_OK;
Exit0:
	return hr;
}
HRESULT KG3DTerrainBlockModifier::SetOppositeHeightValue(FLOAT fOppValue)
{
	m_fOppositeHeightValue = fOppValue;
	return S_OK;
}
HRESULT KG3DTerrainBlockModifier::SetIsGetAbsoluteHeight(BOOL bIsAbs)
{
	m_bIsGetAbsoluteHeight = bIsAbs;
	return S_OK;
}
float KG3DTerrainBlockModifier::GetModifyHeight()
{
	return m_fHeight;
}
HRESULT KG3DTerrainBlockModifier::SetModifyHeight(float& fHeight)
{
	m_fHeight = fHeight;
	return S_OK;
}
DWORD KG3DTerrainBlockModifier::GetTerrainState()
{
	return m_dwState;
}
HRESULT KG3DTerrainBlockModifier::SetTerrainState(DWORD dwState)
{
	m_dwState = dwState;
	return S_OK;
}
float KG3DTerrainBlockModifier::GetWidthMax()
{
	return m_fWidthMax;
}
float KG3DTerrainBlockModifier::GetWidthMin()
{
	return m_fWidthMin;
}
float KG3DTerrainBlockModifier::GetHeightMax()
{
	return m_fHeightMax;
}
float KG3DTerrainBlockModifier::GetHeightMin()
{
	return m_fHeightMin;
}
HRESULT KG3DTerrainBlockModifier::GetWidthMax(FLOAT* pValue)
{
	(*pValue) = m_fWidthMax;
	return S_OK;
}
HRESULT KG3DTerrainBlockModifier::GetWidthMin(FLOAT* pValue)
{
	(*pValue) = m_fWidthMin;
	return S_OK;
}
HRESULT KG3DTerrainBlockModifier::SetWidthMax(FLOAT fValue)
{
	m_fWidthMax = fValue;
	return S_OK;
}
HRESULT KG3DTerrainBlockModifier::SetWidthMin(FLOAT fValue)
{
	m_fWidthMin = fValue;
	return S_OK;
}
HRESULT KG3DTerrainBlockModifier::GetHeightMax(FLOAT* pValue)
{
	(*pValue) = m_fHeightMax;
	return S_OK;
}
HRESULT KG3DTerrainBlockModifier::GetHeightMin(FLOAT* pValue)
{
	(*pValue) = m_fHeightMin;
	return S_OK;
}
HRESULT KG3DTerrainBlockModifier::SetHeightMax(FLOAT fValue)
{
	m_fHeightMax = fValue;
	return S_OK;
}
HRESULT KG3DTerrainBlockModifier::SetHeightMin(FLOAT fValue)
{
	m_fHeightMin = fValue;
	return S_OK;
}
HRESULT KG3DTerrainBlockModifier::SetNumPerM2(FLOAT fValue)
{
	m_fNumPerM2 = fValue;
	return S_OK;
};

HRESULT KG3DTerrainBlockModifier::SetTextureIndexMax(INT nMax)
{
	m_nTextureMax =nMax;
	return S_OK;
}
HRESULT KG3DTerrainBlockModifier::SetTextureIndexMin(INT nMin)
{
	m_nTextureMin =nMin;
	return S_OK;
}
float KG3DTerrainBlockModifier::GetTerrainModifyAlpha()
{
	return m_fAlpha;
}
HRESULT KG3DTerrainBlockModifier::SetTerrainModifyAlpha(FLOAT fValue)
{
	m_fAlpha = fValue;
	return S_OK;
}
DWORD KG3DTerrainBlockModifier::GetInnerHeightBrushSize()
{
	return (DWORD)m_Brush.wInsideHeightBrushSize;
}
HRESULT KG3DTerrainBlockModifier::SetInnerHeightBrushSize(DWORD dwSize)
{
	m_Brush.wInsideHeightBrushSize = (WORD) dwSize;
	return S_OK;
}
HRESULT KG3DTerrainBlockModifier::SetOuterHeightBrushSize(DWORD dwSize)
{
	m_Brush.wOuterHeightBrushSize = (WORD) dwSize;
	return S_OK;
}
DWORD KG3DTerrainBlockModifier::GetOuterHeightBrushSize()
{
	return (DWORD)m_Brush.wOuterHeightBrushSize;
}
HRESULT KG3DTerrainBlockModifier::SetDiShu(FLOAT fPower)
{
	m_fDiShu = fPower;
	return S_OK;
}
HRESULT KG3DTerrainBlockModifier::ReSizeHeightData(const WORD BrushSize)
{
	return m_Brush.ReSizeHeightData(BrushSize);
}
HRESULT KG3DTerrainBlockModifier::SetPickRayIntersection(D3DXVECTOR3 vInter)
{
	this->m_PickRayIntersection = vInter;
	return S_OK;
}
DWORD KG3DTerrainBlockModifier::GetType()
{
	return m_dwType;
}
void KG3DTerrainBlockModifier::SetType(DWORD dwType)
{
	m_dwType = dwType;
}
D3DXVECTOR3 KG3DTerrainBlockModifier::GetPickRayIntersection()
{
	return this->m_PickRayIntersection;
}

HRESULT KG3DTerrainBlockModifier::UpdatePatternList(int* pList, const unsigned int uNumPattern)
{
	HRESULT hr = E_FAIL;
	m_vecPatternList.clear();
	KG_PROCESS_ERROR(pList);
	for (unsigned int i = 0; i < uNumPattern; i++)
	{
		m_vecPatternList.push_back(pList[i]);
	}
	hr = S_OK;
Exit0:
	return hr;
}
int KG3DTerrainBlockModifier::GetPatternIndexBySelection()
{
	int nReturn = -1;
	KG_PROCESS_ERROR(m_vecPatternList.size());
	nReturn = m_vecPatternList[rand() % m_vecPatternList.size()];
Exit0:
	return nReturn;
}
int KG3DTerrainBlockModifier::GetRandomTextureIndex()
{
	int nIndex = 0;
	if (m_pTextureIndex && m_nNumTextureIndex)
	{
		nIndex = m_pTextureIndex[rand() % m_nNumTextureIndex];
	}
	return nIndex;
}
HRESULT KG3DTerrainBlockModifier::SetTextureIndexInfo(INT* pPointer, INT* pNumIndex)
{
    HRESULT hrResult = E_FAIL;
    INT nNumIndex = 0;

    KGLOG_PROCESS_ERROR(pPointer);
    KGLOG_PROCESS_ERROR(pNumIndex);

    nNumIndex = *pNumIndex;

    if (m_nNumTextureIndex != nNumIndex)
        SAFE_DELETE_ARRAY(m_pTextureIndex);

    KG_PROCESS_SUCCESS(nNumIndex == 0);

    m_pTextureIndex = new int[nNumIndex];
    KGLOG_PROCESS_ERROR(m_pTextureIndex);

    for (INT i = 0; i < nNumIndex; i++)
    {
        m_pTextureIndex[i] = pPointer[i];
    }

Exit1:
    m_nNumTextureIndex = nNumIndex;

	hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        SAFE_DELETE_ARRAY(m_pTextureIndex);
    }
	return hrResult;
}

HRESULT KG3DTerrainBlockModifier::ClearAllTerrainObject(KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr)
{
	if (!pOutDoorSpaceMgr)
		return E_FAIL;
	
	D3DXVECTOR2 A,C;
	pOutDoorSpaceMgr->ForceLoadAllTerrainBlock();
	pOutDoorSpaceMgr->GetSceneRect(&A,&C);
	
	vector<POINT>vecRegion;
	pOutDoorSpaceMgr->SelectRegion(&vecRegion,A,C,TRUE);
	for (size_t i=0;i<vecRegion.size();i++)
	{
		POINT Index = vecRegion[i];
		KG3DSceneRegion* pRegion = pOutDoorSpaceMgr->GetRegion(Index.x,Index.y);
		if(pRegion && pRegion->m_lpTerrainBlock)
		{ 
			switch(m_dwType)
			{
			case MAP_GRASS:
				{
					KG3DTerrainBlockGrass* pGrassBlock = pRegion->m_lpTerrainBlock->m_lpGrassData;
					if(pGrassBlock)
					{
						pGrassBlock->ClearGrassList();
						g_cTerrainBlockModifier.SetObjectDataToTerrainBlock(BLOCK_TYPE_REGION,Index,Terrain_Block_Grass,pGrassBlock,
												pOutDoorSpaceMgr->m_uSetHandle[MAP_GRASS],pOutDoorSpaceMgr->GetSceneDataInterface());
					}
				}
				break;
			case MAP_STONE:
				{
					KG3DTerrainBlockGrass* pStoneBlock = pRegion->m_lpTerrainBlock->m_lpStoneData;
					if(pStoneBlock)
					{
						pStoneBlock->ClearGrassList();
						g_cTerrainBlockModifier.SetObjectDataToTerrainBlock(BLOCK_TYPE_REGION,Index,Terrain_Block_Stone,pStoneBlock,
												pOutDoorSpaceMgr->m_uSetHandle[MAP_STONE],pOutDoorSpaceMgr->GetSceneDataInterface());
					}
				}
				break;
			case MAP_FRUTEX:
				{
					KG3DTerrainBlockGrass* pFrutexBlock = pRegion->m_lpTerrainBlock->m_lpFrutexData;
					if(pFrutexBlock)
					{
						pFrutexBlock->ClearGrassList();
						g_cTerrainBlockModifier.SetObjectDataToTerrainBlock(BLOCK_TYPE_REGION,Index,Terrain_Block_Frutex,pFrutexBlock,
											pOutDoorSpaceMgr->m_uSetHandle[MAP_FRUTEX],pOutDoorSpaceMgr->GetSceneDataInterface());
					}
				}
				break;
			}

		}		
	}
	return S_OK;
}
HRESULT KG3DTerrainBlockModifier::DeleteTerrainObject(KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr,BOOL bIsModifierData)
{
	if (!pOutDoorSpaceMgr)
		return E_FAIL;

	float fBrushSize = g_cTerrainBlockModifier.m_Brush.wOuterHeightBrushSize*100.0F;
	D3DXVECTOR2 P(m_PickRayIntersection.x,m_PickRayIntersection.z);
	D3DXVECTOR2 A,C;
	FRECT rect(P,fBrushSize);
	rect.ToPosition(&A,&C);
	vector<POINT>vecRegion;
	pOutDoorSpaceMgr->ForceLoadTerrainBlock(A,C);
	pOutDoorSpaceMgr->SelectRegion(&vecRegion,A,C,TRUE);
	for (size_t i=0;i<vecRegion.size();i++)
	{
		POINT Index = vecRegion[i];
		KG3DSceneRegion* pRegion = pOutDoorSpaceMgr->GetRegion(Index.x,Index.y);
		if(pRegion && pRegion->m_lpTerrainBlock)
		{ 
			switch(m_dwType)
			{
			case MAP_GRASS:
				{
					KG3DTerrainBlockGrass* pGrassBlock = pRegion->m_lpTerrainBlock->m_lpGrassData;
					if(pGrassBlock)
						pGrassBlock->RemoveGrass(rect);
					if(bIsModifierData)
						g_cTerrainBlockModifier.SetObjectDataToTerrainBlock(BLOCK_TYPE_REGION,Index,Terrain_Block_Grass,pGrassBlock,
																			pOutDoorSpaceMgr->m_uSetHandle[MAP_GRASS],
																			pOutDoorSpaceMgr->GetSceneDataInterface());
				}
				break;
			case MAP_STONE:
				{
					KG3DTerrainBlockGrass* pStoneBlock = pRegion->m_lpTerrainBlock->m_lpStoneData;
					if(pStoneBlock)
						pStoneBlock->RemoveGrass(rect);
					if(bIsModifierData)
						g_cTerrainBlockModifier.SetObjectDataToTerrainBlock(BLOCK_TYPE_REGION,Index,Terrain_Block_Stone,pStoneBlock,
																			pOutDoorSpaceMgr->m_uSetHandle[MAP_STONE],
																			pOutDoorSpaceMgr->GetSceneDataInterface());
				}
			    break;
			case MAP_FRUTEX:
				{
					KG3DTerrainBlockGrass* pFrutexBlock = pRegion->m_lpTerrainBlock->m_lpFrutexData;
					if(pFrutexBlock)
						pFrutexBlock->RemoveGrass(rect);
					if(bIsModifierData)
						g_cTerrainBlockModifier.SetObjectDataToTerrainBlock(BLOCK_TYPE_REGION,Index,Terrain_Block_Frutex,pFrutexBlock,
																			pOutDoorSpaceMgr->m_uSetHandle[MAP_FRUTEX],
																			pOutDoorSpaceMgr->GetSceneDataInterface());
				}
			    break;
			}
			
		}		
	}
	return S_OK;
}
HRESULT KG3DTerrainBlockModifier::AddTerrainObject(KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr)
{
	if (!pOutDoorSpaceMgr)
		return E_FAIL;
	//DWORD dwLength = 0;
	float fBrushSize = m_Brush.wOuterHeightBrushSize* 100.0F;
	D3DXVECTOR2 P(m_PickRayIntersection.x,m_PickRayIntersection.z);
	FRECT rect(P,fBrushSize);
	
	D3DXVECTOR2 A,C;
	rect.ToPosition(&A,&C);

	int nPatternIndex = 0;
	float W = rect.Width;
	float H = rect.Height;
	int Num = (int)(W * H / 100000 * m_fNumPerM2);
	for (int i = 0; i < Num;i++)
	{
		float X = rand()%10000/10000.0F*W - 0.5F*W;
		float Z = rand()%10000/10000.0F*H - 0.5F*H;

		D3DXVECTOR3 Pos = D3DXVECTOR3(X,0,Z) + D3DXVECTOR3(P.x,0,P.y);

		
		int nIndex = 0;
		if (m_pTextureIndex && m_nNumTextureIndex)
		{
			nIndex = m_pTextureIndex[rand() % m_nNumTextureIndex];
		}

		float SWidth =max(5.0f, m_fWidthMax - m_fWidthMin);
		float Width =m_fWidthMin + rand() % 100 / 100.0F * SWidth;
		float SHeight = max(5.0f, m_fHeightMax - m_fHeightMin);
		float Height = m_fHeightMin + rand()%100/100.0F*SHeight;
		Pos.y = pOutDoorSpaceMgr->GetGroundHeight(Pos.x,Pos.z);

		nPatternIndex = GetPatternIndexBySelection();
		if (nPatternIndex == -1)
			continue;

		int nRegionX = GetSceneRegionIndex(Pos.x);
		int nRegionZ = GetSceneRegionIndex(Pos.z);

		KG3DSceneRegion* pRegion = pOutDoorSpaceMgr->GetRegion(nRegionX,nRegionZ);
		if(pRegion && pRegion->m_lpTerrainBlock)
		{
			KG3DTerrainBlockGrass* pBlock = NULL;
			switch(m_dwType)
			{
			case MAP_GRASS:
				pBlock = pRegion->m_lpTerrainBlock->GetTerrainGrassBlock(TRUE);
				break;
			case MAP_STONE:
				pBlock = pRegion->m_lpTerrainBlock->GetTerrainStoneBlock(TRUE);
				break;
			case MAP_FRUTEX:
				pBlock = pRegion->m_lpTerrainBlock->GetTerrainFrutexBlock(TRUE);
				break;
			}
				
			if(pBlock /*&& (!pBlock->m_bAutoGrass)*/)
			{
				KG3DTerrainGrass Grass;
				Grass.vPosition = Pos;
				Grass.byteIndex = (BYTE)nIndex;
				Grass.dwColor = 0xFF808080;
				Grass.bytePatternIndex =(BYTE)nPatternIndex;
				Grass.fWidth = Width;
				Grass.fHeight= Height;
				pOutDoorSpaceMgr->GetRegionNormal(Grass.vNormal,Pos.x,Pos.z);
				pBlock->AddGrass(Grass);
				pBlock->m_bAutoGrass = FALSE;
			}
		}
	}

	vector<POINT>vecRegion;
	pOutDoorSpaceMgr->SelectRegion(&vecRegion,A,C,TRUE);
	for (size_t i=0;i<vecRegion.size();i++)
	{
		POINT Index = vecRegion[i];
		KG3DSceneRegion* pRegion = pOutDoorSpaceMgr->GetRegion(Index.x,Index.y);
		if(pRegion && pRegion->m_lpTerrainBlock)
		{
			KG3DTerrainBlockGrass* pBlock = NULL;
			DWORD dwTypeData = Terrain_Block_Grass;
			UINT  uSetHandle =0;
			switch(m_dwType)
			{
			case MAP_GRASS:
				{
					pBlock = pRegion->m_lpTerrainBlock->GetTerrainGrassBlock();
					dwTypeData = Terrain_Block_Grass;
					uSetHandle = pOutDoorSpaceMgr->m_uSetHandle[MAP_GRASS];
					break;
				}
				
			case MAP_STONE:
				{
					pBlock = pRegion->m_lpTerrainBlock->GetTerrainStoneBlock();
					dwTypeData = Terrain_Block_Stone;
					uSetHandle = pOutDoorSpaceMgr->m_uSetHandle[MAP_STONE];
					break;
				}
				
			case MAP_FRUTEX:
				{
					pBlock = pRegion->m_lpTerrainBlock->GetTerrainFrutexBlock();
					dwTypeData = Terrain_Block_Frutex;
					uSetHandle = pOutDoorSpaceMgr->m_uSetHandle[MAP_FRUTEX];
					break;
				}
			}
			if(pBlock)
				SetObjectDataToTerrainBlock(BLOCK_TYPE_REGION,Index,dwTypeData,pBlock,uSetHandle,pOutDoorSpaceMgr->GetSceneDataInterface());
		}		
	}
	
	return S_OK;
}
HRESULT KG3DTerrainBlockModifier::ModifyGroundHeight(KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr,float fAlpha)
{
	int fBrushSize = m_Brush.wOuterHeightBrushSize * 100;
	int nBrushCenterX =(int) m_PickRayIntersection.x / 100;
	int nBrushCenterZ =(int) m_PickRayIntersection.z / 100;

	D3DXVECTOR2 P((float)(nBrushCenterX *100),(float)(nBrushCenterZ * 100));
	FRECT rect(P,(float)fBrushSize);
	D3DXVECTOR2 A,C;
	rect.ToPosition(&A,&C);
	vector<POINT>vecRegion;
	pOutDoorSpaceMgr->SelectRegion(&vecRegion,A,C,TRUE);
	int nSize = (int)vecRegion.size();
	if( nSize <= 0)
		return S_OK;

	KG_PROCESS_ERROR(m_pHeightMap);

	int nXS = vecRegion[0].x * TERRIANREGIONCELL;
	int nZS = vecRegion[0].y * TERRIANREGIONCELL;

	nSize = (int)m_Brush.wOuterHeightBrushSize/2;
	int nHeighMap_X = m_nHegihtMapStartIndex_X * 8 * TERRIANREGIONCELL;
	int nHeighMap_Z = m_nHegihtMapStartIndex_Z * 8 * TERRIANREGIONCELL;
	int nStarX = max(max(nXS,nBrushCenterX - nSize) - nHeighMap_X,0);
	int nStarZ = max(max(nZS,nBrushCenterZ - nSize) - nHeighMap_Z,0);
	
	int nEndX = min(m_pHeightMap->GetMapWidth()-1 ,nBrushCenterX + nSize - nHeighMap_X);
	int nEndZ = min(m_pHeightMap->GetMapHeight()-1 ,nBrushCenterZ + nSize - nHeighMap_Z);
	nBrushCenterX -= nHeighMap_X; 
	nBrushCenterZ -= nHeighMap_Z; 
	int weight = 0,lenght = 0;
	float K;
	const int r = nSize;
	const int sign = fAlpha > 0.0f ? 1 : -1;

	for(int z = nStarZ ; z <= nEndZ ; ++z)
	{
		for(int x = nStarX; x <= nEndX ; ++x)
		{
			int xx = x - (nBrushCenterX);
			int zz = z - (nBrushCenterZ);

			float *pHeight = m_pHeightMap->GetVertexPointer(x,z);
			lenght = (int)(sqrtf((float)xx * xx + (float)zz * zz));

			if(m_bIsGetAbsoluteHeight)
			{
				if(lenght > r)
					continue;

				if(lenght <= m_Brush.wInsideHeightBrushSize / 2)
					weight = (int)fAlpha;
				else
				{
					float ra = (float)(xx * xx + zz * zz) / (r * r / 2.0f);
					K = powf(m_fDiShu,-ra);
					weight = (int)(K * fAlpha);
				}
			}
			else
			{
				if(lenght > r)
					continue;

				if(lenght <= m_Brush.wInsideHeightBrushSize / 2)
					weight = (int)m_fOppositeHeightValue * sign;
				else
				{
					float ra = (float)(xx * xx + zz * zz) / (r * r / 2.0f);
					K = powf(m_fDiShu,-ra);
					weight = int(m_fOppositeHeightValue * sign * K);
				}

				(*pHeight) = pOutDoorSpaceMgr->GetGroundHeight((float)x,(float)z) + weight;
				continue;
			}
			//     weight = (int)(K * fAlpha);
			(*pHeight) += weight;
		}
	}
	m_pHeightMap->ComputeNormal(nStarX,nEndX,nStarZ,nEndZ); // 计算法向量
	{
		vecRegion.clear();
		D3DXVECTOR2 SelA(A.x - 100 , A.y - 100),SelC(C.x+ 100,C.y +100);
		pOutDoorSpaceMgr->SelectRegion(&vecRegion,SelA,SelC,TRUE);

		A = D3DXVECTOR2((float)m_nHegihtMapStartIndex_X * SCENESECTIONWIDTH,(float)m_nHegihtMapStartIndex_Z * SCENESECTIONWIDTH);
		C = D3DXVECTOR2(A.x + (m_pHeightMap->GetMapWidth())*100.0F,A.y + (m_pHeightMap->GetMapHeight())*100.0F);
		SetBlockHeight(BLOCK_TYPE_REGION,&vecRegion,m_pHeightMap,A,C,pOutDoorSpaceMgr);
	}
Exit0:
	return S_OK;
}

HRESULT KG3DTerrainBlockModifier::FlatTerrain(KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr)
{
	float K =  min(1.0F,m_fAlpha*0.01F);
	int fBrushSize = m_Brush.wOuterHeightBrushSize * 100;
	int nBrushCenterX =(int) m_PickRayIntersection.x / 100;
	int nBrushCenterZ =(int) m_PickRayIntersection.z / 100;

	D3DXVECTOR2 P((float)(nBrushCenterX *100),(float)(nBrushCenterZ * 100));
	FRECT rect(P,(float)fBrushSize);
	D3DXVECTOR2 A,C;
	rect.ToPosition(&A,&C);
	vector<POINT>vecRegion;
	pOutDoorSpaceMgr->SelectRegion(&vecRegion,A,C,TRUE);
	int nSize = (int)vecRegion.size();
	if( nSize <= 0)
		return S_OK;
	KG_PROCESS_ERROR(m_pHeightMap);
	int nXS = vecRegion[0].x * TERRIANREGIONCELL;
	int nZS = vecRegion[0].y * TERRIANREGIONCELL;

	nSize = (int)m_Brush.wOuterHeightBrushSize/2;

	int nHeighMap_X = m_nHegihtMapStartIndex_X * 8 * TERRIANREGIONCELL;
	int nHeighMap_Z = m_nHegihtMapStartIndex_Z * 8 * TERRIANREGIONCELL;
	int nStarX = max(max(nXS,nBrushCenterX - nSize) - nHeighMap_X,0);
	int nStarZ = max(max(nZS,nBrushCenterZ - nSize) - nHeighMap_Z,0);

	int nEndX = min(m_pHeightMap->GetMapWidth()-1 ,nBrushCenterX + nSize - nHeighMap_X);
	int nEndZ = min(m_pHeightMap->GetMapHeight()-1 ,nBrushCenterZ + nSize - nHeighMap_Z);
		
	m_pHeightMap->SmoothMapHeight(nStarX,nEndX,nStarZ,nEndZ,K);
	m_pHeightMap->ComputeNormal(nStarX,nEndX,nStarZ,nEndZ); // 计算法向量

	{
		vecRegion.clear();
		D3DXVECTOR2 SelA(A.x - 100 , A.y - 100),SelC(C.x+ 100,C.y +100);
		pOutDoorSpaceMgr->SelectRegion(&vecRegion,SelA,SelC,TRUE);

		A = D3DXVECTOR2((float)m_nHegihtMapStartIndex_X * SCENESECTIONWIDTH,(float)m_nHegihtMapStartIndex_Z * SCENESECTIONWIDTH);
		C = D3DXVECTOR2(A.x + (m_pHeightMap->GetMapWidth())*100.0F,A.y + (m_pHeightMap->GetMapHeight())*100.0F);
		SetBlockHeight(BLOCK_TYPE_REGION,&vecRegion,m_pHeightMap,A,C,pOutDoorSpaceMgr);
	}
Exit0:
	return S_OK;
}

HRESULT KG3DTerrainBlockModifier::SetGroundHeight(KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr)
{
	int fBrushSize = m_Brush.wOuterHeightBrushSize * 100;
	int nBrushCenterX =(int) m_PickRayIntersection.x / 100;
	int nBrushCenterZ =(int) m_PickRayIntersection.z / 100;

	D3DXVECTOR2 P((float)(nBrushCenterX *100),(float)(nBrushCenterZ * 100));
	FRECT rect(P,(float)fBrushSize);
	D3DXVECTOR2 A,C;
	rect.ToPosition(&A,&C);
	vector<POINT>vecRegion;
	KG_PROCESS_ERROR(pOutDoorSpaceMgr);
	pOutDoorSpaceMgr->SelectRegion(&vecRegion,A,C,TRUE);
	int nSize = (int)vecRegion.size();
	if( nSize <= 0)
		return S_OK;

	KG_PROCESS_ERROR(m_pHeightMap);

	int nXS = vecRegion[0].x * TERRIANREGIONCELL;
	int nZS = vecRegion[0].y * TERRIANREGIONCELL;

	nSize = (int)m_Brush.wOuterHeightBrushSize/2;

	int nHeighMap_X = m_nHegihtMapStartIndex_X * 8 * TERRIANREGIONCELL;
	int nHeighMap_Z = m_nHegihtMapStartIndex_Z * 8 * TERRIANREGIONCELL;
	int nStarX = max(max(nXS,nBrushCenterX - nSize) - nHeighMap_X,0);
	int nStarZ = max(max(nZS,nBrushCenterZ - nSize) - nHeighMap_Z,0);

	int nEndX = min(m_pHeightMap->GetMapWidth()-1 ,nBrushCenterX + nSize - nHeighMap_X);
	int nEndZ = min(m_pHeightMap->GetMapHeight()-1 ,nBrushCenterZ + nSize - nHeighMap_Z);

	m_pHeightMap->SetHeight(nStarX,nEndX,nStarZ,nEndZ,m_fHeight);
	m_pHeightMap->ComputeNormal(nStarX,nEndX,nStarZ,nEndZ); // 计算法向量

	{
		vecRegion.clear();
		D3DXVECTOR2 SelA(A.x - 100 , A.y - 100),SelC(C.x+ 100,C.y +100);
		pOutDoorSpaceMgr->SelectRegion(&vecRegion,SelA,SelC,TRUE);

		A = D3DXVECTOR2((float)m_nHegihtMapStartIndex_X * SCENESECTIONWIDTH,(float)m_nHegihtMapStartIndex_Z * SCENESECTIONWIDTH);
		C = D3DXVECTOR2(A.x + (m_pHeightMap->GetMapWidth())*100.0F,A.y + (m_pHeightMap->GetMapHeight())*100.0F);
		SetBlockHeight(BLOCK_TYPE_REGION,&vecRegion,m_pHeightMap,A,C,pOutDoorSpaceMgr);
	}
Exit0:
	return S_OK;
}

HRESULT KG3DTerrainBlockModifier::GSFFitGround(KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr)
{
	HRESULT hr = E_FAIL;
	D3DXVECTOR2 A,C;
	vector<POINT> vecRegionIndex;
	KG_PROCESS_ERROR(pOutDoorSpaceMgr);

	pOutDoorSpaceMgr->ForceLoadAllTerrainBlock();

	pOutDoorSpaceMgr->GetTerrainRect(A,C);
	pOutDoorSpaceMgr->SelectRegion(&vecRegionIndex,A,C,TRUE);
	
	for (size_t i=0;i<vecRegionIndex.size();i++)
	{
		POINT Index = vecRegionIndex[i];
		KG3DSceneRegion* pRegion = pOutDoorSpaceMgr->GetRegion(Index.x,Index.y);
		if(pRegion)
		{
			KG3DTerrainBlock* pTerrainBlock = pRegion->m_lpTerrainBlock;
			if (pTerrainBlock)
			{
				pTerrainBlock->GSFFitGround(pOutDoorSpaceMgr);
				int uSetHandle;
				DWORD dwTypeData;
				KG3DTerrainBlockGrass* pGrassBlock = pTerrainBlock->GetTerrainGrassBlock();
				KG3DTerrainBlockGrass* pStoneBlock = pTerrainBlock->GetTerrainStoneBlock();
				KG3DTerrainBlockGrass* pFrutexBlock = pTerrainBlock->GetTerrainFrutexBlock();
				
				if(pGrassBlock)
				{
					dwTypeData = Terrain_Block_Grass;
					uSetHandle = pOutDoorSpaceMgr->m_uSetHandle[MAP_GRASS];
					SetObjectDataToTerrainBlock(BLOCK_TYPE_REGION,Index,dwTypeData,pGrassBlock,uSetHandle,pOutDoorSpaceMgr->GetSceneDataInterface());
				}
				if(pStoneBlock)
				{
					dwTypeData = Terrain_Block_Stone;
					uSetHandle = pOutDoorSpaceMgr->m_uSetHandle[MAP_STONE];
					SetObjectDataToTerrainBlock(BLOCK_TYPE_REGION,Index,dwTypeData,pStoneBlock,uSetHandle,pOutDoorSpaceMgr->GetSceneDataInterface());
				}
				if(pFrutexBlock)
				{
					dwTypeData = Terrain_Block_Frutex;
					uSetHandle = pOutDoorSpaceMgr->m_uSetHandle[MAP_FRUTEX];
					SetObjectDataToTerrainBlock(BLOCK_TYPE_REGION,Index,dwTypeData,pFrutexBlock,uSetHandle,pOutDoorSpaceMgr->GetSceneDataInterface());
				}
			}
		}
	}
	hr = S_OK;
Exit0:
	return hr;
}

HRESULT KG3DTerrainBlockModifier::PaintGroundTexture(KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr)
{
	int fBrushSize = m_Brush.wOuterHeightBrushSize * 100;
	//int nBrushCenterX =(int) m_PickRayIntersection.x / 100;
	//int nBrushCenterZ =(int) m_PickRayIntersection.z / 100;

	D3DXVECTOR2 P(m_PickRayIntersection.x ,m_PickRayIntersection.z);//((float)(nBrushCenterX *100),(float)(nBrushCenterZ * 100));
	FRECT rect(P,(float)fBrushSize);
	D3DXVECTOR2 A,C;
	rect.ToPosition(&A,&C);
	vector<KG3DSceneRegion*> vecRegion;
	vector<KG3DSceneSection*> vecSection;
	vector<KG3DSceneZone*>   vecZone;
	D3DXVECTOR2 SelA(A.x-100,A.y-100);
	D3DXVECTOR2 SelC(C.x+100,C.y+100);
	pOutDoorSpaceMgr->SelectRegion(&vecRegion,SelA,SelC);
	pOutDoorSpaceMgr->SelectSection(&vecSection,SelA,SelC);
	pOutDoorSpaceMgr->SelectZone(&vecZone,SelA,SelC);
	
	int nSize = (int)vecRegion.size();
	for (int i=0 ;i<nSize;i++)
	{
		KG3DTerrainBlock* pTerrainBlock = vecRegion[i]->m_lpTerrainBlock;
		if (pTerrainBlock)
		{
			pTerrainBlock->PaintTexture(A,C,m_byPaintTexIndex);
			pTerrainBlock->ModifyDetailTexture(pOutDoorSpaceMgr->GetSceneDataInterface());
			KG3DTerrainBlockGrass* pBlock = pTerrainBlock->GetTerrainGrassBlock();
			if(pBlock && pBlock->m_bAutoGrass)
			{
				POINT Index;
				Index.x = pTerrainBlock->m_nIndex_X;Index.y = pTerrainBlock->m_nIndex_Z;
				UINT uSetHandle = pOutDoorSpaceMgr->m_uSetHandle[MAP_GRASS];
				SetObjectDataToTerrainBlock(BLOCK_TYPE_REGION,Index,Terrain_Block_Grass,pBlock,uSetHandle,pOutDoorSpaceMgr->GetSceneDataInterface());
			}
		}
	}
	
	nSize = (int)vecSection.size();
	for (int i=0 ;i<nSize;i++)
	{
		KG3DTerrainBlock* pTerrainBlock = vecSection[i]->m_lpTerrainBlock;
		if (pTerrainBlock)
		{
			pTerrainBlock->PaintTexture(A,C,m_byPaintTexIndex);
			pTerrainBlock->ModifyDetailTexture(pOutDoorSpaceMgr->GetSceneDataInterface());
		}
	}
	nSize = (int)vecZone.size();
	for (int i=0 ;i<nSize;i++)
	{
		KG3DTerrainBlock* pTerrainBlock = vecZone[i]->m_lpTerrainBlock;
		if (pTerrainBlock)
		{
			pTerrainBlock->PaintTexture(A,C,m_byPaintTexIndex);
			pTerrainBlock->ModifyDetailTexture(pOutDoorSpaceMgr->GetSceneDataInterface());
		}
	}
	return S_OK;
}

HRESULT KG3DTerrainBlockModifier::SetPaintTexIndex(BYTE byIndex)
{
	m_byPaintTexIndex = byIndex;;
	return S_OK;
}

BYTE KG3DTerrainBlockModifier::GetPaintTexIndex()
{
	return m_byPaintTexIndex;
}
HRESULT KG3DTerrainBlockModifier::SetCurrentBrushMap(LPCTSTR pFileName)
{
	m_Brush.ChangeBrushMap(pFileName);
	return S_OK;
}

HRESULT KG3DTerrainBlockModifier::GetTBModifierTextureStride(int* pX,int* pY,DWORD dwType,KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr)
{
	KG_PROCESS_ERROR(dwType < 3);
	if(pOutDoorSpaceMgr)
	{
		KG3DTerrainGrassSet* pGrassSet = pOutDoorSpaceMgr->m_lpSetPointer[dwType];
		KG_PROCESS_ERROR(NULL != pGrassSet);

		(*pX) = pGrassSet->m_dwStrideX;
		(*pY) = pGrassSet->m_dwStrideY;
	}
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DTerrainBlockModifier::SetTBModifierTextureStride(int  nX,int  nY,DWORD dwType,KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr)
{
	//m_nStrideX = nX;
	//m_nStrideY = nY;
	KG_PROCESS_ERROR(dwType < 3);
	if(pOutDoorSpaceMgr)
	{
		KG3DTerrainGrassSet* pGrassSet = pOutDoorSpaceMgr->m_lpSetPointer[dwType];
		KG_PROCESS_ERROR(NULL != pGrassSet);

		pGrassSet->m_dwStrideX = nX;
		pGrassSet->m_dwStrideY = nY;

		KG3DMemoryFile* pFile = g_cMemoryFilePool.RequestResource();
		if(pFile)
		{
			DWORD dwLength = 0;
			pGrassSet->SaveToInformationBuffer(dwLength,pFile);

			KG3DSceneDataInterface* pData = pOutDoorSpaceMgr->GetSceneDataInterface();
			switch(dwType)
			{
			case MAP_GRASS:
				pData->Request_ModifyTerrainInformation(pOutDoorSpaceMgr->m_uSetHandle[MAP_GRASS],Terrain_Info_GrassSet,dwLength,pFile->GetBuffer());
				break;
			case MAP_STONE:
				pData->Request_ModifyTerrainInformation(pOutDoorSpaceMgr->m_uSetHandle[MAP_STONE],Terrain_Info_StoneSet,dwLength,pFile->GetBuffer());
				break;
			case MAP_FRUTEX:
				pData->Request_ModifyTerrainInformation(pOutDoorSpaceMgr->m_uSetHandle[MAP_FRUTEX],Terrain_Info_FrutexSet,dwLength,pFile->GetBuffer());
				break;
			}
			g_cMemoryFilePool.FreeResource(pFile);
			pFile = NULL;
		}
	}
	return S_OK;
Exit0:
	return E_FAIL;
}

int KG3DTerrainBlockModifier::GetGrassDensity()
{
	return m_nGrassDensity;
}
HRESULT KG3DTerrainBlockModifier::SetGrassDensity(int nDensity)
{
	m_nGrassDensity = nDensity;
	return S_OK;
}
HRESULT KG3DTerrainBlockModifier::BuildAutoGrass(KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr,BYTE byDetailTexIndex)
{
	KG_PROCESS_ERROR(pOutDoorSpaceMgr);
	{
		D3DXVECTOR2 A,C;
		pOutDoorSpaceMgr->ForceLoadAllTerrainBlock();
		pOutDoorSpaceMgr->GetSceneRect(&A,&C);
		vector<KG3DSceneRegion*> vecRegion;
		pOutDoorSpaceMgr->SelectRegion(&vecRegion,A,C);
		vector<KG3DSceneRegion*>::iterator i = vecRegion.begin();
		while(i != vecRegion.end())
		{
			KG3DTerrainBlock* pTerrainBlock = (*i)->m_lpTerrainBlock;
			if(pTerrainBlock)
			{
				int nWidth = BLOCK_CELL_COUNT+1;
				for (int nZ = 0;nZ <= BLOCK_CELL_COUNT;nZ++ )
				{
					for (int nX = 0;nX <= BLOCK_CELL_COUNT;nX++ )
					{
						int nIndex =  nZ * nWidth + nX;
						BYTE byTexture = pTerrainBlock->m_byteTextureIndex[nIndex];
						if(byTexture == byDetailTexIndex)
						{
							KG3DTerrainBlockGrass* pGrassBlock = pTerrainBlock->GetTerrainGrassBlock(TRUE);
							if(pGrassBlock)
							{
								if(rand() % g_cTerrainBlockModifier.GetGrassDensity() == 0)
								{
									BYTE byGrassTexIndex = (BYTE)GetRandomTextureIndex();
									BYTE byPattren = (BYTE)GetPatternIndexBySelection();
									float SWidth =max(5.0f, m_fWidthMax - m_fWidthMin);
									float fWidth = m_fWidthMin + rand() % 100 / 100.0F * SWidth;
									float SHeight = max(5.0f,m_fHeightMax - m_fHeightMin);
									float fHeight = g_cTerrainBlockModifier.m_fHeightMin + rand()%100/100.0F*SHeight;
									pGrassBlock->SetAutoGrassData(nX,nZ,TRUE,byDetailTexIndex,byGrassTexIndex,byPattren,fWidth,fHeight);
								}
								else
								{
									pGrassBlock->SetAutoGrassData(nX,nZ,FALSE,0,0,0,0,0);
								}
							}
						}
					}
				}
				KG3DTerrainBlockGrass* pGrassBlock = pTerrainBlock->GetTerrainGrassBlock();
				if(pGrassBlock && pGrassBlock->m_bAutoGrass)
				{
					POINT Index;
					Index.x = pTerrainBlock->m_nIndex_X;Index.y = pTerrainBlock->m_nIndex_Z;
					UINT uSetHandle = pOutDoorSpaceMgr->m_uSetHandle[MAP_GRASS];
					SetObjectDataToTerrainBlock(BLOCK_TYPE_REGION,Index,Terrain_Block_Grass,pGrassBlock,uSetHandle,pOutDoorSpaceMgr->GetSceneDataInterface());

				}
				
			}
			i++;
		}
	}
	
	return S_OK;
Exit0:
	return E_FAIL;
}
HRESULT KG3DTerrainBlockModifier::DeleteAutoGrass(KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr,BYTE byDetailTexIndex)
{
	KG_PROCESS_ERROR(pOutDoorSpaceMgr);
	{
		pOutDoorSpaceMgr->ForceLoadAllTerrainBlock();
		D3DXVECTOR2 A,C;
		pOutDoorSpaceMgr->GetSceneRect(&A,&C);
		vector<KG3DSceneRegion*> vecRegion;
		pOutDoorSpaceMgr->SelectRegion(&vecRegion,A,C);
		vector<KG3DSceneRegion*>::iterator i = vecRegion.begin();
		while(i != vecRegion.end())
		{
			KG3DTerrainBlock* pTerrainBlock = (*i)->m_lpTerrainBlock;
			if(pTerrainBlock)
			{
				KG3DTerrainBlockGrass* pGrassBlock = pTerrainBlock->GetTerrainGrassBlock();
				if(pGrassBlock && pGrassBlock->m_bAutoGrass)
				{
					BOOL bCreate=FALSE;
					BYTE TextureIndex,byGrassTexIndex,byPattren;
					float fWidth,fHeight;
					for (int nZ = 0 ; nZ < GRASS_NUM; nZ++)
					{
						for(int nX = 0; nX < GRASS_NUM;nX++)
						{	
							pGrassBlock->GetAutoGrassData(nX,nZ,bCreate,TextureIndex,byGrassTexIndex,byPattren,fWidth,fHeight);
							if(bCreate && TextureIndex == byDetailTexIndex )					
							{
								pGrassBlock->SetAutoGrassData(nX,nZ,FALSE,0,0,0,0,0);//刷除该标志位
							}

						}
					}
					{
						POINT Index;
						Index.x = pTerrainBlock->m_nIndex_X;Index.y = pTerrainBlock->m_nIndex_Z;
						UINT uSetHandle = pOutDoorSpaceMgr->m_uSetHandle[MAP_GRASS];
						SetObjectDataToTerrainBlock(BLOCK_TYPE_REGION,Index,Terrain_Block_Grass,pGrassBlock,uSetHandle,pOutDoorSpaceMgr->GetSceneDataInterface());
					}
				}		
			}

			i++;
		}
	}
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DTerrainBlockModifier::DeleteAutoGrassByBrush(KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr)
{
	int fBrushSize = m_Brush.wOuterHeightBrushSize * 100;
	int nBrushCenterX =(int) m_PickRayIntersection.x / 100;
	int nBrushCenterZ =(int) m_PickRayIntersection.z / 100;

	D3DXVECTOR2 P((float)(nBrushCenterX *100),(float)(nBrushCenterZ * 100));
	FRECT rect(P,(float)fBrushSize);
	D3DXVECTOR2 A,C;
	rect.ToPosition(&A,&C);
	vector<KG3DSceneRegion*> vecRegion;

	KG_PROCESS_ERROR(pOutDoorSpaceMgr);
	pOutDoorSpaceMgr->ForceLoadTerrainBlock(A,C);
	pOutDoorSpaceMgr->SelectRegion(&vecRegion,A,C);
	
	int nSize = (int)vecRegion.size();
	for (int i=0 ;i<nSize;i++)
	{
		KG3DTerrainBlock* pTerrainBlock = vecRegion[i]->m_lpTerrainBlock;
		if(pTerrainBlock)
		{
			KG3DTerrainBlockGrass* pGrassBlock = pTerrainBlock->GetTerrainGrassBlock();
			if(pGrassBlock && pGrassBlock->m_bAutoGrass)
			{
				POINT Index;
				Index.x = pTerrainBlock->m_nIndex_X;Index.y = pTerrainBlock->m_nIndex_Z;
				D3DXVECTOR2 vStartPos((float)(Index.x * 1600),(float)(Index.y * 1600));
				int nXStart = (int)(A.x - vStartPos.x)/100;
				int nZStart = (int)(A.y - vStartPos.y)/100;
				int nXEnd   = (int)(C.x - vStartPos.x)/100;
				int nZEnd   = (int)(C.y - vStartPos.y)/100;
				nXStart = max(0,nXStart);
				nZStart = max(0,nZStart);
				nXEnd   = min(16,nXEnd);
				nZEnd   = min(16,nZEnd); 
				BOOL bCreate=FALSE;
				BYTE TextureIndex,byGrassTexIndex,byPattren;
				float fWidth,fHeight;
				for (int nZ = nZStart ; nZ <= nZEnd; nZ++)
				{
					for(int nX = nXStart; nX <= nXEnd;nX++)
					{
						pGrassBlock->GetAutoGrassData(nX,nZ,bCreate,TextureIndex,byGrassTexIndex,byPattren,fWidth,fHeight);
						if(bCreate && TextureIndex == m_byPaintTexIndex)					
						{
							pGrassBlock->SetAutoGrassData(nX,nZ,FALSE,0,0,0,0,0);//刷除该标志位
						}
					}
				}
				
				UINT uSetHandle = pOutDoorSpaceMgr->m_uSetHandle[MAP_GRASS];
				SetObjectDataToTerrainBlock(BLOCK_TYPE_REGION,Index,Terrain_Block_Grass,pGrassBlock,uSetHandle,pOutDoorSpaceMgr->GetSceneDataInterface());
			}		
		}
	}
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DTerrainBlockModifier::AddAutoGrassByBrush(KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr)
{
	int fBrushSize = m_Brush.wOuterHeightBrushSize * 100;

	D3DXVECTOR2 BrushPos(m_PickRayIntersection.x ,m_PickRayIntersection.z);;
	FRECT rect(BrushPos,(float)fBrushSize);
	D3DXVECTOR2 A,C;
	rect.ToPosition(&A,&C);
	D3DXVECTOR2 SelA(A.x-100,A.y-100);
	D3DXVECTOR2 SelC(C.x+100,C.y+100);

	vector<KG3DSceneRegion*> vecRegion;

	KG_PROCESS_ERROR(pOutDoorSpaceMgr);
	pOutDoorSpaceMgr->SelectRegion(&vecRegion,SelA,SelC);
	
	int nSize = (int)vecRegion.size();
	for (int i=0 ;i<nSize;i++)
	{
		KG3DTerrainBlock* pTerrainBlock = vecRegion[i]->m_lpTerrainBlock;
		if(pTerrainBlock)
		{
			pTerrainBlock->AddAutoGrassByBrush(A,C);
			KG3DTerrainBlockGrass* pGrassBlock = pTerrainBlock->GetTerrainGrassBlock();
			if(pGrassBlock &&  pGrassBlock->m_bAutoGrass )
			{
				POINT Index;
				Index.x = pTerrainBlock->m_nIndex_X;Index.y = pTerrainBlock->m_nIndex_Z;
				UINT uSetHandle = pOutDoorSpaceMgr->m_uSetHandle[MAP_GRASS];
				SetObjectDataToTerrainBlock(BLOCK_TYPE_REGION,Index,Terrain_Block_Grass,pGrassBlock,uSetHandle,pOutDoorSpaceMgr->GetSceneDataInterface());
			}		
		}
	}
	return S_OK;
Exit0:	
	return E_FAIL;
}
void KG3DTerrainBlockModifier::SetInvertOperation(BOOL bSet)
{
	m_bInvertOperation = bSet;
}

HRESULT KG3DTerrainBlockModifier::ModifyWaterHeight(KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr)
{
	int fBrushSize = m_Brush.wOuterHeightBrushSize * 100;
	int nBrushCenterX =(int) m_PickRayIntersection.x / 100;
	int nBrushCenterZ =(int) m_PickRayIntersection.z / 100;

	D3DXVECTOR2 P((float)(nBrushCenterX *100),(float)(nBrushCenterZ * 100));
	FRECT rect(P,(float)fBrushSize);
	D3DXVECTOR2 A,C;
	rect.ToPosition(&A,&C);
	vector<KG3DSceneRegion*> vecRegion;
	vector<KG3DSceneSection*> vecSection;
	vector<KG3DSceneZone*>   vecZone;
	pOutDoorSpaceMgr->SelectRegion(&vecRegion,A,C);
	pOutDoorSpaceMgr->SelectSection(&vecSection,A,C);
	pOutDoorSpaceMgr->SelectZone(&vecZone,A,C);

	int nSize = (int)vecRegion.size();
	for (int i=0 ;i<nSize;i++)
	{
		KG3DTerrainBlock* pTerrainBlock = vecRegion[i]->m_lpTerrainBlock;
		if (pTerrainBlock)
		{
			pTerrainBlock->SetWaterHeight(A,C,m_fHeight,pOutDoorSpaceMgr->GetSceneDataInterface(),TRUE);
		}
	}

	nSize = (int)vecSection.size();
	for (int i=0 ;i<nSize;i++)
	{
		KG3DTerrainBlock* pTerrainBlock = vecSection[i]->m_lpTerrainBlock;
		if (pTerrainBlock)
		{
			pTerrainBlock->SetWaterHeight(A,C,m_fHeight,pOutDoorSpaceMgr->GetSceneDataInterface(),TRUE);
		}
	}
	nSize = (int)vecZone.size();
	for (int i=0 ;i<nSize;i++)
	{
		KG3DTerrainBlock* pTerrainBlock = vecZone[i]->m_lpTerrainBlock;
		if (pTerrainBlock)
		{
			pTerrainBlock->SetWaterHeight(A,C,m_fHeight,pOutDoorSpaceMgr->GetSceneDataInterface(),TRUE);
		}
	}
	return S_OK;
}

HRESULT KG3DTerrainBlockModifier::SetTerrainWaterDefaultHeight(KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr,float fHeight)
{
	KG_PROCESS_ERROR(pOutDoorSpaceMgr);
	{
		D3DXVECTOR2 A,C;
		pOutDoorSpaceMgr->GetSceneRect(&A,&C);
		pOutDoorSpaceMgr->ForceLoadAllTerrainBlock();
		vector<KG3DSceneRegion*> vecRegion;
		vector<KG3DSceneSection*> vecSection;
		vector<KG3DSceneZone*>   vecZone;
		pOutDoorSpaceMgr->SelectRegion(&vecRegion,A,C);
		pOutDoorSpaceMgr->SelectSection(&vecSection,A,C);
		pOutDoorSpaceMgr->SelectZone(&vecZone,A,C);
		int nSize = (int)vecRegion.size();
		for (int i=0 ;i<nSize;i++)
		{
			KG3DTerrainBlock* pTerrainBlock = vecRegion[i]->m_lpTerrainBlock;
			if (pTerrainBlock)
			{
				pTerrainBlock->SetWaterHeight(A,C,fHeight,pOutDoorSpaceMgr->GetSceneDataInterface(),TRUE);
			}
		}

		nSize = (int)vecSection.size();
		for (int i=0 ;i<nSize;i++)
		{
			KG3DTerrainBlock* pTerrainBlock = vecSection[i]->m_lpTerrainBlock;
			if (pTerrainBlock)
			{
				pTerrainBlock->SetWaterHeight(A,C,fHeight,pOutDoorSpaceMgr->GetSceneDataInterface(),TRUE);
			}
		}
		nSize = (int)vecZone.size();
		for (int i=0 ;i<nSize;i++)
		{
			KG3DTerrainBlock* pTerrainBlock = vecZone[i]->m_lpTerrainBlock;
			if (pTerrainBlock)
			{
				pTerrainBlock->SetWaterHeight(A,C,fHeight,pOutDoorSpaceMgr->GetSceneDataInterface(),TRUE);
			}
		}
	}

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DTerrainBlockModifier::FillHeightMap(D3DXVECTOR2 A,D3DXVECTOR2 C,KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr)
{	
	HRESULT hr =E_FAIL;
	D3DXVECTOR2 MapA,MapC;
	D3DXVECTOR2 StartPos,EndPos;
	KG_PROCESS_ERROR(pOutDoorSpaceMgr);
	KG_PROCESS_ERROR(m_pHeightMap);
	pOutDoorSpaceMgr->GetSceneRect(&MapA,&MapC);
	
	StartPos.x = max(A.x,MapA.x);
	StartPos.y = max(A.y,MapA.y);
	EndPos.x   = min(C.x,MapC.x);
	EndPos.y   = min(C.y,MapC.y);
	int nWidth,nHeight;
	int nStartX = GetSceneSectionIndex(StartPos.x);
	int nStartZ = GetSceneSectionIndex(StartPos.y);
	int nEndX   = GetSceneSectionIndex(EndPos.x)+1; 
	int nEndZ   = GetSceneSectionIndex(EndPos.y)+1;
	
	m_nHegihtMapStartIndex_X = nStartX;
	m_nHegihtMapStartIndex_Z = nStartZ;
	nWidth  =(nEndX - nStartX) * 128 +1; //nNumScetionX * 128 +1;
	nHeight =(nEndZ - nStartZ) * 128 +1;//nNumScetionZ * 128 +1;
	hr = m_pHeightMap->Init(nWidth,nHeight);
	KG_COM_PROCESS_ERROR(hr);
	hr = m_pHeightMap->SetHeightScale(0, 1);
	KG_COM_PROCESS_ERROR(hr);
	{
		float fStartX = (float)nStartX * SCENESECTIONWIDTH;
		float fStartZ = (float)nStartZ * SCENESECTIONWIDTH;
		D3DXVECTOR2 vA( fStartX + 100,fStartZ + 100);
		D3DXVECTOR2 vC((float)((nEndX) * SCENESECTIONWIDTH - 100) ,(float)((nEndZ) * SCENESECTIONWIDTH -100));
		pOutDoorSpaceMgr->ForceLoadTerrainBlock(vA,vC);
		vector<KG3DSceneRegion*> vecRegion;
		pOutDoorSpaceMgr->SelectRegion(&vecRegion,vA,vC);
		
		for (size_t i=0;i<vecRegion.size();i++)
		{
			KG3DSceneRegion* pRegion = vecRegion[i];
			if(pRegion && pRegion->m_lpTerrainBlock)
			{
				VFormat::_Faces_Normal_Texture0* pVertexs = pRegion->m_lpTerrainBlock->m_Vertexs;
				for (int nZ = 0;nZ <= BLOCK_CELL_COUNT;nZ++ )
				{
					for (int nX = 0;nX <= BLOCK_CELL_COUNT;nX++ )
					{
						int nIndex = nZ * (BLOCK_CELL_COUNT+1) + nX;

						int U = (int)(pVertexs[nIndex].p.x - fStartX) / 100;
						int V = (int)(pVertexs[nIndex].p.z - fStartZ) / 100;
						float* pH = m_pHeightMap->GetVertexPointer(U,V);
						*pH = pVertexs[nIndex].p.y;
						*(m_pHeightMap->GetVertexNormalPointer(U,V)) = pVertexs[nIndex].Normal;
					}
				}
			}
		}
		
	}
Exit0:
	return hr;
}

HRESULT KG3DTerrainBlockModifier::FillDetailTextureIndexMap(D3DXVECTOR2 A,D3DXVECTOR2 C,KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr)
{
	HRESULT hr =E_FAIL;
	D3DXVECTOR2 MapA,MapC;
	D3DXVECTOR2 StartPos,EndPos;
	KG_PROCESS_ERROR(pOutDoorSpaceMgr);
	pOutDoorSpaceMgr->GetSceneRect(&MapA,&MapC);

	StartPos.x = max(A.x,MapA.x);
	StartPos.y = max(A.y,MapA.y);
	EndPos.x   = min(C.x,MapC.x);
	EndPos.y   = min(C.y,MapC.y);
	int nWidth,nHeight;
	int nStartX = GetSceneSectionIndex(StartPos.x);
	int nStartZ = GetSceneSectionIndex(StartPos.y);
	int nEndX   = GetSceneSectionIndex(EndPos.x)+1; 
	int nEndZ   = GetSceneSectionIndex(EndPos.y)+1;

	m_nTextureIndexMapStartIndex_X = nStartX * 8 * TERRIANREGIONCELL;
	m_nTextureIndexMapStartIndex_Z = nStartZ * 8 * TERRIANREGIONCELL;

	nWidth  =(nEndX - nStartX) * 128 +1; //nNumScetionX * 128 +1;
	nHeight =(nEndZ - nStartZ) * 128 +1;//nNumScetionZ * 128 +1;

	m_nTextureIndexMapWidth = nWidth;
	m_nTextureIndexMapHeight = nHeight;

	SAFE_DELETE_ARRAY(m_pDetailTextureIndexMap);
	m_pDetailTextureIndexMap = new BYTE[nWidth * nHeight];
	KG_PROCESS_ERROR(m_pDetailTextureIndexMap);
	{
		float fStartX = (float)nStartX * SCENESECTIONWIDTH;
		float fStartZ = (float)nStartZ * SCENESECTIONWIDTH;
		D3DXVECTOR2 vA( fStartX + 100,fStartZ + 100);
		D3DXVECTOR2 vC((float)((nEndX) * SCENESECTIONWIDTH - 100) ,(float)((nEndZ) * SCENESECTIONWIDTH -100));
		pOutDoorSpaceMgr->ForceLoadTerrainBlock(vA,vC);
		vector<KG3DSceneRegion*> vecRegion;
		pOutDoorSpaceMgr->SelectRegion(&vecRegion,vA,vC);

		for (size_t i=0;i<vecRegion.size();i++)
		{
			KG3DSceneRegion* pRegion = vecRegion[i];
			if(pRegion && pRegion->m_lpTerrainBlock)
			{
				BYTE* pIndex = pRegion->m_lpTerrainBlock->m_byteTextureIndex;
				VFormat::_Faces_Normal_Texture0* pVertexs = pRegion->m_lpTerrainBlock->m_Vertexs;
				for (int nZ = 0;nZ <= BLOCK_CELL_COUNT;nZ++ )
				{
					for (int nX = 0;nX <= BLOCK_CELL_COUNT;nX++ )
					{
						int nIndex = nZ * (BLOCK_CELL_COUNT+1) + nX;

						int U = (int)(pVertexs[nIndex].p.x - fStartX) / 100;
						int V = (int)(pVertexs[nIndex].p.z - fStartZ) / 100;
						m_pDetailTextureIndexMap[V * nWidth + U] = pIndex[nIndex];
					}
				}

			}
		}

	}
	hr = S_OK;
Exit0:
	return hr;
}

HRESULT KG3DTerrainBlockModifier::UpdateTerrainBlockHeight(KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr)
{
	if (pOutDoorSpaceMgr && m_pHeightMap)
	{
		D3DXVECTOR2 A((float)m_nHegihtMapStartIndex_X* SCENESECTIONWIDTH,(float)m_nHegihtMapStartIndex_Z* SCENESECTIONWIDTH);
		D3DXVECTOR2 C(A.x +(m_pHeightMap->GetMapWidth())*100.0F,A.y + (m_pHeightMap->GetMapHeight())*100.0F);
		D3DXVECTOR2 vSelA(A.x + 100, A.y +100),vSelC(C.x - 200, C.y - 200);
		vector<POINT>vecZone,vecSection,vecRegion;
		pOutDoorSpaceMgr->SelectRegion(&vecRegion,vSelA,vSelC,TRUE);
		pOutDoorSpaceMgr->SelectSection(&vecSection,vSelA,vSelC,TRUE);
		pOutDoorSpaceMgr->SelectZone(&vecZone,vSelA,vSelC,TRUE);
		//Region要在Section前面,Section要在Zone前面，不然计算Terrain最高最低高度时,Section,Zone的最高最低高度值可能有误差
		SetBlockHeight(BLOCK_TYPE_REGION,&vecRegion,m_pHeightMap,A,C,pOutDoorSpaceMgr);
		SetBlockHeight(BLOCK_TYPE_SECTION,&vecSection,m_pHeightMap,A,C,pOutDoorSpaceMgr);
		SetBlockHeight(BLOCK_TYPE_ZONE,&vecZone,m_pHeightMap,A,C,pOutDoorSpaceMgr);

	}
	return S_OK;
}

HRESULT KG3DTerrainBlockModifier::UpdateDetailTextureIndex(KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr)
{
	if (pOutDoorSpaceMgr && m_pDetailTextureIndexMap)
	{
		D3DXVECTOR2 A((float)m_nTextureIndexMapStartIndex_X * 100,(float)m_nTextureIndexMapStartIndex_Z * 100);
		D3DXVECTOR2 C(A.x + m_nTextureIndexMapWidth * 100.0F,A.y + m_nTextureIndexMapHeight * 100.0F);
		D3DXVECTOR2 vSelA(A.x + 100, A.y +100),vSelC(C.x - 200, C.y - 200);
		vector<POINT>vecZone,vecSection,vecRegion;
		pOutDoorSpaceMgr->SelectRegion(&vecRegion,vSelA,vSelC,TRUE);
		pOutDoorSpaceMgr->SelectSection(&vecSection,vSelA,vSelC,TRUE);
		pOutDoorSpaceMgr->SelectZone(&vecZone,vSelA,vSelC,TRUE);

		SetTerrainBlockTextureIndex(BLOCK_TYPE_REGION,&vecRegion,A,C,pOutDoorSpaceMgr);
		SetTerrainBlockTextureIndex(BLOCK_TYPE_SECTION,&vecSection,A,C,pOutDoorSpaceMgr);
		SetTerrainBlockTextureIndex(BLOCK_TYPE_ZONE,&vecZone,A,C,pOutDoorSpaceMgr);

	}
	return S_OK;
}

HRESULT KG3DTerrainBlockModifier::SetTerrainBlockTextureIndex(int nLevel,vector<POINT>*pvecTerrainBlockIndex,D3DXVECTOR2 A,D3DXVECTOR2 C,KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr)
{
	for (size_t i=0;i<pvecTerrainBlockIndex->size();i++)
	{
		POINT Index = (*pvecTerrainBlockIndex)[i];
		SetTerrainBlockTextureIndex(nLevel,Index,A,C,pOutDoorSpaceMgr);
	}
	return S_OK;
}

HRESULT KG3DTerrainBlockModifier::SetTerrainBlockTextureIndex(int nLevel,POINT Index,D3DXVECTOR2 A,D3DXVECTOR2 C,KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr)
{
	float U_Len = C.x - A.x;
	float V_Len = C.y - A.y;

	float CellLength = 0;
	float BlockLength = 0;

	KGLOG_PROCESS_ERROR(m_pDetailTextureIndexMap);
	KGLOG_PROCESS_ERROR(pOutDoorSpaceMgr);
	KG3DSceneDataInterface* pDataInterface = pOutDoorSpaceMgr->GetSceneDataInterface();
	KGLOG_PROCESS_ERROR(pDataInterface);

	SceneBlockFindLength(nLevel,BlockLength,CellLength);
	KG3DTerrainBlock* pTerrainBlock = NULL;
	if (nLevel == BLOCK_TYPE_REGION)
	{
		KG3DSceneRegion* pRegion = pOutDoorSpaceMgr->GetRegion(Index.x,Index.y);
		if (pRegion)
			pTerrainBlock = pRegion->m_lpTerrainBlock;
	}
	else if (nLevel == BLOCK_TYPE_SECTION)
	{
		KG3DSceneSection* pSection = pOutDoorSpaceMgr->GetSection(Index.x,Index.y);
		if (pSection)
			pTerrainBlock = pSection->m_lpTerrainBlock;
	}
	else
	{
		KG3DSceneZone* pZone = pOutDoorSpaceMgr->GetZone(Index.x,Index.y);
		if (pZone)
			pTerrainBlock = pZone->m_lpTerrainBlock;
	}
	KGLOG_PROCESS_ERROR(pTerrainBlock);
	for (int nZ = 0;nZ <= BLOCK_CELL_COUNT;nZ++ )
	{
		for (int nX = 0;nX <= BLOCK_CELL_COUNT;nX++ )
		{
			int nIndex = nZ * (BLOCK_CELL_COUNT+1) + nX;

			float fPos_x = Index.x * BlockLength + nX * CellLength;
			float fPos_z = Index.y * BlockLength + nZ * CellLength;

			float U = (fPos_x - A.x) / U_Len;
			float V = (fPos_z - A.y) / V_Len;
			if((U>=0.0f && U <= 1.0f)  && (V>=0.0f && V<= 1.0f))
			{
				//H = Vertexs[nIndex].p.y = pHeightMap->GetHeight(U,V);
				float X = 0 ,Y = 0;
				float u = max(0.0F,min(1.0F,U));
				float v = max(0.0F,min(1.0F,V));

				float u_T = u * m_nTextureIndexMapWidth;
				float v_T = v * m_nTextureIndexMapHeight;

				X = 0 ;Y = 0;
				modf(u_T,&X);
				modf(v_T,&Y);

				int xx = min(m_nTextureIndexMapWidth-1,(int)X);
				int zz = min(m_nTextureIndexMapHeight-1,(int)Y);

				BYTE nTextureIndex = m_pDetailTextureIndexMap[zz * m_nTextureIndexMapWidth + xx];
				pTerrainBlock->m_byteTextureIndex[nIndex] = nTextureIndex;
			}
		}
	}
	pTerrainBlock->ModifyDetailTexture(pOutDoorSpaceMgr->GetSceneDataInterface());
	return S_OK;
Exit0:
	return E_FAIL;
}


HRESULT KG3DTerrainBlockModifier::SetBlockHeight(int nLevel,vector<POINT>*pvecTerrainBlockIndex,KG3DHeightMap* pHeightMap,D3DXVECTOR2 A,D3DXVECTOR2 C,KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr)
{
	for (size_t i=0;i<pvecTerrainBlockIndex->size();i++)
	{
		POINT Index = (*pvecTerrainBlockIndex)[i];
		SetBlockHeight(nLevel,Index,pHeightMap,A,C,pOutDoorSpaceMgr);
	}
	return S_OK;
}

HRESULT KG3DTerrainBlockModifier::SetBlockHeight(int nLevel,POINT Index,KG3DHeightMap* pHeightMap,D3DXVECTOR2 A,D3DXVECTOR2 C,KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr)
{
	float U_Len = C.x - A.x;
	float V_Len = C.y - A.y;

	VFormat::_Faces_Normal_Texture0 Vertexs[17*17];//顶点
	float CellLength = 0;
	float BlockLength = 0;
	float fHeightMin = 0,fHeightMax = 0;

	KGLOG_PROCESS_ERROR(pHeightMap);
	KGLOG_PROCESS_ERROR(pOutDoorSpaceMgr);
	KG3DSceneDataInterface* pDataInterface = pOutDoorSpaceMgr->GetSceneDataInterface();
	KGLOG_PROCESS_ERROR(pDataInterface);

	SceneBlockFindLength(nLevel,BlockLength,CellLength);
	int K = 0;
	for (int nZ = 0;nZ <= BLOCK_CELL_COUNT;nZ++ )
	{
		for (int nX = 0;nX <= BLOCK_CELL_COUNT;nX++ )
		{
			float H = 0;
			float fWaterHeight = 0;
			int nIndex = nZ * (BLOCK_CELL_COUNT+1) + nX;

			Vertexs[nIndex].p.x = Index.x * BlockLength + nX * CellLength;
			Vertexs[nIndex].p.z = Index.y * BlockLength + nZ * CellLength;

			float U = (Vertexs[nIndex].p.x - A.x) / U_Len;
			float V = (Vertexs[nIndex].p.z - A.y) / V_Len;
			if((U>=0.0f && U <= 1.0f)  && (V>=0.0f && V<= 1.0f))
			{
				H = Vertexs[nIndex].p.y = pHeightMap->GetHeight(U,V);
				Vertexs[nIndex].Normal = pHeightMap->GetNormal(U,V);	
			}
			else
			{
				H = Vertexs[nIndex].p.y = pOutDoorSpaceMgr->GetGroundHeight(Vertexs[nIndex].p.x,Vertexs[nIndex].p.z);
				pOutDoorSpaceMgr->GetRegionNormal(Vertexs[nIndex].Normal,Vertexs[nIndex].p.x,Vertexs[nIndex].p.z);
			}
			if(K==0)
			{
				fHeightMin = fHeightMax = H;
				K++;
			}
			else
			{

				fHeightMax = max(H,fHeightMax);
				fHeightMin = min(H,fHeightMin);
				if(SUCCEEDED(pOutDoorSpaceMgr->GetWaterHeight(fWaterHeight,Vertexs[nIndex].p.x,Vertexs[nIndex].p.z)))
				{
					fHeightMax = max(fWaterHeight,fHeightMax);
					fHeightMin = min(fWaterHeight,fHeightMin);
				}
			}
			
		}
	}
	//////////////////////////////////////////////////////////////////////////
	switch(nLevel)
	{
	case BLOCK_TYPE_REGION://什么都不用做
		break;
	case BLOCK_TYPE_SECTION:
		{
			KG3DSceneSection* pSection = pOutDoorSpaceMgr->GetSection(Index.x,Index.y);
			if (pSection)
			{
				//int nNumRegion = EM_SCENE_SECTION_REGION_COUNT;
				for(int i=0;i<EM_SCENE_SECTION_REGION_COUNT;i++)
				{
					KG3DSceneRegion* pRegion = pSection->m_lpRegions[i];
					if (pRegion && pRegion->m_lpTerrainBlock)
					{
						fHeightMax = max(fHeightMax,pRegion->m_lpTerrainBlock->m_fHeightMax);
						fHeightMin = min(fHeightMin,pRegion->m_lpTerrainBlock->m_fHeightMin);
					}
				}
			}
			break;
		}
	case BLOCK_TYPE_ZONE:
		{
			KG3DSceneZone* pZone = pOutDoorSpaceMgr->GetZone(Index.x,Index.y);
			if (pZone)
			{
				//int nNumSection = ;
				for(int i=0;i<EM_SCENE_SECTION_REGION_COUNT;i++)
				{
					KG3DSceneSection* pSection = pZone->m_lpSections[i];
					if (pSection && pSection->m_lpTerrainBlock)
					{
						fHeightMax = max(fHeightMax,pSection->m_lpTerrainBlock->m_fHeightMax);
						fHeightMin = min(fHeightMin,pSection->m_lpTerrainBlock->m_fHeightMin);
					}
				}
			}
			break;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	{
		DWORD dwVersion = 0;
		DWORD dwLength = 0;

		KG3DMemoryFile* pFile = g_cMemoryFilePool.RequestResource();
		if(pFile)
		{
			pFile->write(&dwVersion,sizeof(DWORD));
			pFile->write(&fHeightMax,sizeof(float));
			pFile->write(&fHeightMin,sizeof(float));
			pFile->write(Vertexs,sizeof(Vertexs));
			dwLength = pFile->GetLength();

			pDataInterface->Request_ModifyTerrainBlock(nLevel,Index,Terrain_Block_GroundHeight,dwLength,pFile->GetBuffer());
			g_cMemoryFilePool.FreeResource(pFile);
			pFile = NULL;
		}
	}

	return S_OK;
Exit0:
	return E_FAIL;
}

int KG3DTerrainBlockModifier::GetHegihtMapStartIndexX(int nLevel)
{
	int nHeighMap_X = m_nHegihtMapStartIndex_X;
	switch(nLevel)
	{
	case BLOCK_TYPE_REGION:
		nHeighMap_X = m_nHegihtMapStartIndex_X * 8;
		break;
	case BLOCK_TYPE_SECTION:
		nHeighMap_X = m_nHegihtMapStartIndex_X;
		break;
	case BLOCK_TYPE_ZONE:
		nHeighMap_X = m_nHegihtMapStartIndex_X / 8;
		break;
	}
	return nHeighMap_X;
}

int KG3DTerrainBlockModifier::GetHegihtMapStartIndexZ(int nLevel)
{
	int nHeighMap_Z = m_nHegihtMapStartIndex_Z;
	switch(nLevel)
	{
	case BLOCK_TYPE_REGION:
		nHeighMap_Z = m_nHegihtMapStartIndex_Z * 8;
		break;
	case BLOCK_TYPE_SECTION:
		nHeighMap_Z = m_nHegihtMapStartIndex_Z;
		break;
	case BLOCK_TYPE_ZONE:
		nHeighMap_Z = m_nHegihtMapStartIndex_Z / 8;
		break;
	}
	return nHeighMap_Z;
}

HRESULT KG3DTerrainBlockModifier::ComputeTerrainNormal(KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr)
{
	HRESULT hr = E_FAIL;
	KG_PROCESS_ERROR(pOutDoorSpaceMgr);
	{
		D3DXVECTOR2 A,C;
		pOutDoorSpaceMgr->GetSceneRect(&A,&C);
		pOutDoorSpaceMgr->ForceLoadAllTerrainBlock();
		D3DXVECTOR2 AA(A.x + 100, A.y +100),CC(C.x - 200, C.y - 200);
		hr = FillHeightMap(AA,CC,pOutDoorSpaceMgr);
		KG_COM_PROCESS_ERROR(hr);
		
		m_pHeightMap->ComputeNormal();			
		UpdateTerrainBlockHeight(pOutDoorSpaceMgr);

		hr = S_OK;
	}
Exit0:
	return hr;
}
HRESULT KG3DTerrainBlockModifier::CompositorTerrainTexture(KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr)
{
	HRESULT hr = E_FAIL;
	KG_PROCESS_ERROR(pOutDoorSpaceMgr);
	{
		D3DXVECTOR2 A,C;
		pOutDoorSpaceMgr->GetSceneRect(&A,&C);
		pOutDoorSpaceMgr->ForceLoadAllTerrainBlock();
		vector<KG3DSceneRegion*> vecRegion;
		vector<KG3DSceneSection*> vecSection;
		vector<KG3DSceneZone*>   vecZone;

		D3DXVECTOR2 SelA(A.x-100,A.y-100);
		D3DXVECTOR2 SelC(C.x+100,C.y+100);
		pOutDoorSpaceMgr->SelectRegion(&vecRegion,SelA,SelC);
		pOutDoorSpaceMgr->SelectSection(&vecSection,SelA,SelC);
		pOutDoorSpaceMgr->SelectZone(&vecZone,SelA,SelC);
			
		int nSize = (int)vecRegion.size();
		for (int i=0 ;i<nSize;i++)
		{
			KG3DTerrainBlock* pTerrainBlock = vecRegion[i]->m_lpTerrainBlock;
			if (pTerrainBlock)
				pTerrainBlock->ModifyDetailTexture(pOutDoorSpaceMgr->GetSceneDataInterface());
			
		}
		nSize = (int)vecSection.size();
		for (int i=0 ;i<nSize;i++)
		{
			KG3DTerrainBlock* pTerrainBlock = vecSection[i]->m_lpTerrainBlock;
			if (pTerrainBlock)
				pTerrainBlock->ModifyDetailTexture(pOutDoorSpaceMgr->GetSceneDataInterface());
		}
		nSize = (int)vecZone.size();
		for (int i=0 ;i<nSize;i++)
		{
			KG3DTerrainBlock* pTerrainBlock = vecZone[i]->m_lpTerrainBlock;
			if (pTerrainBlock)
				pTerrainBlock->ModifyDetailTexture(pOutDoorSpaceMgr->GetSceneDataInterface());
		}
		hr = S_OK;
	}
Exit0:
	return hr;
}
HRESULT KG3DTerrainBlockModifier::PaintTextureFromFilterMap(KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr,LPCSTR pFileName)
{
	HRESULT hr = E_FAIL;
	vector<KG3DSceneRegion*> vecRegion;
	vector<KG3DSceneSection*> vecSection;
	vector<KG3DSceneZone*>   vecZone;
	D3DSURFACE_DESC desc;
	D3DLOCKED_RECT  rect;
	D3DXVECTOR2 A,C;
	LPDIRECT3DTEXTURE9 pTexture = NULL;
	//KG3DTexture FilterMap;

	KG_PROCESS_ERROR(pOutDoorSpaceMgr);
	hr = D3DXCreateTextureFromFileEx(g_pd3dDevice, pFileName, D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &pTexture);
	//hr = g_cTextureTable.LoadResourceFromFile(pFileName,0,TEXOP_DISABLEDTX,(IKG3DResourceBase**)&pFilterMap);
	//FilterMap.LoadFromFile(pFileName,0,0);
	KG_COM_PROCESS_ERROR(hr);
	pOutDoorSpaceMgr->ForceLoadAllTerrainBlock();
	pOutDoorSpaceMgr->GetSceneRect(&A,&C);
	pOutDoorSpaceMgr->SelectRegion(&vecRegion,A,C);
	pOutDoorSpaceMgr->SelectSection(&vecSection,A,C);
	pOutDoorSpaceMgr->SelectZone(&vecZone,A,C);

	float fWidth = C.x - A.x;
	float fHeight= C.y - A.y;

	//pTexture =FilterMap.GetTexture();//->GetTexture();

	hr = pTexture->GetLevelDesc(0,&desc);
	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);
	hr = pTexture->LockRect(0,&rect,NULL,0);
	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

	int nSize = (int)vecRegion.size();
	for (int i=0 ;i<nSize;i++)
	{
		KG3DTerrainBlock* pTerrainBlock = vecRegion[i]->m_lpTerrainBlock;
		if (pTerrainBlock)
		{
			pTerrainBlock->PaintTextureByFilterMap(fWidth,fHeight,desc,(DWORD*)(rect.pBits),m_byPaintTexIndex);
			pTerrainBlock->ModifyDetailTexture(pOutDoorSpaceMgr->GetSceneDataInterface());
		}
	}

	nSize = (int)vecSection.size();
	for (int i=0 ;i<nSize;i++)
	{
		KG3DTerrainBlock* pTerrainBlock = vecSection[i]->m_lpTerrainBlock;
		if (pTerrainBlock)
		{
			pTerrainBlock->PaintTextureByFilterMap(fWidth,fHeight,desc,(DWORD*)(rect.pBits),m_byPaintTexIndex);
			pTerrainBlock->ModifyDetailTexture(pOutDoorSpaceMgr->GetSceneDataInterface());
		}
	}
	nSize = (int)vecZone.size();
	for (int i=0 ;i<nSize;i++)
	{
		KG3DTerrainBlock* pTerrainBlock = vecZone[i]->m_lpTerrainBlock;
		if (pTerrainBlock)
		{
			pTerrainBlock->PaintTextureByFilterMap(fWidth,fHeight,desc,(DWORD*)(rect.pBits),m_byPaintTexIndex);
			pTerrainBlock->ModifyDetailTexture(pOutDoorSpaceMgr->GetSceneDataInterface());
		}
	}
	hr = pTexture->UnlockRect(0);

Exit0:
	SAFE_RELEASE(pTexture);
	//VERIFY_RELEASE(pFilterMap);
	return hr;
}

HRESULT KG3DTerrainBlockModifier::ExportTexture(KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr,LPCSTR pFileName,DWORD dwType)
{
	HRESULT hr = E_FAIL;
	vector<KG3DSceneRegion*> vecRegion;
	D3DSURFACE_DESC desc;
	D3DLOCKED_RECT  rect;
	D3DXVECTOR2 A,C;
	LPDIRECT3DTEXTURE9 pSaveTexture = NULL;
	
	KG_PROCESS_ERROR(pOutDoorSpaceMgr);
		
	pOutDoorSpaceMgr->ForceLoadAllTerrainBlock();
	pOutDoorSpaceMgr->GetSceneRect(&A,&C);
	pOutDoorSpaceMgr->SelectRegion(&vecRegion,A,C);

	float fWidth = C.x - A.x;
	float fHeight= C.y - A.y;
	int nTextureWidth = (int)(fWidth / 100);
	int nTextureHeight= (int)(fHeight/ 100);
	D3DXCreateTexture(g_pd3dDevice,nTextureWidth,nTextureHeight,0,0,D3DFMT_A8R8G8B8,D3DPOOL_MANAGED,&pSaveTexture);

	hr = pSaveTexture->GetLevelDesc(0,&desc);
	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);
	hr = pSaveTexture->LockRect(0,&rect,NULL,0);
	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

	int nSize = (int)vecRegion.size();
	for (int i=0 ;i<nSize;i++)
	{
		KG3DTerrainBlock* pTerrainBlock = vecRegion[i]->m_lpTerrainBlock;
		if (pTerrainBlock)
		{
			switch(dwType)
			{
			case EXPORT_FilterMap:
				pTerrainBlock->PaintToFilterMap(fWidth,fHeight,desc,(DWORD*)(rect.pBits),m_byPaintTexIndex);
				break;
			case EXPORT_NormalMap:
				pTerrainBlock->PaintToNormalMap(fWidth,fHeight,desc,(DWORD*)(rect.pBits));
				break;
			}
		}
	}
	
	hr = pSaveTexture->UnlockRect(0);

	D3DXIMAGE_FILEFORMAT iff = D3DXIFF_TGA;
	TCHAR strFileName2[MAX_PATH];
	size_t nDotPos = strlen( pFileName ) - 1;
	while ( nDotPos >= 0 && pFileName[nDotPos] != '.' )
	{
		nDotPos--;
	}
	strcpy( strFileName2, pFileName + nDotPos + 1 );
	_strlwr( strFileName2 );

	if ( !strcmp( strFileName2, "jpg" ) )
		iff = D3DXIFF_JPG;
	else if ( !strcmp( strFileName2, "tga" ) )
		iff = D3DXIFF_TGA;
	else if ( !strcmp( strFileName2, "dds" ) )
		iff = D3DXIFF_DDS;
	else if ( !strcmp( strFileName2, "bmp" ) )
		iff = D3DXIFF_BMP;
	hr = D3DXSaveTextureToFile(pFileName, iff, pSaveTexture, NULL);
Exit0:
	VERIFY_RELEASE(pSaveTexture);
	if(FAILED(hr))
	{
		MessageBox(NULL,"Texture导出失败！","错误",MB_ICONWARNING);
	}
	return hr;
}

HRESULT KG3DTerrainBlockModifier::HandlePickOperation(KG3DScene& sceneCur, D3DXVECTOR3 vSrc, D3DXVECTOR3 vNormalizedDir )
{
#if 1
	static DWORD s_dwLastQuestTime = 0;
	enum{em_min_quest_gap = 30,};
	DWORD dwCurQuestTime = g_cGraphicsTool.GetNowTime();
	if(dwCurQuestTime - s_dwLastQuestTime < 30)
		return S_OK;

	s_dwLastQuestTime = dwCurQuestTime;
#endif

	//如果当前是在操作水面的话，需要对水进行碰撞
	DWORD dwIntersectionMask = EM_RAY_INTERSECTION_MASK_TERRAIN;
	if (m_dwState >= WATER_BEGIN && m_dwState <= WATER_END)
	{
		dwIntersectionMask |= EM_RAY_INTERSECTION_MASK_WATER;
	}
	
	const FLOAT fRange = 50000;

	FLOAT fRetTemp = FLT_MAX;
	//在这里把碰撞范围搞大了，但是这是编辑器专用的，所以无所谓，其它地方小心，会影响效率
	BOOL bRet = ::RayIntersectionTerrain(sceneCur, vSrc, vNormalizedDir, &fRetTemp, fRange, dwIntersectionMask);
	if (bRet)
	{
		D3DXVECTOR3 vRet = vSrc + vNormalizedDir * fRetTemp;
		this->SetPickRayIntersection(vRet);
	}
	return S_OK;
}

HRESULT KG3DTerrainBlockModifier::ClearTexture(KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr,BYTE byTexIndex)
{
	HRESULT hr = E_FAIL;
	vector<KG3DSceneRegion*> vecRegion;
	vector<KG3DSceneSection*> vecSection;
	vector<KG3DSceneZone*>   vecZone;
	D3DXVECTOR2 A,C;
	KG_PROCESS_ERROR(pOutDoorSpaceMgr);
	
	pOutDoorSpaceMgr->ForceLoadAllTerrainBlock();
	pOutDoorSpaceMgr->GetSceneRect(&A,&C);
	pOutDoorSpaceMgr->SelectRegion(&vecRegion,A,C);
	pOutDoorSpaceMgr->SelectSection(&vecSection,A,C);
	pOutDoorSpaceMgr->SelectZone(&vecZone,A,C);

	int nSize = (int)vecRegion.size();
	for (int i=0 ;i<nSize;i++)
	{
		KG3DTerrainBlock* pTerrainBlock = vecRegion[i]->m_lpTerrainBlock;
		if (pTerrainBlock)
		{
			pTerrainBlock->ClearTexture(byTexIndex);
			pTerrainBlock->ModifyDetailTexture(pOutDoorSpaceMgr->GetSceneDataInterface());
			KG3DTerrainBlockGrass* pGrassBlock = pTerrainBlock->GetTerrainGrassBlock();
			if( pGrassBlock)
			{
				POINT Index = {pTerrainBlock->m_nIndex_X,pTerrainBlock->m_nIndex_Z};
				UINT uSetHandle = pOutDoorSpaceMgr->m_uSetHandle[MAP_GRASS];
				SetObjectDataToTerrainBlock(BLOCK_TYPE_REGION,Index,Terrain_Block_Grass,pGrassBlock,uSetHandle,pOutDoorSpaceMgr->GetSceneDataInterface());
			}
		}
	}

	nSize = (int)vecSection.size();
	for (int i=0 ;i<nSize;i++)
	{
		KG3DTerrainBlock* pTerrainBlock = vecSection[i]->m_lpTerrainBlock;
		if (pTerrainBlock)
		{
			pTerrainBlock->ClearTexture(byTexIndex);
			pTerrainBlock->ModifyDetailTexture(pOutDoorSpaceMgr->GetSceneDataInterface());
		}
	}
	nSize = (int)vecZone.size();
	for (int i=0 ;i<nSize;i++)
	{
		KG3DTerrainBlock* pTerrainBlock = vecZone[i]->m_lpTerrainBlock;
		if (pTerrainBlock)
		{
			pTerrainBlock->ClearTexture(byTexIndex);
			pTerrainBlock->ModifyDetailTexture(pOutDoorSpaceMgr->GetSceneDataInterface());
		}
	}
Exit0:
	return hr;
}

HRESULT KG3DTerrainBlockModifier::RefreshTerrain(KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr)
{
	HRESULT hr = E_FAIL;
	KG_PROCESS_ERROR(pOutDoorSpaceMgr);
	{
		D3DXVECTOR2 A,C;
		pOutDoorSpaceMgr->GetSceneRect(&A,&C);
		pOutDoorSpaceMgr->ForceLoadAllTerrainBlock();
		D3DXVECTOR2 AA(A.x + 100, A.y +100),CC(C.x - 200, C.y - 200);
		hr = FillHeightMap(AA,CC,pOutDoorSpaceMgr);
		KG_COM_PROCESS_ERROR(hr);
		UpdateTerrainBlockHeight(pOutDoorSpaceMgr);
		hr = S_OK;
	}
Exit0:
	return hr;
}

