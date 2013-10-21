#include "StdAfx.h"
#include "KG3DSnapTool.h"
#include "KG3DCamera.h"
#include "KG3DModelSpeedTree.h"
//#include "../Image/KBmpFile24.h"
#include "kg3dscenesceneeditor.h"
#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern int g_nNumAllowTextureDynamicLoadFrame;
extern BOOL g_bRenderSoundBall;
BOOL SaveBufferToBmp(LPSTR lpFileName, PVOID pBitmap, int nPitch, int nWidth, int nHeight, bool bIs24Bit);
KG3DSnapTool::KG3DSnapTool(void)
{
	m_lpTargetSurface = NULL;
	m_lpDepthSurface = NULL;
	m_nSnapWidth = 0;
	m_nSnapHeight = 0;

	//m_lpSnapBuffer = NULL;
	//m_szFileName = "";

	m_lpTargetSave = NULL;
	m_lpDepthSave = NULL;
	m_lpTargetData = NULL;
	
	m_lpTexSurface = NULL;

	m_ZFar = 1000.0f;
	m_ZNear = 100.0f;
	m_fScale = 1.0f;

    m_pSceneEditor = NULL;
}

KG3DSnapTool::~KG3DSnapTool(void)
{
    ASSERT(!m_lpTargetSurface);
    ASSERT(!m_lpTargetData);
    ASSERT(!m_lpDepthSurface);
	ASSERT(!m_lpTexSurface);
    //ASSERT(!m_lpSnapBuffer);
}

