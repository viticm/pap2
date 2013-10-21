// SceneSkinEdit.cpp: implementation of the KSceneSceneEdit class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SceneEditor.h"
#include "SceneSkinEdit.h"
#include ".\sceneskinedit.h"
#include <algorithm>

#include "KSceneSceneEditorDialogGroundTextureMgr.h"

#include "KSceneSceneEditorDialogNewMapWizard.h"
#include "KSceneSceneEditorDialogObjectMgr.h"
#include "MeshTerran.h"
#include "KModelSceneSFX.h"

#include "KSceneSceneEditorNpcPropertiesDialog.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//游戏逻辑的文件头
typedef struct _MAP_FILE_HEADER
{
	DWORD	m_dwWidth;
	DWORD	m_dwHeight;
}MAP_FILE_HEADER;



KSceneSceneEdit::KSceneSceneEdit()
{
	m_bEnablePlaceArray = FALSE;
	m_bEnableRandomRotation = TRUE;
	m_bEnableRandomScale = FALSE;
	m_fRotationDN = - D3DX_PI;
	m_fRotationUP = D3DX_PI;
	m_fScaleDN = 0.7f;
	m_fScaleUP = 1.4f;
	m_fPlaceArrayDest = 2.5f;

	m_fSunHeight = 0.5f;

	m_lpTextureSky = NULL;

	m_dwTestTimeStart = 0;
	m_bTesting = FALSE;
	m_nFrameCount = 0;

	//m_lpWater = NULL;
	m_lpInHandObject = NULL;

	m_bLOD = FALSE;

	//m_lpTerrainLOD = NULL;

	m_lpSkyBox = NULL;
	m_lpSSFX = NULL;
	m_CameraDistance = 1200;

	m_CameraLookAt = D3DXVECTOR3(0,100,0);
	m_CameraAngle = D3DXVECTOR3(3.14f/6,-3.14f/4,0);

	float H = m_CameraDistance * sinf(m_CameraAngle.x);
	float L = m_CameraDistance * cosf(m_CameraAngle.x);

	m_CameraPosition = m_CameraLookAt + D3DXVECTOR3(L*sinf(m_CameraAngle.y),H,L*cosf(m_CameraAngle.y));
	m_LastOperation = OPERATION_NONE;

	m_lpMoveGrid = NULL;
	m_lpRotateGrid = NULL;

	m_dType = SCENE_SKINEDITOR;

	m_lpMapMesh = NULL;
	m_dType = 0;
	ID = 0;

	//m_lpLightFlare = NULL;
	m_dwState = 0;
	m_bShowSelectionRect = FALSE;

	m_GridTextureID = 0;

	m_SelectedCenter = D3DXVECTOR3(0,0,0);
	m_dwSubState = 0;

	//m_lpObjectTool = NULL;

	m_bMeshEdit = FALSE;

	m_nDestMapWidth = 0;
	m_nDestMapHeigth = 0;
	m_bNeedReSizeMap = FALSE;

	m_vecInHandObjectName.clear();
	m_nCurInHandObjectIndex = -1;

	m_lpDialogPanel = NULL;

	m_hWndSmallMap = NULL;
	m_hWndObjectPreView = NULL;

	m_bNeedRefreshSmallMap = FALSE;
	m_dwSmallMapOption = 0;
	m_fSmallMapScaling = 1.0f;
	m_SmallMapCenter = D3DXVECTOR3(0,0,0);

	m_bOptimizeSmallEntity = FALSE;
	m_lpDialogLargeTerrian = NULL;
	m_lpDialogSceneSetting = NULL;
	m_dwWaterReflectionMapID = 0;

	m_lpDialogGroundTextureMgr = NULL;
	m_lpDialogObjectMgr = NULL;

	m_pScene = NULL;

	static DWORD s_dwLogicalMapIDCounter = 0;
	s_dwLogicalMapIDCounter++;
	m_dwLogicalMapID = s_dwLogicalMapIDCounter;

	m_dwShowObscale = 0;
}

KSceneSceneEdit::~KSceneSceneEdit()
{
	if (m_pScene != NULL)
	{
		//删除原来的地图
		g_SO3World.DeleteScene(m_pScene, TRUE);
		m_pScene = NULL;
	}

	SAFE_DELETE(m_lpDialogPanel);
	SAFE_DELETE(m_lpDialogSceneSetting);
	SAFE_DELETE(m_lpDialogLargeTerrian);
	SAFE_DELETE(m_lpDialogGroundTextureMgr);
	//////////////////////////////////////////////////////////////////////////
	//清理旧的GROUP
	{
		vector<ObjectGroupLeaf*>::iterator i = m_vecEntityGroupLeaf.begin();
		while(i!=m_vecEntityGroupLeaf.end())
		{
			ObjectGroupLeaf* pGroup = *i;
			SAFE_DELETE(pGroup);
			i++;
		}
		m_vecEntityGroupLeaf.clear();
	}
}

HRESULT KSceneSceneEdit::ShowDialogSceneSetting(BOOL bShow)//显示或者关闭场景设置对话框
{
	if(bShow)
	{
		if(!m_lpDialogSceneSetting)
		{
			m_lpDialogSceneSetting = new KSceneSettingDialog;
			if(!m_lpDialogSceneSetting)
			{
				return E_FAIL;
			}
			m_lpDialogSceneSetting->Create(m_lpDialogSceneSetting->IDD);
			m_lpDialogSceneSetting->SetSceneEditor(this);
		}
		m_lpDialogSceneSetting->ShowWindow(TRUE);
	}
	else
	{
		if(m_lpDialogSceneSetting)
		{
			m_lpDialogSceneSetting->ShowWindow(FALSE);
		}
	}
	return S_OK;
}

HRESULT KSceneSceneEdit::ShowDialogGroundTextureMgr(BOOL bShow)
{
	if(bShow)
	{
		if(!m_lpDialogGroundTextureMgr)
		{
			m_lpDialogGroundTextureMgr = new KSceneSceneEditorDialogGroundTextureMgr;
			if(!m_lpDialogGroundTextureMgr)
			{
				return E_FAIL;
			}
			m_lpDialogGroundTextureMgr->SetSceneEditor(this);
			m_lpDialogGroundTextureMgr->Create(IDD_SCENEEDITOR_GROUNDTEXTURE);
		}
		m_lpDialogGroundTextureMgr->ShowWindow(TRUE);
		m_lpDialogGroundTextureMgr->CenterWindow();
	}
	else
	{
		if(m_lpDialogGroundTextureMgr)
		{
			m_lpDialogGroundTextureMgr->ShowWindow(FALSE);
		}
	}
	return S_OK;
}

HRESULT KSceneSceneEdit::ShowDialogObjectMgr(BOOL bShow)
{
	if(bShow)
	{
		if(!m_lpDialogObjectMgr)
		{
			m_lpDialogObjectMgr = new KSceneSceneEditorDialogObjectMgr;
			if(!m_lpDialogObjectMgr)
			{
				return E_FAIL;
			}
			m_lpDialogObjectMgr->SetSceneEditor(this);
			m_lpDialogObjectMgr->Create(IDD_SCENEEDITOR_OBJECTMGR);
		}
		m_lpDialogObjectMgr->ShowWindow(TRUE);
		m_lpDialogObjectMgr->CenterWindow();
	}
	else
	{
		if(m_lpDialogObjectMgr)
		{
			m_lpDialogObjectMgr->ShowWindow(FALSE);
		}
	}
	return S_OK;
}

HRESULT KSceneSceneEdit::ShowDialogLargeTerrian(BOOL bShow)//显示或者关闭大尺寸地形编辑器
{
	if(bShow)
	{
		if(!m_lpDialogLargeTerrian)
		{
			m_lpDialogLargeTerrian = new KSceneSceneEditorDialogLargeTerrian;
			if(!m_lpDialogLargeTerrian)
			{
				return E_FAIL;
			}
			m_lpDialogLargeTerrian->Create(IDD_SCENEEDITOR_LARGETERRIAN);
		}
		m_lpDialogLargeTerrian->ShowWindow(TRUE);
		m_lpDialogLargeTerrian->CenterWindow();
	}
	else
	{
		if(m_lpDialogLargeTerrian)
		{
			m_lpDialogLargeTerrian->ShowWindow(FALSE);
		}
	}
	return S_OK;
}

HRESULT KSceneSceneEdit::ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (WM_MBUTTONUP==message)
	{
		if(m_bEditing)
		{
			switch(m_dwState)
			{
			case EDITSTATE_PLACEOBJECT:
				{
					if(m_lpInHandObject)
					{
						if(m_bEnableRandomRotation)
						{
							float W = m_fRotationUP - m_fRotationDN;
							float K = (rand()%100)*0.01f * W + m_fRotationDN;
							m_lpInHandObject->m_Rotation.x = K;
						}
						if(m_bEnableRandomScale)
						{
							float W = m_fScaleUP - m_fScaleDN;
							float K = (rand()%100)*0.01f * W + m_fScaleDN;
							m_lpInHandObject->m_Scaling = D3DXVECTOR3(K,K,K);
						}
					}
					break;
				}
			}
		}
	}
	return KSceneEditBase::ProcessMessage(message,wParam,lParam);

}

HRESULT KSceneSceneEdit::FrameMove()
{
	{
		D3DXVECTOR3 Light(1,-2,1);
		//D3DXVECTOR3 Light(sinf(timeGetTime()*0.0002f),-2,cosf(timeGetTime()*0.0002f));
		D3DXVec3Normalize(&m_SunDirection,&Light);
	}

	if (m_bTesting)
	{
		m_nFrameCount++;
		int Time = timeGetTime() - m_dwTestTimeStart;
		if(Time>=30000)
		{
			m_bTesting = FALSE;
		}
		else
		{
			float t = Time * 6.283f / 30000 ;
			
			float X = sinf(t)*3000 + m_Center.x;
			float Z = cosf(t)*3000 + m_Center.z;
			m_lpGirlNo1->Edit_Move(&D3DXVECTOR3(X,0,Z));
		}
	}

	
	//m_lpLightFlare->FrameMove();

	m_lpMapMesh->FrameMove();

	if (m_lpInHandObject)
	{
		m_lpInHandObject->FrameMove();
	}

	if(!m_bEditing)
	{
		m_lpGirlNo1->FrameMove();

		if(g_KeyState[VK_UP])
		{
			D3DXVECTOR3 Vec(0,0,-10);
			D3DXMATRIX Mat;
			D3DXMatrixRotationY(&Mat,m_lpGirlNo1->m_Rotation.x);
			D3DXVec3TransformCoord(&Vec,&Vec,&Mat);

			D3DXVECTOR3 Pos = m_lpGirlNo1->m_Position + Vec;
			m_lpGirlNo1->Edit_Move(&Pos);
		}

		if(g_KeyState[VK_DOWN])
		{
			D3DXVECTOR3 Vec(0,0,10);
			D3DXMATRIX Mat;
			D3DXMatrixRotationY(&Mat,m_lpGirlNo1->m_Rotation.x);
			D3DXVec3TransformCoord(&Vec,&Vec,&Mat);

			D3DXVECTOR3 Pos = m_lpGirlNo1->m_Position + Vec;
			m_lpGirlNo1->Edit_Move(&Pos);
		}

		float H = 0;
		m_lpMapMesh->GetMapHeight(&H,m_lpGirlNo1->m_Position.x,m_lpGirlNo1->m_Position.z,1);
		m_lpGirlNo1->m_Position.y = H;


		if(g_KeyState[VK_RIGHT])
		{
			m_lpGirlNo1->m_Rotation.x += 3.1415f/36;
		}
		if(g_KeyState[VK_LEFT])
		{
			m_lpGirlNo1->m_Rotation.x -= 3.1415f/36;
		}

		if(g_KeyState[VK_PRIOR])
		{
			m_CameraAngle.x += 3.1415f/36;
		}
		if(g_KeyState[VK_NEXT])
		{
			m_CameraAngle.x -= 3.1415f/36;
		}
		if(g_KeyState[VK_HOME])
		{
			m_CameraAngle.y += 3.1415f/36;
		}
		if(g_KeyState[VK_END])
		{
			m_CameraAngle.y -= 3.1415f/36;
		}
		if(g_KeyState[VK_INSERT])
		{
			m_CameraDistance += 100;
		}
		if(g_KeyState[VK_DELETE])
		{
			m_CameraDistance -= 100;
		}
	}
	else
	{
		switch(m_dwState)
		{
		case EDITSTATE_TERRANEDIT:
			{
				if ((g_KeyState[VK_SPACE]==0)&&(g_Mouse_L))
				{
					KModelTerrain* pTerran = (KModelTerrain*) m_lpMapMesh;
					if ((pTerran)&&(pTerran->m_dwState))
					{
						pTerran->OnBush();
					}
				}
				break;
			}
		}

		if(m_bNeedRefreshSmallMap)
		{
			RenderSmallMap();
			m_bNeedRefreshSmallMap = FALSE;
		}

		RenderObjectPreview();

		if(m_lpDialogLargeTerrian)
		{
			if(m_lpDialogLargeTerrian->IsWindowVisible())
			{
				Render4LargeTerrianDialog(m_lpDialogLargeTerrian->GetOutPutWnd());
			}
		}
	}

	return K3DScene::FrameMove();
}

HRESULT KSceneSceneEdit::WaterMapCreate()
{
	HRESULT hr = S_OK;
	{
		LPTEXTURE pNewTexture = NULL;
		if (m_dwWaterReflectionMapID==0)
		{
			if (FAILED(g_cTextureTable.Get1NewTexture(&pNewTexture,0)))
				return hr;
		}
		else
		{
			if (FAILED(g_cTextureTable.GetTexture(&pNewTexture,m_dwWaterReflectionMapID)))
				return hr;
		}

		SAFE_RELEASE(pNewTexture->m_lpTexture);

		LPDIRECT3DTEXTURE9 pTexture = NULL;
		HRESULT hr = D3DXCreateTexture(g_pd3dDevice,512,
			512,
			1,
			D3DUSAGE_RENDERTARGET,
			D3DFMT_A8R8G8B8,
			D3DPOOL_DEFAULT,
			&pTexture);

		pNewTexture->m_lpTexture = pTexture;
		m_dwWaterReflectionMapID = pNewTexture->ID;
	}	


	//g_cTextureTable.LoadFromFile(&m_dGUIID,"模型文件\\2月17日2.tga");
	return S_OK;
}

HRESULT KSceneSceneEdit::WaterMapRelease()
{
	HRESULT hr = S_OK;

	if (m_dwWaterReflectionMapID!=0)
	{
		LPTEXTURE pNewTexture = NULL;
		if (SUCCEEDED(g_cTextureTable.GetTexture(&pNewTexture,m_dwWaterReflectionMapID)))
		{
			SAFE_RELEASE(pNewTexture->m_lpTexture);
		}
	}
	return S_OK;
}

