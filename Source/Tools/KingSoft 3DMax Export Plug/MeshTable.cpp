// MeshTable.cpp: implementation of the CMeshTable class.
//
//////////////////////////////////////////////////////////////////////
#include "header.h"
#include "MeshTable.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
extern void printMat(const CMesh::_Matrix& mat, FILE* fp);

CMeshTable::CMeshTable()
{
	m_dMaxNumMesh = 0;
	m_dNumMesh = 0 ;
	m_lpMeshes = 0;
}

CMeshTable::~CMeshTable()
{

}

HRESULT CMeshTable::Get1NewMeshPointer(LPMESH **pppMesh)
{
	if(m_dMaxNumMesh ==0 )
	{
		m_lpMeshes = new LPMESH[5];
		ZeroMemory(m_lpMeshes ,sizeof(LPMESH)*5);
		m_dMaxNumMesh = 5;
	}
	if(m_dNumMesh >= m_dMaxNumMesh)
	{
		LPMESH* Temp = m_lpMeshes;
		m_dMaxNumMesh +=5;
		m_lpMeshes = new LPMESH[m_dMaxNumMesh];
		ZeroMemory(m_lpMeshes ,sizeof(LPMESH)*m_dMaxNumMesh);
		for(DWORD i=0;i<m_dMaxNumMesh-5 ;i++) m_lpMeshes[i] = Temp[i];
		delete [] Temp;
	}	
	m_dNumMesh ++;
	(*pppMesh) = &m_lpMeshes[ m_dNumMesh -1 ];

	return S_OK;
}

HRESULT CMeshTable::Get1NewMesh(LPMESH *ppMesh)
{
	HRESULT hr = E_FAIL;
	CMesh** ppNewPointer = NULL;
	
	hr = Get1NewMeshPointer(&ppNewPointer);
	KG_COM_PROCESS_ERROR(hr);

	(*ppNewPointer) = new CMesh;
	KGLOG_PROCESS_ERROR(*ppNewPointer);
	
	(*ppNewPointer)->ID = m_dNumMesh;
	(*ppMesh) = (*ppNewPointer);

	hr = S_OK;
Exit0:
	return hr;
}

HRESULT CMeshTable::GetMesh(LPMESH *ppMesh, DWORD ID)
{
	if((ID>m_dNumMesh)||(ID==0)) return E_FAIL;
	(*ppMesh) = m_lpMeshes[ID-1];
	return S_OK;
}

HRESULT CMeshTable::CleanUp()
{
	for(DWORD i=0;i<m_dNumMesh;i++)
	{
		LPMESH pMesh = m_lpMeshes[i];
		pMesh->CleanUp();
		delete pMesh;
		m_lpMeshes[i] = NULL;
	}
	if(m_lpMeshes!=NULL)
	{
		delete [] m_lpMeshes;
		m_lpMeshes = NULL;
	}
	m_dNumMesh = 0;
	m_dMaxNumMesh = 0;
	return S_OK;
}

