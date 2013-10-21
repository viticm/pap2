////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DGraphicsStructEx.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2007-6-22 16:58:33
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "KG3DGraphicsStructEx.h"
#include "KG3DGraphicsTool.h"
#include "KG3DCamera.h"
#include "KG3DModelST.h"

namespace GraphicsStruct
{
	//////////////////////////////////////////////////////////////////////////
	RenderStateAlphaWithoutSettingAlphaTest::RenderStateAlphaWithoutSettingAlphaTest( DWORD AlphaBlendEnable /*= TRUE*/,DWORD SrcBlend /*= D3DBLEND_SRCALPHA*/, DWORD DestBlend /*= D3DBLEND_INVSRCALPHA*/ )
	{
		g_pd3dDevice->GetRenderState(D3DRS_ALPHABLENDENABLE,&m_dwBackAlphaBlendEnable);
		g_pd3dDevice->GetRenderState(D3DRS_SRCBLEND,&m_dwBackSrcBlend);
		g_pd3dDevice->GetRenderState(D3DRS_DESTBLEND,&m_dwBackDestlend);
		g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, AlphaBlendEnable);
		g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, SrcBlend);
		g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, DestBlend);

		//g_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	}

	RenderStateAlphaWithoutSettingAlphaTest::~RenderStateAlphaWithoutSettingAlphaTest()
	{
		g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,m_dwBackAlphaBlendEnable);
		g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND,m_dwBackSrcBlend);
		g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND,m_dwBackDestlend);
	}
	//////////////////////////////////////////////////////////////////////////
	//extern KG3DGraphicsTool g_cGraphicsTool;

	RenderStateGlobleCameraZFar::RenderStateGlobleCameraZFar( FLOAT fZFar /*= -FLT_EPSILON*/ )
	{
		g_pd3dDevice->GetTransform(D3DTS_PROJECTION, &m_matProj);
		D3DXMATRIX matTemp;
		if (fZFar > 0)
		{
			KG3DCamera* pCamera = g_cGraphicsTool.GetCamera();
			if(!pCamera)
				return;
			FLOAT fCamFov, fCamAspect, fCamZNear, fCamZFar;
			pCamera->GetPerspective( &fCamFov
				, &fCamAspect
				, &fCamZNear
				, &fCamZFar 
				);
			
			D3DXMatrixPerspectiveFovLH(&matTemp, fCamFov, fCamAspect, fCamZNear, fZFar > 0 ? fZFar : FLT_MAX/2);
		}
		else
		{
			//默认的版本下，根据DX的投射矩阵公式：(参考《3D数学基础：图形与游戏开发》)
			/*
			ZoomX, 0, 0, 0,
			0, ZoomY, 0, 0,
			0, 0, f/(f-n), 1,
			0, 0, n*f/(n-f), 0,

			其中当f趋向无限，_33~1,而34~n
			*/
			matTemp = m_matProj;
			matTemp._33 = 1 + FLT_EPSILON;
			matTemp._43 = -(1 + FLT_EPSILON);
		}
		g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &matTemp);
		
	}

	RenderStateGlobleCameraZFar::~RenderStateGlobleCameraZFar()
	{
		g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &m_matProj);
	}
	//////////////////////////////////////////////////////////////////////////
	RenderStateWorldMatrixMoveWithCamera::RenderStateWorldMatrixMoveWithCamera()
	{
		KG3DCamera* pCamera = g_cGraphicsTool.GetCamera();
		if(!pCamera)
			return;
		D3DXVECTOR3 vecCameraPos;
		pCamera->GetPosition(&vecCameraPos);
		g_pd3dDevice->GetTransform( D3DTS_WORLD, &m_matWorldSave);
		D3DXMATRIX TMatrix = m_matWorldSave;
		TMatrix._41 =  vecCameraPos.x;
		TMatrix._43 =  vecCameraPos.z;
		g_pd3dDevice->SetTransform( D3DTS_WORLD, &TMatrix);
	}
	RenderStateWorldMatrixMoveWithCamera::~RenderStateWorldMatrixMoveWithCamera()
	{
		g_pd3dDevice->SetTransform(D3DTS_WORLD, &m_matWorldSave);
	}
	//////////////////////////////////////////////////////////////////////////
	SamplerStateAddressVClamp::SamplerStateAddressVClamp(DWORD dwSampler /*= 0*/)
		:m_SamplerState(dwSampler, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP)
	{
		
	}

	SamplerStateAddressVClamp::~SamplerStateAddressVClamp()
	{
		
	}
	//////////////////////////////////////////////////////////////////////////
	MaterialWhite::MaterialWhite()
	{
		static const D3DMATERIAL9 TempWhiteMaterial = 
		{ { 1.0f, 1.0f, 1.0f, 1.0f, }/*Diffuse*/
		, { 1.0f, 1.0f, 1.0f, 1.0f, }/*Ambient*/
		, { 0.0f, 0.0f, 0.0f, 0.0f, }/*Specular*/
		, { 0.0f, 0.0f, 0.0f, 0.0f, }/*Emmisive*/
		};

		g_pd3dDevice->GetMaterial(&m_Material);
		g_pd3dDevice->SetMaterial(&TempWhiteMaterial);
	}

	MaterialWhite::~MaterialWhite()
	{
		g_pd3dDevice->SetMaterial(&m_Material);
	}

	LightDisable::LightDisable( DWORD nWhichLight )
		:m_nIndex(nWhichLight)
		,m_Value(FALSE)
	{
		g_pd3dDevice->GetLightEnable(nWhichLight, &m_Value);
		g_pd3dDevice->LightEnable(nWhichLight, FALSE);
	}

	LightDisable::~LightDisable()
	{
		g_pd3dDevice->LightEnable(m_nIndex, m_Value);
	}

	RenderTargetSave::RenderTargetSave( DWORD uIndex )
		:m_pTarget(NULL)
		,m_uIndex(uIndex)
	{
		g_pd3dDevice->GetRenderTarget(uIndex, &m_pTarget);
		g_pd3dDevice->GetViewport(&m_ViewportSave);
	}

	RenderTargetSave::~RenderTargetSave()
	{
		g_pd3dDevice->SetRenderTarget(m_uIndex, m_pTarget);
		SAFE_RELEASE(m_pTarget);

		g_pd3dDevice->SetViewport(&m_ViewportSave);//如果SetRenderTarget。一定要还原Viewport			
	}

	DelpthStencilSave::DelpthStencilSave()
		:m_pDelpthStencil(NULL)
	{
		g_pd3dDevice->GetDepthStencilSurface(&m_pDelpthStencil);
	}

	DelpthStencilSave::~DelpthStencilSave()
	{
		g_pd3dDevice->SetDepthStencilSurface(m_pDelpthStencil);
		SAFE_RELEASE(m_pDelpthStencil);
	}

	TextureDisable::TextureDisable( DWORD Index )
		:m_pTexSave(NULL)
		,m_uIndex(Index)
	{
		g_pd3dDevice->GetTexture(Index, &m_pTexSave);
		g_pd3dDevice->SetTexture(Index, NULL);
	}

	TextureDisable::~TextureDisable()
	{
		if (m_pTexSave)
		{
			g_pd3dDevice->SetTexture(m_uIndex, m_pTexSave);
			SAFE_RELEASE(m_pTexSave);
		}
	}

	StencilEnable::StencilEnable()
	{
		g_pd3dDevice->SetRenderState(D3DRS_STENCILENABLE, TRUE);
	}

	StencilEnable::~StencilEnable()
	{
		g_pd3dDevice->SetRenderState(D3DRS_STENCILENABLE, FALSE);
	}

	ColorWriteDisable::ColorWriteDisable()
	{
		g_pd3dDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0);
	}

	ColorWriteDisable::~ColorWriteDisable()
	{
		g_pd3dDevice->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED 
			| D3DCOLORWRITEENABLE_GREEN
			| D3DCOLORWRITEENABLE_BLUE
			| D3DCOLORWRITEENABLE_ALPHA);
	}

	StencilTwoSidedEnable::StencilTwoSidedEnable()
	{
		g_pd3dDevice->SetRenderState(D3DRS_TWOSIDEDSTENCILMODE, TRUE);
	}

	StencilTwoSidedEnable::~StencilTwoSidedEnable()
	{
		g_pd3dDevice->SetRenderState(D3DRS_TWOSIDEDSTENCILMODE, FALSE);
	}

	SaveMatrix::SaveMatrix( D3DTRANSFORMSTATETYPE dwType )
		:m_Type(dwType)
	{
		g_pd3dDevice->GetTransform(dwType, &m_Mat);
	}

	SaveMatrix::~SaveMatrix()
	{
		g_pd3dDevice->SetTransform(m_Type, &m_Mat);
	}

	FVFSetter::FVFSetter( DWORD dwFVFNew )
	{
		g_pd3dDevice->GetFVF(&dwFVF);
		g_pd3dDevice->SetFVF(dwFVFNew);
	}

	FVFSetter::~FVFSetter()
	{
		g_pd3dDevice->SetFVF(dwFVF);
	}

	
};
////////////////////////////////////////////////////////////////////////////////

