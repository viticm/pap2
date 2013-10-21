#include "StdAfx.h"
#include "./IPPVideo/IPPCodec.h"
#include "./KG3DEngineManager.h"
#include "./KG3DScene.h"
#include "./KG3DSceneWndManager.h"
#include "./KG3DSceneOutputWnd.h"
#include "./KG3DGraphicsTool.h"
#include "./KG3DGraphiceEngine.h"
#include "./KG3DPostRenderEffectShockWave.h"
#include "../ViMD3DLib/include/ViMD3DLib.h"				//ViM

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////
extern DWORD g_dwNumViews;			// ViM
extern DWORD g_xpos, g_ypos, g_width, g_height, g_adapterIdx, g_refreshRate;	// ViM
extern KG3DEngineOption g_cEngineOption;

KG3DSceneOutputWnd::KG3DSceneOutputWnd( KG3DSceneWndManager* pMaster )
:m_pCamera(reinterpret_cast<KG3DCamera*>(m_CameraBuffer))
{
	{
		/*由config.ini中"KG3DENGINE"下的"TrackCameraOrSimpleCamera"控制
		如果非客户端的话，创建的一定是TrackCamera
		
		因为黄蕾那边新的Camera已经改完了，所以客户端不需要带Track的摄像机了，
		去掉bTrackCameraOrSimpleCamera标志 Last modify by Chen TianHong:2008-9-22 11:17:15
		*/
		bool bUseTrackCamera = /*g_cEngineOption.bTrackCameraOrSimpleCamera ||*/ ! g_bClient;
		InitCamera(m_CameraBuffer, _countof(m_CameraBuffer)*sizeof(BYTE), bUseTrackCamera);
	}
	m_pMaster = pMaster;
	ASSERT( m_pMaster != NULL );

	m_bPresent = TRUE;
	m_bNeedRender = TRUE;
	m_nID = 0;
	m_hWnd = NULL;
	m_dwOption = 0;
	m_dwType = 0xffffffff;///<CTH>///设一个绝对不可能存在的值作为错误

	ZeroMemory(&m_Rect,sizeof(RECT));
	ZeroMemory(&m_Viewport,sizeof(D3DVIEWPORT9));
	ZeroMemory(m_szName,sizeof(TCHAR)*MAX_PATH);

	m_pOptionSpace = NULL;
	m_nOptionSpaceSize = 0;

	m_bEnableOutput = TRUE;

	m_pRenderTextures = NULL;	// ViM
	m_pRenderTargets = NULL;	// ViM
	m_fCamTranslation = 40.0f;	// ViM
	m_fCamFixpoint = 3.6f;		// ViM
	m_bUseVIM3D = FALSE;

	_ASSERTE(NULL != m_pCamera);

	m_lpParentScene = pMaster->GetParentScene();
	m_RenderManager.SetParentScene( m_lpParentScene );

	m_bEnableScaleOutput = FALSE;
}

KG3DSceneOutputWnd::~KG3DSceneOutputWnd(void)
{
	UnInitCamera(m_CameraBuffer, _countof(m_CameraBuffer)*sizeof(BYTE));

	SAFE_DELETE(m_pOptionSpace);
	if(m_bUseVIM3D)
		UnInitV3D();
}

HRESULT KG3DSceneOutputWnd::InitV3D()
{
	D3DSURFACE_DESC desc;															// ViM
	LPDIRECT3DSURFACE9 surf=NULL;
	HRESULT hr = E_FAIL;// ViM
	hr = g_pd3dDevice->GetRenderTarget(0, &surf);									// ViM
	KG_COM_PROCESS_ERROR(hr);														// ViM
	hr= surf->GetDesc(&desc);
	KG_COM_PROCESS_ERROR(hr);														// ViM

	m_pRenderTextures = new LPDIRECT3DTEXTURE9[g_dwNumViews];						// ViM
	KG_PROCESS_ERROR(m_pRenderTextures);
	ZeroMemory(m_pRenderTextures, g_dwNumViews*sizeof(LPDIRECT3DTEXTURE9));			// ViM
	
	m_pRenderTargets = new LPDIRECT3DSURFACE9[g_dwNumViews];						// ViM
	KG_PROCESS_ERROR(m_pRenderTargets);
	ZeroMemory(m_pRenderTargets, g_dwNumViews*sizeof(LPDIRECT3DSURFACE9));			// ViM

	for (DWORD i=0;i<g_dwNumViews;i++)												// ViM
	{																				// ViM
		hr = g_pd3dDevice->CreateTexture(desc.Width/2, desc.Height/2, 1, 			// ViM
			D3DUSAGE_RENDERTARGET, desc.Format, D3DPOOL_DEFAULT, 					// ViM
			&m_pRenderTextures[i], NULL);											// ViM
		KG_COM_PROCESS_ERROR(hr);													// ViM
		hr = m_pRenderTextures[i]->GetSurfaceLevel(0, &m_pRenderTargets[i]);		// ViM
		KG_COM_PROCESS_ERROR(hr);													// ViM
	}																				// ViM
	SAFE_RELEASE(surf);
																					// ViM
	m_bUseVIM3D = TRUE;

	return S_OK;
Exit0:
	if (m_pRenderTextures)
	{
		for (DWORD i = 0; i < g_dwNumViews; i++)
		{
			SAFE_RELEASE(m_pRenderTextures[i]);
		}
	}
	if (m_pRenderTargets)
	{
		for (DWORD i = 0; i < g_dwNumViews; i++)
		{
			SAFE_RELEASE(m_pRenderTargets[i]);
		}
	}
	SAFE_DELETE_ARRAY(m_pRenderTextures);
	SAFE_DELETE_ARRAY(m_pRenderTargets);
	SAFE_RELEASE(surf);
	return E_FAIL;
}

