#include "StdAfx.h"
#include "KG3DTerrainBlockGrass.h"
#include "KG3DTemplates.h"
#include "KG3DMesh.h"
#include "KG3DMeshTable.h"
#include "KG3DTexture.h"
#include "KG3DTextureTable.h"
#include "KG3DMemoryFile.h"
#include "KG3DSceneOutDoorSpaceMgr.h"
#include "KG3DModel.h"
#include "KG3DSubSetRenderer.h"
#include "KG3DFileFinder.h"
#include "KG3DTerrainBlock.h"
#include "KG3DTerrainBlockModifier.h"

TKG3DResourcePool<KG3DTerrainBlockGrass> g_cTerrainBlockGrassPool;
TKG3DResourcePool<KG3DTerrainBlockGrassShareVB_High> g_cTerrainBlockGrassShareVBHighPool;

//////////////////////////////////////////////////////////////////////////
struct KG3DTerrainGrassVertexInputBuffer
{
	KG3DTerrainGrassVertex *pGrassVertexBuffer;//[MAX_GRASSVERTEX];	
	DWORD                  dwSize;
	KG3DTerrainGrassVertexInputBuffer();
	~KG3DTerrainGrassVertexInputBuffer();
	HRESULT CheckSize(DWORD dwSize);
};

KG3DTerrainGrassVertexInputBuffer::KG3DTerrainGrassVertexInputBuffer():
pGrassVertexBuffer(NULL),
dwSize(0)
{
}

KG3DTerrainGrassVertexInputBuffer::~KG3DTerrainGrassVertexInputBuffer()
{
	KG_DELETE_ARRAY(pGrassVertexBuffer);
	dwSize = 0;
}

HRESULT KG3DTerrainGrassVertexInputBuffer::CheckSize(DWORD dwSize)
{
	HRESULT hRetCode = E_FAIL;

	if (this->dwSize < dwSize)
	{
        KG_DELETE_ARRAY(pGrassVertexBuffer);
		this->dwSize = 0;
	}

	if (!pGrassVertexBuffer)
	{
        pGrassVertexBuffer = new KG3DTerrainGrassVertex[dwSize];
        KG_PROCESS_ERROR(pGrassVertexBuffer);

        this->dwSize = dwSize;
	}

	hRetCode = S_OK;
Exit0:
	return hRetCode;
}

KG3DTerrainGrassVertexInputBuffer g_TerrainGrassVertexInputBuffer;
extern int g_nLoadCount;
int g_nGrassVertexBufferFillPerFrame = 0;

HRESULT KG3DTerrainGrassPattern::Init()
{
	return S_OK;
}

HRESULT KG3DTerrainGrassPattern::UnInit()
{
	SAFE_DELETE_ARRAY(m_lpVertexBuffer);
	m_dwNumFace = 0;
	m_dwNumVertex = 0;
	return S_OK;
}

KG3DTerrainGrassPattern::KG3DTerrainGrassPattern()
{
	m_lpVertexBuffer = NULL;
	m_dwNumFace = 0;
	m_dwNumVertex = 0;
}

KG3DTerrainGrassPattern::~KG3DTerrainGrassPattern()
{
	UnInit();
}

D3DCOLOR CompressNormal(const D3DXVECTOR3 &vNormal)
{
    D3DCOLOR dwNormalCompressed = 0;

    D3DXVECTOR3 vTemp = (vNormal + D3DXVECTOR3(1.0f, 1.0f, 1.0f)) / 2;

    dwNormalCompressed = 
        D3DCOLOR_XRGB((DWORD(vTemp.x * 255)), (DWORD(vTemp.y * 255)), (DWORD(vTemp.z * 255)));

    return dwNormalCompressed;
}

void DecompressNormal(DWORD dwNormal, D3DXVECTOR3 *pRetNormal)
{
    ASSERT(pRetNormal);

    pRetNormal->x = D3DCOLOR_GETRED_FLOAT(dwNormal);
    pRetNormal->y = D3DCOLOR_GETGREEN_FLOAT(dwNormal);
    pRetNormal->z = D3DCOLOR_GETBLUE_FLOAT(dwNormal);

    *pRetNormal = (*pRetNormal) * 2 - D3DXVECTOR3(1.0f, 1.0f, 1.0f);
}

D3DCOLOR CompressUV(D3DXVECTOR3 vUV)
{
    D3DCOLOR dwUVCompressed = 0;
    
    dwUVCompressed = 
        D3DCOLOR_XRGB((DWORD(vUV.x * 255)), (DWORD(vUV.y * 255)), (DWORD(vUV.z * 255)));

    return dwUVCompressed;
}

void DecompressUV(D3DCOLOR dwUV, D3DXVECTOR3 *pRetUV)
{
    ASSERT(pRetUV);

    pRetUV->x = D3DCOLOR_GETRED_FLOAT(dwUV);
    pRetUV->y = D3DCOLOR_GETGREEN_FLOAT(dwUV);
    pRetUV->z = D3DCOLOR_GETBLUE_FLOAT(dwUV);
}

