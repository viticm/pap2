#include "..\StdAfx.h"
#include ".\kg3dterrainregiondata.h"
#include "../KG3DSceneEntity.h"
#include "./KG3DTerrain.h"
#include "./KG3DTerrainEx.h"
#include "../KG3DEngineManager.h"
#include "../KG3DTextureTable.h"
#include "..\kg3dgraphicstool.h"
#include "../KG3DRenderTargetTool.h"
#include "KGLog.h"
#include <assert.h>


#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define LIGHTMAP_SIZE 128
extern LPDIRECT3DDEVICE9 g_pd3dDevice;

namespace Terrain
{

KG3DTerrainRegionData::KG3DTerrainRegionData(void)
{
	m_bNeedReRenderTexture = FALSE;
	m_nNumPaintGround = rand()%10;
	m_lpGroundTextureMgr = NULL;
	m_lpRegionTexture = NULL;
	m_lpRegionTextureMgr = NULL;
	m_lpTexCoord = m_lpLightMapCoord = NULL;

	m_iIndex_X = 0;
	m_iIndex_Z = 0;

	m_lpHeightGround = NULL;

	m_lpLevel0SubsetVB = m_lpLevel1SubsetVB = m_lpLevel2SubsetVB = NULL;

	IsCorrect0[0] = IsCorrect0[1] = IsCorrect0[2] = IsCorrect0[3] = 
	IsCorrect1[0] = IsCorrect1[1] = IsCorrect1[2] = IsCorrect1[3] = false;

	m_byUpdateStartX = m_byUpdateStartZ = m_byUpdateEndX = m_byUpdateEndZ = 0;
	m_lpRegionLightMap = NULL;
}

KG3DTerrainRegionData*  KG3DTerrainRegionData::GetNeighborRegion_Left( KG3DTerrain* pTerrain)
{
	//return pTerrain->m_RegionManager.GetRegionByIndex(m_iIndex_X - 1,m_iIndex_Z);
	for(DWORD i=0;i<pTerrain->m_veclpVisibleRegion.size();i++)
	{
		LPREGION pRegion = pTerrain->m_veclpVisibleRegion[i];

		if ((pRegion->m_iIndex_X == m_iIndex_X - 1)&&
			(pRegion->m_iIndex_Z == m_iIndex_Z))
		{
			return pRegion;
		}
	}
	return NULL;
}

KG3DTerrainRegionData*  KG3DTerrainRegionData::GetNeighborRegion_Right( KG3DTerrain* pTerrain)
{
	return pTerrain->m_RegionManager.GetRegionByIndex(m_iIndex_X + 1,m_iIndex_Z);
	/*for(DWORD i=0;i<pTerrain->m_veclpVisibleRegion.size();i++)
	{
		LPREGION pRegion = pTerrain->m_veclpVisibleRegion[i];

		if ((pRegion->m_iIndex_X == m_iIndex_X + 1)&&
			(pRegion->m_iIndex_Z == m_iIndex_Z))
		{
			return pRegion;
		}
	}*/
	return NULL;
}

KG3DTerrainRegionData*  KG3DTerrainRegionData::GetNeighborRegion_Up( KG3DTerrain* pTerrain)
{
	return pTerrain->m_RegionManager.GetRegionByIndex(m_iIndex_X,m_iIndex_Z+1);
	/*for(DWORD i=0;i<pTerrain->m_veclpVisibleRegion.size();i++)
	{
		LPREGION pRegion = pTerrain->m_veclpVisibleRegion[i];

		if ((pRegion->m_iIndex_X == m_iIndex_X)&&
			(pRegion->m_iIndex_Z == m_iIndex_Z+1))
		{
			return pRegion;
		}
	}*/
	return NULL;
}

KG3DTerrainRegionData*  KG3DTerrainRegionData::GetNeighborRegion_Down( KG3DTerrain* pTerrain)
{
	return pTerrain->m_RegionManager.GetRegionByIndex(m_iIndex_X,m_iIndex_Z-1);
	/*for(DWORD i=0;i<pTerrain->m_veclpVisibleRegion.size();i++)
	{
		LPREGION pRegion = pTerrain->m_veclpVisibleRegion[i];

		if ((pRegion->m_iIndex_X == m_iIndex_X )&&
			(pRegion->m_iIndex_Z == m_iIndex_Z-1))
		{
			return pRegion;
		}
	}*/
	return NULL;
}

KG3DTerrainRegionData::~KG3DTerrainRegionData(void)
{
	SAFE_DELETE(m_lpHeightGround);
	SAFE_RELEASE(m_lpLevel0SubsetVB);
	SAFE_RELEASE(m_lpLevel1SubsetVB);
	SAFE_RELEASE(m_lpLevel2SubsetVB);
	SAFE_RELEASE(m_lpRegionLightMap);
	SAFE_RELEASE(m_lpRegionObjectShadowMap);
}

HRESULT KG3DTerrainRegionData::CreateRegion(Terrain::KG3DTerrain *pTerrain,const int IndexX,const int IndexZ,
											float *HeightData)
{
	HRESULT hr;
	int i,j;

	m_iIndex_X = IndexX;
	m_iIndex_Z = IndexZ;

	D3DXVECTOR3 A(IndexX * 1600.0F,0,IndexZ * 1600.0F);
	D3DXVECTOR3 B = A + D3DXVECTOR3(1600.0F,1000.0F,1600.0F);
	m_BBox.Clear();
	m_BBox.AddPosition(A);
	m_BBox.AddPosition(B);

	m_lpHeightGround = new KG3DTerrainHeightMap();
	m_lpHeightGround->Create(D3DXVECTOR2(m_BBox.A.x,m_BBox.A.z),(TERRIANREGIONCELL),
							(TERRIANREGIONCELL),HEIGHTMAPDATATYPE_FLOATHEIGHTNORMAL,TERRIANCELLWIDTH,
							0.0);

	m_CenterPos = D3DXVECTOR3(m_BBox.A.x + 800,0.0,m_BBox.A.z + 800);

	D3DXVECTOR3 t;
	for(i = 0;i < 17;++i)
	{
		for(j = 0;j < 17;++j)
		{ 
			t.y = HeightData[((IndexZ-pTerrain->m_iZTerrainStartRegionIndex) * 16 + i) * pTerrain->m_iTerrainWidth + 
				(IndexX-pTerrain->m_iXTerrainStartRegionIndex) * 16 + j];

			m_lpHeightGround->SetVertexPosition(t,j,i);
		}
	}

	if(m_lpLevel0SubsetVB)
		SAFE_RELEASE(m_lpLevel0SubsetVB);
	if(m_lpLevel1SubsetVB)
		SAFE_RELEASE(m_lpLevel1SubsetVB);
	if(m_lpLevel2SubsetVB)
		SAFE_RELEASE(m_lpLevel2SubsetVB);
	if(m_lpLightMapCoord)
		SAFE_RELEASE(m_lpLightMapCoord);
	if(m_lpTexCoord)
		SAFE_RELEASE(m_lpTexCoord);

	if(FAILED( hr = g_pd3dDevice->CreateVertexBuffer(TERRIANREGIONCELL * TERRIANREGIONCELL * 6 *
		sizeof(VFormat::_Faces_Normal_Diffuse_Texture0),0, 0,
		D3DPOOL_DEFAULT, &m_lpLevel0SubsetVB, NULL)))
	{
		return hr;  
	}

	if(FAILED( hr = g_pd3dDevice->CreateVertexBuffer(TERRIANREGIONCELL * TERRIANREGIONCELL * 6 *
		sizeof(VFormat::_Texture1),0, 0,
		D3DPOOL_DEFAULT, &m_lpTexCoord, NULL)))
	{
		return hr;  
	}

	if(FAILED( hr = g_pd3dDevice->CreateVertexBuffer(TERRIANREGIONCELL * TERRIANREGIONCELL * 6 *
		sizeof(VFormat::_Texture1),0, 0,
		D3DPOOL_DEFAULT, &m_lpLightMapCoord, NULL)))
	{
		return hr;  
	}

	if(FAILED( hr = g_pd3dDevice->CreateVertexBuffer((TERRIANREGIONCELL / 2 + 1 ) * (TERRIANREGIONCELL / 2 + 1) *
		sizeof(VFormat::_Faces_Normal_Diffuse_Texture1),
		0, D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_NORMAL| D3DFVF_TEX1,
		D3DPOOL_DEFAULT, &m_lpLevel1SubsetVB, NULL)))
	{
		return hr;  
	}

	if(FAILED( hr = g_pd3dDevice->CreateVertexBuffer((TERRIANREGIONCELL / 4 + 1 ) * (TERRIANREGIONCELL / 4 + 1) *
		sizeof(VFormat::_Faces_Normal_Diffuse_Texture1),
		0, D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_NORMAL| D3DFVF_TEX1,
		D3DPOOL_DEFAULT, &m_lpLevel2SubsetVB, NULL)))
	{
		return hr;  
	}

	CreateVertexBuffer(pTerrain);

	g_pd3dDevice->CreateTexture( LIGHTMAP_SIZE, LIGHTMAP_SIZE,
		0, D3DUSAGE_RENDERTARGET,
		D3DFMT_A8R8G8B8,
		D3DPOOL_DEFAULT,
		&m_lpRegionLightMap,
		NULL  );

	g_pd3dDevice->CreateTexture( LIGHTMAP_SIZE, LIGHTMAP_SIZE,
		0, D3DUSAGE_RENDERTARGET,
		D3DFMT_A8R8G8B8,
		D3DPOOL_DEFAULT,
		&m_lpRegionObjectShadowMap,
		NULL  );

	m_Water.CreateWaterUp(pTerrain,m_iIndex_X,m_iIndex_Z);
	return S_OK;
}

HRESULT KG3DTerrainRegionData::CalCorrectPointOffset(const Terrain::KG3DTerrain *pTerrain)
{
	int cellcount = 0;
	int i,j,k;
	WORD CellIndex,PointBase,PointOffset[2];

	{
		for(i = 0;i < (int)m_vecLevel0Subsets.size();++i)
		{
			for(j = 0;j < (int)m_vecLevel0Subsets[i].CellIndex.size();++j)
				cellcount++;
		}
	}

	for(i = 0;i < 4;++i)
	{
		for(j = 0;j < 8;++j)
		{
			for(k = 0;k < 2;++k)
			{
				CellIndex = pTerrain->Level0CorrectCellIndex[i][j][k];
				PointBase = (WORD)CalCorrectCellOffset(CellIndex);
				assert(PointBase !=  -1);
				assert(PointBase < 16 * 16);
				PointBase *= 6;

				switch(i)
				{
				case 0:
					if(k == 0)
					{
						PointOffset[0] = PointBase + 2;
						PointOffset[1] = PointBase + 3;
					}
					else
						PointOffset[0] = PointBase + 1;

					break;

				case 1:
					if(k == 0)
					{
						PointOffset[0] = PointBase + 2;
						PointOffset[1] = PointBase + 3;
					}
					else
						PointOffset[0] = PointBase + 4;

					break;

				case 2:
					if(k == 0)
						PointOffset[0] = PointBase + 4;
					else
					{
						PointOffset[0] = PointBase + 0;
						PointOffset[1] = PointBase + 5;
					} 

					break;

				case 3:
					if(k == 0)
						PointOffset[0] = PointBase + 1;
					else
					{
						PointOffset[0] = PointBase + 0;
						PointOffset[1] = PointBase + 5;
					}

					break;
				}

				if(i < 2)
				{
					if(k == 1)
						Level0CorrectPointOffset[i][j][0] = PointOffset[0];
					else
					{
						Level0CorrectPointOffset[i][j][1] = PointOffset[0];
						Level0CorrectPointOffset[i][j][2] = PointOffset[1];
					}
				}
				else
				{
					if(k == 0)
						Level0CorrectPointOffset[i][j][0] = PointOffset[0];
					else
					{
						Level0CorrectPointOffset[i][j][1] = PointOffset[0];
						Level0CorrectPointOffset[i][j][2] = PointOffset[1];
					}
				}
			}
		}
	}

	for(i = 0;i < 4;++i)
	{
		for(j = 0;j < 8;++j)
		{
			for(k = 0;k < 2;++k)
			{
				assert(Level0CorrectPointOffset[i][j][k] < 16 * 16 * 6);
			}
		}
	}

	return S_OK;
}

HRESULT KG3DTerrainRegionData::MakeObjectShadow(KG3DTerrainEx *pTerrain)
{
	D3DXPLANE plane = D3DXPLANE(0.0,1.0,0.0,0.0);
	D3DXVECTOR4 light = D3DXVECTOR4(0.0,-1.0,0.0,0.0);
	D3DXVECTOR3 objt,objs;
	D3DXQUATERNION  objr;
	D3DXMATRIXA16 saveworld,saveview,saveproj,shadowmat,viewmat,projmat,wvmat,
				  objtmat,objrmat,objsmat,lightmat;
	g_pd3dDevice->GetTransform(D3DTS_WORLD,&saveworld);
	g_pd3dDevice->GetTransform(D3DTS_VIEW,&saveview);
	g_pd3dDevice->GetTransform(D3DTS_PROJECTION,&saveproj);
	D3DXMatrixShadow(&shadowmat,&light,&plane);

//	g_pd3dDevice->SetTransform(D3DTS_WORLD,&shadowmat);

	AABBOX aabb;
	D3DXVECTOR3 viewpos,lookat,up;
	up = D3DXVECTOR3(0.0,0.0,1.0);
	float w,h,n,f;
	LPDIRECT3DSURFACE9 newtarget,newtarget2;
	m_lpRegionObjectShadowMap->GetSurfaceLevel(0,&newtarget);
	list<KG3DSceneEntity *>::iterator lpe = m_plistEntity.begin();
	KG3DSceneEntity *pe = NULL;


	LPDIRECT3DSURFACE9 savetarget,savedepth;
	g_pd3dDevice->GetRenderTarget(0,&savetarget);
	g_pd3dDevice->GetDepthStencilSurface(&savedepth);

	lookat = m_CenterPos;
	viewpos = lookat + D3DXVECTOR3(10.0,10000.0,10.0);

	w = 1600.0;
	h = 1600.0;
	n = 1.0;
	f = 100000.0;

	D3DXMatrixLookAtLH(&viewmat,&viewpos,&lookat,&up);
	D3DXMatrixOrthoLH(&projmat,w ,h,n,f);

	//g_pd3dDevice->SetRenderState(D3DRS_ZENABLE,FALSE);

	pTerrain->m_pRenderTargetTool->BeginNewRenderTarget(NULL,&viewmat,&projmat,newtarget);
	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 
		0xfffffff, 1.0f, 0 );
	g_pd3dDevice->BeginScene();