KG3DCameraSphere::KG3DCameraSphere()
{
	m_vCenter = D3DXVECTOR3(0.f, 0.f, 0.f);
	m_fRadius = 0.f;
}

KG3DCameraSphere::~KG3DCameraSphere()
{

}

void KG3DCameraSphere::Update(D3DXVECTOR3& vCenter, float fRaduis)
{
	m_vCenter   = vCenter;
	m_fRadius   = fRaduis;
}

bool KG3DCameraSphere::_ChkModelCross(KG3DModel* pModel)
{
	D3DXMATRIX matWorldInv;
	D3DXVECTOR3 vBoxCenter;
	D3DXVECTOR3 vCenter;

	if (NULL == pModel)
		return false;

	KG3DMesh* pMesh = pModel->GetMesh();
	if(NULL == pMesh)
		return false;

	KG3DMaterial* pMaterial = pModel->GetMaterial();

	if (!pMesh || !pMesh->m_ppMeshes[KG3DMesh::SMBT_NORMAL] || !pMaterial || !(pMaterial->IsLoaded()))
		return false;

	bool bCW = false;

	KG3DMaterial::KG3DMaterialSubset* pSubMtl = pMaterial->GetSubMaterial(0);
	KGLOG_PROCESS_ERROR(pSubMtl);

	if (!(pSubMtl->m_dwOption & MATERIAL_OPTION_CULL_CCW))
		bCW = true;

	pModel->ComputeBBox();

	vBoxCenter = pModel->m_BBox.GetCenter();

	float fBoxRadius = D3DXVec3Length(&(pModel->m_BBox.A - pModel->m_BBox.B)) * 0.5f;

	float fDistance  = D3DXVec3Length(&(m_vCenter - vBoxCenter));

	if (fDistance > (fBoxRadius + m_fRadius))
		return false;



	D3DXMatrixInverse(&matWorldInv, NULL, &pModel->m_matWorld);
	D3DXVec3TransformCoord(&vCenter, &m_vCenter, &matWorldInv);

	WORD* pIndies = NULL;
	BYTE* pVertex = NULL;
	DWORD NumFaces  = pMesh->m_ppMeshes[KG3DMesh::SMBT_NORMAL]->GetNumFaces();
	DWORD dwStrick  = D3DXGetFVFVertexSize(pMesh->m_ppMeshes[KG3DMesh::SMBT_NORMAL]->GetFVF());

	// 先不考虑这么恐怖的模型
	ASSERT(!(pMesh->m_ppMeshes[KG3DMesh::SMBT_NORMAL]->GetOptions() & D3DXMESH_32BIT));

	if (FAILED(pMesh->m_ppMeshes[KG3DMesh::SMBT_NORMAL]->LockVertexBuffer(D3DLOCK_READONLY, (void**)&pVertex)))
		return false;

	if (FAILED(pMesh->m_ppMeshes[KG3DMesh::SMBT_NORMAL]->LockIndexBuffer(D3DLOCK_READONLY, (void**)&pIndies)))
	{
		pMesh->m_ppMeshes[KG3DMesh::SMBT_NORMAL]->UnlockVertexBuffer();
		return false;
	}

	for (DWORD i = 0; i < NumFaces; i++)
	{
		D3DXVECTOR3* pA = (D3DXVECTOR3*)&pVertex[pIndies[0 + i * 3] * dwStrick];
		D3DXVECTOR3* pB = (D3DXVECTOR3*)&pVertex[pIndies[1 + i * 3] * dwStrick];
		D3DXVECTOR3* pC = (D3DXVECTOR3*)&pVertex[pIndies[2 + i * 3] * dwStrick];
		D3DXVECTOR3 vInsert;

		if (IsSphereCrossTriangle(vCenter, m_fRadius, *pA, *pB, *pC, bCW, &m_ObsInfo))
		{
			D3DXVec3TransformCoord(&m_vCenter, &m_ObsInfo.vUpdate, &pModel->m_matWorld);  

			pMesh->m_ppMeshes[KG3DMesh::SMBT_NORMAL]->UnlockIndexBuffer();
			pMesh->m_ppMeshes[KG3DMesh::SMBT_NORMAL]->UnlockVertexBuffer();

			return true;
		}
	}

	pMesh->m_ppMeshes[KG3DMesh::SMBT_NORMAL]->UnlockIndexBuffer();
	pMesh->m_ppMeshes[KG3DMesh::SMBT_NORMAL]->UnlockVertexBuffer();

Exit0:
	return false;
}