HRESULT KSceneSceneEdit::RenderWaterReflection()
{
	if(m_dwWaterReflectionMapID==0)
	{
		WaterMapCreate();
	}

	LPTEXTURE pTexture = NULL;
	if (FAILED(g_cTextureTable.GetTexture(&pTexture,m_dwWaterReflectionMapID)))
		return E_FAIL;
	if(!pTexture->m_lpTexture)
		return E_FAIL;
	LPDIRECT3DSURFACE9 lpSurface = NULL;
	if(FAILED(pTexture->m_lpTexture->GetSurfaceLevel(0,&lpSurface)))
		return E_FAIL;
	lpSurface->Release();
	LPDIRECT3DSURFACE9 lpSurfaceSave = NULL;
	g_pd3dDevice->GetRenderTarget(0,&lpSurfaceSave);
	lpSurfaceSave->Release();
	g_pd3dDevice->SetRenderTarget(0,lpSurface);
	//////////////////////////////////////////////////////////////////////////
	
	D3DVIEWPORT9 ViewPortSave,ViewPort;
	g_pd3dDevice->GetViewport(&ViewPortSave);
	ViewPort = ViewPortSave;

	D3DXMATRIX Mat_View_Save;
	D3DXMATRIX Mat_Proj_Save;
	g_pd3dDevice->GetTransform( D3DTS_VIEW, &Mat_View_Save );
	g_pd3dDevice->GetTransform( D3DTS_PROJECTION, &Mat_Proj_Save );
	////////////////////////////////////////////////////////////////////////////
	ViewPort.X = 0;
	ViewPort.Y = 0;
	ViewPort.Width  = 512;
	ViewPort.Height = 512;
	g_pd3dDevice->SetViewport(&ViewPort);

	g_pd3dDevice->GetTransform( D3DTS_VIEW, &Mat_View_Save );

	D3DXPLANE WaterPlane;
	float H =  0;//100*sinf(timeGetTime()*0.001f);

	D3DXVECTOR3 A(0,H,0);
	D3DXVECTOR3 B(100,H,0);
	D3DXVECTOR3 C(0,H,100);

	D3DXPlaneFromPoints(&WaterPlane,&A,&C,&B);
	D3DXMATRIX matReflect,matView;
	D3DXMatrixReflect( &matReflect, &WaterPlane );
	D3DXMatrixMultiply( &matView, &matReflect, &Mat_View_Save );
	g_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

	//////////////////////////////////////////////////////////////////////////
	g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
		0x80000080, 1.0f, 0 );

	//SAFE_RENDER(m_lpSkyBox);


	g_pd3dDevice->SetClipPlane( 0, WaterPlane );
	g_pd3dDevice->SetRenderState( D3DRS_CLIPPLANEENABLE, D3DCLIPPLANE0 );
	g_pd3dDevice->SetRenderState( D3DRS_CULLMODE,         D3DCULL_CCW );

	m_lpMapMesh->Render();

	m_lpMapMesh->RenderRegionEntity();


	//////////////////////////////////////////////////////////////////////////	
	g_pd3dDevice->SetViewport(&ViewPortSave);
	g_pd3dDevice->SetRenderState( D3DRS_CULLMODE,         D3DCULL_CW );
	g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &Mat_Proj_Save );
	g_pd3dDevice->SetRenderTarget(0,lpSurfaceSave);
	
	g_pd3dDevice->SetTransform( D3DTS_VIEW, &Mat_View_Save );
	g_pd3dDevice->SetRenderState( D3DRS_CLIPPLANEENABLE,  FALSE  );
	return S_OK;
}

HRESULT KSceneSceneEdit::RenderScene()
{
	m_Fog.ApplyFog();

	float NowTime = timeGetTime()*1.0f;
	int FPS = (int)(1000.0f/(NowTime - m_TimeLast));

	static snGameLastTime = 0;
	if ((NowTime - snGameLastTime) > (1000 / 16))
	{
		g_SO3World.Activate();
		if (!m_bEditing)
		{
			static snLastX = 2048;
			static snLastY = 2048;

			KPlayer* pPlayer = &(g_SO3World.m_ObjectManager.m_PlayerSet[CLIENT_PLAYER_INDEX]);
			if(pPlayer)
			{
				int nX = pPlayer->m_nX;
				int nY = pPlayer->m_nY;

				if (!(nX == snLastX && nY == snLastY))
				{
					D3DXVECTOR3 Out(0,0,0);

					PT_Logical2Scene(&Out,nX,nY,0);

					m_lpGirlNo1->SetDestPosition(Out);

					snLastX = nX;
					snLastY = nY;
				}
			}
		}

		snGameLastTime = (int)NowTime;
	}



	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE,TRUE);
	SAFE_RENDER(m_lpSkyBox);
	//	SAFE_RENDER(m_lpSSFX);


	GraphicsStruct::KCamera* pCamera = &g_GraphiceEngine.m_cCamera; 

	if(!m_bEditing)
	{
		ComputeCamera();

		pCamera->Position  += (m_CameraPosition - pCamera->Position)*0.3f;
		pCamera->ViewPoint += (m_CameraLookAt - pCamera->ViewPoint)*0.3f;
		pCamera->zFar = 26000;
		pCamera->zNear = 10;
		pCamera->SetCamera();

		list<D3DXVECTOR3>::iterator k = m_listMoveDestPos.begin();
		while(k!=m_listMoveDestPos.end())
		{
			D3DXVECTOR3 Pos = *k;
			D3DXVECTOR3 X(Pos.x,Pos.y+200,Pos.z); 

			g_cGraphicsTool.DrawLine(&Pos,&X,0xFFFF0000,0xFFFF0000);
			D3DXVECTOR3 Y(Pos.x,Pos.y+200,Pos.z-1000); 
			g_cGraphicsTool.DrawLine(&Y,&X,0xFFFFFFFF,0xFFFF00FF);		

			k++;
		}
	}

	g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	KModelTerrain* pTerran = (KModelTerrain*) m_lpMapMesh;

	if ((m_dwState==EDITSTATE_TERRANEDIT)&&(pTerran)&&(pTerran->m_dwState))
	{
		KGraphicsEngine* pEng = &g_GraphiceEngine;

		D3DXVECTOR3 Orig,Dir;
		pEng->GetPickRay(&Orig,&Dir);
		D3DXVECTOR3 A = Orig + Dir*100;

		pTerran->SelectByPickRay(&Orig,&Dir);
	}
	else 
	{
		if (pTerran)
			pTerran->m_dwState = 0;
	}

	if(!m_bEditing)
	{
		m_lpMapMesh->m_FocusPosition = m_lpGirlNo1->m_Position;
		m_lpMapMesh->SetFocusRegion(&m_lpGirlNo1->m_Position);

		m_lpMapMesh->Render();

		SAFE_RENDER(m_lpGirlNo1);
		//m_lpMapMesh->RenderRegionEntity();

		if (m_dwWaterReflectionMapID!=0)
		{
			LPTEXTURE pNewTexture = NULL;
			if (SUCCEEDED(g_cTextureTable.GetTexture(&pNewTexture,m_dwWaterReflectionMapID)))
			{
				m_lpMapMesh->RenderWater(m_lpTextureSky,pNewTexture);
			}
		}	

		g_pd3dDevice->SetRenderState(D3DRS_LIGHTING,TRUE);

		//SAFE_RENDER(m_lpLightFlare);
	}
	else
	{
		pCamera->zFar = 50000;
		pCamera->zNear = 10;

		pCamera->SetCamera();	

		D3DXVECTOR3 Orig,Dir;
		g_cGraphicsTool.GetCameraPos(&Orig);
		g_cGraphicsTool.GetCameraFront(&Dir);
		m_lpMapMesh->SetFocusRegion(&Orig,&Dir);

		DWORD dwCull = 0;
		g_pd3dDevice->GetRenderState(D3DRS_CULLMODE,&dwCull);
		g_pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
		m_lpMapMesh->Render();
		g_pd3dDevice->SetRenderState(D3DRS_LIGHTING,TRUE);
		g_pd3dDevice->SetRenderState(D3DRS_CULLMODE,dwCull);


		K3DScene::ENTITYLIST::iterator i = m_RenderEntityList.begin();
		while(i!=m_RenderEntityList.end())
		{
			KSceneEntity Entity = *i;
			switch(Entity.Type)
			{
			case SCENEENTITY_OBJECT:
				{
					LPOBJECT pObject = (LPOBJECT) Entity.Pointer;
					if(m_lpMapMesh->IsObjectVisible(pObject))
					{
						Entity.Render();
					}
					break;
				}
			default:
				{
					Entity.Render();
					break;
				}
			}
			i++;
		}

		i = m_SelectedList.begin();
		while(i!=m_SelectedList.end())
		{
			KSceneEntity Entity = *i;
			DrawSelectObjectBox(&Entity);
			if(i == m_SelectedList.begin())
			{
				DrawSelectObjectInfo(&Entity);
			}

			i++;
		}

		if (m_dwWaterReflectionMapID!=0)
		{
			LPTEXTURE pNewTexture = NULL;
			if (SUCCEEDED(g_cTextureTable.GetTexture(&pNewTexture,m_dwWaterReflectionMapID)))
			{
				m_lpMapMesh->RenderWater(m_lpTextureSky,pNewTexture);
			}
		}	

		if (m_bShowSelectionRect)
		{
			D3DXVECTOR2 A,C;
			POINT PA = m_PointSelectStart;
			POINT PC = m_PointSelectEnd;

			A = D3DXVECTOR2(PA.x*1.0f,PA.y*1.0f);
			C = D3DXVECTOR2(PC.x*1.0f,PC.y*1.0f);
			float U = (C.x - A.x) *0.1f;
			float V = (C.y - A.y) *0.1f;
			g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
			g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCCOLOR);
			g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ONE);
			g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

			g_cGraphicsTool.DrawScreenRect(&A,&C,0,U,V,0xFFFFFFFF,m_GridTextureID);
		}	

		if ((m_dwState == EDITSTATE_MOVE)||(m_dwState == EDITSTATE_SCALE))
		{
			if (SUCCEEDED(GetSelectCenter(&m_SelectedCenter)))
			{
				D3DXMATRIX Scaling,Trans,Temp;
				D3DXVECTOR3 PO;
				g_cGraphicsTool.GetCameraPos(&PO);
				PO = PO - m_SelectedCenter;

				float S = D3DXVec3Length(&PO)*0.001f;
				D3DXMatrixScaling(&Scaling,S,S,S);
				D3DXMatrixTranslation(&Trans,m_SelectedCenter.x,m_SelectedCenter.y,m_SelectedCenter.z);

				D3DXMatrixMultiply(&Temp,&Scaling,&Trans);
				m_lpMoveGrid->SetMatrix(&Temp);

				if (m_dwSubState==0)
				{			
					KGraphicsEngine* pEng = &g_GraphiceEngine;

					D3DXVECTOR3 Orig,Dir;
					pEng->GetPickRay(&Orig,&Dir);
					m_lpMoveGrid->SelectByPickRay(&Orig,&Dir);
				}

				m_lpMoveGrid->Render();
			}
		}
		else if (m_dwState == EDITSTATE_ROTATE)
		{
			if (SUCCEEDED(GetSelectCenter(&m_SelectedCenter)))
			{
				D3DXVECTOR3 PO;
				g_cGraphicsTool.GetCameraPos(&PO);
				PO = PO - m_SelectedCenter;

				float S = D3DXVec3Length(&PO)*0.002f;
				m_lpRotateGrid->SetMatrix(S,&m_SelectedCenter);

				if (m_dwSubState==0)
				{			
					KGraphicsEngine* pEng = &g_GraphiceEngine;

					D3DXVECTOR3 Orig,Dir;
					pEng->GetPickRay(&Orig,&Dir);
					m_lpRotateGrid->SelectByPickRay(&Orig,&Dir);
				}

				m_lpRotateGrid->Render();
			}
		}
		else if (m_dwState == EDITSTATE_PLACEOBJECT)
		{
			KGraphicsEngine* pEng = &g_GraphiceEngine;
			D3DXVECTOR3 Orig,Dir;
			D3DXVECTOR3 Position(0,0,0);
			pEng->GetPickRay(&Orig,&Dir);

			if(m_lpInHandObject)
			{
				if (SUCCEEDED(m_lpMapMesh->GetMapCursorPosition(&Position,&Orig,&Dir)))
				{
					m_lpInHandObject->Edit_Move(&Position);

					m_lpInHandObject->Render();
				}
			}

			if(m_bEnablePlaceArray)
			{
				m_lpMapMesh->SelectByPickRay(&Orig,&Dir);

				m_lpMapMesh->DrawSelectPoly();
			}
		}
	}

	if(m_dwShowObscale)
	{
		m_lpMapMesh->RenderObscale();
	}
	//g_cGraphicsTool.DrawShadowVolumeMask();
	
	TCHAR str[60];
	wsprintf(str,"FPS %d",FPS);
	m_TimeLast = NowTime;
	{
		int NumOfVisibleRegion = m_lpMapMesh->m_VisibleRegionList.size();
		wsprintf(str,"VRegion %d",NumOfVisibleRegion);
		g_cGraphicsTool.DrawText(200,150,1,1,0xFFFF00FF,20,"宋体",str,0);

		
		wsprintf(str,"ReflectRegion %i",m_lpMapMesh->m_lpWaterReflectVisibleRegion.size());
		g_cGraphicsTool.DrawText(200,200,1,1,0xFFFF00FF,20,"宋体",str,0);
	}

	//	if (m_nFrameCount>0)
	//	{
	//	if(m_bTesting)
	//		{
//	wsprintf(str,"Frame Count %d ",m_nFrameCount);
//	g_cGraphicsTool.DrawText(300,100,1,1,0xFFFF00FF,20,"宋体",str,0);
	//		}
	//		else
	//		{
	//			sprintf(str,"Frames: %d FPS: %.1f ",m_nFrameCount,m_nFrameCount/30.0f);
	//			g_cGraphicsTool.DrawText(100,500,1,1,0xFFFF00FF,20,"宋体",str,0);
	//		}
	//	}

	SAFE_RENDER(m_lpSSFX);
	return S_OK;
}

HRESULT KSceneSceneEdit::Render()
{	
	//RenderWaterReflection();

	RenderScene();

	return S_OK;
}