	while (lpe != m_plistEntity.end())
	{
		pe = *lpe;
		pe->GetTranslation(&objt);
		pe->GetRotation(&objr);
		pe->GetScaling(&objs);
		pe->GetBBox(&aabb);
			
		D3DXMatrixRotationQuaternion(&objrmat,&objr);
		D3DXMatrixTranslation(&objtmat,objt.x,objt.y,objt.z);
		D3DXMatrixScaling(&objsmat,objs.x,objs.y,objs.z);

	
		lightmat = /*objsmat * objrmat * objrmat **/ objtmat * viewmat * projmat;

		pTerrain->m_lpEffectTerrainShadow->SetTechnique("effectplaneshadow");

		pTerrain->m_lpEffectTerrainShadow->SetMatrix("sunworldViewProj",&lightmat);

		UINT pass;
		pTerrain->m_lpEffectTerrainShadow->Begin(&pass,0);

		for(UINT i = 0;i < pass;++i)
		{
			pTerrain->m_lpEffectTerrainShadow->BeginPass(i);

			pe->RenderDirect();
			
		//	RenderRegion(pTerrain);

			pTerrain->m_lpEffectTerrainShadow->EndPass();
		}
		pTerrain->m_lpEffectTerrainShadow->End();


		lpe++;
	}
	g_pd3dDevice->EndScene();
	pTerrain->m_pRenderTargetTool->EndNewRenderTarget(savetarget,savedepth);
	newtarget->Release();


	savetarget->Release();
	savedepth->Release();

//	g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE ,false);

	char name[255],name2[255];
	sprintf(name,g_Def_AppDirectory);
	sprintf(name2,"\\objectshadow\\region_%i_%i.tga",m_iIndex_Z,m_iIndex_X);
	strcat(name,name2);
	D3DXSaveTextureToFile(name, D3DXIFF_TGA ,m_lpRegionObjectShadowMap,NULL);
	

	return S_OK;
}

HRESULT KG3DTerrainRegionData::MakeObjectShadow2(KG3DTerrainEx *pTerrain)
{
	HRESULT hr;
	D3DXPLANE plane = D3DXPLANE(0.0,1.0,0.0,0.0);
	D3DXVECTOR4 light = D3DXVECTOR4(0.0,-1.0,0.0,0.0);
	D3DXVECTOR3 objt,objs;
	D3DXQUATERNION  objr;
	D3DXMATRIXA16 saveworld,saveview,saveproj,shadowmat,viewmat,projmat,wvmat,regionprojmat,
		objtmat,objrmat,objsmat,lightmat;
	g_pd3dDevice->GetTransform(D3DTS_WORLD,&saveworld);
	g_pd3dDevice->GetTransform(D3DTS_VIEW,&saveview);
	g_pd3dDevice->GetTransform(D3DTS_PROJECTION,&saveproj);
	D3DXMatrixShadow(&shadowmat,&light,&plane);

	//	g_pd3dDevice->SetTransform(D3DTS_WORLD,&shadowmat);

	AABBOX aabb;
	D3DXVECTOR3 viewpos,lookat,up;
	up = D3DXVECTOR3(0.0,0.0,1.0);
	float w,h,n,f;
	LPDIRECT3DSURFACE9 newtarget,newtarget2;
//	m_lpRegionObjectShadowMap->GetSurfaceLevel(0,&newtarget);
	list<KG3DSceneEntity *>::iterator lpe = m_plistEntity.begin();
	KG3DSceneEntity *pe = NULL;


	LPDIRECT3DSURFACE9 savetarget,savedepth;
	g_pd3dDevice->GetRenderTarget(0,&savetarget);
	g_pd3dDevice->GetDepthStencilSurface(&savedepth);

	lookat = m_CenterPos;
	viewpos = lookat + D3DXVECTOR3(10.0,10000.0,10.0);

	w = 1600.0;
	h = 1600.0;
	n = 1.0;
	f = 100000.0;

	D3DXMatrixLookAtLH(&viewmat,&viewpos,&lookat,&up);
	D3DXMatrixOrthoLH(&projmat,w ,h,n,f);

	//g_pd3dDevice->SetRenderState(D3DRS_ZENABLE,FALSE);

////////////////////////////////////make zmap///////////////////////////////////////

	g_pd3dDevice->BeginScene();
	pTerrain->m_lpTerrainZmap->GetSurfaceLevel(0,&newtarget);
	pTerrain->m_pRenderTargetTool->BeginNewRenderTarget(NULL,&viewmat,&projmat,newtarget);
	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 
		0xfffffff, 1.0f, 0 );
	
	
	while (lpe != m_plistEntity.end())
	{
		pe = *lpe;
		pe->GetTranslation(&objt);
		pe->GetRotation(&objr);
		pe->GetScaling(&objs);
		pe->GetBBox(&aabb);

		D3DXMatrixRotationQuaternion(&objrmat,&objr);
		D3DXMatrixTranslation(&objtmat,objt.x,objt.y,objt.z);
		D3DXMatrixScaling(&objsmat,objs.x,objs.y,objs.z);


		regionprojmat =  viewmat * projmat;
		lightmat = objtmat * testview * testproj;

		pTerrain->m_lpEffectTerrainShadow->SetTechnique("effectdepth");
		pTerrain->m_lpEffectTerrainShadow->SetMatrix("sunworldViewProj",&lightmat);

		UINT pass;
		pTerrain->m_lpEffectTerrainShadow->Begin(&pass,0);

		for(UINT i = 0;i < pass;++i)
		{
			pTerrain->m_lpEffectTerrainShadow->BeginPass(i);

			pe->RenderDirect();
		//	RenderRegion(pTerrain);
		//	pTerrain->RendertestVB();
			
			pTerrain->m_lpEffectTerrainShadow->EndPass();
		}
		
		pTerrain->m_lpEffectTerrainShadow->End();


		lpe++;
	}
	g_pd3dDevice->EndScene();
	pTerrain->m_pRenderTargetTool->EndNewRenderTarget(savetarget,savedepth);
	

/////////////////////////////make obj lightmap/////////////////////////////////////

	hr = g_pd3dDevice->BeginScene();
	m_lpRegionObjectShadowMap->GetSurfaceLevel(0,&newtarget2);
	pTerrain->m_pRenderTargetTool->BeginNewRenderTarget(NULL,NULL,NULL,newtarget2);
	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 
		0xfffffff, 1.0f, 0 );
	

	regionprojmat = /*objtmat **/ viewmat * projmat;
	lightmat = testview * testproj;

	hr = pTerrain->m_lpEffectTerrainShadow->SetTechnique("effectshadow");


	hr = pTerrain->m_lpEffectTerrainShadow->SetMatrix("sunworldViewProj2",&lightmat);
	hr = pTerrain->m_lpEffectTerrainShadow->SetMatrix("projectwvpmat",&regionprojmat);
	hr = pTerrain->m_lpEffectTerrainShadow->SetTexture("ShadowMap",pTerrain->m_lpTerrainZmap);

	UINT pass;
	hr = pTerrain->m_lpEffectTerrainShadow->Begin(&pass,0);

	for(UINT i = 0;i < pass;++i)
	{
		hr = pTerrain->m_lpEffectTerrainShadow->BeginPass(i);

		RenderRegion(pTerrain);
	//	pe->RenderDirect();
	//	pTerrain->RendertestVB();

		hr = pTerrain->m_lpEffectTerrainShadow->EndPass();
	}
	hr = pTerrain->m_lpEffectTerrainShadow->End();

	hr = g_pd3dDevice->EndScene();
	pTerrain->m_pRenderTargetTool->EndNewRenderTarget(savetarget,savedepth);
	newtarget2->Release();
	newtarget->Release();


//////////////////////////////////////////////////////////////////////////////////

	savetarget->Release();
	savedepth->Release();

	char name[255],name2[255];
	sprintf(name,g_Def_AppDirectory);
	sprintf(name2,"\\objectshadow\\region_%i_%i.tga",m_iIndex_Z,m_iIndex_X);
	strcat(name,name2);
	D3DXSaveTextureToFile(name, D3DXIFF_TGA ,m_lpRegionObjectShadowMap,NULL);

	sprintf(name,g_Def_AppDirectory);
	sprintf(name2,"\\objectshadow\\region_%i_%iZMap.tga",m_iIndex_Z,m_iIndex_X);
	strcat(name,name2);
	D3DXSaveTextureToFile(name, D3DXIFF_TGA ,pTerrain->m_lpTerrainZmap,NULL);

	return S_OK;
}

HRESULT KG3DTerrainRegionData::UpdateTerrainHeightMap(KG3DTerrainEx *pTerrain)
{
	int i,j;
	WORD indexX,indexZ,baseX,baseZ;
	float *phm = pTerrain->m_pfHeightMapData;
	assert(phm);

	indexX = (WORD)m_iIndex_X - pTerrain->m_RegionManager.m_nRegionStart_X;
	indexZ = (WORD)m_iIndex_Z - pTerrain->m_RegionManager.m_nRegionStart_Z;
	baseX = (WORD)indexX * 16;
	baseZ = (WORD)indexZ * 16;

	for(i = m_byUpdateStartZ;i <= m_byUpdateEndZ;++i)
	{
		for(j = m_byUpdateStartX;j <= m_byUpdateEndX;++j)
		{
			phm[(i + baseZ) * pTerrain->m_iTerrainWidth + baseX + j] = m_lpHeightGround->GetVertexHeight(j,i);
		}
	}


	return S_OK;
}

HRESULT KG3DTerrainRegionData::CalCorrectVertexLightValue()
{
	int i,j;
	float lit;
	D3DXVECTOR3 t;
	D3DXVECTOR3 SunVector = D3DXVECTOR3(1.0,1.0,1.0);
	D3DXVec3Normalize(&SunVector,&SunVector);

	for(i = 0;i < 17;++i)
	{
		for(j = 0;j < 17;++j)
		{
			D3DXVec3Normalize(&t,&m_VertexNormal[i * 17 + j]);
			//D3DXVec3Normalize(&t,&m_TriangleNormal[i * 16  * 2 + j]);
			lit = D3DXVec3Dot(&t,&SunVector);

			if(lit > 0.00001)
			{
				m_fVertexLightValue[i * 17 + j] = 1.0f;
			}
			else
				m_fVertexLightValue[i * 17 + j] = 0.7f + 0.3f * lit;
		}
	}

	return S_OK;
}

HRESULT KG3DTerrainRegionData::UpdateVertexNormal(const KG3DTerrainEx* pTerrain)
{
	int i,j;
	VFormat::_Faces_Normal_Diffuse_Texture0 *pv;
	VFormat::_Faces_Normal_Diffuse_Texture1 *pv1;
	int CellSize = 100;
	D3DXVECTOR3 A,B,C,D;
	int CellStart,cellindex,cellcount = 0;
	int xcell,zcell;
	float *pcolor = pTerrain->m_pfVertexLightValue;
	assert(pcolor);

	WORD indexX,indexZ,baseX,baseZ;

	indexX = (WORD)m_iIndex_X - pTerrain->m_RegionManager.m_nRegionStart_X;
	indexZ = (WORD)m_iIndex_Z - pTerrain->m_RegionManager.m_nRegionStart_Z;
	baseX = (WORD)indexX * 16;
	baseZ = (WORD)indexZ * 16;

	m_lpLevel0SubsetVB->Lock(0,TERRIANREGIONCELL * TERRIANREGIONCELL * 6 *sizeof(VFormat::_Faces_Normal_Diffuse_Texture0),
		(void **)&pv,0);

	for(i = 0;i < m_vecLevel0Subsets.size();++i)
	{
		for(j = 0;j < m_vecLevel0Subsets[i].CellIndex.size();++j)
		{
			cellindex = m_vecLevel0Subsets[i].CellIndex[j];
			xcell = cellindex % 16;
			zcell = cellindex / 16;
			
			A = pTerrain->m_pvecVertexNormal[(baseZ + zcell) * pTerrain->m_iTerrainWidth + baseX + xcell];
			B = pTerrain->m_pvecVertexNormal[(baseZ + zcell + 1) * pTerrain->m_iTerrainWidth + baseX + xcell];
			C = pTerrain->m_pvecVertexNormal[(baseZ + zcell + 1) * pTerrain->m_iTerrainWidth + baseX + xcell + 1];
			D = pTerrain->m_pvecVertexNormal[(baseZ + zcell) * pTerrain->m_iTerrainWidth + baseX + xcell + 1];

			//	A.diffuse = 0x00FFFFFF |(((DWORD)pCell->TextureWeight[0][0])<<24);
			pv[cellcount * 6 + 0].Normal = A;
			pv[cellcount * 6 + 1].Normal = B;
			pv[cellcount * 6 + 2].Normal = C;
			pv[cellcount * 6 + 3].Normal = C;
			pv[cellcount * 6 + 4].Normal = D;
			pv[cellcount * 6 + 5].Normal = A;

			cellcount++;
		}
	}

	m_lpLevel0SubsetVB->Unlock();	

	m_lpLevel1SubsetVB->Lock(0,(TERRIANREGIONCELL / 2 + 1) * (TERRIANREGIONCELL / 2 + 1) * sizeof(VFormat::_Faces_Normal_Diffuse_Texture1),
		(void **)&pv1,0);
	for(i = 0;i <= TERRIANREGIONCELL / 2;++i)
	{
		for(j = 0;j <= TERRIANREGIONCELL / 2;++j)
		{
			//index[0] = (i * 2 + m_iIndex_Z * 16) * pTerrain->m_iTerrainWidth + j * 2 + m_iIndex_X;
			A = pTerrain->m_pvecVertexNormal[i * 2 * 17 + j * 2];
			pv1[i * (TERRIANREGIONCELL / 2 + 1) + j].Normal = A;
		}	
	}
	m_lpLevel1SubsetVB->Unlock();
	
		
	m_lpLevel2SubsetVB->Lock(0,(TERRIANREGIONCELL / 4 + 1) * (TERRIANREGIONCELL / 4 + 1) * sizeof(VFormat::_Faces_Normal_Diffuse_Texture1),
			(void **)&pv1,0);
	for(i = 0;i <= TERRIANREGIONCELL / 4;++i)
	{
		for(j = 0;j <= TERRIANREGIONCELL / 4;++j)
		{
			A = pTerrain->m_pvecVertexNormal[i * 4 * 17 + j * 4];
			pv1[i * (TERRIANREGIONCELL / 4 + 1) + j].Normal = A;
				
		}
	}
	m_lpLevel2SubsetVB->Unlock();
	
	return S_OK;
}

