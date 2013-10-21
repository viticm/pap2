#include "stdafx.h"

///编译宏选择
#include "KG3DMeshSkyBox.h"
//#include "KG3DTexture.h"
#include "KG3DEnvEffCommonDef.h"
#include "./KG3DEngineManager.h"///g_Def_WorkDirectory
#include "./KG3DGraphicsTool.h"///g_cGraphicTool
//#include "./KG3DTextureTable.h"
#include "KG3DStructs.h"
//#include "GraphicsStruct.h"
//#include "KG3DMeshTable.h"
#include "MathTool.h"
#include "KGINIFileHelpers.h"
#include "shaders/ShaderDefinitions.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

namespace KG3D_ENVIRONMENT_EFFECT
{
	/************************************************************************/
	//Sky成员函数定义开始
	/************************************************************************/
	KG3DMeshSkyBox::KG3DMeshSkyBox(void)
		:KG3DEnvEffect(0, KG3DTYPE_ENVEFF_SKYSYSTEM_SKYBOX)
		,m_pD3DXMesh(NULL)
		,m_pSkyColorMap(NULL)
		,m_pSkyColorSubMap(NULL)
		//,m_uTextureHeight(0)
		//,m_uTextureWidth(0)
		,m_bUseSubTexture(FALSE)
		,m_fBlendPercent(0)
		//,m_bIsInitialized(false)
	{
		//m_d3dc_FogColor = D3DCOLOR_XRGB( 255, 255, 255 );
		m_d3dc_TextureBlendFactor = D3DCOLOR_XRGB(255, 255, 255);
	}

	KG3DMeshSkyBox::~KG3DMeshSkyBox(void)
	{
		SAFE_RELEASE(m_pD3DXMesh);
		SAFE_RELEASE(m_pSkyColorMap);
		SAFE_RELEASE(m_pSkyColorSubMap);	
	}

	//HRESULT KG3DMeshSkyBox::LoadFromFile(const char* cszFileName, unsigned uFileNameHash, unsigned uOption)
//	HRESULT KG3DMeshSkyBox::CustomInit()
//	{
//		HRESULT hr = E_FAIL;
//		KG3DMesh* pMeshTemp = NULL;
//
//		KG_PROCESS_SUCCESS(m_bIsInitialized);
//
//		
//		//创建Mesh
//		{
//#if 1
//			hr = g_cMeshTable.LoadResourceFromFile("Data\\public\\SkyBox\\SkySphere.Mesh", 0, 0, (IKG3DResourceBase**)&pMeshTemp);
//			KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);
//
//			LPD3DXMESH pMeshInner = pMeshTemp->GetMesh(KG3DMesh::SMBT_NORMAL);
//			KG_PROCESS_ERROR(pMeshInner);
//
//			_ASSERTE(NULL == m_pD3DXMesh);
//			m_pD3DXMesh = ProcessMesh(pMeshInner, pMeshTemp->m_BBox.GetCenter(), EM_DEFAULT_RADIUS);
//			KG_PROCESS_ERROR(NULL != m_pD3DXMesh);
//			SAFE_RELEASE(pMeshTemp);
//#else
//
//			LPD3DXMESH pMeshSphere = NULL;
//			hr = D3DXCreateSphere(g_pd3dDevice, EM_DEFAULT_RADIUS, 32, 32, &pMeshSphere, NULL);
//			KG_COM_PROCESS_ERROR(hr);
//			_ASSERTE(NULL == m_pD3DXMesh);
//			m_pD3DXMesh = ProcessMesh(pMeshSphere, D3DXVECTOR3(0,0,0), 1);
//			if (NULL == m_pD3DXMesh)
//			{
//				SAFE_RELEASE(pMeshSphere);
//				goto Exit0;
//			}
//#endif
//		}
//
//		//创建贴图
//		{
//			_ASSERTE(! m_pSkyColorMap);
//			hr = g_cTextureTable.CreateTexture(4, 512, D3DFMT_A8R8G8B8, TEXTYPE_COMMON, &m_pSkyColorMap);
//			KG_COM_PROCESS_ERROR(hr);
//
//			///<TexInfoEnsure>	
//			_ASSERTE( m_pSkyColorMap->GetFormat() == D3DFMT_A8R8G8B8 );///必须是这种格式，不然就没法改了
//			hr = m_pSkyColorMap->GetSize( &m_uTextureWidth, &m_uTextureHeight );
//			KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);
//			///</TexInfoEnsure>
//		}
//		
//Exit1:
//		SAFE_RELEASE(pMeshTemp);
//		m_bIsInitialized = true;
//		return S_OK;
//Exit0:
//		_KG3D_DEBUG_REPORT(_T("Mesh载入失败了，为什么？"));
//		SAFE_RELEASE(pMeshTemp); 
//		SAFE_RELEASE(m_pD3DXMesh);//如果模型载入失败的话，封闭所有方法
//		SAFE_RELEASE(m_pSkyColorMap);
//		return E_FAIL;
//	}

	/*HRESULT KG3DMeshSkyBox::SetSkyTexture(char pszName[])
	{
		KGLOG_PROCESS_ERROR(m_pSkyColorMap);

		return S_OK;
Exit0:
		return E_FAIL;
	}*/

	HRESULT KG3DMeshSkyBox::FrameMove()
	{
		return S_OK;
	}

