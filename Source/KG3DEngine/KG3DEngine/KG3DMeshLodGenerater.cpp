#include "StdAfx.h"
#include "KG3DMeshLodGenerater.h"
#include "KG3DMesh.h"
#include "KG3DGraphicsTool.h"

vector<D3DXVECTOR3>g_vecModelEditorPoints;

KG3DMeshLodGenerater::KG3DMeshLodGenerater(int NumBoneSrc,KG3DMesh* pMesh)
{
	m_nNumBoneSrc = NumBoneSrc;
	m_lpMeshSrc = pMesh;

	AddBoneReplaceRule("bip01 l finger","bip01 l hand");
	AddBoneReplaceRule("bip01 r finger","bip01 r hand");

	m_lpMeshSys = NULL;

	/*vector<VertexBoneInfo>newvec;
	for (int i=0;i<m_nNumBoneSrc;i++)
	{
		m_mapVertexIndex[i] = newvec;
	}*/
}

KG3DMeshLodGenerater::~KG3DMeshLodGenerater(void)
{
	SAFE_RELEASE(m_lpMeshSys);
}

HRESULT KG3DMeshLodGenerater::AddBoneReplaceRule(LPSTR pNameSrc,LPSTR pNameDest)
{
	BoneReplace replace;
	replace.szNameSrc  = pNameSrc;
	replace.szNameDest = pNameDest;

	m_vecBoneReplace.push_back(replace);
	return S_OK;
}

