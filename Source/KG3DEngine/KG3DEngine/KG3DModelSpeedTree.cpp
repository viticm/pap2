////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DModelSpeedTree.cpp
//  Version     : 1.0
//  Author      : ZhaoXinyu (Chiyer)
//  CopyRight   : Kingsoft Corporation All rights Reserved
//  History     : 2005-10-19 11:56:47
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KG3DModelSpeedTree.h"
#include "KG3DEngineManager.h"
#include "KG3DGraphicsTool.h"
#include "KG3DMesh.h"
#include "KG3DScene.h"
#include "KG3DModelTable.h"
#include "KG3DMeshTable.h"
#include "KG3DMaterialTable.h"
#include "KG3DClipTable.h"
#include "KG3DRenderState.h"
#include "KG3DSceneEditorBase.h"
#include "KG3DSceneModelEditor.h"
#include "shlwapi.h"
#include "MathTool.h"
#include "KG3DTextureTable.h"
#include "KG3DBillboardCloud.h"
#include "KG3DTerrainBlock.h"
#include "KG3DMultiThreadLoader.h"
#include "KG3DShaderManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

float g_fSpeedTreeLeafScale = 1.0F;
BOOL g_bRenderTree = TRUE;

LPDIRECT3DVERTEXDECLARATION9 KG3DModelSpeedTree::m_lpVertexDeclaration = NULL;

map<KG3DModelSpeedTree::_TreeTexInfo, KG3DModelSpeedTree::_TexArray> KG3DModelSpeedTree::m_mapBillBoardTexs;
LPDIRECT3DVERTEXBUFFER9 KG3DModelSpeedTree::m_pBillBoardBuf = NULL;

KG3DSpeedTreeLeafsTable g_cSpeedTreeLeafsTable;

extern int g_nLoadCount;
extern int g_nLoadTime ;
extern DWORD g_dwMainThreadID;//渲染线程ID

//默认的树叶材质， 创建新的SpdTree时会用到
const TCHAR *s_strDefaultLeafMaterial = "data\\public\\defaultleaf.mtl";

KG3DSpeedTreeLeafsData::KG3DSpeedTreeLeafsData(UINT uHashID)
{
	m_fRollLeft           = -0.3F;
	m_fRollRight          = 0.6F;
	m_fMaxRollSpeed       = 0.0006f;
	m_fRollAccelerate     = 0.000002F;
	m_fPitchLimit         = 0.2F;
	m_fMaxPitchSpeed      = 0.0006f;
	m_fPitchAccelerate    = 0.000002F;
	
	m_lpLeafVertexBuffer = NULL;

	m_ulRefCount = 0;
	m_bNeedRecomputePostionByBone = FALSE;

	m_uFileHashID = uHashID;
	m_bLoaded = FALSE;
}

KG3DSpeedTreeLeafsData::~KG3DSpeedTreeLeafsData()
{
	ClearLeafs();
	SAFE_RELEASE(m_lpLeafVertexBuffer);
}

ULONG KG3DSpeedTreeLeafsData::AddRef()
{
    return (ULONG)KG_InterlockedIncrement((long *)&m_ulRefCount);
}

ULONG KG3DSpeedTreeLeafsData::Release()
{
    ULONG uNewRefCount = (ULONG)KG_InterlockedDecrement((long *)&m_ulRefCount);
    if (uNewRefCount == 0)
		g_cSpeedTreeLeafsTable.FreeLeafsData(this);
    return uNewRefCount;
}

HRESULT KG3DSpeedTreeLeafsData::ClearLeafs()
{
	LEAFIter_t i = m_ListLeaf.begin();
	while(i!=m_ListLeaf.end())
	{
		LPLEAF pLeaf = *i;
        SAFE_DELETE(pLeaf);
		i++;
	}
	m_ListLeaf.clear();
	m_bLoaded = FALSE;

	return S_OK;
}

KG3DModelSpeedTree::KG3DModelSpeedTree(void)
{
	m_lpLeafsData = NULL;

    m_lpModelTree         = NULL;
    m_bNeedUpdataPosition = TRUE;
    m_bNeedUpdateVB       = TRUE;
    

    m_bNeeDrawBone        = FALSE;
	m_fLeafScale = 1.0F;

	m_nUpdata =  rand()%10;


	////////////////////////////////////////////////
	// next link
	m_nOption	=	0;

	////////////////////////////////////////////////
	// dirty uv / position member
	//m_bDirtyUV = true;
	//m_bDirtyPosition = true;

    ///////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////
	m_uFileHashID = 0;
	m_fLastLeafScale = 1.0f;
}

KG3DModelSpeedTree::~KG3DModelSpeedTree(void)
{
	UnInit();
}

HRESULT KG3DModelSpeedTree::UnInit()
{
	SAFE_RELEASE(m_lpModelTree);
	///////////////////////////////////////////////////////////////////////
	SAFE_RELEASE(m_lpLeafsData);
	
	KG3DModel::UnInit();
	m_bMeshPostLoadProcessed = FALSE;
	return S_OK;
}

HRESULT KG3DModelSpeedTree::OnLostDevice()
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    hr = KG3DModel::OnLostDevice();
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KG3DModelSpeedTree::OnResetDevice()
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    hr = KG3DModel::OnResetDevice();
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KG3DSpeedTreeLeafsData::FillLeafVerticesBuffer()
{
	_LEAF_VERTEX *pData = NULL;
	LEAFIter_t i;
	HRESULT hr = E_FAIL;

	KGLOG_PROCESS_ERROR(m_lpLeafVertexBuffer);
	KGLOG_PROCESS_ERROR(m_bLoaded);

	i = m_ListLeaf.begin();

	hr = m_lpLeafVertexBuffer->Lock(0, 0, (void**)&pData, 0);
	KGLOG_COM_PROCESS_ERROR(hr);

	KGLOG_PROCESS_ERROR(pData);

	while (i != m_ListLeaf.end())
	{
		LPLEAF leaf = *i;

		//float fScale = m_fLeafScale * 1.5f;
		float fSize = leaf->Size;

		float tuA = leaf->TextureUV % 2 * 0.5f;
		float tuB = tuA + 0.5f;
		float tuC = tuA;
		float tuD = tuA + 0.5f;

		float tvA = leaf->TextureUV / 2 * 0.5f;
		float tvB = tvA;
		float tvC = tvA + 0.5f;
		float tvD = tvA + 0.5f;

		pData->x = leaf->OriginPosition.x;        pData->y = leaf->OriginPosition.y;        pData->z = leaf->OriginPosition.z;
		pData->nx = leaf->NormalOffset.x;   pData->ny = leaf->NormalOffset.y;   pData->nz = leaf->NormalOffset.z;
		pData->ox = -fSize;                 pData->oy = -fSize;
		pData->tu = tuD;                    pData->tv = tvD;
		pData++;

		pData->x = leaf->OriginPosition.x;        pData->y = leaf->OriginPosition.y;        pData->z = leaf->OriginPosition.z;
		pData->nx = leaf->NormalOffset.x;   pData->ny = leaf->NormalOffset.y;   pData->nz = leaf->NormalOffset.z;
		pData->ox = fSize;                  pData->oy = -fSize;
		pData->tu = tuC;                    pData->tv = tvC;
		pData++;

		pData->x = leaf->OriginPosition.x;        pData->y = leaf->OriginPosition.y;        pData->z = leaf->OriginPosition.z;
		pData->nx = leaf->NormalOffset.x;   pData->ny = leaf->NormalOffset.y;   pData->nz = leaf->NormalOffset.z;
		pData->ox = -fSize;                 pData->oy = fSize;
		pData->tu = tuB;                    pData->tv = tvB;
		pData++;

		///////////////////////////////////////
		pData->x = leaf->OriginPosition.x;        pData->y = leaf->OriginPosition.y;        pData->z = leaf->OriginPosition.z;
		pData->nx = leaf->NormalOffset.x;   pData->ny = leaf->NormalOffset.y;   pData->nz = leaf->NormalOffset.z;
		pData->ox = fSize;                  pData->oy = -fSize;
		pData->tu = tuC;                    pData->tv = tvC;
		pData++;

		pData->x = leaf->OriginPosition.x;        pData->y = leaf->OriginPosition.y;        pData->z = leaf->OriginPosition.z;
		pData->nx = leaf->NormalOffset.x;   pData->ny = leaf->NormalOffset.y;   pData->nz = leaf->NormalOffset.z;
		pData->ox = fSize;                  pData->oy = fSize;
		pData->tu = tuA;                    pData->tv = tvA;
		pData++;

		pData->x = leaf->OriginPosition.x;        pData->y = leaf->OriginPosition.y;        pData->z = leaf->OriginPosition.z;
		pData->nx = leaf->NormalOffset.x;   pData->ny = leaf->NormalOffset.y;   pData->nz = leaf->NormalOffset.z;
		pData->ox = -fSize;                 pData->oy = fSize;
		pData->tu = tuB;                    pData->tv = tvB;
		pData++;

		++i;
	}

	hr = m_lpLeafVertexBuffer->Unlock();
	KGLOG_COM_PROCESS_ERROR(hr);

	hr = S_OK;
Exit0:
	return hr;
}

HRESULT KG3DModelSpeedTree::FillLeafVerticesBuffer()
{
	//KG3DMesh* pMesh = NULL;
	HRESULT hr = S_OK;

	DWORD dwThreadID = ::GetCurrentThreadId();
	KGLOG_PROCESS_ERROR(dwThreadID == g_dwMainThreadID);
	ASSERT(dwThreadID == g_dwMainThreadID);

	KGLOG_PROCESS_ERROR(m_lpLeafsData);

	hr = m_lpLeafsData->FillLeafVerticesBuffer();
	KGLOG_COM_PROCESS_ERROR(hr);

	m_bNeedUpdateVB = FALSE;
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSpeedTreeLeafsData::CreateVerticesBuffer()
{
	HRESULT hr = S_OK;

	KG_PROCESS_ERROR(m_bLoaded);
	VERIFY_RELEASE(m_lpLeafVertexBuffer);
	KG_PROCESS_SUCCESS(!m_ListLeaf.size());

	hr = g_pd3dDevice->CreateVertexBuffer(
		(UINT)sizeof(_LEAF_VERTEX) * m_ListLeaf.size() * 2 * 3,
		0, 0, D3DPOOL_MANAGED, &m_lpLeafVertexBuffer, NULL);
	KGLOG_COM_PROCESS_ERROR(hr);

	hr = FillLeafVerticesBuffer();
	KG_COM_PROCESS_ERROR(hr);

Exit1:
	hr = S_OK;
Exit0:
	return hr;
}

HRESULT KG3DModelSpeedTree::CreateVerticesBuffer()
{
	HRESULT hr = S_OK;

	DWORD dwThreadID = ::GetCurrentThreadId();
	KGLOG_PROCESS_ERROR(dwThreadID == g_dwMainThreadID);
	ASSERT(dwThreadID == g_dwMainThreadID);

	KG_PROCESS_ERROR(m_lpLeafsData);

	hr = m_lpLeafsData->CreateVerticesBuffer();
	KGLOG_COM_PROCESS_ERROR(hr);

    hr = S_OK;
Exit0:
    return hr;
}

HRESULT KG3DModelSpeedTree::LoadTreeMesh( 
    const char cszFileName[], unsigned uFileNameHash, unsigned uOption ,BOOL MultiThread
)
{
	HRESULT hr = E_FAIL;
	DWORD dwTime = timeGetTime();

    if (
        (cszFileName == NULL)         ||
        (_strcmpi(cszFileName, "") == 0)
    )
    {
        return hr;
    }

	SAFE_RELEASE(m_lpModelTree);
	hr = g_cModelTable.LoadResourceFromFile(cszFileName, 0, uOption|MLO_MESH_SPEEDTREE/*uOption | MLO_MESH_SPEEDTREE|MLO_TEXTURE_MULTI*/,
		(IKG3DResourceBase **)&m_lpModelTree);
	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);
	
	/*m_lpMesh = m_lpModelTree->GetMesh();
	m_lpMesh->AddRef();*/

    m_BBox.A = m_lpModelTree->m_BBox.A;
    m_BBox.B = m_lpModelTree->m_BBox.B;
    
	m_szTreeMeshName = cszFileName;
	
	hr = S_OK;

	DWORD dwCost = timeGetTime() - dwTime;
	if(dwCost >= 30)
	{
		//KGLogPrintf(KGLOG_ERR, "%d LoadTreeMesh File %s.",dwCost,cszFileName);
	}
Exit0:
	return hr;
}