	HRESULT KG3DMeshSkyBox::Render(const KG3DEnvEffRenderContext& renderContext)
	{
		HRESULT hr = E_FAIL;
		_ASSERTE(NULL != m_pD3DXMesh && NULL != m_pSkyColorMap && NULL != m_pSkyColorSubMap);
		LPD3DXEFFECT pShader = g_GetDefaultShaderGroup().GetDefaultEffect(DEFST_SKYBOX);
		_ASSERTE(NULL != pShader);
		{
			static ShaderConstsSkyBox	shaderConsts;
			shaderConsts.fScale	= 5000;

			HrV(pShader->SetValue("g_shaderConstsSkyBox", &shaderConsts, sizeof(shaderConsts)));
	
			static ShaderVariantsSkyBox shaderVariants;
			shaderVariants.fBlendPercent = m_fBlendPercent;
			shaderVariants.vBottomColor.x = renderContext.vFogColor.r;
			shaderVariants.vBottomColor.y = renderContext.vFogColor.g;
			shaderVariants.vBottomColor.z = renderContext.vFogColor.b;

			{
				/*FLOAT fBottomPercent0 = (FLOAT)m_DescStruct.nBotmPercent
					/ (FLOAT)(m_DescStruct.nTopPercent + m_DescStruct.nMidPercent + m_DescStruct.nBotmPercent);

				FLOAT fBottomPercent1 = (FLOAT)m_SubKey.nBotmPercent
					/ (FLOAT)(m_SubKey.nTopPercent + m_SubKey.nMidPercent + m_SubKey.nBotmPercent);

				FLOAT fResult = 0;
				MT_FLOAT_BLEND(fBottomPercent0, fBottomPercent1, fResult, m_fBlendPercent);*/

				KG3DDWGC_Key_SkyBox mainKey;
				m_DescStruct.ToKey(mainKey);

				KG3DDWGC_Key_SkyBox curKey;
				curKey.CalculateInterp(mainKey, m_SubKey, m_fBlendPercent);

				FLOAT fTotalPercent = (FLOAT)(curKey.nTopPercent + curKey.nMidPercent + curKey.nBotmPercent);
				if (fTotalPercent < FLT_EPSILON)
				{
					shaderVariants.fBottomBlendStartMinusEnd = 0.001f;
				}
				else
				{
					shaderVariants.fBottomBlendStartMinusEnd = (FLOAT)(curKey.nMid_BotmBlendPercent) / fTotalPercent;
					if (shaderVariants.fBottomBlendStartMinusEnd < FLT_EPSILON)
					{
						shaderVariants.fBottomBlendStartMinusEnd = 0.001f;
					}
				}
				
				shaderVariants.fBottomBlendEnd = (FLOAT)(curKey.nBotmPercent) / fTotalPercent;
			}			
			
			{
				D3DXMATRIX matView, matProj;
				g_pd3dDevice->GetTransform(D3DTS_VIEW, &matView);
				g_pd3dDevice->GetTransform(D3DTS_PROJECTION, &matProj);

				shaderVariants.matViewProj	= matView * matProj;

				hr = g_cGraphicsTool.GetCurCameraData(&shaderVariants.vCamPos, NULL);
				KG_COM_PROCESS_ERROR(hr);
			}

			HrV(pShader->SetValue("g_shaderVariantsSkyBox", &shaderVariants, sizeof(shaderVariants)));
			HrV(pShader->SetTexture("g_texMain", m_pSkyColorMap));
			LPDIRECT3DTEXTURE9 pSubTex = m_bUseSubTexture ? m_pSkyColorSubMap : m_pSkyColorMap;
			HrV(pShader->SetTexture("g_texSub", pSubTex));
			

			HrV(pShader->Begin(NULL, 0));
			{
				pShader->BeginPass(0);
				m_pD3DXMesh->DrawSubset(0);
				pShader->EndPass();
				pShader->End();
			}
		}
Exit0:
		return S_OK;
	}
	KG3DMeshSkyBox::RenderState::RenderState()
		:m_AddressVCamp(0)
	{
		
	}	

//	LPD3DXMESH KG3DMeshSkyBox::ProcessMesh(LPD3DXMESH pMeshIn, D3DXVECTOR3	vecCenter, FLOAT fSize)
//	{
//		_ASSERTE(pMeshIn);
//		/*float fSize = EM_DEFAULT_RADIUS;*/
//		HRESULT hr = E_FAIL;
//
//		LPD3DXMESH pMesh = NULL;
//		hr = pMeshIn->CloneMeshFVF(D3DXMESH_MANAGED, D3DFVF_XYZ | D3DFVF_NORMAL| D3DFVF_TEX1, g_pd3dDevice, &pMesh);
//		KG_COM_PROCESS_ERROR(hr);
//
//		hr = D3DXComputeNormals(pMesh,NULL);
//		KG_COM_PROCESS_ERROR(hr);
//
//		struct TypeVertex 
//		{
//			D3DXVECTOR3 Pos;
//			D3DXVECTOR3 Nor;
//			D3DXVECTOR2 Tex;
//		}* pVers = NULL;
//
//		hr = pMesh->LockVertexBuffer(0, (void**)&pVers);
//		KG_COM_PROCESS_ERROR(hr);
//		{
//			DWORD dwNumVertices = pMesh->GetNumVertices();
//			_ASSERTE(pMesh->GetNumBytesPerVertex() == sizeof(TypeVertex));
//			D3DXVECTOR3 dir;
//			for (DWORD i = 0; i < dwNumVertices; i++)
//			{
//				dir = pVers[i].Pos - vecCenter;		///以Size为倍数放大Mesh
//				D3DXVec3Normalize( &dir, &dir );
//
//				//pVers[i].Tex = D3DXVECTOR2(dir.x, (dir.y + 1) / 2);
//
//				pVers[i].Pos += ( dir )*fSize;
//			}
//
//			pMesh->UnlockVertexBuffer();
//		}
//		return pMesh;
//Exit0:
//		SAFE_RELEASE(pMesh);
//		return NULL;
//	}
	
