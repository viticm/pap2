#include "StdAfx.h"
#include "KG3DMesh.h"
#include "KG3DRenderState.h"
#include "KG3DBip.h"
#include "KG3DGraphicsTool.h"
#include "KG3DGraphiceEngine.h"
#include "KG3DEngineManager.h"
#include "KG3DTextureTable.h"
#include "MathTool.h"
#include "KG3DScene.h"
#include "KG3DSFXProjectionTextureMgr.h"
#include "KG3DMeshTable.h"
#include "KG3DModelST.h"
#include "KG3DTerrainBlock.h"
#include "kg3dselector.h"
#include "KG3DFlexibleBody.h"
#include "KG3DIntersectionBase.h"
#include "..\KG3DMultiThreadLoader.h"
#include "KG3DSceneSceneEditor.h"
#include "KG3DSceneOutDoorSpaceMgr.h"
#include "kg3dbsp.h"
#include "KG3DMeshLodGenerater.h"


#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#pragma warning(disable : 4731)

extern LPDIRECT3DDEVICE9  g_pd3dDevice;
extern DWORD              g_dwRenderCount;
extern float              g_fBoneWeight;

extern int g_nNumTextureDynamicLoadedFrame;
extern int g_nNumAllowTextureDynamicLoadFrame;//每桢允许操作(加载，更新)的贴图上限

extern int g_nLoadCount;
extern int g_nLoadTime ;

//////////////////////////////////////////////////////////////////////////
extern BOOL g_bOpenSelectTexture;
//////////////////////////////////////////////////////////////////////////
extern HRESULT g_SetMaterialDetailInfo(LPD3DXEFFECT pEffect,KG3DModel* pModel);
//extern HRESULT g_SetMaterialDetailInfoFast(LPD3DXEFFECT pEffect,KG3DModel* pModel, KG3D_MaterialDetailInfo_ParamHandle *pParamHandle);	//<DetailInfoParamMove>
extern D3DXMATRIX g_stMatIdentity;

BOOL g_bForceGenerateBspForMesh = FALSE;

enum { KG3DMESH_READ_BLOCK_PARAMETER = 8 };

struct KG3DMeshStaticData
{
    D3DXVECTOR3 vPositionBuffer[MAXVERTEX];
    D3DXVECTOR3 vNormalBuffer[MAXVERTEX];
    D3DCOLOR    DiffuseBuffer[MAXVERTEX];
    D3DXVECTOR3 vTextureUVW1Buffer[MAXVERTEX];
    D3DXVECTOR3 vTextureUVW2Buffer[MAXVERTEX];
    D3DXVECTOR3 vTextureUVW3Buffer[MAXVERTEX];
    DWORD       dwFaceIndicesBuffer[MAXVERTEX];
    DWORD       dwSubsetIndices[MAXVERTEX];

    KG3DMesh::ReadBlockParameter ReadBlockParameter[KG3DMESH_READ_BLOCK_PARAMETER]; //Do not use multithread loading

    ID3DXMesh* p3DXMeshDiscard[KG3DMesh::SMBT_COUNT];//正在多线程加载的mesh由于修改加载选项为直接加载所导致的失效mesh

	KG3DMeshStaticData()
	{
		ReadBlockParameter[0] = KG3DMesh::ReadBlockParameter(vPositionBuffer, 0, sizeof(D3DXVECTOR3), D3DFVF_XYZ, 0); //Position
		ReadBlockParameter[1] = KG3DMesh::ReadBlockParameter(vNormalBuffer, 0, sizeof(D3DXVECTOR3), D3DFVF_NORMAL, 0); //Normal
		ReadBlockParameter[2] = KG3DMesh::ReadBlockParameter(DiffuseBuffer, 0, sizeof(D3DXVECTOR3), D3DFVF_DIFFUSE, 0); //Diffuse
		ReadBlockParameter[3] = KG3DMesh::ReadBlockParameter(vTextureUVW1Buffer, 0, sizeof(D3DXVECTOR3), D3DFVF_TEX1, 0); //Texture Cordinate0
		ReadBlockParameter[4] = KG3DMesh::ReadBlockParameter(vTextureUVW2Buffer, 0, sizeof(D3DXVECTOR3), D3DFVF_TEX2, D3DFVF_TEX1); //Texture Cordinate1
		ReadBlockParameter[5] = KG3DMesh::ReadBlockParameter(vTextureUVW3Buffer, 0, sizeof(D3DXVECTOR3), D3DFVF_TEX3, D3DFVF_TEX1 | D3DFVF_TEX2); //Texture Cordinate2
		ReadBlockParameter[6] = KG3DMesh::ReadBlockParameter(dwFaceIndicesBuffer, 1, sizeof(DWORD) * 3, 0, 0); //Face Indices
		ReadBlockParameter[7] = KG3DMesh::ReadBlockParameter(dwSubsetIndices, 1, sizeof(DWORD), 0, 0); //Subset Indices

		memset(p3DXMeshDiscard, 0, sizeof(p3DXMeshDiscard));
	}

	HRESULT UnInit()
	{
		for (DWORD i = 0; i < KG3DMesh::SMBT_COUNT; i++)
		{
			SAFE_RELEASE(p3DXMeshDiscard[i]);
		}
		return S_OK;
	}
};
TKG3DResourcePool<KG3DMeshStaticData>g_cMeshStaticDataPool;

static LPCSTR s_strSceneRoot = "scene root";

KG3DMaterial::KG3DMaterialSubset* g_pSubMatLast = NULL;

struct BoneSocketData
{
	KG3DMesh::_BoneInfo g_BoneInfo[KG3DMesh::s_dwMaxBone];
	KG3DMesh::_Socket   g_Sockets[KG3DMesh::s_dwMaxSocket];

	HRESULT UnInit()
	{
		return S_OK;
	}
};
TKG3DResourcePool<BoneSocketData>g_cMeshBoneSocketPool;

const TCHAR* g_szDefMeshIni = "\\data\\public\\mesh default.ini";

const static DWORD s_dwLoadOptionMask = (0xffffffff & MLO_MESH_SPEEDTREE);

extern DWORD g_dwMainThreadID;//渲染线程ID

static const D3DVERTEXELEMENT9 g_VBDecl_Skin[] = 
{
	{0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
	{0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 1},
	{0, 24, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT,0},
	{0, 36, D3DDECLTYPE_UBYTE4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDINDICES, 0},
	{0, 40, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0},
	{0, 52, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
	{0, 64, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1},
	{0, 76, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT,  0},
	{0, 88, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0},
	{0, 100,D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 2},
	{0, 112,D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 3},
	{0, 124,D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   1},
	{0, 136,D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   2},
	{0, 148,D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   3},
	D3DDECL_END()
};

static const D3DVERTEXELEMENT9 g_VBDecl_Skin2Link[] = 
{
	{0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
	{0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 1},
	{0, 24, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT,0},
	{0, 36, D3DDECLTYPE_UBYTE4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDINDICES, 0},
	{0, 40, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0},
	{0, 52, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
	{0, 60,D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   1},
	D3DDECL_END(),
};

static const D3DVERTEXELEMENT9 g_VBDecl_Geometry[] =
{
	{0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
	{0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT,0},
	{0, 24, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0},
	{0, 36, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
	{0, 48, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1},
	{0, 60, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT,  0},
	{0, 72, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0},

	D3DDECL_END()
};


static const D3DVERTEXELEMENT9 g_VBDecl_Geometry_FixPipe[] =
{
	{0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
	{0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT,0},
	{0, 24, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0},
	{0, 36, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},

	D3DDECL_END()
};

static const D3DVERTEXELEMENT9 g_Decl_NormalMap[] =
{
	{0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0}, 
	{0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0}, 
	{0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0}, 
	{0, 32, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1}, 
	{0, 40, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT,  0}, 
	{0, 52, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0}, 
	D3DDECL_END()
};

KG3DMesh::VertexFromatOffsetItem s_VertexFormat[] = {
	{ D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1, 3,   
	0,  1,  3, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
	12, 12, 12, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
	0, 12, 24, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
	12, 12,  8, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff },//PosNorTex1

	{ D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX2, 4,
	0,  1,  3, 4, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
	12, 12, 12, 12, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
	0, 12, 24, 32, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
	12, 12,  8,  8, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff },//PosNorTex2

	{ D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX3, 5,
	0,  1,  3, 4, 5, 0xffffffff, 0xffffffff, 0xffffffff,
	12, 12, 12, 12, 12, 0xffffffff, 0xffffffff, 0xffffffff,  
	0, 12, 24, 32, 40, 0xffffffff, 0xffffffff, 0xffffffff,
	12, 12,  8,  8,  8, 0xffffffff, 0xffffffff, 0xffffffff },//PosNorTex3

	{ D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1, 3,
	0,  2,  3, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
	12,  4, 12, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
	0, 12, 16, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
	12,  4,  8, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff },//PosDiffuseTex1

	{ D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX2, 4,
	0,  2,  3,  4, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
	12,  4, 12, 12, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
	0, 12, 16, 24, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
	12,  4,  8,  8, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff },//PosDiffuseTex2

	{ D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX3, 5, 
	0,  2,  3,  4,  5, 0xffffffff, 0xffffffff, 0xffffffff,
	12,  4, 12, 12, 12, 0xffffffff, 0xffffffff, 0xffffffff,
	0, 12, 16, 24, 32, 0xffffffff, 0xffffffff, 0xffffffff,
	12,  4,  8,  8,  8, 0xffffffff, 0xffffffff, 0xffffffff },//PosDiffuseTex2

	{ D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_NORMAL | D3DFVF_TEX1, 4,
	0,  1,  2,  3, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
	0, 12,  4, 12, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
	0, 12, 16, 20, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
	0, 12,  4,  8, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff },//PosNormalDiffuseTex1

	{ D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_NORMAL | D3DFVF_TEX2, 5,
	0,  1,  2,  3,  4, 0xffffffff, 0xffffffff, 0xffffffff,
	0, 12,  4, 12, 12, 0xffffffff, 0xffffffff, 0xffffffff,
	0, 12, 16, 24, 32, 0xffffffff, 0xffffffff, 0xffffffff,
	0, 12,  4,  8,  8, 0xffffffff, 0xffffffff, 0xffffffff },//PosNormalDiffuseTex2

	{ D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_NORMAL | D3DFVF_TEX1, 6,
	0,  1,  2,  3,  4,  5, 0xffffffff, 0xffffffff,
	0, 12,  4,  8,  8,  8, 0xffffffff, 0xffffffff,
	0, 12, 16, 28, 40, 52, 0xffffffff, 0xffffffff,
	0, 12,  4, 12, 12, 12, 0xffffffff, 0xffffffff }//PosNormalDiffuseTex3
};

KG3DMesh::ReadBlockParameter::ReadBlockParameter()
{
}

KG3DMesh::ReadBlockParameter::ReadBlockParameter(void* pBuffer, DWORD dwNumElementOffset, DWORD dwElementSize, DWORD dwFVFAdd, DWORD dwFVFRemove)
{
    this->pBuffer = pBuffer;
	this->dwNumElementOffset = dwNumElementOffset;
	this->dwElementSize = dwElementSize;
	this->dwFVFAdd = dwFVFAdd;
	this->dwFVFRemove = dwFVFRemove;
}

HRESULT Init3DMeshStaticResource()
{
    HRESULT hrResult = E_FAIL;
	//这里要对g_cMeshStaticDataPool的m_dwMaxPool赋值（而且不能太大），默认值是400，导至内存空间不够分配而宕机。
	g_cMeshStaticDataPool.m_dwMaxPool = 3;
	g_cMeshBoneSocketPool.m_dwMaxPool = 3;
    hrResult = S_OK;
//Exit0:

    return hrResult;
}

HRESULT UnInit3DMeshStaticResource()
{
	g_cMeshStaticDataPool.UnInit();
	g_cMeshBoneSocketPool.UnInit();
    return S_OK;
}

KG3DMesh::KG3DMesh(void)
	//:m_pBillBoardCloundVertexBuffer(NULL)
{
	m_uOption = 0;
	m_dwID = 0;
	m_dwType = MESHTYPE_DEFAULT;
	m_ulRefCount = 0;
	m_nNumCurFacesLOD = 0;
	//m_nBaseBoneID initialization not needed;
	//m_pBoneMatricesForRender = NULL;
	//m_scName = NULL;

	m_pMeshEnhanced = NULL;
	m_pMeshBeforeProgress = NULL;
	m_pVerticesLodWeight = NULL;
	//////////////////////////////////////////////////////////////////////////
	//基本属性
	m_dwOption = 0;//选项
	m_dwOptionEx = 0;//保存MESH数据的扩展选项
	m_dwNumSubset = 0;//模型上子物体的数目
	//////////////////////////////////////////////////////////////////////////
	//骨骼和插槽
	m_dwNumBone = 0;//骨骼数目


	m_dwNumSocket = 0;//插槽数目
	//m_pBoneMatricesForRender = NULL;//骨骼矩阵数组

	m_dwMaxFaceInfl = 0;        //对于蒙皮模型，每个Attribute涉及到骨骼的最大值
	m_dwNumAttributeGroups = 0; //蒙皮模型切分后的自部分数目
	m_pBoneCombinationTable = NULL;//骨骼蒙皮信息

	m_pSkinInfo    = NULL;//骨骼蒙皮信息
	m_pVertexRemap = NULL;//顶点重新定位表，用于顶点动画
	//////////////////////////////////////////////////////////////////////////
	//网格
	for (DWORD i = 0; i < SMBT_COUNT; i++)
	{
		m_ppMeshes[i] = NULL;
	}
	m_pMeshProgress  = NULL; //优化显示后网格，可能是静止的也可能是骨骼蒙皮的；
    m_pModel = NULL;

    m_dwVertexStride = 0;
	m_lpNormal = NULL;
	m_lpBinormal = NULL;
	m_lpTangent = NULL;
    m_dwCurSel  = (DWORD)-1;

    //m_pBillBoardCloundVertexBuffer = NULL;
    //m_dwBillBoardCount = 0;
	m_lpCpuProcessData = NULL;

	//m_lpMeshStaticData = NULL;
	m_lpMeshBoneSocketData = NULL;

	m_bInMultiThreadLoadList = FALSE;

	m_lpBsp = NULL;
} 

KG3DMesh::~KG3DMesh(void)
{
	UnInit();

	ASSERT(m_dwNumBone == 0);
    ASSERT(m_dwNumSocket == 0);
	ASSERT(m_dwVertexStride == 0);

	ASSERT(m_pBoneCombinationTable == NULL);
	
	for (DWORD i = 0; i < SMBT_COUNT; i++)
	{
		ASSERT(m_ppMeshes[i] == NULL);
	}

	ASSERT(m_pMeshProgress == NULL);
	ASSERT(m_pVertexRemap == NULL);
	ASSERT(m_pMeshEnhanced == NULL);
	ASSERT(m_pMeshBeforeProgress == NULL);

	ASSERT(m_pVerticesLodWeight == NULL);
	ASSERT(m_nBaseBoneID.empty());
	ASSERT(m_pSkinInfo == NULL);
	ASSERT(m_lpNormal == NULL);
	ASSERT(m_lpBinormal == NULL);
	ASSERT(m_lpTangent == NULL);

	ASSERT(m_lpCpuProcessData == NULL);

}


HRESULT KG3DMesh::UnInit()
{
	m_dwNumBone = 0;
    //Delete Socket information
	m_dwNumSocket = 0;
	m_dwVertexStride = 0;

	SAFE_RELEASE(m_pBoneCombinationTable);
	
	for (DWORD i = 0; i < SMBT_COUNT; i++)
	{
		SAFE_RELEASE(m_ppMeshes[i]);
	}
	SAFE_RELEASE(m_pMeshProgress);
	SAFE_RELEASE(m_pVertexRemap);
	SAFE_RELEASE(m_pMeshEnhanced);
	SAFE_RELEASE(m_pMeshBeforeProgress);

	SAFE_DELETE_ARRAY(m_pVerticesLodWeight);
	m_nBaseBoneID.clear();
	SAFE_RELEASE(m_pSkinInfo);
	SAFE_RELEASE(m_lpNormal);
	SAFE_RELEASE(m_lpBinormal);
	SAFE_RELEASE(m_lpTangent);
    //SAFE_RELEASE(m_pBillBoardCloundVertexBuffer);

	SAFE_DELETE(m_lpCpuProcessData);

	/*if(m_lpMeshStaticData)
	{
		g_cMeshStaticDataPool.FreeResource(m_lpMeshStaticData);
		m_lpMeshStaticData = NULL;
	}*/

	if(m_lpMeshBoneSocketData)
	{
		g_cMeshBoneSocketPool.FreeResource(m_lpMeshBoneSocketData);
		m_lpMeshBoneSocketData = NULL;
	}	

	SAFE_DELETE(m_lpBsp);

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
//加载, 保存

KG3DMesh::ReadInMeshData::ReadInMeshData()
{
	pPos = NULL;
	pNormals = NULL;
	pDiffuse = NULL;
	pUV1 = NULL;
	pUV2 = NULL;
	pUV3 = NULL;
	pFaceIndices = NULL;
	pSubsetIndices = NULL;
	dwMeshFVF = 0;
	dwNumFaces = 0;
	dwNumVertices = 0;
	dwNumSubset = 0;
}

HRESULT KG3DMesh::LoadPropertyFromIni(LPCSTR cszFileName)
{
    HRESULT hrResult  = E_FAIL;
	TCHAR strDriver[MAX_PATH];
	TCHAR strPath[MAX_PATH];
	TCHAR strFile[MAX_PATH];
	TCHAR strIniPathName[MAX_PATH];
	IIniFile* pIniFile = NULL;
	
	KG_PROCESS_ERROR(cszFileName);

	_splitpath_s(cszFileName, 
		strDriver,
		MAX_PATH,
		strPath,
		MAX_PATH,
		strFile,
		MAX_PATH,
		NULL,
		0);
	
	sprintf_s(strIniPathName,
		MAX_PATH,
		"%s%s%s.ini",
		strDriver,
		strPath,
		strFile);
	
	m_strPropertyFileName = strIniPathName;

    if (g_IsFileExist(strIniPathName))
        pIniFile = g_OpenIniFile(strIniPathName, false, false);
    else
        pIniFile = g_OpenIniFile(g_szDefMeshIni, false, false);

    ASSERT(pIniFile);

    if (pIniFile)
    {
        int nColorARGB[4];

        // =============声音相关(Sound)================
        pIniFile->GetString(
            TEXT("Sounds"), TEXT("szRandGlobSound1_FilePath"),
            TEXT(""), m_objPropertys.szRandGlobSound1_FilePath,
            sizeof(m_objPropertys.szRandGlobSound1_FilePath)
        );
        pIniFile->GetInteger(
            TEXT("Sounds"), TEXT("nRandGlobSound1_Inteval"),
            0, &m_objPropertys.nRandGlobSound1_Inteval
        );
        pIniFile->GetInteger(
            TEXT("Sounds"), TEXT("nRandGlobSound1_Range"),
            0, &m_objPropertys.nRandGlobSound1_Range
        );
        pIniFile->GetInteger(
            TEXT("Sounds"), TEXT("nRandGlobSound1_AppearRate"),
            0, &m_objPropertys.nRandGlobSound1_AppearRate
        );

        pIniFile->GetString(
            TEXT("Sounds"), TEXT("szRandGlobSound2_FilePath"),
            TEXT(""), m_objPropertys.szRandGlobSound2_FilePath,
            sizeof(m_objPropertys.szRandGlobSound2_FilePath)
        );
        pIniFile->GetInteger(
            TEXT("Sounds"), TEXT("nRandGlobSound2_Inteval"),
            0, &m_objPropertys.nRandGlobSound2_Inteval
        );
        pIniFile->GetInteger(
            TEXT("Sounds"), TEXT("nRandGlobSound2_Range"),
            0, &m_objPropertys.nRandGlobSound2_Range
        );
        pIniFile->GetInteger(
            TEXT("Sounds"), TEXT("nRandGlobSound2_AppearRate"),
            0, &m_objPropertys.nRandGlobSound2_AppearRate
        );


        pIniFile->GetString(
            TEXT("Sounds"), TEXT("szRandTimebSound1_FilePath"),
            TEXT(""), m_objPropertys.szRandTimebSound1_FilePath,
            sizeof(m_objPropertys.szRandTimebSound1_FilePath)
        );
        pIniFile->GetInteger(
            TEXT("Sounds"), TEXT("nRandTimeSound1_Inteval"),
            0, &m_objPropertys.nRandTimeSound1_Inteval
        );
        pIniFile->GetInteger(
            TEXT("Sounds"), TEXT("nRandTimeSound1_Range"),
            0, &m_objPropertys.nRandTimeSound1_Range
        );
        pIniFile->GetInteger(
            TEXT("Sounds"), TEXT("nRandTimeSound1_AppearRate"),
            0, &m_objPropertys.nRandTimeSound1_AppearRate
        );
        pIniFile->GetInteger(
            TEXT("Sounds"), TEXT("nAffectTime1"),
            0, &m_objPropertys.nAffectTime1
        );

        pIniFile->GetString(
            TEXT("Sounds"), TEXT("szRandTimebSound2_FilePath"),
            TEXT(""), m_objPropertys.szRandTimebSound2_FilePath,
            sizeof(m_objPropertys.szRandTimebSound2_FilePath)
        );
        pIniFile->GetInteger(
            TEXT("Sounds"), TEXT("nRandTimeSound2_Inteval"),
            0, &m_objPropertys.nRandTimeSound2_Inteval
        );
        pIniFile->GetInteger(
            TEXT("Sounds"), TEXT("nRandTimeSound2_Rnage"),
            0, &m_objPropertys.nRandTimeSound2_Rnage
        );
        pIniFile->GetInteger(
            TEXT("Sounds"), TEXT("nRandTimeSound2_AppearRate"),
            0, &m_objPropertys.nRandTimeSound2_AppearRate
        );
        pIniFile->GetInteger(
            TEXT("Sounds"), TEXT("nAffectTime2"),
            0, &m_objPropertys.nAffectTime2
        );

        // ==================显示相关(Display)================
        pIniFile->GetFloat(
            TEXT("Display"), TEXT("fVisibleInFog"), 
            0, &m_objPropertys.fVisibleInFog
        );
        pIniFile->GetInteger(
            TEXT("Display"), TEXT("bAutoProduceObstacle"), 
            m_objPropertys.bAutoProduceObstacle,
            &m_objPropertys.bAutoProduceObstacle
        );
        pIniFile->GetInteger(
            TEXT("Display"), TEXT("bObscatleCamera"), 
            m_objPropertys.bObscatleCamera,
            &m_objPropertys.bObscatleCamera
        );
        pIniFile->GetInteger(
            TEXT("Display"), TEXT("bAffectByTime"), 
            m_objPropertys.bAffectByTime,
            &m_objPropertys.bAffectByTime
        );
        pIniFile->GetInteger(
            TEXT("Display"), TEXT("bAutoConnect"), 
            m_objPropertys.bAutoConnect,
            &m_objPropertys.bAutoConnect
        );
        pIniFile->GetInteger(
            TEXT("Display"), TEXT("bTransparence"), 
            m_objPropertys.bTransparence,
            &m_objPropertys.bTransparence
        );
        pIniFile->GetInteger(
            TEXT("Display"), TEXT("nEyeablePRI"), 
            0, &m_objPropertys.nEyeablePRI
        );
        pIniFile->GetInteger(
            TEXT("Display"), TEXT("nObscatleType"), 
            0, &m_objPropertys.nObscatleType
        );
        pIniFile->GetInteger(
            TEXT("Display"), TEXT("nTime1"), 
            0, &m_objPropertys.nTime1
        );
        pIniFile->GetInteger(
            TEXT("Display"), TEXT("nTime2"), 
            0, &m_objPropertys.nTime2
        );

        pIniFile->GetMultiInteger( 
            TEXT("Display"), TEXT("dwGlobColor"), nColorARGB, 4 
        );
        m_objPropertys.dwGlobColor = D3DCOLOR_ARGB(
            nColorARGB[0], nColorARGB[1], nColorARGB[2], nColorARGB[3]
        );
        pIniFile->GetMultiInteger( 
            TEXT("Display"), TEXT("dwPartColor"), nColorARGB, 4 
        );
        m_objPropertys.dwPartColor = D3DCOLOR_ARGB(
            nColorARGB[0], nColorARGB[1], nColorARGB[2], nColorARGB[3]
        );

        pIniFile->GetInteger(
            TEXT("Display"), TEXT("AutoTowards"), 0, &m_objPropertys.bAutoTowards
            );

		//<MODEL_LOD>
	/*
			do
				{
					TCHAR	tcsLODMeshPath[MAX_PATH];
					_tcscpy_s(tcsLODMeshPath, TEXT("nLODMeshAttached"));
					TCHAR	tcsLODMeshSection[] = TEXT("Display");
		
					m_objPropertys.vecLODMeshName.clear();
					INT nLODMeshAttached = 0;//挂了多少LOD
					if (! pIniFile->GetInteger(tcsLODMeshSection, tcsLODMeshPath, 0, &nLODMeshAttached))
						break;
		
					if(nLODMeshAttached <= 0)
						break;
					
					
					for (INT i = 0; i < nLODMeshAttached; ++i)
					{
						if(! pIniFile->GetNextKey(tcsLODMeshSection, tcsLODMeshPath, tcsLODMeshPath))
						{
							m_objPropertys.vecLODMeshName.clear();
							break;
						}				
						TCHAR TempStr[MAX_PATH];
						if (! pIniFile->GetString(tcsLODMeshSection, tcsLODMeshPath, _T(""), TempStr, _countof(TempStr)))
						{
							m_objPropertys.vecLODMeshName.clear();
							break;
						}
						TempStr[MAX_PATH-1] = '\0';
		
						if (NULL == _tcsstr(TempStr, _T("LOD")))//应该是不可能到这里的，保存出错了
						{
							KGLogPrintf(KGLOG_WARNING, _T("Mesh对应的ini文件读写出错，可能是错误保存，LOD载入失败: %s")
								, cszFileName);
							m_objPropertys.vecLODMeshName.clear();
							break;
						}
						
						m_objPropertys.vecLODMeshName.push_back(std::tstring(TempStr));
					}
				}while(false);*/
		
		//</MODEL_LOD>
		
        // ===============文字显示相关(Text)================
        pIniFile->GetString(
            TEXT("Editer"), TEXT("szName"), TEXT(""),
            m_objPropertys.szName, sizeof(m_objPropertys.szName)
        );

        pIniFile->GetString(
            TEXT("Editer"), TEXT("szSuffix"), TEXT(""),
            m_objPropertys.szSuffix, sizeof(m_objPropertys.szSuffix)
        );
        pIniFile->GetString(
            TEXT("Editer"), TEXT("szNote"), TEXT(""),
            m_objPropertys.szNote, sizeof(m_objPropertys.szNote)
        );
        pIniFile->GetString(
            TEXT("Editer"), TEXT("szKeyWord"), TEXT(""),
            m_objPropertys.szKeyWord, sizeof(m_objPropertys.szKeyWord)
        );
        pIniFile->GetInteger(
            TEXT("Editer"), TEXT("nViewClass"),
            0, &m_objPropertys.nViewClass
        );
    }
    

    if (!m_objPropertys.szName[0])
    {  
        sprintf(
            m_objPropertys.szName, TEXT("%s"), m_scName.c_str()
        );
    }


    m_objPropertys.bInitallized = TRUE;
    hrResult = S_OK;
Exit0:
    KG_COM_RELEASE(pIniFile);
    return hrResult;
} 

HRESULT KG3DMesh::SavePropertyToIni(LPCTSTR cszFileName)
{
    HRESULT hrResult  = E_FAIL;

    if (cszFileName) 
        m_strPropertyFileName = cszFileName;
    IIniFile* pIniFile = g_OpenIniFile(m_strPropertyFileName.c_str(), false, true);
    KG_PROCESS_ERROR(pIniFile);

        // =============声音相关(Sound)================
    if (m_objPropertys.bInitallized)
    {
        int nColorARGB[4];
        pIniFile->WriteString(
            TEXT("Sounds"), TEXT("szRandGlobSound1_FilePath"),
            m_objPropertys.szRandGlobSound1_FilePath
        );
        pIniFile->WriteInteger(
            TEXT("Sounds"), TEXT("nRandGlobSound1_Inteval"),
            m_objPropertys.nRandGlobSound1_Inteval
        );
        pIniFile->WriteInteger(
            TEXT("Sounds"), TEXT("nRandGlobSound1_Range"),
            m_objPropertys.nRandGlobSound1_Range
        );
        pIniFile->WriteInteger(
            TEXT("Sounds"), TEXT("nRandGlobSound1_AppearRate"),
            m_objPropertys.nRandGlobSound1_AppearRate
        );

        pIniFile->WriteString(
            TEXT("Sounds"), TEXT("szRandGlobSound2_FilePath"),
            m_objPropertys.szRandGlobSound2_FilePath
        );
        pIniFile->WriteInteger(
            TEXT("Sounds"), TEXT("nRandGlobSound2_Inteval"),
            m_objPropertys.nRandGlobSound2_Inteval
        );
        pIniFile->WriteInteger(
            TEXT("Sounds"), TEXT("nRandGlobSound2_Range"),
            m_objPropertys.nRandGlobSound2_Range
        );
        pIniFile->WriteInteger(
            TEXT("Sounds"), TEXT("nRandGlobSound2_AppearRate"),
            m_objPropertys.nRandGlobSound2_AppearRate
        );


        pIniFile->WriteString(
            TEXT("Sounds"), TEXT("szRandTimebSound1_FilePath"),
            m_objPropertys.szRandTimebSound1_FilePath
        );
        pIniFile->WriteInteger(
            TEXT("Sounds"), TEXT("nRandTimeSound1_Inteval"),
            m_objPropertys.nRandTimeSound1_Inteval
        );
        pIniFile->WriteInteger(
            TEXT("Sounds"), TEXT("nRandTimeSound1_Range"),
            m_objPropertys.nRandTimeSound1_Range
        );
        pIniFile->WriteInteger(
            TEXT("Sounds"), TEXT("nRandTimeSound1_AppearRate"),
            m_objPropertys.nRandTimeSound1_AppearRate
        );
        pIniFile->WriteInteger(
            TEXT("Sounds"), TEXT("nAffectTime1"),
            m_objPropertys.nAffectTime1
        );

        pIniFile->WriteString(
            TEXT("Sounds"), TEXT("szRandTimebSound2_FilePath"),
            m_objPropertys.szRandTimebSound2_FilePath
        );
        pIniFile->WriteInteger(
            TEXT("Sounds"), TEXT("nRandTimeSound2_Inteval"),
            m_objPropertys.nRandTimeSound2_Inteval
        );
        pIniFile->WriteInteger(
            TEXT("Sounds"), TEXT("nRandTimeSound2_Rnage"),
            m_objPropertys.nRandTimeSound2_Rnage
        );
        pIniFile->WriteInteger(
            TEXT("Sounds"), TEXT("nRandTimeSound2_AppearRate"),
            m_objPropertys.nRandTimeSound2_AppearRate
        );
        pIniFile->WriteInteger(
            TEXT("Sounds"), TEXT("nAffectTime2"),
            m_objPropertys.nAffectTime2
        );

        // ==================显示相关(Display)================
        pIniFile->WriteFloat(
            TEXT("Display"), TEXT("fVisibleInFog"), 
            m_objPropertys.fVisibleInFog
        );
        pIniFile->WriteInteger(
            TEXT("Display"), TEXT("bAutoProduceObstacle"), 
            m_objPropertys.bAutoProduceObstacle
        );
        pIniFile->WriteInteger(
            TEXT("Display"), TEXT("bObscatleCamera"), 
            m_objPropertys.bObscatleCamera
        );
        pIniFile->WriteInteger(
            TEXT("Display"), TEXT("bAffectByTime"), 
            m_objPropertys.bAffectByTime
        );
        pIniFile->WriteInteger(
            TEXT("Display"), TEXT("bAutoConnect"), 
            m_objPropertys.bAutoConnect
        );
        pIniFile->WriteInteger(
            TEXT("Display"), TEXT("bTransparence"), 
            m_objPropertys.bTransparence
        );
        pIniFile->WriteInteger(
            TEXT("Display"), TEXT("nEyeablePRI"), 
            m_objPropertys.nEyeablePRI
        );
        pIniFile->WriteInteger(
            TEXT("Display"), TEXT("nObscatleType"), 
            m_objPropertys.nObscatleType
        );
        pIniFile->WriteInteger(
            TEXT("Display"), TEXT("nTime1"), 
            m_objPropertys.nTime1
        );
        pIniFile->WriteInteger(
            TEXT("Display"), TEXT("nTime2"), 
            m_objPropertys.nTime2
        );

        nColorARGB[0] = (m_objPropertys.dwGlobColor & 0xFF000000)>>24;
        nColorARGB[1] = (m_objPropertys.dwGlobColor & 0x00FF0000)>>16;
        nColorARGB[2] = (m_objPropertys.dwGlobColor & 0x0000FF00)>>8;
        nColorARGB[3] = (m_objPropertys.dwGlobColor & 0x000000FF);

        pIniFile->WriteMultiInteger( 
            TEXT("Display"), TEXT("dwGlobColor"), nColorARGB, 4 
        );

        nColorARGB[0] = (m_objPropertys.dwPartColor & 0xFF000000)>>24;
        nColorARGB[1] = (m_objPropertys.dwPartColor & 0x00FF0000)>>16;
        nColorARGB[2] = (m_objPropertys.dwPartColor & 0x0000FF00)>>8;
        nColorARGB[3] = (m_objPropertys.dwPartColor & 0x000000FF);

        pIniFile->WriteMultiInteger( 
            TEXT("Display"), TEXT("dwPartColor"), nColorARGB, 4 
        );
        m_objPropertys.dwPartColor = D3DCOLOR_ARGB(
            nColorARGB[0], nColorARGB[1], nColorARGB[2], nColorARGB[3]
        );

        pIniFile->WriteInteger(
            TEXT("Display"), TEXT("AutoTowards"), m_objPropertys.bAutoTowards
            );

		//<MODEL_LOD>
		/*
		{
					size_t LODMeshCount = m_objPropertys.vecLODMeshName.size();
					if(LODMeshCount <= 0 && LODMeshCount < INT_MAX)
						pIniFile->WriteInteger(TEXT("Display"), TEXT("nLODMeshAttached"), static_cast<INT>(LODMeshCount));	
					for (size_t i = 0; i < LODMeshCount; ++i)
					{
						std::tstring TempStr = TEXT("szLODMesh")+i;
						INT nRet = pIniFile->WriteString(TEXT("Display"), TempStr.c_str()
							, m_objPropertys.vecLODMeshName[i].c_str());
						if (! nRet)
						{
							pIniFile->WriteInteger(TEXT("Display"), TEXT("nLODMeshAttached"), 0);
							break;
						}
					}
				}*/
		
		//</MODEL_LOD>

        // ===============文字显示相关(Text)================
        pIniFile->WriteString(
            TEXT("Editer"), TEXT("szName"), 
            m_objPropertys.szName
        );
        pIniFile->WriteString(
            TEXT("Editer"), TEXT("szSuffix"),
            m_objPropertys.szSuffix
        );
        pIniFile->WriteString(
            TEXT("Editer"), TEXT("szNote"), 
            m_objPropertys.szNote
        );
        pIniFile->WriteString(
            TEXT("Editer"), TEXT("szKeyWord"),
            m_objPropertys.szKeyWord
        );
        pIniFile->WriteInteger(
            TEXT("Editer"), TEXT("nViewClass"),
            m_objPropertys.nViewClass
        );
    }

    pIniFile->Save(m_strPropertyFileName.c_str());

    hrResult = S_OK;
Exit0:
    KG_COM_RELEASE(pIniFile);
    return hrResult;
}

HRESULT KG3DMesh::Helper_LoadMeshVersion0(IFile *pFile,
										  unsigned uOption)
{
	HRESULT hrRetCode = E_FAIL;
	HRESULT hrResult = E_FAIL;

	hrRetCode = Helper_LoadMeshFromFile(pFile, FALSE);
	KG_COM_PROCESS_ERROR(hrRetCode);

	//创建在内存中的MESH
	hrRetCode = Helper_CreateD3DXMesh(m_MeshData);
	KGLOG_COM_PROCESS_ERROR(hrRetCode);

	//Fill in data
	hrRetCode = Helper_FillVertexData(m_MeshData);
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

	hrResult = S_OK;
Exit0:
	return hrResult;
}


HRESULT KG3DMesh::Helper_LoadFromFileMultiThreadVersion0(IFile *pFile,
														 unsigned uOption)
{
	HRESULT hrResult = E_FAIL;
	HRESULT hrRetCode  = E_FAIL;

	hrRetCode = Helper_LoadMeshFromFile(pFile, TRUE);
	KG_COM_PROCESS_ERROR(hrRetCode);

	DWORD dwThreadID = ::GetCurrentThreadId();
	if(dwThreadID == g_dwMainThreadID)
	{
		hrRetCode = Helper_FillVertexData(m_MeshData);
		KG_COM_PROCESS_ERROR(hrRetCode);

		if (m_dwOption & MESH_SKINED)
		{
			hrRetCode = Helper_CreateSkinInfo();
			KG_COM_PROCESS_ERROR(hrRetCode);

			if (!(m_dwOptionEx & MESH_SAVE_BONEBBOX))
			{
				CreateBBoxOfBone();
			}
		}
	}

	hrResult = S_OK;
Exit0:
	return hrResult;
}

HRESULT KG3DMesh::LoadFromFileMultiThread()
{
	HRESULT hrResult = E_FAIL;
	HRESULT hrRetCode  = E_FAIL;
	_MeshFileHead FileHeader;
	unsigned long uSize = 0;
	unsigned uOption = m_uOption & s_dwLoadOptionMask;
	IFile *pFile = NULL;

	DynamicState state = STATE_LOADFAILED;

	if(m_eDynamicLoadState == STATE_MULTITHREAD_PREPEAR)
	{
		/*if(!m_lpMeshStaticData)
		{
			m_lpMeshStaticData = g_cMeshStaticDataPool.RequestResource();
		}*/
		if(!m_lpMeshBoneSocketData)
		{
			m_lpMeshBoneSocketData = g_cMeshBoneSocketPool.RequestResource();
		}
		//KGLOG_PROCESS_ERROR(m_lpMeshStaticData);
		KGLOG_PROCESS_ERROR(m_lpMeshBoneSocketData);

		hrRetCode = Helper_LoadMeshInformationFromFile(m_scName.c_str(), 
			m_uOption);
		KG_COM_PROCESS_ERROR(hrRetCode);

		pFile = g_OpenFile(m_scName.c_str());
		KG_PROCESS_ERROR(pFile);

		uSize = pFile->Read(&FileHeader, sizeof(_MeshFileHead));
		KG_PROCESS_ERROR(uSize == sizeof(_MeshFileHead));

		KG_PROCESS_ERROR(FileHeader.dwFileMask == 0x4D455348);

        KGLOG_PROCESS_ERROR(FileHeader.dwVersion == 0);
        hrRetCode = Helper_LoadFromFileMultiThreadVersion0(pFile, uOption);
        KGLOG_COM_PROCESS_ERROR(hrRetCode);    

		hrRetCode = LoadPropertyFromIni(m_scName.c_str());
		KG_COM_PROCESS_ERROR(hrRetCode);
		
		/*if(m_lpMeshStaticData)
		{
			g_cMeshStaticDataPool.FreeResource(m_lpMeshStaticData);
			m_lpMeshStaticData = NULL;
		}*/
		/*if(m_lpMeshBoneSocketData)
		{
			g_cMeshBoneSocketPool.FreeResource(m_lpMeshBoneSocketData);
			m_lpMeshBoneSocketData = NULL;
		}*/
    
        state = STATE_MULTITHREADREAD_FINISH;
	}

	hrResult = S_OK;
Exit0:
	KG_COM_RELEASE(pFile);
	m_eDynamicLoadState = state;
	return hrResult;
} 

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
		BoneInfo1 &BoneInfo = m_BoneInfo[dwBoneIndex];
		hrRetCode = m_pSkinInfo->SetBoneInfluence(dwBoneIndex,
			BoneInfo.NumVerticesRef, 
			BoneInfo.VerticesIndexRef, 
			BoneInfo.VerticesWeightRef);
		KG_COM_PROCESS_ERROR(hrRetCode);

		hrRetCode = m_pSkinInfo->SetBoneName(dwBoneIndex, BoneInfo.strBoneName);
		KG_COM_PROCESS_ERROR(hrRetCode);

		D3DXMATRIX matOffset;
		D3DXMatrixInverse(&matOffset, NULL, &BoneInfo.CurMatrix);
		hrRetCode = m_pSkinInfo->SetBoneOffsetMatrix(dwBoneIndex, &matOffset);
		KG_COM_PROCESS_ERROR(hrRetCode);
	}
	hrResult = S_OK;
Exit0:
	return hrResult;
}

HRESULT KG3DMesh::MainThreadProcessLoadAfterMultiThread()
{
	HRESULT hrRetCode = E_FAIL;
	HRESULT hrResult = S_OK;

	if(g_eEngineOption.bUseMultiThreadLoad &&
		g_nNumTextureDynamicLoadedFrame <= g_nNumAllowTextureDynamicLoadFrame)
	{
		hrResult = Helper_CreateD3DXMesh(m_MeshData);
		KGLOG_COM_PROCESS_ERROR(hrResult);

		hrRetCode = Helper_FillVertexData(m_MeshData);
		KG_COM_PROCESS_ERROR(hrRetCode);

		if (m_dwOption & MESH_SKINED)
		{
			hrRetCode = Helper_CreateSkinInfo();
			KG_COM_PROCESS_ERROR(hrRetCode);

			if (!(m_dwOptionEx & MESH_SAVE_BONEBBOX))
			{
				CreateBBoxOfBone();
			}
		}

		DWORD dwTime = timeGetTime();

		if (m_uOption & MLO_MESH_SPEEDTREE)
		{
			m_dwOption &= ~MESH_SKINED;
		}

		hrRetCode = S_OK;

		if (m_dwOption & MESH_SKINED)
		{
			if (!m_ppMeshes[SMBT_BLENDED])
			{
				//如果SMBT_BLENDED已经存在了就不需要做其他处理
				hrRetCode = CreateSkinMesh();
			}
		}
		else
		{
			hrRetCode = OptimizeUnSkinedMesh();
		}

		if(SUCCEEDED(hrRetCode))
		{
			g_nNumTextureDynamicLoadedFrame++;
			m_bBBoxChanged = TRUE;

			FireEvent(KG3DEVENT_MESH_LOADED);
			g_nLoadCount++;
			int loadcost = (int)(timeGetTime() - dwTime);
			g_nLoadTime += loadcost;
			if(loadcost>30)
			{
				KGLogPrintf(KGLOG_ERR, "Mesh %d Load File %s.", loadcost,m_scName.c_str());
			}

			/*if(m_lpMeshStaticData)
			{
				g_cMeshStaticDataPool.FreeResource(m_lpMeshStaticData);
				m_lpMeshStaticData = NULL;
			}*/
			if(m_lpMeshBoneSocketData)
			{
				g_cMeshBoneSocketPool.FreeResource(m_lpMeshBoneSocketData);
				m_lpMeshBoneSocketData = NULL;
			}

			m_eDynamicLoadState = STATE_VIDEOMEM;
		}
		else
		{
			KGLogPrintf(KGLOG_ERR, "MULTITHREADREAD_FINISH Failed File %s.",m_scName.c_str());
		}
	}

	hrRetCode = S_OK;
Exit0:
	return hrRetCode;
}

HRESULT KG3DMesh::CheckLoadingState()
{
	HRESULT hrRetCode = E_FAIL;
	HRESULT hrResult = S_OK;
	DWORD   dwStartTime = timeGetTime();

	DWORD dwThreadID = ::GetCurrentThreadId();
	if(dwThreadID!=g_dwMainThreadID)
	{
		//dwThreadID ++;
	}	
	//_ASSERT(dwThreadID == g_dwMainThreadID);
	KG_PROCESS_ERROR(dwThreadID == g_dwMainThreadID);


	switch(m_eDynamicLoadState)
	{
	case STATE_MULTITHREAD_OKTOTHREAD:
		{
			hrResult = E_FAIL;

			m_eDynamicLoadState = STATE_MULTITHREAD_PREPEAR;
			g_cMultiThreadLoader.TryMultiThreadLoad_Mesh(this);

			DWORD dwCost = timeGetTime() - dwStartTime;
			if(dwCost > 30)
			{
				KGLogPrintf(KGLOG_ERR, "%d KG3DMesh::CheckLoadingState STATE_MULTITHREAD_OKTOTHREAD  %s.",dwCost , m_scName.c_str());
			}
		}
		break;
	case STATE_MULTITHREAD_PREPEAR:
		{
			hrResult = E_FAIL;
		}
		break;
	case STATE_MULTITHREADREAD_FINISH:
		{
			hrResult = MainThreadProcessLoadAfterMultiThread();
		}
		break;
	case STATE_NONE:
	case STATE_LOADFAILED:
		{
			hrResult = E_FAIL;
		}
		break;
	case STATE_VIDEOMEM:
		break;
	default:
		assert(0);
	}
//Exit1:
Exit0:

	DWORD dwCost = timeGetTime() - dwStartTime;
	if(g_cEngineOption.bEnableTimeOptimizeLog && dwCost > 10)
	{
		KGLogPrintf(KGLOG_ERR, "TimeOptimize %d KG3DMesh::CheckLoadingState  %s.",dwCost , m_scName.c_str());
	}

	return hrResult;
}


HRESULT KG3DMesh::LoadFromFile(const TCHAR* strFileName, unsigned uFileNameHash, unsigned uOption)
{
	HRESULT hrResult = E_FAIL;
	HRESULT hrRetCode  = E_FAIL;
	IFile* pFile = NULL;
	unsigned long uSize = 0;
	_MeshFileHead FileHeader;
		
	UnInit();
	KG_PROCESS_ERROR(strFileName);
	m_scName = strFileName;
	m_uOption = uOption;


	if((uOption & MLO_MESH_MULTI) && (g_eEngineOption.bUseMultiThreadLoad))
	{
		m_eDynamicLoadState = STATE_MULTITHREAD_OKTOTHREAD;
		g_nLoadCount++;
	}	
	else
	{
		DWORD dwTime = timeGetTime();

		pFile = g_OpenFile(strFileName);
		KG_PROCESS_ERROR(pFile);

		uSize = pFile->Read(&FileHeader, sizeof(_MeshFileHead));
		KG_PROCESS_ERROR(uSize == sizeof(_MeshFileHead));

		KG_PROCESS_ERROR(FileHeader.dwFileMask == 0x4D455348);

		/*if(!m_lpMeshStaticData)
		{
			m_lpMeshStaticData = g_cMeshStaticDataPool.RequestResource();
		}*/
		//KGLOG_PROCESS_ERROR(m_lpMeshStaticData);
		if(!m_lpMeshBoneSocketData)
		{
			m_lpMeshBoneSocketData = g_cMeshBoneSocketPool.RequestResource();
		}
		KGLOG_PROCESS_ERROR(m_lpMeshBoneSocketData);

        KGLOG_PROCESS_ERROR(FileHeader.dwVersion == 0);
        hrRetCode = Helper_LoadMeshVersion0(pFile, uOption);
        KGLOG_COM_PROCESS_ERROR(hrRetCode);    

		hrRetCode = LoadPropertyFromIni(strFileName);
		KG_COM_PROCESS_ERROR(hrRetCode);

		//g_cMeshStaticDataPool.FreeResource(m_lpMeshStaticData);
		g_cMeshBoneSocketPool.FreeResource(m_lpMeshBoneSocketData);
		//m_lpMeshStaticData = NULL;
		m_lpMeshBoneSocketData = NULL;

		m_eDynamicLoadState = STATE_VIDEOMEM;
		g_nNumTextureDynamicLoadedFrame++;

		g_nLoadCount++;
		int loadcost = (int)(timeGetTime() - dwTime);
		if(loadcost>30)
		{
			//KGLogPrintf(KGLOG_ERR, "UnMutiMesh %d Load File %s.", loadcost,m_scName.c_str());
		}

		g_nLoadTime += loadcost;
	}
    hrResult = S_OK;
Exit0:
	KG_COM_RELEASE(pFile);
	if (FAILED(hrResult) && strFileName)
	{
		KGLogPrintf(KGLOG_ERR, "Load file %s failed.", strFileName);
	}
	return hrResult;
}



HRESULT KG3DMesh::Helper_BlockReader(IFile *pFile,
									 DWORD& dwFVF,
									 DWORD dwOffset,
									 const _MeshHead* pMeshHeader,
									 const ReadBlockParameter& Parameter)
{
	HRESULT hrResult = E_FAIL;
	unsigned long uSize = 0;
	DWORD dwNumElement = *(reinterpret_cast<const DWORD*>(pMeshHeader) + Parameter.dwNumElementOffset);
	KG_PROCESS_ERROR(pFile);
	
	pFile->Seek(dwOffset, SEEK_SET);

	uSize = pFile->Read(Parameter.pBuffer, Parameter.dwElementSize * dwNumElement);
	KG_PROCESS_ERROR(uSize == Parameter.dwElementSize * dwNumElement);

	dwFVF |= Parameter.dwFVFAdd;
	dwFVF &= ~Parameter.dwFVFRemove;

Exit0:
	return hrResult;
}

HRESULT KG3DMesh::Helper_LoadMeshInformationFromFile(const char* strFileName,
													 unsigned uOption)
{
    HRESULT hrResult = E_FAIL;
	HRESULT hrRetCode = E_FAIL;

	IFile* pFile = NULL;
	unsigned long uSize = 0;
	_MeshFileHead FileHeader;
	
	pFile = g_OpenFile(strFileName);
	KG_PROCESS_ERROR(pFile);

	uSize = pFile->Read(&FileHeader, sizeof(_MeshFileHead));
	KG_PROCESS_ERROR(uSize == sizeof(_MeshFileHead));

	KG_PROCESS_ERROR(FileHeader.dwFileMask == _MeshFileHead::s_dwFileMask);
	
    KGLOG_PROCESS_ERROR(FileHeader.dwVersion == 0);
    hrRetCode = Helper_LoadMeshInformationFromFileVersion0(pFile, uOption);
    KG_COM_PROCESS_ERROR(hrRetCode);
	
	hrResult = S_OK;
Exit0:
	KG_COM_RELEASE(pFile);
	if (FAILED(hrResult))
	{
		KGLogPrintf(KGLOG_ERR, "KG3DMesh::Helper_LoadMeshInformationFromFile %s failed.", strFileName);
	}
	return hrResult;
}

HRESULT KG3DMesh::Helper_LoadMeshInformationFromFileVersion0(IFile *pFile, 
													 unsigned uOption)
{
	HRESULT hrRetCode = E_FAIL;
	HRESULT hrResult = E_FAIL;
	unsigned long uSize = 0;
	_MeshHead MeshHead;
	static KG3DMeshStaticData vMeshStaticData;

	KG_PROCESS_ERROR(pFile);

	uSize = pFile->Read(&MeshHead, sizeof(_MeshHead));
	KG_PROCESS_ERROR(uSize == sizeof(_MeshHead));

	KG_PROCESS_ERROR(MeshHead.dwNumVertices);
	KG_PROCESS_ERROR(MeshHead.dwNumFaces);
	KG_PROCESS_ERROR(MeshHead.dwNumSubset);

	if (MeshHead.Blocks.SkinInfoBlock)
	{
		m_dwOption |= MESH_SKINED;
	}

	DWORD dwThreadID = ::GetCurrentThreadId();

	if(dwThreadID == g_dwMainThreadID)
	{
		hrResult = m_MeshData.Init(MeshHead,&vMeshStaticData);
		KGLOG_COM_PROCESS_ERROR(hrResult);

		hrResult = m_MeshData.CreateFromKG3DMeshStaticData(&vMeshStaticData);
		KGLOG_COM_PROCESS_ERROR(hrResult);

		hrRetCode = Helper_CreateD3DXMesh(m_MeshData);
		KG_COM_PROCESS_ERROR(hrRetCode);
	}
	
	hrResult = S_OK;
Exit0:
	return hrResult;
}

HRESULT KG3DMeshCpuProcessData::PrepareRender()
{
	HRESULT hr = E_FAIL;
	DWORD dwNowTime = timeGetTime();
	KGLOG_PROCESS_ERROR(m_lpVertex);
	KGLOG_PROCESS_ERROR(m_lpIndex);

	if(!m_lpIB)
	{
		hr = g_pd3dDevice->CreateIndexBuffer(sizeof(WORD) * m_dwNumFace * 3,
			0,D3DFMT_INDEX16,D3DPOOL_MANAGED,&m_lpIB,NULL);
		KGLOG_COM_PROCESS_ERROR(hr);

		WORD* pIndex = NULL;
		hr = m_lpIB->Lock(0,0,(void**)&pIndex,0);
		KGLOG_COM_PROCESS_ERROR(hr);

		memcpy(pIndex,m_lpIndex,sizeof(WORD)*3*m_dwNumFace);

		hr = m_lpIB->Unlock();
		KGLOG_COM_PROCESS_ERROR(hr);
	}

	if(!m_lpVB)
	{
		hr = g_pd3dDevice->CreateVertexBuffer(sizeof(KG3DCpuProcessVertexFormat) * m_dwNumVertex,0,D3DFVF_CUSTOMVERTEX,D3DPOOL_MANAGED,&m_lpVB,NULL);
		KGLOG_COM_PROCESS_ERROR(hr);

		KG3DCpuProcessVertexFormat* pVertex = NULL;

		hr = m_lpVB->Lock(0,0,(void**)&pVertex,0);
		KGLOG_COM_PROCESS_ERROR(hr);

		memcpy(pVertex,m_lpVertex,sizeof(KG3DCpuProcessVertexFormat)*m_dwNumVertex);

		hr = m_lpVB->Unlock();
		KGLOG_COM_PROCESS_ERROR(hr);
	}

	if(g_cEngineOption.bEnableNormalMap && !m_lpVBTangent)
	{
		hr = g_pd3dDevice->CreateVertexBuffer(sizeof(D3DXVECTOR3) * m_dwNumVertex,0,
			D3DFVF_XYZ,D3DPOOL_MANAGED,&m_lpVBTangent,NULL);
		KGLOG_COM_PROCESS_ERROR(hr);

		D3DXVECTOR3* pTangent = NULL;

		hr = m_lpVBTangent->Lock(0,0,(void**)&pTangent,0);
		KGLOG_COM_PROCESS_ERROR(hr);

		ComputeTangent(pTangent);

		hr = m_lpVBTangent->Unlock();
		KGLOG_COM_PROCESS_ERROR(hr);
	}
	hr = S_OK;
Exit0:
	DWORD dwCost = timeGetTime() - dwNowTime;
	if(g_cEngineOption.bEnableTimeOptimizeLog && dwCost >= 5)
	{
		KGLogPrintf(KGLOG_ERR, "TimeOptimize %d KG3DMeshCpuProcessData::PrepareRender.",dwCost);
	}
	return hr;
}

HRESULT KG3DMeshCpuProcessData::Create(DWORD dwFace,DWORD dwVertex )
{
	SAFE_DELETE_ARRAY(m_lpIndex);
	SAFE_DELETE_ARRAY(m_lpVertex);

	m_dwNumFace   = dwFace;
	m_lpIndex = new WORD[m_dwNumFace * 3];
	KGLOG_PROCESS_ERROR(m_lpIndex);

	m_dwNumVertex = dwVertex;
	m_lpVertex = new KG3DCpuProcessVertexFormat[m_dwNumVertex];
	KGLOG_PROCESS_ERROR(m_lpVertex);

	m_lpVertexInfo = new BYTE[m_dwNumVertex];
	KGLOG_PROCESS_ERROR(m_lpVertexInfo);

	for (DWORD i=0;i<m_dwNumVertex;i++)
	{
		m_lpVertexInfo[i] = 0;
	}


	return S_OK;
Exit0:
	return E_FAIL;
}

D3DXVECTOR3 KG3DMeshCpuProcessData::calculateTangentSpaceVector(
	const D3DXVECTOR3& position1, const D3DXVECTOR3& position2, const D3DXVECTOR3& position3,
	float u1, float v1, float u2, float v2, float u3, float v3)
{
	//side0 is the vector along one side of the triangle of vertices passed in, 
	//and side1 is the vector along another side. Taking the cross product of these returns the normal.
	D3DXVECTOR3 side0 = position1 - position2;
	D3DXVECTOR3 side1 = position3 - position1;
	//Calculate face normal
	D3DXVECTOR3 normal;
	D3DXVec3Cross(&normal,&side1,&side0);
	D3DXVec3Normalize(&normal,&normal);
	//Now we use a formula to calculate the tangent. 
	D3DXVec3Normalize(&side0,&side0);
	D3DXVec3Normalize(&side1,&side1);


	float deltaV0 = v1 - v2;
	float deltaV1 = v3 - v1;
	D3DXVECTOR3 tangent = deltaV1 * side0 - deltaV0 * side1;
	D3DXVec3Normalize(&tangent,&tangent);
	//Calculate binormal
	float deltaU0 = u1 - u2;
	float deltaU1 = u3 - u1;
	D3DXVECTOR3 binormal = deltaU1 * side0 - deltaU0 * side1;
	D3DXVec3Normalize(&binormal,&binormal);
	//Now, we take the cross product of the tangents to get a vector which 
	//should point in the same direction as our normal calculated above. 
	//If it points in the opposite direction (the dot product between the normals is less than zero), 
	//then we need to reverse the s and t tangents. 
	//This is because the triangle has been mirrored when going from tangent space to object space.
	//reverse tangents if necessary
	D3DXVECTOR3 tangentCross;
	D3DXVec3Cross(&tangentCross,&tangent,&binormal);
	/*if ( D3DXVec3Dot(&tangentCross,&normal) < 0.0f)
	{
		tangent = -tangent;
		binormal = -binormal;
	}*/

	return tangent;

}

HRESULT KG3DMeshCpuProcessData::ComputeTangent(D3DXVECTOR3* pTangent)
{
	KGLOG_PROCESS_ERROR(pTangent);

	ZeroMemory(pTangent,sizeof(D3DXVECTOR3)*m_dwNumVertex);

	for (DWORD i_Face=0;i_Face<m_dwNumFace;i_Face++)
	{
		WORD wAIndex = m_lpIndex[i_Face * 3    ];
		WORD wBIndex = m_lpIndex[i_Face * 3 + 1];
		WORD wCIndex = m_lpIndex[i_Face * 3 + 2];

		KG3DCpuProcessVertexFormat& A = m_lpVertex[wAIndex];
		KG3DCpuProcessVertexFormat& B = m_lpVertex[wBIndex];
		KG3DCpuProcessVertexFormat& C = m_lpVertex[wCIndex];

		/*D3DXVECTOR3 pB_A = B.vPosition - A.vPosition;
		D3DXVECTOR3 pC_B = C.vPosition - B.vPosition;
		D3DXVECTOR3 pA_C = A.vPosition - C.vPosition;
		D3DXVec3Normalize(&pB_A,&pB_A);
		D3DXVec3Normalize(&pC_B,&pC_B);
		D3DXVec3Normalize(&pA_C,&pA_C);

		D3DXVECTOR3 uvB_A = D3DXVECTOR3(B.tu-A.tu,B.tv-A.tv,0);
		D3DXVECTOR3 uvC_B = D3DXVECTOR3(C.tu-B.tu,C.tv-B.tv,0);
		D3DXVECTOR3 uvA_C = D3DXVECTOR3(A.tu-C.tu,A.tv-C.tv,0);
		D3DXVec3Normalize(&uvB_A,&uvB_A);
		D3DXVec3Normalize(&uvC_B,&uvC_B);
		D3DXVec3Normalize(&uvA_C,&uvA_C);*/
		D3DXVECTOR3 uvB_A = calculateTangentSpaceVector(
			A.vPosition,C.vPosition,B.vPosition,
			A.tu,A.tv,C.tu,C.tv,B.tu,B.tv);

		D3DXVECTOR3 uvB_B = calculateTangentSpaceVector(
			B.vPosition,A.vPosition,C.vPosition,
			B.tu,B.tv,A.tu,A.tv,C.tu,C.tv);

		D3DXVECTOR3 uvB_C = calculateTangentSpaceVector(
			C.vPosition,B.vPosition,A.vPosition,
			C.tu,C.tv,B.tu,B.tv,A.tu,A.tv);
		
		float t0 = D3DXVec3Dot(&pTangent[wAIndex],&uvB_A);
		if(t0<0)
			uvB_A *= -1;
		float t1 = D3DXVec3Dot(&pTangent[wBIndex],&uvB_B);
		if(t1<0)
			uvB_B *= -1;
		float t2 = D3DXVec3Dot(&pTangent[wCIndex],&uvB_C);
		if(t2<0)
			uvB_C *= -1;

		pTangent[wAIndex] += uvB_A;
		pTangent[wBIndex] += uvB_B;
		pTangent[wCIndex] += uvB_C;
	}

	for(DWORD i = 0; i < m_dwNumVertex; i ++ )
	{
		/*D3DXVECTOR3 &T = pTangent[i];
		D3DXVECTOR3 &N = m_lpVertex[i].vNormal;
		float a = N.x * T.x + N.y * T.y + N.z * T.z;
		D3DXVec3Normalize(&pTangent[i], &(T - N * a));
		KGLogPrintf(KGLOG_WARNING, "%d Tangent %f %f %f ",i,T.x,T.y,T.z);*/
		D3DXVec3Normalize(&pTangent[i], &pTangent[i]);
		//D3DXVECTOR3 temp;
		//D3DXVec3Normalize(&vertex[i]->binormal, D3DXVec3Cross(&temp, &N, &vertex[i]->tangent));
	}

	WedingTangent(pTangent);
	WedingTangent(pTangent);
	WedingTangent(pTangent);

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DMeshCpuProcessData::WedingTangent(D3DXVECTOR3* pTangent)
{
	for(DWORD i = 0; i < m_dwNumVertex; i ++ )
	{
		KG3DCpuProcessVertexFormat& A = m_lpVertex[i];

		for(DWORD j=i+1;j<m_dwNumVertex;j++)
		{
			KG3DCpuProcessVertexFormat& B = m_lpVertex[j];

			D3DXVECTOR3 v = A.vPosition - B.vPosition;
			float L = D3DXVec3LengthSq(&v);

			if(L<1.0F)
			{
				D3DXVECTOR3 &TA = pTangent[i];
				D3DXVECTOR3 &TB = pTangent[j];
				
				D3DXVECTOR3 T = (TA + TB)*0.5F;
				TA = TB = T;
			}
		}
	}

	return S_OK;
}

//
//HRESULT KG3DMeshCpuProcessData::ComputeTangent(D3DXVECTOR3* pTangent)
//{
//	KGLOG_PROCESS_ERROR(pTangent);
//
//	for (DWORD i_Face=0;i_Face<m_dwNumFace;i_Face++)
//	{
//		int nOffset = 3 * i_Face;
//		int idxVertex0 = m_lpIndex[nOffset];
//		int idxVertex1 = m_lpIndex[nOffset + 1];
//		int idxVertex2 = m_lpIndex[nOffset + 2];
//
//		KG3DCpuProcessVertexFormat& A = m_lpVertex[idxVertex0];
//		KG3DCpuProcessVertexFormat& B = m_lpVertex[idxVertex1];
//		KG3DCpuProcessVertexFormat& C = m_lpVertex[idxVertex2];
//
//		D3DXVECTOR3 v0 = A.vPosition;
//		D3DXVECTOR3 v1 = B.vPosition;
//		D3DXVECTOR3 v2 = C.vPosition;
//
//		D3DXVECTOR2 w0 = D3DXVECTOR2(A.tu, A.tv);
//		D3DXVECTOR2 w1 = D3DXVECTOR2(B.tu, B.tv);
//		D3DXVECTOR2 w2 = D3DXVECTOR2(C.tu, C.tv);
//
//		float x1 = v1.x - v0.x;
//		float x2 = v2.x - v0.x;
//		float y1 = v1.y - v0.y;
//		float y2 = v2.y - v0.y;
//		float z1 = v1.z - v0.z;
//		float z2 = v2.z - v0.z;
//
//		float s1 = w1.x - w0.x;
//		float s2 = w2.x - w0.x;
//		float t1 = w1.y - w0.y;
//		float t2 = w2.y - w0.y;
//		float r = 1.0F / (s1 * t2 - s2 * t1);
//		D3DXVECTOR3 sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r);
//		D3DXVECTOR3 tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r, (s1 * z2 - s2 * z1) * r);
//
//		pTangent[idxVertex0] += sdir;
//		pTangent[idxVertex1] += sdir;
//		pTangent[idxVertex2] += sdir;
//
//		/*tan2[idxVertex0] += tdir;
//		tan2[idxVertex1] += tdir;
//		tan2[idxVertex2] += tdir;*/
//
//	}
//
//	for(DWORD i = 0; i < m_dwNumVertex; i ++ )
//	{
//		D3DXVECTOR3 &N = m_lpVertex[i].vNormal;
//		D3DXVECTOR3 &T = pTangent[i];
//		float a = N.x * T.x + N.y * T.y + N.z * T.z;
//		D3DXVec3Normalize(&pTangent[i], &(T - N * a));
//		//D3DXVECTOR3 temp;
//		//D3DXVec3Normalize(&vertex[i]->binormal, D3DXVec3Cross(&temp, &N, &vertex[i]->tangent));
//	}
//
//	return S_OK;
//Exit0:
//	return E_FAIL;
//}
HRESULT KG3DMesh::CreateDataForCpuProcess()
{
    /*
    {0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
    {0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 1},
    {0, 24, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT,0},
    {0, 36, D3DDECLTYPE_UBYTE4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDINDICES, 0},
    {0, 40, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0},
    {0, 52, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
    {0, 64, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1},
    ...
    */

    struct _Vertex 
    {
        D3DXVECTOR3 pos;
        D3DXVECTOR3 normal;
        D3DXVECTOR2 tex0;
    };

    HRESULT hr = E_FAIL;
    _Vertex* pVertex = NULL;
    WORD* pIndex = NULL;
    DWORD* pSubset = NULL;
    DWORD Stride = 0;

    KGLOG_PROCESS_ERROR(m_ppMeshes[SMBT_NORMAL]);
    KGLOG_PROCESS_ERROR(!m_lpCpuProcessData);

    m_lpCpuProcessData = new KG3DMeshCpuProcessData();
    KGLOG_PROCESS_ERROR(m_lpCpuProcessData);

    hr  = m_lpCpuProcessData->Create(m_MeshData.dwNumFaces, m_MeshData.dwNumVertices);
    KGLOG_COM_PROCESS_ERROR(hr);

    Stride = GetVertexStride(m_ppMeshes[SMBT_NORMAL]->GetFVF());

    hr = m_ppMeshes[SMBT_NORMAL]->LockVertexBuffer(D3DLOCK_READONLY, (void**)&pVertex);
    KGLOG_COM_PROCESS_ERROR(hr);

    if (FAILED(m_ppMeshes[SMBT_NORMAL]->LockIndexBuffer(D3DLOCK_READONLY, (void**)&pIndex)))
    {
        m_ppMeshes[SMBT_NORMAL]->UnlockVertexBuffer();
        KG_PROCESS_ERROR(false);
    }
    
    if (FAILED(m_ppMeshes[SMBT_NORMAL]->LockAttributeBuffer(0, &pSubset)))
    {
        m_ppMeshes[SMBT_NORMAL]->UnlockIndexBuffer();
        m_ppMeshes[SMBT_NORMAL]->UnlockVertexBuffer();
        KG_PROCESS_ERROR(false);
    }

    for (DWORD iFace = 0; iFace < m_MeshData.dwNumFaces; ++iFace)
    {
        DWORD dwSubsetIndex = pSubset[iFace];

        for (int i = 0; i < 3; ++i)
        {
            DWORD dwIndex = iFace * 3 + i;
            DWORD dwVertexIndex = pIndex[dwIndex];
            _Vertex* pv = (_Vertex*)(((BYTE*)pVertex) + Stride * dwVertexIndex);

            m_lpCpuProcessData->m_lpIndex[dwIndex] = (WORD)dwVertexIndex;

            KG3DCpuProcessVertexFormat* p = &m_lpCpuProcessData->m_lpVertex[dwVertexIndex];
            p->vPosition = pv->pos;
            p->vNormal   = pv->normal;
            p->tu        = pv->tex0.x;
            p->tv        = pv->tex0.y;
            p->tIndex    = (float)dwSubsetIndex;

            m_lpCpuProcessData->m_lpVertexInfo[dwVertexIndex] = (BYTE)dwSubsetIndex;
        }

    }

    m_ppMeshes[SMBT_NORMAL]->UnlockAttributeBuffer();
    m_ppMeshes[SMBT_NORMAL]->UnlockIndexBuffer();
    m_ppMeshes[SMBT_NORMAL]->UnlockVertexBuffer();

    for(DWORD i = 0; i < m_dwNumBone; i++)
    {
        BoneInfo1& BoneInfo = m_BoneInfo[i];
        if(BoneInfo.IsFlexibleBone)
        {
            for (DWORD k=0;k<BoneInfo.NumVerticesRef;k++)
            {
                DWORD dwIndex = BoneInfo.VerticesIndexRef[k];
                m_lpCpuProcessData->m_lpVertexInfo[dwIndex] = 1;
            }
        }
    }

    return S_OK;
Exit0:
    return E_FAIL;

}

HRESULT KG3DMesh::CreateDataForCpuProcess(_MeshHead* pMeshHead,ReadInMeshData* pMeshData)
{
	HRESULT hr = E_FAIL;

	KGLOG_PROCESS_ERROR(!m_lpCpuProcessData);

	m_lpCpuProcessData = new KG3DMeshCpuProcessData();
	KGLOG_PROCESS_ERROR(m_lpCpuProcessData);

	hr  = m_lpCpuProcessData->Create(pMeshData->dwNumFaces,pMeshData->dwNumVertices);
	KGLOG_COM_PROCESS_ERROR(hr);

	for (DWORD iFace = 0;iFace < pMeshData->dwNumFaces;iFace++)
	{
		DWORD dwSubsetIndex = pMeshData->pSubsetIndices[iFace];

		for (int i=0;i<3;i++)
		{
			DWORD dwIndex = iFace * 3 + i;
			
			DWORD dwVertexIndex = pMeshData->pFaceIndices[dwIndex];

			if(dwVertexIndex >= pMeshData->dwNumVertices)
			{
				int S = 0;
				S++;
			}
			KGLOG_PROCESS_ERROR( dwVertexIndex < pMeshData->dwNumVertices );
			
			{

				m_lpCpuProcessData->m_lpIndex[dwIndex] = (WORD)dwVertexIndex;

				KG3DCpuProcessVertexFormat* p = &m_lpCpuProcessData->m_lpVertex[dwVertexIndex];
				p->vPosition = pMeshData->pPos    [dwVertexIndex];
				p->vNormal   = pMeshData->pNormals[dwVertexIndex];
				p->tu        = pMeshData->pUV1    [dwVertexIndex].x;
				p->tv        = pMeshData->pUV1    [dwVertexIndex].y;
				p->tIndex    = (float)dwSubsetIndex;

				m_lpCpuProcessData->m_lpVertexInfo[dwVertexIndex] = (BYTE)dwSubsetIndex;
			}
		}
	}

	for(DWORD i = 0; i < m_dwNumBone; i++)
	{
		BoneInfo1& BoneInfo = m_BoneInfo[i];
		if(BoneInfo.IsFlexibleBone)
		{
			for (DWORD k=0;k<BoneInfo.NumVerticesRef;k++)
			{
				DWORD dwIndex = BoneInfo.VerticesIndexRef[k];
				m_lpCpuProcessData->m_lpVertexInfo[dwIndex] = 1;
			}
		}
	}

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DMesh::Helper_LoadMeshFromFile(IFile *pFile,
										  BOOL bMultiThread)
{
	HRESULT hrResult = E_FAIL;
	HRESULT hrRetCode  = E_FAIL;
	unsigned long uSize = 0;
	_MeshHead MeshHead;
	static KG3DMeshStaticData vMeshStaticData;
	static KG3DMeshStaticData vMeshStaticDataMutiThread;
	
	uSize = pFile->Read(&MeshHead, sizeof(_MeshHead));
	KG_PROCESS_ERROR(uSize == sizeof(_MeshHead));

	KG_PROCESS_ERROR(MeshHead.dwNumVertices);
	KG_PROCESS_ERROR(MeshHead.dwNumFaces);
	KG_PROCESS_ERROR(MeshHead.dwNumSubset);

	//////////////////////////////////////////////////////////////////////////
	KGLOG_PROCESS_ERROR(m_lpMeshBoneSocketData);
	//////////////////////////////////////////////////////////////////////////

	if (bMultiThread)
	{
		for (size_t i = 0; i < KG3DMESH_READ_BLOCK_PARAMETER; ++i)
		{
			if (MeshHead.dwBlocks[i])
			{
				Helper_BlockReader(pFile, 
					m_MeshData.dwMeshFVF, 
					MeshHead.dwBlocks[i], 
					&MeshHead, 
					vMeshStaticDataMutiThread.ReadBlockParameter[i]);
			}
		}
		hrResult = m_MeshData.Init(MeshHead,&vMeshStaticDataMutiThread);
		KGLOG_COM_PROCESS_ERROR(hrResult);

		hrResult = m_MeshData.CreateFromKG3DMeshStaticData(&vMeshStaticDataMutiThread);
		KGLOG_COM_PROCESS_ERROR(hrResult);

		CheckAndLoadBspFile(MeshHead);
	}
	else
	{
		for (size_t i = 0; i < KG3DMESH_READ_BLOCK_PARAMETER; ++i)
		{
			if (MeshHead.dwBlocks[i])
			{
				Helper_BlockReader(pFile, 
					m_MeshData.dwMeshFVF, 
					MeshHead.dwBlocks[i], 
					&MeshHead,
					vMeshStaticData.ReadBlockParameter[i]);
			}
		}
		hrResult = m_MeshData.Init(MeshHead,&vMeshStaticData);
		KGLOG_COM_PROCESS_ERROR(hrResult);

		hrResult = m_MeshData.CreateFromKG3DMeshStaticData(&vMeshStaticData);
		KGLOG_COM_PROCESS_ERROR(hrResult);

		CheckAndLoadBspFile(MeshHead);
	}


	m_BBox.Clear();
	m_BBox.AddPosition(m_MeshData.pPos, MeshHead.dwNumVertices);

    m_dwNumSubset = m_MeshData.dwNumSubset;

	//read skin info
	if (MeshHead.Blocks.SkinInfoBlock)
	{
		if (MeshHead.Blocks.BBoxBlock)
		{
			m_dwOptionEx |= MESH_SAVE_BONEBBOX;
		}
		pFile->Seek(MeshHead.Blocks.SkinInfoBlock, SEEK_SET);
		hrRetCode = Helper_LoadSkinInfoFromFile(pFile);
		KGLOG_COM_PROCESS_ERROR(hrRetCode);

		hrRetCode = ConvertBoneToCurrentVersion();
		KGLOG_COM_PROCESS_ERROR(hrRetCode);

		hrRetCode = Helper_LoadSocketFromFile(pFile);
		KGLOG_COM_PROCESS_ERROR(hrRetCode);

		hrRetCode = ConvertSocketToCurrentVersion();
		KGLOG_COM_PROCESS_ERROR(hrRetCode);
	}
	
	//////////////////////////////////////////////////////////////////////////
	if(g_cEngineOption.bMultiTextureRender)
		CreateDataForCpuProcess(&MeshHead,&m_MeshData);

	hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KG3DMesh::Helper_LoadSkinInfoFromFile(IFile* pFile)
{
	HRESULT hrResult = E_FAIL;
	size_t uSize = 0;
	TCHAR strBoneName[MAX_BONE_NAME_LENGTH] = {};
	TCHAR strParentName[MAX_BONE_NAME_LENGTH] = {};
	TCHAR strChildName[MAX_BONE_NAME_LENGTH] = {};

	m_dwNumBone = 0;
	m_nBaseBoneID.clear();

	KGLOG_PROCESS_ERROR(pFile);
	
	uSize = pFile->Read(&m_dwNumBone, sizeof(DWORD));
	KGLOG_PROCESS_ERROR(uSize == sizeof(DWORD));
	KGLOG_PROCESS_ERROR(m_dwNumBone < s_dwMaxBone);
	KG_PROCESS_SUCCESS(m_dwNumBone == 0);

	KGLOG_PROCESS_ERROR(m_lpMeshBoneSocketData);
	//////////////////////////////////////////////////////////////////////////

	for (DWORD i = 0; i < m_dwNumBone; i++)
	{
		//Read bone name
		uSize = pFile->Read(strBoneName, sizeof(TCHAR) * MAX_BONE_NAME_LENGTH);
		KGLOG_PROCESS_ERROR(uSize == MAX_BONE_NAME_LENGTH * sizeof(TCHAR));
		strBoneName[MAX_BONE_NAME_LENGTH-1] = '\0';
		_strlwr_s(strBoneName, MAX_BONE_NAME_LENGTH);

		//Read parent name
		uSize = pFile->Read(strParentName, sizeof(TCHAR) * MAX_BONE_NAME_LENGTH);
		KGLOG_PROCESS_ERROR(uSize == MAX_BONE_NAME_LENGTH * sizeof(TCHAR));
		strParentName[MAX_BONE_NAME_LENGTH-1] = '\0';
		_strlwr_s(strParentName, MAX_BONE_NAME_LENGTH);

		m_lpMeshBoneSocketData->g_BoneInfo[i].scName = strBoneName;
		m_lpMeshBoneSocketData->g_BoneInfo[i].scParentName = strParentName;
	
		DWORD dwNumChild = 0;
		uSize = pFile->Read(&dwNumChild, sizeof(DWORD));
		KGLOG_PROCESS_ERROR(uSize == sizeof(DWORD));

		m_lpMeshBoneSocketData->g_BoneInfo[i].ChildNameVector.clear();
		m_lpMeshBoneSocketData->g_BoneInfo[i].ChildIDVector.clear();
		
		for (DWORD j = 0; j < dwNumChild; j++)
		{
			uSize = pFile->Read(strChildName, sizeof(TCHAR) * MAX_BONE_NAME_LENGTH);
			KGLOG_PROCESS_ERROR(uSize == MAX_BONE_NAME_LENGTH * sizeof(TCHAR));
			strChildName[MAX_BONE_NAME_LENGTH-1] = '\0';
			//_strlwr_s(strChildName, MAX_BONE_NAME_LENGTH);
			// g_BoneInfo[i].ChildNameVector.push_back(strChildName);
			// _strlwr_s 多线程下有问题，改用 std::string 的。
			std::string s(strChildName);
			transform(s.begin(), s.end(), s.begin(), tolower);
			m_lpMeshBoneSocketData->g_BoneInfo[i].ChildNameVector.push_back(s);
		}

		uSize = pFile->Read(&m_lpMeshBoneSocketData->g_BoneInfo[i].MatrixOffset, sizeof(D3DXMATRIX));
		KGLOG_PROCESS_ERROR(uSize == sizeof(D3DXMATRIX));

		uSize = pFile->Read(&m_lpMeshBoneSocketData->g_BoneInfo[i].MatrixOffsetBase, sizeof(D3DXMATRIX));
		KGLOG_PROCESS_ERROR(uSize == sizeof(D3DXMATRIX));

		uSize = pFile->Read(&m_lpMeshBoneSocketData->g_BoneInfo[i].NumVerticesRef, sizeof(DWORD));
		KGLOG_PROCESS_ERROR(uSize == sizeof(DWORD));
		
		//SAFE_DELETE_ARRAY(g_BoneInfo[i].VerticesIndexRef);
		m_lpMeshBoneSocketData->g_BoneInfo[i].VerticesIndexRef = new DWORD[m_lpMeshBoneSocketData->g_BoneInfo[i].NumVerticesRef];
		KGLOG_PROCESS_ERROR(m_lpMeshBoneSocketData->g_BoneInfo[i].VerticesIndexRef);

		//SAFE_DELETE_ARRAY(g_BoneInfo[i].VerticesWeightRef);
		m_lpMeshBoneSocketData->g_BoneInfo[i].VerticesWeightRef = new float[m_lpMeshBoneSocketData->g_BoneInfo[i].NumVerticesRef];
		KGLOG_PROCESS_ERROR(m_lpMeshBoneSocketData->g_BoneInfo[i].VerticesWeightRef);
		
		uSize = pFile->Read(m_lpMeshBoneSocketData->g_BoneInfo[i].VerticesIndexRef, sizeof(DWORD) * m_lpMeshBoneSocketData->g_BoneInfo[i].NumVerticesRef);
		KGLOG_PROCESS_ERROR(uSize == m_lpMeshBoneSocketData->g_BoneInfo[i].NumVerticesRef * sizeof(DWORD));

		uSize  = pFile->Read(m_lpMeshBoneSocketData->g_BoneInfo[i].VerticesWeightRef, sizeof(float) * m_lpMeshBoneSocketData->g_BoneInfo[i].NumVerticesRef);
		KGLOG_PROCESS_ERROR(uSize == m_lpMeshBoneSocketData->g_BoneInfo[i].NumVerticesRef * sizeof(float));

		D3DXMatrixInverse(&m_lpMeshBoneSocketData->g_BoneInfo[i].CurMatrix, NULL, &m_lpMeshBoneSocketData->g_BoneInfo[i].MatrixOffset);
		
		if (m_dwOptionEx & MESH_SAVE_BONEBBOX)
		{
			uSize = pFile->Read(&m_lpMeshBoneSocketData->g_BoneInfo[i].BBoxScaleMatrix, sizeof(D3DXMATRIX));
			KGLOG_PROCESS_ERROR(uSize == sizeof(D3DXMATRIX));
			uSize = pFile->Read(&m_lpMeshBoneSocketData->g_BoneInfo[i].bBox.A, sizeof(D3DXVECTOR3));
			KGLOG_PROCESS_ERROR(uSize == sizeof(D3DXVECTOR3));
			uSize = pFile->Read(&m_lpMeshBoneSocketData->g_BoneInfo[i].bBox.B, sizeof(D3DXVECTOR3));
			KGLOG_PROCESS_ERROR(uSize == sizeof(D3DXVECTOR3));
			uSize = pFile->Read(&m_lpMeshBoneSocketData->g_BoneInfo[i].bBox.bInit, sizeof(BOOL));
			KGLOG_PROCESS_ERROR(uSize == sizeof(BOOL));
		}
	}
	
	m_dwOption |= MESH_SKINED;
	Helper_AdjustSkeleton();
Exit1:
	hrResult = S_OK;
Exit0:
	if (FAILED(hrResult))
	{
		m_dwNumBone = 0;
	}
	return hrResult;
}

int FindBoneOrg(LPCSTR strBoneName, int nNumBones,BoneSocketData* pData)
{
	int nReturn = -1;
	KG3DMesh::_BoneInfo* pInfo = NULL;
	KG_PROCESS_ERROR(strBoneName);
	for(int i = 0; i < nNumBones; i++)
	{
		pInfo = &pData->g_BoneInfo[i];
		if (!stricmp(pData->g_BoneInfo[i].scName.c_str(), strBoneName))
		{
			nReturn = i;
			break;
		}
	}
Exit0:
	return nReturn;	
}

void KG3DMesh::Helper_AdjustSkeleton()
{
	m_nBaseBoneID.clear();

	KG_PROCESS_ERROR(m_dwNumBone);

	for (DWORD i = 0; i < m_dwNumBone; i++)
	{
		for (size_t j = 0; j < m_lpMeshBoneSocketData->g_BoneInfo[i].ChildNameVector.size(); j++)
		{
			int nBoneIndex = FindBoneOrg(m_lpMeshBoneSocketData->g_BoneInfo[i].ChildNameVector[j].c_str(), m_dwNumBone,m_lpMeshBoneSocketData);
			if (nBoneIndex != -1)
			{
				m_lpMeshBoneSocketData->g_BoneInfo[i].ChildIDVector.push_back(nBoneIndex);
				m_lpMeshBoneSocketData->g_BoneInfo[nBoneIndex].nParentIndex = i;
			}
		}
		if (FindBoneOrg(m_lpMeshBoneSocketData->g_BoneInfo[i].scParentName.c_str(), m_dwNumBone,m_lpMeshBoneSocketData) == -1)
		{
			m_lpMeshBoneSocketData->g_BoneInfo[i].nParentIndex = -1;
			m_nBaseBoneID.push_back(i);
		}
	}
Exit0:
	;
}

HRESULT KG3DMesh::Helper_LoadSocketFromFile(IFile* pFile)
{
	HRESULT hrResult = E_FAIL;
	unsigned long uSize = 0;
	TCHAR strSocketName[MAX_BONE_NAME_LENGTH];
	TCHAR strParentBone[MAX_BONE_NAME_LENGTH];

	//Clear first
	m_dwNumSocket = 0;

	KGLOG_PROCESS_ERROR(pFile);
	KGLOG_PROCESS_ERROR(m_lpMeshBoneSocketData);
	
	uSize = pFile->Read(&m_dwNumSocket, sizeof(DWORD));
	KG_PROCESS_ERROR(uSize == sizeof(DWORD));

	KG_PROCESS_SUCCESS(m_dwNumSocket == 0);
		
	for (DWORD i = 0; i < m_dwNumSocket; i++)
	{
		uSize = pFile->Read(strSocketName, sizeof(TCHAR) * MAX_BONE_NAME_LENGTH);
		KG_PROCESS_ERROR(uSize == MAX_BONE_NAME_LENGTH * sizeof(TCHAR));
		_strlwr_s(strSocketName);

		uSize = pFile->Read(strParentBone, sizeof(TCHAR) * MAX_BONE_NAME_LENGTH);
		KG_PROCESS_ERROR(uSize == MAX_BONE_NAME_LENGTH * sizeof(TCHAR));
		_strlwr_s(strParentBone);

		uSize = pFile->Read(&m_lpMeshBoneSocketData->g_Sockets[i].MatrixOffset, sizeof(D3DXMATRIX));
		KG_PROCESS_ERROR(uSize == sizeof(D3DXMATRIX));

		m_lpMeshBoneSocketData->g_Sockets[i].scName = strSocketName;
		m_lpMeshBoneSocketData->g_Sockets[i].scParentBoneName = strParentBone;

		int nParentIndex = FindBone(strParentBone);
		if (nParentIndex != -1 )
		{
			m_lpMeshBoneSocketData->g_Sockets[i].m_nParentBone = nParentIndex;
			m_lpMeshBoneSocketData->g_Sockets[i].CurMatrix = m_lpMeshBoneSocketData->g_Sockets[i].MatrixOffset * m_lpMeshBoneSocketData->g_BoneInfo[nParentIndex].CurMatrix;
		}
	}
Exit1:
	hrResult = S_OK;
Exit0:
	if (FAILED(hrResult))
	{
		m_dwNumSocket = 0;
	}
	return hrResult;
}


HRESULT KG3DMesh::Helper_CreateD3DXMesh(const ReadInMeshData& Data)
{
	HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
	
	KG_PROCESS_SUCCESS(m_ppMeshes[SMBT_NORMAL]);
	
	if (Data.dwNumVertices <= 65535)
	{
        hr = D3DXCreateMeshFVF(
            Data.dwNumFaces, 
            Data.dwNumVertices, 
            D3DXMESH_SYSTEMMEM, 
            Data.dwMeshFVF, 
            g_pd3dDevice, 
            &m_ppMeshes[SMBT_NORMAL]
        );
		if(FAILED(hr))
		{
			int k = 0;
		}
        KGLOG_COM_PROCESS_ERROR(hr);
    }
	else
	{
        hr = D3DXCreateMeshFVF(
            Data.dwNumFaces,
            Data.dwNumVertices, 
            D3DXMESH_SYSTEMMEM | D3DXMESH_32BIT, 
            Data.dwMeshFVF, 
            g_pd3dDevice, 
            &m_ppMeshes[SMBT_NORMAL]
        );
		if(FAILED(hr))
		{
			int k = 0;
		}
        KGLOG_COM_PROCESS_ERROR(hr);
	}

Exit1:
    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        SAFE_RELEASE(m_ppMeshes[SMBT_NORMAL]);
    }
	return hrResult;
}
HRESULT KG3DMesh::SaveToFile(const TCHAR *cszFileName,   unsigned uOption)
{
	HRESULT hr = E_FAIL;
	HRESULT hrResult = E_FAIL;
	FILE* fpFile = NULL;
	_MeshFileHead MeshFileHead;
	_MeshHead     MeshHead;
	DWORD BlockEnd  = 0xFFFFFFFF;  
	D3DXVECTOR3* pVerteicePosition  = NULL;
	D3DXVECTOR3* pVerticeNormal     = NULL;
	D3DCOLOR* pVerticeDiffuse       = NULL;
	D3DXVECTOR3* pVerticeTextCoord1 = NULL;
	D3DXVECTOR3* pVerticeTextCoord2 = NULL;
	D3DXVECTOR3* pVerticeTextCoord3 = NULL;
	DWORD* pFacesIndex              = NULL;
	DWORD* pSubsetID                = NULL;
	WORD* pIndex                    = NULL;
	DWORD * pAttrib                 = NULL;
	DWORD  Pos_Start = 0;
	DWORD  dwNumVertices = 0;
	DWORD  dwNumFaces    = 0;
	DWORD  FVF = 0;
	BOOL bSkined = FALSE;

	KG_PROCESS_ERROR(cszFileName);
	KG_PROCESS_ERROR(m_ppMeshes[SMBT_NORMAL]);
	fpFile = fopen(cszFileName, "wb");
	KGLOG_PROCESS_ERROR(fpFile);

	Pos_Start = ftell(fpFile);
	dwNumVertices = m_ppMeshes[SMBT_NORMAL]->GetNumVertices();
	dwNumFaces    = m_ppMeshes[SMBT_NORMAL]->GetNumFaces();
	FVF = m_ppMeshes[SMBT_NORMAL]->GetFVF();

	MeshFileHead.MeshCount_NotUse = 1;
	MeshHead.dwNumFaces =    dwNumFaces;
	MeshHead.dwNumVertices = dwNumVertices;
	MeshHead.dwNumSubset =   m_dwNumSubset;

	if (uOption == MODEL_SAVEOPT_SPEEDTREE)
	{
		m_dwOption |= MESH_SKINED;
	}

	bSkined = (m_dwOption & MESH_SKINED);
	//(by huangjinshou
	if (m_dwOptionEx & MESH_SAVE_BONEBBOX )//保存Bone包围合
	{
		MeshHead.Blocks.BBoxBlock = 1;//true;
	}

	fwrite(&MeshFileHead, sizeof(_MeshFileHead), 1, fpFile);
	fwrite(&MeshHead, sizeof(_MeshHead), 1, fpFile);

	switch(FVF)
	{
	case (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1):
		{
			VFormat::FACES_NORMAL_TEXTURE1 * pVers = NULL;
			hr = m_ppMeshes[SMBT_NORMAL]->LockVertexBuffer(0, (void**) &pVers);
			KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

			hr = m_ppMeshes[SMBT_NORMAL]->LockIndexBuffer (0, (void**) &pIndex);
			KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

			hr = m_ppMeshes[SMBT_NORMAL]->LockAttributeBuffer(0, (DWORD**) &pAttrib);
			KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

			pVerteicePosition = new D3DXVECTOR3[dwNumVertices];
			KG_PROCESS_ERROR(pVerteicePosition);
			ZeroMemory(pVerteicePosition, sizeof(D3DXVECTOR3)*dwNumVertices);

			pVerticeNormal = new D3DXVECTOR3[dwNumVertices];
			KG_PROCESS_ERROR(pVerticeNormal);
			ZeroMemory(pVerticeNormal, sizeof(D3DXVECTOR3)*dwNumVertices);

			pVerticeTextCoord1 = new D3DXVECTOR3[dwNumVertices];
			KG_PROCESS_ERROR(pVerticeTextCoord1);
			ZeroMemory(pVerticeTextCoord1, sizeof(D3DXVECTOR3)*dwNumVertices);

			pFacesIndex = new DWORD[dwNumFaces*3];
			KG_PROCESS_ERROR(pFacesIndex);
			ZeroMemory(pFacesIndex, sizeof(DWORD)*dwNumFaces*3);

			pSubsetID = new DWORD[dwNumFaces];
			KG_PROCESS_ERROR(pSubsetID);
			ZeroMemory(pSubsetID, sizeof(DWORD) * dwNumFaces);

			for(DWORD i = 0; i < dwNumVertices; i++)
			{
				pVerteicePosition[i] = pVers[i].p;
				pVerticeNormal[i]   = pVers[i].Normal;
				pVerticeTextCoord1[i].x = pVers[i].tu1;
				pVerticeTextCoord1[i].y = pVers[i].tv1;
			}

			for(DWORD i = 0; i < dwNumFaces; i++)
			{
				pFacesIndex[i * 3] = pIndex[i * 3];
				pFacesIndex[i * 3 + 1] = pIndex[i * 3 + 1];
				pFacesIndex[i * 3 + 2] = pIndex[i * 3 + 2];
				pSubsetID[i] = pAttrib[i];
			}
			break;
		}
	case (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX2):
		{
			VFormat::FACES_NORMAL_TEXTURE2 * pVers = NULL;

			hr = m_ppMeshes[SMBT_NORMAL]->LockVertexBuffer(0, (void**) &pVers);
			KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

			hr = m_ppMeshes[SMBT_NORMAL]->LockIndexBuffer (0, (void**) &pIndex);
			KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

			hr = m_ppMeshes[SMBT_NORMAL]->LockAttributeBuffer(0, (DWORD**) &pAttrib);
			KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

			pVerteicePosition = new D3DXVECTOR3[dwNumVertices];
			KG_PROCESS_ERROR(pVerteicePosition);
			ZeroMemory(pVerteicePosition, sizeof(D3DXVECTOR3)*dwNumVertices);

			pVerticeNormal = new D3DXVECTOR3[dwNumVertices];
			KG_PROCESS_ERROR(pVerticeNormal);
			ZeroMemory(pVerticeNormal, sizeof(D3DXVECTOR3)*dwNumVertices);

			pVerticeTextCoord1 = new D3DXVECTOR3[dwNumVertices];
			KG_PROCESS_ERROR(pVerticeTextCoord1);
			ZeroMemory(pVerticeTextCoord1, sizeof(D3DXVECTOR3)*dwNumVertices);

			pVerticeTextCoord2 = new D3DXVECTOR3[dwNumVertices];
			KG_PROCESS_ERROR(pVerticeTextCoord2);
			ZeroMemory(pVerticeTextCoord2, sizeof(D3DXVECTOR3)*dwNumVertices);

			pFacesIndex = new DWORD[dwNumFaces*3];
			KG_PROCESS_ERROR(pFacesIndex);
			ZeroMemory(pFacesIndex, sizeof(DWORD)*dwNumFaces*3);

			pSubsetID = new DWORD[dwNumFaces];
			KG_PROCESS_ERROR(pSubsetID);
			ZeroMemory(pSubsetID, sizeof(DWORD)*dwNumFaces);

			for(DWORD i=0;i<dwNumVertices;i++)
			{
				pVerteicePosition[i] = pVers[i].p;
				pVerticeNormal[i]   = pVers[i].Normal;
				pVerticeTextCoord1[i].x = pVers[i].tu1;
				pVerticeTextCoord1[i].y = pVers[i].tv1;
				pVerticeTextCoord2[i].x = pVers[i].tu2;
				pVerticeTextCoord2[i].y = pVers[i].tv2;
			}
			for(DWORD i=0;i<dwNumFaces;i++)
			{
				pFacesIndex[i*3  ] = pIndex[i*3  ];
				pFacesIndex[i*3+1] = pIndex[i*3+1];
				pFacesIndex[i*3+2] = pIndex[i*3+2];
				pSubsetID[i] = pAttrib[i];
			}
			break;
		}
	case (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX3):
		{
			VFormat::FACES_NORMAL_TEXTURE3 * pVers = NULL;
			hr = m_ppMeshes[SMBT_NORMAL]->LockVertexBuffer(0, (void**) &pVers);
			KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

			hr = m_ppMeshes[SMBT_NORMAL]->LockIndexBuffer (0, (void**) &pIndex);
			KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

			hr = m_ppMeshes[SMBT_NORMAL]->LockAttributeBuffer(0, (DWORD**) &pAttrib);
			KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

			pVerteicePosition = new D3DXVECTOR3[dwNumVertices];
			KG_PROCESS_ERROR(pVerteicePosition);
			ZeroMemory(pVerteicePosition, sizeof(D3DXVECTOR3)*dwNumVertices);

			pVerticeNormal = new D3DXVECTOR3[dwNumVertices];
			KG_PROCESS_ERROR(pVerticeNormal);
			ZeroMemory(pVerticeNormal, sizeof(D3DXVECTOR3)*dwNumVertices);

			pVerticeTextCoord1 = new D3DXVECTOR3[dwNumVertices];
			KG_PROCESS_ERROR(pVerticeTextCoord1);
			ZeroMemory(pVerticeTextCoord1, sizeof(D3DXVECTOR3)*dwNumVertices);

			pVerticeTextCoord2 = new D3DXVECTOR3[dwNumVertices];
			KG_PROCESS_ERROR(pVerticeTextCoord2);
			ZeroMemory(pVerticeTextCoord2, sizeof(D3DXVECTOR3)*dwNumVertices);

			pVerticeTextCoord3 = new D3DXVECTOR3[dwNumVertices];
			KG_PROCESS_ERROR(pVerticeTextCoord3);
			ZeroMemory(pVerticeTextCoord3, sizeof(D3DXVECTOR3)*dwNumVertices);

			pFacesIndex = new DWORD[dwNumFaces*3];
			KG_PROCESS_ERROR(pFacesIndex);
			ZeroMemory(pFacesIndex, sizeof(DWORD)*dwNumFaces*3);

			pSubsetID = new DWORD[dwNumFaces];
			KG_PROCESS_ERROR(pSubsetID);
			ZeroMemory(pSubsetID, sizeof(DWORD)*dwNumFaces);

			for(DWORD i=0;i<dwNumVertices;i++)
			{
				pVerteicePosition[i] = pVers[i].p;
				pVerticeNormal[i]   = pVers[i].Normal;
				pVerticeTextCoord1[i].x = pVers[i].tu1;
				pVerticeTextCoord1[i].y = pVers[i].tv1;
				pVerticeTextCoord2[i].x = pVers[i].tu2;
				pVerticeTextCoord2[i].y = pVers[i].tv2;
				pVerticeTextCoord3[i].x = pVers[i].tu3;
				pVerticeTextCoord3[i].y = pVers[i].tv3;
			}
			for (DWORD i = 0; i < dwNumFaces; i++)
			{
				pFacesIndex[i*3  ] = pIndex[i*3  ];
				pFacesIndex[i*3+1] = pIndex[i*3+1];
				pFacesIndex[i*3+2] = pIndex[i*3+2];
				pSubsetID[i] = pAttrib[i];
			}
			break;
		}

	case (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1):
		{
			VFormat::FACES_DIFFUSE_TEXTURE1 * pVers = NULL;
			hr = m_ppMeshes[SMBT_NORMAL]->LockVertexBuffer(0, (void**) &pVers);
			KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

			hr = m_ppMeshes[SMBT_NORMAL]->LockIndexBuffer (0, (void**) &pIndex);
			KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

			hr = m_ppMeshes[SMBT_NORMAL]->LockAttributeBuffer(0, (DWORD**) &pAttrib);
			KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

			pVerteicePosition = new D3DXVECTOR3[dwNumVertices];
			KG_PROCESS_ERROR(pVerteicePosition);
			ZeroMemory(pVerteicePosition, sizeof(D3DXVECTOR3)*dwNumVertices);

			pVerticeDiffuse = new D3DCOLOR[dwNumVertices];
			KG_PROCESS_ERROR(pVerticeDiffuse);
			ZeroMemory(pVerticeDiffuse, sizeof(D3DCOLOR)*dwNumVertices);

			pVerticeTextCoord1 = new D3DXVECTOR3[dwNumVertices];
			KG_PROCESS_ERROR(pVerticeTextCoord1);
			ZeroMemory(pVerticeTextCoord1, sizeof(D3DXVECTOR3)*dwNumVertices);

			pFacesIndex = new DWORD[dwNumFaces*3];
			KG_PROCESS_ERROR(pFacesIndex);
			ZeroMemory(pFacesIndex, sizeof(DWORD)*dwNumFaces*3);

			pSubsetID = new DWORD[dwNumFaces];
			KG_PROCESS_ERROR(pSubsetID);
			ZeroMemory(pSubsetID, sizeof(DWORD)*dwNumFaces);

			for(DWORD i=0;i<dwNumVertices;i++)
			{
				pVerteicePosition[i] = pVers[i].p;
				pVerticeDiffuse[i]   = pVers[i].diffuse;
				pVerticeTextCoord1[i].x = pVers[i].tu1;
				pVerticeTextCoord1[i].y = pVers[i].tv1;
			}
			for(DWORD i=0;i<dwNumFaces;i++)
			{
				pFacesIndex[i*3  ] = pIndex[i*3  ];
				pFacesIndex[i*3+1] = pIndex[i*3+1];
				pFacesIndex[i*3+2] = pIndex[i*3+2];
				pSubsetID[i] = pAttrib[i];
			}
			break;
		}
	case (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX2):
		{
			VFormat::FACES_DIFFUSE_TEXTURE2 * pVers = NULL;
			hr = m_ppMeshes[SMBT_NORMAL]->LockVertexBuffer(0, (void**) &pVers);
			KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

			hr = m_ppMeshes[SMBT_NORMAL]->LockIndexBuffer (0, (void**) &pIndex);
			KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

			hr = m_ppMeshes[SMBT_NORMAL]->LockAttributeBuffer(0, (DWORD**) &pAttrib);
			KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

			pVerteicePosition = new D3DXVECTOR3[dwNumVertices];
			KG_PROCESS_ERROR(pVerteicePosition);
			ZeroMemory(pVerteicePosition, sizeof(D3DXVECTOR3)*dwNumVertices);

			pVerticeDiffuse = new D3DCOLOR[dwNumVertices];
			KG_PROCESS_ERROR(pVerticeDiffuse);
			ZeroMemory(pVerticeDiffuse, sizeof(D3DCOLOR)*dwNumVertices);

			pVerticeTextCoord1 = new D3DXVECTOR3[dwNumVertices];
			KG_PROCESS_ERROR(pVerticeTextCoord1);
			ZeroMemory(pVerticeTextCoord1, sizeof(D3DXVECTOR3)*dwNumVertices);

			pVerticeTextCoord2 = new D3DXVECTOR3[dwNumVertices];
			KG_PROCESS_ERROR(pVerticeTextCoord2);
			ZeroMemory(pVerticeTextCoord2, sizeof(D3DXVECTOR3)*dwNumVertices);

			pFacesIndex = new DWORD[dwNumFaces*3];
			KG_PROCESS_ERROR(pFacesIndex);
			ZeroMemory(pFacesIndex, sizeof(DWORD)*dwNumFaces*3);

			pSubsetID = new DWORD[dwNumFaces];
			KG_PROCESS_ERROR(pSubsetID);
			ZeroMemory(pSubsetID, sizeof(DWORD)*dwNumFaces);

			for(DWORD i=0;i<dwNumVertices;i++)
			{
				pVerteicePosition[i] = pVers[i].p;
				pVerticeDiffuse[i]   = pVers[i].diffuse;
				pVerticeTextCoord1[i].x = pVers[i].tu1;
				pVerticeTextCoord1[i].y = pVers[i].tv1;
				pVerticeTextCoord2[i].x = pVers[i].tu2;
				pVerticeTextCoord2[i].y = pVers[i].tv2;
			}
			for(DWORD i=0;i<dwNumFaces;i++)
			{
				pFacesIndex[i*3  ] = pIndex[i*3  ];
				pFacesIndex[i*3+1] = pIndex[i*3+1];
				pFacesIndex[i*3+2] = pIndex[i*3+2];
				pSubsetID[i] = pAttrib[i];
			}
			break;
		}
	case (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX3):
		{
			VFormat::FACES_DIFFUSE_TEXTURE3 * pVers = NULL;
			hr = m_ppMeshes[SMBT_NORMAL]->LockVertexBuffer(0, (void**) &pVers);
			KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

			hr = m_ppMeshes[SMBT_NORMAL]->LockIndexBuffer (0, (void**) &pIndex);
			KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

			hr = m_ppMeshes[SMBT_NORMAL]->LockAttributeBuffer(0, (DWORD**) &pAttrib);
			KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

			pVerteicePosition = new D3DXVECTOR3[dwNumVertices];
			KG_PROCESS_ERROR(pVerteicePosition);
			ZeroMemory(pVerteicePosition, sizeof(D3DXVECTOR3)*dwNumVertices);

			pVerticeDiffuse = new D3DCOLOR[dwNumVertices];
			KG_PROCESS_ERROR(pVerticeDiffuse);
			ZeroMemory(pVerticeDiffuse, sizeof(D3DCOLOR)*dwNumVertices);

			pVerticeTextCoord1 = new D3DXVECTOR3[dwNumVertices];
			KG_PROCESS_ERROR(pVerticeTextCoord1);
			ZeroMemory(pVerticeTextCoord1, sizeof(D3DXVECTOR3)*dwNumVertices);

			pVerticeTextCoord2 = new D3DXVECTOR3[dwNumVertices];
			KG_PROCESS_ERROR(pVerticeTextCoord2);
			ZeroMemory(pVerticeTextCoord2, sizeof(D3DXVECTOR3)*dwNumVertices);

			pVerticeTextCoord3 = new D3DXVECTOR3[dwNumVertices];
			KG_PROCESS_ERROR(pVerticeTextCoord3);
			ZeroMemory(pVerticeTextCoord3, sizeof(D3DXVECTOR3)*dwNumVertices);

			pFacesIndex = new DWORD[dwNumFaces*3];
			KG_PROCESS_ERROR(pFacesIndex);
			ZeroMemory(pFacesIndex, sizeof(DWORD)*dwNumFaces*3);

			pSubsetID = new DWORD[dwNumFaces];
			KG_PROCESS_ERROR(pSubsetID);
			ZeroMemory(pSubsetID, sizeof(DWORD)*dwNumFaces);

			for(DWORD i=0;i<dwNumVertices;i++)
			{
				pVerteicePosition[i] = pVers[i].p;
				pVerticeDiffuse[i]   = pVers[i].diffuse;
				pVerticeTextCoord1[i].x = pVers[i].tu1;
				pVerticeTextCoord1[i].y = pVers[i].tv1;
				pVerticeTextCoord2[i].x = pVers[i].tu2;
				pVerticeTextCoord2[i].y = pVers[i].tv2;
				pVerticeTextCoord3[i].x = pVers[i].tu3;
				pVerticeTextCoord3[i].y = pVers[i].tv3;
			}
			for(DWORD i=0;i<dwNumFaces;i++)
			{
				pFacesIndex[i * 3]     = pIndex[i * 3];
				pFacesIndex[i * 3 + 1] = pIndex[i * 3 + 1];
				pFacesIndex[i * 3 + 2] = pIndex[i * 3 + 2];
				pSubsetID[i] = pAttrib[i];
			}
			break;
		}
	case (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX1):
		{
			VFormat::FACES_NORMAL_DIFFUSE_TEXTURE1 * pVers = NULL;
			hr = m_ppMeshes[SMBT_NORMAL]->LockVertexBuffer(0, (void**) &pVers);
			KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

			hr = m_ppMeshes[SMBT_NORMAL]->LockIndexBuffer (0, (void**) &pIndex);
			KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

			hr = m_ppMeshes[SMBT_NORMAL]->LockAttributeBuffer(0, (DWORD**) &pAttrib);
			KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

			pVerteicePosition = new D3DXVECTOR3[dwNumVertices];
			KG_PROCESS_ERROR(pVerteicePosition);
			ZeroMemory(pVerteicePosition, sizeof(D3DXVECTOR3)*dwNumVertices);

			pVerticeNormal = new D3DXVECTOR3[dwNumVertices];
			KG_PROCESS_ERROR(pVerticeNormal);
			ZeroMemory(pVerticeNormal, sizeof(D3DXVECTOR3)*dwNumVertices);

			pVerticeDiffuse = new D3DCOLOR[dwNumVertices];
			KG_PROCESS_ERROR(pVerticeDiffuse);
			ZeroMemory(pVerticeDiffuse, sizeof(D3DCOLOR)*dwNumVertices);

			pVerticeTextCoord1 = new D3DXVECTOR3[dwNumVertices];
			KG_PROCESS_ERROR(pVerticeTextCoord1);
			ZeroMemory(pVerticeTextCoord1, sizeof(D3DXVECTOR3) * dwNumVertices);

			pFacesIndex = new DWORD[dwNumFaces * 3];
			KG_PROCESS_ERROR(pFacesIndex);
			ZeroMemory(pFacesIndex, sizeof(DWORD) * dwNumFaces * 3);

			pSubsetID = new DWORD[dwNumFaces];
			KG_PROCESS_ERROR(pSubsetID);
			ZeroMemory(pSubsetID, sizeof(DWORD)*dwNumFaces);

			for(DWORD i=0;i<dwNumVertices;i++)
			{
				pVerteicePosition[i] = pVers[i].p;
				pVerticeDiffuse[i]   = pVers[i].diffuse;
				pVerticeNormal[i]   = pVers[i].Normal;
				pVerticeTextCoord1[i].x = pVers[i].tu1;
				pVerticeTextCoord1[i].y = pVers[i].tv1;
			}
			for(DWORD i=0;i<dwNumFaces;i++)
			{
				pFacesIndex[i*3  ] = pIndex[i*3  ];
				pFacesIndex[i*3+1] = pIndex[i*3+1];
				pFacesIndex[i*3+2] = pIndex[i*3+2];
				pSubsetID[i] = pAttrib[i];
			}
			break;
		}
	case (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX2):
		{
			VFormat::FACES_NORMAL_DIFFUSE_TEXTURE2 * pVers = NULL;
			hr = m_ppMeshes[SMBT_NORMAL]->LockVertexBuffer(0, (void**) &pVers);
			KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

			hr = m_ppMeshes[SMBT_NORMAL]->LockIndexBuffer (0, (void**) &pIndex);
			KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

			hr = m_ppMeshes[SMBT_NORMAL]->LockAttributeBuffer(0, (DWORD**) &pAttrib);
			KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

			pVerteicePosition = new D3DXVECTOR3[dwNumVertices];
			KG_PROCESS_ERROR(pVerteicePosition);
			ZeroMemory(pVerteicePosition, sizeof(D3DXVECTOR3)*dwNumVertices);

			pVerticeNormal = new D3DXVECTOR3[dwNumVertices];
			KG_PROCESS_ERROR(pVerticeNormal);
			ZeroMemory(pVerticeNormal, sizeof(D3DXVECTOR3)*dwNumVertices);

			pVerticeDiffuse = new D3DCOLOR[dwNumVertices];
			KG_PROCESS_ERROR(pVerticeDiffuse);
			ZeroMemory(pVerticeDiffuse, sizeof(D3DCOLOR)*dwNumVertices);

			pVerticeTextCoord1 = new D3DXVECTOR3[dwNumVertices];
			KG_PROCESS_ERROR(pVerticeTextCoord1);
			ZeroMemory(pVerticeTextCoord1, sizeof(D3DXVECTOR3)*dwNumVertices);

			pVerticeTextCoord2 = new D3DXVECTOR3[dwNumVertices];
			KG_PROCESS_ERROR(pVerticeTextCoord2);
			ZeroMemory(pVerticeTextCoord2, sizeof(D3DXVECTOR3)*dwNumVertices);

			pFacesIndex = new DWORD[dwNumFaces*3];
			KG_PROCESS_ERROR(pFacesIndex);
			ZeroMemory(pFacesIndex, sizeof(DWORD)*dwNumFaces*3);

			pSubsetID = new DWORD[dwNumFaces];
			KG_PROCESS_ERROR(pSubsetID);
			ZeroMemory(pSubsetID, sizeof(DWORD)*dwNumFaces);

			for(DWORD i=0;i<dwNumVertices;i++)
			{
				pVerteicePosition[i] = pVers[i].p;
				pVerticeDiffuse[i]   = pVers[i].diffuse;
				pVerticeNormal[i]   = pVers[i].Normal;
				pVerticeTextCoord1[i].x = pVers[i].tu1;
				pVerticeTextCoord1[i].y = pVers[i].tv1;
				pVerticeTextCoord2[i].x = pVers[i].tu2;
				pVerticeTextCoord2[i].y = pVers[i].tv2;
			}
			for(DWORD i=0;i<dwNumFaces;i++)
			{
				pFacesIndex[i*3  ] = pIndex[i*3  ];
				pFacesIndex[i*3+1] = pIndex[i*3+1];
				pFacesIndex[i*3+2] = pIndex[i*3+2];
				pSubsetID[i] = pAttrib[i];
			}
			break;
		}
	case (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX3):
		{
			VFormat::FACES_NORMAL_DIFFUSE_TEXTURE3 * pVers = NULL;
			hr = m_ppMeshes[SMBT_NORMAL]->LockVertexBuffer(0, (void**) &pVers);
			KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

			hr = m_ppMeshes[SMBT_NORMAL]->LockIndexBuffer (0, (void**) &pIndex);
			KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

			hr = m_ppMeshes[SMBT_NORMAL]->LockAttributeBuffer(0, (DWORD**) &pAttrib);
			KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

			pVerteicePosition = new D3DXVECTOR3[dwNumVertices];
			KG_PROCESS_ERROR(pVerteicePosition);
			ZeroMemory(pVerteicePosition, sizeof(D3DXVECTOR3)*dwNumVertices);

			pVerticeNormal = new D3DXVECTOR3[dwNumVertices];
			KG_PROCESS_ERROR(pVerticeNormal);
			ZeroMemory(pVerticeNormal, sizeof(D3DXVECTOR3)*dwNumVertices);

			pVerticeDiffuse = new D3DCOLOR[dwNumVertices];
			KG_PROCESS_ERROR(pVerticeDiffuse);
			ZeroMemory(pVerticeDiffuse, sizeof(D3DCOLOR)*dwNumVertices);

			pVerticeTextCoord1 = new D3DXVECTOR3[dwNumVertices];
			KG_PROCESS_ERROR(pVerticeTextCoord1);
			ZeroMemory(pVerticeTextCoord1, sizeof(D3DXVECTOR3)*dwNumVertices);

			pVerticeTextCoord2 = new D3DXVECTOR3[dwNumVertices];
			KG_PROCESS_ERROR(pVerticeTextCoord2);
			ZeroMemory(pVerticeTextCoord2, sizeof(D3DXVECTOR3)*dwNumVertices);

			pVerticeTextCoord3 = new D3DXVECTOR3[dwNumVertices];
			KG_PROCESS_ERROR(pVerticeTextCoord3);
			ZeroMemory(pVerticeTextCoord3, sizeof(D3DXVECTOR3)*dwNumVertices);

			pFacesIndex = new DWORD[dwNumFaces*3];
			KG_PROCESS_ERROR(pFacesIndex);
			ZeroMemory(pFacesIndex, sizeof(DWORD)*dwNumFaces*3);

			pSubsetID = new DWORD[dwNumFaces];
			KG_PROCESS_ERROR(pSubsetID);
			ZeroMemory(pSubsetID, sizeof(DWORD)*dwNumFaces);

			for(DWORD i=0;i<dwNumVertices;i++)
			{
				pVerteicePosition[i] = pVers[i].p;
				pVerticeDiffuse[i]   = pVers[i].diffuse;
				pVerticeNormal[i]   = pVers[i].Normal;
				pVerticeTextCoord1[i].x = pVers[i].tu1;
				pVerticeTextCoord1[i].y = pVers[i].tv1;
				pVerticeTextCoord2[i].x = pVers[i].tu2;
				pVerticeTextCoord2[i].y = pVers[i].tv2;
				pVerticeTextCoord3[i].x = pVers[i].tu3;
				pVerticeTextCoord3[i].y = pVers[i].tv3;
			}
			for(DWORD i=0;i<dwNumFaces;i++)
			{
				pFacesIndex[i*3  ] = pIndex[i*3  ];
				pFacesIndex[i*3+1] = pIndex[i*3+1];
				pFacesIndex[i*3+2] = pIndex[i*3+2];
				pSubsetID[i] = pAttrib[i];
			}
			break;
		}
	}


	//////////////////////////////////////////////////////////////////////////

	if ((FVF &D3DFVF_XYZ) && (pVerteicePosition))
	{
		MeshHead.Blocks.PositionBlock = ftell(fpFile) - Pos_Start;

		fwrite(pVerteicePosition, sizeof(D3DXVECTOR3), dwNumVertices, fpFile); //顶点位置坐标
		fwrite(&BlockEnd, sizeof(DWORD), 1, fpFile);
	}

	if ((FVF &D3DFVF_NORMAL) && (pVerticeNormal))
	{
		MeshHead.Blocks.NormalBlock = ftell(fpFile) - Pos_Start;

		fwrite(pVerticeNormal, sizeof(D3DXVECTOR3), dwNumVertices, fpFile); //顶点位置坐标
		fwrite(&BlockEnd, sizeof(DWORD), 1, fpFile);
	}

	if ((FVF &D3DFVF_DIFFUSE) && (pVerticeDiffuse))
	{
		MeshHead.Blocks.DiffuseBlock = ftell(fpFile) - Pos_Start;

		fwrite(pVerticeDiffuse, sizeof(D3DCOLOR), dwNumVertices, fpFile); //顶点位置坐标
		fwrite(&BlockEnd, sizeof(DWORD), 1, fpFile);
	}

	if (((FVF &D3DFVF_TEX1) || (FVF &D3DFVF_TEX2) || (FVF &D3DFVF_TEX3)) && (pVerticeTextCoord1))
	{
		MeshHead.Blocks.TextureUVW1Block = ftell(fpFile) - Pos_Start;

		fwrite(pVerticeTextCoord1, sizeof(D3DXVECTOR3), dwNumVertices, fpFile); //顶点位置坐标
		fwrite(&BlockEnd, sizeof(DWORD), 1, fpFile);
	}

	if (((FVF &D3DFVF_TEX2) || (FVF &D3DFVF_TEX3)) && (pVerticeTextCoord2))
	{
		MeshHead.Blocks.TextureUVW2Block = ftell(fpFile) - Pos_Start;

		fwrite(pVerticeTextCoord2, sizeof(D3DXVECTOR3), dwNumVertices, fpFile); //顶点位置坐标
		fwrite(&BlockEnd, sizeof(DWORD), 1, fpFile);
	}

	if ((FVF &D3DFVF_TEX3) && (pVerticeTextCoord3))
	{
		MeshHead.Blocks.TextureUVW3Block = ftell(fpFile) - Pos_Start;

		fwrite(pVerticeTextCoord3, sizeof(D3DXVECTOR3), dwNumVertices, fpFile); //顶点位置坐标
		fwrite(&BlockEnd, sizeof(DWORD), 1, fpFile);
	}

	if (pFacesIndex)
	{
		MeshHead.Blocks.FacesIndexBlock = ftell(fpFile) - Pos_Start;

		fwrite(pFacesIndex, sizeof(DWORD), dwNumFaces*3, fpFile);//面索引
		fwrite(&BlockEnd, sizeof(DWORD), 1, fpFile);
	}

	if (pSubsetID)
	{
		MeshHead.Blocks.SubsetIndexBlock = ftell(fpFile) - Pos_Start;

		fwrite(pSubsetID, sizeof(DWORD), dwNumFaces, fpFile);//面索引
		fwrite(&BlockEnd, sizeof(DWORD), 1, fpFile);
	}

	if (bSkined)
	{
		MeshHead.Blocks.SkinInfoBlock = ftell(fpFile) - Pos_Start;
		//Bone Info
		fwrite(&m_dwNumBone, sizeof(DWORD), 1, fpFile);
		for(DWORD dwBoneIndex=0;dwBoneIndex<m_dwNumBone;dwBoneIndex++)
		{
			BoneInfo1& Info = m_BoneInfo[dwBoneIndex];
			
			fwrite(Info.strBoneName, sizeof(TCHAR), MAX_BONE_NAME_LENGTH, fpFile);
			if (Info.dwParentIndex == -1)
			{
				fwrite(s_strSceneRoot, sizeof(TCHAR), MAX_BONE_NAME_LENGTH, fpFile);
			}
			else
			{
				fwrite(m_BoneInfo[Info.dwParentIndex].strBoneName, sizeof(TCHAR), MAX_BONE_NAME_LENGTH, fpFile);
			}
			fwrite(&Info.dwNumChild, sizeof(DWORD), 1, fpFile);

			for(DWORD i_Child=0;i_Child < Info.dwNumChild; i_Child++)
			{
				//Here need to check if scName length is less than 30
				fwrite(m_BoneInfo[Info.dwChildIndex[i_Child]].strBoneName,
					sizeof(TCHAR), 
					MAX_BONE_NAME_LENGTH, 
					fpFile);
			}

			D3DXMATRIX matOffset;
			D3DXMatrixInverse(&matOffset, NULL, &Info.CurMatrix);
			fwrite(&matOffset, sizeof(D3DMATRIX), 1, fpFile);
			fwrite(&matOffset, sizeof(D3DMATRIX), 1, fpFile);//不是问题， 冗余数据

			fwrite(&Info.NumVerticesRef, sizeof(DWORD), 1, fpFile);
			fwrite(Info.VerticesIndexRef, sizeof(DWORD), Info.NumVerticesRef, fpFile);
			fwrite(Info.VerticesWeightRef, sizeof(float), Info.NumVerticesRef, fpFile);
			//(BoneBox
			if (m_dwOptionEx & MESH_SAVE_BONEBBOX)
			{
				fwrite(&Info.BBoxScaleMatrix, sizeof(D3DMATRIX), 1, fpFile);//BoneBox's ScaleMatrix
				fwrite(&Info.bBox.A, sizeof(D3DXVECTOR3), 1, fpFile);
				fwrite(&Info.bBox.B, sizeof(D3DXVECTOR3), 1, fpFile);
				fwrite(&Info.bBox.bInit, sizeof(BOOL), 1, fpFile);
			}	
		}


		fwrite(&m_dwNumSocket, sizeof(DWORD), 1, fpFile);
		for(DWORD i_Socket = 0; i_Socket < m_dwNumSocket;i_Socket++)
		{
			Socket1 &Socket = m_Sockets[i_Socket];
			fwrite(Socket.strSocketName, sizeof(TCHAR), MAX_BONE_NAME_LENGTH, fpFile);
			fwrite(m_BoneInfo[Socket.dwParentBoneIndex].strBoneName,
				sizeof(TCHAR), 
				MAX_BONE_NAME_LENGTH, 
				fpFile);
			fwrite(&Socket.matMatrixOffset, sizeof(D3DMATRIX), 1, fpFile);
		}
		fwrite(&BlockEnd, sizeof(DWORD), 1, fpFile);
	}
	/////////////////////////////////////////////////////
	MeshFileHead.dwBlockLength = ftell(fpFile) - Pos_Start;
	fseek(fpFile, Pos_Start, SEEK_SET);
	fwrite(&MeshFileHead, sizeof(_MeshFileHead), 1, fpFile);
	fwrite(&MeshHead, sizeof(_MeshHead), 1, fpFile);

	hrResult = S_OK;
Exit0:

	if (m_ppMeshes[SMBT_NORMAL])
	{
		m_ppMeshes[SMBT_NORMAL]->UnlockVertexBuffer();
		m_ppMeshes[SMBT_NORMAL]->UnlockIndexBuffer();
		m_ppMeshes[SMBT_NORMAL]->UnlockAttributeBuffer();
	}

	SAFE_DELETE_ARRAY(pVerteicePosition);
	SAFE_DELETE_ARRAY(pVerticeNormal);
	SAFE_DELETE_ARRAY(pVerticeDiffuse);
	SAFE_DELETE_ARRAY(pVerticeTextCoord1);
	SAFE_DELETE_ARRAY(pVerticeTextCoord2);
	SAFE_DELETE_ARRAY(pVerticeTextCoord3);
	SAFE_DELETE_ARRAY(pFacesIndex);
	SAFE_DELETE_ARRAY(pSubsetID);

	if (uOption == MODEL_SAVEOPT_SPEEDTREE)
	{
		m_dwOption &= (~MESH_SKINED);
	}

	if(fpFile)
	{
		fclose(fpFile);
		fpFile = NULL;
	}
	return hrResult;
}


HRESULT KG3DMesh::PrepareRender(unsigned uOption)//压缩数据
{
	return S_OK;
}

HRESULT KG3DMesh::PostRender()//恢复数据，对象可以立即被使用
{
	return S_OK;
}

HRESULT KG3DMesh::GetDateInformation(int* pnMem, int* pnVedioMem, int* pnConMem, int* pnConVMem)//得到数据信息，参数依次为占用内存大小，现存大小，压缩后内存大小，压缩后内存大小
{
	return S_OK;
}

unsigned   KG3DMesh::GetLastUseTime()//查询上次使用的时间
{
	return 0;
}

unsigned   KG3DMesh::GetUseFrequency()//查询使用频率，一段时间里的使用次数
{
	return 0;
}


//////////////////////////////////////////////////////////////////////////
//骨骼相关
HRESULT	KG3DMesh::SetBoneIndex(WORD* pIndex)
{
	return S_OK;
}

int KG3DMesh::FindBone(LPCSTR strBoneName)
{
	int nReturn = -1;
	BoneInfo1* pInfo = NULL;
	KG_PROCESS_ERROR(strBoneName);
	for(DWORD dwBoneIndex = 0;dwBoneIndex < m_dwNumBone; dwBoneIndex++)
	{
		pInfo = &m_BoneInfo[dwBoneIndex];
		if (!stricmp(m_BoneInfo[dwBoneIndex].strBoneName, strBoneName))
		{
			nReturn = static_cast<int>(dwBoneIndex);
			break;
		}
	}
Exit0:
	return nReturn;
}

//////////////////////////////////////////////////////////////////////////
//插槽相关
HRESULT KG3DMesh::FindSocket(LPCTSTR strSocketName, INT *pnIndex)
{
	int nReturn = -1;
	Socket1* pSocket = NULL;
	KG_PROCESS_ERROR(strSocketName);
	for(DWORD dwSocketIndex = 0;dwSocketIndex < m_dwNumSocket; dwSocketIndex++)
	{
		pSocket = &m_Sockets[dwSocketIndex];
		if (!stricmp(strSocketName, pSocket->strSocketName))
		{
			nReturn = static_cast<int>(dwSocketIndex);
			break;
		}
	}
    *pnIndex = nReturn;
Exit0:
	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
//绘制
HRESULT KG3DMesh::RenderWithoutStateChange(float fAlpha)
{
	return S_OK;
}

HRESULT KG3DMesh::RenderSelfShadow(LPD3DXEFFECT &lpEffect,
                                   const D3DXMATRIX &matVP,const D3DXMATRIX &matWVP,LPDIRECT3DTEXTURE9 lpDepthMap)
{
    LPD3DXMESH pMeshToRender = NULL;

    if (m_ppMeshes[SMBT_OPTIMIZED] && (m_dwOption & MESH_OPTIMIZED)) 
    {
        pMeshToRender = m_ppMeshes[SMBT_OPTIMIZED];
    }
    else
    {
        pMeshToRender = m_ppMeshes[SMBT_NORMAL];
    }
	ASSERT(pMeshToRender);
    
    if(pMeshToRender)
	{
		for(DWORD i = 0; i < m_dwNumSubset; i++)
		{
			pMeshToRender->DrawSubset(i); 
		}
	}
	return S_OK;
}


HRESULT KG3DMesh::RenderShadowMap(LPD3DXEFFECT &lpEffect,KG3DMaterial* pMaterial,const D3DXMATRIX &matVP,const D3DXMATRIX &matWVP)
{
    HRESULT hr = E_FAIL;
    LPD3DXMESH pMeshToRender = NULL;

	if(m_dwOption & MESH_SKINED)
	{
		//渲染阴影时候不处理骨骼动画
        pMeshToRender = m_ppMeshes[SMBT_NORMAL];
	}
	else if (m_ppMeshes[SMBT_OPTIMIZED] && (m_dwOption & MESH_OPTIMIZED)) 
	{
		pMeshToRender = m_ppMeshes[SMBT_OPTIMIZED];
	}
	else
	{
		pMeshToRender = m_ppMeshes[SMBT_NORMAL];
	}

	if(!pMeshToRender)
		return E_FAIL;

    if(pMaterial)
    {
        KG3DMaterial::KG3DMaterialSubset* pCurSubMtl = NULL;

        for(DWORD i = 0; i < m_dwNumSubset; i++)
        {
            if(i >= pMaterial->GetNumMaterial())
            {
                i = pMaterial->GetNumMaterial() - 1;
            }
            pCurSubMtl = pMaterial->GetSubMaterial(i);
            if(pCurSubMtl)
            {
				KG3DTexture* pTexture = pCurSubMtl->GetSubMeshTexture();
                if(pTexture)
                    hr = lpEffect->SetTexture("EntityTexture",pTexture->GetTexture());      
                hr = lpEffect->CommitChanges();

                pMeshToRender->DrawSubset(i);
            }
        }
    }

    return S_OK;
}
 

BOOL KG3DMesh::IsSkin()
{
	return m_dwOption & MESH_SKINED;
}

HRESULT KG3DMesh::GetRenderMatrix(LPD3DXMESH& pMeshToRender,D3DXMATRIX*& pRenderMatrices,void* pExtInfo)
{
	if (pExtInfo)
	{
		MeshRenderExtraInfo* pRenderExt = static_cast<MeshRenderExtraInfo*>(pExtInfo);
		switch(pRenderExt->dwType)
		{
		case ENUM_MESHRENDER_EXTR_INFO_MESH:
			{
				//如果有顶点动画, 使用顶点动画之后的模型
				pMeshToRender = static_cast<LPD3DXMESH>(pRenderExt->Data);
				break;
			}
		case ENUM_MESHRENDER_EXTR_INFO_MATRIX:
			{
				pRenderMatrices = static_cast<D3DXMATRIX*>(pRenderExt->Data);
				break;
			}
		default:
			assert(0);
		}
	}
	return S_OK;
}

HRESULT KG3DMesh::Render(KG3DMaterial* pMaterial, 
						 float fAlpha, 
						 DWORD dwOption, 
						 void* pExtInfo,
						 BOOL bUseDetail)
{
	HRESULT hr = E_FAIL;

	if(m_eDynamicLoadState != STATE_VIDEOMEM)
	{
		return S_OK;
	}

	KG3DRenderState RenderState;
	LPD3DXMESH pMeshToRender = NULL;
	D3DXMATRIX* pRenderMatrices = NULL;
	
	KG_PROCESS_ERROR(pMaterial && pMaterial->IsLoaded());

	if (m_ppMeshes[SMBT_OPTIMIZED] && (m_dwOption & MESH_OPTIMIZED)) 
	{
		//使用Optimize Mesh
		pMeshToRender = m_ppMeshes[SMBT_OPTIMIZED];
	}
	else
	{
		pMeshToRender = m_ppMeshes[SMBT_NORMAL];
	}

	hr = GetRenderMatrix(pMeshToRender,pRenderMatrices,pExtInfo);
	if (dwOption & RENDER_NO_MATERIAL)
	{
		pMaterial = NULL;
	}

	if (dwOption == RENDER_REFACTION)
	{
		//如果是折射, 不更新动画
		hr = DrawMesh(pMeshToRender, pMaterial, fAlpha, dwOption,bUseDetail);
	}
	else
	{
		if (m_dwOption & MESH_SKINED)  
		{ 
			//带Skin的模型
			hr = Shader_RenderSkinMesh(pRenderMatrices, pMaterial, fAlpha, dwOption);
		}
		else
		{
			//普通的模型
			switch(pMaterial->GetShaderType())
			{
			case KG3DMaterial::MAT_SHADER_TYPE_AMBIENT:
				{
					//hr = Shader_DrawMeshAmbientMap(pMeshToRender, pMaterial, fAlpha, dwOption,bUseDetail);
					hr = DrawMesh(pMeshToRender, pMaterial, fAlpha, dwOption,bUseDetail);
					break;
				}
			case KG3DMaterial::MAT_SHADER_TYPE_NONE:
				{
					hr = DrawMesh(pMeshToRender, pMaterial, fAlpha, dwOption,bUseDetail);
					break;
				}
			case KG3DMaterial::MAT_SHADER_TYPE_NORMAL_NOSKIN:
				{
					hr = Shader_DrawMeshNormalNoSkin(pMeshToRender, pMaterial, fAlpha, dwOption);
					break;
				}
			}
		}
	}
	//DrawBigBox();
Exit0:
	return hr;
}

HRESULT KG3DMesh::OptimizeUnSkinedMesh()
{
	HRESULT hr = E_FAIL;
	LPD3DXMESH pMeshClone = NULL;
	LPD3DXBUFFER pAdjacencyBuffer = NULL;
	LPD3DXMESH pMeshOptimized = NULL;
	DWORD dwNumFaces = 0;
	DWORD dwOption = 0;
	LPD3DXMESH &pMeshNormal = m_ppMeshes[SMBT_NORMAL];
	
	BOOL bComputeTangent = g_cEngineOption.bEnableNormalMap;

	D3DVERTEXELEMENT9 VertexDesc[MAX_FVF_DECL_SIZE];

	KGLOG_PROCESS_ERROR(!(m_dwOption & MESH_SKINED));
	KGLOG_PROCESS_ERROR(pMeshNormal);
	
	dwNumFaces = pMeshNormal->GetNumFaces();
	dwOption = D3DXMESH_MANAGED | (pMeshNormal->GetOptions() & D3DXMESH_32BIT);

	if(bComputeTangent)
	{
		hr = pMeshNormal->CloneMesh(dwOption, g_Decl_NormalMap, g_pd3dDevice, &pMeshClone);
		KGLOG_COM_PROCESS_ERROR(hr);
	}
	else
	{
		pMeshNormal->GetDeclaration(VertexDesc);

		hr = pMeshNormal->CloneMesh(dwOption,VertexDesc, g_pd3dDevice, &pMeshClone);
		KGLOG_COM_PROCESS_ERROR(hr);

	}
		
	//Create adjacency buffer
	hr = D3DXCreateBuffer(dwNumFaces * 3 * sizeof(DWORD), &pAdjacencyBuffer);
	KGLOG_COM_PROCESS_ERROR(hr);

	DWORD *pBuffer = (DWORD*)pAdjacencyBuffer->GetBufferPointer();

	hr = pMeshNormal->GenerateAdjacency(0, pBuffer);//(DWORD*)pAdjacencyBuffer->GetBufferPointer());
	KGLOG_COM_PROCESS_ERROR(hr);

		
	if(bComputeTangent)
	{
#ifdef D3DXTANGENT_GENERATE_IN_PLACE 
		//for sdk oct 2005
		hr = D3DXComputeTangentFrameEx(pMeshClone, 
			D3DDECLUSAGE_TEXCOORD, 
			0, 
			D3DDECLUSAGE_BINORMAL,
			0, 
			D3DDECLUSAGE_TANGENT,
			0,
			D3DX_DEFAULT,
			0, 
			D3DXTANGENT_GENERATE_IN_PLACE, 
			NULL, 
			-1.01f, 
			-0.01f,
			-1.01f, 
			NULL,
			NULL);
#else
		//for sdk version before oct 2005, buggy
		hr = D3DXComputeTangent(pMeshClone, 0, 0, 0, 0, NULL);
#endif
		KGLOG_COM_PROCESS_ERROR(hr);

	}
	
	hr = pMeshClone->Optimize(D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_VERTEXCACHE | D3DXMESH_WRITEONLY , 
		                     (DWORD*)pAdjacencyBuffer->GetBufferPointer(), 
		                      NULL,
							  NULL,
							  &m_pVertexRemap,
							  &pMeshOptimized);
	if(FAILED(hr))
	{
		hr = pMeshClone->Optimize(D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_VERTEXCACHE , 
			(DWORD*)pAdjacencyBuffer->GetBufferPointer(), 
			NULL,
			NULL,
			&m_pVertexRemap,
			&pMeshOptimized);

		/*if(FAILED(hr))
		{
			hr = pMeshClone->Optimize(D3DXMESHOPT_ATTRSORT , 
				(DWORD*)pAdjacencyBuffer->GetBufferPointer(), 
				NULL,
				NULL,
				&m_pVertexRemap,
				&pMeshOptimized);

		}*/
	}
	KGLOG_COM_PROCESS_ERROR(hr);

	SAFE_RELEASE(m_ppMeshes[SMBT_OPTIMIZED]);
	m_ppMeshes[SMBT_OPTIMIZED] = pMeshOptimized;
	m_dwOption |= MESH_OPTIMIZED;

Exit0:

	SAFE_RELEASE(pMeshClone);
	SAFE_RELEASE(pAdjacencyBuffer);
	return hr;
}

HRESULT KG3DMesh::CreateSkinMesh()
{
	HRESULT hrRetCode = E_FAIL;
	HRESULT hrResult = E_FAIL;
	DWORD dwStartTime = timeGetTime();

	hrRetCode = Helper_CreateSkinMesh();
	KG_COM_PROCESS_ERROR(hrRetCode);

	hrRetCode = SkinMeshPostProcess();
	KG_COM_PROCESS_ERROR(hrRetCode);
	
	hrResult = S_OK;
Exit0:
	DWORD dwCost = timeGetTime() - dwStartTime;
	if(g_cEngineOption.bEnableTimeOptimizeLog && dwCost > 10)
	{
		KGLogPrintf(KGLOG_ERR, "TimeOptimize %d KG3DMesh::CreateSkinMesh  %s.",dwCost , m_scName.c_str());
	}
	return hrResult;
}

HRESULT KG3DMesh::Shader_DrawMesh_Begin(LPD3DXMESH pMesh,
										float fAlpha,
										DWORD dwOption,BOOL bUseDetail)
{
	HRESULT hrResult = E_FAIL;
	LPD3DXEFFECT pShader = NULL;

	if(bUseDetail)
	{
		pShader = g_cGraphicsTool.GetDefaultShader(DEFST_NEWLIGHT_DETAIL);
	}
	else
	{
		pShader = g_cGraphicsTool.GetDefaultShader(DEFST_NEWLIGHT);
	}
	KG_PROCESS_ERROR(pShader);

	hrResult = Shader_DrawMeshSubset_Begin(0, NULL, fAlpha, dwOption, pShader ,bUseDetail);
	
Exit0:
	return hrResult;
	
}

HRESULT KG3DMesh::Shader_DrawMeshSubset_Begin(int subsetid,
										KG3DMaterial::KG3DMaterialSubset* pSubMat, 
										float fAlpha,
										DWORD dwOption,
										LPD3DXEFFECT pShader,BOOL bUseDetail)
{
	HRESULT hrResult = E_FAIL;
	HRESULT hrRetCode = E_FAIL;
	UINT uDummy = 0;

	KG_PROCESS_ERROR(pShader);

	//Helper_ShaderSetFogInfo(pShader);

	if(bUseDetail)
	{
		hrRetCode = pShader->SetTechnique("ConverMapTechnique");
		if(FAILED(hrResult))
		{
			int k = 0;
		}
		KGLOG_COM_CHECK_ERROR(hrRetCode);

		/*KG3DTerrainConverMap* pConverMap = NULL;
		pConverMap = g_cGraphicsTool.GetCurrentConverMap();
		if (pConverMap)
		{
			pShader->SetVector("vConverRect",&pConverMap->GetRect());
			pShader->SetTexture("tConver",pConverMap->GetRenderTargetTex());
		}*///<ConverMap Shared>
	}
	else
	{
		KG3DTerrainConverMap* pConverMap = NULL;
		pConverMap = g_cGraphicsTool.GetCurrentConverMap();
		if (pConverMap)
		{
			/*pShader->SetVector("vConverRect",&pConverMap->GetRect());
			pShader->SetTexture("tConver",pConverMap->GetRenderTargetTex());*/

			//<ConverMap Shared>

			hrRetCode = pShader->SetTechnique("ConverMapTechnique");
			KGLOG_COM_CHECK_ERROR(hrRetCode);
		}
		else
		{
			hrRetCode = pShader->SetTechnique("DefaultTechnique");
			KGLOG_COM_CHECK_ERROR(hrRetCode);
		}
	}

	/*pCamera = g_cGraphicsTool.GetCamera();
	if (pCamera)
	{
		D3DXVECTOR3 vEyes;

		pCamera->GetPosition(&vEyes);

		hrRetCode = pShader->SetValue("eyes", &vEyes, sizeof(D3DXVECTOR3));
        KGLOG_COM_CHECK_ERROR(hrRetCode);
	}*/

	pShader->Begin(&uDummy, 0);

	hrResult = S_OK;
Exit0:
	return hrResult;
}


HRESULT KG3DMesh::Shader_DrawMeshSubset_DepthOnly(LPD3DXMESH pMesh,
												 int subsetid,
												 KG3DMaterial::KG3DMaterialSubset* pSubMat)
{
	pMesh->DrawSubset(subsetid);

	return S_OK;
}



HRESULT KG3DMesh::Shader_DrawMeshSubset_NoChange(LPD3DXMESH pMesh,
												 int subsetid,
										   KG3DMaterial::KG3DMaterialSubset* pSubMat, 
										   float fAlpha,
										   DWORD dwOption,
										   LPD3DXEFFECT pShader,
										   D3DXMATRIX* pmatWVP,
										   D3DXMATRIX* pmatWorld,
										   BOOL bUseDetail)
{
	HRESULT hrRetCode = E_FAIL;
	HRESULT hrResult = E_FAIL;
	IEKG3DScene* pScene = NULL;
	D3DXMATRIX matWorld, matView, matProj, matWVP;
	KG3DRuntimeMaterial* pRuntimeMaterial = NULL;

	if(bUseDetail)
	{
		pShader = g_cGraphicsTool.GetDefaultShader(DEFST_NEWLIGHT_DETAIL);
	}
	else
	{
		pShader = g_cGraphicsTool.GetDefaultShader(DEFST_NEWLIGHT);
	}
	KG_PROCESS_ERROR(pShader);
	//g_SetShadowInfo(pShader);//<ShadowParamMoveToShared>

    if (m_pModel)
    {
        g_SetPointLighting(pShader, m_pModel->GetPointLightInfo(), MAX_POINTLIGHT, false);
    }

	pSubMat->SetMaterial(fAlpha, dwOption);
	
	if (m_pModel)
	{
		pRuntimeMaterial = m_pModel->GetRuntimeMaterial();
		KG_PROCESS_ERROR(pRuntimeMaterial);

		pRuntimeMaterial->BeginRender();
	}

	D3DMATERIAL9 mat;
	g_pd3dDevice->GetMaterial(&mat);
	
	
	
	/*hrRetCode = pShader->SetValue("spe_exp",     &mat.Power,      sizeof(float));
	hrRetCode = pShader->SetValue("color_cast",    &(pSubMat->m_ColorCast[m_pModel->GetDefaultColorCastIndex()]),  sizeof(D3DCOLORVALUE));
	hrRetCode = pShader->SetValue("emssive_power", &pSubMat->m_fEmssPower, sizeof(float));
	hrRetCode = pShader->SetBool("bhightlight", pSubMat->m_dwOption & MATERIAL_OPTION_SPECULARENABLE);*/

	//改成一次Set进去
	{
		KG3DShaderParamMaterialEx materialEx;
		materialEx.bhightlight = !!(pSubMat->m_dwOption & MATERIAL_OPTION_SPECULARENABLE);
		materialEx.color_cast = pSubMat->m_ColorCast[m_pModel->GetDefaultColorCastIndex()];
		materialEx.emssive_power = pSubMat->m_fEmssPower;
		materialEx.spe_exp = mat.Power;
		hrRetCode = pShader->SetValue("materialEx", &materialEx, sizeof(materialEx));
	}

	//注意顺序，这里改了mat.Power，上面的没有改
	mat.Power = pSubMat->m_fSpecPower;	
	hrRetCode = pShader->SetValue("materialCur", &mat, sizeof(mat));

	g_pd3dDevice->GetTransform(D3DTS_WORLD, &matWorld);
	g_pd3dDevice->GetTransform(D3DTS_VIEW, &matView);
	g_pd3dDevice->GetTransform(D3DTS_PROJECTION, &matProj);
	matWVP = matWorld * matView * matProj;
	hrRetCode = pShader->SetMatrix("WorldViewProj", &matWVP);
	hrRetCode = pShader->SetMatrix("matWorld", &matWorld);


    if (m_pModel && g_ProjTextureRender.NeedProject(m_pModel->m_BBox) && m_pModel->GetBindType() == ENUM_BIND_NONE)
        g_ProjTextureRender.SetEffectParam(pShader);

	pScene = g_cGraphicsTool.GetScene();
	
	g_pd3dDevice->SetTexture(1, NULL);
	_MtlAlphaOperation* pAlphaOp = pSubMat->GetAlphaOperation();
	BOOL bEnableAlphaFactor = !(pAlphaOp->EnableAlphaTest || pAlphaOp->EnableAlphaBlend);

	_MtlRenderTwiceAlphaOption *pTwiceOption = pSubMat->GetAlphaRenderTwice();
	if (pTwiceOption->bEnable)
	{
		bEnableAlphaFactor = FALSE;
	}

	if(bUseDetail)
	{
		g_SetMaterialDetailInfo(pShader,m_pModel);
	}

	pShader->SetBool("bAlphaUseFactor", bEnableAlphaFactor);
	pShader->CommitChanges();

	pShader->BeginPass(0);
	hrRetCode = pMesh->DrawSubset(subsetid);
	KG_COM_PROCESS_ERROR(hrRetCode);
	hrResult = S_OK;
Exit0:
	if (pShader != NULL)
	{
		pShader->EndPass();
		pSubMat->RestoreMaterial();
        pShader->SetTexture("SelTexture", NULL);
	}
	if (pRuntimeMaterial)
	{
		pRuntimeMaterial->EndRender();
	}
	return hrResult;
}

HRESULT KG3DMesh::Shader_DrawMeshSubset_End(KG3DMaterial::KG3DMaterialSubset* pSubMat, 
											LPD3DXEFFECT pShader)
{
  	if(g_pSubMatLast)
	{
		g_pSubMatLast->RestoreMaterial();
		g_pSubMatLast = NULL;
	}

	if (pShader)
	{
		pShader->End();
        //pShader->SetTexture("tConver", NULL);//<ConverMap Shared>
	}
	return S_OK;
}

HRESULT KG3DMesh::Shader_DrawMesh_NoChange(LPD3DXMESH pMesh,
									  KG3DMaterial* pMaterial, 
									  float fAlpha,
									  DWORD dwOption,BOOL bUseDetail)
{
	LPD3DXEFFECT pShader = NULL;

	HRESULT hr = E_FAIL;
	KG_PROCESS_ERROR(pMesh);

	KG_PROCESS_ERROR(pMaterial);

	for(DWORD i = 0; i < m_dwNumSubset; i++)
	{
		KG3DMaterial::KG3DMaterialSubset* pSubMat = pMaterial->GetSubMaterial(i);
		if(pSubMat)
		{
			Shader_DrawMeshSubset_NoChange(pMesh,i,pSubMat,fAlpha,dwOption,NULL,&m_pModel->m_matR_WorldViewProj,&m_pModel->m_matWorld,bUseDetail);
		}
	}

	hr = S_OK;
Exit0:
	SAFE_RELEASE(pShader);
	return hr;
}



HRESULT KG3DMesh::Shader_DrawMesh_End(LPD3DXMESH pMesh,
										float fAlpha,
										DWORD dwOption,BOOL bUseDetail)
{
	LPD3DXEFFECT pShader = NULL;
	HRESULT hr = E_FAIL;
	KG_PROCESS_ERROR(pMesh);

	if(bUseDetail)
	{
		pShader = g_cGraphicsTool.GetDefaultShader(DEFST_NEWLIGHT_DETAIL);
	}
	else
	{
		pShader = g_cGraphicsTool.GetDefaultShader(DEFST_NEWLIGHT);
	}
	KG_PROCESS_ERROR(pShader);

	if(g_pSubMatLast)
	{
		g_pSubMatLast->RestoreMaterial();
		g_pSubMatLast = NULL;
	}

	pShader->EndPass();
	pShader->End();

	hr = S_OK;
Exit0:
	return hr;
}


HRESULT KG3DMesh::DrawMesh(LPD3DXMESH pMesh,
						   KG3DMaterial* pMaterial, 
						   float fAlpha,
						   DWORD dwOption,BOOL bUseDetail)
{
	HRESULT hrRetCode = E_FAIL;
	HRESULT hrResult = E_FAIL;

	hrRetCode = Shader_DrawMesh_Begin(pMesh, fAlpha, dwOption,bUseDetail);
	KG_COM_PROCESS_ERROR(hrRetCode);


	hrRetCode = Shader_DrawMesh_NoChange(pMesh, pMaterial, fAlpha, dwOption,bUseDetail);
	KG_COM_PROCESS_ERROR(hrRetCode);
	
	hrResult = S_OK;
Exit0:
	Shader_DrawMesh_End(pMesh, fAlpha, dwOption,bUseDetail);
	return hrResult;
}


HRESULT KG3DMesh::GenerateEnhancedMesh(UINT dwNewNumSegs, LPD3DXMESH pMesh)
{
	LPD3DXBUFFER pAdjacencyBuffer = NULL;
	LPD3DXMESH pMeshEnhancedSysMem = NULL;
	LPD3DXMESH pMeshTemp = NULL;
	HRESULT    hr = E_FAIL;
	
	KG_PROCESS_ERROR(pMesh);
	// if using hardware, just copy the mesh
	if(g_cGraphicsEngine.GetD3DCaps().DevCaps & D3DDEVCAPS_NPATCHES)
	{
		hr = pMesh->CloneMeshFVF(D3DXMESH_WRITEONLY | D3DXMESH_NPATCHES | 
			(pMesh->GetOptions() &D3DXMESH_32BIT), 
			pMesh->GetFVF(), g_pd3dDevice, &pMeshTemp);
		KG_COM_PROCESS_ERROR(hr);
	}
	else  
	{
		// tesselate the mesh in software
		// Contains the adjaceny info loaded with the mesh
		hr = D3DXCreateBuffer(pMesh->GetNumFaces() * 3 * sizeof(DWORD), 
			 &pAdjacencyBuffer);
		KG_COM_PROCESS_ERROR(hr);

		hr = pMesh->GenerateAdjacency(0, 
			(DWORD*)pAdjacencyBuffer->GetBufferPointer());
		KG_COM_PROCESS_ERROR(hr);

		// Create an enhanced version of the mesh, will be in sysmem since source is
		hr = D3DXTessellateNPatches(pMesh, 
			(DWORD*)pAdjacencyBuffer->GetBufferPointer(), 
			(float)dwNewNumSegs, 
			FALSE, 
			&pMeshEnhancedSysMem, 
			NULL);
		if(FAILED(hr))
		{
			// If the tessellate failed, there might have been more triangles or vertices 
			// than can fit into a 16bit mesh, so try cloning to 32bit before tessellation
			hr = pMesh->CloneMeshFVF(D3DXMESH_SYSTEMMEM | D3DXMESH_32BIT, 
				pMesh->GetFVF(), 
				g_pd3dDevice, 
				&pMeshTemp);
			KG_COM_PROCESS_ERROR(hr);

			hr = D3DXTessellateNPatches(pMeshTemp, 
				(DWORD*)pAdjacencyBuffer->GetBufferPointer(), 
				(float)dwNewNumSegs,
				FALSE, 
				&pMeshEnhancedSysMem, 
				NULL);
			KG_COM_PROCESS_ERROR(hr);
		}

		// Make a vid mem version of the mesh  
		// Only set WRITEONLY if it doesn't use 32bit indices, because those 
		// often need to be emulated, which means that D3DX needs read-access.
		DWORD dwMeshEnhancedFlags = (pMeshEnhancedSysMem->GetOptions() & D3DXMESH_32BIT);
		if(dwMeshEnhancedFlags == 0)
		{
			dwMeshEnhancedFlags |= D3DXMESH_WRITEONLY;
		}
        SAFE_RELEASE(pMeshTemp);
		hr = pMeshEnhancedSysMem->CloneMeshFVF(dwMeshEnhancedFlags, 
			pMesh->GetFVF(), 
			g_pd3dDevice, 
			&pMeshTemp);
		KG_COM_PROCESS_ERROR(hr);
	}
	SAFE_RELEASE(m_pMeshEnhanced);
	m_pMeshEnhanced = pMeshTemp;
	m_pMeshEnhanced->AddRef();
	//g_dwNumSegs     = dwNewNumSegs;
Exit0:
	SAFE_RELEASE(pMeshTemp);
	SAFE_RELEASE(pAdjacencyBuffer);
	SAFE_RELEASE(pMeshEnhancedSysMem);
	return hr;
}

HRESULT KG3DMesh::CreateProgressMesh()
{
	HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
	LPD3DXBUFFER pAdjacencyBuffer = NULL; 
	LPD3DXMESH pMesh = m_ppMeshes[SMBT_OPTIMIZED];
	LPD3DXMESH pTempMesh = NULL;

	if(m_pMeshProgress)
		return S_OK;//如果已经创建就直接返回

	if (m_ppMeshes[SMBT_BLENDED])
	{
		pMesh = m_ppMeshes[SMBT_BLENDED];
	}

	KG_PROCESS_ERROR(pMesh);

	//GenerateEnhancedMesh(2, pMesh);

	//return S_OK;

	if(!m_pMeshBeforeProgress)
	{
		D3DXWELDEPSILONS Epsilons;
		hr = D3DXCreateBuffer(pMesh->GetNumFaces() * 3 * sizeof(DWORD), 
			 &pAdjacencyBuffer);
		KG_COM_PROCESS_ERROR(hr);
		
		hr = pMesh->GenerateAdjacency(0,
			(DWORD*)pAdjacencyBuffer->GetBufferPointer());
		KG_COM_PROCESS_ERROR(hr);

		hr = D3DXCleanMesh(D3DXCLEAN_SIMPLIFICATION, 
			pMesh, 
			(DWORD*)(pAdjacencyBuffer->GetBufferPointer()), 
			&pTempMesh, 
			(DWORD*)(pAdjacencyBuffer->GetBufferPointer()), 
			NULL);
		KG_COM_PROCESS_ERROR(hr);
		
		//Weld the mesh using all epsilons of 0.0f.  A small epsilon like 1e-6 works well too
		memset(&Epsilons, 0, sizeof(D3DXWELDEPSILONS));
		hr = D3DXWeldVertices(pTempMesh, 
			0, 
			&Epsilons, 
			(DWORD*)pAdjacencyBuffer->GetBufferPointer(), 
			(DWORD*)pAdjacencyBuffer->GetBufferPointer(), 
			NULL, 
			NULL);

		KG_COM_PROCESS_ERROR(hr);
		
		hr = D3DXValidMesh(pTempMesh, 
			(DWORD*)pAdjacencyBuffer->GetBufferPointer(), 
			NULL);
		KG_COM_PROCESS_ERROR(hr);
		
		SAFE_RELEASE(pAdjacencyBuffer);
		SAFE_DELETE_ARRAY(m_pVerticesLodWeight);
		
		SAFE_RELEASE(m_pMeshBeforeProgress);
		m_pMeshBeforeProgress = pTempMesh;
		pTempMesh->AddRef();
		m_pVerticesLodWeight =  new float[m_pMeshBeforeProgress->GetNumVertices()];
		KGLOG_PROCESS_ERROR(m_pVerticesLodWeight);

		for(DWORD i = 0; i < m_pMeshBeforeProgress->GetNumVertices(); i++)
		{
			m_pVerticesLodWeight[i] = 1.0f;
		}
	}

	hr = D3DXCreateBuffer(m_pMeshBeforeProgress->GetNumFaces() * 3 * sizeof(DWORD), 
		 &pAdjacencyBuffer);
	KG_COM_PROCESS_ERROR(hr);

	hr = m_pMeshBeforeProgress->GenerateAdjacency(0, (DWORD*)pAdjacencyBuffer->GetBufferPointer());
	KG_COM_PROCESS_ERROR(hr);

	SAFE_RELEASE(m_pMeshProgress);
	hr = D3DXGeneratePMesh(m_pMeshBeforeProgress, 
		(DWORD*)pAdjacencyBuffer->GetBufferPointer(), 
		NULL, 
		m_pVerticesLodWeight, 
		1, 
		D3DXMESHSIMP_FACE, 
		 &m_pMeshProgress);
	KG_COM_PROCESS_ERROR(hr);

	m_dwOption |= MESH_PROGRESS;

	m_nNumCurFacesLOD = m_pMeshProgress->GetMaxFaces() /2 ;
	m_pMeshProgress->SetNumFaces(m_nNumCurFacesLOD);
	
    hrResult = S_OK;
Exit0:
	SAFE_RELEASE(pTempMesh);
	SAFE_RELEASE(pAdjacencyBuffer);
	if (FAILED(hr))
	{
		SAFE_DELETE_ARRAY(m_pVerticesLodWeight);
	}
	return hrResult;
}

HRESULT KG3DMesh::GetProgressMeshMinFaces(INT *pnMin)
{
    HRESULT hrResult = E_FAIL;
	KGLOG_PROCESS_ERROR(pnMin);
	*pnMin = 0;

	KG_PROCESS_ERROR(m_pMeshProgress);
    *pnMin = m_pMeshProgress->GetMinFaces();
    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KG3DMesh::GetProgressMeshMaxFaces(INT *pnMax)
{
    HRESULT hrResult = E_FAIL;
    KGLOG_PROCESS_ERROR(pnMax);
	*pnMax = 0;
	
	KG_PROCESS_ERROR(m_pMeshProgress);
	*pnMax = m_pMeshProgress->GetMaxFaces();
    hrResult = S_OK;
Exit0:
    return hrResult;

}

HRESULT KG3DMesh::SetNumFaceLOD(INT nNumFace)
{
	m_nNumCurFacesLOD = nNumFace;
	if(m_pMeshProgress)
		m_pMeshProgress->SetNumFaces(nNumFace);
    return S_OK;
}

D3DXMATRIX* KG3DMesh::GetSocketMatrix(int nIndex)
{
	if((DWORD)nIndex < m_dwNumSocket)
	{
		return &m_Sockets[nIndex].matCurMatrix;
	}
	return NULL;
}

D3DXMATRIX* KG3DMesh::GetBoneMatrix(int nBoneIndex)
{
    if ((DWORD)nBoneIndex < m_dwNumBone)
    {
        return &m_BoneInfo[nBoneIndex].CurMatrix;
    }

    return NULL;
}


HRESULT KG3DMesh::DrawNormal()
{
	HRESULT hr = E_FAIL;
	float fNormalLength = 50.0f;
	DWORD dwNumVertices = 0;
	DWORD dwNumFaces = 0;
	DWORD dwStride = 0;
	VFormat::NormalDrawer* pNormals = NULL;
	D3DXVECTOR3* pMeshNormal = NULL;
	BYTE* pMeshVertex = NULL;
	LPD3DXMESH pMesh = NULL;
	short* pIndices = NULL;
	if(m_ppMeshes[SMBT_OPTIMIZED])
    {
        pMesh = m_ppMeshes[SMBT_OPTIMIZED];
    }
	else if(m_ppMeshes[SMBT_NORMAL])
    {
        pMesh = m_ppMeshes[SMBT_NORMAL];
    }
    KG_PROCESS_ERROR(pMesh);

	dwNumVertices = pMesh->GetNumVertices();
	dwNumFaces = pMesh->GetNumFaces();
	if(!m_lpNormal)
	{
		hr = g_pd3dDevice->CreateVertexBuffer(dwNumVertices * sizeof(VFormat::NormalDrawer) * 2, 0, VFormat::NormalDrawer::m_dwFVF, D3DPOOL_MANAGED, &m_lpNormal, 0);
		if(FAILED(hr))
			goto Exit0;
		m_lpNormal->Lock(0, 0, (void**) &pNormals, 0); 
		pMesh->LockVertexBuffer(0, (void**) &pMeshVertex);
		pMesh->LockIndexBuffer(0, (void**) &pIndices);
		dwStride = pMesh->GetNumBytesPerVertex();
		for(int i = 0; i < (int)dwNumVertices; i++)
		{
			pMeshNormal = reinterpret_cast<D3DXVECTOR3*>(pMeshVertex + dwStride * i);
			pNormals[2 * i].p = *pMeshNormal;
			pNormals[2 * i].dwColor = 0xff0000ff;
			pMeshNormal = reinterpret_cast<D3DXVECTOR3*>(pMeshVertex + dwStride * i + 12);
			pNormals[2 * i + 1].p = pNormals[2*i].p + (*pMeshNormal * fNormalLength);
			pNormals[2 * i + 1].dwColor = 0xff0000ff;
		}
		pMesh->UnlockVertexBuffer();
		
		m_lpNormal->Unlock(); 
		pMesh->UnlockIndexBuffer();
		
	}
	g_pd3dDevice->SetFVF(VFormat::NormalDrawer::m_dwFVF);
	g_pd3dDevice->SetStreamSource(0, m_lpNormal, 0, sizeof(VFormat::NormalDrawer));
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	g_pd3dDevice->DrawPrimitive(D3DPT_LINELIST, 0, dwNumVertices);
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	hr = S_OK;
Exit0:
	return hr;
}


HRESULT KG3DMesh::DrawBinormal()
{
	HRESULT hr = E_FAIL;
	float fNormalLength = 50.0f;
	DWORD dwNumVertices = 0;
	DWORD dwStride = 0;
	VFormat::NormalDrawer* pBinormals = NULL;
	D3DXVECTOR3* pMeshBiNormal = NULL;
	BYTE* pMeshVertex = NULL;
	ASSERT(m_ppMeshes[SMBT_OPTIMIZED]);
	dwNumVertices = m_ppMeshes[SMBT_OPTIMIZED]->GetNumVertices();
	if(!m_lpBinormal)
	{
		hr = g_pd3dDevice->CreateVertexBuffer(dwNumVertices * sizeof(VFormat::NormalDrawer) * 2, 0, VFormat::NormalDrawer::m_dwFVF, D3DPOOL_MANAGED, &m_lpBinormal, 0);
		if(FAILED(hr))
			goto Exit0;
		m_lpBinormal->Lock(0, 0, (void**) &pBinormals, 0);
		m_ppMeshes[SMBT_OPTIMIZED]->LockVertexBuffer(0, (void**) &pMeshVertex);
		dwStride = m_ppMeshes[SMBT_OPTIMIZED]->GetNumBytesPerVertex();
		for(int i = 0; i < (int)dwNumVertices; i++)
		{
			pMeshBiNormal = reinterpret_cast<D3DXVECTOR3*>(pMeshVertex + dwStride * i);
			pBinormals[2 * i].p = *pMeshBiNormal;
			pBinormals[2 * i].dwColor = 0xff00ff00;
			pMeshBiNormal = reinterpret_cast<D3DXVECTOR3*>(pMeshVertex + dwStride * i + 52);
			pBinormals[2 * i + 1].p = pBinormals[2*i].p + (*pMeshBiNormal * fNormalLength);
			pBinormals[2 * i + 1].dwColor = 0xff00ff00;
		}
		m_ppMeshes[SMBT_OPTIMIZED]->UnlockVertexBuffer();
		m_lpBinormal->Unlock();
	}
	g_pd3dDevice->SetFVF(VFormat::NormalDrawer::m_dwFVF);
	g_pd3dDevice->SetStreamSource(0, m_lpBinormal, 0, sizeof(VFormat::NormalDrawer));
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	g_pd3dDevice->DrawPrimitive(D3DPT_LINELIST, 0, dwNumVertices);
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	hr = S_OK;
Exit0:
	return hr;
}

HRESULT KG3DMesh::DrawTangent()
{
	HRESULT hr = E_FAIL;
	float fNormalLength = 50.0f;
	DWORD dwNumVertices = 0; 
	DWORD dwStride = 0;
	VFormat::NormalDrawer* pTangents = NULL;
	D3DXVECTOR3* pMeshTangent = NULL;
	BYTE* pMeshVertex = NULL;
	ASSERT(m_ppMeshes[SMBT_OPTIMIZED]);
	dwNumVertices = m_ppMeshes[SMBT_OPTIMIZED]->GetNumVertices();
	if(!m_lpTangent)
	{
		hr = g_pd3dDevice->CreateVertexBuffer(dwNumVertices * sizeof(VFormat::NormalDrawer) * 2, 0, VFormat::NormalDrawer::m_dwFVF, D3DPOOL_MANAGED, &m_lpTangent, 0);
		if(FAILED(hr))
			goto Exit0;
		m_lpTangent->Lock(0, 0, (void**) &pTangents, 0);
		m_ppMeshes[SMBT_OPTIMIZED]->LockVertexBuffer(0, (void**) &pMeshVertex);
		dwStride = m_ppMeshes[SMBT_OPTIMIZED]->GetNumBytesPerVertex();
		for(int i = 0; i < (int)dwNumVertices; i++)
		{
			pMeshTangent = reinterpret_cast<D3DXVECTOR3*>(pMeshVertex + dwStride * i);
			pTangents[2 * i].p = *pMeshTangent;
			pTangents[2 * i].dwColor = 0xffff00ff;
			pMeshTangent = reinterpret_cast<D3DXVECTOR3*>(pMeshVertex + dwStride * i + 40);
			pTangents[2 * i + 1].p = pTangents[2*i].p + (*pMeshTangent * fNormalLength);
			pTangents[2 * i + 1].dwColor = 0xffff0000;
		}
		m_ppMeshes[SMBT_OPTIMIZED]->UnlockVertexBuffer();
		m_lpTangent->Unlock();
	}
	g_pd3dDevice->SetFVF(VFormat::NormalDrawer::m_dwFVF);
	g_pd3dDevice->SetStreamSource(0, m_lpTangent, 0, sizeof(VFormat::NormalDrawer));
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	g_pd3dDevice->DrawPrimitive(D3DPT_LINELIST, 0, dwNumVertices);
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	hr = S_OK;
Exit0:
	return hr;
}

BOOL KG3DMesh::IsVertexInside(D3DXVECTOR3& vVertex, D3DXVECTOR3& vVexDir, KG3DModel* pModel)
{
    int nResult  = false;
    HRESULT hrResult = E_FAIL;


    D3DXVECTOR3 vSrc = vVertex;
    D3DXVECTOR3 vDir = vVexDir;

    BOOL bHit = false;
    DWORD Index = 0;

    LPDIRECT3DINDEXBUFFER9  pIndexBuffer = NULL;
    LPDIRECT3DVERTEXBUFFER9 pVertxBuffer = NULL;
    D3DINDEXBUFFER_DESC     IndexDesc;
   
    KG3DMaterial* pMaterial = NULL;
	KG3DMaterial::KG3DMaterialSubset* pSubMtl = NULL;

    KG_PROCESS_ERROR(m_ppMeshes[SMBT_NORMAL]);
    KG_PROCESS_ERROR(pModel);

    pMaterial = pModel->GetMaterial();
    KG_PROCESS_ERROR(pMaterial && pMaterial->IsLoaded());

	pSubMtl = pMaterial->GetSubMaterial(0);
    KG_PROCESS_ERROR(pSubMtl);

    hrResult = D3DXIntersect(m_ppMeshes[SMBT_NORMAL], &vSrc, &vDir, &bHit, &Index, NULL, NULL, NULL, NULL, NULL);
    KG_COM_PROCESS_ERROR(hrResult);

    KG_PROCESS_ERROR(bHit);

    ASSERT(Index >= 0);

    hrResult = m_ppMeshes[SMBT_NORMAL]->GetIndexBuffer(&pIndexBuffer);
    KG_COM_PROCESS_ERROR(hrResult);

    hrResult = m_ppMeshes[SMBT_NORMAL]->GetVertexBuffer(&pVertxBuffer);
    KG_COM_PROCESS_ERROR(hrResult);

    pIndexBuffer->GetDesc(&IndexDesc);
        
    if (IndexDesc.Format == D3DFMT_INDEX32)
    {
        // faint - -! 

        KG_PROCESS_ERROR(false);
    }
    else
    {
        BYTE*       pVertex = NULL;
        D3DXVECTOR3 vFace[3];

        WORD*       pIndex = NULL;
        WORD        face[3];

        int         nVetrexSize = 0;

        D3DXVECTOR3 vNormal;
        D3DXVECTOR3 vEdge1;
        D3DXVECTOR3 vEdge2;

        float       fCos;


        hrResult = pIndexBuffer->Lock(0, 0, (void**)&pIndex, D3DLOCK_READONLY);
        KG_COM_PROCESS_ERROR(hrResult);

        face[0] = pIndex[Index * 3 + 0];
        face[1] = pIndex[Index * 3 + 1];
        face[2] = pIndex[Index * 3 + 2];

        pIndexBuffer->Unlock();

        hrResult = pVertxBuffer->Lock(0, 0, (void**)&pVertex, D3DLOCK_READONLY);
        KG_COM_PROCESS_ERROR(hrResult);

        nVetrexSize = (int)m_ppMeshes[SMBT_NORMAL]->GetNumBytesPerVertex();

        vFace[0] = *(D3DXVECTOR3*)&pVertex[face[0] * nVetrexSize];
        vFace[1] = *(D3DXVECTOR3*)&pVertex[face[1] * nVetrexSize];
        vFace[2] = *(D3DXVECTOR3*)&pVertex[face[2] * nVetrexSize];

        pVertxBuffer->Unlock();

        vEdge1 = vFace[1] - vFace[0];
        vEdge2 = vFace[2] - vFace[0];

        D3DXVec3Cross(&vNormal, &vEdge1, &vEdge2);
        
        D3DXVec3Normalize(&vNormal, &vNormal);
        D3DXVec3Normalize(&vDir, &vDir);

            
        // faint!! 
        if (pSubMtl->m_dwOption & MATERIAL_OPTION_CULL_CW)
            vNormal *= -1.f;

        fCos = D3DXVec3Dot(&vNormal, &vDir);

        KG_PROCESS_ERROR(fCos > 0.f);

    }

    nResult = true;
Exit0:
    //SAFE_RELEASE(pIndexBuffer);
    //SAFE_RELEASE(pVertxBuffer);

    return nResult;
}

BOOL KG3DMesh::GetCameraNearRayIntersect(IN CONST D3DXVECTOR3 vSrc[],
                                   IN CONST D3DXVECTOR3 vDst[],
                                   IN CONST INT         nNum,
								   IN INT *pIgnoreList,
								   IN INT nNumIgnoreList,
                                   OUT D3DXVECTOR3*     pCross,
                                   OUT      INT*        pIndex

                                   )
{
    if (g_cGraphicsEngine.m_bSupportSSE)
        return GetNearRayIntersect_SSE(vSrc, vDst, nNum, pIgnoreList, nNumIgnoreList, pCross, pIndex);
    else
        ASSERT(false);

    return false;
}


namespace {

    inline void SSE_Vec3Cross(D3DXVECTOR4* r, const D3DXVECTOR4* v1, const D3DXVECTOR4* v2)
    {
        _asm
        {
            pushad

            mov    eax,  v1
            mov    edx,  v2
            movaps xmm0, dword ptr [eax]
            movaps xmm1, dword ptr [edx]
            movaps xmm2, xmm0
            movaps xmm3, xmm1

            shufps xmm0, xmm0, 0xc9
            shufps xmm1, xmm1, 0xd2
            mulps  xmm0, xmm1

            shufps xmm2, xmm2, 0xd2
            shufps xmm3, xmm3, 0xc9
            mulps  xmm2, xmm3

            subps  xmm0, xmm2

            movaps xmm2, xmm0;
            mulps  xmm0, xmm0;
            movaps xmm1, xmm0;
            shufps xmm1, xmm1, 4Eh;
            addps  xmm0, xmm1;
            movaps xmm1, xmm0;
            shufps xmm1, xmm1, 11h;
            addps  xmm0, xmm1;

            rsqrtps xmm0, xmm0;
            mulps   xmm2, xmm0;

            mov     eax,  r

            movaps  dword ptr[eax],    xmm2

            popad
        }
    }

    float SSE_Vec3Dot(const D3DXVECTOR4* v1, const D3DXVECTOR4* v2)
    {
        __declspec(align(16)) D3DXVECTOR4 vr;

        _asm
        {
            pushad

            mov    eax,  v1
            mov    edx,  v2

            movaps xmm0, dword ptr [eax]
            mulps  xmm0, dword ptr [edx]

            movaps xmm1, xmm0
            shufps xmm1, xmm0,   0x4e
            addps  xmm1, xmm0
            shufps xmm0, xmm1,   0xb1
            addps  xmm0, xmm1

            movaps vr,     xmm0

            popad
        }

        return vr.x;
    }


    inline bool IsVertexInTriangle(IN CONST D3DXVECTOR3* v,
                                   IN CONST D3DXVECTOR3* a,
                                   IN CONST D3DXVECTOR3* b,
                                   IN CONST D3DXVECTOR3* c,
                                   IN CONST D3DXVECTOR3* n
                                   )
    {
        D3DXVECTOR3 vCross;

        D3DXVec3Cross(&vCross, &(*b - *a), &(*v - *a));
        D3DXVec3Normalize(&vCross, &vCross);

        if (D3DXVec3Dot(&vCross, n) < -0.5f)
            return false;

        D3DXVec3Cross(&vCross, &(*c - *b), &(*v - *b));
        D3DXVec3Normalize(&vCross, &vCross);

        if (D3DXVec3Dot(&vCross, n) < -0.5f)
            return false;

        D3DXVec3Cross(&vCross, &(*a - *c), &(*v - *c));
        D3DXVec3Normalize(&vCross, &vCross);

        if (D3DXVec3Dot(&vCross, n) < -0.5f)
            return false;

        return true;
    }
}

#pragma warning(disable : 4795) //4795：在函数中插入asm会阻碍优化
#pragma warning(disable : 4740)	//4740: 流进入或流出inline的ASM代码会阻止全局优化
BOOL KG3DMesh::GetNearRayIntersect_SSE(IN CONST D3DXVECTOR3 vSrc[], 
                                       IN CONST D3DXVECTOR3 vDst[],
                                       IN CONST INT         nNum, 
                                       IN INT *pIgnoreList,
									   IN INT nNumIgnoreList,
									   OUT D3DXVECTOR3*     pCross,
                                       OUT      INT*        pIndex
                                       )
{
    if (!m_ppMeshes[SMBT_NORMAL] || !pCross || nNum <= 0)
        return false;

    float fDisNear = 0.f;
    *pCross = vSrc[0];
    *pIndex = 0;
	int nNumIgnore = nNumIgnoreList;
	__declspec(align(16)) float a_max[4]   = {FLT_MIN, FLT_MIN, FLT_MIN, FLT_MIN};
    __declspec(align(16)) float a_min[4]   = {FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX};
    __declspec(align(16)) D3DXVECTOR4 vDir = vDst[0] - vSrc[0];

    for (int i = 0; i < nNum; i++)
    {
        a_max[0] = max(a_max[0], max(vSrc[i].x, vDst[i].x));
        a_max[1] = max(a_max[1], max(vSrc[i].y, vDst[i].y));
        a_max[2] = max(a_max[2], max(vSrc[i].z, vDst[i].z));

        a_min[0] = min(a_min[0], min(vSrc[i].x, vDst[i].x));
        a_min[1] = min(a_min[1], min(vSrc[i].y, vDst[i].y));
        a_min[2] = min(a_min[2], min(vSrc[i].z, vDst[i].z));
    }

    WORD* Indices = NULL;
    BYTE* Vers    = NULL;
	int *pIgnore = pIgnoreList;
    int   nVSize = (int)m_ppMeshes[SMBT_NORMAL]->GetNumBytesPerVertex();
    int   nFace  = (int)m_ppMeshes[SMBT_NORMAL]->GetNumFaces();
	DWORD *dwAttributeIndex = NULL;

	if (SUCCEEDED(m_ppMeshes[SMBT_NORMAL]->LockAttributeBuffer(0, &dwAttributeIndex)))
	{
		if (SUCCEEDED(m_ppMeshes[SMBT_NORMAL]->LockIndexBuffer(D3DLOCK_READONLY, (void**)&Indices)))
		{
			if (SUCCEEDED(m_ppMeshes[SMBT_NORMAL]->LockVertexBuffer(D3DLOCK_READONLY, (void**)&Vers)))
			{
				__declspec(align(16)) D3DXVECTOR4 v0_4;
				__declspec(align(16)) D3DXVECTOR4 v1_4;
				__declspec(align(16)) D3DXVECTOR4 v2_4;

				int itor = -1;

				__asm {
					// init
Start :
					pushad                                                  // push the regisert state
					mov     ecx,            dword ptr [Indices]             // ecx  : Indices ptr
					mov     ebx,            itor                            // ebx  : face iterator
					movaps  xmm4,           a_max                           // xmm4 : max pos of line
					movaps  xmm5,           a_min                           // xmm5 : min pos of line

Next :
					// loop control
					add     ebx,            1
					cmp     ebx,            nFace
					jge     Fish

					xor eax, eax //如果没有pIgnoreList, 继续
					cmp eax, nNumIgnore
					je Process

CheckIngoreList:
					mov edx, dwAttributeIndex
					mov ecx, dword ptr[edx + ebx * 4]
					mov edx, pIgnore
					cmp ecx, dword ptr[edx + eax * 4]
					je Next
					add eax, 1
					cmp eax, nNumIgnore
					jne CheckIngoreList
Process:					
					
					// get the vecter of the face
					mov     ecx,            dword ptr [Indices]             // ecx  : Indices ptr
					mov     eax,            ebx
					imul    eax,            eax,    3  

					movzx   edx,            word ptr [ecx + eax * 2] 
					imul    edx,            dword ptr [nVSize]
					add     edx,            dword ptr [Vers] 

					movups  xmm0,           [edx]                           // xmm0  : v0

					movzx   edx,            word ptr [ecx + eax * 2 + 2] 
					imul    edx,            dword ptr [nVSize]
					add     edx,            dword ptr [Vers] 

					movups  xmm1,           [edx]                           // xmm1  : v1

					movzx   edx,            word ptr [ecx + eax * 2 + 4] 
					imul    edx,            dword ptr [nVSize]
					add     edx,            dword ptr [Vers] 

					movups  xmm2,           [edx]                           // xmm2  : v2

					movaps  xmm3,           xmm0                            // xmm3  : the max pos 
					maxps   xmm3,           xmm1
					maxps   xmm3,           xmm2

					movaps  xmm7,           xmm0                            // xmm7  : the min pos
					minps   xmm7,           xmm1
					minps   xmm7,           xmm2

						// do bbox check

					movaps  xmm6,           xmm4
					cmpltps xmm6,           xmm7
					movmskps eax,           xmm6    
					and      eax,           0xFFFFFFF7

					cmp     eax,            0
					jne     Next

					movaps  xmm6,           xmm3
					cmpltps xmm6,           xmm5
					movmskps eax,           xmm6
					and      eax,           0xFFFFFFF7

					cmp     eax,            0
					jne     Next

					mov     itor,           ebx
					movaps  v0_4,           xmm0
					movaps  v1_4,           xmm1
					movaps  v2_4,           xmm2

					popad

				} 

				//                D3DXVECTOR3* v0 = (D3DXVECTOR3*)&v0_4;
				//                D3DXVECTOR3* v1 = (D3DXVECTOR3*)&v1_4;
				//
				/* 
				D3DXVECTOR3	Vec3TestArray[] = {
				D3DXVECTOR3(FLT_MAX, FLT_MIN, FLT_MAX),
				D3DXVECTOR3(10, -10, 0),
				};*/
				//  D3DXVECTOR3* v2 = (D3DXVECTOR3*)&v2_4;

				__declspec(align(16)) D3DXVECTOR4 e1 = v1_4 - v0_4;
				__declspec(align(16)) D3DXVECTOR4 e2 = v2_4 - v0_4;
				__declspec(align(16)) D3DXVECTOR4 nr;

				SSE_Vec3Cross(&nr, &e1, &e2);

				for (int i = 0; i < nNum; i++) 
				{
					__declspec(align(16)) D3DXVECTOR4 vTo = 
						v0_4 - D3DXVECTOR4(vSrc[i].x, vSrc[i].y, vSrc[i].z, 0.f);

					FLOAT fDir = SSE_Vec3Dot(&vDir, &nr);
					FLOAT fDis = SSE_Vec3Dot(&vTo,  &nr);

					if (fDir == 0.f)
						continue;

					if (fDir > 0.f && fDis < 0.f)
						continue;

					if (fDir < 0.f && fDis > 0.f)
						continue;

					if (fDis < 0.f)
						fDis = -fDis;

					if (fDir < 0.f)
						fDir = -fDir;

					if (fDis > fDir)
						continue;


					__declspec(align(16)) D3DXVECTOR4 vCross = 
						D3DXVECTOR4(vSrc[i].x, vSrc[i].y, vSrc[i].z, 0.f) + vDir * (fDis / fDir);

					if (!IsVertexInTriangle((D3DXVECTOR3*)&vCross, (D3DXVECTOR3*)&v0_4, (D3DXVECTOR3*)&v1_4, (D3DXVECTOR3*)&v2_4, (D3DXVECTOR3*)&nr))
						continue;

					//if (!SSE_IsVertexInTriangle(&vCross, &v0_4, &v1_4, &v2_4, &nr))
					//    goto Start;


					D3DXVECTOR3 vc = D3DXVECTOR3(vCross.x, vCross.y, vCross.z) - vSrc[i];
					FLOAT dis = vc.x * vc.x + vc.y * vc.y + vc.z * vc.z;

					if (dis > fDisNear)
					{
						fDisNear = dis;
						*pIndex = i;
					}


				}

				goto Start ;
Fish :
				__asm {popad}

				m_ppMeshes[SMBT_NORMAL]->UnlockVertexBuffer();
			}
			m_ppMeshes[SMBT_NORMAL]->UnlockIndexBuffer();
		}
		m_ppMeshes[SMBT_NORMAL]->UnlockAttributeBuffer();
	}

    if (fDisNear > 0.f)
    {
        D3DXVECTOR3 dir;
        D3DXVec3Normalize(&dir, (D3DXVECTOR3*)&vDir);

        fDisNear = sqrtf(fDisNear);
        *pCross = vSrc[*pIndex] + dir * fDisNear;

        return true;
    }

    return false;
}
#pragma warning(default : 4795)

BOOL KG3DMesh::GetRayIntersect(DWORD& dwCount,D3DXVECTOR3* pInterPoint,DWORD dwMaxNum,D3DXVECTOR3 Origin,D3DXVECTOR3 Direction)
{
	LPD3DXBUFFER pBuffer = NULL;

	BOOL bHit = FALSE;
	DWORD dwHitCount = 0;

	if(m_lpBsp && g_cEngineOption.bUseModelBsp)
	{
		return m_lpBsp->GetRayIntersect(dwCount,pInterPoint,dwMaxNum,Origin,Direction);
	}
	else
	{
		KG_PROCESS_ERROR(m_ppMeshes[SMBT_NORMAL]);
		{
			FLOAT fHitDis = FLT_MAX;//D3DXIntersect的FLOAT * pDist只返回最近的交点，已经验证过了，不是返回一个数组
			D3DXVECTOR3 OriginNew = Origin - Direction * 5000;
			if(FAILED(D3DXIntersect(m_ppMeshes[SMBT_NORMAL], &OriginNew, &Direction, &bHit, 
				NULL, NULL, NULL, 
				&fHitDis/*fHitDis*/,&pBuffer, 
				&dwHitCount)))
				return FALSE;

			if(bHit)
			{
				LPD3DXINTERSECTINFO pInfo = (LPD3DXINTERSECTINFO)pBuffer->GetBufferPointer();
				if(dwHitCount > dwMaxNum)
					dwHitCount = dwMaxNum;
				for(DWORD i = 0; i < dwHitCount; i++)
				{
					pInterPoint[i] = OriginNew + Direction * pInfo[i].Dist;
				}
				dwCount = dwHitCount;
				//SAFE_RELEASE(pBuffer);
				return TRUE;
			}
			//SAFE_RELEASE(pBuffer);
		}
	}

Exit0:
	return FALSE;
}

BOOL KG3DMesh::RayIntersection( const D3DXVECTOR3& vSrc, const D3DXVECTOR3& vNormalizedDir, FLOAT* pfRet
							   , KG3DIntersectionFilter* pFilter /*= NULL*/)
{
	FLOAT fHitDis = FLT_MAX;
	BOOL bHit = FALSE;

	if (NULL != pFilter)
	{
		HRESULT hr = pFilter->Accept(*this);
		if (FAILED(hr))
			return FALSE;
	}
	
	if(m_lpBsp && g_cEngineOption.bUseModelBsp)
	{
		D3DXVECTOR3 vSrc0 = vSrc;
		D3DXVECTOR3 vNormalizedDir0 = vNormalizedDir;
		return m_lpBsp->RayIntersection(vSrc0,vNormalizedDir0,pfRet,pFilter);
	}
	else
	{
		//这样不是很准，DX可能做了些优化，如果实在不行了改改那个SSe那个用
		HRESULT hr = D3DXIntersect(m_ppMeshes[SMBT_NORMAL], &vSrc, &vNormalizedDir, &bHit, 
			NULL, NULL, NULL, 
			&fHitDis, NULL, 
			NULL);

		if (SUCCEEDED(hr))
		{	
			if (bHit && fHitDis > 0)
			{
				_ASSERTE(fHitDis < FLT_MAX);
				if (NULL != pfRet)
				{
					*pfRet = fHitDis;
				}
				return TRUE;			
			}
		}
		return FALSE;
	}

}

//LPD3DXMESH KG3DMesh::GetActiveMesh()
//{
//	HRESULT hrRetCode = E_FAIL;
//	LPD3DXMESH pReturn = NULL;
//	
//	
//Exit0:
//	return pReturn;
//}

//HRESULT KG3DMesh::UpdateActiveMesh(D3DXMATRIX* pMatrices)
//{
//	HRESULT hrResult = E_FAIL;
//	HRESULT hrRetCode  = E_FAIL;
//	LPD3DXMESH pMesh = GetActiveMesh();
//	void* pCurrentVertices = NULL;
//	DWORD dwNumVertices = 0;
//	KG_PROCESS_ERROR(pMesh);
//	
//	hrRetCode = pMesh->LockVertexBuffer(0, &pCurrentVertices);
//	KG_COM_PROCESS_ERROR(hrRetCode);
//
//	for (DWORD i = 0; i < dwNumVertices; i++)
//	{
//		
//	}
//	
//	
//	hrResult = S_OK;
//Exit0:
//	if (pMesh)
//	{
//		pMesh->UnlockVertexBuffer();
//	}
//	return hrResult;
//}

//BOOL KG3DMesh::IsRayIntersect1(D3DXVECTOR3 &Inter, D3DXVECTOR3 Origin, D3DXVECTOR3 Direction, D3DXMATRIX* pMatrices, BBox* pBoxUpdate)
//{
//	HRESULT hr = E_FAIL;
//	BOOL bHit = FALSE;
//	DWORD dwHitCount;
//	float fHitDis[100];
//	KG_PROCESS_ERROR(m_ppMeshes[SMBT_NORMAL]);
//	LPD3DXMESH pCheckMesh = m_ppMeshes[SMBT_NORMAL];
//	if (m_ppMeshes[SMBT_ACTIVE] && m_pSkinInfo && pMatrices)
//	{
//		PBYTE   pbVerticesSrc;
//		PBYTE   pbVerticesDest;
//
//		HRESULT hr = m_ppMeshes[SMBT_NORMAL]->LockVertexBuffer(D3DLOCK_READONLY, (LPVOID*)&pbVerticesSrc );
//		KG_COM_PROCESS_ERROR(hr);
//		hr = m_ppMeshes[SMBT_ACTIVE]->LockVertexBuffer(0, (LPVOID*)&pbVerticesDest );
//		KG_COM_PROCESS_ERROR(hr);
//
//		hr = m_pSkinInfo->UpdateSkinnedMesh(
//			pMatrices, NULL, pbVerticesSrc, pbVerticesDest
//			);
//		KG_COM_PROCESS_ERROR(hr);
//
//		if (/*!g_bClient && */m_dwVertexStride > sizeof(D3DXVECTOR3))
//		{
//			D3DXVECTOR3 vMin;
//			D3DXVECTOR3 vMax;
//			if (SUCCEEDED(D3DXComputeBoundingBox(
//				(D3DXVECTOR3*)pbVerticesDest,
//				m_ppMeshes[SMBT_ACTIVE]->GetNumVertices(),
//				m_dwVertexStride,
//				&vMin,
//				&vMax
//				))
//				)
//			{
//				m_BBox.A = vMin;
//				m_BBox.B = vMax;
//
//				if (pBoxUpdate)
//					*pBoxUpdate = m_BBox;
//
//
//			}
//		}
//		pCheckMesh = m_ppMeshes[SMBT_ACTIVE];
//	}
//	KG_PROCESS_ERROR(pCheckMesh);
//	hr = D3DXIntersect(pCheckMesh, 
//		&Origin, 
//		&Direction, 
//		&bHit, 
//		NULL,
//		NULL,
//		NULL, 
//		fHitDis,
//		NULL, 
//		&dwHitCount);
//	/*KG_PROCESS_SUCCESS(!hr);*/
//	KG_COM_PROCESS_ERROR(hr);
//
//	if(bHit)
//	{
//		float fMin = 10000000;
//		fMin = fHitDis[0];
//		Inter = Origin + fMin * Direction;
//	}
//	//Exit1:
//Exit0:
//	if (m_ppMeshes[SMBT_NORMAL])
//	{
//		m_ppMeshes[SMBT_NORMAL]->UnlockVertexBuffer();
//	}
//	if (m_ppMeshes[SMBT_ACTIVE])
//	{
//		m_ppMeshes[SMBT_ACTIVE]->UnlockVertexBuffer();
//	}
//	return bHit;
//}



BOOL KG3DMesh::IsRayIntersect(D3DXVECTOR3 &Inter, D3DXVECTOR3 Origin, D3DXVECTOR3 Direction, D3DXMATRIX* pMatrices, BBox* pBoxUpdate)
{
	HRESULT hr = E_FAIL;
	BOOL bHit = FALSE;
    BOOL bLockNormalVertexbuffer = FALSE;
    BOOL bLockActiveVertexbuffer = FALSE;
	DWORD dwHitCount;
    LPD3DXMESH pCheckMesh = NULL;
	float fHitDis[100];

	KG_PROCESS_ERROR(m_ppMeshes[SMBT_NORMAL]);

    pCheckMesh = m_ppMeshes[SMBT_NORMAL];

    if (m_ppMeshes[SMBT_ACTIVE] && m_pSkinInfo && pMatrices)
    {
        PBYTE pbVerticesSrc;
        PBYTE pbVerticesDest;
		D3DXMATRIX matTemp[s_dwMaxBone];

		for (DWORD i = 0; i < m_dwNumBone; i++)
			matTemp[i] = m_matReverseMatrix[i] * pMatrices[i];

        hr = m_ppMeshes[SMBT_NORMAL]->LockVertexBuffer(D3DLOCK_READONLY, (LPVOID*)&pbVerticesSrc);
        KG_COM_PROCESS_ERROR(hr);

        bLockNormalVertexbuffer = TRUE;

        hr = m_ppMeshes[SMBT_ACTIVE]->LockVertexBuffer(0, (LPVOID*)&pbVerticesDest);
        KG_COM_PROCESS_ERROR(hr);

        bLockActiveVertexbuffer = TRUE;

        hr = m_pSkinInfo->UpdateSkinnedMesh(matTemp, NULL, pbVerticesSrc, pbVerticesDest);
        KG_COM_PROCESS_ERROR(hr);

        if (m_dwVertexStride > sizeof(D3DXVECTOR3))
        {
            D3DXVECTOR3 vMin;
            D3DXVECTOR3 vMax;

            if (SUCCEEDED(D3DXComputeBoundingBox((D3DXVECTOR3*)pbVerticesDest, m_ppMeshes[SMBT_ACTIVE]->GetNumVertices(),m_dwVertexStride, &vMin,&vMax)))
            {
                m_BBox.A = vMin;
                m_BBox.B = vMax;

                if (pBoxUpdate)
                {
                    *pBoxUpdate = m_BBox;
                }
            }
        }

        pCheckMesh = m_ppMeshes[SMBT_ACTIVE];
    }

	KG_PROCESS_ERROR(pCheckMesh);

	hr = D3DXIntersect(pCheckMesh, &Origin, &Direction, &bHit, NULL, NULL, NULL, fHitDis, NULL, &dwHitCount);
    KG_COM_PROCESS_ERROR(hr);

	if(bHit)
	{
		float fMin = 10000000;
		fMin = fHitDis[0];
		Inter = Origin + fMin * Direction;
	}

Exit0:
	if (bLockNormalVertexbuffer && m_ppMeshes[SMBT_NORMAL])
		m_ppMeshes[SMBT_NORMAL]->UnlockVertexBuffer();

    if (bLockActiveVertexbuffer && m_ppMeshes[SMBT_ACTIVE])
		m_ppMeshes[SMBT_ACTIVE]->UnlockVertexBuffer();

    return bHit;
}


HRESULT KG3DMesh::DrawBones(void* pExtInfo)
{
	HRESULT hrResult = E_FAIL;
	HRESULT hrRetCode = E_FAIL;
	KG3DRenderState RenderState;
	RenderState.SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);

	MeshRenderExtraInfo* pData = static_cast<MeshRenderExtraInfo*>(pExtInfo);
	D3DXMATRIX* pmatWorld = NULL;
	
	if (pData)
	{
		//Matrics from pExtInfo
		KG_PROCESS_ERROR(pData->dwType == ENUM_MESHRENDER_EXTR_INFO_MATRIX);
		KG_PROCESS_ERROR(pData->Data);

		pmatWorld = static_cast<D3DXMATRIX*>(pData->Data);
		for(DWORD i = 0; i < m_dwNumBone; i++)
		{
			BoneInfo1& BoneInfo = m_BoneInfo[i];
			D3DXVECTOR3 vec3A = D3DXVECTOR3(0, 0, 0);
			D3DXVECTOR3 vec3B = D3DXVECTOR3(0, 0, 0);

			D3DXVec3TransformCoord(&vec3B, &vec3B, &pmatWorld[i]);

			if (BoneInfo.dwParentIndex == s_dwInvalidateIndex)
				continue;

			D3DXVec3TransformCoord(&vec3A, &vec3A, &pmatWorld[BoneInfo.dwParentIndex]);
			vec3A += D3DXVECTOR3(0, 0, 550);
			vec3B += D3DXVECTOR3(0, 0, 550);

			if(BoneInfo.IsFlexibleBone)
			{
				hrRetCode = g_cGraphicsTool.DrawLine(&vec3A, &vec3B, 0xFFFF00FF, 0xFFFF00FF); 
			}
			else
			{
				hrRetCode = g_cGraphicsTool.DrawLine(&vec3A, &vec3B, 0xFF0000FF, 0xFFFF0000);
			}
			KG_COM_PROCESS_ERROR(hrRetCode);
		}
	}
	else
	{
		for (WORD i = 0; i < m_dwNumBone; i++)
		{
			BoneInfo1& BoneInfo = m_BoneInfo[i];
			D3DXVECTOR3 vec3A = D3DXVECTOR3(0, 0, 0);
			D3DXVECTOR3 vec3B = D3DXVECTOR3(0, 0, 0);

			D3DXVec3TransformCoord(&vec3B, &vec3B, &BoneInfo.CurMatrix);

			if (BoneInfo.dwParentIndex == s_dwInvalidateIndex)
				continue;
			
			BoneInfo1& ParentInfo = m_BoneInfo[BoneInfo.dwParentIndex];
			D3DXVec3TransformCoord(&vec3A, &vec3A, &ParentInfo.CurMatrix);
			vec3A += D3DXVECTOR3(0, 0, 550);
			vec3B += D3DXVECTOR3(0, 0, 550);

			if(BoneInfo.IsFlexibleBone)
			{
				hrRetCode = g_cGraphicsTool.DrawLine(&vec3A, &vec3B, 0xFFFF00FF, 0xFFFF00FF); 
			}
			else
			{
				hrRetCode = g_cGraphicsTool.DrawLine(&vec3A, &vec3B, 0xFF0000FF, 0xFFFF0000);
			}
			KG_COM_PROCESS_ERROR(hrRetCode);
		}
	}
	
	hrResult = S_OK;
Exit0:
	return hrResult;
}

HRESULT KG3DMesh::GetNumBones(INT *pnRetNum)
{
    HRESULT hrResult = E_FAIL;
    KGLOG_PROCESS_ERROR(pnRetNum);
    *pnRetNum = static_cast<int>(m_dwNumBone);
	hrResult = S_OK;
Exit0:
    return hrResult;
}
int KG3DMesh::GetNumBones()
{
    return static_cast<int>(m_dwNumBone);
}

HRESULT KG3DMesh::AttachBip(KG3DBip* pBip, std::vector<int>& RemapTable)
{
	HRESULT hr = E_FAIL;
	KGLOG_PROCESS_ERROR(pBip);
	
	RemapTable.resize(m_dwNumBone);
	for(DWORD i = 0; i < m_dwNumBone; i++)
	{
		int nBoneIndex = pBip->FindBone(m_BoneInfo[i].strBoneName);
		RemapTable[i] = nBoneIndex;
		if (nBoneIndex != -1)
		{
			DWORD dwParentIndex = m_BoneInfo[i].dwParentIndex;
			if (dwParentIndex != s_dwInvalidateIndex)
			{
				D3DXMATRIX matInvParent;
				D3DXMatrixInverse(&matInvParent, NULL, &m_BoneInfo[dwParentIndex].CurMatrix);
				D3DXMATRIX mat = m_BoneInfo[i].CurMatrix * matInvParent;
				pBip->SetBoneLocalMatrix(nBoneIndex,mat);
			}
			else
			{
				pBip->SetBoneLocalMatrix(nBoneIndex,m_BoneInfo[i].CurMatrix);
			}
		}
	}
	hr = S_OK;
Exit0:
	return hr;
}

HRESULT KG3DMesh::PostLoadProcess(void* pData)
{
	HRESULT hrResult = E_FAIL;
    HRESULT hrRetCode = E_FAIL;
	KG3DModel::PostMeshLoadData* pModelData = 
		reinterpret_cast<KG3DModel::PostMeshLoadData*>(pData);
	
	D3DXMATRIX* pRenderMatrices = *(pModelData->ppRenderMatrices);
	D3DXMATRIX* pSocketMatrices = *(pModelData->ppSocketMatrices);
	D3DXMATRIX* pLocalMatrices = *(pModelData->ppLocalMatrices);
	
	*(pModelData->pNumSockets) = m_dwNumSocket;
	
	if (m_dwNumBone)
	{
		pRenderMatrices = new D3DXMATRIX[m_dwNumBone];
		KG_PROCESS_ERROR(pRenderMatrices);

		pLocalMatrices = new D3DXMATRIX[m_dwNumBone];
		KG_PROCESS_ERROR(pLocalMatrices);

		memset(pRenderMatrices, 0, sizeof(D3DXMATRIX) * m_dwNumBone);
		for(DWORD i = 0; i < m_dwNumBone; i++)
		{
			pRenderMatrices[i] = m_BoneInfo[i].CurMatrix;
			//pLocalMatrices[i] = m_BoneInfo[i].LocalWorldCurMatrix_NotNeeded;
		
            hrRetCode = KG3DIsFlexibleBodyBone(m_BoneInfo[i].strBoneName, &m_BoneInfo->IsFlexibleBone);
            KGLOG_COM_PROCESS_ERROR(hrRetCode);
        }
	}

	if (m_dwNumSocket)
	{
		pSocketMatrices = new D3DXMATRIX[m_dwNumSocket];
		KGLOG_PROCESS_ERROR(pSocketMatrices);

		memset(pSocketMatrices, 0, sizeof(D3DXMATRIX) * m_dwNumSocket);

		for (DWORD i = 0; i < m_dwNumSocket; i++)
		{
			pSocketMatrices[i] = m_Sockets[i].matCurMatrix;
		}
	}
	
    SAFE_DELETE_ARRAY(*pModelData->ppRenderMatrices);
    SAFE_DELETE_ARRAY(*pModelData->ppSocketMatrices);
    SAFE_DELETE_ARRAY(*pModelData->ppLocalMatrices);
	*pModelData->ppRenderMatrices = pRenderMatrices;
	*pModelData->ppSocketMatrices = pSocketMatrices;
	*pModelData->ppLocalMatrices = pLocalMatrices;

	hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        KG_DELETE_ARRAY(pSocketMatrices);
        KG_DELETE_ARRAY(pLocalMatrices);
        KG_DELETE_ARRAY(pRenderMatrices);
    }
	return hrResult;
}


void KG3DMesh::UpdateToObj(D3DXMATRIX* pMatObj, D3DXMATRIX* pMatLocal)
{
	D3DXMATRIX matBase;
	D3DXMatrixIdentity(&matBase);
	for (size_t j = 0; j < m_nBaseBoneID.size(); j++)
	{
		int nCurBaseBone = m_nBaseBoneID[j];
		pMatObj[nCurBaseBone] = pMatLocal[nCurBaseBone];
		for(size_t i = 0; i < m_BoneInfo[nCurBaseBone].dwNumChild; i++)
		{
			Helper_UpdateToObj(m_BoneInfo[nCurBaseBone].dwChildIndex[i], 
				pMatObj[nCurBaseBone], 
				pMatLocal,
				pMatObj);
		}
	}
}

void KG3DMesh::Helper_UpdateToObj(int nIndex, 
								  const D3DXMATRIX& matBase, 
								  D3DXMATRIX* pMatLocal, 
								  D3DXMATRIX* pMatObj)
{
	pMatObj[nIndex] = pMatLocal[nIndex] * matBase;
	for (size_t i = 0; i < m_BoneInfo[nIndex].dwNumChild; i++)
	{
		Helper_UpdateToObj(m_BoneInfo[nIndex].dwChildIndex[i], 
			pMatObj[nIndex], 
			pMatLocal, 
			pMatObj);
	}
}

DWORD KG3DMesh::GetVertexStride(DWORD dwFVF)
{
	DWORD dwStride = 0;
	const VertexFromatOffsetItem *pVertexFromatItem = GetVertexFormat(dwFVF);
	KG_PROCESS_ERROR(pVertexFromatItem);
	
	dwStride = pVertexFromatItem->dwDestOffset[pVertexFromatItem->dwNumElement - 1] + 
		pVertexFromatItem->dwDestStride[pVertexFromatItem->dwNumElement - 1];
Exit0:
	return dwStride;
}

const KG3DMesh::VertexFromatOffsetItem* KG3DMesh::GetVertexFormat(DWORD dwFVF)
{
	const VertexFromatOffsetItem *pReturn = NULL;
	size_t uSize = sizeof(s_VertexFormat) / sizeof(VertexFromatOffsetItem);
	size_t i = 0;
	for (; i < uSize; i++)
	{
		if (s_VertexFormat[i].dwFVF == dwFVF)
			break;
	}
	KG_PROCESS_ERROR(i != uSize);

	pReturn = &s_VertexFormat[i];
Exit0:
	return pReturn;
}

HRESULT KG3DMesh::Helper_FillVertexData(const ReadInMeshData& Data)
{
	HRESULT hrRetCode  = E_FAIL;
	HRESULT hrResult = E_FAIL;
	BYTE *pVertexData = NULL;
	short *pIndices = NULL;
	DWORD *pAttributeTable = NULL;
	const VertexFromatOffsetItem *pVertexFormat = NULL;

	DWORD dwDestVertexStride = 0;
	KG_PROCESS_ERROR(Data.dwNumVertices < s_dwMaxNumVertices);
	KG_PROCESS_ERROR(m_ppMeshes[SMBT_NORMAL]);

	dwDestVertexStride = m_ppMeshes[SMBT_NORMAL]->GetNumBytesPerVertex();
	KG_PROCESS_ERROR(dwDestVertexStride);

	hrRetCode = m_ppMeshes[SMBT_NORMAL]->LockVertexBuffer(0, (void**)(&pVertexData));
	KG_COM_PROCESS_ERROR(hrRetCode);

	pVertexFormat = GetVertexFormat(Data.dwMeshFVF);
	KG_PROCESS_ERROR(pVertexFormat);

	m_dwVertexStride = GetVertexStride(Data.dwMeshFVF);

	for (DWORD i = 0; i < Data.dwNumVertices; i++)
	{
		BYTE *pCurrentVertexData = pVertexData + dwDestVertexStride * i;
		for (DWORD j = 0; j < pVertexFormat->dwNumElement; j++)
		{
			const BYTE *pCurrentSrc = *(reinterpret_cast<BYTE* const*>(&Data) + pVertexFormat->dwSrcOffset[j]);
			memcpy(pCurrentVertexData + pVertexFormat->dwDestOffset[j],
				pCurrentSrc + pVertexFormat->dwSrcStride[j] * i,
				pVertexFormat->dwDestStride[j]);
		}
	}
	hrRetCode = m_ppMeshes[SMBT_NORMAL]->LockIndexBuffer(0, (void**)&pIndices);
	KG_COM_PROCESS_ERROR(hrRetCode);

	hrRetCode = m_ppMeshes[SMBT_NORMAL]->LockAttributeBuffer(0, &pAttributeTable);
	KG_COM_PROCESS_ERROR(hrRetCode);

	
	for(DWORD i = 0; i < Data.dwNumFaces; i++)
	{
		pIndices[i * 3]     = static_cast<short>(Data.pFaceIndices[i * 3]);
		pIndices[i * 3 + 1] = static_cast<short>(Data.pFaceIndices[i * 3 + 1]);
		pIndices[i * 3 + 2] = static_cast<short>(Data.pFaceIndices[i * 3 + 2]);
		pAttributeTable[i] = Data.pSubsetIndices[i];
	}

	hrResult = S_OK;
Exit0:
	if (m_ppMeshes[SMBT_NORMAL])
	{
		m_ppMeshes[SMBT_NORMAL]->UnlockVertexBuffer();
		m_ppMeshes[SMBT_NORMAL]->UnlockIndexBuffer();
		m_ppMeshes[SMBT_NORMAL]->UnlockAttributeBuffer();
	}
	return hrResult;
}

HRESULT KG3DMesh::Helper_CreateSkinMesh()
{
	HRESULT hr = E_FAIL;
	LPD3DXEFFECT pShader = g_cGraphicsTool.GetDefaultShader(DEFST_SKINMESH);
	DWORD* pAdjacencyTable = NULL;
	DWORD dwNumFace = 0;
	LPD3DXMESH pMesh = NULL;
	DWORD dwNewFVF = NULL;
	DWORD dwOldFVF = NULL;
	DWORD dwStartTime = timeGetTime();
	D3DVERTEXELEMENT9 decl[MAX_FVF_DECL_SIZE];

	KG_PROCESS_ERROR(NULL != pShader);
	
	KG_PROCESS_ERROR(m_ppMeshes[SMBT_NORMAL]);
	if (!(m_dwOptionEx & MESH_SAVE_BLENDMESH))
	{
		KG_PROCESS_ERROR(m_pSkinInfo);

		dwNumFace = m_ppMeshes[SMBT_NORMAL]->GetNumFaces();
		KG_PROCESS_ERROR(dwNumFace);

		int nNumBone = min(KG3DGraphicsTool::m_nMaxNumBone, static_cast<int>(m_dwNumBone));

		pAdjacencyTable = new DWORD[dwNumFace * 3];
		KG_PROCESS_ERROR(pAdjacencyTable);

		hr = m_ppMeshes[SMBT_NORMAL]->GenerateAdjacency(0.0f, pAdjacencyTable);
		KG_COM_PROCESS_ERROR(hr);

		SAFE_RELEASE(m_ppMeshes[SMBT_BLENDED]);
		SAFE_RELEASE(m_pBoneCombinationTable);

		hr = m_pSkinInfo->ConvertToIndexedBlendedMesh(m_ppMeshes[SMBT_NORMAL],
			0,
			nNumBone,
			pAdjacencyTable,
			NULL, 
			NULL,
			NULL,
			&m_dwMaxFaceInfl,
			&m_dwNumAttributeGroups,
			&m_pBoneCombinationTable,
			&m_ppMeshes[SMBT_BLENDED]
			);
		KG_COM_PROCESS_ERROR(hr);
		
		dwOldFVF = m_ppMeshes[SMBT_BLENDED]->GetFVF();
		dwNewFVF = (dwOldFVF & D3DFVF_POSITION_MASK) | (dwOldFVF & D3DFVF_TEXCOUNT_MASK) | D3DFVF_NORMAL |D3DFVF_LASTBETA_UBYTE4;
		hr = m_ppMeshes[SMBT_BLENDED]->CloneMesh(
			D3DXMESH_MANAGED, 
			g_VBDecl_Skin2Link,
			g_pd3dDevice, 
			&pMesh);
		KG_COM_PROCESS_ERROR(hr);
		
		pMesh->GetDeclaration(decl);
		//因为vertex shader将UBYTE4解释成D3DCOLOR, 所以需要转换类型
		//不可以用CloneMesh来完成， 因为CloneMesh将值转化为float再转化会整型
		D3DVERTEXELEMENT9* pIndicator = decl;
		while(pIndicator->Stream != 0xff)
		{
			if ((pIndicator->Usage == D3DDECLUSAGE_BLENDINDICES) &&
				pIndicator->UsageIndex == 0)
			{
				pIndicator->Type = D3DDECLTYPE_D3DCOLOR;
			}
			pIndicator++;
		}

		pMesh->UpdateSemantics(decl);

		SAFE_RELEASE(m_ppMeshes[SMBT_BLENDED]);
		m_ppMeshes[SMBT_BLENDED] = pMesh;
	}

	// create the active mesh use to select active model
    hr = m_ppMeshes[SMBT_NORMAL]->CloneMeshFVF(
        D3DXMESH_SYSTEMMEM, 
        m_ppMeshes[SMBT_NORMAL]->GetFVF(),
        g_pd3dDevice, 
        &m_ppMeshes[SMBT_ACTIVE]
        );
    KG_COM_PROCESS_ERROR(hr);
    //

	hr = S_OK;
Exit0:
	SAFE_DELETE_ARRAY(pAdjacencyTable);

	DWORD dwCost = timeGetTime() - dwStartTime;
	if(g_cEngineOption.bEnableTimeOptimizeLog && dwCost > 10)
	{
		KGLogPrintf(KGLOG_ERR, "TimeOptimize %d KG3DMesh::Helper_CreateSkinMesh  %s.",dwCost , m_scName.c_str());
	}
	return hr;
		
}
///////////////////////////////////////////////////////////
HRESULT KG3DMesh::Shader_RenderSkinMeshDepth_Begin()
{
	HRESULT hrResult = E_FAIL;
	UINT uNumPass = 0;

	HRESULT hr = S_OK;
	LPD3DXEFFECT pSkinShader = g_cGraphicsTool.GetDefaultShader(DEFST_SKINMESH);
	KG_PROCESS_ERROR(pSkinShader);
	hr = pSkinShader->SetTechnique("tecZ");
	KGLOG_COM_CHECK_ERROR(hr);
	hr = pSkinShader->Begin(&uNumPass,0 );//D3DXFX_DONOTSAVESTATE
	KG_COM_PROCESS_ERROR(hr);

	hrResult = S_OK;
Exit0:
	return hrResult;
}
HRESULT KG3DMesh::Shader_RenderSkinMeshDepth_NoChange(D3DXMATRIX* pmatBones, 
                                                      KG3DMaterial* pMaterial, 
													  DWORD dwOption)
{
	HRESULT hrResult = E_FAIL;
	HRESULT hrRetCode = S_OK;
	LPD3DXEFFECT pSkinShader = g_cGraphicsTool.GetDefaultShader(DEFST_SKINMESH);
	D3DXMATRIX matTemp[KG3DGraphicsTool::MAX_NUM_BONE_PER_SET_SHADER_2];

	D3DXMATRIX mat;
	D3DXMatrixIdentity(&mat);
	D3DXMATRIX matView;
	D3DXMATRIX matProj;
	D3DXMATRIX matWorld;
	D3DXMATRIX matViewProj;
	D3DXBONECOMBINATION* pBoneComb = NULL;
	KG3DRenderState RenderState;
	LPD3DXMESH pRenderMesh = GetRenderSkinMesh();

    KG_PROCESS_ERROR(pMaterial);
	KG_PROCESS_ERROR(pRenderMesh);
	KG_PROCESS_ERROR(pmatBones);
	KG_PROCESS_ERROR(pSkinShader);
	KG_PROCESS_ERROR(m_pBoneCombinationTable);
	KG_PROCESS_ERROR(m_pModel);
	KG_PROCESS_ERROR(pMaterial->IsLoaded());

	{
		matWorld = m_pModel->m_matWorld;
		g_pd3dDevice->GetTransform(D3DTS_VIEW, &matView);
		g_pd3dDevice->GetTransform(D3DTS_PROJECTION, &matProj);

		matViewProj = matWorld * matView * matProj;
		hrRetCode = pSkinShader->SetInt("nNumBlend", m_dwMaxFaceInfl);
		KG_COM_PROCESS_ERROR(hrRetCode);

		hrRetCode = pSkinShader->SetMatrix("WorldViewProj", &matViewProj);
		KG_COM_PROCESS_ERROR(hrRetCode);

		pSkinShader->SetInt("nIndex", m_pModel->GetSelectIndex());
	}

	RenderState.SetRenderState(D3DRS_FOGENABLE, FALSE);
	RenderState.SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	RenderState.SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);	

	pBoneComb = static_cast<D3DXBONECOMBINATION*>(m_pBoneCombinationTable->GetBufferPointer());
	KG_PROCESS_ERROR(pBoneComb);

	hrRetCode = pSkinShader->BeginPass(0);
	KG_COM_PROCESS_ERROR(hrRetCode);	

	
		
	for (DWORD i = 0; i < m_dwNumAttributeGroups; i++)
	{
		if (pBoneComb[i].AttribId >= pMaterial->m_dwNumMaterials)
			continue;

		KG3DMaterial::KG3DMaterialSubset* pSubMat = pMaterial->GetSubMaterial(pBoneComb[i].AttribId);
		KGLOG_PROCESS_ERROR(pSubMat);

		_MtlAlphaOperation* pAlphaOp = pSubMat->GetAlphaOperation();
		BOOL bEnableAlphaFactor = !(pAlphaOp->EnableAlphaTest || pAlphaOp->EnableAlphaBlend);

		_MtlRenderTwiceAlphaOption *pTwiceOption = pSubMat->GetAlphaRenderTwice();
		if (pTwiceOption->bEnable)
		{
			bEnableAlphaFactor = FALSE;
		}

		DWORD dwCullMode = D3DCULL_CW;
		if (pAlphaOp->EnableAlphaTest)
		{
			//RenderState.SetRenderState(D3DRS_ALPHAREF, pAlphaOp->BackAlphaTestRef);
			RenderState.SetRenderState(D3DRS_ALPHAREF, 254);
		}
		if (m_dwOption & MATERIAL_OPTION_CULL_NONE)
		{
			dwCullMode = D3DCULL_NONE;
		}
		else if (m_dwOption & MATERIAL_OPTION_CULL_CCW)
		{
			dwCullMode = D3DCULL_CCW;
		}
		RenderState.SetRenderState(D3DRS_CULLMODE, dwCullMode);
		pSkinShader->SetBool("bAlphaUseFactor", bEnableAlphaFactor);

		int nNumBone = min(KG3DGraphicsTool::m_nMaxNumBone, static_cast<int>(m_dwNumBone));
		if (dwOption & RENDER_BIP)
		{
			nNumBone = min(nNumBone, (int)(m_pModel->m_Mesh2Bip.size()));
			for (int k = 0; k < nNumBone; k++)
			{
				DWORD dwBoneIndex = pBoneComb[i].BoneId[k];
				if (dwBoneIndex != UINT_MAX)
				{
					int nMapedBoneIndex = m_pModel->m_Mesh2Bip[dwBoneIndex];
					if (nMapedBoneIndex != -1)
					{
						matTemp[k] = pmatBones[nMapedBoneIndex];
					}
					else
					{
						matTemp[k] = m_pModel->m_pBoneMatricesForRender[dwBoneIndex];
					}
				}
				else
				{
					matTemp[k] = mat;
				}
			}
		}
		else
		{
			for (int k = 0; k < nNumBone; k++)
			{
				DWORD dwBoneIndex = pBoneComb[i].BoneId[k];
				if (dwBoneIndex != UINT_MAX)
				{
					matTemp[k] = pmatBones[dwBoneIndex];
				}
				else
				{
					matTemp[k] = mat;
				}
			}
		}
		hrRetCode = pSkinShader->SetMatrixArray("matBones", matTemp, nNumBone);
		if (SUCCEEDED(hrRetCode))
		{
			//KG3DTexture* pTexture = NULL;
			//g_cTextureTable.GetResource(&pTexture, pSubMat->m_dwTextureIDs[0]);
			//if (pTexture)
			{
				//g_pd3dDevice->SetTexture(0, pTexture->GetTexture());
			}
			//SAFE_RELEASE(pTexture);
			if (pSubMat->m_lpTextures[0])
			{
				g_pd3dDevice->SetTexture(0, pSubMat->m_lpTextures[0]->GetTexture());
			}
			pSkinShader->CommitChanges();
			hrRetCode = pRenderMesh->DrawSubset(i);
		}
	}
	hrResult = S_OK;
Exit0:
	if (pSkinShader)
	{
		hrRetCode = pSkinShader->EndPass();
	}
	return hrResult;
}
HRESULT KG3DMesh::Shader_RenderSkinMeshDepth_End()
{
	HRESULT hrResult = E_FAIL;
	HRESULT hrRetCode  = E_FAIL;
	LPD3DXEFFECT pSkinShader = g_cGraphicsTool.GetDefaultShader(DEFST_SKINMESH);

	KG_PROCESS_ERROR(pSkinShader);
	hrRetCode = pSkinShader->End();
	KG_COM_PROCESS_ERROR(hrRetCode);
	//pSkinShader->SetTexture("tConver", NULL);//<ConverMap Shared>
	hrResult = S_OK;
Exit0:
	g_pd3dDevice->SetVertexShader(NULL);
	g_pd3dDevice->SetPixelShader(NULL);

	return hrResult;
}
///////////////////////////////////////////////////////////
HRESULT KG3DMesh::Shader_RenderSkinMesh_Begin(BOOL bUseConverTexture,BOOL bUseDetail)
{
	HRESULT hrResult = E_FAIL;
	UINT uNumPass = 0;
	std::vector<KG3DRuntimeMaterial*>RunTimeMaterial;

	HRESULT hr = S_OK;
	LPD3DXEFFECT pSkinShader = g_cGraphicsTool.GetDefaultShader(DEFST_SKINMESH);
	KG_PROCESS_ERROR(pSkinShader);

    size_t uParamHandleSize = 0;
    KG3D_DEFST_SKINMESH_ParamHandle *pParamHandle = 
        (KG3D_DEFST_SKINMESH_ParamHandle *)g_cGraphicsTool.GetDefaultShaderParamHandleBuffer(DEFST_SKINMESH, &uParamHandleSize);
    if (NULL != pParamHandle)
    {
        KG_PROCESS_ERROR(uParamHandleSize == sizeof(KG3D_DEFST_SKINMESH_ParamHandle));

		//Get and set shader const
		//g_SetShadowInfo(pSkinShader);//<ShadowParamMoveToShared>

		hr = Helper_ShaderSetConverMap(pSkinShader, bUseConverTexture,TRUE,bUseDetail);

		hr = pSkinShader->Begin(&uNumPass,D3DXFX_DONOTSAVESTATE );//D3DXFX_DONOTSAVESTATE
		KG_COM_PROCESS_ERROR(hr);
    }	

	hrResult = S_OK;
Exit0:
	return hrResult;
}

static std::vector<RenderTwiceInfo> s_RenderTwiceStack;
//static std::vector<RenderTwiceInfo> s_RenderSpecialAlphaStack[DEFST_COUNT];
static std::vector<KG3DModel*> s_RenderSpecialAlpha;

static int s_nTwicePass = 0;
//static int s_nSpecialAlphaPass = 0;
static void PushBackRenderTwiceInfo(const RenderTwiceInfo& Input)
{
	Input.pModel->AddRef();
	Input.pMesh->AddRef();
	Input.pMaterial->AddRef();
	if (Input.pModel->m_lpParentModelST)
	{
		Input.pModel->m_lpParentModelST->AddRef();
	}
	s_RenderTwiceStack.push_back(Input);
}

void PushBackRenderSpecialAlphaModel(KG3DModel *pModel)
{
	pModel->AddRef();
	s_RenderSpecialAlpha.push_back(pModel);
}

void ClearRenderSpecialAlphaModel()
{
    for (size_t i = 0; i < s_RenderSpecialAlpha.size(); ++i)
    {
        s_RenderSpecialAlpha[i]->Release();
    }
    s_RenderSpecialAlpha.clear();
}

//void PushBackRenderSpecialAlphaInfo(DWORD dwShaderType, const RenderTwiceInfo &Input)
//{
//	Input.pModel->AddRef();
//	Input.pMesh->AddRef();
//	Input.pMaterial->AddRef();
//	if (Input.pModel->m_lpParentModelST)
//	{
//		Input.pModel->m_lpParentModelST->AddRef();
//	}
//	s_RenderSpecialAlphaStack[dwShaderType].push_back(Input);
//}

static D3DXVECTOR3 s_vec3ViewPoint;

bool Modelsort(KG3DModel* pA, KG3DModel* pB)
{
	D3DXVECTOR3 vec3A(pA->m_matWorld._41, pA->m_matWorld._42, pA->m_matWorld._43);
	D3DXVECTOR3 vec3B(pB->m_matWorld._41, pB->m_matWorld._42, pB->m_matWorld._43);
	return D3DXVec3Length(&(vec3A - s_vec3ViewPoint)) > D3DXVec3Length(&(vec3B - s_vec3ViewPoint));
}


namespace {

    __forceinline void SetWaterClipPane(KG3DModel* pModel, bool bUnder)
    {
        if (!pModel)
            return;

        KG3DSceneSceneEditor* pEditor = NULL;

        if (g_cGraphicsTool.GetCurScene())
            pEditor = g_cGraphicsTool.GetCurScene()->GetSceneSceneEditor();

        if (!pEditor || !pEditor->m_lpSceneOutDoorSpaceMgr)
            return;

        D3DXPLANE vFFP;
        D3DXPLANE vPP;
        float fWaterHeight = 0.f;

        pEditor->m_lpSceneOutDoorSpaceMgr->GetWaterHeight(fWaterHeight, pModel->m_matWorld._41, pModel->m_matWorld._43);
        pEditor->GetWaterClipPane(&vFFP, &vPP, fWaterHeight, bUnder);
        g_pd3dDevice->SetClipPlane(0, vPP);

    }
}

void g_RenderSpecailAlpha()
{
	D3DXMATRIX matView;
	g_pd3dDevice->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, NULL, &matView);
	s_vec3ViewPoint = D3DXVECTOR3(matView._41, matView._42, matView._43);

	sort(s_RenderSpecialAlpha.begin(), s_RenderSpecialAlpha.end(), Modelsort);

	for (size_t i = 0; i < s_RenderSpecialAlpha.size(); i++)
	{
		s_RenderSpecialAlpha[i]->RenderSpecialAlpha(0, NULL);
	}
}

void g_RenderRenderTwiceInfo(bool bUnderWater, bool bClear)
{
    GraphicsStruct::RenderState clipEnable(D3DRS_CLIPPLANEENABLE, D3DCLIPPLANE0);

	if (g_cEngineOption.bEnableSpecialAlphaBlend)
	{
		s_nTwicePass = 1;
		for (size_t i = 0; i < s_RenderTwiceStack.size(); i++)
		{
            const RenderTwiceInfo& CurInfo = s_RenderTwiceStack[i];

            if (!CurInfo.pModel || !CurInfo.pMesh || !CurInfo.pMaterial)
                continue;

            SetWaterClipPane(CurInfo.pModel, bUnderWater);

			CurInfo.pMesh->SetModel(CurInfo.pModel);
			if (i == 0)
			{
				CurInfo.pMesh->Shader_RenderSkinMesh_Begin(TRUE,CurInfo.pModel->IsClientAdd());
			}
			CurInfo.pMesh->Shader_RenderSkinMesh_NoChange(CurInfo.pmatBones,
				CurInfo.pMaterial,
				CurInfo.fAlpha,
				CurInfo.dwOption,
				CurInfo.pModel->HasDetail());
			CurInfo.pMesh->SetModel(NULL);
            
			if (i == s_RenderTwiceStack.size() - 1)
			{
				CurInfo.pMesh->Shader_RenderSkinMesh_End();
			}

            if (bClear)
            {
                if (CurInfo.pModel->m_lpParentModelST)
                    CurInfo.pModel->m_lpParentModelST->Release();
                CurInfo.pModel->Release();
                CurInfo.pMesh->Release();
                CurInfo.pMaterial->Release();
            }
		}
	}
	s_nTwicePass = 0;

    if (bClear)
	    s_RenderTwiceStack.clear();
}




void g_RenderRenderTwiceInfo()
{
    if (g_cEngineOption.bEnableSpecialAlphaBlend)
    {
        s_nTwicePass = 1;
        for (size_t i = 0; i < s_RenderTwiceStack.size(); i++)
        {
            const RenderTwiceInfo& CurInfo = s_RenderTwiceStack[i];

            if (!CurInfo.pModel || !CurInfo.pMesh || !CurInfo.pMaterial)
                continue;

            CurInfo.pMesh->SetModel(CurInfo.pModel);
            if (i == 0)
            {
                CurInfo.pMesh->Shader_RenderSkinMesh_Begin(TRUE,CurInfo.pModel->IsClientAdd());
            }
            CurInfo.pMesh->Shader_RenderSkinMesh_NoChange(CurInfo.pmatBones,
                CurInfo.pMaterial,
                CurInfo.fAlpha,
                CurInfo.dwOption,
                CurInfo.pModel->HasDetail());
            CurInfo.pMesh->SetModel(NULL);

            if (i == s_RenderTwiceStack.size() - 1)
            {
                CurInfo.pMesh->Shader_RenderSkinMesh_End();
            }

            if (CurInfo.pModel->m_lpParentModelST)
                CurInfo.pModel->m_lpParentModelST->Release();
            CurInfo.pModel->Release();
            CurInfo.pMesh->Release();
            CurInfo.pMaterial->Release();

        }
    }
    s_nTwicePass = 0;
    s_RenderTwiceStack.clear();

    g_RenderSpecailAlpha();
}

void g_ClearRenderTwiceInfo()
{
	if (g_cEngineOption.bEnableSpecialAlphaBlend)
	{
		for (size_t i = 0; i < s_RenderTwiceStack.size(); i++)
		{
            const RenderTwiceInfo& CurInfo = s_RenderTwiceStack[i];
            
            if (CurInfo.pMesh)
			    CurInfo.pMesh->SetModel(NULL);

            if (CurInfo.pModel->m_lpParentModelST)
                CurInfo.pModel->m_lpParentModelST->Release();
            CurInfo.pModel->Release();
            CurInfo.pMesh->Release();
            CurInfo.pMaterial->Release();
		}
	}
	s_RenderTwiceStack.clear();
	for (size_t i = 0; i < s_RenderSpecialAlpha.size(); i++)
	{
		KG3DModel *pModel = s_RenderSpecialAlpha[i];
		SAFE_RELEASE(pModel);
	}
	s_RenderSpecialAlpha.clear();
}


HRESULT KG3DMesh::PrepareMatrixForSkinMeshRender(int& nNumBone,const D3DXMATRIX* matTemp[],DWORD dwOption,int i,D3DXMATRIX* pmatBones,D3DXMATRIX* pIdentify)
{
	D3DXBONECOMBINATION* pBoneComb = NULL;

	pBoneComb = static_cast<D3DXBONECOMBINATION*>(m_pBoneCombinationTable->GetBufferPointer());
	KG_PROCESS_ERROR(pBoneComb);

	if (dwOption & RENDER_BIP)
	{
		nNumBone = min(nNumBone, (int)(m_pModel->m_Mesh2Bip.size()));

		for (int k = 0; k < nNumBone; k++)
		{
			DWORD dwBoneIndex = pBoneComb[i].BoneId[k];
			if (dwBoneIndex != UINT_MAX)
			{
				int nMapedBoneIndex = m_pModel->m_Mesh2Bip[dwBoneIndex];
				if (nMapedBoneIndex != -1)
				{
					matTemp[k] = &(pmatBones[nMapedBoneIndex]);
				}
				else
				{
					matTemp[k] = &(m_pModel->m_pBoneMatricesForRender[dwBoneIndex]);
				}
			}
			else
			{
				matTemp[k] = pIdentify;
			}
		}
	}
	else
	{
		for (int k = 0; k < nNumBone; k++)
		{
			DWORD dwBoneIndex = pBoneComb[i].BoneId[k];
			if (dwBoneIndex != UINT_MAX)
			{
				matTemp[k] = &pmatBones[dwBoneIndex];
			}
			else
			{
				matTemp[k] = pIdentify;
			}

		}
	}

	return S_OK;
Exit0:
	return E_FAIL;
}

static bool s_bPushed = false;
HRESULT KG3DMesh::Shader_RenderSkinMesh_NoChange(D3DXMATRIX* pmatBones, 
                                        KG3DMaterial* pMaterial, 
										float fAlpha,
										DWORD dwOption,BOOL bUseDetail)
{
	HRESULT hrResult = E_FAIL;
	HRESULT hrRetCode = S_OK;

    D3DXMATRIX matView;
    D3DXMATRIX matProj;
    D3DXMATRIX matWorld;
    D3DXMATRIX matViewProj;
    D3DXVECTOR3 vEyes;

    LPD3DXEFFECT pSkinShader = g_cGraphicsTool.GetDefaultShader(DEFST_SKINMESH);
    unsigned uParamHandleSize = 0;
    KG3D_DEFST_SKINMESH_ParamHandle *pParamHandle = 
        (KG3D_DEFST_SKINMESH_ParamHandle *)g_cGraphicsTool.GetDefaultShaderParamHandleBuffer(DEFST_SKINMESH, &uParamHandleSize);
	KG_PROCESS_ERROR(NULL != pParamHandle);
	KG_PROCESS_ERROR(uParamHandleSize == sizeof(KG3D_DEFST_SKINMESH_ParamHandle));

    KG3DCamera* pCamera = g_cGraphicsTool.GetCamera();
    const D3DXMATRIX* matTemp[KG3DGraphicsTool::MAX_NUM_BONE_PER_SET_SHADER_2];

	D3DXBONECOMBINATION* pBoneComb = NULL;
	LPD3DXMESH pRenderMesh = GetRenderSkinMesh();

    KG3DRuntimeMaterial* pRuntimeMaterial = NULL;
	KG_PROCESS_ERROR(pCamera);
	KG_PROCESS_ERROR(pMaterial);
	KG_PROCESS_ERROR(pRenderMesh);
	KG_PROCESS_ERROR(pmatBones);
	KG_PROCESS_ERROR(pSkinShader);
	KG_PROCESS_ERROR(m_pBoneCombinationTable);
	KG_PROCESS_ERROR(m_pModel);
	KG_PROCESS_ERROR(pMaterial->IsLoaded());

	pMaterial->SetShaderType(KG3DMaterial::MAT_SHADER_TYPE_SKIN);
	
	pBoneComb = static_cast<D3DXBONECOMBINATION*>(m_pBoneCombinationTable->GetBufferPointer());
	KG_PROCESS_ERROR(pBoneComb);

    KG3DMaterial::KG3DMaterialSubset* pSubMat = pMaterial->GetSubMaterial(0);
	KGLOG_PROCESS_ERROR(pSubMat);

	matWorld = m_pModel->m_matWorld;
	s_bPushed = false;
	g_pd3dDevice->GetTransform(D3DTS_VIEW, &matView);
	g_pd3dDevice->GetTransform(D3DTS_PROJECTION, &matProj);

	matViewProj = matWorld * matView * matProj;


#if 0
	struct SkinShaderHandles 
	{
		D3DXHANDLE hNumBlend;
		D3DXHANDLE hWorldViewProj;
		D3DXHANDLE hWorld;
	};

	static LPCTSTR s_SkinShaderHandleStrings[] = 
	{
		_T("nNumBlend"),
		_T("WorldViewProj"),
		_T("matWorld"),
	};

	SkinShaderHandles* pHandles = NULL;
	LPD3DXEFFECT pSkinShader = g_GetDefaultShaderGroup().GetDefaultEffectAndHandles(DEFST_SKINMESH, s_SkinShaderHandleStrings, _countof(s_SkinShaderHandleStrings), (LPVOID*)&pHandles, sizeof(*pHandles));

	KG_PROCESS_ERROR(NULL != pSkinShader);
	_ASSERTE(NULL != pSkinShader);
	hrRetCode = pSkinShader->SetInt(pHandles->hNumBlend, m_dwMaxFaceInfl);
	hrRetCode = pSkinShader->SetMatrix(pHandles->hWorldViewProj, &matViewProj);
	hrRetCode = pSkinShader->SetMatrix(pHandles->hWorld, &matWorld);
	
#endif

    KG_SHADER_SET(pSkinShader, SetInt, nNumBlend, m_dwMaxFaceInfl, pParamHandle, hrRetCode);
    KG_COM_PROCESS_ERROR(hrRetCode);

	KG_SHADER_SET(pSkinShader, SetMatrix, WorldViewProj, &matViewProj, pParamHandle, hrRetCode);
    KG_COM_PROCESS_ERROR(hrRetCode);

	KG_SHADER_SET(pSkinShader, SetMatrix, matWorld,      &matWorld,    pParamHandle, hrRetCode);
    KG_COM_PROCESS_ERROR(hrRetCode);

	pCamera->GetPosition(&vEyes);
    KG_SHADER_SET_VALUE(pSkinShader, SetValue, eyes, &vEyes, sizeof(D3DXVECTOR3), pParamHandle, hrRetCode);
    KG_COM_PROCESS_ERROR(hrRetCode);

	IEKG3DScene* pScene = g_cGraphicsTool.GetScene();
	KG_PROCESS_ERROR(pScene);

	//////////////////////////////////////////////////////////////////////////
	DWORD dwPassID = static_cast<int>(pSubMat->m_dwPassID);
	if(bUseDetail)
	{
		//g_SetMaterialDetailInfoFast(pSkinShader,m_pModel, &(pParamHandle->MaterialDetailInfo));
        g_SetMaterialDetailInfo(pSkinShader,m_pModel);//<DetailInfoParamMove>
	}

	if(m_pModel->HasDetail())
	{
		dwPassID = 0;
	}
	hrRetCode = pSkinShader->BeginPass(dwPassID);
	KG_COM_PROCESS_ERROR(hrRetCode);
	
	for (DWORD i = 0; i < m_dwNumAttributeGroups; i++)
	{
		if (pBoneComb[i].AttribId >= pMaterial->m_dwNumMaterials)
			continue;

        KG3DMaterial::KG3DMaterialSubset* pSubMat = pMaterial->GetSubMaterial(pBoneComb[i].AttribId);
		KGLOG_PROCESS_ERROR(pSubMat);
		
		//if(0)
		{
			pRuntimeMaterial = m_pModel->GetRuntimeMaterial();
			KG_PROCESS_ERROR(pRuntimeMaterial);

			_MtlAlphaOperation* pAlphaOp = pSubMat->GetAlphaOperation();
			BOOL bEnableAlphaFactor = !(pAlphaOp->EnableAlphaTest || pAlphaOp->EnableAlphaBlend);

			_MtlRenderTwiceAlphaOption *pTwiceOption = pSubMat->GetAlphaRenderTwice();
			if (pTwiceOption->bEnable)
			{
				bEnableAlphaFactor = FALSE;
			}

			if (g_cEngineOption.bEnableSpecialAlphaBlend)
			{
				//在Alpha值等于1的时候才使用两次渲染的alpha blend方式
				_MtlRenderTwiceAlphaOption *pTwiceOption = pSubMat->GetAlphaRenderTwice();
				if (s_nTwicePass == 1 && (!pTwiceOption->bEnable))
					continue;

				if (pTwiceOption->bEnable)
				{
					if (s_nTwicePass == 0)
					{
						pRuntimeMaterial->SetRenderState(D3DRS_ALPHAREF, pTwiceOption->dwAlphaRefPass0);
						pRuntimeMaterial->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
						pRuntimeMaterial->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
						pRuntimeMaterial->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
						float fLod = 1;
						m_pModel->GetLOD(fLod);

						if (fLod >= RENDERTWICE_CULL_LODLEVEL &&  !s_bPushed)
						{
							RenderTwiceInfo Info;
							Info.dwOption = dwOption;
							Info.fAlpha = fAlpha;
							Info.pmatBones = pmatBones;
							Info.pMaterial = pMaterial;
							Info.pMesh = this;
							Info.pModel = m_pModel;
							PushBackRenderTwiceInfo(Info);
							s_bPushed = true;
						}
						pRuntimeMaterial->Enable(TRUE);
					}
					else
					{
						//pRuntimeMaterial->SetRenderState(D3DRS_ALPHAREF, pTwiceOption->dwAlphaRefPass0);
						//pRuntimeMaterial->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_LESS);
						pRuntimeMaterial->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
						pRuntimeMaterial->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
						pRuntimeMaterial->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
						pRuntimeMaterial->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
						pRuntimeMaterial->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
						pRuntimeMaterial->Enable(TRUE);
					}
				}
			}

			KG_SHADER_SET(pSkinShader, SetBool, bAlphaUseFactor, bEnableAlphaFactor, pParamHandle, hrRetCode);
			pSubMat->SetMaterial(fAlpha, dwOption);
			pRuntimeMaterial->BeginRender();
		}

		D3DMATERIAL9 materialCurrent;
		g_pd3dDevice->GetMaterial(&materialCurrent);
       
        /*KG_SHADER_SET_VALUE(pSkinShader, SetValue, spe_exp, &(materialCurrent.Power), sizeof(float), pParamHandle, hrRetCode);
		KG_SHADER_SET_VALUE(pSkinShader, SetValue, color_cast, &(pSubMat->m_ColorCast[m_pModel->GetDefaultColorCastIndex()]), sizeof(D3DCOLORVALUE), pParamHandle, hrRetCode);

		KG_SHADER_SET_VALUE(pSkinShader, SetValue, emssive_power, &pSubMat->m_fEmssPower, sizeof(float), pParamHandle, hrRetCode);
		KG_SHADER_SET(pSkinShader, SetBool, bhightlight, pSubMat->m_dwOption & MATERIAL_OPTION_SPECULARENABLE, pParamHandle, hrRetCode);*/

		//改成一次设进去
		KG3DShaderParamMaterialEx materialEx;
		materialEx.bhightlight = !!(pSubMat->m_dwOption & MATERIAL_OPTION_SPECULARENABLE);
		materialEx.color_cast = pSubMat->m_ColorCast[m_pModel->GetDefaultColorCastIndex()];
		materialEx.emssive_power = pSubMat->m_fEmssPower;
		materialEx.spe_exp = materialCurrent.Power;
		pSkinShader->SetValue("materialEx", &materialEx, sizeof(materialEx));

		//注意顺序，这里改了material
		materialCurrent.Power=pSubMat->m_fSpecPower;
		KG_SHADER_SET_VALUE(pSkinShader, SetValue, materialCur, &materialCurrent, sizeof(materialCurrent), pParamHandle, hrRetCode);

     
        g_SetPointLighting(pSkinShader, m_pModel->GetPointLightInfo(), MAX_POINTLIGHT, false);

		switch(dwPassID)
		{
		case PASS_SkinShader_old:
			{
				hrRetCode = g_pd3dDevice->SetTexture(1, NULL);
			}
			break;
		case PASS_SkinShader_spec:
		case PASS_SkinShader_diff:
			break;
		case PASS_SkinShader_emap:
		case PASS_SkinShader_all:
			{
				if (pScene)
					g_pd3dDevice->SetTexture(2, pScene->GetEnvMap(NULL, 0));
			}
			break;
		}

		int nNumBone = min(KG3DGraphicsTool::m_nMaxNumBone, static_cast<int>(m_dwNumBone));
		PrepareMatrixForSkinMeshRender(nNumBone,matTemp,dwOption,i,pmatBones,&g_stMatIdentity);
		ASSERT(nNumBone);

        hrRetCode = pSkinShader->SetMatrixPointerArray("matBones", (const D3DXMATRIX**)&matTemp, nNumBone);
	    if (SUCCEEDED(hrRetCode))
	    {
		    KG3DTexture* pTexture = pSubMat->m_lpTextures[0];
		    if (pTexture)
		    {
			    g_pd3dDevice->SetTexture(0, pTexture->GetTexture());
		    }
		    pSkinShader->CommitChanges();
		    hrRetCode = pRenderMesh->DrawSubset(i);
	    }

		//if(0)
		{
			pRuntimeMaterial->EndRender();
			pSubMat->RestoreMaterial();
		}
	}

	hrResult = S_OK;
Exit0:
	if (pSkinShader)
	{
		hrRetCode = pSkinShader->EndPass();
	}
	return hrResult;
}

HRESULT KG3DMesh::Shader_RenderSkinMesh_End()
{
	HRESULT hrResult = E_FAIL;
	HRESULT hrRetCode  = E_FAIL;
	LPD3DXEFFECT pSkinShader = g_cGraphicsTool.GetDefaultShader(DEFST_SKINMESH);

	KG_PROCESS_ERROR(pSkinShader);
	hrRetCode = pSkinShader->End();
	KG_COM_PROCESS_ERROR(hrRetCode);

    //pSkinShader->SetTexture("tConver", NULL);//<ConverMap Shared>

	hrResult = S_OK;
Exit0:
	g_pd3dDevice->SetVertexShader(NULL);
	g_pd3dDevice->SetPixelShader(NULL);

	return hrResult;
}

HRESULT KG3DMesh::Shader_RenderSkinMesh_SingleSubset(D3DXMATRIX *pmatBones, KG3DMaterial *pMaterial, DWORD dwOption, BOOL bDetail)
{
    HRESULT hrResult = E_FAIL;
    HRESULT hrRetCode = E_FAIL;
    KG3DMaterial::KG3DMaterialSubset *pSubMat = NULL;
    LPD3DXEFFECT pSkinShader = NULL;
    void *pvShanderParamHandleBuffer = NULL;
    KG3D_DEFST_SKINMESH_ParamHandle *pParamHandle = NULL;
    unsigned uParamSize = 0;
    D3DXBONECOMBINATION *pBoneComb = NULL;
    LPD3DXMESH pRenderMesh = NULL;
    DWORD dwPassID = 0;
    KG3DRuntimeMaterial *pRuntimeMaterial = NULL;
    D3DXMATRIX matView;
    D3DXMATRIX matProj;
    D3DXMATRIX matWorld;
    D3DXMATRIX matViewProj;
    const D3DXMATRIX* matTemp[KG3DGraphicsTool::MAX_NUM_BONE_PER_SET_SHADER_2];
    IDirect3DBaseTexture9 *pTexture1Save = NULL;
    IDirect3DBaseTexture9 *pTexture2Save = NULL;
    BOOL bTexture1Save = FALSE;
    BOOL bTexture2Save = FALSE;
    KG3DShaderParamMaterialEx materialEx;
    D3DMATERIAL9 materialCurrent;

    KGLOG_PROCESS_ERROR(pmatBones);
    KGLOG_PROCESS_ERROR(pMaterial);
    KGLOG_PROCESS_ERROR(m_dwNumAttributeGroups == 1);

    pMaterial->SetShaderType(KG3DMaterial::MAT_SHADER_TYPE_SKIN);

    pRuntimeMaterial = m_pModel->GetRuntimeMaterial();
    KGLOG_PROCESS_ERROR(pRuntimeMaterial);

    pBoneComb = (D3DXBONECOMBINATION *)m_pBoneCombinationTable->GetBufferPointer();
    KGLOG_PROCESS_ERROR(pBoneComb);

    pSubMat = pMaterial->GetSubMaterial(0);
    KGLOG_PROCESS_ERROR(pSubMat);

    dwPassID = m_pModel->HasDetail() ? 0 : pSubMat->m_dwPassID;

    pSubMat = pMaterial->GetSubMaterial(pBoneComb[0].AttribId);
    KGLOG_PROCESS_ERROR(pSubMat);

    pSkinShader = g_cGraphicsTool.GetDefaultShader(DEFST_SKINMESH);
    KGLOG_PROCESS_ERROR(pSkinShader);

    pvShanderParamHandleBuffer = g_cGraphicsTool.GetDefaultShaderParamHandleBuffer(DEFST_SKINMESH, &uParamSize);
    KGLOG_PROCESS_ERROR(pvShanderParamHandleBuffer);
    KGLOG_PROCESS_ERROR(uParamSize == sizeof(KG3D_DEFST_SKINMESH_ParamHandle));

    pParamHandle = (KG3D_DEFST_SKINMESH_ParamHandle *)pvShanderParamHandleBuffer;
    ASSERT(pParamHandle);

    matWorld = m_pModel->m_matWorld;
    g_pd3dDevice->GetTransform(D3DTS_VIEW, &matView);
    g_pd3dDevice->GetTransform(D3DTS_PROJECTION, &matProj);

    matViewProj = matWorld * matView * matProj;

    KG_SHADER_SET(pSkinShader, SetInt, nNumBlend, m_dwMaxFaceInfl, pParamHandle, hrRetCode);
    KG_COM_PROCESS_ERROR(hrRetCode);

    KG_SHADER_SET(pSkinShader, SetMatrix, WorldViewProj, &matViewProj, pParamHandle, hrRetCode);
    KG_COM_PROCESS_ERROR(hrRetCode);

    KG_SHADER_SET(pSkinShader, SetMatrix, matWorld,      &matWorld,    pParamHandle, hrRetCode);
    KG_COM_PROCESS_ERROR(hrRetCode);

    g_pd3dDevice->GetMaterial(&materialCurrent);

    materialEx.bhightlight = !!(pSubMat->m_dwOption & MATERIAL_OPTION_SPECULARENABLE);
    materialEx.color_cast = pSubMat->m_ColorCast[m_pModel->GetDefaultColorCastIndex()];
    materialEx.emssive_power = pSubMat->m_fEmssPower;
    materialEx.spe_exp = materialCurrent.Power;
    pSkinShader->SetValue("materialEx", &materialEx, sizeof(materialEx));

    if (bDetail)
    {
        g_SetMaterialDetailInfo(pSkinShader, m_pModel);
    }

    pRenderMesh = GetRenderSkinMesh();
    KGLOG_PROCESS_ERROR(pRenderMesh);

    if (g_cEngineOption.bEnableSpecialAlphaBlend)
    {
        //在Alpha值等于1的时候才使用两次渲染的alpha blend方式
        _MtlRenderTwiceAlphaOption *pTwiceOption = pSubMat->GetAlphaRenderTwice();
        KG_PROCESS_SUCCESS(s_nTwicePass == 1 && (!pTwiceOption->bEnable));

        if (pTwiceOption->bEnable)
        {
            if (s_nTwicePass == 0)
            {
                pRuntimeMaterial->SetRenderState(D3DRS_ALPHAREF, pTwiceOption->dwAlphaRefPass0);
                pRuntimeMaterial->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
                pRuntimeMaterial->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
                float fLod = 1;
                m_pModel->GetLOD(fLod);

                if (fLod >= RENDERTWICE_CULL_LODLEVEL)
                {
                    RenderTwiceInfo Info;
                    Info.dwOption = dwOption;
                    Info.fAlpha = 1;
                    Info.pmatBones = pmatBones;
                    Info.pMaterial = pMaterial;
                    Info.pMesh = this;
                    Info.pModel = m_pModel;
                    PushBackRenderTwiceInfo(Info);
                }
                pRuntimeMaterial->Enable(TRUE);
            }
            else
            {
                pRuntimeMaterial->SetRenderState(D3DRS_ALPHAREF, pTwiceOption->dwAlphaRefPass0);
                pRuntimeMaterial->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_LESSEQUAL);
                pRuntimeMaterial->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
                pRuntimeMaterial->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
                pRuntimeMaterial->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
                pRuntimeMaterial->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
                pRuntimeMaterial->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
                pRuntimeMaterial->Enable(TRUE);
            }

            hrRetCode = pRuntimeMaterial->BeginRender();
            KGLOG_COM_PROCESS_ERROR(hrRetCode);
        }
    }
    
    g_SetPointLighting(pSkinShader, m_pModel->GetPointLightInfo(), MAX_POINTLIGHT, false);    

    switch ((int)dwPassID)
    {
    case PASS_SkinShader_old:
        {
            g_pd3dDevice->GetTexture(1, &pTexture1Save);
            if (pTexture1Save != NULL)
            {
                hrRetCode = g_pd3dDevice->SetTexture(1, NULL);

                bTexture1Save = TRUE;
            }
        }
        break;
    case PASS_SkinShader_spec:
    case PASS_SkinShader_diff:
        break;
    case PASS_SkinShader_emap:
    case PASS_SkinShader_all:
        {
            g_pd3dDevice->GetTexture(2, &pTexture2Save);

            IEKG3DScene* pScene = g_cGraphicsTool.GetScene();
            if (pScene)
            {
                IDirect3DBaseTexture9 *pTexture = pScene->GetEnvMap(NULL, 0);
                if (pTexture != pTexture2Save)
                {
                    g_pd3dDevice->SetTexture(2, pTexture);

                    bTexture2Save = TRUE;
                }
            }
        }
        break;
    }

    int nNumBone = min(KG3DGraphicsTool::m_nMaxNumBone, static_cast<int>(m_dwNumBone));
    hrRetCode = PrepareMatrixForSkinMeshRender(nNumBone, matTemp, dwOption, 0, pmatBones, &g_stMatIdentity);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    hrRetCode = pSkinShader->SetMatrixPointerArray("matBones", (const D3DXMATRIX**)&matTemp, nNumBone);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    pSkinShader->BeginPass(dwPassID);

    pRenderMesh->DrawSubset(0);

    pSkinShader->EndPass();

Exit1:
    hrResult = S_OK;
Exit0:
    if (bTexture2Save)
    {
        g_pd3dDevice->SetTexture(2, pTexture2Save);
        bTexture2Save = FALSE;
    }

    SAFE_RELEASE(pTexture2Save);

    if (bTexture1Save)
    {
        g_pd3dDevice->SetTexture(1, pTexture1Save);
        bTexture1Save = FALSE;
    }

    SAFE_RELEASE(pTexture1Save);

    if (pRuntimeMaterial)
    {
        pRuntimeMaterial->EndRender();
    }
    return hrResult;
}

//////////////////////////////////////////////////////////////////////////
HRESULT KG3DMesh::Shader_RenderSkinMesh(D3DXMATRIX* pmatBones, 
										KG3DMaterial* pMaterial, 
										float fAlpha,
										DWORD dwOption)
{
	HRESULT hrResult = E_FAIL;
	HRESULT hrRetCode = E_FAIL;

	KG_PROCESS_ERROR(m_pModel);
	
	BOOL bdetail = m_pModel->IsClientAdd() || m_pModel->HasDetail();
	hrRetCode = Shader_RenderSkinMesh_Begin(FALSE, bdetail);
	KG_COM_PROCESS_ERROR(hrRetCode);

	hrRetCode = Shader_RenderSkinMesh_NoChange(pmatBones,pMaterial,fAlpha,dwOption,bdetail);
	KG_COM_PROCESS_ERROR(hrRetCode);

	hrResult = S_OK;
Exit0:
    Shader_RenderSkinMesh_End();
	return hrResult;
}


inline LPD3DXMESH KG3DMesh::GetRenderSkinMesh()
{
	if (m_dwOption & MESH_PROGRESS)
	{
		return (LPD3DXMESH)m_pMeshProgress;
	}
	else
	{
		return m_ppMeshes[SMBT_BLENDED];
	}
}

//HRESULT KG3DMesh::Shader_DrawMeshAmbientMap_Begin(LPD3DXMESH pRenderMesh,
//											float fAlpha,
//											DWORD dwOption,BOOL bUseDetail)
//{
//	HRESULT hrResult = E_FAIL;
//	HRESULT hrRetCode = E_FAIL;
//	LPD3DXEFFECT pShader = NULL;
//
//	if(bUseDetail)
//	{
//		pShader = g_cGraphicsTool.GetDefaultShader(DEFST_NEWLIGHT_DETAIL);
//	}
//	else
//	{
//		pShader = g_cGraphicsTool.GetDefaultShader(DEFST_NEWLIGHT);
//	}
//	KG_PROCESS_ERROR(pShader);
//
//	UINT uNumPass = 0;
//	KG_PROCESS_ERROR(pRenderMesh);
//
//	//if (g_cEngineOption.bAmbientShaderOnly)
//	//{
//	//	ShaderType = DEFST_AMBIENT_ONLY;
//	//}
//	//
//	//pShader = g_cGraphicsTool.GetDefaultShader(ShaderType); 
//	//KG_PROCESS_ERROR(pShader);
//
//
//	//g_SetShadowInfo(pShader);//<ShadowParamMoveToShared>
//	hrRetCode = Helper_ShaderSetConverMap(pShader, TRUE,FALSE,bUseDetail);
//	hrRetCode = pShader->Begin(&uNumPass, 0);
//	hrRetCode = pShader->BeginPass(0);
//	
//	hrResult = S_OK;
//Exit0:
//	return hrResult;
//	
//}
//
//HRESULT KG3DMesh::Shader_DrawMeshAmbientMap_NoChange(LPD3DXMESH pRenderMesh,
//												  KG3DMaterial* pMaterial,
//												  float fAlpha,
//												  DWORD dwOption)
//{
//	HRESULT hr = E_FAIL;
//	HRESULT hrResult = E_FAIL;
//	D3DXMATRIX matWorld;
//	D3DXMATRIX matView;
//	D3DXMATRIX matProjection;
//	D3DXMATRIX matWorldViewProj;
//	D3DXMATRIX matWorldView;
//	LPD3DXEFFECT pShader = NULL;
//	KG3DTexture* pTexture = NULL;
//	D3DMATERIAL9 matCurrent;
//
//	KG3DDefaultShaderType ShaderType = DEFST_AMBIENT;
//	
//	KG_PROCESS_ERROR(pRenderMesh);
//	KG_PROCESS_ERROR(m_pModel);
//	KG_PROCESS_ERROR(pMaterial);
//	
//	if (g_cEngineOption.bAmbientShaderOnly)
//	{
//		ShaderType = DEFST_AMBIENT_ONLY;
//	}
//	pShader = g_cGraphicsTool.GetDefaultShader(ShaderType); 
//	KG_PROCESS_ERROR(pShader);
//
//	hr = g_pd3dDevice->GetTransform(D3DTS_WORLD, &matWorld);
//	hr = g_pd3dDevice->GetTransform(D3DTS_VIEW, &matView);
//	hr = g_pd3dDevice->GetTransform(D3DTS_PROJECTION, &matProjection);
//
//	matWorldViewProj = matWorld * matView * matProjection;
//	matWorldView = matWorld * matView;
//
//	hr = pShader->SetMatrix("View", &matView);
//	hr = pShader->SetMatrix("WorldViewProj", &matWorldViewProj);
//	hr = pShader->SetMatrix("WorldView", &matWorldView);
//
//
//    if (m_pModel && g_ProjTextureRender.NeedProject(m_pModel->m_BBox))
//        g_ProjTextureRender.SetEffectParam(pShader);
//	
//	for (DWORD i = 0; i < m_dwNumSubset; i++)
//	{
//	
//		KG3DMaterial::KG3DMaterialSubset* pSubMat = pMaterial->GetSubMaterial(i);
//		KGLOG_PROCESS_ERROR(pSubMat);
//	
//		hr = pSubMat->SetMaterial(fAlpha, dwOption);
//		KG_COM_PROCESS_ERROR(hr);
//	
//		KG3DRuntimeMaterial* pRuntime = m_pModel->GetRuntimeMaterial();
//		ASSERT(pRuntime);
//		
//		pRuntime->BeginRender();
//		
//		g_pd3dDevice->GetMaterial(&matCurrent);
//		
//		/*
//		hr = pShader->SetValue("m_emissive", &matCurrent.Emissive, sizeof(D3DCOLORVALUE));
//		hr = pShader->SetValue("m_diffuse", &matCurrent.Diffuse, sizeof(D3DCOLORVALUE));
//		hr = pShader->SetValue("m_ambient", &matCurrent.Ambient, sizeof(D3DCOLORVALUE));
//		hr = pShader->SetValue("m_specular", &matCurrent.Specular, sizeof(D3DCOLORVALUE));
//		hr = pShader->SetValue("m_shinness", &matCurrent.Power, sizeof(float));
//		*/
//		//上面的改成下面的一次设进去,m_shinness其实就是Power，所以在Shader里面改成用materialCur.specPower
//		hr = pShader->SetValue("materialCur", &matCurrent, sizeof(matCurrent));
//
//		
//		
//		for (int j = 0; j < 2; j++)
//		{
//			pTexture = pSubMat->m_lpTextures[j];
//			if(pTexture)
//			{
//				g_pd3dDevice->SetTexture(j,pTexture->GetTexture());
//			}
//		}
//
//		hr = pShader->CommitChanges();
//		hr = pRenderMesh->DrawSubset(i);
//
//		pRuntime->EndRender();
//        pSubMat->RestoreMaterial();
//	}		
//
//	
//	hrResult = S_OK;
//Exit0:
//    pShader->SetTexture("LightMap",NULL);
//    pShader->SetTexture("SelTexture", NULL);
//	return hrResult;
//}
//
//HRESULT KG3DMesh::Shader_DrawMeshAmbientMap_End(LPD3DXMESH pRenderMesh,
//												  float fAlpha,
//												  DWORD dwOption)
//{
//	HRESULT hrRetCode = E_FAIL;
//	HRESULT hrResult = E_FAIL;
//	LPD3DXEFFECT pShader = NULL;
//
//	KG3DDefaultShaderType ShaderType = DEFST_AMBIENT;
//
//	if (g_cEngineOption.bAmbientShaderOnly)
//	{
//		ShaderType = DEFST_AMBIENT_ONLY;
//	}
//	pShader = g_cGraphicsTool.GetDefaultShader(ShaderType); 
//	KG_PROCESS_ERROR(pShader);
//
//	hrRetCode = pShader->EndPass();
//	hrRetCode = pShader->End();
//
//    //pShader->SetTexture("tConver", NULL);//<ConverMap Shared>
//
//	hrResult = S_OK;
//Exit0:
//	return hrResult;
//}


//HRESULT KG3DMesh::Shader_DrawMeshAmbientMap(LPD3DXMESH pRenderMesh,
//											KG3DMaterial* pMaterial,
//											float fAlpha,
//											DWORD dwOption,BOOL bUseDetail)
//{
//	HRESULT hr = E_FAIL;
//
//	hr = Shader_DrawMeshAmbientMap_Begin(pRenderMesh,fAlpha,dwOption,bUseDetail);
//	KG_COM_PROCESS_ERROR(hr);
//
//	hr = Shader_DrawMeshAmbientMap_NoChange(pRenderMesh,pMaterial,fAlpha,dwOption);
//
//	hr = Shader_DrawMeshAmbientMap_End(pRenderMesh,fAlpha,dwOption);
//	KG_COM_PROCESS_ERROR(hr);
//
//Exit0:
//	return hr;
//}

HRESULT KG3DMesh::Shader_DrawMeshNormalNoSkin(LPD3DXMESH pRenderMesh,
                                              KG3DMaterial* pMaterial,
											  float fAlpha,
											  DWORD dwOption)
{
	HRESULT hr = E_FAIL;

	hr = Shader_DrawMeshNormalNoSkin_Begin();
	KG_COM_PROCESS_ERROR(hr);

	hr = Shader_DrawMeshNormalNoSkin_NoChange(pRenderMesh,pMaterial,fAlpha,dwOption);

	hr = Shader_DrawMeshNormalNoSkin_End();
	KG_COM_PROCESS_ERROR(hr);

Exit0:
	return hr;
}

HRESULT KG3DMesh::Shader_DrawMeshNormalNoSkin_Begin()
{
	HRESULT hrResult = E_FAIL;
	HRESULT hr = E_FAIL;
	LPD3DXEFFECT pShader = NULL;
	
	UINT uNumPass = 0;

	//KG_PROCESS_ERROR(pRenderMesh);

	pShader = g_cGraphicsTool.GetDefaultShader(DEFST_NORMAL_NOSKIN);
	KG_PROCESS_ERROR(pShader);

	//g_SetShadowInfo(pShader);//<ShadowParamMoveToShared>
	
	pShader->SetTechnique("TechNormalNoSkin");
	
	hr = pShader->Begin(&uNumPass, 0);
	KG_COM_PROCESS_ERROR(hr);

	hr = pShader->BeginPass(0);
	if(FAILED(hr))
	{
		pShader->End();
	}
	KG_COM_PROCESS_ERROR(hr);

	hrResult = S_OK;
Exit0:
	return hrResult;
}

HRESULT KG3DMesh::Shader_DrawMeshNormalNoSkin_NoChange(LPD3DXMESH pRenderMesh,KG3DMaterial* pMaterial, float fAlpha, DWORD dwOption)
{
	HRESULT hrResult = E_FAIL;
	HRESULT hr = E_FAIL;
	D3DXMATRIX matWorld, matView, matProj, matWorldViewProj, matWorldInverse, matViewInverse, matWorldView;
	LPD3DXEFFECT pShader = NULL;
    KG3DRuntimeMaterial* pRuntimeMaterial = NULL;

	KG_PROCESS_ERROR(pRenderMesh);
	KG_PROCESS_ERROR(pMaterial);
	KG_PROCESS_ERROR(m_pModel);

	pShader = g_cGraphicsTool.GetDefaultShader(DEFST_NORMAL_NOSKIN);
	KG_PROCESS_ERROR(pShader);

	hr = g_pd3dDevice->GetTransform(D3DTS_WORLD, &matWorld);
	hr = g_pd3dDevice->GetTransform(D3DTS_VIEW, &matView);
	hr = g_pd3dDevice->GetTransform(D3DTS_PROJECTION, &matProj);

	matWorldViewProj = matWorld * matView * matProj;
	matWorldView = matWorld * matView;
	D3DXMatrixInverse(&matWorldInverse, NULL, &matWorld);
	D3DXMatrixInverse(&matViewInverse, NULL, &matView);
	
	hr = pShader->SetMatrix("WorldViewProj", &matWorldViewProj);
	hr = pShader->SetMatrix("WorldInverse", &matWorldInverse);
	hr = pShader->SetMatrix("ViewInverse", &matViewInverse);
	hr = pShader->SetMatrix("World", &matWorld);
	hr = pShader->SetMatrix("WorldView", &matWorldView);

	for (DWORD i = 0; i < m_dwNumSubset; i++)
	{
		static D3DMATERIAL9 mat;
		KG3DMaterial::KG3DMaterialSubset* pSubMat = pMaterial->GetSubMaterial(i);
		KGLOG_PROCESS_ERROR(pSubMat);

		hr = pSubMat->SetMaterial(fAlpha, dwOption);
		if (hr == S_FALSE)
		{
			hr = S_OK;
			continue;
		}
		
		pRuntimeMaterial = m_pModel->GetRuntimeMaterial();
		ASSERT(pRuntimeMaterial);
				
		g_pd3dDevice->GetMaterial(&mat);
		
		/*hr = pShader->SetValue("m_emissive", &mat.Emissive, sizeof(D3DCOLORVALUE));
		hr = pShader->SetValue("m_diffuse", &mat.Diffuse, sizeof(D3DCOLORVALUE));
		hr = pShader->SetValue("m_ambient", &mat.Ambient, sizeof(D3DCOLORVALUE));
		hr = pShader->SetValue("m_specular", &mat.Specular, sizeof(D3DCOLORVALUE));
		hr = pShader->SetValue("m_shinness", &mat.Power, sizeof(float));
		*/
		//上面的改成下面的一次设进去,m_shinness其实就是Power，所以在Shader里面改成用materialCur.specPower
		hr = pShader->SetValue("materialCur", &mat, sizeof(mat));

		

		hr = pShader->CommitChanges();

		hr = pRenderMesh->DrawSubset(i);

		pSubMat->RestoreMaterial();

		pRuntimeMaterial->EndRender();
	}

	hrResult = S_OK;
Exit0:
	return hrResult;
}

HRESULT KG3DMesh::Shader_DrawMeshNormalNoSkin_End()
{
	HRESULT hr = S_OK;
	LPD3DXEFFECT pShader = NULL;

	pShader = g_cGraphicsTool.GetDefaultShader(DEFST_NORMAL_NOSKIN);
	KG_PROCESS_ERROR(pShader);

	hr = pShader->EndPass();
	hr = pShader->End();

Exit0:
	return hr;
}



static void GetFaceNormal(short* pIndices,
	                      BYTE* pVertices,
						  size_t Stride,
						  DWORD dwFaceIndex, 
						  D3DXVECTOR3& vec3FaceNormal)
{
	short sA, sB, sC;
	KG_PROCESS_ERROR(pIndices);
	KG_PROCESS_ERROR(pVertices);
	KG_PROCESS_ERROR(Stride > 0);

	sA = pIndices[dwFaceIndex * 3];
	sB = pIndices[dwFaceIndex * 3 + 1];
	sC = pIndices[dwFaceIndex * 3 + 2];

	D3DXVECTOR3* pA = (D3DXVECTOR3*)(pVertices + Stride * sA);
	D3DXVECTOR3* pB = (D3DXVECTOR3*)(pVertices + Stride * sB);
	D3DXVECTOR3* pC = (D3DXVECTOR3*)(pVertices + Stride * sC);

	D3DXVec3Cross(&vec3FaceNormal,
		&((*pB) - (*pA)),
		&((*pC) - (*pB)));
	D3DXVec3Normalize(&vec3FaceNormal, &vec3FaceNormal);
Exit0:
	return;

}

BOOL KG3DMesh::GetRayIntersectPointAndDir(const D3DXVECTOR3& vec3Origin, 
										  const D3DXVECTOR3& vec3Direction,
										  D3DXVECTOR3* pIntersectionPoints,
										  float* pAngles,
										  DWORD& dwNumIntersectionPoints,
										  const int nBufferSize)
{
	const static int BufferSize = 100;
	HRESULT hr = E_FAIL;
	BOOL bHit = FALSE;
	DWORD dwHitCount;
	float fHitDis[BufferSize];
	LPD3DXBUFFER pAllHits = NULL;
	D3DXINTERSECTINFO* pInfo = NULL;
	short* pIndices = NULL;
	BYTE* pVertices = NULL;
	DWORD dwStride = 0;

	KG_PROCESS_ERROR(m_ppMeshes[SMBT_NORMAL]);

	hr = D3DXIntersect(m_ppMeshes[SMBT_NORMAL], 
		&vec3Origin, 
		&vec3Direction, 
		&bHit, 
		NULL, 
		NULL, 
		NULL, 
		fHitDis, 
		&pAllHits, 
		&dwHitCount);

	KG_COM_PROCESS_ERROR(hr);

	KG_PROCESS_ERROR(bHit);
	pInfo = static_cast<D3DXINTERSECTINFO*>(pAllHits->GetBufferPointer());
	KG_PROCESS_ERROR(pInfo);
	
	dwStride = m_ppMeshes[SMBT_NORMAL]->GetNumBytesPerVertex();
	hr = m_ppMeshes[SMBT_NORMAL]->LockIndexBuffer(0, (void**)&pIndices);
	KG_COM_PROCESS_ERROR(hr);

	hr = m_ppMeshes[SMBT_NORMAL]->LockVertexBuffer(0, (void**)&pVertices);
	KG_COM_PROCESS_ERROR(hr);

	dwNumIntersectionPoints = min(dwHitCount, (DWORD)nBufferSize);

	for(DWORD i = 0; i < dwNumIntersectionPoints; i++)
	{
		D3DXVECTOR3 vec3FaceNormal;
		GetFaceNormal(pIndices,
			pVertices,
			dwStride,
			pInfo[i].FaceIndex,
			vec3FaceNormal);

		pAngles[i] = D3DXVec3Dot(&vec3FaceNormal, 
			&vec3Direction);
		pIntersectionPoints[i] = vec3Origin + vec3Direction * pInfo[i].Dist;
	}
Exit0:
	if (m_ppMeshes[SMBT_NORMAL])
	{
		m_ppMeshes[SMBT_NORMAL]->UnlockIndexBuffer();
		m_ppMeshes[SMBT_NORMAL]->UnlockVertexBuffer();
	}
	SAFE_RELEASE(pAllHits);
	return bHit;
}

BOOL KG3DMesh::IsSecondUV()
{
	BOOL bReturn = FALSE;
	KG_PROCESS_ERROR(m_ppMeshes[SMBT_NORMAL]);

	if (((m_ppMeshes[SMBT_NORMAL]->GetFVF() & 0xff00) >> 8) > 1)
	{
		bReturn = TRUE;
	}
Exit0:
	return bReturn;
}

HRESULT KG3DMesh::HalfSphereNormal(DWORD dwOption)
{
	HRESULT hr = E_FAIL;
	SphereNormalVertex* pVertex = NULL;
	BYTE* pData = NULL;
	DWORD dwNumVertex = 0;
	DWORD dwStride = 0;
	D3DXVECTOR3 vec3Center;
	KG_PROCESS_ERROR(m_ppMeshes[SMBT_NORMAL]);

	hr = m_ppMeshes[SMBT_NORMAL]->LockVertexBuffer(0, (void**)&pData);
	KG_COM_PROCESS_ERROR(hr);

	dwNumVertex = m_ppMeshes[SMBT_NORMAL]->GetNumVertices();
	dwStride = m_ppMeshes[SMBT_NORMAL]->GetNumBytesPerVertex();
	vec3Center = m_BBox.GetCenter();
	vec3Center.y = min(m_BBox.A.y, m_BBox.B.y);

	for (DWORD i = 0; i < dwNumVertex; i++)
	{
		pVertex = reinterpret_cast<SphereNormalVertex*>(pData);
		D3DXVec3Normalize(&pVertex->vec3Normal, &(pVertex->vec3Pos - vec3Center));
	}

	hr = OptimizeUnSkinedMesh();
	KG_COM_PROCESS_ERROR(hr);

	hr = S_OK;
Exit0:
	if (m_ppMeshes[SMBT_NORMAL])
	{
		m_ppMeshes[SMBT_NORMAL]->UnlockVertexBuffer();
	}
	return hr;
}

void KG3DMesh::GetProfile(DWORD& dwNumFacesTotal,DWORD& dwNumVertices)
{
	if(m_ppMeshes[SMBT_NORMAL])
	{
		dwNumFacesTotal += m_ppMeshes[SMBT_NORMAL]->GetNumFaces();
		dwNumVertices += m_ppMeshes[SMBT_NORMAL]->GetNumVertices();
	}
}

LPCSTR KG3DMesh::GetBoneName(int nIndex)
{
	LPCSTR strReturn = NULL;
	KG_PROCESS_ERROR(nIndex >=0 && nIndex < static_cast<int>(m_dwNumBone));
	strReturn = m_BoneInfo[nIndex].strBoneName;
Exit0:
	return strReturn;
}

LPCSTR KG3DMesh::GetSocketName(int nIndex)
{
	LPCSTR strReturn = NULL;
	KG_PROCESS_ERROR(nIndex >= 0 && nIndex < static_cast<int>(m_dwNumSocket));
	strReturn = m_Sockets[nIndex].strSocketName;
Exit0:
	return strReturn;
}

DWORD KG3DMesh::GetNumSockets()
{
	return m_dwNumSocket;
}
////////////////////////////////////////////////////////////////////////////////
//IE interface
HRESULT KG3DMesh::GetBaseBoneIDVectorSize(DWORD *pdwSize)
{
    HRESULT hrResult = E_FAIL;
    KGLOG_PROCESS_ERROR(pdwSize);
    *pdwSize = (DWORD)(m_nBaseBoneID.size());
    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KG3DMesh::GetBaseBoneIDVectorItem(DWORD dwPosition, INT *pBaseBoneID)
{
    HRESULT hrResult = E_FAIL;
    DWORD dwSize = 0;

    KGLOG_PROCESS_ERROR(pBaseBoneID);
    
    dwSize = (DWORD)(m_nBaseBoneID.size());
    KGLOG_PROCESS_ERROR(dwPosition < dwSize);

    *pBaseBoneID = m_nBaseBoneID[dwPosition];
    
    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KG3DMesh::GetBoneInfoName(INT nBaseBoneID, LPCTSTR *ppszName)
{
    HRESULT hrResult = E_FAIL;
    KGLOG_PROCESS_ERROR((nBaseBoneID >= 0) && ((DWORD)nBaseBoneID < m_dwNumBone));
    KGLOG_PROCESS_ERROR(ppszName);

    *ppszName = m_BoneInfo[nBaseBoneID].strBoneName;
    
    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KG3DMesh::GetBoneInfoChildVectorSize(DWORD dwBoneInfoPosition, DWORD *pdwSize)
{
    HRESULT hrResult = E_FAIL;
    BoneInfo1 *pBoneInfo = NULL;

    KGLOG_PROCESS_ERROR(dwBoneInfoPosition < m_dwNumBone);
    KGLOG_PROCESS_ERROR(pdwSize);
    
    pBoneInfo = &m_BoneInfo[dwBoneInfoPosition];
    *pdwSize = (DWORD)(pBoneInfo->dwNumChild);

    hrResult = S_OK;
Exit0:
    return hrResult;
}
HRESULT KG3DMesh::GetBoneInfoChildVectorItem(DWORD dwBoneInfoPosition, DWORD dwChildPosition, INT *pnID)
{
    HRESULT hrResult = E_FAIL;
    BoneInfo1 *pBoneInfo = NULL;
    DWORD dwSize = 0;

    KGLOG_PROCESS_ERROR(dwBoneInfoPosition < m_dwNumBone);
    KGLOG_PROCESS_ERROR(pnID);

    pBoneInfo = &m_BoneInfo[dwBoneInfoPosition];
    
    dwSize = (DWORD)(pBoneInfo->dwNumChild);
    KGLOG_PROCESS_ERROR(dwChildPosition < dwSize);

    *pnID = pBoneInfo->dwChildIndex[dwChildPosition];

    hrResult = S_OK;
Exit0:
    return hrResult;

}

HRESULT KG3DMesh::SetBoneInfoScale(DWORD dwBoneInfoPosition, FLOAT x, FLOAT y, FLOAT z)
{
    //ASSERT(0);
	HRESULT hrResult = E_FAIL;

	if(m_pModel && m_pModel->m_pBip)
	{
		if(dwBoneInfoPosition < m_pModel->m_Mesh2Bip.size())
		{
			int nIndex = m_pModel->m_Mesh2Bip[dwBoneInfoPosition];

			const TCHAR* pName = m_pModel->m_pBip->GetBoneName(nIndex);
			D3DXVECTOR3 vScale = D3DXVECTOR3(x,y,z);
			m_pModel->m_pBip->SetBoneScale(pName,vScale);
			m_pModel->m_pBip->EnableBoneScale(TRUE);
		}
		;
	}
    //BoneInfo1 *pBone = NULL;
    
    //KGLOG_PROCESS_ERROR(dwBoneInfoPosition < m_dwNumBone);
    
    //pBone = &(m_BoneInfo[dwBoneInfoPosition]);

    //pBone->BoneScale = D3DXVECTOR3(x, y, z);

    hrResult = S_OK;
//Exit0:
    return hrResult;
}

HRESULT KG3DMesh::GetSocketName(DWORD dwPosition, LPCTSTR *ppszName)
{
    HRESULT hrResult = E_FAIL;
    KGLOG_PROCESS_ERROR(ppszName);
    KGLOG_PROCESS_ERROR(dwPosition < m_dwNumSocket);
    *ppszName = m_Sockets[dwPosition].strSocketName;
    hrResult = S_OK;
Exit0:
    return hrResult;
}
HRESULT KG3DMesh::SetSocketName(DWORD dwPosition, LPCTSTR pszName)
{
    HRESULT hrResult = E_FAIL;
    KGLOG_PROCESS_ERROR(pszName);
    KGLOG_PROCESS_ERROR(dwPosition < m_dwNumSocket);
    strcpy_s(m_Sockets[dwPosition].strSocketName,
		s_dwMaxSocketName,
		pszName);
    hrResult = S_OK;
Exit0:
    return hrResult;
}
HRESULT KG3DMesh::GetSocketParentBoneName(DWORD dwPosition, LPCTSTR *ppszParentBoneName)
{
    HRESULT hrResult = E_FAIL;
    KGLOG_PROCESS_ERROR(ppszParentBoneName);
    KGLOG_PROCESS_ERROR(dwPosition < m_dwNumSocket);
    *ppszParentBoneName = m_BoneInfo[m_Sockets[dwPosition].dwParentBoneIndex].strBoneName;
    hrResult = S_OK;
Exit0:
    return hrResult;
}
HRESULT KG3DMesh::GetOption(DWORD *pdwOption)
{
    HRESULT hrResult = E_FAIL;
    
    KGLOG_PROCESS_ERROR(pdwOption);

    *pdwOption = m_dwOption;

    hrResult = S_OK;
Exit0:
    return hrResult;
}
HRESULT KG3DMesh::SetOption(DWORD dwOption)
{
    HRESULT hrResult = E_FAIL;
    m_dwOption = dwOption;
    hrResult = S_OK;
//Exit0:
    return hrResult;
}
HRESULT KG3DMesh::GetSocketCount(DWORD *pdwSocketCount)
{
    HRESULT hrResult = E_FAIL;

    KGLOG_PROCESS_ERROR(pdwSocketCount);

    *pdwSocketCount = m_dwNumSocket;

    hrResult = S_OK;
Exit0:
    return hrResult;
}
HRESULT KG3DMesh::IsMeshSysNotNull(BOOL *pbState)
{
    HRESULT hrResult = E_FAIL;

    KGLOG_PROCESS_ERROR(pbState);

    if (m_ppMeshes[SMBT_NORMAL])
        *pbState = TRUE;
    else
        *pbState = FALSE;

    hrResult = S_OK;
Exit0:
    return hrResult;
}
HRESULT KG3DMesh::GetMeshSysFVF(DWORD *pdwFVF)
{
    HRESULT hrResult = E_FAIL;

    KGLOG_PROCESS_ERROR(pdwFVF);

    *pdwFVF = m_ppMeshes[SMBT_NORMAL]->GetFVF();

    hrResult = S_OK;
Exit0:
    return hrResult;
}
HRESULT KG3DMesh::GetMeshSysNumVertices(DWORD *pdwNumVertex)
{
    HRESULT hrResult = E_FAIL;

    KGLOG_PROCESS_ERROR(pdwNumVertex);
    
    *pdwNumVertex = m_ppMeshes[SMBT_NORMAL]->GetNumVertices();

    hrResult = S_OK;
Exit0:
    return hrResult;
}
HRESULT KG3DMesh::LockVertexBufferOfMesfSys(DWORD dwFlag, PVOID *ppBuffer)
{
    HRESULT hrResult = E_FAIL;
    HRESULT hrRetCode = E_FAIL;
    
    KGLOG_PROCESS_ERROR(ppBuffer);

    hrRetCode = m_ppMeshes[SMBT_NORMAL]->LockVertexBuffer(dwFlag, ppBuffer);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    hrResult = S_OK;
Exit0:
    return hrResult;

}

HRESULT KG3DMesh::UnLockVertexBufferOfMesfSys()
{
    HRESULT hrResult = E_FAIL;
    HRESULT hrRetCode = E_FAIL;

    hrRetCode = m_ppMeshes[SMBT_NORMAL]->UnlockVertexBuffer();
    KGLOG_COM_PROCESS_ERROR(hrRetCode);
    
    hrResult = S_OK;
Exit0:
    return hrResult;

}

HRESULT KG3DMesh::AddSocket(LPCTSTR strParentBoneName, LPCTSTR strSocketName)
{
    //重名插槽的处理问题暂时没有
    HRESULT hrResult = E_FAIL;
    /*int nBoneIndex = FindBone(strParentBoneName); 
	KG_PROCESS_ERROR(nBoneIndex != -1);
	
	{
		DWORD dwNewSocket = m_dwNumSocket + 1;
		_Socket* pNewSockets = new _Socket[dwNewSocket];
		ASSERT(pNewSockets);
		for(DWORD i = 0; i < m_dwNumSocket; i++)
		{
			pNewSockets[i] = m_Sockets[i];
		}
		pNewSockets[m_dwNumSocket].scName = strSocketName;
		pNewSockets[m_dwNumSocket].scParentBoneName = strParentBoneName;
		int nParentBoneIndex = FindBone(strParentBoneName);
		ASSERT(nParentBoneIndex != -1);
		pNewSockets[m_dwNumSocket].m_nParentBone = nParentBoneIndex;

		_BoneInfo* pInfo = &m_pBoneInfo[nBoneIndex];
		D3DXMatrixIdentity(&pNewSockets[m_dwNumSocket].MatrixOffset);
		D3DXMatrixMultiply(&pNewSockets[m_dwNumSocket].CurMatrix,
			&pInfo->CurMatrix,
			&pNewSockets[m_dwNumSocket].MatrixOffset);
		D3DXVECTOR3 A,B,C;
		A = D3DXVECTOR3(pNewSockets[m_dwNumSocket].CurMatrix._11,pNewSockets[m_dwNumSocket].CurMatrix._12,pNewSockets[m_dwNumSocket].CurMatrix._13);
		B = D3DXVECTOR3(pNewSockets[m_dwNumSocket].CurMatrix._21,pNewSockets[m_dwNumSocket].CurMatrix._22,pNewSockets[m_dwNumSocket].CurMatrix._23);
		C = D3DXVECTOR3(pNewSockets[m_dwNumSocket].CurMatrix._31,pNewSockets[m_dwNumSocket].CurMatrix._32,pNewSockets[m_dwNumSocket].CurMatrix._33);

		float Sx,Sy,Sz;
		Sx = D3DXVec3Length(&A);
		Sy = D3DXVec3Length(&B);
		Sz = D3DXVec3Length(&C);
		D3DXMATRIX matScale;
		D3DXMatrixScaling(&matScale,Sx,Sy,Sz);
		D3DXMatrixInverse(&matScale,NULL,&matScale);
		D3DXMatrixMultiply(&pNewSockets[m_dwNumSocket].CurMatrix,
			&matScale,
			&pInfo->CurMatrix);

		SAFE_DELETE_ARRAY(m_pSockets);
		m_pSockets = pNewSockets;
		m_dwNumSocket++;
	}*/
    hrResult = S_OK;
//Exit0:
	return hrResult;
}

HRESULT KG3DMesh::RemoveSocket(LPCTSTR strSocketName)
{
    HRESULT hrResult = E_FAIL;
    for(DWORD i = 0; i < m_dwNumSocket; i++)
    {
        Socket1* pSocket = &m_Sockets[i];
        if ( !strcmp(strSocketName, pSocket->strSocketName) )
        {
            for(DWORD j = i; j < m_dwNumSocket - 1; j++)
            {
                m_Sockets[j] = m_Sockets[j + 1];
            }
            m_dwNumSocket--;
            hrResult = S_OK;
            break;
        }
    }
    return hrResult;
}


HRESULT KG3DMesh::SphereNormal()
{
    HRESULT hrRetCode = E_FAIL;
	HRESULT hrResult = E_FAIL;

	DWORD dwFVF = 0;
	DWORD dwNumVertices = 0;
	DWORD dwNormalOffset = 0;
	DWORD dwStride = 0;
	BYTE *pVertex = NULL;
	const VertexFromatOffsetItem* pVertexFormat = NULL;
	KG_PROCESS_ERROR(m_ppMeshes[SMBT_NORMAL]);
	
	dwFVF = m_ppMeshes[SMBT_NORMAL]->GetFVF();
	dwNumVertices = m_ppMeshes[SMBT_NORMAL]->GetNumVertices();
	KG_PROCESS_ERROR(dwNumVertices);

	dwStride = m_ppMeshes[SMBT_NORMAL]->GetNumBytesPerVertex();
	KG_PROCESS_ERROR(dwStride);
	
	pVertexFormat = GetVertexFormat(dwFVF);
	KG_PROCESS_ERROR(pVertexFormat);
	KG_PROCESS_ERROR(pVertexFormat->dwFVF & D3DFVF_NORMAL);
	
	dwNormalOffset = pVertexFormat->dwDestOffset[1];
	KG_PROCESS_ERROR(dwNormalOffset != 0xffffffff);

	hrRetCode = m_ppMeshes[SMBT_NORMAL]->LockVertexBuffer(0, (void**)&pVertex);
	KG_COM_PROCESS_ERROR(hrRetCode);

	for (DWORD i = 0; i < dwNumVertices; i++)
	{
		D3DXVECTOR3 *pPosition = reinterpret_cast<D3DXVECTOR3*>(pVertex + dwStride * i);
		D3DXVECTOR3 *pNormal = reinterpret_cast<D3DXVECTOR3*>(pVertex + dwStride * i + dwNormalOffset);
		*pNormal = SphereNormalCompute(*pPosition);
	}

	hrRetCode = m_ppMeshes[SMBT_NORMAL]->UnlockVertexBuffer();
	KG_COM_PROCESS_ERROR(hrRetCode);

	if (!(m_dwOption & MESH_SKINED))
	{
		hrRetCode = OptimizeUnSkinedMesh();
		KG_COM_PROCESS_ERROR(hrRetCode);
	}
	else
	{
		hrRetCode = CreateSkinMesh();
		KG_COM_PROCESS_ERROR(hrRetCode);
	}
	
	hrResult = S_OK;
Exit0:
	if (m_ppMeshes[SMBT_NORMAL])
	{
		m_ppMeshes[SMBT_NORMAL]->UnlockVertexBuffer();
	}

	return hrResult;
}
D3DXVECTOR3 KG3DMesh::SphereNormalCompute(D3DXVECTOR3& Pos)
{
	D3DXVECTOR3 normal = Pos;
	D3DXVec3Normalize(&normal,&normal);
	return normal;
}

HRESULT KG3DMesh::OnLostDevice()
{
	/*SAFE_RELEASE(m_pSkinInfo);
	SAFE_RELEASE(m_ppMeshes[SMBT_OPTIMIZED]);
	m_eDynamicLoadState = STATE_MULTITHREADREAD_FINISH;*/
	//UnInit();
	return S_OK;
}

HRESULT KG3DMesh::OnResetDevice()
{
	//CheckLoadingState();
	return S_OK;
}

HRESULT KG3DMesh::CreateBBoxOfBone()
{
	HRESULT hr = E_FAIL;
	if(!m_ppMeshes[SMBT_NORMAL])
		return E_FAIL;
	
	_BoneInfo* pBoneInfo = NULL;
	BYTE* pVer = NULL;

	KGLOG_PROCESS_ERROR(m_lpMeshBoneSocketData);

	hr = m_ppMeshes[SMBT_NORMAL]->LockVertexBuffer(0, (void**)&pVer);
	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);
	for(DWORD dwBoneIndex=0;dwBoneIndex< m_dwNumBone;dwBoneIndex++)
	{
		BoneInfo1& BoneInfo = m_BoneInfo[dwBoneIndex];
		pBoneInfo = &m_lpMeshBoneSocketData->g_BoneInfo[dwBoneIndex];
		BoneInfo.bBox.Clear();
		for (DWORD dwVerNum = 0 ; dwVerNum < pBoneInfo->NumVerticesRef ; dwVerNum++)
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

HRESULT KG3DMesh::ComputeBoneBBoxByIndex(DWORD dwIndex)
{
	HRESULT hr = E_FAIL;
	if(!m_ppMeshes[SMBT_NORMAL])
		return E_FAIL;

	BYTE* pVer = NULL;

	hr = m_ppMeshes[SMBT_NORMAL]->LockVertexBuffer(0, (void**)&pVer);
	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);
	KG_PROCESS_ERROR(dwIndex < m_dwNumBone);
	/*m_BoneInfo[dwIndex].bBox.Clear();
	for (DWORD i = 0 ;  i < m_BoneInfo[dwIndex].dwNumVerticesRef; i++)
	{
		float weight  = m_BoneInfo[dwIndex].VerticesWeightRef[i];
		if (weight > g_fBoneWeight)
		{
			D3DXVECTOR3* Pos = (D3DXVECTOR3*)(&pVer[m_BoneInfo[dwIndex].VerticesIndexRef[i] * m_dwVertexStride]);
			m_BoneInfo[dwIndex].bBox.AddPosition(*Pos);
		}
	}*/
Exit0:
	hr = m_ppMeshes[SMBT_NORMAL]->UnlockVertexBuffer();
	return hr;
}


void KG3DMesh::SetCurSelected(DWORD Index)
{
	m_dwCurSel = Index;
}

void KG3DMesh::SetBoneBBoxScale(DWORD Index,float& X,float& Y,float& Z)
{
	D3DXVECTOR3 Cpos;
	D3DXMATRIX TransMatrix,TransMatrix2,ScaleMatrix;
	KG_PROCESS_ERROR(m_dwCurSel < m_dwNumBone);
	D3DXMatrixScaling(&ScaleMatrix,X,Y,Z);

	Cpos = m_BoneInfo[Index].bBox.GetCenter();
	D3DXMatrixTranslation(&TransMatrix,-Cpos.x,-Cpos.y,-Cpos.z);
	D3DXMatrixTranslation(&TransMatrix2,Cpos.x,Cpos.y,Cpos.z);
	m_BoneInfo[Index].BBoxScaleMatrix = TransMatrix * ScaleMatrix * TransMatrix2;
	
Exit0:
	return ;
}

void KG3DMesh::GetBoneBBoxScale(DWORD Index,float& x,float& y,float& z)
{
	KG_PROCESS_ERROR(Index < m_dwNumBone);
	x = m_BoneInfo[Index].BBoxScaleMatrix.m[0][0];
	y = m_BoneInfo[Index].BBoxScaleMatrix.m[1][1];
	z = m_BoneInfo[Index].BBoxScaleMatrix.m[2][2];
Exit0:
	return ;
}

BOOL KG3DMesh::GetAllBoneBox(vector<BBox>& BoneBoxInfo)
{
	BBox bBox;
	DWORD dwCount = 0;
	for (DWORD i=0; i<m_dwNumBone;i++)
	{
		BBox NewBox;
		if (bBox == m_BoneInfo[i].bBox)
		{
			dwCount++;
		}
		NewBox.AddPosition(m_BoneInfo[i].bBox.A, m_BoneInfo[i].bBox.B,&m_BoneInfo[i].BBoxScaleMatrix);
		BoneBoxInfo.push_back(NewBox);		
	}
	if (dwCount == m_dwNumBone)
	{
		BoneBoxInfo.clear();
		return FALSE;
	}
	return TRUE;
}

LPD3DXMESH KG3DMesh::GetMesh(MeshType Type)
{
	ASSERT(Type < SMBT_COUNT);
	return m_ppMeshes[Type];
}

HRESULT KG3DMesh::SetLOD(float fLod)
{
	if(m_pMeshProgress)
	{
		DWORD dwMax = m_pMeshProgress->GetMaxFaces();
		//DWORD dwMin = m_pMeshProgress->GetMinFaces();

		//float K = (3.0F-fLod) * 0.3333F;
		DWORD NumFace = (DWORD)(dwMax * fLod);

		m_pMeshProgress->SetNumFaces(NumFace);
	}
	return S_OK;
}

HRESULT KG3DMesh::SkinMeshPostProcess()
{
	HRESULT hrResult = E_FAIL;
	HRESULT hrRetCode  = E_FAIL;
	DWORD dwStride = 0;
	DWORD dwNumVertex = 0;
	DWORD dwPositionOffset[4], dwNormalOffset[4];
	DWORD dwBlendWeightOffset = 0;
	DWORD dwBlendIndicesOffset = 0;
	BYTE *pVertexBuffer = NULL;
	DWORD dwStartTime = timeGetTime();
	D3DVERTEXELEMENT9 *pIndicator = NULL;
	LPD3DXBONECOMBINATION pBoneCombination = NULL;
	D3DVERTEXELEMENT9 VertexDecl[MAX_FVF_DECL_SIZE];
	ID3DXMesh* &pBlendMesh = m_ppMeshes[SMBT_BLENDED];

	memset(dwPositionOffset, 0xff, sizeof(DWORD) * 4);
	memset(dwNormalOffset, 0xff, sizeof(DWORD) * 4);

	std::set<DWORD> ProcessedVertex;
	KG_PROCESS_ERROR(pBlendMesh);
	KG_PROCESS_ERROR(m_pBoneCombinationTable);

	dwNumVertex = pBlendMesh->GetNumVertices();
	KG_PROCESS_ERROR(dwNumVertex);

	hrRetCode = pBlendMesh->LockVertexBuffer(0, (void**)&pVertexBuffer);
	KG_COM_PROCESS_ERROR(hrRetCode);

	dwStride = pBlendMesh->GetNumBytesPerVertex();
	KG_PROCESS_ERROR(dwStride);

	pBoneCombination = static_cast<LPD3DXBONECOMBINATION>(m_pBoneCombinationTable->GetBufferPointer());
	KG_PROCESS_ERROR(pBoneCombination);

	hrRetCode = pBlendMesh->GetDeclaration(VertexDecl);
	KG_COM_PROCESS_ERROR(hrRetCode);

	pIndicator = VertexDecl;
	while(pIndicator->Stream != 0xff)
	{
		switch(pIndicator->Usage)
		{
		case D3DDECLUSAGE_POSITION:
			{
				dwPositionOffset[pIndicator->UsageIndex] = pIndicator->Offset;
				break;
			}
		case D3DDECLUSAGE_BLENDINDICES:
			{
				dwBlendIndicesOffset = pIndicator->Offset;
			}
			break;
		case D3DDECLUSAGE_BLENDWEIGHT:
			{
				dwBlendWeightOffset = pIndicator->Offset;
			}
			break;
		case D3DDECLUSAGE_NORMAL:
			{
				dwNormalOffset[pIndicator->UsageIndex] = pIndicator->Offset;
			}
			break;
		}
		pIndicator++;
	}

	for (DWORD i = 0; i < m_dwNumAttributeGroups; i++)
	{
		D3DXMATRIX matTemp[KG3DGraphicsTool::MAX_NUM_BONE_PER_SET_SHADER_2];
		int nNumBone = min(KG3DGraphicsTool::m_nMaxNumBone, static_cast<int>(m_dwNumBone));
		for (int k = 0; k < nNumBone; k++)
		{
			DWORD dwBoneIndex = pBoneCombination[i].BoneId[k];
			if (dwBoneIndex == UINT_MAX)
			{
				D3DXMatrixIdentity(&matTemp[k]);
				continue;
			}
			matTemp[k] = m_matReverseMatrix[dwBoneIndex];
		}

		for (DWORD j = pBoneCombination[i].VertexStart; j < pBoneCombination[i].VertexCount + pBoneCombination[i].VertexStart; j++)
		{
			D3DXVECTOR3 *pVertexPosition[4];
			D3DXVECTOR3 *pVertexNormal[4];

			for (DWORD k = 0; k < m_dwMaxFaceInfl; k++)
			{
				pVertexPosition[k] = reinterpret_cast<D3DXVECTOR3*>(pVertexBuffer + dwStride * j + dwPositionOffset[k]);
				pVertexNormal[k]   = reinterpret_cast<D3DXVECTOR3*>(pVertexBuffer + dwStride * j + dwNormalOffset[k]);
			}
			
			BYTE *pBlendIndex      = reinterpret_cast<BYTE*>(pVertexBuffer + dwStride * j + dwBlendIndicesOffset);
			D3DXVECTOR3 vec3OrgPos = *pVertexPosition[0];
			D3DXVECTOR3 vec3OrgNormal = *pVertexNormal[0];
			for (BYTE k = 0; k < m_dwMaxFaceInfl; k++)
			{
				if (pBlendIndex[k] == UINT_MAX)
				{
					*pVertexPosition[k] = D3DXVECTOR3(.0f, .0f, .0f);
					continue;
				}
				D3DXVec3TransformCoord(pVertexPosition[k], &vec3OrgPos, &matTemp[pBlendIndex[k]]);
				D3DXVec3TransformNormal(pVertexNormal[k], &vec3OrgNormal, &matTemp[pBlendIndex[k]]);
			}
		}
	}
	hrResult = S_OK;
Exit0:
	if (pBlendMesh)
	{
		pBlendMesh->UnlockVertexBuffer();
	}

	DWORD dwCost = timeGetTime() - dwStartTime;
	if(g_cEngineOption.bEnableTimeOptimizeLog && dwCost > 10)
	{
		KGLogPrintf(KGLOG_ERR, "TimeOptimize %d KG3DMesh::SkinMeshPostProcess  %s.",dwCost , m_scName.c_str());
	}
	return hrResult;
}

HRESULT KG3DMesh::ConvertBoneToCurrentVersion()
{
	HRESULT hrResult = E_FAIL;
	KG_PROCESS_SUCCESS(m_dwNumBone == 0);

	KGLOG_PROCESS_ERROR(m_lpMeshBoneSocketData);
	
	for(DWORD i = 0; i < m_dwNumBone; i++)
	{
		strcpy_s(m_BoneInfo[i].strBoneName, 
			s_dwMaxBoneName, 
			m_lpMeshBoneSocketData->g_BoneInfo[i].scName.c_str());
		_strlwr_s(m_BoneInfo[i].strBoneName, s_dwMaxBoneName);
		m_BoneInfo[i].dwParentIndex = static_cast<DWORD>(m_lpMeshBoneSocketData->g_BoneInfo[i].nParentIndex);
		m_BoneInfo[i].dwNumChild = static_cast<DWORD>(m_lpMeshBoneSocketData->g_BoneInfo[i].ChildIDVector.size());
		
		for (DWORD j = 0; j < m_BoneInfo[i].dwNumChild; j++)
		{
			m_BoneInfo[i].dwChildIndex[j] = m_lpMeshBoneSocketData->g_BoneInfo[i].ChildIDVector[j];
		}

		m_BoneInfo[i].CurMatrix = m_lpMeshBoneSocketData->g_BoneInfo[i].CurMatrix;
		//m_BoneInfo[i].LocalWorldCurMatrix_NotNeeded = m_lpMeshBoneSocketData->g_BoneInfo[i].LocalWorldCurMatrix_NotNeeded;
		//m_BoneInfo[i].nFlexBoneLevel_NotNeeded = m_lpMeshBoneSocketData->g_BoneInfo[i].nFlexBoneLevel_NotNeeded;
		m_BoneInfo[i].IsFlexibleBone = m_lpMeshBoneSocketData->g_BoneInfo[i].IsFlexibleBone;
		m_BoneInfo[i].bBox           = m_lpMeshBoneSocketData->g_BoneInfo[i].bBox;
		m_BoneInfo[i].BBoxScaleMatrix = m_lpMeshBoneSocketData->g_BoneInfo[i].BBoxScaleMatrix;
		/*if ((!strcmp(m_BoneInfo[i].strBoneName, "bip01 head")) ||
			(!strcmp(m_BoneInfo[i].strBoneName, "bip01 l foot")) ||
			(!strcmp(m_BoneInfo[i].strBoneName, "bip01 r foot")) ||
			(!strcmp(m_BoneInfo[i].strBoneName, "bip01 l hand")) ||
			(!strcmp(m_BoneInfo[i].strBoneName, "bip01 r hand")))
		{
			D3DXMATRIX matscale;
			float fScale = 1.0f / 2.0f;
			D3DXMatrixScaling(&matscale, fScale, fScale, fScale);
			m_BoneInfo[i].CurMatrix = matscale * m_BoneInfo[i].CurMatrix;
		}*/
		D3DXMatrixInverse(&m_matReverseMatrix[i], 
			NULL,
			&m_BoneInfo[i].CurMatrix);
		m_BoneInfo[i].CurMatrix = m_lpMeshBoneSocketData->g_BoneInfo[i].CurMatrix;
		m_BoneInfo[i].NumVerticesRef = m_lpMeshBoneSocketData->g_BoneInfo[i].NumVerticesRef;
		SAFE_DELETE_ARRAY(m_BoneInfo[i].VerticesIndexRef);
		SAFE_DELETE_ARRAY(m_BoneInfo[i].VerticesWeightRef);
		m_BoneInfo[i].VerticesIndexRef = m_lpMeshBoneSocketData->g_BoneInfo[i].VerticesIndexRef;
		m_BoneInfo[i].VerticesWeightRef = m_lpMeshBoneSocketData->g_BoneInfo[i].VerticesWeightRef;
		m_lpMeshBoneSocketData->g_BoneInfo[i].VerticesIndexRef = NULL;
		m_lpMeshBoneSocketData->g_BoneInfo[i].VerticesWeightRef = NULL;
	}
Exit1:
	hrResult = S_OK;
	return hrResult;
Exit0:
	return E_FAIL;
}

HRESULT KG3DMesh::ConvertSocketToCurrentVersion()
{
	HRESULT hrResult = E_FAIL;
	KG_PROCESS_SUCCESS(m_dwNumSocket == 0);

	KGLOG_PROCESS_ERROR(m_lpMeshBoneSocketData);

	for (DWORD i = 0; i < m_dwNumSocket; i++)
	{
		strcpy_s(m_Sockets[i].strSocketName,
			s_dwMaxSocketName,
			m_lpMeshBoneSocketData->g_Sockets[i].scName.c_str());
		_strlwr_s(m_Sockets[i].strSocketName, s_dwMaxSocketName);
		
		m_Sockets[i].dwParentBoneIndex = static_cast<DWORD>(m_lpMeshBoneSocketData->g_Sockets[i].m_nParentBone);
		m_Sockets[i].matMatrixOffset = m_lpMeshBoneSocketData->g_Sockets[i].MatrixOffset;
        m_Sockets[i].matCurMatrix = m_lpMeshBoneSocketData->g_Sockets[i].CurMatrix;

		/*if (!strcmp(m_Sockets[i].strSocketName, "s_face"))
		{
			D3DXMATRIX matscale;
			float fScale = 2.0f;
			D3DXMatrixScaling(&matscale, fScale, fScale, fScale);
			m_Sockets[i].matCurMatrix = matscale * m_Sockets[i].matCurMatrix;
			m_Sockets[i].matMatrixOffset = matscale * g_Sockets[i].MatrixOffset;
		}*/
	}
Exit1:
	hrResult = S_OK;
	return hrResult;

Exit0:
	return E_FAIL;
}

DWORD KG3DMesh::GetViewClass()
{
	return (DWORD)m_objPropertys.nViewClass;
}

BOOL KG3DMesh::IsBBoxChanged()
{
	return m_bBBoxChanged;
}

HRESULT KG3DMesh::Init()
{
	m_scName = "";
	return S_OK;
}


ULONG STDMETHODCALLTYPE KG3DMesh::AddRef()
{
    return (ULONG)KG_InterlockedIncrement((long *)&m_ulRefCount);
}

ULONG STDMETHODCALLTYPE KG3DMesh::Release()
{
    ULONG uNewRefCount = (ULONG)KG_InterlockedDecrement((long *)&m_ulRefCount);
    if (uNewRefCount == 0)
        OnReferenceZero(); 
    return uNewRefCount;
}

HRESULT KG3DMesh::OnReferenceZero()
{
	return g_cMeshTable.ReleaseResource(this, RELEASE_INQUEUE);
}

unsigned KG3DMesh::GetType()
{
	return m_dwType;
}

HRESULT STDMETHODCALLTYPE KG3DMesh::QueryInterface( 
	/* [in] */ REFIID riid,
	/* [iid_is][out] */ void **ppvObject)
{
	return E_FAIL;
}


DWORD KG3DMesh::GetID()
{
	return m_dwID;
}

HRESULT KG3DMesh::GetName(LPCTSTR *ppszName)
{
	HRESULT hrResult = E_FAIL;
	KGLOG_PROCESS_ERROR(ppszName);

	*ppszName = m_scName.c_str();
	hrResult = S_OK;
Exit0:
	return hrResult;

}

HRESULT KG3DMesh::GetType(DWORD *pdwType)
{
	HRESULT hrResult = E_FAIL;
	KGLOG_PROCESS_ERROR(pdwType);
	*pdwType = GetType();
	hrResult = S_OK;
Exit0:
	return hrResult;
}

////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DMesh::GetObjProperty(BYTE **ppbObj)
{
	HRESULT hrResult = E_FAIL;
	KGLOG_PROCESS_ERROR(ppbObj);

	*ppbObj = (BYTE *)(&m_objPropertys);

	hrResult = S_OK;
Exit0:
	return hrResult;
}

const KG3DOBJProperty& KG3DMesh::GetObjProperty()
{
	return m_objPropertys;
}
HRESULT KG3DMesh::SetViewLayer(int nLayer)
{
	m_objPropertys.nViewClass = nLayer;
	return S_OK;
}

void KG3DMesh::SetModel(KG3DModel* pModel)
{
	m_pModel = pModel;
}

//Struct Member functions
KG3DMesh::_BoneInfo::~_BoneInfo()
{
	ChildNameVector.clear();
	ChildIDVector.clear();

	NumVerticesRef = 0;
	SAFE_DELETE_ARRAY(VerticesIndexRef);
	SAFE_DELETE_ARRAY(VerticesWeightRef);
	BoneLength = 0;
}

KG3DMesh::_BoneInfo::_BoneInfo()
{
	scName = "";
	scParentName = "";
	NumVerticesRef = 0;
	VerticesIndexRef = NULL;
	VerticesWeightRef = 0;
	BoneLength = 0;
	bFlexibleBone_NotUsed = FALSE;
	nParentIndex = -1;

	IsFlexibleBone = FALSE;
	nFlexBoneLevel_NotNeeded = 0;
	D3DXMatrixIdentity(&BBoxScaleMatrix);
}

KG3DMesh::_Socket::_Socket()
{
	m_nParentBone = -1;
	YawPitchRoll = D3DXVECTOR3(0,0,0);
}

void KG3DMesh::_Socket::GetCurPosition()
{
	CurPosition.x = CurMatrix._41;
	CurPosition.y = CurMatrix._42;
	CurPosition.z = CurMatrix._43;
}

void KG3DMesh::_Socket::GetCurDirectionPosition()
{
	GetCurPosition();

	D3DXVECTOR3 Y(0,1,0);
	D3DXVECTOR3 Dir(0,0,0);

	D3DXVec3TransformNormal(&Dir,&Y,&MatrixBase);
	CurDirectionPosition = CurPosition + Dir * 100;
}

KG3DMesh::BoneInfo1::BoneInfo1()
{
	strBoneName[0] = '\0';
	memset(dwChildIndex, 0xff, sizeof(DWORD) * s_dwMaxChildBone);
	dwParentIndex = s_dwInvalidateIndex;
	IsFlexibleBone = FALSE;
	nFlexBoneLevel_NotNeeded = 0;
	D3DXMatrixIdentity(&BBoxScaleMatrix);
	NumVerticesRef = 0;
	VerticesIndexRef = NULL;
	VerticesWeightRef = NULL;
}

KG3DMesh::BoneInfo1::~BoneInfo1()
{
	SAFE_DELETE_ARRAY(VerticesWeightRef);
	SAFE_DELETE_ARRAY(VerticesIndexRef);
}

KG3DMesh::_MeshFileHead::_MeshFileHead()
{
	dwFileMask     = s_dwFileMask;
	dwBlockLength  = 0;
	dwVersion      = 0;//s_dwCurrentVersion;
	AnimationBlock_NotUse = 0;
	ZeroMemory(ExtendBlock_NotUse,sizeof(DWORD)*10);
	MeshCount_NotUse      = 1;
};

KG3DMesh::_MeshHead::_MeshHead()
{
	ZeroMemory(dwBlocks, sizeof(DWORD) * 30);
}

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
		case STATE_LOADFAILED:
		case STATE_MULTITHREAD_OKTOTHREAD:
			break;
		//正在动态加载
		case STATE_MULTITHREAD_PREPEAR:
			{
				/*if (m_bInMultiThreadLoadList)
				{
					for (DWORD i = 0; i < SMBT_COUNT; i++)
					{
						if (m_ppMeshes[i] && m_lpMeshStaticData)
						{
							m_lpMeshStaticData->p3DXMeshDiscard[i] = m_ppMeshes[i];
							m_lpMeshStaticData->p3DXMeshDiscard[i]->AddRef();
						}
					}
				}*/
				break;
			}
		//动态加载完成了, 只需要checkloadstate就可以完成加载
		case STATE_MULTITHREADREAD_FINISH:
			{
				CheckLoadingState();
				goto Exit1;
				break;
			}
		default:
			assert(0);
		}
		
		hrRetCode = LoadFromFile(m_scName.c_str(), 
		0,
		uOption);
		KG_COM_PROCESS_ERROR(hrRetCode);
	}
	m_uOption = uOption;
Exit1:
	hrResult = S_OK;
Exit0:
	return hrResult;
}

//BOOL KG3DMesh::GetNearRayIntersect( const D3DXVECTOR3& vSrc, const D3DXVECTOR3& vNormalizedDir , FLOAT* pfRet )
//{
//	return this->RayIntersection(vSrc, vNormalizedDir, pfRet);
//}

HRESULT KG3DMesh::Helper_ShaderSetConverMap( LPD3DXEFFECT pShader, BOOL bUseConverTexture,BOOL bSkin,BOOL bUseDetail)
{
	HRESULT hr = E_FAIL;

	if(bSkin)
	{
		if(bUseDetail)
		{
			hr = pShader->SetTechnique("ConverMapTechnique_Detail");
			if(FAILED(hr))
			{
				int k = 0;
			}
			KGLOG_COM_CHECK_ERROR(hr);
		}
		else
		{
			hr = pShader->SetTechnique("DefaultTechnique");
			KGLOG_COM_CHECK_ERROR(hr);
		}
	}
	else
	{
		if(bUseConverTexture && bUseDetail)
		{
			KG3DTerrainConverMap* pConverMap = g_cGraphicsTool.GetCurrentConverMap();
			if (pConverMap)
			{
				hr = pShader->SetTechnique("ConverMapTechnique");
				KGLOG_COM_CHECK_ERROR(hr);
				if(FAILED(hr))
				{
					int k = 0;
				}
			}
			else
			{
				hr = pShader->SetTechnique("DefaultTechnique");
				KGLOG_COM_CHECK_ERROR(hr);
			}
		}
		else
		{
			hr = pShader->SetTechnique("DefaultTechnique");
			KGLOG_COM_CHECK_ERROR(hr);
		}		
	}



	return hr;
}

BOOL KG3DMesh::IsLightInConverMap()
{
	return (DWORD)m_objPropertys.bAffectByTime;
}


HRESULT KG3DMesh::RenderForSelector(KG3DMaterial* pMaterial, 
									void* pExtInfo,
									DWORD dwOption)
{
	HRESULT hrResult = E_FAIL;
	HRESULT hrRetCode  = E_FAIL;

	LPD3DXMESH pMeshToRender = NULL;
	D3DXMATRIX* pRenderMatrices = NULL;

	KG_PROCESS_SUCCESS(m_eDynamicLoadState != STATE_VIDEOMEM);
	KG_PROCESS_ERROR(pMaterial);

	if (m_ppMeshes[SMBT_OPTIMIZED] && (m_dwOption & MESH_OPTIMIZED)) 
	{
		//使用Optimize Mesh
		pMeshToRender = m_ppMeshes[SMBT_OPTIMIZED];
	}
	else
	{
		pMeshToRender = m_ppMeshes[SMBT_NORMAL];
	}

	hrRetCode = GetRenderMatrix(pMeshToRender, pRenderMatrices, pExtInfo);
	if (m_dwOption & MESH_SKINED)  
	{ 
		//带Skin的模型
		hrRetCode = Shader_RenderSkinMeshForSelector(pRenderMatrices, pMaterial, dwOption);
	}
	else
	{
		//普通的模型
		hrRetCode = Shader_RenderMeshForSelector(pMaterial);
	}
	KG_COM_PROCESS_ERROR(hrRetCode);
Exit1:
	hrResult = S_OK;
Exit0:
	return hrResult;
}

LPD3DXEFFECT g_pSkinShader = NULL;
HRESULT KG3DMesh::Shader_RenderSkinMeshForSelector(D3DXMATRIX *pMatrices, KG3DMaterial *pMaterial, DWORD dwOption)
{	
	Shader_RenderSkinMeshForSelector_Begin(pMatrices);
		
	Shader_RenderSkinMeshForSelector_NoChange(pMatrices, dwOption, pMaterial);

	return S_OK;
}


HRESULT KG3DMesh::Shader_RenderMeshForSelector(KG3DMaterial *pMaterial)
{
	Shader_RenderMeshForSelector_Begin();
	Shader_RenderMeshForSelector_NoChange(pMaterial);

	return S_OK;
}

HRESULT KG3DMesh::Shader_RenderMeshForSelector_Begin()
{
	HRESULT hrResult = E_FAIL;
	UINT uNumPass = 0;
	HRESULT hr = S_OK;
	LPD3DXEFFECT pShader = g_cGraphicsTool.GetDefaultShader(DEFST_NEWLIGHT);
	KG_PROCESS_ERROR(pShader);

	if (g_pSkinShader != pShader)
	{
		if (g_pSkinShader)
		{
			g_pSkinShader->End();
		}
		pShader->SetTechnique("Selector");
		hr = pShader->Begin(&uNumPass,0 );//D3DXFX_DONOTSAVESTATE
		KG_COM_PROCESS_ERROR(hr);

		g_pSkinShader = pShader;
	}
	hrResult = S_OK;
Exit0:
	return hrResult;
}


HRESULT KG3DMesh::Shader_RenderMeshForSelector_NoChange(KG3DMaterial *pMaterial)
{
	HRESULT hrRetCode = E_FAIL;
	HRESULT hrResult = E_FAIL;
	KG3DRenderState RenderState;
	DWORD dwCullMode = D3DCULL_NONE;
	D3DXMATRIX matWorld, matView, matProj, matWVP;
	LPD3DXEFFECT pShader = g_cGraphicsTool.GetDefaultShader(DEFST_NEWLIGHT);
	LPD3DXMESH pMeshToRender = m_ppMeshes[SMBT_OPTIMIZED] ? m_ppMeshes[SMBT_OPTIMIZED] : m_ppMeshes[SMBT_NORMAL];
	KG_PROCESS_ERROR(pShader);
	KG_PROCESS_ERROR(pMaterial);
	KG_PROCESS_ERROR(pMeshToRender);
	KG_PROCESS_ERROR(m_pModel);

	pShader->BeginPass(0);
	g_pd3dDevice->GetTransform(D3DTS_WORLD, &matWorld);
	g_pd3dDevice->GetTransform(D3DTS_VIEW, &matView);
	g_pd3dDevice->GetTransform(D3DTS_PROJECTION, &matProj);
	matWVP = matWorld * matView * matProj;

	RenderState.SetRenderState(D3DRS_FOGENABLE, FALSE);
	RenderState.SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	
	hrRetCode = pShader->SetMatrix("WorldViewProj", &matWVP);
	hrRetCode = pShader->SetMatrix("matWorld", &m_pModel->m_matWorld);

	g_pd3dDevice->SetTexture(0, NULL);
	pShader->SetInt("nIndex", m_pModel->GetSelectIndex());
	
	KGLOG_PROCESS_ERROR(pMaterial->IsLoaded());

	for (DWORD i = 0; i < pMaterial->m_dwNumMaterials; i++)
	{
		KG3DMaterial::KG3DMaterialSubset* pSubMtl = pMaterial->GetSubMaterial(i);
		KGLOG_PROCESS_ERROR(pSubMtl);

		KG3DTexture* pTexture = pSubMtl->m_lpTextures[0];
		if (pTexture)
		{
			g_pd3dDevice->SetTexture(0, pTexture->GetTexture());
		}

		_MtlAlphaOperation* pAlphaOp = pSubMtl->GetAlphaOperation();
		BOOL bEnableAlphaFactor = !(pAlphaOp->EnableAlphaTest || pAlphaOp->EnableAlphaBlend);

		_MtlRenderTwiceAlphaOption *pTwiceOption = pSubMtl->GetAlphaRenderTwice();
		if (pTwiceOption->bEnable)
		{
			bEnableAlphaFactor = FALSE;
		}

		if (pAlphaOp->EnableAlphaTest)
		{
			RenderState.SetRenderState(D3DRS_ALPHAREF, pAlphaOp->BackAlphaTestRef);
		}
		RenderState.SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
		pShader->SetBool("bAlphaUseFactor", bEnableAlphaFactor);
		pShader->CommitChanges();
		if (m_dwOption & MATERIAL_OPTION_CULL_CCW)
		{
			dwCullMode = D3DCULL_CCW;
		}
		else if (m_dwOption & MATERIAL_OPTION_CULL_CW)
		{
			dwCullMode = D3DCULL_CW;
		}
		RenderState.SetRenderState(D3DRS_CULLMODE, dwCullMode);
		hrRetCode = pMeshToRender->DrawSubset(i);
	}
	KG_COM_PROCESS_ERROR(hrRetCode);
	hrResult = S_OK;
Exit0:
	if (pShader)
	{
		pShader->EndPass();
	}
	return hrResult;
}

HRESULT KG3DMesh::Shader_RenderSkinMeshForSelector_Begin(D3DXMATRIX* pmatBones)
{
	HRESULT hrResult = E_FAIL;
	UINT uNumPass = 0;
	HRESULT hr = S_OK;
	LPD3DXEFFECT pSkinShader = g_cGraphicsTool.GetDefaultShader(DEFST_SKINMESH);
	KG_PROCESS_ERROR(pSkinShader);

	
	if (g_pSkinShader != pSkinShader)
	{
		if (g_pSkinShader)
		{
			g_pSkinShader->End();
		}
		pSkinShader->SetTechnique("Selector");
		hr = pSkinShader->Begin(&uNumPass,0 );//D3DXFX_DONOTSAVESTATE
		KG_COM_PROCESS_ERROR(hr);

		g_pSkinShader = pSkinShader;
	}
	hrResult = S_OK;
Exit0:
	return hrResult;
}

HRESULT KG3DMesh::Shader_RenderSkinMeshForSelector_End(D3DXMATRIX* pmatBones)
{
	HRESULT hrResult = E_FAIL;
	HRESULT hrRetCode  = E_FAIL;
	LPD3DXEFFECT pSkinShader = g_cGraphicsTool.GetDefaultShader(DEFST_SKINMESH);

	KG_PROCESS_ERROR(pSkinShader);
	hrRetCode = pSkinShader->End();
	KG_COM_PROCESS_ERROR(hrRetCode);


	hrResult = S_OK;
Exit0:
	g_pd3dDevice->SetVertexShader(NULL);
	g_pd3dDevice->SetPixelShader(NULL);

	return hrResult;
}


HRESULT KG3DMesh::Shader_RenderSkinMeshForSelector_NoChange(D3DXMATRIX* pmatBones,
															DWORD dwOption,
															KG3DMaterial* pMaterial)
{
	HRESULT hrResult = E_FAIL;
	HRESULT hrRetCode = S_OK;
	LPD3DXEFFECT pSkinShader = g_cGraphicsTool.GetDefaultShader(DEFST_SKINMESH);
	D3DXMATRIX matTemp[KG3DGraphicsTool::MAX_NUM_BONE_PER_SET_SHADER_2];

	D3DXMATRIX mat;
	D3DXMatrixIdentity(&mat);
	
	D3DXBONECOMBINATION* pBoneComb = NULL;
	KG3DRenderState RenderState;
	LPD3DXMESH pRenderMesh = GetRenderSkinMesh();

    KG_PROCESS_ERROR(pMaterial);
	KG_PROCESS_ERROR(pRenderMesh);
	KG_PROCESS_ERROR(pmatBones);
	KG_PROCESS_ERROR(pSkinShader);
	KG_PROCESS_ERROR(m_pBoneCombinationTable);
	KG_PROCESS_ERROR(m_pModel);
	KG_PROCESS_ERROR(pMaterial->IsLoaded());

	{
		D3DXMATRIX matView;
		D3DXMATRIX matProj;
		D3DXMATRIX matWorld;
		D3DXMATRIX matViewProj;
		matWorld = m_pModel->m_matWorld;
		g_pd3dDevice->GetTransform(D3DTS_VIEW, &matView);
		g_pd3dDevice->GetTransform(D3DTS_PROJECTION, &matProj);

		matViewProj = matWorld * matView * matProj;
		hrRetCode = pSkinShader->SetInt("nNumBlend", m_dwMaxFaceInfl);
		KG_COM_PROCESS_ERROR(hrRetCode);

		hrRetCode = pSkinShader->SetMatrix("WorldViewProj", &matViewProj);
		KG_COM_PROCESS_ERROR(hrRetCode);

		pSkinShader->SetInt("nIndex", m_pModel->GetSelectIndex());
	}

	RenderState.SetRenderState(D3DRS_FOGENABLE, FALSE);
	RenderState.SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	RenderState.SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);	

	pBoneComb = static_cast<D3DXBONECOMBINATION*>(m_pBoneCombinationTable->GetBufferPointer());
	KG_PROCESS_ERROR(pBoneComb);

	hrRetCode = pSkinShader->BeginPass(0);
	KG_COM_PROCESS_ERROR(hrRetCode);	
		
	for (DWORD i = 0; i < m_dwNumAttributeGroups; i++)
	{
		if (pBoneComb[i].AttribId >= pMaterial->m_dwNumMaterials)
			continue;

		KG3DMaterial::KG3DMaterialSubset* pSubMat = pMaterial->GetSubMaterial(pBoneComb[i].AttribId);
		KGLOG_PROCESS_ERROR(pSubMat);

		_MtlAlphaOperation* pAlphaOp = pSubMat->GetAlphaOperation();
		BOOL bEnableAlphaFactor = !(pAlphaOp->EnableAlphaTest || pAlphaOp->EnableAlphaBlend);

		_MtlRenderTwiceAlphaOption *pTwiceOption = pSubMat->GetAlphaRenderTwice();
		if (pTwiceOption->bEnable)
		{
			bEnableAlphaFactor = FALSE;
		}

		DWORD dwCullMode = D3DCULL_NONE;
		if (pAlphaOp->EnableAlphaTest)
		{
			RenderState.SetRenderState(D3DRS_ALPHAREF, pAlphaOp->BackAlphaTestRef);
		}
		if (m_dwOption & MATERIAL_OPTION_CULL_CCW)
		{
			dwCullMode = D3DCULL_CCW;
		}
		else if (m_dwOption & MATERIAL_OPTION_CULL_CW)
		{
			dwCullMode = D3DCULL_CW;
		}
		RenderState.SetRenderState(D3DRS_CULLMODE, dwCullMode);
		pSkinShader->SetBool("bAlphaUseFactor", bEnableAlphaFactor);
		
		int nNumBone = min(KG3DGraphicsTool::m_nMaxNumBone, static_cast<int>(m_dwNumBone));
		if (dwOption & RENDER_BIP)
		{
			nNumBone = min(nNumBone, (int)(m_pModel->m_Mesh2Bip.size()));
			for (int k = 0; k < nNumBone; k++)
			{
				DWORD dwBoneIndex = pBoneComb[i].BoneId[k];
				if (dwBoneIndex != UINT_MAX)
				{
					int nMapedBoneIndex = m_pModel->m_Mesh2Bip[dwBoneIndex];
					if (nMapedBoneIndex != -1)
					{
						matTemp[k] = pmatBones[nMapedBoneIndex];
					}
					else
					{
						matTemp[k] = m_pModel->m_pBoneMatricesForRender[dwBoneIndex];
					}
				}
				else
				{
					matTemp[k] = mat;
				}
			}
		}
		else
		{
			for (int k = 0; k < nNumBone; k++)
			{
				DWORD dwBoneIndex = pBoneComb[i].BoneId[k];
				if (dwBoneIndex != UINT_MAX)
				{
					matTemp[k] = pmatBones[dwBoneIndex];
				}
				else
				{
					matTemp[k] = mat;
				}
			}
		}
		hrRetCode = pSkinShader->SetMatrixArray("matBones", matTemp, nNumBone);
		if (SUCCEEDED(hrRetCode))
		{
			//KG3DTexture* pTexture = NULL;
			//g_cTextureTable.GetResource(&pTexture, pSubMat->m_dwTextureIDs[0]);
			//if (pTexture)
			{
				//g_pd3dDevice->SetTexture(0, pTexture->GetTexture());
			}
			//SAFE_RELEASE(pTexture);
			pSkinShader->CommitChanges();
			hrRetCode = pRenderMesh->DrawSubset(i);
		}
	}
	hrResult = S_OK;
Exit0:
	if (pSkinShader)
	{
		hrRetCode = pSkinShader->EndPass();
	}
	return hrResult;
}

HRESULT KG3DMesh::CheckAndLoadBspFile(_MeshHead& MeshHead)
{
	HRESULT hr = E_FAIL;
	TCHAR strDriver[MAX_PATH];
	TCHAR strPath[MAX_PATH];
	TCHAR strFile[MAX_PATH];
	TCHAR strBSPPathName[MAX_PATH];

	_splitpath_s(m_scName.c_str(), 
		strDriver,
		MAX_PATH,
		strPath,
		MAX_PATH,
		strFile,
		MAX_PATH,
		NULL,
		0);

	sprintf_s(strBSPPathName,
		MAX_PATH,
		"%s%s%s.bsp",
		strDriver,
		strPath,
		strFile);


	if (!g_bForceGenerateBspForMesh && g_IsFileExist(strBSPPathName))
	{
		if(!m_lpBsp)
		{
			m_lpBsp = new KG3DBsp;
		}
		KGLOG_PROCESS_ERROR(m_lpBsp);

		hr = m_lpBsp->LoadFromFile(strBSPPathName);
		KGLOG_COM_PROCESS_ERROR(hr);

        if (m_lpBsp->m_dwNumVertex != MeshHead.dwNumVertices || m_lpBsp->m_dwTriangles != MeshHead.dwNumFaces)
        {
            KGLogPrintf(KGLOG_ERR, "%s, BSP unmatch Mesh :(", m_scName.c_str());
            KG_PROCESS_ERROR(FALSE);
        }
	}
	else if(g_bForceGenerateBspForMesh && MeshHead.dwNumFaces >= 256)
	{
        if (!g_IsFileExist(strBSPPathName))
        {
            KGLogPrintf(KGLOG_ERR, "Face >= 256 没有创建 %s",strBSPPathName);
        }

        sprintf_s(strBSPPathName,
            MAX_PATH,
            "%s%s%s_test.bsp",
            strDriver,
            strPath,
            strFile);

		if(!m_lpBsp)
		{
			m_lpBsp = new KG3DBsp;
		}
		KGLOG_PROCESS_ERROR(m_lpBsp);

		DWORD dwStartTime = timeGetTime();

		hr = m_lpBsp->CreateFromMesh(MeshHead.dwNumVertices,MeshHead.dwNumFaces,m_MeshData.pPos,m_MeshData.pFaceIndices);
		KGLOG_COM_PROCESS_ERROR(hr);

		hr = m_lpBsp->SaveToFile(strBSPPathName);
		KGLOG_COM_PROCESS_ERROR(hr);

		DWORD dwCost = timeGetTime() - dwStartTime;
		if(dwCost > 500)
		{
			KGLogPrintf(KGLOG_WARNING, "BSP %d %d Face %s",dwCost,MeshHead.dwNumFaces,
				strBSPPathName);
		}
	}

	hr = S_OK;
Exit0:
	if(FAILED(hr))
	{
		SAFE_DELETE(m_lpBsp);
		KGLogPrintf(KGLOG_ERR, "%s 创建失败",strBSPPathName);
	}
	return hr;

}

HRESULT KG3DMesh::SavePrograssMeshAsLowMesh(LPSTR pName)
{
	HRESULT hr = E_FAIL;
	KG3DMeshLodGenerater* pGenerater = new KG3DMeshLodGenerater(m_dwNumBone,this);
	KGLOG_PROCESS_ERROR(m_pMeshProgress);

	hr = pGenerater->SavePrograssMeshAsLowMesh(m_pMeshProgress,pName);

Exit0:
	SAFE_DELETE(pGenerater);
	return hr;
	
}

HRESULT KG3DMesh::ReadInMeshData::Init(_MeshHead& MeshHead,KG3DMeshStaticData* pStaticData)
{
	KGLOG_PROCESS_ERROR(pStaticData);

	dwNumVertices = MeshHead.dwNumVertices;
	dwNumFaces = MeshHead.dwNumFaces;
	dwNumSubset = MeshHead.dwNumSubset;

	for (size_t i = 0; i < KG3DMESH_READ_BLOCK_PARAMETER; ++i)
	{
		if (MeshHead.dwBlocks[i])
		{
			dwMeshFVF |=  pStaticData->ReadBlockParameter[i].dwFVFAdd;
			dwMeshFVF &= ~pStaticData->ReadBlockParameter[i].dwFVFRemove;			
		}
	}

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DMesh::ReadInMeshData::CreateFromKG3DMeshStaticData(KG3DMeshStaticData* pStaticData)
{
	KGLOG_PROCESS_ERROR(pStaticData);

	SAFE_DELETE_ARRAY(pPos);
	SAFE_DELETE_ARRAY(pNormals);
	SAFE_DELETE_ARRAY(pDiffuse);
	SAFE_DELETE_ARRAY(pUV1);
	SAFE_DELETE_ARRAY(pUV2);
	SAFE_DELETE_ARRAY(pUV3);
	SAFE_DELETE_ARRAY(pFaceIndices);
	SAFE_DELETE_ARRAY(pSubsetIndices);

	pPos     = new D3DXVECTOR3[dwNumVertices];
	KGLOG_PROCESS_ERROR(pPos);
	pNormals = new D3DXVECTOR3[dwNumVertices];
	KGLOG_PROCESS_ERROR(pNormals);
	pDiffuse = new D3DCOLOR   [dwNumVertices];
	KGLOG_PROCESS_ERROR(pDiffuse);

	pUV1     = new D3DXVECTOR3[dwNumVertices];
	KGLOG_PROCESS_ERROR(pUV1);
	pUV2     = new D3DXVECTOR3[dwNumVertices];
	KGLOG_PROCESS_ERROR(pUV2);
	pUV3     = new D3DXVECTOR3[dwNumVertices];
	KGLOG_PROCESS_ERROR(pUV3);

	pFaceIndices   = new DWORD[dwNumFaces*3];
	KGLOG_PROCESS_ERROR(pFaceIndices);
	pSubsetIndices = new DWORD[dwNumFaces];
	KGLOG_PROCESS_ERROR(pSubsetIndices);

	memcpy(pPos,    pStaticData->vPositionBuffer,sizeof(D3DXVECTOR3)*dwNumVertices);
	memcpy(pNormals,pStaticData->vNormalBuffer  ,sizeof(D3DXVECTOR3)*dwNumVertices);
	memcpy(pDiffuse,pStaticData->DiffuseBuffer  ,sizeof(D3DCOLOR   )*dwNumVertices);

	memcpy(pUV1,    pStaticData->vTextureUVW1Buffer,sizeof(D3DXVECTOR3)*dwNumVertices);
	memcpy(pUV2,    pStaticData->vTextureUVW2Buffer,sizeof(D3DXVECTOR3)*dwNumVertices);
	memcpy(pUV3,    pStaticData->vTextureUVW3Buffer,sizeof(D3DXVECTOR3)*dwNumVertices);

	memcpy(pFaceIndices  ,pStaticData->dwFaceIndicesBuffer,sizeof(DWORD)*dwNumFaces*3);
	memcpy(pSubsetIndices,pStaticData->dwSubsetIndices    ,sizeof(DWORD)*dwNumFaces);



	return S_OK;
Exit0:
	return E_FAIL;
}

KG3DMesh::ReadInMeshData::~ReadInMeshData()
{
	UnInit();
}

HRESULT KG3DMesh::ReadInMeshData::UnInit()
{
	SAFE_DELETE_ARRAY(pPos);
	SAFE_DELETE_ARRAY(pNormals);
	SAFE_DELETE_ARRAY(pDiffuse);
	SAFE_DELETE_ARRAY(pUV1);
	SAFE_DELETE_ARRAY(pUV2);
	SAFE_DELETE_ARRAY(pUV3);
	SAFE_DELETE_ARRAY(pFaceIndices);
	SAFE_DELETE_ARRAY(pSubsetIndices);

	dwNumFaces = 0;
	dwNumSubset = 0;
	dwNumVertices = 0;

	return S_OK;
}