HRESULT KG3DModelSpeedTree::LoadTreeMtl( 
    const char cszFileName[], unsigned uFileNameHash, unsigned uOption 
)
{
	HRESULT hr = E_FAIL;

    if (
        (cszFileName == NULL)         ||
        (_strcmpi(cszFileName, "") == 0)
    )
    {
        goto Exit0;
    }

	TCHAR Name[256];
	wsprintf(Name,"%s",cszFileName);
	hr = m_lpModelTree->LoadMaterialFromFile(Name,uOption);
	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

	m_szTreeMtlName = cszFileName;
    return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DModelSpeedTree::LoadTreeAni(
    const char cszFileName[], unsigned uFileNameHash, unsigned uOption
)
{
    if (
        (cszFileName == NULL)         ||
        (_strcmpi(cszFileName, "") == 0)
    )
    {
        return E_FAIL;
    }
    KG3DClip* pClip = NULL;
	if (FAILED(g_cClipTable.LoadResourceFromFile(cszFileName, 0, 0/*uOption*/, &pClip)))
    {
        return E_FAIL;
    }
    SAFE_RELEASE(m_lpModelTree->m_lpClip);
    m_lpModelTree->m_lpClip = pClip;

	m_szTreeAniName = cszFileName;
    return S_OK;
}

HRESULT KG3DModelSpeedTree::LoadPropertyFromIni(const char cszFileName[])
{
	//speedtree的property来自树干的mesh的ini
	return S_OK;
}

HRESULT KG3DModelSpeedTree::SavePropertyToIni(const char cszFileName[])
{
	HRESULT hResult = E_FAIL;
	HRESULT hRetCode = E_FAIL;
	IEKG3DMesh *pMesh = NULL;

	KG_PROCESS_ERROR(m_lpModelTree);

	hRetCode = m_lpModelTree->GetIEKG3DMesh(&pMesh);
	KGLOG_COM_PROCESS_ERROR(hRetCode);

	KG_PROCESS_ERROR(pMesh);

	pMesh->SavePropertyToIni(NULL);

	hResult	= S_OK;
Exit0:
	return hResult;
}

//HRESULT KG3DModelSpeedTree::LoadPropertyFromIni(const char cszFileName[])
//{
//    HRESULT hResult  = E_FAIL;
//
//    //m_lpLeafsData->m_strPropertyFileName = cszFileName;
//	//KG3DOBJProperty& m_objPropertys = //m_lpLeafsData->m_objPropertys;
//
//    IIniFile* pIniFile = NULL;
//    if (::PathFileExists(cszFileName))
//    {
//        int nColorARGB[4];
//
//        pIniFile = g_OpenIniFile(cszFileName, false, false);
//        KG_PROCESS_ERROR(pIniFile); 
//
//        // =============声音相关(Sound)================
//        pIniFile->GetString(
//            TEXT("Sounds"), TEXT("szRandGlobSound1_FilePath"),
//            TEXT(""), m_objPropertys.szRandGlobSound1_FilePath,
//            sizeof(m_objPropertys.szRandGlobSound1_FilePath)
//            );
//        pIniFile->GetInteger(
//            TEXT("Sounds"), TEXT("nRandGlobSound1_Inteval"),
//            0, &m_objPropertys.nRandGlobSound1_Inteval
//            );
//        pIniFile->GetInteger(
//            TEXT("Sounds"), TEXT("nRandGlobSound1_Range"),
//            0, &m_objPropertys.nRandGlobSound1_Range
//            );
//        pIniFile->GetInteger(
//            TEXT("Sounds"), TEXT("nRandGlobSound1_AppearRate"),
//            0, &m_objPropertys.nRandGlobSound1_AppearRate
//            );
//
//        pIniFile->GetString(
//            TEXT("Sounds"), TEXT("szRandGlobSound2_FilePath"),
//            TEXT(""), m_objPropertys.szRandGlobSound2_FilePath,
//            sizeof(m_objPropertys.szRandGlobSound2_FilePath)
//            );
//        pIniFile->GetInteger(
//            TEXT("Sounds"), TEXT("nRandGlobSound2_Inteval"),
//            0, &m_objPropertys.nRandGlobSound2_Inteval
//            );
//        pIniFile->GetInteger(
//            TEXT("Sounds"), TEXT("nRandGlobSound2_Range"),
//            0, &m_objPropertys.nRandGlobSound2_Range
//            );
//        pIniFile->GetInteger(
//            TEXT("Sounds"), TEXT("nRandGlobSound2_AppearRate"),
//            0, &m_objPropertys.nRandGlobSound2_AppearRate
//            );
//
//
//        pIniFile->GetString(
//            TEXT("Sounds"), TEXT("szRandTimebSound1_FilePath"),
//            TEXT(""), m_objPropertys.szRandTimebSound1_FilePath,
//            sizeof(m_objPropertys.szRandTimebSound1_FilePath)
//            );
//        pIniFile->GetInteger(
//            TEXT("Sounds"), TEXT("nRandTimeSound1_Inteval"),
//            0, &m_objPropertys.nRandTimeSound1_Inteval
//            );
//        pIniFile->GetInteger(
//            TEXT("Sounds"), TEXT("nRandTimeSound1_Range"),
//            0, &m_objPropertys.nRandTimeSound1_Range
//            );
//        pIniFile->GetInteger(
//            TEXT("Sounds"), TEXT("nRandTimeSound1_AppearRate"),
//            0, &m_objPropertys.nRandTimeSound1_AppearRate
//            );
//        pIniFile->GetInteger(
//            TEXT("Sounds"), TEXT("nAffectTime1"),
//            0, &m_objPropertys.nAffectTime1
//            );
//
//        pIniFile->GetString(
//            TEXT("Sounds"), TEXT("szRandTimebSound2_FilePath"),
//            TEXT(""), m_objPropertys.szRandTimebSound2_FilePath,
//            sizeof(m_objPropertys.szRandTimebSound2_FilePath)
//            );
//        pIniFile->GetInteger(
//            TEXT("Sounds"), TEXT("nRandTimeSound2_Inteval"),
//            0, &m_objPropertys.nRandTimeSound2_Inteval
//            );
//        pIniFile->GetInteger(
//            TEXT("Sounds"), TEXT("nRandTimeSound2_Rnage"),
//            0, &m_objPropertys.nRandTimeSound2_Rnage
//            );
//        pIniFile->GetInteger(
//            TEXT("Sounds"), TEXT("nRandTimeSound2_AppearRate"),
//            0, &m_objPropertys.nRandTimeSound2_AppearRate
//            );
//        pIniFile->GetInteger(
//            TEXT("Sounds"), TEXT("nAffectTime2"),
//            0, &m_objPropertys.nAffectTime2
//            );
//
//        // ==================显示相关(Display)================
//        pIniFile->GetFloat(
//            TEXT("Display"), TEXT("fVisibleInFog"), 
//            0, &m_objPropertys.fVisibleInFog
//            );
//        pIniFile->GetInteger(
//            TEXT("Display"), TEXT("bAutoProduceObstacle"), 
//            m_objPropertys.bAutoProduceObstacle,
//            &m_objPropertys.bAutoProduceObstacle
//            );
//        pIniFile->GetInteger(
//            TEXT("Display"), TEXT("bObscatleCamera"), 
//            m_objPropertys.bObscatleCamera,
//            &m_objPropertys.bObscatleCamera
//            );
//        pIniFile->GetInteger(
//            TEXT("Display"), TEXT("bAffectByTime"), 
//            m_objPropertys.bAffectByTime,
//            &m_objPropertys.bAffectByTime
//            );
//        pIniFile->GetInteger(
//            TEXT("Display"), TEXT("bAutoConnect"), 
//            m_objPropertys.bAutoConnect,
//            &m_objPropertys.bAutoConnect
//            );
//        pIniFile->GetInteger(
//            TEXT("Display"), TEXT("bTransparence"), 
//            m_objPropertys.bTransparence,
//            &m_objPropertys.bTransparence
//            );
//        pIniFile->GetInteger(
//            TEXT("Display"), TEXT("nEyeablePRI"), 
//            0, &m_objPropertys.nEyeablePRI
//            );
//        pIniFile->GetInteger(
//            TEXT("Display"), TEXT("nObscatleType"), 
//            0, &m_objPropertys.nObscatleType
//            );
//        pIniFile->GetInteger(
//            TEXT("Display"), TEXT("nTime1"), 
//            0, &m_objPropertys.nTime1
//            );
//        pIniFile->GetInteger(
//            TEXT("Display"), TEXT("nTime2"), 
//            0, &m_objPropertys.nTime2
//            );
//
//        pIniFile->GetMultiInteger( 
//            TEXT("Display"), TEXT("dwGlobColor"), nColorARGB, 4 
//            );
//        m_objPropertys.dwGlobColor = D3DCOLOR_ARGB(
//            nColorARGB[0], nColorARGB[1], nColorARGB[2], nColorARGB[3]
//        );
//        pIniFile->GetMultiInteger( 
//            TEXT("Display"), TEXT("dwPartColor"), nColorARGB, 4 
//            );
//        m_objPropertys.dwPartColor = D3DCOLOR_ARGB(
//            nColorARGB[0], nColorARGB[1], nColorARGB[2], nColorARGB[3]
//        );
//
//        // ===============文字显示相关(Text)================
//        pIniFile->GetString(
//            TEXT("Editer"), TEXT("szName"), TEXT(""),
//            m_objPropertys.szName, sizeof(m_objPropertys.szName)
//            );
//        pIniFile->GetString(
//            TEXT("Editer"), TEXT("szSuffix"), TEXT(""),
//            m_objPropertys.szSuffix, sizeof(m_objPropertys.szSuffix)
//            );
//        pIniFile->GetString(
//            TEXT("Editer"), TEXT("szNote"), TEXT(""),
//            m_objPropertys.szNote, sizeof(m_objPropertys.szNote)
//            );
//        pIniFile->GetString(
//            TEXT("Editer"), TEXT("szKeyWord"), TEXT(""),
//            m_objPropertys.szKeyWord, sizeof(m_objPropertys.szKeyWord)
//            );
//        pIniFile->GetInteger(
//            TEXT("Editer"), TEXT("nViewClass"),
//            0, &m_objPropertys.nViewClass
//            );
//    }
//    else
//    {
//        //ZeroMemory(&m_objPropertys, sizeof(m_objPropertys));    
//        sprintf(
//            m_objPropertys.szName, TEXT("%s"), m_scName.c_str()
//            );
//    }
//
//
//    m_objPropertys.bInitallized = TRUE;
//    hResult = S_OK;
//Exit0:
//    KG_COM_RELEASE(pIniFile);
//    return hResult;
//} 

//HRESULT KG3DModelSpeedTree::SavePropertyToIni(const char cszFileName[])
//{
//    HRESULT hResult  = E_FAIL;
//
//    if (cszFileName) 
//        m_lpLeafsData->m_strPropertyFileName = cszFileName;
//	KG3DOBJProperty& m_objPropertys = m_lpLeafsData->m_objPropertys;
//
//    IIniFile* pIniFile = g_OpenIniFile(m_lpLeafsData->m_strPropertyFileName.c_str(), false, true);
//    KG_PROCESS_ERROR(pIniFile);
//
//    // =============声音相关(Sound)================
//    if (m_objPropertys.bInitallized)
//    {
//        int nColorARGB[4];
//        pIniFile->WriteString(
//            TEXT("Sounds"), TEXT("szRandGlobSound1_FilePath"),
//            m_objPropertys.szRandGlobSound1_FilePath
//            );
//        pIniFile->WriteInteger(
//            TEXT("Sounds"), TEXT("nRandGlobSound1_Inteval"),
//            m_objPropertys.nRandGlobSound1_Inteval
//            );
//        pIniFile->WriteInteger(
//            TEXT("Sounds"), TEXT("nRandGlobSound1_Range"),
//            m_objPropertys.nRandGlobSound1_Range
//            );
//        pIniFile->WriteInteger(
//            TEXT("Sounds"), TEXT("nRandGlobSound1_AppearRate"),
//            m_objPropertys.nRandGlobSound1_AppearRate
//            );
//
//        pIniFile->WriteString(
//            TEXT("Sounds"), TEXT("szRandGlobSound2_FilePath"),
//            m_objPropertys.szRandGlobSound2_FilePath
//            );
//        pIniFile->WriteInteger(
//            TEXT("Sounds"), TEXT("nRandGlobSound2_Inteval"),
//            m_objPropertys.nRandGlobSound2_Inteval
//            );
//        pIniFile->WriteInteger(
//            TEXT("Sounds"), TEXT("nRandGlobSound2_Range"),
//            m_objPropertys.nRandGlobSound2_Range
//            );
//        pIniFile->WriteInteger(
//            TEXT("Sounds"), TEXT("nRandGlobSound2_AppearRate"),
//            m_objPropertys.nRandGlobSound2_AppearRate
//            );
//
//
//        pIniFile->WriteString(
//            TEXT("Sounds"), TEXT("szRandTimebSound1_FilePath"),
//            m_objPropertys.szRandTimebSound1_FilePath
//            );
//        pIniFile->WriteInteger(
//            TEXT("Sounds"), TEXT("nRandTimeSound1_Inteval"),
//            m_objPropertys.nRandTimeSound1_Inteval
//            );
//        pIniFile->WriteInteger(
//            TEXT("Sounds"), TEXT("nRandTimeSound1_Range"),
//            m_objPropertys.nRandTimeSound1_Range
//            );
//        pIniFile->WriteInteger(
//            TEXT("Sounds"), TEXT("nRandTimeSound1_AppearRate"),
//            m_objPropertys.nRandTimeSound1_AppearRate
//            );
//        pIniFile->WriteInteger(
//            TEXT("Sounds"), TEXT("nAffectTime1"),
//            m_objPropertys.nAffectTime1
//            );
//
//        pIniFile->WriteString(
//            TEXT("Sounds"), TEXT("szRandTimebSound2_FilePath"),
//            m_objPropertys.szRandTimebSound2_FilePath
//            );
//        pIniFile->WriteInteger(
//            TEXT("Sounds"), TEXT("nRandTimeSound2_Inteval"),
//            m_objPropertys.nRandTimeSound2_Inteval
//            );
//        pIniFile->WriteInteger(
//            TEXT("Sounds"), TEXT("nRandTimeSound2_Rnage"),
//            m_objPropertys.nRandTimeSound2_Rnage
//            );
//        pIniFile->WriteInteger(
//            TEXT("Sounds"), TEXT("nRandTimeSound2_AppearRate"),
//            m_objPropertys.nRandTimeSound2_AppearRate
//            );
//        pIniFile->WriteInteger(
//            TEXT("Sounds"), TEXT("nAffectTime2"),
//            m_objPropertys.nAffectTime2
//            );
//
//        // ==================显示相关(Display)================
//        pIniFile->WriteFloat(
//            TEXT("Display"), TEXT("fVisibleInFog"), 
//            m_objPropertys.fVisibleInFog
//            );
//        pIniFile->WriteInteger(
//            TEXT("Display"), TEXT("bAutoProduceObstacle"), 
//            m_objPropertys.bAutoProduceObstacle
//            );
//        pIniFile->WriteInteger(
//            TEXT("Display"), TEXT("bObscatleCamera"), 
//            m_objPropertys.bObscatleCamera
//            );
//        pIniFile->WriteInteger(
//            TEXT("Display"), TEXT("bAffectByTime"), 
//            m_objPropertys.bAffectByTime
//            );
//        pIniFile->WriteInteger(
//            TEXT("Display"), TEXT("bAutoConnect"), 
//            m_objPropertys.bAutoConnect
//            );
//        pIniFile->WriteInteger(
//            TEXT("Display"), TEXT("bTransparence"), 
//            m_objPropertys.bTransparence
//            );
//        pIniFile->WriteInteger(
//            TEXT("Display"), TEXT("nEyeablePRI"), 
//            m_objPropertys.nEyeablePRI
//            );
//        pIniFile->WriteInteger(
//            TEXT("Display"), TEXT("nObscatleType"), 
//            m_objPropertys.nObscatleType
//            );
//        pIniFile->WriteInteger(
//            TEXT("Display"), TEXT("nTime1"), 
//            m_objPropertys.nTime1
//            );
//        pIniFile->WriteInteger(
//            TEXT("Display"), TEXT("nTime2"), 
//            m_objPropertys.nTime2
//            );
//
//        nColorARGB[0] = (m_objPropertys.dwGlobColor & 0xFF000000)>>24;
//        nColorARGB[1] = (m_objPropertys.dwGlobColor & 0x00FF0000)>>16;
//        nColorARGB[2] = (m_objPropertys.dwGlobColor & 0x0000FF00)>>8;
//        nColorARGB[3] = (m_objPropertys.dwGlobColor & 0x000000FF);
//
//        pIniFile->WriteMultiInteger( 
//            TEXT("Display"), TEXT("dwGlobColor"), nColorARGB, 4 
//            );
//
//        nColorARGB[0] = (m_objPropertys.dwPartColor & 0xFF000000)>>24;
//        nColorARGB[1] = (m_objPropertys.dwPartColor & 0x00FF0000)>>16;
//        nColorARGB[2] = (m_objPropertys.dwPartColor & 0x0000FF00)>>8;
//        nColorARGB[3] = (m_objPropertys.dwPartColor & 0x000000FF);
//
//        pIniFile->WriteMultiInteger( 
//            TEXT("Display"), TEXT("dwPartColor"), nColorARGB, 4 
//            );
//        //m_objPropertys.dwPartColor = D3DCOLOR_ARGB(
//        //    nColorARGB[0], nColorARGB[1], nColorARGB[2], nColorARGB[3]
//        //);
//
//        // ===============文字显示相关(Text)================
//        pIniFile->WriteString(
//            TEXT("Editer"), TEXT("szName"), 
//            m_objPropertys.szName
//            );
//        pIniFile->WriteString(
//            TEXT("Editer"), TEXT("szSuffix"),
//            m_objPropertys.szSuffix
//            );
//        pIniFile->WriteString(
//            TEXT("Editer"), TEXT("szNote"), 
//            m_objPropertys.szNote
//            );
//        pIniFile->WriteString(
//            TEXT("Editer"), TEXT("szKeyWord"),
//            m_objPropertys.szKeyWord
//            );
//        pIniFile->WriteInteger(
//            TEXT("Editer"), TEXT("nViewClass"),
//            m_objPropertys.nViewClass
//            );
//    }
//
//    pIniFile->Save(m_lpLeafsData->m_strPropertyFileName.c_str());
//
//    hResult = S_OK;
//Exit0:
//    KG_COM_RELEASE(pIniFile);
//    return hResult;
//}

HRESULT KG3DSpeedTreeLeafsData::LoadFromFile(IFile* pFile,PVOID pHead,KG3DModelSpeedTree* pTree)
{
	KG3DModelSpeedTree::SpeedTreeFileHeader* pHeader = static_cast<KG3DModelSpeedTree::SpeedTreeFileHeader*>(pHead);
	DWORD dwVersion = 0;
	BOOL bOldVersion = FALSE;
	ClearLeafs();

	pFile->Read(&dwVersion, sizeof(DWORD));
	if (pHeader->dwMark == KG3DModelSpeedTree::dwVersion0)
	{
		m_bNeedRecomputePostionByBone = TRUE;
		bOldVersion = TRUE;
	}

	if (dwVersion == 0)
	{
		for(DWORD i = 0; i < pHeader->dwNumLeaf; i++)
		{
			LPLEAF pLeaf = new Leaf(pTree);
			LeafData* pData = static_cast<Leaf*>(pLeaf);
			pFile->Read(pData, sizeof(LeafData));

			m_ListLeaf.push_back(pLeaf);
			pLeaf->YawPitchRollSpeed.z = 0.0001F;
			pLeaf->YawPitchRoll.z = (rand()%200-100)*0.001F;
		}
	}

	m_bLoaded = TRUE;

	return S_OK;
}

HRESULT KG3DModelSpeedTree::_LoadFromFile( 
	const char cszFileName[], 
	unsigned uFileNameHash, 
	unsigned uOption 
	)
{

	HRESULT hr = E_FAIL; 
	TCHAR strPathName[MAX_PATH];
	TCHAR strLoadFilePath[MAX_PATH];
	TCHAR strDriver[MAX_PATH];
	TCHAR strPath[MAX_PATH];
	TCHAR strFileName[MAX_PATH];
	TCHAR strExt[MAX_PATH];
	DWORD dwBlockEnd = 0;
	std::vector<LPLEAF>::iterator i;

	SpeedTreeFileHeader Header;
	IFile* pFile = NULL;
	DWORD Version  = 0;(void)Version;
	BOOL bReUsed = FALSE;

	//UnInit();

	KG_PROCESS_ERROR(cszFileName[0] != '\0');

	m_uFileHashID = g_FileNameHash(cszFileName);
	SAFE_RELEASE(m_lpLeafsData);
	m_lpLeafsData = g_cSpeedTreeLeafsTable.RequestLeafsData(bReUsed,m_uFileHashID);

	pFile = g_OpenFile(cszFileName);
	KGLOG_PROCESS_ERROR(pFile);



	pFile->Read(&Header, sizeof(SpeedTreeFileHeader));

	Header.OutPut(this);

	///////////////////////////////////////////////////////////////////////////////
	{

		_splitpath_s(cszFileName, 
			strDriver, 
			MAX_PATH,
			strPath, 
			MAX_PATH,
			NULL,
			0,
			NULL,
			0);

		_splitpath_s(Header.szTreeMeshName, 
			NULL, 
			0,
			NULL, 
			0, 
			strFileName, 
			MAX_PATH,
			strExt, 
			MAX_PATH);

		sprintf_s(strPathName, 
			MAX_PATH, 
			"%s%s", 
			strDriver, 
			strPath);

		sprintf_s(strLoadFilePath, 
			MAX_PATH, 
			"%s%s%s", 
			strPathName,
			strFileName,
			strExt);

		hr = LoadTreeMesh(strLoadFilePath, uFileNameHash, uOption,TRUE );
		KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

		_splitpath_s(Header.szTreeMtlName, 
			NULL, 
			0,
			NULL, 
			0,
			strFileName,
			MAX_PATH,
			strExt,
			MAX_PATH);

		sprintf_s(strLoadFilePath, 
			MAX_PATH,
			"%s%s%s",
			strPathName,
			strFileName, 
			strExt);

		hr = LoadTreeMtl(strLoadFilePath, uFileNameHash, uOption);
		KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);
	}
	////////////////////////////////////////////////////////////////////////
	if(!bReUsed)
	{
		m_lpLeafsData->LoadFromFile(pFile,&Header,this);
		TryUpdataLeafVertexBuffer();
	}


	pFile->Read(&dwBlockEnd, sizeof(DWORD));

	_splitpath_s(cszFileName, 
		NULL,
		0,
		NULL,
		0,
		strFileName, 
		MAX_PATH,
		NULL,
		0);

	sprintf_s(strLoadFilePath, 
		MAX_PATH,
		"%s%s.Mtl",
		strPathName, 
		strFileName);

	m_szSpeedTreeMtlName = strLoadFilePath;
	hr = LoadMaterialFromFile(strLoadFilePath, uOption | MLO_MESH_SPEEDTREE);
	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

	ComputeBBox();
	m_BBoxLocal = m_BBox;

	if(m_lpModelTree)
		m_lpModelTree->SetParentObjMatrix(&m_matWorld);

	m_szSpeedTreeName = cszFileName;
	//m_scFullName = cszFileName;
	m_scName = cszFileName;

	/*if(!g_bClient)
	{
		hr = LoadPropertyFromIni(cszFileName);
		KG_COM_PROCESS_ERROR(hr);
	}*/


	// billboard clound 
	/*
	m_bHasBillboardCounld = FALSE;
	TCHAR szFileName[MAX_PATH];
	//wsprintf(szFileName, TEXT("%s%s"), m_scFullName.c_str(), TEXT(".bbc"));
	KG_CUSTOM_HELPERS::StrReplaceExtension(m_scName.c_str(), _T("bbc"), szFileName);
	if (g_IsFileExist(szFileName))
	{
	//wsprintf(szFileName, TEXT("%s%s"), m_scFullName.c_str(), TEXT(".png"));

	KG_CUSTOM_HELPERS::StrReplaceExtension(m_scName.c_str(), _T("png"), szFileName);
	if (g_IsFileExist(szFileName))
	m_bHasBillboardCounld = TRUE;
	}*/
	//<BillboardCloud>	
	m_bBillboardCloudCanBeLoaded = KG3DBillboardCloud::CheckIsBillboardExist(m_scName.c_str());
	//</BillboardCloud>	
	m_bMeshPostLoadProcessed = TRUE;

	hr = S_OK;
Exit0:
	SAFE_RELEASE(pFile);
	return hr;
}