HRESULT KSceneSceneEdit::Initialize(PVOID pSceneTable)
{
	K3DScene::Initialize(pSceneTable);

	g_cMeshTable.Get1NewXMesh((LPMODEL*)(&m_lpMoveGrid),7);
	m_lpMoveGrid->LoadMesh    ("MoveGrid");

	g_cMeshTable.Get1NewXMesh((LPMODEL*)(&m_lpRotateGrid),8);
	m_lpRotateGrid->LoadMesh    ("RotateGrid");

	g_cTextureTable.LoadFromFile(&m_GridTextureID,"Textures\\Grid.Tga");

	LPMODEL pMesh = NULL;
	
	{
		g_cMeshTable.Get1NewXMesh((LPMODEL*)(&m_lpSkyBox),MODELTYPE_SKYBOX);
		m_lpSkyBox->LoadMesh("Sky");

		m_lpSkyBox->GetSkyTexture(&m_lpTextureSky);
	}

	/*{
		g_cMeshTable.Get1NewXMesh((LPMODEL*)(&m_lpWater),MODELTYPE_WATER);
		m_lpWater->LoadMesh("Water");
	}*/

	{
		g_cMeshTable.Get1NewXMesh((LPMODEL*)(&m_lpMapMesh),MODELTYPE_TERRAIN);
		m_lpMapMesh->LoadMesh("");
		m_lpMapMesh->SetSceneEditor(this);

		m_nDestMapWidth  = m_lpMapMesh->m_Map_Width;
		m_nDestMapHeigth = m_lpMapMesh->m_Map_Height;
		//AddRenderMesh(m_lpMapMesh);
	}

	{	
		g_cMeshTable.Get1NewXMesh((LPMODEL*)(&m_lpSSFX),MODELTYPE_SCREENSFX);

		m_lpSSFX->LoadData("模型文件\\fog.ssfx");
		m_lpSSFX->LoadMaterial("模型文件\\fog.Mtl");
	}

	{
		//g_cMeshTable.Get1NewXMesh((LPMODEL*)(&m_lpTerrainLOD),MODELTYPE_TERRAINLOD);
		//m_lpTerrainLOD->LoadMesh("");
		//m_lpTerrainLOD->SetTerrainData(m_lpMapMesh);
		//AddRenderMesh(m_lpMapMesh);
	}

	g_cMeshTable.Get1NewXMesh(&m_lpLightFlare,MODELTYPE_LIGHTFLARE);
	m_lpLightFlare->LoadMesh("Flare");

	/*g_cMeshTable.Get1NewXMesh((LPMODEL*)(&m_lpMainTool),MODELTYPE_MAINTOOL);
	m_lpMainTool->LoadMesh("Main");
	m_lpMainTool->SetMap((KModelTerrain*)m_lpMapMesh,TRUE);
	m_lpMainTool->SetSceneEditor(this);	
	m_lpMainTool->m_bHide = TRUE;*/


	/*g_cMeshTable.Get1NewXMesh((LPMODEL*)(&m_lpObjectTool),MODELTYPE_OBJECTTABLETOOL);
	m_lpObjectTool->LoadMesh("ObjectTool");
	m_lpObjectTool->SetSceneEditor(this);*/

	/*g_cObjectTable.Get1NewObject(&m_lpInHandObject,0);
	m_lpInHandObject->AddMesh(m_lpMoveGrid->ID);
	m_lpInHandObject->m_vecMeshKeeper[0].m_dMeshID = 0;*/

	g_cObjectTable.Get1NewObject((LPOBJECT*)&m_lpGirlNo1,OBJECTTYPE_GIRLNO1);
	m_lpGirlNo1->Load();

	D3DXVECTOR3 Center = m_lpMapMesh->m_BBoxMap.GetCenter();
	//m_lpGirlNo1->m_Position.x = Center.x;
	//m_lpGirlNo1->m_Position.z = Center.z;
	m_Center = Center;

	GraphicsStruct::KCamera* pCamera = &g_GraphiceEngine.m_cCamera; 

	pCamera->Position  = Center+ D3DXVECTOR3(1000,1000,1000);
	pCamera->ViewPoint = Center;

	pCamera->SetCamera();

	m_lpMapMesh->FrameMove();

	WaterMapCreate();
	//////////////////////////////////////////////////////////////////////////
	//
	Acquire();

	PropertyBase::KDlg_PropertyDialog* pDialog = new PropertyBase::KDlg_PropertyDialog();
	pDialog->SetPropertyBase(this);

	pDialog->Create(IDD_PRODLG,NULL);
	pDialog->ShowWindow(FALSE);

	m_lpDlgPropertySet = pDialog;

	m_lpDialogPanel = new KSceneSceneEditorPanelDialog();
	m_lpDialogPanel->SetSceneEditor(this);

	m_lpDialogPanel->Create(m_lpDialogPanel->IDD);
	m_lpDialogPanel->ShowWindow(TRUE);
	//m_lpDialogPanel->CenterWindow();

	TCHAR abc[256];
	::SetCurrentDirectory(g_Def_AppDirectory);
	::GetCurrentDirectory(256,abc);

	return S_OK;
}


HRESULT KSceneSceneEdit::Edit_Select()
{
	return KSceneEditBase::Edit_Select();
}

void KSceneSceneEdit::ONLBUTTONUP(WPARAM wParam, LPARAM lParam)
{
	switch(m_dwState)
	{
	case EDITSTATE_TERRANEDIT:
		{
			KModelTerrain* pTerran = (KModelTerrain*) m_lpMapMesh;
			if ((pTerran)&&(pTerran->m_dwState))
			{
				pTerran->OnBushFinish();
			}
		}
	};

	KSceneEditBase::ONLBUTTONUP(wParam,lParam);
}

void KSceneSceneEdit::ONRBUTTONDOWN(WPARAM wParam, LPARAM lParam)
{
	if(!m_bEditing)
	{
		m_lpGirlNo1->Attack();
	}
	else
	{
		switch(m_dwState)
		{
		case EDITSTATE_PLACEOBJECT:
			{
				if(GetAsyncKeyState(VK_LCONTROL))
				{
					ScrollInHandObject(TRUE);
				}
				else
				{
					ScrollInHandObject(FALSE);
				}

				break;
			}
		}
	}
}

void KSceneSceneEdit::ONLBUTTONDOWN(WPARAM wParam, LPARAM lParam)
{
	if(m_bEditing)
	{
		switch(m_dwState)
		{
		case EDITSTATE_TERRANEDIT:
			{
				if (g_KeyState[VK_SPACE]==0)
				{
					KModelTerrain* pTerran = (KModelTerrain*) m_lpMapMesh;
					if ((pTerran)&&(pTerran->m_dwState))
					{
						pTerran->OnBushStart();

						pTerran->OnBush();

					}
				}
				break;
			}
		case EDITSTATE_PLACEOBJECT:
			{
				if (g_KeyState[VK_SPACE]==0)
				{
					Edit_PlaceObject();
				}
				break;
			}
		}
	}
	else
	{
		KGraphicsEngine* pEng = &g_GraphiceEngine;

		D3DXVECTOR3 Orig,Dir;
		pEng->GetPickRay(&Orig,&Dir);

		D3DXVECTOR3 Out(0,0,0);
		if(SUCCEEDED(m_lpMapMesh->GetMapCursorPosition(&Out,&Orig,&Dir)))
		{
			int nX =0,nY=0,nZ=0;
			PT_Scene2Logical(&nX,&nY,&nZ,Out);

			KPlayer* pPlayer = &(g_SO3World.m_ObjectManager.m_PlayerSet[CLIENT_PLAYER_INDEX]);
			if(pPlayer)
			{
			//	pPlayer->DoMove(nX,nY);

				m_listMoveDestPos.clear();
				m_listMoveDestPos.push_back(Out);
			}

		}
	}



	KSceneEditBase::ONLBUTTONDOWN(wParam,lParam);
}

void KSceneSceneEdit::ONMOUSEMOVE(WPARAM wParam, LPARAM lParam)
{
	KGraphicsEngine* pEng = &g_GraphiceEngine;

	D3DXVECTOR3 Orig,Dir;
	pEng->GetPickRay(&Orig,&Dir);
	D3DXVECTOR3 Out(0,0,0);
	if(SUCCEEDED(m_lpMapMesh->GetMapCursorPosition(&Out,&Orig,&Dir)))
	{
		m_listMoveDestPos.clear();
		m_listMoveDestPos.push_back(Out);
	}

	if(m_bEditing)
	{
		int fwKeys = wParam; 
		int xPos = LOWORD(lParam); 
		int yPos = HIWORD(lParam);

		if (fwKeys&MK_LBUTTON)
		{
			switch(m_dwState)
			{
			/*case EDITSTATE_TERRANEDIT:
				{
					KModelTerrain* pTerran = (KModelTerrain*) m_lpMapMesh;
					if ((pTerran)&&(pTerran->m_dwState))
					{
						pTerran->OnBush();
					}
					break;
				}*/
			case EDITSTATE_PLACEOBJECT:
				{
					m_bShowSelectionRect = FALSE;
					break;
				}
			}
		}
	}
	else
	{
		int fwKeys = wParam; 
		int xPos = LOWORD(lParam); 
		int yPos = HIWORD(lParam);

		if (fwKeys&MK_LBUTTON)
		{
			/*KPlayer* pPlayer = &(m_SO3World.m_PlayerSet[0]);
			if(pPlayer)
			{
				pPlayer->DoMove((int)(Out.x * 128.0f / 50),(int)(Out.z * 128.0f / 50));
			}*/
		}
		else if (fwKeys&MK_MBUTTON)
		{
			float X = 0.005f*((float)xPos-m_LastMousePos.x);
			float Y = 0.005f*((float)yPos-m_LastMousePos.y);
			
			m_CameraAngle.y += X;
			m_CameraAngle.x += Y;

			/*if (m_CameraAngle.x<D3DX_PI/24)
				m_CameraAngle.x = D3DX_PI/24;*/

			if (m_CameraAngle.x >D3DX_PI*3/7)
				m_CameraAngle.x = D3DX_PI*3/7;
			
			ComputeCamera();
		}

		m_LastMousePos.x = xPos;
		m_LastMousePos.y = yPos;
	}

	return KSceneEditBase::ONMOUSEMOVE(wParam,lParam);
}

void KSceneSceneEdit::ONMOUSEWHEEL(WPARAM wParam, LPARAM lParam)
{
	if(!m_bEditing)
	{
		int k = 0;
		k |=wParam;
		float S = 0;
		if (k<0)
			S = -0.1f;
		else 
			S = 0.1f;
		m_CameraDistance *= (1+S);
		ComputeCamera();
	}
	return KSceneEditBase::ONMOUSEWHEEL(wParam,lParam);
}

void KSceneSceneEdit::PutEntitytoTerrianRegion(KSceneEntity& Entity,BOOL bOptimize)
{
	//m_lpMapMesh->m_TerrainRegion.AddEntity(Entity);
	m_lpMapMesh->PutEntity2Region(Entity,bOptimize);
}

void KSceneSceneEdit::PutEntitytoRegion()
{
	/*m_lpMapMesh->ClearAllRegionEntity();

	list<KSceneEntity>::iterator i = m_RenderEntityList.begin();
	while (i!=m_RenderEntityList.end())
	{
		KSceneEntity Entity = *i;
		m_lpMapMesh->AddRegionEntity(Entity);
		i++;
	}*/
}


HRESULT KSceneSceneEdit::ToggleEditing()//切换编辑/测试模式
{
	m_bEditing = !m_bEditing;
	m_dwState = 0;


	g_pSoundSys->StopAllMusic();

	if(!m_bEditing)
	{
		m_lpMapMesh->CleanEntityOnRegion();
		m_lpMapMesh->ComputeMapBBox();
		//m_lpMapMesh->ComputeRegionsStandHeight();

		list<KSceneEntity>::iterator i = m_RenderEntityList.begin();
		while (i!=m_RenderEntityList.end())
		{
			KSceneEntity Entity = *i;
			PutEntitytoTerrianRegion(Entity,m_bOptimizeSmallEntity);
			i++;
		}

		TCHAR Name[256];
		wsprintf(Name,"%s\\Music\\TRACK02.WAV",g_Def_AppDirectory);
		//g_pSoundSys->PlayAmusic(Name,PlayLooping);

		//////////////////////////////////////////////////////////////////////////
		SetEditorData2LogicalData();

		int nX=0,nY=0,nZ=0;
		PT_Scene2Logical(&nX,&nY,&nZ,m_lpGirlNo1->m_Position);

		KPlayer* pPlayer = g_SO3World.NewPlayer(1);
		if (pPlayer)
		{
			pPlayer->m_nVelocityXY = 256;
			pPlayer->m_nMaxVelocityXY = 256;
			g_SO3World.AddPlayer(pPlayer, m_pScene,nX,nY);
		}
	}
	return S_OK;
}

/************************************************************************/
/* 将编辑器数据影射到游戏逻辑的数据                                     */
/************************************************************************/
// 编辑器需要告诉游戏逻辑的数据有：
// NPC的 坐标、类型（战斗NPC、BOSS、任务NPC）、编号（模板编号、BOSS表的编号、任务NPC表的编号）、等级（可以为0）、名字（可以为空）
// Doodad的 坐标、编号、等级、名字
BOOL KSceneSceneEdit::SetEditorData2LogicalData(void)
{
	BOOL bRetCode = FALSE;
	ENTITYLIST::iterator i;

	if (m_pScene != NULL)
	{
		//删除原来的地图
		bRetCode = g_SO3World.DeleteScene(m_pScene, TRUE);
		KGLOG_PROCESS_ERROR(bRetCode);

		m_pScene = NULL;
	}

	DWORD W = (DWORD)(m_lpMapMesh->m_BBoxMap.GetLength()/100.0F/REGION_GRID_WIDTH);
	DWORD H = (DWORD)(m_lpMapMesh->m_BBoxMap.GetWidth() /100.0F/REGION_GRID_HEIGHT);
	//创建新地图
	m_pScene = g_SO3World.NewScene(m_dwLogicalMapID, 1,W,H);
	KGLOG_PROCESS_ERROR(m_pScene != NULL);


	//影射单元格上的障碍信息和脚本ID
	for(DWORD i_Region =0; i_Region < m_lpMapMesh->m_vecRegions.size(); i_Region++)
	{
		KModelTerrain::LPREGION pRegion = m_lpMapMesh->m_vecRegions[i_Region];
		float BaseHeight = m_lpMapMesh->m_BBoxMap.A.y;

		for(int x = 0;x < 16; x++)
		{
			for(int z = 0;z < 16; z++)
			{/*
				KObstacle Obstacle;
				memset(&Obstacle, 0, sizeof(Obstacle));
				float y = 0;

				float XXX = (x * 2 + pRegion->m_nIndex_X * 32)*50.0F;
				float ZZZ = (z * 2 + pRegion->m_nIndex_Y * 32)*50.0F;

				m_lpMapMesh->GetMapHeight(&y, XXX,ZZZ, 0);

				if ((y - BaseHeight - 1) > 0)
					Obstacle.m_Altitude = ((int)(y - BaseHeight - 1) / ALTITUDE_UNIT);
				else
					Obstacle.m_Altitude = 0;
				Obstacle.m_PersonObstacle = 
					pRegion->m_Cells[x*16+z].m_Obstacle.m_PersonObstacle;

				int nX=0,nY=0,nZ=0;
				PT_Scene2Logical(&nX,&nY,&nZ,D3DXVECTOR3(XXX,0,ZZZ));
				m_pScene->SetObstacle(nX/CELL_LENGTH,nY/CELL_LENGTH,&Obstacle);

				DWORD Index = pRegion->m_Cells[x*16+z].m_dwScriptID;
				if(Index>0)
				{
					char szFileName[MAX_PATH];
					g_ExtractFileName(szFileName, (LPSTR)m_lpMapMesh->m_vecScriptFile[Index-1].c_str());
					m_pScene->SetScriptID(nX/CELL_LENGTH,nY/CELL_LENGTH, g_FileName2Id(szFileName));
				}*/

			}
		}
	}

	//Npc和Doodad
	i = m_RenderEntityList.begin();
	while(i!=m_RenderEntityList.end())
	{
		KSceneEntity Entity = *i;
		switch(Entity.Type)
		{
		case SCENEENTITY_OBJECT:
			{
				LPOBJECT pObject = (LPOBJECT) Entity.Pointer;
				if(pObject->m_dwType == OBJECTTYPE_NPC)
				{
					KRepNPC* pRepNpc = (KRepNPC*) pObject;
					if(pRepNpc->m_pNpcTemplate)
					{
						int X,Z;
						pRepNpc->GetLogicalPosition(&X, &Z, &(m_lpMapMesh->m_BBoxMap));

						static DWORD s_dwNpcID = 1;
						KNpc* pNpc = g_SO3World.NewNpc(s_dwNpcID++);
						KGLOG_PROCESS_ERROR(pNpc);

						pRepNpc->GetLogicalTemplateID(&(pNpc->m_dwTemplateID));
						pRepNpc->GetLogicalKind((int*)&(pNpc->m_Kind));
						pRepNpc->GetLogicalLevel(&(pNpc->m_nLevel));
						pRepNpc->GetLogicalName(pNpc->m_szName);
						pRepNpc->GetLogicalScriptID(&(pNpc->m_dwScriptID));

						bRetCode = g_SO3World.AddNpc(pNpc, m_pScene, X, Z);
						KGLOG_PROCESS_ERROR(bRetCode);
					}
				}
				break;
			}
		}
		i++;
	}
	return TRUE;

Exit0:

	return FALSE;
}