HRESULT KG3DSnapTool::Init()
{
	HRESULT hr = S_OK;
	
    ASSERT(!m_lpDepthSurface);
    ASSERT(!m_lpTargetData);
    ASSERT(!m_lpTargetSurface);
	ASSERT(!m_lpTexSurface);

	hr = g_pd3dDevice->CreateRenderTarget(SNAP_UNIT_SIZE,SNAP_UNIT_SIZE,
		D3DFMT_A8R8G8B8,D3DMULTISAMPLE_NONE,0,
		TRUE,
		&m_lpTargetSurface,NULL);
	KGLOG_COM_PROCESS_ERROR(hr);

	hr = g_pd3dDevice->CreateOffscreenPlainSurface(SNAP_UNIT_SIZE,SNAP_UNIT_SIZE,D3DFMT_A8R8G8B8,
		D3DPOOL_SYSTEMMEM,&m_lpTargetData,NULL);
	KGLOG_COM_PROCESS_ERROR(hr);

    hr = g_pd3dDevice->CreateDepthStencilSurface(SNAP_UNIT_SIZE,
        SNAP_UNIT_SIZE,
        D3DFMT_D24S8,D3DMULTISAMPLE_NONE,0,TRUE,
        &m_lpDepthSurface,
        NULL);
    KGLOG_COM_PROCESS_ERROR(hr);

    m_EntityMgrSketch.Init(FALSE);
    
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSnapTool::UnInit()
{
	SAFE_RELEASE(m_lpTargetSurface);
	SAFE_RELEASE(m_lpTargetData);
    SAFE_RELEASE(m_lpDepthSurface);
	SAFE_RELEASE(m_lpTexSurface);
	//SAFE_DELETE_ARRAY(m_lpSnapBuffer);
    m_EntityMgrSketch.UnInit();

	return S_OK;
}

HRESULT KG3DSnapTool::OnLostDevice()
{
    SAFE_RELEASE(m_lpDepthSurface);
    SAFE_RELEASE(m_lpTargetData);
    SAFE_RELEASE(m_lpTargetSurface);
	//SAFE_RELEASE(m_lpTexSurface);
    m_EntityMgrSketch.OnLostDevice();

    return S_OK;
}

HRESULT KG3DSnapTool::OnResetDevice()
{
    HRESULT hrResult = E_FAIL;
    HRESULT hrRetCode = E_FAIL;

    ASSERT(!m_lpDepthSurface);
    ASSERT(!m_lpTargetData);
    ASSERT(!m_lpTargetSurface);
	ASSERT(!m_lpTexSurface);
    m_EntityMgrSketch.OnResetDevice();

    hrRetCode = g_pd3dDevice->CreateRenderTarget(
		SNAP_UNIT_SIZE,
		SNAP_UNIT_SIZE,
        D3DFMT_A8R8G8B8,
		D3DMULTISAMPLE_NONE,
		0,
        TRUE,
        &m_lpTargetSurface,
		NULL
	);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    hrRetCode = g_pd3dDevice->CreateOffscreenPlainSurface(
		SNAP_UNIT_SIZE,
		SNAP_UNIT_SIZE,
		D3DFMT_A8R8G8B8,
        D3DPOOL_SYSTEMMEM,
		&m_lpTargetData,
		NULL
	);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    hrRetCode = g_pd3dDevice->CreateDepthStencilSurface(
		SNAP_UNIT_SIZE,
        SNAP_UNIT_SIZE,
        D3DFMT_D24S8,
		D3DMULTISAMPLE_NONE,
		0,
		TRUE,
        &m_lpDepthSurface,
        NULL
	);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    hrResult = S_OK;
Exit0:
    return hrResult;
}


void KG3DSnapTool::SetScale(float fScale)
{
    m_fScale = fScale;
}

void KG3DSnapTool::SetSceneEditor(KG3DSceneSceneEditor *pSceneEditor)
{
    m_pSceneEditor = pSceneEditor;
}

HRESULT KG3DSnapTool::ComputeSnapSize(const AABBOX& BoxScene)
{
	HRESULT hr = E_FAIL;
    static float s_fCellSize = 100.0f;
    float fSceneLength = BoxScene.GetLength();
    float fSceneWidth = BoxScene.GetWidth();
    int nStrideX = static_cast<int>(fSceneLength / s_fCellSize);
    int nStrideZ = static_cast<int>(fSceneWidth / s_fCellSize);

    float fTotalSizeX = nStrideX * m_fScale;//Size of bmp final size in X
    float fTotalSizeZ = nStrideZ * m_fScale;//Size of bmp final size in Z

    int nCntX = static_cast<int>(ceil(fTotalSizeX / SNAP_UNIT_SIZE));
    int nCntZ = static_cast<int>(ceil(fTotalSizeZ / SNAP_UNIT_SIZE));

    float fStep = SNAP_UNIT_SIZE * s_fCellSize / m_fScale;
    m_nSnapWidth = nCntX * SNAP_UNIT_SIZE;
    m_nSnapHeight = nCntZ * SNAP_UNIT_SIZE;

    D3DXVECTOR3 vCameraDirection(0,1,0.01F);
    D3DXVECTOR3 vec3Center = BoxScene.GetCenter();
    D3DXVECTOR3 vec3Eye = vec3Center + vCameraDirection * 100000;
    KG3DCamera vCamera;

    vCamera.SetPositionLookAtUp(&vec3Eye, &vec3Center, &D3DXVECTOR3(0,1,0));

    m_ZNear = 10;
    m_ZFar = BoxScene.GetHeight() * 1.5f;

    D3DXVECTOR3 vRight = vCamera.GetCameraRight();
    D3DXVECTOR3 vUp = vCamera.GetCameraUp();

    D3DXVECTOR3 vec3EyeOrg = vec3Eye - 0.5f * fSceneLength * vRight - 0.5f * fSceneWidth * vUp;
    D3DXVECTOR3 vec3LookAtOrg = vec3Center - 0.5f * fSceneLength * vRight - 0.5f * fSceneWidth * vUp;
    m_listSetCameraInfo.clear();
    for(int i = 0; i < nCntZ; i++)
    {
        for(int j = 0; j < nCntX; j++)
        {
            StepCameraInfo StepInfo;
            StepInfo.nX = j;
            StepInfo.nZ = i;
            StepInfo.fWidth = fStep;
            StepInfo.fHeight = fStep;

            D3DXVECTOR3 vec3Trans = (j + 0.5f) * vRight * fStep + (i + 0.5f) * vUp * fStep;
            StepInfo.vLookAt = vec3Trans + vec3LookAtOrg;
            StepInfo.vEye = vec3Trans + vec3EyeOrg;

            m_listSetCameraInfo.push_back(StepInfo);
        }
    }

    //////////////////////////////////////////////////////////////////////////
   // m_szFileName = pFileName;
  /*  SAFE_DELETE_ARRAY(m_lpSnapBuffer);
    m_lpSnapBuffer = new DWORD[m_nSnapWidth * m_nSnapHeight];
    if(!m_lpSnapBuffer)
        return E_FAIL;
    ZeroMemory(m_lpSnapBuffer,sizeof(DWORD)*m_nSnapWidth * m_nSnapHeight);*/
	SAFE_RELEASE(m_lpTexSurface);
	hr = g_pd3dDevice->CreateOffscreenPlainSurface(
		m_nSnapWidth,
		m_nSnapHeight,
		D3DFMT_A8R8G8B8,
		D3DPOOL_SYSTEMMEM,
		&m_lpTexSurface,
		NULL
		);
	KGLOG_COM_PROCESS_ERROR(hr);
	hr = S_OK;
Exit0:
    return hr;
}

HRESULT KG3DSnapTool::BegainStep()
{
	HRESULT hr = S_OK;
	D3DSURFACE_DESC desc;

	KGLOG_PROCESS_ERROR(m_lpTargetSurface);
	KGLOG_PROCESS_ERROR(m_lpDepthSurface);
	KGLOG_PROCESS_ERROR(m_lpTexSurface);
	//KGLOG_PROCESS_ERROR(m_lpSnapBuffer);
	//////////////////////////////////////////////////////////////////////////
	hr = g_pd3dDevice->GetRenderTarget(0,&m_lpTargetSave);
	KGLOG_COM_PROCESS_ERROR(hr);
	m_lpTargetSave->Release();
	hr = g_pd3dDevice->GetDepthStencilSurface(&m_lpDepthSave);
	KGLOG_COM_PROCESS_ERROR(hr);
	m_lpDepthSave->Release();
	
	g_pd3dDevice->GetViewport(&m_viewPortSave);
	//////////////////////////////////////////////////////////////////////////
	hr = g_pd3dDevice->SetRenderTarget(0,m_lpTargetSurface);
	KGLOG_COM_PROCESS_ERROR(hr);

	hr = g_pd3dDevice->SetDepthStencilSurface(m_lpDepthSurface);
	KGLOG_COM_PROCESS_ERROR(hr);
	//////////////////////////////////////////////////////////////////////////
	m_lpTargetSurface->GetDesc(&desc);
	D3DVIEWPORT9 viewPort;
	viewPort.X = viewPort.Y = 0;
	viewPort.Width = desc.Width;
	viewPort.Height = desc.Height;
	viewPort.MinZ = 0.0F;
	viewPort.MaxZ = 1.0F;
	g_pd3dDevice->SetViewport(&viewPort);
	//////////////////////////////////////////////////////////////////////////
	hr = g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 
		0xFF808080, 1.0f, 0 );

	g_pd3dDevice->BeginScene();

    //g_bRenderSnap = TRUE;

	return S_OK;

Exit0:
	return E_FAIL;
}