	HRESULT	KG3DMeshSkyBox::ModifyTextureByInterp(const KG3DMeshSkyBoxDescStruct& TargetStruct, LPDIRECT3DTEXTURE9 pTex)	
	{
		HRESULT hr = E_FAIL;
		KG_PROCESS_ERROR(NULL != pTex);	
		{
			///拆分出三个颜色的RGB值
			D3DCOLORVALUE d3dxcolor_Top = Color2Value( TargetStruct.d3dc_xrgb_TopColor );
			D3DCOLORVALUE d3dxcolor_Mid = Color2Value( TargetStruct.d3dc_xrgb_MidColor );
			//D3DCOLORVALUE d3dxcolor_Botm = Color2Value( TargetStruct.d3dc_xrgb_BotmColor ); 
			D3DCOLORVALUE d3dxcolor_Botm = Color2Value(TargetStruct.d3dc_xrgb_MidColor);	//使用0作为颜色，Bottom的现在换到PixelShader做
			D3DCOLORVALUE d3dxcolor_Temp = {1,1,1,1};

			SkyColorPosStruct	TempPosStruct;

			//LPDIRECT3DTEXTURE9 pD3DTexSkyColorMap = pTex;
			
			D3DSURFACE_DESC desc;
			HrV(pTex->GetLevelDesc(0, &desc));

			int nWidth = (int)desc.Width;
			int nHeight = (int)desc.Height;

			///对结构中的信息进行预处理
			///先防缩到整数的cStruct再使用	
			TranslateStructToPos( TargetStruct, TempPosStruct, nWidth, nHeight);


			D3DLOCKED_RECT	rect;
			int	nX =0, nY = 0;

			typedef DWORD TypePixel;
			KGLOG_PROCESS_ERROR( nWidth && nHeight );///不能由GetSize生成0

			//开始锁定
			HRESULT hr = pTex->LockRect( 0, &rect, NULL, 0);
			KG_COM_PROCESS_ERROR(hr);

			BYTE* pLine = (BYTE*)rect.pBits;
			
			D3DCOLOR	d3dc_RowPixelColor = D3DCOLOR_XRGB( 0, 0, 0 );

			float	fAddR = 0.0f, fAddG = 0.0f, fAddB = 0.0f;///每次增量
			

			///遍历每行
			nY = TempPosStruct.n0TopPos;

			///Top的部分
			d3dc_RowPixelColor = TargetStruct.d3dc_xrgb_TopColor;
			for (/*nY在外部初始化*/ ;nY < TempPosStruct.n1TopMidBlendBoundPos; nY++, pLine += rect.Pitch)	
			{
				TypePixel* pPixels = (TypePixel*)pLine;
				for ( nX = 0; nX < nWidth; nX++, pPixels++ )///每行
					(*pPixels) = d3dc_RowPixelColor;
			}

			///每次增量，并设置初始值
			fAddR = ( d3dxcolor_Mid.r - d3dxcolor_Top.r ) 
				/ ( TempPosStruct.n2MidTopPos - TempPosStruct.n1TopMidBlendBoundPos); 
			fAddG = ( d3dxcolor_Mid.g - d3dxcolor_Top.g ) 
				/ ( TempPosStruct.n2MidTopPos - TempPosStruct.n1TopMidBlendBoundPos); 
			fAddB = ( d3dxcolor_Mid.b - d3dxcolor_Top.b ) 
				/ ( TempPosStruct.n2MidTopPos - TempPosStruct.n1TopMidBlendBoundPos); 
			d3dxcolor_Temp.r = d3dxcolor_Top.r;	
			d3dxcolor_Temp.g = d3dxcolor_Top.g;
			d3dxcolor_Temp.b = d3dxcolor_Top.b;

			///Top和Mid之间Blend的部分
			for ( ;nY < TempPosStruct.n2MidTopPos; nY++, pLine += rect.Pitch )
			{
				TypePixel* pPixels = (TypePixel*)pLine;
				d3dxcolor_Temp.r += fAddR;
				d3dxcolor_Temp.g += fAddG;
				d3dxcolor_Temp.b += fAddB;
				d3dc_RowPixelColor = D3DCOLOR_COLORVALUE( d3dxcolor_Temp.r, d3dxcolor_Temp.g, d3dxcolor_Temp.b, 1.0f );

				for ( nX = 0; nX < nWidth; nX++, pPixels++ )///每行
					(*pPixels) = d3dc_RowPixelColor;
			}
			
			///Mid的部分
			d3dc_RowPixelColor = D3DCOLOR_COLORVALUE( d3dxcolor_Temp.r, d3dxcolor_Temp.g, d3dxcolor_Temp.b, 1.0f );	///为了融合，用Temp的颜色
			for (  ;nY < TempPosStruct.n3MidBotmPos; nY++, pLine += rect.Pitch )	
			{
				TypePixel* pPixels = (TypePixel*)pLine;
				for ( nX = 0; nX < nWidth; nX++, pPixels++ )///每行
					(*pPixels) = d3dc_RowPixelColor;
			}

			///每次增量，并设置初始值
			fAddR = ( d3dxcolor_Botm.r - d3dxcolor_Temp.r ) 
				/ ( TempPosStruct.n4MidBotmBlendBoundPos - TempPosStruct.n3MidBotmPos); 
			fAddG = ( d3dxcolor_Botm.g - d3dxcolor_Temp.g ) 
				/ ( TempPosStruct.n4MidBotmBlendBoundPos - TempPosStruct.n3MidBotmPos); 
			fAddB = ( d3dxcolor_Botm.b - d3dxcolor_Temp.b ) 
				/ ( TempPosStruct.n4MidBotmBlendBoundPos - TempPosStruct.n3MidBotmPos); 

			///Mid和Bottom之间的部分
			for ( ; nY < TempPosStruct.n4MidBotmBlendBoundPos; nY++, pLine += rect.Pitch )
			{
				TypePixel* pPixels = (TypePixel*)pLine;
				d3dxcolor_Temp.r += fAddR;
				d3dxcolor_Temp.g += fAddG;
				d3dxcolor_Temp.b += fAddB;
				d3dc_RowPixelColor = D3DCOLOR_COLORVALUE( d3dxcolor_Temp.r, d3dxcolor_Temp.g, d3dxcolor_Temp.b, 1.0f );

				for ( nX = 0; nX < nWidth; nX++, pPixels++ )///每行
					(*pPixels) = d3dc_RowPixelColor;
			}

			///Bott0m的部分
			d3dc_RowPixelColor = D3DCOLOR_COLORVALUE( d3dxcolor_Temp.r, d3dxcolor_Temp.g, d3dxcolor_Temp.b, 1.0f );	///为了融合，用Temp的颜色
			for ( ; nY < TempPosStruct.n5BotmPos ; nY++, pLine += rect.Pitch )	
			{
				TypePixel* pPixels = (TypePixel*)pLine;
				for ( nX = 0; nX < nWidth; nX++, pPixels++ )///每行
					(*pPixels) = d3dc_RowPixelColor;
			}

			_ASSERTE(pLine <= (BYTE*)rect.pBits + rect.Pitch * (nHeight + 1));

			pTex->UnlockRect(0);
		}
		return S_OK;
Exit0:
		return E_FAIL;
	}