HRESULT KG3DTerrainRegionData::CalculateTriangleNormal()
{
	int i,j;
	D3DXVECTOR3 vertex[4];
	D3DXVECTOR3 TempNormal,vec1,vec2;

	for(i = 0;i < 16;++i)
	{
		for(j = 0;j < 16;++j)
		{
			vertex[0].x = j * 100.0f;	vertex[0].z = i * 100.0f;	
			vertex[0].y = m_lpHeightGround->GetVertexHeight(j,i);//m_pfHeightMapData[i * m_iTerrainWidth + j];

			vertex[1].x = j * 100.0f;	vertex[1].z = (i + 1) * 100.0f;	
			vertex[1].y = m_lpHeightGround->GetVertexHeight(j,i + 1);//m_pfHeightMapData[(i + 1) * m_iTerrainWidth + j];

			vertex[2].x = (j + 1) * 100.0f;	vertex[2].z = (i + 1) * 100.0f;	
			vertex[2].y = m_lpHeightGround->GetVertexHeight(j + 1,i + 1);//m_pfHeightMapData[(i + 1) * m_iTerrainWidth + j + 1];

			vertex[3].x = (j + 1) * 100.0f;	vertex[3].z = i * 100.0f;	
			vertex[3].y = m_lpHeightGround->GetVertexHeight(j + 1,i);//m_pfHeightMapData[i * m_iTerrainWidth + j + 1];

			vec1 = vertex[1] - vertex[0];	
			vec2 = vertex[3] - vertex[1];
			D3DXVec3Normalize(&vec1,&vec1);		
			D3DXVec3Normalize(&vec2,&vec2);
			D3DXVec3Cross(&TempNormal,&vec1,&vec2);
			D3DXVec3Normalize(&TempNormal,&TempNormal);
			m_TriangleNormal[i * 16 * 2 + j + 0] = TempNormal;
			//m_pvecTriangleNormal[i * (m_iTerrainWidth - 1) * 2 + j * 2 + 0] = TempNormal;

			vec1 = vertex[2] - vertex[1];	
			vec2 = vertex[3] - vertex[2];
			D3DXVec3Normalize(&vec1,&vec1);		
			D3DXVec3Normalize(&vec2,&vec2);
			D3DXVec3Cross(&TempNormal,&vec1,&vec2);
			D3DXVec3Normalize(&TempNormal,&TempNormal);
			m_TriangleNormal[i * 16 * 2 + j + 1] = TempNormal;
			//m_pvecTriangleNormal[i * (m_iTerrainWidth - 1) * 2 + j * 2 + 1] = TempNormal;
		}
	}

	return S_OK;
}

HRESULT KG3DTerrainRegionData::CalculateVertexNormal()
{
	int i,j,k,index;
	D3DXVECTOR3 vertex[4];
	D3DXVECTOR3 TempNormal[6],SunNormal;

	for(i = 1;i < 16;++i)
	{
		for(j = 1;j < 16;++j)
		{
			index = i * 16 * 2 + j * 2;
			TempNormal[0] = m_TriangleNormal[index];

			index = i * 16 * 2 + (j - 1) * 2;
			TempNormal[1] = m_TriangleNormal[index];
			TempNormal[2] = m_TriangleNormal[index + 1];

			index = (i - 1) * 16 * 2 + j * 2;
			TempNormal[3] = m_TriangleNormal[index];
			TempNormal[4] = m_TriangleNormal[index + 1];

			index = (i - 1) * 16 * 2 + (j - 1) * 2;
			TempNormal[5] = m_TriangleNormal[index + 1];

			memset(&SunNormal,0,sizeof(D3DXVECTOR3));
			for(k = 0;k < 6;++k)
				SunNormal += TempNormal[k];

			m_VertexNormal[i * 17 + j] = SunNormal / 6.0;
		}
	}

	//left_bottom vertex normal
	m_VertexNormal[0] = m_TriangleNormal[0];
	//right_bottom vertex normal
	m_VertexNormal[17 - 1] = (m_TriangleNormal[16 * 2] + m_TriangleNormal[16 * 2 + 1]) / 2.0;

	// bottom vertex normal
	for(i = 0;i < 1;++i)
	{
		for(j = 1;j < 16;++j)
		{
			index = index = i * 16 * 2 + (j - 1) * 2;
			TempNormal[0] = m_TriangleNormal[index];
			TempNormal[1] = m_TriangleNormal[index + 1];

			index = index = i * 16 * 2 + j * 2;
			TempNormal[2] = m_TriangleNormal[index];

			memset(&SunNormal,0,sizeof(D3DXVECTOR3));
			for(k = 0;k < 3;++k)
				SunNormal += TempNormal[k];

			m_VertexNormal[i * 17 + j] = SunNormal / 3.0;
		}
	}

	//left vertex normal
	for(i = 1;i < 16;++i)
	{
		for(j = 0;j < 1;++j)
		{
			index = (i - 1) * 16 * 2 + j * 2;
			TempNormal[0] = m_TriangleNormal[index];
			TempNormal[1] = m_TriangleNormal[index + 1];

			index = i * 16 * 2 + j * 2;
			TempNormal[2] = m_TriangleNormal[index];

			memset(&SunNormal,0,sizeof(D3DXVECTOR3));
			for(k = 0;k < 3;++k)
				SunNormal += TempNormal[k];

			m_VertexNormal[i * 17 + j] = SunNormal / 3.0;
		}
	}

	//left_top vertex normal
	index = (17 - 2) * (17 - 1) * 2;
	m_VertexNormal[16 * 17] = (m_TriangleNormal[index] + m_TriangleNormal[index + 1]) / 2.0;

	//top vertex normal
	for(i = 16;i < 17;++i)
	{
		for(j = 1; j < 16;++j)
		{
			index = (i - 1) * 16 * 2 + (j - 1) * 2;
			TempNormal[0] = m_TriangleNormal[index + 1];

			index = (i - 1) * 16 * 2 + j * 2;
			TempNormal[1] = m_TriangleNormal[index];
			TempNormal[2] = m_TriangleNormal[index + 1];

			memset(&SunNormal,0,sizeof(D3DXVECTOR3));
			for(k = 0;k < 3;++k)
				SunNormal += TempNormal[k];

			m_VertexNormal[i * 17 + j] = SunNormal / 3.0;
		}
	}

	//right_top vertex normal
	index = (17 - 1) * (17 - 1) * 2;
	m_VertexNormal[17 * 16] = (m_TriangleNormal[index] + m_TriangleNormal[index + 1]) / 2.0;

	//right vertex normal
	for(i = 1;i < 16;++i)
	{
		index = (i - 1) * 16 * 2 + 16 * 2;
		TempNormal[0] = m_TriangleNormal[index + 1];

		index = i * 16 * 2 + 16 * 2;
		TempNormal[1] = m_TriangleNormal[index];
		TempNormal[2] = m_TriangleNormal[index + 1];

		memset(&SunNormal,0,sizeof(D3DXVECTOR3));
		for(k = 0;k < 3;++k)
			SunNormal += TempNormal[k];

		m_VertexNormal[i * 17 + 16] = SunNormal / 3.0;
	}

	return S_OK;
}

HRESULT KG3DTerrainRegionData::UpdateVertexColor(const KG3DTerrainEx *pTerrain)
{
	int i,j;
	VFormat::_Faces_Normal_Diffuse_Texture0 *pv;
	VFormat::_Faces_Normal_Diffuse_Texture1 *pv1;
	int CellSize = 100;
	BYTE A,B,C,D;
	int CellStart,cellindex,cellcount = 0;
	int xcell,zcell;
	float *pcolor = pTerrain->m_pfVertexLightValue;
	assert(pcolor);

	WORD indexX,indexZ,baseX,baseZ;

	indexX = m_iIndex_X - pTerrain->m_RegionManager.m_nRegionStart_X;
	indexZ = m_iIndex_Z - pTerrain->m_RegionManager.m_nRegionStart_Z;
	baseX = indexX * 16;
	baseZ = indexZ * 16;

	m_lpLevel0SubsetVB->Lock(0,TERRIANREGIONCELL * TERRIANREGIONCELL * 6 *sizeof(VFormat::_Faces_Normal_Diffuse_Texture0),
		(void **)&pv,0);

	for(i = 0;i < m_vecLevel0Subsets.size();++i)
	{
		for(j = 0;j < m_vecLevel0Subsets[i].CellIndex.size();++j)
		{
			cellindex = m_vecLevel0Subsets[i].CellIndex[j];
			xcell = cellindex % 16;
			zcell = cellindex / 16;
			
			A = pcolor[(baseZ + zcell) * pTerrain->m_iTerrainWidth + baseX + xcell] * 255;//m_fVertexLightValue[zcell * 17 + xcell] * 255;
			B = pcolor[(baseZ + zcell + 1) * pTerrain->m_iTerrainWidth + baseX + xcell] * 255;//m_fVertexLightValue[(zcell + 1) * 17 + xcell] * 255;
			C = pcolor[(baseZ + zcell + 1) * pTerrain->m_iTerrainWidth + baseX + xcell + 1] * 255;//m_fVertexLightValue[(zcell + 1) * 17 + xcell + 1] * 255;
			D = pcolor[(baseZ + zcell) * pTerrain->m_iTerrainWidth + baseX + xcell + 1] * 255;//m_fVertexLightValue[zcell * 17 + xcell + 1] * 255;

			if(A == 0 || B == 0 || C == 0 || D == 0)
			{
				A ++;
			}

		//	A.diffuse = 0x00FFFFFF |(((DWORD)pCell->TextureWeight[0][0])<<24);
			pv[cellcount * 6 + 0].diffuse &= 0xFF000000;
			pv[cellcount * 6 + 0].diffuse |= (((DWORD)A)); 
			pv[cellcount * 6 + 0].diffuse |= (((DWORD)A << 8)); 
			pv[cellcount * 6 + 0].diffuse |= (((DWORD)A << 16)); 

			pv[cellcount * 6 + 1].diffuse &= 0xFF000000;
			pv[cellcount * 6 + 1].diffuse |= (((DWORD)B)); 
			pv[cellcount * 6 + 1].diffuse |= (((DWORD)B << 8)); 
			pv[cellcount * 6 + 1].diffuse |= (((DWORD)B << 16)); 

			pv[cellcount * 6 + 2].diffuse &= 0xFF000000;
			pv[cellcount * 6 + 2].diffuse |= (((DWORD)C)); 
			pv[cellcount * 6 + 2].diffuse |= (((DWORD)C << 8)); 
			pv[cellcount * 6 + 2].diffuse |= (((DWORD)C << 16)); 

			pv[cellcount * 6 + 3].diffuse &= 0xFF000000;
			pv[cellcount * 6 + 3].diffuse |= (((DWORD)C)); 
			pv[cellcount * 6 + 3].diffuse |= (((DWORD)C << 8)); 
			pv[cellcount * 6 + 3].diffuse |= (((DWORD)C << 16)); 

			pv[cellcount * 6 + 4].diffuse &= 0xFF000000;
			pv[cellcount * 6 + 4].diffuse |= (((DWORD)D)); 
			pv[cellcount * 6 + 4].diffuse |= (((DWORD)D << 8)); 
			pv[cellcount * 6 + 4].diffuse |= (((DWORD)D << 16)); 

			pv[cellcount * 6 + 5].diffuse &= 0xFF000000;
			pv[cellcount * 6 + 5].diffuse |= (((DWORD)A)); 
			pv[cellcount * 6 + 5].diffuse |= (((DWORD)A << 8)); 
			pv[cellcount * 6 + 5].diffuse |= (((DWORD)A << 16)); 

			cellcount++;
		}
	}

	m_lpLevel0SubsetVB->Unlock();



//	m_lpLevel1SubsetVB->Lock(0,(TERRIANREGIONCELL / 2 + 1) * (TERRIANREGIONCELL / 2 + 1) * sizeof(VFormat::_Faces_Normal_Diffuse_Texture1),
//		(void **)&pv1,0);
//	for(i = 0;i <= TERRIANREGIONCELL / 2;++i)
//	{
//		for(j = 0;j <= TERRIANREGIONCELL / 2;++j)
//		{
//			//index[0] = (i * 2 + m_iIndex_Z * 16) * pTerrain->m_iTerrainWidth + j * 2 + m_iIndex_X;
//			A = m_fVertexLightValue[i * 2 * 17 + j * 2] * 255;;
//			pv1[i * (TERRIANREGIONCELL / 2 + 1) + j].diffuse &= 0xFF000000;
//			pv1[i * (TERRIANREGIONCELL / 2 + 1) + j].diffuse |= (((DWORD)A)); 
//			pv1[i * (TERRIANREGIONCELL / 2 + 1) + j].diffuse |= (((DWORD)A << 8)); 
//			pv1[i * (TERRIANREGIONCELL / 2 + 1) + j].diffuse |= (((DWORD)A << 8)); 
//		}	
//	}
//	m_lpLevel1SubsetVB->Unlock();
//
//
//
//	m_lpLevel2SubsetVB->Lock(0,(TERRIANREGIONCELL / 4 + 1) * (TERRIANREGIONCELL / 4 + 1) * sizeof(VFormat::_Faces_Normal_Diffuse_Texture1),
//		(void **)&pv1,0);
//	for(i = 0;i <= TERRIANREGIONCELL / 4;++i)
//	{
//		for(j = 0;j <= TERRIANREGIONCELL / 4;++j)
//		{
//	//		pv[i * (TERRIANREGIONCELL / 4 + 1) + j].p.y = m_lpHeightGround->GetVertexHeight(i * 4 * 17 + j * 4); 
////			index[0] = (i * 4 + m_iIndex_Z * 16) * pTerrain->m_iTerrainWidth + j * 4 + m_iIndex_X;
//			A = m_fVertexLightValue[i * 4 * 17 + j * 4] * 255;
//			pv1[i * (TERRIANREGIONCELL / 4 + 1) + j].diffuse &= 0xFF000000;
//			pv1[i * (TERRIANREGIONCELL / 4 + 1) + j].diffuse |= (((DWORD)A)); 
//			pv1[i * (TERRIANREGIONCELL / 4 + 1) + j].diffuse |= (((DWORD)A << 8)); 
//			pv1[i * (TERRIANREGIONCELL / 4 + 1) + j].diffuse |= (((DWORD)A << 8)); 
//		}
//	}
//	m_lpLevel2SubsetVB->Unlock();

	return S_OK;
}