HRESULT KG3DSnapTool::EndStep(int nX,int nZ)
{
	HRESULT hr = S_OK;
	BOOL bTargetLocked = FALSE;
	BOOL bTexLocked = FALSE;
	D3DLOCKED_RECT SrcRect,DestRect;
	//////////////////////////////////////////////////////////////////////////
	g_pd3dDevice->EndScene();

	//////////////////////////////////////////////////////////////////////////
	hr = g_pd3dDevice->GetRenderTargetData(m_lpTargetSurface,m_lpTargetData);
	KGLOG_COM_PROCESS_ERROR(hr);
	hr = m_lpTargetData->LockRect(&SrcRect,NULL,0);
	KGLOG_COM_PROCESS_ERROR(hr);
	bTargetLocked = TRUE;
	hr = m_lpTexSurface->LockRect(&DestRect,NULL,0);
	KGLOG_COM_PROCESS_ERROR(hr);
	bTexLocked = TRUE;

	int nSrcWidth  = SrcRect.Pitch / 4;
	DWORD* pSrcData = (DWORD*)SrcRect.pBits; 

	int nDestWidth = DestRect.Pitch / 4;
	_ASSERT(nDestWidth == m_nSnapWidth);
	DWORD* pDestData= (DWORD*)DestRect.pBits;

	{
		int ZZZ = m_nSnapHeight / SNAP_UNIT_SIZE - 1 - nZ;
		for (int z=0;z<SNAP_UNIT_SIZE;z++)
		{
			for (int x=0;x<SNAP_UNIT_SIZE;x++)
			{
				int nIndexSrc = z * nSrcWidth + x;
				int nIndexDest = (ZZZ*SNAP_UNIT_SIZE + z)* nDestWidth + (nX*SNAP_UNIT_SIZE+x);
				pDestData[nIndexDest] = pSrcData[nIndexSrc];
				//m_lpSnapBuffer[nIndexDest] = pSrcData[nIndexSrc];
			}
		}
	}

	hr = m_lpTargetData->UnlockRect();
	KGLOG_COM_PROCESS_ERROR(hr);
	bTargetLocked = FALSE;
	hr = m_lpTexSurface->UnlockRect();
	KGLOG_COM_PROCESS_ERROR(hr);
	bTexLocked = FALSE;
	//////////////////////////////////////////////////////////////////////////
	hr = g_pd3dDevice->SetRenderTarget(0,m_lpTargetSave);
	KGLOG_COM_PROCESS_ERROR(hr);
	hr = g_pd3dDevice->SetDepthStencilSurface(m_lpDepthSave);
	KGLOG_COM_PROCESS_ERROR(hr);

	g_pd3dDevice->SetViewport(&m_viewPortSave);
	
	return S_OK;
Exit0:
	if(bTargetLocked)
		m_lpTargetData->UnlockRect();
	if(bTexLocked)
		m_lpTexSurface->UnlockRect();
	return E_FAIL;
}