HRESULT KG3DSceneOutputWnd::UnInitV3D()
{
	if (m_pRenderTargets)									// ViM
	{														// ViM
		for (DWORD i=0;i<g_dwNumViews;i++)					// ViM
		{														// ViM
			if (m_pRenderTargets[i])							// ViM
				m_pRenderTargets[i]->Release();					// ViM
		}														// ViM
		delete [] m_pRenderTargets; m_pRenderTargets = 0;		// ViM
	}														// ViM
	if (m_pRenderTextures)									// ViM
	{														// ViM
		for (DWORD i=0;i<g_dwNumViews;i++)					// ViM
		{														// ViM
			if (m_pRenderTextures[i])							// ViM
				m_pRenderTextures[i]->Release();					// ViM
		}														// ViM
		delete [] m_pRenderTextures; m_pRenderTextures = 0;	// ViM
	}														// ViM

	m_bUseVIM3D = FALSE;
	return S_OK;
}

HRESULT KG3DSceneOutputWnd::GetViewPortWidthHeight(
    DWORD *pdwRetWidth, DWORD *pdwRetHeight
)
{
    HRESULT hResult  = E_FAIL;

    KGLOG_PROCESS_ERROR(pdwRetWidth);
    KGLOG_PROCESS_ERROR(pdwRetHeight);

    *pdwRetWidth  = m_Viewport.Width;
    *pdwRetHeight = m_Viewport.Height;

//Exit1:
    hResult = S_OK;
Exit0:
    return hResult;
}

////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DSceneOutputWnd::GetRect(RECT *pRetRect)
{
    HRESULT hResult  = E_FAIL;

    KGLOG_PROCESS_ERROR(pRetRect);

    *pRetRect = m_Rect;

//Exit1:
    hResult = S_OK;
Exit0:
    return hResult;
}

////////////////////////////////////////////////////////////////////////////////
void KG3DSceneOutputWnd::OnResizeWindow(PRECT const pRect)
{
    ASSERT(m_hWnd);
    ASSERT(m_pCamera);

	if (pRect == NULL && m_hWnd)
		::GetClientRect(m_hWnd, &m_Rect);
	else
		m_Rect = *pRect;

	m_Viewport.X = m_Rect.left;
	m_Viewport.Y = m_Rect.top;
	m_Viewport.Width = m_Rect.right - m_Rect.left;
	m_Viewport.Height = m_Rect.bottom - m_Rect.top;
	m_Viewport.MinZ = 0.0F;
	m_Viewport.MaxZ = 1.0F;


    float Fov, As, Zn, Zf, AsNew;

	if(m_Viewport.Height != 0)
		AsNew = ((float)m_Viewport.Width) / ((float)m_Viewport.Height);	
	else
		AsNew = 800.0f / 600.0f;

    if (AsNew <= 0.f)
        return;

    m_pCamera->GetPerspective(&Fov, &As,   &Zn, &Zf);
    m_pCamera->SetPerspective(Fov,  AsNew, Zn,   Zf);

    m_pCamera->GetGlobalPerspective(&Fov, &As,   &Zn, &Zf);
    m_pCamera->SetGlobalPerspective(Fov,  AsNew, Zn,   Zf);
    

    ResetDevice(IsEnablePostRender(), m_Viewport.Width, m_Viewport.Height);


	// ipp video
	ASSERT( m_pMaster );
    IEKG3DSceneOutputWnd *piPrimaryWindow = NULL;
    m_pMaster->GetPrimaryWindow(&piPrimaryWindow);
	ASSERT(piPrimaryWindow);
	if (piPrimaryWindow != static_cast<IEKG3DSceneOutputWnd *>(this))
	{
		return ;
	}
}

HRESULT KG3DSceneOutputWnd::GetNear_Far_Point(D3DXVECTOR3 &Near,D3DXVECTOR3 &Far)
{
	D3DXVECTOR3 nv,fv;
	D3DXMATRIX matProj,matView,m;
	POINT ptCursor;
	GetCursorPos( &ptCursor );
	ScreenToClient( m_hWnd, &ptCursor );

	float Width  = (float)m_Viewport.Width;
	float Height = (float)m_Viewport.Height;

	float X = (float)ptCursor.x;
	float Y = (float)ptCursor.y;

	

	fv.x =(float)  ( ( ( 2.0f * X ) / Width  ) - 1.0f);
	fv.y =(float) -( ( ( 2.0f * Y ) / Height ) - 1.0f);
	fv.z =  1.0f;

	nv.x =(float)  ( ( ( 2.0f * X ) / Width  ) - 1.0f);
	nv.y =(float) -( ( ( 2.0f * Y ) / Height ) - 1.0f);
	nv.z =  0.1f;

	g_pd3dDevice->GetTransform( D3DTS_PROJECTION, &matProj );
	g_pd3dDevice->GetTransform( D3DTS_VIEW, &matView );
	m = matView * matProj;
	D3DXMatrixInverse(&m,0,&m);

	D3DXVec3TransformCoord(&Near,&nv,&m);
	D3DXVec3TransformCoord(&Far,&fv,&m);


	return S_OK;
}

