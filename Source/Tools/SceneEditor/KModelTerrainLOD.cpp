// KModelTerrainLOD.cpp: implementation of the KModelTerrainLOD class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sceneeditor.h"
#include "KModelTerrainLOD.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
extern LPDIRECT3DDEVICE9  g_pd3dDevice;
extern KGraphicsTool      g_cGraphicsTool;
extern D3DCAPS9           g_D3DCaps;
extern HWND               g_hBaseWnd;

extern TCHAR              g_Def_AppDirectory[256];
extern TCHAR              g_Def_ModelDirectory[256];
extern TCHAR              g_Def_TextureDirectory[256];
extern TCHAR              g_Def_ModelTextureDirectory[256];
extern IMusicApplication  *g_pSoundSys;

float KTerrainTree::GetHeightValue(float& x, float& z)
{
	float ret = 0.f;

	if(m_lpTerrainData)
	{
		m_lpTerrainData->GetMapHeight(&ret, x, z,1);
	}

	return ret;
}

void KTerrainTree::Render()
{
	g_pd3dDevice->SetStreamSource(0, m_Stream.m_pVertexBuffer, 0, m_Stream.m_SizeOfVertex);
	g_pd3dDevice->SetFVF(m_Stream.m_dwFVF);
	g_pd3dDevice->SetIndices(m_TerrainGroup.GetIndexBuffer());
	g_pd3dDevice->DrawIndexedPrimitive(m_TerrainGroup.m_RSDI.m_PrimitiveType, 0, 0, m_TerrainGroup.m_RSDI.m_NumOfVertices, 0, m_TerrainGroup.m_RSDI.m_NumOfPrimitives);
}

KModelTerrainLOD::KModelTerrainLOD(): KModel()
{
	m_lpTerrainData = S_OK;
	m_pKTT = new KTerrainTree;
	g_SetTerrainTree(m_pKTT);
}

HRESULT KModelTerrainLOD::SetTerrainData(KModelTerrain* lpTerrainData)
{
	m_lpTerrainData = lpTerrainData;

	m_pKTT->SetTerrainData(lpTerrainData);

	return S_OK;
}

HRESULT KModelTerrainLOD::LoadMesh(LPSTR pFileName)
{
	try
	{
	Terrain::TerrainTreeDesc ttdesc;

	Common::String sTTDName = g_Def_AppDirectory;
	ttdesc.SetName(sTTDName + "\\Default.ttd");
	ttdesc.ReadFile();
	m_pKTT->SetTerrainTreeDesc(ttdesc);
	g_GetRenderCore()->SetD3DDevice(g_pd3dDevice);
	m_pKTT->CreateD3DResources();

	float wx = 0.f, wy = 0.f, wz = 0.f;
	if(m_lpTerrainData)
	{
		D3DXVECTOR3 WorldCenter(0,0,0);
		//(m_lpTerrainData->m_HeightMapGround.m_BBox_A + m_lpTerrainData->m_HeightMapGround.m_BBox_B) * 0.5;
		wx = WorldCenter.x;
		wy = WorldCenter.y;
		wz = WorldCenter.z;
	}
	m_pKTT->CreateQuadTree(wx, wy, wz);

	D3DXVECTOR3 dxvPos, dxvX, dxvY, dxvZ;
	g_cGraphicsTool.GetCameraPos(&dxvPos);
	g_cGraphicsTool.GetCameraFront(&dxvZ);
	g_cGraphicsTool.GetCameraLeftUp(&dxvX, &dxvY);

	float Aspect = g_cGraphicsTool.m_lpCurCamera->Aspect;
	if(Aspect < 1)
	{
		Aspect = 1 / Aspect;
	}

	m_Camera.Init(dxvPos, dxvPos + dxvZ, dxvY, g_cGraphicsTool.m_lpCurCamera->zNear, g_cGraphicsTool.m_lpCurCamera->zFar, g_cGraphicsTool.m_lpCurCamera->Field * Aspect, Aspect);

	m_pKTT->SetDrawOutlineOnly(false);

	D3DXCreateTextureFromFile(g_pd3dDevice, sTTDName + "\\GroundMap\\TileGround01.Jpg", &m_TestTexture);

	}
	catch(Common::Exception e)
	{
		MessageBox(0, e.GetErrorMessage(), e.GetTitle(), MB_OK);
	}

	return S_OK;
}

HRESULT KModelTerrainLOD::Render()
{
	//g_cGraphicsTool.DrawGroundGrid();

	try
	{

	//g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	if(m_pKTT->CreateBTTree(m_Camera))
	{
		////m_lpTerrainData->GenerateGroundTexture();
		//m_lpTerrainData->m_TerrainRegion.GetTexture(m_lpTerrainData);
		//if(m_lpTerrainData)
		//{
		//	if(m_lpTerrainData->m_TerrainRegion.m_pTerrainTexture)
		//	{
		//		if(m_lpTerrainData->m_TerrainRegion.m_pTerrainTexture->m_lpTexture)
		//		{
		
		if(m_TestTexture)
		{
			g_pd3dDevice->SetTexture(0, m_TestTexture);
		}
		//		}
		//	}
		//}

		m_pKTT->Render();
	}

	}
	catch(Common::Exception e)
	{
		MessageBox(0, e.GetErrorMessage(), e.GetTitle(), MB_OK);
	}

	return S_OK;
}

HRESULT KModelTerrainLOD::FrameMove()
{
	try
	{

	D3DXVECTOR3 dxvPos, dxvX, dxvY, dxvZ;
	g_cGraphicsTool.GetCameraPos(&dxvPos);
	g_cGraphicsTool.GetCameraFront(&dxvZ);
	g_cGraphicsTool.GetCameraLeftUp(&dxvX, &dxvY);

	m_Camera.m_Location = dxvPos;
	m_Camera.m_AxisX = -dxvX;
	m_Camera.m_AxisY = dxvY;
	m_Camera.m_AxisZ = dxvZ;

	m_Camera.OnViewChanged();

	}
	catch(Common::Exception e)
	{
		MessageBox(0, e.GetErrorMessage(), e.GetTitle(), MB_OK);
	}

	return S_OK;
}

HRESULT KModelTerrainLOD::CleanUp()
{
	return S_OK;
}