HRESULT KG3DSnapTool::GetCurrentCameraStep(KG3DCamera* pCamera,int& nX,int& nZ)
{
    if(m_listSetCameraInfo.size()==0)
        return E_FAIL;
    StepCameraInfo& vStepInfo = * m_listSetCameraInfo.begin();

    pCamera->SetPositionLookAtUp(&vStepInfo.vEye, &vStepInfo.vLookAt, &D3DXVECTOR3(0,1,0));
    pCamera->SetOrthogonal(vStepInfo.fWidth,vStepInfo.fHeight,m_ZNear,m_ZFar);

    nX = vStepInfo.nX;
    nZ = vStepInfo.nZ;

    m_listSetCameraInfo.pop_front();

    return S_OK;
}

HRESULT KG3DSnapTool::Render(const AABBOX& BoxScene,LPSTR pFileName)
{
    HRESULT hrResult = E_FAIL;
    HRESULT hrRetCode = E_FAIL;
    KG3DCamera Camera;
    int nX = 0, nZ = 0;
    
    KGLOG_PROCESS_ERROR(pFileName);
    KGLOG_PROCESS_ERROR(m_pSceneEditor);

    hrRetCode = ComputeSnapSize(BoxScene);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    while(SUCCEEDED(GetCurrentCameraStep(&Camera,nX,nZ)))
    {
        //sprintf(str,"Scene Snap Step Left: %d",pEditor->m_vSnapTool.m_listSetCameraInfo.size());
        //::SetWindowText(pWnd->m_hWnd,str);

        BegainStep();

        BOOL bSave3 = g_bRenderSoundBall;
        g_bRenderSoundBall = FALSE;
        int nWD = g_cEngineOption.nWaterDetail;
        g_cEngineOption.nWaterDetail = WD_LOWER;

        m_EntityMgrSketch.EnableCheckVisibleCount(FALSE);//不能有渲染计数，否则会渲染不出物件
   
        m_EntityMgrSketch.Clear();

        KG3DFrustum frustum;
        frustum.CreateByCamera(Camera);
        vector<KG3DFrustum> vecFrustum;
        vecFrustum.push_back(frustum);
		
        m_EntityMgrSketch.GetVisibleEntity(
            vecFrustum,
            m_pSceneEditor,
            Camera.GetPosition()
        );

        m_EntityMgrSketch.FrameMoveVisibleEntity();
	
        //m_EntityMgrSketch.EnableCheckVisibleCount(TRUE);
        //不渲染Info
        BOOL bRenderInfo = g_cEngineManager.GetRenderOption(RENDER_INFO);
        g_cEngineManager.SetRenderOption(RENDER_INFO, FALSE);

        //这里不用ZPrePass了，应该也可以吧
        Camera.SetCamera();

        {
            GraphicsStruct::RenderStateAlpha Alpha(0x20,FALSE,FALSE,D3DBLEND_SRCALPHA,D3DBLEND_INVSRCALPHA);
            GraphicsStruct::RenderState colorWrite(D3DRS_COLORWRITEENABLE ,0);
            GraphicsStruct::RenderState ZWrite(D3DRS_ZWRITEENABLE,TRUE);
            GraphicsStruct::RenderState lightEnable(D3DRS_LIGHTING,FALSE);
            m_EntityMgrSketch.RenderForDepthOnly(m_pSceneEditor, TRUE, TRUE, TRUE);
        }

        {
            GraphicsStruct::RenderState ZWrite(D3DRS_ZWRITEENABLE,FALSE);
            GraphicsStruct::RenderState ZCmp(D3DRS_ZFUNC,D3DCMP_EQUAL);
            GraphicsStruct::RenderState rsColor(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN 
                | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA);

            m_EntityMgrSketch.RenderForColorOnly(m_pSceneEditor, NULL);
            m_EntityMgrSketch.RenderWaterAndSFX(m_pSceneEditor);
        }
     
		g_bRenderSoundBall = bSave3;
        g_cEngineOption.nWaterDetail = nWD;
		
        g_cEngineManager.SetRenderOption(RENDER_INFO, bRenderInfo);
        _ASSERTE((g_cEngineManager.GetRenderOption(RENDER_INFO) && bRenderInfo)
            || (!g_cEngineManager.GetRenderOption(RENDER_INFO)&&!bRenderInfo));

        EndStep(nX,nZ);
    }

	{
		if(m_lpTexSurface)
		{
			hrRetCode = D3DXSaveSurfaceToFile(pFileName,D3DXIFF_BMP,m_lpTexSurface,NULL,NULL);
			if (FAILED(hrRetCode))
			{
				MessageBox(NULL,"保存缩略图失败！","提示",MB_ICONERROR);
			}
			SAFE_RELEASE(m_lpTexSurface);
			/*BOOL bSuccessed = SaveBufferToBmp(pFileName, m_lpSnapBuffer,
				m_nSnapWidth*4, m_nSnapWidth, m_nSnapHeight,false);
		
			if (!bSuccessed)
			{
				MessageBox(NULL,"保存缩略图失败！","提示",MB_ICONERROR);
			}*/

		}
		g_bRenderTree = TRUE;
		g_fSpeedTreeLeafScale = 1.0f;
	}

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KG3DSnapTool::ExportTerrainTexture(const AABBOX& BoxScene,KG3DSceneSceneEditor* pSceneEditor,LPSTR pFileName)
{
	HRESULT hrResult = E_FAIL;
	HRESULT hrRetCode = E_FAIL;
	KG3DCamera Camera;
	int nX = 0, nZ = 0;
	KGLOG_PROCESS_ERROR(pFileName);
	KGLOG_PROCESS_ERROR(pSceneEditor);

	hrRetCode = ComputeSnapSize(BoxScene);
	KGLOG_COM_PROCESS_ERROR(hrRetCode);

	while(SUCCEEDED(GetCurrentCameraStep(&Camera,nX,nZ)))
	{
		BegainStep();

		m_EntityMgrSketch.Clear();

		KG3DFrustum frustum;
		frustum.CreateByCamera(Camera);
		vector<KG3DFrustum> vecFrustum;
		vecFrustum.push_back(frustum);

		m_EntityMgrSketch.GetVisibleEntity(
			vecFrustum,
			pSceneEditor,
			Camera.GetPosition()
			);

		Camera.SetCamera();

		m_EntityMgrSketch.RenderTerrain_Texture();

		EndStep(nX,nZ);
	}
	if(m_lpTexSurface)
	{
		hrRetCode = D3DXSaveSurfaceToFile(pFileName,D3DXIFF_TGA,m_lpTexSurface,NULL,NULL);
		if (FAILED(hrRetCode))
		{
			MessageBox(NULL,"导出贴图失败！","提示",MB_ICONERROR);
		}
		else
		{
			MessageBox(NULL,"导出贴图成功！","提示",MB_OK);
		}
		SAFE_RELEASE(m_lpTexSurface);
	}
	hrResult = S_OK;
Exit0:
	return hrResult;
}

BOOL SaveBufferToBmp(LPSTR lpFileName, PVOID pBitmap, int nPitch, int nWidth, int nHeight, bool bIs24Bit)
{
	if(!pBitmap)
		return FALSE;

	BITMAPFILEHEADER	FileHeader;
	BITMAPINFOHEADER	InfoHeader;
	KSmartFile			pFile;
	DWORD				OffBits;
//	void*				Buffer;

	// byte per line % 4 must = 0
	int nBytesPerLine = nWidth * 3;
	if ((nBytesPerLine % 4) != 0)
		nBytesPerLine = nBytesPerLine + 4 - (nBytesPerLine % 4);

	// alloc buffer
	//if ((Buffer = malloc(nHeight * nBytesPerLine)) == NULL)
		//return FALSE;

	// offset in bytes
	OffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	// file header
	FileHeader.bfType          = 0x4d42; // "BM"
	FileHeader.bfSize          = nBytesPerLine * nHeight + OffBits;
	FileHeader.bfReserved1     = 0;
	FileHeader.bfReserved2     = 0;
	FileHeader.bfOffBits       = OffBits;

	// info header
	InfoHeader.biSize          = sizeof(BITMAPINFOHEADER);
	InfoHeader.biWidth         = nWidth;
	InfoHeader.biHeight        = nHeight;
	InfoHeader.biPlanes        = 1;
	InfoHeader.biBitCount      = 24;
	InfoHeader.biCompression   = 0;
	InfoHeader.biSizeImage     = 0;
	InfoHeader.biXPelsPerMeter = 0xb40;
	InfoHeader.biYPelsPerMeter = 0xb40;
	InfoHeader.biClrUsed       = 0;
	InfoHeader.biClrImportant  = 0;

	// crete the file
	if ((pFile = g_CreateFile(lpFileName)) == NULL)
	{
		//free(Buffer);
		return FALSE;
	}

	// write file head
	pFile->Write(&FileHeader, sizeof(FileHeader));

	// write info head
	pFile->Write(&InfoHeader, sizeof(InfoHeader));

	// encode bitmap
	LPBYTE lpSrc = (LPBYTE)pBitmap;

	lpSrc += (nHeight - 1) * nPitch;
	if(bIs24Bit)
	{
		for (int i = 0; i < nHeight; i++)
		{
			pFile->Write(lpSrc, nPitch);
			lpSrc -= nPitch;
		}
	}
	else
	{
		for (int i = 0; i < nHeight; i++)
		{
			for (int j = 0; j < nWidth; j++)
			{
				pFile->Write(lpSrc, sizeof(BYTE) * 3);
				lpSrc += 4;
			}
			lpSrc -= nPitch*2;
		}
	}
	// write bitmap bits data
	return TRUE;
}