	HRESULT KG3DMeshSkyBox::SaveMapSetting(IIniFile* pIniFile, LPCTSTR pSection)
	{
		_ASSERTE( pIniFile );
		bool bResult = m_DescStruct.SaveMapSetting(pIniFile, "SkySphere");
		KG_PROCESS_ERROR(bResult);
		//pIniFile->WriteInteger("SkySphere", "BoolSynWithFogColor", (int)m_bSynWithFogColor);

		//Exit1:
		return S_OK;
Exit0:
		return E_FAIL;
	}
//	HRESULT KG3DMeshSkyBox::LoadMapSetting(IIniFile* pIniFile, LPCTSTR pSection)
//	{
//		
//		HRESULT hr = E_FAIL;
//		if(! pIniFile)
//		{
//			hr = CustomInit();
//			KG_COM_PROCESS_ERROR(hr);
//
//			hr = RequestModification((const KG3DMeshSkyBoxDescStruct*)NULL);//用默认的Struct来创建
//			KG_COM_PROCESS_ERROR(hr);
//			return hr;
//		}
//		else
//		{
//			_ASSERTE( pIniFile );
//			if (m_bIsInitialized)
//			{
//				KG_COM_PROCESS_ERROR(CustomInit());
//			}
//			KG3DMeshSkyBoxDescStruct	TempStruct;
//			bool bResult = TempStruct.LoadMapSetting(pIniFile, "SkySphere");
//			KG_PROCESS_ERROR(bResult);
//
//			hr = ModifyTextureByInterp(TempStruct, m_pSkyColorMap);
//			if (SUCCEEDED(hr))
//			{
//				m_DescStruct = TempStruct;
//			}
//
//			/*
//			BOOL BoolSynWithFogColor;
//						pIniFile->GetInteger( "SkySphere", "BoolSynWithFogColor", 0, &BoolSynWithFogColor );
//						EnableSynWithFogColor( BoolSynWithFogColor, m_SkyColorStruct.d3dc_xrgb_BotmColor );*/
//			
//
//			return S_OK;
//		}
//Exit0:
//		_KG3D_DEBUG_SELF_ASSERTE(NULL&&_STRINGER(KG3DMeshSkyBox)":"_STRINGER(LoadMapSetting));
//		return E_FAIL;
//	}
	HRESULT	KG3DMeshSkyBox::RequestModification( const KG3DMeshSkyBoxDescStruct* pSKStruct )
	{		
		KG_PROCESS_ERROR(NULL != pSKStruct);
		//KG_PROCESS_ERROR(pSKStruct->fRadius > 0 && pSKStruct->fRadius < FLT_MAX);

		KG_PROCESS_ERROR(pSKStruct->nTopPercent >= 0);
		KG_PROCESS_ERROR(pSKStruct->nTop_MidBlendPercent >= 0);
		KG_PROCESS_ERROR(pSKStruct->nMidPercent >= 0);
		KG_PROCESS_ERROR(pSKStruct->nMid_BotmBlendPercent >= 0);
		KG_PROCESS_ERROR(pSKStruct->nBotmPercent >= 0);
		//KG_PROCESS_ERROR(pSKStruct->nMid_BotmBlendPercent <= pSKStruct->nMidPercent && pSKStruct->nTop_MidBlendPercent <= pSKStruct->nTopPercent);

		HRESULT hr = ModifyTextureByInterp( *pSKStruct, m_pSkyColorMap );
		KG_COM_PROCESS_ERROR(hr);
		if(&m_DescStruct != pSKStruct)
		{
			m_DescStruct = *pSKStruct;
		}
		return S_OK;
Exit0:
		_KG3D_DEBUG_SELF_ASSERTE(NULL&&_STRINGER(KG3DMeshSkyBox)":"_STRINGER(RequestModification));
		return E_FAIL;
	}
	
	/*HRESULT KG3DMeshSkyBox::RequestSubTextureRedraw(const KG3DMeshSkyBoxDescStruct* pSKStruct)
	{
		if (!m_bIsInitialized)
		{
			KG_COM_PROCESS_ERROR(CustomInit());
		}
		KG_PROCESS_ERROR(pSKStruct);
		{
			KG3DMeshSkyBoxDescStruct tempStruct(*pSKStruct);
			_KG3D_DEBUG_SELF_OUTPUT0(_STRINGER(KG3DMeshSkyBox)":"_STRINGER(RequestSubTextureRedraw));

			if (! m_pSkyColorSubMap)
			{
				return CreateSubTextureByInterp(tempStruct);
			}
			HRESULT hr = ModifyTextureByInterp(tempStruct, m_pSkyColorSubMap);
			if (SUCCEEDED(hr))
			{
				tempStruct.ToKey(m_SubKey);
			}
			_KG3D_DEBUG_SELF_OUTPUT1(":%d\n",((SUCCEEDED(hr)) ? TRUE : FALSE));
			return hr;
		}
Exit0:
		_KG3D_DEBUG_SELF_ASSERTE(NULL&&_STRINGER(KG3DMeshSkyBox)":"_STRINGER(RequestSubTextureRedraw));
		return E_FAIL;
	}*/

//
//	HRESULT KG3DMeshSkyBox::CreateSubTextureByInterp(const KG3DMeshSkyBoxDescStruct& SKSubStruct)
//	{
//		HRESULT hr;
//
//		KG_PROCESS_ERROR(! m_pSkyColorSubMap);
//		_ASSERTE(! m_pSkyColorSubMap);
//		_ASSERTE(m_pSkyColorMap);//需要写创建材质的代码,主贴图创建之后这个才能创建
//
//		hr = g_cTextureTable.CreateTexture(m_uTextureWidth, m_uTextureHeight
//			, D3DFMT_A8R8G8B8, TEXTYPE_COMMON, &m_pSkyColorSubMap);
//		KG_COM_PROCESS_ERROR(hr);
//
//		///<TexInfoEnsure>	
//		_ASSERTE( m_pSkyColorSubMap->GetFormat() == D3DFMT_A8R8G8B8 );///必须是这种格式，不然就没法改了
//		///</TexInfoEnsure>
//
//		hr = ModifyTextureByInterp(SKSubStruct, m_pSkyColorSubMap);
//		if (SUCCEEDED(hr))
//		{
//			SKSubStruct.ToKey(m_SubKey);
//		}
//		KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);
//
//		//Exit1:
//		return S_OK;
//Exit0:
//		SAFE_RELEASE(m_pSkyColorSubMap);
//		return E_FAIL;	
//	}
	VOID KG3DMeshSkyBox::TranslateStructToPos(const KG3DMeshSkyBoxDescStruct& cStruct, SkyColorPosStruct& cPosStruct
		, int nTextureWidth, int nTextureHeight)
	{
		///先防缩到整数的cStruct再使用
		KG3DMeshSkyBoxDescStruct tempStruct = cStruct;

		//检查数据有效性并裁剪数据
		MT_LIMIT_TO_RANGE(tempStruct.nTopPercent, 1000, 0);
		MT_LIMIT_TO_RANGE(tempStruct.nMidPercent, 1000, 0);
		MT_LIMIT_TO_RANGE(tempStruct.nBotmPercent, 1000, 0);
		MT_LIMIT_TO_RANGE(tempStruct.nTop_MidBlendPercent, tempStruct.nTopPercent, 0);
		MT_LIMIT_TO_RANGE(tempStruct.nMid_BotmBlendPercent, tempStruct.nMidPercent, 0);

		//把把数据防缩到贴图高度
		int	nTempTotalPercent = tempStruct.nTopPercent + tempStruct.nMidPercent + tempStruct.nBotmPercent;
		if (nTempTotalPercent <= 0)///读取不正确啦
		{
			nTempTotalPercent = 1;
		}
		
		float fPercentScaleFactor = (float)nTextureHeight / nTempTotalPercent;  

		tempStruct.nTopPercent = (int)(tempStruct.nTopPercent * fPercentScaleFactor );
		_ASSERTE( 0 <= tempStruct.nTopPercent && tempStruct.nTopPercent <= nTextureHeight );

		tempStruct.nMidPercent = (int)(tempStruct.nMidPercent * fPercentScaleFactor );
		_ASSERTE( 0 <= tempStruct.nMidPercent && tempStruct.nMidPercent <= nTextureHeight );

		tempStruct.nBotmPercent = nTextureHeight - tempStruct.nTopPercent - tempStruct.nMidPercent;
		_ASSERTE(  0 <= tempStruct.nBotmPercent && tempStruct.nBotmPercent <= nTextureHeight );

		tempStruct.nTop_MidBlendPercent 
			= (int)(tempStruct.nTop_MidBlendPercent * fPercentScaleFactor ); 
		if(  0 > tempStruct.nTop_MidBlendPercent )
			tempStruct.nTop_MidBlendPercent = 0;
		if( tempStruct.nTop_MidBlendPercent > tempStruct.nTopPercent )
			tempStruct.nTop_MidBlendPercent = tempStruct.nTopPercent;

		tempStruct.nMid_BotmBlendPercent 
			= (int)(tempStruct.nMid_BotmBlendPercent * fPercentScaleFactor ); 
		if(  0 > tempStruct.nMid_BotmBlendPercent )
			tempStruct.nMid_BotmBlendPercent = 0;
		if( tempStruct.nMid_BotmBlendPercent > tempStruct.nBotmPercent )
			tempStruct.nMid_BotmBlendPercent = tempStruct.nBotmPercent;


		//范围转换完成
		//开始边界转换
		cPosStruct.nTotalHeight = nTextureHeight;
		///Pos是要用小于比较的，而不是小于或等于，所以不用管数组从0开始的问题，每个范围用 A <= X < B比较行了
		cPosStruct.n0TopPos = 0;
		cPosStruct.n1TopMidBlendBoundPos = tempStruct.nTopPercent-tempStruct.nTop_MidBlendPercent;
		cPosStruct.n2MidTopPos = cPosStruct.n1TopMidBlendBoundPos + tempStruct.nTop_MidBlendPercent;
		cPosStruct.n3MidBotmPos = cPosStruct.n2MidTopPos + tempStruct.nMidPercent - tempStruct.nMid_BotmBlendPercent;
		cPosStruct.n4MidBotmBlendBoundPos = cPosStruct.n3MidBotmPos + tempStruct.nMid_BotmBlendPercent;
		cPosStruct.n5BotmPos = cPosStruct.nTotalHeight;
		//保证各个边界呈递增状态
		_ASSERTE( cPosStruct.n0TopPos <= cPosStruct.n1TopMidBlendBoundPos );
		_ASSERTE( cPosStruct.n1TopMidBlendBoundPos <= cPosStruct.n2MidTopPos );
		_ASSERTE( cPosStruct.n2MidTopPos <= cPosStruct.n3MidBotmPos );
		_ASSERTE( cPosStruct.n3MidBotmPos <= cPosStruct.n4MidBotmBlendBoundPos );
		_ASSERTE( cPosStruct.n4MidBotmBlendBoundPos <= cPosStruct.n5BotmPos );///保证不会数组越界

		//Exit1:
		return;
	}
	
