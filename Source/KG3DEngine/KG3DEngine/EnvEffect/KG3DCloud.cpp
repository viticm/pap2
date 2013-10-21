#include "stdafx.h"

///编译宏选择
#include "KG3DEnvEffCommonDef.h"

#include "KG3DCloud.h"
#include "./KG3DEngineManager.h"
#include "./KG3DGraphicsTool.h"
#include "./KG3DTextureTable.h"
#include "KG3DModelTable.h"
#include "KGINIFileHelpers.h"
#include "KG3DStructs.h"
#include "IEKG3DCloud.h"
#include "KGINIFileHelpers.h"
#include "KG3DDefaultShaderGroup.h"
#include "KG3DRenderGlobalState.h"

extern KG3DEngineEditorOption g_eEngineOption;

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace KG3D_ENVIRONMENT_EFFECT
{
	///静态成员
	//bool KG3DCloud::m_bIsRenderStarted = false;///用于记录StartRender和EndRender对
	const TCHAR strDefaultTexPath[] = _T("Data\\public\\Cloud\\cloud.tga");
	const TCHAR strDefaultMaskPath[] = _T("Data\\public\\Cloud\\mask.dds");

	const TCHAR	strCloudShaderPath[] = _T("data/public/shader/cloud.fx");
	//KG3DCloud::RenderStateStruct KG3DCloud::m_RenderStateStruct;

	struct CloudShaderParams 
	{
		float texCoordBias;
		float alphaFactor;
		D3DXVECTOR3 sunDir;    //Transforming sunLight.dir to the texture coordinate, it's z should be reversed
		D3DCOLORVALUE diffuse;
		D3DCOLORVALUE ambient;
        float fTexFactor;

		CloudShaderParams(){ZeroMemory(this, sizeof(*this));}
	};
	static CloudShaderParams s_cloudShaderParams;
	CloudShaderParams& KG3DCloud::GetShaderParams()
	{
		return s_cloudShaderParams;	//共享一个
	}
	/************************************************************************/
	// KG3DCloud定义开始
	/************************************************************************/
	KG3DCloud::KG3DCloud()
		:KG3DEnvEffect(0, KG3DTYPE_ENVEFF_SKYSYSTEM_CLOUD)
		,m_lpFace(NULL)
		,m_pColorTex(NULL)
		,m_pMaskTex(NULL)
		,m_State(0)
		,m_fPercent(0)
	{
		ZeroMemory( m_tszColorTexPathBuffer, MAX_PATH );
		ZeroMemory( m_tszMaskTexPathBuffer, MAX_PATH );

#if defined(DEBUG) | defined(_DEBUG)
		_ASSERTE(this->IsSubStateEnable() == FALSE);
		this->SubStateEnable(TRUE);
		_ASSERTE(this->IsSubStateEnable() == TRUE);
		this->SubStateEnable(FALSE);
		_ASSERTE(this->IsSubStateEnable() == FALSE);
#endif
	}

	KG3DCloud::~KG3DCloud()
	{
		SAFE_RELEASE(m_lpFace);

		SAFE_RELEASE(m_pColorTex);
		SAFE_RELEASE(m_pMaskTex);	
	}

	HRESULT KG3DCloud::FrameMove()
	{
		float fDelay = g_cGraphicsTool.GetTinyTime() /1000.0f;
		if (g_eEngineOption.bStopSpecialElements)
		{
			fDelay = 0;
		}
		m_UpdateDataStruct.m_matTexTrans._31 += m_UpdateDataStruct.m_fSpeedXcos * fDelay;
		m_UpdateDataStruct.m_matTexTrans._32 += m_UpdateDataStruct.m_fSpeedYcos * fDelay;
		m_UpdateDataStruct.m_matTexTrans._31 = fmod(m_UpdateDataStruct.m_matTexTrans._31, 10.0f);
		m_UpdateDataStruct.m_matTexTrans._32 = fmod(m_UpdateDataStruct.m_matTexTrans._32, 10.0f);
		
		return S_OK;
	}

#if defined(DEBUG) | defined(_DEBUG)
	static BOOL s_bRenderBegan = FALSE;
#endif
	HRESULT KG3DCloud::BeginRender(const D3DXMATRIX& matViewProj)
	{
		_ASSERTE(! s_bRenderBegan);

		LPD3DXEFFECT p = m_ShaderHolder.GetEffect();
		_ASSERTE(NULL != p);
		
		g_pd3dDevice->SetFVF(D3DFVF_Faces_Texture2);

		HRESULT hr = E_FAIL;
		hr = p->SetMatrix(_T("gWorldMatrix"), &D3DXMatrixGetIdentity());
		hr = p->SetMatrix(_T("gViewProjMatrix"), &matViewProj);

		const KG3DLightParam& lightParam = g_GetRenderGlobalState().GetSunLightParam();
		

		{
			CloudShaderParams& params = GetShaderParams();
			params.sunDir = lightParam.dir;
		}

		hr = p->SetValue(_T("gShaderParams"), &lightParam, sizeof(lightParam));

		hr = p->CommitChanges();		

#if defined(DEBUG) | defined(_DEBUG)
		s_bRenderBegan = TRUE;
#endif
		return S_OK;
	}

	HRESULT KG3DCloud::EndRender()
	{
		_ASSERTE(s_bRenderBegan);
		
#if defined(DEBUG) | defined(_DEBUG)
		s_bRenderBegan = FALSE;
#endif

		return S_OK;
	}

	HRESULT KG3DCloud::Render(const KG3DEnvEffRenderContext& renderContext)
	{
#if 0
		KG_PROCESS_ERROR(m_pColorTex && m_pMaskTex);
		{
			//首先计算整体透明度，如果在动态变化中，需要使用SubKey和DescStruct插值出透明度
			//如果透明度太低，那么连渲染都可以省掉了
			bool bUseThirdStage = false;
			float fAllAlpha = m_DescStruct.m_fVisibility;
			if(this->IsSubStateEnable())
			{
				MT_FLOAT_BLEND(m_DescStruct.m_fVisibility, m_SubKey.m_fVisibility, fAllAlpha, m_fPercent);	
			}

			KG_PROCESS_SUCCESS(fAllAlpha < 2 * FLT_EPSILON);

			//透明度在0-1之间的话，根据是否使用动态天气，用TFactor来控制整体透明度
			if (fAllAlpha > -FLT_EPSILON && fAllAlpha < 1- 2 * FLT_EPSILON)
			{
				bUseThirdStage = true;
			}

			if (bUseThirdStage)
			{
				DWORD dwAlpha = static_cast<DWORD>(fAllAlpha * 255.0f);
				g_pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(dwAlpha, dwAlpha, dwAlpha, dwAlpha));
				g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);
				g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
				g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
			}


			//设定基本的固定管线，其它状态在RenderState中统一设了。
			{
				//设定贴图Transform，令云动起来
				g_pd3dDevice->SetTransform(D3DTS_TEXTURE0, &m_UpdateDataStruct.m_matTexTrans);

				m_pColorTex->SetTexture(0);
				m_pMaskTex->SetTexture(1);

				_ASSERTE(m_lpFace);
				g_pd3dDevice->SetStreamSource(0, m_lpFace, 0, sizeof(VFormat::FACES_TEXTURE2));
			}

			//渲染两次，第一次，光渲染出颜色
			g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
			g_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
			g_pd3dDevice->SetRenderState(D3DRS_ALPHAREF, 0x00000000);

			g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

			//第二次，画ZBuffer，关闭颜色，用于光晕遮盖
			g_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
			g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);

			DWORD dwAlphaRef = static_cast<DWORD>(255.f * m_DescStruct.m_fAlphaRef); 
			g_pd3dDevice->SetRenderState(D3DRS_ALPHAREF, dwAlphaRef);

			GraphicsStruct::RenderState RSColorOut(D3DRS_COLORWRITEENABLE, FALSE);
			g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

			if (bUseThirdStage)//还原
			{
				g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
				g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
			}

		}
