#include "StdAfx.h"
#include "KG3DSkinModelRender.h"
#include "kg3dmodel.h"
#include "kg3dmodelst.h"
#include "KG3DShaderManager.h"
#include "KG3DTerrainBlock.h"
#include "KG3DEnvEffLight.h"
#include "kg3dbip.h"
#include "KG3DSubSetRenderer.h"
#include "KG3DTexture.h"
#include "KG3DScenePointLightRender.h"
#include "kg3dtexturetable.h"
#include "KG3DTempleteMultiThead.h"

extern DWORD g_dwRenderCount;
extern BOOL  volatile g_bThreadExit;
extern BOOL    g_ModelProcessThreadLock[8];
TKG3DMultiTheadBuffer<ModelVBFillList>* g_pModelVBFillList;

BOOL g_bModelProcessExit = FALSE;
extern BOOL g_bClient;//是否处于客户端

HRESULT InitModelVBFillList()
{
	HRESULT hr = E_FAIL;
	g_pModelVBFillList = new TKG3DMultiTheadBuffer<ModelVBFillList>[8];
	KGLOG_PROCESS_ERROR(g_pModelVBFillList);

	for (int i=0;i<8;i++)
	{
		g_pModelVBFillList[i].Init();
	}

	hr = S_OK;
Exit0:
	return hr;
}

HRESULT UnInitModelVBFillList()
{
	SAFE_DELETE_ARRAY(g_pModelVBFillList);
	return S_OK;
}

HRESULT g_ModelVertexBufferFillProcess(int nIndex)
{
	ModelVBFillList* pList = NULL;
	ModelVBFillList* pListNext = NULL;

	if(SUCCEEDED(g_pModelVBFillList[nIndex].FindList_DataOK(pList,pListNext)))
	{
		//DWORD dwNowTime = GetTickCount();

		if( pList && pList->m_lpSkinModelRender )
		{
			//KG_PROCESS_ERROR(pListNext->m_dwState != BLOCK_LIST_STATE_DATAOK );
			KG_PROCESS_ERROR(!pList->m_lpSkinModelRender->m_bExitMultiThread);

			while (pList->m_nMultiThreadComputeCount > 0)
			{
				for (DWORD i =0;i<pList->m_lpSkinModelRender->m_dwArrayUse;i++)
				{

					if((int)i % g_cEngineOption.nNumCpuThread == nIndex)
					{
						KG3DSkinCPURender* pRender = pList->m_lpSkinModelRender->m_azSkinModelRender[i];

						if(pRender)
						{
							pRender->AddRef();

							pRender->ComputeSkin();

							pRender->Release();
						}

						KG_PROCESS_ERROR(!pList->m_lpSkinModelRender->m_bExitMultiThread);
					}
					KG_PROCESS_ERROR(!pList->m_lpSkinModelRender->m_bExitMultiThread);
				}
				pList->m_nMultiThreadComputeCount --;
			}		

Exit0:
			pList->m_lpSkinModelRender->Release();
			pList->m_lpSkinModelRender = NULL;
			pList->m_dwState = BLOCK_LIST_STATE_NODATA;


		}
	}
	return S_OK;
}




KG3DSkinCPURender::KG3DSkinCPURender()
{
	m_lpVB1 = NULL;
	m_lpIB = NULL;
	m_lpModel = NULL;

	m_lpVertexSrc = NULL;
	m_lpBuffer1 = NULL;
	m_lpPositionLast = NULL;

	m_dwNumFace = 0;
	m_dwNumVertex = 0;

	m_ulRefCount = 0;

	m_bSkinUpdate = FALSE;
	m_dwLastFillTime = 0;


}

KG3DSkinCPURender::~KG3DSkinCPURender()
{
	UnInit();
}

ULONG KG3DSkinCPURender::AddRef()
{
    return (ULONG)KG_InterlockedIncrement((long *)&m_ulRefCount);
}

ULONG KG3DSkinCPURender::Release()
{
    ULONG uNewRefCount = (ULONG)KG_InterlockedDecrement((long *)&m_ulRefCount);
    if (uNewRefCount == 0)
        delete this; 
    return uNewRefCount;
}

HRESULT KG3DSkinCPURender::Init()
{
	return S_OK;
}

HRESULT KG3DSkinCPURender::UnInit()
{
	SAFE_RELEASE(m_lpModel);

	SAFE_RELEASE(m_lpVB1);
	SAFE_RELEASE(m_lpIB);

	SAFE_DELETE_ARRAY(m_lpVertexSrc);
	SAFE_DELETE_ARRAY(m_lpBuffer1);
	SAFE_DELETE_ARRAY(m_lpPositionLast);

	m_dwNumFace = 0;
	m_dwNumVertex = 0;

	for (size_t i=0;i<m_vecSubRender.size();i++)
	{
		KG3DSkinCPURender* pRender = m_vecSubRender[i];
		SAFE_RELEASE(pRender);
	}
	m_vecSubRender.clear();
	m_bSkinUpdate = FALSE;

	return S_OK;
}