HRESULT KG3DTerrainGrassPattern::CreateFromMesh(LPCTSTR pFileName)
{
	UnInit();

	HRESULT hr = S_OK;
	KG3DMesh* pMesh = NULL;
	ID3DXMesh* p3DMesh = NULL;
	KG3DTerrainGrassVertexSrc* pSrcVertex = NULL;
	WORD* pSrcIndex = NULL;

	hr = g_cMeshTable.LoadResourceFromFile(pFileName,0,0,(IKG3DResourceBase**)&pMesh);
	KGLOG_COM_PROCESS_ERROR(hr);
	
	p3DMesh = pMesh->m_ppMeshes[KG3DMesh::SMBT_NORMAL];
	KGLOG_PROCESS_ERROR(p3DMesh);

	hr = p3DMesh->LockVertexBuffer(0, (void**)&pSrcVertex);
	KGLOG_COM_PROCESS_ERROR(hr);

	hr = p3DMesh->LockIndexBuffer(0, (void**)&pSrcIndex);
	KGLOG_COM_PROCESS_ERROR(hr);

	m_dwNumFace = p3DMesh->GetNumFaces();
	KGLOG_PROCESS_ERROR(m_dwNumFace);

	m_dwNumVertex = m_dwNumFace * 3;
	KGLOG_PROCESS_ERROR(m_dwNumVertex);

	m_lpVertexBuffer = new KG3DTerrainGrassVertex[m_dwNumVertex];
	KGLOG_PROCESS_ERROR(m_lpVertexBuffer);

	////////////////////////////////////////////////////////
	//求模版模型的最大和最小高度
	float max_y = 0.0f;
	float min_y = 0.0f;
	for (DWORD i = 0; i < m_dwNumFace * 3; i++)
	{
		WORD idx = pSrcIndex[i];
		float h = pSrcVertex[idx].vPos.y;
		if(i == 0)
		{
			max_y = h;
			min_y = h;
		}
		else
		{
			if(h > max_y)
				max_y = h;
			if(h < min_y)
				min_y = h;
		}
	}
	float deltaH = max_y - min_y;

	///////////////////////////////////////////////////////

	for (DWORD i = 0; i < m_dwNumFace; i++)
	{
		WORD A = pSrcIndex[i*3  ];
		WORD B = pSrcIndex[i*3+1];
		WORD C = pSrcIndex[i*3+2];
        D3DXVECTOR3 vN;

		m_lpVertexBuffer[i*3  ].vPos = pSrcVertex[A].vPos;
        m_lpVertexBuffer[i*3  ].dwNormal = CompressNormal(pSrcVertex[A].vNormal);
        vN.x = pSrcVertex[A].vUV.x; 
        vN.y = pSrcVertex[A].vUV.y;
		vN.z = 1.0f - (pSrcVertex[A].vPos.y - min_y) / deltaH;  //由高度插值决定摆幅 by likan
        m_lpVertexBuffer[i*3 ].dwUV     = CompressUV(vN);

		m_lpVertexBuffer[i*3+1].vPos    = pSrcVertex[B].vPos;
        m_lpVertexBuffer[i*3+1].dwNormal = CompressNormal(pSrcVertex[B].vNormal);
        vN.x = pSrcVertex[B].vUV.x; 
        vN.y = pSrcVertex[B].vUV.y;
		vN.z = 1.0f - (pSrcVertex[B].vPos.y - min_y) / deltaH;
        m_lpVertexBuffer[i*3+1].dwUV     = CompressUV(vN);

		m_lpVertexBuffer[i*3+2].vPos    = pSrcVertex[C].vPos;
		m_lpVertexBuffer[i*3+2].dwNormal = CompressNormal(pSrcVertex[C].vNormal);
        vN.x = pSrcVertex[C].vUV.x; 
        vN.y = pSrcVertex[C].vUV.y;
		vN.z = 1.0f - (pSrcVertex[C].vPos.y - min_y) / deltaH;
        m_lpVertexBuffer[i*3+2].dwUV     = CompressUV(vN);
	}

	m_szName = pFileName;
	hr = S_OK;
Exit0:
	if (p3DMesh)
	{
		hr = p3DMesh->UnlockIndexBuffer();
		hr = p3DMesh->UnlockVertexBuffer();
	}

	SAFE_RELEASE(pMesh);
	return hr;
}

//////////////////////////////////////////////////////////////////////////
HRESULT KG3DTerrainGrassSet::Init()
{
	return S_OK;
}

HRESULT KG3DTerrainGrassSet::UnInit()
{
	for (size_t i=0;i<m_vecGrassPattern.size();i++)
	{
		KG3DTerrainGrassPattern* pPattern = m_vecGrassPattern[i];
		SAFE_DELETE(pPattern);
	}
	m_vecGrassPattern.clear();

	SAFE_RELEASE(m_lpGrassTexture);
	return S_OK;
}

KG3DTerrainGrassSet::KG3DTerrainGrassSet()
{
	m_dwStrideX = 1;
	m_dwStrideY = 1;
	m_lpGrassTexture = NULL;

	m_ulRefCount = 0;
}

KG3DTerrainGrassSet::~KG3DTerrainGrassSet()
{
	UnInit();
}

HRESULT KG3DTerrainGrassSet::LoadPattern(LPCTSTR pFileName,int& nIndex)
{
	HRESULT hr = E_FAIL;
	KG3DTerrainGrassPattern* pNewPattern = NULL;
	TCHAR szRelativePath[MAX_PATH];

	wsprintf(szRelativePath,"%s", GetRetPath(pFileName));
	for (size_t i=0;i<m_vecGrassPattern.size();i++)
	{
		KG3DTerrainGrassPattern* pPattern = m_vecGrassPattern[i];
		if (_strcmpi(pPattern->m_szName.c_str(),szRelativePath)==0)
		{
			nIndex = (int)i;
			return S_OK;
		}
	}

	pNewPattern = new KG3DTerrainGrassPattern;
	KGLOG_PROCESS_ERROR(pNewPattern);

	hr = pNewPattern->CreateFromMesh(szRelativePath);
	KGLOG_COM_PROCESS_ERROR(hr);

	nIndex = (int)m_vecGrassPattern.size();
	m_vecGrassPattern.push_back(pNewPattern);

	return S_OK;
Exit0:

	SAFE_DELETE(pNewPattern);
	return E_FAIL;
}

KG3DTerrainGrassPattern* KG3DTerrainGrassSet::FindPattern(int nIndex)
{
	KG_PROCESS_ERROR(nIndex>=0);
	KG_PROCESS_ERROR(nIndex<(int)m_vecGrassPattern.size());

	return m_vecGrassPattern[nIndex];
Exit0:
	return NULL;
}

ULONG KG3DTerrainGrassSet::AddRef()
{
    return (ULONG)KG_InterlockedIncrement((long *)&m_ulRefCount);
}

ULONG KG3DTerrainGrassSet::Release()
{
    ULONG uNewRefCount = (ULONG)KG_InterlockedDecrement((long *)&m_ulRefCount);
    if (uNewRefCount == 0)
        delete this; 
    return uNewRefCount;
}

HRESULT KG3DTerrainGrassSet::SetTexture(LPCSTR strFileName)
{
	HRESULT hr = E_FAIL;

	VERIFY_RELEASE(m_lpGrassTexture);

	hr = g_cTextureTable.LoadResourceFromFile(strFileName, 
		0, 
		TEXOP_RELOAD, 
		(IKG3DResourceBase **)&m_lpGrassTexture);
	KG_COM_PROCESS_ERROR(hr);

	m_strTextureFileName = strFileName;
Exit0:
	return hr;
}