	//用于动态天气的部分	
	HRESULT KG3DMeshSkyBox::ToSolidInterface( LPVOID* pEffectInterface )
	{
		if (!pEffectInterface)
		{
			return E_INVALIDARG;
		}
		IEKG3DMeshSkyBox* pInterface = this;
		*pEffectInterface = static_cast<LPVOID>(pInterface);
		return S_OK;
	}
	
	///<DW_Controller>
	HRESULT KG3DMeshSkyBox::DWGCRequestPopFrontState(float fCurBlendPercent/* = 0.0f*/)
	{
		HRESULT hr = E_FAIL;
		if (fCurBlendPercent > 0 && fCurBlendPercent < 1.0f)
		{
			KG3DDWGC_Key_SkyBox MidKey, BeforeKey;
			m_DescStruct.ToKey(BeforeKey);
			MidKey.CalculateInterp(BeforeKey, m_SubKey, fCurBlendPercent);
			KG3DMeshSkyBoxDescStruct TempStruct = MidKey;
			TempStruct.m_dwModificationOption = KG3DMESHSKYBOX_MDO_ALL;
			hr = this->RequestModification(&TempStruct);
		}
		else
		{
			//交换两张贴图
			{
				LPDIRECT3DTEXTURE9	pTempTexture = m_pSkyColorMap;
				m_pSkyColorMap = m_pSkyColorSubMap;
				m_pSkyColorSubMap = pTempTexture;
			}

			//交换两个Struct
			{
				KG3DMeshSkyBoxDescStruct tempStruct = m_DescStruct;
				m_DescStruct = m_SubKey;
				tempStruct.ToKey(m_SubKey);
				hr = S_OK;
			}
		}
		
		_KG3D_DEBUG_SELF_OUTPUT0(_STRINGER(KG3DMeshSkyBox)":"_STRINGER(DWGCRequestPopFrontState)"\n");
		return hr;
	}
	HRESULT KG3DMeshSkyBox::DWGCGetCurStateToKey( KG3DDWGC_KeyBase* pRetKey)
	{
		TypeKey* pSolidKey = dynamic_cast<TypeKey*>(pRetKey);
		KG_PROCESS_ERROR(pSolidKey);
		m_DescStruct.ToKey(*pSolidKey);
		return S_OK;
Exit0:
		return E_FAIL;
	}

	HRESULT KG3DMeshSkyBox::DWGCRequestSubStateChange(const KG3DDWGC_KeyBase* pRetKey)
	{
		_KG3D_DEBUG_SELF_OUTPUT0(_STRINGER(KG3DMeshSkyBox)":"_STRINGER(DWGCRequestSubStateChange)"\n");
		const TypeKey* pSolidKey = dynamic_cast<const TypeKey*>(pRetKey);
		KG_PROCESS_ERROR(pSolidKey);
		{
			KG3DMeshSkyBoxDescStruct tempStruct(*pSolidKey);
			/*if (! m_pSkyColorSubMap)
			{
				return CreateSubTextureByInterp(tempStruct);
			}*/
			_ASSERTE(NULL != m_pSkyColorSubMap);
			HRESULT hr = ModifyTextureByInterp(tempStruct, m_pSkyColorSubMap);
			if (SUCCEEDED(hr))
			{
				m_SubKey = *pSolidKey;
			}
			return hr;
		}
Exit0:
		return E_FAIL;
	}