HRESULT KG3DModelSpeedTree::LoadFromFile( 
    const char cszFileName[], 
	unsigned uFileNameHash, 
	unsigned uOption 
)
{
	HRESULT hr = E_FAIL;

	//////////////////////////////////////////////////////////////////////////
	DWORD dwThreadID = ::GetCurrentThreadId();
	if(dwThreadID != g_dwMainThreadID)
	{
		uOption |= MLO_MESH_MULTI;
	}	
	
	m_uOption = uOption | MLO_MESH_SPEEDTREE;
	m_scName  = cszFileName;



	if((uOption & MLO_MESH_MULTI) && (g_eEngineOption.bUseMultiThreadLoad))
	{
		g_cMultiThreadLoader.TryMultiThreadLoad_Model(this);
		hr = S_OK;
	}
	else
	{
		hr = _LoadFromFile(cszFileName,uFileNameHash,uOption);
	}

//Exit0:

	g_nLoadCount++;

	return hr;
}

HRESULT KG3DSpeedTreeLeafsData::ConvertToWorldCoordinate(KG3DModelSpeedTree* pSpeedTree)
{
	HRESULT hr = E_FAIL;
	KGLOG_PROCESS_ERROR(m_bLoaded);

	{
		LEAFIter_t i = m_ListLeaf.begin();
		LEAFIter_t iend =m_ListLeaf.end();
		while (i!=iend)
		{
			Leaf* pLeaf = *i;

			pLeaf->m_pSpeedTree = pSpeedTree;
			pLeaf->ConvertToWorldCoordinate();

			i++;
		}
	}

	hr = E_FAIL;
Exit0:
	return hr;
}

HRESULT KG3DModelSpeedTree::TryUpdataLeafVertexBuffer()
{
	HRESULT hr = E_FAIL;
	KGLOG_PROCESS_ERROR(m_lpLeafsData && m_lpLeafsData->m_bLoaded);

	if(m_lpLeafsData->m_bNeedRecomputePostionByBone)
	{
		BOOL bReady = FALSE;

		if(m_lpModelTree)
		{
			if(m_lpModelTree->IsLoaded() && m_lpMesh)
			{
				bReady = TRUE;
			}
		}

		KG_PROCESS_ERROR(bReady);

		m_lpLeafsData->ConvertToWorldCoordinate(this);

		if(m_lpModelTree)
		{
			hr = m_lpLeafsData->ComputeLeafNormal(m_lpModelTree->m_BBoxLocal.GetCenter());
			KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);
		}

		m_bNeedUpdateVB = FALSE;

		m_lpLeafsData->m_bNeedRecomputePostionByBone = FALSE;

        m_BBoxLocal.Clear();
        ComputeBBox();
        m_bBBoxChanged = TRUE;
	}


	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSpeedTreeLeafsData::AddLeaf(LPLEAF pLeaf)
{
	m_ListLeaf.push_back(pLeaf);
	return S_OK;
}

LPLEAF KG3DModelSpeedTree::Add1Leaf(int BoneIndex)
{
    HRESULT hRetCode = E_FAIL;
    KG3DMesh* pTrunkMesh = NULL;
	LPLEAF pLeaf  = NULL;
	D3DXVECTOR3 vec3Up(0, 1, 0);
    D3DXVECTOR3 vec3Base(0, 0, 0);
	D3DXVECTOR3 vec3Offset;
	D3DXVECTOR3 vec3Position;
	int nBaseBone = 0;
    int nNumBone = 0;
	D3DXVECTOR3 vec3Normal;

	KG_PROCESS_ERROR(m_lpModelTree);
	
	pTrunkMesh = m_lpModelTree->GetMesh();
	KG_PROCESS_ERROR(pTrunkMesh);
    hRetCode = pTrunkMesh->GetNumBones(&nNumBone);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

	KG_PROCESS_ERROR(BoneIndex < nNumBone);

	const KG3DMesh::BoneInfo1& BoneInfo = pTrunkMesh->m_pBoneInfo[BoneIndex];

	pLeaf = new Leaf(this);
	KG_PROCESS_ERROR(pLeaf);

	pLeaf->BoneID = BoneIndex;

	vec3Offset = D3DXVECTOR3((float)(rand() % 60 - 30), (float)(rand() % 60 - 30), (float)(rand() % 60 - 30));
	
    pLeaf->Size = (float)(rand() % 20 + 50);
    pLeaf->TextureUV = rand() % 4;
    pLeaf->Wave = 0.1F;
    pLeaf->YawPitchRollAccelerate = D3DXVECTOR3(0, 0, 0.00001f);

    const D3DXMATRIX& matBone = BoneInfo.CurMatrix;
    pLeaf->OriginPosition = D3DXVECTOR3(matBone._41, matBone._42, matBone._43) + vec3Offset;
    nBaseBone = pTrunkMesh->FindBone("Bone01");
    
    if (nBaseBone != -1 )
    {
       vec3Base = D3DXVECTOR3(matBone._41, matBone._42, matBone._43);
    }

    vec3Normal = pLeaf->OriginPosition - D3DXVec3Dot(&vec3Position, &vec3Up) * vec3Up - vec3Base;
	D3DXVec3Normalize(&pLeaf->NormalOffset, &vec3Normal);
	m_lpLeafsData->AddLeaf(pLeaf);

    CreateVerticesBuffer();

	return pLeaf;
Exit0:
    SAFE_DELETE(pLeaf);
	return NULL;
}

//HRESULT KG3DModelSpeedTree::RenderShadowMap(LPD3DXEFFECT &lpEffect,const D3DXMATRIX &matVP,const D3DXMATRIX &matWVP)
//{
//    HRESULT hr = E_FAIL;
//    HRESULT hRetCode = E_FAIL;
//	GraphicsStruct::RenderState cull(D3DRS_CULLMODE,D3DCULL_NONE);
//    
//	KGLOG_PROCESS_ERROR(m_lpLeafsData);
//	UpdatePositionBufferShadowMap((UINT)m_lpLeafsData->m_ListLeaf.size());
//
//    LPDIRECT3DTEXTURE9 lptexture = m_lpMaterial->GetSpeedTreeLeafTexture(0);
//    //assert(lptexture);
//	if (!lptexture)
//		OutputDebugString("lptexture null, KG3DModelSpeedTree.cpp line 963\r\n");
//    hr = lpEffect->SetTexture("EntityTexture",lptexture);
//	hr = lpEffect->SetMatrix("projectwvpmat",&matVP);//这里不需要世界变换矩阵，前面的函数已经把树叶按照世界坐标摆好了
//	hr = lpEffect->CommitChanges();
//
//	//////////////////////////////////////////////////////////////////////////
//    // 渲染叶子
//    UINT uNum = (UINT)m_lpLeafsData->m_ListLeaf.size();
//    hr = g_pd3dDevice->SetStreamSource( 
//        0, 
//        m_lpLeafsData->m_lpVBufShadowMap, 
//        0, 
//        sizeof(VFormat::FACES_NORMAL_TEXTURE1));
//    KG_COM_PROCESS_ERROR(hr);
//    g_pd3dDevice->SetFVF(D3DFVF_Faces_Normal_Texture1);
//
//
//    hr = g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0 , uNum * 2);
//
//	//////////////////////////////////////////////////////////////////////////
//    // 渲染树干
//    hr = lpEffect->SetMatrix("projectwvpmat",&matWVP);
//    hr = lpEffect->CommitChanges();
//
//    m_lpModelTree->RenderShadowMap(lpEffect,matVP,matWVP);
//
//    hRetCode = S_OK;
//Exit0:
//    return hRetCode;
//}

HRESULT KG3DModelSpeedTree::SetPointLightInfo(KG3D_PointLight_Info& vInfo)
{
    KG3DModel::SetPointLightInfo(vInfo);

    if (m_lpModelTree)
        m_lpModelTree->SetPointLightInfo(vInfo);

    return S_OK;
}