HRESULT KG3DSkinCPURender::CreateFromModel(KG3DModel* pModel,BOOL bSKin)
{
	HRESULT hr = E_FAIL;
	KGLOG_PROCESS_ERROR(pModel);

	//////////////////////////////////////////////////////////////////////////
	UnInit();

	SAFE_RELEASE(m_lpModel);
	m_lpModel = pModel;
	m_lpModel->AddRef();

	DWORD dwModelType = m_lpModel->GetType();

	if(dwModelType == MESHTYPE_DEFAULT)
	{
		//////////////////////////////////////////////////////////////////////////
		KG3DMesh* pMesh = m_lpModel->GetMesh();
		KGLOG_PROCESS_ERROR(pMesh);

		LPD3DXMESH pD3DXMesh = pMesh->GetMesh(KG3DMesh::SMBT_NORMAL);
		KGLOG_PROCESS_ERROR(pD3DXMesh);

		DWORD dwFVF = pD3DXMesh->GetFVF();
		KGLOG_PROCESS_ERROR((dwFVF == D3DFVF_Faces_Normal_Texture1) || (dwFVF == D3DFVF_Faces_Normal_Texture2));

		m_dwNumFace   = pD3DXMesh->GetNumFaces();
		m_dwNumVertex = pD3DXMesh->GetNumVertices();

		m_lpVertexSrc = new VertexFormat[m_dwNumVertex];
		KGLOG_PROCESS_ERROR(m_lpVertexSrc);

		if(bSKin)
		{
			m_lpBuffer1 = new VertexFormat[m_dwNumVertex];
			KGLOG_PROCESS_ERROR(m_lpBuffer1);

			m_lpPositionLast = new D3DXVECTOR3[m_dwNumVertex];
			KGLOG_PROCESS_ERROR(m_lpPositionLast);
		}
		//////////////////////////////////////////////////////////////////////////

		pD3DXMesh->GetIndexBuffer(&m_lpIB);


		if(dwFVF == D3DFVF_Faces_Normal_Texture1)
		{
			VFormat::FACES_NORMAL_TEXTURE1* pVertex = NULL;

			hr = pD3DXMesh->LockVertexBuffer(0,(void**)&pVertex);
			KGLOG_COM_PROCESS_ERROR(hr);

			for (DWORD i=0;i<m_dwNumVertex;i++)
			{
				m_lpVertexSrc[i].p      = pVertex[i].p;
				m_lpVertexSrc[i].Normal = pVertex[i].Normal;
				m_lpVertexSrc[i].tu1    = pVertex[i].tu1;
				m_lpVertexSrc[i].tv1    = pVertex[i].tv1;
				m_lpVertexSrc[i].tIndex = (float)i;

				if(bSKin)
					m_lpPositionLast[i] = pVertex[i].p;
			}
		}
		else
		{
			VFormat::FACES_NORMAL_TEXTURE2* pVertex = NULL;

			hr = pD3DXMesh->LockVertexBuffer(0,(void**)&pVertex);
			KGLOG_COM_PROCESS_ERROR(hr);

			for (DWORD i=0;i<m_dwNumVertex;i++)
			{
				m_lpVertexSrc[i].p      = pVertex[i].p;
				m_lpVertexSrc[i].Normal = pVertex[i].Normal;
				m_lpVertexSrc[i].tu1    = pVertex[i].tu1;
				m_lpVertexSrc[i].tv1    = pVertex[i].tv1;
				m_lpVertexSrc[i].tIndex = (float)i;

				m_lpPositionLast[i] = pVertex[i].p;
			}
		}

		hr = pD3DXMesh->UnlockVertexBuffer();
		KGLOG_COM_PROCESS_ERROR(hr);

		//////////////////////////////////////////////////////////////////////////
		{
			DWORD* pAttriab = NULL;
			WORD*  pIndex = NULL;

			hr = pD3DXMesh->LockAttributeBuffer(0,&pAttriab);
			KGLOG_COM_PROCESS_ERROR(hr);

			hr = pD3DXMesh->LockIndexBuffer(0,(void**)&pIndex);
			if(FAILED(hr))
			{
				pD3DXMesh->UnlockAttributeBuffer();
			}
			KGLOG_COM_PROCESS_ERROR(hr);

			for (DWORD i=0;i<m_dwNumFace;i++)
			{
				DWORD dwTIndex = pAttriab[i];
				for (int k=0;k<3;k++)
				{
					WORD wVertexIndex = pIndex[i*3+k];
					m_lpVertexSrc[wVertexIndex].tIndex = (float)dwTIndex;
				}
			}

			hr = pD3DXMesh->UnlockIndexBuffer();
			if(FAILED(hr))
			{
				pD3DXMesh->UnlockAttributeBuffer();
			}
			KGLOG_COM_PROCESS_ERROR(hr);

			hr = pD3DXMesh->UnlockAttributeBuffer();
			KGLOG_COM_PROCESS_ERROR(hr);
		}

		hr = g_pd3dDevice->CreateVertexBuffer(sizeof(VertexFormat) * m_dwNumVertex,
			D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,D3DFVF_Faces_Normal_Texture1,D3DPOOL_DEFAULT,&m_lpVB1,NULL);
		KGLOG_COM_PROCESS_ERROR(hr);
		//////////////////////////////////////////////////////////////////////////
		if(bSKin)
			memcpy(m_lpBuffer1,m_lpVertexSrc,sizeof(VertexFormat) * m_dwNumVertex);

		FillVertexBuffer(m_lpVertexSrc);

		if(bSKin)
			m_eModelType = MODELTYPE_SKIN;
		else
			m_eModelType = MODELTYPE_STATIC;
	}
	else if(dwModelType == MESHTYPE_MODELST)
	{
		KG3DModelST* pModelST = static_cast<KG3DModelST*>(m_lpModel);
		
		if(0)
		{
			int ncount = pModelST->GetNumModel();
			for (int i =0;i<ncount;i++)
			{
				KG3DModel* pSubModel = NULL;
				if(SUCCEEDED(pModelST->GetModel(i,(IKG3DModel**)&pSubModel)))
				{					
					if(pSubModel)
					{
						DWORD dwShaderType = pSubModel->GetShaderType();
						if(dwShaderType == KG3DMaterial::MAT_SHADER_TYPE_SKIN)
						{
							KG3DSkinCPURender* pNewRender = new KG3DSkinCPURender();
							KGLOG_PROCESS_ERROR(pNewRender);
							pNewRender->Init();

							pNewRender->CreateFromModel(pSubModel,TRUE);

							m_vecSubRender.push_back(pNewRender);
						}
						//////////////////////////////////////////////////////////////////////////
						/*for (std::set<KG3DModel*>::iterator i = pSubModel->m_ChildModels.begin(); i != pSubModel->m_ChildModels.end(); i++)
						{
							KG3DModel* pSSubModel = *i;
							if(pSSubModel->CheckNeedRender())
							{
								KG3DSkinCPURender* pNewRender = new KG3DSkinCPURender();
								KGLOG_PROCESS_ERROR(pNewRender);
								pNewRender->Init();

								pNewRender->CreateFromModel(pSSubModel,FALSE);

								m_vecSubRender.push_back(pNewRender);

							}
						}*/
					}
				}
			}
		}

		/*for (std::set<KG3DModel*>::iterator i = m_lpModel->m_ChildModels.begin(); i != m_lpModel->m_ChildModels.end(); i++)
		{
			KG3DModel* pSubModel = *i;

			if(pSubModel && pSubModel->CheckNeedRender())
			{
				KG3DSkinCPURender* pNewRender = new KG3DSkinCPURender();
				KGLOG_PROCESS_ERROR(pNewRender);
				pNewRender->Init();

				pNewRender->CreateFromModel(pSubModel,TRUE);

				m_vecSubRender.push_back(pNewRender);
			}
		}*/

		m_eModelType = MODELTYPE_MODELST;
	}


	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSkinCPURender::ComputeSkin()
{
	if(m_bSkinUpdate)
		return S_OK;

	if(m_eModelType == MODELTYPE_MODELST)
	{
	}
	else if(m_eModelType == MODELTYPE_SKIN)
	{
		ComputeSkin(m_lpBuffer1);
	}

	m_bSkinUpdate = TRUE;

	return S_OK;
}

HRESULT KG3DSkinCPURender::ComputeSkin(VertexFormat* pDest)
{
	D3DXVECTOR3 zero(0,0,0);
	KGLOG_PROCESS_ERROR(m_lpModel);

	KG3DMesh* pMesh = m_lpModel->GetMesh();
	KGLOG_PROCESS_ERROR(pMesh);

	for (DWORD i=0;i<m_dwNumVertex;i++)
	{
		pDest[i].p = zero;
		pDest[i].Normal = zero;
		pDest[i].tu1 = m_lpVertexSrc[i].tu1;
		pDest[i].tv1 = m_lpVertexSrc[i].tv1;
	}

	//////////////////////////////////////////////////////////////////////////
	//逐个更新骨骼
	for (DWORD i=0;i<pMesh->m_dwNumBone;i++)
	{
		KG3DMesh::BoneInfo1* pBoneInfo = &pMesh->m_pBoneInfo[i];
		D3DXMATRIX Mat = m_lpModel->m_pBoneMatricesForRender[i];
		D3DXMATRIX& MatRev = pMesh->m_pmatReverseMatrix[i];

		//D3DXMatrixInverse(&MatRev,NULL,&pBoneInfo->CurMatrix);

		//////////////////////////////////////////////////////////////////////////
		if(m_lpModel->m_lpParentModelST)
		{
			D3DXMATRIX* pRenderMatrix = m_lpModel->m_lpParentModelST->GetRenderMatrix();
			if(pRenderMatrix)
			{
				if( i <m_lpModel->m_Mesh2Bip.size())
				{
					int nMapedBoneIndex = m_lpModel->m_Mesh2Bip[i];
					if (nMapedBoneIndex != -1)
					{
						KG_PROCESS_ERROR(pRenderMatrix);
						Mat = pRenderMatrix[nMapedBoneIndex];
					}
					else
					{
						Mat = m_lpModel->m_pBoneMatricesForRender[i];
					}	
				}
			}
		}

		Mat = MatRev * Mat;
		//////////////////////////////////////////////////////////////////////////
		for (DWORD k=0;k<pBoneInfo->NumVerticesRef;k++)
		{
			DWORD dwIndex = pBoneInfo->VerticesIndexRef[k];
			float fWeight = pBoneInfo->VerticesWeightRef[k];

			D3DXVECTOR3 vP,vN;
			D3DXVec3TransformCoord (&vP,&m_lpVertexSrc[dwIndex].p     ,&Mat);
			D3DXVec3TransformNormal(&vN,&m_lpVertexSrc[dwIndex].Normal,&Mat);

			pDest[dwIndex].p      += vP * fWeight;
			pDest[dwIndex].Normal += vN * fWeight;
		}		
	}

	if(g_cEngineOption.bFlexBodySmooth)
	{
		KG3DMesh* pMesh= m_lpModel->GetMesh();
		if(pMesh && pMesh->m_lpCpuProcessData)
		{
			for (DWORD i = 0;i<m_dwNumVertex;i++)
			{
				if(i < pMesh->m_lpCpuProcessData->m_dwNumVertex)
				{
					if(pMesh->m_lpCpuProcessData->m_lpVertexInfo[i])
					{
						pDest[i].p = (pDest[i].p - m_lpPositionLast[i]) * 0.1F +  m_lpPositionLast[i];
						m_lpPositionLast[i] = pDest[i].p;
					}
				}
			}
		}
	}

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSkinCPURender::FillVertexBuffer(VertexFormat* pSrc)
{
	HRESULT hr = E_FAIL;

	KGLOG_PROCESS_ERROR(m_lpVB1);

	VertexFormat* pVertex = NULL;
	hr = m_lpVB1->Lock(0,0,(void**)&pVertex,0);
	KGLOG_COM_PROCESS_ERROR(hr);

	memcpy(pVertex,pSrc,sizeof(VertexFormat) * m_dwNumVertex);

	hr = m_lpVB1->Unlock();
	KGLOG_COM_PROCESS_ERROR(hr);

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT g_SetMaterialDetailInfo(LPD3DXEFFECT pEffect,KG3DModel* pModel, int nSubSetID)
{
	//这个函数将来的掉用是挺频繁的，小心其耗时，累积起来就很客观了。尽量减少变量的变换和计算的时间

    HRESULT hr = E_FAIL;
#if defined(DEBUG) | defined(_DEBUG)
	DWORD dwNowTime = timeGetTime();
#endif
    KGLOG_PROCESS_ERROR(pEffect);
    KGLOG_PROCESS_ERROR(pModel);

	float fNowTime = (float)timeGetTime();

    KG3DMaterialDetail* pDetail = pModel->GetDetail(nSubSetID);
    if (!pDetail)
    {
		D3DXVECTOR4 vZero(0,0,0,0);
		KG3DShaderParamDetail detailParam;
		D3DXVECTOR4 vFlow;
		ZeroMemory(vFlow,sizeof(D3DXVECTOR4));
		ZeroMemory(&detailParam, sizeof(detailParam));
		detailParam.vDetailScale = D3DXVECTOR4(1,0,1,0);
		detailParam.vBodyScales = D3DXVECTOR4(1,1,1,1);

		hr = pEffect->SetValue("detailParam", &detailParam, sizeof(detailParam));
		pEffect->SetVector("vFlow",&vFlow);

		/*D3DXVECTOR4 bodyscale(1,1,1,1);
		D3DXVECTOR4 envSpe(0,0,0,0);
		D3DXVECTOR4 detailscale(1,0,1,0);

        pEffect->SetVector("vEnvSpecular",&envSpe);
        pEffect->SetVector("vBodyScales",&bodyscale);
        pEffect->SetVector("vDetailScale",&detailscale);*/

        hr = pEffect->SetTexture("tChannel",NULL);
        hr = pEffect->SetTexture("tDetail0",NULL);
        hr = pEffect->SetTexture("tDetail1",NULL);
        goto Exit1;
    }

	{

		D3DXVECTOR4 envSpe;
		envSpe.x = pDetail->m_fEnv;
		envSpe.y = pDetail->m_fSpecular;
		envSpe.z = envSpe.w = 0;

		D3DXVECTOR4 bodyscale;
		bodyscale.x = pDetail->m_fMainColorScale;
		bodyscale.y = pDetail->m_fMainColorMax;
		bodyscale.z = pDetail->m_fMainColorMin;
		bodyscale.w = 0;

		D3DXVECTOR4 detailscale;
		detailscale.x = pDetail->m_fDetail0UVScale;
		detailscale.y = pDetail->m_fDetail0ColorScale;
		detailscale.z = pDetail->m_fDetail1UVScale;
		detailscale.w = pDetail->m_fDetail1ColorScale;

		D3DXVECTOR4 detailmtl;
		detailmtl.x = pDetail->m_fDetail0Env;
		detailmtl.y = pDetail->m_fDetail0Specular;
		detailmtl.z = pDetail->m_fDetail1Env;
		detailmtl.w = pDetail->m_fDetail1Specular;

		D3DCOLORVALUE& bodycolor = pDetail->m_cMainColor;
		D3DCOLORVALUE& bodycolor1 = pDetail->m_cMainColor1;

		//////////////////////////////////////////////////////////////////////////
		D3DXVECTOR4 vFlow;
		vFlow.x = pDetail->m_fFlow_ColorScale;
		vFlow.y = pDetail->m_fFlow_UVScale;
		vFlow.z = pDetail->m_vFlow_Speed.x * fNowTime *0.001F;
		vFlow.w = pDetail->m_vFlow_Speed.y * fNowTime *0.001F;

		if(pDetail->m_lpFlowTexture)
		{
			LPDIRECT3DTEXTURE9 pT = pDetail->m_lpFlowTexture->GetTexture();
			if(!pT)
			{
				vFlow.x = 0;
				pT = g_cTextureTable.GetNullTexture()->GetTexture();
			}
			pEffect->SetTexture("tFlow",pT);
		}
		else
		{
			pEffect->SetTexture("tFlow",g_cTextureTable.GetNullTexture()->GetTexture());
			vFlow.x = 0;
		}

		pEffect->SetVector("vFlow",&vFlow);
		pEffect->SetVector("vFlowColor",(D3DXVECTOR4*)&pDetail->m_Flow_Color);
		//////////////////////////////////////////////////////////////////////////

		if(pDetail->m_lpChannelTexture)
		{
			LPDIRECT3DTEXTURE9 pT = pDetail->m_lpChannelTexture->GetTexture();
			if(!pT)
			{
				detailscale.y = 0;
				detailscale.w = 0;
				pT = g_cTextureTable.GetNullTexture()->GetTexture();
			}
			pEffect->SetTexture("tChannel",pT);
		}
		else
		{
			pEffect->SetTexture("tChannel",g_cTextureTable.GetConverTexture()->GetTexture());
			detailscale.y = 0;
			detailscale.w = 0;
		}

		if(pDetail->m_lpDetailTexture0)
		{
			LPDIRECT3DTEXTURE9 pT = pDetail->m_lpDetailTexture0->GetTexture();
			pEffect->SetTexture("tDetail0",pT);
			if(!pT)
			{
				detailscale.y = 0;
			}
		}
		else
		{
			pEffect->SetTexture("tDetail0",g_cTextureTable.GetConverTexture()->GetTexture());
			detailscale.y = 0;
		}

		if(pDetail->m_lpDetailTexture1)
		{
			LPDIRECT3DTEXTURE9 pT = pDetail->m_lpDetailTexture1->GetTexture();
			pEffect->SetTexture("tDetail1",pT);
			if(!pT)
			{
				detailscale.w = 0;
			}
		}
		else
		{
			pEffect->SetTexture("tDetail1",g_cTextureTable.GetConverTexture()->GetTexture());
			detailscale.w = 0;
		}

		if(g_bClient)
		{
			int nTable = 0;
			int nDefine = 0;
			pModel->GetDetail(&nTable,&nDefine);
			if(nDefine==0)
			{
				bodycolor.a = 0;
			}	
		}

		//改成一次设入所有参数
		/*pEffect->SetVector("vEnvSpecular",&envSpe);
		pEffect->SetVector("vBodyScales",&bodyscale);
		pEffect->SetVector("vDetailScale",&detailscale);
		pEffect->SetVector("vDetailMtl",&detailmtl);

		pEffect->SetVector("vBodyColor"   ,(D3DXVECTOR4*)&bodycolor   );
		pEffect->SetVector("vBodyColor1"   ,(D3DXVECTOR4*)&bodycolor1 );
		pEffect->SetVector("vDetail0Color",(D3DXVECTOR4*)&pDetail->m_cDetial0Color);
		pEffect->SetVector("vDetail1Color",(D3DXVECTOR4*)&pDetail->m_cDetial1Color);*/

		KG3DShaderParamDetail detailParam;
		detailParam.vDetailScale = detailscale;
		detailParam.vDetailMtl = detailmtl;
		detailParam.vDetail0Color = pDetail->m_cDetial0Color;
		detailParam.vDetail1Color = pDetail->m_cDetial1Color;
		detailParam.vBodyColor = bodycolor;
		detailParam.vBodyColor1 = bodycolor1;
		detailParam.vBodyScales = bodyscale;
		detailParam.vEnvSpecular = envSpe;

		hr = pEffect->SetValue("detailParam", &detailParam, sizeof(detailParam));
	}

Exit1:
    hr = S_OK;
Exit0:

#if defined(DEBUG) | defined(_DEBUG)
	DWORD dwCost = timeGetTime() - dwNowTime;
	if(g_cEngineOption.bEnableTimeOptimizeLog && dwCost >= 5)
	{
		KGLogPrintf(KGLOG_ERR, "TimeOptimize %d g_SetMaterialDetailInfo.",dwCost);
	}
#endif
    return hr;
}

#if 0//<DetailInfoParamMove>
HRESULT g_SetMaterialDetailInfoFast(LPD3DXEFFECT pEffect,KG3DModel* pModel, KG3D_MaterialDetailInfo_ParamHandle *pParamHandle)
{
    HRESULT hr = E_FAIL;
    HRESULT hrRetCode = E_FAIL;
    DWORD dwNowTime = timeGetTime();
    KGLOG_PROCESS_ERROR(pEffect);
    KGLOG_PROCESS_ERROR(pModel);
    KGLOG_PROCESS_ERROR(pParamHandle);

    KG3DMaterialDetail* pDetail = pModel->GetDetail();
    if (!pDetail)
    {
        D3DXVECTOR4 bodyscale(1,1,1,1);
        D3DXVECTOR4 envSpe(0,0,0,0);
        D3DXVECTOR4 detailscale(1,0,1,0);

        KG_SHADER_SET(pEffect, SetVector, vEnvSpecular, &envSpe,   pParamHandle, hrRetCode);
        KG_SHADER_SET(pEffect, SetVector, vBodyScales, &bodyscale, pParamHandle, hrRetCode);
        KG_SHADER_SET(pEffect, SetVector, vDetailScale, &detailscale, pParamHandle, hrRetCode);

        KG_SHADER_SET(pEffect, SetTexture, tChannel, NULL, pParamHandle, hrRetCode);
        KG_SHADER_SET(pEffect, SetTexture, tDetail0, NULL, pParamHandle, hrRetCode);
        KG_SHADER_SET(pEffect, SetTexture, tDetail1, NULL, pParamHandle, hrRetCode);
        goto Exit1;
    }

    {
    D3DXVECTOR4 envSpe;
    envSpe.x = pDetail->m_fEnv;
    envSpe.y = pDetail->m_fSpecular;
    envSpe.z = envSpe.w = 0;

    D3DXVECTOR4 bodyscale;
    bodyscale.x = pDetail->m_fMainColorScale;
    bodyscale.y = pDetail->m_fMainColorMax;
    bodyscale.z = pDetail->m_fMainColorMin;
    bodyscale.w = 0;

    D3DXVECTOR4 detailscale;
    detailscale.x = pDetail->m_fDetail0UVScale;
    detailscale.y = pDetail->m_fDetail0ColorScale;
    detailscale.z = pDetail->m_fDetail1UVScale;
    detailscale.w = pDetail->m_fDetail1ColorScale;

    D3DXVECTOR4 detailmtl;
    detailmtl.x = pDetail->m_fDetail0Env;
    detailmtl.y = pDetail->m_fDetail0Specular;
    detailmtl.z = pDetail->m_fDetail1Env;
    detailmtl.w = pDetail->m_fDetail1Specular;

    D3DCOLORVALUE& bodycolor = pDetail->m_cMainColor;
	D3DCOLORVALUE& bodycolor1 = pDetail->m_cMainColor1;

    if(pDetail->m_lpChannelTexture)
    {
        LPDIRECT3DTEXTURE9 pT = pDetail->m_lpChannelTexture->GetTexture();
        if(!pT)
        {
            detailscale.y = 0;
            detailscale.w = 0;
            pT = g_cTextureTable.GetNullTexture()->GetTexture();
        }
        KG_SHADER_SET(pEffect, SetTexture, tChannel, pT, pParamHandle, hrRetCode);
    }
    else
    {
        KG_SHADER_SET(pEffect, SetTexture, tChannel, g_cTextureTable.GetNullTexture()->GetTexture(), pParamHandle, hrRetCode);
        detailscale.y = 0;
        detailscale.w = 0;
    }

    if(pDetail->m_lpDetailTexture0)
    {
        LPDIRECT3DTEXTURE9 pT = pDetail->m_lpDetailTexture0->GetTexture();
        KG_SHADER_SET(pEffect, SetTexture, tDetail0, pT, pParamHandle, hrRetCode);
        if(!pT)
        {
            detailscale.y = 0;
        }
    }
    else
    {
        KG_SHADER_SET(pEffect, SetTexture, tDetail0, g_cTextureTable.GetConverTexture()->GetTexture(), pParamHandle, hrRetCode);
        detailscale.y = 0;
    }

    if(pDetail->m_lpDetailTexture1)
    {
        LPDIRECT3DTEXTURE9 pT = pDetail->m_lpDetailTexture1->GetTexture();
        KG_SHADER_SET(pEffect, SetTexture, tDetail1, pT, pParamHandle, hrRetCode);
        if(!pT)
        {
            detailscale.w = 0;
        }
    }
    else
    {
        KG_SHADER_SET(pEffect, SetTexture, tDetail1, g_cTextureTable.GetConverTexture()->GetTexture(), pParamHandle, hrRetCode);
        detailscale.w = 0;
    }

    if(g_bClient)
    {
        int nTable = 0;
        int nDefine = 0;
        pModel->GetDetail(&nTable,&nDefine);
        if(nDefine==0)
        {
            bodycolor.a = 0;
        }	
    }

    KG_SHADER_SET(pEffect, SetVector, vEnvSpecular, &envSpe,   pParamHandle, hrRetCode);
    KG_SHADER_SET(pEffect, SetVector, vBodyScales, &bodyscale,   pParamHandle, hrRetCode);
    KG_SHADER_SET(pEffect, SetVector, vDetailScale, &detailscale,   pParamHandle, hrRetCode);
    KG_SHADER_SET(pEffect, SetVector, vDetailMtl, &detailmtl,   pParamHandle, hrRetCode);

    KG_SHADER_SET(pEffect, SetVector, vBodyColor, (D3DXVECTOR4*)&bodycolor,   pParamHandle, hrRetCode);
	KG_SHADER_SET(pEffect, SetVector, vBodyColor1, (D3DXVECTOR4*)&bodycolor1,   pParamHandle, hrRetCode);
    KG_SHADER_SET(pEffect, SetVector, vDetail0Color, (D3DXVECTOR4*)&pDetail->m_cDetial0Color,   pParamHandle, hrRetCode);
    KG_SHADER_SET(pEffect, SetVector, vDetail1Color, (D3DXVECTOR4*)&pDetail->m_cDetial1Color,   pParamHandle, hrRetCode);
    }

Exit1:
    hr = S_OK;
Exit0:

    DWORD dwCost = timeGetTime() - dwNowTime;
    if(g_cEngineOption.bEnableTimeOptimizeLog && dwCost >= 5)
    {
        KGLogPrintf(KGLOG_ERR, "TimeOptimize %d g_SetMaterialDetailInfo.",dwCost);
    }
    return hr;
}
#endif

HRESULT KG3DSkinCPURender::Render(LPD3DXEFFECT pEffect,KG3DSkinModelRender* pModelRender,BOOL bUpdate)
{
	GraphicsStruct::RenderState light(D3DRS_LIGHTING,FALSE);
	GraphicsStruct::RenderState cull (D3DRS_CULLMODE,D3DCULL_CW);
	KGLOG_PROCESS_ERROR(m_lpModel);

	if(m_lpModel->m_bHideFromRenderTools/* || m_lpModel->GetAlpha() <0.0001F*/)
		return S_OK;

	if(m_eModelType == MODELTYPE_MODELST)
	{
		DWORD dwNowTime = GetTickCount();

		//for (size_t i=0;i<m_vecSubRender.size();i++)
		//{
		//	KG3DSkinCPURender* pSubRender = m_vecSubRender[i];

		//	if(bUpdate && m_bSkinUpdate /*&& dwNowTime - m_dwLastFillTime>30*/)
		//	{
		//		if(pSubRender->m_eModelType == MODELTYPE_SKIN)
		//		{
		//			pSubRender->FillVertexBuffer(pSubRender->m_lpBuffer1);
		//			pSubRender->m_bSkinUpdate = FALSE;
		//			pSubRender->m_dwLastFillTime = dwNowTime;
		//		}
		//	}

		//	pSubRender->Render(pEffect,pModelRender,bUpdate);
		//}

		m_bSkinUpdate = FALSE;
		m_dwLastFillTime = dwNowTime;
	}
	else
	{
		KG_PROCESS_ERROR(m_lpVertexSrc);

		int nPass = 0;
		if(m_eModelType == MODELTYPE_SKIN)
		{
			DWORD dwNowTime = GetTickCount();
			if(bUpdate && m_lpModel->IsAnimationed() && m_bSkinUpdate && (dwNowTime - m_dwLastFillTime>33))
			{
				FillVertexBuffer(m_lpBuffer1);

				m_dwLastFillTime = dwNowTime;
				m_bSkinUpdate = FALSE;
			}
		}


		pEffect->SetMatrix("mWorld", &m_lpModel->m_matWorld);

		{
			KG3DMaterial* pMtl = m_lpModel->GetMaterial();
			KG3DMaterial::KG3DMaterialSubset* pSubMtl = NULL;

			g_SetPointLighting(pEffect,m_lpModel->GetPointLightInfo());

			int nTexture  = pMtl->GetNumMaterial();

			if(nTexture<=4)
				nPass = 0;
			else if (nTexture<=8)
				nPass = 1;
			else if(nTexture<=12)
				nPass = 2;

			for (int i=0;i<nTexture;i++)
			{
				pSubMtl = pMtl->GetSubMaterial(i);
				if(pSubMtl)
				{
                    g_SetMaterialDetailInfo(pEffect,m_lpModel, i);
					KG3DTexture* pTexture = pSubMtl->GetSubMeshTexture();
					if(pTexture)
						pTexture->SetTexture(g_pd3dDevice,i);

					g_pd3dDevice->SetSamplerState(i,D3DSAMP_MAGFILTER,(D3DTEXTUREFILTERTYPE)g_cEngineOption.nSampMagFilter);
					g_pd3dDevice->SetSamplerState(i,D3DSAMP_MINFILTER,(D3DTEXTUREFILTERTYPE)g_cEngineOption.nSampMinFilter);
					g_pd3dDevice->SetSamplerState(i,D3DSAMP_MIPFILTER,(D3DTEXTUREFILTERTYPE)g_cEngineOption.nSampMipFilter);
					g_pd3dDevice->SetSamplerState(i, D3DSAMP_MAXANISOTROPY ,g_cEngineOption.dwMaxAnisotropy);
				}
			}
		}

		g_pd3dDevice->SetVertexDeclaration(pModelRender->m_lpVertexDeclaration);
		g_pd3dDevice->SetStreamSource(0,m_lpVB1,0,sizeof(VertexFormat));
		g_pd3dDevice->SetIndices(m_lpIB);
		pEffect->CommitChanges();
		pEffect->BeginPass(nPass);

		g_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,m_dwNumVertex,0,m_dwNumFace);

		pEffect->EndPass();
	}

	return S_OK;
Exit0:
	return E_FAIL;
}

//////////////////////////////////////////////////////////////////////////

KG3DSkinModelRender::KG3DSkinModelRender(void)
{
	ZeroMemory(m_azSkinModelRender,sizeof(KG3DSkinCPURender*)*MAXSKINMODEL);
	m_dwArrayUse = 0;
	m_ulRefCount = 0;
	m_bExitMultiThread = FALSE;

	m_bDataChange = FALSE;

	m_lpVertexDeclaration = NULL;
}

KG3DSkinModelRender::~KG3DSkinModelRender(void)
{
	UnInit();
}


HRESULT KG3DSkinModelRender::AddModel(KG3DModel* pModel)
{
	KGLOG_PROCESS_ERROR(pModel);

	ModelContaner* pCon = &m_vStand;

	{

		map<KG3DModel*,KG3DSkinCPURender*>::iterator ifind = pCon->m_mapCPURender.find(pModel);
		if (ifind== pCon->m_mapCPURender.end())
		{
			KG3DSkinCPURender* pNewRender = new KG3DSkinCPURender;
			KGLOG_PROCESS_ERROR(pNewRender);

			pNewRender->AddRef();

			pNewRender->Init();

			DWORD dwModelType = pModel->GetType();
			if (dwModelType == MESHTYPE_MODELST)
			{
				pNewRender->CreateFromModel(pModel,TRUE);
			}
			else
			{
				DWORD dwShaderType = pModel->GetShaderType();

				if(dwShaderType == KG3DMaterial::MAT_SHADER_TYPE_SKIN)
					pNewRender->CreateFromModel(pModel,TRUE);
				else
					pNewRender->CreateFromModel(pModel,FALSE);
			}

			pCon->m_mapCPURender[pModel] = pNewRender;

			PutRenderToArray(pNewRender);
		}

	}

	pModel->SetVisibleCount(g_dwRenderCount);

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSkinModelRender::UnInit()
{
	ModelContaner* pCon = &m_vStand;

	map<KG3DModel*,KG3DSkinCPURender*>::iterator i = pCon->m_mapCPURender.begin();
	map<KG3DModel*,KG3DSkinCPURender*>::iterator iend = pCon->m_mapCPURender.end();
	while (i!=iend)
	{
		//KG3DModel* pModel = i->first;
		KG3DSkinCPURender* pRender = i->second;
	
		SAFE_RELEASE(pRender);
	
		i++;
	}
	pCon->m_mapCPURender.clear();

	{
		for (DWORD i=0;i<m_dwArrayUse;i++)
		{
			if(m_azSkinModelRender[i])
			{
				m_azSkinModelRender[i]->Release();
			}
		}
	}

	ZeroMemory(m_azSkinModelRender,sizeof(KG3DSkinCPURender*)*MAXSKINMODEL);
	m_dwArrayUse = 0;

	SAFE_RELEASE(m_lpVertexDeclaration);
	m_bDataChange = FALSE;
	return S_OK;
}


HRESULT KG3DSkinModelRender::PutRenderToArray(KG3DSkinCPURender* pRender)
{
	for (DWORD i=0;i<m_dwArrayUse;i++)
	{
		if(!m_azSkinModelRender[i])
		{
			m_azSkinModelRender[i] = pRender;
			pRender->AddRef();
			return S_OK;
		}
	}

	KGLOG_PROCESS_ERROR(m_dwArrayUse <= MAXSKINMODEL -1);

	pRender->AddRef();
	m_azSkinModelRender[m_dwArrayUse] = pRender;
	m_dwArrayUse++;

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSkinModelRender::RemoveRenderFromArray(KG3DSkinCPURender* pRender)
{
	for (DWORD i=0;i<m_dwArrayUse;i++)
	{
		if( m_azSkinModelRender[i] == pRender)
		{
			m_azSkinModelRender[i] = NULL;
			pRender->Release();
			return S_OK;
		}
	}

	return E_FAIL;
}

HRESULT KG3DSkinModelRender::FrameMove(KG3DScenePointLightRender* pLights)
{
	DWORD dwUnVisible = 220;

	BOOL bTest = (g_dwRenderCount % 10 == 0);

	if(bTest)
	{
		ModelContaner* pCon = &m_vStand;

		{
			map<KG3DModel*,KG3DSkinCPURender*>::iterator i = pCon->m_mapCPURender.begin();
			map<KG3DModel*,KG3DSkinCPURender*>::iterator iend = pCon->m_mapCPURender.end();
			while (i!=iend)
			{
				KG3DModel* pModel = i->first;
				KG3DSkinCPURender* pRender = i->second;

				if(g_dwRenderCount - pModel->GetVisibleCount() > dwUnVisible)
				{
					RemoveRenderFromArray(pRender);

					SAFE_RELEASE(pRender);

					i = pCon->m_mapCPURender.erase(i);

					m_bDataChange = TRUE;
				}
				else
				{
					pLights->FindRefrenceLightForModel(pModel);
					i++;
				}
			}
		}
	}
	
	//if(m_bDataChange)
	for (int i=0;i<g_cEngineOption.nNumCpuThread;i++)
	{
		ModelVBFillList* pList = NULL;
		ModelVBFillList* pListNext = NULL;

		if(SUCCEEDED(g_pModelVBFillList[i].FindList_NoData(pList,pListNext)))
		{
			if(pList)
			{
				SAFE_RELEASE(pList->m_lpSkinModelRender);
				pList->m_lpSkinModelRender = this;
				pList->m_lpSkinModelRender->AddRef();
				pList->m_nMultiThreadComputeCount = 100;
				pList->m_dwState = BLOCK_LIST_STATE_DATAOK;
			}
		}
		m_bDataChange = FALSE;
	}
	return S_OK;
}

HRESULT KG3DSkinModelRender::RenderForDepth()
{
	ModelContaner* pCon = &m_vStand;

	LPD3DXEFFECT pEffect = pCon->m_vShader.GetEffect();
	D3DXMATRIX matView, matProj, matViewProj;
	g_pd3dDevice->GetTransform(D3DTS_VIEW, &matView);
	g_pd3dDevice->GetTransform(D3DTS_PROJECTION, &matProj);
	matViewProj = matView * matProj;
	pEffect->SetMatrix("mViewProj", &matViewProj);

	UINT uPass = 0;

	pEffect->SetTechnique("TechZ");
	pEffect->Begin(&uPass,0);
	//////////////////////////////////////////////////////////////////////////
	map<KG3DModel*,KG3DSkinCPURender*>::iterator i = pCon->m_mapCPURender.begin();
	map<KG3DModel*,KG3DSkinCPURender*>::iterator iend = pCon->m_mapCPURender.end();
	while (i!=iend)
	{
		KG3DModel* pModel = i->first;
		KG3DSkinCPURender* pRender = i->second;

		if(!pModel->m_bHideFromRenderTools && pModel->m_bVisible /*&& pModel->GetAlpha()>=1.0F*/)
			pRender->Render(pEffect,this,TRUE);

		i++;
	}

	//////////////////////////////////////////////////////////////////////////
	pEffect->End();
	return S_OK;
}

HRESULT KG3DSkinModelRender::Render(BOOL bUpdate)
{
	ModelContaner* pCon = &m_vStand;

	LPD3DXEFFECT pEffect = pCon->m_vShader.GetEffect();
	D3DXMATRIX matView, matProj, matViewProj;
	g_pd3dDevice->GetTransform(D3DTS_VIEW, &matView);
	g_pd3dDevice->GetTransform(D3DTS_PROJECTION, &matProj);
	matViewProj = matView * matProj;
	pEffect->SetMatrix("mViewProj", &matViewProj);

	UINT uPass = 0;

	/*KG3DTerrainConverMap* pConverMap = g_cGraphicsTool.GetCurrentConverMap();
	if (pConverMap)
	{
		pEffect->SetVector("vConverRect",&pConverMap->GetRect());
		pEffect->SetTexture("tConver",pConverMap->GetRenderTargetTex());
		}*///<ConverMap Shared>

	//g_SetShadowInfo(pEffect);//<ShadowParamMoveToShared>
	if(0)
		pEffect->SetTechnique("Tech");
	else
		pEffect->SetTechnique("Tech_Channel");

	pEffect->Begin(&uPass,0);
	//////////////////////////////////////////////////////////////////////////
	map<KG3DModel*,KG3DSkinCPURender*>::iterator i = pCon->m_mapCPURender.begin();
	map<KG3DModel*,KG3DSkinCPURender*>::iterator iend = pCon->m_mapCPURender.end();
	while (i!=iend)
	{
		KG3DModel* pModel = i->first;
		KG3DSkinCPURender* pRender = i->second;

		if(!pModel->m_bHideFromRenderTools && pModel->m_bVisible /*&& pModel->GetAlpha()>=1.0F*/)
			pRender->Render(pEffect,this,bUpdate);

		i++;
	}

	//////////////////////////////////////////////////////////////////////////
	pEffect->End();
	return S_OK;
}


ULONG KG3DSkinModelRender::AddRef()
{
    return (ULONG)KG_InterlockedIncrement((long *)&m_ulRefCount);
}

ULONG KG3DSkinModelRender::Release()
{
    ULONG uNewRefCount = (ULONG)KG_InterlockedDecrement((long *)&m_ulRefCount);
    if (uNewRefCount == 0)
        delete this; 
    return uNewRefCount;
}

HRESULT KG3DSkinModelRender::Init()
{
	HRESULT hr = S_OK;

	ModelContaner* pCon = &m_vStand;

	D3DVERTEXELEMENT9 VertexElement[] = {
		{ 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_POSITION,  0 },
		{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_NORMAL,    0 },
		{ 0, 24, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD,  0 },
		D3DDECL_END()
	};

	hr = g_pd3dDevice->CreateVertexDeclaration(VertexElement, &m_lpVertexDeclaration);
	KGLOG_COM_PROCESS_ERROR(hr);

	if(g_cEngineOption.bEnableNormalMap)
	{
		TCHAR ShaderPath[] = _T("data\\public\\shader\\MultiTextureSkin_normalmap.fx.fx");
		pCon->m_vShader = g_GetShaderManager().Create(g_pd3dDevice, ShaderPath);
	}
	else
	{
		TCHAR ShaderPath[] = _T("data\\public\\shader\\MultiTextureSkin.fx");
		pCon->m_vShader = g_GetShaderManager().Create(g_pd3dDevice, ShaderPath);
	}

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSkinModelRender::GetAllModels(vector<IEKG3DModel*>* pvecModels)
{
	HRESULT hr = E_FAIL;
	KGLOG_PROCESS_ERROR(pvecModels);
	ModelContaner* pCon = &m_vStand;

	{
	
		map<KG3DModel*,KG3DSkinCPURender*>::iterator i = pCon->m_mapCPURender.begin();
		map<KG3DModel*,KG3DSkinCPURender*>::iterator iend = pCon->m_mapCPURender.end();
		while (i!=iend)
		{
			KG3DModel* pModel = i->first;

			pvecModels->push_back(pModel);

			i++;
		}
	
	}

	hr = S_OK;
Exit0:
	return hr;
}

HRESULT KG3DSkinModelRender::AutoReleadModifiedResource()
{
	return S_OK;
}
/////////////////////////////////////////////////////////////////////////////

KG3DSkinShaderRender::KG3DSkinShaderRender()
{

}

KG3DSkinShaderRender::~KG3DSkinShaderRender()
{

}

HRESULT KG3DSkinShaderRender::Init()
{
    m_SingleSubMeshSkinModels.reserve(512);
    m_MultiSubMeshSkinModels.reserve(512);

    return S_OK;
}

HRESULT KG3DSkinShaderRender::UnInit()
{
    return S_OK;
}

HRESULT KG3DSkinShaderRender::Clear()
{
    m_SingleSubMeshSkinModels.clear();
    m_MultiSubMeshSkinModels.clear();

    return S_OK;
}

HRESULT KG3DSkinShaderRender::InputModel(KG3DModel *pModel)
{
    HRESULT hrResult  = E_FAIL;
    KG3DMesh *pMesh = NULL;
    KG3DMaterial *pMaterail = NULL;
    D3DXBONECOMBINATION *pBoneComb = NULL;
    size_t uIndex = 0;
    float fIlluminationLevel = 0.0f;

    KGLOG_PROCESS_ERROR(pModel);

    pMesh = pModel->GetMesh();
	KG_ASSERT_EXIT(pMesh);

    pMaterail = pModel->GetMaterial();
	KG_ASSERT_EXIT(pMaterail);

    pBoneComb = static_cast<D3DXBONECOMBINATION *>(pMesh->m_pBoneCombinationTable->GetBufferPointer());;
    KG_PROCESS_ERROR(pBoneComb);

    fIlluminationLevel = pModel->GetIlluminationLevel();
    if (pMesh->m_dwNumAttributeGroups == 1 && fIlluminationLevel <= 1.0f)
    {
        uIndex = m_SingleSubMeshSkinModels.size();
        m_SingleSubMeshSkinModels.resize(uIndex + 1);

        m_SingleSubMeshSkinModels[uIndex].dwMaterialID = pMaterail->m_dwID;
        m_SingleSubMeshSkinModels[uIndex].dwSubMaterialIndex = pBoneComb[0].AttribId;
        m_SingleSubMeshSkinModels[uIndex].pModel = pModel;
    }
    else
    {
        uIndex = m_MultiSubMeshSkinModels.size();
        m_MultiSubMeshSkinModels.resize(uIndex + 1);

        m_MultiSubMeshSkinModels[uIndex].dwMaterialID = pMaterail->m_dwID;
        m_MultiSubMeshSkinModels[uIndex].dwSubMaterialIndex = pBoneComb[0].AttribId;
        m_MultiSubMeshSkinModels[uIndex].pModel = pModel;
    }

    ASSERT(pModel->GetAlpha() == 1.0f);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

BOOL SortCompare(KG3DSkinShaderRender::SKIN_MODEL_INFO& rA, KG3DSkinShaderRender::SKIN_MODEL_INFO& rB)
{
    if (rA.dwMaterialID == rB.dwMaterialID)
        return (rA.dwSubMaterialIndex < rB.dwSubMaterialIndex);
    else
        return (rA.dwMaterialID < rB.dwMaterialID);
}

HRESULT KG3DSkinShaderRender::Sort()
{
    sort(m_SingleSubMeshSkinModels.begin(), m_SingleSubMeshSkinModels.end(), SortCompare);

    return S_OK;
}

HRESULT KG3DSkinShaderRender::RenderForDepth()
{
    HRESULT hrResult = E_FAIL;

    if (!m_SingleSubMeshSkinModels.empty())
    {
        m_SingleSubMeshSkinModels[0].pModel->Shader_RenderSkinMeshDepth_Begin();
        for (size_t i = 0 ; i < m_SingleSubMeshSkinModels.size(); ++i)
        {
			if(!m_SingleSubMeshSkinModels[i].pModel->m_bHideFromRenderTools )
				m_SingleSubMeshSkinModels[i].pModel->Shader_RenderSkinMeshDepth_NoChange();
        }
        m_SingleSubMeshSkinModels[0].pModel->Shader_RenderSkinMeshDepth_End();
    }

    if (!m_MultiSubMeshSkinModels.empty())
    {
        m_MultiSubMeshSkinModels[0].pModel->Shader_RenderSkinMeshDepth_Begin();
        for (size_t i = 0 ; i < m_MultiSubMeshSkinModels.size(); ++i)
        {
			if(!m_MultiSubMeshSkinModels[i].pModel->m_bHideFromRenderTools  )
				m_MultiSubMeshSkinModels[i].pModel->Shader_RenderSkinMeshDepth_NoChange();
        }
        m_MultiSubMeshSkinModels[0].pModel->Shader_RenderSkinMeshDepth_End();
    }

//Exit1:
    hrResult = S_OK;
//Exit0:
    return hrResult;
}

BOOL bTestM = FALSE;

HRESULT KG3DSkinShaderRender::Render(BOOL bJustNear)
{
    HRESULT hrResult  = E_FAIL;
    HRESULT hrRetCode = E_FAIL;
    KG3DModel *pModel = NULL;
    KG3DMaterial *pMaterial = NULL;
    KG3DMaterial::KG3DMaterialSubset *pSubMaterial = NULL;
    DWORD dwLastMaterialID = 0xFFFFFFFF;
    DWORD dwLastSubMaterialIndex = 0xFFFFFFFF;
    KG3DRenderState RenderState;
    KG3DMaterial::KG3DMaterialSubset *pLastSubMaterial = NULL;

    if (!m_SingleSubMeshSkinModels.empty())
    {
        m_SingleSubMeshSkinModels[0].pModel->Shader_RenderSkinMesh_Begin(TRUE);
        for (size_t i = 0; i < m_SingleSubMeshSkinModels.size(); ++i)
        {
            pModel = m_SingleSubMeshSkinModels[i].pModel;

            // if material changed, reset it
            if (
                (i == 0) ||
                (m_SingleSubMeshSkinModels[i].dwMaterialID != dwLastMaterialID) ||
                (m_SingleSubMeshSkinModels[i].dwSubMaterialIndex != dwLastSubMaterialIndex)
            )
            {
                pMaterial = pModel->GetMaterial();
                KGLOG_PROCESS_ERROR(pMaterial);

                if (!pMaterial->IsLoaded())
                {
                    continue;
                }

                if (pLastSubMaterial)
                {
                    pLastSubMaterial->RestoreMaterial();
                    pLastSubMaterial = NULL;
                }

                // set material and const params in shader
                pSubMaterial = pMaterial->GetSubMaterial(m_SingleSubMeshSkinModels[i].dwSubMaterialIndex);

                hrRetCode = _SetMaterial(pSubMaterial);
                KGLOG_COM_PROCESS_ERROR(hrRetCode);

                pLastSubMaterial = pSubMaterial;
                dwLastMaterialID = m_SingleSubMeshSkinModels[i].dwMaterialID;
                dwLastSubMaterialIndex = m_SingleSubMeshSkinModels[i].dwSubMaterialIndex;
            }

			BOOL bRenderModel = TRUE;
			if(bJustNear)
				if(pModel->m_fLOD<0.8)
					bRenderModel = FALSE;

			if(!pModel->m_bHideFromRenderTools && bRenderModel)
				pModel->Shader_RenderSkinMesh_SingleSubset();        
        }
        m_SingleSubMeshSkinModels[0].pModel->Shader_RenderSkinMesh_End();
    }

    if (pLastSubMaterial)
    {
        pLastSubMaterial->RestoreMaterial();
        pLastSubMaterial = NULL;
    }

    KG_PROCESS_SUCCESS(m_MultiSubMeshSkinModels.empty());

    m_MultiSubMeshSkinModels[0].pModel->Shader_RenderSkinMesh_Begin(TRUE);

    for (size_t i = 0; i < m_MultiSubMeshSkinModels.size(); ++i)
    {
        pModel = m_MultiSubMeshSkinModels[i].pModel;
        ASSERT(pModel);

		BOOL bRenderModel = TRUE;
		if(bJustNear)
			if(pModel->m_fLOD<0.8)
				bRenderModel = FALSE;

		if(!pModel->m_bHideFromRenderTools && bRenderModel)
			pModel->Shader_RenderSkinMesh_NoChange();
    }

    m_MultiSubMeshSkinModels[0].pModel->Shader_RenderSkinMesh_End();

Exit1:
    hrResult = S_OK;
Exit0:
    if (pLastSubMaterial)
    {
        pLastSubMaterial->RestoreMaterial();
        pLastSubMaterial = NULL;
    }

    return hrResult;
}

HRESULT KG3DSkinShaderRender::_SetMaterial(KG3DMaterial::KG3DMaterialSubset *pSubMaterial)
{
    HRESULT hrResult = E_FAIL;
    HRESULT hrRetCode = E_FAIL;
    LPD3DXEFFECT pSkinShader = NULL;
    void *pvShanderParamHandleBuffer = NULL;
    KG3D_DEFST_SKINMESH_ParamHandle *pParamHandle = NULL;
    unsigned uParamSize = 0;
    _MtlAlphaOperation* pAlphaOp = NULL;
    _MtlRenderTwiceAlphaOption *pTwiceOption = NULL;
    BOOL bEnableAlphaFactor = FALSE;
    D3DMATERIAL9 materialCurrent;
    KG3DShaderParamMaterialEx materialEx;
	KGLOG_PROCESS_ERROR(pSubMaterial);
    
    pSkinShader = g_cGraphicsTool.GetDefaultShader(DEFST_SKINMESH);
    KGLOG_PROCESS_ERROR(pSkinShader);

    pvShanderParamHandleBuffer = g_cGraphicsTool.GetDefaultShaderParamHandleBuffer(DEFST_SKINMESH, &uParamSize);
    KGLOG_PROCESS_ERROR(pvShanderParamHandleBuffer);
    KGLOG_PROCESS_ERROR(uParamSize == sizeof(KG3D_DEFST_SKINMESH_ParamHandle));

    pParamHandle = (KG3D_DEFST_SKINMESH_ParamHandle *)pvShanderParamHandleBuffer;
    ASSERT(pParamHandle);

    pAlphaOp = pSubMaterial->GetAlphaOperation();
    KGLOG_PROCESS_ERROR(pAlphaOp);

    pTwiceOption = pSubMaterial->GetAlphaRenderTwice();
    KGLOG_PROCESS_ERROR(pTwiceOption);

    bEnableAlphaFactor = (!(pAlphaOp->EnableAlphaTest || pAlphaOp->EnableAlphaBlend));
    bEnableAlphaFactor = pTwiceOption->bEnable ? FALSE : bEnableAlphaFactor;

    KG_SHADER_SET(pSkinShader, SetBool, bAlphaUseFactor, bEnableAlphaFactor, pParamHandle, hrRetCode);

    // 进渲染队列里的model的alpha值一定为1.0f, 
    // 高亮的model放到m_MultiSubMeshSkinModels, 走原有未排序的渲染流程
    hrRetCode = pSubMaterial->SetMaterial(1.0f, 0);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);
    
    g_pd3dDevice->GetMaterial(&materialCurrent);

    materialCurrent.Power = pSubMaterial->m_fSpecPower;
    KG_SHADER_SET_VALUE(pSkinShader, SetValue, materialCur, &materialCurrent, sizeof(materialCurrent), pParamHandle, hrRetCode);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

unsigned KG3DSkinShaderRender::GetModelCount()
{
    return (unsigned)(m_SingleSubMeshSkinModels.size() + m_MultiSubMeshSkinModels.size());
}

HRESULT KG3DSkinModelRender::OnLostDevice()
{
	ModelContaner* pCon = &m_vStand;

	map<KG3DModel*,KG3DSkinCPURender*>::iterator i = pCon->m_mapCPURender.begin();
	map<KG3DModel*,KG3DSkinCPURender*>::iterator iend = pCon->m_mapCPURender.end();
	while (i!=iend)
	{
		//KG3DModel* pModel = i->first;
		KG3DSkinCPURender* pRender = i->second;

		SAFE_RELEASE(pRender);

		i++;
	}
	pCon->m_mapCPURender.clear();

	{
		for (DWORD i=0;i<m_dwArrayUse;i++)
		{
			if(m_azSkinModelRender[i])
			{
				m_azSkinModelRender[i]->Release();
			}
		}
	}

	ZeroMemory(m_azSkinModelRender,sizeof(KG3DSkinCPURender*)*MAXSKINMODEL);
	m_dwArrayUse = 0;

	return S_OK;
}

HRESULT KG3DSkinModelRender::OnResetDevice()
{
	return S_OK;
}