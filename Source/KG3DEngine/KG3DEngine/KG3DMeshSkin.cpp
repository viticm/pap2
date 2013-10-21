#include "StdAfx.h"
#include "KG3DMeshSkin.h"
#include "KG3DModelST.h"
#include "KG3DTextureTable.h"
#include "KG3DBip.h"

BOOL g_bCpuSkinTest = FALSE;
extern HRESULT g_SetMaterialDetailInfo(LPD3DXEFFECT pEffect,KG3DModel* pModel, int nSubSetID);

KG3DMeshSkin::KG3DMeshSkin(void)
{
	m_lpMesh = NULL;

	m_nNumBone = 0;
	m_nNumSubset = 0;
	m_nNumVertex = 0;

	m_lppCurrentBoneMatrix = NULL;
	m_lppBoneMatrixBaseInverse = NULL;
	m_lpBoneMatrixFinal = NULL;

	m_lpVertexBufferSrc = NULL;
	m_lpVertexBufferDest = NULL;

	m_nUpdateTime = 0;
	m_nLastUpdateTime = 0;
	m_bNeedUpdateVertex = FALSE;
}

KG3DMeshSkin::~KG3DMeshSkin(void)
{
	UnInit();
}

HRESULT KG3DMeshSkin::UnInit()
{
	SAFE_RELEASE(m_lpMesh);
	m_nNumBone = 0;
	m_nNumSubset = 0;
	m_nNumVertex = 0;

	SAFE_DELETE_ARRAY(m_lppCurrentBoneMatrix);
	SAFE_DELETE_ARRAY(m_lppBoneMatrixBaseInverse);
	SAFE_DELETE_ARRAY(m_lpBoneMatrixFinal);

	SAFE_DELETE_ARRAY(m_lpVertexBufferSrc);
	SAFE_DELETE_ARRAY(m_lpVertexBufferDest);

	return S_OK;
}

HRESULT KG3DMeshSkin::CreateFromModelST(KG3DModelST* pModelST)
{
	HRESULT hr = E_FAIL;

	int nNumVertex = 0;
	int nNumFace   = 0;
	int nNumSubset = 0;

	int nNumBone   = pModelST->m_pBip->GetNumBones();

	for (int i=0;i<pModelST->GetNumModel();i++)
	{
		KG3DModel* pModel = NULL;
		pModelST->GetModel(i,(IKG3DModel**)&pModel);

		if(pModel)
		{
			ModelInfo vInfo;
			
			vInfo.pModel = pModel;

			vInfo.nNumVertex = pModel->GetVertexNum();
			vInfo.nNumFace   = pModel->GetFaceNum();
			pModel->GetNumMaterial(&vInfo.nNumSubset);
			ASSERT(vInfo.nNumSubset>=1);

			pModel->GetNumBones(&vInfo.nNumBone);

			vInfo.nVertexStart = nNumVertex;
			vInfo.nFaceStart   = nNumFace;
			vInfo.nSubsetStart = nNumSubset;
			vInfo.nBoneStart   = nNumBone;

			pModel->GetMeshBoneToBipInfo(vInfo.nNumBoneBip,vInfo.nNumBoneMeshOnly);

			nNumVertex += vInfo.nNumVertex;
			nNumFace   += vInfo.nNumFace;

			for (int nSub=0;nSub<vInfo.nNumSubset;nSub++)
			{
				KG3DMaterial::KG3DMaterialSubset* pMaterialSubset = pModel->GetMaterialSubset(nSub);
				m_vecMaterialSubset.push_back(pMaterialSubset);
				nNumSubset++;
			}

			for (size_t b=0;b<pModel->m_Mesh2Bip.size();b++)
			{
				int nIndex = pModel->m_Mesh2Bip[b];
				int nNewIndex = nIndex;
				if(nIndex==-1)
				{
					nNewIndex = nNumBone;
					nNumBone++;
				}
				vInfo.vecBoneIndexMap.push_back(nNewIndex);
			}
			m_vecModelInfo.push_back(vInfo);
		}
	}

	m_nNumSubset = nNumSubset;
	m_nNumBone   = nNumBone;

	KGLOG_PROCESS_ERROR(m_nNumBone<=256);

	hr = CreateSkinMesh(nNumVertex,nNumFace);
	KGLOG_COM_PROCESS_ERROR(hr);

	hr = FillData();
	KGLOG_COM_PROCESS_ERROR(hr);

	hr = CreateBoneMatrix(pModelST);
	KGLOG_COM_PROCESS_ERROR(hr);

	m_bCreateCompleted = TRUE;
Exit0:
	return hr;
}