void KSceneSceneEdit::ONKEYDOWN(WPARAM wParam, LPARAM lParam)
{
	int nRetCode = false;

	if('P' == wParam)
	{
		ToggleEditing();//切换编辑/测试模式
	}

	if('T' == wParam)
	{
		ShowDialogLargeTerrian(TRUE);//召唤大地形编辑框
	}

	if('Y' == wParam)
	{
		ShowDialogGroundTextureMgr(TRUE);//
	}

	if('U' == wParam)
	{
		ShowDialogObjectMgr(TRUE);//
	}

	if('L' == wParam)
	{
		m_bLOD = !m_bLOD;
	}

	if('I' == wParam)
	{
		g_cGraphicsTool.m_bSunSpecular = !g_cGraphicsTool.m_bSunSpecular;
	}
	
	if(m_bEditing)
	{
		switch(wParam)
		{

		case 'O':
			m_lpMapMesh->UpdateVisibleRegion(TRUE);
			break;

		case 'D':
			Edit_DropSelectedObject();
			break;
		case 'M':
			Edit_EditMesh();
			break;
		case 219://[
			{
				KModelTerrain* pTerran = (KModelTerrain*) m_lpMapMesh;
				if (pTerran)
				{
					if (pTerran->m_BrushGroundTexture.m_nWidth>=1)
						pTerran->m_BrushGroundTexture.m_nWidth--;
				}
				break;
			}
		case 221://]
			{
				KModelTerrain* pTerran = (KModelTerrain*) m_lpMapMesh;
				if (pTerran)
				{
					if (pTerran->m_BrushGroundTexture.m_nWidth < 100)
						pTerran->m_BrushGroundTexture.m_nWidth++;
					if (pTerran->m_BrushGroundTexture.m_nWidth >= 100 )
						pTerran->m_BrushGroundTexture.m_nWidth = 100;
				}
				break;
			}
		case 189://=
			{
				KModelTerrain* pTerran = (KModelTerrain*) m_lpMapMesh;
				if (pTerran)
				{
					if (pTerran->m_BrushGroundTexture.m_nAlpha>=1)
						pTerran->m_BrushGroundTexture.m_nAlpha--;
				}
				break;
			}
		case 187://-
			{
				KModelTerrain* pTerran = (KModelTerrain*) m_lpMapMesh;
				if (pTerran)
				{
					if (pTerran->m_BrushGroundTexture.m_nAlpha < 100)
						pTerran->m_BrushGroundTexture.m_nAlpha++;
					if (pTerran->m_BrushGroundTexture.m_nAlpha >= 100 )
						pTerran->m_BrushGroundTexture.m_nAlpha = 100;
				}
				break;
			}
		case VK_F2:
			{
				KModelTerrain* pTerran = (KModelTerrain*) m_lpMapMesh;
				if (pTerran)
				{
					if (pTerran->m_FillMode != D3DFILL_WIREFRAME)
						pTerran->m_FillMode = D3DFILL_WIREFRAME;
					else
						pTerran->m_FillMode = D3DFILL_SOLID ;
				}
				break;
			}
		case VK_F3:
			{
				KModelTerrain* pTerran = (KModelTerrain*) m_lpMapMesh;
				if (pTerran)
				{
					if (pTerran->m_bTexture == TRUE)
						pTerran->m_bTexture = FALSE;
					else
						pTerran->m_bTexture = TRUE ;
				}
				break;
			}
		case VK_F5:
			{
				ENTITYLIST::iterator i = m_RenderEntityList.begin();
				while(i!=m_RenderEntityList.end())
				{
					KSceneEntity Entity = *i;
					if (Entity.Type==SCENEENTITY_OBJECT)
					{
						LPOBJECT pObject = (LPOBJECT) Entity.Pointer;
						pObject->ComputeBBox();
					}
					i++;
				}
				break;
			}
		case VK_TAB:
			{
				if(m_lpDialogPanel)
				{
					m_lpDialogPanel->ShowWindow(TRUE);
				}

				break;
			}
		}

		switch(m_dwState)
		{
		case EDITSTATE_PLACEOBJECT:
			{
				switch(wParam)
				{
				case VK_UP:
					{
						ScrollInHandObject(TRUE);
						return;
						break;
					}
				case VK_DOWN:
					{
						ScrollInHandObject(FALSE);
						return;
						break;
					}
				}

				break;
			}
		}
	}
	else
	{
		switch(wParam)
		{
		case VK_F1:
			{
				m_CameraDistance = 350;
				m_CameraAngle = D3DXVECTOR3(3.14f/9,-3.14f/4,0);
				break;
			}
		case VK_F2:
			{
				m_CameraDistance = 800;
				m_CameraAngle = D3DXVECTOR3(3.14f/8,-3.14f/4,0);
				break;
			}
		case VK_F3:
			{
				m_CameraDistance = 2000;
				m_CameraAngle = D3DXVECTOR3(3.14f/6,-3.14f/4,0);
				break;
			}
		case 'M':
			{
				m_lpGirlNo1->m_bRiding = !m_lpGirlNo1->m_bRiding;
				break;
			}
		case 'T':
			{
				if (m_bTesting==FALSE)
				{
					m_bTesting = TRUE;
					m_dwTestTimeStart = timeGetTime();
					m_nFrameCount = 0;
				}
				break;
			}
		case 'N':
			{
				m_lpGirlNo1->NextCharacter();
				break;
			}
		case 'R':
			{
				m_lpGirlNo1->ToggleRun();
				break;
			}
		case VK_ADD:
			{
				m_lpGirlNo1->m_fSpeed += 1;
				if(m_lpGirlNo1->m_fSpeed>100)
					m_lpGirlNo1->m_fSpeed = 100;
				break;
			}
		case VK_SUBTRACT:
			{
				m_lpGirlNo1->m_fSpeed -= 1;
				if(m_lpGirlNo1->m_fSpeed<1)
					m_lpGirlNo1->m_fSpeed = 1;
				break;
			}
		case VK_PAUSE:
			{
				if(m_TimeControl.m_fSpeed>0.5f)
				{
					m_TimeControl.SetDestSpeed(0,0.3f);
				}
				else
				{
					m_TimeControl.SetDestSpeed(1,0.3f);
				}
				break;
			}
		}
	}
	KSceneEditBase::ONKEYDOWN(wParam,lParam);
}

HRESULT KSceneSceneEdit::SaveObjects(FILE *pFile)
{
	DWORD ObjectCount = 0;
	DWORD ObjectNPC = 0;
	ENTITYLIST::iterator i = m_RenderEntityList.begin();
	while(i!=m_RenderEntityList.end())
	{
		KSceneEntity Entity = *i;
		switch(Entity.Type)
		{
		case SCENEENTITY_OBJECT:
			{
				LPOBJECT pObject = (LPOBJECT) Entity.Pointer;
				if (pObject->m_dwType==OBJECT_DEFAULT)
				{
					ObjectCount++;
				}
				else if(pObject->m_dwType == OBJECTTYPE_NPC)
				{
					ObjectNPC++;
				}
				break;
			}
		case SCENEENTITY_MODEL:
			{
				LPMODEL pMesh = (LPMODEL) Entity.Pointer;
				int s = 0;

				break;
			}
		}
		i++;
	}

	fwrite(&ObjectCount,sizeof(DWORD),1,pFile);

	i = m_RenderEntityList.begin();
	while(i!=m_RenderEntityList.end())
	{
		KSceneEntity Entity = *i;
		switch(Entity.Type)
		{
		case SCENEENTITY_OBJECT:
			{
				LPOBJECT pObject = (LPOBJECT) Entity.Pointer;
				if (pObject->m_dwType==OBJECT_DEFAULT)
				{
					if(_strcmpi(pObject->m_FileName.c_str(),"")!=0)
					{
						DWORD Size = 1;
						fwrite(&Size,sizeof(DWORD),1,pFile);

						fwrite(&pObject->m_Scaling ,sizeof(D3DXVECTOR3),1,pFile);
						fwrite(&pObject->m_Rotation,sizeof(D3DXVECTOR3),1,pFile);
						fwrite(&pObject->m_Position,sizeof(D3DXVECTOR3),1,pFile);

						TCHAR Name[256];
						wsprintf(Name,"%s",pObject->m_FileName.c_str());
						fwrite(Name,sizeof(TCHAR),256,pFile);
						fwrite(Name,sizeof(TCHAR),256,pFile);
						fwrite(Name,sizeof(TCHAR),256,pFile);
					}
					else
					{
						DWORD Size = pObject->m_vecMeshKeeper.size();
						fwrite(&Size,sizeof(DWORD),1,pFile);

						fwrite(&pObject->m_Scaling ,sizeof(D3DXVECTOR3),1,pFile);
						fwrite(&pObject->m_Rotation,sizeof(D3DXVECTOR3),1,pFile);
						fwrite(&pObject->m_Position,sizeof(D3DXVECTOR3),1,pFile);

						for(DWORD K=0;K<Size;K++)
						{
							DWORD XMeshID = pObject->m_vecMeshKeeper[K].m_dMeshID;
							LPMODEL pMesh = NULL;
							if (SUCCEEDED(g_cMeshTable.GetXMesh(&pMesh,XMeshID)))
							{
								fwrite(pMesh->m_szMeshFileName,sizeof(TCHAR),256,pFile);
								fwrite(pMesh->m_szMtlFileName,sizeof(TCHAR),256,pFile);
								fwrite(pMesh->m_szAniFileName,sizeof(TCHAR),256,pFile);
							}
						}
					}

				}
			}
			break;
		}
		i++;
	}

	fwrite(&ObjectNPC,sizeof(DWORD),1,pFile);
	i = m_RenderEntityList.begin();
	while(i!=m_RenderEntityList.end())
	{
		KSceneEntity Entity = *i;
		switch(Entity.Type)
		{
		case SCENEENTITY_OBJECT:
			{
				LPOBJECT pObject = (LPOBJECT) Entity.Pointer;
				if (pObject->m_dwType==OBJECTTYPE_NPC)
				{
					fwrite(&pObject->m_Scaling ,sizeof(D3DXVECTOR3),1,pFile);
					fwrite(&pObject->m_Rotation,sizeof(D3DXVECTOR3),1,pFile);
					fwrite(&pObject->m_Position,sizeof(D3DXVECTOR3),1,pFile);

					KRepNPC* pNpc = (KRepNPC*) pObject;

					TCHAR Name[256];
					wsprintf(Name,"%s",pNpc->m_scIniFileName.c_str());
					fwrite(Name,sizeof(TCHAR),256,pFile);
				}
			}
			break;
		}
		i++;
	}
	return S_OK;
}

HRESULT KSceneSceneEdit::ComputeCamera()
{
	//float K = m_lpGirlNo1->m_Rotation.x - m_CameraAngle.y;
	//if(K>D3DX_PI)
	//{
	//	m_CameraAngle.y += D3DX_PI*2;
	//}
	//else if(K<-D3DX_PI)
	//{
	//	m_CameraAngle.y -= D3DX_PI*2;
	//}

	//K = m_lpGirlNo1->m_Rotation.x - m_CameraAngle.y;
	//if(abs(K)>0.2F)
	//{
	//	if(K<0)
	//		m_CameraAngle.y -= m_TimeControl.GetTinyTime()*0.0005F;
	//	else
	//		m_CameraAngle.y += m_TimeControl.GetTinyTime()*0.0005F;
	//}

	m_CameraLookAt = D3DXVECTOR3(0,140-m_CameraDistance*0.1f,0)+m_lpGirlNo1->m_Position;

	float H = m_CameraDistance * sinf(m_CameraAngle.x);
	float L = m_CameraDistance * cosf(m_CameraAngle.x);

	/*if(abs(m_lpGirlNo1->m_Rotation.x - m_CameraAngle.y)>D3DX_PI*0.5f)
	{
		m_CameraAngle.y += (m_lpGirlNo1->m_Rotation.x - m_CameraAngle.y)*0.01f;
	}*/

	m_CameraPosition = m_CameraLookAt + 
		D3DXVECTOR3(L*sinf(m_CameraAngle.y),H,L*cosf(m_CameraAngle.y));
	//m_CameraPosition = m_CameraLookAt + D3DXVECTOR3(L*sinf(m_lpGirlNo1->m_Rotation.x),H,L*cosf(m_lpGirlNo1->m_Rotation.x));
	return S_OK;
}

