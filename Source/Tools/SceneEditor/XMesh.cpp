// XMesh.cpp: implementation of the KModel class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SceneEditor.h"
#include "XMesh.h"
#include <direct.h>
#include ".\xmesh.h"
#include "KSequence.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


LPD3DXMESH g_pBlendDebug = NULL;
extern MFCFramework::KSceneEditorApp theApp;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


void swap(float& fData1, float fData2)
{
	float fTemp = fData1;
	fData1 = fData2;
	fData2 = fTemp;
}

void ExchangeMatrixInputYZ(D3DXMATRIX& mat)
{
	swap(mat._31, mat._21);
	swap(mat._32, mat._22);
	swap(mat._33, mat._23);
}

void printMat(const D3DXMATRIX& mat, FILE* fp)
{
	assert(fp);
	for( int i = 0; i < 4; i++ )
	{
		for ( int j = 0; j < 4; j++ )
		{
			fprintf(fp, "%f, ", mat.m[i][j]);
		}
		fprintf(fp, "\n");
	}
	fprintf(fp, "\n");
}

KModel::KModel()
{
	m_pVerticesLodWeight = NULL;

	m_pMeshEnhanced = NULL;
	m_pMeshBeforeProgress = NULL;
	m_pMeshProgress = NULL;
	m_nNumCurFacesLOD = NULL;
	m_bRecaculateNormal = false;
	m_fPlanarShadowHeight = 0;
	 
	m_dwType = 0;
	m_nCurAnimationIndex = -1;

	m_BBox_A = D3DXVECTOR3(0,0,0);
	m_BBox_B = D3DXVECTOR3(0,0,0);

	m_nBillBoardMethod = 0;
	ID = 0;
	m_dNumMaterial = 0;
	m_lpMaterial = NULL;
	m_pMeshOpt = NULL;
	m_pBlendMesh = NULL;
	m_lpStaticVertexPos = NULL;
	m_pBoneCombinationTable = NULL;
	m_MaxFaceInfl = 0;
	m_NumAttributeGroups = 0;
	m_pBoneMatrices = NULL;
	m_iAttributeSW = 0;
	m_dOption = 0;
	m_pVertexRemap = NULL;
	m_pCartoonShader = NULL;
	m_pFrameShader = NULL;

	m_dwShadow = 0;

	wsprintf(m_szMeshFileName,"");
	wsprintf(m_szMtlFileName ,"");
	wsprintf(m_szAniFileName ,"");

	m_bSHPRTReadyToRender = false;
	m_pPRTBuffer = NULL;
	m_pPRTCompBuffer = NULL;
	m_pPRTEffect = NULL;       
	m_pSHIrradEnvMapEffect = NULL;       
	m_pNDotLEffect = NULL;

	m_ShadowVolume.EdageVertices = NULL;
	m_ShadowVolume.Edages        = NULL;
	m_ShadowVolume.NumVertices   = 0;

	m_nNumCurBlendSet = 0;


	//////////////////////////////////////////////////////////////////////////
	//PRT parameters
	m_pVBNormal = NULL;
	m_bDrawNormal = FALSE;

	m_hPRTSimulationThreadId = NULL;
	m_dwPRTSimulationThreadId = 0;

	m_pSHPRTBuffer = NULL;
	m_aClusterBases = NULL; 
	m_aClusteredPCA = NULL;

	m_dwNumClusters = 13;
	m_dwNumPCAVectors = 8;

	m_hPRTSimulationThreadId = NULL;
	m_pMeshPRT = NULL;
	m_pSHPRTEffect = NULL;

	m_DiffuseReflectance.r = 1.0f;
	m_DiffuseReflectance.b = 1.0f;
	m_DiffuseReflectance.g = 1.0f;
	m_DiffuseReflectance.a = 0.0f;

	m_Absoption.r = 0.03f;
	m_Absoption.g = 0.17f;
	m_Absoption.b = 0.48f;
	m_Absoption.a = 0.0f;

	m_ReducedScattering.r = 0.74f;
	m_ReducedScattering.g = 0.88f;
	m_ReducedScattering.b = 1.01f;
	m_ReducedScattering.a = 0.0f;

	m_dwOrder = 6;
	m_dwNumRays = 1024;
	m_dwNumBounces = 1;
	m_bSubsurfaceScattering = false;
	m_fLengthScale = 25.0f;
	m_bSpectral = true;

	m_fPRTLightScale = 1.00f;
	m_fPRTConeRadius = 0.5f;
	m_fRelativeIndexOfRefraction = 1.33f;

	//m_vDefaultLightDir[0] = D3DXVECTOR3(0.37f,0.36f,-0.86f);
	m_vDefaultLightDir[0] = D3DXVECTOR3(0.37f,0,300.0f);
	m_vDefaultLightDir[1] = D3DXVECTOR3(9999,9999,9999);
	m_vDefaultLightDir[2] = D3DXVECTOR3(9999,9999,9999);
	for( int i=0; i<MAX_LIGHTS; i++ )
	{
		m_vLightDir[i] = m_vDefaultLightDir[i];
		D3DXMatrixIdentity( &m_mLightRot[i] );
		D3DXMatrixIdentity( &m_mLightRotSnapshot[i] );
	}
	m_nActiveLight = 0;
	m_nNumActiveLights = 3;
	m_fLightScale = 1.0f;
	m_fLengthScale = 25.0f;
	m_aClusterBases = NULL;
	m_aPRTConstants = NULL;
	m_pSHIrradEnvMapEffect = NULL;
	m_pNDotLEffect = NULL;

	m_PRTLightColor = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);        
	memset(&m_PRTReloadState, 0, sizeof(m_PRTReloadState));
	m_bDrawBoneTrace = FALSE;

	m_pBondID = NULL;

#ifdef KEY_FRAME_INTERPOLATION_BUG
	
	m_pFrameA = NULL;
	m_pFrameB = NULL;
	m_nFrameA = -1;
	m_nFrameB = -1;

#endif

	m_bPlaySequence = FALSE;
	m_pCurSeq = new KSequence;
	m_pCurSeq->LinkToModel(this);


}

KModel::~KModel()
{

}

HRESULT KModel::Acquire()
{
	if (!m_bNeedReAcquire)
		return S_OK;

	PropertyBase::_PropertyPage* pPage = NULL;
	int PageIndex = -1;

	PageIndex = this->AddPage("信息");

	pPage = this->FindPage(PageIndex);
	if(pPage)
	{
		PropertyBase::_PropertyGroup Group(0,"文件名",FALSE);
		int index_Group = pPage->AddGroup(Group);
		PropertyBase::_PropertyGroup* pGroup = pPage->FindGroup(index_Group);

		pGroup->AddPropertyString("Mesh File",m_szMeshFileName);
		pGroup->AddPropertyString("Mtl File", m_szMtlFileName);
		pGroup->AddPropertyString("Ani File", m_szAniFileName);

		PropertyBase::_PropertyGroup Group1(0,"基本数据",FALSE);
		index_Group = pPage->AddGroup(Group1);
		pGroup = pPage->FindGroup(index_Group);

		pGroup->AddPropertyInt("材质数目",(int*)(&m_dNumMaterial),m_dNumMaterial,m_dNumMaterial);

	}
	m_bNeedReAcquire = FALSE;
	PageIndex = this->AddPage("蒙皮");
	pPage = this->FindPage(PageIndex);
	if(pPage)
	{
		{
			PropertyBase::_PropertyGroup Group(0,"基本信息",FALSE);
			int index_Group = pPage->AddGroup(Group);
			PropertyBase::_PropertyGroup* pGroup = pPage->FindGroup(index_Group);

			pGroup->AddPropertyInt("Num of Bone",(int*)(&m_dNumBone),m_dNumBone,m_dNumBone);
		}
	}
	PageIndex = this->AddPage("材质");
	pPage = this->FindPage(PageIndex);
	if(pPage)
	{
		{
			PropertyBase::_PropertyGroup Group(0,"材质列表",FALSE);
			int index_Group = pPage->AddGroup(Group);
			PropertyBase::_PropertyGroup* pGroup = pPage->FindGroup(index_Group);

			pGroup->AddPropertyMtlList("MtlList",this);
		}
		{
			PropertyBase::_PropertyGroup Group(0,"材质基本选项",FALSE);
			int index_Group = pPage->AddGroup(Group);
			PropertyBase::_PropertyGroup* pGroup = pPage->FindGroup(index_Group);
			pGroup->AddPropertyMtlOption("MaterialOption",this);

		}
		{
			PropertyBase::_PropertyGroup Group(0,"贴图选项",FALSE);
			int index_Group = pPage->AddGroup(Group);
			PropertyBase::_PropertyGroup* pGroup = pPage->FindGroup(index_Group);
			pGroup->AddPropertyMtlTex("TextureList",this);
		}
		{
			PropertyBase::_PropertyGroup Group(0,"渲染状态选项",FALSE);
			int index_Group = pPage->AddGroup(Group);
			PropertyBase::_PropertyGroup* pGroup = pPage->FindGroup(index_Group);
			pGroup->AddPropertyMtlState("RenderState",this);
		}
		{
			PropertyBase::_PropertyGroup Group(0,"DX9效果",FALSE);
			int index_Group = pPage->AddGroup(Group);
			PropertyBase::_PropertyGroup* pGroup = pPage->FindGroup(index_Group);
			pGroup->AddPropertyMtlEffect("Effect",this);
		}
	}

	PageIndex = this->AddPage("动作");
	pPage = this->FindPage(PageIndex);
	if(pPage)
	{
		{
			PropertyBase::_PropertyGroup Group(0,"动作列表",FALSE);
			int index_Group = pPage->AddGroup(Group);
			PropertyBase::_PropertyGroup* pGroup = pPage->FindGroup(index_Group);

			pGroup->AddPropertyAni("MtlList",this);
		}
	}

	return S_OK;
}

HRESULT KModel::LoadFromX(LPSTR pFileName)
{
	HRESULT hr;
	m_dOption &= !MESH_OPTIMIZED;
	LPD3DXBUFFER   pD3DXMtrlBuffer;
	LPD3DXBUFFER   m_pAdjacencyBuffer;   // Contains the adjaceny info loaded with the mesh
	//DWORD          *rgdwAdjacencyTemp = NULL;
	// Load the mesh from the specified file
	if ( FAILED( hr = D3DXLoadMeshFromX( pFileName , D3DXMESH_MANAGED, 
		g_pd3dDevice,
		&m_pAdjacencyBuffer, 
		&pD3DXMtrlBuffer,NULL,
		&m_dNumMaterial, 
		&m_pMeshSys ) ) )
	{
		return hr ;
	}

	m_lpMaterial = new MATERIAL[m_dNumMaterial];
	ZeroMemory(m_lpMaterial,sizeof(MATERIAL)*m_dNumMaterial);

	D3DXMATERIAL* d3dxMaterials = (D3DXMATERIAL*)pD3DXMtrlBuffer->GetBufferPointer();
	for( DWORD i=0; i<m_dNumMaterial; i++ )
	{
		// Copy the material
		LPMATERIAL pMaterial = &m_lpMaterial[i]; 
		pMaterial->m_sMaterial9 = d3dxMaterials[i].MatD3D;
		pMaterial->m_sMaterial9.Ambient = pMaterial->m_sMaterial9.Diffuse;
		//force ambient = diffuse , better in view
		DWORD TextureID = 0;
		if (d3dxMaterials[i].pTextureFilename!=NULL)
		{
			if (SUCCEEDED(g_cTextureTable.LoadFromFile(&TextureID,d3dxMaterials[i].pTextureFilename)))
			{
				pMaterial->m_dTextureIDs[0] = TextureID;
			}
		}
	}
	pD3DXMtrlBuffer->Release();
	PRT_Initialize();
	LoadShader();
	SAFE_RELEASE(m_pAdjacencyBuffer);
	return S_OK;
}

HRESULT KModel::Render()
{
	g_cGraphicsTool.CreatWind(g_cGraphicsTool.m_WindForce);
	g_cGraphicsTool.m_PreWind += 0.01f * ( g_cGraphicsTool.m_WindForce - g_cGraphicsTool.m_PreWind );
	if( m_bPlaySequence )
	{
		m_pCurSeq->UpdateSequence();
	}
	else
	{
		UpdateAnimation();
	}
	
	SetMatrix();

	if ((m_dOption&MESH_SKINED)&&(m_lpBoneInfo!=NULL))
	{
		//SoftSkinning(); 
		//DrawMesh(m_pBlendMesh) ;
		VertexProcessSkin(); 
		g_pd3dDevice->SetTransform( D3DTS_WORLD , &m_Matrix);

		//DrawBones();
		//DrawBBox();
		//DrawSocket();
		
	}
	else if ( m_bSHPRTReadyToRender )
	{
#if D3D_SDK_VERSION == 32
		if ( !m_pPRTEffect )
			PRT_LoadEffects();
		g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
		PRT_UpdateLightingEnvironment();
		PRT_RenderWithPRT(TRUE);
#endif
#if D3D_SDK_VERSION == 31
		PRT_RenderMeshWithSHPRT();
#endif
	}
	else if (m_dOption&MESH_OPTIMIZED)
	{
		DrawMesh(m_pMeshOpt) ;
	}
	else
	{
		DrawMesh(m_pMeshSys) ;
	}
	if( m_bDrawNormal )
		DrawNormal();
	if ( m_bDrawBoneTrace )
		DrawBoneTrace(m_strTracedBoneName.c_str());
	RestoreMatrix();

	
	return S_OK;
}


HRESULT KModel::RenderWithEffect(DWORD Option,PVOID pData,D3DXMATRIX* matParent)
{
	SetMatrix();
	HRESULT hr = S_OK;

	MTLMSEFFECT* pHEffect = (MTLMSEFFECT*) pData;
	LPD3DXEFFECT pEffect = pHEffect->m_HEffect->pEffect;

	hr = pEffect->SetMatrix("mWorld",&m_CurMatrix);
	g_pd3dDevice->SetTransform( D3DTS_WORLD , &m_CurMatrix );

	{
		g_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
		g_pd3dDevice->SetRenderState(D3DRS_ALPHAREF, 20);
		g_pd3dDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
		g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA); 


	}

	if (m_dOption&MESH_OPTIMIZED)
	{
		for(DWORD i=0;i<m_dNumMaterial;i++)
		{
			LPMATERIAL pMtl = &m_lpMaterial[i];
			SetMaterial(i);
			LPTEXTURE pTexture = NULL;
			if(SUCCEEDED(g_cTextureTable.GetTexture(&pTexture,pMtl->m_dTextureIDs[0])))
			{
				hr = pEffect->SetTexture("Tex0",pTexture->m_lpTexture);
			}

			pEffect->CommitChanges();
			m_pMeshOpt->DrawSubset(i);
		}
	}

	else
	{

		for(DWORD i=0;i<m_dNumMaterial;i++)
		{
			LPMATERIAL pMtl = &m_lpMaterial[i];
			SetMaterial(i);
			LPTEXTURE pTexture = NULL;
			if(SUCCEEDED(g_cTextureTable.GetTexture(&pTexture,pMtl->m_dTextureIDs[0])))
			{
				pEffect->SetTexture("Tex0",pTexture->m_lpTexture);
			}

			pEffect->CommitChanges();
			m_pMeshSys->DrawSubset(i);
		}

	}

	RestoreMatrix();

	return S_OK;
}

void KModel::Initialize(PVOID pXMeshTable)
{
	m_pMeshSys = NULL;
	m_pSkinInfo = NULL;
	m_lpBoneInfo = NULL;

	m_lpSockets = NULL;

	m_dNumAnimation = 0;
	m_dNumBone = 0;
	m_dNumSocket = 0;

	D3DXMatrixIdentity(&m_Matrix);
	m_pBindSocketName = NULL;
	m_pBindMesh = NULL;

	m_nCurAnimationIndex = -1;
	m_dCurAnimationMethod = 0;
	m_fCurAnimationSpeed = 1.0f;
	m_fCurAnimationStart = timeGetTime()*1.0f;

}

HRESULT KModel::LoadBlendMesh(void* pBuffer, LPD3DXMESH* ppMesh)
{
	HRESULT hr = E_FAIL;
	return hr;
}

HRESULT KModel::LoadBlendMesh(FILE* pFile, LPD3DXMESH* ppMesh)
{
	HRESULT hr = E_FAIL;
	BlendMeshHeader header;
	BYTE* pVertexBuffer = NULL;
	BYTE* pIndexBuffer = NULL;
	BYTE* pBuffer = NULL;
	DWORD* pAttBuffer = NULL;
	assert(pFile);
	SAFE_RELEASE(*ppMesh);
	if (fread(&header, sizeof(BlendMeshHeader), 1, pFile) != 1 )
		return hr;

	pBuffer = new BYTE[header.BlockLength];
	if( !pBuffer )
		return hr;
	if( fread(pBuffer, sizeof(BYTE), header.BlockLength, pFile) != header.BlockLength )
		goto ERR_EXIT;

	//m_dNumVertices = header.NumVertices;
	//m_dNumFaces = header.NumFaces;
	//m_bSkined = TRUE;
	m_NumAttributeGroups = header.NumAtt;
	hr = D3DXCreateMesh(header.NumFaces, header.NumVertices, header.Options, header.decl, g_pd3dDevice, &(*ppMesh));
	
	if( SUCCEEDED(hr) )
	{
		LPDIRECT3DVERTEXBUFFER9 lpVB = 0;
		LPDIRECT3DINDEXBUFFER9 lpIB = 0;
		D3DINDEXBUFFER_DESC ibDesc;
		D3DVERTEXBUFFER_DESC vbDesc;
		(*ppMesh)->GetIndexBuffer(&lpIB);
		(*ppMesh)->GetVertexBuffer(&lpVB);
		lpIB->GetDesc(&ibDesc);
		lpVB->GetDesc(&vbDesc);

		(*ppMesh)->LockIndexBuffer(0, (LPVOID*)&pIndexBuffer);
		(*ppMesh)->LockVertexBuffer(0, (LPVOID*)&pVertexBuffer);
		(*ppMesh)->LockAttributeBuffer(0, &pAttBuffer);

		//fread(pVertexBuffer, sizeof(BYTE), header.VertexBufferSize, pFile);
		//fread(pIndexBuffer, sizeof(BYTE), header.IndexBufferSize, pFile);

		//fread(pAttRange, sizeof(D3DXATTRIBUTERANGE), header.NumAtt, pFile);
		memcpy(pVertexBuffer, pBuffer, sizeof(BYTE) * vbDesc.Size);
		memcpy(pIndexBuffer, pBuffer + header.VertexBufferSize, sizeof(BYTE) * ibDesc.Size);

		(*ppMesh)->UnlockIndexBuffer();
		(*ppMesh)->UnlockVertexBuffer();
		
		
		SAFE_RELEASE(m_pBoneCombinationTable);
		D3DXCreateBuffer( header.BoneCombinSize, &m_pBoneCombinationTable);
		memcpy(m_pBoneCombinationTable->GetBufferPointer(), pBuffer + sizeof(BYTE) * (header.IndexBufferSize + header.VertexBufferSize) + sizeof(D3DXATTRIBUTERANGE) * header.NumAtt, header.BoneCombinSize);
		memcpy(pAttBuffer, pBuffer + sizeof(BYTE) * (header.IndexBufferSize + header.VertexBufferSize) + sizeof(D3DXATTRIBUTERANGE) * header.NumAtt + header.BoneCombinSize, sizeof(DWORD) * header.NumFaces);
		(*ppMesh)->UnlockAttributeBuffer();
		m_dNumMaterial = header.NumAtt;
		m_MaxFaceInfl = header.MaxFaceInfl;

		(*ppMesh)->SetAttributeTable((D3DXATTRIBUTERANGE*)(pBuffer + sizeof(BYTE) * (header.IndexBufferSize + header.VertexBufferSize)), header.NumAtt);
		//m_NumAttributeGroups = header.BoneCombinSize / sizeof(D3DXBONECOMBINATION);

		//SAFE_DELETE_ARRAY(m_pBondID);
		//m_pBondID = new DWORD[m_MaxFaceInfl * m_NumAttributeGroups];
		//fread(m_pBondID, sizeof(DWORD), m_MaxFaceInfl * m_NumAttributeGroups, pFile);
		LPD3DXBONECOMBINATION pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(m_pBoneCombinationTable->GetBufferPointer());
		for ( DWORD dwAttGroupIndex = 0; dwAttGroupIndex < m_NumAttributeGroups; dwAttGroupIndex ++ )
		{
			pBoneComb[dwAttGroupIndex].BoneId = reinterpret_cast<DWORD*>(&pBoneComb[m_NumAttributeGroups]) + dwAttGroupIndex * 4;
			for( int i = m_MaxFaceInfl; i < 4; i++ )
			{
				pBoneComb[dwAttGroupIndex].BoneId[i] = 0xffffffff;
			}
		}
	}
	SAFE_DELETE_ARRAY(pBuffer);
	return hr;

ERR_EXIT:
	SAFE_DELETE_ARRAY(pBuffer);
	return E_FAIL;
}

HRESULT KModel::SaveBlendedMesh(ID3DXMesh* pMesh, FILE* pFile)
{
	HRESULT hr = S_OK;
	assert(pMesh && pFile);
	BlendMeshHeader header;
	D3DXATTRIBUTERANGE* pAttRange = NULL;
	BYTE* pVertexBuffer = NULL;
	BYTE* pIndexBuffer = NULL;
	IDirect3DIndexBuffer9* lpIndexBuffer;
	IDirect3DVertexBuffer9* lpVertexBuffer;
	D3DINDEXBUFFER_DESC IndexBufferDesc;
	D3DVERTEXBUFFER_DESC VertexBufferDesc;
	DWORD* pAttBuffer = NULL;

	header.NumFaces = pMesh->GetNumFaces();
	header.NumVertices = pMesh->GetNumVertices();
	header.Options = pMesh->GetOptions();
	header.MaxFaceInfl = m_MaxFaceInfl;//This is the size of Bone ID array
	

	//pAttBuffer = new DWORD[header.NumFaces];
	pMesh->GetAttributeTable(0, &header.NumAtt);
	pAttRange = new D3DXATTRIBUTERANGE[header.NumAtt];
	pMesh->GetAttributeTable(pAttRange, &header.NumAtt);
	pMesh->GetDeclaration(header.decl);
	
	pMesh->GetIndexBuffer(&lpIndexBuffer);
	lpIndexBuffer->GetDesc(&IndexBufferDesc);
	header.IndexBufferSize = IndexBufferDesc.Size;
	pMesh->LockIndexBuffer(D3DLOCK_READONLY, (LPVOID*)&pIndexBuffer);

	pMesh->GetVertexBuffer(&lpVertexBuffer);
	lpVertexBuffer->GetDesc(&VertexBufferDesc);
	header.VertexBufferSize = VertexBufferDesc.Size;
	pMesh->LockVertexBuffer(D3DLOCK_READONLY, (LPVOID*)&pVertexBuffer);

	
	pMesh->LockAttributeBuffer(D3DLOCK_READONLY, &pAttBuffer);

	header.BoneCombinSize = m_pBoneCombinationTable->GetBufferSize();

	header.BlockLength = sizeof(D3DXATTRIBUTERANGE) * header.NumAtt +
		VertexBufferDesc.Size + IndexBufferDesc.Size + header.BoneCombinSize + sizeof(DWORD) * header.NumFaces;
	
	fwrite(&header, sizeof(BlendMeshHeader), 1, pFile);
	fwrite(pVertexBuffer, sizeof(BYTE), VertexBufferDesc.Size, pFile);
	fwrite(pIndexBuffer, sizeof(BYTE), IndexBufferDesc.Size, pFile);
	fwrite(pAttRange, sizeof(D3DXATTRIBUTERANGE), header.NumAtt, pFile);
	fwrite(m_pBoneCombinationTable->GetBufferPointer(), sizeof(BYTE), header.BoneCombinSize, pFile);
	fwrite(pAttBuffer, sizeof(DWORD), header.NumFaces, pFile);
	
	LPD3DXBONECOMBINATION lpBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(m_pBoneCombinationTable->GetBufferPointer());
	
	
	//DWORD* pBoneIDBuffer = new DWORD[m_NumAttributeGroups * header.MaxFaceInfl];
	DWORD dwAttGroupIndex;
	DWORD dwBufferOffset = 0;
	for ( dwAttGroupIndex = 0; dwAttGroupIndex < m_NumAttributeGroups; dwAttGroupIndex++ )
	{
		//memcpy(pBoneIDBuffer + dwBufferOffset, lpBoneComb[dwAttGroupIndex].BoneId, sizeof(DWORD) * header.MaxFaceInfl);
		//dwBufferOffset += header.MaxFaceInfl;
		int c = 0;
	}
	//fwrite(pBoneIDBuffer, sizeof(DWORD), m_NumAttributeGroups * header.MaxFaceInfl, pFile);*/
	pMesh->UnlockVertexBuffer();
	pMesh->UnlockIndexBuffer();
	pMesh->UnlockAttributeBuffer();
	
//////////////////////////////////////////////////////////////////////////
//Release local alloc memory here	
	SAFE_DELETE_ARRAY(pAttRange);
	//SAFE_DELETE_ARRAY(pBoneIDBuffer);
	//SAFE_DELETE_ARRAY(pAttBuffer);
	return hr;
}

//HRESULT KModel::LoadMesh(LPSTR pFileName)
//{
//	_MeshFileHead MeshFileHead;
//
//	DWORD BlockEnd  = 0xFFFFFFFF;  
//	FILE* pFile = fopen(pFileName,"rb");
//	if (pFile==NULL) return E_FAIL;
//
//	DWORD Pos_Start = ftell(pFile);
//
//	fread(&MeshFileHead,sizeof(_MeshFileHead),1,pFile);
//	if (MeshFileHead.FileMask!=0x4D455348) return E_FAIL;
//
//	DWORD m_dNumVertices = 0;
//	DWORD m_dNumFaces    = 0;
//	DWORD m_dNumSubsets  = 0;
//	BOOL  m_bSkined      = 0;
//
//	D3DXVECTOR3 * m_lpVerticePosition = NULL;
//	D3DXVECTOR3 * m_lpVerticeNormal   = NULL;
//	D3DCOLOR    * m_lpVerticeDiffuse  = NULL;
//
//	D3DXVECTOR3 * m_lpVerticeTextCoord1 = NULL;
//	D3DXVECTOR3 * m_lpVerticeTextCoord2 = NULL;
//	D3DXVECTOR3 * m_lpVerticeTextCoord3 = NULL;
//
//	DWORD * m_lpFacesIndex = NULL;
//	DWORD * m_lpSubsetID   = NULL;
//
//	_MeshHead* pMeshHead = new _MeshHead[MeshFileHead.MeshCount];
//	for(DWORD i_Mesh=0;i_Mesh<MeshFileHead.MeshCount;i_Mesh++)
//	{
//		fread(&pMeshHead[i_Mesh],sizeof(_MeshHead),1,pFile);
//
//		m_dNumVertices = pMeshHead[i_Mesh].NumVertices;//顶点数目
//		m_dNumFaces    = pMeshHead[i_Mesh].NumFaces;//面（三角形）数目
//		m_dNumSubsets  = pMeshHead[i_Mesh].NumSubset;//子部分数目
//		m_bSkined      = pMeshHead[i_Mesh].SkinInfoBlock;//是否包含骨骼信息
//
//		if (pMeshHead[i_Mesh].PositionBlock)
//		{
//			m_lpVerticePosition = new D3DXVECTOR3[m_dNumVertices];
//			ZeroMemory(m_lpVerticePosition,sizeof(D3DXVECTOR3)*m_dNumVertices);
//
//			fseek(pFile,Pos_Start+pMeshHead[i_Mesh].PositionBlock,SEEK_SET);
//
//			fread( m_lpVerticePosition,sizeof(D3DXVECTOR3),m_dNumVertices,pFile); //顶点位置坐标
//			fread(&BlockEnd,sizeof(DWORD),1,pFile);
//		}
//		if (pMeshHead[i_Mesh].NormalBlock)
//		{
//			m_lpVerticeNormal = new D3DXVECTOR3[m_dNumVertices];
//			ZeroMemory(m_lpVerticeNormal,sizeof(D3DXVECTOR3)*m_dNumVertices);
//
//			fseek(pFile,Pos_Start+pMeshHead[i_Mesh].NormalBlock,SEEK_SET);
//
//			fread( m_lpVerticeNormal,sizeof(D3DXVECTOR3),m_dNumVertices,pFile); //顶点位置坐标
//			fread(&BlockEnd,sizeof(DWORD),1,pFile);
//		}
//
//		if (pMeshHead[i_Mesh].DiffuseBlock)
//		{
//			m_lpVerticeDiffuse = new D3DCOLOR[m_dNumVertices];
//			ZeroMemory(m_lpVerticeDiffuse,sizeof(D3DCOLOR)*m_dNumVertices);
//
//			fseek(pFile,Pos_Start+pMeshHead[i_Mesh].DiffuseBlock,SEEK_SET);
//
//			fread( m_lpVerticeDiffuse,sizeof(D3DCOLOR),m_dNumVertices,pFile); //顶点位置坐标
//			fread(&BlockEnd,sizeof(DWORD),1,pFile);
//		}
//
//		if (pMeshHead[i_Mesh].TextureUVW1Block)
//		{
//			m_lpVerticeTextCoord1 = new D3DXVECTOR3[m_dNumVertices];
//			ZeroMemory(m_lpVerticeTextCoord1,sizeof(D3DXVECTOR3)*m_dNumVertices);
//
//			fseek(pFile,Pos_Start+pMeshHead[i_Mesh].TextureUVW1Block,SEEK_SET);
//
//			fread( m_lpVerticeTextCoord1,sizeof(D3DXVECTOR3),m_dNumVertices,pFile); //顶点位置坐标
//			fread(&BlockEnd,sizeof(DWORD),1,pFile);
//		}
//
//		if (pMeshHead[i_Mesh].TextureUVW2Block)
//		{
//			m_lpVerticeTextCoord2 = new D3DXVECTOR3[m_dNumVertices];
//			ZeroMemory(m_lpVerticeTextCoord2,sizeof(D3DXVECTOR3)*m_dNumVertices);
//
//			fseek(pFile,Pos_Start+pMeshHead[i_Mesh].TextureUVW2Block,SEEK_SET);
//
//			fread( m_lpVerticeTextCoord2,sizeof(D3DXVECTOR3),m_dNumVertices,pFile); //顶点位置坐标
//			fread(&BlockEnd,sizeof(DWORD),1,pFile);
//		}
//
//		if (pMeshHead[i_Mesh].TextureUVW3Block)
//		{
//			m_lpVerticeTextCoord3 = new D3DXVECTOR3[m_dNumVertices];
//			ZeroMemory(m_lpVerticeTextCoord3,sizeof(D3DXVECTOR3)*m_dNumVertices);
//
//			fseek(pFile,Pos_Start+pMeshHead[i_Mesh].TextureUVW3Block,SEEK_SET);
//
//			fread( m_lpVerticeTextCoord3,sizeof(D3DXVECTOR3),m_dNumVertices,pFile); //顶点位置坐标
//			fread(&BlockEnd,sizeof(DWORD),1,pFile);
//		}
//
//		if (pMeshHead[i_Mesh].FacesIndexBlock)
//		{
//			m_lpFacesIndex = new DWORD[m_dNumFaces*3];
//			ZeroMemory(m_lpFacesIndex,sizeof(DWORD)*m_dNumFaces*3);
//
//			fseek(pFile,Pos_Start+pMeshHead[i_Mesh].FacesIndexBlock,SEEK_SET);
//
//			fread( m_lpFacesIndex,sizeof(DWORD),m_dNumFaces*3,pFile);//面索引
//			fread(&BlockEnd,sizeof(DWORD),1,pFile);
//		}
//
//		if (pMeshHead[i_Mesh].SubsetIndexBlock)
//		{
//			m_lpSubsetID = new DWORD[m_dNumFaces];
//			ZeroMemory(m_lpSubsetID,sizeof(DWORD)*m_dNumFaces);
//
//			fseek(pFile,Pos_Start+pMeshHead[i_Mesh].SubsetIndexBlock,SEEK_SET);
//
//			fread( m_lpSubsetID  ,sizeof(DWORD),m_dNumFaces,pFile);  //子部分索引
//			fread(&BlockEnd,sizeof(DWORD),1,pFile);
//
//		}
//		if (m_bSkined)
//		{
//			DWORD NumBone = 0;
//			fread(&NumBone,sizeof(DWORD),1,pFile);//骨骼数目
//			LPBONEINFO lpSrcBoneInfo = new BONEINFO[NumBone];
//
//			for(DWORD i_Bone=0;i_Bone<NumBone;i_Bone++)
//			{
//				LPBONEINFO pInfo = &lpSrcBoneInfo[i_Bone];
//
//				TCHAR Info_Name[256];
//				TCHAR Info_ParentName[256];
//
//				fread(Info_Name,sizeof(char),30,pFile);
//				fread(Info_ParentName,sizeof(char),30,pFile);
//
//				pInfo->scName = Info_Name;
//				pInfo->scParentName = Info_ParentName;
//
//				if (strcmpi(pInfo->scParentName.c_str(),"Scene Root")==0)
//					m_nBaseBoneID.push_back(i_Bone);
//
//				DWORD NumChild = 0;
//				fread(&NumChild,sizeof(DWORD),1,pFile);	
//
//				for(DWORD i_Child=0;i_Child<NumChild;i_Child++)
//				{
//					TCHAR str[256];
//					fread(str,sizeof(TCHAR),30,pFile);//
//					string ChileName = str;
//					pInfo->ChildNameVector.push_back(ChileName);
//					int S = 0;
//					pInfo->ChildIDVector.push_back(S);
//				}
//
//				fread(&pInfo->MatrixOffset,sizeof(D3DXMATRIX),1,pFile);//
//				fread(&pInfo->MatrixOffsetBase,sizeof(D3DXMATRIX),1,pFile);//
//
//				fread(&pInfo->NumVerticesRef,sizeof(DWORD),1,pFile);//
//				pInfo->VerticesIndexRef = new DWORD[pInfo->NumVerticesRef];
//				ZeroMemory(pInfo->VerticesIndexRef,sizeof(DWORD)*pInfo->NumVerticesRef);
//				pInfo->VerticesWeightRef = new float[pInfo->NumVerticesRef];
//				ZeroMemory(pInfo->VerticesWeightRef,sizeof(float)*pInfo->NumVerticesRef);
//
//				fread(pInfo->VerticesIndexRef,sizeof(DWORD),
//					pInfo->NumVerticesRef,pFile);//
//				fread(pInfo->VerticesWeightRef,sizeof(float),
//					pInfo->NumVerticesRef,pFile);//
//
//				D3DXMatrixInverse(&pInfo->CurMatrix,NULL,&pInfo->MatrixOffset);
//				pInfo->RelativeMatrix = pInfo->MatrixOffsetBase;
//				D3DXMatrixInverse(&pInfo->MatrixOffsetBase,NULL,&pInfo->MatrixOffset);
//
//
//				//D3DXVECTOR3 Scaling,Position;
//
//				//MatrixExract(Scaling,Position,pInfo->Rotation,&pInfo->CurMatrix);
//
//				//pInfo->BonePosition = Position;
//				//pInfo->BaseBonePosition = pInfo->BonePosition;
//				//pInfo->LocalWorldCurMatrix = pInfo->CurMatrix;
//				//			pInfo->ObjectBonePosition = pInfo->BonePosition;
//
//				pInfo->BoneScale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);//Scaling;
//				//pInfo->BaseBoneScale = pInfo->BoneScale;
//				//pInfo->BaseRotation = pInfo->Rotation;
//			}
//
//			m_lpBoneInfo = lpSrcBoneInfo;
//			m_dOption |= MESH_SKINED;
//			m_dNumBone = NumBone;
//
//			DWORD NumSocket = 0;
//			fread(&NumSocket,sizeof(DWORD),1,pFile);
//			if (NumSocket>0)
//			{
//				m_lpSockets = new SOCKET[NumSocket];
//
//				for(DWORD i_Socket=0;i_Socket<NumSocket;i_Socket++)
//				{
//					LPSOCKET pSocket = &m_lpSockets[i_Socket];
//					//pSocket->scName = new TCHAR[30];
//					//pSocket->scParentBoneName = new TCHAR[30];
//
//					TCHAR Socket_Name[256];
//					TCHAR Socket_ParentBoneName[256];
//					fread(Socket_Name,sizeof(TCHAR),30,pFile);//
//					fread(Socket_ParentBoneName,sizeof(TCHAR),30,pFile);//
//					fread(&pSocket->MatrixOffset,sizeof(D3DMATRIX),1,pFile);//
//					pSocket->scName = Socket_Name;
//					pSocket->scParentBoneName = Socket_ParentBoneName;
//
//					/*float temp[4];
//					temp[0] = pSocket->MatrixOffset._31;
//					temp[1] = pSocket->MatrixOffset._32;
//					temp[2] = pSocket->MatrixOffset._33;
//					temp[3] = pSocket->MatrixOffset._34;
//
//					pSocket->MatrixOffset._31 = pSocket->MatrixOffset._21;
//					pSocket->MatrixOffset._32 = pSocket->MatrixOffset._22;
//					pSocket->MatrixOffset._33 = pSocket->MatrixOffset._23;
//					pSocket->MatrixOffset._34 = pSocket->MatrixOffset._24;
//
//
//					pSocket->MatrixOffset._21 = temp[0];
//					pSocket->MatrixOffset._22 = temp[1];
//					pSocket->MatrixOffset._23 = temp[2];
//					pSocket->MatrixOffset._24 = temp[3];*/
//
//
//					pSocket->MatrixOffsetBack = pSocket->MatrixOffset;//对偏移矩阵做个备份,便于修改
//
//
//					int Index = FindBone((LPSTR)(pSocket->scParentBoneName.c_str())); 
//					//5-9-2005 start
//					/*if (Index>=0)
//					{
//					D3DXMATRIX mat;
//					LPBONEINFO pInfoParent = &m_lpBoneInfo[Index];
//					D3DXMatrixMultiply(&pSocket->CurMatrix,
//					&pInfoParent->CurMatrix,
//					&pSocket->MatrixOffset);
//					D3DXVECTOR3 A,B,C;
//					A = D3DXVECTOR3(pInfoParent->CurMatrix._11,pInfoParent->CurMatrix._12,pInfoParent->CurMatrix._13);
//					B = D3DXVECTOR3(pInfoParent->CurMatrix._21,pInfoParent->CurMatrix._22,pInfoParent->CurMatrix._23);
//					C = D3DXVECTOR3(pInfoParent->CurMatrix._31,pInfoParent->CurMatrix._32,pInfoParent->CurMatrix._33);
//					float Sx,Sy,Sz;
//					Sx = D3DXVec3Length(&A);
//					Sy = D3DXVec3Length(&B);
//					Sz = D3DXVec3Length(&C);
//					D3DXMATRIX matScale;
//					D3DXMatrixScaling(&matScale,Sx,Sy,Sz);
//					D3DXMatrixInverse(&matScale,NULL,&matScale);
//
//					D3DXMatrixMultiply(&mat,
//					&matScale,
//					&pInfoParent->CurMatrix);
//
//					D3DXMatrixMultiply(&pSocket->CurMatrix,
//					&pSocket->MatrixOffset,&mat);
//					}*/
//					//5-9-2005 end
//					if (Index>=0)
//					{
//						D3DXMATRIX mat;
//						LPBONEINFO pInfoParent = &m_lpBoneInfo[Index];
//						//pSocket->CurMatrix = pSocket->MatrixOffset;
//						D3DXMatrixMultiply(&pSocket->CurMatrix,
//							&pSocket->MatrixOffset, &pInfoParent->CurMatrix);
//					}
//				}
//			}
//			m_dNumSocket = NumSocket;
//
//			//////////////////////////////////////////////////////////////////////////
//			//先整理骨骼基本信息
//			for(i_Bone=0;i_Bone<NumBone;i_Bone++)
//			{
//				LPBONEINFO pInfo = &lpSrcBoneInfo[i_Bone];
//
//				char flex[] = "FL_";
//				char*  result;
//				result = strstr( pInfo->scName.c_str(), flex);
//				if(result)
//					pInfo->bFlexibleBone = TRUE;
//
//				pInfo->nParentIndex = FindBone((LPSTR)pInfo->scParentName.c_str());
//				if(pInfo->nParentIndex!=-1)
//				{
//					LPBONEINFO pInfoParent =  &lpSrcBoneInfo[pInfo->nParentIndex];
//					D3DXMATRIX Mat;
//					D3DXMatrixInverse(&Mat,NULL,&pInfoParent->CurMatrix);
//					pInfo->RelativeMatrix = pInfo->CurMatrix * Mat;
//
//				}
//				for(DWORD i_Child=0;i_Child<pInfo->ChildNameVector.size();i_Child++)
//				{
//					pInfo->ChildIDVector[i_Child] = 
//						FindBone((LPSTR)pInfo->ChildNameVector[i_Child].c_str());
//				}
//			}
//
//			//////////////////////////////////////////////////////////////////////////
//			//创建柔体骨骼的固定点,固定点是骨骼链的起点或者骨骼链上第一个柔体骨骼
//			for(i_Bone=0;i_Bone<NumBone;i_Bone++)	
//			{
//				LPBONEINFO pInfo = &lpSrcBoneInfo[i_Bone];
//				if(pInfo->bFlexibleBone)
//				{
//					/*			if ((pInfo->nParentIndex>=0)&&(pInfo->nParentIndex<(int)NumBone))
//					{
//					LPBONEINFO pParentInfo = &lpSrcBoneInfo[pInfo->nParentIndex];
//					D3DXVECTOR3 BoneVec = pInfo->BonePosition - pParentInfo->BonePosition;
//					//pInfo->BaseBonePosition = BoneVec;
//					pInfo->BoneLength = D3DXVec3Length(&BoneVec);
//					if(!pParentInfo->bFlexibleBone)
//					{
//					_FlexibleBone FBone;
//					FBone.Index = i_Bone;
//					FBone.PrePosition = pInfo->BonePosition;
//					m_vecStaticFlexibleBoneIndex.push_back(FBone);
//
//					}
//					}
//					if(pInfo->nParentIndex==-1)
//					{
//					pInfo->BoneLength = 0;
//					_FlexibleBone FBone;
//					FBone.Index = i_Bone;
//					FBone.PrePosition = pInfo->BonePosition;
//
//
//					m_vecStaticFlexibleBoneIndex.push_back(FBone);					
//					}
//					}
//					}*/
//					if(pInfo->nParentIndex==-1)
//					{
//						pInfo->BoneLength = 0;
//						_FlexibleBone FBone;
//						FBone.Index = i_Bone;
//						FBone.PrePosition = pInfo->BonePosition;
//						m_vecStaticFlexibleBoneIndex.push_back(FBone);					
//					}
//					else if ((pInfo->nParentIndex>=0)&&(pInfo->nParentIndex<(int)NumBone))
//					{
//						LPBONEINFO pParentInfo = &lpSrcBoneInfo[pInfo->nParentIndex];
//						D3DXVECTOR3 BoneVec = pInfo->BonePosition - pParentInfo->BonePosition;
//
//						pInfo->BonePositionOffset = BoneVec;
//						//pInfo->BaseBonePosition = BoneVec;
//						pInfo->BoneLength = D3DXVec3Length(&BoneVec);
//
//						if(!pParentInfo->bFlexibleBone)
//						{
//							_FlexibleBone FBone;
//							FBone.Index = i_Bone;
//							FBone.PrePosition = pInfo->BonePosition;
//							m_vecStaticFlexibleBoneIndex.push_back(FBone);
//						}
//					}
//					else
//					{
//						pInfo->nParentIndex =-1;
//					}
//				}
//			} 
//
//		}
//	}
//
//	if ( !MeshFileHead.ExtendBlock[BLEND_MESH_SAVED] )
//	{
//		fread(&BlockEnd,sizeof(DWORD),1,pFile);
//		fclose(pFile);
//		wsprintf(m_szMeshFileName,"%s",pFileName);
//	}
//
//
//
//	//////////////////////////////柔体/////////////////////////////////////////////
//	for(int i_Flex=0;i_Flex<(int)m_vecStaticFlexibleBoneIndex.size();i_Flex++)
//	{
//		int nFlexBoneIndex = m_vecStaticFlexibleBoneIndex[i_Flex].Index;
//		LPBONEINFO pInfo = &m_lpBoneInfo[nFlexBoneIndex];
//
//		/*LPBONEINFO pIn = pInfo;
//		vector<D3DXVECTOR3>Position1;
//		vector<float>Len;
//		int n=0;
//		D3DXVECTOR3 Position;*/
//
//		//while ( (int)pIn->ChildIDVector.size() > 0 )
//		//{
//
//		//	Position.x = pIn->CurMatrix._41;
//		//	Position.y = pIn->CurMatrix._42;
//		//	Position.z = pIn->CurMatrix._43;
//		//	Position1.push_back( Position );
//
//		//	int BoneIndex = pIn->ChildIDVector[0];
//		//	if(BoneIndex==-1)
//		//	{
//		//		break;
//		//	}
//		//	pIn = &m_lpBoneInfo[BoneIndex];	
//		//	if( n>0 )
//		//	{
//		//		Position = Position1[n] - Position1[n-1];
//		//		float L = D3DXVec3Length( &Position );
//		//		Len.push_back(L);
//		//	}
//		//	n++;
//		//}
//		//Position.x = pIn->CurMatrix._41;
//		//Position.y = pIn->CurMatrix._42;
//		//Position.z = pIn->CurMatrix._43;
//		//Position1.push_back( Position );
//		//if( n>0 )
//		//{
//		//	Position = Position1[n] - Position1[n-1];
//		//	float L = D3DXVec3Length( &Position );
//		//	Len.push_back( L);
//		//}
//		//n++;
//
//
//
//		//m_vecStaticFlexibleBoneIndex[i_Flex].ropeSimulation = new Water::RopeSimulation(
//		//	n,						//1. mass的数量
//		//	Position1,
//		//	0.05f,					//2. 每个mass的重量
//		//	10.0f,					//3. 弹簧的张量
//		//	Len,					//4. 弹簧不变形时的长度
//		//	0.2f,					//5. 弹簧的摩擦力
//		//	D3DXVECTOR3(0, -9.81f, 0), //6. 重力
//		//	0.02f,					//7. 空气的摩擦力
//		//	100.0f,					//8. ground repulsion constant
//		//	0.2f,					//9. 地表摩擦力
//		//	2.0f,					//10. ground absorption constant
//		//	0.0f);				//11. 地面的高度值
//
//	}
//	//////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//	///////////////////////////////////////////////////////////////////////////////////////
//	//创建 XMesh 
//	HRESULT hr = S_OK;
//	
//	if ( !MeshFileHead.ExtendBlock[BLEND_MESH_SAVED] )
//	{
//		DWORD FVF = D3DFVF_XYZ;
//		if (pMeshHead[0].NormalBlock)
//			FVF |= D3DFVF_NORMAL;
//		if (pMeshHead[0].DiffuseBlock)
//			FVF |= D3DFVF_DIFFUSE;
//
//		if (pMeshHead[0].TextureUVW3Block)
//			FVF |= D3DFVF_TEX3;
//		else if (pMeshHead[0].TextureUVW2Block)
//			FVF |= D3DFVF_TEX2;
//		else if (pMeshHead[0].TextureUVW1Block)
//			FVF |= D3DFVF_TEX1;
//
//		if(m_dNumFaces<=65535)
//		{
//			if (FAILED(hr = D3DXCreateMeshFVF(m_dNumFaces,m_dNumVertices,D3DXMESH_MANAGED,
//				FVF,g_pd3dDevice,&m_pMeshSys)))
//			{
//				return hr;
//			}
//		}
//		else
//		{
//			if (FAILED(hr = D3DXCreateMeshFVF(m_dNumFaces,m_dNumVertices,D3DXMESH_MANAGED|D3DXMESH_32BIT,
//				FVF,g_pd3dDevice,&m_pMeshSys)))
//			{
//				return hr;
//			}
//		}
//
//
//		switch(FVF)
//		{
//		case (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1):
//			{
//				VFormat::FACES_NORMAL_TEXTURE1 * pVers = NULL;
//				WORD* pIndex = NULL;
//				DWORD * pAttrib = NULL;
//				if (FAILED(m_pMeshSys->LockVertexBuffer(0,(void**)&pVers)))
//					return E_FAIL;
//				if (FAILED(m_pMeshSys->LockIndexBuffer (0,(void**)&pIndex)))
//					return E_FAIL;
//				if (FAILED(m_pMeshSys->LockAttributeBuffer(0,(DWORD**)&pAttrib)))
//					return E_FAIL;
//				for(DWORD i=0;i<m_dNumVertices;i++)
//				{
//					pVers[i].p = m_lpVerticePosition[i];
//					pVers[i].Normal = m_lpVerticeNormal[i];
//					pVers[i].tu1 = m_lpVerticeTextCoord1[i].x;
//					pVers[i].tv1 = m_lpVerticeTextCoord1[i].y;
//				}
//				if(m_dNumFaces<=65535)
//				{
//					for( i=0;i<m_dNumFaces;i++)
//					{
//						pIndex[i*3  ] = (WORD)m_lpFacesIndex[i*3  ];
//						pIndex[i*3+1] = (WORD)m_lpFacesIndex[i*3+1];
//						pIndex[i*3+2] = (WORD)m_lpFacesIndex[i*3+2];
//						pAttrib[i] = m_lpSubsetID[i];
//					}
//				}
//				else
//				{
//					for( i=0;i<m_dNumFaces;i++)
//					{
//						pIndex[i*3  ] = (WORD)m_lpFacesIndex[i*3  ];
//						pIndex[i*3+1] = (WORD)m_lpFacesIndex[i*3+1];
//						pIndex[i*3+2] = (WORD)m_lpFacesIndex[i*3+2];
//						pAttrib[i] = m_lpSubsetID[i];
//					}
//				}
//
//				D3DXComputeBoundingBox((D3DXVECTOR3*) pVers,m_dNumVertices,sizeof(VFormat::FACES_NORMAL_TEXTURE1),
//					&m_BBox_A,&m_BBox_B);
//				break;
//			}
//		case (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX2):
//			{
//				VFormat::FACES_NORMAL_TEXTURE2 * pVers = NULL;
//				WORD* pIndex = NULL;
//				DWORD * pAttrib = NULL;
//				if (FAILED(m_pMeshSys->LockVertexBuffer(0,(void**)&pVers)))
//					return E_FAIL;
//				if (FAILED(m_pMeshSys->LockIndexBuffer (0,(void**)&pIndex)))
//					return E_FAIL;
//				if (FAILED(m_pMeshSys->LockAttributeBuffer(0,(DWORD**)&pAttrib)))
//					return E_FAIL;
//				for(DWORD i=0;i<m_dNumVertices;i++)
//				{
//					pVers[i].p = m_lpVerticePosition[i];
//					pVers[i].Normal = m_lpVerticeNormal[i];
//					pVers[i].tu1 = m_lpVerticeTextCoord1[i].x;
//					pVers[i].tv1 = m_lpVerticeTextCoord1[i].y;
//					pVers[i].tu2 = m_lpVerticeTextCoord2[i].x;
//					pVers[i].tv2 = m_lpVerticeTextCoord2[i].y;
//				}
//				if(m_dNumFaces<=65535)
//				{
//					for( i=0;i<m_dNumFaces;i++)
//					{
//						pIndex[i*3  ] = (WORD)m_lpFacesIndex[i*3  ];
//						pIndex[i*3+1] = (WORD)m_lpFacesIndex[i*3+1];
//						pIndex[i*3+2] = (WORD)m_lpFacesIndex[i*3+2];
//						pAttrib[i] = m_lpSubsetID[i];
//					}
//				}
//				else
//				{
//					for( i=0;i<m_dNumFaces;i++)
//					{
//						pIndex[i*3  ] = (WORD)m_lpFacesIndex[i*3  ];
//						pIndex[i*3+1] = (WORD)m_lpFacesIndex[i*3+1];
//						pIndex[i*3+2] = (WORD)m_lpFacesIndex[i*3+2];
//						pAttrib[i] = m_lpSubsetID[i];
//					}
//				}
//				D3DXComputeBoundingBox((D3DXVECTOR3*) pVers,m_dNumVertices,sizeof(VFormat::FACES_NORMAL_TEXTURE2),
//					&m_BBox_A,&m_BBox_B);
//				break;
//			}
//		case (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX3):
//			{
//				VFormat::FACES_NORMAL_TEXTURE3 * pVers = NULL;
//				WORD* pIndex = NULL;
//				DWORD * pAttrib = NULL;
//				if (FAILED(m_pMeshSys->LockVertexBuffer(0,(void**)&pVers)))
//					return E_FAIL;
//				if (FAILED(m_pMeshSys->LockIndexBuffer (0,(void**)&pIndex)))
//					return E_FAIL;
//				if (FAILED(m_pMeshSys->LockAttributeBuffer(0,(DWORD**)&pAttrib)))
//					return E_FAIL;
//				for(DWORD i=0;i<m_dNumVertices;i++)
//				{
//					pVers[i].p = m_lpVerticePosition[i];
//					pVers[i].Normal = m_lpVerticeNormal[i];
//					pVers[i].tu1 = m_lpVerticeTextCoord1[i].x;
//					pVers[i].tv1 = m_lpVerticeTextCoord1[i].y;
//					pVers[i].tu2 = m_lpVerticeTextCoord2[i].x;
//					pVers[i].tv2 = m_lpVerticeTextCoord2[i].y;
//					pVers[i].tu3 = m_lpVerticeTextCoord3[i].x;
//					pVers[i].tv3 = m_lpVerticeTextCoord3[i].y;
//				}
//				if(m_dNumFaces<=65535)
//				{
//					for( i=0;i<m_dNumFaces;i++)
//					{
//						pIndex[i*3  ] = (WORD)m_lpFacesIndex[i*3  ];
//						pIndex[i*3+1] = (WORD)m_lpFacesIndex[i*3+1];
//						pIndex[i*3+2] = (WORD)m_lpFacesIndex[i*3+2];
//						pAttrib[i] = m_lpSubsetID[i];
//					}
//				}
//				else
//				{
//					for( i=0;i<m_dNumFaces;i++)
//					{
//						pIndex[i*3  ] = (WORD)m_lpFacesIndex[i*3  ];
//						pIndex[i*3+1] = (WORD)m_lpFacesIndex[i*3+1];
//						pIndex[i*3+2] = (WORD)m_lpFacesIndex[i*3+2];
//						pAttrib[i] = m_lpSubsetID[i];
//					}
//				}
//				D3DXComputeBoundingBox((D3DXVECTOR3*) pVers,m_dNumVertices,sizeof(VFormat::FACES_NORMAL_TEXTURE3),
//					&m_BBox_A,&m_BBox_B);
//				break;
//			}
//
//		case (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1):
//			{
//				VFormat::FACES_DIFFUSE_TEXTURE1 * pVers = NULL;
//				WORD* pIndex = NULL;
//				DWORD * pAttrib = NULL;
//				if (FAILED(m_pMeshSys->LockVertexBuffer(0,(void**)&pVers)))
//					return E_FAIL;
//				if (FAILED(m_pMeshSys->LockIndexBuffer (0,(void**)&pIndex)))
//					return E_FAIL;
//				if (FAILED(m_pMeshSys->LockAttributeBuffer(0,(DWORD**)&pAttrib)))
//					return E_FAIL;
//				for(DWORD i=0;i<m_dNumVertices;i++)
//				{
//					pVers[i].p = m_lpVerticePosition[i];
//					pVers[i].diffuse = m_lpVerticeDiffuse[i];
//					pVers[i].tu1 = m_lpVerticeTextCoord1[i].x;
//					pVers[i].tv1 = m_lpVerticeTextCoord1[i].y;
//				}
//				if(m_dNumFaces<=65535)
//				{
//					for( i=0;i<m_dNumFaces;i++)
//					{
//						pIndex[i*3  ] = (WORD)m_lpFacesIndex[i*3  ];
//						pIndex[i*3+1] = (WORD)m_lpFacesIndex[i*3+1];
//						pIndex[i*3+2] = (WORD)m_lpFacesIndex[i*3+2];
//						pAttrib[i] = m_lpSubsetID[i];
//					}
//				}
//				else
//				{
//					for( i=0;i<m_dNumFaces;i++)
//					{
//						pIndex[i*3  ] = (WORD)m_lpFacesIndex[i*3  ];
//						pIndex[i*3+1] = (WORD)m_lpFacesIndex[i*3+1];
//						pIndex[i*3+2] = (WORD)m_lpFacesIndex[i*3+2];
//						pAttrib[i] = m_lpSubsetID[i];
//					}
//				}
//				D3DXComputeBoundingBox((D3DXVECTOR3*) pVers,m_dNumVertices,sizeof(VFormat::FACES_DIFFUSE_TEXTURE1),
//					&m_BBox_A,&m_BBox_B);
//				break;
//			}
//		case (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX2):
//			{
//				VFormat::FACES_DIFFUSE_TEXTURE2 * pVers = NULL;
//				WORD* pIndex = NULL;
//				DWORD * pAttrib = NULL;
//				if (FAILED(m_pMeshSys->LockVertexBuffer(0,(void**)&pVers)))
//					return E_FAIL;
//				if (FAILED(m_pMeshSys->LockIndexBuffer (0,(void**)&pIndex)))
//					return E_FAIL;
//				if (FAILED(m_pMeshSys->LockAttributeBuffer(0,(DWORD**)&pAttrib)))
//					return E_FAIL;
//				for(DWORD i=0;i<m_dNumVertices;i++)
//				{
//					pVers[i].p = m_lpVerticePosition[i];
//					pVers[i].diffuse = m_lpVerticeDiffuse[i];
//					pVers[i].tu1 = m_lpVerticeTextCoord1[i].x;
//					pVers[i].tv1 = m_lpVerticeTextCoord1[i].y;
//					pVers[i].tu2 = m_lpVerticeTextCoord2[i].x;
//					pVers[i].tv2 = m_lpVerticeTextCoord2[i].y;
//				}
//				if(m_dNumFaces<=65535)
//				{
//					for( i=0;i<m_dNumFaces;i++)
//					{
//						pIndex[i*3  ] = (WORD)m_lpFacesIndex[i*3  ];
//						pIndex[i*3+1] = (WORD)m_lpFacesIndex[i*3+1];
//						pIndex[i*3+2] = (WORD)m_lpFacesIndex[i*3+2];
//						pAttrib[i] = m_lpSubsetID[i];
//					}
//				}
//				else
//				{
//					for( i=0;i<m_dNumFaces;i++)
//					{
//						pIndex[i*3  ] = (WORD)m_lpFacesIndex[i*3  ];
//						pIndex[i*3+1] = (WORD)m_lpFacesIndex[i*3+1];
//						pIndex[i*3+2] = (WORD)m_lpFacesIndex[i*3+2];
//						pAttrib[i] = m_lpSubsetID[i];
//					}
//				}
//				D3DXComputeBoundingBox((D3DXVECTOR3*) pVers,m_dNumVertices,sizeof(VFormat::FACES_DIFFUSE_TEXTURE2),
//					&m_BBox_A,&m_BBox_B);
//				break;
//			}
//		case (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX3):
//			{
//				VFormat::FACES_DIFFUSE_TEXTURE3 * pVers = NULL;
//				WORD* pIndex = NULL;
//				DWORD * pAttrib = NULL;
//				if (FAILED(m_pMeshSys->LockVertexBuffer(0,(void**)&pVers)))
//					return E_FAIL;
//				if (FAILED(m_pMeshSys->LockIndexBuffer (0,(void**)&pIndex)))
//					return E_FAIL;
//				if (FAILED(m_pMeshSys->LockAttributeBuffer(0,(DWORD**)&pAttrib)))
//					return E_FAIL;
//				for(DWORD i=0;i<m_dNumVertices;i++)
//				{
//					pVers[i].p = m_lpVerticePosition[i];
//					pVers[i].diffuse = m_lpVerticeDiffuse[i];
//					pVers[i].tu1 = m_lpVerticeTextCoord1[i].x;
//					pVers[i].tv1 = m_lpVerticeTextCoord1[i].y;
//					pVers[i].tu2 = m_lpVerticeTextCoord2[i].x;
//					pVers[i].tv2 = m_lpVerticeTextCoord2[i].y;
//					pVers[i].tu3 = m_lpVerticeTextCoord3[i].x;
//					pVers[i].tv3 = m_lpVerticeTextCoord3[i].y;
//				}
//				if(m_dNumFaces<=65535)
//				{
//					for( i=0;i<m_dNumFaces;i++)
//					{
//						pIndex[i*3  ] = (WORD)m_lpFacesIndex[i*3  ];
//						pIndex[i*3+1] = (WORD)m_lpFacesIndex[i*3+1];
//						pIndex[i*3+2] = (WORD)m_lpFacesIndex[i*3+2];
//						pAttrib[i] = m_lpSubsetID[i];
//					}
//				}
//				else
//				{
//					for( i=0;i<m_dNumFaces;i++)
//					{
//						pIndex[i*3  ] = (WORD)m_lpFacesIndex[i*3  ];
//						pIndex[i*3+1] = (WORD)m_lpFacesIndex[i*3+1];
//						pIndex[i*3+2] = (WORD)m_lpFacesIndex[i*3+2];
//						pAttrib[i] = m_lpSubsetID[i];
//					}
//				}
//				D3DXComputeBoundingBox((D3DXVECTOR3*) pVers,m_dNumVertices,sizeof(VFormat::FACES_DIFFUSE_TEXTURE3),
//					&m_BBox_A,&m_BBox_B);
//				break;
//			}
//		case (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX1):
//			{
//				VFormat::FACES_NORMAL_DIFFUSE_TEXTURE1 * pVers = NULL;
//				WORD* pIndex = NULL;
//				DWORD * pAttrib = NULL;
//				if (FAILED(m_pMeshSys->LockVertexBuffer(0,(void**)&pVers)))
//					return E_FAIL;
//				if (FAILED(m_pMeshSys->LockIndexBuffer (0,(void**)&pIndex)))
//					return E_FAIL;
//				if (FAILED(m_pMeshSys->LockAttributeBuffer(0,(DWORD**)&pAttrib)))
//					return E_FAIL;
//				for(DWORD i=0;i<m_dNumVertices;i++)
//				{
//					pVers[i].p = m_lpVerticePosition[i];
//					pVers[i].Normal = m_lpVerticeNormal[i];
//					pVers[i].diffuse = m_lpVerticeDiffuse[i];
//					pVers[i].tu1 = m_lpVerticeTextCoord1[i].x;
//					pVers[i].tv1 = m_lpVerticeTextCoord1[i].y;
//				}
//				if(m_dNumFaces<=65535)
//				{
//					for( i=0;i<m_dNumFaces;i++)
//					{
//						pIndex[i*3  ] = (WORD)m_lpFacesIndex[i*3  ];
//						pIndex[i*3+1] = (WORD)m_lpFacesIndex[i*3+1];
//						pIndex[i*3+2] = (WORD)m_lpFacesIndex[i*3+2];
//						pAttrib[i] = m_lpSubsetID[i];
//					}
//				}
//				else
//				{
//					for( i=0;i<m_dNumFaces;i++)
//					{
//						pIndex[i*3  ] = (WORD)m_lpFacesIndex[i*3  ];
//						pIndex[i*3+1] = (WORD)m_lpFacesIndex[i*3+1];
//						pIndex[i*3+2] = (WORD)m_lpFacesIndex[i*3+2];
//						pAttrib[i] = m_lpSubsetID[i];
//					}
//				}
//				D3DXComputeBoundingBox((D3DXVECTOR3*) pVers,m_dNumVertices,sizeof(VFormat::FACES_NORMAL_DIFFUSE_TEXTURE1),
//					&m_BBox_A,&m_BBox_B);
//				break;
//			}
//		case (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX2):
//			{
//				VFormat::FACES_NORMAL_DIFFUSE_TEXTURE2 * pVers = NULL;
//				WORD* pIndex = NULL;
//				DWORD * pAttrib = NULL;
//				if (FAILED(m_pMeshSys->LockVertexBuffer(0,(void**)&pVers)))
//					return E_FAIL;
//				if (FAILED(m_pMeshSys->LockIndexBuffer (0,(void**)&pIndex)))
//					return E_FAIL;
//				if (FAILED(m_pMeshSys->LockAttributeBuffer(0,(DWORD**)&pAttrib)))
//					return E_FAIL;
//				for(DWORD i=0;i<m_dNumVertices;i++)
//				{
//					pVers[i].p = m_lpVerticePosition[i];
//					pVers[i].Normal = m_lpVerticeNormal[i];
//					pVers[i].diffuse = m_lpVerticeDiffuse[i];
//					pVers[i].tu1 = m_lpVerticeTextCoord1[i].x;
//					pVers[i].tv1 = m_lpVerticeTextCoord1[i].y;
//					pVers[i].tu2 = m_lpVerticeTextCoord2[i].x;
//					pVers[i].tv2 = m_lpVerticeTextCoord2[i].y;
//				}
//				if(m_dNumFaces<=65535)
//				{
//					for( i=0;i<m_dNumFaces;i++)
//					{
//						pIndex[i*3  ] = (WORD)m_lpFacesIndex[i*3  ];
//						pIndex[i*3+1] = (WORD)m_lpFacesIndex[i*3+1];
//						pIndex[i*3+2] = (WORD)m_lpFacesIndex[i*3+2];
//						pAttrib[i] = m_lpSubsetID[i];
//					}
//				}
//				else
//				{
//					for( i=0;i<m_dNumFaces;i++)
//					{
//						pIndex[i*3  ] =  (WORD)m_lpFacesIndex[i*3  ];
//						pIndex[i*3+1] =  (WORD)m_lpFacesIndex[i*3+1];
//						pIndex[i*3+2] =  (WORD)m_lpFacesIndex[i*3+2];
//						pAttrib[i] = m_lpSubsetID[i];
//					}
//				}
//				D3DXComputeBoundingBox((D3DXVECTOR3*) pVers,m_dNumVertices,sizeof(VFormat::FACES_NORMAL_DIFFUSE_TEXTURE2),
//					&m_BBox_A,&m_BBox_B);
//				break;
//			}
//		case (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX3):
//			{
//				VFormat::FACES_NORMAL_DIFFUSE_TEXTURE3 * pVers = NULL;
//				WORD* pIndex = NULL;
//				DWORD * pAttrib = NULL;
//				if (FAILED(m_pMeshSys->LockVertexBuffer(0,(void**)&pVers)))
//					return E_FAIL;
//				if (FAILED(m_pMeshSys->LockIndexBuffer (0,(void**)&pIndex)))
//					return E_FAIL;
//				if (FAILED(m_pMeshSys->LockAttributeBuffer(0,(DWORD**)&pAttrib)))
//					return E_FAIL;
//				for(DWORD i=0;i<m_dNumVertices;i++)
//				{
//					pVers[i].p = m_lpVerticePosition[i];
//					pVers[i].Normal = m_lpVerticeNormal[i];
//					pVers[i].diffuse = m_lpVerticeDiffuse[i];
//					pVers[i].tu1 = m_lpVerticeTextCoord1[i].x;
//					pVers[i].tv1 = m_lpVerticeTextCoord1[i].y;
//					pVers[i].tu2 = m_lpVerticeTextCoord2[i].x;
//					pVers[i].tv2 = m_lpVerticeTextCoord2[i].y;
//					pVers[i].tu3 = m_lpVerticeTextCoord3[i].x;
//					pVers[i].tv3 = m_lpVerticeTextCoord3[i].y;
//				}
//				if(m_dNumFaces<=65535)
//				{
//					for( i=0;i<m_dNumFaces;i++)
//					{
//						pIndex[i*3  ] = (WORD)m_lpFacesIndex[i*3  ];
//						pIndex[i*3+1] = (WORD)m_lpFacesIndex[i*3+1];
//						pIndex[i*3+2] = (WORD)m_lpFacesIndex[i*3+2];
//						pAttrib[i] = m_lpSubsetID[i];
//					}
//				}
//				else
//				{
//					for( i=0;i<m_dNumFaces;i++)
//					{
//						pIndex[i*3  ] =  (WORD)m_lpFacesIndex[i*3  ];
//						pIndex[i*3+1] =  (WORD)m_lpFacesIndex[i*3+1];
//						pIndex[i*3+2] =  (WORD)m_lpFacesIndex[i*3+2];
//						pAttrib[i] = m_lpSubsetID[i];
//					}
//				}
//				D3DXComputeBoundingBox((D3DXVECTOR3*) pVers,m_dNumVertices,sizeof(VFormat::FACES_NORMAL_DIFFUSE_TEXTURE3),
//					&m_BBox_A,&m_BBox_B);
//				break;
//			}
//		}
//
//		if (FAILED(m_pMeshSys->UnlockVertexBuffer()))
//			return E_FAIL;
//		if (FAILED(m_pMeshSys->UnlockIndexBuffer()))
//			return E_FAIL;
//		if (FAILED(m_pMeshSys->UnlockAttributeBuffer()))
//			return E_FAIL;
//
//		SAFE_DELETE_ARRAY(pMeshHead);
//		SAFE_DELETE_ARRAY(m_lpVerticePosition);
//		SAFE_DELETE_ARRAY(m_lpVerticeNormal);
//		SAFE_DELETE_ARRAY(m_lpVerticeTextCoord1);
//		SAFE_DELETE_ARRAY(m_lpVerticeTextCoord2);
//		SAFE_DELETE_ARRAY(m_lpVerticeTextCoord3);
//		SAFE_DELETE_ARRAY(m_lpFacesIndex);
//		SAFE_DELETE_ARRAY(m_lpSubsetID);
//
//		m_dNumMaterial = m_dNumSubsets;
//		m_lpMaterial = new MATERIAL[m_dNumMaterial];
//		ZeroMemory(m_lpMaterial,sizeof(MATERIAL)*m_dNumMaterial);
//
//		DWORD Def_Option = MATERIAL_OPTION_ZBUFFER_TRUE|
//			MATERIAL_OPTION_FILL_SOLID|
//			MATERIAL_OPTION_SHADE_GOURAUD|
//			MATERIAL_OPTION_CULL_CW|
//			MATERIAL_OPTION_SPECULARENABLE|
//			MATERIAL_OPTION_LIGHTING;
//
//		for(DWORD i=0;i<m_dNumMaterial;i++)
//		{
//			m_lpMaterial[i].m_sMaterial9.Diffuse.r = 0.7f ;
//			m_lpMaterial[i].m_sMaterial9.Diffuse.g = 0.7f ;
//			m_lpMaterial[i].m_sMaterial9.Diffuse.b = 0.7f ;
//			m_lpMaterial[i].m_sMaterial9.Diffuse.a = 1.0f ;
//			m_lpMaterial[i].m_sMaterial9.Ambient = m_lpMaterial[i].m_sMaterial9.Diffuse;
//			m_lpMaterial[i].m_sMaterial9.Specular = m_lpMaterial[i].m_sMaterial9.Diffuse;
//			m_lpMaterial[i].m_sMaterial9.Power = 15;
//			m_lpMaterial[i].m_dOption = Def_Option;
//		}
//
//		m_lpStaticVertexPos = m_lpVerticePosition;
//		//m_bRecaculateNormal = false;
//		if ((FVF&D3DFVF_NORMAL) && m_bRecaculateNormal)
//			D3DXComputeNormals(m_pMeshSys,NULL);
//		/////////////////////////////////////////////////////////////////////////////////////
//		//load shader for test
//		//LoadShader();
//
//		if (!m_bSkined)
//		{
//			//CreateProgressMesh();
//			OptimizeUnSkinedMesh();
//		}
//	
//
//		
//
//		if (m_bSkined)
//		{
//			CreateSkinInfo();
//			//CreateProgressMesh();
//		}
//		
//		//Save blend mesh
//		if ( m_pBlendMesh )
//		{
//			DWORD dwBlockEnd = 0xffffffff;
//			pFile = fopen(pFileName, "wb");
//			//lBlendMeshOffset = lBlendMeshOffset - ftell(pFile);
//			SaveMesh(pFile);
//			/*MeshFileHead.ExtendBlock[BLEND_MESH_SAVED] = lBlendMeshOffset;
//			fseek(pFile, SEEK_SET, 0);
//			fwrite(&MeshFileHead, sizeof(_MeshFileHead), 1, pFile);
//			fseek(pFile, SEEK_END, 0 );
//			*///SaveBlendedMesh(m_pBlendMesh, pFile);
//			FILE* fp1 = fopen("c:\\test.mesh", "wb");
//			SaveBlendedMesh(m_pBlendMesh, fp1);
//			fwrite(&dwBlockEnd, sizeof(DWORD), 1, pFile);
//			fclose(pFile);
//		}
//		else
//		{
//			fread(&BlockEnd,sizeof(DWORD),1,pFile);
//			fclose(pFile);
//			wsprintf(m_szMeshFileName,"%s",pFileName);
//
//
//			m_dNumMaterial = m_dNumSubsets;
//			m_lpMaterial = new MATERIAL[m_dNumMaterial];
//			ZeroMemory(m_lpMaterial,sizeof(MATERIAL)*m_dNumMaterial);
//
//			DWORD Def_Option = MATERIAL_OPTION_ZBUFFER_TRUE|
//				MATERIAL_OPTION_FILL_SOLID|
//				MATERIAL_OPTION_SHADE_GOURAUD|
//				MATERIAL_OPTION_CULL_CW|
//				MATERIAL_OPTION_SPECULARENABLE|
//				MATERIAL_OPTION_LIGHTING;
//
//			for(DWORD i=0;i<m_dNumMaterial;i++)
//			{
//				m_lpMaterial[i].m_sMaterial9.Diffuse.r = 0.7f ;
//				m_lpMaterial[i].m_sMaterial9.Diffuse.g = 0.7f ;
//				m_lpMaterial[i].m_sMaterial9.Diffuse.b = 0.7f ;
//				m_lpMaterial[i].m_sMaterial9.Diffuse.a = 1.0f ;
//				m_lpMaterial[i].m_sMaterial9.Ambient = m_lpMaterial[i].m_sMaterial9.Diffuse;
//				m_lpMaterial[i].m_sMaterial9.Specular = m_lpMaterial[i].m_sMaterial9.Diffuse;
//				m_lpMaterial[i].m_sMaterial9.Power = 15;
//				m_lpMaterial[i].m_dOption = Def_Option;
//			}
//
//		}
//	}
//	else//Load saved blend mesh
//	{
//		fseek(pFile, MeshFileHead.ExtendBlock[BLEND_MESH_SAVED], SEEK_SET);
//		m_pBoneMatrices = new D3DXMATRIX[m_dNumBone];
//		LoadBlendMesh(pFile, &m_pBlendMesh);
//		fread(&BlockEnd,sizeof(DWORD),1,pFile);
//		fclose(pFile);
//		wsprintf(m_szMeshFileName,"%s",pFileName);
//
//	}
//
//	
//	//////////////////////////////////////////////////////////////////////////
//	//Test PRT
//	//////////////////////////////////////////////////////////////////////////
//	//LPD3DXMESH pMesh;
//	//(m_dOption & MESH_OPTIMIZED) ? pMesh = m_pMeshOpt :	pMesh = m_pMeshSys;
//
//	////Rearrange index to make every vertex to be clock wise
//	//DWORD face1 = m_pMeshSys->GetNumFaces();
//	//HRESULT hrReCalNormal = S_OK;
//	//DWORD dwFaceCnt = pMesh->GetNumFaces();
//	////if ( dwFaceCnt < 500 )
//	//{
//	//	//m_bDrawNormal = FALSE;
//	//	//return S_OK;
//	//}
//	//BYTE* pMeshVB = NULL;
//	//DWORD dwFaceIndex;
//	//DWORD dwVertexCnt = pMesh->GetNumVertices();
//	//DWORD dwMeshStride = pMesh->GetNumBytesPerVertex();
//
//	//BOOL bDWORDIndex = false;
//	//if( dwVertexCnt > 65535 )
//	//	bDWORDIndex = true;
//	//WORD* pIB = NULL;
//	//if ( m_bRecaculateNormal )
//	//{
//	//	hrReCalNormal = pMesh->LockIndexBuffer(0, (void**)&pIB);
//	//	hrReCalNormal = pMesh->LockVertexBuffer( 0, (void**)&pMeshVB);
//	//	for ( dwFaceIndex = 0; dwFaceIndex < dwFaceCnt; dwFaceIndex++ )
//	//	{
//	//		D3DXVECTOR3 vec3Buffer[3];
//	//		D3DXVECTOR3 vec3Normal;
//
//	//		//Extract the face points position 
//	//		vec3Buffer[0] = *((D3DXVECTOR3*)(pMeshVB + (*pIB) * dwMeshStride));
//	//		vec3Buffer[1] = *((D3DXVECTOR3*)(pMeshVB + (*(pIB + 1)) * dwMeshStride));
//	//		vec3Buffer[2] = *((D3DXVECTOR3*)(pMeshVB + (*(pIB + 2)) * dwMeshStride));
//
//	//		D3DXVECTOR3 vec3Ct = (vec3Buffer[0] + vec3Buffer[1] + vec3Buffer[2]) / 3.0f;
//	//		//Get face normal, CW
//	//		GetFaceNormal(vec3Buffer, &vec3Normal);
//	//		//Vertex indices sequence reverse
//	//		/*if ( D3DXVec3Dot(&vec3Ct, &vec3Normal) > 0.0f )
//	//		{
//	//		WORD wTemp = *(pIB + 2);
//	//		*(pIB + 2) = *(pIB + 1);
//	//		*(pIB + 1) = wTemp;
//	//		vec3Normal *= -1.0f;
//	//		} */
//
//	//		int i;
//	//		for ( i = 0; i < 3; i++ )
//	//		{
//	//			D3DXVECTOR3& vec3PtNormal = *((D3DXVECTOR3*)(pMeshVB + (*(pIB + i)) * dwMeshStride) + 1);
//	//			if ( D3DXVec3Dot(&vec3Normal, &vec3PtNormal ) > 0.0f )
//	//				vec3PtNormal *= -1.0f;
//	//		}
//	//		pIB += 3;
//
//	//	}
//
//	//	pMesh->UnlockIndexBuffer();
//	//	pMesh->UnlockVertexBuffer();
//
//	//}
//
//
//	////Prepare for normal vertex buffer
//	////m_bDrawNormal = false;
//	//if ( m_bDrawNormal )
//	//{
//	//	HRESULT hrDrawNormal;
//	//	SAFE_RELEASE(m_pVBNormal);
//	//	_NormalPoint* pNormalVB = NULL;
//
//	//	hrDrawNormal = pMesh->LockVertexBuffer( 0, (void**)&pMeshVB);
//	//	if ( FAILED(hrDrawNormal) )
//	//		goto Err_Normal_Init;
//
//	//	hrDrawNormal = g_pd3dDevice->CreateVertexBuffer(dwVertexCnt * 2 * sizeof(_NormalPoint),0,  FVF_NORMAL, D3DPOOL_MANAGED, &m_pVBNormal, 0);
//	//	if (FAILED(hrDrawNormal))
//	//		goto Err_Normal_Init;
//
//
//
//	//	hrDrawNormal = m_pVBNormal->Lock(0, 0, (void**)&pNormalVB, 0);
//	//	if ( FAILED(hrDrawNormal) )
//	//		goto Err_Normal_Init;
//
//	//	DWORD dwIndex;
//	//	for ( dwIndex = 0; dwIndex < dwVertexCnt; dwIndex++ )
//	//	{
//	//		D3DXVECTOR3* pVec3VB = (D3DXVECTOR3*)pMeshVB;
//	//		pNormalVB->vPosition = *pVec3VB;
//	//		pNormalVB->color = 0xffff0000;
//	//		(pNormalVB+1)->vPosition = (*(pVec3VB + 1) * 1) + (*pVec3VB);
//	//		(pNormalVB+1)->color = 0xff0000ff;
//	//		//Reverse the normal
//	//		//*(pVec3VB+1) *= -1.0f;
//	//		pMeshVB += dwMeshStride;
//	//		pNormalVB += 2;
//	//	}
//
//	//	pMesh->UnlockVertexBuffer();
//	//	m_pVBNormal->Unlock();
//	//}
//
//	//PRT_AdjustMeshDecl();
//	//DWORD dwAttCnt;
//	//m_pMeshPRT->GetAttributeTable(NULL, &dwAttCnt);
//	///*if ( dwAttCnt != m_dNumMaterial )
//	//{
//	//	Save2XFile("c:\\temp.x");
//	//	SAFE_RELEASE(m_pMeshSys);
//	//	SAFE_RELEASE(m_pMeshPRT);
//	//	hr = D3DXLoadMeshFromX("c:\\temp.x", D3DXMESH_MANAGED, 
//	//		g_pd3dDevice,
//	//		NULL,
//	//		NULL,NULL,
//	//		&dwAttCnt, 
//	//		&m_pMeshSys );
//	//}*/
//	//
//	//TCHAR strDriver[MAX_PATH];
//	//TCHAR strPath[MAX_PATH];
//	//TCHAR strFile[MAX_PATH];
//	//TCHAR strExt[MAX_PATH];
//	//_splitpath(m_szMeshFileName, strDriver, strPath, strFile, strExt);
//	//strcpy(m_PRTReloadState.strPRTBufferFileName, strFile);
//	//int iLength = strlen(strFile);
//	//strcpy(strFile + iLength, ".pca");
//	//m_PRTReloadState.bUseReloadState = true;
//	//m_PRTReloadState.bLoadCompressed = true;
//	//g_cFileFinder.FindFile(m_PRTReloadState.strPRTBufferFileName, strFile);
//	//if ( !strcmp(m_PRTReloadState.strPRTBufferFileName, "") )
//	//{
//	//	strcpy(strFile + iLength , ".prt");
//	//	m_PRTReloadState.bLoadCompressed = false;
//	//	g_cFileFinder.FindFile(m_PRTReloadState.strPRTBufferFileName, strFile);
//	//	if ( !strcmp(m_PRTReloadState.strPRTBufferFileName, "") )
//	//	{
//	//		m_PRTReloadState.bUseReloadState = false;
//	//		return S_OK;
//	//	}
//
//	//}
//	//PRT_Initialize();
//	
//	//////////////////////////////////////////////////////////////////////////
//	//Test PRT END
//	//////////////////////////////////////////////////////////////////////////
//
//	//////////////////////////////////////////////////////////////////////////
//	//Add 6-1-2005 test blend mesh save
//	/*if ( m_bSkined )
//	{
//		FILE* fp = fopen("d:\\test.mesh", "wb");
//		SaveBlendedMesh(m_pBlendMesh, fp);
//		fclose(fp);
//		m_pBlendMesh->Release();
//		fp = fopen("d:\\test.mesh", "rb");
//		LoadBlendMesh(fp, &m_pBlendMesh);
//		fclose(fp);
//
//	}*/
//	//Add 6-1-2005 test blend mesh save end
//	//////////////////////////////////////////////////////////////////////////
//	
//
//	return S_OK;
//
//Err_Normal_Init:
//	m_bDrawNormal = false;
//	return S_OK;
//
//} 

HRESULT KModel::LoadMesh(LPSTR pFileName)
{
	_MeshFileHead MeshFileHead;

	DWORD BlockEnd  = 0xFFFFFFFF;  
	FILE* pFile = fopen(pFileName,"rb");
	if (pFile==NULL) return E_FAIL;

	DWORD Pos_Start = ftell(pFile);

	fread(&MeshFileHead,sizeof(_MeshFileHead),1,pFile);
	if (MeshFileHead.FileMask!=0x4D455348) return E_FAIL;

	DWORD m_dNumVertices = 0;
	DWORD m_dNumFaces    = 0;
	DWORD m_dNumSubsets  = 0;
	BOOL  m_bSkined      = 0;

	D3DXVECTOR3 * m_lpVerticePosition = NULL;
	D3DXVECTOR3 * m_lpVerticeNormal   = NULL;
	D3DCOLOR    * m_lpVerticeDiffuse  = NULL;

	D3DXVECTOR3 * m_lpVerticeTextCoord1 = NULL;
	D3DXVECTOR3 * m_lpVerticeTextCoord2 = NULL;
	D3DXVECTOR3 * m_lpVerticeTextCoord3 = NULL;

	DWORD * m_lpFacesIndex = NULL;
	DWORD * m_lpSubsetID   = NULL;

	_MeshHead* pMeshHead = new _MeshHead[MeshFileHead.MeshCount];
	
	for(DWORD i_Mesh=0;i_Mesh<MeshFileHead.MeshCount;i_Mesh++)
	{
		fread(&pMeshHead[i_Mesh],sizeof(_MeshHead),1,pFile);

		m_dNumVertices = pMeshHead[i_Mesh].NumVertices;//顶点数目
		m_dNumFaces    = pMeshHead[i_Mesh].NumFaces;//面（三角形）数目
		m_dNumSubsets  = pMeshHead[i_Mesh].NumSubset;//子部分数目
		m_bSkined      = pMeshHead[i_Mesh].SkinInfoBlock;//是否包含骨骼信息

		if (pMeshHead[i_Mesh].PositionBlock)
		{
			m_lpVerticePosition = new D3DXVECTOR3[m_dNumVertices];
			ZeroMemory(m_lpVerticePosition,sizeof(D3DXVECTOR3)*m_dNumVertices);

			fseek(pFile,Pos_Start+pMeshHead[i_Mesh].PositionBlock,SEEK_SET);

			fread( m_lpVerticePosition,sizeof(D3DXVECTOR3),m_dNumVertices,pFile); //顶点位置坐标
			fread(&BlockEnd,sizeof(DWORD),1,pFile);
		}
		if (pMeshHead[i_Mesh].NormalBlock)
		{
			m_lpVerticeNormal = new D3DXVECTOR3[m_dNumVertices];
			ZeroMemory(m_lpVerticeNormal,sizeof(D3DXVECTOR3)*m_dNumVertices);

			fseek(pFile,Pos_Start+pMeshHead[i_Mesh].NormalBlock,SEEK_SET);

			fread( m_lpVerticeNormal,sizeof(D3DXVECTOR3),m_dNumVertices,pFile); //顶点位置坐标
			fread(&BlockEnd,sizeof(DWORD),1,pFile);
		}

		if (pMeshHead[i_Mesh].DiffuseBlock)
		{
			m_lpVerticeDiffuse = new D3DCOLOR[m_dNumVertices];
			ZeroMemory(m_lpVerticeDiffuse,sizeof(D3DCOLOR)*m_dNumVertices);

			fseek(pFile,Pos_Start+pMeshHead[i_Mesh].DiffuseBlock,SEEK_SET);

			fread( m_lpVerticeDiffuse,sizeof(D3DCOLOR),m_dNumVertices,pFile); //顶点位置坐标
			fread(&BlockEnd,sizeof(DWORD),1,pFile);
		}

		if (pMeshHead[i_Mesh].TextureUVW1Block)
		{
			m_lpVerticeTextCoord1 = new D3DXVECTOR3[m_dNumVertices];
			ZeroMemory(m_lpVerticeTextCoord1,sizeof(D3DXVECTOR3)*m_dNumVertices);

			fseek(pFile,Pos_Start+pMeshHead[i_Mesh].TextureUVW1Block,SEEK_SET);

			fread( m_lpVerticeTextCoord1,sizeof(D3DXVECTOR3),m_dNumVertices,pFile); //顶点位置坐标
			fread(&BlockEnd,sizeof(DWORD),1,pFile);
		}

		if (pMeshHead[i_Mesh].TextureUVW2Block)
		{
			m_lpVerticeTextCoord2 = new D3DXVECTOR3[m_dNumVertices];
			ZeroMemory(m_lpVerticeTextCoord2,sizeof(D3DXVECTOR3)*m_dNumVertices);

			fseek(pFile,Pos_Start+pMeshHead[i_Mesh].TextureUVW2Block,SEEK_SET);

			fread( m_lpVerticeTextCoord2,sizeof(D3DXVECTOR3),m_dNumVertices,pFile); //顶点位置坐标
			fread(&BlockEnd,sizeof(DWORD),1,pFile);
		}

		if (pMeshHead[i_Mesh].TextureUVW3Block)
		{
			m_lpVerticeTextCoord3 = new D3DXVECTOR3[m_dNumVertices];
			ZeroMemory(m_lpVerticeTextCoord3,sizeof(D3DXVECTOR3)*m_dNumVertices);

			fseek(pFile,Pos_Start+pMeshHead[i_Mesh].TextureUVW3Block,SEEK_SET);

			fread( m_lpVerticeTextCoord3,sizeof(D3DXVECTOR3),m_dNumVertices,pFile); //顶点位置坐标
			fread(&BlockEnd,sizeof(DWORD),1,pFile);
		}

		if (pMeshHead[i_Mesh].FacesIndexBlock)
		{
			m_lpFacesIndex = new DWORD[m_dNumFaces*3];
			ZeroMemory(m_lpFacesIndex,sizeof(DWORD)*m_dNumFaces*3);

			fseek(pFile,Pos_Start+pMeshHead[i_Mesh].FacesIndexBlock,SEEK_SET);

			fread( m_lpFacesIndex,sizeof(DWORD),m_dNumFaces*3,pFile);//面索引
			fread(&BlockEnd,sizeof(DWORD),1,pFile);
		}

		if (pMeshHead[i_Mesh].SubsetIndexBlock)
		{
			m_lpSubsetID = new DWORD[m_dNumFaces];
			ZeroMemory(m_lpSubsetID,sizeof(DWORD)*m_dNumFaces);

			fseek(pFile,Pos_Start+pMeshHead[i_Mesh].SubsetIndexBlock,SEEK_SET);

			fread( m_lpSubsetID  ,sizeof(DWORD),m_dNumFaces,pFile);  //子部分索引
			fread(&BlockEnd,sizeof(DWORD),1,pFile);

		}
		if (m_bSkined)
		{
			DWORD NumBone = 0;
			fread(&NumBone,sizeof(DWORD),1,pFile);//骨骼数目
			LPBONEINFO lpSrcBoneInfo = new BONEINFO[NumBone];

			for(DWORD i_Bone=0;i_Bone<NumBone;i_Bone++)
			{
				LPBONEINFO pInfo = &lpSrcBoneInfo[i_Bone];

				TCHAR Info_Name[256];
				TCHAR Info_ParentName[256];

				fread(Info_Name,sizeof(char),30,pFile);
				fread(Info_ParentName,sizeof(char),30,pFile);

				pInfo->scName = Info_Name;
				pInfo->scParentName = Info_ParentName;

				if (strcmpi(pInfo->scParentName.c_str(),"Scene Root")==0)
					m_nBaseBoneID.push_back(i_Bone);

				DWORD NumChild = 0;
				fread(&NumChild,sizeof(DWORD),1,pFile);	

				for(DWORD i_Child=0;i_Child<NumChild;i_Child++)
				{
					TCHAR str[256];
					fread(str,sizeof(TCHAR),30,pFile);//
					string ChileName = str;
					pInfo->ChildNameVector.push_back(ChileName);
					int S = 0;
					pInfo->ChildIDVector.push_back(S);
				}

				fread(&pInfo->MatrixOffset,sizeof(D3DXMATRIX),1,pFile);//
				fread(&pInfo->MatrixOffsetBase,sizeof(D3DXMATRIX),1,pFile);//

				fread(&pInfo->NumVerticesRef,sizeof(DWORD),1,pFile);//
				pInfo->VerticesIndexRef = new DWORD[pInfo->NumVerticesRef];
				ZeroMemory(pInfo->VerticesIndexRef,sizeof(DWORD)*pInfo->NumVerticesRef);
				pInfo->VerticesWeightRef = new float[pInfo->NumVerticesRef];
				ZeroMemory(pInfo->VerticesWeightRef,sizeof(float)*pInfo->NumVerticesRef);

				fread(pInfo->VerticesIndexRef,sizeof(DWORD),
					pInfo->NumVerticesRef,pFile);//
				fread(pInfo->VerticesWeightRef,sizeof(float),
					pInfo->NumVerticesRef,pFile);//

				D3DXMatrixInverse(&pInfo->CurMatrix,NULL,&pInfo->MatrixOffset);
				pInfo->RelativeMatrix = pInfo->MatrixOffsetBase;
				D3DXMatrixInverse(&pInfo->MatrixOffsetBase,NULL,&pInfo->MatrixOffset);


				//D3DXVECTOR3 Scaling,Position;

				//MatrixExract(Scaling,Position,pInfo->Rotation,&pInfo->CurMatrix);

				//pInfo->BonePosition = Position;
				//pInfo->BaseBonePosition = pInfo->BonePosition;
				//pInfo->LocalWorldCurMatrix = pInfo->CurMatrix;
				//			pInfo->ObjectBonePosition = pInfo->BonePosition;

				pInfo->BoneScale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);//Scaling;
				//pInfo->BaseBoneScale = pInfo->BoneScale;
				//pInfo->BaseRotation = pInfo->Rotation;
			}

			m_lpBoneInfo = lpSrcBoneInfo;
			m_dOption |= MESH_SKINED;
			m_dNumBone = NumBone;

			DWORD NumSocket = 0;
			fread(&NumSocket,sizeof(DWORD),1,pFile);
			if (NumSocket>0)
			{
				m_lpSockets = new SOCKET[NumSocket];

				for(DWORD i_Socket=0;i_Socket<NumSocket;i_Socket++)
				{
					LPSOCKET pSocket = &m_lpSockets[i_Socket];
					//pSocket->scName = new TCHAR[30];
					//pSocket->scParentBoneName = new TCHAR[30];

					TCHAR Socket_Name[256];
					TCHAR Socket_ParentBoneName[256];
					fread(Socket_Name,sizeof(TCHAR),30,pFile);//
					fread(Socket_ParentBoneName,sizeof(TCHAR),30,pFile);//
					fread(&pSocket->MatrixOffset,sizeof(D3DMATRIX),1,pFile);//
					pSocket->scName = Socket_Name;
					pSocket->scParentBoneName = Socket_ParentBoneName;


					pSocket->MatrixOffsetBack = pSocket->MatrixOffset;//对偏移矩阵做个备份,便于修改


					int Index = FindBone((LPSTR)(pSocket->scParentBoneName.c_str())); 
					//5-9-2005 start
					/*if (Index>=0)
					{
					D3DXMATRIX mat;
					LPBONEINFO pInfoParent = &m_lpBoneInfo[Index];
					D3DXMatrixMultiply(&pSocket->CurMatrix,
					&pInfoParent->CurMatrix,
					&pSocket->MatrixOffset);
					D3DXVECTOR3 A,B,C;
					A = D3DXVECTOR3(pInfoParent->CurMatrix._11,pInfoParent->CurMatrix._12,pInfoParent->CurMatrix._13);
					B = D3DXVECTOR3(pInfoParent->CurMatrix._21,pInfoParent->CurMatrix._22,pInfoParent->CurMatrix._23);
					C = D3DXVECTOR3(pInfoParent->CurMatrix._31,pInfoParent->CurMatrix._32,pInfoParent->CurMatrix._33);
					float Sx,Sy,Sz;
					Sx = D3DXVec3Length(&A);
					Sy = D3DXVec3Length(&B);
					Sz = D3DXVec3Length(&C);
					D3DXMATRIX matScale;
					D3DXMatrixScaling(&matScale,Sx,Sy,Sz);
					D3DXMatrixInverse(&matScale,NULL,&matScale);

					D3DXMatrixMultiply(&mat,
					&matScale,
					&pInfoParent->CurMatrix);

					D3DXMatrixMultiply(&pSocket->CurMatrix,
					&pSocket->MatrixOffset ,&mat);
					}*/
					//5-9-2005 end
					if (Index>=0)
					{
						D3DXMATRIX mat;
						LPBONEINFO pInfoParent = &m_lpBoneInfo[Index];
						//pSocket->CurMatrix = pSocket->MatrixOffset;
						D3DXMatrixMultiply(&pSocket->CurMatrix,
							&pSocket->MatrixOffset, &pInfoParent->CurMatrix);
					}
				}
			}
			m_dNumSocket = NumSocket;

			//////////////////////////////////////////////////////////////////////////
			//先整理骨骼基本信息
			for(i_Bone=0;i_Bone<NumBone;i_Bone++)
			{
				LPBONEINFO pInfo = &lpSrcBoneInfo[i_Bone];

				char flex[] = "FL_";
				char*  result;
				result = strstr( pInfo->scName.c_str(), flex);
				if(result)
					pInfo->bFlexibleBone = TRUE;

				pInfo->nParentIndex = FindBone((LPSTR)pInfo->scParentName.c_str());
				if(pInfo->nParentIndex!=-1)
				{
					LPBONEINFO pInfoParent =  &lpSrcBoneInfo[pInfo->nParentIndex];
					D3DXMATRIX Mat;
					D3DXMatrixInverse(&Mat,NULL,&pInfoParent->CurMatrix);
					pInfo->RelativeMatrix = pInfo->CurMatrix * Mat;

				}
				for(DWORD i_Child=0;i_Child<pInfo->ChildNameVector.size();i_Child++)
				{
					pInfo->ChildIDVector[i_Child] = 
						FindBone((LPSTR)pInfo->ChildNameVector[i_Child].c_str());
				}
			}

			//////////////////////////////////////////////////////////////////////////
			//创建柔体骨骼的固定点,固定点是骨骼链的起点或者骨骼链上第一个柔体骨骼
			for(i_Bone=0;i_Bone<NumBone;i_Bone++)	
			{
				LPBONEINFO pInfo = &lpSrcBoneInfo[i_Bone];
				if(pInfo->bFlexibleBone)
				{
					/*			if ((pInfo->nParentIndex>=0)&&(pInfo->nParentIndex<(int)NumBone))
					{
					LPBONEINFO pParentInfo = &lpSrcBoneInfo[pInfo->nParentIndex];
					D3DXVECTOR3 BoneVec = pInfo->BonePosition - pParentInfo->BonePosition;
					//pInfo->BaseBonePosition = BoneVec;
					pInfo->BoneLength = D3DXVec3Length(&BoneVec);
					if(!pParentInfo->bFlexibleBone)
					{
					_FlexibleBone FBone;
					FBone.Index = i_Bone;
					FBone.PrePosition = pInfo->BonePosition;
					m_vecStaticFlexibleBoneIndex.push_back(FBone);

					}
					}
					if(pInfo->nParentIndex==-1)
					{
					pInfo->BoneLength = 0;
					_FlexibleBone FBone;
					FBone.Index = i_Bone;
					FBone.PrePosition = pInfo->BonePosition;


					m_vecStaticFlexibleBoneIndex.push_back(FBone);					
					}
					}
					}*/
					if(pInfo->nParentIndex==-1)
					{
						pInfo->BoneLength = 0;
						_FlexibleBone FBone;
						FBone.Index = i_Bone;
						FBone.PrePosition = pInfo->BonePosition;
						m_vecStaticFlexibleBoneIndex.push_back(FBone);					
					}
					else if ((pInfo->nParentIndex>=0)&&(pInfo->nParentIndex<(int)NumBone))
					{
						LPBONEINFO pParentInfo = &lpSrcBoneInfo[pInfo->nParentIndex];
						D3DXVECTOR3 BoneVec = pInfo->BonePosition - pParentInfo->BonePosition;

						pInfo->BonePositionOffset = BoneVec;
						//pInfo->BaseBonePosition = BoneVec;
						pInfo->BoneLength = D3DXVec3Length(&BoneVec);

						if(!pParentInfo->bFlexibleBone)
						{
							_FlexibleBone FBone;
							FBone.Index = i_Bone;
							FBone.PrePosition = pInfo->BonePosition;
							m_vecStaticFlexibleBoneIndex.push_back(FBone);
						}
					}
					else
					{
						pInfo->nParentIndex =-1;
					}
				}
			} 

		}
	}

	//if ( MeshFileHead.ExtendBlock[BLEND_MESH_SAVED] )
	//{
	//	fseek(pFile, MeshFileHead.ExtendBlock[BLEND_MESH_SAVED], SEEK_SET);
	//	LoadBlendMesh(pFile, &g_pBlendDebug);//&m_pBlendMesh);
	//	//fread(&BlockEnd,sizeof(DWORD),1,pFile);
	//}

	fread(&BlockEnd,sizeof(DWORD),1,pFile);
	fclose(pFile);
	wsprintf(m_szMeshFileName,"%s",pFileName);

	//////////////////////////////柔体/////////////////////////////////////////////
	for(int i_Flex=0;i_Flex<(int)m_vecStaticFlexibleBoneIndex.size();i_Flex++)
	{
		int nFlexBoneIndex = m_vecStaticFlexibleBoneIndex[i_Flex].Index;
		LPBONEINFO pInfo = &m_lpBoneInfo[nFlexBoneIndex];
	}
	///////////////////////////////////////////////////////////////////////////////////////
	//创建 XMesh 

	HRESULT hr = S_OK;

	DWORD FVF = D3DFVF_XYZ;
	if (pMeshHead[0].NormalBlock)
		FVF |= D3DFVF_NORMAL;
	if (pMeshHead[0].DiffuseBlock)
		FVF |= D3DFVF_DIFFUSE;

	if (pMeshHead[0].TextureUVW3Block)
		FVF |= D3DFVF_TEX3;
	else if (pMeshHead[0].TextureUVW2Block)
		FVF |= D3DFVF_TEX2;
	else if (pMeshHead[0].TextureUVW1Block)
		FVF |= D3DFVF_TEX1;
	if ( 1 )//!MeshFileHead.ExtendBlock[BLEND_MESH_SAVED] )
	{
		if(m_dNumFaces<=65535)
		{
			if (FAILED(hr = D3DXCreateMeshFVF(m_dNumFaces,m_dNumVertices,D3DXMESH_MANAGED,
				FVF,g_pd3dDevice,&m_pMeshSys)))
			{
				return hr;
			}
		}
		else
		{
			if (FAILED(hr = D3DXCreateMeshFVF(m_dNumFaces,m_dNumVertices,D3DXMESH_MANAGED|D3DXMESH_32BIT,
				FVF,g_pd3dDevice,&m_pMeshSys)))
			{
				return hr;
			}
		}


		switch(FVF)
		{
		case (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1):
			{
				VFormat::FACES_NORMAL_TEXTURE1 * pVers = NULL;
				WORD* pIndex = NULL;
				DWORD * pAttrib = NULL;
				if (FAILED(m_pMeshSys->LockVertexBuffer(0,(void**)&pVers)))
					return E_FAIL;
				if (FAILED(m_pMeshSys->LockIndexBuffer (0,(void**)&pIndex)))
					return E_FAIL;
				if (FAILED(m_pMeshSys->LockAttributeBuffer(0,(DWORD**)&pAttrib)))
					return E_FAIL;
				for(DWORD i=0;i<m_dNumVertices;i++)
				{
					pVers[i].p = m_lpVerticePosition[i];
					pVers[i].Normal = m_lpVerticeNormal[i];
					pVers[i].tu1 = m_lpVerticeTextCoord1[i].x;
					pVers[i].tv1 = m_lpVerticeTextCoord1[i].y;
				}
				if(m_dNumFaces<=65535)
				{
					for( i=0;i<m_dNumFaces;i++)
					{
						pIndex[i*3  ] = (WORD)m_lpFacesIndex[i*3  ];
						pIndex[i*3+1] = (WORD)m_lpFacesIndex[i*3+1];
						pIndex[i*3+2] = (WORD)m_lpFacesIndex[i*3+2];
						pAttrib[i] = m_lpSubsetID[i];
					}
				}
				else
				{
					for( i=0;i<m_dNumFaces;i++)
					{
						pIndex[i*3  ] = (WORD)m_lpFacesIndex[i*3  ];
						pIndex[i*3+1] = (WORD)m_lpFacesIndex[i*3+1];
						pIndex[i*3+2] = (WORD)m_lpFacesIndex[i*3+2];
						pAttrib[i] = m_lpSubsetID[i];
					}
				}

				D3DXComputeBoundingBox((D3DXVECTOR3*) pVers,m_dNumVertices,sizeof(VFormat::FACES_NORMAL_TEXTURE1),
					&m_BBox_A,&m_BBox_B);
				break;
			}
		case (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX2):
			{
				VFormat::FACES_NORMAL_TEXTURE2 * pVers = NULL;
				WORD* pIndex = NULL;
				DWORD * pAttrib = NULL;
				if (FAILED(m_pMeshSys->LockVertexBuffer(0,(void**)&pVers)))
					return E_FAIL;
				if (FAILED(m_pMeshSys->LockIndexBuffer (0,(void**)&pIndex)))
					return E_FAIL;
				if (FAILED(m_pMeshSys->LockAttributeBuffer(0,(DWORD**)&pAttrib)))
					return E_FAIL;
				for(DWORD i=0;i<m_dNumVertices;i++)
				{
					pVers[i].p = m_lpVerticePosition[i];
					pVers[i].Normal = m_lpVerticeNormal[i];
					pVers[i].tu1 = m_lpVerticeTextCoord1[i].x;
					pVers[i].tv1 = m_lpVerticeTextCoord1[i].y;
					pVers[i].tu2 = m_lpVerticeTextCoord2[i].x;
					pVers[i].tv2 = m_lpVerticeTextCoord2[i].y;
				}
				if(m_dNumFaces<=65535)
				{
					for( i=0;i<m_dNumFaces;i++)
					{
						pIndex[i*3  ] = (WORD)m_lpFacesIndex[i*3  ];
						pIndex[i*3+1] = (WORD)m_lpFacesIndex[i*3+1];
						pIndex[i*3+2] = (WORD)m_lpFacesIndex[i*3+2];
						pAttrib[i] = m_lpSubsetID[i];
					}
				}
				else
				{
					for( i=0;i<m_dNumFaces;i++)
					{
						pIndex[i*3  ] = (WORD)m_lpFacesIndex[i*3  ];
						pIndex[i*3+1] = (WORD)m_lpFacesIndex[i*3+1];
						pIndex[i*3+2] = (WORD)m_lpFacesIndex[i*3+2];
						pAttrib[i] = m_lpSubsetID[i];
					}
				}
				D3DXComputeBoundingBox((D3DXVECTOR3*) pVers,m_dNumVertices,sizeof(VFormat::FACES_NORMAL_TEXTURE2),
					&m_BBox_A,&m_BBox_B);
				break;
			}
		case (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX3):
			{
				VFormat::FACES_NORMAL_TEXTURE3 * pVers = NULL;
				WORD* pIndex = NULL;
				DWORD * pAttrib = NULL;
				if (FAILED(m_pMeshSys->LockVertexBuffer(0,(void**)&pVers)))
					return E_FAIL;
				if (FAILED(m_pMeshSys->LockIndexBuffer (0,(void**)&pIndex)))
					return E_FAIL;
				if (FAILED(m_pMeshSys->LockAttributeBuffer(0,(DWORD**)&pAttrib)))
					return E_FAIL;
				for(DWORD i=0;i<m_dNumVertices;i++)
				{
					pVers[i].p = m_lpVerticePosition[i];
					pVers[i].Normal = m_lpVerticeNormal[i];
					pVers[i].tu1 = m_lpVerticeTextCoord1[i].x;
					pVers[i].tv1 = m_lpVerticeTextCoord1[i].y;
					pVers[i].tu2 = m_lpVerticeTextCoord2[i].x;
					pVers[i].tv2 = m_lpVerticeTextCoord2[i].y;
					pVers[i].tu3 = m_lpVerticeTextCoord3[i].x;
					pVers[i].tv3 = m_lpVerticeTextCoord3[i].y;
				}
				if(m_dNumFaces<=65535)
				{
					for( i=0;i<m_dNumFaces;i++)
					{
						pIndex[i*3  ] = (WORD)m_lpFacesIndex[i*3  ];
						pIndex[i*3+1] = (WORD)m_lpFacesIndex[i*3+1];
						pIndex[i*3+2] = (WORD)m_lpFacesIndex[i*3+2];
						pAttrib[i] = m_lpSubsetID[i];
					}
				}
				else
				{
					for( i=0;i<m_dNumFaces;i++)
					{
						pIndex[i*3  ] = (WORD)m_lpFacesIndex[i*3  ];
						pIndex[i*3+1] = (WORD)m_lpFacesIndex[i*3+1];
						pIndex[i*3+2] = (WORD)m_lpFacesIndex[i*3+2];
						pAttrib[i] = m_lpSubsetID[i];
					}
				}
				D3DXComputeBoundingBox((D3DXVECTOR3*) pVers,m_dNumVertices,sizeof(VFormat::FACES_NORMAL_TEXTURE3),
					&m_BBox_A,&m_BBox_B);
				break;
			}

		case (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1):
			{
				VFormat::FACES_DIFFUSE_TEXTURE1 * pVers = NULL;
				WORD* pIndex = NULL;
				DWORD * pAttrib = NULL;
				if (FAILED(m_pMeshSys->LockVertexBuffer(0,(void**)&pVers)))
					return E_FAIL;
				if (FAILED(m_pMeshSys->LockIndexBuffer (0,(void**)&pIndex)))
					return E_FAIL;
				if (FAILED(m_pMeshSys->LockAttributeBuffer(0,(DWORD**)&pAttrib)))
					return E_FAIL;
				for(DWORD i=0;i<m_dNumVertices;i++)
				{
					pVers[i].p = m_lpVerticePosition[i];
					pVers[i].diffuse = m_lpVerticeDiffuse[i];
					pVers[i].tu1 = m_lpVerticeTextCoord1[i].x;
					pVers[i].tv1 = m_lpVerticeTextCoord1[i].y;
				}
				if(m_dNumFaces<=65535)
				{
					for( i=0;i<m_dNumFaces;i++)
					{
						pIndex[i*3  ] = (WORD)m_lpFacesIndex[i*3  ];
						pIndex[i*3+1] = (WORD)m_lpFacesIndex[i*3+1];
						pIndex[i*3+2] = (WORD)m_lpFacesIndex[i*3+2];
						pAttrib[i] = m_lpSubsetID[i];
					}
				}
				else
				{
					for( i=0;i<m_dNumFaces;i++)
					{
						pIndex[i*3  ] = (WORD)m_lpFacesIndex[i*3  ];
						pIndex[i*3+1] = (WORD)m_lpFacesIndex[i*3+1];
						pIndex[i*3+2] = (WORD)m_lpFacesIndex[i*3+2];
						pAttrib[i] = m_lpSubsetID[i];
					}
				}
				D3DXComputeBoundingBox((D3DXVECTOR3*) pVers,m_dNumVertices,sizeof(VFormat::FACES_DIFFUSE_TEXTURE1),
					&m_BBox_A,&m_BBox_B);
				break;
			}
		case (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX2):
			{
				VFormat::FACES_DIFFUSE_TEXTURE2 * pVers = NULL;
				WORD* pIndex = NULL;
				DWORD * pAttrib = NULL;
				if (FAILED(m_pMeshSys->LockVertexBuffer(0,(void**)&pVers)))
					return E_FAIL;
				if (FAILED(m_pMeshSys->LockIndexBuffer (0,(void**)&pIndex)))
					return E_FAIL;
				if (FAILED(m_pMeshSys->LockAttributeBuffer(0,(DWORD**)&pAttrib)))
					return E_FAIL;
				for(DWORD i=0;i<m_dNumVertices;i++)
				{
					pVers[i].p = m_lpVerticePosition[i];
					pVers[i].diffuse = m_lpVerticeDiffuse[i];
					pVers[i].tu1 = m_lpVerticeTextCoord1[i].x;
					pVers[i].tv1 = m_lpVerticeTextCoord1[i].y;
					pVers[i].tu2 = m_lpVerticeTextCoord2[i].x;
					pVers[i].tv2 = m_lpVerticeTextCoord2[i].y;
				}
				if(m_dNumFaces<=65535)
				{
					for( i=0;i<m_dNumFaces;i++)
					{
						pIndex[i*3  ] = (WORD)m_lpFacesIndex[i*3  ];
						pIndex[i*3+1] = (WORD)m_lpFacesIndex[i*3+1];
						pIndex[i*3+2] = (WORD)m_lpFacesIndex[i*3+2];
						pAttrib[i] = m_lpSubsetID[i];
					}
				}
				else
				{
					for( i=0;i<m_dNumFaces;i++)
					{
						pIndex[i*3  ] = (WORD)m_lpFacesIndex[i*3  ];
						pIndex[i*3+1] = (WORD)m_lpFacesIndex[i*3+1];
						pIndex[i*3+2] = (WORD)m_lpFacesIndex[i*3+2];
						pAttrib[i] = m_lpSubsetID[i];
					}
				}
				D3DXComputeBoundingBox((D3DXVECTOR3*) pVers,m_dNumVertices,sizeof(VFormat::FACES_DIFFUSE_TEXTURE2),
					&m_BBox_A,&m_BBox_B);
				break;
			}
		case (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX3):
			{
				VFormat::FACES_DIFFUSE_TEXTURE3 * pVers = NULL;
				WORD* pIndex = NULL;
				DWORD * pAttrib = NULL;
				if (FAILED(m_pMeshSys->LockVertexBuffer(0,(void**)&pVers)))
					return E_FAIL;
				if (FAILED(m_pMeshSys->LockIndexBuffer (0,(void**)&pIndex)))
					return E_FAIL;
				if (FAILED(m_pMeshSys->LockAttributeBuffer(0,(DWORD**)&pAttrib)))
					return E_FAIL;
				for(DWORD i=0;i<m_dNumVertices;i++)
				{
					pVers[i].p = m_lpVerticePosition[i];
					pVers[i].diffuse = m_lpVerticeDiffuse[i];
					pVers[i].tu1 = m_lpVerticeTextCoord1[i].x;
					pVers[i].tv1 = m_lpVerticeTextCoord1[i].y;
					pVers[i].tu2 = m_lpVerticeTextCoord2[i].x;
					pVers[i].tv2 = m_lpVerticeTextCoord2[i].y;
					pVers[i].tu3 = m_lpVerticeTextCoord3[i].x;
					pVers[i].tv3 = m_lpVerticeTextCoord3[i].y;
				}
				if(m_dNumFaces<=65535)
				{
					for( i=0;i<m_dNumFaces;i++)
					{
						pIndex[i*3  ] = (WORD)m_lpFacesIndex[i*3  ];
						pIndex[i*3+1] = (WORD)m_lpFacesIndex[i*3+1];
						pIndex[i*3+2] = (WORD)m_lpFacesIndex[i*3+2];
						pAttrib[i] = m_lpSubsetID[i];
					}
				}
				else
				{
					for( i=0;i<m_dNumFaces;i++)
					{
						pIndex[i*3  ] = (WORD)m_lpFacesIndex[i*3  ];
						pIndex[i*3+1] = (WORD)m_lpFacesIndex[i*3+1];
						pIndex[i*3+2] = (WORD)m_lpFacesIndex[i*3+2];
						pAttrib[i] = m_lpSubsetID[i];
					}
				}
				D3DXComputeBoundingBox((D3DXVECTOR3*) pVers,m_dNumVertices,sizeof(VFormat::FACES_DIFFUSE_TEXTURE3),
					&m_BBox_A,&m_BBox_B);
				break;
			}
		case (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX1):
			{
				VFormat::FACES_NORMAL_DIFFUSE_TEXTURE1 * pVers = NULL;
				WORD* pIndex = NULL;
				DWORD * pAttrib = NULL;
				if (FAILED(m_pMeshSys->LockVertexBuffer(0,(void**)&pVers)))
					return E_FAIL;
				if (FAILED(m_pMeshSys->LockIndexBuffer (0,(void**)&pIndex)))
					return E_FAIL;
				if (FAILED(m_pMeshSys->LockAttributeBuffer(0,(DWORD**)&pAttrib)))
					return E_FAIL;
				for(DWORD i=0;i<m_dNumVertices;i++)
				{
					pVers[i].p = m_lpVerticePosition[i];
					pVers[i].Normal = m_lpVerticeNormal[i];
					pVers[i].diffuse = m_lpVerticeDiffuse[i];
					pVers[i].tu1 = m_lpVerticeTextCoord1[i].x;
					pVers[i].tv1 = m_lpVerticeTextCoord1[i].y;
				}
				if(m_dNumFaces<=65535)
				{
					for( i=0;i<m_dNumFaces;i++)
					{
						pIndex[i*3  ] = (WORD)m_lpFacesIndex[i*3  ];
						pIndex[i*3+1] = (WORD)m_lpFacesIndex[i*3+1];
						pIndex[i*3+2] = (WORD)m_lpFacesIndex[i*3+2];
						pAttrib[i] = m_lpSubsetID[i];
					}
				}
				else
				{
					for( i=0;i<m_dNumFaces;i++)
					{
						pIndex[i*3  ] = (WORD)m_lpFacesIndex[i*3  ];
						pIndex[i*3+1] = (WORD)m_lpFacesIndex[i*3+1];
						pIndex[i*3+2] = (WORD)m_lpFacesIndex[i*3+2];
						pAttrib[i] = m_lpSubsetID[i];
					}
				}
				D3DXComputeBoundingBox((D3DXVECTOR3*) pVers,m_dNumVertices,sizeof(VFormat::FACES_NORMAL_DIFFUSE_TEXTURE1),
					&m_BBox_A,&m_BBox_B);
				break;
			}
		case (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX2):
			{
				VFormat::FACES_NORMAL_DIFFUSE_TEXTURE2 * pVers = NULL;
				WORD* pIndex = NULL;
				DWORD * pAttrib = NULL;
				if (FAILED(m_pMeshSys->LockVertexBuffer(0,(void**)&pVers)))
					return E_FAIL;
				if (FAILED(m_pMeshSys->LockIndexBuffer (0,(void**)&pIndex)))
					return E_FAIL;
				if (FAILED(m_pMeshSys->LockAttributeBuffer(0,(DWORD**)&pAttrib)))
					return E_FAIL;
				for(DWORD i=0;i<m_dNumVertices;i++)
				{
					pVers[i].p = m_lpVerticePosition[i];
					pVers[i].Normal = m_lpVerticeNormal[i];
					pVers[i].diffuse = m_lpVerticeDiffuse[i];
					pVers[i].tu1 = m_lpVerticeTextCoord1[i].x;
					pVers[i].tv1 = m_lpVerticeTextCoord1[i].y;
					pVers[i].tu2 = m_lpVerticeTextCoord2[i].x;
					pVers[i].tv2 = m_lpVerticeTextCoord2[i].y;
				}
				if(m_dNumFaces<=65535)
				{
					for( i=0;i<m_dNumFaces;i++)
					{
						pIndex[i*3  ] = (WORD)m_lpFacesIndex[i*3  ];
						pIndex[i*3+1] = (WORD)m_lpFacesIndex[i*3+1];
						pIndex[i*3+2] = (WORD)m_lpFacesIndex[i*3+2];
						pAttrib[i] = m_lpSubsetID[i];
					}
				}
				else
				{
					for( i=0;i<m_dNumFaces;i++)
					{
						pIndex[i*3  ] =  (WORD)m_lpFacesIndex[i*3  ];
						pIndex[i*3+1] =  (WORD)m_lpFacesIndex[i*3+1];
						pIndex[i*3+2] =  (WORD)m_lpFacesIndex[i*3+2];
						pAttrib[i] = m_lpSubsetID[i];
					}
				}
				D3DXComputeBoundingBox((D3DXVECTOR3*) pVers,m_dNumVertices,sizeof(VFormat::FACES_NORMAL_DIFFUSE_TEXTURE2),
					&m_BBox_A,&m_BBox_B);
				break;
			}
		case (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX3):
			{
				VFormat::FACES_NORMAL_DIFFUSE_TEXTURE3 * pVers = NULL;
				WORD* pIndex = NULL;
				DWORD * pAttrib = NULL;
				if (FAILED(m_pMeshSys->LockVertexBuffer(0,(void**)&pVers)))
					return E_FAIL;
				if (FAILED(m_pMeshSys->LockIndexBuffer (0,(void**)&pIndex)))
					return E_FAIL;
				if (FAILED(m_pMeshSys->LockAttributeBuffer(0,(DWORD**)&pAttrib)))
					return E_FAIL;
				for(DWORD i=0;i<m_dNumVertices;i++)
				{
					pVers[i].p = m_lpVerticePosition[i];
					pVers[i].Normal = m_lpVerticeNormal[i];
					pVers[i].diffuse = m_lpVerticeDiffuse[i];
					pVers[i].tu1 = m_lpVerticeTextCoord1[i].x;
					pVers[i].tv1 = m_lpVerticeTextCoord1[i].y;
					pVers[i].tu2 = m_lpVerticeTextCoord2[i].x;
					pVers[i].tv2 = m_lpVerticeTextCoord2[i].y;
					pVers[i].tu3 = m_lpVerticeTextCoord3[i].x;
					pVers[i].tv3 = m_lpVerticeTextCoord3[i].y;
				}
				if(m_dNumFaces<=65535)
				{
					for( i=0;i<m_dNumFaces;i++)
					{
						pIndex[i*3  ] = (WORD)m_lpFacesIndex[i*3  ];
						pIndex[i*3+1] = (WORD)m_lpFacesIndex[i*3+1];
						pIndex[i*3+2] = (WORD)m_lpFacesIndex[i*3+2];
						pAttrib[i] = m_lpSubsetID[i];
					}
				}
				else
				{
					for( i=0;i<m_dNumFaces;i++)
					{
						pIndex[i*3  ] =  (WORD)m_lpFacesIndex[i*3  ];
						pIndex[i*3+1] =  (WORD)m_lpFacesIndex[i*3+1];
						pIndex[i*3+2] =  (WORD)m_lpFacesIndex[i*3+2];
						pAttrib[i] = m_lpSubsetID[i];
					}
				}
				D3DXComputeBoundingBox((D3DXVECTOR3*) pVers,m_dNumVertices,sizeof(VFormat::FACES_NORMAL_DIFFUSE_TEXTURE3),
					&m_BBox_A,&m_BBox_B);
				break;
			}
		}

		if (FAILED(m_pMeshSys->UnlockVertexBuffer()))
			return E_FAIL;
		if (FAILED(m_pMeshSys->UnlockIndexBuffer()))
			return E_FAIL;
		if (FAILED(m_pMeshSys->UnlockAttributeBuffer()))
			return E_FAIL;
	}
	
	m_dNumMaterial = m_dNumSubsets;
	m_lpMaterial = new MATERIAL[m_dNumMaterial];
	ZeroMemory(m_lpMaterial,sizeof(MATERIAL)*m_dNumMaterial);

	DWORD Def_Option = MATERIAL_OPTION_ZBUFFER_TRUE|
		MATERIAL_OPTION_FILL_SOLID|
		MATERIAL_OPTION_SHADE_GOURAUD|
		MATERIAL_OPTION_CULL_CW|
		MATERIAL_OPTION_SPECULARENABLE|
		MATERIAL_OPTION_LIGHTING;

	for(DWORD i=0;i<m_dNumMaterial;i++)
	{
		m_lpMaterial[i].m_sMaterial9.Diffuse.r = 0.7f ;
		m_lpMaterial[i].m_sMaterial9.Diffuse.g = 0.7f ;
		m_lpMaterial[i].m_sMaterial9.Diffuse.b = 0.7f ;
		m_lpMaterial[i].m_sMaterial9.Diffuse.a = 1.0f ;
		m_lpMaterial[i].m_sMaterial9.Ambient = m_lpMaterial[i].m_sMaterial9.Diffuse;
		m_lpMaterial[i].m_sMaterial9.Specular = m_lpMaterial[i].m_sMaterial9.Diffuse;
		m_lpMaterial[i].m_sMaterial9.Power = 15;
		m_lpMaterial[i].m_dOption = Def_Option;
	}

	m_lpStaticVertexPos = m_lpVerticePosition;
	//m_bRecaculateNormal = false;
	if ((FVF&D3DFVF_NORMAL) && m_bRecaculateNormal)
		D3DXComputeNormals(m_pMeshSys,NULL);
	/////////////////////////////////////////////////////////////////////////////////////
	//load shader for test
	//LoadShader();

	if (!m_bSkined)
	{
		//CreateProgressMesh();
		OptimizeUnSkinedMesh();
	}

	SAFE_DELETE_ARRAY(pMeshHead);
	SAFE_DELETE_ARRAY(m_lpVerticePosition);
	SAFE_DELETE_ARRAY(m_lpVerticeNormal);
	SAFE_DELETE_ARRAY(m_lpVerticeTextCoord1);
	SAFE_DELETE_ARRAY(m_lpVerticeTextCoord2);
	SAFE_DELETE_ARRAY(m_lpVerticeTextCoord3);
	SAFE_DELETE_ARRAY(m_lpFacesIndex);
	SAFE_DELETE_ARRAY(m_lpSubsetID);

	if (m_bSkined)
	{
		CreateSkinInfo(0);//MeshFileHead.ExtendBlock[BLEND_MESH_SAVED]);
		//CreateProgressMesh();
		/*if ( !MeshFileHead.ExtendBlock[BLEND_MESH_SAVED] )
		{
			TCHAR strNewFileName[255];
			strcpy(strNewFileName, m_szMeshFileName);
			strNewFileName[strlen(strNewFileName) - 5] = '\0';
			strcat(strNewFileName, "_New.Mesh");
			
			FILE* fpNew = fopen(strNewFileName, "wb");
			long lStartPos = ftell(fpNew);
			SaveMesh(fpNew);
			fclose(fpNew);
		}*/
	}

	//////////////////////////////////////////////////////////////////////////
	//Test Save Blend mesh
//if ( g_pBlendDebug )
//{
//	DWORD dwAttTableSize = 0;	
//	DWORD dwAttTableSize1 = 0;	
//	g_pBlendDebug->GetAttributeTable(NULL, &dwAttTableSize);
//	D3DXATTRIBUTERANGE* pAttRange = new D3DXATTRIBUTERANGE[dwAttTableSize];
//	g_pBlendDebug->GetAttributeTable(pAttRange, &dwAttTableSize);
//	FILE* fpdebug000 = fopen("d:\\attrange.txt", "w");
//	LPDIRECT3DINDEXBUFFER9 pIB = NULL;
//	LPDIRECT3DVERTEXBUFFER9 pVB = NULL;
//	for( unsigned uindex = 0; uindex < dwAttTableSize; uindex++ )
//	{
//		fwrite(&pAttRange[uindex], sizeof(D3DXATTRIBUTERANGE), 1, fpdebug000);
//		fputs("======================\n", fpdebug000);
//	}
//	g_pBlendDebug->GetIndexBuffer(&pIB);
//	g_pBlendDebug->GetVertexBuffer(&pVB);
//	D3DVERTEXBUFFER_DESC vb_desc;
//	D3DINDEXBUFFER_DESC ib_desc;
//	pIB->GetDesc(&ib_desc);
//	pVB->GetDesc(&vb_desc);
//
//	m_pBlendMesh->GetAttributeTable(NULL, &dwAttTableSize1);
//	D3DXATTRIBUTERANGE* pAttRange1 = new D3DXATTRIBUTERANGE[dwAttTableSize1];
//	m_pBlendMesh->GetAttributeTable(pAttRange1, &dwAttTableSize1);
//	LPDIRECT3DINDEXBUFFER9 pIB1 = NULL;
//	LPDIRECT3DVERTEXBUFFER9 pVB1 = NULL;
//	m_pBlendMesh->GetIndexBuffer(&pIB1);
//	m_pBlendMesh->GetVertexBuffer(&pVB1);
//	D3DVERTEXBUFFER_DESC vb_desc1;
//	D3DINDEXBUFFER_DESC ib_desc1;
//	pIB1->GetDesc(&ib_desc1);
//	pVB1->GetDesc(&vb_desc1);
//
//	
//	LPD3DXBUFFER pBuffer = NULL;
//	DWORD time = timeGetTime();
//	D3DXCreateBuffer(g_pBlendDebug->GetNumFaces()*3*sizeof(DWORD), &pBuffer);
//	g_pBlendDebug->GenerateAdjacency(0.0f, (DWORD*)pBuffer->GetBufferPointer());
//
//	DWORD AdjTime = timeGetTime() - time;
//	g_pBlendDebug->OptimizeInplace(D3DXMESHOPT_ATTRSORT, (DWORD*)pBuffer->GetBufferPointer(), NULL, NULL, NULL);
//	DWORD optTime = timeGetTime() - time;
//	DWORD dwFace = g_pBlendDebug->GetNumFaces();
//	DWORD dwFace1 = m_pBlendMesh->GetNumFaces();
//
//	g_pBlendDebug->GetAttributeTable(NULL, &dwAttTableSize);
//	SAFE_DELETE_ARRAY(pAttRange);
//	pAttRange = new D3DXATTRIBUTERANGE[dwAttTableSize];
//	g_pBlendDebug->GetAttributeTable(pAttRange, &dwAttTableSize);
//	fclose(fpdebug000);
//	//fputs("new mesh here\n", fpdebug000);
//	fpdebug000 = fopen("d:\\attrange1.txt", "w");
//	for( unsigned int uindex = 0; uindex < dwAttTableSize; uindex++ )
//	{
//		fwrite(&pAttRange[uindex], sizeof(D3DXATTRIBUTERANGE), 1, fpdebug000);
//		fputs("======================\n", fpdebug000);
//	}
//	int nFordebug1111 = 0;
//	BYTE* pIndex1 = NULL;
//	m_pBlendMesh->LockVertexBuffer(D3DLOCK_READONLY, (LPVOID*)&pIndex1);
//
//	BYTE* pIndex = NULL;
//	g_pBlendDebug->LockVertexBuffer(D3DLOCK_READONLY, (LPVOID*)&pIndex);
//
//	if ( !memcmp(pIndex1, pIndex, vb_desc.Size) )
//	{
//		int nFordebug222 = 100;
//	}
//	/*for( DWORD dwIndex = 0; dwIndex < dwFace * 3; dwIndex++ )
//	{
//		if ( pIndex1[dwIndex] != pIndex[dwIndex] )
//		{
//			int nFordeubg222  =0 ;
//		}
//	}*/
//	m_pBlendMesh->UnlockAttributeBuffer();
//	g_pBlendDebug->UnlockAttributeBuffer();
//	SAFE_DELETE_ARRAY(pAttRange);
//	SAFE_DELETE_ARRAY(pAttRange1);
//	fclose(fpdebug000);
//
//}
	//Test Save Blend Mesh end
	return S_OK;
}


HRESULT KModel::LoadMaterial(LPSTR pFileName)
{
	DWORD Mask = 0;//MATL对应的ASECC代码
	DWORD BlockEnd  = 0xFFFFFFFF;  
	DWORD BlockLength = 0;
	FILE* pFile = fopen(pFileName,"rb");
	if (pFile==NULL) return E_FAIL;

	fread(&Mask,sizeof(DWORD),1,pFile);
	if (Mask!=0x4D41544C) return E_FAIL;
	//读取块长度
	fread(&BlockLength,sizeof(DWORD),1,pFile);
	DWORD m_dNumSubsets = 0;
	fread(&m_dNumSubsets,sizeof(DWORD),1,pFile); //子部分数目

	SAFE_DELETE_ARRAY(m_lpMaterial);
	m_lpMaterial = new MATERIAL[m_dNumMaterial];
	/////////////////////////////////////////////////////////////////////
	//把读出的材质信息转换到现有材质中,如果文件中的材质数大于现有材质的数目
	//只取前面的材质

	for(DWORD i_Sub=0;i_Sub<m_dNumSubsets;i_Sub++)
	{
		if (i_Sub>=m_dNumMaterial)
			break;

		LPMATERIAL pMaterial = &m_lpMaterial[i_Sub]; 
		fread(&pMaterial->m_sMaterial9.Ambient, sizeof(D3DCOLORVALUE),1,pFile); 
		fread(&pMaterial->m_sMaterial9.Diffuse, sizeof(D3DCOLORVALUE),1,pFile); 
		fread(&pMaterial->m_sMaterial9.Specular,sizeof(D3DCOLORVALUE),1,pFile); 
		fread(&pMaterial->m_sMaterial9.Emissive,sizeof(D3DCOLORVALUE),1,pFile); 
		fread(&pMaterial->m_sMaterial9.Power,sizeof(float),1,pFile);

		if ((pMaterial->m_sMaterial9.Power<=0)||(pMaterial->m_sMaterial9.Power>100))
			pMaterial->m_sMaterial9.Power = 15.0f;
		fread(&pMaterial->m_dOption,sizeof(DWORD),1,pFile);//材质选项

		pMaterial->m_dOption |= MATERIAL_OPTION_FILL_SOLID;

		DWORD NumMaterialOption = 0;
		fread(&NumMaterialOption,sizeof(DWORD),1,pFile);////选项数目

		for(DWORD i_MOp=0;i_MOp<NumMaterialOption;i_MOp++)
		{
			LPOPTION pOption = new OPTION;
			LoadOption(pOption,pFile,pMaterial);
			if(pOption->Type!=OPTION_MATERIAL_MSEFFECT)
			{
				pMaterial->m_lpMatOptions.push_back(pOption);
			}
			else
			{
				SAFE_DELETE(pOption);
			}
		}
		pMaterial->m_dwNumOptions = pMaterial->m_lpMatOptions.size();

		DWORD NumTexture = 0;
		fread(&NumTexture,sizeof(DWORD),1,pFile);//帖图数目
		DWORD KK =0;
		pMaterial->m_dTextureIDs[0] =0;
		pMaterial->m_dTextureIDs[1] =0;
		pMaterial->m_dTextureIDs[2] =0;
		pMaterial->m_dTextureIDs[3] =0;
		pMaterial->m_dTextureIDs[4] =0;
		pMaterial->m_dTextureIDs[5] =0;
		pMaterial->m_dTextureIDs[6] =0;
		pMaterial->m_dTextureIDs[7] =0;
		for(DWORD i_Text=0;i_Text<NumTexture;i_Text++)
		{
			float Amount = 0;
			TCHAR TextureType[20];
			TCHAR FileName[256];
			fread(&Amount,sizeof(float),1,pFile);
			fread( TextureType,sizeof(TCHAR),20,pFile);
			fread( FileName,sizeof(TCHAR),256,pFile);
			DWORD NumTextureOption =0;
			fread(&NumTextureOption,sizeof(DWORD),1,pFile);//帖图选项数目
			pMaterial->m_lpTextureNumOption[KK] = NumTextureOption;

			//pMaterial->m_lpTextureOptions[KK];
			TCHAR path_buffer[256];
			TCHAR drive[256];
			TCHAR dir[256];
			TCHAR fname[256];
			TCHAR ext[256];

			wsprintf(path_buffer,"%s",FileName);
			_splitpath( path_buffer, drive, dir, fname, ext );
			wsprintf(FileName,"%s%s%s%s",drive,dir,fname,ext);

			for(DWORD i_TOption=0;i_TOption<NumTextureOption;i_TOption++)
			{
				LPOPTION pNewOption = new OPTION;
				LoadOption(pNewOption,pFile,pMaterial);
				if(pNewOption->Type!=OPTION_MATERIAL_MSEFFECT)
				{
					pMaterial->m_lpTextureOptions[KK].push_back(pNewOption);
				}
				else
				{
					SAFE_DELETE(pNewOption);
				}
			}
			//////////////////////////////////////////////////////////////////////////
			//
			if (_stricmp(FileName,"")!=0)
			{
				pMaterial->m_scTextureName[KK] = FileName;
			}

			if (_stricmp(TextureType,"")!=0)
			{
				pMaterial->m_scTextureType[KK] = TextureType;
			}

			if (_stricmp(TextureType,"*MAP_OPACITY")!=0)
			{
				DWORD TextureID = 0;
				if (_stricmp(FileName,"")!=0)
				{
					TCHAR TextureName[256];
					wsprintf(TextureName,"%s%s",fname,ext);

					if (SUCCEEDED(g_cTextureTable.LoadFromFile(&TextureID,TextureName)))
					{
						pMaterial->m_dTextureIDs[KK] = TextureID;
						KK++;
					}
					else
					{
						pMaterial->m_dTextureIDs[KK] = 0;
						//MessageBox(g_hBaseWnd,FileName,"Load Failed!",0);
					}
					/*else
					{
					TCHAR str[256];
					TCHAR path_buffer[256];
					TCHAR drive[256];
					TCHAR dir[256];
					TCHAR fname[256];
					TCHAR ext[256];

					wsprintf(path_buffer,"%s",FileName);
					_splitpath( path_buffer, drive, dir, fname, ext );

					wsprintf(str,"Textures\\%s%s",fname,ext);
					if (SUCCEEDED(g_cTextureTable.LoadFromFile(&TextureID,str)))
					{
					pMaterial->m_dTextureIDs[KK] = TextureID;
					KK++;
					}
					else
					{
					pMaterial->m_dTextureIDs[KK] = 0;
					MessageBox(g_hBaseWnd,FileName,"Load Failed!",0);
					}
					}*/
				}	
			}
		}
	}	
	fread(&BlockEnd,sizeof(DWORD),1,pFile);

	fclose(pFile);
	wsprintf(m_szMtlFileName,"%s",pFileName);

	//For PRT set up the texture list
	PRT_GetAlbedoTextures();
	return S_OK;
}

int KModel::FindBone(LPCSTR pBoneName)
{
	if ((m_dOption|MESH_SKINED)&&(m_lpBoneInfo!=NULL))
	{
		for(DWORD i_Bone=0;i_Bone<m_dNumBone;i_Bone++)
		{
			LPBONEINFO pInfo = &m_lpBoneInfo[i_Bone];
			if (strcmp(pBoneName,pInfo->scName.c_str())==0)
				return i_Bone;
		}
		return -1;
	}
	return -1;
}

int KModel::FindSocket(LPCSTR pSocketName)
{
	for(DWORD i_Socket=0;i_Socket<m_dNumSocket;i_Socket++)
	{
		LPSOCKET pSocket = &m_lpSockets[i_Socket];
		if (strcmp(pSocketName,pSocket->scName.c_str())==0)
			return i_Socket;
	}
	return -1;
}

int KModel::FindAnimation(LPCSTR pAnimationName)
{
	for(DWORD i_Animation=0;i_Animation<m_dNumAnimation;i_Animation++)
	{
		LPANIMATION pAni = m_lpAnimations[i_Animation];
		if (strcmp(pAnimationName,pAni->Name)==0)
			return i_Animation;
	}
	return -1;
}

HRESULT KModel::DeleteAnimation(LPSTR pAnimationName)
{
	int AniID = FindAnimation(pAnimationName);
	if (AniID<0)
		return E_FAIL;
	LPANIMATION pAni = m_lpAnimations[AniID];
	m_lpAnimations.erase(&m_lpAnimations[AniID]);

	SAFE_DELETE(pAni);

	m_dNumAnimation --;
	m_nCurAnimationIndex = -1;

	return S_OK;
}

HRESULT KModel::LoadAnimation(LPSTR pFileName)
{
	DWORD Mask = 0;
	DWORD BlockEnd  = 0xFFFFFFFF;  
	DWORD BlockLength = 0;
	DWORD NumAnimation = 0;
	FILE* pFile = fopen(pFileName,"rb");
	if (pFile==NULL) return E_FAIL;
	fread(&Mask,sizeof(DWORD),1,pFile);
	if (Mask!=0x414E494D)//ANIM对应的ASECC代码
	{
		fclose(pFile);
		return E_FAIL;
	}
	fread(&BlockLength, sizeof(DWORD),1,pFile);
	fread(&NumAnimation,sizeof(DWORD),1,pFile);                  //动画数目，这里默认为１

	for(DWORD i_Ani=0;i_Ani<NumAnimation;i_Ani++)
	{
		LPANIMATION pAni = new ANIMATION;

		pAni->Name = new TCHAR[30];

		DWORD AnimationType = 0;
		fread(&AnimationType,sizeof(DWORD),1,pFile);
		fread( pAni->Name,sizeof(TCHAR),30,pFile);

		/////////////////////////////////////////////////////////////////////////////////////
		if (AnimationType==ANIMATION_BONE)
		{
			fread(&pAni->NumAnimatedBone,sizeof(DWORD),1,pFile);            //该动画中包含的骨骼数目
			fread(&pAni->NumFrames,sizeof(DWORD),1,pFile);                  //该动画中包含的桢数目
			fread(&pAni->FrameLength,sizeof(float),1,pFile);                //该动画中包含的每桢之间的时间间隔

			pAni->AnimatedBoneName = new 
				LPSTR[pAni->NumAnimatedBone];
			ZeroMemory(pAni->AnimatedBoneName,
				sizeof(LPSTR)*pAni->NumAnimatedBone);
			pAni->AnimatedBoneMatrix = new 
				D3DXMATRIX[pAni->NumFrames*pAni->NumAnimatedBone];
			ZeroMemory(pAni->AnimatedBoneMatrix,
				sizeof(D3DXMATRIX)*pAni->NumFrames*pAni->NumAnimatedBone);

			for(DWORD i_Bone =0;i_Bone < pAni->NumAnimatedBone;i_Bone++)
			{
				pAni->AnimatedBoneName[i_Bone] = new TCHAR[30];
				fread(pAni->AnimatedBoneName[i_Bone],sizeof(TCHAR),30,pFile); 
			}		
			fread(pAni->AnimatedBoneMatrix,sizeof(D3DXMATRIX)*pAni->NumFrames*pAni->NumAnimatedBone,1,pFile);//输出骨骼变换数目
			pAni->Type = ANIMATION_BONE;
		}
		if (AnimationType==ANIMATION_BONE_RELATIVE)
		{
			fread(&pAni->NumAnimatedBone,sizeof(DWORD),1,pFile);            //该动画中包含的骨骼数目
			fread(&pAni->NumFrames,sizeof(DWORD),1,pFile);                  //该动画中包含的桢数目
			fread(&pAni->FrameLength,sizeof(float),1,pFile);                //该动画中包含的每桢之间的时间间隔

			pAni->AnimatedBoneName = new 
				LPSTR[pAni->NumAnimatedBone];
			ZeroMemory(pAni->AnimatedBoneName,
				sizeof(LPSTR)*pAni->NumAnimatedBone);

			pAni->AnimatedBoneMatrix = new 
				D3DXMATRIX[pAni->NumFrames*pAni->NumAnimatedBone];
			ZeroMemory(pAni->AnimatedBoneMatrix,
				sizeof(D3DXMATRIX)*pAni->NumFrames*pAni->NumAnimatedBone);

			DWORD Size = pAni->NumFrames*pAni->NumAnimatedBone;

			//pAni->BoneScaling     = new D3DXVECTOR3[Size];
			//pAni->BoneRotation    = new D3DXQUATERNION[Size];
			//pAni->BoneTranslation = new D3DXVECTOR3[Size];

			for(DWORD i_Bone =0;i_Bone < pAni->NumAnimatedBone;i_Bone++)
			{
				pAni->AnimatedBoneName[i_Bone] = new TCHAR[30];
				fread(pAni->AnimatedBoneName[i_Bone],sizeof(TCHAR),30,pFile); 
			}	

			fread(pAni->AnimatedBoneMatrix,sizeof(D3DXMATRIX)*pAni->NumFrames*pAni->NumAnimatedBone,1,pFile);//输出骨骼变换数目

			//FILE* fp = fopen("d:\\debug_Bone_Trans_Related.txt", "w+");
			/*for (int i = 0; i < pAni->NumAnimatedBone; i++ )
			{
				fprintf(fp, "Bone Offset: %d\n", i * pAni->NumFrames);
				printMat(pAni->AnimatedBoneMatrix[i * pAni->NumFrames], fp);	
				fprintf(fp, "================================\n");
			}
			fclose(fp);*/
			/*for(i_Bone =0;i_Bone < pAni->NumAnimatedBone;i_Bone++)
			{
			LPSTR Name = pAni->AnimatedBoneName[i_Bone];
			int BoneIndex = FindBone(Name);
			if ((BoneIndex>=0)&&((DWORD)BoneIndex<m_dNumBone))
			{
			LPBONEINFO pInfo = &m_lpBoneInfo[BoneIndex];
			D3DXMATRIX InvRot;
			D3DXMatrixInverse(&InvRot,NULL,&pInfo->BaseRotation);

			for(DWORD i_Frame=0;i_Frame<pAni->NumFrames;i_Frame++)
			{
			int Index = i_Bone*pAni->NumFrames + i_Frame;

			D3DXMATRIX*     pSrcMat   = &pAni->AnimatedBoneMatrix[Index];
			D3DXVECTOR3*    pDestScal = &pAni->BoneScaling       [Index];
			D3DXQUATERNION* pDestRot  = &pAni->BoneRotation      [Index];
			D3DXVECTOR3*    pDestPos  = &pAni->BoneTranslation   [Index];
			D3DXMATRIX      MatTemp;

			MatrixExract(*pDestScal,*pDestPos,MatTemp,pSrcMat);

			//if (pInfo->BaseBoneLength>=0.00001f)
			(*pDestPos) -= pInfo->BaseBonePosition;

			//if (pInfo->BaseBoneScale>=0.00001f)
			(*pDestScal).x /= pInfo->BaseBoneScale.x;
			(*pDestScal).y /= pInfo->BaseBoneScale.y;
			(*pDestScal).z /= pInfo->BaseBoneScale.z;

			(*pSrcMat) = InvRot * MatTemp;						
			}
			}
			}*/

			/*fread(pAni->BoneScaling,    sizeof(D3DXVECTOR3 )   *Size,1,pFile);//输出骨骼变换数目
			fread(pAni->BoneRotation,   sizeof(D3DXQUATERNION )*Size,1,pFile);//输出骨骼变换数目
			fread(pAni->BoneTranslation,sizeof(D3DXVECTOR3 )   *Size,1,pFile);//输出骨骼变换数目
			*/

			pAni->Type = ANIMATION_BONE_RELATIVE;
		}
		else if ( AnimationType == ANIMATION_BONE_RTS )
		{
			fread(&pAni->NumAnimatedBone,sizeof(DWORD),1,pFile);            //该动画中包含的骨骼数目
			fread(&pAni->NumFrames,sizeof(DWORD),1,pFile);                  //该动画中包含的桢数目
			fread(&pAni->FrameLength,sizeof(float),1,pFile);                //该动画中包含的每桢之间的时间间隔

			pAni->AnimatedBoneName = new LPSTR[pAni->NumAnimatedBone];
			ZeroMemory(pAni->AnimatedBoneName, sizeof(LPSTR)*pAni->NumAnimatedBone);

			pAni->AnimatedBoneRTS = new RTS[pAni->NumAnimatedBone * pAni->NumFrames];
			ZeroMemory( pAni->AnimatedBoneRTS, sizeof(RTS) * pAni->NumAnimatedBone * pAni->NumFrames );
			//pAni->AnimatedBoneMatrix = new D3DXMATRIX[pAni->NumFrames*pAni->NumAnimatedBone];
			//ZeroMemory(pAni->AnimatedBoneMatrix, sizeof(D3DXMATRIX)*pAni->NumFrames*pAni->NumAnimatedBone);

			for(DWORD i_Bone =0;i_Bone < pAni->NumAnimatedBone;i_Bone++)
			{
				pAni->AnimatedBoneName[i_Bone] = new TCHAR[30];
				fread(pAni->AnimatedBoneName[i_Bone],sizeof(TCHAR),30,pFile); 
			}		
			fread(pAni->AnimatedBoneRTS,sizeof(RTS)*pAni->NumFrames*pAni->NumAnimatedBone,1,pFile);//输出骨骼变换数目
			pAni->Type = ANIMATION_BONE_RTS;
		}
		else if ( AnimationType == ANIMATION_BONE_KEYONLY )
		{
			//For debug
			FILE* fp = fopen("d:\\debug_Bone_Trans_Key_Only.txt", "w+");
			
			fread(&pAni->NumAnimatedBone,sizeof(DWORD),1,pFile);            //该动画中包含的骨骼数目
			fread(&pAni->NumFrames,sizeof(DWORD),1,pFile);                  //该动画中包含的桢数目
			fread(&pAni->FrameLength,sizeof(float),1,pFile);                //该动画中包含的每桢之间的时间间隔
			
			pAni->AnimatedBoneName = new LPSTR[pAni->NumAnimatedBone];
			ZeroMemory(pAni->AnimatedBoneName, sizeof(LPSTR)*pAni->NumAnimatedBone);

			//pAni->AnimatedBoneMatrix = new D3DXMATRIX[pAni->NumAnimatedBone * pAni->NumFrames];
			//ZeroMemory( pAni->AnimatedBoneMatrix, sizeof(D3DXMATRIX) * pAni->NumAnimatedBone * pAni->NumFrames );

			for(DWORD i_Bone =0;i_Bone < pAni->NumAnimatedBone;i_Bone++)
			{
				pAni->AnimatedBoneName[i_Bone] = new TCHAR[30];
				fread(pAni->AnimatedBoneName[i_Bone],sizeof(TCHAR),30,pFile); 
			}		
			//Bone animation matrix offset info

			pAni->Offset = new DWORD[pAni->NumAnimatedBone];
			fread(pAni->Offset, sizeof(DWORD), pAni->NumAnimatedBone, pFile);
			DWORD dwLength;//num of matrix
			fread(&dwLength, sizeof(DWORD), 1, pFile);

			pAni->AnimatedBoneMatrix = new D3DXMATRIX[dwLength];
			ZeroMemory(pAni->AnimatedBoneMatrix, sizeof(D3DXMATRIX) * dwLength);
			fread(pAni->AnimatedBoneMatrix, sizeof(D3DXMATRIX) , dwLength, pFile);
			
			
			
			pAni->Static = new BOOL[pAni->NumAnimatedBone];
			DWORD i;
			//Generate bone movements information, static or not
			for ( i = 0; i < pAni->NumAnimatedBone - 1; i++ )
			{
				if ( pAni->Offset[i + 1] - pAni->Offset[i] == 1)
					pAni->Static[i] = TRUE;
				else 
					pAni->Static[i] = FALSE;
			}

			//check the last bone's animation status, if one frame before end of block, it's static.
			if ( dwLength - pAni->Offset[pAni->NumAnimatedBone - 1] == 1)
				pAni->Static[pAni->NumAnimatedBone - 1] = TRUE;
			else
				pAni->Static[pAni->NumAnimatedBone - 1] = FALSE;

			
			for ( i = 0; i < pAni->NumAnimatedBone; i++ )
			{
				fprintf(fp, "Bone Offset: %d\n", pAni->Offset[i]);
				printMat(pAni->AnimatedBoneMatrix[pAni->Offset[i]], fp);	
				fprintf(fp, "================================\n");
			}
			
			pAni->Type = ANIMATION_BONE_KEYONLY;
			//For debug
			fclose(fp);
		}
		else if (AnimationType==ANIMATION_VERTICES_RELATIVE)
		{
			DWORD NumVertices = 0;
			fread(&NumVertices,sizeof(float),1,pFile);                //顶点总数，用来检测是否和网格相匹配 
			if (NumVertices!=m_pMeshSys->GetNumVertices())
			{
				fclose(pFile);
				return E_FAIL;
			}
			/////////////////////////////////////////////////////////////////////////////////////
			fread(&pAni->NumAnimatedVertice,sizeof(DWORD),1,pFile);         //该动画中包含的顶点数目
			fread(&pAni->NumFrames,sizeof(DWORD),1,pFile);                  //该动画中包含的桢数目
			fread(&pAni->FrameLength,sizeof(float),1,pFile);                //该动画中包含的每桢之间的时间间隔
			pAni->AnimatedVerticesIndex = new DWORD[pAni->NumAnimatedVertice];
			ZeroMemory(pAni->AnimatedVerticesIndex,sizeof(DWORD)*pAni->NumAnimatedVertice);
			pAni->AnimatedVerticesPosition = 
				new D3DXVECTOR3[pAni->NumAnimatedVertice*pAni->NumFrames];
			ZeroMemory(pAni->AnimatedVerticesPosition,sizeof(D3DXVECTOR3)*
				pAni->NumAnimatedVertice*pAni->NumFrames);

			fread(pAni->AnimatedVerticesIndex,sizeof(DWORD),
				pAni->NumAnimatedVertice,pFile); 

			fread(pAni->AnimatedVerticesPosition,sizeof(D3DXVECTOR3)*
				pAni->NumAnimatedVertice*pAni->NumFrames,1,pFile);//输出骨骼变换数目
			pAni->Type = AnimationType;
		}
		else if (AnimationType==ANIMATION_VERTICES)
		{
			DWORD NumVertices = 0;
			fread(&NumVertices,sizeof(float),1,pFile);                //顶点总数，用来检测是否和网格相匹配 
			if (NumVertices!=m_pMeshSys->GetNumVertices())
			{
				fclose(pFile);
				return E_FAIL;
			}
			/////////////////////////////////////////////////////////////////////////////////////
			fread(&pAni->NumAnimatedVertice,sizeof(DWORD),1,pFile);         //该动画中包含的顶点数目
			fread(&pAni->NumFrames,sizeof(DWORD),1,pFile);                  //该动画中包含的桢数目
			fread(&pAni->FrameLength,sizeof(float),1,pFile);                //该动画中包含的每桢之间的时间间隔
			pAni->AnimatedVerticesIndex = new DWORD[pAni->NumAnimatedVertice];
			ZeroMemory(pAni->AnimatedVerticesIndex,sizeof(DWORD)*pAni->NumAnimatedVertice);
			pAni->AnimatedVerticesPosition = 
				new D3DXVECTOR3[pAni->NumAnimatedVertice*pAni->NumFrames];
			ZeroMemory(pAni->AnimatedVerticesPosition,sizeof(D3DXVECTOR3)*
				pAni->NumAnimatedVertice*pAni->NumFrames);

			fread(pAni->AnimatedVerticesIndex,sizeof(DWORD),
				pAni->NumAnimatedVertice,pFile); 

			fread(pAni->AnimatedVerticesPosition,sizeof(D3DXVECTOR3)*
				pAni->NumAnimatedVertice*pAni->NumFrames,1,pFile);//输出骨骼变换数目

			pAni->Type = AnimationType;
		}
		else if(AnimationType==ANIMATION_BLADE_LENGTH)
		{
			DWORD Version = 0;
			fread(&Version,sizeof(DWORD),1,pFile);                  //

			fread(&pAni->NumBlade,sizeof(DWORD),1,pFile);                  //该动画中包含的刀光的条数
			fread(&pAni->NumFrames,sizeof(DWORD),1,pFile);                  //该动画中包含的桢数目
			fread(&pAni->FrameLength,sizeof(float),1,pFile);                //该动画中包含的每桢之间的时间间隔
			
			_Animation::_Blade_ShowLength* pBladeLength = new _Animation::_Blade_ShowLength[pAni->NumFrames * pAni->NumBlade];
			ZeroMemory(pBladeLength,sizeof(_Animation::_Blade_ShowLength)*pAni->NumFrames*pAni->NumBlade);

			fread(pBladeLength,sizeof(_Animation::_Blade_ShowLength),
				pAni->NumFrames * pAni->NumBlade,pFile); 

			pAni->BladeData = pBladeLength;
			pAni->Type = AnimationType;
		}

		m_lpAnimations.push_back(pAni);
	}
	fread(&BlockEnd,sizeof(DWORD),1,pFile);

	m_dNumAnimation += NumAnimation;
	fclose(pFile);
	wsprintf(m_szAniFileName,"%s",pFileName);
	return S_OK;
}

//Add 5-16-2005 Save bone scale information
HRESULT KModel::SaveBoneScale(const TCHAR* strFileName)
{
	HRESULT hr = E_FAIL;
	DWORD dwBoneIndex;
	BoneScaleHeader bsHeader = { BONE_SCALE_MAGIC_NUMBER, m_dNumBone };
	if (strFileName)
	{
		FILE* fp = fopen(strFileName, "w+");
        assert(fp);
		fwrite(&bsHeader, sizeof(BoneScaleHeader), 1, fp);
		BoneScaleData* BSData = new BoneScaleData[m_dNumBone];
		for( dwBoneIndex = 0; dwBoneIndex < m_dNumBone; dwBoneIndex++ )
		{
			strcpy(BSData[dwBoneIndex].strBoneName, m_lpBoneInfo[dwBoneIndex].scName.c_str());
			BSData[dwBoneIndex].vec3Scale = m_lpBoneInfo[dwBoneIndex].BoneScale;
		}
		fwrite(BSData, sizeof(BoneScaleData), m_dNumBone, fp);
		SAFE_DELETE_ARRAY(BSData);
		hr = S_OK;
		fclose(fp);
	}
	return hr;
}

HRESULT KModel::LoadBoneScale(const TCHAR* strFileName)
{
	HRESULT hr = E_FAIL;
	DWORD dwNumNotFoundBone = 0;
	DWORD dwBoneIndex;
	if ( strFileName )
	{
		FILE* fp = fopen(strFileName, "r");
		if ( fp )
		{
			BoneScaleHeader bsHeader;
			fread(&bsHeader, sizeof(BoneScaleHeader), 1, fp);
			if ( bsHeader.dwMagicNumber != BONE_SCALE_MAGIC_NUMBER )
			{
				hr = D3DERR_INVALIDCALL;
			}
			else
			{
				BoneScaleData* pbsData = new BoneScaleData[bsHeader.dwNumBones];
#ifdef _DEBUG
				assert(pbsData);
#endif
				fread(pbsData, sizeof(BoneScaleData), bsHeader.dwNumBones, fp);
				for( dwBoneIndex = 0; dwBoneIndex < m_dNumBone; dwBoneIndex++ )
				{
					DWORD dwBoneOffset = FindBone(pbsData[dwBoneIndex].strBoneName);
					if ( dwBoneOffset != -1 )
					{
						m_lpBoneInfo[dwBoneOffset].BoneScale = pbsData[dwBoneIndex].vec3Scale;
					}
					else
					{
						dwNumNotFoundBone++;//record the number of bones not found
					}
				}
				SAFE_DELETE_ARRAY(pbsData);
				hr = S_OK;
			}
			fclose(fp);
		}
	}
	return hr;
}

//Add 5-16-2005 Save bone scale information end

HRESULT KModel::UpdateAnimation()
{
	if ((m_nCurAnimationIndex<0)||((DWORD)m_nCurAnimationIndex>=m_dNumAnimation))
	{
		for(DWORD i_Bone=0;i_Bone<m_dNumBone;i_Bone++)
		{
			LPBONEINFO pInfo = &m_lpBoneInfo[i_Bone];
			/*if(!pInfo->bFlexibleBone)
			{
				D3DXMatrixInverse(&pInfo->LocalWorldCurMatrix,NULL,&pInfo->MatrixOffset);		
				D3DXMatrixMultiply(&pInfo->CurMatrix,
					&pInfo->LocalWorldCurMatrix,&m_Matrix);
				D3DXMatrixMultiply(&pInfo->MatrixOffsetBase,
					&pInfo->MatrixOffsetBase,&m_Matrix); 	
				pInfo->BaseBonePosition.x = pInfo->LocalWorldCurMatrix._41;
				pInfo->BaseBonePosition.y = pInfo->LocalWorldCurMatrix._42;
				pInfo->BaseBonePosition.z = pInfo->LocalWorldCurMatrix._43;
			}  */ 
		}
	}
	else
	{
		LPANIMATION pAni = m_lpAnimations[m_nCurAnimationIndex];

		if (pAni==NULL) return E_FAIL;
		if (pAni->Type==ANIMATION_VERTICES_RELATIVE)
		{
			int nFrame = 0;
			int nFrameTo = 0;
			float R = 0;
			GetCurFrame(&nFrame,&nFrameTo,&R);

			VFormat::FACES_NORMAL_TEXTURE1 * pVers = NULL;

			if (FAILED(m_pMeshSys->LockVertexBuffer(0,(void**)&pVers)))
				return E_FAIL;
			for(DWORD i_Ver=0;i_Ver<pAni->NumAnimatedVertice;i_Ver++)
			{
				DWORD VIndex = pAni->AnimatedVerticesIndex[i_Ver];

				D3DXVECTOR3 PosA = pAni->AnimatedVerticesPosition[i_Ver*pAni->NumFrames + nFrame];
				D3DXVECTOR3 PosB = pAni->AnimatedVerticesPosition[i_Ver*pAni->NumFrames + nFrameTo];
				pVers[VIndex].p = m_lpStaticVertexPos[VIndex] + PosA*(1-R) + PosB*R;
			}

			if (FAILED(m_pMeshSys->UnlockVertexBuffer()))
				return E_FAIL;
			m_dOption &= (~MESH_OPTIMIZED);
		}
		else if (pAni->Type==ANIMATION_VERTICES)
		{
			int nFrame = 0;
			int nFrameTo = 0;
			float R = 0;
			GetCurFrame(&nFrame,&nFrameTo,&R);

			VFormat::FACES_NORMAL_TEXTURE1 * pVers = NULL;

			if (FAILED(m_pMeshSys->LockVertexBuffer(0,(void**)&pVers)))
				return E_FAIL;
			for(DWORD i_Ver=0;i_Ver<pAni->NumAnimatedVertice;i_Ver++)
			{
				DWORD VIndex = pAni->AnimatedVerticesIndex[i_Ver];

				D3DXVECTOR3 PosA = pAni->AnimatedVerticesPosition[i_Ver*pAni->NumFrames + nFrame];
				D3DXVECTOR3 PosB = pAni->AnimatedVerticesPosition[i_Ver*pAni->NumFrames + nFrameTo];
				pVers[VIndex].p = PosA*(1-R) + PosB*R;
			}

			if (FAILED(m_pMeshSys->UnlockVertexBuffer()))
				return E_FAIL;
			m_dOption &= (~MESH_OPTIMIZED);
		}
		else if (pAni->Type==ANIMATION_BONE)
		{
			int nFrame = 0;
			int nFrameTo = 0;
			float R = 0;
			GetCurFrame(&nFrame,&nFrameTo,&R);

			//////////////////////////柔体//////////////////////////////////////////
			for(DWORD i_Bone=0;i_Bone<pAni->NumAnimatedBone;i_Bone++)
			{
				LPSTR Name = pAni->AnimatedBoneName[i_Bone];
				int BoneIndex = FindBone(Name);
				D3DXMATRIX MatA = pAni->AnimatedBoneMatrix[i_Bone*pAni->NumFrames + nFrame];
				D3DXMATRIX MatB = pAni->AnimatedBoneMatrix[i_Bone*pAni->NumFrames + nFrameTo];
				if ((BoneIndex>=0)&&((DWORD)BoneIndex<m_dNumBone))
				{
					LPBONEINFO pInfo = &m_lpBoneInfo[BoneIndex];
					//if(pInfo->bFlexibleBone)
					//{
					//	if ((pInfo->nParentIndex>=0)&&(pInfo->nParentIndex<(int)m_dNumBone))
					//	{
					//		LPBONEINFO pParent = &m_lpBoneInfo[pInfo->nParentIndex];
					//		if(pParent->bFlexibleBone)
					//		{
					//			continue;
					//		}
					//		else
					//		{	
					//			//计算柔体骨骼的根节点在从上一帧位置到下一帧位置的变换矩阵
					//			D3DXMATRIX MatT = MatA*(1-R) + MatB*R;
					//			D3DXMATRIX MatTemp;
					//			D3DXMatrixInverse( &MatTemp, NULL, &pInfo->LocalWorldCurMatrix );
					//			D3DXMatrixMultiply( &MatTemp, &MatTemp, &MatT );

					//			for(int i_Flex=0;i_Flex<(int)m_vecStaticFlexibleBoneIndex.size();i_Flex++)
					//			{
					//				int nFlexBoneIndex = m_vecStaticFlexibleBoneIndex[i_Flex].Index;
					//				if( nFlexBoneIndex == BoneIndex )
					//				{
					//					m_vecStaticFlexibleBoneIndex[i_Flex].MatrixFlexWold=MatTemp;
					//					break;
					//				}
					//			}
					//		}
					//	}

					//	if (pInfo->nParentIndex<0)
					//	{
					//		D3DXMATRIX MatT = MatA*(1-R) + MatB*R;
					//		D3DXMATRIX MatTemp;
					//		D3DXMatrixInverse( &MatTemp, NULL, &pInfo->LocalWorldCurMatrix );
					//		D3DXMatrixMultiply( &MatTemp, &MatTemp, &MatT );

					//		for(int i_Flex=0;i_Flex<(int)m_vecStaticFlexibleBoneIndex.size();i_Flex++)
					//		{
					//			int nFlexBoneIndex = m_vecStaticFlexibleBoneIndex[i_Flex].Index;
					//			if( nFlexBoneIndex == BoneIndex )
					//			{
					//				m_vecStaticFlexibleBoneIndex[i_Flex].MatrixFlexWold=MatTemp;
					//				break;
					//			}
					//		}  
					//	}
					//}
					//else
					if(!pInfo->bFlexibleBone)
					{
						pInfo->CurMatrix = MatA*(1-R) + MatB*R;
						D3DXMatrixMultiply(&pInfo->CurMatrix,
							&pInfo->CurMatrix,&m_Matrix);	
					}




				}
			}
			for(DWORD i_Socket=0;i_Socket<m_dNumSocket;i_Socket++)
			{
				LPSOCKET pSocket = &m_lpSockets[i_Socket];
				int Index = FindBone((LPSTR)pSocket->scParentBoneName.c_str()); 
				if (Index>=0)
				{
					D3DXMATRIX mat;

					LPBONEINFO pInfoParent = &m_lpBoneInfo[Index];
#ifndef SWORD2
					*D3DXMatrixMultiply(&pSocket->CurMatrix,
						&pSocket->MatrixOffset, &pInfoParent->CurMatrix	);
#endif						

					//////////////////////////////////////////////////////////////////////////
					//Delete 5-11-2005 for correct bug of Socket
#ifdef SWORD2
					/*D3DXMatrixMultiply(&pSocket->CurMatrix,
						&pInfoParent->CurMatrix, &pSocket->MatrixOffset	);*/
					D3DXVECTOR3 A,B,C;
					A = D3DXVECTOR3(pInfoParent->CurMatrix._11,pInfoParent->CurMatrix._12,pInfoParent->CurMatrix._13);
					B = D3DXVECTOR3(pInfoParent->CurMatrix._21,pInfoParent->CurMatrix._22,pInfoParent->CurMatrix._23);
					C = D3DXVECTOR3(pInfoParent->CurMatrix._31,pInfoParent->CurMatrix._32,pInfoParent->CurMatrix._33);
					float Sx,Sy,Sz;
					Sx = D3DXVec3Length(&A);
					Sy = D3DXVec3Length(&B);
					Sz = D3DXVec3Length(&C);
					D3DXMATRIX matScale;
					D3DXMatrixScaling(&matScale,Sx,Sy,Sz);
					D3DXMatrixInverse(&matScale,NULL,&matScale);

					D3DXMatrixMultiply(&mat, &matScale, &pInfoParent->CurMatrix );

					pSocket->MatrixBase = mat;//
					D3DXMatrixMultiply(&pSocket->CurMatrix, &pSocket->MatrixOffset,&mat);
#endif
					//Delete 5-11-2005 for correct bug for socket end
					//////////////////////////////////////////////////////////////////////////
					
				}
			}
		}
		else if (pAni->Type==ANIMATION_BONE_RELATIVE)
		{
			int nFrame = 0;
			int nFrameTo = 0;
			float R = 0;
			GetCurFrame(&nFrame,&nFrameTo,&R);

			float T = sinf(timeGetTime()*0.001f)*0.03f+1.0f;
			//float T = 1.0f;

			//FILE* fp = fopen("d:\\debug_relate.txt", "w+");
			for(DWORD i_Bone=0;i_Bone<pAni->NumAnimatedBone;i_Bone++)
			{
				LPSTR Name = pAni->AnimatedBoneName[i_Bone];
				int BoneIndex = FindBone(Name);
				if ((BoneIndex>=0)&&((DWORD)BoneIndex<m_dNumBone))
				{
					LPBONEINFO pInfo = &m_lpBoneInfo[BoneIndex];

					int Index_A = i_Bone*pAni->NumFrames + nFrame;
					int Index_B = i_Bone*pAni->NumFrames + nFrameTo;

					D3DXMATRIX   MatFrameA,MatFrameB;

					MatFrameA = pAni->AnimatedBoneMatrix[Index_A];
					MatFrameB = pAni->AnimatedBoneMatrix[Index_B];

					pInfo->RelativeMatrix = MatFrameA * (1-R) + MatFrameB * R;
					/*if ( !strcmp(Name, "Bip01 L Clavicle") || !strcmp(Name, "Bip01 R Clavicle") || !strcmp(Name, "Bip01 L Thigh") || !strcmp(Name, "Bip01 R Thigh") )
					{
						pInfo->RelativeMatrix._11 *= 2.0f;
						pInfo->RelativeMatrix._22 *= 2.0f;
						pInfo->RelativeMatrix._33 *= 2.0f;
					}*/
					//fprintf(fp, "Bone: %d\nFrameA\n", BoneIndex);
					//printMat(MatFrameA, fp);
					//fprintf(fp, "FrameB\n");
					//printMat(MatFrameB, fp);
					//fprintf(fp, "=================================\n");
				}
			}
			//fclose(fp);

			D3DXMATRIX Matrix;
			D3DXMatrixIdentity(&Matrix);
			
			size_t NumBaseBone = m_nBaseBoneID.size();
			for( size_t S = 0; S < NumBaseBone; S++ )
			{
				int nBaseBoneID = m_nBaseBoneID[S];
				UpdataRelativeBoneMatrix(m_nBaseBoneID[S],Matrix);
			}

			for(DWORD i_Socket=0;i_Socket<m_dNumSocket;i_Socket++)
			{
				LPSOCKET pSocket = &m_lpSockets[i_Socket];
				int Index = FindBone((LPSTR)pSocket->scParentBoneName.c_str()); 
				if (Index>=0)
				{
					D3DXMATRIX mat;

					LPBONEINFO pInfoParent = &m_lpBoneInfo[Index];
					D3DXMatrixMultiply(&pSocket->CurMatrix,
						&pSocket->MatrixOffset, &pInfoParent->CurMatrix );
				}
			}

			/*for(DWORD i_Socket=0;i_Socket<m_dNumSocket;i_Socket++)
			{
			LPSOCKET pSocket = &m_lpSockets[i_Socket];
			int Index = FindBone(pSocket->ParentBoneName); 
			if (Index>=0)
			{
			D3DXMATRIX mat;

			LPBONEINFO pInfoParent = &m_lpBoneInfo[Index];
			D3DXMatrixMultiply(&pSocket->CurMatrix,
			&pInfoParent->CurMatrix,
			&pSocket->MatrixOffset);
			D3DXVECTOR3 A,B,C;
			A = D3DXVECTOR3(pInfoParent->CurMatrix._11,pInfoParent->CurMatrix._12,pInfoParent->CurMatrix._13);
			B = D3DXVECTOR3(pInfoParent->CurMatrix._21,pInfoParent->CurMatrix._22,pInfoParent->CurMatrix._23);
			C = D3DXVECTOR3(pInfoParent->CurMatrix._31,pInfoParent->CurMatrix._32,pInfoParent->CurMatrix._33);
			float Sx,Sy,Sz;
			Sx = D3DXVec3Length(&A);
			Sy = D3DXVec3Length(&B);
			Sz = D3DXVec3Length(&C);
			D3DXMATRIX matScale;
			D3DXMatrixScaling(&matScale,Sx,Sy,Sz);
			D3DXMatrixInverse(&matScale,NULL,&matScale);

			D3DXMatrixMultiply(&mat,
			&matScale,
			&pInfoParent->CurMatrix);

			D3DXMatrixMultiply(&pSocket->CurMatrix,
			&pSocket->MatrixOffset,&mat);
			}
			}*/
		}
		else if ( pAni->Type == ANIMATION_BONE_KEYONLY )
		{
			int nFrame = 0;
			int nFrameTo = 0;
			float R = 0.0;
			GetCurFrame(&nFrame,&nFrameTo,&R);
			
			float T = sinf(timeGetTime()*0.001f)*0.03f+1.0f;
			//float T = 1.0f;
	
			static bool boutput = 0;
		
			//FILE* fp = fopen("d:\\debug_key_only.txt", "w+");
			for(DWORD i_Bone=0;i_Bone<pAni->NumAnimatedBone;i_Bone++)
			{
				LPSTR Name = pAni->AnimatedBoneName[i_Bone];
				int BoneIndex = FindBone(Name);
				if ((BoneIndex>=0)&&((DWORD)BoneIndex<m_dNumBone))
				{
					LPBONEINFO pInfo = &m_lpBoneInfo[BoneIndex];
					if ( pAni->Static[BoneIndex] )//if the bone is not move in object space
					{
						pInfo->RelativeMatrix = pAni->AnimatedBoneMatrix[pAni->Offset[BoneIndex]];
						//fprintf(fp, "Bone: %d\nStatic\n", BoneIndex);
						//printMat(pInfo->RelativeMatrix, fp);
						//fprintf(fp, "=================================\n");
					}
					else
					{
						int Index_A = pAni->Offset[BoneIndex] + nFrame;
						int Index_B = pAni->Offset[BoneIndex] + nFrameTo;

						D3DXMATRIX   MatFrameA,MatFrameB;

						MatFrameA = pAni->AnimatedBoneMatrix[Index_A];
						MatFrameB = pAni->AnimatedBoneMatrix[Index_B];
		
						pInfo->RelativeMatrix = MatFrameA * (1-R) + MatFrameB * R;
						
						//fprintf(fp, "Bone: %d\nFrameA\n", BoneIndex);
						//printMat(MatFrameA, fp);
						//fprintf(fp, "FrameB\n");
						//printMat(MatFrameB, fp);
						//fprintf(fp, "=================================\n");
					}
				}

			}
			//fclose(fp);

			D3DXMATRIX Matrix;
			D3DXMatrixIdentity(&Matrix);
            for(int S=0; S<(int)m_nBaseBoneID.size(); S++)
			{
				UpdataRelativeBoneMatrix(m_nBaseBoneID[S],Matrix);
			}
		}

		

		else if ( pAni->Type == ANIMATION_BONE_RTS )
		{

			int nFrame = 0;
			int nFrameTo = 0;
			float R = 0.5f;
			GetCurFrame(&nFrame,&nFrameTo,&R);
			//nFrame = 2;
			//nFrameTo = 3;
#ifdef KEY_FRAME_INTERPOLATION_BUG
//////////////////////////////////////////////////////////////////////////
//Add 5-26-2005 Key frame interpolation bug fix
//Huh, stupid way, hah? Do you think i'm that stupid. If there is no that god damn bug of relative interpolation bug.
//I'm trying to fix it.
			/*BOOL bRecacalate = FALSE;
			DWORD BoneIndex;
			if ( m_nFrameAnimationIndex != m_nCurAnimationIndex )
				bRecacalate = TRUE;
			
			if ( !m_pFrameA )
				m_pFrameA = new D3DXMATRIX[m_dNumBone];
			if ( !m_pFrameB)
				m_pFrameB = new D3DXMATRIX[m_dNumBone];
			assert(m_pFrameB && m_pFrameA);
			D3DXMATRIX matSRot, matSRotInv, matRot, matScale, matTrans, matScaleSign;
			D3DXQUATERNION sRot, Rot;
			D3DXVECTOR3 Scale, Translation;
			D3DXMATRIX Matrix;
			D3DXMatrixIdentity(&Matrix);

			if ( m_nFrameB == nFrameTo )
			{
				D3DXMATRIX* pTemp = m_pFrameB;
				m_pFrameB = m_pFrameA;
				m_pFrameA = pTemp;
				m_nFrameA = m_nFrameB;
			}

			for (DWORD i_Bone = 0; i_Bone < pAni->NumAnimatedBone; i_Bone++ )
			{
				LPSTR Name = pAni->AnimatedBoneName[i_Bone];
				int BoneIndex = FindBone(Name);
				if ( BoneIndex >= 0 && (DWORD)BoneIndex < m_dNumBone )
				{
					LPBONEINFO pInfo = &m_lpBoneInfo[BoneIndex];
					int Index_A = i_Bone*pAni->NumFrames + nFrame;
					int Index_B = i_Bone*pAni->NumFrames + nFrameTo;


					RTS& FrameA = pAni->AnimatedBoneRTS[Index_A];
					RTS& FrameB = pAni->AnimatedBoneRTS[Index_B];
					
					if ( nFrame != m_nFrameA || bRecacalate )
					{
						D3DXMatrixRotationQuaternion(&matSRot, &FrameA.SRotation);
						D3DXMatrixInverse(&matSRotInv, NULL, &matSRot);

						D3DXMatrixRotationQuaternion(&matRot, &FrameA.Rotation);
						D3DXMatrixScaling(&matScale, FrameA.Scale.x, FrameA.Scale.y, FrameA.Scale.z);

						matScale._11 *= pInfo->BoneScale.x;
						matScale._22 *= pInfo->BoneScale.y;
						matScale._33 *= pInfo->BoneScale.z;

						m_pFrameA[BoneIndex] = matRot * matSRot *matScale * matSRotInv;
						D3DXMatrixScaling(&matScale, FrameA.Sign, FrameA.Sign, FrameA.Sign );
						m_pFrameA[BoneIndex] = matScale * m_pFrameA[BoneIndex];

						D3DXMatrixTranspose(&m_pFrameA[BoneIndex], &m_pFrameA[BoneIndex]);
						m_pFrameA[BoneIndex].m[3][0] += FrameA.Translation.x;
						m_pFrameA[BoneIndex].m[3][1] += FrameA.Translation.y;
						m_pFrameA[BoneIndex].m[3][2] += FrameA.Translation.z;
					}

					if ( nFrameTo != m_nFrameB || bRecacalate )
					{
						D3DXMatrixRotationQuaternion(&matSRot, &FrameB.SRotation);
						D3DXMatrixInverse(&matSRotInv, NULL, &matSRot);

						D3DXMatrixRotationQuaternion(&matRot, &FrameB.Rotation);
						D3DXMatrixScaling(&matScale, FrameB.Scale.x, FrameB.Scale.y, FrameB.Scale.z);

						matScale._11 *= pInfo->BoneScale.x;
						matScale._22 *= pInfo->BoneScale.y;
						matScale._33 *= pInfo->BoneScale.z;

						m_pFrameB[BoneIndex] = matRot * matSRot *matScale * matSRotInv;
						D3DXMatrixScaling(&matScale, FrameB.Sign, FrameB.Sign, FrameB.Sign );

						m_pFrameB[BoneIndex] = matScale * m_pFrameB[BoneIndex];
						D3DXMatrixTranspose(&m_pFrameB[BoneIndex], &m_pFrameB[BoneIndex]);

						m_pFrameB[BoneIndex].m[3][0] += FrameB.Translation.x;
						m_pFrameB[BoneIndex].m[3][1] += FrameB.Translation.y;
						m_pFrameB[BoneIndex].m[3][2] += FrameB.Translation.z;
					}
				}
			}

			if ( m_nFrameA != nFrame || bRecacalate )
			{
				for(int S=0;S<(int)m_nBaseBoneID.size();S++)
				{
					UpdataRelativeBoneMatrix(m_nBaseBoneID[S],Matrix, m_pFrameA, m_pFrameA);
				}
			}
			
			if ( m_nFrameB != nFrameTo || bRecacalate )
			{
				for(int S=0;S<(int)m_nBaseBoneID.size();S++)
				{
					UpdataRelativeBoneMatrix(m_nBaseBoneID[S],Matrix, m_pFrameB, m_pFrameB);
				}
			}

			for ( BoneIndex = 0; BoneIndex < m_dNumBone; BoneIndex++ )
			{
				m_lpBoneInfo[BoneIndex].CurMatrix = m_pFrameA[BoneIndex] * ( 1 - R ) + m_pFrameB[BoneIndex] * R;
			}
			
			m_nFrameA = nFrame;
			m_nFrameB = nFrameTo;*/
			
//Add 5-26-2005 Key frame interpolation bug fix end
//////////////////////////////////////////////////////////////////////////
			
#else
			float T = sinf(timeGetTime()*0.001f)*0.03f+1.0f;
			//float T = 1.0f;


			for(DWORD i_Bone=0;i_Bone<pAni->NumAnimatedBone;i_Bone++)
			{
				LPSTR Name = pAni->AnimatedBoneName[i_Bone];
				/*if ( !strcmp( Name, "Bip01 L Calf" ) )
				{
					int asdfasdfasdf = 10;
				}*/
				int BoneIndex = FindBone(Name);
				if ((BoneIndex>=0)&&((DWORD)BoneIndex<m_dNumBone))
				{
					LPBONEINFO pInfo = &m_lpBoneInfo[BoneIndex];

					int Index_A = i_Bone*pAni->NumFrames + nFrame;
					int Index_B = i_Bone*pAni->NumFrames + nFrameTo;

					
					RTS& rtsFrameA = pAni->AnimatedBoneRTS[Index_A];
					RTS& rtsFrameB = pAni->AnimatedBoneRTS[Index_B];

					InterpolateRTSBoneKeyFrame(pInfo, rtsFrameA, rtsFrameB, R);

					
				}
			}

			D3DXMATRIX Matrix;
			D3DXMatrixIdentity(&Matrix);
			
			for(int S=0;S<(int)m_nBaseBoneID.size();S++)
			{
				UpdataRelativeBoneMatrix(m_nBaseBoneID[S],Matrix);
			}
#endif
			for(DWORD i_Socket=0;i_Socket<m_dNumSocket;i_Socket++)
			{
				LPSOCKET pSocket = &m_lpSockets[i_Socket];
				int Index = FindBone((LPSTR)pSocket->scParentBoneName.c_str()); 
				if (Index>=0)
				{
					D3DXMATRIX mat;

					LPBONEINFO pInfoParent = &m_lpBoneInfo[Index];
					//pSocket->CurMatrix = pSocket->MatrixOffset;
					D3DXMatrixMultiply(&pSocket->CurMatrix, &pSocket->MatrixOffset, &pInfoParent->CurMatrix );
				}
			}
			/*if ( m_dNumAnimation == 2 )
			{
				static float time = timeGetTime() / 1000.0f;
				LPANIMATION pAni1 = m_lpAnimations[0];
				LPANIMATION pAni2 = m_lpAnimations[1];
				AnimateTo(m_lpAnimations[0], m_lpAnimations[1], 2.0f, timeGetTime() / 1000.0f - time);
			}*/
		}


	}
	//////////////////////////柔体//////////////////////////////////////////
	for(int i_Flex=0;i_Flex<(int)m_vecStaticFlexibleBoneIndex.size();i_Flex++)
	{
		int nFlexBoneIndex = m_vecStaticFlexibleBoneIndex[i_Flex].Index;
		LPBONEINFO pInfo = &m_lpBoneInfo[nFlexBoneIndex];
		UpdateFlexibleBone(pInfo,0,i_Flex);
		//D3DXMatrixIdentity(&m_vecStaticFlexibleBoneIndex[i_Flex].MatrixFlexWold);
	}

	return S_OK;
}

HRESULT KModel::CreateSkinInfo(BOOL bLoad)
{
	HRESULT hr = S_OK;

	SAFE_RELEASE(m_pSkinInfo);
	SAFE_DELETE_ARRAY(m_pBoneMatrices);
	m_pBoneMatrices = new D3DXMATRIX[m_dNumBone];
	if ( bLoad)
	{
		//if( FAILED( D3DXCreateSkinInfoFVF( m_pBlendMesh->GetNumVertices(), m_pBlendMesh->GetFVF(), m_dNumBone, &m_pSkinInfo )))
		//	return E_FAIL;
		m_dOption |= MESH_SKINED;
		return hr;
	}
	else 
	{
		if (FAILED(D3DXCreateSkinInfoFVF(m_pMeshSys->GetNumVertices(),
		m_pMeshSys->GetFVF(),m_dNumBone,&m_pSkinInfo)))
			return E_FAIL;
	}
	
	//FILE* Note = fopen("D:\\SkinDebug.txt","wt");
	//fprintf(Note,"Num of Vertices : %d \n",m_pMeshSys->GetNumVertices());

	for(DWORD i_Bone=0;i_Bone<m_dNumBone;i_Bone++)
	{
		LPBONEINFO pBinfo = &m_lpBoneInfo[i_Bone];
		if (FAILED(hr = m_pSkinInfo->SetBoneInfluence(i_Bone,pBinfo->NumVerticesRef,
			pBinfo->VerticesIndexRef,
			pBinfo->VerticesWeightRef)))
			return hr; 
		if (FAILED(hr = m_pSkinInfo->SetBoneName(i_Bone,pBinfo->scName.c_str())))
		{
			return hr;
		}
		m_pSkinInfo->SetBoneOffsetMatrix(i_Bone,&pBinfo->MatrixOffset); 
		//fprintf(Note,"%d Bone %s Has %d Vertices \n",i_Bone,pBinfo->scName.c_str(),pBinfo->NumVerticesRef);

		for(DWORD n=0;n<pBinfo->NumVerticesRef;n++)
		{
			DWORD Index = pBinfo->VerticesIndexRef[n];
			//fprintf(Note,"    %d Vertices %d Weight %f \n",n,
			//	Index,
			//	pBinfo->VerticesWeightRef[n]);
			//if(Index>=m_pMeshSys->GetNumVertices())
			//{
			//	ASSERT(NULL);
			//}
		}
	}
	//fclose(Note);
	

	//just to use soft skinning
	/*hr = m_pMeshSys->CloneMeshFVF(D3DXMESH_MANAGED, 
	m_pMeshSys->GetFVF(),
	g_pd3dDevice, &m_pBlendMesh);
	if (FAILED(hr))
	return hr;
	return S_OK;*/

	//MessageBox(g_hBaseWnd," Start to Create Blended Mesh !","Load Skined Mesh!",0);
	if (FAILED(CreateSkinBlendedMesh(bLoad)))
	{
		MessageBox(g_hBaseWnd," FAILED to Create Blended Mesh !","Load Skined Mesh!",0);
		return E_FAIL;
	}
	else
	{
		m_dOption |= MESH_SKINED;
	}
	//MessageBox(g_hBaseWnd," Skinning All Succeeded !","Load Skined Mesh!",0);
	return S_OK;
}

HRESULT KModel::SoftSkinning()
{
	D3DXMATRIX* m_pBoneMatrices = new D3DXMATRIX[m_dNumBone];
	// set up bone transforms
	for (DWORD iBone = 0; iBone < m_dNumBone; iBone++)
	{
		LPBONEINFO pInfo = &m_lpBoneInfo[iBone];
		D3DXMatrixMultiply(&m_pBoneMatrices[iBone],
			&(pInfo->MatrixOffset),
			&(pInfo->CurMatrix));
	}

	PBYTE       pbVerticesSrc;
	PBYTE       pbVerticesDest;
	if (FAILED(m_pMeshSys->LockVertexBuffer(0, (LPVOID*)&pbVerticesSrc)))
		return E_FAIL;
	if (FAILED(m_pBlendMesh->LockVertexBuffer(0, (LPVOID*)&pbVerticesDest)))
		return E_FAIL;

	// generate skinned mesh
	if (FAILED(m_pSkinInfo->UpdateSkinnedMesh(m_pBoneMatrices, NULL, pbVerticesSrc, pbVerticesDest)))
		return E_FAIL;

	m_pMeshSys->UnlockVertexBuffer();
	m_pBlendMesh->UnlockVertexBuffer();
	return S_OK;
}

HRESULT KModel::AnimateTo(LPANIMATION pFrom, LPANIMATION pTo, float fTimeSpan, float CurrentTime)
{
	//CurrentTime = 2.0f;
	ASSERT(pFrom);
	ASSERT(pTo);
	if ( pFrom->Type != pTo->Type || 
		(pFrom->Type != ANIMATION_BONE_RTS) ||
		(pFrom->NumAnimatedBone != pTo->NumAnimatedBone )
		)
		return -1;
	
	float R = CurrentTime / fTimeSpan;
	
	if ( R > 1.0f )
		R = 1.0f;

	for(DWORD i_Bone=0;i_Bone<pFrom->NumAnimatedBone;i_Bone++)
	{
		LPSTR Name = pFrom->AnimatedBoneName[i_Bone];
		int BoneIndex = FindBone(Name);
		if ((BoneIndex>=0)&&((DWORD)BoneIndex<m_dNumBone))
		{
			LPBONEINFO pInfo = &m_lpBoneInfo[BoneIndex];

			int Index_A = i_Bone*pFrom->NumFrames + pFrom->NumFrames - 1;
			int Index_B = i_Bone*pTo->NumFrames;

			D3DXMATRIX   MatFrameA,MatFrameB;
			RTS          rtsFrameA, rtsFrameB;
			rtsFrameA = pFrom->AnimatedBoneRTS[Index_A];
			rtsFrameB = pTo->AnimatedBoneRTS[Index_B];

			D3DXMATRIX matSRot, matSRotInv, matRot, matScale, matTrans;

			D3DXQUATERNION sRot, Rot;
			D3DXQuaternionSlerp(&sRot, &rtsFrameA.SRotation, &rtsFrameB.SRotation, R);

			D3DXMatrixRotationQuaternion(&matSRot, &sRot);
			D3DXMatrixInverse(&matSRotInv, NULL, &matSRot);

			D3DXQuaternionSlerp(&Rot, &rtsFrameA.Rotation, &rtsFrameB.Rotation, R);

			D3DXMatrixRotationQuaternion(&matRot, &Rot);
			
			D3DXMatrixScaling(&matScale, rtsFrameA.Scale.x * ( 1.0f - R ) + rtsFrameB.Scale.x * R, 
				              rtsFrameA.Scale.y * (1.0f - R) + rtsFrameB.Scale.y * R, 
							  rtsFrameA.Scale.z * (1.0f - R) + rtsFrameB.Scale.z * R);
			MatFrameA = matRot * matSRot *matScale * matSRotInv;
			D3DXMatrixScaling(&matScale, rtsFrameA.Sign, rtsFrameA.Sign, rtsFrameA.Sign );
			MatFrameA = matScale * MatFrameA;

			D3DXMatrixTranspose(&MatFrameA, &MatFrameA);
			MatFrameA.m[3][0] += rtsFrameA.Translation.x * ( 1 - R) + rtsFrameB.Translation.x * R;
			MatFrameA.m[3][1] += rtsFrameA.Translation.y * ( 1 - R) + rtsFrameB.Translation.y * R;
			MatFrameA.m[3][2] += rtsFrameA.Translation.z * ( 1 - R) + rtsFrameB.Translation.z * R;


			//D3DXMatrixRotationQuaternion(&matSRot, &rtsFrameB.SRotation);
			//D3DXMatrixInverse(&matSRotInv, NULL, &matSRot);

			//D3DXMatrixRotationQuaternion(&matRot, &rtsFrameB.Rotation);
			//D3DXMatrixScaling(&matScale, rtsFrameB.Scale.x, rtsFrameB.Scale.y, rtsFrameB.Scale.z);
			//MatFrameB = matRot * matSRot *matScale * matSRotInv;
			//D3DXMatrixScaling(&matScale, rtsFrameB.Sign, rtsFrameB.Sign, rtsFrameB.Sign );
			//MatFrameB = matScale * MatFrameB;
			//D3DXMatrixTranspose(&MatFrameB, &MatFrameB);

			//MatFrameB.m[3][0] += rtsFrameB.Translation.x;
			//MatFrameB.m[3][1] += rtsFrameB.Translation.y;
			//MatFrameB.m[3][2] += rtsFrameB.Translation.z;

			pInfo->RelativeMatrix = MatFrameA;// * (1-R) + MatFrameB * R; 
		}
	}

	D3DXMATRIX Matrix;
	D3DXMatrixIdentity(&Matrix);
	for(int S=0;S<(int)m_nBaseBoneID.size();S++)
	{
		UpdataRelativeBoneMatrix(m_nBaseBoneID[S],Matrix);
	}
	return S_OK;
}



HRESULT KModel::DrawMesh(LPD3DXMESH pMesh)
{
	//SetShader();

	for(DWORD i=0;i<m_dNumMaterial;i++)
	{
		DrawMeshSubset(pMesh,i);
	}

	/*

	for( i=0;i<m_dNumMaterial;i++)
	{
	pMesh->DrawSubset(i);
	}
	*/
	g_pd3dDevice->SetVertexDeclaration(NULL);
	g_pd3dDevice->SetVertexShader(NULL);

	return S_OK;
}

HRESULT KModel::DrawBones()
{
	for(DWORD i_Bone=0;i_Bone<m_dNumBone;i_Bone++)
	{
		LPBONEINFO pInfo = &m_lpBoneInfo[i_Bone];
		D3DXVECTOR3 A,B;
		B = A = D3DXVECTOR3(0,0,0);
		D3DXVec3TransformCoord(&B,&B,&pInfo->CurMatrix);

		int Index = FindBone((LPSTR)pInfo->scParentName.c_str()); 
		if (Index>=0)
		{
			LPBONEINFO pInfoParent = &m_lpBoneInfo[Index];
			D3DXVec3TransformCoord(&A,&A,&pInfoParent->CurMatrix);
			A += D3DXVECTOR3(0,0,150);
			B += D3DXVECTOR3(0,0,150);

			if(pInfo->bFlexibleBone)
			{
				g_cGraphicsTool.DrawLine(&A,&B,0xFFFF00FF,0xFFFF00FF); 
			}
			else
			{
				g_cGraphicsTool.DrawLine(&A,&B,0xFF0000FF,0xFFFF0000); 
			}
		}
	}
	m_SelectedBone.DrawBoneIndicator();
	return S_OK;
}

HRESULT KModel::VertexProcessSkin()
{

	HRESULT hr = S_OK;
	DWORD AttribIdPrev = UNUSED32; 

	if (!m_pBoneCombinationTable)
		return E_FAIL;

	LPD3DXBONECOMBINATION pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(m_pBoneCombinationTable->GetBufferPointer());

	D3DXMATRIX MatSave;
	g_pd3dDevice->GetTransform( D3DTS_WORLD , &MatSave);

	for(DWORD i_Bone=0;i_Bone<m_dNumBone;i_Bone++)
	{
		LPBONEINFO pInfo = &m_lpBoneInfo[i_Bone];
		D3DXMatrixMultiply(&m_pBoneMatrices[i_Bone],
			&(pInfo->MatrixOffset),
			&(pInfo->CurMatrix));
		/*D3DXMatrixMultiply(&m_pBoneMatrices[i_Bone],
			&m_pBoneMatrices[i_Bone],
			&MatSave);*/
	}

	LPD3DXMESH pMesh = NULL;

	if(m_dOption&MESH_PROGRESS)
	{
		//pMesh = (LPD3DXMESH)m_pMeshProgress;
		pMesh = (LPD3DXMESH)m_pMeshEnhanced;
	}
	else
	{
		pMesh = m_pBlendMesh;
	}
	// Draw using default vtx processing of the device (typically HW)
	D3DXMATRIX matView;
	D3DXMATRIX matViewSave;
	g_pd3dDevice->GetTransform( D3DTS_VIEW, &matViewSave );
	D3DXMatrixMultiply( &matView, &MatSave, &matViewSave );
	g_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

	for (DWORD iAttrib = 0; iAttrib < m_NumAttributeGroups; iAttrib++)
	{
		DWORD NumBlend = 0;
		for (DWORD i = 0; i < m_MaxFaceInfl; ++i)
		{
			if (pBoneComb[iAttrib].BoneId[i] != UINT_MAX)
			{
				NumBlend = i;
			}
		}
		if (g_D3DCaps.MaxVertexBlendMatrices >= NumBlend + 1)
		{
			// first calculate the world matrices for the current set of blend weights and get the accurate count of the number of blends
			for (DWORD i = 0; i < m_MaxFaceInfl; ++i)
			{
				DWORD iMatrixIndex = pBoneComb[iAttrib].BoneId[i];
				if (iMatrixIndex != UINT_MAX)
				{
					g_pd3dDevice->SetTransform( D3DTS_WORLDMATRIX( i ), &m_pBoneMatrices[iMatrixIndex] );
				}
			}
			g_pd3dDevice->SetRenderState(D3DRS_VERTEXBLEND, NumBlend);


			DrawSkinMeshSubset(pMesh,pBoneComb[iAttrib].AttribId,iAttrib,NumBlend+1);
		}
	}
	// If necessary, draw parts that HW could not handle using SW
	if (m_iAttributeSW < m_NumAttributeGroups)
	{
		AttribIdPrev = UNUSED32; 
		g_pd3dDevice->SetSoftwareVertexProcessing(TRUE);
		for (DWORD iAttrib = m_iAttributeSW; iAttrib < m_NumAttributeGroups; iAttrib++)
		{
			DWORD NumBlend = 0;
			for (DWORD i = 0; i < m_MaxFaceInfl; ++i)
			{
				if (pBoneComb[iAttrib].BoneId[i] != UINT_MAX)
				{
					NumBlend = i;
				}
			}
			if (g_D3DCaps.MaxVertexBlendMatrices < NumBlend + 1)
			{
				// first calculate the world matrices for the current set of blend weights and get the accurate count of the number of blends
				for (DWORD i = 0; i < m_MaxFaceInfl; ++i)
				{
					DWORD iMatrixIndex = pBoneComb[iAttrib].BoneId[i];
					if (iMatrixIndex != UINT_MAX)
					{
						g_pd3dDevice->SetTransform( D3DTS_WORLDMATRIX( i ), &m_pBoneMatrices[iMatrixIndex] );
					}
				}

				g_pd3dDevice->SetRenderState(D3DRS_VERTEXBLEND, NumBlend);

				DrawSkinMeshSubset(pMesh,pBoneComb[iAttrib].AttribId,iAttrib,NumBlend+1);

			}
		}
		g_pd3dDevice->SetSoftwareVertexProcessing(FALSE);
	}
	g_pd3dDevice->SetTransform( D3DTS_VIEW, &matViewSave );
	g_pd3dDevice->SetRenderState(D3DRS_VERTEXBLEND, 0);
	g_pd3dDevice->SetTransform( D3DTS_WORLD , &MatSave);

	return S_OK;

	

	return S_OK;
}

HRESULT KModel::DrawSkinMeshSubset(LPD3DXMESH pMesh,DWORD MaterialID,DWORD AttribID,DWORD NumBoneBlend)
{
	if(NumBoneBlend>4) 
		return S_OK;
 
	HRESULT hr = S_OK;
	LPMATERIAL pMaterial = &m_lpMaterial[MaterialID]; 

	if (pMaterial->m_lpEffect)
	{
		LPMTLMSEFFECT pEffect = pMaterial->m_lpEffect;
		if( pEffect->m_HEffect->pEffect)
		{
			pEffect->SetParameter(pMaterial,this);

			D3DXVECTOR3 Light(0,0,0);
			g_cGraphicsTool.GetSunDirection(Light);
			D3DXVECTOR4 Vec(Light.x,Light.y,Light.z,0);

			pEffect->m_HEffect->pEffect->SetVector("Light Direction",&Vec);
			pEffect->m_HEffect->pEffect->SetInt("NumBoneBlend",NumBoneBlend);
			D3DXMATRIX BlendMatrix[8];
			ZeroMemory(BlendMatrix,sizeof(D3DXMATRIX)*8);
			//D3DXMATRIX CurMatrix;
			//g_pd3dDevice->GetTransform(D3DTS_WORLD,&CurMatrix);

			for(DWORD k=0;k<NumBoneBlend;k++)
			{
				g_pd3dDevice->GetTransform(D3DTS_WORLDMATRIX(k),&BlendMatrix[k]);
				//D3DXMatrixMultiply(&BlendMatrix[k],&BlendMatrix[k],&CurMatrix);
			}
			pEffect->m_HEffect->pEffect->SetMatrixArray("mWorldMatrixArray",BlendMatrix,NumBoneBlend);

			UINT cPass = 0;
			pEffect->m_HEffect->pEffect->Begin(&cPass,0);

			for (UINT iPass = 0; iPass < cPass; iPass++)
			{
				if( FAILED( hr = pEffect->m_HEffect->pEffect->BeginPass(iPass) ) )
					return DXTRACE_ERR( TEXT("m_pEffect->Pass"), hr );

				hr = pMesh->DrawSubset(AttribID);

				if( FAILED( hr ) )
					return DXTRACE_ERR( TEXT("m_pMesh->DrawSubset"), hr );
			}
			hr = pEffect->m_HEffect->pEffect->End();
		}
	}
	else
	{
		if (SUCCEEDED(SetMaterial(MaterialID)))
		{
			//SetShader();
			pMesh->DrawSubset(AttribID);
			RestoreMaterial(MaterialID);
		}
	}

	return S_OK;
}

HRESULT KModel::DrawMeshSubset(LPD3DXMESH pMesh,LPMATERIAL pMaterial,DWORD SubsetID)
{
	HRESULT hr = S_OK;

	if (pMaterial->m_lpEffect)
	{
		LPMTLMSEFFECT pEffect = pMaterial->m_lpEffect;
		if( pEffect->m_HEffect->pEffect)
		{
			pEffect->SetParameter(pMaterial,this);

			D3DXVECTOR3 Light(0,0,0);
			g_cGraphicsTool.GetSunDirection(Light);
			D3DXVECTOR4 Vec(Light.x,Light.y,Light.z,0);

			pEffect->m_HEffect->pEffect->SetVector("Light Direction",&Vec);

			UINT cPass = 0;
			pEffect->m_HEffect->pEffect->Begin(&cPass,D3DXFX_DONOTSAVESTATE);

			for (UINT iPass = 0; iPass < cPass; iPass++)
			{
				if( FAILED( hr = pEffect->m_HEffect->pEffect->BeginPass(iPass) ) )
					return DXTRACE_ERR( TEXT("m_pEffect->Pass"), hr ); 

				pEffect->m_HEffect->pEffect->CommitChanges();

				hr = pMesh->DrawSubset(SubsetID);

				if( FAILED( hr ) )
					return DXTRACE_ERR( TEXT("m_pMesh->DrawSubset"), hr );

				if( FAILED( hr = pEffect->m_HEffect->pEffect->EndPass() ) )
					return DXTRACE_ERR( TEXT("m_pEffect->EndPass"), hr );

			}
			hr = pEffect->m_HEffect->pEffect->End();
		}
	}
	else
	{
		if (SUCCEEDED(SetMaterial(SubsetID)))
		{
			//SetShader();
			pMesh->DrawSubset(SubsetID);
			RestoreMaterial(SubsetID);
		}
	}

	return S_OK;
}

HRESULT KModel::DrawMeshSubset(LPD3DXMESH pMesh,DWORD SubsetID)
{
	HRESULT hr = S_OK;
	LPMATERIAL pMaterial = &m_lpMaterial[SubsetID]; 
	return DrawMeshSubset(pMesh,pMaterial,SubsetID);
}

HRESULT KModel::SetMaterial(DWORD MaterialID)
{
	if (MaterialID>=m_dNumMaterial)
		return E_FAIL;

	if (m_dOption & MESH_NOMATERAIL)
	{
		g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
		g_pd3dDevice->SetTexture(0,NULL);
		g_pd3dDevice->SetTexture(1,NULL);
		g_pd3dDevice->SetTexture(2,NULL);
		g_pd3dDevice->SetTexture(3,NULL);
		return S_OK;
	}

	LPMATERIAL pMaterial = &m_lpMaterial[MaterialID]; 
	if (pMaterial->m_dOption & MATERIAL_OPTION_HIDE)
		return E_FAIL;

	if (pMaterial->m_dOption&MATERIAL_OPTION_CULL_NONE)
	{
		g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	}
	else if (pMaterial->m_dOption&MATERIAL_OPTION_CULL_CCW)
	{
		g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	}
	else
	{
		g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	}


	if (pMaterial->m_dOption & MATERIAL_OPTION_ZBUFFER_FALSE)
	{
		g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	}
	else if (pMaterial->m_dOption & MATERIAL_OPTION_ZBUFFER_USEW)
	{
		g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_USEW);
	}
	else/* (pMaterial->m_dOption & MATERIAL_OPTION_ZBUFFER_TRUE)*/
	{
		g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	}

	if (pMaterial->m_dOption & MATERIAL_OPTION_SHADE_GOURAUD)
	{
		g_pd3dDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
	}
	else if (pMaterial->m_dOption & MATERIAL_OPTION_SHADE_FLAT)
	{
		g_pd3dDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_FLAT);
	}
	else if (pMaterial->m_dOption & MATERIAL_OPTION_SHADE_PHONG)
	{
		g_pd3dDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_PHONG);
	}

	if (pMaterial->m_dOption & MATERIAL_OPTION_LIGHTING)
	{
		g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	}
	else
	{
		g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	}
	//g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	if (pMaterial->m_dOption & MATERIAL_OPTION_FILL_SOLID)
	{
		g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	}	
	else if (pMaterial->m_dOption & MATERIAL_OPTION_FILL_POINT)
	{
		g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_POINT);
	}
	else
	{
		g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	}


	if (pMaterial->m_dOption & MATERIAL_OPTION_SPECULARENABLE)
	{
		g_pd3dDevice->SetRenderState(D3DRS_SPECULARENABLE, TRUE);
	}
	else
	{
		g_pd3dDevice->SetRenderState(D3DRS_SPECULARENABLE, FALSE);
	}

	if (pMaterial->m_dOption & MATERIAL_OPTION_ZWRITEDISABLE)
	{
		g_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	}
	else
	{
		g_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	}
	
	g_pd3dDevice->SetMaterial(&pMaterial->m_sMaterial9); 

	g_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHAREF, 20);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	for(DWORD i_Option=0;i_Option<pMaterial->m_dwNumOptions;i_Option++)
	{
		LPOPTION pOpt = pMaterial->m_lpMatOptions[i_Option];
		SetOption(0,pOpt);
	}


	float t = timeGetTime()*2.0f;

	for(int i=0;i<8;i++)
	{
		if (pMaterial->m_dTextureIDs[i]!=0)
		{
			LPTEXTURE pTexture = NULL;

			if (SUCCEEDED(g_cTextureTable.GetTexture(&pTexture,pMaterial->m_dTextureIDs[i])))
			{
				pTexture->Update(1,t);
				g_pd3dDevice->SetTexture(i,pTexture->m_lpTexture);
				g_pd3dDevice->SetTextureStageState(i,D3DTSS_TEXCOORDINDEX,D3DTSS_TCI_PASSTHRU);
				g_pd3dDevice->SetTextureStageState(i,D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_DISABLE);
				g_pd3dDevice->SetTextureStageState(i,D3DTSS_COLOROP,D3DTOP_MODULATE);
				g_pd3dDevice->SetTextureStageState(i,D3DTSS_ALPHAOP,D3DTOP_MODULATE);
				g_pd3dDevice->SetSamplerState(i,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR);
				g_pd3dDevice->SetSamplerState(i,D3DSAMP_MINFILTER,D3DTEXF_LINEAR);

				for(DWORD i_TOption=0;i_TOption<pMaterial->m_lpTextureNumOption[i];i_TOption++)
				{
					LPOPTION pOpt = pMaterial->m_lpTextureOptions[i][i_TOption];
					SetOption(i,pOpt);
				}
			}
		}
		else
		{
			g_pd3dDevice->SetTexture(i,NULL);
			g_pd3dDevice->SetTextureStageState(i,D3DTSS_COLOROP,D3DTOP_DISABLE);
			g_pd3dDevice->SetTextureStageState(i,D3DTSS_ALPHAOP,D3DTOP_DISABLE);
			break;
		}
	}

	return S_OK;
}

HRESULT KModel::AddSocket(LPSTR pParentBoneName, LPSTR pSocket)
{
	int BoneIndex = FindBone(pParentBoneName); 
	if (BoneIndex<0) return E_FAIL;
	DWORD NewSocket = m_dNumSocket + 1;
	LPSOCKET pNewSockets = new KModel::SOCKET[NewSocket];
	for(DWORD i=0;i<m_dNumSocket;i++)
	{
		pNewSockets[i] = m_lpSockets[i];
	}
	pNewSockets[m_dNumSocket].scName = pSocket;
	pNewSockets[m_dNumSocket].scParentBoneName = pParentBoneName;

	LPBONEINFO pInfo = &m_lpBoneInfo[BoneIndex];
	D3DXMatrixIdentity(&pNewSockets[m_dNumSocket].MatrixOffset);
	//pNewSockets[m_dNumSocket].CurMatrix = pInfo->CurMatrix;

	D3DXMatrixMultiply(&pNewSockets[m_dNumSocket].CurMatrix,
		&pInfo->CurMatrix,
		&pNewSockets[m_dNumSocket].MatrixOffset);
	D3DXVECTOR3 A,B,C;
	A = D3DXVECTOR3(pNewSockets[m_dNumSocket].CurMatrix._11,pNewSockets[m_dNumSocket].CurMatrix._12,pNewSockets[m_dNumSocket].CurMatrix._13);
	B = D3DXVECTOR3(pNewSockets[m_dNumSocket].CurMatrix._21,pNewSockets[m_dNumSocket].CurMatrix._22,pNewSockets[m_dNumSocket].CurMatrix._23);
	C = D3DXVECTOR3(pNewSockets[m_dNumSocket].CurMatrix._31,pNewSockets[m_dNumSocket].CurMatrix._32,pNewSockets[m_dNumSocket].CurMatrix._33);

	float Sx,Sy,Sz;
	Sx = D3DXVec3Length(&A);
	Sy = D3DXVec3Length(&B);
	Sz = D3DXVec3Length(&C);
	D3DXMATRIX matScale;
	D3DXMatrixScaling(&matScale,Sx,Sy,Sz);
	D3DXMatrixInverse(&matScale,NULL,&matScale);
	D3DXMatrixMultiply(&pNewSockets[m_dNumSocket].CurMatrix,
		&matScale,
		&pInfo->CurMatrix);

	delete [] m_lpSockets;
	m_lpSockets = pNewSockets;
	m_dNumSocket++;

	return S_OK;
}

HRESULT KModel::DrawSocket()
{
	for(DWORD i_Socket=0;i_Socket<m_dNumSocket;i_Socket++)
	{
		LPSOCKET pSocket = &m_lpSockets[i_Socket];
		D3DXVECTOR3 A = D3DXVECTOR3(0,0,0);

		int Index = FindBone((LPSTR)pSocket->scParentBoneName.c_str()); 
		if (Index>=0)
		{
			LPBONEINFO pInfoParent = &m_lpBoneInfo[Index];

			/*
			D3DXVECTOR3 A,B,C;
			float Sx,Sy,Sz;


			D3DXMATRIX mat;
			A = D3DXVECTOR3(pInfoParent->CurMatrix._11,pInfoParent->CurMatrix._12,pInfoParent->CurMatrix._13);
			B = D3DXVECTOR3(pInfoParent->CurMatrix._21,pInfoParent->CurMatrix._22,pInfoParent->CurMatrix._23);
			C = D3DXVECTOR3(pInfoParent->CurMatrix._31,pInfoParent->CurMatrix._32,pInfoParent->CurMatrix._33);
			Sx = D3DXVec3Length(&A);
			Sy = D3DXVec3Length(&B);
			Sz = D3DXVec3Length(&C);
			D3DXMATRIX matScale;
			D3DXMatrixScaling(&matScale,Sx,Sy,Sz);
			D3DXMatrixInverse(&matScale,NULL,&matScale);
			D3DXMatrixMultiply(&mat,
			&matScale,
			&pInfoParent->CurMatrix);

			D3DXMatrixMultiply(&pSocket->CurMatrix,
			&mat,
			&pSocket->MatrixOffset);*/

			A = D3DXVECTOR3(0,0,0);
			//D3DXVec3TransformCoord(&A,&A,&pInfoParent->CurMatrix);
			D3DXVec3TransformCoord(&A,&A,&pSocket->CurMatrix);

			D3DXVECTOR3 M(0,0,150);
			A += M;


			D3DXVECTOR3 Axis_X,Axis_Y,Axis_Z;
			D3DXVECTOR3 XXX(60,0,0);
			D3DXVECTOR3 YYY(0,60,0);
			D3DXVECTOR3 ZZZ(0,0,60);
			D3DXVec3TransformCoord(&Axis_X,&XXX,&pSocket->CurMatrix);
			D3DXVec3TransformCoord(&Axis_Y,&YYY,&pSocket->CurMatrix);
			D3DXVec3TransformCoord(&Axis_Z,&ZZZ,&pSocket->CurMatrix);

			//XXX = Axis_X*0.1f + M + Axis_Y;
			XXX = Axis_X + M;
			YYY = Axis_Y + M;
			ZZZ = Axis_Z + M;

			g_cGraphicsTool.DrawLine(&XXX,&A,0xffff0000,0xffff0000); 
			g_cGraphicsTool.DrawLine(&YYY,&A,0xff00ff00,0xff00ff00); 
			//g_cGraphicsTool.DrawLine(&YYY,&XXX,0xffffffff,0xffffffff); 
			g_cGraphicsTool.DrawLine(&ZZZ,&A,0xffffffff,0xffffffff); 
			//g_cGraphicsTool.DrawLine(&ZZZ,&A,0xff0000ff,0xffffffff); 

			//g_cGraphicsTool.DrawLine(&pSocket->CurDirectionPosition,&A,0xFFFF00FF,0xFF00FFFF); 
			
			//D3DXVECTOR3 XBase(1,0,0);
			//D3DXVec3TransformNormal(&XBase,&XBase,&pSocket->MatrixBase);
			//D3DXVec3Normalize(&XBase,&XBase);
			//g_cGraphicsTool.DrawLine(&(A + XBase*50),&A,0xFFFF0000,0xFFFF0000); 

			//D3DXVECTOR3 YBase(0,1,0);
			//D3DXVec3TransformNormal(&YBase,&YBase,&pSocket->MatrixBase);
			//D3DXVec3Normalize(&YBase,&YBase);
			//g_cGraphicsTool.DrawLine(&(A + YBase*50),&A,0xFF00FF00,0xFF00FF00); 

			//D3DXVECTOR3 ZBase(0,0,1);
			//D3DXVec3TransformNormal(&ZBase,&ZBase,&pSocket->MatrixBase);
			//D3DXVec3Normalize(&ZBase,&ZBase);
			//g_cGraphicsTool.DrawLine(&(A + ZBase*50),&A,0xFF0000FF,0xFF0000FF); 


			//D3DXVECTOR3 YCur = pSocket->CurDirectionPosition - pSocket->CurPosition;
			//D3DXVec3Normalize(&YCur,&YCur);

			//D3DXVECTOR3 Cross(0,0,0);
			//D3DXVec3Cross(&Cross,&YBase,&YCur);

			//g_cGraphicsTool.DrawLine(&(A + Cross*50),&A,0xFFFF0000,0xFF0000FF); 

		}
	}
	return S_OK;
}

HRESULT KModel::DeleteSocket(LPCSTR SocketName)
{
	for(DWORD i=0;i<m_dNumSocket;i++)
	{
		LPSOCKET pSocket = &m_lpSockets[i];
		if (strcmp(SocketName,pSocket->scName.c_str())==0)
		{
			for(DWORD j=i;j<m_dNumSocket-1;j++)
			{
				m_lpSockets[j] = m_lpSockets[j+1];
			}
			m_dNumSocket--;
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT KModel::Bind(LPMODEL pXMesh,LPSTR SocketName)
{
	if (pXMesh==NULL)
		return E_FAIL;
	if (pXMesh==this)
		return E_FAIL;
	m_pBindMesh = pXMesh;
	if (m_pBindSocketName==NULL)
		m_pBindSocketName = new TCHAR[30];
	wsprintf(m_pBindSocketName,"%s",SocketName);
	m_dOption |= MESH_BIND;
	return S_OK;
}

HRESULT KModel::UnBind()
{
	m_pBindMesh = NULL;
	if (m_pBindSocketName!=NULL)
		delete [] m_pBindSocketName;
	m_pBindSocketName = NULL;
	m_dOption &= ~MESH_BIND;
	return S_OK;
}

BOOL KModel::IsCurAnimationEnd()
{
	float NowTime = 0;
	LPSCENE pScene = g_cGraphicsTool.GetCurScene();
	if(pScene)
	{
		NowTime = pScene->m_TimeControl.GetNowTime();
	}
	else
	{
		NowTime = (float)timeGetTime();
	}

	if(m_nCurAnimationIndex>=0)
	{
		float L = NowTime - m_fCurAnimationStart;
		LPANIMATION pAni = m_lpAnimations[m_nCurAnimationIndex];
		if(L > (pAni->FrameLength*pAni->NumFrames)/m_fCurAnimationSpeed)
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	return FALSE;
}

HRESULT KModel::PlayAnimation(LPSTR AnimationName, DWORD PlayMethod, float Speed)
{
	m_bPlaySequence = FALSE;
	int AniIndex = FindAnimation(AnimationName);
	if (AniIndex<0)
		return E_FAIL;
	m_nCurAnimationIndex = AniIndex;
	m_dCurAnimationMethod = PlayMethod;
	m_fCurAnimationSpeed = Speed;
	LPSCENE pScene = g_cGraphicsTool.GetCurScene();
	if(pScene)
	{
		m_fCurAnimationStart = pScene->m_TimeControl.GetNowTime();
	}
	else
	{
		m_fCurAnimationStart = timeGetTime()*1.0f;
	}
	return S_OK;
}

HRESULT KModel::PlayAnimation(int AniIndex, DWORD PlayMethod, float Speed)
{
	m_bPlaySequence = FALSE;
	if (AniIndex<0)
		return E_FAIL;
	m_nCurAnimationIndex = AniIndex;
	m_dCurAnimationMethod = PlayMethod;
	m_fCurAnimationSpeed = Speed;
	LPSCENE pScene = g_cGraphicsTool.GetCurScene();
	if(pScene)
	{
		m_fCurAnimationStart = pScene->m_TimeControl.GetNowTime();
	}
	else
	{
		m_fCurAnimationStart = timeGetTime()*1.0f;
	}

	return S_OK;
}

void KModel::GetCurFrame(int* pnFrame,int* pnFrameTo,float* S)
{
	if ( !m_bPlaySequence )
	{
		float Time = 0;
		LPSCENE pScene = g_cGraphicsTool.GetCurScene();
		if(pScene)
		{
			Time = (pScene->m_TimeControl.GetNowTime() - m_fCurAnimationStart)*m_fCurAnimationSpeed;
		}
		else
		{
			Time = (timeGetTime()-m_fCurAnimationStart)*m_fCurAnimationSpeed;
		}

		if ((m_nCurAnimationIndex<0)||((DWORD)m_nCurAnimationIndex>=m_lpAnimations.size()))
			return ;
		LPANIMATION pAni = m_lpAnimations[m_nCurAnimationIndex];

		if (m_dCurAnimationMethod == PLAY_NONE)
		{
			if (Time>pAni->FrameLength*pAni->NumFrames)
			{
				*pnFrame = pAni->NumFrames - 1;
				*pnFrameTo = pAni->NumFrames - 1;
			}
			else
			{
				(*pnFrame) = (int)(Time/pAni->FrameLength);
				if ((DWORD) *pnFrame>=pAni->NumFrames) *pnFrame = pAni->NumFrames - 1;
				*pnFrameTo = *pnFrame+1;
				if ( (DWORD) *pnFrameTo>=pAni->NumFrames) *pnFrameTo = pAni->NumFrames - 1;
				*S = (Time - *pnFrame * pAni->FrameLength)/pAni->FrameLength;
			}

		}
		else if (m_dCurAnimationMethod==PLAY_CIRCLE)
		{
			int T = (int) (Time/(pAni->FrameLength * (pAni->NumFrames -1)));
			float m_Time = Time - (pAni->FrameLength * (pAni->NumFrames-1)) * T;
			(*pnFrame) = (int)(m_Time/pAni->FrameLength);
			if ((DWORD)*pnFrame>=pAni->NumFrames) 
				*pnFrame = 0;
			*pnFrameTo = *pnFrame + 1;
			
			if ( (DWORD)*pnFrameTo >= pAni->NumFrames)
				*pnFrameTo = 0;
			*S = (m_Time - *pnFrame * pAni->FrameLength) / pAni->FrameLength;
		}
		else if (m_dCurAnimationMethod==PLAY_PINGPANG)
		{
			int T = (int) (Time/(pAni->FrameLength * pAni->NumFrames));
			float m_Time = Time - (pAni->FrameLength*pAni->NumFrames) * T;
			if (T%2==0)
			{
				(*pnFrame) = (int)(m_Time/pAni->FrameLength);
				if ((DWORD) *pnFrame>=pAni->NumFrames) *pnFrame = pAni->NumFrames - 1;
				*pnFrameTo = *pnFrame+1;
				if ( (DWORD) *pnFrameTo>=pAni->NumFrames) *pnFrameTo = pAni->NumFrames - 1;
				*S = (m_Time - *pnFrame * pAni->FrameLength)/pAni->FrameLength;
			}
			else
			{
				int K =  (int)(m_Time/pAni->FrameLength);
				(*pnFrame) = pAni->NumFrames - K ;

				if ((DWORD) *pnFrame>=pAni->NumFrames) *pnFrame = pAni->NumFrames - 1;
				*pnFrameTo = *pnFrame - 1;
				if ( (DWORD) *pnFrameTo<0) *pnFrameTo = 0;
				*S = (m_Time - K * pAni->FrameLength)/pAni->FrameLength;
			}
		}
		else if (m_dCurAnimationMethod==PLAY_HOLD)
		{
			Time = m_fCurAnimationStart;
			if (Time>pAni->FrameLength*pAni->NumFrames)
			{
				*pnFrame = pAni->NumFrames - 1;
				*pnFrameTo = pAni->NumFrames - 1;
			}
			else
			{
				(*pnFrame) = (int)(Time/pAni->FrameLength);
				if ((DWORD) *pnFrame>=pAni->NumFrames) *pnFrame = pAni->NumFrames - 1;
				*pnFrameTo = *pnFrame+1;
				if ( (DWORD) *pnFrameTo>=pAni->NumFrames) *pnFrameTo = pAni->NumFrames - 1;
				*S = (Time - *pnFrame * pAni->FrameLength)/pAni->FrameLength;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	//Add 6-14-2005 Animation sequence
	else
	{
		
	}
	//Add 6-14-2005 Animation sequence end
	//////////////////////////////////////////////////////////////////////////
}

HRESULT KModel::ClearnAnimations()
{
	//Clean Animation data
	for(DWORD i=0;i<(int)m_lpAnimations.size();i++)
	{
		LPANIMATION pAni = m_lpAnimations[i];
		SAFE_DELETE(pAni);
	}
	m_lpAnimations.clear();
	m_dNumAnimation = 0;

	m_nCurAnimationIndex = -1;

	return S_OK;
}
HRESULT KModel::CleanUp()
{
	//////////////////////////////////////////////////////////////////////////
	//Add 6-14-2006 Animation Sequence
	//ClearSequence();
	//Add 6-14-2006 Animation Sequence end
	//////////////////////////////////////////////////////////////////////////
	SAFE_DELETE( m_pCurSeq );
	ClearAnimationTracer();

	/////////////////////////////////////////////////////////////////////////////
	for(int i_Flex=0;i_Flex<(int)m_vecStaticFlexibleBoneIndex.size();i_Flex++)
	{
		//m_vecStaticFlexibleBoneIndex[i_Flex].ropeSimulation->release();		// Release The ropeSimulation
		//delete(m_vecStaticFlexibleBoneIndex[i_Flex].ropeSimulation);			// Delete The ropeSimulation
		m_vecStaticFlexibleBoneIndex[i_Flex].ropeSimulation = NULL;	
	}

	ClearnAnimations();

	SAFE_DELETE_ARRAY(m_lpBoneInfo);

	m_dNumBone = 0;

	SAFE_DELETE_ARRAY(m_lpSockets);
	m_dNumSocket = 0;


	SAFE_DELETE_ARRAY(m_pBoneMatrices);
	SAFE_RELEASE(m_pBoneCombinationTable);
	/////////////////////////////////////////////////////////////////////////////
	//Clean Material data
	SAFE_DELETE_ARRAY(m_lpMaterial);

	m_dNumMaterial = 0;
	/////////////////////////////////////////////////////////////////////////////
	//Clean Mesh data
	//SAFE_DELETE_ARRAY(m_lpStaticVertexPos);
	SAFE_RELEASE(m_pBlendMesh);
	SAFE_RELEASE(m_pMeshOpt);
	SAFE_RELEASE(m_pMeshSys);
	SAFE_RELEASE(m_pVertexRemap);

	for(int s=0;s<(int)m_lpBoneBlendSet.size();s++)
	{
		SAFE_DELETE(m_lpBoneBlendSet[s]);
	}

	m_nNumCurBlendSet = 0;
	m_lpBoneBlendSet.clear();

	SAFE_DELETE_ARRAY(m_pBindSocketName);

	SAFE_RELEASE(m_pBoneCombinationTable);
	SAFE_DELETE_ARRAY(m_pBoneMatrices);

	SAFE_RELEASE(m_pSkinInfo);

	//////////////////////////////////////////////////////////////////////////
	//For PRT
	SAFE_DELETE_ARRAY(m_aClusterBases);
	SAFE_DELETE_ARRAY(m_aClusteredPCA);
	SAFE_DELETE_ARRAY(m_aPRTConstants);

	SAFE_RELEASE(m_pMeshPRT);

#if D3D_SDK_VERSION == 31 //9.0b
	SAFE_RELEASE(m_pSHPRTBuffer);
	SAFE_RELEASE(m_pSHPRTEffect);
#endif
#if D3D_SDK_VERSION == 32 //9.0c

	//SAFE_RELEASE( m_pMaterialBuffer );
	SAFE_RELEASE( m_pPRTBuffer );
	SAFE_RELEASE( m_pPRTCompBuffer );
	SAFE_RELEASE( m_pPRTEffect );
	SAFE_RELEASE( m_pSHIrradEnvMapEffect );
	SAFE_RELEASE( m_pNDotLEffect );

#endif
	//////////////////////////////////////////////////////////////////////////

	SAFE_RELEASE(m_pVBNormal);

	SAFE_RELEASE(m_pMeshProgress);
	SAFE_RELEASE(m_pMeshBeforeProgress);
	SAFE_DELETE_ARRAY(m_pVerticesLodWeight);
	SAFE_DELETE_ARRAY(m_pBondID);

#ifdef KEY_FRAME_INTERPOLATION_BUG
	SAFE_DELETE_ARRAY(m_pFrameA);
	SAFE_DELETE_ARRAY(m_pFrameB);
#endif
	//ClearSequence();
	return S_OK;
}

HRESULT KModel::CheckUpMaterial()
{
	DWORD * pMaterialRemap = new DWORD[m_dNumMaterial];
	ZeroMemory(pMaterialRemap,sizeof(DWORD)*m_dNumMaterial);
	LPMATERIAL pTempMat = new MATERIAL[m_dNumMaterial];
	ZeroMemory(pTempMat,sizeof(MATERIAL)*m_dNumMaterial);
	DWORD NewNumMaterial = 0;
	for(DWORD i=0;i<m_dNumMaterial;i++)
	{
		LPMATERIAL pMat = &m_lpMaterial[i];
		for(DWORD j=0;j<NewNumMaterial;j++)
		{
			LPMATERIAL pMatUN = &pTempMat[j];

			int result1 = memcmp( &pMatUN->m_sMaterial9,&pMat->m_sMaterial9, sizeof(D3DMATERIAL9));
			int result2 = memcmp(  pMatUN->m_dTextureIDs, pMat->m_dTextureIDs, sizeof(DWORD)*8);

			if ((result1==0)&&(result2==0))
			{
				pMaterialRemap[i] = j;
				break;
			}
		}
		if (j>=NewNumMaterial)
		{
			pTempMat[NewNumMaterial].m_dOption = pMat->m_dOption;
			pTempMat[NewNumMaterial].m_sMaterial9 = pMat->m_sMaterial9;
			for(int k=0;k<8;k++)
			{
				pTempMat[NewNumMaterial].m_dTextureIDs[k] = pMat->m_dTextureIDs[k];
				pTempMat[NewNumMaterial].m_scTextureName[k] = pMat->m_scTextureName[k];
				pTempMat[NewNumMaterial].m_scTextureType[k] = pMat->m_scTextureType[k];
			}
			pMaterialRemap[i] = NewNumMaterial;
			NewNumMaterial++;
		}
	}

	DWORD * pAttrib = NULL;
	DWORD  NumFaces = m_pMeshSys->GetNumFaces();

	if (FAILED(m_pMeshSys->LockAttributeBuffer(0,(DWORD**)&pAttrib)))
		return E_FAIL;
	for(i=0;i<NumFaces;i++)
	{
		pAttrib[i] = pMaterialRemap[pAttrib[i]];
	}
	if (FAILED(m_pMeshSys->UnlockAttributeBuffer()))
		return E_FAIL;

	SAFE_DELETE_ARRAY(m_lpMaterial);
	m_lpMaterial = pTempMat;
	m_dNumMaterial = NewNumMaterial;
	SAFE_DELETE_ARRAY(pMaterialRemap);

	if (m_dOption&MESH_SKINED)
	{
		CreateSkinBlendedMesh(FALSE);
	}
	if (m_dOption&MESH_OPTIMIZED)
	{
		OptimizeUnSkinedMesh();
	}
	return S_OK;
}

HRESULT KModel::CreateSkinBlendedMesh(BOOL bLoad)
{
	HRESULT hr = S_OK;

	LPD3DXBUFFER m_pAdjacencyBuffer = NULL;   // Contains the adjaceny info loaded with the mesh
	if ( !bLoad )
	{
		if ( FAILED( hr = D3DXCreateBuffer(m_pMeshSys->GetNumFaces()*3*sizeof(DWORD),
			&m_pAdjacencyBuffer)))
			return hr;
		if ( FAILED( hr = m_pMeshSys->GenerateAdjacency(0,
			(DWORD*)m_pAdjacencyBuffer->GetBufferPointer())))
			return hr;

		SAFE_RELEASE(m_pBlendMesh);
		SAFE_RELEASE(m_pBoneCombinationTable);

		hr = m_pSkinInfo->ConvertToBlendedMesh(
			m_pMeshSys,
			D3DXMESH_MANAGED|D3DXMESHOPT_VERTEXCACHE, 
			(DWORD*)m_pAdjacencyBuffer->GetBufferPointer(), 
			NULL, NULL, NULL, //&pRemap, 
			&m_MaxFaceInfl,
			&m_NumAttributeGroups, 
			&m_pBoneCombinationTable, 
			&m_pBlendMesh);
		if (FAILED(hr))
			return hr;

		/*D3DVERTEXELEMENT9 decl[MAX_FVF_DECL_SIZE];
		pBlendMesh->GetDeclaration(decl);
		SAFE_RELEASE(m_pBlendMesh);
		D3DXCreateMesh(pBlendMesh->GetNumFaces(), pBlendMesh->GetNumVertices(), pBlendMesh->GetOptions(), decl, 
			g_pd3dDevice, &m_pBlendMesh );

		DWORD* AttBuffer = NULL;
		BYTE* VertexBuffer = NULL;
		BYTE* IndexBuffer = NULL;

		DWORD* AttBufferSrc = NULL;
		BYTE* VertexBufferSrc = NULL;
		BYTE* IndexBufferSrc = NULL;

		m_pBlendMesh->LockAttributeBuffer(0, &AttBuffer);
		m_pBlendMesh->LockIndexBuffer(D3DLOCK_DISCARD, (LPVOID*)&IndexBuffer);
		m_pBlendMesh->LockVertexBuffer(D3DLOCK_DISCARD, (LPVOID*)&VertexBuffer);


		pBlendMesh->LockAttributeBuffer(0, &AttBufferSrc);
		pBlendMesh->LockIndexBuffer(D3DLOCK_DISCARD, (LPVOID*)&IndexBufferSrc);
		pBlendMesh->LockVertexBuffer(D3DLOCK_DISCARD, (LPVOID*)&VertexBufferSrc);

		LPDIRECT3DINDEXBUFFER9 pIndexBuffer = NULL;
		LPDIRECT3DVERTEXBUFFER9 pVertexBuffer = NULL;

		pBlendMesh->GetVertexBuffer(&pVertexBuffer);
		pBlendMesh->GetIndexBuffer(&pIndexBuffer);

		D3DVERTEXBUFFER_DESC vbDesc;
		D3DINDEXBUFFER_DESC  ibDesc;

		pVertexBuffer->GetDesc(&vbDesc);
		pIndexBuffer->GetDesc(&ibDesc);

		memcpy(AttBuffer, AttBufferSrc, sizeof(DWORD) * m_pBlendMesh->GetNumFaces());
		memcpy(IndexBuffer, IndexBufferSrc, sizeof(BYTE) * vbDesc.Size );
		memcpy(VertexBuffer, VertexBufferSrc, sizeof(BYTE) * ibDesc.Size);
		
		DWORD dwAttSize;
		D3DXATTRIBUTERANGE* pAttRange = 0;
		pBlendMesh->GetAttributeTable(0, &dwAttSize);
		pAttRange = new D3DXATTRIBUTERANGE[dwAttSize];
		pBlendMesh->GetAttributeTable(pAttRange, &dwAttSize);
		m_pBlendMesh->SetAttributeTable(pAttRange, dwAttSize);
		pBlendMesh->Release();*/
		/*m_pSkinInfo->Release();
		pBoneCombinationTable->Release();
		pBlendMesh->Release();
		pSkinInfo->Remap(m_pBlendMesh->GetNumVertices(), (DWORD*)pRemap->GetBufferPointer());
		m_pSkinInfo = pSkinInfo;*/
		
	}


	/* If the device can only do 2 matrix blends, ConvertToBlendedMesh cannot approximate all meshes to it
	Thus we split the mesh in two parts: The part that uses at most 2 matrices and the rest. The first is
	drawn using the device's HW vertex processing and the rest is drawn using SW vertex processing. */

	LPD3DXBONECOMBINATION rgBoneCombinations  = reinterpret_cast<LPD3DXBONECOMBINATION>(m_pBoneCombinationTable->GetBufferPointer());

	// look for any set of bone combinations that do not fit the caps
	for (m_iAttributeSW = 0; m_iAttributeSW < m_NumAttributeGroups; m_iAttributeSW++)
	{
		DWORD cInfl   = 0;
		for (DWORD iInfl = 0; iInfl < m_MaxFaceInfl; iInfl++)
		{
			if (rgBoneCombinations[m_iAttributeSW].BoneId[iInfl] != UINT_MAX)
			{
				++cInfl;
			}
		}
		if (cInfl > g_D3DCaps.MaxVertexBlendMatrices)
		{
			break;
		}
	}

	// if there is both HW and SW, add the Software Processing flag
	if (m_iAttributeSW < m_NumAttributeGroups)
	{
		LPD3DXMESH pMeshTmp;

		hr = m_pBlendMesh->CloneMeshFVF(D3DXMESH_SOFTWAREPROCESSING|m_pBlendMesh->GetOptions(), 
			m_pBlendMesh->GetFVF(),
			g_pd3dDevice, &pMeshTmp);
		if (FAILED(hr))
		{
			return hr;
		}

		m_pBlendMesh->Release();
		m_pBlendMesh = pMeshTmp;
		pMeshTmp = NULL;
	}

	SAFE_RELEASE(m_pAdjacencyBuffer);

	SortSkinSubset();

	//SAFE_RELEASE(m_pSkinInfo);
	return S_OK;
}

HRESULT KModel::OptimizeUnSkinedMesh()
{
	if (m_dOption&MESH_SKINED)
		return S_OK;
	HRESULT hr = S_OK;

	LPD3DXMESH pMeshClone = NULL;

	const D3DVERTEXELEMENT9 g_VBDecl_Geometry[] =
	{
		{0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0},
		{0, 24, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		{0, 36, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1},
		{0, 48, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT,  0},
		{0, 60, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0},
		D3DDECL_END()
	};

	if(FAILED(m_pMeshSys->CloneMesh(m_pMeshSys->GetOptions(),g_VBDecl_Geometry,
		g_pd3dDevice,&pMeshClone)))
	{
		return E_FAIL;
	}

	//Optimize mesh	
	// force 32 byte vertices
	LPD3DXBUFFER m_pAdjacencyBuffer = NULL;   // Contains the adjaceny info loaded with the mesh
	if ( FAILED( hr = D3DXCreateBuffer(m_pMeshSys->GetNumFaces()*3*sizeof(DWORD),
		&m_pAdjacencyBuffer)))
		return hr;
	if ( FAILED( hr = m_pMeshSys->GenerateAdjacency(0,
		(DWORD*)m_pAdjacencyBuffer->GetBufferPointer())))
		return hr;

	if(FAILED(D3DXComputeTangent(pMeshClone,0,0,0,0,(DWORD*)m_pAdjacencyBuffer->GetBufferPointer())))
	{
		return E_FAIL;
	}

	LPD3DXMESH m_pMeshOptimized = NULL;
	hr = pMeshClone->Optimize( D3DXMESHOPT_VERTEXCACHE|D3DXMESH_MANAGED,
		(DWORD*)m_pAdjacencyBuffer->GetBufferPointer(),
		NULL, NULL, &m_pVertexRemap, &m_pMeshOptimized);

	if ( FAILED(hr) )
		return hr;
	SAFE_RELEASE(m_pMeshOpt);
	SAFE_RELEASE(pMeshClone);

	m_pMeshOpt = m_pMeshOptimized;

	SAFE_RELEASE(m_pAdjacencyBuffer);

	m_dOption|=MESH_OPTIMIZED;

	return S_OK;
}


HRESULT KModel::SetOption(DWORD TextureStage,LPOPTION pOption)
{
	HRESULT hr = S_OK;

	switch(pOption->Type)
	{

	case OPTION_MATERIAL_CULLING:
		{
			LPMATCULL pMatCull = (LPMATCULL) pOption->pData;
			g_pd3dDevice->GetRenderState(D3DRS_CULLMODE,&pMatCull->BakCullMode);
			g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, pMatCull->CullMode);
			break;
		}
	case OPTION_MATERIAL_ALPHABLEND:
		{
			LPMATBLEND pMatBlend = (LPMATBLEND) pOption->pData;
			g_pd3dDevice->GetRenderState(D3DRS_SRCBLEND, &pMatBlend->BakSrcBlend);
			g_pd3dDevice->GetRenderState(D3DRS_DESTBLEND,&pMatBlend->BakDestBlend);
			g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND,  pMatBlend->SrcBlend);
			g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, pMatBlend->DestBlend);
			g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
			break;
		}
	case OPTION_MATERIAL_ALPHAOPERATION:
		{
			LPMTLALPHAOPERATION pMtlAlphaOp = (LPMTLALPHAOPERATION) pOption->pData;
			g_pd3dDevice->GetRenderState(D3DRS_ALPHABLENDENABLE,         &pMtlAlphaOp->BackEnableAlphaBlend);
			g_pd3dDevice->GetRenderState(D3DRS_SRCBLEND,                 &pMtlAlphaOp->BackSrcBlend);
			g_pd3dDevice->GetRenderState(D3DRS_DESTBLEND,                &pMtlAlphaOp->BackDestBlend);
			g_pd3dDevice->GetRenderState(D3DRS_BLENDOP,                  &pMtlAlphaOp->BackBlendOperator);
			g_pd3dDevice->GetRenderState(D3DRS_BLENDFACTOR ,             &pMtlAlphaOp->BackBlendFactor);

			g_pd3dDevice->GetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, &pMtlAlphaOp->BackEnableSeparateAlphaBlend);
			g_pd3dDevice->GetRenderState(D3DRS_BLENDOPALPHA,             &pMtlAlphaOp->BackBlendOperatorAlpha);
			g_pd3dDevice->GetRenderState(D3DRS_SRCBLENDALPHA,            &pMtlAlphaOp->BackSrcBlendAlpha);
			g_pd3dDevice->GetRenderState(D3DRS_DESTBLENDALPHA,           &pMtlAlphaOp->BackDestBlendAlpha);

			g_pd3dDevice->GetRenderState(D3DRS_ALPHATESTENABLE,          &pMtlAlphaOp->BackEnableAlphaTest);
			g_pd3dDevice->GetRenderState(D3DRS_ALPHAREF,                 &pMtlAlphaOp->BackAlphaTestRef);
			g_pd3dDevice->GetRenderState(D3DRS_ALPHAFUNC,                &pMtlAlphaOp->BackAlphaFunction);

			g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,          pMtlAlphaOp->EnableAlphaBlend);
			g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND,                  pMtlAlphaOp->SrcBlend);
			g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND,                 pMtlAlphaOp->DestBlend);
			g_pd3dDevice->SetRenderState(D3DRS_BLENDOP,                   pMtlAlphaOp->BlendOperator);
			g_pd3dDevice->SetRenderState(D3DRS_BLENDFACTOR ,              pMtlAlphaOp->BlendFactor);

			g_pd3dDevice->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE,  pMtlAlphaOp->EnableSeparateAlphaBlend);
			g_pd3dDevice->SetRenderState(D3DRS_BLENDOPALPHA,              pMtlAlphaOp->BlendOperatorAlpha);
			g_pd3dDevice->SetRenderState(D3DRS_SRCBLENDALPHA,             pMtlAlphaOp->SrcBlendAlpha);
			g_pd3dDevice->SetRenderState(D3DRS_DESTBLENDALPHA,            pMtlAlphaOp->DestBlendAlpha);

			g_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE,           pMtlAlphaOp->EnableAlphaTest);
			g_pd3dDevice->SetRenderState(D3DRS_ALPHAREF,                  pMtlAlphaOp->AlphaTestRef);
			g_pd3dDevice->SetRenderState(D3DRS_ALPHAFUNC,                 pMtlAlphaOp->AlphaFunction);

			break;
		}
	case OPTION_TEXTURE_OPERATION:
		{
			LPTEXTUREOP pTextureOp = (LPTEXTUREOP) pOption->pData;
			g_pd3dDevice->GetTextureStageState(TextureStage,D3DTSS_COLOROP,&pTextureOp->BakColorOp);
			g_pd3dDevice->GetTextureStageState(TextureStage,D3DTSS_ALPHAOP,&pTextureOp->BakAlphaOp);
			g_pd3dDevice->GetTextureStageState(TextureStage,D3DTSS_COLORARG1,&pTextureOp->BakColorArg1);
			g_pd3dDevice->GetTextureStageState(TextureStage,D3DTSS_COLORARG2,&pTextureOp->BakColorArg2);
			g_pd3dDevice->GetTextureStageState(TextureStage,D3DTSS_ALPHAARG1,&pTextureOp->BakAlphaArg1);
			g_pd3dDevice->GetTextureStageState(TextureStage,D3DTSS_ALPHAARG2,&pTextureOp->BakAlphaArg2);
			//g_pd3dDevice->GetRenderState(D3DRS_BLENDFACTOR,&pTextureOp->BakTFactor);

			g_pd3dDevice->SetTextureStageState(TextureStage,D3DTSS_COLOROP,pTextureOp->ColorOp);
			g_pd3dDevice->SetTextureStageState(TextureStage,D3DTSS_ALPHAOP,pTextureOp->AlphaOp);
			g_pd3dDevice->SetTextureStageState(TextureStage,D3DTSS_COLORARG1,pTextureOp->ColorArg1);
			g_pd3dDevice->SetTextureStageState(TextureStage,D3DTSS_COLORARG2,pTextureOp->ColorArg2);
			g_pd3dDevice->SetTextureStageState(TextureStage,D3DTSS_ALPHAARG1,pTextureOp->AlphaArg1);
			g_pd3dDevice->SetTextureStageState(TextureStage,D3DTSS_ALPHAARG2,pTextureOp->AlphaArg2);
			//g_pd3dDevice->SetRenderState(D3DRS_BLENDFACTOR,pTextureOp->TFactor);
			break;
		}
	case OPTION_TEXTURE_OPERATIONEX:
		{
			LPTEXTUREOPEX pTextureOpEx = (LPTEXTUREOPEX) pOption->pData;
			g_pd3dDevice->GetTextureStageState(TextureStage,D3DTSS_COLOROP,&pTextureOpEx->BakColorOp);
			g_pd3dDevice->GetTextureStageState(TextureStage,D3DTSS_ALPHAOP,&pTextureOpEx->BakAlphaOp);
			g_pd3dDevice->GetTextureStageState(TextureStage,D3DTSS_COLORARG1,&pTextureOpEx->BakColorArg1);
			g_pd3dDevice->GetTextureStageState(TextureStage,D3DTSS_COLORARG2,&pTextureOpEx->BakColorArg2);
			g_pd3dDevice->GetTextureStageState(TextureStage,D3DTSS_ALPHAARG1,&pTextureOpEx->BakAlphaArg1);
			g_pd3dDevice->GetTextureStageState(TextureStage,D3DTSS_ALPHAARG2,&pTextureOpEx->BakAlphaArg2);
			g_pd3dDevice->GetTextureStageState(TextureStage,D3DTSS_ALPHAARG0,&pTextureOpEx->BakAlphaArg0);
			g_pd3dDevice->GetTextureStageState(TextureStage,D3DTSS_COLORARG0,&pTextureOpEx->BakColorArg0);
			g_pd3dDevice->GetTextureStageState(TextureStage,D3DTSS_RESULTARG,&pTextureOpEx->BakResult);
			g_pd3dDevice->GetTextureStageState(TextureStage,D3DTSS_CONSTANT ,&pTextureOpEx->BakConstant);
			//g_pd3dDevice->GetRenderState(D3DRS_BLENDFACTOR,&pTextureOp->BakTFactor);

			g_pd3dDevice->SetTextureStageState(TextureStage,D3DTSS_COLOROP,pTextureOpEx->ColorOp);
			g_pd3dDevice->SetTextureStageState(TextureStage,D3DTSS_ALPHAOP,pTextureOpEx->AlphaOp);
			g_pd3dDevice->SetTextureStageState(TextureStage,D3DTSS_COLORARG1,pTextureOpEx->ColorArg1);
			g_pd3dDevice->SetTextureStageState(TextureStage,D3DTSS_COLORARG2,pTextureOpEx->ColorArg2);
			g_pd3dDevice->SetTextureStageState(TextureStage,D3DTSS_ALPHAARG1,pTextureOpEx->AlphaArg1);
			g_pd3dDevice->SetTextureStageState(TextureStage,D3DTSS_ALPHAARG2,pTextureOpEx->AlphaArg2);
			g_pd3dDevice->SetTextureStageState(TextureStage,D3DTSS_ALPHAARG0,pTextureOpEx->AlphaArg0);
			g_pd3dDevice->SetTextureStageState(TextureStage,D3DTSS_COLORARG0,pTextureOpEx->ColorArg0);
			g_pd3dDevice->SetTextureStageState(TextureStage,D3DTSS_RESULTARG,pTextureOpEx->Result);
			g_pd3dDevice->SetTextureStageState(TextureStage,D3DTSS_CONSTANT ,pTextureOpEx->Constant);
			//g_pd3dDevice->SetRenderState(D3DRS_BLENDFACTOR,pTextureOp->TFactor);
			break;
		}
	case OPTION_TEXTURE_MAPMETHOD:
		{
			LPTEXTUREMAP pMap = (LPTEXTUREMAP) pOption->pData;
			g_pd3dDevice->GetTextureStageState(TextureStage,D3DTSS_TEXCOORDINDEX,&pMap->BakTextureMap);
			g_pd3dDevice->SetTextureStageState(TextureStage,D3DTSS_TEXCOORDINDEX,pMap->TextureMap);
			break;
		}
	case OPTION_TEXTURE_TRANSFROM:
		{
			float T = timeGetTime()*0.001f;
			LPTEXTURETF pTF = (LPTEXTURETF) pOption->pData;
			g_pd3dDevice->SetTextureStageState(TextureStage,
				D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_COUNT3|D3DTTFF_PROJECTED );

			D3DXMATRIX Mat1,Mat2;

			D3DXMatrixScaling(&Mat1,pTF->Scaling.x,pTF->Scaling.y,pTF->Scaling.z);
			D3DXMatrixRotationYawPitchRoll(&Mat2,pTF->Rotation.x*T,
				pTF->Rotation.y*T,
				pTF->Rotation.z*T);
			D3DXMatrixMultiply(&Mat1,&Mat1,&Mat2);
			Mat1._41 = pTF->Position.x*T;
			Mat1._42 = pTF->Position.y*T;
			Mat1._43 = pTF->Position.z*T;

			switch(TextureStage)
			{
			case 0:
				g_pd3dDevice->SetTransform(D3DTS_TEXTURE0,&Mat1);
				break;
			case 1:
				g_pd3dDevice->SetTransform(D3DTS_TEXTURE1,&Mat1);
				break;
			case 2:
				g_pd3dDevice->SetTransform(D3DTS_TEXTURE2,&Mat1);
				break;
			case 3:
				g_pd3dDevice->SetTransform(D3DTS_TEXTURE3,&Mat1);
				break;
			}
			break;
		}
	}
	return S_OK;
}

HRESULT KModel::RestoreOption(DWORD TextureStage, LPOPTION pOption)
{
	switch(pOption->Type)
	{
	case OPTION_MATERIAL_CULLING:
		{
			LPMATCULL pMatCull = (LPMATCULL) pOption->pData;
			g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, pMatCull->BakCullMode);
			break;
		}
	case OPTION_MATERIAL_ALPHABLEND:
		{
			LPMATBLEND pMatBlend = (LPMATBLEND) pOption->pData;
			g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND,  pMatBlend->BakSrcBlend);
			g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, pMatBlend->BakDestBlend);
			break;
		}
	case OPTION_MATERIAL_ALPHAOPERATION:
		{
			LPMTLALPHAOPERATION pMtlAlphaOp = (LPMTLALPHAOPERATION) pOption->pData;
			g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,          pMtlAlphaOp->BackEnableAlphaBlend);
			g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND,                  pMtlAlphaOp->BackSrcBlend);
			g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND,                 pMtlAlphaOp->BackDestBlend);
			g_pd3dDevice->SetRenderState(D3DRS_BLENDOP,                   pMtlAlphaOp->BackBlendOperator);
			g_pd3dDevice->SetRenderState(D3DRS_BLENDFACTOR ,              pMtlAlphaOp->BackBlendFactor);

			g_pd3dDevice->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE,  pMtlAlphaOp->BackEnableSeparateAlphaBlend);
			g_pd3dDevice->SetRenderState(D3DRS_BLENDOPALPHA,              pMtlAlphaOp->BackBlendOperatorAlpha);
			g_pd3dDevice->SetRenderState(D3DRS_SRCBLENDALPHA,             pMtlAlphaOp->BackSrcBlendAlpha);
			g_pd3dDevice->SetRenderState(D3DRS_DESTBLENDALPHA,            pMtlAlphaOp->BackDestBlendAlpha);

			g_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE,           pMtlAlphaOp->BackEnableAlphaTest);
			g_pd3dDevice->SetRenderState(D3DRS_ALPHAREF,                  pMtlAlphaOp->BackAlphaTestRef);
			g_pd3dDevice->SetRenderState(D3DRS_ALPHAFUNC,                 pMtlAlphaOp->BackAlphaFunction);

			break;
		}
	case OPTION_TEXTURE_OPERATION:
		{
			LPTEXTUREOP pTextureOp = (LPTEXTUREOP) pOption->pData;
			g_pd3dDevice->SetTextureStageState(TextureStage,D3DTSS_COLOROP,pTextureOp->BakColorOp);
			g_pd3dDevice->SetTextureStageState(TextureStage,D3DTSS_ALPHAOP,pTextureOp->BakAlphaOp);
			g_pd3dDevice->SetTextureStageState(TextureStage,D3DTSS_COLORARG1,pTextureOp->BakColorArg1);
			g_pd3dDevice->SetTextureStageState(TextureStage,D3DTSS_COLORARG2,pTextureOp->BakColorArg2);
			g_pd3dDevice->SetTextureStageState(TextureStage,D3DTSS_ALPHAARG1,pTextureOp->BakAlphaArg1);
			g_pd3dDevice->SetTextureStageState(TextureStage,D3DTSS_ALPHAARG2,pTextureOp->BakAlphaArg2);
			g_pd3dDevice->SetRenderState(D3DRS_BLENDFACTOR,pTextureOp->BakTFactor);
			break;
		}
	case OPTION_TEXTURE_OPERATIONEX:
		{
			LPTEXTUREOPEX pTextureOpEx = (LPTEXTUREOPEX) pOption->pData;
			g_pd3dDevice->SetTextureStageState(TextureStage,D3DTSS_COLOROP,pTextureOpEx->BakColorOp);
			g_pd3dDevice->SetTextureStageState(TextureStage,D3DTSS_ALPHAOP,pTextureOpEx->BakAlphaOp);
			g_pd3dDevice->SetTextureStageState(TextureStage,D3DTSS_COLORARG1,pTextureOpEx->BakColorArg1);
			g_pd3dDevice->SetTextureStageState(TextureStage,D3DTSS_COLORARG2,pTextureOpEx->BakColorArg2);
			g_pd3dDevice->SetTextureStageState(TextureStage,D3DTSS_ALPHAARG1,pTextureOpEx->BakAlphaArg1);
			g_pd3dDevice->SetTextureStageState(TextureStage,D3DTSS_ALPHAARG2,pTextureOpEx->BakAlphaArg2);
			g_pd3dDevice->GetTextureStageState(TextureStage,D3DTSS_ALPHAARG0,&pTextureOpEx->BakAlphaArg0);
			g_pd3dDevice->GetTextureStageState(TextureStage,D3DTSS_COLORARG0,&pTextureOpEx->BakColorArg0);
			g_pd3dDevice->GetTextureStageState(TextureStage,D3DTSS_RESULTARG,&pTextureOpEx->BakResult);
			g_pd3dDevice->GetTextureStageState(TextureStage,D3DTSS_CONSTANT ,&pTextureOpEx->BakConstant);
			//g_pd3dDevice->SetRenderState(D3DRS_BLENDFACTOR,pTextureOp->BakTFactor);
			break;
		}	
	case OPTION_TEXTURE_MAPMETHOD:
		{
			LPTEXTUREMAP pMap = (LPTEXTUREMAP) pOption->pData;
			g_pd3dDevice->SetTextureStageState(TextureStage,D3DTSS_TEXCOORDINDEX,pMap->BakTextureMap);
			break;
		}
	case OPTION_TEXTURE_TRANSFROM:
		{
			g_pd3dDevice->SetTextureStageState(TextureStage,D3DTSS_TEXCOORDINDEX,D3DTSS_TCI_PASSTHRU);
			g_pd3dDevice->SetTextureStageState(TextureStage,D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_DISABLE);
			break;
		}
	}
	return S_OK;
}

HRESULT KModel::RestoreMaterial(DWORD MaterialID)
{
	if (MaterialID>=m_dNumMaterial)
		return S_OK;
	LPMATERIAL pMaterial = &m_lpMaterial[MaterialID]; 
	//g_pd3dDevice->SetMaterial(&pMaterial->m_sMaterial9); 

	for(DWORD i_Option=0;i_Option<pMaterial->m_dwNumOptions;i_Option++)
	{
		LPOPTION pOpt = pMaterial->m_lpMatOptions[i_Option];
		RestoreOption(0,pOpt);
	}

	//g_pd3dDevice->SetVertexShader(NULL);

	for(int i=0;i<8;i++)
	{
		//KModelTable* pXMeshTable = (KModelTable*) m_pMeshTable;
		//LPTEXTURE pTexture = NULL;

		//g_pd3dDevice->SetTexture(i,NULL);
		if (pMaterial->m_dTextureIDs[i]!=0)
		{
			for(DWORD i_TOption=0;i_TOption<pMaterial->m_lpTextureNumOption[i];i_TOption++)
			{
				LPOPTION pOpt = pMaterial->m_lpTextureOptions[i][i_TOption];
				RestoreOption(i,pOpt);
			}	
		}
	}

	return S_OK;
}

HRESULT KModel::SaveMesh(LPSTR pFileName, BOOL bSaveBlendMesh /* = FALSE*/)
{
	FILE* pFile = fopen(pFileName,"wb");
	if (pFile==NULL) return E_FAIL;

	SaveMesh(pFile, bSaveBlendMesh);

	fclose(pFile);
	return S_OK;
}

HRESULT KModel::SaveMaterial(LPSTR pFileName)
{
	DWORD Mask = 0x4D41544C;//MATL对应的ASECC代码
	DWORD BlockEnd  = 0xFFFFFFFF;  
	DWORD BlockLength = 0;
	FILE* pFile = fopen(pFileName,"wb");
	if (pFile==NULL) return E_FAIL;
	fwrite(&Mask,sizeof(DWORD),1,pFile);
	//计算并写入快长度
	BlockLength += sizeof(DWORD)*2 ;//子部分数目，结束标志

	for(DWORD i_Sub=0;i_Sub<m_dNumMaterial;i_Sub++)
	{
		//LPMATERIAL pMaterial = &m_lpMaterial[i_Sub]; 
		BlockLength += sizeof(float)*17 + sizeof(DWORD) + //四种颜色，反光强度，帖图数目
			8*(sizeof(float)+sizeof(TCHAR)*(20+256));//帖图信息 
	}
	fwrite(&BlockLength,sizeof(DWORD),1,pFile);

	fwrite(&m_dNumMaterial,sizeof(DWORD),1,pFile);                  //子部分数目
	for( i_Sub=0;i_Sub<m_dNumMaterial;i_Sub++)
	{
		LPMATERIAL pMaterial = &m_lpMaterial[i_Sub]; 
		fwrite(&pMaterial->m_sMaterial9.Ambient,sizeof(D3DCOLORVALUE),1,pFile); 
		fwrite(&pMaterial->m_sMaterial9.Diffuse,sizeof(D3DCOLORVALUE),1,pFile); 
		fwrite(&pMaterial->m_sMaterial9.Specular,sizeof(D3DCOLORVALUE),1,pFile); 
		fwrite(&pMaterial->m_sMaterial9.Emissive,sizeof(D3DCOLORVALUE),1,pFile); 
		fwrite(&pMaterial->m_sMaterial9.Power,sizeof(float),1,pFile);

		DWORD NumMtlOption = pMaterial->m_dwNumOptions;
		if(pMaterial->m_lpEffect)
		{
			NumMtlOption = pMaterial->m_dwNumOptions + 1;
		}	

		fwrite(&pMaterial->m_dOption,sizeof(DWORD),1,pFile);//材质选项

		fwrite(&NumMtlOption,sizeof(DWORD),1,pFile);//选项数目
		for(DWORD i_MOp=0;i_MOp<pMaterial->m_dwNumOptions;i_MOp++)
		{
			LPOPTION pOption = pMaterial->m_lpMatOptions[i_MOp];
			SaveOption(pOption,pFile);
		}
		if(pMaterial->m_lpEffect)
		{
			_Option EffectOption;
			EffectOption.pData = pMaterial;
			EffectOption.Type = OPTION_MATERIAL_MSEFFECT;
			SaveOption(&EffectOption,pFile);
			EffectOption.pData = NULL;
		}

		DWORD NumTexture = 0;
		for(DWORD kk=0;kk<8;kk++)
		{
			if (pMaterial->m_dTextureIDs[kk]==0)
				break;
			NumTexture++;
		}
		fwrite(&NumTexture,sizeof(DWORD),1,pFile);//帖图数目
		for(DWORD i_Text=0;i_Text<NumTexture;i_Text++)
		{
			float Amount = 0;
			fwrite(&Amount,sizeof(float),1,pFile);
			TCHAR Type[256];
			TCHAR Name[256];
			wsprintf(Type,"%s",pMaterial->m_scTextureType[i_Text].c_str());
			wsprintf(Name,"%s",pMaterial->m_scTextureName[i_Text].c_str());

			TCHAR path_buffer[256];
			TCHAR drive[256];
			TCHAR dir[256];
			TCHAR fname[256];
			TCHAR ext[256];

			wsprintf(path_buffer,"%s",pMaterial->m_scTextureName[i_Text].c_str());
			_splitpath( path_buffer, drive, dir, fname, ext );
			wsprintf(Name,"Texture\\%s%s",fname,ext);

			fwrite( Type,sizeof(TCHAR),20,pFile);
			fwrite( Name,sizeof(TCHAR),256,pFile);

			pMaterial->m_lpTextureNumOption[i_Text] = pMaterial->m_lpTextureOptions[i_Text].size();

			fwrite(&pMaterial->m_lpTextureNumOption[i_Text],sizeof(DWORD),1,pFile);//帖图选项数目
			for(DWORD i_TOption=0;i_TOption<pMaterial->m_lpTextureNumOption[i_Text];i_TOption++)
			{
				LPOPTION pOption = pMaterial->m_lpTextureOptions[i_Text][i_TOption];
				SaveOption(pOption,pFile);
			}
		}
	}	
	fwrite(&BlockEnd,sizeof(DWORD),1,pFile);

	fclose(pFile);
	return S_OK;
}

HRESULT KModel::LoadOption(LPOPTION pOption,FILE* pFile,LPMATERIAL pMtl)
{
	fread(&pOption->Type,sizeof(DWORD),1,pFile);
	switch(pOption->Type)
	{
	case OPTION_MATERIAL_CULLING:
		{
			LPMATCULL pMatCull = new _MatCull;
			fread(pMatCull,sizeof(_MatCull),1,pFile);
			pOption->pData = pMatCull;
			break;
		}
	case OPTION_MATERIAL_ALPHABLEND:
		{
			_MatBlend NewMatBlend;
			fread(&NewMatBlend,sizeof(_MatBlend),1,pFile);

			LPMTLALPHAOPERATION pMtlAlphaOp = new MTLALPHAOPERATION;
			pMtlAlphaOp->SrcBlend  = NewMatBlend.SrcBlend;
			pMtlAlphaOp->DestBlend = NewMatBlend.DestBlend;

			pOption->pData = pMtlAlphaOp;
			pOption->Type = OPTION_MATERIAL_ALPHAOPERATION;
			break;
		}
	case OPTION_MATERIAL_ALPHAOPERATION:
		{
			LPMTLALPHAOPERATION pMtlAlphaOp = new MTLALPHAOPERATION;
			fread(pMtlAlphaOp,sizeof(MTLALPHAOPERATION),1,pFile);
			pOption->pData = pMtlAlphaOp;
			break;
		}
	case OPTION_MATERIAL_MSEFFECT:
		{
			//LPMTLMSEFFECT pEffect = (LPMTLMSEFFECT) pOption->pData;
			TCHAR FileName[256];
			TCHAR FindName[256];

			int   nTechnique = -1;
			DWORD NumCons = 0;

			fread(FileName,sizeof(TCHAR),256,pFile);
			fread(&nTechnique,sizeof(int),1,pFile);
			fread(&NumCons,sizeof(DWORD),1,pFile);

			if(pMtl)
			{
				if (!pMtl->m_lpEffect)
				{
					pMtl->m_lpEffect = new KModel::MTLMSEFFECT;
				}

				if(FAILED(g_cFileFinder.FindFile(FindName,FileName)))
				{
					return E_FAIL;
				}
				if (FAILED(g_EffectMgr.GetResource(&pMtl->m_lpEffect->m_HEffect,FindName)))
				{
					return E_FAIL;
				}
				else
				{
					pMtl->m_lpEffect->OnLoad();
				}		

				pMtl->m_lpEffect->m_nCurTechnique = nTechnique;
			}

			break;
		} 

	case OPTION_TEXTURE_OPERATION:
		{
			_TextureOp Op;
			fread(&Op,sizeof(_TextureOp),1,pFile);

			LPTEXTUREOPEX pTextureOpEx = new _TextureOpEx ;

			pTextureOpEx->AlphaArg1 = Op.AlphaArg1;
			pTextureOpEx->AlphaArg2 = Op.AlphaArg2;
			pTextureOpEx->ColorArg1 = Op.ColorArg1;
			pTextureOpEx->ColorArg2 = Op.ColorArg2;
			pTextureOpEx->AlphaOp = Op.AlphaOp;
			pTextureOpEx->ColorOp = Op.ColorOp;
			pTextureOpEx->TFactor = Op.TFactor;

			pOption->pData = pTextureOpEx;
			pOption->Type = OPTION_TEXTURE_OPERATIONEX;
			break;
		}
	case OPTION_TEXTURE_OPERATIONEX:
		{
			LPTEXTUREOPEX pTextureOpEx = new _TextureOpEx ;
			fread(pTextureOpEx,sizeof(_TextureOpEx),1,pFile);
			pOption->pData = pTextureOpEx;
			break;
		}
	case OPTION_TEXTURE_MAPMETHOD:
		{
			LPTEXTUREMAP pMap = new _TextureMap ;
			fread(pMap,sizeof(_TextureMap),1,pFile);
			pOption->pData = pMap;
			break;
		}
	case OPTION_TEXTURE_TRANSFROM:
		{
			LPTEXTURETF pTf = new _TextureTf ;
			fread(pTf,sizeof(_TextureTf),1,pFile);
			pOption->pData = pTf;
			break;
		}
	}
	return S_OK;
}

HRESULT KModel::SaveOption(LPOPTION pOption, FILE *pFile)
{
	fwrite(&pOption->Type,sizeof(DWORD),1,pFile);
	switch(pOption->Type)
	{
	case OPTION_MATERIAL_CULLING:
		{
			LPMATCULL pMatCull = (LPMATCULL) pOption->pData;
			fwrite(pMatCull,sizeof(_MatCull),1,pFile);
			break;
		}
	case OPTION_MATERIAL_ALPHABLEND:
		{
			LPMATBLEND pMatBlend = (LPMATBLEND) pOption->pData;
			fwrite(pMatBlend,sizeof(_MatBlend),1,pFile);
			break;
		}
	case OPTION_MATERIAL_ALPHAOPERATION:
		{
			LPMTLALPHAOPERATION pMtlAlphaOp = (LPMTLALPHAOPERATION) pOption->pData;
			fwrite(pMtlAlphaOp,sizeof(MTLALPHAOPERATION),1,pFile);
			break;
		}
	case OPTION_MATERIAL_MSEFFECT:
		{
			LPMATERIAL pMtl = (LPMATERIAL) pOption->pData;
			LPMTLMSEFFECT pEffect = pMtl->m_lpEffect;

			TCHAR path_buffer[256];
			TCHAR drive[256];
			TCHAR dir[256];
			TCHAR fname[256];
			TCHAR ext[256];

			wsprintf(path_buffer,"%s",pEffect->m_HEffect->m_szFileName.c_str());
			_splitpath( path_buffer, drive, dir, fname, ext );

			TCHAR FileName[256];
			wsprintf(FileName,"%s%s",fname,ext);
			DWORD NumCons = 0;

			fwrite(FileName,sizeof(TCHAR),256,pFile);
			fwrite(&pEffect->m_nCurTechnique,sizeof(int),1,pFile);
			fwrite(&NumCons,sizeof(DWORD),1,pFile);
			break;
		}

	case OPTION_TEXTURE_OPERATION:
		{
			LPTEXTUREOP pTextureOp = (LPTEXTUREOP) pOption->pData;
			fwrite(pTextureOp,sizeof(_TextureOp),1,pFile);
			break;
		}
	case OPTION_TEXTURE_OPERATIONEX:
		{
			LPTEXTUREOPEX pTextureOpEx = (LPTEXTUREOPEX) pOption->pData;
			fwrite(pTextureOpEx,sizeof(_TextureOpEx),1,pFile);
			break;
		}
	case OPTION_TEXTURE_MAPMETHOD:
		{
			LPTEXTUREMAP pMap = (LPTEXTUREMAP) pOption->pData;
			fwrite(pMap,sizeof(_TextureMap),1,pFile);
			break;
		}
	case OPTION_TEXTURE_TRANSFROM:
		{
			LPTEXTURETF pTf = (LPTEXTURETF) pOption->pData;
			fwrite(pTf,sizeof(_TextureTf),1,pFile);
			break;
		}
	}
	return S_OK;
}

HRESULT KModel::SaveIndividualAnimation()
{
	HRESULT hr = E_FAIL;
	DWORD Mask = 0x414e494d;
	DWORD BlockEnd = 0xffffffff;
	DWORD BlockLength = 0;
	DWORD NumAnimation = 1;
	DWORD m_dNumVertices = 0;
	if(m_pMeshSys)
		m_dNumVertices = m_pMeshSys->GetNumVertices();
	int nIndex;
	TCHAR Path[512];
	TCHAR FilePath[512];
	int nPathLength = strlen(m_szAniFileName) ;
	if ( nPathLength )
	{
		strcpy(Path, m_szAniFileName);
		while ( nPathLength > 0 && m_szAniFileName[nPathLength - 1] != '\\' )
		{
			nPathLength--;
		}
		Path[nPathLength] = '\0';
		for ( nIndex = 0; nIndex < static_cast<int>(m_dNumAnimation); nIndex++ )
		{
			LPANIMATION& pAni = m_lpAnimations[nIndex];
			strcpy(FilePath, Path);
			strcat(FilePath, pAni->Name);
			strcat(FilePath, ".ani");
			FILE* pFile = fopen(FilePath, "wb");
			if ( pFile )
			{
				fwrite(&Mask, sizeof(DWORD), 1, pFile);
				fwrite(&BlockLength, sizeof(DWORD), 1, pFile);
				fwrite(&NumAnimation, sizeof(DWORD), 1, pFile);
				fwrite(&pAni->Type, sizeof(DWORD), 1, pFile);
				fwrite(pAni->Name, sizeof(TCHAR), 30, pFile);

				if (pAni->Type==ANIMATION_BONE)
				{
					fwrite(&pAni->NumAnimatedBone,sizeof(DWORD),1,pFile);          //该动画中包含的骨骼数目
					fwrite(&pAni->NumFrames,sizeof(DWORD),1,pFile);                  //该动画中包含的桢数目
					fwrite(&pAni->FrameLength,sizeof(float),1,pFile);                //该动画中包含的每桢之间的时间间隔

					for(DWORD i_Bone =0;i_Bone <pAni->NumAnimatedBone;i_Bone++)
					{
						LPSTR BoneName = pAni->AnimatedBoneName[i_Bone];
						fwrite(BoneName,sizeof(TCHAR),30,pFile); 
					}		
					fwrite(pAni->AnimatedBoneMatrix,sizeof(D3DXMATRIX)*pAni->NumAnimatedBone*pAni->NumFrames,1,pFile);//输出骨骼变换数目
				}
				else if ( pAni->Type == ANIMATION_BONE_RELATIVE )
				{

					fwrite(&pAni->NumAnimatedBone,sizeof(DWORD),1,pFile);          //该动画中包含的骨骼数目
					fwrite(&pAni->NumFrames,sizeof(DWORD),1,pFile);                  //该动画中包含的桢数目
					fwrite(&pAni->FrameLength,sizeof(float),1,pFile);
					for(DWORD i_Bone =0;i_Bone <pAni->NumAnimatedBone;i_Bone++)
					{
						LPSTR BoneName = pAni->AnimatedBoneName[i_Bone];
						fwrite(BoneName,sizeof(TCHAR),30,pFile); 
					}		
					fwrite(pAni->AnimatedBoneMatrix,sizeof(D3DXMATRIX)*pAni->NumAnimatedBone*pAni->NumFrames,1,pFile);//输出骨骼变换数目

				}
				else if ( pAni->Type == ANIMATION_BONE_RTS )
				{
					fwrite(&pAni->NumAnimatedBone,sizeof(DWORD),1,pFile);          //该动画中包含的骨骼数目
					fwrite(&pAni->NumFrames,sizeof(DWORD),1,pFile);                  //该动画中包含的桢数目
					fwrite(&pAni->FrameLength,sizeof(float),1,pFile);
					for(DWORD i_Bone =0;i_Bone <pAni->NumAnimatedBone;i_Bone++)
					{
						LPSTR BoneName = pAni->AnimatedBoneName[i_Bone];
						fwrite(BoneName,sizeof(TCHAR),30,pFile); 
					}		
					fwrite(pAni->AnimatedBoneRTS,sizeof(RTS) * pAni->NumAnimatedBone*pAni->NumFrames,1,pFile);//输出骨骼变换数目
				}
				else if (pAni->Type==ANIMATION_VERTICES_RELATIVE)
				{
					fwrite(&m_dNumVertices,sizeof(float),1,pFile);                //顶点总数，用来检测是否和网格相匹配 
					/////////////////////////////////////////////////////////////////////////////////////
					fwrite(&pAni->NumAnimatedVertice,sizeof(DWORD),1,pFile);       //该动画中包含的顶点数目
					fwrite(&pAni->NumFrames,sizeof(DWORD),1,pFile);                  //该动画中包含的桢数目
					fwrite(&pAni->FrameLength,sizeof(float),1,pFile);                //该动画中包含的每桢之间的时间间隔

					fwrite(pAni->AnimatedVerticesIndex,sizeof(DWORD),pAni->NumAnimatedVertice,pFile); 
					fwrite(pAni->AnimatedVerticesPosition,sizeof(D3DXVECTOR3)*pAni->NumAnimatedVertice*
						pAni->NumFrames,1,pFile);//输出骨骼变换数目
				}
				else if (pAni->Type==ANIMATION_VERTICES)
				{
					fwrite(&m_dNumVertices,sizeof(float),1,pFile);                //顶点总数，用来检测是否和网格相匹配 
					/////////////////////////////////////////////////////////////////////////////////////
					fwrite(&pAni->NumAnimatedVertice,sizeof(DWORD),1,pFile);       //该动画中包含的顶点数目
					fwrite(&pAni->NumFrames,sizeof(DWORD),1,pFile);                  //该动画中包含的桢数目
					fwrite(&pAni->FrameLength,sizeof(float),1,pFile);                //该动画中包含的每桢之间的时间间隔

					fwrite(pAni->AnimatedVerticesIndex,sizeof(DWORD),pAni->NumAnimatedVertice,pFile); 
					fwrite(pAni->AnimatedVerticesPosition,sizeof(D3DXVECTOR3)*pAni->NumAnimatedVertice*
						pAni->NumFrames,1,pFile);//输出骨骼变换数目
				}
				else if(pAni->Type==ANIMATION_BLADE_LENGTH)
				{
					DWORD Version = 0;
					fwrite(&Version,sizeof(DWORD),1,pFile);                  //

					fwrite(&pAni->NumBlade,sizeof(DWORD),1,pFile);                  //该动画中包含的刀光的条数
					fwrite(&pAni->NumFrames,sizeof(DWORD),1,pFile);                  //该动画中包含的桢数目
					fwrite(&pAni->FrameLength,sizeof(float),1,pFile);                //该动画中包含的每桢之间的时间间隔

					_Animation::_Blade_ShowLength* pBladeLength = (_Animation::_Blade_ShowLength*) pAni->BladeData;

					fwrite(pBladeLength,sizeof(_Animation::_Blade_ShowLength),
						pAni->NumFrames * pAni->NumBlade,pFile); 
				}
				fwrite(&BlockEnd,sizeof(DWORD),1,pFile);
				fclose(pFile);
				hr = S_OK;
			}

		}
	}
	return hr;
}

HRESULT KModel::SaveAnimation(LPSTR pFileName)
{

	DWORD Mask = 0x414E494D;//ANIM对应的ASECC代码
	DWORD BlockEnd  = 0xFFFFFFFF;  
	DWORD BlockLength = 0;
	FILE* pFile = fopen(pFileName,"wb");
	if (pFile==NULL) return E_FAIL;
	fwrite(&Mask,sizeof(DWORD),1,pFile);
	///////////////////////////////////////////////////////////////////////////////////////
	//写入块长度
	DWORD m_dNumVertices = 0;
	if(m_pMeshSys)
	m_dNumVertices = m_pMeshSys->GetNumVertices();

	fwrite(&BlockLength,sizeof(DWORD),1,pFile);

	//////////////////////////////////////////////////////////////////////////////////////
	fwrite(&m_dNumAnimation,sizeof(DWORD),1,pFile);                      //动画数目，这里默认为１	
	for(DWORD i_Ani=0;i_Ani<m_dNumAnimation;i_Ani++)
	{
		KModel::LPANIMATION pAni = m_lpAnimations[i_Ani];

		fwrite(&pAni->Type,sizeof(DWORD),1,pFile);
		fwrite( pAni->Name,sizeof(TCHAR),30,pFile);

		if (pAni->Type==ANIMATION_BONE)
		{
			fwrite(&pAni->NumAnimatedBone,sizeof(DWORD),1,pFile);          //该动画中包含的骨骼数目
			fwrite(&pAni->NumFrames,sizeof(DWORD),1,pFile);                  //该动画中包含的桢数目
			fwrite(&pAni->FrameLength,sizeof(float),1,pFile);                //该动画中包含的每桢之间的时间间隔

			for(DWORD i_Bone =0;i_Bone <pAni->NumAnimatedBone;i_Bone++)
			{
				LPSTR BoneName = pAni->AnimatedBoneName[i_Bone];
				fwrite(BoneName,sizeof(TCHAR),30,pFile); 
			}		
			fwrite(pAni->AnimatedBoneMatrix,sizeof(D3DXMATRIX)*pAni->NumAnimatedBone*pAni->NumFrames,1,pFile);//输出骨骼变换数目
		}
		else if ( pAni->Type == ANIMATION_BONE_RELATIVE )
		{

			fwrite(&pAni->NumAnimatedBone,sizeof(DWORD),1,pFile);          //该动画中包含的骨骼数目
			fwrite(&pAni->NumFrames,sizeof(DWORD),1,pFile);                  //该动画中包含的桢数目
			fwrite(&pAni->FrameLength,sizeof(float),1,pFile);
			for(DWORD i_Bone =0;i_Bone <pAni->NumAnimatedBone;i_Bone++)
			{
				LPSTR BoneName = pAni->AnimatedBoneName[i_Bone];
				fwrite(BoneName,sizeof(TCHAR),30,pFile); 
			}		
			fwrite(pAni->AnimatedBoneMatrix,sizeof(D3DXMATRIX)*pAni->NumAnimatedBone*pAni->NumFrames,1,pFile);//输出骨骼变换数目

		}
		else if ( pAni->Type == ANIMATION_BONE_RTS )
		{
			fwrite(&pAni->NumAnimatedBone,sizeof(DWORD),1,pFile);          //该动画中包含的骨骼数目
			fwrite(&pAni->NumFrames,sizeof(DWORD),1,pFile);                  //该动画中包含的桢数目
			fwrite(&pAni->FrameLength,sizeof(float),1,pFile);
			for(DWORD i_Bone =0;i_Bone <pAni->NumAnimatedBone;i_Bone++)
			{
				LPSTR BoneName = pAni->AnimatedBoneName[i_Bone];
				fwrite(BoneName,sizeof(TCHAR),30,pFile); 
			}		
			fwrite(pAni->AnimatedBoneRTS,sizeof(RTS) * pAni->NumAnimatedBone*pAni->NumFrames,1,pFile);//输出骨骼变换数目
		}
		else if (pAni->Type==ANIMATION_VERTICES_RELATIVE)
		{
			fwrite(&m_dNumVertices,sizeof(float),1,pFile);                //顶点总数，用来检测是否和网格相匹配 
			/////////////////////////////////////////////////////////////////////////////////////
			fwrite(&pAni->NumAnimatedVertice,sizeof(DWORD),1,pFile);       //该动画中包含的顶点数目
			fwrite(&pAni->NumFrames,sizeof(DWORD),1,pFile);                  //该动画中包含的桢数目
			fwrite(&pAni->FrameLength,sizeof(float),1,pFile);                //该动画中包含的每桢之间的时间间隔

			fwrite(pAni->AnimatedVerticesIndex,sizeof(DWORD),pAni->NumAnimatedVertice,pFile); 
			fwrite(pAni->AnimatedVerticesPosition,sizeof(D3DXVECTOR3)*pAni->NumAnimatedVertice*
				pAni->NumFrames,1,pFile);//输出骨骼变换数目
		}
		else if (pAni->Type==ANIMATION_VERTICES)
		{
			fwrite(&m_dNumVertices,sizeof(float),1,pFile);                //顶点总数，用来检测是否和网格相匹配 
			/////////////////////////////////////////////////////////////////////////////////////
			fwrite(&pAni->NumAnimatedVertice,sizeof(DWORD),1,pFile);       //该动画中包含的顶点数目
			fwrite(&pAni->NumFrames,sizeof(DWORD),1,pFile);                  //该动画中包含的桢数目
			fwrite(&pAni->FrameLength,sizeof(float),1,pFile);                //该动画中包含的每桢之间的时间间隔

			fwrite(pAni->AnimatedVerticesIndex,sizeof(DWORD),pAni->NumAnimatedVertice,pFile); 
			fwrite(pAni->AnimatedVerticesPosition,sizeof(D3DXVECTOR3)*pAni->NumAnimatedVertice*
				pAni->NumFrames,1,pFile);//输出骨骼变换数目
		}
		else if(pAni->Type==ANIMATION_BLADE_LENGTH)
		{
			DWORD Version = 0;
			fwrite(&Version,sizeof(DWORD),1,pFile);                  //

			fwrite(&pAni->NumBlade,sizeof(DWORD),1,pFile);                  //该动画中包含的刀光的条数
			fwrite(&pAni->NumFrames,sizeof(DWORD),1,pFile);                  //该动画中包含的桢数目
			fwrite(&pAni->FrameLength,sizeof(float),1,pFile);                //该动画中包含的每桢之间的时间间隔

			_Animation::_Blade_ShowLength* pBladeLength = (_Animation::_Blade_ShowLength*) pAni->BladeData;

			fwrite(pBladeLength,sizeof(_Animation::_Blade_ShowLength),
				pAni->NumFrames * pAni->NumBlade,pFile); 
		}
	}
	fwrite(&BlockEnd,sizeof(DWORD),1,pFile);

	fclose(pFile);
	return S_OK;
}

HRESULT KModel::MoveTextureFile(LPSTR pDirName)
{
	for(DWORD i=0;i<m_dNumMaterial;i++)
	{
		LPMATERIAL pMaterial = &m_lpMaterial[i]; 
		for(DWORD kk=0;kk<8;kk++)
		{
			if (pMaterial->m_dTextureIDs[kk]==0)
				break;
			TCHAR str[256];
			TCHAR path_buffer[256];
			TCHAR drive[256];
			TCHAR dir[256];
			TCHAR fname[256];
			TCHAR ext[256];

			wsprintf(path_buffer,"%s",pMaterial->m_scTextureName[kk].c_str());
			_splitpath( path_buffer, drive, dir, fname, ext );
			mkdir(pDirName);
			wsprintf(str,"%s\\%s%s",pDirName,fname,ext);
			//wsprintf(str,"%s%s",fname,ext);

			CopyFile(pMaterial->m_scTextureName[kk].c_str(),str,FALSE);

			pMaterial->m_scTextureName[kk] = str;
		}

	}
	return S_OK;
}

HRESULT KModel::DrawBBox()
{
	D3DXMATRIX MatSave;
	g_pd3dDevice->GetTransform(D3DTS_WORLD,&MatSave);

	g_pd3dDevice->SetTransform(D3DTS_WORLD,&m_Matrix);
	g_cGraphicsTool.DrawAABBox(&m_BBox_A,&m_BBox_B,0xFF0000FF);

	g_pd3dDevice->SetTransform(D3DTS_WORLD,&MatSave);
	return S_OK;
}

HRESULT KModel::LoadShader()
{
	HRESULT hr = S_OK;
	D3DVERTEXELEMENT9 decl[] =
	{
		{ 0, 0 , D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 }, // v0
		{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },   // v3
		{ 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 }, // v7
		{ 0, 32, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0 },  // v8
		D3DDECL_END()
	};

	if ( FAILED( g_pd3dDevice->CreateVertexDeclaration( decl, &g_pVertexDeclaration ) ) )
		return E_FAIL;

	TCHAR Shad1[255];
	wsprintf( Shad1,"D:\\WorkSpace\\SkinMeshEdit\\bumpshader3.vsh");
	LPD3DXBUFFER pCode=NULL;

	if ( FAILED(hr = D3DXAssembleShaderFromFile(Shad1, NULL, NULL, 0, &pCode, NULL ) ) )
		return hr;
	if ( FAILED(g_pd3dDevice->CreateVertexShader( (DWORD*)pCode->GetBufferPointer(), &m_pCartoonShader ) ) )
		return E_FAIL;
	SAFE_RELEASE(pCode);

	wsprintf( Shad1,"D:\\WorkSpace\\SkinMeshEdit\\bumpshader2.vsh");
	pCode=NULL;

	if ( FAILED(hr = D3DXAssembleShaderFromFile(Shad1, NULL, NULL, 0, &pCode, NULL ) ) )
		return hr;
	if ( FAILED(g_pd3dDevice->CreateVertexShader( (DWORD*)pCode->GetBufferPointer(), &m_pFrameShader ) ) )
		return E_FAIL;
	SAFE_RELEASE(pCode);

	LPD3DXMESH pMesh = NULL;
	hr = m_pMeshSys->CloneMesh(D3DXMESH_MANAGED, decl, g_pd3dDevice, &pMesh);
	if (FAILED(hr))
		return E_FAIL;

	hr = D3DXComputeNormals(pMesh,NULL);
	if (FAILED(hr))
		return E_FAIL;
	hr = D3DXComputeTangent( pMesh, 0, 0, 0, TRUE, NULL );
	if (FAILED(hr))
		return E_FAIL;

	m_pMeshSys->Release();
	m_pMeshSys = pMesh;

	return S_OK;
}

HRESULT KModel::SetShader()
{
	HRESULT hr = S_OK;

	float s = (sinf(timeGetTime()*0.001f)+1)*0.1f;
	D3DXVECTOR3 U,V,W,Pos;
	D3DXVECTOR4 Light,one(1,1,1,1),half(0.5f,0.5f,0.5f,0.5f),Camera( 0.0f, 1.0f, -3.5F,0 );;
	D3DXVECTOR4 con(0.18f,0,0.025f,s);

	D3DXVECTOR3 vecDir = D3DXVECTOR3(100*sinf(timeGetTime()*0.001f),
		-100,
		100*cosf(timeGetTime()*0.001f) );

	Light.x = - vecDir.x;//g_LightVector.x;
	Light.y = - vecDir.y;//g_LightVector.y;
	Light.z = - vecDir.z;//g_LightVector.z;
	Light.w = 0;


	D3DXVec4Normalize(&Light,&Light);


	D3DXMATRIXA16  m_matObject;
	D3DXMATRIXA16  m_matView;
	D3DXMATRIXA16  m_matProj;
	D3DXMATRIXA16  m_matTot;

	D3DXMATRIXA16 m_MObject;
	D3DXMATRIXA16 m_MpView;
	D3DXMATRIXA16 m_MpProj;

	g_pd3dDevice->GetTransform( D3DTS_WORLD, &m_MObject );
	g_pd3dDevice->GetTransform( D3DTS_VIEW,  &m_MpView );
	g_pd3dDevice->GetTransform( D3DTS_PROJECTION,&m_MpProj );

	Camera.x = m_MpView._41;
	Camera.y = m_MpView._42;
	Camera.z = m_MpView._43;

	D3DXMATRIXA16  temp;

	D3DXMatrixMultiply(&m_matTot,&m_MObject,&m_MpView);

	//D3DXMatrixTranspose(&temp, &m_matTot);

	//the inverse transform

	D3DXMatrixMultiply(&m_matTot,&m_matTot,&m_MpProj);

	D3DXMatrixTranspose(&m_matTot,&m_matTot);
	D3DXMatrixTranspose(&m_matObject,&m_MObject);

	//load the values into the registers
	g_pd3dDevice->SetVertexShaderConstantF(  32, (float*)&one,    1);
	g_pd3dDevice->SetVertexShaderConstantF(  33, (float*)&half,  1);
	g_pd3dDevice->SetVertexShaderConstantF(  34, (float*)&Camera,  1);
	g_pd3dDevice->SetVertexShaderConstantF(  35, (float*)&con,  1);

	g_pd3dDevice->SetVertexShaderConstantF(   0, (float*)&m_matObject, 4);
	g_pd3dDevice->SetVertexShaderConstantF(   8, (float*)&m_matTot, 4);	
	g_pd3dDevice->SetVertexShaderConstantF(  12, (float*)&Light,  1 );
	g_pd3dDevice->SetVertexShaderConstantF(  16, (float*)(&m_MObject), 4);


	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);
	hr = g_pd3dDevice->SetVertexDeclaration( g_pVertexDeclaration );

	hr = g_pd3dDevice->SetVertexShader(m_pFrameShader);
	//g_pd3dDevice->SetVertexShader(m_pCartoonShader);

	g_pd3dDevice->SetPixelShader(0);

	return S_OK;
}

HRESULT KModel::DrawMeshNormal()
{
	return S_OK;
}

HRESULT KModel::PlaneShadowRender()
{
	D3DXVECTOR3 vecDir(0,-1,0);
	g_cGraphicsTool.GetSunDirection(vecDir);

	D3DXMATRIX matShadow,matsave,matCur;
	g_pd3dDevice->GetTransform( D3DTS_WORLD , &matsave);
	D3DXMatrixMultiply(&matCur,&matsave,&m_Matrix);

	LPSCENE pScene = g_cGraphicsTool.GetCurScene();
	if(pScene)
	{
		D3DXVECTOR2 XZ = D3DXVECTOR2(matCur._41,matCur._43);
		D3DXVECTOR3 Height;
		pScene->GetHeight(&Height,&XZ,0);
		m_fPlanarShadowHeight = Height.y;
	}

	D3DXVECTOR4 Light;
	Light.x = - vecDir.x;
	Light.y = - vecDir.y;
	Light.z = - vecDir.z;
	Light.w = 0;
	D3DXVec4Normalize(&Light,&Light);

	D3DXPLANE Plane;
	D3DXVECTOR3 AAA(0,m_fPlanarShadowHeight,0);
	D3DXVECTOR3 BBB(0,1,0);
	D3DXPlaneFromPointNormal(&Plane,&AAA,&BBB);


	D3DXMatrixShadow(&matShadow,&Light,&Plane);
	D3DXMatrixMultiply(&matShadow,&matCur,&matShadow);

	D3DMATERIAL9 Mtl,MtlSave;
	ZeroMemory(&Mtl,sizeof(D3DMATERIAL9));
	Mtl.Diffuse.a = 1;

	g_pd3dDevice->GetMaterial(&MtlSave);

	g_pd3dDevice->SetTransform( D3DTS_WORLD , &matShadow);
	g_pd3dDevice->SetMaterial(&Mtl);
	g_pd3dDevice->SetTexture(0,NULL);
	g_pd3dDevice->SetTexture(1,NULL);
	g_pd3dDevice->SetRenderState( D3DRS_CULLMODE , D3DCULL_NONE);
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);

	if ((m_dOption&MESH_SKINED)&&(m_lpBoneInfo!=NULL))
	{
		DWORD AttribIdPrev = UNUSED32; 

		if (!m_pBoneCombinationTable)
			return E_FAIL;
		LPD3DXBONECOMBINATION pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(m_pBoneCombinationTable->GetBufferPointer());

		D3DXMATRIX MatSave;
		g_pd3dDevice->GetTransform( D3DTS_WORLD , &MatSave);

		// Draw using default vtx processing of the device (typically HW)
		for (DWORD iAttrib = 0; iAttrib < m_NumAttributeGroups; iAttrib++)
		{
			DWORD NumBlend = 0;
			for (DWORD i = 0; i < m_MaxFaceInfl; ++i)
			{
				if (pBoneComb[iAttrib].BoneId[i] != UINT_MAX)
				{
					NumBlend = i;
				}
			}
			if (g_D3DCaps.MaxVertexBlendMatrices >= NumBlend + 1)
			{
				// first calculate the world matrices for the current set of blend weights and get the accurate count of the number of blends
				for (DWORD i = 0; i < m_MaxFaceInfl; ++i)
				{
					DWORD iMatrixIndex = pBoneComb[iAttrib].BoneId[i];
					if (iMatrixIndex != UINT_MAX)
					{
						LPBONEINFO pInfo = &m_lpBoneInfo[iMatrixIndex];

						D3DXMatrixMultiply(&m_pBoneMatrices[iMatrixIndex],
							&(pInfo->MatrixOffset),
							&(pInfo->CurMatrix));
						D3DXMatrixMultiply(&m_pBoneMatrices[iMatrixIndex],
							&m_pBoneMatrices[iMatrixIndex],
							&MatSave);
						g_pd3dDevice->SetTransform( D3DTS_WORLDMATRIX( i ), &m_pBoneMatrices[iMatrixIndex] );
					}
				}
				g_pd3dDevice->SetRenderState(D3DRS_VERTEXBLEND, NumBlend);
				// lookup the material used for this subset of faces
				if ((AttribIdPrev != pBoneComb[iAttrib].AttribId) || (AttribIdPrev == UNUSED32))
				{
					SetMaterial(pBoneComb[iAttrib].AttribId);
					AttribIdPrev = pBoneComb[iAttrib].AttribId;
				}

				// draw the subset now that the correct material and matrices are loaded
				//if (SUCCEEDED(hr))
				g_pd3dDevice->SetMaterial(&Mtl);
				m_pBlendMesh->DrawSubset(iAttrib);
				//RestoreMaterial(pBoneComb[iAttrib].AttribId);
			}
		}
		// If necessary, draw parts that HW could not handle using SW
		if (m_iAttributeSW < m_NumAttributeGroups)
		{
			AttribIdPrev = UNUSED32; 
			g_pd3dDevice->SetSoftwareVertexProcessing(TRUE);
			for (iAttrib = m_iAttributeSW; iAttrib < m_NumAttributeGroups; iAttrib++)
			{
				DWORD NumBlend = 0;
				for (DWORD i = 0; i < m_MaxFaceInfl; ++i)
				{
					if (pBoneComb[iAttrib].BoneId[i] != UINT_MAX)
					{
						NumBlend = i;
					}
				}
				if (g_D3DCaps.MaxVertexBlendMatrices < NumBlend + 1)
				{
					// first calculate the world matrices for the current set of blend weights and get the accurate count of the number of blends
					for (DWORD i = 0; i < m_MaxFaceInfl; ++i)
					{
						DWORD iMatrixIndex = pBoneComb[iAttrib].BoneId[i];
						if (iMatrixIndex != UINT_MAX)
						{
							LPBONEINFO pInfo = &m_lpBoneInfo[iMatrixIndex];
							D3DXMatrixMultiply(&m_pBoneMatrices[iMatrixIndex],
								&(pInfo->MatrixOffset),
								&(pInfo->CurMatrix));
							D3DXMatrixMultiply(&m_pBoneMatrices[iMatrixIndex],
								&m_pBoneMatrices[iMatrixIndex],
								&MatSave);
							g_pd3dDevice->SetTransform( D3DTS_WORLDMATRIX( i ), &m_pBoneMatrices[iMatrixIndex] );
						}
					}

					g_pd3dDevice->SetRenderState(D3DRS_VERTEXBLEND, NumBlend);

					// lookup the material used for this subset of faces
					if ((AttribIdPrev != pBoneComb[iAttrib].AttribId) || (AttribIdPrev == UNUSED32))
					{
						SetMaterial(pBoneComb[iAttrib].AttribId);
						AttribIdPrev = pBoneComb[iAttrib].AttribId;
					}

					// draw the subset now that the correct material and matrices are loaded
					//if (SUCCEEDED(hr))
					g_pd3dDevice->SetMaterial(&Mtl);
					m_pBlendMesh->DrawSubset(iAttrib);
					//RestoreMaterial(pBoneComb[iAttrib].AttribId);
				}
			}
			g_pd3dDevice->SetSoftwareVertexProcessing(FALSE);
		}
		g_pd3dDevice->SetRenderState(D3DRS_VERTEXBLEND, 0);
		g_pd3dDevice->SetTransform( D3DTS_WORLD , &MatSave);

	}
	else
	{
		for(DWORD i=0;i<m_dNumMaterial;i++)
		{
			SetMaterial(i);
			g_pd3dDevice->SetMaterial(&Mtl);
			if(m_pMeshSys)
				m_pMeshSys->DrawSubset(i);
		}
	}

	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	g_pd3dDevice->SetTransform( D3DTS_WORLD , &matsave);
	g_pd3dDevice->SetMaterial(&MtlSave);

	return S_OK;
}

HRESULT KModel::SortSkinSubset()
{
	m_vecSkinSubset.clear();
	m_vecSkinSubset.resize(m_dNumMaterial);

	HRESULT hr = S_OK;
	DWORD AttribIdPrev = UNUSED32; 

	if (!m_pBoneCombinationTable)
		return E_FAIL;

	LPD3DXBONECOMBINATION pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(m_pBoneCombinationTable->GetBufferPointer());

	// Draw using default vtx processing of the device (typically HW)
	for (DWORD iAttrib = 0; iAttrib < m_NumAttributeGroups; iAttrib++)
	{
		DWORD NumBlend = 0;
		for (DWORD i = 0; i < m_MaxFaceInfl; ++i)
		{
			if (pBoneComb[iAttrib].BoneId[i] != UINT_MAX)
			{
				NumBlend = i;
			}
		}

		int MatrialIndex = pBoneComb[iAttrib].AttribId;

		_SKinSubsetInfo Info;
		Info.AttribID = iAttrib;
		Info.NumBlend = NumBlend + 1;	

		m_vecSkinSubset[MatrialIndex].SubsetIndex.push_back(Info);
	}
	return S_OK;
}
HRESULT KModel::Absolute_Move(D3DXVECTOR3* pPos)
{
	if (!m_pMeshSys)
		return E_FAIL;

	VFormat::FACES_NORMAL_TEXTURE1 * pVers = NULL;
	if (FAILED(m_pMeshSys->LockVertexBuffer(0,(void**)&pVers)))
		return E_FAIL;

	DWORD NumVer = m_pMeshSys->GetNumVertices();
	for(DWORD i=0;i<NumVer;i++)
	{
		pVers[i].p += *pPos;
	}

	D3DXComputeBoundingBox((D3DXVECTOR3*) pVers,NumVer,sizeof(VFormat::FACES_NORMAL_TEXTURE1),
		&m_BBox_A,&m_BBox_B);

	if (FAILED(m_pMeshSys->UnlockVertexBuffer()))
		return E_FAIL;

	BOOL m_bSkined = m_dOption&MESH_SKINED;
	if (!m_bSkined)
		OptimizeUnSkinedMesh();
	else
		CreateSkinInfo(TRUE);
	return S_OK;
}

HRESULT KModel::FrameMove()
{
	return S_OK;
}

HRESULT KModel::SaveSeclectedSubset2Mesh(BOOL* SelectedID,LPSTR pMeshFileName,LPSTR pMtlFileName,DWORD Option)
{
	vector<DWORD>vecSubsetFacesCount;
	vecSubsetFacesCount.resize(m_dNumMaterial,0);

	DWORD * pAttrib = NULL;
	DWORD NumFaces = m_pMeshSys->GetNumFaces();
	if (FAILED(m_pMeshSys->LockAttributeBuffer(0,(DWORD**)&pAttrib)))
		return E_FAIL;

	for(DWORD i=0;i<NumFaces;i++)
	{
		DWORD ID = pAttrib[i];
		if ((ID>=0)&&(ID<m_dNumMaterial))
		{
			vecSubsetFacesCount[ID]++;
		}

		if(SelectedID[ID])
		{
			;
		}
	}


	m_pMeshSys->UnlockAttributeBuffer();

	return S_OK;
}

HRESULT KModel::SaveMesh(FILE* pFile, BOOL bSaveBlendMesh/*= FALSE*/)
{
	/*DWORD Mask = 0x4D455348;//MESH对应的ASECC代码
	DWORD BlockEnd  = 0xFFFFFFFF;  
	DWORD BlockLength = 0;
	fwrite(&Mask,sizeof(DWORD),1,pFile);

	DWORD m_dNumVertices = m_pMeshSys->GetNumVertices();
	DWORD m_dNumFaces = m_pMeshSys->GetNumFaces();
	//计算并写入快长度
	BlockLength = sizeof(DWORD)*6 +//顶点数目,面（三角形）数目,子部分数目,是否包含骨骼信息,插槽数目，结束标志
	sizeof(D3DXVECTOR3)*m_dNumVertices*3 +//顶点位置坐标，顶点法向量，顶点帖图坐标UVW
	sizeof(DWORD)*m_dNumFaces*4;   //面索引，子部分索引
	if (!(m_dOption&MESH_SKINED))
	{
	BlockLength += sizeof(DWORD);
	for(DWORD i_Bone=0;i_Bone<m_dNumBone;i_Bone++)
	{
	LPBONEINFO pInfo = &m_lpBoneInfo[i_Bone];
	BlockLength += sizeof(DWORD)*2+//包含点数，自骨骼数
	sizeof(D3DMATRIX)+            //位移矩阵
	sizeof(TCHAR)*60+          //自己和父骨骼名称
	(sizeof(DWORD)+sizeof(float))*pInfo->NumVerticesRef;//顶点信息
	for(DWORD i_Child=0;i_Child<pInfo->NumChild;i_Child++)
	{
	BlockLength+=sizeof(TCHAR)*30;//
	}
	}
	}
	fwrite(&BlockLength,sizeof(DWORD),1,pFile);
	//////////////////////////////////////////////////////////////////////////
	//申请内存，并清零
	D3DXVECTOR3 * m_lpVerticePosition = new D3DXVECTOR3[m_dNumVertices];
	ZeroMemory(m_lpVerticePosition,sizeof(D3DXVECTOR3)*m_dNumVertices);
	D3DXVECTOR3 * m_lpVerticeNormal = new D3DXVECTOR3[m_dNumVertices];
	ZeroMemory(m_lpVerticeNormal,sizeof(D3DXVECTOR3)*m_dNumVertices);
	D3DXVECTOR3 * m_lpVerticeTextCoord = new D3DXVECTOR3[m_dNumVertices];
	ZeroMemory(m_lpVerticeTextCoord,sizeof(D3DXVECTOR3)*m_dNumVertices);
	DWORD * m_lpFacesIndex = new DWORD[m_dNumFaces*3];
	ZeroMemory(m_lpFacesIndex,sizeof(DWORD)*m_dNumFaces*3);
	DWORD * m_lpSubsetID = new DWORD[m_dNumFaces];
	ZeroMemory(m_lpSubsetID,sizeof(DWORD)*m_dNumFaces);
	//////////////////////////////////////////////////////////////////////////
	//得到数据
	VFormat::FACES_NORMAL_TEXTURE1 * pVers = NULL;
	WORD* pIndex = NULL;
	DWORD * pAttrib = NULL;
	if (FAILED(m_pMeshSys->LockVertexBuffer(0,(void**)&pVers)))
	return E_FAIL;
	if (FAILED(m_pMeshSys->LockIndexBuffer (0,(void**)&pIndex)))
	return E_FAIL;
	if (FAILED(m_pMeshSys->LockAttributeBuffer(0,(DWORD**)&pAttrib)))
	return E_FAIL;
	for(DWORD i=0;i<m_dNumVertices;i++)
	{
	m_lpVerticePosition[i] = pVers[i].p;
	m_lpVerticeNormal[i] = pVers[i].Normal;
	m_lpVerticeTextCoord[i].x = pVers[i].tu1;
	m_lpVerticeTextCoord[i].y = pVers[i].tv1;
	}
	for( i=0;i<m_dNumFaces;i++)
	{
	m_lpFacesIndex[i*3  ] = pIndex[i*3  ];
	m_lpFacesIndex[i*3+1] = pIndex[i*3+1];
	m_lpFacesIndex[i*3+2] = pIndex[i*3+2];
	m_lpSubsetID[i] = pAttrib[i];
	}
	if (FAILED(m_pMeshSys->UnlockVertexBuffer()))
	return E_FAIL;
	if (FAILED(m_pMeshSys->UnlockIndexBuffer()))
	return E_FAIL;
	if (FAILED(m_pMeshSys->UnlockAttributeBuffer()))
	return E_FAIL;
	//////////////////////////////////////////////////////////////////////////
	fwrite(&m_dNumVertices,sizeof(DWORD),1,pFile);                  //顶点数目
	fwrite( m_lpVerticePosition,sizeof(D3DXVECTOR3),m_dNumVertices,pFile); //顶点位置坐标
	fwrite( m_lpVerticeNormal,sizeof(D3DXVECTOR3),m_dNumVertices,pFile);   //顶点法向量
	fwrite( m_lpVerticeTextCoord,sizeof(D3DXVECTOR3),m_dNumVertices,pFile);//顶点帖图坐标UVW


	fwrite(&m_dNumFaces,sizeof(DWORD),1,pFile);               //面（三角形）数目
	fwrite( m_lpFacesIndex,sizeof(DWORD),m_dNumFaces*3,pFile);//面索引
	fwrite( m_lpSubsetID  ,sizeof(DWORD),m_dNumFaces,pFile);  //子部分索引

	fwrite(&m_dNumMaterial,sizeof(DWORD),1,pFile);//子部分数目

	BOOL m_bSkined = (m_dOption&MESH_SKINED);

	fwrite(&m_bSkined,sizeof(BOOL),1,pFile);//是否包含骨骼信息
	if (m_bSkined)
	{
	//骨骼信息
	fwrite(&m_dNumBone ,sizeof(DWORD),1,pFile);//
	for(DWORD i_Bone=0;i_Bone<m_dNumBone;i_Bone++)
	{
	LPBONEINFO pInfo = &m_lpBoneInfo[i_Bone];
	fwrite(pInfo->Name,sizeof(TCHAR),30,pFile);//
	fwrite(pInfo->ParentName,sizeof(TCHAR),30,pFile);//
	fwrite(&pInfo->NumChild,sizeof(DWORD),1,pFile);//
	for(DWORD i_Child=0;i_Child<pInfo->NumChild;i_Child++)
	{
	fwrite(pInfo->lpChildNames[i_Child],sizeof(TCHAR),30,pFile);//
	}
	fwrite(&pInfo->MatrixOffset,sizeof(D3DMATRIX),1,pFile);//
	fwrite(&pInfo->NumVerticesRef,sizeof(DWORD),1,pFile);//
	fwrite(pInfo->VerticesIndexRef,sizeof(DWORD),
	pInfo->NumVerticesRef,pFile);//
	fwrite(pInfo->VerticesWeightRef,sizeof(float),
	pInfo->NumVerticesRef,pFile);//
	}
	}

	fwrite(&m_dNumSocket,sizeof(DWORD),1,pFile);
	for(DWORD i_Socket=0;i_Socket<m_dNumSocket;i_Socket++)
	{
	LPSOCKET pSocket = &m_lpSockets[i_Socket];
	fwrite(pSocket->Name,sizeof(TCHAR),30,pFile);//
	fwrite(pSocket->ParentBoneName,sizeof(TCHAR),30,pFile);//
	fwrite(&pSocket->MatrixOffset,sizeof(D3DMATRIX),1,pFile);//
	}
	fwrite(&BlockEnd,sizeof(DWORD),1,pFile);
	delete [] m_lpVerticeNormal;
	delete [] m_lpVerticeTextCoord;
	delete [] m_lpFacesIndex;
	delete [] m_lpSubsetID;*/

	_MeshFileHead MeshFileHead;
	_MeshHead     MeshHead;
	DWORD BlockEnd  = 0xFFFFFFFF;  

	DWORD   Pos_Start = ftell(pFile);

	DWORD  m_dNumVertices = m_pMeshSys->GetNumVertices();
	DWORD  m_dNumFaces    = m_pMeshSys->GetNumFaces();
	DWORD  FVF = m_pMeshSys->GetFVF();

	MeshFileHead.MeshCount = 1;
	MeshHead.NumFaces =    m_dNumFaces;
	MeshHead.NumVertices = m_dNumVertices;
	MeshHead.NumSubset =   m_dNumMaterial;
	BOOL m_bSkined = (m_dOption&MESH_SKINED);

	fwrite(&MeshFileHead,sizeof(_MeshFileHead),1,pFile);
	fwrite(&MeshHead,sizeof(_MeshHead),1,pFile);

	D3DXVECTOR3 * m_lpVerticePosition = NULL;
	D3DXVECTOR3 * m_lpVerticeNormal   = NULL;
	D3DCOLOR    * m_lpVerticeDiffuse  = NULL;
	D3DXVECTOR3 * m_lpVerticeTextCoord1 = NULL;
	D3DXVECTOR3 * m_lpVerticeTextCoord2 = NULL;
	D3DXVECTOR3 * m_lpVerticeTextCoord3 = NULL;
	DWORD * m_lpFacesIndex = NULL;
	DWORD * m_lpSubsetID = NULL;

	switch(FVF)
	{
	case (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1):
		{
			VFormat::FACES_NORMAL_TEXTURE1 * pVers = NULL;
			WORD* pIndex = NULL;
			DWORD * pAttrib = NULL;
			if (FAILED(m_pMeshSys->LockVertexBuffer(0,(void**)&pVers)))
				return E_FAIL;
			if (FAILED(m_pMeshSys->LockIndexBuffer (0,(void**)&pIndex)))
				return E_FAIL;
			if (FAILED(m_pMeshSys->LockAttributeBuffer(0,(DWORD**)&pAttrib)))
				return E_FAIL;

			m_lpVerticePosition = new D3DXVECTOR3[m_dNumVertices];
			ZeroMemory(m_lpVerticePosition,sizeof(D3DXVECTOR3)*m_dNumVertices);
			m_lpVerticeNormal = new D3DXVECTOR3[m_dNumVertices];
			ZeroMemory(m_lpVerticeNormal,sizeof(D3DXVECTOR3)*m_dNumVertices);

			m_lpVerticeTextCoord1 = new D3DXVECTOR3[m_dNumVertices];
			ZeroMemory(m_lpVerticeTextCoord1,sizeof(D3DXVECTOR3)*m_dNumVertices);

			m_lpFacesIndex = new DWORD[m_dNumFaces*3];
			ZeroMemory(m_lpFacesIndex,sizeof(DWORD)*m_dNumFaces*3);
			m_lpSubsetID = new DWORD[m_dNumFaces];
			ZeroMemory(m_lpSubsetID,sizeof(DWORD)*m_dNumFaces);

			for(DWORD i=0;i<m_dNumVertices;i++)
			{
				m_lpVerticePosition[i] = pVers[i].p;
				m_lpVerticeNormal[i]   = pVers[i].Normal;
				m_lpVerticeTextCoord1[i].x = pVers[i].tu1;
				m_lpVerticeTextCoord1[i].y = pVers[i].tv1;
			}
			for( i=0;i<m_dNumFaces;i++)
			{
				m_lpFacesIndex[i*3  ] = pIndex[i*3  ];
				m_lpFacesIndex[i*3+1] = pIndex[i*3+1];
				m_lpFacesIndex[i*3+2] = pIndex[i*3+2];
				m_lpSubsetID[i] = pAttrib[i];
			}
			break;
		}
	case (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX2):
		{
			VFormat::FACES_NORMAL_TEXTURE2 * pVers = NULL;
			WORD* pIndex = NULL;
			DWORD * pAttrib = NULL;
			if (FAILED(m_pMeshSys->LockVertexBuffer(0,(void**)&pVers)))
				return E_FAIL;
			if (FAILED(m_pMeshSys->LockIndexBuffer (0,(void**)&pIndex)))
				return E_FAIL;
			if (FAILED(m_pMeshSys->LockAttributeBuffer(0,(DWORD**)&pAttrib)))
				return E_FAIL;

			m_lpVerticePosition = new D3DXVECTOR3[m_dNumVertices];
			ZeroMemory(m_lpVerticePosition,sizeof(D3DXVECTOR3)*m_dNumVertices);
			m_lpVerticeNormal = new D3DXVECTOR3[m_dNumVertices];
			ZeroMemory(m_lpVerticeNormal,sizeof(D3DXVECTOR3)*m_dNumVertices);

			m_lpVerticeTextCoord1 = new D3DXVECTOR3[m_dNumVertices];
			ZeroMemory(m_lpVerticeTextCoord1,sizeof(D3DXVECTOR3)*m_dNumVertices);
			m_lpVerticeTextCoord2 = new D3DXVECTOR3[m_dNumVertices];
			ZeroMemory(m_lpVerticeTextCoord2,sizeof(D3DXVECTOR3)*m_dNumVertices);

			m_lpFacesIndex = new DWORD[m_dNumFaces*3];
			ZeroMemory(m_lpFacesIndex,sizeof(DWORD)*m_dNumFaces*3);
			m_lpSubsetID = new DWORD[m_dNumFaces];
			ZeroMemory(m_lpSubsetID,sizeof(DWORD)*m_dNumFaces);

			for(DWORD i=0;i<m_dNumVertices;i++)
			{
				m_lpVerticePosition[i] = pVers[i].p;
				m_lpVerticeNormal[i]   = pVers[i].Normal;
				m_lpVerticeTextCoord1[i].x = pVers[i].tu1;
				m_lpVerticeTextCoord1[i].y = pVers[i].tv1;
				m_lpVerticeTextCoord2[i].x = pVers[i].tu2;
				m_lpVerticeTextCoord2[i].y = pVers[i].tv2;
			}
			for( i=0;i<m_dNumFaces;i++)
			{
				m_lpFacesIndex[i*3  ] = pIndex[i*3  ];
				m_lpFacesIndex[i*3+1] = pIndex[i*3+1];
				m_lpFacesIndex[i*3+2] = pIndex[i*3+2];
				m_lpSubsetID[i] = pAttrib[i];
			}
			break;
		}
	case (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX3):
		{
			VFormat::FACES_NORMAL_TEXTURE3 * pVers = NULL;
			WORD* pIndex = NULL;
			DWORD * pAttrib = NULL;
			if (FAILED(m_pMeshSys->LockVertexBuffer(0,(void**)&pVers)))
				return E_FAIL;
			if (FAILED(m_pMeshSys->LockIndexBuffer (0,(void**)&pIndex)))
				return E_FAIL;
			if (FAILED(m_pMeshSys->LockAttributeBuffer(0,(DWORD**)&pAttrib)))
				return E_FAIL;
			m_lpVerticePosition = new D3DXVECTOR3[m_dNumVertices];
			ZeroMemory(m_lpVerticePosition,sizeof(D3DXVECTOR3)*m_dNumVertices);
			m_lpVerticeNormal = new D3DXVECTOR3[m_dNumVertices];
			ZeroMemory(m_lpVerticeNormal,sizeof(D3DXVECTOR3)*m_dNumVertices);

			m_lpVerticeTextCoord1 = new D3DXVECTOR3[m_dNumVertices];
			ZeroMemory(m_lpVerticeTextCoord1,sizeof(D3DXVECTOR3)*m_dNumVertices);
			m_lpVerticeTextCoord2 = new D3DXVECTOR3[m_dNumVertices];
			ZeroMemory(m_lpVerticeTextCoord2,sizeof(D3DXVECTOR3)*m_dNumVertices);
			m_lpVerticeTextCoord3 = new D3DXVECTOR3[m_dNumVertices];
			ZeroMemory(m_lpVerticeTextCoord3,sizeof(D3DXVECTOR3)*m_dNumVertices);

			m_lpFacesIndex = new DWORD[m_dNumFaces*3];
			ZeroMemory(m_lpFacesIndex,sizeof(DWORD)*m_dNumFaces*3);
			m_lpSubsetID = new DWORD[m_dNumFaces];
			ZeroMemory(m_lpSubsetID,sizeof(DWORD)*m_dNumFaces);

			for(DWORD i=0;i<m_dNumVertices;i++)
			{
				m_lpVerticePosition[i] = pVers[i].p;
				m_lpVerticeNormal[i]   = pVers[i].Normal;
				m_lpVerticeTextCoord1[i].x = pVers[i].tu1;
				m_lpVerticeTextCoord1[i].y = pVers[i].tv1;
				m_lpVerticeTextCoord2[i].x = pVers[i].tu2;
				m_lpVerticeTextCoord2[i].y = pVers[i].tv2;
				m_lpVerticeTextCoord3[i].x = pVers[i].tu3;
				m_lpVerticeTextCoord3[i].y = pVers[i].tv3;
			}
			for( i=0;i<m_dNumFaces;i++)
			{
				m_lpFacesIndex[i*3  ] = pIndex[i*3  ];
				m_lpFacesIndex[i*3+1] = pIndex[i*3+1];
				m_lpFacesIndex[i*3+2] = pIndex[i*3+2];
				m_lpSubsetID[i] = pAttrib[i];
			}
			break;
		}

	case (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1):
		{
			VFormat::FACES_DIFFUSE_TEXTURE1 * pVers = NULL;
			WORD* pIndex = NULL;
			DWORD * pAttrib = NULL;
			if (FAILED(m_pMeshSys->LockVertexBuffer(0,(void**)&pVers)))
				return E_FAIL;
			if (FAILED(m_pMeshSys->LockIndexBuffer (0,(void**)&pIndex)))
				return E_FAIL;
			if (FAILED(m_pMeshSys->LockAttributeBuffer(0,(DWORD**)&pAttrib)))
				return E_FAIL;

			m_lpVerticePosition = new D3DXVECTOR3[m_dNumVertices];
			ZeroMemory(m_lpVerticePosition,sizeof(D3DXVECTOR3)*m_dNumVertices);
			m_lpVerticeDiffuse = new D3DCOLOR[m_dNumVertices];
			ZeroMemory(m_lpVerticeDiffuse,sizeof(D3DCOLOR)*m_dNumVertices);

			m_lpVerticeTextCoord1 = new D3DXVECTOR3[m_dNumVertices];
			ZeroMemory(m_lpVerticeTextCoord1,sizeof(D3DXVECTOR3)*m_dNumVertices);

			m_lpFacesIndex = new DWORD[m_dNumFaces*3];
			ZeroMemory(m_lpFacesIndex,sizeof(DWORD)*m_dNumFaces*3);
			m_lpSubsetID = new DWORD[m_dNumFaces];
			ZeroMemory(m_lpSubsetID,sizeof(DWORD)*m_dNumFaces);

			for(DWORD i=0;i<m_dNumVertices;i++)
			{
				m_lpVerticePosition[i] = pVers[i].p;
				m_lpVerticeDiffuse[i]   = pVers[i].diffuse;
				m_lpVerticeTextCoord1[i].x = pVers[i].tu1;
				m_lpVerticeTextCoord1[i].y = pVers[i].tv1;
			}
			for( i=0;i<m_dNumFaces;i++)
			{
				m_lpFacesIndex[i*3  ] = pIndex[i*3  ];
				m_lpFacesIndex[i*3+1] = pIndex[i*3+1];
				m_lpFacesIndex[i*3+2] = pIndex[i*3+2];
				m_lpSubsetID[i] = pAttrib[i];
			}
			break;
		}
	case (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX2):
		{
			VFormat::FACES_DIFFUSE_TEXTURE2 * pVers = NULL;
			WORD* pIndex = NULL;
			DWORD * pAttrib = NULL;
			if (FAILED(m_pMeshSys->LockVertexBuffer(0,(void**)&pVers)))
				return E_FAIL;
			if (FAILED(m_pMeshSys->LockIndexBuffer (0,(void**)&pIndex)))
				return E_FAIL;
			if (FAILED(m_pMeshSys->LockAttributeBuffer(0,(DWORD**)&pAttrib)))
				return E_FAIL;
			m_lpVerticePosition = new D3DXVECTOR3[m_dNumVertices];
			ZeroMemory(m_lpVerticePosition,sizeof(D3DXVECTOR3)*m_dNumVertices);
			m_lpVerticeDiffuse = new D3DCOLOR[m_dNumVertices];
			ZeroMemory(m_lpVerticeDiffuse,sizeof(D3DCOLOR)*m_dNumVertices);

			m_lpVerticeTextCoord1 = new D3DXVECTOR3[m_dNumVertices];
			ZeroMemory(m_lpVerticeTextCoord1,sizeof(D3DXVECTOR3)*m_dNumVertices);
			m_lpVerticeTextCoord2 = new D3DXVECTOR3[m_dNumVertices];
			ZeroMemory(m_lpVerticeTextCoord2,sizeof(D3DXVECTOR3)*m_dNumVertices);

			m_lpFacesIndex = new DWORD[m_dNumFaces*3];
			ZeroMemory(m_lpFacesIndex,sizeof(DWORD)*m_dNumFaces*3);
			m_lpSubsetID = new DWORD[m_dNumFaces];
			ZeroMemory(m_lpSubsetID,sizeof(DWORD)*m_dNumFaces);

			for(DWORD i=0;i<m_dNumVertices;i++)
			{
				m_lpVerticePosition[i] = pVers[i].p;
				m_lpVerticeDiffuse[i]   = pVers[i].diffuse;
				m_lpVerticeTextCoord1[i].x = pVers[i].tu1;
				m_lpVerticeTextCoord1[i].y = pVers[i].tv1;
				m_lpVerticeTextCoord2[i].x = pVers[i].tu2;
				m_lpVerticeTextCoord2[i].y = pVers[i].tv2;
			}
			for( i=0;i<m_dNumFaces;i++)
			{
				m_lpFacesIndex[i*3  ] = pIndex[i*3  ];
				m_lpFacesIndex[i*3+1] = pIndex[i*3+1];
				m_lpFacesIndex[i*3+2] = pIndex[i*3+2];
				m_lpSubsetID[i] = pAttrib[i];
			}
			break;
		}
	case (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX3):
		{
			VFormat::FACES_DIFFUSE_TEXTURE3 * pVers = NULL;
			WORD* pIndex = NULL;
			DWORD * pAttrib = NULL;
			if (FAILED(m_pMeshSys->LockVertexBuffer(0,(void**)&pVers)))
				return E_FAIL;
			if (FAILED(m_pMeshSys->LockIndexBuffer (0,(void**)&pIndex)))
				return E_FAIL;
			if (FAILED(m_pMeshSys->LockAttributeBuffer(0,(DWORD**)&pAttrib)))
				return E_FAIL;
			m_lpVerticePosition = new D3DXVECTOR3[m_dNumVertices];
			ZeroMemory(m_lpVerticePosition,sizeof(D3DXVECTOR3)*m_dNumVertices);
			m_lpVerticeDiffuse = new D3DCOLOR[m_dNumVertices];
			ZeroMemory(m_lpVerticeDiffuse,sizeof(D3DCOLOR)*m_dNumVertices);

			m_lpVerticeTextCoord1 = new D3DXVECTOR3[m_dNumVertices];
			ZeroMemory(m_lpVerticeTextCoord1,sizeof(D3DXVECTOR3)*m_dNumVertices);
			m_lpVerticeTextCoord2 = new D3DXVECTOR3[m_dNumVertices];
			ZeroMemory(m_lpVerticeTextCoord2,sizeof(D3DXVECTOR3)*m_dNumVertices);
			m_lpVerticeTextCoord3 = new D3DXVECTOR3[m_dNumVertices];
			ZeroMemory(m_lpVerticeTextCoord3,sizeof(D3DXVECTOR3)*m_dNumVertices);

			m_lpFacesIndex = new DWORD[m_dNumFaces*3];
			ZeroMemory(m_lpFacesIndex,sizeof(DWORD)*m_dNumFaces*3);
			m_lpSubsetID = new DWORD[m_dNumFaces];
			ZeroMemory(m_lpSubsetID,sizeof(DWORD)*m_dNumFaces);

			for(DWORD i=0;i<m_dNumVertices;i++)
			{
				m_lpVerticePosition[i] = pVers[i].p;
				m_lpVerticeDiffuse[i]   = pVers[i].diffuse;
				m_lpVerticeTextCoord1[i].x = pVers[i].tu1;
				m_lpVerticeTextCoord1[i].y = pVers[i].tv1;
				m_lpVerticeTextCoord2[i].x = pVers[i].tu2;
				m_lpVerticeTextCoord2[i].y = pVers[i].tv2;
				m_lpVerticeTextCoord3[i].x = pVers[i].tu3;
				m_lpVerticeTextCoord3[i].y = pVers[i].tv3;
			}
			for( i=0;i<m_dNumFaces;i++)
			{
				m_lpFacesIndex[i*3  ] = pIndex[i*3  ];
				m_lpFacesIndex[i*3+1] = pIndex[i*3+1];
				m_lpFacesIndex[i*3+2] = pIndex[i*3+2];
				m_lpSubsetID[i] = pAttrib[i];
			}
			break;
		}
	case (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX1):
		{
			VFormat::FACES_NORMAL_DIFFUSE_TEXTURE1 * pVers = NULL;
			WORD* pIndex = NULL;
			DWORD * pAttrib = NULL;
			if (FAILED(m_pMeshSys->LockVertexBuffer(0,(void**)&pVers)))
				return E_FAIL;
			if (FAILED(m_pMeshSys->LockIndexBuffer (0,(void**)&pIndex)))
				return E_FAIL;
			if (FAILED(m_pMeshSys->LockAttributeBuffer(0,(DWORD**)&pAttrib)))
				return E_FAIL;
			m_lpVerticePosition = new D3DXVECTOR3[m_dNumVertices];
			ZeroMemory(m_lpVerticePosition,sizeof(D3DXVECTOR3)*m_dNumVertices);
			m_lpVerticeNormal = new D3DXVECTOR3[m_dNumVertices];
			ZeroMemory(m_lpVerticeNormal,sizeof(D3DXVECTOR3)*m_dNumVertices);
			m_lpVerticeDiffuse = new D3DCOLOR[m_dNumVertices];
			ZeroMemory(m_lpVerticeDiffuse,sizeof(D3DCOLOR)*m_dNumVertices);

			m_lpVerticeTextCoord1 = new D3DXVECTOR3[m_dNumVertices];
			ZeroMemory(m_lpVerticeTextCoord1,sizeof(D3DXVECTOR3)*m_dNumVertices);

			m_lpFacesIndex = new DWORD[m_dNumFaces*3];
			ZeroMemory(m_lpFacesIndex,sizeof(DWORD)*m_dNumFaces*3);
			m_lpSubsetID = new DWORD[m_dNumFaces];
			ZeroMemory(m_lpSubsetID,sizeof(DWORD)*m_dNumFaces);

			for(DWORD i=0;i<m_dNumVertices;i++)
			{
				m_lpVerticePosition[i] = pVers[i].p;
				m_lpVerticeDiffuse[i]   = pVers[i].diffuse;
				m_lpVerticeNormal[i]   = pVers[i].Normal;
				m_lpVerticeTextCoord1[i].x = pVers[i].tu1;
				m_lpVerticeTextCoord1[i].y = pVers[i].tv1;
			}
			for( i=0;i<m_dNumFaces;i++)
			{
				m_lpFacesIndex[i*3  ] = pIndex[i*3  ];
				m_lpFacesIndex[i*3+1] = pIndex[i*3+1];
				m_lpFacesIndex[i*3+2] = pIndex[i*3+2];
				m_lpSubsetID[i] = pAttrib[i];
			}
			break;
		}
	case (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX2):
		{
			VFormat::FACES_NORMAL_DIFFUSE_TEXTURE2 * pVers = NULL;
			WORD* pIndex = NULL;
			DWORD * pAttrib = NULL;
			if (FAILED(m_pMeshSys->LockVertexBuffer(0,(void**)&pVers)))
				return E_FAIL;
			if (FAILED(m_pMeshSys->LockIndexBuffer (0,(void**)&pIndex)))
				return E_FAIL;
			if (FAILED(m_pMeshSys->LockAttributeBuffer(0,(DWORD**)&pAttrib)))
				return E_FAIL;
			m_lpVerticePosition = new D3DXVECTOR3[m_dNumVertices];
			ZeroMemory(m_lpVerticePosition,sizeof(D3DXVECTOR3)*m_dNumVertices);
			m_lpVerticeNormal = new D3DXVECTOR3[m_dNumVertices];
			ZeroMemory(m_lpVerticeNormal,sizeof(D3DXVECTOR3)*m_dNumVertices);
			m_lpVerticeDiffuse = new D3DCOLOR[m_dNumVertices];
			ZeroMemory(m_lpVerticeDiffuse,sizeof(D3DCOLOR)*m_dNumVertices);

			m_lpVerticeTextCoord1 = new D3DXVECTOR3[m_dNumVertices];
			ZeroMemory(m_lpVerticeTextCoord1,sizeof(D3DXVECTOR3)*m_dNumVertices);
			m_lpVerticeTextCoord2 = new D3DXVECTOR3[m_dNumVertices];
			ZeroMemory(m_lpVerticeTextCoord2,sizeof(D3DXVECTOR3)*m_dNumVertices);

			m_lpFacesIndex = new DWORD[m_dNumFaces*3];
			ZeroMemory(m_lpFacesIndex,sizeof(DWORD)*m_dNumFaces*3);
			m_lpSubsetID = new DWORD[m_dNumFaces];
			ZeroMemory(m_lpSubsetID,sizeof(DWORD)*m_dNumFaces);

			for(DWORD i=0;i<m_dNumVertices;i++)
			{
				m_lpVerticePosition[i] = pVers[i].p;
				m_lpVerticeDiffuse[i]   = pVers[i].diffuse;
				m_lpVerticeNormal[i]   = pVers[i].Normal;
				m_lpVerticeTextCoord1[i].x = pVers[i].tu1;
				m_lpVerticeTextCoord1[i].y = pVers[i].tv1;
				m_lpVerticeTextCoord2[i].x = pVers[i].tu2;
				m_lpVerticeTextCoord2[i].y = pVers[i].tv2;
			}
			for( i=0;i<m_dNumFaces;i++)
			{
				m_lpFacesIndex[i*3  ] = pIndex[i*3  ];
				m_lpFacesIndex[i*3+1] = pIndex[i*3+1];
				m_lpFacesIndex[i*3+2] = pIndex[i*3+2];
				m_lpSubsetID[i] = pAttrib[i];
			}
			break;
		}
	case (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX3):
		{
			VFormat::FACES_NORMAL_DIFFUSE_TEXTURE3 * pVers = NULL;
			WORD* pIndex = NULL;
			DWORD * pAttrib = NULL;
			if (FAILED(m_pMeshSys->LockVertexBuffer(0,(void**)&pVers)))
				return E_FAIL;
			if (FAILED(m_pMeshSys->LockIndexBuffer (0,(void**)&pIndex)))
				return E_FAIL;
			if (FAILED(m_pMeshSys->LockAttributeBuffer(0,(DWORD**)&pAttrib)))
				return E_FAIL;
			m_lpVerticePosition = new D3DXVECTOR3[m_dNumVertices];
			ZeroMemory(m_lpVerticePosition,sizeof(D3DXVECTOR3)*m_dNumVertices);
			m_lpVerticeNormal = new D3DXVECTOR3[m_dNumVertices];
			ZeroMemory(m_lpVerticeNormal,sizeof(D3DXVECTOR3)*m_dNumVertices);
			m_lpVerticeDiffuse = new D3DCOLOR[m_dNumVertices];
			ZeroMemory(m_lpVerticeDiffuse,sizeof(D3DCOLOR)*m_dNumVertices);

			m_lpVerticeTextCoord1 = new D3DXVECTOR3[m_dNumVertices];
			ZeroMemory(m_lpVerticeTextCoord1,sizeof(D3DXVECTOR3)*m_dNumVertices);
			m_lpVerticeTextCoord2 = new D3DXVECTOR3[m_dNumVertices];
			ZeroMemory(m_lpVerticeTextCoord2,sizeof(D3DXVECTOR3)*m_dNumVertices);
			m_lpVerticeTextCoord3 = new D3DXVECTOR3[m_dNumVertices];
			ZeroMemory(m_lpVerticeTextCoord3,sizeof(D3DXVECTOR3)*m_dNumVertices);

			m_lpFacesIndex = new DWORD[m_dNumFaces*3];
			ZeroMemory(m_lpFacesIndex,sizeof(DWORD)*m_dNumFaces*3);
			m_lpSubsetID = new DWORD[m_dNumFaces];
			ZeroMemory(m_lpSubsetID,sizeof(DWORD)*m_dNumFaces);

			for(DWORD i=0;i<m_dNumVertices;i++)
			{
				m_lpVerticePosition[i] = pVers[i].p;
				m_lpVerticeDiffuse[i]   = pVers[i].diffuse;
				m_lpVerticeNormal[i]   = pVers[i].Normal;
				m_lpVerticeTextCoord1[i].x = pVers[i].tu1;
				m_lpVerticeTextCoord1[i].y = pVers[i].tv1;
				m_lpVerticeTextCoord2[i].x = pVers[i].tu2;
				m_lpVerticeTextCoord2[i].y = pVers[i].tv2;
				m_lpVerticeTextCoord3[i].x = pVers[i].tu3;
				m_lpVerticeTextCoord3[i].y = pVers[i].tv3;
			}
			for( i=0;i<m_dNumFaces;i++)
			{
				m_lpFacesIndex[i*3  ] = pIndex[i*3  ];
				m_lpFacesIndex[i*3+1] = pIndex[i*3+1];
				m_lpFacesIndex[i*3+2] = pIndex[i*3+2];
				m_lpSubsetID[i] = pAttrib[i];
			}
			break;
		}
	}

	if (FAILED(m_pMeshSys->UnlockVertexBuffer()))
		return E_FAIL;
	if (FAILED(m_pMeshSys->UnlockIndexBuffer()))
		return E_FAIL;
	if (FAILED(m_pMeshSys->UnlockAttributeBuffer()))
		return E_FAIL;
	//////////////////////////////////////////////////////////////////////////

	if ((FVF&D3DFVF_XYZ)&&(m_lpVerticePosition))
	{
		MeshHead.PositionBlock = ftell(pFile) - Pos_Start;

		fwrite( m_lpVerticePosition,sizeof(D3DXVECTOR3),m_dNumVertices,pFile); //顶点位置坐标
		fwrite(&BlockEnd,sizeof(DWORD),1,pFile);
	}

	if ((FVF&D3DFVF_NORMAL)&&(m_lpVerticeNormal))
	{
		MeshHead.NormalBlock = ftell(pFile) - Pos_Start;

		fwrite( m_lpVerticeNormal,sizeof(D3DXVECTOR3),m_dNumVertices,pFile); //顶点位置坐标
		fwrite(&BlockEnd,sizeof(DWORD),1,pFile);
	}

	if ((FVF&D3DFVF_DIFFUSE)&&(m_lpVerticeDiffuse))
	{
		MeshHead.DiffuseBlock = ftell(pFile) - Pos_Start;

		fwrite( m_lpVerticeDiffuse,sizeof(D3DCOLOR),m_dNumVertices,pFile); //顶点位置坐标
		fwrite(&BlockEnd,sizeof(DWORD),1,pFile);
	}

	if ((FVF&D3DFVF_TEX1)&&(m_lpVerticeTextCoord1))
	{
		MeshHead.TextureUVW1Block = ftell(pFile) - Pos_Start;

		fwrite( m_lpVerticeTextCoord1,sizeof(D3DXVECTOR3),m_dNumVertices,pFile); //顶点位置坐标
		fwrite(&BlockEnd,sizeof(DWORD),1,pFile);
	}

	if ((FVF&D3DFVF_TEX2)&&(m_lpVerticeTextCoord2))
	{
		MeshHead.TextureUVW2Block = ftell(pFile) - Pos_Start;

		fwrite( m_lpVerticeTextCoord2,sizeof(D3DXVECTOR3),m_dNumVertices,pFile); //顶点位置坐标
		fwrite(&BlockEnd,sizeof(DWORD),1,pFile);
	}

	if ((FVF&D3DFVF_TEX3)&&(m_lpVerticeTextCoord3))
	{
		MeshHead.TextureUVW3Block = ftell(pFile) - Pos_Start;

		fwrite( m_lpVerticeTextCoord3,sizeof(D3DXVECTOR3),m_dNumVertices,pFile); //顶点位置坐标
		fwrite(&BlockEnd,sizeof(DWORD),1,pFile);
	}

	if (m_lpFacesIndex)
	{
		MeshHead.FacesIndexBlock = ftell(pFile) - Pos_Start;

		fwrite( m_lpFacesIndex,sizeof(DWORD),m_dNumFaces*3,pFile);//面索引
		fwrite(&BlockEnd,sizeof(DWORD),1,pFile);
	}

	if (m_lpSubsetID)
	{
		MeshHead.SubsetIndexBlock = ftell(pFile) - Pos_Start;

		fwrite( m_lpSubsetID,sizeof(DWORD),m_dNumFaces,pFile);//面索引
		fwrite(&BlockEnd,sizeof(DWORD),1,pFile);
	}
	
	if (m_bSkined)
	{
		MeshHead.SkinInfoBlock = ftell(pFile) - Pos_Start;
		//骨骼信息
		fwrite(&m_dNumBone ,sizeof(DWORD),1,pFile);//
		for(DWORD i_Bone=0;i_Bone<m_dNumBone;i_Bone++)
		{
			LPBONEINFO pInfo = &m_lpBoneInfo[i_Bone];
			DWORD NumChild = pInfo->ChildNameVector.size();

			TCHAR Name[30];
			TCHAR ParentName[30];
			wsprintf(Name,"%s",pInfo->scName.c_str());
			wsprintf(ParentName,"%s",pInfo->scParentName.c_str());

			fwrite(Name,sizeof(TCHAR),30,pFile);//
			fwrite(ParentName,sizeof(TCHAR),30,pFile);//
			fwrite(&NumChild,sizeof(DWORD),1,pFile);//

			for(DWORD i_Child=0;i_Child<NumChild;i_Child++)
			{
				TCHAR Name[30];
				wsprintf(Name,"%s",pInfo->ChildNameVector[i_Child].c_str());
				fwrite(Name,sizeof(TCHAR),30,pFile);//
			}

			fwrite(&pInfo->MatrixOffset,sizeof(D3DMATRIX),1,pFile);//
			fwrite(&pInfo->MatrixOffsetBase,sizeof(D3DMATRIX),1,pFile);//

			fwrite(&pInfo->NumVerticesRef,sizeof(DWORD),1,pFile);//
			fwrite(pInfo->VerticesIndexRef,sizeof(DWORD),
				pInfo->NumVerticesRef,pFile);//
			fwrite(pInfo->VerticesWeightRef,sizeof(float),
				pInfo->NumVerticesRef,pFile);//
		}


		fwrite(&m_dNumSocket,sizeof(DWORD),1,pFile);
		for(DWORD i_Socket=0;i_Socket<m_dNumSocket;i_Socket++)
		{
			LPSOCKET pSocket = &m_lpSockets[i_Socket];
			TCHAR Name[30];
			TCHAR ParentName[30];
			wsprintf(Name,"%s",pSocket->scName.c_str());
			wsprintf(ParentName,"%s",pSocket->scParentBoneName.c_str());

			fwrite(Name,sizeof(TCHAR),30,pFile);//
			fwrite(ParentName,sizeof(TCHAR),30,pFile);//
			fwrite(&pSocket->MatrixOffset,sizeof(D3DMATRIX),1,pFile);//
		}

		fwrite(&BlockEnd,sizeof(DWORD),1,pFile);
	}
	SAFE_DELETE_ARRAY(m_lpVerticePosition);
	SAFE_DELETE_ARRAY(m_lpVerticeNormal);
	SAFE_DELETE_ARRAY(m_lpVerticeTextCoord1);
	SAFE_DELETE_ARRAY(m_lpVerticeTextCoord2);
	SAFE_DELETE_ARRAY(m_lpVerticeTextCoord3);
	SAFE_DELETE_ARRAY(m_lpFacesIndex);
	SAFE_DELETE_ARRAY(m_lpSubsetID);

	if ( bSaveBlendMesh && m_pBlendMesh )
	{
		MeshFileHead.ExtendBlock[BLEND_MESH_SAVED] = ftell(pFile) -Pos_Start;
		SaveBlendedMesh(m_pBlendMesh, pFile);
	}
	MeshFileHead.BlockLength = ftell(pFile) - Pos_Start;
	fseek(pFile,Pos_Start,SEEK_SET);
	fwrite(&MeshFileHead,sizeof(_MeshFileHead),1,pFile);
	fwrite(&MeshHead,sizeof(_MeshHead),1,pFile);

	return S_OK;
}

HRESULT KModel::TakeSnap(LPDIRECT3DTEXTURE9* ppTexture)
{
	HRESULT hr = S_OK;
	LPDIRECT3DTEXTURE9 m_pTexture = NULL; // Textures for Render targer mesh
	LPDIRECT3DSURFACE9 m_lpSurfaceDep = NULL ;

	DWORD    m_TextureWidth = 128;
	float    CameraDistance = -1.0f;
	D3DCOLOR m_BaseColor = 0x00AAAAAA;

	if ( FAILED(hr = g_pd3dDevice->CreateDepthStencilSurface(m_TextureWidth,m_TextureWidth,
		D3DFMT_D24S8,D3DMULTISAMPLE_NONE,0,
		TRUE,
		&m_lpSurfaceDep,NULL)))
		return hr;

	if ( FAILED(hr = D3DXCreateTexture(g_pd3dDevice,
		m_TextureWidth,m_TextureWidth,
		1,D3DUSAGE_RENDERTARGET|D3DUSAGE_AUTOGENMIPMAP,
		D3DFMT_A8R8G8B8,
		D3DPOOL_DEFAULT,
		&m_pTexture)))
		return hr;

	//////////////////////////////////////////////////////////////////////////
	//g_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0xFFFFFFFF );
	g_pd3dDevice->SetTexture(0,NULL);
	g_pd3dDevice->SetTexture(1,NULL);
	g_pd3dDevice->SetTexture(2,NULL);
	g_pd3dDevice->SetTexture(3,NULL);

	D3DVIEWPORT9 ViewPort,ViewPortSave;
	ViewPort.X = ViewPort.Y = 0;
	ViewPort.Width = ViewPort.Height = m_TextureWidth;
	ViewPort.MinZ = 0;
	ViewPort.MaxZ = 1.0f;


	g_pd3dDevice->GetViewport(&ViewPortSave);

	LPDIRECT3DSURFACE9 lpSurface = NULL;
	LPDIRECT3DSURFACE9 lpSurfaceDepSave = NULL;
	LPDIRECT3DSURFACE9 lpSurfaceSave = NULL;
	if (FAILED(hr = m_pTexture->GetSurfaceLevel(0,&lpSurface)))
		return hr;
	lpSurface->Release();


	g_pd3dDevice->GetRenderTarget(0,&lpSurfaceSave);
	lpSurfaceSave->Release();
	g_pd3dDevice->GetDepthStencilSurface(&lpSurfaceDepSave);
	lpSurfaceDepSave->Release();

	float th = timeGetTime()*0.001f;
	D3DXVECTOR3 Front(-1.0f,1.0f,-1.0f);
	D3DXVec3Normalize(&Front,&Front);
	//g_cGraphicsTool.GetCameraFront(&Front); 

	D3DXVECTOR3 vEyePt;
	D3DXVECTOR3 vLookatPt(0,0,0);
	D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
	D3DXMATRIXA16 matView,matSave;

	D3DXMATRIXA16 matProj,matProjSave;
	D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/3, 1.0f, 1, 100000.0f );
	g_pd3dDevice->GetTransform( D3DTS_PROJECTION, &matProjSave );
	g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

	CameraDistance = D3DXVec3Length(&(m_BBox_B - m_BBox_A));
	vLookatPt = (m_BBox_A+m_BBox_B)*0.5f;
	vEyePt = Front * CameraDistance + vLookatPt;

	D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
	g_pd3dDevice->GetTransform( D3DTS_VIEW, &matSave );
	g_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

	g_pd3dDevice->SetRenderTarget(0,lpSurface);
	hr = g_pd3dDevice->SetDepthStencilSurface(m_lpSurfaceDep);

	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 
		m_BaseColor, 1.0f, 0 );

	//m_pTextureRender->BeginScene(lpSurface,&ViewPort);
	g_pd3dDevice->BeginScene();

	//g_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);
	//g_pd3dDevice->SetRenderState(D3DRS_ZENABLE,TRUE);
	//g_pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
	//g_pd3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_GREATER );

	DWORD Shadow = m_dwShadow;
	m_dwShadow = 0;
	KModel::Render();
	m_dwShadow = Shadow;

	//m_pTextureRender->EndScene(D3DX_FILTER_NONE);
	g_pd3dDevice->EndScene();

	g_pd3dDevice->SetRenderTarget(0,lpSurfaceSave);
	g_pd3dDevice->SetDepthStencilSurface(lpSurfaceDepSave);
	g_pd3dDevice->SetViewport(&ViewPortSave);
	g_pd3dDevice->SetTransform( D3DTS_VIEW, &matSave );
	g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProjSave );

	//g_pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
	//g_pd3dDevice->SetRenderState(D3DRS_ZENABLE,TRUE);
	//g_pd3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESS );

	(*ppTexture) = m_pTexture;
	SAFE_RELEASE(m_lpSurfaceDep);
	return S_OK;

}

HRESULT KModel::DrawSnap(void)
{
	LPDIRECT3DTEXTURE9 pTexture = NULL;
	TakeSnap(&pTexture);
	//g_pd3dDevice->SetTexture(0,pTexture);
	D3DXVECTOR2 A(200,200);
	D3DXVECTOR2 C(400,400);
	g_cGraphicsTool.DrawScreenRect(&A,&C,0,0xFFFFFFFF,pTexture);
	SAFE_RELEASE(pTexture);

	return S_OK;
}

HRESULT KModel::SaveSnap(LPSTR pFileName)
{
	LPDIRECT3DTEXTURE9 pTexture = NULL;
	TakeSnap(&pTexture);
	D3DXSaveTextureToFile(pFileName,D3DXIFF_PNG,pTexture,NULL);
	SAFE_RELEASE(pTexture);
	return E_NOTIMPL;
}

HRESULT KModel::ShadowVolumeCreate()
{
	//D3DXVECTOR3 Light(0,0.5,-1);
	float T = timeGetTime()*0.0001f;
	D3DXVECTOR3 Light(sinf(T),0.5,-cosf(T));
	D3DXVECTOR3 ShadowLine = - Light*1000;

	DWORD NumV = m_pMeshSys->GetNumFaces()*3;
	D3DXVECTOR3* pTemp_Ver    = new D3DXVECTOR3[NumV*6];
	WORD*        pTemp_Edages = new WORD[NumV*2];
	DWORD        NumVertices = 0;
	DWORD        NumEdages = 0;

	VFormat::FACES_NORMAL_TEXTURE1 * pVers = NULL;
	WORD* pIndex = NULL;

	if (FAILED(m_pMeshSys->LockVertexBuffer(0,(void**)&pVers)))
		return E_FAIL;
	if (FAILED(m_pMeshSys->LockIndexBuffer (0,(void**)&pIndex)))
		return E_FAIL;

	//////////////////////////////////////////////////////////////////////////
	//create edages
	DWORD NumFaces = m_pMeshSys->GetNumFaces();
	for(DWORD i=0;i<NumFaces;i++)
	{
		WORD A = pIndex[i*3  ];
		WORD B = pIndex[i*3+1];
		WORD C = pIndex[i*3+2];

		D3DXVECTOR3 v0 = pVers[A].p;
		D3DXVECTOR3 v1 = pVers[B].p;
		D3DXVECTOR3 v2 = pVers[C].p;

		// Transform vertices or transform light?
		D3DXVECTOR3 vCross1(v2-v1);
		D3DXVECTOR3 vCross2(v1-v0);
		D3DXVECTOR3 vNormal;
		D3DXVec3Cross( &vNormal, &vCross1, &vCross2 );
		if ( D3DXVec3Dot( &vNormal, &Light ) >= 0.0f )
		{

			ShadowVolumeAddEdage(pTemp_Edages,&NumEdages,A,B);
			ShadowVolumeAddEdage(pTemp_Edages,&NumEdages,B,C);
			ShadowVolumeAddEdage(pTemp_Edages,&NumEdages,C,A);
		}
	}
	for(i=0;i<NumEdages;i++)
	{
		WORD A = pTemp_Edages[2*i+0];
		WORD B = pTemp_Edages[2*i+1];
		D3DXVECTOR3 Ver_A = pVers[A].p;
		D3DXVECTOR3 Ver_B = pVers[B].p;
		D3DXVECTOR3 Ver_C = Ver_B + ShadowLine;
		D3DXVECTOR3 Ver_D = Ver_A + ShadowLine;

		pTemp_Ver[i*6  ] = Ver_A;
		pTemp_Ver[i*6+1] = Ver_B;
		pTemp_Ver[i*6+2] = Ver_D;
		pTemp_Ver[i*6+3] = Ver_D;
		pTemp_Ver[i*6+4] = Ver_B;
		pTemp_Ver[i*6+5] = Ver_C;
	}

	if (FAILED(m_pMeshSys->UnlockVertexBuffer()))
		return E_FAIL;
	if (FAILED(m_pMeshSys->UnlockIndexBuffer()))
		return E_FAIL;

	m_ShadowVolume.NumVertices = NumEdages*6;
	SAFE_DELETE_ARRAY(m_ShadowVolume.EdageVertices);
	SAFE_DELETE_ARRAY(m_ShadowVolume.Edages);
	m_ShadowVolume.EdageVertices = new D3DXVECTOR3[NumEdages*6];
	memcpy(m_ShadowVolume.EdageVertices,pTemp_Ver,sizeof(D3DXVECTOR3)*NumEdages*6);
	m_ShadowVolume.Edages = new WORD[NumEdages*2];
	memcpy(m_ShadowVolume.Edages,pTemp_Edages,sizeof(WORD)*NumEdages*2);

	SAFE_DELETE_ARRAY( pTemp_Ver );
	SAFE_DELETE_ARRAY( pTemp_Edages );

	m_dwShadow = 2;
	return S_OK;
}

void KModel::ShadowVolumeAddEdage(WORD* pTemp_Edages,DWORD* pNumEdages,WORD A,WORD B)
{
	// Remove interior edges (which appear in the list twice)
	for( DWORD i=0; i < *pNumEdages; i++ )
	{
		if ( ( pTemp_Edages[2*i+0] == A && pTemp_Edages[2*i+1] == B ) ||
			( pTemp_Edages[2*i+0] == B && pTemp_Edages[2*i+1] == A ) )
		{
			if ( *pNumEdages > 1 )
			{
				pTemp_Edages[2*i+0] = pTemp_Edages[2*(*pNumEdages-1)+0];
				pTemp_Edages[2*i+1] = pTemp_Edages[2*(*pNumEdages-1)+1];
			}
			(*pNumEdages)--;
			return;
		}
	}

	pTemp_Edages[2**pNumEdages+0] = A;
	pTemp_Edages[2**pNumEdages+1] = B;
	(*pNumEdages)++;
}

HRESULT KModel::ShadowVolumeRender()
{
	// Disable z-buffer writes (note: z-testing still occurs), and enable the
	// stencil-buffer
	g_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,  FALSE );
	g_pd3dDevice->SetRenderState( D3DRS_STENCILENABLE, TRUE );

	// Dont bother with interpolating color
	g_pd3dDevice->SetRenderState( D3DRS_SHADEMODE,     D3DSHADE_FLAT );

	// Set up stencil compare fuction, reference value, and masks.
	// Stencil test passes if ((ref & mask) cmpfn (stencil & mask)) is true.
	// Note: since we set up the stencil-test to always pass, the STENCILFAIL
	// renderstate is really not needed.
	g_pd3dDevice->SetRenderState( D3DRS_STENCILFUNC,  D3DCMP_ALWAYS );
	g_pd3dDevice->SetRenderState( D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP );
	g_pd3dDevice->SetRenderState( D3DRS_STENCILFAIL,  D3DSTENCILOP_KEEP );

	// If ztest passes, inc/decrement stencil buffer value
	g_pd3dDevice->SetRenderState( D3DRS_STENCILREF,       0x1 );
	g_pd3dDevice->SetRenderState( D3DRS_STENCILMASK,      0xffffffff );
	g_pd3dDevice->SetRenderState( D3DRS_STENCILWRITEMASK, 0xffffffff );
	g_pd3dDevice->SetRenderState( D3DRS_STENCILPASS,      D3DSTENCILOP_INCR );

	// Make sure that no pixels get drawn to the frame buffer
	g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	g_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ZERO );
	g_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

	if ( ( g_D3DCaps.StencilCaps & D3DSTENCILCAPS_TWOSIDED ) != 0 )
	{
		// With 2-sided stencil, we can avoid rendering twice:
		g_pd3dDevice->SetRenderState( D3DRS_TWOSIDEDSTENCILMODE, TRUE );
		g_pd3dDevice->SetRenderState( D3DRS_CCW_STENCILFUNC,  D3DCMP_ALWAYS );
		g_pd3dDevice->SetRenderState( D3DRS_CCW_STENCILZFAIL, D3DSTENCILOP_KEEP );
		g_pd3dDevice->SetRenderState( D3DRS_CCW_STENCILFAIL,  D3DSTENCILOP_KEEP );
		g_pd3dDevice->SetRenderState( D3DRS_CCW_STENCILPASS, D3DSTENCILOP_DECR );

		g_pd3dDevice->SetRenderState( D3DRS_CULLMODE,  D3DCULL_NONE );

		// Draw both sides of shadow volume in stencil/z only
		ShadowVolumeDraw();

		g_pd3dDevice->SetRenderState( D3DRS_TWOSIDEDSTENCILMODE, FALSE );
	}
	else
	{
		// Draw front-side of shadow volume in stencil/z only
		ShadowVolumeDraw();

		// Now reverse cull order so back sides of shadow volume are written.
		g_pd3dDevice->SetRenderState( D3DRS_CULLMODE,   D3DCULL_CW );

		// Decrement stencil buffer value
		g_pd3dDevice->SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_DECR );

		// Draw back-side of shadow volume in stencil/z only
		ShadowVolumeDraw();
	}

	// Restore render states
	g_pd3dDevice->SetRenderState( D3DRS_SHADEMODE, D3DSHADE_GOURAUD );
	g_pd3dDevice->SetRenderState( D3DRS_CULLMODE,  D3DCULL_CCW );
	g_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,     TRUE );
	g_pd3dDevice->SetRenderState( D3DRS_STENCILENABLE,    FALSE );
	g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

	return S_OK;}

HRESULT KModel::ShadowVolumeDraw()
{
	g_pd3dDevice->SetFVF( D3DFVF_XYZ );
	g_pd3dDevice->SetTexture(0,NULL);
	g_pd3dDevice->SetTexture(1,NULL);

	return g_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLELIST, m_ShadowVolume.NumVertices/3,
		m_ShadowVolume.EdageVertices, sizeof(D3DXVECTOR3) );

}

HRESULT KModel::LoadBoneBlendSetFromMeshFile(LPSTR pFileName)
{
	_MeshFileHead MeshFileHead;

	DWORD BlockEnd  = 0xFFFFFFFF;  
	FILE* pFile = fopen(pFileName,"rb");
	if (pFile==NULL) return E_FAIL;

	DWORD Pos_Start = ftell(pFile);

	fread(&MeshFileHead,sizeof(_MeshFileHead),1,pFile);
	if (MeshFileHead.FileMask!=0x4D455348) return E_FAIL;

	DWORD m_dNumVertices = 0;
	DWORD m_dNumFaces    = 0;
	DWORD m_dNumSubsets  = 0;
	BOOL  m_bSkined      = 0;

	D3DXVECTOR3 * m_lpVerticePosition = NULL;
	D3DXVECTOR3 * m_lpVerticeNormal   = NULL;
	D3DCOLOR    * m_lpVerticeDiffuse  = NULL;

	D3DXVECTOR3 * m_lpVerticeTextCoord1 = NULL;
	D3DXVECTOR3 * m_lpVerticeTextCoord2 = NULL;
	D3DXVECTOR3 * m_lpVerticeTextCoord3 = NULL;

	DWORD * m_lpFacesIndex = NULL;
	DWORD * m_lpSubsetID   = NULL;

	_MeshHead* pMeshHead = new _MeshHead[MeshFileHead.MeshCount];
	for(DWORD i_Mesh=0;i_Mesh<MeshFileHead.MeshCount;i_Mesh++)
	{
		fread(&pMeshHead[i_Mesh],sizeof(_MeshHead),1,pFile);

		m_bSkined = pMeshHead[i_Mesh].SkinInfoBlock;//是否包含骨骼信息

		fseek(pFile,Pos_Start+pMeshHead[i_Mesh].SkinInfoBlock,SEEK_SET);

		if (m_bSkined)
		{
			//FILE* Note = fopen("D:\\Blend.txt","at");

			DWORD NumBone = 0;
			fread(&NumBone,sizeof(DWORD),1,pFile);//骨骼数目

			LPBONEBLENDSET pBBSet = new BONEBLENDSET;
			pBBSet->NumBones = NumBone;
			wsprintf(pBBSet->SetName,"%s",pFileName);
			pBBSet->BoneBlendInfos = new BONEBLENDINFO[NumBone];

			for(DWORD i_Bone=0;i_Bone<NumBone;i_Bone++)
			{
				TCHAR Name[256];
				TCHAR ParentName[256];
				TCHAR ChildName[256];
				DWORD NumChild;
				D3DXMATRIX  MatrixOffset,MatrixBaseOffset;
				D3DXMATRIX  MatTemp ;

				fread(Name,sizeof(TCHAR),30,pFile);	
				fread(ParentName,sizeof(TCHAR),30,pFile);	

				fread(&NumChild,sizeof(DWORD),1,pFile);	

				for(DWORD i_Child=0;i_Child<NumChild;i_Child++)
				{
					fread(ChildName,sizeof(TCHAR),30,pFile);//
				}

				fread(&MatrixOffset,sizeof(D3DXMATRIX),1,pFile);//
				fread(&MatrixBaseOffset,sizeof(D3DXMATRIX),1,pFile);//

				DWORD NumVerticesIndexRef = 0;
				DWORD VerticesIndex = 0;
				float VerticesWeight = 0;

				fread(&NumVerticesIndexRef,sizeof(DWORD),1,pFile);//

				for(int k=0;k<(int)NumVerticesIndexRef;k++)
				{
					fread(&VerticesIndex,sizeof(DWORD),1,pFile);//
				}
				for( k=0;k<(int)NumVerticesIndexRef;k++)
				{
					fread(&VerticesWeight,sizeof(float),1,pFile);//
				}

				int BoneIndex = FindBone(Name);
				if (BoneIndex>=0)
				{
					LPBONEINFO pInfo = &m_lpBoneInfo[BoneIndex];
					pBBSet->BoneBlendInfos[i_Bone].
						Create(Name,&MatrixBaseOffset);
					//pInfo->BoneLength = pBBSet->BoneBlendInfos[i_Bone].BoneLength;

					D3DXVECTOR3 Scaling,Position;

					MatrixExract(Scaling,Position,pInfo->Rotation,
						&MatrixBaseOffset);

					pInfo->BonePosition = Position;
					//pInfo->BoneScale = Scaling;

					//fprintf(Note,"%d %s  Length %f \n",i_Bone,pInfo->Name,pInfo->BoneLength);
					//pInfo->BoneLength = pInfo->BaseBoneLength*1.1f;
				}
			}

			m_lpBoneBlendSet.push_back(pBBSet);

			//fprintf(Note,"\n");
			//fclose(Note);
		}
	}

	fread(&BlockEnd,sizeof(DWORD),1,pFile);
	fclose(pFile);



	return S_OK;
}

HRESULT KModel::BoneBlendProcess()
{

	if (m_nNumCurBlendSet<=0)
		return S_OK;
	float f = sinf(timeGetTime()*0.1f)*0.5f + 0.5f;

	/*if (f<0)
	f = 1;
	else
	f = 0;*/

	m_fBoneBlendsetWeight[0] = f;
	m_fBoneBlendsetWeight[1] = 1.0f - f;

	D3DXMATRIX  MatrixOffset;

	for(DWORD i_Bone=0;i_Bone<m_dNumBone;i_Bone++)
	{
		LPBONEINFO pInfo = &m_lpBoneInfo[i_Bone];

		float  BoneLength = 0;
		for(int i_Set=0;i_Set<m_nNumCurBlendSet;i_Set++)
		{
			int   SetIndex  = m_dwBoneBlendsetID[i_Set];
			float SetWeight = m_fBoneBlendsetWeight[i_Set];

			LPBONEBLENDSET pSet = m_lpBoneBlendSet[SetIndex];
			if (i_Set==0)
			{
				BoneLength = pSet->BoneBlendInfos[i_Bone].BoneLength * SetWeight;
			}
			else
			{
				BoneLength += pSet->BoneBlendInfos[i_Bone].BoneLength * SetWeight;
			}
		}
		//pInfo->BoneLength = BoneLength;

	}
	return S_OK;
}

HRESULT KModel::UpdataRelativeBoneMatrix(int BoneIndex,D3DXMATRIX Matrix)
{
	if ( BoneIndex == -1 )
		return S_OK;
	LPBONEINFO pInfo = &m_lpBoneInfo[BoneIndex];
	pInfo->CurMatrix = pInfo->RelativeMatrix * Matrix;
	
	for(DWORD i_Child=0;i_Child<pInfo->ChildIDVector.size();i_Child++)
	{
		int ChildIndex = pInfo->ChildIDVector[i_Child];
		UpdataRelativeBoneMatrix(ChildIndex,pInfo->CurMatrix);
	}
	return S_OK;
}

HRESULT KModel::UpdataRelativeBoneMatrix()
{
	D3DXMATRIX Matrix;
	D3DXMatrixIdentity(&Matrix);
	for(int S=0;S<(int)m_nBaseBoneID.size();S++)
	{
		UpdataRelativeBoneMatrix(m_nBaseBoneID[S],Matrix);
	}
	return S_OK;
}

void KModel::BONEINFO::UpdateCurMatrix()
{
	D3DXMATRIX   MatTemp;
	D3DXVECTOR3  Scale = BoneScale;
	D3DXVECTOR3  Pos = BonePosition ;
	D3DXMATRIX   MatScale,MatRotation,MatPos;

	D3DXMatrixScaling(&MatScale,Scale.x,Scale.y,Scale.z);
	D3DXMatrixTranslation(&MatPos,Pos.x,Pos.y,Pos.z);

	RelativeMatrix = MatScale * BaseRotation * MatPos;
}

HRESULT KModel::SetMatrix()
{
	g_pd3dDevice->GetTransform( D3DTS_WORLD , &m_MatrixSave);

	if (m_dOption & MESH_BIND)
	{
		int SocketIndex = m_pBindMesh->FindSocket(m_pBindSocketName);
		if (SocketIndex>=0)
		{
			LPSOCKET pSocket= &m_pBindMesh->m_lpSockets[SocketIndex];
			m_Matrix = pSocket->CurMatrix;
		}
	}


	switch(m_nBillBoardMethod) 
	{
	case BILLBOARD_NONE:
		{
			if ((m_dOption&MESH_SKINED)&&(m_lpBoneInfo!=NULL))
			{
				D3DXMatrixMultiply(&m_CurMatrix,&m_Matrix,&m_MatrixSave);
				g_pd3dDevice->SetTransform( D3DTS_WORLD , &m_MatrixSave);	
			}
			else
			{
				D3DXMatrixMultiply(&m_CurMatrix,&m_Matrix,&m_MatrixSave);
				g_pd3dDevice->SetTransform( D3DTS_WORLD , &m_CurMatrix);	
			}
			break;
		}
	case BILLBOARD_POSITION_ONLY:
		{
			D3DXMatrixMultiply(&m_CurMatrix,&m_Matrix,&m_MatrixSave);
			D3DXVECTOR3 Position;

			Position.x = m_CurMatrix._41;
			Position.y = m_CurMatrix._42;
			Position.z = m_CurMatrix._43;

			D3DXMatrixTranslation(&m_CurMatrix,Position.x,Position.y,Position.z);
			g_pd3dDevice->SetTransform( D3DTS_WORLD , &m_CurMatrix );
			break;
		}
	case BILLBOARD_NORMAL:
		{
			D3DXMATRIX MatTemp;
			D3DXVECTOR3 vDir3D;
			g_cGraphicsTool.GetCameraFront(&vDir3D);

			if ( vDir3D.x < 0.0f )
				D3DXMatrixRotationY( &MatTemp, -atanf(vDir3D.z/vDir3D.x)+D3DX_PI/2 );
			else
				D3DXMatrixRotationY( &MatTemp, -atanf(vDir3D.z/vDir3D.x)-D3DX_PI/2 );

			D3DXMATRIX mat1;
			D3DXVECTOR2 vDir2D(vDir3D.x,vDir3D.z);
			float C = D3DXVec2Length(&vDir2D);

			D3DXVECTOR3 Axis;
			D3DXVec3Cross(&Axis,&D3DXVECTOR3(vDir3D.x,0,vDir3D.z),&vDir3D);

			if ( vDir3D.y < 0.0f )
				D3DXMatrixRotationAxis( &mat1,&Axis, -atanf(vDir3D.y/C) - D3DX_PI/2);
			else
				D3DXMatrixRotationAxis( &mat1,&Axis,  atanf(vDir3D.y/C) + D3DX_PI/2);

			D3DXMatrixMultiply(&MatTemp,&MatTemp,&mat1);

			D3DXMatrixMultiply(&m_CurMatrix,&m_Matrix,&m_MatrixSave);

			D3DXVECTOR3 Position;

			Position.x = m_CurMatrix._41;
			Position.y = m_CurMatrix._42;
			Position.z = m_CurMatrix._43;

			D3DXMatrixTranslation(&m_CurMatrix,Position.x,Position.y,Position.z);
			D3DXMatrixMultiply(&m_CurMatrix,&MatTemp,&m_CurMatrix);
			g_pd3dDevice->SetTransform( D3DTS_WORLD , &m_CurMatrix );
			break;
		}
	}

	return S_OK;
}

HRESULT KModel::RestoreMatrix()
{
	g_pd3dDevice->SetTransform( D3DTS_WORLD , &m_MatrixSave);
	return S_OK;
}

HRESULT KModel::InvalidateDeviceObjects(void)
{
	//SAFE_RELEASE( m_pPRTEffect );
	//SAFE_RELEASE( m_pSHIrradEnvMapEffect );
	//SAFE_RELEASE( m_pNDotLEffect );
	if ( m_pPRTEffect)
		m_pPRTEffect->OnLostDevice();
	if ( m_pSHIrradEnvMapEffect)
		m_pSHIrradEnvMapEffect->OnLostDevice();
	if ( m_pNDotLEffect)
		m_pNDotLEffect->OnLostDevice();
	return E_NOTIMPL;
}

HRESULT KModel::RestoreDeviceObjects(void)
{
	if( m_pPRTEffect )
		( m_pPRTEffect->OnLostDevice() );
	if( m_pSHIrradEnvMapEffect )
		( m_pSHIrradEnvMapEffect->OnLostDevice() );
	if( m_pNDotLEffect )
		( m_pNDotLEffect->OnLostDevice() );
	return E_NOTIMPL;
}

HRESULT KModel::Save2XFile(LPSTR pFileName)
{
	TCHAR path_buffer[256];
	TCHAR drive[256];
	TCHAR dir[256];
	TCHAR fname[256];
	TCHAR ext[256];

	wsprintf(path_buffer,"%s",pFileName);
	_splitpath( path_buffer, drive, dir, fname, ext );

	TCHAR Path[256];
	wsprintf(Path,"%s%s",drive,dir);

	HRESULT hr = S_OK;
	LPD3DXBUFFER m_pAdjacencyBuffer = NULL;   // Contains the adjaceny info loaded with the mesh
	LPD3DXMESH pMesh = m_pMeshSys;
	(m_dOption&MESH_OPTIMIZED)? pMesh = m_pMeshOpt : pMesh = m_pMeshSys;
	if ( FAILED( hr = D3DXCreateBuffer(pMesh->GetNumFaces()*3*sizeof(DWORD),
		&m_pAdjacencyBuffer)))
		return hr;
	if ( FAILED( hr = pMesh->GenerateAdjacency(0,
		(DWORD*)m_pAdjacencyBuffer->GetBufferPointer())))
		return hr;

	D3DXMATERIAL* Material = new D3DXMATERIAL[m_dNumMaterial];

	for(DWORD i=0;i<m_dNumMaterial;i++)
	{
		LPMATERIAL pMat = &m_lpMaterial[i];
		Material[i].MatD3D = pMat->m_sMaterial9;

		Material[i].pTextureFilename = NULL;

		if(pMat->m_dTextureIDs[0])
		{
			LPTEXTURE pText = NULL;
			g_cTextureTable.GetTexture(&pText,pMat->m_dTextureIDs[0]);

			if(pText)
			{
				TCHAR path_buffer[256];
				TCHAR drive[256];
				TCHAR dir[256];
				TCHAR fname[256];
				TCHAR ext[256];

				wsprintf(path_buffer,"%s",pText->m_lpFileName);
				_splitpath( path_buffer, drive, dir, fname, ext );

				TCHAR TextFileName[256];
				wsprintf(TextFileName,"%s%s",fname,ext);

				Material[i].pTextureFilename = new TCHAR[256];
				wsprintf(Material[i].pTextureFilename,"%s",TextFileName);

				TCHAR NewTextFileName[256];
				wsprintf(NewTextFileName,"%s%s",Path,TextFileName);

				CopyFile(NewTextFileName,pText->m_lpFileName,FALSE);

			}
		}
	}

	hr = D3DXSaveMeshToX(pFileName,
		pMesh,
		(DWORD*)m_pAdjacencyBuffer->GetBufferPointer(),
		Material,
		NULL,
		m_dNumMaterial,
		D3DXF_FILEFORMAT_BINARY);

	for(DWORD i=0;i<m_dNumMaterial;i++)
	{
		SAFE_DELETE_ARRAY(Material[i].pTextureFilename);
	}
	SAFE_RELEASE(m_pAdjacencyBuffer);
	SAFE_DELETE_ARRAY(Material);

	return hr;
}

HRESULT KModel::RenderShadow(void)
{
	if (m_dwShadow==1)
	{
		PlaneShadowRender();
	}
	else if (m_dwShadow==2)
	{
		ShadowVolumeCreate();
		ShadowVolumeRender();
	}

	return E_NOTIMPL;
}


////////////////////////////柔体部分//////////////////////////////////
/*
说明：
骨骼的运动基本完善，共实现两种效果。
1、整体牵动的柔体骨骼运动，适合衣服等大片块的柔体；
2、用绳子来模拟柔体的骨骼运动，适合剑穗、辫子等类似绳子运动的柔体。
◎但尚不能有效解决网格间的碰撞检测问题；
◎尚未将效果调节参数传至界面；
*/

BOOL KModel::UpdateFlexibleBone(LPBONEINFO pInfo,int i,int i_Flex)
{
	int select;   //传到界面的柔体效果选择参数
	select=0;
	switch(select)
	{
	case 0:
		//整体牵动的柔体骨骼运动，适合衣服等大片块的柔体
		UpdateFlexibleBone0(pInfo,i,i_Flex);
		break;
	case 1:
		//整体牵动的柔体骨骼运动，适合衣服等大片块的柔体
		UpdateFlexibleBone1(pInfo,i,i_Flex);
		break;

	case 2:
		//用绳子来模拟柔体的骨骼运动，适合剑穗、辫子等类似绳子运动的柔体
		UpdateFlexibleBone2(pInfo,i,i_Flex);  

		break;

	default:
		return FALSE;
		break;
	}

	return TRUE;
}

/**
* @brief 产生柔体飘动的效果，随风动以及随物体运动的飘动
* @param pInfo 骨骼
* @param i 该骨骼位于骨骼链中的第几根
* @param fWaveDegree （界面参数）该骨骼随风飘起的程度，取值在[0，1]之间
* @return 
*/

BOOL KModel::UpdateFlexibleBone0(LPBONEINFO pInfo,int i,int i_Flex)
{
	LPBONEINFO pParent = NULL;    //父亲骨骼
	if (( pInfo->nParentIndex >=0 ) /*&& ( i > 0 )*/)
	{ 
		pParent = &m_lpBoneInfo[pInfo->nParentIndex];
		float time = 0;
		LPSCENE pScene = g_cGraphicsTool.GetCurScene();
		if(pScene)
		{
			time = pScene->m_TimeControl.GetNowTime();
		}
		D3DXMATRIX Rot;
		D3DXVECTOR3 Scale,Position;
		MatrixExract(Scale,Position,Rot,&pInfo->RelativeMatrix);
		D3DXMatrixRotationX(&Rot,sinf(time*0.003f+0.4f*i)*0.1f+D3DX_PI);

		D3DXMATRIX Mat = pInfo->RelativeMatrix/* * Rot*/;

		float K = time*0.0055f+0.7f*i;

		Mat._43 += sinf(K)*0.8F*i;
		Mat._42 += cosf(K)*0.8F*i;

		/*D3DXMatrixIdentity(&Mat);
		Mat._41 = pInfo->RelativeMatrix._41;
		Mat._42 = pInfo->RelativeMatrix._42;
		Mat._43 = pInfo->RelativeMatrix._43;*/

		pInfo->CurMatrix = Mat * pParent->CurMatrix;
		//D3DXMatrixMultiply( &pInfo->CurMatrix,&pParent->CurMatrix,&pInfo->MatrixOffsetBase);
	}

	for ( int i_C=0; i_C < (int)pInfo->ChildIDVector.size(); i_C ++ )//递归循环
	{  
		int BoneIndex = pInfo->ChildIDVector[i_C];
		if ((BoneIndex >= 0) && (BoneIndex < (int)m_dNumBone))
		{
			LPBONEINFO pChild = &m_lpBoneInfo[BoneIndex];
			UpdateFlexibleBone( pChild, i+1, i_Flex);  
		}
	} 
	return TRUE;
}

BOOL KModel::UpdateFlexibleBone1(LPBONEINFO pInfo,int i,int i_Flex)
{
	const float fDegree = 0.1f;  //正弦飘动的振幅，参数
	const float fMrap = 0.001f;  //随物体运动飘起的缓慢度，参数
	float fInfect = 0.5f;       //随风飘动的幅度，参数
	float fMInfect = 0.1f;       //随运动飘动的幅度，参数
	//	int nCollision =0;       //是否检测，裙子与腿的碰撞检测
	float fWaveDegree=m_vecStaticFlexibleBoneIndex[i_Flex].fWaveDegree;

	//Conver是柔体骨骼的根节点在从上一帧位置到下一帧位置的变换矩阵，MatrixFlexWold在UpdateAnimation()函数中计算
	D3DXMATRIX Conver=m_vecStaticFlexibleBoneIndex[i_Flex].MatrixFlexWold;

	float K = sinf( timeGetTime() * -0.002f + i*4 ) * (2); //产生正弦飘动的效果

	D3DXVECTOR3 WindForce = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );   //风作用的矢量
	D3DXVECTOR3 MoveForce = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );   //物体运动作用的矢量
	D3DXVECTOR3 Noise = D3DXVECTOR3( 0, 1, 0 );   //正弦飘动的矢量
	D3DXVec3Normalize( &Noise, &Noise );

	/*
	//裙子与腿的碰撞检测,设定刚体

	D3DXVECTOR3 Nor,Nor2;  //刚体的轴矢量
	D3DXVECTOR3 Position;  
	Position = D3DXVECTOR3( 0, 0, 0 );  
	Nor = D3DXVECTOR3( 0, 1, 0 );

	float fHeigh,fRadius;  //刚体的半高和半径
	fRadius = 2.5f;
	fHeigh = (sqrt((m_BBox_B.x-m_BBox_A.x)*(m_BBox_B.x-m_BBox_A.x)+(m_BBox_B.y-m_BBox_A.y)*(m_BBox_B.y-m_BBox_A.y)+(m_BBox_B.z-m_BBox_A.z)*(m_BBox_B.z-m_BBox_A.z)))/2.0f;
	//设定刚体
	Physics::LPRIGIDBODY pRigid1 = NULL;
	Physics::LPRIGIDBODY pRigid2 = NULL;
	Physics::LPRIGIDBODY pRigid3 = NULL;

	pRigid1 = new Physics::KRigidBody;
	pRigid2 = new Physics::KRigidBody;
	pRigid3 = new Physics::KRigidBody;

	Physics::KPhysicsSimulator  m_Sim;

	Position = Position + Nor * ( fHeigh );   //刚体中心点的位置
	pRigid1->SetSP( Position, Nor, fHeigh, fRadius);   //设定刚体 
	*/

	LPBONEINFO pParent = NULL;    //父亲骨骼

	D3DXMATRIX tempM;
	D3DXVECTOR3 tempV; 
	D3DXVECTOR3 NewPos;

	D3DXMatrixMultiply( &pInfo->LocalWorldCurMatrix,&pInfo->LocalWorldCurMatrix,&Conver);	
	D3DXMatrixMultiply( &pInfo->CurMatrix,&pInfo->LocalWorldCurMatrix,&m_Matrix);
	D3DXVec3TransformCoord( &pInfo->BaseBonePosition,&pInfo->BaseBonePosition,&Conver);

	if (( pInfo->nParentIndex >=0 ) && ( i > 0 ))
	{ 
		pParent = &m_lpBoneInfo[pInfo->nParentIndex];
		NewPos = (pInfo->BaseBonePosition-pParent->BaseBonePosition)+pInfo->BoneLength * Noise * K *fDegree;
		D3DXVec3Normalize( &NewPos, &NewPos );
		NewPos = NewPos * pInfo->BoneLength;

		if ((m_nCurAnimationIndex<0)||((DWORD)m_nCurAnimationIndex>=m_dNumAnimation))   //是否需要风吹动的效果
		{
			//产生风吹渐进飘起落下的效果		
			WindForce = fWaveDegree* ( g_cGraphicsTool.m_PreWind ); 
			fInfect = fInfect + float(i * 0.1);

			D3DXMatrixInverse(  &tempM, NULL, &m_Matrix );
			D3DXVec3TransformCoord( &WindForce, &WindForce, &tempM );

			//计算骨骼相对父亲骨骼的新位置（即偏移量）
			NewPos =pInfo->BoneLength *(WindForce)*(fInfect) + NewPos;	
			D3DXVec3Normalize( &NewPos, &NewPos );
			NewPos = NewPos * pInfo->BoneLength;
		}

		D3DXVECTOR3 Position1;
		Position1.x = m_lpBoneInfo[m_vecStaticFlexibleBoneIndex[i_Flex].Index].CurMatrix._41;
		Position1.y = m_lpBoneInfo[m_vecStaticFlexibleBoneIndex[i_Flex].Index].CurMatrix._42;
		Position1.z = m_lpBoneInfo[m_vecStaticFlexibleBoneIndex[i_Flex].Index].CurMatrix._43;

		pParent = &m_lpBoneInfo[pInfo->nParentIndex];

		//产生随运动渐进飘起落下的效果
		m_vecStaticFlexibleBoneIndex[i_Flex].MoveForce =-Position1 + m_vecStaticFlexibleBoneIndex[i_Flex].PrePosition;
		MoveForce =m_vecStaticFlexibleBoneIndex[i_Flex].MoveForce * fWaveDegree;
		m_vecStaticFlexibleBoneIndex[i_Flex].PrePosition+=0.04f*(Position1 - m_vecStaticFlexibleBoneIndex[i_Flex].PrePosition);

		//简单的解决碰撞检测问题的方法，此处代码暂未写完整
		int select;
		select = 0;   // 参数
		switch(select)
		{
		case 1:  
			//柔体位于身体前方
			if( MoveForce.z<0 )  MoveForce.z=0;	
			break;

		case 2:
			//柔体位于身体后方
			if( MoveForce.z>0 )  MoveForce.z=0; 

			break;
		case 3:
			//柔体位于身体左面 
			if( MoveForce.x>0 )  MoveForce.x=0; 

			break;
		case 4:
			//柔体位于身体右面
			if( MoveForce.x<0 )  MoveForce.x=0; 

			break;		

		default:

			break;
		}



		fMInfect = fMInfect + float(i * 0.2);
		///////////////////////测试代码/////////////////////////////
		if( pInfo->BoneLength>20)
		{
			if( MoveForce.y>0 ){
				MoveForce.z=MoveForce.z+MoveForce.y;
				MoveForce.y=0;
			} 
			fMInfect = fMInfect*4.0f;
		}
		////////////////////////////////////////////////


		//计算骨骼相对父亲骨骼的新位置（即偏移量）
		NewPos =(MoveForce)*(fMInfect)+NewPos;	
		D3DXVec3Normalize( &NewPos, &NewPos );
		NewPos = NewPos * pInfo->BoneLength;

		/*	
		//设定刚体
		Nor2 = NewPos;
		D3DXVec3Normalize( &Nor2, &Nor2);
		tempV.x=pParent->LocalWorldCurMatrix._41;
		tempV.y=pParent->LocalWorldCurMatrix._42;
		tempV.z=pParent->LocalWorldCurMatrix._43;
		Position = tempV + Nor2 * (pInfo->BoneLength / 2.0f); ////刚体中心点的位置
		pRigid2->SetSP( Position, Nor2, pInfo->BoneLength / 2.0f, 0.2f );    //设定刚体

		//		g_cGraphicsTool.DrawSP( pRigid1->pSp );
		//		g_cGraphicsTool.DrawSP( pRigid2->pSp ); 

		//碰撞后柔体骨骼的运动处理		
		if ( m_Sim.CheckCollision( pRigid1, pRigid2 ) && nCollision)
		{
		if ( i > 1 )
		{ //柔体不为第一跟骨骼时，紧贴身体刚体表面

		D3DXVECTOR3 OffSet, B;
		OffSet.x = pInfo->LocalWorldCurMatrix._41 - pParent->LocalWorldCurMatrix._41;
		OffSet.y = pInfo->LocalWorldCurMatrix._42 - pParent->LocalWorldCurMatrix._42;
		OffSet.z = pInfo->LocalWorldCurMatrix._43 - pParent->LocalWorldCurMatrix._43;
		B.x = ( pInfo->BaseBonePosition.x);
		B.z = ( pInfo->BaseBonePosition.z);
		B.y = 0;
		D3DXVECTOR3 C( 0, 1, 0);

		D3DXVec3Cross( &B, &B, &C );
		D3DXVec3Normalize( &B, &B );

		B = ( D3DXVec3Dot( &OffSet, &B ) - D3DXVec3Dot( &NewPos, &B ) ) * (-B);
		NewPos = D3DXVECTOR3( 0, -1, 0);
		NewPos = NewPos * pInfo->BoneLength;
		//	NewPos = NewPos + B;
		D3DXVec3Normalize( &NewPos, &NewPos );
		NewPos = NewPos * pInfo->BoneLength;
		pInfo->LocalWorldCurMatrix._41 = pParent->LocalWorldCurMatrix._41 + NewPos.x;
		pInfo->LocalWorldCurMatrix._42 = pParent->LocalWorldCurMatrix._42 + NewPos.y;
		pInfo->LocalWorldCurMatrix._43 = pParent->LocalWorldCurMatrix._43 + NewPos.z;			
		}
		else
		{//柔体为第一跟骨骼时，骨骼下端点紧贴身体刚体表面
		D3DXVECTOR3 OffSet, B;
		OffSet.x = pInfo->LocalWorldCurMatrix._41 - pParent->LocalWorldCurMatrix._41;
		OffSet.y = pInfo->LocalWorldCurMatrix._42 - pParent->LocalWorldCurMatrix._42;
		OffSet.z = pInfo->LocalWorldCurMatrix._43 - pParent->LocalWorldCurMatrix._43;
		B.x = ( pInfo->BaseBonePosition.x);
		B.z = ( pInfo->BaseBonePosition.z);
		B.y = 0;
		D3DXVECTOR3 C( 0, 1, 0);
		D3DXVec3Cross( &B, &B, &C );
		D3DXVec3Normalize( &B, &B );

		B = ( D3DXVec3Dot( &OffSet, &B ) - D3DXVec3Dot( &NewPos, &B ) ) * (-B);
		NewPos = OffSet + B;
		D3DXVec3Normalize( &NewPos, &NewPos );
		NewPos = NewPos * pInfo->BoneLength; 
		pInfo->LocalWorldCurMatrix._41 = pParent->LocalWorldCurMatrix._41 + NewPos.x;
		pInfo->LocalWorldCurMatrix._42 = pParent->LocalWorldCurMatrix._42 + NewPos.y;
		pInfo->LocalWorldCurMatrix._43 = pParent->LocalWorldCurMatrix._43 + NewPos.z; 

		} 
		}
		else
		{  */
		pInfo->LocalWorldCurMatrix._41 = pParent->LocalWorldCurMatrix._41 + NewPos.x;
		pInfo->LocalWorldCurMatrix._42 = pParent->LocalWorldCurMatrix._42 + NewPos.y;
		pInfo->LocalWorldCurMatrix._43 = pParent->LocalWorldCurMatrix._43 + NewPos.z; 


		//	}

		D3DXMatrixMultiply( &pInfo->CurMatrix, &pInfo->LocalWorldCurMatrix, &m_Matrix );	
	}  

	/*	delete pRigid1;
	delete pRigid2;
	delete pRigid3;
	pRigid1 = NULL;
	pRigid2 = NULL;
	pRigid3 = NULL;   */
	i++;   
	for ( int i_C=0; i_C < (int)pInfo->ChildIDVector.size(); i_C ++ )
	{  //递归循环
		int BoneIndex = pInfo->ChildIDVector[i_C];
		if ((BoneIndex >= 0) && (BoneIndex < (int)m_dNumBone))
		{
			LPBONEINFO pChild = &m_lpBoneInfo[BoneIndex];
			UpdateFlexibleBone( pChild, i, i_Flex);  
		}
	} 
	return TRUE;
}
BOOL KModel::UpdateFlexibleBone2(LPBONEINFO pInfo,int i,int i_Flex)
{
	//用绳子来模拟柔体的骨骼运动，适合剑穗、辫子等类似绳子运动的柔体
	//模拟绳子的类Krope,在KRope.h中。
	//绳子sping物理模拟中的各参数在LoadMesh（）函数中设置
	D3DXMATRIX Conver=m_vecStaticFlexibleBoneIndex[i_Flex].MatrixFlexWold;
	D3DXVECTOR3 Position1,Position2;
	Position2.x = pInfo->CurMatrix._41;
	Position2.y = pInfo->CurMatrix._42;
	Position2.z = pInfo->CurMatrix._43;

	D3DXMatrixMultiply( &pInfo->LocalWorldCurMatrix,&pInfo->LocalWorldCurMatrix,&Conver);	
	D3DXMatrixMultiply( &pInfo->CurMatrix,&pInfo->LocalWorldCurMatrix,&m_Matrix);

	Position1.x = pInfo->CurMatrix._41;
	Position1.y = pInfo->CurMatrix._42;
	Position1.z = pInfo->CurMatrix._43;


	D3DXVECTOR3 ropeConnectionVel = D3DXVECTOR3( 0, 0, 0);
	ropeConnectionVel.x += Position1.x - Position2.x;
	ropeConnectionVel.y += Position1.y - Position2.y;
	ropeConnectionVel.z += Position1.z - Position2.z;

	m_vecStaticFlexibleBoneIndex[i_Flex].ropeSimulation->setRopeConnectionVel(ropeConnectionVel);				// Set The Obtained ropeConnectionVel In The Simulation

	///////////////此段代码用于调整绳子模拟中通过a值来计算新位置中的dt值//////////////////////
	///////////dt和nn均为调节参数///////////////////////////
	float dt = 1.0f/30.0f;										// Let's Convert Milliseconds To Seconds
	float maxPossible_dt = 0.002f;											// Maximum Possible dt Is 0.002 Seconds
	// This Is Needed To Prevent Pass Over Of A Non-Precise dt Value

	int numOfIterations = (int)(dt / maxPossible_dt) + 1;					// Calculate Number Of Iterations To Be Made At This Update Depending On maxPossible_dt And dt
	if (numOfIterations != 0)												// Avoid Division By Zero
		dt = dt / numOfIterations;											// dt Should Be Updated According To numOfIterations
	int nn = 100;
	/////////////////////////////
	for (int a = 0; a < nn; ++a)								// We Need To Iterate Simulations "numOfIterations" Times
		m_vecStaticFlexibleBoneIndex[i_Flex].ropeSimulation->operate(dt,Position1);

	LPBONEINFO pIn = pInfo;
	int j=0;
	Water::Mass* mass1;

	while ( (int)pIn->ChildIDVector.size() > 0 )
	{
		mass1 = m_vecStaticFlexibleBoneIndex[i_Flex].ropeSimulation->getMass(j);
		D3DXVECTOR3* pos1 = &mass1->pos;
		pIn->CurMatrix._41 = pos1->x;
		pIn->CurMatrix._42 = pos1->y;
		pIn->CurMatrix._43 = pos1->z;
		int BoneIndex = pIn->ChildIDVector[0];
		if(BoneIndex==-1)
		{
			break;
		}
		j++;
		pIn = &m_lpBoneInfo[BoneIndex];
	}

	mass1 = m_vecStaticFlexibleBoneIndex[i_Flex].ropeSimulation->getMass(j);
	D3DXVECTOR3* pos1 = &mass1->pos;
	pIn->CurMatrix._41 = pos1->x;
	pIn->CurMatrix._42 = pos1->y;
	pIn->CurMatrix._43 = pos1->z;
	return TRUE;
}
//////////////////////////////////////////////////////////////////////////
//切割模型
//////////////////////////////////////////////////////////////////////////

/**
* @brief 对边界点进行三角剖分
* @param vec_EdgeVertex 切割后的边界点
* @param NumEdgeVertex 边界点的个数
* @param pEdgeIndex 返回三角剖分后的三角形带
* @return 0表示失败，非0 表示三角剖分后的生成的三角形面个数
*/


int KModel::TripTriangulation(const vector <EdgeVertex> &vec_EdgeVertex, int nNumEdgeVertex, WORD **ppwEdgeIndex, int *pnRetNumEdgeFace)
{
	int nResult = false;

	///对edgevertex区分成不同的环，同时进行排序
	vector <EdgeVertex>  vec_EdgeRing[100];//假设最多有100个边环
	EdgeVertex *pCurrentVer = NULL;
	BOOL *b_EdgeVertex = NULL;
	int i = 0;
	int m = 0;
	int k = 0;
	int nNumEdgeFace = 0;
	int j=0;
	int nCurrentFace,nCurrentFace2;
	DWORD dwNumCurrentRingVer = 0;
	DWORD dwNumEdge = 0;
	WORD wFlag = 0;

	b_EdgeVertex = new BOOL[nNumEdgeVertex];
	if (!b_EdgeVertex)
		goto Exit0;

	pCurrentVer = new EdgeVertex();
	if ( !pCurrentVer )
		goto Exit0;

	for (i = 0;i < nNumEdgeVertex; i++)
	{
		b_EdgeVertex[i] = FALSE;
	}

	///m用来计数已遍历整个边界点，k用来计数遍历的边环，i用来计数当前边环中的点

	( * pCurrentVer ) = vec_EdgeVertex[0];
	nCurrentFace = pCurrentVer->ParentFace[0];	
	nCurrentFace2 = pCurrentVer->ParentFace[1];	///如果取得第一个节点不是边界点，则它在两个面中
	for ( k = 0, i=0 ; (k < 100) && (m < nNumEdgeVertex); k++ )
	{			
		///以currentVer为当前环的首结点，遍历当前环所有点

		vec_EdgeRing[k].push_back( *pCurrentVer );
		b_EdgeVertex[i] = TRUE;
		m++;
		for ( j=1;( j < nNumEdgeVertex ) && ( m < nNumEdgeVertex ) ;j++ )
		{
			for ( i = 1;i< ( nNumEdgeVertex ) && ( m < nNumEdgeVertex ) ;i++ )
			{///遍历edgeVertex.找到下一个关联点
				if ( b_EdgeVertex[i] == FALSE )
					if ( ( vec_EdgeVertex[i].ParentFace[0] == nCurrentFace ) || ( vec_EdgeVertex[i].ParentFace[1] == nCurrentFace ) )
					{
						vec_EdgeRing[k].push_back( vec_EdgeVertex[i] );
						( * pCurrentVer ) = vec_EdgeVertex[i];			
						b_EdgeVertex[i] = TRUE;
						m++;
						if ( pCurrentVer->ParentFace[1] == -1 )
						{
							if ( nCurrentFace2 != -1 ) 
							{///如果currentVer是边界点，只包含在一个面中，则如果环的起始点不是边界点，从currentFace2开始再搜索
								nCurrentFace = nCurrentFace2;
								nCurrentFace2 = -1;
								break;
							}	
							else if ( nCurrentFace2 == -1 )
							{///起始点是边界点或者已经从搜索完起始点的两个方向，则终止环的循环遍历搜索
								j = nNumEdgeVertex;
								break;
							}
						}
						else 
						{///改变currentFace
							if ( nCurrentFace == pCurrentVer->ParentFace[0] )
								nCurrentFace = pCurrentVer->ParentFace[1];
							else
								nCurrentFace = pCurrentVer->ParentFace[0];				
							break;
						}
					}

			}
		}
		if ( m < nNumEdgeVertex )
		{
			///如果还有点没有遍历，则寻找下一个环的首结点
			for ( i = 1 ; i < nNumEdgeVertex ; i++ )
			{
				if ( b_EdgeVertex[i] == FALSE )
				{				
					( * pCurrentVer ) = vec_EdgeVertex[i];///找到，并置为currentVer			
					nCurrentFace = pCurrentVer->ParentFace[0];
					nCurrentFace2 = pCurrentVer->ParentFace[1];
					//b_EdgeVertex[i]=TRUE;
					break;
				}
			}
		}

	}///	for (k=0;k<10 && m< NumEdgeVertex;k++)



	//////////////////////////对已序点进行三角剖分///////////////////////////////////////



	/*
	//*********************采用隔点逐次连接三角形发进行剖分***************************

	for (i=0;i<k;i++)
	{///分环进行三角剖分
	for (m=1,NumEdge=0;( NumEdge<=vec_EdgeRing[i].size()-3 ) &&( vec_EdgeRing[i].size()>=3) && ( 2*m< vec_EdgeRing[i].size() );)
	{//一个环内的弦个数为 n(v)-3,用NumEdge来记录
	//m为增量权值		
	for (DWORD j=0;((j+2*m)<vec_EdgeRing[i].size())  &&( NumEdge<=vec_EdgeRing[i].size()-3 );NumEdge++)
	{
	pEdgeIndex[3*NumEdgeFace]=vec_EdgeRing[i][j].Index;
	pEdgeIndex[3*NumEdgeFace+1]=vec_EdgeRing[i][j+1*m].Index;
	pEdgeIndex[3*NumEdgeFace+2]=vec_EdgeRing[i][j+2*m].Index;
	j=j+2*m;
	NumEdgeFace++;
	}
	m=2*m;
	}
	}

	*/
	//////////////////////////////采用三角形带方法进行三角剖分/////////////////////////////

	//NumCurrentRingVer=vec_EdgeRing[0].size();
	for ( m = 0,j = 0 ; m < k ; m++ )
	{
		dwNumCurrentRingVer = vec_EdgeRing[m].size();
		///i记录三角形带下方的点，j记录三角形带上方的点
		for ( dwNumEdge = 0,i = 0,j = dwNumCurrentRingVer - 1; ( dwNumEdge < ( dwNumCurrentRingVer - 3 ) ) && ( dwNumCurrentRingVer >= 3 ); dwNumEdge++, nNumEdgeFace++)
		{
			if ( wFlag == 0 )
			{
				( * ppwEdgeIndex )[3 * nNumEdgeFace] = vec_EdgeRing[m][i].Index;
				( * ppwEdgeIndex )[3 * nNumEdgeFace + 1] = vec_EdgeRing[m][j-1].Index;
				( * ppwEdgeIndex )[3 * nNumEdgeFace + 2] = vec_EdgeRing[m][j].Index;
				j--;
				wFlag = 1;

			}
			else if ( wFlag == 1 )
			{
				( * ppwEdgeIndex )[3*nNumEdgeFace] = vec_EdgeRing[m][i].Index;
				( * ppwEdgeIndex )[3*nNumEdgeFace + 1] = vec_EdgeRing[m][i+1].Index;
				( * ppwEdgeIndex )[3*nNumEdgeFace + 2] = vec_EdgeRing[m][j].Index;
				i++;
				wFlag = 0;				
			}
		}
	}

	( * pnRetNumEdgeFace ) = nNumEdgeFace;
	nResult = true;

	///内存分配不足退出
Exit0:


	for ( i = 0; i < 100; i++ )
	{	
		vec_EdgeRing[i].clear();
	}

	if ( b_EdgeVertex )
	{
		delete []b_EdgeVertex;
		b_EdgeVertex = NULL;
	}

	if ( pCurrentVer )
	{
		delete pCurrentVer;
		pCurrentVer = NULL;
	}

	return nResult;
}


int   KModel::CullByPlane(KModel** mod_Top,KModel** mod_Bottom,D3DXPLANE* pPlane)
{
	int nResult = false;
	///inital vector
	vector <VFormat::FACES_NORMAL_TEXTURE1> vec_NewVers;/// for new Vertex
	vector <EdgeVertex> vec_EdgeVertex;///for edge vertex

	D3DXVECTOR3 t_vector;//临时变量，用于计算两点的距离	
	VFormat::FACES_NORMAL_TEXTURE1 t_NewVers;
	EdgeVertex t_EdgeVertex;//临时存储边点

	if( ( D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1) != m_pMeshSys->GetFVF() )
		goto Exit1;

	g_cMeshTable.Get1NewXMesh(mod_Top,MODELTYPE_DEFAULT);
	g_cMeshTable.Get1NewXMesh(mod_Bottom,MODELTYPE_DEFAULT);

	///边点



	///得到网格的vertex和face的Num值	
	DWORD NumVertex = m_pMeshSys->GetNumVertices();
	DWORD NumFaces  = m_pMeshSys->GetNumFaces();


	//用于计数	
	DWORD NumFacesTop  = 0;
	DWORD NumFacesBottom  = 0;
	DWORD NumNewVertex=0;
	///用于计算两点比例


	float prop1=0;
	float prop2=0;
	float dist1,dist2;	
	///初始化布尔数组
	BOOL* bTopFaces = NULL;
	bTopFaces = new BOOL[NumFaces];
	if ( !bTopFaces )
		goto Exit0;
	BOOL* bBottomFaces = NULL;
	bBottomFaces = new BOOL[NumFaces];
	if ( !bBottomFaces )
		goto Exit0;

	for (DWORD l=0;l<NumFaces;l++)
	{
		bTopFaces[l]=FALSE;
		bBottomFaces[l]=FALSE;
	}

	//用于处理分裂后新产生的面
	WORD* pSplitIndexTop = NULL;
	pSplitIndexTop = new WORD[4000];
	if ( !pSplitIndexTop )
		goto Exit0;

	DWORD * pSplitAttribTop = NULL;
	DWORD NumSplitTop=0;

	WORD* pSplitIndexBottom = NULL;
	pSplitIndexBottom = new WORD[4000];
	if ( !pSplitIndexBottom )
		goto Exit0;

	DWORD * pSplitAttribBottom = NULL;
	DWORD NumSplitBottom=0;



	FLOAT fEdgeVertex;
	WORD P1,P2,T;

	///用于计算临接点
	LPD3DXBUFFER m_pAdjacencyBuffer = NULL;   // Contains the adjaceny info loaded with the mesh
	if ( FAILED( D3DXCreateBuffer(m_pMeshSys->GetNumFaces()*3*sizeof(DWORD),
		&m_pAdjacencyBuffer)))
		goto Exit0;
	if ( FAILED(  m_pMeshSys->GenerateAdjacency(0,
		(DWORD*)m_pAdjacencyBuffer->GetBufferPointer())))
		goto Exit0;

	DWORD * pAdjacen = (DWORD*)m_pAdjacencyBuffer->GetBufferPointer();

	///用于判断顶点，是否已经被列入边点,因为一个点要关联两个面
	OriginEdgeVer * pOriginEdgeVer = NULL;
	pOriginEdgeVer = new OriginEdgeVer[NumVertex];
	if ( !pOriginEdgeVer )
		goto Exit0;



	//访问网格(访问前必须作Lock操作，最后做UnLock操作)
	VFormat::FACES_NORMAL_TEXTURE1 * pVers = NULL;
	WORD* pIndex = NULL;
	DWORD * pAttrib = NULL;
	if (FAILED(m_pMeshSys->LockVertexBuffer(0,(void**)&pVers)))
		goto Exit0;
	if (FAILED(m_pMeshSys->LockIndexBuffer (0,(void**)&pIndex)))
		goto Exit0;
	if (FAILED(m_pMeshSys->LockAttributeBuffer(0,(DWORD**)&pAttrib)))
		goto Exit0;


	//访问每个Face,计算边点，为topface,bottomface赋值。
	for(DWORD i=0;i<NumFaces;i++)
	{
		WORD A,B,C;
		DWORD SubsetID;
		A = pIndex[i*3  ];
		B = pIndex[i*3+1];
		C = pIndex[i*3+2];
		SubsetID = pAttrib[i];
		int n;

		DWORD F_A,F_B,F_C;
		F_A=pAdjacen[3*i];
		F_B=pAdjacen[3*i+1];
		F_C=pAdjacen[3*i+2];

		D3DXVECTOR3 Pos_A = pVers[A].p;
		D3DXVECTOR3 Pos_B = pVers[B].p;
		D3DXVECTOR3 Pos_C = pVers[C].p;

		float tu_A = pVers[A].tu1;
		float tu_B = pVers[B].tu1;
		float tu_C = pVers[C].tu1;
		float tv_A = pVers[A].tv1;
		float tv_B = pVers[B].tv1;
		float tv_C = pVers[C].tv1;


		//为每个顶点计算平面的距离D
		FLOAT d_A=pPlane->a*Pos_A.x+pPlane->b*Pos_A.y+pPlane->c*Pos_A.z+pPlane->d;
		FLOAT d_B=pPlane->a*Pos_B.x+pPlane->b*Pos_B.y+pPlane->c*Pos_B.z+pPlane->d;
		FLOAT d_C=pPlane->a*Pos_C.x+pPlane->b*Pos_C.y+pPlane->c*Pos_C.z+pPlane->d;
		FLOAT d_plane=d_A*d_B*d_C; 

		//判断该face是否与plane相交
		if ((d_A>0)&&(d_B>0)&&(d_C>0))
		{
			//该三角面位于plane的正法向量上
			bTopFaces[i] = TRUE;
			NumFacesTop++;
		}
		else if ((d_A<0)&&(d_B<0)&&(d_C<0))
		{
			//该三角面位于plane的负法向量上
			bBottomFaces[i] = TRUE;
			NumFacesBottom ++;
		}

		else if (d_A==0)
		{
			///顶点A在平面上,设置A为边点
			if (pOriginEdgeVer[A].EdgeVer==FALSE)
			{
				t_EdgeVertex.Vertex=pVers[A];
				t_EdgeVertex.ParentFace[0]=i;
				t_EdgeVertex.Index=A;
				vec_EdgeVertex.push_back(t_EdgeVertex);
				pOriginEdgeVer[A].EdgeVer=TRUE;
				pOriginEdgeVer[A].VerIndex=vec_EdgeVertex.size()-1;
			}			
			else
			{	
				n= pOriginEdgeVer[A].VerIndex;
				vec_EdgeVertex[n].ParentFace[1]=i;
			}
			if (d_B==0)
			{
				//顶点b也在平面上
				//将B记录为边点
				if (pOriginEdgeVer[B].EdgeVer==FALSE)
				{
					t_EdgeVertex.Vertex=pVers[B];	
					t_EdgeVertex.ParentFace[0]=i;
					t_EdgeVertex.Index=B;
					vec_EdgeVertex.push_back(t_EdgeVertex);
					pOriginEdgeVer[B].EdgeVer=TRUE;
					pOriginEdgeVer[B].VerIndex=vec_EdgeVertex.size()-1;
				}			
				else
				{	
					n= pOriginEdgeVer[B].VerIndex;
					vec_EdgeVertex[n].ParentFace[1]=i;
				}

				if (d_C>0)
				{
					bTopFaces[i]=TRUE;
					NumFacesTop++;
				}
				else if (d_C<0)
				{
					//该三角面在负法向量上
					bBottomFaces[i] = TRUE;
					NumFacesBottom ++;
				}
			}
			//A,C两点在平面上
			else if (d_C==0)
			{
				if (pOriginEdgeVer[A].EdgeVer==FALSE)
				{
					t_EdgeVertex.Vertex=pVers[C];	
					t_EdgeVertex.ParentFace[0]=i;
					t_EdgeVertex.Index=C;
					vec_EdgeVertex.push_back(t_EdgeVertex);
					pOriginEdgeVer[C].EdgeVer=TRUE;
					pOriginEdgeVer[C].VerIndex=vec_EdgeVertex.size()-1;
				}			
				else
				{	
					n= pOriginEdgeVer[C].VerIndex;
					vec_EdgeVertex[n].ParentFace[1]=i;
				}

				if (d_B>0)
				{
					//该三角面在正法向量上
					bTopFaces[i] = TRUE;
					NumFacesTop++;
				}
				else if (d_B<0)
				{
					//该三角面在负法向量上
					bBottomFaces[i] = TRUE;
					NumFacesBottom ++;
				}
			}
			else //b,c!=0
			{

				if (d_B>0&&d_C>0)
				{
					//该三角面在正法向量上
					bTopFaces[i] = TRUE;
					NumFacesTop ++;
				}
				else if (d_B<0 && d_C<0)
				{
					//该三角面在负法向量上
					bBottomFaces[i] = TRUE;
					NumFacesBottom ++;					
				}

				else //在b-c边上有边点t插入
				{
					//					if ((F_B<NumFaces)&&(F_B>=0))
					//					{
					if (F_B<i)
					{
						//如果该临接边对应的临接面index小于i,则在该边上已经产生了边点
						for (l=0;l<vec_EdgeVertex.size();l++)
						{
							if( (vec_EdgeVertex[l].ParentFace[0]==F_B)&&(vec_EdgeVertex[l].ParentFace[1]==i) )
							{							
								T=vec_EdgeVertex[l].Index;
								break;
							}
						}	
					}
					else 
					{
						//B,C两点分别位于平面的两侧,原三角面分裂
						//计算该边点的p值并插入链表
						fEdgeVertex=pPlane->a*Pos_B.x+pPlane->b*Pos_B.y+pPlane->c*Pos_B.z+pPlane->d;
						fEdgeVertex/=pPlane->a*(Pos_B.x-Pos_C.x)+pPlane->b*(Pos_B.y-Pos_C.y)+pPlane->c*(Pos_B.z-Pos_C.z);

						t_NewVers.p.x=Pos_B.x+(Pos_C.x-Pos_B.x)*fEdgeVertex;
						t_NewVers.p.y=Pos_B.y+(Pos_C.y-Pos_B.y)*fEdgeVertex;
						t_NewVers.p.z=Pos_B.z+(Pos_C.z-Pos_B.z)*fEdgeVertex;

						t_vector = t_NewVers.p - Pos_B;
						dist1 = D3DXVec3Length(&t_vector);


						t_vector = Pos_B - Pos_C;
						dist2 = D3DXVec3Length(&t_vector);

						prop1=dist1/dist2; prop2=1-prop1;

						t_NewVers.tu1= prop1*tu_B+prop2*tu_C;
						t_NewVers.tv1= prop1*tv_B+prop2*tv_C;
						t_NewVers.Normal=prop1*pVers[B].Normal+prop2*pVers[C].Normal;

						vec_NewVers.push_back(t_NewVers);

						T=(WORD)(NumVertex+NumNewVertex);//t's index value						

						t_EdgeVertex.Vertex=t_NewVers;
						t_EdgeVertex.ParentFace[0]=i;
						t_EdgeVertex.Index=T;
						t_EdgeVertex.ParentFace[1]=F_B;
						vec_EdgeVertex.push_back(t_EdgeVertex);

						// add new vertex and edge vertex
						NumNewVertex++;
					}
					//					}//if ((F_B<NumFaces)&&(F_B>=0))
					//					else 
					//					{///该F_B不存在，该边为边界边
					//						
					//					}
					//三角面片分裂成两个

					if (d_B>0)
					{
						//三角形ABt在top 
						pSplitIndexTop[3*NumSplitTop]=A;
						pSplitIndexTop[3*NumSplitTop+1]=T;//t
						pSplitIndexTop[3*NumSplitTop+2]=B;

						NumSplitTop++;NumFacesTop++;
						//三角形AtC	,bottom
						pSplitIndexBottom[3*NumSplitBottom]=A;
						pSplitIndexBottom[3*NumSplitBottom+1]=T;
						pSplitIndexBottom[3*NumSplitBottom+2]=C;//t

						NumSplitBottom++;NumFacesBottom ++;
					}
					else if (d_B<0)
					{
						//三角形ABt,在bottom
						pSplitIndexBottom[3*NumSplitBottom]=A;
						pSplitIndexBottom[3*NumSplitBottom+1]=B;
						pSplitIndexBottom[3*NumSplitBottom+2]=T;//t

						NumSplitBottom++;NumFacesBottom ++;						

						//三角形AtC	
						pSplitIndexTop[3*NumSplitTop]=A;
						pSplitIndexTop[3*NumSplitTop+1]=T;//t
						pSplitIndexTop[3*NumSplitTop+2]=C;

						NumSplitTop++;NumFacesTop++;
					}

				}
			}
		}
		else if (d_A>0)
		{
			if (d_B==0)
			{
				///记下b为边点
				if (pOriginEdgeVer[B].EdgeVer==FALSE)
				{
					t_EdgeVertex.Vertex=pVers[B];	
					t_EdgeVertex.ParentFace[0]=i;
					t_EdgeVertex.Index=B;
					vec_EdgeVertex.push_back(t_EdgeVertex);
					pOriginEdgeVer[B].EdgeVer=TRUE;
					pOriginEdgeVer[B].VerIndex=vec_EdgeVertex.size()-1;
				}			
				else
				{	
					n= pOriginEdgeVer[B].VerIndex;
					vec_EdgeVertex[n].ParentFace[1]=i;
				}

				if (d_C==0)
				{
					//将C记录为边点
					if (pOriginEdgeVer[C].EdgeVer==FALSE)
					{
						t_EdgeVertex.Vertex=pVers[C];	
						t_EdgeVertex.ParentFace[0]=i;
						t_EdgeVertex.Index=C;
						vec_EdgeVertex.push_back(t_EdgeVertex);
						pOriginEdgeVer[C].EdgeVer=TRUE;
						pOriginEdgeVer[C].VerIndex=vec_EdgeVertex.size()-1;
					}			
					else
					{	
						n= pOriginEdgeVer[C].VerIndex;
						vec_EdgeVertex[n].ParentFace[1]=i;
					}					
					//该三角形处于top
					bTopFaces[i] = TRUE;
					NumFacesTop ++;
				}
				else if (d_C>0)
				{
					//该三角形处于top
					bTopFaces[i] = TRUE;
					NumFacesTop ++;
				}
				else if (d_C<0)
				{				
					//A,C两点分别位于平面的两侧,原三角面分裂
					if (F_C<i)
					{
						//如果该临接边对应的临接面index小于i,则在该边上已经产生了边点
						for (l=0;l<vec_EdgeVertex.size();l++)
						{
							if( (vec_EdgeVertex[l].ParentFace[0]==F_C)&&(vec_EdgeVertex[l].ParentFace[1]==i))
							{
								//vec_EdgeVertex[l].ParentFace[1]==i;
								T=vec_EdgeVertex[l].Index;
								break;
							}
						}						
					}
					else 
					{					
						//计算该边点并插入链表
						fEdgeVertex=pPlane->a*Pos_A.x+pPlane->b*Pos_A.y+pPlane->c*Pos_A.z+pPlane->d;
						fEdgeVertex/=pPlane->a*(Pos_A.x-Pos_C.x)+pPlane->b*(Pos_A.y-Pos_C.y)+pPlane->c*(Pos_A.z-Pos_C.z);
						t_NewVers.p.x=Pos_A.x+(Pos_C.x-Pos_A.x)*fEdgeVertex;
						t_NewVers.p.y=Pos_A.y+(Pos_C.y-Pos_A.y)*fEdgeVertex;
						t_NewVers.p.z=Pos_A.z+(Pos_C.z-Pos_A.z)*fEdgeVertex;

						t_vector = t_NewVers.p - Pos_A;
						dist1 = D3DXVec3Length(&t_vector);

						t_vector = Pos_A - Pos_C;
						dist2 = D3DXVec3Length(&t_vector);

						prop1=dist1/dist2; prop2=1-prop1;

						t_NewVers.tu1= prop1*tu_A+prop2*tu_C;
						t_NewVers.tv1= prop1*tv_A+prop2*tv_C;
						t_NewVers.Normal=prop1*pVers[A].Normal+prop2*pVers[C].Normal;
						T=(WORD)(NumVertex+NumNewVertex);//t's index

						vec_NewVers.push_back(t_NewVers);
						t_EdgeVertex.Vertex=t_NewVers;///add edgevertex
						t_EdgeVertex.ParentFace[0]=i;
						t_EdgeVertex.ParentFace[1]=F_C;
						t_EdgeVertex.Index=T;
						vec_EdgeVertex.push_back( t_EdgeVertex);					

						//三角面片分裂成两个
						// add new vertex and edge vertex
						NumNewVertex++;

					}
					//三角形ABt,in top 
					pSplitIndexTop[3*NumSplitTop]=A;
					pSplitIndexTop[3*NumSplitTop+1]=B;
					pSplitIndexTop[3*NumSplitTop+2]=T;//t

					NumSplitTop++;NumFacesTop++;
					//三角形tBC in bottom	
					pSplitIndexBottom[3*NumSplitBottom]=T;//t
					pSplitIndexBottom[3*NumSplitBottom+1]=B;
					pSplitIndexBottom[3*NumSplitBottom+2]=C;

					NumSplitBottom++;NumFacesBottom ++;			

				}
			}
			else if (d_C==0)
			{
				//将C记录为边点
				if (pOriginEdgeVer[C].EdgeVer==FALSE)
				{
					t_EdgeVertex.Vertex=pVers[C];
					t_EdgeVertex.ParentFace[0]=i;
					t_EdgeVertex.Index=C;
					vec_EdgeVertex.push_back(t_EdgeVertex);
					pOriginEdgeVer[C].EdgeVer=TRUE;
					pOriginEdgeVer[C].VerIndex=vec_EdgeVertex.size()-1;
				}			
				else
				{	
					n= pOriginEdgeVer[C].VerIndex;
					vec_EdgeVertex[n].ParentFace[1]=i;
				}					


				if (d_B>0)
				{
					//该三角形处于top
					bTopFaces[i] = TRUE;
					NumFacesTop ++;
				}
				else if (d_B<0)
				{
					//A,B两点分别位于平面的两侧,原三角面分裂
					if (F_A<i)
					{
						//如果该临接边对应的临接面index小于i,则在该边上已经产生了边点
						for (l=0;l<vec_EdgeVertex.size();l++)
						{
							if ((vec_EdgeVertex[l].ParentFace[0]==F_A)&&(vec_EdgeVertex[l].ParentFace[1]==i))
							{
								//vec_EdgeVertex[l].ParentFace[1]==i;
								T=vec_EdgeVertex[l].Index;
								break;
							}
						}						
					}
					else 
					{
						//计算该边点并插入链表
						fEdgeVertex=pPlane->a*Pos_A.x+pPlane->b*Pos_A.y+pPlane->c*Pos_A.z+pPlane->d;
						fEdgeVertex/=pPlane->a*(Pos_A.x-Pos_B.x)+pPlane->b*(Pos_A.y-Pos_B.y)+pPlane->c*(Pos_A.z-Pos_B.z);
						t_NewVers.p.x=Pos_A.x+(Pos_B.x-Pos_A.x)*fEdgeVertex;
						t_NewVers.p.y=Pos_A.y+(Pos_B.y-Pos_A.y)*fEdgeVertex;
						t_NewVers.p.z=Pos_A.z+(Pos_B.z-Pos_A.z)*fEdgeVertex;


						t_vector = t_NewVers.p - Pos_A;
						dist1 = D3DXVec3Length(&t_vector);

						t_vector = Pos_A - Pos_B;
						dist2 = D3DXVec3Length(&t_vector);

						prop1=dist1/dist2; prop2=1-prop1;

						t_NewVers.tu1= prop1*tu_A+prop2*tu_B;
						t_NewVers.tv1= prop1*tv_A+prop2*tv_B;
						t_NewVers.Normal=prop1*pVers[A].Normal+prop2*pVers[B].Normal;
						T=(WORD)(NumVertex+NumNewVertex);//t						

						vec_NewVers.push_back(t_NewVers);
						t_EdgeVertex.Vertex=t_NewVers;///add edgevertex
						t_EdgeVertex.ParentFace[0]=i;
						t_EdgeVertex.ParentFace[1]=F_A;
						t_EdgeVertex.Index=T;
						vec_EdgeVertex.push_back( t_EdgeVertex);					
						// add new vertex and edge vertex
						NumNewVertex++;
					}
					//三角面片分裂成两个

					//三角形AtC in top
					pSplitIndexTop[3*NumSplitTop]=A;
					pSplitIndexTop[3*NumSplitTop+1]=T;//t
					pSplitIndexTop[3*NumSplitTop+2]=C;

					NumSplitTop++;NumFacesTop++;
					//三角形tBC	in bottom
					pSplitIndexBottom[3*NumSplitBottom]=A;
					pSplitIndexBottom[3*NumSplitBottom+1]=T;
					pSplitIndexBottom[3*NumSplitBottom+2]=C;//t

					NumSplitBottom++;NumFacesBottom ++;
				}

			}
			else //d_B!=0 && d_C!=0
			{
				if (d_B>0)//d_C<0
				{
					//a,b 位于top,c位于bottom,原三角面分裂
					//计算该A-C边点并插入链表
					if (F_C<i)
					{
						//如果该临接边对应的临接面index小于i,则在该边上已经产生了边点
						for (l=0;l<vec_EdgeVertex.size();l++)
						{
							if( (vec_EdgeVertex[l].ParentFace[0]==F_C)&&(vec_EdgeVertex[l].ParentFace[1]==i))
							{
								//vec_EdgeVertex[l].ParentFace[1]==i;
								P1=vec_EdgeVertex[l].Index;
								break;
							}
						}						
					}
					else 
					{
						fEdgeVertex=pPlane->a*Pos_A.x+pPlane->b*Pos_A.y+pPlane->c*Pos_A.z+pPlane->d;
						fEdgeVertex/=pPlane->a*(Pos_A.x-Pos_C.x)+pPlane->b*(Pos_A.y-Pos_C.y)+pPlane->c*(Pos_A.z-Pos_C.z);
						t_NewVers.p.x=Pos_A.x+(Pos_C.x-Pos_A.x)*fEdgeVertex;
						t_NewVers.p.y=Pos_A.y+(Pos_C.y-Pos_A.y)*fEdgeVertex;
						t_NewVers.p.z=Pos_A.z+(Pos_C.z-Pos_A.z)*fEdgeVertex;

						t_vector = t_NewVers.p - Pos_A;
						dist1 = D3DXVec3Length(&t_vector);

						t_vector = Pos_A - Pos_C;
						dist2 = D3DXVec3Length(&t_vector);


						prop1=dist1/dist2; prop2=1-prop1;

						t_NewVers.tu1= prop1*tu_A+prop2*tu_C;
						t_NewVers.tv1= prop1*tv_A+prop2*tv_C;
						t_NewVers.Normal=prop1*pVers[A].Normal+prop2*pVers[C].Normal;

						P1=(WORD)(NumVertex+NumNewVertex);
						vec_NewVers.push_back(t_NewVers);
						t_EdgeVertex.Vertex=t_NewVers;///add edgevertex
						t_EdgeVertex.ParentFace[0]=i; 
						t_EdgeVertex.ParentFace[1]=F_C;
						t_EdgeVertex.Index=P1;
						vec_EdgeVertex.push_back( t_EdgeVertex);					
						// add new vertex and edge vertex
						NumNewVertex++;
					}

					//计算该B-C边点并插入链表
					if (F_B<i)
					{
						//如果该临接边对应的临接面index小于i,则在该边上已经产生了边点
						for (l=0;l<vec_EdgeVertex.size();l++)
						{
							if( (vec_EdgeVertex[l].ParentFace[0]==F_B)&&(vec_EdgeVertex[l].ParentFace[1]==i))
							{
								//vec_EdgeVertex[l].ParentFace[1]==i;
								P2=vec_EdgeVertex[l].Index;
								break;
							}
						}						
					}
					else 
					{	
						fEdgeVertex=pPlane->a*Pos_B.x+pPlane->b*Pos_B.y+pPlane->c*Pos_B.z+pPlane->d;
						fEdgeVertex/=pPlane->a*(Pos_B.x-Pos_C.x)+pPlane->b*(Pos_B.y-Pos_C.y)+pPlane->c*(Pos_B.z-Pos_C.z);
						t_NewVers.p.x=Pos_B.x+(Pos_C.x-Pos_B.x)*fEdgeVertex;
						t_NewVers.p.y=Pos_B.y+(Pos_C.y-Pos_B.y)*fEdgeVertex;
						t_NewVers.p.z=Pos_B.z+(Pos_C.z-Pos_B.z)*fEdgeVertex;


						t_vector = t_NewVers.p - Pos_B;
						dist1 = D3DXVec3Length(&t_vector);

						t_vector = Pos_B - Pos_C;
						dist2 = D3DXVec3Length(&t_vector);


						prop1=dist1/dist2; prop2=1-prop1;

						t_NewVers.tu1= prop1*tu_B+prop2*tu_C;
						t_NewVers.tv1= prop1*tv_B+prop2*tv_C;
						t_NewVers.Normal=prop1*pVers[B].Normal+prop2*pVers[C].Normal;

						P2=(WORD)(NumVertex+NumNewVertex);
						vec_NewVers.push_back(t_NewVers);
						t_EdgeVertex.Vertex=t_NewVers;///add edgevertex
						t_EdgeVertex.ParentFace[0]=i;
						t_EdgeVertex.ParentFace[1]=F_B;
						t_EdgeVertex.Index=P2;
						vec_EdgeVertex.push_back( t_EdgeVertex);					

						// add new vertex 
						NumNewVertex++;
					}

					//三角形ABp1 in top
					pSplitIndexTop[3*NumSplitTop]=A;
					pSplitIndexTop[3*NumSplitTop+1]=B;
					pSplitIndexTop[3*NumSplitTop+2]=P1;

					NumSplitTop++;NumFacesTop++;
					//三角形p1Bp2 in top
					pSplitIndexTop[3*NumSplitTop]=P1;
					pSplitIndexTop[3*NumSplitTop+1]=B;
					pSplitIndexTop[3*NumSplitTop+2]=P2;

					NumSplitTop++;NumFacesTop++;

					//三角形P1P2C	in bottom
					pSplitIndexBottom[3*NumSplitBottom]=P1;
					pSplitIndexBottom[3*NumSplitBottom+1]=P2;
					pSplitIndexBottom[3*NumSplitBottom+2]=C;

					NumSplitBottom++;NumFacesBottom ++;
				}
				else if (d_B<0)
				{
					if (d_C>0)
					{
						//a,c在top,B在bottom
						//计算该A-B边点并插入链表
						if (F_A<i)
						{
							//如果该临接边对应的临接面index小于i,则在该边上已经产生了边点
							for (l=0;l<vec_EdgeVertex.size();l++)
							{
								if ((vec_EdgeVertex[l].ParentFace[0]==F_A)&&(vec_EdgeVertex[l].ParentFace[1]==i))
								{
									//vec_EdgeVertex[l].ParentFace[1]==i;
									P1=vec_EdgeVertex[l].Index;
									break;
								}
							}						
						}
						else 
						{
							fEdgeVertex=pPlane->a*Pos_A.x+pPlane->b*Pos_A.y+pPlane->c*Pos_A.z+pPlane->d;
							fEdgeVertex/=pPlane->a*(Pos_A.x-Pos_B.x)+pPlane->b*(Pos_A.y-Pos_B.y)+pPlane->c*(Pos_A.z-Pos_B.z);
							t_NewVers.p.x=Pos_A.x+(Pos_B.x-Pos_A.x)*fEdgeVertex;
							t_NewVers.p.y=Pos_A.y+(Pos_B.y-Pos_A.y)*fEdgeVertex;
							t_NewVers.p.z=Pos_A.z+(Pos_B.z-Pos_A.z)*fEdgeVertex;

							t_vector = t_NewVers.p - Pos_A;
							dist1 = D3DXVec3Length(&t_vector);

							t_vector = Pos_A - Pos_B;
							dist2 = D3DXVec3Length(&t_vector);

							prop1=dist1/dist2; prop2=1-prop1;

							t_NewVers.tu1= prop1*tu_A+prop2*tu_B;
							t_NewVers.tv1= prop1*tv_A+prop2*tv_B;
							t_NewVers.Normal=prop1*pVers[A].Normal+prop2*pVers[B].Normal;						

							P1=(WORD)(NumVertex+NumNewVertex);
							vec_NewVers.push_back(t_NewVers);
							t_EdgeVertex.Vertex=t_NewVers;///add edgevertex
							t_EdgeVertex.ParentFace[0]=i;
							t_EdgeVertex.ParentFace[1]=F_A;
							t_EdgeVertex.Index=P1;
							vec_EdgeVertex.push_back( t_EdgeVertex);					

							// add new vertex and edge vertex
							NumNewVertex++;				
						}
						//计算该B-C边点并插入链表
						if (F_B<i)
						{
							//如果该临接边对应的临接面index小于i,则在该边上已经产生了边点
							for (l=0;l<vec_EdgeVertex.size();l++)
							{
								if( (vec_EdgeVertex[l].ParentFace[0]==F_B)&&(vec_EdgeVertex[l].ParentFace[1]==i))
								{
									//vec_EdgeVertex[l].ParentFace[1]==i;
									P2=vec_EdgeVertex[l].Index;
									break;
								}
							}						
						}
						else 
						{
							fEdgeVertex=pPlane->a*Pos_B.x+pPlane->b*Pos_B.y+pPlane->c*Pos_B.z+pPlane->d;
							fEdgeVertex/=pPlane->a*(Pos_B.x-Pos_C.x)+pPlane->b*(Pos_B.y-Pos_C.y)+pPlane->c*(Pos_B.z-Pos_C.z);
							t_NewVers.p.x=Pos_B.x+(Pos_C.x-Pos_B.x)*fEdgeVertex;
							t_NewVers.p.y=Pos_B.y+(Pos_C.y-Pos_B.y)*fEdgeVertex;
							t_NewVers.p.z=Pos_B.z+(Pos_C.z-Pos_B.z)*fEdgeVertex;

							t_vector = t_NewVers.p - Pos_B;
							dist1 = D3DXVec3Length(&t_vector);

							t_vector = Pos_B - Pos_C;
							dist2 = D3DXVec3Length(&t_vector);


							prop1=dist1/dist2; prop2=1-prop1;

							t_NewVers.tu1= prop1*tu_B+prop2*tu_C;
							t_NewVers.tv1= prop1*tv_B+prop2*tv_C;
							t_NewVers.Normal=prop1*pVers[B].Normal+prop2*pVers[C].Normal;

							P2=(WORD)(NumVertex+NumNewVertex);
							vec_NewVers.push_back(t_NewVers);
							t_EdgeVertex.Vertex=t_NewVers;///add edgevertex
							t_EdgeVertex.ParentFace[0]=i;
							t_EdgeVertex.ParentFace[1]=F_B;
							t_EdgeVertex.Index=P2;
							vec_EdgeVertex.push_back( t_EdgeVertex);					

							// add new vertex and edge vertex
							NumNewVertex++;
						}											
						//三角形Ap1C in top
						pSplitIndexTop[3*NumSplitTop]=A;
						pSplitIndexTop[3*NumSplitTop+1]=P1;
						pSplitIndexTop[3*NumSplitTop+2]=C;

						NumSplitTop++;NumFacesTop++;
						//三角形p1p2C in top
						pSplitIndexTop[3*NumSplitTop]=P1;
						pSplitIndexTop[3*NumSplitTop+1]=P2;
						pSplitIndexTop[3*NumSplitTop+2]=C;

						NumSplitTop++;NumFacesTop++;
						//三角形P1BP2	in bottom
						pSplitIndexBottom[3*NumSplitBottom]=P1;
						pSplitIndexBottom[3*NumSplitBottom+1]=B;
						pSplitIndexBottom[3*NumSplitBottom+2]=P2;

						NumSplitBottom++;NumFacesBottom ++;
					} 
					else if (d_C<0)
					{
						//a在top,B,c在bottom
						//计算该A-B边点并插入链表
						if (F_A<i)
						{
							//如果该临接边对应的临接面index小于i,则在该边上已经产生了边点
							for (l=0;l<vec_EdgeVertex.size();l++)
							{
								if ((vec_EdgeVertex[l].ParentFace[0]==F_A)&&(vec_EdgeVertex[l].ParentFace[1]==i))
								{
									//vec_EdgeVertex[l].ParentFace[1]==i;
									P1=vec_EdgeVertex[l].Index;
									break;
								}
							}						
						}
						else 
						{
							fEdgeVertex=pPlane->a*Pos_A.x+pPlane->b*Pos_A.y+pPlane->c*Pos_A.z+pPlane->d;
							fEdgeVertex/=pPlane->a*(Pos_A.x-Pos_B.x)+pPlane->b*(Pos_A.y-Pos_B.y)+pPlane->c*(Pos_A.z-Pos_B.z);
							t_NewVers.p.x=Pos_A.x+(Pos_B.x-Pos_A.x)*fEdgeVertex;
							t_NewVers.p.y=Pos_A.y+(Pos_B.y-Pos_A.y)*fEdgeVertex;
							t_NewVers.p.z=Pos_A.z+(Pos_B.z-Pos_A.z)*fEdgeVertex;

							t_vector = t_NewVers.p - Pos_A;
							dist1 = D3DXVec3Length(&t_vector);

							t_vector = Pos_A - Pos_B;
							dist2 = D3DXVec3Length(&t_vector);


							prop1=dist1/dist2; prop2=1-prop1;

							t_NewVers.tu1= prop1*tu_A+prop2*tu_B;
							t_NewVers.tv1= prop1*tv_A+prop2*tv_B;
							t_NewVers.Normal=prop1*pVers[A].Normal+prop2*pVers[B].Normal;						


							P1=(WORD)(NumVertex+NumNewVertex);
							vec_NewVers.push_back(t_NewVers);
							t_EdgeVertex.Vertex=t_NewVers;///add edgevertex
							t_EdgeVertex.ParentFace[0]=i;
							t_EdgeVertex.ParentFace[1]=F_A;
							t_EdgeVertex.Index=P1;
							vec_EdgeVertex.push_back( t_EdgeVertex);												
							// add new vertex and edge vertex
							NumNewVertex++;										
						}
						//计算该A-C边点并插入链表
						if (F_C<i)
						{
							//如果该临接边对应的临接面index小于i,则在该边上已经产生了边点
							for (l=0;l<vec_EdgeVertex.size();l++)
							{
								if( (vec_EdgeVertex[l].ParentFace[0]==F_C)&&(vec_EdgeVertex[l].ParentFace[1]==i))
								{
									//	vec_EdgeVertex[l].ParentFace[1]==i;
									P2=vec_EdgeVertex[l].Index;
									break;
								}
							}						
						}
						else 
						{
							fEdgeVertex=pPlane->a*Pos_A.x+pPlane->b*Pos_A.y+pPlane->c*Pos_A.z+pPlane->d;
							fEdgeVertex/=pPlane->a*(Pos_A.x-Pos_C.x)+pPlane->b*(Pos_A.y-Pos_C.y)+pPlane->c*(Pos_A.z-Pos_C.z);
							t_NewVers.p.x=Pos_A.x+(Pos_C.x-Pos_A.x)*fEdgeVertex;
							t_NewVers.p.y=Pos_A.y+(Pos_C.y-Pos_A.y)*fEdgeVertex;
							t_NewVers.p.z=Pos_A.z+(Pos_C.z-Pos_A.z)*fEdgeVertex;

							t_vector = t_NewVers.p - Pos_A;
							dist1 = D3DXVec3Length(&t_vector);

							t_vector = Pos_A - Pos_C;
							dist2 = D3DXVec3Length(&t_vector);

							prop1=dist1/dist2; prop2=1-prop1;

							t_NewVers.tu1= prop1*tu_A+prop2*tu_C;
							t_NewVers.tv1= prop1*tv_A+prop2*tv_C;
							t_NewVers.Normal=prop1*pVers[A].Normal+prop2*pVers[C].Normal;						

							P2=(WORD)(NumVertex+NumNewVertex);
							vec_NewVers.push_back(t_NewVers);
							t_EdgeVertex.Vertex=t_NewVers;///add edgevertex
							t_EdgeVertex.ParentFace[0]=i;
							t_EdgeVertex.ParentFace[1]=F_C;
							t_EdgeVertex.Index=P2;
							vec_EdgeVertex.push_back( t_EdgeVertex);					

							// add new vertex and edge vertex
							NumNewVertex++;					
						}											
						//三角形Ap1p2 in top
						pSplitIndexTop[3*NumSplitTop]=A;
						pSplitIndexTop[3*NumSplitTop+1]=P1;
						pSplitIndexTop[3*NumSplitTop+2]=P2;

						NumSplitTop++;NumFacesTop++;
						//三角形p1BC in bottom
						pSplitIndexBottom[3*NumSplitBottom]=P1;
						pSplitIndexBottom[3*NumSplitBottom+1]=B;
						pSplitIndexBottom[3*NumSplitBottom+2]=C;

						NumSplitBottom++;NumFacesBottom ++;
						//三角形P2p1C	in bottom
						pSplitIndexBottom[3*NumSplitBottom]=P2;
						pSplitIndexBottom[3*NumSplitBottom+1]=P1;
						pSplitIndexBottom[3*NumSplitBottom+2]=C;

						NumSplitBottom++;NumFacesBottom ++;
					}
				}
			}

		}
		else if (d_A<0)
		{
			if (d_B==0)
			{
				//将B记录为边点
				if (pOriginEdgeVer[B].EdgeVer==FALSE)
				{
					t_EdgeVertex.Vertex=pVers[B];	
					t_EdgeVertex.ParentFace[0]=i;
					t_EdgeVertex.Index=B;
					vec_EdgeVertex.push_back(t_EdgeVertex);
					pOriginEdgeVer[B].EdgeVer=TRUE;
					pOriginEdgeVer[B].VerIndex=vec_EdgeVertex.size()-1;
				}			
				else
				{	
					n= pOriginEdgeVer[B].VerIndex;
					vec_EdgeVertex[n].ParentFace[1]=i;
				}				


				if (d_C==0)
				{
					//将C记录为边点
					if (pOriginEdgeVer[C].EdgeVer==FALSE)
					{
						t_EdgeVertex.Vertex=pVers[C];
						t_EdgeVertex.ParentFace[0]=i;
						t_EdgeVertex.Index=C;
						vec_EdgeVertex.push_back(t_EdgeVertex);
						pOriginEdgeVer[C].EdgeVer=TRUE;
						pOriginEdgeVer[C].VerIndex=vec_EdgeVertex.size()-1;
					}			
					else
					{	
						n= pOriginEdgeVer[C].VerIndex;
						vec_EdgeVertex[n].ParentFace[1]=i;
					}					

					//该三角形处于Bottom
					bBottomFaces[i] = TRUE;
					NumFacesBottom ++;
				}
				else if (d_C>0)
				{
					//A,C两点分别位于平面的两侧,原三角面分裂
					//计算该边点并插入链表
					if (F_C<i)
					{
						//如果该临接边对应的临接面index小于i,则在该边上已经产生了边点
						for (l=0;l<vec_EdgeVertex.size();l++)
						{
							if ((vec_EdgeVertex[l].ParentFace[0]==F_C)&&(vec_EdgeVertex[l].ParentFace[1]==i))
							{
								//vec_EdgeVertex[l].ParentFace[1]==i;
								T=vec_EdgeVertex[l].Index;
								break;
							}
						}						
					}
					else 
					{
						fEdgeVertex=pPlane->a*Pos_A.x+pPlane->b*Pos_A.y+pPlane->c*Pos_A.z+pPlane->d;
						fEdgeVertex/=pPlane->a*(Pos_A.x-Pos_C.x)+pPlane->b*(Pos_A.y-Pos_C.y)+pPlane->c*(Pos_A.z-Pos_C.z);
						t_NewVers.p.x=Pos_A.x+(Pos_C.x-Pos_A.x)*fEdgeVertex;
						t_NewVers.p.y=Pos_A.y+(Pos_C.y-Pos_A.y)*fEdgeVertex;
						t_NewVers.p.z=Pos_A.z+(Pos_C.z-Pos_A.z)*fEdgeVertex;

						t_vector = t_NewVers.p - Pos_A;
						dist1 = D3DXVec3Length(&t_vector);

						t_vector = Pos_A - Pos_C;
						dist2 = D3DXVec3Length(&t_vector);


						prop1=dist1/dist2; prop2=1-prop1;

						t_NewVers.tu1= prop1*tu_A+prop2*tu_C;
						t_NewVers.tv1= prop1*tv_A+prop2*tv_C;
						t_NewVers.Normal=prop1*pVers[A].Normal+prop2*pVers[C].Normal;						
						T=(WORD)(NumVertex+NumNewVertex);//t

						vec_NewVers.push_back(t_NewVers);
						t_EdgeVertex.Vertex=t_NewVers;///add edgevertex
						t_EdgeVertex.ParentFace[0]=i;
						t_EdgeVertex.ParentFace[1]=F_C;
						t_EdgeVertex.Index=T;
						vec_EdgeVertex.push_back( t_EdgeVertex);		
						// add new vertex and edge vertex
						NumNewVertex++;
					}
					//三角形tBC	in top
					pSplitIndexTop[3*NumSplitTop]=T;//t
					pSplitIndexTop[3*NumSplitTop+1]=B;
					pSplitIndexTop[3*NumSplitTop+2]=C;

					NumSplitTop++;NumFacesTop++;					
					//三角形ABt in bottom
					pSplitIndexBottom[3*NumSplitBottom]=A;
					pSplitIndexBottom[3*NumSplitBottom+1]=B;
					pSplitIndexBottom[3*NumSplitBottom+2]=T;//t

					NumSplitBottom++;NumFacesBottom ++;					
				}
				else if (d_C<0)
				{				
					//该三角形处于bottom
					bBottomFaces[i] = TRUE;
					NumFacesBottom ++;

				}				
			}
			else if (d_C==0)
			{
				//将C记录为边点
				if (pOriginEdgeVer[C].EdgeVer==FALSE)
				{
					t_EdgeVertex.Vertex=pVers[C];	
					t_EdgeVertex.ParentFace[0]=i;
					t_EdgeVertex.Index=C;
					vec_EdgeVertex.push_back(t_EdgeVertex);
					pOriginEdgeVer[C].EdgeVer=TRUE;
					pOriginEdgeVer[C].VerIndex=vec_EdgeVertex.size()-1;
				}			
				else
				{	
					n= pOriginEdgeVer[C].VerIndex;
					vec_EdgeVertex[n].ParentFace[1]=i;
				}					

				if (d_B>0)
				{
					//A,B两点分别位于平面的两侧,原三角面分裂
					//计算该边点并插入链表
					if (F_A<i)
					{
						//如果该临接边对应的临接面index小于i,则在该边上已经产生了边点
						for (l=0;l<vec_EdgeVertex.size();l++)
						{
							if( (vec_EdgeVertex[l].ParentFace[0]==F_A)&&(vec_EdgeVertex[l].ParentFace[1]==i))
							{
								//	vec_EdgeVertex[l].ParentFace[1]==i;
								T=vec_EdgeVertex[l].Index;
								break;
							}
						}						
					}
					else 
					{
						fEdgeVertex=pPlane->a*Pos_A.x+pPlane->b*Pos_A.y+pPlane->c*Pos_A.z+pPlane->d;
						fEdgeVertex/=pPlane->a*(Pos_A.x-Pos_B.x)+pPlane->b*(Pos_A.y-Pos_B.y)+pPlane->c*(Pos_A.z-Pos_B.z);
						t_NewVers.p.x=Pos_A.x+(Pos_B.x-Pos_A.x)*fEdgeVertex;
						t_NewVers.p.y=Pos_A.y+(Pos_B.y-Pos_A.y)*fEdgeVertex;
						t_NewVers.p.z=Pos_A.z+(Pos_B.z-Pos_A.z)*fEdgeVertex;


						t_vector = t_NewVers.p - Pos_A;
						dist1 = D3DXVec3Length(&t_vector);

						t_vector = Pos_A - Pos_B;
						dist2 = D3DXVec3Length(&t_vector);


						prop1=dist1/dist2; prop2=1-prop1;

						t_NewVers.tu1= prop1*tu_A+prop2*tu_B;
						t_NewVers.tv1= prop1*tv_A+prop2*tv_B;
						t_NewVers.Normal=prop1*pVers[A].Normal+prop2*pVers[B].Normal;	
						T=(WORD)(NumVertex+NumNewVertex);

						vec_NewVers.push_back(t_NewVers);

						t_EdgeVertex.Vertex=t_NewVers;///add edgevertex
						t_EdgeVertex.ParentFace[0]=i;
						t_EdgeVertex.ParentFace[1]=F_A;
						t_EdgeVertex.Index=T;
						vec_EdgeVertex.push_back( t_EdgeVertex);					
						// add new vertex and edge vertex
						NumNewVertex++;
					}
					//三角面片分裂成两个
					//三角形tBC	in top
					pSplitIndexTop[3*NumSplitTop]=T;
					pSplitIndexTop[3*NumSplitTop+1]=B;
					pSplitIndexTop[3*NumSplitTop+2]=C;

					NumSplitTop++;NumFacesTop++;

					//三角形AtC in bottom
					pSplitIndexBottom[3*NumSplitBottom]=A;
					pSplitIndexBottom[3*NumSplitBottom+1]=T;
					pSplitIndexBottom[3*NumSplitBottom+2]=C;

					NumSplitBottom++;NumFacesBottom ++;
				}
				else if (d_B<0)
				{
					//该三角形处于bottom
					bBottomFaces[i] = TRUE;
					NumFacesBottom ++;
				}

			}
			else //d_B!=0 && d_C!=0
			{
				if (d_B<0)//d_C>0
				{
					//a,b 位于bottom,c位于top,原三角面分裂
					//计算该A-C边点并插入链表
					if (F_C<i)
					{
						//如果该临接边对应的临接面index小于i,则在该边上已经产生了边点
						for (l=0;l<vec_EdgeVertex.size();l++)
						{
							if ((vec_EdgeVertex[l].ParentFace[0]==F_C)&&(vec_EdgeVertex[l].ParentFace[1]==i))
							{
								//vec_EdgeVertex[l].ParentFace[1]==i;
								P1=vec_EdgeVertex[l].Index;
								break;
							}
						}						
					}
					else 
					{
						fEdgeVertex=pPlane->a*Pos_A.x+pPlane->b*Pos_A.y+pPlane->c*Pos_A.z+pPlane->d;
						fEdgeVertex/=pPlane->a*(Pos_A.x-Pos_C.x)+pPlane->b*(Pos_A.y-Pos_C.y)+pPlane->c*(Pos_A.z-Pos_C.z);
						t_NewVers.p.x=Pos_A.x+(Pos_C.x-Pos_A.x)*fEdgeVertex;
						t_NewVers.p.y=Pos_A.y+(Pos_C.y-Pos_A.y)*fEdgeVertex;
						t_NewVers.p.z=Pos_A.z+(Pos_C.z-Pos_A.z)*fEdgeVertex;					

						t_vector = t_NewVers.p - Pos_A;
						dist1 = D3DXVec3Length(&t_vector);

						t_vector = Pos_A - Pos_C;
						dist2 = D3DXVec3Length(&t_vector);

						prop1=dist1/dist2; prop2=1-prop1;

						t_NewVers.tu1= prop1*tu_A+prop2*tu_C;
						t_NewVers.tv1= prop1*tv_A+prop2*tv_C;
						t_NewVers.Normal=prop1*pVers[A].Normal+prop2*pVers[C].Normal;	

						P1=(WORD)(NumVertex+NumNewVertex);
						vec_NewVers.push_back(t_NewVers);

						t_EdgeVertex.Vertex=t_NewVers;///add edgevertex
						t_EdgeVertex.ParentFace[0]=i;
						t_EdgeVertex.ParentFace[1]=F_C;
						t_EdgeVertex.Index=P1;
						vec_EdgeVertex.push_back( t_EdgeVertex);					


						// add new vertex and edge vertex
						NumNewVertex++;
					}

					//计算该B-C边点并插入链表
					if (F_B<i)
					{
						//如果该临接边对应的临接面index小于i,则在该边上已经产生了边点
						for (l=0;l<vec_EdgeVertex.size();l++)
						{
							if( (vec_EdgeVertex[l].ParentFace[0]==F_B)&&(vec_EdgeVertex[l].ParentFace[1]==i))
							{
								//	vec_EdgeVertex[l].ParentFace[1]==i;
								P2=vec_EdgeVertex[l].Index;
								break;
							}
						}						
					}
					else 
					{
						fEdgeVertex=pPlane->a*Pos_B.x+pPlane->b*Pos_B.y+pPlane->c*Pos_B.z+pPlane->d;
						fEdgeVertex/=pPlane->a*(Pos_B.x-Pos_C.x)+pPlane->b*(Pos_B.y-Pos_C.y)+pPlane->c*(Pos_B.z-Pos_C.z);
						t_NewVers.p.x=Pos_B.x+(Pos_C.x-Pos_B.x)*fEdgeVertex;
						t_NewVers.p.y=Pos_B.y+(Pos_C.y-Pos_B.y)*fEdgeVertex;
						t_NewVers.p.z=Pos_B.z+(Pos_C.z-Pos_B.z)*fEdgeVertex;

						t_vector = t_NewVers.p - Pos_B;
						dist1 = D3DXVec3Length(&t_vector);

						t_vector = Pos_B - Pos_C;
						dist2 = D3DXVec3Length(&t_vector);

						prop1=dist1/dist2; prop2=1-prop1;

						t_NewVers.tu1= prop1*tu_B+prop2*tu_C;
						t_NewVers.tv1= prop1*tv_B+prop2*tv_C;
						t_NewVers.Normal=prop1*pVers[B].Normal+prop2*pVers[C].Normal;	

						P2=(WORD)(NumVertex+NumNewVertex);
						vec_NewVers.push_back(t_NewVers);
						t_EdgeVertex.Vertex=t_NewVers;///add edgevertex
						t_EdgeVertex.ParentFace[0]=i;
						t_EdgeVertex.ParentFace[1]=F_B;
						t_EdgeVertex.Index=P2;
						vec_EdgeVertex.push_back( t_EdgeVertex);				

						// add new vertex and edge vertex
						NumNewVertex++;					
					}
					//三角形P1P2C	in top
					pSplitIndexTop[3*NumSplitTop]=P1;
					pSplitIndexTop[3*NumSplitTop+1]=P2;
					pSplitIndexTop[3*NumSplitTop+2]=C;

					NumSplitTop++;NumFacesTop++;
					//三角形ABp1 in bottom
					pSplitIndexBottom[3*NumSplitBottom]=A;
					pSplitIndexBottom[3*NumSplitBottom+1]=B;
					pSplitIndexBottom[3*NumSplitBottom+2]=P1;

					NumSplitBottom++;NumFacesBottom ++;
					//三角形p1Bp2 in bottom
					pSplitIndexBottom[3*NumSplitBottom]=P1;
					pSplitIndexBottom[3*NumSplitBottom+1]=B;
					pSplitIndexBottom[3*NumSplitBottom+2]=P2;

					NumSplitBottom++;NumFacesBottom ++;			
				}
				else if (d_B>0)
				{
					if (d_C>0)
					{
						//B,c在top,a在bottom
						//计算该A-B边点p1并插入链表
						if (F_A<i)
						{
							//如果该临接边对应的临接面index小于i,则在该边上已经产生了边点
							for (l=0;l<vec_EdgeVertex.size();l++)
							{
								if( (vec_EdgeVertex[l].ParentFace[0]==F_A)&&(vec_EdgeVertex[l].ParentFace[1]==i))
								{
									//	vec_EdgeVertex[l].ParentFace[1]==i;
									P1=vec_EdgeVertex[l].Index;
									break;
								}
							}						
						}
						else 
						{
							fEdgeVertex=pPlane->a*Pos_A.x+pPlane->b*Pos_A.y+pPlane->c*Pos_A.z+pPlane->d;
							fEdgeVertex/=pPlane->a*(Pos_A.x-Pos_B.x)+pPlane->b*(Pos_A.y-Pos_B.y)+pPlane->c*(Pos_A.z-Pos_B.z);
							t_NewVers.p.x=Pos_A.x+(Pos_B.x-Pos_A.x)*fEdgeVertex;
							t_NewVers.p.y=Pos_A.y+(Pos_B.y-Pos_A.y)*fEdgeVertex;
							t_NewVers.p.z=Pos_A.z+(Pos_B.z-Pos_A.z)*fEdgeVertex;

							t_vector = t_NewVers.p - Pos_A;
							dist1 = D3DXVec3Length(&t_vector);

							t_vector = Pos_B - Pos_A;
							dist2 = D3DXVec3Length(&t_vector);


							prop1=dist1/dist2; prop2=1-prop1;

							t_NewVers.tu1= prop1*tu_A+prop2*tu_B;
							t_NewVers.tv1= prop1*tv_A+prop2*tv_B;
							t_NewVers.Normal=prop1*pVers[A].Normal+prop2*pVers[B].Normal;							

							P1=(WORD)(NumVertex+NumNewVertex);
							vec_NewVers.push_back(t_NewVers);
							t_EdgeVertex.Vertex=t_NewVers;///add edgevertex
							t_EdgeVertex.ParentFace[0]=i;
							t_EdgeVertex.ParentFace[1]=F_A;
							t_EdgeVertex.Index=P1;
							vec_EdgeVertex.push_back( t_EdgeVertex);					

							// add new vertex and edge vertex
							NumNewVertex++;

						}
						//计算该A-C边点 p2并插入链表
						if (F_C<i)
						{
							//如果该临接边对应的临接面index小于i,则在该边上已经产生了边点
							for (l=0;l<vec_EdgeVertex.size();l++)
							{
								if ((vec_EdgeVertex[l].ParentFace[0]==F_B)&&(vec_EdgeVertex[l].ParentFace[1]==i))
								{
									//vec_EdgeVertex[l].ParentFace[1]==i;
									P2=vec_EdgeVertex[l].Index;
									break;
								}
							}						
						}
						else 
						{
							fEdgeVertex=pPlane->a*Pos_A.x+pPlane->b*Pos_A.y+pPlane->c*Pos_A.z+pPlane->d;
							fEdgeVertex/=pPlane->a*(Pos_A.x-Pos_C.x)+pPlane->b*(Pos_A.y-Pos_C.y)+pPlane->c*(Pos_A.z-Pos_C.z);
							t_NewVers.p.x=Pos_A.x+(Pos_C.x-Pos_A.x)*fEdgeVertex;
							t_NewVers.p.y=Pos_A.y+(Pos_C.y-Pos_A.y)*fEdgeVertex;
							t_NewVers.p.z=Pos_A.z+(Pos_C.z-Pos_A.z)*fEdgeVertex;

							t_vector = t_NewVers.p - Pos_A;
							dist1 = D3DXVec3Length(&t_vector);

							t_vector = Pos_A - Pos_C;
							dist2 = D3DXVec3Length(&t_vector);


							prop1=dist1/dist2; prop2=1-prop1;

							t_NewVers.tu1= prop1*tu_A+prop2*tu_C;
							t_NewVers.tv1= prop1*tv_A+prop2*tv_C;
							t_NewVers.Normal=prop1*pVers[A].Normal+prop2*pVers[C].Normal;	

							P2=(WORD)(NumVertex+NumNewVertex);
							vec_NewVers.push_back(t_NewVers);
							t_EdgeVertex.Vertex=t_NewVers;///add edgevertex
							t_EdgeVertex.ParentFace[0]=i;
							t_EdgeVertex.ParentFace[1]=F_C;
							t_EdgeVertex.Index=P2;
							vec_EdgeVertex.push_back( t_EdgeVertex);					

							// add new vertex and edge vertex
							NumNewVertex++;								
						}
						//三角形p2BC in top
						pSplitIndexTop[3*NumSplitTop]=P2;
						pSplitIndexTop[3*NumSplitTop+1]=B;
						pSplitIndexTop[3*NumSplitTop+2]=C;

						NumSplitTop++;NumFacesTop++;
						//三角形p2p1B in top
						pSplitIndexTop[3*NumSplitTop]=P2;
						pSplitIndexTop[3*NumSplitTop+1]=P1;
						pSplitIndexTop[3*NumSplitTop+2]=B;

						NumSplitTop++;NumFacesTop++;
						//三角形AP1P2	in bottom
						pSplitIndexBottom[3*NumSplitBottom]=A;
						pSplitIndexBottom[3*NumSplitBottom+1]=P1;
						pSplitIndexBottom[3*NumSplitBottom+2]=P2;

						NumSplitBottom++;NumFacesBottom ++;
					}
					else if (d_C<0)
					{
						//a,c在top,B在bottom
						//计算该A-B边点并插入链表
						if (F_A<i)
						{
							//如果该临接边对应的临接面index小于i,则在该边上已经产生了边点
							for (l=0;l<vec_EdgeVertex.size();l++)
							{
								if ((vec_EdgeVertex[l].ParentFace[0]==F_A)&&(vec_EdgeVertex[l].ParentFace[1]==i))
								{
									//vec_EdgeVertex[l].ParentFace[1]==i;
									P1=vec_EdgeVertex[l].Index;
									break;
								}
							}						
						}
						else 
						{
							fEdgeVertex=pPlane->a*Pos_A.x+pPlane->b*Pos_A.y+pPlane->c*Pos_A.z+pPlane->d;
							fEdgeVertex/=pPlane->a*(Pos_A.x-Pos_B.x)+pPlane->b*(Pos_A.y-Pos_B.y)+pPlane->c*(Pos_A.z-Pos_B.z);
							t_NewVers.p.x=Pos_A.x+(Pos_B.x-Pos_A.x)*fEdgeVertex;
							t_NewVers.p.y=Pos_A.y+(Pos_B.y-Pos_A.y)*fEdgeVertex;
							t_NewVers.p.z=Pos_A.z+(Pos_B.z-Pos_A.z)*fEdgeVertex;

							t_vector = t_NewVers.p - Pos_A;
							dist1 = D3DXVec3Length(&t_vector);

							t_vector = Pos_A - Pos_B;
							dist2 = D3DXVec3Length(&t_vector);

							prop1=dist1/dist2; prop2=1-prop1;

							t_NewVers.tu1= prop1*tu_A+prop2*tu_B;
							t_NewVers.tv1= prop1*tv_A+prop2*tv_B;
							t_NewVers.Normal=prop1*pVers[A].Normal+prop2*pVers[B].Normal;		

							P1=(WORD)(NumVertex+NumNewVertex);
							vec_NewVers.push_back(t_NewVers);
							t_EdgeVertex.Vertex=t_NewVers;///add edgevertex
							t_EdgeVertex.ParentFace[0]=i;
							t_EdgeVertex.ParentFace[1]=F_A;
							t_EdgeVertex.Index=P1;
							vec_EdgeVertex.push_back( t_EdgeVertex);					

							// add new vertex and edge vertex
							NumNewVertex++;
						}
						//计算该B-C边点并插入链表
						if (F_B<i)
						{
							//如果该临接边对应的临接面index小于i,则在该边上已经产生了边点
							for (l=0;l<vec_EdgeVertex.size();l++)
							{
								if( (vec_EdgeVertex[l].ParentFace[0]==F_B)&&(vec_EdgeVertex[l].ParentFace[1]==i))

								{
									//	vec_EdgeVertex[l].ParentFace[1]==i;
									P2=vec_EdgeVertex[l].Index;
									break;
								}
							}						
						}
						else 
						{
							fEdgeVertex=pPlane->a*Pos_B.x+pPlane->b*Pos_B.y+pPlane->c*Pos_B.z+pPlane->d;
							fEdgeVertex/=pPlane->a*(Pos_B.x-Pos_C.x)+pPlane->b*(Pos_B.y-Pos_C.y)+pPlane->c*(Pos_B.z-Pos_C.z);
							t_NewVers.p.x=Pos_B.x+(Pos_C.x-Pos_B.x)*fEdgeVertex;
							t_NewVers.p.y=Pos_B.y+(Pos_C.y-Pos_B.y)*fEdgeVertex;
							t_NewVers.p.z=Pos_B.z+(Pos_C.z-Pos_B.z)*fEdgeVertex;

							t_vector = t_NewVers.p - Pos_B;
							dist1 = D3DXVec3Length(&t_vector);

							t_vector = Pos_B - Pos_C;
							dist2 = D3DXVec3Length(&t_vector);


							prop1=dist1/dist2; prop2=1-prop1;

							t_NewVers.tu1= prop1*tu_B+prop2*tu_C;
							t_NewVers.tv1= prop1*tv_B+prop2*tv_C;
							t_NewVers.Normal=prop1*pVers[B].Normal+prop2*pVers[C].Normal;	

							P2=(WORD)(NumVertex+NumNewVertex);
							vec_NewVers.push_back(t_NewVers);
							t_EdgeVertex.Vertex=t_NewVers;///add edgevertex
							t_EdgeVertex.ParentFace[0]=i;
							t_EdgeVertex.ParentFace[1]=F_B;
							t_EdgeVertex.Index=P2;
							vec_EdgeVertex.push_back( t_EdgeVertex);					

							// add new vertex and edge vertex
							NumNewVertex++;
						}

						//三角形P1BP2	in top
						pSplitIndexTop[3*NumSplitTop]=P1;
						pSplitIndexTop[3*NumSplitTop+1]=B;
						pSplitIndexTop[3*NumSplitTop+2]=P2;

						NumSplitTop++;NumFacesTop++;
						//三角形Ap1C in bottom
						pSplitIndexBottom[3*NumSplitBottom]=A;
						pSplitIndexBottom[3*NumSplitBottom+1]=P1;
						pSplitIndexBottom[3*NumSplitBottom+2]=C;

						NumSplitBottom++;NumFacesBottom ++;
						//三角形p1p2C in bottom
						pSplitIndexBottom[3*NumSplitBottom]=P1;
						pSplitIndexBottom[3*NumSplitBottom+1]=P2;
						pSplitIndexBottom[3*NumSplitBottom+2]=C;

						NumSplitBottom++;NumFacesBottom ++;			

					}
				}
			}
		}

	}

	///---------------------------三角刨分-----------------------------------------------------
	///对edgevertex区分成不同的环，同时进行排序
	///对已序点进行三角剖分
	WORD * pEdgeIndex = NULL;
	pEdgeIndex = new WORD [4000];//保存由边点三角剖分后生成的三角面
	if ( !pEdgeIndex )
		goto Exit0;

	int NumEdgeFace=0;

	int nTriangulationResult = false;
	if ( vec_EdgeVertex.size() > 3 )
	{
		nTriangulationResult = TripTriangulation( vec_EdgeVertex, vec_EdgeVertex.size(),&pEdgeIndex, &NumEdgeFace );
		if ( !nTriangulationResult )
			goto Exit0;

	}

	//--------------------------------------------------------------------------------------------------------------
	//创建正法向量的网格
	if (FAILED(D3DXCreateMeshFVF(NumFacesTop+NumEdgeFace,NumNewVertex+NumVertex,D3DXMESH_MANAGED,
		(D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1),g_pd3dDevice, &((*mod_Top)->m_pMeshSys))))
	{
		goto Exit0;
	}


	VFormat::FACES_NORMAL_TEXTURE1 * pTopVers = NULL;
	WORD* pTopIndex = NULL;
	DWORD * pTopAttrib = NULL;	

	if (FAILED((*mod_Top)->m_pMeshSys->LockVertexBuffer(0,(void**)&pTopVers)))
		goto Exit0;
	if (FAILED((*mod_Top)->m_pMeshSys->LockIndexBuffer (0,(void**)&pTopIndex)))
		goto Exit0;
	if (FAILED((*mod_Top)->m_pMeshSys->LockAttributeBuffer(0,(DWORD**)&pTopAttrib)))
		goto Exit0;

	{
		//建立新顶点
		for(DWORD i=0;i<(NumVertex);i++)
		{
			pTopVers[i] = pVers[i];
		}

		for(DWORD j=0;i<(NumVertex+NumNewVertex);i++,j++)
		{
			pTopVers[i] = vec_NewVers[j];//将新增加的点放入原先vers的末尾
		}


		DWORD K = 0;
		for( i=0;i<NumFaces;i++)
		{
			WORD A,B,C;
			DWORD SubsetID;
			A = pIndex[i*3  ];
			B = pIndex[i*3+1];
			C = pIndex[i*3+2];
			SubsetID = pAttrib[i];

			if(bTopFaces[i]==TRUE)
			{
				pTopIndex[K*3  ] = A;
				pTopIndex[K*3+1] = B;
				pTopIndex[K*3+2] = C;

				pTopAttrib[K] = SubsetID;

				K ++;
			}
		}


		//将新增加的top面，放入末尾
		for( j=0;K<NumFacesTop;K++,j++)
		{
			WORD A,B,C;
			DWORD SubsetID;
			A = pSplitIndexTop[j*3  ];
			B = pSplitIndexTop[j*3+1];
			C = pSplitIndexTop[j*3+2];
			//SubsetID = pAttrib[i];
			SubsetID = 0;

			pTopIndex[K*3  ] = A;
			pTopIndex[K*3+1] = B;
			pTopIndex[K*3+2] = C;

			pTopAttrib[K] = SubsetID;

		}

		///将新增加的edgeFace，放入末尾	
		for (j=0;j<(DWORD)NumEdgeFace;j++,K++)
		{
			WORD A,B,C;
			DWORD SubsetID;
			A = pEdgeIndex[j*3  ];
			B = pEdgeIndex[j*3+1];
			C = pEdgeIndex[j*3+2];
			//SubsetID = pAttrib[i];
			SubsetID = 0;

			pTopIndex[K*3  ] = A;
			pTopIndex[K*3+1] = B;
			pTopIndex[K*3+2] = C;

			pTopAttrib[K] = SubsetID;
		}


		D3DXComputeBoundingBox((D3DXVECTOR3*) pTopVers,NumNewVertex+NumVertex,sizeof(VFormat::FACES_NORMAL_TEXTURE1),
			&((*mod_Top)->m_BBox_A),&((*mod_Top)->m_BBox_B));

		if (FAILED((*mod_Top)->m_pMeshSys->UnlockVertexBuffer()))
			goto Exit0;
		if (FAILED((*mod_Top)->m_pMeshSys->UnlockIndexBuffer()))
			goto Exit0;
		if (FAILED((*mod_Top)->m_pMeshSys->UnlockAttributeBuffer()))
			goto Exit0;

		//vec_NewVers.release();
	}

	//创建负法向量的网格
	if (FAILED(D3DXCreateMeshFVF(NumFacesBottom+NumEdgeFace,NumNewVertex+NumVertex,D3DXMESH_MANAGED,
		(D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1),g_pd3dDevice, &((*mod_Bottom)->m_pMeshSys))))
	{
		goto Exit0;
	}

	VFormat::FACES_NORMAL_TEXTURE1 * pBottomVers = NULL;
	WORD* pBottomIndex = NULL;
	DWORD * pBottomAttrib = NULL;	

	if (FAILED((*mod_Bottom)->m_pMeshSys->LockVertexBuffer(0,(void**)&pBottomVers)))
		goto Exit0;
	if (FAILED((*mod_Bottom)->m_pMeshSys->LockIndexBuffer (0,(void**)&pBottomIndex)))
		goto Exit0;
	if (FAILED((*mod_Bottom)->m_pMeshSys->LockAttributeBuffer(0,(DWORD**)&pBottomAttrib)))
		goto Exit0;

	{

		for(DWORD i=0;i<(NumVertex);i++)
		{
			pBottomVers[i] = pVers[i];
		}

		for(DWORD j=0;i<(NumVertex+NumNewVertex);i++,j++)
		{
			pBottomVers[i] = vec_NewVers[j];//将新增加的点放入原先vers的末尾
		}

		DWORD K = 0;
		for( i=0;i<NumFaces;i++)
		{
			WORD A,B,C;
			DWORD SubsetID;
			A = pIndex[i*3  ];
			B = pIndex[i*3+1];
			C = pIndex[i*3+2];
			SubsetID = pAttrib[i];


			if(bBottomFaces[i])
			{
				pBottomIndex[K*3  ] = A;
				pBottomIndex[K*3+1] = B;
				pBottomIndex[K*3+2] = C;

				pBottomAttrib[K] = SubsetID;

				K ++;
			}
		}
		//将新增加的bottom面，放入末尾
		for( j=0;K<NumFacesBottom;K++,j++)
		{
			WORD A,B,C;
			DWORD SubsetID;
			A = pSplitIndexBottom[j*3  ];
			B = pSplitIndexBottom[j*3+1];
			C = pSplitIndexBottom[j*3+2];
			//			SubsetID = pAttrib[i];
			SubsetID = 0;

			pBottomIndex[K*3  ] = A;
			pBottomIndex[K*3+1] = B;
			pBottomIndex[K*3+2] = C;

			pBottomAttrib[K] = SubsetID;

		}

		///将新增加的edgeFace，放入末尾	
		for (j=0;j<(DWORD)NumEdgeFace;j++,K++)
		{
			WORD A,B,C;
			DWORD SubsetID;
			A = pEdgeIndex[j*3  ];
			B = pEdgeIndex[j*3+1];
			C = pEdgeIndex[j*3+2];
			//SubsetID = pAttrib[i];
			SubsetID = 0;

			pBottomIndex[K*3  ] = A;
			pBottomIndex[K*3+1] = B;
			pBottomIndex[K*3+2] = C;

			pBottomAttrib[K] = SubsetID;
		}

		D3DXComputeBoundingBox((D3DXVECTOR3*) pBottomVers,NumNewVertex+NumVertex,sizeof(VFormat::FACES_NORMAL_TEXTURE1),
			&((*mod_Bottom)->m_BBox_A),&((*mod_Bottom)->m_BBox_B));

		if (FAILED((*mod_Bottom)->m_pMeshSys->UnlockVertexBuffer()))
			goto Exit0;
		if (FAILED((*mod_Bottom)->m_pMeshSys->UnlockIndexBuffer()))
			goto Exit0;
		if (FAILED((*mod_Bottom)->m_pMeshSys->UnlockAttributeBuffer()))
			goto Exit0;
	}

	///释放vector


	//---------------------------------------------------------------------------------------------------------------

	if (FAILED(m_pMeshSys->UnlockVertexBuffer()))
		goto Exit0;
	if (FAILED(m_pMeshSys->UnlockIndexBuffer()))
		goto Exit0;
	if (FAILED(m_pMeshSys->UnlockAttributeBuffer()))
		goto Exit0;

	//为产生的模型载入材质
	(*mod_Top)->m_dNumMaterial = m_dNumMaterial;
	(*mod_Top)->m_lpMaterial = new MATERIAL[m_dNumMaterial];
	ZeroMemory((*mod_Top)->m_lpMaterial,sizeof(MATERIAL)*m_dNumMaterial);
	(*mod_Top)->LoadMaterial(m_szMtlFileName);

	//为产生的模型载入材质
	(*mod_Bottom)->m_dNumMaterial = m_dNumMaterial;
	(*mod_Bottom)->m_lpMaterial = new MATERIAL[m_dNumMaterial];
	ZeroMemory((*mod_Bottom)->m_lpMaterial,sizeof(MATERIAL)*m_dNumMaterial);
	(*mod_Bottom)->LoadMaterial(m_szMtlFileName);

	//优化显示
	//pNewModel->OptimizeUnSkinedMesh();
	//SAFE_DELETE_ARRAY(m_edgeVertex);
	//SAFE_DELETE_ARRAY(pNewVers);
	nResult = true;
Exit0:	 
	vec_EdgeVertex.clear();

	vec_NewVers.clear();

	if ( bTopFaces )
	{
		delete []bTopFaces;
		bTopFaces = NULL;
	}

	if ( bBottomFaces )
	{
		delete []bBottomFaces;
		bBottomFaces = NULL;
	}

	if ( pEdgeIndex )
	{
		delete []pEdgeIndex;
		pEdgeIndex = NULL;
	}
	if ( pSplitIndexTop )
	{
		delete []pSplitIndexTop;
		pSplitIndexTop = NULL;
	}
	if ( pSplitIndexBottom )
	{
		delete []pSplitIndexBottom;
		pSplitIndexBottom = NULL;
	}
	if ( pOriginEdgeVer )
	{
		delete []pOriginEdgeVer;
		pOriginEdgeVer = NULL;
	}
	/*
	if ( t_NewVers )
	{
	delete t_NewVers;
	t_NewVers = NULL;
	}
	*/

	return nResult;	
Exit1:
	return nResult;

}

HRESULT KModel::MTLMSEFFECT::SetParameter(MATERIAL* pMaterial,KModel* pModel)
{
	HRESULT hr = S_OK;

	if(!m_HEffect->pEffect)
		return E_FAIL;

	if(m_nCurTechnique == -1)
	{
		m_HEffect->pEffect->ValidateTechnique(NULL);
		//m_nCurTechnique = 0;
	}
	else
	{
		D3DXHANDLE pTec = m_HEffect->pEffect->GetTechnique(m_nCurTechnique);
		if(pTec)
		{
			m_HEffect->pEffect->SetTechnique(pTec);
		}
	}

	for(UINT K = 0;K<InputConstant.size();K++)
	{
		SHADERCONSTANT* pConst = &InputConstant[K];
		D3DXMATRIX World,View,Project;

		g_pd3dDevice->GetTransform( D3DTS_WORLD, &World );
		g_pd3dDevice->GetTransform( D3DTS_VIEW,  &View );
		g_pd3dDevice->GetTransform( D3DTS_PROJECTION, &Project );

		hr = m_HEffect->pEffect->SetMatrix("World",&World);
		hr = m_HEffect->pEffect->SetMatrix("View" ,&View);
		hr = m_HEffect->pEffect->SetMatrix("Projection",&Project);

		switch(pConst->dwType)
		{
		case MSEFFECT_WORLD:
			{
				D3DXMATRIX World;
				g_pd3dDevice->GetTransform( D3DTS_WORLD, &World );
				//m_HEffect->pEffect->SetMatrix(pConst->scName.c_str(),&World);
				m_HEffect->pEffect->SetValue(pConst->m_hParameter,&World,sizeof(D3DXMATRIX));
				break;
			}
		case MSEFFECT_WORLDI:
			{
				D3DXMATRIX World;
				g_pd3dDevice->GetTransform( D3DTS_WORLD, &World );
				D3DXMatrixInverse(&World,NULL,&World);
				//m_HEffect->pEffect->SetMatrix(pConst->scName.c_str(),&World);
				m_HEffect->pEffect->SetValue(pConst->m_hParameter,&World,sizeof(D3DXMATRIX));
				break;
			}
		case MSEFFECT_WORLDIT:
			{
				D3DXMATRIX World;
				g_pd3dDevice->GetTransform( D3DTS_WORLD, &World );
				D3DXMatrixInverse(&World,NULL,&World);
				D3DXMatrixTranspose(&World,&World);
				//m_HEffect->pEffect->SetMatrix(pConst->scName.c_str(),&World);
				m_HEffect->pEffect->SetValue(pConst->m_hParameter,&World,sizeof(D3DXMATRIX));
				break;
			}

		case MSEFFECT_VIEW:
			{
				D3DXMATRIX World;
				g_pd3dDevice->GetTransform( D3DTS_VIEW, &World );
				//m_HEffect->pEffect->SetMatrix(pConst->scName.c_str(),&World);
				m_HEffect->pEffect->SetValue(pConst->m_hParameter,&World,sizeof(D3DXMATRIX));
				break;
			}
		case MSEFFECT_VIEWI:
			{
				D3DXMATRIX World;
				g_pd3dDevice->GetTransform( D3DTS_VIEW, &World );
				D3DXMatrixInverse(&World,NULL,&World);
				//m_HEffect->pEffect->SetMatrix(pConst->scName.c_str(),&World);
				m_HEffect->pEffect->SetValue(pConst->m_hParameter,&World,sizeof(D3DXMATRIX));
				break;
			}
		case MSEFFECT_VIEWIT:
			{
				D3DXMATRIX World;
				g_pd3dDevice->GetTransform( D3DTS_VIEW, &World );
				D3DXMatrixInverse(&World,NULL,&World);
				D3DXMatrixTranspose(&World,&World);
				//m_HEffect->pEffect->SetMatrix(pConst->scName.c_str(),&World);
				m_HEffect->pEffect->SetValue(pConst->m_hParameter,&World,sizeof(D3DXMATRIX));
				break;
			}

		case MSEFFECT_PROJECTION:
			{
				D3DXMATRIX World;
				g_pd3dDevice->GetTransform( D3DTS_PROJECTION, &World );
				//m_HEffect->pEffect->SetMatrix(pConst->scName.c_str(),&World);
				m_HEffect->pEffect->SetValue(pConst->m_hParameter,&World,sizeof(D3DXMATRIX));
				break;
			}
		case MSEFFECT_PROJECTIONI:
			{
				D3DXMATRIX World;
				g_pd3dDevice->GetTransform( D3DTS_PROJECTION, &World );
				D3DXMatrixInverse(&World,NULL,&World);
				//m_HEffect->pEffect->SetMatrix(pConst->scName.c_str(),&World);
				m_HEffect->pEffect->SetValue(pConst->m_hParameter,&World,sizeof(D3DXMATRIX));
				break;
			}
		case MSEFFECT_PROJECTIONIT:
			{
				D3DXMATRIX World;
				g_pd3dDevice->GetTransform( D3DTS_PROJECTION, &World );
				D3DXMatrixInverse(&World,NULL,&World);
				D3DXMatrixTranspose(&World,&World);
				//m_HEffect->pEffect->SetMatrix(pConst->scName.c_str(),&World);
				m_HEffect->pEffect->SetValue(pConst->m_hParameter,&World,sizeof(D3DXMATRIX));
				break;
			}

		case MSEFFECT_WORLDVIEW:
			{
				D3DXMATRIX World,View;
				g_pd3dDevice->GetTransform( D3DTS_WORLD, &World );
				g_pd3dDevice->GetTransform( D3DTS_VIEW , &View );
				D3DXMatrixMultiply(&World,&World,&View);
				//m_HEffect->pEffect->SetMatrix(pConst->scName.c_str(),&World);
				m_HEffect->pEffect->SetValue(pConst->m_hParameter,&World,sizeof(D3DXMATRIX));
				break;
			}
		case MSEFFECT_WORLDVIEWI:
			{
				D3DXMATRIX World,View;
				g_pd3dDevice->GetTransform( D3DTS_WORLD, &World );
				g_pd3dDevice->GetTransform( D3DTS_VIEW , &View );
				D3DXMatrixMultiply(&World,&World,&View);
				D3DXMatrixInverse(&World,NULL,&World);
				//m_HEffect->pEffect->SetMatrix(pConst->scName.c_str(),&World);
				m_HEffect->pEffect->SetValue(pConst->m_hParameter,&World,sizeof(D3DXMATRIX));
				break;
			}
		case MSEFFECT_WORLDVIEWIT:
			{
				D3DXMATRIX World,View;
				g_pd3dDevice->GetTransform( D3DTS_WORLD, &World );
				g_pd3dDevice->GetTransform( D3DTS_VIEW , &View );
				D3DXMatrixMultiply(&World,&World,&View);
				D3DXMatrixInverse(&World,NULL,&World);
				D3DXMatrixTranspose(&World,&World);
				//m_HEffect->pEffect->SetMatrix(pConst->scName.c_str(),&World);
				m_HEffect->pEffect->SetValue(pConst->m_hParameter,&World,sizeof(D3DXMATRIX));
				break;
			}		

		case MSEFFECT_WORLDVIEWPROJ:
			{
				D3DXMATRIX World,View,Proj;
				g_pd3dDevice->GetTransform( D3DTS_WORLD, &World );
				g_pd3dDevice->GetTransform( D3DTS_VIEW , &View );
				g_pd3dDevice->GetTransform( D3DTS_PROJECTION , &Proj );
				D3DXMatrixMultiply(&World,&World,&View);
				D3DXMatrixMultiply(&World,&World,&Proj);
				//m_HEffect->pEffect->SetMatrix(pConst->scName.c_str(),&World);
				m_HEffect->pEffect->SetValue(pConst->m_hParameter,&World,sizeof(D3DXMATRIX));
				break;
			}
		case MSEFFECT_WORLDVIEWPROJI:
			{
				D3DXMATRIX World,View,Proj;
				g_pd3dDevice->GetTransform( D3DTS_WORLD, &World );
				g_pd3dDevice->GetTransform( D3DTS_VIEW , &View );
				g_pd3dDevice->GetTransform( D3DTS_PROJECTION , &Proj );
				D3DXMatrixMultiply(&World,&World,&View);
				D3DXMatrixMultiply(&World,&World,&Proj);
				D3DXMatrixInverse(&World,NULL,&World);
				//m_HEffect->pEffect->SetMatrix(pConst->scName.c_str(),&World);
				m_HEffect->pEffect->SetValue(pConst->m_hParameter,&World,sizeof(D3DXMATRIX));
				break;
			}
		case MSEFFECT_WORLDVIEWPROJIT:
			{
				D3DXMATRIX World,View,Proj;
				g_pd3dDevice->GetTransform( D3DTS_WORLD, &World );
				g_pd3dDevice->GetTransform( D3DTS_VIEW , &View );
				g_pd3dDevice->GetTransform( D3DTS_PROJECTION , &Proj );
				D3DXMatrixMultiply(&World,&World,&View);
				D3DXMatrixMultiply(&World,&World,&Proj);
				D3DXMatrixInverse(&World,NULL,&World);
				D3DXMatrixTranspose(&World,&World);
				//m_HEffect->pEffect->SetMatrix(pConst->scName.c_str(),&World);
				m_HEffect->pEffect->SetValue(pConst->m_hParameter,&World,sizeof(D3DXMATRIX));
				break;
			}
		case MSEFFECT_SUNDIRMATRIX:
			{
				K3DScene* pScene = g_cGraphicsTool.GetCurScene();
				if(pScene)
				{
					D3DXMATRIX LightTrans;
					//g_pd3dDevice->GetTransform( D3DTS_VIEW , &LightTrans );

					//pScene->m_SunLight.Direction;
					D3DXMatrixLookAtLH(&LightTrans,&D3DXVECTOR3(0,0,0),
						(D3DXVECTOR3*)&pScene->m_SunLight.Direction,
						&D3DXVECTOR3(0,0,0.001f));
					//m_HEffect->pEffect->SetMatrix(pConst->scName.c_str(),&LightTrans);
					m_HEffect->pEffect->SetValue(pConst->m_hParameter,&LightTrans,sizeof(D3DXMATRIX));
				}
				break;
			}
			//////////////////////////////////////////////////////////////////////////
			//values 	
		case MSEFFECT_COLORRGBA:
			{
				D3DXVECTOR4 * pData = (D3DXVECTOR4*) pConst->pData;
				hr = m_HEffect->pEffect->SetVector(pConst->m_hParameter,pData);
				break;
			}
			//////////////////////////////////////////////////////////////////////////
			//values 
		case MSEFFECT_FLOAT:
			{
				float* pData = (float*) pConst->pData;
				hr = m_HEffect->pEffect->SetValue(pConst->m_hParameter,pData,4);

				break;
			}
		case MSEFFECT_INT:
			{
				int* pData = (int*) pConst->pData;
				hr = m_HEffect->pEffect->SetValue(pConst->m_hParameter,pData,4);
				break;
			}
			//////////////////////////////////////////////////////////////////////////
			//Materials	
		case MSEFFECT_TEXTURE:
		case MSEFFECT_TEXTURECUBE:
			{
				LPTEXTURE pTexture = NULL;
				LPTEXTUREDATA pTextData = (LPTEXTUREDATA) pConst->pData;

				if (SUCCEEDED(g_cTextureTable.GetTexture(&pTexture,pTextData->TextureID)))
				{
					LPDIRECT3DCUBETEXTURE9 pCube = (LPDIRECT3DCUBETEXTURE9) pTexture->m_lpTexture;
					hr = m_HEffect->pEffect->SetTexture(pConst->scName.c_str(),pCube);
				}
				break;
			}
		case MSEFFECT_MATERIALAMBIENT:
			{
				D3DXVECTOR4 Vec;
				Vec.x = pMaterial->m_sMaterial9.Ambient.r;
				Vec.y = pMaterial->m_sMaterial9.Ambient.g;
				Vec.z = pMaterial->m_sMaterial9.Ambient.b;
				Vec.w = pMaterial->m_sMaterial9.Ambient.a;

				//hr = m_HEffect->pEffect->SetVector(pConst->scName.c_str(),&Vec);
				hr = m_HEffect->pEffect->SetValue(pConst->m_hParameter,&Vec,sizeof(D3DXVECTOR4));
				break;
			}
		case MSEFFECT_MATERIALDIFFUSE:
			{
				D3DXVECTOR4 Vec;
				Vec.x = pMaterial->m_sMaterial9.Diffuse.r;
				Vec.y = pMaterial->m_sMaterial9.Diffuse.g;
				Vec.z = pMaterial->m_sMaterial9.Diffuse.b;
				Vec.w = pMaterial->m_sMaterial9.Diffuse.a;

				//hr = m_HEffect->pEffect->SetVector(pConst->scName.c_str(),&Vec);
				hr = m_HEffect->pEffect->SetValue(pConst->m_hParameter,&Vec,sizeof(D3DXVECTOR4));
				break;
			}
		case MSEFFECT_MATERIALSPECULAR:
			{
				D3DXVECTOR4 Vec;
				Vec.x = pMaterial->m_sMaterial9.Specular.r;
				Vec.y = pMaterial->m_sMaterial9.Specular.g;
				Vec.z = pMaterial->m_sMaterial9.Specular.b;
				Vec.w = pMaterial->m_sMaterial9.Specular.a;

				//hr = m_HEffect->pEffect->SetVector(pConst->scName.c_str(),&Vec);
				hr = m_HEffect->pEffect->SetValue(pConst->m_hParameter,&Vec,sizeof(D3DXVECTOR4));

				break;
			}
		case MSEFFECT_MATERIALEMISSIVE:
			{
				D3DXVECTOR4 Vec;
				Vec.x = pMaterial->m_sMaterial9.Emissive.r;
				Vec.y = pMaterial->m_sMaterial9.Emissive.g;
				Vec.z = pMaterial->m_sMaterial9.Emissive.b;
				Vec.w = pMaterial->m_sMaterial9.Emissive.a;

				//hr = m_HEffect->pEffect->SetVector(pConst->scName.c_str(),&Vec);
				hr = m_HEffect->pEffect->SetValue(pConst->m_hParameter,&Vec,sizeof(D3DXVECTOR4));

				break;
			}

		case MSEFFECT_MATERIALPOWER:
			{
				D3DXVECTOR4 Vec;
				Vec.x = pMaterial->m_sMaterial9.Power;
				Vec.y = 0;
				Vec.z = 0;
				Vec.w = 0;

				//hr = m_HEffect->pEffect->SetVector(pConst->scName.c_str(),&Vec);
				hr = m_HEffect->pEffect->SetValue(pConst->m_hParameter,&Vec,sizeof(D3DXVECTOR4));

				break;
			}

		case MSEFFECT_LOCALBBOXMIN:
			{
				D3DXVECTOR4 Vec;
				Vec.x = pModel->m_BBox_A.x;
				Vec.y = pModel->m_BBox_A.y;
				Vec.z = pModel->m_BBox_A.z;
				Vec.w = 0;
				//hr = m_HEffect->pEffect->SetVector(pConst->scName.c_str(),&Vec);
				hr = m_HEffect->pEffect->SetValue(pConst->m_hParameter,&Vec,sizeof(D3DXVECTOR4));

				break;
			}
		case MSEFFECT_LOCALBBOXMAX:
			{
				D3DXVECTOR4 Vec;
				Vec.x = pModel->m_BBox_B.x;
				Vec.y = pModel->m_BBox_B.y;
				Vec.z = pModel->m_BBox_B.z;
				Vec.w = 0;
				//hr = m_HEffect->pEffect->SetVector(pConst->scName.c_str(),&Vec);
				hr = m_HEffect->pEffect->SetValue(pConst->m_hParameter,&Vec,sizeof(D3DXVECTOR4));

				break;
			}
		case MSEFFECT_LOCALBBOXSIZE:
			{
				D3DXVECTOR4 Vec;
				Vec.x = pModel->m_BBox_B.x - pModel->m_BBox_A.x;
				Vec.y = pModel->m_BBox_B.y - pModel->m_BBox_A.y;
				Vec.z = pModel->m_BBox_B.z - pModel->m_BBox_A.z;
				Vec.w = 0;
				//hr = m_HEffect->pEffect->SetVector(pConst->scName.c_str(),&Vec);
				hr = m_HEffect->pEffect->SetValue(pConst->m_hParameter,&Vec,sizeof(D3DXVECTOR4));

				break;
			}
		}

	}

	for(int K =0;K<8;K++)
	{
		if(pMaterial->m_dTextureIDs[K])
		{
			LPTEXTURE pTexture = NULL;

			TCHAR TextName[256];
			wsprintf(TextName,"Tex%d",K);

			if (SUCCEEDED(g_cTextureTable.GetTexture(&pTexture,pMaterial->m_dTextureIDs[K])))
			{
				hr = m_HEffect->pEffect->SetTexture(TextName,pTexture->m_lpTexture);
			}
		}
		else
		{
			break;
		}
	}


	return S_OK;
}

HRESULT KModel::MTLMSEFFECT::OnLoad()
{
	InputConstant.clear();

	if(!m_HEffect->pEffect)
		return E_FAIL;

	D3DXEFFECT_DESC Desc;
	m_HEffect->pEffect->GetDesc(&Desc);

	for(UINT K = 0;K<Desc.Parameters;K++)
	{
		D3DXHANDLE Par_Handle = m_HEffect->pEffect->GetParameter(NULL,K);
		if(Par_Handle)
		{
			D3DXPARAMETER_DESC Par_Desc;
			m_HEffect->pEffect->GetParameterDesc(Par_Handle,&Par_Desc);

			TCHAR str[256];
			wsprintf(str,"%s",Par_Desc.Name);

			switch(Par_Desc.Class)
			{
			case D3DXPC_SCALAR:
				switch(Par_Desc.Type)
				{
				case D3DXPT_VOID:
					{
						SHADERCONSTANT NewCon(MSEFFECT_VOID,str,NULL,Par_Handle);
						InputConstant.push_back(NewCon);
						break;
					}
				case D3DXPT_BOOL:
					{
						SHADERCONSTANT NewCon(MSEFFECT_BOOL,str,NULL,Par_Handle);
						InputConstant.push_back(NewCon);
						break;
					}
				case D3DXPT_INT:
					{
						int * pValue = new int;
						m_HEffect->pEffect->GetValue(Par_Handle,pValue,sizeof(int));

						SHADERCONSTANT NewCon(MSEFFECT_INT,str,pValue,Par_Handle);
						InputConstant.push_back(NewCon);
						break;
					}
				case D3DXPT_FLOAT:
					{
						if(Par_Desc.Semantic)
						{
							if(_stricmp(Par_Desc.Semantic,"MATERIALPOWER")==0)
							{
								SHADERCONSTANT NewCon(MSEFFECT_MATERIALPOWER,str,NULL,Par_Handle);
								InputConstant.push_back(NewCon);
							}
						}
						else
						{
							float * pValue = new float;
							m_HEffect->pEffect->GetValue(Par_Handle,pValue,sizeof(float));

							SHADERCONSTANT NewCon(MSEFFECT_FLOAT,str,pValue,Par_Handle);
							InputConstant.push_back(NewCon);
						}
						break;
					}
				}
				break;
			case D3DXPC_VECTOR:
				if(Par_Desc.Rows==1)
				{
					if(Par_Desc.Semantic!=NULL)
					{
						if(_stricmp(Par_Desc.Semantic,"MATERIALAMBIENT")==0)
						{
							SHADERCONSTANT NewCon(MSEFFECT_MATERIALAMBIENT,str,NULL,Par_Handle);
							InputConstant.push_back(NewCon);
						}
						else if(_stricmp(Par_Desc.Semantic,"MATERIALDIFFUSE")==0)
						{
							SHADERCONSTANT NewCon(MSEFFECT_MATERIALDIFFUSE,str,NULL,Par_Handle);
							InputConstant.push_back(NewCon);
						}
						else if(_stricmp(Par_Desc.Semantic,"MATERIALSPECULAR")==0)
						{
							SHADERCONSTANT NewCon(MSEFFECT_MATERIALSPECULAR,str,NULL,Par_Handle);
							InputConstant.push_back(NewCon);
						}
						else if(_stricmp(Par_Desc.Semantic,"MATERIALEMISSIVE")==0)
						{
							SHADERCONSTANT NewCon(MSEFFECT_MATERIALEMISSIVE,str,NULL,Par_Handle);
							InputConstant.push_back(NewCon);
						}
						else if(_stricmp(Par_Desc.Semantic,"COLOR")==0)
						{
							D3DCOLORVALUE * pValue = new D3DCOLORVALUE;
							m_HEffect->pEffect->GetVector(Par_Handle,(D3DXVECTOR4*)pValue);

							SHADERCONSTANT NewCon(MSEFFECT_COLORRGBA,str,pValue,Par_Handle);
							InputConstant.push_back(NewCon);
						}
						//////////////////////////////////////////////////////////////////////////
						else if(_stricmp(Par_Desc.Semantic,"LOCALBBOXMIN")==0)
						{
							SHADERCONSTANT NewCon(MSEFFECT_LOCALBBOXMIN,str,NULL,Par_Handle);
							InputConstant.push_back(NewCon);
						}
						else if(_stricmp(Par_Desc.Semantic,"LOCALBBOXMAX")==0)
						{
							SHADERCONSTANT NewCon(MSEFFECT_LOCALBBOXMAX,str,NULL,Par_Handle);
							InputConstant.push_back(NewCon);
						}
						else if(_stricmp(Par_Desc.Semantic,"LOCALBBOXSIZE")==0)
						{
							SHADERCONSTANT NewCon(MSEFFECT_LOCALBBOXSIZE,str,NULL,Par_Handle);
							InputConstant.push_back(NewCon);
						}						
					}
					else
					{
						PVOID Pointer = NULL;
						switch(Par_Desc.Columns)
						{
						case 2:
							{
								Pointer = new D3DXVECTOR2(0,0);
								SHADERCONSTANT NewCon(MSEFFECT_FLOAT2,str,Pointer,Par_Handle);
								InputConstant.push_back(NewCon);
								break;
							}
						case 3:
							{
								Pointer = new D3DXVECTOR3(0,0,0);
								SHADERCONSTANT NewCon(MSEFFECT_FLOAT3,str,Pointer,Par_Handle);
								InputConstant.push_back(NewCon);
								break;
							}
						case 4:
							{
								Pointer = new D3DXVECTOR4(0,0,0,0);
								SHADERCONSTANT NewCon(MSEFFECT_FLOAT4,str,Pointer,Par_Handle);
								InputConstant.push_back(NewCon);
								break;
							}
						}
					}

				}

				break;
			case D3DXPC_MATRIX_ROWS:
				{
					if(Par_Desc.Semantic)
					{
						if(_stricmp(Par_Desc.Semantic,"WORLD")==0)
						{
							SHADERCONSTANT NewCon(MSEFFECT_WORLD,str,NULL,Par_Handle);
							InputConstant.push_back(NewCon);
						}
						else if(_stricmp(Par_Desc.Semantic,"WORLDI")==0)
						{
							SHADERCONSTANT NewCon(MSEFFECT_WORLDI,str,NULL,Par_Handle);
							InputConstant.push_back(NewCon);
						}
						else if(_stricmp(Par_Desc.Semantic,"WORLDIT")==0)
						{
							SHADERCONSTANT NewCon(MSEFFECT_WORLDIT,str,NULL,Par_Handle);
							InputConstant.push_back(NewCon);
						}			
						else if(_stricmp(Par_Desc.Semantic,"VIEW")==0)
						{
							SHADERCONSTANT NewCon(MSEFFECT_VIEW,str,NULL,Par_Handle);
							InputConstant.push_back(NewCon);
						}	
						else if(_stricmp(Par_Desc.Semantic,"VIEWI")==0)
						{
							SHADERCONSTANT NewCon(MSEFFECT_VIEWI,str,NULL,Par_Handle);
							InputConstant.push_back(NewCon);
						}	
						else if(_stricmp(Par_Desc.Semantic,"VIEWIT")==0)
						{
							SHADERCONSTANT NewCon(MSEFFECT_VIEWIT,str,NULL,Par_Handle);
							InputConstant.push_back(NewCon);
						}		
						else if(_stricmp(Par_Desc.Semantic,"PROJECTION")==0)
						{
							SHADERCONSTANT NewCon(MSEFFECT_PROJECTION,str,NULL,Par_Handle);
							InputConstant.push_back(NewCon);
						}			
						else if(_stricmp(Par_Desc.Semantic,"PROJECTIONI")==0)
						{
							SHADERCONSTANT NewCon(MSEFFECT_PROJECTIONI,str,NULL,Par_Handle);
							InputConstant.push_back(NewCon);
						}
						else if(_stricmp(Par_Desc.Semantic,"PROJECTIONIT")==0)
						{
							SHADERCONSTANT NewCon(MSEFFECT_PROJECTIONIT,str,NULL,Par_Handle);
							InputConstant.push_back(NewCon);
						}

						else if(_stricmp(Par_Desc.Semantic,"WORLDVIEW")==0)
						{
							SHADERCONSTANT NewCon(MSEFFECT_WORLDVIEW,str,NULL,Par_Handle);
							InputConstant.push_back(NewCon);
						}
						else if(_stricmp(Par_Desc.Semantic,"WORLDVIEWI")==0)
						{
							SHADERCONSTANT NewCon(MSEFFECT_WORLDVIEWI,str,NULL,Par_Handle);
							InputConstant.push_back(NewCon);
						}
						else if(_stricmp(Par_Desc.Semantic,"WORLDVIEWIT")==0)
						{
							SHADERCONSTANT NewCon(MSEFFECT_WORLDVIEWIT,str,NULL,Par_Handle);
							InputConstant.push_back(NewCon);
						}

						else if(_stricmp(Par_Desc.Semantic,"WORLDVIEWPROJ")==0)
						{
							SHADERCONSTANT NewCon(MSEFFECT_WORLDVIEWPROJ,str,NULL,Par_Handle);
							InputConstant.push_back(NewCon);
						}
						else if(_stricmp(Par_Desc.Semantic,"WORLDVIEWPROJI")==0)
						{
							SHADERCONSTANT NewCon(MSEFFECT_WORLDVIEWPROJI,str,NULL,Par_Handle);
							InputConstant.push_back(NewCon);
						}
						else if(_stricmp(Par_Desc.Semantic,"WORLDVIEWPROJIT")==0)
						{
							SHADERCONSTANT NewCon(MSEFFECT_WORLDVIEWPROJIT,str,NULL,Par_Handle);
							InputConstant.push_back(NewCon);
						}
						else if(_stricmp(Par_Desc.Semantic,"SUNTRANSFORMATION")==0)
						{
							SHADERCONSTANT NewCon(MSEFFECT_SUNDIRMATRIX,str,NULL,Par_Handle);
							InputConstant.push_back(NewCon);
						}				
					}

					else
					{
						;//Do nothing
					}
				}
				break;
			case D3DXPC_MATRIX_COLUMNS:
				break;
			case D3DXPC_OBJECT:
				{
					switch(Par_Desc.Type)
					{
					case D3DXPT_STRING:
						{
							TCHAR* pChar = new TCHAR[256];
							SHADERCONSTANT NewCon(MSEFFECT_STRING,str,pChar,Par_Handle);
							InputConstant.push_back(NewCon);
							break;
						}
					case D3DXPT_TEXTURE:
						{
							LPTEXTUREDATA pTextData = new _TextureData;

							DWORD Type = MSEFFECT_TEXTURE;

							for(UINT S = 0;S<Par_Desc.Annotations;S++)
							{
								D3DXHANDLE Ano_Handle = m_HEffect->pEffect->GetAnnotation(Par_Handle,S);
								D3DXPARAMETER_DESC Desc;
								m_HEffect->pEffect->GetParameterDesc(Ano_Handle,&Desc);


								if(Desc.Type==D3DXPT_STRING)
								{
									if(_stricmp(Desc.Name,"type")==0)
									{
										LPCSTR Name = NULL;
										m_HEffect->pEffect->GetString(Ano_Handle,&Name);
										if(_stricmp(Name,"CUBE")==0)
										{
											Type = MSEFFECT_TEXTURECUBE;
										}
										else if(_stricmp(Name,"VOLUME")==0)
										{
											Type = MSEFFECT_TEXTURE3D;
										}
									}
									else if(_stricmp(Desc.Name,"name")==0)
									{
										LPCSTR Name = NULL;
										m_HEffect->pEffect->GetString(Ano_Handle,&Name);
										pTextData->FileName = Name;
									}
								}
							}

							TCHAR TexFile[256];
							if(SUCCEEDED(g_cFileFinder.FindFile(TexFile,(LPSTR)pTextData->FileName.c_str())))
							{
								if(Type==MSEFFECT_TEXTURECUBE)
								{
									g_cTextureTable.LoadCubeFromFile(&pTextData->TextureID,
										TexFile);
								}
								if(Type==MSEFFECT_TEXTURE3D)
								{
									g_cTextureTable.LoadVolumeFromFile(&pTextData->TextureID,
										TexFile);
								}
								else
								{
									g_cTextureTable.LoadFromFile(&pTextData->TextureID,
										TexFile);
								}
							}

							SHADERCONSTANT NewCon(Type,str,pTextData,Par_Handle);
							InputConstant.push_back(NewCon);
							break;
						}
					case D3DXPT_TEXTURE1D:
					case D3DXPT_TEXTURE2D:
					case D3DXPT_TEXTURE3D:
						{
							break;//do nothing
						}
					}

					break;
				}
			case D3DXPC_STRUCT:
				break;

			}
		}
	}


	return S_OK;
}

KModel::LPMATERIAL KModel::GetFirstSelectedMtl()
{
	for(DWORD i=0;i<m_dNumMaterial;i++)
	{
		LPMATERIAL pMtl = &m_lpMaterial[i];
		if(pMtl->m_dOption & MATERIAL_OPTION_SELECTED)
		{
			return pMtl;
		}
	}
	return NULL;
}




HRESULT KModel::MTLMSEFFECT::SetParameter(KModel* pModel)
{
	HRESULT hr = S_OK;

	if(!m_HEffect->pEffect)
		return E_FAIL;

	if(m_nCurTechnique == -1)
	{
		m_HEffect->pEffect->ValidateTechnique(NULL);
		//m_nCurTechnique = 0;
	}
	else
	{
		D3DXHANDLE pTec = m_HEffect->pEffect->GetTechnique(m_nCurTechnique);
		if(pTec)
		{
			m_HEffect->pEffect->SetTechnique(pTec);
		}
	}

	for(UINT K = 0;K<InputConstant.size();K++)
	{
		SHADERCONSTANT* pConst = &InputConstant[K];
		D3DXMATRIX World,View,Project;

		g_pd3dDevice->GetTransform( D3DTS_WORLD, &World );
		g_pd3dDevice->GetTransform( D3DTS_VIEW,  &View );
		g_pd3dDevice->GetTransform( D3DTS_PROJECTION, &Project );

		hr = m_HEffect->pEffect->SetMatrix("World",&World);
		hr = m_HEffect->pEffect->SetMatrix("View" ,&View);
		hr = m_HEffect->pEffect->SetMatrix("Projection",&Project);

		switch(pConst->dwType)
		{
		case MSEFFECT_WORLD:
			{
				D3DXMATRIX World;
				g_pd3dDevice->GetTransform( D3DTS_WORLD, &World );
				//m_HEffect->pEffect->SetMatrix(pConst->scName.c_str(),&World);
				m_HEffect->pEffect->SetValue(pConst->m_hParameter,&World,sizeof(D3DXMATRIX));
				break;
			}
		case MSEFFECT_WORLDI:
			{
				D3DXMATRIX World;
				g_pd3dDevice->GetTransform( D3DTS_WORLD, &World );
				D3DXMatrixInverse(&World,NULL,&World);
				//m_HEffect->pEffect->SetMatrix(pConst->scName.c_str(),&World);
				m_HEffect->pEffect->SetValue(pConst->m_hParameter,&World,sizeof(D3DXMATRIX));
				break;
			}
		case MSEFFECT_WORLDIT:
			{
				D3DXMATRIX World;
				g_pd3dDevice->GetTransform( D3DTS_WORLD, &World );
				D3DXMatrixInverse(&World,NULL,&World);
				D3DXMatrixTranspose(&World,&World);
				//m_HEffect->pEffect->SetMatrix(pConst->scName.c_str(),&World);
				m_HEffect->pEffect->SetValue(pConst->m_hParameter,&World,sizeof(D3DXMATRIX));
				break;
			}

		case MSEFFECT_VIEW:
			{
				D3DXMATRIX World;
				g_pd3dDevice->GetTransform( D3DTS_VIEW, &World );
				//m_HEffect->pEffect->SetMatrix(pConst->scName.c_str(),&World);
				m_HEffect->pEffect->SetValue(pConst->m_hParameter,&World,sizeof(D3DXMATRIX));
				break;
			}
		case MSEFFECT_VIEWI:
			{
				D3DXMATRIX World;
				g_pd3dDevice->GetTransform( D3DTS_VIEW, &World );
				D3DXMatrixInverse(&World,NULL,&World);
				//m_HEffect->pEffect->SetMatrix(pConst->scName.c_str(),&World);
				m_HEffect->pEffect->SetValue(pConst->m_hParameter,&World,sizeof(D3DXMATRIX));
				break;
			}
		case MSEFFECT_VIEWIT:
			{
				D3DXMATRIX World;
				g_pd3dDevice->GetTransform( D3DTS_VIEW, &World );
				D3DXMatrixInverse(&World,NULL,&World);
				D3DXMatrixTranspose(&World,&World);
				//m_HEffect->pEffect->SetMatrix(pConst->scName.c_str(),&World);
				m_HEffect->pEffect->SetValue(pConst->m_hParameter,&World,sizeof(D3DXMATRIX));
				break;
			}

		case MSEFFECT_PROJECTION:
			{
				D3DXMATRIX World;
				g_pd3dDevice->GetTransform( D3DTS_PROJECTION, &World );
				//m_HEffect->pEffect->SetMatrix(pConst->scName.c_str(),&World);
				m_HEffect->pEffect->SetValue(pConst->m_hParameter,&World,sizeof(D3DXMATRIX));
				break;
			}
		case MSEFFECT_PROJECTIONI:
			{
				D3DXMATRIX World;
				g_pd3dDevice->GetTransform( D3DTS_PROJECTION, &World );
				D3DXMatrixInverse(&World,NULL,&World);
				//m_HEffect->pEffect->SetMatrix(pConst->scName.c_str(),&World);
				m_HEffect->pEffect->SetValue(pConst->m_hParameter,&World,sizeof(D3DXMATRIX));
				break;
			}
		case MSEFFECT_PROJECTIONIT:
			{
				D3DXMATRIX World;
				g_pd3dDevice->GetTransform( D3DTS_PROJECTION, &World );
				D3DXMatrixInverse(&World,NULL,&World);
				D3DXMatrixTranspose(&World,&World);
				//m_HEffect->pEffect->SetMatrix(pConst->scName.c_str(),&World);
				m_HEffect->pEffect->SetValue(pConst->m_hParameter,&World,sizeof(D3DXMATRIX));
				break;
			}

		case MSEFFECT_WORLDVIEW:
			{
				D3DXMATRIX World,View;
				g_pd3dDevice->GetTransform( D3DTS_WORLD, &World );
				g_pd3dDevice->GetTransform( D3DTS_VIEW , &View );
				D3DXMatrixMultiply(&World,&World,&View);
				//m_HEffect->pEffect->SetMatrix(pConst->scName.c_str(),&World);
				m_HEffect->pEffect->SetValue(pConst->m_hParameter,&World,sizeof(D3DXMATRIX));
				break;
			}
		case MSEFFECT_WORLDVIEWI:
			{
				D3DXMATRIX World,View;
				g_pd3dDevice->GetTransform( D3DTS_WORLD, &World );
				g_pd3dDevice->GetTransform( D3DTS_VIEW , &View );
				D3DXMatrixMultiply(&World,&World,&View);
				D3DXMatrixInverse(&World,NULL,&World);
				//m_HEffect->pEffect->SetMatrix(pConst->scName.c_str(),&World);
				m_HEffect->pEffect->SetValue(pConst->m_hParameter,&World,sizeof(D3DXMATRIX));
				break;
			}
		case MSEFFECT_WORLDVIEWIT:
			{
				D3DXMATRIX World,View;
				g_pd3dDevice->GetTransform( D3DTS_WORLD, &World );
				g_pd3dDevice->GetTransform( D3DTS_VIEW , &View );
				D3DXMatrixMultiply(&World,&World,&View);
				D3DXMatrixInverse(&World,NULL,&World);
				D3DXMatrixTranspose(&World,&World);
				//m_HEffect->pEffect->SetMatrix(pConst->scName.c_str(),&World);
				m_HEffect->pEffect->SetValue(pConst->m_hParameter,&World,sizeof(D3DXMATRIX));
				break;
			}		

		case MSEFFECT_WORLDVIEWPROJ:
			{
				D3DXMATRIX World,View,Proj;
				g_pd3dDevice->GetTransform( D3DTS_WORLD, &World );
				g_pd3dDevice->GetTransform( D3DTS_VIEW , &View );
				g_pd3dDevice->GetTransform( D3DTS_PROJECTION , &Proj );
				D3DXMatrixMultiply(&World,&World,&View);
				D3DXMatrixMultiply(&World,&World,&Proj);
				//m_HEffect->pEffect->SetMatrix(pConst->scName.c_str(),&World);
				m_HEffect->pEffect->SetValue(pConst->m_hParameter,&World,sizeof(D3DXMATRIX));
				break;
			}
		case MSEFFECT_WORLDVIEWPROJI:
			{
				D3DXMATRIX World,View,Proj;
				g_pd3dDevice->GetTransform( D3DTS_WORLD, &World );
				g_pd3dDevice->GetTransform( D3DTS_VIEW , &View );
				g_pd3dDevice->GetTransform( D3DTS_PROJECTION , &Proj );
				D3DXMatrixMultiply(&World,&World,&View);
				D3DXMatrixMultiply(&World,&World,&Proj);
				D3DXMatrixInverse(&World,NULL,&World);
				//m_HEffect->pEffect->SetMatrix(pConst->scName.c_str(),&World);
				m_HEffect->pEffect->SetValue(pConst->m_hParameter,&World,sizeof(D3DXMATRIX));
				break;
			}
		case MSEFFECT_WORLDVIEWPROJIT:
			{
				D3DXMATRIX World,View,Proj;
				g_pd3dDevice->GetTransform( D3DTS_WORLD, &World );
				g_pd3dDevice->GetTransform( D3DTS_VIEW , &View );
				g_pd3dDevice->GetTransform( D3DTS_PROJECTION , &Proj );
				D3DXMatrixMultiply(&World,&World,&View);
				D3DXMatrixMultiply(&World,&World,&Proj);
				D3DXMatrixInverse(&World,NULL,&World);
				D3DXMatrixTranspose(&World,&World);
				//m_HEffect->pEffect->SetMatrix(pConst->scName.c_str(),&World);
				m_HEffect->pEffect->SetValue(pConst->m_hParameter,&World,sizeof(D3DXMATRIX));
				break;
			}
		case MSEFFECT_SUNDIRMATRIX:
			{
				K3DScene* pScene = g_cGraphicsTool.GetCurScene();
				if(pScene)
				{
					D3DXMATRIX LightTrans;
					//g_pd3dDevice->GetTransform( D3DTS_VIEW , &LightTrans );

					//pScene->m_SunLight.Direction;
					D3DXMatrixLookAtLH(&LightTrans,&D3DXVECTOR3(0,0,0),
						(D3DXVECTOR3*)&pScene->m_SunLight.Direction,
						&D3DXVECTOR3(0,0,0.001f));
					//m_HEffect->pEffect->SetMatrix(pConst->scName.c_str(),&LightTrans);
					m_HEffect->pEffect->SetValue(pConst->m_hParameter,&LightTrans,sizeof(D3DXMATRIX));
				}
				break;
			}
			//////////////////////////////////////////////////////////////////////////
			//values 	
		case MSEFFECT_COLORRGBA:
			{
				D3DXVECTOR4 * pData = (D3DXVECTOR4*) pConst->pData;
				hr = m_HEffect->pEffect->SetVector(pConst->m_hParameter,pData);
				break;
			}
			//////////////////////////////////////////////////////////////////////////
			//values 	
		case MSEFFECT_FLOAT:
			{
				float* pData = (float*) pConst->pData;
				hr = m_HEffect->pEffect->SetValue(pConst->m_hParameter,pData,4);

				break;
			}
		case MSEFFECT_INT:
			{
				int* pData = (int*) pConst->pData;
				hr = m_HEffect->pEffect->SetValue(pConst->m_hParameter,pData,4);
				break;
			}
			//////////////////////////////////////////////////////////////////////////
			//Materials	
		case MSEFFECT_TEXTURE:
		case MSEFFECT_TEXTURECUBE:
			{
				LPTEXTURE pTexture = NULL;
				LPTEXTUREDATA pTextData = (LPTEXTUREDATA) pConst->pData;

				if (SUCCEEDED(g_cTextureTable.GetTexture(&pTexture,pTextData->TextureID)))
				{
					LPDIRECT3DCUBETEXTURE9 pCube = (LPDIRECT3DCUBETEXTURE9) pTexture->m_lpTexture;
					hr = m_HEffect->pEffect->SetTexture(pConst->scName.c_str(),pCube);
				}
				break;
			}
			/*case MSEFFECT_MATERIALAMBIENT:
			{
			D3DXVECTOR4 Vec;
			Vec.x = pMaterial->m_sMaterial9.Ambient.r;
			Vec.y = pMaterial->m_sMaterial9.Ambient.g;
			Vec.z = pMaterial->m_sMaterial9.Ambient.b;
			Vec.w = pMaterial->m_sMaterial9.Ambient.a;

			//hr = m_HEffect->pEffect->SetVector(pConst->scName.c_str(),&Vec);
			hr = m_HEffect->pEffect->SetValue(pConst->m_hParameter,&Vec,sizeof(D3DXVECTOR4));
			break;
			}
			case MSEFFECT_MATERIALDIFFUSE:
			{
			D3DXVECTOR4 Vec;
			Vec.x = pMaterial->m_sMaterial9.Diffuse.r;
			Vec.y = pMaterial->m_sMaterial9.Diffuse.g;
			Vec.z = pMaterial->m_sMaterial9.Diffuse.b;
			Vec.w = pMaterial->m_sMaterial9.Diffuse.a;

			//hr = m_HEffect->pEffect->SetVector(pConst->scName.c_str(),&Vec);
			hr = m_HEffect->pEffect->SetValue(pConst->m_hParameter,&Vec,sizeof(D3DXVECTOR4));
			break;
			}
			case MSEFFECT_MATERIALSPECULAR:
			{
			D3DXVECTOR4 Vec;
			Vec.x = pMaterial->m_sMaterial9.Specular.r;
			Vec.y = pMaterial->m_sMaterial9.Specular.g;
			Vec.z = pMaterial->m_sMaterial9.Specular.b;
			Vec.w = pMaterial->m_sMaterial9.Specular.a;

			//hr = m_HEffect->pEffect->SetVector(pConst->scName.c_str(),&Vec);
			hr = m_HEffect->pEffect->SetValue(pConst->m_hParameter,&Vec,sizeof(D3DXVECTOR4));

			break;
			}
			case MSEFFECT_MATERIALEMISSIVE:
			{
			D3DXVECTOR4 Vec;
			Vec.x = pMaterial->m_sMaterial9.Emissive.r;
			Vec.y = pMaterial->m_sMaterial9.Emissive.g;
			Vec.z = pMaterial->m_sMaterial9.Emissive.b;
			Vec.w = pMaterial->m_sMaterial9.Emissive.a;

			//hr = m_HEffect->pEffect->SetVector(pConst->scName.c_str(),&Vec);
			hr = m_HEffect->pEffect->SetValue(pConst->m_hParameter,&Vec,sizeof(D3DXVECTOR4));

			break;
			}

			case MSEFFECT_MATERIALPOWER:
			{
			D3DXVECTOR4 Vec;
			Vec.x = pMaterial->m_sMaterial9.Power;
			Vec.y = 0;
			Vec.z = 0;
			Vec.w = 0;

			//hr = m_HEffect->pEffect->SetVector(pConst->scName.c_str(),&Vec);
			hr = m_HEffect->pEffect->SetValue(pConst->m_hParameter,&Vec,sizeof(D3DXVECTOR4));

			break;
			}*/

		case MSEFFECT_LOCALBBOXMIN:
			{
				D3DXVECTOR4 Vec;
				Vec.x = pModel->m_BBox_A.x;
				Vec.y = pModel->m_BBox_A.y;
				Vec.z = pModel->m_BBox_A.z;
				Vec.w = 0;
				//hr = m_HEffect->pEffect->SetVector(pConst->scName.c_str(),&Vec);
				hr = m_HEffect->pEffect->SetValue(pConst->m_hParameter,&Vec,sizeof(D3DXVECTOR4));

				break;
			}
		case MSEFFECT_LOCALBBOXMAX:
			{
				D3DXVECTOR4 Vec;
				Vec.x = pModel->m_BBox_B.x;
				Vec.y = pModel->m_BBox_B.y;
				Vec.z = pModel->m_BBox_B.z;
				Vec.w = 0;
				//hr = m_HEffect->pEffect->SetVector(pConst->scName.c_str(),&Vec);
				hr = m_HEffect->pEffect->SetValue(pConst->m_hParameter,&Vec,sizeof(D3DXVECTOR4));

				break;
			}
		case MSEFFECT_LOCALBBOXSIZE:
			{
				D3DXVECTOR4 Vec;
				Vec.x = pModel->m_BBox_B.x - pModel->m_BBox_A.x;
				Vec.y = pModel->m_BBox_B.y - pModel->m_BBox_A.y;
				Vec.z = pModel->m_BBox_B.z - pModel->m_BBox_A.z;
				Vec.w = 0;
				//hr = m_HEffect->pEffect->SetVector(pConst->scName.c_str(),&Vec);
				hr = m_HEffect->pEffect->SetValue(pConst->m_hParameter,&Vec,sizeof(D3DXVECTOR4));

				break;
			}
		}

	}

	return S_OK;
}

HRESULT KModel::ModifySubsetID(DWORD* pSubsetID)
{
	if(!m_pMeshSys)
		return E_FAIL;

	DWORD * pAttrib = NULL;
	if (FAILED(m_pMeshSys->LockAttributeBuffer(0,(DWORD**)&pAttrib)))
		return E_FAIL;
	DWORD NumFaces = m_pMeshSys->GetNumFaces();

	for(DWORD i=0;i<NumFaces;i++)
	{
		DWORD ID = pAttrib[i];
		if ((ID>=0)&&(ID<m_dNumMaterial))
		{
			pAttrib[i] = pSubsetID[ID];
		}
	}

	m_pMeshSys->UnlockAttributeBuffer();

	//////////////////////////////////////////////////////////////////////////
	if (m_dOption&MESH_SKINED)
	{
		CreateSkinInfo(TRUE);
	}
	else
	{
		OptimizeUnSkinedMesh();
	}


	return S_OK;
}

HRESULT KModel::DrawNormal(void)
{
	HRESULT hr = S_OK;

	if ( !m_bDrawNormal )
		return hr;
	LPD3DXMESH pMesh;
	(m_dOption & MESH_OPTIMIZED) ? pMesh = m_pMeshOpt :	pMesh = m_pMeshOpt;

	if(!pMesh)
		return S_OK;

	DWORD dwVertexCnt = pMesh->GetNumVertices();
	hr = g_pd3dDevice->SetStreamSource(0, m_pVBNormal, 0, sizeof(_NormalPoint));
	hr = g_pd3dDevice->SetFVF(FVF_NORMAL);
	hr = g_pd3dDevice->SetTexture(0, NULL);
	hr = g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	hr = g_pd3dDevice->DrawPrimitive(D3DPT_LINELIST, 0, dwVertexCnt);
	return hr;
}

void KModel::GetFaceNormal(D3DXVECTOR3* pvec3StartPoint, D3DXVECTOR3* pvec3Normal)
{
	if ( !pvec3StartPoint || !pvec3Normal)
		return;
	D3DXVECTOR3 vec3Dir1 = *(pvec3StartPoint+1) - *(pvec3StartPoint);
	D3DXVECTOR3 vec3Dir2 = *(pvec3StartPoint+2) - *(pvec3StartPoint + 1);
	D3DXVec3Cross(pvec3Normal, &vec3Dir1, &vec3Dir2);
	D3DXVec3Normalize(pvec3Normal, pvec3Normal);
}


//--------------------------------------------------------------------------------------
// Generate a mesh that can be tesselated.
HRESULT KModel::GenerateEnhancedMesh( UINT dwNewNumSegs,LPD3DXMESH pMesh)
{
	LPD3DXMESH pMeshEnhancedSysMem = NULL;
	LPD3DXMESH pMeshTemp;
	HRESULT    hr;

	if( pMesh == NULL )
		return S_OK;

	// if using hw, just copy the mesh
	if( g_D3DCaps.DevCaps & D3DDEVCAPS_NPATCHES )
	{
		hr = pMesh->CloneMeshFVF( D3DXMESH_WRITEONLY | D3DXMESH_NPATCHES | 
			(pMesh->GetOptions() & D3DXMESH_32BIT), 
			pMesh->GetFVF(), g_pd3dDevice, &pMeshTemp );
		if( FAILED(hr) )
			return hr;
	}
	else  // tesselate the mesh in sw
	{
		LPD3DXBUFFER m_pAdjacencyBuffer = NULL;   // Contains the adjaceny info loaded with the mesh
		if( FAILED( hr = D3DXCreateBuffer(pMesh->GetNumFaces()*3*sizeof(DWORD),
			&m_pAdjacencyBuffer)))
			return hr;
		if( FAILED( hr = pMesh->GenerateAdjacency(0,
			(DWORD*)m_pAdjacencyBuffer->GetBufferPointer())))
			return hr;

		// Create an enhanced version of the mesh, will be in sysmem since source is
		hr = D3DXTessellateNPatches( pMesh, (DWORD*)m_pAdjacencyBuffer->GetBufferPointer(), 
			(float)dwNewNumSegs, FALSE, &pMeshEnhancedSysMem, NULL );
		if( FAILED(hr) )
		{
			// If the tessellate failed, there might have been more triangles or vertices 
			// than can fit into a 16bit mesh, so try cloning to 32bit before tessellation

			hr = pMesh->CloneMeshFVF( D3DXMESH_SYSTEMMEM | D3DXMESH_32BIT, 
				pMesh->GetFVF(), g_pd3dDevice, &pMeshTemp );
			if (FAILED(hr))
				return hr;

			hr = D3DXTessellateNPatches( pMeshTemp, (DWORD*)m_pAdjacencyBuffer->GetBufferPointer(), 
				(float)dwNewNumSegs, FALSE, &pMeshEnhancedSysMem, NULL );
			if( FAILED(hr) )
			{
				pMeshTemp->Release();
				return hr;
			}

			pMeshTemp->Release();
		}

		// Make a vid mem version of the mesh  
		// Only set WRITEONLY if it doesn't use 32bit indices, because those 
		// often need to be emulated, which means that D3DX needs read-access.
		DWORD dwMeshEnhancedFlags = pMeshEnhancedSysMem->GetOptions() & D3DXMESH_32BIT;
		if( (dwMeshEnhancedFlags & D3DXMESH_32BIT) == 0)
			dwMeshEnhancedFlags |= D3DXMESH_WRITEONLY;

		hr = pMeshEnhancedSysMem->CloneMeshFVF( dwMeshEnhancedFlags, pMesh->GetFVF(),
			g_pd3dDevice, &pMeshTemp );
		if( FAILED(hr) )
		{
			SAFE_RELEASE( pMeshEnhancedSysMem );
			return hr;
		}

		// Latch in the enhanced mesh
		SAFE_RELEASE( pMeshEnhancedSysMem );
	}

	SAFE_RELEASE( m_pMeshEnhanced );
	m_pMeshEnhanced = pMeshTemp;
	//g_dwNumSegs     = dwNewNumSegs;

	return S_OK;
}

HRESULT KModel::CreateProgressMesh(LPD3DXMESH pMesh)
{
	HRESULT hr = S_OK;

	GenerateEnhancedMesh(2,pMesh);

	LPD3DXBUFFER pAdjacencyBuffer = NULL;   // Contains the adjaceny info loaded with the mesh

	if(!m_pMeshBeforeProgress)
	{
		LPD3DXMESH pTempMesh = NULL;
		if( FAILED( hr = D3DXCreateBuffer(pMesh->GetNumFaces()*3*sizeof(DWORD),
			&pAdjacencyBuffer)))
			return hr;
		if( FAILED( hr = pMesh->GenerateAdjacency(0,
			(DWORD*)pAdjacencyBuffer->GetBufferPointer())))
			return hr;	
		
		// perform simple cleansing operations on mesh
		//////////////////////////////////////////////////////////////////////////
		//D3D9c Change
		if( FAILED( hr = D3DXCleanMesh( D3DXCLEAN_SIMPLIFICATION, pMesh,
			(DWORD*)pAdjacencyBuffer->GetBufferPointer(), &pTempMesh, 
			(DWORD*)pAdjacencyBuffer->GetBufferPointer(), NULL ) ) )
		{
			//m_dwNumMaterials = 0;
			goto End;
		}

		SAFE_RELEASE(m_pMeshBeforeProgress);
		m_pMeshBeforeProgress = pTempMesh;
		D3DXWELDEPSILONS Epsilons;

		//  Perform a weld to try and remove excess vertices like the model bigship1.x in the DX9.0 SDK (current model is fixed)
		//    Weld the mesh using all epsilons of 0.0f.  A small epsilon like 1e-6 works well too
		memset(&Epsilons, 0, sizeof(D3DXWELDEPSILONS));
		if( FAILED( hr = D3DXWeldVertices( pMesh, 0, &Epsilons, 
			(DWORD*)pAdjacencyBuffer->GetBufferPointer(), 
			(DWORD*)pAdjacencyBuffer->GetBufferPointer(), NULL, NULL ) ) )
		{
			//m_dwNumMaterials = 0;
			goto End;
		}

		// verify validity of mesh for simplification
		if( FAILED( hr = D3DXValidMesh( pMesh, (DWORD*)pAdjacencyBuffer->GetBufferPointer(), NULL ) ) )
		{
			//m_dwNumMaterials = 0;
			goto End;
		}

		SAFE_RELEASE(pAdjacencyBuffer);
		SAFE_DELETE_ARRAY(m_pVerticesLodWeight);
		m_pVerticesLodWeight =  new float[m_pMeshBeforeProgress->GetNumVertices()];
		for(DWORD i=0;i<m_pMeshBeforeProgress->GetNumVertices();i++)
		{
			m_pVerticesLodWeight[i] = 1.0f;
		}
	}

	if( FAILED( hr = D3DXCreateBuffer(m_pMeshBeforeProgress->GetNumFaces()*3*sizeof(DWORD),
		&pAdjacencyBuffer)))
		return hr;
	if( FAILED( hr = m_pMeshBeforeProgress->GenerateAdjacency(0,
		(DWORD*)pAdjacencyBuffer->GetBufferPointer())))
		return hr;	
	
	SAFE_RELEASE(m_pMeshProgress);
	hr = D3DXGeneratePMesh( m_pMeshBeforeProgress,
		(DWORD*)pAdjacencyBuffer->GetBufferPointer(),
		NULL,
		m_pVerticesLodWeight, 
		1, 
		D3DXMESHSIMP_VERTEX,
		&m_pMeshProgress);
	if( FAILED(hr) )
		goto End;

	m_dOption |= MESH_PROGRESS;

	m_nNumCurFacesLOD = m_pMeshProgress->GetMaxFaces() /2 ;
	m_pMeshProgress->SetNumFaces(m_nNumCurFacesLOD);

	SAFE_RELEASE(pAdjacencyBuffer);

	return S_OK;

End:
	SAFE_RELEASE(pAdjacencyBuffer);
	SAFE_DELETE_ARRAY(m_pVerticesLodWeight);

	return E_FAIL;
}

//////////////////////////////////////////////////////////////////////////
//PRT START
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//For 9.0b
//////////////////////////////////////////////////////////////////////////
#if D3D_SDK_VERSION == 31
HRESULT	KModel::PRT_LoadResult()
{
	HRESULT hr;

	SAFE_RELEASE(m_pSHPRTBuffer);

	if( FAILED( hr = PRT_ReadBufferFromFile( m_strResultsFile, &m_pSHPRTBuffer ) ) )
	{
		//MessageBox( m_hWnd, TEXT("Couldn't load simulator results file.  Run the simulator first to precompute the transfer vectors for this mesh."), TEXT("Error"), MB_OK );
		return DXTRACE_ERR( TEXT("ReadSHPRTBufferFromFile"), hr );
	}

	// Compress the saved PRT simulator results 
	hr = PRT_CompressData();
	if( FAILED(hr) )    
		return DXTRACE_ERR( TEXT("CompressData"), hr );

	return S_OK;
}


-----------------------------------------------------------------------------
Name: SaveSHPRTBufferToFile()
Desc: Save a SH PRT buffer to a file
	  -----------------------------------------------------------------------------
	  HRESULT KModel::PRT_SaveSHPRTBufferToFile()
{
	// Create a file

	HANDLE hFile = CreateFile( m_strResultsFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL, NULL );
	if( hFile == INVALID_HANDLE_VALUE )
		return DXTRACE_ERR( TEXT("CreateFile"), E_FAIL );

	// Get the buffer pointer & size
	VOID* pBuffer      = m_pSHPRTBuffer->GetBufferPointer();
	DWORD dwBufferSize = m_pSHPRTBuffer->GetBufferSize();

	// Write to the file
	DWORD dwWritten = 0;
	int nResult = WriteFile( hFile, pBuffer, dwBufferSize, &dwWritten, NULL );
	DWORD lasterr = GetLastError();
	if( nResult == 0 || dwWritten != dwBufferSize )
	{
		CloseHandle( hFile );
		return DXTRACE_ERR( TEXT("WriteFile"), E_FAIL );
	}

	CloseHandle( hFile );
	return S_OK;
}

HRESULT KModel::PRT_StopSimulation()
{
	return S_OK;
}

-----------------------------------------------------------------------------
Name: LaunchPRTSimulation()
Desc: Get the settings from the user and launch the PRT simulator on another 
	  thread cause it'll likely take a while and the UI would be 
	  unresponsive otherwise
	  -----------------------------------------------------------------------------
	  HRESULT KModel::PRT_LaunchSimulation()
{
	TCHAR drive[256];
	TCHAR dir[256];
	TCHAR fname[256];
	TCHAR ext[256];
	HRESULT hr;

	//Check exists saved result
	_splitpath(m_szMeshFileName, drive, dir, fname, ext);
	TCHAR szExt[] = ".PRT";
	int iLen = strlen(fname);
	TCHAR szSaveFileName[256];
	strcpy(szSaveFileName, fname);
	strcpy(szSaveFileName + iLen, szExt);
	g_cFileFinder.FindFile(m_strResultsFile, szSaveFileName);
	if ( strcmp(m_strResultsFile, "") )
	{
		int iResult = MessageBox(g_hBaseWnd, "Do you want to recaculate?", "PRT file exists", MB_OKCANCEL);
		if ( iResult == IDCANCEL )
		{
			//Load saved data
			PRT_LoadResult();
			//PRT_LoadSHPRTEffect();
			MessageBox(g_hBaseWnd, "Loading Complete...", "OK", MB_OK);
			m_bSHPRTReadyToRender = true;
			return S_OK;
		}
	}
	//Caculate new data 
	strcpy(m_strResultsFile, m_szMeshFileName);
	int iFileNameLen = strlen(m_szMeshFileName) - strlen(ext);
	strcpy(m_strResultsFile + iFileNameLen, szExt);
	InitializeCriticalSection( &m_cs );

	g_cGraphicsTool.SetPRTSimulationModel(this);

	if( m_hPRTSimulationThreadId )
	{
		// Ask to stop the PRT simulator if it's running in the other thread
		DWORD dwResult = WaitForSingleObject( m_hPRTSimulationThreadId, 0 );
		if( dwResult == WAIT_TIMEOUT )
		{
			int nResult = MessageBox( g_hBaseWnd , TEXT("Simulation still processing.  Abort current simulation?"), TEXT("Warning"), MB_YESNO|MB_ICONWARNING );
			if( nResult == IDNO )
				return S_OK;

			if( FAILED( hr = PRT_StopSimulation() ) )
				return hr;
		}
	}

	m_bStopSimulator = false;
	m_bPRTSimulationComplete = false;

	// Ask the user about param settings for the PRT Simulation
	//int nResult = (int) DialogBox( NULL, MAKEINTRESOURCE(IDD_SIMULATION_OPTIONS),
	//	g_hBaseWnd, StaticSimulationOptionsDlgProc );

	//if( nResult == IDOK )
	{
		//DialogBox( g_hInst, MAKEINTRESOURCE(IDD_COMPRESS), m_hWnd, StaticCompressDlgProc );

		m_bPRTSimulationFailed = false;
		m_bSHPRTReadyToRender = false;
		//m_nTechnique = 1;

		// Launch the PRT simulator on another thread cause it'll 
		// likely take a while and the UI would be unresponsive otherwise
		m_hPRTSimulationThreadId = CreateThread( NULL, 0, PRT_StaticPRTSimulationThreadProc, 
			this, 0, &m_dwPRTSimulationThreadId );
	}

	//HMENU hMenu = GetSubMenu( GetMenu( m_hWnd ), 1 );
	//EnableMenuItem( hMenu, IDM_ABORT_SIMULATION, (m_hPRTSimulationThreadId != NULL) ? MF_ENABLED : MF_GRAYED  );

	//Pause(false);

	DeleteCriticalSection( &m_cs );

	//PRT_LoadSHPRTEffect();

	return S_OK;
}

HRESULT KModel::PRT_ReadBufferFromFile( TCHAR* strFile, ID3DXBuffer** ppd3dxBuffer )
{
	HRESULT hr;

	// Open the file
	HANDLE hFile = CreateFile( strFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL, NULL );
	if( hFile == INVALID_HANDLE_VALUE )
		return DXTRACE_ERR( TEXT("CreateFile"), E_FAIL );

	// Get the file size
	DWORD dwFileSize = GetFileSize( hFile, NULL );

	// Make an ID3DXBuffer
	hr = D3DXCreateBuffer( dwFileSize, ppd3dxBuffer );
	if( FAILED(hr) )
		return DXTRACE_ERR( TEXT("D3DXCreateBuffer"), hr );

	VOID* pBuffer = (*ppd3dxBuffer)->GetBufferPointer();

	// Fill up the ID3DXBuffer
	DWORD dwRead = 0;
	int nResult = ReadFile( hFile, pBuffer, dwFileSize, &dwRead, NULL );
	if( nResult == 0 || dwRead != dwFileSize )
	{
		SAFE_RELEASE( *ppd3dxBuffer );
		CloseHandle( hFile );
		return DXTRACE_ERR( TEXT("ReadFile"), E_FAIL );
	}

	CloseHandle( hFile );
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: StaticPRTSimulationThreadProc
// Desc: static helper function
//-----------------------------------------------------------------------------
DWORD WINAPI KModel::PRT_StaticPRTSimulationThreadProc( LPVOID lpParameter )
{   
	return g_cGraphicsTool.PRTSimulationThreadProc();
}

//-----------------------------------------------------------------------------
// Name: SetupVBWithPCAData()
// Desc: Setup the vertex buffer with PCA data
//-----------------------------------------------------------------------------
HRESULT KModel::PRT_SetupVBWithPCAData( ID3DXMesh* pMesh,
									   ID3DXBuffer* pCPCABuffer, 
									   D3DXSHPRTBUFFER_DESC desc,
									   DWORD dwNumClusters, 
									   DWORD dwNumPCAVectors )
{
	HRESULT hr;
	//PRT_AdjustMeshDecl(&pMesh);
	// Each vertex is now assigned to a cluster.  Call D3DXSHPRTCompExtractClusterIDs()
	// to get the cluster ID for every vertex.  Simply pass in an array of UINTs
	// that is the size of the number of vertices (which equals the number of samples), and 
	// the cluster ID for vertex N will be at puClusterIDs[N].
	UINT* puClusterIDs = new UINT[ desc.NumSamples ];
	if( puClusterIDs == NULL )
		return DXTRACE_ERR( TEXT("new"), E_OUTOFMEMORY );

	hr = D3DXSHPRTCompExtractClusterIDs( pCPCABuffer, puClusterIDs );
	if( FAILED(hr) )
		return DXTRACE_ERR( TEXT("D3DXSHPRTCompExtractClusterIDs"), hr );

	// Now use the cluster ID info to store a value in the mesh in the 
	// D3DDECLUSAGE_BLENDWEIGHT[0].  This value will be passed into the 
	// vertex shader to allow the shader use this number as an offset 
	// into a large array.  The value is based on the cluster ID and the
	// stride of the array.  
	BYTE* pV = NULL;
	hr = m_pMeshPRT->LockVertexBuffer( 0, (void**) &pV );
	if( FAILED(hr) )
		return DXTRACE_ERR( TEXT("LockVertexBuffer"), hr );

	UINT uStride = m_pMeshPRT->GetNumBytesPerVertex();
	BYTE* pClusterID = pV + 32; // 32 == D3DDECLUSAGE_BLENDWEIGHT[0] offset
	for( UINT uVert = 0; uVert < desc.NumSamples; uVert++ ) 
	{
		// The array in vertex shader is an array of float4.
		// For each cluster, there are (1+3*(NUM_PCA_VECTORS/4)) float4's.
		// We store a number that lets the shader index the constant table to get 
		// to the correct cluster for this vertex.
		float fArrayOffset = (float)(puClusterIDs[uVert] * (1+3*(m_dwNumPCAVectors/4))); 
		memcpy(pClusterID, &fArrayOffset, sizeof(float));
		pClusterID += uStride;
	}

	m_pMeshPRT->UnlockVertexBuffer();
	SAFE_DELETE_ARRAY(puClusterIDs);

	// Store the PCA weights in the mesh starting at D3DDECLUSAGE_BLENDWEIGHT[1].  
	// D3DDECLUSAGE_BLENDWEIGHT[1-6] are float4.  So if dwNumPCAVectors == 16, then 
	// this function will write data to D3DDECLUSAGE_BLENDWEIGHT[1] 
	// through D3DDECLUSAGE_BLENDWEIGHT[4].
	hr = D3DXSHPRTCompExtractToMesh( dwNumPCAVectors, m_pMeshPRT, pCPCABuffer, 
		D3DDECLUSAGE_BLENDWEIGHT, 1 );
	if( FAILED(hr) )
		return DXTRACE_ERR( TEXT("D3DXSHPRTCompExtractToMesh"), hr );

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: CompressData()
// Desc: This sets up the CPCA data needed by the vertex shader
//-----------------------------------------------------------------------------
HRESULT KModel::PRT_CompressData()
{
	HRESULT hr;

	m_bSHPRTTooManyConstants = false;

	PRT_AdjustMeshDecl(&m_pMeshSys);
	// Ensure correct state 
	if( m_pSHPRTBuffer == NULL || 
		m_pMeshPRT == NULL )//|| 
		//m_hPRTSimulationThreadId != NULL )
	{
		return E_FAIL;
	}

	// The number of vertex consts need by the shader can't exceed the 
	// amount the HW can support
	DWORD dwNumVConsts = m_dwNumClusters * (1 + MAX_NUM_CHANNELS*m_dwNumPCAVectors/4) + 4;
	if( dwNumVConsts > g_D3DCaps.MaxVertexShaderConst )
	{
		m_bSHPRTTooManyConstants = true;
		return E_FAIL;
	}

	// Get the description of the SH PRT buffer
	D3DXSHPRTBUFFER_DESC desc;
	ZeroMemory( &desc, sizeof(D3DXSHPRTBUFFER_DESC) );
	hr = D3DXSHPRTExtractDesc( m_pSHPRTBuffer, &desc );
	if( FAILED(hr) )
		return DXTRACE_ERR( TEXT("D3DXSHPRTExtractDesc"), hr );

	// Sanity check since the user could load an incompatible mesh & results file
	int iNumV = m_pMeshPRT->GetNumVertices();
	if(  m_pMeshPRT->GetNumVertices()!= desc.NumSamples )
	{
		//MessageBox( m_hWnd, TEXT("Number of vertices on mesh differ from number of samples in the precomputed results file.  Where the results were computed with a different mesh?"), TEXT("Error"), MB_OK );
		SAFE_RELEASE( m_pSHPRTBuffer );
		return E_FAIL;
	}

	// Make note of the order used & if its spectral data
	m_dwOrder = desc.Order;
	m_bSpectral = (desc.NumChannels == 3) ? true : false;

	// Compress the results buffer using dwNumClusters clusters, 
	// and dwNumPCAVectors PCA vectors.  pCPCABuffer will contain the 
	// compressed buffer.
	ID3DXBuffer* pCPCABuffer = NULL;
	hr = D3DXSHPRTCompress( desc.Order, m_pSHPRTBuffer, 
		D3DXSHCQUAL_FASTLOWQUALITY, m_dwNumClusters, m_dwNumPCAVectors, 
		&pCPCABuffer );
	if( FAILED(hr) )
	{
		//MessageBox( m_hWnd, TEXT("D3DXSHPRTCompress failed.  Ensure that the number of clusters is smaller than the number of mesh vertices"), TEXT("Error"), MB_OK );
		DXTRACE_ERR( TEXT("D3DXSHPRTCompress"), hr );
		return S_OK;
	}



	// Setup the vertex buffer with PCA data
	hr = PRT_SetupVBWithPCAData( m_pMeshPRT, pCPCABuffer, desc, m_dwNumClusters, m_dwNumPCAVectors );
	if( FAILED(hr) )    
		return DXTRACE_ERR( TEXT("SetupVBWithPCAData"), hr );

	// Extract the cluster bases into a large array of floats.  
	// D3DXSHPRTCompExtractBasis will extract the basis 
	// for a single cluster.  The basis for a cluster is an array of
	// (NumPCAVectors+1)*(NumChannels*Order^2) floats.  
	// The "1+" is for the cluster mean.
	int nChannelSize   = desc.Order * desc.Order; 
	int nPCAVectorSize = nChannelSize * desc.NumChannels;  
	int nBasisSize     = nPCAVectorSize * (m_dwNumPCAVectors+1);  
	int nBufferSize    = nBasisSize * m_dwNumClusters; 

	SAFE_DELETE_ARRAY( m_aClusterBases );
	m_aClusterBases = new float[nBufferSize];
	if( m_aClusterBases == NULL )
		return DXTRACE_ERR( TEXT("new"), E_OUTOFMEMORY );

	for( DWORD iCluster = 0; iCluster < m_dwNumClusters; iCluster++ ) 
	{
		// D3DXSHPRTCompExtractBasis() extracts the basis for a single 
		// cluster at a time.
		hr = D3DXSHPRTCompExtractBasis( iCluster, pCPCABuffer, 
			&m_aClusterBases[iCluster * nBasisSize] );
		if( FAILED( hr ) )
			return DXTRACE_ERR( TEXT("D3DXSHPRTCompExtractBasis"), hr );
	}

	SAFE_DELETE_ARRAY( m_aClusteredPCA );
	m_aClusteredPCA = new float[m_dwNumClusters*(4+MAX_NUM_CHANNELS*m_dwNumPCAVectors)];
	if( m_aClusteredPCA == NULL )
		return DXTRACE_ERR( TEXT("new"), E_OUTOFMEMORY );

	// Cleanup and allow rendering using the SH PRT data
	SAFE_RELEASE( pCPCABuffer );

	m_bSHPRTReadyToRender = true;

	// If this fails, there should be debug output as to 
	// they the .fx file failed to compile
	hr = PRT_LoadSHPRTEffect();
	/*if( FAILED(hr) )    
	return DXTRACE_ERR( TEXT("LoadSHPRTEffect"), hr );*/

	// Evaluate the lights in terms of SH coefficients 
	// and set the shader constants, and call this function
	// whenever the lights or object rotates and need to be reevaluated
	PRT_UpdateLightDir();
	if( FAILED( hr = PRT_EvalLightsAndSetConstants() ) )
		return hr;

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: UpdateLightDir()
// Desc: Make the arcball respond correctly when the camera has be rotated.
//-----------------------------------------------------------------------------
HRESULT KModel::PRT_UpdateLightDir()
{
	//D3DXMATRIX mInvView;
	//D3DXMATRIX m_mView;
	//g_pd3dDevice->GetTransform(D3DTS_VIEW, &m_mView);
	//D3DXMatrixInverse(&mInvView, NULL, &m_mView);
	//mInvView._41 = mInvView._42 = mInvView._43 = 0;

	//D3DXMATRIX mLastRotInv;
	//D3DXMatrixInverse(&mLastRotInv, NULL, &m_mLightRotSnapshot[m_nActiveLight]);

	//D3DXMATRIX mRot;// = *m_LightArcBall[m_nActiveLight].GetRotationMatrix();
	//D3DXMatrixRotationY(&mRot, timeGetTime()/1000.0f);
	////D3DXMatrixIdentity(&mRot);
	//m_mLightRotSnapshot[m_nActiveLight] = mRot;

	//// Accumulate the delta of the arcball's rotation in view space.
	//// Note that per-frame delta rotations could be problematic over long periods of time.
	//m_mLightRot[m_nActiveLight] *= m_mView * mLastRotInv * mRot * mInvView;

	//// Since we're accumulating delta rotations, we need to orthonormalize 
	//// the matrix to prevent eventual matrix skew
	//D3DXVECTOR3* pXBasis = (D3DXVECTOR3*) &m_mLightRot[m_nActiveLight]._11;
	//D3DXVECTOR3* pYBasis = (D3DXVECTOR3*) &m_mLightRot[m_nActiveLight]._21;
	//D3DXVECTOR3* pZBasis = (D3DXVECTOR3*) &m_mLightRot[m_nActiveLight]._31;
	//D3DXVec3Normalize( pXBasis, pXBasis );
	//D3DXVec3Cross( pYBasis, pZBasis, pXBasis );
	//D3DXVec3Normalize( pYBasis, pYBasis );
	//D3DXVec3Cross( pZBasis, pXBasis, pYBasis );

	//// Transform the default direction vector by the light's rotation matrix
	//D3DXVec3TransformNormal( &m_vLightDir[m_nActiveLight], &m_vDefaultLightDir[m_nActiveLight], &m_mLightRot[m_nActiveLight] );

	m_vLightDir[0].z = sin((float)timeGetTime() / 1000.0f);
	m_vLightDir[0].x = 0;//cos((float)timeGetTime() / 1000.0f);//* sin(float(timeGetTime() / 1000.0f));
	m_vLightDir[0].y = cos((float)timeGetTime() / 1000.0f);
	D3DXVec3Normalize(&m_vLightDir[0], &m_vLightDir[0]);

	m_vLightDir[1].z = sin((float)timeGetTime() / 1000.0f);
	m_vLightDir[1].x = cos((float)timeGetTime() / 1000.0f);//* sin(float(timeGetTime() / 1000.0f));
	m_vLightDir[1].y = 0;//cos((float)timeGetTime() / 1000.0f);
	D3DXVec3Normalize(&m_vLightDir[1], &m_vLightDir[1]);

	m_vLightDir[2].z = 0;
	m_vLightDir[2].x = sin((float)timeGetTime() / 1000.0f);//cos((float)timeGetTime() / 1000.0f);//* sin(float(timeGetTime() / 1000.0f));
	m_vLightDir[2].y = cos((float)timeGetTime() / 1000.0f);
	D3DXVec3Normalize(&m_vLightDir[2], &m_vLightDir[2]);
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: RenderMeshWithSHPRT()
// Desc: Render the scene with PRT
//-----------------------------------------------------------------------------
HRESULT KModel::PRT_RenderMeshWithSHPRT()
{
	HRESULT hr;
	UINT iPass, cPasses;
	KModelTable* pTable = (KModelTable*) m_pMeshTable;
	D3DXMATRIX matView;
	D3DXMATRIX matProj;
	D3DXMATRIX matWorld;
	if( !m_bSHPRTReadyToRender || m_aClusterBases == NULL || 
		m_pMeshPRT == NULL    || m_pSHPRTEffect == NULL )
		return S_OK;
	D3DXHANDLE hTex0 = m_pSHPRTEffect->GetParameterByName(NULL, "g_Tex0");
	//m_pSHPRTEffect->SetTexture(hTex0, )
	PRT_UpdateLightDir();

	g_pd3dDevice->GetTransform(D3DTS_VIEW, &matView);
	g_pd3dDevice->GetTransform(D3DTS_PROJECTION, &matProj);
	g_pd3dDevice->GetTransform(D3DTS_WORLD, &matWorld);
	D3DXMATRIX mWorldViewProj  = matWorld * matView * matProj;
	m_pSHPRTEffect->SetMatrix( "mWorldViewProjection", &mWorldViewProj);
	if( FAILED( hr = PRT_EvalLightsAndSetConstants() ) )
		return hr;

	// Set the technique and render the scene
	m_pSHPRTEffect->SetTechnique( "PrecomputedSHLighting" );

	if( FAILED( hr = m_pSHPRTEffect->Begin(&cPasses, 0) ) )
		return DXTRACE_ERR( TEXT("m_pSHPRTEffect->Begin"), hr );
	for (iPass = 0; iPass < cPasses; iPass++)
	{
		if( FAILED( hr = m_pSHPRTEffect->BegineBeginPass(iPass) ) )
			return DXTRACE_ERR( TEXT("m_pSHPRTEffect->Pass"), hr );

		//DWORD dwAttribs = 0;
		//m_pMeshPRT->GetAttributeTable( NULL, &dwAttribs );

		for( DWORD i=0; i<m_dNumMaterial; i++ )
		{
			LPMATERIAL pMtl = &m_lpMaterial[i];
			LPTEXTURE pTexture = NULL;
			if(SUCCEEDED(g_cTextureTable.GetTexture(&pTexture,pMtl->m_dTextureIDs[0])))
			{
				m_pSHPRTEffect->SetTexture("Tex0", pTexture->m_lpTexture);
			}
			if( FAILED( hr = m_pMeshPRT->DrawSubset(i) ) )
				return DXTRACE_ERR( TEXT("m_pMesh->DrawSubset"), hr );

		}
	}

	m_pSHPRTEffect->End();
	return S_OK;
}


//-----------------------------------------------------------------------------
// Name: AdjustMeshDecl()
// Desc: Make the mesh have a known decl in order to pass per vertex CPCA 
//       data to the shader
//-----------------------------------------------------------------------------
//HRESULT KModel::PRT_AdjustMeshDecl( ID3DXMesh** ppMesh )
//{
//	HRESULT hr;
//	//////////////////////////////////////////////////////////////////////////
//	//LPD3DXMESH pInMesh = m_pMeshSys;
//	//////////////////////////////////////////////////////////////////////////
//
//
//	LPD3DXMESH pInMesh;
//	if(m_dOption & MESH_OPTIMIZED)
//		pInMesh = m_pMeshOpt;
//	else 
//		pInMesh = m_pMeshSys;
//
//	SAFE_RELEASE(m_pMeshPRT);
//
//	//LPD3DXMESH pOutMesh = NULL;
//
//	D3DVERTEXELEMENT9 decl[MAX_FVF_DECL_SIZE] = 
//	{
//		{0,  0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
//		{0,  12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
//		{0,  24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
//		{0,  32, D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 0},
//		{0,  36, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 1},
//		{0,  52, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 2},
//		{0,  68, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 3},
//		{0,  84, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 4},
//		{0, 100, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 5},
//		{0, 116, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 6},
//		D3DDECL_END()
//	};
//
//	// To do CPCA, we need to store (m_dwNumPCAVectors + 1) scalers per vertex, so 
//	// make the mesh have a known decl to store this data.  Since we can't do 
//	// skinning and PRT at once, we use D3DDECLUSAGE_BLENDWEIGHT[0] 
//	// to D3DDECLUSAGE_BLENDWEIGHT[6] to store our per vertex data needed for PRT.
//	// Notice that D3DDECLUSAGE_BLENDWEIGHT[0] is a float1, and
//	// D3DDECLUSAGE_BLENDWEIGHT[1]-D3DDECLUSAGE_BLENDWEIGHT[6] are float4.  This allows 
//	// up to 24 PCA weights and 1 float that gives the vertex shader 
//	// an index into the vertex's cluster's data
//	hr = pInMesh->CloneMesh( pInMesh->GetOptions(), 
//		decl, 
//		g_pd3dDevice, &m_pMeshPRT );
//	if( FAILED(hr) )
//		return DXTRACE_ERR( TEXT("CloneMeshFVF"), hr );
//
//	return S_OK;
//}



//-----------------------------------------------------------------------------
// Name: LoadSHPRTEffect()
// Desc: Load the SHPRTVertex.fx effect file
//-----------------------------------------------------------------------------
//HRESULT KModel::PRT_LoadSHPRTEffect()
//{
//	HRESULT hr;
//
//	// The number of vertex consts need by the shader can't exceed the 
//	// amount the HW can support
//
//	DWORD dwNumVConsts = m_dwNumClusters * (1 + MAX_NUM_CHANNELS*m_dwNumPCAVectors/4) + 4;
//	if( dwNumVConsts > g_D3DCaps.MaxVertexShaderConst )
//		return E_FAIL;
//
//	SAFE_RELEASE( m_pSHPRTEffect );
//
//	D3DXMACRO aDefines[3];
//	CHAR szMaxNumClusters[64];
//	_snprintf( szMaxNumClusters, 64, "%d", m_dwNumClusters );
//	szMaxNumClusters[63] = 0;
//	CHAR szMaxNumPCAVectors[64];
//	_snprintf( szMaxNumPCAVectors, 64, "%d", m_dwNumPCAVectors );
//	szMaxNumPCAVectors[63] = 0;
//	aDefines[0].Name       = "NUM_CLUSTERS";
//	aDefines[0].Definition = szMaxNumClusters;
//	aDefines[1].Name       = "NUM_PCA_VECTORS";
//	aDefines[1].Definition = szMaxNumPCAVectors;
//	aDefines[2].Name       = NULL;
//	aDefines[2].Definition = NULL;
//
//	// Define DEBUG_VS and/or DEBUG_PS to debug vertex and/or pixel shaders with the shader debugger.  
//	// Debugging vertex shaders requires either REF or software vertex processing, and debugging 
//	// pixel shaders requires REF.  The D3DXSHADER_FORCE_*_SOFTWARE_NOOPT flag improves the debug 
//	// experience in the shader debugger.  It enables source level debugging, prevents instruction 
//	// reordering, prevents dead code elimination, and forces the compiler to compile against the next 
//	// higher available software target, which ensures that the unoptimized shaders do not exceed 
//	// the shader model limitations.  Setting these flags will cause slower rendering since the shaders 
//	// will be unoptimized and forced into software.  See the DirectX documentation for more information 
//	// about using the shader debugger.
//	DWORD dwShaderFlags = 0;
//#ifdef DEBUG_VS
//	dwShaderFlags |= D3DXSHADER_FORCE_VS_SOFTWARE_NOOPT;
//#endif
//#ifdef DEBUG_PS
//	dwShaderFlags |= D3DXSHADER_FORCE_PS_SOFTWARE_NOOPT;
//#endif
//
//	// Read the D3DX effect file
//	TCHAR str[MAX_PATH];
//	g_cFileFinder.FindFile(str,"SHPRTVertex.fx");
//
//	hr = D3DXCreateEffectFromFile( g_pd3dDevice, str, aDefines, NULL, 
//		dwShaderFlags, NULL, &m_pSHPRTEffect, NULL );
//
//	// If this fails, there should be debug output as to 
//	// they the .fx file failed to compile
//	if( FAILED( hr ) )
//		return DXTRACE_ERR( TEXT("D3DXCreateEffectFromFile"), hr );
//
//	// Make sure the technique works on this card
//	hr = m_pSHPRTEffect->ValidateTechnique( "PrecomputedSHLighting" );
//	if( FAILED( hr ) )
//		return DXTRACE_ERR( TEXT("ValidateTechnique"), hr );
//
//	return S_OK;
//}




//-----------------------------------------------------------------------------
// Name: EvalLightsAndSetConstants()
// Desc: Evaluate the lights in terms of SH coefficients 
//       and set the shader constants, and call this function
//       whenever the lights or object rotates and need to be reevaluated
//-----------------------------------------------------------------------------
HRESULT KModel::PRT_EvalLightsAndSetConstants()
{
	HRESULT hr;

	if( m_bSHPRTTooManyConstants || 
		!m_bSHPRTReadyToRender || 
		m_aClusterBases == NULL || 
		m_pMeshPRT == NULL || 
		m_pSHPRTEffect == NULL )
		return S_OK;

	D3DXCOLOR lightColor(1.0f, 1.0f, 1.0f, 1.0f);        
	lightColor *= m_fLightScale;

	float fLight[MAX_LIGHTS][3][MAX_SH_ORDER*MAX_SH_ORDER];  

	// Pass in the light direction, the intensity of each channel, and it returns
	// the source radiance as an array of order^2 SH coefficients for each color channel.  
	D3DXVECTOR3 lightDirObjectSpace;
	D3DXMATRIX mWorldInv;
	D3DXMATRIX m_mWorld;
	g_pd3dDevice->GetTransform(D3DTS_WORLD,&m_mWorld);
	D3DXMatrixInverse(&mWorldInv, NULL, &m_mWorld);

	int i;
	for( i=0; i<m_nNumActiveLights; i++ )
	{
		// Transform the world space light dir into object space
		// Note that if there's multiple objects using PRT in the scene, then
		// for each object you need to either evaulate the lights in object space
		// evaulate the lights in world and rotate the light coefficients 
		// into object space.
		D3DXVec3TransformNormal( &lightDirObjectSpace, &m_vLightDir[i], &mWorldInv );

		// This sample uses D3DXSHEvalDirectionalLight(), but there's other 
		// types of lights provided by D3DXSHEval*.  Pass in the 
		// order of SH, color of the light, and the direction of the light 
		// in object space.
		// The output is the source radiance coefficients for the SH basis functions.  
		// There are 3 outputs, one for each channel (R,G,B). 
		// Each output is an array of m_dwOrder^2 floats.  
		hr = D3DXSHEvalDirectionalLight( m_dwOrder, &lightDirObjectSpace, 
			lightColor.r, lightColor.g, lightColor.b,
			fLight[i][0], fLight[i][1], fLight[i][2] );
		if( FAILED(hr) )
			return DXTRACE_ERR( TEXT("D3DXSHEvalDirectionalLight"), hr );
	}

	// For multiple lights, just them sum up using D3DXSHAdd().
	for( i=1; i<m_nNumActiveLights; i++ )
	{
		// D3DXSHAdd will add Order^2 floats.  There are 3 color channels, 
		// so call it 3 times.
		D3DXSHAdd( fLight[0][0], m_dwOrder, fLight[0][0], fLight[i][0] );
		D3DXSHAdd( fLight[0][1], m_dwOrder, fLight[0][1], fLight[i][1] );
		D3DXSHAdd( fLight[0][2], m_dwOrder, fLight[0][2], fLight[i][2] );
	}

	// Setup the shader constants based on the source radiance coefficients and
	// the cluster basis.
	if( FAILED( hr = PRT_SetShaderConstants( fLight[0][0], fLight[0][1], fLight[0][2] ) ) )
		return hr;

	return S_OK;
}




//-----------------------------------------------------------------------------
// Name: SetShaderConstants()
// Desc: Setup the shader constants based on the source radiance 
//       coefficients and the cluster basis.
//-----------------------------------------------------------------------------
HRESULT KModel::PRT_SetShaderConstants( float* pfRLC, float* pfGLC, float* pfBLC )
{
	HRESULT hr;

	DWORD dwNumChannels = (m_bSpectral) ? 3 : 1;
	int nChannelSize    = m_dwOrder * m_dwOrder; 
	int nPCAVectorSize = nChannelSize * dwNumChannels;  
	int nBasisSize     = nPCAVectorSize * (m_dwNumPCAVectors+1);  
	int nBufferSize     = m_dwNumClusters * (4+MAX_NUM_CHANNELS*m_dwNumPCAVectors);
	int nChannelMult    = (m_bSpectral?1:0) * nChannelSize;

	// Compute an array of floats to pass as constants to the vertex shader.
	// This array is the source radiance dotted with the transfer function.
	// The source radiance is the lighting environment in terms of spherical
	// harmonic coefficients which comes from D3DXSHEvalDirectionalLight, and 
	// the transfer function are in terms of spherical harmonic basis coefficients 
	// that comes from the D3DXSHPRTCompExtractBasis().
	for( DWORD iCluster = 0; iCluster < m_dwNumClusters; iCluster++ ) 
	{
		// First we compute L dot Mk, where Mk is the mean of cluster k.
		m_aClusteredPCA[iCluster*(4+MAX_NUM_CHANNELS*m_dwNumPCAVectors)+0] = D3DXSHDot( m_dwOrder, &m_aClusterBases[iCluster*nBasisSize + 0*nChannelMult], pfRLC );
		m_aClusteredPCA[iCluster*(4+MAX_NUM_CHANNELS*m_dwNumPCAVectors)+1] = D3DXSHDot( m_dwOrder, &m_aClusterBases[iCluster*nBasisSize + 1*nChannelMult], pfGLC );
		m_aClusteredPCA[iCluster*(4+MAX_NUM_CHANNELS*m_dwNumPCAVectors)+2] = D3DXSHDot( m_dwOrder, &m_aClusterBases[iCluster*nBasisSize + 2*nChannelMult], pfBLC );
		m_aClusteredPCA[iCluster*(4+MAX_NUM_CHANNELS*m_dwNumPCAVectors)+3] = 0.0f;

		// Then we compute L dot Bkj, where Bkj is the 
		// jth PCA basis vector for cluster k
		float* pPCAStart = &m_aClusteredPCA[iCluster*(4+MAX_NUM_CHANNELS*m_dwNumPCAVectors)+4];
		for( DWORD iPCA = 0; iPCA < m_dwNumPCAVectors; iPCA++ ) 
		{
			int nOffset = iCluster*nBasisSize + (iPCA+1)*nPCAVectorSize;

			pPCAStart[0*m_dwNumPCAVectors + iPCA] = D3DXSHDot( m_dwOrder, &m_aClusterBases[nOffset + 0*nChannelMult], pfRLC );
			pPCAStart[1*m_dwNumPCAVectors + iPCA] = D3DXSHDot( m_dwOrder, &m_aClusterBases[nOffset + 1*nChannelMult], pfGLC );
			pPCAStart[2*m_dwNumPCAVectors + iPCA] = D3DXSHDot( m_dwOrder, &m_aClusterBases[nOffset + 2*nChannelMult], pfBLC );
		}
	}

	hr = m_pSHPRTEffect->SetFloatArray( "vClusteredPCA", (float*)m_aClusteredPCA, nBufferSize );
	if( FAILED( hr ) )
		return DXTRACE_ERR( TEXT("SetFloatArray"), hr );

	return S_OK;
}

-----------------------------------------------------------------------------
Name: SetupVBWithPCAData()
Desc: Setup the vertex buffer with PCA data
	  -----------------------------------------------------------------------------
	  HRESULT KModel::SetupVBWithPCAData( ID3DXMesh* pMesh,
	  ID3DXBuffer* pCPCABuffer, 
	  D3DXSHPRTBUFFER_DESC desc,
	  DWORD dwNumClusters, 
	  DWORD dwNumPCAVectors )
{
	HRESULT hr;

	// Each vertex is now assigned to a cluster.  Call D3DXSHPRTCompExtractClusterIDs()
	// to get the cluster ID for every vertex.  Simply pass in an array of UINTs
	// that is the size of the number of vertices (which equals the number of samples), and 
	// the cluster ID for vertex N will be at puClusterIDs[N].
	UINT* puClusterIDs = new UINT[ desc.NumSamples ];
	if( puClusterIDs == NULL )
		return DXTRACE_ERR( TEXT("new"), E_OUTOFMEMORY );

	hr = D3DXSHPRTCompExtractClusterIDs( pCPCABuffer, puClusterIDs );
	if( FAILED(hr) )
		return DXTRACE_ERR( TEXT("D3DXSHPRTCompExtractClusterIDs"), hr );

	// Now use the cluster ID info to store a value in the mesh in the 
	// D3DDECLUSAGE_BLENDWEIGHT[0].  This value will be passed into the 
	// vertex shader to allow the shader use this number as an offset 
	// into a large array.  The value is based on the cluster ID and the
	// stride of the array.  
	BYTE* pV = NULL;
	hr = pMesh->LockVertexBuffer( 0, (void**) &pV );
	if( FAILED(hr) )
		return DXTRACE_ERR( TEXT("LockVertexBuffer"), hr );

	UINT uStride = pMesh->GetNumBytesPerVertex();
	BYTE* pClusterID = pV + 32; // 32 == D3DDECLUSAGE_BLENDWEIGHT[0] offset
	for( UINT uVert = 0; uVert < desc.NumSamples; uVert++ ) 
	{
		// The array in vertex shader is an array of float4.
		// For each cluster, there are (1+3*(NUM_PCA_VECTORS/4)) float4's.
		// We store a number that lets the shader index the constant table to get 
		// to the correct cluster for this vertex.
		float fArrayOffset = (float)(puClusterIDs[uVert] * (1+3*(m_dwNumPCAVectors/4))); 
		memcpy(pClusterID, &fArrayOffset, sizeof(float));
		pClusterID += uStride;
	}

	pMesh->UnlockVertexBuffer();
	SAFE_DELETE_ARRAY(puClusterIDs);

	// Store the PCA weights in the mesh starting at D3DDECLUSAGE_BLENDWEIGHT[1].  
	// D3DDECLUSAGE_BLENDWEIGHT[1-6] are float4.  So if dwNumPCAVectors == 16, then 
	// this function will write data to D3DDECLUSAGE_BLENDWEIGHT[1] 
	// through D3DDECLUSAGE_BLENDWEIGHT[4].
	hr = D3DXSHPRTCompExtractToMesh( dwNumPCAVectors, pMesh, pCPCABuffer, 
		D3DDECLUSAGE_BLENDWEIGHT, 1 );
	if( FAILED(hr) )
		return DXTRACE_ERR( TEXT("D3DXSHPRTCompExtractToMesh"), hr );

	return S_OK;
}
#endif
//////////////////////////////////////////////////////////////////////////
//For 9.0b Ends
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
//For 9.0c Here
UINT KModel::PRT_GetOrderFromNumCoeffs( UINT dwNumCoeffs )
{
	UINT dwOrder=1; 
	while(dwOrder*dwOrder < dwNumCoeffs) 
		dwOrder++;
	return dwOrder;
}

void KModel::PRT_RenderWithSHIrradEnvMap( bool bRenderWithAlbedo )
{
	UINT iPass, cPasses;

	D3DXMATRIX mWorldViewProj;
	D3DXMATRIX mWorld;
	D3DXMATRIX mView;
	D3DXMATRIX mProj;

	g_pd3dDevice->GetTransform(D3DTS_WORLD, &mWorld);
	g_pd3dDevice->GetTransform(D3DTS_VIEW, &mView);
	g_pd3dDevice->GetTransform(D3DTS_PROJECTION, &mProj);
	D3DXMatrixMultiply(&mWorldViewProj, &mWorld, &mView);
	D3DXMatrixMultiply(&mWorldViewProj, &mWorldViewProj, &mProj);

	m_pSHIrradEnvMapEffect->SetMatrix( "g_mWorldViewProjection", &mWorldViewProj );

	bool bHasAlbedoTexture = false;
	for(DWORD i=0; i<m_pAlbedoTextures.size(); i++ )
	{
		if( m_pAlbedoTextures[i] != NULL )
			bHasAlbedoTexture = true;
	}
	if( !bHasAlbedoTexture )
		bRenderWithAlbedo = false;

	if( bRenderWithAlbedo )
	{
		( m_pSHIrradEnvMapEffect->SetTechnique( "RenderWithSHIrradEnvMap" ) );
	}
	else
	{
		( m_pSHIrradEnvMapEffect->SetTechnique( "RenderWithSHIrradEnvMapNoAlbedo" ) );
	}

	if( !bRenderWithAlbedo )
	{
		D3DXCOLOR clrWhite = D3DXCOLOR(1,1,1,1);
		( m_pSHIrradEnvMapEffect->SetValue("MaterialDiffuseColor", &clrWhite, sizeof(D3DCOLORVALUE) ) );
	}

	( m_pSHIrradEnvMapEffect->Begin(&cPasses, 0) );

	for (iPass = 0; iPass < cPasses; iPass++)
	{
		( m_pSHIrradEnvMapEffect->BeginPass(iPass) );

		DWORD dwAttribs = 0;
		( m_pMeshPRT->GetAttributeTable( NULL, &dwAttribs ) );
		for( DWORD i=0; i<dwAttribs; i++ )
		{
			if( bRenderWithAlbedo )
			{
				if( m_pAlbedoTextures.size() > (int) i )
					( m_pSHIrradEnvMapEffect->SetTexture( "AlbedoTexture", m_pAlbedoTextures[i] ) );
				( m_pSHIrradEnvMapEffect->SetValue("MaterialDiffuseColor", &m_lpMaterial[i].m_sMaterial9.Diffuse, sizeof(D3DCOLORVALUE) ) );
				( m_pSHIrradEnvMapEffect->CommitChanges() );
			}
			( m_pMeshPRT->DrawSubset(i) );
		}

		( m_pSHIrradEnvMapEffect->EndPass() );
	}

	( m_pSHIrradEnvMapEffect->End() );
}

void KModel::PRT_RenderWithNdotL( bool bRenderWithAlbedo, int nNumLights, float fLightScale )
{
	UINT iPass, cPasses;

	D3DXMATRIX mWorldViewProj;
	D3DXMATRIX mWorld;
	D3DXMATRIX mView;
	D3DXMATRIX mProj;

	g_pd3dDevice->GetTransform(D3DTS_WORLD, &mWorld);
	g_pd3dDevice->GetTransform(D3DTS_VIEW, &mView);
	g_pd3dDevice->GetTransform(D3DTS_PROJECTION, &mProj);
	D3DXMatrixMultiply(&mWorldViewProj, &mWorld, &mView);
	D3DXMatrixMultiply(&mWorldViewProj, &mWorldViewProj, &mProj);
	D3DXMatrixInverse(&mWorld, NULL, &mWorld);

	m_pNDotLEffect->SetMatrix( "g_mWorldViewProjection", &mWorldViewProj );
	m_pNDotLEffect->SetMatrix( "g_mWorldInv", &mWorld );

	D3DXVECTOR4 vLightDir[10];
	D3DXVECTOR4 vLightsDiffuse[10];
	D3DXVECTOR4 lightOn(1,1,1,1);
	D3DXVECTOR4 lightOff(0,0,0,0);
	lightOn *= fLightScale;

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//Light Dir should be got from else where
	//Set to const




	for( int i=0; i<nNumLights; i++ )
		vLightDir[i] = D3DXVECTOR4( float(rand())/RAND_MAX, float(rand())/RAND_MAX, float(rand())/RAND_MAX,0 );
	for( int i=0; i<10; i++ )
		vLightsDiffuse[i] = (nNumLights > i) ? lightOn : lightOff;
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	bool bHasAlbedoTexture = false;
	for(DWORD i=0; i<m_pAlbedoTextures.size(); i++ )
	{
		if( m_pAlbedoTextures[i] != NULL )
			bHasAlbedoTexture = true;
	}
	if( !bHasAlbedoTexture )
		bRenderWithAlbedo = false;

	if( bRenderWithAlbedo )
	{
		( m_pNDotLEffect->SetTechnique( "RenderWithNdotL" ) );
	}
	else
	{
		( m_pNDotLEffect->SetTechnique( "RenderWithNdotLNoAlbedo" ) );
	}

	if( !bRenderWithAlbedo )
	{
		D3DXCOLOR clrWhite = D3DXCOLOR(1,1,1,1);
		( m_pNDotLEffect->SetValue("MaterialDiffuseColor", &clrWhite, sizeof(D3DCOLORVALUE) ) );
	}

	( m_pNDotLEffect->Begin(&cPasses, 0) );

	for (iPass = 0; iPass < cPasses; iPass++)
	{
		( m_pNDotLEffect->BeginPass(iPass) );

		// 10 and 20 are the register constants
		( g_pd3dDevice->SetVertexShaderConstantF( 10, (float*)vLightDir, nNumLights ) );
		( g_pd3dDevice->SetVertexShaderConstantF( 20, (float*)vLightsDiffuse, 10 ) );

		DWORD dwAttribs = 0;
		( m_pMeshPRT->GetAttributeTable( NULL, &dwAttribs ) );
		for( DWORD i=0; i<dwAttribs; i++ )
		{
			if( bRenderWithAlbedo )
			{
				if( m_pAlbedoTextures.size() > (int) i )
					( m_pNDotLEffect->SetTexture( "AlbedoTexture", m_pAlbedoTextures[i] ) );
				( m_pNDotLEffect->SetValue("MaterialDiffuseColor", &m_lpMaterial[i].m_sMaterial9.Diffuse, sizeof(D3DCOLORVALUE) ) );
				( m_pNDotLEffect->CommitChanges() );
			}
			( m_pMeshPRT->DrawSubset(i) );
		}

		( m_pNDotLEffect->EndPass() );
	}

	( m_pNDotLEffect->End() );
}

void KModel::PRT_RenderWithPRT( bool bRenderWithAlbedo )
{
	UINT iPass, cPasses;

	D3DXMATRIX mWorldViewProj;
	D3DXMATRIX mWorld;
	D3DXMATRIX mView;
	D3DXMATRIX mProj;

	g_pd3dDevice->GetTransform(D3DTS_WORLD, &mWorld);
	g_pd3dDevice->GetTransform(D3DTS_VIEW, &mView);
	g_pd3dDevice->GetTransform(D3DTS_PROJECTION, &mProj);
	D3DXMatrixMultiply(&mWorldViewProj, &mWorld, &mView);
	D3DXMatrixMultiply(&mWorldViewProj, &mWorldViewProj, &mProj);
	m_pPRTEffect->SetMatrix( "g_mWorldViewProjection", &mWorldViewProj );

	bool bHasAlbedoTexture = false;
	for(DWORD i=0; i<m_pAlbedoTextures.size(); i++ )
	{
		if( m_pAlbedoTextures[i] != NULL )
			bHasAlbedoTexture = true;
	}
	if( !bHasAlbedoTexture )
		bRenderWithAlbedo = false;

	if( bRenderWithAlbedo )
	{
		( m_pPRTEffect->SetTechnique( "RenderWithPRTColorLights" ) );
	}
	else
	{
		( m_pPRTEffect->SetTechnique( "RenderWithPRTColorLightsNoAlbedo" ) );
	}

	if( !bRenderWithAlbedo )
	{
		D3DXCOLOR clrWhite = D3DXCOLOR(1,1,1,1);
		( m_pPRTEffect->SetValue("MaterialDiffuseColor", &clrWhite, sizeof(D3DCOLORVALUE) ) );
	}

	( m_pPRTEffect->Begin(&cPasses, 0) );

	for (iPass = 0; iPass < cPasses; iPass++)
	{
		( m_pPRTEffect->BeginPass(iPass) );

		DWORD dwAttribs = 0;
		( m_pMeshPRT->GetAttributeTable( NULL, &dwAttribs ) );
		for( DWORD i=0; i<m_dNumMaterial; i++ )
		{            
			if( bRenderWithAlbedo )
			{
				if( m_pAlbedoTextures.size() > (int) i )
					m_pPRTEffect->SetTexture( "AlbedoTexture", m_pAlbedoTextures[i] );
				m_pPRTEffect->SetValue("MaterialDiffuseColor", &m_lpMaterial[i].m_sMaterial9.Diffuse, sizeof(D3DCOLORVALUE));
				m_pPRTEffect->CommitChanges();
			}
			m_pMeshPRT->DrawSubset(i);
		}

		m_pPRTEffect->EndPass();
	}

	m_pPRTEffect->End();
}
void KModel::PRT_ComputeShaderConstants( float* pSHCoeffsRed, float* pSHCoeffsGreen, float* pSHCoeffsBlue, DWORD dwNumCoeffsPerChannel )
{
	ASSERT( dwNumCoeffsPerChannel == m_pPRTCompBuffer->GetNumCoeffs() );

	UINT dwNumCoeffs   = m_pPRTCompBuffer->GetNumCoeffs();
	UINT dwOrder       = m_dwOrder;
	UINT dwNumChannels = m_pPRTCompBuffer->GetNumChannels();
	UINT dwNumClusters = m_pPRTCompBuffer->GetNumClusters();
	UINT dwNumPCA      = m_pPRTCompBuffer->GetNumPCA();

	//
	// With compressed PRT, a single diffuse channel is caluated by:
	//       R[p] = (M[k] dot L') + sum( w[p][j] * (B[k][j] dot L');
	// where the sum runs j between 0 and # of PCA vectors
	//       R[p] = exit radiance at point p
	//       M[k] = mean of cluster k 
	//       L' = source radiance approximated with SH coefficients
	//       w[p][j] = the j'th PCA weight for point p
	//       B[k][j] = the j'th PCA basis vector for cluster k
	//
	// Note: since both (M[k] dot L') and (B[k][j] dot L') can be computed on the CPU, 
	// these values are passed as constants using the array m_aPRTConstants.   
	// 
	// So we compute an array of floats, m_aPRTConstants, here.
	// This array is the L' dot M[k] and L' dot B[k][j].
	// The source radiance is the lighting environment in terms of spherical
	// harmonic coefficients which can be computed with D3DXSHEval* or D3DXSHProjectCubeMap.  
	// M[k] and B[k][j] are also in terms of spherical harmonic basis coefficients 
	// and come from ID3DXPRTCompBuffer::ExtractBasis().
	//
	DWORD dwClusterStride = dwNumChannels*dwNumPCA + 4;
	DWORD dwBasisStride = dwNumCoeffs*dwNumChannels*(dwNumPCA + 1);  

	for( DWORD iCluster = 0; iCluster < dwNumClusters; iCluster++ ) 
	{
		// For each cluster, store L' dot M[k] per channel, where M[k] is the mean of cluster k
		m_aPRTConstants[iCluster*dwClusterStride + 0] = D3DXSHDot( dwOrder, &m_aClusterBases[iCluster*dwBasisStride + 0*dwNumCoeffs], pSHCoeffsRed );
		m_aPRTConstants[iCluster*dwClusterStride + 1] = D3DXSHDot( dwOrder, &m_aClusterBases[iCluster*dwBasisStride + 1*dwNumCoeffs], pSHCoeffsGreen );
		m_aPRTConstants[iCluster*dwClusterStride + 2] = D3DXSHDot( dwOrder, &m_aClusterBases[iCluster*dwBasisStride + 2*dwNumCoeffs], pSHCoeffsBlue );
		m_aPRTConstants[iCluster*dwClusterStride + 3] = 0.0f;

		// Then per channel we compute L' dot B[k][j], where B[k][j] is the jth PCA basis vector for cluster k
		float* pPCAStart = &m_aPRTConstants[iCluster*dwClusterStride + 4];
		for( DWORD iPCA = 0; iPCA < dwNumPCA; iPCA++ ) 
		{
			int nOffset = iCluster*dwBasisStride + (iPCA+1)*dwNumCoeffs*dwNumChannels;

			pPCAStart[0*dwNumPCA + iPCA] = D3DXSHDot( dwOrder, &m_aClusterBases[nOffset + 0*dwNumCoeffs], pSHCoeffsRed );
			pPCAStart[1*dwNumPCA + iPCA] = D3DXSHDot( dwOrder, &m_aClusterBases[nOffset + 1*dwNumCoeffs], pSHCoeffsGreen );
			pPCAStart[2*dwNumPCA + iPCA] = D3DXSHDot( dwOrder, &m_aClusterBases[nOffset + 2*dwNumCoeffs], pSHCoeffsBlue );
		}
	}

	m_pPRTEffect->SetFloatArray( "aPRTConstants", (float*)m_aPRTConstants, dwNumClusters*(4+dwNumChannels*dwNumPCA));
}

void KModel::PRT_ComputeSHIrradEnvMapConstants( float* pSHCoeffsRed, float* pSHCoeffsGreen, float* pSHCoeffsBlue )
{ 
	float* fLight[3] = { pSHCoeffsRed, pSHCoeffsGreen, pSHCoeffsBlue };

	// Lighting environment coefficients
	D3DXVECTOR4 vCoefficients[3];

	// These constants are described in the article by Peter-Pike Sloan titled 
	// "Efficient Evaluation of Irradiance Environment Maps" in the book 
	// "ShaderX 2 - Shader Programming Tips and Tricks" by Wolfgang F. Engel.
	static const float s_fSqrtPI = ((float)sqrtf(D3DX_PI));
	const float fC0 = 1.0f/(2.0f*s_fSqrtPI);
	const float fC1 = (float)sqrt(3.0f)/(3.0f*s_fSqrtPI);
	const float fC2 = (float)sqrt(15.0f)/(8.0f*s_fSqrtPI);
	const float fC3 = (float)sqrt(5.0f)/(16.0f*s_fSqrtPI);
	const float fC4 = 0.5f*fC2;

	for( int iChannel=0; iChannel<3; iChannel++ )
	{
		vCoefficients[iChannel].x = -fC1*fLight[iChannel][3];
		vCoefficients[iChannel].y = -fC1*fLight[iChannel][1];
		vCoefficients[iChannel].z =  fC1*fLight[iChannel][2];
		vCoefficients[iChannel].w =  fC0*fLight[iChannel][0] - fC3*fLight[iChannel][6];
	}

	( m_pSHIrradEnvMapEffect->SetVector( "cAr", &vCoefficients[0] ) );
	( m_pSHIrradEnvMapEffect->SetVector( "cAg", &vCoefficients[1] ) );
	( m_pSHIrradEnvMapEffect->SetVector( "cAb", &vCoefficients[2] ) );

	for( iChannel=0; iChannel<3; iChannel++ )
	{
		vCoefficients[iChannel].x =      fC2*fLight[iChannel][4];
		vCoefficients[iChannel].y =     -fC2*fLight[iChannel][5];
		vCoefficients[iChannel].z = 3.0f*fC3*fLight[iChannel][6];
		vCoefficients[iChannel].w =     -fC2*fLight[iChannel][7];
	}

	( m_pSHIrradEnvMapEffect->SetVector( "cBr", &vCoefficients[0] ) );
	( m_pSHIrradEnvMapEffect->SetVector( "cBg", &vCoefficients[1] ) );
	( m_pSHIrradEnvMapEffect->SetVector( "cBb", &vCoefficients[2] ) );

	vCoefficients[0].x = fC4*fLight[0][8];
	vCoefficients[0].y = fC4*fLight[1][8];
	vCoefficients[0].z = fC4*fLight[2][8];
	vCoefficients[0].w = 1.0f;

	( m_pSHIrradEnvMapEffect->SetVector( "cC", &vCoefficients[0] ) );
}

HRESULT KModel::PRT_ExtractCompressedDataForPRTShader() 
{ 
	HRESULT hr = S_OK;
	m_bSHPRTReadyToRender = false;
	// First call ID3DXPRTCompBuffer::NormalizeData.  This isn't nessacary, 
	// but it makes it easier to use data formats that have little presision.
	// It normalizes the PCA weights so that they are between [-1,1]
	// and modifies the basis vectors accordingly.  
	hr = m_pPRTCompBuffer->NormalizeData();

	UINT dwNumSamples  = m_pPRTCompBuffer->GetNumSamples();
	UINT dwNumCoeffs   = m_pPRTCompBuffer->GetNumCoeffs();
	UINT dwNumChannels = m_pPRTCompBuffer->GetNumChannels();
	UINT dwNumClusters = m_pPRTCompBuffer->GetNumClusters();
	UINT dwNumPCA      = m_pPRTCompBuffer->GetNumPCA();

	// With clustered PCA, each vertex is assigned to a cluster.  To figure out 
	// which vertex goes with which cluster, call ID3DXPRTCompBuffer::ExtractClusterIDs.
	// This will return a cluster ID for every vertex.  Simply pass in an array of UINTs
	// that is the size of the number of vertices (which also equals the number of samples), and 
	// the cluster ID for vertex N will be at puClusterIDs[N].
	UINT* pClusterIDs = new UINT[ dwNumSamples ];
	ASSERT( pClusterIDs );
	m_pPRTCompBuffer->ExtractClusterIDs( pClusterIDs );

	D3DVERTEXELEMENT9 declCur[MAX_FVF_DECL_SIZE];
	m_pMeshPRT->GetDeclaration( declCur );

	// Now use this cluster ID info to store a value in the mesh in the 
	// D3DDECLUSAGE_BLENDWEIGHT[0] which is declared in the vertex decl to be a float1
	// This value will be passed into the vertex shader to allow the shader 
	// use this number as an offset into an array of shader constants.  
	// The value we set per vertex is based on the cluster ID and the stride 
	// of the shader constants array.  

	BYTE* pV = NULL;
	m_pMeshPRT->LockVertexBuffer( 0, (void**) &pV );
	UINT uStride = m_pMeshPRT->GetNumBytesPerVertex();
	BYTE* pClusterID = pV + 32; // 32 == D3DDECLUSAGE_BLENDWEIGHT[0] offset
	for( UINT uVert = 0; uVert < dwNumSamples; uVert++ ) 
	{
		float fArrayOffset = (float)(pClusterIDs[uVert] * (1+3*(dwNumPCA/4))); 
		memcpy(pClusterID, &fArrayOffset, sizeof(float));
		pClusterID += uStride;
	}
	m_pMeshPRT->UnlockVertexBuffer();
	SAFE_DELETE_ARRAY(pClusterIDs);

	
	// Now we also need to store the per vertex PCA weights.  Earilier when
	// the mesh was loaded, we changed the vertex decl to make room to store these
	// PCA weights.  In this sample, we will use D3DDECLUSAGE_BLENDWEIGHT[1] to 
	// D3DDECLUSAGE_BLENDWEIGHT[6].  Using D3DDECLUSAGE_BLENDWEIGHT intead of some other 
	// usage was an arbritatey decision.  Since D3DDECLUSAGE_BLENDWEIGHT[1-6] were 
	// declared as float4 then we can store up to 6*4 PCA weights per vertex.  They don't
	// have to be declared as float4, but its a reasonable choice.  So for example, 
	// if dwNumPCAVectors=16 the function will write data to D3DDECLUSAGE_BLENDWEIGHT[1-4]
	hr =  m_pPRTCompBuffer->ExtractToMesh( dwNumPCA, D3DDECLUSAGE_BLENDWEIGHT, 1, m_pMeshPRT );
	if ( FAILED(hr))
		return hr;
	// Extract the cluster bases into a large array of floats.  
	// ID3DXPRTCompBuffer::ExtractBasis will extract the basis 
	// for a single cluster.  
	//
	// A single cluster basis is an array of
	// (NumPCA+1)*NumCoeffs*NumChannels floats
	// The "1+" is for the cluster mean.
	int nClusterBasisSize = (dwNumPCA+1) * dwNumCoeffs * dwNumChannels;  
	int nBufferSize       = nClusterBasisSize * dwNumClusters; 

	SAFE_DELETE_ARRAY( m_aClusterBases );
	m_aClusterBases = new float[nBufferSize];
	ASSERT( m_aClusterBases );

	for( DWORD iCluster = 0; iCluster < dwNumClusters; iCluster++ ) 
	{
		// ID3DXPRTCompBuffer::ExtractBasis() extracts the basis for a single cluster at a time.
		hr =  m_pPRTCompBuffer->ExtractBasis( iCluster, &m_aClusterBases[iCluster * nClusterBasisSize] );
	}

	SAFE_DELETE_ARRAY( m_aPRTConstants );
    m_aPRTConstants = new float[dwNumClusters*(4+dwNumChannels*dwNumPCA)];
	ASSERT( m_aPRTConstants );
	
	m_bSHPRTReadyToRender = true;
	return hr;
}


void KModel::PRT_SetPRTBuffer( ID3DXPRTBuffer* pPRTBuffer, TCHAR* strFile ) 
{ 
	SAFE_RELEASE(m_pPRTBuffer);
	SAFE_RELEASE(m_pPRTCompBuffer);
	m_pPRTBuffer = pPRTBuffer;
	m_dwOrder = PRT_GetOrderFromNumCoeffs( m_pPRTBuffer->GetNumCoeffs() );
	strncpy( m_PRTReloadState.strPRTBufferFileName, strFile, MAX_PATH );
	m_PRTReloadState.strPRTBufferFileName[MAX_PATH - 1] = L'\0';
}

void KModel::PRT_CompressBuffer( D3DXSHCOMPRESSQUALITYTYPE Quality, UINT NumClusters, UINT NumPCA )
{ 
	ASSERT( m_pPRTBuffer != NULL );
	SAFE_RELEASE(m_pPRTCompBuffer);
	D3DXCreatePRTCompBuffer( Quality, NumClusters, NumPCA, m_pPRTBuffer, &m_pPRTCompBuffer );
	m_PRTReloadState.quality = Quality;
	m_PRTReloadState.dwNumClusters = NumClusters;
	m_PRTReloadState.dwNumPCA = NumPCA;
	m_PRTReloadState.bUseReloadState = true;
	m_PRTReloadState.bLoadCompressed = false;
	m_dwOrder = PRT_GetOrderFromNumCoeffs( m_pPRTBuffer->GetNumCoeffs() );
}

HRESULT KModel::PRT_LoadCompPRTBufferFromFile( TCHAR* strFile )
{
	SAFE_RELEASE( m_pPRTBuffer );
	SAFE_RELEASE( m_pPRTCompBuffer );

	TCHAR str[MAX_PATH];
	if ( !strcmp(strFile, ""))
		return S_OK;
	g_cFileFinder.FindFile(str, strFile);
	if ( !strcmp(str, ""))
		MessageBox(NULL, "PRT File Not Found.", "Error", MB_OK);
	strcpy( m_PRTReloadState.strPRTBufferFileName, str );
	D3DXLoadPRTCompBufferFromFile( str, &m_pPRTCompBuffer );
	m_PRTReloadState.bUseReloadState = true;
	m_PRTReloadState.bLoadCompressed = true;
	m_dwOrder = PRT_GetOrderFromNumCoeffs( m_pPRTCompBuffer->GetNumCoeffs() );
	return S_OK;
}

HRESULT KModel::PRT_LoadPRTBufferFromFile( TCHAR* strFile )
{
	HRESULT hr = S_OK;
	SAFE_RELEASE( m_pPRTBuffer );
	SAFE_RELEASE( m_pPRTCompBuffer );

	TCHAR str[MAX_PATH];
	g_cFileFinder.FindFile( str, strFile );
	if ( !strcmp(str, "") )
		MessageBox(NULL, "PRT File Not Found !", "Error", MB_OK);
	strcpy( m_PRTReloadState.strPRTBufferFileName, str );
	hr = D3DXLoadPRTBufferFromFile( str, &m_pPRTBuffer );
	if ( FAILED(hr))
	{
		return E_FAIL;
	}
	m_dwOrder = PRT_GetOrderFromNumCoeffs( m_pPRTBuffer->GetNumCoeffs() );
	return S_OK;
}

HRESULT KModel::PRT_AdjustMeshDecl()
{
	HRESULT hr = S_OK;
	D3DVERTEXELEMENT9 decl[MAX_FVF_DECL_SIZE] = 
	{
		{0,  0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0,  12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
		{0,  24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		{0,  32, D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 0},
		{0,  36, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 1},
		{0,  52, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 2},
		{0,  68, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 3},
		{0,  84, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 4},
		{0, 100, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 5},
		{0, 116, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 6},
		D3DDECL_END()
	};

	// To do CPCA, we need to store (g_dwNumPCAVectors + 1) scalers per vertex, so 
	// make the mesh have a known decl to store this data.  Since we can't do 
	// skinning and PRT at once, we use D3DDECLUSAGE_BLENDWEIGHT[0] 
	// to D3DDECLUSAGE_BLENDWEIGHT[6] to store our per vertex data needed for PRT.
	// Notice that D3DDECLUSAGE_BLENDWEIGHT[0] is a float1, and
	// D3DDECLUSAGE_BLENDWEIGHT[1]-D3DDECLUSAGE_BLENDWEIGHT[6] are float4.  This allows 
	// up to 24 PCA weights and 1 float that gives the vertex shader 
	// an index into the vertex's cluster's data
	SAFE_RELEASE(m_pMeshPRT);
	hr = m_pMeshSys->CloneMesh( m_pMeshSys->GetOptions(), decl, g_pd3dDevice, &m_pMeshPRT);
    DWORD* rgdwAdjacency = new DWORD[m_pMeshPRT->GetNumFaces() * 3];
	m_pMeshPRT->ConvertPointRepsToAdjacency(NULL, rgdwAdjacency);
	hr = m_pMeshPRT->OptimizeInplace(D3DXMESHOPT_VERTEXCACHE | D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_IGNOREVERTS, 
		rgdwAdjacency, NULL, NULL, NULL);
	SAFE_DELETE_ARRAY(rgdwAdjacency);
		// Make sure there are normals which are required for lighting
	//if( !(m_pMeshSys->GetFVF() & D3DFVF_NORMAL) )
	//hr = D3DXComputeNormals( m_pMeshPRT, NULL );
	return hr;
}

HRESULT KModel::PRT_Initialize()
{
	HRESULT hr = S_OK;
	m_bSHPRTReadyToRender = false;
	hr = PRT_AdjustMeshDecl();
	if ( FAILED(hr))
		return E_FAIL;
	if( m_PRTReloadState.bUseReloadState )
	{
		if( m_PRTReloadState.bLoadCompressed )
		{
			hr = PRT_LoadCompPRTBufferFromFile( m_PRTReloadState.strPRTBufferFileName );
			if ( FAILED(hr))
				return E_FAIL;
		}
		else
		{
			m_PRTReloadState.quality = D3DXSHCQUAL_FASTLOWQUALITY;
			m_PRTReloadState.dwNumClusters = 1;
			m_PRTReloadState.dwNumPCA = 24;
			hr = PRT_LoadPRTBufferFromFile( m_PRTReloadState.strPRTBufferFileName );
			if ( FAILED(hr))
				return E_FAIL;
			PRT_CompressBuffer( m_PRTReloadState.quality, m_PRTReloadState.dwNumClusters, m_PRTReloadState.dwNumPCA );
		}
		PRT_ExtractCompressedDataForPRTShader();
		hr = PRT_LoadEffects();
		if ( FAILED(hr))
			return E_FAIL;
		m_bSHPRTReadyToRender = true;
	}
	return S_OK;
}

HRESULT KModel::PRT_GetAlbedoTextures()
{
	if ( m_pAlbedoTextures.size() )
		m_pAlbedoTextures.clear();
	for (DWORD dwNumSubset = 0; dwNumSubset < m_dNumMaterial; dwNumSubset++ )
	{
		LPTEXTURE pTexture = NULL;
		IDirect3DTexture9* pAlbedoTexture = NULL;
		if(SUCCEEDED(g_cTextureTable.GetTexture(&pTexture,m_lpMaterial[dwNumSubset].m_dTextureIDs[0])))
		{
			pAlbedoTexture = pTexture->m_lpTexture;
		}
		m_pAlbedoTextures.push_back(pAlbedoTexture);
	}

	return S_OK;
}
HRESULT KModel::PRT_LoadEffects()
{
	HRESULT hr;

	UINT dwNumChannels = m_pPRTCompBuffer->GetNumChannels();
	UINT dwNumClusters = m_pPRTCompBuffer->GetNumClusters();
	UINT dwNumPCA      = m_pPRTCompBuffer->GetNumPCA();

	// The number of vertex consts need by the shader can't exceed the 
	// amount the HW can support

	DWORD dwNumVConsts = dwNumClusters * (1 + dwNumChannels*dwNumPCA/4) + 4;
	if( dwNumVConsts > g_D3DCaps.MaxVertexShaderConst )
		return E_FAIL;

	SAFE_RELEASE( m_pPRTEffect );
	SAFE_RELEASE( m_pSHIrradEnvMapEffect );
	SAFE_RELEASE( m_pNDotLEffect );

	D3DXMACRO aDefines[3];
	char szMaxNumClusters[64];
	sprintf( szMaxNumClusters, "%d", dwNumClusters );
	szMaxNumClusters[63] = 0;
	char szMaxNumPCA[64];
	sprintf( szMaxNumPCA, "%d", dwNumPCA );
	szMaxNumPCA[63] = 0;
	aDefines[0].Name       = "NUM_CLUSTERS";
	aDefines[0].Definition = szMaxNumClusters;
	aDefines[1].Name       = "NUM_PCA";
	aDefines[1].Definition = szMaxNumPCA;
	aDefines[2].Name       = NULL;
	aDefines[2].Definition = NULL;

	// Define DEBUG_VS and/or DEBUG_PS to debug vertex and/or pixel shaders with the shader debugger.  
	// Debugging vertex shaders requires either REF or software vertex processing, and debugging 
	// pixel shaders requires REF.  The D3DXSHADER_FORCE_*_SOFTWARE_NOOPT flag improves the debug 
	// experience in the shader debugger.  It enables source level debugging, prevents instruction 
	// reordering, prevents dead code elimination, and forces the compiler to compile against the next 
	// higher available software target, which ensures that the unoptimized shaders do not exceed 
	// the shader model limitations.  Setting these flags will cause slower rendering since the shaders 
	// will be unoptimized and forced into software.  See the DirectX documentation for more information 
	// about using the shader debugger.
	DWORD dwShaderFlags = 0;
#ifdef DEBUG_VS
	dwShaderFlags |= D3DXSHADER_FORCE_VS_SOFTWARE_NOOPT;
#endif
#ifdef DEBUG_PS
	dwShaderFlags |= D3DXSHADER_FORCE_PS_SOFTWARE_NOOPT;
#endif
	// Read the D3DX effect file
	
	TCHAR str[MAX_PATH];
	g_cFileFinder.FindFile(str, TEXT("SimpleLighting.fx") );
	( D3DXCreateEffectFromFile( g_pd3dDevice, str, NULL, NULL, 
		dwShaderFlags, NULL, &m_pNDotLEffect, NULL ) );

	g_cFileFinder.FindFile(str,TEXT("SHIrradianceEnvMap.fx") );
	( D3DXCreateEffectFromFile( g_pd3dDevice, str, NULL, NULL, 
		dwShaderFlags, NULL, &m_pSHIrradEnvMapEffect, NULL ) );
		
	g_cFileFinder.FindFile(str, "PRTColorLights.fx");
	// If this fails, there should be debug output as to 
	// they the .fx file failed to compile
	hr = D3DXCreateEffectFromFile( g_pd3dDevice, str, aDefines, NULL, 
		dwShaderFlags, NULL, &m_pPRTEffect, NULL );
	// Make sure the technique works on this card
	hr = m_pPRTEffect->ValidateTechnique( "RenderWithPRTColorLights" );
	if( FAILED( hr ) )
		return DXTRACE_ERR( TEXT("ValidateTechnique"), hr );

	
	return S_OK;
}

void KModel::PRT_UpdateLightingEnvironment()
{
	//float fEnv1Scaler = (g_RenderingUI.GetSlider( IDC_ENVIRONMENT_1_SCALER )->GetValue() / 100.0f);
	//float fEnv2Scaler = (g_RenderingUI.GetSlider( IDC_ENVIRONMENT_2_SCALER )->GetValue() / 100.0f);
	//float fEnvBlendScaler = (g_RenderingUI.GetSlider( IDC_ENVIRONMENT_BLEND_SCALER )->GetValue() / 100.0f);

	float fLight[MAX_LIGHTS][3][D3DXSH_MAXORDER*D3DXSH_MAXORDER];  

	D3DXCOLOR lightColor = m_PRTLightColor * m_fPRTLightScale;


	DWORD dwOrder = PRT_GetOrder();

	// Pass in the light direction, the intensity of each channel, and it returns
	// the source radiance as an array of order^2 SH coefficients for each color channel.  
	D3DXVECTOR3 lightDirObjectSpace;
	D3DXMATRIX mWorldInv;
	D3DXMATRIX mWorld;
	g_pd3dDevice->GetTransform(D3DTS_WORLD, &mWorld);
	D3DXMatrixInverse(&mWorldInv, NULL, &mWorld );

	int i;
	float fTime = (float)timeGetTime();

	D3DXVECTOR3 vLight[3] = { D3DXVECTOR3(sin(fTime/1000), 0, cos(fTime/1000)), 
		D3DXVECTOR3(sin(fTime/500), 0, cos(fTime/500)),
		D3DXVECTOR3(0, 1, 0)
	};
	for( i=0; i<m_nNumActiveLights; i++ )
	{
		// Transform the world space light dir into object space
		// Note that if there's multiple objects using PRT in the scene, then
		// for each object you need to either evaulate the lights in object space
		// evaulate the lights in world and rotate the light coefficients 
		// into object space.
		//D3DXVECTOR3 vLight = D3DXVECTOR3(sin(fTime/1000), 0, cos(fTime/1000));
		D3DXVec3TransformNormal( &lightDirObjectSpace, &vLight[i], &mWorldInv );

		// This sample uses D3DXSHEvalDirectionalLight(), but there's other 
		// types of lights provided by D3DXSHEval*.  Pass in the 
		// order of SH, color of the light, and the direction of the light 
		// in object space.
		// The output is the source radiance coefficients for the SH basis functions.  
		// There are 3 outputs, one for each channel (R,G,B). 
		// Each output is an array of m_dwOrder^2 floats.  
		D3DXSHEvalConeLight( dwOrder, &lightDirObjectSpace, m_fPRTConeRadius,
			lightColor.r, lightColor.g, lightColor.b,
			fLight[i][0], fLight[i][1], fLight[i][2] );
	}

	float fSum[3][D3DXSH_MAXORDER*D3DXSH_MAXORDER];  
	ZeroMemory( fSum, 3*D3DXSH_MAXORDER*D3DXSH_MAXORDER*sizeof(float) );

	// For multiple lights, just them sum up using D3DXSHAdd().
	for( i=0; i<m_nNumActiveLights; i++ )
	{
		// D3DXSHAdd will add Order^2 floats.  There are 3 color channels, 
		// so call it 3 times.
		D3DXSHAdd( fSum[0], dwOrder, fSum[0], fLight[i][0] );
		D3DXSHAdd( fSum[1], dwOrder, fSum[1], fLight[i][1] );
		D3DXSHAdd( fSum[2], dwOrder, fSum[2], fLight[i][2] );
	}

	/*float fSkybox1[3][D3DXSH_MAXORDER*D3DXSH_MAXORDER];  
	float fSkybox1Rot[3][D3DXSH_MAXORDER*D3DXSH_MAXORDER];  
	D3DXSHScale( fSkybox1[0], dwOrder, g_fSkyBoxLightSH[g_dwSkyBoxA][0], fEnv1Scaler*(1.0f-fEnvBlendScaler) );
	D3DXSHScale( fSkybox1[1], dwOrder, g_fSkyBoxLightSH[g_dwSkyBoxA][1], fEnv1Scaler*(1.0f-fEnvBlendScaler) );
	D3DXSHScale( fSkybox1[2], dwOrder, g_fSkyBoxLightSH[g_dwSkyBoxA][2], fEnv1Scaler*(1.0f-fEnvBlendScaler) );
	D3DXSHRotate( fSkybox1Rot[0], dwOrder, &mWorldInv, fSkybox1[0] );
	D3DXSHRotate( fSkybox1Rot[1], dwOrder, &mWorldInv, fSkybox1[1] );
	D3DXSHRotate( fSkybox1Rot[2], dwOrder, &mWorldInv, fSkybox1[2] );
	D3DXSHAdd( fSum[0], dwOrder, fSum[0], fSkybox1Rot[0] );
	D3DXSHAdd( fSum[1], dwOrder, fSum[1], fSkybox1Rot[1] );
	D3DXSHAdd( fSum[2], dwOrder, fSum[2], fSkybox1Rot[2] );*/

	/*float fSkybox2[3][D3DXSH_MAXORDER*D3DXSH_MAXORDER];  
	float fSkybox2Rot[3][D3DXSH_MAXORDER*D3DXSH_MAXORDER];  
	D3DXSHScale( fSkybox2[0], dwOrder, g_fSkyBoxLightSH[g_dwSkyBoxB][0], fEnv2Scaler*fEnvBlendScaler );
	D3DXSHScale( fSkybox2[1], dwOrder, g_fSkyBoxLightSH[g_dwSkyBoxB][1], fEnv2Scaler*fEnvBlendScaler );
	D3DXSHScale( fSkybox2[2], dwOrder, g_fSkyBoxLightSH[g_dwSkyBoxB][2], fEnv2Scaler*fEnvBlendScaler );
	D3DXSHRotate( fSkybox2Rot[0], dwOrder, &mWorldInv, fSkybox2[0] );
	D3DXSHRotate( fSkybox2Rot[1], dwOrder, &mWorldInv, fSkybox2[1] );
	D3DXSHRotate( fSkybox2Rot[2], dwOrder, &mWorldInv, fSkybox2[2] );
	D3DXSHAdd( fSum[0], dwOrder, fSum[0], fSkybox2Rot[0] );
	D3DXSHAdd( fSum[1], dwOrder, fSum[1], fSkybox2Rot[1] );
	D3DXSHAdd( fSum[2], dwOrder, fSum[2], fSkybox2Rot[2] );*/

	PRT_ComputeShaderConstants( fSum[0], fSum[1], fSum[2], dwOrder*dwOrder );
	PRT_ComputeSHIrradEnvMapConstants( fSum[0], fSum[1], fSum[2] );
}
//For 9.c Ends
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//PRT ENDS
//////////////////////////////////////////////////////////////////////////

KModel::MTLMSEFFECT::~MTLMSEFFECT()
{
	if(m_HEffect)
	{
		//SAFE_RELEASE(m_HEffect->pEffect);
		//SAFE_DELETE(m_HEffect);
		m_HEffect = NULL;
	}

	for(DWORD i=0;i<InputConstant.size();i++)
	{
		InputConstant[i].Clear();
	}
	InputConstant.clear();
}

HRESULT KModel::SHADERCONSTANT::Clear()
{
	switch(dwType)
	{
	case MSEFFECT_STRING:
		SAFE_DELETE_ARRAY(pData);
		break;

	case MSEFFECT_INT:
	case MSEFFECT_FLOAT:
	case MSEFFECT_FLOAT2:
	case MSEFFECT_FLOAT3:
	case MSEFFECT_FLOAT4:
		SAFE_DELETE(pData);
		break;

	case MSEFFECT_TEXTURE:
	case MSEFFECT_TEXTURE3D:
	case MSEFFECT_TEXTURECUBE:
		{
			LPTEXTUREDATA pText = (LPTEXTUREDATA) pData;
			SAFE_DELETE(pText);
			break;
		}
	default:
		if(pData)
		{
			int k = 0;
		}
	}
	scName.clear();
	return S_OK;
}

HRESULT KModel::Copy(KModel** ppNewModel)
{
	(*ppNewModel) = this;
	return S_OK;
}

void KModel::ClearAnimationTracer()
{
	DWORD dwFrameIndex;
	map<string, D3DXMATRIX**>::iterator it = m_AnimationTracer.begin();
	while ( it != m_AnimationTracer.end() )
	{
		string strAnimationName = it->first;
		int nAniIndex = FindAnimation(strAnimationName.c_str());
		DWORD dwNumFrames = m_lpAnimations[nAniIndex]->NumFrames;
		for( dwFrameIndex = 0; dwFrameIndex < dwNumFrames; dwFrameIndex++ )
		{
			delete[] it->second[dwFrameIndex];
		}
		delete[] it->second;
		it++;
	}
	m_AnimationTracer.clear();

	map<string, LPDIRECT3DVERTEXBUFFER9>::iterator itBT = m_BoneTracer.begin();
	while ( itBT != m_BoneTracer.begin() )
	{
		itBT->second->Release();
		itBT++;
	}
	m_BoneTracer.clear();
}

HRESULT KModel::InitializeTrace(TCHAR* strAnimationName)
{
	HRESULT hr = E_FAIL;
    assert(strAnimationName);
	int nAnimationIndex = FindAnimation(strAnimationName);
	if ( m_AnimationTracer.count(strAnimationName) )
	{
		//Already initialized
		hr = S_OK;
	}
	else
	{
		DWORD dwNumFrames = m_lpAnimations[nAnimationIndex]->NumFrames;
		DWORD dwFrameIndex;
		D3DXMATRIX** ppSkeletonMatrices = new D3DXMATRIX*[dwNumFrames];
		assert(ppSkeletonMatrices);
		for( dwFrameIndex = 0; dwFrameIndex < dwNumFrames; dwFrameIndex++ )
		{
			ppSkeletonMatrices[dwFrameIndex] = new D3DXMATRIX[m_dNumBone];
			assert(ppSkeletonMatrices);
			GetTransformAtFrame(dwFrameIndex, ppSkeletonMatrices[dwFrameIndex]);
		}
		m_AnimationTracer[strAnimationName] = ppSkeletonMatrices;
		hr = S_OK;		
	}
	return hr;
}

HRESULT KModel::DrawBoneTrace(const TCHAR* strBoneName)
{
	HRESULT hr = E_FAIL;
	if ( m_nCurAnimationIndex < 0 || (DWORD)m_nCurAnimationIndex > m_dNumAnimation )
		return hr;

	int nBoneIndex = FindBone(strBoneName);
	if ( nBoneIndex != -1 )
	{
		//BONEINFO& pBone = m_lpBoneInfo[nBoneIndex];
			
		if ( !m_BoneTracer.count(strBoneName) )
		{
			LPDIRECT3DVERTEXBUFFER9 pBoneVB = NULL;
			hr = g_pd3dDevice->CreateVertexBuffer(sizeof(_NormalPoint) * m_lpAnimations[m_nCurAnimationIndex]->NumFrames * 2, 
				                                  0, D3DFVF_DIFFUSE | D3DFVF_XYZ, D3DPOOL_MANAGED, &pBoneVB, 0 );
			if ( SUCCEEDED(hr) )
			{
				_NormalPoint* pData = NULL;
				pBoneVB->Lock(0, 0, (void**)&pData, 0);
				D3DXMATRIX** ppBoneMatrices = m_AnimationTracer[m_lpAnimations[m_nCurAnimationIndex]->Name];
				for( DWORD i = 0; i < m_lpAnimations[m_nCurAnimationIndex]->NumFrames; i++ )
				{
					//D3DXMATRIX* pBoneMatrix = m_AnimationTracer[m_lpAnimations[m_nCurAnimationIndex]->Name]
					D3DXVec3TransformCoord(&(pData + i)->vPosition, &(pData + i)->vPosition, ppBoneMatrices[i] + nBoneIndex);//pBoneMatrices[i]
					(pData+i)->color = 0xffff0000;
					(pData + i + m_lpAnimations[m_nCurAnimationIndex]->NumFrames)->vPosition = (pData + i)->vPosition;
					(pData + i + m_lpAnimations[m_nCurAnimationIndex]->NumFrames)->color = 0xffffffff;
				}
				pBoneVB->Unlock();
				m_BoneTracer[strBoneName] = pBoneVB;

			}
		}
		
		if ( m_BoneTracer.count(strBoneName) )//Make sure the vertex buffer is valid
		{
			//There is vertex buffer for the bone
			D3DXMATRIX matWorldSave, matWorld;
			D3DXMatrixIdentity(&matWorld);
			matWorld._43 = 150.0f;
			g_pd3dDevice->GetTransform(D3DTS_WORLD, &matWorldSave);
			g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);
			g_pd3dDevice->SetStreamSource(0, m_BoneTracer[strBoneName], 0, sizeof(_NormalPoint));
			g_pd3dDevice->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);
			g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
			g_pd3dDevice->SetTexture(0, NULL);

			g_pd3dDevice->DrawPrimitive(D3DPT_LINESTRIP, 0, m_lpAnimations[m_nCurAnimationIndex]->NumFrames - 1);

			
			g_pd3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);
			g_pd3dDevice->DrawPrimitive(D3DPT_POINTLIST, m_lpAnimations[m_nCurAnimationIndex]->NumFrames, m_lpAnimations[m_nCurAnimationIndex]->NumFrames);
			g_pd3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
			g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorldSave);
			hr = S_OK;
		}
		
		
	}
	return hr;
}


//Note: The size of pMatrix must be at least as large as the number of bones the skeleton otherwise crash will occur
HRESULT KModel::GetTransformAtFrame(int nFrame, D3DXMATRIX* pMatrix)
{
	HRESULT hr = E_FAIL;
	DWORD dwBoneIndex;
	if ( !pMatrix || m_nCurAnimationIndex < 0 || (DWORD)m_nCurAnimationIndex > m_dNumAnimation || 
		m_lpAnimations[m_nCurAnimationIndex]->NumFrames <= static_cast<DWORD>(nFrame) )
		return hr;
	
	LPANIMATION& pCurrentAnimation = m_lpAnimations[m_nCurAnimationIndex];
	switch( pCurrentAnimation->Type)
	{
	case ANIMATION_BONE:
		{
			for( dwBoneIndex = 0; dwBoneIndex < pCurrentAnimation->NumAnimatedBone; dwBoneIndex++ )
			{
				TCHAR* strBoneName = pCurrentAnimation->AnimatedBoneName[dwBoneIndex];
				DWORD dwBoneOffset = FindBone(strBoneName);
				if( dwBoneOffset != -1 )
				{
					DWORD dwMatrixOffset = pCurrentAnimation->NumFrames * dwBoneOffset + nFrame;
					pMatrix[dwBoneOffset] = pCurrentAnimation->AnimatedBoneMatrix[dwMatrixOffset];
				}
			}
			hr = S_OK;
			break;
		}
	case ANIMATION_BONE_RELATIVE:
		{
			D3DXMATRIX* pRelativeMatrix = new D3DXMATRIX[m_dNumBone];


			assert(pRelativeMatrix);

			for( dwBoneIndex = 0; dwBoneIndex < pCurrentAnimation->NumAnimatedBone; dwBoneIndex++ )
			{
				TCHAR* strBoneName = pCurrentAnimation->AnimatedBoneName[dwBoneIndex];
				DWORD dwBoneOffset = FindBone(strBoneName);
				if( dwBoneOffset != -1 )
				{
					DWORD dwMatrixOffset = pCurrentAnimation->NumFrames * dwBoneOffset + nFrame;
					pRelativeMatrix[dwBoneOffset] = pCurrentAnimation->AnimatedBoneMatrix[dwMatrixOffset];
				}
			}

			D3DXMATRIX Matrix;
			D3DXMatrixIdentity(&Matrix);
			for(int S=0; S<(int)m_nBaseBoneID.size(); S++)
			{
				UpdataRelativeBoneMatrix(m_nBaseBoneID[S], Matrix, pRelativeMatrix, pMatrix);
			}

			SAFE_DELETE_ARRAY(pRelativeMatrix);
			hr = S_OK;
		}
		break;
	case ANIMATION_BONE_RTS:
		{
			D3DXMATRIX* pRelativeMatrix = new D3DXMATRIX[m_dNumBone];
			assert(pRelativeMatrix);
			for( dwBoneIndex = 0; dwBoneIndex < pCurrentAnimation->NumAnimatedBone; dwBoneIndex++ )
			{
				TCHAR* strBoneName = pCurrentAnimation->AnimatedBoneName[dwBoneIndex];
				DWORD dwBoneOffset = FindBone(strBoneName);
				LPBONEINFO pInfo = &m_lpBoneInfo[dwBoneOffset];
				if( dwBoneOffset != -1 )
				{
					DWORD dwMatrixOffset = pCurrentAnimation->NumFrames * dwBoneOffset + nFrame;
					D3DXMATRIX&   MatFrame = pRelativeMatrix[dwBoneOffset];
					RTS&         rtsFrame = pCurrentAnimation->AnimatedBoneRTS[dwMatrixOffset];
					

					D3DXMATRIX matSRot, matSRotInv, matRot, matScale, matTrans;


					D3DXMatrixRotationQuaternion(&matSRot, &rtsFrame.SRotation);
					D3DXMatrixInverse(&matSRotInv, NULL, &matSRot);

					D3DXMatrixRotationQuaternion(&matRot, &rtsFrame.Rotation);
					D3DXMatrixScaling(&matScale, rtsFrame.Scale.x, rtsFrame.Scale.y, rtsFrame.Scale.z);

					matScale._11 *= pInfo->BoneScale.x;
					matScale._22 *= pInfo->BoneScale.y;
					matScale._33 *= pInfo->BoneScale.z;

					MatFrame = matRot * matSRot *matScale * matSRotInv;
					D3DXMatrixScaling(&matScale, rtsFrame.Sign, rtsFrame.Sign, rtsFrame.Sign );
					MatFrame = matScale * MatFrame;

					D3DXMatrixTranspose(&MatFrame, &MatFrame);
					MatFrame.m[3][0] += rtsFrame.Translation.x;
					MatFrame.m[3][1] += rtsFrame.Translation.y;
					MatFrame.m[3][2] += rtsFrame.Translation.z;
				}
			}

			D3DXMATRIX Matrix;
			D3DXMatrixIdentity(&Matrix);
			for(int S=0; S<(int)m_nBaseBoneID.size(); S++)
			{
				UpdataRelativeBoneMatrix(m_nBaseBoneID[S], Matrix, pRelativeMatrix, pMatrix);
			}
			SAFE_DELETE_ARRAY(pRelativeMatrix);
			hr = S_OK;
		}
		break;

	}
	return hr;
}


//Note: pSrc and pDest must be the same size and both sizes should be at least equal to skeleton bone size
HRESULT KModel::UpdataRelativeBoneMatrix(int BoneIndex,D3DXMATRIX Matrix, D3DXMATRIX* pSrc, D3DXMATRIX* pDest)
{
	HRESULT hr = E_FAIL;

	if ( BoneIndex == -1 || !pSrc || !pDest )
		return hr;
	
	LPBONEINFO pInfo = &m_lpBoneInfo[BoneIndex];
	pDest[BoneIndex] = pSrc[BoneIndex] * Matrix;
	//pSrc[BoneIndex] = pDest[BoneIndex];

	for(DWORD i_Child=0;i_Child<pInfo->ChildIDVector.size();i_Child++)
	{
		int ChildIndex = pInfo->ChildIDVector[i_Child];
		UpdataRelativeBoneMatrix(ChildIndex,pDest[BoneIndex], pSrc, pDest);
	}
	return hr;
}

//HRESULT KModel::InterpolateSquadRTSBoneKeyFrame(LPBONEINFO& pInfo, const RTS& FrameCur, const RTS& FrameA, const RTS& FrameB, const RTS& FrameC, float fWeight)
//{
//	
//}

HRESULT KModel::InterpolateRTSBoneKeyFrame(LPBONEINFO& pInfo, RTS& FrameA, RTS& FrameB, float fWeight)
{

	D3DXMATRIX matSRot, matSRotInv, matRot, matScale, matTrans, matScaleSign;
	D3DXQUATERNION sRot, Rot;
	D3DXVECTOR3 Scale, Translation;

	D3DXMATRIX MatFrameA, MatFrameB;

	/////////////////////////////////////////////////////////////////////////////////////////////////////
	//Key frame interpolation 1 end
	/*D3DXMatrixRotationQuaternion(&matSRot, &FrameA.SRotation);
	D3DXMatrixInverse(&matSRotInv, NULL, &matSRot);

	D3DXMatrixRotationQuaternion(&matRot, &FrameA.Rotation);
	D3DXMatrixScaling(&matScale, FrameA.Scale.x, FrameA.Scale.y, FrameA.Scale.z);

	matScale._11 *= pInfo->BoneScale.x;
	matScale._22 *= pInfo->BoneScale.y;
	matScale._33 *= pInfo->BoneScale.z;

	MatFrameA = matRot * matSRot *matScale * matSRotInv;
	D3DXMatrixScaling(&matScale, FrameA.Sign, FrameA.Sign, FrameA.Sign );
	MatFrameA = matScale * MatFrameA;

	D3DXMatrixTranspose(&MatFrameA, &MatFrameA);
	MatFrameA.m[3][0] += FrameA.Translation.x;
	MatFrameA.m[3][1] += FrameA.Translation.y;
	MatFrameA.m[3][2] += FrameA.Translation.z;


	D3DXMatrixRotationQuaternion(&matSRot, &FrameB.SRotation);
	D3DXMatrixInverse(&matSRotInv, NULL, &matSRot);

	D3DXMatrixRotationQuaternion(&matRot, &FrameB.Rotation);
	D3DXMatrixScaling(&matScale, FrameB.Scale.x, FrameB.Scale.y, FrameB.Scale.z);

	matScale._11 *= pInfo->BoneScale.x;
	matScale._22 *= pInfo->BoneScale.y;
	matScale._33 *= pInfo->BoneScale.z;

	MatFrameB = matRot * matSRot *matScale * matSRotInv;
	D3DXMatrixScaling(&matScale, FrameB.Sign, FrameB.Sign, FrameB.Sign );

	MatFrameB = matScale * MatFrameB;
	D3DXMatrixTranspose(&MatFrameB, &MatFrameB);

	MatFrameB.m[3][0] += FrameB.Translation.x;
	MatFrameB.m[3][1] += FrameB.Translation.y;
	MatFrameB.m[3][2] += FrameB.Translation.z;
	pInfo->RelativeMatrix = MatFrameA * (1-fWeight) + MatFrameB * fWeight; */
	//Key frame interpolation 1 end
	//////////////////////////////////////////////////////////////////////////



	//////////////////////////////////////////////////////////////////////////
	//Key frame interpolation 2	

	//sRot = FrameA.SRotation;
	if ( !strcmp(pInfo->scName.c_str(), "Bip01 L Calf") )
	{
		int nForDebug0 = 10;
	}
	
	D3DXQUATERNION quat1, quat2;

	//D3DXQuaternionNormalize(&quat1, &FrameA.SRotation);
	//D3DXQuaternionNormalize(&quat2, &FrameB.SRotation);
	//D3DXQuaternionSlerp(&sRot, &quat1, &quat2, fWeight);

	//D3DXQuaternionNormalize(&sRot, &sRot);
	if (D3DXQuaternionDot(&FrameA.SRotation, &FrameB.SRotation) < 0.0f )
	{
		const char* strBoneName = pInfo->scName.c_str();
		FrameA.SRotation = -FrameA.SRotation;
	}
	D3DXQuaternionSlerp(&sRot, &FrameA.SRotation, &FrameB.SRotation, fWeight);
	
	
	
	D3DXMatrixRotationQuaternion(&matSRot, &sRot);
	D3DXMatrixTranspose(&matSRot, &matSRot);
	D3DXMatrixInverse(&matSRotInv, NULL, &matSRot);

	//Rot = FrameA.Rotation;

	//D3DXQuaternionNormalize(&quat1, &FrameA.Rotation);
	//D3DXQuaternionNormalize(&quat2, &FrameB.Rotation);
	//D3DXQuaternionSlerp(&Rot, &quat1, &quat2, fWeight);
	//D3DXQuaternionNormalize(&Rot, &Rot);

	if ( D3DXQuaternionDot(&FrameA.Rotation, &FrameB.Rotation) < 0.0f )
	{
		const char* strBoneName = pInfo->scName.c_str();
		FrameA.Rotation = -FrameA.Rotation;
	}
	D3DXQuaternionSlerp(&Rot, &FrameA.Rotation, &FrameB.Rotation, fWeight);

	D3DXMatrixRotationQuaternion(&matRot, &Rot);
	D3DXMatrixTranspose(&matRot, &matRot);

	//Scale = FrameA.Scale;
	//Translation = FrameA.Translation;
	D3DXVec3Lerp(&Scale, &FrameA.Scale, &FrameB.Scale, fWeight);
	D3DXVec3Lerp(&Translation, &FrameA.Translation, &FrameB.Translation, fWeight);

	D3DXMatrixScaling(&matScale, Scale.x * pInfo->BoneScale.x, Scale.y * pInfo->BoneScale.y, Scale.z * pInfo->BoneScale.z);

	D3DXMatrixScaling(&matScaleSign, FrameA.Sign, FrameA.Sign, FrameA.Sign);
	D3DXMatrixTranslation(&matTrans, Translation.x, Translation.y, Translation.z);

	//D3DXMatrixTranspose(&matSRotInv, &matSRotInv);
	//D3DXMatrixTranspose(&matSRot, &matSRot);
	//D3DXMatrixTranspose(&matRot, &matRot);


	//D3DXMatrixTranspose(&matTrans, &matTrans);
	//pInfo->RelativeMatrix = matTrans * matScaleSign * matRot * matSRot *matScale * matSRotInv;
	pInfo->RelativeMatrix = matSRotInv * matScale * matSRot * matRot * matScaleSign * matTrans;
	//D3DXMatrixTranspose(&pInfo->RelativeMatrix, &pInfo->RelativeMatrix);
	/*pInfo->RelativeMatrix._41 += Translation.x;
	pInfo->RelativeMatrix._42 += Translation.y;
	pInfo->RelativeMatrix._43 += Translation.z;*/

	
	////Key frame interpolation 2	end
	//////////////////////////////////////////////////////////////////////////
	return S_OK;
}

HRESULT KModel::SHADERCONSTANT::GetDesc(LPSTR pStr)
{
	switch(dwType)
	{
	case MSEFFECT_VOID:
		wsprintf(pStr,"任意类型");
		return S_OK;
		break;
	case MSEFFECT_BOOL:
		wsprintf(pStr,"BOOL");
		return S_OK;
		break;
	case MSEFFECT_INT:
		{
			int* pValue = (int*) this->pData;
			wsprintf(pStr,"INT %d",*pValue);
			return S_OK;
			break;
		}
	case MSEFFECT_FLOAT:
		{
			float * pValue = (float*) this->pData;
			sprintf(pStr,"FLOAT %.2f",*pValue);
			return S_OK;
			break;
		}

	case MSEFFECT_FLOAT2:
		wsprintf(pStr,"FLOAT2");
		return S_OK;
		break;
	case MSEFFECT_FLOAT3:
		wsprintf(pStr,"FLOAT3");
		return S_OK;
		break;
	case MSEFFECT_FLOAT4:
		wsprintf(pStr,"FLOAT4");
		return S_OK;
		break;

	case MSEFFECT_TEXTURE:
		{
			LPTEXTUREDATA  pValue = (LPTEXTUREDATA) pData;		
			wsprintf(pStr,"TEXTURE %s",pValue->FileName.c_str());
			return S_OK;
			break;
		}
	case MSEFFECT_TEXTURE1D:
		{
			LPTEXTUREDATA  pValue = (LPTEXTUREDATA) pData;		
			wsprintf(pStr,"TEXTURE1D %s",pValue->FileName.c_str());
			return S_OK;
			break;
		}
	case MSEFFECT_TEXTURE2D:
		{
			LPTEXTUREDATA  pValue = (LPTEXTUREDATA) pData;		
			wsprintf(pStr,"TEXTURE2D %s",pValue->FileName.c_str());
			return S_OK;
			break;
		}
	case MSEFFECT_TEXTURE3D:
		{
			LPTEXTUREDATA  pValue = (LPTEXTUREDATA) pData;		
			wsprintf(pStr,"TEXTURE3D %s",pValue->FileName.c_str());
			return S_OK;
			break;
		}
	case MSEFFECT_TEXTURECUBE:
		{
			LPTEXTUREDATA  pValue = (LPTEXTUREDATA) pData;		
			wsprintf(pStr,"TEXTURECUBE %s",pValue->FileName.c_str());
			return S_OK;
			break;
		}

	case MSEFFECT_MATERIALAMBIENT:
		wsprintf(pStr,"MATERIALAMBIENT 自动");
		return E_FAIL;
		break;
	case MSEFFECT_MATERIALDIFFUSE:
		wsprintf(pStr,"MATERIALDIFFUSE自动");
		return E_FAIL;
		break;
	case MSEFFECT_MATERIALSPECULAR:
		wsprintf(pStr,"MATERIALSPECULAR自动");
		return E_FAIL;
		break;
	case MSEFFECT_MATERIALEMISSIVE:
		wsprintf(pStr,"MATERIALEMISSIVE自动");
		return E_FAIL;
		break;
	case MSEFFECT_MATERIALPOWER:
		wsprintf(pStr,"MATERIALPOWER自动");
		return E_FAIL;
		break;

	case  MSEFFECT_LOCALBBOXMIN :
		wsprintf(pStr,"LOCALBBOXMIN 自动");
		return E_FAIL;
		break;
	case MSEFFECT_LOCALBBOXMAX:
		wsprintf(pStr,"LOCALBBOXMAX自动");
		return E_FAIL;
		break;

	case MSEFFECT_WORLD :
		wsprintf(pStr,"WORLD 自动");
		return E_FAIL;
		break;
	case MSEFFECT_WORLDI :
		wsprintf(pStr,"WORLDI自动");
		return E_FAIL;
		break;
	case MSEFFECT_WORLDIT  :
		wsprintf(pStr,"WORLDIT 自动");
		return E_FAIL;
		break;
	case MSEFFECT_VIEW :
		wsprintf(pStr,"VIEW自动");
		return E_FAIL;
		break;
	case MSEFFECT_VIEWI :
		wsprintf(pStr,"VIEWI自动");
		return E_FAIL;
		break;
	case MSEFFECT_VIEWIT :
		wsprintf(pStr,"VIEWIT自动");
		return E_FAIL;
		break;
	case MSEFFECT_PROJECTION  :
		wsprintf(pStr,"PROJECTION自动");
		return E_FAIL;
		break;
	case MSEFFECT_PROJECTIONI  :
		wsprintf(pStr,"PROJECTIONI 自动");
		return E_FAIL;
		break;
	case MSEFFECT_PROJECTIONIT  :
		wsprintf(pStr,"PROJECTIONIT 自动");
		return E_FAIL;
		break;
	case MSEFFECT_WORLDVIEW :
		wsprintf(pStr,"WORLDVIEW自动");
		return E_FAIL;
		break;
	case MSEFFECT_WORLDVIEWI :
		wsprintf(pStr,"WORLDVIEWI自动");
		return E_FAIL;
		break;
	case MSEFFECT_WORLDVIEWIT :
		wsprintf(pStr,"WORLDVIEWIT自动");
		return E_FAIL;
		break;

	case MSEFFECT_WORLDVIEWPROJ :
		wsprintf(pStr,"WORLDVIEWPROJ自动");
		return E_FAIL;
		break;
	case MSEFFECT_WORLDVIEWPROJI :
		wsprintf(pStr,"WORLDVIEWPROJI自动");
		return E_FAIL;
		break;
	case MSEFFECT_WORLDVIEWPROJIT :
		wsprintf(pStr,"WORLDVIEWPROJIT自动");
		return E_FAIL;
		break;
	case MSEFFECT_SUNDIRMATRIX :
		wsprintf(pStr,"SUNDIRMATRIX自动");
		return E_FAIL;
		break;

	case MSEFFECT_POSITION :
		wsprintf(pStr,"POSITION");
		return S_OK;
		break;
	case MSEFFECT_DIRECTION :
		wsprintf(pStr,"DIRECTION");
		return S_OK;
		break;
	case MSEFFECT_COLORRGBA :
		wsprintf(pStr,"COLORRGBA");
		return S_OK;
		break;
	case MSEFFECT_STRING :
		wsprintf(pStr,"STRING");
		return S_OK;
		break;

	default:
		wsprintf(pStr,"未定义");
		return S_OK;
		break;
	}
	return S_OK;
}
HRESULT KModel::SHADERCONSTANT::GetParameter(LPSTR pStr)
{
	return S_OK;
}




//////////////////////////////////////////////////////////////////////////
//Add 7-25-2005 MultiMotion Start
HRESULT KMotion::CheckBWTable(TCHAR** ppErrBones )
{
	HRESULT hr = E_FAIL;
	return hr;
}

HRESULT KMotion::SaveMotion( const TCHAR* strMotionName )
{
	HRESULT hr = E_FAIL;
	return hr;
}

HRESULT KMotion::LoadMotion( const TCHAR* strMotionName )
{
	HRESULT hr = E_FAIL;
	return hr;
}

void KMotion::PlayMotion( float fSpeed, int nType )
{

}

void KMotion::Pause()
{

}

void KMotion::Stop()
{

}

KMotion::Track* KMotion::FindTrack( TRACK_IDENTIFIER ID )
{
	Track* pReturn = NULL;
#ifdef CLIENT_EDITION
	for( int nIndex = 0; nIndex < m_nNumTrack; nIndex++ )
	{
		Track* pCurTrack = m_pTracks[nIndex];
		if ( TrackIDCmp( pCurTrack->TrackID, ID ) )
		{
			pReturn = pCurTrack;
			break;
		}
	}
#else
	list<Track*>::iterator it = m_Tracks.begin();
	while( it != m_Tracks.end() )
	{
		Track* pCurTrack = (*it);
		if ( TrackIDCmp( pCurTrack->TrackID, ID) )
		{
			pReturn = pCurTrack;
			break;
		}
		it++;
	}
#endif
	return pReturn;
}

//HRESULT KMotion::AttachSequence( AnimationSequenceHeader* pSeqHeader, BoneWeightTable* pBoneWeightTable, float fStartTime, BOOL bWarpInRange )
//{
//	HRESULT hr = E_FAIL;
//	
//	return hr;
//}
//Add 7-25-2005 MultiMotion End
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//Add 7-25-2005 Bone indicator start
const float KModel::KBoneIndicator::m_fWidth = 2.0f;
HRESULT KModel::KBoneIndicator::LinkToBone( KModel* pModel, const TCHAR* pBoneName )
{
	HRESULT hr = E_FAIL;
	ASSERT( pModel && pBoneName );
	m_nBoneIndex = pModel->FindBone( pBoneName );
	if ( m_nBoneIndex != -1 )
	{
		KModel::LPBONEINFO pBone = &pModel->m_lpBoneInfo[m_nBoneIndex];
		m_nParentBoneIndex = pModel->FindBone( pBone->scParentName.c_str() );
		if ( m_nParentBoneIndex != -1 && pBone->scParentName != "" )
		{
			m_pModel = pModel;
			m_bHide = FALSE;
			hr = S_OK;
		}
	}
	return hr;
}

HRESULT KModel::KBoneIndicator::DrawBoneIndicator()
{
	HRESULT hr = E_FAIL;
	if ( ! m_bHide )
	{
		if ( !m_lpBoneIndicator )
			InitBoneIndicator();
	
		UpdateBoneIndicator();
		g_pd3dDevice->SetFVF( BoneIndicatorVertex::FVF );
		g_pd3dDevice->SetStreamSource( 0, m_lpBoneIndicator, 0, sizeof( BoneIndicatorVertex) );
		g_pd3dDevice->SetIndices( m_lpBoneIndicatorIB );
		g_pd3dDevice->SetTexture( 0 , NULL );
		g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
		//D3DXMATRIX matSave;
		//g_pd3dDevice->GetTransform( D3DTS_WORLD, &matSave );
		//D3DXMATRIX mat = m_pModel->m_lpBoneInfo[m_nParentBoneIndex].CurMatrix;
		//mat._11 = 1.0f;
		//mat._22 = 1.0f;
		//mat._33 = 1.0f;
		//g_pd3dDevice->SetTransform( D3DTS_WORLD, &mat );
		//g_pd3dDevice->DrawPrimitive( D3DPT_LINELIST, 0, 4 );
		//g_pd3dDevice->SetTransform( D3DTS_WORLD, &matSave );
		g_pd3dDevice->DrawIndexedPrimitive( D3DPT_LINELIST, 0, 0, 6, 0, 12 );
		hr = S_OK;
	}
	return hr;
}

HRESULT KModel::KBoneIndicator::InitBoneIndicator()
{
	HRESULT hr = E_FAIL;
	SAFE_RELEASE( m_lpBoneIndicator );
	if( SUCCEEDED( g_pd3dDevice->CreateVertexBuffer( sizeof(BoneIndicatorVertex) * 8, 0, 
		            BoneIndicatorVertex::FVF, D3DPOOL_MANAGED, &m_lpBoneIndicator, 0 ) ) )
	{
		
		if( SUCCEEDED( g_pd3dDevice->CreateIndexBuffer( 4 * 12, 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_lpBoneIndicatorIB, 0 ) ) )
		{
			//short Buffer[] = { { 0, 1,}, { 0, 3}};
			short Buffer[12][2] = { {0, 1}, {0, 3}, {0, 4}, {1, 2}, {1, 5}, {2, 3}, 
			                        {2, 6}, {3, 7}, {4, 7}, {4, 5}, {5, 6}, {6, 7} };
			short* pIndex = NULL;
			m_lpBoneIndicatorIB->Lock( 0, 0, (void**)&pIndex , 0 );
			memcpy(pIndex, Buffer, sizeof(short) * 24);
			m_lpBoneIndicatorIB->Unlock();
			hr = S_OK;
		}
	}
	return hr;
}

void KModel::KBoneIndicator::ClearBoneIndicator()
{
	SAFE_RELEASE( m_lpBoneIndicator );
	SAFE_RELEASE( m_lpBoneIndicatorIB );
}

KModel::KBoneIndicator::KBoneIndicator()
{
	m_lpBoneIndicator = NULL;
	m_lpBoneIndicatorIB = NULL;
	m_bHide = TRUE;
	m_nBoneIndex = -1;
	m_nParentBoneIndex = -1;
	m_pModel = NULL;
	m_fBoneLength = 9999999.0f;
	InitBoneIndicator();
}

KModel::KBoneIndicator::~KBoneIndicator()
{
	ClearBoneIndicator();
}

void KModel::KBoneIndicator::UpdateBoneIndicator()
{
	ASSERT( m_pModel );
	float fBoneLength = .0f;
	KModel::LPBONEINFO pBone = &m_pModel->m_lpBoneInfo[m_nBoneIndex];
	D3DXMATRIX matLocal;
	if ( m_nParentBoneIndex != -1 )
	{
		KModel::LPBONEINFO pParentBone = &m_pModel->m_lpBoneInfo[m_nParentBoneIndex];
		D3DXVECTOR3 vec3Bone( 0.0f, 0.0f, 0.0f );
		D3DXVECTOR3 vec3ParentBone( 0.0f, 0.0f , 0.0f );
		D3DXVec3TransformCoord( &vec3Bone, &vec3Bone, &pBone->CurMatrix );
		D3DXVec3TransformCoord( &vec3ParentBone , &vec3ParentBone, &pParentBone->CurMatrix );
		D3DXVECTOR3 vec3BoneDir = vec3Bone - vec3ParentBone;
		BoneIndicatorVertex* pVert = NULL;
		
		D3DXVECTOR3 vec3Scale;
		D3DXQUATERNION quatRot;
		D3DXVECTOR3 vec3Trans;
		D3DXMATRIX matLocal;
		D3DXMatrixMultiply( &matLocal, &pBone->CurMatrix, &pParentBone->CurMatrix );
		
		m_lpBoneIndicator->Lock( 0, 0, (void**)&pVert, D3DLOCK_DISCARD );
		D3DXVECTOR3 vec3X(1.0f, 0.0f , 0.0f);
		D3DXVECTOR3 vec3Y(0.0f, 1.0f,  0.0f);
		if ( D3DXVec3Dot( &vec3X, &vec3BoneDir ) == 0.0f )
		{
			vec3X = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
		}
		D3DXVec3Cross(&vec3Y, &vec3X, &vec3BoneDir );
		D3DXVec3Cross(&vec3X, &vec3Y, &vec3BoneDir );
		D3DXVec3Normalize( &vec3X, &vec3X );
		D3DXVec3Normalize( &vec3Y, &vec3Y );

		for ( int i = 0; i < 8; i++ )
			pVert[i].dwColor = 0xffffffff;
		pVert[0].vec3Pos = (  vec3X + vec3Y ) * m_fWidth;
		pVert[1].vec3Pos = ( -vec3X + vec3Y ) * m_fWidth;
		pVert[2].vec3Pos = ( -vec3X - vec3Y ) * m_fWidth;
		pVert[3].vec3Pos = (  vec3X - vec3Y ) * m_fWidth;

		for ( int i = 0; i < 4; i++ )
		{
			//D3DXVec3TransformCoord( &pVert[i].vec3Pos, &pVert[i].vec3Pos, &matRot );
			pVert[i].vec3Pos += D3DXVECTOR3( pBone->CurMatrix._41, pBone->CurMatrix._42, pBone->CurMatrix._43 );
			pVert[i].vec3Pos.z += 150.0f;
			pVert[i + 4].vec3Pos = pVert[i].vec3Pos - vec3BoneDir;
		}
		
		m_lpBoneIndicator->Unlock();
	}
	else 
	{
		//No parent bone
		fBoneLength = 20.0f;
		matLocal = pBone->MatrixOffset;
	}

}
//Add 7-25-2005 Bone indicator end
//////////////////////////////////////////////////////////////////////////

void KModel::PlaySequence( TCHAR* strSeqName )
{
	m_bPlaySequence = TRUE;
	m_pCurSeq->PlaySequence( strSeqName );	
}

HRESULT KModel::FixVertexWeight()
{
	if ( m_pBlendMesh )
	{
		if(!m_pMeshSys)
			return E_FAIL;
		float* pWeights = new float[m_pMeshSys->GetNumVertices()];
		memset(pWeights, 0, sizeof(float) * m_pMeshSys->GetNumVertices());
		for(int i = 0; i < m_dNumBone; i++)
		{
			BONEINFO& Bone = m_lpBoneInfo[i];
			for(int j = 0; j < Bone.NumVerticesRef; j++)
			{
				pWeights[Bone.VerticesIndexRef[j]] += Bone.VerticesWeightRef[j];
			}
		}
		for(int i = 0; i < m_dNumBone; i++)
		{
			BONEINFO& Bone = m_lpBoneInfo[i];
			for(int j = 0; j < Bone.NumVerticesRef; j++)
			{
				Bone.VerticesWeightRef[j] /= pWeights[Bone.VerticesIndexRef[j]];
			}
		}
		memset(pWeights, 0, sizeof(float) * m_pMeshSys->GetNumVertices());
		for(int i = 0; i < m_dNumBone; i++)
		{
			BONEINFO& Bone = m_lpBoneInfo[i];
			for(int j = 0; j < Bone.NumVerticesRef; j++)
			{
				pWeights[Bone.VerticesIndexRef[j]] += Bone.VerticesWeightRef[j];
			}
		}
		for(int i = 0; i < m_pMeshSys->GetNumVertices(); i++)
		{
			if ( fabs(pWeights[i] - 1.0f) > 0.0001 )
			{
				TCHAR strErrorInfo[256];
				sprintf(strErrorInfo, "模型: %s 顶点: %d 权重和: %f", m_szMeshFileName, i, pWeights[i]);
			}
		}
		CreateSkinInfo(FALSE);
		SaveMesh(m_szMeshFileName, TRUE);
		SAFE_DELETE_ARRAY(pWeights);
	}
}