int KG3DTerrainRegionData::CalCorrectCellOffset(const int CellIndex)
{
	int i,j;
	int count = 0,c = 0;

	for(i = 0;i < (int)m_vecLevel0Subsets.size();++i)
	{
		for(j = 0;j < (int)m_vecLevel0Subsets[i].CellIndex.size();++j)
		{
			c++;
			if(m_vecLevel0Subsets[i].CellIndex[j] == CellIndex)
				return count;

			count++;
		}
	}
	assert(c > 0);

	return -1;
}

HRESULT KG3DTerrainRegionData::CalCorrectValue(const KG3DTerrain* pTerrain)
{
	int i,j;
	int index1,index2;
	float t1;

	for(i = 0;i < 4;++i)
	{
		for(j = 0;j < 8;++j)
		{	
			index1 = pTerrain->Level0CalCorrectPointIndex[i][j][0];
			index2 = pTerrain->Level0CalCorrectPointIndex[i][j][1];
			t1 = (m_lpHeightGround->GetVertexHeight(index1) + 
				m_lpHeightGround->GetVertexHeight(index2)) / 2;


			Level0CorrectPoint[i][j] = t1;
		}
	}
	for(i = 0;i < 4;++i)
	{
		for(j = 0;j < 4;++j)
		{	
			t1 = (m_lpHeightGround->GetVertexHeight(pTerrain->Level1CalCorrectPointIndex[i][j][0]) + 
				m_lpHeightGround->GetVertexHeight(pTerrain->Level1CalCorrectPointIndex[i][j][1])) / 2;


			Level1CorrectPoint[i][j] = t1;
		}
	}	

	return S_OK;
}

HRESULT KG3DTerrainRegionData::UpDateHeightMap(const KG3DTerrain* pTerrain)
{
	int i,j;
	D3DXVECTOR3 t;

	//for(i = 0;i < 17;++i)
	//{
	//	for(j = 0;j < 17;++j)
	//	{
	//		t.y = pTerrain->m_pfHeightMapData[(m_iIndex_Z * 16 + i) * pTerrain->m_iTerrainWidth + m_iIndex_X * 16 + j];
	//		m_lpHeightGround->SetVertexPosition(t,j,i);
	//	}
	//}

	return S_OK;
}

HRESULT KG3DTerrainRegionData::GetModifyRect(const int CenterX,const int CenterZ,const WORD BrushSize)
{
	m_nUpdateStartX =  CenterX - BrushSize/2 - m_iIndex_X*16;
	m_nUpdateStartZ =  CenterZ - BrushSize/2 - m_iIndex_Z*16;
	m_nUpdateEndX = CenterX + BrushSize/2 - m_iIndex_X*16;	
	m_nUpdateEndZ = CenterZ + BrushSize/2 - m_iIndex_Z*16;
	m_nUpdateStartX = max(0,m_nUpdateStartX);
	m_nUpdateStartZ = max(0,m_nUpdateStartZ);
	m_nUpdateEndX = min(16,m_nUpdateEndX);
	m_nUpdateEndZ = min(16,m_nUpdateEndZ);
	return S_OK;
}

HRESULT KG3DTerrainRegionData::HoistHeightMap(const int CenterX,const int CenterZ,const WORD BrushSize,float fAlpha)
{
	BYTE i,j;
	float dis,h,a,w;
	int indexx,indexz;
	D3DXVECTOR3 t;

//	GetModifyRect(CenterX,CenterZ,BrushSize);

	for(i = m_byUpdateStartZ;i <= m_byUpdateEndZ;++i)
	{
		for(j = m_byUpdateStartX;j <= m_byUpdateEndX;++j)
		{
			indexx = m_iIndex_X * 16 + j;
			indexz = m_iIndex_Z * 16 + i;
			dis = sqrtf((indexz - CenterZ) * (indexz - CenterZ) + (indexx - CenterX) * (indexx - CenterX));
			a = dis / 2 / sqrt((float)BrushSize / 2);
			w = exp(-a) * fAlpha;

			h = m_lpHeightGround->GetVertexHeight(i * 17 + j);
			h += w;
			t.y = h;
			m_lpHeightGround->SetVertexPosition(t,j,i);
		}
	}

//	CalculateTriangleNormal();
//	CalculateVertexNormal();
//	CalCorrectVertexLightValue();
//	UpdateVertexColor();

	return S_OK;
}

HRESULT KG3DTerrainRegionData::FallHeightMap(const int CenterX,const int CenterZ,const WORD BrushSize,float fAlpha)
{
	BYTE i,j;
	float dis,h,a,w;
	int indexx,indexz;
	D3DXVECTOR3 t;

//	GetModifyRect(CenterX,CenterZ,BrushSize);

	for(i = m_byUpdateStartZ;i <= m_byUpdateEndZ;++i)
	{
		for(j = m_byUpdateStartX;j <= m_byUpdateEndX;++j)
		{
			indexx = m_iIndex_X * 16 + j;
			indexz = m_iIndex_Z * 16 + i;
			dis = sqrtf((indexz - CenterZ) * (indexz - CenterZ) + (indexx - CenterX) * (indexx - CenterX));
			a = dis / 2 / sqrt((float)BrushSize / 2);
			w = exp(-a) * fAlpha;

			h = m_lpHeightGround->GetVertexHeight(i * 17 + j);
			h -= w;
			t.y = h;
			m_lpHeightGround->SetVertexPosition(t,j,i);
		}
	}

//	CalculateTriangleNormal();
//	CalculateVertexNormal();

	return S_OK;
}

HRESULT KG3DTerrainRegionData::UpDateWater()
{
	m_Water.UpdateWater(m_lpHeightGround);
	return S_OK;
}
HRESULT KG3DTerrainRegionData::UpDateHeightMap(const BYTE XStartVertex,const BYTE XEndVertex,
											   const BYTE ZStartVertex,const BYTE ZEndVertex,
											   const int CenterX,const int CenterZ,const WORD BrushSize)
{
	BYTE i,j;
	float dis,h,a,w;
	int indexx,indexz;
	D3DXVECTOR3 t;

	for(i = ZStartVertex;i <= ZEndVertex;++i)
	{
		for(j = XStartVertex;j <= XEndVertex;++j)
		{
			indexx = m_iIndex_X * 16 + j;
			indexz = m_iIndex_Z * 16 + i;
			dis = sqrtf((indexz - CenterZ) * (indexz - CenterZ) + (indexx - CenterX) * (indexx - CenterX));
			a = dis / 2 / sqrt((float)BrushSize / 2);
			w = exp(-a) * 10.0;

			h = m_lpHeightGround->GetVertexHeight(i * 17 + j);
			h += w;
			t.y = h;
			m_lpHeightGround->SetVertexPosition(t,j,i);
		}
	}

	return S_OK;
}

HRESULT KG3DTerrainRegionData::UpdateVertexBufferHeight()
{
	int i,j;
	VFormat::_Faces_Normal_Diffuse_Texture0 *pv;
	VFormat::_Faces_Normal_Diffuse_Texture1 *pv1;
	int CellSize = 100;
	float A,B,C,D;
	int CellStart,cellindex,cellcount = 0;
	int xcell,zcell;

	m_lpLevel0SubsetVB->Lock(0,TERRIANREGIONCELL * TERRIANREGIONCELL * 6 *sizeof(VFormat::_Faces_Normal_Diffuse_Texture0),
							(void **)&pv,0);
	//for(i = 0;i < TERRIANREGIONCELL;++i)
	//{
	//	for(j = 0;j < TERRIANREGIONCELL;++j)
	//	{
	//		CellStart = i * TERRIANREGIONCELL + j;
	//		D3DXVECTOR3 Pos(j*CellSize + m_BBox.A.x,0,i*CellSize + m_BBox.A.z);

	//		A = m_lpHeightGround->GetVertexHeight(i * 17 + j);
	//		B = m_lpHeightGround->GetVertexHeight((i + 1) * 17 + j);
	//		C = m_lpHeightGround->GetVertexHeight((i + 1) * 17 + j + 1);
	//		D = m_lpHeightGround->GetVertexHeight(i * 17 + j + 1);

	//		pv[CellStart * 6 + 0].p.y = A; 
	//		pv[CellStart * 6 + 1].p.y = B; 
	//		pv[CellStart * 6 + 2].p.y = C; 

	//		pv[CellStart * 6 + 3].p.y = C; 
	//		pv[CellStart * 6 + 4].p.y = D; 
	//		pv[CellStart * 6 + 5].p.y = A; 
	//	}
	//}

	for(i = 0;i < m_vecLevel0Subsets.size();++i)
	{
		for(j = 0;j < m_vecLevel0Subsets[i].CellIndex.size();++j)
		{
			cellindex = m_vecLevel0Subsets[i].CellIndex[j];
			xcell = cellindex % 16;
			zcell = cellindex / 16;
			
			A = m_lpHeightGround->GetVertexHeight(zcell * 17 + xcell);
			B = m_lpHeightGround->GetVertexHeight((zcell + 1) * 17 + xcell);
			C = m_lpHeightGround->GetVertexHeight((zcell + 1) * 17 + xcell + 1);
			D = m_lpHeightGround->GetVertexHeight(zcell * 17 + xcell + 1);

			pv[cellcount * 6 + 0].p.y = A; 
			pv[cellcount * 6 + 1].p.y = B; 
			pv[cellcount * 6 + 2].p.y = C; 

			pv[cellcount * 6 + 3].p.y = C; 
			pv[cellcount * 6 + 4].p.y = D; 
			pv[cellcount * 6 + 5].p.y = A; 

			cellcount++;
		}
	}

	m_lpLevel0SubsetVB->Unlock();



	m_lpLevel1SubsetVB->Lock(0,(TERRIANREGIONCELL / 2 + 1) * (TERRIANREGIONCELL / 2 + 1) * sizeof(VFormat::_Faces_Normal_Diffuse_Texture1),
		(void **)&pv1,0);
	for(i = 0;i <= TERRIANREGIONCELL / 2;++i)
	{
		for(j = 0;j <= TERRIANREGIONCELL / 2;++j)
		{
			pv1[i * (TERRIANREGIONCELL / 2 + 1) + j].p.y = m_lpHeightGround->GetVertexHeight(i * 2 * 17 + j * 2); 
		}
	}

	// update aabb
	D3DXVECTOR3 BBox_A,BBox_B;
	D3DXComputeBoundingBox((D3DXVECTOR3*)pv1,9 * 9,sizeof(VFormat::_Faces_Normal_Diffuse_Texture1),
							&BBox_A,&BBox_B);

	m_BBox.AddPosition(BBox_A);
	m_BBox.AddPosition(BBox_B);
	m_lpLevel1SubsetVB->Unlock();



	m_lpLevel2SubsetVB->Lock(0,(TERRIANREGIONCELL / 4 + 1) * (TERRIANREGIONCELL / 4 + 1) * sizeof(VFormat::_Faces_Normal_Diffuse_Texture1),
		(void **)&pv1,0);
	for(i = 0;i <= TERRIANREGIONCELL / 4;++i)
	{
		for(j = 0;j <= TERRIANREGIONCELL / 4;++j)
		{
			pv1[i * (TERRIANREGIONCELL / 4 + 1) + j].p.y = m_lpHeightGround->GetVertexHeight(i * 4 * 17 + j * 4); 
		}
	}
	m_lpLevel2SubsetVB->Unlock();


	return S_OK;
}

