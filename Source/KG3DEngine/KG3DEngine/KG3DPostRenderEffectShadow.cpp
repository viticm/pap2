#include "StdAfx.h"
#include "KG3DPostRenderEffectShadow.h"
#include "KG3DSceneShadowMap.h"
#include "KG3DPostRenderEffectManager.h"
#include "KG3DGraphicsTool.h"
//#include "KG3DEnvEffLight.h"
#include "KG3DSceneSceneEditor.h"
// a simple function to convert degrees to radians
#define DegreeToRadian(fDegrees) ((3.14159265f/180.0f)*fDegrees)

extern int g_nDepthTexturePackingType;
int JITTER_SIZE = 32;
int JITTER_SAMPLES = 8;

namespace PostRender
{
	KG3DPostRenderEffectShadow::KG3DPostRenderEffectShadow( KG3DPostRenderEffectManager* pMgr )
        : KG3DPostRenderEffectBase(pMgr)
        , m_dwShadowmapSize(1024)
        , m_lpShadowMap(NULL)
        , m_lpEffect(NULL)
        , m_lpDepthTexture(NULL)
        , m_lpDepthSurface(NULL)
		, m_lpJitterTexture(NULL)
		, m_pBlurTexture(NULL)
		, m_pSMColorSurface(NULL)
		, m_pTempDepthSurface(NULL)
		, m_bJustResetEnable(FALSE)
		, m_dwTargetWidth(0)
		, m_dwTargetHeight(0)
	{
		m_fSoftness = 2.0f; 
		m_fJitter = 1.0f;
		m_lpParentScene = NULL;
	}

    KG3DPostRenderEffectShadow::~KG3DPostRenderEffectShadow()
    {
        HRESULT hr = E_FAIL;

        SAFE_DELETE(m_lpShadowMap);
        SAFE_RELEASE(m_lpEffect);

        hr = UnInitTextures();
        KGLOG_COM_CHECK_ERROR(hr);

        hr = UnInitJitter();
        KGLOG_COM_CHECK_ERROR(hr);
    }


    HRESULT KG3DPostRenderEffectShadow::Enable(BOOL bEnable)
    {
        HRESULT hResult  = E_FAIL;
        HRESULT hRetCode = E_FAIL;
        BOOL bInitTextures = FALSE;
        BOOL bInitJitter = FALSE;

        if (bEnable)
        {
            if (!m_lpShadowMap)
                m_lpShadowMap = new KG3DSceneShadowMap;

			KG3DSceneEntityMgr* pEntityMgr = NULL;
			if (m_lpParentScene)
			{
				pEntityMgr = &m_lpParentScene->m_EntityMgr;
			}

            if (!m_lpShadowMap->m_bInit)
			{
                hRetCode = m_lpShadowMap->Init(m_dwShadowmapSize,pEntityMgr);
			    KG_COM_PROCESS_ERROR(hRetCode);
			}

            if (!m_lpEffect)
                KG_COM_PROCESS_ERROR(ReloadFX());
            /*
            D3DXVECTOR4 UV[9];
            float x = 1.0F/m_dwShadowmapSize * 0.5F;
            float y = 1.0F/m_dwShadowmapSize * 0.5F;

            UV[0] = D3DXVECTOR4( 0, 0,0,0);
            UV[1] = D3DXVECTOR4(-x, 0,0,0);
            UV[2] = D3DXVECTOR4( x, 0,0,0);
            UV[3] = D3DXVECTOR4( 0,-y,0,0);
            UV[4] = D3DXVECTOR4( 0, y,0,0);
            UV[5] = D3DXVECTOR4(-x, y,0,0);
            UV[6] = D3DXVECTOR4( x, y,0,0);
            UV[7] = D3DXVECTOR4( x,-y,0,0);
            UV[8] = D3DXVECTOR4( x,-y,0,0);

            m_lpEffect->SetVectorArray("g_avSampleOffsets", UV, 9);
			*/
			DWORD dwHeight = 0,dwWidth = 0;
			IEKG3DScene* pScene = g_cGraphicsTool.GetScene();
			if (pScene)
			{
				IEKG3DSceneOutputWnd* pOutputWindow = NULL;
				pScene->GetCurOutputWnd(&pOutputWindow);
				if (pOutputWindow)
				{
					pOutputWindow->GetViewPortWidthHeight(&dwWidth,&dwHeight);
				}
			}
			if (!m_lpDepthTexture || (dwWidth  != m_dwTargetWidth) ||
				(dwHeight != m_dwTargetHeight))
            {
				m_dwTargetWidth  = dwWidth;
				m_dwTargetHeight = dwHeight;
				hRetCode = InitTextures();
                KGLOG_COM_PROCESS_ERROR(hRetCode);

                bInitTextures = TRUE;
            }

			if (!m_lpJitterTexture)
			{
				hRetCode = InitJitter();
                KGLOG_COM_PROCESS_ERROR(hRetCode); /*May Cause Memory Leak When Failed Later£¬Will Fix it later!*/

                bInitJitter = TRUE;
			}
			m_bJustResetEnable = TRUE;
        }
        else
        {
            if (m_lpShadowMap)
            {
                hRetCode = m_lpShadowMap->UnInit();
                KGLOG_COM_PROCESS_ERROR(hRetCode);
            }

            SAFE_RELEASE(m_lpEffect);

            hRetCode = UnInitTextures();
            KGLOG_COM_PROCESS_ERROR(hRetCode);

			hRetCode = UnInitJitter();
            KGLOG_COM_PROCESS_ERROR(hRetCode);
        }

        m_bEnable = bEnable;

        hResult = S_OK;
Exit0:
        if (FAILED(hResult))
        {
            if (bInitJitter)
            {
                hRetCode = UnInitJitter();
                KGLOG_COM_CHECK_ERROR(hRetCode);
            }
            if (bInitTextures)
            {
                hRetCode = UnInitTextures();
                KGLOG_COM_CHECK_ERROR(hRetCode);
            }
        }
        return hResult;
    }