HRESULT KG3DModelSpeedTree::RenderLeafsBegin(int nPass,KG3DLight& rLight,KG3DTerrainConverMap* pConverMap,BOOL bUpdateTime,float fTrans)
{
    HRESULT hr = E_FAIL;
	D3DXMATRIX matWorld, matView, matProj, matViewProj;

	D3DXVECTOR4 FogInfo;
	DWORD dwNowTime = g_cGraphicsTool.GetNowTime();
	LPD3DXEFFECT lpEffect = g_GetDefaultShaderGroup().GetDefaultEffect(DEFST_SPEED_TREE);
	KG_PROCESS_ERROR(lpEffect);

	g_pd3dDevice->GetTransform(D3DTS_VIEW, &matView);
	g_pd3dDevice->GetTransform(D3DTS_PROJECTION, &matProj);
    
	//KG3DCamera* pCamera = g_cGraphicsTool.GetSceneMainCamera();

	//if(pCamera)
    {
		
        D3DXVECTOR3 vLeft(-matView._11, -matView._21, -matView._31);
        D3DXVECTOR3 vUp(matView._12, matView._22, matView._32);
		D3DXVECTOR3 vFront(matView._13, matView._23, matView._33);

        matViewProj = matView * matProj;

        hr = lpEffect->SetTechnique("Tech");

        lpEffect->SetMatrix("mViewProj", &matViewProj);

        lpEffect->SetFloatArray("vCameraLeft", (float*)&vLeft, 3);
        lpEffect->SetFloatArray("vCameraUp", (float*)&vUp, 3);
		lpEffect->SetFloatArray("vCameraFront", (float*)&vFront, 3);
		lpEffect->SetFloat("fTrans",fTrans);

        if(bUpdateTime)
            lpEffect->SetFloat("Time",dwNowTime * 0.001F);
    }

	UINT uiNumPasses = 0;
	lpEffect->Begin(&uiNumPasses, 0);

	lpEffect->BeginPass(nPass);

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DModelSpeedTree::ApplyLeafsMaterial()
{
    HRESULT hrResult = E_FAIL;
    LPD3DXEFFECT lpEffect = g_GetDefaultShaderGroup().GetDefaultEffect(DEFST_SPEED_TREE);
    _ASSERTE(NULL != lpEffect);
    KG_PROCESS_ERROR(m_lpLeafsData);
    KG_PROCESS_ERROR(m_lpLeafsData->m_lpLeafVertexBuffer);

    if (m_lpMaterial && m_lpMaterial->IsResourceReady(TRUE))
    {
        KG3DMaterial::LPMATERIAL pSubMtl = m_lpMaterial->GetSubMaterial(0);
        if (pSubMtl)
        {
            KG3DTexture* pTexture = pSubMtl->GetSubMeshTexture();
            KG_PROCESS_ERROR(pTexture);

            pTexture->SetTexture(g_pd3dDevice,0);

            if(g_cEngineOption.bEnableSpeedTreeNormalMap)
            {
                KG3DTexture* pNormal = pSubMtl->GetSubMeshTextureNormal();
                if(pNormal)
                    lpEffect->SetTexture("tNormal", pNormal->GetTexture());
            }
            g_pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
        }
    }

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KG3DModelSpeedTree::RenderLeafsEnd()
{
	LPD3DXEFFECT lpEffect = g_GetDefaultShaderGroup().GetDefaultEffect(DEFST_SPEED_TREE);
	_ASSERTE(NULL != lpEffect);
	lpEffect->EndPass();
	lpEffect->End();

    return S_OK;
}

HRESULT KG3DModelSpeedTree::RenderLeafsWithoutEffectAndMtlChange()
{
	if (!g_bRenderTree)
		return S_OK;
	LPD3DXEFFECT lpEffect = g_GetDefaultShaderGroup().GetDefaultEffect(DEFST_SPEED_TREE);
	_ASSERTE(NULL != lpEffect);
	KG_PROCESS_ERROR(m_lpLeafsData);
	KG_PROCESS_ERROR(m_lpLeafsData->m_lpLeafVertexBuffer);

	//////////////////////////////////////////////////////////////////////////
    g_SetPointLighting(lpEffect, m_lpPointLightInfo, 5, false);

	lpEffect->SetMatrix("mWorld", &m_matWorld);

	float fScale = m_fLeafScale * g_fSpeedTreeLeafScale * 1.5f;
	lpEffect->SetFloat("Scale", fScale);

    lpEffect->SetFloat("Alpha",m_fAlpha);
	lpEffect->SetBool("bAnimation", g_cEngineOption.bGrassAnimation);
	//////////////////////////////////////////////////////////////////////////
	//render leaf
    g_pd3dDevice->SetVertexDeclaration(m_lpVertexDeclaration);
    g_pd3dDevice->SetStreamSource(0, m_lpLeafsData->m_lpLeafVertexBuffer, 0, sizeof(_LEAF_VERTEX));

	lpEffect->CommitChanges();

	DWORD dwSrcCount = static_cast<DWORD>(m_lpLeafsData->GetLeafCount()) * 2;
	DWORD dwCount = min(dwSrcCount,(DWORD)(dwSrcCount / g_cEngineOption.nVegetationDensity * m_fLOD));
	
	if(dwCount)
		g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0 , dwCount);

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DModelSpeedTree::RenderLeafs()
{
    HRESULT hrResult   = E_FAIL;
    HRESULT hrRetCode  = E_FAIL;
    BOOL    bBeginFlag = FALSE;

	if (!g_bRenderTree)
		return S_OK;

	KG3DScene* pScene = g_cGraphicsTool.GetCurScene();

	if(pScene)
	{
		KG3DLight& rLight = g_cGraphicsTool.GetSunLight(*pScene);

		hrRetCode = RenderLeafsBegin(FALSE,rLight,NULL,FALSE,0);
        KG_COM_PROCESS_ERROR(hrRetCode);

        bBeginFlag = TRUE;

        hrRetCode = ApplyLeafsMaterial();
        KG_COM_PROCESS_ERROR(hrRetCode);

		hrRetCode = RenderLeafsWithoutEffectAndMtlChange();
        KG_COM_PROCESS_ERROR(hrRetCode);

		hrRetCode = RenderLeafsEnd();
        KG_COM_PROCESS_ERROR(hrRetCode);
	}

    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        if (bBeginFlag)
        {
            RenderLeafsEnd();
            bBeginFlag = FALSE;
        }
    }
	return hrResult;
}

HRESULT KG3DModelSpeedTree::RenderTreeModel()
{
	if (!g_bRenderTree)
		return S_OK;
	KG_PROCESS_ERROR(m_lpModelTree);

    SAFE_RENDER(m_lpModelTree, 0, NULL);
    if (m_bNeeDrawBone)
    {
        KG3DMesh* pTreeMesh = m_lpModelTree->GetMesh();
        if(pTreeMesh)
        {
            pTreeMesh->DrawBones(NULL);
        }
    }

    return S_OK;
Exit0:
	return E_FAIL;
}


bool KG3DModelSpeedTree::IsFaraway()
{
    KG3DCamera* pCamera = g_cGraphicsTool.GetCamera();

    if (!pCamera)
        return false;

    D3DXVECTOR3 vCameraPos;
    D3DXVECTOR3 vSelfPos;

    pCamera->GetPosition(&vCameraPos);
    vSelfPos = D3DXVECTOR3(m_matWorld._41, m_matWorld._42, m_matWorld._43);

    FLOAT fDisSq = D3DXVec3Length(&(vSelfPos - vCameraPos));
    if (fDisSq < m_BBox.GetDiameter() * 5.8f)
        return false;

    return true;
}

DWORD KG3DModelSpeedTree::QueryCameraLookAtFace()
{
    KG3DCamera* pCamera = g_cGraphicsTool.GetCamera();
    DWORD dwFace = TREE_FRONT;

    if (!pCamera)
        return dwFace;

    D3DXVECTOR3 vSelfPos = D3DXVECTOR3(m_matWorld._41, m_matWorld._42, m_matWorld._43);
    D3DXVECTOR3 vCamPos;

    pCamera->GetPosition(&vCamPos);
    vCamPos = vCamPos - vSelfPos;

    if (vCamPos.z < 0.f && fabs(vCamPos.z) >= fabs(vCamPos.x))
        dwFace = TREE_FRONT;
    else if (vCamPos.z > 0.f && fabs(vCamPos.z) >= fabs(vCamPos.x)) 
        dwFace = TREE_BACK;
    else if (vCamPos.x < 0.f && fabs(vCamPos.x) >= fabs(vCamPos.z))
        dwFace = TREE_LEFT;
    else if (vCamPos.x > 0.f && fabs(vCamPos.x) >= fabs(vCamPos.z))
        dwFace = TREE_RIGHT;
    else
        dwFace = TREE_UP;

    float max_hor = max(fabs(vCamPos.x), fabs(vCamPos.z));
    if (vCamPos.y / max_hor > 1.1f)
        dwFace = TREE_UP;

    return dwFace;

}

bool operator < (const KG3DModelSpeedTree::_TreeTexInfo& l, const KG3DModelSpeedTree::_TreeTexInfo& r) 
{
    if (l.dwHash < r.dwHash)
        return true;

    if (l.dwHash == r.dwHash)
    {
        if (l.q.x < r.q.x)
            return true;

        if (l.q.x == r.q.x)
        {
            if (l.q.y < r.q.y)
                return true;

            if (l.q.y == r.q.y) 
            {
                if (l.q.z < r.q.z)
                    return true;

                if (l.q.z == r.q.z)
                    return l.q.w < r.q.w;

                return false;
            }

            return false;
        }

        return false;
    }
    return false;
}

IDirect3DTexture9* KG3DModelSpeedTree::GetBillboareTex(DWORD dwFace)
{
    HRESULT hResult = E_FAIL;
    HRESULT hRetCode = E_FAIL;
    IDirect3DTexture9* piTexture = NULL;
    IDirect3DSurface9* piTargetSave = NULL;
    IDirect3DSurface9* piSurface = NULL;
    KG3DCamera* pCamera = NULL;
    D3DXVECTOR3 vSelfPos;
    D3DXVECTOR3 vCenter;
    float fRadio;
    D3DXVECTOR3 vCamRenderPos;
    float fRenderW = 0.f;
    float fRenderH = 0.f;
    D3DXVECTOR3 vCamRenderUP = D3DXVECTOR3(0.f, 1.f, 0.f);
    D3DXVECTOR3 vCetSave;
    D3DXVECTOR3 vPosSave;

    ASSERT(dwFace >= TREE_FRONT && dwFace < TREE_DIR_NUM);
    
    hRetCode = g_pd3dDevice->CreateTexture(64, 
        64,
        1,
        D3DUSAGE_RENDERTARGET,
        D3DFMT_A8R8G8B8, 
        D3DPOOL_DEFAULT,
        &piTexture,//&(texArray[dwFace]),
        NULL
    );
    KGLOG_COM_PROCESS_ERROR(hRetCode);
    ASSERT(piTexture);

    pCamera = g_cGraphicsTool.GetCamera();
    KGLOG_PROCESS_ERROR(pCamera);

    vSelfPos = D3DXVECTOR3(m_matWorld._41, m_matWorld._42, m_matWorld._43);
    vCenter = m_BBox.GetCenter();

    fRadio = max(m_BBox.GetHeight(), max(m_BBox.GetLength(), m_BBox.GetWidth()));
    pCamera->Capture();

    switch (dwFace)
    {
    case TREE_FRONT :
        vCamRenderPos = vCenter + D3DXVECTOR3(0.f, 0.f, -fRadio);
        fRenderW = m_BBox.GetLength();
        fRenderH = m_BBox.GetHeight();
        break;
    case TREE_BACK :
        vCamRenderPos = vCenter + D3DXVECTOR3(0.f, 0.f,  fRadio);
        fRenderW = m_BBox.GetLength();
        fRenderH = m_BBox.GetHeight();
        break;
    case TREE_LEFT :
        vCamRenderPos = vCenter + D3DXVECTOR3(-fRadio, 0.f, 0.f);
        fRenderW = m_BBox.GetWidth();
        fRenderH = m_BBox.GetHeight();
        break;
    case TREE_RIGHT :
        vCamRenderPos = vCenter + D3DXVECTOR3( fRadio, 0.f, 0.f);
        fRenderW = m_BBox.GetWidth();
        fRenderH = m_BBox.GetHeight();
        break;
    case TREE_UP :
        vCamRenderPos = vCenter + D3DXVECTOR3(0.f, fRadio, 0.f);
        vCamRenderUP = D3DXVECTOR3(0.f, 0.f, 1.f);
        fRenderW = m_BBox.GetLength();
        fRenderH = m_BBox.GetWidth();
        break;
    default :
        ASSERT(false);
        break;
    }

    hRetCode = g_pd3dDevice->GetRenderTarget(0, &piTargetSave);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = piTexture->GetSurfaceLevel(0, &piSurface);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    g_pd3dDevice->SetRenderTarget(0, piSurface);

    pCamera->GetPosition(&vPosSave);
    pCamera->GetLookAtPosition(&vCetSave);

    pCamera->SetOrthogonal(fRenderW, fRenderH, 1.f, 10000.f);
    /*pCamera->SetPosition(vCamRenderPos);
    pCamera->SetUpDirection(vCamRenderUP);
    pCamera->SetLookAtPosition(vCenter);*/
	pCamera->SetPositionLookAtUp(&vCamRenderPos, &vCenter, &vCamRenderUP);

    g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.f, 0);
    Render(0, NULL);

    /*pCamera->SetUpDirection(D3DXVECTOR3(0.f, 1.f, 0.f));
    pCamera->SetPosition(vPosSave);
    pCamera->SetLookAtPosition(vCetSave);*/
	pCamera->SetPositionLookAtUp(&vPosSave, &vCetSave, &D3DXVECTOR3(0,1,0));
    pCamera->RestoreCapture();
    g_pd3dDevice->SetRenderTarget(0, piTargetSave);

    hResult = S_OK;
Exit0:
    KG_COM_RELEASE(piSurface);
    KG_COM_RELEASE(piTargetSave);
    if (FAILED(hResult))
    {
        KG_COM_RELEASE(piTexture);
    }
    return piTexture;
}

HRESULT KG3DModelSpeedTree::RenderBillboard()
{
    ASSERT(false);

    KG3DCamera* pCamera = g_cGraphicsTool.GetCamera();

    if (!pCamera)
        return E_FAIL;

    DWORD dwFace = QueryCameraLookAtFace();

    D3DXVECTOR3 vCameraUp = D3DXVECTOR3(0.f, 1.f, 0.f);
    D3DXVECTOR3 vCameraPos;
    D3DXVECTOR3 vCameraFront;
    D3DXVECTOR3 vCameraRight;
    D3DXVECTOR3 vCenter = m_BBox.GetCenter();

    pCamera->GetPosition(&vCameraPos);
    vCameraFront = pCamera->GetCameraFront();
    vCameraRight = pCamera->GetCameraRight();

    float fRenderW = 0.f;
    float fRenderH = 0.f;

    switch (dwFace)
    {
    case TREE_FRONT :
    case TREE_BACK :
        fRenderW = m_BBox.GetLength();
        fRenderH = m_BBox.GetHeight();
        break;
    case TREE_LEFT :
    case TREE_RIGHT :
        fRenderW = m_BBox.GetWidth();
        fRenderH = m_BBox.GetHeight();
        break;
    case TREE_UP :
        fRenderW = m_BBox.GetLength();
        fRenderH = m_BBox.GetWidth();
        pCamera->GetUpDirection(&vCameraUp);
        break;
    default :
        ASSERT(false);
        break;
    }

    D3DXVECTOR3 A;
    D3DXVECTOR3 B;
    D3DXVECTOR3 C;

    if (dwFace == TREE_UP) 
    {
        A = D3DXVECTOR3(m_BBox.A.x, m_BBox.B.y * 0.5f, m_BBox.B.z);
        B = D3DXVECTOR3(m_BBox.B.x, m_BBox.B.y * 0.5f, m_BBox.B.z);
        C = D3DXVECTOR3(m_BBox.A.x, m_BBox.B.y * 0.5f, m_BBox.A.z);
    }
    else
    {
        A = vCenter - vCameraRight * fRenderW * 0.5f + D3DXVECTOR3(0.f, fRenderH * 0.5f, 0.f);
        B = vCenter + vCameraRight * fRenderW * 0.5f + D3DXVECTOR3(0.f, fRenderH * 0.5f, 0.f);
        C = vCenter - vCameraRight * fRenderW * 0.5f - D3DXVECTOR3(0.f, fRenderH * 0.5f, 0.f);
    }

    KG3DRenderState State;
    State.SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    State.SetRenderState(D3DRS_ALPHAREF, 0x00000008E);
    State.SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
    g_cGraphicsTool.DrawRect(&A, &B, &C, 0xffffffff, GetBillboareTex(dwFace));


    return S_OK;
}

HRESULT KG3DModelSpeedTree::UpdateBillboardBuf(VFormat::FACES_DIFFUSE_TEXTURE1* pVer, size_t index, DWORD dwFace)
{
    KG3DCamera* pCamera = g_cGraphicsTool.GetCamera();

    if (!pCamera)
        return E_FAIL;

    D3DXVECTOR3 vCameraUp = D3DXVECTOR3(0.f, 1.f, 0.f);
    D3DXVECTOR3 vCameraPos;
    D3DXVECTOR3 vCameraFront;
    D3DXVECTOR3 vCameraRight;
    D3DXVECTOR3 vCenter = m_BBox.GetCenter();

    pCamera->GetPosition(&vCameraPos);
    vCameraFront = pCamera->GetCameraFront();
    vCameraRight = pCamera->GetCameraRight();

    float fRenderW = 0.f;
    float fRenderH = 0.f;

    switch (dwFace)
    {
    case TREE_FRONT :
    case TREE_BACK :
        fRenderW = m_BBox.GetLength();
        fRenderH = m_BBox.GetHeight();
        break;
    case TREE_LEFT :
    case TREE_RIGHT :
        fRenderW = m_BBox.GetWidth();
        fRenderH = m_BBox.GetHeight();
        break;
    case TREE_UP :
        fRenderW = m_BBox.GetLength();
        fRenderH = m_BBox.GetWidth();
        pCamera->GetUpDirection(&vCameraUp);
        break;
    default :
        ASSERT(false);
        break;
    }

    if (dwFace == TREE_UP) 
    {
        pVer[index * 6 + 0].p = D3DXVECTOR3(m_BBox.A.x, m_BBox.B.y * 0.5f, m_BBox.B.z);
        pVer[index * 6 + 1].p = D3DXVECTOR3(m_BBox.B.x, m_BBox.B.y * 0.5f, m_BBox.B.z);
        pVer[index * 6 + 2].p = D3DXVECTOR3(m_BBox.A.x, m_BBox.B.y * 0.5f, m_BBox.A.z);

        pVer[index * 6 + 3].p = D3DXVECTOR3(m_BBox.A.x, m_BBox.B.y * 0.5f, m_BBox.A.z);
        pVer[index * 6 + 4].p = D3DXVECTOR3(m_BBox.B.x, m_BBox.B.y * 0.5f, m_BBox.B.z);
        pVer[index * 6 + 5].p = D3DXVECTOR3(m_BBox.B.x, m_BBox.B.y * 0.5f, m_BBox.A.z);
    }
    else
    {
        pVer[index * 6 + 0].p = vCenter - vCameraRight * fRenderW * 0.5f + D3DXVECTOR3(0.f, fRenderH * 0.5f, 0.f);
        pVer[index * 6 + 1].p = vCenter + vCameraRight * fRenderW * 0.5f + D3DXVECTOR3(0.f, fRenderH * 0.5f, 0.f);
        pVer[index * 6 + 2].p = vCenter - vCameraRight * fRenderW * 0.5f - D3DXVECTOR3(0.f, fRenderH * 0.5f, 0.f);

        pVer[index * 6 + 3].p = vCenter - vCameraRight * fRenderW * 0.5f - D3DXVECTOR3(0.f, fRenderH * 0.5f, 0.f);
        pVer[index * 6 + 4].p = vCenter + vCameraRight * fRenderW * 0.5f + D3DXVECTOR3(0.f, fRenderH * 0.5f, 0.f);
        pVer[index * 6 + 5].p = vCenter + vCameraRight * fRenderW * 0.5f - D3DXVECTOR3(0.f, fRenderH * 0.5f, 0.f);
    }

    pVer[index * 6 + 0].diffuse = 0xffffffff;
    pVer[index * 6 + 0].tu1 = 0.f;
    pVer[index * 6 + 0].tv1 = 0.f;

    pVer[index * 6 + 1].diffuse = 0xffffffff;
    pVer[index * 6 + 1].tu1 = 1.f;
    pVer[index * 6 + 1].tv1 = 0.f;

    pVer[index * 6 + 2].diffuse = 0xffffffff;
    pVer[index * 6 + 2].tu1 = 0.f;
    pVer[index * 6 + 2].tv1 = 1.f;

    pVer[index * 6 + 3].diffuse = 0xffffffff;
    pVer[index * 6 + 3].tu1 = 0.f;
    pVer[index * 6 + 3].tv1 = 1.f;

    pVer[index * 6 + 4].diffuse = 0xffffffff;
    pVer[index * 6 + 4].tu1 = 1.f;
    pVer[index * 6 + 4].tv1 = 0.f;

    pVer[index * 6 + 5].diffuse = 0xffffffff;
    pVer[index * 6 + 5].tu1 = 1.f;
    pVer[index * 6 + 5].tv1 = 1.f;


    return S_OK;
}