HRESULT KSceneSceneEdit::LoadObjects(FILE *pFile)
{
	DWORD Time_Find = 0;
	DWORD Time_Load = 0;
	DWORD Time;

	DWORD ObjectCount = 0;
	DWORD ObjectNPC = 0;
	fread(&ObjectCount,sizeof(DWORD),1,pFile);
	for(DWORD K=0;K<ObjectCount;K++)
	{
		if(K==432)
		{
			int sss= 0;
		}
		DWORD MeshCount = 0;
		fread(&MeshCount,sizeof(DWORD),1,pFile);
		LPOBJECT pObject = NULL;

		D3DXVECTOR3 Scaling;
		D3DXVECTOR3 Rotation;
		D3DXVECTOR3 Position;

		fread(&Scaling ,sizeof(D3DXVECTOR3),1,pFile);
		fread(&Rotation,sizeof(D3DXVECTOR3),1,pFile);
		fread(&Position,sizeof(D3DXVECTOR3),1,pFile);
		
		for(DWORD M=0;M<MeshCount;M++)
		{
			TCHAR MeshFile[256];
			TCHAR MtlFile[256];
			TCHAR AniFile[256];
			fread(MeshFile,sizeof(TCHAR),256,pFile);
			fread( MtlFile,sizeof(TCHAR),256,pFile);
			fread( AniFile,sizeof(TCHAR),256,pFile);
			
			TCHAR path_buffer[256];
			TCHAR drive[256];
			TCHAR dir[256];
			TCHAR fname[256];
			TCHAR ext[256];

			wsprintf(path_buffer,"%s",MeshFile);
			_splitpath( path_buffer, drive, dir, fname, ext );

			TCHAR FileExtName[256];
			TCHAR Name[256];

			wsprintf(FileExtName,"%s%s",fname,ext);

			if(_strcmpi(ext,".Object")==0)
			{
				if(FAILED(g_cObjectTable.LoadbjectFromFile(&pObject,FileExtName)))
				{
					//MessageBox(NULL,FileExtName,"找不到该文件",0);
				}
				break;
			}
			else
			{
				DWORD NewXMeshID = 0;

				Time = timeGetTime();
				if(SUCCEEDED(g_cFileFinder.FindFile(Name,FileExtName)))
				{
					Time_Find += timeGetTime() - Time;

					Time = timeGetTime();
					if (SUCCEEDED(g_cMeshTable.LoadFromFile(&NewXMeshID,Name)))
					{
						LPMODEL pMesh = NULL;
						g_cMeshTable.GetXMesh(&pMesh,NewXMeshID);

						if (strcmp(MtlFile,"")!=0)
						{
							wsprintf(path_buffer,"%s",MtlFile);
							_splitpath( path_buffer, drive, dir, fname, ext );
							wsprintf(FileExtName,"%s%s",fname,ext);

							if (SUCCEEDED(g_cFileFinder.FindFile(Name,FileExtName)))
							{
								pMesh->LoadMaterial(Name);
							}
						}
						Time_Load += timeGetTime() - Time;

						if(!pObject)
						{
							if (FAILED(g_cObjectTable.Get1NewObject(&pObject,0)))
							{
								return E_FAIL;
							}
						}

						pObject->AddMesh(NewXMeshID);

						TCHAR Title[256];
						sprintf(Title,"%d -- %d 物件载入",K,ObjectCount);
						::SetWindowText(g_hBaseWnd,Title);
					}			
				}
				else
				{
					MessageBox(g_hBaseWnd,MeshFile,"找不到模型文件",0);
				}
			}
		}

		if(pObject)
		{
			pObject->m_dwScrabbleOption =0;

			AddRenderObject(pObject);
			AddSelectableObject(pObject);

			pObject->m_Scaling = Scaling;
			pObject->m_Rotation = Rotation;
			pObject->m_Position = Position;

			pObject->ComputeBBox();
		}

	}

	fread(&ObjectNPC,sizeof(DWORD),1,pFile);
	
	if(ObjectNPC==0xFFFFFFFF)
	{
		return S_OK;
	}
	for(DWORD K=0;K<ObjectNPC;K++)
	{
		KRepNPC* pNPC = NULL;
		if (FAILED(g_cObjectTable.Get1NewObject((LPOBJECT*)(&pNPC),OBJECTTYPE_NPC)))
		{
			return E_FAIL;
		};
		fread(&pNPC->m_Scaling ,sizeof(D3DXVECTOR3),1,pFile);
		fread(&pNPC->m_Rotation,sizeof(D3DXVECTOR3),1,pFile);
		fread(&pNPC->m_Position,sizeof(D3DXVECTOR3),1,pFile);
		TCHAR Name[256];
		fread(Name,sizeof(TCHAR),256,pFile);
		
		if(_strcmpi(Name,"")==0)
			continue;

		//TCHAR InName[256];
		TCHAR OutName[256];

		if(SUCCEEDED(g_cFileFinder.FindFile(OutName,Name)))
		{
			IIniFile* Ini = g_OpenIniFile(OutName);
			if(Ini)
			{
				if(SUCCEEDED(pNPC->LoadByIni(Ini,OutName)))
				{
					AddRenderObject(pNPC);
					AddSelectableObject(pNPC);
				}
			}
			Ini->Release();
		}
		
	}

	TCHAR Title[256];
	sprintf(Title,"Find %d Load %d Object Loaded",Time_Find,Time_Load);
	//MessageBox(g_hBaseWnd,Title,"时间",0);

	return S_OK;
}

HRESULT KSceneSceneEdit::Edit_DropSelectedObject()
{
	K3DScene::ENTITYLIST::iterator i = m_SelectedList.begin();
	while(i!=m_SelectedList.end())
	{
		KSceneEntity Entity = *i;
		switch(Entity.Type)
		{
		case SCENEENTITY_OBJECT:
			{
				LPOBJECT pObject = (LPOBJECT) Entity.Pointer;
				D3DXVECTOR3 Pos = pObject->m_Position;
				float Height = 0;
				if (SUCCEEDED(m_lpMapMesh->GetMapHeight(&Height,Pos.x,Pos.z,1)))
				{
					Pos.y = Height;
					pObject->Edit_Move(&Pos);
				}
			}
			break;
		}
		i++;
	}
	return S_OK;
}

HRESULT KSceneSceneEdit::Edit_Cut()
{
	return KSceneEditBase::Edit_Cut();
	
	/*RecordLastOperation_Cut();
	
	K3DScene::ENTITYLIST::iterator i = m_SelectedList.begin();

	m_ClipBoardList.clear();

	while(i!=m_SelectedList.end())
	{
		KSceneEntity Entity = *i;
		
		m_ClipBoardList.push_back(Entity);

		switch(Entity.Type)
		{
		case SCENEENTITY_OBJECT:
			{
				LPOBJECT pObject = (LPOBJECT) Entity.Pointer;
				RemoveRenderObject(pObject);
				RemoveSelectableObject(pObject);
				break;
			}
		}
		
		i++;

		m_SelectedList.remove(Entity);
	}*/

	return S_OK;
}


HRESULT KSceneSceneEdit::Edit_Paste()
{
	return KSceneEditBase::Edit_Paste();

	/*K3DScene::ENTITYLIST::iterator i = m_ClipBoardList.begin();
	if (i!=m_ClipBoardList.end())
	{
		m_SelectedList.clear();
	}
	while(i!=m_ClipBoardList.end())
	{
		KSceneEntity Entity = *i;
		if (Entity.Type == SCENEENTITY_OBJECT)
		{
			LPOBJECT pObject = (LPOBJECT) Entity.Pointer;
			if (CheckRenderObject(pObject))
			{
				g_cObjectTable.CopyObject(&pObject,pObject);
			}
			AddRenderObject(pObject);
			AddSelectableObject(pObject);
			AddSelectedObject(pObject);
			i++;
			//m_ClipBoardList.remove(pEntity);
			//SAFE_DELETE(pEntity);
			continue;
		}
		i++;
	}*/
	return S_OK;
}

HRESULT KSceneSceneEdit::Edit_Copy()
{
	return KSceneEditBase::Edit_Copy();

	/*K3DScene::ENTITYLIST::iterator i = m_SelectedList.begin();

	m_ClipBoardList.clear();

	while(i!=m_SelectedList.end())
	{
		KSceneEntity Entity = *i;
		
		m_ClipBoardList.push_back(Entity);
		
		i++;
	}

	return S_OK;*/
}

HRESULT KSceneSceneEdit::Edit_Undo()
{
	m_lpMapMesh->Undo();

	KSceneEditBase::Edit_Undo();
	return S_OK;
}


HRESULT KSceneSceneEdit::Edit_PlaceObject()
{
	if(!m_lpInHandObject)
		return E_FAIL;

	_SceneOperatorPlaceObject* pOperator = new _SceneOperatorPlaceObject;

	m_vecOperation.push_back(pOperator);

	if(m_bEnablePlaceArray)
	{
		float W = m_lpMapMesh->m_SelectionPoly.m_XZRect.Width;
		float H = m_lpMapMesh->m_SelectionPoly.m_XZRect.Height;

		int X = (int) ( W / m_lpInHandObject->m_BBox.GetLength());
		int Z = (int) ( H / m_lpInHandObject->m_BBox.GetWidth());

		int Count = (int) (X * Z / m_fPlaceArrayDest / m_fPlaceArrayDest);
		if(Count<=0)
			Count = 1;
		if(Count>=200)
			Count = 200;

		LPOBJECT pObject = NULL;
		for(int i=0;i<Count;i++)
		{
			if(SUCCEEDED(m_lpInHandObject->Copy(&pObject)))
			{
				float XX = (W*0.5f) * (rand()%200-100) * 0.01f;
				float ZZ = (H*0.5f) * (rand()%200-100) * 0.01f;

				float YY = 0;
				m_lpMapMesh->GetMapHeight(&YY,pObject->m_Position.x+XX,pObject->m_Position.z+ZZ,1);

				pObject->m_Position += D3DXVECTOR3(XX,YY,ZZ);

				pObject->m_dwScrabbleOption = 0;
				AddRenderObject(pObject);
				AddSelectableObject(pObject);

				pOperator->AddPlacedObject(pObject);

				if(m_bEnableRandomRotation)
				{
					float W = m_fRotationUP - m_fRotationDN;
					float K = (rand()%100)*0.01f * W + m_fRotationDN;
					m_lpInHandObject->m_Rotation.x = K;
				}
				if(m_bEnableRandomScale)
				{
					float W = m_fScaleUP - m_fScaleDN;
					float K = (rand()%100)*0.01f * W + m_fScaleDN;
					m_lpInHandObject->m_Scaling = D3DXVECTOR3(K,K,K);
				}
			}
		}
	}
	else
	{
		LPOBJECT pObject = NULL;

		if(SUCCEEDED(m_lpInHandObject->Copy(&pObject)))
		{
			pObject->m_dwScrabbleOption = 0;
			AddRenderObject(pObject);
			AddSelectableObject(pObject);

			pOperator->AddPlacedObject(pObject);

			if(m_bEnableRandomRotation)
			{
				float W = m_fRotationUP - m_fRotationDN;
				float K = (rand()%100)*0.01f * W + m_fRotationDN;
				m_lpInHandObject->m_Rotation.x = K;
			}
			if(m_bEnableRandomScale)
			{
				float W = m_fScaleUP - m_fScaleDN;
				float K = (rand()%100)*0.01f * W + m_fScaleDN;
				m_lpInHandObject->m_Scaling = D3DXVECTOR3(K,K,K);
			}
		}
	}	

	return S_OK;
}


HRESULT KSceneSceneEdit::Edit_EditMesh(void)
{
	m_bMeshEdit = TRUE;
	::PostMessage(NULL,ID_MESHEDIT,0,0);
	return S_OK;
}

HRESULT KSceneSceneEdit::SaveScene(LPSTR pFileName)
{
	//////////////////////////////////////////////////////////////////////////
	//前期准备
	GroupObjects();//建立物件分组和索引

	m_lpMapMesh->CleanEntityOnRegion();//将物件放置到地图区域上
	m_lpMapMesh->ComputeMapBBox();
	list<KSceneEntity>::iterator i = m_RenderEntityList.begin();
	while (i!=m_RenderEntityList.end())
	{
		KSceneEntity Entity = *i;
		PutEntitytoTerrianRegion(Entity,m_bOptimizeSmallEntity);
		i++;
	}
	//////////////////////////////////////////////////////////////////////////
	//
	FILE* pFile = fopen(pFileName,"wb");
	if (!pFile)
		return E_FAIL;

	_SceneHead SceneHead;

	DWORD   BlockEnd  = 0xFFFFFFFF;  
	DWORD   Pos_Start = ftell(pFile);
	fwrite(&SceneHead,sizeof(_SceneHead),1,pFile);
	//////////////////////////////////////////////////////////////////////////
	{
		SceneHead.MapsBlock = ftell(pFile) - Pos_Start;
		DWORD NumMap = 1;
		fwrite(&NumMap,sizeof(DWORD),1,pFile);

		m_lpMapMesh->SaveMap(pFile);

		fwrite(&BlockEnd,sizeof(DWORD),1,pFile);
	}

	SceneHead.BlockLength = ftell(pFile) - Pos_Start;
	fseek(pFile,Pos_Start,SEEK_SET);
	fwrite(&SceneHead,sizeof(_SceneHead),1,pFile);
	fclose(pFile);

	TCHAR str[256];
	TCHAR path_buffer[256];
	TCHAR drive[256];
	TCHAR dir[256];
	TCHAR fname[256];
	TCHAR ext[256];

	wsprintf(path_buffer,"%s",pFileName);
	_splitpath( path_buffer, drive, dir, fname, ext );

	wsprintf(str,"%s%s%s.Obj",drive,dir,fname);
	pFile = fopen(str,"wb");
	if (!pFile)
		return E_FAIL;
	SaveObjects(pFile);
	fwrite(&BlockEnd,sizeof(DWORD),1,pFile);
	fclose(pFile);

//游戏逻辑用的记录文件
	SetCurrentDirectory(g_Def_AppDirectory);
	BOOL bRetCode = FALSE;

	bRetCode = SetEditorData2LogicalData();
	KGLOG_PROCESS_ERROR(bRetCode);

	if (!m_bEditing)
	{
		KPlayer* pPlayer = g_SO3World.NewPlayer(1);
		if (pPlayer)
		{
			pPlayer->m_nVelocityXY = 256;
			pPlayer->m_nMaxVelocityXY = 256;
			bRetCode = g_SO3World.AddPlayer(pPlayer, m_pScene, 
				(int)(m_lpGirlNo1->m_Position.x - m_lpMapMesh->m_BBoxMap.A.x) * 256 / 100,
				(int)(m_lpGirlNo1->m_Position.z - m_lpMapMesh->m_BBoxMap.A.z) * 256 / 100);
			KGLOG_PROCESS_ERROR(pPlayer);
		}
	}

	bRetCode = m_pScene->Save(fname);
	KGLOG_PROCESS_ERROR(bRetCode);

	return E_NOTIMPL;

Exit0:
	::MessageBox(NULL, "存储失败！", NULL, 0);

	return E_FAIL;
}