HRESULT KG3DTerrainRegionData::RenderRegionMakeShadow(KG3DTerrain *pTerrain)
{
	//g_pd3dDevice->SetTexture(0,NULL);
	//DWORD dwLight = 0;
	//g_pd3dDevice->GetRenderState(D3DRS_LIGHTING,&dwLight);
	//g_pd3dDevice->SetRenderState(D3DRS_LIGHTING,FALSE);

	//g_pd3dDevice->SetStreamSource(0, m_lpMakeShadowVB, 0, sizeof(VFormat::_Faces_Normal_Diffuse_Texture1));  
	//g_pd3dDevice->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_NORMAL| D3DFVF_TEX1);
	//g_pd3dDevice->SetIndices(pTerrain->m_lpLevel0Index[0]);
	//g_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,17 * 17,0,16 * 16 * 2);

	return S_OK;
}

HRESULT KG3DTerrainRegionData::RenderRegion(KG3DTerrain *pTerrain)
{
	//g_cGraphicsTool.DrawAABBox(&m_BBox,0xFF00FF00);
	g_pd3dDevice->SetTexture(0,NULL);
	DWORD dwLight = 0;
	g_pd3dDevice->GetRenderState(D3DRS_LIGHTING,&dwLight);
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING,FALSE);
	if(m_unLevel == 0)
	{
		if(m_vecLevel0Subsets.size()==0)
		{
			m_CellTextureManager.CreateRegionSubsetInfo(this);
			m_CellTextureManager.CreateRegionSubsetVertexBuffer(this);
		//	CalCorrectPointOffset(pTerrain);
		}
		for(size_t i=0;i<m_vecLevel0Subsets.size();i++)
		{
			_Subset* pSubset = &m_vecLevel0Subsets[i];
			DrawSubset(pSubset,pTerrain);
		}
	}
	else if(m_unLevel == 1)
	{
		if(m_lpRegionTextureMgr)
		{
			if((m_lpRegionTextureMgr->GetTexture(this)!=1)||(m_bNeedReRenderTexture))
			{
				RenderRegionTexture(pTerrain);
				m_bNeedReRenderTexture = FALSE;
			}

			if(m_lpRegionTexture)
			{
				g_pd3dDevice->SetTexture(0,m_lpRegionTexture->m_pTexture);
				g_pd3dDevice->SetTexture(1,NULL);
			}
			g_pd3dDevice->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);
			g_pd3dDevice->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
		}
		g_pd3dDevice->SetStreamSource(0, m_lpLevel1SubsetVB, 0, sizeof(VFormat::_Faces_Normal_Diffuse_Texture1));  
		g_pd3dDevice->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_NORMAL| D3DFVF_TEX1);
		g_pd3dDevice->SetIndices(pTerrain->m_lpLevel1Index[0]);
		g_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,9 * 9,0,8 * 8 * 2);
	}
	else
	{

		//if(m_lpRegionTextureMgr)
		{
			/*if((m_lpRegionTextureMgr->GetTexture(this)!=1)||(m_bNeedReRenderTexture))
			{
				RenderRegionTexture(pTerrain);
				m_bNeedReRenderTexture = FALSE;
			}
			if(m_lpRegionTexture)*/
			{
				g_pd3dDevice->SetTexture(0,NULL);
				g_pd3dDevice->SetTexture(1,NULL);
			}
		}
	/*	g_pd3dDevice->SetStreamSource(0, m_lpLevel2SubsetVB, 0, sizeof(VFormat::_Faces_Normal_Diffuse_Texture1));  
		g_pd3dDevice->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_NORMAL| D3DFVF_TEX1);
		g_pd3dDevice->SetIndices(pTerrain->m_lpLevel2Index[0]);
		g_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,5 * 5,0,4 * 4 * 2);
	*/}
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING,dwLight);

	return S_OK;
}

HRESULT KG3DTerrainRegionData::RenderRegionTexture(const KG3DTerrain *pTerrain)
{
	if(!m_lpRegionTexture)
		return S_OK;
	DWORD dwFog = 0;
	g_pd3dDevice->GetRenderState(D3DRS_FOGENABLE, &dwFog);
	g_pd3dDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);

	D3DXMATRIX Mat_View_Save;
	D3DXMATRIX Mat_Proj_Save;
	LPDIRECT3DSURFACE9 lpSurfaceDepSave = NULL;
	LPDIRECT3DSURFACE9 lpSurfaceSave = NULL;
	LPDIRECT3DSURFACE9 lpTarget = NULL;
	D3DVIEWPORT9 ViewPort,ViewPortSave;
	//////////////////////////////////////////////////////////////////////////
	//Save back buffer & transform & viewport
	g_pd3dDevice->GetRenderTarget(0,&lpSurfaceSave);
	g_pd3dDevice->GetDepthStencilSurface(&lpSurfaceDepSave);
	lpSurfaceSave->Release();
	lpSurfaceDepSave->Release();
	g_pd3dDevice->GetTransform( D3DTS_VIEW, &Mat_View_Save );
	g_pd3dDevice->GetTransform( D3DTS_PROJECTION, &Mat_Proj_Save );
	g_pd3dDevice->GetViewport(&ViewPortSave);
	//////////////////////////////////////////////////////////////////////////
	D3DSURFACE_DESC Desc;
	m_lpRegionTexture->m_pTexture->GetLevelDesc(0,&Desc);
	m_lpRegionTexture->m_pTexture->GetSurfaceLevel(0,&lpTarget);
	lpTarget->Release();

	ViewPort.X = 0;ViewPort.Y = 0;
	ViewPort.Width = Desc.Width;
	ViewPort.Height = Desc.Height;
	ViewPort.MinZ = 0;ViewPort.MaxZ = 1.0f;

	g_pd3dDevice->SetViewport(&ViewPort);

	g_pd3dDevice->SetRenderTarget(0,lpTarget);

	D3DXVECTOR3 Center = m_BBox.GetCenter();
	D3DXVECTOR3 Eye = Center + D3DXVECTOR3(0,10000,0);
	D3DXMATRIX  Mat_View,Mat_Proj;
	D3DXMatrixLookAtLH(&Mat_View,&Eye,&Center,&D3DXVECTOR3(0,0,1));
	D3DXMatrixOrthoLH(&Mat_Proj,1600,1600,1,20000);
	g_pd3dDevice->SetTransform( D3DTS_VIEW, &Mat_View );
	g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &Mat_Proj );
	//////////////////////////////////////////////////////////////////////////
	//reade began to render
//	g_pd3dDevice->BeginScene();
	g_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);

	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 
		0xFFFFFFFF, 1.0f, 0 );

	if(m_vecLevel0Subsets.size()==0)
	{
		m_CellTextureManager.CreateRegionSubsetInfo(this);
		m_CellTextureManager.CreateRegionSubsetVertexBuffer(this);
	}
	for(size_t i=0;i<m_vecLevel0Subsets.size();i++)
	{
		_Subset* pSubset = &m_vecLevel0Subsets[i];
		DrawSubset(pSubset,pTerrain);
	}

//	g_pd3dDevice->EndScene();
	g_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);

	//////////////////////////////////////////////////////////////////////////
	//Restore
	g_pd3dDevice->SetTransform( D3DTS_VIEW, &Mat_View_Save );
	g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &Mat_Proj_Save );
	g_pd3dDevice->SetRenderTarget(0,lpSurfaceSave);
	g_pd3dDevice->SetDepthStencilSurface(lpSurfaceDepSave);
	g_pd3dDevice->SetViewport(&ViewPortSave);

	g_pd3dDevice->SetRenderState(D3DRS_FOGENABLE, dwFog);

	return S_OK;
}

HRESULT KG3DTerrainRegionData::CreateVertexBuffer(const Terrain::KG3DTerrain *pTerrain)
{
	int i,j,count = 0;
	int CellSize = 100;
	VFormat::_Faces_Normal_Diffuse_Texture0 A,B,C,D;
	VFormat::_Faces_Normal_Diffuse_Texture0 *pVers;

	VFormat::_Texture1 AT,BT,CT,DT;
	VFormat::_Texture1 *pTexCoord;

	if (FAILED(m_lpLevel0SubsetVB->Lock(0,16 * 16 * 6 * sizeof(VFormat::_Faces_Normal_Diffuse_Texture0),
		(void **)&pVers,0)))
		return E_FAIL;

	for(i = 0;i < 16;++i)
	{
		for(j = 0;j < 16;++j)
		{
			D3DXVECTOR3 Pos(j*CellSize + m_BBox.A.x,0,i*CellSize + m_BBox.A.z);

			A.p = Pos;
			B.p = Pos + D3DXVECTOR3(0,0,(float)CellSize);
			C.p = Pos + D3DXVECTOR3((float)CellSize,0,(float)CellSize);
			D.p = Pos + D3DXVECTOR3((float)CellSize,0,0);


			m_lpHeightGround->GetPosition(&A.p,D3DXVECTOR2(A.p.x,A.p.z));
			m_lpHeightGround->GetPosition(&B.p,D3DXVECTOR2(B.p.x,B.p.z));
			m_lpHeightGround->GetPosition(&C.p,D3DXVECTOR2(C.p.x,C.p.z));
			m_lpHeightGround->GetPosition(&D.p,D3DXVECTOR2(D.p.x,D.p.z));

			A.Normal = B.Normal = C.Normal = D.Normal = D3DXVECTOR3(0,1,0);

			A.diffuse = 0xFFFFFFFF ;
			B.diffuse = 0xFFFFFFFF ;
			C.diffuse = 0xFFFFFFFF ;
			D.diffuse = 0xFFFFFFFF ;

			pVers[(i * 16 + j)*6   ]  = A;
			pVers[(i * 16 + j)*6+1] = B;
			pVers[(i * 16 + j)*6+2] = C;
			pVers[(i * 16 + j)*6+3] = C;
			pVers[(i * 16 + j)*6+4] = D;
			pVers[(i * 16 + j)*6+5] = A;
		}
	}

	if (FAILED(m_lpLevel0SubsetVB->Unlock()))
		return E_FAIL;

	if (FAILED(m_lpTexCoord->Lock(0,16 * 16 * 6 * sizeof(VFormat::_Texture1),
		(void **)&pTexCoord,0)))
		return E_FAIL;

	for(i = 0;i < 16;++i)
	{
		for(j = 0;j < 16;++j)
		{

			float U = (float)(j * 1.0F / 16 );
			float V = (float)(i * 1.0F /16 );
			float t = (float)(1.0f / 16.0f );

			AT.tu1 = U;   AT.tv1 = V;
			BT.tu1 = U;	  BT.tv1 = V + t;
			CT.tu1 = U+t; CT.tv1 = V+t;
			DT.tu1 = U + t;   DT.tv1 = V;

			pTexCoord[(i * 16 + j)*6  ] = AT;
			pTexCoord[(i * 16 + j)*6+1] = BT;
			pTexCoord[(i * 16 + j)*6+2] = CT;
			pTexCoord[(i * 16 + j)*6+3] = CT;
			pTexCoord[(i * 16 + j)*6+4] = DT;
			pTexCoord[(i * 16 + j)*6+5] = AT;
		}
	}

	if (FAILED(m_lpTexCoord->Unlock()))
		return E_FAIL;

	if (FAILED(m_lpLightMapCoord->Lock(0,16 * 16 * 6 * sizeof(VFormat::_Texture1),
		(void **)&pTexCoord,0)))
		return E_FAIL;

	for(i = 0;i < 16;++i)
	{
		for(j = 0;j < 16;++j)
		{

			float U = (float)(j / 16);
			float V = (float)(i /16);
			float t = (float)(1.0 / 16.0);
			/*float U = (float)((j + m_iIndex_X * 16) / pTerrain->m_iTerrainWidth);
			float V = (float)((i + m_iIndex_Z * 16) / pTerrain->m_iTerrainHeight);
			float t = (float)(1.0 / pTerrain->m_iTerrainWidth);*/

			AT.tu1 = U;   AT.tv1 = V;
			BT.tu1 = U;	 BT.tv1 = V + t;
			CT.tu1 = U+t; CT.tv1 = V+t;
			DT.tu1 = U + t;   DT.tv1 = V;

			pTexCoord[(i * 16 + j)*6  ] = AT;
			pTexCoord[(i * 16 + j)*6+1] = BT;
			pTexCoord[(i * 16 + j)*6+2] = CT;
			pTexCoord[(i * 16 + j)*6+3] = CT;
			pTexCoord[(i * 16 + j)*6+4] = DT;
			pTexCoord[(i * 16 + j)*6+5] = AT;
		}
	}

	if (FAILED(m_lpLightMapCoord->Unlock()))
		return E_FAIL;

	VFormat::_Faces_Normal_Diffuse_Texture1 A1,B1,C1,D1;
	VFormat::_Faces_Normal_Diffuse_Texture1 *pVers1;

	if (FAILED(m_lpLevel1SubsetVB->Lock(0,9 * 9 * sizeof(VFormat::_Faces_Normal_Diffuse_Texture1),
		(void **)&pVers1,0)))
		return E_FAIL;

	for(i = 0;i < 9;++i)
	{
		for(j = 0;j < 9;++j)
		{
			D3DXVECTOR3 Pos(j*2*CellSize + m_BBox.A.x,0,i*CellSize*2 + m_BBox.A.z);

			A1.p = Pos;


			m_lpHeightGround->GetPosition(&A.p,D3DXVECTOR2(A1.p.x,A1.p.z));

			A1.Normal = D3DXVECTOR3(0,1,0);

			A1.diffuse = 0xFFFFFFFF ;

			float U = j / 8.0F;
			float V = 1.0F - i / 8.0F;

			A1.tu1 = U;   A1.tv1 = V;

			pVers1[i * 9 + j]  = A1;
		}
	}

	//// cal aabb
	D3DXVECTOR3 BBox_A,BBox_B;
	D3DXComputeBoundingBox((D3DXVECTOR3*)pVers1,9 * 9,sizeof(VFormat::_Faces_Normal_Diffuse_Texture1),
							&BBox_A,&BBox_B);

	m_BBox.AddPosition(BBox_A);
	m_BBox.AddPosition(BBox_B);

	if (FAILED(m_lpLevel1SubsetVB->Unlock()))
		return E_FAIL;

	if (FAILED(m_lpLevel2SubsetVB->Lock(0,5 * 5 * sizeof(VFormat::_Faces_Normal_Diffuse_Texture1),
		(void **)&pVers1,0)))
		return E_FAIL;

	for(i = 0;i < 5;++i)
	{
		for(j = 0;j < 5;++j)
		{
			D3DXVECTOR3 Pos(j*4*CellSize + m_BBox.A.x,0,i*CellSize*4 + m_BBox.A.z);

			A1.p = Pos;

			m_lpHeightGround->GetPosition(&A.p,D3DXVECTOR2(A1.p.x,A1.p.z));

			A1.Normal = D3DXVECTOR3(0,1,0);

			A1.diffuse = 0xFFFFFFFF ;

			float U = j / 4.0F;
			float V = 1.0F - i / 4.0F;


			A1.tu1 = U;   A1.tv1 = V;

			pVers1[i * 5 + j]  = A1;
		}
	}

	if (FAILED(m_lpLevel2SubsetVB->Unlock()))
		return E_FAIL;

	//if (FAILED(m_lpMakeShadowVB->Lock(0,17 * 17 * sizeof(VFormat::_Faces_Normal_Diffuse_Texture1),
	//	(void **)&pVers1,0)))
	//	return E_FAIL;

	//for(i = 0;i < 17;++i)
	//{
	//	for(j = 0;j < 17;++j)
	//	{
	//		D3DXVECTOR3 Pos(j*CellSize + m_BBox.A.x,0,i*CellSize + m_BBox.A.z);

	//		A1.p = Pos;

	//		m_lpHeightGround->GetPosition(&A1.p,D3DXVECTOR2(A1.p.x,A1.p.z));

	//		A1.Normal = D3DXVECTOR3(0,1,0);

	//		A1.diffuse = 0xFFFFFFFF ;

	//		float U = (float)(m_iIndex_X * 16 + j) / pTerrain->m_iTerrainWidth;
	//		float V = (float)(m_iIndex_Z * 16 + i) / pTerrain->m_iTerrainHeight;


	//		A1.tu1 = U;   A1.tv1 = V;

	//		pVers1[i * 17 + j]  = A1;
	//	}
	//}

	//if(FAILED(m_lpMakeShadowVB->Unlock()))
	//	return E_FAIL;

	return S_OK;
}

