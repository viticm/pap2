#include "stdafx.h"
#include "kg3dmodeltable.h"
#include "kg3dgrasspatterncontainer.h"
#include "KG3DFileFinder.h"
#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

KG3DGrassPatternContainer g_GrassPatternContainer;

PatternModel::~PatternModel()
{
	SAFE_DELETE_ARRAY(pVertexBuffer);
	SAFE_DELETE_ARRAY(pIndicesBuffer);
}

PatternModel::PatternModel()
{
	pVertexBuffer = NULL;
	pIndicesBuffer = NULL;
	dwVertexCnt = 0;
	dwIndicesCnt = 0;
	strModelFileName[0] = 0; //Added by Wilson, Fixed by Frank.
}

PatternModel::PatternModel(const PatternModel& src)
{
	if (&src != this)
	{
		pVertexBuffer = NULL;	//Added by Wilson, Fixed by Frank.
		pIndicesBuffer = NULL;
		if (src.pVertexBuffer)
		{		
			pVertexBuffer = new HighDefGrassModelVertex[src.dwVertexCnt];
			memcpy(pVertexBuffer, src.pVertexBuffer, sizeof(HighDefGrassModelVertex) * src.dwVertexCnt);
		}
		if (src.pIndicesBuffer)
		{
			pIndicesBuffer = new short[src.dwIndicesCnt];
			memcpy(pIndicesBuffer, src.pIndicesBuffer, sizeof(short) * src.dwIndicesCnt);
		}
		dwVertexCnt = src.dwVertexCnt;
		dwIndicesCnt = src.dwIndicesCnt;
		
		strcpy_s(strModelFileName, sizeof(TCHAR) * MAX_PATH, src.strModelFileName);
		PatternBBox = src.PatternBBox;
		
	}
}

PatternModel& PatternModel::operator =(const PatternModel& src)
{
	if (&src != this)
	{
		if (src.dwVertexCnt)
		{
		       pVertexBuffer = new HighDefGrassModelVertex[src.dwVertexCnt];
		}
		else 
		{
		       pVertexBuffer = NULL;
		}

		if (src.dwIndicesCnt)
		{
		pIndicesBuffer = new short[src.dwIndicesCnt];
		}
		else
		{
			pIndicesBuffer = NULL;
		}
		dwVertexCnt = src.dwVertexCnt;
		dwIndicesCnt = src.dwIndicesCnt;
		memcpy(pVertexBuffer, src.pVertexBuffer, sizeof(HighDefGrassModelVertex) * src.dwVertexCnt);
		memcpy(pIndicesBuffer, src.pIndicesBuffer, sizeof(short) * src.dwIndicesCnt);
		strcpy_s(strModelFileName, sizeof(TCHAR) * MAX_PATH, src.strModelFileName);
		PatternBBox = src.PatternBBox;
	}
	return *this;
}


KG3DGrassPatternContainer::KG3DGrassPatternContainer()
{
}

KG3DGrassPatternContainer::~KG3DGrassPatternContainer()
{
	UnInit();

	
}