HRESULT KSceneSceneEdit::LoadScene(LPSTR pFileName)
{
	FILE* pFile = fopen(pFileName,"rb");
	if (!pFile)
		return E_FAIL;

	_SceneHead SceneHead;
	BOOL nNeedLoadObjFile = TRUE;

	DWORD   BlockEnd  = 0xFFFFFFFF;  
	DWORD   Pos_Start = ftell(pFile);
	fread(&SceneHead,sizeof(_SceneHead),1,pFile);
	//////////////////////////////////////////////////////////////////////////
	if(SceneHead.MapsBlock)
	{
		fseek(pFile,Pos_Start+SceneHead.MapsBlock,SEEK_SET);
		DWORD NumMap = 0;
		fread(&NumMap,sizeof(DWORD),1,pFile);

		m_lpMapMesh->LoadMap(pFile,&nNeedLoadObjFile);
		fread(&BlockEnd,sizeof(DWORD),1,pFile);
	}
	fclose(pFile);

	D3DXVECTOR3 Center = m_lpMapMesh->m_BBoxMap.GetCenter();
	//m_lpGirlNo1->m_Position.x = Center.x;
	//m_lpGirlNo1->m_Position.z = Center.z;
	m_Center = Center;

	/*if(SceneHead.ObjectBlock)
	{
		fseek(pFile,Pos_Start+SceneHead.ObjectBlock,SEEK_SET);
		LoadObjects(pFile);
		fread(&BlockEnd,sizeof(DWORD),1,pFile);
	}*/

	if(nNeedLoadObjFile)
	{
		TCHAR str[256];
		TCHAR path_buffer[256];
		TCHAR drive[256];
		TCHAR dir[256];
		TCHAR fname[256];
		TCHAR ext[256];

		wsprintf(path_buffer,"%s",pFileName);
		_splitpath( path_buffer, drive, dir, fname, ext );

		wsprintf(str,"%s%s%s.Obj",drive,dir,fname);
		pFile = fopen(str,"rb");
		if (!pFile)
			return E_FAIL;
		LoadObjects(pFile);
		fread(&BlockEnd,sizeof(DWORD),1,pFile);
		fclose(pFile);
	}

	return S_OK;
}

HRESULT KSceneSceneEdit::CheckUpMapSize(int& Size)
{
	int K = 1;
	while(Size>K)
	{
		K *= 2;
	}
	Size = K;
	return S_OK;
}

HRESULT KSceneSceneEdit::Acquire()
{
	if(!m_bNeedReAcquire)
		return S_OK;

	KSceneEditBase::Acquire();

	int PageIndex = this->AddPage("场景");

	PropertyBase::_PropertyPage* pPage = this->FindPage(PageIndex);
	if(pPage)
	{
		PropertyBase::_PropertyGroup Group2(0,"场景设置",FALSE);
		int index_Group = pPage->AddGroup(Group2);
		PropertyBase::_PropertyGroup* pGroup = pPage->FindGroup(index_Group);
		//pGroup->AddPropertyColor("Water Up Color",&m_lpMapMesh->m_MaterialWaterUp.Diffuse);
		//pGroup->AddPropertyFloat("Water Up Alpha",&m_lpMapMesh->m_MaterialWaterUp.Diffuse.a,0,1.0f);

		pGroup->AddPropertyColor("Water Down Color",&m_lpMapMesh->m_ColorDownWater);
		//pGroup->AddPropertyFloat("Water Down Alpha",&m_lpMapMesh->m_MaterialWaterDown.Diffuse.a,0,1.0f);
		pGroup->AddPropertyFloat("太阳高度",&m_fSunHeight,0,10.0f);

		pGroup->AddPropertyColor("天空颜色",&m_lpMapMesh->m_ColorSceneSky);
		pGroup->AddPropertyColor("地面颜色",&m_lpMapMesh->m_ColorSceneGround);

		pGroup->AddPropertyFloat("天光照明系数",&m_lpMapMesh->m_fHemScale,0,2);
	}
	PageIndex = this->AddPage("物品");
	pPage = this->FindPage(PageIndex);
	if(pPage)
	{
		PropertyBase::_PropertyGroup Group(0,"放置物品",FALSE);
		int index_Group = pPage->AddGroup(Group);
		PropertyBase::_PropertyGroup* pGroup = pPage->FindGroup(index_Group);

		pGroup->AddPropertySceneObjectTable("Resize",this);
	}


	m_bNeedReAcquire = TRUE;
	return S_OK;
}

//设置放置物件替换的目录
HRESULT KSceneSceneEdit::SetInHandObjectFileDirectory(LPCSTR pDir,LPCSTR pCurFileName)
{
	m_vecInHandObjectName.clear();
	
	TCHAR path_buffer[256];
	TCHAR drive[256];
	TCHAR dir[256];
	TCHAR fname[256];
	TCHAR ext[256];

	WIN32_FIND_DATA Find;
	TCHAR str[256];
	wsprintf(str,"%s\\*.*",pDir);

	HANDLE hFind = FindFirstFile(str,&Find);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		wsprintf(path_buffer,"%s",Find.cFileName);
		_splitpath( path_buffer, drive, dir, fname, ext );
		wsprintf(str,"%s%s",fname,ext);

		if((_strcmpi(path_buffer,".")!=0)&&(_strcmpi(path_buffer,"..")!=0))
		{
			wsprintf(str,"%s%s",pDir,path_buffer);
			DWORD Attrib = GetFileAttributes(str);

			if((Attrib!=0xFFFFFFFF)&&(Attrib&FILE_ATTRIBUTE_DIRECTORY))
			{
				;//do nothing;
			}
			else if(_strcmpi(ext,".Mesh")==0)
			{
				m_vecInHandObjectName.push_back(path_buffer);
				if(_strcmpi(str,pCurFileName)==0)
				{
					m_nCurInHandObjectIndex = m_vecInHandObjectName.size() - 1;
				}
			}
		}

		while(FindNextFile(hFind,&Find))
		{
			wsprintf(path_buffer,"%s",Find.cFileName);
			_splitpath( path_buffer, drive, dir, fname, ext );
			wsprintf(str,"%s%s",fname,ext);
			if((_strcmpi(path_buffer,".")!=0)&&(_strcmpi(path_buffer,"..")!=0))
			{
				wsprintf(str,"%s%s",pDir,path_buffer);
				DWORD Attrib = GetFileAttributes(str);

				if((Attrib!=0xFFFFFFFF)&&(Attrib&FILE_ATTRIBUTE_DIRECTORY))
				{
					;//do nothing;
				}
				else if(_strcmpi(ext,".Mesh")==0)
				{
					TCHAR FileName[256];
					wsprintf(FileName,"%s%s%s",pDir,fname,ext);
					m_vecInHandObjectName.push_back(FileName);

					m_nCurInHandObjectIndex = m_vecInHandObjectName.size() - 1;
				}
			}
		}
		FindClose(hFind);

	}
	return S_OK;
}

HRESULT KSceneSceneEdit::ScrollInHandObject(BOOL m_bUp)
{
	if(m_nCurInHandObjectIndex==-1)
		return S_OK;
	
	if(m_bUp)
	{
		m_nCurInHandObjectIndex++;
	}
	else
	{
		m_nCurInHandObjectIndex--;
	}

	if(m_nCurInHandObjectIndex<0)
		m_nCurInHandObjectIndex = m_vecInHandObjectName.size() -1;
	if(m_nCurInHandObjectIndex>=(int)m_vecInHandObjectName.size())
		m_nCurInHandObjectIndex = 0;

	string* pFileName = &m_vecInHandObjectName[m_nCurInHandObjectIndex];

	DWORD MeshID = 0;
	LPMODEL pModel = NULL;
	if(SUCCEEDED(g_cMeshTable.LoadFromFile(&MeshID,(LPSTR)pFileName->c_str())))
	{
		if(SUCCEEDED(g_cMeshTable.GetXMesh(&pModel,MeshID)))
		{
			TCHAR path_buffer[256];
			TCHAR drive[256];
			TCHAR dir[256];
			TCHAR fname[256];
			TCHAR ext[256];
			TCHAR str[256];

			wsprintf(path_buffer,"%s",pFileName->c_str());
			_splitpath( path_buffer, drive, dir, fname, ext );
			wsprintf(str,"%s%s%s.Mtl",drive,dir,fname);

			pModel->LoadMaterial(str);
		}
		if(m_lpInHandObject)
		{
			m_lpInHandObject->m_vecMeshKeeper[0].m_dMeshID = MeshID;
		}
	}

	return S_OK;
}

HRESULT KSceneSceneEdit::OnNewScence()
{
	KSceneSceneEditorDialogNewMapWizard Dlg;
	Dlg.SetSceneEditor(this);
	
	if(IDOK==Dlg.DoModal())
	{
		m_lpMapMesh->ResizeMapSize(Dlg.m_dwWidth,Dlg.m_dwHeight);

		m_pScene = NULL;

		D3DXVECTOR3 Center = m_lpMapMesh->m_BBoxMap.GetCenter();
		//m_lpGirlNo1->m_Position.x = Center.x;
		//m_lpGirlNo1->m_Position.z = Center.z;
		m_Center = Center;

	}

	return S_OK;
}

HRESULT KSceneSceneEdit::Edit_GroupSelectedEntity()
{
	KSceneEntityGroup Group;
	

	m_vecEntityGroup.push_back(Group);
	KSceneEntityGroup* pGroup = &m_vecEntityGroup[m_vecEntityGroup.size()-1];
	pGroup->m_ListEntity = m_SelectedList;
	pGroup->ComputeBBox();

	Edit_Cut();

	KSceneEntity Entity(SCENEENTITY_GROUP,
		(PVOID)(m_vecEntityGroup.size()-1),
		this);

	//m_SelectedList.clear();
	
	/*{
		KSceneEntityGroup* pGroup0 = NULL;
		KSceneEntityGroup* pGroup1 = NULL;

		if(m_vecEntityGroup.size()>0)
		{
			pGroup0 = &m_vecEntityGroup[0];
		}
		if(m_vecEntityGroup.size()>1)
		{
			pGroup1 = &m_vecEntityGroup[1];
		}

		int s = 0;
	}*/

	m_RenderEntityList.push_back(Entity);
	m_SelectableList.push_back(Entity);
	m_SelectedList.push_back(Entity);

	return S_OK;
}

HRESULT KSceneSceneEdit::Edit_UnGroupSelectedEntity()
{
	KSceneEntity Entity;
	if(SUCCEEDED(GetFirstSelectedEntity(Entity)))
	{
		if(Entity.Type==SCENEENTITY_GROUP)
		{
			DWORD Index = (DWORD) Entity.Pointer;
			KSceneEntityGroup* pGroup = &m_vecEntityGroup[Index] ;

			list<KSceneEntity>::iterator i=pGroup->m_ListEntity.begin();
			while(i!=pGroup->m_ListEntity.end())
			{
				KSceneEntity En = *i;

				En.Move(pGroup->m_Position);

				m_RenderEntityList.push_back(En);
				m_SelectableList.push_back(En);
				m_SelectedList.push_back(En);

				i++;
			}

			KSceneEntity GroupEntity(SCENEENTITY_GROUP,
				(PVOID)(m_vecEntityGroup.size()-1),
				this);
			m_RenderEntityList.remove(GroupEntity);
			m_SelectableList.remove(GroupEntity);
			m_SelectedList.remove(GroupEntity);
		}
	}

	return S_OK;
}

HRESULT KSceneSceneEdit::Render4LargeTerrianDialog(HWND hOutputWnd)
{
	if(!hOutputWnd)
		return S_OK;

	RECT WndRect;
	::GetWindowRect(hOutputWnd,&WndRect);

	D3DVIEWPORT9 ViewPortSave,ViewPort;
	g_pd3dDevice->GetViewport(&ViewPortSave);
	ViewPort = ViewPortSave;

	D3DXMATRIX Mat_View_Save;
	D3DXMATRIX Mat_Proj_Save;
	g_pd3dDevice->GetTransform( D3DTS_VIEW, &Mat_View_Save );
	g_pd3dDevice->GetTransform( D3DTS_PROJECTION, &Mat_Proj_Save );
	D3DXVECTOR3 CameraPos,CameraFront,B,C,CameraLeft;
	g_cGraphicsTool.GetCameraPos(&CameraPos);
	g_cGraphicsTool.GetCameraFront(&CameraFront);
	g_cGraphicsTool.GetCameraLeftUp(&CameraLeft,&B);
	float R = 3000;
	float Beta = D3DX_PI/12;

	B = CameraPos + R*cosf(Beta)*CameraFront + R*sinf(Beta)*CameraLeft;
	C = CameraPos + R*cosf(Beta)*CameraFront - R*sinf(Beta)*CameraLeft;
	//////////////////////////////////////////////////////////////////////////
	ViewPort.X = WndRect.left;
	ViewPort.Y = WndRect.top;
	ViewPort.Width  = WndRect.right - WndRect.left;
	ViewPort.Height = WndRect.bottom- WndRect.top;
	g_pd3dDevice->SetViewport(&ViewPort);

	D3DXMATRIX  Mat_View,Mat_Proj;
	D3DXVECTOR3 Center = m_lpMapMesh->m_BBoxMap.GetCenter();
	float Length = 1.2F*max(m_lpMapMesh->m_BBoxMap.GetWidth(),m_lpMapMesh->m_BBoxMap.GetHeight());

	float t = timeGetTime()*0.0005F;
	D3DXVECTOR3 Eye = Center + D3DXVECTOR3(Length*sinf(t),0.5F*Length,Length*cosf(t));

	D3DXMatrixLookAtLH(&Mat_View,&Eye,&Center,&D3DXVECTOR3(0,1,0));
	D3DXMatrixOrthoLH(&Mat_Proj,Length,Length,1,50000);
	g_pd3dDevice->SetTransform( D3DTS_VIEW, &Mat_View );
	g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &Mat_Proj );
	g_pd3dDevice->SetTransform( D3DTS_VIEW, &Mat_View );
	g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &Mat_Proj );
	//////////////////////////////////////////////////////////////////////////
	
	g_pd3dDevice->BeginScene();
	g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
		0xFF666666, 1.0f, 0 );
	//////////////////////////////////////////////////////////////////////////	

	m_lpMapMesh->RenderForSmallMap(1);
	if (m_dwWaterReflectionMapID!=0)
	{
		LPTEXTURE pNewTexture = NULL;
		if (SUCCEEDED(g_cTextureTable.GetTexture(&pNewTexture,m_dwWaterReflectionMapID)))
		{
			m_lpMapMesh->RenderWater(m_lpTextureSky,pNewTexture);
		}
	}	

	g_cGraphicsTool.DrawAABBox(&m_lpMapMesh->m_BBoxMap.A,&m_lpMapMesh->m_BBoxMap.B,0xFFFFFFFF);
	//////////////////////////////////////////////////////////////////////////	
	g_pd3dDevice->EndScene();

	g_pd3dDevice->Present(&WndRect,NULL,hOutputWnd,NULL);

	//////////////////////////////////////////////////////////////////////////	
	g_pd3dDevice->SetViewport(&ViewPortSave);
	g_pd3dDevice->SetTransform( D3DTS_VIEW, &Mat_View_Save );
	g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &Mat_Proj_Save );

	return S_OK;
}

