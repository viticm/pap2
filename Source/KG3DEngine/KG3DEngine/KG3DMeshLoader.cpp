////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DMeshLoader.cpp
//  Version     : 1.0
//  Creator     : Wu Caizhong
//  Create Date : 2009/6/16 14:46:34
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "KG3DMesh.h"

#include "KG3DEngineManager.h"
#include "KG3DFileLoader.h"
#include "..\KG3DMultiThreadLoader.h"
#include "KG3DModel.h"
#include "KG3DBsp.h"

extern float g_fBoneWeight;

extern DWORD g_dwMainThreadID;//渲染线程ID
extern int g_nNumTextureDynamicLoadedFrame;
extern int g_nNumAllowTextureDynamicLoadFrame;//每桢允许操作(加载，更新)的贴图上限

extern int g_nLoadCount;
extern int g_nLoadTime ;

////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DMesh::LoadFromFile(const TCHAR* cszFileName, unsigned uFileNameHash, unsigned uOption)
{
	HRESULT hrResult = E_FAIL;
	HRESULT hrRetCode  = E_FAIL;
	int nRetCode = false;
    TCHAR szBSPFile[MAX_PATH];
    TCHAR szPropertyFile[MAX_PATH];
    IKG_Buffer *piMeshFile = NULL;
    IKG_Buffer *piBSPFile  = NULL;
    IIniFile   *piPropertyFile = NULL;

	KG_PROCESS_ERROR(cszFileName);
	m_scName = cszFileName;
	m_uOption = uOption;

    hrRetCode = ChangePathExtName(cszFileName, "bsp", sizeof(szBSPFile), szBSPFile);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    hrRetCode = ChangePathExtName(cszFileName, "ini", sizeof(szPropertyFile), szPropertyFile);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

	if((uOption & MLO_MESH_MULTI) && (g_eEngineOption.bUseMultiThreadLoad))
	{
        m_hMeshFileTask = g_FileLoader.CreateTask(cszFileName, FILE_TYPE_NORMAL);
        KGLOG_PROCESS_ERROR(m_hMeshFileTask);

        m_hBSPFileTask = g_FileLoader.CreateTask(szBSPFile, FILE_TYPE_NORMAL);
        KGLOG_PROCESS_ERROR(m_hBSPFileTask);

        m_hPropertyFileTask = g_FileLoader.CreateTask(szPropertyFile, FILE_TYPE_INI);
        KGLOG_PROCESS_ERROR(m_hPropertyFileTask);

        m_eDynamicLoadState = STATE_MULTITHREAD_PREPEAR;
		g_nLoadCount++;
	}
	else
	{
		DWORD dwTime = timeGetTime();

		nRetCode = g_FileLoader.LoadImmediately(cszFileName, &piMeshFile);
		KGLOG_PROCESS_ERROR(nRetCode && "File not exist");

        nRetCode = g_FileLoader.LoadImmediately(szBSPFile, &piBSPFile);
        //KGLOG_PROCESS_ERROR(nRetCode && "File not exist");

        piPropertyFile = g_OpenIniFile(szPropertyFile);
        //KG_PROCESS_ERROR(piPropertyFile && "File not exist");

        hrRetCode = _LoadFromMemory(piMeshFile, piBSPFile, piPropertyFile, uOption);
        KGLOG_COM_PROCESS_ERROR(hrRetCode);

        int loadcost = (int)(timeGetTime() - dwTime);
		if(loadcost>30)
		{
			//KGLogPrintf(KGLOG_ERR, "UnMutiMesh %d Load File %s.", loadcost,m_scName.c_str());
		}

		g_nLoadTime += loadcost;
	}
    hrResult = S_OK;
Exit0:
	KG_COM_RELEASE(piPropertyFile);
    KG_COM_RELEASE(piBSPFile);
    KG_COM_RELEASE(piMeshFile);
	if (FAILED(hrResult) && cszFileName)
	{
		KGLogPrintf(KGLOG_ERR, "Load file %s failed.", cszFileName);
	}
	return hrResult;
}

////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DMesh::_LoadFromMemory(
    IKG_Buffer *piMeshFile, IKG_Buffer *piBSPFile, IIniFile *piPropertyFile, unsigned uOption
)
{
    HRESULT hrRetCode = E_FAIL;
    HRESULT hrResult = E_FAIL;
    int nRetCode = false;
    _MeshFileHead *pFileHeader = NULL;
    KG3DBufferReader BufferReader;

    ASSERT(piMeshFile);
    //ASSERT(piBSPFile);
    //ASSERT(piPropertyFile);

    nRetCode = BufferReader.SetBuffer(piMeshFile);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = BufferReader.ReferenceRead((void **)&pFileHeader, sizeof(_MeshFileHead));
    KGLOG_PROCESS_ERROR(nRetCode);

    KGLOG_PROCESS_ERROR(pFileHeader->dwFileMask == 0x4D455348);
    KGLOG_PROCESS_ERROR(pFileHeader->dwVersion == 0);

    hrRetCode = _LoadMeshFileFromMemory(&BufferReader, uOption);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    if (piBSPFile)
    {
        hrRetCode = _LoadBSPFileFromMemory(piBSPFile);
        KGLOG_COM_PROCESS_ERROR(hrRetCode);
    }

    if (piPropertyFile)
    {
        hrRetCode = _LoadPropertyFromIni(piPropertyFile);
        KGLOG_COM_PROCESS_ERROR(hrRetCode);
    }

    m_eDynamicLoadState = STATE_VIDEOMEM;
    g_nNumTextureDynamicLoadedFrame++;
    g_nLoadCount++;

    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        m_eDynamicLoadState = STATE_LOADFAILED;
    }
    return hrResult;
}

