#include "StdAfx.h"
#include "kg3dscenesceneeditor.h"
#include "KG3DGraphicsTool.h"
#include "kg3dmeshtable.h"
#include "KG3DStructs.h"
#include ".\kg3drepresentobjecttable.h"
#include "kg3denginemanager.h"
#include "KG3DEnvironment.h"
#include "KG3DGraphiceEngine.h"
#include "KG3DRepresentNPC.h"
//#include "..\..\..\Include\SO3GlobalDef.h"
#include "KG3DMeshLensFlare.h"
#include "Kg3dHOcclusion.h"
#include ".\kg3dmodeltable.h"
#include "KG3DSceneEntityGroup.h"
#include "KG3DSceneEntityGroupTable.h"
#include "KG3DRepresentObjectSet.h"
#include "KG3DSFX.h"
#include "KG3DPostRenderEffectShockWave.h"
#include "KG3DSceneLayer.h"
#include "KG3DRepresentObjectVisibleEntitySet.h"
#include "KG3DMeasureTool.h"
#include "KG3DPostRenderEffectBloom.h"
#include "KG3DMessenger.h"
#include "PhysiscSystem/KG3DPhysiscScene.h"
#include "PhysiscSystem/KG3DPhysiscManager.h"
#include "KG3DRegionInfoManager.h"
#include "MathToolEx.h"
#include "MathTool.h"
#include "..\ViMD3DLib\include\ViMD3DLib.h"				//ViM
#include "KG3DRepresentObjectSoundBall.h"
#include "KG3DSceneSettings.h"
#include "KG3DSceneCameraMovement.h"
#include "KG3DTextureTable.h"
#include "KG3DRepresentObjectPVS.h"
#include "KG3DRepresentObjectNode.h"
#include "KG3DTerrainRoad.h"
#include "Terrain/KG3DTerrainRiver.h"
#include "KG3DMaterialTable.h"

#include "KGINIFileHelpers.h"
//#include "KG3DEnvEffLight.h"

#include "KG3DRepresentNPC.h"
#include "KG3DSFXSelectTexture.h"

#include "KG3DSFXProjectionTextureMgr.h"
#include "KG3DSceneOutDoorSpaceMgr.h"
#include "KG3DSceneBlock.h"

#include "KG3DLocalMoveCoord.h"
#include "KG3DLocalRotationCoord.h"
#include "KG3DLocalScalCoord.h"
#include "KG3DRepresentNPC.h"
#include "KG3DCommonContainer.h"
#include "KG3DCommonObjectBuilder.h"
#include "KG3DRecorder.h"
#include "KG3DSceneBlock.h"

#include "kg3dsceneshadowmap.h"
#include "KG3DSceneObjectPlacementTool.h"
#include "KG3DSceneSelectionTool.h"
#include "KG3DTypes.h"
#include "KG3DTerrainBlockModifier.h"
#include "KG3DTerrainRender.h"
//#include "KG3DIntersection.h"
#include <time.h>
#include "KG3DIntersection.h"
#include "KG3DHeightMap.h"
//#include "KG3DLODAdjuster.h"

#include "KG3DRepresentObjectWall.h"
#include "KG3DWaterEffect.h"
#include "KG3DTerrainBlock.h"
#include "..\..\..\Include\IKG3DSoundShell.h"
#include "KG3DRenderGlobalState.h"
#include "KG3DModelShadowRenderer.h"
#include "kg3dcaption.h"
#include "kg3dselector.h"

#include "kg3dcliptable.h"
#include "KG3DTerrainBlockGrass.h"

#include "KG3DEnvEffFog.h"
#include "KG3DCollocator.h"

#include "KG3DSceneFunctions.h"

#include "KG3DSceneSelectionToolImp.h" // add by suntao

#include "KG3DMultiThreadLoader.h"
#include "KG3DTerrainRepresentInfo.h"
#include "KG3DProfile.h"

#include "KG3DRenderTargetTool.h"

extern BOOL g_bDesign;


#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////
#define CLOCK(profile_name) \
	ULONG __time_begin_##profile_name;\
	__asm rdtsc\
	__asm mov __time_begin_##profile_name, eax

#define UNCLOCK(profile_name) \
	__asm rdtsc\
	__asm sub eax, __time_begin_##profile_name\
	__asm mov __time_begin_##profile_name, eax

#define DEBUGOUT_CLOCK(profile_name) \
	TCHAR __str_##profile_name[256];\
	_stprintf_s(__str_##profile_name, _T(#profile_name":  %4f"), ((FLOAT)__time_begin_##profile_name) / 10000000);\
	g_cGraphicsTool.DrawDebugText(__str_##profile_name);

extern KG3DHOcclusion		g_Occlusion;
extern BOOL					g_bClient;
extern BOOL					g_bRenderGrassAndStone;
extern TCHAR g_szSceneRunnerModelName[MAX_PATH];
extern TCHAR g_szSceneRunnerMtlName[MAX_PATH];
extern TCHAR g_szSceneRunnerAniName[MAX_PATH];
extern TCHAR g_szSceneRunnerFaceModelName[MAX_PATH];
extern TCHAR g_szSceneRunnerFaceMtlName[MAX_PATH];

extern int g_nNumAllowTextureDynamicLoadFrame;
extern int g_nNumTextureDynamicLoadedFrame;

extern float g_fSpeedTreeLeafScale;

float g_fCameraNpcDistance = 0.0f;
BOOL g_bRenderBlockEdge = FALSE;//是否绘制TerrainBlock边界
BOOL g_bRenderWaterEdge = FALSE;//是否绘制WaterBlock边界
BOOL  g_bEnableAddEntity = TRUE;//利用这个暂时关闭表现逻辑像编辑器添加模型的操作
extern DWORD g_dwNumViews;
extern BOOL g_bUseVIM3D;
extern BOOL g_bAlwayRenderWater;
//BOOL g_bRenderSnap = TRUE;
static KG3DSceneSceneEditor *g_pSceneSceneEditor = NULL;

static KG3DSceneOutputWnd* s_OutWnd = NULL;
bool bRenderFloorList = false;

//---Vonbo---BOOL g_bShowTestDlg = FALSE;
//////////////////////////////////////////////////////////////////////////
//KG3DSFXSelectTexture  g_sfxSelectTexture;
extern BOOL g_bOpenSelectTexture;
//////////////////////////////////////////////////////////////////////////
extern BOOL g_bRenderRoadNode;//Add by huangjinshou 2008-1-2
extern BOOL g_bRenderSoundBall;
extern BOOL g_bPlaySoundBall;

extern BOOL g_bDesign;//是否策划编辑器

int g_nLoadCount = 0;
int g_nLoadTime = 0;

DWORD g_dwEntityNum = 0; //记录当前场景中有多少个模型

BOOL g_bEnableScaleOutput = TRUE;//是否使用缩放输出，即渲染使用1024然后拉伸输出到屏幕
int  g_nScaleOutputSize = 800;

BOOL g_bEnableBlockBox = FALSE;//场景编辑器中，顶视图是否显示TerrainBlock的包围盒。

extern HRESULT g_MultiThreadBoneAniRemove(KG3DModel* pModel);
extern HRESULT InputEntityMgrToMultiThreadCull(KG3DSceneEntityMgr* pMgr);

extern TKG3DResourcePool<KG3DTerrainBlock>g_cTerrainBlockPool;
extern TKG3DResourcePool<KG3DTerrainBlockShareVB> g_cTerrainBlockShareVB;
extern TKG3DResourcePool<KG3DTerrainBlockWaterShareVB> g_cTerrainBlockWaterShareVB;
extern TKG3DResourcePool<KG3DTerrainBlockBlendFactorShareVB>g_cTerrainBlockBlendFactorShareVB;

HRESULT KG3DSceneSceneEditor::OnLostDevice()
{
	HRESULT hr = E_FAIL;
	HRESULT hrResult = E_FAIL;

	hr = KG3DScene::OnLostDevice();
	KGLOG_COM_PROCESS_ERROR(hr);

	hr = GetCommonObjectNode().OnLostDevice();
	KGLOG_COM_PROCESS_ERROR(hr);

	hr = m_vSnapTool.OnLostDevice();
	KGLOG_COM_PROCESS_ERROR(hr);

	hr = m_PVSParameter.OnLostDevice();
	KGLOG_COM_PROCESS_ERROR(hr);

	if (m_lpWaterEffect)
	{
		hr = m_lpWaterEffect->OnLostDevice();
		KGLOG_COM_PROCESS_ERROR(hr);
	}

    if (!g_bClient)
    {
	    hr = m_EntityMgrTopView.OnLostDevice();
	    KGLOG_COM_PROCESS_ERROR(hr);
    }

	SAFE_RELEASE(m_lpMovieSurface);

	hrResult = S_OK;
Exit0:
	return hrResult;
}

HRESULT KG3DSceneSceneEditor::OnResetDevice()
{
	HRESULT hr = E_FAIL;
	HRESULT hrResult = E_FAIL;

	if (m_lpMovieSurface == NULL && m_MovieSize.cx > 0 && m_MovieSize.cy > 0)
	{
		hr = g_pd3dDevice->CreateRenderTarget(m_MovieSize.cx, m_MovieSize.cy, D3DFMT_A8R8G8B8, 
			D3DMULTISAMPLE_NONE, 0, FALSE, &m_lpMovieSurface, NULL);
		KGLOG_COM_PROCESS_ERROR(hr);
	}

    if (!g_bClient)
    {
	    hr = m_EntityMgrTopView.OnResetDevice();
	    KGLOG_COM_PROCESS_ERROR(hr);
    }

	if (m_lpWaterEffect)
	{
		hr = m_lpWaterEffect->OnResetDevice();
		KGLOG_COM_PROCESS_ERROR(hr);
	}

	hr = m_PVSParameter.OnResetDevice();
	KGLOG_COM_PROCESS_ERROR(hr);

	hr = m_vSnapTool.OnResetDevice();
	KGLOG_COM_PROCESS_ERROR(hr);

	hr = GetCommonObjectNode().OnResetDevice();
	KGLOG_COM_PROCESS_ERROR(hr);

	hr = KG3DScene::OnResetDevice();
	KGLOG_COM_PROCESS_ERROR(hr);

	hrResult = S_OK;
Exit0:
	return hrResult;
}

KG3DSceneSceneEditor::PVSParameter::PVSParameter()
{
	pDepthSave        = NULL;
	pRenderSurface    = NULL;
	pRenderTargetSave = NULL;
	pRenderTargetTexture = NULL;
	bUsePVS           = FALSE;
	nWidth = 800;
	nHeight = 600;
	pDepth = NULL;
	pRenderTargetTexture = NULL;
}

HRESULT KG3DSceneSceneEditor::PVSParameter::OnLostDevice()
{
	UnInit();

	return S_OK;
}

HRESULT KG3DSceneSceneEditor::PVSParameter::OnResetDevice()
{
	Init();
	return S_OK;
}

HRESULT KG3DSceneSceneEditor::PVSParameter::Init()
{
	HRESULT hr = E_FAIL;
	HRESULT hRetCode = E_FAIL;
	if (!pRenderTargetTexture)
	{
		hr = g_pd3dDevice->CreateTexture(nWidth,
			nHeight,
			1,
			D3DUSAGE_RENDERTARGET,
			D3DFMT_A8R8G8B8,
			D3DPOOL_DEFAULT,
			&pRenderTargetTexture,
			NULL);

		/*hr = g_pd3dDevice->CreateRenderTarget(nWidth, 
		nHeight,
		D3DFMT_A8R8G8B8,
		D3DMULTISAMPLE_NONE,
		0,
		FALSE,
		&pRenderSurface);*/
		KG_COM_PROCESS_ERROR(hr);
	}

	if (!pDepth)	
	{
		hr = g_pd3dDevice->CreateDepthStencilSurface(nWidth,
			nHeight,
			D3DFMT_D24S8,
			D3DMULTISAMPLE_NONE,
			0,
			FALSE,
			&pDepth,
			NULL);
		KG_COM_PROCESS_ERROR(hr);
	}

	//pRenderTargetTexture->AddRef();
	//pDepth->AddRef();

	hRetCode = S_OK;
Exit0:
	return hRetCode;
}

HRESULT KG3DSceneSceneEditor::PVSParameter::UnInit()
{
	SAFE_RELEASE(pRenderSurface);
	SAFE_RELEASE(pRenderTargetTexture);
	SAFE_RELEASE(pDepth);

	return S_OK;
}
KG3DSceneSceneEditor::KG3DSceneSceneEditor(void)
:m_lpLogicEditor(NULL)
//,m_pLODAdjuster(NULL)
,m_CamareState(NULL)
,m_lpWaterEffect(NULL)
,m_lpEvnMap(NULL)
{
	m_dwPreState = 0;
	m_dwSceneMapID = 0xffffffff;
	m_bTerrainPainting = FALSE;
	
	//由基类初始化，这里不要搅合
	//m_bMoving = FALSE;
	//m_bSelecting = FALSE;
	
	
	m_fMipmapLod = - 1.0F;
	m_pSFXSuede     = NULL;
	m_pRepresentNPC = NULL;
	//	m_pSFXMeteor = NULL;
	vCamera = D3DXVECTOR3(1000,0,D3DX_PI/8);
	/*
	m_bShowLogicalCell = FALSE;
	m_bShowLogicalTerrenCell = FALSE;
	m_bShowLogicalSelectedCell = FALSE;
	m_bShowLogicalItemCell = FALSE;*/

	m_dwInterCount = 0;
	m_fSceneRunnerSpeed = 1.0F;
	m_pTestModel = NULL;

	m_vCameraPos = D3DXVECTOR3(0,500,-1000);
	m_vCameraLookAtPos = D3DXVECTOR3(0,0,0);
	m_vCameraUpDirection = D3DXVECTOR3(0,1,0);

	g_pSceneSceneEditor = this;

	m_vCameraTopPos = D3DXVECTOR3(0,0,0);
	m_fCameraTopScale = 10;

	//m_nCurrentLightMapIndex = 0;
	m_pCurrentVisibleSet = NULL;

	m_vMouseTargetPosition = D3DXVECTOR3(0,0,0); 
	m_bMouseTargetMoving = FALSE;

	//此处的赋值无意义，是不是要改
	//m_nMaxFactor = 0.5f;
	//m_nMinFactor = 0.1f;

	m_pLogicalSceneConnection = NULL;

	m_bMovieREC         = FALSE;
	m_strMovieSavePath  = g_Def_WorkDirectory;
	m_strMovieSavePath += TEXT("\\Movies");
	m_lpMovieSurface    = NULL;
	m_MovieSize.cx      = 0;
	m_MovieSize.cy      = 0;

	this->SetUseCrossPlayMode(TRUE);

	m_RegionSelectA = D3DXVECTOR2(0,0);
	m_RegionSelectC = D3DXVECTOR2(0,0);
	m_RegionSelectStartPos = D3DXVECTOR2(0,0);
	m_fPolyVertexHeight = 100;

	m_lpMainWindow = NULL;

	m_dwSceneSelectionState = 0;//0未选择，1确定左上角，2确定右下角
	m_vPositionSceneSelectionA = D3DXVECTOR3(0,0,0);
	m_vPositionSceneSelectionC = D3DXVECTOR3(0,0,0);
	m_pTerrainBlock = NULL;
	m_bIsRenderCopyBrush = FALSE;
	m_bIsTerrainState = FALSE;

	/* move to post render 

	m_lpSMZTexture = NULL;
	m_pSMColorTexture = NULL;
	m_pSMColorSurface = NULL;
	m_pSMZSurface = NULL;

	m_lpSMShadowMapColor = NULL;
	m_lpSMShadowMapColorSurface = NULL;
	//m_lpPostShadowColor1 = NULL;
	//m_lpPostShadowColorSurface1 = NULL;
	//m_lpPostShadowColor2 = NULL;
	//m_lpPostShadowColorSurface2 = NULL;

	m_lpEffectPostRender = NULL;

	*/
	m_bUseFFTW = FALSE;

	/* move to post render 
	m_lpShadowMap = NULL;
	//m_lpShadowMapEntityMgr = NULL;
	m_bUseDepthTexture = FALSE;//暂时禁止掉，提交的时候再加上
	m_bUseShadowMap = FALSE;
	m_bUseCSM = FALSE;
	*/

	m_dwPreLoadingFrame = 0;

	m_bUnderWater = FALSE;
	m_fWaterHeigh = -100;
	m_lpWaterEffect = NULL;

	m_bEnableSceneBlockSelect[0] = FALSE;
	m_bEnableSceneBlockSelect[1] = FALSE;
	m_bEnableSceneBlockSelect[2] = FALSE;
	m_bEnableSceneBlockSelect[3] = FALSE;

	m_bSelectSceneBlockExistOnly = FALSE;

	m_bShowWallConnectionPoint = FALSE;
	m_vWallConnectionPointA = D3DXVECTOR3(-100,0,0);
	m_vWallConnectionPointB = D3DXVECTOR3( 100,0,0);

	m_bEnableShowLogicalSize = FALSE;
	m_bEnableShowAppointLogicalRegion = FALSE;
	m_bEnableWireFrameMode = FALSE;
	m_fZFar = 20000;
	m_nNumPreloadModels = 0;

	// 策划场景编辑器
	m_DE_Forward = FALSE;
	m_DE_Backward = FALSE;
	m_DE_Left = FALSE;
	m_DE_Right = FALSE;
	m_ForceSetCameraData.valid = FALSE;	
	m_PlayActionIndex = 0;
	m_nCurrentEditBrushIndex = 0;
	//
	
	m_fLoadingState = 0;
}

KG3DSceneSceneEditor::~KG3DSceneSceneEditor(void)
{
	UnInit();

	g_pSceneSceneEditor = NULL;

	SAFE_RELEASE(m_pTerrainBlock);
	SAFE_RELEASE(m_lpMovieSurface);

	/* move to post render 
	SAFE_RELEASE(m_lpSMZTexture);
	SAFE_RELEASE(m_pSMColorTexture);
	SAFE_RELEASE(m_pSMColorSurface);
	SAFE_RELEASE(m_pSMZSurface);

	SAFE_RELEASE(m_lpSMShadowMapColor);
	SAFE_RELEASE(m_lpSMShadowMapColorSurface);
	//SAFE_RELEASE(m_lpPostShadowColor1);
	//SAFE_RELEASE(m_lpPostShadowColorSurface1);
	//SAFE_RELEASE(m_lpPostShadowColor2);
	//SAFE_RELEASE(m_lpPostShadowColorSurface2);

	SAFE_DELETE(m_lpShadowMap);
	*/

	// 释放原先复制的对象 add by suntao
	for (vector<KG3DRepresentObject*>::iterator it = m_vectorCopyRepresentObject.begin(); it != m_vectorCopyRepresentObject.end(); it++)
	{
		KG3DRepresentObject* pRepresentObject = *it;
		SAFE_RELEASE(pRepresentObject);
	}

	g_OutputDebugInfo("KG3DSceneSceneEditor 成功析构");

	m_lpMainWindow = NULL;

	//SAFE_RELEASE(m_lpEffectPostRender);
	//SAFE_RELEASE(m_lpEffectPostRender);
	//SAFE_DELETE(m_pLODAdjuster);
}

HRESULT KG3DSceneSceneEditor::RenderForEachWindow(KG3DSceneOutputWnd& wndCur)
{
	KG3DSceneOutputWnd* pWnd = &wndCur;
	//在Scene的主入口，设置全局的参数
	{
		KG3DLight& SunLight  = g_cGraphicsTool.GetSunLight(*this);
		g_GetRenderGlobalStateModifier().SetSunLightParam(SunLight.GetParam());

		KG3DLight& PlayerLight = g_cGraphicsTool.GetPlayerLight(*this);
		g_GetRenderGlobalStateModifier().SetPlayerLightParam(PlayerLight.GetParam());
	}

	switch(pWnd->GetType())
	{
	case OUTPUTWND_SCENEEDITOR_MAIN:
		{
			m_lpMainWindow = pWnd;

			if(g_bEnableScaleOutput)
				pWnd->m_bEnableScaleOutput = TRUE;

			if(g_bUseVIM3D)
			{
				if(!pWnd->m_bUseVIM3D)
					pWnd->InitV3D();

				pWnd->GetCamera().SetCamera();

				LPDIRECT3DSURFACE9 pOrigRenderTarget = 0;									// ViM
				if (FAILED( g_pd3dDevice->GetRenderTarget(0, &pOrigRenderTarget)))			// ViM
					return E_FAIL;																	// ViM


				//float K = sinf(timeGetTime()*0.001F)*0.5+0.5;
				ViMD3Dutil_GetCamFocalTranslation(pWnd->m_fCamFixpoint,
					pWnd->m_fCamTranslation,
					pWnd->GetCamera().GetPerspectiveData().fFovy,
					pWnd->GetCamera().GetPerspectiveData().fAspect,
					pWnd->GetCamera().GetPerspectiveData().fzNear,
					pWnd->GetCamera().GetPerspectiveData().fzFar,
					400,
					1280,
					g_dwNumViews);

				for (DWORD i=0;i<g_dwNumViews;i++)
				{

					if (FAILED(g_pd3dDevice->SetRenderTarget(0, pWnd->m_pRenderTargets[i])))	// ViM
					{					
						pOrigRenderTarget->Release();
						return E_FAIL;																// ViM
					}
					//////////////////////////////////////////////////////////////////////////
					D3DXMATRIX matProj = pWnd->GetCamera().GetProjectionMatrix();

					ViMD3Dutil_MatrixPerspectiveFovLH(&matProj,
						pWnd->GetCamera().GetPerspectiveData().fFovy,
						pWnd->GetCamera().GetPerspectiveData().fAspect,
						pWnd->GetCamera().GetPerspectiveData().fzNear,
						pWnd->GetCamera().GetPerspectiveData().fzFar,		// ViM
						pWnd->m_fCamTranslation,
						pWnd->m_fCamFixpoint,
						i,
						g_dwNumViews);							    // ViM
					//matProj._41 += i*10.1F;
					/*ViMD3Dutil_MatrixModify(&matProj,
					pWnd->m_fCamTranslation,
					pWnd->m_fCamFixpoint,
					i,
					g_dwNumViews);*/

					g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

					RenderMainWindow(wndCur,&matProj);
				}

				if (FAILED( g_pd3dDevice->SetRenderTarget(0, pOrigRenderTarget)))	
				{// ViM
					pOrigRenderTarget->Release();				
					return E_FAIL;																// ViM

				}pOrigRenderTarget->Release();
			}
			else
			{
				RenderMainWindow(wndCur,&pWnd->GetCamera().GetProjectionMatrix());
			}			
			return S_OK;
			break;
		}
	case OUTPUTWND_SCENEEDITOR_OBJECTPREVEIW:
		return RenderObjectPreviewWindows(pWnd);
		break;
	case OUTPUTWND_SCENEEDITOR_TOPVIEW:
		return RenderTopView(pWnd);
		break;
	case OUTPUTWND_SCENEEDITOR_PREVIEW:
		return RenderScenePreviewWindows(pWnd);
		break;
	case OUTPUTWND_SCENEEDITOR_TEXTUREVIEW:
		return RenderTextureWindows(pWnd);
		break;
	default:
        ASSERT(FALSE);
		break;
	};
	return E_FAIL;
}

/*
int KG3DSceneSceneEditor::GetUnusedLightMapTextureIndex()
{
m_nCurrentLightMapIndex++;
return m_nCurrentLightMapIndex;
}*/


void KG3DSceneSceneEditor::SetCoordMode(DWORD dwMode) 
{
	this->GetCoordMove().SetMode(dwMode);
	this->GetCoordScale().SetMode(dwMode);
	this->GetCoordRotate().SetMode(dwMode);
}



HRESULT KG3DSceneSceneEditor::RenderTopView(KG3DSceneOutputWnd *pWnd)
{
    return RenderTopView_New(pWnd);
}

HRESULT KG3DSceneSceneEditor::RenderTopView_New(KG3DSceneOutputWnd *pWnd)
{
	HRESULT hResult  = E_FAIL;
	HRESULT hRetCode = E_FAIL;

	D3DMATERIAL9 material;
	ZeroMemory(&material, sizeof(material));

	material.Ambient.a = 1.f;
	material.Ambient.r = 1.f;
	material.Ambient.g = 1.f;
	material.Ambient.b = 1.f;
	material.Diffuse = material.Ambient;
	g_pd3dDevice->SetMaterial(&material);

	bool bHasRenderWindowBegin = false;

	/*pWnd->GetCamera().SetPosition(m_vCameraTopPos + D3DXVECTOR3(0,100000,0));
	pWnd->GetCamera().SetLookAtPosition(m_vCameraTopPos);
	pWnd->GetCamera().SetUpDirection(D3DXVECTOR3(0,0,1));*/
	pWnd->GetCamera().SetPositionLookAtUp(&(m_vCameraTopPos + D3DXVECTOR3(0,100000,0))
		, &m_vCameraTopPos
		, &D3DXVECTOR3(0,0,1));

	float a,b,c,d;
	pWnd->GetCamera().GetGlobalOrthogonal(&a,&b,&c,&d);
	a = pWnd->m_Viewport.Width * m_fCameraTopScale;
	b = pWnd->m_Viewport.Height * m_fCameraTopScale;
	c = 10;
	d = 200000;
	pWnd->GetCamera().SetGlobalOrthogonal(a,b,c,d);
	pWnd->GetCamera().RestoreGlobalProject();
	g_cGraphicsTool.SetCamera(&pWnd->GetCamera());

	PrepareRenderTopWindow(pWnd); 

	g_cGraphicsTool.ApplyLight(*this);

	hRetCode = RenderWindowBegin(pWnd,TRUE);
	KGLOG_COM_PROCESS_ERROR(hRetCode);

	bHasRenderWindowBegin = true;

	hRetCode = g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
	KGLOG_COM_PROCESS_ERROR(hRetCode);

	hRetCode = g_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	KGLOG_COM_PROCESS_ERROR(hRetCode);

	g_bHideVisibleSetEdgeMesh = !m_bEditing;

	const D3DXMATRIX* matProj = &pWnd->GetCamera().GetProjectionMatrix();

	ProcessEarlyZTopWindow(pWnd);
	RenderTopWindowsColor(pWnd, matProj);

	if ((this->GetEditState()==SCENESTATE_TERRAIN)&&(!g_bClient))
	{
		if (m_lpSceneOutDoorSpaceMgr)
		{
			m_lpSceneOutDoorSpaceMgr->RenderBrush();
		}
	}

	RenderHelpLine();

	if (m_SceneWndManager->GetCurOutputWnd())
	{
		D3DXMATRIX Proj,View;
		Proj = m_SceneWndManager->GetCurOutputWnd()->GetCamera().GetProjectionMatrix();
		View = m_SceneWndManager->GetCurOutputWnd()->GetCamera().GetViewMatrix();
		RenderSelectedEntityMark(this->GetEditState(),m_SceneWndManager->GetCurOutputWnd()->m_Viewport,Proj,View,pWnd==m_SceneWndManager->GetCurOutputWnd());
	}

    if (!g_bClient)
    {
        int nWaterDetailSave = g_cEngineOption.nWaterDetail;
        g_cEngineOption.nWaterDetail = WD_LOWER;
		m_EntityMgrTopView.RenderWaterAndSFX(this);
        g_cEngineOption.nWaterDetail = nWaterDetailSave;
		
		if(g_bEnableBlockBox)
			m_EntityMgrTopView.RenderVisibleZoneBox(this, FALSE);
    }
    
	RenderSceneRegionSelection();
	if(m_bEditing && m_bEnableShowAppointLogicalRegion)
	{
		float Y = 50000;
		GraphicsStruct::RenderState fog(D3DRS_FOGENABLE,FALSE);
		GraphicsStruct::RenderState zW(D3DRS_ZWRITEENABLE,FALSE);
		GraphicsStruct::RenderStateAlpha Alpha(0x20,FALSE,TRUE,D3DBLEND_SRCALPHA,D3DBLEND_ONE);
		D3DXVECTOR3 A = D3DXVECTOR3(m_nLogicalRegionX_Start*3200.0F,Y,m_nLogicalRegionZ_Start*3200.0F);
		D3DXVECTOR3 B = A + D3DXVECTOR3(m_nCountLogicalRegionX*3200.0F,0,0);
		D3DXVECTOR3 D = A+ D3DXVECTOR3(0,0,m_nCountLogicalRegionZ*3200.0F);
		g_cGraphicsTool.DrawRect(&A,&B,&D,0x50FF0000,NULL);
		AABBOX BOX;
		BOX.A = A;
		BOX.B = B+D-A+D3DXVECTOR3(0,1000,0);
		g_cGraphicsTool.DrawAABBox(&BOX,0x80FF0000);
	}
    hResult = S_OK;
Exit0 :
	if (bHasRenderWindowBegin)
	{
		hRetCode = RenderWindowEnd(pWnd);
		if (FAILED(hRetCode))
		{
			//KGLogPrintf(KGLOG_ERR, "KG3DSceneSceneEditor::RenderMainWindow(%.8x) RenderWindowEnd() failed\n", pWnd);
		}
	}
	return hResult;
}

HRESULT KG3DSceneSceneEditor::RenderTopWindowsColor(KG3DSceneOutputWnd *pWnd,const D3DXMATRIX* matProj)
{
	GraphicsStruct::RenderState ZWrite(D3DRS_ZWRITEENABLE,FALSE);
	GraphicsStruct::RenderState ZCmp(D3DRS_ZFUNC,D3DCMP_EQUAL);

    if (!g_bClient)
    {
        m_EntityMgrTopView.RenderForColorOnly(this, pWnd);
    }

	if(m_pRepresentNPC)
		m_pRepresentNPC->Render(RENDER_NONE); 



	ShowInfo();
    if (!g_bClient)
    {
    	m_EntityMgrTopView.RenderProfile();
    }

	return S_OK;
}

HRESULT KG3DSceneSceneEditor::RenderMainWindowsColor(KG3DSceneOutputWnd *pWnd)
{
	GraphicsStruct::RenderState ZWrite(D3DRS_ZWRITEENABLE,FALSE);
	GraphicsStruct::RenderState ZCmp(D3DRS_ZFUNC,D3DCMP_EQUAL);

	DWORD dw_RsState = D3DFILL_SOLID;
	g_pd3dDevice->GetRenderState(D3DRS_FILLMODE,&dw_RsState);
	if (m_bEnableWireFrameMode)
		g_pd3dDevice->SetRenderState(D3DRS_FILLMODE,D3DFILL_WIREFRAME);
	else
		g_pd3dDevice->SetRenderState(D3DRS_FILLMODE,D3DFILL_SOLID);	

	g_cGraphicsTool.SetCurrentConverMap(m_lpSceneOutDoorSpaceMgr->m_lpConverMap);


	if (NULL != m_pRepresentNPC)//要在EntityMgr绘制之前Input
	{
		g_GetModelShadowRenderer().InputShadowDummy(m_pRepresentNPC->GetTranslation(), 50);
	}

	m_EntityMgr.RenderForColorOnly(this, pWnd);

	if (!m_bEditing && m_pRepresentNPC)
	{
		GraphicsStruct::RenderState colorWrite(D3DRS_COLORWRITEENABLE ,D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_GREEN);
		GraphicsStruct::RenderState ZCmp(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
		m_pRepresentNPC->Render(RENDER_NONE); 
	}

	{
		list<KG3DRepresentNpcScreenRunner*>::iterator i = m_listRepresentNPC.begin();
		list<KG3DRepresentNpcScreenRunner*>::iterator iend = m_listRepresentNPC.end();
		while (i!=iend)
		{
			KG3DRepresentNPC* pNpc = *i;
			pNpc->Render(RENDER_NONE);
			i++;
		}
	}

	g_pd3dDevice->SetRenderState(D3DRS_FILLMODE,dw_RsState);
	g_cGraphicsTool.SetCurrentConverMap(NULL);
	return S_OK;
}


HRESULT KG3DSceneSceneEditor::PreprocessWaterScene(KG3DSceneOutputWnd& Wnd,BOOL bNeedBeginScene)
{
	HRESULT hr = E_FAIL;
	DWORD dwPixelVersion = 0;

	float fWaterHeight = 0.0f;
	D3DXVECTOR3 vCameraPosition;
	BOOL bWaterVisible = FALSE;

	Wnd.GetCamera().GetPosition(&vCameraPosition);
	//////////////////////////////////////////////////////////////////////////
	//得到摄象机和水面之间的关系
	hr = m_lpSceneOutDoorSpaceMgr->GetWaterHeight(fWaterHeight, vCameraPosition.x, vCameraPosition.z);
	if(FAILED(hr))
		m_bUnderWater = FALSE;
    else
		m_bUnderWater = (fWaterHeight > vCameraPosition.y);

    if (!m_bUnderWater)
    {
        bWaterVisible = m_EntityMgr.IsWaterVisible();

        if (!bWaterVisible)
		{
            return S_OK;
		}
    }

	m_fWaterHeigh = m_fWaterHeigh*0.8F+0.2F*fWaterHeight;

	{
		dwPixelVersion = (g_cGraphicsEngine.GetD3DCaps().PixelShaderVersion & 0x0000FF00) >> 8;

		if (dwPixelVersion >= 2 && g_cEngineManager.GetRenderOption(RENDER_WATER))
		{
            if (g_cEngineOption.nWaterDetail >= WD_HIGH)
            {
                hr = RenderWaterWaveTexture(&Wnd);
                KGLOG_COM_PROCESS_ERROR(hr);

                if (!m_bUnderWater)
                {
                    hr = RenderReflectTexture(&Wnd,bNeedBeginScene);
                    KGLOG_COM_PROCESS_ERROR(hr);
                }
            }
		}
	}

	return S_OK;
Exit0:
	return hr;
}

IEKG3DWaterEffect* KG3DSceneSceneEditor::GetWaterEffect()
{
	_KG3D_DEBUG_SUPPERSUB_POINTER_EQUAL(IEKG3DWaterEffect, KG3DWaterEffect);//有些地方使用了IEKG3DWaterEffect到KG3DWaterEffect的强转，这里要保证强转的成功
    return m_lpWaterEffect;
}
//
//HRESULT KG3DSceneSceneEditor::CalcFrustum(KG3DCamera* pCamera)
//{
//    ASSERT(pCamera);
//
//    /*
//      clear scene node frustum first 
//     */
//
//    m_EntityMgr.ClearFrustum();
//
//
//    /*
//      calc current frustum now
//     */
//
//    KG3DSceneSpaceNode* pCurrNode = GetSpaceNodeByHandle(pCamera->m_uSceneSpaceNode);
//    ASSERT(pCurrNode);
//
//    KG3DFrustum Frustum;
//    Frustum.CreateByCamera(*pCamera);
//
//    KG3DSceneSpaceNode::SetTopVisableNode(NULL);
//    pCurrNode->CalcFrustum(Frustum, LOOK_IN_TO_OUT);
//    ASSERT(KG3DSceneSpaceNode::GetTopVisableNode());
//
//    return S_OK;
//}

HRESULT KG3DSceneSceneEditor::NotifyObjInfomationChanged(IEKG3DRepresentObject* pObj)
{
    if (!pObj)
        return E_FAIL;
    m_lpSceneOutDoorSpaceMgr->RequestObjectInformationChange(static_cast<KG3DRepresentObject*>(pObj));

    return S_OK;
}

HRESULT KG3DSceneSceneEditor::UpdateEditSpaceNodeInformation()
{
	if (m_pEditSpaceNode != m_lpSceneOutDoorSpaceMgr)
		m_lpSceneOutDoorSpaceMgr->RequestObjectInformationChange(dynamic_cast<KG3DRepresentObjectPVS*>(m_pEditSpaceNode));


	return S_OK;
}

HRESULT KG3DSceneSceneEditor::PrepareRenderTopWindow(KG3DSceneOutputWnd *pWnd)
{
	D3DXVECTOR3 vec3Pos;
	pWnd->GetCamera().GetPosition(&vec3Pos);

    if (!g_bClient)
    {
    	m_EntityMgrTopView.SetVisibleGate(0,0.1F);
    }
	ProcessSpaceNodeByCamera(&pWnd->GetCamera());
	CalcFrustum(&pWnd->GetCamera());

	m_vCameraPos = vec3Pos;

	if(m_lpSceneOutDoorSpaceMgr)
	{
		D3DXVECTOR3 vFront;
		pWnd->GetCamera().GetFront(&vFront);
		D3DXVECTOR2 vDir(vFront.x,vFront.z);
		D3DXVECTOR2 vPos(vec3Pos.x,vec3Pos.z);
		float fWidth;
		float fHeight;
		float fNear;
		float fFar;
		pWnd->GetCamera().GetOrthogonal(&fWidth, &fHeight, &fNear, &fFar);
		

		float fRadius = max(fWidth, fHeight);
		m_lpSceneOutDoorSpaceMgr->ProcessDynamicLoad(FALSE, vPos,vDir, fRadius, fRadius, 0,this);
	}

    if (!g_bClient)
    {
        m_EntityMgrTopView.GetVisibleEntity(
            m_lpSceneOutDoorSpaceMgr->GetFrustums(),
            this,
            m_vCameraPos
        );
        //m_EntityMgrTopView.m_lpTerrainRender->CopyRenderTerrainBlock(m_EntityMgr.m_lpTerrainRender);


        m_EntityMgrTopView.FrameMoveVisibleEntity();

        if (g_dwRenderCount % 10 < 2)
        {
            m_EntityMgrTopView.GetSelectableEntityList(/*&m_listSelectableEntity, */*this, !g_bClient,g_bClient);
        }
    }

	return S_OK;
}


HRESULT KG3DSceneSceneEditor::PrepareRenderMainWindow(KG3DSceneOutputWnd *pWnd)
{
    KG3PROFILE_DURATION("Prepare Render Main Window");
    D3DXVECTOR3 vec3Pos,vecFront;
    float a,b,c,d;
    UINT uOutDoorHandle = 0;
    KG3DCamera *pCamera = NULL;

    pCamera = &(pWnd->GetCamera());

	/* update camera */

	pCamera->GetPerspective(&a, &b, &c, &d);
    pCamera->SetPerspective(m_fFovy, b, c, m_fZFar);
	pCamera->GetFront(&vecFront);
	pCamera->GetPosition(&vec3Pos);
	m_vCameraPos = vec3Pos;
	
	//根据m_fZFar调整一下Fog的参数
	g_cGraphicsTool.SetFogDistancePercent(*this, m_fZFar / 40000.0f);

	/* dynamic load */
	if(m_lpSceneOutDoorSpaceMgr)
	{
		D3DXVECTOR2 vPos(vec3Pos.x, vec3Pos.z);
		D3DXVECTOR2 vDir(vecFront.x,vecFront.z);
		m_lpSceneOutDoorSpaceMgr->ProcessDynamicLoad(FALSE, vPos,vDir, m_fZFar, m_fZFar,25000/* m_fZFar / 20*/,this);
		m_lpSceneOutDoorSpaceMgr->ProcessSoundBall(pCamera);
	}

	/* calc the frustum */
    ProcessSpaceNodeByCamera(pCamera);

    uOutDoorHandle = m_lpSceneOutDoorSpaceMgr->GetHandle();
    if (pCamera->m_uSceneSpaceNode == uOutDoorHandle)
    {
        pCamera->SetPosition((vec3Pos - vecFront * 100.0f)); //优化裁剪用
    }

	CalcFrustum(pCamera);

    if (pCamera->m_uSceneSpaceNode == uOutDoorHandle)
    {
        pCamera->SetPosition(vec3Pos);
    }

    /* use current frustums to get visable entity */
	m_EntityMgr.GetVisibleEntity(
		m_lpSceneOutDoorSpaceMgr->GetFrustums(),
		this,
		m_vCameraPos
    );

    /* frame the visable entity */
	m_EntityMgr.FrameMoveVisibleEntity();

	if (g_dwRenderCount % 10 < 2)
	{
		m_EntityMgr.GetSelectableEntityList(/*&m_listSelectableEntity, */*this, !g_bClient,g_bClient);
	}

	return S_OK;
}

HRESULT KG3DSceneSceneEditor::ProcessEarlyZTopWindow(KG3DSceneOutputWnd *pWnd)
{
	GraphicsStruct::RenderStateAlpha Alpha(0x20,FALSE,FALSE,D3DBLEND_SRCALPHA,D3DBLEND_INVSRCALPHA);

	GraphicsStruct::RenderState colorWrite(D3DRS_COLORWRITEENABLE ,0);
	GraphicsStruct::RenderState ZWrite(D3DRS_ZWRITEENABLE,TRUE);
	GraphicsStruct::RenderState lightEnable(D3DRS_LIGHTING,FALSE);


    if (!g_bClient)
    {
    	m_EntityMgrTopView.RenderForDepthOnly(this,TRUE,TRUE,TRUE);
    }

	/*if(m_pRepresentNPC)
	m_pRepresentNPC->Render(RENDER_NONE);*/

	return S_OK;
}

HRESULT KG3DSceneSceneEditor::ProcessEarlyZ(KG3DSceneOutputWnd *pWnd)
{
	GraphicsStruct::RenderStateAlpha Alpha(0x20,FALSE,FALSE,D3DBLEND_SRCALPHA,D3DBLEND_INVSRCALPHA);

	DWORD dwColorWrite = 0;
	if(g_cEngineOption.bUseRenderTarget_DepthTexture)
	{
		if (g_nDepthTexturePackingType == 0)
			dwColorWrite = D3DCOLORWRITEENABLE_RED;
		else
			dwColorWrite = D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA;
	}


	GraphicsStruct::RenderState colorWrite(D3DRS_COLORWRITEENABLE ,dwColorWrite);
	GraphicsStruct::RenderState ZWrite(D3DRS_ZWRITEENABLE,TRUE);
	GraphicsStruct::RenderState lightEnable(D3DRS_LIGHTING,FALSE);

	DWORD dw_RsState = D3DFILL_SOLID;
	g_pd3dDevice->GetRenderState(D3DRS_FILLMODE,&dw_RsState);
	if (m_bEnableWireFrameMode)
		g_pd3dDevice->SetRenderState(D3DRS_FILLMODE,D3DFILL_WIREFRAME);
	else
		g_pd3dDevice->SetRenderState(D3DRS_FILLMODE,D3DFILL_SOLID);	

	m_EntityMgr.RenderForDepthOnly(
		this,
		g_cEngineManager.GetRenderOption(RENDER_TERRAIN),
		g_cEngineManager.GetRenderOption(RENDER_TREE),
		g_cEngineManager.GetRenderOption(RENDER_ENTITY)
	);

	if(!m_bEditing && m_pRepresentNPC)
	{
		m_pRepresentNPC->Render(RENDER_NONE);
	}

	{
		list<KG3DRepresentNpcScreenRunner*>::iterator i = m_listRepresentNPC.begin();
		list<KG3DRepresentNpcScreenRunner*>::iterator iend = m_listRepresentNPC.end();
		while (i!=iend)
		{
			KG3DRepresentNPC* pNpc = *i;
			pNpc->Render(RENDER_NONE);
			i++;
		}
	}

	g_pd3dDevice->SetRenderState(D3DRS_FILLMODE,dw_RsState);	

	return S_OK;
}

HRESULT KG3DSceneSceneEditor::ProcessForCameraAnimation(KG3DSceneOutputWnd *pWnd)
{
	HRESULT hr = S_OK;
	IEKG3DSceneCameraAnimation *piCameraAnimation = NULL;
	m_pCameraMovement->GetCurCamAnimation(&piCameraAnimation);

	KG3DSceneCameraAnimation *pKG3DSceneCameraAnimation = static_cast<KG3DSceneCameraAnimation *>(piCameraAnimation);
	KGLOG_PROCESS_ERROR(pKG3DSceneCameraAnimation);

	BOOL bCheck = FALSE;
	if(g_bClient)
	{
		bCheck = TRUE;
	}
	else
	{
        ULONG uState = this->GetEditState();
		bCheck = (uState == SCENESTATE_CAMERAANIMATION) || (uState == SCENESTATE_BENCHMARK);
	}

	if (bCheck)
	{
		///CameraAnimation Operation
		ASSERT( pKG3DSceneCameraAnimation );
		DWORD dwNowTime =  g_cGraphicsTool.GetNowTime();

        if (this->GetEditState() == SCENESTATE_BENCHMARK)
        {
            HRESULT hRetCode = E_FAIL;
            if(pKG3DSceneCameraAnimation->IsFinished(dwNowTime))
            {
                DWORD dwIndex = 0;
                DWORD dwCount = 0;

                hRetCode = m_pCameraMovement->GetAnimationCount(&dwCount);
                KG_COM_CHECK_ERROR(hRetCode);

                hRetCode = m_pCameraMovement->GetCurCamAniIndex(&dwIndex);
                KG_COM_CHECK_ERROR(hRetCode);

                if (dwIndex == dwCount - 1)
                    dwIndex = 0;
                else
                    dwIndex++;

                pKG3DSceneCameraAnimation->StopPlay();

                hRetCode = m_pCameraMovement->SetCurCamAnimation(dwIndex);
                KG_COM_CHECK_ERROR(hRetCode);

                hRetCode = m_pCameraMovement->GetCurCamAnimation(&piCameraAnimation);
                KG_COM_CHECK_ERROR(hRetCode);

                pKG3DSceneCameraAnimation = static_cast<KG3DSceneCameraAnimation*>(piCameraAnimation);
                KG_PROCESS_ERROR(pKG3DSceneCameraAnimation);

                pKG3DSceneCameraAnimation->SetRenderCount(0);
                pKG3DSceneCameraAnimation->StartPlay(FALSE);
            }
            hRetCode = pKG3DSceneCameraAnimation->UpdateCameraByTime(&pWnd->GetCamera());
            KG_COM_PROCESS_ERROR(hRetCode);

            pKG3DSceneCameraAnimation->SetRenderCount(pKG3DSceneCameraAnimation->GetRenderCount()+1);
            KG_PROCESS_SUCCESS(TRUE);
        }

		if(g_eEngineOption.bShowTestDlg)
		{
			DWORD dwPlayedTime = dwNowTime - pKG3DSceneCameraAnimation->GetAnimationStartTime();
			if(dwPlayedTime >= pKG3DSceneCameraAnimation->GetAnimationTotalTime())
			{
				float fps = pKG3DSceneCameraAnimation->GetRenderCount() * 1000.0F / dwPlayedTime;
				//TCHAR output[256];
				//sprintf(output,"Test Result: RenderCount %d Fps: %.2f\n",pKG3DSceneCameraAnimation->GetRenderCount(),fps);
				//MessageBox(g_hBaseWnd,output,"测试结果",MB_OK);
				KGLogPrintf(KGLOG_DEBUG, "Test Result: RenderCount %d Fps: %.2f\n", pKG3DSceneCameraAnimation->GetRenderCount(), fps);

				pKG3DSceneCameraAnimation->SetAnimationStartTime(dwNowTime);
				pKG3DSceneCameraAnimation->SetRenderCount(0);


			}
		}
		//May Cause Some Bugs,Just Temp.
		if (m_bMovieREC && !g_bClient)
		{
            DWORD dwPlayedTime = dwNowTime - pKG3DSceneCameraAnimation->GetAnimationStartTime();
			if(dwPlayedTime >= pKG3DSceneCameraAnimation->GetAnimationTotalTime())
			this->EndMovieREC();	
		}

		hr = pKG3DSceneCameraAnimation->UpdateCamera(&pWnd->GetCamera());
		// if have not comera animation is also return E_FAIL, but 
		// it is should not to exit the funciton 
		// is should continue 
		// KGLOG_COM_PROCESS_ERROR(hr);

		pKG3DSceneCameraAnimation->SetRenderCount(pKG3DSceneCameraAnimation->GetRenderCount()+1);
	}
	else
	{
		BOOL bIsPlaying = FALSE;
		pKG3DSceneCameraAnimation->GetPlayState(&bIsPlaying);
		if (bIsPlaying)
		{
			pKG3DSceneCameraAnimation->StopPlay();
		}
	}

Exit1:
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneSceneEditor::ShockWaveRender()
{
    // must check the d3d cap
    /*GraphicsStruct::RenderState speAlpha(D3DRS_SEPARATEALPHABLENDENABLE, TRUE);
    GraphicsStruct::RenderState opAlpha(D3DRS_BLENDOPALPHA, D3DBLENDOP_ADD);
    GraphicsStruct::RenderState srcAlpha(D3DRS_SRCBLENDALPHA, D3DBLEND_BLENDFACTOR);
    GraphicsStruct::RenderState dstAlpha(D3DRS_DESTBLENDALPHA, D3DBLEND_ZERO);
    GraphicsStruct::RenderState facColor(D3DRS_BLENDFACTOR, 0x00000000);*/

	// Here render all shock wave effect
	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
	m_EntityMgr.ShockWaveRender();

	return S_OK;
}

HRESULT KG3DSceneSceneEditor::ProcessPostRender(KG3DSceneOutputWnd *pWnd)
{
	if (pWnd->IsEnablePostRender())
	{
		GraphicsStruct::RenderState RSFog(D3DRS_FOGENABLE,FALSE);
		GraphicsStruct::RenderState ZWrite(D3DRS_ZWRITEENABLE, FALSE);

		pWnd->RenderPostEffect(this);
	}

	return S_OK;
}

HRESULT KG3DSceneSceneEditor::ProcessCameraForMainWindows(KG3DSceneOutputWnd* pWnd)
{
	float fDepth  = 0.0F;
	float fFovy   = 0.0f;
	float fAspect = 0.0f;
	float fzNear  = 0.0f;
	float fzFar   = 0.0f;
	D3DXMATRIX mView;

	g_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, m_Sample_Mag);
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, m_Sample_Min);
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, m_Sample_Mip);
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_MAXANISOTROPY ,g_cEngineOption.dwMaxAnisotropy);
	g_pd3dDevice->SetSamplerState(1, D3DSAMP_MAGFILTER, m_Sample_Mag);
	g_pd3dDevice->SetSamplerState(1, D3DSAMP_MINFILTER, m_Sample_Min);
	g_pd3dDevice->SetSamplerState(1, D3DSAMP_MIPFILTER, m_Sample_Mip);
	g_pd3dDevice->SetSamplerState(1, D3DSAMP_MAXANISOTROPY ,g_cEngineOption.dwMaxAnisotropy);
	g_pd3dDevice->SetSamplerState(2, D3DSAMP_MAGFILTER, m_Sample_Mag);
	g_pd3dDevice->SetSamplerState(2, D3DSAMP_MINFILTER, m_Sample_Min);
	g_pd3dDevice->SetSamplerState(2, D3DSAMP_MIPFILTER, m_Sample_Mip);
	g_pd3dDevice->SetSamplerState(2, D3DSAMP_MAXANISOTROPY ,g_cEngineOption.dwMaxAnisotropy);
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_MIPMAPLODBIAS,  FtoDW(m_fMipmapLod));
	g_pd3dDevice->SetSamplerState(1, D3DSAMP_MIPMAPLODBIAS,  FtoDW(m_fMipmapLod));
	g_pd3dDevice->SetSamplerState(2, D3DSAMP_MIPMAPLODBIAS,  FtoDW(m_fMipmapLod));
	g_pd3dDevice->SetRenderState( D3DRS_DEPTHBIAS, FtoDW(fDepth));

	pWnd->GetCamera().RestoreGlobalProject();

	pWnd->GetCamera().GetPerspective(&fFovy,&fAspect,&fzNear,&fzFar);
	m_vCameraMainPers = D3DXVECTOR4(fFovy,fAspect,fzNear,fzFar);
	pWnd->GetCamera().SetPerspective(m_fFovy,fAspect,fzNear,m_fZFar);

	pWnd->GetCamera().GetPosition(&m_vCameraMainPos);

	pWnd->GetCamera().SetCamera();

	g_pd3dDevice->GetTransform(D3DTS_VIEW,&mView);
	m_vCameraMainFront = D3DXVECTOR3(mView._13,mView._23,mView._33);
	m_vCameraMainLeft = D3DXVECTOR3(mView._11,mView._21,mView._31);

	return S_OK;
}

HRESULT KG3DSceneSceneEditor::ProcessMovieRecForMainWindows(KG3DSceneOutputWnd* pWnd)
{
	if (m_bMovieREC && m_lpMovieSurface) 
	{
		LPDIRECT3DSURFACE9 lpBackBuffer = NULL;
		TCHAR   szFileName[MAX_PATH];
		HRESULT hRetCode = E_FAIL;
		RECT    Rect;
		RECT    RectImage;
		PRECT   pRectImage = NULL;

		D3DVIEWPORT9 ViewPort;
		g_pd3dDevice->GetViewport(&ViewPort);

		Rect.left   = ViewPort.X;
		Rect.top    = ViewPort.Y;
		Rect.right  = ViewPort.X + ViewPort.Width;
		Rect.bottom = ViewPort.Y + ViewPort.Height;

		if (m_nAutoScanl)
		{
			D3DSURFACE_DESC Desc;
			m_lpMovieSurface->GetDesc(&Desc);
			RectImage.left   = 0;
			RectImage.top    = 0;

			FLOAT fScanlViewPort = (FLOAT)ViewPort.Height / (FLOAT)ViewPort.Width;
			FLOAT fScanlImage    = (FLOAT)Desc.Height / (FLOAT)Desc.Width;

			if (fScanlViewPort > fScanlImage)
			{
				RectImage.right  = (LONG)(Desc.Height / fScanlViewPort);
				RectImage.bottom = Desc.Height;             
			}
			else
			{
				RectImage.right  = Desc.Width;
				RectImage.bottom = (LONG)(Desc.Width * fScanlViewPort);                
			}

			pRectImage = &RectImage;
		}
		TCHAR szFileType[8];
		switch (m_ImageType)
		{
		case D3DXIFF_BMP :
			sprintf(szFileType, TEXT("%s"), TEXT(".bmp"));
			break;
		case D3DXIFF_JPG :
			sprintf(szFileType, TEXT("%s"), TEXT(".jpg"));
			break;
		case D3DXIFF_PNG :
			sprintf(szFileType, TEXT("%s"), TEXT(".png"));
			break;
		case D3DXIFF_DDS :
			sprintf(szFileType, TEXT("%s"), TEXT(".dds"));
			break;
		case D3DXIFF_TGA :
			sprintf(szFileType, TEXT("%s"), TEXT(".tga"));
			break;
		default :
			sprintf(szFileType, TEXT("%s"), TEXT(".bmp"));
			break;
		}

		sprintf(szFileName, TEXT("%s%05d%s"), m_strMovieSavePath.c_str(), m_nSaveCount++, szFileType);

		hRetCode = g_pd3dDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &lpBackBuffer);
		if (SUCCEEDED(hRetCode))
		{
			hRetCode = g_pd3dDevice->StretchRect(
				lpBackBuffer,
				&Rect,
				m_lpMovieSurface,
				pRectImage,
				m_FiterType
				);
			if (SUCCEEDED(hRetCode))
			{
				D3DXSaveSurfaceToFile(
					szFileName,
					m_ImageType,
					m_lpMovieSurface,
					NULL,
					pRectImage
					);
			}

		}
		SAFE_RELEASE(lpBackBuffer);

		RECT rectInfo = {ViewPort.X + 8, ViewPort.Y + ViewPort.Height - 20, ViewPort.X + ViewPort.Width, ViewPort.Y + ViewPort.Height};
		TCHAR strMovieInfo[128];
		sprintf(strMovieInfo, TEXT("已录制%d帧,片长%.2f秒"), m_nSaveCount, m_nSaveCount * g_cEngineManager.GetTimeControl().m_dwInterval / 1000.f);
		g_cGraphicsTool.GetFont2()->DrawText(
			NULL, strMovieInfo, -1, &rectInfo, DT_NOCLIP, D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f) 
			);
	}
	return S_OK;
}

/* move to post render 
HRESULT KG3DSceneSceneEditor::OutPutPostRenderShadow(KG3DSceneOutputWnd *pWnd,D3DVIEWPORT9* pView)
{
KGLOG_PROCESS_ERROR(m_lpEffectPostRender);

m_lpEffectPostRender->SetTexture("tMainDepth",m_lpSMZTexture);
m_lpEffectPostRender->SetTexture("tMainColor",m_pSMColorTexture);
m_lpEffectPostRender->SetTexture("tPostShadow1",m_lpSMShadowMapColor);
m_lpEffectPostRender->SetTechnique("tec1");

UINT uPass = 0;
m_lpEffectPostRender->Begin(&uPass,0);
m_lpEffectPostRender->BeginPass(0);

g_cGraphicsTool.DrawScreenRectNoChange(&D3DXVECTOR2(0,0),&D3DXVECTOR2(pView->Width*1.0F,pView->Height*1.0F),0,0xFFFFFFFF);

m_lpEffectPostRender->EndPass();
m_lpEffectPostRender->End();

return S_OK;
Exit0:
return E_FAIL;
}
*/

HRESULT KG3DSceneSceneEditor::RenderMainWindow(KG3DSceneOutputWnd& wndCur,const D3DXMATRIX* matProj)
{
	KG3DSceneOutputWnd* pWnd = &wndCur;
	s_OutWnd = pWnd;
	HRESULT hr = E_FAIL;
	BOOL bHasRenderWindowBegin = FALSE;
	DWORD dwNowTime = timeGetTime();


	ProcessCameraForMainWindows(pWnd);

	ProcessForCameraAnimation(pWnd);
	//////////////////////////////////////////////////////////////////////////
	g_cGraphicsTool.ApplyLight(*this);

	g_cGraphicsTool.SetCamera(&pWnd->GetCamera());
	{
		///CTH,调节GPS，设定其位置
		D3DXVECTOR3 CamLookAtPos(0, 0,0);
		pWnd->GetCamera().GetLookAtPosition(&CamLookAtPos);
		this->m_GameGPS.AdjustPos(CamLookAtPos);
	}

	
	if (SUCCEEDED(g_pd3dDevice->BeginScene()))
	{
		if (pWnd->NeedPreZRender())
			pWnd->PreZRender(this);
		g_pd3dDevice->EndScene();
	}
	//////////////////////////////////////////////////////////////////////////
	//渲染准备
	PrepareRenderMainWindow(pWnd);
	PreprocessWaterScene(*pWnd,TRUE);

	//////////////////////////////////////////////////////////////////////////
	if(g_bEnableScaleOutput)
	{
		KG3DRenderTargetTool* pTool = g_cGraphicsTool.GetScaleOutputRenderTool();
		if(pTool)
		{
			pTool->BeginNewRenderTarget(NULL,NULL,NULL);
		}
	}

	hr = RenderWindowBegin(pWnd,!g_bEnableScaleOutput);
	KGLOG_COM_PROCESS_ERROR(hr);
	bHasRenderWindowBegin = TRUE;

    /*
      check whether under the water, and render the refl and wave texture 
     */
	

	//////////////////////////////////////////////////////////////////////////
	if(pWnd && g_cEngineOption.bEnableDynamicConverMap)
	{	
		RenderConverMapTexture(pWnd);
	}

	hr = g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
	KGLOG_COM_PROCESS_ERROR(hr);

	hr = g_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	KGLOG_COM_PROCESS_ERROR(hr);

	g_bHideVisibleSetEdgeMesh = !m_bEditing;
	//////////////////////////////////////////////////////////////////////////

	float a,b,c,d;
	pWnd->GetCamera().GetPerspective(&a,&b,&c,&d);
	pWnd->GetCamera().SetPerspective(m_fFovy,b,10,m_fZFar);

	//////////////////////////////////////////////////////////////////////////
	if(pWnd && pWnd->IsEnablePostRender() && g_cEngineOption.bUseRenderTarget_DepthTexture)

	{
		g_cGraphicsTool.EnableRenderTarget(0);
		g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0 );
	}

    // 编辑器线框模式下，为使闭塞裁剪效果正确而多画一次深度
    if (m_bEnableWireFrameMode)
    {
        GraphicsStruct::RenderState fog(D3DRS_FOGENABLE,FALSE);
        m_bEnableWireFrameMode = false;
        ProcessEarlyZ(pWnd);
        m_bEnableWireFrameMode = true;
        g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
    }

	{
		GraphicsStruct::RenderState fog(D3DRS_FOGENABLE,FALSE);
		ProcessEarlyZ(pWnd);
	}
	if (pWnd->NeedPreColorRender())
		pWnd->PreColorRender(this);
	if(pWnd && pWnd->IsEnablePostRender() && g_cEngineOption.bUseRenderTarget_DepthTexture)

		g_cGraphicsTool.EnableRenderTarget(1);

	//////////////////////////////////////////////////////////////////////////
	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, 0x007f7f7f, 1.0f, 0 );

	if(g_cEngineManager.GetRenderOption( RENDER_ENVIRONMENT))
	{
		GetEnvironment().Render();
	}

	g_SetShaderSharedParams(*this, wndCur
					, m_bUnderWater ? &m_lpWaterEffect->m_waterFogParam : NULL
					, m_lpSceneOutDoorSpaceMgr->m_lpConverMap);

	RenderMainWindowsColor(pWnd);

	//渲染特殊元件，有些元件很难放入视锥裁剪的范畴，可以用这个来渲染
	GetCommonObjectNode().Render();
	//向渲染消息的Listenner发送消息，这样他们可以插入到渲染队列中，
	GetMessageSender(KM_RENDER).PostMessage(KG3DMessage(KM_RENDER));

    

	{
		g_cGraphicsTool.SetCurrentConverMap(m_lpSceneOutDoorSpaceMgr->m_lpConverMap);
		/*
		  must render refr after render the scene
		 */
		if (g_cEngineOption.nWaterDetail >= WD_MEDIUM)
		{
			//GraphicsStruct::RenderState fogEnable(D3DRS_FOGENABLE, FALSE);
			RenderRefractTexture(pWnd);
		}

        g_RenderRenderTwiceInfo(true, false);
		g_cCaption2D.Render();//增加可以渲染头顶名字
		RenderWaterAndSFX();
        g_RenderRenderTwiceInfo(false, true);
        m_EntityMgr.RenderSpecialAlphaModel();

		g_cGraphicsTool.SetCurrentConverMap(NULL);
	}


	if ((this->GetEditState()==SCENESTATE_TERRAIN)&&(!g_bClient))
	{
		if (m_lpSceneOutDoorSpaceMgr)
		{
			m_lpSceneOutDoorSpaceMgr->RenderBrush();
		}
	}

	RenderHelpLine();

	//m_EntityMgr.RenderVisibleObjectBBox();

	if(g_cEngineManager.GetRenderOption( RENDER_ENVIRONMENT))
	{
		GetEnvironment().PostRender();
	}

	if(m_bEditing)
	{
		if(m_lpSceneOutDoorSpaceMgr && m_lpSceneOutDoorSpaceMgr->m_lpRoadMgr)
		{
			KG3DSerpentine* pRoad = m_lpSceneOutDoorSpaceMgr->m_lpRoadMgr->GetCurSerpentine();
			if(pRoad)
			{
				pRoad->RenderSelectMark();
			}
		}
		if(m_lpSceneOutDoorSpaceMgr && m_lpSceneOutDoorSpaceMgr->m_lpRiverMgr)
		{
			KG3DSerpentine* pRiver = m_lpSceneOutDoorSpaceMgr->m_lpRiverMgr->GetCurSerpentine();
			if(pRiver)
			{
				pRiver->RenderSelectMark();
			}
		}
		if( this->GetEditState() == SCENESTATE_ADDPOLYVERTEX)
		{
			RenderPoly();
		}

		if (m_lpLogicEditor && m_lpLogicEditor->GetShowLogicalCell())
		{
			m_EntityMgr.RenderLogicalCells(m_lpLogicEditor->GetShowLogicalTerrenCell(),
				m_lpLogicEditor->GetShowLogicalItemCell(), 
				m_lpLogicEditor->GetShowLogicalSelectedCell(),
				m_pLogicalSceneConnection,this);
		}


		if ((this->GetEditState() == SCENESTATE_TERRAINSELECTION)||(this->GetEditState() == SCENESTATE_TERRAINSELECTIONMODIFY))
		{
			AABBOX box;
			box.AddPosition(m_vPositionSceneSelectionA);
			box.AddPosition(m_vPositionSceneSelectionC);
			g_cGraphicsTool.DrawAABBox(&box,0xFF00FF00);		
		}

		if (m_bIsRenderCopyBrush)
		{
			m_pTerrainBlock->Render(0);
		}
		
		if (this->GetEditState() == SCENESTATE_CELLLOGICAL)
        {
		    //KG3DSceneLogicalEditorBase::UpdateEditingCells(m_lpTerrain->m_lpCurrentTerrainEx,D3DXVECTOR3(0,0,0));
		    //KG3DSceneLogicalEditorBase::RenderLogicalCell();
			
		    if(m_lpLogicEditor)
		    {
			    D3DXVECTOR3 src;
			    D3DXVECTOR3 dst;
			    D3DXVECTOR3 Inter;
				FLOAT fDis = FLT_MAX;
			    pWnd->GetPickRay(&src, &dst, NULL);
			    D3DXVec3Normalize(&dst, &dst);		    
				BOOL bFoundInter = ::RayIntersection(*this, src, dst, &fDis, NULL, NULL);
				if (bFoundInter)
				{
					Inter = src + dst * fDis;			  
				    m_lpLogicEditor->UpdateEditingCells(m_nLogicalSceneX_Start,m_nLogicalSceneZ_Start,this, Inter);
			    }
			    m_lpLogicEditor->RenderLogicalCell();
		    }
			
        }
	}

	if(this->GetEditState() == SCENESTATE_SELECTREGION)
	{
		RenderSceneRegionSelection();
	}
	else if(this->GetEditState() == SCENESTATE_ADDPOLYVERTEX)
	{
		RenderPoly();
	}

	if(m_bEnableShowLogicalSize)
	{
		D3DXVECTOR3 A = D3DXVECTOR3(m_nLogicalSceneX_Start*100.0F,10,m_nLogicalSceneZ_Start*100.0F);
		D3DXVECTOR3 B = A + D3DXVECTOR3(m_nLogicalSceneX_Width*100.0F,0,0);
		D3DXVECTOR3 D = A + D3DXVECTOR3(0,0,m_nLogicalSceneZ_Width*100.0F);
		//GraphicsStruct::RenderState zEnable(D3DRS_ZENABLE,FALSE);
		GraphicsStruct::RenderState fog(D3DRS_FOGENABLE,FALSE);
		GraphicsStruct::RenderState zW(D3DRS_ZWRITEENABLE,FALSE);
		GraphicsStruct::RenderStateAlpha Alpha(0x20,FALSE,TRUE,D3DBLEND_SRCALPHA,D3DBLEND_ONE);
		g_cGraphicsTool.DrawRect(&A,&B,&D,0x8000FF00,NULL);
		AABBOX BOX;
		BOX.A = A;
		BOX.B = B+D-A+D3DXVECTOR3(0,10000,0);
		g_cGraphicsTool.DrawAABBox(&BOX,0x8000FF00);
	}
	if (m_bEditing && m_bEnableShowAppointLogicalRegion)
	{
		GraphicsStruct::RenderState fog(D3DRS_FOGENABLE,FALSE);
		GraphicsStruct::RenderState zW(D3DRS_ZWRITEENABLE,FALSE);
		GraphicsStruct::RenderStateAlpha Alpha(0x20,FALSE,TRUE,D3DBLEND_SRCALPHA,D3DBLEND_ONE);
		D3DXVECTOR3 A = D3DXVECTOR3(m_nLogicalRegionX_Start*3200.0F,20,m_nLogicalRegionZ_Start*3200.0F);
		D3DXVECTOR3 B = A + D3DXVECTOR3(m_nCountLogicalRegionX*3200.0F,0,0);
		D3DXVECTOR3 D = A + D3DXVECTOR3(0,0,m_nCountLogicalRegionZ*3200.0F);

		D3DXVECTOR3 A1 = D3DXVECTOR3(m_nLogicalRegionX_Start*3200.0F,25000,m_nLogicalRegionZ_Start*3200.0F);
		D3DXVECTOR3 B1 = A1 + D3DXVECTOR3(m_nCountLogicalRegionX*3200.0F,0,0);
		D3DXVECTOR3 D1 = A1 + D3DXVECTOR3(0,0,m_nCountLogicalRegionZ*3200.0F);
		g_cGraphicsTool.DrawRect(&A,&B,&D,0x80FF0000,NULL);
		g_cGraphicsTool.DrawRect(&A1,&B1,&D1,0x80FF0000,NULL);
		AABBOX BOX2;
		BOX2.A = A;
		BOX2.B = B+D-A+D3DXVECTOR3(0,25000,0);
		g_cGraphicsTool.DrawAABBox(&BOX2,0x80FF0000);
	}
	if (m_bEditing && m_SceneWndManager->GetCurOutputWnd())
	{
		D3DXMATRIX Proj,View;
		Proj = m_SceneWndManager->GetCurOutputWnd()->GetCamera().GetProjectionMatrix();
		View = m_SceneWndManager->GetCurOutputWnd()->GetCamera().GetViewMatrix();
		RenderSelectedEntityMark(this->GetEditState(),m_SceneWndManager->GetCurOutputWnd()->m_Viewport,Proj,View,pWnd==m_SceneWndManager->GetCurOutputWnd());
	}

    m_BspRender.Render();

	//g_cGraphicsTool.EnableRenderTarget(-1);//不再允许-1值，原来EnableRenderTarget(-1)变成DisableRenderTarget
	//g_Selector.OnRender();
	g_cSoundBallRender.Render();
	//if(pWnd && pWnd->IsEnablePostRender() && g_cEngineOption.bUseRenderTarget_DepthTexture)
	//{
	//	g_cGraphicsTool.DisableRenderTarget();
	//}	
    if ((this->GetEditState() != SCENESTATE_CAMERAANIMATION) &&
        (!g_bClient) && 
        (this->GetEditState() != SCENESTATE_BENCHMARK)
    )
	{
		IEKG3DSceneCameraAnimation *piCameraAnimation = NULL;
		m_pCameraMovement->GetCurCamAnimation(&piCameraAnimation);

		KG3DSceneCameraAnimation *pKG3DSceneCameraAnimation = static_cast<KG3DSceneCameraAnimation *>(piCameraAnimation);

		///CameraAnimation Operation
		///如果现在列表中没有任何的CamAni的话，Get这个方法会自动连接一个空的CamAni。
		hr = pKG3DSceneCameraAnimation->RenderCameraKeyFrame(0);
		KGLOG_COM_PROCESS_ERROR(hr);
	}
	
	//通知外部渲染事件
	g_cEngineManager.OnRenderEnd(0);
	//KG3DSelector的渲染要放在ProcessPostRender之前， 因为ProcessPostRender会清除Zbuffer
	
	//////////////////////////////////////////////////////////////////////////
	// Post Render Process
	ProcessPostRender(pWnd);
	if (g_bRePlay)
	{
		static KG3DTexture* s_pLogoTexture = NULL;
		if (s_pLogoTexture == NULL)
		{
			g_cTextureTable.Get1NewResource(&s_pLogoTexture,0,0);
			s_pLogoTexture->LoadFromFile("data\\benchmark_logo.dds",0,TEXOP_COMMON | TEXOP_RELOAD | TEXOP_DISABLEMIPMAP | TEXOP_DISABLESCALE);
		}

		D3DVIEWPORT9 viewPort;
		g_pd3dDevice->GetViewport(&viewPort);
		if (s_pLogoTexture != NULL)
		{
			LPDIRECT3DTEXTURE9 pLogoTexture = s_pLogoTexture->GetTexture();
			if (pLogoTexture != NULL)
				g_cGraphicsTool.DrawScreenRect(&D3DXVECTOR2(0,0),&D3DXVECTOR2((FLOAT)viewPort.Width,(FLOAT)viewPort.Height),0,0xffffffff,pLogoTexture);
		}
	}
	ShowInfo();

	//////////////////////////////////////////////////////////////////////////
	//结束渲染前通知entitymge
	m_EntityMgr.OnRenderEnd();
	m_lpSceneOutDoorSpaceMgr->DoDynamicUnloadEntityShells();

	hr = S_OK;
Exit0:
	g_cGraphicsTool.ReleaseFullControlOfFog(*this);//无论成功与否都需要调用这个
	if (bHasRenderWindowBegin)
	{
		hr = RenderWindowEnd(pWnd);
		if (FAILED(hr))
		{
			//KGLogPrintf(KGLOG_ERR, "KG3DSceneSceneEditor::RenderMainWindow(%.8x) RenderWindowEnd() failed\n", pWnd);
		}
	}

	if(g_bEnableScaleOutput)
	{
		KG3DRenderTargetTool* pTool = g_cGraphicsTool.GetScaleOutputRenderTool();
		if(pTool)
		{
			pTool->EndNewRenderTarget();
			pTool->ScaleOutput(pWnd);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	ProcessMovieRecForMainWindows(pWnd);

	DWORD dwCost = timeGetTime() - dwNowTime;
	if(g_cEngineOption.bEnableTimeOptimizeLog && dwCost >= 50)
	{
		KGLogPrintf(KGLOG_ERR, "TimeOptimize %d KG3DSceneSceneEditor::RenderMainWindow.",dwCost);
	}
	//////////////////////////////////////////////////////////////////////////

	return hr;
}

void KG3DSceneSceneEditor::EnableShowSelectedBsp(BOOL bShow)
{
    ULONG uSelCount = this->GetSelectionTool().GetSelectionCount();
    
    if (uSelCount <= 0)
        return;

    vector<KG3DModel*> vpBspModel;

    {
        TypeEntityAccessProxy& proxy = this->GetSelectionTool().GetSelectionAccessProxy();
        
        for(proxy.Begin(); ! proxy.IsEnd(); proxy.StepForward())
        {
            KG3DSceneEntity& entityRef = proxy.GetCur();

            if (entityRef.GetType() != SCENEENTITY_OBJECT)
                continue;

            KG3DRepresentObject* pObj = (KG3DRepresentObject*)entityRef.m_pPointer;
            pObj->GetBspModel(&vpBspModel);
        }
    }

    if (bShow)
        for (size_t i = 0; i < vpBspModel.size(); ++i)
            m_BspRender.Push(vpBspModel[i]);
    else
        for (size_t i = 0; i < vpBspModel.size(); ++i)
            m_BspRender.Remove(vpBspModel[i]);

}


static unsigned s_uFPS = 0;

HRESULT KG3DSceneSceneEditor::ShowInfo()
{
	static unsigned s_uFrameCount = 0;
	static DWORD s_dwLastTime = timeGetTime();		
	DWORD dwCurrentTime = timeGetTime();				
	TCHAR szInfo[512];

	++s_uFrameCount;
	if((dwCurrentTime - s_dwLastTime) >= 1000)
	{
		s_uFPS        = (unsigned)((s_uFrameCount * 1000.0f / (dwCurrentTime - s_dwLastTime)) + 0.5);
		s_dwLastTime  = dwCurrentTime;
		s_uFrameCount = 0;
	}
	//srand((int)time(NULL));
	//int nFpsStep = 80 + rand() % 10;
	//snprintf(szInfo, sizeof(szInfo), "FPS: %u", nFpsStep);
    snprintf(szInfo, sizeof(szInfo), "FPS: %u", s_uFPS);
	szInfo[sizeof(szInfo) - 1] = '\0';
	g_cGraphicsTool.DrawDebugText(szInfo);
	
    if (!g_bClient)
    {
		
		snprintf(szInfo, sizeof(szInfo), "镜头移动速度：X = %d,Y = %d",g_eEngineOption.nFlySpeedX,g_eEngineOption.nFlySpeedY);
		szInfo[sizeof(szInfo) - 1] = '\0';
		g_cGraphicsTool.DrawDebugText(szInfo);

		if(m_pRepresentNPC)
		{
			D3DXVECTOR3 vPos(0,0,0);
			float fMoveSpeed = 0;
			m_pRepresentNPC->GetCurPosition(&vPos);

			fMoveSpeed = m_pRepresentNPC->GetMoveSpeed();

			//
			KG3DCamera& CAMERA = m_SceneWndManager->GetCurOutputWnd()->GetCamera();
			D3DXVECTOR3 POS;
			D3DXVECTOR3 LOOKAT;
			CAMERA.GetPosition(&POS);
			CAMERA.GetLookAtPosition(&LOOKAT);

			D3DXVECTOR3 DELTA = LOOKAT - POS;
			FLOAT xz = sqrt(DELTA.x * DELTA.x + DELTA.z * DELTA.z);
			FLOAT yaw;
			yaw = atan(DELTA.y / xz);
			yaw = yaw * 180 / D3DX_PI;

			if(g_eEngineOption.bUseLogical)
			{
				int nX,nY,nZ;
				SceneToLogical(&nX,&nY,&nZ,vPos);

				snprintf(szInfo, sizeof(szInfo), "SceneRunner: Speed = %.0f, 当前坐标:(%.0f,%.0f,%.0f) 逻辑坐标：(%d,%d,%d) 镜头:(%.0f,%.0f,%.0f) 角度:%.1f",
					fMoveSpeed,
					vPos.x, vPos.y, vPos.z,
					nX, nY, nZ,
					POS.x, POS.y, POS.z,
					yaw
					);
			}
			else
			{
				snprintf(szInfo, sizeof(szInfo), "SceneRunner: Speed = %.0f, 当前坐标:(%.0f,%.0f,%.0f) 镜头:(%.0f,%.0f,%.0f) 角度:%.1f",
					fMoveSpeed,
					vPos.x,vPos.y,vPos.z,
					POS.x, POS.y, POS.z,
					yaw
					);
			}

			szInfo[sizeof(szInfo) - 1] = '\0';
			g_cGraphicsTool.DrawDebugText(szInfo);
		}
		
        if (g_cEngineManager.GetRenderOption(RENDER_VIDEO_MEMERY_INFO))
            g_cGraphicsTool.DrawVideoInfo(200, 10);

		if (GetEditSpaceNode() && GetEditSpaceNode() != m_lpSceneOutDoorSpaceMgr)
		{
			g_cGraphicsTool.DrawFocusRect(m_lpMainWindow->m_Viewport, 4, 0xFFFFFF00);
		} 
	}
	else
	{
		if (g_cEngineManager.GetRenderOption(RENDER_VIDEO_MEMERY_INFO))
			g_cGraphicsTool.DrawVideoInfo(200, 10);

	}

	{
		DWORD dwObject = m_lpSceneOutDoorSpaceMgr->GetLoadedObjectShellCount();
		DWORD dwModel = g_cModelTable.GetResourceCount();
		DWORD dwMesh  = g_cMeshTable.GetResourceCount();
		DWORD dwMtl   = g_cMaterialTable.GetResourceCount();
		DWORD dwTexture = g_cTextureTable.GetResourceCount();
		DWORD dwAni = g_cClipTable.GetResourceCount();

		DWORD dwGrass = 0,dwGrassTotal = 0;
		g_cTerrainBlockGrassShareVBHighPool.GetState(dwGrass,dwGrassTotal);

		DWORD dwTerrianBlock = 0,dwTerrianBlockTotal = 0;
		g_cTerrainBlockPool.GetState(dwTerrianBlock,dwTerrianBlockTotal);
		DWORD dwCientModel = m_EntityMgr.GetVisibleClientModels();
        DWORD dwVisableObj = m_EntityMgr.GetVisableObjectNum();
        DWORD dwVisableZoom = 0;
        DWORD dwVisableSection = 0;
        DWORD dwVisabeRegion = 0;
		DWORD dwVisibleGrass = 0;
        BOOL  bWaterVisible = m_EntityMgr.IsWaterVisible();
        TCHAR szCurrNode[128];
		DWORD dwTerrainVB = 0,dwTerrainVBTotal = 0;
		g_cTerrainBlockBlendFactorShareVB.GetState(dwTerrainVB,dwTerrainVBTotal);

        *szCurrNode = 0;

        KG3DSceneSpaceNode* pCurrNode = NULL;
        KG3DCamera* pCamera = g_cGraphicsTool.GetCamera();
        
        if (pCamera)
            pCurrNode = GetSpaceNodeByHandle(pCamera->m_uSceneSpaceNode);
        
        if (pCurrNode)
        {
            _tcsncpy(szCurrNode, pCurrNode->GetNodeName(), sizeof(szCurrNode));
            TCHAR* t = _tcsrchr(szCurrNode, TEXT('\\'));
            if (t)
            {
                _tcsncpy(szCurrNode, ++t, sizeof(szCurrNode));
                if ((t = _tcsrchr(szCurrNode, TEXT('.'))) != NULL)
                    *t = 0;
            }
        }

        m_EntityMgr.GetVisable(dwVisableZoom, dwVisableSection, dwVisabeRegion,dwVisibleGrass);

        snprintf(szInfo, sizeof(szInfo), "Obj:%4d Model:%4d Mesh:%4d Mtl:%4d Text:%4d Ani:%4d\n"
            "GrassVB:%4d TerrainBlock:%4d Client:%4d\n\n"
            "Visable Object  : %d\n"
            "Visable Zoom    : %d\n"
            "Visable Section : %d\n"
            "Visable Region  : %d\n"
			"Visable Grass   : %d\n"
            "Water Visible   : %d:%d\n\n"
            "In Space Node   : %s\n"
			"TerrainVB       : %d %d\n"
            "ModelBsp Enable : %s",
			dwObject,dwModel,dwMesh,dwMtl,dwTexture,dwAni,dwGrass,dwTerrianBlock,
			dwCientModel, dwVisableObj, dwVisableZoom, 
			dwVisableSection, dwVisabeRegion,dwVisibleGrass, 
            m_EntityMgr.m_lpTerrainRender->IsWaterVisible(), bWaterVisible,
            szCurrNode,
			dwTerrainVB,dwTerrainVBTotal,
            (g_cEngineOption.bUseModelBsp ? "TRUE" : "FALSE"));

		szInfo[sizeof(szInfo) - 1] = '\0';
		g_cGraphicsTool.DrawDebugText(szInfo);
	}

	if (g_cEngineManager.GetRenderOption(RENDER_INFO))
	{
		g_cGraphicsTool.DrawAllDebugTextsAtLast();//绘制所有的Debug文字
	}
	else
	{
		g_cGraphicsTool.FlushDebugTexts();
	}

	return S_OK;
}

HRESULT KG3DSceneSceneEditor::RenderClientWindowBegin(KG3DSceneOutputWnd& Wnd, DWORD dwFlags)
{
	HRESULT hr = E_FAIL;

	if ((dwFlags & KG3DUI_SCENE_FLAGS_DISABLE_VIEWPORT) == 0)
	{

		if(!g_bEnableScaleOutput)
		{
			hr = g_pd3dDevice->SetViewport(&Wnd.m_Viewport);
			KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

			D3DRECT rect = { Wnd.m_Viewport.X, Wnd.m_Viewport.Y, Wnd.m_Viewport.X + Wnd.m_Viewport.Width, Wnd.m_Viewport.Y + Wnd.m_Viewport.Height };

			hr = g_pd3dDevice->Clear(1, &rect, D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0x00FFFFFF, 1.0f, 0);
			KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);
		}
		else
		{
			hr = g_pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0x00FFFFFF, 1.0f, 0);
			KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);
		}
	}

	Wnd.GetCamera().SetCamera();

	g_cGraphicsTool.SetCamera(&Wnd.GetCamera());

	if (Wnd.IsEnablePostRender())
	{
		Wnd.RenderTargetSet(dwFlags & KG3DUI_SCENE_FLAGS_ENABLE_ALPHA);

		hr = g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0x00808080, 1.0f, 0);
		KGLOG_COM_PROCESS_ERROR(hr);
	}

	return S_OK;
Exit0:
	return hr;
}

HRESULT KG3DSceneSceneEditor::RenderClientWindowEnd(KG3DSceneOutputWnd& Wnd)
{
	ProcessPostRender(&Wnd);
    
    //if (Wnd.m_pCamera)
    //    Wnd.m_pCamera->ClearOffset();

	return S_OK;
}


HRESULT KG3DSceneSceneEditor::RenderClientWindow(KG3DSceneOutputWnd& Wnd, DWORD dwFlags)
{
	//在Scene的主入口，设置全局的参数
	{
		KG3DLight& SunLight  = g_cGraphicsTool.GetSunLight(*this);
		g_GetRenderGlobalStateModifier().SetSunLightParam(SunLight.GetParam());

		KG3DLight& PlayerLight = g_cGraphicsTool.GetPlayerLight(*this);
		g_GetRenderGlobalStateModifier().SetPlayerLightParam(PlayerLight.GetParam());
	}

	DWORD dwStartTime = timeGetTime();
	HRESULT hrResult = E_FAIL;
    HRESULT hrRetCode = E_FAIL;
    KG3DCamera *pCamera = NULL;
    D3DXVECTOR3 CamLookAtPos;
    DWORD dwLastZEnable = FALSE;
    DWORD dwLastZWriteEnable = FALSE;
    float a,b,c,d;
    BOOL bHasRenderWindowBegin = FALSE;
	DWORD dwT[8];

	//////////////////////////////////////////////////////////////////////////
	m_fZFar = g_cEngineOption.fCameraDistance;

	if(m_uOption & SCENE_FIX_CAMRERANGLE)
	{
		m_fZFar = 40000;
		m_fFovy = D3DX_PI * 40 / 180;
	}
	else
		m_fFovy = g_cEngineOption.fCameraAngle;

	m_Sample_Mag = (D3DTEXTUREFILTERTYPE)(g_cEngineOption.nSampMagFilter);
	m_Sample_Min = (D3DTEXTUREFILTERTYPE)(g_cEngineOption.nSampMinFilter);
	m_Sample_Mip = (D3DTEXTUREFILTERTYPE)(g_cEngineOption.nSampMipFilter);
	//////////////////////////////////////////////////////////////////////////
	m_pCurRenderWnd = &Wnd;

	g_cGraphicsTool.ApplyLight(*this);

	m_lpMainWindow = &Wnd;

	ProcessForCameraAnimation(&Wnd);

	dwT[0] = timeGetTime() - dwStartTime;

	g_cGraphicsTool.SetCamera(&Wnd.GetCamera());

	//////////////////////////////////////////////////////////////////////////
	if (Wnd.NeedPreZRender())
		Wnd.PreZRender(this);
	PrepareRenderMainWindow(&Wnd);
	PreprocessWaterScene(Wnd,FALSE);

	dwT[1] = timeGetTime() - dwStartTime;

	if(g_cEngineOption.bEnableDynamicConverMap)
	{	
		RenderConverMapTexture(&Wnd);
	}
	//////////////////////////////////////////////////////////////////////////


	///CTH,调节GPS，设定其位置
    pCamera = &(Wnd.GetCamera());
	pCamera->GetLookAtPosition(&CamLookAtPos);
	m_GameGPS.AdjustPos(CamLookAtPos);

	if(g_bEnableScaleOutput)
	{
		KG3DRenderTargetTool* pTool = g_cGraphicsTool.GetScaleOutputRenderTool();
		if(pTool)
		{
			pTool->BeginNewRenderTarget(NULL,NULL,NULL);
		}
	}   
	
	hrRetCode = RenderClientWindowBegin(Wnd, dwFlags);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    bHasRenderWindowBegin = TRUE;

	g_pd3dDevice->GetRenderState(D3DRS_ZENABLE, &dwLastZEnable);
	g_pd3dDevice->GetRenderState(D3DRS_ZWRITEENABLE, &dwLastZWriteEnable);
	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
	g_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	g_bHideVisibleSetEdgeMesh = TRUE;

	pCamera->GetPerspective(&a, &b, &c, &d);
	pCamera->SetPerspective(m_fFovy, b, 20, m_fZFar);

	dwT[2] = timeGetTime() - dwStartTime;
	//////////////////////////////////////////////////////////////////////////
	if(Wnd.IsEnablePostRender() && g_cEngineOption.bUseRenderTarget_DepthTexture)
	{
		g_cGraphicsTool.EnableRenderTarget(0);
		g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0 );
	}

	{
		GraphicsStruct::RenderState fog(D3DRS_FOGENABLE,FALSE);
		ProcessEarlyZ(&Wnd);
	}

	dwT[3] = timeGetTime() - dwStartTime;
	if (Wnd.NeedPreColorRender())
		Wnd.PreColorRender(this);
	if(Wnd.IsEnablePostRender() && g_cEngineOption.bUseRenderTarget_DepthTexture)
		g_cGraphicsTool.EnableRenderTarget(1);

	//////////////////////////////////////////////////////////////////////////
	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, 0x007f7f7f, 1.0f, 0 );

	if(g_cEngineManager.GetRenderOption( RENDER_ENVIRONMENT))
	{
		GetEnvironment().Render();
	}
    
	dwT[4] = timeGetTime() - dwStartTime;

	g_SetShaderSharedParams(*this, Wnd
		, m_bUnderWater ? &m_lpWaterEffect->m_waterFogParam : NULL
		, m_lpSceneOutDoorSpaceMgr->m_lpConverMap);

	RenderMainWindowsColor(&Wnd);

	dwT[5] = timeGetTime() - dwStartTime;

	//g_Selector.OnRender();
	if(g_cEngineManager.GetRenderOption(RENDER_ENVIRONMENT))
	{
		//渲染特殊元件，有些元件很难放入视锥裁剪的范畴，可以用这个来渲染
		GetCommonObjectNode().Render();
		//向渲染消息的Listenner发送消息，这样他们可以插入到渲染队列中，
		GetMessageSender(KM_RENDER).PostMessage(KG3DMessage(KM_RENDER));

        GetEnvironment().PostRender();
	}
    	
	dwT[6] = timeGetTime() - dwStartTime;
	{
		g_cGraphicsTool.SetCurrentConverMap(m_lpSceneOutDoorSpaceMgr->m_lpConverMap);

        /*
        must render refr after render the scene
        */
        if (g_cEngineOption.nWaterDetail >= WD_MEDIUM)
        {
            //GraphicsStruct::RenderState fogEnable(D3DRS_FOGENABLE, FALSE);
            RenderRefractTexture(&Wnd);
        }

        g_RenderRenderTwiceInfo(true, false);
        g_cCaption2D.Render();
		RenderWaterAndSFX();
        g_RenderRenderTwiceInfo(false, true);
        m_EntityMgr.RenderSpecialAlphaModel();

		g_cGraphicsTool.SetCurrentConverMap(NULL);
	}

	RenderHelpLine();
	dwT[7] = timeGetTime() - dwStartTime;
    //test 
    /*
    if (1)
    {
        set<KG3DModel*>::iterator s = m_EntityMgr.m_lpCurrentCullData->m_setVisiableModels.begin();
        set<KG3DModel*>::iterator send = m_EntityMgr.m_lpCurrentCullData->m_setVisiableModels.end();
        while (s!=send)
        {
            KG3DModel* pModel = *s;
            if (pModel && pModel->m_bSelectable)
            {
                g_cGraphicsTool.DrawAABBox(&pModel->m_BBox, 0xffffffff);
            }
            ++s;
        }

    }
    */
	g_cGraphicsTool.DrawAllDebugTextsAtLast();//如果不调用这句就必须FlushText

	m_EntityMgr.OnRenderEnd();
	m_lpSceneOutDoorSpaceMgr->DoDynamicUnloadEntityShells();

	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, dwLastZEnable);
	g_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, dwLastZWriteEnable);

	ShowInfo();	

    hrResult = S_OK;
Exit0:
	g_cGraphicsTool.ReleaseFullControlOfFog(*this);//无论成功与否都需要调用这个
    if (bHasRenderWindowBegin)
    {
        RenderClientWindowEnd(Wnd);
        bHasRenderWindowBegin = FALSE;
    }

	if(g_bEnableScaleOutput)
	{
		KG3DRenderTargetTool* pTool = g_cGraphicsTool.GetScaleOutputRenderTool();
		if(pTool)
		{
			pTool->EndNewRenderTarget();

			pTool->ScaleOutput(&Wnd);
		}
	}

	DWORD dwCost = timeGetTime() - dwStartTime;
	if(g_cEngineOption.bEnableTimeOptimizeLog && dwCost > 20)
	{
		KGLogPrintf(KGLOG_INFO,"TimeOptimize %d KG3DSceneSceneEditor::RenderClientWindow 1_%d 2_%d 3_%d 4_%d 5_%d 6_%d 7_%d 8_%d ",dwCost,
			dwT[0],dwT[1],dwT[2],dwT[3],dwT[4],dwT[5],dwT[6],dwT[7]);
	}
    return hrResult;
}

HRESULT KG3DSceneSceneEditor::RenderTextureWindows( KG3DSceneOutputWnd *pWnd )
{

	_ASSERTE(pWnd);
	OUTPUTWND_TEXTURE_PARAM TempParam;
	HRESULT hr = E_FAIL;
	hr = pWnd->GetTextureWndParam(&TempParam);
	KG_COM_PROCESS_ERROR(hr);
	RenderWindowBegin(pWnd,TRUE);
	{
		RECT TempRect = pWnd->m_Rect;
		FRECT TempFRect(0, 0, static_cast<float>(TempRect.right - TempRect.left), static_cast<float>(TempRect.bottom - TempRect.top));
		D3DXVECTOR2 V1, V2;
		TempFRect.ToPosition(&V1, &V2);

		//KG3DTexture* pTex = NULL;
		//hr = g_cTextureTable.GetResource(&pTex, TempParam.dwTextureID);
		if (SUCCEEDED(hr) /*&& pTex*/)
		{
			GraphicsStruct::SamplerState SS_0V(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
			GraphicsStruct::SamplerState SS_0U(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
			GraphicsStruct::SamplerState SS_MIN(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);//渲染贴图窗口的确是用应该用点采样
			GraphicsStruct::SamplerState SS_MAG(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
			GraphicsStruct::SamplerState SS_MIP(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
			GraphicsStruct::RenderState  RS_Fog(D3DRS_FOGENABLE, FALSE);
			g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
			g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			g_cGraphicsTool.DrawScreenRect(&V1, &V2, 0, 0xFFFFFFFF, TempParam.dwTextureID);
		}

	}
	RenderWindowEnd(pWnd);
	return hr;
Exit0:
	return E_FAIL;
}
//用于策划工具的预览渲染
HRESULT KG3DSceneSceneEditor::RenderScenePreviewWindows(KG3DSceneOutputWnd *pWnd)
{
	KG3DRepresentObject* pObj = NULL;
	HRESULT hr = this->GetObjectInHandle(reinterpret_cast<IEKG3DRepresentObject**>(&pObj));
	_KG3D_DEBUG_SUPPERSUB_POINTER_EQUAL(IEKG3DRepresentObject, KG3DRepresentObject);
	//if(m_lpHandleObject)
	if(SUCCEEDED(hr))
	{
		RenderWindowBegin(pWnd,TRUE);
		D3DXVECTOR3 Pos(0,0,0);
		/*m_lpHandleObject*/pObj->GetTranslation(&Pos);
		float L = /*m_lpHandleObject*/pObj->m_BBox.GetLength();
		float W = /*m_lpHandleObject*/pObj->m_BBox.GetWidth();
		float H = /*m_lpHandleObject*/pObj->m_BBox.GetHeight();
		float K  = sqrtf(L*L+W*W+H*H)*2.0F;
		float RotationAngle = g_cGraphicsTool.GetNowTime()*0.001F;
		D3DXVECTOR3 V(sinf(RotationAngle),0.3F,cosf(RotationAngle));
		D3DXVECTOR3 Lookat = V*K + Pos;

		Pos.y += H/2;

		D3DXVECTOR3 vDir = Lookat - Pos;
		vDir *= m_fWheel;
		Lookat = Pos + vDir;

		/*pWnd->GetCamera().SetLookAtPosition(Pos);
		pWnd->GetCamera().SetPosition(Lookat);*/

		pWnd->GetCamera().SetPositionLookAtUp(&Lookat, &Pos, NULL);
		pWnd->GetCamera().RestoreGlobalProject();

		/*m_lpHandleObject*/pObj->Render(0);

		if(/*m_lpHandleObject*/pObj->GetType()==REPRESENTOBJECT_SET)
		{
			KG3DRepresentObjectSet *pSet = 
				static_cast<KG3DRepresentObjectSet*>(/*m_lpHandleObject*/pObj);

			RECT destRect1;
			SetRect(&destRect1, 2, 0, 0, 80);
			TCHAR strCameraInfo[256];
			sprintf(strCameraInfo, "Set %d of %d",pSet->GetCurrentModel()+1,pSet->GetNumModelofSet());

			if(g_cEngineManager.GetRenderOption(RENDER_INFO))
			{
				/*
				LPD3DXFONT pFont = g_cGraphicsTool.GetFont();
				if (pFont)
				{
				pFont->DrawTextA(NULL, strCameraInfo, -1, &destRect1, 
				DT_NOCLIP, D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f));
				}*/
				g_cGraphicsTool.DrawDebugText(strCameraInfo);//现在全局统一排序绘制
			}
		}
		g_cCaption2D.Render();
		RenderWindowEnd(pWnd);
	}


	return S_OK;
}


HRESULT KG3DSceneSceneEditor::RenderObjectPreviewWindows(KG3DSceneOutputWnd *pWnd)
{
	RenderWindowBegin(pWnd,TRUE);

	KG3DRepresentObject* pObj = NULL;
	HRESULT hr = this->GetObjectInHandle(reinterpret_cast<IEKG3DRepresentObject**>(&pObj));
	_KG3D_DEBUG_SUPPERSUB_POINTER_EQUAL(IEKG3DRepresentObject, KG3DRepresentObject);
	if (SUCCEEDED(hr))
	{
		BOOL bAlertRange = g_bRenderAlertRange;
		BOOL bAttackRange = g_bRenderAttackRange;
		EnableRenderAlertRange(FALSE);
		EnableRenderAttackRange(FALSE);

		if(m_bShowWallConnectionPoint)
		{
			pObj->SetTranslation(&D3DXVECTOR3(0,0,0));
		}
		D3DXVECTOR3 ObjectPos = pObj->GetTranslation();
		
		pObj->ComputeBBox();
		
		{
			float ObjRadius  = pObj->m_BBox.GetDiameter() / 2;
			float RotationAngle = g_cGraphicsTool.GetNowTime()*0.0001F;
			D3DXVECTOR3 Direction(sinf(RotationAngle),0.5f,cosf(RotationAngle));
			D3DXVec3Normalize(&Direction, &Direction);

			/*摄像机焦点在物件正中，假设角度为斜下45。那么可以知道摄像机如果要
			在整个旋转的过程中看见整个BBox，那么需要至少看见BBox在对角的斜切面
			那么可以计算出摄像机的距离

			后来发现45度太大，改小了一点。但距离不变。编辑器看上去效果挺好。
			*/


			KG3DCamera& Cam = pWnd->GetCamera();
			FLOAT fHalfFovY = Cam.GetPerspectiveData().fFovy / 2;

			FLOAT fMinDistance = fHalfFovY < FLT_EPSILON 
				? 2 * ObjRadius 
				: ObjRadius / tan(fHalfFovY) + Cam.GetPerspectiveData().fzNear + 10;	//10是偏移

			D3DXVECTOR3 vCameraLookAt = ObjectPos;
			vCameraLookAt.y += pObj->m_BBox.GetHeight() / 2;
			D3DXVECTOR3 vCameraPos = Direction * fMinDistance + vCameraLookAt;

			Cam.SetPositionLookAtUp(&vCameraPos, &vCameraLookAt, NULL);
		}


		pObj->RenderHierarchy();//pObj->Render(0);

		if(m_bShowWallConnectionPoint)
		{
			g_pd3dDevice->SetRenderState(D3DRS_ZENABLE,D3DZB_FALSE);

			g_cGraphicsTool.DrawPoint(m_vWallConnectionPointA,0xFF00FF00);
			g_cGraphicsTool.DrawPoint(m_vWallConnectionPointB,0xFFFF0000);
			g_cGraphicsTool.DrawLine(&m_vWallConnectionPointA,&m_vWallConnectionPointB,0xFF00FF00,0xFFFF0000);

			m_vWallMeasureTool.Render(&pWnd->GetCamera(),pWnd->m_Viewport);
		}

		EnableRenderAlertRange(bAlertRange);
		EnableRenderAttackRange(bAttackRange);
	}

	RenderWindowEnd(pWnd);	
	return S_OK;
}

HRESULT KG3DSceneSceneEditor::LoadOneMap(const char cszFileName[], unsigned uFileNameHash, unsigned uOption)
{
	return S_OK;
}

HRESULT KG3DSceneSceneEditor::LoadPreloadNpc(LPCSTR strFileName)
{
	HRESULT hResult = E_FAIL;
	IFile *pFile = NULL;
	TCHAR strInput[MAX_PATH];
	unsigned int uSize = 0;
	KG_PROCESS_ERROR(strFileName);

	pFile = g_OpenFile(strFileName);
	KG_PROCESS_ERROR(pFile);

	uSize = pFile->Read(strInput, MAX_PATH);
	while (uSize)
	{
		m_PreloadModelsName.push_back(strInput);

		/*KG3DModel *pModel = NULL;
		hResult = g_cModelTable.LoadResourceFromFile(strInput, 0, 0, (IKG3DResourceBase**)&pModel);
		if (FAILED(hResult))
		{
		KGLogPrintf(KGLOG_ERR, "Preload model %s failed.\n", strInput);
		}
		else
		{
		m_PreloadModels.push_back(pModel);
		}*/
		uSize = pFile->Read(strInput, MAX_PATH);
	}
	m_nNumPreloadModels = (int)m_PreloadModelsName.size();
	hResult = S_OK;
Exit0:
	KG_COM_RELEASE(pFile);
	return hResult;
}

HRESULT KG3DSceneSceneEditor::LoadFromFile(const char cszFileName[], unsigned uFileNameHash, unsigned uOption)
{
    HRESULT hrResult = E_FAIL;
    HRESULT hrRetCode = E_FAIL;
	DWORD dwTime = timeGetTime();

	BOOL bIsPassRecord = FALSE;
	if (g_cRecorder.IsRecord() && !g_cRecorder.IsLock())
	{
		m_dwRecordID =  KG3DRecorder::GetMasterID();
		KG3DRecorder::Parameters MyParameters;
		MyParameters.nNum = 4;
		MyParameters.size_tDataSize = sizeof(TrueType) + 
			MAX_PATH * sizeof(char) + sizeof(uFileNameHash) + sizeof(uOption);
		char* pData = new char[MyParameters.size_tDataSize];
		MyParameters.pData = pData;
		MyParameters.nFrom[0] = 0;		
		*(TrueType*)(&pData[MyParameters.nFrom[0]]) = TT_SCENEEDITOR;
		MyParameters.nFrom[1] = sizeof(TrueType);
		strcpy(&pData[MyParameters.nFrom[1]],cszFileName);
		MyParameters.nFrom[2] = sizeof(TrueType) + MAX_PATH * sizeof(char);
		*(unsigned*)(&pData[MyParameters.nFrom[2]]) = uFileNameHash;
		MyParameters.nFrom[3] = sizeof(TrueType) + MAX_PATH * sizeof(char) + sizeof(uFileNameHash);
		*(unsigned*)(&pData[MyParameters.nFrom[3]]) = uOption;
		g_cRecorder.RecordEvent(m_dwRecordID,FT_KG3DSCENEEDITOR_LOADFROMFILE,
			GetCurrentTime(),MyParameters);
		bIsPassRecord = TRUE;
		g_cRecorder.Lock();
	}


	m_uOption = uOption;

	m_scName = cszFileName;//后续的加载可以用这个来找到资源
	KG3DScene* pCurSceneSave = g_cGraphicsTool.GetCurScene();
	g_cGraphicsTool.SetCurScene(this);//后面的初始化很可能需要这句// Last modify by Chen Tianhong: 2007-8-25 17:51:45
	
	BOOL bRet = FALSE;
	KG_CUSTOM_HELPERS::StrPathSplited pathSplited;

	{
		KGLogPrintf(KGLOG_WARNING,"KG3DSceneSceneEditor::LoadFromFile %s",cszFileName);
	}

	//新建的状态
	if(_strcmpi(cszFileName,"")==0)
	{

		{
			GetEnvironment().LoadMapSetting(NULL, NULL);
		}
		if (!m_lpRegionDWInfoManager)
		{
			KG3DRegionInfoManager* pRM = NULL;
			hrRetCode = g_cRegionInfoManagerTable.Get1NewResource(&pRM, TYPE_RIM_DW, 0);///注意这个创建的是DW的InfoManager
			if (SUCCEEDED(hrRetCode))
			{
				_ASSERTE(pRM);
				m_lpRegionDWInfoManager = pRM;
				_KG3D_DEBUG_FAIL_REPORT_MSG(m_lpRegionDWInfoManager, _T("SSE创建")_STRINGER(KG3DRegionDWInfoManager));
			}
		}


		//////////////////////////////////////////////////////////////////////////
		//创建一个大地图
		m_lpSceneOutDoorSpaceMgr->InitOutDoor(0,0,5,5,"NewFile.map");		

		//生成点光源
		//GetCommonObjectNode().Request(KG3DCOBJ_HANDLE(KG3DTYPE_ENVEFF_POINTLIGHT_TABLE, 0), NULL, 0, 0, NULL);

		goto Exit1;

	}
	
	KGLOG_PROCESS_ERROR(_tcslen(cszFileName) < MAX_PATH && _T("保存路径太长，下面的路径函数处理会失败"));

	{
		//载入地形
		g_cGraphicsTool.SetCurScene(this);//后面的初始化很可能需要这句// Last modify by Chen Tianhong: 2007-8-25 17:51:45
		hrRetCode = m_lpSceneOutDoorSpaceMgr->LoadFromFile(cszFileName, 0, 0);
		KGLOG_COM_PROCESS_ERROR(hrRetCode);

		m_lpSceneOutDoorSpaceMgr->PauseMultiThreadLoad(TRUE);

		if(uOption & SCENE_DISABLE_DYNAMICLOAD)
			m_lpSceneOutDoorSpaceMgr->ForceLoadAllEntity();
		else
		{
			D3DXVECTOR3 vPos;
			m_GameGPS.GetPos(vPos);
			D3DXVECTOR2 vPos2(vPos.x,vPos.z),A,C;
			const float fRange = 12000.0f;
			A = vPos2 - D3DXVECTOR2(fRange,fRange);
			C = vPos2 + D3DXVECTOR2(fRange,fRange);
			m_lpSceneOutDoorSpaceMgr->ForceLoadEntity(A,C);
		}

		m_lpSceneOutDoorSpaceMgr->PauseMultiThreadLoad(FALSE);

		TCHAR EntityName[256];
		TCHAR LayerName[256];

		TCHAR strLightFileName[256];
		TCHAR strLightmapDir[256];


		{
			TCHAR strPathBuffer[MAX_PATH] = _T("");

			errno_t errCode = _tcscpy_s(strPathBuffer, cszFileName);
			KGLOG_PROCESS_ERROR(errCode == 0 && _T("Scene加载，路径非法，加载失败"));

			bRet = KG_CUSTOM_HELPERS::StrReplace(strPathBuffer, _T('/'), _T('\\'));
			_ASSERTE(bRet);

			bRet = KG_CUSTOM_HELPERS::StrPathSplit(strPathBuffer, pathSplited);
			KGLOG_PROCESS_ERROR(bRet && _T("Scene加载，路径分解失败，加载失败"));
		}

		wsprintf(EntityName,"%s%s%s.Entity",pathSplited.driver,pathSplited.dir,pathSplited.fileName);
		wsprintf(LayerName,"%s%s%s.Layer",pathSplited.driver,pathSplited.dir,pathSplited.fileName);
		wsprintf(strLightFileName, "%s%s%s.lt", pathSplited.driver, pathSplited.dir, pathSplited.fileName);
		//wsprintf(strCameraAniFileName, "%s%s%s.CameraAni", pathSplited.driver, pathSplited.dir, pathSplited.fileName);
		wsprintf(strLightmapDir, "%s%s\\%sLightMaps\\", pathSplited.driver, pathSplited.dir,pathSplited.fileName);

		{
			//<SCENE_SETTING>
			TCHAR strPathBuffer[MAX_PATH * 2] = _T("");
			ZeroMemory(strPathBuffer, _countof(strPathBuffer));
			_stprintf_s(strPathBuffer, _T("%s%s%s.ini"), pathSplited.driver, pathSplited.dir, pathSplited.fileName);

			m_lpSettings->LoadFromFile(strPathBuffer, 0);
			//<SCENE_SETTING>
		}

		{
			TCHAR strPathBuffer[MAX_PATH * 2] = _T("");
			hrRetCode = KG3DCollocator::GetSavePath(EM_SAVE_OP_SCENE_SETTINGS,pathSplited, NULL, strPathBuffer);
			_ASSERTE(SUCCEEDED(hrRetCode));
			hrRetCode = LoadMapSetting(strPathBuffer);
		}	

		{
			TCHAR strPathBuffer[MAX_PATH * 2] = _T("");
			hrRetCode = KG3DCollocator::GetSavePath(EM_SAVE_OP_CAMERA_ANIMATION, pathSplited, NULL, strPathBuffer);
			_ASSERTE(SUCCEEDED(hrRetCode));
			hrRetCode = m_pCameraMovement->LoadFromFile(strPathBuffer);
		}
		//KG_COM_PROCESS_ERROR(hr);

		//////////////////////////////////////////////////////////////////////////
		//根据引擎选项进行一些处理
		

		if(NULL == m_lpRegionDWInfoManager)
		{
			/*TCHAR strRegionInfoDir[MAX_PATH];

			_ASSERTE(pathSplited.dir[_tcslen(pathSplited.dir) - 1] == '\\');

			INT nRet = _stprintf_s(strRegionInfoDir, _T("%s%s%sRegionInfo"), pathSplited.driver, pathSplited.dir, pathSplited.fileName);
			if (nRet >= 0)
			{
				_tmkdir(strRegionInfoDir);
				_tcscat_s(strRegionInfoDir, _T("\\"));

				_tcscat_s(strRegionInfoDir, KG3DRegionDWInfoManager::GetDefaultMapName());

				KG3DRegionInfoManager* pRM = NULL;
				///可以从文件名得到Type，并自动决定是不是要载入
				hrRetCode = g_cRegionInfoManagerTable.LoadResourceFromFile(strRegionInfoDir, 0, 0, (IKG3DResourceBase**)&pRM);
				if (SUCCEEDED(hrRetCode))
				{
					_ASSERTE(pRM);
					m_lpRegionDWInfoManager = pRM;
				}
				else
				{
					_ASSERTE(! pRM);
					hrRetCode = g_cRegionInfoManagerTable.Get1NewResource(&pRM, TYPE_RIM_DW, 0);///注意这个创建的是DW的InfoManager
					if (SUCCEEDED(hrRetCode))
					{
						_ASSERTE(pRM);
						m_lpRegionDWInfoManager = pRM;

					}
				}
				_KG3D_DEBUG_FAIL_REPORT_MSG(m_lpRegionDWInfoManager, _T("SSE创建")_STRINGER(KG3DRegionDWInfoManager));			
			}*/

			TCHAR strPathBuffer[MAX_PATH * 2] = _T("");
			hrRetCode = KG3DCollocator::GetSavePath(EM_SAVE_OP_REGION_INFO, pathSplited, NULL, strPathBuffer);
			_ASSERTE(SUCCEEDED(hrRetCode));

			KG3DRegionInfoManager* pRM = NULL;
			///可以从文件名得到Type，并自动决定是不是要载入
			hrRetCode = g_cRegionInfoManagerTable.LoadResourceFromFile(strPathBuffer, 0, 0, (IKG3DResourceBase**)&pRM);
			if (SUCCEEDED(hrRetCode))
			{
				_ASSERTE(NULL != pRM);
				m_lpRegionDWInfoManager = pRM;
			}
			else
			{
				_ASSERTE(NULL == pRM);
				hrRetCode = g_cRegionInfoManagerTable.Get1NewResource(&pRM, TYPE_RIM_DW, 0);///注意这个创建的是DW的InfoManager
				if (SUCCEEDED(hrRetCode))
				{
					_ASSERTE(NULL != pRM);
					m_lpRegionDWInfoManager = pRM;

				}
			}
			_KG3D_DEBUG_FAIL_REPORT_MSG(m_lpRegionDWInfoManager, _T("SSE创建")_STRINGER(KG3DRegionDWInfoManager));	

		}

		//加载环境
		{
			TCHAR strEnvName[MAX_PATH];
			hrRetCode = KG3DCollocator::GetSavePath(EM_SAVE_OP_ENVIRONMENT, pathSplited, NULL, strEnvName);
			_ASSERTE(SUCCEEDED(hrRetCode));
			hrRetCode = LoadKG3DEnvironment(strEnvName);
		}

        {
            TCHAR szEffectParamName[MAX_PATH];
            hrRetCode = KG3DCollocator::GetSavePath(EM_SAVE_OP_EFFECTPARAMS, pathSplited, NULL, szEffectParamName);
            _ASSERTE(SUCCEEDED(hrRetCode));
            hrRetCode = PostRender::KG3DPostRenderEffectManager::LoadParams(szEffectParamName);

            memcpy(&m_EffectParams, &PostRender::KG3DPostRenderEffectManager::m_sEffectParams, sizeof(PostRenderParams));
        }

		////加载灯光文件

		//{
		//	TCHAR strPointLightDir[MAX_PATH];
		//	INT nRet = _stprintf_s(strPointLightDir, _T("%s%s\\%s.pointlight"), pathSplited.driver, pathSplited.dir, pathSplited.fileName);
		//	bool bLoadLightTableOrCreateLightTable = nRet >= 0 && g_IsFileExist(strPointLightDir);

		//	GetCommonObjectNode().Request(KG3DCOBJ_HANDLE(KG3DTYPE_ENVEFF_POINTLIGHT_TABLE, 0)
		//		, bLoadLightTableOrCreateLightTable ? strPointLightDir : NULL, 0, 0, NULL);
		//}

		TCHAR strPreloadNpc[MAX_PATH];
		strcpy_s(strPreloadNpc, MAX_PATH, cszFileName);
		_strlwr_s(strPreloadNpc);
		TCHAR *strModify = strstr(strPreloadNpc, ".map");
		KG_PROCESS_ERROR(strModify);
		strcpy_s(strModify, MAX_PATH - strlen(strPreloadNpc) - 4, "preload.dat");
		//LoadPreloadNpc(strPreloadNpc);
	}

	pCurSceneSave = NULL;
Exit1:
	hrResult = S_OK;
Exit0:
	if(FAILED(hrResult))
		m_scName = _T("");

	if(pCurSceneSave)
		g_cGraphicsTool.SetCurScene(pCurSceneSave);
	
	//m_EntityMgr.Init();//在Scene的Init中已经Init过一次了，不要再Init了，不然所有资源又创建两次
	
	if (bIsPassRecord)
		g_cRecorder.UnLock();

	if(g_cEngineOption.bEnableTimeOptimizeLog)
	{
		DWORD dwCost = timeGetTime() - dwTime;
		KGLogPrintf(KGLOG_INFO,"TimeOptimize KG3DSceneSceneEditor::LoadFromFile cost %d",dwCost);
	}

	return hrResult;

}

HRESULT KG3DSceneSceneEditor::SaveToFile(const char cszFileName[], unsigned uOption)
{
	if(0 == uOption)
	{
		_ASSERTE(NULL);
		KGLogPrintf(KGLOG_ERR, _T("SaveToFile, uOption为0，参数不正确，需要选择至少一项保存项目。改成全部保存"));
		uOption = EM_SAVE_OP_ALL;
	}

	BOOL bRet = FALSE;
	HRESULT hrRetCode = E_FAIL;
	

	KGLOG_PROCESS_ERROR(_tcslen(cszFileName) < MAX_PATH && _T("保存路径太长，下面的路径函数处理会失败"));
	
	{
		//分离路径，分离之后下面再合成会更方便。这里用StrPathSplited来加强中间的错误控制
		KG_CUSTOM_HELPERS::StrPathSplited pathSplited;
		{
			TCHAR strPathBuffer[MAX_PATH] = _T("");
			bRet = (0 == _tcscpy_s(strPathBuffer, cszFileName));
			KGLOG_PROCESS_ERROR(bRet && _T("Scene保存，路径名处理失败，保存失败"));

			bRet = KG_CUSTOM_HELPERS::StrReplace(strPathBuffer, _T('/'), _T('\\'));
			_ASSERTE(bRet);

			bRet = KG_CUSTOM_HELPERS::StrPathSplit(cszFileName, pathSplited);
			KGLOG_PROCESS_ERROR(bRet && _T("Scene保存，分解文件路径失败，保存失败"));
		}

		if(uOption & EM_SAVE_OP_TERRAIN_AND_ENTITY)
		{
			m_lpSceneOutDoorSpaceMgr->SaveToFile(cszFileName,uOption);
		}

		if(uOption & EM_SAVE_OP_SCENE_SETTINGS)
		{
			TCHAR strPathBuffer[MAX_PATH * 2]= _T("");
			hrRetCode = KG3DCollocator::GetSavePath(EM_SAVE_OP_SCENE_SETTINGS, pathSplited, NULL, strPathBuffer);
			_ASSERTE(SUCCEEDED(hrRetCode));
			SaveMapSetting(strPathBuffer);
		}


		/*TCHAR strCameraAniFileName[256];
		wsprintf(strCameraAniFileName, "%s%s%s.CameraAni", pathSplited.driver, pathSplited.dir, pathSplited.fileName);*/

		if (uOption & EM_SAVE_OP_CAMERA_ANIMATION)
		{
			TCHAR strPathBuffer[MAX_PATH * 2] = _T("");
			hrRetCode = KG3DCollocator::GetSavePath(EM_SAVE_OP_CAMERA_ANIMATION, pathSplited, NULL, strPathBuffer);
			_ASSERTE(SUCCEEDED(hrRetCode));
			if(NULL != m_pCameraMovement)
			{
				m_pCameraMovement->SaveToFile(strPathBuffer);
			}
		}


		/*if (m_lpRegionDWInfoManager)
		{
			TCHAR strPathBuffer[MAX_PATH * 2] = _T("");
			INT nRet = _stprintf_s(strPathBuffer, _T("%s%s\\%sRegionInfo"), pathSplited.driver, pathSplited.dir, pathSplited.fileName);
			if (nRet >= 0)
			{
				_tmkdir(strPathBuffer);
				_tcscat_s(strPathBuffer, _T("\\"));
				_tcscat_s(strPathBuffer, KG3DRegionDWInfoManager::GetDefaultMapName());

				m_lpRegionDWInfoManager->SaveToFile(strPathBuffer, 0);
			}
		}*/
		if (uOption & EM_SAVE_OP_REGION_INFO)
		{
			TCHAR strPathBuffer[MAX_PATH * 2] = _T("");
			KGCH::StrPathSplited pathSplitedRegionInfo;
			hrRetCode = KG3DCollocator::GetSavePath(EM_SAVE_OP_REGION_INFO, pathSplited, &pathSplitedRegionInfo, NULL, 0);
			_ASSERTE(SUCCEEDED(hrRetCode));
			BOOL bRet = pathSplitedRegionInfo.GetCatedDirectory(strPathBuffer, _countof(strPathBuffer));
			_ASSERTE(bRet);
			g_CreatePath(strPathBuffer);	//目录不存在的话要创建
			bRet= pathSplitedRegionInfo.GetCatedPath(strPathBuffer, _countof(strPathBuffer));
			if(NULL != m_lpRegionDWInfoManager)
			{
				m_lpRegionDWInfoManager->SaveToFile(strPathBuffer, 0);
			}
		}

		if(uOption & EM_SAVE_OP_ENVIRONMENT)
		{
			TCHAR strPathBuffer[MAX_PATH * 2] = _T("");
			hrRetCode = KG3DCollocator::GetSavePath(EM_SAVE_OP_ENVIRONMENT, pathSplited, NULL, strPathBuffer);
			_ASSERTE(SUCCEEDED(hrRetCode));
			SaveKG3DEnvironment(strPathBuffer);
		}

        if (uOption & EM_SAVE_OP_EFFECTPARAMS)
        {
            TCHAR strPathBuffer[MAX_PATH * 2] = _T("");
            hrRetCode = KG3DCollocator::GetSavePath(EM_SAVE_OP_EFFECTPARAMS, pathSplited, NULL, strPathBuffer);
            _ASSERTE(SUCCEEDED(hrRetCode));

            memcpy(&PostRender::KG3DPostRenderEffectManager::m_sEffectParams, &m_EffectParams, sizeof(PostRenderParams));
            PostRender::KG3DPostRenderEffectManager::SaveParams(strPathBuffer);
        }

		//<SCENE_SETTING>
		{
			TCHAR strSettings[MAX_PATH];
			_stprintf_s(strSettings, _T("%s%s%s.ini"), pathSplited.driver, pathSplited.dir, pathSplited.fileName);

			m_lpSettings->SaveToFile(strSettings, 0);
		}
		//<SCENE_SETTING>
	}

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneSceneEditor::SaveMapSetting(LPSTR pFileName)
{
	IIniFile *pIniFile = g_OpenIniFile(pFileName,true,true);
	if (!pIniFile)
	{
		IFile* pFile = g_CreateFile(pFileName);
		KG_COM_RELEASE(pFile);
		pIniFile = g_OpenIniFile(pFileName,true,true);
	}
	_ASSERTE( pIniFile );


	//////////////////////////////////////////////////////////////////////////
	// camera localize
	KG3DCamera *pCamera = &m_SceneWndManager->GetCurOutputWnd()->GetCamera();
	D3DXVECTOR3  vPos;

	pCamera->GetPosition( &vPos );
	pIniFile->WriteStruct( "CameraSet", "Pos", &vPos, sizeof( vPos ) );

	pCamera->GetLookAtPosition( &vPos );
	pIniFile->WriteStruct( "CameraSet", "LookAtPos", &vPos, sizeof( vPos ) );

	pCamera->GetUpDirection( &vPos );
	pIniFile->WriteStruct( "CameraSet", "UpDirection", &vPos, sizeof( vPos ) );



	//////////////////////////////////////////////////////////////////////////
	pIniFile->WriteString("Scene", "evnmap", m_strEnvMapName.c_str());

	pIniFile->WriteInteger("LogicalScene","X_Start",m_nLogicalSceneX_Start);
	pIniFile->WriteInteger("LogicalScene","Z_Start",m_nLogicalSceneZ_Start);
	pIniFile->WriteInteger("LogicalScene","X_Width",m_nLogicalSceneX_Width);
	pIniFile->WriteInteger("LogicalScene","Z_Width",m_nLogicalSceneZ_Width);

    if (!g_bClient)
        pIniFile->WriteInteger("Main","nActiveCurveStype", g_cEngineOption.nActiveCurveStype);

    if (m_lpWaterEffect)
        m_lpWaterEffect->SaveToFile(pIniFile);

	pIniFile->Save(pFileName);
	pIniFile->Release();

	return S_OK;
	//Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneSceneEditor::LoadMapSetting(LPSTR pFileName)
{
	IIniFile *pIniFile = g_OpenIniFile(pFileName);
	if (!pIniFile)
	{
		//MessageBox(g_hBaseWnd, "Not Scene Setting File!", "Error", MB_OK);
		KGLogPrintf(KGLOG_ERR, "KG3DSceneSceneEditor::LoadMapSetting Error: Not Scene Setting File!\n");
		return E_FAIL;
	}

	//////////////////////////////////////////////////////////////////////////
	// camera localize
	m_vCameraPos = D3DXVECTOR3(0,500,-1000);
	m_vCameraLookAtPos = D3DXVECTOR3(0,0,0);
	m_vCameraUpDirection = D3DXVECTOR3(0,1,0);

	pIniFile->GetStruct( "CameraSet", "Pos", &m_vCameraPos, sizeof( m_vCameraPos ) );
	pIniFile->GetStruct( "CameraSet", "LookAtPos", &m_vCameraLookAtPos, sizeof( m_vCameraLookAtPos ) );
	pIniFile->GetStruct( "CameraSet", "UpDirection", &m_vCameraUpDirection, sizeof( m_vCameraUpDirection ) );


	//////////////////////////////////////////////////////////////////////////
	TCHAR szEvnMap[MAX_PATH];
	pIniFile->GetString("Scene", "evnmap", KG3DTextureTable::GetDefaultEnvMapName(), szEvnMap, sizeof(szEvnMap));

	SetEnvMap(szEvnMap);

	//////////////////////////////////////////////////////////////////////////
	//logical
	int nX_Start = 0,nZ_Start = 0,nX_Width = 64,nZ_Width = 64;

    m_lpSceneOutDoorSpaceMgr->GetSceneRect(nX_Start,nZ_Start,nX_Width,nZ_Width);

	pIniFile->GetInteger("LogicalScene","X_Start",nX_Start,&m_nLogicalSceneX_Start);
	pIniFile->GetInteger("LogicalScene","Z_Start",nZ_Start,&m_nLogicalSceneZ_Start);
	pIniFile->GetInteger("LogicalScene","X_Width",nX_Width,&m_nLogicalSceneX_Width);
	pIniFile->GetInteger("LogicalScene","Z_Width",nZ_Width,&m_nLogicalSceneZ_Width);

    pIniFile->GetInteger("Main","nActiveCurveStype", 0, &g_cEngineOption.nActiveCurveStype);

    if (m_lpWaterEffect)
        m_lpWaterEffect->LoadFromFile(pIniFile);

	pIniFile->Release();

	return S_OK;
}

HRESULT KG3DSceneSceneEditor::LoadRepresentNpcList()
{
	IIniFile* pFile = NULL;
	HRESULT hr = E_FAIL;
	int nNumNpc = 0;

	{
		list<KG3DRepresentNpcScreenRunner*>::iterator i = m_listRepresentNPC.begin();
		list<KG3DRepresentNpcScreenRunner*>::iterator iend = m_listRepresentNPC.end();
		while (i!=iend)
		{
			KG3DRepresentNpcScreenRunner* pNpc = *i;
			SAFE_RELEASE(pNpc);
			i++;
		}
		m_listRepresentNPC.clear();
	}

	pFile = g_OpenIniFile("Data\\public\\RepresentNpcList.ini");
	KGLOG_PROCESS_ERROR(pFile);

	pFile->GetInteger("Main","nNumNpc",0,&nNumNpc);

	for (int i=0;i<nNumNpc;i++)
	{
		TCHAR szSectionName[MAX_PATH];
		wsprintf(szSectionName,"%d",i);

		KG3DRepresentNpcScreenRunner* pNpc = NULL;

		hr = g_GetObjectTable().Get1NewResource((KG3DRepresentObject**)&pNpc, REPRESENTOBJECT_NPC, 0);
		KGLOG_COM_PROCESS_ERROR(hr);

		m_listRepresentNPC.push_back(pNpc);

		pNpc->LoadFromIniSection(pFile,szSectionName,0);
	}
Exit0:
	SAFE_RELEASE(pFile);
	return hr;
}

HRESULT KG3DSceneSceneEditor::Init()
{
	HRESULT hr = E_FAIL;

	g_OutputDebugInfo("KG3DSceneSceneEditor Init");
	//ASSERT(_CrtCheckMemory());

	g_cGraphicsTool.SetCurScene(this);
	m_fZFar = g_cEngineOption.fCameraDistance;

	if(m_uOption & SCENE_FIX_CAMRERANGLE)
	{
		m_fFovy = D3DX_PI * 40 / 180;
		m_fZFar = 400000;
	}
	else
		m_fFovy = g_cEngineOption.fCameraAngle;

	m_Sample_Mag = (D3DTEXTUREFILTERTYPE)(g_cEngineOption.nSampMagFilter);
	m_Sample_Min = (D3DTEXTUREFILTERTYPE)(g_cEngineOption.nSampMinFilter);
	m_Sample_Mip = (D3DTEXTUREFILTERTYPE)(g_cEngineOption.nSampMipFilter);

	m_fMipmapLod = -0.5F;

	KG3DSceneEditorBase::Init();
	//ASSERT(_CrtCheckMemory());

	m_EntityMgr.m_bUseMultiThread = TRUE;
	/*if(g_eEngineOption.bUseMultiThreadCull)
	{
	InputEntityMgrToMultiThreadCull(&m_EntityMgr);
	}*/

    if (!g_bClient)
    {
	    m_EntityMgrTopView.Init(FALSE);
	    m_EntityMgrTopView.m_bGetEntityFromContainerDirect = TRUE;
	    m_EntityMgrTopView.SetCullLevelDepth(2);
	    m_EntityMgrTopView.EnableCheckVisibleCount(FALSE);
    }

	m_lpWaterEffect = new KG3DWaterEffect();
	m_lpWaterEffect->Init();
	//m_lpWaterEffect->LoadFromFile("data\\public\\defaultscene\\waterconfig.wfx");

	this->SetAmbient(0xFF9FBADF);
	g_pd3dDevice->SetRenderState(D3DRS_AMBIENT, 0xFF9FBADF);//


	SAFE_RELEASE(m_lpRegionDWInfoManager);

	if (!g_bClient)
	{
		SAFE_RELEASE(m_pRepresentNPC);

		//_ASSERT(_CrtCheckMemory());
		if( SUCCEEDED(g_GetObjectTable().Get1NewResource((KG3DRepresentObject**)&m_pRepresentNPC, REPRESENTOBJECT_NPC, 0)))
		{
			m_pRepresentNPC->LoadFromIniFileEx("config.ini","SCENEEDITOR",0);
		}
		//////////////////////////////////////////////////////////////////////////
		LoadRepresentNpcList();

		m_vSnapTool.Init();

		hr = m_PVSParameter.Init();

		IKG3DSoundShell *pSoundShell = g_cEngineManager.GetSound();
		if(pSoundShell)
			hr = pSoundShell->Unlock();
	}
	if(g_bDesign)
	{
		SetSelectOption(EM_SELMASK_LOGICOBJ);//逻辑编辑器中``只需要选取逻辑对像
	}
	//////////////////////////////////////////////////////////////////////////

	for (size_t i = 0; i < m_PreloadModels.size(); i++)
	{
		SAFE_RELEASE(m_PreloadModels[i]);
	}
	m_PreloadModels.clear();

	m_lpSceneOutDoorSpaceMgr->SetSceneEditor(this);

	return S_OK;
}


HRESULT KG3DSceneSceneEditor::UnInit()
{
	for (size_t i = 0; i < m_PreloadModels.size(); i++)
	{
		SAFE_RELEASE(m_PreloadModels[i]);
	}
	m_PreloadModels.clear();

	m_PVSParameter.UnInit();
	for (size_t i = 0; i < m_vecSFXSuede.size(); i++)
		SAFE_RELEASE(m_vecSFXSuede[i]);
	m_vecSFXSuede.clear();


	DeleteAllMeasureTool();


	SAFE_DELETE(m_pSFXSuede);
	SAFE_RELEASE(m_pRepresentNPC);
	{
		list<KG3DRepresentNpcScreenRunner*>::iterator i = m_listRepresentNPC.begin();
		list<KG3DRepresentNpcScreenRunner*>::iterator iend = m_listRepresentNPC.end();
		while (i!=iend)
		{
			KG3DRepresentNpcScreenRunner* pNpc = *i;
			SAFE_RELEASE(pNpc);
			i++;
		}
		m_listRepresentNPC.clear();
	}
	SAFE_RELEASE(m_pTestModel);

    if (!m_listEntityRenderLast.empty())
    {
        KGLogPrintf(KGLOG_ERR, "[Scene] some RenderLast entity not released when UnInit()");
        m_listEntityRenderLast.clear();
    }

	//m_listEntityNotInQTree.clear();;

    if (!g_bClient)
    {
	    m_EntityMgrTopView.UnInit();
    }

	map<int,KG3DTexture*>::iterator S = m_mapLightMapTextureEntity.begin();
	while (S!=m_mapLightMapTextureEntity.end())
	{
		KG3DTexture *pTexture = S->second;
		SAFE_RELEASE(pTexture);
		S++;
	}

	m_mapLightMapTextureEntity.clear();

	m_vSnapTool.UnInit();

	if(m_lpLogicEditor)
		m_lpLogicEditor->UnInit();

	SAFE_RELEASE(m_lpRegionDWInfoManager);

	SAFE_DELETE(m_lpWaterEffect);

	SAFE_RELEASE(m_pTerrainBlock);

	SAFE_RELEASE(m_lpEvnMap);

	return KG3DSceneEditorBase::UnInit();

}

void KG3DSceneSceneEditor::MsgOnLButtonUp(WPARAM wParam, LPARAM lParam)//<PROCESS_MESSAGE>
{
	if(m_bEditing)
	{
		KG3DSceneEditorBase::MsgOnLButtonUp(wParam,lParam);
		if(this->GetEditState()==SCENESTATE_TERRAIN)
		{
			if(m_lpSceneOutDoorSpaceMgr)
				g_cTerrainBlockModifier.OnBrushEnd(m_lpSceneOutDoorSpaceMgr);
		}
	}

	/*
	if ((!m_bEditing) && (m_pRepresentNPC) && (m_SceneWndManager->GetCurOutputWnd()) && (!g_bClient))
	{
	m_SceneWndManager->GetCurOutputWnd()->GetCamera().ProcessLButtonUp(wParam, lParam);
	}*/

}


void KG3DSceneSceneEditor::MsgOnKeyDown(WPARAM wParam, LPARAM lParam)//<PROCESS_MESSAGE>
{

	if (wParam == VK_HOME)
	{
		if (m_pRepresentNPC && !m_bEditing)	
		{
			m_pRepresentNPC->SetCurPosition(D3DXVECTOR3(5000.0f ,0.0f, 5000.0f), *this, false);
			m_SceneWndManager->GetCurOutputWnd()->GetCamera().SetPosition(D3DXVECTOR3(5000.0f ,500.0f, 4000.0f));
		}
	}
	if(wParam=='H')
	{
		if(m_SceneWndManager->GetCurOutputWnd())
		{
			m_SceneWndManager->GetCurOutputWnd()->EnableSaveToFile(!m_SceneWndManager->GetCurOutputWnd()->IsSaveToFile());
			if(m_SceneWndManager->GetCurOutputWnd()->IsSaveToFile())
				m_SceneWndManager->GetCurOutputWnd()->SetCurrentFrame(0);
		}
	}
	if(wParam==VK_ADD)
	{
		m_fSceneRunnerSpeed += 0.5F;
		m_fSceneRunnerSpeed = min(100.0F,m_fSceneRunnerSpeed);
	}
	if(wParam==VK_SUBTRACT)
	{
		m_fSceneRunnerSpeed -= 0.2F;
		m_fSceneRunnerSpeed = max(0.1F,m_fSceneRunnerSpeed);
	}

	if(m_bEditing)
	{
		KG3DSceneEditorBase::MsgOnKeyDown(wParam,lParam);
        if (this->GetEditState()==SCENESTATE_SELECT)
		{
			/*KG3DSceneEntityList::iterator i = m_listSelectedEntity.begin();
			while (i!=m_listSelectedEntity.end())*/
			TypeEntityAccessProxy& proxy = this->GetSelectionTool().GetSelectionAccessProxy();
			for(proxy.Begin(); ! proxy.IsEnd(); proxy.StepForward())
			{
				//KG3DSceneEntity Entity = *i;
				KG3DSceneEntity& Entity = proxy.GetCur();
				Entity.ProcessMessage(WM_KEYDOWN,wParam,lParam);
				//i++;
			}

			if(wParam == static_cast<WPARAM>(g_bDesign ? _T('X') : _T('D')))
			{
				DrapSelectedEntity();
			}
			else if(wParam==VK_SPACE)
			{
				GetSelectedObjectInHand();
			}
		}

		if(wParam=='N')
		{
			g_cEngineManager.GetRenderOption( RENDER_ENTITY ) ? g_cEngineManager.SetRenderOption( RENDER_ENTITY ,FALSE) : g_cEngineManager.SetRenderOption( RENDER_ENTITY ,TRUE);
		}
		else if(wParam=='B')
		{
			g_cEngineManager.GetRenderOption( RENDER_TERRAIN ) ? g_cEngineManager.SetRenderOption( RENDER_TERRAIN ,FALSE) : g_cEngineManager.SetRenderOption( RENDER_TERRAIN ,TRUE);
		}
		else if(wParam=='G')
		{
			g_cEngineManager.GetRenderOption( RENDER_GRASS ) ? g_cEngineManager.SetRenderOption( RENDER_GRASS ,FALSE) : g_cEngineManager.SetRenderOption( RENDER_GRASS ,TRUE);
			g_cEngineManager.GetRenderOption( RENDER_STONE ) ? g_cEngineManager.SetRenderOption( RENDER_STONE ,FALSE) : g_cEngineManager.SetRenderOption( RENDER_STONE ,TRUE);
		}
		else if (wParam=='F')
		{
			g_cEngineManager.GetRenderOption( RENDER_FRUTEX ) ? g_cEngineManager.SetRenderOption( RENDER_FRUTEX ,FALSE) : g_cEngineManager.SetRenderOption( RENDER_FRUTEX ,TRUE);
		}
		else if (wParam=='C')//复制物件,还没有找到为什么CTRL+C不行
		{
			if (g_bDesign)
				CopySelection(); // add by suntao

			// 2009-1-12 <Copy Refactor>复制改成3dsmax的方式，不需要用Ctrl+C记录选择，而是直接用Ctrl+V就复制出当前的选择。记录选择这个操作需要考虑太多物件相关性的问题，而且也应该尽量向Max的操作看齐
			// MessageBox(NULL, _T("复制不需要Ctrl+C。直接按Ctrl+V就可以复制选择的物件，请参考3dsmax的操作"), NULL, MB_OK);
			// CopySelectedEntity();
		}
		else if (wParam=='V')
		{
			if(MessageBox(g_hBaseWnd,"确认复制物件？",NULL,
				MB_YESNO | MB_ICONQUESTION) != IDNO)
			{
				this->PasteSelection();
			}
		}
		else if(wParam=='K')
		{
			m_EntityMgr.PrintfProfile();
		}
	}
	else
	{
		/*if(wParam=='K') //测试代码
		{
		if(m_pRepresentNPC)
		{
		D3DXVECTOR3 Pos;
		m_pRepresentNPC->GetTranslation(&Pos);

		AddVisibleSet(Pos);
		}
		}
		*/

		KG3DSceneEditorBase::MsgOnKeyDown(wParam,lParam);
	}
}
//<PROCESS_MESSAGE>
void KG3DSceneSceneEditor::MsgOnLButtonDown(WPARAM wParam, LPARAM lParam)
{

	// KGLogPrintf(KGLOG_ERR,"鼠标左健按下");

	if(m_bEditing)
	{
		KG3DSceneEditorBase::MsgOnLButtonDown(wParam,lParam);
		if(this->GetEditState()==SCENESTATE_TERRAIN)
		{
			if(m_lpSceneOutDoorSpaceMgr)
				g_cTerrainBlockModifier.OnBrushStart(m_lpSceneOutDoorSpaceMgr);
		}
	}

	/*
	if ((!m_bEditing) && (m_pRepresentNPC) && (m_SceneWndManager->GetCurOutputWnd()) && (!g_bClient)) 
	{
	m_SceneWndManager->GetCurOutputWnd()->GetCamera().ProcessLButtonDn(wParam, lParam);
	}*/


	//Exit0:
	return;
}

void KG3DSceneSceneEditor::MsgOnRButtonDown(WPARAM wParam, LPARAM lParam)
{
	if ((!m_bEditing) && (m_pRepresentNPC) && (m_SceneWndManager->GetCurOutputWnd()) && (!g_bClient)) 
	{
		m_pRepresentNPC->ProcessRButtonDn(wParam, lParam);
	}
}

void KG3DSceneSceneEditor::MsgOnRButtonUp(WPARAM wParam, LPARAM lParam)
{
	if ((!m_bEditing) && (m_pRepresentNPC) && (m_SceneWndManager->GetCurOutputWnd()) && (!g_bClient))
	{
		m_pRepresentNPC->ProcessRButtonUp(wParam, lParam);
	}
}


void KG3DSceneSceneEditor::MsgOnMouseWheel(WPARAM wParam, LPARAM lParam)
{
	if(m_bEditing)
	{
		KG3DSceneEditorBase::MsgOnMouseWheel(wParam,lParam);
	}
	else
	{
		int k = 0;
		k |=wParam;
		float S = 1.0F;
		if (k>0)
			S = 0.9F;
		else 
			S = 1.1F;
		vCamera.x *= S;

		KG3DTrackCamera* pTrackCamera = m_SceneWndManager->GetCurOutputWnd()->GetCamera().GetTrackCameraInterface();
		if(NULL != pTrackCamera)
			pTrackCamera->ProcessMouseWeel(wParam, lParam);

	}
}
//<PROCESS_MESSAGE>
void KG3DSceneSceneEditor::MsgOnMouseMove(WPARAM wParam, LPARAM lParam)
{
	if(!m_bEditing)
	{
		int fwKeys = (int)wParam; 
		int xPos = LOWORD(lParam); 
		int yPos = HIWORD(lParam);

		if(m_SceneWndManager->GetCurOutputWnd())
		{
			//			 float Step = 1000.0F;
			D3DXVECTOR3 PosMove(0,0,0);
			D3DXVECTOR3 Pos,LookAt;
			m_SceneWndManager->GetCurOutputWnd()->GetCamera().GetPosition(&Pos);
			m_SceneWndManager->GetCurOutputWnd()->GetCamera().GetLookAtPosition(&LookAt);

			D3DXVECTOR3 Left = m_SceneWndManager->GetCurOutputWnd()->GetCamera().GetCameraRight();
			D3DXVECTOR3 Front = m_SceneWndManager->GetCurOutputWnd()->GetCamera().GetCameraFront();
			D3DXVECTOR3 Up = m_SceneWndManager->GetCurOutputWnd()->GetCamera().GetCameraUp();

			if(fwKeys&MK_MBUTTON)
			{
				D3DXVECTOR3 N = Pos - LookAt;
				//				 float R = D3DXVec3Length(&N);
				float XX = (xPos - g_nMouseMoveX) * 0.003F;
				float YY = (yPos - g_nMouseMoveY) * 0.003F;

				vCamera.y += XX;
				vCamera.z += YY;
			}			
		}
		g_nMouseMoveX = xPos;
		g_nMouseMoveY = yPos;
	}
}

HRESULT KG3DSceneSceneEditor::OnBrushCellLogical()
{
	if(m_lpLogicEditor)
	{
		//_ASSERTE(m_lpLogicEditor);
		m_lpLogicEditor->PaintSelectedLogicalCell(g_KeyState[VK_CONTROL] != 1, g_KeyState[VK_SHIFT] == 1 && g_KeyState[VK_CONTROL] == 1);

		if (g_bDesign)
		{
			HRESULT hrRetCode = NULL;
			IEKG3DSceneLogicalEditorBase* logicEditor = NULL;
			hrRetCode = GetIELogicScene(&logicEditor);
			KGLOG_COM_PROCESS_ERROR(hrRetCode);
			DWORD state = 0;
			logicEditor->GetCellModifyState(&state);
			if (state != 0)
			{
				D3DXVECTOR3 Pos, Dir, Inter;
				m_SceneWndManager->GetCurOutputWnd()->GetPickRay(&Pos,&Dir, NULL);
				FLOAT fDis = FLT_MAX;
				BOOL bFoundInter = ::RayIntersectionTerrain(*this, Pos, Dir, &fDis);
				if (bFoundInter)
				{
					Inter = Pos + Dir * fDis;
					m_vecCurrentEditBrushPos = Inter;
				}
				
				// 在地表外的就不要乱更新坐标了。
				if (m_vecCurrentEditBrushPos.x >= 0 && m_vecCurrentEditBrushPos.x <= m_nLogicalSceneX_Width * 100 &&
					m_vecCurrentEditBrushPos.z >= 0 && m_vecCurrentEditBrushPos.z <= m_nLogicalSceneZ_Width * 100)
				{
					FireEvent2(EVENTEDITORLOGICALCELLUPDATEPOS, 0, m_nCurrentEditBrushIndex, &m_vecCurrentEditBrushPos);
				}
			}
		}
	}
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneSceneEditor::SetCurrentEditBrushIndex(INT nIndex)
{
	m_nCurrentEditBrushIndex = nIndex;
	return S_OK;
}

HRESULT KG3DSceneSceneEditor::GetObjectEffectLight( BBox &ObjectAABB,std::vector<BYTE> &EffectLightID)
{
	return S_OK;
}


HRESULT KG3DSceneSceneEditor::SaveObjectToFile(FILE *pFile,KG3DRepresentObject *pObject)
{
	HRESULT hr = E_FAIL;
	size_t SizeWrite = 0;
	DWORD ModelCnt = pObject->GetModelCount(); 
	DWORD Type = pObject->GetType();
	KG3DModel *pModel = NULL;
	unsigned ModelType = 0;

	if (Type == REPRESENTOBJECT_DEFAULT)
		Type = EXT_REPRESENTOBJECT_DEFAULT2;
	if (Type == REPRESENTOBJECT_SET)
		Type = EXT_REPRESENTOBJECT_SET2;

	SizeWrite = fwrite(&Type, 1, sizeof(DWORD), pFile);
	KGLOG_PROCESS_ERROR_CONSOLE(SizeWrite == sizeof(DWORD));

	SizeWrite = fwrite(&ModelCnt, 1, sizeof(DWORD), pFile);
	KGLOG_PROCESS_ERROR_CONSOLE(SizeWrite == sizeof(DWORD));

	SizeWrite = fwrite(&pObject->m_Scaling, 1, sizeof(D3DXVECTOR3), pFile);
	KGLOG_PROCESS_ERROR_CONSOLE(SizeWrite == sizeof(D3DXVECTOR3));

	SizeWrite = fwrite(&pObject->m_ScalingCenter, 1, sizeof(D3DXVECTOR3), pFile);
	KGLOG_PROCESS_ERROR_CONSOLE(SizeWrite == sizeof(D3DXVECTOR3));

	SizeWrite = fwrite(&pObject->m_ScalingRotation, 1, sizeof(D3DXQUATERNION), pFile);
	KGLOG_PROCESS_ERROR_CONSOLE(SizeWrite == sizeof(D3DXQUATERNION));

	SizeWrite = fwrite(&pObject->m_RotationCenter, 1, sizeof(D3DXVECTOR3), pFile);
	KGLOG_PROCESS_ERROR_CONSOLE(SizeWrite == sizeof(D3DXVECTOR3));

	SizeWrite = fwrite(&pObject->m_Rotation, 1, sizeof(D3DXQUATERNION), pFile);
	KGLOG_PROCESS_ERROR_CONSOLE(SizeWrite == sizeof(D3DXQUATERNION));

	SizeWrite = fwrite(&pObject->m_Translation, 1, sizeof(D3DXVECTOR3), pFile);
	KGLOG_PROCESS_ERROR_CONSOLE(SizeWrite == sizeof(D3DXVECTOR3));

	switch(Type)
	{
	case EXT_REPRESENTOBJECT_DEFAULT2:
		{
			char MeshName[MAX_PATH];
			char MtlName[MAX_PATH];
			char AniName[MAX_PATH];
			char RetName[MAX_PATH];

			for (DWORD i = 0; i < ModelCnt; i++)
			{
				hr = pObject->GetModelFileName(i, MeshName, MtlName, AniName);
				KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

				hr = g_pFileFinder->GetRelativePath(g_Def_WorkDirectory, MeshName, RetName);
				KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

				SizeWrite = fwrite(RetName, 1, sizeof(char) * MAX_PATH, pFile);
				KGLOG_PROCESS_ERROR_CONSOLE(SizeWrite == sizeof(char) * MAX_PATH);

				hr = g_pFileFinder->GetRelativePath(g_Def_WorkDirectory, MtlName, RetName);
				KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

				SizeWrite = fwrite(RetName, 1, sizeof(char) * MAX_PATH, pFile);
				KGLOG_PROCESS_ERROR_CONSOLE(SizeWrite == sizeof(char) * MAX_PATH);

				hr = g_pFileFinder->GetRelativePath(g_Def_WorkDirectory, AniName, RetName);
				KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

				SizeWrite = fwrite(RetName, 1, sizeof(char) * MAX_PATH, pFile);
				KGLOG_PROCESS_ERROR_CONSOLE(SizeWrite == sizeof(char) * MAX_PATH);

				hr = pObject->GetModel(pModel, i,FALSE);
				KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

				ModelType = pModel->GetType();
				SizeWrite = fwrite(&ModelType, 1, sizeof(unsigned), pFile);
				KGLOG_PROCESS_ERROR_CONSOLE(SizeWrite == sizeof(unsigned));

				INT32 nLightmapTextureIndex = -1;
				SizeWrite = fwrite(&nLightmapTextureIndex, 1, sizeof(INT32), pFile);
				KGLOG_PROCESS_ERROR_CONSOLE(SizeWrite == sizeof(INT32));

				if (ModelType == MESHTYPE_SFX)
				{
					KG3DSFX *psfx = (KG3DSFX*)pModel;
					float fSizeScale = psfx->GetScaleSize();
					SizeWrite = fwrite(&fSizeScale, 1, sizeof(float), pFile);
					KGLOG_PROCESS_ERROR_CONSOLE(SizeWrite == sizeof(float));    
				}

				if (ModelCnt > 1)
				{

					SizeWrite = fwrite(&pModel->m_Scaling, 1, sizeof(D3DXVECTOR3), pFile);
					KGLOG_PROCESS_ERROR_CONSOLE(SizeWrite == sizeof(D3DXVECTOR3));

					SizeWrite = fwrite(&pModel->m_ScalingCenter, 1, sizeof(D3DXVECTOR3), pFile);
					KGLOG_PROCESS_ERROR_CONSOLE(SizeWrite == sizeof(D3DXVECTOR3));

					SizeWrite = fwrite(&pModel->m_ScalingRotation, 1, sizeof(D3DXQUATERNION), pFile);
					KGLOG_PROCESS_ERROR_CONSOLE(SizeWrite == sizeof(D3DXQUATERNION));

					SizeWrite = fwrite(&pModel->m_RotationCenter, 1, sizeof(D3DXVECTOR3), pFile);
					KGLOG_PROCESS_ERROR_CONSOLE(SizeWrite == sizeof(D3DXVECTOR3));

					SizeWrite = fwrite(&pModel->m_Rotation, 1, sizeof(D3DXQUATERNION), pFile);
					KGLOG_PROCESS_ERROR_CONSOLE(SizeWrite == sizeof(D3DXQUATERNION));

					SizeWrite = fwrite(&pModel->m_Translation, 1, sizeof(D3DXVECTOR3), pFile);
					KGLOG_PROCESS_ERROR_CONSOLE(SizeWrite == sizeof(D3DXVECTOR3));
				}
			}
		}
		break;
	case EXT_REPRESENTOBJECT_DEFAULT:
		{
			char MeshName[MAX_PATH];
			char MtlName[MAX_PATH];
			char AniName[MAX_PATH];
			char RetName[MAX_PATH];

			for (DWORD i = 0; i < ModelCnt; i++)
			{
				hr = pObject->GetModelFileName(i, MeshName, MtlName, AniName);
				KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

				hr = g_pFileFinder->GetRelativePath(g_Def_WorkDirectory, MeshName, RetName);
				KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

				SizeWrite = fwrite(RetName, 1, sizeof(char) * MAX_PATH, pFile);
                 KGLOG_PROCESS_ERROR_CONSOLE(SizeWrite == sizeof(char) * MAX_PATH);
                 
                 hr = g_pFileFinder->GetRelativePath(g_Def_WorkDirectory, MtlName, RetName);
                 KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

                 SizeWrite = fwrite(RetName, 1, sizeof(char) * MAX_PATH, pFile);
                 KGLOG_PROCESS_ERROR_CONSOLE(SizeWrite == sizeof(char) * MAX_PATH);

                 hr = g_pFileFinder->GetRelativePath(g_Def_WorkDirectory, AniName, RetName);
                 KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

                 SizeWrite = fwrite(RetName, 1, sizeof(char) * MAX_PATH, pFile);
                 KGLOG_PROCESS_ERROR_CONSOLE(SizeWrite == sizeof(char) * MAX_PATH);

                 hr = pObject->GetModel(pModel, i,FALSE);
                 KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

                 ModelType = pModel->GetType();
                 SizeWrite = fwrite(&ModelType, 1, sizeof(unsigned), pFile);
                 KGLOG_PROCESS_ERROR_CONSOLE(SizeWrite == sizeof(unsigned));

                 if (ModelType == MESHTYPE_SFX)
                 {
                    KG3DSFX *psfx = (KG3DSFX*)pModel;
                    float fSizeScale = psfx->GetScaleSize();
                    SizeWrite = fwrite(&fSizeScale, 1, sizeof(float), pFile);
                    KGLOG_PROCESS_ERROR_CONSOLE(SizeWrite == sizeof(float));    
                 }

                 if (ModelCnt > 1)
                 {

                     SizeWrite = fwrite(&pModel->m_Scaling, 1, sizeof(D3DXVECTOR3), pFile);
                     KGLOG_PROCESS_ERROR_CONSOLE(SizeWrite == sizeof(D3DXVECTOR3));

                     SizeWrite = fwrite(&pModel->m_ScalingCenter, 1, sizeof(D3DXVECTOR3), pFile);
                     KGLOG_PROCESS_ERROR_CONSOLE(SizeWrite == sizeof(D3DXVECTOR3));

                     SizeWrite = fwrite(&pModel->m_ScalingRotation, 1, sizeof(D3DXQUATERNION), pFile);
                     KGLOG_PROCESS_ERROR_CONSOLE(SizeWrite == sizeof(D3DXQUATERNION));

                     SizeWrite = fwrite(&pModel->m_RotationCenter, 1, sizeof(D3DXVECTOR3), pFile);
                     KGLOG_PROCESS_ERROR_CONSOLE(SizeWrite == sizeof(D3DXVECTOR3));

                     SizeWrite = fwrite(&pModel->m_Rotation, 1, sizeof(D3DXQUATERNION), pFile);
                     KGLOG_PROCESS_ERROR_CONSOLE(SizeWrite == sizeof(D3DXQUATERNION));

                     SizeWrite = fwrite(&pModel->m_Translation, 1, sizeof(D3DXVECTOR3), pFile);
                     KGLOG_PROCESS_ERROR_CONSOLE(SizeWrite == sizeof(D3DXVECTOR3));
                 }
             }
         }
         break;

	 case REPRESENTOBJECT_DEFAULT:
		 {
             ASSERT(0);

		 }
		 break;
	 case EXT_REPRESENTOBJECT_SET2:
		 {
			 char ModelSetName[256];
			 char RetName[256];
			 int  nCurModel = 0;

			 KG3DRepresentObjectSet *pSet = static_cast<KG3DRepresentObjectSet*>(pObject);
			 KGLOG_PROCESS_ERROR_CONSOLE(pSet);

			 hr = pSet->GetModelSetIniFileName(ModelSetName);
			 KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

			 nCurModel = pSet->GetCurrentModel();

			 hr = g_pFileFinder->GetRelativePath(g_Def_WorkDirectory, ModelSetName, RetName);
			 KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

			 SizeWrite = fwrite(RetName, 1, sizeof(char) * 256, pFile);
			 KGLOG_PROCESS_ERROR_CONSOLE(SizeWrite == sizeof(char) * 256);

			 SizeWrite = fwrite(&nCurModel, 1, sizeof(int), pFile);
			 KGLOG_PROCESS_ERROR_CONSOLE(SizeWrite == sizeof(int));

			 KG3DModel *pModel = NULL;
			 hr = pSet->GetModel(pModel, 0,FALSE);
			 
			 INT32 nLightmapTextureIndex = -1;
			 SizeWrite = fwrite(&nLightmapTextureIndex, 1, sizeof(INT32), pFile);
			 KGLOG_PROCESS_ERROR_CONSOLE(SizeWrite == sizeof(INT32));

		 }
		 break;	
	 case REPRESENTOBJECT_SET:
		 {
			 char ModelSetName[256];
			 char RetName[256];
			 int  nCurModel = 0;
			 
			 KG3DRepresentObjectSet *pSet = static_cast<KG3DRepresentObjectSet*>(pObject);
			 KGLOG_PROCESS_ERROR_CONSOLE(pSet);

			 hr = pSet->GetModelSetIniFileName(ModelSetName);
             KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

			 nCurModel = pSet->GetCurrentModel();

			 hr = g_pFileFinder->GetRelativePath(g_Def_WorkDirectory, ModelSetName, RetName);
             KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

			 SizeWrite = fwrite(RetName, 1, sizeof(char) * 256, pFile);
             KGLOG_PROCESS_ERROR_CONSOLE(SizeWrite == sizeof(char) * 256);

			 SizeWrite = fwrite(&nCurModel, 1, sizeof(int), pFile);
             KGLOG_PROCESS_ERROR_CONSOLE(SizeWrite == sizeof(int));
		 }
		 break;

     default:
         break;
	 }

	 hr = S_OK;
Exit0:
	 return hr;
 }


 bool operator< (const POINT& _Left, const POINT& _Right)
 {
    if (_Left.x < _Right.x)
        return true;

    if (_Left.x == _Right.x)
        return _Left.y < _Right.y;

    return false;
 }



 //bool operator<(const POINT& _Left, const POINT& _Right)
 //{
	// return ( (_Left.y < _Right.y) && (_Left.x < _Right.x) );
 //}
 //
 bool operator==(const POINT& _Left, const POINT& _Right)
 {
	 return ( (_Left.y == _Right.y) && (_Left.x == _Right.x) );
 }





 HRESULT KG3DSceneSceneEditor::ImportOldFile(LPSTR pFileName)
 {
	 //HRESULT hr = S_OK;
	 return E_FAIL;

 }
 //HRESULT KG3DSceneSceneEditor::ScaleBegin(D3DXVECTOR3& Origin,D3DXVECTOR3& Direction)
 //{
	// KG3DSceneSelectBase::ScaleBegin(Origin,Direction);
	// return S_OK;
 //}

 //HRESULT KG3DSceneSceneEditor::ScaleEnd()//代码移动到OnReceiveMessage
 //{
	// TypeEntityAccessProxy& proxy = this->GetSelectionTool().GetSelectionAccessProxy();
	// for(proxy.Begin(); ! proxy.IsEnd(); proxy.StepForward())
	// {
	//	 KG3DSceneEntity& Entity = proxy.GetCur();

	//	 if (Entity.GetType()==SCENEENTITY_OBJECT)
	//	 {

	//		 KG3DRepresentObject* pObject = 
	//			 static_cast<KG3DRepresentObject*>(Entity.m_pPointer);

	//		 if(pObject->GetType()==REPRESENTOBJECT_NPC)
	//		 {
	//			 FireEvent2(EVENTEDITORNPCSCALED,0,0,pObject);
	//		 }
	//		 else if(pObject->GetType()==REPRESENTOBJECT_DOODAD)
	//		 {
	//			 FireEvent2(EVENTEDITORDOODADSCALED,0,0,pObject);
	//		 }

 //            pObject->ComputeBBox();
 //            pObject->OnTransformationChange(this);
	//	 }
 //   }

	// KG3DSceneSelectBase::ScaleEnd();

 //    if (m_pEditSpaceNode != m_lpSceneOutDoorSpaceMgr)
 //        UpdateEditSpaceNodeInformation();

	// return S_OK;
 //}

 //HRESULT KG3DSceneSceneEditor::RotateBegin(D3DXVECTOR3& Origin,D3DXVECTOR3& Direction)
 //{
	// KG3DSceneSelectBase::RotateBegin(Origin,Direction);
	// return S_OK;
 //}

 //HRESULT KG3DSceneSceneEditor::RotateEnd()	//代码移动到OnReceiveMessage
 //{
	// TypeEntityAccessProxy& proxy = this->GetSelectionTool().GetSelectionAccessProxy();
	// for(proxy.Begin(); ! proxy.IsEnd(); proxy.StepForward())
	// {
	//	 KG3DSceneEntity& Entity = proxy.GetCur();

	//	 if (Entity.GetType()==SCENEENTITY_OBJECT)
	//	 {

	//		 KG3DRepresentObject* pObject = 
	//			 static_cast<KG3DRepresentObject*>(Entity.m_pPointer);

	//		 if(pObject->GetType()==REPRESENTOBJECT_NPC)
	//		 {
	//			 FireEvent2(EVENTEDITORNPCROTATED,0,0,pObject);
	//		 }
	//		 else if(pObject->GetType()==REPRESENTOBJECT_DOODAD)
	//		 {
	//			 FireEvent2(EVENTEDITORDOODADROTATED,0,0,pObject);
	//		 }
	//		 else if(pObject->GetType()==REPRESENTOBJECT_WAYPOINT)
	//		 {
	//			 FireEvent2(EVENTEDITORWAYPOINTROTATED,0,0,pObject);
	//		 }	
	//		 else if(pObject->GetType()==REPRESENTOBJECT_REFRESHPOINT)
	//		 {
	//			 FireEvent2(EVENTEDITORREFRESHPOINTROTATED,0,0,pObject);
	//		 }

 //            pObject->ComputeBBox();
 //            pObject->OnTransformationChange(this);
	//	 }
	// }
	// //////////////////////////////////////////////////////////////////////////
	// KG3DSceneSelectBase::RotateEnd();

 //    if (m_pEditSpaceNode != m_lpSceneOutDoorSpaceMgr)
 //        UpdateEditSpaceNodeInformation();

	// return S_OK;
 //}


//<Move Refactor>
 //HRESULT KG3DSceneSceneEditor::MoveBegin(D3DXVECTOR3& Origin,D3DXVECTOR3& Direction)
 //{
	// KG3DSceneSelectBase::MoveBegin(Origin,Direction);
	// return S_OK;
 //}

//2009-2-4 MoveEnd 的代码移动到OnReceiveMessage中

 //HRESULT KG3DSceneSceneEditor::MoveEnd()
 //{
	// TypeEntityAccessProxy& proxy = this->GetSelectionTool().GetSelectionAccessProxy();
	// for(proxy.Begin(); ! proxy.IsEnd(); proxy.StepForward())
	// {
	//	KG3DSceneEntity& Entity = proxy.GetCur();

	//	 if (Entity.GetType()==SCENEENTITY_OBJECT)
	//	 {

	//		 KG3DRepresentObject* pObject = 
	//			 static_cast<KG3DRepresentObject*>(Entity.m_pPointer);

	//		 if(pObject->GetType()==REPRESENTOBJECT_NPC)
	//		 {
	//			 FireEvent2(EVENTEDITORNPCMOVED,0,0,pObject);
	//		 }
	//		 else if(pObject->GetType()==REPRESENTOBJECT_DOODAD)
	//		 {
	//			 FireEvent2(EVENTEDITORDOODADMOVED,0,0,pObject);
	//		 }
	//		 else if(pObject->GetType()==REPRESENTOBJECT_WAYPOINT)
	//		 {
	//			 FireEvent2(EVENTEDITORWAYPOINTMOVED,0,0,pObject);
	//		 }	
	//		 else if(pObject->GetType()==REPRESENTOBJECT_TRAFFICPOINT)
	//		 {
	//			 FireEvent2(EVENTEDITORTRAFFICPOINTMOVED,0,0,pObject);
	//		 }	
	//		 else if(pObject->GetType()==REPRESENTOBJECT_REFRESHPOINT)
	//		 {
	//			 FireEvent2(EVENTEDITORREFRESHPOINTMOVED,0,0,pObject);
	//		 }
	//		 else if (pObject->GetType()==REPRESENTOBJECT_NODE)// add by huangjinshou 2008-2-21
	//		 {
	//			 KG3DRepresentObjectNode* pObjectNode = (KG3DRepresentObjectNode*) pObject;
	//			 pObjectNode->Refresh();
	//		 }

	//		 pObject->ComputeBBox();
	//		 pObject->OnTransformationChange(this);
	//	 }
	// }
	// //////////////////////////////////////////////////////////////////////////
	// KG3DSceneSelectBase::MoveEnd();

 //    if (m_pEditSpaceNode != m_lpSceneOutDoorSpaceMgr)
 //        UpdateEditSpaceNodeInformation();

	// return S_OK;
 //}

 void KG3DSceneSceneEditor::ZoomCoordinate(float fZoom)
 {
     switch (this->GetEditState())
     {
     case SCENESTATE_MOVE :
         this->GetCoordMove().Zoom(fZoom);
         break;
     case SCENESTATE_ROTATE :
         this->GetCoordRotate().Zoom(fZoom);
         break;
     case SCENESTATE_SCALE :
         this->GetCoordScale().Zoom(fZoom);
         break;
     default :
         break;
     }
 }

 HRESULT KG3DSceneSceneEditor::RenderConverMapTexture(KG3DSceneOutputWnd* pWnd)
 {
     HRESULT hrResult = E_FAIL;
     HRESULT hr = E_FAIL;
     KG3DCamera vCamera;
     KG3DTerrainConverMap* pConverMap = NULL;
     KG3DTexture* p3DTextureLight = NULL;
     LPDIRECT3DTEXTURE9 pRenderTargetTexture = NULL;
     LPDIRECT3DSURFACE9 pRenderTargetSurface = NULL;
     LPDIRECT3DSURFACE9 pDepthStencilSurface = NULL;
     LPDIRECT3DTEXTURE9 pTextureLight = NULL;
     LPDIRECT3DSURFACE9 pDepthStencilSurfaceSave = NULL;
     LPDIRECT3DSURFACE9 pRenderTargetSurfaceSave = NULL;
     D3DXMATRIX mWorldSave;
     D3DXMATRIX mViewSave;
     D3DXMATRIX mProjectionSave;
     D3DVIEWPORT9 ViewPort;
     D3DVIEWPORT9 ViewPortSave;
     D3DSURFACE_DESC Desc;
     D3DXVECTOR4 vConverMap;
     D3DXVECTOR3 vLookAt;
     D3DXVECTOR3 vPos;
     BOOL bPrepareRender = FALSE;
     BOOL bBeginScene = FALSE;

     KG_PROCESS_SUCCESS(m_lpSceneOutDoorSpaceMgr == NULL);

     pConverMap = m_lpSceneOutDoorSpaceMgr->m_lpConverMap;
     KG_PROCESS_SUCCESS(pConverMap == NULL);

     pRenderTargetTexture = pConverMap->GetRenderTargetTex();
     KG_PROCESS_SUCCESS(pRenderTargetTexture == NULL);

     pDepthStencilSurface = g_cGraphicsTool.GetShareDepthSurface512();
     KG_PROCESS_SUCCESS(pDepthStencilSurface == NULL);

     p3DTextureLight = pConverMap->m_lpTextureLight;
     KG_PROCESS_SUCCESS(p3DTextureLight == NULL);

     pTextureLight = p3DTextureLight->GetTexture();
     KG_PROCESS_SUCCESS(pTextureLight == NULL);

     // Save back buffer & transform & viewport
     g_pd3dDevice->GetRenderTarget(0, &pRenderTargetSurfaceSave);
     g_pd3dDevice->GetDepthStencilSurface(&pDepthStencilSurfaceSave);
     g_pd3dDevice->GetTransform(D3DTS_WORLD, &mWorldSave);
     g_pd3dDevice->GetTransform(D3DTS_VIEW, &mViewSave);
     g_pd3dDevice->GetTransform(D3DTS_PROJECTION, &mProjectionSave);
     g_pd3dDevice->GetViewport(&ViewPortSave);

     pRenderTargetTexture->GetLevelDesc(0,&Desc);
     pRenderTargetTexture->GetSurfaceLevel(0, &pRenderTargetSurface);

     ViewPort.X = 0;
     ViewPort.Y = 0;
     ViewPort.Width = Desc.Width;
     ViewPort.Height = Desc.Height;
     ViewPort.MinZ = 0.0f;
     ViewPort.MaxZ = 1.0f;

     g_pd3dDevice->SetViewport(&ViewPort);

     g_pd3dDevice->SetRenderTarget(0, pRenderTargetSurface);
     g_pd3dDevice->SetDepthStencilSurface(pDepthStencilSurface);

     bPrepareRender = TRUE;

     vConverMap = pConverMap->GetRect();

     vLookAt = D3DXVECTOR3(vConverMap.x + vConverMap.z * 0.5f, 0.0f, vConverMap.y - vConverMap.w * 0.5f);
     vPos = vLookAt + D3DXVECTOR3(0.0f, 40000.0f, 0.0f);

     vCamera.SetPosition(vPos);
     vCamera.SetLookAtPosition(vLookAt);
     vCamera.SetOrthogonal(vConverMap.z, vConverMap.w, 100.0f, 80000.0f);
     vCamera.SetUpDirection(D3DXVECTOR3(0.0f, 0.0f, 1.0f));
     vCamera.SetCamera();

     // began to render
     g_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
     g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);

     //hr = g_pd3dDevice->BeginScene();
    // KGLOG_COM_PROCESS_ERROR(hr);

     bBeginScene = TRUE;

     g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0xFF000000, 1.0f, 0);

     hr = OnRenderConverMapTexture(&vCamera, pTextureLight, pConverMap);
     KGLOG_COM_PROCESS_ERROR(hr);

Exit1:
     hrResult = S_OK;
Exit0:
     /*if (bBeginScene)
     {
         g_pd3dDevice->EndScene();
     }*/

     if (bPrepareRender)
     {
         //Restore
         g_pd3dDevice->SetTransform(D3DTS_WORLD, &mWorldSave);
         g_pd3dDevice->SetTransform(D3DTS_VIEW, &mViewSave);
         g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &mProjectionSave);

         g_pd3dDevice->SetRenderTarget(0, pRenderTargetSurfaceSave);
         g_pd3dDevice->SetDepthStencilSurface(pDepthStencilSurfaceSave);
         g_pd3dDevice->SetViewport(&ViewPortSave);
     }

     KG_COM_RELEASE(pRenderTargetSurface);
     KG_COM_RELEASE(pRenderTargetSurfaceSave);
     KG_COM_RELEASE(pDepthStencilSurfaceSave);

     return hrResult;
 }

 HRESULT KG3DSceneSceneEditor::RenderRefractTexture(KG3DSceneOutputWnd* pWnd)
 {
     HRESULT hResult  = E_FAIL;
     HRESULT hRetCode = E_FAIL;

     IDirect3DSurface9* pTarget = NULL;
     IDirect3DSurface9* pRefrac = NULL;

     D3DVIEWPORT9 ViewPort;
     RECT RectView;

     KG_PROCESS_ERROR(m_lpWaterEffect);
     KG_PROCESS_ERROR(m_lpWaterEffect->m_lpRefractTexture);

     hRetCode = m_EntityMgr.RenderWaterRefract(this);
     KG_COM_PROCESS_ERROR(hRetCode);

     hRetCode = g_pd3dDevice->GetRenderTarget(0, &pTarget);
     KG_COM_PROCESS_ERROR(hRetCode);

     hRetCode = m_lpWaterEffect->m_lpRefractTexture->GetSurfaceLevel(0, &pRefrac);
     KG_COM_PROCESS_ERROR(hRetCode);

	 if(g_bEnableScaleOutput)
	 {
		 SetRect(&RectView, 0, 0, g_nScaleOutputSize, g_nScaleOutputSize);
	 }
	 else
	 {
		 g_pd3dDevice->GetViewport(&ViewPort);
		 SetRect(&RectView, ViewPort.X, ViewPort.Y, ViewPort.X + ViewPort.Width, ViewPort.Y + ViewPort.Height);
	 }

     hRetCode = g_pd3dDevice->StretchRect(pTarget, &RectView, pRefrac, 0, D3DTEXF_NONE); 
     KG_COM_PROCESS_ERROR(hRetCode);

     if (false)
         D3DXSaveTextureToFile("d:\\test1.bmp",D3DXIFF_BMP, m_lpWaterEffect->m_lpRefractTexture, NULL);
     if (false)
         D3DXSaveSurfaceToFile("d:\\test2.bmp",D3DXIFF_BMP, pTarget, NULL, NULL);

     hResult = S_OK;
Exit0 :
     SAFE_RELEASE(pRefrac);
     SAFE_RELEASE(pTarget);
     return hResult;
 }

 HRESULT KG3DSceneSceneEditor::RenderReflectTexture(KG3DSceneOutputWnd* pWnd,BOOL bNeedBeginScene)
 {
     KG3DCamera* pCamera = &pWnd->GetCamera();
	 {
		 float a,b,c,d;
		 pCamera->GetPerspective(&a,&b,&c,&d);
		 pCamera->SetPerspective(m_fFovy,b,c,m_fZFar);
	 }
	 g_cGraphicsTool.SetCamera(pCamera);
	 LPDIRECT3DSURFACE9 lpDepth = g_cGraphicsTool.GetShareDepthSurface512();

	 if(!m_lpWaterEffect)
		 return S_OK;

	 if(!m_lpWaterEffect->m_lpReflectTexture)
         return S_OK;	
     if(!lpDepth)
         return S_OK;

     D3DXMATRIX Mat_Wold_Save;
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
     g_pd3dDevice->GetTransform( D3DTS_WORLD, &Mat_Wold_Save );
     g_pd3dDevice->GetTransform( D3DTS_VIEW, &Mat_View_Save );
     g_pd3dDevice->GetTransform( D3DTS_PROJECTION, &Mat_Proj_Save );
     g_pd3dDevice->GetViewport(&ViewPortSave);
     //////////////////////////////////////////////////////////////////////////

	 //Force disable shadow when rendering reflect texture.
	 LPD3DXEFFECT pShader = g_GetDefaultShaderGroup().GetDefaultEffect(DEFST_LIBRARY);
	 _ASSERTE(NULL != pShader);
	 if (pWnd && pShader)
		g_SetShadowParams(pShader,*pWnd,TRUE);

     D3DSURFACE_DESC Desc;

     m_lpWaterEffect->m_lpReflectTexture->GetLevelDesc(0,&Desc);
     m_lpWaterEffect->m_lpReflectTexture->GetSurfaceLevel(0,&lpTarget);

     ViewPort.X = 0;ViewPort.Y = 0;
     ViewPort.Width = Desc.Width;
     ViewPort.Height = Desc.Height;
     ViewPort.MinZ = 0;ViewPort.MaxZ = 1.0f;

     g_pd3dDevice->SetViewport(&ViewPort);

     g_pd3dDevice->SetRenderTarget(0,lpTarget);
     g_pd3dDevice->SetDepthStencilSurface(lpDepth);

     D3DXVECTOR3 PosBack,LookatBack;
     D3DXVECTOR3 Pos,Lookat;
     float a,b,c,d;
     pCamera->GetPosition(&PosBack);
     pCamera->GetLookAtPosition(&LookatBack);
     pCamera->GetPerspective(&a,&b,&c,&d);

     Pos = PosBack;Pos.y = 2 * m_fWaterHeigh - Pos.y ;
     Lookat = LookatBack;Lookat.y = 2 * m_fWaterHeigh - Lookat.y;
     
	 /*pCamera->SetPosition(Pos);
     pCamera->SetLookAtPosition(Lookat);
     pCamera->SetUpDirection(D3DXVECTOR3(0,-1,0));
     pCamera->SetCamera();*/

	 pCamera->SetPositionLookAtUp(&Pos, &Lookat, &D3DXVECTOR3(0,-1,0));
     
	 //////////////////////////////////////////////////////////////////////////
     //reade began to render
     g_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
     g_pd3dDevice->SetRenderState(D3DRS_ZENABLE,D3DZB_TRUE);

	 if(bNeedBeginScene)
		 g_pd3dDevice->BeginScene(); // already in scene

     g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 
         0x00000000, 1.0f, 0 );

     OnRenderReflectTexture(pCamera);

	 if(bNeedBeginScene)
		 g_pd3dDevice->EndScene();
     /////////////////////////////////////////////////////////////////////////
     //Restore
	 if (pWnd && pShader)
		 g_SetShadowParams(pShader,*pWnd);
     g_pd3dDevice->SetTransform( D3DTS_WORLD, &Mat_Wold_Save );
     g_pd3dDevice->SetTransform( D3DTS_VIEW, &Mat_View_Save );
     g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &Mat_Proj_Save );
     g_pd3dDevice->SetRenderTarget(0,lpSurfaceSave);
     g_pd3dDevice->SetDepthStencilSurface(lpSurfaceDepSave);
     g_pd3dDevice->SetViewport(&ViewPortSave);

     /*pCamera->SetPosition(PosBack);
     pCamera->SetLookAtPosition(LookatBack);
     pCamera->SetUpDirection(D3DXVECTOR3(0,1,0));
     pCamera->SetCamera();*/

	 pCamera->SetPositionLookAtUp(&PosBack, &LookatBack, &D3DXVECTOR3(0,1,0));
   
     KG_COM_RELEASE(lpTarget);
     KG_COM_RELEASE(lpSurfaceSave);
     KG_COM_RELEASE(lpSurfaceDepSave);

     return S_OK;
 }

 HRESULT KG3DSceneSceneEditor::RenderWaterWaveTexture(KG3DSceneOutputWnd* pWnd)
 {
	 KG3DCamera* pCamera = &pWnd->GetCamera();
	 LPDIRECT3DSURFACE9 lpDepth = g_cGraphicsTool.GetShareDepthSurface512();

	 if(!m_lpWaterEffect->m_lpWaveTexture)
		 return S_OK;	
	 if(!lpDepth)
		 return S_OK;

	 D3DXMATRIX Mat_Wold_Save;
	 D3DXMATRIX Mat_View_Save;
	 D3DXMATRIX Mat_Proj_Save;
	 static D3DXMATRIX matworld = *D3DXMatrixIdentity(&matworld);

	 LPDIRECT3DSURFACE9 piSurfaceDepSave = NULL;
	 LPDIRECT3DSURFACE9 piSurfaceSave = NULL;
	 LPDIRECT3DSURFACE9 piTarget = NULL;
	 D3DVIEWPORT9 ViewPort,ViewPortSave;
	 //////////////////////////////////////////////////////////////////////////
	 //Save back buffer & transform & viewport
	 g_pd3dDevice->GetRenderTarget(0,&piSurfaceSave);
	 g_pd3dDevice->GetDepthStencilSurface(&piSurfaceDepSave);
	 g_pd3dDevice->GetTransform( D3DTS_WORLD, &Mat_Wold_Save );
	 g_pd3dDevice->GetTransform( D3DTS_VIEW, &Mat_View_Save );
	 g_pd3dDevice->GetTransform( D3DTS_PROJECTION, &Mat_Proj_Save );
	 g_pd3dDevice->GetViewport(&ViewPortSave);
	 //////////////////////////////////////////////////////////////////////////
	 D3DSURFACE_DESC Desc;
	 m_lpWaterEffect->m_lpWaveTexture->GetLevelDesc(0,&Desc);
	 m_lpWaterEffect->m_lpWaveTexture->GetSurfaceLevel(0,&piTarget);

	 ViewPort.X = 0;ViewPort.Y = 0;
	 ViewPort.Width = Desc.Width;
	 ViewPort.Height = Desc.Height;
	 ViewPort.MinZ = 0;ViewPort.MaxZ = 1.0f;

	 g_pd3dDevice->SetViewport(&ViewPort);

	 g_pd3dDevice->SetRenderTarget(0,piTarget);
	 g_pd3dDevice->SetDepthStencilSurface(lpDepth);
	 pWnd->GetCamera().SetCamera();
	 g_pd3dDevice->SetTransform( D3DTS_WORLD, &matworld );

	 //////////////////////////////////////////////////////////////////////////
	 //reade began to render
	// g_pd3dDevice->BeginScene();

	 g_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
	 g_pd3dDevice->SetRenderState(D3DRS_ZENABLE,D3DZB_TRUE);

	 g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 
		 0, 1.0f, 0 );

	 OnRenderWaterWaveTexture(pCamera);

	 //g_pd3dDevice->EndScene();
	 /////////////////////////////////////////////////////////////////////////
	 //Restore
	 g_pd3dDevice->SetTransform( D3DTS_WORLD, &Mat_Wold_Save );
	 g_pd3dDevice->SetTransform( D3DTS_VIEW, &Mat_View_Save );
	 g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &Mat_Proj_Save );
	 g_pd3dDevice->SetRenderTarget(0,piSurfaceSave);
	 g_pd3dDevice->SetDepthStencilSurface(piSurfaceDepSave);
	 g_pd3dDevice->SetViewport(&ViewPortSave);

	 KG_COM_RELEASE(piTarget);
	 KG_COM_RELEASE(piSurfaceSave);
	 KG_COM_RELEASE(piSurfaceDepSave);

	 return S_OK;
 }

 void KG3DSceneSceneEditor::GetWaterClipPane(D3DXPLANE* vFFP, D3DXPLANE* vPP, float fWaterHeight, bool bUnder)
 {
     if (!vFFP || !vPP)
         return;

     D3DXMATRIX matVP,matView,matProj;
     g_pd3dDevice->GetTransform(D3DTS_VIEW,&matView);
     g_pd3dDevice->GetTransform(D3DTS_PROJECTION,&matProj);
     matVP = matView * matProj;
     D3DXMatrixTranspose(&matVP,&matVP);
     D3DXMatrixInverse(&matVP,NULL,&matVP);

     if(bUnder)
         D3DXPlaneFromPointNormal(vFFP,&D3DXVECTOR3(0,fWaterHeight,0),&D3DXVECTOR3(0,-1,0));
     else
         D3DXPlaneFromPointNormal(vFFP,&D3DXVECTOR3(0,fWaterHeight,0),&D3DXVECTOR3(0,1,0));

     D3DXPlaneTransform(vPP, vFFP, &matVP);
 }


 HRESULT KG3DSceneSceneEditor::OnRenderReflectTexture(KG3DCamera *pCamera)
 {
	 D3DXPLANE WaterPlanePP;
     D3DXPLANE WaterPlaneFFP;

     GetWaterClipPane(&WaterPlaneFFP, &WaterPlanePP, m_fWaterHeigh, !!m_bUnderWater); 
	 
	/* D3DXMATRIX matVP,matView,matProj;
	 g_pd3dDevice->GetTransform(D3DTS_VIEW,&matView);
	 g_pd3dDevice->GetTransform(D3DTS_PROJECTION,&matProj);
	 matVP = matView * matProj;
	 D3DXMatrixTranspose(&matVP,&matVP);
	 D3DXMatrixInverse(&matVP,NULL,&matVP);

	 if(m_bUnderWater)
		 D3DXPlaneFromPointNormal(&WaterPlaneFFP,&D3DXVECTOR3(0,m_fWaterHeigh,0),&D3DXVECTOR3(0,-1,0));
	 else
		 D3DXPlaneFromPointNormal(&WaterPlaneFFP,&D3DXVECTOR3(0,m_fWaterHeigh,0),&D3DXVECTOR3(0,1,0));

	 D3DXPlaneTransform(&WaterPlanePP, &WaterPlaneFFP, &matVP);*/


	 g_pd3dDevice->SetRenderState(D3DRS_CLIPPLANEENABLE,D3DCLIPPLANE0);
	 g_pd3dDevice->SetClipPlane(0,(float *)&WaterPlanePP);

	
	 g_pd3dDevice->SetRenderState(D3DRS_CLIPPLANEENABLE,FALSE);

    if (g_cEngineManager.GetRenderOption(RENDER_ENVIRONMENT))
    {
        //if (g_cEngineOption.nWaterDetail >= WD_VERY_HIGH)
	        GetEnvironment().Render();
        //else
        //    GetEnvironment().Render(KG3DTYPE_ENVEFF_SKYSYSTEM_FARMOUNTAIN);
    }

    if (g_cEngineOption.nWaterDetail >= WD_VERY_HIGH)
    {
        m_EntityMgr.RenderReflection(this,m_fWaterHeigh,WaterPlaneFFP,WaterPlanePP);
    }

    if (g_cEngineManager.GetRenderOption(RENDER_ENVIRONMENT))
	    GetEnvironment().PostRender();
	 
	 return S_OK;
 }

 HRESULT KG3DSceneSceneEditor::OnRenderConverMapTexture(KG3DCamera *pCamera,LPDIRECT3DTEXTURE9 pTLight,KG3DTerrainConverMap* pConvermap)
 {
	 GraphicsStruct::RenderState Fog(D3DRS_FOGENABLE,FALSE);
	 GraphicsStruct::RenderState Light(D3DRS_LIGHTING,FALSE);
	 GraphicsStruct::RenderState Zenable(D3DRS_ZENABLE,FALSE);

     D3DXVECTOR4 Value =D3DXVECTOR4(1,1,1,1);

	 {
		 GraphicsStruct::RenderStateAlpha Alpha(0x20,FALSE,FALSE,D3DBLEND_SRCALPHA,D3DBLEND_ONE);
		 D3DXVECTOR3 A = D3DXVECTOR3(pConvermap->GetRect().x,0,pConvermap->GetRect().y);
		 D3DXVECTOR3 B = A + D3DXVECTOR3(pConvermap->GetRect().z,0,0);
		 D3DXVECTOR3 D = A + D3DXVECTOR3(0,0,-pConvermap->GetRect().w);

		 //float K = sinf(timeGetTime()*0.00002F);

		 D3DCOLOR color = D3DCOLOR_RGBA((int)(Value.y*255),(int)(Value.z*255),(int)(Value.w*255),255);
		 
		 g_cGraphicsTool.DrawRect(&A,&B,&D,color,pConvermap->GetColorTex());
	 }

	 GraphicsStruct::RenderStateAlpha Alpha(0x20,TRUE,TRUE,D3DBLEND_SRCALPHA,D3DBLEND_ONE);

	 //D3DCOLOR color = D3DCOLOR_RGBA((int)(Value.x*255),(int)(Value.x*255),(int)(Value.x*255),255);
	 /*if(m_pRepresentNPC)
	 {
		 D3DXVECTOR3 vPostion;
		 m_pRepresentNPC->GetTranslation(&vPostion);

		 float r = 2000;

		 D3DXVECTOR3 A = vPostion + D3DXVECTOR3(-r,0,-r);
		 D3DXVECTOR3 B = vPostion + D3DXVECTOR3( r,0,-r);
		 D3DXVECTOR3 D = vPostion + D3DXVECTOR3(-r,0, r);


		 g_cGraphicsTool.DrawRect(&A,&B,&D,color,pTLight);
	 }*/

	 //////////////////////////////////////////////////////////////////////////
	 /*{
		 KG3DSceneEntityList::iterator eIter = m_listEntityRenderLast.begin();
		 D3DXVECTOR3 vTrans;

		 for ( ;eIter != m_listEntityRenderLast.end(); ++ eIter )
		 {
			 KG3DSceneEntity *pEntity = &(*eIter);

			 if(pEntity->GetType() == SCENEENTITY_MODEL)
			 {
				 KG3DModel* pModel = static_cast<KG3DModel*>(pEntity->m_pPointer);
				 if(!pModel->m_bSelectable)
				 {
					 D3DXVECTOR3 vTrans;
					 pModel->GetTranslation(&vTrans);

					 float r = 2000;

					 D3DXVECTOR3 A = vTrans + D3DXVECTOR3(-r,0,-r);
					 D3DXVECTOR3 B = vTrans + D3DXVECTOR3( r,0,-r);
					 D3DXVECTOR3 D = vTrans + D3DXVECTOR3(-r,0, r);

					 g_cGraphicsTool.DrawRect(&A,&B,&D,color,pTLight);
				 }
			 }
		 }
	 }*/

	 //////////////////////////////////////////////////////////////////////////
	 {
		 D3DXVECTOR2 A(-0.5F,-0.5F);
		 D3DXVECTOR2 C(512,512);
		 D3DXVECTOR2 scale(2,2);

		 float K = timeGetTime()*0.00002F;

		 D3DXVECTOR2 t(K,K);
		 GraphicsStruct::RenderStateAlpha Alpha(0x0,FALSE,FALSE,D3DBLEND_DESTCOLOR,D3DBLEND_ZERO);
		 g_cGraphicsTool.DrawScreenRectUVTrans(&A,&C,0,scale,t,0xFFFFFFFF,pConvermap->m_lpTextureCloud);
	 }

	 return S_OK;
 }
 HRESULT KG3DSceneSceneEditor::OnRenderWaterWaveTexture(KG3DCamera *pCamera)
 {
	 if(m_lpWaterEffect)
	 {
		 m_lpWaterEffect->RenderWaveSFX(TRUE,FALSE);
	 }
	 return S_OK;
 }

 

 HRESULT KG3DSceneSceneEditor::DrapSelectedEntity()
 {
	 /*KG3DSceneEntityList::iterator i = m_listSelectedEntity.begin();
	 while (i!=m_listSelectedEntity.end())*/

	 TypeEntityAccessProxy& proxy = this->GetSelectionTool().GetSelectionAccessProxy();
	 for(proxy.Begin(); ! proxy.IsEnd(); proxy.StepForward())
	 {
		 //KG3DSceneEntity Entity = *i;
		 KG3DSceneEntity Entity = proxy.GetCur();

		 D3DXVECTOR3 Pos;
		 Entity.GetTranslation(&Pos);
		
		 float Height = Pos.y;
		 DWORD dwMask = EM_RAY_INTERSECTION_MASK_TERRAIN | EM_RAY_INTERSECTION_MASK_OBJECT;

		 D3DXVECTOR3 newPos = Pos;
		 struct CustomFilter : public KG3DIntersectionFilter 
		 {
				KG3DRepresentObject* m_pNotIntersectedObject;
				virtual HRESULT Accept(KG3DRepresentObject& Object)
				{
					if(&Object == m_pNotIntersectedObject)
						return E_FAIL;
					return S_OK;
				}
		 };
		 CustomFilter customFilter;
		 customFilter.m_pNotIntersectedObject = (KG3DRepresentObject*)Entity.m_pPointer;

		 BOOL bGetSucceeded = ::RayIntersectionVertical(*this,newPos,&Height,NULL,dwMask, &customFilter);
		

		 float fGroundHeight = m_lpSceneOutDoorSpaceMgr->GetGroundHeightEx(Pos.x,Pos.z);
		 if(Pos.y < fGroundHeight || ! bGetSucceeded)
			 Pos.y = fGroundHeight;
		 else
			 Pos.y = Height;
	
		 Entity.SetTranslation(&Pos);

		 if(Entity.GetType()==SCENEENTITY_GROUP)
		 {
			 KG3DSceneEntityGroup *pGroup = static_cast<KG3DSceneEntityGroup*>(Entity.m_pPointer);
			 KG3DSceneEntityList::iterator j = pGroup->m_listEntity.begin();
			 while (j!=pGroup->m_listEntity.end())
			 {
				 KG3DSceneEntity EntityInGroup = *j;
				 EntityInGroup.GetTranslation(&Pos);
				 Pos.y = GetHeight(Pos.x,Pos.z);
				 EntityInGroup.SetTranslation(&Pos);
				 j++;
			 }
		 }
		 if(Entity.GetType()==SCENEENTITY_OBJECT)
		 {
			 KG3DRepresentObject* pObject = 
				 static_cast<KG3DRepresentObject*>(Entity.m_pPointer);

			 pObject->FrameMove();//<SLEEP_REFRACTOR>
			 pObject->UpdateTransformation();

			 if(pObject->GetType()==REPRESENTOBJECT_NPC)
			 {
				 FireEvent2(EVENTEDITORNPCMOVED,0,0,pObject);
			 }
			 else if(pObject->GetType()==REPRESENTOBJECT_DOODAD)
			 {
				 FireEvent2(EVENTEDITORDOODADMOVED,0,0,pObject);
			 }
			 else if(pObject->GetType()==REPRESENTOBJECT_WAYPOINT)
			 {
				 FireEvent2(EVENTEDITORWAYPOINTMOVED,0,0,pObject);
			 }	
			 else if(pObject->GetType()==REPRESENTOBJECT_TRAFFICPOINT)
			 {
				 FireEvent2(EVENTEDITORTRAFFICPOINTMOVED,0,0,pObject);
			 }	
			 else if(pObject->GetType()==REPRESENTOBJECT_REFRESHPOINT)
			 {
				 FireEvent2(EVENTEDITORREFRESHPOINTMOVED,0,0,pObject);
			 }

             pObject->ComputeBBox();
             pObject->OnTransformationChange(this);
		 }
		 //i++;
	 }
	 return S_OK;
 }

 HRESULT KG3DSceneSceneEditor::RemoveEntityFromRenderList()
 {
	/* KG3DSceneEntityList::iterator i = m_listSelectedEntity.begin();
	 while ( i!= m_listSelectedEntity.end())
	 {
		 KG3DSceneEntity Entity = *i;*/

	 TypeEntityAccessProxy& proxy = this->GetSelectionTool().GetSelectionAccessProxy();
	 for(proxy.Begin(); ! proxy.IsEnd(); proxy.StepForward())
	 {
		 KG3DSceneEntity Entity = proxy.GetCur();

		 m_listEntityRenderLast.remove(Entity);

		 m_EntityMgr.RemoveEntity(0,Entity);

         if (!g_bClient)
         {
		    m_EntityMgrTopView.RemoveEntity(0,Entity);
         }
		 //i++;
	 }
     return S_OK;
 }

 HRESULT KG3DSceneSceneEditor::ShockwaveEntity()
{
    HRESULT hResult  = E_FAIL;

    KGLOG_PROCESS_ERROR(/*m_listSelectedEntity.size() == 1*/1 == this->GetSelectionTool().GetSelectionCount());

    RemoveEntityFromRenderList();

  //  {
  //      //KG3DSceneEntity Entity = *m_listSelectedEntity.begin();
		//TypeEntityAccessProxy& proxy = this->GetSelectionTool().GetSelectionAccessProxy();
		//proxy.Begin();
		//KG3DSceneEntity Entity = proxy.GetCur();

  //      KG3DSceneEntityList::iterator i = find(m_listRenderEntity.begin(), m_listRenderEntity.end(), Entity);
  //      if (i != m_listRenderEntity.end())
  //      {
  //          m_listRenderEntity.remove(Entity);
  //          m_listPostRenderEntity.push_back(Entity);
  //      }
  //      else
  //      {
  //          m_listPostRenderEntity.remove(Entity);
  //          m_listRenderEntity.push_back(Entity);
  //      }
  //  }

//Exit1:
    hResult = S_OK;
Exit0:
    return hResult;
}

 HRESULT KG3DSceneSceneEditor::DeleteSelectedEntity()
 {
	 BOOL bRefreshRoad = FALSE; 
	 BOOL bRefreshRiver = FALSE;
	

	 std::vector<KG3DSceneEntity> vecDelEntities;
	 
	 ULONG uSelCount = this->GetSelectionTool().GetSelectionCount();
	 KG_PROCESS_SUCCESS(0 == uSelCount);

	 {
		 //vector预分配空间就不会因为重分配行为造成巨大的时延了，对微软的伟大的连深浅拷贝都不分开的std尤其需要注意这个。
		 vecDelEntities.reserve(static_cast<size_t>(uSelCount));

		 //选择表里面的东西先取出来，清空了选择表之后再做其它释放动作，不然先释放了，选择表里面的东西就全是无主指针了
		 //咱选择表暂时没有引用计数
		 TypeEntityAccessProxy& proxy = this->GetSelectionTool().GetSelectionAccessProxy();
		 for(proxy.Begin(); ! proxy.IsEnd(); proxy.StepForward())
		 {
			 KG3DSceneEntity& entityRef = proxy.GetCur();
			 vecDelEntities.push_back(entityRef);
		 }

		 KG3DSceneEditorBase::DeleteSelectedEntity();
		 
		 for ( size_t i = 0, uVecCount = vecDelEntities.size(); i < uVecCount; ++i)
		 {
			 KG3DSceneEntity& entityRef = vecDelEntities[i];
			 RemoveRenderEntityAndNotifyLogic(entityRef, FALSE);

			 //下面这段代码是由于新地形下，Selection只有Entity没有Object造成的,以后KG3DSceneSelectionToolSolid完成之后要去掉
			 if (entityRef.GetType() == SCENEENTITY_OBJECT)
			 {
				 KG3DRepresentObject* pObject = (KG3DRepresentObject*)entityRef.m_pPointer;
				 if(pObject->GetType()==REPRESENTOBJECT_NODE)
				 {
					 DWORD dwNodeType = static_cast<KG3DRepresentObjectNode*>(pObject)->GetNodeType();
					 if(dwNodeType == NODETYPE_ROAD)
						 bRefreshRoad = TRUE;
					 else if(dwNodeType == NODETYPE_RIVER)
						 bRefreshRiver = TRUE;
				 }
				 pObject->OnDeleteFromScene(this);//通知这个事件
				 //pObject->SetScene(NULL);//<SetScene Refactor>
				 m_lpSceneOutDoorSpaceMgr->RemoveRepresentObject(pObject);
			 }


			 if (entityRef.GetType() == SCENEENTITY_PVS_PORTAL)
			 {
				 KG3DPvsPortal* ptl = (KG3DPvsPortal*)entityRef.m_pPointer;
				 if (ptl->GetParentPvs())
				 {
					 ptl->GetParentPvs()->DeletePortal(ptl);
				 }
			 }
		 }
		 if(m_lpSceneOutDoorSpaceMgr)
			 m_lpSceneOutDoorSpaceMgr->RefreshRoadRiver(bRefreshRoad,bRefreshRiver);
		
	 }
Exit1:
	 return S_OK;
 }

 HRESULT KG3DSceneSceneEditor::GroupSameNameAndTypeObjects()
 {
	 m_mapObjectGroup.clear();
	 
	 KG3DSceneEntityList::iterator i = m_listRenderEntity .begin();
	 while(i!=m_listRenderEntity.end())
	 {
		 KG3DSceneEntity Entity = *i;
		 if(Entity.GetType()==SCENEENTITY_OBJECT)
		 {
			 KG3DRepresentObject *pObject = static_cast<KG3DRepresentObject*>(Entity.m_pPointer);
			 if(pObject)
			 {
				 TCHAR Name[256];
				 pObject->GetName(Name, _countof(Name));

				 map<string,ObjectGroup>::iterator i = m_mapObjectGroup.find(Name);
				 if(i!=m_mapObjectGroup.end())
				 {
					 ObjectGroup *pGroup = &i->second;
					 pGroup->listObject.push_back(pObject);
				 }
				 else
				 {
					 ObjectGroup NewGroup;
					 NewGroup.listObject.push_back(pObject);
					 
					 m_mapObjectGroup.insert(map<string,ObjectGroup>::value_type(Name,NewGroup));
				 }
			 }
		 }
		 i++;
	 }
	 return S_OK;
 }

 HRESULT KG3DSceneSceneEditor::GetSelecteNPC(IEKG3DModel*& piModel)
 {
     KG3DModel *pModel = NULL;

	 if(/*m_listSelectedEntity.size()==0*/ 0 == this->GetSelectionTool().GetSelectionCount())
		 return E_FAIL;

	 /*KG3DSceneEntityList::iterator i = m_listSelectedEntity .begin();
	 while(i!=m_listSelectedEntity.end())
	 {
		 KG3DSceneEntity Entity = *i;*/
	 TypeEntityAccessProxy& proxy = this->GetSelectionTool().GetSelectionAccessProxy();
	 for(proxy.Begin(); ! proxy.IsEnd(); proxy.StepForward())
	 {
		 KG3DSceneEntity Entity = proxy.GetCur();

		 if(Entity.GetType()==SCENEENTITY_MODEL)
		 {
			 KG3DRepresentNPC *pNpc = (KG3DRepresentNPC*) Entity.m_pPointer;
			 if(SUCCEEDED(pNpc->GetModel(pModel,0,FALSE)))
             {
                 piModel = pModel;
				 return S_OK;
             }
		 }
		 //i++;
	 }
	 return E_FAIL;
 }

 HRESULT KG3DSceneSceneEditor::GetSelectPosition(D3DXVECTOR2 const& vScale, D3DXVECTOR3& vPosition)
 {
	 D3DXVECTOR3 RayOrig,RayDir;

	 //m_SceneWndManager->GetCurOutputWnd()->GetPickRay(&RayOrig,&RayDir,vScale);
	 m_SceneWndManager->GetCurOutputWnd()->GetPickRayFromWindowsPos(vScale, RayOrig, RayDir);

	 BOOL bFoundInter = FALSE;
	 FLOAT fDist = FLT_MAX;
	 bFoundInter = ::RayIntersection(*this, RayOrig, RayDir, &fDist, NULL, NULL);
	 KG_PROCESS_ERROR(bFoundInter);
	 vPosition = RayOrig + RayDir * fDist;
	 return S_OK;
Exit0:
	 return E_FAIL;
 }

extern int g_bUseNewSelection;
static DWORD s_dwSelectUpdateTimeSpan = 0;
static BOOL s_bGetSelectModelListInvoked = FALSE;
//HRESULT KG3DSceneSceneEditor::GetSelectModelListForClient(const D3DXVECTOR2 &vPosScreen, 
//														  std::vector<IKG3DModel*> &pList)
//{
//	if (g_bUseNewSelection)
//	{
//		POINT MousePoint;
//		MousePoint.x = static_cast<LONG>(vPosScreen.x);
//		MousePoint.y = static_cast<LONG>(vPosScreen.y);
//
//		g_Selector.m_MousePoint = MousePoint;
//		
//		pList.clear();
//		KG3DModel *pModel = g_Selector.GetSelected();
//		if (pModel)
//		{
//			pList.push_back(pModel);
//		}
//		s_bGetSelectModelListInvoked = TRUE;
//	}
//	else
//	{
//		GetSelectModelList(vPosScreen, pList);
//	}
//	
//	return S_OK;
//}

//HRESULT KG3DSceneSceneEditor::GetSelectModelList()//<SelectModelList Refactor>
//{
//	//这个是给客户端专用的
//	static DWORD dwLast = timeGetTime();
//	DWORD dwTimeCurrent = timeGetTime();
//	if (!s_bGetSelectModelListInvoked || dwTimeCurrent - dwLast < s_dwSelectUpdateTimeSpan)
//	{
//		return S_OK;
//	}
//
//	s_bGetSelectModelListInvoked = FALSE;
//
//	dwLast = dwTimeCurrent;
//
//	D3DXVECTOR3 RayOrig,RayDir;
//	
//	D3DXVECTOR2 vPosScreen(static_cast<float>(g_Selector.m_MousePoint.x), static_cast<float>(g_Selector.m_MousePoint.y));
//
//	g_Selector.ResetSelection();
//	//m_SceneWndManager->GetCurOutputWnd()->GetPickRay(&RayOrig,&RayDir,vPosScreen);
//	m_SceneWndManager->GetCurOutputWnd()->GetPickRayFromWindowsPos(vPosScreen, RayOrig, RayDir);
//
//	D3DXMATRIX matWorldViewProjScreen;
//	{
//		D3DXMATRIX matProj, matView, matWorld;
//		matProj = m_SceneWndManager->GetCurOutputWnd()->GetCamera().GetProjectionMatrix();
//		matView = m_SceneWndManager->GetCurOutputWnd()->GetCamera().GetViewMatrix();
//		D3DXMatrixIdentity(&matWorld);
//		D3DXMATRIX matViewPort;
//		D3DXMatrixViewPort(m_SceneWndManager->GetCurOutputWnd()->m_Viewport, matViewPort);
//
//		matWorldViewProjScreen = matWorld * matView * matProj * matViewPort;
//	}
//
//	struct Visitor : public KG3DSceneEntityVisitor 
//	{
//		D3DXMATRIX m_matAll;
//		virtual HRESULT Accept(KG3DSceneEntity& entity)
//		{
//			if(entity.GetType() != SCENEENTITY_MODEL)
//				return S_OK;
//
//			AABBOX box;
//			AABBOX boxScreen;
//			D3DXVECTOR3 boxPoses[8];
//
//			entity.GetBBox(&box);
//
//			box.Extract(boxPoses);
//
//			D3DXVec3TransformCoordArray(boxPoses, sizeof(D3DXVECTOR3), boxPoses, sizeof(D3DXVECTOR3), &m_matAll, _countof(boxPoses));
//			boxScreen.AddPosition(boxPoses, _countof(boxPoses));
//
//			if((boxScreen.A.z > -FLT_EPSILON) && (boxScreen.B.z - 1.f < FLT_EPSILON)&&
//				(g_Selector.m_MousePoint.x < boxScreen.B.x)&&(g_Selector.m_MousePoint.x > boxScreen.A.x)&&
//				(g_Selector.m_MousePoint.y < boxScreen.B.y)&&(g_Selector.m_MousePoint.y > boxScreen.A.y))
//			{
//				g_Selector.Push(static_cast<KG3DModel*>(entity.m_pPointer));
//			}
//			
//			return S_OK;
//		}
//	};
//	Visitor visitor;
//	visitor.m_matAll = matWorldViewProjScreen;
//
//	this->GetSelectionTool().TraverseSelectable(visitor);
//	return S_OK;
//}
//
//HRESULT KG3DSceneSceneEditor::GetSelectModelList(const D3DXVECTOR2 &fScale,std::vector<IKG3DModel*> &pList)//<SelectModelList Refactor>
//{
//	return GetSelectModelList(fScale, pList, TRUE);//为什么叫fScale？完全不是Scale的意思？	
//}
//
//
HRESULT KG3DSceneSceneEditor::GetSelectModelList(const D3DXVECTOR2& vPosScreen, std::vector<IKG3DModel*>& pList
												 , BOOL bGetNearestOrAll)//<SelectModelList Refactor>
{
	D3DXVECTOR3 RayOrig,RayDir;
	IKG3DModel *pimodel;

	HRESULT hr = E_FAIL;
	m_SceneWndManager->GetCurOutputWnd()->GetPickRayFromWindowsPos(vPosScreen, RayOrig, RayDir);

	hr = this->GetSelectionTool().PointSelection(EM_SELECTION_INCREASE_MODE_DEFAULT, RayOrig, RayDir, bGetNearestOrAll);//注意这里没有触发SelectEnd，即没有发出选择的消息，原因是不需要，选择列表是立刻就清空的。
	KG_COM_PROCESS_ERROR(hr);

	TypeEntityAccessProxy& proxy = this->GetSelectionTool().GetSelectionAccessProxy();
	for(proxy.Begin(); ! proxy.IsEnd(); proxy.StepForward())
	{
		KG3DSceneEntity Entity = proxy.GetCur();
		pimodel = static_cast<IKG3DModel *>(Entity.m_pPointer);
		pList.push_back(pimodel);
	}

	ClearSelection();

	return S_OK;
Exit0:
	return S_FALSE;	//逻辑那边如果失败一定会写Log的，所以这里不返回E_FAIL
}
//HRESULT KG3DSceneSceneEditor::GetSelectModelList(const D3DXVECTOR2 &vLeftUp,const D3DXVECTOR2 &vRightBottom,
//												 std::vector<IKG3DModel*> &pList)//<SelectModelList Refactor>
//{
//	//return S_OK;
//
//	D3DXVECTOR3 RayOrig,RayDir;
//	IKG3DModel *pimodel;
//
//	_ASSERTE(NULL != m_SceneWndManager->GetCurOutputWnd());
//	m_SceneWndManager->GetCurOutputWnd()->GetPickRayFromWindowsPos(vLeftUp, RayOrig, RayDir);
//	
//
//	/*SelectBegin(RayOrig,RayDir,vLeftUp,vRightBottom,EM_SELECTION_INCREASE_MODE_DEFAULT);
//	SelectFrameMove(RayOrig,RayDir,vRightBottom);
//	D3DXMATRIX Proj,View;
//	Proj = m_SceneWndManager->GetCurOutputWnd()->GetCamera().GetProjectionMatrix();
//	View = m_SceneWndManager->GetCurOutputWnd()->GetCamera().GetViewMatrix();
//
//	SelectEnd(m_SceneWndManager->GetCurOutputWnd()->m_Viewport,Proj,View);*/
//
//	//2009-02-03 选择可以直接选
//	D3DXVECTOR4 vLeftTopRightBottom(vLeftUp.x, vLeftUp.y, vRightBottom.x, vRightBottom.y);
//	const D3DXMATRIX& matProj = m_SceneWndManager->GetCurOutputWnd()->GetCamera().GetProjectionMatrix();
//	const D3DXMATRIX& matView = m_SceneWndManager->GetCurOutputWnd()->GetCamera().GetViewMatrix();
//	D3DXMATRIX matViewport;
//	D3DXMatrixViewPort(m_SceneWndManager->GetCurOutputWnd()->m_Viewport, matViewport);
//	this->GetSelectionTool().RectSelection(EM_SELECTION_INCREASE_MODE_DEFAULT, vLeftTopRightBottom, matView, matProj, matViewport, EM_RAY_INTERSECTION_MAX_RANGE* 2);
//
//	TypeEntityAccessProxy& proxy = this->GetSelectionTool().GetSelectionAccessProxy();
//	for(proxy.Begin(); ! proxy.IsEnd(); proxy.StepForward())
//	{
//		KG3DSceneEntity Entity = proxy.GetCur();
//		pimodel = static_cast<IKG3DModel *>(Entity.m_pPointer);
//		pList.push_back(pimodel);
//
//		//pse++;
//	}
//
//	return S_OK;
//}

HRESULT KG3DSceneSceneEditor::GetFirstSelectedModel(IEKG3DModel*& piModel)
{
	KG3DModel *pModel = NULL;
	if(/*m_listSelectedEntity.size()==0*/  0 == this->GetSelectionTool().GetSelectionCount())
		return E_FAIL;

	/*KG3DSceneEntityList::iterator i = m_listSelectedEntity .begin();
	while(i!=m_listSelectedEntity.end())
	{*/
	TypeEntityAccessProxy& proxy = this->GetSelectionTool().GetSelectionAccessProxy();
	for(proxy.Begin(); ! proxy.IsEnd(); proxy.StepForward())
	{
		KG3DSceneEntity Entity = proxy.GetCur();
		if(Entity.GetType()==SCENEENTITY_OBJECT)
		{
			KG3DRepresentObject *pObject = (KG3DRepresentObject*) Entity.m_pPointer;
			if(SUCCEEDED(pObject->GetModel(pModel,0,FALSE)))
			{
				piModel = pModel;
				return S_OK;
			}
		}
		//i++;
	}
	return E_FAIL;
}

HRESULT KG3DSceneSceneEditor::GetFloor(const D3DXVECTOR3* pvInput, FLOAT* pfRet)
{
	static BOOL bNew = 1;
	if (bNew)
	{
		//if(s_uFPS > 30)
		{
			if(NULL == pvInput || NULL == pfRet)
				return E_FAIL;

			DWORD dwMask = EM_RAY_INTERSECTION_MASK_TERRAIN | EM_RAY_INTERSECTION_MASK_OBJECT;
			::RayIntersectionVertical(*this, *pvInput, pfRet, NULL, dwMask);

			return S_OK;
		}

	}
	else
	{
		const D3DXVECTOR3* pVecIn = pvInput;
		D3DXVECTOR3  vOut;
		D3DXVECTOR3* pVecOut = &vOut;

		HRESULT hr = E_FAIL;
		list<KG3DEntityObjectShell*>::iterator i;
		list<KG3DEntityObjectShell*>::iterator iend;
		list<KG3DEntityObjectShell*>listShell;

		D3DXVECTOR3 vecGround;
		D3DXVECTOR2 vecA;
		D3DXVECTOR2 vecC; 

		int nHavePosFlag = false;

		KG_PROCESS_ERROR(pVecOut);
		KG_PROCESS_ERROR(pVecIn);

		*pVecOut   = *pVecIn;
		pVecOut->y = static_cast<FLOAT>(INT_MIN);

		vecGround.x = pVecIn->x;
		vecGround.y = m_lpSceneOutDoorSpaceMgr->GetGroundHeight(pVecIn->x, pVecIn->z);
		vecGround.z = pVecIn->z;

		if (vecGround.y <= pVecIn->y && vecGround.y > pVecOut->y)
		{
			pVecOut->y   = vecGround.y;
			nHavePosFlag = true;
		}

		vecA = D3DXVECTOR2(pVecIn->x, pVecIn->z);

		hr = m_lpSceneOutDoorSpaceMgr->GetObjectShellIntersectPoint2D(&listShell,vecA);
		KGLOG_COM_PROCESS_ERROR(hr);

		i = listShell.begin();
		iend = listShell.end();
		while (i!=iend)
		{
			D3DXVECTOR3 vecInter[128];
			DWORD dwCount = 0x0;

			KG3DEntityObjectShell* pShell = *i;
			KG3DRepresentObject* pObject = pShell->m_lpObject;

			if(pObject  /*&&!pObject->m_bSelected*/)//没有检查到Selected的任何用处
		 {
			 if (pObject->GetRayIntersect(dwCount,vecInter,128,*pVecIn,D3DXVECTOR3(0.0f, 1.0f, 0.0f)))
			 {
				 for (DWORD s = 0; s < dwCount; s++)
				 {
					 if (vecInter[s].y <= pVecIn->y && vecInter[s].y >= pVecOut->y)
					 {
						 pVecOut->y = vecInter[s].y;
						 nHavePosFlag = true;
					 }
				 }
			 }
		 }

			i++;
		}

		if (!nHavePosFlag)
			pVecOut->y = vecGround.y;

		if(NULL != pfRet)
			*pfRet = pVecOut->y;

		hr = S_OK;
		return hr;
Exit0:
		if (!pVecOut || !pVecIn)
			hr = E_POINTER;
		return hr;
	}
}


HRESULT KG3DSceneSceneEditor::GetWaterHeight(D3DXVECTOR3* pReturn, D3DXVECTOR3* pIn)
{
	HRESULT hResult  = E_FAIL;
    HRESULT hRetCode = E_FAIL;

	D3DXVECTOR3 vTerrain = *pIn;
	KG_PROCESS_ERROR(pReturn);

	*pReturn = *pIn;

	hRetCode = m_lpSceneOutDoorSpaceMgr->GetWaterHeight(pReturn->y, pIn->x, pIn->z);
    KG_COM_PROCESS_ERROR(hRetCode);

	hResult = S_OK;
Exit0 :
	return hResult;
}

HRESULT KG3DSceneSceneEditor::GetHeight(D3DXVECTOR3 *pOut,D3DXVECTOR3 *pIn)


{
	HRESULT hr = E_FAIL;

	_ASSERT(pOut);
	_ASSERT(pIn);

	*pOut = *pIn;

	pOut->y = m_lpSceneOutDoorSpaceMgr->GetGroundHeight(pIn->x,pIn->z);

	hr = S_OK;
//Exit0:
	return hr;
}

 float KG3DSceneSceneEditor::GetHeight(float x, float z)
 {
    return m_lpSceneOutDoorSpaceMgr->GetGroundHeight(x,z);
 }

typedef struct tagPointLogic
{
	D3DXVECTOR3		vecPoint;
	BOOL			bFaceUp;
} LogicalPoint;

 /*HRESULT KG3DSceneSceneEditor::GetStandHeight(DWORD& dwCount,D3DXVECTOR3 *pInter,D3DXVECTOR3& vIn,DWORD dwMaxCount)
 {
	 HRESULT hr = E_FAIL;
     list<KG3DEntityObjectShell*>::iterator i;
     list<KG3DEntityObjectShell*>::iterator iend;

	 dwCount = 0;
	 D3DXVECTOR3 Out(vIn.x,0,vIn.z);
	 Out.y = m_lpSceneOutDoorSpaceMgr->GetGroundHeight(vIn.x,vIn.z);

	 dwCount++;
	 pInter[0] = Out;

	 list<KG3DEntityObjectShell*>listShell;

	 D3DXVECTOR2 A(vIn.x,vIn.z);

	 hr = m_lpSceneOutDoorSpaceMgr->GetObjectShellIntersectPoint2D(&listShell,A);
	 KGLOG_COM_PROCESS_ERROR(hr);

	 i = listShell.begin();
	 iend = listShell.end();
	 while (i!=iend)
	 {
		 D3DXVECTOR3 vecInter[128];
		 KG3DEntityObjectShell* pShell = *i;
		 KG3DRepresentObject* pObject = pShell->m_lpObject;

		 if(pObject)
		 {

			 DWORD dwEntityCount = 0;
			 D3DXVECTOR3 vEntityInter[128];

			 if(pObject->GetRayIntersect(dwEntityCount,vEntityInter,128,vIn,D3DXVECTOR3(0,1,0)))
			 {
				 for(DWORD s=0;s<dwEntityCount;s++)
				 {
					 if(s+dwCount<dwMaxCount)
					 {
						 pInter[s+dwCount] = vEntityInter[s];
					 }
					 else
						 break;
				 }

				 dwCount += dwEntityCount;
				 if(dwCount >= dwMaxCount)
				 {
					 dwCount = dwMaxCount;
				 }
			 }
		 }

		 i++;
	 }
	 return S_OK;
Exit0:
	return E_FAIL;
}*/
extern const KG3DLogicalConstances& g_GetLogicalConstants();
HRESULT KG3DSceneSceneEditor::LogicalToScene(D3DXVECTOR3 *pPos, float fX, float fY, float fZ, BOOL bOnFloor)
{
	HRESULT hr = E_FAIL;
	D3DXVECTOR3 Height = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	_ASSERT(m_lpSceneOutDoorSpaceMgr);

	const KG3DLogicalConstances& lgC = g_GetLogicalConstants();
	_ASSERT(lgC.cellLength > 0);

	
	KGLOG_PROCESS_ERROR(pPos);

	/*pPos->x = fX * LOGICAL_CELL_CM_LENGTH / CELL_LENGTH + m_nLogicalSceneX_Start * 100;
	pPos->z = fY * LOGICAL_CELL_CM_LENGTH / CELL_LENGTH + m_nLogicalSceneZ_Start * 100;
	pPos->y = fZ / POINT_PER_ALTITUDE * ALTITUDE_UNIT + TERRAIN_MIN_HEIGHT;*/
	pPos->x = fX *  lgC.logicalCellCmLength/ lgC.cellLength + m_nLogicalSceneX_Start * 100;
	pPos->z = fY * lgC.logicalCellCmLength / lgC.cellLength + m_nLogicalSceneZ_Start * 100;
	pPos->y = fZ / lgC.pointPerAltitude * lgC.altitudeUnit + lgC.terrainMinHeight;


	if (bOnFloor)
	{
		//if(!m_lpFloorCache->GetFloorHeight(pPos,YY))
		{
			Height = *pPos;
			//hr = GetFloor(&Height, pPos);
			hr = GetFloor(pPos, &Height.y);
			KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);
			pPos->y = Height.y;


			//m_lpFloorCache->InPutData(pPos,YY);
		}
	}

	hr = S_OK;
Exit0:
	return hr;
}

HRESULT KG3DSceneSceneEditor::SceneToLogical(int *pX, int *pY, int *pZ, D3DXVECTOR3 Pos)
{
	HRESULT hr = E_FAIL;
	D3DXVECTOR3 Height;
	
	const KG3DLogicalConstances& lgC = g_GetLogicalConstants();
	_ASSERTE(0 != lgC.logicalCellCmLength && _T("用到逻辑的话，逻辑的结构体必须初始化，不然什么Bug都有"));
	hr = GetHeight(&Height, &Pos);
	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

	/*if (pX)
		*pX = (int)((Pos.x - m_nLogicalSceneX_Start*100) * CELL_LENGTH / LOGICAL_CELL_CM_LENGTH);
	if (pY)
		*pY = (int)((Pos.z - m_nLogicalSceneZ_Start*100) * CELL_LENGTH / LOGICAL_CELL_CM_LENGTH);
	if (pZ)
		*pZ = (int)((Pos.y - TERRAIN_MIN_HEIGHT) * POINT_PER_ALTITUDE / ALTITUDE_UNIT);*/

	if (pX)
		*pX = (int)((Pos.x - m_nLogicalSceneX_Start*100) * lgC.cellLength / lgC.logicalCellCmLength);
	if (pY)
		*pY = (int)((Pos.z - m_nLogicalSceneZ_Start*100) * lgC.cellLength / lgC.logicalCellCmLength);
	if (pZ)
		*pZ = (int)((Pos.y - lgC.terrainMinHeight) * lgC.pointPerAltitude / lgC.altitudeUnit);

	hr = S_OK;

Exit0:
	return hr;
}


/*
HRESULT KG3DSceneSceneEditor::AddRenderEntity(IKG3DResourceBase *pEntity, 
unsigned uClass, 
BOOL bSelectable/ *= TRUE * /)
{

//暂时禁止掉这个判断
if(!g_bEnableAddEntity)
return S_OK;

KG3DModel *pModel = static_cast<KG3DModel*>(pEntity);
if(!pModel)
return E_FAIL;


/*if(g_eEngineOption.bUseProgressMeshLOD)

{
pModel->CreateProgressMesh();
}

if(m_lpPhysicsScene)
{
m_lpPhysicsScene->InputModel(pModel,TRUE);
}

KG3DSceneEntity Entity(SCENEENTITY_MODEL,pModel);
m_lpSceneOutDoorSpaceMgr->InputModel(pModel);

if (pModel->m_nIsSockWave)
m_listPostRenderEntity.push_back(Entity);
else
m_listEntityRenderLast.push_back(Entity);
	KG3DSceneEntity Entity(SCENEENTITY_MODEL,pModel);
	m_lpSceneOutDoorSpaceMgr->InputModel(pModel);

if (bSelectable)
{
m_listSelectableEntity.push_back(Entity);
}

pModel->m_bSelectable = bSelectable;


#if 0
if (pModel->m_scName.find("M2_1001.mdl") != 0xffffffff)
{
TCHAR szSFXPath[MAX_PATH];
KG3DSFX* pSFX = NULL;
g_cModelTable.LoadResourceFromFile(
"H:\\sWordIII\\sword3-products\\trunk\\client\\data\\陷阱火A001.Sfx", 0, MLO_MULTI, (IKG3DResourceBase**)&pSFX
);
pSFX->BindToBone(pModel, "");

}
#endif

return S_OK;
}*/

/*

/*
HRESULT KG3DSceneSceneEditor::RemoveRenderEntity(IKG3DResourceBase *pEntity, 
unsigned uClass,
BOOL bSelectable/ *= TRUE * /)
{
KG3DModel *pModel = static_cast<KG3DModel*>(pEntity);
if(!pModel)
return E_FAIL;

if (g_cRecorder.IsRecord() && m_dwRecordID != 0 && pModel->m_dwRecordID != 0)
{
KG3DRecorder::Parameters MyParameters;
MyParameters.nNum = 3;
MyParameters.size_tDataSize = sizeof(DWORD) + sizeof(unsigned) + sizeof(BOOL);
char* pData = new char[MyParameters.size_tDataSize];
MyParameters.pData = pData;
MyParameters.nFrom[0] = 0;
*(DWORD*)(&pData[MyParameters.nFrom[0]]) = pModel->m_dwRecordID;
MyParameters.nFrom[1] = sizeof(DWORD);
*(unsigned*)(&pData[MyParameters.nFrom[1]]) = uClass;
MyParameters.nFrom[2] = sizeof(DWORD) + sizeof(unsigned);
*(BOOL*)(&pData[MyParameters.nFrom[2]]) = bSelectable;
g_cRecorder.RecordEvent(m_dwRecordID,FT_KG3DSCENESCENEEDITOR_REMOVERENDERENTITY,
GetCurrentTime(),MyParameters);
}

if(m_lpPhysicsScene)
{
m_lpPhysicsScene->RemoveModel(pModel,TRUE);
}
KG3DSceneEntity Entity(SCENEENTITY_MODEL,pModel);


m_lpSceneOutDoorSpaceMgr->RemoveModel(pModel);

m_EntityMgr.RemoveRenderModel(pModel);

if (pModel->m_nIsSockWave)
m_listPostRenderEntity.remove(Entity);
else
m_listEntityRenderLast.remove(Entity);

if (bSelectable)
m_listSelectableEntity.remove(Entity);
pModel->m_bSelectable = bSelectable;

return S_OK;
}
*/

//HRESULT KG3DSceneSceneEditor::GroupSelectEntity()
//{
//	HRESULT hr = E_FAIL;
//	KG3DSceneEntityGroup *pNewGroup = NULL;
//	KG3DSceneEntityList::iterator i;
//
//	if(m_listSelectedEntity.size()<2)
//		return E_FAIL;
//
//	hr = g_cSceneEntityGroupTable.Get1NewResource(&pNewGroup,0,0);
//	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);
//
//	i = m_listSelectedEntity.begin();
//	while(i!=m_listSelectedEntity.end())
//	{
//		KG3DSceneEntity &Entity = *i;
//
//		pNewGroup->AddEntity(Entity);
//
//		m_listSelectableEntity.remove(Entity);
//
//		i++;
//	}
//
//	m_listSelectedEntity.clear();
//
//	m_listEntityGroup.push_back(pNewGroup);
//
//	{
//		KG3DSceneEntity NewEntity(SCENEENTITY_GROUP,pNewGroup);
//		m_listSelectableEntity.push_back(NewEntity);
//	}
//
//	return S_OK;
//Exit0:
//	return E_FAIL;
//}

//HRESULT KG3DSceneSceneEditor::UnGroupSelectEntity()//取消打包
//{
//	KG3DSceneEntityList::iterator i = m_listSelectedEntity.begin();
//	while(i!=m_listSelectedEntity.end())
//	{
//		KG3DSceneEntity &Entity = *i;
//		if(Entity.GetType()==SCENEENTITY_GROUP)
//		{
//			KG3DSceneEntityGroup *pGroup = static_cast<KG3DSceneEntityGroup*>(Entity.m_pPointer);
//			if(pGroup)
//			{
//				KG3DSceneEntityList::iterator j = pGroup->m_listEntity.begin();
//				while (j!=pGroup->m_listEntity.end())
//				{
//					KG3DSceneEntity Entity = *j;
//					m_listSelectableEntity.push_back(Entity);
//					j++;
//				}
//			}
//			m_listSelectableEntity.remove(Entity);
//			m_listEntityGroup.remove(pGroup);
//			SAFE_RELEASE(pGroup);
//		}
//		i++;
//	}
//	m_listSelectedEntity.clear();
//	return S_OK;
//}

BOOL KG3DSceneSceneEditor::IsSelectedPvs()
{
    TypeEntityAccessProxy& proxy = this->GetSelectionTool().GetSelectionAccessProxy();

    for(proxy.Begin(); ! proxy.IsEnd(); proxy.StepForward())
    {
        KG3DSceneEntity Entity = proxy.GetCur();

        if(Entity.GetType() == SCENEENTITY_OBJECT)
        {
            KG3DRepresentObject *pObject = static_cast<KG3DRepresentObject*>(Entity.m_pPointer);

            if(pObject->GetType() == REPRESENTOBJECT_PVS)
            {	
                return TRUE;
            }
        }
    }

    return FALSE;

}

HRESULT KG3DSceneSceneEditor::UnitSelectEntity()
{
	HRESULT hr = E_FAIL;
	KG3DRepresentObject *pNewObject = NULL;
	DWORD ModelIndex = 0;
	D3DXVECTOR3 vSum(0,0,0);

	if( this->GetSelectionTool().GetSelectionCount() < 2)
		return E_FAIL;
	///////////////////////////////////////////////////////////////////////////
	//先得到中心位置
	//还要先保存成待释放列表。因为下面要释放，如果释放之后再ClearSelection，会出错，原因是外层已经删除掉东西了，而Selection里面还保留着引用，又没有引用计数。

	std::vector<KG3DSceneEntity> vecEntitiesToUninit;
	{
		size_t uSelCount = static_cast<size_t>(this->GetSelectionTool().GetSelectionCount());
		_ASSERTE(uSelCount < 10000 && _T("如果选择了1万个物件，那么选择器出Bug了"));
		vecEntitiesToUninit.reserve(uSelCount);//预分配内存，能大大加速vector的push_back的速度
	}

	TypeEntityAccessProxy& proxy = this->GetSelectionTool().GetSelectionAccessProxy();
	for(proxy.Begin(); ! proxy.IsEnd(); proxy.StepForward())
	{
		KG3DSceneEntity entityRef = proxy.GetCur();
		D3DXVECTOR3 vPos;
		entityRef.GetTranslation(&vPos);
		vSum += vPos;

		vecEntitiesToUninit.push_back(entityRef);
	}

	D3DXVECTOR3 vNewObjectPos = vSum / (float)this->GetSelectionTool().GetSelectionCount();
	ASSERT_FLOAT_IS_VALIED(vNewObjectPos.x);
	{
		hr = g_GetObjectTable().Get1NewResource(&pNewObject,0,0);
		KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

		pNewObject->SetTranslation(&vNewObjectPos);
	}

	//上面保存了列表，下面就可以清空选择了
	this->GetSelectionTool().ClearSelection();
	
	for(size_t i = 0, uVecCount = vecEntitiesToUninit.size(); i < uVecCount; ++i)
	{
		KG3DSceneEntity entityTemp = vecEntitiesToUninit[i];

		if(entityTemp.GetType() == SCENEENTITY_OBJECT)
		{
			KG3DRepresentObject *pObject = static_cast<KG3DRepresentObject*>(entityTemp.m_pPointer);

			BOOL bFromSet = FALSE;
			TCHAR strSetName[MAX_PATH];
			int  nIndex = 0;
			if(pObject->GetType() == REPRESENTOBJECT_SOUNDBALL)
			{	
				//i++;
				continue;
			}

			if(pObject->GetType()==REPRESENTOBJECT_SET)
			{
				KG3DRepresentObjectSet *pSet = static_cast<KG3DRepresentObjectSet*>(pObject);
				if(pSet)
				{
					bFromSet = TRUE;
					pSet->GetModelSetIniFileName(strSetName);
					nIndex = pSet->GetCurrentModel();
				}
			}
			D3DXVECTOR3    vObjTrans(0,0,0);
			D3DXQUATERNION vObjRot(0,0,0,1);
			D3DXVECTOR3    vObjScale(1,1,1);
			D3DXMATRIX     vmat = pObject->m_Matrix;

			pObject->GetTranslation(&vObjTrans);
			pObject->GetScaling(&vObjScale);
			pObject->GetRotation(&vObjRot);

			for(DWORD i=0;i<pObject->GetModelCount();i++)
			{
				TCHAR MeshName[MAX_PATH];
				TCHAR MtlName[MAX_PATH];
				TCHAR AniName[MAX_PATH];

				KG3DModel *pModel = NULL;
				KG3DModel *pNewModel = NULL;
				KG3DRepresentObject::ModelInformation *pModelInfo = NULL;

				pModelInfo = pObject->GetModelInformation(i);
				pObject->GetModel(pModel,i,FALSE);
				pObject->GetModelFileName(i,MeshName,MtlName,AniName);

				pNewObject->SetModel(ModelIndex,MeshName,MtlName,AniName,TRUE);
				pNewObject->GetModel(pNewModel,ModelIndex,FALSE);

				//////////////////////////////////////////////////////////////////////////
				D3DXVECTOR3    vModelTrans;
				D3DXVECTOR3    vModelScale;
				D3DXQUATERNION  vModelRot;

				pModel->GetTranslation(&vModelTrans);
				pModel->GetScaling(&vModelScale);
				pModel->GetRotation(&vModelRot);

				D3DXVECTOR3 vTrans;
				D3DXVec3TransformCoord(&vTrans,&vModelTrans,&vmat);
				D3DXVECTOR3 vScale;
				vScale.x = vObjScale.x * vModelScale.x;
				vScale.y = vObjScale.y * vModelScale.y;
				vScale.z = vObjScale.z * vModelScale.z;
				D3DXQUATERNION vRot;
				vRot = vModelRot * vObjRot;
				D3DXQuaternionNormalize(&vRot,&vRot);
				//////////////////////////////////////////////////////////////////////////
			/*	pNewModel->SetTranslation(&(vTrans-vNewObjectPos));
				pNewModel->SetScaling(&vScale);
				pNewModel->SetRotation(&vRot);
				pNewModel->UpdateTransformation();
				pNewModel->ComputeBBox();*/
				//合并的时候，把低模的位置信息也更新
				vTrans -= vNewObjectPos;
				pNewObject->SetModelTranslation(ModelIndex,vTrans);
				pNewObject->SetModelScaling(ModelIndex,vScale);
				pNewObject->SetModelRotation(ModelIndex,vRot);
				pNewObject->UpdateModelTransformation(ModelIndex);
				pNewObject->ComputeModelBBox(ModelIndex);

				//////////////////////////////////////////////////////////////////////////
				if(pModelInfo)
				{
					pNewObject->SetModelInformation(ModelIndex,pModelInfo);
				}
				if(bFromSet)
				{
					KG3DRepresentObject::ModelInformation ModelInfo;
					ModelInfo.nIndex = nIndex;
					ModelInfo.nTypeSource = 1;
					ModelInfo.szSource = strSetName;
					pNewObject->SetModelInformation(ModelIndex,&ModelInfo);
				}

				ModelIndex++;
			}


			m_lpSceneOutDoorSpaceMgr->RemoveRepresentObject(pObject);
			this->RemoveRenderEntity(entityTemp, FALSE);//因为现在在SelectedList中检索，所以把第二参数置为FALSE
		}
	}	

	pNewObject->FrameMove();//<SLEEP_REFRACTOR>
	pNewObject->ComputeBBox();

	if (pNewObject->GetModelCount()<=0)
		SAFE_RELEASE(pNewObject);

	if(pNewObject)
	{
		DWORD uOption = EM_SCENE_ADD_OPTION_SELECTABLE | EM_SCENE_ADD_OPTION_SELECTED | EM_SCENE_ADD_OPTION_TO_FILE;
		AddRepresentObject(pNewObject, uOption, NULL);
		if(uOption & EM_SCENE_ADD_OPTION_TO_FILE) //如果是ToFile，会New一个新的Object.不释放掉会造成内存泄漏
			SAFE_RELEASE(pNewObject);
	}


	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneSceneEditor::UnUnitObject(KG3DRepresentObject *pObject,KG3DSceneEntityList *plistTemp)
{
	HRESULT hr = S_OK;
	D3DXVECTOR3    vObjTrans;
	D3DXVECTOR3    vObjScale;
	D3DXQUATERNION  vObjRot;
	D3DXMATRIX     vmat;
	DWORD dwNumModel = 0;

	KG_PROCESS_ERROR(NULL != pObject);
	dwNumModel = pObject->GetModelCount();

	pObject->GetTranslation(&vObjTrans);
	pObject->GetScaling(&vObjScale);
	pObject->GetRotation(&vObjRot);
	vmat = pObject->m_Matrix;

	for(DWORD i=0;i<dwNumModel;i++)
	{
		KG3DModel *pModel = NULL;
		KG3DRepresentObject::ModelInformation *pModelInfo = pObject->GetModelInformation(i);
		hr = pObject->GetModel(pModel,i,FALSE);
		KG_COM_PROCESS_ERROR(hr);

		if(NULL == pModel)
			continue;

		TCHAR MeshName[MAX_PATH];
		TCHAR MtlName[MAX_PATH];
		TCHAR AniName[MAX_PATH];

		D3DXVECTOR3    vModelTrans;
		D3DXVECTOR3    vModelScale;
		D3DXQUATERNION  vModelRot;
		//////////////////////////////////////////////////////////////////////////
		hr = pObject->GetModelFileName(i,MeshName,MtlName,AniName);
		KGLOG_COM_PROCESS_ERROR(hr);

		pModel->GetTranslation(&vModelTrans);
		pModel->GetScaling(&vModelScale);
		pModel->GetRotation(&vModelRot);
		//////////////////////////////////////////////////////////////////////////
		KG3DRepresentObject *pNewObject = NULL;

		if ((pModelInfo)&&(pModelInfo->nTypeSource==1))
		{
			hr = g_GetObjectTable().NewOneResourse(REPRESENTOBJECT_SET,(IKG3DResourceBase**)&pNewObject);
			KGLOG_COM_PROCESS_ERROR(hr);

			KG3DRepresentObjectSet *pSet = static_cast<KG3DRepresentObjectSet*>(pNewObject);
			if(pSet)
			{
				pSet->LoadFromIniFile((LPSTR)pModelInfo->szSource.c_str(), 0);
				pSet->SetCurrentModel(pModelInfo->nIndex,FALSE);
			}
		}
		else
		{
			hr = g_GetObjectTable().NewOneResourse(REPRESENTOBJECT_DEFAULT,(IKG3DResourceBase**)&pNewObject);
			KGLOG_COM_PROCESS_ERROR(hr);
			hr = pNewObject->SetModel(0,MeshName,MtlName,AniName,TRUE);
			if(FAILED(hr))
			{
				SAFE_RELEASE(pNewObject);
				goto Exit0;
			}
		}

		D3DXVECTOR3 vTrans;
		D3DXVec3TransformCoord(&vTrans,&vModelTrans,&vmat);
		D3DXVECTOR3 vScale;
		vScale.x = vObjScale.x * vModelScale.x;
		vScale.y = vObjScale.y * vModelScale.y;
		vScale.z = vObjScale.z * vModelScale.z;
		D3DXQUATERNION vRot;
		vRot = vModelRot * vObjRot;
		D3DXQuaternionNormalize(&vRot,&vRot);

		pNewObject->SetTranslation(&vTrans);
		pNewObject->SetScaling(&vScale);
		pNewObject->SetRotation(&vRot);
		pNewObject->UpdateTransformation();
		pNewObject->ComputeBBox();

		//this->AddRepresentObject(pNewObject, TRUE, FALSE,TRUE);

		_KG3D_DEBUG_SUPPERSUB_POINTER_EQUAL(IEKG3DRepresentObject, KG3DRepresentObject);//下面有强转，这里检查一下

		KG3DRepresentObject* pObjectToFileCreated = NULL;
		DWORD uOption = EM_SCENE_ADD_OPTION_SELECTABLE|EM_SCENE_ADD_OPTION_TO_FILE;
		this->AddRepresentObject(pNewObject,uOption , (IEKG3DRepresentObject**)&pObjectToFileCreated);
		if (NULL != pObjectToFileCreated)//如果ToFile，内部会创建一个新的Object
		{
			plistTemp->push_back(KG3DSceneEntity(SCENEENTITY_OBJECT, pObjectToFileCreated));
		}
		if(uOption & EM_SCENE_ADD_OPTION_TO_FILE)//如果是ToFile，会New一个新的Object.不释放掉会造成内存泄漏
			SAFE_RELEASE(pNewObject);
	}
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneSceneEditor::UnUnitSelectEntity()//解散组合的模型
{
	HRESULT hr = E_FAIL;
	KG3DSceneEntityList listTempEntity;

	if(this->GetSelectionTool().GetSelectionCount() < 1)
		return E_FAIL;
	///////////////////////////////////////////////////////////////////////////

	std::vector<KG3DSceneEntity> vecEntities;	//要释放之前，先保存起来，清理了选择列表再做外层释放，不然会出错

	vecEntities.reserve((size_t)(this->GetSelectionTool().GetSelectionCount()));

	TypeEntityAccessProxy& proxy = this->GetSelectionTool().GetSelectionAccessProxy();
	for(proxy.Begin(); ! proxy.IsEnd(); proxy.StepForward())
	{
		KG3DSceneEntity entityRef = proxy.GetCur();
		vecEntities.push_back(entityRef);
	}

	this->GetSelectionTool().ClearSelection();

	for (size_t i = 0; i < vecEntities.size(); ++i)
	{
		KG3DSceneEntity entityRef = vecEntities[i];
		if(entityRef.GetType() == SCENEENTITY_OBJECT)
		{
			KG3DRepresentObject *pObject = static_cast<KG3DRepresentObject*>(entityRef.m_pPointer);

			DWORD dwNumModel = pObject->GetModelCount();
			if(dwNumModel>=2)
			{
				hr = UnUnitObject(pObject,&listTempEntity);
				KGLOG_COM_PROCESS_ERROR(hr);


				m_lpSceneOutDoorSpaceMgr->RemoveRepresentObject(pObject);
				RemoveRenderEntity(entityRef, FALSE);
			}
		}
	}

	KG3DSceneSelectionTool& selTool = this->GetSelectionTool();
	for (KG3DSceneEntityList::iterator it = listTempEntity.begin(); it != listTempEntity.end(); ++it)
	{
		KG3DSceneEntity& entityRef = *it;
		selTool.SetSelected(entityRef, TRUE);
	}

	return S_OK;
Exit0:
	return E_FAIL;
}


HRESULT KG3DSceneSceneEditor::AddRenderEntity(IKG3DResourceBase *pEntity, 
											  unsigned uClass, 
											  BOOL bSelectable,
											  BOOL bLight)
{
    HRESULT hrResult = E_FAIL;
    HRESULT hrRetCode = E_FAIL;
    KG3DModel *pModel = NULL;
    KG3DSceneEntity Entity(SCENEENTITY_MODEL, pEntity);
    int nEnableHideFromRenderToolsFlag = false;
    int nSetPhysiceSceneFlag = false;
    int nOutDoorSpaceMgrInputModelFlag = false;

    KGLOG_PROCESS_ERROR(pEntity);    
    KG_PROCESS_ERROR(g_bEnableAddEntity);

	pModel = dynamic_cast<KG3DModel*>(pEntity);
    KGLOG_PROCESS_ERROR(pModel);

#ifdef _DEBUG
    {
        KG3DSceneEntityList::iterator itFound;
        itFound = std::find(m_listEntityRenderLast.begin(), m_listEntityRenderLast.end(), Entity);
        if (itFound != m_listEntityRenderLast.end())
        {
            KGLogPrintf(KGLOG_ERR, "call AddRenderEntity more then one time:%s", pModel->m_scName.c_str());
            KG_PROCESS_ERROR(false);
        }
    }
#endif

	if (g_cRecorder.IsRecord() && m_dwRecordID != 0 && pModel->m_dwRecordID != 0 && !g_cRecorder.IsLock())
	{
		KG3DRecorder::Parameters MyParameters;
		MyParameters.nNum = 3;
		MyParameters.size_tDataSize = sizeof(DWORD) + sizeof(unsigned) + sizeof(BOOL);
		char* pData = new char[MyParameters.size_tDataSize];
		MyParameters.pData = pData;
		MyParameters.nFrom[0] = 0;
		*(DWORD*)(&pData[MyParameters.nFrom[0]]) = pModel->m_dwRecordID;
		MyParameters.nFrom[1] = sizeof(DWORD);
		*(unsigned*)(&pData[MyParameters.nFrom[1]]) = uClass;
		MyParameters.nFrom[2] = sizeof(DWORD) + sizeof(unsigned);
		*(BOOL*)(&pData[MyParameters.nFrom[2]]) = bSelectable;
		g_cRecorder.RecordEvent(m_dwRecordID,FT_KG3DSCENEEDITOR_ADDRENDERENTITY,
			GetCurrentTime(),MyParameters);
	}

    hrRetCode = pModel->EnableHideFromRenderTools(FALSE);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);
    nEnableHideFromRenderToolsFlag = true;

	pModel->m_bSelectable = bSelectable;
	pModel->m_bCreateDynamicLight = bLight;
    pModel->SetClientAdd(TRUE);
	pModel->SetPhysicsScene(m_lpPhysicsScene);
    nSetPhysiceSceneFlag = true;

	hrRetCode = m_lpSceneOutDoorSpaceMgr->InputModel(pModel);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);
    nOutDoorSpaceMgrInputModelFlag = true;

	/*if (pModel->m_nIsSockWave)
	{
		m_listPostRenderEntity.push_back(Entity);
		if (pModel->m_dwShockMode == SHOCK_WAVE_ADD)
			m_listEntityRenderLast.push_back(Entity);
	}
	else*/
		
	m_listEntityRenderLast.push_back(Entity);
    m_EntityMgr.m_bNeedReGetVisibleEntity = TRUE;

	if(bSelectable)
	{
		this->IEInsertEntitySelectable(SCENEENTITY_MODEL, pModel, TRUE);
	}

    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        if (nOutDoorSpaceMgrInputModelFlag)
        {
            m_lpSceneOutDoorSpaceMgr->RemoveModel(pModel);
            nOutDoorSpaceMgrInputModelFlag = false;
        }
        if (nSetPhysiceSceneFlag)
        {
            pModel->SetPhysicsScene(NULL);
            nSetPhysiceSceneFlag = false;
        }
        if (nEnableHideFromRenderToolsFlag)
        {
            pModel->EnableHideFromRenderTools(TRUE);
            nEnableHideFromRenderToolsFlag = false;
        }
    }
	return hrResult;
}

HRESULT KG3DSceneSceneEditor::RemoveRenderEntity(IKG3DResourceBase *pEntity, 
												 unsigned uClass,
												 BOOL bSelectable/*= TRUE */)
{
	KG3DModel *pModel = static_cast<KG3DModel*>(pEntity);
	if(!pModel)
		return E_FAIL;

	pModel->EnableHideFromRenderTools(TRUE);

	if (g_cRecorder.IsRecord() && m_dwRecordID != 0 && pModel->m_dwRecordID != 0 && !g_cRecorder.IsLock())
	{
		KG3DRecorder::Parameters MyParameters;
		MyParameters.nNum = 3;
		MyParameters.size_tDataSize = sizeof(DWORD) + sizeof(unsigned) + sizeof(BOOL);
		char* pData = new char[MyParameters.size_tDataSize];
		MyParameters.pData = pData;
		MyParameters.nFrom[0] = 0;
		*(DWORD*)(&pData[MyParameters.nFrom[0]]) = pModel->m_dwRecordID;
		MyParameters.nFrom[1] = sizeof(DWORD);
		*(unsigned*)(&pData[MyParameters.nFrom[1]]) = uClass;
		MyParameters.nFrom[2] = sizeof(DWORD) + sizeof(unsigned);
		*(BOOL*)(&pData[MyParameters.nFrom[2]]) = bSelectable;
		g_cRecorder.RecordEvent(m_dwRecordID,FT_KG3DSCENEEDITOR_REMOVERENDERENTITY,
			GetCurrentTime(),MyParameters);
	}

	pModel->SetPhysicsScene(NULL);
	KG3DSceneEntity Entity(SCENEENTITY_MODEL,pModel);

    m_lpSceneOutDoorSpaceMgr->RemoveModel(pModel);

	m_EntityMgr.RemoveRenderModel(pModel);

	pModel->EnableHideFromRenderTools(TRUE);

	/*if (pModel->m_nIsSockWave)
	{
		m_listPostRenderEntity.remove(Entity);
		if (pModel->m_dwShockMode == SHOCK_WAVE_ADD)
			m_listEntityRenderLast.remove(Entity);
	}
	else*/
		
	m_listEntityRenderLast.remove(Entity);

	/*if (bSelectable)
		RemoveSelectableEntity(Entity, TRUE);*/
	if(bSelectable)
	{
		//this->GetSelectionTool().SetSelectable(Entity, FALSE);//<Selectable Refactor>

		this->IEInsertEntitySelectable(SCENEENTITY_MODEL, pModel, FALSE);

		this->GetSelectionTool().SetSelected(Entity, FALSE);
	}

	m_EntityMgr.m_bNeedReGetVisibleEntity = TRUE;

	pModel->LeaveSceneRegion(TRUE);
    pModel->LeavePvs(TRUE);

	return S_OK;
}
HRESULT KG3DSceneSceneEditor::RemoveRepresentObject(IEKG3DRepresentObject* pObject)
{
	//如果是旧地形就调用RemoveRenderEntity，这个是统一的删除函数。
	//否则，要走一套新的流程，新的地形中没有Entity，一切以Object为准

	KG_PROCESS_ERROR(NULL != pObject);
	{
		KG3DRepresentObject* pReprensentObj = static_cast<KG3DRepresentObject*>(pObject);

		pReprensentObj->OnDeleteFromScene(this);//通知这个事件
		//pReprensentObj->SetScene(NULL);//<SetScene Refactor>

		KG3DSceneOutDoorSpaceMgr* p = GetOutDoorSpaceMgr();
		KG_PROCESS_ERROR(p != NULL);

		p->RemoveRepresentObject(pReprensentObj);		
			
		//不需要释放选择的部分，选择还没有写
	}
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneSceneEditor::RemoveRenderEntity( KG3DSceneEntity Entity, BOOL bEitherRemoveInSelectedList)
{
	KG3DRepresentObject* pObject = NULL;
	SCENEENTITYTYPE eType = Entity.GetType();
	if (eType == SCENEENTITY_OBJECT)
	{
		pObject = static_cast<KG3DRepresentObject*>(Entity.m_pPointer);

		_ASSERTE(NULL != pObject);
		/*if (pObject->IsSockWave())
			m_listPostRenderEntity.remove(Entity);*/

		pObject->SetPhysicsScene(NULL);
	}

	//杂七杂八的Entity全在这里释放了
	//RemoveSelectableEntity(Entity, bEitherRemoveInSelectedList);

	KG3DSceneSelectionTool& selTool = this->GetSelectionTool();
	
	//selTool.SetSelectable(Entity, FALSE);//<Selectable Refactor>
	this->InsertEntitySelectable(Entity, FALSE);
	if (bEitherRemoveInSelectedList)
	{
		selTool.SetSelected(Entity, FALSE);
	}

	m_lpSceneOutDoorSpaceMgr->RemoveRepresentObject(pObject);

	m_listRenderEntity.remove(Entity);

    if (!g_bClient)
    {
	    m_EntityMgrTopView.RemoveEntity(0,Entity);
    }
	m_EntityMgr.RemoveEntity(0,Entity);

	//m_listEntityNotInQTree.remove(Entity);
	m_listEntityRenderLast.remove(Entity);

	return S_OK;
}

HRESULT KG3DSceneSceneEditor::RemoveRenderEntity( SCENEENTITYTYPE eEntityType, PVOID pvEntityPointer )
{
	if(NULL == pvEntityPointer)//不能允许里面没有东西的Entity，否则每个函数都要判断一下，很多地方会当
		return E_FAIL;
	return RemoveRenderEntity(KG3DSceneEntity(eEntityType, pvEntityPointer));
}

HRESULT KG3DSceneSceneEditor::AddRepresentObject(IEKG3DRepresentObject* pObject, ULONG uOption, IEKG3DRepresentObject** ppObjectCreated)
{
	//如果是用这个函数添加的话，要负责生命周期管理
	HRESULT hr = E_FAIL;
	{
		_KG3D_DEBUG_SUPPERSUB_POINTER_EQUAL(IEKG3DRepresentObject, KG3DRepresentObject);

		KG3DRepresentObject* pRepresentObj = static_cast<KG3DRepresentObject*>(pObject);
		KG_PROCESS_ERROR(NULL != pRepresentObj);

		pRepresentObj->m_bSleep = FALSE;
		pRepresentObj->FrameMove();	//<SLEEP_REFRACTOR>
		pRepresentObj->ComputeBBox();
		//这句话不知道有没有必要，但是DirectPutInHandObject是有这句的 注意其位置，需要在m_lpScene后面
		//用处应该是更新位置，并放入PhysiscScene	

		//这里选择的部分还没有写

		//放入OutDoor中，析构由OutDoor处理
		KG3DSceneOutDoorSpaceMgr* p = GetOutDoorSpaceMgr();
		KG_PROCESS_ERROR(p != NULL);

		if(pRepresentObj->GetType() != REPRESENTOBJECT_NODE)
		{
			D3DXVECTOR2 vPos;
			D3DXVECTOR2 vDir(0,0);
			vPos.x = pRepresentObj->m_Translation.x;
			vPos.y = pRepresentObj->m_Translation.z;
			
			p->ProcessDynamicLoad(TRUE,vPos,vDir,1000,1000,1000,this);
		}

        /* do this to update obj's world matrix */
        pRepresentObj->m_bSleep = false;
        pRepresentObj->FrameMove();//<SLEEP_REFRACTOR>

		KG3DRepresentObject* pCreatedObject = NULL;
		BOOL bToFile = uOption & EM_SCENE_ADD_OPTION_TO_FILE;
	    hr = p->InputRepresentObject(pRepresentObj, TRUE , bToFile, &pCreatedObject);//如果ToFile，内部会复制一个Object的，所以要额外取得
		KG_COM_PROCESS_ERROR(hr);

#if defined(DEBUG) | defined(_DEBUG)
		if(bToFile)
		{
			_ASSERTE(NULL != pCreatedObject);
		}
#endif

		KG3DRepresentObject* pObjOperating = bToFile ? pCreatedObject : pRepresentObj;

		if (uOption & EM_SCENE_ADD_OPTION_SELECTABLE)
		{
			KG3DSceneSelectionTool& selTool = this->GetSelectionTool();
			selTool.SetSelectable(*pObjOperating, TRUE);
			if (uOption & EM_SCENE_ADD_OPTION_SELECTED)
			{
				selTool.SetSelected(*pObjOperating, TRUE);
			}
		}

        if (uOption & EM_SCENE_ADD_OPTION_TO_PHYSICS)
		{
			for (int i = 0; i < (int)pObjOperating->GetModelCount(); ++i)
			{
				KG3DModel* pModel = pObjOperating->GetModel(i);

				if (pModel)
					pModel->SetPhysicsScene(m_lpPhysicsScene);
			}
		}

		if (NULL != ppObjectCreated)
		{
			if (bToFile)
			{
				*ppObjectCreated = pCreatedObject;
			}
		}

		return S_OK;
	}
Exit0:
	return E_FAIL;
}



HRESULT KG3DSceneSceneEditor::AddRenderEntity( KG3DSceneEntity Entity, BOOL bIsSelectable/*TRUE*/, BOOL bIsSelected /*FALSE*/)
{
	//先取得属性
	BOOL bIsPostRender = FALSE;	

	SCENEENTITYTYPE eType = Entity.GetType();
	if (eType == SCENEENTITY_OBJECT)
	{	
		KG3DRepresentObject* pObj = (KG3DRepresentObject*)(Entity.m_pPointer);
		_ASSERTE(pObj != NULL);
		if (pObj->IsSockWave())
		{
			bIsPostRender = TRUE;
		}
	}

	   
	/*if(bIsPostRender)
	{
		m_listPostRenderEntity.push_back(Entity);
	}*/

#if defined(DEBUG) | defined(_DEBUG)	//Entity不应该有添加两次的情况，这里应huangshan要求，用来检查这个情况
	{
		KG3DSceneEntityList::iterator itFound = std::find(m_listRenderEntity.begin()
			, m_listRenderEntity.end(), Entity);
		if (itFound != m_listRenderEntity.end())
		{
			KGLogPrintf(KGLOG_ERR, _T("一个Entity被添加了两次，不应该这样"));
		}
	}
#endif
	m_listRenderEntity.push_back(Entity);

	if(bIsSelectable)
	{
		//SetSelectable(Entity, TRUE);//<Selectable Refactor>
		this->InsertEntitySelectable(Entity, TRUE);
		if (bIsSelected)
		{
			SetSelected(Entity, TRUE);
		}
	}


	return S_OK;
//Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneSceneEditor::AddRenderEntity( SCENEENTITYTYPE eEntityType, PVOID pvEntityPointer )
{
	if (pvEntityPointer == NULL)//不能允许里面没有东西的Entity，否则每个函数都要判断一下，很多地方会当
		return E_FAIL;
	return AddRenderEntity(KG3DSceneEntity(eEntityType, pvEntityPointer));
}

STDMETHODIMP KG3DSceneSceneEditor::DelRepresentObject( IEKG3DRepresentObject* pObject )
{
	return RemoveRepresentObject(pObject);
}

HRESULT KG3DSceneSceneEditor::DelRenderEntity( KG3DSceneEntity Entity, BOOL bEitherRemoveInSelectedList/*TRUE*/ )
{
	return RemoveRenderEntity(Entity, bEitherRemoveInSelectedList);
}


HRESULT KG3DSceneSceneEditor::FrameMove()
{
    KG3PROFILE_DURATION("Scene FrameMove");
	DWORD dwStartTime = timeGetTime();

	g_dwRenderCount++;


	if (0)
	{
		g_GetDefaultShaderGroup().LoadAllShaders(); //测试是否能够全部加载shader的函数
	}

	g_cGraphicsTool.SetCurScene(this);//后面很可能需要这句// Last modify by Chen Tianhong: 2008-1-18 11:05:54

	g_GetModelShadowRenderer().FrameMove();

	// Sky Box
	if (g_cEngineManager.GetFrameMoveOption(FRAMEMOVE_ENVIRONMENT))
		GetEnvironment().FrameMove();

	m_SceneWndManager->FrameMove();

	GetCommonObjectNode().FrameMove();

    FrameMoveCamera();

    _FrameMoveRenderEntifyLast();

    m_lpSceneOutDoorSpaceMgr->FrameMove(10);

    m_EntityMgr.FrameMove(this,TRUE);

    if (m_lpPhysicsScene && g_cEngineManager.GetFrameMoveOption(FRAMEMOVE_PHYSICS))
    {
        IKG3DCamera* pCamera = GetCurrentCamera();
        if (pCamera)
        {
            D3DXVECTOR3 vLookAtPos;
            D3DXVECTOR3 vCameraPos;
            pCamera->GetLookAtPosition(&vLookAtPos);
            pCamera->GetPosition(&vCameraPos);
            m_lpPhysicsScene->FrameMove((DWORD)g_cEngineManager.GetTimeControl().GetTinyTime(), vCameraPos, vLookAtPos);
        }
    }

    KG_PROCESS_SUCCESS(g_bClient);

    //////////////////////////////////////////////////////////////////////////
    // 以下是只与编辑器相关的代码
    //////////////////////////////////////////////////////////////////////////
	
    FrameMoveSelection(); // 刷新复制对象的笔刷 add by suntao

    // Npc And Camera FrameMove
    if ((!m_bEditing) && (m_pSFXSuede) && (m_SceneWndManager->GetCurOutputWnd()) && (!g_bClient))
        m_pSFXSuede->FrameMove();

	if (!g_bClient && !m_bEditing && m_pRepresentNPC && g_cEngineManager.GetFrameMoveOption(FRAMEMOVE_SKIN_MESH))
	{
		m_pRepresentNPC->FrameMove();
		D3DXVECTOR3 vLastPos = m_pRepresentNPC->GetTranslation();
		{
			list<KG3DRepresentNpcScreenRunner*>::iterator i = m_listRepresentNPC.begin();
			list<KG3DRepresentNpcScreenRunner*>::iterator iend = m_listRepresentNPC.end();
			while (i!=iend)
			{
				KG3DRepresentNpcScreenRunner* pNpc = *i;

				D3DXVECTOR3 vPos = pNpc->GetTranslation();
				D3DXVECTOR3 V = vPos - vLastPos;
				float K = D3DXVec3Length(&V);
				if(K>= 100)
				{
					vPos = vLastPos + V * 0.95F;
					//pNpc->SetTranslation(&vPos);
					pNpc->SetCurPosition(vPos,*this,1);

					D3DXVECTOR3 vDir = -V;
					vDir.y = 0;
					D3DXVec3Normalize(&vDir,&vDir);

					pNpc->SetDirection(vDir);
				}
				vLastPos = vPos;

				pNpc->FrameMove();
				i++;
			}
		}
	}

	if ((m_bEditing)&&(!g_bClient))
	{
		if(this->GetEditState()==SCENESTATE_TERRAIN)
		{
			if(m_lpSceneOutDoorSpaceMgr && m_bTerrainPainting)
			{
				g_cTerrainBlockModifier.OnBrush(m_lpSceneOutDoorSpaceMgr);
				if(g_cTerrainBlockModifier.GetTerrainState() == MAP_PAINT_GROUND && m_EntityMgr.m_lpTerrainRender)
					m_EntityMgr.m_lpTerrainRender->EnableSortTerrainBlock();
			}
		}
		else if(this->GetEditState()==SCENESTATE_CELLLOGICAL)
		{
			if(m_SceneWndManager->GetCurOutputWnd())
			{
				D3DXVECTOR3 Pos,Dir,Inter;

				static int nX = 0;
				static int nY = 0;

				POINT ptCursor;
				GetCursorPos( &ptCursor );

				if (ptCursor.x != nX || ptCursor.y != nY)
				{
					m_SceneWndManager->GetCurOutputWnd()->GetPickRay(&Pos,&Dir, NULL);

					FLOAT fDis = FLT_MAX;
					BOOL bFoundInter = ::RayIntersection(*this, Pos, Dir,&fDis, NULL, NULL);		 
					if(m_lpLogicEditor && bFoundInter)
					{
						Inter = Pos + Dir * fDis;	
						m_lpLogicEditor->SelectLogicalCells(m_nLogicalSceneX_Start,m_nLogicalSceneZ_Start,this, &Pos, &Dir,  Inter);
					}
					nX = ptCursor.x;
					nY = ptCursor.y;
				}
			}

			if (g_KeyState[VK_LBUTTON])
			{
				OnBrushCellLogical();
			}
		}
	}

	if(!g_bClient && this->GetEditState() == SCENESTATE_MOVE ||
		this->GetEditState() == SCENESTATE_ROTATE ||
		this->GetEditState() == SCENESTATE_SCALE)
	{
		/*for (KG3DSceneEntityList::iterator i = m_listSelectedEntity.begin();
		i != m_listSelectedEntity.end();
		i++)
		{*/

		TypeEntityAccessProxy& proxy = this->GetSelectionTool().GetSelectionAccessProxy();
		for(proxy.Begin(); ! proxy.IsEnd(); proxy.StepForward())
		{
			KG3DSceneEntity Entity = proxy.GetCur();
			Entity.FrameMove();
		}
	}

	GetMessageSender(KM_FRAMEMOVE).PostMessage(KG3DMessage(KM_FRAMEMOVE));

	if (!g_bClient && m_lpWaterEffect && g_cEngineManager.GetFrameMoveOption(FRAMEMOVE_SFX))
	{
		//m_lpWaterEffect->UpdataActive();

		if(m_pRepresentNPC /*&& g_cEngineOption.nWaterDetail >= WD_HIGH*/)
		{
			D3DXVECTOR3 vTrans;
			m_pRepresentNPC->GetTranslation(&vTrans);

			m_lpWaterEffect->CheckEntityAndWater(m_pRepresentNPC,vTrans,200,this);
		}
        else
            m_lpWaterEffect->DeleteAllSFX();
	}



	if (!g_bClient)
    {
	    m_EntityMgrTopView.FrameMove(this,FALSE);
    }

	if (!g_bClient && g_cEngineManager.GetSoundFocusScene() == this)
	{
		IKG3DCamera* pCamera = GetCurrentCamera();
		if (pCamera)
		{
			D3DXVECTOR3 vec3Pos, vec3Front, vec3Top(0.0f, 1.0f, 0.0f);
			D3DXVECTOR3 vec3Vel(0.f, 0.f, 0.f);
			pCamera->GetFront(&vec3Front);
			pCamera->GetPosition(&vec3Pos);
			IKG3DSoundShell *pSoundShell = g_cEngineManager.GetSound();
			if (pSoundShell)
			{
				vec3Pos *= 0.01f;
				pSoundShell->SetListenerPosition(&vec3Pos, &vec3Vel);
				pSoundShell->SetOrientation(&vec3Front, &vec3Top);
			}
		}
	}
Exit1:
    DWORD dwCost = timeGetTime() - dwStartTime;
    if(g_cEngineOption.bEnableTimeOptimizeLog && dwCost >= 20)
    {
        KGLogPrintf(KGLOG_ERR, "TimeOptimize %d KG3DSceneSceneEditor::FrameMove.",dwCost);
    }

    if(m_uOption & SCENE_DISABLE_DYNAMICLOAD)
    {
        Sleep(20);
    }

	return S_OK;
}

HRESULT KG3DSceneSceneEditor::FrameMoveSelection()
{	
	if (this->GetEditState() != SCENESTATE_COPY_REPRESENTOBJECT) // 清理掉复制的表现对象
	{
		if (m_vectorCopyRepresentObject.size() > 0)
		{
			this->GetSelectionTool().ClearSelection(); // 先清空选择的对象

			for (vector<KG3DRepresentObject*>::iterator it = m_vectorCopyRepresentObject.begin(); it != m_vectorCopyRepresentObject.end(); it++)
			{
				KG3DRepresentObject* pRepresentObject = *it;
				RemoveRepresentObject(pRepresentObject);
			}

			m_vectorCopyRepresentObject.clear(); // 清理掉复制的表现对象
		}		
	}
	else
	{
		KG_PROCESS_SUCCESS(m_vectorCopyRepresentObject.size() == 0 /*&& NULL == m_pObjSetInHandle*/);
		{
			KG3DScene* pScene = this;
			KG3DSceneOutputWnd* pWnd = pScene->GetPrimaryWindow();
			KG_PROCESS_ERROR(NULL != pWnd);
			
			D3DXVECTOR3 vSrc;
			D3DXVECTOR3 vNormalizedDir;
			pWnd->GetPickRay(&vSrc, &vNormalizedDir, NULL);
			
			BOOL bFoundInter = FALSE;

			// 计算出复制对象坐标的中值来
			D3DXVECTOR3 vCenter(0, 0, 0);
			FLOAT tempX = 0;
			FLOAT tempY = 0;
			FLOAT tempZ = 0;

			for (vector<KG3DRepresentObject*>::iterator it = m_vectorCopyRepresentObject.begin(); it != m_vectorCopyRepresentObject.end(); it++)
			{
				KG3DRepresentObject* pRepresentObject = *it;
				D3DXVECTOR3 objectLocation = pRepresentObject->GetTranslation();
				tempX += objectLocation.x;
				tempY += objectLocation.y;
				tempZ += objectLocation.z;
			}

			tempX = (tempX / m_vectorCopyRepresentObject.size());
			tempY = (tempY / m_vectorCopyRepresentObject.size());
			tempZ = (tempZ / m_vectorCopyRepresentObject.size());
			vCenter.x = tempX;
			vCenter.y = tempY;
			vCenter.z = tempZ;

			D3DXVECTOR3 locationOffset = vSrc - vCenter;
				
			for (vector<KG3DRepresentObject*>::iterator it = m_vectorCopyRepresentObject.begin(); it != m_vectorCopyRepresentObject.end(); it++)
			{
				KG3DRepresentObject* pRepresentObject = *it;								
				bFoundInter = FALSE;
				D3DXVECTOR3 vInter(0, 0, 0);
				D3DXVECTOR3 newLocation = pRepresentObject->GetTranslation() + locationOffset;
				D3DXVECTOR3 newDirection = vNormalizedDir;

				// 新的碰撞接口
				if (this->IsUseCrossPlayMode())// 连对象也检测
				{
					FLOAT fTempRet = FLT_MAX;
					
					bFoundInter = ::RayIntersection(*this, newLocation, newDirection, &fTempRet, NULL, NULL, EM_RAY_INTERSECTION_MAX_RANGE, &m_CopyInterFil);
					if (bFoundInter)
					{
						vInter = newLocation + newDirection * fTempRet;
					}
				}
				else			
				{
					FLOAT fTempRet = FLT_MAX;
					bFoundInter = ::RayIntersectionTerrain(*this, newLocation, newDirection, &fTempRet);
					if (bFoundInter)
					{
						vInter = newLocation + newDirection * fTempRet;
					}
				}

				KG_PROCESS_ERROR(bFoundInter);

				if (pRepresentObject->GetType() == REPRESENTOBJECT_TRAFFICPOINT)
				{
					D3DXVECTOR3 vecWaterHeigt;
					this->GetWaterHeight(&vecWaterHeigt, &vInter);
					if (vInter.y < vecWaterHeigt.y)
					{
						vInter.y = vecWaterHeigt.y;
					}
				}

				pRepresentObject->SetTranslation(&vInter);
			}					
		}
			
		for (vector<KG3DRepresentObject*>::iterator it = m_vectorCopyRepresentObject.begin(); it != m_vectorCopyRepresentObject.end(); it++)
		{
			KG3DRepresentObject* pRepresentObject = *it;
			pRepresentObject->FrameMove();
		}
	}	
	
Exit1:
	return S_OK;
Exit0:

	return E_FAIL;
}

HRESULT KG3DSceneSceneEditor::DoPlaceObject()
{
	KG3DRepresentObject *pCopyObject = NULL;
	BOOL bIsObjectAddedToScene = FALSE;

	for (vector<KG3DRepresentObject*>::iterator it = m_vectorCopyRepresentObject.begin(); it != m_vectorCopyRepresentObject.end(); it++)
	{
		pCopyObject = *it;
		HRESULT hr = E_FAIL;
		
		bIsObjectAddedToScene = FALSE;
		/*
		{
			DWORD dwNowTime = g_cGraphicsTool.GetNowTime();
			enum
			{
				EM_MIN_PLACE_OBJ_TIME_GAP = 300,//300毫秒之内不能放Obj 
			};
			KG_PROCESS_ERROR(dwNowTime - m_dwLastPlaceTime > EM_MIN_PLACE_OBJ_TIME_GAP);
			m_dwLastPlaceTime = dwNowTime;
		}
		*/

		KG_PROCESS_ERROR(pCopyObject->IsResourceReady() );//不Ready的话，会出错
		
		KG3DRepresentObject *pObject = NULL;
		hr = g_GetObjectTable().GetCopy(&pObject, pCopyObject);
		KG_COM_PROCESS_ERROR(hr);

		// 复制坐标
		D3DXVECTOR3 Pos(0,0,0);
		pCopyObject->GetTranslation(&Pos);
		pObject->SetTranslation(&Pos);

		// 自动绑上在和模型同目录中和它同名字的bind文件　
		for (DWORD n = 0; n < pObject->GetModelCount(); n++)
		{
			KG3DModel* pModel = NULL;
			if (SUCCEEDED(pObject->GetModel(pModel, n,FALSE)))
			{

				TCHAR szFilePath[MAX_PATH];
				sprintf(szFilePath, TEXT("%s"), pModel->m_scName.c_str());
				TCHAR* pStr = strrchr(szFilePath, '.');
				if (pStr)
				{
					KG3DSFXSuede* pSuede = NULL;
					strncpy(pStr, TEXT(".bind\0"), sizeof(TEXT(".bind\0")));
					if(g_IsFileExist(szFilePath))
					{
						if (SUCCEEDED(g_cModelTable.LoadResourceFromFile(szFilePath, 0, MLO_TEXTURE_MULTI, (IKG3DResourceBase**)&pSuede)))
						{
							pSuede->GetAnimationController(ANICTL_PRIMARY)->PauseAnimation(TRUE);
							if (SUCCEEDED(pSuede->BindToBone(pModel, "")))
								this->GetVecSFXSuede().push_back(pSuede);
							else
								SAFE_RELEASE(pSuede);
						}
					}
				}
			}
		}

		BOOL bToFile = TRUE;
		if(pObject->GetType()==REPRESENTOBJECT_NPC)
		{
			bToFile = FALSE;
		}
		else if(pObject->GetType()==REPRESENTOBJECT_DOODAD)
		{
			bToFile = FALSE;
		}
		else if(pObject->GetType()==REPRESENTOBJECT_WAYPOINT)
		{
			bToFile = FALSE;
		}	
		else if(pObject->GetType()==REPRESENTOBJECT_TRAFFICPOINT)
		{
			bToFile = FALSE;
		}
		else if(pObject->GetType()==REPRESENTOBJECT_REFRESHPOINT)
		{
			bToFile = FALSE;
		}
		else if (pObject->GetType() == REPRESENTOBJECT_NODE)
		{
			bToFile = FALSE;
		}
		else if (pObject->GetType() == REPRESENTOBJECT_WALL)
		{
			bToFile = FALSE;//暂时不要放到文件中
		}

		//hr = m_pSceneEditor->AddRepresentObject(pObject,TRUE, FALSE,bToFile);	//生命周期由Scene管理
		ULONG uAddOption = EM_SCENE_ADD_OPTION_SELECTABLE;
		if(bToFile)
			uAddOption |= EM_SCENE_ADD_OPTION_TO_FILE;
		hr = this->AddRepresentObject(pObject, uAddOption, NULL);
		KG_COM_PROCESS_ERROR(hr);
		bIsObjectAddedToScene = TRUE;

		if(pObject->GetType()==REPRESENTOBJECT_DEFAULT)
		{
			KG3DModel* pModel = NULL;
			if(SUCCEEDED(pObject->GetModel(pModel,0,FALSE)))
			{
				KG3DPhysiscScene* pPhysiscScene = this->GetPhysiscScene();
				pModel->SetPhysicsScene(pPhysiscScene);
			}
		}

		if(pObject->GetType()==REPRESENTOBJECT_NPC)
		{
			this->ForceFireEvent2(EVENTEDITORNPCPLACED,0,0,pObject);
		}
		else if(pObject->GetType()==REPRESENTOBJECT_DOODAD)
		{
			this->ForceFireEvent2(EVENTEDITORDOODADPLACED,0,0,pObject);
		}
		else if(pObject->GetType()==REPRESENTOBJECT_WAYPOINT)
		{
			this->ForceFireEvent2(EVENTEDITORWAYPOINTPLACED,0,0,pObject);
		}	
		else if(pObject->GetType()==REPRESENTOBJECT_TRAFFICPOINT)
		{
			this->ForceFireEvent2(EVENTEDITORTRAFFICPOINTPLACED,0,0,pObject);
		}
		else if(pObject->GetType()==REPRESENTOBJECT_REFRESHPOINT)
		{
			this->ForceFireEvent2(EVENTEDITORREFRESHPOINTPLACED,0,0,pObject);
		}

		if(pObject->GetType() == REPRESENTOBJECT_NODE)
		{
			KG3DRepresentObjectNode* pObjectNode = static_cast<KG3DRepresentObjectNode*>(pObject);
			KGLOG_PROCESS_ERROR(pObjectNode);
			DWORD dwNodeType = pObjectNode->GetNodeType();
			D3DXVECTOR3 vPos;
			pObjectNode->GetTranslation(&vPos);
			switch(dwNodeType)
			{
			case NODETYPE_ROAD:
				{

					KG3DTerrainRoadMgr* pRoadMgr = NULL;
					KG_PROCESS_ERROR(this->m_lpSceneOutDoorSpaceMgr);
					pRoadMgr =  this->m_lpSceneOutDoorSpaceMgr->m_lpRoadMgr;
					KG_PROCESS_ERROR(pRoadMgr);
					pRoadMgr->AddNodeToCurSerpentine(pObjectNode,vPos);				
					break;
				}
			case NODETYPE_RIVER:
				{
					KG3DTerrainRiverMgr* pRiverMgr = NULL;
					KG_PROCESS_ERROR(this->m_lpSceneOutDoorSpaceMgr);
					pRiverMgr =  this->m_lpSceneOutDoorSpaceMgr->m_lpRiverMgr;
					KG_PROCESS_ERROR(pRiverMgr);
					hr = pRiverMgr->AddNodeToCurSerpentine(pObjectNode,vPos);
					break;
				}
			case NODETYPE_WALLKEYPOINT:
				{
					KG3DRepresentObjectWall* pWall = NULL;
					int nWall= 0;
					int nKey = 0;
					pObjectNode->GetWall(pWall,nWall,nKey);
					if(pWall)
					{
						pWall->ModifyWallKeyPoint(pWall,nWall,nKey,vPos);
					}

					KG3DRepresentObjectNode* pHandleNode = static_cast<KG3DRepresentObjectNode*>(pCopyObject);
					if(pHandleNode)
					{
						pHandleNode->SetWall(pWall,nWall,nKey+1);
					}
					break;
				}
			}
		}
	}

	return S_OK;
Exit0:
	if (bIsObjectAddedToScene)
	{
		RemoveRepresentObject(pCopyObject);
	}

	return E_FAIL;
}

HRESULT KG3DSceneSceneEditor::CopySelection()
{
	// 释放原先复制的对象
	if (m_vectorCopyRepresentObject.size() > 0)
	{
		for (vector<KG3DRepresentObject*>::iterator it = m_vectorCopyRepresentObject.begin(); it != m_vectorCopyRepresentObject.end(); it++)
		{
			KG3DRepresentObject* pRepresentObject = *it;		
			SAFE_RELEASE(pRepresentObject);
		}

		m_vectorCopyRepresentObject.clear(); 
	}	

	HRESULT hr = E_FAIL;

	std::vector<KG3DRepresentObject*> vecSelected; // 要复制出来，不然一边复制，选择会增多
	ULONG selSize = this->GetSelectionTool().GetSelectionCount();
	KG_PROCESS_ERROR(selSize <= 30 && _T("选择太多，复制不执行"));
	vecSelected.reserve(selSize);

	TypeEntityAccessProxy& proxy = this->GetSelectionTool().GetSelectionAccessProxy();
	for(proxy.Begin(); ! proxy.IsEnd(); proxy.StepForward())
	{
		KG3DSceneEntity Entity = proxy.GetCur();
		if(Entity.GetType()==SCENEENTITY_OBJECT)
		{
			KG3DRepresentObject *pObject = static_cast<KG3DRepresentObject*>(Entity.m_pPointer);
			if(pObject)
			{
				vecSelected.push_back(pObject);
			}
		}
	}

	this->GetSelectionTool().ClearSelection();//这里应该清空选择，然后在新物件创建的时候选中新的物件
	for (std::vector<KG3DRepresentObject*>::iterator it = vecSelected.begin(); it != vecSelected.end(); ++it)
	{
		KG3DRepresentObject* pObject = *it;

		KG3DRepresentObject *pNewObject = NULL;
		hr = g_GetObjectTable().GetCopy(&pNewObject,pObject);
		KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

		D3DXVECTOR3 Pos(0,0,0);
		pObject->GetTranslation(&Pos);
		pNewObject->SetTranslation(&Pos);
		pNewObject->UpdateTransformation();

#if defined(DEBUG) | defined(_DEBUG)
		ULONG uRef = KGGetRef(pNewObject);
#endif

		ULONG uOption = EM_SCENE_ADD_OPTION_SELECTABLE | EM_SCENE_ADD_OPTION_SELECTED;

		if(pNewObject->GetType()==REPRESENTOBJECT_NPC)
		{
			AddRepresentObject(pNewObject, uOption, NULL);
			// FireEvent2(EVENTEDITORNPCPLACED,0,0,pNewObject);
		}
		else if(pNewObject->GetType()==REPRESENTOBJECT_DOODAD)
		{
			AddRepresentObject(pNewObject, uOption, NULL);
			// FireEvent2(EVENTEDITORDOODADPLACED,0,0,pNewObject);
		}
		else if(pNewObject->GetType()==REPRESENTOBJECT_WAYPOINT)
		{
			AddRepresentObject(pNewObject, uOption, NULL);
			// FireEvent2(EVENTEDITORWAYPOINTPLACED,0,0,pNewObject);
		}
		else if(pNewObject->GetType()==REPRESENTOBJECT_REFRESHPOINT)
		{
			AddRepresentObject(pNewObject, uOption, NULL);
			// FireEvent2(EVENTEDITORREFRESHPOINTPLACED,0,0,pNewObject);
		}
		else			
		{
			AddRepresentObject(pNewObject, uOption | EM_SCENE_ADD_OPTION_TO_FILE, NULL);
		}				
		
		m_vectorCopyRepresentObject.push_back(pNewObject);

#if defined(DEBUG) | defined(_DEBUG)
		_ASSERTE(uRef == KGGetRef(pNewObject) && _T("生命周期应该是被接管了，这里不用管了"));
#endif
				
	}	
	
	// 记录碰撞检查需要过滤的对象
	m_CopyInterFil.SetFiltObject(m_vectorCopyRepresentObject);

	// 设置场景状态
	this->SetEditState(SCENESTATE_COPY_REPRESENTOBJECT);
	
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneSceneEditor::PasteSelection()
{
	HRESULT hr = E_FAIL;

	std::vector<KG3DRepresentObject*>	vecSelected;	//要复制出来，不然一边复制，选择会增多
	ULONG selSize = this->GetSelectionTool().GetSelectionCount();
	KG_PROCESS_ERROR(selSize <= 30 && _T("选择太多，复制不执行"));
	vecSelected.reserve(selSize);

	TypeEntityAccessProxy& proxy = this->GetSelectionTool().GetSelectionAccessProxy();
	for(proxy.Begin(); ! proxy.IsEnd(); proxy.StepForward())
	{
		KG3DSceneEntity Entity = proxy.GetCur();
		if(Entity.GetType()==SCENEENTITY_OBJECT)
		{
			KG3DRepresentObject *pObject = static_cast<KG3DRepresentObject*>(Entity.m_pPointer);
			if(pObject)
			{
				vecSelected.push_back(pObject);
			}
		}
	}

	this->GetSelectionTool().ClearSelection();//这里应该清空选择，然后在新物件创建的时候选中新的物件
	for (std::vector<KG3DRepresentObject*>::iterator it = vecSelected.begin(); it != vecSelected.end(); ++it)
	{
		KG3DRepresentObject* pObject = *it;

		KG3DRepresentObject *pNewObject = NULL;
		hr = g_GetObjectTable().GetCopy(&pNewObject,pObject);
		KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

		D3DXVECTOR3 Pos(0,0,0);

		pObject->GetTranslation(&Pos);
		pNewObject->SetTranslation(&Pos);

#if defined(DEBUG) | defined(_DEBUG)
		ULONG uRef = KGGetRef(pNewObject);
#endif

		ULONG uOption = EM_SCENE_ADD_OPTION_SELECTABLE | EM_SCENE_ADD_OPTION_SELECTED;

		if(pNewObject->GetType()==REPRESENTOBJECT_NPC)
		{
			//AddRepresentObject(pNewObject, TRUE, TRUE,FALSE);
			AddRepresentObject(pNewObject, uOption, NULL);
			FireEvent2(EVENTEDITORNPCPLACED,0,0,pNewObject);
		}
		else if(pNewObject->GetType()==REPRESENTOBJECT_DOODAD)
		{
			//AddRepresentObject(pNewObject, TRUE, TRUE,FALSE);
			AddRepresentObject(pNewObject, uOption, NULL);
			FireEvent2(EVENTEDITORDOODADPLACED,0,0,pNewObject);
		}
		else if(pNewObject->GetType()==REPRESENTOBJECT_WAYPOINT)
		{
			//AddRepresentObject(pNewObject, TRUE, TRUE,FALSE);
			AddRepresentObject(pNewObject, uOption, NULL);
			FireEvent2(EVENTEDITORWAYPOINTPLACED,0,0,pNewObject);
		}
		else if(pNewObject->GetType()==REPRESENTOBJECT_REFRESHPOINT)
		{
			//AddRepresentObject(pNewObject, TRUE, TRUE,FALSE);
			AddRepresentObject(pNewObject, uOption, NULL);
			FireEvent2(EVENTEDITORREFRESHPOINTPLACED,0,0,pNewObject);
		}
		else			
		{
			//AddRepresentObject(pNewObject, TRUE, TRUE,TRUE);
			AddRepresentObject(pNewObject, uOption | EM_SCENE_ADD_OPTION_TO_FILE, NULL);
		}

#if defined(DEBUG) | defined(_DEBUG)
		_ASSERTE(uRef == KGGetRef(pNewObject) && _T("生命周期应该是被接管了，这里不用管了"));
#endif

		

		//AddSelectedEntity(SCENEENTITY_OBJECT,pNewObject);
		//this->GetSelectionTool().SetSelected(KG3DSceneEntity(SCENEENTITY_OBJECT,pNewObject), TRUE);
	}

	_ASSERTE(static_cast<size_t>(this->GetSelectionTool().GetSelectionCount()) == vecSelected.size());
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneSceneEditor::AddVisibleSet(D3DXVECTOR3& Position)
{
	HRESULT hr = S_OK;
	KG3DRepresentObjectVisibleEntitySet *pVisibleSet = NULL;

	hr = g_GetObjectTable().NewOneResourse(REPRESENTOBJECT_VISIBLESET,(IKG3DResourceBase**)&pVisibleSet);

	pVisibleSet->LoadEdgeModel("Data\\public\\SkyBox\\Sky.Mesh");

	pVisibleSet->SetTranslation(&Position);

	//AddRepresentObject(pVisibleSet, TRUE, FALSE,TRUE);
	AddRepresentObject(pVisibleSet, EM_SCENE_ADD_OPTION_SELECTABLE | EM_SCENE_ADD_OPTION_TO_FILE, NULL);

	return S_OK;
}
HRESULT KG3DSceneSceneEditor::ApplyVisibleSetChange(KG3DRepresentObjectVisibleEntitySet *pSet,BOOL bIn)
{
	KGLOG_PROCESS_ERROR_CONSOLE(pSet);

	//if(bIn)
	//{
	//	KG3DSceneLayer* pLayerIn = FindLayer(pSet->m_dwInsideLayerID);
	//	KG3DSceneLayer* pLayerOut = FindLayer(pSet->m_dwOutsideLayerID);
	//	if(pLayerIn)
	//	{
	//		pLayerIn->m_bVisible = TRUE;
	//	}
	//	if(pLayerOut)
	//	{
	//		pLayerOut->m_bVisible = FALSE;
	//	}

	//}
	//else
	//{
	//	KG3DSceneLayer* pLayerIn = FindLayer(pSet->m_dwInsideLayerID);
	//	KG3DSceneLayer* pLayerOut = FindLayer(pSet->m_dwOutsideLayerID);
	//	if(pLayerIn)
	//	{
	//		pLayerIn->m_bVisible = FALSE;
	//	}
	//	if(pLayerOut)
	//	{
	//		pLayerOut->m_bVisible = TRUE;
	//	}
	//}
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneSceneEditor::ForceLoadAllEntity()
{
	BOOL bSaveMultithread = g_eEngineOption.bUseMultiThreadLoad;
	g_eEngineOption.bUseMultiThreadLoad = FALSE;

	m_lpSceneOutDoorSpaceMgr->ForceLoadAllEntity();

	g_eEngineOption.bUseMultiThreadLoad = bSaveMultithread;
	return S_OK;
}
HRESULT KG3DSceneSceneEditor::ForceLoadEntity(D3DXVECTOR2 A, D3DXVECTOR2 C)
{
	BOOL bSaveMultithread = g_eEngineOption.bUseMultiThreadLoad;
	g_eEngineOption.bUseMultiThreadLoad = FALSE;

	m_lpSceneOutDoorSpaceMgr->ForceLoadEntity(A,C);

	g_eEngineOption.bUseMultiThreadLoad = bSaveMultithread;
	return S_OK;
}
//HRESULT KG3DSceneSceneEditor::SaveLayers(LPSTR pFileName)
//{
//	size_t SizeWrite = 0;
//	KGLOG_PROCESS_ERROR(pFileName);
//
//	FILE* pFile = fopen(pFileName,"wb");
//	KGLOG_PROCESS_ERROR(pFile);
//
//	DWORD dwFileType = 0x08070602;
//	DWORD dwVersion  = 0;
//	DWORD dwNumLayer= (DWORD)m_vecSceneLayer.size();
//	DWORD dwExten[128];
//	ZeroMemory(dwExten,sizeof(DWORD)*128);
//
//	SizeWrite = fwrite(&dwFileType, 1, sizeof(DWORD), pFile);
//	KGLOG_PROCESS_ERROR_CONSOLE(SizeWrite == sizeof(DWORD));
//
//	SizeWrite = fwrite(&dwVersion, 1, sizeof(DWORD), pFile);
//	KGLOG_PROCESS_ERROR_CONSOLE(SizeWrite == sizeof(DWORD));
//
//	SizeWrite = fwrite(&dwNumLayer, 1, sizeof(DWORD), pFile);
//	KGLOG_PROCESS_ERROR_CONSOLE(SizeWrite == sizeof(DWORD));
//
//	SizeWrite = fwrite(&dwExten, 1, sizeof(DWORD)*128, pFile);
//	KGLOG_PROCESS_ERROR_CONSOLE(SizeWrite == sizeof(DWORD)*128);
//
//	for(size_t i=0;i<m_vecSceneLayer.size();i++)
//	{
//		KG3DSceneLayer* pLayer = m_vecSceneLayer[i];
//
//		LayerSaveInfo vLayerSaveInfo;
//		ZeroMemory(&vLayerSaveInfo,sizeof(LayerSaveInfo));
//		vLayerSaveInfo.dwParentID = 0xFFFFFFFF;
//
//		if(pLayer)
//		{
//			vLayerSaveInfo.bUsed = TRUE;
//			vLayerSaveInfo.bVisible = pLayer->m_bVisible;
//			vLayerSaveInfo.bSelecteable = pLayer->m_bSelectable;
//			vLayerSaveInfo.dwID = pLayer->m_dwID;
//			if(pLayer->m_pParentLayer)
//			{
//				vLayerSaveInfo.dwParentID = pLayer->m_pParentLayer->m_dwID;
//			}
//			wsprintf(vLayerSaveInfo.Name,"%s",pLayer->m_sName.c_str());
//		}
//		else
//		{
//			vLayerSaveInfo.bUsed = FALSE;
//		}
//
//		SizeWrite = fwrite(&vLayerSaveInfo, 1, sizeof(LayerSaveInfo), pFile);
//		KGLOG_PROCESS_ERROR_CONSOLE(SizeWrite == sizeof(LayerSaveInfo));
//
//	}
//	fclose(pFile);
//	return S_OK;
//Exit0:
//	return E_FAIL;
//}
//
//HRESULT KG3DSceneSceneEditor::LoadLayers(LPSTR pFileName)
//{
//	size_t SizeRead = 0;
//	FILE *pFile = NULL;
//	vector<LayerSaveInfo>vecLayerSaveInfo;
//	KGLOG_PROCESS_ERROR(pFileName);
//
//
//	pFile = fopen(pFileName,"rb");
//	KGLOG_PROCESS_ERROR(pFile);
//
//	DWORD dwFileType = 0x08070602;
//	DWORD dwVersion  = 0;
//	DWORD dwNumLayer= (DWORD)m_vecSceneLayer.size();
//	DWORD dwExten[128];
//	ZeroMemory(dwExten,sizeof(DWORD)*128);
//
//	DeleteAllLayers();
//
//	SizeRead = fread(&dwFileType, 1, sizeof(DWORD), pFile);
//	KGLOG_PROCESS_ERROR_CONSOLE(SizeRead == sizeof(DWORD));
//
//	SizeRead = fread(&dwVersion, 1, sizeof(DWORD), pFile);
//	KGLOG_PROCESS_ERROR_CONSOLE(SizeRead == sizeof(DWORD));
//
//	SizeRead = fread(&dwNumLayer, 1, sizeof(DWORD), pFile);
//	KGLOG_PROCESS_ERROR_CONSOLE(SizeRead == sizeof(DWORD));
//
//	SizeRead = fread(&dwExten, 1, sizeof(DWORD)*128, pFile);
//	KGLOG_PROCESS_ERROR_CONSOLE(SizeRead == sizeof(DWORD)*128);
//
//	for(size_t i=0;i<dwNumLayer;i++)
//	{
//		LayerSaveInfo vLayerSaveInfo;
//		ZeroMemory(&vLayerSaveInfo,sizeof(LayerSaveInfo));
//		//////////////////////////////////////////////////////////////////////////
//		SizeRead = fread(&vLayerSaveInfo, 1, sizeof(LayerSaveInfo), pFile);
//		KGLOG_PROCESS_ERROR_CONSOLE(SizeRead == sizeof(LayerSaveInfo));
//
//		vecLayerSaveInfo.push_back(vLayerSaveInfo);
//
//		if(vLayerSaveInfo.bUsed)
//		{
//			KG3DSceneLayer* pLayer = new KG3DSceneLayer(vLayerSaveInfo.Name, 0);
//			KGLOG_PROCESS_ERROR_CONSOLE(pLayer);
//			pLayer->m_dwID = vLayerSaveInfo.dwID;
//			pLayer->m_bVisible = vLayerSaveInfo.bVisible;
//			pLayer->m_bSelectable = vLayerSaveInfo.bSelecteable;
//
//			m_vecSceneLayer.push_back(pLayer);
//			if(i==0)
//				m_pRootLayer = pLayer;
//		}
//		else
//		{
//			m_vecSceneLayer.push_back(NULL);
//		}
//	}
//
//	for(size_t i=0;i<dwNumLayer;i++)
//	{
//		DWORD dwParentID = vecLayerSaveInfo[i].dwParentID;
//		if(dwParentID!=0xFFFFFFFF)
//		{
//			KG3DSceneLayer *pLayer = m_vecSceneLayer[i];
//			KG3DSceneLayer *pParent = m_vecSceneLayer[dwParentID];
//
//			if ((pParent)&&(pLayer))
//			{
//				pParent->AddChildLayer(pLayer);
//			}
//		}
//	}
//
//	fclose(pFile);
//	pFile = NULL;
//	return S_OK;
//Exit0:
//	if (pFile)
//	{
//		fclose(pFile);
//		pFile = NULL;
//	}
//	return E_FAIL;
//}

HRESULT KG3DSceneSceneEditor::GetFirstSelectedObject(IEKG3DRepresentObject** ppObj)
{
	_ASSERTE(ppObj);
	if (ppObj)
	{
		*ppObj = GetFirstSelectedObject();
		return S_OK;
	}
	return E_FAIL;
}

IEKG3DRepresentObject *KG3DSceneSceneEditor::GetFirstSelectedObject()
{
	/*KG3DSceneEntityList::iterator i = m_listSelectedEntity.begin();
	while (i!=m_listSelectedEntity.end())
	{
		KG3DSceneEntity& Entity = *i;*/
	TypeEntityAccessProxy& proxy = this->GetSelectionTool().GetSelectionAccessProxy();
	for(proxy.Begin(); ! proxy.IsEnd(); proxy.StepForward())
	{
		KG3DSceneEntity Entity = proxy.GetCur();

		switch(Entity.GetType())
		{
		case SCENEENTITY_OBJECT:
			{
				KG3DRepresentObject *pObject = static_cast<KG3DRepresentObject*>(Entity.m_pPointer);
				return pObject;
				break;
			}
		}
		//i++;
	}
	return NULL;
}

HRESULT KG3DSceneSceneEditor::GetSelectedObjectInHand()
{
	KG3DRepresentObject* pObjCopied = NULL;
	KG3DRepresentObject *pObject = dynamic_cast<KG3DRepresentObject*>(GetFirstSelectedObject());
	KG_PROCESS_SUCCESS(pObject == NULL);

	//SAFE_RELEASE(m_lpHandleObject);

	HRESULT hr = E_FAIL;
	hr = g_GetObjectTable().GetCopy(&/*m_lpHandleObject*/pObjCopied,pObject);
	KG_COM_PROCESS_ERROR(hr);

	hr = this->SetObjectInHandle(pObjCopied, TRUE);
	if (FAILED(hr))
	{
		SAFE_RELEASE(pObjCopied);
		goto Exit0;
	}

Exit1:
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneSceneEditor::AddOneMeasureTool()
{
	D3DXVECTOR3 A(0,0,0);
	D3DXVECTOR3 B(0,300,0);
	KG3DMeasureTool *pMeasureTool = NULL;

	pMeasureTool = new KG3DMeasureTool;
	KGLOG_PROCESS_ERROR(pMeasureTool);

	pMeasureTool->CreateTwoPointLine(A,B);

	{
		//KG3DSceneEntity EntityA(SCENEENTITY_POSITION,&pMeasureTool->m_vPositions[0]);//<Selectable Refactor>
		//this->GetSelectionTool().SetSelectable(EntityA, TRUE);
		this->IEInsertEntitySelectable(SCENEENTITY_POSITION,&pMeasureTool->m_vPositions[0], TRUE);

		/*KG3DSceneEntity EntityB(SCENEENTITY_POSITION,&pMeasureTool->m_vPositions[1]);//<Selectable Refactor>
		this->GetSelectionTool().SetSelectable(EntityB, TRUE);*/
		this->IEInsertEntitySelectable(SCENEENTITY_POSITION,&pMeasureTool->m_vPositions[1], TRUE);
	}

	m_listMeasureTool.push_back(pMeasureTool);
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneSceneEditor::DeleteMeasureTool(KG3DMeasureTool *pMeasureTool)
{
	/*KG3DSceneEntity EntityA(SCENEENTITY_POSITION,&pMeasureTool->m_vPositions[0]);
	this->GetSelectionTool().SetSelectable(EntityA, FALSE);*///<Selectable Refactor>
	this->IEInsertEntitySelectable(SCENEENTITY_POSITION,&pMeasureTool->m_vPositions[0], FALSE);

	/*KG3DSceneEntity EntityB(SCENEENTITY_POSITION,&pMeasureTool->m_vPositions[1]);
	this->GetSelectionTool().SetSelectable(EntityB, FALSE);*///<Selectable Refactor>
	this->IEInsertEntitySelectable(SCENEENTITY_POSITION,&pMeasureTool->m_vPositions[1], FALSE);

	m_listMeasureTool.remove(pMeasureTool);
	SAFE_DELETE(pMeasureTool);
	return S_OK;
}

HRESULT KG3DSceneSceneEditor::DeleteAllMeasureTool()
{
	list<KG3DMeasureTool*>::iterator i = m_listMeasureTool.begin();
	while (i!=m_listMeasureTool.end())
	{
		KG3DMeasureTool *pMeasureTool = *i;

		/*KG3DSceneEntity EntityA(SCENEENTITY_POSITION,&pMeasureTool->m_vPositions[0]);
		this->GetSelectionTool().SetSelectable(EntityA, FALSE);*///<Selectable Refactor>
		this->IEInsertEntitySelectable(SCENEENTITY_POSITION,&pMeasureTool->m_vPositions[0], FALSE);

		/*KG3DSceneEntity EntityB(SCENEENTITY_POSITION,&pMeasureTool->m_vPositions[1]);
		this->GetSelectionTool().SetSelectable(EntityB, FALSE);*///<Selectable Refactor>
		this->IEInsertEntitySelectable(SCENEENTITY_POSITION,&pMeasureTool->m_vPositions[1], FALSE);

		SAFE_DELETE(pMeasureTool);
		i++;
	}
	m_listMeasureTool.clear();
	return S_OK;
}

HRESULT KG3DSceneSceneEditor::RenderMeasureTools(KG3DCamera *pCamera,D3DVIEWPORT9& viewport)
{
	list<KG3DMeasureTool*>::iterator i = m_listMeasureTool.begin();
	while (i!=m_listMeasureTool.end())
	{
		KG3DMeasureTool *pMeasureTool = *i;

		pMeasureTool->Render(pCamera,viewport);

		i++;
	}
	return S_OK;
}               
                        
HRESULT KG3DSceneSceneEditor::RenderSceneSnap(LPSTR pFileName)
{
    HRESULT hrResult = E_FAIL;
    HRESULT hrRetCode = E_FAIL;
    KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr = NULL;
    D3DXVECTOR2 LowerLeft, UpperRight;
	int nSave = g_nNumAllowTextureDynamicLoadFrame;
	g_nNumAllowTextureDynamicLoadFrame = 65535;
	BOOL bSave2 = g_eEngineOption.bUseMultiThreadLoad;
	g_eEngineOption.bUseMultiThreadLoad = FALSE;
	BOOL bSaveUnLoad =g_cEngineOption.bEnableDynamicUnload;
	g_cEngineOption.bEnableDynamicUnload =FALSE;

    pOutDoorSpaceMgr = this->GetOutDoorSpaceMgr();
    KGLOG_PROCESS_ERROR(pOutDoorSpaceMgr);
	
    hrRetCode = this->ForceLoadAllEntity();
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    hrRetCode = pOutDoorSpaceMgr->GetSceneRect(&LowerLeft, &UpperRight);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    //400000是默认的场景高度
    {
        AABBOX Box(D3DXVECTOR3(LowerLeft.x, -400000, LowerLeft.y), D3DXVECTOR3(UpperRight.x, 400000, UpperRight.y));

        m_vSnapTool.SetSceneEditor(this);

        hrRetCode = m_vSnapTool.Render(Box, pFileName);
        KGLOG_COM_PROCESS_ERROR(hrRetCode);
    }

    hrResult = S_OK;
Exit0:
	g_nNumAllowTextureDynamicLoadFrame = nSave;
	g_eEngineOption.bUseMultiThreadLoad = bSave2;
	g_cEngineOption.bEnableDynamicUnload =bSaveUnLoad;
    return hrResult;
}


HRESULT KG3DSceneSceneEditor::ExprotTerrainTexture(LPSTR pFileName)
{
	HRESULT hrResult = E_FAIL;
	HRESULT hrRetCode = E_FAIL;
	KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr = NULL;
	D3DXVECTOR2 LowerLeft, UpperRight;
	int nSave = g_nNumAllowTextureDynamicLoadFrame;
	g_nNumAllowTextureDynamicLoadFrame = 65535;
	BOOL bSave2 = g_eEngineOption.bUseMultiThreadLoad;
	g_eEngineOption.bUseMultiThreadLoad = FALSE;
	BOOL bSaveUnLoad =g_cEngineOption.bEnableDynamicUnload;
	g_cEngineOption.bEnableDynamicUnload =FALSE;

	pOutDoorSpaceMgr = this->GetOutDoorSpaceMgr();
	KGLOG_PROCESS_ERROR(pOutDoorSpaceMgr);

	{
		m_lpSceneOutDoorSpaceMgr->ForceLoadAllTerrainBlock();
	}

	hrRetCode = pOutDoorSpaceMgr->GetSceneRect(&LowerLeft, &UpperRight);
	KGLOG_COM_PROCESS_ERROR(hrRetCode);

	//400000是默认的场景高度
	{
		
		AABBOX Box(D3DXVECTOR3(LowerLeft.x, -400000, LowerLeft.y), D3DXVECTOR3(UpperRight.x, 400000, UpperRight.y));

		hrRetCode = m_vSnapTool.ExportTerrainTexture(Box,this,pFileName);
		KGLOG_COM_PROCESS_ERROR(hrRetCode);
	}

	hrResult = S_OK;
Exit0:

	g_nNumAllowTextureDynamicLoadFrame = nSave;
	g_eEngineOption.bUseMultiThreadLoad = bSave2;
	g_cEngineOption.bEnableDynamicUnload =bSaveUnLoad;
	return hrResult;
}
template<typename T>
inline T GetValidValue(T InValue, INT nOff)
{
	T TValue = InValue + ((T)nOff);
	T minvalue = ((T)100 < TValue ? (T)100 : TValue);
	return (1 > minvalue) ? 1 : minvalue;
}

template<>
inline WORD GetValidValue(WORD InValue, INT nOff)
{
	INT TValue = InValue + nOff;
	INT minvalue = (100 < TValue ? 100 : TValue);
	return WORD((1 > minvalue) ? 1 : minvalue);
}

inline void KG3DSceneSceneEditor::SetCamareState(WORD wState, BOOL bSet)
{
	if (bSet)
		m_CamareState |= WORD(wState);
	else
		m_CamareState &= ~WORD(wState);
}

//专门处理CS方式的镜头漫游
void KG3DSceneSceneEditor::TrackCamareFrameMove()
{
	D3DXVECTOR3 Pos,LookAt, vDelta, vFront, vRight, vUp;
	m_SceneWndManager->GetCurOutputWnd()->GetCamera().GetPosition(&Pos);
	m_SceneWndManager->GetCurOutputWnd()->GetCamera().GetLookAtPosition(&LookAt);
	vUp = m_SceneWndManager->GetCurOutputWnd()->GetCamera().GetCameraUp();
	vFront = m_SceneWndManager->GetCurOutputWnd()->GetCamera().GetCameraFront();
	vRight = m_SceneWndManager->GetCurOutputWnd()->GetCamera().GetCameraRight();

	D3DXVec3Normalize(&vFront, &vFront);
	D3DXVec3Normalize(&vRight, &vRight);
	D3DXVec3Normalize(&vUp, &vUp);

	if (m_CamareState & KG3DSceneSceneEditor::cmsCamareUp)
	{
		vUp *= float(g_eEngineOption.nFlySpeedY);
		Pos += vUp;
		LookAt += vUp;
	}
	else if (m_CamareState & KG3DSceneSceneEditor::cmsCamareDown)
	{
		vUp *= float(g_eEngineOption.nFlySpeedY);
		Pos -= vUp;
		LookAt -= vUp;
	}
	if (m_CamareState & KG3DSceneSceneEditor::cmsFoward)
	{
		vFront *= float(g_eEngineOption.nFlySpeedX);
		Pos += vFront;
		LookAt += vFront;
	}
	else if (m_CamareState & KG3DSceneSceneEditor::cmsBack)
	{
		vFront *= -float(g_eEngineOption.nFlySpeedX);
		Pos += vFront;
		LookAt += vFront;
	}
	if (m_CamareState & KG3DSceneSceneEditor::cmsMoveLeft)
	{
		vRight *= -float(g_eEngineOption.nFlySpeedX);
		Pos += vRight;
		LookAt += vRight;
	}
	else if (m_CamareState & KG3DSceneSceneEditor::cmsMoveRight)
	{
		vRight *= float(g_eEngineOption.nFlySpeedX);
		Pos += vRight;
		LookAt += vRight;
	}

	m_SceneWndManager->GetCurOutputWnd()->GetCamera().SetPositionLookAtUp(&Pos, &LookAt, NULL);
}

INT KG3DSceneSceneEditor::ExecAction(INT nActionID, INT nExecState, WPARAM wParam, LPARAM lParam)//nExeState如果是1，表示按键被按下，如果是0，表示按下了，然后弹起
{
	/*
	在这里加东西的童鞋请看一下

	KG3DSceneSelectionTool和其它工具的Listener模式的说明：

	假设需要在 EXECACTION_SOMETHING 的时候需要调用 ATool的DoSomething()
	
	如果正向调用：

	则，在这里写上

	switch(actionID)
	{
		...
		case EXECACTION_SOMETHING:
		{
			aTool.DoSomething();
		}
	}

	如果使用Listener模式：

	1.把ATool从KG3DMessageReceiver系列类中派生，并注册，这个参考IKG3DMessenger.h的说明
	scene.RegisterListener(KM_EXECUTE_ACTION, m_pReceiver);
	scene.RegisterListener(KM_STATE_CHANGE, m_pReceiver);
	scene

	Scene 有各种Listener可以注册，没有的可以自己加
	Messenger基类都会自己管理生命周期的，所以注册之后不用管谁先析构之类的问题。

	2.写OnReceiverMessage
	virtual void __stdcall OnReceiveMessage( const KG3DMessage& Message , IKG3DMessageSenderBase* pSender )
	{
		if(Message.m_uMessage != KM_EXECUTE_ACTION)
			return;

		KG3DMessageActionExecutionStruct* pExecActionStruct = reinterpret_cast<KG3DMessageActionExecutionStruct*>(Message.m_dwWParam);
		if(NULL == pExecActionStruct)
			return;

		switch(actionID)
		{
			...
			case EXECACTION_SOMETHING:
			{
				this->DoSomething();
			}
		}
	}

	3.用Listener模式的类，如果内部保存一个从属的KG3DScene的引用就更方便了。譬如
	如果DoSomething需要Camera。那么可以this->DoSomething(m_Scene.GetCamera());
	具体可以参考KG3DSceneSelectionToolImp

	虽然这样看上去是个双向耦合，但KG3DScene会慢慢改造成完全的接口类的，所以通过这个接口
	类取得别的Scene下层节点（如OutputWnd，Camera）并不能认为是耦合。场景图SceneGraph型3DEngine中，Scene的公共接口是作为大部分和Scene相关的类的操作对象的
	
	
	Listener的好处是
	
	1.减少耦合。其实Dosomething是否需要外部知道呢，还是仅仅为了传几个参数，或者是为了一个调用时间。如果是这样的Listener能减少很多耦合。如KG3DTransformTool
	，暴露的接口几乎可以忽略。

	2.集中代码。如果为了调用时间，写成正向调用的话，那么代码一定是散布的，各个调用点都需要暴露一个方法。散布的代码会造成改动的困难。特别是对输入消息的相应
	，各种类的代码就都搅合在一起了。switch case会庞大到难以改动的地步。分成OnXX，OnXX，并virtual，会让改动变得更难。

	3.集中状态量。上述的情况如果要处理状态量，如根据类的某状态改某东西，那么就复杂了。ATool可能需要暴露N多接口。而用Listener的话，全在OnReceiveMessage里面
	处理就好了

	4.Virtools的输入处理用的就是Listener的模式，使用起来是很方便的。一般这种操作方式适合于处理系统消息，特别是输入消息。

	坏处：
	变复杂了一点。一般来说正向调用的逻辑都清晰一些。在内聚型的模块中，正向比较好。
	*/


	INT nRet = 1;
	
	int xPos = LOWORD(lParam); 
	int yPos = HIWORD(lParam);		

	//(by dengzhihui 2006年8月17日 15:24:43
	//将无符号数转为有符号整数
	xPos = xPos > 0x7fff ? (xPos - 0xffff) : xPos;
	yPos = yPos > 0x7fff ? (yPos - 0xffff) : yPos;
	INT nXOff = xPos - g_nMouseMoveX;
	INT nYOff = yPos - g_nMouseMoveY;
	//)

	// 策划场景编辑器
	KG3DSceneOutputWnd *pWnd = m_SceneWndManager->GetCurOutputWnd();
	KG_PROCESS_ERROR(pWnd);
	{

	KG3DCamera& camera = pWnd->GetCamera();
	D3DXVECTOR3 Pos,LookAt;
	D3DXVECTOR3 Left = camera.GetCameraRight();
	D3DXVECTOR3 Front = camera.GetCameraFront();
	D3DXVECTOR3 Up = camera.GetCameraUp();
	camera.GetPosition(&Pos);
	camera.GetLookAtPosition(&LookAt);

	switch (nActionID)
	{/*
	case EXEACTION_DE_FORWARD:  
		{
			if (m_bEditing) // 检查是否在p模式
			{
				m_DE_Forward  = nExecState; 				
			}
			else
			{
				m_pRepresentNPC->SetMoveSpeed(m_CameraSpeed * 2);
				m_pRepresentNPC->SetNpcMoveBits(KG3DRepresentNPC::nmsFoward, nExecState);
			}
			break;
		}
	case EXEACTION_DE_BACKWARD: 
		{
			if (m_bEditing) // 检查是否在p模式
			{
				m_DE_Backward = nExecState;				
			}
			else
			{
				m_pRepresentNPC->SetMoveSpeed(m_CameraSpeed * 2);
				m_pRepresentNPC->SetNpcMoveBits(KG3DRepresentNPC::nmsBack, nExecState);
			}
			break;
		}
	case EXEACTION_DE_LEFT:     
		{
			if (m_bEditing) // 检查是否在p模式
			{
				m_DE_Left = nExecState; 
			}
			else
			{
				m_pRepresentNPC->SetMoveSpeed(m_CameraSpeed * 2);
				m_pRepresentNPC->SetNpcMoveBits(KG3DRepresentNPC::nmsTurnBodyLeft, nExecState);
			}
			break;
		}
	case EXEACTION_DE_RIGHT:    
		{
			if (m_bEditing) // 检查是否在p模式
			{
				m_DE_Right = nExecState; 
			}
			else
			{
				m_pRepresentNPC->SetMoveSpeed(m_CameraSpeed * 2);
				m_pRepresentNPC->SetNpcMoveBits(KG3DRepresentNPC::nmsTurnBodyRight, nExecState);				
			}
			break;
		}

		// 快速前进
	case EXEACTION_DE_FAST_FORWARD: 
		{
			if (m_bEditing) // 检查是否在p模式
			{
				m_DE_Fast_Forward = nExecState;
			}
			else
			{
				m_pRepresentNPC->SetMoveSpeed(m_CameraSpeed * 4);
				m_pRepresentNPC->SetNpcMoveBits(KG3DRepresentNPC::nmsFoward, nExecState);
			}
			break;
		}
	case EXEACTION_DE_FAST_BACKWARD:
		{
			if (m_bEditing) // 检查是否在p模式
			{
				m_DE_Fast_Backward = nExecState;
			}
			else
			{
				m_pRepresentNPC->SetMoveSpeed(m_CameraSpeed * 4);
				m_pRepresentNPC->SetNpcMoveBits(KG3DRepresentNPC::nmsBack, nExecState);
			}
			break;
		}
	case EXEACTION_DE_FAST_LEFT:
		{
			if (m_bEditing) // 检查是否在p模式
			{
				m_DE_Fast_Left = nExecState;
			}
			else
			{
				m_pRepresentNPC->SetMoveSpeed(m_CameraSpeed * 4);
				m_pRepresentNPC->SetNpcMoveBits(KG3DRepresentNPC::nmsTurnBodyLeft, nExecState);
			}
			break;
		}
	case EXEACTION_DE_FAST_RIGHT:
		{
			if (m_bEditing) // 检查是否在p模式
			{
				m_DE_Fast_Right = nExecState;
			}
			else
			{
				m_pRepresentNPC->SetMoveSpeed(m_CameraSpeed * 4);
				m_pRepresentNPC->SetNpcMoveBits(KG3DRepresentNPC::nmsTurnBodyRight, nExecState);	
			}
			break;
		}		*/
	case EXEACTION_ROTATE_VIEW: //shift+中键，以选定的点为中心旋转摄像机视角
		{
			if (nExecState == 0)
			{
				m_bMouseTargetMoving = FALSE;
				break;
			}

			if(!m_bMouseTargetMoving)
			{
				m_bMouseTargetMoving = TRUE;
			}
			else
			{
				if(m_SceneWndManager->GetCurOutputWnd())
				{
					D3DXVECTOR3 Pos,LookAt;
					m_SceneWndManager->GetCurOutputWnd()->GetCamera().GetPosition(&Pos);
					m_SceneWndManager->GetCurOutputWnd()->GetCamera().GetLookAtPosition(&LookAt);

					//D3DXVECTOR3 Right = m_SceneWndManager->GetCurOutputWnd()->GetCamera().GetCameraRight();

					D3DXVECTOR3 VE = Pos - m_vMouseTargetPosition;
					D3DXVECTOR3 VL = LookAt - m_vMouseTargetPosition;

					//D3DXVECTOR3 N;
					//D3DXVec3Cross(&N,&VE,&VL);

					float cmspeed = 0.003F;
					float fx = (xPos - g_nMouseMoveX) * cmspeed;
					float fy = (yPos - g_nMouseMoveY) * cmspeed;

					D3DXMATRIX mat;
					D3DXMatrixRotationYawPitchRoll(&mat,fx,fy,0);

					D3DXVECTOR3 VEnew,VLnew;
					D3DXVec3TransformCoord(&VEnew,&VE,&mat);
					D3DXVec3TransformCoord(&VLnew,&VL,&mat);

					D3DXVECTOR3 Posnew,LookAtnew;
					Posnew = m_vMouseTargetPosition + VEnew;
					LookAtnew = m_vMouseTargetPosition + VLnew;

					/*m_SceneWndManager->GetCurOutputWnd()->GetCamera().SetPosition(Posnew);
					m_SceneWndManager->GetCurOutputWnd()->GetCamera().SetLookAtPosition(LookAtnew);*/

					m_SceneWndManager->GetCurOutputWnd()->GetCamera().SetPositionLookAtUp(&Posnew, &LookAtnew, NULL);
				}
			}
		}
		break;
	case EXEACTION_INVERT_ACTION: //反向操作，需要判断当前的操作状态决定作什么事情。
		{
			InvertCurrentOperation(nExecState);
		}
		break;
	case EXEACTION_GO_FORWARD://前进
		if (!m_bEditing){
			m_pRepresentNPC->SetNpcMoveBits(KG3DRepresentNPC::nmsFoward, nExecState);
		}
		else
			SetCamareState(KG3DSceneSceneEditor::cmsFoward, nExecState);
		break;
	case EXEACTION_GO_BACKWARD://后退
		if (!m_bEditing){
			m_pRepresentNPC->SetNpcMoveBits(KG3DRepresentNPC::nmsBack, nExecState);
		}
		else
			SetCamareState(KG3DSceneSceneEditor::cmsBack, nExecState);
		break;
	case EXEACTION_TURN_LEFT://左转
		if (!m_bEditing){
			m_pRepresentNPC->SetNpcMoveBits(KG3DRepresentNPC::nmsTurnLeft, nExecState);
		}
		else
			nRet = 0;//需要放行给WINDOWS处理
		break;
	case EXEACTION_TURN_RIGHT://右转
		if (!m_bEditing){
			m_pRepresentNPC->SetNpcMoveBits(KG3DRepresentNPC::nmsTurnRight, nExecState);
		}
		else
			nRet = 0;//需要放行给WINDOWS处理
		break;
	case EXEACTION_MOV_LEFT://左移
		if (!m_bEditing){
			m_pRepresentNPC->SetNpcMoveBits(KG3DRepresentNPC::nmsMoveLeft, nExecState);
		}
		else
			SetCamareState(KG3DSceneSceneEditor::cmsMoveLeft, nExecState);
		break;
	case EXEACTION_MOV_RIGHT://右移
		if (!m_bEditing){
			m_pRepresentNPC->SetNpcMoveBits(KG3DRepresentNPC::nmsMoveRight, nExecState);
		}
		else
			SetCamareState(KG3DSceneSceneEditor::cmsMoveRight, nExecState);

		break;
	case EXEACTION_MOV_UP://上升
		if (m_bEditing)
		{
			SetCamareState(KG3DSceneSceneEditor::cmsCamareUp, nExecState);
		}
		else // P模式下是左旋 add by suntao
		{
			m_pRepresentNPC->SetNpcMoveBits(KG3DRepresentNPC::nmsTurnBodyLeft, nExecState);
		}
		break;
	case EXEACTION_MOV_DOWN://下降
		if (m_bEditing)
		{
			SetCamareState(KG3DSceneSceneEditor::cmsCamareDown, nExecState);
		}
		else // P模式下是右旋 add by sutnao
		{
			m_pRepresentNPC->SetNpcMoveBits(KG3DRepresentNPC::nmsTurnBodyRight, nExecState);
		}
		break;
	case EXEACTION_CAM_CIRCLE://转动摄像机
		if (!m_bEditing){
			m_pRepresentNPC->SetNpcMoveBits(KG3DRepresentNPC::nmsCamareTurn, nExecState);
		}
		else
			nRet = 0;//需要放行给WINDOWS处理
		break;
	case EXEACTION_JUMP://跳
		if (!m_bEditing){
			m_pRepresentNPC->SetNpcMoveBits(KG3DRepresentNPC::nmsJump, nExecState);
		}
		else
			nRet = 0;//需要放行给WINDOWS处理		
		break;
	case EXEACTION_TURN_LEFT_VIEW_FOLLOWED://转身同时转动摄像机
		if (!m_bEditing){
			m_pRepresentNPC->SetNpcMoveBits(KG3DRepresentNPC::nmsTurnBodyLeft, nExecState);
		}
		else
			nRet = 0;//需要放行给WINDOWS处理
		break;
	case EXEACTION_TURN_RIGHT_VIEW_FOLLOWED://转身同时转动摄像机
		if (!m_bEditing){
			m_pRepresentNPC->SetNpcMoveBits(KG3DRepresentNPC::nmsTurnBodyRight, nExecState);
		}
		else
			nRet = 0;//需要放行给WINDOWS处理
		break;
	case EXEACTION_PRESURE://调整压感
		{
			if(this->GetEditState()==SCENESTATE_TERRAIN)
			{
				switch(g_cTerrainBlockModifier.GetTerrainState())
				{
				case MAP_PAINT_GROUND:
					g_cTerrainBlockModifier.m_Brush.nPaintTexturePower = GetValidValue(g_cTerrainBlockModifier.m_Brush.nPaintTexturePower, -nYOff);					
					break;
				default:
					g_cTerrainBlockModifier.m_fAlpha = GetValidValue(g_cTerrainBlockModifier.m_fAlpha, -nYOff);
				}
			}
			m_bTerrainPainting = FALSE;
		}
		break;
	case EXEACTION_INNER_CIRCLE:
		{   //调整内圈
			if(this->GetEditState()==SCENESTATE_TERRAIN )
			{
				g_cTerrainBlockModifier.m_Brush.wInsideHeightBrushSize = GetValidValue(g_cTerrainBlockModifier.m_Brush.wInsideHeightBrushSize, nXOff);
				if(g_cTerrainBlockModifier.m_dwState != MAP_PAINT_GROUND)
					m_bTerrainPainting = FALSE;
			}
		}
		//转到下一个开始调整外圈 内外圈同时调整
	case EXEACTION_OUTTER_CIRCLE://调整外圈
		{
			if(this->GetEditState()==SCENESTATE_TERRAIN)
			{
				//switch(g_cTerrainBlockModifier.m_dwState)
				//{
				//case MAP_PAINT_GROUND:
				//	{	//地表贴图刷大小
				//		g_cTerrainBlockModifier.m_Brush.wOuterHeightBrushSize = GetValidValue(g_cTerrainBlockModifier.m_Brush.wOuterHeightBrushSize, nXOff);
				//		g_cTerrainBlockModifier.m_Brush.ReSizeHeightData((WORD)g_cTerrainBlockModifier.m_Brush.wOuterHeightBrushSize);
				//	}
				//	break;
				//default:
				{   //升降地形的刷子大小
					g_cTerrainBlockModifier.m_Brush.wOuterHeightBrushSize = GetValidValue(g_cTerrainBlockModifier.m_Brush.wOuterHeightBrushSize, nXOff);
					//内圈不能大于外圈
					g_cTerrainBlockModifier.m_Brush.wInsideHeightBrushSize = 
						min(g_cTerrainBlockModifier.m_Brush.wInsideHeightBrushSize , g_cTerrainBlockModifier.m_Brush.wOuterHeightBrushSize);
					g_cTerrainBlockModifier.m_Brush.ReSizeHeightData((WORD)g_cTerrainBlockModifier.m_Brush.wOuterHeightBrushSize);
				}
				//}
				//m_bTerrainPainting = FALSE;
			}
		}
		break;
	case EXEACTION_LEFE_KEY_DOWN://点选键按下
	case EXEACTION_SELECTION_ADD://在当前选择范围中，再增加选择内容
	case EXEACTION_DE_MOVE_ANEAR_GROUND: // 物件帖地表
	case EXEACTION_SELECTION_SUTRACT://在当前选择范围中，再减少选择内容
		{
			nRet = ExecActionSelect(nActionID, nExecState, nXOff, nYOff);
		}
		break;
	case EXEACTION_SMOOTH_TERRAIN://平滑地表
		if(this->GetEditState()==SCENESTATE_TERRAIN)
		{
			switch(g_cTerrainBlockModifier.GetTerrainState())
			{
			case GROUND_UP:
			case GROUND_DOWN:
			case GROUND_NOISE:
			case GROUND_SET:
			case GROUND_GET:
				m_dwPreState = g_cTerrainBlockModifier.GetTerrainState();//记住最近一次的刷动作
				break;
			case GROUND_FLAT:
				if (nExecState && !m_bTerrainPainting) //只在进入功能时记录一次
					m_dwPreState = g_cTerrainBlockModifier.GetTerrainState();
				break;
			}
			//释放平滑动作后，恢复到平滑前的刷动作
			if (nExecState)
			{
				g_cTerrainBlockModifier.SetTerrainState(GROUND_FLAT);
				m_bTerrainPainting = TRUE;
			}
			else
			{
				g_cTerrainBlockModifier.SetTerrainState(m_dwPreState);
				m_bTerrainPainting = FALSE;
			}
		}
		break;
	case EXEACTION_MOUSE_MOVE:
		{
			/*	//<Selecting Refactor> //2009-1-14 放到KG3DSceneSelectBase里面了
			DWORD dwCurTime = g_cGraphicsTool.GetNowTime();
			static DWORD dwLastTime = dwCurTime;
			if(dwCurTime - dwLastTime < 25)	//每25毫秒才响应一次MouseMove，不然消息太频繁会造成帧速率过低
				break;

			dwLastTime = dwCurTime;

			int xPos = LOWORD(lParam); 
			int yPos = HIWORD(lParam);		

			xPos = xPos > 0x7fff ? (xPos - 0xffff) : xPos;
			yPos = yPos > 0x7fff ? (yPos - 0xffff) : yPos;


			if(m_bSelecting)
			{
				if(m_SceneWndManager->GetCurOutputWnd() && IsSelecting())
				{
					D3DXVECTOR3 RayOrig,RayDir;
					D3DXVECTOR2 RayOrig2D;
					m_SceneWndManager->GetCurOutputWnd()->GetPickRay(&RayOrig,&RayDir,&RayOrig2D);
					SelectFrameMove(RayOrig, RayDir, RayOrig2D);
				}
			}*/

			//this->OnMouseMove(wParam, lParam);
			this->SyncPickRay();
		}
		break;
	case EXEACTION_SPEED_UP://加速
		if (nExecState)
		{
			if (m_bEditing)
			{
				g_eEngineOption.nFlySpeedX += 5;
				g_eEngineOption.nFlySpeedY += 5;
			}
			else{
				m_pRepresentNPC->m_fMoveSpeed += 50.0f;
				m_pRepresentNPC->m_fMoveSpeed = min(10000.0f, m_pRepresentNPC->m_fMoveSpeed);
			}
		}
		break;
	case EXEACTION_SPEED_DOWN://减速
		if (nExecState)
		{
			if (m_bEditing)
			{
				g_eEngineOption.nFlySpeedX -= 5;
				g_eEngineOption.nFlySpeedY -= 5;
				g_eEngineOption.nFlySpeedX = max(g_eEngineOption.nFlySpeedX, 5);
				g_eEngineOption.nFlySpeedY = max(g_eEngineOption.nFlySpeedY, 5);
			}
			else
			{
				m_pRepresentNPC->m_fMoveSpeed -= 50.0f;
				m_pRepresentNPC->m_fMoveSpeed = max(100.0f, m_pRepresentNPC->m_fMoveSpeed);
			}
		}
		break;
	default:	//注意这里是没有处理的才交给Base处理，否则BoardCastAction会调用两次
		nRet = KG3DSceneEditorBase::ExecAction(nActionID, nExecState, wParam, lParam);	
	}
	//
	//g_nMouseMoveX = xPos;
	//g_nMouseMoveY = yPos;

	BoardCastAction(nActionID, nExecState, wParam, lParam);
	}
Exit0:
	return nRet;
}





void KG3DSceneSceneEditor::FindLoadedNpc(vector<IEKG3DRepresentObject *>&npcs, DWORD templateid)
{
	SceneLoadedNpcVistor vistor;
	vistor.m_Tid = templateid;
	vistor.m_findObjs = &npcs;
	Traverse(vistor);
}

HRESULT SceneLoadedNpcVistor::Accept(KG3DRepresentObject& Obj)
{
	if(Obj.GetType() == REPRESENTOBJECT_NPC)
	{
		KG3DRepresentNPC *npc = static_cast<KG3DRepresentNPC *>(&Obj);
		//当前npc的templateid
		DWORD _tid = 0;
		npc->GetTemplateID(&_tid);
		if(m_Tid ==  _tid)
		{
			m_findObjs->push_back(npc);
		}
	}
	return S_OK;
}


//按templateid查找场景中已经加载的doodad
void KG3DSceneSceneEditor::FindLoadedDoodad(vector<IEKG3DRepresentObject *>&doodads, DWORD templateid)
{
	SceneLoadedDoodadVistor vistor;
	vistor.m_Tid = templateid;
	vistor.m_findObjs = &doodads;
	Traverse(vistor);
}


HRESULT SceneLoadedDoodadVistor::Accept(KG3DRepresentObject& Obj)
{
	if(Obj.GetType() == REPRESENTOBJECT_DOODAD)
	{
		KG3DRepresentDoodad *doodad = static_cast<KG3DRepresentDoodad *>(&Obj);
		//当前npc的templateid
		DWORD _tid = 0;
		doodad->GetTemplateID(&_tid);
		if(m_Tid ==  _tid)
		{
			m_findObjs->push_back(doodad);
		}
	}
	return S_OK;
}



//更改场景中的实体 by likan
void KG3DSceneSceneEditor::ReplaceSelectedEntity(bool all)
{

	//std::list<KG3DSceneEntity> &ls1 =  m_CtEntities;
	//std::list<KG3DSceneEntity> &ls2 =  m_listRenderEntity;

	TypeEntityAccessProxy& proxy = this->GetSelectionTool().GetSelectionAccessProxy();
	
	
	vector<UINT> selectedEntityIds;
	
	
	for(proxy.Begin(); ! proxy.IsEnd(); proxy.StepForward())
	{
		KG3DSceneEntity& entityRef = proxy.GetCur();

		SCENEENTITYTYPE  type;
        entityRef.GetType(&type);
		//限定可替换的类型
		if(type != SCENEENTITY_OBJECT && type != SCENEENTITY_MODEL && type != SCENEENTITY_MODELSUBSET)
		{
			char msg[256];
			entityRef.GetName(msg, 256);
			strcat(msg, "不能被替换");
		    MessageBox(0, msg, "警告", 0);
			continue;
		}

		KG3DRepresentObject* pObject = (KG3DRepresentObject*)entityRef.m_pPointer;
		assert(pObject);
		int id = pObject->GetHandleHeldBySpaceMgr();
        selectedEntityIds.push_back(id);
	}
 
	if(selectedEntityIds.size() == 0)
	{
		MessageBox(0, "没有选中场景对象", "警告", 0);
		return;
	}

	KG3DRepresentObject* pObj = NULL;
	this->GetObjectInHandle(reinterpret_cast<IEKG3DRepresentObject**>(&pObj));

	if(!pObj)
	{
       MessageBox(0, "没有选中列表中的模型","警告", 0);
	   return;
	}

	int nOK = MessageBox(0, "确定要替换吗？","提示信息",MB_OKCANCEL   |   MB_ICONQUESTION);
	if(nOK == IDOK)
	{
	  vector<UINT> allNeedReplaceHandles;
	  KG3DSceneOutDoorSpaceMgr* p = GetOutDoorSpaceMgr();
	  p->ReplaceEntity(selectedEntityIds, pObj, all, allNeedReplaceHandles);

	  if(!all)
	  {
		  for(vector<UINT>::iterator it = selectedEntityIds.begin(); it!= selectedEntityIds.end(); it ++)
		  {
		    UINT &id = *it;
			for(proxy.Begin(); ! proxy.IsEnd(); proxy.StepForward())
			{
				KG3DSceneEntity& entityRef = proxy.GetCur();
				
				SCENEENTITYTYPE  type;
				entityRef.GetType(&type);
				//限定可替换的类型
				if(type != SCENEENTITY_OBJECT && type != SCENEENTITY_MODEL && type != SCENEENTITY_MODELSUBSET)
					continue;

				KG3DRepresentObject* pObject = (KG3DRepresentObject*)entityRef.m_pPointer;
				assert(pObject);
				UINT _id = pObject->GetHandleHeldBySpaceMgr();
				if(_id == id)
				{
					p->ReloadEntiyInShell(id, &pObject);
					entityRef.m_pPointer = pObject; //重新加载了，并更新选择列表中Entity里面包含的Object指向
					break;
				}
			}
		  }
	  }
	  else
	  {
           SceneReLoadEntityVistor vistor;
		   vistor.handles = allNeedReplaceHandles;
		   vistor.m_lpSceneOutDoorSpaceMgr = p;
		   vistor.proxy = &proxy;
		   Traverse(vistor);
	  }
      //替换完成，设置为场景选择状态，这样能取消对模型列表的选择
	  SetEditState(SCENESTATE_SELECT);
	}
}



//2009-10-23 by likan
HRESULT SceneReLoadEntityVistor::Accept(KG3DRepresentObject& Obj)
{
	UINT id = Obj.GetHandleHeldBySpaceMgr(); //当前实体的id是否在更新列表中？
	for(vector<UINT>::iterator it = handles.begin(); it!= handles.end(); it ++)
	{
		UINT &_id = *it;
		if(id == _id)
		{
		  bool selected = false;
		 
		  //查看被替换的是否正被选中，如果选中了，还要更换内部的Obj指针
		  for(proxy->Begin(); ! proxy->IsEnd(); proxy->StepForward())
		  {
             KG3DSceneEntity& entityRef = proxy->GetCur();
			 
			 SCENEENTITYTYPE  type;
			 entityRef.GetType(&type);
			 //限定可替换的类型
			 if(type != SCENEENTITY_OBJECT && type != SCENEENTITY_MODEL && type != SCENEENTITY_MODELSUBSET)
				 continue;

			 KG3DRepresentObject* pObj = (KG3DRepresentObject*)entityRef.m_pPointer;
			 assert(pObj);
			 UINT idd = pObj->GetHandleHeldBySpaceMgr();
			 if( idd == id)
			 {	
				KG3DRepresentObject* pnewObject = NULL;
				//这种操作可能会带来风险，如果存在其他的entity拷贝，内部obj指针没有更新，则会有这个问题，但目前编辑器只有select列表存在这样的拷贝，对此列表的指针进行了更换
				//目前编辑器其他的拷贝列表都已经不再使用了，目前只能暂时这样处理了
				m_lpSceneOutDoorSpaceMgr->ReloadEntiyInShell(id, &pnewObject);
				entityRef.m_pPointer = pnewObject; //重新加载了，并更新选择列表中Entity里面包含的Object指向
				selected = true;
				break;
			 }
		  }
		  //选择列表中没有，直接更换就好
		  if(!selected)
		  {
			  KG3DRepresentObject* pnewObject = NULL;
			  m_lpSceneOutDoorSpaceMgr->ReloadEntiyInShell(id, &pnewObject);
		  }

		}
	}
	return S_OK;
}


void KG3DSceneSceneEditor::SyncPickRay()
{
	//设置引擎中的当前同步的光标点

	if (! m_bEditing && this->GetEditState() != SCENESTATE_TERRAIN)
		return;

	_ASSERTE(! g_bClient && _T("绝对不能在客户端使用这个函数，这个函数为了编辑方便，精度很高，会拖慢帧数"));
	if(NULL == m_SceneWndManager->GetCurOutputWnd())
		return;

	D3DXVECTOR3 vSrc, vNormalizedDir;
	HRESULT hr = m_SceneWndManager->GetCurOutputWnd()->GetPickRay(&vSrc, &vNormalizedDir, NULL);
	if(FAILED(hr))
		return;

	g_cTerrainBlockModifier.HandlePickOperation(*this, vSrc, vNormalizedDir);	
}
HANDLE KG3DSceneSceneEditor::AddSceneSigns(const D3DXVECTOR3& vPos)
{
	static KG3DModel* pModel = NULL;

    if (!pModel)
    {
        const char* fileName = "\\data\\public\\TagEditor\\Pattern.Mesh";
        char filePath[MAX_PATH];

        g_GetFullPath(filePath, fileName);
        g_cModelTable.LoadResourceFromFile(filePath, 0, MLO_MULTI, (IKG3DResourceBase**)&pModel);
    }

    if (pModel == NULL)
        return 0;

    D3DXVECTOR3 pos = vPos;
    D3DXVECTOR3 scl = D3DXVECTOR3(10.f, 10.f, 10.f);
    pModel->SetScaling(&scl);
    pModel->SetTranslation(&pos);
    pModel->m_bSleep = FALSE;
    pModel->FrameMove();
	pModel->SetPhysicsScene(m_lpPhysicsScene);

    KG3DSceneEntity Entity(SCENEENTITY_MODEL,pModel);

    m_lpSceneOutDoorSpaceMgr->InputModel(pModel);

    /*if (pModel->m_nIsSockWave)
    {
        m_listPostRenderEntity.push_back(Entity);
        if (pModel->m_dwShockMode == SHOCK_WAVE_ADD)
            m_listEntityRenderLast.push_back(Entity);
    }
    else*/
        
	m_listEntityRenderLast.push_back(Entity);

    //pModel->m_bSelectable = bSelectable;

	m_EntityMgr.m_bNeedReGetVisibleEntity = TRUE;

	return (HANDLE)pModel;
}

void KG3DSceneSceneEditor::DelSceneSings(HANDLE Handle)
{
	if (Handle == 0)
		return;

	KG3DModel* pModel = (KG3DModel*)Handle;

	RemoveRenderEntity(pModel, 0, FALSE);

}

HRESULT KG3DSceneSceneEditor::AddPvsPortal()
{
	KG3DRepresentObjectPVS* pvs = NULL;

	if (GetEditSpaceNode() != m_lpSceneOutDoorSpaceMgr)
		pvs = (KG3DRepresentObjectPVS*)GetEditSpaceNode();

	if (!pvs)
	{
		KG3DSceneSelectionTool& selTool = this->GetSelectionTool();
		if (/*m_listSelectedEntity.size() */ selTool.GetSelectionCount() > 0)
		{
			//KG3DSceneEntity e = *m_listSelectedEntity.begin();
			TypeEntityAccessProxy& proxy = selTool.GetSelectionAccessProxy();
			proxy.Begin();

			KG3DSceneEntity& entity = proxy.GetCur();
			if (entity.GetType() == SCENEENTITY_OBJECT)
			{
				KG3DRepresentObject* obj = (KG3DRepresentObject*)entity.m_pPointer;
				if (obj->GetType() == REPRESENTOBJECT_PVS)
					pvs = (KG3DRepresentObjectPVS*)obj;
			}
		}
	}

	if (!pvs)
		return E_FAIL;

	D3DXVECTOR3 vPos = D3DXVECTOR3(0.f, 500.f,  0.f);
	D3DXVECTOR3 vDir = D3DXVECTOR3(0.f, 0.f, -1.f);
	AddPvsPortal(pvs, vPos, vDir ,1000.f, 1000.f);

	return S_OK;
}

HRESULT KG3DSceneSceneEditor::AddPvsPortal(IEKG3DRepresentPVS* pPvs, 
										   const D3DXVECTOR3&  vPos,
										   const D3DXVECTOR3&  vDir, 
										   float fWidth, 
										   float fHeight
										   )
{
	/*
	* this function just call by scene editor
	*/

	if (!pPvs)
		return E_FAIL;

	KG3DPvsPortal* pPortal = NULL;
	((KG3DRepresentObjectPVS*)pPvs)->AddPortal(pPortal, vPos, vDir, fWidth, fHeight);

	/*
	* for scene editor, we need edit the portal
	*/

	/*KG3DSceneEntity e(SCENEENTITY_PVS_PORTAL, pPortal);
	this->GetSelectionTool().SetSelectable(e, TRUE);*///<Selectable Refactor>
	this->IEInsertEntitySelectable(SCENEENTITY_PVS_PORTAL, pPortal, TRUE);

	UpdateEditSpaceNodeInformation();

	return S_OK;
}



HRESULT KG3DSceneSceneEditor::ResetSelectTerrain()
{	
	m_dwSceneSelectionState = 0;//0未选择，1确定左上角，2确定右下角
	m_vPositionSceneSelectionA = D3DXVECTOR3(0,0,0);
	m_vPositionSceneSelectionC = D3DXVECTOR3(0,0,0);
	return S_OK;
}

HRESULT  KG3DSceneSceneEditor::SaveTerrainBlock(const char cszFileName[])
{
	if (m_pTerrainBlock)
		m_pTerrainBlock->SaveBlockToFile(cszFileName);
	return S_OK;
}
HRESULT  KG3DSceneSceneEditor::LoadTerrainBlock(const char cszFileName[])
{	
	if (!m_pTerrainBlock)
	{
		m_pTerrainBlock = new KG3DRepresentTerrainBlock;
		m_pTerrainBlock->AddRef();
		m_pTerrainBlock->SetType(REPRESENTOBJECT_TERRAINBLOCK);
		m_pTerrainBlock->SetTerrainPoint(m_lpSceneOutDoorSpaceMgr);
		//m_pTerrainBlock->SetObjectList(&m_vecCopyObject,&m_vecPosToCopyTerrain);
		/*
		AddRenderEntityEx(SCENEENTITY_OBJECT,m_pTerrainBlock);
		AddSelectableEntityEx(SCENEENTITY_OBJECT,m_pTerrainBlock);
		AddSelectedEntityEx(SCENEENTITY_OBJECT,m_pTerrainBlock);*/
	}

	if(m_pTerrainBlock)
	{
		//AddRepresentObject(m_pTerrainBlock,TRUE,TRUE,FALSE);
		AddRepresentObject(m_pTerrainBlock, EM_SCENE_ADD_OPTION_SELECTABLE | EM_SCENE_ADD_OPTION_SELECTED, NULL);
	}

	IEKG3DSceneOutputWnd* pWindow; 
	RECT rect;
	GetCurOutputWnd(&pWindow);
	pWindow->GetRect(&rect);
	D3DXVECTOR2 vec2ScreenPos;
	vec2ScreenPos.x = (rect.left + rect.right) / 2.0F;
	vec2ScreenPos.y = (rect.bottom + rect.top) / 2.0F; 
	D3DXVECTOR3 vec3CenterPos;
	GetSelectPosition(vec2ScreenPos,vec3CenterPos);
	vec3CenterPos.y += 500;
	m_pTerrainBlock->SetTranslation(&vec3CenterPos);
	m_pTerrainBlock->SetRotation(&D3DXQUATERNION(0,0,0,1));
	m_pTerrainBlock->SetRotationCenter(&D3DXVECTOR3(0,0,0));


	/*
	for (size_t i = 0;i < m_vecCopyObject.size(); i++)
	{
	RemoveRenderEntityEx(SCENEENTITY_OBJECT,m_vecCopyObject[i]);
	SAFE_DELETE(m_vecCopyObject[i]);
	}
	m_vecCopyObject.clear();*/
	m_pTerrainBlock->RemoveAllCopyObjectsInScene(*this, TRUE);


	//m_vecPosToCopyTerrain.clear();
	m_pTerrainBlock->ReadBlockFromFile(cszFileName);
	m_pTerrainBlock->UpdateTransformation();


	/*
	for (size_t i = 0;i < m_vecCopyObject.size(); i++)
	AddRenderEntityEx(SCENEENTITY_OBJECT,m_vecCopyObject[i]);*/

	m_pTerrainBlock->AddAllCopyObjectsToScene(*this);

	m_pTerrainBlock->ComputeBBox();
	m_pTerrainBlock->ComputeDynamicBBox();
	return S_OK;
}

HRESULT  KG3DSceneSceneEditor::SelectTerrainFinished()
{
	KG3DRepresentObject* pNewObject = NULL;
	if (m_lpSceneOutDoorSpaceMgr)
		m_lpSceneOutDoorSpaceMgr->CopySelectedTerrain(m_vPositionSceneSelectionA,m_vPositionSceneSelectionC);
	if (!m_pTerrainBlock)
	{
		m_pTerrainBlock = new KG3DRepresentTerrainBlock;
		KG_PROCESS_ERROR(m_pTerrainBlock);
		m_pTerrainBlock->AddRef();
		m_pTerrainBlock->SetType(REPRESENTOBJECT_TERRAINBLOCK);
		m_pTerrainBlock->SetTerrainPoint(m_lpSceneOutDoorSpaceMgr);
	}
	{
		IEKG3DSceneOutputWnd* pWindow; 
		RECT rect;
		GetCurOutputWnd(&pWindow);
		pWindow->GetRect(&rect);
		D3DXVECTOR2 vec2ScreenPos;
		vec2ScreenPos.x = (rect.left + rect.right) / 2.0F;
		vec2ScreenPos.y = (rect.bottom + rect.top) / 2.0F; 
		D3DXVECTOR3 vec3CenterPos;
		GetSelectPosition(vec2ScreenPos,vec3CenterPos);
		vec3CenterPos.y += 500;
		m_pTerrainBlock->SetTranslation(&vec3CenterPos);
		m_pTerrainBlock->SetRotation(&D3DXQUATERNION(0,0,0,1));
		m_pTerrainBlock->SetRotationCenter(&D3DXVECTOR3(0,0,0));

		/*
		for (size_t i = 0;i < m_vecCopyObject.size();i++)
		{
		RemoveRenderEntityEx(SCENEENTITY_OBJECT,m_vecCopyObject[i]);
		SAFE_DELETE(m_vecCopyObject[i]);
		}
		m_vecCopyObject.clear();*/
		m_pTerrainBlock->RemoveAllCopyObjectsInScene(*this, TRUE);

		//m_vecPosToCopyTerrain.clear();//RemoveAllCopyObjectsInScene的时候已经做了
		KG3DSceneEntityList tempEntityList;
		D3DXVECTOR2 vec2A = D3DXVECTOR2(m_vPositionSceneSelectionA.x,m_vPositionSceneSelectionA.z);
		D3DXVECTOR2 vec2B = D3DXVECTOR2(m_vPositionSceneSelectionC.x,m_vPositionSceneSelectionC.z);
		m_lpSceneOutDoorSpaceMgr->GetEntityIntersectRect(&tempEntityList,vec2A,vec2B,FALSE);
		VOID* pvRetPointer = NULL;
		//KG3DRepresentObject* pObject = NULL ;
		D3DXVECTOR3 vec3NowPos;
		KG3DTransformation Transformation;

		D3DXVECTOR3 vecPos = m_vPositionSceneSelectionA + m_vPositionSceneSelectionC;
		vecPos.x = vecPos.x / 2;
		vecPos.y = vecPos.y / 2;
		vecPos.z = vecPos.z / 2;
		KG3DSceneEntityList::iterator t = tempEntityList.begin();
		KG3DSceneEntityList::iterator iend = tempEntityList.end();
		while (t!=iend)
		{
			t->GetPointer(&pvRetPointer);
			KG3DRepresentObject* pObject = (KG3DRepresentObject*)pvRetPointer;
			if (pObject->GetType() != REPRESENTOBJECT_TERRAINBLOCK)
			{
				HRESULT hr = g_GetObjectTable().GetCopy(&pNewObject,pObject);
				KG_COM_PROCESS_ERROR(hr);
				pNewObject->GetTranslation(&vec3NowPos);
				vec3NowPos.x = vec3NowPos.x - vecPos.x ;
				vec3NowPos.z = vec3NowPos.z - vecPos.z;
				pNewObject->SetTranslation(&vec3NowPos);
				Transformation = *((KG3DTransformation*)pNewObject);
				//m_vecPosToCopyTerrain.push_back(Transformation);	//AddCopyObject的时候保存

				m_pTerrainBlock->AddCopyObject(pNewObject);
				//AddRenderEntityEx(SCENEENTITY_OBJECT,pNewObject);	//改成下面的Add
			}
			t++;
		}

		m_pTerrainBlock->AddAllCopyObjectsToScene(*this);

		m_pTerrainBlock->UpdateTransformation();

		//AddRepresentObject(m_pTerrainBlock,TRUE,TRUE,FALSE);
		AddRepresentObject(m_pTerrainBlock, EM_SCENE_ADD_OPTION_SELECTABLE | EM_SCENE_ADD_OPTION_SELECTED, NULL);
		m_pTerrainBlock->AddRef();
		m_pTerrainBlock->AddRef();
	}

	return S_OK;
Exit0:
	SAFE_RELEASE(pNewObject);
	return E_FAIL;
}

HRESULT KG3DSceneSceneEditor::TerrainCopyWindowOnShow()
{
	m_bIsTerrainState = TRUE;
	if (!m_pTerrainBlock)
		return S_OK;

	/*
	AddRenderEntityEx(SCENEENTITY_OBJECT,m_pTerrainBlock);
	AddSelectableEntityEx(SCENEENTITY_OBJECT,m_pTerrainBlock);
	AddSelectedEntityEx(SCENEENTITY_OBJECT,m_pTerrainBlock);*/
	//AddRenderEntity(KG3DSceneEntity(SCENEENTITY_OBJECT, m_pTerrainBlock), TRUE, TRUE);

	//m_pTerrainBlock->UpdateTransformation();
	/*
	for (size_t i = 0;i < m_vecCopyObject.size(); i++)
	AddRenderEntityEx(SCENEENTITY_OBJECT,m_vecCopyObject[i]);*/
	//m_pTerrainBlock->AddAllCopyObjectsToScene(*this);		

	return S_OK;
}

HRESULT KG3DSceneSceneEditor::TerrainCopyWindowOnHide()
{
	m_bIsTerrainState = FALSE;
	if (!m_pTerrainBlock)
		return S_OK;

	/*
	RemoveSelectableEntityEx(SCENEENTITY_OBJECT,m_pTerrainBlock);
	RemoveRenderEntityEx(SCENEENTITY_OBJECT,m_pTerrainBlock);*/
	//RemoveRenderEntity(SCENEENTITY_OBJECT, m_pTerrainBlock);	
	//m_pTerrainBlock->UpdateTransformation();
	/*
	for (size_t i = 0;i < m_vecCopyObject.size(); i++)
	RemoveRenderEntityEx(SCENEENTITY_OBJECT,m_vecCopyObject[i]);*/
	//m_pTerrainBlock->RemoveAllCopyObjectsInScene(*this, FALSE);

	return S_OK;
}

HRESULT KG3DSceneSceneEditor::PasteSeclctTerrain(DWORD dwMixType,bool bIsPasteBlend,bool bIsPasteObject,bool bIsMax,float fWeight,float fWidth)
{
	HRESULT hr = E_FAIL;
	AABBOX bBox;
	if (m_lpSceneOutDoorSpaceMgr)
		m_lpSceneOutDoorSpaceMgr->PasteSelectedTerrain(dwMixType,bIsPasteBlend,bIsMax,fWeight,fWidth,&bBox);

	KG3DSceneEntityList tempEntityList;
	VOID* pvRetPointer = NULL;
	D3DXVECTOR2 vec2A = D3DXVECTOR2(bBox.A.x,bBox.A.z);
	D3DXVECTOR2 vec2B = D3DXVECTOR2(bBox.B.x,bBox.B.z);

	//从地形中把物件找出来，对于每个物件运行一次复制
	m_lpSceneOutDoorSpaceMgr->GetEntityIntersectRect(&tempEntityList,vec2A,vec2B,FALSE);
	if (bIsPasteObject)
	{		
		std::set<KG3DRepresentObject*> setObj;
		KG3DSceneEntityList::iterator t = tempEntityList.begin();
		KG3DSceneEntityList::iterator iend = tempEntityList.end();
		while (t!=iend)
		{
			t->GetPointer(&pvRetPointer);
			KG3DRepresentObject* pObj = (KG3DRepresentObject*)pvRetPointer;
			setObj.insert(pObj);
			t++;
		}
		std::set<KG3DRepresentObject*>::iterator iSet = setObj.begin();
		std::set<KG3DRepresentObject*>::iterator iSetEnd = setObj.end();
		while (iSet != iSetEnd)
		{
			if ((*iSet)->GetType() != REPRESENTOBJECT_TERRAINBLOCK)
			{
				D3DXVECTOR3 vecPos;
				(*iSet)->GetTranslation(&vecPos);
				int nPosFlag = m_lpSceneOutDoorSpaceMgr->PosInWhichArea(vecPos,fWidth);
				if (nPosFlag == 2)
				{
					//RemoveRenderEntityEx(SCENEENTITY_OBJECT,pObject);
					//RemoveSelectableEntityEx(SCENEENTITY_OBJECT,pObject);
					RemoveRepresentObject((*iSet));//Last modify by Chen TianHong:2008-4-17 17:12:44原来是用这个函数的，但这里真的是用这个函数吗？pObj会析构的
				}
				else if (nPosFlag == 1)
				{
					vecPos.y = GetHeight(vecPos.x,vecPos.z);
					(*iSet)->SetTranslation(&vecPos);
				}
			}
			iSet++;
		}
		KG3DRepresentObject* pObject = NULL;
		D3DXVECTOR3 vecPos;
		size_t uCount = m_pTerrainBlock->GetCopyObjectCount();
		for (size_t i = 0; i < uCount; i++)
		{
			KG3DRepresentObject* pSrcObj = m_pTerrainBlock->GetCopyObject(i);
			_ASSERTE(pSrcObj);

			hr = g_GetObjectTable().GetCopy(&pObject, pSrcObj);
			if (FAILED(hr))
			{
				continue;
			}

			pObject->GetTranslation(&vecPos);
			vecPos.y = GetHeight(vecPos.x,vecPos.z);
			pObject->SetTranslation(&vecPos);

			//AddRepresentObject(pObject,TRUE,TRUE,FALSE);
			AddRepresentObject(m_pTerrainBlock, EM_SCENE_ADD_OPTION_SELECTABLE | EM_SCENE_ADD_OPTION_SELECTED, NULL);
		}
	}
	else
	{
		KG3DSceneEntityList::iterator t = tempEntityList.begin();
		KG3DSceneEntityList::iterator iend = tempEntityList.end();
		while (t!=iend)
		{
			t->GetPointer(&pvRetPointer);
			KG3DRepresentObject* pObject = (KG3DRepresentObject*)pvRetPointer;
			D3DXVECTOR3 vecPos;
			if (pObject->GetType() != REPRESENTOBJECT_TERRAINBLOCK)
			{
				pObject->GetTranslation(&vecPos);
				int nPosFlag = m_lpSceneOutDoorSpaceMgr->PosInWhichArea(vecPos,fWidth);
				if (nPosFlag != 0)
				{
					vecPos.y = GetHeight(vecPos.x,vecPos.z);
					pObject->SetTranslation(&vecPos);
				}
			}
			t++;
		}
	}
	if(m_EntityMgr.m_lpTerrainRender)
		m_EntityMgr.m_lpTerrainRender->EnableSortTerrainBlock();
	return S_OK;
}

INT KG3DSceneSceneEditor::ExecActionSelect(INT nActionID, INT nExecState, INT nXOff, INT nYOff)
{
	INT nRet = 0;

	BOOL IsLeftButtonDown = FALSE;

	if(this->GetEditState()==SCENESTATE_TERRAIN)
	{
		IsLeftButtonDown = !!nExecState;
		m_bTerrainPainting = !!nExecState;

		if(nActionID == EXEACTION_LEFE_KEY_DOWN)
		{

			if(IsLeftButtonDown)
			{
				if(m_lpSceneOutDoorSpaceMgr)
					g_cTerrainBlockModifier.OnBrushStart(m_lpSceneOutDoorSpaceMgr);
			}
			else
			{
				if(m_lpSceneOutDoorSpaceMgr)
					g_cTerrainBlockModifier.OnBrushEnd(m_lpSceneOutDoorSpaceMgr);
			}
		}

		nRet = 1;
	}	
	else if (this->GetEditState() == SCENESTATE_ADDPOLYVERTEX && nExecState )
	{
		if (m_lpMainWindow)
		{
			D3DXVECTOR3 Inter(0, 0, 0);
			D3DXVECTOR3 vSrc;
			D3DXVECTOR3 vNormalizedDir;
			m_lpMainWindow->GetPickRay(&vSrc, &vNormalizedDir, NULL);

			BOOL bFoundInter = FALSE;
			//新的碰撞接口
			if (IsUseCrossPlayMode())//连对象也检测
			{
				FLOAT fTempRet = FLT_MAX;
				bFoundInter = ::RayIntersection(*this, vSrc, vNormalizedDir, &fTempRet, NULL, NULL);
				if (bFoundInter)
				{
					Inter = vSrc + vNormalizedDir * fTempRet;
				}
			}
			else
			{
				FLOAT fTempRet = FLT_MAX;
				bFoundInter = ::RayIntersectionTerrain(*this, vSrc, vNormalizedDir, &fTempRet);
				if (bFoundInter)
				{
					Inter = vSrc + vNormalizedDir * fTempRet;
				}
			}

			if (bFoundInter)
			{
				int n = (int) m_vecPolyVertexes.size();
				Inter.y -= 60.0f;
				AddPolyVetex(n,Inter);
			}			
		}
		nRet = 0;//不让后续处理
	}
	else if(this->GetEditState() == SCENESTATE_TERRAINSELECTION)
	{
		IsLeftButtonDown = !!nExecState;

		if ((m_dwSceneSelectionState == 0)&&(IsLeftButtonDown))
		{
			D3DXVECTOR3 vSrc;
			D3DXVECTOR3 vNormalizedDir;
			m_lpMainWindow->GetPickRay(&vSrc, &vNormalizedDir, NULL);
			BOOL bFoundInter = FALSE;
			FLOAT fTempRet = FLT_MAX;
			bFoundInter = ::RayIntersectionTerrain(*this, vSrc, vNormalizedDir, &fTempRet);
			if (bFoundInter)
			{
				m_vPositionSceneSelectionA = vSrc + vNormalizedDir * fTempRet;
			}
			m_vPositionSceneSelectionC = m_vPositionSceneSelectionA;
			m_vPositionSceneSelectionC.y += 500;
			m_dwSceneSelectionState++;	
		}

		if(m_dwSceneSelectionState == 1)
		{
			D3DXVECTOR3 vSrc;
			D3DXVECTOR3 vNormalizedDir;
			m_lpMainWindow->GetPickRay(&vSrc, &vNormalizedDir, NULL);
			BOOL bFoundInter = FALSE;
			FLOAT fTempRet = FLT_MAX;
			bFoundInter = ::RayIntersectionTerrain(*this, vSrc, vNormalizedDir, &fTempRet);
			if (bFoundInter)
			{
				m_vPositionSceneSelectionC = vSrc + vNormalizedDir * fTempRet;
			}
			m_vPositionSceneSelectionC.y += 500;

			if(!IsLeftButtonDown)
			{
				m_dwSceneSelectionState = 0;
				if(m_vPositionSceneSelectionA.x>m_vPositionSceneSelectionC.x)
					swap(m_vPositionSceneSelectionA.x,m_vPositionSceneSelectionC.x);
				if(m_vPositionSceneSelectionA.y>m_vPositionSceneSelectionC.y)
					swap(m_vPositionSceneSelectionA.y,m_vPositionSceneSelectionC.y);
				if(m_vPositionSceneSelectionA.z>m_vPositionSceneSelectionC.z)
					swap(m_vPositionSceneSelectionA.z,m_vPositionSceneSelectionC.z);				
				CheckTerrianSelection();
				//m_dwEditState = SCENESTATE_TERRAINSELECTIONMODIFY;
				this->SetEditState(SCENESTATE_TERRAINSELECTIONMODIFY);
			}
		}

		nRet = 0;//不让后续处理
	}
	else if (this->GetEditState() == SCENESTATE_PLACEUNIT && !nExecState)
	{
		nRet = 1;//需要后续处理, NPC摆放
	}
	else if (this->GetEditState() == SCENESTATE_COPY_REPRESENTOBJECT) // 放置复制的对象 add by suntao
	{		
		IsLeftButtonDown = !!nExecState;

		if (nActionID == EXEACTION_LEFE_KEY_DOWN && IsLeftButtonDown)
		{
			DoPlaceObject();
		}		

		// 不需要后续处理
		nRet = 0;
	}
	else
	{			
		// 策划场景编辑器
		if (nActionID == EXEACTION_DE_MOVE_ANEAR_GROUND)
		{
			if (g_bDesign && SCENESTATE_MOVE == this->GetEditState()) // 移动过程中贴地
			{				
				D3DXVECTOR3 oldpos, newpos;

				KG3DSceneSelectionTool& selTool = this->GetSelectionTool();
				TypeEntityAccessProxy& proxy = selTool.GetSelectionAccessProxy();
				for (proxy.Begin(); ! proxy.IsEnd(); proxy.StepForward())
				{
					KG3DSceneEntity& entity = proxy.GetCur();
					entity.GetTranslation(&oldpos);
					newpos = oldpos;
					this->GetFloor(&oldpos, &newpos.y);
					entity.SetTranslation(&newpos);
				}
				
				// 和放置NPC一样的处理消息
				this->SetEditState(SCENESTATE_PLACEUNIT);
				nRet = 1;
			}
			else
			{
				nRet = KG3DSceneEditorBase::ExecActionSelect(nActionID, nExecState, nXOff, nYOff);	
			}
		}					
	}
	//Exit0:
	return nRet;
}
//代码移动到OnReceiveMessage
//HRESULT KG3DSceneSceneEditor::SelectEnd(D3DVIEWPORT9& Viewport,D3DXMATRIX& Projection,D3DXMATRIX& View)
//{
//	KG3DSceneEditorBase::SelectEnd(Viewport,Projection,View);
//	
//	TypeEntityAccessProxy& proxy = this->GetSelectionTool().GetSelectionAccessProxy();
//	for(proxy.Begin(); ! proxy.IsEnd(); proxy.StepForward())
//	{
//		KG3DSceneEntity Entity = proxy.GetCur();
//
//		if (Entity.GetType()==SCENEENTITY_OBJECT)
//		{
//
//			KG3DRepresentObject* pObject = 
//				static_cast<KG3DRepresentObject*>(Entity.m_pPointer);
//
//			if(pObject->GetType()==REPRESENTOBJECT_NODE)
//			{
//				KG3DRepresentObjectNode* pNode = static_cast<KG3DRepresentObjectNode*>(pObject);
//				if (m_lpSceneOutDoorSpaceMgr && m_lpSceneOutDoorSpaceMgr->m_lpRoadMgr)
//				{
//					if (pNode->GetNodeType() == NODETYPE_ROAD)
//					{
//						KG3DSerpentine* pRoad;
//						pNode->GetPointer((PVOID*)&pRoad);
//						DWORD i = m_lpSceneOutDoorSpaceMgr->m_lpRoadMgr->GetSerpentineIndex(pRoad);
//						FireEvent2(EVENT_ROADNODE_SELECTED,0,i,0);
//						break;
//					}
//				}	
//			}
//		}
//		//i++;
//	}
//	return S_OK;
//}
void KG3DSceneSceneEditor::InvertCurrentOperation(INT nInvert)
{
	if(this->GetEditState()==SCENESTATE_TERRAIN)
	{
		if(m_lpSceneOutDoorSpaceMgr)
		{
			switch(g_cTerrainBlockModifier.GetTerrainState())
			{
			case GROUND_UP:
			case GROUND_DOWN:
				g_cTerrainBlockModifier.SetInvertOperation(nInvert);
				m_bTerrainPainting = !!nInvert;
				break;
			}
		}
	}
}

HRESULT KG3DSceneSceneEditor::AddOutputWindow(LPCTSTR szName, HWND hWnd, DWORD dwType, DWORD_PTR dwOption, int *pOutputWindowID)
{
	HRESULT hr = E_FAIL;
	if (dwType == OUTPUTWND_SCENEEDITOR_TEXTUREVIEW)///<CTH>
	{
		OUTPUTWND_TEXTURE_PARAM* pOTParam = (OUTPUTWND_TEXTURE_PARAM*)dwOption;
		KG_PROCESS_ERROR(pOTParam);
		KG3DTexture* pTemp = NULL;
		hr = g_cTextureTable.GetResource(&pTemp, pOTParam->dwTextureID);
		KG_COM_PROCESS_ERROR(hr);
		KG_PROCESS_ERROR(pTemp);
		pTemp->Release();
	}
	hr = KG3DSceneEditorBase::AddOutputWindow(szName,hWnd,dwType,dwOption,pOutputWindowID);

	if(dwType==OUTPUTWND_SCENEEDITOR_MAIN)
	{
		KG3DSceneOutputWnd *pWnd = static_cast<KG3DSceneOutputWnd *>(m_SceneWndManager->GetOutputWindow(*pOutputWindowID));
		if(pWnd)
		{

			if (g_cRecorder.IsRecord() && m_dwRecordID != 0 && !g_cRecorder.IsLock())
			{
				KG3DRecorder::Parameters MyParameters;
				MyParameters.nNum = 2;
				MyParameters.size_tDataSize = MAX_PATH * sizeof(char) + sizeof(DWORD) + sizeof(DWORD);
				char* pData = new char[MyParameters.size_tDataSize];
				MyParameters.pData = pData;
				MyParameters.nFrom[0] = 0;
				strcpy(&pData[MyParameters.nFrom[0]],szName);
				MyParameters.nFrom[1] = MAX_PATH * sizeof(char) ;
				pWnd->GetCamera().m_dwRecordID = KG3DRecorder::GetMasterID();
				*(DWORD*)(&pData[MyParameters.nFrom[1]]) = pWnd->GetCamera().m_dwRecordID;
				/*
				MyParameters.nFrom[2] = (strlen(szName) + 1) * sizeof(char) + sizeof(DWORD);
				*(DWORD*)(&pData[MyParameters.nFrom[2]]) = dwType;*/
				g_cRecorder.RecordEvent(m_dwRecordID,FT_KG3DSCENEEDITOR_ADDOUTPUTWINDOW,
					GetCurrentTime(),MyParameters);
			}
			pWnd->OnResizeWindow(&(pWnd->m_Rect));
			if(g_cEngineOption.bPostEffectEnable)
			{
				RECT r = pWnd->m_Rect;
				DWORD dwWidth = r.right - r.left;
				DWORD dwHeight =  r.bottom - r.top;

				pWnd->ResetDevice(g_cEngineOption.bPostEffectEnable, dwWidth, dwHeight);
	
			}
			if(g_cEngineOption.bMotionBlur)
			{
				pWnd->EnablePostEffect(POST_EFFECT_MOTIONBLUE, g_cEngineOption.bMotionBlur);
			}
			if (g_cEngineOption.ModelShadowType > EM_MODEL_SHADOW_TYPE_LOW)
			{
				pWnd->EnablePostEffect(POST_EFFECT_SHADOW, g_cEngineOption.ModelShadowType> EM_MODEL_SHADOW_TYPE_LOW);
			}

			if(g_cEngineOption.bBloomEnable)
			{
				pWnd->EnablePostEffect(POST_EFFECT_BLOOM, g_cEngineOption.bBloomEnable);
				//PostRender::KG3DPostRenderEffectBloom* pBloom = pWnd->GetBloomEffect();
				//pBloom->SetGate(g_eEngineOption.fBloomGate);
			}

			if(g_cEngineOption.bHDR)
			{
				pWnd->EnablePostEffect(POST_EFFECT_HDR, g_cEngineOption.bHDR);
			}

			if(g_cEngineOption.bGodRay)
			{
				pWnd->EnablePostEffect(POST_EFFECT_GODRAY, g_cEngineOption.bGodRay);

			}

			if(g_cEngineOption.bSSAO)
			{
				pWnd->EnablePostEffect(POST_EFFECT_SSAO, g_cEngineOption.bSSAO);
			}

			if(g_cEngineOption.bGlobalFog)
			{
				pWnd->EnablePostEffect(POST_EFFECT_FOG, g_cEngineOption.bGlobalFog);
			}


			if(g_cEngineOption.bDetail)
			{
				pWnd->EnablePostEffect(POST_EFFECT_DETAIL, g_cEngineOption.bDetail);
			}
			if (g_cEngineOption.bShockWaveEnable)
			{
				pWnd->EnablePostEffect(POST_EFFECT_SOCKWARE, g_cEngineOption.bShockWaveEnable);
			}
			if (g_cEngineOption.bDOF)
			{
				pWnd->EnablePostEffect(POST_EFFECT_DOF, g_cEngineOption.bDOF);
			}
			//Modify by FengBo 2007-9-7
			if (g_cEngineOption.bHSIEnable)
			{
				pWnd->EnablePostEffect(POST_EFFECT_HSI , g_cEngineOption.bHSIEnable);
			}
            if (g_cEngineOption.bCurveCMYK)
                pWnd->EnablePostEffect(POST_EFFECT_CURVE_CMYK, g_cEngineOption.bCurveCMYK);

			pWnd->EnablePostEffect(POST_EFFECT_AOE , FALSE);
			//end of 2007-9-7

			// enable aoe away
			//  pWnd->EnablePostEffect(POST_EFFECT_AOE, TRUE);

/*
			pWnd->SetBloomGate(g_eEngineOption.fBloomGate);
			pWnd->SetBloomDelaye(g_eEngineOption.fBloomDelay);
			pWnd->SetBloomExposure1(g_eEngineOption.fBloomExposure1);
			pWnd->SetBloomExposure2(g_eEngineOption.fBloomExposure2);
			pWnd->SetBloomGaussCoefficient(g_eEngineOption.fBloomGaussCoefficient);
			pWnd->SetfBloomSampleNum(g_eEngineOption.nBloomSampleNum);

            pWnd->SetDetailPower(g_eEngineOption.fDetailPower);
            pWnd->SetBlurPower(g_eEngineOption.fBlurPower);
            pWnd->SetBlurDistance(g_eEngineOption.fBlurDistance);

			pWnd->SetHSIHueScale(g_eEngineOption.fHueScale);
			pWnd->SetHSIHueBias(g_eEngineOption.fHueBias);
			pWnd->SetHSISaturationScale(g_eEngineOption.fSaturationScale);
			pWnd->SetHSISaturationBias(g_eEngineOption.fSaturationBias);
			pWnd->SetHSIIntensityScale(g_eEngineOption.fIntensityScale);
			pWnd->SetHSIIntensityBias(g_eEngineOption.fIntensityBias);

			pWnd->SetShockWavePower(g_eEngineOption.fShockWavePower);

			pWnd->SetGodRaySampleNum(g_eEngineOption.nGodRaySampleNum);
			pWnd->SetGodRayDensity(g_eEngineOption.fGodRayDensity);
			pWnd->SetGodRayWeight(g_eEngineOption.fGodRayWeight);
			pWnd->SetGodRayDecay(g_eEngineOption.fGodRayDecay);
			pWnd->SetGodRayExposure(g_eEngineOption.fGodRayExposure);

			pWnd->SetSSAOLevel(g_eEngineOption.nSSAOLevel);
			pWnd->SetSSAOSampRadius(g_eEngineOption.fSSAOSampleRadius);
			pWnd->SetSSAODarkFact(g_eEngineOption.fSSAODarkFact);
			pWnd->SetSSAOBlurWeight(g_eEngineOption.fBlurWeight);

			
			pWnd->SetFogDensityV(g_eEngineOption.fFogDensityV);
			pWnd->SetFogSlopeThreshold(g_eEngineOption.fSlopeThreshold);
			pWnd->SetFogHeightFalloff(g_eEngineOption.fHeightFalloff);
			pWnd->SetFogcolor(g_eEngineOption.vecFogcolor);
*/

			m_SceneWndManager->SetPrimaryWindow(*pOutputWindowID);

		}///if(pWnd)
	}
	return S_OK;
Exit0:
	return E_FAIL;
}

void KG3DSceneSceneEditor::OnCommand(enumCommandType eCommandType)
{
	switch(eCommandType)
	{		
	case ctInc: //放大缩小当前的笔刷一格			
		ExecAction(EXEACTION_INNER_CIRCLE, 1, 0, MAKELPARAM(g_nMouseMoveX+1, g_nMouseMoveY+1));
		break;
	case ctDec:
		ExecAction(EXEACTION_INNER_CIRCLE, 1, 0, MAKELPARAM(g_nMouseMoveX-1, g_nMouseMoveY-1));
		break;
	case ctHideObject: //隐藏物件
		g_cEngineManager.GetRenderOption( RENDER_ENTITY ) ? g_cEngineManager.SetRenderOption( RENDER_ENTITY ,FALSE) : g_cEngineManager.SetRenderOption( RENDER_ENTITY ,TRUE);
		break;
	case ctHideTerrain://隐藏地表
		g_cEngineManager.GetRenderOption( RENDER_TERRAIN ) ? g_cEngineManager.SetRenderOption( RENDER_TERRAIN ,FALSE) : g_cEngineManager.SetRenderOption( RENDER_TERRAIN ,TRUE);
		break;
	case ctHideGrass:  //隐藏草
		g_cEngineManager.GetRenderOption( RENDER_GRASS ) ? g_cEngineManager.SetRenderOption( RENDER_GRASS ,FALSE) : g_cEngineManager.SetRenderOption( RENDER_GRASS ,TRUE);
		break;
	case ctHideTree:  //隐藏树叶
		g_cEngineManager.GetRenderOption( RENDER_TREE ) ? g_cEngineManager.SetRenderOption( RENDER_TREE ,FALSE) : g_cEngineManager.SetRenderOption( RENDER_TREE ,TRUE);
		break;
	case ctHideNPCAttackRange://隐藏NPC攻击范围
		{
			/*KG3DSceneEntityList::iterator i = m_listSelectedEntity.begin();
			while(i!=m_listSelectedEntity.end())
			{
				KG3DSceneEntity Entity = *i;*/
			TypeEntityAccessProxy& proxy = this->GetSelectionTool().GetSelectionAccessProxy();
			for(proxy.Begin(); ! proxy.IsEnd(); proxy.StepForward())
			{
				KG3DSceneEntity Entity = proxy.GetCur();

				if(Entity.GetType() == SCENEENTITY_OBJECT)
				{
					KG3DRepresentObject* pObject = (KG3DRepresentObject* ) Entity.m_pPointer;
					if(pObject->GetType() == REPRESENTOBJECT_NPC)
					{
						FireEvent2(EVENTEDITORNPCHIDEATTACKRANGE,0,0,pObject);
					}
				}
				//i++;
			}
		}
		break;
	case ctEnd:    //P模式时，复位摄像机到后面
		break;
	case ctPageUp: //改变模型
	case ctPageDown: 
	case ctHome:
		{
			if (m_bEditing)
			{
				static BYTE codemap[][2] ={
					{0, 0},
					{0, 0},
					{0, 0},
					{0, 0},
					{ctPageDown, 2},
					{ctPageUp, 1},
					{ctHome, 3},
				};
				DWORD dwEditorState = this->GetEditState();
				switch(dwEditorState)
				{
				case SCENESTATE_PLACEOBJ:
					{
						KG3DRepresentObject* pObj = NULL;
						HRESULT hr = this->GetObjectInHandle(reinterpret_cast<IEKG3DRepresentObject**>(&pObj));
						if(SUCCEEDED(hr))
						{						
							pObj->ChangeModel(codemap[eCommandType][1]);
						} 
					}
					break;
				case SCENESTATE_SELECT:
					{
						TypeEntityAccessProxy& proxy = this->GetSelectionTool().GetSelectionAccessProxy();
						for(proxy.Begin(); ! proxy.IsEnd(); proxy.StepForward())
						{
							KG3DSceneEntity Entity = proxy.GetCur();

							Entity.ChangeModel(codemap[eCommandType][1]);
							if(SCENEENTITY_OBJECT == Entity.GetType() && m_lpSceneOutDoorSpaceMgr)
							{
								KG3DRepresentObject* pObj = static_cast<KG3DRepresentObject*>(Entity.m_pPointer); 
								if(pObj && pObj->GetType() == REPRESENTOBJECT_SET)
								{
									pObj->OnInformationChange(this);//
									pObj->OnTransformationChange(this);//两句是不一样的功能，
								}

							}
						}
					}
					break;
				case SCENESTATE_MOVE:
				case SCENESTATE_ROTATE:
					{
						KG3DSceneEditorBase::OnCommand(eCommandType);
					}
					break;
				}
			}
			else if(ctHome == eCommandType)
			{   //P模式时，复位摄像头为正前方
				m_pRepresentNPC->SetCurPosition(D3DXVECTOR3(5000.0f ,0.0f, 5000.0f), *this, false);
				m_SceneWndManager->GetCurOutputWnd()->GetCamera().SetPosition(D3DXVECTOR3(5000.0f ,500.0f, 4000.0f));					
			}
		}
		break;
	case ctDropObject:
		if (m_bEditing)
			DrapSelectedEntity();
		break;
	default:
		KG3DSceneEditorBase::OnCommand(eCommandType);
		break;
	}
}


HRESULT KG3DSceneSceneEditor::RenderToScreen(D3DXVECTOR2 const* pA, D3DXVECTOR2 const* pC, D3DXVECTOR2 const* pUVA, D3DXVECTOR2 const* pUVC, float fZ, DWORD dwFlags)
{
    KG3PROFILE_DURATION("Render To Screen");
	HRESULT hr = E_FAIL;
	HRESULT hrResult = E_FAIL;

	g_cGraphicsTool.SetCurScene(this);

	ASSERT(m_SceneWndManager->m_listOutputWindows.size() == 1);

	for (list<KG3DSceneOutputWnd*>::iterator it = m_SceneWndManager->m_listOutputWindows.begin(); 
		it != m_SceneWndManager->m_listOutputWindows.end(); ++it)
	{
		KG3DSceneOutputWnd* pWnd = *it;
		ASSERT(pWnd);

		hr = RenderClientWindow(*pWnd, dwFlags);
		KGLOG_COM_PROCESS_ERROR(hr);
	}

	hrResult = S_OK;
Exit0:
	return hrResult;
}

HRESULT KG3DSceneSceneEditor::SetHelpLineSize(size_t nLine, size_t nSize)
{
	if (nLine >= m_vecHelpLine.size())
		m_vecHelpLine.resize(nLine + 1);

	m_vecHelpLine[nLine].vecPosition.resize(nSize);

	return S_OK;
}


HRESULT KG3DSceneSceneEditor::SetHelpLineStart(size_t nLine, size_t nStart)
{
	if (nLine >= m_vecHelpLine.size())
		m_vecHelpLine.resize(nLine + 1);

	m_vecHelpLine[nLine].nStart = (int)nStart;
	return S_OK;
}

DWORD KG3DSceneSceneEditor::GetHelpLineSize()
{
	return (DWORD)m_vecHelpLine.size();
}

HRESULT KG3DSceneSceneEditor::SetHelpLineName(size_t nLine,LPSTR pName)
{
	KGLOG_PROCESS_ERROR(nLine>=0 && nLine<m_vecHelpLine.size());

	m_vecHelpLine[nLine].szName = pName;
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneSceneEditor::SetHelpLine(size_t nLine,int nSetID, size_t nBegin, size_t nCount, D3DCOLOR color, D3DXVECTOR3* pPos)
{
	if (nLine >= m_vecHelpLine.size())
		m_vecHelpLine.resize(nLine + 1);

	m_vecHelpLine[nLine].dwColor = color;
	m_vecHelpLine[nLine].nSetID = nSetID;

	size_t nPoint = nBegin + nCount;

	if (nPoint > m_vecHelpLine[nLine].vecPosition.size())
		m_vecHelpLine[nLine].vecPosition.resize(nPoint);

	if (pPos)
	{
		for (size_t nIndex = 0; nIndex < nCount; ++nIndex)
		{
			m_vecHelpLine[nLine].vecPosition[nIndex + nBegin] = pPos[nIndex];
		}
	}

	return S_OK;
}

HRESULT KG3DSceneSceneEditor::GetHelpLine(size_t nLine, size_t* pPoint, D3DXVECTOR3** ppPos)
{
	size_t nPoint = 0;
	if (nLine < m_vecHelpLine.size())
	{
		nPoint = m_vecHelpLine[nLine].vecPosition.size();

		if (pPoint)
			*pPoint = nPoint;

		if (ppPos)
		{
			if (nPoint > 0)
				*ppPos = &m_vecHelpLine[nLine].vecPosition[0];
			else
				*ppPos = NULL;
		}
	}
	else
	{
		if (pPoint)
			*pPoint = 0;
		if (ppPos)
			*ppPos = NULL;
	}
	return S_OK;
}


HRESULT KG3DSceneSceneEditor::ClearAllHelpLine()
{
	m_vecHelpLine.clear();
	return S_OK;
}



static int s_nSetID;
static BOOL IsLineEqual(const KG3DSceneSceneEditor::Lines& line)   
{ 
	return line.nSetID == s_nSetID; 
}; 

HRESULT KG3DSceneSceneEditor::ClearSetHelpLine(int nSetID)
{
	vector<Lines>::iterator itBegin = m_vecHelpLine.begin();
	vector<Lines>::iterator itEnd = m_vecHelpLine.end();
	s_nSetID = nSetID;
	m_vecHelpLine.erase(std::remove_if(m_vecHelpLine.begin(), m_vecHelpLine.end(),IsLineEqual),m_vecHelpLine.end()); 
	return S_OK;
}

HRESULT KG3DSceneSceneEditor::RenderHelpLine()
{
	GraphicsStruct::RenderState Zenable(D3DRS_ZENABLE,D3DZB_TRUE);
	GraphicsStruct::RenderState ZWrite(D3DRS_ZWRITEENABLE,TRUE);
	GraphicsStruct::RenderState ZFun(D3DRS_ZFUNC,D3DCMP_LESS);
	//GraphicsStruct::RenderStateAlpha Alpha(0x80,TRUE,TRUE,D3DBLEND_SRCALPHA,D3DBLEND_INVSRCALPHA);
	//GraphicsStruct::RenderState colorWrite(D3DRS_COLORWRITEENABLE ,D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_GREEN);

	for (size_t nLine = 0; nLine < m_vecHelpLine.size(); nLine++)
	{
		Lines& vHelpLine = m_vecHelpLine[nLine];
		size_t const nStart = vHelpLine.nStart;
		size_t const nCount = vHelpLine.vecPosition.size();


		if (nCount >= 2)
		{
			if (nStart == -1)
			{
				for (size_t nPoint = 0; nPoint < nCount - 1; ++nPoint)
				{
					g_cGraphicsTool.DrawLine(
						&vHelpLine.vecPosition[nPoint], 
						&vHelpLine.vecPosition[nPoint + 1],
						vHelpLine.dwColor,
						vHelpLine.dwColor);


				}

				g_cGraphicsTool.DrawLine(
					&vHelpLine.vecPosition[nCount - 1],
					&vHelpLine.vecPosition[0],
					vHelpLine.dwColor,
					vHelpLine.dwColor);

				if (g_cEngineManager.GetRenderOption(RENDER_INFO))
				{
					D3DXVECTOR3 vPosCenter = vHelpLine.vecPosition[0] + D3DXVECTOR3(0,50,0);
					g_cGraphicsTool.DrawScreenText3D(vPosCenter,(LPSTR)vHelpLine.szName.c_str(),vHelpLine.dwColor);
				}
			}
			else
			{
				ASSERT(nStart >= 0 && nStart < nCount);

				for (size_t nPoint = nStart; nPoint < nCount - 1; ++nPoint)
				{
					g_cGraphicsTool.DrawLine(
						&vHelpLine.vecPosition[nPoint], 
						&vHelpLine.vecPosition[nPoint + 1],
						vHelpLine.dwColor,
						vHelpLine.dwColor);
				}

				if (nStart != 0)
				{
					g_cGraphicsTool.DrawLine(
						&vHelpLine.vecPosition[nCount - 1],
						&vHelpLine.vecPosition[0],
						vHelpLine.dwColor,
						vHelpLine.dwColor);
				}
				if (nStart > 1)
				{
					for (size_t nPoint = 0; nPoint < nStart - 1; ++nPoint)
					{
						g_cGraphicsTool.DrawLine(
							&vHelpLine.vecPosition[nPoint], 
							&vHelpLine.vecPosition[nPoint + 1],
							vHelpLine.dwColor,
							vHelpLine.dwColor);
					}
				}
			}
		}
	}
	return S_OK;
}

HRESULT KG3DSceneSceneEditor::AddSelectedEntityToDoodadRefreshSet(DWORD dwSetID)
{
	/*KG3DSceneEntityList::iterator i = m_listSelectedEntity.begin();
	while (i!=m_listSelectedEntity.end())
	{
		KG3DSceneEntity Entity = *i;*/
	TypeEntityAccessProxy& proxy = this->GetSelectionTool().GetSelectionAccessProxy();
	for(proxy.Begin(); ! proxy.IsEnd(); proxy.StepForward())
	{
		KG3DSceneEntity Entity = proxy.GetCur();
		if(Entity.GetType()==SCENEENTITY_OBJECT)
		{
			KG3DRepresentObject* pObject = (KG3DRepresentObject*) Entity.m_pPointer;
			DWORD dwType = pObject->GetType();

			if(dwType == REPRESENTOBJECT_DOODAD)
			{
				/*KG3DRepresentDoodad* pDoodad = 
				static_cast<KG3DRepresentDoodad*>(pObject);
				IEKG3DRepresentDoodad* pIEDoodad = 
				static_cast<IEKG3DRepresentDoodad*>(pDoodad);*/

				FireEvent2(EVENTEDITORDOODADADDTOSET,0,dwSetID,pObject);
			}
		}
		//i++;
	}
	return S_OK;
}

HRESULT KG3DSceneSceneEditor::AddSelectedEntityToNPCRefreshSet(DWORD dwSetID)
{
	/*KG3DSceneEntityList::iterator i = m_listSelectedEntity.begin();
	while (i!=m_listSelectedEntity.end())
	{
		KG3DSceneEntity Entity = *i;*/
	TypeEntityAccessProxy& proxy = this->GetSelectionTool().GetSelectionAccessProxy();
	for(proxy.Begin(); ! proxy.IsEnd(); proxy.StepForward())
	{
		KG3DSceneEntity Entity = proxy.GetCur();
		if(Entity.GetType()==SCENEENTITY_OBJECT)
		{
			KG3DRepresentObject* pObject = (KG3DRepresentObject*) Entity.m_pPointer;
			DWORD dwType = pObject->GetType();

			if(dwType == REPRESENTOBJECT_NPC)
			{
				/*KG3DRepresentNPC* pNPC = 
				static_cast<KG3DRepresentNPC*>(pObject);
				IEKG3DRepresentNPC* pIENpc = 
				static_cast<IEKG3DRepresentNPC*>(pNPC);*/
				FireEvent2(EVENTEDITORNPCADDTOSET,0,dwSetID,pObject);
			}
		}
		//i++;
	}
	return S_OK;
}

HRESULT KG3DSceneSceneEditor::RemoveSelectedEntityToDoodadRefreshSet(DWORD dwSetID)
{
	/*KG3DSceneEntityList::iterator i = m_listSelectedEntity.begin();
	while (i!=m_listSelectedEntity.end())
	{
		KG3DSceneEntity Entity = *i;*/
	TypeEntityAccessProxy& proxy = this->GetSelectionTool().GetSelectionAccessProxy();
	for(proxy.Begin(); ! proxy.IsEnd(); proxy.StepForward())
	{
		KG3DSceneEntity Entity = proxy.GetCur();
		if(Entity.GetType()==SCENEENTITY_OBJECT)
		{
			KG3DRepresentObject* pObject = (KG3DRepresentObject*) Entity.m_pPointer;
			DWORD dwType = pObject->GetType();

			if(dwType == REPRESENTOBJECT_DOODAD)
			{
				FireEvent2(EVENTEDITORDOODADREMOVEFROMESET,0,dwSetID,pObject);
			}
		}
		//i++;
	}
	return S_OK;
}

HRESULT KG3DSceneSceneEditor::RemoveSelectedEntityToNPCRefreshSet(DWORD dwSetID)
{
	/*KG3DSceneEntityList::iterator i = m_listSelectedEntity.begin();
	while (i!=m_listSelectedEntity.end())
	{
		KG3DSceneEntity Entity = *i;*/
	TypeEntityAccessProxy& proxy = this->GetSelectionTool().GetSelectionAccessProxy();
	for(proxy.Begin(); ! proxy.IsEnd(); proxy.StepForward())
	{
		KG3DSceneEntity Entity = proxy.GetCur();
		if(Entity.GetType()==SCENEENTITY_OBJECT)
		{
			KG3DRepresentObject* pObject = (KG3DRepresentObject*) Entity.m_pPointer;
			DWORD dwType = pObject->GetType();

			if(dwType == REPRESENTOBJECT_NPC)
			{
				/*KG3DRepresentNPC* pNPC = 
				static_cast<KG3DRepresentNPC*>(pObject);
				IEKG3DRepresentNPC* pIENpc = 
				static_cast<IEKG3DRepresentNPC*>(pNPC);*/
				FireEvent2(EVENTEDITORNPCREMOVEFROMESET,0,dwSetID,pObject);
			}
		}
		//i++;
	}
	return S_OK;
}

HRESULT KG3DSceneSceneEditor::ClearSelectedEntityList()
{
	//m_listSelectedEntity.clear();
	this->GetSelectionTool().ClearSelection();
	return S_OK;
}

/*
HRESULT KG3DSceneSceneEditor::AddSelectedEntityEx(SCENEENTITYTYPE type,PVOID pPointer)
{
KG3DSceneEntity Entity(type,pPointer);
m_listSelectedEntity.push_back(Entity);
return S_OK;
}*/


HRESULT KG3DSceneSceneEditor::AddSelectedEntityToAIGroup(DWORD dwSetID)
{
	/*KG3DSceneEntityList::iterator i = m_listSelectedEntity.begin();
	while (i!=m_listSelectedEntity.end())
	{
		KG3DSceneEntity Entity = *i;*/
	TypeEntityAccessProxy& proxy = this->GetSelectionTool().GetSelectionAccessProxy();
	for(proxy.Begin(); ! proxy.IsEnd(); proxy.StepForward())
	{
		KG3DSceneEntity Entity = proxy.GetCur();
		if(Entity.GetType()==SCENEENTITY_OBJECT)
		{
			KG3DRepresentObject* pObject = (KG3DRepresentObject*) Entity.m_pPointer;
			DWORD dwType = pObject->GetType();

			if(dwType == REPRESENTOBJECT_NPC)
			{
				FireEvent2(EVENTEDITORNPCADDTOAIGROUP,0,dwSetID,pObject);
			}
		}
		//i++;
	}
	return S_OK;
}

HRESULT KG3DSceneSceneEditor::RemoveSelectedEntityFromAIGroup(DWORD dwSetID)
{
	/*KG3DSceneEntityList::iterator i = m_listSelectedEntity.begin();
	while (i!=m_listSelectedEntity.end())
	{
		KG3DSceneEntity Entity = *i;*/
	TypeEntityAccessProxy& proxy = this->GetSelectionTool().GetSelectionAccessProxy();
	for(proxy.Begin(); ! proxy.IsEnd(); proxy.StepForward())
	{
		KG3DSceneEntity Entity = proxy.GetCur();
		if(Entity.GetType()==SCENEENTITY_OBJECT)
		{
			KG3DRepresentObject* pObject = (KG3DRepresentObject*) Entity.m_pPointer;
			DWORD dwType = pObject->GetType();

			if(dwType == REPRESENTOBJECT_NPC)
			{
				/*KG3DRepresentNPC* pNPC = 
				static_cast<KG3DRepresentNPC*>(pObject);
				IEKG3DRepresentNPC* pIENpc = 
				static_cast<IEKG3DRepresentNPC*>(pNPC);*/
				FireEvent2(EVENTEDITORNPCREMOVEFROMEAIGROUP,0,dwSetID,pObject);
			}
		}
		//i++;
	}
	return S_OK;
}

HRESULT KG3DSceneSceneEditor::SetNPCPosition(BOOL UseRayIntersectionPosOrInputPos, BOOL bTopViewSet, const D3DXVECTOR3* pInputPos)//P模式放NPC的时候用
{

	D3DXVECTOR3 vecNpcDir;
	D3DXVECTOR3 vec3NpcPos;

	D3DXVECTOR3 rayStart, rayDir;
	KG3DSceneOutputWnd* pMainWnd =static_cast<KG3DSceneOutputWnd *>(m_SceneWndManager
		->GetOutputWindow( OUTPUTWND_SCENEEDITOR_MAIN ));
	ASSERT( pMainWnd != NULL );

	m_SceneWndManager->GetCurOutputWnd()->GetPickRay( &rayStart,&rayDir, NULL);

	if(UseRayIntersectionPosOrInputPos)
	{
		D3DXVECTOR3 vInter;	

		enum{em_npc_ray_intersection_length = 50000,};

		FLOAT fDistance = FLT_MAX;
		BOOL bFound = ::RayIntersection(*this, rayStart, rayDir, &fDistance,NULL,NULL,em_npc_ray_intersection_length);

		if (bFound) 
		{
			_ASSERTE(fDistance < em_npc_ray_intersection_length);
			vInter = rayStart + rayDir * fDistance;
		}
		else
		{
			vInter = rayStart;
		}
		//GetFloor(&vec3NpcPos, &vInter);
		vec3NpcPos = vInter;
		GetFloor(&vInter, &vec3NpcPos.y);
	}
	else
	{
		KG_PROCESS_ERROR(NULL != pInputPos);
		//GetFloor(&vec3NpcPos, pInputPos);	
		vec3NpcPos = *pInputPos;
		GetFloor(pInputPos, &vec3NpcPos.y);

	}

	{
		if (bTopViewSet)
		{
			vecNpcDir = D3DXVECTOR3(0.f, 0.f, 1.f);
		}
		else
		{
			vecNpcDir = D3DXVECTOR3(rayDir.x, 0.f, rayDir.z);
		}
		D3DXVec3Normalize(&vecNpcDir, &vecNpcDir);

		if (NULL != m_pRepresentNPC)
			m_pRepresentNPC->SetCurPosition(vec3NpcPos, *this, TRUE);

		m_pRepresentNPC->SetDirection(vecNpcDir);
	}

	{
		D3DXVECTOR3 vNewCampPos = vec3NpcPos + D3DXVECTOR3(0.f, 0.5f, -0.5f) * 10000.f;

		pMainWnd->GetCamera().SetPositionLookAtUp(&vNewCampPos, &vec3NpcPos, &D3DXVECTOR3(0,1,0));
	} 

	m_vCameraMainPos.x = rayStart.x;
	m_vCameraMainPos.z = rayStart.z;

	m_vCameraTopPos.x = m_vCameraMainPos.x;
	m_vCameraTopPos.z = m_vCameraMainPos.z;

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneSceneEditor::GetIELogicScene(IEKG3DSceneLogicalEditorBase** pLogicScene)
{
	_ASSERTE(pLogicScene);
	if (pLogicScene)
	{
		*pLogicScene = m_lpLogicEditor;
		return S_OK;
	}
	return E_FAIL;
}

//)

//(by dengzhihui Refactor 2006年11月27日
HRESULT KG3DSceneSceneEditor::SetEditState(DWORD dwEditState)
{
	return KG3DSceneEditorBase::SetEditState(dwEditState);
}

HRESULT KG3DSceneSceneEditor::GetEditState(DWORD* pdwState)
{
	return KG3DSceneEditorBase::GetEditState(pdwState);
}

ULONG KG3DSceneSceneEditor::GetEditState()
{
	//return m_dwEditState;
	return KG3DSceneEditorBase::GetEditState();
}

HRESULT KG3DSceneSceneEditor::LoadCameraAnimation(LPSTR pszCameraAniFile)
{
    HRESULT hResult = E_FAIL;
    KG_PROCESS_ERROR(pszCameraAniFile);
    KG_PROCESS_ERROR(m_pCameraMovement);

    hResult = m_pCameraMovement->LoadFromFile(pszCameraAniFile);

Exit0:
    return hResult;
}

HRESULT KG3DSceneSceneEditor::StartPlayCamAni()
{
    HRESULT hResult  = E_FAIL;
    HRESULT hRetCode = E_FAIL;
    IEKG3DSceneCameraAnimation* piCameraAnimation = NULL;
    BOOL bPlayed = FALSE;
    KG_PROCESS_ERROR(m_pCameraMovement);

    hRetCode = m_pCameraMovement->GetCurCamAnimation(&piCameraAnimation);
    KG_COM_PROCESS_ERROR(hRetCode);
    KG_PROCESS_ERROR(piCameraAnimation);
    
    hRetCode = piCameraAnimation->GetPlayState(&bPlayed);
    if (bPlayed)
    {
        piCameraAnimation->StopPlay();
    }

    hRetCode = m_pCameraMovement->SetCurCamAnimation(0);
    KG_COM_PROCESS_ERROR(hRetCode);

    piCameraAnimation = NULL;
    hRetCode = m_pCameraMovement->GetCurCamAnimation(&piCameraAnimation);
    KG_COM_PROCESS_ERROR(hRetCode);
    KG_PROCESS_ERROR(piCameraAnimation);
    
    piCameraAnimation->StartPlay(FALSE);
    this->SetEditState(SCENESTATE_BENCHMARK);
    
    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT KG3DSceneSceneEditor::StopPlayCamAni()
{
    HRESULT hResult  = E_FAIL;
    HRESULT hRetCode = E_FAIL;
    IEKG3DSceneCameraAnimation* piCameraAnimation = NULL;
    KG_PROCESS_ERROR(m_pCameraMovement);

    this->SetEditState(SCENESTATE_SELECT);

    hRetCode = m_pCameraMovement->GetCurCamAnimation(&piCameraAnimation);
    KG_COM_PROCESS_ERROR(hRetCode);
    KG_PROCESS_ERROR(piCameraAnimation);

    piCameraAnimation->StopPlay();

Exit0:
    return hResult;
}

HRESULT KG3DSceneSceneEditor::PausePlayCamAni()
{
    HRESULT hResult  = E_FAIL;
    HRESULT hRetCode = E_FAIL;
    IEKG3DSceneCameraAnimation* piCameraAnimation = NULL;
    KG3DSceneCameraAnimation* pKG3DSceneCameraAnimation = NULL;
    KG_PROCESS_ERROR(m_pCameraMovement);

    hRetCode = m_pCameraMovement->GetCurCamAnimation(&piCameraAnimation);
    KG_COM_PROCESS_ERROR(hRetCode);

    pKG3DSceneCameraAnimation = static_cast<KG3DSceneCameraAnimation*>(piCameraAnimation);
    KG_PROCESS_ERROR(pKG3DSceneCameraAnimation);
    
    BOOL bIsPlaying = FALSE;
    pKG3DSceneCameraAnimation->GetPlayState(&bIsPlaying);
    KG_PROCESS_ERROR(bIsPlaying);

    pKG3DSceneCameraAnimation->PausePlay(NULL, TRUE);
Exit0:
    return hResult;
}

HRESULT KG3DSceneSceneEditor::GetCurOutputWnd(IEKG3DSceneOutputWnd **ppiRetCurOutputWnd)
{
	return KG3DScene::GetCurOutputWnd(ppiRetCurOutputWnd);
}


HRESULT KG3DSceneSceneEditor::SetObjectInHandle(IEKG3DRepresentObject* pObject, BOOL bReleaseObjectInHandleWhenReplace)
{
	//保留这个函数，主要是为了避免外部调用的时候，每次都要得到PlacementTool的接口比较麻烦
	KG3DSceneObjectPlacementTool* pObjPlacementTool = GetObjectPlacementTool();
	KG_PROCESS_ERROR(NULL != pObjPlacementTool);
	return pObjPlacementTool->SetObjectInHandle(pObject, bReleaseObjectInHandleWhenReplace);
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneSceneEditor::GetObjectInHandle(IEKG3DRepresentObject** ppObj)
{
	KG3DSceneObjectPlacementTool* pObjPlacementTool = GetObjectPlacementTool();
	if (pObjPlacementTool)
	{
		return pObjPlacementTool->GetObjectInHandle(ppObj);
	}
	return E_FAIL;
}

HRESULT KG3DSceneSceneEditor::RemoveOutputWindow(int nOutputWindowID)
{
	return KG3DScene::RemoveOutputWindow(nOutputWindowID);
}

HRESULT KG3DSceneSceneEditor::SetSampleMip(D3DTEXTUREFILTERTYPE eType)
{
	m_Sample_Mip = eType;
	return S_OK;
}

HRESULT KG3DSceneSceneEditor::SetSampleMag(D3DTEXTUREFILTERTYPE eType)
{
	m_Sample_Mag = eType;
	return S_OK;
}
HRESULT KG3DSceneSceneEditor::SetSampleMin(D3DTEXTUREFILTERTYPE eType)
{
	m_Sample_Min = eType;
	return S_OK;
}

HRESULT KG3DSceneSceneEditor::SetMipmapLod(float fValue)
{
	m_fMipmapLod = fValue;
	return S_OK;
}

HRESULT KG3DSceneSceneEditor::GetScene(IEKG3DScene** ppScene)
{
	_ASSERTE(ppScene);
	if (ppScene)
	{
		*ppScene = dynamic_cast<KG3DScene*>(this);
		return S_OK;
	}
	return E_FAIL;
}

HRESULT KG3DSceneSceneEditor::LockSelSpaceNode()
{
	if (/*m_listSelectedEntity.size()*/this->GetSelectionTool().GetSelectionCount() != 1)
		return E_CDN_NUMBER;

	TypeEntityAccessProxy& proxy = this->GetSelectionTool().GetSelectionAccessProxy();
	proxy.Begin();
	KG3DSceneEntity e = proxy.GetCur();
	//KG3DSceneEntity e = *m_listSelectedEntity.begin();


	if (e.GetType() != SCENEENTITY_OBJECT)
		return E_CDN_TYPE;


	KG3DRepresentObject* obj = (KG3DRepresentObject*)e.m_pPointer;

	if (obj->GetType() != REPRESENTOBJECT_PVS)
		return E_CDN_TYPE;

	KG3DRepresentObjectPVS* pvs = dynamic_cast<KG3DRepresentObjectPVS*>(obj);

	ASSERT(pvs);

	m_pEditSpaceNode = pvs;


	return S_OK;

}

HRESULT KG3DSceneSceneEditor::UnLockSpaceNode()
{
	m_pEditSpaceNode = m_lpSceneOutDoorSpaceMgr;

	return S_OK;
}

HRESULT KG3DSceneSceneEditor::GetSceneDetailMaterialMgr(IEKG3DDetailMaterialMgr** ppMtl)
{
	KGLOG_PROCESS_ERROR(m_lpSceneOutDoorSpaceMgr);

	KGLOG_PROCESS_ERROR(m_lpSceneOutDoorSpaceMgr->m_lpDetailMtls);


	(*ppMtl) = (IEKG3DDetailMaterialMgr*)m_lpSceneOutDoorSpaceMgr->m_lpDetailMtls;

	return S_OK;
Exit0:
	return E_FAIL;
}

//HRESULT KG3DSceneSceneEditor::GetSelectedEntityCount(INT* nCount)
//{
//	_ASSERTE(nCount);
//	if (nCount)
//	{
//		*nCount = (INT)/*m_listSelectedEntity.size()*/this->GetSelectionTool().GetSelectionCount();
//		return S_OK;
//	}
//
//	return E_FAIL;
//}
//HRESULT KG3DSceneSceneEditor::GetSelectedEntity(INT nIndex, IEKG3DSceneEntity** ppEntity)
//{
//	_ASSERTE(ppEntity && nIndex >= 0 && nIndex < (INT)this->GetSelectionTool().GetSelectionCount());
//	HRESULT hr = E_FAIL;
//	KG_PROCESS_ERROR(ppEntity);
//	KG_PROCESS_ERROR(nIndex >= 0);
//	KG_PROCESS_ERROR(nIndex < (INT)this->GetSelectionTool().GetSelectionCount());
//
//	hr = KG3DSceneSelectBase::GetSelectedEntity(nIndex, ppEntity);	//不用对0做特殊操作，直接调用就好了
//Exit0:
//	return hr;
//}

//(by dengzhihui Refactor 2006年11月28日
BOOL KG3DSceneSceneEditor::GetWaterReflect()
{
    return g_cEngineOption.nWaterDetail >= WD_VERY_HIGH;
}

HRESULT KG3DSceneSceneEditor::SetWaterReflect(BOOL bValue)
{
	return E_NOTIMPL;
}


BOOL KG3DSceneSceneEditor::GetWaterRefract()
{
    return g_cEngineOption.nWaterDetail >= WD_MEDIUM;
}

HRESULT KG3DSceneSceneEditor::SetWaterRefract(BOOL bValue)
{
    return E_NOTIMPL;
}

HRESULT KG3DSceneSceneEditor::GetSceneEditorBase(IEKG3DSceneEditorBase** ppSceneBase)
{
	_ASSERTE(ppSceneBase);

	if (ppSceneBase)
	{
		*ppSceneBase = static_cast<IEKG3DSceneEditorBase*>(this);
		return S_OK;
	}
	return E_FAIL;
}

float KG3DSceneSceneEditor::GetZFar()
{
	return m_fZFar;
}
HRESULT KG3DSceneSceneEditor::SetZFar(float fZFar)
{
	m_fZFar = fZFar;
	return S_OK;
}

HRESULT KG3DSceneSceneEditor::GetSceneSelector(IEKG3DSceneSelectBase** ppSelector)
{
	_ASSERTE(ppSelector);
	if (ppSelector)
	{
		*ppSelector = static_cast<IEKG3DSceneSelectBase*>(this);
		return S_OK;
	}
	return E_FAIL;
}

HRESULT KG3DSceneSceneEditor::GetRepresentNPC(IEKG3DRepresentNPC** ppNPC)
{
	_ASSERTE(ppNPC);
	if (ppNPC)
	{
		*ppNPC = m_pRepresentNPC;
		return S_OK;
	}
	return E_FAIL;
}

HRESULT KG3DSceneSceneEditor::GetPrimaryWindow(IEKG3DSceneOutputWnd **ppiRetPrimaryWindow)
{
	_ASSERTE(ppiRetPrimaryWindow);
	if (ppiRetPrimaryWindow)
	{
		return KG3DScene::GetPrimaryWindow(ppiRetPrimaryWindow);
	}
	return E_FAIL;
}



D3DXVECTOR3 KG3DSceneSceneEditor::GetCameraTopPos()
{
	return m_vCameraTopPos;
}

HRESULT KG3DSceneSceneEditor::SetCameraTopPos(float x, float z)
{
	m_vCameraTopPos.x = x;
	m_vCameraTopPos.z = z;
	return S_OK;
}

D3DXVECTOR3 KG3DSceneSceneEditor::GetCameraMainPos()
{
	return m_vCameraMainPos;
}

float KG3DSceneSceneEditor::GetCameraTopScale()
{
	return m_fCameraTopScale;
}

HRESULT KG3DSceneSceneEditor::SetCameraTopScale(float fScale)
{
	m_fCameraTopScale = fScale;
	return S_OK;
}

HRESULT KG3DSceneSceneEditor::ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam)//<PROCESS_MESSAGE>
{
	return KG3DSceneEditorBase::ProcessMessage(message, wParam, lParam);
}

HRESULT KG3DSceneSceneEditor::AddSelectedEntityByIndex(INT Index)
{
	int K = 0;

	map<string,KG3DSceneSceneEditor::ObjectGroup>::iterator i =
		m_mapObjectGroup.begin();

	KG3DSceneSelectionTool& selTool = this->GetSelectionTool();

	while(i!=m_mapObjectGroup.end())
	{
		string GroupName = i->first;
		KG3DSceneSceneEditor::ObjectGroup* pGroup = &i->second;

		if((int)Index == K)
		{
			list<KG3DRepresentObject*>::iterator j = pGroup->listObject.begin();
			while(j!=pGroup->listObject.end())
			{
				KG3DRepresentObject* pObject = *j;
				KG3DSceneEntity Entity(SCENEENTITY_OBJECT,pObject);
				
				//AddSelectedEntity(Entity);
				selTool.SetSelected(Entity, TRUE);

				j++;
			}
		}

		K++;
		i++;
	}
	return S_OK;
}

INT KG3DSceneSceneEditor::GetObjectGroupSize()
{
	INT nCount = 0;
	map<string,KG3DSceneSceneEditor::ObjectGroup>::iterator i = m_mapObjectGroup.begin();
	while(i != m_mapObjectGroup.end())
	{
		nCount ++;
		i++;
	}
	return nCount;
}

HRESULT KG3DSceneSceneEditor::GetGroupInfo(INT nIndex, INT& nGroupSize, LPTSTR szName, INT nBufLen)
{
	INT nCount = 0;
	map<string,KG3DSceneSceneEditor::ObjectGroup>::iterator i = m_mapObjectGroup.begin();
	while(i != m_mapObjectGroup.end())
	{
		if (nIndex == nCount)
		{
			string GroupName = i->first;
			KG3DSceneSceneEditor::ObjectGroup* pGroup = &i->second;
			nGroupSize = (INT)pGroup->listObject.size();
			lstrcpy(szName, GroupName.substr(0, nBufLen).c_str());
			break;
		}

		nCount ++;
		i++;
	}
	return nCount;
}
HRESULT KG3DSceneSceneEditor::IESetLogicalScene(IEKG3DSceneLogicalEditorBase* pLogicalScene)
{
	m_lpLogicEditor = pLogicalScene;
	//return SetLogicalScene(pLogicalScene);
	return S_OK;
}


HRESULT KG3DSceneSceneEditor::SetSnapToolScale(FLOAT fScale)
{
	m_vSnapTool.SetScale(fScale);
	return S_OK;
}


HRESULT KG3DSceneSceneEditor::IEOnResizeWindow(PRECT const pRect)
{
	return OnResizeWindow(pRect);
}

HRESULT KG3DSceneSceneEditor::IELoadPoly(
	LPCTSTR pScriptFileName,LPCTSTR pPolyName,
	DWORD dwColor,
	LPCTSTR szLogicalSetName,
	int nLogicalSetIndex,
	DWORD dwModifyState,
	float fVertexHeight,
	int nNumVertex,
	D3DXVECTOR3 vPos[])
{
	BeginAddPolyVertex();

	m_fPolyVertexHeight = fVertexHeight;

	for (int i=0;i<nNumVertex;i++)
	{
		AddPolyVetex(i,vPos[i]);
	}
	KG3DRepresentPoly* pNewPoly = NULL;
	EndAddPolyVertex(0,&pNewPoly);
	if(pNewPoly)
	{
		pNewPoly->SetPolyName(pPolyName);
		pNewPoly->SetScriptFileName(pScriptFileName);
		pNewPoly->SetColor(dwColor);
		pNewPoly->SetLogicalSetName(szLogicalSetName);
		pNewPoly->SetLogicalSetID(nLogicalSetIndex);
		pNewPoly->SetModifyState(dwModifyState);	
	}
	return S_OK;
}

struct Point
{
	int nX;
	int nZ;
};

static Point Filter[] = {{0,0}, {0, -1}, {-1, 0}, {1, 0}, {1, 0}};
void KG3DSceneSceneEditor::GetEnviromentParamenter(const D3DXVECTOR3& vec3Pos, 
												   EnviromentParameter& EnvPara)
{
	static int nFilterSize = sizeof(Filter) / sizeof(Point);
	//Get light map information
	LPDIRECT3DTEXTURE9 pLightMap = NULL;
	{
		//KG3DLight& PlayerLight = g_cGraphicsTool.GetPlayerLight(*this);
		const KG3DLightParam& lightParam = g_cGraphicsTool.GetPlayerLightParam(*this);
		EnvPara.LightDiffuse = lightParam.diffuse;//Color2Value(PlayerLight.GetDiffuse());
		EnvPara.LightAmbient = Value2Color(lightParam.sceneAmbient);//(PlayerLight.GetSceneAmbient());
	}
	EnvPara.Diffuse.a = 1.0f;
	EnvPara.Diffuse.r = 1.0f;
	EnvPara.Diffuse.g = 1.0f;
	EnvPara.Diffuse.b = 1.0f;
	EnvPara.Ambient.a = 1.0f;
	EnvPara.Ambient.r = 1.0f;
	EnvPara.Ambient.g = 1.0f;
	EnvPara.Ambient.b = 1.0f;

	EnvPara.Diffuse.a = 1.0f;
	EnvPara.Diffuse.r = EnvPara.Diffuse.b;
	EnvPara.Diffuse.g = EnvPara.Diffuse.b;
//Exit0:
	if (pLightMap)
	{
		pLightMap->UnlockRect(NULL);
	}
	;
}
//)


HRESULT KG3DSceneSceneEditor::BeginMovieREC(
	SIZE                 Size, 
	FLOAT                fInterval, 
	D3DTEXTUREFILTERTYPE FiterType,
	D3DXIMAGE_FILEFORMAT ImageType,
	INT                  nAutoScale
	)
{
	HRESULT hResult  = E_FAIL;
	HRESULT hRetCode = E_FAIL;

	KG_PROCESS_ERROR(Size.cx > 0 && Size.cy > 0);

	//m_TimeControl.m_fNowTime = 0.f;
	m_bMovieREC  = TRUE;
	m_nSaveCount = 0;

	TCHAR szFilePath[MAX_PATH];
	time_t tmNow;
	time(&tmNow);
	_tcsftime(szFilePath, _countof(szFilePath), "%Y_%m_%d_%H_%M_%S", localtime(&tmNow));

	m_strMovieSavePath = TEXT("Movies\\");
	mkdir(m_strMovieSavePath.c_str());

	m_strMovieSavePath += szFilePath;
	m_strMovieSavePath += TEXT("\\");
	mkdir(m_strMovieSavePath.c_str());

	m_ImageType  = ImageType;
	m_FiterType  = FiterType;
	m_nAutoScanl = nAutoScale;
	m_MovieSize  = Size;

	g_cEngineManager.GetTimeControl().m_dwInterval = (DWORD)fInterval;

	SAFE_RELEASE(m_lpMovieSurface);
	hRetCode = g_pd3dDevice->CreateRenderTarget(
		Size.cx,
		Size.cy,
		D3DFMT_A8R8G8B8,
		D3DMULTISAMPLE_NONE,
		0,
		FALSE,
		&m_lpMovieSurface,
		NULL
		);
	/* hRetCode = g_pd3dDevice->CreateOffscreenPlainSurface(
	Size.cx,
	Size.cy,
	D3DFMT_A8R8G8B8,
	D3DPOOL_SCRATCH,
	&m_lpMovieSurface,
	NULL
	);*/
	KG_COM_PROCESS_ERROR(hRetCode);

	hResult = S_OK;
Exit0:
	return hResult;
}

HRESULT KG3DSceneSceneEditor::EndMovieREC()
{
	SAFE_RELEASE(m_lpMovieSurface);
	g_cEngineManager.GetTimeControl().m_dwLastTime = timeGetTime() - g_cEngineManager.GetTimeControl().m_dwInterval;
	g_cEngineManager.GetTimeControl().m_dwNowTime  = timeGetTime();
	m_bMovieREC = FALSE;

	return S_OK;
}

HRESULT KG3DSceneSceneEditor::PlayEntityAnimation(int nPlay)
{
	IEKG3DSceneCameraMovement*  pCamMov = NULL;
	IEKG3DSceneCameraAnimation* pCamAni = NULL;


    if (nPlay)
    {
        for (size_t i = 0; i < m_vecSFXSuede.size(); i++)
        {
            m_vecSFXSuede[i]->GetAnimationController(ANICTL_PRIMARY)->SeekAnimation(
                ANIMATION_SEEK_SET, 0
                );
            m_vecSFXSuede[i]->PlayAnimation(
                ENU_ANIMATIONPLAY_CIRCLE,
                TEXT(""),
                1.f,
                0,
                NULL,
				NULL,
				ANICTL_PRIMARY
                );
        }
    }
    else
    {
        for (size_t i = 0; i < m_vecSFXSuede.size(); i++)
        {
            m_vecSFXSuede[i]->GetAnimationController(ANICTL_PRIMARY)->SeekAnimation(
                ANIMATION_SEEK_SET, 0
                );
             m_vecSFXSuede[i]->GetAnimationController(ANICTL_PRIMARY)->PauseAnimation(TRUE);
        }
    }



	if (SUCCEEDED(GetCameraMovement(&pCamMov)))
	{
		if (SUCCEEDED(pCamMov->GetCurCamAnimation(&pCamAni)))
		{
			if (nPlay)
			{
				pCamAni->StartPlay(TRUE);
			}
			else
			{
				pCamAni->StopPlay();
			}
		}
    }


    KG3DSceneEntityList::iterator i = m_listRenderEntity.begin();
    while (i != m_listRenderEntity.end())
    {
        KG3DSceneEntity Entity = *i;
        
        switch (Entity.GetType())
        {
        case SCENEENTITY_OBJECT :
            {
                KG3DRepresentObject* pObject = (KG3DRepresentObject*)Entity.m_pPointer;
                for (DWORD n = 0; n < pObject->GetModelCount(); n++)
                {
                    KG3DModel* pModel = NULL;
                    if (SUCCEEDED(pObject->GetModel(pModel, n,FALSE)))
                    {
                        if (nPlay)
                        {             
                            if (pModel->GetAnimationController(ANICTL_PRIMARY)->HasAnimation())
                            {
                                pModel->GetAnimationController(ANICTL_PRIMARY)->SeekAnimation(
                                    ANIMATION_SEEK_SET, 0
                                    );
                                pModel->GetAnimationController(ANICTL_PRIMARY)->PauseAnimation(FALSE);
                            }
                            else
                            {
                                TCHAR szFilePath[MAX_PATH];
                                sprintf(szFilePath, TEXT("%s"), pModel->m_scName.c_str());
                                TCHAR* pStr = strrchr(szFilePath, '\\');
                                if (!pStr)
                                    break;

                                strncpy(pStr, TEXT("\\movie.ani\0"), sizeof(TEXT("\\movie.ani\0")));

                                if (PathFileExists(szFilePath))
                                {
                                    pModel->PlayAnimation(
                                        ENU_ANIMATIONPLAY_CIRCLE,
                                        szFilePath,
                                        1.f,
                                        0,
                                        NULL,
										NULL,
										ANICTL_PRIMARY
                                     );
                                }
                            }
                        }
                        else
                        {
                            pModel->GetAnimationController(ANICTL_PRIMARY)->SeekAnimation(
                                ANIMATION_SEEK_SET, 0
                                );
                            pModel->GetAnimationController(ANICTL_PRIMARY)->PauseAnimation(TRUE);
                        }
                    }
                }
            }
            break;

        case SCENEENTITY_MODEL :
            {
                KG3DModel* pModel = (KG3DModel*)Entity.m_pPointer;
                if (pModel->GetType() == MESHTYPE_SFX)
                {
                    
                }
                else
                {
                    if (nPlay)
                    {
                        if (pModel->GetAnimationController(ANICTL_PRIMARY)->HasAnimation())
                        {
                            pModel->GetAnimationController(ANICTL_PRIMARY)->SeekAnimation(
                                ANIMATION_SEEK_SET, 0
                                );
                            pModel->GetAnimationController(ANICTL_PRIMARY)->PauseAnimation(FALSE);
                        }
                        else
                        {

                            
                            TCHAR szFilePath[MAX_PATH];
                            sprintf(szFilePath, TEXT("%s"), pModel->m_scName.c_str());
                            TCHAR* pStr = strrchr(szFilePath, '\\');
                            if (!pStr)
                                break;

                            strncpy(pStr, TEXT("\\movie.ani\0"), sizeof(TEXT("\\movie.ani\0")));

                            if (PathFileExists(szFilePath))
                            {
                                pModel->PlayAnimation(
                                    ENU_ANIMATIONPLAY_CIRCLE,
                                    szFilePath,
                                    1.f,
                                    0,
                                    NULL,
									NULL,
                                    ANICTL_PRIMARY);
                            }
                        }
                    }
                    else
                    {
                        pModel->GetAnimationController(ANICTL_PRIMARY)->SeekAnimation(
                            ANIMATION_SEEK_SET, 0
                            );
                        pModel->GetAnimationController(ANICTL_PRIMARY)->PauseAnimation(TRUE);
                    }
                }
            }
            break;

		default :
			break;
		}
		i++;
	}
	return S_OK;
}


IEKG3DRepresentNPC* KG3DSceneSceneEditor::GetIEKG3DRepresentNPC(PVOID pObject)
{
	KG3DRepresentNPC* pNpc = static_cast<KG3DRepresentNPC*>(pObject);
	return pNpc;
}

IEKG3DRepresentDoodad* KG3DSceneSceneEditor::GetIEKG3DRepresentDoodad(PVOID pObject)
{
	KG3DRepresentDoodad* pDoodad = static_cast<KG3DRepresentDoodad*>(pObject);
	return pDoodad;
}

IEKG3DRepresentWayPoint* KG3DSceneSceneEditor::GetIEKG3DRepresentWayPoint(PVOID pObject)
{
	KG3DRepresentWayPoint* pPoint = static_cast<KG3DRepresentWayPoint*>(pObject);
	return pPoint;
}

IEKG3DRepresentTrafficPoint* KG3DSceneSceneEditor::GetIEKG3DRepresentTrafficPoint(PVOID pObject)
{
	KG3DRepresentTrafficPoint* pPoint = static_cast<KG3DRepresentTrafficPoint*>(pObject);
	return pPoint;
}

IEKG3DRepresentRefreshPoint* KG3DSceneSceneEditor::GetIEKG3DRepresentRefreshPoint(PVOID pObject)
{
	KG3DRepresentRefreshPoint* pPoint = static_cast<KG3DRepresentRefreshPoint*>(pObject);
	return pPoint;
}

HRESULT KG3DSceneSceneEditor::SetEditing(DWORD dwEditing)
{
	KG3DSceneEditorBase::SetEditing(dwEditing);

	//g_bRenderSoundBall = m_bEditing;
	g_bRenderRoadNode = m_bEditing;
	return S_OK;
}

HRESULT KG3DSceneSceneEditor::BeginSelectSceneBlock(BOOL bZone,BOOL bSection,BOOL bRegion,BOOL bOnlyExist)
{
	//m_dwEditState = SCENESTATE_SELECTREGION ;
	this->SetEditState(SCENESTATE_SELECTREGION);

	m_listSceneBlockSelectPoint.clear();
	m_listSceneBlockSelected[0].clear();
	m_listSceneBlockSelected[1].clear();
	m_listSceneBlockSelected[2].clear();
	m_listSceneBlockSelected[3].clear();

	m_bEnableSceneBlockSelect[BLOCK_TYPE_REGION ] = bRegion;
	m_bEnableSceneBlockSelect[BLOCK_TYPE_SECTION] = bSection;
	m_bEnableSceneBlockSelect[BLOCK_TYPE_ZONE   ] = bZone;
	m_bEnableSceneBlockSelect[BLOCK_TYPE_AREA   ] = FALSE;

	m_bSelectSceneBlockExistOnly = bOnlyExist;

	return S_OK;
}

HRESULT KG3DSceneSceneEditor::EndSelectSceneBlock(int nAddFlag, int nFinishFlag)
{
	for (int i=0;i<4;i++)
	{
		m_listTempSelBlock[i].clear();

		if(m_bEnableSceneBlockSelect[i])
		{
			if (nAddFlag )
			{
				vector<POINT>vecBlock;
				if(i==BLOCK_TYPE_REGION)
				{
					m_lpSceneOutDoorSpaceMgr->SelectRegion(&vecBlock,m_RegionSelectA,m_RegionSelectC,m_bSelectSceneBlockExistOnly);
				}
				else if (i==BLOCK_TYPE_SECTION)
				{
					m_lpSceneOutDoorSpaceMgr->SelectSection(&vecBlock,m_RegionSelectA,m_RegionSelectC,m_bSelectSceneBlockExistOnly);
				}
				else if (i==BLOCK_TYPE_ZONE)
				{
					m_lpSceneOutDoorSpaceMgr->SelectZone(&vecBlock,m_RegionSelectA,m_RegionSelectC,m_bSelectSceneBlockExistOnly);
				}

				m_listSceneBlockSelected[i].clear();
				for (size_t k=0;k<vecBlock.size();k++)
				{
					m_listSceneBlockSelected[i].push_back(vecBlock[k]);
				}

				if (nFinishFlag)
				{
					list<POINT> listTemp = m_listSceneBlockSelected[i];
					m_listSceneBlockSelected[i].clear();

					std::set_symmetric_difference(
						listTemp.begin(),
						listTemp.end(),
						m_listTempSelBlock[i].begin(),
						m_listTempSelBlock[i].end(),
						std::inserter(m_listSceneBlockSelected[i], m_listSceneBlockSelected[i].begin())
						);
					m_listTempSelBlock[i].clear();
				}
			}
			else
			{
				vector<POINT>vecBlock;
				if(i==BLOCK_TYPE_REGION)
				{
					m_lpSceneOutDoorSpaceMgr->SelectRegion(&vecBlock,m_RegionSelectA,m_RegionSelectC,m_bSelectSceneBlockExistOnly);
				}
				else if (i==BLOCK_TYPE_SECTION)
				{
					m_lpSceneOutDoorSpaceMgr->SelectSection(&vecBlock,m_RegionSelectA,m_RegionSelectC,m_bSelectSceneBlockExistOnly);
				}
				else if (i==BLOCK_TYPE_ZONE)
				{
					m_lpSceneOutDoorSpaceMgr->SelectZone(&vecBlock,m_RegionSelectA,m_RegionSelectC,m_bSelectSceneBlockExistOnly);
				}

				m_listSceneBlockSelected[i].clear();
				for (size_t k=0;k<vecBlock.size();k++)
				{
					m_listSceneBlockSelected[i].push_back(vecBlock[k]);
				}
			}	
		}

	}

	return S_OK;
}

HRESULT KG3DSceneSceneEditor::ApplyHeightMapToSelectedSceneBlock(LPTSTR pHeightMap,int nWidth,float fLowest,float fHeightTotal)
{
	//HRESULT hr = E_FAIL;

	KG3DHeightMap vHeightMap;
	TCHAR path_buffer[256];
	TCHAR drive[256];
	TCHAR dir[256];
	TCHAR fname[256];
	TCHAR ext[256];

	wsprintf(path_buffer,"%s",pHeightMap);
	_splitpath( path_buffer, drive, dir, fname, ext );

	if(_strcmpi(ext,".r32")==0)
	{
		vHeightMap.CreateFromR32File(nWidth,nWidth,pHeightMap);
	}
	else if(_strcmpi(ext,".bmp")==0)
	{
		vHeightMap.CreateFromBmpFile(pHeightMap);
	}
	else if(_strcmpi(ext,".png")==0)
	{
		vHeightMap.CreateFromPngFile(pHeightMap);
	}
	else
	{
		KGLOG_PROCESS_ERROR(0);
	}

	vHeightMap.SetHeightScale(fLowest,fHeightTotal);
	vHeightMap.ComputeNormal();

	m_lpSceneOutDoorSpaceMgr->SetRegionHeight(&vHeightMap,m_RegionSelectA,m_RegionSelectC);
	m_lpSceneOutDoorSpaceMgr->SetSectionHeight(&vHeightMap,m_RegionSelectA,m_RegionSelectC);
	m_lpSceneOutDoorSpaceMgr->SetZoneHeight(&vHeightMap,m_RegionSelectA,m_RegionSelectC);
	
	


	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneSceneEditor::ApplyBaseMapToSelectedSceneBlock(LPTSTR pBaseMap)
{
    return m_lpSceneOutDoorSpaceMgr->ApplyBaseMapToSelectedSceneBlock(pBaseMap);
}

HRESULT KG3DSceneSceneEditor::AddSelectSceneBlockPoint(int nIndex,D3DXVECTOR3 vPos)
{
	D3DXVECTOR2 v2D;
	v2D.x = vPos.x;
	v2D.y = vPos.z;
	if(nIndex==0)
	{
		m_RegionSelectA = v2D;
		m_RegionSelectC = v2D;
		m_RegionSelectStartPos = v2D;
	}
	else
	{
		m_RegionSelectA = m_RegionSelectStartPos;
		m_RegionSelectC = v2D;
	}
	if (m_RegionSelectA.x > m_RegionSelectC.x)
	{
		swap(m_RegionSelectA.x,m_RegionSelectC.x);
	}
	if (m_RegionSelectA.y > m_RegionSelectC.y)
	{
		swap(m_RegionSelectA.y,m_RegionSelectC.y);
	}
	return S_OK;
}

HRESULT KG3DSceneSceneEditor::EnableRenderSelectedSceneBlock(BOOL bEnable)
{
	return S_OK;
}

std::list<POINT>* KG3DSceneSceneEditor::GetCurSelectedSceneBlock(int nLevel)
{
	return &m_listSceneBlockSelected[nLevel];
}

HRESULT KG3DSceneSceneEditor::CreateSelectedSceneBlock(BOOL bCreateTerrain)
{
	/*m_lpSceneOutDoorSpaceMgr->RequestCreateSceneBlock(BLOCK_TYPE_ZONE   ,&m_listSceneBlockSelected[BLOCK_TYPE_ZONE]   ,bCreateTerrain);
	m_lpSceneOutDoorSpaceMgr->RequestCreateSceneBlock(BLOCK_TYPE_SECTION,&m_listSceneBlockSelected[BLOCK_TYPE_SECTION],bCreateTerrain);
	m_lpSceneOutDoorSpaceMgr->RequestCreateSceneBlock(BLOCK_TYPE_REGION ,&m_listSceneBlockSelected[BLOCK_TYPE_REGION] ,bCreateTerrain);*/
	g_cTerrainBlockModifier.OnAddTerrainBlock(m_lpSceneOutDoorSpaceMgr,m_listSceneBlockSelected[BLOCK_TYPE_SECTION]);
	return S_OK;
}

HRESULT KG3DSceneSceneEditor::DeleteSelectedSceneBlock(BOOL bOnlyDeleteTerrain)
{
	/*m_lpSceneOutDoorSpaceMgr->RequestDeleteSceneBlock(BLOCK_TYPE_ZONE   ,&m_listSceneBlockSelected[BLOCK_TYPE_ZONE]   ,bOnlyDeleteTerrain);
	m_lpSceneOutDoorSpaceMgr->RequestDeleteSceneBlock(BLOCK_TYPE_SECTION,&m_listSceneBlockSelected[BLOCK_TYPE_SECTION],bOnlyDeleteTerrain);
	m_lpSceneOutDoorSpaceMgr->RequestDeleteSceneBlock(BLOCK_TYPE_REGION ,&m_listSceneBlockSelected[BLOCK_TYPE_REGION] ,bOnlyDeleteTerrain);*/
	g_cTerrainBlockModifier.OnDeleteTerrainBlock(m_lpSceneOutDoorSpaceMgr,m_listSceneBlockSelected[BLOCK_TYPE_SECTION]);
	return S_OK;
}

HRESULT KG3DSceneSceneEditor::ComputeRegionSelectionRect()
{	
	list<D3DXVECTOR2>::iterator i = m_listSceneBlockSelectPoint.begin();
	list<D3DXVECTOR2>::iterator iend = m_listSceneBlockSelectPoint.end();

	int k =0;
	while (i!=iend)
	{
		D3DXVECTOR2& v2D = *i;
		D3DXVECTOR3 vPos;
		vPos.x = v2D.x; 
		vPos.z = v2D.y; 
		vPos.y = 0; 

		if(k==0)
		{
			m_RegionSelectA = v2D;
			m_RegionSelectC = v2D;
		}
		else
		{
			m_RegionSelectA.x = min(m_RegionSelectA.x,v2D.x);
			m_RegionSelectA.y = min(m_RegionSelectA.y,v2D.y);
			m_RegionSelectC.x = max(m_RegionSelectC.x,v2D.x);
			m_RegionSelectC.y = max(m_RegionSelectC.y,v2D.y);
		}
		i++;
		k++;
	}
	return S_OK;
}


HRESULT KG3DSceneSceneEditor::RenderSceneRegionSelection()
{
	GraphicsStruct::RenderState Zenable(D3DRS_ZENABLE,FALSE);

	{
		list<D3DXVECTOR2>::iterator i = m_listSceneBlockSelectPoint.begin();
		list<D3DXVECTOR2>::iterator iend = m_listSceneBlockSelectPoint.end();

		int K = 0;
		D3DXVECTOR3 vStart(0,0,0);
		D3DXVECTOR3 vLast(0,0,0);

		while (i!=iend)
		{
			D3DXVECTOR2& v2D = *i;
			D3DXVECTOR3 vPos;
			vPos.x = v2D.x; 
			vPos.z = v2D.y; 
			vPos.y = 0; 

			g_cGraphicsTool.DrawPoint(vPos,0xFFFF0000);
			if(K==0)
			{
				vStart = vPos;
				vLast = vPos;
			}
			else if(K == (int)m_listSceneBlockSelectPoint.size())
			{
				g_cGraphicsTool.DrawLine(&vPos,&vStart,0xFFFF0000,0xFFFF0000);
			}
			else
			{
				g_cGraphicsTool.DrawLine(&vPos,&vLast,0xFFFF0000,0xFFFF0000);
			}


			i++;
			K++;
		}
	}

	{
		D3DXVECTOR3 A,B,C,D;
		A.x = D.x = m_RegionSelectA.x;
		A.z = B.z = m_RegionSelectA.y;
		B.x = C.x = m_RegionSelectC.x;
		C.z = D.z = m_RegionSelectC.y;
		A.y = B.y = C.y = D.y = 0;

		g_cGraphicsTool.DrawLine(&A,&B,0xFF00FF00,0xFF00FF00);
		g_cGraphicsTool.DrawLine(&B,&C,0xFF00FF00,0xFF00FF00);
		g_cGraphicsTool.DrawLine(&C,&D,0xFF00FF00,0xFF00FF00);
		g_cGraphicsTool.DrawLine(&D,&A,0xFF00FF00,0xFF00FF00);
	}

	for (int s=0;s<4;s++)
	{
		DWORD dwColor = 0xFFFFFF00;
		if(s==BLOCK_TYPE_SECTION)
			dwColor = 0xFF00FF00;
		else if(s==BLOCK_TYPE_ZONE)
			dwColor = 0xFFFF0000;

		{
			list<POINT>::iterator i = m_listSceneBlockSelected[s].begin();
			list<POINT>::iterator iend = m_listSceneBlockSelected[s].end();

			D3DXVECTOR3 BoxPos[8];

			while (i!=iend)
			{
				POINT& p = *i;

				if (std::find(m_listTempSelBlock[s].begin(), m_listTempSelBlock[s].end(), p) != m_listTempSelBlock[s].end())
				{
					i++;
					continue;
				}

				D3DXVECTOR4 rect = GetSceneBlockRect(s,p.x,p.y);
				AABBOX box;
				box.AddPosition(D3DXVECTOR3(rect.x         ,-100,rect.y));
				box.AddPosition(D3DXVECTOR3(rect.x + rect.z, 200,rect.y+rect.w));

				g_cGraphicsTool.DrawAABBox(&box,dwColor);
				i++;
			}
		}	

		{
			list<POINT>::iterator i = m_listTempSelBlock[s].begin();
			list<POINT>::iterator iend = m_listTempSelBlock[s].end();

			D3DXVECTOR3 BoxPos[8];

			while (i!=iend)
			{
				POINT& p = *i;

				if (std::find(m_listSceneBlockSelected[s].begin(), m_listSceneBlockSelected[s].end(), p) != m_listSceneBlockSelected[s].end())
				{
					i++;
					continue;
				}

				D3DXVECTOR4 rect = GetSceneBlockRect(s,p.x,p.y);
				AABBOX box;
				box.AddPosition(D3DXVECTOR3(rect.x         ,-100,rect.y));
				box.AddPosition(D3DXVECTOR3(rect.x + rect.z, 200,rect.y+rect.w));

				g_cGraphicsTool.DrawAABBox(&box,dwColor);

				i++;
			}
		}
	}

	return S_OK;

}

HRESULT KG3DSceneSceneEditor::BeginAddPolyVertex()
{
	//m_dwEditState = SCENESTATE_ADDPOLYVERTEX;
	this->SetEditState(SCENESTATE_ADDPOLYVERTEX);
	m_vecPolyVertexes.clear();
	return S_OK;
}

HRESULT KG3DSceneSceneEditor::EndAddPolyVertex(DWORD dwType,KG3DRepresentPoly** ppPolyOut)
{
	HRESULT hr = S_OK;
	KG3DPoly poly;
	KG_PROCESS_ERROR(m_vecPolyVertexes.size()>2);

	for (size_t i=0;i<m_vecPolyVertexes.size();i++)
	{
		D3DXVECTOR3& vPos = m_vecPolyVertexes[i];
		poly.AddVertex(vPos);
	}

	poly.SetHeight(m_fPolyVertexHeight);
	poly.ComputeBBox();

	switch(dwType)
	{
	case 0://转化为游戏逻辑用
		{
			KG3DRepresentPoly* pPoly = NULL;
			hr = g_GetObjectTable().NewOneResourse(REPRESENTOBJECT_POLY,(IKG3DResourceBase**)&pPoly);
			KGLOG_COM_PROCESS_ERROR(hr);

			pPoly->CreateFromScenePoly(&poly);

			//AddRepresentObject(pPoly, TRUE, FALSE,FALSE);
			AddRepresentObject(pPoly, EM_SCENE_ADD_OPTION_SELECTABLE, NULL);

			m_vecPolyVertexes.clear();
			if(ppPolyOut)
			{
				(*ppPolyOut) = pPoly;
			}

			IEKG3DRepresentPoly* pIEoly = pPoly;
			FireEvent2(EVENTEDITORPOLYPLACED,0,0,pIEoly);

			break;
		}
	case 1:
		{
			KG3DRepresentPoly* pPoly = NULL;
			hr = g_GetObjectTable().NewOneResourse(REPRESENTOBJECT_POLY,(IKG3DResourceBase**)&pPoly);
			KGLOG_COM_PROCESS_ERROR(hr);

			pPoly->CreateFromScenePoly(&poly);

			//AddRepresentObject(pPoly, TRUE, FALSE,FALSE);
			AddRepresentObject(pPoly, EM_SCENE_ADD_OPTION_SELECTABLE, NULL);

			m_vecPolyVertexes.clear();
			if(ppPolyOut)
			{
				(*ppPolyOut) = pPoly;
			}
			break;
		}
	}

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneSceneEditor::EndAddPolyVertex(DWORD dwType)
{
	return EndAddPolyVertex(dwType,NULL);
}

HRESULT KG3DSceneSceneEditor::AddPolyVetex(int nIndex,D3DXVECTOR3& vPos)
{
	if(nIndex>=(int)m_vecPolyVertexes.size())
	{
		m_vecPolyVertexes.resize(nIndex+1,D3DXVECTOR3(0,0,0));
	}
	m_vecPolyVertexes[nIndex] = vPos;

	return S_OK;
}

HRESULT KG3DSceneSceneEditor::RenderPoly()
{
	GraphicsStruct::RenderState Zenable(D3DRS_ZENABLE,FALSE);

	{
		D3DXVECTOR3 vStart(0,0,0);
		D3DXVECTOR3 vLast(0,0,0);

		D3DXVECTOR3 vH(0,m_fPolyVertexHeight,0);

		for (size_t i=0;i<m_vecPolyVertexes.size();i++)
		{
			D3DXVECTOR3& vPos = m_vecPolyVertexes[i];
			g_cGraphicsTool.DrawPoint(vPos,0xFFFF0000);

			if(i==0)
			{
				vStart = vPos;
				vLast = vPos;
			}

			if(i >= m_vecPolyVertexes.size() -1)
			{
				g_cGraphicsTool.DrawLine(&vPos,&vStart,0xFFFFFFFF,0xFF0000FF);
				g_cGraphicsTool.DrawLine(&(vPos+vH),&(vStart+vH),0xFFFFFFFF,0xFF0000FF);
			}

			g_cGraphicsTool.DrawLine(&vPos,&vLast,0xFF0000FF,0xFFFFFFFF);
			g_cGraphicsTool.DrawLine(&(vPos+vH),&(vLast+vH),0xFF0000FF,0xFFFFFFFF);
			g_cGraphicsTool.DrawLine(&vPos,&(vPos+vH),0xFF0000FF,0xFF0000FF);

			vLast = vPos;
		}
	}

	return S_OK;
}

FLOAT KG3DSceneSceneEditor::GetPolyHeight()
{
	return m_fPolyVertexHeight;
}

HRESULT KG3DSceneSceneEditor::SetPolyHeight(FLOAT fH)
{
	m_fPolyVertexHeight = fH;
	return S_OK;
}

IEKG3DRepresentPoly* KG3DSceneSceneEditor::GetSelectedPoly()
{
	if(this->GetSelectionTool().GetSelectionCount() <= 0)
		return NULL;

	/*KG3DSceneEntityList::iterator istart = m_listSelectedEntity.begin();
	KG3DSceneEntity Entity = *istart;*/

	TypeEntityAccessProxy& proxy = this->GetSelectionTool().GetSelectionAccessProxy();
	proxy.Begin();
	KG3DSceneEntity Entity = proxy.GetCur();

	if(Entity.GetType()==SCENEENTITY_OBJECT)
	{
		KG3DRepresentObject* pObject = static_cast<KG3DRepresentObject*>(Entity.m_pPointer);
		if(!pObject)
			return NULL;
		if (pObject->GetType() == REPRESENTOBJECT_POLY)
		{
			KG3DRepresentPoly* pPoly = static_cast<KG3DRepresentPoly*>(pObject);
			return pPoly;
		}
	}

	return NULL;
}

INT KG3DSceneSceneEditor::GetLogicalPolyIntersectRect(IEKG3DRepresentPoly* pPolyArray[],INT nMax,D3DXVECTOR2 A,D3DXVECTOR2 C)
{
	int nCount = 0;
	KG3DSceneEntityList listEntity;
	
    m_lpSceneOutDoorSpaceMgr->GetEntityIntersectRect(&listEntity,A,C,FALSE);

	KG3DSceneEntityList::iterator i = listEntity.begin();
	KG3DSceneEntityList::iterator iend = listEntity.end();

	while (i!=iend)
	{
		KG3DSceneEntity& Entity = *i;

		if(Entity.GetType()==SCENEENTITY_OBJECT)
		{
			KG3DRepresentObject* pObject = static_cast<KG3DRepresentObject*>(Entity.m_pPointer);
			if (pObject->GetType() == REPRESENTOBJECT_POLY)
			{
				KG3DRepresentPoly* pPoly = static_cast<KG3DRepresentPoly*>(pObject);
				pPolyArray[nCount] = pPoly;
				nCount++;
			}
			else if (pObject->GetType() == REPRESENTOBJECT_PVS)
			{
				KG3DRepresentObjectPVS* pPvs = static_cast<KG3DRepresentObjectPVS*>(pObject);
				vector<KG3DRepresentPoly*> vPoly;
				pPvs->GetPolyIntersectRect(vPoly,A,C);
				for (size_t i = 0;i<vPoly.size();i++)
				{
					KG3DRepresentPoly* pPoly = vPoly[i];
					pPolyArray[nCount] = pPoly;
					nCount++;
				}

			}

		}
		i++;
	}
	return nCount;
}
BOOL KG3DSceneSceneEditor::IsRenderPoly()
{
	return g_bRenderRepresentPoly;
}
HRESULT KG3DSceneSceneEditor::EnableRenderPoly(BOOL bEnable)
{
	g_bRenderRepresentPoly = bEnable;
	return S_OK;
}

HRESULT KG3DSceneSceneEditor::EnableRenderAlertRange(BOOL bEnable)
{
	g_bRenderAlertRange = bEnable;
	return S_OK;
}

HRESULT KG3DSceneSceneEditor::EnableRenderAttackRange(BOOL bEnable)
{
	g_bRenderAttackRange = bEnable;
	if(g_bRenderAttackRange)
	{
		KG3DSceneEntityList::iterator i = m_listRenderEntity.begin();
		while(i!=m_listRenderEntity.end())
		{
			KG3DSceneEntity &Entity = *i;

			if (Entity.GetType()==SCENEENTITY_OBJECT)
			{

				KG3DRepresentObject* pObject = static_cast<KG3DRepresentObject*>(Entity.m_pPointer);

				if(pObject->GetType()==REPRESENTOBJECT_NPC)
				{
					IEKG3DRepresentNPC* pRepNPC = GetIEKG3DRepresentNPC(pObject);
					pRepNPC->SetbAttackRange(bEnable);
				}
			}
			i++;
		}
	}

	return S_OK;
}


HRESULT KG3DSceneSceneEditor::SaveKG3DEnvironment( LPSTR pFileName )
{
	IIniFile *pIniFile = g_OpenIniFile(pFileName, false, true);
	if (!pIniFile)
	{
		g_CreateFile(pFileName);
		pIniFile = g_OpenIniFile(pFileName, false, true);
	}
	_KG3D_DEBUG_FAIL_REPORT_MSG(pIniFile, "INI文件指针创建失败，Environment不能保存");
	KG_PROCESS_ERROR(pIniFile);

	GetEnvironment().SaveMapSetting( pIniFile );

	pIniFile->Save(pFileName);
	KG_COM_RELEASE(pIniFile);

	return S_OK;
Exit0:
	KG_COM_RELEASE(pIniFile);
	return E_FAIL;
}

HRESULT KG3DSceneSceneEditor::LoadKG3DEnvironment( LPSTR pFileName )
{
	IIniFile *pIniFile = g_OpenIniFile(pFileName);
	_KG3D_DEBUG_FAIL_REPORT_MSG(pIniFile, "INI文件指针创建失败，Environment不能读取");
	KG_PROCESS_ERROR(pIniFile);
	//if ( m_lpEnvironment )
	{
		HRESULT hr = GetEnvironment().LoadMapSetting(pIniFile, NULL);
		KG_COM_PROCESS_ERROR(hr);
	}
	KG_COM_RELEASE(pIniFile);
	return S_OK;
Exit0:
	KG_COM_RELEASE(pIniFile);
	return E_FAIL;
}

HRESULT KG3DSceneSceneEditor::MoveScene(int nXStart,int nZStart)
{
	ForceLoadAllEntity();

	KG3DSceneEntityList::iterator i = m_listRenderEntity.begin();
	KG3DSceneEntityList::iterator iend = m_listRenderEntity.end();
	while (i!=iend)
	{
		KG3DSceneEntity& Entity = *i;
		D3DXVECTOR3 vPos;
		Entity.GetTranslation(&vPos);
		vPos.x += X;
		vPos.z += Z;
		Entity.SetTranslation(&vPos);
		Entity.FrameMove();

		i++;
	}

	return S_OK;
}

HRESULT KG3DSceneSceneEditor::AddOneMap(LPCTSTR pFileName)
{
	return LoadFromFile(pFileName,0,0);
}

HRESULT KG3DSceneSceneEditor::LoadDefaultTerrain( KG3DTERRAIN_LOAD_PARAM* pLoadParam )
{
    return m_lpSceneOutDoorSpaceMgr->LoadDefaultTerrain(pLoadParam);
}

HRESULT KG3DSceneSceneEditor::AddPolyVetexInTopView(D3DXVECTOR3& vInter)
{
	int n = (int) m_vecPolyVertexes.size();
	vInter.y -= 60.0f;
	AddPolyVetex(n,vInter);
	return S_OK;
}

void KG3DSceneSceneEditor::RenderForPVSGeneration()
{
	D3DXVECTOR3 vec3Pos;
	s_OutWnd->GetCamera().GetPosition(&vec3Pos);
	;
}
BOOL KG3DSceneSceneEditor::IsSceneSoundEnable()
{
	return g_bPlaySoundBall;
}

HRESULT KG3DSceneSceneEditor::EnableSceneSound(BOOL bEnable)
{
	g_bPlaySoundBall = bEnable;
	return S_OK;
}

BOOL KG3DSceneSceneEditor::IsSceneSoundVisiable()
{
	return g_bRenderSoundBall;
}

HRESULT KG3DSceneSceneEditor::VisiableSceneSound(BOOL bVisiable)
{
	g_bRenderSoundBall = bVisiable;
	return S_OK;
}

BOOL KG3DSceneSceneEditor::IsSoundRangeVisible()
{
	return g_SoundBallConfig.m_bRenderSoundBallRange;
}

void KG3DSceneSceneEditor::VisibleSoundRange(BOOL bVisible)
{
	g_SoundBallConfig.m_bRenderSoundBallRange = bVisible ? true : false;
}

HRESULT KG3DSceneSceneEditor::BeginPVSRender()
{
	HRESULT hr = E_FAIL;
	HRESULT hResult = E_FAIL;

	KG_PROCESS_ERROR(m_PVSParameter.pRenderTargetTexture);
	KG_PROCESS_ERROR(m_PVSParameter.pDepth);

	hr = g_pd3dDevice->GetRenderTarget(0, &m_PVSParameter.pRenderTargetSave);
	KG_COM_PROCESS_ERROR(hr);

	hr = g_pd3dDevice->GetDepthStencilSurface(&m_PVSParameter.pDepthSave);
	KG_COM_PROCESS_ERROR(hr);

	hr = g_pd3dDevice->GetViewport(&m_PVSParameter.vpSave);
	KG_COM_PROCESS_ERROR(hr);

	hr = m_PVSParameter.pRenderTargetTexture->GetSurfaceLevel(0, &m_PVSParameter.pRenderSurface);
	KG_COM_PROCESS_ERROR(hr);

	hr = g_pd3dDevice->SetRenderTarget(0, m_PVSParameter.pRenderSurface);
	KG_COM_PROCESS_ERROR(hr);

	hr = g_pd3dDevice->SetDepthStencilSurface(m_PVSParameter.pDepth);
	KG_COM_PROCESS_ERROR(hr);

	hr = g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 
		0xff0000ff, 1.0f, 0);
	KG_COM_PROCESS_ERROR(hr);

	hr = g_pd3dDevice->BeginScene();
	KG_COM_PROCESS_ERROR(hr);

	hResult = S_OK;
Exit0:
	KG_COM_RELEASE(m_PVSParameter.pRenderSurface);
	if (FAILED(hResult))
	{
		KG_COM_RELEASE(m_PVSParameter.pDepthSave);
		KG_COM_RELEASE(m_PVSParameter.pRenderTargetSave);
	}
	return hResult;
}

HRESULT KG3DSceneSceneEditor::EndPVSRender()
{
	HRESULT hr = E_FAIL;
	HRESULT hResult = E_FAIL;
	hr = g_pd3dDevice->EndScene();
	KG_COM_PROCESS_ERROR(hr);

	KG_PROCESS_ERROR(m_PVSParameter.pRenderTargetSave);
	KG_PROCESS_ERROR(m_PVSParameter.pDepthSave);

	static bool bSaveToFile = true;
	static int nFileCount = 0;
	TCHAR strFileName[MAX_PATH];
	if (bSaveToFile)
	{
		sprintf(strFileName, "d:\\%d.jpg", nFileCount++);
		D3DXSaveTextureToFile(strFileName, D3DXIFF_JPG, m_PVSParameter.pRenderTargetTexture, NULL);
	}

	hr = g_pd3dDevice->SetRenderTarget(0, m_PVSParameter.pRenderTargetSave);
	KG_COM_PROCESS_ERROR(hr);

	hr = g_pd3dDevice->SetDepthStencilSurface(m_PVSParameter.pDepthSave);
	KG_COM_PROCESS_ERROR(hr);

	hr = g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 
		0xff00ff00, 1.0f, 0);
	KG_COM_PROCESS_ERROR(hr);

	hr = g_pd3dDevice->SetViewport(&m_PVSParameter.vpSave);
	KG_COM_PROCESS_ERROR(hr);

	hr = g_pd3dDevice->BeginScene();
	KG_COM_PROCESS_ERROR(hr);

	hr = g_cGraphicsTool.DrawScreenRect(&D3DXVECTOR2(0.0f, 0.0f),
		&D3DXVECTOR2(static_cast<float>(m_PVSParameter.vpCurrent.Width), static_cast<float>(m_PVSParameter.vpCurrent.Height)), 
		0.0f,
		0xffffffff,
		m_PVSParameter.pRenderTargetTexture);


	hr = g_pd3dDevice->EndScene();
	KG_COM_PROCESS_ERROR(hr);

	hResult = S_OK;
Exit0:
	KG_COM_RELEASE(m_PVSParameter.pDepthSave);
	KG_COM_RELEASE(m_PVSParameter.pRenderTargetSave);
	return hResult;
}

BOOL KG3DSceneSceneEditor::GetPVSState()
{
	return m_PVSParameter.bUsePVS;
}

void KG3DSceneSceneEditor::EnablePVS(BOOL bEnable)
{
	m_PVSParameter.bUsePVS = bEnable;
}

float KG3DSceneSceneEditor::GetLoadingProcess()
{
	float Knext = m_fLoadingState;
	KGLOG_PROCESS_ERROR(m_lpSceneOutDoorSpaceMgr);

	if(m_fLoadingState<=0.0001F)
	{
		m_lpSceneOutDoorSpaceMgr->PauseMultiThreadLoad(TRUE);
	}
	while ( (Knext - m_fLoadingState<=0.1F)&&(Knext<1.0F) )
	{
		Knext = GetLoadingProcessTick();
		m_lpSceneOutDoorSpaceMgr->FrameMove(4096);
	}

	if(Knext>=1.0F)
	{
		m_lpSceneOutDoorSpaceMgr->PauseMultiThreadLoad(FALSE);
	}
	m_fLoadingState = Knext;

Exit0:
	return Knext;
}

float KG3DSceneSceneEditor::GetLoadingProcessTick()
{
	HRESULT hr = E_FAIL;
	if(m_nNumPreloadModels)
	{
		if(m_PreloadModelsName.size())
		{
			string name = *m_PreloadModelsName.begin();
			m_PreloadModelsName.pop_front();

			KG3DModel *pModel = NULL;
			hr = g_cModelTable.LoadResourceFromFile(name.c_str(), 0, 0, (IKG3DResourceBase**)&pModel);
			if (FAILED(hr))
			{
				KGLogPrintf(KGLOG_ERR, "Preload model %s failed.\n", name.c_str());
			}
			else
			{
				m_PreloadModels.push_back(pModel);
			}

			return (m_PreloadModels.size() * 1.0F / m_nNumPreloadModels) * 0.5F;
		}
		else
		{
			m_nNumPreloadModels = 0;
			return 0.5f;
		}
	}
	else
	{
		m_dwPreLoadingFrame++;

		if(m_dwPreLoadingFrame<=1)
		{
			m_GameGPS.GetPos(m_vCameraPos);
			D3DXVECTOR2 vPos(m_vCameraPos.x,m_vCameraPos.z);
			KGLogPrintf(KGLOG_INFO,"Loading Position:%.2f %.2f",vPos.x,vPos.y);
	        D3DXVECTOR2 vDir(0,0);

			m_lpSceneOutDoorSpaceMgr->ProcessDynamicLoad(TRUE,vPos,vDir,10000,8000,1000,this);
		}

		return m_lpSceneOutDoorSpaceMgr->GetLoadingProcess() * 0.5F + 0.5F;
	}

}

HRESULT KG3DSceneSceneEditor::SetSceneSettingData( DWORD dwEnum, DWORD_PTR dwData )
{
	HRESULT hr = E_FAIL;

	switch(dwEnum)
	{
	case scene_setting_enable_render_terrain_mask:
		{
			KG3DTerrainRender* pTR = m_EntityMgr.GetTerrainRender();
			KG_PROCESS_ERROR(NULL != pTR && !g_bClient);

			BOOL bEnable = dwData != 0;

			if (bEnable)
			{
				INT StartX, StartY, XWidth, YWidth;
				hr = m_lpSceneOutDoorSpaceMgr->GetSceneRect(StartX, StartY, XWidth, YWidth);
				KG_COM_PROCESS_ERROR(hr);
				D3DXVECTOR4 vSceneRect((float)(StartX),(float)(StartY),(float)(XWidth),(float)(YWidth));
				vSceneRect *= 100;//因为取出来的单位是米
				hr = pTR->EnableRenderMask(bEnable, _kg_to_smaller_size_cast<DWORD>(dwData), &vSceneRect);

			}
			else
				hr = pTR->EnableRenderMask(bEnable, 0, NULL);
			KG_COM_PROCESS_ERROR(hr);

			hr = m_lpSettings->SetSceneSettingData(dwEnum, dwData);
			_ASSERTE(SUCCEEDED(hr));
			break;
		}
	default:
		hr = m_lpSettings->SetSceneSettingData(dwEnum, dwData);
	}

Exit0:	
	return hr;
}

HRESULT KG3DSceneSceneEditor::GetSceneSettingData( DWORD dwEnum, DWORD_PTR* pdwData )
{
	return m_lpSettings->GetSceneSettingData(dwEnum, pdwData);
}

HRESULT KG3DSceneSceneEditor::ReplaceSelectedObjectWithInHand()
{
	HRESULT hr = E_FAIL;

	

	IEKG3DRepresentObject* pObjectInHandle = NULL;
	KG3DSceneObjectPlacementTool* pObjPlacementTool = this->GetObjectPlacementTool();
	KG_PROCESS_ERROR(NULL != pObjPlacementTool);

	hr = pObjPlacementTool->GetObjectInHandle(&pObjectInHandle);
	_ASSERTE(SUCCEEDED(hr));
	
	/*KG3DSceneEntityList::iterator i = m_listSelectedEntity.begin();
	KG3DSceneEntityList::iterator iend = m_listSelectedEntity.end();
	while ( i!=iend )
	{
		KG3DSceneEntity Entity = *i;*/
	TypeEntityAccessProxy& proxy = this->GetSelectionTool().GetSelectionAccessProxy();
	for(proxy.Begin(); ! proxy.IsEnd(); proxy.StepForward())
	{
		KG3DSceneEntity Entity = proxy.GetCur();

		D3DXVECTOR3 vPos,vScale,vRotXYZ;
		D3DXQUATERNION vRot;
		Entity.GetTranslation(&vPos);
		Entity.GetScaling(&vScale);
		Entity.GetRotation(&vRot,&vRotXYZ);

		/*m_lpHandleObject*/pObjectInHandle->SetTranslation(&vPos);
		/*m_lpHandleObject*/pObjectInHandle->SetScaling(&vScale);
		/*m_lpHandleObject*/pObjectInHandle->SetRotation(&vRot);

		pObjPlacementTool->DoPlaceObject();

		//i++;
	}

	DeleteSelectedEntity();
	hr = S_OK;
Exit0:
	return hr;
}

void KG3DSceneSceneEditor::SetUseBoneBox(BOOL val)
{
	KG3DModel::m_Static_bUseBoneBox = val;
}

BOOL KG3DSceneSceneEditor::GetUseBoneBox()
{
	return KG3DModel::m_Static_bUseBoneBox;
}

IKG3DScene* KG3DSceneSceneEditor::GetIInterface()
{
	return this;
}
//////////////////////////////////////////////////////////////////////////
HRESULT KG3DSceneSceneEditor::SetOpenAddTexture(BOOL bFlag)
{
	g_bOpenSelectTexture = bFlag;
	return S_OK;
}

BOOL KG3DSceneSceneEditor::GetOpenAddTexture() const
{
	return g_bOpenSelectTexture;
}
/////////////////////////////////////////////////////////
//void KG3DSceneSceneEditor::GetfModelLodLevel(float* fLodLevel)
//{
//	if(fLodLevel)
//		*fLodLevel = g_fModelLODLevelHighToMiddle;
//}

//void KG3DSceneSceneEditor::SetfModelLodLevel(float fLodLevel)
//{
//	g_fModelLODLevelHighToMiddle = fLodLevel;
//}

//////////////////////////////////////////////////////////////////////////
//pvs
HRESULT	KG3DSceneSceneEditor::GetCurrentSelectedObjectPVS(IEKG3DRepresentPVS** ppPVS)
{
	HRESULT hr = E_FAIL;
	KG3DRepresentObject* pObject = (KG3DRepresentObject*)(GetFirstSelectedObject());
	KG_PROCESS_ERROR(pObject);

	KG_PROCESS_ERROR(pObject->GetType() == REPRESENTOBJECT_PVS);
	KG3DRepresentObjectPVS* pPVS = static_cast<KG3DRepresentObjectPVS*>(pObject);

	(*ppPVS) = pPVS;
	hr = S_OK;
Exit0:
	return hr;
}


HRESULT KG3DSceneSceneEditor::ConvertCurrentSelectedObjectToPVS(IEKG3DRepresentPVS** ppPVS)
{
	HRESULT hResult  = E_FAIL;
	HRESULT hRetCode = E_FAIL;

	KG3DRepresentObjectPVS* pPVS = NULL;

	KG3DRepresentObject* pObject = (KG3DRepresentObject*)(GetFirstSelectedObject());
	KG_PROCESS_ERROR(pObject);

	hRetCode = g_GetObjectTable().NewOneResourse(REPRESENTOBJECT_PVS,(IKG3DResourceBase**)&pPVS);
	KGLOG_COM_PROCESS_ERROR(hRetCode);

	pPVS->Copy(pObject);
	(*ppPVS) = pPVS;
	pPVS->FrameMove();
	pPVS->ComputeBBox(); 

	DeleteSelectedEntity();
	//m_listSelectedEntity.clear();
	this->GetSelectionTool().ClearSelection();

    pPVS->CheckLoadIndoorWall();

    //AddRepresentObject(pPVS, TRUE, TRUE, TRUE);
	AddRepresentObject(pPVS, EM_SCENE_ADD_OPTION_SELECTABLE | EM_SCENE_ADD_OPTION_SELECTED | EM_SCENE_ADD_OPTION_TO_FILE, NULL);

	hResult = S_OK;
Exit0 :
	return hResult;
}

HRESULT KG3DSceneSceneEditor::MoveSceneObjectIntoPVS(IEKG3DRepresentPVS* pPVS)
{
	if(!pPVS)
		return E_FAIL;

	KG3DRepresentObjectPVS* pPVSObject = static_cast<KG3DRepresentObjectPVS*>(pPVS);

	/*KG3DSceneEntityList::iterator i = m_listSelectedEntity.begin();
	while (i!=m_listSelectedEntity.end())
	{
		KG3DSceneEntity Entity = *i;*/
	TypeEntityAccessProxy& proxy = this->GetSelectionTool().GetSelectionAccessProxy();
	for(proxy.Begin(); ! proxy.IsEnd(); proxy.StepForward())
	{
		KG3DSceneEntity Entity = proxy.GetCur();
		if (Entity.GetType() == SCENEENTITY_OBJECT)
		{
			KG3DRepresentObject* pObject = static_cast<KG3DRepresentObject*>(Entity.m_pPointer);
			if(pPVSObject != pObject)
			{
				pPVSObject->m_setObjectInside.insert(pObject);
				pObject->SetParentObject(pPVSObject);

				m_listRenderEntity.remove(Entity);
				
				//this->GetSelectionTool().SetSelectable(Entity, FALSE);//<Selectable Refactor>
				this->InsertEntitySelectable(Entity, FALSE);
			}
		}
		//i++;
	}

	//m_listSelectedEntity.clear();
	this->GetSelectionTool().ClearSelection();

	return S_OK;
}

HRESULT KG3DSceneSceneEditor::MoveObjectFromPVSToScene(IEKG3DRepresentPVS* pPVS)
{
	return S_OK;
}


HRESULT	KG3DSceneSceneEditor::GetCurrentSelectedObjectAntiPortal(IEKG3DRepresentAntiPortal** ppAntiPortal)
{
	HRESULT hr = E_FAIL;
	KG3DRepresentObject* pObject = (KG3DRepresentObject*)(GetFirstSelectedObject());
	KG_PROCESS_ERROR(pObject);

	KG_PROCESS_ERROR(pObject->GetType() == REPRESENTOBJECT_ANTIPORTAL);
	KG3DRepresentObjectAntiPortal* pAntiPortal = static_cast<KG3DRepresentObjectAntiPortal*>(pObject);

	(*ppAntiPortal) = pAntiPortal;
	hr = S_OK;
Exit0:
	return hr;
}

HRESULT KG3DSceneSceneEditor::ConvertCurrentSelectedObjectToAntiPortal(IEKG3DRepresentAntiPortal** ppAntiPortal)
{
	HRESULT hr = E_FAIL;
	KG3DRepresentObjectAntiPortal* pAntiPortal = NULL;
	KG3DRepresentObject* pObject = (KG3DRepresentObject*)(GetFirstSelectedObject());
	KG_PROCESS_ERROR(pObject);

	hr = g_GetObjectTable().NewOneResourse(REPRESENTOBJECT_ANTIPORTAL,(IKG3DResourceBase**)&pAntiPortal);
	KGLOG_COM_PROCESS_ERROR(hr);

	pAntiPortal->Copy(pObject);
	(*ppAntiPortal) = pAntiPortal;
	pAntiPortal->ComputeBBox();
	//////////////////////////////////////////////////////////////////////////
	pAntiPortal->m_fHeight = pAntiPortal->m_BBox.GetHeight() * 0.9F;
	float w = min(pAntiPortal->m_BBox.GetWidth(),pAntiPortal->m_BBox.GetLength());
	pAntiPortal->m_fWidthBottom = pAntiPortal->m_fWidthTop = w * 0.9F;
	pAntiPortal->m_dwAntiPortalType = 1;

	DeleteSelectedEntity();
	//m_listSelectedEntity.clear();
	this->GetSelectionTool().ClearSelection();

    //AddRepresentObject(pAntiPortal, TRUE, TRUE,FALSE);
	AddRepresentObject(pAntiPortal, EM_SCENE_ADD_OPTION_SELECTABLE | EM_SCENE_ADD_OPTION_SELECTED, NULL);
	hr = S_OK;
Exit0:
	return hr;
}

HRESULT KG3DSceneSceneEditor::CheckTerrianSelection()
{
	float kWidth = 100;

	m_vPositionSceneSelectionA.x = (int)(m_vPositionSceneSelectionA.x / kWidth)*kWidth;
	m_vPositionSceneSelectionA.z = (int)(m_vPositionSceneSelectionA.z / kWidth)*kWidth;
	m_vPositionSceneSelectionC.x = (int)(m_vPositionSceneSelectionC.x / kWidth)*kWidth;
	m_vPositionSceneSelectionC.z = (int)(m_vPositionSceneSelectionC.z / kWidth)*kWidth;

	return S_OK;
}

HRESULT KG3DSceneSceneEditor::DeleteNodeFromScene(DWORD dwNodeType)
{
	HRESULT hr = E_FAIL;
	//m_listTempSelectedEntity.clear();//<m_listTempSelectedEntity Refactor>
	
	//m_listSelectedEntity.clear();
	this->GetSelectionTool().ClearSelection();
	KGLOG_PROCESS_ERROR(m_lpSceneOutDoorSpaceMgr);

	if (dwNodeType == NODETYPE_ROAD)
	{
		KGLOG_PROCESS_ERROR(m_lpSceneOutDoorSpaceMgr->m_lpRoadMgr);
		KG3DSerpentine* pRoad = NULL;	
		pRoad = m_lpSceneOutDoorSpaceMgr->m_lpRoadMgr->GetCurSerpentine();
		KGLOG_PROCESS_ERROR(pRoad);
		{
			KG3DSceneSelectionTool& selTool = this->GetSelectionTool();
			list<KG3DRepresentObjectNode*>::iterator i = pRoad->m_listNode.begin();
			while (i != pRoad->m_listNode.end())
			{
				KG3DRepresentObject* pObject =  (*i);
				KG3DSceneEntity Entity(SCENEENTITY_OBJECT,pObject);
				
				//m_listSelectedEntity.push_back(Entity);
				selTool.SetSelected(Entity, TRUE);
				i++;
			}
		}


		/*	KG3DTerrainRender* pTerrainRender = NULL; 
		pTerrainRender = m_EntityMgr.GetTerrainRender();
		if (pTerrainRender)
		{
			pTerrainRender->CleanRoad();
		}*/
	}
	else if (dwNodeType == NODETYPE_RIVER)
	{
		KGLOG_PROCESS_ERROR(m_lpSceneOutDoorSpaceMgr->m_lpRiverMgr);
		KG3DSerpentine* pRiver = m_lpSceneOutDoorSpaceMgr->m_lpRiverMgr->GetCurSerpentine();
		KGLOG_PROCESS_ERROR(pRiver);
		{
			KG3DSceneSelectionTool& selTool = this->GetSelectionTool();
			list<KG3DRepresentObjectNode*>::iterator i = pRiver->m_listNode.begin();
			while (i != pRiver->m_listNode.end())
			{
				KG3DRepresentObject* pObject =  (*i);
				KG3DSceneEntity Entity(SCENEENTITY_OBJECT,pObject);

				//m_listSelectedEntity.push_back(Entity);
				selTool.SetSelected(Entity, TRUE);
				i++;
			}
		}
		/*KG3DTerrainRender* pTerrainRender = NULL; 
		pTerrainRender = m_EntityMgr.GetTerrainRender();
		if (pTerrainRender)
		{
			pTerrainRender->CleanRiver();
		}*/
	}	
	DeleteSelectedEntity();
	SetEditState(SCENESTATE_SELECT);
	hr = S_OK;
Exit0:
	return hr;
}

HRESULT KG3DSceneSceneEditor::AddNodeToScene(DWORD dwNodeType)
{
	HRESULT hr = E_FAIL;
	float nSize = 10 ;
	KG3DRepresentObjectNode* pObjectNode = NULL;
	KGLOG_PROCESS_ERROR(m_lpSceneOutDoorSpaceMgr);
	//SAFE_RELEASE(m_lpHandleObject);//新的放置工具不能直接操作HandleObject

	{
		KG3DRepresentObject* pObject = NULL;

		hr = g_GetObjectTable().NewOneResourse(REPRESENTOBJECT_NODE,(IKG3DResourceBase**)&pObject);
		KGLOG_COM_PROCESS_ERROR(hr);

		pObjectNode = static_cast<KG3DRepresentObjectNode*>(pObject);
	}
	pObjectNode->SetNodeType(dwNodeType);
	if (dwNodeType == NODETYPE_ROAD)
	{
		KGLOG_PROCESS_ERROR(m_lpSceneOutDoorSpaceMgr->m_lpRoadMgr);
		KG3DSerpentine* pRoad = m_lpSceneOutDoorSpaceMgr->m_lpRoadMgr->GetCurSerpentine();
		KGLOG_PROCESS_ERROR(pRoad);
		nSize = pRoad->GetNodeSize();
	}
	else if(dwNodeType == NODETYPE_RIVER)
	{
		KGLOG_PROCESS_ERROR(m_lpSceneOutDoorSpaceMgr->m_lpRiverMgr);
		KG3DSerpentine* pRiver = m_lpSceneOutDoorSpaceMgr->m_lpRiverMgr->GetCurSerpentine();
		KGLOG_PROCESS_ERROR(pRiver);
		nSize = pRiver->GetNodeSize();
	}

	pObjectNode->SetScaling(&D3DXVECTOR3(nSize,nSize,nSize));

	//新的放置工具改成这样操作
	hr = this->SetObjectInHandle(pObjectNode, TRUE);
	KG_COM_PROCESS_ERROR(hr);
	//SetEditState(SCENESTATE_PLACEOBJ);
	return S_OK;
Exit0:
	SAFE_RELEASE(pObjectNode);
	return E_FAIL;
}

HRESULT KG3DSceneSceneEditor::AddNodeToScene(KG3DRepresentObjectNode** ppNode,D3DXVECTOR3* pvTrans,D3DXVECTOR3* pvScale)
{
	HRESULT hr = E_FAIL;
	KG3DRepresentObject* pObject;
	hr = g_GetObjectTable().NewOneResourse(REPRESENTOBJECT_NODE,(IKG3DResourceBase**)&pObject);
	KGLOG_COM_PROCESS_ERROR(hr);

	if (pvTrans)
		pObject->SetTranslation(pvTrans);
	if (pvScale)
		pObject->SetScaling(pvScale);

	//AddRepresentObject(pObject, TRUE, FALSE,FALSE);
	AddRepresentObject(pObject, EM_SCENE_ADD_OPTION_SELECTABLE, NULL);

	(*ppNode) = static_cast<KG3DRepresentObjectNode*>(pObject);

	hr = S_OK;
Exit0:
	return hr;
}

void KG3DSceneSceneEditor::SetRenderNode(BOOL bRender)
{
	g_bRenderRoadNode = bRender;
}

BOOL KG3DSceneSceneEditor::GetRenderNode()
{
	return g_bRenderRoadNode;
}

HRESULT CheckResourceFormatSupport(LPDIRECT3DDEVICE9 pd3dDevice, D3DFORMAT fmt, D3DRESOURCETYPE ResType, DWORD dwUsage)
{
	HRESULT hr = E_FAIL;
	HRESULT hrResult = E_FAIL;
	LPDIRECT3D9 pD3D = NULL;
	D3DCAPS9 DevCaps;
	D3DDISPLAYMODE DisplayMode;

	KGLOG_PROCESS_ERROR(pd3dDevice);

	hr = pd3dDevice->GetDirect3D(&pD3D);
	KGLOG_COM_PROCESS_ERROR(hr);

	hr = pd3dDevice->GetDeviceCaps(&DevCaps);
	KGLOG_COM_PROCESS_ERROR(hr);

	hr = pD3D->GetAdapterDisplayMode(DevCaps.AdapterOrdinal, &DisplayMode);
	KGLOG_COM_PROCESS_ERROR(hr);

	hr = pD3D->CheckDeviceFormat(DevCaps.AdapterOrdinal, DevCaps.DeviceType, DisplayMode.Format, dwUsage, ResType, fmt);
	KGLOG_COM_PROCESS_ERROR(hr);

	hrResult = S_OK;
Exit0:
	SAFE_RELEASE(pD3D);
	return hrResult;
}

/* move to post render 
//HRESULT KG3DSceneSceneEditor::CreateSMZTexture(KG3DSceneOutputWnd *pWnd)
//{
//	HRESULT hr = S_OK;
//	SAFE_RELEASE(m_lpSMZTexture);
//	SAFE_RELEASE(m_pSMColorTexture);
//	SAFE_RELEASE(m_pSMColorSurface);
//	SAFE_RELEASE(m_pSMZSurface);
//
//	
//	D3DDISPLAYMODE mode;
//	g_pd3dDevice->GetDisplayMode(0,&mode);
//
//	int nWidth = mode.Width;
//	int nHeight = mode.Height;
//
//	D3DFORMAT zFormat = D3DFMT_D24S8;
//
//	if(FAILED(CheckResourceFormatSupport(g_pd3dDevice, zFormat, D3DRTYPE_TEXTURE, D3DUSAGE_DEPTHSTENCIL)))
//	{
//		MessageBox(NULL, _T("Device/driver does not support hardware shadow maps!"), _T("ERROR"), MB_OK|MB_SETFOREGROUND|MB_TOPMOST);
//		return E_FAIL;
//	}
//
//	D3DFORMAT colorFormat = D3DFMT_A8R8G8B8;
//	if( (zFormat == D3DFMT_D16) ||
//		(zFormat == D3DFMT_D15S1) )
//		colorFormat = D3DFMT_R5G6B5;
//
//	m_dwShadowmapSize = 1024;
//
//	//hr = g_pd3dDevice->CreateTexture( nWidth, nHeight, 1, D3DUSAGE_DEPTHSTENCIL, (D3DFORMAT)MAKEFOURCC('R','A','W','Z'), D3DPOOL_DEFAULT, &m_lpSMZTexture, NULL);
//	hr = g_pd3dDevice->CreateTexture(nWidth, nHeight, 1, D3DUSAGE_RENDERTARGET, colorFormat, 
//		D3DPOOL_DEFAULT, &m_pSMColorTexture, NULL);
//	KGLOG_COM_PROCESS_ERROR(hr);
//
//	hr = g_pd3dDevice->CreateTexture( nWidth, nHeight, 1, D3DUSAGE_DEPTHSTENCIL,(D3DFORMAT)MAKEFOURCC('I','N','T','Z'), 
//		D3DPOOL_DEFAULT, &m_lpSMZTexture, NULL);
//	KGLOG_COM_PROCESS_ERROR(hr);
//
//	hr = m_pSMColorTexture->GetSurfaceLevel(0, &m_pSMColorSurface);
//	KGLOG_COM_PROCESS_ERROR(hr);
//
//	hr = m_lpSMZTexture->GetSurfaceLevel(0, &m_pSMZSurface);
//	KGLOG_COM_PROCESS_ERROR(hr);
//
//	KGLOG_PROCESS_ERROR(m_pSMColorSurface);
//
//	KGLOG_PROCESS_ERROR(m_pSMZSurface);
//
//	//////////////////////////////////////////////////////////////////////////
//	if(m_bUseShadowMap)
//	{
//		hr = g_pd3dDevice->CreateTexture( m_dwShadowmapSize, m_dwShadowmapSize, 1, D3DUSAGE_RENDERTARGET, colorFormat, 
//			D3DPOOL_DEFAULT, &m_lpSMShadowMapColor, NULL);
//		KGLOG_COM_PROCESS_ERROR(hr);
//
//		hr = m_lpSMShadowMapColor->GetSurfaceLevel(0, &m_lpSMShadowMapColorSurface);
//		KGLOG_COM_PROCESS_ERROR(hr);
//
//		KGLOG_PROCESS_ERROR(m_lpSMShadowMapColorSurface);
//
//		/*
//		hr = g_pd3dDevice->CreateTexture( m_dwShadowmapSize, m_dwShadowmapSize, 1, D3DUSAGE_DEPTHSTENCIL,(D3DFORMAT)MAKEFOURCC('I','N','T','Z'), 
//		D3DPOOL_DEFAULT, &m_lpSMShadowMapColor, NULL);
//		KGLOG_COM_PROCESS_ERROR(hr);
//
//		hr = g_pd3dDevice->CreateTexture( m_dwShadowmapSize, m_dwShadowmapSize, 1, D3DUSAGE_RENDERTARGET, colorFormat, 
//			D3DPOOL_DEFAULT, &m_lpPostShadowColor2, NULL);
//		KGLOG_COM_PROCESS_ERROR(hr);
//
//
//		hr = m_lpPostShadowColor1->GetSurfaceLevel(0, &m_lpPostShadowColorSurface1);
//		KGLOG_COM_PROCESS_ERROR(hr);
//
//		hr = m_lpPostShadowColor2->GetSurfaceLevel(0, &m_lpPostShadowColorSurface2);
//		KGLOG_COM_PROCESS_ERROR(hr);
//
//
//
//		KGLOG_PROCESS_ERROR(m_lpPostShadowColorSurface1);
//
//		KGLOG_PROCESS_ERROR(m_lpPostShadowColorSurface2);*/
//
//		D3DXVECTOR4 UV[9];
//		float x = 1.0F/m_dwShadowmapSize * 0.5F;
//		float y = 1.0F/m_dwShadowmapSize * 0.5F;
//
//		UV[0] = D3DXVECTOR4( 0, 0,0,0);
//		UV[1] = D3DXVECTOR4(-x, 0,0,0);
//		UV[2] = D3DXVECTOR4( x, 0,0,0);
//		UV[3] = D3DXVECTOR4( 0,-y,0,0);
//		UV[4] = D3DXVECTOR4( 0, y,0,0);
//		UV[5] = D3DXVECTOR4(-x, y,0,0);
//		UV[6] = D3DXVECTOR4( x, y,0,0);
//		UV[7] = D3DXVECTOR4( x,-y,0,0);
//		UV[8] = D3DXVECTOR4( x,-y,0,0);
//
//		m_lpEffectPostRender->SetVectorArray("g_avSampleOffsets",UV,9);
//
//		m_lpShadowMap = new KG3DSceneShadowMap;
//
//		m_lpShadowMap->Init(m_dwShadowmapSize);
//
//		hr = S_OK;
//	}
//
//Exit0:
//  KG_COM_RELEASE(m_lpPostShadowColorSurface2);
//  KG_COM_RELEASE(m_lpPostShadowColorSurface1);
//  KG_COM_RELEASE(m_lpPostShadowColor2);
//  KG_COM_RELEASE(m_lpSMShadowMapColor);
//  KG_COM_RELEASE(m_lpSMShadowMapColorSurface);
//  KG_COM_RELEASE(m_lpSMShadowMapColor);
//  KG_COM_RELEASE(m_pSMZSurface);
//  KG_COM_RELEASE(m_pSMColorSurface);
//	return hr;
//}
//<<<<<<< KG3DSceneSceneEditor.cpp
////
////HRESULT KG3DSceneSceneEditor::ProcessShadowMap(KG3DCamera* pCamera,D3DXVECTOR4& vLight)
////{
////	if(m_lpSceneOutDoorSpaceMgr)
////	{
////		m_lpSceneOutDoorSpaceMgr->EnableRealTimeShadowMap(TRUE,&m_EntityMgr);
////
////		m_lpSceneOutDoorSpaceMgr->ProcessShadowMap(pCamera,vLight,this);
////	}
////
////	return S_OK;
//////Exit0:
//////	return E_FAIL;
////}
//=======
//
//HRESULT KG3DSceneSceneEditor::ProcessShadowMap(KG3DCamera* pCamera,D3DXVECTOR4& vLight)
//{
//	if(m_lpSceneOutDoorSpaceMgr)
//	{
//		m_lpSceneOutDoorSpaceMgr->EnableRealTimeShadowMap(TRUE,&m_EntityMgr);
//
//		m_lpSceneOutDoorSpaceMgr->ProcessShadowMap(pCamera,vLight,this);
//	}
//
//	return S_OK;
//	//Exit0:
//	//	return E_FAIL;
//}
//>>>>>>> 1.497
//
//HRESULT KG3DSceneSceneEditor::ProcessPostRenderShadow(KG3DSceneOutputWnd* pWnd)
//{
//	HRESULT hr = S_OK;
//	LPDIRECT3DSURFACE9 pDepthSave = NULL;
//	LPDIRECT3DSURFACE9 pTargetSave = NULL;
//	D3DVIEWPORT9 viewPortSave;
//	D3DVIEWPORT9 viewPort;
//
//	hr = g_pd3dDevice->GetDepthStencilSurface(&pDepthSave);
//	hr = pDepthSave->Release();
//
//	hr = g_pd3dDevice->GetRenderTarget(0,&pTargetSave);
//	hr = pTargetSave->Release();
//
//	g_pd3dDevice->GetViewport(&viewPortSave);
//
//	//////////////////////////////////////////////////////////////////////////
//	{
//		hr = g_pd3dDevice->GetDepthStencilSurface(&pDepthSave);
//		hr = pDepthSave->Release();
//
//		hr = g_pd3dDevice->GetRenderTarget(0,&pTargetSave);
//		hr = pTargetSave->Release();
//
//		g_pd3dDevice->GetViewport(&viewPortSave);
//		//////////////////////////////////////////////////////////////////////////
//		viewPort.X = viewPort.Y = 0;
//		viewPort.Height = viewPort.Width = m_dwShadowmapSize;
//		viewPort.MaxZ = 1;viewPort.MinZ =0;
//
//		hr = g_pd3dDevice->SetDepthStencilSurface(m_lpShadowMap->m_ShadowLevel[0].m_lpSMShadowMapSurface);
//		KGLOG_COM_PROCESS_ERROR(hr);
//
//		hr = g_pd3dDevice->SetRenderTarget(0,m_lpSMShadowMapColorSurface);
//		KGLOG_COM_PROCESS_ERROR(hr);
//
//		g_pd3dDevice->SetViewport(&viewPort);
//
//		hr = g_pd3dDevice->BeginScene();
//
//		GraphicsStruct::RenderState cull(D3DRS_CULLMODE,D3DCULL_NONE);
//		GraphicsStruct::RenderState fog(D3DRS_FOGENABLE,FALSE);
//		GraphicsStruct::RenderStateAlpha Alpha(0x0,FALSE,TRUE,D3DBLEND_SRCALPHA,D3DBLEND_INVSRCALPHA);
//
//		m_lpEffectPostRender->SetTexture("tMainDepth",m_lpSMZTexture);
//		m_lpEffectPostRender->SetTexture("tMainColor",m_pSMColorTexture);
//		m_lpEffectPostRender->SetTexture("tShadowmap1",m_lpShadowMap->m_ShadowLevel[0].m_lpSMShadowMap);
//		m_lpEffectPostRender->SetTexture("tShadowmap2",m_lpShadowMap->m_ShadowLevel[1].m_lpSMShadowMap);
//		m_lpEffectPostRender->SetTexture("tShadowmap3",m_lpShadowMap->m_ShadowLevel[2].m_lpSMShadowMap);
//
//		m_lpEffectPostRender->SetMatrix("matLightVP1",&m_lpShadowMap->m_ShadowLevel[0].m_matVP);
//		m_lpEffectPostRender->SetMatrix("matLightVP2",&m_lpShadowMap->m_ShadowLevel[1].m_matVP);
//		m_lpEffectPostRender->SetMatrix("matLightVP3",&m_lpShadowMap->m_ShadowLevel[2].m_matVP);	
//
//		m_lpEffectPostRender->SetTechnique("tec0");
//
//		UINT uPass = 0;
//		m_lpEffectPostRender->Begin(&uPass,0);
//		m_lpEffectPostRender->BeginPass(0);
//
//		g_cGraphicsTool.DrawScreenRectNoChange(&D3DXVECTOR2(0,0),&D3DXVECTOR2(m_dwShadowmapSize*1.0F,m_dwShadowmapSize*1.0F),0,0xFFFFFFFF);
//
//		m_lpEffectPostRender->EndPass();
//		m_lpEffectPostRender->End();
//
//		hr = g_pd3dDevice->EndScene();
//
//		//////////////////////////////////////////////////////////////////////////
//		hr = g_pd3dDevice->SetDepthStencilSurface(pDepthSave);
//		hr = g_pd3dDevice->SetRenderTarget(0,pTargetSave);
//		g_pd3dDevice->SetViewport(&viewPortSave);
//	}
//
//	return S_OK;
//Exit0:
//	return E_FAIL;
//}


//HRESULT KG3DSceneSceneEditor::SmoothPostRenderShadow(KG3DSceneOutputWnd* pWnd)
//{
//	HRESULT hr = S_OK;
//
//
//	if(!m_bUseCSM)
//	{
//		LPDIRECT3DSURFACE9 pDepthSave = NULL;
//		LPDIRECT3DSURFACE9 pTargetSave = NULL;
//		D3DVIEWPORT9 viewPortSave;
//		D3DVIEWPORT9 viewPort;
//		viewPort.X = viewPort.Y = 0;
//		viewPort.Height = viewPort.Width = m_dwShadowmapSize;
//		viewPort.MaxZ = 1;viewPort.MinZ =0;
//
//		hr = g_pd3dDevice->GetDepthStencilSurface(&pDepthSave);
//		hr = pDepthSave->Release();
//
//		hr = g_pd3dDevice->GetRenderTarget(0,&pTargetSave);
//		hr = pTargetSave->Release();
//
//		g_pd3dDevice->GetViewport(&viewPortSave);
//
//		hr = g_pd3dDevice->SetDepthStencilSurface(m_lpSMShadowMapSurface);
//		KGLOG_COM_PROCESS_ERROR(hr);
//
//
//		//////////////////////////////////////////////////////////////////////////
//
//		//hr = g_pd3dDevice->Clear(0, 0, D3DCLEAR_TARGET , 0x0, 1, 0);
//		{
//			GraphicsStruct::RenderState cull(D3DRS_CULLMODE,D3DCULL_NONE);
//
//			GraphicsStruct::RenderState ZCmp_1(D3DRS_ZFUNC,D3DCMP_LESSEQUAL);
//			GraphicsStruct::RenderState ZWrite_1(D3DRS_ZWRITEENABLE,TRUE);
//			GraphicsStruct::RenderState fog(D3DRS_FOGENABLE,FALSE);
//
//			for (int i=0;i<3;i++)
//			{
//				{
//					hr = g_pd3dDevice->SetRenderTarget(0,m_lpPostShadowColorSurface2);
//					KGLOG_COM_PROCESS_ERROR(hr);
//
//					hr = g_pd3dDevice->BeginScene();
//					g_pd3dDevice->SetViewport(&viewPort);
//
//					m_lpEffectPostRender->SetTexture("tMainColor" ,m_lpPostShadowColor1);
//					m_lpEffectPostRender->SetTechnique("tec2");
//
//					UINT uPass = 0;
//					m_lpEffectPostRender->Begin(&uPass,0);
//					m_lpEffectPostRender->BeginPass(0);
//
//					g_cGraphicsTool.DrawScreenRectNoChange(&D3DXVECTOR2(0,0),&D3DXVECTOR2(m_dwShadowmapSize*1.0F,m_dwShadowmapSize*1.0F),0,0xFFFFFFFF);
//
//					m_lpEffectPostRender->EndPass();
//					m_lpEffectPostRender->End();
//
//					hr = g_pd3dDevice->EndScene();
//				}
//				{
//					hr = g_pd3dDevice->SetRenderTarget(0,m_lpPostShadowColorSurface1);
//					KGLOG_COM_PROCESS_ERROR(hr);
//
//					hr = g_pd3dDevice->BeginScene();
//					g_pd3dDevice->SetViewport(&viewPort);
//
//					m_lpEffectPostRender->SetTexture("tMainColor" ,m_lpPostShadowColor2);
//					m_lpEffectPostRender->SetTechnique("tec2");
//
//					UINT uPass = 0;
//					m_lpEffectPostRender->Begin(&uPass,0);
//					m_lpEffectPostRender->BeginPass(0);
//
//					g_cGraphicsTool.DrawScreenRectNoChange(&D3DXVECTOR2(0,0),&D3DXVECTOR2(m_dwShadowmapSize*1.0F,m_dwShadowmapSize*1.0F),0,0xFFFFFFFF);
//
//					m_lpEffectPostRender->EndPass();
//					m_lpEffectPostRender->End();
//
//					hr = g_pd3dDevice->EndScene();
//				}
//
//			}
//		}
//		//////////////////////////////////////////////////////////////////////////
//
//		hr = g_pd3dDevice->SetDepthStencilSurface(pDepthSave);
//		hr = g_pd3dDevice->SetRenderTarget(0,pTargetSave);
//		g_pd3dDevice->SetViewport(&viewPortSave);
//	}
//	else
//	{
//		for (int i=0;i<3;i++)
//		{
//			m_lpShadowMap->m_ShadowLevel[i].SmoothPostRenderShadow(this);
//
//			if(i>1)
//				break;
//		}
//	}
//	
//
//	return S_OK;
//Exit0:
//	return E_FAIL;
//}

HRESULT KG3DSceneSceneEditor::AddTrafficPoint(IEKG3DRepresentObject** ppPoint,D3DXVECTOR3& vecPos,DWORD dwType,DWORD dwSetID)
{
	IKG3DResourceBase *piResoure = NULL;
	KG3DRepresentTrafficPoint* pTrafficPoint = NULL;
	HRESULT hr = E_FAIL;
	HRESULT hRetCode =E_FAIL;
	KG_PROCESS_ERROR(ppPoint);
	hRetCode = g_GetObjectTable().NewOneResourse(REPRESENTOBJECT_TRAFFICPOINT, &piResoure);
	KGLOG_COM_PROCESS_ERROR(hRetCode);

	pTrafficPoint = dynamic_cast<KG3DRepresentTrafficPoint *>(piResoure);
	KGLOG_PROCESS_ERROR(pTrafficPoint);
	pTrafficPoint->SetTranslation(&vecPos);
	pTrafficPoint->SetKind(dwType);
	pTrafficPoint->SetSetID(dwSetID);
	KG3DRepresentObject* pObject = (KG3DRepresentObject*)(pTrafficPoint);


	ULONG uAddOption = EM_SCENE_ADD_OPTION_SELECTABLE;
	hr = AddRepresentObject(pObject, uAddOption, NULL);
	KG_COM_PROCESS_ERROR(hr);

	//this->GetSelectionTool().SetSelectable(Entity, TRUE);//<Selectable Refactor>
	this->IEInsertEntitySelectable(SCENEENTITY_OBJECT, pObject, TRUE);	
	*ppPoint = pObject;
	hr = S_OK;
Exit0:
	return hr;
}
HRESULT KG3DSceneSceneEditor::GetEngineIEInterface( DWORD dwType, DWORD_PTR wParam, DWORD_PTR lParam, VOID** ppiInterface )
{
	LPVOID lpReturn = NULL;
	KG_PROCESS_ERROR(NULL != ppiInterface);
	switch(dwType)
	{
	case KG3DTYPE_SCENE_SHADOW_TOOLS:
		{
			//IEKG3DCommonObject* p = RequestMember(KG3DTYPE_SCENE_SHADOW_TOOLS, NULL, reinterpret_cast<DWORD_PTR>(this), 0);
			IEKG3DCommonObject* p = GetCommonObjectNode().Request(KG3DCOBJ_HANDLE(KG3DTYPE_SCENE_SHADOW_TOOLS, 0)
				, NULL, reinterpret_cast<DWORD_PTR>(this), 0, NULL);
			if (p)
			{
				_ASSERTE(p->GetType() == dwType);
				lpReturn = p->ToAnotherInterface(0);//转换成IE接口
			}
		}
		break;
	case KG3DTYPE_SCENE_OBJECT_PLACEMENT_TOOL:
		{
			//IEKG3DCommonObject* p = this->GetCommonObjectBuilder().Build(dwType, NULL, reinterpret_cast<DWORD_PTR>(this), 0);
			//IEKG3DCommonObject* p = RequestMember(KG3DTYPE_SCENE_OBJECT_PLACEMENT_TOOL, NULL, reinterpret_cast<DWORD_PTR>(this), 0);
			IEKG3DCommonObject* p = GetCommonObjectNode().Request(KG3DCOBJ_HANDLE(KG3DTYPE_SCENE_OBJECT_PLACEMENT_TOOL, 0)
				, NULL, reinterpret_cast<DWORD_PTR>(this), NULL, NULL);
			if (p)
			{
				_ASSERTE(p->GetType() == dwType);
				lpReturn = p->ToAnotherInterface(0);//转换成IE接口
			}
		}
		break;
	default:
		return KG3DScene::GetEngineIEInterface(dwType, wParam, lParam, ppiInterface);
		break;
	}
	if (lpReturn)
	{
		*ppiInterface = lpReturn;
		return S_OK;
	}
Exit0:
	return E_FAIL;
}
HRESULT KG3DSceneSceneEditor::GetEngineIEAbstructInterface( DWORD dwAbstructType, DWORD dwType, DWORD_PTR wParam, DWORD_PTR lParam, VOID** ppiInterface )
{
	LPVOID lpReturn = NULL;
	//HRESULT hr = E_FAIL;
	KG_PROCESS_ERROR(NULL != ppiInterface);
	switch(dwType)
	{
	case KG3DTYPE_SCENE_SHADOW_TOOLS:
		{
			IEKG3DCommonObject* p = GetCommonObjectNode().Request(KG3DCOBJ_HANDLE(KG3DTYPE_SCENE_SHADOW_TOOLS, 0)
				, NULL, reinterpret_cast<DWORD_PTR>(this), 0, NULL);		
			lpReturn = p;
		}
	case KG3DTYPE_SCENE_OBJECT_PLACEMENT_TOOL:
		{
			IEKG3DCommonObject* p = GetCommonObjectNode().Request(KG3DCOBJ_HANDLE(KG3DTYPE_SCENE_OBJECT_PLACEMENT_TOOL, 0)
				, NULL, reinterpret_cast<DWORD_PTR>(this), 0, NULL);
			lpReturn = p;
		}
		break;
	default:
		return KG3DScene::GetEngineIEAbstructInterface(dwAbstructType, dwType, wParam, lParam, ppiInterface);
		break;
	}
	if (lpReturn)
	{
		*ppiInterface = lpReturn;
		return S_OK;
	}
Exit0:
	return E_FAIL;
}


//用于保存一个静态的值，用于加速查找出物件
struct MemberGetterBooster
{
	INT m_iPos;
	MemberGetterBooster(DWORD TypeList[], INT Count, DWORD dwType)
	{
		INT iPos = 0;
		for (; iPos < Count; ++iPos)
		{
			if (TypeList[iPos] == dwType)
			{
				break;
			}
		}
		_ASSERTE(iPos < Count);
		m_iPos = iPos;

	}
};


VOID KG3DSceneSceneEditor::ForceFireEvent2( INT nSinkID,INT nsubSinkID,DWORD dwValue,PVOID pPoint )
{
	this->FireEvent2(nSinkID, nsubSinkID, dwValue, pPoint);
}

KG3DSceneSceneEditor* KG3DSceneSceneEditor::GetSceneSceneEditor()
{
	return this;
}


HRESULT KG3DSceneSceneEditor::SetSelected( const IEKG3DSceneEntity& Entity, BOOL bSelected )
{
	return KG3DSceneSelectBase::SetSelected(Entity, bSelected);
}

HRESULT KG3DSceneSceneEditor::SetSelected( SCENEENTITYTYPE dwType, LPVOID Pointer, BOOL bSelected )
{
	HRESULT hRetCode = E_FAIL;
	HRESULT hResult = E_FAIL;
	KG3DSceneEntity SelectedEntity(dwType, Pointer);
	hRetCode = SetSelected(SelectedEntity, bSelected);
	KG_COM_PROCESS_ERROR(hRetCode);
	//bSelected ? SelectedEntity.OnSelect() : SelectedEntity.OnUnSelect();//2009-3-26改在SelectBase里面做
	hResult = S_OK;
Exit0:
	return hResult;
}

HRESULT KG3DSceneSceneEditor::GetTBModifierTextureStride(int* pX,int* pY,DWORD dwType)
{
	return g_cTerrainBlockModifier.GetTBModifierTextureStride(pX,pY,dwType,m_lpSceneOutDoorSpaceMgr);

}
HRESULT KG3DSceneSceneEditor::SetTBModifierTextureStride(int  nX,int  nY,DWORD dwType)
{
	return g_cTerrainBlockModifier.SetTBModifierTextureStride(nX,nY,dwType,m_lpSceneOutDoorSpaceMgr);
}

HRESULT KG3DSceneSceneEditor::GetTBModifierWidthMax(FLOAT* pValue)
{
	return g_cTerrainBlockModifier.GetWidthMax(pValue);
}
HRESULT KG3DSceneSceneEditor::GetTBModifierWidthMin(FLOAT* pValue)
{
	return g_cTerrainBlockModifier.GetWidthMin(pValue);
}
HRESULT KG3DSceneSceneEditor::SetTBModifierWidthMax(FLOAT fValue)
{
	return g_cTerrainBlockModifier.SetWidthMax(fValue);
}
HRESULT KG3DSceneSceneEditor::SetTBModifierWidthMin(FLOAT fValue)
{
	return g_cTerrainBlockModifier.SetWidthMin(fValue);
}
HRESULT KG3DSceneSceneEditor::GetTBModifierHeightMax(FLOAT* pValue)
{
	return g_cTerrainBlockModifier.GetHeightMax(pValue);
}
HRESULT KG3DSceneSceneEditor::GetTBModifierHeightMin(FLOAT* pValue)
{
	return g_cTerrainBlockModifier.GetHeightMin(pValue);
}
HRESULT KG3DSceneSceneEditor::SetTBModifierHeightMax(FLOAT fValue)
{
	return g_cTerrainBlockModifier.SetHeightMax(fValue);
}
HRESULT KG3DSceneSceneEditor::SetTBModifierHeightMin(FLOAT fValue)
{
	return g_cTerrainBlockModifier.SetHeightMin(fValue);
}
HRESULT KG3DSceneSceneEditor::SetTBModifierNumPerM2(FLOAT fValue)
{
	return g_cTerrainBlockModifier.SetNumPerM2(fValue);
}
DWORD KG3DSceneSceneEditor::GetTBModifierState()
{
	return	g_cTerrainBlockModifier.GetTerrainState();
}
HRESULT KG3DSceneSceneEditor::SetTBModifierState(DWORD dwState)
{
	return g_cTerrainBlockModifier.SetTerrainState(dwState);
}
HRESULT KG3DSceneSceneEditor::UpdatePatternList(INT* pList,INT nCount)
{
	return g_cTerrainBlockModifier.UpdatePatternList(pList,nCount);
}
HRESULT KG3DSceneSceneEditor::SetTBModifierTextureIndexInfo(INT* pPointer, INT* pNumIndex)
{
	return g_cTerrainBlockModifier.SetTextureIndexInfo(pPointer,pNumIndex);
}
HRESULT KG3DSceneSceneEditor::SetTBModifierTextureIndexMax(INT nMax)
{
	return g_cTerrainBlockModifier.SetTextureIndexMax(nMax);
}

HRESULT KG3DSceneSceneEditor::SetTBModifierTextureIndexMin(INT nMin)
{
	return g_cTerrainBlockModifier.SetTextureIndexMin(nMin);
}
HRESULT KG3DSceneSceneEditor::GetTerrainModifyHeight(float& fHeight)
{
	fHeight = g_cTerrainBlockModifier.GetModifyHeight();
	return S_OK;
}
HRESULT KG3DSceneSceneEditor::SetTerrainModifyHeight(float fHeight)
{
	return g_cTerrainBlockModifier.SetModifyHeight(fHeight);
}
HRESULT KG3DSceneSceneEditor::SetOppositeHeightValue(FLOAT fOppValue)
{
	return g_cTerrainBlockModifier.SetOppositeHeightValue(fOppValue);
}
HRESULT KG3DSceneSceneEditor::SetIsGetAbsoluteHeight(BOOL bIsAbs)
{
	return g_cTerrainBlockModifier.SetIsGetAbsoluteHeight(bIsAbs);
}
float KG3DSceneSceneEditor::GetTerrainModifyAlpha()
{
	return g_cTerrainBlockModifier.GetTerrainModifyAlpha();
}
HRESULT KG3DSceneSceneEditor::SetTerrainModifyAlpha(FLOAT fValue)
{
	g_cTerrainBlockModifier.SetTerrainModifyAlpha(fValue);
	return S_OK;
}
DWORD KG3DSceneSceneEditor::GetInnerHeightBrushSize()
{
	return g_cTerrainBlockModifier.GetInnerHeightBrushSize();
}
DWORD KG3DSceneSceneEditor::GetOuterHeightBrushSize()
{
	return g_cTerrainBlockModifier.GetOuterHeightBrushSize();
}
HRESULT KG3DSceneSceneEditor::SetInnerHeightBrushSize(DWORD dwSize)
{
	return g_cTerrainBlockModifier.SetInnerHeightBrushSize(dwSize);
}
HRESULT KG3DSceneSceneEditor::SetOuterHeightBrushSize(DWORD dwSize)
{
	return g_cTerrainBlockModifier.SetOuterHeightBrushSize(dwSize);
}
HRESULT KG3DSceneSceneEditor::SetDiShu(FLOAT fPower)
{
	return g_cTerrainBlockModifier.SetDiShu(fPower);
}
HRESULT KG3DSceneSceneEditor::ReSizeHeightData(const WORD BrushSize)
{
	return g_cTerrainBlockModifier.ReSizeHeightData(BrushSize);
}
HRESULT KG3DSceneSceneEditor::SetPaintTexIndex(BYTE byIndex)
{
	return g_cTerrainBlockModifier.SetPaintTexIndex(byIndex);
}

BYTE KG3DSceneSceneEditor::GetPaintTexIndex()
{
	return g_cTerrainBlockModifier.GetPaintTexIndex();
}
HRESULT KG3DSceneSceneEditor::SetCurrentBrushMap(LPCTSTR pFileName)
{
	return g_cTerrainBlockModifier.SetCurrentBrushMap(pFileName);
}
int KG3DSceneSceneEditor::GetTBModifierGrassDensity()
{
	return g_cTerrainBlockModifier.GetGrassDensity();
}
HRESULT KG3DSceneSceneEditor::SetTBModifierGrassDensity(int nDensity)
{
	return g_cTerrainBlockModifier.SetGrassDensity(nDensity);
}
HRESULT KG3DSceneSceneEditor::BuildAutoGrass(BYTE byDetailTexIndex)
{
	return g_cTerrainBlockModifier.BuildAutoGrass(m_lpSceneOutDoorSpaceMgr,byDetailTexIndex);
}
HRESULT KG3DSceneSceneEditor::DeleteAutoGrass(BYTE byDetailTexIndex)
{
	return g_cTerrainBlockModifier.DeleteAutoGrass(m_lpSceneOutDoorSpaceMgr,byDetailTexIndex);
}
HRESULT KG3DSceneSceneEditor::SetAddAutoGrass(BOOL bVal)
{
	g_cTerrainBlockModifier.m_bAddAutoGrass = bVal;
	return S_OK;
}
HRESULT KG3DSceneSceneEditor::ClearAllTerrainObject()
{
	return g_cTerrainBlockModifier.ClearAllTerrainObject(m_lpSceneOutDoorSpaceMgr);
}
HRESULT KG3DSceneSceneEditor::SetTerrainWaterDefaultHeight(float fHeight)
{
	return g_cTerrainBlockModifier.SetTerrainWaterDefaultHeight(m_lpSceneOutDoorSpaceMgr,fHeight);
}
IEKG3DSceneOutDoorSpaceMgr* KG3DSceneSceneEditor::GetIEKG3DSceneOutDoorSpaceMgr()
{
	return (IEKG3DSceneOutDoorSpaceMgr*)m_lpSceneOutDoorSpaceMgr;
}

HRESULT KG3DSceneSceneEditor::RemoveRenderEntityAndNotifyLogic( SCENEENTITYTYPE type,PVOID pPointer )
{
	return RemoveRenderEntityAndNotifyLogic(KG3DSceneEntity(type, pPointer), TRUE);
}

HRESULT KG3DSceneSceneEditor::RemoveRenderEntityAndNotifyLogic( KG3DSceneEntity Entity, BOOL bEitherRemoveInSelectedList /*= TRUE*/ )
{
	HRESULT hr = E_FAIL;
	//HRESULT hr = RemoveRenderEntity(Entity, bEitherRemoveInSelectedList);
	//KG_COM_PROCESS_ERROR(hr);

	if (Entity.GetType() == SCENEENTITY_OBJECT)
	{
		KG3DRepresentObject* pObject = reinterpret_cast<KG3DRepresentObject*>(Entity.m_pPointer);
		if(pObject->GetType()==REPRESENTOBJECT_NPC)
		{			 
			FireEvent2(EVENTEDITORNPCDELETED,0,0,pObject);
		}
		else if(pObject->GetType()==REPRESENTOBJECT_DOODAD)
		{
			FireEvent2(EVENTEDITORDOODADDELETED,0,0,pObject);
		}
		else if(pObject->GetType()==REPRESENTOBJECT_WAYPOINT)
		{
			FireEvent2(EVENTEDITORWAYPOINTDELETED,0,0,pObject);
		}
		else if(pObject->GetType()==REPRESENTOBJECT_TRAFFICPOINT)
		{
			FireEvent2(EVENTEDITORTRAFFICPOINTDELETED,0,0,pObject);
		}
		else if(pObject->GetType()==REPRESENTOBJECT_REFRESHPOINT)
		{
			FireEvent2(EVENTEDITORREFRESHPOINTDELETED,0,0,pObject);
		}
		else if(pObject->GetType()==REPRESENTOBJECT_POLY)
		{
			KG3DRepresentPoly* pPoly = static_cast<KG3DRepresentPoly*>(pObject);
			IEKG3DRepresentPoly* pIEPoly = pPoly;
			FireEvent2(EVENTEDITORPOLYDELETED,0,0,pIEPoly);
		}
	}
//Exit0:
	return hr;
}

HRESULT KG3DSceneSceneEditor::GSFFitGround()
{
	HRESULT hr = E_FAIL;
	if (m_lpSceneOutDoorSpaceMgr)
		hr = g_cTerrainBlockModifier.GSFFitGround(m_lpSceneOutDoorSpaceMgr);
	return hr;
}

//HRESULT KG3DSceneSceneEditor::SelectBegin(D3DXVECTOR3 Origin,D3DXVECTOR3 Direction,const D3DXVECTOR2 Origin2DLeftUp, const D3DXVECTOR2 Origin2DRightBottom, DWORD dwOperation)
//{
//	return KG3DSceneSelectBase::SelectBegin(Origin,Direction,Origin2DLeftUp,Origin2DRightBottom,dwOperation);
//}

//HRESULT KG3DSceneSceneEditor::SelectBegin(D3DXVECTOR3 Origin,D3DXVECTOR3 Direction,D3DXVECTOR2 Origin2D, DWORD dwOperation)
//{
//	return KG3DSceneSelectBase::SelectBegin(Origin,Direction,Origin2D,dwOperation);
//}

HRESULT KG3DSceneSceneEditor::GetGroundHeight(float& fHeight,float x,float y)
{
	fHeight = m_lpSceneOutDoorSpaceMgr->GetGroundHeight(x,y);
	return S_OK;
}

void KG3DSceneSceneEditor::FrameMoveCamera()
{
	if ((!m_bEditing) && (m_pRepresentNPC) && (m_SceneWndManager->GetCurOutputWnd()) && (!g_bClient))
	{
		D3DXVECTOR3 vecNpcPos;
		D3DXVECTOR3 vecNpcDir;
		D3DXVECTOR3 vecCamPos;
		D3DXVECTOR3 vecDirCNp;
		D3DXVECTOR3 Position;

		static bool bInit = false;
		if (!bInit)
		{
			m_pRepresentNPC->SetPhysicsScene(m_lpPhysicsScene);
			bInit = true;
		}
		//m_pRepresentNPC->NPCFrameMove();
		_ASSERTE(NULL != m_pRepresentNPC);
		KSF::FrameMoveSceneRunner(*m_pRepresentNPC, *this);

		m_pRepresentNPC->SaveTheFloorHeight(*this, m_pLogicalSceneConnection);

		m_pRepresentNPC->GetTranslation(&Position);

		m_pRepresentNPC->GetCurPosition(&vecNpcPos);
		m_pRepresentNPC->GetDirection(&vecNpcDir);
		vecNpcPos = vecNpcPos + D3DXVECTOR3(0.0f, 180.0f, 0.0f);

		{
			KG3DTrackCamera* pTrackCamera = m_SceneWndManager->GetCurOutputWnd()->GetCamera().GetTrackCameraInterface();
			if (NULL != pTrackCamera)
			{
				pTrackCamera->UpdateAsClient(
					vecNpcPos, vecNpcDir, m_pRepresentNPC->m_fMoveSpeed, 
					m_pRepresentNPC->m_dwMovementType
					);
			}
		}

		m_SceneWndManager->GetCurOutputWnd()->GetCamera().GetPosition(&vecCamPos);
		vecDirCNp = vecCamPos - vecNpcPos;

		g_fCameraNpcDistance =  D3DXVec3Length(&vecDirCNp);
		if (g_fCameraNpcDistance < 200.0f)
			m_pRepresentNPC->SetAlpha(g_fCameraNpcDistance / 200);
		else
			m_pRepresentNPC->SetAlpha(1.0f);
	}

	if (m_bEditing && m_SceneWndManager->GetCurOutputWnd())
	{
		TrackCamareFrameMove();
	}

	if ((!m_bEditing || g_bClient) && m_SceneWndManager->GetCurOutputWnd())
	{
		m_SceneWndManager->GetCurOutputWnd()->GetCamera().BindScene(this);
		m_SceneWndManager->GetCurOutputWnd()->GetCamera().FrameMove();
	}

	// 策划场景编辑器
	if (g_bDesign)
	{
		FrameMoveCameraDesign();
	}
}

HRESULT KG3DSceneSceneEditor::FrameMoveCameraDesign()
{
	KG3DSceneOutputWnd* pOutputWnd = m_SceneWndManager->GetCurOutputWnd();
	if (!pOutputWnd)
		return S_OK;
	KG3DCamera& camera = pOutputWnd->GetCamera();
	D3DXVECTOR3 Pos,LookAt;
	camera.GetPosition(&Pos);
	camera.GetLookAtPosition(&LookAt);	

	D3DXVECTOR3 Front = camera.GetCameraFront();
	D3DXVECTOR3 right = camera.GetCameraRight();

	if(m_ForceSetCameraData.valid) // 强制设置镜头位置
	{
		Pos = m_ForceSetCameraData.position;
		LookAt = m_ForceSetCameraData.direction;
		m_ForceSetCameraData.valid = FALSE;
	}
	// end ahpho

	camera.SetPositionLookAtUp(&Pos, &LookAt, NULL);
	return S_OK;
}

HRESULT KG3DSceneSceneEditor::GetVisibleModelFromOutDoor(
    std::set<KG3DModel *> &rsetModels,
    const std::vector<KG3DFrustum> &crvecFrustum
)
{
    HRESULT hrResult = E_FAIL;
    KG3DSceneEntityList::iterator itEntify;
    KG3DSceneEntity *pEntity = NULL;
    KG3DModel *pModel = NULL;
    KG3DSceneEntityList listEntityRenderLast = m_listEntityRenderLast;
    KG3DFrustum::INTERSECT_TYPE eIntersectType = KG3DFrustum::OUTSIDE;
    KG3DSceneRegion *pRegion = NULL;
    int nXStart = 0;
    int nZStart = 0;
    int nXEnd = 0;
    int nZEnd = 0;

    for (size_t k = 0; k < crvecFrustum.size(); ++k)
    {
        const KG3DFrustum *pcFrustum = &(crvecFrustum[k]);
        for (int i = 0;i < 8;i++)
        {
            if(k == 0 && i == 0)
            {
                nXStart = nXEnd = (int)(pcFrustum->GetPoint(i).x / EM_SCENE_REGION_WIDTH);
                nZStart = nZEnd = (int)(pcFrustum->GetPoint(i).z / EM_SCENE_REGION_WIDTH);
            }
            else
            {
                nXStart = min(int(pcFrustum->GetPoint(i).x / EM_SCENE_REGION_WIDTH), nXStart);
                nZStart = min(int(pcFrustum->GetPoint(i).z / EM_SCENE_REGION_WIDTH), nZStart);
                nXEnd = max(int(pcFrustum->GetPoint(i).x / EM_SCENE_REGION_WIDTH), nXEnd);
                nZEnd = max(int(pcFrustum->GetPoint(i).z / EM_SCENE_REGION_WIDTH), nZEnd);
            }
        }
    }
    // +-1 是对某些大小超过一个Region的大模型做视椎边界修正
    nXStart -= 1;
    nXEnd += 1;
    nZStart -= 1;
    nZEnd += 1;

    for (itEntify = listEntityRenderLast.begin() ;itEntify != listEntityRenderLast.end(); ++ itEntify)
    {
        pEntity = &(*itEntify);
        if (pEntity->GetType() != SCENEENTITY_MODEL)
            continue;
    
        pModel = static_cast<KG3DModel*>(pEntity->m_pPointer);
        if (pModel->m_pPvsOn) // 在室内
            continue;

        pRegion = (KG3DSceneRegion *)pModel->m_lpSceneRegionOn;

        if (!pRegion)
            continue;

        if (
            (pRegion->m_dwVisibleCount == g_dwRenderCount) &&
            (pRegion->m_Data.m_eFrustumIntersectType == KG3DFrustum::INSIDE)
        )
        {
            rsetModels.insert(pModel);
            continue;
        }

        if (
            (pRegion->m_nIndexX < nXStart) || (pRegion->m_nIndexX > nXEnd) ||
            (pRegion->m_nIndexZ < nZStart) || (pRegion->m_nIndexZ > nZEnd)
        )
        {
            continue;
        }

        for (size_t i = 0; i < crvecFrustum.size(); ++i)
        {
            eIntersectType = (KG3DFrustum::INTERSECT_TYPE)crvecFrustum[i].Intersect(pModel->m_BBox);
            if (eIntersectType != KG3DFrustum::OUTSIDE)
            {
                rsetModels.insert(pModel);
                break;
            }
        }
    }

    hrResult = S_OK;
//Exit0:
    return hrResult;
}

HRESULT KG3DSceneSceneEditor::_FrameMoveRenderEntifyLast()
{
	DWORD dwStartTime = timeGetTime();

	HRESULT hrResult = E_FAIL;
	KG3DSceneEntityList::iterator itEntify;
    static DWORD s_dwComputeDynamicBBoxIndex = 0;
    DWORD dwEntityIndex = 0;
	KG3DSceneEntityList listEntityRenderLast = m_listEntityRenderLast;

	if (s_dwComputeDynamicBBoxIndex > m_listEntityRenderLast.size())
		s_dwComputeDynamicBBoxIndex = 0;
	
	// for all client add to m_listEntityRenderLast
	for (itEntify = listEntityRenderLast.begin() ;itEntify != listEntityRenderLast.end(); ++ itEntify )
	{
		KG3DSceneEntity *pEntity = &(*itEntify);
		
		if(pEntity->GetType() == SCENEENTITY_MODEL)
		{
			KG3DModel* pModel = static_cast<KG3DModel*>(pEntity->m_pPointer);

			D3DXVECTOR3 V1 = pModel->m_Translation_Last - pModel->m_Translation;
			float K = D3DXVec3LengthSq(&V1);

			if((pModel->GetID()%20 == g_dwRenderCount%20) || K >= 10000)
			{
				m_lpSceneOutDoorSpaceMgr->InputModel(pModel);
				pModel->m_Translation_Last = pModel->m_Translation;
			}
			
			if (K >= 100 && m_lpWaterEffect && pModel->m_bVisible && (pModel->GetType() == MESHTYPE_DEFAULT || pModel->GetType() == MESHTYPE_MODELST))
			{
				D3DXVECTOR3 vEntifyPos;
				pEntity->GetTranslation(&vEntifyPos);
				m_lpWaterEffect->CheckEntityAndWater(pModel,vEntifyPos,300,this);
			}

            if (pModel->GetType() == MESHTYPE_MODELST)
            {
                if (s_dwComputeDynamicBBoxIndex == dwEntityIndex)
				{
					pModel->ComputeDynamicBBox();
				}

            }
		}
		pEntity->FrameMove(); // must at last, it will set pModel->m_bVisible to false
		++dwEntityIndex;
	}
  	++s_dwComputeDynamicBBoxIndex;

	if (m_lpWaterEffect)
	{
		m_lpWaterEffect->FrameMove();
	}

    hrResult = S_OK;

	DWORD dwCost = timeGetTime() - dwStartTime;
	if(g_cEngineOption.bEnableTimeOptimizeLog && dwCost >= 20)
	{
		KGLogPrintf(KGLOG_ERR, "TimeOptimize %d  KG3DSceneSceneEditor::_FrameMoveRenderEntifyLast.",dwCost);
	}

	return hrResult;
}

HRESULT KG3DSceneSceneEditor::GetDefaultWallDefine(IEKG3DWallDefine** ppWallDefine)
{
	HRESULT hr = E_FAIL;

	KG3DWallDefine* pNewDefine = NULL;

	hr = g_cWallDefineTable.LoadWallDefineFromFile("NewWallDefine.wall",&pNewDefine);
	KGLOG_COM_PROCESS_ERROR(hr);

	(*ppWallDefine) = pNewDefine;

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneSceneEditor::SetWallConnectionPoint(BOOL bShow,D3DXVECTOR3 vA,D3DXVECTOR3 vB)
{
	m_bShowWallConnectionPoint = bShow;
	m_vWallConnectionPointA = vA;
	m_vWallConnectionPointB = vB;

	D3DXVECTOR3 V1 = vA - D3DXVECTOR3(0,200,0);
	D3DXVECTOR3 V2 = vB - D3DXVECTOR3(0,200,0);

	m_vWallMeasureTool.CreateTwoPointLine(V1,V2);

	return S_OK;
}

IEKG3DWall* KG3DSceneSceneEditor::GetFirstSelectedObjectWall()
{
	IEKG3DRepresentObject* pIEObject = GetFirstSelectedObject();
	KGLOG_PROCESS_ERROR(pIEObject);

	KG3DRepresentObject* pObject = static_cast<KG3DRepresentObject*>(pIEObject);

	if (pObject->GetType() == REPRESENTOBJECT_WALL)
	{
		KG3DRepresentObjectWall* pWall = static_cast<KG3DRepresentObjectWall*>(pObject);
		return pWall;
	}
Exit0:
	return NULL;
}

HRESULT KG3DSceneSceneEditor::AddWallNodeToScene(IEKG3DWall* pWall,int nWall,int nKeyPoint)
{
	HRESULT hr = E_FAIL;
	float nSize = 10 ;
	KG3DRepresentObjectNode* pObjectNode = NULL;
	KGLOG_PROCESS_ERROR(m_lpSceneOutDoorSpaceMgr);

	//SAFE_RELEASE(m_lpHandleObject);//新的放置工具不能直接操作HandleObject

	{
		KG3DRepresentObject* pObject = NULL;

		hr = g_GetObjectTable().NewOneResourse(REPRESENTOBJECT_NODE,(IKG3DResourceBase**)&pObject);
		KGLOG_COM_PROCESS_ERROR(hr);

		pObjectNode = static_cast<KG3DRepresentObjectNode*>(pObject);
	}
	pObjectNode->SetNodeType(NODETYPE_WALLKEYPOINT);

	{
		KG3DRepresentObjectWall* pWallObject = static_cast<KG3DRepresentObjectWall*>(pWall);
		pObjectNode->SetWall(pWallObject,nWall,nKeyPoint);
	}



	pObjectNode->SetScaling(&D3DXVECTOR3(nSize,nSize,nSize));

	//新的放置工具改成这样操作
	hr = SetObjectInHandle(pObjectNode, TRUE);
	KG_COM_PROCESS_ERROR(hr);
	return S_OK;
Exit0:
	SAFE_RELEASE(pObjectNode);
	return E_FAIL;
	return S_OK;
}
HRESULT KG3DSceneSceneEditor::GetLODProperties( FLOAT* pfLODHightToMiddleSwitchAngleGate, FLOAT* pfLODMiddleToLowSwitchAngleGate )
{
	if (NULL != pfLODHightToMiddleSwitchAngleGate)
	{
		*pfLODHightToMiddleSwitchAngleGate = m_EntityMgr.GetCullData(0).m_fVisibleAngleGate;
	}
	if (NULL != pfLODMiddleToLowSwitchAngleGate)
	{
		*pfLODMiddleToLowSwitchAngleGate = m_EntityMgr.GetCullData(0).m_fLODMiddleToLowSwitchAngleGate;
	}
	return S_OK;
}

HRESULT KG3DSceneSceneEditor::SetLODProperties( FLOAT fLODHightToMiddleSwitchAngleGate, FLOAT fLODMiddleToLowSwitchAngleGate )
{
	if (fLODHightToMiddleSwitchAngleGate < fLODMiddleToLowSwitchAngleGate 
		&& fLODHightToMiddleSwitchAngleGate > 0)
	{
		m_EntityMgr.GetCullData(0).m_fVisibleAngleGate = fLODHightToMiddleSwitchAngleGate;
		m_EntityMgr.GetCullData(0).m_fLODMiddleToLowSwitchAngleGate = fLODMiddleToLowSwitchAngleGate;
		return S_OK;
	}
	return E_FAIL;
}

HRESULT KG3DSceneSceneEditor::ExprotTerrainHeightMap(LPCSTR pFileName,DWORD dwOption)
{
    return m_lpSceneOutDoorSpaceMgr->ExprotTerrainHeightMap(pFileName,dwOption);
}

HRESULT KG3DSceneSceneEditor::ImportTerrainHeightMap(LPCSTR pFileName,DWORD dwOption)
{
    return m_lpSceneOutDoorSpaceMgr->ImportTerrainHeightMap(pFileName,dwOption);

}

HRESULT KG3DSceneSceneEditor::GetSceneHeightInfo(int* pLowest,int* pHeightTotal)
{
    return m_lpSceneOutDoorSpaceMgr->GetSceneHeightInfo(pLowest,pHeightTotal);
}

HRESULT KG3DSceneSceneEditor::SetSceneHeightInfo(int  nLowest,int  nHeightTotal)
{
    return m_lpSceneOutDoorSpaceMgr->SetSceneHeightInfo(nLowest,nHeightTotal);
}

HRESULT KG3DSceneSceneEditor::RefreshWaterEffect()
{
	if(m_lpWaterEffect)
		m_lpWaterEffect->ReloadEffect();

	return S_OK;
}

HRESULT KG3DSceneSceneEditor::EnableShowLogicalSize(BOOL bEnable)
{
	m_bEnableShowLogicalSize = bEnable;
	return S_OK;
}
HRESULT KG3DSceneSceneEditor::EnableShowAppointLogicalRegion(BOOL bEnable)
{
	m_bEnableShowAppointLogicalRegion = bEnable;
	return S_OK;
}
HRESULT KG3DSceneSceneEditor::SetEnableWireFrameMode(BOOL bEnable)
{
	m_bEnableWireFrameMode = bEnable;
	return S_OK;
}

DWORD KG3DSceneSceneEditor::GetEntityNum()
{
	return g_dwEntityNum;
}

HRESULT KG3DSceneSceneEditor::ReloadTerrainRenderShader()
{
	HRESULT hr = E_FAIL;
	if(m_EntityMgr.m_lpTerrainRender)
		hr = m_EntityMgr.m_lpTerrainRender->ReloadShaders();
	return hr;
}

HRESULT KG3DSceneSceneEditor::SetRenderBlockEdge(BOOL bVal)
{
	g_bRenderBlockEdge = bVal;
	return S_OK;
}
HRESULT KG3DSceneSceneEditor::SetRenderWaterEdge(BOOL bVal)
{
	g_bRenderWaterEdge = bVal;
	return S_OK;
}

HRESULT KG3DSceneSceneEditor::RenderWaterAndSFX()
{
	m_EntityMgr.RenderWaterAndSFX(this);

	if (g_cEngineManager.GetRenderOption(RENDER_WATER) && m_lpWaterEffect)
	{
		GraphicsStruct::RenderState Zenable(D3DRS_ZENABLE,D3DZB_TRUE);
		GraphicsStruct::RenderState ZWrite(D3DRS_ZWRITEENABLE,FALSE);
		GraphicsStruct::RenderState ZFun(D3DRS_ZFUNC,D3DCMP_LESS);
		GraphicsStruct::RenderState colorWrite(D3DRS_COLORWRITEENABLE ,D3DCOLORWRITEENABLE_ALPHA|D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_GREEN);

        if (g_cEngineOption.nWaterDetail >= WD_HIGH)
			m_lpWaterEffect->RenderWaveSFX(FALSE, TRUE);
        else
		    m_lpWaterEffect->RenderWaveSFX(TRUE, TRUE);
	}

	return S_OK;
}

HRESULT KG3DSceneSceneEditor::ComputeAllEntityBBox()
{
	HRESULT hr = E_FAIL;
	if(m_lpSceneOutDoorSpaceMgr)
	{
		ForceLoadAllEntity();
		m_lpSceneOutDoorSpaceMgr->ComputeAllEntityBBox();
	}
	return hr;
}
HRESULT KG3DSceneSceneEditor::RefreshTerrain()
{
	return g_cTerrainBlockModifier.RefreshTerrain(m_lpSceneOutDoorSpaceMgr);
}
HRESULT KG3DSceneSceneEditor::ComputeTerrainNormal()
{
	return g_cTerrainBlockModifier.ComputeTerrainNormal(m_lpSceneOutDoorSpaceMgr);
}
HRESULT KG3DSceneSceneEditor::CompositorTerrainTexture()
{
	return g_cTerrainBlockModifier.CompositorTerrainTexture(m_lpSceneOutDoorSpaceMgr);
}
HRESULT KG3DSceneSceneEditor::RemoveSelectableModel(IKG3DResourceBase *pEntity)
{
	/*return KG3DSceneSelectBase::RemoveSelectableEntity(SCENEENTITY_MODEL, 
		pEntity, 
		TRUE);*/

	KG3DSceneEntity entity(SCENEENTITY_MODEL, pEntity);
	this->GetSelectionTool().SetSelected(entity, FALSE);
	//return this->GetSelectionTool().SetSelectable(entity, FALSE);//<Selectable Refactor>
	return this->IEInsertEntitySelectable(SCENEENTITY_MODEL, pEntity, FALSE);
}
HRESULT KG3DSceneSceneEditor::ClearTextureFromTerrain(BYTE byTexIndex)
{
	HRESULT hr = g_cTerrainBlockModifier.ClearTexture(m_lpSceneOutDoorSpaceMgr,byTexIndex);
	if(m_EntityMgr.m_lpTerrainRender)
		m_EntityMgr.m_lpTerrainRender->EnableSortTerrainBlock();
	return hr;
}
HRESULT KG3DSceneSceneEditor::PaintTerrainTextureFromFilterMap(LPCSTR pFileName)
{
	HRESULT hr = g_cTerrainBlockModifier.PaintTextureFromFilterMap(m_lpSceneOutDoorSpaceMgr,pFileName);
	if(m_EntityMgr.m_lpTerrainRender)
		m_EntityMgr.m_lpTerrainRender->EnableSortTerrainBlock();
	return hr;
}

HRESULT KG3DSceneSceneEditor::ExprotFilterMap(LPCSTR pFileName)
{
	return g_cTerrainBlockModifier.ExportTexture(m_lpSceneOutDoorSpaceMgr,pFileName,EXPORT_FilterMap);
}

HRESULT KG3DSceneSceneEditor::ExprotNormalMap(LPCSTR pFileName)
{
	return g_cTerrainBlockModifier.ExportTexture(m_lpSceneOutDoorSpaceMgr,pFileName,EXPORT_NormalMap);
}
// 策划场景编辑器
HRESULT KG3DSceneSceneEditor::SetCameraSpeed(FLOAT fPercent)
{
	//if (m_bEditing)
	//{
		g_eEngineOption.nFlySpeedX = (INT)(20.0f + fPercent * (300.0f - 20.0f));
		g_eEngineOption.nFlySpeedY = (INT)(20.0f + fPercent * (300.0f - 20.0f));
	//}
	//else
	//{
		m_pRepresentNPC->m_fMoveSpeed = 100.0f + fPercent * (10000.0f - 100.0f);
	//}
	return S_OK;
}
HRESULT KG3DSceneSceneEditor::GetCameraPosLookat(AtlVector3* pos, AtlVector3* lookat)
{
	KG3DCamera& camera = m_SceneWndManager->GetCurOutputWnd()->GetCamera();
	D3DXVECTOR3 POS;
	D3DXVECTOR3 LOOKAT;
	camera.GetPosition(&POS);
	camera.GetLookAtPosition(&LOOKAT);
	pos->x = POS.x; pos->y = POS.y; pos->z = POS.z;
	lookat->x = LOOKAT.x; lookat->y = LOOKAT.y; lookat->z = LOOKAT.z;
	return S_OK;
}
HRESULT KG3DSceneSceneEditor::SetCameraPosLookat(AtlVector3* pos, AtlVector3* lookat)
{
	KG3DCamera& camera = m_SceneWndManager->GetCurOutputWnd()->GetCamera();
	D3DXVECTOR3 POS(pos->x, pos->y, pos->z);
	D3DXVECTOR3 LOOKAT(lookat->x, lookat->y, lookat->z);
	camera.SetPositionLookAtUp(&POS, &LOOKAT, NULL);

	return S_OK;
}

HRESULT KG3DSceneSceneEditor::NpcPlayNextAction()
{
	HRESULT hRetCode = E_FAIL;
	SCENEENTITYTYPE EntityType = SCENEENTITY_NONE;
   	IKG3DModel* model;
	INT selectedEntityCount = 0;
	INT modelCount = 0;

	KG3DSceneSelectionTool& selTool = this->GetSelectionTool();
	selectedEntityCount = selTool.GetSelectionCount();

	//GetSelectedEntityCount(&selectedEntityCount);
	if(selectedEntityCount > 0)
	{
		IEKG3DSceneEntity* pEntity = NULL;
		//GetSelectedEntity(0, &pEntity);

		TypeEntityAccessProxy& proxy = selTool.GetSelectionAccessProxy();
		pEntity = &proxy.GetFront();

		hRetCode = pEntity->GetType(&EntityType);
		KGLOG_COM_PROCESS_ERROR(hRetCode);
		if(EntityType==SCENEENTITY_OBJECT)
		{
			IEKG3DRepresentObject* pObject = NULL;
			hRetCode = pEntity->GetPointer((VOID **)&pObject);
			KGLOG_COM_PROCESS_ERROR(hRetCode);
			if(pObject->GetType()==REPRESENTOBJECT_NPC)
			{				
				TCHAR objectName[MAX_PATH];
				pObject->GetName(objectName, countof(objectName));
				pObject->GetModelCount(&modelCount);
				if(modelCount > 0)
				{
					pObject->GetModel(model, 0);
					TCHAR meshName[MAX_PATH];
					TCHAR modelName[MAX_PATH];
					TCHAR aniName[MAX_PATH];
					TCHAR engineDir[MAX_PATH];
					g_GetRootPath(engineDir);
					string rootDir(engineDir);
					pObject->GetModelFileName(0, meshName, modelName, aniName);										
					string folderName(meshName);
					string aniFileName;

					if(folderName == m_CurrentModelName)
					{
						m_PlayActionIndex++;

						if (m_PlayActionIndex >= m_AniFileCount)
						{
							m_PlayActionIndex = 0;
						}

						aniFileName = m_AniFileArray[m_PlayActionIndex];							
					}
					else
					{
						m_CurrentModelName = folderName;
						size_t uLastIndex = folderName.find_last_of('\\');					
						folderName = folderName.substr(0, uLastIndex);
						uLastIndex = folderName.find_last_of('\\');
						folderName = folderName.substr(0, uLastIndex);
						string relevateName = rootDir + "\\" + folderName + "\\动作";

						string fileName = relevateName + "\\*.ani";						
						HANDLE hFile;
						WIN32_FIND_DATA FindFileData;   

						hFile = FindFirstFile(fileName.c_str(), &FindFileData); 

						if (hFile != INVALID_HANDLE_VALUE)
						{
							BOOL blnExact = true;   
							m_AniFileCount = 0;   
							while (blnExact)   
							{   
								m_AniFileArray[m_AniFileCount] = folderName + "\\动作\\" + string(FindFileData.cFileName);   
								m_AniFileCount++;    
								blnExact = FindNextFile(hFile, &FindFileData);   
							}   

							FindClose(hFile);
						}

						aniFileName = m_AniFileArray[0];
					}

					model->PlayAnimation(ENU_ANIMATIONPLAY_CIRCLE, aniFileName.c_str(), 1.0f, 0, NULL, NULL, ANICTL_PRIMARY);
				}							
			}			
		}
	}

Exit0:	
	return S_OK;
}

HRESULT KG3DSceneSceneEditor::NpcPlayPreviousAction()
{
	HRESULT hRetCode = E_FAIL;
	SCENEENTITYTYPE EntityType = SCENEENTITY_NONE;
	IKG3DModel* model;
	INT selectedEntityCount = 0;
	INT modelCount = 0;

	KG3DSceneSelectionTool& selTool = this->GetSelectionTool();
	selectedEntityCount = selTool.GetSelectionCount();

	//GetSelectedEntityCount(&selectedEntityCount);
	if(selectedEntityCount > 0)
	{
		IEKG3DSceneEntity* pEntity = NULL;
		//GetSelectedEntity(0, &pEntity);

		TypeEntityAccessProxy& proxy = selTool.GetSelectionAccessProxy();
		pEntity = &proxy.GetFront();

		hRetCode = pEntity->GetType(&EntityType);
		KGLOG_COM_PROCESS_ERROR(hRetCode);
		if(EntityType==SCENEENTITY_OBJECT)
		{
			IEKG3DRepresentObject* pObject = NULL;
			hRetCode = pEntity->GetPointer((VOID **)&pObject);
			KGLOG_COM_PROCESS_ERROR(hRetCode);
			if(pObject->GetType()==REPRESENTOBJECT_NPC)
			{				
				TCHAR objectName[MAX_PATH];
				pObject->GetName(objectName, countof(objectName));
				pObject->GetModelCount(&modelCount);
				if(modelCount > 0)
				{
					pObject->GetModel(model, 0);
					TCHAR meshName[MAX_PATH];
					TCHAR modelName[MAX_PATH];
					TCHAR aniName[MAX_PATH];
					TCHAR engineDir[MAX_PATH];
					g_GetRootPath(engineDir);
					string rootDir(engineDir);
					pObject->GetModelFileName(0, meshName, modelName, aniName);										
					string folderName(meshName);
					string aniFileName;

					if(folderName == m_CurrentModelName)
					{
						m_PlayActionIndex--;

						if (m_PlayActionIndex < 0)
						{
							m_PlayActionIndex = m_AniFileCount - 1;
						}						

						aniFileName = m_AniFileArray[m_PlayActionIndex];							
					}
					else
					{
						m_CurrentModelName = folderName;
						size_t uLastIndex = folderName.find_last_of('\\');					
						folderName = folderName.substr(0, uLastIndex);
						uLastIndex = folderName.find_last_of('\\');
						folderName = folderName.substr(0, uLastIndex);
						string relevateName = rootDir + "\\" + folderName + "\\动作";

						string fileName = relevateName + "\\*.ani";						
						HANDLE hFile;
						WIN32_FIND_DATA FindFileData;   

						hFile = FindFirstFile(fileName.c_str(), &FindFileData); 

						if (hFile != INVALID_HANDLE_VALUE)
						{
							BOOL blnExact = true;    
							m_AniFileCount = 0;   
							while (blnExact)   
							{   
								m_AniFileArray[m_AniFileCount] = folderName + "\\动作\\" + string(FindFileData.cFileName);   
								m_AniFileCount++;     
								blnExact = FindNextFile(hFile, &FindFileData);   
							}   

							FindClose(hFile);
						}

						aniFileName = m_AniFileArray[0];
					}

					model->PlayAnimation(ENU_ANIMATIONPLAY_CIRCLE, aniFileName.c_str(), 1.0f, 0, NULL, NULL, ANICTL_PRIMARY);
				}							
			}			
		}
	}

Exit0:	
	return S_OK;
}

HRESULT KG3DSceneSceneEditor::SetObjectSelectMaskDesign(DWORD mask)
{
	if (mask == -1)
	{
		KG3DSceneSelectBase::SetSelectOption(EM_SELMASK_ALL, -1);
	}
	else
	{
		KG3DSceneSelectBase::SetSelectOption(EM_SELMASK_LOGICOBJ, mask);
	}
	return S_OK;
}

HRESULT KG3DSceneSceneEditor::SetCameraLocation(FLOAT fPositionX, FLOAT fPositionY, FLOAT fPositionZ, FLOAT fDirectionX, FLOAT fDirectionY, FLOAT fDirectionZ)
{
	D3DXVECTOR3* position = new D3DXVECTOR3(fPositionX, fPositionY, fPositionZ);
	D3DXVECTOR3* direction = new D3DXVECTOR3(fDirectionX, fDirectionY, fDirectionZ);
	m_ForceSetCameraData.position = *position;
	m_ForceSetCameraData.direction = *direction;
	m_ForceSetCameraData.valid = TRUE;

	return S_OK;
}

HRESULT KG3DSceneSceneEditor::RotateSelectedObject(LPTSTR newRotationStr)
{
	D3DXVECTOR3 mousePosition(0, 0, 0);
	D3DXVECTOR3 vSrc;
	D3DXVECTOR3 vNormalizedDir;
	BOOL bFoundInter = FALSE;

	KG3DSceneOutputWnd* pWnd = this->KG3DScene::GetPrimaryWindow();
	pWnd->GetPickRay(&vSrc, &vNormalizedDir, NULL);

	if (this->IsUseCrossPlayMode()) //连对象也检测
	{
		FLOAT fTempRet = FLT_MAX;
		bFoundInter = ::RayIntersection(*this, vSrc, vNormalizedDir, &fTempRet, NULL, NULL);
		if (bFoundInter)
		{
			mousePosition = vSrc + vNormalizedDir * fTempRet;

			
			KG3DSceneEntity Entity;
			D3DXQUATERNION dValue;
			std::tstring rotationString = _T("");
			BOOL entitySelected = /*!(m_listSelectedEntity.size() == 0)*/ 0 != this->GetSelectionTool().GetSelectionCount();

			/*KG3DSceneEntityList::iterator i = m_listSelectedEntity.begin();
			while (i != m_listSelectedEntity.end())
			{
				Entity = *i;*/

			TypeEntityAccessProxy& proxy = this->GetSelectionTool().GetSelectionAccessProxy();
			for(proxy.Begin(); ! proxy.IsEnd(); proxy.StepForward())
			{
				KG3DSceneEntity Entity = proxy.GetCur();

				D3DXVECTOR3 dValueXYZ;

				D3DXVECTOR3 Trans;
				Entity.GetTranslation(&Trans);

				D3DXVECTOR3 vFace = mousePosition - Trans;
				D3DXVec3Normalize(&vFace, &vFace);					

				D3DXQuaternionRotationAxis(&dValue, &D3DXVECTOR3(0,1,0), (vFace.z < 0 ? acos(vFace.x) : -acos(vFace.x)) - D3DX_PI/2);

				Entity.SetRotation(&dValue, NULL);
				
				TCHAR charArray[128];
				sprintf(charArray, "%f,%f,%f,%f;", dValue.x, dValue.y, dValue.z, dValue.w);					
				rotationString = rotationString + string(charArray);
				entitySelected = TRUE;
				
				//++i;
			}

			if (entitySelected)
			{
				sprintf(newRotationStr, "%s", rotationString.c_str());
				//*newRotationStr = _com_util::ConvertStringToBSTR(rotationString.c_str());				
			}
		}
	}

	return S_OK;
}

HRESULT KG3DSceneSceneEditor::SetSelected(IEKG3DRepresentObject* pObject, BOOL bSelected)
{
	/*for (KG3DSceneEntityList::iterator it = m_listSelectableEntity.begin(); it != m_listSelectableEntity.end(); it++)
	{
		KG3DSceneEntity entity = *it;
		if (entity.m_pPointer == static_cast<LPVOID>(pObject))
		{
			if (bAdd)
			{
				AddSelectedEntity(entity);
			}
			else
			{
				m_listSelectedEntity.remove(entity);
			}
			break;
		}
	}
	return S_OK;*/
	if(NULL == pObject)
		return E_FAIL;
	return this->GetSelectionTool().SetSelected(*pObject, bSelected);
}

HRESULT KG3DSceneSceneEditor::UpdateSelectableEntityList(LONG iCount)
{
 	FrameMove();
 	IEKG3DSceneOutputWnd* pWnd = NULL;
 	GetCurOutputWnd(&pWnd);
 	this->PrepareRenderMainWindow((KG3DSceneOutputWnd*)pWnd);

	for (int i = 0; i < iCount; i++)
		m_EntityMgr.GetSelectableEntityList(/*&m_listSelectableEntity, */*this, !g_bClient,g_bClient);
	return S_OK;
}

HRESULT KG3DSceneSceneEditor::GetSceneRect(LONG* width, LONG* height)
{
	D3DXVECTOR2 vLowerLeft, vUpperRight;
	HRESULT hr = m_lpSceneOutDoorSpaceMgr->GetSceneRect(&vLowerLeft, &vUpperRight);
	*width = (LONG)vUpperRight.x;
	*height = (LONG)vUpperRight.y;
	return hr;
}

void KG3DSceneSceneEditor::SetSelectOption(DWORD dwOption)
{
	KG3DSceneSelectBase::SetSelectOption(dwOption, -1);
}
DWORD KG3DSceneSceneEditor::GetSelectOption()
{
	return KG3DSceneSelectBase::GetSelectOption();
}
void KG3DSceneSceneEditor::SetLimitValue(DWORD dwLimitValue)
{
	KG3DSceneSelectBase::SetLimitValue(dwLimitValue);
}
DWORD KG3DSceneSceneEditor::GetLimitValue()
{
	return KG3DSceneSelectBase::GetLimitValue();
}

void __stdcall KG3DSceneSceneEditor::OnReceiveMessage( const KG3DMessage& Message , IKG3DMessageSenderBase* pSender )
{

	switch(Message.m_uMessage)
	{
	case KM_SELECT:
		{
			if(Message.m_dwLParam != EM_EVENT_STATE_END)
				break;
			TypeEntityAccessProxy& proxy = this->GetSelectionTool().GetSelectionAccessProxy();
			for(proxy.Begin(); ! proxy.IsEnd(); proxy.StepForward())
			{
				KG3DSceneEntity Entity = proxy.GetCur();

				if (Entity.GetType() !=SCENEENTITY_OBJECT)
					continue;

				KG3DRepresentObject* pObject = 
					static_cast<KG3DRepresentObject*>(Entity.m_pPointer);

				if(pObject->GetType()==REPRESENTOBJECT_NODE)
				{
					KG3DRepresentObjectNode* pNode = static_cast<KG3DRepresentObjectNode*>(pObject);
					if (m_lpSceneOutDoorSpaceMgr && m_lpSceneOutDoorSpaceMgr->m_lpRoadMgr)
					{
						if (pNode->GetNodeType() == NODETYPE_ROAD)
						{
							KG3DSerpentine* pRoad;
							pNode->GetPointer((PVOID*)&pRoad);
							DWORD i = m_lpSceneOutDoorSpaceMgr->m_lpRoadMgr->GetSerpentineIndex(pRoad);
							FireEvent2(EVENT_ROADNODE_SELECTED,0,i,0);
							break;
						}
					}	
				}
				
			}//end of for
		}
		break;
	case KM_TRANSLATION:
		{
			if(Message.m_dwLParam != EM_EVENT_STATE_END)
				break;
			TypeEntityAccessProxy& proxy = this->GetSelectionTool().GetSelectionAccessProxy();
			 for(proxy.Begin(); ! proxy.IsEnd(); proxy.StepForward())
			 {
				KG3DSceneEntity& Entity = proxy.GetCur();

				 if (Entity.GetType()==SCENEENTITY_OBJECT)
				 {

					 KG3DRepresentObject* pObject = 
						 static_cast<KG3DRepresentObject*>(Entity.m_pPointer);

					 if(pObject->GetType()==REPRESENTOBJECT_NPC)
					 {
						 FireEvent2(EVENTEDITORNPCMOVED,0,0,pObject);
					 }
					 else if(pObject->GetType()==REPRESENTOBJECT_DOODAD)
					 {
						 FireEvent2(EVENTEDITORDOODADMOVED,0,0,pObject);
					 }
					 else if(pObject->GetType()==REPRESENTOBJECT_WAYPOINT)
					 {
						 FireEvent2(EVENTEDITORWAYPOINTMOVED,0,0,pObject);
					 }	
					 else if(pObject->GetType()==REPRESENTOBJECT_TRAFFICPOINT)
					 {
						 FireEvent2(EVENTEDITORTRAFFICPOINTMOVED,0,0,pObject);
					 }	
					 else if(pObject->GetType()==REPRESENTOBJECT_REFRESHPOINT)
					 {
						 FireEvent2(EVENTEDITORREFRESHPOINTMOVED,0,0,pObject);
					 }
					 else if (pObject->GetType()==REPRESENTOBJECT_NODE)// add by huangjinshou 2008-2-21
					 {
						 KG3DRepresentObjectNode* pObjectNode = (KG3DRepresentObjectNode*) pObject;
						 pObjectNode->Refresh();
					 }

					 pObject->ComputeBBox();
					 pObject->OnTransformationChange(this);
				 }
			 }//End of for
			 
			 if (m_pEditSpaceNode != m_lpSceneOutDoorSpaceMgr)
			    UpdateEditSpaceNodeInformation();
		}
	    break;
	case KM_ROTATION:
		{
			if(Message.m_dwLParam != EM_EVENT_STATE_END)
				break;

			TypeEntityAccessProxy& proxy = this->GetSelectionTool().GetSelectionAccessProxy();
			for(proxy.Begin(); ! proxy.IsEnd(); proxy.StepForward())
			{
				KG3DSceneEntity& Entity = proxy.GetCur();

				if (Entity.GetType()==SCENEENTITY_OBJECT)
				{

					KG3DRepresentObject* pObject = 
						static_cast<KG3DRepresentObject*>(Entity.m_pPointer);

					if(pObject->GetType()==REPRESENTOBJECT_NPC)
					{
						FireEvent2(EVENTEDITORNPCROTATED,0,0,pObject);
					}
					else if(pObject->GetType()==REPRESENTOBJECT_DOODAD)
					{
						FireEvent2(EVENTEDITORDOODADROTATED,0,0,pObject);
					}
					else if(pObject->GetType()==REPRESENTOBJECT_WAYPOINT)
					{
						FireEvent2(EVENTEDITORWAYPOINTROTATED,0,0,pObject);
					}	
					else if(pObject->GetType()==REPRESENTOBJECT_REFRESHPOINT)
					{
						FireEvent2(EVENTEDITORREFRESHPOINTROTATED,0,0,pObject);
					}

					pObject->ComputeBBox();
					pObject->OnTransformationChange(this);
				}
			}

			if (m_pEditSpaceNode != m_lpSceneOutDoorSpaceMgr)
				UpdateEditSpaceNodeInformation();
		}
		break;
	case KM_SCALING:
		{
			TypeEntityAccessProxy& proxy = this->GetSelectionTool().GetSelectionAccessProxy();
			for(proxy.Begin(); ! proxy.IsEnd(); proxy.StepForward())
			{
				KG3DSceneEntity& Entity = proxy.GetCur();

				if (Entity.GetType()==SCENEENTITY_OBJECT)
				{

					KG3DRepresentObject* pObject = 
						static_cast<KG3DRepresentObject*>(Entity.m_pPointer);

					if(pObject->GetType()==REPRESENTOBJECT_NPC)
					{
						FireEvent2(EVENTEDITORNPCSCALED,0,0,pObject);
					}
					else if(pObject->GetType()==REPRESENTOBJECT_DOODAD)
					{
						FireEvent2(EVENTEDITORDOODADSCALED,0,0,pObject);
					}

					pObject->ComputeBBox();
					pObject->OnTransformationChange(this);
				}
			}

			if (m_pEditSpaceNode != m_lpSceneOutDoorSpaceMgr)
				UpdateEditSpaceNodeInformation();
		}
		break;
	default:
	    break;
	}
}

ULONG KG3DSceneSceneEditor::GetObjectCount( DWORD dwTypeFilter )
{
	if (NULL == m_lpSceneOutDoorSpaceMgr)
	{
		return 0;
	}
	return m_lpSceneOutDoorSpaceMgr->GetObjectCount(dwTypeFilter);
}

HRESULT KG3DSceneSceneEditor::Traverse( KG3DRepresentObjectVisitor& visitor )
{
	KG_PROCESS_ERROR(NULL != m_lpSceneOutDoorSpaceMgr);
	{
		D3DXVECTOR2 vLowerLeft, vUpperRight;
		HRESULT hr = m_lpSceneOutDoorSpaceMgr->GetSceneRect(&vLowerLeft, &vUpperRight);
		KG_COM_PROCESS_ERROR(hr);

		KG3DEntityCtVisitorForObjVisitor ctVisitorTemp(visitor);

		return m_lpSceneOutDoorSpaceMgr->Traverse(ctVisitorTemp, vLowerLeft.x, vLowerLeft.y, vUpperRight.x, vUpperRight.y);
	}
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneSceneEditor::GetSelectionToolInterface( IEKG3DSceneSelectionTool** ppTool )
{
	return KG3DSceneEditorBase::GetSelectionToolInterface(ppTool);
}
HRESULT KG3DSceneSceneEditor::SetAlwayRenderWater(BOOL bVal)
{
	g_bAlwayRenderWater = bVal;

	LPD3DXEFFECT pWaterEffect = g_GetDefaultShaderGroup().GetDefaultEffect(DEFST_WATER);
	if (NULL != pWaterEffect)
	{
		pWaterEffect->SetBool("bAlphaBlend",!bVal);
	}
	return S_OK;
}
BOOL KG3DSceneSceneEditor::GetAlwayRenderWater()
{
	return g_bAlwayRenderWater;
}

extern BOOL g_EnableLoadTexture;

HRESULT KG3DSceneSceneEditor::EnableLoadTexture(BOOL bEnable)
{
	g_EnableLoadTexture = bEnable;
	return S_OK;
}

HRESULT KG3DSceneSceneEditor::PauseMultiThreadLoad(BOOL bEnable)
{
	m_lpSceneOutDoorSpaceMgr->PauseMultiThreadLoad(bEnable);
	g_cMultiThreadLoader.PauseMultiThreadLoad(bEnable);

	return S_OK;
}

LPDIRECT3DCUBETEXTURE9 KG3DSceneSceneEditor::GetEnvMap(LPTSTR pNameBuffer, size_t uBufferCount)//<EnvMap Mov>
{
	if (NULL == m_lpEvnMap)	//不曾初始化的话，使用Default那个就够了
	{
		if (NULL != pNameBuffer)
		{
			_tcscpy_s(pNameBuffer, uBufferCount, KG3DTextureTable::GetDefaultEnvMapName());
		}
		return KG3DTextureTable::GetDefaultEnvMap();
	}

	if (NULL != pNameBuffer)
	{
		_tcscpy_s(pNameBuffer, uBufferCount, m_strEnvMapName.c_str());
	}
	return m_lpEvnMap;
}

HRESULT KG3DSceneSceneEditor::SetEnvMap(LPCTSTR szPath)//<EnvMap Mov>
{
	if (NULL != _tcsstr(szPath, KG3DTextureTable::GetDefaultEnvMapName()))
	{
		return S_OK;	//使用了默认的环境路径，那么用全局的就行了
	}

	LPDIRECT3DCUBETEXTURE9 pTex = NULL;
	HRESULT hr = KG3DD3DXCreateCubeTextureFromFile(g_pd3dDevice, szPath, &pTex);
	if (SUCCEEDED(hr))
	{
		SAFE_RELEASE(m_lpEvnMap);
		m_lpEvnMap = pTex;
		m_strEnvMapName = szPath;
	}
	return hr;
}

DWORD KG3DSceneSceneEditor::GetGroundType(const D3DXVECTOR3 &vec3Pos)
{
	DWORD dwReturn = INVALID_GROUNDTYPE;

	const TerrainRepresentInfo *pInfo = GetGroundRepresentInfo(vec3Pos);
	KG_PROCESS_ERROR(pInfo);

	dwReturn = pInfo->dwType;
Exit0:
	return dwReturn;
}

const TerrainRepresentInfo *KG3DSceneSceneEditor::GetGroundRepresentInfo(const D3DXVECTOR3 &vec3Pos)
{
	const TerrainRepresentInfo *pReturn = NULL;
	D3DXVECTOR2 vec2Pos(vec3Pos.x, vec3Pos.z);
	
	KG_PROCESS_ERROR(m_lpSceneOutDoorSpaceMgr);

	DWORD dwTextureIndex = m_lpSceneOutDoorSpaceMgr->GetGroundType(vec2Pos);
	KG_PROCESS_ERROR(dwTextureIndex != INVALID_GROUNDTYPE);

	KG_PROCESS_ERROR(m_EntityMgr.m_lpTerrainRender);
	DWORD dwHash = m_EntityMgr.m_lpTerrainRender->GetDetailTextureInfo(dwTextureIndex);
	pReturn = g_GetTerrainRepresentInfo().GetInfo(dwHash);

Exit0:
	return pReturn;
}
HRESULT KG3DSceneSceneEditor::BuildPolyAndSetInHand(int nVergeNum,float fRadii,float fHeight,float fDegree)
{
	HRESULT hr = S_OK;
	KG3DPoly poly;
	
	D3DXVECTOR3 vPos(0,0,0); 
	float fRadian = D3DXToRadian(fDegree);
	for (int i=0;i < nVergeNum;i++)
	{
		vPos.x = cosf(2* D3DX_PI / nVergeNum * i + fRadian)* fRadii;
		vPos.z = sinf(2* D3DX_PI / nVergeNum * i + fRadian)* fRadii;
		poly.AddVertex(vPos);
	}

	poly.SetHeight(fHeight);
	poly.ComputeBBox();


	KG3DRepresentPoly* pPoly = NULL;
	hr = g_GetObjectTable().NewOneResourse(REPRESENTOBJECT_POLY,(IKG3DResourceBase**)&pPoly);
	KGLOG_COM_PROCESS_ERROR(hr);

	pPoly->CreateFromScenePoly(&poly);
	{
		IEKG3DRepresentObject* pObject = static_cast<IEKG3DRepresentObject*>(static_cast<KG3DRepresentObject*>(pPoly));
		SetObjectInHandle(pObject,TRUE);
	}

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneSceneEditor::UpdateSettings(DWORD dwSettingType)
{
    HRESULT hResult  = E_FAIL;
    HRESULT hRetCode = E_FAIL;
    TCHAR szRetName[MAX_PATH] = {0};
    KG_CUSTOM_HELPERS::StrPathSplited  pathSplited;

    {
        BOOL bRetCode = FALSE;
        TCHAR strPathBuffer[MAX_PATH] = _T("");
        bRetCode = (0 == _tcscpy_s(strPathBuffer, m_scName.c_str()));
        KGLOG_PROCESS_ERROR(bRetCode && _T("Scene保存，路径名处理失败，保存失败"));

        bRetCode = KG_CUSTOM_HELPERS::StrReplace(strPathBuffer, _T('/'), _T('\\'));
        _ASSERTE(bRetCode);

        bRetCode = KG_CUSTOM_HELPERS::StrPathSplit(strPathBuffer, pathSplited);
        KGLOG_PROCESS_ERROR(bRetCode && _T("Scene保存，分解文件路径失败，保存失败"));
    }
    
    // save prev
    {
        hRetCode = KG3DCollocator::GetSavePath(EM_SAVE_OP_ENVIRONMENT, pathSplited, NULL, szRetName);
        KG_COM_PROCESS_ERROR(hRetCode);

        hRetCode = GetEnvironment().Save(szRetName);
        KG_COM_PROCESS_ERROR(hRetCode);
        
        hRetCode = KG3DCollocator::GetSavePath(EM_SAVE_OP_EFFECTPARAMS, pathSplited, NULL, szRetName);
        KG_COM_PROCESS_ERROR(hRetCode);

        memcpy(&PostRender::KG3DPostRenderEffectManager::m_sEffectParams, &m_EffectParams, sizeof(PostRenderParams));
        hRetCode = PostRender::KG3DPostRenderEffectManager::SaveParams(szRetName);
        KG_COM_PROCESS_ERROR(hRetCode);
    }

    GetEnvironment().SetSettingType(dwSettingType);

    // load
    {
        hRetCode = KG3DCollocator::GetSavePath(EM_SAVE_OP_ENVIRONMENT, pathSplited, NULL, szRetName);
        KG_COM_PROCESS_ERROR(hRetCode);

        hRetCode = GetEnvironment().Reload(szRetName);

        hRetCode = KG3DCollocator::GetSavePath(EM_SAVE_OP_EFFECTPARAMS, pathSplited, NULL, szRetName);
        KG_COM_PROCESS_ERROR(hRetCode);

        hRetCode = PostRender::KG3DPostRenderEffectManager::LoadParams(szRetName);
        memcpy(&m_EffectParams, &PostRender::KG3DPostRenderEffectManager::m_sEffectParams, sizeof(PostRenderParams));
    }

    hResult = S_OK;
Exit0:
    return hResult;
}

void KG3DSceneSceneEditor::EnableBlockBox(BOOL bEnable)
{
	g_bEnableBlockBox = bEnable;
}