HRESULT KG3DTerrainGrassSet::SaveToInformationBuffer(DWORD& dwLength,KG3DMemoryFile* pFile)
{
	//pFile->Reset();
	DWORD dwVersion = 0;
	pFile->write(&dwVersion,sizeof(DWORD));
	pFile->write(&m_dwStrideX,sizeof(DWORD));
	pFile->write(&m_dwStrideY,sizeof(DWORD));
	CHAR szName[MAX_PATH];
		
	wsprintf(szName,"%s", GetRetPath(m_strTextureFileName.c_str()));
	//wsprintf(szName,"%s", m_strTextureFileName.c_str());
	pFile->write(szName,sizeof(szName));

	DWORD NumPattern = (DWORD)m_vecGrassPattern.size();
	pFile->write(&NumPattern,sizeof(DWORD));

	for (DWORD i=0;i<NumPattern;i++)
	{
		KG3DTerrainGrassPattern* pPattern = m_vecGrassPattern[i];
		if(pPattern)
		{
			//wsprintf(szName,"%s", pPattern->m_szName.c_str());
			wsprintf(szName,"%s", GetRetPath(pPattern->m_szName.c_str()));
		}
		else
		{
			wsprintf(szName,"");
		}
		pFile->write(szName,sizeof(szName));
	}

	dwLength = pFile->GetLength();
	return S_OK;
} 