HRESULT KG3DSceneOutputWnd::GetCursorScreenPos(D3DXVECTOR2 &ScreenPos)
{
	POINT ptCursor;
	GetCursorPos( &ptCursor );
	ScreenToClient( m_hWnd, &ptCursor );

	float Width  = (float)m_Viewport.Width;
	float Height = (float)m_Viewport.Height;
	(void)Height;//避免C4189
	float X = (float)ptCursor.x;
	float Y = (float)ptCursor.y;

	ScreenPos.x = X / Width;
	ScreenPos.y = Y / Height;

	return S_OK;
}

//HRESULT KG3DSceneOutputWnd::GetPickRay(D3DXVECTOR3 * RayOrig,D3DXVECTOR3* pNormalizedDir,const D3DXVECTOR2 &RayOrig2D)
//{
//	D3DXVECTOR3 vPickRayDir;
//	D3DXVECTOR3 vPickRayOrig,v;
//
//	m_pCamera->SetCamera();
//	D3DXMATRIX matProj;
//	g_pd3dDevice->GetTransform( D3DTS_PROJECTION, &matProj );
//
//	float X = RayOrig2D.x;
//	float Y = RayOrig2D.y;
//
//	float Width  = (float)m_Viewport.Width;
//	float Height = (float)m_Viewport.Height;
//
//	m_vec2StartCursorPos.x = X / Width;
//	m_vec2StartCursorPos.y = Y / Height;
//
//	v.x =(float)  ( ( ( 2.0f * X ) / Width  ) - 1.0f) / matProj._11;
//	v.y =(float) -( ( ( 2.0f * Y ) / Height ) - 1.0f) / matProj._22;
//	v.z =  1.0f;
//
//	// Get the inverse view matrix
//
//	D3DXMATRIX matView, m;
//	g_pd3dDevice->GetTransform( D3DTS_VIEW, &matView );
//	D3DXMatrixInverse( &m, NULL, &matView );
//
//	// Transform the screen space pick ray into 3D space
//	vPickRayDir.x  = v.x*m._11 + v.y*m._21 + v.z*m._31;
//	vPickRayDir.y  = v.x*m._12 + v.y*m._22 + v.z*m._32;
//	vPickRayDir.z  = v.x*m._13 + v.y*m._23 + v.z*m._33;
//	vPickRayOrig.x = m._41 ;
//	vPickRayOrig.y = m._42 ; 
//	vPickRayOrig.z = m._43 ;
//
//	D3DXVec3Normalize(&vPickRayDir, &vPickRayDir);
//
//	*RayOrig = vPickRayOrig;
//	*pNormalizedDir  = vPickRayDir ;
//
//	return S_OK;
//}
HRESULT KG3DSceneOutputWnd::GetMatrixViewProjViewport( D3DXMATRIX& matViewProjViewport )
{
	D3DXMATRIX matViewport;
	D3DXMatrixViewPort(m_Viewport,matViewport);
	m_pCamera->SetCamera();	//这个会变慢，但是暂时只能这样，以后要把Camera改成不需要Set的形式
	matViewProjViewport = m_pCamera->GetViewMatrix() * m_pCamera->GetProjectionMatrix() * matViewport;
	return S_OK;
}