HRESULT KG3DModelSpeedTree::InitBillBoardTree()
{
    for (map<_TreeTexInfo, _TexArray>::iterator i = m_mapBillBoardTexs.begin(); 
         i != m_mapBillBoardTexs.end(); ++i)
    {
        i->second.vTreeRefs[TREE_FRONT].clear();
        i->second.vTreeRefs[TREE_LEFT].clear();
        i->second.vTreeRefs[TREE_RIGHT].clear();
        i->second.vTreeRefs[TREE_BACK].clear();
        i->second.vTreeRefs[TREE_UP].clear();
    }

    return S_OK;
}

HRESULT KG3DModelSpeedTree::AddBillBoardTree(KG3DModelSpeedTree* pTree)
{
    DWORD dwFace = pTree->QueryCameraLookAtFace();
    
    D3DXQUATERNION q;
    D3DXVECTOR3 t;
    D3DXVECTOR3 s;
    
    D3DXMatrixDecompose(&s, &q, &t, &pTree->m_matWorld);
    _TreeTexInfo info = {pTree->m_uFileHashID, q};
    _TexArray& texArray = m_mapBillBoardTexs[info];
    texArray.vTreeRefs[dwFace].push_back(pTree);

    return S_OK;
}


HRESULT KG3DModelSpeedTree::ClearOldTreeTex()
{

    for (map<_TreeTexInfo, _TexArray>::iterator i = m_mapBillBoardTexs.begin(); 
        i != m_mapBillBoardTexs.end(); ++i)
    {
        for (int t = 0; t < TREE_DIR_NUM; ++t)
        {
            if (i->second.vTreeRefs[t].size() == 0)
                SAFE_RELEASE(i->second.TexArray[t]);
        }
    }
    return S_OK;
}

HRESULT KG3DModelSpeedTree::RenderBillboardTree()
{
    KG3DCamera* pCamera = g_cGraphicsTool.GetCamera();

    D3DXVECTOR3 vCamPos;
    pCamera->GetPosition(&vCamPos);
    static D3DXVECTOR3 vCamPrv = vCamPos;
    if (D3DXVec3LengthSq(&(vCamPos - vCamPrv)) > 100000.f * 100000.f)
    {
        vCamPrv = vCamPos;
        ClearOldTreeTex();
    }

    KG3DRenderState State;
    GraphicsStruct::RenderStateCullMode CullMode(D3DCULL_NONE);

    State.SetRenderState( D3DRS_LIGHTING, FALSE );	
    State.SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE);
    State.SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_MODULATE);
    State.SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE);
    State.SetTextureStageState(1,D3DTSS_ALPHAOP,D3DTOP_DISABLE);
    State.SetSamplerState(0,D3DSAMP_MAGFILTER,(D3DTEXTUREFILTERTYPE)g_cEngineOption.nSampMagFilter);
    State.SetSamplerState(0,D3DSAMP_MINFILTER,(D3DTEXTUREFILTERTYPE)g_cEngineOption.nSampMinFilter);
    State.SetSamplerState(0,D3DSAMP_MIPFILTER,(D3DTEXTUREFILTERTYPE)g_cEngineOption.nSampMipFilter);
	State.SetSamplerState(0, D3DSAMP_MAXANISOTROPY ,g_cEngineOption.dwMaxAnisotropy);
    State.SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    State.SetRenderState(D3DRS_ALPHAREF, 0x00000008E);
    State.SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);

    for (map<_TreeTexInfo, _TexArray>::iterator i = m_mapBillBoardTexs.begin(); 
        i != m_mapBillBoardTexs.end(); ++i)
    {
        _TexArray& texArray = i->second;
       
        for (int e = 0; e < TREE_DIR_NUM; ++e) 
        {
            size_t index = 0;
            VFormat::FACES_DIFFUSE_TEXTURE1* pVer = NULL;
            for (size_t t = 0; t < texArray.vTreeRefs[e].size(); ++t)
            {
                if (t == 0)
                {
                    if (texArray.TexArray[e] == NULL)
                        texArray.TexArray[e] = texArray.vTreeRefs[e][t]->GetBillboareTex(e);
                    g_pd3dDevice->SetTexture(0, texArray.TexArray[e]);
                    g_pd3dDevice->SetTexture(1,NULL);
                    g_pd3dDevice->SetTexture(2,NULL);
                }

                if (index == 0) 
                {
                    if (FAILED(m_pBillBoardBuf->Lock(0, 0, (void**)&pVer, D3DLOCK_DISCARD)))
                        return E_FAIL;
                }

                texArray.vTreeRefs[e][t]->UpdateBillboardBuf(pVer, index++, e);
               
                if (index >= 50 || t == texArray.vTreeRefs[e].size() - 1)
                {
                    if (FAILED(m_pBillBoardBuf->Unlock()))
                        return E_FAIL;

                    g_pd3dDevice->SetStreamSource(0, m_pBillBoardBuf, 0, sizeof( VFormat::FACES_DIFFUSE_TEXTURE1));
                    g_pd3dDevice->SetFVF(D3DFVF_Faces_Diffuse_Texture1);
                    g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0 , (UINT)index * 2);

                    index = 0;
                }
            }
        }
    }
    return S_OK;
}

HRESULT KG3DModelSpeedTree::Render(unsigned int uOption, void* pExtInfo)
{
	if (!g_bRenderTree)
		return S_OK;

	if(!IsLoaded())
		return S_OK;

	if (m_bNeedUpdateVB)
		FillLeafVerticesBuffer();

	//渲染树干
	RenderTreeModel();

    RenderLeafs();

    return S_OK;
}

HRESULT KG3DModelSpeedTree::SaveLeaf(LPCSTR pFileName)
{
    FILE* pFile = fopen(pFileName, "wb");
    if(!pFile)
        return E_FAIL;

    SpeedTreeFileHeader Header;
    Header.Input(this);
    DWORD Version = 0;
    DWORD BlockEnd = 0xFFFFFFFF;

	fwrite(&Header,sizeof(SpeedTreeFileHeader),1,pFile);
    fwrite(&Version,sizeof(DWORD),1,pFile);
    LEAFIter_t i = m_lpLeafsData->m_ListLeaf.begin();
    while(i!=m_lpLeafsData->m_ListLeaf.end())
    {
        LPLEAF pLeaf = *i;
        LeafData* pLeafData = static_cast<LeafData*>(pLeaf);
        fwrite(pLeafData,sizeof(LeafData),1,pFile);
        i++;
    }
    fwrite(&BlockEnd,sizeof(DWORD),1,pFile);
    fclose(pFile);

    return S_OK;
}

HRESULT KG3DSpeedTreeLeafsData::ComputeLeafNormal(D3DXVECTOR3 vCenter)
{
	LEAFIter_t i;
	KGLOG_PROCESS_ERROR(m_bLoaded);

	i = m_ListLeaf.begin();

	while(i != m_ListLeaf.end())
	{
		LPLEAF pLeaf = *i;

		D3DXVECTOR3 Normal = pLeaf->Position - vCenter;
		D3DXVec3Normalize(&pLeaf->NormalOffset, &Normal);

		i++;
	}

	return S_OK;
Exit0:
	return E_FAIL;
}

BOOL KG3DModelSpeedTree::IsBBoxChanged()
{
    if (m_bBBoxChanged)
        return TRUE;
    else if (m_lpModelTree)
        return m_lpModelTree->IsBBoxChanged();
    else
        return FALSE;
}

void KG3DModelSpeedTree::SetBBoxChanged(BOOL bBBoxChanged)
{
    if (m_lpModelTree)
    {
        m_lpModelTree->SetBBoxChanged(bBBoxChanged);
        m_BBoxLocal.Clear();
        ComputeBBox();
    }
    m_bBBoxChanged = bBBoxChanged;
}

HRESULT KG3DModelSpeedTree::ComputeBBox()
{
    D3DXVECTOR3 vec3BoxPos[8];
	HRESULT hr = E_FAIL;

	KG_PROCESS_ERROR(m_lpModelTree);
	DWORD dwID = ::GetCurrentThreadId();
	KG_PROCESS_ERROR(dwID == g_dwMainThreadID);

	Helper_UpdateTransformation(TRUE);

	if (!m_BBoxLocal.bInit)
	{
		LEAFIter_t i;
		KG_PROCESS_SUCCESS(!m_lpLeafsData);
		KG_PROCESS_SUCCESS(!m_lpLeafsData->m_bLoaded);

		i = m_lpLeafsData->m_ListLeaf.begin(); 
		m_BBoxLocal.Clear();
		//m_lpModelTree->FrameMove();
		m_lpModelTree->ComputeBBox();
		m_BBoxLocal = m_lpModelTree->m_BBoxLocal;
		m_BBox = m_lpModelTree->m_BBox;
				 
		while(i != m_lpLeafsData->m_ListLeaf.end())
		{
			LPLEAF pLeaf = *i;
			m_BBoxLocal.A.x = min(m_BBoxLocal.A.x, pLeaf->OriginPosition.x - pLeaf->Size);
			m_BBoxLocal.A.y = min(m_BBoxLocal.A.y, pLeaf->OriginPosition.y - pLeaf->Size);
			m_BBoxLocal.A.z = min(m_BBoxLocal.A.z, pLeaf->OriginPosition.z - pLeaf->Size);
			m_BBoxLocal.B.x = max(m_BBoxLocal.B.x, pLeaf->OriginPosition.x + pLeaf->Size);
			m_BBoxLocal.B.y = max(m_BBoxLocal.B.y, pLeaf->OriginPosition.y + pLeaf->Size);
			m_BBoxLocal.B.z = max(m_BBoxLocal.B.z, pLeaf->OriginPosition.z + pLeaf->Size);
		   i++;
		}
	}

	m_BBoxLocal.TransformCoord(vec3BoxPos, &m_matWorld);
	m_BBox.Clear();
	m_BBox.AddPosition(vec3BoxPos, 8);

Exit1:
	hr = S_OK;
Exit0:
    return hr;
}

//HRESULT KG3DModelSpeedTree::UpdateLeafPosition(UINT uNumLeaf)
//{
//	if(!m_lpModelTree)
//		return E_FAIL;
//	KG3DMesh* pMesh = static_cast<KG3DMesh*>(m_lpModelTree->m_lpMesh);
//	if(!pMesh)
//		return E_FAIL;
//
//    D3DXVECTOR3 Left, Up, Front, CPos;
//    KG3DCamera* pCamera = g_cGraphicsTool.GetCamera();
//    Front = pCamera->GetCameraFront();
//    Up    = pCamera->GetCameraUp();
//    Left  = pCamera->GetCameraRight() * -1;
//    pCamera->GetPosition(&CPos);
//
//    D3DXMATRIX CurMatrix;
//    g_pd3dDevice->GetTransform(D3DTS_WORLD, &CurMatrix);
//
//    float TinyTime = 2;
//
//	UINT S = 0;
//    LEAFIter_t i = m_lpLeafsData->m_ListLeaf.begin();
//    while(i != m_lpLeafsData->m_ListLeaf.end())
//    {
//        LPLEAF pLeaf = *i;
//
//        D3DXVec3TransformCoord(&pLeaf->Position,
//			&pLeaf->OriginPosition, 
//			&CurMatrix);
//
//        D3DXVec3TransformNormal(&pLeaf->Normal, 
//			&pLeaf->NormalOffset, 
//			&CurMatrix);
//
//        if((pLeaf->YawPitchRoll.z>0.3F)||(pLeaf->YawPitchRoll.z<-0.3F))
//			pLeaf->YawPitchRollSpeed.z *= -1;
//		pLeaf->YawPitchRoll.z += pLeaf->YawPitchRollSpeed.z;
//
//        i++;
//		S++;
//		if(S>=uNumLeaf)
//			break;
//    }
//
//    return S_OK;
//}

HRESULT KG3DModelSpeedTree::DeleteLeaf(LPLEAF pLeaf)
{
    //m_ListLeaf.remove(pLeaf);
	LEAFIter_t iterRemove = std::find( m_lpLeafsData->m_ListLeaf.begin(), m_lpLeafsData->m_ListLeaf.end(), pLeaf );
	
	if ( iterRemove != m_lpLeafsData->m_ListLeaf.end( ) )
	{
		LEAFIter_t iterLast = m_lpLeafsData->m_ListLeaf.end() - 1;
		
		std::swap( iterRemove, iterLast );
		m_lpLeafsData->m_ListLeaf.erase( iterLast );
	}

    CreateVerticesBuffer();
    SAFE_DELETE(pLeaf);

    return S_OK;
}

HRESULT KG3DModelSpeedTree::Copy(KG3DModel** ppNewModel)
{
    return S_OK;
}

HRESULT KG3DModelSpeedTree::GetMeshFileName(LPSTR pFileName)
{
	wsprintf(pFileName,"%s",m_szSpeedTreeName.c_str());
	return S_OK;
}

HRESULT KG3DModelSpeedTree::GetMaterialFileName(LPSTR pFileName)
{
	wsprintf(pFileName,"%s",m_szSpeedTreeMtlName.c_str());
	return S_OK;
}

HRESULT KG3DModelSpeedTree::GetAnimationFileName(LPSTR pFileName)
{
	wsprintf(pFileName,"");
	return S_OK;
}