bool KG3DCameraSphere::CheckFacesCross(D3DXVECTOR3* pFaceVertex, DWORD dwFaceNum)
{
	if (!pFaceVertex || dwFaceNum <= 0)
		return false;

	for (DWORD i = 0; i < dwFaceNum; i++)
	{
		D3DXVECTOR3* pA = &pFaceVertex[0 + i * 3];
		D3DXVECTOR3* pB = &pFaceVertex[1 + i * 3];
		D3DXVECTOR3* pC = &pFaceVertex[2 + i * 3];

		if (IsSphereCrossTriangle(m_vCenter, m_fRadius, *pA, *pB, *pC, false, &m_ObsInfo))
		{
			m_vCenter = m_ObsInfo.vUpdate; 
			return true;
		}
	}

	return false;
}

bool KG3DCameraSphere::CheckModelCross(KG3DModel* pModel)
{
	if (!pModel)
		return false;

	KG3DModelST* pModelST = dynamic_cast<KG3DModelST*>(pModel);

	if (pModelST)
	{
		int NumChild = pModelST->GetNumModel();

		for (int i = 0; i < NumChild; i++)
		{
			KG3DModel* pModel = NULL;
			pModelST->GetModel(i, (IKG3DModel**)&pModel);

			if (_ChkModelCross(pModel))
				return true;
		}
	}
	else
	{
		return _ChkModelCross(pModel);
	}

	return false;
}