HRESULT KSceneSceneEdit::RenderSmallMap()
{
	if(!m_hWndSmallMap)
		return S_OK;

	RECT WndRect;
	::GetWindowRect(m_hWndSmallMap,&WndRect);

	D3DVIEWPORT9 ViewPortSave,ViewPort;
	g_pd3dDevice->GetViewport(&ViewPortSave);
	ViewPort = ViewPortSave;

	D3DXMATRIX Mat_View_Save;
	D3DXMATRIX Mat_Proj_Save;
	g_pd3dDevice->GetTransform( D3DTS_VIEW, &Mat_View_Save );
	g_pd3dDevice->GetTransform( D3DTS_PROJECTION, &Mat_Proj_Save );

	D3DXVECTOR3 CameraPos,CameraFront,B,C,CameraLeft;
	g_cGraphicsTool.GetCameraPos(&CameraPos);
	g_cGraphicsTool.GetCameraFront(&CameraFront);
	g_cGraphicsTool.GetCameraLeftUp(&CameraLeft,&B);
	float R = 3000;
	float Beta = D3DX_PI/12;

	B = CameraPos + R*cosf(Beta)*CameraFront + R*sinf(Beta)*CameraLeft;
	C = CameraPos + R*cosf(Beta)*CameraFront - R*sinf(Beta)*CameraLeft;
	//////////////////////////////////////////////////////////////////////////
	ViewPort.X = WndRect.left;
	ViewPort.Y = WndRect.top;
	ViewPort.Width  = WndRect.right - WndRect.left;
	ViewPort.Height = WndRect.bottom- WndRect.top;
	g_pd3dDevice->SetViewport(&ViewPort);

	D3DXMATRIX  Mat_View,Mat_Proj;
	D3DXVECTOR3 Center = m_lpMapMesh->m_BBoxMap.GetCenter();
	float Length = m_fSmallMapScaling * 1.05f*max(m_lpMapMesh->m_BBoxMap.GetWidth(),m_lpMapMesh->m_BBoxMap.GetHeight());

	D3DXVECTOR3 Eye = Center + D3DXVECTOR3(0,2000,0);

	D3DXMatrixLookAtLH(&Mat_View,&Eye,&Center,&D3DXVECTOR3(0,0,-1));
	D3DXMatrixOrthoLH(&Mat_Proj,Length,Length,1,5000);
	g_pd3dDevice->SetTransform( D3DTS_VIEW, &Mat_View );
	g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &Mat_Proj );
	//////////////////////////////////////////////////////////////////////////

	g_pd3dDevice->BeginScene();
	g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
		0xff666666, 1.0f, 0 );


	m_lpMapMesh->RenderForSmallMap(m_dwSmallMapOption);

	{
		K3DScene::ENTITYLIST::iterator i = m_SelectedList.begin();
		while(i!=m_SelectedList.end())
		{
			KSceneEntity Entity = *i;
			DrawSelectObjectBox(&Entity);
			i++;
		}
	}

	if(m_dwSmallMapOption&2)
	{
		K3DScene::ENTITYLIST::iterator i = m_RenderEntityList.begin();
		while(i!=m_RenderEntityList.end())
		{
			KSceneEntity Entity = *i;
			switch(Entity.Type)
			{
			case SCENEENTITY_OBJECT:
				{
					LPOBJECT pObject = (LPOBJECT) Entity.Pointer;
					pObject->DrawBBox();
					break;
				}
			}
			i++;
		}
	}


	g_cGraphicsTool.DrawLine(&CameraPos,&B,0xFFFFFFFF,0xFFFF0000);
	g_cGraphicsTool.DrawLine(&CameraPos,&C,0xFFFFFFFF,0xFFFF0000);

	//////////////////////////////////////////////////////////////////////////	
	g_pd3dDevice->EndScene();

	g_pd3dDevice->Present(&WndRect,NULL,m_hWndSmallMap,NULL);

	//////////////////////////////////////////////////////////////////////////	
	g_pd3dDevice->SetViewport(&ViewPortSave);
	g_pd3dDevice->SetTransform( D3DTS_VIEW, &Mat_View_Save );
	g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &Mat_Proj_Save );

	return S_OK;
}

HRESULT KSceneSceneEdit::RenderObjectPreview()
{
	if(!m_hWndObjectPreView)
		return S_OK;

	RECT WndRect;
	::GetWindowRect(m_hWndObjectPreView,&WndRect);

	D3DVIEWPORT9 ViewPortSave,ViewPort;
	g_pd3dDevice->GetViewport(&ViewPortSave);
	ViewPort = ViewPortSave;

	D3DXMATRIX Mat_View_Save;
	D3DXMATRIX Mat_Proj_Save;
	g_pd3dDevice->GetTransform( D3DTS_VIEW, &Mat_View_Save );
	g_pd3dDevice->GetTransform( D3DTS_PROJECTION, &Mat_Proj_Save );

	//////////////////////////////////////////////////////////////////////////
	ViewPort.X = WndRect.left;
	ViewPort.Y = WndRect.top;
	ViewPort.Width  = WndRect.right - WndRect.left;
	ViewPort.Height = WndRect.bottom- WndRect.top;
	g_pd3dDevice->SetViewport(&ViewPort);

	D3DXMATRIX  Mat_View,Mat_Proj;
	D3DXVECTOR3 Center(0,0,0);// = (pRegion->BBox_A + pRegion->BBox_B)*0.5f;
	D3DXVECTOR3 B_A(100,100,100);// = pRegion->BBox_B - pRegion->BBox_A;
	float K = timeGetTime()*0.0005f;
	float Length = 1000;
	
	if(m_dwState==EDITSTATE_PLACEOBJECT)
	{
		if(m_lpInHandObject)
		{
			Center = m_lpInHandObject->m_BBox.GetCenter();
			D3DXVec3TransformCoord(&Center,&Center,&m_lpInHandObject->m_Matrix);
			D3DXVECTOR3 Cross = m_lpInHandObject->m_BBox.B - m_lpInHandObject->m_BBox.A;
			Length = D3DXVec3Length(&Cross);
		}
		else
		{
			int k =0;
		}
	}
	else
	{
		KSceneEntity Entity;
		if(SUCCEEDED(GetFirstSelectedEntity(Entity)))
		{
			if(Entity.Type==SCENEENTITY_OBJECT)
			{
				LPOBJECT pObject= (LPOBJECT) Entity.Pointer;
				
				Center = pObject->m_BBox.GetCenter();
				D3DXVec3TransformCoord(&Center,&Center,&pObject->m_Matrix);
				D3DXVECTOR3 Cross = pObject->m_BBox.B - pObject->m_BBox.A;
				Length = D3DXVec3Length(&Cross);
			}
		}
	}


	D3DXVECTOR3 Eye = Center + 1000 * D3DXVECTOR3(sinf(K),0.3f,cosf(K));

	D3DXMatrixLookAtLH(&Mat_View,&Eye,&Center,&D3DXVECTOR3(0,1,0));
	D3DXMatrixOrthoLH(&Mat_Proj,Length,Length,1,5000);
	g_pd3dDevice->SetTransform( D3DTS_VIEW, &Mat_View );
	g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &Mat_Proj );
	//////////////////////////////////////////////////////////////////////////
	
	g_pd3dDevice->BeginScene();
	g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
		0xff666666, 1.0f, 0 );

	if(m_dwState==EDITSTATE_PLACEOBJECT)
	{
		if(m_lpInHandObject)
		{
			m_lpInHandObject->Render();
		}
	}
	else
	{
		KSceneEntity Entity;
		if(SUCCEEDED(GetFirstSelectedEntity(Entity)))
		{
			if(Entity.Type==SCENEENTITY_OBJECT)
			{
				LPOBJECT pObject= (LPOBJECT) Entity.Pointer;
				pObject->Render();
			}
		}
	}

	/*{
		D3DXMATRIX  World;
		D3DXMatrixIdentity(&World);
		Center = D3DXVECTOR3(0,0,0);
		Eye = Center + 1000 * D3DXVECTOR3(sinf(K),0.3f,cosf(K));
		D3DXMatrixLookAtLH(&Mat_View,&Eye,&Center,&D3DXVECTOR3(0,1,0));

		g_pd3dDevice->SetTransform( D3DTS_WORLD, &World );
		g_pd3dDevice->SetTransform( D3DTS_VIEW, &Mat_View );

		g_cGraphicsTool.DrawGroundGrid(50);
	}*/


	//////////////////////////////////////////////////////////////////////////	
	g_pd3dDevice->EndScene();

	g_pd3dDevice->Present(&WndRect,NULL,m_hWndObjectPreView,NULL);

	//////////////////////////////////////////////////////////////////////////	
	g_pd3dDevice->SetViewport(&ViewPortSave);
	g_pd3dDevice->SetTransform( D3DTS_VIEW, &Mat_View_Save );
	g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &Mat_Proj_Save );
		
	return S_OK;
}

BOOL KSceneSceneEdit::IsAbleToUnGroup()
{
	KSceneEntity Entity;
	if(SUCCEEDED(GetFirstSelectedEntity(Entity)))
	{
		if(Entity.Type==SCENEENTITY_GROUP)
		{
			return TRUE;
		}
	}

	return FALSE;
}

HRESULT KSceneSceneEdit::_SceneOperatorPlaceObject::AddPlacedObject(LPOBJECT pObject)
{
	KSceneEntity Entity(SCENEENTITY_OBJECT,pObject);
	m_EntityList.push_back(Entity);

	return S_OK;
}

HRESULT KSceneSceneEdit::_SceneOperatorPlaceObject::Undo(KSceneEditBase* pScene)
{
	K3DScene::ENTITYLIST::iterator i = m_EntityList.begin();
	while(i!=m_EntityList.end())
	{
		KSceneEntity Entity = *i;
		pScene->m_RenderEntityList.remove(Entity);
		pScene->m_SelectableList.remove(Entity);
		pScene->m_SelectedList.remove(Entity);

		i++;
	}
	return S_OK;
}

KSceneSceneEdit::_SceneOperatorPlaceObject::_SceneOperatorPlaceObject()
{
	;
}

KSceneSceneEdit::_SceneOperatorPlaceObject::~_SceneOperatorPlaceObject()
{

}

HRESULT KSceneSceneEdit::MoveCamera2SmallMapPos(float X,float Y)
{
	float XX = (1-X) * m_lpMapMesh->m_BBoxMap.GetLength();
	float ZZ = Y * m_lpMapMesh->m_BBoxMap.GetWidth();

	D3DXVECTOR3 Pos = m_lpMapMesh->m_BBoxMap.A + D3DXVECTOR3(XX,1000,ZZ);

	GraphicsStruct::KCamera* pCamera = &g_GraphiceEngine.m_cCamera; 

	D3DXVECTOR3 T = pCamera->Position - Pos;
	pCamera->Position = Pos;
	pCamera->ViewPoint -= T;

	pCamera->SetCamera();

	return S_OK;
}

HRESULT KSceneSceneEdit::OutputSmallMap(LPSTR pFileName)
{
	FILE* pFile = fopen(pFileName,"wb");
	if(!pFile)
		return E_FAIL;
	int X_Start = (int)(m_lpMapMesh->m_BBoxMap.A.x / TERRIANCELLWIDTH);
	int Z_Start = (int)(m_lpMapMesh->m_BBoxMap.A.z / TERRIANCELLWIDTH);

	int Width  = (int)(m_lpMapMesh->m_BBoxMap.GetLength() / TERRIANCELLWIDTH) / 32;
	int Height = (int)(m_lpMapMesh->m_BBoxMap.GetWidth()  / TERRIANCELLWIDTH) / 32;

	DWORD size = Width*256*Height*256;
	DWORD* pColor = new DWORD[size];
	if(!pColor)
		return E_FAIL;

	m_lpMapMesh->OutputSmallMap(pFile,pColor);

	_TGAHeader Head;
	ZeroMemory(&Head,sizeof(_TGAHeader));
	Head.Id =0;
	Head.ColourMapType = 0;
	Head.ImageType = 2;
	Head.ColourMapOrigin = 0;
	Head.ColourMapLength = 0;
	Head.ColourMapType   = 0;
	Head.x_origin = 0;
	Head.y_origin = 0;
	Head.width  = (unsigned short) Width*256;
	Head.height = (unsigned short) Height*256;
	Head.bitsperpixel = 32;
	Head.imagedescriptor = 8;

	fwrite(&Head,sizeof(_TGAHeader),1,pFile);
	fwrite(pColor,4,size,pFile);

	SAFE_DELETE_ARRAY(pColor);
	fclose(pFile);

	return S_OK;
}

HRESULT KSceneSceneEdit::InvalidateDeviceObjects(void)
{
	WaterMapRelease();
	return S_OK;
}

HRESULT KSceneSceneEdit::RestoreDeviceObjects(void)
{
	WaterMapCreate();
	return S_OK;
}

KSceneSceneEdit::MissileMgr::MissileMgr()
{

}

KSceneSceneEdit::MissileMgr::~MissileMgr()
{

}

KSceneSceneEdit::ObjectGroupLeaf::ObjectGroupLeaf()
{
	ZeroMemory(m_szName,sizeof(TCHAR)*256);
}

KSceneSceneEdit::ObjectGroupLeaf::~ObjectGroupLeaf()
{
	/*list<ObjectGroup*>::iterator i = m_listSubGroup.begin();
	while(i!=m_listSubGroup.end())
	{
		ObjectGroup* pGroup = *i;
		SAFE_DELETE(pGroup);
		i++;
	}
	m_listSubGroup.clear();*/
}

HRESULT KSceneSceneEdit::ObjectGroupLeaf::Split2SubGroupByMeshRef()
{
	/*list<LPOBJECT>::iterator i = m_listObject.begin();
	while(i!=m_listObject.end())
	{
		LPOBJECT pObject = *i;

		list<ObjectGroup*>::iterator i_Group = m_listSubGroup.begin();
		while(i_Group!=m_listSubGroup.end())
		{
			ObjectGroup* pGroup = *i_Group;
			list<LPOBJECT>::iterator i_SubObject = pGroup->m_listObject.begin();
			LPOBJECT pSubObject = *i_SubObject;
			if(pSubObject->m_vecMeshKeeper.size()==pObject->m_vecMeshKeeper.size())
			{
				if(pSubObject->m_vecMeshKeeper[0].m_dMeshID == pObject->m_vecMeshKeeper[0].m_dMeshID)
				{
					pGroup->m_listObject.push_back(pObject);
					break;
				}
			}
			i_Group++;
		}

		if(i_Group==m_listSubGroup.end())
		{
			ObjectGroup* pNewGroup = new ObjectGroup;
			pNewGroup->m_listObject.push_back(pObject);
			m_listSubGroup.push_back(pNewGroup);
		}

		i++;
	}*/
	return S_OK;
}

KSceneSceneEdit::ObjectGroupTree::ObjectGroupTree()
{
	ZeroMemory(m_szName,sizeof(TCHAR)*256);
	m_lpParentTree = NULL;
}

KSceneSceneEdit::ObjectGroupTree::~ObjectGroupTree()
{
	{
		list<ObjectGroupTree*>::iterator i = m_listSubGroupTree.begin();
		while(i!=m_listSubGroupTree.end())
		{
			ObjectGroupTree* pGroup = *i;
			SAFE_DELETE(pGroup);
			i++;
		}
		m_listSubGroupTree.clear();
	}
}