HRESULT KG3DModelSpeedTree::CreateAuto(int nLeaf, D3DXVECTOR3 *pScale)
{
	HRESULT hr = E_FAIL;
	int ileaf = 0;
	KG3DMesh* pTrunkMesh = NULL;
	LEAFIter_t i = m_lpLeafsData->m_ListLeaf.begin();
	
	KG_PROCESS_ERROR(m_lpModelTree);
	
	while(i != m_lpLeafsData->m_ListLeaf.end())
	{ 
		LPLEAF pLeaf = *i;
		SAFE_DELETE(pLeaf);
		i++;
	}
	
	m_lpLeafsData->m_ListLeaf.clear();
	pTrunkMesh = m_lpModelTree->GetMesh();
	KG_PROCESS_ERROR(pTrunkMesh);
	KG_PROCESS_ERROR(pTrunkMesh->GetNumBones());
	
	while(ileaf < nLeaf)
	{
		int BoneIndex = rand() % pTrunkMesh->m_dwNumBone;

		const KG3DMesh::BoneInfo1& Bone = pTrunkMesh->m_pBoneInfo[BoneIndex];
		int nParent = pTrunkMesh->FindBone(Bone.strBoneName);

		if(nParent>=0)
		{
			const KG3DMesh::BoneInfo1& Parent = pTrunkMesh->m_pBoneInfo[nParent];

			const D3DXMATRIX& mat1 = Bone.CurMatrix;
			const D3DXMATRIX& mat2 = Parent.CurMatrix;

			D3DXVECTOR3 A(mat1._41, mat1._42, mat1._43);
			D3DXVECTOR3 B(mat2._41, mat2._42, mat2._43);

			LPLEAF pLeaf  = new Leaf(this);
			pLeaf->BoneID = BoneIndex;

			pLeaf->OriginPosition = D3DXVECTOR3((rand() % 100 - 50) * 0.01F * pScale->x, 
				(rand() % 100 - 50) * 0.01F * pScale->x, 
				(rand() % 100 - 50) * 0.01F * pScale->x) + A; 

			pLeaf->Size = pScale->x;
			pLeaf->Size      = (float)(rand() % 20 + 50);
			pLeaf->TextureUV = rand() % 4;
			pLeaf->Wave      = 0.1F;
			pLeaf->YawPitchRollAccelerate = D3DXVECTOR3(0, 0, 0.00001f);
			
			m_lpLeafsData->AddLeaf(pLeaf);

			ileaf++;
		}
	}

	m_lpLeafsData->m_bLoaded = TRUE;

	CreateVerticesBuffer();

	hr = S_OK;
Exit0:
	return hr;
}



//void  KG3DModelSpeedTree::UpdatePositionBufferShadowMap( UINT leafNums)
//{
//    HRESULT hr;
//
//    if (!m_lpLeafsData->m_lpVBufShadowMap)
//    {
//        hr = g_pd3dDevice->CreateVertexBuffer(
//            sizeof(VFormat::_Faces_Normal_Texture1) * (UINT)m_lpLeafsData->m_ListLeaf.size() * 2 * 3,
//            0, 0, D3DPOOL_MANAGED, &m_lpLeafsData->m_lpVBufShadowMap, NULL);
//        if (FAILED(hr))
//        {
//            return;
//        }
//	}
//	//////////////////////////////////////////////////////////////////////////
//	{
//		VFormat::_Faces_Normal_Texture1 pv1;
//
//		_LEAF_VERTEX *pData = NULL;
//		VFormat::_Faces_Normal_Texture1 *pDec = NULL;
//		hr = m_lpLeafsData->m_lpLeafVertexBuffer->Lock(0, 0, (void**)&pData, 0);
//		hr = m_lpLeafsData->m_lpVBufShadowMap->Lock(0,0,(void**)&pDec,0);
//
//		for(size_t i = 0; i < m_lpLeafsData->m_ListLeaf.size() * 6; ++i)
//		{
//			pDec[i].p.x = pData[i].x;
//			pDec[i].p.y = pData[i].y;
//			pDec[i].p.z = pData[i].z;
//
//			pDec[i].Normal.x = pData[i].nx;
//			pDec[i].Normal.y = pData[i].ny;
//			pDec[i].Normal.z = pData[i].nz;
//
//			pDec[i].tu1 = pData[i].tu;
//			pDec[i].tv1 = pData[i].tv;
//		}
//
//		hr = m_lpLeafsData->m_lpLeafVertexBuffer->Unlock();
//		hr = m_lpLeafsData->m_lpVBufShadowMap->Unlock();
//	}
//	//////////////////////////////////////////////////////////////////////////
//    D3DXVECTOR3 Left;
//    D3DXVECTOR3 Up;
//    D3DXVECTOR3 Front;
//    D3DLIGHT9 Light;
//    g_pd3dDevice->GetLight(0,&Light);
//
//    Front = D3DXVECTOR3(Light.Direction.x,Light.Direction.y,Light.Direction.z);
//    D3DXVec3Cross(&Left,&D3DXVECTOR3(0.0f,1.0f,0.0f),&Front);
//    D3DXVec3Normalize(&Left,&Left);
//    D3DXVec3Cross(&Up,&Front,&Left);
//    D3DXVec3Normalize(&Up,&Up);
//
//	//////////////////////////////////////////////////////////////////////////
//    VFormat::FACES_NORMAL_TEXTURE1 * pVer = NULL;
//    if (FAILED(m_lpLeafsData->m_lpVBufShadowMap->Lock(0, 0, (void**)&pVer, 0)))
//    {
//        return ;
//    }
//
//    float const fInvScale = 1 / m_fLeafScale;
//
//    LEAFIter_t iterBegin = m_lpLeafsData->m_ListLeaf.begin();
//    LEAFIter_t iterEnd = iterBegin + (int)leafNums;
//
//	float fScale = m_fLeafScale * g_fSpeedTreeLeafScale;
//
//    for ( float const leafScale = fScale; 
//        iterBegin != iterEnd; 
//        ++ iterBegin 
//        )
//    {
//        LPLEAF iterLeaf = *iterBegin;
//
//        float const fSzie = iterLeaf->Size * leafScale;
//        float const SinA  = fSzie * sinf(iterLeaf->YawPitchRoll.z);
//        float const CosA = fSzie * cosf(iterLeaf->YawPitchRoll.z);
//        float const SinB  = fSzie * sinf(iterLeaf->YawPitchRoll.z+D3DX_PI/2);
//        float const CosB = fSzie * cosf(iterLeaf->YawPitchRoll.z+D3DX_PI/2);
//
//		D3DXVECTOR3 vLeafPos;
//		D3DXVec3TransformCoord(&vLeafPos,&iterLeaf->Position,&m_matWorld);
//
//        D3DXVECTOR3 const A = vLeafPos + CosA * Left + SinA * Up;
//        D3DXVECTOR3 const B = vLeafPos + CosB * Left + SinB * Up;
//        D3DXVECTOR3 const C = vLeafPos - CosA * Left - SinA * Up;
//        D3DXVECTOR3 const D = vLeafPos - CosB * Left - SinB * Up;
//        D3DXVECTOR3 const normal = fInvScale * iterLeaf->Normal;
//
//
//        pVer->p      = A;
//        pVer->Normal = normal;
//        pVer ++;
//
//
//        pVer->p      = B;
//        pVer->Normal = normal;
//        pVer ++;
//
//
//        pVer->p      = D;
//        pVer->Normal = normal;
//        pVer ++;
//
//
//        pVer->p      = D;
//        pVer->Normal = normal;
//        pVer ++;
//
//
//        pVer->p      = B;
//        pVer->Normal = normal;
//        pVer ++;
//
//
//        pVer->p      = C;
//        pVer->Normal = normal;
//        pVer ++;
//
//    }
//
//    m_lpLeafsData->m_lpVBufShadowMap->Unlock();
//
//}

HRESULT KG3DModelSpeedTree::RenderReflect(float fWaterHeight,D3DXPLANE& WaterPlane,D3DXPLANE& WaterPlaneFX)
{
	if (!g_bRenderTree)
		return S_OK;

	g_pd3dDevice->SetRenderState(D3DRS_CLIPPLANEENABLE,D3DCLIPPLANE0);
	g_pd3dDevice->SetClipPlane(0,(float *)&WaterPlaneFX);

	RenderLeafs();
	/*HRESULT hr = E_FAIL;
	UpdateTransformation();
	D3DXMATRIX matSave;
	D3DXMATRIX matIdentity;

	hr = g_pd3dDevice->GetTransform(D3DTS_WORLD, &matSave);
	UINT uNum = (UINT)m_lpLeafsData->m_ListLeaf.size();

	if (m_pParentMatrix)
		m_matWorld = m_Matrix * (*m_pParentMatrix);
	else
		m_matWorld = m_Matrix;

	D3DXMatrixIdentity(&matIdentity);
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &matIdentity);

	if (m_lpMaterial)
	{
		KG3DMaterial::LPMATERIAL pSubMtl = NULL;
		m_lpMaterial->GetSubMaterial(0, &pSubMtl);
		if (pSubMtl)
			g_pd3dDevice->SetTexture(0, pSubMtl->GetSubMeshTexture());
	}

	g_pd3dDevice->SetVertexDeclaration(m_lpVertexDeclaration);
	g_pd3dDevice->SetStreamSource(0, m_lpLeafsData->m_lpLeafVertexBuffer, 0, sizeof(_LEAF_VERTEX));

	lpEffect->SetTechnique("Tech0");

	D3DXMATRIX matWorld, matView, matProj, matViewProj;
	g_pd3dDevice->GetTransform(D3DTS_VIEW, &matView);
	g_pd3dDevice->GetTransform(D3DTS_PROJECTION, &matProj);
	D3DXVECTOR3 vLeft(-matView._11, -matView._21, -matView._31);
	D3DXVECTOR3 vUp(matView._12, matView._22, matView._32);
	matViewProj = matView * matProj;
	lpEffect->SetMatrix("mWorld", &m_matWorld);
	lpEffect->SetMatrix("mViewProj", &matViewProj);
	lpEffect->SetFloatArray("vCameraLeft", (float*)&vLeft, 3);
	lpEffect->SetFloatArray("vCameraUp", (float*)&vUp, 3);
	D3DXVECTOR3 vScale(m_matWorld._11, m_matWorld._12, m_matWorld._13);
	float fScale = D3DXVec3Length(&vScale);

	lpEffect->SetFloat("Scale", fScale);
	D3DXVECTOR4 FogInfo;
	g_pd3dDevice->GetRenderState(D3DRS_FOGSTART,(DWORD*)&FogInfo.x);
	g_pd3dDevice->GetRenderState(D3DRS_FOGEND,(DWORD*)&FogInfo.y);
	g_pd3dDevice->GetRenderState(D3DRS_FOGDENSITY,(DWORD*)&FogInfo.z);
	lpEffect->SetVector("FogInfo", &FogInfo);

	DWORD dwAmbient;
	_ASSERTE(NULL&&"光参应该从g_GetRenderGlobalState取，如果这段代码重新用的话");g_pd3dDevice->GetRenderState(D3DRS_AMBIENT, &dwAmbient);
	D3DLIGHT9 light;
	g_pd3dDevice->GetLight(0, &light);
	D3DXCOLOR amb(dwAmbient);
	lpEffect->SetVector("LightAmbient", (D3DXVECTOR4*)&amb);
	lpEffect->SetFloatArray("LightDirection", (float*)&light.Direction, 3);
	lpEffect->SetVector("LightDiffuse", (D3DXVECTOR4*)&light.Diffuse);

	UINT uiNumPasses = 0;
	lpEffect->Begin(&uiNumPasses, 0);

	for (UINT uiPass = 0; uiPass < uiNumPasses; uiPass++)
	{
		lpEffect->BeginPass(uiPass);
		g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0 , uNum * 2);
		lpEffect->EndPass();
	}

	lpEffect->End()*/;

	g_pd3dDevice->SetRenderState(D3DRS_CLIPPLANEENABLE,FALSE);

	if (m_lpModelTree)
	{
		m_lpModelTree->RenderReflect(fWaterHeight,WaterPlane,WaterPlaneFX);
	}

	//g_pd3dDevice->SetTransform(D3DTS_WORLD, &matSave);

	return S_OK;
}


HRESULT Leaf::Render()
{
	DWORD dwZEnable;
	g_pd3dDevice->GetRenderState(D3DRS_ZENABLE, &dwZEnable);
	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);

	D3DXVec3TransformCoord(&Position, &OriginPosition, &m_pSpeedTree->m_matWorld);
	AABBOX Box;
	Box.AddPosition(Position - D3DXVECTOR3(5,5,5));
	Box.AddPosition(Position + D3DXVECTOR3(5,5,5));
	g_cGraphicsTool.DrawAABBox(&Box,0xFFFFFFFF);

	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, dwZEnable);
	return S_OK;
}

HRESULT Leaf::RenderSelectMark(DWORD dwColor)
{
	KG3DRenderState RenderState;
	D3DXMATRIX matBone;
	static AABBOX Box;

	KG_PROCESS_ERROR(m_pSpeedTree);
	RenderState.SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);

	D3DXVec3TransformCoord(&Position, &OriginPosition, &m_pSpeedTree->m_matWorld);
	Box.Clear();
	Box.AddPosition(Position - D3DXVECTOR3(8,8,8));
	Box.AddPosition(Position + D3DXVECTOR3(8,8,8));
	g_cGraphicsTool.DrawAABBox(&Box, dwColor);

	m_pSpeedTree->GetBoneWorldMatrix(matBone, BoneID);
	g_cGraphicsTool.DrawLine(&D3DXVECTOR3(matBone._41, matBone._42, matBone._43), 
		&Position, 
		dwColor, 
		dwColor);

Exit0:
	return S_OK;
}

HRESULT Leaf::OnDelete()
{
	m_pSpeedTree->DeleteLeaf(this);
	return S_OK;
}

HRESULT Leaf::ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam)//<PROCESS_MESSAGE>
{
	switch(message)
	{
	case WM_KEYDOWN:
		{
			switch(wParam)
			{
			case '1':
				TextureUV = 0;
				break;
			case '2':
				TextureUV = 1;
				break;
			case '3':
				TextureUV = 2;
				break;
			case '4':
				TextureUV = 3;
				break;
			case '5':
				TextureUV = rand() % 4;
				break;
			}
		}
		break;
	case WM_LBUTTONUP:
		{
			//m_pSpeedTree->ComputeLeafOffsetPosition();
		}
		break;
	}

	//m_pSpeedTree->FillLeafVerticesBuffer();
	return S_OK;
}

HRESULT KG3DModelSpeedTree::SaveToFile(const TCHAR cszFileName[], UINT uOption)
{
	HRESULT hr = E_FAIL;
    HRESULT hRetCode = E_FAIL;
	TCHAR strDriver[MAX_PATH];
	TCHAR strPath[MAX_PATH];
	TCHAR strFile[MAX_PATH];
	TCHAR strExt[MAX_PATH];
	TCHAR strSpdTreeFile[MAX_PATH];
	//Speed tree file ext is XXX.mesh.STree and XXX.mesh.mtl
    std::string strTreeMeshNameSave = m_szTreeMeshName;
    std::string strTreeeMtlNameSave = m_szTreeMtlName;

	FrameMove();

	KG_PROCESS_ERROR(cszFileName);
	_splitpath(cszFileName, strDriver, strPath, strFile, strExt);
	
	sprintf_s(strSpdTreeFile, MAX_PATH, "%s%s%s.mtl", strDriver, strPath, strFile);
	hr = m_lpMaterial->SaveToFile(strSpdTreeFile, 0);
	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

		
	KG_PROCESS_ERROR(m_lpModelTree);
	if (uOption == ENU_SAVE_AS)
	{
        TCHAR strFileName[MAX_PATH];
		TCHAR *pModify = NULL;
		strcpy_s(strFileName, 
			MAX_PATH,
			strFile);
		pModify = strrchr(strFileName, '.');
		if (pModify)
		{
			*pModify = '\0';
		}
		sprintf_s(strSpdTreeFile, MAX_PATH, "%s%s%s", strDriver, strPath, strFileName);
        m_szTreeMeshName = strSpdTreeFile;
        m_szTreeMtlName = strSpdTreeFile;
        m_szTreeMeshName += ".mesh";
        m_szTreeMtlName += ".mtl";
    }

	hr = m_lpModelTree->SaveToFile(m_szTreeMeshName.c_str(), 0/*MODEL_SAVEOPT_SPEEDTREE*/);
	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);
	
	sprintf_s(strSpdTreeFile, 
		MAX_PATH, 
		"%s%s%s%s",
		strDriver, 
		strPath, 
		strFile,
		strExt);

	hr = SaveLeaf(strSpdTreeFile);
	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);
    
    hRetCode = S_OK;
Exit0:
	return hRetCode;
}

HRESULT KG3DModelSpeedTree::GetBoneMatrix(unsigned int uIndex, 
										  D3DXMATRIX &Mat, 
										  D3DXVECTOR3* pvec3LocalOffset /*= NULL*/)
{
	return m_lpModelTree->GetBoneMatrix(uIndex, &Mat, pvec3LocalOffset);
}