	HRESULT KG3DMeshSkyBox::DWGCEnableSubState( BOOL bEnable )
	{
		if(NULL != m_pSkyColorSubMap)
		{
			m_bUseSubTexture = bEnable;
			return S_OK;
		}
		m_bUseSubTexture = FALSE;
		return S_OK;
	}

	HRESULT KG3DMeshSkyBox::DWGCSetPercent( float fPercent )
	{
		m_fBlendPercent = fPercent;
		int nAlpha = (int)(255*(m_fBlendPercent));
		m_d3dc_TextureBlendFactor = D3DCOLOR_ARGB(nAlpha, 0, 0, 0);
		return S_OK;
	}
	HRESULT KG3DMeshSkyBox::DWGCRequestModification( const KG3DDWGC_KeyBase* pKey )
	{
		const KG3DDWGC_Key_SkyBox* pKeySkyBox = dynamic_cast<const KG3DDWGC_Key_SkyBox*>(pKey);
		if(!pKeySkyBox)
			return E_FAIL;
		KG3DMeshSkyBoxDescStruct TempStruct(*pKeySkyBox);
		TempStruct.m_dwModificationOption = KG3DMESHSKYBOX_MDO_ALL;
		return RequestModification((KG3DMeshSkyBoxDescStruct*)&TempStruct);	
	}
	///</DW_Controller>

	/************************************************************************/
	//接口定义开始
	/************************************************************************/
	HRESULT KG3DMeshSkyBox::RequestModification( const KG3DMESHSKYBOX_DESCSTRUCT* pSKStruct )
	{
		if (NULL == m_pSkyColorMap || NULL == pSKStruct )
		{
			return E_FAIL; //如果贴图没有创建的话就不能修改贴图内容
		}
		KG3DMeshSkyBoxDescStruct tempStruct(*pSKStruct);
		HRESULT hr = ModifyTextureByInterp(tempStruct, m_pSkyColorMap);
		if (SUCCEEDED(hr))
		{
			m_DescStruct = tempStruct;
		}
		return hr;
	}
	HRESULT KG3DMeshSkyBox::GetDescStruct( KG3DMESHSKYBOX_DESCSTRUCT* pSKStruct )
	{
		_ASSERTE(pSKStruct);
		m_DescStruct.ToBaseStruct(pSKStruct);
		return S_OK;
	}

	HRESULT KG3DMeshSkyBox::CreateInstance(IIniFile* pIniFile, LPCTSTR strSection, LPVOID* ppInstance )
	{
		HRESULT hr = E_FAIL;

		KG3DMeshSkyBox* pSkyBox = NULL;
		//KG3DMesh* pMeshTemp = NULL;
		KG_PROCESS_ERROR(NULL != ppInstance);
		pSkyBox = new KG3DMeshSkyBox;
		KG_PROCESS_ERROR(NULL != pSkyBox);

		{
			//创建Mesh
			{
				/*hr = g_cMeshTable.LoadResourceFromFile("Data\\public\\SkyBox\\SkySphere.Mesh", 0, 0, (IKG3DResourceBase**)&pMeshTemp);
				KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

				LPD3DXMESH pMeshInner = pMeshTemp->GetMesh(KG3DMesh::SMBT_NORMAL);
				KG_PROCESS_ERROR(pMeshInner);

				_ASSERTE(NULL == pSkyBox->m_pD3DXMesh);
				pSkyBox->m_pD3DXMesh = ProcessMesh(pMeshInner, pMeshTemp->m_BBox.GetCenter(), EM_DEFAULT_RADIUS);
				KG_PROCESS_ERROR(NULL != pSkyBox->m_pD3DXMesh);
				SAFE_RELEASE(pMeshTemp);*/

				hr = D3DXCreateSphere(g_pd3dDevice, 1,32,32,&pSkyBox->m_pD3DXMesh, NULL);
				KG_COM_PROCESS_ERROR(hr);
			}

			//创建贴图
			{
				_ASSERTE(NULL == pSkyBox->m_pSkyColorMap);
				//hr = g_cTextureTable.CreateTexture(4, 512, D3DFMT_A8R8G8B8, TEXTYPE_COMMON, &pSkyBox->m_pSkyColorMap);
				hr = D3DXCreateTexture(g_pd3dDevice, 4, 512, 1, 0, D3DFMT_X8R8G8B8, D3DPOOL_MANAGED, &pSkyBox->m_pSkyColorMap);
				KG_COM_PROCESS_ERROR(hr);

				///<TexInfoEnsure>	
				//_ASSERTE( pSkyBox->m_pSkyColorMap->GetFormat() == D3DFMT_A8R8G8B8 );///必须是这种格式，不然就没法改了
				//hr = pSkyBox->m_pSkyColorMap->GetSize( &pSkyBox->m_uTextureWidth, &pSkyBox->m_uTextureHeight );
				//KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);
				///</TexInfoEnsure>
			

				_ASSERTE(NULL == pSkyBox->m_pSkyColorSubMap);
				//hr = g_cTextureTable.CreateTexture(pSkyBox->m_uTextureWidth, pSkyBox->m_uTextureHeight
				//	, D3DFMT_A8R8G8B8, TEXTYPE_COMMON, &(pSkyBox->m_pSkyColorSubMap));
				hr = D3DXCreateTexture(g_pd3dDevice, 4, 512, 1, 0, D3DFMT_X8R8G8B8, D3DPOOL_MANAGED, &pSkyBox->m_pSkyColorSubMap);
				KG_COM_PROCESS_ERROR(hr);

				//_ASSERTE( pSkyBox->m_pSkyColorSubMap->GetFormat() == D3DFMT_A8R8G8B8 );///必须是这种格式，不然就没法改了
			}
		}

		if(! pIniFile)
		{
			pSkyBox->m_DescStruct.SetDefaultValue();
			hr = pSkyBox->ModifyTextureByInterp(pSkyBox->m_DescStruct, pSkyBox->m_pSkyColorMap);
			KG_COM_PROCESS_ERROR(hr);
		}
		else
		{
			KG3DMeshSkyBoxDescStruct	TempStruct;
			bool bResult = TempStruct.LoadMapSetting(pIniFile, strSection);
			KG_PROCESS_ERROR(bResult);

			hr = pSkyBox->ModifyTextureByInterp(TempStruct, pSkyBox->m_pSkyColorMap);
			if (SUCCEEDED(hr))
			{
				pSkyBox->m_DescStruct = TempStruct;
			}

			/*
			BOOL BoolSynWithFogColor;
			pIniFile->GetInteger( "SkySphere", "BoolSynWithFogColor", 0, &BoolSynWithFogColor );
			EnableSynWithFogColor( BoolSynWithFogColor, m_SkyColorStruct.d3dc_xrgb_BotmColor );*/
		}

		//初始化完成之后把Shader的常量设进去
		{
			//Preset some data
			/*KG3DShaderHolder shaderHolder = g_GetDefaultShaderGroup().GetShaderHolder(DEFST_SKYBOX);
			{
				static ShaderConstsSkyBox	shaderConsts;
				shaderConsts.fScale	= m_DescStruct.fRadius;
			}*/
		}

		//SAFE_RELEASE(pMeshTemp);
		_ASSERTE(NULL != ppInstance);
		*(KG3DMeshSkyBox**)ppInstance = pSkyBox;
		return S_OK;
Exit0:
		//SAFE_RELEASE(pMeshTemp);
		SAFE_DELETE(pSkyBox);
		return E_FAIL;
	}
	/************************************************************************/
	//KG3DMeshSkyBoxDescStruct定义开始
	/************************************************************************/
	KG3DMeshSkyBoxDescStruct::KG3DMeshSkyBoxDescStruct()
	{
		this->SetDefaultValue();
	}