HRESULT KG3DSceneOutputWnd::GetPickRayWindowsPos( D3DXVECTOR2& vRayPos )
{
	POINT ptCursor;
	GetCursorPos( &ptCursor );
	ScreenToClient( m_hWnd, &ptCursor );

	D3DXVECTOR2 vPosWindow(static_cast<FLOAT>(ptCursor.x), static_cast<FLOAT>(ptCursor.y));

	vRayPos = vPosWindow;

	return S_OK;
}
HRESULT KG3DSceneOutputWnd::GetPickRayFromWindowsPos( const D3DXVECTOR2& vPosWindow, D3DXVECTOR3& vSrcRet, D3DXVECTOR3& vNormalizedDirRet)
{
	D3DXMATRIX matViewProjViewport;
	this->GetMatrixViewProjViewport(matViewProjViewport);

	::CalculatePickRay(vPosWindow, matViewProjViewport, vSrcRet, vNormalizedDirRet);

	return S_OK;
}
HRESULT KG3DSceneOutputWnd::GetPickRay(D3DXVECTOR3 * RayOrig,D3DXVECTOR3* pNormalizedDir,D3DXVECTOR2 * RayOrig2D)
{
	KG_PROCESS_ERROR(NULL != RayOrig && NULL != pNormalizedDir);

	{
		D3DXVECTOR2 vPosWindow;

		HRESULT hr = GetPickRayWindowsPos(vPosWindow);
		_ASSERTE(SUCCEEDED(hr));

		if(NULL != RayOrig2D)
			*RayOrig2D = vPosWindow;

		D3DXMATRIX matViewProjViewport;
		this->GetMatrixViewProjViewport(matViewProjViewport);

		hr = ::CalculatePickRay(vPosWindow, matViewProjViewport, *RayOrig, *pNormalizedDir);
		return hr;
	}
	
Exit0:
	return E_FAIL;

	//下面的算法简化了Matrix的运算，比较快。不过在特殊矩阵下面会出错，稍微牺牲一下速度统一成比较野蛮的逆矩阵算法好了。

	/*D3DXVECTOR3 vPickRayDir;
	D3DXVECTOR3 vPickRayOrig;

	m_pCamera->SetCamera();
	D3DXMATRIX matProj;
	g_pd3dDevice->GetTransform( D3DTS_PROJECTION, &matProj );

	POINT ptCursor;
	GetCursorPos( &ptCursor );
	ScreenToClient( m_hWnd, &ptCursor );

	D3DXVECTOR3 v;
	// Compute the vector of the pick ray in screen space v

	float Width  = (float)m_Viewport.Width;
	float Height = (float)m_Viewport.Height;

	float X = (float)ptCursor.x;
	float Y = (float)ptCursor.y;

	if(RayOrig2D)
	{
		(*RayOrig2D) = D3DXVECTOR2(X,Y);
	}

	if(m_pCamera->IsPerspective())
	{
		m_vec2StartCursorPos.x = X / Width;
		m_vec2StartCursorPos.y = Y / Height;

		v.x =(float)  ( ( ( 2.0f * X ) / Width  ) - 1.0f) / matProj._11;
		v.y =(float) -( ( ( 2.0f * Y ) / Height ) - 1.0f) / matProj._22;
		v.z =  1.0f;

		// Get the inverse view matrix

		D3DXMATRIX matView, m;
		g_pd3dDevice->GetTransform( D3DTS_VIEW, &matView );
		D3DXMatrixInverse( &m, NULL, &matView );

		// Transform the screen space pick ray into 3D space
		vPickRayDir.x  = v.x*m._11 + v.y*m._21 + v.z*m._31;
		vPickRayDir.y  = v.x*m._12 + v.y*m._22 + v.z*m._32;
		vPickRayDir.z  = v.x*m._13 + v.y*m._23 + v.z*m._33;
		vPickRayOrig.x = m._41 ;
		vPickRayOrig.y = m._42 ; 
		vPickRayOrig.z = m._43 ;

		D3DXVec3Normalize(&vPickRayDir, &vPickRayDir);

		*RayOrig = vPickRayOrig;
		*pNormalizedDir  = vPickRayDir ;
	}
	else
	{
		float W,H,N,F;
		m_pCamera->GetOrthogonal(&W,&H,&N,&F);

		D3DXMATRIX matView, m;
		g_pd3dDevice->GetTransform( D3DTS_VIEW, &matView );

		float x =(float)  ( ( ( 1.0f * X ) / Width  ) - 0.5f) * W;
		float z =(float) -( ( ( 1.0f * Y ) / Height ) - 0.5f) * H;

		D3DXVECTOR3 Left = D3DXVECTOR3(matView._11,matView._21,matView._31);
		D3DXVECTOR3 Up   = D3DXVECTOR3(matView._12,matView._22,matView._32);

		D3DXVECTOR3 Pos;
		m_pCamera->GetPosition(&Pos);

		*RayOrig = Pos + x * Left + z * Up;
		D3DXVECTOR3 vTemp(matView._13,matView._23,matView._33);
		D3DXVec3Normalize(&vTemp, &vTemp);
		*pNormalizedDir  = vTemp;
	}
	return S_OK;*/
}

HRESULT KG3DSceneOutputWnd::Present()
{
	// ipp video
	ASSERT( m_pMaster );
    IEKG3DSceneOutputWnd *piPrimaryWindow = NULL;
    m_pMaster->GetPrimaryWindow(&piPrimaryWindow);
    if(NULL == piPrimaryWindow)
		return E_FAIL;
	if (piPrimaryWindow == static_cast<IEKG3DSceneOutputWnd *>(this) )
	{
		KG3DEngineManager* pEngineMng = dynamic_cast< KG3DEngineManager* >( g_GetEngineManager() );
		ASSERT( pEngineMng );

		if ( pEngineMng )
		{
			pEngineMng->MoviePresent( );
		}

	}


	if(m_bUseVIM3D)
	{
		GraphicsStruct::RenderState fog(D3DRS_FOGENABLE,FALSE);

		if (!( ViMD3D_RasterTextures(g_cGraphicsEngine.m_vimd3d,		// ViM
			0, 0, g_width, g_height, // ViM	//For fullscreen use g_xpos, g_ypos, g_width and g_height
			g_dwNumViews, m_pRenderTextures)))			// ViM
			return E_FAIL;								// ViM
	}

    if (!m_bPresent)
        return S_FALSE;

	static DWORD s_dwPresentCount = 0;
	g_cEngineManager.OnPresent(s_dwPresentCount++);
	if (m_hWnd == g_hBaseWnd && g_bFullScreen)
    {
        g_cGraphicsEngine.Present(NULL, NULL, g_hBaseWnd, NULL);
    }
    else
    {
        RECT rect;
        rect.left = rect.top = 0;
        rect.right = m_Viewport.Width;
        rect.bottom = m_Viewport.Height;

        g_cGraphicsEngine.Present(&rect, NULL, m_hWnd, NULL);
    }

	return S_OK;
}