HRESULT KG3DModelSpeedTree::GetSocketMatrix(unsigned int uIndex, 
											D3DXMATRIX& mat,
											D3DXVECTOR3* pvec3LocalOffset /* = NULL*/)
{
	return m_lpModelTree->GetSocketMatrix(uIndex, mat, pvec3LocalOffset);
}

void KG3DModelSpeedTree::GetProfile(DWORD& dwNumFacesTotal,DWORD& dwNumVertices,DWORD& dwNumTextures,DWORD& dwNumID)
{
	if(m_lpModelTree)
	{
		m_lpModelTree->GetProfile(dwNumFacesTotal,dwNumVertices,dwNumTextures,dwNumID);
	}
	dwNumFacesTotal += (DWORD)m_lpLeafsData->m_ListLeaf.size()*2;
	dwNumVertices += (DWORD)m_lpLeafsData->m_ListLeaf.size()*4;
	dwNumTextures++;
	dwNumID++;
}

void Leaf::ConvertToWorldCoordinate()
{
    HRESULT hRetCode = E_FAIL;
    int nNumBone = 0;
	KG3DMesh* pTrunkMesh = NULL;
	KGLOG_PROCESS_ERROR(m_pSpeedTree);

	pTrunkMesh = m_pSpeedTree->GetMesh();
	KGLOG_PROCESS_ERROR(pTrunkMesh);
    
    hRetCode = pTrunkMesh->GetNumBones(&nNumBone);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

	KG_PROCESS_ERROR(BoneID < nNumBone);

	const KG3DMesh::BoneInfo1& Bone = pTrunkMesh->m_pBoneInfo[BoneID];

	D3DXVec3TransformCoord(&OriginPosition, &OriginPosition, &Bone.CurMatrix);
	D3DXVec3TransformCoord(&Position, &OriginPosition, &m_pSpeedTree->m_matWorld);
Exit0:
	return;
}

void KG3DModelSpeedTree::ReclusterLeaf()
{
	for (size_t i = 0; i < m_lpLeafsData->m_ListLeaf.size(); i++)
	{
		Helper_ReclusterLeaf(*m_lpLeafsData->m_ListLeaf[i]);
	}
}

void KG3DModelSpeedTree::Helper_ReclusterLeaf(Leaf& L)
{
	KG3DMesh* pMesh = NULL;
	KG_PROCESS_ERROR(m_lpModelTree);
	
	pMesh = m_lpModelTree->GetMesh();
	KG_PROCESS_ERROR(pMesh);
	
	float fShortDis = 9999999999.f;

	for (DWORD i = 0; i < pMesh->m_dwNumBone; i++)
	{
		const D3DXMATRIX& BoneMat = pMesh->m_pBoneInfo[i].CurMatrix;
		D3DXVECTOR3 vec3BonePos = D3DXVECTOR3(BoneMat._41, BoneMat._42, BoneMat._43);
		
		float fDistance = D3DXVec3Length(&(L.OriginPosition - vec3BonePos));
		if (fDistance < fShortDis)
		{
			fShortDis = fDistance;
			L.BoneID = i;
		}
	}
Exit0:
	return;
}


void KG3DModelSpeedTree::ExcludeLeaf()
{
	for (size_t i = 0; i < m_lpLeafsData->m_ListLeaf.size(); i++)
	{
		//m_ListLeaf[i]
	}
}

void KG3DModelSpeedTree::CheckLeafSpan(const Leaf *pLeaf, size_t uEndIndex, float fThreshold)
{
	KG_PROCESS_ERROR(pLeaf);
	for (size_t i = 0; i < uEndIndex; i++)
	{
		//float fm_ListLeaf[i].Position
	}
Exit0:
	return;
}



HRESULT Leaf::UpdateLeafOriginPosition()
{
    HRESULT hResult = E_FAIL;
	D3DXVECTOR3 vec3Root;
	KG_PROCESS_ERROR(m_pSpeedTree);

	vec3Root = D3DXVECTOR3(m_pSpeedTree->m_matWorld._41, m_pSpeedTree->m_matWorld._42, m_pSpeedTree->m_matWorld._43);
	OriginPosition = Position - vec3Root;
    hResult = S_OK;
Exit0:
	return hResult;
}

void KG3DModelSpeedTree::GetBoneWorldMatrix(D3DXMATRIX& matBone, unsigned int uIndex)
{
    HRESULT hRetCode = E_FAIL;
    int nNumBone = 0;
	KG3DMesh *pTrunk = NULL;
	KG_PROCESS_ERROR(m_lpModelTree);

	pTrunk = m_lpModelTree->GetMesh();
	KG_PROCESS_ERROR(pTrunk);
    hRetCode = pTrunk->GetNumBones(&nNumBone);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

	KG_PROCESS_ERROR(static_cast<unsigned int>(nNumBone) > uIndex);

	matBone = pTrunk->m_pBoneInfo[uIndex].CurMatrix * m_matWorld;
Exit0:
	return;
}


HRESULT KG3DModelSpeedTree::SetLocalBBoxDirty()
{
	m_BBoxLocal.bInit = FALSE;
    return S_OK;
}

HRESULT KG3DModelSpeedTree::LoadLeafFromFile(LPCTSTR strFileName)
{
	HRESULT hr = E_FAIL;
	IFile *pFile = NULL;
	SpeedTreeFileHeader Header;
	TCHAR strDriver[MAX_PATH];
	TCHAR strPath[MAX_PATH];
	TCHAR strFile[MAX_PATH];
	TCHAR strMtl[MAX_PATH];
	DWORD dwVersion = 0;

	std::vector<LPLEAF>::iterator i = m_lpLeafsData->m_ListLeaf.begin();

	KG_PROCESS_ERROR(strFileName);
	
	pFile = g_OpenFile(strFileName);
	KG_PROCESS_ERROR(pFile);
	
	while (i != m_lpLeafsData->m_ListLeaf.end())
	{
		LPLEAF pLeaf = *i;
		SAFE_DELETE(pLeaf);
		i++;
	}
	m_lpLeafsData->m_ListLeaf.clear();

	pFile->Read(&Header, sizeof(SpeedTreeFileHeader));

	pFile->Read(&dwVersion, sizeof(DWORD));
	if (dwVersion == 0)
	{
		for(DWORD i = 0; i < Header.dwNumLeaf; i++)
		{
			LPLEAF pLeaf = new Leaf(this);
            pFile->Read(static_cast<LeafData*>(pLeaf), sizeof(LeafData));
			if (Header.dwMark == dwVersion0)
			{
				pLeaf->ConvertToWorldCoordinate();
			}

			m_lpLeafsData->m_ListLeaf.push_back(pLeaf);
			pLeaf->YawPitchRollSpeed.z = 0.0001F;
			pLeaf->YawPitchRoll.z = (rand()%200-100)*0.001F;
		}
	}

	m_lpLeafsData->m_bLoaded = TRUE;

	hr = CreateVerticesBuffer();
	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

	_splitpath_s(strFileName, 
		strDriver,
		MAX_PATH,
		strPath,
		MAX_PATH,
		strFile, 
		MAX_PATH,
		NULL,
		0);

	sprintf_s(strMtl, 
		MAX_PATH,
		"%s%s%s.Mtl",
		strDriver, 
		strPath,
		strFile);

	m_szSpeedTreeMtlName = strMtl;
	hr = LoadMaterialFromFile(strMtl, 0);
	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

	if(m_lpModelTree && m_lpLeafsData)
	{
		hr = m_lpLeafsData->ComputeLeafNormal(m_lpModelTree->m_BBoxLocal.GetCenter());
		KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);
	}

	m_BBoxLocal.bInit = FALSE;
	ComputeBBox();
	ReclusterLeaf();

	hr = S_OK;
Exit0:
	SAFE_RELEASE(pFile);

	if (FAILED(hr))
	{
		if (!strFileName)
        {
            KGLogPrintf(KGLOG_ERR, "Invalidate file name.");
        }
        else
        {
            KGLogPrintf(KGLOG_ERR, "KG3DModelSpeedTree::LoadLeafFromFile(%s) failed.", strFileName);
        }
	}
	return hr;
}


void Leaf::SetDirtyFlag()
{
	KG_PROCESS_ERROR(m_pSpeedTree);
	m_pSpeedTree->m_bNeedUpdateVB = TRUE;
Exit0:
	return;
}


HRESULT KG3DModelSpeedTree::SetEditMode(BOOL bEditMode, 
									 IEKG3DSceneModelEditor* pSceneA)
{
    HRESULT hResult = E_FAIL;
	m_bEditMode = bEditMode;

	KG3DSceneModelEditor* pModelEditor = static_cast<KG3DSceneModelEditor*>(pSceneA);
	KG_PROCESS_ERROR(pModelEditor);

	KG3DSceneModelEditor::KG3DSpeedTreeModifier* pSpdTreeEditor = pModelEditor->GetSpeedTreeModifier();
	KG_PROCESS_ERROR(pSpdTreeEditor);
	KG_PROCESS_SUCCESS(!m_lpLeafsData);

	if (m_bEditMode)
	{
		for (size_t i = 0; i < m_lpLeafsData->m_ListLeaf.size(); i++)
		{
			//pModelEditor->RemoveSelectedEntity(SCENEENTITY_SPEEDTREELEAF, m_lpLeafsData->m_ListLeaf[i]);
			//pModelEditor->RemoveSelectableEntity(KG3DSceneEntity(SCENEENTITY_SPEEDTREELEAF, m_lpLeafsData->m_ListLeaf[i]));
			pModelEditor->RemoveRenderEntity(KG3DSceneEntity(SCENEENTITY_SPEEDTREELEAF, m_lpLeafsData->m_ListLeaf[i]));
			
            pModelEditor->AddRenderEntity(KG3DSceneEntity(SCENEENTITY_SPEEDTREELEAF, m_lpLeafsData->m_ListLeaf[i]));
			//pModelEditor->AddSelectableEntity(SCENEENTITY_SPEEDTREELEAF, m_lpLeafsData->m_ListLeaf[i]);
		}
		pSpdTreeEditor->SetSpeedTree(this);
	}
	else
	{
		pSpdTreeEditor->UnSelectAll();
		pSpdTreeEditor->SetSpeedTree(NULL);
		
		for (size_t i = 0; i < m_lpLeafsData->m_ListLeaf.size(); i++)
		{
			//pModelEditor->RemoveSelectedEntity(SCENEENTITY_SPEEDTREELEAF, m_lpLeafsData->m_ListLeaf[i]);
			//pModelEditor->RemoveSelectableEntity(KG3DSceneEntity(SCENEENTITY_SPEEDTREELEAF, m_lpLeafsData->m_ListLeaf[i]));
			pModelEditor->RemoveRenderEntity(KG3DSceneEntity(SCENEENTITY_SPEEDTREELEAF, m_lpLeafsData->m_ListLeaf[i]));
		}

	}
Exit1:
	hResult = S_OK;
Exit0:
	return hResult;
}


HRESULT KG3DModelSpeedTree::ChangeLeafTexture(LPCTSTR strFileName)
{
	HRESULT hr = E_FAIL;
	KG3DTexture* pNewTexture = NULL;
	KG_PROCESS_ERROR(m_lpMaterial);
	KG_PROCESS_ERROR(m_lpMaterial->GetNumMaterial());

	hr = g_cTextureTable.LoadResourceFromFile(strFileName, 0, TEXOP_RELOAD, (IKG3DResourceBase**)&pNewTexture);
	KG_COM_PROCESS_ERROR(hr);

	hr = m_lpMaterial->SetSubsetTexture(0, 0, pNewTexture);
	KG_COM_PROCESS_ERROR(hr);

	hr = S_OK;
Exit0:
    if (FAILED(hr))
    {
        SAFE_RELEASE(pNewTexture);
    }
	return hr;
}

HRESULT KG3DModelSpeedTree::Init(IEKG3DModel* piTrunkModel)
{
	HRESULT hr = E_FAIL;
	TCHAR strDrivers[MAX_PATH];
	TCHAR strPath[MAX_PATH];
	TCHAR strFileName[MAX_PATH];
	TCHAR strPathName[MAX_PATH];
    KG3DModel *pTrunkModel = NULL;
	BOOL bUsed = FALSE;
	SAFE_RELEASE(m_lpModelTree);
		
	KG_PROCESS_ERROR(piTrunkModel);
	

    pTrunkModel = dynamic_cast<KG3DModel *>(piTrunkModel);
    KG_PROCESS_ERROR(pTrunkModel->GetType() == MESHTYPE_DEFAULT);

	_splitpath_s(pTrunkModel->m_scName.c_str(), 
		strDrivers,
		MAX_PATH,
		strPath,
		MAX_PATH,
		strFileName,
		MAX_PATH,
		NULL,
		0);

	sprintf_s(strPathName, 
		MAX_PATH,
		"%s%s%s.mesh",
		strDrivers,
		strPath,
		strFileName);

	hr = g_cModelTable.LoadResourceFromFile(strPathName,
		0, 
		MLO_MESH_SPEEDTREE, 
		(IKG3DResourceBase**)&m_lpModelTree);
	KG_COM_PROCESS_ERROR(hr);

	m_scName = strFileName;
	sprintf_s(strPathName, 
		MAX_PATH, 
		"%s%s%s_ST.mesh.STree",
		strDrivers,
		strPath,
		strFileName);

	//m_scFullName = strPathName;

	SAFE_RELEASE(m_lpMesh);
    m_lpMesh = m_lpModelTree->GetMesh();
	m_lpMesh->AddRef();
	m_szTreeMeshName = m_lpMesh->m_scName;
	m_szTreeMtlName = m_lpModelTree->m_lpMaterial->m_scName;
	
    SAFE_RELEASE(m_lpMaterial);
	hr = g_cMaterialTable.Get1NewResource(&m_lpMaterial, 0, 0);
	KG_COM_PROCESS_ERROR(hr);

	hr = m_lpMaterial->LoadFromFile(s_strDefaultLeafMaterial, 
		NULL, 
		0);
	KG_COM_PROCESS_ERROR(hr);

	//hr = m_lpMaterial->CreateSubset(1);
	//KG_COM_PROCESS_ERROR(hr);

	m_uFileHashID = g_FileNameHash(strPathName);
	SAFE_RELEASE(m_lpLeafsData);

	m_lpLeafsData = g_cSpeedTreeLeafsTable.RequestLeafsData(bUsed,m_uFileHashID);
	ComputeBBox();

	return S_OK;
Exit0:
	SAFE_RELEASE(m_lpMaterial);
	SAFE_RELEASE(m_lpModelTree);
	return E_FAIL;
}


//void KG3DModelSpeedTree::SetLeafScaling(const D3DXVECTOR3* pValue)
//{
//	float fScale = pValue->x / m_fLastLeafScale;
//	m_fLeafScale *= fScale;
//	/*for (size_t i = 0; i < m_lpLeafsData->m_ListLeaf.size(); i++)
//	{
//		m_lpLeafsData->m_ListLeaf[i]->Size *= fScale;
//	}
//	m_bNeedUpdateVB = TRUE;*/
//	m_fLastLeafScale = pValue->x;
//}

void KG3DModelSpeedTree::SetLeafScaling(const D3DXVECTOR3* pValue)
{
	m_fLeafScale = pValue->x;
}
////////////////////////////////////////////////////////////////////////////////
//IE interface
HRESULT Leaf::SetTextureUV(INT nTextureUV)
{
    HRESULT hResult = E_FAIL;
    
    TextureUV = nTextureUV;
    SetDirtyFlag();    
    hResult = S_OK;
//Exit0:
    return hResult;
}

HRESULT Leaf::SetSize(FLOAT fSize)
{
    HRESULT hResult = E_FAIL;

    Size = fSize;
    SetDirtyFlag();
    hResult = S_OK;
//Exit0:
    return hResult;
}
HRESULT Leaf::SetWave(FLOAT fWave)
{
    HRESULT hResult = E_FAIL;

    Wave = fWave;
    SetDirtyFlag();
    hResult = S_OK;
//Exit0:
    return hResult;
}
HRESULT Leaf::SetAmbient(D3DCOLORVALUE *pColor) 
{
    Ambient = *pColor;
    SetDirtyFlag();
    return S_OK;
}