    HRESULT KG3DPostRenderEffectShadow::ProcessShadowMap()
    {
        KG3DCamera* pCamera = g_cGraphicsTool.GetCamera();

        if (!pCamera || !m_lpShadowMap || !m_lpEffect || !m_lpDepthSurface)
            return E_FAIL;

        BOOL bNeedRender = TRUE;
        D3DXVECTOR3 vPosition;
		float vfDepthBias[3];
		float vfBiasSlope[3];
		if (g_bClient)
		{
			KG3DScene* pScene = g_cGraphicsTool.GetCurScene();
			if (pScene)
			{
				pScene->GetFocus(&vPosition);
			}
			else
			{
				pCamera->GetLookAtPosition(&vPosition);
			}
		}
		else
		{
			 pCamera->GetLookAtPosition(&vPosition);
		}

        D3DXVECTOR3 vFront = pCamera->GetCameraFront();
        D3DXMATRIX matView_C,matProj_C;
        D3DXVECTOR3 vNormal(0,1,0);

        matView_C = pCamera->GetViewMatrix();
        matProj_C = pCamera->GetProjectionMatrix();

        D3DXMATRIX matA = matView_C * matProj_C;
        D3DXMATRIX matCameraVPInv;

		D3DXMatrixInverse(&matCameraVPInv,NULL,&matA);
        m_lpEffect->SetMatrix("matCameraVPInv",&matCameraVPInv);

        D3DXVECTOR3 V = vPosition - m_lpShadowMap->m_vPositionLast;
        float Len = D3DXVec3LengthSq(&V);
        if(Len<360000 && !m_bJustResetEnable)
            bNeedRender = FALSE;
        else
		{
            m_lpShadowMap->m_vPositionLast = vPosition;
			if (m_bJustResetEnable > 2)
				m_bJustResetEnable = FALSE;
			else if (m_bJustResetEnable)
				m_bJustResetEnable++;
		}


		//////////////////////////////////////////////////////////////////

        if (TRUE/*m_bUseCSM*/)
        {	
  
			KG3DScene* pCurScene = g_cGraphicsTool.GetCurScene();
			_ASSERTE(NULL != pCurScene);
			if (NULL != pCurScene)
			{
				//vNormal = g_cGraphicsTool.GetSunLight(*g_cGraphicsTool.GetCurScene()).GetDirection();
				vNormal = g_cGraphicsTool.GetSunLightParam(*pCurScene).dir;
			}
			
            D3DXVec3Normalize(&vNormal,&(-vNormal));

            D3DXMATRIX matVP[3];
            for (int i=0;i<3;i++)
            {
                if(i==0 || bNeedRender)
                {
				                  
					float fSzie = m_lpShadowMap->m_ShadowLevel[i].m_fSize;
                    float fLength = 20000;

                    D3DXVECTOR3 vC = vPosition + vNormal * fLength;
                    m_lpShadowMap->m_ShadowLevel[i].m_vCameraLight.SetPosition(vC);
                    m_lpShadowMap->m_ShadowLevel[i].m_vCameraLight.SetLookAtPosition(vPosition);
                    m_lpShadowMap->m_ShadowLevel[i].m_vCameraLight.SetUpDirection(D3DXVECTOR3(0,1,0));
                    m_lpShadowMap->m_ShadowLevel[i].m_vCameraLight.SetOrthogonal(fSzie,fSzie,100,fLength*1.5F);
                    m_lpShadowMap->m_ShadowLevel[i].m_vCameraLight.SetCamera();
                    D3DXMATRIX matView_L,matProj_L;
                    matView_L = m_lpShadowMap->m_ShadowLevel[i].m_vCameraLight.GetViewMatrix();
                    matProj_L = m_lpShadowMap->m_ShadowLevel[i].m_vCameraLight.GetProjectionMatrix();

                    matVP[i] = matView_L * matProj_L;
					
					
                    m_lpShadowMap->m_ShadowLevel[i].m_matVP = matVP[i];
                }
            }
		
			/*
			//////////////////////////////////////////////////////////////////
			//Compute PSSM matrix for each shadowmapd
			//////////////////////////////////////////////////////////////////
			
			//1st step: CalculateSplitDistances
			const int nSumSplit = 3;
			float fCameraNear = 0,fCameraFar = 0,fFovy = 0,fAspect = 0;
			pCamera->GetPerspective(&fFovy,&fAspect,&fCameraNear,&fCameraFar);
			fCameraFar = 20000;
			float fSplitDis[nSumSplit + 1];
			float fSplitSchemeLambda = 0.5;
			for (int i = 0;i < nSumSplit; i++)
			{
				
				float fIDM = i / (float)nSumSplit;
				float fLog = fCameraNear * powf((fCameraFar / fCameraNear),fIDM);
				float fUniform = fCameraNear + (fCameraFar - fCameraNear) * fIDM;
				fSplitDis[i] = fLog * fSplitSchemeLambda + fUniform * (1- fSplitSchemeLambda);
				
				//fSplitDis[i] = fCameraNear + (fCameraFar - fCameraNear) * i / (float)nSumSplit;
			}
			 fSplitDis[0] = fCameraNear;
			 fSplitDis[nSumSplit] = fCameraFar;

			//2nd step: CalculateLightForFrustum
			D3DXVECTOR3 pPoints[8];
			float fScale=1.1f;
			D3DXMATRIX matVP[3];
			for (int i = 0; i< nSumSplit; i++)
			{
				//Sub 1st step: CalculateFrustumCorners
				if(i != 2 || bNeedRender)
                {
					float fNear = fSplitDis[i];
					float fFar = fSplitDis[i + 1];
					D3DXVECTOR3 vZ = vPosition - pCamera->GetPosition();
					D3DXVec3Normalize(&vZ,&vZ);
		            
					D3DXVECTOR3 vUp;
					pCamera->GetUpDirection(&vUp);
					D3DXVECTOR3 vX;
					D3DXVec3Cross(&vX,&vUp,&vZ);
					D3DXVec3Normalize(&vX,&vX);

					D3DXVECTOR3 vY;
					D3DXVec3Cross(&vY,&vZ,&vX);


					float fNearPlaneHeight = tanf(fFovy * 0.5f) * fNear;
					float fNearPlaneWidth = fNearPlaneHeight * fAspect;

					float fFarPlaneHeight = tanf(fFovy * 0.5f) * fFar;
					float fFarPlaneWidth = fFarPlaneHeight * fAspect;

					D3DXVECTOR3 vNearPlaneCenter = pCamera->GetPosition() + vZ * fNear;
					D3DXVECTOR3 vFarPlaneCenter = pCamera->GetPosition() + vZ * fFar;

					pPoints[0] = D3DXVECTOR3(vNearPlaneCenter - vX*fNearPlaneWidth - vY*fNearPlaneHeight);
					pPoints[1] = D3DXVECTOR3(vNearPlaneCenter - vX*fNearPlaneWidth + vY*fNearPlaneHeight);
					pPoints[2] = D3DXVECTOR3(vNearPlaneCenter + vX*fNearPlaneWidth + vY*fNearPlaneHeight);
					pPoints[3] = D3DXVECTOR3(vNearPlaneCenter + vX*fNearPlaneWidth - vY*fNearPlaneHeight);

					pPoints[4] = D3DXVECTOR3(vFarPlaneCenter - vX*fFarPlaneWidth - vY*fFarPlaneHeight);
					pPoints[5] = D3DXVECTOR3(vFarPlaneCenter - vX*fFarPlaneWidth + vY*fFarPlaneHeight);
					pPoints[6] = D3DXVECTOR3(vFarPlaneCenter + vX*fFarPlaneWidth + vY*fFarPlaneHeight);
					pPoints[7] = D3DXVECTOR3(vFarPlaneCenter + vX*fFarPlaneWidth - vY*fFarPlaneHeight);

					// calculate center of points
					D3DXVECTOR3 vCenter(0,0,0);
					for(int j = 0;j < 8;j++) vCenter += pPoints[i];
					vCenter /= 8;

					// for each point
					for(int j = 0; j< 8;j++)
					{
						// scale by adding offset from center
						pPoints[j] += (pPoints[j]-vCenter) * (fScale-1);
					}
		  
					//Sub 2nd step: 
					// calculate standard view and projection matrices for light
					D3DXMATRIX matLightView,matLightProj;
					D3DXVECTOR3 vLightSource,vLightTarget;
					//float fLightFOV = 90.0f,fLightNear = 10.0f,fLightFarMax = 40000.0f;
					
					vNormal = g_cGraphicsTool.GetSunLight(*g_cGraphicsTool.GetCurScene()).GetDirection();
					D3DXVec3Normalize(&vNormal,&(-vNormal));
					float fLength = 10000;
					D3DXVECTOR3 vC = vCenter + vNormal * fLength;
					vLightSource = vC;
					vLightTarget = vCenter;
					// view matrix
					D3DXMatrixLookAtLH(&matLightView, &vLightSource, &vLightTarget, &D3DXVECTOR3(0,1,0));
					// projection matrix
					 //D3DXMatrixPerspectiveFovLH(&matLightProj, DegreeToRadian(fLightFOV), 1, fLightNear, fLightFarMax);
					D3DXMatrixOrthoLH(&matLightProj,14000,14000,10,15000);
					// Next we will find the min and max values of the current
					// frustum split in lights post-projection space
					// (where coordinate range is from -1.0 to 1.0)
					//
					float fMaxX=-FLT_MAX;
					float fMaxY=-FLT_MAX;
					float fMinX=FLT_MAX;
					float fMinY=FLT_MAX;
					float fMaxZ=-FLT_MAX;
					float fMinZ=FLT_MAX;

					D3DXMATRIX matLightViewProj=matLightView * matLightProj;

					// for each corner point
					for(int j=0;j<8;j++)
					{
						// transform point
						D3DXVECTOR4 vTransformed;
						D3DXVec3Transform(&vTransformed,&pPoints[j],&matLightViewProj);

						// project x and y
						vTransformed.x/=vTransformed.w;
						vTransformed.y/=vTransformed.w;
						vTransformed.z/=vTransformed.w;
						// find min and max values
						if(vTransformed.x>fMaxX) fMaxX=vTransformed.x;
						if(vTransformed.y>fMaxY) fMaxY=vTransformed.y;
						if(vTransformed.y<fMinY) fMinY=vTransformed.y;
						if(vTransformed.x<fMinX) fMinX=vTransformed.x;

						// find largest z distance
						if(vTransformed.z>fMaxZ) fMaxZ=vTransformed.z;
						if(vTransformed.z<fMinZ) fMinZ=vTransformed.z;
					}
					fMinZ = 0;

		       
					float fScaleX=2.0f/(fMaxX-fMinX);
					float fScaleY=2.0f/(fMaxY-fMinY);

					float fOffsetX=-0.5f*(fMaxX+fMinX)*fScaleX;
					float fOffsetY=-0.5f*(fMaxY+fMinY)*fScaleY;

					float fScaleZ = 1.0f / (fMaxZ - fMinZ);
					float fOffsetZ = -fMinZ * fScaleZ;

					D3DXMATRIX mCropView(  fScaleX,     0.0f,  0.0f,   0.0f,
										0.0f,  fScaleY,  0.0f,   0.0f,
										0.0f,     0.0f, fScaleZ,   0.0f,
									fOffsetX, fOffsetY, fOffsetZ,   1.0f );

					// multiply the projection matrix with it
					matLightProj *= mCropView;

				
					matVP[i] = matLightView * matLightProj;

					
					
					D3DXVECTOR3 CovertPoint[6] = 
					{
						D3DXVECTOR3(-1.0,-1.0,0.0)
						,D3DXVECTOR3(1.0,-1.0,0.0)
						,D3DXVECTOR3(-1.0,1.0,0.0)
						,D3DXVECTOR3(1.0,1.0,0.0)
						,D3DXVECTOR3(0,0,0)
						,D3DXVECTOR3(0,0,1)

					};
					D3DXMATRIX matInvLightProj;
					D3DXMatrixInverse(&matInvLightProj,NULL,&matVP[i]);
					for(int j=0;j<6;j++)
					{
						D3DXVECTOR4 vTransformed;
						D3DXVec3Transform(&vTransformed,&CovertPoint[j],&matInvLightProj);
						CovertPoint[j].x = vTransformed.x / vTransformed.w;
						CovertPoint[j].y = vTransformed.y / vTransformed.w;
						CovertPoint[j].z = vTransformed.z / vTransformed.w;
					}
					float fWidth = D3DXVec3Length(&(CovertPoint[0] - CovertPoint[1]));
					float fHeight = D3DXVec3Length(&(CovertPoint[1] - CovertPoint[3]));
					float fDis = D3DXVec3Length(&(CovertPoint[4] - CovertPoint[5]));
					D3DXVECTOR3 vecDir;
					D3DXVec3Normalize(&vecDir,&(CovertPoint[4] - CovertPoint[5]));
					D3DXVECTOR3 vecPos = CovertPoint[4] + vecDir * 1000;

					m_lpShadowMap->m_ShadowLevel[i].m_vCameraLight.SetUpDirection(D3DXVECTOR3(0,1,0));
					m_lpShadowMap->m_ShadowLevel[i].m_vCameraLight.SetPosition(vecPos);
					m_lpShadowMap->m_ShadowLevel[i].m_vCameraLight.SetLookAtPosition(CovertPoint[5]);
					m_lpShadowMap->m_ShadowLevel[i].m_vCameraLight.SetOrthogonal(fWidth,fHeight,100,100 + fDis);
					m_lpShadowMap->m_ShadowLevel[i].m_vCameraLight.SetCamera();

					D3DXMATRIX matView_L,matProj_L;
					matView_L = m_lpShadowMap->m_ShadowLevel[i].m_vCameraLight.GetViewMatrix();
					matProj_L = m_lpShadowMap->m_ShadowLevel[i].m_vCameraLight.GetProjectionMatrix();

					matVP[i] = matView_L * matProj_L;
					m_lpShadowMap->m_ShadowLevel[i].m_matVP = matVP[i];
				}
			}
			*/
            m_lpEffect->SetMatrix("matLightVP1",&matVP[0]);
            m_lpEffect->SetMatrix("matLightVP2",&matVP[1]);
            m_lpEffect->SetMatrix("matLightVP3",&matVP[2]);	

           	vfDepthBias[0] = 0.0002f;
			vfDepthBias[1] = 0.0005f;
			vfDepthBias[2] = 0.0008f;
			vfBiasSlope[0] = 1.0f;
			vfBiasSlope[1] = 2.0f;
			vfBiasSlope[2] = 4.0f;

            for (int i=0;i<3;i++)
            {
                if(i == 0 || bNeedRender)
                {
					//if(NULL != g_pd3dDevice)
					//{
					//	g_pd3dDevice->SetTransform(D3DTS_VIEW,&matLV[i]);
					//	g_pd3dDevice->SetTransform(D3DTS_PROJECTION,&matLP[i]);
					//} 
					GraphicsStruct::RenderState depthBias(D3DRS_DEPTHBIAS,*(DWORD*)&vfDepthBias[i]);
					GraphicsStruct::RenderState biasSlope(D3DRS_SLOPESCALEDEPTHBIAS,*(DWORD*)&vfBiasSlope[i]);
					m_lpShadowMap->m_ShadowLevel[i].ProcessShadowMap(&m_lpShadowMap->m_ShadowLevel[i].m_vCameraLight,
                        m_lpShadowMap->m_vPositionLast,
                        g_cGraphicsTool.GetCurScene()->GetSceneSceneEditor(),
                        m_lpShadowMap,
                        //m_lpDepthSurface);
						m_pSMColorSurface);
                }
            }
        }

        return S_OK;
    }