HRESULT KG3DSceneOutputWnd::RenderWindowBegin(BOOL bSetViewPort)
{
    ASSERT(m_pCamera);

	if(bSetViewPort)
		g_pd3dDevice->SetViewport(&m_Viewport);
	
	if(!m_bUseVIM3D)
		m_pCamera->SetCamera();

	g_cGraphicsTool.SetCamera(m_pCamera);

	if(IsEnablePostRender())
	{
		m_RenderManager.RenderTargetSet();
	}

	if( FAILED( g_pd3dDevice->BeginScene() ) )
	{
		return E_FAIL;
	}

	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 
		0x007f7f7f, 1.0f, 0 );

	return S_OK;
}

HRESULT KG3DSceneOutputWnd::RenderWindowEnd()
{
    if(IsEnablePostRender() && !m_RenderManager.m_bTargetRestore)
        m_RenderManager.RenderTargetRestore();

    if (FAILED( g_pd3dDevice->EndScene()))
        return E_FAIL;

	return S_OK;
}

HRESULT KG3DSceneOutputWnd::SetOption( DWORD_PTR dwOption )
{
	KG_PROCESS_ERROR(m_dwType <= MAX_TYPE);///没有设Type就调用就会触发这个
	try///截获访问异常
	{
		if (m_dwType == OUTPUTWND_SCENEEDITOR_TEXTUREVIEW)
		{
			OUTPUTWND_TEXTURE_PARAM* pParam = (OUTPUTWND_TEXTURE_PARAM*)dwOption;
			//SAFE_DELETE(m_pOptionSpace);
			if (! m_pOptionSpace)
			{
				m_pOptionSpace = new OUTPUTWND_TEXTURE_PARAM;
				if (!m_pOptionSpace)
				{
					throw bad_alloc();
				}
			}
			m_nOptionSpaceSize = sizeof(OUTPUTWND_TEXTURE_PARAM);
			if (pParam != m_pOptionSpace)
			{
				memcpy_s(m_pOptionSpace, m_nOptionSpaceSize, pParam, m_nOptionSpaceSize);
			}
			else
			{
				_ASSERTE(NULL&& "不应该等于的，发生了什么？\n");
			}
			
		}
		m_dwOption = dwOption;
	}
	catch (...)
	{
		m_dwOption = 0;
		m_nOptionSpaceSize = 0;
		SAFE_DELETE(m_pOptionSpace);
	}
	return S_OK;
Exit0:
	_KG3D_DEBUG_REPORT("KG3DSceneOutputWnd::SetOption");
	return E_FAIL;
}

DWORD_PTR KG3DSceneOutputWnd::GetOption()
{
	if (m_dwType != OUTPUTWND_SCENEEDITOR_TEXTUREVIEW)
	{
		return m_dwOption;
	}
	return 0;
}


HRESULT KG3DSceneOutputWnd::GetOptionStruct( LPVOID pOptionSpace, INT nBufferSize, INT* nOptionSpaceSize )
{
	KG_PROCESS_ERROR(pOptionSpace && m_pOptionSpace
		&& pOptionSpace != m_pOptionSpace
		&& nBufferSize >= m_nOptionSpaceSize);
	{
		INT nRet = memcpy_s(pOptionSpace, nBufferSize, m_pOptionSpace, m_nOptionSpaceSize);
		KG_PROCESS_ERROR(nRet == 0);
		if (nOptionSpaceSize)
		{
			*nOptionSpaceSize = m_nOptionSpaceSize;
		}
	}
	return S_OK;
Exit0:
	return E_FAIL;
	
}

HRESULT KG3DSceneOutputWnd::GetTextureWndParam( OUTPUTWND_TEXTURE_PARAM* pResult )
{
	return GetOptionStruct(pResult, sizeof(OUTPUTWND_TEXTURE_PARAM), NULL);
}

HRESULT KG3DSceneOutputWnd::Enable( BOOL bEnable )
{
	m_bEnableOutput = bEnable;
	return S_OK;
}

HRESULT KG3DSceneOutputWnd::IsEnable( BOOL* bRetEnable )
{
	if (bRetEnable)
	{
		*bRetEnable = m_bEnableOutput;
		return S_OK;
	}
	return E_FAIL;
}

HRESULT KG3DSceneOutputWnd::SetType( DWORD dwType )
{
	KG_PROCESS_ERROR(dwType <= MAX_TYPE);
	m_dwType = dwType;
	return S_OK;
Exit0:
	return E_FAIL;
}

DWORD KG3DSceneOutputWnd::GetType()
{
	return m_dwType;
}

HRESULT KG3DSceneOutputWnd::GetCameraStation( IEKG3DCameraStation **ppiStation )
{
	return  m_pCamera->GetStation(ppiStation);
}

HRESULT KG3DSceneOutputWnd::InitCameraStation( const char pcszFileName[] )
{
	IEKG3DCameraStation* pCameraStation = NULL;
	HRESULT hr = m_pCamera->GetStation(&pCameraStation);
	KG_COM_PROCESS_ERROR(hr);
	return pCameraStation->IniByConfigFile(pcszFileName);
Exit0:
	return E_FAIL;
}