HRESULT KG3DTerrainRegionData::LoadFromFile(LPSTR pFileName)
{
	if(!m_lpGroundTextureMgr)
		return E_FAIL;

	struct _TexFileHead
	{
		DWORD dwMask;
		DWORD Version;
		_TexFileHead()
		{
			dwMask = 0xFDFDFDFD;
			Version  = 0;
		}
	}TexFileHead;

	DWORD dwBlockEnd = 0xFFFFFFFF;
	FILE* pFile = fopen(pFileName,"rb");
	if(!pFile)
	{
		KGLogPrintf(KGLOG_ERR, "无法读取文件%s.\n",pFileName);
		return E_FAIL;
	}
	static _CellInfoFile CellInfoFile[16*16];

	fread(&TexFileHead,sizeof(TexFileHead),1,pFile);
	fread(&CellInfoFile,sizeof(_CellInfoFile),16*16,pFile);
	fread(&dwBlockEnd,sizeof(DWORD),1,pFile);

	fclose(pFile);

	for(int i=0;i<16*16;i++ )
	{
		m_Level0CellTextureInfo[i].TextureCount = (BYTE) CellInfoFile[i].TextureCount;

		m_Level0CellTextureInfo[i].TextureIndex[0] = (BYTE)
			(m_lpGroundTextureMgr->FindTextureByUID(CellInfoFile[i].TextureUID[0])+1);
		m_Level0CellTextureInfo[i].TextureIndex[1] = (BYTE)
			(m_lpGroundTextureMgr->FindTextureByUID(CellInfoFile[i].TextureUID[1])+1);

		 m_Level0CellTextureInfo[i].TextureWeight[0][0]= CellInfoFile[i].TextureWeight[0][0];
		 m_Level0CellTextureInfo[i].TextureWeight[0][1]= CellInfoFile[i].TextureWeight[0][1];
		 m_Level0CellTextureInfo[i].TextureWeight[0][2]= CellInfoFile[i].TextureWeight[0][2];
		 m_Level0CellTextureInfo[i].TextureWeight[0][3]= CellInfoFile[i].TextureWeight[0][3];
		 m_Level0CellTextureInfo[i].TextureWeight[1][0]= CellInfoFile[i].TextureWeight[1][0];
		 m_Level0CellTextureInfo[i].TextureWeight[1][1]= CellInfoFile[i].TextureWeight[1][1];
		 m_Level0CellTextureInfo[i].TextureWeight[1][2]= CellInfoFile[i].TextureWeight[1][2];
		 m_Level0CellTextureInfo[i].TextureWeight[1][3]= CellInfoFile[i].TextureWeight[1][3];
	}
	m_CellTextureManager.CreateRegionSubsetInfo(this);
	m_CellTextureManager.CreateRegionSubsetVertexBuffer(this);

	return S_OK;
}

HRESULT KG3DTerrainRegionData::LoadFromFile(_RegionSave* pRegionSave,FILE* pFile,DWORD Pos_Start,KG3DTerrain* pTerrain)
{
	return S_OK;
}

HRESULT KG3DTerrainRegionData::ReadGroundBlock(FILE* pFile,DWORD Pos_Start,DWORD Pos_Seek)
{
	fseek(pFile,Pos_Start + Pos_Seek,SEEK_SET);

	DWORD BlockEnd = 0xFFFFFFFF;
	int   NumVertices = 33*33;
	float Heights[33*33];
	DWORD TexturesIndex [33*33];
	DWORD TexturesWeight[33*33];
	DWORD Colors[33*33];
	DWORD VecticesColors[33*33];

	int Num = NumVertices;

	DWORD Version = 0;
	fread(&Version       ,sizeof(DWORD),1,pFile);

	fread(Heights        ,sizeof(float),Num,pFile);
	fread(Colors         ,sizeof(DWORD),Num,pFile);
	fread(TexturesIndex  ,sizeof(DWORD),Num,pFile);
	fread(TexturesWeight ,sizeof(DWORD),Num,pFile);
	fread(VecticesColors ,sizeof(DWORD),Num,pFile);

	fread(&BlockEnd,sizeof(DWORD),1,pFile);

	DWORD dwNumCell = 32;

	for(int X=0;X<=(int)dwNumCell;X++)
	{
		for(int Y=0;Y<=(int)dwNumCell;Y++)
		{
			int Index =  X * (dwNumCell+1) + Y;

			D3DXVECTOR3 Pos(0,Heights[Index],0);
			m_lpHeightGround->SetVertexPosition(Pos,X,Y);

			m_BBox.A.y = min(m_BBox.A.y,Heights[Index]);
			m_BBox.A.y = min(m_BBox.A.y,Heights[Index]);
			m_BBox.B.y = max(m_BBox.B.y,Heights[Index]);
			m_BBox.B.y = max(m_BBox.B.y,Heights[Index]);

			//m_GroundTextureInfo.m_lpVerticesInfo[Index].ColorPainted = Colors[Index];

			DWORD Index_1 =  TexturesIndex[Index] & 0x000000FF;
			DWORD Index_2 = (TexturesIndex[Index] & 0x0000FF00)>>8;
			DWORD Index_3 = (TexturesIndex[Index] & 0x00FF0000)>>16;
			DWORD Index_4 = (TexturesIndex[Index] & 0xFF000000)>>24;

			DWORD Weight_1 =  TexturesWeight[Index] & 0x000000FF;
			DWORD Weight_2 = (TexturesWeight[Index] & 0x0000FF00)>>8;
			DWORD Weight_3 = (TexturesWeight[Index] & 0x00FF0000)>>16;
			DWORD Weight_4 = (TexturesWeight[Index] & 0xFF000000)>>24;

			/*m_GroundTextureInfo.m_lpVerticesInfo[Index].TextureIndex[0] = (BYTE)Index_1;
			m_GroundTextureInfo.m_lpVerticesInfo[Index].TextureIndex[1] = (BYTE)Index_2;
			m_GroundTextureInfo.m_lpVerticesInfo[Index].TextureIndex[2] = (BYTE)Index_3;
			m_GroundTextureInfo.m_lpVerticesInfo[Index].TextureIndex[3] = (BYTE)Index_4;

			m_GroundTextureInfo.m_lpVerticesInfo[Index].TextureWeight[0] = (BYTE)Weight_1;
			m_GroundTextureInfo.m_lpVerticesInfo[Index].TextureWeight[1] = (BYTE)Weight_2;
			m_GroundTextureInfo.m_lpVerticesInfo[Index].TextureWeight[2] = (BYTE)Weight_3;
			m_GroundTextureInfo.m_lpVerticesInfo[Index].TextureWeight[3] = (BYTE)Weight_4;

			AddGroundTextureUsed(Index_1-1);
			AddGroundTextureUsed(Index_2-1);
			AddGroundTextureUsed(Index_3-1);
			AddGroundTextureUsed(Index_4-1);*/

			//pVers[Index].diffuse = VecticesColors[Index];
		}
	}

	//m_nNeedRefreshGroundTexture++;
	return S_OK;
}

HRESULT KG3DTerrainRegionData::ReadEntityBlock(FILE* pFile,DWORD Pos_Start,DWORD Pos_Seek)
{
	fseek(pFile,Pos_Start+Pos_Seek,SEEK_SET);

	DWORD BlockEnd = 0xFFFFFFFF;
	DWORD Version = 0;
	fread(&Version,sizeof(DWORD),1,pFile);

	fread(&BlockEnd,sizeof(DWORD),1,pFile);

	return S_OK;
}

KG3DTerrainRegionData::_CellTextureManager::_CellTextureManager()
{
	;
}

KG3DTerrainRegionData::_CellTextureManager:: ~_CellTextureManager()
{
	;
}

HRESULT KG3DTerrainRegionData::_CellTextureManager::CreateRegionSubsetInfo(KG3DTerrainRegionData* pRegion)
{
	pRegion->m_vecLevel0Subsets.clear();

	for(DWORD i=0;i<16*16;i++)
	{
		_CellInfo* pCell = &pRegion->m_Level0CellTextureInfo[i];

		BOOL bFound = FALSE;
		for(DWORD K =0;K<pRegion->m_vecLevel0Subsets.size();K++)
		{
			if((pRegion->m_vecLevel0Subsets[K].TextureIndex[0]==pCell->TextureIndex[0])&&
				(pRegion->m_vecLevel0Subsets[K].TextureIndex[1]==pCell->TextureIndex[1]))
			{
				bFound = TRUE;
				pRegion->m_vecLevel0Subsets[K].CellIndex.push_back(i);
				break;
			}
		}
		if(!bFound)
		{
			_Subset Subset;
			Subset.TextureIndex[0] = pCell->TextureIndex[0];
			Subset.TextureIndex[1] = pCell->TextureIndex[1];
			Subset.CellIndex.push_back(i);
			pRegion->m_vecLevel0Subsets.push_back(Subset);
		}
	}
	
	return CreateRegionSubsetVertexBuffer(pRegion);
}
		