Exit1:
		return S_OK;
Exit0:
		return E_FAIL;
#else
		_ASSERTE(NULL != m_pColorTex && NULL != m_pMaskTex && _T("贴图不存在"));
		{
			//首先计算整体透明度，如果在动态变化中，需要使用SubKey和DescStruct插值出透明度
			//如果透明度太低，那么连渲染都可以省掉了

			CloudShaderParams& shaderParams = GetShaderParams();
			if(this->IsSubStateEnable())
			{
                shaderParams.fTexFactor = m_DescStruct.m_fTexFector;
				MT_FLOAT_BLEND(m_DescStruct.m_fVisibility, m_SubKey.m_fVisibility, shaderParams.alphaFactor, m_fPercent);
				
				
				std::pair<UINT, UINT> widthAndHeight = m_pColorTex->GetSize();
				shaderParams.texCoordBias = 1.f / (float)min(widthAndHeight.first, widthAndHeight.second);

				//sunDir2D在BeginRender中已经填充
				D3DCOLOR tempColor;
				TwoColorBlend(m_DescStruct.m_diffuseColor, m_SubKey.m_diffuseColor, tempColor, m_fPercent);
				shaderParams.diffuse = Color2Value(tempColor);
				TwoColorBlend(m_DescStruct.m_ambientColor, m_SubKey.m_ambientColor, tempColor, m_fPercent);
				shaderParams.ambient = Color2Value(tempColor);
			}
			else
			{
                shaderParams.fTexFactor = m_DescStruct.m_fTexFector;
				shaderParams.alphaFactor = m_DescStruct.m_fVisibility;

				std::pair<UINT, UINT> widthAndHeight = m_pColorTex->GetSize();
				FLOAT minScale = static_cast<FLOAT>(min(widthAndHeight.first, widthAndHeight.second));
				minScale = max(minScale, 1.f);
				shaderParams.texCoordBias = 1.f / (float)minScale;

				shaderParams.diffuse = Color2Value(m_DescStruct.m_diffuseColor);
				shaderParams.ambient = Color2Value(m_DescStruct.m_ambientColor);
			}
			KG_PROCESS_SUCCESS(shaderParams.alphaFactor < 2 * FLT_EPSILON);


			LPD3DXEFFECT pEffect = m_ShaderHolder.GetEffect();
			_ASSERTE(pEffect);

			HRESULT hr = E_FAIL;

			hr = pEffect->SetValue(_T("gShaderParams"), &shaderParams, sizeof(shaderParams));

			hr = pEffect->SetMatrix(_T("gTexTransMatrix"), &m_UpdateDataStruct.m_matTexTrans);
			hr = pEffect->SetTexture(_T("gTex0"), m_pColorTex->GetTexture());
			hr = pEffect->SetTexture(_T("gTex1"), m_pMaskTex->GetTexture());
			hr = pEffect->CommitChanges();

			_ASSERTE(NULL != m_lpFace);

			g_pd3dDevice->SetStreamSource(0, m_lpFace, 0, sizeof(VFormat::FACES_TEXTURE2));

			//渲染两次，第一次，光渲染出颜色
			if(m_DescStruct.m_bEnableControlColor)
			{
				hr = pEffect->SetTechnique(_T("advancedTech"));
			}
			else
			{
				hr = pEffect->SetTechnique(_T("defaultTech"));
			}
			KG_COM_PROCESS_ERROR(hr);
			hr = pEffect->Begin(NULL, 0);
			if(SUCCEEDED(hr))
			{
				hr = pEffect->BeginPass(0);
				if (SUCCEEDED(hr))
				{
					g_pd3dDevice->SetRenderState(D3DRS_ALPHAREF, 0);
					g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
					pEffect->EndPass();
				}

				static BOOL bRenderZ = TRUE;	//渲染了Z之后才能挡住太阳
				if (bRenderZ)
				{
					hr = pEffect->BeginPass(1);
					if (SUCCEEDED(hr))
					{
						DWORD dwAlphaRef = static_cast<DWORD>(255.f * m_DescStruct.m_fAlphaRef); 
						g_pd3dDevice->SetRenderState(D3DRS_ALPHAREF, dwAlphaRef);
						g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
						pEffect->EndPass();
					}
				}
				pEffect->End();
			}
		}