HRESULT KG3DTerrainGrassSet::LoadFromInformationBuffer(DWORD dwLength,KG3DMemoryFile* pFile)
{
	UnInit();

	//pFile->Reset();
	DWORD dwVersion = 0;
	pFile->read(&dwVersion,sizeof(DWORD));
	pFile->read(&m_dwStrideX,sizeof(DWORD));
	pFile->read(&m_dwStrideY,sizeof(DWORD));
	CHAR szName[MAX_PATH];
	pFile->read(szName,sizeof(szName));
	SetTexture(szName);

	DWORD NumPattern = 0;
	pFile->read(&NumPattern,sizeof(DWORD));

	for (DWORD i=0;i<NumPattern;i++)
	{
		pFile->read(szName,sizeof(szName));
		int nIndex = 0;
		LoadPattern(szName,nIndex);
	}
	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
KG3DTerrainBlockGrassShareVB_High::KG3DTerrainBlockGrassShareVB_High()
{
	m_pVB = NULL;
	m_dwNumFacesTotal = 0;
	m_dwUsedFaces = 0;
	dwLastTime = 0;
	pUser = NULL;
	m_bUserLock = FALSE;
}

KG3DTerrainBlockGrassShareVB_High::~KG3DTerrainBlockGrassShareVB_High()
{
	UnInit();

	SAFE_RELEASE(m_pVB);
	m_dwNumFacesTotal = 0;
	m_dwUsedFaces = 0;
}


HRESULT KG3DTerrainBlockGrassShareVB_High::CheckSize(DWORD dwFace)
{
    HRESULT hResult = E_FAIL;
	HRESULT hr = E_FAIL;

	if (dwFace > m_dwNumFacesTotal)
	{
		UnInit();

        SAFE_RELEASE(m_pVB);
        m_dwNumFacesTotal = 0;

		DWORD dwCreateFace = max(dwFace,(DWORD)512);

        hr = g_pd3dDevice->CreateVertexBuffer(dwCreateFace * 3 * sizeof(KG3DTerrainGrassVertex),
            D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &m_pVB, NULL);
        KGLOG_COM_PROCESS_ERROR(hr);

		m_dwNumFacesTotal = dwCreateFace;
	}
	
	m_dwUsedFaces = dwFace;

    hResult = S_OK;
Exit0:
	return hResult;
}

HRESULT KG3DTerrainBlockGrassShareVB_High::Create(KG3DTerrainBlockGrass* pBlock)
{
	HRESULT hr = S_OK;

	if (pBlock)
	{
		DWORD dwFace = pBlock->GetTotalGrassFaceCount();

		if(dwFace)
		{
			hr = CheckSize(dwFace);
			KGLOG_COM_PROCESS_ERROR(hr);

			if( dwFace*3 < MAX_GRASSVERTEX )
			{
				hr = g_TerrainGrassVertexInputBuffer.CheckSize(dwFace * 3);
				KG_COM_PROCESS_ERROR(hr);
				ASSERT(g_TerrainGrassVertexInputBuffer.pGrassVertexBuffer);

				DWORD dwRealFaceCount = 0;
				hr = FillGrassVertex(dwRealFaceCount,pBlock, g_TerrainGrassVertexInputBuffer.pGrassVertexBuffer);
				KGLOG_COM_PROCESS_ERROR(hr);
				ASSERT(dwFace == dwRealFaceCount);
				m_dwUsedFaces = dwRealFaceCount;
				//////////////////////////////////////////////////////////////////////////
				KGLOG_PROCESS_ERROR(m_pVB);

				KG3DTerrainGrassVertex* pDest = NULL;
				hr = m_pVB->Lock(0,dwFace * 3 * sizeof(KG3DTerrainGrassVertex),(void**)&pDest,0);
				KGLOG_COM_PROCESS_ERROR(hr);

				memcpy(pDest, 
					g_TerrainGrassVertexInputBuffer.pGrassVertexBuffer,
					dwFace * 3 * sizeof(KG3DTerrainGrassVertex));

				hr = m_pVB->Unlock();
				KGLOG_COM_PROCESS_ERROR(hr);
			}
			else
			{
				ASSERT(0);
			}
		}
		else
		{
			m_dwUsedFaces = 0;
		}
	}
	
	dwLastTime = timeGetTime();
	pUser = pBlock;
	m_bOkToRender = TRUE;

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DTerrainBlockGrassShareVB_High::FillXMesh(DWORD& dwPosition,LPD3DXMESH pXMesh,D3DXMATRIX* pMat)
{
	HRESULT hr = S_OK;
    HRESULT hrResult = S_OK;
	BOOL bIndexBufferLocked = FALSE;
    BOOL bVertexBufferLocked = FALSE;
	DWORD dwFace = 0;
	DWORD dwFVF = 0;
    DWORD dwVertexSize = 0;
	WORD* pIndex = NULL;
	BYTE* pVertexData = NULL;
	KG3DTerrainGrassVertex *pGrassInputBuffer = NULL;
    D3DXVECTOR3 vTemp;
    
	KGLOG_PROCESS_ERROR(pXMesh);
	KGLOG_PROCESS_ERROR(pMat);

	pGrassInputBuffer = g_TerrainGrassVertexInputBuffer.pGrassVertexBuffer;
	KG_PROCESS_ERROR(pGrassInputBuffer);
	ASSERT(dwPosition < g_TerrainGrassVertexInputBuffer.dwSize);
	ASSERT((dwPosition + 3) < g_TerrainGrassVertexInputBuffer.dwSize);

	dwFace = pXMesh->GetNumFaces();
	dwFVF  = pXMesh->GetFVF();
	KGLOG_PROCESS_ERROR(dwFVF == D3DFVF_Faces_Normal_Texture1 || dwFVF == D3DFVF_Faces_Normal_Texture2);

	dwVertexSize = pXMesh->GetNumBytesPerVertex();

    hr = pXMesh->LockIndexBuffer(0,(void**)&pIndex);
	KGLOG_COM_PROCESS_ERROR(hr);

	bIndexBufferLocked = TRUE;

	hr = pXMesh->LockVertexBuffer(0,(void**)&pVertexData);
	KGLOG_COM_PROCESS_ERROR(hr);

    bVertexBufferLocked = TRUE;

	for (DWORD iFace = 0;iFace<dwFace;iFace++)
	{
		WORD nA = pIndex[iFace*3  ];
		WORD nC = pIndex[iFace*3+1];
		WORD nB = pIndex[iFace*3+2];

		VFormat::FACES_NORMAL_TEXTURE1* pVerA = (VFormat::FACES_NORMAL_TEXTURE1*)&pVertexData[dwVertexSize * nA];
		VFormat::FACES_NORMAL_TEXTURE1* pVerB = (VFormat::FACES_NORMAL_TEXTURE1*)&pVertexData[dwVertexSize * nB];
		VFormat::FACES_NORMAL_TEXTURE1* pVerC = (VFormat::FACES_NORMAL_TEXTURE1*)&pVertexData[dwVertexSize * nC];

		D3DXVec3TransformCoord(&pGrassInputBuffer[dwPosition].vPos,&pVerA->p,pMat);
        D3DXVec3TransformNormal(&vTemp, &pVerA->Normal, pMat);
        pGrassInputBuffer[dwPosition].dwNormal = CompressNormal(vTemp);
        vTemp = D3DXVECTOR3(pVerA->tu1, pVerA->tv1, 0);
        pGrassInputBuffer[dwPosition].dwUV = CompressUV(vTemp);

		dwPosition++;

		D3DXVec3TransformCoord(&pGrassInputBuffer[dwPosition].vPos,&pVerB->p,pMat);
        D3DXVec3TransformNormal(&vTemp, &pVerB->Normal, pMat);
        pGrassInputBuffer[dwPosition].dwNormal = CompressNormal(vTemp);
        vTemp = D3DXVECTOR3(pVerB->tu1, pVerB->tv1, 0);
        pGrassInputBuffer[dwPosition].dwUV = CompressUV(vTemp);
		dwPosition++;

		D3DXVec3TransformCoord(&pGrassInputBuffer[dwPosition].vPos,&pVerC->p,pMat);
        D3DXVec3TransformNormal(&vTemp, &pVerC->Normal, pMat);
        pGrassInputBuffer[dwPosition].dwNormal = CompressNormal(vTemp);
        vTemp = D3DXVECTOR3(pVerC->tu1, pVerC->tv1, 0);
        pGrassInputBuffer[dwPosition].dwUV = CompressUV(vTemp);
		dwPosition++;
	}

    hrResult = S_OK;
Exit0:
    if (pXMesh)
    {
        if (bVertexBufferLocked)
        {
            hr = pXMesh->UnlockVertexBuffer();
            KGLOG_COM_CHECK_ERROR(hr);
        }
        if (bIndexBufferLocked)
        {
            hr = pXMesh->UnlockIndexBuffer();
            KGLOG_COM_CHECK_ERROR(hr);
        }
    }

	return hrResult;
}



HRESULT KG3DTerrainBlockGrassShareVB_High::CreateFromModels(DWORD dwTotalFace,vector<KG3DModel*>*pvecModel,PVOID pCallUser)
{
	HRESULT hr = S_OK;

	KGLOG_PROCESS_ERROR(pvecModel);
	KGLOG_PROCESS_ERROR(dwTotalFace>=0 && dwTotalFace*3 < MAX_GRASSVERTEX);

	hr = CheckSize(dwTotalFace);
	KGLOG_COM_PROCESS_ERROR(hr);

	hr = g_TerrainGrassVertexInputBuffer.CheckSize(dwTotalFace * 3);
	KG_COM_PROCESS_ERROR(hr);

	DWORD dwFacePosition = 0;
	for (size_t i=0;i<pvecModel->size();i++)
	{
		KG3DModel* pModel = (*pvecModel)[i];
		KGLOG_PROCESS_ERROR(pModel);

		KG3DMesh* pMesh = pModel->GetMesh();
		KGLOG_PROCESS_ERROR(pMesh);

		LPD3DXMESH pXMesh = pMesh->GetMesh(KG3DMesh::SMBT_NORMAL);
		KGLOG_PROCESS_ERROR(pXMesh);

		hr = FillXMesh(dwFacePosition,pXMesh,&pModel->m_matWorld);
		KGLOG_COM_PROCESS_ERROR(hr);
	}
	
	KGLOG_PROCESS_ERROR(dwFacePosition == dwTotalFace * 3);

	{
		KG3DTerrainGrassVertex* pDest = NULL;
		hr = m_pVB->Lock(0,dwTotalFace * 3 * sizeof(KG3DTerrainGrassVertex),(void**)&pDest,0);
		KGLOG_COM_PROCESS_ERROR(hr);

		memcpy(pDest,
			g_TerrainGrassVertexInputBuffer.pGrassVertexBuffer,
			dwTotalFace * 3 * sizeof(KG3DTerrainGrassVertex));

		hr = m_pVB->Unlock();
		KGLOG_COM_PROCESS_ERROR(hr);
	}

	dwLastTime = timeGetTime();
	pUser = pCallUser;
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DTerrainBlockGrassShareVB_High::Init()
{
    UnInit();
    return S_OK;
}

HRESULT KG3DTerrainBlockGrassShareVB_High::UnInit()
{
	dwLastTime = 0;
	pUser = NULL;
	m_bUserLock = FALSE;
	m_dwUsedFaces = 0;
	m_bOkToRender = FALSE;

	return S_OK;
}

HRESULT KG3DTerrainBlockGrassShareVB_High::OnLostDevice()
{
	UnInit();
	SAFE_RELEASE(m_pVB);
	m_dwNumFacesTotal = 0;
	m_dwUsedFaces = 0;
	m_bOkToRender = FALSE;
    return S_OK;
}

HRESULT KG3DTerrainBlockGrassShareVB_High::OnResetDevice()
{
    HRESULT hResult = E_FAIL;
//    HRESULT hr = S_OK;

    KGLOG_PROCESS_ERROR(m_pVB == NULL);

    KG_PROCESS_SUCCESS(m_dwNumFacesTotal == 0);

Exit1:
    hResult = S_OK;
Exit0:
    return hResult;
}

//////////////////////////////////////////////////////////////////////////
KG3DTerrainBlockGrass::KG3DTerrainBlockGrass()
{
	m_dwNumGrass = 0;
	m_lpGrassData = NULL;

	ZeroMemory(m_lpAutoGrassData,sizeof(DWORD)*GRASS_NUM * GRASS_NUM);
	ZeroMemory(m_lpAutoGrassWidth,sizeof(float)*GRASS_NUM * GRASS_NUM);
	ZeroMemory(m_lpAutoGrassHeight,sizeof(float)*GRASS_NUM * GRASS_NUM);
	
	m_lpGrassSet = NULL;
	
	m_bNeedRefreshGrass = FALSE;
	m_lpShareGrassVB = NULL;

	m_dwNumGrassFace = 0;

	m_uHandleGrassSet = 0;
	m_lpParentTerrainBlock = NULL;

	m_bAutoGrass = FALSE;
	m_ulRefCount = 0;
	m_dwLastVisibleCount = 0;

	
}

KG3DTerrainBlockGrass::~KG3DTerrainBlockGrass(void)
{
	UnInit();
}

HRESULT KG3DTerrainBlockGrass::Init(DWORD dwType)
{
	m_dwBlockType = dwType;
	ZeroMemory(m_lpAutoGrassData,sizeof(DWORD)*GRASS_NUM * GRASS_NUM);
	ZeroMemory(m_lpAutoGrassWidth,sizeof(float)*GRASS_NUM * GRASS_NUM);
	ZeroMemory(m_lpAutoGrassHeight,sizeof(float)*GRASS_NUM * GRASS_NUM);
	m_lpGrassSet = NULL;
	m_dwNumGrassFace = 0;
	m_uHandleGrassSet = 0;
	m_dwNumGrass = 0;
	m_bAutoGrass = FALSE;
	return S_OK;
}

HRESULT KG3DTerrainBlockGrass::UnInit()
{
	m_dwNumGrass = 0;
	m_dwNumGrassFace = 0;
	m_bNeedRefreshGrass = FALSE;
	m_uHandleGrassSet = 0;

	m_bAutoGrass = FALSE;

	SAFE_DELETE_ARRAY(m_lpGrassData);
	m_listGrass.clear();

	SAFE_RELEASE(m_lpGrassSet);

	//SAFE_DELETE_ARRAY(m_lpGrassVertex);
	ZeroMemory(m_lpAutoGrassData,sizeof(DWORD)*GRASS_NUM * GRASS_NUM);
	ZeroMemory(m_lpAutoGrassWidth,sizeof(float)*GRASS_NUM * GRASS_NUM);
	ZeroMemory(m_lpAutoGrassHeight,sizeof(float)*GRASS_NUM * GRASS_NUM);

	UnPrepareShareGrassVB();

	m_lpParentTerrainBlock = NULL;
	m_dwLastVisibleCount = 0;

	return S_OK;
}

HRESULT KG3DTerrainBlockGrass::CreateGrass(DWORD dwNum)
{
	SAFE_DELETE_ARRAY(m_lpGrassData);
	m_listGrass.clear();
	m_dwNumGrass = dwNum;
	if((!m_bAutoGrass) && m_dwNumGrass )
	{
		m_lpGrassData = new KG3DTerrainGrass[m_dwNumGrass];
		KGLOG_PROCESS_ERROR(m_lpGrassData);
		ZeroMemory(m_lpGrassData,sizeof(KG3DTerrainGrass) * m_dwNumGrass);
	}

	return S_OK;
Exit0:
	return E_FAIL;
}
HRESULT KG3DTerrainBlockGrass::SetGrassSet(KG3DTerrainGrassSet* pSet,UINT uHandleSet)
{
	SAFE_RELEASE(m_lpGrassSet);
	m_lpGrassSet = pSet;
	if (m_lpGrassSet)
	{
		m_lpGrassSet->AddRef();
	}
	m_uHandleGrassSet = uHandleSet;
	return S_OK;
}

HRESULT KG3DTerrainBlockGrass::PrepareShareGrassVB()
{
	HRESULT hr = E_FAIL;
	DWORD dwThreadID = GetCurrentThreadId();
	/*if(dwThreadID != g_dwMainThreadID)
	{
		int k = 0;
	}*/
	KG_PROCESS_ERROR(dwThreadID == g_dwMainThreadID);

	if(!m_bAutoGrass)
		KG_PROCESS_ERROR(m_dwNumGrass);
	
	//HRESULT hr = S_OK;
	BOOL bNeedRefresh = FALSE;
	if(!m_lpShareGrassVB)
	{
		m_lpShareGrassVB = g_cTerrainBlockGrassShareVBHighPool.RequestResource();
		bNeedRefresh = TRUE;
	}
	else if(!m_lpShareGrassVB->m_bOkToRender)
	{
		bNeedRefresh = TRUE;
	}

	if(bNeedRefresh||m_bNeedRefreshGrass)
	{
		if(g_nGrassVertexBufferFillPerFrame <= 2 )
		{
			hr = m_lpShareGrassVB->Create(this);
			if(SUCCEEDED(hr))
			{
				m_bNeedRefreshGrass = FALSE;
				g_nLoadCount ++;
				g_nGrassVertexBufferFillPerFrame++;
			}
			else
			{
				m_bNeedRefreshGrass = TRUE;
			}
		}
		else
		{
			m_bNeedRefreshGrass = TRUE;
		}
	}

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DTerrainBlockGrass::UnPrepareShareGrassVB()
{
	if(m_lpShareGrassVB)
	{
		g_cTerrainBlockGrassShareVBHighPool.FreeResource(m_lpShareGrassVB);
		m_lpShareGrassVB = NULL;
	}
	return S_OK;
}

DWORD KG3DTerrainBlockGrass::GetTotalGrassFaceCount()
{
	DWORD S = 0;
	if(m_lpGrassSet)
	{	
		
		if(m_bAutoGrass)
		{
			BOOL bCread = FALSE;
			BYTE byPatternIndex = 0;
			BYTE byTexIndex=0;
			BYTE byDetailText = 0;
			float fWidth = 0;
			float fHeight = 0;
			for (int nZ = 0 ; nZ < GRASS_NUM; nZ++)
			{
				for(int nX = 0; nX < GRASS_NUM;nX++)
				{
					GetAutoGrassData(nX,nZ,bCread,byDetailText,byTexIndex,byPatternIndex,fWidth,fHeight);
					if(bCread)
					{
						KG3DTerrainGrassPattern* pPattern = m_lpGrassSet->FindPattern(byPatternIndex);
						if(pPattern)
						{
							S += pPattern->m_dwNumFace;
						}
					}
				}	
			}
		}
		else
		{
			for (DWORD i=0;i<m_dwNumGrass;i++)
			{
				KG3DTerrainGrass* pGrass = &m_lpGrassData[i];


				KG3DTerrainGrassPattern* pPattern = m_lpGrassSet->FindPattern(pGrass->bytePatternIndex);
				if(pPattern)
				{
					S += pPattern->m_dwNumFace;
				}
			}
		}
			
		
	}
	return S; 
}

void MatrixByNormal(D3DXMATRIX& matUpRotation,D3DXVECTOR3& vNormal)
{
	D3DXVECTOR3 vec3DefaultUp = D3DXVECTOR3(0,1,0);
	D3DXVECTOR3 vec3Axis;

	float K = D3DXVec3Dot(&vNormal, &vec3DefaultUp);
	ASSERT(K>=-1 && K<= 1);

	float fAngle = acos(K);
	if (abs(K) >= 0.999F)
	{
		D3DXMatrixIdentity(&matUpRotation);
	}
	else
	{
		D3DXVec3Cross(&vec3Axis, &vNormal, &vec3DefaultUp);
		D3DXMatrixRotationAxis(&matUpRotation, &vec3Axis, -fAngle);
	}
}
HRESULT KG3DTerrainBlockGrassShareVB_High::FillGrassVertex(DWORD& dwFaceCount,KG3DTerrainBlockGrass* pBlockGrass,KG3DTerrainGrassVertex* pVertex)
{
	D3DXVECTOR3 vec3DefaultUp(0.0f, 1.0f, 0.0f);
	D3DXMATRIX matRotation;
	D3DXMATRIX matUpRotation;
	D3DXVECTOR3 vec3Axis;

	BOOL bAutoRotation = TRUE;

	KGLOG_PROCESS_ERROR(pBlockGrass);
	KGLOG_PROCESS_ERROR(pVertex);
	KGLOG_PROCESS_ERROR(pBlockGrass->m_lpGrassSet);


	DWORD S = 0;
	if(pBlockGrass->m_bAutoGrass)
	{
		BOOL bCreate = FALSE;
		BYTE byPatternIndex = 0;
		BYTE byTexIndex=0;
		BYTE byDetailText = 0;

		KG_PROCESS_ERROR(pBlockGrass->m_lpParentTerrainBlock);
		KG_PROCESS_ERROR(pBlockGrass->m_lpParentTerrainBlock->IsLoaded());
		for (int nZ = 0;nZ < GRASS_NUM;nZ++ )
		{
			for (int nX = 0;nX < GRASS_NUM;nX++ )
			{
				float fWidth = 0;
				float fHeight = 0;
				pBlockGrass->GetAutoGrassData(nX,nZ,bCreate,byDetailText,byTexIndex,byPatternIndex,fWidth,fHeight);
				if(bCreate) 
				{
					KG3DTerrainGrassPattern* pPattern = pBlockGrass->m_lpGrassSet->FindPattern(byPatternIndex);
					if(pPattern)
					{
						D3DXVECTOR3 vNormal = pBlockGrass->m_lpParentTerrainBlock->GetVertexNormal(nX,nZ);
						D3DXVECTOR3 vPos   = pBlockGrass->m_lpParentTerrainBlock->GetVertexPos(nX,nZ);

						MatrixByNormal(matUpRotation,vNormal);

						//////////////////////////////////////////////////////////////////////////

						float g_fScale = 1.4F;

						if (bAutoRotation)
						{
							float fAngle = static_cast<float>( rand() % 360) / 180 * D3DX_PI;
							D3DXMatrixRotationY(&matRotation, fAngle);

							for (DWORD k=0;k<pPattern->m_dwNumVertex;k++)
							{
								KG3DTerrainGrassVertex* pSrc = &pPattern->m_lpVertexBuffer[k];
								KG3DTerrainGrassVertex* pDest = &pVertex[S];

								pDest->vPos = pSrc->vPos;
								pDest->vPos.x *= fWidth * g_fScale;
								pDest->vPos.z *= fWidth * g_fScale;
								pDest->vPos.y *= fHeight * g_fScale;
								pDest->dwNormal = CompressNormal(vNormal);
								D3DXVec3TransformCoord(&pDest->vPos, &pDest->vPos, &matRotation);
								D3DXVec3TransformCoord(&pDest->vPos, &pDest->vPos, &matUpRotation);
								pDest->vPos += vPos;
                                D3DXVECTOR3 vUV;
                                DecompressUV(pSrc->dwUV, &vUV);
								GetUVFromIndex(byTexIndex, vUV,
									pBlockGrass->m_lpGrassSet->m_dwStrideX,
									pBlockGrass->m_lpGrassSet->m_dwStrideY);
                                pDest->dwUV = CompressUV(vUV);
								S++;
							}

						}
					}
				}
			}
		}
	}
	else
	{
		KGLOG_PROCESS_ERROR(pBlockGrass->m_lpGrassData);
		for (DWORD i=0;i<pBlockGrass->m_dwNumGrass;i++)//while( it != m_listGrass.end())
		{
			KG3DTerrainGrass* pGrass = &pBlockGrass->m_lpGrassData[i]; //*it;

			KG3DTerrainGrassPattern* pPattern = pBlockGrass->m_lpGrassSet->FindPattern(pGrass->bytePatternIndex);
			if(pPattern)
			{
				MatrixByNormal(matUpRotation,pGrass->vNormal);
				//////////////////////////////////////////////////////////////////////////

				float g_fScale = 1.4F;

				if (bAutoRotation)
				{
					float fAngle = static_cast<float>( rand() % 360) / 180 * D3DX_PI;
					D3DXMatrixRotationY(&matRotation, fAngle);

					for (DWORD k=0;k<pPattern->m_dwNumVertex;k++)
					{
						KG3DTerrainGrassVertex* pSrc = &pPattern->m_lpVertexBuffer[k];
						KG3DTerrainGrassVertex* pDest = &pVertex[S];

						pDest->vPos = pSrc->vPos;
						pDest->vPos.x *= pGrass->fWidth * g_fScale;
						pDest->vPos.z *= pGrass->fWidth * g_fScale;
						pDest->vPos.y *= pGrass->fHeight * g_fScale;

						pDest->dwNormal = pSrc->dwNormal;

						D3DXVec3TransformCoord(&pDest->vPos, &pDest->vPos, &matRotation);
						//D3DXVec3TransformNormal(&pDest->vNormal, &pDest->vNormal, &matRotation);
						D3DXVec3TransformCoord(&pDest->vPos, &pDest->vPos, &matUpRotation);
						pDest->dwNormal = CompressNormal(D3DXVECTOR3(0,1,0));// pGrass->vNormal;

						pDest->vPos += pGrass->vPosition;

                        D3DXVECTOR3 vUV;
                        DecompressUV(pSrc->dwUV, &vUV);
						GetUVFromIndex(pGrass->byteIndex, vUV,
							pBlockGrass->m_lpGrassSet->m_dwStrideX,
							pBlockGrass->m_lpGrassSet->m_dwStrideY);
                        pDest->dwUV = CompressUV(vUV);
						S++;
					}

				}
				else
				{
					for (DWORD k=0;k<pPattern->m_dwNumVertex;k++)
					{
						KG3DTerrainGrassVertex* pSrc = &pPattern->m_lpVertexBuffer[k];
						KG3DTerrainGrassVertex* pDest = &pVertex[S];

						pDest->vPos = pSrc->vPos;
						pDest->vPos.x *= pGrass->fWidth * g_fScale;
						pDest->vPos.z *= pGrass->fWidth * g_fScale;
						pDest->vPos.y *= pGrass->fHeight * g_fScale;

						pDest->dwNormal = pSrc->dwNormal;

						D3DXVec3TransformCoord(&pDest->vPos, &pDest->vPos, &matUpRotation);
						pDest->dwNormal = CompressNormal(pGrass->vNormal);

						pDest->vPos += pGrass->vPosition;

                        D3DXVECTOR3 vUV;
                        DecompressUV(pSrc->dwUV, &vUV);
						GetUVFromIndex(pGrass->byteIndex, vUV,
							pBlockGrass->m_lpGrassSet->m_dwStrideX,
							pBlockGrass->m_lpGrassSet->m_dwStrideY);
                        pDest->dwUV = CompressUV(vUV);
						S++;
					}
				}
			}
		}
	}
	dwFaceCount = S / 3;
	ASSERT(dwFaceCount);
	return S_OK;
Exit0:
	return E_FAIL;
}

void KG3DTerrainBlockGrassShareVB_High::GetUVFromIndex(DWORD dwTexIndex,
											  D3DXVECTOR3& vec2UV,DWORD m_dwStrideX,DWORD m_dwStrideY)
{
	float fUSpan = 1.0f / m_dwStrideX;
	float fVSpan = 1.0f / m_dwStrideY;

	int nIndexU = dwTexIndex % m_dwStrideX;
	int nIndexV = dwTexIndex / m_dwStrideX;

	vec2UV.x = vec2UV.x * fUSpan + nIndexU * fUSpan;
	vec2UV.y = vec2UV.y * fVSpan + nIndexV * fVSpan;
}

LPDIRECT3DVERTEXBUFFER9 KG3DTerrainBlockGrass::GetShareGrassVBuffer(DWORD& dwFaces)
{
	if(!m_bAutoGrass)
	    KG_PROCESS_ERROR(m_dwNumGrass);

	if (m_bNeedRefreshGrass || !m_lpShareGrassVB)
	{
		PrepareShareGrassVB();
        KG_PROCESS_ERROR(!m_bNeedRefreshGrass);
	}

	if(m_lpShareGrassVB && !m_lpShareGrassVB->m_bOkToRender)
	{
		PrepareShareGrassVB();
	}

	KG_PROCESS_ERROR(m_lpShareGrassVB && m_lpShareGrassVB->m_bOkToRender);

	dwFaces = m_lpShareGrassVB->m_dwUsedFaces / g_cEngineOption.nVegetationDensity;
	return m_lpShareGrassVB->m_pVB;

Exit0 :
	dwFaces = 0;
	return NULL;
}

LPDIRECT3DTEXTURE9 KG3DTerrainBlockGrass::GetGrassTexture()
{
	if(m_lpGrassSet)
	{
		if(m_lpGrassSet->m_lpGrassTexture)
		{
			return m_lpGrassSet->m_lpGrassTexture->GetTexture();
		}
	}
	return NULL;
}


HRESULT KG3DTerrainBlockGrass::CreateGrassVertex()
{
	m_bNeedRefreshGrass = TRUE;

	return S_OK;
}


HRESULT KG3DTerrainBlockGrass::AddGrass(KG3DTerrainGrass& sGrass)
{
	KG_PROCESS_ERROR(m_listGrass.size() < 65535);
	m_listGrass.push_back(sGrass);
	m_dwNumGrass = (DWORD)m_listGrass.size();
Exit0:
	return S_OK;
}


HRESULT KG3DTerrainBlockGrass::RemoveGrass(FRECT rect)
{
	list<KG3DTerrainGrass>::iterator i = m_listGrass.begin();

	while (i != m_listGrass.end())
	{
		KG3DTerrainGrass sGrass = *i;

		D3DXVECTOR2 pos;

		pos.x = sGrass.vPosition.x;
		pos.y = sGrass.vPosition.z;

		if(rect.IsPositionInRect(pos))
		{
			i = m_listGrass.erase(i);
		}
		else
		{
			i++;
		}
	}
	m_dwNumGrass = (DWORD)m_listGrass.size();
	//m_bBillBoardDirtyFlag = TRUE;
	//m_bHighDefDirtyFlag   = TRUE;
	return S_OK;
}

HRESULT KG3DTerrainBlockGrass::ClearGrassList()
{
	m_listGrass.clear();
	m_dwNumGrass = 0;
	return S_OK;
}
HRESULT KG3DTerrainBlockGrass::GetVecGrass(vector<KG3DTerrainGrass>* pVecGrass)
{
	HRESULT hr = E_FAIL;
	//KG_PROCESS_ERROR(m_lpGrassData);
	//for(int nGrass = 0; nGrass < m_dwNumGrass; nGrass++)
	//{
	//	pVecGrass->push_back(m_lpGrassData[nGrass]);
	//}
	/*for (int n = 0; n < (int)m_listGrass.size();n++)
	{
		pVecGrass->push_back(m_vecGrass[n]);
	}
	m_vecGrass.clear();*/
	list<KG3DTerrainGrass>::iterator i = m_listGrass.begin();
	while (i != m_listGrass.end())
	{
		KG3DTerrainGrass sGrass = *i;
		pVecGrass->push_back(sGrass);
		i++;
	}
	hr = S_OK;
	return hr;
}
HRESULT KG3DTerrainBlockGrass::FitGround(KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr)
{
	list<KG3DTerrainGrass>::iterator i = m_listGrass.begin();
	while (i != m_listGrass.end())
	{
		KG3DTerrainGrass* pGrass = &(*i);
		pGrass->vPosition.y = pOutDoorSpaceMgr->GetGroundHeight(pGrass->vPosition.x , pGrass->vPosition.z);
		i++;
	}
	return S_OK;
}
//HRESULT KG3DTerrainBlockGrass::FillVertex(KG3DTerrainGrassVertex* pVertex,KG3DTerrainGrassSet* pSet,KG3DTerrainGrass* pData,DWORD dwNum)
//{
//	D3DXVECTOR3 vec3DefaultUp(0.0f, 1.0f, 0.0f);
//	D3DXMATRIX matRotation;
//	D3DXMATRIX matUpRotation;
//	D3DXVECTOR3 vec3Axis;
//
//	BOOL bAutoRotation = TRUE;
//	//list<KG3DTerrainGrass>::iterator it = m_listGrass.begin();
//	KGLOG_PROCESS_ERROR(pVertex);
//
//	DWORD S = 0;
//	for (DWORD i=0;i<dwNum;i++)//while( it != m_listGrass.end())
//	{
//		KG3DTerrainGrass* pGrass = &pData[i]; //*it;
//
//		if(pSet)
//		{
//			KG3DTerrainGrassPattern* pPattern = pSet->FindPattern(pGrass->bytePatternIndex);
//			if(pPattern)
//			{
//				float fAngle = acos(D3DXVec3Dot(&pGrass->vNormal, &vec3DefaultUp));
//				if (abs(fAngle) < 0.001)
//				{
//					D3DXMatrixIdentity(&matUpRotation);
//				}
//				else
//				{
//					D3DXVec3Cross(&vec3Axis, &pGrass->vNormal, &vec3DefaultUp);
//					D3DXMatrixRotationAxis(&matUpRotation, &vec3Axis, -fAngle);
//				}
//
//				//////////////////////////////////////////////////////////////////////////
//
//				float g_fScale = 1.4F;
//
//				if (bAutoRotation)
//				{
//					float fAngle = static_cast<float>( rand() % 360) / 180 * D3DX_PI;
//					D3DXMatrixRotationY(&matRotation, fAngle);
//
//
//					for (DWORD k=0;k<pPattern->m_dwNumVertex;k++)
//					{
//						KG3DTerrainGrassVertex* pSrc = &pPattern->m_lpVertexBuffer[k];
//						KG3DTerrainGrassVertex* pDest = &pVertex[S];
//
//						pDest->vPos = pSrc->vPos;
//						pDest->vPos.x *= pGrass->fWidth * g_fScale;
//						pDest->vPos.z *= pGrass->fWidth * g_fScale;
//						pDest->vPos.y *= pGrass->fHeight * g_fScale;
//
//						pDest->vNormal = pSrc->vNormal;
//
//						D3DXVec3TransformCoord(&pDest->vPos, &pDest->vPos, &matRotation);
//						D3DXVec3TransformNormal(&pDest->vNormal, &pDest->vNormal, &matRotation);
//						D3DXVec3TransformCoord(&pDest->vPos, &pDest->vPos, &matUpRotation);
//						pDest->vNormal = pGrass->vNormal;
//
//						pDest->vPos += pGrass->vPosition;
//
//						pDest->vUV = pSrc->vUV;
//						GetUVFromIndex(pGrass->byteIndex, pDest->vUV,pSet->m_dwStrideX,pSet->m_dwStrideY);
//
//						//pDest->dwColor = pGrass->dwColor;
//						//pVBData[j].vec2UVSrc = pPattern->pVertexBuffer[j].vec2UV;
//
//						S++;
//					}
//
//				}
//				else
//				{
//					for (DWORD k=0;k<pPattern->m_dwNumVertex;k++)
//					{
//						KG3DTerrainGrassVertex* pSrc = &pPattern->m_lpVertexBuffer[k];
//						KG3DTerrainGrassVertex* pDest = &pVertex[S];
//
//						pDest->vPos = pSrc->vPos;
//						pDest->vPos.x *= pGrass->fWidth * g_fScale;
//						pDest->vPos.z *= pGrass->fWidth * g_fScale;
//						pDest->vPos.y *= pGrass->fHeight * g_fScale;
//
//						pDest->vNormal = pSrc->vNormal;
//
//						D3DXVec3TransformCoord(&pDest->vPos, &pDest->vPos, &matUpRotation);
//						pDest->vNormal = pGrass->vNormal;
//
//						pDest->vPos += pGrass->vPosition;
//
//						pDest->vUV = pSrc->vUV;
//						GetUVFromIndex(pGrass->byteIndex, pDest->vUV,pSet->m_dwStrideX,pSet->m_dwStrideY);
//
//						S++;
//					}
//				}
//			}
//		}
//		//it++;
//	}
//	return S_OK;
//Exit0:
//	return E_FAIL;
//}

HRESULT KG3DTerrainBlockGrass::GetAutoGrassData(int nX,int nZ,BOOL& bCreat,BYTE& byDetailTexture,BYTE& byTexIndex,BYTE& byPattrenIndex,float& fWidth,float& fHeight)
{
	if((nX<0)||(nX>=GRASS_NUM)||(nZ<0)||(nZ>=GRASS_NUM)) 
	{
		bCreat = FALSE;
		return S_OK;
	}
	int Index = nZ * GRASS_NUM + nX;
	DWORD* dwTemp = &m_lpAutoGrassData[Index];
	BYTE* byTemp = (BYTE*)(dwTemp);
	byPattrenIndex = byTemp[0];
	byTexIndex = byTemp[1];
	byDetailTexture = byTemp[2];
	bCreat = byTemp[3];
	fWidth = m_lpAutoGrassWidth[Index];
	fHeight = m_lpAutoGrassHeight[Index];
	return S_OK;
}
HRESULT KG3DTerrainBlockGrass::SetAutoGrassData(int nX,int nZ,BOOL bCreat,BYTE byDetailTexture,BYTE byTexIndex,BYTE byPattrenIndex,float fWidth,float fHeight)
{
	if((nX<0)||(nX>=GRASS_NUM)||(nZ<0)||(nZ>=GRASS_NUM)) 	
		return S_OK;

	int Index = nZ * GRASS_NUM + nX;
	DWORD* dwTemp = &m_lpAutoGrassData[Index];
	BYTE* byTemp = (BYTE*)(dwTemp);
	byTemp[0] = byPattrenIndex;
	byTemp[1] = byTexIndex;
	byTemp[2] = byDetailTexture;
	byTemp[3] = (BYTE)bCreat ;
	m_lpAutoGrassWidth[Index] = fWidth;
	m_lpAutoGrassHeight[Index]= fHeight;
	m_bAutoGrass = TRUE;
	return S_OK;
}

ULONG KG3DTerrainBlockGrass::AddRef()
{
    return (ULONG)KG_InterlockedIncrement((long *)&m_ulRefCount);
}

ULONG KG3DTerrainBlockGrass::Release()
{
    ULONG uNewRefCount = (ULONG)KG_InterlockedDecrement((long *)&m_ulRefCount);
    if (uNewRefCount == 0)
		g_cTerrainBlockGrassPool.FreeResource(this);
    return uNewRefCount;
}