HRESULT Leaf::SetDiffuse(D3DCOLORVALUE *pColor) 
{
    Diffuse = *pColor;
    SetDirtyFlag();
    return S_OK;
}

HRESULT Leaf::GetBoneID(INT *pnBoneID)
{
    HRESULT hResult = E_FAIL;

    KGLOG_PROCESS_ERROR(pnBoneID);

    *pnBoneID = BoneID;
    hResult = S_OK;
Exit0:
    return hResult;

}
HRESULT Leaf::GetSelectState(BOOL *pbSelected)
{
    HRESULT hResult = E_FAIL;

    KGLOG_PROCESS_ERROR(pbSelected);

    *pbSelected = bSelected;

    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT KG3DModelSpeedTree::GetLeafVectorSize(DWORD *pdwVectorSzie)
{
    HRESULT hResult = E_FAIL;

    KGLOG_PROCESS_ERROR(pdwVectorSzie);

    *pdwVectorSzie = (DWORD)m_lpLeafsData->m_ListLeaf.size();

    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT KG3DModelSpeedTree::GetLeafVectorItem(DWORD dwIndex, IELeaf **ppiLeaf)
{
    HRESULT hResult = E_FAIL;
    DWORD dwSize = 0;
         
    KGLOG_PROCESS_ERROR(ppiLeaf);

    dwSize = (DWORD)m_lpLeafsData->m_ListLeaf.size();

    KGLOG_PROCESS_ERROR(dwIndex < dwSize);

    *ppiLeaf = m_lpLeafsData->m_ListLeaf[dwIndex];

    hResult = S_OK;
Exit0:
    return hResult;
}
HRESULT KG3DModelSpeedTree::GetModelTree(IEKG3DModel **ppiModel)
{
    HRESULT hResult = E_FAIL;

    KGLOG_PROCESS_ERROR(ppiModel);

    *ppiModel = m_lpModelTree;

    hResult = S_OK;
Exit0:
    return hResult;

}
HRESULT KG3DModelSpeedTree::GetNeedUpdatePositionState(BOOL *pbNeed)
{
    HRESULT hResult = E_FAIL;

    KGLOG_PROCESS_ERROR(pbNeed);

    *pbNeed = m_bNeedUpdataPosition;

    hResult = S_OK;
Exit0:
    return hResult;

}
HRESULT KG3DModelSpeedTree::SetNeedUpdatePositionState(BOOL bNeed)
{
    HRESULT hResult = E_FAIL;

    m_bNeedUpdataPosition = bNeed;

    hResult = S_OK;
//Exit0:
    return hResult;
}
HRESULT KG3DModelSpeedTree::SetNeedDrawBone(BOOL bNeed)
{
    HRESULT hResult = E_FAIL;

    m_bNeeDrawBone = bNeed;

    hResult = S_OK;
//Exit0:
    return hResult;
}
/*
HRESULT KG3DModelSpeedTree::GetFullName(LPCTSTR *ppszFullName)
{
    HRESULT hResult = E_FAIL;

    KGLOG_PROCESS_ERROR(ppszFullName);

    *ppszFullName = m_scFullName.c_str();

    hResult = S_OK;
Exit0:
    return hResult;

}*/

HRESULT KG3DModelSpeedTree::SetPitchLimit(FLOAT fPitch)
{
    m_lpLeafsData->m_fPitchLimit = fPitch;
    return S_OK;
}
HRESULT KG3DModelSpeedTree::GetPitchLimit(FLOAT *pfPitch)
{
    HRESULT hResult = E_FAIL;

    KGLOG_PROCESS_ERROR(pfPitch);

    *pfPitch = m_lpLeafsData->m_fPitchLimit;

    hResult = S_OK;
Exit0:
    return hResult;

}
HRESULT KG3DModelSpeedTree::SetRollLeft(FLOAT fRollLeft)
{
    m_lpLeafsData->m_fRollLeft = fRollLeft;
    return S_OK;
}
HRESULT KG3DModelSpeedTree::SetRollRight(FLOAT fRollRight)
{
    m_lpLeafsData->m_fRollRight = fRollRight;
    return S_OK;
}
HRESULT KG3DModelSpeedTree::GetRollRight(FLOAT *pfRollRight)
{
    HRESULT hResult = E_FAIL;

    KGLOG_PROCESS_ERROR(pfRollRight);

    *pfRollRight = m_lpLeafsData->m_fRollRight;

    hResult = S_OK;
Exit0:
    return hResult;

}
HRESULT KG3DModelSpeedTree::SetMaxRollSpeed(FLOAT fMaxRollSpeed)
{
    m_lpLeafsData->m_fMaxRollSpeed = fMaxRollSpeed;
    return S_OK;
}
HRESULT KG3DModelSpeedTree::GetMaxRollSpeed(FLOAT *pfMaxRollSpeed)
{
    HRESULT hResult = E_FAIL;

    KGLOG_PROCESS_ERROR(pfMaxRollSpeed);

    *pfMaxRollSpeed = m_lpLeafsData->m_fMaxRollSpeed;

    hResult = S_OK;
Exit0:
    return hResult;

}
HRESULT KG3DModelSpeedTree::SetMaxPitchSpeed(FLOAT fMaxPitchSpeed)
{
    m_lpLeafsData->m_fMaxPitchSpeed = fMaxPitchSpeed;
    return S_OK;
}
HRESULT KG3DModelSpeedTree::SetRollAccelerate(FLOAT fRollAccelerate)
{
    m_lpLeafsData->m_fRollAccelerate = fRollAccelerate;
    return S_OK;
}
HRESULT KG3DModelSpeedTree::GetRollAccelerate(FLOAT *pfRollAccelerate)
{
    HRESULT hResult = E_FAIL;
    
    KGLOG_PROCESS_ERROR(pfRollAccelerate);

    *pfRollAccelerate = m_lpLeafsData->m_fRollAccelerate;

    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT KG3DModelSpeedTree::SetPitchAccelerate(FLOAT fPitchAccelerate)
{
    m_lpLeafsData->m_fPitchAccelerate = fPitchAccelerate;
    return S_OK;
}

DWORD KG3DModelSpeedTree::GetVertexNum()
{
    return KG3DModel::GetVertexNum() + m_lpLeafsData->GetLeafCount() * 6;
}

DWORD KG3DModelSpeedTree::GetFaceNum()
{
    return KG3DModel::GetFaceNum() + m_lpLeafsData->GetLeafCount() * 2;
}


HRESULT KG3DModelSpeedTree::GetObjProperty(BYTE **ppbObj)
{
    HRESULT hResult = E_FAIL;
	HRESULT hRetCode = E_FAIL;
	IEKG3DMesh *pMesh = NULL;

	KG_PROCESS_ERROR(m_lpModelTree);
	KG_PROCESS_ERROR(ppbObj);

	hRetCode = m_lpModelTree->GetIEKG3DMesh(&pMesh);
	KG_COM_PROCESS_ERROR(hRetCode);

	hRetCode = pMesh->GetObjProperty(ppbObj);
	KG_COM_PROCESS_ERROR(hRetCode);

    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT KG3DModelSpeedTree::SetAlpha(float fAlpha, BOOL bUseSpecail)
{
	if(m_lpModelTree)
	{
		m_lpModelTree->SetAlpha(fAlpha,bUseSpecail);
	}
	return KG3DModel::SetAlpha(fAlpha,bUseSpecail);
}

HRESULT KG3DModelSpeedTree::InitStaticResource()
{
	HRESULT hResult = E_FAIL;
	HRESULT hRetCode = E_FAIL;

    D3DVERTEXELEMENT9 VertexElement[] = {
        { 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_POSITION,  0 },
        { 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_NORMAL,    0 },
        { 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD,  0 },
        { 0, 32, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD,  1 },
        D3DDECL_END()
    };

	SAFE_RELEASE(m_lpVertexDeclaration);
	SAFE_RELEASE(m_pBillBoardBuf);

    hResult = g_pd3dDevice->CreateVertexDeclaration(VertexElement, &m_lpVertexDeclaration);
    KG_COM_PROCESS_ERROR(hResult);

    hResult = g_pd3dDevice->CreateVertexBuffer(BILLBOARD_BUF_NUM * 6 * sizeof(VFormat::FACES_DIFFUSE_TEXTURE1),
        D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, 
        D3DFVF_Faces_Diffuse_Texture1,
        D3DPOOL_DEFAULT, 
        &m_pBillBoardBuf, 
        NULL);
    KG_COM_PROCESS_ERROR(hResult);

    hRetCode = S_OK;

Exit0:
	return hRetCode;
}

HRESULT KG3DModelSpeedTree::UnInitStaticResource()
{
	SAFE_RELEASE(m_lpVertexDeclaration);
    SAFE_RELEASE(m_pBillBoardBuf);

    for (map<_TreeTexInfo, _TexArray>::iterator i = m_mapBillBoardTexs.begin(); 
        i != m_mapBillBoardTexs.end(); ++i)
    {
        SAFE_RELEASE(i->second.TexArray[TREE_FRONT]);
        SAFE_RELEASE(i->second.TexArray[TREE_LEFT]);
        SAFE_RELEASE(i->second.TexArray[TREE_RIGHT]);
        SAFE_RELEASE(i->second.TexArray[TREE_BACK]);
        SAFE_RELEASE(i->second.TexArray[TREE_UP]);
    }

	return S_OK;
}

HRESULT KG3DModelSpeedTree::OnLostDeviceStatic()
{
    HRESULT hr = S_OK;

    SAFE_RELEASE(m_pBillBoardBuf);

    for (map<_TreeTexInfo, _TexArray>::iterator i = m_mapBillBoardTexs.begin(); 
        i != m_mapBillBoardTexs.end(); ++i)
    {
        SAFE_RELEASE(i->second.TexArray[TREE_FRONT]);
        SAFE_RELEASE(i->second.TexArray[TREE_LEFT]);
        SAFE_RELEASE(i->second.TexArray[TREE_RIGHT]);
        SAFE_RELEASE(i->second.TexArray[TREE_BACK]);
        SAFE_RELEASE(i->second.TexArray[TREE_UP]);
    }

//Exit0:
    return hr;
}

HRESULT KG3DModelSpeedTree::OnResetDeviceStatic()
{
    HRESULT hr = S_OK;

    hr = g_pd3dDevice->CreateVertexBuffer( BILLBOARD_BUF_NUM * 6 * sizeof(VFormat::FACES_DIFFUSE_TEXTURE1),
        D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFVF_Faces_Diffuse_Texture1, D3DPOOL_DEFAULT, &m_pBillBoardBuf, NULL);
    KGLOG_COM_PROCESS_ERROR(hr);

Exit0:
    if (FAILED(hr))
    {
        OnLostDeviceStatic();
    }
    return hr;
}


HRESULT KG3DSpeedTreeLeafsTable::FreeLeafsData(KG3DSpeedTreeLeafsData* pData)
{
	HRESULT hr = E_FAIL;
	map<UINT,KG3DSpeedTreeLeafsData*>::iterator i;

	m_Lock.Lock();

	KG_PROCESS_ERROR(pData);

	i = s_mapLeafsData.find(pData->m_uFileHashID);

	if(i != s_mapLeafsData.end())
	{
		s_mapLeafsData.erase(i);
        SAFE_DELETE(pData);
		hr = S_OK;
	}
Exit0:
	m_Lock.Unlock();

	return hr;
}

void KG3DModelSpeedTree::SetParentObjMatrix(D3DXMATRIX* pMatrix)
{
	if(m_lpModelTree)
	{
		m_lpModelTree->SetParentObjMatrix(pMatrix);
	}
	return KG3DModel::SetParentObjMatrix(pMatrix);
}

HRESULT KG3DModelSpeedTree::FrameMove()
{
	HRESULT hr = E_FAIL;
	DWORD dwStartTime = timeGetTime();

	if(m_lpModelTree)
	{
		m_lpModelTree->FrameMove();

		if(m_lpModelTree->IsLoaded())
		{
			m_lpModelTree->SetParentObjMatrix(m_pParentMatrix);

			if(!m_lpMesh)
			{
				m_lpMesh = m_lpModelTree->GetMesh();
				if(m_lpMesh)
					m_lpMesh->AddRef();
			}
		}
	}


	if(m_lpLeafsData && m_lpLeafsData->m_bLoaded)
	{
		if(m_lpLeafsData->m_bNeedRecomputePostionByBone)
		{
			TryUpdataLeafVertexBuffer();
		}
		else if(!m_lpLeafsData->m_lpLeafVertexBuffer)
		{
			hr = CreateVerticesBuffer();
		}
	}

	hr = KG3DModel::FrameMove();

	DWORD loadcost = timeGetTime() - dwStartTime;

    if(g_cEngineOption.bEnableTimeOptimizeLog && loadcost > 30)
	{
		KGLogPrintf(KGLOG_INFO, "%d KG3DModelSpeedTree::FrameMove %s.", loadcost,m_scName.c_str());
	}

	return hr;
}

BOOL KG3DModelSpeedTree::IsNeedFrameMove()
{
	return TRUE;

	if(!m_lpLeafsData->m_lpLeafVertexBuffer)
		return TRUE;
	else
		return !m_bMeshPostLoadProcessed;
}

KG3DSpeedTreeLeafsTable::KG3DSpeedTreeLeafsTable()
{
}

KG3DSpeedTreeLeafsTable::~KG3DSpeedTreeLeafsTable()
{
	map<UINT,KG3DSpeedTreeLeafsData*>::iterator i = s_mapLeafsData.begin();
	map<UINT,KG3DSpeedTreeLeafsData*>::iterator iend = s_mapLeafsData.end();
	while (i != iend)
	{
		KG3DSpeedTreeLeafsData* pData = i->second;
		
		SAFE_DELETE(pData);

		i++;
	}
	s_mapLeafsData.clear();
}

KG3DSpeedTreeLeafsData* KG3DSpeedTreeLeafsTable::RequestLeafsData(BOOL& bReUsed,UINT uFileHashID)
{
	m_Lock.Lock();

	KG3DSpeedTreeLeafsData* pData = NULL;

	map<UINT,KG3DSpeedTreeLeafsData*>::iterator i = s_mapLeafsData.find(uFileHashID);
	if (i == s_mapLeafsData.end())
	{
		pData = new KG3DSpeedTreeLeafsData(uFileHashID);
		KGLOG_PROCESS_ERROR(pData);
		s_mapLeafsData[uFileHashID] = pData;
		pData->AddRef();
		bReUsed = FALSE;
	}
	else
	{
		pData = i->second;
		pData->AddRef();
		bReUsed = TRUE;
	}

Exit0:
	m_Lock.Unlock();

	return pData;
}

HRESULT KG3DModelSpeedTree::LoadFromFileMultiThread()
{
	_LoadFromFile(m_scName.c_str(),0,m_uOption);

	return S_OK;
}

HRESULT KG3DModelSpeedTree::LoadPostProcess()
{
    if (m_lpModelTree)
    {
        m_lpModelTree->LoadPostProcess();
    }

    return S_OK;
}

BOOL KG3DModelSpeedTree::IsLoaded()
{
    BOOL bLoaded = FALSE;

    if (!m_bInMultiThreadLoadList)
    {
        bLoaded =  m_bMeshPostLoadProcessed && m_lpModelTree->IsLoaded();
    }

	return bLoaded;
}

void KG3DModelSpeedTree::SetTranslation(D3DXVECTOR3 *pValue)
{
	if (m_lpModelTree)
	{
		m_lpModelTree->SetTranslation(pValue);
	}
	KG3DModel::SetTranslation(pValue);
}

void KG3DModelSpeedTree::SetRotation(D3DXQUATERNION *pValue)
{
	if (m_lpModelTree)
	{
		m_lpModelTree->SetRotation(pValue);
	}
	KG3DModel::SetRotation(pValue);
}

void KG3DModelSpeedTree::SetScaling(D3DXVECTOR3 *pValue)
{
	if (m_lpModelTree)
	{
		m_lpModelTree->SetScaling(pValue);
	}
	SetLeafScaling(pValue);
	KG3DModel::SetScaling(pValue);
}