	KG3DMeshSkyBoxDescStruct::KG3DMeshSkyBoxDescStruct(const KG3DMeshSkyBoxDescStruct& OtherStruct )
	{
		if ( &OtherStruct == this )
		{
			return;
		}
		const KG3DMESHSKYBOX_DESCSTRUCT* pOtherStructBase = &OtherStruct;
		KG3DMESHSKYBOX_DESCSTRUCT* pMyBase = this;
		memcpy_s( pMyBase, sizeof(KG3DMESHSKYBOX_DESCSTRUCT), pOtherStructBase, sizeof(KG3DMESHSKYBOX_DESCSTRUCT));	
	}

	KG3DMeshSkyBoxDescStruct::KG3DMeshSkyBoxDescStruct(const KG3DMESHSKYBOX_DESCSTRUCT& OtherStruct )
	{
		if ( &OtherStruct == this )
		{
			return;
		}
		KG3DMESHSKYBOX_DESCSTRUCT* pMyBase = this;
		memcpy_s( pMyBase, sizeof(KG3DMESHSKYBOX_DESCSTRUCT), &OtherStruct, sizeof(KG3DMESHSKYBOX_DESCSTRUCT));	
	}

	KG3DMeshSkyBoxDescStruct::KG3DMeshSkyBoxDescStruct( const KG3DDWGC_Key_SkyBox& OtherStruct )
	{
		if ( (KG3DMESHSKYBOX_DESCSTRUCT*)&OtherStruct == (KG3DMESHSKYBOX_DESCSTRUCT*)this )
		{
			return;
		}
		const KG3DMESHSKYBOX_DESCSTRUCT* pOtherStructBase = &OtherStruct;
		KG3DMESHSKYBOX_DESCSTRUCT* pMyBase = this;
		memcpy_s( pMyBase, sizeof(KG3DMESHSKYBOX_DESCSTRUCT), pOtherStructBase, sizeof(KG3DMESHSKYBOX_DESCSTRUCT));	
	}
	bool KG3DMeshSkyBoxDescStruct::operator==( const KG3DMeshSkyBoxDescStruct &cStruct )
	{
		if ( 0 == memcmp( this, &cStruct, sizeof(KG3DMESHSKYBOX_DESCSTRUCT) ) )///最后一个nTotal不用比较
		{
			return true;
		}
		return false;			
	}
	bool KG3DMeshSkyBoxDescStruct::operator!=( const KG3DMeshSkyBoxDescStruct &cStruct )
	{
		if ( 0 == memcmp( this, &cStruct, sizeof(KG3DMESHSKYBOX_DESCSTRUCT) ) )///最后一个nTotal不用比较
		{
			return false;
		}
		return true;			
	}
	void KG3DMeshSkyBoxDescStruct::ToBaseStruct( KG3DMESHSKYBOX_DESCSTRUCT* pOtherStruct )
	{
		_ASSERTE(pOtherStruct);
		if ( this == pOtherStruct )
		{
			return;
		}
		KG3DMESHSKYBOX_DESCSTRUCT* pMyBase = this;
		memcpy_s( pOtherStruct, sizeof(KG3DMESHSKYBOX_DESCSTRUCT), pMyBase, sizeof(KG3DMESHSKYBOX_DESCSTRUCT));	
	}
	void KG3DMeshSkyBoxDescStruct::ToKey( KG3DDWGC_Key_SkyBox& OtherStruct )const
	{
		if ( (const void*)this == (const void*)&OtherStruct )
		{
			return;
		}
		KG3DMESHSKYBOX_DESCSTRUCT* pOtherStructBase = &OtherStruct;
		const KG3DMESHSKYBOX_DESCSTRUCT* pMyBase = this;
		memcpy_s( pOtherStructBase, sizeof(KG3DMESHSKYBOX_DESCSTRUCT), pMyBase, sizeof(KG3DMESHSKYBOX_DESCSTRUCT));	
	}
	bool KG3DMeshSkyBoxDescStruct::SaveMapSetting( IIniFile* pIniFile, LPCTSTR pSection )
	{
		_ASSERTE( pIniFile && pSection );
		using KG_CUSTOM_HELPERS::SaveINIFileData;
		SaveINIFileData<D3DCOLOR>(pIniFile, pSection, "TopColor", d3dc_xrgb_TopColor);
		SaveINIFileData<D3DCOLOR>(pIniFile, pSection, "MidColor", d3dc_xrgb_MidColor);
		//SaveINIFileData<D3DCOLOR>(pIniFile, pSection, "BotmColor", d3dc_xrgb_BotmColor);
		
		pIniFile->WriteInteger(pSection, "TopPercent", nTopPercent);
		pIniFile->WriteInteger(pSection, "MidPercent", nMidPercent);
		pIniFile->WriteInteger(pSection, "BotmPercent", nBotmPercent);
		pIniFile->WriteInteger(pSection, "Top_MidBlendPercent", nTop_MidBlendPercent);
		pIniFile->WriteInteger(pSection, "Mid_BotmBlendPercent", nMid_BotmBlendPercent);
		//pIniFile->WriteFloat(pSection, "Radius", fRadius);
		return true;
	}