HRESULT KG3DTerrainRegionData::_CellTextureManager::CreateRegionSubsetVertexBuffer(KG3DTerrainRegionData* pRegion)
{
	int PerSideCell;
	int CellSize;
	int Size;

	Size = 16 * 16 * 6;
	PerSideCell =16;
	CellSize = 100;

	VFormat::_Faces_Normal_Diffuse_Texture0 *pVers;
	if (FAILED(pRegion->m_lpLevel0SubsetVB->Lock(0,Size * sizeof(VFormat::_Faces_Normal_Diffuse_Texture0),
		(void **)&pVers,0)))
		return E_FAIL;

	DWORD dwCellStart = 0;
	for(DWORD K = 0;K < pRegion->m_vecLevel0Subsets.size();K++)
	{
		_Subset* pSubset = &pRegion->m_vecLevel0Subsets[K];
		pSubset->CellStart = dwCellStart;

		for(DWORD N = 0;N < pSubset->CellIndex.size();N++)
		{
			DWORD CellIndex = pSubset->CellIndex[N];
			_CellInfo* pCell  = &pRegion->m_Level0CellTextureInfo[CellIndex];
			int nX = CellIndex % PerSideCell;
			int nZ = CellIndex / PerSideCell;

			VFormat::_Faces_Normal_Diffuse_Texture0 A,B,C,D;
			D3DXVECTOR3 Pos(nX*CellSize + pRegion->m_BBox.A.x,0,nZ*CellSize + pRegion->m_BBox.A.z);

			A.p = Pos;
			B.p = Pos + D3DXVECTOR3(0,0,(float)CellSize);
			C.p = Pos + D3DXVECTOR3((float)CellSize,0,(float)CellSize);
			D.p = Pos + D3DXVECTOR3((float)CellSize,0,0);


			pRegion->m_lpHeightGround->GetPosition(&A.p,D3DXVECTOR2(A.p.x,A.p.z));
			pRegion->m_lpHeightGround->GetPosition(&B.p,D3DXVECTOR2(B.p.x,B.p.z));
			pRegion->m_lpHeightGround->GetPosition(&C.p,D3DXVECTOR2(C.p.x,C.p.z));
			pRegion->m_lpHeightGround->GetPosition(&D.p,D3DXVECTOR2(D.p.x,D.p.z));

			A.Normal = B.Normal = C.Normal = D.Normal = D3DXVECTOR3(0,1,0);

			A.diffuse = 0x00FFFFFF |(((DWORD)pCell->TextureWeight[0][0])<<24);
			B.diffuse = 0x00FFFFFF |(((DWORD)pCell->TextureWeight[0][1])<<24);
			C.diffuse = 0x00FFFFFF |(((DWORD)pCell->TextureWeight[0][2])<<24);
			D.diffuse = 0x00FFFFFF |(((DWORD) pCell->TextureWeight[0][3])<<24);

			/*float U = nX / 4.0F;
			float V = nZ / 4.0F;
			float t = 0.25F ;

			A.tu1 = V;   A.tv1 = U;
			B.tu1 = V+t;B.tv1 = U;
			C.tu1 = V+t;C.tv1 = U+t;
			D.tu1 = V;   D.tv1 = U+t;*/

			pVers[dwCellStart*6   ]  = A;
			pVers[dwCellStart*6+1] = B;
			pVers[dwCellStart*6+2] = C;
			pVers[dwCellStart*6+3] = C;
			pVers[dwCellStart*6+4] = D;
			pVers[dwCellStart*6+5] = A;

			dwCellStart++;
		}
	}

	pRegion->m_lpLevel0SubsetVB->Unlock();
	dwCellStart = 0;

	VFormat::_Texture1 *pTexCoord;
	if (FAILED(pRegion->m_lpLightMapCoord->Lock(0,Size * sizeof(VFormat::_Texture1),
		(void **)&pTexCoord,0)))
		return E_FAIL;

//	DWORD dwCellStart = 0;
	for(DWORD K = 0;K < pRegion->m_vecLevel0Subsets.size();K++)
	{
		_Subset* pSubset = &pRegion->m_vecLevel0Subsets[K];
		pSubset->CellStart = dwCellStart;

		for(DWORD N = 0;N < pSubset->CellIndex.size();N++)
		{
			DWORD CellIndex = pSubset->CellIndex[N];
			_CellInfo* pCell  = &pRegion->m_Level0CellTextureInfo[CellIndex];
			int nX = CellIndex % PerSideCell;
			int nZ = CellIndex / PerSideCell;

			VFormat::_Texture1 A,B,C,D;
	
			float U = nX / 16.0f;
			float V = nZ / 16.0f;
			float t = 1.0f / 16.0f ;

		
			A.tu1 = U;   A.tv1 = V;
			B.tu1 = U;	 B.tv1 = V + t;
			C.tu1 = U+t; C.tv1 = V+t;
			D.tu1 = U + t;   D.tv1 = V;

			pTexCoord[dwCellStart*6  ] = A;
			pTexCoord[dwCellStart*6+1] = B;
			pTexCoord[dwCellStart*6+2] = C;
			pTexCoord[dwCellStart*6+3] = C;
			pTexCoord[dwCellStart*6+4] = D;
			pTexCoord[dwCellStart*6+5] = A;

			dwCellStart++;
		}
	}

	pRegion->m_lpLightMapCoord->Unlock();
	dwCellStart = 0;

	if (FAILED(pRegion->m_lpTexCoord->Lock(0,Size * sizeof(VFormat::_Texture1),
		(void **)&pTexCoord,0)))
		return E_FAIL;

//	DWORD dwCellStart = 0;
	for(DWORD K = 0;K < pRegion->m_vecLevel0Subsets.size();K++)
	{
		_Subset* pSubset = &pRegion->m_vecLevel0Subsets[K];
		pSubset->CellStart = dwCellStart;

		for(DWORD N = 0;N < pSubset->CellIndex.size();N++)
		{
			DWORD CellIndex = pSubset->CellIndex[N];
			_CellInfo* pCell  = &pRegion->m_Level0CellTextureInfo[CellIndex];
			int nX = CellIndex % PerSideCell;
			int nZ = CellIndex / PerSideCell;

			VFormat::_Texture1 A,B,C,D;

			float U = nX / 4.0f;
			float V = nZ / 4.0f;
			float t = (float)1.0 / 4.0 ;

			A.tu1 = U;   A.tv1 = V;
			B.tu1 = U;	 B.tv1 = V + t;
			C.tu1 = U+t; C.tv1 = V+t;
			D.tu1 = U + t;   D.tv1 = V;

			pTexCoord[dwCellStart*6  ] = A;
			pTexCoord[dwCellStart*6+1] = B;
			pTexCoord[dwCellStart*6+2] = C;
			pTexCoord[dwCellStart*6+3] = C;
			pTexCoord[dwCellStart*6+4] = D;
			pTexCoord[dwCellStart*6+5] = A;

			dwCellStart++;
		}
	}

		pRegion->m_lpTexCoord->Unlock();

	return S_OK;
}

HRESULT   KG3DTerrainRegionData::DrawSubset(_Subset* pSubset,const KG3DTerrain* pTerrain)
{
	if(!m_lpLevel0SubsetVB)
		return S_OK;
	if(!m_lpGroundTextureMgr)
		return S_OK;

	_GroundTexture* pTextureA = m_lpGroundTextureMgr->GetTexture((int)pSubset->TextureIndex[0] - 1);

	if(pTextureA)
	{
		if(pTextureA->m_lpDiffuseTexture)
			pTextureA->m_lpDiffuseTexture->SetTexture(0);

		_GroundTexture* pTextureB = m_lpGroundTextureMgr->GetTexture((int)pSubset->TextureIndex[1] - 1);

		if(pTextureB)
		{
			if(pTextureB->m_lpDiffuseTexture)
				pTextureB->m_lpDiffuseTexture->SetTexture(1);

			pSubset->TextureCount = 2;
		}
		else
			pSubset->TextureCount = 1;
	}
	else
	{
		g_pd3dDevice->SetTexture(0,NULL);
		g_pd3dDevice->SetTexture(1,NULL);
	}

	g_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	g_pd3dDevice->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	g_pd3dDevice->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	g_pd3dDevice->SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	if(pSubset->TextureCount==1)
	{
	//	KG3DTerrainEx * pex= (KG3DTerrainEx *)pTerrain;
		if(!pTerrain->m_bLightMapIsInit)
		{
			g_pd3dDevice->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);
			g_pd3dDevice->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
			g_pd3dDevice->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_CURRENT);		

			g_pd3dDevice->SetStreamSource(0, m_lpLevel0SubsetVB, 0, sizeof(VFormat::_Faces_Normal_Diffuse_Texture0));  
			g_pd3dDevice->SetStreamSource(1, m_lpTexCoord, 0, sizeof(VFormat::_Texture1));   
			g_pd3dDevice->SetVertexDeclaration(pTerrain->m_lpTerrainVertexDeclaration0);
			g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST,pSubset->CellStart*6,(UINT)pSubset->CellIndex.size()*2);
		}
		else
		{
	//		g_pd3dDevice->SetTexture(1,m_lpRegionLightMap);
			
			g_pd3dDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE,D3DMCS_COLOR1);
			g_pd3dDevice->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE);
			g_pd3dDevice->SetTextureStageState(1,D3DTSS_COLORARG1,D3DTA_DIFFUSE);		
			g_pd3dDevice->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);

			/*g_pd3dDevice->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_MODULATE );
			g_pd3dDevice->SetTextureStageState(1,D3DTSS_COLORARG1,D3DTA_TEXTURE);
			g_pd3dDevice->SetTextureStageState(1,D3DTSS_COLORARG2,D3DTA_CURRENT);		
			g_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,D3DTOP_DISABLE );*/

			g_pd3dDevice->SetStreamSource(0, m_lpLevel0SubsetVB, 0, sizeof(VFormat::_Faces_Normal_Diffuse_Texture0));  
			g_pd3dDevice->SetStreamSource(1, m_lpTexCoord, 0, sizeof(VFormat::_Texture1));  
		//	g_pd3dDevice->SetStreamSource(2, m_lpLightMapCoord, 0, sizeof(VFormat::_Texture1));  
			g_pd3dDevice->SetVertexDeclaration(pTerrain->m_lpTerrainVertexDeclaration0);
			g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST,pSubset->CellStart*6,(UINT)pSubset->CellIndex.size()*2);
		}
	}
	else if(pSubset->TextureCount==2)
	{
		if(!pTerrain->m_bLightMapIsInit)
		{	
			g_pd3dDevice->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG2);
			g_pd3dDevice->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_DIFFUSE);		
			g_pd3dDevice->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_TEXTURE);
			g_pd3dDevice->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);
			g_pd3dDevice->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_DIFFUSE);		
			g_pd3dDevice->SetTextureStageState(0,D3DTSS_ALPHAARG2,D3DTA_TEXTURE);

			g_pd3dDevice->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_BLENDDIFFUSEALPHA);
			g_pd3dDevice->SetTextureStageState(1,D3DTSS_COLORARG1,D3DTA_CURRENT);		
			g_pd3dDevice->SetTextureStageState(1,D3DTSS_COLORARG2,D3DTA_TEXTURE);
			g_pd3dDevice->SetTextureStageState(1,D3DTSS_ALPHAOP,D3DTOP_SELECTARG2);
			g_pd3dDevice->SetTextureStageState(1,D3DTSS_ALPHAARG1,D3DTA_DIFFUSE);		
			g_pd3dDevice->SetTextureStageState(1,D3DTSS_ALPHAARG2,D3DTA_TEXTURE);
			g_pd3dDevice->SetTextureStageState(1,D3DTSS_TEXCOORDINDEX,D3DTSS_TCI_PASSTHRU);

			g_pd3dDevice->SetStreamSource(0, m_lpLevel0SubsetVB, 0, sizeof(VFormat::_Faces_Normal_Diffuse_Texture0));  
			g_pd3dDevice->SetStreamSource(1, m_lpTexCoord, 0, sizeof(VFormat::_Texture1));  
			g_pd3dDevice->SetStreamSource(2, m_lpTexCoord, 0, sizeof(VFormat::_Texture1)); 
			g_pd3dDevice->SetVertexDeclaration(pTerrain->m_lpTerrainVertexDeclaration1);
			g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST,pSubset->CellStart*6,(UINT)pSubset->CellIndex.size()*2);
		}
		else
		{
			g_pd3dDevice->SetTexture(2,m_lpRegionLightMap);

			g_pd3dDevice->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG2);
			g_pd3dDevice->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_DIFFUSE);		
			g_pd3dDevice->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_TEXTURE);
			g_pd3dDevice->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);
			g_pd3dDevice->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_DIFFUSE);		
			g_pd3dDevice->SetTextureStageState(0,D3DTSS_ALPHAARG2,D3DTA_TEXTURE);

			g_pd3dDevice->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_BLENDDIFFUSEALPHA);
			g_pd3dDevice->SetTextureStageState(1,D3DTSS_COLORARG1,D3DTA_CURRENT);		
			g_pd3dDevice->SetTextureStageState(1,D3DTSS_COLORARG2,D3DTA_TEXTURE);
			g_pd3dDevice->SetTextureStageState(1,D3DTSS_ALPHAOP,D3DTOP_SELECTARG2);
			g_pd3dDevice->SetTextureStageState(1,D3DTSS_ALPHAARG1,D3DTA_DIFFUSE);		
			g_pd3dDevice->SetTextureStageState(1,D3DTSS_ALPHAARG2,D3DTA_TEXTURE);
			g_pd3dDevice->SetTextureStageState(1,D3DTSS_TEXCOORDINDEX,D3DTSS_TCI_PASSTHRU);

			g_pd3dDevice->SetTextureStageState(2,D3DTSS_COLOROP,D3DTOP_MODULATE );
			g_pd3dDevice->SetTextureStageState(2,D3DTSS_COLORARG1,D3DTA_TEXTURE);
			g_pd3dDevice->SetTextureStageState(2,D3DTSS_COLORARG2,D3DTA_CURRENT);		
			g_pd3dDevice->SetTextureStageState( 2, D3DTSS_ALPHAOP,D3DTOP_DISABLE );

			g_pd3dDevice->SetStreamSource(0, m_lpLevel0SubsetVB, 0, sizeof(VFormat::_Faces_Normal_Diffuse_Texture0));  
			g_pd3dDevice->SetStreamSource(1, m_lpTexCoord, 0, sizeof(VFormat::_Texture1));  
			g_pd3dDevice->SetStreamSource(2, m_lpTexCoord, 0, sizeof(VFormat::_Texture1)); 
			g_pd3dDevice->SetStreamSource(3, m_lpLightMapCoord, 0, sizeof(VFormat::_Texture1));  
			g_pd3dDevice->SetVertexDeclaration(pTerrain->m_lpTerrainVertexDeclaration2);
			g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST,pSubset->CellStart*6,(UINT)pSubset->CellIndex.size()*2);
		}
	}


	return S_OK;
}