HRESULT CMeshTable::UnitMesh(DWORD *pNewMeshID)
{
	DWORD NumMesh = m_dNumMesh;
	DWORD NumVertices = 0;
	DWORD NumFaces = 0;
	DWORD NumSubset = 0;
	DWORD NumBone = 0;
	DWORD NumAnimatedBone = 0;
	DWORD NumAnimatedVertice = 0;
	DWORD AnimationType = 0;
	DWORD NumFrames = 0;
	float FrameLength = 0;
	BOOL  Skined = FALSE;
	LPSTR* ppBoneName = new LPSTR[200];
	LPSTR* ppAnimatedBoneName = new LPSTR[200];
	BOOL bUVAnimation = FALSE;
	////////////////////////////////////////////////////////////////////////////////////////
	//计算一下大小先
	for(DWORD i = 0; i < NumMesh; i++)
	{
		LPMESH pMesh = m_lpMeshes[i];
		NumVertices += pMesh->m_dNumVertices;
		NumFaces += pMesh->m_dNumFaces;
		NumSubset += pMesh->m_dNumSubsets;
		NumAnimatedVertice += pMesh->m_dNumAnimationVertices;
		AnimationType |= pMesh->m_dwAnimationType;
		NumFrames = max(pMesh->m_dNumFrames, NumFrames);
		FrameLength = max(pMesh->m_fFrameLength, FrameLength);
		if(pMesh->m_lpAnimationUV)
			bUVAnimation |= TRUE;
        if(Skined && !pMesh->m_bSkined)
			MessageBox(NULL, "混合导出了蒙皮和未蒙皮的模型， 结果可能出现异常", "提示", MB_OK);
		Skined |= pMesh->m_bSkined;
		
		for(DWORD i_Bone=0;i_Bone<pMesh->m_dNumBone;i_Bone++)
		{
			CMesh::_BoneInfo* pInfo = &pMesh->m_lpBoneInfo[i_Bone];
			int BoneIndex = FindBone(pInfo->szName,ppBoneName,NumBone);
			if(BoneIndex==-1)
			{
				ppBoneName[NumBone] = new TCHAR[30];
				wsprintf(ppBoneName[NumBone],"%s",pInfo->szName);
				NumBone++;
			}
		}
		for(i_Bone=0;i_Bone<pMesh->m_dNumAnimationBones;i_Bone++)
		{
			LPSTR pBoneName = pMesh->m_lpAnimationBonesName[i_Bone];
			int BoneIndex = FindBone(pBoneName,ppAnimatedBoneName,NumAnimatedBone);
			if(BoneIndex==-1)
			{
				ppAnimatedBoneName[NumAnimatedBone] = new TCHAR[30];
				wsprintf(ppAnimatedBoneName[NumAnimatedBone],"%s",pBoneName);
				NumAnimatedBone++;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	//创建一个新的MESH保存合并后的数据
	LPMESH pNewMesh = NULL;
	if(FAILED(Get1NewMesh(&pNewMesh)))
		return E_FAIL;
	pNewMesh->m_dNumVertices = NumVertices;
	pNewMesh->m_dNumFaces = NumFaces;
	pNewMesh->m_dNumSubsets = NumSubset;
	pNewMesh->m_lpVerticePosition  = new CMesh::VEC3[NumVertices];
	pNewMesh->m_lpVerticeNormal    = new CMesh::VEC3[NumVertices];
	pNewMesh->m_lpVerticeTextCoord1 = new CMesh::VEC3[NumVertices];
	pNewMesh->m_lpVerticeTextCoord2 = new CMesh::VEC3[NumVertices];
	pNewMesh->m_lpVerticeTextCoord3 = new CMesh::VEC3[NumVertices];
	pNewMesh->m_lpVerticeDiffuse = new CMesh::COLOR4[NumVertices];

	pNewMesh->m_lpAnimationVerticesIndex = new DWORD[NumAnimatedVertice];
	bUVAnimation ?
		pNewMesh->m_lpAnimationUV = new CMesh::VEC3[NumAnimatedVertice * NumFrames] :
		pNewMesh->m_lpAnimationUV = NULL;

	pNewMesh->m_lpAnimationVerticesPosition  = new CMesh::VEC3[NumAnimatedVertice*NumFrames];
	pNewMesh->m_lpFacesIndex = new DWORD[NumFaces*3];
	pNewMesh->m_lpSubsetID   = new DWORD[NumFaces];
	pNewMesh->m_lpMaterial   = new CMesh::MATERIAL[NumSubset];
	if(Skined||NumAnimatedBone>0)
	{
		pNewMesh->m_dNumBone = NumBone;
		pNewMesh->m_lpBoneInfo = new CMesh::BONEINFO[NumBone];
		ZeroMemory(pNewMesh->m_lpBoneInfo,sizeof(CMesh::BONEINFO)*NumBone);

		pNewMesh->m_dNumAnimationBones = NumAnimatedBone;
		pNewMesh->m_lpAnimationBonesName = new LPSTR[NumAnimatedBone];
	
		ZeroMemory(pNewMesh->m_lpAnimationBonesName,sizeof(LPSTR)*NumAnimatedBone);
	
		DWORD AniSize = NumAnimatedBone*NumFrames;

		if(AnimationType==ANIMATION_BONE)
		{
			pNewMesh->m_lpAnimationBonesTrans = new CMesh::MATRIX[AniSize];
			ZeroMemory(pNewMesh->m_lpAnimationBonesTrans,sizeof(CMesh::MATRIX)*AniSize);
		}
		else if(AnimationType==ANIMATION_BONE_RELATIVE)
		{
			/*pNewMesh->m_lpAniBScaling     = new CMesh::VEC3[AniSize];
			pNewMesh->m_lpAniBRotation    = new CMesh::VEC4[AniSize];
			pNewMesh->m_lpAniBTranslation = new CMesh::VEC3[AniSize];

			ZeroMemory(pNewMesh->m_lpAniBScaling,sizeof(CMesh::VEC3)*AniSize);
			ZeroMemory(pNewMesh->m_lpAniBScaling,sizeof(CMesh::VEC4)*AniSize);
			ZeroMemory(pNewMesh->m_lpAniBScaling,sizeof(CMesh::VEC3)*AniSize);*/

			pNewMesh->m_lpAnimationBonesTrans = new CMesh::MATRIX[AniSize];
			ZeroMemory(pNewMesh->m_lpAnimationBonesTrans,sizeof(CMesh::MATRIX)*AniSize);
		}
		else if (AnimationType == ANIMATION_BONE_RTS )
		{
			pNewMesh->m_lpRTSAniBones = new CMesh::RTSBoneAnimation[AniSize];
			ZeroMemory(pNewMesh->m_lpRTSAniBones, sizeof(CMesh::RTSBoneAnimation) * AniSize);
		}
		else if (AnimationType == ANIMATION_BONE_KEYONLY)
		{
			//////////////////////////////////////////////////////////////////////////
			//Add here
			pNewMesh->m_lpAnimationBonesTrans = new CMesh::MATRIX[AniSize];
			ZeroMemory(pNewMesh->m_lpAnimationBonesTrans,sizeof(CMesh::MATRIX)*AniSize);
			//if ( pMesh->m_lpBoneInfo[pMesh->m_dNumBone - 1].Static )
			//	AniSize = pMesh->m_lpBoneInfo[pMesh->m_dNumBone - 1].Offset + 1;
			//else
			//	AniSize = pMesh->m_lpBoneInfo[pMesh->m_dNumBone - 1].Offset + pMesh->m_dNumFrames;
			//pNewMesh->m_lpAnimationBonesTrans = new CMesh::MATRIX[AniSize];
			//ZeroMemory(pNewMesh->m_lpAnimationBonesTrans, sizeof(CMesh::MATRIX)*AniSize);
		}
	
	}
	ZeroMemory(pNewMesh->m_lpVerticePosition  ,sizeof(CMesh::VEC3)*NumVertices);
	ZeroMemory(pNewMesh->m_lpVerticeNormal    ,sizeof(CMesh::VEC3)*NumVertices);
	ZeroMemory(pNewMesh->m_lpVerticeTextCoord1 ,sizeof(CMesh::VEC3)*NumVertices);
	ZeroMemory(pNewMesh->m_lpVerticeTextCoord2 ,sizeof(CMesh::VEC3)*NumVertices);
	ZeroMemory(pNewMesh->m_lpVerticeTextCoord3 ,sizeof(CMesh::VEC3)*NumVertices);
	ZeroMemory(pNewMesh->m_lpVerticeDiffuse ,sizeof(CMesh::COLOR4)*NumVertices);

	ZeroMemory(pNewMesh->m_lpAnimationVerticesPosition ,sizeof(CMesh::VEC3)*NumAnimatedVertice*NumFrames);
	ZeroMemory(pNewMesh->m_lpAnimationVerticesIndex ,sizeof(DWORD)*NumAnimatedVertice);
	ZeroMemory(pNewMesh->m_lpFacesIndex  ,sizeof(DWORD)*NumFaces*3);
	ZeroMemory(pNewMesh->m_lpSubsetID    ,sizeof(DWORD)*NumFaces);
	ZeroMemory(pNewMesh->m_lpMaterial    ,sizeof(CMesh::MATERIAL)*NumSubset);
	//////////////////////////////////////////////////////////////////////////
	//写入数据
	DWORD NowVertices = 0;
	DWORD NowFaces = 0 ;
	DWORD NowSubset = 0;
	DWORD NowAnimtedVertices = 0;
	
	if(pNewMesh->m_lpBoneInfo!=NULL)
	{
		for(i=0;i<NumBone;i++)
		{
			CMesh::LPBONEINFO pInfo = &pNewMesh->m_lpBoneInfo[i];
			wsprintf(pInfo->szName,"%s",ppBoneName[i]);
		}
	}
	for(i=0;i<NumAnimatedBone;i++)
	{
		pNewMesh->m_lpAnimationBonesName[i] = new TCHAR[30];
		wsprintf(pNewMesh->m_lpAnimationBonesName[i],"%s",ppAnimatedBoneName[i]);
	}
	DWORD Offset = 0;
	for( i=0;i<NumMesh;i++)
	{
		LPMESH pMesh = m_lpMeshes[i];
		for(DWORD i_Ver =0;i_Ver<pMesh->m_dNumVertices;i_Ver++)
		{
			pNewMesh->m_lpVerticePosition [i_Ver+NowVertices] = pMesh->m_lpVerticePosition[i_Ver];
			pNewMesh->m_lpVerticeNormal   [i_Ver+NowVertices] = pMesh->m_lpVerticeNormal  [i_Ver];
			pNewMesh->m_lpVerticeDiffuse   [i_Ver+NowVertices] = pMesh->m_lpVerticeDiffuse  [i_Ver];

			pNewMesh->m_lpVerticeTextCoord1[i_Ver+NowVertices] = pMesh->m_lpVerticeTextCoord1[i_Ver];
			pNewMesh->m_lpVerticeTextCoord2[i_Ver+NowVertices] = pMesh->m_lpVerticeTextCoord2[i_Ver];
			pNewMesh->m_lpVerticeTextCoord3[i_Ver+NowVertices] = pMesh->m_lpVerticeTextCoord3[i_Ver];
		}
		for(DWORD i_Face=0;i_Face<pMesh->m_dNumFaces;i_Face++)
		{
			pNewMesh->m_lpFacesIndex [(i_Face+NowFaces)*3] = pMesh->m_lpFacesIndex[i_Face*3] + NowVertices;
			pNewMesh->m_lpFacesIndex [(i_Face+NowFaces)*3+1] = pMesh->m_lpFacesIndex[i_Face*3+1] + NowVertices;
			pNewMesh->m_lpFacesIndex [(i_Face+NowFaces)*3+2] = pMesh->m_lpFacesIndex[i_Face*3+2] + NowVertices;
			pNewMesh->m_lpSubsetID[i_Face+NowFaces] = pMesh->m_lpSubsetID[i_Face] + NowSubset;
		}

		if(pMesh->m_lpMaterial!=NULL)
			for(DWORD i_Set=0;i_Set<pMesh->m_dNumSubsets;i_Set++)
		{
			pNewMesh->m_lpMaterial[i_Set+NowSubset].Ambient = 
				pMesh->m_lpMaterial[i_Set].Ambient;
			pNewMesh->m_lpMaterial[i_Set+NowSubset].Diffuse = 
				pMesh->m_lpMaterial[i_Set].Diffuse;
			pNewMesh->m_lpMaterial[i_Set+NowSubset].Emissive = 
				pMesh->m_lpMaterial[i_Set].Emissive;
			pNewMesh->m_lpMaterial[i_Set+NowSubset].Specular = 
				pMesh->m_lpMaterial[i_Set].Specular;
			pNewMesh->m_lpMaterial[i_Set+NowSubset].Power    = 
				pMesh->m_lpMaterial[i_Set].Power;
			pNewMesh->m_lpMaterial[i_Set+NowSubset].Option    = 
				pMesh->m_lpMaterial[i_Set].Option;
			pNewMesh->m_lpMaterial[i_Set+NowSubset].NumTexture = 
				pMesh->m_lpMaterial[i_Set].NumTexture;
			pNewMesh->m_lpMaterial[i_Set+NowSubset].Textures = 
				new CMesh::TEXTURE[pMesh->m_lpMaterial[i_Set].NumTexture];

			for(DWORD i_Text=0;i_Text<pMesh->m_lpMaterial[i_Set].NumTexture;i_Text++)
			{
				pNewMesh->m_lpMaterial[i_Set+NowSubset].Textures[i_Text].Amount =
					pMesh->m_lpMaterial[i_Set].Textures[i_Text].Amount ;
				pNewMesh->m_lpMaterial[i_Set+NowSubset].Textures[i_Text].FileName = 
					new TCHAR[256];
				pNewMesh->m_lpMaterial[i_Set+NowSubset].Textures[i_Text].TextureType = 
					new TCHAR[30];

				wsprintf(pNewMesh->m_lpMaterial[i_Set+NowSubset].Textures[i_Text].FileName,"%s",
					pMesh->m_lpMaterial[i_Set].Textures[i_Text].FileName) ;
				wsprintf(pNewMesh->m_lpMaterial[i_Set+NowSubset].Textures[i_Text].TextureType,"%s",
					pMesh->m_lpMaterial[i_Set].Textures[i_Text].TextureType) ;

								
				pNewMesh->m_lpMaterial[i_Set+NowSubset].NumTextureOption[i_Text] = 
					pMesh->m_lpMaterial[i_Set].NumTextureOption[i_Text];
					
				if(pMesh->m_lpMaterial[i_Set].NumTextureOption[i_Text]>0)
				{					
					pNewMesh->m_lpMaterial[i_Set+NowSubset].TexturesOption[i_Text] = 
						new CMesh::_Option[1];
					pNewMesh->m_lpMaterial[i_Set+NowSubset].TexturesOption[i_Text][0].Type = 
						OPTION_TEXTURE_TRANSFROM;
					
					CMesh::_TextureTf* pTextureTF = new CMesh::_TextureTf;
					ZeroMemory(pTextureTF,sizeof(CMesh::_TextureTf));
					CMesh::_TextureTf* pSrcTextureTF = (CMesh::_TextureTf*) 
						pMesh->m_lpMaterial[i_Set].TexturesOption[i_Text][0].pData;
					
					pTextureTF->Scaling.x = pSrcTextureTF->Scaling.x;
					pTextureTF->Scaling.y = pSrcTextureTF->Scaling.y;
					pTextureTF->Scaling.z = pSrcTextureTF->Scaling.z;
					
					pNewMesh->m_lpMaterial[i_Set+NowSubset].TexturesOption[i_Text][0].pData = 
						pTextureTF;
				}
			}
		}
		if(AnimationType == ANIMATION_VERTICES_RELATIVE)
		{
			for( i_Ver =0;i_Ver<pMesh->m_dNumAnimationVertices;i_Ver++)
			{
				DWORD NewIndex = i_Ver+NowAnimtedVertices;
				pNewMesh->m_lpAnimationVerticesIndex[NewIndex]
					= pMesh->m_lpAnimationVerticesIndex[i_Ver] + NowVertices;
				for(DWORD i_Frame=0;i_Frame<NumFrames;i_Frame++)
				{
					pNewMesh->m_lpAnimationVerticesPosition[NewIndex*NumFrames+i_Frame] =
						pMesh->m_lpAnimationVerticesPosition[i_Ver*NumFrames+i_Frame];
				}
			}
		}
		if(AnimationType == ANIMATION_VERTICES)
		{
			for( i_Ver =0;i_Ver<pMesh->m_dNumAnimationVertices;i_Ver++)
			{
				DWORD NewIndex = i_Ver+NowAnimtedVertices;
				pNewMesh->m_lpAnimationVerticesIndex[NewIndex]
					= pMesh->m_lpAnimationVerticesIndex[i_Ver] + NowVertices;
				for(DWORD i_Frame=0;i_Frame<NumFrames;i_Frame++)
				{
					pNewMesh->m_lpAnimationVerticesPosition[NewIndex * NumFrames + i_Frame] =
						pMesh->m_lpAnimationVerticesPosition[i_Ver * NumFrames + i_Frame];
					
					if(pNewMesh->m_lpAnimationUV)
					{
						pNewMesh->m_lpAnimationUV[NewIndex * NumFrames + i_Frame] = 
							pMesh->m_lpAnimationUV[i_Ver * NumFrames + i_Frame];
					}
				}
			}
		}

		if(Skined)
		{
			for(DWORD i_Bone=0;i_Bone<pMesh->m_dNumBone;i_Bone++)
			{
				CMesh::LPBONEINFO pInfo = &pMesh->m_lpBoneInfo[i_Bone];
				int BoneIndex = FindBone(pInfo->szName,pNewMesh->m_lpBoneInfo,NumBone);
				if(BoneIndex>=0)
				{
					CMesh::LPBONEINFO pBoneInfo = &pNewMesh->m_lpBoneInfo[BoneIndex];
					
					wsprintf(pBoneInfo->szParentName,"%s",pInfo->szParentName);
					if((pBoneInfo->NumChild==0)&&(pInfo->NumChild!=0))
					{
						pBoneInfo->NumChild = pInfo->NumChild;
						pBoneInfo->lpChildNames = new LPSTR[pBoneInfo->NumChild];
						ZeroMemory(pBoneInfo->lpChildNames,sizeof(LPSTR)*pBoneInfo->NumChild);
						for(DWORD i_Child=0;i_Child<pInfo->NumChild;i_Child++)
						{
							pBoneInfo->lpChildNames[i_Child] = new TCHAR[30];
							wsprintf(pBoneInfo->lpChildNames[i_Child],"%s",
								pInfo->lpChildNames[i_Child]);
						}
					}
					pBoneInfo->MatrixOffset = pInfo->MatrixOffset;
					pBoneInfo->BaseBoneMatrix = pInfo->BaseBoneMatrix;
					
					DWORD NumOldVerticesRef = pBoneInfo->NumVerticesRef;

					DWORD* TempVerticesIndex = pBoneInfo->VerticesIndexRef;
					float* TempVerticesWeight = pBoneInfo->VerticesWeightRef;
					pBoneInfo->NumVerticesRef += pInfo->NumVerticesRef;
					
					pBoneInfo->VerticesIndexRef  = new DWORD[pBoneInfo->NumVerticesRef];
					pBoneInfo->VerticesWeightRef = new float[pBoneInfo->NumVerticesRef];
					ZeroMemory(pBoneInfo->VerticesIndexRef,sizeof(DWORD)*pBoneInfo->NumVerticesRef);
					ZeroMemory(pBoneInfo->VerticesWeightRef,sizeof(float)*pBoneInfo->NumVerticesRef);
					
					for(DWORD i_Ref=0;i_Ref<NumOldVerticesRef;i_Ref++)
					{
						pBoneInfo->VerticesIndexRef[i_Ref] = TempVerticesIndex[i_Ref];
						pBoneInfo->VerticesWeightRef[i_Ref] = TempVerticesWeight[i_Ref];
					}
					for( i_Ref=0;i_Ref<pInfo->NumVerticesRef;i_Ref++)
					{
						pBoneInfo->VerticesIndexRef [i_Ref+NumOldVerticesRef] = 
							pInfo->VerticesIndexRef[i_Ref] + NowVertices;
						pBoneInfo->VerticesWeightRef[i_Ref+NumOldVerticesRef] = 
							pInfo->VerticesWeightRef[i_Ref];
					}
					pBoneInfo->Static = pInfo->Static;
                    pBoneInfo->Offset = pInfo->Offset;
					if(TempVerticesIndex!=NULL) delete [] TempVerticesIndex;
					if(TempVerticesWeight!=NULL) delete [] TempVerticesWeight;
					TempVerticesIndex = NULL;
					TempVerticesWeight= NULL;
				}
			}
		}
		if(AnimationType==ANIMATION_BONE)
		{
			for(DWORD i_Bone=0;i_Bone<pMesh->m_dNumAnimationBones;i_Bone++)
			{
				LPSTR pBoneName = pMesh->m_lpAnimationBonesName[i_Bone];
				int BoneIndex = FindBone(pBoneName,pNewMesh->m_lpAnimationBonesName,NumAnimatedBone);
				if(BoneIndex>=0)
				{
					for(DWORD i_Frame=0;i_Frame<NumFrames;i_Frame++)
					{
						pNewMesh->m_lpAnimationBonesTrans[BoneIndex*NumFrames+i_Frame] =
							pMesh->m_lpAnimationBonesTrans[i_Bone*NumFrames+i_Frame];
					}
				}
			}
		}
		else if(AnimationType==ANIMATION_BONE_RELATIVE)
		{
			//FILE* fp = fopen("d:\\debug_relative_export.txt", "w+");
			for(DWORD i_Bone=0;i_Bone<pMesh->m_dNumAnimationBones;i_Bone++)
			{
				LPSTR pBoneName = pMesh->m_lpAnimationBonesName[i_Bone];
				int BoneIndex = FindBone(pBoneName,pNewMesh->m_lpAnimationBonesName,NumAnimatedBone);
				if(BoneIndex>=0)
				{
					//fprintf(fp, "bone: %d\n========================================\n", i_Bone);
					for(DWORD i_Frame=0;i_Frame<NumFrames;i_Frame++)
					{
						pNewMesh->m_lpAnimationBonesTrans[BoneIndex*NumFrames+i_Frame] =
							pMesh->m_lpAnimationBonesTrans[i_Bone*NumFrames+i_Frame];

						//printMat(pMesh->m_lpAnimationBonesTrans[BoneIndex * NumFrames + i_Frame], fp);
						//fprintf(fp, "========================\n");
						/*pNewMesh->m_lpAniBScaling[BoneIndex*NumFrames+i_Frame] =
							pMesh->m_lpAniBScaling[i_Bone*NumFrames+i_Frame];

						pNewMesh->m_lpAniBRotation[BoneIndex*NumFrames+i_Frame] =
							pMesh->m_lpAniBRotation[i_Bone*NumFrames+i_Frame];

						pNewMesh->m_lpAniBTranslation[BoneIndex*NumFrames+i_Frame] =
							pMesh->m_lpAniBTranslation[i_Bone*NumFrames+i_Frame];*/
					}
				}
			}
		}
		else if( AnimationType == ANIMATION_BONE_RTS ) 
		{
			for( DWORD i_Bone = 0; i_Bone<pMesh->m_dNumAnimationBones; i_Bone++ )
			{
				LPSTR pBoneName = pMesh->m_lpAnimationBonesName[i_Bone];
				int BoneIndex = FindBone(pBoneName,pNewMesh->m_lpAnimationBonesName,NumAnimatedBone);
				if(BoneIndex>=0)
				{
					for(DWORD i_Frame=0;i_Frame<NumFrames;i_Frame++)
					{
						pNewMesh->m_lpRTSAniBones[BoneIndex*NumFrames + i_Frame] = pMesh->m_lpRTSAniBones[i_Bone*NumFrames+i_Frame];
					}
				}

			}
		}
		else if(AnimationType == ANIMATION_BONE_KEYONLY )
		{
			//////////////////////////////////////////////////////////////////////////
			//Add here
			//Rearrange all mesh
			//FILE* fp = fopen("d:\\debug_before_rearrange.txt", "w+");
			//FILE* fp1 = fopen("d:\\debug_after_rearrange.txt", "w+");
			for (DWORD iBone = 0; iBone < pMesh->m_dNumBone; iBone++ )
			{
				LPSTR pBoneName = pMesh->m_lpAnimationBonesName[iBone];
				int BoneIndex = FindBone(pBoneName,pNewMesh->m_lpAnimationBonesName,NumAnimatedBone);
				pNewMesh->m_lpBoneInfo[BoneIndex].Static = pMesh->m_lpBoneInfo[iBone].Static;
				if(BoneIndex>=0)
				{
					if ( pMesh->m_lpBoneInfo[iBone].Static )
					{
						pNewMesh->m_lpAnimationBonesTrans[Offset] = pMesh->m_lpAnimationBonesTrans[pMesh->m_dNumFrames * iBone];
						//fprintf(fp, "Static Bone\n");
						//fprintf(fp1, "Static Bone\n");
						//fprintf(fp, "bone: %d\n========================================\n", iBone);
						//fprintf(fp1, "bone: %d %s\n========================================\n", BoneIndex, pBoneName);
						//printMat(pMesh->m_lpAnimationBonesTrans[pMesh->m_dNumFrames * iBone], fp);
                        //fprintf(fp, "========================\n");
						//printMat(pNewMesh->m_lpAnimationBonesTrans[Offset], fp1);
						//fprintf(fp1, "========================\n");
						Offset++;
						pNewMesh->m_lpBoneInfo[BoneIndex].Offset = Offset - 1;
					}
					else
					{
						
						//fprintf(fp, "bone: %d\n========================================\n", iBone);
						//fprintf(fp1, "bone: %d %s\n========================================\n", BoneIndex, pBoneName);
						for ( DWORD dwFrame = 0; dwFrame < pMesh->m_dNumFrames; dwFrame++ )
						{
							pNewMesh->m_lpAnimationBonesTrans[Offset] = pMesh->m_lpAnimationBonesTrans[pMesh->m_dNumFrames * iBone + dwFrame];
							//printMat(pMesh->m_lpAnimationBonesTrans[pMesh->m_dNumFrames * iBone + dwFrame], fp);
							//fprintf(fp, "========================\n");
							//printMat(pNewMesh->m_lpAnimationBonesTrans[Offset], fp1);
							//fprintf(fp1, "========================\n");
							Offset++;
						}
						pNewMesh->m_lpBoneInfo[BoneIndex].Offset = Offset - pMesh->m_dNumFrames;
						
					}
					
				}
				//else
				//{
					//fprintf(fp1, "bone %s is lost.\n", pBoneName);
				//}
			}
		
	
			//fclose(fp);
			//fclose(fp1);

		}
		NowVertices += pMesh->m_dNumVertices ;
		NowFaces    += pMesh->m_dNumFaces;
		NowSubset   += pMesh->m_dNumSubsets;
		NowAnimtedVertices += pMesh->m_dNumAnimationVertices;

		//////////////////////////////////////////////////////////////////////////
		list<CMesh::_Socket>::iterator i_Socket = pMesh->m_listSocket.begin();
		while(i_Socket!=pMesh->m_listSocket.end())
		{
			CMesh::_Socket Socket = *i_Socket;
			pNewMesh->m_listSocket.remove(Socket);
			pNewMesh->m_listSocket.push_back(Socket);
			i_Socket++;
		}

        //------- Added By Zeb 2005-11-17 -------
        i_Socket = pMesh->m_listCollisionSurface.begin();
        while(i_Socket != pMesh->m_listCollisionSurface.end())
        {
            CMesh::_Socket Socket = *i_Socket;
            pNewMesh->m_listCollisionSurface.remove(Socket);
            pNewMesh->m_listCollisionSurface.push_back(Socket);
            i_Socket++;
        }
        //------- Added By Zeb End --------------
	}
	
	

	pNewMesh->m_dNumVertices = NumVertices;
	pNewMesh->m_dNumFaces    = NumFaces;
	pNewMesh->m_dNumSubsets  = NumSubset;
	pNewMesh->m_dwAnimationType = AnimationType;
	pNewMesh->m_fFrameLength = FrameLength;
	pNewMesh->m_dNumFrames  = NumFrames;
	pNewMesh->m_dNumAnimationVertices = NumAnimatedVertice;
	
	pNewMesh->m_dNumAnimationBones = NumAnimatedBone;
	pNewMesh->m_bSkined = Skined;

	/*fprintf(pFile,"Num Vertices  %d \n",pNewMesh->m_dNumVertices);
	for(DWORD i_Ver =0;i_Ver<pNewMesh->m_dNumVertices;i_Ver++)
	{
		fprintf(pFile,"Ver %d Pos %f %f %f \n",i_Ver,
			pNewMesh->m_lpVerticePosition[i_Ver].x,
			pNewMesh->m_lpVerticePosition[i_Ver].y,
			pNewMesh->m_lpVerticePosition[i_Ver].z);
	}
	fprintf(pFile,"Num Faces  %d \n",pNewMesh->m_dNumFaces);
	for(DWORD i_Face=0;i_Face<pNewMesh->m_dNumFaces;i_Face++)
	{
		DWORD A = pNewMesh->m_lpFacesIndex[i_Face*3];
		DWORD B = pNewMesh->m_lpFacesIndex[i_Face*3];
		DWORD C = pNewMesh->m_lpFacesIndex[i_Face*3];
		fprintf(pFile,"Faces %d Index %d %d %d Sunset %d\n",i_Face,A,B,C,
			pNewMesh->m_lpSubsetID[i_Face]);
	}
	fprintf(pFile,"Num Subset  %d \n",pNewMesh->m_dNumSubsets);
	fclose(pFile);
	*/
	for(i=0;i<NumBone;i++)
	{
		if(ppBoneName[i]!=NULL)
		{
			delete [] ppBoneName[i];
			ppBoneName[i] = NULL;
		}
	}
	if(ppBoneName!=NULL)
	{
		delete [] ppBoneName;
		ppBoneName = NULL;
	}
	for(i=0;i<NumAnimatedBone;i++)
	{
		if(ppAnimatedBoneName[i]!=NULL)
		{
			delete [] ppAnimatedBoneName[i];
			ppAnimatedBoneName[i] = NULL;
		}
	}
	if(ppAnimatedBoneName!=NULL)
	{
		delete [] ppAnimatedBoneName;
		ppAnimatedBoneName = NULL;
	}
	
	(*pNewMeshID) = pNewMesh->ID;
	return S_OK;
}

int CMeshTable::FindBone(LPSTR pBoneName, LPSTR *ppBones, DWORD NumBone)
{
	if(ppBones!=NULL)
	{
		for(DWORD i_Bone=0;i_Bone<NumBone;i_Bone++)
		{
			LPSTR pBone = ppBones[i_Bone];
			if(strcmp(pBoneName,pBone)==0)
				return i_Bone;
		}
		return -1;
	}
	return -1;
}

int CMeshTable::FindBone(LPSTR pBoneName, CMesh::LPBONEINFO ppBones, DWORD NumBone)
{
	if(ppBones!=NULL)
	{
		for(DWORD i_Bone=0;i_Bone<NumBone;i_Bone++)
		{
			CMesh::LPBONEINFO pInfo = &ppBones[i_Bone];
			LPSTR pBone = pInfo->szName;
			if(strcmp(pBoneName,pBone)==0)
				return i_Bone;
		}
		return -1;
	}
	return -1;
}