HRESULT  KSceneSceneEdit::GetEntityGroupIndex(DWORD* pIndex,KSceneEntity Entity)
{
	switch(Entity.Type)
	{
	case SCENEENTITY_OBJECT:
		{
			LPOBJECT pObject = (LPOBJECT) Entity.Pointer;
			if(pObject->m_dwType==OBJECT_DEFAULT)
			{
				if(_strcmpi(pObject->m_FileName.c_str(),"")==0)
				{
					vector<ObjectGroupLeaf*>::iterator i_Group = m_vecEntityGroupLeaf.begin();
					while(i_Group!=m_vecEntityGroupLeaf.end())
					{
						ObjectGroupLeaf* pGroup = *i_Group;
						vector<LPOBJECT>::iterator i_SubObject = pGroup->m_vecObject.begin();
						LPOBJECT pSubObject = *i_SubObject;

						if((_strcmpi(pSubObject->m_FileName.c_str(),"")==0)
							&&(pSubObject->m_vecMeshKeeper.size()==pObject->m_vecMeshKeeper.size())
							&&(pSubObject->m_vecMeshKeeper.size()>0))
						{
							if(pSubObject->m_vecMeshKeeper[0].m_dMeshID == pObject->m_vecMeshKeeper[0].m_dMeshID)
							{
								(*pIndex) = pGroup->m_dwObjectIndex;
								return S_OK;
							}
						}
						i_Group++;
					}
				
				}
				else
				{
					vector<ObjectGroupLeaf*>::iterator i_Group = m_vecEntityGroupLeaf.begin();
					while(i_Group!=m_vecEntityGroupLeaf.end())
					{
						ObjectGroupLeaf* pGroup = *i_Group;
						vector<LPOBJECT>::iterator i_SubObject = pGroup->m_vecObject.begin();
						LPOBJECT pSubObject = *i_SubObject;

						if(_strcmpi(pSubObject->m_FileName.c_str(),pObject->m_FileName.c_str())==0)
						{
							(*pIndex) = pGroup->m_dwObjectIndex;
							return S_OK;
						}
						i_Group++;
					}
				}
			}
			else
			{
				return E_FAIL;
			}
			break;
		}
	default:
		return E_FAIL;
		break;
	}
	return E_FAIL;

}

HRESULT  KSceneSceneEdit::GroupObjectsSelected()
{
	//////////////////////////////////////////////////////////////////////////
	//清理旧的GROUP
	{
		vector<ObjectGroupLeaf*>::iterator i = m_vecEntityGroupLeafSelected.begin();
		while(i!=m_vecEntityGroupLeafSelected.end())
		{
			ObjectGroupLeaf* pGroup = *i;
			SAFE_DELETE(pGroup);
			i++;
		}
		m_vecEntityGroupLeafSelected.clear();
	}

	K3DScene::ENTITYLIST::iterator i = m_SelectedList.begin();
	while(i!=m_SelectedList.end())
	{
		KSceneEntity Entity = *i;
		switch(Entity.Type)
		{
		case SCENEENTITY_OBJECT:
			{
				LPOBJECT pObject = (LPOBJECT) Entity.Pointer;
				if(pObject->m_dwType==OBJECT_DEFAULT)
				{
					if(_strcmpi(pObject->m_FileName.c_str(),"")==0)
					{
						vector<ObjectGroupLeaf*>::iterator i_Group = m_vecEntityGroupLeafSelected.begin();
						while(i_Group!=m_vecEntityGroupLeafSelected.end())
						{
							ObjectGroupLeaf* pGroup = *i_Group;
							vector<LPOBJECT>::iterator i_SubObject = pGroup->m_vecObject.begin();
							LPOBJECT pSubObject = *i_SubObject;

							if((_strcmpi(pSubObject->m_FileName.c_str(),"")==0)
								&&(pSubObject->m_vecMeshKeeper.size()==pObject->m_vecMeshKeeper.size())
								&&(pSubObject->m_vecMeshKeeper.size()>0))
							{
								if(pSubObject->m_vecMeshKeeper[0].m_dMeshID == pObject->m_vecMeshKeeper[0].m_dMeshID)
								{
									pGroup->m_vecObject.push_back(pObject);
									break;
								}
							}

							i_Group++;
						}

						if(i_Group==m_vecEntityGroupLeafSelected.end())
						{
							ObjectGroupLeaf* pNewGroup = new ObjectGroupLeaf;
							pNewGroup->m_vecObject.push_back(pObject);
							m_vecEntityGroupLeafSelected.push_back(pNewGroup);
						}				
					}
					else
					{
						vector<ObjectGroupLeaf*>::iterator i_Group = m_vecEntityGroupLeafSelected.begin();
						while(i_Group!=m_vecEntityGroupLeafSelected.end())
						{
							ObjectGroupLeaf* pGroup = *i_Group;
							vector<LPOBJECT>::iterator i_SubObject = pGroup->m_vecObject.begin();
							LPOBJECT pSubObject = *i_SubObject;

							if(_strcmpi(pSubObject->m_FileName.c_str(),pObject->m_FileName.c_str())==0)
							{
								pGroup->m_vecObject.push_back(pObject);
								break;
							}

							i_Group++;
						}

						if(i_Group==m_vecEntityGroupLeafSelected.end())
						{
							ObjectGroupLeaf* pNewGroup = new ObjectGroupLeaf;
							pNewGroup->m_vecObject.push_back(pObject);
							m_vecEntityGroupLeafSelected.push_back(pNewGroup);
						}
					}
				}
				else if(pObject->m_dwType==OBJECTTYPE_NPC)
				{
					KRepNPC* pNPC = (KRepNPC*)pObject;

					vector<ObjectGroupLeaf*>::iterator i_Group = 
						m_vecEntityGroupLeafSelected.begin();

					while(i_Group!=m_vecEntityGroupLeafSelected.end())
					{
						ObjectGroupLeaf* pGroup = *i_Group;
						vector<LPOBJECT>::iterator i_SubObject = pGroup->m_vecObject.begin();
						LPOBJECT pSubObject = *i_SubObject;
						KRepNPC* pSubNPC = (KRepNPC*) pSubObject;

						if(_strcmpi(pSubNPC->m_scIniFileName.c_str(),pNPC->m_scIniFileName.c_str())==0)
						{
							pGroup->m_vecObject.push_back(pNPC);
							break;
						}

						i_Group++;
					}

					if(i_Group==m_vecEntityGroupLeafSelected.end())
					{
						ObjectGroupLeaf* pNewGroup = new ObjectGroupLeaf;
						pNewGroup->m_vecObject.push_back(pObject);
						m_vecEntityGroupLeafSelected.push_back(pNewGroup);
					}
				}
				break;
			}
		}
		i++;
	}

	//////////////////////////////////////////////////////////////////////////
	//建立索引关系
	{
		for(DWORD i=0;i<m_vecEntityGroupLeafSelected.size();i++)
		{
			ObjectGroupLeaf* pGroup = m_vecEntityGroupLeafSelected[i];
			pGroup->m_dwObjectIndex = i;

			for(DWORD i_SubObject=0;i_SubObject<pGroup->m_vecObject.size();i_SubObject++)
			{
				LPOBJECT pSubObject = pGroup->m_vecObject[i_SubObject];
				pSubObject->m_dwGroupIndex = pGroup->m_dwObjectIndex;
				pSubObject->m_dwIndexInGroup = i_SubObject;

			}
		}
	}
	return S_OK;
}

HRESULT  KSceneSceneEdit::GroupObjects()
{
	//////////////////////////////////////////////////////////////////////////
	//清理旧的GROUP
	{
		vector<ObjectGroupLeaf*>::iterator i = m_vecEntityGroupLeaf.begin();
		while(i!=m_vecEntityGroupLeaf.end())
		{
			ObjectGroupLeaf* pGroup = *i;
			SAFE_DELETE(pGroup);
			i++;
		}
		m_vecEntityGroupLeaf.clear();
	}

	K3DScene::ENTITYLIST::iterator i = m_RenderEntityList.begin();
	while(i!=m_RenderEntityList.end())
	{
		KSceneEntity Entity = *i;
		switch(Entity.Type)
		{
		case SCENEENTITY_OBJECT:
			{
				LPOBJECT pObject = (LPOBJECT) Entity.Pointer;
				if(pObject->m_dwType==OBJECT_DEFAULT)
				{
					if(_strcmpi(pObject->m_FileName.c_str(),"")==0)
					{
						vector<ObjectGroupLeaf*>::iterator i_Group = m_vecEntityGroupLeaf.begin();
						while(i_Group!=m_vecEntityGroupLeaf.end())
						{
							ObjectGroupLeaf* pGroup = *i_Group;
							if(pGroup->m_vecObject.size()>0)
							{
								vector<LPOBJECT>::iterator i_SubObject = pGroup->m_vecObject.begin();
								LPOBJECT pSubObject = *i_SubObject;

								if((_strcmpi(pSubObject->m_FileName.c_str(),"")==0)
									&&(pSubObject->m_vecMeshKeeper.size()==pObject->m_vecMeshKeeper.size())
									&&(pSubObject->m_vecMeshKeeper.size()>0))
								{
									if(pSubObject->m_vecMeshKeeper[0].m_dMeshID == pObject->m_vecMeshKeeper[0].m_dMeshID)
									{
										pGroup->m_vecObject.push_back(pObject);
										break;
									}
								}
							}

							i_Group++;
						}

						if(i_Group==m_vecEntityGroupLeaf.end())
						{
							ObjectGroupLeaf* pNewGroup = new ObjectGroupLeaf;
							pNewGroup->m_vecObject.push_back(pObject);
							m_vecEntityGroupLeaf.push_back(pNewGroup);
						}				
					}
					else
					{
						vector<ObjectGroupLeaf*>::iterator i_Group = m_vecEntityGroupLeaf.begin();
						while(i_Group!=m_vecEntityGroupLeaf.end())
						{
							ObjectGroupLeaf* pGroup = *i_Group;
							if(pGroup->m_vecObject.size()>0)
							{
								vector<LPOBJECT>::iterator i_SubObject = pGroup->m_vecObject.begin();
								LPOBJECT pSubObject = *i_SubObject;

								if(_strcmpi(pSubObject->m_FileName.c_str(),pObject->m_FileName.c_str())==0)
								{
									pGroup->m_vecObject.push_back(pObject);
									break;
								}
							}
							i_Group++;
						}

						if(i_Group==m_vecEntityGroupLeaf.end())
						{
							ObjectGroupLeaf* pNewGroup = new ObjectGroupLeaf;
							pNewGroup->m_vecObject.push_back(pObject);
							m_vecEntityGroupLeaf.push_back(pNewGroup);
						}
					}
				}
				else if(pObject->m_dwType==OBJECTTYPE_NPC)
				{
					KRepNPC* pNPC = (KRepNPC*)pObject;

					vector<ObjectGroupLeaf*>::iterator i_Group = 
						m_vecEntityGroupLeaf.begin();

					while(i_Group!=m_vecEntityGroupLeaf.end())
					{
						ObjectGroupLeaf* pGroup = *i_Group;

						if(pGroup->m_vecObject.size()>0)
						{
							vector<LPOBJECT>::iterator i_SubObject = pGroup->m_vecObject.begin();
							LPOBJECT pSubObject = *i_SubObject;
							if(pSubObject->m_dwType==OBJECTTYPE_NPC)
							{
								KRepNPC* pSubNPC = (KRepNPC*) pSubObject;

								if(_strcmpi(pSubNPC->m_scIniFileName.c_str(),pNPC->m_scIniFileName.c_str())==0)
								{
									pGroup->m_vecObject.push_back(pNPC);
									break;
								}
							}
						}
						
						i_Group++;
					}

					if(i_Group==m_vecEntityGroupLeaf.end())
					{
						ObjectGroupLeaf* pNewGroup = new ObjectGroupLeaf;
						pNewGroup->m_vecObject.push_back(pObject);
						m_vecEntityGroupLeaf.push_back(pNewGroup);
					}
				}
				break;
			}
		}
		i++;
	}

	//////////////////////////////////////////////////////////////////////////
	//建立索引关系
	{
		for(DWORD i=0;i<m_vecEntityGroupLeaf.size();i++)
		{
			ObjectGroupLeaf* pGroup = m_vecEntityGroupLeaf[i];
			pGroup->m_dwObjectIndex = i;

			for(DWORD i_SubObject=0;i_SubObject<pGroup->m_vecObject.size();i_SubObject++)
			{
				LPOBJECT pSubObject = pGroup->m_vecObject[i_SubObject];
				pSubObject->m_dwGroupIndex = pGroup->m_dwObjectIndex;
				pSubObject->m_dwIndexInGroup = i_SubObject;

			}
		}
	}
	return S_OK;
}

HRESULT KSceneSceneEdit::GetHeight(D3DXVECTOR3* pOut,D3DXVECTOR2* pIN,DWORD Type)
{
	if(!m_lpMapMesh)
	{
		*pOut = D3DXVECTOR3(0,0,0);
		return S_OK;
	}

	pOut->x = pIN->x;
	pOut->z = pIN->y;
	m_lpMapMesh->GetMapHeight(&pOut->y,pIN->x,pIN->y,0);
	return S_OK;

}

HRESULT KSceneSceneEdit::PT_Scene2Logical(int* pX,int* pY,int* pZ,D3DXVECTOR3 Pos)
{
	(*pX) = (int)(Pos.x - m_lpMapMesh->m_BBoxMap.A.x)*CELL_LENGTH/100;
	(*pY) = (int)(Pos.z - m_lpMapMesh->m_BBoxMap.A.z)*CELL_LENGTH/100;
	(*pZ) = (int)(Pos.y - m_lpMapMesh->m_BBoxMap.A.y)*CELL_LENGTH/100;
	return S_OK;
}

HRESULT KSceneSceneEdit::PT_Logical2Scene(D3DXVECTOR3* pPos,int X,int Y,int Z)
{
	pPos->x = X * 100.0F /CELL_LENGTH + m_lpMapMesh->m_BBoxMap.A.x;
	pPos->z = Y * 100.0F /CELL_LENGTH + m_lpMapMesh->m_BBoxMap.A.z;
	//pPos->y = Z * 100.0F /CELL_LENGTH + m_lpMapMesh->m_BBoxMap.A.y;
	return S_OK;
}

HRESULT KSceneSceneEdit::OnShowSelectedEntityProperties(void)
{
	BOOL bAllNPC = TRUE;

	list<KSceneEntity>::iterator i = m_SelectedList.begin();
	while(i!=m_SelectedList.end())
	{
		KSceneEntity Entity = *i;
		if(Entity.Type==SCENEENTITY_OBJECT)
		{
			LPOBJECT pObject = (LPOBJECT) Entity.Pointer;
			if(pObject->m_dwType == OBJECTTYPE_NPC)
			{
				;
			}
			else
			{
				bAllNPC = FALSE;
			}
		}
		i++;
	}

	if(bAllNPC)
	{
		KSceneSceneEditorNpcPropertiesDialog Dlg;
		Dlg.SetSceneEditor(this);
		Dlg.DoModal();
	}

	return S_OK;
}