////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DMesh::_LoadMeshFileFromMemory(KG3DBufferReader *pBufferReader,
										  unsigned uOption)
{
	HRESULT hrRetCode = E_FAIL;
	HRESULT hrResult = E_FAIL;
    ReadInMeshData MeshData;

	ASSERT(pBufferReader);

	hrRetCode = Helper_LoadMeshFromBuffer(pBufferReader, &MeshData);
	KG_COM_PROCESS_ERROR(hrRetCode);

	//创建在内存中的MESH
	hrRetCode = Helper_CreateD3DXMesh(MeshData);
	KGLOG_COM_PROCESS_ERROR(hrRetCode);

	//Fill in data
	hrRetCode = Helper_FillVertexData(MeshData);
	KGLOG_COM_PROCESS_ERROR(hrRetCode);

	if (uOption & MLO_MESH_SPEEDTREE)
	{
		m_dwOption &= ~MESH_SKINED;
	}

	if (m_dwOption & MESH_SKINED)
	{
		hrRetCode = Helper_CreateSkinInfo();
		KG_COM_PROCESS_ERROR(hrRetCode);

		hrRetCode = CreateSkinMesh();
		KG_COM_PROCESS_ERROR(hrRetCode);

		if (!(m_dwOptionEx & MESH_SAVE_BONEBBOX))
		{
			CreateBBoxOfBone();
		}

		if(g_eEngineOption.bUseProgressMeshLOD)
		{
			CreateProgressMesh();
		}
	}
	else
	{
		hrRetCode = OptimizeUnSkinedMesh();
	}
	KG_COM_PROCESS_ERROR(hrRetCode);

	//////////////////////////////////////////////////////////////////////////
	if(g_cEngineOption.bMultiTextureRender)
		CreateDataForCpuProcess();

	hrResult = S_OK;
Exit0:
	return hrResult;
}