	bool KG3DMeshSkyBoxDescStruct::LoadMapSetting( IIniFile* pIniFile, LPCSTR pSection )
	{
		_ASSERTE( pIniFile && pSection );
		using KG_CUSTOM_HELPERS::LoadINIFileData;
		LoadINIFileData<D3DCOLOR>(pIniFile, pSection, "TopColor", 0, &d3dc_xrgb_TopColor);
		LoadINIFileData<D3DCOLOR>(pIniFile, pSection, "MidColor", 0, &d3dc_xrgb_MidColor);
		//LoadINIFileData<D3DCOLOR>(pIniFile, pSection, "BotmColor", 0, &d3dc_xrgb_BotmColor);

		pIniFile->GetInteger(pSection, "TopPercent", 0, &nTopPercent);
		pIniFile->GetInteger(pSection, "MidPercent", 0, &nMidPercent);
		pIniFile->GetInteger(pSection, "BotmPercent", 0, &nBotmPercent);
		pIniFile->GetInteger(pSection, "Top_MidBlendPercent", 0, &nTop_MidBlendPercent);
		pIniFile->GetInteger(pSection, "Mid_BotmBlendPercent", 0, &nMid_BotmBlendPercent);
		//pIniFile->GetFloat(pSection, "Radius", 0, &fRadius);
		return true;
	}

	bool KG3DMeshSkyBoxDescStruct::SetDefaultValue()
	{
		///初始化渐变结构
		//ZeroMemory( &m_SkyColorStruct, sizeof(SkyColorStruct) );
		d3dc_xrgb_TopColor = D3DCOLOR_XRGB( 0, 107, 255 );
		d3dc_xrgb_MidColor = D3DCOLOR_XRGB( 0, 209, 255 );
		//d3dc_xrgb_BotmColor = D3DCOLOR_XRGB( 255, 255, 255 );
		nTopPercent = 70;
		nMidPercent = 30;
		nBotmPercent = 100;
		nTop_MidBlendPercent = 70;
		nMid_BotmBlendPercent = 30;
		//fRadius = 50000.f;

		//nTotalPercent = 200;
		return true;
	}

	/************************************************************************/
	//KG3DDWGC_Key_SkyBox 
	/************************************************************************/
	bool KG3DDWGC_Key_SkyBox::CalculateInterp(const KG3DDWGC_KeyBase& BaseKeyBefore
		, const KG3DDWGC_KeyBase& BaseKeyAfter
		//, const KG3DEnvEff_DWGC_Key_Struct_Base& BaseKeyResult
		, float fPercent)
	{
		KG3DDWGC_Key_SkyBox& KeyBefore = (KG3DDWGC_Key_SkyBox&)BaseKeyBefore;
		KG3DDWGC_Key_SkyBox& KeyAfter = (KG3DDWGC_Key_SkyBox&)BaseKeyAfter;
		//DWGC_KG3DMeshSkyBox_Key_Struct& KeyResult = (DWGC_KG3DMeshSkyBox_Key_Struct&)BaseKeyResult;
		if (fPercent <= 0.0 )
		{
			*this = KeyBefore;
			return true;
		}
		else if (fPercent >= 1.0)
		{
			*this = KeyAfter;
			return true;
		}
		TwoColorBlend(KeyBefore.d3dc_xrgb_TopColor, KeyAfter.d3dc_xrgb_TopColor, this->d3dc_xrgb_TopColor, fPercent);
		TwoColorBlend(KeyBefore.d3dc_xrgb_MidColor, KeyAfter.d3dc_xrgb_MidColor, this->d3dc_xrgb_MidColor, fPercent);
		//TwoColorBlend(KeyBefore.d3dc_xrgb_BotmColor, KeyAfter.d3dc_xrgb_BotmColor, this->d3dc_xrgb_BotmColor, fPercent);
		MT_INT_BLEND(KeyBefore.nTopPercent, KeyAfter.nTopPercent, this->nTopPercent, fPercent);
		MT_INT_BLEND(KeyBefore.nMidPercent, KeyAfter.nMidPercent, this->nMidPercent, fPercent);
		MT_INT_BLEND(KeyBefore.nBotmPercent, KeyAfter.nBotmPercent, this->nBotmPercent, fPercent);
		MT_INT_BLEND(KeyBefore.nTop_MidBlendPercent, KeyAfter.nTop_MidBlendPercent, this->nTop_MidBlendPercent, fPercent);
		MT_INT_BLEND(KeyBefore.nMid_BotmBlendPercent, KeyAfter.nMid_BotmBlendPercent, this->nMid_BotmBlendPercent, fPercent);
		///不用担心数据的有效性，在要求更改的时候，TranslateStructToPos会处理非法数据
		return true;
	}
	bool KG3DDWGC_Key_SkyBox::SaveMapSetting( IIniFile* pIniFile, LPCTSTR pSection )
	{
		KG3DMeshSkyBoxDescStruct tempStruct(*this);
		return tempStruct.SaveMapSetting(pIniFile, pSection);
	}
	bool KG3DDWGC_Key_SkyBox::LoadMapSetting( IIniFile* pIniFile, LPCTSTR pSection )
	{
		KG3DMeshSkyBoxDescStruct tempStruct;
		if (false == tempStruct.LoadMapSetting(pIniFile, pSection))
		{
			return false;
		}
		tempStruct.ToKey(*this);
		return true;
	}
	/*bool KG3DDWGC_Key_SkyBox::IsLegal(KG3DDWGC_KeyTableManagerBase*)const
	{
		if (nTopPercent >= 0
			&& nMidPercent >= 0
			&& nBotmPercent >= 0
			&& nTop_MidBlendPercent <= nTopPercent
			&& nMid_BotmBlendPercent <= nMidPercent)
		{
			return true;
		}
		return false;
	}*/
#if defined(DEBUG) | defined(_DEBUG)
	void KG3DDWGC_Key_SkyBox::DebugPrint(std::ostringstream& OSS)
	{
		/*
		_ASSERTE(pDebugStream);
				char czTemp[32];
				sprintf_s(czTemp, 32, "%x; ", d3dc_xrgb_TopColor);
				_tcscat_s(pDebugStream, nSize, czTemp);
				nSize-=(int)strlen(czTemp);*/
		OSS << d3dc_xrgb_TopColor << "; ";
	}
#endif


	
};