float KG3DTerrainRegionData::BrushWeight(float X,float Y,float CX,float CY,float BrushWidth,int nType)
{
	if(nType==0)
	{
		float dis = sqrtf((Y - CY) * (Y - CY) + (X - CX) * (X - CX));
		if(dis>BrushWidth/2)
			return 0;

		float a = dis / BrushWidth;
		float w = exp(-a) ;
		return w;
	}
	else if(nType==0)
	{
		float dis = sqrtf((Y - CY) * (Y - CY) + (X - CX) * (X - CX));
		float a = dis / BrushWidth;
		float w = exp(-a) ;
		return w;
	}
	return 0;
}

HRESULT KG3DTerrainRegionData::PaintGroundTexture(FRECT Rect,int TextureIndex,int Alpha,int nShape)
{
	D3DXVECTOR2 Brush_Center = Rect.GetCenter();
	float Width = Rect.Width;
	//////////////////////////////////////////////////////////////////////////
	D3DXVECTOR2 A,C;
	Rect.ToPosition(&A,&C);
	int CellSize = 100;

	int nX_Start = (int)((A.x-m_BBox.A.x) / CellSize);
	int nX_End   = (int)((C.x-m_BBox.A.x) / CellSize);

	int nZ_Start = (int)((A.y-m_BBox.A.z) / CellSize);
	int nZ_End   = (int)((C.y-m_BBox.A.z) / CellSize);

	//////////////////////////////////////////////////////////////////////////

	if ((nX_Start <= 16 )&&(nZ_Start <= 16)&&
		(nX_End >= 0)&&(nZ_End >= 0))
	{
		nX_Start = max(0,nX_Start);nZ_Start = max(0,nZ_Start);
		nX_End = min(16,nX_End);nZ_End = min(16,nZ_End);

		for(int Z = nZ_Start;Z<nZ_End;Z++)
		{
			float Brush_Z = Z*CellSize+m_BBox.A.z;
			float Brush_ZU = (Z+1)*CellSize+m_BBox.A.z;

			for(int X = nX_Start;X<nX_End;X++)
			{
				float Brush_X = X*CellSize+m_BBox.A.x;
				float Brush_XU = (X+1)*CellSize+m_BBox.A.x;

				int Index = Z * 16 + X;

				_CellInfo* pCellInfo = &m_Level0CellTextureInfo[Index];
				static int  nAlpha[4];
				nAlpha[0] = (int)(BrushWeight(Brush_X,Brush_Z,Brush_Center.x,Brush_Center.y,Width,nShape) *Alpha);
				nAlpha[1] = (int)(BrushWeight(Brush_X,Brush_ZU,Brush_Center.x,Brush_Center.y,Width,nShape) *Alpha);
				nAlpha[2] = (int)(BrushWeight(Brush_XU,Brush_ZU,Brush_Center.x,Brush_Center.y,Width,nShape) *Alpha);
				nAlpha[3] = (int)(BrushWeight(Brush_XU,Brush_Z,Brush_Center.x,Brush_Center.y,Width,nShape) *Alpha);
				m_CellTextureManager.PaintCellTexture(pCellInfo,TextureIndex + 1,nAlpha);
			}
		}
		m_nNumPaintGround++;
		if(m_nNumPaintGround>4)
		{
			m_CellTextureManager.CreateRegionSubsetInfo(this);
			m_nNumPaintGround = 0;
			m_bNeedReRenderTexture = TRUE;
		}
		return S_OK;//
	}
	else
	{
		return S_OK;
	}
}

HRESULT KG3DTerrainRegionData::_CellTextureManager::PaintCellTexture(_CellInfo* pInfo,int TextureIndex,int Alpha[])
{
	//////////////////////////////////////////////////////////////////////////
	//先寻找相同的贴图
	if(pInfo->TextureIndex[0] == TextureIndex)
	{
		SmoothAdd(&pInfo->TextureWeight[0][0],&pInfo->TextureWeight[1][0],Alpha[0]);
		SmoothAdd(&pInfo->TextureWeight[0][1],&pInfo->TextureWeight[1][1],Alpha[1]);
		SmoothAdd(&pInfo->TextureWeight[0][2],&pInfo->TextureWeight[1][2],Alpha[2]);
		SmoothAdd(&pInfo->TextureWeight[0][3],&pInfo->TextureWeight[1][3],Alpha[3]);
	}
	else if(pInfo->TextureIndex[1] == TextureIndex)
	{
		SmoothAdd(&pInfo->TextureWeight[1][0],&pInfo->TextureWeight[0][0],Alpha[0]);
		SmoothAdd(&pInfo->TextureWeight[1][1],&pInfo->TextureWeight[0][1],Alpha[1]);
		SmoothAdd(&pInfo->TextureWeight[1][2],&pInfo->TextureWeight[0][2],Alpha[2]);
		SmoothAdd(&pInfo->TextureWeight[1][3],&pInfo->TextureWeight[0][3],Alpha[3]);
	}
	else if(pInfo->TextureIndex[0] == 0)
	{
		pInfo->TextureIndex[0] = (BYTE)TextureIndex;
		SmoothAdd(&pInfo->TextureWeight[0][0],&pInfo->TextureWeight[1][0],Alpha[0]);
		SmoothAdd(&pInfo->TextureWeight[0][1],&pInfo->TextureWeight[1][1],Alpha[1]);
		SmoothAdd(&pInfo->TextureWeight[0][2],&pInfo->TextureWeight[1][2],Alpha[2]);
		SmoothAdd(&pInfo->TextureWeight[0][3],&pInfo->TextureWeight[1][3],Alpha[3]);

	}
	else if(pInfo->TextureIndex[1] == 0)
	{
		pInfo->TextureIndex[1] = (BYTE)TextureIndex;
		SmoothAdd(&pInfo->TextureWeight[1][0],&pInfo->TextureWeight[0][0],Alpha[0]);
		SmoothAdd(&pInfo->TextureWeight[1][1],&pInfo->TextureWeight[0][1],Alpha[1]);
		SmoothAdd(&pInfo->TextureWeight[1][2],&pInfo->TextureWeight[0][2],Alpha[2]);
		SmoothAdd(&pInfo->TextureWeight[1][3],&pInfo->TextureWeight[0][3],Alpha[3]);
	}
	else
	{
		int W0 = pInfo->TextureWeight[0][0]+
			pInfo->TextureWeight[0][1]+	
			pInfo->TextureWeight[0][2]+		
			pInfo->TextureWeight[0][3];
		int W1 = pInfo->TextureWeight[1][0]+
			pInfo->TextureWeight[1][1]+	
			pInfo->TextureWeight[1][2]+		
			pInfo->TextureWeight[1][3];
		if(W0<=W1)
		{
			pInfo->TextureIndex[0] = (BYTE)TextureIndex;
		}
		else
		{
			pInfo->TextureIndex[1] = (BYTE)TextureIndex;
		}
	}

	if(pInfo->TextureIndex[0]<pInfo->TextureIndex[1])
	{
		swap(pInfo->TextureIndex[0],pInfo->TextureIndex[1]);
		swap(pInfo->TextureWeight[0][0],pInfo->TextureWeight[1][0]);
		swap(pInfo->TextureWeight[0][1],pInfo->TextureWeight[1][1]);
		swap(pInfo->TextureWeight[0][2],pInfo->TextureWeight[1][2]);
		swap(pInfo->TextureWeight[0][3],pInfo->TextureWeight[1][3]);
	}

	pInfo->TextureCount = 0;
	if(pInfo->TextureIndex[0]>0)
		pInfo->TextureCount++;
	if(pInfo->TextureIndex[1]>0)
		pInfo->TextureCount++;

	return S_OK;
}

HRESULT KG3DTerrainRegionData::SaveGroundHeightToFile(LPSTR pDirName)
{
	if(!m_lpHeightGround)
		return E_FAIL;

	TCHAR FileName[256];
	wsprintf(FileName,"%s\\Region_%d_%d.GHeight",pDirName,m_iIndex_X,m_iIndex_Z);
	
	FILE* pFile = fopen(FileName,"wb");
	if(!pFile)
	{
		KGLogPrintf(KGLOG_ERR, "无法写入文件%s.\n",FileName);
		return E_FAIL;
	}
	m_lpHeightGround->Save2File(pFile);
	fclose(pFile);
	return S_OK;
}

HRESULT KG3DTerrainRegionData::LoadGroundHeightFromFile(LPSTR pFileName,const KG3DTerrain *pTerrain)
{
	if(!m_lpHeightGround)
		return E_FAIL;

	FILE* pFile = fopen(pFileName,"rb");
	if(!pFile)
	{
		KGLogPrintf(KGLOG_ERR, "无法读取文件%s.\n",pFileName);
		return E_FAIL;
	}
	m_lpHeightGround->LoadFromFile(pFile);
	fclose(pFile);

	CreateVertexBuffer(pTerrain);
	return S_OK;
}

HRESULT KG3DTerrainRegionData::SaveGroundTextureToFile(LPSTR pDirName)
{
	if(!m_lpGroundTextureMgr)
		return E_FAIL;

	TCHAR FileName[256];
	wsprintf(FileName,"%s\\Region_%d_%d.Tex",pDirName,m_iIndex_X,m_iIndex_Z);
	struct _TexFileHead
	{
		DWORD dwMask;
		DWORD Version;
		_TexFileHead()
		{
			dwMask = 0xFDFDFDFD;
			Version  = 0;
		}
	}TexFileHead;

	DWORD dwBlockEnd = 0xFFFFFFFF;
	FILE* pFile = fopen(FileName,"wb");
	if(!pFile)
	{
		KGLogPrintf(KGLOG_ERR, "无法写入文件%s.\n",FileName);
		return E_FAIL;
	}

	static _CellInfoFile CellInfoFile[16*16];
	for(int i=0;i<16*16;i++ )
	{
		CellInfoFile[i].TextureCount = m_Level0CellTextureInfo[i].TextureCount;
		
		_GroundTexture* pTextureA = m_lpGroundTextureMgr->GetTexture((int)m_Level0CellTextureInfo[i].TextureIndex[0] - 1);

		if(pTextureA)
			CellInfoFile[i].TextureUID[0] = (DWORD) pTextureA->GetUID();
		else
			CellInfoFile[i].TextureUID[0] = 0;

		_GroundTexture* pTextureB = m_lpGroundTextureMgr->GetTexture((int)m_Level0CellTextureInfo[i].TextureIndex[1] - 1);
		if(pTextureB)
			CellInfoFile[i].TextureUID[1] = (DWORD) pTextureB->GetUID();
		else
			CellInfoFile[i].TextureUID[1] = 0;

		CellInfoFile[i].TextureWeight[0][0] = m_Level0CellTextureInfo[i].TextureWeight[0][0];
		CellInfoFile[i].TextureWeight[0][1] = m_Level0CellTextureInfo[i].TextureWeight[0][1];
		CellInfoFile[i].TextureWeight[0][2] = m_Level0CellTextureInfo[i].TextureWeight[0][2];
		CellInfoFile[i].TextureWeight[0][3] = m_Level0CellTextureInfo[i].TextureWeight[0][3];
		CellInfoFile[i].TextureWeight[1][0] = m_Level0CellTextureInfo[i].TextureWeight[1][0];
		CellInfoFile[i].TextureWeight[1][1] = m_Level0CellTextureInfo[i].TextureWeight[1][1];
		CellInfoFile[i].TextureWeight[1][2] = m_Level0CellTextureInfo[i].TextureWeight[1][2];
		CellInfoFile[i].TextureWeight[1][3] = m_Level0CellTextureInfo[i].TextureWeight[1][3];
	}

	fwrite(&TexFileHead,sizeof(TexFileHead),1,pFile);
	fwrite(&CellInfoFile,sizeof(_CellInfoFile),16*16,pFile);
	fwrite(&dwBlockEnd,sizeof(DWORD),1,pFile);

	fclose(pFile);

	return S_OK;
}

HRESULT KG3DTerrainRegionData::CheckBorder(KG3DTerrainEx* pTerrain)
{
	if(!m_lpHeightGround)
		return S_OK;

	LPREGION pRegion = NULL;

	pRegion = GetNeighborRegion_Left(pTerrain);
	if(pRegion)
	{
		if(pRegion->m_lpHeightGround)
			m_lpHeightGround->CheckBorder_Left(pRegion->m_lpHeightGround);
	}
	//////////////////////////////////////////////////////////////////////////
	pRegion = GetNeighborRegion_Up(pTerrain);
	if(pRegion)
	{
		if(pRegion->m_lpHeightGround)
			m_lpHeightGround->CheckBorder_Up(pRegion->m_lpHeightGround);
	}	

	//////////////////////////////////////////////////////////////////////////
	//pRegion = GetNeighborRegion_Right(pTerrain);
	//if(pRegion)
	//{
	//	if(pRegion->m_lpHeightGround)
	//		m_lpHeightGround->CheckBorder_Right(pRegion->m_lpHeightGround);
	//}

	////////////////////////////////////////////////////////////////////////////
	//pRegion = GetNeighborRegion_Down(pTerrain);
	//if(pRegion)
	//{
	//	if(pRegion->m_lpHeightGround)
	//		m_lpHeightGround->CheckBorder_Down(pRegion->m_lpHeightGround);
	//}
	return S_OK;
}

}