////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DMesh::Helper_CreateSkinInfo()
{
	HRESULT hrResult = E_FAIL;
	HRESULT hrRetCode = E_FAIL;
	SAFE_RELEASE(m_pSkinInfo);
	KG_PROCESS_ERROR(m_ppMeshes[SMBT_NORMAL]);

	hrRetCode = D3DXCreateSkinInfoFVF(m_ppMeshes[SMBT_NORMAL]->GetNumVertices(),
		m_ppMeshes[SMBT_NORMAL]->GetFVF(),
		m_dwNumBone,
		&m_pSkinInfo);
	KG_COM_PROCESS_ERROR(hrRetCode);

	for(DWORD dwBoneIndex = 0; dwBoneIndex < m_dwNumBone; dwBoneIndex++)
	{
		BoneInfo1 &BoneInfo = m_pBoneInfo[dwBoneIndex];
		hrRetCode = m_pSkinInfo->SetBoneInfluence(dwBoneIndex,
			BoneInfo.NumVerticesRef,
			BoneInfo.VerticesIndexRef,
			BoneInfo.VerticesWeightRef);
		KG_COM_PROCESS_ERROR(hrRetCode);

		hrRetCode = m_pSkinInfo->SetBoneName(dwBoneIndex, BoneInfo.strBoneName);
		KG_COM_PROCESS_ERROR(hrRetCode);

		hrRetCode = m_pSkinInfo->SetBoneOffsetMatrix(dwBoneIndex, &m_pmatReverseMatrix[dwBoneIndex]);
		KG_COM_PROCESS_ERROR(hrRetCode);
	}
	hrResult = S_OK;
Exit0:
	return hrResult;
}
////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DMesh::CheckLoadingState()
{
	HRESULT hrRetCode = E_FAIL;
	HRESULT hrResult = S_OK;

	DWORD dwThreadID = ::GetCurrentThreadId();
	KGLOG_PROCESS_ERROR(dwThreadID == g_dwMainThreadID);

	switch(m_eDynamicLoadState)
	{
	case STATE_MULTITHREAD_PREPEAR:
		{
            hrRetCode = _ProcessDelayLoad();
            KGLOG_COM_PROCESS_ERROR(hrRetCode);
		}
		break;
	case STATE_NONE:
	case STATE_LOADFAILED:
		{
            KG_PROCESS_ERROR(false);
		}
		break;
	case STATE_VIDEOMEM:
		break;
	default:
		assert(0);
	}
    hrResult = S_OK;
Exit0:
	return hrResult;
}
////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DMesh::_ProcessDelayLoad()
{
    HRESULT hrResult = E_FAIL;
    HRESULT hrRetCode = E_FAIL;
    int nRetCode = false;
    FILE_LOAD_STATE eFileLoadState = FILE_LOAD_STATE_FAILED;
    IKG_Buffer *piMeshFile = NULL;
    IKG_Buffer *piBSPFile  = NULL;
    IIniFile   *piPropertyFile = NULL;

    // check first
    nRetCode = g_FileLoader.CheckResult(m_hMeshFileTask, &eFileLoadState);
    KGLOG_PROCESS_ERROR(nRetCode);
    KG_PROCESS_SUCCESS(eFileLoadState == FILE_LOAD_STATE_WAIT);
    KGLOG_PROCESS_ERROR(eFileLoadState != FILE_LOAD_STATE_FAILED);

    nRetCode = g_FileLoader.CheckResult(m_hBSPFileTask, &eFileLoadState);
    KGLOG_PROCESS_ERROR(nRetCode);
    KG_PROCESS_SUCCESS(eFileLoadState == FILE_LOAD_STATE_WAIT);
    //KGLOG_PROCESS_ERROR(eFileLoadState != FILE_LOAD_STATE_FAILED);

    nRetCode = g_FileLoader.CheckResult(m_hPropertyFileTask, &eFileLoadState);
    KGLOG_PROCESS_ERROR(nRetCode);
    KG_PROCESS_SUCCESS(eFileLoadState == FILE_LOAD_STATE_WAIT);
    //KGLOG_PROCESS_ERROR(eFileLoadState != FILE_LOAD_STATE_FAILED);

    // fetch result when all file was loaded
    nRetCode = g_FileLoader.FetchResultNormal(m_hMeshFileTask, &eFileLoadState, &piMeshFile);
    KGLOG_PROCESS_ERROR(nRetCode);
    KGLOG_PROCESS_ERROR(eFileLoadState == FILE_LOAD_STATE_SUCCESS);

    nRetCode = g_FileLoader.FetchResultNormal(m_hBSPFileTask, &eFileLoadState, &piBSPFile);
    KGLOG_PROCESS_ERROR(nRetCode);
    //KGLOG_PROCESS_ERROR(eFileLoadState == FILE_LOAD_STATE_SUCCESS);

    nRetCode = g_FileLoader.FetchResultIni(m_hPropertyFileTask, &eFileLoadState, &piPropertyFile);
    KGLOG_PROCESS_ERROR(nRetCode);
    //KGLOG_PROCESS_ERROR(eFileLoadState == FILE_LOAD_STATE_SUCCESS);

    hrRetCode = _LoadFromMemory(piMeshFile, piBSPFile, piPropertyFile, m_uOption);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

Exit1:
    hrResult = S_OK;
Exit0:
    KG_COM_RELEASE(piPropertyFile);
    KG_COM_RELEASE(piBSPFile);
    KG_COM_RELEASE(piMeshFile);
    if (FAILED(hrResult))
    {
        m_eDynamicLoadState = STATE_LOADFAILED;
    }
    if (m_eDynamicLoadState != STATE_MULTITHREAD_PREPEAR)
    {
        if (m_hPropertyFileTask)
        {
            g_FileLoader.DeleteTask(m_hPropertyFileTask);
            m_hPropertyFileTask = NULL;
        }
        if (m_hBSPFileTask)
        {
            g_FileLoader.DeleteTask(m_hBSPFileTask);
            m_hBSPFileTask = NULL;
        }
        if (m_hMeshFileTask)
        {
            g_FileLoader.DeleteTask(m_hMeshFileTask);
            m_hMeshFileTask = NULL;
        }
    }
    return hrResult;
}
////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DMesh::Helper_LoadMeshFromBuffer(
    KG3DBufferReader *pBufferReader, ReadInMeshData *pRetMeshData
)
{
	HRESULT hrResult = E_FAIL;
	HRESULT hrRetCode  = E_FAIL;
	int nRetCode = false;
	_MeshHead *pMeshHead = NULL;
    
    ASSERT(pBufferReader);
    ASSERT(pRetMeshData);

	nRetCode = pBufferReader->ReferenceRead((void **)&pMeshHead, sizeof(_MeshHead));
	KGLOG_PROCESS_ERROR(nRetCode);

	KG_PROCESS_ERROR(pMeshHead->dwNumVertices);
	KG_PROCESS_ERROR(pMeshHead->dwNumFaces);
	KG_PROCESS_ERROR(pMeshHead->dwNumSubset);

	pRetMeshData->dwNumVertices  = pMeshHead->dwNumVertices;
	pRetMeshData->dwNumFaces	 = pMeshHead->dwNumFaces;
	pRetMeshData->dwNumSubset	 = pMeshHead->dwNumSubset;
	m_dwNumSubset = pRetMeshData->dwNumSubset;

	//////////////////////////////////////////////////////////////////////////
	if (pMeshHead->Blocks.PositionBlock)
	{
		nRetCode = pBufferReader->Seek(pMeshHead->Blocks.PositionBlock, SEEK_SET);
		KGLOG_PROCESS_ERROR(nRetCode);

		nRetCode = pBufferReader->ReferenceRead(
			(void **)&(pRetMeshData->pPos), sizeof(D3DXVECTOR3) * pMeshHead->dwNumVertices
		);
		KGLOG_PROCESS_ERROR(nRetCode);

		pRetMeshData->dwMeshFVF |= D3DFVF_XYZ;
	}
	if (pMeshHead->Blocks.NormalBlock)
	{
		nRetCode = pBufferReader->Seek(pMeshHead->Blocks.NormalBlock, SEEK_SET);
		KGLOG_PROCESS_ERROR(nRetCode);

		nRetCode = pBufferReader->ReferenceRead(
			(void **)&(pRetMeshData->pNormals), sizeof(D3DXVECTOR3) * pMeshHead->dwNumVertices
		);
		KGLOG_PROCESS_ERROR(nRetCode);

		pRetMeshData->dwMeshFVF |= D3DFVF_NORMAL;
	}
	if (pMeshHead->Blocks.DiffuseBlock)
	{
		nRetCode = pBufferReader->Seek(pMeshHead->Blocks.DiffuseBlock, SEEK_SET);
		KGLOG_PROCESS_ERROR(nRetCode);

		nRetCode = pBufferReader->ReferenceRead(
			(void **)&(pRetMeshData->pDiffuse), sizeof(D3DXVECTOR3) * pMeshHead->dwNumVertices
		);
		KGLOG_PROCESS_ERROR(nRetCode);

		pRetMeshData->dwMeshFVF |= D3DFVF_DIFFUSE;
	}
	if (pMeshHead->Blocks.TextureUVW1Block)
	{
		nRetCode = pBufferReader->Seek(pMeshHead->Blocks.TextureUVW1Block, SEEK_SET);
		KGLOG_PROCESS_ERROR(nRetCode);

		nRetCode = pBufferReader->ReferenceRead(
			(void **)&(pRetMeshData->pUV1), sizeof(D3DXVECTOR3) * pMeshHead->dwNumVertices
		);
		KGLOG_PROCESS_ERROR(nRetCode);

		pRetMeshData->dwMeshFVF |= D3DFVF_TEX1;
	}
	if (pMeshHead->Blocks.TextureUVW2Block)
	{
		nRetCode = pBufferReader->Seek(pMeshHead->Blocks.TextureUVW2Block, SEEK_SET);
		KGLOG_PROCESS_ERROR(nRetCode);

		nRetCode = pBufferReader->ReferenceRead(
			(void **)&(pRetMeshData->pUV2), sizeof(D3DXVECTOR3) * pMeshHead->dwNumVertices
		);
		KGLOG_PROCESS_ERROR(nRetCode);

		pRetMeshData->dwMeshFVF |= D3DFVF_TEX2;
		pRetMeshData->dwMeshFVF &= ~(D3DFVF_TEX1);
	}
	if (pMeshHead->Blocks.TextureUVW3Block)
	{
		nRetCode = pBufferReader->Seek(pMeshHead->Blocks.TextureUVW3Block, SEEK_SET);
		KGLOG_PROCESS_ERROR(nRetCode);

		nRetCode = pBufferReader->ReferenceRead(
			(void **)&(pRetMeshData->pUV3), sizeof(D3DXVECTOR3) * pMeshHead->dwNumVertices
		);
		KGLOG_PROCESS_ERROR(nRetCode);

		pRetMeshData->dwMeshFVF |= D3DFVF_TEX3;
		pRetMeshData->dwMeshFVF &= ~(D3DFVF_TEX1 | D3DFVF_TEX2);
	}
	if (pMeshHead->Blocks.FacesIndexBlock)
	{
		nRetCode = pBufferReader->Seek(pMeshHead->Blocks.FacesIndexBlock, SEEK_SET);
		KGLOG_PROCESS_ERROR(nRetCode);

		nRetCode = pBufferReader->ReferenceRead(
			(void **)&(pRetMeshData->pFaceIndices), sizeof(DWORD) * 3 * pMeshHead->dwNumFaces
		);
		KGLOG_PROCESS_ERROR(nRetCode);
	}
	if (pMeshHead->Blocks.SubsetIndexBlock)
	{
		nRetCode = pBufferReader->Seek(pMeshHead->Blocks.SubsetIndexBlock, SEEK_SET);
		KGLOG_PROCESS_ERROR(nRetCode);

		nRetCode = pBufferReader->ReferenceRead(
			(void **)&(pRetMeshData->pSubsetIndices), sizeof(DWORD) * pMeshHead->dwNumFaces
		);
		KGLOG_PROCESS_ERROR(nRetCode);
	}
	if (pMeshHead->Blocks.SkinInfoBlock)
	{
		if (pMeshHead->Blocks.BBoxBlock)
		{
			m_dwOptionEx |= MESH_SAVE_BONEBBOX;
		}
		nRetCode = pBufferReader->Seek(pMeshHead->Blocks.SkinInfoBlock, SEEK_SET);
		KGLOG_PROCESS_ERROR(nRetCode);
		hrRetCode = _Helper_LoadSkinInfoFromBuffer(pBufferReader);
		KGLOG_COM_PROCESS_ERROR(hrRetCode);

		hrRetCode = Helper_LoadSocketFromBuffer(pBufferReader);
		KGLOG_COM_PROCESS_ERROR(hrRetCode);
	}

	m_BBox.Clear();
	m_BBox.AddPosition(pRetMeshData->pPos, pRetMeshData->dwNumVertices);

	hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KG3DMesh::_Helper_LoadSkinInfoFromBuffer(KG3DBufferReader *pBufferReader)
{
	HRESULT hrResult = E_FAIL;
	int nRetCode = 0;
	typedef char NAME_ARRAY[s_dwMaxChildBone][MAX_BONE_NAME_LENGTH];
	struct _CHILD_NAME
	{
		DWORD	   dwChildCount;
		NAME_ARRAY *pNames;
	};
	_CHILD_NAME *pChildNames = NULL;

	ASSERT(pBufferReader);

	m_dwNumBone = 0;
    KG_DELETE_ARRAY(m_pmatReverseMatrix);
    KG_DELETE_ARRAY(m_pBoneInfo);

	nRetCode = pBufferReader->Read(&m_dwNumBone, sizeof(DWORD));
	KGLOG_PROCESS_ERROR(nRetCode);
	KGLOG_PROCESS_ERROR(m_dwNumBone < s_dwMaxBone);
	KG_PROCESS_SUCCESS(m_dwNumBone == 0);

    m_pmatReverseMatrix = new D3DXMATRIX[m_dwNumBone];
    KG_PROCESS_ERROR(m_pmatReverseMatrix);

    m_pBoneInfo = new BoneInfo1[m_dwNumBone];
    KG_PROCESS_ERROR(m_pBoneInfo);

	pChildNames = new _CHILD_NAME[m_dwNumBone];
	KGLOG_PROCESS_ERROR(pChildNames);
	//////////////////////////////////////////////////////////////////////////
	for (DWORD i = 0; i < m_dwNumBone; i++)
	{
		//Read bone name
		ASSERT(sizeof(m_pBoneInfo[i].strBoneName) >= sizeof(TCHAR) * MAX_BONE_NAME_LENGTH);
		nRetCode = pBufferReader->Read(m_pBoneInfo[i].strBoneName, sizeof(TCHAR) * MAX_BONE_NAME_LENGTH);
		KGLOG_PROCESS_ERROR(nRetCode);
		m_pBoneInfo[i].strBoneName[countof(m_pBoneInfo[i].strBoneName)-1] = '\0';
		strlwr(m_pBoneInfo[i].strBoneName);

		//seek parent name, child info is enough to build the skeleton
		nRetCode = pBufferReader->Seek(sizeof(TCHAR) * MAX_BONE_NAME_LENGTH, SEEK_CUR);
		KGLOG_PROCESS_ERROR(nRetCode);

		// child bone
		nRetCode = pBufferReader->Read(&(pChildNames[i].dwChildCount), sizeof(DWORD));
		KGLOG_PROCESS_ERROR(nRetCode);

		nRetCode = pBufferReader->ReferenceRead(
			(void **)&(pChildNames[i].pNames), sizeof(TCHAR) * MAX_BONE_NAME_LENGTH * pChildNames[i].dwChildCount
		);
		KGLOG_PROCESS_ERROR(nRetCode);

		//
		nRetCode = pBufferReader->Read(&m_pmatReverseMatrix[i], sizeof(D3DXMATRIX)); // MatrixOffset
		KGLOG_PROCESS_ERROR(nRetCode);

		nRetCode = pBufferReader->Seek(sizeof(D3DXMATRIX), SEEK_CUR);				// MatrixOffsetBase_NoUse
		KGLOG_PROCESS_ERROR(nRetCode);

		nRetCode = pBufferReader->Read(&m_pBoneInfo[i].NumVerticesRef, sizeof(DWORD));
		KGLOG_PROCESS_ERROR(nRetCode);

		ASSERT(!m_pBoneInfo[i].VerticesIndexRef);
		m_pBoneInfo[i].VerticesIndexRef = new DWORD[m_pBoneInfo[i].NumVerticesRef];
		KGLOG_PROCESS_ERROR(m_pBoneInfo[i].VerticesIndexRef);

		ASSERT(!m_pBoneInfo[i].VerticesWeightRef);
		m_pBoneInfo[i].VerticesWeightRef = new float[m_pBoneInfo[i].NumVerticesRef];
		KGLOG_PROCESS_ERROR(m_pBoneInfo[i].VerticesWeightRef);

		nRetCode = pBufferReader->Read(m_pBoneInfo[i].VerticesIndexRef, sizeof(DWORD) * m_pBoneInfo[i].NumVerticesRef);
		KGLOG_PROCESS_ERROR(nRetCode);

		nRetCode  = pBufferReader->Read(m_pBoneInfo[i].VerticesWeightRef, sizeof(float) * m_pBoneInfo[i].NumVerticesRef);
		KGLOG_PROCESS_ERROR(nRetCode);

		D3DXMatrixInverse(&m_pBoneInfo[i].CurMatrix, NULL, &m_pmatReverseMatrix[i]);

		if (m_dwOptionEx & MESH_SAVE_BONEBBOX)
		{
			nRetCode = pBufferReader->Read(&m_pBoneInfo[i].BBoxScaleMatrix, sizeof(D3DXMATRIX));
			KGLOG_PROCESS_ERROR(nRetCode);
			nRetCode = pBufferReader->Read(&m_pBoneInfo[i].bBox.A, sizeof(D3DXVECTOR3));
			KGLOG_PROCESS_ERROR(nRetCode);
			nRetCode = pBufferReader->Read(&m_pBoneInfo[i].bBox.B, sizeof(D3DXVECTOR3));
			KGLOG_PROCESS_ERROR(nRetCode);
			nRetCode = pBufferReader->Read(&m_pBoneInfo[i].bBox.bInit, sizeof(BOOL));
			KGLOG_PROCESS_ERROR(nRetCode);
		}
	}

	// -------------------------- Build Skeleton --------------------------
	for (DWORD i = 0; i < m_dwNumBone; i++)
	{
		m_pBoneInfo[i].dwNumChild = pChildNames[i].dwChildCount;
		for (size_t j = 0; j < pChildNames[i].dwChildCount; ++j)
		{
			const char *pszChileName = (*(pChildNames[i].pNames))[j];
			int nChildIndex = FindBone(pszChileName);
			KGLOG_PROCESS_ERROR((nChildIndex >= 0) && "Can't find child bone");

			m_pBoneInfo[i].dwChildIndex[j] = nChildIndex;
			m_pBoneInfo[nChildIndex].dwParentIndex = i;
		}
	}
	for (DWORD i = 0; i < m_dwNumBone; i++)
	{
		if (m_pBoneInfo[i].dwParentIndex == s_dwInvalidateIndex)
		{
			m_nBaseBoneID.push_back(i);
		}
	}

	m_dwOption |= MESH_SKINED;
Exit1:
	hrResult = S_OK;
Exit0:
	KG_DELETE_ARRAY(pChildNames);
	if (FAILED(hrResult))
	{
        KG_DELETE_ARRAY(m_pmatReverseMatrix);
        KG_DELETE_ARRAY(m_pBoneInfo);
		m_dwNumBone = 0;
	}
	return hrResult;
}

HRESULT KG3DMesh::Helper_LoadSocketFromBuffer(KG3DBufferReader *pBufferReader)
{
	HRESULT hrResult = E_FAIL;
	int nRetCode = false;
	TCHAR *pszParentBone = NULL;

	ASSERT(pBufferReader);

	nRetCode = pBufferReader->Read(&m_dwNumSocket, sizeof(DWORD));
	KGLOG_PROCESS_ERROR(nRetCode);

    m_pSockets = new Socket1[m_dwNumSocket];
    KG_PROCESS_ERROR(m_pSockets);

	for (DWORD i = 0; i < m_dwNumSocket; i++)
	{
		ASSERT(sizeof(m_pSockets[i].strSocketName) >= sizeof(TCHAR) * MAX_BONE_NAME_LENGTH);
		nRetCode = pBufferReader->Read(m_pSockets[i].strSocketName, sizeof(TCHAR) * MAX_BONE_NAME_LENGTH);
		KGLOG_PROCESS_ERROR(nRetCode);
		m_pSockets[i].strSocketName[countof(m_pSockets[i].strSocketName) - 1] = '\0';
		strlwr(m_pSockets[i].strSocketName);

		nRetCode = pBufferReader->ReferenceRead((void **)&pszParentBone, sizeof(TCHAR) * MAX_BONE_NAME_LENGTH);
		KGLOG_PROCESS_ERROR(nRetCode);

		nRetCode = pBufferReader->Read(&m_pSockets[i].matMatrixOffset, sizeof(D3DXMATRIX));
		KGLOG_PROCESS_ERROR(nRetCode);

		int nParentIndex = FindBone(pszParentBone);
		KGLOG_PROCESS_ERROR((nParentIndex >= 0) && "can't find parent bone for socket");

		m_pSockets[i].dwParentBoneIndex = nParentIndex;
		D3DXMatrixMultiply(&m_pSockets[i].matCurMatrix, &m_pSockets[i].matMatrixOffset, &m_pBoneInfo[nParentIndex].CurMatrix);
	}

	hrResult = S_OK;
Exit0:
	if (FAILED(hrResult))
	{
        KG_DELETE_ARRAY(m_pSockets);
		m_dwNumSocket = 0;
	}
	return hrResult;
}

HRESULT KG3DMesh::CreateBBoxOfBone()
{
	HRESULT hr = E_FAIL;
	if(!m_ppMeshes[SMBT_NORMAL])
		return E_FAIL;

	BYTE* pVer = NULL;

	hr = m_ppMeshes[SMBT_NORMAL]->LockVertexBuffer(0, (void**)&pVer);
	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);
	for(DWORD dwBoneIndex=0;dwBoneIndex< m_dwNumBone;dwBoneIndex++)
	{
		BoneInfo1& BoneInfo = m_pBoneInfo[dwBoneIndex];
		BoneInfo.bBox.Clear();
		for (DWORD dwVerNum = 0 ; dwVerNum < BoneInfo.NumVerticesRef ; dwVerNum++)
		{
			if ( BoneInfo.VerticesWeightRef[dwVerNum] > g_fBoneWeight)
			{
				D3DXVECTOR3* Pos = (D3DXVECTOR3*)(&pVer[BoneInfo.VerticesIndexRef[dwVerNum] * m_dwVertexStride]);
				BoneInfo.bBox.AddPosition(*Pos);
			}

		}
	}
	m_dwOptionEx |= MESH_SAVE_BONEBBOX; //保存Bone包围合
Exit0:
	hr = m_ppMeshes[SMBT_NORMAL]->UnlockVertexBuffer();
	return hr;
}
////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DMesh::RefreshOption(unsigned uOption)
{
	HRESULT hrResult = E_FAIL;
	HRESULT hrRetCode  = E_FAIL;

	KG_PROCESS_SUCCESS(m_eDynamicLoadState == STATE_VIDEOMEM);
	KG_PROCESS_SUCCESS(uOption == m_uOption);

	//在加载为立即加载的时候
	if ((uOption & MLO_MULTI) == 0)
	{
		switch (m_eDynamicLoadState)
		{
		//什么都还没有做
		case STATE_NONE:
            hrRetCode = LoadFromFile(m_scName.c_str(), 0, uOption);
            KGLOG_COM_PROCESS_ERROR(hrRetCode);
            break;

		case STATE_LOADFAILED:
            KG_PROCESS_ERROR(false);
			break;
		//正在动态加载
		case STATE_MULTITHREAD_PREPEAR:
			{
				while (m_eDynamicLoadState == STATE_MULTITHREAD_PREPEAR)
                {
                    Sleep(1); // wait
                    hrRetCode = CheckLoadingState();
                    //KGLOG_COM_PROCESS_ERROR(hrRetCode);
                }
                KGLOG_PROCESS_ERROR(m_eDynamicLoadState == STATE_VIDEOMEM);
				goto Exit1;
				break;
			}
		default:
			assert(0);
		}
	}
	m_uOption = uOption;
Exit1:
	hrResult = S_OK;
Exit0:
	return hrResult;
}
////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DMesh::_LoadBSPFileFromMemory(IKG_Buffer *piBSPFile)
{
	HRESULT hrResult  = E_FAIL;
    HRESULT hrRetCode = E_FAIL;
    DWORD dwBSPVertexCount = 0;
    DWORD dwBSPFaceCount = 0;
    DWORD dwMeshVerticesCount = 0;
    DWORD dwMeshFacesCount = 0;
    LPD3DXMESH piMesh = m_ppMeshes[SMBT_NORMAL];
    KG3DBsp *pBSP = NULL;
   
    ASSERT(piBSPFile);

    KGLOG_PROCESS_ERROR(piMesh);
    dwMeshFacesCount    = piMesh->GetNumFaces();
    dwMeshVerticesCount = piMesh->GetNumVertices();

    pBSP = new KG3DBsp;
    KGLOG_PROCESS_ERROR(pBSP);

	hrRetCode = pBSP->LoadFromMemory(piBSPFile);
	KGLOG_COM_PROCESS_ERROR(hrRetCode);

    hrRetCode = pBSP->GetMeshInfo(&dwBSPVertexCount, &dwBSPFaceCount);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    if ((dwBSPVertexCount != dwMeshVerticesCount) || (dwBSPFaceCount != dwMeshFacesCount))
    {
        KGLogPrintf(KGLOG_ERR, "%s, BSP unmatch Mesh :(", m_scName.c_str());
        KG_PROCESS_ERROR(FALSE);
    }

    ASSERT(!m_lpBsp);
    m_lpBsp = pBSP;
    pBSP = NULL;
	hrResult = S_OK;
Exit0:
    KG_DELETE(pBSP);
	if(FAILED(hrResult))
	{
		KGLogPrintf(KGLOG_ERR, "%s BSP加载失败", m_scName.c_str());
	}
	return hrResult;
}
////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DMesh::CreateBspFile()
{
    HRESULT hrResult = E_FAIL;
    HRESULT hrRetCode = E_FAIL;
    int nRetCode = false;
    TCHAR szBSPPathName[MAX_PATH];
    void *pvVerticesBuffer = NULL;
    WORD* pIndexBuffer    = NULL;
    D3DXVECTOR3 *pPos = NULL;
    DWORD *pdwFaceIndex = NULL;
    DWORD dwStride = 0;
    DWORD i = 0;
    DWORD dwNumFaces = 0;
    DWORD dwNumVertices = 0;
    DWORD dwStartTime = timeGetTime();
    LPD3DXMESH piMesh = m_ppMeshes[SMBT_NORMAL];
    KG3DBsp *pBSP = NULL;

    KGLOG_PROCESS_ERROR(piMesh);

    dwStride      = piMesh->GetNumBytesPerVertex();
    dwNumVertices = piMesh->GetNumVertices();
    dwNumFaces    = piMesh->GetNumFaces();

    KG_PROCESS_SUCCESS(dwNumFaces < 256);

    pPos = new D3DXVECTOR3[dwNumVertices];
    KG_ASSERT_EXIT(pPos);

    pdwFaceIndex = new DWORD[dwNumFaces * 3];
    KG_ASSERT_EXIT(pdwFaceIndex);

    hrRetCode = piMesh->LockVertexBuffer(D3DLOCK_READONLY, (void **)&pvVerticesBuffer);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    hrRetCode = piMesh->LockIndexBuffer(D3DLOCK_READONLY, (void **)&pIndexBuffer);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    for (i = 0; i < dwNumVertices; ++i)
    {
        pPos[i] = *(D3DXVECTOR3 *)(((BYTE *)pvVerticesBuffer) + dwStride * i);
    }
    for (i = 0; i < dwNumFaces * 3; ++i)
    {
        pdwFaceIndex[i] = pIndexBuffer[i];
    }

    // -------------------------- create BSP --------------------------

    hrRetCode = ChangePathExtName(m_scName.c_str(), "bsp", sizeof(szBSPPathName), szBSPPathName);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    pBSP = new KG3DBsp;
    KGLOG_PROCESS_ERROR(pBSP);

    hrRetCode = pBSP->CreateFromMesh(dwNumVertices, dwNumFaces, pPos, pdwFaceIndex);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    hrRetCode = pBSP->SaveToFile(szBSPPathName);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    DWORD dwCost = timeGetTime() - dwStartTime;
    if(dwCost > 500)
    {
        KGLogPrintf(
            KGLOG_WARNING, "BSP %d %d Face %s",
            dwCost, dwNumFaces, szBSPPathName
        );
    }

    KG_DELETE(m_lpBsp); // recreate
    m_lpBsp = pBSP;
    pBSP = NULL;
Exit1:
    hrResult = S_OK;
Exit0:
    KG_DELETE(pBSP);
    if (pIndexBuffer)
    {
        piMesh->UnlockIndexBuffer();
        pIndexBuffer = NULL;
    }
    if (pvVerticesBuffer)
    {
        piMesh->UnlockVertexBuffer();
        pvVerticesBuffer = NULL;
    }
    KG_DELETE_ARRAY(pdwFaceIndex);
    KG_DELETE_ARRAY(pPos);

    if(FAILED(hrResult))
    {
        KGLogPrintf(KGLOG_ERR, "%s 创建失败", szBSPPathName);
    }
    return hrResult;
}

HRESULT  KG3DMesh::_LoadPropertyFromIni(IIniFile *piPropertyFile)
{
    HRESULT hrResult = E_FAIL;
    HRESULT hrRetCode = E_FAIL;

    ASSERT(piPropertyFile);

    // ==================显示相关(Display)================
    piPropertyFile->GetInteger(
        TEXT("Display"), TEXT("bAutoProduceObstacle"), 
        m_objPropertys.bAutoProduceObstacle,
        &m_objPropertys.bAutoProduceObstacle
    );
    piPropertyFile->GetInteger(
        TEXT("Display"), TEXT("bObscatleCamera"), 
        m_objPropertys.bObscatleCamera,
        &m_objPropertys.bObscatleCamera
    );

    m_objPropertys.bInitallized = TRUE;

    hrResult = S_OK;
Exit0:
    return hrResult;
}