    HRESULT KG3DPostRenderEffectShadow::ProcessPostRenderShadow()
    {
        HRESULT hResult  = E_FAIL;
        HRESULT hRetCode = E_FAIL;
        LPDIRECT3DSURFACE9 pDepthSave  = NULL;
        LPDIRECT3DSURFACE9 pTargetSave = NULL;
        D3DVIEWPORT9 ViewportSave;
        D3DVIEWPORT9 Viewport;
        BOOL bSetRenderTarget = FALSE;
        BOOL bSetDepthStencilSurface = FALSE;
        BOOL bSetViewport = FALSE;

        KG_PROCESS_ERROR(m_lpShadowMap);
        KG_PROCESS_ERROR(m_lpEffect);

        hRetCode = g_pd3dDevice->GetDepthStencilSurface(&pDepthSave);
        KGLOG_COM_PROCESS_ERROR(hRetCode);

        hRetCode = g_pd3dDevice->GetRenderTarget(0, &pTargetSave);
        KGLOG_COM_PROCESS_ERROR(hRetCode);

        hRetCode = g_pd3dDevice->GetViewport(&ViewportSave);
        KGLOG_COM_PROCESS_ERROR(hRetCode);

		
		//hRetCode = g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.f, 0);
        //KGLOG_COM_PROCESS_ERROR(hRetCode);
 
        Viewport.X = 0;
        Viewport.Y = 0;
        Viewport.Height = m_dwTargetHeight;
        Viewport.Width = m_dwTargetWidth;
        Viewport.MaxZ = 1;
        Viewport.MinZ = 0;

        hRetCode = g_pd3dDevice->SetRenderTarget(0, m_lpDepthSurface);
        KGLOG_COM_PROCESS_ERROR(hRetCode);

        bSetRenderTarget = TRUE;

        hRetCode = g_pd3dDevice->SetDepthStencilSurface(m_pTempDepthSurface);
        KGLOG_COM_PROCESS_ERROR(hRetCode);

        bSetDepthStencilSurface = TRUE;

        hRetCode = g_pd3dDevice->SetViewport(&Viewport);
        KGLOG_COM_PROCESS_ERROR(hRetCode);

        bSetViewport = TRUE;

		hRetCode = g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.f, 0);
		KGLOG_COM_PROCESS_ERROR(hRetCode);
        {
            GraphicsStruct::RenderState cull(D3DRS_CULLMODE,D3DCULL_NONE);
            GraphicsStruct::RenderState fog(D3DRS_FOGENABLE,FALSE);
            GraphicsStruct::RenderStateAlpha Alpha(0x0,FALSE,TRUE,D3DBLEND_SRCALPHA,D3DBLEND_INVSRCALPHA);

            m_lpEffect->SetTexture("tMainDepth", m_pPostRenderEffectMgr->GetDepthTexture());
            m_lpEffect->SetTexture("tMainColor", m_pPostRenderEffectMgr->GetRenderTexture());
            m_lpEffect->SetTexture("tShadowmap1",m_lpShadowMap->m_ShadowLevel[0].m_lpSMShadowMap);
            m_lpEffect->SetTexture("tShadowmap2",m_lpShadowMap->m_ShadowLevel[1].m_lpSMShadowMap);
            m_lpEffect->SetTexture("tShadowmap3",m_lpShadowMap->m_ShadowLevel[2].m_lpSMShadowMap);

			if (false)
			{
				D3DXSaveTextureToFile("e:\\test.bmp",D3DXIFF_BMP ,m_pPostRenderEffectMgr->GetDepthTexture(),NULL);
			}

			//set special texture matrix for shadow mapping
			float fOffsetX = 0.5f + (0.5f / (float)m_dwShadowmapSize);
			float fOffsetY = 0.5f + (0.5f / (float)m_dwShadowmapSize);
			unsigned int range = 1;            //note different scale in DX9!
			float fBias    = 0.0f;
			D3DXMATRIX texScaleBiasMat( 0.5f,     0.0f,     0.0f,         0.0f,
										0.0f,    -0.5f,     0.0f,         0.0f,
										0.0f,     0.0f,     (float)range, 0.0f,
										fOffsetX, fOffsetY, fBias,        1.0f );
			
            m_lpEffect->SetMatrix("matLightVP1",&(m_lpShadowMap->m_ShadowLevel[0].m_matVP * texScaleBiasMat));
            m_lpEffect->SetMatrix("matLightVP2",&(m_lpShadowMap->m_ShadowLevel[1].m_matVP * texScaleBiasMat));
            m_lpEffect->SetMatrix("matLightVP3",&(m_lpShadowMap->m_ShadowLevel[2].m_matVP * texScaleBiasMat));	

            m_lpEffect->SetValue(TEXT("bAreaShadow"), &PostRender::KG3DPostRenderEffectManager::m_sEffectParams.bEnableAreaShadow, sizeof(int));

			//set soft shadow params
			float softness[] = {0, 0, 0, PostRender::KG3DPostRenderEffectManager::m_sEffectParams.fSoftness / m_dwTargetWidth},
            jitter[] = {PostRender::KG3DPostRenderEffectManager::m_sEffectParams.fJitter / JITTER_SIZE, PostRender::KG3DPostRenderEffectManager::m_sEffectParams.fJitter / JITTER_SIZE, 0, 0};
			m_lpEffect->SetValue("FilterSize", softness, sizeof(float)*4);
			m_lpEffect->SetValue("JitterScale", jitter, sizeof(float)*4);
			float fOffsetU = 1.0,fOffsetV = 1.0;
			float fScalU = 1.0,fScalV = 1.0;
			m_pPostRenderEffectMgr->GetDepthTexCoordScale(&fScalU, &fScalV);
			
			//DWORD dwWidth = m_pPostRenderEffectMgr->GetWidth();
			//DWORD dwHeight = m_pPostRenderEffectMgr->GetHeight();
			fOffsetU = 0.5f / (float)m_dwTargetWidth;
			fOffsetV = 0.5f / (float)m_dwTargetHeight;
			m_lpEffect->SetValue("OffsetU",&fOffsetU,sizeof(float));
			m_lpEffect->SetValue("OffsetV",&fOffsetV,sizeof(float));
			m_lpEffect->SetValue("ScalU",&fScalU,sizeof(float));
			m_lpEffect->SetValue("ScalV",&fScalV,sizeof(float));
			//set jitter map as texture
			m_lpEffect->SetTexture("Jitter", m_lpJitterTexture);


            if (SUCCEEDED(m_lpEffect->SetTechnique("tec0")))
            {
                UINT uPass = 0;
                m_lpEffect->Begin(&uPass,0);
                m_lpEffect->BeginPass(0);

                //g_cGraphicsTool.DrawScreenRectNoChange(&D3DXVECTOR2(0,0),&D3DXVECTOR2(m_dwShadowmapSize*1.0F,m_dwShadowmapSize*1.0F),0,0xFFFFFFFF);
				g_cGraphicsTool.DrawScreenRectNoChange(&D3DXVECTOR2(0,0),&D3DXVECTOR2(m_dwTargetWidth * 1.0F,m_dwTargetHeight * 1.0F),0,0xFFFFFFFF);

                m_lpEffect->EndPass();
                m_lpEffect->End();
            }

            m_lpEffect->SetTexture("tMainDepth", NULL);
            m_lpEffect->SetTexture("tMainColor", NULL);
            m_lpEffect->SetTexture("tShadowmap1", NULL);
            m_lpEffect->SetTexture("tShadowmap2", NULL);
            m_lpEffect->SetTexture("tShadowmap3",NULL);
			m_lpEffect->SetTexture("Jitter", NULL);

			if (false)
			{
				D3DXSaveTextureToFile("e:\\test.bmp",D3DXIFF_BMP ,m_lpDepthTexture,NULL);
			}
        }
		if (g_cEngineOption.ModelShadowType >= EM_MODEL_SHADOW_TYPE_SUPER_HIGH)
        {
			D3DXVECTOR2 vSampleOffsets[15];
			float fSampleWeights[15];

			LPDIRECT3DSURFACE9 pBlurSurface = NULL;
			m_pBlurTexture->GetSurfaceLevel(0,&pBlurSurface);
			hRetCode = g_pd3dDevice->SetRenderTarget(0, pBlurSurface);
			hRetCode = g_pd3dDevice->SetViewport(&Viewport);
			hRetCode = m_lpEffect->SetTexture("tShadowMask", m_lpDepthTexture);
			KG_COM_PROCESS_ERROR(hRetCode);

			//float fPixWidth = 1.0f / m_dwShadowmapSize;
			//int k = 0;
			//for (int i = -1; i <= 1; i++)
			//{
			//	for (int j = -1; j <= 1; j++)
			//	{
			//		vSampleOffsets[k].x = i * fPixWidth;
			//		vSampleOffsets[k].y = j * fPixWidth;
			//		k++;
			//	}
			//}

			GetGaussianOffsets(true,D3DXVECTOR2(1.0f / m_dwTargetWidth, 1.0f / m_dwTargetHeight),vSampleOffsets,fSampleWeights);
			m_lpEffect->SetValue("g_vSampleOffsets",vSampleOffsets,sizeof(D3DXVECTOR2) * 15);
			m_lpEffect->SetValue("g_fSampleWeights",fSampleWeights,sizeof(float) * 15);
			{
				//GraphicsStruct::RenderStateAlpha Alpha(0x0,FALSE,TRUE,D3DBLEND_SRCALPHA,D3DBLEND_INVSRCALPHA);

				hRetCode = g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, 0xffffffff, 1.0f, 0);
				KG_COM_PROCESS_ERROR(hRetCode);

				if (SUCCEEDED(m_lpEffect->SetTechnique("tec0")))
				{
					UINT uPass = 0;
					m_lpEffect->Begin(&uPass,0);
					m_lpEffect->BeginPass(1);

					//g_cGraphicsTool.DrawScreenRectNoChange(&D3DXVECTOR2(0,0),&D3DXVECTOR2(m_dwShadowmapSize*1.0F,m_dwShadowmapSize*1.0F),0,0xFFFFFFFF);
					g_cGraphicsTool.DrawScreenRectNoChange(&D3DXVECTOR2(0,0),&D3DXVECTOR2(m_dwTargetWidth * 1.0F,m_dwTargetHeight * 1.0F),0,0xFFFFFFFF);

					m_lpEffect->EndPass();
					m_lpEffect->End();
				}

			}
			hRetCode = g_pd3dDevice->SetRenderTarget(0, m_lpDepthSurface);
			hRetCode = g_pd3dDevice->SetViewport(&Viewport);
			hRetCode = m_lpEffect->SetTexture("tShadowMask",m_pBlurTexture);
			GetGaussianOffsets(false,D3DXVECTOR2(1.0f / m_dwTargetWidth,1.0f / m_dwTargetHeight),vSampleOffsets,fSampleWeights);
			m_lpEffect->SetValue("g_vSampleOffsets",vSampleOffsets,sizeof(D3DXVECTOR2) * 15);
			m_lpEffect->SetValue("g_fSampleWeights",fSampleWeights,sizeof(float) * 15);