IEKG3DCamera* KG3DSceneOutputWnd::GetIECamera()
{
	return m_pCamera;
}

HRESULT KG3DSceneOutputWnd::ResetDevice( BOOL bEnable, DWORD dwWidth, DWORD dwHeight )
{
	return m_RenderManager.ResetDevice(bEnable, dwWidth, dwHeight, FALSE,m_bEnableScaleOutput);
}

HRESULT KG3DSceneOutputWnd::EnablePostEffect( PostRenderEffectType Type, BOOL bEnable )
{
	return m_RenderManager.EnablePostEffect(Type, bEnable,m_bEnableScaleOutput);
}

HRESULT KG3DSceneOutputWnd::ReloadFX()
{
	return m_RenderManager.ReloadFX();
}

PostRender::KG3DPostRenderEffectManager* KG3DSceneOutputWnd::GetPostRenderManager()
{
	return &m_RenderManager;
}

PostRender::KG3DPostRenderEffectBloom* KG3DSceneOutputWnd::GetBloomEffect()
{
	return m_RenderManager.GetBloomEffect();
}

HRESULT KG3DSceneOutputWnd::OnLostDevice()
{
	if (g_bUseVIM3D)
		UnInitV3D();
	return m_RenderManager.OnLostDevice();
}

HRESULT KG3DSceneOutputWnd::OnResetDevice()
{
	if (g_bUseVIM3D)
		InitV3D();
	return m_RenderManager.OnResetDevice(m_bEnableScaleOutput);
}

KG3DCamera& KG3DSceneOutputWnd::GetCamera()
{
	return *m_pCamera;
}

void KG3DSceneOutputWnd::InitCamera(BYTE* pBuffer, INT BufferSize, bool bTrackCameraOrSimpleCamera)
{
	//// Last modify by Chen TianHong:2008-6-11 16:17:45
	//TrackCamera的功能是自动弹簧和遮挡检测，如果把这些功能移动到逻辑的话，逻辑直接用KG3DCamera就足够了
	_ASSERTE(BufferSize >= EM_CAMERA_BUFFER_SIZE);
	ZeroMemory(pBuffer, BufferSize);

#if defined(DEBUG) | defined(_DEBUG)
#pragma push_macro("new")
#undef new
#endif
	if(bTrackCameraOrSimpleCamera)
	{
		KG3DCamera* pCamera = new(pBuffer) KG3DTrackCamera;
        KG_USE_ARGUMENT(pCamera);
		_ASSERTE((VOID*)pCamera == (VOID*)pBuffer);
	}
	else
	{
		KG3DCamera* pCamera = new(pBuffer) KG3DCamera;
        KG_USE_ARGUMENT(pCamera);
		_ASSERTE((VOID*)pCamera == (VOID*)pBuffer);
	}

#if defined(DEBUG) | defined(_DEBUG)
#pragma pop_macro("new")
#endif
}

void KG3DSceneOutputWnd::UnInitCamera( BYTE* pBuffer, INT BufferSize )
{
	_ASSERTE(BufferSize >= EM_CAMERA_BUFFER_SIZE);
	KG3DCamera* p = reinterpret_cast<KG3DCamera*>(pBuffer);
	p->~KG3DCamera();//注意KG3DCamera是虚析构的，所以这里能正确调用到析构函数
}


#if defined(KG_ENABLE_TDD)
KG_TEST_BEGIN(KG3DSceneOutputWnd)
	{
		//检查Camera的正确构造和正确释放
		{
			class KG3DSceneOutputWndTest : public KG3DSceneOutputWnd
			{
			public:
				static void InitCameraTest(BYTE* pBuffer, INT BufferSize, bool bTrackCameraOrSimpleCamera)
				{
					return KG3DSceneOutputWnd::InitCamera(pBuffer, BufferSize, bTrackCameraOrSimpleCamera);
				}
				static void UnInitCameraTest(BYTE* pBuffer, INT BufferSize)
				{
					return KG3DSceneOutputWnd::UnInitCamera(pBuffer, BufferSize);
				}
				KG3DSceneOutputWndTest():KG3DSceneOutputWnd(NULL){}//反正不会真的实例化，怎么搞都行
			}; 

			BYTE Buffer[sizeof(KG3DTrackCamera)]; 

			KG3DCamera* pCamera = reinterpret_cast<KG3DCamera*>(Buffer);

			INT nBufferSize = _countof(Buffer)*sizeof(BYTE);
			KG3DSceneOutputWndTest::InitCameraTest(Buffer, nBufferSize, true);

			//这个时候，在Buffer中应该创建了一个TrackCamera
			_ASSERTE(NULL != pCamera->GetTrackCameraInterface());
			
			//把它释放掉，然后在创建一个普通Camera看看
			KG3DSceneOutputWndTest::UnInitCameraTest(Buffer, nBufferSize);
			_ASSERTE(pCamera->IsPerspective() == 0x12345678);//验证释放，TrackCamera会在释放的时候设这个值，Camera不会

			KG3DSceneOutputWndTest::InitCameraTest(Buffer, nBufferSize, false);
			_ASSERTE(NULL == pCamera->GetTrackCameraInterface());

			KG3DSceneOutputWndTest::UnInitCameraTest(Buffer, nBufferSize);
			_ASSERTE(pCamera->IsPerspective() != 0x12345678);//验证释放
		}

		//_KG3D_DEBUG_REPORT("KG3DSceneOutputWnd Test OK");
	}