HRESULT KG3DMeshSkin::CreateSkinMesh(int nNumVertex,int nNumFace)
{
	HRESULT hr = E_FAIL;
	SAFE_RELEASE(m_lpMesh);

	static const D3DVERTEXELEMENT9 VBDecl[] = 
	{
		{0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT,0},
		{0, 28, D3DDECLTYPE_UBYTE4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDINDICES, 0},
		{0, 32, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0},
		{0, 44, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		D3DDECL_END(),
	}; 

	hr = D3DXCreateMesh(nNumFace,nNumVertex,D3DXMESH_WRITEONLY,VBDecl,g_pd3dDevice,&m_lpMesh);
	KGLOG_COM_PROCESS_ERROR(hr);

	m_lpVertexBufferSrc = new VBFormat[nNumVertex];
	KGLOG_PROCESS_ERROR(m_lpVertexBufferSrc);
	m_lpVertexBufferDest = new VBFormat[nNumVertex];
	KGLOG_PROCESS_ERROR(m_lpVertexBufferDest);

	m_nNumVertex = nNumVertex;

	hr = S_OK;
Exit0:
	return hr;
}


HRESULT KG3DMeshSkin::FillDataWithModel(ModelInfo& vInfo,VBFormat* pDestVB,WORD* pDestIndex,DWORD* pDestAttrib)
{
	HRESULT hr = E_FAIL;
	BOOL bVBLock = FALSE;
	BOOL bIBLock = FALSE;
	BOOL bABLock = FALSE;	
	
	LPD3DXMESH pSrcMeshSys = NULL;
	KG3DMesh* pMesh = vInfo.pModel->GetMesh();
	KGLOG_PROCESS_ERROR(pMesh);

	pSrcMeshSys = pMesh->GetMesh(KG3DMesh::SMBT_NORMAL);
	KGLOG_PROCESS_ERROR(pSrcMeshSys);

	struct SrcVerFormat
	{
		D3DXVECTOR3 vPosition;
		D3DXVECTOR3 vNormal;
		D3DXVECTOR2 vUV;
	};

	SrcVerFormat* pSrcVB = NULL;
	WORD*     pSrcIndex = NULL;
	DWORD*    pSrcAttrib = NULL;

	KGLOG_PROCESS_ERROR(pSrcMeshSys);

	hr = pSrcMeshSys->LockVertexBuffer(0,(void**)&pSrcVB);
	KGLOG_COM_PROCESS_ERROR(hr);
	bVBLock = TRUE;

	hr = pSrcMeshSys->LockIndexBuffer(0,(void**)&pSrcIndex);
	KGLOG_COM_PROCESS_ERROR(hr);
	bIBLock = TRUE;

	hr = pSrcMeshSys->LockAttributeBuffer(0,&pSrcAttrib);
	KGLOG_COM_PROCESS_ERROR(hr);
	bABLock = TRUE;

	//////////////////////////////////////////////////////////////////////////
	for (int i=0;i<vInfo.nNumVertex;i++)
	{
		int nIndexDest = vInfo.nVertexStart + i;
		pDestVB[nIndexDest].vPosition = pSrcVB[i].vPosition;
		pDestVB[nIndexDest].vNormal   = pSrcVB[i].vNormal;
		pDestVB[nIndexDest].vUV       = pSrcVB[i].vUV;

		pDestVB[nIndexDest].vBlendWeight = D3DXVECTOR4(0,0,0,0);
		pDestVB[nIndexDest].vBlendIndex  = 0xFFFFFFFF;

		if(m_lpVertexBufferDest)
		{
			m_lpVertexBufferDest[nIndexDest] = pDestVB[nIndexDest];
		}
		if(m_lpVertexBufferSrc)
		{
			m_lpVertexBufferSrc[nIndexDest] = pDestVB[nIndexDest];
		}	
	}

	for (int i=0;i<vInfo.nNumFace*3;i++)
	{
		int nIndexDest = vInfo.nFaceStart * 3 + i;
		pDestIndex[nIndexDest] = (WORD)(pSrcIndex[i] + vInfo.nVertexStart);
	}

	for (int i=0;i<vInfo.nNumFace;i++)
	{
		int nIndexDest = vInfo.nFaceStart + i;
		pDestAttrib[nIndexDest] = pSrcAttrib[i] + vInfo.nSubsetStart;
	}

	for (int i=0;i<(int)pMesh->m_dwNumBone;i++)//遍历模型上的每一根骨骼
	{
		KG3DMesh::BoneInfo1* pBone = &pMesh->m_pBoneInfo[i];

		for (DWORD dwVertexIndex =0;dwVertexIndex<pBone->NumVerticesRef;dwVertexIndex++)//遍历每一个骨骼影响的顶点
		{
			int   IndexSrc = pBone->VerticesIndexRef[dwVertexIndex];//旧的顶点序号
			float fWeight = pBone->VerticesWeightRef[dwVertexIndex];//旧的顶点权重

			int nNewBoneIndex = vInfo.vecBoneIndexMap[i];//新的骨骼编号

			int nIndexDest = vInfo.nVertexStart + IndexSrc;//新的顶点编号

			BYTE*  pBoneIndexOnVertex  = (BYTE* )&pDestVB[nIndexDest].vBlendIndex;
			float* pBoneWeightOnVertex = (float*)&pDestVB[nIndexDest].vBlendWeight;

			for (int s=0;s<4;s++)
			{
				if(pBoneIndexOnVertex[s]==0xFF)
				{
					pBoneIndexOnVertex [s] = (BYTE)nNewBoneIndex;
					pBoneWeightOnVertex[s] = fWeight;
					break;
				}
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	
	hr = S_OK;
Exit0:
	if(pSrcMeshSys)
	{
		if(bVBLock)
		{
			pSrcMeshSys->UnlockVertexBuffer();
			bVBLock = FALSE;
		}

		if(bIBLock)
		{
			pSrcMeshSys->UnlockIndexBuffer();
			bIBLock = FALSE;
		}

		if(bABLock)
		{
			pSrcMeshSys->UnlockAttributeBuffer();
			bABLock = FALSE;
		}
	}
	return hr;
}

HRESULT KG3DMeshSkin::FillData()
{
	HRESULT hr = E_FAIL;
	BOOL bVBLock = FALSE;
	BOOL bIBLock = FALSE;
	BOOL bABLock = FALSE;

	VBFormat* pVB = NULL;
	WORD*     pIndex = NULL;
	DWORD*    pAttrib = NULL;

	KGLOG_PROCESS_ERROR(m_lpMesh);

	hr = m_lpMesh->LockVertexBuffer(0,(void**)&pVB);
	KGLOG_COM_PROCESS_ERROR(hr);
	bVBLock = TRUE;

	hr = m_lpMesh->LockIndexBuffer(0,(void**)&pIndex);
	KGLOG_COM_PROCESS_ERROR(hr);
	bIBLock = TRUE;

	hr = m_lpMesh->LockAttributeBuffer(0,&pAttrib);
	KGLOG_COM_PROCESS_ERROR(hr);
	bABLock = TRUE;

	//////////////////////////////////////////////////////////////////////////
	for (size_t i=0;i<m_vecModelInfo.size();i++)
	{
		ModelInfo& vInfo = m_vecModelInfo[i];
		hr = FillDataWithModel(vInfo,pVB,pIndex,pAttrib);
		KGLOG_COM_PROCESS_ERROR(hr);
	}

	//////////////////////////////////////////////////////////////////////////
	for (int i=0;i<m_nNumVertex;i++)
	{
		if(m_lpVertexBufferDest)
		{
			m_lpVertexBufferDest[i] = pVB[i];
		}
		if(m_lpVertexBufferSrc)
		{
			m_lpVertexBufferSrc[i] = pVB[i];
		}	
	}
	//////////////////////////////////////////////////////////////////////////
	hr = S_OK;
Exit0:
	if(m_lpMesh)
	{
		if(bVBLock)
		{
			m_lpMesh->UnlockVertexBuffer();
			bVBLock = FALSE;
		}

		if(bIBLock)
		{
			m_lpMesh->UnlockIndexBuffer();
			bIBLock = FALSE;
		}

		if(bABLock)
		{
			m_lpMesh->UnlockAttributeBuffer();
			bABLock = FALSE;
		}
	}
	return hr;
}

HRESULT KG3DMeshSkin::RenderNoChange(KG3DModelST* pModelST,LPD3DXEFFECT pShader)
{

	D3DXMATRIX matView, matProj, matWVP;
	g_pd3dDevice->GetTransform(D3DTS_VIEW, &matView);
	g_pd3dDevice->GetTransform(D3DTS_PROJECTION, &matProj);

	for (size_t s=0;s<m_vecModelInfo.size();s++)
	{
		ModelInfo& vInfo = m_vecModelInfo[s];

		{
			D3DXMATRIX matWorld = vInfo.pModel->m_Matrix * pModelST->m_matWorld; 
			matWVP = matWorld * matView * matProj;
			pShader->SetMatrix("WorldViewProj", &matWVP);
			pShader->SetMatrix("matWorld", &matWorld);
		}	
		
		for (int nID=0;nID<vInfo.nNumSubset;nID++)
		{

			KG3DMaterial::KG3DMaterialSubset* pMtl = vInfo.pModel->GetMaterialSubset(nID);
			if(pMtl)
			{
				pMtl->SetMaterial(1,0);
				g_SetMaterialDetailInfo(pShader,vInfo.pModel, nID);
				pShader->CommitChanges();
				if(m_lpMesh)
					m_lpMesh->DrawSubset(vInfo.nSubsetStart + nID);
				pMtl->RestoreMaterial();
			}
		}
	}

//Exit0:
	return S_OK;
}

HRESULT KG3DMeshSkin::CreateBoneMatrix(KG3DModelST* pModelST)
{
	HRESULT hr = E_FAIL;

	SAFE_DELETE_ARRAY(m_lppCurrentBoneMatrix);
	SAFE_DELETE_ARRAY(m_lppBoneMatrixBaseInverse);
	SAFE_DELETE_ARRAY(m_lpBoneMatrixFinal);
	
	m_lppCurrentBoneMatrix = new D3DXMATRIX*[m_nNumBone];
	KGLOG_PROCESS_ERROR(m_lppCurrentBoneMatrix);

	m_lppBoneMatrixBaseInverse = new D3DXMATRIX*[m_nNumBone];
	KGLOG_PROCESS_ERROR(m_lppBoneMatrixBaseInverse);

	m_lpBoneMatrixFinal = new D3DXMATRIX[m_nNumBone];
	KGLOG_PROCESS_ERROR(m_lpBoneMatrixFinal);


	ZeroMemory(m_lppCurrentBoneMatrix    ,sizeof(D3DXMATRIX*) * m_nNumBone);
	ZeroMemory(m_lppBoneMatrixBaseInverse,sizeof(D3DXMATRIX*) * m_nNumBone);
	ZeroMemory(m_lpBoneMatrixFinal       ,sizeof(D3DXMATRIX ) * m_nNumBone);
	//////////////////////////////////////////////////////////////////////////
	D3DXMATRIX* pSrcMatrix = pModelST->m_pBip->GetObjMatricesArray();
	int nBipBoneEnd = pModelST->m_pBip->GetNumBones();

	for (int i=0;i<nBipBoneEnd;i++)
	{
		m_lppCurrentBoneMatrix[i] = &pSrcMatrix[i];
	}

	for (size_t i=0;i<m_vecModelInfo.size();i++)
	{
		ModelInfo& vInfo = m_vecModelInfo[i];
		for (size_t sBone=0;sBone<vInfo.vecBoneIndexMap.size();sBone++)
		{
			KG3DMesh* pMesh = vInfo.pModel->GetMesh();
			KGLOG_PROCESS_ERROR(pMesh);

			int nBoneIndexNew = vInfo.vecBoneIndexMap[sBone];
			int nBoneIndexOld = vInfo.pModel->m_Mesh2Bip[sBone];

			ASSERT(nBoneIndexNew<m_nNumBone);

			if(nBoneIndexOld==-1)//表示属于私有骨骼
			{
				m_lppCurrentBoneMatrix[nBoneIndexNew]     = &vInfo.pModel->m_pBoneMatricesForRender[sBone];
				m_lppBoneMatrixBaseInverse[nBoneIndexNew] = &pMesh->m_pmatReverseMatrix[sBone];
			}
			else//表示来自BIP
			{
				m_lppBoneMatrixBaseInverse[nBoneIndexNew] = &pMesh->m_pmatReverseMatrix[sBone];
			}
		}
	}
	hr = S_OK;
Exit0:
	return hr;
}

HRESULT KG3DMeshSkin::ComputeMatrix()
{
	for (int i=0;i<m_nNumBone;i++)
	{
		D3DXMATRIX* pCurrent = m_lppCurrentBoneMatrix[i];
		D3DXMATRIX* pInverse = m_lppBoneMatrixBaseInverse[i];

		if(pCurrent && pInverse)
			m_lpBoneMatrixFinal[i] = (*pInverse) * (*pCurrent);
		//else
		//	ASSERT(0);
	}
	return S_OK;
}

HRESULT KG3DMeshSkin::ComputeSkining()
{
	for (int i=0;i<m_nNumVertex;i++)
	{
		VBFormat& vertex = m_lpVertexBufferSrc[i];

		BYTE*  pBoneIndexOnVertex  = (BYTE* )&vertex.vBlendIndex;
		float* pBoneWeightOnVertex = (float*)&vertex.vBlendWeight;

		D3DXVECTOR3 Pos_Sum(0,0,0);
		D3DXVECTOR3 Nor_Sum(0,0,0);

		for (int s=0;s<4;s++)
		{
			BYTE bIndex = pBoneIndexOnVertex[s];
			if(bIndex !=0xFF)
			{
				D3DXVECTOR3 pos,nor;

				D3DXVec3TransformCoord (&pos,&vertex.vPosition,&m_lpBoneMatrixFinal[bIndex]);
				D3DXVec3TransformNormal(&nor,&vertex.vNormal  ,&m_lpBoneMatrixFinal[bIndex]);
				Pos_Sum += pos * pBoneWeightOnVertex[s];
				Nor_Sum += nor * pBoneWeightOnVertex[s];
			}
			else
				break;
		}

		m_lpVertexBufferDest[i].vPosition = Pos_Sum;
		m_lpVertexBufferDest[i].vNormal   = Nor_Sum;

	}

	return S_OK;
}

HRESULT KG3DMeshSkin::UpdateVertex()
{
	HRESULT hr = E_FAIL;

	BOOL bVBLock = FALSE;
	VBFormat* pDestVB = NULL;

	KGLOG_PROCESS_ERROR(m_lpMesh);

	hr = m_lpMesh->LockVertexBuffer(0,(void**)&pDestVB);
	KGLOG_COM_PROCESS_ERROR(hr);
	bVBLock = TRUE;

	memcpy(pDestVB,m_lpVertexBufferDest,sizeof(VBFormat)*m_nNumVertex);

	hr = S_OK;
Exit0:
	if(bVBLock)
	{
		m_lpMesh->UnlockVertexBuffer();
	}
	return hr;
}

extern DWORD g_dwRenderCount;

HRESULT KG3DMeshSkin::FrameMove()
{
	int nNow = timeGetTime();
	if(m_bNeedUpdateVertex && ( nNow - m_nLastUpdateTime >= m_nUpdateTime ))
	{
		UpdateVertex();
		m_nLastUpdateTime = nNow;

		m_bNeedUpdateVertex = FALSE;
	}

	return S_OK;
}

HRESULT KG3DMeshSkin::MultiThreadProcess()
{
	if(!m_bCreateCompleted)
		return S_OK;

	if(!m_bNeedUpdateVertex)
	{
		ComputeMatrix();
		ComputeSkining();
		m_bNeedUpdateVertex = TRUE;
	}

	return S_OK;
}