HRESULT KG3DGrassPatternContainer::LoadPattern(LPCSTR strFileName, int& nIndex)
{
	HRESULT hr = E_FAIL;
	HRESULT hResult = E_FAIL;
	KG3DModel* pModel = NULL;
	KG3DMesh* pMesh = NULL;
	ID3DXMesh* p3DMesh = NULL;
	HighDefGrassModelVertex* pSrcVertex = NULL;
	short* pSrcIndex = NULL;
	int nReturn = 0;
	TCHAR fileName[MAX_PATH];
	KG_PROCESS_ERROR(strFileName);
	wsprintf(fileName,"%s",GetRetPath(strFileName));	
	hr = FindPattern(fileName, &nReturn);
	KGLOG_COM_PROCESS_ERROR(hr);

    if (nReturn != -1)
	{
		nIndex = nReturn;
		hr = S_OK;
		hResult = S_OK;
		goto Exit0;
	}

	m_Patterns.push_back(PatternModel()); //Added by Wilson, Fixed by Frank.
	//m_Patterns.resize(m_Patterns.size() + 1);

	hr = g_cModelTable.LoadResourceFromFile(fileName, 0, MLO_TEXTURE_MULTI, (IKG3DResourceBase**)&pModel);
	KG_COM_PROCESS_ERROR(hr);
	m_p3DModel.push_back(pModel);
	pMesh = pModel->GetMesh();
	KG_PROCESS_ERROR(NULL != pMesh);
	
	p3DMesh = pMesh->m_ppMeshes[KG3DMesh::SMBT_NORMAL];
	KG_PROCESS_ERROR(p3DMesh);

	hr = p3DMesh->LockVertexBuffer(0, (void**)&pSrcVertex);
	KG_COM_PROCESS_ERROR(hr);

	hr = p3DMesh->LockIndexBuffer(0, (void**)&pSrcIndex);
	KG_COM_PROCESS_ERROR(hr);

	PatternModel& Pattern = m_Patterns[m_Patterns.size() - 1];

	Pattern.dwVertexCnt = p3DMesh->GetNumVertices();
	KG_PROCESS_ERROR(Pattern.dwVertexCnt);
	
	Pattern.dwIndicesCnt = p3DMesh->GetNumFaces() * 3;
	KG_PROCESS_ERROR(Pattern.dwIndicesCnt);

	Pattern.pIndicesBuffer = new short[Pattern.dwIndicesCnt];
	KG_PROCESS_ERROR(Pattern.pIndicesBuffer);

	Pattern.pVertexBuffer = new HighDefGrassModelVertex[Pattern.dwVertexCnt];
	KG_PROCESS_ERROR(Pattern.pVertexBuffer);

	for (DWORD i = 0; i < Pattern.dwVertexCnt; i++)
	{
		Pattern.pVertexBuffer[i].vec3Pos = pSrcVertex[i].vec3Pos;
		Pattern.pVertexBuffer[i].vec3Normal = pSrcVertex[i].vec3Normal;//D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		Pattern.pVertexBuffer[i].vec2UV = pSrcVertex[i].vec2UV;
	}

	memcpy(Pattern.pIndicesBuffer, pSrcIndex, sizeof(short) * Pattern.dwIndicesCnt);
	
	Pattern.PatternBBox = pMesh->m_BBox;
	strcpy_s(Pattern.strModelFileName, sizeof(TCHAR) * MAX_PATH, fileName);
	_strlwr_s(Pattern.strModelFileName, sizeof(TCHAR) * MAX_PATH);
	nIndex = static_cast<int>(m_Patterns.size()) - 1;
	hr = S_OK;
	hResult = S_OK;
Exit0:
	if (p3DMesh)
	{
		hr = p3DMesh->UnlockIndexBuffer();
		hr = p3DMesh->UnlockVertexBuffer();
	}

	//SAFE_RELEASE(pMesh);//原来GetMesh是加引用计数的，现在不加
	
	if (FAILED(hr))
	{
		if(strFileName)
			KGLogPrintf(KGLOG_ERR, "KG3DGrassPatternContainer::LoadPattern(%s) failed.", strFileName);
	}
	return hResult;
}

HRESULT KG3DGrassPatternContainer::FindPattern(LPCSTR strFileName, int *pnRetIndex)
{
	HRESULT hrResult = E_FAIL;
	*pnRetIndex = -1;
	KG_PROCESS_ERROR(strFileName);

	for (size_t i = 0; i < m_Patterns.size(); i++)
	{
		if (!_strcmpi(m_Patterns[i].strModelFileName, strFileName))
		{
			*pnRetIndex = static_cast<int>(i);
			break;
		}
	}
    
    hrResult = S_OK;
Exit0:
	return hrResult;
}

PatternModel* KG3DGrassPatternContainer::GetPattern(unsigned int nIndex)
{
	PatternModel* pReturn = NULL;
	KG_PROCESS_ERROR(nIndex < m_Patterns.size());
	pReturn = &(m_Patterns[nIndex]);
Exit0:
	return pReturn;
}

KG3DModel* KG3DGrassPatternContainer::GetModel(unsigned int nIndex)
{
	KG3DModel* pReturn = NULL;
	KG_PROCESS_ERROR(nIndex < m_p3DModel.size());
	pReturn = m_p3DModel[nIndex];
Exit0:
	return pReturn;
}

HRESULT KG3DGrassPatternContainer::Init()
{
    return S_OK;
}

HRESULT KG3DGrassPatternContainer::UnInit()
{
	for (unsigned int i=0;i < m_p3DModel.size();i++)
	{
		SAFE_RELEASE(m_p3DModel[i]);
	}

    return S_OK;
}