KG_TEST_END(KG3DSceneOutputWnd)
#endif

int KG3DSceneOutputWnd::SetBloomGate(float fValue)
{
    m_lpParentScene->m_EffectParams.fBloomGate = fValue;
    return m_RenderManager.SetBloomGate(fValue);
}

int KG3DSceneOutputWnd::SetBloomExposure1(float fValue)
{
    m_lpParentScene->m_EffectParams.fBloomExposure1 = fValue;
    return m_RenderManager.SetBloomExposure1(fValue);
}

int KG3DSceneOutputWnd::SetBloomExposure2(float fValue)
{
    m_lpParentScene->m_EffectParams.fBloomExposure2 = fValue;
    return m_RenderManager.SetBloomExposure2(fValue);
}

int KG3DSceneOutputWnd::SetBloomGaussCoefficient(float fValue)
{
    m_lpParentScene->m_EffectParams.fBloomGaussCoefficient = fValue;
    return m_RenderManager.SetBloomGaussCoefficient(fValue);
}

int KG3DSceneOutputWnd::SetfBloomSampleNum(int nValue)
{
    m_lpParentScene->m_EffectParams.nBloomSampleNum = nValue;
    return m_RenderManager.SetfBloomSampleNum(nValue);
}

int KG3DSceneOutputWnd::SetBloomDelaye(float fValue)
{
    m_lpParentScene->m_EffectParams.fBloomDelay = fValue;
    return m_RenderManager.SetBloomDelaye(fValue);
}

int KG3DSceneOutputWnd::SetDetailPower(float fValue)
{ 
    m_lpParentScene->m_EffectParams.fDetailPower = fValue;
    return m_RenderManager.SetDetailPower(fValue);
}

int KG3DSceneOutputWnd::SetBlurPower(float fValue)
{
    m_lpParentScene->m_EffectParams.fBlurPower = fValue;
    return m_RenderManager.SetBlurPower(fValue);
}

int KG3DSceneOutputWnd::SetBlurDistance(float fValue)
{
    m_lpParentScene->m_EffectParams.fBlurDistance = fValue;
    return m_RenderManager.SetBlurDistance(fValue);
}

int KG3DSceneOutputWnd::SetHSIHueScale(float fValue)
{
    m_lpParentScene->m_EffectParams.fHueScale = fValue;
    return m_RenderManager.SetHSIHueScale(fValue);
}

int KG3DSceneOutputWnd::SetHSIHueBias(float fValue)
{
    m_lpParentScene->m_EffectParams.fHueBias = fValue;
    return m_RenderManager.SetHSIHueBias(fValue);
}

int KG3DSceneOutputWnd::SetHSISaturationScale(float fValue)
{
    m_lpParentScene->m_EffectParams.fSaturationScale = fValue;
    return m_RenderManager.SetHSISaturationScale(fValue);
}

int KG3DSceneOutputWnd::SetHSISaturationBias(float fValue)
{
    m_lpParentScene->m_EffectParams.fSaturationBias = fValue;
    return m_RenderManager.SetHSISaturationBias(fValue);
}

int KG3DSceneOutputWnd::SetHSIIntensityScale(float fValue)
{
    m_lpParentScene->m_EffectParams.fIntensityScale = fValue;
    return m_RenderManager.SetHSIIntensityScale(fValue);
}

int KG3DSceneOutputWnd::SetHSIIntensityBias(float fValue)
{
    m_lpParentScene->m_EffectParams.fIntensityBias = fValue;
    return m_RenderManager.SetHSIIntensityBias(fValue);
}

int KG3DSceneOutputWnd::SetCMYK_KC(float k)
{
    m_lpParentScene->m_EffectParams.fKC = k;
    return m_RenderManager.SetCMYK_KC(k);
}

int KG3DSceneOutputWnd::SetCMYK_KM(float k)
{
    m_lpParentScene->m_EffectParams.fKM = k;
    return m_RenderManager.SetCMYK_KM(k);
}

int KG3DSceneOutputWnd::SetCMYK_KY(float k)
{
    m_lpParentScene->m_EffectParams.fKY = k;
    return m_RenderManager.SetCMYK_KY(k);
}

int KG3DSceneOutputWnd::SetShockWavePower(float fValue)
{
    m_lpParentScene->m_EffectParams.fShockWavePower = fValue;
    return m_RenderManager.SetShockWavePower(fValue);
}

int KG3DSceneOutputWnd::SetGodRaySampleNum(int nValue)
{
    m_lpParentScene->m_EffectParams.nGodRaySampleNum = nValue;
    return m_RenderManager.SetGodRaySampleNum(nValue);
}

int KG3DSceneOutputWnd::SetGodRayDensity(float fValue)
{
    m_lpParentScene->m_EffectParams.fGodRayDensity = fValue;
    return m_RenderManager.SetGodRayDensity(fValue);
}