HRESULT KG3DMeshLodGenerater::FillVertexSys(DWORD dwVertex,DWORD dwFace,BYTE* pData,int nDataSize,int nPos,int nNormal,int nUV,DWORD* pSubset)
{
	KGLOG_PROCESS_ERROR(dwVertex==m_vecVertexBoneLink.size());

	for (DWORD i=0;i<dwVertex;i++)
	{
		D3DXVECTOR3* pPosition  = (D3DXVECTOR3*)&pData[ i*nDataSize + nPos    ];
		D3DXVECTOR3* pNormal    = (D3DXVECTOR3*)&pData[ i*nDataSize + nNormal ];
		//D3DXVECTOR2* pUV        = (D3DXVECTOR2*)&pData[ i*nDataSize + nUV     ];
		////////////////////////////////////////////////////////////////////////////

		VertexBoneWeight& newVertex = m_vecVertexBoneLink[i];

		*pPosition = newVertex.vPositionSrc;
		*pNormal   = newVertex.vNormalSrc  ;
	}
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DMeshLodGenerater::SortBonesByVertexData(DWORD dwVertex,DWORD dwFace,BYTE* pData,int nDataSize,int nBoneIndexOffset,int nBoneWeightOffset,
										int nPos,int nPos1,int nNormal,int nNormal1,int nUV,WORD* pIndex,DWORD* pSubset)
{
	//对SUBSET进行分类
	m_mapAttribute.clear();
	for (DWORD i=0;i<dwFace;i++)
	{
		DWORD dwAttribute = pSubset[i];

		map< DWORD,vector<DWORD> >::iterator t = m_mapAttribute.find(dwAttribute);
		if(t==m_mapAttribute.end())
		{
			vector<DWORD>newvec;
			newvec.push_back(i);
			m_mapAttribute[dwAttribute] = newvec;
		}
		else
		{
			vector<DWORD>& vecFace = t->second;
			vecFace.push_back(i);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	g_vecModelEditorPoints.clear();

	for (DWORD i=0;i<dwVertex;i++)
	{
		DWORD* pBoneIndexs = (DWORD*)&pData[ i*nDataSize + nBoneIndexOffset ];

		DWORD dwA= ((*pBoneIndexs) & 0x000000FF) ;
		DWORD dwB = ((*pBoneIndexs) & 0x0000FF00) >> 8;
		DWORD dwC = ((*pBoneIndexs) & 0x00FF0000) >> 16;
		DWORD dwD = ((*pBoneIndexs) & 0xFF000000) >> 24;

		D3DXVECTOR4* pBoneWeight = (D3DXVECTOR4*)&pData[ i*nDataSize + nBoneWeightOffset ];

		D3DXVECTOR3* pPosition  = (D3DXVECTOR3*)&pData[ i*nDataSize + nPos    ];
		D3DXVECTOR3* pPosition1 = (D3DXVECTOR3*)&pData[ i*nDataSize + nPos1   ];
		D3DXVECTOR3* pNormal    = (D3DXVECTOR3*)&pData[ i*nDataSize + nNormal ];
		D3DXVECTOR3* pNormal1   = (D3DXVECTOR3*)&pData[ i*nDataSize + nNormal1];
		D3DXVECTOR2* pUV        = (D3DXVECTOR2*)&pData[ i*nDataSize + nUV     ];
		////////////////////////////////////////////////////////////////////////////

		VertexBoneWeight newVertex;
		newVertex.vPosition  = *pPosition ;
		newVertex.vPosition1 = *pPosition1;
		newVertex.vNormal    = *pNormal   ;
		newVertex.vNormal1   = *pNormal1  ;
		newVertex.vUV        = *pUV       ;

		newVertex.AddBone(dwA,pBoneWeight->x);
		newVertex.AddBone(dwB,pBoneWeight->y);
		newVertex.AddBone(dwC,pBoneWeight->z);
		newVertex.AddBone(dwD,pBoneWeight->w);


		m_vecVertexBoneLink.push_back(newVertex);

		g_vecModelEditorPoints.push_back(newVertex.vPositionSrc + D3DXVECTOR3(500,0,0));
	}

	SortBonesFromVertex(pIndex);

	//ReduceBones();

	GenerateBoneRemap();

	return S_OK;

}

HRESULT KG3DMeshLodGenerater::SortBonesFromVertex(WORD* pIndex)
{
	HRESULT hr = E_FAIL;
	D3DXMATRIX vSrcCurMatrix[MAX_PATH];
	ASSERT(m_lpMeshSrc->m_dwNumBone<MAX_PATH);

	for (DWORD i=0;i<m_lpMeshSrc->m_dwNumBone;i++)
	{
		vSrcCurMatrix[i] = m_lpMeshSrc->m_pBoneInfo[i].CurMatrix;
	}
	//////////////////////////////////////////////////////////////////////////
	D3DXBONECOMBINATION* pBoneComb = NULL;
	pBoneComb = static_cast<D3DXBONECOMBINATION*>(m_lpMeshSrc->m_pBoneCombinationTable->GetBufferPointer());
	KG_PROCESS_ERROR(pBoneComb);
	//////////////////////////////////////////////////////////////////////////
	{
		map< DWORD,vector<DWORD> >::iterator t = m_mapAttribute.begin();
		map< DWORD,vector<DWORD> >::iterator tend = m_mapAttribute.end();
		while (t!=tend)
		{
			DWORD dwAttribute = t->first;
			vector<DWORD>& vecFace = t->second;
			//////////////////////////////////////////////////////////////////////////
			const D3DXMATRIX* matTemp[MAX_PATH];
			int nNumBone = m_nNumBoneSrc;

			D3DXMATRIX matIdentfy = D3DXMatrixGetIdentity();

			m_lpMeshSrc->PrepareMatrixForSkinMeshRender(nNumBone,matTemp,0,dwAttribute,vSrcCurMatrix,&matIdentfy);


			for (size_t i=0;i<vecFace.size();i++)
			{
				DWORD dwFaceIndex = vecFace[i];

				WORD wA = pIndex[dwFaceIndex*3  ];
				WORD wB = pIndex[dwFaceIndex*3+1];
				WORD wC = pIndex[dwFaceIndex*3+2];

				VertexBoneWeight& vWeightA = m_vecVertexBoneLink[wA];
				VertexProcess(&vWeightA,matTemp,pBoneComb[dwAttribute].BoneId);

				VertexBoneWeight& vWeightB = m_vecVertexBoneLink[wB];
				VertexProcess(&vWeightB,matTemp,pBoneComb[dwAttribute].BoneId);
				VertexBoneWeight& vWeightC = m_vecVertexBoneLink[wC];
				VertexProcess(&vWeightC,matTemp,pBoneComb[dwAttribute].BoneId);

			}

			//////////////////////////////////////////////////////////////////////////
			t++;
		}

		for (size_t i=0;i<m_vecVertexBoneLink.size();i++)
		{
			VertexBoneWeight& vWeight = m_vecVertexBoneLink[i];

			for (DWORD s=0;s<vWeight.dwNumBone;s++)
			{
				DWORD dwSrcIndex = vWeight.dwBoneIndexReal[s];
				DWORD dwDestBoneIndex = 0;

				hr = TryBoneReplace(dwSrcIndex,dwDestBoneIndex);
				if(SUCCEEDED(hr))
				{
					vWeight.dwBoneIndexReal[s] = dwDestBoneIndex;
				}
				else
				{
					;
				}

				AddBone(vWeight.dwBoneIndexReal[s],(DWORD)i,vWeight.fWeight[s]);
			}
		}
	}

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DMeshLodGenerater::AddBone(DWORD dwBoneIndex,DWORD dwVertexIndex,float fWeight)
{                    
	map< DWORD,vector<VertexBoneInfo> >::iterator i = m_mapVertexIndex.find(dwBoneIndex);
	
	if(fWeight<=0.00001F)
		fWeight = 0;
	
	if (i!=m_mapVertexIndex.end())
	{
		vector<VertexBoneInfo>& vecVertexBoneInfo= i->second;

		VertexBoneInfo newInfo;
		newInfo.dwVertexIndex = dwVertexIndex;
		newInfo.fWeight = fWeight;

		vecVertexBoneInfo.push_back(newInfo);
	}
	else
	{
		vector<VertexBoneInfo> vecVertexBoneInfo;

		VertexBoneInfo newInfo;
		newInfo.dwVertexIndex = dwVertexIndex;
		newInfo.fWeight = fWeight;

		vecVertexBoneInfo.push_back(newInfo);

		m_mapVertexIndex[dwBoneIndex] = vecVertexBoneInfo;
	}

	return S_OK;
}

HRESULT KG3DMeshLodGenerater::ReduceBones()
{
	HRESULT hr = E_FAIL;

	map< DWORD,vector<VertexBoneInfo> >::iterator i = m_mapVertexIndex.begin();
	map< DWORD,vector<VertexBoneInfo> >::iterator iend = m_mapVertexIndex.end();
	while (i!=iend)
	{
		DWORD dwBoneIndex = i->first;
		vector<VertexBoneInfo>& vecVertexBoneInfo = i->second;

		DWORD dwDestBoneIndex = 0;

		hr = TryBoneReplace(dwBoneIndex,dwDestBoneIndex);
		if(SUCCEEDED(hr))
		{
			map< DWORD,vector<VertexBoneInfo> >::iterator t = m_mapVertexIndex.find(dwDestBoneIndex);
			if(t!=m_mapVertexIndex.end())
			{
				vector<VertexBoneInfo>& destVec = t->second;
				for (size_t s=0;s<vecVertexBoneInfo.size();s++)
				{
					destVec.push_back(vecVertexBoneInfo[s]);
				}
			}
			else
			{
				m_mapVertexIndex[dwDestBoneIndex] = vecVertexBoneInfo;
			}

			i = m_mapVertexIndex.erase(i);
		}
		else
		{
			i++;
		}

	}

	return S_OK;
}

HRESULT KG3DMeshLodGenerater::TryBoneReplace(DWORD dwBoneIndex,DWORD& dwDestBoneIndex)
{
	KG3DMesh::BoneInfo1* pBoneInfo = &m_lpMeshSrc->m_pBoneInfo[dwBoneIndex];

	string szName = pBoneInfo->strBoneName;

	for (size_t i=0;i<m_vecBoneReplace.size();i++)
	{
		BoneReplace& vReplace = m_vecBoneReplace[i];

		size_t nFind = szName.find(vReplace.szNameSrc);

		if(nFind!=-1)
		{
			int nDestBoneIndex = m_lpMeshSrc->FindBone(vReplace.szNameDest.c_str());
			if(nDestBoneIndex != -1)
			{
				dwDestBoneIndex = (DWORD)nDestBoneIndex;
				return S_OK;
			}
		}
	}

	return E_FAIL;
}

HRESULT KG3DMeshLodGenerater::VertexProcess(VertexBoneWeight* pVertex,const D3DXMATRIX* matTemp[],DWORD* BoneMap)
{
	D3DXVECTOR3 vPosSum(0,0,0);
	D3DXVECTOR3 vNormalSum(0,0,0);
	KGLOG_PROCESS_ERROR(m_lpMeshSrc);

	for (DWORD i=0;i<pVertex->dwNumBone;i++)
	{
		DWORD dwBoneIndex = pVertex->dwBoneIndex[i];

		D3DXVECTOR3 vPosOld(0,0,0);
		D3DXVECTOR3 vNormalOld(0,0,0);

		D3DXMATRIX* pMat = (D3DXMATRIX*)matTemp[dwBoneIndex];

		if(i==0)
		{
			D3DXVec3TransformCoord(&vPosOld,&pVertex->vPosition,pMat);
			D3DXVec3TransformNormal(&vNormalOld,&pVertex->vNormal,pMat);
		}
		else if(i==1)
		{
			D3DXVec3TransformCoord(&vPosOld,&pVertex->vPosition1,pMat);
			D3DXVec3TransformNormal(&vNormalOld,&pVertex->vNormal1,pMat);
		}

		vPosSum    += vPosOld    * pVertex->fWeight[i];
		vNormalSum += vNormalOld * pVertex->fWeight[i];
		
		pVertex->dwBoneIndexReal[i] = BoneMap[dwBoneIndex];
	}
		
	pVertex->vPositionSrc = vPosSum;
	pVertex->vNormalSrc   = vNormalSum;

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DMeshLodGenerater::VertexBoneWeight::AddBone(DWORD dwBone,float fW)
{
	if(fW >= 0.000000001F)
	{
		dwBoneIndex[dwNumBone] = dwBone;
		fWeight[dwNumBone] = fW;
		dwNumBone++;
	}
	return S_OK;
}

HRESULT KG3DMeshLodGenerater::SaveToMesh(LPSTR pName)
{
	HRESULT hr = E_FAIL;
	KG3DMesh newMesh;

	newMesh.m_dwOption = m_lpMeshSrc->m_dwOption;
	newMesh.m_dwOptionEx = m_lpMeshSrc->m_dwOptionEx;
	newMesh.m_dwNumSubset = m_lpMeshSrc->m_dwNumSubset;

	newMesh.m_ppMeshes[KG3DMesh::SMBT_NORMAL] = m_lpMeshSys;
	m_lpMeshSys->AddRef();

	newMesh.m_dwNumBone = (DWORD)m_mapVertexIndex.size();
	if(newMesh.m_dwNumBone)
	{
        newMesh.m_pBoneInfo = new KG3DMesh::BoneInfo1[newMesh.m_dwNumBone];
        KG_PROCESS_ERROR(newMesh.m_pBoneInfo);
        newMesh.m_pmatReverseMatrix = new D3DXMATRIX[newMesh.m_dwNumBone];
        KG_PROCESS_ERROR(newMesh.m_pmatReverseMatrix);

		int nNewBoneIndex = 0;
		map< DWORD,vector<VertexBoneInfo> >::iterator i = m_mapVertexIndex.begin();
		map< DWORD,vector<VertexBoneInfo> >::iterator iend = m_mapVertexIndex.end();
		while (i!=iend)
		{
			DWORD dwOldIndex = i->first;
			vector<VertexBoneInfo>& vecVertex = i->second;

			KG3DMesh::BoneInfo1* pSrcBone  = &m_lpMeshSrc->m_pBoneInfo[dwOldIndex];
			KG3DMesh::BoneInfo1* pDestBone = &newMesh.m_pBoneInfo[nNewBoneIndex];

			pDestBone->bBox = pSrcBone->bBox;
			pDestBone->BBoxScaleMatrix = pSrcBone->BBoxScaleMatrix;
			pDestBone->CurMatrix = pSrcBone->CurMatrix;
			pDestBone->IsFlexibleBone = pSrcBone->IsFlexibleBone;
			pDestBone->LocalWorldCurMatrix_NotNeeded = pSrcBone->LocalWorldCurMatrix_NotNeeded;
			pDestBone->nFlexBoneLevel_NotNeeded = pSrcBone->nFlexBoneLevel_NotNeeded;
			strcpy(pDestBone->strBoneName,pSrcBone->strBoneName);
			//////////////////////////////////////////////////////////////////////////
			{
				pDestBone->dwParentIndex = (DWORD)FindBoneNewIndex(pSrcBone->dwParentIndex);

				pDestBone->dwNumChild = 0;
				for (DWORD s=0;s<pSrcBone->dwNumChild;s++)
				{
					int nNewChildIndex = FindBoneNewIndex(pSrcBone->dwChildIndex[s]);
					if(nNewChildIndex!=-1)
					{
						pDestBone->dwChildIndex[pDestBone->dwNumChild] = nNewChildIndex;
						pDestBone->dwNumChild++;
					}
				}
			}

			//////////////////////////////////////////////////////////////////////////
						
			pDestBone->NumVerticesRef = (DWORD)vecVertex.size();
			if(pDestBone->NumVerticesRef)
			{
				pDestBone->VerticesIndexRef = new DWORD[pDestBone->NumVerticesRef];
				KGLOG_PROCESS_ERROR(pDestBone->VerticesIndexRef);
				pDestBone->VerticesWeightRef = new float[pDestBone->NumVerticesRef];
				KGLOG_PROCESS_ERROR(pDestBone->VerticesWeightRef);

				for (size_t t=0;t<vecVertex.size();t++)
				{
					pDestBone->VerticesIndexRef[t]  = vecVertex[t].dwVertexIndex;
					pDestBone->VerticesWeightRef[t] = vecVertex[t].fWeight;
				}
			}

			i++;
			nNewBoneIndex++;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	{
		newMesh.m_dwNumSocket = m_lpMeshSrc->m_dwNumSocket;

        newMesh.m_pSockets = new KG3DMesh::Socket1[newMesh.m_dwNumSocket];
        KG_PROCESS_ERROR(newMesh.m_pSockets);

        for (DWORD i=0;i<newMesh.m_dwNumSocket;i++)
		{
			KG3DMesh::Socket1* pSrcSocket  = &m_lpMeshSrc->m_pSockets[i];
			KG3DMesh::Socket1* pDestSocket = &newMesh.m_pSockets[i];

			pDestSocket->dwParentBoneIndex = FindBoneNewIndex(pSrcSocket->dwParentBoneIndex);
			pDestSocket->matCurMatrix = pSrcSocket->matCurMatrix;
			pDestSocket->matMatrixOffset = pSrcSocket->matMatrixOffset;
			strcpy(pDestSocket->strSocketName,pSrcSocket->strSocketName);
		}
	}

	hr = newMesh.SaveToFile(pName,0);
	KGLOG_COM_PROCESS_ERROR(hr);

	return S_OK;
Exit0:
    KG_DELETE_ARRAY(newMesh.m_pBoneInfo);
    KG_DELETE_ARRAY(newMesh.m_pmatReverseMatrix);
    KG_DELETE_ARRAY(newMesh.m_pSockets);
	return E_FAIL;
}

HRESULT KG3DMeshLodGenerater::GenerateBoneRemap()
{
	m_mapBoneRemap.clear();
	if(0)
	{
		int nNewIndex = 0;
		map< DWORD,vector<VertexBoneInfo> >::iterator i = m_mapVertexIndex.begin();
		map< DWORD,vector<VertexBoneInfo> >::iterator iend = m_mapVertexIndex.end();
		while (i!=iend)
		{
			DWORD dwOldIndex = i->first;
			m_mapBoneRemap[dwOldIndex] = nNewIndex; 
			i++;
			nNewIndex++;
		}
	}
	else
	{
		for (int i=0;i<m_nNumBoneSrc;i++)
		{
			m_mapBoneRemap[i] = i;
		}
	}

	return S_OK;
}

int KG3DMeshLodGenerater::FindBoneNewIndex(DWORD dwOldIndex)
{
	map< DWORD, int >::iterator i = m_mapBoneRemap.find(dwOldIndex);
	if(i!=m_mapBoneRemap.end())
	{
		return i->second;
	}
	else
	{
		return -1;
	}

}

HRESULT KG3DMeshLodGenerater::SavePrograssMeshAsLowMesh(LPD3DXPMESH pMeshProgress,LPSTR pName)
{
	HRESULT hr = E_FAIL;
	LPD3DXMESH pNewMesh = NULL;
	D3DVERTEXELEMENT9 VertexDesc[MAX_FVF_DECL_SIZE];
	ZeroMemory(VertexDesc,sizeof(VertexDesc));

	pMeshProgress->GetDeclaration(VertexDesc);

	if(m_nNumBoneSrc)
	{
		hr = pMeshProgress->CloneMesh(0,VertexDesc,g_pd3dDevice,&pNewMesh);
		KGLOG_COM_PROCESS_ERROR(hr);

		KGLOG_PROCESS_ERROR(pNewMesh);

		DWORD dwFace   = pNewMesh->GetNumFaces();
		DWORD dwVertex = pNewMesh->GetNumVertices();

		int nBoneIndex = 0;
		int nWeightIndex = 0;
		int nPos = 0;
		int nPos1 = 0;
		int nNormal = 0;
		int nNormal1 = 0;
		int nUV = 0;

		int VertexSize = D3DXGetDeclVertexSize(VertexDesc,0);

		for (int i=0;i<MAX_FVF_DECL_SIZE;i++)
		{
			if(VertexDesc[i].Stream < 0xFF)
			{
				if(VertexDesc[i].Usage ==  D3DDECLUSAGE_BLENDINDICES)
				{
					nBoneIndex = VertexDesc[i].Offset;
				}

				if(VertexDesc[i].Usage == D3DDECLUSAGE_BLENDWEIGHT)
				{
					nWeightIndex = VertexDesc[i].Offset;
				}

				if(VertexDesc[i].Usage == D3DDECLUSAGE_POSITION)
				{
					if(VertexDesc[i].UsageIndex == 0)
					{
						nPos = VertexDesc[i].Offset;
					}
					else if(VertexDesc[i].UsageIndex == 1)
					{
						nPos1 = VertexDesc[i].Offset;
					}
				}

				if(VertexDesc[i].Usage == D3DDECLUSAGE_NORMAL)
				{
					if (VertexDesc[i].UsageIndex==0)
					{
						nNormal = VertexDesc[i].Offset;
					}
					else if(VertexDesc[i].UsageIndex==1)
					{
						nNormal1 = VertexDesc[i].Offset;
					}
				}

				if(VertexDesc[i].Usage == D3DDECLUSAGE_TEXCOORD)
				{
					nUV = VertexDesc[i].Offset;
				}
			}
			else
			{
				break;
			}
		}


		BYTE* pData = NULL;
		WORD* pIndex = NULL;
		DWORD* pAttribute = NULL;
		hr = pNewMesh->LockVertexBuffer(0,(void**)&pData);
		KGLOG_COM_PROCESS_ERROR(hr);

		hr = pNewMesh->LockIndexBuffer(0,(void**)&pIndex);
		KGLOG_COM_PROCESS_ERROR(hr);

		hr = pNewMesh->LockAttributeBuffer(0,&pAttribute);
		KGLOG_COM_PROCESS_ERROR(hr);

		SortBonesByVertexData(dwVertex,dwFace,pData,VertexSize,nBoneIndex,nWeightIndex,nPos,nPos1,nNormal,nNormal1,nUV,pIndex,pAttribute);

		hr = pNewMesh->UnlockVertexBuffer();
		KGLOG_COM_PROCESS_ERROR(hr);

		hr = pNewMesh->UnlockIndexBuffer();
		KGLOG_COM_PROCESS_ERROR(hr);

		hr = pNewMesh->UnlockAttributeBuffer();
		KGLOG_COM_PROCESS_ERROR(hr);

		//////////////////////////////////////////////////////////////////////////
		DWORD dwFVF = 0;
		hr = m_lpMeshSrc->GetMeshSysFVF(&dwFVF);
		KGLOG_COM_PROCESS_ERROR(hr);

		SAFE_RELEASE(m_lpMeshSys);
		hr = pNewMesh->CloneMeshFVF(0,dwFVF,g_pd3dDevice,&m_lpMeshSys);
		KGLOG_COM_PROCESS_ERROR(hr);

		hr = m_lpMeshSys->GetDeclaration(VertexDesc);
		KGLOG_COM_PROCESS_ERROR(hr);

		VertexSize = D3DXGetDeclVertexSize(VertexDesc,0);

		for (int i=0;i<MAX_FVF_DECL_SIZE;i++)
		{
			if(VertexDesc[i].Stream < 0xFF)
			{

				if(VertexDesc[i].Usage == D3DDECLUSAGE_POSITION)
				{
					if(VertexDesc[i].UsageIndex == 0)
					{
						nPos = VertexDesc[i].Offset;
					}
				}

				if(VertexDesc[i].Usage == D3DDECLUSAGE_NORMAL)
				{
					if (VertexDesc[i].UsageIndex==0)
					{
						nNormal = VertexDesc[i].Offset;
					}
				}
				if(VertexDesc[i].Usage == D3DDECLUSAGE_TEXCOORD)
				{
					nUV = VertexDesc[i].Offset;
				}
			}
			else
			{
				break;
			}
		}

		dwFace   = pNewMesh->GetNumFaces();
		dwVertex = pNewMesh->GetNumVertices();
	
		//////////////////////////////////////////////////////////////////////////
		hr = m_lpMeshSys->LockVertexBuffer(0,(void**)&pData);
		KGLOG_COM_PROCESS_ERROR(hr);

		hr = m_lpMeshSys->LockAttributeBuffer(0,&pAttribute);
		KGLOG_COM_PROCESS_ERROR(hr);

		FillVertexSys(dwVertex,dwFace,pData,VertexSize,nPos,nNormal,nUV,pAttribute);

		hr = m_lpMeshSys->UnlockVertexBuffer();
		KGLOG_COM_PROCESS_ERROR(hr);

		hr = m_lpMeshSys->UnlockAttributeBuffer();
		KGLOG_COM_PROCESS_ERROR(hr);

	}
	else
	{
		DWORD dwFVF = 0;
		hr = m_lpMeshSrc->GetMeshSysFVF(&dwFVF);
		KGLOG_COM_PROCESS_ERROR(hr);

		SAFE_RELEASE(m_lpMeshSys);
		hr = pMeshProgress->CloneMeshFVF(0,dwFVF,g_pd3dDevice,&m_lpMeshSys);
		KGLOG_COM_PROCESS_ERROR(hr);
	}

	//////////////////////////////////////////////////////////////////////////
	hr = SaveToMesh(pName);
	KGLOG_COM_PROCESS_ERROR(hr);

	hr = S_OK;

Exit0:
	
	SAFE_RELEASE(pNewMesh);

	return S_OK;
}