			hRetCode = g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, 0xffffffff, 1.0f, 0);
			KG_COM_PROCESS_ERROR(hRetCode);

			if (SUCCEEDED(m_lpEffect->SetTechnique("tec0")))
			{
				UINT uPass = 0;
				m_lpEffect->Begin(&uPass,0);
				m_lpEffect->BeginPass(2);

				//g_cGraphicsTool.DrawScreenRectNoChange(&D3DXVECTOR2(0,0),&D3DXVECTOR2(m_dwShadowmapSize*1.0F,m_dwShadowmapSize*1.0F),0,0xFFFFFFFF);
				g_cGraphicsTool.DrawScreenRectNoChange(&D3DXVECTOR2(0,0),&D3DXVECTOR2(m_dwTargetWidth * 1.0F,m_dwTargetHeight * 1.0F),0,0xFFFFFFFF);

				m_lpEffect->EndPass();
				m_lpEffect->End();
			}

			SAFE_RELEASE(pBlurSurface);
			m_lpEffect->SetTexture("SamplerShadowMask", NULL);
		}
        hResult = S_OK;
Exit0:
        if (bSetRenderTarget)
        {
            hRetCode = g_pd3dDevice->SetRenderTarget(0, pTargetSave);
            KGLOG_COM_CHECK_ERROR(hRetCode);
        }
        SAFE_RELEASE(pTargetSave);

        if (bSetDepthStencilSurface)
        {
            hRetCode = g_pd3dDevice->SetDepthStencilSurface(pDepthSave);
            KGLOG_COM_CHECK_ERROR(hRetCode);
        }
        SAFE_RELEASE(pDepthSave);

        if (bSetViewport)
        {
            hRetCode = g_pd3dDevice->SetViewport(&ViewportSave);
            KGLOG_COM_CHECK_ERROR(hRetCode);
        }

        return hResult;
    }

    HRESULT KG3DPostRenderEffectShadow::ApplyEffect(KG3DScene* pScene)
    {
        if (FAILED(ProcessShadowMap()))
            return E_FAIL;

        if (FAILED(ProcessPostRenderShadow()))
            return E_FAIL;

        return S_OK;
    }

    HRESULT KG3DPostRenderEffectShadow::ReloadFX()
    {
        SAFE_RELEASE(m_lpEffect);

		LPCTSTR strPackType = NULL;
		switch (g_nDepthTexturePackingType)
		{
		case 0:
			strPackType = _T("0");
			break;
		case 1:
			strPackType = _T("1");
			break;
		default:
			strPackType = _T("0");
			break;
		}
		D3DXMACRO macroPackType[] = {
			{_T("PACKTYPE"), strPackType},	
			{NULL, NULL},
		};
		
        return KG3DD3DXCreateEffectFromFile(
            g_pd3dDevice, 
            SHADOW_FXFILE_PATH, 
            macroPackType, 
            NULL, 
            0, 
            NULL, 
            &m_lpEffect, 
            NULL);
    }

    HRESULT KG3DPostRenderEffectShadow::InitTextures()
    {
        HRESULT hr = S_OK;
		
        ASSERT(m_dwShadowmapSize > 0);
		ASSERT(m_dwTargetWidth > 0 && m_dwTargetHeight > 0);

		SAFE_RELEASE(m_lpDepthSurface);
		SAFE_RELEASE(m_lpDepthTexture);
		SAFE_RELEASE(m_pBlurTexture);
		SAFE_RELEASE(m_pTempDepthSurface);
		SAFE_RELEASE(m_pSMColorSurface);

        ASSERT(m_lpDepthTexture == NULL);
        ASSERT(m_lpDepthSurface == NULL);
		ASSERT(m_pBlurTexture == NULL);
		ASSERT(m_pSMColorSurface == NULL);
		ASSERT(m_pTempDepthSurface == NULL);


        hr = g_pd3dDevice->CreateTexture(m_dwTargetWidth, m_dwTargetHeight, 1, 
            D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_lpDepthTexture, NULL);
        KGLOG_COM_PROCESS_ERROR(hr);

        hr = m_lpDepthTexture->GetSurfaceLevel(0, &m_lpDepthSurface);
        KGLOG_COM_PROCESS_ERROR(hr);

		hr = g_pd3dDevice->CreateTexture(m_dwTargetWidth, m_dwTargetHeight, 1, 
			D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_pBlurTexture, NULL);
		KGLOG_COM_PROCESS_ERROR(hr);


		hr = g_pd3dDevice->CreateDepthStencilSurface(m_dwTargetWidth, m_dwTargetHeight, D3DFMT_D24S8, 
			D3DMULTISAMPLE_NONE , 0, TRUE, &m_pTempDepthSurface, NULL);
		KGLOG_COM_PROCESS_ERROR(hr);

		hr = g_pd3dDevice->CreateRenderTarget(m_dwShadowmapSize, m_dwShadowmapSize,D3DFMT_A8R8G8B8, D3DMULTISAMPLE_NONE, 
			0, FALSE, &m_pSMColorSurface, NULL);
		KGLOG_COM_PROCESS_ERROR(hr);

Exit0:
        if (FAILED(hr))
        {
            UnInitTextures();
        }
        return hr;
    }

    HRESULT KG3DPostRenderEffectShadow::UnInitTextures()
    {
        HRESULT hr = S_OK;

        SAFE_RELEASE(m_lpDepthSurface);
        SAFE_RELEASE(m_lpDepthTexture);
		SAFE_RELEASE(m_pBlurTexture);
		SAFE_RELEASE(m_pTempDepthSurface);
		SAFE_RELEASE(m_pSMColorSurface);
        return hr;
    }

    HRESULT KG3DPostRenderEffectShadow::OnLostDevice()
    {
        HRESULT hr = E_FAIL;
        HRESULT hrResult = E_FAIL;

        if (m_lpEffect)
        {
            hr = m_lpEffect->OnLostDevice();
            KGLOG_COM_PROCESS_ERROR(hr);
        }

        if (m_lpShadowMap)
        {
            hr = m_lpShadowMap->OnLostDevice();
            KGLOG_COM_PROCESS_ERROR(hr);
        }
		
		hr = UnInitJitter();
        KGLOG_COM_PROCESS_ERROR(hr);

        hr = UnInitTextures();
        KGLOG_COM_PROCESS_ERROR(hr);

        hrResult = S_OK;
Exit0:
        return hrResult;
    }

    HRESULT KG3DPostRenderEffectShadow::OnResetDevice()
    {
        HRESULT hr = E_FAIL;
        HRESULT hrResult = E_FAIL;
        BOOL bInitTextures = FALSE;
        BOOL bInitJitter = FALSE;

        if (m_bEnable)
        {
            hr = InitTextures();
            KGLOG_COM_PROCESS_ERROR(hr);
            
            bInitTextures = TRUE;

			hr = InitJitter();
		    KGLOG_COM_PROCESS_ERROR(hr);

            bInitJitter = TRUE;

			m_bJustResetEnable = TRUE;
        }

        if (m_lpShadowMap)
        {
            hr = m_lpShadowMap->OnResetDevice();
            KGLOG_COM_PROCESS_ERROR(hr);
        }

        if (m_lpEffect)
        {
            hr = m_lpEffect->OnResetDevice();
            KGLOG_COM_PROCESS_ERROR(hr);
        }

        hrResult = S_OK;
Exit0:
        if (FAILED(hrResult))
        {
            if (bInitJitter)
            {
                hr = UnInitJitter();
                KGLOG_COM_CHECK_ERROR(hr);
            }
            if (bInitTextures)
            {
                hr = UnInitTextures();
                KGLOG_COM_CHECK_ERROR(hr);
            }
        }
        return hrResult;
    }

	HRESULT KG3DPostRenderEffectShadow::InitJitter()
	{
		return g_cGraphicsTool.InitJitter(&m_lpJitterTexture);
	}

	HRESULT KG3DPostRenderEffectShadow::UnInitJitter()
	{
		HRESULT hr = S_OK;
		VERIFY_RELEASE(m_lpJitterTexture);
		return hr;	
	}

	float KG3DPostRenderEffectShadow::GetGaussianDistribution( float x, float y, float rho )
	{
		float g = 1.0f / sqrt( 2.0f * 3.141592654f * rho * rho );
		return g * exp( -(x * x + y * y) / (2 * rho * rho) );
	}

	void KG3DPostRenderEffectShadow::GetGaussianOffsets( bool bHorizontal, D3DXVECTOR2 vViewportTexelSize,
		D3DXVECTOR2* vSampleOffsets, float* fSampleWeights )
	{
		// Get the center texel offset and weight
		fSampleWeights[0] = 1.0f * GetGaussianDistribution( 0, 0, 2.0f );
		vSampleOffsets[0] = D3DXVECTOR2( 0.0f, 0.0f );
		// Get the offsets and weights for the remaining taps
		if( bHorizontal )
		{
			for( int i = 1; i < 15; i += 2 )
			{
				vSampleOffsets[i + 0] = D3DXVECTOR2( i * vViewportTexelSize.x, 0.0f );
				vSampleOffsets[i + 1] = D3DXVECTOR2( -i * vViewportTexelSize.x, 0.0f );
				fSampleWeights[i + 0] = 2.0f * GetGaussianDistribution( float(i + 0), 0.0f, 3.0f );
				fSampleWeights[i + 1] = 2.0f * GetGaussianDistribution( float(i + 1), 0.0f, 3.0f );
			}
		}
		else 
		{
			for( int i = 1; i < 15; i += 2 )
			{
				vSampleOffsets[i + 0] = D3DXVECTOR2( 0.0f, i * vViewportTexelSize.y );
				vSampleOffsets[i + 1] = D3DXVECTOR2( 0.0f, -i * vViewportTexelSize.y );
				fSampleWeights[i + 0] = 2.0f * GetGaussianDistribution( 0.0f, float(i + 0), 3.0f );
				fSampleWeights[i + 1] = 2.0f * GetGaussianDistribution( 0.0f, float(i + 1), 3.0f );
			}
		}
	}

}