int KG3DSceneOutputWnd::SetGodRayWeight(float fValue)
{
    m_lpParentScene->m_EffectParams.fGodRayWeight = fValue;
    return m_RenderManager.SetGodRayWeight(fValue);
}

int KG3DSceneOutputWnd::SetGodRayDecay(float fValue)
{
    m_lpParentScene->m_EffectParams.fGodRayDecay = fValue;
    return m_RenderManager.SetGodRayDecay(fValue);
}

int KG3DSceneOutputWnd::SetGodRayExposure(float fValue)
{
    m_lpParentScene->m_EffectParams.fGodRayExposure = fValue;
    return m_RenderManager.SetGodRayExposure(fValue);
}

int KG3DSceneOutputWnd::SetGodRayAngle(float fValue)
{
    m_lpParentScene->m_EffectParams.fLimitAngle = fValue;
    return m_RenderManager.SetGodRayLimitAngle(fValue);
}

int KG3DSceneOutputWnd::SetSSAOLevel(int nLevel)
{
    m_lpParentScene->m_EffectParams.nSSAOLevel = nLevel;
    return m_RenderManager.SetSSAOlevel(nLevel);
}

int KG3DSceneOutputWnd::SetSSAOSampRadius(float radius)
{
    m_lpParentScene->m_EffectParams.fSSAOSampleRadius = radius;
    return m_RenderManager.SetSSAOSampRadius(radius);
}

int KG3DSceneOutputWnd::SetSSAODarkFact(float dark)
{
    m_lpParentScene->m_EffectParams.fSSAODarkFact = dark;
    return m_RenderManager.SetSSAODarkFact(dark);
}

int KG3DSceneOutputWnd::SetSSAOBlurWeight(float w)
{
    m_lpParentScene->m_EffectParams.fBlurWeight = w;
    return m_RenderManager.SetSSAOBlurWeight(w);
}

int KG3DSceneOutputWnd::SetFogDensityV(float fValue)
{
    m_lpParentScene->m_EffectParams.fFogDensityV = fValue;
    return m_RenderManager.SetFogDensityV(fValue);
}

int KG3DSceneOutputWnd::SetFogSlopeThreshold(float fValue)
{
    m_lpParentScene->m_EffectParams.fSlopeThreshold = fValue;
    return m_RenderManager.SetFogSlopeThreshold(fValue);
}

int KG3DSceneOutputWnd::SetFogHeightFalloff(float fValue)
{
    m_lpParentScene->m_EffectParams.fHeightFalloff = fValue;
    return m_RenderManager.SetFogHeightFalloff(fValue);
}

int KG3DSceneOutputWnd::SetFogcolor(D3DXVECTOR4 vecColor)
{
    m_lpParentScene->m_EffectParams.vecFogcolor = vecColor;
    return m_RenderManager.SetFogcolor(vecColor);
}

int KG3DSceneOutputWnd::SetEnabledAreaShadow(BOOL bEnabled)
{
    m_lpParentScene->m_EffectParams.bEnableAreaShadow = bEnabled;
    return m_RenderManager.SetEnabledAreaShadow(bEnabled);
}

int KG3DSceneOutputWnd::SetShadowSoftness(float fValue)
{
    m_lpParentScene->m_EffectParams.fSoftness = fValue;
    return m_RenderManager.SetShadowSoftness(fValue);
}

int KG3DSceneOutputWnd::SetShadowJitter(float fValue)
{
    m_lpParentScene->m_EffectParams.fJitter = fValue;
    return m_RenderManager.SetShadowJitter(fValue);
}

int KG3DSceneOutputWnd::SetHDRBrightGate(float fValue)
{
    m_lpParentScene->m_EffectParams.fHDRBrightGate = fValue;
    return m_RenderManager.SetHDRBrightGate(fValue);
}

int KG3DSceneOutputWnd::SetHDRRectify(float fValue)
{
    m_lpParentScene->m_EffectParams.fHDRRectify = fValue;
    return m_RenderManager.SetHDRRectify(fValue);
}

int KG3DSceneOutputWnd::SetHDRGrayLevel(float fValue)
{
    m_lpParentScene->m_EffectParams.fHDRGrayLevel = fValue;
    return m_RenderManager.SetHDRGrayLevel(fValue);
}

int KG3DSceneOutputWnd::SetHDRStarType(int fValue)
{
    m_lpParentScene->m_EffectParams.nHDRStarType = fValue;
    return m_RenderManager.SetHDRStarType(fValue);
}

int KG3DSceneOutputWnd::SetHDREnableBlueShift(BOOL fValue)
{
    m_lpParentScene->m_EffectParams.bHDREnableBlueShift = fValue;
    return m_RenderManager.SetHDREnableBlueShift(fValue);
}

int KG3DSceneOutputWnd::SetHDRBloomScale(float fValue)
{
    m_lpParentScene->m_EffectParams.fHDRBloomScale = fValue;
    return m_RenderManager.SetHDRBloomScale(fValue);
}

int KG3DSceneOutputWnd::SetHDRStarScale(float fValue)
{
    m_lpParentScene->m_EffectParams.fHDRStarScale = fValue;
    return m_RenderManager.SetHDRStarScale(fValue);
}