Exit1:
		return S_OK;
Exit0:
		return E_FAIL;
#endif
	}

//	
//	KG3DCloud::RenderState::RenderState(bool bUseToGadRay /* = false */)
//		:m_matTexSave(D3DTS_TEXTURE0)
//		,m_TexStageFlag(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2)
//		,m_FVFSetter(D3DFVF_Faces_Texture2)
//		,m_AlphaBlendEnable(D3DRS_ALPHABLENDENABLE, TRUE)
//	{
//		//设定各种Texture混合方式，这些不保存也不还原
//
//        if (!bUseToGadRay)
//        {
//		    g_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
//		    g_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
//        }
//		//g_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE);
//
//		g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1); 
//		g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
//		g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
//		g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
//
//		
//		g_pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
//		g_pd3dDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_CURRENT);
//		g_pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);
//		g_pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
//		g_pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
//
//		g_pd3dDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 1);
//		g_pd3dDevice->SetTextureStageState(2, D3DTSS_COLOROP, D3DTOP_DISABLE);
//
//#if 0	//如果云显示正常，就不用打开这些，如果云显示不正常，需要检查下面的这些状态
//		g_pd3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
//		g_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
//		g_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
//		g_pd3dDevice->SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
//		g_pd3dDevice->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
//		g_pd3dDevice->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
//#endif
//
//	}
//	KG3DCloud::RenderState::~RenderState()
//	{
//		g_pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
//		g_pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
//	}


	/*
HRESULT KG3DCloud::EndRender()
	{
		_ASSERTE(m_bIsRenderStarted);
		RenderStateStruct& RSStruct = m_RenderStateStruct;

		///－11还原Texture的Transform
		g_pd3dDevice->SetTransform(D3DTS_TEXTURE0, &RSStruct.m_matTexTransSave);
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, RSStruct.m_dwTexTransFlags);
		//－9坐标滚动	
		//－8还原所有灯光
		//－9还原FVF
		if( D3DFVF_Faces_Texture2 != RSStruct.m_dwFVF )
		{
			g_pd3dDevice->SetFVF( RSStruct.m_dwFVF );
		}
		//－8还原裁剪
		//－6还原灯光
		//－5还原Fog
		//－3还原AlphaBlend
		if( TRUE != RSStruct.m_dwAlphaBlendEnable )
		{
			g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, RSStruct.m_dwAlphaBlendEnable );
		}	

		//－1还原视点
		m_bIsRenderStarted = false;
		return S_OK;
	}
*/

	/*
	void KG3DCloud::OnReceiveMessage( const KG3DMessage& Message , IKG3DMessageSenderBase* pSender )
		{
			NULL;//暂时不需要做什么，因为贴图重载了也不是很影响
		}*/
	
	HRESULT KG3DCloud::SetColorTexture(LPCTSTR tszPath)
	{
		HRESULT hr = E_FAIL;
		KG3DTexture* pTexTemp = NULL;

		KG_PROCESS_ERROR( NULL != tszPath );
		
		hr = g_cTextureTable.LoadResourceFromFile(tszPath,
				0,
				TEXOP_COMMON | TEXOP_DISABLEDTX,
				(IKG3DResourceBase**)&pTexTemp
				);

		KGLOG_PROCESS_ERROR( SUCCEEDED(hr) && _T("云贴图载入错误"));
		_ASSERTE(NULL != pTexTemp);

		SAFE_RELEASE(m_pColorTex);
		m_pColorTex = pTexTemp;
		pTexTemp = NULL;

		///保存这个路径
		_tcscpy_s( m_tszColorTexPathBuffer, tszPath );
		m_DescStruct.m_tszColorMapPath = m_tszColorTexPathBuffer;///这是唯一能改变路径指针的方法

		//Exit1:
		return S_OK;
Exit0:
		SAFE_RELEASE(pTexTemp);
		return E_FAIL;
	}

	HRESULT KG3DCloud::SetMaskTexture( LPCTSTR tszPath )
	{
		HRESULT hr = E_FAIL;
		KG3DTexture* pTexTemp = NULL;

		KGLOG_PROCESS_ERROR( NULL != tszPath );

		hr = g_cTextureTable.LoadResourceFromFile(
			tszPath,
			0,
			TEXOP_COMMON | TEXOP_DISABLEDTX,
			(IKG3DResourceBase**)&pTexTemp
			);  

		KGLOG_PROCESS_ERROR( SUCCEEDED(hr) && _T("云的蒙版载入错误"));
		_ASSERTE( NULL != pTexTemp );

		SAFE_RELEASE(m_pMaskTex);
		m_pMaskTex = pTexTemp;
		pTexTemp = NULL;

		///保存这个路径
		_tcscpy_s( m_tszMaskTexPathBuffer, tszPath );
		m_DescStruct.m_tszMaskMapPath = m_tszMaskTexPathBuffer;///这是唯一能改变路径指针的方法

		//Exit1:
		return S_OK;
Exit0:
		SAFE_RELEASE(pTexTemp);
		return E_FAIL;
	}

	HRESULT KG3DCloud::SetDefaultTexture(DWORD dwType)
	{
		TCHAR tszTexturePath[MAX_PATH];
		HRESULT hr = E_FAIL;
		switch(dwType)
		{
		case _KG3DCLOUD_MD_TEX:
			_tcscpy_s( tszTexturePath, strDefaultTexPath );
			hr = SetColorTexture(tszTexturePath);
			break;
		case _KG3DCLOUD_MD_MASK:
			_tcscpy_s( tszTexturePath, strDefaultMaskPath );
			hr = SetMaskTexture(tszTexturePath);
			break;
		default:
			break;
		}
		return hr;
	}

	HRESULT KG3DCloud::SetTexture( DWORD dwType, LPCTSTR tszPath )
	{
		HRESULT hr = E_FAIL;
		switch(dwType)
		{
		case _KG3DCLOUD_MD_TEX:
			hr = SetColorTexture(tszPath);
			break;
		case _KG3DCLOUD_MD_MASK:
			hr = SetMaskTexture(tszPath);
			break;
		default:
			break;
		}
		return hr;
	}

	HRESULT KG3DCloud::RequestModification(const KG3DCloudDescStruct* pStruct)
	{
		HRESULT hr = E_FAIL;
		
		KG_PROCESS_ERROR(pStruct);
		DWORD dwMDOption = pStruct->m_dwMidificationOption;
		if (dwMDOption & KG3DCLOUD_MDO_MESH)
		{
			hr = ModifyMesh(pStruct);
			KGLOG_COM_PROCESS_ERROR(hr);
		}

		if (dwMDOption & KG3DCLOUD_MDO_MOVEMENT)
		{
			if (MT_FLOAT_UNEQUAL(pStruct->m_fHeight, m_DescStruct.m_fHeight))
			{
				this->SetHeightChangeHandled(FALSE);
			}
			hr = ModifyMovement(pStruct);
			KGLOG_COM_PROCESS_ERROR(hr);
		}

		if ((dwMDOption & KG3DCLOUD_MDO_COLORTEX) && pStruct->m_tszColorMapPath != NULL)
		{
			if (0 != _tcscmp(m_tszColorTexPathBuffer, pStruct->m_tszColorMapPath) )
			{
				hr = SetColorTexture(pStruct->m_tszColorMapPath);
				KGLOG_COM_PROCESS_ERROR(hr);
			}
		}
		if ((dwMDOption & KG3DCLOUD_MDO_MASKTEX) && pStruct->m_tszMaskMapPath != NULL)
		{
			if (0 != _tcscmp(m_tszMaskTexPathBuffer, pStruct->m_tszMaskMapPath))
			{
				hr = SetMaskTexture(pStruct->m_tszMaskMapPath);
				KGLOG_COM_PROCESS_ERROR(hr);
			}
		}

		if (dwMDOption & KG3DCLOUD_MDO_VISIBILITY)
		{
			KG_PROCESS_ERROR(pStruct->m_fVisibility > -FLT_EPSILON && pStruct->m_fVisibility < 1.0f+ FLT_EPSILON);
			m_DescStruct.m_fVisibility = pStruct->m_fVisibility;
		}

        if (dwMDOption & KG3DCLOUD_MDO_ALPHAREF)
        {
            KG_PROCESS_ERROR(pStruct->m_fAlphaRef > -FLT_EPSILON && pStruct->m_fAlphaRef < 1.0f+ FLT_EPSILON);
            m_DescStruct.m_fAlphaRef = pStruct->m_fAlphaRef;
        }

		if (dwMDOption & KG3DCLOUD_MDO_COLOR)
		{
			m_DescStruct.m_diffuseColor = pStruct->m_diffuseColor;
			m_DescStruct.m_ambientColor = pStruct->m_ambientColor;
		}

		if (dwMDOption & KG3DCLOUD_MDO_ENABLE_CONTRL_COLOR)
		{
			m_DescStruct.m_bEnableControlColor = pStruct->m_bEnableControlColor;
		}

        if (dwMDOption & KG3DCLOUD_MDO_TEXFACTOR)
        {
            m_DescStruct.m_fTexFector = pStruct->m_fTexFector;
        }

		return S_OK;
Exit0:
		return E_FAIL;
	}

	BOOL KG3DCloud::IsHeightChanged()
	{
		return IsHeightChangeHandled();
	}

	VOID KG3DCloud::OnHeightChangeHandled()
	{
		this->SetHeightChangeHandled(FALSE);
	}

	HRESULT KG3DCloud::ModifyMesh( const KG3DCloudDescStruct* pStruct /*= NULL*/ )
	{
		HRESULT hr = E_FAIL;;
        HRESULT hrResult = E_FAIL;
		void* pVertices = NULL;
        BOOL bLockFaceVertexBuffer = FALSE;
		
		if (NULL == m_lpFace)
		{
			hr = g_pd3dDevice->CreateVertexBuffer(4 * sizeof(VFormat::FACES_TEXTURE2), 0,
				D3DFVF_Faces_Texture2, D3DPOOL_MANAGED,  &m_lpFace, NULL);
			KGLOG_COM_PROCESS_ERROR(hr);
		}

		if (pStruct)
		{
			if (m_DescStruct.m_fHeight != pStruct->m_fHeight)
			{
				m_DescStruct.m_fHeight = pStruct->m_fHeight;
			}
			if (m_DescStruct.m_fSize != pStruct->m_fSize)
			{
				m_DescStruct.m_fSize = pStruct->m_fSize;
			}
			if (m_DescStruct.m_nUVRepeat != pStruct->m_nUVRepeat)
			{
				m_DescStruct.m_nUVRepeat = pStruct->m_nUVRepeat;
			}
		}
		
		float	fSize = (float)m_DescStruct.m_fSize;
		float	fHeight = m_DescStruct.m_fHeight;
		float	fUVRepeat = (float)m_DescStruct.m_nUVRepeat;
		///把放大也硬性写在VB中。
		VFormat::FACES_TEXTURE2 face[] = 
		{
			{ D3DXVECTOR3(-fSize, fHeight, fSize), 0.0f,0.0f, 0.0f,0.0f },
			{ D3DXVECTOR3( fSize, fHeight, fSize), fUVRepeat,0.0f, 1.0f,0.0f },
			{ D3DXVECTOR3(-fSize, fHeight,-fSize), 0.0f,fUVRepeat, 0.0f,1.0f },
			{ D3DXVECTOR3( fSize, fHeight,-fSize), fUVRepeat,fUVRepeat, 1.0f,1.0f }
		};

        hr = m_lpFace->Lock(0, sizeof(face), (void**)&pVertices, 0);
        KGLOG_COM_PROCESS_ERROR(hr);

        bLockFaceVertexBuffer = TRUE;

        memcpy(pVertices, face, sizeof(face));
		
        hrResult = S_OK;
Exit0:
        if (bLockFaceVertexBuffer)
        {
            hr = m_lpFace->Unlock();
            KGLOG_COM_CHECK_ERROR(hr);
        }
		return hrResult;
	}

	HRESULT KG3DCloud::ModifyMovement( const KG3DCloudDescStruct* pStruct /*= NULL*/ )
	{
		if (NULL != pStruct)
		{
			m_DescStruct.m_fSpeed = pStruct->m_fSpeed;		
			m_DescStruct.m_fAngel = pStruct->m_fAngel;
		}
		m_UpdateDataStruct.m_fSpeedXcos = m_DescStruct.m_fSpeed * cosf(D3DXToRadian(m_DescStruct.m_fAngel));
		m_UpdateDataStruct.m_fSpeedYcos = m_DescStruct.m_fSpeed * sinf(D3DXToRadian(m_DescStruct.m_fAngel));

		return S_OK;
	}
	const KG3DCloudDescStruct* KG3DCloud::GetStruct()
	{
		return &m_DescStruct;
	}

	HRESULT KG3DCloud::SaveMapSetting( IIniFile* pIniFile, LPCTSTR pSection )
	{
		_ASSERTE(pIniFile);
		_ASSERTE(pSection);
		pIniFile->WriteString(pSection, "TexCloud", m_tszColorTexPathBuffer);
		pIniFile->WriteString(pSection, "TexMask", m_tszMaskTexPathBuffer);
		pIniFile->WriteFloat(pSection, "Height", m_DescStruct.m_fHeight);    
		pIniFile->WriteFloat(pSection, "Speed", m_DescStruct.m_fSpeed);  
		pIniFile->WriteFloat(pSection, "Direction",m_DescStruct.m_fAngel);  
		pIniFile->WriteFloat(pSection, "Size", m_DescStruct.m_fSize);
		pIniFile->WriteInteger(pSection, "Repeat", m_DescStruct.m_nUVRepeat);
		pIniFile->WriteFloat(pSection, "Visibility", m_DescStruct.m_fVisibility);
        pIniFile->WriteFloat(pSection, "AlphaRef", m_DescStruct.m_fAlphaRef);
        pIniFile->WriteFloat(pSection, "TexFactor", m_DescStruct.m_fTexFector);
		KG_CUSTOM_HELPERS::SaveINIFileData(pIniFile, pSection, "Diffuse", m_DescStruct.m_diffuseColor);
		KG_CUSTOM_HELPERS::SaveINIFileData(pIniFile, pSection, "Ambient", m_DescStruct.m_ambientColor);
		KG_CUSTOM_HELPERS::SaveINIFileData(pIniFile, pSection, "EnableControlColor", m_DescStruct.m_bEnableControlColor);
		return S_OK;
	}

	HRESULT KG3DCloud::LoadMapSetting( IIniFile* pIniFile, LPCTSTR pSection )
	{
		struct ShaderChecker 
		{
			ShaderChecker()
			{
				LPD3DXEFFECT p = g_GetDefaultShaderGroup().GetDefaultShader(DEFST_CLOUD).GetEffect();
				_ASSERTE(NULL != p);

				BOOL bCheckOK = FALSE;
				D3DXPARAMETER_DESC desc;
				HRESULT hr = p->GetParameterDesc(_T("gShaderParams"), &desc);

				if (SUCCEEDED(hr))
				{
					bCheckOK = (desc.Bytes == sizeof(CloudShaderParams));
				}

				if(! bCheckOK)
				{
					KGLogPrintf(KGLOG_WARNING, _T("Cloud的Shader有问题，请检查Shader，渲染效果可能不正确"));
				}
			}
		};
		static ShaderChecker checkShader;

		if (! pIniFile)
		{
			KG_COM_PROCESS_ERROR((ModifyMesh(NULL)));
			{
				HRESULT hr  = ModifyMovement(NULL);
				KG_COM_PROCESS_ERROR(hr);
				hr = SetDefaultTexture(_KG3DCLOUD_MD_TEX);
				KG_COM_PROCESS_ERROR(hr);
				hr = SetDefaultTexture(_KG3DCLOUD_MD_MASK);				
				KG_COM_PROCESS_ERROR(hr);
			}
			return S_OK;
		}
		else
		{
			_ASSERTE(pSection);
			TCHAR	tszTempColorMapPath[MAX_PATH];
			TCHAR	tszTempMaskMapPath[MAX_PATH];
			KG3DCloudDescStruct tempStruct;
			KG_CUSTOM_HELPERS::LoadINIFileData(pIniFile, pSection, _T("TexCloud"), _T(""), tszTempColorMapPath, sizeof(tszTempColorMapPath));
			KG_CUSTOM_HELPERS::LoadINIFileData(pIniFile, pSection, _T("TexMask"), _T(""), tszTempMaskMapPath, sizeof(tszTempMaskMapPath));
			KG_CUSTOM_HELPERS::LoadINIFileData<FLOAT>(pIniFile, pSection, _T("Height"), tempStruct.m_fHeight, &tempStruct.m_fHeight);    
			KG_CUSTOM_HELPERS::LoadINIFileData<FLOAT>(pIniFile, pSection, _T("Speed"), tempStruct.m_fSpeed, &tempStruct.m_fSpeed);  
			KG_CUSTOM_HELPERS::LoadINIFileData<FLOAT>(pIniFile, pSection, _T("Direction"), tempStruct.m_fAngel, &tempStruct.m_fAngel);  
			KG_CUSTOM_HELPERS::LoadINIFileData<FLOAT>(pIniFile, pSection, _T("Size"), tempStruct.m_fSize, &tempStruct.m_fSize);
			KG_CUSTOM_HELPERS::LoadINIFileData(pIniFile, pSection, _T("Repeat"), tempStruct.m_nUVRepeat, &tempStruct.m_nUVRepeat);
			KG_CUSTOM_HELPERS::LoadINIFileData(pIniFile, pSection, _T("Visibility"), 1.0f, &tempStruct.m_fVisibility);
            //KG_CUSTOM_HELPERS::LoadINIFileData<FLOAT>(pIniFile, pSection, _T("AlphaRef"), tempStruct.m_fAlphaRef, &tempStruct.m_fAlphaRef);
			KG_CUSTOM_HELPERS::LoadINIFileData<D3DCOLOR>(pIniFile, pSection, _T("Diffuse"), tempStruct.m_diffuseColor, &tempStruct.m_diffuseColor);  
			KG_CUSTOM_HELPERS::LoadINIFileData<D3DCOLOR>(pIniFile, pSection, _T("Ambient"), tempStruct.m_ambientColor, &tempStruct.m_ambientColor); 
			KG_CUSTOM_HELPERS::LoadINIFileData(pIniFile, pSection, "EnableControlColor", FALSE, &tempStruct.m_bEnableControlColor);
            pIniFile->GetFloat(pSection, "TexFactor", 1.0, &tempStruct.m_fTexFector);
			if (0 != _tcscmp( tszTempColorMapPath, _T("")))
			{
				tempStruct.m_tszColorMapPath = tszTempColorMapPath;
			}
			if (0 != _tcscmp( tszTempMaskMapPath, _T("")))
			{
				tempStruct.m_tszMaskMapPath = tszTempMaskMapPath;
			}
			tempStruct.m_dwMidificationOption = KG3DCLOUD_MDO_ALL;
			return RequestModification(&tempStruct);
		}
Exit0:
		return E_FAIL;
	}


	/************************************************************************/
	//接口定义开始
	/************************************************************************/
	HRESULT KG3DCloud::GetStruct( KG3DCLOUD_DESCSTRUCT* pResultStruct )
	{
		if(!pResultStruct)return E_FAIL;
		m_DescStruct.ToBaseStruct(*pResultStruct);
		return S_OK;
	}

	HRESULT	KG3DCloud::RequestModification( const KG3DCLOUD_DESCSTRUCT* pStruct )
	{
		if(NULL == pStruct)return E_FAIL;
		return RequestModification((KG3DCloudDescStruct*)pStruct); 
	}

	HRESULT KG3DCloud::ToSolidInterface( LPVOID* pEffectInterface )
	{
		if (pEffectInterface)
		{
			IEKG3DCloud* p = this;
			*pEffectInterface = p;
			return S_OK;
		}
		return E_INVALIDARG;
	}

	HRESULT KG3DCloud::DWGCRequestPopFrontState(float fCurBlendPercent /*= 0.0f*/)
	{
		if (fCurBlendPercent > 0 && fCurBlendPercent < 1)
		{
			float fRet = 0;
			MT_FLOAT_BLEND(m_DescStruct.m_fVisibility, m_SubKey.m_fVisibility, fRet, fCurBlendPercent);
			m_DescStruct.m_fVisibility = fRet;
			D3DCOLOR colorTemp;
			TwoColorBlend(m_DescStruct.m_diffuseColor, m_SubKey.m_diffuseColor, colorTemp, fCurBlendPercent);
			m_DescStruct.m_diffuseColor = colorTemp;
			TwoColorBlend(m_DescStruct.m_ambientColor, m_SubKey.m_ambientColor, colorTemp, fCurBlendPercent);
			m_DescStruct.m_ambientColor = colorTemp;
		}
		else
		{
			m_DescStruct.m_fVisibility = m_SubKey.m_fVisibility;
			m_DescStruct.m_diffuseColor = m_SubKey.m_diffuseColor;
			m_DescStruct.m_ambientColor = m_SubKey.m_ambientColor;
		}
		return S_OK;
	}

	HRESULT KG3DCloud::DWGCRequestSubStateChange( const KG3DDWGC_KeyBase* pRetKey)
	{
		const KG3DDWGC_Key_Cloud* pKey = dynamic_cast<const KG3DDWGC_Key_Cloud*>(pRetKey);
		KG_PROCESS_ERROR(NULL != pKey);
		{
			KG_PROCESS_ERROR(pKey->m_fVisibility > -FLT_EPSILON && pKey->m_fVisibility < 1+ FLT_EPSILON);
			m_SubKey.m_fVisibility = pKey->m_fVisibility;
			m_SubKey.m_diffuseColor = pKey->m_diffuseColor;
			m_SubKey.m_ambientColor = pKey->m_ambientColor;
			return S_OK;
		}
Exit0:
		return E_FAIL;
	}

	HRESULT KG3DCloud::DWGCGetCurStateToKey( KG3DDWGC_KeyBase* pRetKey )
	{
		KG3DDWGC_Key_Cloud* pKey = dynamic_cast<KG3DDWGC_Key_Cloud*>(pRetKey);
		if (pKey)
		{
			pKey->m_fVisibility = m_DescStruct.m_fVisibility;
			pKey->m_diffuseColor = m_DescStruct.m_diffuseColor;
			pKey->m_ambientColor = m_DescStruct.m_ambientColor;
			return S_OK;
		}
		return E_FAIL;
	}

	HRESULT KG3DCloud::DWGCEnableSubState( BOOL bEnable )
	{
		this->SubStateEnable(bEnable);
		return S_OK;
	}

	HRESULT KG3DCloud::DWGCSetPercent( float fPercent )
	{
		m_fPercent = fPercent;
		return S_OK;
	}

	HRESULT KG3DCloud::DWGCRequestModification( const KG3DDWGC_KeyBase* pRetKey)
	{
		KG3DCloudDescStruct TempStruct;
		const KG3DDWGC_Key_Cloud* pKey = dynamic_cast<const KG3DDWGC_Key_Cloud*>(pRetKey);
		KG_PROCESS_ERROR(pKey);
		
		TempStruct.m_fVisibility = pKey->m_fVisibility;
		TempStruct.m_diffuseColor = pKey->m_diffuseColor;
		TempStruct.m_ambientColor = pKey->m_ambientColor;
		TempStruct.m_dwMidificationOption = KG3DCLOUD_MDO_VISIBILITY | KG3DCLOUD_MDO_COLOR;

		return RequestModification(&TempStruct);
Exit0:
		return E_FAIL;
	}

	KG3DCloud* KG3DCloud::Build( LPCTSTR, DWORD_PTR pIniFile, DWORD_PTR strSection )
	{
		KG3DCloud* p = new KG3DCloud;
		KG_PROCESS_ERROR(NULL != p);
		{
			HRESULT hr = p->LoadMapSetting(reinterpret_cast<IIniFile*>(pIniFile), reinterpret_cast<LPCTSTR>(strSection));
			KG_COM_PROCESS_ERROR(hr);

			p->m_ShaderHolder =g_GetDefaultShaderGroup().GetDefaultShader(DEFST_CLOUD);	//Cloud多次创建，ShaderManager保证得到的ShaderHolder是同一个

			return p;
		}
Exit0:
		SAFE_DELETE(p);
		return NULL;
	}

	ULONG __stdcall KG3DCloud::Release()
	{
		delete this;
		return 0;
	}

	BOOL KG3DCloud::IsSubStateEnable()
	{
		return m_State & emEnableSubState;
	}

	VOID KG3DCloud::SubStateEnable( BOOL bEnable )
	{
		bEnable ? m_State |= emEnableSubState : m_State &= ~emEnableSubState;
	}

	BOOL KG3DCloud::IsHeightChangeHandled()
	{
		return m_State & emIsHeightChangeHandled;
	}

	VOID KG3DCloud::SetHeightChangeHandled( BOOL bHandled )
	{
		bHandled ? m_State |= emIsHeightChangeHandled : m_State &= ~emIsHeightChangeHandled;
	}
	/************************************************************************/
	// KG3DCloudDescStruct
	/************************************************************************/
	KG3DCloudDescStruct::KG3DCloudDescStruct()
	{
		///默认值填充
		m_dwMidificationOption = KG3DCLOUD_MDO_DEFAULT;
		m_fHeight = 80000.0f;
		m_fSpeed  = 0.03f;
		m_fAngel  = 0.0f;
		m_fSize   = 500000.0f;
		m_nUVRepeat = 3;
		m_fVisibility = 1.0f;
        m_fAlphaRef   = 0.5f;
        m_fTexFector  = 1.0f;
	}
	KG3DCloudDescStruct::KG3DCloudDescStruct( const KG3DCloudDescStruct &OtherStruct )
	{
		m_tszColorMapPath = NULL;
		m_tszMaskMapPath = NULL;
		operator=(OtherStruct);
	}
	KG3DCloudDescStruct::KG3DCloudDescStruct( const KG3DCLOUD_DESCSTRUCT &OtherStruct )
	{
		m_tszColorMapPath = NULL;
		m_tszMaskMapPath = NULL;
		operator=(OtherStruct);
	}
	void KG3DCloudDescStruct::operator=( const KG3DCloudDescStruct& OtherStruct )
	{
		if (this == &OtherStruct)
		{
			return;
		}
		LPTSTR tempColorMapPath = m_tszColorMapPath;
		LPTSTR tempMaskMapPath = m_tszMaskMapPath;
		memcpy_s(this, sizeof(KG3DCLOUD_DESCSTRUCT), &OtherStruct, sizeof(KG3DCLOUD_DESCSTRUCT));
		m_tszColorMapPath = tempColorMapPath;
		m_tszMaskMapPath = tempMaskMapPath;
		if (OtherStruct.m_tszColorMapPath != m_tszColorMapPath && NULL != m_tszColorMapPath && NULL != OtherStruct.m_tszColorMapPath)
		{
			_tcscpy_s(m_tszColorMapPath, MAX_PATH, OtherStruct.m_tszColorMapPath);
		}
		if (OtherStruct.m_tszMaskMapPath != m_tszMaskMapPath && NULL != m_tszMaskMapPath && NULL != OtherStruct.m_tszMaskMapPath)
		{
			_tcscpy_s(m_tszMaskMapPath, MAX_PATH, OtherStruct.m_tszMaskMapPath);
		}
	}
	void KG3DCloudDescStruct::operator=( const KG3DCLOUD_DESCSTRUCT& OtherStruct )
	{
		if (this == &OtherStruct)
		{
			return;
		}
		LPTSTR tempColorMapPath = m_tszColorMapPath;
		LPTSTR tempMaskMapPath = m_tszMaskMapPath;
		memcpy_s(this, sizeof(KG3DCLOUD_DESCSTRUCT), &OtherStruct, sizeof(KG3DCLOUD_DESCSTRUCT));
		m_tszColorMapPath = tempColorMapPath;
		m_tszMaskMapPath = tempMaskMapPath;
		if (OtherStruct.m_tszColorMapPath != m_tszColorMapPath && NULL != m_tszColorMapPath && NULL != OtherStruct.m_tszColorMapPath)
		{
			_tcscpy_s(m_tszColorMapPath, MAX_PATH, OtherStruct.m_tszColorMapPath);
		}
		if (OtherStruct.m_tszMaskMapPath != m_tszMaskMapPath && NULL != m_tszMaskMapPath && NULL != OtherStruct.m_tszMaskMapPath)
		{
			_tcscpy_s(m_tszMaskMapPath, MAX_PATH, OtherStruct.m_tszMaskMapPath);
		}
	}
	void KG3DCloudDescStruct::ToBaseStruct( KG3DCLOUD_DESCSTRUCT &OtherStruct )
	{
		if (this == &OtherStruct)
		{
			return;
		}
		LPTSTR tempColorMapPath = OtherStruct.m_tszColorMapPath;
		LPTSTR tempMaskMapPath = OtherStruct.m_tszMaskMapPath;
		memcpy_s(&OtherStruct, sizeof(KG3DCLOUD_DESCSTRUCT), this, sizeof(KG3DCLOUD_DESCSTRUCT));
		OtherStruct.m_tszColorMapPath = tempColorMapPath;
		OtherStruct.m_tszMaskMapPath = tempMaskMapPath;
		if (OtherStruct.m_tszColorMapPath != m_tszColorMapPath && NULL != OtherStruct.m_tszColorMapPath && NULL != m_tszColorMapPath)
		{
			_tcscpy_s(OtherStruct.m_tszColorMapPath, MAX_PATH, m_tszColorMapPath);
		}
		if (OtherStruct.m_tszMaskMapPath != m_tszMaskMapPath && NULL != OtherStruct.m_tszMaskMapPath && NULL != m_tszMaskMapPath)
		{
			_tcscpy_s(OtherStruct.m_tszMaskMapPath, MAX_PATH, m_tszMaskMapPath);
		}
	}

	bool KG3DDWGC_Key_Cloud::SaveMapSetting( IIniFile* pIniFile, LPCTSTR pSection )
	{
		if (NULL != pIniFile && NULL != pSection)
		{
			bool bRet = KG_CUSTOM_HELPERS::SaveINIFileData(pIniFile, pSection, _T("fVisibility"), m_fVisibility);
			KG_PROCESS_ERROR(bRet);
			bRet = KG_CUSTOM_HELPERS::SaveINIFileData(pIniFile, pSection, _T("diffuseColor"), m_diffuseColor);
			KG_PROCESS_ERROR(bRet);
			bRet = KG_CUSTOM_HELPERS::SaveINIFileData(pIniFile, pSection, _T("ambientColor"), m_ambientColor);
			KG_PROCESS_ERROR(bRet);
			return true;
		}
Exit0:
		return false;
	}

	bool KG3DDWGC_Key_Cloud::LoadMapSetting( IIniFile* pIniFile, LPCTSTR pSection )
	{
		if(NULL != pIniFile && NULL != pSection)
		{
			if(! KG_CUSTOM_HELPERS::IsSectionExist(pIniFile, pSection))
				return false;

			bool bRet = KG_CUSTOM_HELPERS::LoadINIFileData(pIniFile, pSection, _T("fVisibility"), 1.0f, &m_fVisibility);
			bRet = KG_CUSTOM_HELPERS::LoadINIFileData<D3DCOLOR>(pIniFile, pSection, _T("diffuseColor"), 0xffffffff, &m_diffuseColor);
			bRet = KG_CUSTOM_HELPERS::LoadINIFileData<D3DCOLOR>(pIniFile, pSection, _T("ambientColor"), 0xffffffff, &m_ambientColor);
			return true;
		}
		return false;	
	}

	bool KG3DDWGC_Key_Cloud::CalculateInterp( const KG3DDWGC_KeyBase& BaseKeyBefore
		, const KG3DDWGC_KeyBase& BaseKeyAfter,  float fPercent)
	{
		const KG3DDWGC_Key_Cloud& KeyBefore = dynamic_cast<const KG3DDWGC_Key_Cloud&>(BaseKeyBefore);
		const KG3DDWGC_Key_Cloud& KeyAfter = dynamic_cast<const KG3DDWGC_Key_Cloud&>(BaseKeyAfter);
		_ASSERTE(&KeyBefore && &KeyAfter);
		
		MT_FLOAT_BLEND(KeyBefore.m_fVisibility, KeyAfter.m_fVisibility, this->m_fVisibility, fPercent);
		TwoColorBlend(KeyBefore.m_diffuseColor, KeyAfter.m_diffuseColor, this->m_diffuseColor, fPercent);
		TwoColorBlend(KeyBefore.m_ambientColor, KeyAfter.m_ambientColor, this->m_ambientColor, fPercent);
		return true;
	}
#if defined(DEBUG) | defined(_DEBUG)
	void KG3DDWGC_Key_Cloud::DebugPrint( std::ostringstream& OSS )
	{
		OSS << "Vs:" << m_fVisibility;
	}
#endif
};